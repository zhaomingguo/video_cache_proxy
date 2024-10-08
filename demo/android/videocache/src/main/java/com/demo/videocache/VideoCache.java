package com.demo.videocache;

import android.os.Handler;
import android.os.Looper;
import android.text.TextUtils;

import androidx.annotation.Keep;
import androidx.annotation.NonNull;


import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import dalvik.annotation.optimization.FastNative;

public class VideoCache {

    static {
        System.loadLibrary("videocache");
    }

    private static final class SingletonHolder {
        private static final VideoCache INSTANCE = new VideoCache();
    }

    private final Handler mainHandler = new Handler(Looper.getMainLooper());
    protected final List<VideoCacheEventCallback> globalCallbacks = new LinkedList<>();
    protected final Map<String, LinkedList<VideoCacheEventCallback>> eventMap = new HashMap<>();

    private VideoCache() {

    }

    public static VideoCache getInstance() {
        return SingletonHolder.INSTANCE;
    }

    public void setCacheConfig(VideoCacheConfig config) {
        int ret = nativeCacheConfig(config);

    }

    public void startCacheProxy() {
        int ret = nativeStartCacheProxy();

    }

    public void addGlobalCallback(@NonNull VideoCacheEventCallback callback) {
        mainHandler.post(() -> {
            if (globalCallbacks.contains(callback)) return;
            globalCallbacks.add(callback);
        });
    }

    public void removeGlobalCallback(@NonNull VideoCacheEventCallback callback) {
        mainHandler.post(() -> {
            globalCallbacks.remove(callback);
        });
    }

    public void addCallback(String proxyUrl, @NonNull VideoCacheEventCallback callback) {
        mainHandler.post(() -> {
            LinkedList<VideoCacheEventCallback> callbacks = eventMap.get(proxyUrl);
            if (callbacks == null) {
                callbacks = new LinkedList<>();
                eventMap.put(proxyUrl, callbacks);
            }
            if (!callbacks.contains(callback)) {
                callbacks.add(callback);
            }
        });
    }

    public void removeCallback(@NonNull String proxyUrl) {
        mainHandler.post(() -> {
            eventMap.remove(proxyUrl);
        });
    }

    public void removeCallback(@NonNull VideoCacheEventCallback callback) {
        mainHandler.post(() -> {
            for (Map.Entry<String, LinkedList<VideoCacheEventCallback>> entry : eventMap.entrySet()) {
                LinkedList<VideoCacheEventCallback> callbacks = entry.getValue();
                callbacks.remove(callback);
            }
        });
    }

    public void removeCallback(@NonNull String proxyUrl, @NonNull VideoCacheEventCallback callback) {
        mainHandler.post(() -> {
            LinkedList<VideoCacheEventCallback> callbacks = eventMap.get(proxyUrl);
            if (callbacks == null || callbacks.isEmpty()) return;
            callbacks.remove(callback);
        });
    }

    public String getProxyPlayUrl(String srcUrl) {
        StringBuilder proxyUrl = new StringBuilder();
        int ret = nativeGetProxyPlayUrl(srcUrl, proxyUrl);
        if (ret == -1) return "";
        return proxyUrl.toString();
    }

    public void cleanCache() {
        nativeCleanCache();
    }

    public void dispose() {
        nativeDispose();
    }

    @Keep
    @FastNative
    protected void onMessageCallback(@NonNull CacheCallbackInfo info) {
        mainHandler.post(() -> {
            dispatchEventInfo(info);
        });
    }

    private void dispatchEventInfo(@NonNull CacheCallbackInfo info) {
        String proxyUrl = info.proxyUrl;

        for (VideoCacheEventCallback callback : globalCallbacks) {
            callback.onEvent(info);
        }

        if (TextUtils.isEmpty(proxyUrl)) {
            // call all
            for (Map.Entry<String, LinkedList<VideoCacheEventCallback>> entry : eventMap.entrySet()) {
                LinkedList<VideoCacheEventCallback> callbacks = entry.getValue();
                if (callbacks != null) {
                    for (VideoCacheEventCallback callback : callbacks) {
                        callback.onEvent(info);
                    }
                }
            }
        } else {
            LinkedList<VideoCacheEventCallback> callbacks = eventMap.get(info.proxyUrl);
            if (callbacks == null) return;
            for (VideoCacheEventCallback callback : callbacks) {
                callback.onEvent(info);
            }
        }
    }

    protected native int nativeCacheConfig(VideoCacheConfig config);

    protected native int nativeStartCacheProxy();

    protected native int nativeGetProxyPlayUrl(String srcUrl, StringBuilder proxyUrl);

    protected native void nativeCleanCache();

    protected native void nativeDispose();
}