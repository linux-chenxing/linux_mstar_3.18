#
# Makefile for MStar GPIO device drivers.
#

#ifeq ($(MAKE_TYPE), MODULE_STANDALONE)
#include $(TOP_DIR)/modules.mk
#endif

# CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))

# general options
#EXTRA_CFLAGS      +=  -Idrivers/mstar/common
#EXTRA_CFLAGS      +=  -Idrivers/mstar/include
#EXTRA_CFLAGS      +=  -Idrivers/mstar/gpio_chicago
#EXTRA_CFLAGS      +=  -Idrivers/mstar/gpio_chicago/chicago
EXTRA_CFLAGS      +=  -Idrivers/mstar/include
EXTRA_CFLAGS      +=  -Idrivers/infinity/pwm/
EXTRA_CFLAGS      +=  -Iinclude/linux

# specific options
EXTRA_CFLAGS      +=  -DTITANIA
EXTRA_CFLAGS      +=  -DMSOS_TYPE_LINUX

# files
obj-$(CONFIG_MS_PWM)	+= ms_pwm.o
ms_pwm-y          += mdrv_pwm.o mhal_pwm.o

