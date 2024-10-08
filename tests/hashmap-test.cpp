#include <gtest/gtest.h>

extern "C" {
#include "video_cache_proxy/src/utils/http-common.h"
}

TEST(HashmapTest, HashmapNew) {
    map_t m = hashmap_new();
    EXPECT_TRUE(m != NULL);
}

TEST(HashmapTest, HashmapPutOK) {
    map_t m = hashmap_new();
    EXPECT_TRUE(m != NULL);
    char buf[8] = "";
    for (int i = 0; i < 100; ++i) {
        snprintf(buf, sizeof(buf), "%d", i);
        int ret = hashmap_put(m, buf, (any_t)"1");
        EXPECT_TRUE(ret == MAP_OK);
    }
}

TEST(HashmapTest, HashmapPutNull) {
    map_t m = NULL;
    int ret = hashmap_length(m);
    EXPECT_TRUE(ret == 0);
    ret = hashmap_put(m, (char*)"test", (any_t)"1");
    EXPECT_TRUE(ret == MAP_OK);
}

TEST(HashmapTest, HashmapPutFull) {
    map_t m = hashmap_new();
    EXPECT_TRUE(m != NULL);
    int ret = 0;
    char buf[8] = "";
    for (int i = 0; i < 256; ++i) {
        snprintf(buf, sizeof(buf), "%d", i);
        ret = hashmap_put(m, buf, (any_t)"1");
        EXPECT_TRUE(ret == MAP_OK);
    }
    ret = hashmap_put(m, (char*)"full", (any_t)"1");
    EXPECT_TRUE(ret == MAP_OK);
}

TEST(HashmapTest, HashmapGet) {
    map_t m = hashmap_new();
    EXPECT_TRUE(m != NULL);
    char* key = (char*)malloc(5);
    memset(key, 0x0, 5);
    memcpy(key, "full", 4);
    int ret = hashmap_put(m, key, (any_t)"1");
    EXPECT_TRUE(ret == MAP_OK);
    ret = hashmap_length(m);
    EXPECT_TRUE(ret == 1);
    memcpy(key, "test", 4);
    any_t* value = (any_t*)malloc(8);
    memset(value, 0x0, 8);
    ret = hashmap_get(m, key, value);
    EXPECT_TRUE(ret == MAP_MISSING);
    ret = hashmap_length(m);
    EXPECT_TRUE(ret == 1);
    memcpy(key, "full", 4);
    ret = hashmap_get(m, key, value);
    EXPECT_TRUE(ret == MAP_OK);
    free(value);
    hashmap_free(m);
}

static int test_hashmap_iterater(any_t a, any_t b) {
    return a > b ? MAP_OK : 1;
}
// hashmap_iterate
// hashmap_hash_int
TEST(HashmapTest, HashmapIter) {
    map_t m = hashmap_new();
    EXPECT_TRUE(m != NULL);
    int ret = hashmap_iterate(m, test_hashmap_iterater, NULL);
    EXPECT_TRUE(ret == MAP_MISSING);
    ret = hashmap_put(m, (char*)"full", (any_t)"1");
    EXPECT_TRUE(ret == MAP_OK);
    ret = hashmap_iterate(m, test_hashmap_iterater, NULL);
    EXPECT_TRUE(ret == MAP_OK);
}

TEST(HashmapTest, HashmapRemove) {
    map_t m = hashmap_new();
    EXPECT_TRUE(m != NULL);
    int ret = hashmap_remove(m, (char*)"test");
    EXPECT_TRUE(ret == MAP_MISSING);
    ret = hashmap_length(m);
    EXPECT_TRUE(ret == 0);
    char* ptest = (char*)malloc(5);
    memset(ptest, 0x0, 5);
    memcpy(ptest, "test", 4);
    ret = hashmap_put(m, ptest, (any_t)"1");
    EXPECT_TRUE(ret == MAP_OK);
    ret = hashmap_length(m);
    EXPECT_TRUE(ret == 1);
    ret = hashmap_remove(m, ptest);
    EXPECT_TRUE(ret == MAP_OK);
    char* p = (char*)malloc(3);
    memset(p, 0x0, 3);
    memcpy(p, "12", 2);
    ret = hashmap_remove(m, p);
    EXPECT_TRUE(ret == MAP_MISSING);
}
