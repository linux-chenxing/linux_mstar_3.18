#
# Makefile for MStar MMA HEAP  drivers.
#

# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar2/include/sys/common
EXTRA_CFLAGS      +=  -Idrivers/mstar2/include
EXTRA_CFLAGS      +=  -Idrivers/mstar2/drv/miu
# specific options
EXTRA_CFLAGS      +=  -Idrivers/staging/android/ion

# files
obj-$(CONFIG_MSTAR_MMAHEAP)     += mdrv-mma_heap.o
mdrv-mma_heap-objs                       += mdrv_mma_heap.o region_substract.o
