import cacheProxyApi, { VideoCacheConfig } from 'libvideocache.so';

export class VideoCacheApi {
  public constructor() {
  }
  public setCacheConfig(c: VideoCacheConfig): void {
    cacheProxyApi.set_cache_config(c)
  }
  public startCacheProxy(): void {
    cacheProxyApi.start_cache_proxy()
  }
  public getProxyUrl(url:string, proxy_url:string[]): void {
    cacheProxyApi.get_proxy_url(url, proxy_url);
  }
  public cleanCache(): void {
    cacheProxyApi.clean_cache();
  }
  public stopCacheProxy(): void {
    cacheProxyApi.stop_cache_proxy()
  }
}