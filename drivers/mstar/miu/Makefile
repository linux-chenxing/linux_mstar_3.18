#
# Makefile for MStar mtlb device drivers.
#

CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))
ifdef CONFIG_MSTAR_PROJECT_NAME
    CONFIG_MSTAR_PROJECT_NAME := $(subst ",,$(CONFIG_MSTAR_PROJECT_NAME))
endif

# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar/include/
EXTRA_CFLAGS      +=  -Idrivers/mstar/include/$(CONFIG_MSTAR_CHIP_NAME)
EXTRA_CFLAGS      +=  -Idrivers/mstar/miu
EXTRA_CFLAGS      +=  -Idrivers/mstar/hal/$(CONFIG_MSTAR_CHIP_NAME)/miu
EXTRA_CFLAGS      +=  -Iinclude/linux
# specific options
EXTRA_CFLAGS      +=


# files
obj-$(CONFIG_MSTAR_MIU)             := mdrv-miu.o

mdrv-miu-objs    := mdrv_miu.o
#mdrv-miu-objs    += ../../hal/$(CONFIG_MSTAR_CHIP_NAME)/miu/mhal_miu.o
mdrv-miu-objs    += ./mhal_miu.o

