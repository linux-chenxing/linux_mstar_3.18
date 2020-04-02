#//========================================================
#// Makefile for MStar DVFS Mechanism
#//========================================================

CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))
ifdef CONFIG_MSTAR_PROJECT_NAME
CONFIG_MSTAR_PROJECT_NAME := $(subst ",,$(CONFIG_MSTAR_PROJECT_NAME))
endif

EXTRA_CFLAGS += -Idrivers/mstar2/include
EXTRA_CFLAGS += -Idrivers/mstar2/drv/dvfs/include
EXTRA_CFLAGS += -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/dvfs
#EXTRA_CFLAGS += -Idrivers/mstar/dvfs/hal/$(CONFIG_MSTAR_CHIP_NAME)/include
EXTRA_CFLAGS += -Iinclude/linux

obj-$(CONFIG_MSTAR_DVFS) := mdrv_dvfs.o
obj-$(CONFIG_MSTAR_DVFS) += ../../hal/$(CONFIG_MSTAR_CHIP_NAME)/dvfs/mhal_dvfs.o 
ifdef CONFIG_MSTAR_DVFS_KERNEL_IIC
obj-$(CONFIG_MSTAR_IIC) += ../../hal/$(CONFIG_MSTAR_CHIP_NAME)/dvfs/mhal_dvfs_power.o
endif
#obj-$(CONFIG_CPU_FREQ) += mdrv_dvfs.o 
#obj-$(CONFIG_CPU_FREQ) += hal/$(CONFIG_MSTAR_CHIP_NAME)/mhal_dvfs.o
