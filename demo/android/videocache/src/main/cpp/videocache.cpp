#include <jni.h>
#include <string>
#include <sys/stat.h>
#include <sys/errno.h>
#include <sys/unistd.h>
#include <dirent.h>

extern "C" {
#include "http-proxy-api.h"
}

JavaVM *jvm = nullptr;
jobject videoCacheObj = nullptr;
jclass callbackInfoClazz = nullptr;

JNIEnv *get_env()
{
    if (jvm == nullptr) {
        return nullptr;
    }

    JNIEnv *env;
    jint res = jvm->AttachCurrentThread(&env, nullptr);
    if (res != JNI_OK) {
        return nullptr;
    }
    return env;
}

static jstring new_utfstring_safely(JNIEnv *env, char *str)
{
    jstring result;
    if (str != nullptr) {
        result = env->NewStringUTF(str);
    } else {
        result = env->NewStringUTF("");
    }

    return result;
}

static void video_cache_global_callback(cache_callback_info *info)
{
    JNIEnv *env = get_env();
    if (env == nullptr) return;
    if (videoCacheObj == nullptr) return;
    if (callbackInfoClazz == nullptr) return;

    jmethodID constructor = env->GetMethodID(callbackInfoClazz, "<init>",
                                             "(Ljava/lang/String;IILjava/lang/String;Ljava/lang/String;DJ)V");

    jstring proxyUrl = new_utfstring_safely(env, info->proxy_url);

    int code = info->msg->code;
    int level = info->msg->level;
    jstring tag = new_utfstring_safely(env, const_cast<char *>(info->msg->tag));
    jstring content = new_utfstring_safely(env, info->msg->content);
    jdouble value = info->msg->value;
    jlong value1 = info->msg->value1;

    jobject infoJava = env->NewObject(callbackInfoClazz, constructor,
                                      proxyUrl, code, level, tag, content, value, value1);

    jclass objClazz = env->GetObjectClass(videoCacheObj);
    jmethodID objMethodId = env->GetMethodID(objClazz, "onMessageCallback",
                                             "(Lcom/demo/videocache/CacheCallbackInfo;)V");
    env->CallVoidMethod(videoCacheObj, objMethodId, infoJava);
}

