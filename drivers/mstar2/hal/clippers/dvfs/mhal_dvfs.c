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

//-------------------------------------------------------------------------------------------------
//  Include files
//-------------------------------------------------------------------------------------------------
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/delay.h>

#include <mstar/mstar_chip.h>

#ifndef __MHAL_DVFS_H__
#include "mhal_dvfs.h"
#endif

extern void MDrvDvfsVoltageSetup(unsigned int dwCpuClock, unsigned int dwVoltage, unsigned int dwVoltageType);

static MSTAR_DVFS_INFO hMstarDvfsInfo =
{
    .bDvfsInitOk = 0,
    .bCpuClockLevel = CONFIG_DVFS_STATE_INIT,
    .bPowerControlLevel = CONFIG_DVFS_POWER_CONTROL_FREEZE,
    .bPowerControlChange = 0,
    .dwOverCounter = 0,
    .dwVidSetting = 0,

    .dwFinalCpuTemperature = 0,
    .dwAvgTempCounterCpu = 0,
    .dwRefTemperature = 0,

    .wCpuClockSetting_High = 0,
    .wCpuClockSetting_Low = 0,

    .dwTemperatureCounter = 0,
    .dwValidCpuClockByTemperature = CONFIG_DVFS_CPU_CLOCK_DEFAULT,

    .dwPresetCpuClock = 0,
    .dwFinalCpuClock = 0,
};

