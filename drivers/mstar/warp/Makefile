#
# Makefile for MStar Infinity2 warp device drivers.

# general options
EXTRA_CFLAGS += -Idrivers/mstar/include
EXTRA_CFLAGS += -Idrivers/mstar/warp/drv/inc
EXTRA_CFLAGS += -Idrivers/mstar/warp/drv/pub
EXTRA_CFLAGS += -Idrivers/mstar/warp/hal/inc
EXTRA_CFLAGS += -Idrivers/mstar/warp/hal/pub

ccflags-$(CONFIG_MS_WARP) += -DMSOS_TYPE_LINUX_KERNEL

# files
obj-$(CONFIG_MS_WARP) += mstar_warp.o
mstar_warp-y := drv/src/common/dev_warp.o \
				drv/src/common/mhal_warp.o \
                drv/src/linux/lxdrv_warp.o \
			    hal/src/hal_ceva.o \
			    hal/src/hal_clk.o \
			    hal/src/hal_warp.o
