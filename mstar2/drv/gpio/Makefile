#
# Makefile for MStar GPIO device drivers.
#

ifeq ($(MAKE_TYPE), MODULE_STANDALONE)
include $(TOP_DIR)/modules.mk
endif

CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))

# general options
#EXTRA_CFLAGS      +=  -Idrivers/mstar/common
EXTRA_CFLAGS      +=  -Idrivers/mstar2/include
EXTRA_CFLAGS      +=  -Idrivers/mstar2/gpio
EXTRA_CFLAGS      +=  -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/gpio

# specific options
EXTRA_CFLAGS      +=  -DTITANIA
EXTRA_CFLAGS      +=  -DMSOS_TYPE_LINUX

# files
obj-$(CONFIG_MSTAR_GPIO)	+= mdrv-gpio.o
mdrv-gpio-objs				+= mdrv_gpio.o mdrv_gpio_io.o
mdrv-gpio-objs				+= ../../hal/$(CONFIG_MSTAR_CHIP_NAME)/gpio/mhal_gpio.o

#export header files
#EXPORT_H_FILES    += mdrv_gpio_io.h
