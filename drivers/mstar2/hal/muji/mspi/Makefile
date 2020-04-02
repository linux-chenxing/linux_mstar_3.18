#
# Makefile for MStar MSPI HAL drivers.
#

# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar2/include
EXTRA_CFLAGS      +=  -Idrivers/mstar2/drv/mspi
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/mspi

EXTRA_CFLAGS      +=  -DRED_LION

# specific options
EXTRA_CFLAGS      +=

# files
obj-y             += mhal_mspi.o