//=================================================================================================
U32 MHalDvfsProc(U32 dwCpuClock)
{
    U32     dwRegisterValue = 0;

    hMstarDvfsInfo.dwPresetCpuClock = dwCpuClock;

    //Check Specific Register to Check DVFS Running State (0x1005_00 = 0x3697)
    dwRegisterValue = *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x100500 << 1));
    if(dwRegisterValue == CONFIG_DVFS_ENABLE_PATTERN)
    {
        S32     dwUpperTemperature = 0;
        S32     dwLowerTemperature = 0;
        S32     dwResetTemperature = 0;

        if(hMstarDvfsInfo.bDvfsInitOk == 0)
        {
            //Initial DVFS Default Settings and Data Structure
            MHalDvfsInit();
        }

        //Get CPU Temperature by PM_SAR
        MHalDvfsCpuTemperature();

        MHalDvfsPowerControlStateUpdate();

        //Get The Upper Bound of CPU Temperature by Register 0x1005_08[15:8]
        dwUpperTemperature = *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x100508 << 1));
        dwUpperTemperature >>= 8;
        if(dwUpperTemperature == 0)
        {
            dwUpperTemperature = CONFIG_DVFS_DEFAULT_UPPER_BOUND;
        }

        //Get The Lower Bound of CPU Temperature by Register 0x1005_08[7:0]
        dwLowerTemperature = *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x100508 << 1));
        dwLowerTemperature &= 0xFF;
        if(dwLowerTemperature == 0)
        {
            dwLowerTemperature = CONFIG_DVFS_DEFAULT_LOWER_BOUND;
        }

        //Get Maximum of CPU Temperature by Register 0x1005_0A[7:0]
        dwResetTemperature = *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x10050a << 1));
        dwResetTemperature &= 0xFF;
        if(dwResetTemperature == 0)
        {
            dwResetTemperature = CONFIG_DVFS_DEFAULT_RESET_THRESHOLD;
        }

        if(hMstarDvfsInfo.bCpuClockLevel == CONFIG_DVFS_STATE_INIT)
        {
            if(hMstarDvfsInfo.bPowerControlLevel == CONFIG_DVFS_POWER_CONTROL_FREEZE)
            {
                dwRegisterValue = (hMstarDvfsInfo.dwVidSetting & 0x00FF);
                if((dwRegisterValue > 0) && (dwRegisterValue < CONFIG_DVFS_CPU_POWER_DEFAULT))
                {
                    MHalDvfsCpuPowerAdjustment(CONFIG_DVFS_CPU_POWER_INIT_MIN_LOWER_BOUND);
                }
                else
                {
                    MHalDvfsCpuPowerAdjustment(CONFIG_DVFS_CPU_POWER_INIT_MIN_UPPER_BOUND);
                }
            }
            else
            {
                //Adjust CPU Power (VDDC_CPU)
                dwRegisterValue = (hMstarDvfsInfo.dwVidSetting & 0x00FF);
                if((dwRegisterValue > 0) && (dwRegisterValue < CONFIG_DVFS_CPU_POWER_DEFAULT))
                {
                    MHalDvfsCpuPowerAdjustment(dwRegisterValue);
                }
                else
                {
                    MHalDvfsCpuPowerAdjustment(CONFIG_DVFS_CPU_POWER_DEFAULT);
                }
            }

            if(hMstarDvfsInfo.bPowerControlLevel == CONFIG_DVFS_POWER_CONTROL_FREEZE)
            {
                //Adjust Core Power (VDDC)
                MHalDvfsCorePowerAdjustment(CONFIG_DVFS_CORE_POWER_INIT);
            }
            else
            {
                dwRegisterValue = ((hMstarDvfsInfo.dwVidSetting & 0xFF00) >> 8);
                if((dwRegisterValue > 0) && (dwRegisterValue < CONFIG_DVFS_CORE_POWER_DEFAULT))
                {
                    MHalDvfsCorePowerAdjustment(dwRegisterValue);
                }
                else
                {
                    MHalDvfsCorePowerAdjustment(CONFIG_DVFS_CORE_POWER_DEFAULT);
                }
            }

            //Raising CPU Clock after adjust system power
            MHalDvfsCpuClockAdjustment(
                                hMstarDvfsInfo.wCpuClockSetting_Low,
                                hMstarDvfsInfo.wCpuClockSetting_High,
                                DVFS_CPU_CLOCK_UP
                                );

            hMstarDvfsInfo.bCpuClockLevel = CONFIG_DVFS_STATE_HIGH_SPEED;
            hMstarDvfsInfo.dwOverCounter = 0;
        }
        else
        {
            if(hMstarDvfsInfo.dwFinalCpuTemperature > dwResetTemperature)
            {
                //Retry 10 Times to Confirm the State of Over Temperature
                if(hMstarDvfsInfo.dwOverCounter < CONFIG_DVFS_AVERAGE_COUNT)
                {
                    DVFS_HAL_DEBUG("[DVFS] Over Temperature Protection: Count = %d / Temperature = %d\n", (unsigned int) hMstarDvfsInfo.dwOverCounter, (unsigned int) hMstarDvfsInfo.dwFinalCpuTemperature);
                    hMstarDvfsInfo.dwOverCounter ++;
                }
                else
                {
                    //Trigger a WDT Reset
                    DVFS_HAL_INFO("[DVFS] Over Temperature Protection\n");
                    *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x00300a << 1)) = 0x00;
                    *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x003008 << 1)) = 0x00;
                    *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x00300a << 1)) = 0x05;
                    *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x003000 << 1)) = 0x01;

                    while(1);
                }
            }

            if(hMstarDvfsInfo.dwFinalCpuTemperature > dwUpperTemperature)
            {
                if(hMstarDvfsInfo.bCpuClockLevel == CONFIG_DVFS_STATE_HIGH_SPEED)
                {
                    if(hMstarDvfsInfo.dwOverCounter < CONFIG_DVFS_AVERAGE_COUNT)
                    {
                        hMstarDvfsInfo.dwOverCounter ++;
                    }
                    else
                    {
                        DVFS_HAL_INFO("[DVFS] Current Temperature: %d\n", (unsigned int) hMstarDvfsInfo.dwFinalCpuTemperature);
                        DVFS_HAL_INFO("[DVFS] Change to Lower CPU Clock Setting\n");

                        //Falling CPU Clock before adjust system power
                        MHalDvfsCpuClockAdjustment(
                                            hMstarDvfsInfo.wCpuClockSetting_Low,
                                            hMstarDvfsInfo.wCpuClockSetting_High,
                                            DVFS_CPU_CLOCK_DOWN
                                            );

                        if(hMstarDvfsInfo.bPowerControlLevel == CONFIG_DVFS_POWER_CONTROL_FREEZE)
                        {
                            dwRegisterValue = (hMstarDvfsInfo.dwVidSetting & 0x00FF);
                            if((dwRegisterValue > 0) && (dwRegisterValue < CONFIG_DVFS_CPU_POWER_DEFAULT))
                            {
                                MHalDvfsCpuPowerAdjustment(CONFIG_DVFS_CPU_POWER_INIT_MIN_LOWER_BOUND);
                            }
                            else
                            {
                                MHalDvfsCpuPowerAdjustment(CONFIG_DVFS_CPU_POWER_INIT_MIN_UPPER_BOUND);
                            }
                        }
                        else
                        {
                            //Adjust CPU Power (VDDC_CPU)
                            dwRegisterValue = (hMstarDvfsInfo.dwVidSetting & 0x00FF);
                            if((dwRegisterValue > 0) && (dwRegisterValue < CONFIG_DVFS_CPU_POWER_DEFAULT))
                            {
                                //MHalDvfsCpuPowerAdjustment(dwRegisterValue);
                                MHalDvfsCpuPowerAdjustment(CONFIG_DVFS_CPU_POWER_PROTECTION_LOWER_BOUND);
                            }
                            else
                            {
                                //MHalDvfsCpuPowerAdjustment(CONFIG_DVFS_CPU_POWER_DEFAULT);
                                MHalDvfsCpuPowerAdjustment(CONFIG_DVFS_CPU_POWER_PROTECTION_UPPER_BOUND);
                            }
                        }

                        if(hMstarDvfsInfo.bPowerControlLevel == CONFIG_DVFS_POWER_CONTROL_FREEZE)
                        {
                            //Adjust Core Power (VDDC)
                            MHalDvfsCorePowerAdjustment(CONFIG_DVFS_CORE_POWER_INIT);
                        }
                        else
                        {
                            dwRegisterValue = ((hMstarDvfsInfo.dwVidSetting & 0xFF00) >> 8);
                            if((dwRegisterValue > 0) && (dwRegisterValue < CONFIG_DVFS_CORE_POWER_DEFAULT))
                            {
                                MHalDvfsCorePowerAdjustment(dwRegisterValue);
                            }
                            else
                            {
                                MHalDvfsCorePowerAdjustment(CONFIG_DVFS_CORE_POWER_DEFAULT);
                            }
                        }

                        hMstarDvfsInfo.bCpuClockLevel = CONFIG_DVFS_STATE_LOW_SPEED;
                        hMstarDvfsInfo.dwOverCounter = 0;
                    }
                }

                if(hMstarDvfsInfo.bCpuClockLevel == CONFIG_DVFS_STATE_LOW_SPEED)
                {
                    hMstarDvfsInfo.dwFinalCpuClock = \
                    hMstarDvfsInfo.dwPresetCpuClock = CONFIG_DVFS_CPU_CLOCK_PROTECTION;
                }
            }

            if(hMstarDvfsInfo.dwFinalCpuTemperature < dwLowerTemperature)
            {
                if(hMstarDvfsInfo.bCpuClockLevel == CONFIG_DVFS_STATE_LOW_SPEED)
                {
                    if(hMstarDvfsInfo.dwOverCounter < CONFIG_DVFS_AVERAGE_COUNT)
                    {
                        hMstarDvfsInfo.dwOverCounter ++;
                    }
                    else
                    {
                        DVFS_HAL_INFO("[DVFS] Current Temperature: %d\n", (unsigned int) hMstarDvfsInfo.dwFinalCpuTemperature);

                        //Adjust system power before raising CPU clock

                        if(hMstarDvfsInfo.bPowerControlLevel == CONFIG_DVFS_POWER_CONTROL_FREEZE)
                        {
                            dwRegisterValue = (hMstarDvfsInfo.dwVidSetting & 0x00FF);
                            if((dwRegisterValue > 0) && (dwRegisterValue < CONFIG_DVFS_CPU_POWER_DEFAULT))
                            {
                                MHalDvfsCpuPowerAdjustment(CONFIG_DVFS_CPU_POWER_INIT_MIN_LOWER_BOUND);
                            }
                            else
                            {
                                MHalDvfsCpuPowerAdjustment(CONFIG_DVFS_CPU_POWER_INIT_MIN_UPPER_BOUND);
                            }
                        }
                        else
                        {
                            //Adjust CPU Power (VDDC_CPU)
                            dwRegisterValue = (hMstarDvfsInfo.dwVidSetting & 0x00FF);
                            if((dwRegisterValue > 0) && (dwRegisterValue < CONFIG_DVFS_CPU_POWER_DEFAULT))
                            {
                                MHalDvfsCpuPowerAdjustment(dwRegisterValue);
                            }
                            else
                            {
                                MHalDvfsCpuPowerAdjustment(CONFIG_DVFS_CPU_POWER_DEFAULT);
                            }
                        }

                        if(hMstarDvfsInfo.bPowerControlLevel == CONFIG_DVFS_POWER_CONTROL_FREEZE)
                        {
                            //Adjust Core Power (VDDC)
                            MHalDvfsCorePowerAdjustment(CONFIG_DVFS_CORE_POWER_INIT);
                        }
                        else
                        {
                            dwRegisterValue = ((hMstarDvfsInfo.dwVidSetting & 0xFF00) >> 8);
                            if((dwRegisterValue > 0) && (dwRegisterValue < CONFIG_DVFS_CORE_POWER_DEFAULT))
                            {
                                MHalDvfsCorePowerAdjustment(dwRegisterValue);
                            }
                            else
                            {
                                MHalDvfsCorePowerAdjustment(CONFIG_DVFS_CORE_POWER_DEFAULT);
                            }
                        }

                        //Raising CPU Clock after adjust system power
                        MHalDvfsCpuClockAdjustment(
                                            hMstarDvfsInfo.wCpuClockSetting_Low,
                                            hMstarDvfsInfo.wCpuClockSetting_High,
                                            DVFS_CPU_CLOCK_UP
                                            );

                        DVFS_HAL_INFO("[DVFS] Change to Higher CPU Clock Setting\n");

                        hMstarDvfsInfo.bCpuClockLevel = CONFIG_DVFS_STATE_HIGH_SPEED;
                        hMstarDvfsInfo.dwOverCounter = 0;
                    }
                }
            }
            else
            {
                if(hMstarDvfsInfo.bCpuClockLevel == CONFIG_DVFS_STATE_LOW_SPEED)
                {
                    hMstarDvfsInfo.dwFinalCpuClock = \
                    hMstarDvfsInfo.dwPresetCpuClock = CONFIG_DVFS_CPU_CLOCK_PROTECTION;
                }
            }

            dwRegisterValue = *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x10050e << 1));
            if((dwRegisterValue != CONFIG_DVFS_DYNAMIC_CLOCK_ADJUST_INIT) || \
               (hMstarDvfsInfo.dwPresetCpuClock != hMstarDvfsInfo.dwFinalCpuClock) || \
               (hMstarDvfsInfo.bPowerControlChange != 0))
            {
                hMstarDvfsInfo.bPowerControlChange = 0;

                //Adjust Special CPU Clock by Specific Register 0x1005_0E[15:0]
                {
                    S32     dwSpecificCpuClock = 0;
                    U32     dwCurrentCpuClock = 0;

                    if(dwRegisterValue == CONFIG_DVFS_DYNAMIC_CLOCK_ADJUST_INIT)
                    {
                        dwRegisterValue = hMstarDvfsInfo.dwPresetCpuClock;
                        DVFS_HAL_DEBUG("[DVFS] Change to Specific CPU Clock: %dMHz\n", (unsigned int) dwRegisterValue);
                    }
                    else
                    {
                        hMstarDvfsInfo.dwPresetCpuClock = dwRegisterValue;
                        DVFS_HAL_DEBUG("[DVFS] Change to Special CPU Clock: %dMHz\n", (unsigned int) dwRegisterValue);
                    }

                    dwSpecificCpuClock = ((2717909UL / dwRegisterValue) * 1000);

                    dwCurrentCpuClock = *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x100502 << 1));

                    MHalDvfsPowerControlStateUpdate();

                    dwRegisterValue = *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x10050e << 1));
                    if(dwRegisterValue == CONFIG_DVFS_DYNAMIC_CLOCK_ADJUST_INIT)
                    {
                       dwRegisterValue = hMstarDvfsInfo.dwPresetCpuClock;
                    }

                    if(dwCurrentCpuClock >= dwRegisterValue)
                    {
                        //Falling CPU Clock before adjust system power
                        MHalDvfsCpuClockAdjustment(
                                            (dwSpecificCpuClock & 0xFFFF),
                                            ((dwSpecificCpuClock >> 16) & 0xFFFF),
                                            DVFS_CPU_CLOCK_UP
                                            );
                        mdelay(CONFIG_DVFS_DELAY_US);
                    }

                    if(dwRegisterValue >= CONFIG_DVFS_CPU_CLOCK_L3)
                    {
#if 0
                        U32     dwCornerChipType = CONFIG_DVFS_SS_CORNER_CHIP_NORMAL;

                        dwCornerChipType = *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x100506 << 1));
                        dwCornerChipType &= CONFIG_DVFS_FF_CORNER_CHIP_MASK;

                        if(dwCornerChipType == CONFIG_DVFS_FF_CORNER_CHIP_MAX)
                        {
                            if(hMstarDvfsInfo.bPowerControlLevel == CONFIG_DVFS_POWER_CONTROL_FREEZE)
                            {
                                dwRegisterValue = (hMstarDvfsInfo.dwVidSetting & 0x00FF);
                                if((dwRegisterValue > 0) && (dwRegisterValue < CONFIG_DVFS_CPU_POWER_DEFAULT))
                                {
                                    MHalDvfsCpuPowerAdjustment(CONFIG_DVFS_CPU_POWER_INIT_MAX_EX_LOWER_BOUND);
                                }
                                else
                                {
                                    MHalDvfsCpuPowerAdjustment(CONFIG_DVFS_CPU_POWER_INIT_MAX_UPPER_BOUND);
                                }
                            }
                            else
                            {
                                dwRegisterValue = (hMstarDvfsInfo.dwVidSetting & 0x00FF);
                                if((dwRegisterValue > 0) && (dwRegisterValue < CONFIG_DVFS_CPU_POWER_DEFAULT))
                                {
                                    MHalDvfsCpuPowerAdjustment(CONFIG_DVFS_CPU_POWER_MAX_EX_LOWER_BOUND);
                                }
                                else
                                {
                                    MHalDvfsCpuPowerAdjustment(CONFIG_DVFS_CPU_POWER_MAX_UPPER_BOUND);
                                }
                            }
                        }
                        else
