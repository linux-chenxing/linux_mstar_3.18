#
# Makefile for MStar XC HAL drivers.
#

# general options
#EXTRA_CFLAGS      +=  -Idrivers/mstar/common
EXTRA_CFLAGS      +=  -Idrivers/mstar/include
EXTRA_CFLAGS      +=  -Idrivers/mstar/xc/drv
EXTRA_CFLAGS      +=  -Idrivers/mstar/xc/drv/$(CONFIG_MSTAR_CHIP_NAME)

EXTRA_CFLAGS      +=  -DRED_LION

# specific options
EXTRA_CFLAGS      +=

# files
obj-y             += mhal_xc.o

#ifdef CONFIG_MSTAR_XC_HDR_SUPPORT
# Dolby Vision
obj-y             += dolby_vision_driver.o
obj-y             += dolby_hdr_mem.o
# Color Format Driver
obj-y             += color_format_ip.o color_format_driver_hardware.o color_format_driver.o color_format_input.o
# TCH
obj-y             += technicolor_driver.o tc_hdr_mem.o
#menuload
obj-y             += mhal_menuload.o
#endif
