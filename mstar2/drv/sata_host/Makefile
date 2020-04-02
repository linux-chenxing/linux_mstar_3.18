#
# Makefile for MStar Video Prealloc Memory device drivers.
#
CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))
ifdef CONFIG_MSTAR_PROJECT_NAME
    CONFIG_MSTAR_PROJECT_NAME := $(subst ",,$(CONFIG_MSTAR_PROJECT_NAME))
endif

# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar2/include
EXTRA_CFLAGS      +=  -Idrivers/mstar2/drv/sata_host
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/sata_host
EXTRA_CFLAGS      +=  -Iinclude/linux

# specific options
EXTRA_CFLAGS      +=

# files
obj-$(CONFIG_MSTAR_SATA_HOST)	:= mdrv-sata-host.o
mdrv-sata-host-objs		:= mdrv_sata_host.o
mdrv-sata-host-objs		+= ../../hal/$(CONFIG_MSTAR_CHIP_NAME)/sata_host/mhal_sata_host.o