#endif
                        {
                            if(hMstarDvfsInfo.bPowerControlLevel == CONFIG_DVFS_POWER_CONTROL_FREEZE)
                            {
                                dwRegisterValue = (hMstarDvfsInfo.dwVidSetting & 0x00FF);
                                if((dwRegisterValue > 0) && (dwRegisterValue < CONFIG_DVFS_CPU_POWER_DEFAULT))
                                {
                                    MHalDvfsCpuPowerAdjustment(CONFIG_DVFS_CPU_POWER_INIT_MAX_LOWER_BOUND);
                                }
                                else
                                {
                                    MHalDvfsCpuPowerAdjustment(CONFIG_DVFS_CPU_POWER_INIT_MAX_UPPER_BOUND);
                                }
                            }
                            else
                            {
                                dwRegisterValue = (hMstarDvfsInfo.dwVidSetting & 0x00FF);
                                if((dwRegisterValue > 0) && (dwRegisterValue < CONFIG_DVFS_CPU_POWER_DEFAULT))
                                {
                                    MHalDvfsCpuPowerAdjustment(CONFIG_DVFS_CPU_POWER_MAX_LOWER_BOUND);
                                }
                                else
                                {
                                    MHalDvfsCpuPowerAdjustment(CONFIG_DVFS_CPU_POWER_MAX_UPPER_BOUND);
                                }
                            }
                        }
                    }
