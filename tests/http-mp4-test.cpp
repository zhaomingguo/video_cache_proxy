#include <gtest/gtest.h>

extern "C" {
#include <event2/event.h>
#include <event-internal.h>
#include "video_cache_proxy/src/mux-format/http-mp4.h"
#include "video_cache_proxy/src/utils/http-common.h"
}

extern const char* g_upstream = NULL;

static struct event_base* base = NULL;
static struct evhttp_connection* conn = NULL;
static struct evhttp_request* req = NULL;
static const char* host = "127.0.0.1";
static short port = g_http_listen_port;
static struct event* mp4_test_ev = NULL;

#define TEST_URL_302 "https://qcspublic-bd2.jojoread.com/vod/paired-reading/av1transcoding/1237082437518436545.mp4?ossUrl=tinman-oss%3A%2F%2Fbs-media-service-vod%2Fpaired-reading%2Fav1transcoding%2F1237082437518436545.mp4"

class HttpMp4Test : public ::testing::Test {
protected:
    // SetUpTestCase
    virtual void SetUp() {
        if (base) {
            event_base_free(base);
        }
        Init();
        base = event_base_new();
        // http://127.0.0.1:8190/mp4/33dc0b79d7ffbf4a363d7d0890edacc1/req.mp4?
        // url=https://player.alicdn.com/video/aliyunmedia.mp4
        conn = evhttp_connection_base_new(base, NULL, host, port);
        req = evhttp_request_new(OnHttpRequestDone, base);
        evhttp_add_header(req->output_headers, "Host", host);
        evhttp_add_header(req->output_headers, "User-Agent", "Cache Proxy Unittest");
    }
    // TeardownTestCase
    virtual void Teardown() {
        // FIXME:
        if (base) {
            event_base_free(base);
            base = NULL;
        }
        delete_directory(test_mp4_path_);
        delete_directory(http_mp4_path_);
    }
    
protected:
    static void OnStopstreamTimer(evutil_socket_t fd, short event, void *arg) {
        struct event_base* base = (struct event_base*)arg;
        if (conn) {
            evhttp_connection_free(conn);
            conn = NULL;
        }
        if (mp4_test_ev) {
            event_del(mp4_test_ev);
            event_free(mp4_test_ev);
            mp4_test_ev = NULL;
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
        if (mp4_test_ev) {
            event_del(mp4_test_ev);
            event_free(mp4_test_ev);
        }
        mp4_test_ev = event_new(base, -1, EV_TIMEOUT | EV_PERSIST, OnStopstreamTimer, base);
        event_add(mp4_test_ev, &tv);
    }
protected:
    void Init() {
        if (strlen(decoded_path_) > 0) {
            return;
        }
        size_t new_url_len = BUFFER_BYTES_1024;
        char* new_url = (char*)malloc(new_url_len);
        memset(new_url, 0x0, new_url_len);
        
        strncat(new_url, "url=", strlen("url="));
        strncat(new_url, src_url_, src_url__len_);
        calculate_md5(new_url, mp4_url_md5_, sizeof(mp4_url_md5_));
        evutil_snprintf(decoded_path_, sizeof(decoded_path_), "/mp4/%s/req.mp4?url=%s", mp4_url_md5_, src_url_);
        strncat(test_mp4_path_, g_cache_config.cache_root_dir, g_cache_config.cache_root_dir_len);
        strncat(test_mp4_path_, MP4_SUB_PATH, strlen(MP4_SUB_PATH));
        strncat(test_mp4_path_, mp4_url_md5_, strlen(mp4_url_md5_));
        create_directories(test_mp4_path_);
        
        memset(new_url, 0x0, new_url_len);
        strncat(new_url, "url=", strlen("url="));
        strncat(new_url, src_http_url_, strlen(src_http_url_));
        char http_md5[BUFFER_BYTES_64] = "";
        calculate_md5(new_url, http_md5, sizeof(http_md5));
        evutil_snprintf(http_decoded_path_, sizeof(http_decoded_path_), "/mp4/%s/req.mp4?url=%s", http_md5, src_http_url_);
        strncat(http_mp4_path_, g_cache_config.cache_root_dir, g_cache_config.cache_root_dir_len);
        strncat(http_mp4_path_, MP4_SUB_PATH, strlen(MP4_SUB_PATH));
        strncat(http_mp4_path_, http_md5, strlen(http_md5));
        create_directories(http_mp4_path_);
        
        memset(new_url, 0x0, new_url_len);
        strncat(new_url, "url=", strlen("url="));
        strncat(new_url, not_found_url_, strlen(not_found_url_));
        char not_found_md5[BUFFER_BYTES_64] = "";
        calculate_md5(new_url, not_found_md5, sizeof(not_found_md5));
        evutil_snprintf(not_found_path_, sizeof(not_found_path_), "/mp4/%s/req.mp4?url=%s", not_found_md5, not_found_url_);
        
        memset(new_url, 0x0, new_url_len);
        strncat(new_url, "url=", strlen("url="));
        strncat(new_url, TEST_URL_302, strlen(TEST_URL_302));
        char md5_302[BUFFER_BYTES_64] = "";
        calculate_md5(new_url, md5_302, sizeof(md5_302));
        evutil_snprintf(url_302_path_, sizeof(url_302_path_), "/mp4/%s/req.mp4?url=%s", md5_302, TEST_URL_302);
        free(new_url);
    }
protected:
    // for https
    const char* src_url_ = "https://player.alicdn.com/video/aliyunmedia.mp4";
    const int32_t src_url__len_ = static_cast<int32_t>(strlen(src_url_));
    char decoded_path_[BUFFER_BYTES_512] = "";
    char test_mp4_path_[BUFFER_BYTES_512] = "";
    // for http only
    const char* src_http_url_ = "http://player.alicdn.com/video/aliyunmedia.mp4";
    const int32_t src_http_url_len_ = static_cast<int32_t>(strlen(src_http_url_));
    char http_decoded_path_[BUFFER_BYTES_512] = "";
    char http_mp4_path_[BUFFER_BYTES_512] = "";
    // for not found
    const char* not_found_url_ = "https://player.alicdn.com/video/not_found.mp4";
    char not_found_path_[BUFFER_BYTES_512] = "";
    char mp4_url_md5_[BUFFER_BYTES_64] = "";
    // for 302 only
    char url_302_path_[BUFFER_BYTES_256] = "";
};

TEST_F(HttpMp4Test, PlayRangeHttpOnly) {
    char proxy_url[BUFFER_BYTES_1024] = "";
    int ret = get_proxy_play_url(src_http_url_, src_http_url_len_, proxy_url, sizeof(proxy_url));
    EXPECT_EQ(ret, 0);
    
    evhttp_add_header(req->output_headers, "Accept", "*/*");
    
    evhttp_make_request(conn, req, EVHTTP_REQ_GET, http_decoded_path_);
    evhttp_connection_set_timeout(req->evcon, 10);
    event_base_dispatch(base);
}

TEST_F(HttpMp4Test, Play302MP4) {
    char proxy_url[BUFFER_BYTES_512] = "";
    int ret = get_proxy_play_url(TEST_URL_302, (int32_t)strlen(TEST_URL_302), proxy_url, sizeof(proxy_url));
    EXPECT_EQ(ret, 0);
    
    evhttp_add_header(req->output_headers, "Accept", "*/*");
    
    evhttp_make_request(conn, req, EVHTTP_REQ_GET, url_302_path_);
    evhttp_connection_set_timeout(req->evcon, 20);
    event_base_dispatch(base);
}

TEST_F(HttpMp4Test, StartPlayRangeClose) {
    char proxy_url[BUFFER_BYTES_1024] = "";
    int ret = get_proxy_play_url(src_url_, src_url__len_, proxy_url, sizeof(proxy_url));
    EXPECT_EQ(ret, 0);
    
    evhttp_add_header(req->output_headers, "Range", "bytes=0-15807084");
    evhttp_add_header(req->output_headers, "Connection", "close");
    
    evhttp_make_request(conn, req, EVHTTP_REQ_GET, decoded_path_);
    evhttp_connection_set_timeout(req->evcon, 7);
    StartStopstreamTimer(3);
    event_base_dispatch(base);
}

TEST_F(HttpMp4Test, StartPlayWithRange) {
    evhttp_add_header(req->output_headers, "Range", "bytes=20807084-20808084");
    evhttp_add_header(req->output_headers, "Connection", "keep-alive");
    
    evhttp_make_request(conn, req, EVHTTP_REQ_GET, decoded_path_);
    evhttp_connection_set_timeout(req->evcon, 30);
    event_base_dispatch(base);
}

TEST_F(HttpMp4Test, StartPlayWithUpstreamOnly) {
    evhttp_add_header(req->output_headers, "Range", "bytes=20807010-20807080");
    evhttp_add_header(req->output_headers, "Connection", "keep-alive");
    
    evhttp_make_request(conn, req, EVHTTP_REQ_GET, decoded_path_);
    evhttp_connection_set_timeout(req->evcon, 30);
    event_base_dispatch(base);
}

TEST_F(HttpMp4Test, StartPlayWithLocal) {
    evhttp_add_header(req->output_headers, "Range", "bytes=20807094-20807184");
    evhttp_add_header(req->output_headers, "Connection", "keep-alive");
    
    evhttp_make_request(conn, req, EVHTTP_REQ_GET, decoded_path_);
    evhttp_connection_set_timeout(req->evcon, 15);
    event_base_dispatch(base);
}

TEST_F(HttpMp4Test, StartPlayRangeWithUpstream) {
    evhttp_add_header(req->output_headers, "Range", "bytes=20807084-20808584");
    evhttp_add_header(req->output_headers, "Connection", "keep-alive");
    
    evhttp_make_request(conn, req, EVHTTP_REQ_GET, decoded_path_);
    evhttp_connection_set_timeout(req->evcon, 30);
    event_base_dispatch(base);
}

TEST_F(HttpMp4Test, StartPlayRangeWithNonContinuousSegment) {
    evhttp_add_header(req->output_headers, "Range", "bytes=20808684-20808884");
    evhttp_add_header(req->output_headers, "Connection", "keep-alive");
    
    evhttp_make_request(conn, req, EVHTTP_REQ_GET, decoded_path_);
    evhttp_connection_set_timeout(req->evcon, 30);
    event_base_dispatch(base);
}

TEST_F(HttpMp4Test, StartPlayRangeWithLocalAndUpstreamAndLocal) {
    evhttp_add_header(req->output_headers, "Range", "bytes=20807084-20808784");
    
    evhttp_make_request(conn, req, EVHTTP_REQ_GET, decoded_path_);
    evhttp_connection_set_timeout(req->evcon, 60);
    event_base_dispatch(base);
}

TEST_F(HttpMp4Test, StartPlayRangeWithClose) {
    evhttp_add_header(req->output_headers, "Range", "bytes=0-");
    evhttp_add_header(req->output_headers, "Connection", "close");
    
    evhttp_make_request(conn, req, EVHTTP_REQ_GET, decoded_path_);
    evhttp_connection_set_timeout(req->evcon, 15);
    StartStopstreamTimer(7);
    event_base_dispatch(base);
}

TEST_F(HttpMp4Test, StartPlayWithAbnormalRange) {
    evhttp_add_header(req->output_headers, "Range", "");
    evhttp_add_header(req->output_headers, "Connection", "close");
    
    evhttp_make_request(conn, req, EVHTTP_REQ_GET, decoded_path_);
    evhttp_connection_set_timeout(req->evcon, 10);
    StartStopstreamTimer(5);
    event_base_dispatch(base);
}

TEST_F(HttpMp4Test, StartPlayWithNotFound) {
    evhttp_add_header(req->output_headers, "Range", "");
    evhttp_add_header(req->output_headers, "Connection", "close");
    g_cache_config.upstream_timeout_seconds = 0;
    evhttp_make_request(conn, req, EVHTTP_REQ_GET, not_found_path_);
    evhttp_connection_set_timeout(req->evcon, 15);
    event_base_dispatch(base);
    g_cache_config.upstream_timeout_seconds = 60;
}

TEST_F(HttpMp4Test, PureProxyNullUpstream) {
    g_upstream = NULL;
    
    evhttp_add_header(req->output_headers, "Range", "bytes=0-");
    evhttp_add_header(req->output_headers, "Connection", "close");
    
    evhttp_make_request(conn, req, EVHTTP_REQ_GET, "/video/aliyunmedia.mp4");
    evhttp_connection_set_timeout(req->evcon, 15);
    StartStopstreamTimer(7);
    event_base_dispatch(base);
}

TEST_F(HttpMp4Test, StartPlayNoRange) {
    delete_directory(test_mp4_path_);
    
    char proxy_url[BUFFER_BYTES_1024] = "";
    int ret = get_proxy_play_url(src_url_, src_url__len_, proxy_url, sizeof(proxy_url));
    EXPECT_EQ(ret, 0);
    
    evhttp_add_header(req->output_headers, "Connection", "close");
    
    evhttp_make_request(conn, req, EVHTTP_REQ_GET, decoded_path_);
    evhttp_connection_set_timeout(req->evcon, 5);
    StartStopstreamTimer(1);
    event_base_dispatch(base);
}

TEST_F(HttpMp4Test, PureProxy) {
    g_upstream = "player.alicdn.com";
    
    evhttp_add_header(req->output_headers, "Range", "bytes=0-");
    evhttp_add_header(req->output_headers, "Connection", "close");
    
    evhttp_make_request(conn, req, EVHTTP_REQ_GET, "/video/aliyunmedia.mp4");
    evhttp_connection_set_timeout(req->evcon, 10);
    event_base_dispatch(base);
}

TEST_F(HttpMp4Test, DominNotConfig) {
    evhttp_add_header(req->output_headers, "Range", "bytes=0-");
    evhttp_add_header(req->output_headers, "Connection", "close");
    g_cache_config.upstream_timeout_seconds = 0;
    evhttp_make_request(conn, req, EVHTTP_REQ_GET,
                        "/mp4/54c6ed9cf16ac2f832fca6659fa31f39/req.mp4?url=https://abc.def.com/video/aliyunmedia.mp4");
    evhttp_connection_set_timeout(req->evcon, 12);
    event_base_dispatch(base);
    g_cache_config.upstream_timeout_seconds = 5;
}

TEST_F(HttpMp4Test, PureProxyNotFound) {
    g_upstream = "player.alicdn.com";
    
    evhttp_add_header(req->output_headers, "Range", "bytes=0-");
    evhttp_add_header(req->output_headers, "Connection", "close");
    g_cache_config.upstream_timeout_seconds = 0;
    evhttp_make_request(conn, req, EVHTTP_REQ_GET, "/video/req.mp4");
    evhttp_connection_set_timeout(req->evcon, 25);
    StartStopstreamTimer(7);
    event_base_dispatch(base);
    g_cache_config.upstream_timeout_seconds = 5;
}
