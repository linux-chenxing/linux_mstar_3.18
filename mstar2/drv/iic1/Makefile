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
EXTRA_CFLAGS      +=  -Idrivers/mstar2/iic1
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/iic1
EXTRA_CFLAGS      +=  -Idrivers/mstar2/gpio
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/gpio

# specific options
EXTRA_CFLAGS      +=  -DTITANIA
EXTRA_CFLAGS      +=  -DMSOS_TYPE_LINUX

# files
obj-$(CONFIG_MSTAR_IIC1)	+= mdrv-iic1.o
mdrv-iic1-objs			+= mdrv_iic1.o mdrv_iic1_io.o i2c1_adapter.o
mdrv-iic1-objs			+= ../../hal/$(CONFIG_MSTAR_CHIP_NAME)/iic1/mhal_iic1.o

# export header files
EXPORT_H_FILES    += mdrv_iic_io1.h