#if 0
                    else if(dwRegisterValue > CONFIG_DVFS_CPU_CLOCK_L2)
                    {
                        if(hMstarDvfsInfo.bPowerControlLevel == CONFIG_DVFS_POWER_CONTROL_FREEZE)
                        {
                            dwRegisterValue = (hMstarDvfsInfo.dwVidSetting & 0x00FF);
                            if((dwRegisterValue > 0) && (dwRegisterValue < CONFIG_DVFS_CPU_POWER_DEFAULT))
                            {
                                MHalDvfsCpuPowerAdjustment(CONFIG_DVFS_CPU_POWER_INIT_MID_EX_LOWER_BOUND);
                            }
                            else
                            {
                                MHalDvfsCpuPowerAdjustment(CONFIG_DVFS_CPU_POWER_INIT_MID_EX_UPPER_BOUND);
                            }
                        }
                        else
                        {
                            dwRegisterValue = (hMstarDvfsInfo.dwVidSetting & 0x00FF);
                            if((dwRegisterValue > 0) && (dwRegisterValue < CONFIG_DVFS_CPU_POWER_DEFAULT))
                            {
                                MHalDvfsCpuPowerAdjustment(CONFIG_DVFS_CPU_POWER_MID_EX_LOWER_BOUND);
                            }
                            else
                            {
                                MHalDvfsCpuPowerAdjustment(CONFIG_DVFS_CPU_POWER_MID_EX_UPPER_BOUND);
                            }
                        }
                    }
