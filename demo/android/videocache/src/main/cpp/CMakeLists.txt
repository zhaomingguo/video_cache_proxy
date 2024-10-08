# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.22.1)

# Declares and names the project.

project("videocache")

set(CMAKE_C_STANDARD 11)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(LIBS_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../libs)
message("ANDROID_ABI=${ANDROID_ABI}")
message("CMake Arguments: ${CMAKE_ARGS}")
message("LIBS_ROOT_DIR: ${LIBS_ROOT_DIR}")

set(VIDEO_CACHE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../video_cache_proxy)
include_directories(${VIDEO_CACHE_ROOT}/api)

add_subdirectory(${VIDEO_CACHE_ROOT} ${CMAKE_CURRENT_BINARY_DIR}/video_cache_proxy)

add_library( # Sets the name of the library.
        videocache

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        videocache.cpp)

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

set(EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../3rdparty)
set(OPENSSL_ROOT_DIR "${EXTERNAL_DIR}/openssl/android/${ANDROID_ABI}")
message("Victor OPENSSL_ROOT_DIR: ${OPENSSL_ROOT_DIR}")
set(LIBEVENT_ROOT_DIR "${EXTERNAL_DIR}/libevent/android/${ANDROID_ABI}")
message("Victor LIBEVENT_ROOT_DIR: ${LIBEVENT_ROOT_DIR}")

target_link_libraries(
        videocache
        video_cache
        ${OPENSSL_ROOT_DIR}/lib/libssl.a
        ${OPENSSL_ROOT_DIR}/lib/libcrypto.a
        ${LIBEVENT_ROOT_DIR}/lib/libevent.a
        ${LIBEVENT_ROOT_DIR}/lib/libevent_openssl.a
        z
        ${log-lib}
)