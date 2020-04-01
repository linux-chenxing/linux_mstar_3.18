#
# Makefile for MStar MFE driver on SOC INFINITY.
#

# general options
EXTRA_CFLAGS += -Idrivers/mstar/include
EXTRA_CFLAGS += -Idrivers/mstar/mhve
EXTRA_CFLAGS += -Idrivers/mstar/mfev5
EXTRA_CFLAGS += -Idrivers/mstar/mfev5/mfe6

# files
obj-$(CONFIG_MS_MFEV5) += mdrv_mfe6.o

mdrv_mfe6-y :=		\
	mfe.o			\
	mfe_asic.o		\
	mfe_h264.o		\
	mfe_rctl.o		\
	mfe_msb2.o		\
