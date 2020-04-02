#
# Makefile for the linux kernel.
#

EXTRA_CFLAGS      +=  -Idrivers/irqchip
EXTRA_CFLAGS      +=  -Iarch/arm/arm-boards/$(CONFIG_MSTAR_CHIP_NAME)/board
EXTRA_CFLAGS      +=  -Iarch/arm/include

obj-y := chip_irq.o chip_arch.o chip_timer.o chip_early_printk.o
obj-$(CONFIG_SMP)			+= platsmp.o
obj-y += platform.o

obj-$(CONFIG_OC_ETM)			+= emu.o
obj-$(CONFIG_MP_MSTAR_STR_BASE)+= sleep.o pm.o sleep_helper.o sleep_helper_asm.o
