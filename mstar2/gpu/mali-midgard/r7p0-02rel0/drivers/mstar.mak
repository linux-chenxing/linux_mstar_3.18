#
# (C) COPYRIGHT 2016 MStar Semiconductor, Inc. All rights reserved.
#
# This program is free software and is provided to you under the terms of the
# GNU General Public License version 2 as published by the Free Software
# Foundation, and any use by you of this program is subject to the terms
# of such GNU licence.
#
# A copy of the licence is included with the program, and can also be obtained
# from Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
# Boston, MA  02110-1301, USA.
#
#

# Note: the config and flags should synchronize to:
# -bldsys/sconstruct
# -bldsys/profile/*.py
# -base/sconscript

# project profile
ifeq ($(EXTMOD),)
PROFILE_DIR = drivers/mstar2/gpu/mali-midgard/r7p0-02rel0/bldsys/profiles
PROFILE_NAME = $(shell echo $(CONFIG_MSTAR_CHIP_NAME))-$(shell echo $(CONFIG_MALI_PROJECT_PLATFORM))
else
ifeq ($(CONFIG),)
	$(error "Please specify CONFIG variable when building out of tree!")
endif
ifeq ($(KDIR),)
	$(error "Please specify KDIR variable when building out of tree!")
endif
PROFILE_DIR = ../bldsys/profiles
PROFILE_NAME = $(shell echo $(CONFIG))
endif
include $(PROFILE_DIR)/$(PROFILE_NAME).mak

# flags
MSTAR_CFLAGS =
ifneq ($(os),android)
MSTAR_CFLAGS += -DCONFIG_KDS
endif
ifneq ($(CONFIG_UMP),)
MSTAR_CFLAGS += -DCONFIG_UMP
endif

# MStar-specific config
MSTAR_PLATFORM_NAME = $(shell echo $(mstar_platform))
MSTAR_USE_FIXED_DEVID ?= 1
MSTAR_UMP_MONOLITHIC ?= 1
MSTAR_UMP_IMPORT_PA ?= 1
MSTAR_UMP_IMPORT_ION ?= 0
MSTAR_UMM_IMPORT ?= 1
MSTAR_SKIP_JOBS_SWITCH ?= 0
mali_dvfs_freq_adjustable ?= 0
using_efuse_core_num ?= 0

ifeq ($(os),android)
MSTAR_USE_FIXED_DEVID = 0
endif

# MStar-specific flags
MSTAR_CFLAGS += -DMSTAR
MSTAR_CFLAGS += -D$(strip $(shell echo $(mstar_platform) | tr a-z A-Z))
MSTAR_CFLAGS += -D$(strip $(shell echo $(project) | tr a-z A-Z))
MSTAR_CFLAGS += -DMALI_MAX_FREQ=$(shell echo $(mali_max_freq))
MSTAR_CFLAGS += -DMALI_MIN_FREQ=$(shell echo $(mali_min_freq))
MSTAR_CFLAGS += -DMALI_BOOST_FREQ=$(shell echo $(mali_boost_freq))
MSTAR_CFLAGS += -DMALI_IRQ=$(shell echo $(mali_irq))
MSTAR_CFLAGS += -DMALI_REG_OFFSET=$(shell echo $(mali_reg_offset))
MSTAR_CFLAGS += -DMSTAR_RIU_ENABLED
MSTAR_CFLAGS += -DMSTAR_PM_CALLBACKS
MSTAR_CFLAGS += -DMSTAR_PM_DVFS_PERIOD=500
MSTAR_CFLAGS += -DMSTAR_PM_GPU_POWEROFF_TICK_NS=1000000000
MSTAR_CFLAGS += -DMSTAR_USE_FIXED_DEVID=$(MSTAR_USE_FIXED_DEVID)
MSTAR_CFLAGS += -DMSTAR_UMP_MONOLITHIC=$(MSTAR_UMP_MONOLITHIC)
MSTAR_CFLAGS += -DMSTAR_UMP_IMPORT_PA=$(MSTAR_UMP_IMPORT_PA)
MSTAR_CFLAGS += -DMSTAR_UMP_IMPORT_ION=$(MSTAR_UMP_IMPORT_ION)
MSTAR_CFLAGS += -DMSTAR_UMM_IMPORT=$(MSTAR_UMM_IMPORT)
MSTAR_CFLAGS += -DMSTAR_DISABLE_CONFIG_OF
MSTAR_CFLAGS += -DMSTAR_DISABLE_CONFIG_NEED_SG_DMA_LENGTH
MSTAR_CFLAGS += -DMSTAR_GPU_UTILIZATION
MSTAR_CFLAGS += -DMSTAR_MEMORY_USAGE
MSTAR_CFLAGS += -DCONFIG_MALI_MIDGARD_DVFS
MSTAR_CFLAGS += -DMSTAR_SKIP_JOBS_SWITCH=$(MSTAR_SKIP_JOBS_SWITCH)
MSTAR_CFLAGS += -DCONFIG_MALI_MIDGARD_DVFS_FREQ_ADJUSTABLE=$(shell echo $(mali_dvfs_freq_adjustable))
MSTAR_CFLAGS += -DMSTAR_DISABLE_GPU_CORES=$(shell echo $(using_efuse_core_num))
