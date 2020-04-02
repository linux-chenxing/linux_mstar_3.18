#
# Copyright (C) 2010-2012 ARM Limited. All rights reserved.
# 
# This program is free software and is provided to you under the terms of the GNU General Public License version 2
# as published by the Free Software Foundation, and any use by you of this program is subject to the terms of such GNU licence.
# 
# A copy of the licence is included with the program, and can also be obtained from Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#

# Set default configuration to use, if Makefile didn't provide one.
# Change this to use a different config.h
MALI_CONFIG ?= os_memory_64m

MALI_DRIVER_DIR=$(src)/$(MALI_TOP_DIR)$(MALI_VERSION)/linux/src/devicedrv/ump

# MStar project config
include $(src)/$(MALI_TOP_DIR)project.mak

# check kernel config
MALI_CHECK_KERNEL_CONFIG ?= 0
ifeq ($(MALI_CHECK_KERNEL_CONFIG),1)
ifneq ($(MALI_CONFIG_CHIP_NAME),$(patsubst "%",%,$(CONFIG_MSTAR_CHIP_NAME)))
$(error kernel config ($(CONFIG_MSTAR_CHIP_NAME)) not matched, should be $(MALI_CONFIG_CHIP_NAME))
endif
endif

# Validate selected config
ifneq ($(shell [ -d $(src)/$(MALI_TOP_DIR)$(MALI_VERSION)/linux/src/devicedrv/ump/arch-$(MALI_CONFIG) ] && [ -f  $(src)/$(MALI_TOP_DIR)$(MALI_VERSION)/linux/src/devicedrv/ump/arch-$(MALI_CONFIG)/config.h ] && echo "OK"), OK)
$(warning Current directory is $(src)/$(MALI_TOP_DIR)$(MALI_VERSION)/linux/src/devicedrv/ump)
$(error No configuration found for config $(MALI_CONFIG). Check that arch-$(MALI_CONFIG)/config.h exists)
else
# Link arch to the selected arch-config directory
$(shell [ -L $(src)/$(MALI_TOP_DIR)$(MALI_VERSION)/linux/src/devicedrv/ump/arch ] && rm $(src)/$(MALI_TOP_DIR)$(MALI_VERSION)/linux/src/devicedrv/ump/arch)
$(shell ln -sf arch-$(MALI_CONFIG) $(src)/$(MALI_TOP_DIR)$(MALI_VERSION)/linux/src/devicedrv/ump/arch)
$(shell touch $(src)/$(MALI_TOP_DIR)$(MALI_VERSION)/linux/src/devicedrv/ump/arch/config.h)
endif

UDD_FILE_PREFIX = ../mali/

ifeq ($(MALI_TARGET_PLATFORM), mstar)
MALI_RELEASE_NAME=$(shell cat $(src)/$(MALI_TOP_DIR)$(MALI_VERSION)/linux/src/devicedrv/ump/../mali/.version 2> /dev/null)
ifeq ($(wildcard $(src)/$(MALI_TOP_DIR)$(MALI_VERSION)/linux/src/devicedrv/ump/../../../version_info.mak),)
	$(error version_info.mak not found)
endif
LIBCL :=
$(eval $(shell grep "LIBCL\s*=" $(src)/$(MALI_TOP_DIR)$(MALI_VERSION)/linux/src/devicedrv/ump/../../../version_info.mak))
ifeq ($(strip $(LIBCL)),)
	$(error LIBCL not found in version_info.mak)
endif
LIBCL := $(strip $(shell (cd $(src)/$(MALI_TOP_DIR)$(MALI_VERSION)/linux/src/devicedrv/ump; echo "$(LIBCL)" | grep '^Change: '| sed -e 's/^Change: //' ) 2>/dev/null ))
ifeq ($(LIBCL),)
	$(error LIBCL invalid format)
endif
MALI_SVN_REV := $(MALI_RELEASE_NAME)-CL$(LIBCL)
else
# Get subversion revision number, fall back to 0000 if no svn info is available
MALI_SVN_REV := $(shell ((svnversion | grep -qv exported && echo -n 'Revision: ' && svnversion) || git svn info | sed -e 's/$$$$/M/' | grep '^Revision: ' || echo ${MALI_RELEASE_NAME}) 2>/dev/null | sed -e 's/^Revision: //')
endif

ccflags-y += -DSVN_REV=$(MALI_SVN_REV)
ccflags-y += -DSVN_REV_STRING=\"$(MALI_SVN_REV)\"

