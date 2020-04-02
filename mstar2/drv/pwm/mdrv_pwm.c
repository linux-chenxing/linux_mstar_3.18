//<MStar Software>
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2010 - 2012 MStar Semiconductor, Inc.
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

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    drvPWM.c
/// @brief  Pulse Width Modulation Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Include Files
////////////////////////////////////////////////////////////////////////////////
//#include "MsCommon.h"
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/string.h>
#include <linux/version.h>
#include <asm/io.h>
#include <asm/uaccess.h>
//#include "MsVersion.h"
#include "mdrv_pwm.h"
#include "mhal_pwm.h"
//#include "MsOS.h"

////////////////////////////////////////////////////////////////////////////////
// Local defines & local structures
////////////////////////////////////////////////////////////////////////////////

#define PWM_DBG_FUNC()            if (_geDbfLevel >= E_PWM_DBGLV_ALL)                   \
                                       {MS_DEBUG_MSG(printk("\t====   %s   ====\n", __FUNCTION__));}

#define PWM_DBG_INFO(x, args...)  if (_geDbfLevel >= E_PWM_DBGLV_INFO )                 \
                                       {MS_DEBUG_MSG(printk(x, ##args));}

#define PWM_DBG_ERR(x, args...)   if (_geDbfLevel >= E_PWM_DBGLV_ERR_ONLY)				\
                                       {MS_DEBUG_MSG(printk(x, ##args));}

////////////////////////////////////////////////////////////////////////////////
// Local & Global Variables
////////////////////////////////////////////////////////////////////////////////
static BOOL  _gbInitPWM = FALSE;
//static PWM_Arg _gsPWMDesc;
static PWM_DbgLv _geDbfLevel = E_PWM_DBGLV_NONE;

static PWM_DrvStatus   _pwmStatus =
                        {
                            .PWM0 = FALSE,
                            .PWM1 = FALSE,
                            .PWM2 = FALSE,
                            .PWM3 = FALSE,
                            .PWM4 = FALSE,
                            .PWM5 = FALSE,
                            .PWM6 = FALSE,
                            .PWM7 = FALSE,
                            .PWM8 = FALSE,
                        };

////////////////////////////////////////////////////////////////////////////////
// Local Function
////////////////////////////////////////////////////////////////////////////////
static BOOL _PWM_IsInit(void);

//-------------------------------------------------------------------------------------------------
/// Description: Set debug level for debug message
/// @param eLevel    \b debug level for PWM driver
/// @return E_PWM_OK : succeed
/// @return E_PWM_FAIL : fail before timeout or illegal parameters
//-------------------------------------------------------------------------------------------------
PWM_Result MDrv_PWM_SetDbgLevel(PWM_DbgLv eLevel)
{
    PWM_DBG_INFO("%s(%u)\r\n", __FUNCTION__, eLevel);

    _geDbfLevel = eLevel;

    return E_PWM_OK;
}

//------------------------------------------------------------------------------
/// Description : Show the PWM_PAD is PWM(True) or GPIO(false)
/// @param  pStatus \b OUT: output PWM driver status
/// @return E_PWM_OK : succeed
/// @return E_PWM_FAIL : fail before timeout or illegal parameters
//------------------------------------------------------------------------------
PWM_Result MDrv_PWM_GetStatus(PWM_DrvStatus *pStatus)
{
    memcpy(pStatus, &_pwmStatus, sizeof(PWM_DrvStatus));
    return E_PWM_OK;
}

//-------------------------------------------------------------------------------------------------
/// Description : Check PWM is initial or not
/// @return TRUE : PWM was initied
/// @return FALSE : PWM was not initied
//-------------------------------------------------------------------------------------------------
static BOOL _PWM_IsInit(void)
{
    PWM_DBG_FUNC();
    if (!_gbInitPWM)
        PWM_DBG_ERR("Call MDrv_PWM_Init first!\n");
    return _gbInitPWM;
}

//-------------------------------------------------------------------------------------------------
/// Description: Initial PWM driver
/// @param eLevel    \b debug level for PWM driver
/// @return E_PWM_OK : succeed
/// @return E_PWM_FAIL : fail before timeout or illegal parameters
//-------------------------------------------------------------------------------------------------
PWM_Result MDrv_PWM_Init(PWM_DbgLv eLevel)
{
    MDrv_PWM_SetDbgLevel(eLevel);

    if(!_gbInitPWM)
    {
        PWM_DBG_INFO("%s\n", __FUNCTION__);
        /* Set All pad output and Is PWM. But it still needs to be enable */
        if( !HAL_PWM_Init() )
        {
            PWM_DBG_ERR("PWM Hal Initial Fail\n");
            return E_PWM_FAIL;
        }

        _gbInitPWM = TRUE;
        return E_PWM_OK;
    }
    else
    {
        PWM_DBG_ERR("PWM has be initiated!\n");
        return E_PWM_OK;
    }
}
EXPORT_SYMBOL(MDrv_PWM_Init);

//-------------------------------------------------------------------------------------------------
/// Description: IOutput enable_bar of PWM pads
/// @param u8IndexPWM    \b which pwm is setting
/// @param bOenPWM    \b True/False for enable/disable
/// @return E_PWM_OK : succeed
/// @return E_PWM_FAIL : fail before timeout or illegal parameters
//-------------------------------------------------------------------------------------------------
PWM_Result MDrv_PWM_Oen(PWM_ChNum u8IndexPWM, BOOL bOenPWM)
{
    PWM_Result ret = E_PWM_OK;
    PWM_DBG_INFO("%s(%u, %u)\r\n", __FUNCTION__, u8IndexPWM, bOenPWM);

    switch(u8IndexPWM)
    {
        case E_PWM_CH0:
            _pwmStatus.PWM0 = TRUE;
            break;
        case E_PWM_CH1:
            _pwmStatus.PWM1 = TRUE;
            break;
        case E_PWM_CH2:
            _pwmStatus.PWM2 = TRUE;
            break;
        case E_PWM_CH3:
            _pwmStatus.PWM3 = TRUE;
            break;
        case E_PWM_CH4:
            _pwmStatus.PWM4 = TRUE;
            break;
        case E_PWM_CH5:
            _pwmStatus.PWM5 = TRUE;
            break;
        case E_PWM_CH6:
            _pwmStatus.PWM6 = TRUE;
            break;
        case E_PWM_CH7:
            _pwmStatus.PWM7 = TRUE;
            break;
        case E_PWM_CH8:
            _pwmStatus.PWM8 = TRUE;
            break;
        default:
            PWM_ASSERT(0);
    }

    if( !(HAL_PWM_Oen(u8IndexPWM, bOenPWM)||_PWM_IsInit()) )
    {
        ret = E_PWM_FAIL;
    }
    return ret;
}
EXPORT_SYMBOL(MDrv_PWM_Oen);

//-------------------------------------------------------------------------------------------------
/// Description: Set the period of the specific pwm
/// @param u8IndexPWM    \b which pwm is setting
/// @param u16PeriodPWM    \b the 18-bit period value
/// @return E_PWM_OK : succeed
/// @return E_PWM_FAIL : fail before timeout or illegal parameters
//-------------------------------------------------------------------------------------------------
PWM_Result MDrv_PWM_Period(PWM_ChNum u8IndexPWM, U32 u32PeriodPWM)
{
    PWM_Result ret = E_PWM_FAIL;
    PWM_DBG_INFO("%s(%u, 0x%08X)\r\n", __FUNCTION__, u8IndexPWM, (int)u32PeriodPWM);

    do{
        HAL_PWM_Period(u8IndexPWM, u32PeriodPWM);
        ret = E_PWM_OK;
    }while(0);

    return ret;
}
EXPORT_SYMBOL(MDrv_PWM_Period);
//-------------------------------------------------------------------------------------------------
/// Description: Set the Duty of the specific pwm
/// @param u8IndexPWM    \b which pwm is setting
/// @param u16DutyPWM    \b the 18-bit Duty value
/// @return E_PWM_OK : succeed
/// @return E_PWM_FAIL : fail before timeout or illegal parameters
//-------------------------------------------------------------------------------------------------
PWM_Result MDrv_PWM_DutyCycle(PWM_ChNum u8IndexPWM, U32 u32DutyPWM)
{
    PWM_Result ret = E_PWM_FAIL;
    PWM_DBG_INFO("%s(%u, 0x%08X)\r\n", __FUNCTION__, u8IndexPWM, (int)u32DutyPWM);

    do{
        HAL_PWM_DutyCycle(u8IndexPWM, u32DutyPWM);
        ret = E_PWM_OK;
    }while(0);

    return ret;
}
EXPORT_SYMBOL(MDrv_PWM_DutyCycle);
//-------------------------------------------------------------------------------------------------
/// Description: Set the Unit_Div of the pwm
/// @param u16UnitDivPWM    \b the Unit Div value
/// @return E_PWM_OK : succeed
/// @return E_PWM_FAIL : fail before timeout or illegal parameters
//-------------------------------------------------------------------------------------------------
PWM_Result MDrv_PWM_UnitDiv(U16 u16UnitDivPWM)
{
    PWM_Result ret = E_PWM_OK;
    PWM_DBG_INFO("%s(0x%04X)\r\n", __FUNCTION__, u16UnitDivPWM);

    if(!HAL_PWM_UnitDiv(u16UnitDivPWM))
    {
        ret = E_PWM_FAIL;
    }
    return ret;
}

//-------------------------------------------------------------------------------------------------
/// Description: Set the Div of the specific pwm
/// @param u8IndexPWM    \b which pwm is setting
/// @param u16DivPWM    \b the 16-bit div value
//-------------------------------------------------------------------------------------------------
void MDrv_PWM_Div(PWM_ChNum u8IndexPWM, U16 u16DivPWM)
{
    PWM_DBG_INFO("%s(%u, 0x%04X)\r\n", __FUNCTION__, u8IndexPWM, u16DivPWM);
    HAL_PWM_Div(u8IndexPWM, u16DivPWM);
}
EXPORT_SYMBOL(MDrv_PWM_Div);

//-------------------------------------------------------------------------------------------------
/// Description:  Set the Polarity of the specific pwm
/// @param u8IndexPWM    \b which pwm is setting
/// @param bPolPWM    \b True/False for Inverse/Non-inverse
//-------------------------------------------------------------------------------------------------
void MDrv_PWM_Polarity(PWM_ChNum u8IndexPWM, BOOL bPolPWM)
{
    PWM_DBG_INFO("%s(%u, %u)\r\n", __FUNCTION__, u8IndexPWM, bPolPWM);
    HAL_PWM_Polarity(u8IndexPWM, bPolPWM);
}
EXPORT_SYMBOL(MDrv_PWM_Polarity);
//-------------------------------------------------------------------------------------------------
/// Description: Set the Vsync Double buffer of the specific pwm
/// @param u8IndexPWM    \b which pwm is setting
/// @param bVdbenPWM    \b True/False for Enable/Disable
//-------------------------------------------------------------------------------------------------
void MDrv_PWM_Vdben(PWM_ChNum u8IndexPWM, BOOL bVdbenPWM)
{
    PWM_DBG_INFO("%s(%u, %u)\r\n", __FUNCTION__, u8IndexPWM, bVdbenPWM);
    HAL_PWM_VDBen(u8IndexPWM, bVdbenPWM);
}
EXPORT_SYMBOL(MDrv_PWM_Vdben);
//-------------------------------------------------------------------------------------------------
/// Description: Set the Hsync reset of the specific pwm
/// @param u8IndexPWM    \b which pwm is setting
/// @param bRstPWM    \b True/False for Enable/Disable
//-------------------------------------------------------------------------------------------------
void MDrv_PWM_ResetEn(PWM_ChNum u8IndexPWM, BOOL bRstPWM)
{
    PWM_DBG_INFO("%s(%u, %u)\r\n", __FUNCTION__, u8IndexPWM, bRstPWM);
    HAL_PWM_Vrest(u8IndexPWM, bRstPWM);
}
EXPORT_SYMBOL(MDrv_PWM_ResetEn);

//-------------------------------------------------------------------------------------------------
/// Description:  Set the Double buffer of the specific pwm
/// @param u8IndexPWM    \b which pwm is setting
/// @param bDbenPWM    \b True/False for Enable/Disable
//-------------------------------------------------------------------------------------------------
void MDrv_PWM_Dben(PWM_ChNum u8IndexPWM, BOOL bDbenPWM)
{
    PWM_DBG_INFO("%s(%u, %u)\r\n", __FUNCTION__, u8IndexPWM, bDbenPWM);
    HAL_PWM_DBen(u8IndexPWM, bDbenPWM);
}
EXPORT_SYMBOL(MDrv_PWM_Dben);

void MDrv_PWM_IMPULSE_EN(PWM_ChNum u8IndexPWM, BOOL bdbenPWM)
{
    PWM_DBG_INFO("%s(%u, %u)\r\n", __FUNCTION__, u8IndexPWM, bdbenPWM);
    HAL_PWM_IMPULSE_EN(u8IndexPWM, bdbenPWM);
}

void MDrv_PWM_ODDEVEN_SYNC(PWM_ChNum u8IndexPWM, BOOL bdbenPWM)
{
    PWM_DBG_INFO("%s(%u, %u)\r\n", __FUNCTION__, u8IndexPWM, bdbenPWM);
    HAL_PWM_ODDEVEN_SYNC(u8IndexPWM, bdbenPWM);
}

//-------------------------------------------------------------------------------------------------
/// Description:  Set the Rst Mux of the specific pwm
/// @param u8IndexPWM    \b which pwm is setting
/// @param bMuxPWM    \b True/False for Enable/Disable
//-------------------------------------------------------------------------------------------------
void MDrv_PWM_RstMux(PWM_ChNum u8IndexPWM, BOOL bMuxPWM)
{
    PWM_DBG_INFO("%s(%u, %u)\r\n", __FUNCTION__, u8IndexPWM, bMuxPWM);
    HAL_PWM_RstMux(u8IndexPWM, bMuxPWM);
}

//-------------------------------------------------------------------------------------------------
/// Description: Set the Rst_Cnt of the specific pwm
/// @param u8IndexPWM    \b which pwm is setting
/// @param u8RstCntPWM    \b the Rst_Cnt value
//-------------------------------------------------------------------------------------------------
void MDrv_PWM_RstCnt(PWM_ChNum u8IndexPWM, U8 u8RstCntPWM)
{
    PWM_DBG_INFO("%s(%u, 0x%02X)\r\n", __FUNCTION__, u8IndexPWM, u8RstCntPWM);
    HAL_PWM_RstCnt(u8IndexPWM, u8RstCntPWM);
}

//-------------------------------------------------------------------------------------------------
/// Description: Set the Bypass Unit of the specific pwm
/// @param u8IndexPWM    \b which pwm is setting
/// @param bBypassPWM    \b True/False for Enable/Disable
//-------------------------------------------------------------------------------------------------
void MDrv_PWM_BypassUnit(PWM_ChNum u8IndexPWM, BOOL bBypassPWM)
{
    PWM_DBG_INFO("%s(%u, %u)\r\n", __FUNCTION__, u8IndexPWM, bBypassPWM);
    HAL_PWM_BypassUnit(u8IndexPWM, bBypassPWM);
}

//-------------------------------------------------------------------------------------------------
/// Description: Counter mode for PWM0 and PWM1
/// @param u8CntModePWM    \b Cnt Mode
/// @return E_PWM_OK : succeed
/// @return E_PWM_FAIL : fail before timeout or illegal parameters
/// @note    \n
///     11: PWM1 donate internal divider to PWM0    \n
///     10: PWM0 donate internal divider to PWM1    \n
///     0x: Normal mode    \n
//-------------------------------------------------------------------------------------------------
PWM_Result MDrv_PWM01_CntMode(PWM_CntMode u8CntModePWM)
{
    PWM_Result ret = E_PWM_FAIL;
    PWM_DBG_INFO("%s(%u)\r\n", __FUNCTION__, u8CntModePWM);
    if( HAL_PWM01_CntMode(u8CntModePWM) )
    {
		ret = E_PWM_OK;
    }
    return ret;
}

//-------------------------------------------------------------------------------------------------
/// Description: Counter mode for PWM2 and PWM3
/// @param u8CntModePWM    \b Cnt Mode
/// @return E_PWM_OK : succeed
/// @return E_PWM_FAIL : fail before timeout or illegal parameters
/// @note    \n
///     11: PWM3 donate internal divider to PWM2    \n
///     10: PWM2 donate internal divider to PWM3    \n
///     0x: Normal mode    \n
//-------------------------------------------------------------------------------------------------
PWM_Result MDrv_PWM23_CntMode(PWM_CntMode u8CntModePWM)
{
    PWM_Result ret = E_PWM_FAIL;
    PWM_DBG_INFO("%s(%u)\r\n", __FUNCTION__, u8CntModePWM);
    if( HAL_PWM23_CntMode(u8CntModePWM) )
    {
		ret = E_PWM_OK;
    }
    return ret;
}

//-------------------------------------------------------------------------------------------------
/// Description: Counter mode for PWM6 and PWM7
/// @param u8CntModePWM    \b Cnt Mode
/// @return E_PWM_OK : succeed
/// @return E_PWM_FAIL : fail before timeout or illegal parameters
/// @note    \n
///     11: PWM7 donate internal divider to PWM6    \n
///     10: PWM6 donate internal divider to PWM7    \n
///     0x: Normal mode    \n
//-------------------------------------------------------------------------------------------------
PWM_Result MDrv_PWM67_CntMode(PWM_CntMode u8CntModePWM)
{
    PWM_Result ret = E_PWM_FAIL;
    PWM_DBG_INFO("%s(%u)\r\n", __FUNCTION__, u8CntModePWM);
    if( HAL_PWM67_CntMode(u8CntModePWM) )
    {
		ret = E_PWM_OK;
    }
    return ret;
}

//-------------------------------------------------------------------------------------------------
/// Description: Set the Shift of the specific pwm
/// @param u8IndexPWM    \b which pwm is setting
/// @param u16DutyPWM    \b the 18-bit Shift value
/// @return E_PWM_OK : succeed
/// @return E_PWM_FAIL : fail before timeout or illegal parameters
//-------------------------------------------------------------------------------------------------
PWM_Result MDrv_PWM_Shift(PWM_ChNum u8IndexPWM, U32 u32DutyPWM)
{
    PWM_Result ret = E_PWM_FAIL;
    PWM_DBG_INFO("%s(%u, 0x%08X)\r\n", __FUNCTION__, u8IndexPWM, (int) u32DutyPWM);
    if ( HAL_PWM_Shift(u8IndexPWM, u32DutyPWM) )
    {
        ret = E_PWM_OK;
    }
    return ret;
}


void MDrv_PWM_Nvsync(PWM_ChNum u8IndexPWM, BOOL bNvsPWM)
{
    PWM_DBG_INFO("%s(%u, %u)\r\n", __FUNCTION__, u8IndexPWM, bNvsPWM);
    HAL_PWM_Nvsync(u8IndexPWM, bNvsPWM);
}


void MDrv_PWM_Align(PWM_ChNum u8IndexPWM, BOOL bAliPWM)
{
    PWM_DBG_INFO("%s(%u, %u)\r\n", __FUNCTION__, u8IndexPWM, bAliPWM);
    HAL_PWM_Align(u8IndexPWM, bAliPWM);
}

void MDrv_PWM_ForceShitfEn(PWM_ChNum u8IndexPWM, BOOL bForcePWM)
{
    PWM_DBG_INFO("%s(%u, %u)\r\n", __FUNCTION__, u8IndexPWM, bForcePWM);
    HAL_PWM_ForceShitfEn(u8IndexPWM, bForcePWM);
}
EXPORT_SYMBOL(MDrv_PWM_ForceShitfEn);


//-------------PM BASE---------------

void MDrv_PM_PWM_Enable(void)
{
    HAL_PM_PWM_Enable();
}

void MDrv_PM_PWM_Period(U16 u16PeriodPWM)
{
   HAL_PM_PWM_Period(u16PeriodPWM);
}

void MDrv_PM_PWM_DutyCycle(U16 u16DutyPWM)
{
   HAL_PM_PWM_DutyCycle(u16DutyPWM);
}

void MDrv_PM_PWM_Div(U8 u8DivPWM)
{
    HAL_PM_PWM_Div(u8DivPWM);
}

void MDrv_PM_PWM_Polarity(BOOL bPolPWM)
{
    HAL_PM_PWM_Polarity(bPolPWM);
}

void MDrv_PM_PWM_Dben(BOOL bDbenPWM)
{
    HAL_PM_PWM_DBen(bDbenPWM);
}

