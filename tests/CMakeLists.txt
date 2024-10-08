cmake_minimum_required(VERSION 3.13)

project(video_cache_unittest CXX)
enable_language(C CXX)

set(CMAKE_CXX_STANDARD 14)
add_compile_options(-fprofile-arcs -ftest-coverage)
add_link_options(--coverage)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../video_cache_proxy 
                 ${CMAKE_CURRENT_BINARY_DIR}/video_cache_proxy-build
                 EXCLUDE_FROM_ALL)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(PROJECT_TOP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)

include_directories(${PROJECT_TOP_DIR})
include_directories(${PROJECT_TOP_DIR}/video_cache_proxy)
include_directories(${PROJECT_TOP_DIR}/tests)
include_directories(${PROJECT_TOP_DIR}/3rdparty/libevent/macos/aarch64/include)
include_directories(${PROJECT_TOP_DIR}/3rdparty/openssl/macos/aarch64/include)

enable_testing()
aux_source_directory(. TEST_FILES)
list(APPEND ${TEST_FILES} ${PROJECT_TOP_DIR}/tests/http-proxy-api-test.hpp)
add_executable(video_cache_unittest ${TEST_FILES})

file(GLOB_RECURSE VIDEO_CACHE_SOURCES ${PROJECT_TOP_DIR}/video_cache_proxy/*.c])
target_sources(video_cache_unittest
  PUBLIC
  ${VIDEO_CACHE_SOURCES}
)

target_link_libraries(video_cache_unittest gtest_main)
target_link_libraries(video_cache_unittest gmock_main)

target_link_libraries(video_cache_unittest
    video_cache
    ${PROJECT_TOP_DIR}/3rdparty/libevent/macos/aarch64/lib/libevent.a
    ${PROJECT_TOP_DIR}/3rdparty/libevent/macos/aarch64/lib/libevent_openssl.a
    ${PROJECT_TOP_DIR}/3rdparty/openssl/macos/aarch64/lib/libssl.a
    ${PROJECT_TOP_DIR}/3rdparty/openssl/macos/aarch64/lib/libcrypto.a
    pthread z)

add_test(NAME VideoCacheUnitTest COMMAND video_cache_unittest)