#endif
                    else if(dwRegisterValue > CONFIG_DVFS_CPU_CLOCK_L1)
                    {
                        if(hMstarDvfsInfo.bPowerControlLevel == CONFIG_DVFS_POWER_CONTROL_FREEZE)
                        {
                            dwRegisterValue = (hMstarDvfsInfo.dwVidSetting & 0x00FF);
                            if((dwRegisterValue > 0) && (dwRegisterValue < CONFIG_DVFS_CPU_POWER_DEFAULT))
                            {
                                MHalDvfsCpuPowerAdjustment(CONFIG_DVFS_CPU_POWER_INIT_MID_LOWER_BOUND);
                            }
                            else
                            {
                                MHalDvfsCpuPowerAdjustment(CONFIG_DVFS_CPU_POWER_INIT_MID_UPPER_BOUND);
                            }
                        }
                        else
                        {
                            dwRegisterValue = (hMstarDvfsInfo.dwVidSetting & 0x00FF);
                            if((dwRegisterValue > 0) && (dwRegisterValue < CONFIG_DVFS_CPU_POWER_DEFAULT))
                            {
                                MHalDvfsCpuPowerAdjustment(CONFIG_DVFS_CPU_POWER_MID_LOWER_BOUND);
                            }
                            else
                            {
                                MHalDvfsCpuPowerAdjustment(CONFIG_DVFS_CPU_POWER_MID_UPPER_BOUND);
                            }
                        }
                    }
                    else if(dwRegisterValue >= CONFIG_DVFS_CPU_CLOCK_L0)
                    {
                        if(hMstarDvfsInfo.bPowerControlLevel == CONFIG_DVFS_POWER_CONTROL_FREEZE)
                        {
                            dwRegisterValue = (hMstarDvfsInfo.dwVidSetting & 0x00FF);
                            if((dwRegisterValue > 0) && (dwRegisterValue < CONFIG_DVFS_CPU_POWER_DEFAULT))
                            {
                                MHalDvfsCpuPowerAdjustment(CONFIG_DVFS_CPU_POWER_INIT_MIN_LOWER_BOUND);
                            }
                            else
                            {
                                MHalDvfsCpuPowerAdjustment(CONFIG_DVFS_CPU_POWER_INIT_MIN_UPPER_BOUND);
                            }
                        }
                        else
                        {
                            //Adjust CPU Power (VDDC_CPU)
                            dwRegisterValue = (hMstarDvfsInfo.dwVidSetting & 0x00FF);
                            if((dwRegisterValue > 0) && (dwRegisterValue < CONFIG_DVFS_CPU_POWER_DEFAULT))
                            {
                                MHalDvfsCpuPowerAdjustment(dwRegisterValue);
                            }
                            else
                            {
                                MHalDvfsCpuPowerAdjustment(CONFIG_DVFS_CPU_POWER_MIN_UPPER_BOUND);
                            }
                        }
                    }
                    else if(dwRegisterValue <= CONFIG_DVFS_CPU_CLOCK_MIN)
                    {
                        if(hMstarDvfsInfo.bPowerControlLevel == CONFIG_DVFS_POWER_CONTROL_FREEZE)
                        {
                            dwRegisterValue = (hMstarDvfsInfo.dwVidSetting & 0x00FF);
                            if((dwRegisterValue > 0) && (dwRegisterValue < CONFIG_DVFS_CPU_POWER_DEFAULT))
                            {
                                MHalDvfsCpuPowerAdjustment(CONFIG_DVFS_CPU_POWER_INIT_MIN_LOWER_BOUND);
                            }
                            else
                            {
                                MHalDvfsCpuPowerAdjustment(CONFIG_DVFS_CPU_POWER_INIT_MIN_UPPER_BOUND);
                            }
                        }
                        else
                        {
                            //Adjust CPU Power (VDDC_CPU)
                            dwRegisterValue = (hMstarDvfsInfo.dwVidSetting & 0x00FF);
                            if((dwRegisterValue > 0) && (dwRegisterValue < CONFIG_DVFS_CPU_POWER_DEFAULT))
                            {
                                //MHalDvfsCpuPowerAdjustment(dwRegisterValue - CONFIG_DVFS_CPU_POWER_DROP_UPPER_BOUND);
                                MHalDvfsCpuPowerAdjustment(CONFIG_DVFS_CPU_POWER_PROTECTION_LOWER_BOUND);
                            }
                            else
                            {
                                //MHalDvfsCpuPowerAdjustment(CONFIG_DVFS_CPU_POWER_MIN_UPPER_BOUND - CONFIG_DVFS_CPU_POWER_DROP_UPPER_BOUND);
                                MHalDvfsCpuPowerAdjustment(CONFIG_DVFS_CPU_POWER_PROTECTION_UPPER_BOUND);
                            }
                        }
                    }
                    else
                    {
                        if(hMstarDvfsInfo.bPowerControlLevel == CONFIG_DVFS_POWER_CONTROL_FREEZE)
                        {
                            dwRegisterValue = (hMstarDvfsInfo.dwVidSetting & 0x00FF);
                            if((dwRegisterValue > 0) && (dwRegisterValue < CONFIG_DVFS_CPU_POWER_DEFAULT))
                            {
                                MHalDvfsCpuPowerAdjustment(CONFIG_DVFS_CPU_POWER_INIT_MIN_LOWER_BOUND);
                            }
                            else
                            {
                                MHalDvfsCpuPowerAdjustment(CONFIG_DVFS_CPU_POWER_INIT_MIN_UPPER_BOUND);
                            }
                        }
                        else
                        {
                            dwRegisterValue = (hMstarDvfsInfo.dwVidSetting & 0x00FF);
                            if((dwRegisterValue > 0) && (dwRegisterValue < CONFIG_DVFS_CPU_POWER_DEFAULT))
                            {
                                MHalDvfsCpuPowerAdjustment(dwRegisterValue);
                            }
                            else
                            {
                                MHalDvfsCpuPowerAdjustment(CONFIG_DVFS_CPU_POWER_DEFAULT);
                            }
                        }
                    }

                    if(hMstarDvfsInfo.bPowerControlLevel == CONFIG_DVFS_POWER_CONTROL_FREEZE)
                    {
                        //Adjust Core Power (VDDC)
                        MHalDvfsCorePowerAdjustment(CONFIG_DVFS_CORE_POWER_INIT);
                    }
                    else
                    {
                        dwRegisterValue = ((hMstarDvfsInfo.dwVidSetting & 0xFF00) >> 8);
                        if((dwRegisterValue > 0) && (dwRegisterValue < CONFIG_DVFS_CORE_POWER_DEFAULT))
                        {
                            MHalDvfsCorePowerAdjustment(dwRegisterValue);
                        }
                        else
                        {
                            MHalDvfsCorePowerAdjustment(CONFIG_DVFS_CORE_POWER_DEFAULT);
                        }
                    }

                    dwRegisterValue = *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x10050e << 1));
                    if(dwRegisterValue == CONFIG_DVFS_DYNAMIC_CLOCK_ADJUST_INIT)
                    {
                       dwRegisterValue = hMstarDvfsInfo.dwPresetCpuClock;
                    }

                    if(dwCurrentCpuClock < dwRegisterValue)
                    {
                        //Raising CPU Clock after adjust system power
                        MHalDvfsCpuClockAdjustment(
                                            (dwSpecificCpuClock & 0xFFFF),
                                            ((dwSpecificCpuClock >> 16) & 0xFFFF),
                                            DVFS_CPU_CLOCK_UP
                                            );
                        mdelay(CONFIG_DVFS_DELAY_US);
                    }
                    hMstarDvfsInfo.bCpuClockLevel = CONFIG_DVFS_STATE_HIGH_SPEED;
                    hMstarDvfsInfo.dwOverCounter = 0;
                    *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x10050e << 1)) = CONFIG_DVFS_DYNAMIC_CLOCK_ADJUST_INIT;
                }

                //Adjust Special CPU Power by Specific Register 0x1005_10[15:0]
                dwRegisterValue = *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x100510 << 1));
                if(dwRegisterValue != CONFIG_DVFS_DYNAMIC_CLOCK_ADJUST_INIT)
                {
                    if((dwRegisterValue > 80) && (dwRegisterValue <= 160))
                    {
                        DVFS_HAL_DEBUG("\033[33m[DVFS] Change to Special CPU Power: %d0mV\033[m\n", (unsigned int) dwRegisterValue);
                        MHalDvfsCpuPowerAdjustment(dwRegisterValue);
                    }
                    *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x100510 << 1)) = CONFIG_DVFS_DYNAMIC_CLOCK_ADJUST_INIT;
                }

                //Adjust Special Core Power by Specific Register 0x1005_12[15:0]
                dwRegisterValue = *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x100512 << 1));
                if(dwRegisterValue != CONFIG_DVFS_DYNAMIC_CLOCK_ADJUST_INIT)
                {
                    if((dwRegisterValue > 80) && (dwRegisterValue <= 160))
                    {
                        DVFS_HAL_DEBUG("\033[33m[DVFS] Change to Special Core Power: %d0mV\033[m\n", (unsigned int) dwRegisterValue);
                        MHalDvfsCpuPowerAdjustment(dwRegisterValue);
                    }
                    *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x100512 << 1)) = CONFIG_DVFS_DYNAMIC_CLOCK_ADJUST_INIT;
                }
            }
        }
        //=========================================================
        //Read 25 degree in PM side
        //MHalDvfsRefTemperature();
        //MsOS_ReleaseMutex(_s32SAR_Dvfs_Mutex);
    }
    else
    {
        //Disable DVFS
        *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x110cb0 << 1)) = 0;
    }

    hMstarDvfsInfo.dwFinalCpuClock = hMstarDvfsInfo.dwPresetCpuClock;

    return hMstarDvfsInfo.dwFinalCpuClock;
}

//=================================================================================================
void MHalDvfsInit(void)
{
    //Init Basic Register Settings
    //- 0x1005_00[15:0] = 0x3697    //Enable
    //- 0x1005_02[15:0] = 900       //Current CPU Clock
    //- 0x1005_04[15:0] = 25        //Current Temperature (CPU)
    //- 0x1005_06[15:2]             //Reserved
    //- 0x1005_06[1:0]              //Corner Chip Type
    //- 0x1005_08[15:8] = 130       //Upper Bound of T-sensor
    //- 0x1005_08[7:0] = 120        //Lower Bound of T-sensor
    //- 0x1005_0a[15:12] = ??       //Offset of CPU Power
    //- 0x1005_0a[11:8] = ??        //Offset of Core Power
    //- 0x1005_0a[7:0] = 150        //Upper Bound of Global Reset
    //- 0x1005_0c[15:8] = 100       //Current Core Power
    //- 0x1005_0c[7:0] = 110        //Current CPU Power
    //- 0x1005_0e[15:0] = 0x2454    //Dynamic CPU Clock Adjustment
    //- 0x1005_10[15:0] = 0x2454    //Dynamic CPU Power Adjustment
    //- 0x1005_12[15:0] = 0x2454    //Dynamic Core Power Adjustment

    *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x110cb2 << 1)) &= ~(0x01);

    //Get Reference Level of 25-degree Temperature in eFuse
    MHalDvfsRefTemperature();

    //Init Test Bus to Measure CPU Clock
    MHalDvfsCpuDisplayInit();

    hMstarDvfsInfo.dwVidSetting = *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x10050c << 1));
    hMstarDvfsInfo.wCpuClockSetting_Low = *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x110ca4 << 1));
    hMstarDvfsInfo.wCpuClockSetting_High = *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x110ca6 << 1));

    hMstarDvfsInfo.bCpuClockLevel = CONFIG_DVFS_STATE_INIT;
    hMstarDvfsInfo.bDvfsInitOk = 1;
}

