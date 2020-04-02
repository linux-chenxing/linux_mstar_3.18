#
# Makefile for MStar CMA Pool device drivers.
#

# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar2/include/sys/common
EXTRA_CFLAGS      +=  -Idrivers/mstar2/include
EXTRA_CFLAGS      +=  -Idrivers/mstar2/drv/cma_pool
EXTRA_CFLAGS      +=  -Idrivers/mstar2/drv/miu
# specific options
EXTRA_CFLAGS      +=  -Idrivers/staging/android/ion

# files
ifeq ($(CONFIG_CMA),y)
obj-$(CONFIG_MSTAR_CMAPOOL)     += mdrv-cma_pool.o
mdrv-cma_pool-objs                       += mdrv_cma_pool.o
endif
