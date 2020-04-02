#
# Makefile for MStar GFLIP HAL drivers.
#

# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar2/include
EXTRA_CFLAGS      +=  -Idrivers/mstar2/drv/ldm
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/ldm

EXTRA_CFLAGS      +=  -DRED_LION

# specific options
EXTRA_CFLAGS      +=

# files
obj-y             += mhal_ldm.o