//=================================================================================================
void MHalDvfsCpuDisplayInit(void)
{
#if 0
#if CONFIG_DVFS_CPU_CLOCK_DISPLAY_ENABLE
    U32  dwRegisterValue = 0;

    //Init Test Bus to Measure CPU Clock
    dwRegisterValue = *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x101896 << 1));
    dwRegisterValue &= ~(0x07);
    dwRegisterValue |= 0x01;
    *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x101896 << 1)) = dwRegisterValue;
    *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x101eea << 1)) = 0;
    dwRegisterValue = *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x101eea << 1));
    dwRegisterValue &= ~(0x07);
    dwRegisterValue |= 0x04;
    dwRegisterValue &= ~(0x01 << 4);
    dwRegisterValue &= ~(0x01 << 5);
    dwRegisterValue &= ~(0x01 << 6);
    dwRegisterValue |= (0x01 << 14);
    *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x101eea << 1)) = dwRegisterValue;
    *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x101eee << 1)) = 0x001F;
    *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x101e62 << 1)) = 0;
    *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x101e62 << 1)) = 1;
#endif
#endif
}

//=================================================================================================
void MHalDvfsCpuDisplay(void)
{
#if 0
#if CONFIG_DVFS_CPU_CLOCK_DISPLAY_ENABLE
    U32     dwRegisterValue = 0;

    //CPU Clock = Register 0x101E_64[15:0] * 12 / 1000 * 4
    dwRegisterValue = *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x101e64 << 1));
    dwRegisterValue *= 12;
    dwRegisterValue /= 1000;
    dwRegisterValue *= 4;
    *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x100502 << 1)) = dwRegisterValue;

    DVFS_HAL_DEBUG("[DVFS] Current CPU Clock: %dMHz\n", (unsigned int) dwRegisterValue);
#endif
#else
    U32     dwRegisterValue = 0;

    dwRegisterValue = *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x100502 << 1));

    DVFS_HAL_DEBUG("[DVFS] Current CPU Clock: %dMHz\n", (unsigned int) dwRegisterValue);
#endif
}

//=================================================================================================
void MHalDvfsCpuTemperature(void)
{
    S32     dwCoreChipValue = 0;
    S32     dwChipTempValue = 0;

    //Read CH8 of PM_SAR to Get CPU Temperature
    *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x001400 << 1)) |= (0x01 << 14);
    mdelay(CONFIG_DVFS_DELAY_US);
    dwCoreChipValue = *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x00148e << 1));    //Channel 8

    hMstarDvfsInfo.dwAvgTempBufferCpu[hMstarDvfsInfo.dwAvgTempCounterCpu] = dwCoreChipValue;
    hMstarDvfsInfo.dwAvgTempCounterCpu ++;
    if(hMstarDvfsInfo.dwAvgTempCounterCpu >= CONFIG_DVFS_DATA_COUNT)
    {
        U32    dwTempCounter = 0;
        U32    dwTempValue = 0;

        DVFS_HAL_DEBUG("[DVFS] .................. Start\n");
        DVFS_HAL_DEBUG("[DVFS] 25-degree Data (CPU): 0x%04x\n", (unsigned int) hMstarDvfsInfo.dwRefTemperature);
        DVFS_HAL_DEBUG("[DVFS] Temperature Sensor Data (CPU): ");
        for(dwTempCounter = 0; dwTempCounter < CONFIG_DVFS_DATA_COUNT; dwTempCounter ++)
        {
            //DVFS_HAL_DEBUG(" - %d: 0x%04x\n", dwTempCounter, dwAvgTempBufferCpu[dwTempCounter]);
            dwTempValue += hMstarDvfsInfo.dwAvgTempBufferCpu[dwTempCounter];
        }
        dwTempValue /= CONFIG_DVFS_DATA_COUNT;

        DVFS_HAL_DEBUG("0x%04x\n", (unsigned int) dwTempValue);

        //dwChipTempValue = ((((((dwRefTemperature - dwTempValue) * 1000) * 2000) / 1024) / 1.45) + 28000);
        if(hMstarDvfsInfo.dwRefTemperature >= dwTempValue)
        {
            dwChipTempValue = (((hMstarDvfsInfo.dwRefTemperature - dwTempValue) * 1347) + CONFIG_DVFS_T_SENSOR_SHIFT);
        }
        else
        {
            dwChipTempValue = ((dwTempValue - hMstarDvfsInfo.dwRefTemperature) * 1347);
            dwChipTempValue = (CONFIG_DVFS_T_SENSOR_SHIFT - dwChipTempValue);
        }

        hMstarDvfsInfo.dwFinalCpuTemperature = (dwChipTempValue / 1000);
        DVFS_HAL_DEBUG("[DVFS] Average Temperature (CPU): %d\n", (unsigned int) hMstarDvfsInfo.dwFinalCpuTemperature);
        *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x100504 << 1)) = hMstarDvfsInfo.dwFinalCpuTemperature;

        hMstarDvfsInfo.dwAvgTempCounterCpu = 0;

        MHalDvfsCpuDisplay();

        DVFS_HAL_DEBUG("[DVFS] .................. End\n");
    }
}

//=================================================================================================
void MHalDvfsRefTemperature(void)
{
    U32     dwRegisterValue = 0;

    //Read 25-degree Reference Level in eFuse
    *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x00204e << 1)) = 0x0027;
    *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x00204c << 1)) = 0x0001;
    while((*(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x00204c << 1)) & 0x0001) != 0);

    dwRegisterValue = *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x002084 << 1));
    dwRegisterValue >>= 6;

    //If no existed, set the default reference level is 406
    hMstarDvfsInfo.dwRefTemperature = dwRegisterValue;
    if(dwRegisterValue == 0)
    {
        hMstarDvfsInfo.dwRefTemperature = 406;
    }
}

