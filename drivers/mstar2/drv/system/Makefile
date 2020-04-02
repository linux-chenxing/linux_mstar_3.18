#
# Makefile for MStar System device drivers.
#
CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))
ifdef CONFIG_MSTAR_PROJECT_NAME
    CONFIG_MSTAR_PROJECT_NAME := $(subst ",,$(CONFIG_MSTAR_PROJECT_NAME))
endif


# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar2/include
EXTRA_CFLAGS      +=  -Idrivers/mstar2/drv/system
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/system
ifdef CONFIG_MSTAR_SPI_FLASH
EXTRA_CFLAGS      +=  -Idrivers/mstar2/drv/flash
EXTRA_CFLAGS      +=  -Idrivers/mstar2/include/sys/common
EXTRA_CFLAGS      +=  -Idrivers/mstar2/include/sys/drv
endif
# specific options
EXTRA_CFLAGS      +=

# files
obj-$(CONFIG_MSTAR_SYSTEM)	+= mdrv-system.o
mdrv-system-objs		+= mdrv_system.o
mdrv-system-objs		+= mdrv_system_io.o
mdrv-system-objs		+= ../../hal/$(CONFIG_MSTAR_CHIP_NAME)/system/mhal_system.o

