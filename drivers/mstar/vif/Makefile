#
# Makefile for MStar VIF device drivers.
#

CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))
# general options
# EXTRA_CFLAGS      +=  -Idrivers/mstar/common
EXTRA_CFLAGS      +=  -Idrivers/mstar/include
EXTRA_CFLAGS      +=  -Idrivers/mstar/vif/drv/inc
EXTRA_CFLAGS      +=  -Idrivers/mstar/vif/$(CONFIG_MSTAR_CHIP_NAME)
EXTRA_CFLAGS      +=  -Idrivers/mstar/cam_os_wrapper/pub



# specific options
#EXTRA_CFLAGS      +=  -DTITANIA
#EXTRA_CFLAGS      +=  -DMSOS_TYPE_LINUX

# files
obj-$(CONFIG_MS_VIF_INFINITY2) += hal/infinity2/hal_vif.o drv/src/drv_vif.o drv/src/linux/vif_sys_linux.o drv/src/common/vif_common.o
obj-$(CONFIG_MS_VIF_INFINITY2) += hal/infinity2/hal_dma.o
# export header files
#EXPORT_H_FILES    += mdrv_vif_io.h
