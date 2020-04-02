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

#include "mdrv_dvfs.h"

#ifndef __MHAL_DVFS_H__
#include "mhal_dvfs.h"
#endif

#ifndef __MHAL_DVFS_POWER_H__
#include "mhal_dvfs_power.h"
#endif

#include <linux/platform_device.h>
#include <linux/pm.h>

//=================================================================================================
static U32 dwPowerChipId = CONFIG_DVFS_CHIP_ID_UNKNOWN;
static U32 bDvfsPowerInitStatus = FALSE;
extern void MDrv_IIC_Init(void);
extern S32 MDrv_SW_IIC_Write(U8 u8ChIIC, U8 u8SlaveID, U8 u8AddrCnt, U8* pu8Addr, U32 u32BufLen, U8* pu8Buf);
extern S32 MDrv_SW_IIC_Read(U8 u8ChIIC, U8 u8SlaveID, U8 u8AddrCnt, U8* pu8Addr, U32 u32BufLen, U8* pu8Buf);

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: SysDvfsInit
/// @brief \b Function  \b Description: Read T-Sensor to Handle DVFS Flow
/// @param <IN>         \b None:
/// @param <OUT>        \b None:
/// @param <RET>        \b None:
/// @param <GLOBAL>     \b None:
////////////////////////////////////////////////////////////////////////////////
void SysDvfsPowerSuspend(void)
{
    bDvfsPowerInitStatus = false;
}

