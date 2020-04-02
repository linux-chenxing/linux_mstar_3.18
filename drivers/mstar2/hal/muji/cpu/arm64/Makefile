#
# Makefile for the linux kernel.
#

EXTRA_CFLAGS      +=  -Idrivers/irqchip
EXTRA_CFLAGS      +=  -Idrivers/mstar2/include

obj-y := chip_irq.o chip_arch.o chip_early_printk.o
obj-$(CONFIG_SMP)			+= platsmp.o
obj-y += platform.o

obj-$(CONFIG_OC_ETM)			+= emu.o
obj-$(CONFIG_MP_MSTAR_STR_BASE)+= sleep.o pm.o
