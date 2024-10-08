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




