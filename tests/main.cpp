#include <iostream>

#include <gtest/gtest.h>

#include "http-proxy-api-test.hpp"

int main(int argc, const char * argv[]) {
    ::testing::AddGlobalTestEnvironment(new CacheGlobalTest);
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
