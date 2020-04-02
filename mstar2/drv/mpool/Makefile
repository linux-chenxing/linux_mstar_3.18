#
# Makefile for MStar Malloc device drivers.
#
CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))
ifdef CONFIG_MSTAR_PROJECT_NAME
    CONFIG_MSTAR_PROJECT_NAME := $(subst ",,$(CONFIG_MSTAR_PROJECT_NAME))
endif

# general options
#EXTRA_CFLAGS      +=  -Idrivers/mstar/common
EXTRA_CFLAGS      +=  -Idrivers/mstar2/include
EXTRA_CFLAGS      +=  -Idrivers/mstar2/drv/mpool
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/mpool
#ifdef CONFIG_MSTAR_DLMALLOC
EXTRA_CFLAGS      +=  -Idrivers/mstar2/drv/dlmalloc
#endif

# specific options
EXTRA_CFLAGS      +=

# files
obj-$(CONFIG_MSTAR_MPOOL)	+= mdrv-mpool.o
mdrv-mpool-objs			+= mdrv_mpool.o

ifdef CONFIG_MIPS
mdrv-mpool-objs			+= mdrv_cache.o
endif
