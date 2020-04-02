#
# Makefile for MStar GFLIP device drivers.
CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))
ifdef CONFIG_MSTAR_PROJECT_NAME
    CONFIG_MSTAR_PROJECT_NAME := $(subst ",,$(CONFIG_MSTAR_PROJECT_NAME))
endif


# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar2/include
EXTRA_CFLAGS      +=  -Idrivers/mstar2/drv/graphic/
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/graphic/
EXTRA_CFLAGS      +=  -Idrivers/mstar2/include/sys/common

EXTRA_CFLAGS      +=  -DRED_LION

# specific options
EXTRA_CFLAGS      +=

# files
mstar_fbdev-y	+= iniparser.o mdrv_graphic_io.o mdrv_graphic.o ../../hal/$(CONFIG_MSTAR_CHIP_NAME)/graphic/mhal_graphic.o
obj-$(CONFIG_MSTAR_GRAPHIC) := mstar_fbdev.o

