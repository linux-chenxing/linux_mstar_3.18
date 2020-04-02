MS_MIPI_DSI_ARCH_NAME=infinity2
MS_MIPI_DSI_OS_NAME=linux

EXTRA_CFLAGS      +=  -Idrivers/mstar/include
EXTRA_CFLAGS      +=  -Idrivers/mstar/include/infinity2
EXTRA_CFLAGS      +=  -Idrivers/mstar/cam_os_wrapper/pub

#
# Makefile for MStar MIPI TX DSI drivers.
#


# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar/driver/drv/mipi_dsi/inc
EXTRA_CFLAGS      +=  -Idrivers/mstar/driver/hal/$(MS_MIPI_DSI_ARCH_NAME)/mipi_dsi/inc
  # MIPI TX DSI bug: must trigger TTL to keep MIPI TX DSI working
EXTRA_CFLAGS      +=  -Idrivers/mstar/driver/drv/pnl/inc


EXTRA_CFLAGS += -Werror

# specific options
#
#EXTRA_CFLAGS      +=  -DSCLOS_TYPE_LINUX_KERNEL



obj-$(CONFIG_MS_MIPI_TX_DSI)   += mdrv_mipi_dsi.o

mdrv_mipi_dsi-y :=     ./src/halMIPIDsi.o \
                       ../../../drv/mipi_dsi/src/mdrv_mipi_dsi_io.o
 			#../../../drv/mipi_dsi/src/drv_mipi_dsi.o \                
