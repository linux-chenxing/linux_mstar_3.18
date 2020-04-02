#
# Makefile for MStar SMC device drivers.
#
CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))
ifdef CONFIG_MSTAR_PROJECT_NAME
    CONFIG_MSTAR_PROJECT_NAME := $(subst ",,$(CONFIG_MSTAR_PROJECT_NAME))
endif

# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar2/include
ifeq ($(CONFIG_MSTAR_CHIP_NAME), macan)
EXTRA_CFLAGS      +=  -Idrivers/mstar2/drv/xc/1_4_0
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_0
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_0/tcgen/include/
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_0/tcgen/basicops/
else
EXTRA_CFLAGS      +=  -Idrivers/mstar2/drv/xc
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/xc
endif
EXTRA_CFLAGS      +=  -Iinclude/linux

# specific options
EXTRA_CFLAGS      +=  -DRED_LION
# files
obj-$(CONFIG_MSTAR_XC)             := mdrv-xc.o

mdrv-xc-objs    += mdrv_xc.o mdrv_xc_io.o
mdrv-xc-objs    += mdrv_xc_menuload.o
mdrv-xc-objs    += mdrv_xc_dynamicscaling.o
mdrv-xc-objs    += mdrv_dlc.o
ifeq ($(CONFIG_MSTAR_CHIP_NAME), macan)
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_0/mhal_xc.o
else
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/mhal_xc.o
endif
ifeq ($(CONFIG_MSTAR_CHIP_NAME),$(filter $(CONFIG_MSTAR_CHIP_NAME), k6 curry macan kano k6lite k7u_ca7 k7u_ca53 c2p))
ifeq ($(CONFIG_MSTAR_CHIP_NAME), macan)
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_0/mhal_menuload.o
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_0/mhal_dynamicscaling.o
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_0/mhal_dlc.o
else
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/mhal_menuload.o
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/mhal_dynamicscaling.o
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/mhal_dlc.o
endif
endif

ifdef CONFIG_MSTAR_XC_HDR_SUPPORT
ifeq ($(CONFIG_MSTAR_CHIP_NAME), macan)
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_0/dolby_vision_driver.o
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_0/dolby_hdr_mem.o
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_0/color_format_driver_hardware.o
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_0/color_format_driver.o
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_0/color_format_input.o
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_0/color_format_ip.o
#tcgen
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_0/tcgen/source/toneCurveLUT.o
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_0/tcgen/source/lc_2x1d_luts.o
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_0/tcgen/source/saturation_table.o
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_0/tcgen/basicops/fx_basicops.o
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_0/tcgen/basicops/fx_math.o
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_0/mhal_pq_adaptive.o
else
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/dolby_vision_driver.o
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/dolby_hdr_mem.o
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/color_format_driver_hardware.o
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/color_format_driver.o
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/color_format_input.o
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/color_format_ip.o
endif
endif

#TCH
ifeq ($(CONFIG_MSTAR_CHIP_NAME),$(filter $(CONFIG_MSTAR_CHIP_NAME), k6 k7u_ca53))
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/technicolor_driver.o
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/tc_hdr_mem.o
endif
