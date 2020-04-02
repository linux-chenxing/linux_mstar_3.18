obj-m       := linux_ktest.o
linux_ktest-objs := ../src/cam_os_wrapper.o cam_os_wrapper_linux_kernel_test.o

export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabihf-

KERNEL = /home/david.tsai/I3/linux-3.18
CC  = $(CROSS_COMPILE)gcc
APP = cam_os_wrapper_test
#EXTRA_CFLAGS := -I/home/david.tsai/I3/IPCM/openmax/cam_os_wrapper/pub/ -I$(KERNEL)/drivers/mstar/include/
EXTRA_CFLAGS := -I/home/david.tsai/I3/cam_os_wrapper/pub/ -I$(KERNEL)/drivers/mstar/include/
USER_CFLAGS := -Wall -Werror -g -O2 -ffunction-sections -funwind-tables -fstack-protector -I../pub/ -I$(KERNEL)/drivers/mstar/include/
USER_LDFLAGS := -L../.build/lib -lcam_os_wrapper

all:
	make modules -C $(KERNEL)  M=`pwd`
	#$(CROSS_COMPILE)strip -g cam_os_wrapper_linux_ktest.ko
	$(CC) $(USER_CFLAGS) $(USER_LDFLAGS) $(APP).c -o $(APP)

clean:
	make modules clean -C $(KERNEL)  M=`pwd`
	rm -rf $(APP)
    
