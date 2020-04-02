#
# Makefile for MStar MMA HEAP  drivers.
#

# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar/include/sys/common
EXTRA_CFLAGS      +=  -Idrivers/mstar/include
EXTRA_CFLAGS      +=  -Idrivers/mstar/mma_heap
EXTRA_CFLAGS      +=  -Idrivers/mstar/miu
EXTRA_CFLAGS      +=  -Idrivers/mstar/cpu/include
# specific options
EXTRA_CFLAGS      +=  -Idrivers/staging/android/ion

# files
obj-$(CONFIG_MSTAR_MMAHEAP)     += mdrv-mma_heap.o
mdrv-mma_heap-objs                       += mdrv_mma_heap.o region_substract.o
