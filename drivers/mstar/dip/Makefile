#
# Makefile for MStar dip device drivers.

# general options
EXTRA_CFLAGS                +=  -Idrivers/mstar/include
EXTRA_CFLAGS                +=  -Idrivers/mstar/camdriver/include

#----------------------- drv -----------------------
EXTRA_CFLAGS                +=  -Idrivers/mstar/dip
EXTRA_CFLAGS                +=  -Idrivers/mstar/dip/drv/inc
EXTRA_CFLAGS                +=  -Idrivers/mstar/dip/drv/pub
#----------------------- hal -----------------------
EXTRA_CFLAGS                +=  -Idrivers/mstar/dip/hal/inc
EXTRA_CFLAGS                +=  -Idrivers/mstar/dip/hal/pub

# specific options
EXTRA_CFLAGS                +=

ccflags-y		+= -DMSOS_TYPE_LINUX_KERNEL
ccflags-y		+= -DDIP_REG_DUMP
ccflags-y		+= -DDIP_UT_MFDEC_SIM_MODE
ccflags-y		+= -DDIP_UT_ALLOC_MEM_FOR_3DDI
#ccflags-y		+= -DDIP_UT_ENABLE_RIU
#ccflags-y		+= -DDIP_UT_WO_IRQ

#--------------------- sources ---------------------
obj-$(CONFIG_MS_DIP)   += mstar_dip.o
mstar_dip-y :=  mdrv_dip_io.o           \
                hal/src/mhal_dip.o      \
                drv/src/mdrv_divp.o
