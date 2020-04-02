#
# Makefile for MStar cam_drv_poll driver.
#

CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))
# general options
# EXTRA_CFLAGS      +=  -Idrivers/mstar/common
EXTRA_CFLAGS      +=  -Idrivers/mstar/include
EXTRA_CFLAGS      +=  -Idrivers/mstar/cam_drv_poll/pub


# specific options

# files
obj-y += src/cam_drv_poll.o

# export header files
EXPORT_H_FILES    += cam_drv_poll.h