//=================================================================================================
void MHalDvfsCpuClockAdjustment(U32 dwDvfsUpperBoundCpuClock_Low, U32 dwDvfsUpperBoundCpuClock_High, U32 bCpuClockRisingFlag)
{
    *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x110ca4 << 1)) = dwDvfsUpperBoundCpuClock_Low;
    *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x110ca6 << 1)) = dwDvfsUpperBoundCpuClock_High;

    *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x110cb0 << 1)) = 0x01;   //switch to LPF control
    *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x110caa << 1)) = 0x06;   //mu[2:0]
    *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x110cae << 1)) = 0x08;   //lpf_update_cnt[7:0]

    if(bCpuClockRisingFlag == DVFS_CPU_CLOCK_UP)
    {
        //Set LPF is Low to High
        *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x110cb2 << 1)) |= (0x01 << 12);
    }
    else
    {
        //Set LPF is High to Low
        *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x110cb2 << 1)) &= ~(0x01 << 12);
    }

    *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x110ca8 << 1)) = 0x00;
    *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x110ca8 << 1)) = 0x01;

    hMstarDvfsInfo.dwPresetCpuClock = (2717909000UL / ((dwDvfsUpperBoundCpuClock_High << 16) | dwDvfsUpperBoundCpuClock_Low));
    *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x100502 << 1)) = hMstarDvfsInfo.dwPresetCpuClock;
}

//=================================================================================================
void MHalDvfsPowerControlStateUpdate(void)
{
    U32     dwRegisterValue = 0;

    dwRegisterValue = *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x100504 << 1));

    if(hMstarDvfsInfo.bPowerControlLevel == CONFIG_DVFS_POWER_CONTROL_FREEZE)
    {
        if(dwRegisterValue >= CONFIG_DVFS_POWER_CONTROL_UPPER_BOUND)
        {
            hMstarDvfsInfo.bPowerControlLevel = CONFIG_DVFS_POWER_CONTROL_NORMAL;
            hMstarDvfsInfo.bPowerControlChange = 1;
        }
    }
    else
    {
        if(dwRegisterValue < CONFIG_DVFS_POWER_CONTROL_LOWER_BOUND)
        {
            hMstarDvfsInfo.bPowerControlLevel = CONFIG_DVFS_POWER_CONTROL_FREEZE;
            hMstarDvfsInfo.bPowerControlChange = 1;
        }
    }
}

//=================================================================================================
void MHalDvfsCpuPowerInit(void)
{
    //TBD
}

//=================================================================================================
void MHalDvfsCpuPowerAdjustment(U32 dwCpuPowerVoltage)
{
    MDrvDvfsVoltageSetup(0, dwCpuPowerVoltage, 0);

#if 0
    U32     dwRegisterValue = 0;

    dwRegisterValue = *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x100504 << 1));

    if(hMstarDvfsInfo.bPowerControlLevel == CONFIG_DVFS_POWER_CONTROL_FREEZE)
    {
        DVFS_HAL_DEBUG("[DVFS] %s: %d\n", __FUNCTION__, __LINE__);
        if(dwRegisterValue >= CONFIG_DVFS_POWER_CONTROL_UPPER_BOUND)
        {
            DVFS_HAL_DEBUG("[DVFS] %s: %d\n", __FUNCTION__, __LINE__);
            MDrvDvfsVoltageSetup(0, dwCpuPowerVoltage, 0);
            hMstarDvfsInfo.bPowerControlLevel = CONFIG_DVFS_POWER_CONTROL_NORMAL;
        }
    }
    else
    {
        DVFS_HAL_DEBUG("[DVFS] %s: %d\n", __FUNCTION__, __LINE__);
        if(dwRegisterValue < CONFIG_DVFS_POWER_CONTROL_LOWER_BOUND)
        {
            DVFS_HAL_DEBUG("[DVFS] %s: %d\n", __FUNCTION__, __LINE__);
            MDrvDvfsVoltageSetup(0, CONFIG_DVFS_CPU_POWER_INIT, 0);
            hMstarDvfsInfo.bPowerControlLevel = CONFIG_DVFS_POWER_CONTROL_FREEZE;
        }
        else
        {
            DVFS_HAL_DEBUG("[DVFS] %s: %d\n", __FUNCTION__, __LINE__);
            MDrvDvfsVoltageSetup(0, dwCpuPowerVoltage, 0);
        }
    }
#endif
}

//=================================================================================================
void MHalDvfsCorePowerInit(void)
{
    //TBD
}

//=================================================================================================
void MHalDvfsCorePowerAdjustment(U32 dwCorePowerVoltage)
{
//  MDrvDvfsVoltageSetup(0, dwCorePowerVoltage, 1);

#if 0
    U32     dwRegisterValue = 0;

    dwRegisterValue = *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x100504 << 1));

    if(hMstarDvfsInfo.bPowerControlLevel == CONFIG_DVFS_POWER_CONTROL_FREEZE)
    {
        DVFS_HAL_DEBUG("[DVFS] %s: %d\n", __FUNCTION__, __LINE__);
        if(dwRegisterValue >= CONFIG_DVFS_POWER_CONTROL_UPPER_BOUND)
        {
            DVFS_HAL_DEBUG("[DVFS] %s: %d\n", __FUNCTION__, __LINE__);
            MDrvDvfsVoltageSetup(0, dwCorePowerVoltage, 1);
            hMstarDvfsInfo.bPowerControlLevel = CONFIG_DVFS_POWER_CONTROL_NORMAL;
        }
    }
    else
    {
        DVFS_HAL_DEBUG("[DVFS] %s: %d\n", __FUNCTION__, __LINE__);
        if(dwRegisterValue < CONFIG_DVFS_POWER_CONTROL_LOWER_BOUND)
        {
            DVFS_HAL_DEBUG("[DVFS] %s: %d\n", __FUNCTION__, __LINE__);
            MDrvDvfsVoltageSetup(0, CONFIG_DVFS_CORE_POWER_INIT, 0);
            hMstarDvfsInfo.bPowerControlLevel = CONFIG_DVFS_POWER_CONTROL_FREEZE;
        }
        else
        {
            DVFS_HAL_DEBUG("[DVFS] %s: %d\n", __FUNCTION__, __LINE__);
            MDrvDvfsVoltageSetup(0, dwCorePowerVoltage, 1);
        }
    }
#endif
}

