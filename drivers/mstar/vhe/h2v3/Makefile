#
# Makefile for MStar MFE driver on SOC INFINITY.
#

# general options
EXTRA_CFLAGS += -Idrivers/mstar/include
EXTRA_CFLAGS += -Idrivers/mstar/include/infinity3
EXTRA_CFLAGS += -Idrivers/mstar/mhve
EXTRA_CFLAGS += -Idrivers/mstar/vhe
EXTRA_CFLAGS += -Idrivers/mstar/vhe/h2v3

# files
obj-$(CONFIG_MS_VHE) += mdrv_h2v3.o

mdrv_h2v3-y :=	\
	h2v3_ops.o	\
	h2v3_ios.o	\
	h2v3_rqc.o	\
	h2v3_reg.o	\
	h265_enc.o	\
