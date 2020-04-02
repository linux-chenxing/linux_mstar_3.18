EXTRA_CFLAGS      +=  -Idrivers/mstar/driver/drv/mload/drv/inc
EXTRA_CFLAGS      +=  -Idrivers/mstar/driver/drv/mload/hal/infinity2/inc
EXTRA_CFLAGS      +=  -Idrivers/mstar/driver/drv/mload/drv/pub/
EXTRA_CFLAGS      +=  -Idrivers/mstar/driver/drv/MI_HAL/pub/




obj-$(CONFIG_MS_MLOAD) +=  mload.o

mload-y :=     drv/src/linux/mdrv_mload.o \
               drv/src/linux/drv_mload.o \
               drv/src/common/mload_common.o \
               hal/infinity2/src/hal_mload.o
