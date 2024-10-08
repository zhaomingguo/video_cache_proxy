#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

#include <gtest/gtest.h>

extern "C" {
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/buffer_compat.h>
#include <evbuffer-internal.h>
#include <event-internal.h>
#include "video_cache_proxy/src/mux-format/http-hls.h"
#include "video_cache_proxy/src/utils/http-common.h"
}

static struct event_base* base = NULL;
static struct evhttp_connection* conn = NULL;
static struct evhttp_request* req = NULL;
static const char* host = "127.0.0.1";
static uint16_t port = g_http_listen_port;
static struct event* hls_test_ev = NULL;

#define MASTER_HTTPS_M3U8_PATH "/hls/cec5f9c37edd67c4362e0ee2587d0310/cec5f9c37edd67c4362e0ee2587d0310/https__vod.tinman.cn__f0a761c020a871eea2c80764a3fc0102__d228aa530b0a4900b67079127b859ac6.m3u8"
#define MASTER_HTTP_M3U8_PATH "/hls/cec5f9c37edd67c4362e0ee2587d0310/cec5f9c37edd67c4362e0ee2587d0310/http__vod.tinman.cn__f0a761c020a871eea2c80764a3fc0102__d228aa530b0a4900b67079127b859ac6.m3u8"
#define NEST_M3U8 "/hls/cec5f9c37edd67c4362e0ee2587d0310/d2aa8a108c8bdcff4809fc21434aba5c/https__vod.tinman.cn__f0a761c020a871eea2c80764a3fc0102--video__d228aa530b0a4900b67079127b859ac6-719e0e098877a1082796f5e985ff9903-video-ld.m3u8"
#define NEST_TS "/hls/cec5f9c37edd67c4362e0ee2587d0310/d2aa8a108c8bdcff4809fc21434aba5c/https__vod.tinman.cn__f0a761c020a871eea2c80764a3fc0102--video__d228aa530b0a4900b67079127b859ac6-719e0e098877a1082796f5e985ff9903-video-ld-00001.ts"

#define LOCAL_TS "/hls/cec5f9c37edd67c4362e0ee2587d0310/d2aa8a108c8bdcff4809fc21434aba5c/https__vod.tinman.cn__f0a761c020a871eea2c80764a3fc0102--video__d228aa530b0a4900b67079127b859ac6-719e0e098877a1082796f5e985ff9903-video-ld-00001.ts"

class HttpHlsTest : public testing::Test {
protected:
    virtual void SetUp() {
        Init();
        if (base) {
            event_base_free(base);
        }
        base = event_base_new();
        // https://vod.tinman.cn/f0a761c020a871eea2c80764a3fc0102/d228aa530b0a4900b67079127b859ac6.m3u8
        conn = evhttp_connection_base_new(base, NULL, host, port);
        req = evhttp_request_new(OnHttpRequestDone, base);
        evhttp_add_header(req->output_headers, "Host", host);
        evhttp_add_header(req->output_headers, "Connection", "close");
        evhttp_add_header(req->output_headers, "User-Agent", "Cache Proxy Unittest");
    }
    virtual void Teardown() {
        if (base) {
            event_base_free(base);
            base = NULL;
        }
        delete_directory(this_m3u8_path_);
    }
    
protected:
    static void OnStopstreamTimer(evutil_socket_t fd, short event, void *arg) {
        struct event_base* base = (struct event_base*)arg;
        if (conn) {
            evhttp_connection_free(conn);
            conn = NULL;
        }
        if (hls_test_ev) {
            event_del(hls_test_ev);
            event_free(hls_test_ev);
            hls_test_ev = NULL;
        }
        if (base) {
            event_base_loopexit(base, NULL);
        }
    }
    static void OnHttpRequestDone(struct evhttp_request *req, void *arg) {
        struct event_base* base = (struct event_base*)arg;
        OnStopstreamTimer(-1, 0, base);
    }
    void StartStopstreamTimer(int seconds) {
        struct timeval tv = {
            .tv_sec = seconds,
            .tv_usec = 0
        };
        if (hls_test_ev) {
            event_del(hls_test_ev);
            event_free(hls_test_ev);
        }
        hls_test_ev = event_new(base, -1, EV_TIMEOUT | EV_PERSIST, OnStopstreamTimer, base);
        event_add(hls_test_ev, &tv);
    }
protected:
    void Init(){
        if (strlen(work_path_)) {
            return;
        }
        size_t ret = get_current_work_path(work_path_, sizeof(work_path_));
        EXPECT_EQ(ret, 0);
        // create hls directory
        char md5[BUFFER_BYTES_64] = "";
        calculate_md5(src_url_, md5, sizeof(md5));
        strncat(this_m3u8_path_, g_cache_config.cache_root_dir, g_cache_config.cache_root_dir_len);
        strncat(this_m3u8_path_, HLS_SUB_PATH, strlen(HLS_SUB_PATH));
        strncat(this_m3u8_path_, md5, strlen(md5));
        strncat(this_m3u8_path_, "/", strlen("/"));
        strncat(this_m3u8_path_, md5, strlen(md5));
        create_directories(this_m3u8_path_);
    }
    char work_path_[BUFFER_BYTES_1024] = "";
    const char* src_url_ = "https://vod.tinman.cn/f0a761c020a871eea2c80764a3fc0102/d228aa530b0a4900b67079127b859ac6.m3u8";
    const int32_t src_url_len_ = static_cast<int32_t>(strlen(src_url_));
    char this_m3u8_path_[BUFFER_BYTES_512] = "";
};

