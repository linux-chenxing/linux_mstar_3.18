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

EXTMOD = 1

SCONS_CONFIGS =
SCONS_CFLAGS =

MALI_KBASE_DEP =
UMP_DEP =

include mstar.mak

SCONS_CFLAGS = $(shell echo $(MSTAR_CFLAGS))

ifeq ($(debug),1)
SCONS_CONFIGS += "CONFIG_MALI_DEBUG=y "
SCONS_CFLAGS += "-DCONFIG_MALI_DEBUG "
endif

ifneq ($(os),android)
SCONS_CONFIGS += CONFIG_KDS=m
MALI_KBASE_DEP += kds
endif

ifeq ($(ump),1)
SCONS_CONFIGS += CONFIG_UMP=m
MALI_KBASE_DEP += ump
UMP_DEP += kds
endif

ifeq ($(arch), arm_v7)
	ARCH = arm
else ifeq ($(arch), arm_v8)
	ARCH = arm64
endif

SCONS_CONFIGS += CONFIG_MALI_PLATFORM_FAKE=y
SCONS_CFLAGS += -DCONFIG_MALI_PLATFORM_FAKE

SCONS_CONFIGS += CONFIG_MALI_PLATFORM_THIRDPARTY=y CONFIG_MALI_PLATFORM_THIRDPARTY_NAME=mstar
SCONS_CFLAGS += -DCONFIG_MALI_PLATFORM_THIRDPARTY

SCONS_CONFIGS += MSTAR_PLATFORM_NAME=$(mstar_platform)
SCONS_CONFIGS += MSTAR_UMM_IMPORT=$(MSTAR_UMM_IMPORT)

SCONS_CONFIGS += CONFIG_MALI_MIDGARD=m

all: kbase

kbase: $(MALI_KBASE_DEP)
	SCONS_CONFIGS="$(SCONS_CONFIGS)" SCONS_CFLAGS="$(SCONS_CFLAGS)" make -C gpu/arm/midgard -j2

kds:
	ARCH=$(ARCH) SCONS_CONFIGS="$(SCONS_CONFIGS)" SCONS_CFLAGS="$(SCONS_CFLAGS)" make -C base/kds -j2

#ump makefile uses variable CONFIG, unset CONFIG before building ump ko
ump: $(UMP_DEP)
	unset CONFIG; ARCH=$(ARCH) SCONS_CFLAGS="$(SCONS_CFLAGS)" make -C base/ump/src -j2

clean: $(addsuffix -clean, $(MALI_KBASE_DEP))
	make -C gpu/arm/midgard clean

kds-clean:
	ARCH=$(ARCH) make -C base/kds clean

ump-clean:
	unset CONFIG; ARCH=$(ARCH) make -C base/ump/src clean
