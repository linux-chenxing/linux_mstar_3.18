#
# Makefile for MStar ISP drivers.
#


# general options
EXTRA_CFLAGS += -Idrivers/mstar/include
EXTRA_CFLAGS += -Iarch/arm/include
EXTRA_CFLAGS += -Iinclude/linux
EXTRA_CFLAGS += -Idrivers/mstar/cam_os_wrapper/pub
EXTRA_CFLAGS += -Idrivers/mstar/isp/infinity2/drv/inc
EXTRA_CFLAGS += -Idrivers/mstar/isp/infinity2/drv/pub
EXTRA_CFLAGS += -Idrivers/mstar/isp/infinity2/hal/infinity2/inc
EXTRA_CFLAGS += -Idrivers/mstar/isp/infinity2/hal/infinity2/pub
EXTRA_CFLAGS += -Idrivers/mstar/isp/infinity2/common
EXTRA_CFLAGS += -Idrivers/mstar/include/infinity2
EXTRA_CFLAGS += -Idrivers/mstar/camdriver/vif/drv/pub
# files

obj-$(CONFIG_MS_ISP_INFINITY2)  := isp.o
isp-y :=  \
					common/isp_sys_api.o \
					common/kernel.o \
					hal/infinity2/src/hal_isp.o \
					hal/infinity2/src/hal_isp_shadow.o \
					hal/infinity2/src/isp_interrupt_handler.o \
					hal/infinity2/src/hal_iq.o \
					hal/infinity2/src/hal_clkpad.o \
					drv/src/linux/drv_isp_module.o \
					drv/src/common/drv_isp_isr.o \
					drv/src/common/drv_isp.o
									#drv/src/common/drv_isp.o \
									#drv/src/linux/drv_isp_module.o
