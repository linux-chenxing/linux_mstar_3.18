#
# Makefile for MStar SMC device drivers.
#
CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))
ifdef CONFIG_MSTAR_PROJECT_NAME
    CONFIG_MSTAR_PROJECT_NAME := $(subst ",,$(CONFIG_MSTAR_PROJECT_NAME))
endif

#HAL_DIRECTORY_PATH := ../../hal/$(CONFIG_MSTAR_CHIP_NAME)
#SOURCES := $(shell find $(srctree) -name '*.c')
#OBJECTS := $(addprefix $(HAL_DIRECTORY_PATH)/,$(SOURCES:%.c=%.o))

# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar2/include
EXTRA_CFLAGS      +=  -Idrivers/mstar2/drv/xc/1_4_2
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_2
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_2/tcgen/
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_2/include
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_2/include/dm2_x
EXTRA_CFLAGS      +=  -Iinclude/linux

# specific options
EXTRA_CFLAGS      +=  -DRED_LION
# files
obj-$(CONFIG_MSTAR_XC)             := mdrv-xc.o

mdrv-xc-objs    += mdrv_xc.o mdrv_xc_io.o mdrv_dlc.o mdrv_xc_proc.o
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_2/color_format_driver_hardware.o
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_2/color_format_driver.o
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_2/color_format_input.o
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_2/color_format_ip.o
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_2/mhal_dlc.o

#mdrv-xc-objs    += $(patsubst %.c,%.o,$(wildcard ../../hal/maserati/xc/*.c))
#$(info $$mdrv-xc-objs is [${mdrv-xc-objs}])
#tcgen dolby 1.4.1
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_2/tcgen/toneCurveInternalFxs.o
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_2/tcgen/toneCurve2pxx.o
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_2/tcgen/toneCurve3pxx.o
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_2/mhal_xc.o
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_2/dolby_vision_driver.o
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_2/dolby_hdr_mem.o
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_2/CdmCfgFxp.o
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_2/CdmMmg.o
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_2/CdmCmmtFxp.o
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_2/CdmUtilFxp.o
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_2/control_path_api.o
mdrv-xc-objs    += mdrv_xc_menuload.o
mdrv-xc-objs    += mdrv_xc_dynamicscaling.o
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_2/mhal_menuload.o
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_2/mhal_dynamicscaling.o
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_2/mhal_pq_adaptive.o
ifeq ($(CONFIG_MSTAR_CHIP_NAME), macan)

#tcgen
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_2/tcgen/fx_basicops.o
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_2/tcgen/fx_math.o
mdrv-xc-objs    += ../../../hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/1_4_2/dolby_hdr_mem.o

endif

