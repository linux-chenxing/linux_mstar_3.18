#
# Makefile for MStar IIC device drivers.
#

ifeq ($(MAKE_TYPE), MODULE_STANDALONE)
include $(TOP_DIR)/modules.mk
endif

CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))

# general options
# EXTRA_CFLAGS      +=  -Idrivers/mstar/common
EXTRA_CFLAGS      +=  -Idrivers/mstar2/include
EXTRA_CFLAGS      +=  -Idrivers/mstar2/iic
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/iic
EXTRA_CFLAGS      +=  -Idrivers/mstar2/gpio
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/gpio

# specific options
EXTRA_CFLAGS      +=  -DTITANIA
EXTRA_CFLAGS      +=  -DMSOS_TYPE_LINUX

# files
obj-$(CONFIG_MSTAR_IIC)	+= mdrv-iic.o
mdrv-iic-objs			+= mdrv_iic.o mdrv_iic_io.o
mdrv-iic-objs			+= ../../hal/$(CONFIG_MSTAR_CHIP_NAME)/iic/mhal_iic.o

# export header files
EXPORT_H_FILES    += mdrv_iic_io.h
