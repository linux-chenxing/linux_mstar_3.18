

# Makefile for MStar cam_os_wrapper driver.
#

CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))
# general options
# EXTRA_CFLAGS      +=  -Idrivers/mstar/common
EXTRA_CFLAGS      +=  -Idrivers/mstar2/include
EXTRA_CFLAGS      +=  -Idrivers/mstar2/cam_os_wrapper
EXTRA_CFLAGS      +=  -Idrivers/mstar2/cam_os_wrapper/pub


# specific options

# files
obj-y += src/cam_os_wrapper.o src/cam_os_export.o src/cam_os_informal_idr.o

# export header files
EXPORT_H_FILES    += cam_os_wrapper.h
