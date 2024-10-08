#!/bin/bash

set -e

HARMONY_SDK_PATH=/Users/mingo/Library/OpenHarmony/Sdk/12

CLANG=$HARMONY_SDK_PATH/native/llvm/bin/clang
CLANGXX=$HARMONY_SDK_PATH/native/llvm/bin/clang++
AR=$HARMONY_SDK_PATH/native/llvm/bin/llvm-ar
RANLIB=$HARMONY_SDK_PATH/native/llvm/bin/llvm-ranlib

# compile for aarch64
TARGET_ARCH=aarch64-linux-gnu
TARGET=aarch64-linux-ohos
SYSROOT=$HARMONY_SDK_PATH/native/sysroot

ZLIB_ROOT_PATH=${HARMONY_SDK_PATH}/native/sysroot/usr
ZLIB_INCLUDE_PATH=${ZLIB_ROOT_PATH}/include
ZLIB_LIB_PATH=${ZLIB_ROOT_PATH}/lib/aarch64-linux-ohos

OPENSSL_DIR=/Users/mingo/Applications/workspace/opensrc/openssl/hmos/aarch64
PKG_CONFIG_PATH="$OPENSSL_DIR/lib/pkgconfig"
CPPFLAGS="-I$OPENSSL_DIR/include -I${ZLIB_INCLUDE_PATH}"

SYSLIB_INCLUDE_DIR=${SYSROOT}/usr/include/aarch64-linux-ohos
echo "syslib=$SYSLIB_INCLUDE_DIR"
CFLAGS="--sysroot=$SYSROOT -I$SYSLIB_INCLUDE_DIR -I$OPENSSL_DIR/include -I$ZLIB_INCLUDE_PATH --target=$TARGET -fPIC"
#CFLAGS="--sysroot=$SYSROOT -I$SYSLIB_INCLUDE_DIR -I$ZLIB_INCLUDE_PATH --target=$TARGET -fPIC"
LDFLAGS="--sysroot=$SYSROOT -L$OPENSSL_DIR/lib -L$ZLIB_LIB_PATH -lz"

export CC=$CLANG
export CXX=$CLANGXX
export AR=$AR
export RANLIB=$RANLIB
export CFLAGS=$CFLAGS
export LDFLAGS=$LDFLAGS

# compile for libevent-2.1.8-stable
LIBEVENT_SRC_PATH=`pwd`/libevent-2.1.8-stable

cd $LIBEVENT_SRC_PATH
PREFIX=${LIBEVENT_SRC_PATH}/ohos/aarch64
mkdir -p ${PREFIX}

function build_event() {
    make clean
    make distclean

    ./configure --host=$TARGET_ARCH --prefix=$PREFIX \
        --with-sysroot=${SYSLIB_INCLUDE_DIR} \
        --with-openssl=$OPENSSL_DIR \
        --with-zlib=$ZLIB_ROOT_PATH \
		--disable-shared \
        CC=$CLANG \
        CXX=$CLANGXX \
        AR=$AR \
        RANLIB=$RANLIB \
        CFLAGS="$CFLAGS" \
        LDFLAGS="$LDFLAGS"

    make verbose=1
    make install
    cd ..
}

build_event
