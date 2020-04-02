MS_SCL_ARCH_NAME=infinity2
MS_SCL_OS_NAME=linux
MS_SCL_SRC_PATH=hal/$(MS_SCL_ARCH_NAME)/mgwin/src

EXTRA_CFLAGS      +=  -Idrivers/mstar/include
EXTRA_CFLAGS      +=  -Idrivers/mstar/include/infinity3
#
# Makefile for MStar SCL drivers.
#


# general options
EXTRA_CFLAGS      +=  -Idrivers/mstar/driver/drv/MI_HAL/pub
EXTRA_CFLAGS      +=  -Idrivers/mstar/driver/drv/mgwin/pub
EXTRA_CFLAGS      +=  -Idrivers/mstar/driver/drv/mgwin/pub/$(MS_SCL_OS_NAME)
EXTRA_CFLAGS      +=  -Idrivers/mstar/driver/drv/mgwin/inc
EXTRA_CFLAGS      +=  -Idrivers/mstar/driver/drv/mgwin/inc/$(MS_SCL_OS_NAME)
EXTRA_CFLAGS      +=  -Idrivers/mstar/driver/drv/scl/inc/$(MS_SCL_OS_NAME)
EXTRA_CFLAGS      +=  -Idrivers/mstar/driver/hal/$(MS_SCL_ARCH_NAME)/mgwin/inc
EXTRA_CFLAGS      +=  -Idrivers/mstar/driver/hal/$(MS_SCL_ARCH_NAME)/mgwin/inc/drv
EXTRA_CFLAGS      +=  -Idrivers/mstar/driver/hal/$(MS_SCL_ARCH_NAME)/mgwin/inc/drv/$(MS_SCL_OS_NAME)
EXTRA_CFLAGS      +=  -Idrivers/mstar/driver/hal/$(MS_SCL_ARCH_NAME)/mgwin/inc/hal
EXTRA_CFLAGS      +=  -Idrivers/mstar/driver/hal/$(MS_SCL_ARCH_NAME)/mgwin/inc/hal/$(MS_SCL_OS_NAME)
EXTRA_CFLAGS      +=  -Idrivers/mstar/cam_os_wrapper/pub
EXTRA_CFLAGS += -Werror

# specific options
#
EXTRA_CFLAGS      +=  -DSCLOS_TYPE_LINUX_KERNEL

obj-$(CONFIG_MS_MGWIN_INFINITY2)   += mdrv_mgwin.o

mdrv_mgwin-y :=   src/hal/hal_scl_mgwin.o \
                  src/drv/drv_scl_mgwin.o \
                  src/drv/drv_scl_mgwin_io_wrapper.o \
                  ../../../drv/mgwin/src/$(MS_SCL_OS_NAME)/drv_scl_mgwin_ioctl_parse.o\
                  ../../../drv/mgwin/src/$(MS_SCL_OS_NAME)/drv_scl_mgwin_module_i2.o


