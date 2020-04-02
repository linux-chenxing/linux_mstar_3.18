#
# Makefile for MStar PCIE device drivers.
#

ifeq ($(MAKE_TYPE), MODULE_STANDALONE)
include $(TOP_DIR)/modules.mk
endif

CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))

# general options
EXTRA_CFLAGS      +=  -Iinclude/linux
EXTRA_CFLAGS      +=  -Idrivers/mstar2/include
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/pcie2

ifeq ($(CONFIG_ARM64),1)
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/cpu/arm64/
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/cpu/arm64/include/mach
else
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/cpu/arm/
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/cpu/arm/include/mach
endif


# files
mstar_pcie2-y := pcie-mstar.o

obj-$(CONFIG_MSTAR_PCIE2) += mstar_pcie2.o
ifeq ($(CONFIG_MSTAR_ARM_BD_FPGA),y)
obj-$(CONFIG_MSTAR_PCIE2) += pcie-iic.o
endif
