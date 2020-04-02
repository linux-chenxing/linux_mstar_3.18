#
# Makefile for MStar Sata Host device drivers.
#
CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))
ifdef CONFIG_MSTAR_PROJECT_NAME
    CONFIG_MSTAR_PROJECT_NAME := $(subst ",,$(CONFIG_MSTAR_PROJECT_NAME))
endif

# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar/include
EXTRA_CFLAGS      +=  -Idrivers/mstar/sata_host
EXTRA_CFLAGS      +=  -Idrivers/mstar/sata_host/$(CONFIG_MSTAR_CHIP_NAME)
EXTRA_CFLAGS      +=  -Iinclude/linux
EXTRA_CFLAGS      +=  -Idrivers/mstar/driver/hal/infinity2/scl/inc/drv/linux

# specific options
EXTRA_CFLAGS      +=

# files
obj-$(CONFIG_MS_SATA_HOST)	:= mdrv-sata-host.o
mdrv-sata-host-y		:= mdrv_sata_host.o
mdrv-sata-host-y        += $(CONFIG_MSTAR_CHIP_NAME)/mhal_sata_host.o
obj-$(CONFIG_MS_SATA_BENCH_TEST) += bench_test/
