package com.demo.videocache;

import androidx.annotation.NonNull;

import java.io.Serializable;

public class CacheCallbackInfo implements Serializable {
    public static final int ON_LOG = 0;

    public static final int ON_BIND_SERVER = 1;

    public static final int ON_ACCEPT_CLIENT = 2;

    public static final int ON_QUALITY_OF_ENV = 100;

    public static final int LOG_LEVEL_INFO = 0;

    public static final int LOG_LEVEL_ERROR = 1;

    public final int code;

    public final int level;

    public final String proxyUrl;

    public final String tag;

    public final String content;

    public final double value;

    public final long value1;

    public CacheCallbackInfo(String proxyUrl,
                             int code, int level,
                             String tag, String content, double value, long value1) {
        this.proxyUrl = proxyUrl;
        this.code = code;
        this.level = level;
        this.tag = tag;
        this.content = content;
        this.value = value;
        this.value1 = value1;
    }

    public boolean isLogEvent() {
        return code == ON_LOG;
    }

    public boolean isQoeEvent() {
        return code == ON_QUALITY_OF_ENV;
    }

    @NonNull
    @Override
    public String toString() {
        return "CacheCallbackInfo{" +
                "code=" + code +
                ", level=" + level +
                ", proxyUrl='" + proxyUrl + '\'' +
                ", tag='" + tag + '\'' +
                ", content='" + content + '\'' +
                ", value=" + value +
                ", value1=" + value1 +
                '}';
    }
}
