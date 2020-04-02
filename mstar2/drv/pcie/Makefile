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
EXTRA_CFLAGS      +=  -Idrivers/mstar2/pcie
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/pcie
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/pcie/include
#add for pcie GPIO dynamic setting
EXTRA_CFLAGS      +=  -Idrivers/usb/host
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/usb


ifeq ($(CONFIG_ARM64),1)
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/cpu/arm64/
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/cpu/arm64/include/mach
else
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/cpu/arm/
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/cpu/arm/include/mach
endif

# specific options
ifdef USE_VIP_EP
EXTRA_CFLAGS += -DUSE_VIP_EP=$(USE_VIP_EP)
endif


# files
mstar_pcie-y := cpd_lnx_drv.o
mstar_pcie-y += mdrv_pcie_drv.o
mstar_pcie-y += ../../hal/$(CONFIG_MSTAR_CHIP_NAME)/pcie/cpdi_core_driver.o
mstar_pcie-y += ../../hal/$(CONFIG_MSTAR_CHIP_NAME)/pcie/cpd_main.o


obj-$(CONFIG_MSTAR_PCIE)	+= mstar_pcie.o


#obj-m += cpdLnxDrv.o
#cpdLnxDrv-objs := cpd_lnx_drv.o  mdrv_pcie_drv.o ../../hal/$(CONFIG_MSTAR_CHIP_NAME)/pcie/cpdi_core_driver.o  ../../hal/$(CONFIG_MSTAR_CHIP_NAME)/pcie/cpd_main.o




