#
# Makefile for MStar SMC device drivers.
#
CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))
ifdef CONFIG_MSTAR_PROJECT_NAME
    CONFIG_MSTAR_PROJECT_NAME := $(subst ",,$(CONFIG_MSTAR_PROJECT_NAME))
endif

# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar2/include
EXTRA_CFLAGS      +=  -Idrivers/mstar2/drv/sc
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/sc
EXTRA_CFLAGS      +=  -Iinclude/linux

# specific options
EXTRA_CFLAGS      +=  -DRED_LION
# files
obj-$(CONFIG_MSTAR_SC)             := mdrv-sc.o

mdrv-sc-objs    += mdrv_sc.o mdrv_sc_io.o
mdrv-sc-objs    += ../../hal/$(CONFIG_MSTAR_CHIP_NAME)/sc/mhal_sc.o
