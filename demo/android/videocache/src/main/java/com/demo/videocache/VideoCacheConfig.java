package com.demo.videocache;

public class VideoCacheConfig {
    private String cacheRootDir;
    private int policy;
    private long maxCacheCapacity;
    private short upstreamTimeoutSeconds;

    private int numDomains;
    private DomainIpsConfig[] domainIpsConfigs;

    public static class DomainIpsConfig {
        private String host;
        private int numIps;
        private String[] ips;

        // Getters and setters
        public String getHost() { return host; }
        public void setHost(String host) { this.host = host; }
        public int getNumIps() { return numIps; }
        public void setNumIps(int numIps) { this.numIps = numIps; }
        public String[] getIps() { return ips; }
        public void setIps(String[] ips) { this.ips = ips; }
    }

    public String getCacheRootDir() { return cacheRootDir; }
    public void setCacheRootDir(String cacheRootDir) { this.cacheRootDir = cacheRootDir; }
    public int getPolicy() { return policy; }
    public void setPolicy(int policy) { this.policy = policy; }
    public long getMaxCacheCapacity() { return maxCacheCapacity; }
    public void setMaxCacheCapacity(long maxCacheCapacity) { this.maxCacheCapacity = maxCacheCapacity; }
    public short getUpstreamTimeoutSeconds() { return upstreamTimeoutSeconds; }
    public void setUpstreamTimeoutSeconds(short upstreamTimeoutSeconds) { this.upstreamTimeoutSeconds = upstreamTimeoutSeconds; }
    public int getNumDomains() { return numDomains; }
    public void setNumDomains(int numDomains) { this.numDomains = numDomains; }
    public DomainIpsConfig[] getDomainIpsConfigs() { return domainIpsConfigs; }
    public void setDomainIpsConfigs(DomainIpsConfig[] domainIpsConfigs) { this.domainIpsConfigs = domainIpsConfigs; }
}