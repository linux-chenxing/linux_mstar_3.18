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
EXTRA_CFLAGS += -Idrivers/mstar2/drv/ir
EXTRA_CFLAGS += -Idrivers/mstar2/hal/$(CONFIG_MSTAR_CHIP_NAME)/ir
EXTRA_CFLAGS      +=  -Iinclude/linux

ifdef CONFIG_MSTAR_IR_MULTIPLE_MODE
obj-$(CONFIG_MSTAR_IR) += mdrv-ir.o
mdrv-ir-objs		+= mdrv_ir_multiple.o
mdrv-ir-objs            += mdrv_ir_io.o
else
obj-$(CONFIG_MSTAR_IR) += mdrv-ir.o
mdrv-ir-objs		+= mdrv_ir.o
mdrv-ir-objs		+= IR_PROTOCOL_INSTALL.o
mdrv-ir-objs		+= IR_PROTOCOL_Common.o
mdrv-ir-objs            += mdrv_ir_io.o
endif