extern "C" __attribute__((unused))
JNIEXPORT jint JNICALL init_video_config(JNIEnv* env, jobject obj, jobject config) {
    if (config == nullptr) {
        return JNI_ERR;
    }

    jclass configClass = env->GetObjectClass(config);
    if (configClass == nullptr) {
        return JNI_ERR;
    }

    videoCacheObj = env->NewGlobalRef(obj);

    jfieldID cacheRootDirField = env->GetFieldID(configClass, "cacheRootDir", "Ljava/lang/String;");
    jfieldID policyField = env->GetFieldID(configClass, "policy", "I");
    jfieldID maxCacheCapacityField = env->GetFieldID(configClass, "maxCacheCapacity", "J");
    jfieldID upstreamTimeoutSecondsField = env->GetFieldID(configClass, "upstreamTimeoutSeconds", "S");
    jfieldID numDomainsField = env->GetFieldID(configClass, "numDomains", "I");
    jfieldID domainIpsConfigsField = env->GetFieldID(configClass, "domainIpsConfigs", "[Lcom/demo/videocache/VideoCacheConfig$DomainIpsConfig;");

    if (cacheRootDirField == nullptr || policyField == nullptr || maxCacheCapacityField == nullptr ||
        upstreamTimeoutSecondsField == nullptr || numDomainsField == nullptr || domainIpsConfigsField == nullptr) {
        return JNI_ERR;
    }

    jstring cacheRootDir = (jstring)env->GetObjectField(config, cacheRootDirField);
    if (cacheRootDir == nullptr) {
        return JNI_ERR;
    }

    jint policy = env->GetIntField(config, policyField);
    jlong maxCacheCapacity = env->GetLongField(config, maxCacheCapacityField);
    jshort upstreamTimeoutSeconds = env->GetShortField(config, upstreamTimeoutSecondsField);
    jint numDomains = env->GetIntField(config, numDomainsField);
    jobjectArray domainIpsConfigs = (jobjectArray)env->GetObjectField(config, domainIpsConfigsField);

    if (domainIpsConfigs == nullptr && numDomains > 0) {
        return JNI_ERR;
    }

    video_cache_config cache_config = {};
    const char *nativeCacheRootDir = env->GetStringUTFChars(cacheRootDir, nullptr);
    cache_config.cache_root_dir = strdup(nativeCacheRootDir);
    cache_config.cache_root_dir_len = strlen(nativeCacheRootDir);
    cache_config.policy = (cache_elimination_policy)policy;
    cache_config.max_cache_capacity = maxCacheCapacity;
    cache_config.upstream_timeout_seconds = upstreamTimeoutSeconds;
    cache_config.num_domains = numDomains;
    cache_config.global_callback = video_cache_global_callback;

    for (jsize i = 0; i < numDomains; ++i) {
        jobject domainIpsConfig = env->GetObjectArrayElement(domainIpsConfigs, i);
        if (domainIpsConfig == nullptr) {
            continue;
        }

        jclass domainIpsConfigClass = env->GetObjectClass(domainIpsConfig);
        jfieldID hostField = env->GetFieldID(domainIpsConfigClass, "host", "Ljava/lang/String;");
        jfieldID numIpsField = env->GetFieldID(domainIpsConfigClass, "numIps", "I");
        jfieldID ipsField = env->GetFieldID(domainIpsConfigClass, "ips", "[Ljava/lang/String;");

        jstring host = (jstring)env->GetObjectField(domainIpsConfig, hostField);
        if (host == nullptr) {
            continue;
        }

        jint numIps = env->GetIntField(domainIpsConfig, numIpsField);
        jobjectArray ips = (jobjectArray)env->GetObjectField(domainIpsConfig, ipsField);
        if (ips == nullptr && numIps > 0) {
            continue;
        }

        const char *nativeHost = env->GetStringUTFChars(host, nullptr);
        strncpy(cache_config.ips_ctx[i].host, nativeHost, sizeof(cache_config.ips_ctx[i].host) - 1);
        cache_config.ips_ctx[i].host[sizeof(cache_config.ips_ctx[i].host) - 1] = '\0';

        cache_config.ips_ctx[i].num_ips = numIps;
        cache_config.ips_ctx[i].ips = (char**)malloc(numIps * sizeof(char*));
        if (cache_config.ips_ctx[i].ips == nullptr) {
            env->ReleaseStringUTFChars(host, nativeHost);
            env->DeleteLocalRef(domainIpsConfig);
            continue;
        }

        for (int j = 0; j < numIps; ++j) {
            jstring ip = (jstring)env->GetObjectArrayElement(ips, j);
            if (ip == nullptr) {
                continue;
            }

            const char *nativeIp = env->GetStringUTFChars(ip, nullptr);
            cache_config.ips_ctx[i].ips[j] = strdup(nativeIp);
            env->ReleaseStringUTFChars(ip, nativeIp);
            env->DeleteLocalRef(ip);
        }

        env->ReleaseStringUTFChars(host, nativeHost);
        env->DeleteLocalRef(domainIpsConfig);
    }

    env->ReleaseStringUTFChars(cacheRootDir, nativeCacheRootDir);
    env->DeleteLocalRef(config);

    set_cache_config(&cache_config);

    free(cache_config.cache_root_dir);
    for (int i = 0; i < numDomains; ++i) {
        for (int j = 0; j < cache_config.ips_ctx[i].num_ips; ++j) {
            free(cache_config.ips_ctx[i].ips[j]);
        }
        free(cache_config.ips_ctx[i].ips);
    }

    return JNI_OK;
}

extern "C" __attribute__((unused))
JNIEXPORT jint JNICALL start_cache_service(JNIEnv* env, jobject obj) {
    return start_cache_proxy();
}

