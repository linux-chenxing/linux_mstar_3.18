#
# Makefile for MStar EMAC device drivers.
#
CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))
ifdef CONFIG_MSTAR_PROJECT_NAME
    CONFIG_MSTAR_PROJECT_NAME := $(subst ",,$(CONFIG_MSTAR_PROJECT_NAME))
endif

# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar2/include
EXTRA_CFLAGS      +=  -Idrivers/mstar2/drv/emac
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/emac
EXTRA_CFLAGS      +=  -Iinclude/linux

# specific options
EXTRA_CFLAGS      +=  -DRED_LION
# files
obj-$(CONFIG_MSTAR_EMAC)             := mdrv-emac.o

ifneq ($(CONFIG_MSTAR_EMAC_V3),)
mdrv-emac-objs    := mdrv_emac_v3.o
else
mdrv-emac-objs    := mdrv_emac.o
endif
ifneq ($(CONFIG_MSTAR_EMAC_V3),)
mdrv-emac-objs    += ../../hal/$(CONFIG_MSTAR_CHIP_NAME)/emac/mhal_emac_v3.o
else
mdrv-emac-objs    += ../../hal/$(CONFIG_MSTAR_CHIP_NAME)/emac/mhal_emac.o
endif
