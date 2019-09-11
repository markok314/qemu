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
        --with-sdlabi=2.0
done

echo "===================%%%%%%%%%%%%%%%%%%%%%%%==================="
EXE_DIR=/tmp/qemu-test/build/rh850-softmmu
DIST_DIR=/var/tmp/qemu/dist
mkdir $DIST_DIR
cp $EXE_DIR/qemu-system-rh850.exe $EXE_DIR/qemu-system-rh850w.exe $DIST_DIR
cp /usr/x86_64-w64-mingw32/sys-root/mingw/bin/*.dll $DIST_DIR
# cp arm-softmmu/qemu-system-arm*.exe /var/tmp/qemu
/bin/bash

