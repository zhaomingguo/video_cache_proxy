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

## Android

### 1. Add Dependency
#### 1.1 ndk version

```
21.1.6352462
```

#### 1.2 dependency
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

## Harmony

### 1 sdk version

```
mingo@localhost:~/Library/OpenHarmony/Sdk/12$tree -L 2
.
├── native
│   ├── NOTICE.txt
│   ├── build
│   ├── build-tools
│   ├── llvm
│   ├── nativeapi_syscap_config.json
│   ├── ndk_system_capability.json
│   ├── oh-uni-package.json
│   └── sysroot
└── toolchains
    ├── NOTICE.txt
    ├── ark_disasm
    ├── configcheck
    ├── diff
    ├── hdc
    ├── hnpcli
    ├── id_defined.json
    ├── idl
    ├── lib
    ├── libusb_shared.dylib
    ├── modulecheck
    ├── oh-uni-package.json
    ├── restool
    ├── syscap_tool
    └── syscapcheck
```

### 2 Add dependcy

```
{
  "name": "entry",
  "version": "1.0.0",
  "description": "Please describe the basic information.",
  "main": "",
  "author": "",
  "license": "",
  "dependencies": {
    "libentry.so": "file:./src/main/cpp/types/libentry",
    "@arch/cicadaplayer": "^0.0.24-SNAPSHOT",
    "videocache": "file:../videocache"
  }
}
```

### 3 api

```
import cacheProxyApi, { VideoCacheConfig } from 'libvideocache.so';

// 配置信息
interface MediaCacheConfig {
  cacheRootDir: string, // 缓存目录
  policy: number, // 缓存策略 默认1=按创建时间淘汰
  maxCacheCapacity: number, // 最大缓存大小，单位：byte
  upstreamTimeoutSeconds: number, // 请求网络资源连接超时，单位：秒
}

// 单例模式，多媒体文件缓存服务
export class MediaCacheService {
  // 静态成员，用于保存类的唯一实例
  private static instance: MediaCacheService;

  // 私有构造函数，防止外部实例化
  private constructor() {
    // 可以在此初始化一些内容
  }

  // 静态方法，用于获取类的唯一实例
  public static getInstance(): MediaCacheService {
    if (!MediaCacheService.instance) {
      MediaCacheService.instance = new MediaCacheService();
    }
    return MediaCacheService.instance;
  }

  public setCacheConfig(config: MediaCacheConfig): void {
    const c: VideoCacheConfig = {
      cacheRootDir: config.cacheRootDir, // 缓存目录
      policy: config.policy, // 缓存策略 默认1=按创建时间淘汰
      maxCacheCapacity: config.maxCacheCapacity, // 最大缓存大小，单位：byte
      upstreamTimeoutSeconds: config.upstreamTimeoutSeconds, // 请求网络资源连接超时，单位：秒
      numDomains: 0, // 下面的配置，从0开始多少个有效长度
      ipsCtx: [] // 域名转ip的配置
    };
    cacheProxyApi.set_cache_config(c)
  }

  public startCacheProxy(): void {
    cacheProxyApi.start_cache_proxy()
  }

  public getProxyUrl(url: string): string {
    const proxy_url: string[] = [''];
    cacheProxyApi.get_proxy_url(url, proxy_url);
    return proxy_url[0];
  }

  public cleanCache(): void {
    cacheProxyApi.clean_cache();
  }

  public stopCacheProxy(): void {
    cacheProxyApi.stop_cache_proxy()
  }
}

```

### 4 Usage

