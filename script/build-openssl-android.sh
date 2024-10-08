#!/bin/bash

export ANDROID_NDK_HOME=/Users/mingo/Library/Android/sdk/ndk/21.4.7075529
export TOOLCHAIN_DIR=$ANDROID_NDK_HOME/toolchains/llvm/prebuilt/darwin-x86_64
export PATH=$TOOLCHAIN_DIR/bin:$PATH

export TARGET_ARCH=aarch64
export API_LEVEL=21

export CC="$TOOLCHAIN_DIR/bin/aarch64-linux-android$API_LEVEL-clang"
export CXX="$TOOLCHAIN_DIR/bin/aarch64-linux-android$API_LEVEL-clang++"
export AR="$TOOLCHAIN_DIR/bin/aarch64-linux-android-ar"
export AS="$TOOLCHAIN_DIR/bin/aarch64-linux-android-as"
export LD="$TOOLCHAIN_DIR/bin/aarch64-linux-android-ld"
export RANLIB="$TOOLCHAIN_DIR/bin/aarch64-linux-android-ranlib"
export STRIP="$TOOLCHAIN_DIR/bin/aarch64-linux-android-strip"

function build() {
    make clean
    OUTPUT_DIR=$(pwd)/android/$TARGET_ARCH
    mkdir -p ${OUTPUT_DIR}

    #config for android arm64
    ./Configure ${TARGET} no-asm zlib no-shared no-ssl2 no-ssl3 no-comp no-hw no-engine --prefix=${OUTPUT_DIR}

    make -j8
    make install
}
TARGET=android-arm64
build

export TARGET_ARCH=armv7a

export TARGET_HOST=armv7a-linux-androideabi
export CC="$TOOLCHAIN_DIR/bin/${TARGET_HOST}${API_LEVEL}-clang"
export CXX="$TOOLCHAIN_DIR/bin/${TARGET_HOST}${API_LEVEL}-clang++"
export AR="$TOOLCHAIN_DIR/bin/${TARGET_HOST}-ar"
export AS="$TOOLCHAIN_DIR/bin/${TARGET_HOST}-as"
export LD="$TOOLCHAIN_DIR/bin/${TARGET_HOST}-ld"
export RANLIB="$TOOLCHAIN_DIR/bin/${TARGET_HOST}-ranlib"
export STRIP="$TOOLCHAIN_DIR/bin/${TARGET_HOST}-strip"

TARGET=android-arm
build