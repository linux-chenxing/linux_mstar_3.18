#
# Makefile for MStar Infinity3 IVE device drivers.

# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar/cwa
EXTRA_CFLAGS      +=  -Idrivers/mstar/include

ccflags-$(CONFIG_MS_CWA)		+= -DMSOS_TYPE_LINUX_KERNEL

# files
obj-$(CONFIG_MS_CWA) += mstar_cwa.o
mstar_cwa-y := mdrv_cwa.o   \
               hal_cwa.o    \
			   hal_ceva.o   \
			   hal_clk.o	\
			   hal_debug.o	\
               drv_cwa.o    			   
