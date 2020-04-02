///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2006 - 2014 MStar Semiconductor, Inc.
// This program is free software.
// You can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation;
// either version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with this program;
// if not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __MHAL_DVFS_H__
#define __MHAL_DVFS_H__

#ifndef __MDRV_TYPES_H__
#include "mdrv_types.h"
#endif

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define DVFS_HAL_INFO(x, args...)                       {printk(x, ##args);}
#define DVFS_HAL_DEBUG(x, args...)                      //{printk(x, ##args);}

#define CONFIG_REGISTER_BASE_ADDRESS                    0xfd000000

#define CONFIG_DVFS_CPU_CLOCK_DISPLAY_ENABLE            1 //0: Disable Test Bus; 1: Enable Test Bus

#define CONFIG_DVFS_ENABLE_PATTERN                      0x3697
#define CONFIG_DVFS_DYNAMIC_CLOCK_ADJUST_INIT           0x2454

#define CONFIG_DVFS_DATA_COUNT                          1
#define CONFIG_DVFS_AVERAGE_COUNT                       5
#define CONFIG_DVFS_DELAY_US                            1
#define CONFIG_DVFS_MUTEX_WAIT_TIME                     50

#define DVFS_CPU_CLOCK_DOWN                             0
#define DVFS_CPU_CLOCK_UP                               1
#define DVFS_CPU_POWER_DOWN                             0
#define DVFS_CPU_POWER_UP                               1

#define CONFIG_DVFS_STATE_LOW_SPEED                     0
#define CONFIG_DVFS_STATE_HIGH_SPEED                    1
#define CONFIG_DVFS_STATE_INIT                          0xFF

#define CONFIG_DVFS_SS_CORNER_CHIP_NORMAL               0x00
#define CONFIG_DVFS_FF_CORNER_CHIP_NORMAL               0x01
#define CONFIG_DVFS_SS_CORNER_CHIP_MAX                  0x02
#define CONFIG_DVFS_FF_CORNER_CHIP_MAX                  0x03
#define CONFIG_DVFS_FF_CORNER_CHIP_MASK                 0x03

#define CONFIG_DVFS_POWER_CONTROL_FREEZE                0
#define CONFIG_DVFS_POWER_CONTROL_NORMAL                1

#define CONFIG_DVFS_POWER_CONTROL_LOWER_BOUND           60
#define CONFIG_DVFS_POWER_CONTROL_UPPER_BOUND           80
#define CONFIG_DVFS_DEFAULT_LOWER_BOUND                 120
#define CONFIG_DVFS_DEFAULT_UPPER_BOUND                 135
#define CONFIG_DVFS_DEFAULT_RESET_THRESHOLD             160

#define CONFIG_DVFS_CPU_POWER_INIT                      115
#define CONFIG_DVFS_CPU_POWER_DEFAULT                   110
#define CONFIG_DVFS_CPU_POWER_DROP                      5

#define CONFIG_DVFS_CPU_POWER_INIT_MAX_EX_LOWER_BOUND   130
#define CONFIG_DVFS_CPU_POWER_INIT_MAX_LOWER_BOUND      130
#define CONFIG_DVFS_CPU_POWER_INIT_MID_EX_LOWER_BOUND   115
#define CONFIG_DVFS_CPU_POWER_INIT_MID_LOWER_BOUND      115
#define CONFIG_DVFS_CPU_POWER_INIT_MIN_LOWER_BOUND      115

#define CONFIG_DVFS_CPU_POWER_INIT_MAX_UPPER_BOUND      150
#define CONFIG_DVFS_CPU_POWER_INIT_MID_EX_UPPER_BOUND   115
#define CONFIG_DVFS_CPU_POWER_INIT_MID_UPPER_BOUND      115
#define CONFIG_DVFS_CPU_POWER_INIT_MIN_UPPER_BOUND      115

#define CONFIG_DVFS_CPU_POWER_MAX_EX_LOWER_BOUND        130
#define CONFIG_DVFS_CPU_POWER_MAX_LOWER_BOUND           130
#define CONFIG_DVFS_CPU_POWER_MID_EX_LOWER_BOUND        105
#define CONFIG_DVFS_CPU_POWER_MID_LOWER_BOUND           105
#define CONFIG_DVFS_CPU_POWER_MIN_LOWER_BOUND           105
#define CONFIG_DVFS_CPU_POWER_DROP_LOWER_BOUND          5

#define CONFIG_DVFS_CPU_POWER_MAX_UPPER_BOUND           150
#define CONFIG_DVFS_CPU_POWER_MID_EX_UPPER_BOUND        110
#define CONFIG_DVFS_CPU_POWER_MID_UPPER_BOUND           110
#define CONFIG_DVFS_CPU_POWER_MIN_UPPER_BOUND           110
#define CONFIG_DVFS_CPU_POWER_DROP_UPPER_BOUND          5

#define CONFIG_DVFS_CORE_POWER_INIT                     105
#define CONFIG_DVFS_CORE_POWER_DEFAULT                  105
#define CONFIG_DVFS_CORE_POWER_MAX                      105
#define CONFIG_DVFS_CORE_POWER_MIN                      105
#define CONFIG_DVFS_CORE_POWER_DROP                     0

#define CONFIG_DVFS_CPU_CLOCK_DEFAULT                   1008
#define CONFIG_DVFS_CPU_CLOCK_MIN                       700
#define CONFIG_DVFS_CPU_CLOCK_L0                        701
#define CONFIG_DVFS_CPU_CLOCK_L1                        900
#define CONFIG_DVFS_CPU_CLOCK_L2                        900
#define CONFIG_DVFS_CPU_CLOCK_L3                        1400
#define CONFIG_DVFS_CPU_CLOCK_MAX                       1400
#define CONFIG_DVFS_CPU_CLOCK_PROTECTION                700

#define CONFIG_DVFS_CPU_CLOCK_FREEZE_MODE               100

#define CONFIG_DVFS_CPU_POWER_PROTECTION_UPPER_BOUND    100
#define CONFIG_DVFS_CPU_POWER_PROTECTION_LOWER_BOUND    100

#define CONFIG_DVFS_T_SENSOR_SHIFT                      28000

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
typedef struct
{
    U32     bDvfsInitOk;
    U32     bCpuClockLevel;
    U32     bPowerControlLevel;
    U32     bPowerControlChange;
    U32     dwOverCounter;
    U32     dwVidSetting;

    S32     dwFinalCpuTemperature;
    U32     dwAvgTempBufferCpu[CONFIG_DVFS_DATA_COUNT];
    U32     dwAvgTempCounterCpu;
    U32     dwRefTemperature;

    U16     wCpuClockSetting_High;
    U16     wCpuClockSetting_Low;

    U32     dwTemperatureCounter;
    U32     dwValidCpuClockByTemperature;

    U32     dwPresetCpuClock;
    U32     dwFinalCpuClock;

} MSTAR_DVFS_INFO;

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------
U32  MHalDvfsProc(U32 dwCpuClock);
void MHalDvfsInit(void);
void MHalDvfsCpuDisplayInit(void);
void MHalDvfsCpuDisplay(void);
void MHalDvfsCpuTemperature(void);
void MHalDvfsRefTemperature(void);
void MHalDvfsCpuClockAdjustment(U32 dwDvfsUpperBoundCpuClock_Low, U32 dwDvfsUpperBoundCpuClock_High, U32 bCpuClockRisingFlag);
void MHalDvfsPowerControlStateUpdate(void);
void MHalDvfsCpuPowerInit(void);
void MHalDvfsCpuPowerAdjustment(U32 dwCpuPowerVoltage);
void MHalDvfsCorePowerInit(void);
void MHalDvfsCorePowerAdjustment(U32 dwCorePowerVoltage);
U32  MHalDvfsQueryCpuClockByTemperature(void);
U32  MHalDvfsQueryTemperature(void);
U32 MHalDvfsGetCpuTemperature(void);
#endif
