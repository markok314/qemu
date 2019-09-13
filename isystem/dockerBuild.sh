#!/bin/bash

# cd $QEMU_SRC
# ./configure --cross-prefix=x86_64-w64-mingw32- --target-list=arm-softmmu
# make
# TARGET_LIST='rh850-softmmu'   - this is defined as env var when starting docker container

cd ../../tests/docker

# The following lines until 'echo' are copied from ./test-mingw, except 'install' and 'clean'

. common.rc

requires mingw dtc

cd "$BUILD_DIR"
DEF_TARGET_LIST="x86_64-softmmu,aarch64-softmmu"

# Build only 64-bit at the moment
# for prefix in x86_64-w64-mingw32- i686-w64-mingw32-; do
for prefix in x86_64-w64-mingw32-; do
    TARGET_LIST=${TARGET_LIST:-$DEF_TARGET_LIST} \
        build_qemu --cross-prefix=$prefix \
        --enable-trace-backends=simple \
        --enable-gnutls \
        --enable-nettle \
        --enable-curl \
        --enable-vnc \
        --enable-bzip2 \
        --enable-guest-agent \
        --with-sdlabi=2.0 \
        --disable-debug-info
done

echo "===================%%%%%%%%%%%%%%%%%%%%%%%==================="
EXE_DIR=/tmp/qemu-test/build
DIST_DIR=/var/tmp/qemu/dist
mkdir $DIST_DIR
cp /usr/x86_64-w64-mingw32/sys-root/mingw/bin/*.dll $DIST_DIR

executables=(arm aarch64 ppc ppc64 tricore rh850)
for exeprefix in "${executables[@]}"; do
    if [[ $TARGET_LIST == *"$exeprefix"* ]]; then
#        cp $EXE_DIR/$exeprefix-softmmu/qemu-system-$exeprefix.exe $DIST_DIR
        cp $EXE_DIR/$exeprefix-softmmu/qemu-system-${exeprefix}w.exe $DIST_DIR
    fi
done

#if [[ $TARGET_LIST == *"arm"* ]]; then
#    cp $EXE_DIR/arm-softmmu/qemu-system-arm.exe $EXE_DIR/arm-softmmu/qemu-system-armw.exe $DIST_DIR
#fi
#
#if [[ $TARGET_LIST == *"aarch64"* ]]; then
#    cp $EXE_DIR/aarch64-softmmu/qemu-system-aarch64.exe $EXE_DIR/aarch64-softmmu/qemu-system-aarch64w.exe $DIST_DIR
#fi
#
#if [[ $TARGET_LIST == *"ppc"* ]]; then
#    cp $EXE_DIR/ppc-softmmu/qemu-system-ppc.exe $EXE_DIR/ppc-softmmu/qemu-system-ppcw.exe $DIST_DIR
#fi
#
#if [[ $TARGET_LIST == *"ppc64"* ]]; then
#    cp $EXE_DIR/ppc64-softmmu/qemu-system-ppc64.exe $EXE_DIR/ppc64-softmmu/qemu-system-ppc64w.exe $DIST_DIR
#fi
#
#if [[ $TARGET_LIST == *"tricore"* ]]; then
#    cp $EXE_DIR/tricore-softmmu/qemu-system-tricore.exe $EXE_DIR/tricore-softmmu/qemu-system-tricorew.exe $DIST_DIR
#fi
#
#if [[ $TARGET_LIST == *"rh850"* ]]; then
#    cp $EXE_DIR/rh850-softmmu/qemu-system-rh850.exe $EXE_DIR/rh850-softmmu/qemu-system-rh850w.exe $DIST_DIR
#fi
#
echo "BUILD FINISHED"
# /bin/bash