```
import { MediaCacheService } from "videocache/src/main/ets/native/MediaCacheService"
import { CicadaPlayer } from '@arch/cicadaplayer';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { common } from '@kit.AbilityKit';
import { media } from '@kit.MediaKit';
import { BusinessError } from '@kit.BasicServicesKit';


@Entry
@Component
struct Index {
  @State message: string = 'Hello World';
  @State poxyUrl: string = '';
  @State showCicadaPlayer: boolean = false;
  @State showSystemPlayer: boolean = false;
  mXComponentController: XComponentController = new XComponentController();
  cicadaPlayer: CicadaPlayer = new CicadaPlayer();
  avPlayer: media.AVPlayer | null = null;
  private context = getContext(this) as common.UIAbilityContext
  cacheRootDirPath = this.context.filesDir

  build() {
    RelativeContainer() {
      Column({ space: 20 }) {
        Row({ space: 20 }) {
          Button('开启缓存服务', { type: ButtonType.Normal, stateEffect: true })
            .onClick(() => {
              MediaCacheService.getInstance()
                .setCacheConfig({
                  cacheRootDir: this.cacheRootDirPath, // 缓存目录
                  policy: 1, // 缓存策略 默认1=LRU
                  maxCacheCapacity: 1024 * 1024 * 1024, // 1024MB
                  upstreamTimeoutSeconds: 10 // ，单位：秒
                })
              // // 2. Start Video Cache
              MediaCacheService.getInstance().startCacheProxy()
            })

          Button('清缓存', { type: ButtonType.Normal, stateEffect: true })
            .onClick(() => {
              MediaCacheService.getInstance().cleanCache()
            })
          Button('kill缓存服务', { type: ButtonType.Normal, stateEffect: true })
            .onClick(() => {
              MediaCacheService.getInstance().stopCacheProxy()
            })
        }

        Row({ space: 20 }) {
          Button('播放-Cicada播放器', { type: ButtonType.Normal, stateEffect: true })
            .onClick(() => {
              this.getProxyUrl();
              this.createCicadaPlayer();
              this.showCicadaPlayer = true;
              this.showSystemPlayer = false;
            })
          Button('播放-系统播放器', { type: ButtonType.Normal, stateEffect: true })
            .onClick(() => {
              this.getProxyUrl();
              this.showCicadaPlayer = false;
              this.showSystemPlayer = true;
            })
        }

        // Cicada播放器
        if (this.showCicadaPlayer && this.poxyUrl != "") {
          XComponent({
            id: "surfaceId",
            type: "surface",
            controller: this.mXComponentController
          })
            .onLoad(() => {
              if (this.cicadaPlayer != null) {
                this.cicadaPlayer.setSurfaceId(this.mXComponentController.getXComponentSurfaceId());
                hilog.info(0x0000, 'hm-media-arkts',
                  'XComponent surfaceId is ' + this.mXComponentController.getXComponentSurfaceId());
              }
            })
            .width("100%")
            .aspectRatio(1.77)
        }

        // 系统播放器
        if (this.showSystemPlayer && this.poxyUrl != "") {
          XComponent({
            id: "surfaceId",
            type: "surface",
            controller: this.mXComponentController
          })
            .onLoad(() => {
              let surfaceId = this.mXComponentController.getXComponentSurfaceId();
              this.createAVPlayer(surfaceId);
              hilog.info(0x0000, 'hm-media-arkts', 'XComponent surfaceId is ' + surfaceId)
            })
            .width("100%")
            .aspectRatio(1.77)
        }
      }
    }
    .height('100%')
    .width('100%')
  }

  getProxyUrl() {
    // 3. Get Video Cache URL
    // https://media.w3.org/2010/05/sintel/trailer.mp4
    this.poxyUrl = MediaCacheService.getInstance()
      .getProxyUrl('https://media.w3.org/2010/05/sintel/trailer.mp4');
    console.info("poxy:" + this.poxyUrl);
  }

  createCicadaPlayer() {
    this.cicadaPlayer.setPositionUpdateListener((positon: number) => {
      hilog.info(0x0000, 'hm-media-arkts', 'setPositionUpdateListener position ' + (positon / 1000.0) + ' ' +
        (this.cicadaPlayer.getDuration() / 1000.0));
    });
    this.cicadaPlayer.setLoadStartListener(() => {
      hilog.info(0x0000, 'hm-media-arkts', 'setLoadStartListener call');
    });
    this.cicadaPlayer.setLoadProgressListener((progress: number) => {
      hilog.info(0x0000, 'hm-media-arkts', 'setLoadProgressListener ' + progress);
    });
    this.cicadaPlayer.setLoadEndListener(() => {
      hilog.info(0x0000, 'hm-media', 'setLoadEndListener call');
    });
    this.cicadaPlayer.setErrorListener((errorCode: number, errorMsg: string) => {
      hilog.info(0x0000, 'hm-media', 'setErrorListener ' + errorCode + " " + errorMsg);
    });
    this.cicadaPlayer.setBufferUpdateListener((positon: number) => {
      hilog.info(0x0000, 'hm-media-arkts', 'setBufferUpdateListener position ' + (positon / 1000.0) + ' ' +
        (this.cicadaPlayer.getDuration() / 1000.0));
    });
    this.cicadaPlayer.setCompleteListener(() => {
      hilog.info(0x0000, 'hm-media', 'setCompleteListener call');
    });
    this.cicadaPlayer.setFboTextureListener((textureId: number) => {
      hilog.info(0x0000, 'hm-media', 'setFboTextureListener call textureId = ' + textureId);
    });
    this.cicadaPlayer.setLogInfoListener((level: number, msg: string, ptr: number, sourceMd5: string) => {
      hilog.info(0x0000, 'hm-media',
        'setLogInfoListener call ' + level + " " + msg + " " + ptr + " " + sourceMd5);
    });
    this.cicadaPlayer.setDataSource(this.poxyUrl);
    this.cicadaPlayer.setAutoPlay(true);
    this.cicadaPlayer.prepare();
  }

  async createAVPlayer(surfaceId?: string) {
    if (this.avPlayer == null) {
      this.avPlayer = await media.createAVPlayer();
    }
    if (this.avPlayer != null) {
      this.avPlayer.url = this.poxyUrl;

      this.avPlayer.on("stateChange", async (state: media.AVPlayerState) => {
        if (this.avPlayer == null) {
          return;
        }
        switch (state) {
          case "idle":
            this.avPlayer.url = this.poxyUrl;
            break;
          case "initialized":
            this.avPlayer.surfaceId = surfaceId;
            this.avPlayer.prepare();
            break;
          case "prepared":
            this.avPlayer.play();
            break;
        }
      })
      this.avPlayer.on("error", (err: BusinessError) => {
        // 播放直播视频时，设置 loop 会报错，而 loop 一定会设置(video_player.dart 中初始化之后会 _applyLooping)，所以屏蔽掉该报错
        // message: Unsupport Capability: The stream is live stream, not support loop
        if (err.code == 801) {
          return;
        }
        this.avPlayer?.reset();
      })
    }

  }
}
```