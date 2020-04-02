#
# Makefile for MStar ZDEC
#
CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))

EXTRA_CFLAGS += -Idrivers/mstar2/include
EXTRA_CFLAGS += -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/zdec

obj-y        += ../../hal/$(CONFIG_MSTAR_CHIP_NAME)/zdec/mhal_zdec.o
obj-y        += mstar_zdec.o

