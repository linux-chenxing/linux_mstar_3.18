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

#ifndef __MDRV_DVFS_H__
#define __MDRV_DVFS_H__

#ifndef __MDRV_TYPES_H__
#include "mdrv_types.h"
#endif

#ifndef __MHAL_DVFS_H__
#include "mhal_dvfs.h"
#endif

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define DVFS_DRV_INFO(x, args...)               //{printk(x, ##args);}
#define DVFS_DRV_DEBUG(x, args...)              //{printk(x, ##args);}

#define CONFIG_DVFS_MAX_CPU_CLOCK               0
#define CONFIG_DVFS_MIN_CPU_CLOCK               1
#define CONFIG_DVFS_IR_BOOTS_CPU_CLOCK          2

#define CONFIG_DVFS_DEFAULT_CPU_POWER           0

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------
U32     MDrvDvfsProc(U32 dwInputCpuClock, U8 dwCpu);
void    MDrvDvfsInit(void);
void    MDrvDvfsCpuDisplay(void);
void    MDrvDvfsCpuPowerAdjustment(U32 dwCpuPowerVoltage);
void    MDrvDvfsCorePowerAdjustment(U32 dwCorePowerVoltage);

U32     MDrvDvfsQueryCpuClock(U32 dwCpuClockType);
U32     MDrvDvfsQueryCpuClockByTemperature(U8 dwCpu);
U32     MDrvDvfsQueryTemperature(void);
U32     MDrvDvfsQueryCpuPower(U32 dwCpuPowerType);
#ifdef CONFIG_MP_DVFS_CPUHOTPLUG
U32     MDrvDvfsGetOverTemperatureFlag(void);
U32    MdrvDvfsGetAntutuCpuNR();
#endif
#endif