class HttpHlsParmTest : public ::testing::TestWithParam<const char*> {
};

// FIXME:
TEST_P(HttpHlsParmTest, ExtractAfterHls) {
#if 1
    const char* url = GetParam();
    extract_after_hls(url, (char*)HLS_SUB_PATH, NULL);
#else
    const char* url = "https://vod.tinman.cn/f0a761c020a871eea2c80764a3fc0102/d228aa530b0a4900b67079127b859ac6.m3u8";
    extract_after_hls(url, (char*)HLS_SUB_PATH, NULL);
    url = HLS_SUB_PATH;
    extract_after_hls(url, (char*)HLS_SUB_PATH, NULL);
    url = "/hls/f0a761c020a871eea2c80764a3fc0102";
    extract_after_hls(url, (char*)HLS_SUB_PATH, NULL);
    url = "/hls/f0a761c020a871eea2c80764a3fc0102/f0a761c020a871eea2c80764a3fc0102";
    extract_after_hls(url, (char*)HLS_SUB_PATH, NULL);
    url = "/hls/f0a761c020a871eea2c80764a3fc0102/f0a761c020a871eea2c80764a3fc0102/vod.tinman.cn";
    extract_after_hls(url, (char*)HLS_SUB_PATH, NULL);
    url = "/hls/f0a761c020a871eea2c80764a3fc0102/f0a761c020a871eea2c80764a3fc0102/vod.tinman.cn__";
    extract_after_hls(url, (char*)HLS_SUB_PATH, NULL);
#endif
}

INSTANTIATE_TEST_SUITE_P(HttpHlsParmTests, HttpHlsParmTest,
                         ::testing::Values("https://vod.tinman.cn/f0a761c020a871eea2c80764a3fc0102/d228aa530b0a4900b67079127b859ac6.m3u8"
                                           "/hls/f0a761c020a871eea2c80764a3fc0102",
                                           "/hls/f0a761c020a871eea2c80764a3fc0102/f0a761c020a871eea2c80764a3fc0102",
                                           "/hls/f0a761c020a871eea2c80764a3fc0102/f0a761c020a871eea2c80764a3fc0102/https__vod.tinman.cn",
                                           "/hls/f0a761c020a871eea2c80764a3fc0102/f0a761c020a871eea2c80764a3fc0102/https__vod.tinman.cn__"));

// for hls unittest
TEST_F(HttpHlsTest, GetM3u8) {
    char proxy_url[BUFFER_BYTES_1024] = "";
    int ret = get_proxy_play_url(src_url_, src_url_len_, proxy_url, sizeof(proxy_url));
    EXPECT_EQ(ret, 0);
    
    evhttp_make_request(conn, req, EVHTTP_REQ_GET, MASTER_HTTPS_M3U8_PATH);
    evhttp_connection_set_timeout(req->evcon, 35);
    event_base_dispatch(base);
}

TEST_F(HttpHlsTest, GetLocalM3u8) {
    evhttp_make_request(conn, req, EVHTTP_REQ_GET, MASTER_HTTPS_M3U8_PATH);
    evhttp_connection_set_timeout(req->evcon, 15);
    event_base_dispatch(base);
}

TEST_F(HttpHlsTest, GetNestM3u8) {
    evhttp_add_header(req->output_headers, "Range", "bytes=0-");
    
    evhttp_make_request(conn, req, EVHTTP_REQ_GET, NEST_M3U8);
    evhttp_connection_set_timeout(req->evcon, 10);
    event_base_dispatch(base);
}

TEST_F(HttpHlsTest, GetNestTs) {
    evhttp_add_header(req->output_headers, "Range", "bytes=0-");
    
    evhttp_make_request(conn, req, EVHTTP_REQ_GET, NEST_TS);
    evhttp_connection_set_timeout(req->evcon, 10);
    event_base_dispatch(base);
}

