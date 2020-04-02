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

#ifndef __MHAL_DVFS_POWER_H__
#define __MHAL_DVFS_POWER_H__

#ifndef __MDRV_TYPES_H__
#include "mdrv_types.h"
#endif

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define CONFIG_DVFS_CPU_POWER_I2C_ENABLE        1
#define CONFIG_DVFS_CORE_POWER_I2C_ENABLE       1
#define CONFIG_DVFS_CPU_POWER_GPIO_ENABLE       0
#define CONFIG_DVFS_CORE_POWER_GPIO_ENABLE      0

#define CONFIG_DVFS_CPU_POWER                   0
#define CONFIG_DVFS_CORE_POWER                  1
#define CONFIG_DVFS_STR_INIT                    0xFE

#define CONFIG_DVFS_CPU_POWER_SHIFT             94
#define CONFIG_DVFS_CPU_POWER_DEFAULT           100

#define CONFIG_DVFS_CORE_POWER_SHIFT            94
#define CONFIG_DVFS_CORE_POWER_DEFAULT          100

#define CONFIG_DVFS_POWER_SWI2C_BUS             0x01
#if (CONFIG_DVFS_CPU_POWER_I2C_ENABLE)
#define CONFIG_DVFS_POWER_SWI2C_CPU_ID          0x80    //0x80
#define CONFIG_DVFS_POWER_SWI2C_ADDR_CPU        ((CONFIG_DVFS_POWER_SWI2C_BUS << 8) | CONFIG_DVFS_POWER_SWI2C_CPU_ID)
#endif
#if (CONFIG_DVFS_CORE_POWER_I2C_ENABLE)
#define CONFIG_DVFS_POWER_SWI2C_CORE_ID         0x82    //0x82
#define CONFIG_DVFS_POWER_SWI2C_ADDR_CORE       ((CONFIG_DVFS_POWER_SWI2C_BUS << 8) | CONFIG_DVFS_POWER_SWI2C_CORE_ID)
#endif

#define CONFIG_DVFS_CHIP_ID_UNKNOWN             0xFF
#define CONFIG_DVFS_CHIP_ID_PRADA               0x79
#define CONFIG_DVFS_CHIP_ID_PRADO               0x9A
#define CONFIG_DVFS_CPU_POWER_SHIFT_PRADO       (69 - 2)
#define CONFIG_DVFS_CORE_POWER_SHIFT_PRADO      (69 - 2)

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------
U32 SysDvfsPowerInit(void);
U32 SysDvfsCpuPowerInit(void);
U32 SysDvfsCpuPowerAdjustment(U32 dwCpuPowerVoltage);
U32 SysDvfsCorePowerInit(void);
U32 SysDvfsCorePowerAdjustment(U32 dwCorePowerVoltage);

#endif // __MHAL_DVFS_POWER_H__

