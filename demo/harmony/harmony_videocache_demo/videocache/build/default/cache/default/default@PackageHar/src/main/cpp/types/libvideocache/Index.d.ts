// 定义 domain_ips_ctx 类型
export interface DomainIpsCtx {
  host: string;                // 对应 char host[DOMAIN_MAX_LEN]
  numIps: number;              // 对应 int num_ips
  ips: string[];               // 对应 char** ips
}

// 定义 video_cache_config 类型
export interface VideoCacheConfig {
  cacheRootDir: string;        // 对应 char* cache_root_dir
  policy: number; // 对应 cache_elimination_policy policy
  maxCacheCapacity: number;    // 对应 int64_t max_cache_capacity
  upstreamTimeoutSeconds: number; // 对应 int16_t upstream_timeout_seconds

  numDomains: number;          // 对应 int16_t num_domains
  ipsCtx: DomainIpsCtx[];      // 对应 struct domain_ips_ctx ips_ctx[NUM_DOMAINS]

  onHttpdnsResult?: (tryDomain: string, succIp: string) => void;
}
export const set_cache_config: (c: VideoCacheConfig) => void;
export const start_cache_proxy: () => void;
export const get_proxy_url: (src_url: string, proxy_url: string[]) => void;
export const clean_cache: () => void;
export const stop_cache_proxy: () => void;
