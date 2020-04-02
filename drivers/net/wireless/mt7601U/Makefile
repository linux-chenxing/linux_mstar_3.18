RT28xx_DIR := drivers/net/wireless/mt7601U
include drivers/net/wireless/mt7601U/Makefile.inc

include $(RT28xx_DIR)/os/linux/config.mk

EXTRA_CFLAGS       +=$(WFLAGS)
EXTRA_CFLAGS      +=  -I$(RT28xx_DIR)/include

include $(RT28xx_DIR)/os/linux/config.mk


obj-$(CONFIG_MT7601U)    += os/linux/
LINUX:
	gcc-linux-gnueabihf-Strip -g os/linux/mt7601Uap.ko
