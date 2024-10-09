# Mobile Video Caching Library

This library is a video caching library designed for mobile platforms (iOS/Android/HarmonyOS), based on an HTTP caching proxy implemented with `libevent`. It currently supports caching for MP4 and HLS videos, enhancing playback smoothness and reducing load times.

## Features

1. **Cross-Platform Support**: Supports video caching on Android, iOS, and HarmonyOS.
2. **Video Format Support**: Supports caching of MP4 and HLS video formats.
3. **HTTP Caching Proxy**: A high-performance HTTP caching proxy built with `libevent`.
4. **HTTP 302 Redirection Support**: Handles HTTP 302 redirects to ensure proper video resource access.
5. **Domain IP Pool Support**: Configurable IP pools for domains, offering flexible resource access.
6. **Cache Management**: Supports customizable cache eviction mechanisms.
7. **LRU Eviction Strategy**: Implements an LRU (Least Recently Used) cache eviction strategy to ensure cache efficiency.

## Android Integration

### 1. Add Dependency

Add the following line to your `build.gradle` file to include the video caching library:

```
implementation 'cn.demo.android.third_party:VideoCache:0.0.22'
```

### 2. Caching Library Interface Class

```
import androidx.annotation.NonNull;
import com.jojo.videocache.CacheCallbackInfo;
import com.jojo.videocache.VideoCache;
import com.jojo.videocache.VideoCacheConfig;
import com.jojo.videocache.VideoCacheEventCallback;
import java.net.InetAddress;
import java.util.List;

public class GeneralCacheProxy {
    private final VideoCache videoCache = VideoCache.getInstance();

    public void initWithConfig(VideoCacheConfig config) {
        // Set configuration for the caching library
        videoCache.setCacheConfig(config);
        videoCache.addGlobalCallback(info -> {
            if (info.isLogEvent()) {
                if (info.level == CacheCallbackInfo.LOG_LEVEL_INFO) { // info
                    Logger.INSTANCE.i(info.tag, "", info.content);
                } else if (info.level == CacheCallbackInfo.LOG_LEVEL_ERROR) { // error
                    Logger.INSTANCE.e(info.tag, "", info.content);
                }
            }
        });
        // Start the caching library
        videoCache.startCacheProxy();
    }

    public void updateConfig(String host, List<InetAddress> iplist) {
        if (iplist == null || iplist.isEmpty()) return;

        VideoCacheConfig videoCacheConfig = new VideoCacheConfig();
        VideoCacheConfig.DomainIpsConfig[] domainIpsConfigs = new VideoCacheConfig.DomainIpsConfig[1];

        domainIpsConfigs[0] = new VideoCacheConfig.DomainIpsConfig();
        domainIpsConfigs[0].setHost(host);
        domainIpsConfigs[0].setNumIps(iplist.size());
        String[] ips = new String[iplist.size()];
        for (int i = 0; i < iplist.size(); i++) {
            ips[i] = iplist.get(i).getHostAddress();
        }
        domainIpsConfigs[0].setIps(ips);

        videoCacheConfig.setDomainIpsConfigs(domainIpsConfigs);
        // Update dynamic configurations for the cache
        videoCache.setCacheConfig(videoCacheConfig);
    }

    public String getProxyUrl(String srcUrl) {
        return videoCache.getProxyPlayUrl(srcUrl);
    }

    public void addCallback(String proxyUrl, @NonNull VideoCacheEventCallback callback) {
        videoCache.addCallback(proxyUrl, callback);
    }

    public void removeCallback(VideoCacheEventCallback callback) {
        videoCache.removeCallback(callback);
    }

    public void cleanCache() {
        // Clean all cached content
        videoCache.cleanCache();
    }

    public void dispose() {
        // Dispose of the cache proxy library
        videoCache.dispose();
    }
}
```

### 3. Video Cache Manager

```
import android.annotation.SuppressLint
import android.content.Context
import java.io.File

object VideoCacheManager {

    @SuppressLint("StaticFieldLeak")
    private var videoCacheProxy: GeneralCacheProxy? = null

    @Synchronized
    fun initGeneralCacheProxy(config: VideoCacheConfig?) {
        if (videoCacheProxy != null) {
            return
        }
        videoCacheProxy = GeneralCacheProxy()
        videoCacheProxy?.initWithConfig(config)
    }

    @Synchronized
    fun getGeneralCacheProxy(): GeneralCacheProxy? {
        return videoCacheProxy
    }
}
```

### 4. Usage

#### 4.1 Initialize and Start Caching

```
if (VideoCacheManager.getGeneralCacheProxy() == null) {
    val cacheDir = cacheConfig.dir + File.separator + GENERAL_CACHE_DIR
    createDir(cacheDir)

    val config = VideoCacheConfig().apply {
        cacheRootDir = cacheDir // Cache directory
        policy = 1 // Cache eviction strategy, default is 1 = LRU
        maxCacheCapacity = cacheConfig.maxCacheSize.toLong() * 1024 * 1024 // Cache size in bytes
        upstreamTimeoutSeconds = 15.toShort() // 15 seconds connection timeout
        numDomains = 0 // Use libevent DNS resolution
    }

    VideoCacheManager.initGeneralCacheProxy(config)
}
```

#### 4.2 Get Proxy URL for Video

```
val proxy: GeneralCacheProxy? = VideoCacheManager.getGeneralCacheProxy()
val proxyUrl = proxy?.getProxyUrl(url) ?: ""
val uri = Uri.parse(url)
val inetAddressList: List<InetAddress>? = uri.host?.let { host ->
    mDnsLookupListener?.lookup(host)
}

if (!inetAddressList.isNullOrEmpty()) {
    // Custom HTTP headers for the player, such as PlayerId to address seek issues
    proxy?.updateConfig(uri.host, inetAddressList)
    config?.customHeaders = arrayOf("PlayerId: $uniKey")
}
```


#### 4.3 Clean Cache

```
cleanCache()
```

#### 4.4 Dispose of the Cache Library

```
dispose()
```
