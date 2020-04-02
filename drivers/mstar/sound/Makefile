
EXTRA_CFLAGS      +=  -Idrivers/mstar/include

#
# Makefile for MStar SCL drivers.
#
EXTRA_CFLAGS      +=  -Idrivers/mstar/sound/drv/pub
EXTRA_CFLAGS      +=  -Idrivers/mstar/sound/drv/inc
EXTRA_CFLAGS      +=  -Idrivers/mstar/sound/hal/pub
EXTRA_CFLAGS      +=  -Idrivers/mstar/sound/drv/pub/linux

# iNfinity2 Platform Support

obj-$(CONFIG_MS_SOUND_INFINITY2) +=  hal/src/hal_audio.o
obj-$(CONFIG_MS_SOUND_INFINITY2) +=  drv/src/common/mhal_audio.o drv/src/linux/drv_audio.o drv/src/linux/drv_audio_module.o
