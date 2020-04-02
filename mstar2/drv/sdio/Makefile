#
# Makefile for mstar SD card device drivers using SDIO IP.
#
CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))
ifdef CONFIG_MSTAR_PROJECT_NAME
    CONFIG_MSTAR_PROJECT_NAME := $(subst ",,$(CONFIG_MSTAR_PROJECT_NAME))
endif


# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar2/include
EXTRA_CFLAGS		+=  -Idrivers/mstar2/drv/sdio
EXTRA_CFLAGS		+=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/sdio/

# specific options
EXTRA_CFLAGS      +=  -DMSOS_TYPE_LINUX

obj-y				+= mmc_host_sdio.o
obj-y				+= ../../hal/$(CONFIG_MSTAR_CHIP_NAME)/sdio/hal_sdio.o
obj-y				+= ../../hal/$(CONFIG_MSTAR_CHIP_NAME)/sdio/hal_sdio_platform.o
