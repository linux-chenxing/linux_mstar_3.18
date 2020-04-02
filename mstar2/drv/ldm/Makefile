#
# Makefile for MStar localdimming device drivers.
CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))
ifdef CONFIG_MSTAR_PROJECT_NAME
    CONFIG_MSTAR_PROJECT_NAME := $(subst ",,$(CONFIG_MSTAR_PROJECT_NAME))
endif

EXTRA_CFLAGS      +=  -Idrivers/mstar2/include
EXTRA_CFLAGS      += -Idrivers/mstar2/drv/ldm/include
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/ldm/
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/mspi/
EXTRA_CFLAGS 	  += -Idrivers/mstar2/drv/mbx


EXTRA_CFLAGS      +=  -DRED_LION


# build moudle

ifeq ($(CONFIG_HAS_LD),y)
obj-$(CONFIG_HAS_LDM) += mdrv_ldm.o
mdrv_ldm-objs +=	mdrv_ldm_init.o
mdrv_ldm-objs +=	mdrv_ldm_io.o
mdrv_ldm-objs +=	mdrv_ldm_parse.o
mdrv_ldm-objs +=	mdrv_ldm_algorithm.o
mdrv_ldm-objs +=	mdrv_ldm_interrupt.o
mdrv_ldm-objs += ../../hal/$(CONFIG_MSTAR_CHIP_NAME)/ldm/mhal_ldm.o
endif



#for DMA Mode localdimming

ifeq ($(CONFIG_HAS_LD_DMA_MODE),y)
obj-$(CONFIG_HAS_LDM) += mdrv_ldm.o
mdrv_ldm-objs +=	mdrv_ldm_init_v2.o
mdrv_ldm-objs +=	mdrv_ldm_io.o
mdrv_ldm-objs +=	mdrv_ldm_parse_v2.o
mdrv_ldm-objs +=	mdrv_ldm_algorithm_v2.o
mdrv_ldm-objs +=	mdrv_ldm_interrupt.o
mdrv_ldm-objs +=	mdrv_ldm_cus.o
mdrv_ldm-objs +=	mdrv_ldm_dma.o
mdrv_ldm-objs += ../../drv/mspi/mdrv_mspi.o
mdrv_ldm-objs += ../../hal/$(CONFIG_MSTAR_CHIP_NAME)/ldm/mhal_ldm_dma.o
mdrv_ldm-objs += ../../hal/$(CONFIG_MSTAR_CHIP_NAME)/ldm/mhal_ldm.o
mdrv_ldm-objs += ../../hal/$(CONFIG_MSTAR_CHIP_NAME)/mspi/mhal_mspi.o
endif

