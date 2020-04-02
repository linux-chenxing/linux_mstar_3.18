#
# Makefile for MStar cpu power manager.
#

CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))


EXTRA_CFLAGS += -Idrivers/mstar2/include

# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar2/include/sys/common
EXTRA_CFLAGS      +=  -Idrivers/mstar2/include
EXTRA_CFLAGS      +=  -Idrivers/mstar2/drv/cpupm
EXTRA_CFLAGS += -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/cpupm
EXTRA_CFLAGS      +=  -Iinclude/linux
EXTRA_CFLAGS += -Idrivers/mstar2/drv/dvfs/include
EXTRA_CFLAGS += -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/dvfs


obj-y				+= ../../hal/$(CONFIG_MSTAR_CHIP_NAME)/cpupm/mhal_cpupm.o
obj-$(CONFIG_MSTAR_CPUPM)		+=	mdrv_cpupm.o



