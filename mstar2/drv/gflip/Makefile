#
# Makefile for MStar GFLIP device drivers.
CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))
ifdef CONFIG_MSTAR_PROJECT_NAME
    CONFIG_MSTAR_PROJECT_NAME := $(subst ",,$(CONFIG_MSTAR_PROJECT_NAME))
endif


# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar2/include
EXTRA_CFLAGS      +=  -Idrivers/mstar2/drv/gflip/
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/gflip/

EXTRA_CFLAGS      +=  -DRED_LION

# specific options
EXTRA_CFLAGS      +=

# files
obj-$(CONFIG_MSTAR_GFLIP)	+= mdrv_gflip_io.o mdrv_gflip.o mdrv_gflip_interrupt.o
obj-$(CONFIG_MSTAR_GFLIP) += ../../hal/$(CONFIG_MSTAR_CHIP_NAME)/gflip/mhal_gflip.o
obj-$(CONFIG_MSTAR_GFLIP) += ../../hal/$(CONFIG_MSTAR_CHIP_NAME)/gflip/mhal_dlc.o