extern "C" __attribute__((unused))
JNIEXPORT jint JNICALL get_proxy_url(JNIEnv* env, jobject obj, jstring jtr_src_url, jobject jstr_proxy_url) {
    const char* src_url = env->GetStringUTFChars(jtr_src_url, nullptr);
    if (src_url == nullptr) {
        return -1;
    }

    int src_url_len = strlen(src_url);
    int proxy_url_len = src_url_len + 256;
    char* proxy_url = static_cast<char*>(malloc(proxy_url_len));
    if (proxy_url == nullptr) {
        env->ReleaseStringUTFChars(jtr_src_url, src_url);
        return -1; // Memory allocation failed
    }
    memset(proxy_url, 0x0, proxy_url_len);

    int result = get_proxy_play_url(src_url, src_url_len, proxy_url, proxy_url_len);
    if (result == 0) {
        jstring jResultUrl = env->NewStringUTF(proxy_url);
        if (jResultUrl == nullptr) {
            free(proxy_url);
            env->ReleaseStringUTFChars(jtr_src_url, src_url);
            return -1; // Failed to create jstring
        }

        jclass stringBuilderClass = env->GetObjectClass(jstr_proxy_url);
        if (stringBuilderClass == nullptr) {
            env->DeleteLocalRef(jResultUrl);
            free(proxy_url);
            env->ReleaseStringUTFChars(jtr_src_url, src_url);
            return -1; // Failed to get StringBuilder class
        }

        jmethodID appendMethod = env->GetMethodID(stringBuilderClass, "append", "(Ljava/lang/String;)Ljava/lang/StringBuilder;");
        if (appendMethod == nullptr) {
            env->DeleteLocalRef(jResultUrl);
            env->DeleteLocalRef(stringBuilderClass);
            free(proxy_url);
            env->ReleaseStringUTFChars(jtr_src_url, src_url);
            return -1; // Failed to get append method
        }

        env->CallObjectMethod(jstr_proxy_url, appendMethod, jResultUrl);
        env->DeleteLocalRef(jResultUrl);
        env->DeleteLocalRef(stringBuilderClass);

        if (env->ExceptionCheck()) {
            env->ExceptionClear(); // Clear exception and return an error
            free(proxy_url);
            env->ReleaseStringUTFChars(jtr_src_url, src_url);
            return -1;
        }
    }

    free(proxy_url);
    env->ReleaseStringUTFChars(jtr_src_url, src_url);

    return result;
}

extern "C" __attribute__((unused))
JNIEXPORT void JNICALL clean_video_cache(JNIEnv* env, jobject obj) {
    clean_cache();
}

extern "C" __attribute__((unused))
JNIEXPORT void JNICALL stop_video_cache(JNIEnv* env, jobject obj) {
    dispose();
}

static JNINativeMethod g_methods[] = {
    {"nativeCacheConfig", "(Lcom/demo/videocache/VideoCacheConfig;)I", (void *)init_video_config},
    {"nativeStartCacheProxy", "()I", (void *)start_cache_service},
    {"nativeGetProxyPlayUrl", "(Ljava/lang/String;Ljava/lang/StringBuilder;)I", (void *)get_proxy_url},
    {"nativeCleanCache", "()V", (void *)clean_video_cache},
    {"nativeDispose", "()V", (void *)stop_video_cache}
};

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    jint result = JNI_ERR;

    if (vm->GetEnv((void **)&env, JNI_VERSION_1_6) != JNI_OK) {
        return result;
    }

    jvm = vm;

    jclass clazz = env->FindClass("com/demo/videocache/VideoCache");
    if (clazz == nullptr) {
        return result;
    }

    jclass callbackInfoClass = env->FindClass("com/demo/videocache/CacheCallbackInfo");
    callbackInfoClazz = (jclass) env->NewGlobalRef(callbackInfoClass);

    if (env->RegisterNatives(clazz, g_methods, sizeof(g_methods) / sizeof(g_methods[0])) < 0) {
        return result;
    }
    return JNI_VERSION_1_6;
}

void JNI_OnUnload(JavaVM *vm, void *reserved) {
}