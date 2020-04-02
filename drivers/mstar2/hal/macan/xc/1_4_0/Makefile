#
# Makefile for MStar XC HAL drivers.
#

# general options
#EXTRA_CFLAGS      +=  -Idrivers/mstar/common
EXTRA_CFLAGS      +=  -Idrivers/mstar/include
EXTRA_CFLAGS      +=  -Idrivers/mstar/xc/drv
EXTRA_CFLAGS      +=  -Idrivers/mstar/xc/drv/$(CONFIG_MSTAR_CHIP_NAME)
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/tcgen/include/
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/tcgen/basicops/

EXTRA_CFLAGS      +=  -DRED_LION

# specific options
EXTRA_CFLAGS      +=

# files
obj-y             += mhal_xc.o dolby_hdr_mem.o

# Dolby Vision
obj-y             += dolby_vision_driver.o

# Color Format Driver
obj-y             += color_format_ip.o color_format_driver_hardware.o color_format_driver.o color_format_input.o

# tcgen
obj-y             += ./tcgen/basicops/fx_basicops.o ./tcgen/basicops/fx_math.o
obj-y             += ./tcgen/source/toneCurveLUT.o ./tcgen/source/lc_2x1d_luts.o ./tcgen/source/saturation_table.o

#menuload
obj-y             += mhal_menuload.o

#pq adaptive tuning
obj-y             += mhal_pq_adaptive.o