//=================================================================================================
U32 MHalDvfsQueryCpuClockByTemperature(void)
{
    U32     dwRegisterValue = 0;

    dwRegisterValue = *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x100500 << 1));
    if(dwRegisterValue == CONFIG_DVFS_ENABLE_PATTERN)
    {
        S32     dwUpperTemperature = 0;
        S32     dwLowerTemperature = 0;
        S32     dwResetTemperature = 0;

        if(hMstarDvfsInfo.bDvfsInitOk != 0)
        {
            //Get current temperature in cpu side
            MHalDvfsCpuTemperature();

            //Get upper bound temperature in user defined register
            dwUpperTemperature = *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x100508 << 1));
            dwUpperTemperature >>= 8;
            if(dwUpperTemperature == 0)
            {
                dwUpperTemperature = CONFIG_DVFS_DEFAULT_UPPER_BOUND;
            }

            //Get lower bound temperature in user defined register
            dwLowerTemperature = *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x100508 << 1));
            dwLowerTemperature &= 0xFF;
            if(dwLowerTemperature == 0)
            {
                dwLowerTemperature = CONFIG_DVFS_DEFAULT_LOWER_BOUND;
            }

            //Get maximum bound temperature in user defined register
            dwResetTemperature = *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x10050a << 1));
            dwResetTemperature &= 0xFF;
            if(dwResetTemperature == 0)
            {
                dwResetTemperature = CONFIG_DVFS_DEFAULT_RESET_THRESHOLD;
            }

            if((hMstarDvfsInfo.dwFinalCpuTemperature > dwResetTemperature) && \
               (hMstarDvfsInfo.bCpuClockLevel != CONFIG_DVFS_STATE_INIT))
            {
                //If exceed maximum temperature, the system will trigger a reset to avoid system crash.

                if(hMstarDvfsInfo.dwTemperatureCounter < CONFIG_DVFS_AVERAGE_COUNT)
                {
                    DVFS_HAL_DEBUG("\033[33m[DVFS] Over Temperature Protection: %d\033[m\n", (unsigned int) hMstarDvfsInfo.dwTemperatureCounter);
                    hMstarDvfsInfo.dwTemperatureCounter ++;
                }
                else
                {
                    //Trigger a WDT Reset
                    DVFS_HAL_INFO("\033[33m[DVFS] Over Temperature Protection\033[m\n");
                    *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x00300a << 1)) = 0x00;
                    *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x003008 << 1)) = 0x00;
                    *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x00300a << 1)) = 0x05;
                    *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x003000 << 1)) = 0x01;

                    while(1);
                }
            }
            else if((hMstarDvfsInfo.dwFinalCpuTemperature > dwUpperTemperature) && \
                    (hMstarDvfsInfo.bCpuClockLevel == CONFIG_DVFS_STATE_HIGH_SPEED))
            {
                //If exceed upper bound temperature, the system will adjust lower cpu clock to avoid overheat issue,

                if(hMstarDvfsInfo.dwTemperatureCounter < CONFIG_DVFS_AVERAGE_COUNT)
                {
                    hMstarDvfsInfo.dwTemperatureCounter ++;
                }
                else
                {
//                  dwRegisterValue = *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x110ca0 << 1));
//                  dwRegisterValue |= (*(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x110ca2 << 1)) << 16);
//                  hMstarDvfsInfo.dwValidCpuClockByTemperature = (2717909000UL / dwRegisterValue);
//                  DVFS_HAL_INFO("[DVFS] **Support CPU Clock: %dMHz\n", (unsigned int) dwRegisterValue);

                    hMstarDvfsInfo.dwValidCpuClockByTemperature = CONFIG_DVFS_CPU_CLOCK_PROTECTION;
                    DVFS_HAL_INFO("\033[33m[DVFS] Over Temperature, Support CPU Clock: %dMHz\033[m\n", (unsigned int) hMstarDvfsInfo.dwValidCpuClockByTemperature);
                    hMstarDvfsInfo.dwTemperatureCounter = 0;
                }
            }
            else if((hMstarDvfsInfo.dwFinalCpuTemperature < dwLowerTemperature) && \
                    (hMstarDvfsInfo.bCpuClockLevel == CONFIG_DVFS_STATE_LOW_SPEED))
            {
                if(hMstarDvfsInfo.dwTemperatureCounter < CONFIG_DVFS_AVERAGE_COUNT)
                {
                    hMstarDvfsInfo.dwTemperatureCounter ++;
                }
                else
                {
                    hMstarDvfsInfo.dwValidCpuClockByTemperature = CONFIG_DVFS_CPU_CLOCK_MAX;
                    DVFS_HAL_INFO("\033[33m[DVFS] Normal Mode, Support CPU Clock: %dMHz\033[m\n", (unsigned int) hMstarDvfsInfo.dwValidCpuClockByTemperature);

                    hMstarDvfsInfo.dwTemperatureCounter = 0;
                }
            }
            else if((hMstarDvfsInfo.dwFinalCpuTemperature >= CONFIG_DVFS_POWER_CONTROL_UPPER_BOUND) &&
                    (hMstarDvfsInfo.bPowerControlLevel == CONFIG_DVFS_POWER_CONTROL_FREEZE))
            {
                if(hMstarDvfsInfo.dwTemperatureCounter < CONFIG_DVFS_AVERAGE_COUNT)
                {
                    hMstarDvfsInfo.dwTemperatureCounter ++;
                }
                else
                {
                    hMstarDvfsInfo.dwValidCpuClockByTemperature = CONFIG_DVFS_CPU_CLOCK_FREEZE_MODE;
                    DVFS_HAL_INFO("\033[33m[DVFS] Disable Freeze Mode\033[m\n");

                    hMstarDvfsInfo.dwTemperatureCounter = 0;
                }
            }
            else if((hMstarDvfsInfo.dwFinalCpuTemperature < CONFIG_DVFS_POWER_CONTROL_LOWER_BOUND) &&
                    (hMstarDvfsInfo.bPowerControlLevel == CONFIG_DVFS_POWER_CONTROL_NORMAL))
            {
                if(hMstarDvfsInfo.dwTemperatureCounter < CONFIG_DVFS_AVERAGE_COUNT)
                {
                    hMstarDvfsInfo.dwTemperatureCounter ++;
                }
                else
                {
                    hMstarDvfsInfo.dwValidCpuClockByTemperature = CONFIG_DVFS_CPU_CLOCK_FREEZE_MODE;
                    DVFS_HAL_INFO("\033[33m[DVFS] Enable Freeze Mode\033[m\n");

                    hMstarDvfsInfo.dwTemperatureCounter = 0;
                }
            }
            else
            {
                if(hMstarDvfsInfo.bCpuClockLevel == CONFIG_DVFS_STATE_INIT)
                {
                    hMstarDvfsInfo.dwValidCpuClockByTemperature = CONFIG_DVFS_CPU_CLOCK_DEFAULT;
                }
                else
                {
                    hMstarDvfsInfo.dwValidCpuClockByTemperature = CONFIG_DVFS_CPU_CLOCK_MAX;
                }
            }
        }
    }

    return hMstarDvfsInfo.dwValidCpuClockByTemperature;
}

//=================================================================================================
U32  MHalDvfsQueryTemperature(void)
{
    U32 dwRegisterValue = 0;

    dwRegisterValue = *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x100504 << 1));
    return dwRegisterValue;
}

//=================================================================================================
U32 MHalDvfsGetCpuTemperature(void)
{
    return 0;
}
//=================================================================================================