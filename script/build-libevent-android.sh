#!/bin/bash

export ANDROID_NDK_HOME=/Users/mingo/Library/Android/sdk/ndk/22.1.7171670
export TOOLCHAIN_DIR=$ANDROID_NDK_HOME/toolchains/llvm/prebuilt/darwin-x86_64
export SYSROOT=$TOOLCHAIN_DIR/sysroot

export TARGET_ARCH=aarch64-linux-android
export TOOLCHAIN=$TOOLCHAIN_DIR
export API_LEVEL=21
TARGET=aarch64-linux-android${API_LEVEL}

export CLANG="$TOOLCHAIN/bin/aarch64-linux-android$API_LEVEL-clang"
export CLANGXX="$TOOLCHAIN/bin/aarch64-linux-android$API_LEVEL-clang++"
export AR="$TOOLCHAIN/bin/aarch64-linux-android-ar"
export LD="$TOOLCHAIN/bin/aarch64-linux-android-ld"
export RANLIB="$TOOLCHAIN/bin/aarch64-linux-android-ranlib"
export STRIP="$TOOLCHAIN/bin/aarch64-linux-android-strip"

OPENSSL_OUTPUT=/Users/mingo/Applications/workspace/opensrc/openssl-android/android
OPENSSL_DIR=${OPENSSL_OUTPUT}/aarch64
PKG_CONFIG_PATH="$OPENSSL_DIR/lib/pkgconfig"

CFLAGS="--sysroot=$SYSROOT -I$OPENSSL_DIR/include --target=$TARGET -fPIC -DEVUTIL_DISABLE_ARC4RANDOM"
LDFLAGS="--sysroot=$SYSROOT -L$OPENSSL_DIR/lib -lz"

OUTPUT_DIR=$(pwd)/android/$TARGET_ARCH
mkdir -p ${OUTPUT_DIR}

function build() {
    ./configure --host=$TARGET_ARCH --prefix=$OUTPUT_DIR \
        --with-sysroot=${SYSROOT} \
        --with-openssl=$OPENSSL_DIR \
        --disable-shared \
        --disable-arc4random \
        CC=$CLANG \
        CXX=$CLANGXX \
        AR=$AR \
        RANLIB=$RANLIB \
        CFLAGS="$CFLAGS" \
        LDFLAGS="$LDFLAGS"

    make clean
    make j8
    make install
}
build

TARGET_ARCH=arm-linux-androideabi
TARGET=arm-linux-android${API_LEVEL}
OUTPUT_DIR=$(pwd)/android/$TARGET_ARCH
mkdir -p ${OUTPUT_DIR}

OPENSSL_DIR=${OPENSSL_OUTPUT}/armv7a
PKG_CONFIG_PATH="$OPENSSL_DIR/lib/pkgconfig"

CFLAGS="--sysroot=$SYSROOT -I$OPENSSL_DIR/include --target=$TARGET -fPIC -DEVUTIL_DISABLE_ARC4RANDOM"
LDFLAGS="--sysroot=$SYSROOT -L$OPENSSL_DIR/lib -lz"

build