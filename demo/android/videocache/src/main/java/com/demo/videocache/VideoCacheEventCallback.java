package com.demo.videocache;

import androidx.annotation.NonNull;

public interface VideoCacheEventCallback {
    void onEvent(@NonNull CacheCallbackInfo info);

}
