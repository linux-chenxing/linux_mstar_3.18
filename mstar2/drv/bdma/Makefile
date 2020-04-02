#//========================================================
#// Makefile for MStar BDMA
#//========================================================

CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))
ifdef CONFIG_MSTAR_PROJECT_NAME
CONFIG_MSTAR_PROJECT_NAME := $(subst ",,$(CONFIG_MSTAR_PROJECT_NAME))
endif

EXTRA_CFLAGS += -Idrivers/mstar2/include
EXTRA_CFLAGS += -Idrivers/mstar2/drv/bdma
EXTRA_CFLAGS += -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/bdma
EXTRA_CFLAGS += -Iinclude/linux

obj-$(CONFIG_MSTAR_BDMA) := mdrv_bdma.o
obj-$(CONFIG_MSTAR_BDMA) += ../../hal/$(CONFIG_MSTAR_CHIP_NAME)/bdma/mhal_bdma.o 
#obj-$(CONFIG_CPU_FREQ) += mdrv_bdma.o 
#obj-$(CONFIG_CPU_FREQ) += hal/$(CONFIG_MSTAR_CHIP_NAME)/mhal_bdma.o
