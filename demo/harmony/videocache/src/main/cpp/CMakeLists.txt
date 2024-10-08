# the minimum version of CMake.
cmake_minimum_required(VERSION 3.5.0)
project(videocache)

set(CMAKE_C_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(DEFINED PACKAGE_FIND_FILE)
    include(${PACKAGE_FIND_FILE})
endif()

# 配置第三方库目录
set(LIBS_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../libs)
message("ANDROID_ABI=${ANDROID_ABI}")
message("CMake Arguments: ${CMAKE_ARGS}")
message("LIBS_ROOT_DIR: ${LIBS_ROOT_DIR}")

# 以源码的方式添加缓存库
set(VIDEO_CACHE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../video_cache_proxy)
# 添加缓存库头文件
include_directories(${VIDEO_CACHE_ROOT}/api)

add_subdirectory(${VIDEO_CACHE_ROOT} ${CMAKE_CURRENT_BINARY_DIR}/video_cache_proxy)

add_library(videocache SHARED napi_init.cpp)

# 缓存库依赖的静态库需要链接
set(EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../3rdparty)
set(OPENSSL_ROOT_DIR "${EXTERNAL_DIR}/openssl/harmony/${OHOS_ARCH}")
message("Victor OPENSSL_ROOT_DIR: ${OPENSSL_ROOT_DIR}")
set(LIBEVENT_ROOT_DIR "${EXTERNAL_DIR}/libevent/harmony/${OHOS_ARCH}")
message("Victor LIBEVENT_ROOT_DIR: ${LIBEVENT_ROOT_DIR}")

target_link_libraries(videocache PUBLIC
                        libace_napi.z.so
                        video_cache
                        ${OPENSSL_ROOT_DIR}/lib/libssl.a
                        ${OPENSSL_ROOT_DIR}/lib/libcrypto.a
                        ${LIBEVENT_ROOT_DIR}/lib/libevent.a
                        ${LIBEVENT_ROOT_DIR}/lib/libevent_openssl.a
                        -lz)