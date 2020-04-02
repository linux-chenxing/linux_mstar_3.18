#
# Makefile for MStar ipanic drivers.
#
CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))

# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar2/include
EXTRA_CFLAGS      +=  -Idrivers/mstar2/drv/ipanic
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/mmc/
EXTRA_CFLAGS      +=  -Idrivers/mstar2/drv/mmc/inc/config/
EXTRA_CFLAGS      +=  -Idrivers/staging/android/

# specific options
EXTRA_CFLAGS      +=  -Iinclude/linux


ipanic-y        := ipanic-utils.o ipanic-emmc.o dump-process.o
obj-$(CONFIG_IPANIC)        += ipanic.o



