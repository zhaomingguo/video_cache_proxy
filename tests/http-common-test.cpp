#include <gtest/gtest.h>

extern "C" {
#include "video_cache_proxy/src/utils/http-common.h"
}

TEST(HttpCommonTest, Max) {
    ssize_t a = 1;
    ssize_t b = 0;
    ssize_t ret = max(a, b);
    EXPECT_EQ(ret, 1);
    ret = max(b, a);
    EXPECT_EQ(ret, 1);
    a = b = 0;
    ret = max(a, b);
    EXPECT_EQ(ret, 0);
}

TEST(HttpCommonTest, GetHttpTime) {
    time_t tm;
    char buf[BUFFER_BYTES_256] = "";
    get_http_time(&tm, buf, sizeof(buf));
}

TEST(HttpCommonTest, GetHttpTimeNull) {
    time_t tm;
    char buf[BUFFER_BYTES_256] = "";
    get_http_time(&tm, buf, sizeof(buf));
}

TEST(HttpCommonTest, GetCurrentPath) {
    char cur_path[BUFFER_BYTES_1024] = "";
    int ret = get_current_work_path(cur_path, sizeof(cur_path));
    EXPECT_EQ(ret, 0);
}

TEST(HttpCommonTest, GetFullname) {
    char cur_path[BUFFER_BYTES_1024] = "";
    int ret = get_current_work_path(cur_path, sizeof(cur_path));
    EXPECT_EQ(ret, 0);
    char whole_path[256] = "";
    get_full_filename(cur_path, whole_path, sizeof(whole_path));
}

TEST(HttpCommonTest, SplitHttpRange) {
    http_range range = {};
    const char* header = "bytes=0-";
    int ret = split_http_range(header, &range);
    EXPECT_EQ(ret, 0);
    header = "xxxx";
    ret = split_http_range(header, &range);
    EXPECT_EQ(ret, 1);
    header = "bytes=0-10988";
    ret = split_http_range(header, &range);
    EXPECT_EQ(ret, 0);
    header = "bytes=0-*";
    ret = split_http_range(header, &range);
    EXPECT_EQ(ret, 0);
    header = "bytes=";
    ret = split_http_range(header, &range);
    EXPECT_EQ(ret, 1);
    header = "bytes=20809984-20839984";
    ret = split_http_range(header, &range);
    EXPECT_EQ(ret, 0);
    header = "";
    ret = split_http_range(header, &range);
    EXPECT_EQ(ret, 1);
}

TEST(HttpCommonTest, IsSurfix) {
    const char* url = "https://host:port/video/req.mp4";
    bool ret = is_suffix(url, MP4_SUFFIX);
    EXPECT_TRUE(ret);
    url = "https://player.alicdn.com/video/hd.m3u8";
    ret = is_suffix(url, M3U8_SUFFIX);
    EXPECT_TRUE(ret);
    url = "https://player.alicdn.com/video/hd.ts";
    ret = is_suffix(url, TS_SUFFIX);
    EXPECT_TRUE(ret);
    url = "mp4";
    ret = is_suffix(url, MP4_SUFFIX);
    EXPECT_FALSE(ret);
//    url = NULL;
//    ret = is_suffix(url, MP4_SUFFIX);
//    EXPECT_FALSE(ret);
}

TEST(HttpCommonTest, CalcMd5) {
    const char* input_str = "fdslgdsgddfaegegdsfdsf";
    char md5[33] = "";
    calculate_md5(input_str,md5, sizeof(md5));
    EXPECT_EQ(strlen(md5), 32);
}

TEST(HttpCommonTest, IsDirectoryExists) {
    char cur_path[BUFFER_BYTES_1024] = "";
    int ret = get_current_work_path(cur_path, sizeof(cur_path));
    EXPECT_EQ(ret, 0);
    ret = is_directory_exists(cur_path);
    EXPECT_TRUE(ret);
    const char* path = "ffdvcdfadf/dff";
    strncat(cur_path, path, strlen(path));
    ret = is_directory_exists(path);
    EXPECT_FALSE(ret);
}


