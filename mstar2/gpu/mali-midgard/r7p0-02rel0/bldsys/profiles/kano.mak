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

mstar_platform=kano

# gpu core
gpu=t82x
hwver=r1p0

# cpu core
core=cortex-a9

# config
mali_max_freq=550 # in MHz
mali_min_freq=550 # in MHz
mali_boost_freq=550 # in Mhz
mali_irq=E_IRQHYPL_G3D2MCU_DFT
mali_reg_offset=0x00360000