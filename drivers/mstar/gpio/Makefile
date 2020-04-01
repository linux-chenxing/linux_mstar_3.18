#
# Makefile for MStar GPIO device drivers.
#
ifdef CONFIG_ARCH_CEDRIC
    CONFIG_MSTAR_CHIP_NAME := cedric
endif
ifdef CONFIG_ARCH_INFINITY
    CONFIG_MSTAR_CHIP_NAME := infinity
endif

# general options
##EXTRA_CFLAGS      +=  -Idrivers/mstar/common
EXTRA_CFLAGS      +=  -Idrivers/mstar/include
EXTRA_CFLAGS      +=  -Idrivers/mstar/gpio
EXTRA_CFLAGS      +=  -Idrivers/mstar/gpio/$(CONFIG_MSTAR_CHIP_NAME)
EXTRA_CFLAGS      +=  -Idrivers/mstar/include/$(CONFIG_MSTAR_CHIP_NAME)

# specific options
EXTRA_CFLAGS      +=  -DMSOS_TYPE_LINUX

# files
obj-$(CONFIG_MS_GPIO_CEDRIC) += mdrv_gpio.o mdrv_gpio_io.o $(CONFIG_MSTAR_CHIP_NAME)/mhal_gpio.o
obj-$(CONFIG_MS_GPIO_INFINITY) +=  mdrv_gpio.o mdrv_gpio_io.o $(CONFIG_MSTAR_CHIP_NAME)/mhal_gpio.o
obj-$(CONFIG_MS_SW_I2C_INFINITY) += ms_gpioi2c.o mdrv_sw_iic.o
