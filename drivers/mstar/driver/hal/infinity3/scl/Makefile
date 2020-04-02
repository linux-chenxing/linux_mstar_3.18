MS_SCL_ARCH_NAME=infinity3
MS_SCL_OS_NAME=linux
MS_SCL_SRC_PATH=hal/$(MS_SCL_ARCH_NAME)/scl/src
EXTRA_CFLAGS      +=  -Idrivers/mstar/include
EXTRA_CFLAGS      +=  -Idrivers/mstar/include/infinity3
#
# Makefile for MStar SCL drivers.
#


# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar/driver/drv/scl/pub
EXTRA_CFLAGS      +=  -Idrivers/mstar/driver/drv/scl/pub/$(MS_SCL_OS_NAME)
EXTRA_CFLAGS      +=  -Idrivers/mstar/driver/drv/scl/inc
EXTRA_CFLAGS      +=  -Idrivers/mstar/driver/drv/scl/inc/pq
EXTRA_CFLAGS      +=  -Idrivers/mstar/driver/drv/scl/inc/$(MS_SCL_OS_NAME)
EXTRA_CFLAGS      +=  -Idrivers/mstar/driver/hal/$(MS_SCL_ARCH_NAME)/scl/inc
EXTRA_CFLAGS      +=  -Idrivers/mstar/driver/hal/$(MS_SCL_ARCH_NAME)/scl/inc/drv
EXTRA_CFLAGS      +=  -Idrivers/mstar/driver/hal/$(MS_SCL_ARCH_NAME)/scl/inc/pq
EXTRA_CFLAGS      +=  -Idrivers/mstar/driver/hal/$(MS_SCL_ARCH_NAME)/scl/inc/drv/$(MS_SCL_OS_NAME)
EXTRA_CFLAGS      +=  -Idrivers/mstar/driver/hal/$(MS_SCL_ARCH_NAME)/scl/inc/hal
EXTRA_CFLAGS      +=  -Idrivers/mstar/driver/hal/$(MS_SCL_ARCH_NAME)/scl/inc/hal/$(MS_SCL_OS_NAME)

EXTRA_CFLAGS += -Werror

# specific options
#
EXTRA_CFLAGS      +=  -DSCLOS_TYPE_LINUX_KERNEL

ccflags-$(CONFIG_ARCH_CEDRIC)		+= -DC3_SIM


obj-$(CONFIG_MS_SCL_INFINITY3)   += mdrv_scl.o

mdrv_scl-y := 	  src/hal/hal_scl.o   \
                  src/hal/hal_scl_pnl.o \
                  src/hal/hal_scl_irq.o \
                  src/hal/hal_utility.o\
                  src/hal/hal_scl_cmdq.o \
                  src/hal/hal_scl_hvsp.o \
                  src/hal/hal_scl_dma.o \
                  src/hal/hal_scl_vip.o \
                  src/pq/hal_scl_pq.o \
                  src/pq/hal_scl_pq_qualitymap_main.o \
                  src/drv/drv_scl_cmdq.o \
                  src/drv/drv_scl_hvsp.o \
                  src/drv/drv_scl_dma.o \
                  src/drv/drv_scl_vip.o \
                  src/drv/drv_scl_pnl.o \
                  src/drv/drv_scl_hvsp_m.o \
                  src/drv/drv_scl_dma_m.o \
                  src/drv/drv_scl_pnl_m.o \
                  src/drv/drv_scl_vip_m.o \
                  src/drv/drv_scl_multiinst_m.o \
                  src/drv/drv_scl_hvsp_io_wrapper.o \
                  src/drv/drv_scl_dma_io_wrapper.o \
                  src/drv/drv_scl_vip_io_wrapper.o \
                  src/drv/drv_scl_pnl_io_wrapper.o \
                  src/drv/$(MS_SCL_OS_NAME)/drv_scl_irq.o \
                  ../../../drv/scl/src/pq/drv_scl_pq.o \
                  ../../../drv/scl/src/pq/drv_scl_pq_bin.o \
                  ../../../drv/scl/src/$(MS_SCL_OS_NAME)/drv_scl_os.o \
                  ../../../drv/scl/src/$(MS_SCL_OS_NAME)/drv_scl_hvsp1_ioctl.o\
                  ../../../drv/scl/src/$(MS_SCL_OS_NAME)/drv_scl_hvsp2_ioctl.o \
                  ../../../drv/scl/src/$(MS_SCL_OS_NAME)/drv_scl_hvsp3_ioctl.o \
                  ../../../drv/scl/src/$(MS_SCL_OS_NAME)/drv_scl_dma1_ioctl.o \
                  ../../../drv/scl/src/$(MS_SCL_OS_NAME)/drv_scl_dma2_ioctl.o \
                  ../../../drv/scl/src/$(MS_SCL_OS_NAME)/drv_scl_dma3_ioctl.o \
                  ../../../drv/scl/src/$(MS_SCL_OS_NAME)/drv_scl_dma4_ioctl.o \
                  ../../../drv/scl/src/$(MS_SCL_OS_NAME)/drv_scl_pnl_ioctl.o \
                  ../../../drv/scl/src/$(MS_SCL_OS_NAME)/drv_scl_vip_ioctl.o \
                  ../../../drv/scl/src/$(MS_SCL_OS_NAME)/drv_scl_dma_ioctl_parse.o \
                  ../../../drv/scl/src/$(MS_SCL_OS_NAME)/drv_scl_hvsp_ioctl_parse.o \
                  ../../../drv/scl/src/$(MS_SCL_OS_NAME)/drv_scl_pnl_ioctl_parse.o


