#include <gtest/gtest.h>

extern "C" {
#include "video_cache_proxy/src/cache/video-cache.h"
#include "video_cache_proxy/src/utils/http-common.h"
}

static void generate_random_string(char *str, size_t length) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    if (length) {
        --length;
        for (size_t n = 0; n < length; n++) {
            int key = rand() % (int)(sizeof(charset) - 1);
            str[n] = charset[key];
        }
        str[length] = '\0';
    }
}

TEST(VideoCacheTest, DeleteFiles) {
    char cur_path[BUFFER_BYTES_1024] = "";
    int ret = get_current_work_path(cur_path, sizeof(cur_path));
    EXPECT_EQ(ret, 0);
    
    // make some data
    size_t src_dir_len = g_cache_config.cache_root_dir_len + BUFFER_BYTES_64;
    char* src_dir = (char*)malloc(src_dir_len);
    memset(src_dir, 0x0, src_dir_len);
    // create mp4 sub dir
    srand((unsigned int)time(NULL));
    char* some_file = (char*)malloc(src_dir_len);
    memset(some_file, 0x0, src_dir_len);
    const size_t length = 10;
    for (int i = 0; i < 250; ++i) {
        char random_string[length] = "";
        generate_random_string(random_string, length);
        char* md5 = (char*)malloc(BUFFER_BYTES_64);
        memset(md5, 0x0, BUFFER_BYTES_64);
        calculate_md5(random_string, md5, sizeof(md5));
        snprintf(src_dir, src_dir_len, "%s/%s/%s/", cur_path, i % 2 ? "mp4" : "hls", md5);
        create_directories(src_dir);
        for (int j = 0; j < 250; ++j) {
            snprintf(some_file, src_dir_len, "%s/%d", src_dir, j);
            FILE* file = fopen(some_file, "w+");
            assert(file);
            for (int k = 0; k < 500; ++k) {
                fwrite(random_string, 1, strlen(random_string), file);
            }
            fclose(file);
        }
    }
    free(src_dir);
    free(some_file);
    try_to_delete_files();
}
