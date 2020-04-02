#
# Makefile for MStar GMAC device drivers.
#
CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))
ifdef CONFIG_MSTAR_PROJECT_NAME
    CONFIG_MSTAR_PROJECT_NAME := $(subst ",,$(CONFIG_MSTAR_PROJECT_NAME))
endif

# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar2/include
EXTRA_CFLAGS      +=  -Idrivers/mstar2/drv/gmac
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/gmac
EXTRA_CFLAGS      +=  -Iinclude/linux

# specific options
EXTRA_CFLAGS      +=  -DRED_LION
# files
obj-$(CONFIG_MSTAR_GMAC)             := mdrv-gmac.o

ifneq ($(CONFIG_MSTAR_GMAC_V2),)
mdrv-gmac-objs    := mdrv_gmac_v2.o
else
ifneq ($(CONFIG_MSTAR_GMAC_V3),)
mdrv-gmac-objs    := mdrv_gmac_v3.o
else
mdrv-gmac-objs    := mdrv_gmac.o
endif
endif

ifneq ($(CONFIG_MSTAR_GMAC_V3),)
mdrv-gmac-objs    += ../../hal/$(CONFIG_MSTAR_CHIP_NAME)/gmac/mhal_gmac_v3.o
else
mdrv-gmac-objs    += ../../hal/$(CONFIG_MSTAR_CHIP_NAME)/gmac/mhal_gmac.o
endif