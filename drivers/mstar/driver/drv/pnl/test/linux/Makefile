CC := $(CROSS_COMPILE)gcc
INCLUDES := -I../../inc -I../../../../../cam_os_wrapper/pub

TARGET := pnl_test

all:
	$(CC) -o $(TARGET) $(TARGET).c $(INCLUDES)

clean:
	rm -rf $(TARGET)
