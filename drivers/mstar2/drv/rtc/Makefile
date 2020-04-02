#
# Makefile for MStar IIC device drivers.
#

ifeq ($(MAKE_TYPE), MODULE_STANDALONE)
include $(TOP_DIR)/modules.mk
endif

CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))

# general options
# EXTRA_CFLAGS      +=  -Idrivers/mstar/common
EXTRA_CFLAGS      +=  -Idrivers/mstar/include
EXTRA_CFLAGS      +=  -Idrivers/mstar/rtc
EXTRA_CFLAGS      +=  -Idrivers/mstar/rtc/hal/$(CONFIG_MSTAR_CHIP_NAME)

# specific options
EXTRA_CFLAGS      +=  -DMSOS_TYPE_LINUX

# files
obj-$(CONFIG_MSTAR_RTC)	+= mdrv-rtc.o
mdrv-rtc-objs			+= ./hal/$(CONFIG_MSTAR_CHIP_NAME)/mhal_rtc.o
mdrv-rtc-objs			+= mdrv_rtc.o mdrv_rtc_io.o

#mdrv-rtc-objs			+= ./hal/$(CONFIG_MSTAR_CHIP_NAME)/mhal_rtc.o

# export header files
EXPORT_H_FILES    += mdrv_rtc_io.h