TEST_F(HttpHlsTest, GetLocalNestTs) {
    evhttp_add_header(req->output_headers, "Range", "bytes=0-");
    
    evhttp_make_request(conn, req, EVHTTP_REQ_GET, NEST_TS);
    evhttp_connection_set_timeout(req->evcon, 10);
    event_base_dispatch(base);
}

TEST_F(HttpHlsTest, GetLocalNestTsPartion) {
    evhttp_add_header(req->output_headers, "Range", "bytes=0-");
    
    evhttp_make_request(conn, req, EVHTTP_REQ_GET, NEST_TS);
    evhttp_connection_set_timeout(req->evcon, 1);
    StartStopstreamTimer(1);
    event_base_dispatch(base);
}

TEST_F(HttpHlsTest, GetHttpM3u8) {
    char proxy_url[BUFFER_BYTES_1024] = "";
    int ret = get_proxy_play_url(src_url_, src_url_len_, proxy_url, sizeof(proxy_url));
    EXPECT_EQ(ret, 0);
    
    evhttp_make_request(conn, req, EVHTTP_REQ_GET, MASTER_HTTP_M3U8_PATH);
    evhttp_connection_set_timeout(req->evcon, 35);
    event_base_dispatch(base);
}

TEST_F(HttpHlsTest, OtherSuffix) {
    char cur_path[BUFFER_BYTES_1024] = "";
    strncat(cur_path, work_path_, strlen(work_path_));
    const char* flvpath = "/flv/12345678901234567890123456789012";
    strncat(cur_path, flvpath, strlen(flvpath));
    
    create_directories(cur_path);
    char flv_pth[BUFFER_BYTES_1024] = "";
    strncat(flv_pth, cur_path, strlen(cur_path));
    strncat(flv_pth,"/test.flv", strlen("/test.flv"));
    
    FILE* file_handle = fopen(flv_pth, "w+");
    char data[BUFFER_BYTES_32] = "fsdcvfdfsdgd";
    fwrite(data, 1, strlen(data), file_handle);
    fclose(file_handle);
    
    const char* flv = "/flv/12345678901234567890123456789012/test.flv";
    evhttp_make_request(conn, req, EVHTTP_REQ_GET, flv);
    evhttp_connection_set_timeout(req->evcon, 1);
    StartStopstreamTimer(1);
    event_base_dispatch(base);
    memset(cur_path, 0x0, sizeof(cur_path));
    strncat(cur_path, work_path_, strlen(work_path_));
    flvpath = "/flv";
    strncat(cur_path, flvpath, strlen(flvpath));
    delete_directory(cur_path);
}

TEST_F(HttpHlsTest, PureProxy) {
    g_upstream = "vod.tinman.cn";

    evhttp_add_header(req->output_headers, "Range", "bytes=0-");
    
    evhttp_make_request(conn, req, EVHTTP_REQ_GET, LOCAL_TS);
    evhttp_connection_set_timeout(req->evcon, 10);
    event_base_dispatch(base);
}

TEST_F(HttpHlsTest, AbsoluteUrl) {
    const char* absolute_url = "#EXTM3U" LF
        "#EXT-X-STREAM-INF:PROGRAM-ID=1, BANDWIDTH=1280000,AVERAGE-BANDWIDTH=1000000, RESOLUTION=640x480" LF
        "https://example.com/480p.m3u8" LF
        "#EXT-X-STREAM-INF:PROGRAM-ID=2, BANDWIDTH=2560000,AVERAGE-BANDWIDTH=2000000, RESOLUTION=1280x720" LF
        "https://example.com/720p.m3u8" LF
        "#EXT-X-STREAM-INF:PROGRAM-ID=3, BANDWIDTH=7680000,AVERAGE-BANDWIDTH=6000000, RESOLUTION=1920x1080" LF
        "https://example.com/1080p.m3u8" LF
        "#EXT-X-ENDLIST";

    char cur_path[BUFFER_BYTES_1024] = "";
    strncat(cur_path, work_path_, strlen(work_path_));
    const char* filename = "/absolute_url_m3u8.txt";
    strncat(cur_path, filename, strlen(filename));
    media_ctx ctx;
    ctx.decoded_path = (char*)"/hls/cec5f9c37edd67c4362e0ee2587d0310/cec5f9c37edd67c4362e0ee2587d0310/t.m3u8";
    memcpy(ctx.index_file, cur_path, strlen(cur_path));
    ctx.index_file_handle = fopen(cur_path, "w+");
    struct evbuffer* evb = evbuffer_new();
    evbuffer_add(evb, absolute_url, strlen(absolute_url));
    size_t ret = write_m3u8_file(&ctx, evb);
    EXPECT_TRUE(ret > 0);
    fclose(ctx.index_file_handle);
    evbuffer_free(evb);
    unlink(cur_path);
}
