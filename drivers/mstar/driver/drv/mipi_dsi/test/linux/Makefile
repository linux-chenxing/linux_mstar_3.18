CC := $(CROSS_COMPILE)gcc
INCLUDES := -I../../inc -I../../../../../cam_os_wrapper/pub

TARGET := mipi_dsi_test

all:
	$(CC) -o $(TARGET) $(TARGET).c $(INCLUDES)

clean:
	rm -rf $(TARGET)
