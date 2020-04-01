#
# Makefile for MStar EMAC device drivers.
#
ifdef CONFIG_ARCH_INFINITY
    CONFIG_MSTAR_CHIP_NAME := infinity
endif
ifdef CONFIG_ARCH_INFINITY3
    CONFIG_MSTAR_CHIP_NAME := infinity3
endif

# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar/include
EXTRA_CFLAGS      +=  -Idrivers/mstar/emac
EXTRA_CFLAGS      +=  -Idrivers/mstar/emac/hal/$(CONFIG_MSTAR_CHIP_NAME)
EXTRA_CFLAGS      +=  -Iinclude/linux

# specific options
# EXTRA_CFLAGS      +=  -DRED_LION
# files
obj-$(CONFIG_MS_EMAC)             := kdrv_emac.o
kdrv_emac-y    := mdrv_emac.o
kdrv_emac-y    += hal/$(CONFIG_MSTAR_CHIP_NAME)/mhal_emac.o