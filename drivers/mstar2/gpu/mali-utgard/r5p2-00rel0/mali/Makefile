#
# Copyright (C) 2010-2015 ARM Limited. All rights reserved.
# 
# This program is free software and is provided to you under the terms of the GNU General Public License version 2
# as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
# 
# A copy of the licence is included with the program, and can also be obtained from Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#

USE_UMPV2=0
USING_PROFILING ?= 1
USING_INTERNAL_PROFILING ?= 0
# MSTAR patch begin
USING_DVFS ?= 0
USING_EFUSE_CORE_NUM ?=0
# MSTAR patch end
MALI_HEATMAPS_ENABLED ?= 0
MALI_DMA_BUF_MAP_ON_ATTACH ?= 1
MALI_PMU_PARALLEL_POWER_UP ?= 0
USING_DT ?= 0

# The Makefile sets up "arch" based on the CONFIG, creates the version info
# string and the __malidrv_build_info.c file, and then call the Linux build
# system to actually build the driver. After that point the Kbuild file takes
# over.

# set up defaults if not defined by the user
ARCH ?= arm

# MStar: set up our defaults
BUILD ?= release
USING_UMP ?= 0

OSKOS=linux
FILES_PREFIX=

check_cc2 = \
	$(shell if $(1) -S -o /dev/null -xc /dev/null > /dev/null 2>&1; \
	then \
		echo "$(2)"; \
	else \
		echo "$(3)"; \
	fi ;)

# MStar: project config
include ../build_system/project/$(CONFIG).mak

# This conditional makefile exports the global definition ARM_INTERNAL_BUILD. Customer releases will not include arm_internal.mak
-include ../../../arm_internal.mak

# Give warning of old config parameters are used
ifneq ($(TARGET_PLATFORM), mstar)
ifneq ($(CONFIG),)
$(warning "You have specified the CONFIG variable which is no longer in used. Use TARGET_PLATFORM instead.")
endif
endif

ifneq ($(CPU),)
$(warning "You have specified the CPU variable which is no longer in used. Use TARGET_PLATFORM instead.")
endif

ifeq ($(MSTAR_PLATFORM),)
$(error "MSTAR_PLATFORM must be specified.")
endif

ifeq ($(TARGET_PLATFORM), mstar)
	MALI_PLATFORM = mstar
	MALI_SHARED_INTERRUPTS = 1
endif

# Include the mapping between TARGET_PLATFORM and KDIR + MALI_PLATFORM
-include MALI_CONFIGURATION
export KDIR ?= $(KDIR-$(TARGET_PLATFORM))
export MALI_PLATFORM ?= $(MALI_PLATFORM-$(TARGET_PLATFORM))

ifneq ($(TARGET_PLATFORM),)
ifeq ($(MALI_PLATFORM),)
$(error "Invalid TARGET_PLATFORM: $(TARGET_PLATFORM)")
endif
endif

# validate lookup result
ifeq ($(KDIR),)
$(error No KDIR found for platform $(TARGET_PLATFORM))
endif

# MSTAR patch begin
ifeq ($(USING_DVFS), 1)

# disable USING_GPU_UTILIZATION
export USING_GPU_UTILIZATION = 0

ifneq ($(strip $(MALI_MAX_SYSTEM_FPS)),)
MALI_MAX_SYSTEM_FPS := $(strip $(MALI_MAX_SYSTEM_FPS))
ifneq ($(shell echo '$(MALI_MAX_SYSTEM_FPS)' | tr -d 0-9 2>/dev/null),)
$(error MALI_MAX_SYSTEM_FPS: $(MALI_MAX_SYSTEM_FPS): invalid format)
endif
export EXTRA_DEFINES += -DMALI_MAX_SYSTEM_FPS=$(MALI_MAX_SYSTEM_FPS)
endif

ifneq ($(strip $(MALI_DESIRED_FPS)),)
MALI_DESIRED_FPS := $(strip $(MALI_DESIRED_FPS))
ifneq ($(shell echo '$(MALI_DESIRED_FPS)' | tr -d 0-9 2>/dev/null),)
$(error MALI_DESIRED_FPS: $(MALI_DESIRED_FPS): invalid format)
endif
export EXTRA_DEFINES += -DMALI_DESIRED_FPS=$(MALI_DESIRED_FPS)
endif

endif
# MSTAR patch end

ifeq ($(USING_GPU_UTILIZATION), 1)
    ifeq ($(USING_DVFS), 1)
        $(error USING_GPU_UTILIZATION conflict with USING_DVFS you can read the Integration Guide to choose which one do you need)
    endif
endif

ifeq ($(USING_UMP),1)
export CONFIG_MALI400_UMP=y
export EXTRA_DEFINES += -DCONFIG_MALI400_UMP=1
ifeq ($(USE_UMPV2),1)
UMP_SYMVERS_FILE ?= ../umpv2/Module.symvers
else
UMP_SYMVERS_FILE ?= ../ump/Module.symvers
endif
KBUILD_EXTRA_SYMBOLS = $(realpath $(UMP_SYMVERS_FILE))
$(warning $(KBUILD_EXTRA_SYMBOLS))
endif

# Define host system directory
KDIR-$(shell uname -m):=/lib/modules/$(shell uname -r)/build

include $(KDIR)/.config

