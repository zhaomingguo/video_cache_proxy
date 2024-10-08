#ifndef http_proxy_api_test_hpp
#define http_proxy_api_test_hpp

#include <stdio.h>

#include <gtest/gtest.h>

class CacheGlobalTest : public ::testing::Environment {
protected:
    virtual void SetUp();
    virtual void TearDown();
};

#endif /* http_proxy_api_test_hpp */
