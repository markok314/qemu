#!bash

cd $QEMU_SRC
./configure --cross-prefix=x86_64-w64-mingw32-
make
#cp arm-softmmu/qemu-system-arm*.exe /var/tmp/qemu
#exit 1
