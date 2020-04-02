#
# Makefile for MStar CPU_calibrating device drivers.
#

ifeq ($(MAKE_TYPE), MODULE_STANDALONE)
include $(TOP_DIR)/modules.mk
endif

CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))

# general options
EXTRA_CFLAGS += -Idrivers/mstar2/include

# files
obj-y	+= mdrv_dlmalloc.o

#export header files