# MSTAR: make sure the kernel CONFIG_SYNC is enabled for Android
ifeq ($(HAVE_ANDROID_OS),1)
ifeq ($(CONFIG_SYNC),)
$(error Linux kernel must support CONFIG_SYNC)
endif
endif

ifeq ($(ARCH), arm)
# when compiling for ARM we're cross compiling
ifeq ($(TARGET_PLATFORM), mstar)
export CROSS_COMPILE ?= arm-none-linux-gnueabi-
else
export CROSS_COMPILE ?= $(call check_cc2, arm-linux-gnueabi-gcc, arm-linux-gnueabi-, arm-none-linux-gnueabi-)
endif
endif

# MStar: arm64
ifeq ($(ARCH), arm64)
# when compiling for ARM we're cross compiling
export CROSS_COMPILE ?= aarch64-linux-gnu-
endif

# report detected/selected settings
ifdef ARM_INTERNAL_BUILD
$(warning TARGET_PLATFORM $(TARGET_PLATFORM))
$(warning KDIR $(KDIR))
$(warning MALI_PLATFORM $(MALI_PLATFORM))
endif

# Set up build config
export CONFIG_MALI400=m
export CONFIG_MALI450=y

export EXTRA_DEFINES += -DCONFIG_MALI400=1
export EXTRA_DEFINES += -DCONFIG_MALI450=1

ifneq ($(MALI_PLATFORM),)
export EXTRA_DEFINES += -DMALI_FAKE_PLATFORM_DEVICE=1
export MALI_PLATFORM_FILES = $(wildcard platform/$(MALI_PLATFORM)/*.c)
endif

ifeq ($(TARGET_PLATFORM), mstar)
export MSTAR_PLATFORM_FILE = $(wildcard platform/mstar/$(MSTAR_PLATFORM)/*.c)
endif

ifeq ($(USING_PROFILING),1)
ifeq ($(CONFIG_TRACEPOINTS),)
$(warning CONFIG_TRACEPOINTS required for profiling)
else
export CONFIG_MALI400_PROFILING=y
export EXTRA_DEFINES += -DCONFIG_MALI400_PROFILING=1
ifeq ($(USING_INTERNAL_PROFILING),1)
export CONFIG_MALI400_INTERNAL_PROFILING=y
export EXTRA_DEFINES += -DCONFIG_MALI400_INTERNAL_PROFILING=1
endif
ifeq ($(MALI_HEATMAPS_ENABLED),1)
export MALI_HEATMAPS_ENABLED=y
export EXTRA_DEFINES += -DCONFIG_MALI400_HEATMAPS_ENABLED
endif
endif
endif

ifeq ($(MALI_DMA_BUF_MAP_ON_ATTACH),1)
export CONFIG_MALI_DMA_BUF_MAP_ON_ATTACH=y
export EXTRA_DEFINES += -DCONFIG_MALI_DMA_BUF_MAP_ON_ATTACH
endif

ifeq ($(MALI_SHARED_INTERRUPTS),1)
export CONFIG_MALI_SHARED_INTERRUPTS=y
export EXTRA_DEFINES += -DCONFIG_MALI_SHARED_INTERRUPTS
endif

ifeq ($(USING_DVFS),1)
export CONFIG_MALI_DVFS=y
export EXTRA_DEFINES += -DCONFIG_MALI_DVFS
endif

# MSTAR patch begin
ifeq ($(USING_EFUSE_CORE_NUM),1)
export CONFIG_MALI_EFUSE_PP_CORE_NUM=y
export EXTRA_DEFINES += -DCONFIG_MALI_EFUSE_CORE_NUM
endif
# MSTAR patch end

ifeq ($(MALI_PMU_PARALLEL_POWER_UP),1)
export CONFIG_MALI_PMU_PARALLEL_POWER_UP=y
export EXTRA_DEFINES += -DCONFIG_MALI_PMU_PARALLEL_POWER_UP
endif

ifdef CONFIG_OF
ifeq ($(USING_DT),1)
export CONFIG_MALI_DT=y
export EXTRA_DEFINES += -DCONFIG_MALI_DT
endif
endif

ifneq ($(BUILD),release)
# Debug
export CONFIG_MALI400_DEBUG=y
else
# Release
ifeq ($(MALI_QUIET),1)
export CONFIG_MALI_QUIET=y
export EXTRA_DEFINES += -DCONFIG_MALI_QUIET
endif
endif

ifeq ($(MALI_SKIP_JOBS),1)
EXTRA_DEFINES += -DPROFILING_SKIP_PP_JOBS=1 -DPROFILING_SKIP_GP_JOBS=1
endif

# MStar: check kernel config
export CHECK_KERNEL_CONFIG = $(CHECK_CONFIG_CHIP_NAME)

ifeq ($(MAKECMDGOALS),clean)
CHECK_KERNEL_CONFIG := 0
endif

all: $(UMP_SYMVERS_FILE)
	$(MAKE) ARCH=$(ARCH) -C $(KDIR) M=$(CURDIR) modules
	@rm $(FILES_PREFIX)__malidrv_build_info.c $(FILES_PREFIX)__malidrv_build_info.o

clean:
	$(MAKE) ARCH=$(ARCH) -C $(KDIR) M=$(CURDIR) clean

kernelrelease:
	$(MAKE) ARCH=$(ARCH) -C $(KDIR) kernelrelease

export CONFIG KBUILD_EXTRA_SYMBOLS
