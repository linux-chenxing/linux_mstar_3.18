#
# Makefile for MStar SD card device drivers.
#
CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))
ifdef CONFIG_MSTAR_PROJECT_NAME
    CONFIG_MSTAR_PROJECT_NAME := $(subst ",,$(CONFIG_MSTAR_PROJECT_NAME))
endif


# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar2/include
EXTRA_CFLAGS      +=  -Idrivers/mstar2/drv/sdmmc/inc
EXTRA_CFLAGS      +=  -Idrivers/mstar2/drv/sdmmc/inc/config
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/sdmmc/

# specific options
EXTRA_CFLAGS      +=  -DMSOS_TYPE_LINUX


ifeq ($(CONFIG_MSTAR_CHIP_NAME),$(filter $(CONFIG_MSTAR_CHIP_NAME),clippers muji monet manhattan kano messi maserati))

obj-y             += ms_sdmmc_lnx.o 
obj-y	            += src5/hal_card_common.o
obj-y	            += src5/hal_card_regs.o
obj-y	            += src5/hal_card_timer.o
obj-y	            += src5/hal_card_intr.o
obj-y	            += src5/hal_sdmmc.o
obj-y	            += ../../hal/$(CONFIG_MSTAR_CHIP_NAME)/sdmmc/sd_platform.o

else

obj-y             += ms_sdmmc_lnx.o 
obj-y	            += src/hal_card_common.o
obj-y	            += src/hal_card_regs.o
obj-y	            += src/hal_card_timer.o
obj-y	            += src/hal_card_intr.o
obj-y	            += src/hal_sdmmc.o
obj-y	            += ../../hal/$(CONFIG_MSTAR_CHIP_NAME)/sdmmc/sd_platform.o

endif


