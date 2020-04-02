#
# Makefile for MStar XC HAL drivers.
#

# general options
#EXTRA_CFLAGS      +=  -Idrivers/mstar/common
EXTRA_CFLAGS      +=  -Idrivers/mstar/include
EXTRA_CFLAGS      +=  -Idrivers/mstar/xc/drv
EXTRA_CFLAGS      +=  -Idrivers/mstar/xc/drv/$(CONFIG_MSTAR_CHIP_NAME)
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/include/
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/tcgen/
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/xc/include/dm2_x

EXTRA_CFLAGS      +=  -DRED_LION

# specific options
EXTRA_CFLAGS      +=

# files
obj-y             += mhal_xc.o dolby_hdr_mem.o

# Dolby Vision
obj-y             += dolby_vision_driver.o
obj-y             += CdmCfgFxp.o CdmMmg.o CdmCmmtFxp.o CdmUtilFxp.o control_path_api.o

# Color Format Driver
obj-y             += color_format_ip.o color_format_driver_hardware.o color_format_driver.o color_format_input.o

# tcgen
obj-y             += ./tcgen/basicops/fx_basicops.o ./tcgen/fx_math.o
# tcgen dolby 1.4.1
obj-y             += ./tcgen/toneCurveInternalFxs.o ./tcgen/toneCurve2pxx.o  
#obj-y             += ./tcgen/source/toneCurveLUT.o ./tcgen/source/lc_2x1d_luts.o ./tcgen/source/saturation_table.o

#menuload
obj-y             += mhal_menuload.o

#pq adaptive tuning
obj-y             += mhal_pq_adaptive.o
