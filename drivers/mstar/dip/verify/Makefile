export CROSS_COMPILE=arm-linux-gnueabihf-
CC  = $(CROSS_COMPILE)gcc
KERNEL = /users/tomy.chiang/swnas/NVR/i2/SDK/linux-3.18

INCLUDES  := -I../../include/ \
            -I../../camdriver/include/ \
			-I../ \
			-I../drv/inc/ \
			-I../drv/pub/ \
			-I../hal/inc/ \
			-I../hal/pub/

TARGET := dip_test

all:
	$(CC) -o $(TARGET) $(TARGET).c $(INCLUDES)

clean:
	rm -rf $(TARGET)
