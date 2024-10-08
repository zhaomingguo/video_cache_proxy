#!/bin/bash

NATIVE_PATH=/Users/mingo/Library/OpenHarmony/Sdk/12/native
LLVM_PATH=$NATIVE_PATH/llvm
SYSROOT_PATH=$NATIVE_PATH/sysroot

function build
{
./Configure $CPU --prefix=$PREFIX zlib \
no-asm \
no-shared \
no-unit-test \
no-tests

make clean
make -j8
make install

}

#armv7a
ARCH=armv7
CPU=linux-armv4
PREFIX=`pwd`/hmos/$ARCH/
mkdir -p ${PREFIX}
export AR=$NATIVE_PATH/llvm/bin/llvm-ar
export CC=$NATIVE_PATH/llvm/bin/clang
export LD=$NATIVE_PATH/llvm/bin/ld.lld
export CFLAGS="--target=arm-linux-ohos --gcc-toolchain=$NATIVE_PATH/llvm --sysroot=$NATIVE_PATH/sysroot -g -fvisibility=hidden -fdata-sections -ffunction-sections -funwind-tables -fstack-protector-strong -no-canonical-prefixes -fno-addrsig -Wa,--noexecstack -Wformat -Werror=format-security -fno-limit-debug-info  -fPIC -MD -march=armv7-a -mthumb"
export LDFLAGS="--rtlib=compiler-rt -fuse-ld=lld -lunwind"
export RANLIB=$NATIVE_PATH/llvm/bin/llvm-ranlib
build

#arm64
ARCH=aarch64
CPU=linux-aarch64
PREFIX=`pwd`/hmos/$ARCH/
mkdir -p ${PREFIX}
export AR=$NATIVE_PATH/llvm/bin/llvm-ar
export CC=$NATIVE_PATH/llvm/bin/clang
export LD=$NATIVE_PATH/llvm/bin/ld.lld
export CFLAGS="--target=aarch64-linux-ohos --gcc-toolchain=$NATIVE_PATH/llvm --sysroot=$NATIVE_PATH/sysroot -g -fvisibility=hidden -fdata-sections -ffunction-sections -funwind-tables -fstack-protector-strong -no-canonical-prefixes -fno-addrsig -Wa,--noexecstack -Wformat -Werror=format-security -fno-limit-debug-info  -fPIC -MD -mthumb"
export LDFLAGS="--rtlib=compiler-rt -fuse-ld=lld -lunwind"
export RANLIB=$NATIVE_PATH/llvm/bin/llvm-ranlib
build
