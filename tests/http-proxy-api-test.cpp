#include "http-proxy-api-test.hpp"

#include <chrono>
#include <thread>

#include <gtest/gtest.h>

extern "C" {
#include "video_cache_proxy/api/http-proxy-api.h"
#include "video_cache_proxy/src/utils/http-common.h"
}

static void on_try_result(const char* try_domain, const char* ip) {
    printf("try_domain=%s,ip=%s\n", try_domain, ip);
}

void CacheGlobalTest::SetUp() {
    dispose();
    const char* src_url = "https://player.alicdn.com/video/aliyunmedia.mp4";
    const int32_t src_url_len = static_cast<int32_t>(strlen(src_url));
    char proxy_url[BUFFER_BYTES_1024] = "";
    int ret = get_proxy_play_url(src_url, src_url_len, proxy_url, sizeof(proxy_url));
    EXPECT_EQ(ret, 1);
    
    char cur_path[BUFFER_BYTES_1024] = "";
    ret = get_current_work_path(cur_path, sizeof(cur_path));
    EXPECT_EQ(ret, 0);
    
    const int64_t len = strlen(cur_path);
    video_cache_config cfg = {
        .cache_root_dir = cur_path,
        .cache_root_dir_len = len,
        .policy = by_lru,
        .max_cache_capacity = 100*1024*1024,
        .upstream_timeout_seconds = 10,
        .num_domains = 1,
    };
    const char* test_domain = "player.alicdn.com";
    const int num_ips = 1;
    for (int i = 0; i < cfg.num_domains; ++i) {
        cfg.ips_ctx[i].num_ips = num_ips;
        memcpy(cfg.ips_ctx[i].host, test_domain, strlen(test_domain));
        cfg.ips_ctx[i].ips = (char**)malloc(sizeof(char*) * num_ips);
        //183.223.43.120
        //const char* test_ip[num_ips] = {"192.168.2.3", "192.168.2.5"};
        const char* test_ip[num_ips] = {"192.168.2.3"};
        for (int j = 0; j < num_ips; ++j) {
            cfg.ips_ctx[i].ips[j] = (char*)malloc(BUFFER_BYTES_16);
            memset(cfg.ips_ctx[i].ips[j], 0x0, BUFFER_BYTES_16);
            memcpy(cfg.ips_ctx[i].ips[j], test_ip[j], strlen(test_ip[j]));
        }
    }
    cfg.on_httpdns_result = on_try_result;
    set_cache_config(&cfg);
    ret = get_proxy_play_url(src_url, src_url_len, proxy_url, sizeof(proxy_url));
    EXPECT_EQ(ret, 1);
    set_cache_config(&cfg);
    for (int i = 0; i < cfg.num_domains; ++i) {
        for (int j = 0; j < num_ips; ++j) {
            free(cfg.ips_ctx[i].ips[j]);
        }
        free(cfg.ips_ctx[i].ips);
    }
    start_cache_proxy();
    usleep(100*1000);
}

void CacheGlobalTest::TearDown() {
    // TODO: Domain number exceeds
    clean_cache();
    clean_cache();
    dispose();
}

TEST(HttpProxyApiTest, GetMp4Url) {
    const char* src_url = "https://player.alicdn.com/video/aliyunmedia.mp4";
    const int32_t src_url_len = static_cast<int32_t>(strlen(src_url));
    char proxy_url[BUFFER_BYTES_1024] = "";
    int ret = get_proxy_play_url(src_url, src_url_len, proxy_url, sizeof(proxy_url));
    EXPECT_EQ(ret, 0);
}

TEST(HttpProxyApiTest, GetM3u8Url) {
    const char* src_url = "https://player.alicdn.com/video/test.m3u8";
    const int32_t src_url_len = static_cast<int32_t>(strlen(src_url));
    char proxy_url[BUFFER_BYTES_1024] = "";
    int ret = get_proxy_play_url(src_url, src_url_len, proxy_url, sizeof(proxy_url));
    EXPECT_EQ(ret, 0);
}

TEST(HttpProxyApiTest, GetHttpUrl) {
    const char* src_url = "http://player.alicdn.com/video/test.flv";
    const int32_t src_url_len = static_cast<int32_t>(strlen(src_url));
    char proxy_url[BUFFER_BYTES_1024] = "";
    int ret = get_proxy_play_url(src_url, src_url_len, proxy_url, sizeof(proxy_url));
    EXPECT_EQ(ret, 1);
}

TEST(HttpProxyApiTest, GetOtherProtocol) {
    const char* src_url = "rtmp://player.alicdn.com/video/stream_name";
    const int32_t src_url_len = static_cast<int32_t>(strlen(src_url));
    char proxy_url[BUFFER_BYTES_1024] = "";
    int ret = get_proxy_play_url(src_url, src_url_len, proxy_url, sizeof(proxy_url));
    EXPECT_EQ(ret, 1);
}

TEST(HttpProxyApiTest, GetOtherUrl) {
    const char* src_url = "https://player.alicdn.com/video/test.flv";
    const int32_t src_url_len = static_cast<int32_t>(strlen(src_url));
    char proxy_url[BUFFER_BYTES_1024] = "";
    int ret = get_proxy_play_url(src_url, src_url_len, proxy_url, sizeof(proxy_url));
    EXPECT_EQ(ret, 1);
}