U32 SysDvfsPowerInit(void)
{
    if (bDvfsPowerInitStatus == TRUE)
        return TRUE;
    if(SysDvfsCpuPowerInit() == TRUE)
    {
        SysDvfsCpuPowerAdjustment(CONFIG_DVFS_CPU_POWER_DEFAULT);
    }
    else
    {
        bDvfsPowerInitStatus = FALSE;
        return bDvfsPowerInitStatus;
    }
/*
    if(SysDvfsCorePowerInit() == TRUE)
    {
        SysDvfsCorePowerAdjustment(CONFIG_DVFS_CORE_POWER_DEFAULT);
    }
    else
    {
        bDvfsPowerInitStatus = FALSE;
        return bDvfsPowerInitStatus;
    }
    //SysDvfsPradoOTPFlagInit();
*/
    bDvfsPowerInitStatus = TRUE;
    return bDvfsPowerInitStatus;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: SysDvfsCpuPowerAdjustment
/// @brief \b Function  \b Description: Update Output Voltage Level in External Power Chip
/// @param <IN>         \b None:
/// @param <OUT>        \b None:
/// @param <RET>        \b None:
/// @param <GLOBAL>     \b None:
////////////////////////////////////////////////////////////////////////////////
U32 SysDvfsCpuPowerAdjustment(U32 dwCpuPowerVoltage)
{
    U32 bDvfsCpuPowerAdjStatus = TRUE;
#if CONFIG_DVFS_CPU_POWER_I2C_ENABLE
    U32 dwRegisterValue = 0;
    U32  dwOriginalCpuPowerVoltage = 0;
    U32  dwSourceRegisterSetting = 0;
    U32  dwTargetRegisterSetting = 0;

    U8  byTargetRegAddress[5] =
        {
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF
        };
    U8  byTargetData[5] =
        {
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF
        };

    if (dwCpuPowerVoltage == 0)
        return bDvfsCpuPowerAdjStatus;

    if(dwPowerChipId == CONFIG_DVFS_CHIP_ID_PRADO)
    {
        byTargetRegAddress[0] = 0x10;
        byTargetRegAddress[1] = (0x06 << 1);
        if (MDrv_SW_IIC_Read(CONFIG_DVFS_POWER_SWI2C_BUS, CONFIG_DVFS_POWER_SWI2C_CPU_ID, 2, byTargetRegAddress, 2, byTargetData) != -1)
        {
            dwOriginalCpuPowerVoltage = (unsigned int) byTargetData[1] + CONFIG_DVFS_CPU_POWER_SHIFT_PRADO;
            DVFS_HAL_DEBUG("\033[33m[INFO] Orginal CPU Power: %d0 mV\033[m\n", (unsigned int) dwOriginalCpuPowerVoltage);
        }

        dwSourceRegisterSetting = (dwOriginalCpuPowerVoltage - CONFIG_DVFS_CPU_POWER_SHIFT_PRADO);
        dwTargetRegisterSetting = (dwCpuPowerVoltage - CONFIG_DVFS_CPU_POWER_SHIFT_PRADO);

        if (bDvfsPowerInitStatus == TRUE)
        {
            if (dwCpuPowerVoltage > dwOriginalCpuPowerVoltage)
            {
                for (;dwSourceRegisterSetting <= dwTargetRegisterSetting; dwSourceRegisterSetting += CONFIG_DVFS_CPU_POWER_STEP)
                {
                     //Set CPU Voltage
                    dwRegisterValue = dwSourceRegisterSetting;
                    byTargetRegAddress[0] = 0x10;
                    byTargetRegAddress[1] = (0x06 << 1);
                    byTargetRegAddress[2] = 0x10;
                    byTargetRegAddress[3] = dwRegisterValue;
                    if (MDrv_SW_IIC_Write(CONFIG_DVFS_POWER_SWI2C_BUS, CONFIG_DVFS_POWER_SWI2C_CPU_ID, 4, byTargetRegAddress, 0, byTargetData) != -1)
                    {
                        DVFS_HAL_DEBUG("\033[37m[INFO] Change to Voltage: %d0 mV (0x%X)\033[m\n", (unsigned int) dwCpuPowerVoltage, (unsigned int) dwRegisterValue);
                    }
                    else
                    {
                        DVFS_HAL_DEBUG("\033[37m[ERROR] Software I2C Write Failed\033[m\n");
                    }
                }
            }
            else if (dwCpuPowerVoltage < dwOriginalCpuPowerVoltage)
            {
                for (;dwSourceRegisterSetting >= dwTargetRegisterSetting; dwSourceRegisterSetting -= CONFIG_DVFS_CPU_POWER_STEP)
                {
                    //Set CPU Voltage
                    dwRegisterValue = dwSourceRegisterSetting;
                    byTargetRegAddress[0] = 0x10;
                    byTargetRegAddress[1] = (0x06 << 1);
                    byTargetRegAddress[2] = 0x10;
                    byTargetRegAddress[3] = dwRegisterValue;
                    if (MDrv_SW_IIC_Write(CONFIG_DVFS_POWER_SWI2C_BUS, CONFIG_DVFS_POWER_SWI2C_CPU_ID, 4, byTargetRegAddress, 0, byTargetData) != -1)
                    {
                        DVFS_HAL_DEBUG("\033[37m[INFO] Change to Voltage: %d0 mV (0x%X)\033[m\n", (unsigned int) dwCpuPowerVoltage, (unsigned int) dwRegisterValue);
                    }
                    else
                    {
                        DVFS_HAL_DEBUG("\033[37m[ERROR] Software I2C Write Failed\033[m\n");
                    }
                }
            }
            else
            {
                DVFS_HAL_DEBUG("\033[37m[INFO] No Need to Change CPU Power\033[m\n");
            }
        }

        if (dwSourceRegisterSetting != dwTargetRegisterSetting)
        {
            //Set CPU Voltage
            dwRegisterValue = (dwCpuPowerVoltage - CONFIG_DVFS_CPU_POWER_SHIFT_PRADO);
            byTargetRegAddress[0] = 0x10;
            byTargetRegAddress[1] = (0x06 << 1);
            byTargetRegAddress[2] = 0x10;
            byTargetRegAddress[3] = dwRegisterValue;
            if (MDrv_SW_IIC_Write(CONFIG_DVFS_POWER_SWI2C_BUS, CONFIG_DVFS_POWER_SWI2C_CPU_ID, 4, byTargetRegAddress, 0, byTargetData) != -1)
            {
                DVFS_HAL_DEBUG("\033[37m[INFO] Change to Voltage: %d0 mV (0x%X)\033[m\n", (unsigned int) dwCpuPowerVoltage, (unsigned int) dwRegisterValue);
            }
            else
            {
                DVFS_HAL_DEBUG("\033[37m[ERROR] Software I2C Write Failed\033[m\n");
            }
        }
    }
#endif
    return bDvfsCpuPowerAdjStatus;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: SysDvfsCorePowerAdjustment
/// @brief \b Function  \b Description: Update Output Voltage Level in External Power Chip
/// @param <IN>         \b None:
/// @param <OUT>        \b None:
/// @param <RET>        \b None:
/// @param <GLOBAL>     \b None:
////////////////////////////////////////////////////////////////////////////////
U32 SysDvfsCorePowerAdjustment(U32 dwCorePowerVoltage)
{
    U32 bDvfsCorePowerAdjStatus = TRUE;
#if CONFIG_DVFS_CORE_POWER_I2C_ENABLE
//Macan Not support CorePowerAdjustment
#endif
    return bDvfsCorePowerAdjStatus;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: SysDvfsCpuPowerInit
/// @brief \b Function  \b Description: The Init Flow of  External Power Chip
/// @param <IN>         \b None:
/// @param <OUT>        \b None:
/// @param <RET>        \b None:
/// @param <GLOBAL>     \b None:
////////////////////////////////////////////////////////////////////////////////
U32 SysDvfsCpuPowerInit(void)
{
    U32 bDvfsCpuPowerInitStatus = TRUE;
#if CONFIG_DVFS_CPU_POWER_I2C_ENABLE
    U8  byTargetRegAddress[5] =
        {
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF
        };
    U8  byTargetData[5] =
        {
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF
        };

    byTargetRegAddress[0] = 0x53;
    byTargetRegAddress[1] = 0x45;
    byTargetRegAddress[2] = 0x52;
    byTargetRegAddress[3] = 0x44;
    byTargetRegAddress[4] = 0x42;
    if(MDrv_SW_IIC_Write(CONFIG_DVFS_POWER_SWI2C_BUS, CONFIG_DVFS_POWER_SWI2C_CPU_ID, 5, byTargetRegAddress, 0, byTargetData) == -1)
    {
        DVFS_HAL_DEBUG("[ERROR] I2C_Enter_I2C Failed\n");
        bDvfsCpuPowerInitStatus = FALSE;
    }

    byTargetRegAddress[0] = 0x7F;
    if(MDrv_SW_IIC_Write(CONFIG_DVFS_POWER_SWI2C_BUS, CONFIG_DVFS_POWER_SWI2C_CPU_ID, 1, byTargetRegAddress, 0, byTargetData) == -1)
    {
        DVFS_HAL_DEBUG("[ERROR] I2C_USE_CFG Failed\n");
        bDvfsCpuPowerInitStatus = FALSE;
    }

    byTargetRegAddress[0] = 0x7D;
    if(MDrv_SW_IIC_Write(CONFIG_DVFS_POWER_SWI2C_BUS, CONFIG_DVFS_POWER_SWI2C_CPU_ID, 1, byTargetRegAddress, 0, byTargetData) == -1)
    {
        DVFS_HAL_DEBUG("[ERROR] I2C_OUT_NO_DELAY Failed\n");
        bDvfsCpuPowerInitStatus = FALSE;
    }

    byTargetRegAddress[0] = 0x50;
    if(MDrv_SW_IIC_Write(CONFIG_DVFS_POWER_SWI2C_BUS, CONFIG_DVFS_POWER_SWI2C_CPU_ID, 1, byTargetRegAddress, 0, byTargetData) == -1)
    {
        DVFS_HAL_DEBUG("[ERROR] I2C_AD_BYTE_EN0 Failed\n");
        bDvfsCpuPowerInitStatus = FALSE;
    }

    byTargetRegAddress[0] = 0x55;
    if(MDrv_SW_IIC_Write(CONFIG_DVFS_POWER_SWI2C_BUS, CONFIG_DVFS_POWER_SWI2C_CPU_ID, 1, byTargetRegAddress, 0, byTargetData) == -1)
    {
        DVFS_HAL_DEBUG("[ERROR] I2C_DA_BYTE_EN1 Failed\n");
        bDvfsCpuPowerInitStatus = FALSE;
    }

    byTargetRegAddress[0] = 0x35;
    if(MDrv_SW_IIC_Write(CONFIG_DVFS_POWER_SWI2C_BUS, CONFIG_DVFS_POWER_SWI2C_CPU_ID, 1, byTargetRegAddress, 0, byTargetData) == -1)
    {
        DVFS_HAL_DEBUG("[ERROR] I2C_USE Failed\n");
        bDvfsCpuPowerInitStatus = FALSE;
    }

    byTargetRegAddress[0] = 0x10;
    byTargetRegAddress[1] = 0xc0;
    if(MDrv_SW_IIC_Read(CONFIG_DVFS_POWER_SWI2C_BUS, CONFIG_DVFS_POWER_SWI2C_CPU_ID, 2, byTargetRegAddress, 2, byTargetData) != -1)
    {
        DVFS_HAL_DEBUG("[INFO] MStar Power IC Chip ID: %x%x\n", (unsigned int) byTargetData[0], (unsigned int) byTargetData[1]);
        dwPowerChipId = (unsigned int) byTargetData[1];
    }
    else
    {
        bDvfsCpuPowerInitStatus = FALSE;
    }

    SysDvfsCpuPowerAdjustment(CONFIG_DVFS_CPU_POWER_DEFAULT);

    if(dwPowerChipId == CONFIG_DVFS_CHIP_ID_PRADO)
    {
        //Set OTP Level
        byTargetRegAddress[0] = 0x10;
        byTargetRegAddress[1] = (0x05 << 1);
        byTargetRegAddress[2] = 0x40;
        byTargetRegAddress[3] = 0x00;
        if(MDrv_SW_IIC_Write(CONFIG_DVFS_POWER_SWI2C_BUS, CONFIG_DVFS_POWER_SWI2C_CPU_ID, 4, byTargetRegAddress, 0, byTargetData) == -1)
        {
            DVFS_HAL_DEBUG("[ERROR] Software I2C Write Failed\n");
            bDvfsCpuPowerInitStatus = FALSE;
        }

        //Set Password
        byTargetRegAddress[0] = 0x10;
        byTargetRegAddress[1] = (0x0C << 1);
        byTargetRegAddress[2] = 0xbe;
        byTargetRegAddress[3] = 0xaf;
        if(MDrv_SW_IIC_Write(CONFIG_DVFS_POWER_SWI2C_BUS, CONFIG_DVFS_POWER_SWI2C_CPU_ID, 4, byTargetRegAddress, 0, byTargetData) == -1)
        {
            DVFS_HAL_DEBUG("[ERROR] Software I2C Write Failed\n");
            bDvfsCpuPowerInitStatus = FALSE;
        }
    }

    //SysDvfsCpuPowerAdjustment(CONFIG_DVFS_CPU_POWER_DEFAULT);
    *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x100510 << 1)) = CONFIG_DVFS_DYNAMIC_POWER_ADJUST_INIT;
#endif
#if CONFIG_DVFS_CPU_POWER_GPIO_ENABLE
#error "No Support CPU Power Adjustment by GPIO in Macan Platform"
#endif

    return bDvfsCpuPowerInitStatus;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: SysDvfsCorePowerInit
/// @brief \b Function  \b Description: The Init Flow of  External Power Chip
/// @param <IN>         \b None:
/// @param <OUT>        \b None:
/// @param <RET>        \b None:
/// @param <GLOBAL>     \b None:
////////////////////////////////////////////////////////////////////////////////
U32 SysDvfsCorePowerInit(void)
{
    U32 bDvfsCorePowerInitStatus = TRUE;
#if CONFIG_DVFS_CORE_POWER_I2C_ENABLE

#endif
#if CONFIG_DVFS_CORE_POWER_GPIO_ENABLE
#error "No Support CPU Power Adjustment by GPIO in Macan Platform"
#endif
    return bDvfsCorePowerInitStatus;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name:
/// @brief \b Function  \b Description:
/// @param <IN>         \b None:
/// @param <OUT>        \b None:
/// @param <RET>        \b None:
/// @param <GLOBAL>     \b None:
////////////////////////////////////////////////////////////////////////////////
void SysDvfsPradoProtect(void)
{
    U8  byTargetRegAddress[5] =
        {
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF
        };
    U8  byTargetData[5] =
        {
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF
        };
    if(dwPowerChipId == CONFIG_DVFS_CHIP_ID_PRADO)
    {
        //Clear OTP flag
        byTargetRegAddress[0] = 0x10;
        byTargetRegAddress[1] = (0x07 << 1);
        byTargetRegAddress[2] = 0x00;
        byTargetRegAddress[3] = 0x01;
        if(MDrv_SW_IIC_Write(CONFIG_DVFS_POWER_SWI2C_BUS, CONFIG_DVFS_POWER_SWI2C_CPU_ID, 4, byTargetRegAddress, 0, byTargetData) != TRUE)
        {
            DVFS_HAL_DEBUG("\033[37m [ERROR] Software I2C Write Failed\033[m\n");
        }

        //Set Password
        byTargetRegAddress[0] = 0x10;
        byTargetRegAddress[1] = 0x69;
        if(MDrv_SW_IIC_Read(CONFIG_DVFS_POWER_SWI2C_BUS, CONFIG_DVFS_POWER_SWI2C_CPU_ID, 2, byTargetRegAddress, 2, byTargetData) == TRUE)
        {
            DVFS_HAL_DEBUG("\033[33m [DVFS] MStar Power IC Chip ID: %x%x\033[m\n", (unsigned int) byTargetData[0], (unsigned int) byTargetData[1]);
            if ((byTargetData[0] & 0x02) == 0x02)
            {
                *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x100514 << 1)) = 1;
            }
            else
            {
                *(volatile U16 *)(CONFIG_REGISTER_BASE_ADDRESS + (0x100514 << 1)) = 0;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: SysDvfsCpuPowerInit
/// @brief \b Function  \b Description: The Init Flow of  External Power Chip
/// @param <IN>         \b None:
/// @param <OUT>        \b None:
/// @param <RET>        \b None:
/// @param <GLOBAL>     \b None:
////////////////////////////////////////////////////////////////////////////////
void SysDvfsPradoOTPFlagInit(void)
{
    U8  byTargetRegAddress[5] =
    {
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF
    };
    U8  byTargetData[5] =
    {
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF
    };

    if(dwPowerChipId == CONFIG_DVFS_CHIP_ID_PRADO)
    {
        //Disable OTP flag
        byTargetRegAddress[0] = 0x10;
        byTargetRegAddress[1] = (0x08 << 1);
        byTargetRegAddress[2] = 0x0f;
        byTargetRegAddress[3] = 0x40;
        if(MDrv_SW_IIC_Write(CONFIG_DVFS_POWER_SWI2C_BUS, CONFIG_DVFS_POWER_SWI2C_CPU_ID, 4, byTargetRegAddress, 0, byTargetData) != TRUE)
        {
            DVFS_HAL_DEBUG("\033[37m [ERROR] Software I2C Write Failed \033[m\n");
        }

        //Set Password
        byTargetRegAddress[0] = 0x10;
        byTargetRegAddress[1] = (0x09 << 1);
        byTargetRegAddress[2] = 0x5a;
        byTargetRegAddress[3] = 0xa5;
        if(MDrv_SW_IIC_Write(CONFIG_DVFS_POWER_SWI2C_BUS, CONFIG_DVFS_POWER_SWI2C_CPU_ID, 4, byTargetRegAddress, 0, byTargetData) != TRUE)
        {
            DVFS_HAL_DEBUG("\033[37m [ERROR] Software I2C Write Failed \033[m\n");
        }
    }
}