TEST(HttpCommonTest, ExtractIdentifier) {
    const char* url = "https://localhost:8190/mp4/xxxxxxxxx/aliyunmedia.mp4";
    char buf[128] = "";
    int ret = extract_identifier(url, "/mp4/", buf, sizeof(buf));
    EXPECT_EQ(ret, 0);
    url = "https://localhost:8190/hls/zzz/t.m3u8";
    ret = extract_identifier(url, "/hls/", buf, sizeof(buf));
    EXPECT_EQ(ret, 0);
    url = "https://localhost:8190/yyy/req.mp4";
    memset(buf, 0x0, sizeof(buf));
    ret = extract_identifier(url, "/xxxx/", buf, sizeof(buf));
    EXPECT_EQ(ret, -1);
    url = "https://localhost:8190";
    ret = extract_identifier(url, "", buf, sizeof(buf));
    EXPECT_EQ(ret, 0);
    url = "https://localhost:8190/mp4/xxx";
    ret = extract_identifier(url, "/mp4/", buf, sizeof(buf));
    EXPECT_EQ(ret, -1);
    url = "https://localhost:8190/mp4/xxxxxxxxx/req.mp4";
    ret = extract_identifier(url, "/mp4/", buf, 0);
    EXPECT_EQ(ret, -1);
}

TEST(HttpCommonTest, ParseUrl) {
    const char* url = "https://player.alicdn.com/video/req.mp4";
    char host[256] = "";
    char path[512] = "";
    char filename[256] = "";
    parse_url(url, host, path, filename);
    
    url = "video/req.mp4";
    memset(host, 0x0, sizeof(host));
    memset(path, 0x0, sizeof(path));
    memset(filename, 0x0, sizeof(filename));
    parse_url(url, host, path, filename);
    
    url = "test.m3u8";
    memset(host, 0x0, sizeof(host));
    memset(path, 0x0, sizeof(path));
    memset(filename, 0x0, sizeof(filename));
    parse_url(url, host, path, filename);
    
    url = "video/req.mp4";
    memset(host, 0x0, sizeof(host));
    memset(path, 0x0, sizeof(path));
    memset(filename, 0x0, sizeof(filename));
    parse_url(url, host, path, NULL);
}

TEST(HttpCommonTest, CreateDirs) {
    char cur_path[BUFFER_BYTES_1024] = "";
    int ret = get_current_work_path(cur_path, sizeof(cur_path));
    EXPECT_EQ(ret, 0);
    
    char* tmp_path = (char*)malloc(sizeof(cur_path) + BUFFER_BYTES_32);
    memset(tmp_path, 0x0, sizeof(cur_path));
    memcpy(tmp_path, cur_path, sizeof(cur_path));
    
    const char* path = "/mp4";
    strncat(tmp_path, path, strlen(path));
    ret = create_directories(tmp_path);
    EXPECT_EQ(ret, 0);
    
    memset(tmp_path, 0x0, sizeof(cur_path));
    memcpy(tmp_path, cur_path, sizeof(cur_path));
    const char* tmp_dirname = "/mp4/tmp";
    strncat(tmp_path, tmp_dirname, strlen(tmp_dirname));
    ret = create_directories(tmp_path);
    EXPECT_EQ(ret, 0);

    memset(tmp_path, 0x0, sizeof(cur_path));
    memcpy(tmp_path, cur_path, sizeof(cur_path));
    const char* sub_dirpath = "/mp4/1";
    strncat(tmp_path, sub_dirpath, strlen(sub_dirpath));
    ret = create_directories(tmp_path);
    EXPECT_EQ(ret, 0);
    free(tmp_path);
}

TEST(HttpCommonTest, DeleteDirs) {
    char cur_path[BUFFER_BYTES_1024] = "";
    int ret = get_current_work_path(cur_path, sizeof(cur_path));
    EXPECT_EQ(ret, 0);
    
    char* tmp_path = (char*)malloc(sizeof(cur_path) + BUFFER_BYTES_32);
    memset(tmp_path, 0x0, sizeof(cur_path));
    memcpy(tmp_path, cur_path, sizeof(cur_path));
    
    const char* path = "/mp4_tmp";
    strncat(tmp_path, path, strlen(path));
    ret = create_directories(tmp_path);
    EXPECT_EQ(ret, 0);
    ret = delete_directory(tmp_path);
    EXPECT_EQ(ret, 0);
    
    memset(tmp_path, 0x0, sizeof(cur_path));
    memcpy(tmp_path, cur_path, sizeof(cur_path));
    const char* tmp_dirname = "/mp4/tmp1";
    strncat(tmp_path, tmp_dirname, strlen(tmp_dirname));
    ret = create_directories(tmp_path);
    EXPECT_EQ(ret, 0);
    ret = delete_directory(tmp_path);
    EXPECT_EQ(ret, 0);

    memset(tmp_path, 0x0, sizeof(cur_path));
    memcpy(tmp_path, cur_path, sizeof(cur_path));
    const char* sub_dirpath = "/mp4/2";
    strncat(tmp_path, sub_dirpath, strlen(sub_dirpath));
    ret = create_directories(tmp_path);
    EXPECT_EQ(ret, 0);
    ret = delete_directory(tmp_path);
    EXPECT_EQ(ret, 0);
    free(tmp_path);
}
