#include "napi/native_api.h"
#include <string.h>
#include <vector>

extern "C" {
#include "http-proxy-api.h"
}

#include <node_api.h>

#define NUM_DOMAINS 16

struct HttpdnsCallbackContext {
    napi_env env;
    napi_ref callback_ref;
};

static HttpdnsCallbackContext* context = NULL;

static void HttpdnsResultCallback(const char* try_domain, const char* succ_ip) {
    if (!context) return;
    napi_env env = context->env;
    napi_ref callback_ref = context->callback_ref;

    napi_value global, callback;
    napi_get_global(env, &global);
    napi_get_reference_value(env, callback_ref, &callback);

    napi_value args[2];
    napi_create_string_utf8(env, try_domain, NAPI_AUTO_LENGTH, &args[0]);
    napi_create_string_utf8(env, succ_ip, NAPI_AUTO_LENGTH, &args[1]);

    napi_value result;
    napi_call_function(env, global, callback, 2, args, &result);
}

static void CleanUpConfig(video_cache_config& config, uint32_t num_domains) {
    for (uint32_t i = 0; i < num_domains; ++i) {
        for (int j = 0; j < config.ips_ctx[i].num_ips; ++j) {
            delete[] config.ips_ctx[i].ips[j];
        }
        delete[] config.ips_ctx[i].ips;
    }
}

static napi_value SetCacheConfig(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1];
    napi_value global;
    napi_get_global(env, &global);

    napi_get_cb_info(env, info, &argc, args, NULL, NULL);
    napi_value obj = args[0];

    video_cache_config config;
    memset(&config, 0, sizeof(config));

    napi_value cache_root_dir, policy, max_cache_capacity, upstream_timeout_seconds, num_domains, ips_ctx_array;

    napi_get_named_property(env, obj, "cacheRootDir", &cache_root_dir);
    napi_get_named_property(env, obj, "policy", &policy);
    napi_get_named_property(env, obj, "maxCacheCapacity", &max_cache_capacity);
    napi_get_named_property(env, obj, "upstreamTimeoutSeconds", &upstream_timeout_seconds);
    napi_get_named_property(env, obj, "numDomains", &num_domains);
    napi_get_named_property(env, obj, "ipsCtx", &ips_ctx_array);
    
    size_t length = 0;
    napi_get_value_string_utf8(env, cache_root_dir, nullptr, 0, &length);
    std::unique_ptr<char[]> buffer(new (std::nothrow) char[length + 1]);
    if (!buffer) return nullptr;  // 内存分配失败，返回错误

    napi_get_value_string_utf8(env, cache_root_dir, buffer.get(), length + 1, &length);
    config.cache_root_dir = buffer.get();
    config.cache_root_dir_len = length;
    
    napi_get_value_int32(env, policy, reinterpret_cast<int32_t*>(&config.policy));
    napi_get_value_int64(env, max_cache_capacity, &config.max_cache_capacity);
    napi_get_value_int32(env, upstream_timeout_seconds, reinterpret_cast<int32_t*>(&config.upstream_timeout_seconds));
    napi_get_value_int32(env, num_domains, reinterpret_cast<int32_t*>(&config.num_domains));
    
    uint32_t array_length = 0;
    napi_get_array_length(env, ips_ctx_array, &array_length);
    for (uint32_t i = 0; i < array_length && i < NUM_DOMAINS; ++i) {
        napi_value element;
        napi_get_element(env, ips_ctx_array, i, &element);

        napi_value host, num_ips, ips_array;
        napi_get_named_property(env, element, "host", &host);
        napi_get_named_property(env, element, "numIps", &num_ips);
        napi_get_named_property(env, element, "ips", &ips_array);

        napi_get_value_string_utf8(env, host, config.ips_ctx[i].host, DOMAIN_MAX_LEN, &length);
        napi_get_value_int32(env, num_ips, &config.ips_ctx[i].num_ips);

        uint32_t ips_array_length = 0;
        napi_get_array_length(env, ips_array, &ips_array_length);
        config.ips_ctx[i].ips = new (std::nothrow) char*[ips_array_length];
        if (!config.ips_ctx[i].ips) {
            CleanUpConfig(config, i);
            return nullptr;
        }

        for (uint32_t j = 0; j < ips_array_length; ++j) {
            napi_value ip_element;
            napi_get_element(env, ips_array, j, &ip_element);

            napi_get_value_string_utf8(env, ip_element, nullptr, 0, &length);
            config.ips_ctx[i].ips[j] = new (std::nothrow) char[length + 1];
            if (!config.ips_ctx[i].ips[j]) {
                CleanUpConfig(config, i);
                return nullptr;
            }
            napi_get_value_string_utf8(env, ip_element, config.ips_ctx[i].ips[j], length + 1, &length);
        }
    }
    
    std::unique_ptr<HttpdnsCallbackContext> context(new (std::nothrow) HttpdnsCallbackContext());
    if (!context) {
        CleanUpConfig(config, array_length);
        return nullptr;
    }

    napi_value on_httpdns_result;
    if (napi_get_named_property(env, obj, "onHttpdnsResult", &on_httpdns_result) == napi_ok) {
        napi_create_reference(env, on_httpdns_result, 1, &context->callback_ref);
        context->env = env;
        config.on_httpdns_result = HttpdnsResultCallback;
    } else {
        config.on_httpdns_result = nullptr;
    }
    
    set_cache_config(&config);
    
    if (context->callback_ref) {
        napi_delete_reference(env, context->callback_ref);
    }
    
    CleanUpConfig(config, array_length);
    return global;
}

