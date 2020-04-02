#
# Makefile for the kernel IrDA char device drivers.
#
CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))
ifdef CONFIG_MSTAR_PROJECT_NAME
    CONFIG_MSTAR_PROJECT_NAME := $(subst ",,$(CONFIG_MSTAR_PROJECT_NAME))
endif

CHIP    = titania

EXTRA_CFLAGS += -Idrivers/mstar2/hal/mbx/drv
EXTRA_CFLAGS += -Idrivers/mstar2/include
EXTRA_CFLAGS += -Idrivers/mstar2/drv/ir_mirc
EXTRA_CFLAGS += -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/ir
EXTRA_CFLAGS +=  -Iinclude/linux

obj-$(CONFIG_MSTAR_IR_REFACTOR) += mstar_ir.o \
									ir_core.o \
									protocols/ir-nec-decoder.o \
									protocols/ir-rca-decoder.o \
									protocols/ir-p7051-decoder.o \
									protocols/ir-rc5-decoder.o \
									protocols/ir-rc6-decoder.o \
									protocols/ir-toshiba-decoder.o
obj-y += keymaps/
obj-$(CONFIG_IR_DYNAMIC_CONFIG) += ir_dynamic_config/
