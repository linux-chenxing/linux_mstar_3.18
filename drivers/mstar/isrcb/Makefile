#
# Makefile for interrupt router driver
#

CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))
# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar/cam_os_wrapper/pub
EXTRA_CFLAGS      +=  -Idrivers/mstar/isrcb/drv/pub

# files
obj-$(CONFIG_MS_ISRCB_INFINITY2) := isrcb.o
isrcb-y := drv/src/common/drv_isrcb.o\
           drv/src/linux/drv_isrcb_module.o