static napi_value StartCacheProxy(napi_env env, napi_callback_info info) {
    (void)env;
    (void)info;
    napi_value ret = 0;
    start_cache_proxy();
    return ret;
}

static napi_value GetProxyUrl(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[2];
    napi_value output;
    
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    
    if (argc != 2) {
        napi_throw_type_error(env, nullptr, "Two arguments expected");
        return nullptr;
    }
    
    size_t str_len;
    napi_get_value_string_utf8(env, args[0], nullptr, 0, &str_len);
    std::vector<char> input_str(str_len + 1);
    napi_get_value_string_utf8(env, args[0], input_str.data(), str_len + 1, &str_len);
    
    int proxy_url_len = input_str.size() + 256;
    char* proxy_url = new char[proxy_url_len];
    get_proxy_play_url(input_str.data(), input_str.size(), proxy_url, proxy_url_len);
    
    napi_create_string_utf8(env, proxy_url, NAPI_AUTO_LENGTH, &output);
    napi_set_element(env, args[1], 0, output);

    delete[] proxy_url;
    return nullptr;
}

static napi_value CleanCache(napi_env env, napi_callback_info info) {
    (void)env, (void)info;
    clean_cache();
    return nullptr;
}

static napi_value StopCacheProxy(napi_env env, napi_callback_info info) {
    (void)env, (void)info;
    dispose();
    return nullptr;
}

EXTERN_C_START
static napi_value Init(napi_env env, napi_value exports) {
    napi_property_descriptor desc[] = {
        {"set_cache_config", nullptr, SetCacheConfig, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"start_cache_proxy", nullptr, StartCacheProxy, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"get_proxy_url", nullptr, GetProxyUrl, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"clean_cache", nullptr, CleanCache, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"stop_cache_proxy", nullptr, StopCacheProxy, nullptr, nullptr, nullptr, napi_default, nullptr}
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    return exports;
}
EXTERN_C_END

static napi_module demoModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "videocache",
    .nm_priv = ((void*)0),
    .reserved = { 0 },
};

extern "C" __attribute__((constructor)) void RegisterVideoCacheModule(void)
{
    napi_module_register(&demoModule);
}