ccflags-y += -I$(src)/$(MALI_TOP_DIR)$(MALI_VERSION)/linux/src/devicedrv/ump -I$(src)/$(MALI_TOP_DIR)$(MALI_VERSION)/linux/src/devicedrv/ump/common -I$(src)/$(MALI_TOP_DIR)$(MALI_VERSION)/linux/src/devicedrv/ump/linux -I$(src)/$(MALI_TOP_DIR)$(MALI_VERSION)/linux/src/devicedrv/ump/../mali/common -I$(src)/$(MALI_TOP_DIR)$(MALI_VERSION)/linux/src/devicedrv/ump/../mali/linux -I$(src)/$(MALI_TOP_DIR)$(MALI_VERSION)/linux/src/devicedrv/ump/../../ump/include/ump
ccflags-y += -DMALI_STATE_TRACKING=0
ccflags-$(CONFIG_UMP_DEBUG) += -DDEBUG

# MStar flags
ifeq ($(MALI_TARGET_PLATFORM), mstar)
	ifeq ($(MALI_MSTAR_PLATFORM),)
		$(error "MSTAR_PLATFORM must be specified.")
	endif

	ccflags-y += -DMSTAR
	ccflags-y += -DMSTAR_$(MALI_MSTAR_PLATFORM)

	ifeq ($(MALI_ARCH), mips)
		ccflags-y += -DMIPS
	endif

	ifneq ($(MALI_USING_RIU),0)
		ccflags-y += -DMSTAR_RIU_ENABLED
		ccflags-y += -DMSTAR_RIU_ADDRESS_TYPE=$(MALI_RIU_ADDRESS_TYPE)
	endif

	ifneq ($(MALI_PHYS_TO_BUS_ADDRESS_ADJUST),)
		ccflags-y += -DPHYS_TO_BUS_ADDRESS_ADJUST=$(strip $(MALI_PHYS_TO_BUS_ADDRESS_ADJUST))
	endif

	ifneq ($(MSTAR_MIU1_PHY_BASE),)
		ccflags-y += -DMSTAR_MIU1_PHY_BASE=$(strip $(MSTAR_MIU1_PHY_BASE))
	endif

	ifneq ($(MALI_USING_FIXED_DEVID),)
		ccflags-y += -DMSTAR_USING_FIXED_DEVID=$(MALI_USING_FIXED_DEVID)
		ccflags-y += -Idrivers/mstar/include
	else
		ccflags-y += -DMSTAR_USING_FIXED_DEVID=0
	endif
endif

	ccflags-y += -Idrivers/staging 

# For customer releases the Linux Device Drivers will be provided as ARM proprietary and GPL releases:
# The ARM proprietary product will only include the license/proprietary directory
# The GPL product will only include the license/gpl directory

ifeq ($(wildcard $(src)/$(MALI_TOP_DIR)$(MALI_VERSION)/linux/src/devicedrv/ump/linux/license/gpl/*),)
ccflags-y += -I$(src)/$(MALI_TOP_DIR)$(MALI_VERSION)/linux/src/devicedrv/ump/linux/license/proprietary
else
ccflags-y += -I$(src)/$(MALI_TOP_DIR)$(MALI_VERSION)/linux/src/devicedrv/ump/linux/license/gpl
endif

ump-y = common/ump_kernel_common.o \
	common/ump_kernel_descriptor_mapping.o \
	common/ump_kernel_api.o \
	common/ump_kernel_ref_drv.o \
	linux/ump_kernel_linux.o \
	linux/ump_kernel_memory_backend_os.o \
	linux/ump_kernel_memory_backend_dedicated.o \
	linux/ump_memory_backend.o \
	linux/ump_ukk_wrappers.o \
	linux/ump_ukk_ref_wrappers.o \
	linux/ump_osk_atomics.o \
	linux/ump_osk_low_level_mem.o \
	linux/ump_osk_misc.o \
	$(UDD_FILE_PREFIX)linux/mali_osk_atomics.o \
	$(UDD_FILE_PREFIX)linux/mali_osk_locks.o \
	$(UDD_FILE_PREFIX)linux/mali_osk_memory.o \
	$(UDD_FILE_PREFIX)linux/mali_osk_math.o \
	$(UDD_FILE_PREFIX)linux/mali_osk_misc.o

obj-$(CONFIG_UMP) := ump.o

