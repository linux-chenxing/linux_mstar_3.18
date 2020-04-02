#
# Makefile for MStar MBX device drivers.
CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))
ifdef CONFIG_MSTAR_PROJECT_NAME
    CONFIG_MSTAR_PROJECT_NAME := $(subst ",,$(CONFIG_MSTAR_PROJECT_NAME))
endif


# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar2/include
EXTRA_CFLAGS      +=  -Idrivers/mstar2/drv/mbx
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/mbx
EXTRA_CFLAGS      +=  -DRED_LION

# specific options
EXTRA_CFLAGS      +=

# files
obj-$(CONFIG_MSTAR_MBX)	+= mdrv-mbx.o
mdrv-mbx-objs		+= mdrv_mbx_io.o mdrv_mbx.o mdrv_mbx_msgpool.o

ifdef CONFIG_COMPAT
mdrv-mbx-objs		+= mdrv_mbx_compat.o
endif

mdrv-mbx-objs		+=  mapi_mbx.o
mdrv-mbx-objs		+= ../../hal/$(CONFIG_MSTAR_CHIP_NAME)/mbx/mhal_mbx.o
mdrv-mbx-objs		+= ../../hal/$(CONFIG_MSTAR_CHIP_NAME)/mbx/mhal_mbx_interrupt.o

