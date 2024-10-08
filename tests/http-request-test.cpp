#include <gtest/gtest.h>

extern "C" {
#include <event2/http.h>
#include "http-internal.h"
#include "video_cache_proxy/src/http/http.h"
#include "video_cache_proxy/src/http/http-request.h"
#include "video_cache_proxy/src/http/http-handler.h"
}

TEST(HttpRequestTest, StartHttpService) {
    // FIXME:
    int ret = http_request_init();
    EXPECT_EQ(ret, 0);
}

TEST(HttpHandlerTest, HandlerCallback) {
    struct evhttp_request* req = evhttp_request_new(NULL, NULL);
    http_handler_cb(req, NULL);
}
