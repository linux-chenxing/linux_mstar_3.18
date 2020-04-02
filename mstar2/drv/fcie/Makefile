#
# Makefile for mstar SD card device drivers using FCIE IP.
#
CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))
ifdef CONFIG_MSTAR_PROJECT_NAME
    CONFIG_MSTAR_PROJECT_NAME := $(subst ",,$(CONFIG_MSTAR_PROJECT_NAME))
endif


# general options
EXTRA_CFLAGS		+=  -Idrivers/mstar2/include
EXTRA_CFLAGS		+=  -Idrivers/mstar2/drv/fcie
EXTRA_CFLAGS		+=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/fcie/

# specific options
EXTRA_CFLAGS		+=  -DMSOS_TYPE_LINUX

obj-y				+= mmc_host_fcie.o
obj-y				+= ../../hal/$(CONFIG_MSTAR_CHIP_NAME)/fcie/hal_fcie.o
obj-y				+= ../../hal/$(CONFIG_MSTAR_CHIP_NAME)/fcie/hal_fcie_platform.o
