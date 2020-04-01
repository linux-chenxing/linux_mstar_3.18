#!/bin/bash
DEST_PATH=~/src/ipcam/project/I1/image
DEST_PATH=~/src/ipcam/project/I1/package/system/bin
case "$1" in
  ait_6366|AIT_6366)
    echo "build driver for AIT 6366"
    export CROSS_COMPILE=/opt/buildroot-vsnv3_2014q1/bin/arm-linux-
    export PLATFORM=AIT
    export LINUX_SRC=/home/shmily.chen/ipc/linux-3.18
    ;;

  mstar_i1|MSTAR_I1)

    echo "build driver for Mstar I1"
    export CROSS_COMPILE=/opt/gcc-linaro-arm-linux-gnueabihf-4.8-2014.04_linux/bin/arm-linux-gnueabihf-
    export PLATFORM=MSTAR_I1
    export LINUX_SRC=/home/shmily.chen/ipc/linux-3.18
    ;;

  *)
    echo "build driver for x86"
    ;;
esac


make all

${CROSS_COMPILE}strip ./os/linux/mt7601Uap.ko --strip-unneeded

sync
