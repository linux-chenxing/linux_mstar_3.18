MS_PNL_ARCH_NAME=infinity2
MS_PNL_OS_NAME=linux

EXTRA_CFLAGS      +=  -Idrivers/mstar/include
EXTRA_CFLAGS      +=  -Idrivers/mstar/include/infinity2
EXTRA_CFLAGS      +=  -Idrivers/mstar/cam_os_wrapper/pub

#
# Makefile for MStar PNL drivers.
#


# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar/driver/drv/pnl/inc
EXTRA_CFLAGS      +=  -Idrivers/mstar/driver/hal/$(MS_PNL_ARCH_NAME)/pnl/inc


EXTRA_CFLAGS += -Werror

# specific options
#
#EXTRA_CFLAGS      +=  -DSCLOS_TYPE_LINUX_KERNEL



obj-$(CONFIG_MS_PNL)   += mdrv_pnl.o

mdrv_pnl-y :=     ./src/halPnl.o \
                  ../../../drv/pnl/src/drv_pnl.o \
                  ../../../drv/pnl/src/mdrv_pnl_io.o
                 
