ifeq ($(WIFI_MODE),)
RT28xx_MODE = AP
else
RT28xx_MODE = $(WIFI_MODE)
endif
ifeq ($(TARGET),)
TARGET = LINUX
endif

ifeq ($(CHIPSET),)
CHIPSET = 7601U
endif

MODULE = $(word 1, $(CHIPSET))

#OS ABL - YES or NO
OSABL = NO