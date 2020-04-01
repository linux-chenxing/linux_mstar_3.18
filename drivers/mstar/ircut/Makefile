ifdef CONFIG_ARCH_INFINITY
    CONFIG_MSTAR_CHIP_NAME := infinity
endif
EXTRA_CFLAGS      +=  -Idrivers/mstar/include
EXTRA_CFLAGS      +=  -Idrivers/mstar/include/$(CONFIG_MSTAR_CHIP_NAME)

obj-$(CONFIG_MS_IRCUT)	+= ms_ircut.o
