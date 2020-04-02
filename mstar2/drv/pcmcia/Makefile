#
# Makefile for MStar PCMCIA device drivers.
#
CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))
ifdef CONFIG_MSTAR_PROJECT_NAME
    CONFIG_MSTAR_PROJECT_NAME := $(subst ",,$(CONFIG_MSTAR_PROJECT_NAME))
endif

# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar2/include
EXTRA_CFLAGS      +=  -Idrivers/mstar2/drv/pcmcia
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/pcmcia
EXTRA_CFLAGS      +=  -Iinclude/linux
EXTRA_CFLAGS      +=  -Idrivers/media/dvb-core

# files
obj-$(CONFIG_MSTAR_PCMCIA)             := mdrv-pcmcia.o
mdrv-pcmcia-objs    := mdrv_pcmcia.o
mdrv-pcmcia-objs    += drvPCMCIA.o 
mdrv-pcmcia-objs    += ../../hal/$(CONFIG_MSTAR_CHIP_NAME)/pcmcia/halPCMCIA.o
