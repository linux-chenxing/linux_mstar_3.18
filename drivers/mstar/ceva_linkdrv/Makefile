#
# Makefile for MStar Infinity3 IVE device drivers.

# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar/ceva_linkdrv
EXTRA_CFLAGS      +=  -Idrivers/mstar/include

ccflags-$(CONFIG_MS_XM6)		+= -DMSOS_TYPE_LINUX_KERNEL

# files
obj-$(CONFIG_MS_XM6) += ceva_link.o
ceva_link-y := ceva_linkdrv_xm6.o   \
               ceva_linkdrv.o    \
               protected_mem_db.o \
			   hal_ceva.o \
			   boot_loader.o
