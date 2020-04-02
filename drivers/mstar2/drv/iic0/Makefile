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
EXTRA_CFLAGS      +=  -Idrivers/mstar2/iic0
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/iic0
EXTRA_CFLAGS      +=  -Idrivers/mstar2/gpio
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/gpio

# specific options
EXTRA_CFLAGS      +=  -DTITANIA
EXTRA_CFLAGS      +=  -DMSOS_TYPE_LINUX

# files
obj-$(CONFIG_MSTAR_IIC0)	+= mdrv-iic0.o
mdrv-iic0-objs			+= mdrv_iic0.o mdrv_iic0_io.o i2c0_adapter.o
mdrv-iic0-objs			+= ../../hal/$(CONFIG_MSTAR_CHIP_NAME)/iic0/mhal_iic0.o

# export header files
EXPORT_H_FILES    += mdrv_iic_io0.h
