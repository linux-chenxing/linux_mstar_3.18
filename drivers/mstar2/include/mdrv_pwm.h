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
/// @file   drvPWM.h
/// @brief  Pulse Width Modulation driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _DRV_TEMP_H_
#define _DRV_TEMP_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "mdrv_types.h"

////////////////////////////////////////////////////////////////////////////////
// MACRO
////////////////////////////////////////////////////////////////////////////////
#define PWM_DEBUG   (1)
#if (PWM_DEBUG)
#define PWM_PRINT(_fmt, _args...)    printk(KERN_WARNING "[%s][%d] " _fmt, __FUNCTION__, __LINE__, ## _args)
#define PWM_ASSERT(_con) \
    do { \
        if (!(_con)) { \
            printk(KERN_CRIT "BUG at %s:%d assert(%s)\n", \
                    __FILE__, __LINE__, #_con); \
            BUG(); \
        } \
    } while (0)
#define MS_DEBUG_MSG(x)       x
#else
#define PWM_PRINT(fmt, args...)
#define PWM_ASSERT(arg)
#define MS_DEBUG_MSG(x)
#endif

#if defined(CONFIG_MSTAR_MSYSTEM) || defined(CONFIG_MSTAR_MSYSTEM_MODULE)
  #define MSYSTEM_STATIC
#else//#if defined(CONFIG_MSTAR_MSYSTEM) || defined(CONFIG_MSTAR_MSYSTEM_MODULE)
  #define MSYSTEM_STATIC static
#endif//#if defined(CONFIG_MSTAR_MSYSTEM) || defined(CONFIG_MSTAR_MSYSTEM_MODULE)

////////////////////////////////////////////////////////////////////////////////
// Define & data type
////////////////////////////////////////////////////////////////////////////////
#define MSIF_PWM_LIB_CODE                     {'P','W','M','_'}    //Lib code
#define MSIF_PWM_LIBVER                       {'0','1'}            //LIB version
#define MSIF_PWM_BUILDNUM                     {'0','6'}            //Build Number
#define MSIF_PWM_CHANGELIST                   {'0','0','3','4','3','8','2','1'} //P4 ChangeList Number
#define MSIF_PWM_OS                           '0'                  //OS
#define PWM_IOC_MAGIC                           'p'
#define PWM_IOC_MAXNR                            3
#define MSIF_TAG                    {'M','S','I','F'}                   // MSIF
#define MSIF_CLASS                  {'0','0'}                           // DRV/API (DDI)
#define MSIF_CUS                    0x0000                              // MStar Common library
#define MSIF_MOD                    0x0000                              // MStar Common library
#define MSIF_CHIP                   0x000B
#define MSIF_CPU                    '1'
#define MSIF_OS                     '0'

#define          PWM_DRV_VERSION                  /* Character String for DRV/API version             */  \
            MSIF_TAG,                         /* 'MSIF'                                           */  \
            MSIF_CLASS,                       /* '00'                                             */  \
            MSIF_CUS,                         /* 0x0000                                           */  \
            MSIF_MOD,                         /* 0x0000                                           */  \
            MSIF_CHIP,                                                                              \
            MSIF_CPU,                                                                               \
            MSIF_PWM_LIB_CODE,                    /* IP__                                             */  \
            MSIF_PWM_LIBVER,                      /* 0.0 ~ Z.Z                                        */  \
            MSIF_PWM_BUILDNUM,                    /* 00 ~ 99                                          */  \
            MSIF_PWM_CHANGELIST,                  /* CL#                                              */  \
            MSIF_OS


typedef enum _PWM_DbgLv
{
    E_PWM_DBGLV_NONE,          // no debug message
    E_PWM_DBGLV_ERR_ONLY,      // show error only
    E_PWM_DBGLV_INFO,          // show error & informaiton
    E_PWM_DBGLV_ALL            // show error, information & funciton name
}PWM_DbgLv;

typedef enum _PWM_ChNum
{
    E_PWM_CH0,
    E_PWM_CH1,
    E_PWM_CH2,
    E_PWM_CH3,
    E_PWM_CH4,
    E_PWM_CH5,
    E_PWM_CH6,
    E_PWM_CH7,
    E_PWM_CH8,
    E_PWM_CH9
}PWM_ChNum;

typedef enum _PWM_CntMode
{
    E_PWM_Normal = 0x00,
    E_PWM_01Mode = 0x10,
    E_PWM_10Mode = 0x11
}PWM_CntMode;

typedef enum _PWM_Result
{
    E_PWM_OK,
    E_PWM_FAIL
} PWM_Result;

typedef struct _PWM_Info
{
    U8   u8ChNum;           // number of this channel
    U32  u32IOMap;          // base of this channel's registers
    U32  mck;               // base clock rate from mcu
}PWM_Info;

/* This structure is full the same with hal for now. */
typedef struct _PWM_Arg
{
    U16  index;
    U8   div;
    U16  period;
    U16  duty;
    BOOL polarity;
    BOOL vdben;
    BOOL rsten;
    BOOL dben;
} PWM_Arg;

typedef struct _PWM_DrvStatus
{
    BOOL PWM0;
    BOOL PWM1;
    BOOL PWM2;
    BOOL PWM3;
    BOOL PWM4;
    BOOL PWM5;
    BOOL PWM6;
    BOOL PWM7;
    BOOL PWM8;
} PWM_DrvStatus;

#define NON_INVERSE     0
#define INVERSE         1

////////////////////////////////////////////////////////////////////////////////
// Extern Function
////////////////////////////////////////////////////////////////////////////////
void MDrv_PM_PWM_Enable(void);
void MDrv_PM_PWM_Period(U16 u16PeriodPWM);
void MDrv_PM_PWM_DutyCycle(U16 u16DutyPWM);
void MDrv_PM_PWM_Div(U8 u8DivPWM);
void MDrv_PM_PWM_Polarity(BOOL bPolPWM);
void MDrv_PM_PWM_Dben(BOOL bDbenPWM);

//------------------------------------------------------------------------------
/// Description : Show the PWM_PAD is PWM(True) or GPIO(false)
/// @param  pStatus \b OUT: output PWM driver status
/// @return E_PWM_OK : succeed
/// @return E_PWM_FAIL : fail before timeout or illegal parameters
//------------------------------------------------------------------------------
PWM_Result MDrv_PWM_GetStatus(PWM_DrvStatus *pStatus);

//-------------------------------------------------------------------------------------------------
/// Description: Initial PWM driver
/// @param eLevel    \b debug level for PWM driver
/// @return E_PWM_OK : succeed
/// @return E_PWM_FAIL : fail before timeout or illegal parameters
//-------------------------------------------------------------------------------------------------
PWM_Result MDrv_PWM_Init(PWM_DbgLv eLevel);
//-------------------------------------------------------------------------------------------------
/// Description: IOutput enable_bar of PWM pads
/// @param u8IndexPWM    \b which pwm is setting
/// @param bOenPWM    \b True/False for enable/disable
/// @return E_PWM_OK : succeed
/// @return E_PWM_FAIL : fail before timeout or illegal parameters
//-------------------------------------------------------------------------------------------------
PWM_Result MDrv_PWM_Oen(PWM_ChNum u8IndexPWM, BOOL bOenPWM);
//-------------------------------------------------------------------------------------------------
/// Description: Set the period of the specific pwm
/// @param u8IndexPWM    \b which pwm is setting
/// @param u16PeriodPWM    \b the 18-bit period value
/// @return E_PWM_OK : succeed
/// @return E_PWM_FAIL : fail before timeout or illegal parameters
//-------------------------------------------------------------------------------------------------
PWM_Result MDrv_PWM_Period(PWM_ChNum u8IndexPWM, U32 u32PeriodPWM);
//-------------------------------------------------------------------------------------------------
/// Description: Set the Duty of the specific pwm
/// @param u8IndexPWM    \b which pwm is setting
/// @param u16DutyPWM    \b the 18-bit Duty value
/// @return E_PWM_OK : succeed
/// @return E_PWM_FAIL : fail before timeout or illegal parameters
//-------------------------------------------------------------------------------------------------
PWM_Result MDrv_PWM_DutyCycle(PWM_ChNum u8IndexPWM, U32 u32DutyPWM);
//-------------------------------------------------------------------------------------------------
/// Description: Set the Unit_Div of the pwm
/// @param u16UnitDivPWM    \b the Unit Div value
/// @return E_PWM_OK : succeed
/// @return E_PWM_FAIL : fail before timeout or illegal parameters
//-------------------------------------------------------------------------------------------------
PWM_Result MDrv_PWM_UnitDiv(U16 u16UnitDivPWM);
//-------------------------------------------------------------------------------------------------
/// Description: Set the Div of the specific pwm
/// @param u8IndexPWM    \b which pwm is setting
/// @param u16DivPWM    \b the 16-bit div value
//-------------------------------------------------------------------------------------------------
void MDrv_PWM_Div(PWM_ChNum u8IndexPWM, U16 u16DivPWM);
//-------------------------------------------------------------------------------------------------
/// Description:  Set the Polarity of the specific pwm
/// @param u8IndexPWM    \b which pwm is setting
/// @param bPolPWM    \b True/False for Inverse/Non-inverse
//-------------------------------------------------------------------------------------------------
void MDrv_PWM_Polarity(PWM_ChNum u8IndexPWM, BOOL bPolPWM);
//-------------------------------------------------------------------------------------------------
/// Description: Set the Vsync Double buffer of the specific pwm
/// @param u8IndexPWM    \b which pwm is setting
/// @param bVdbenPWM    \b True/False for Enable/Disable
//-------------------------------------------------------------------------------------------------
void MDrv_PWM_Vdben(PWM_ChNum u8IndexPWM, BOOL bVdbenPWM);
//-------------------------------------------------------------------------------------------------
/// Description: Set the Hsync reset of the specific pwm
/// @param u8IndexPWM    \b which pwm is setting
/// @param bRstPWM    \b True/False for Enable/Disable
//-------------------------------------------------------------------------------------------------
void MDrv_PWM_ResetEn(PWM_ChNum u8IndexPWM, BOOL bRstPWM);
//-------------------------------------------------------------------------------------------------
/// Description:  Set the Double buffer of the specific pwm
/// @param u8IndexPWM    \b which pwm is setting
/// @param bDbenPWM    \b True/False for Enable/Disable
//-------------------------------------------------------------------------------------------------
void MDrv_PWM_Dben(PWM_ChNum u8IndexPWM, BOOL bDbenPWM);
//-------------------------------------------------------------------------------------------------
/// Description: IMPULSE ENABLE
/// @param u8IndexPWM    \b which pwm is setting
/// @param bOenPWM    \b True/False for enable/disable
/// @return E_PWM_OK : succeed
/// @return E_PWM_FAIL : fail before timeout or illegal parameters
//-------------------------------------------------------------------------------------------------
void MDrv_PWM_IMPULSE_EN(PWM_ChNum index, BOOL bdbenPWM);
//-------------------------------------------------------------------------------------------------
/// Description: ODDEVEN_SYNC setting
/// @param u8IndexPWM    \b which pwm is setting
/// @param bOenPWM    \b True/False for enable/disable
/// @return E_PWM_OK : succeed
/// @return E_PWM_FAIL : fail before timeout or illegal parameters
//-------------------------------------------------------------------------------------------------
void MDrv_PWM_ODDEVEN_SYNC(PWM_ChNum index, BOOL bdbenPWM);
//-------------------------------------------------------------------------------------------------
/// Description:  Set the Rst Mux of the specific pwm
/// @param u8IndexPWM    \b which pwm is setting
/// @param bMuxPWM    \b True/False for Enable/Disable
//-------------------------------------------------------------------------------------------------
void MDrv_PWM_RstMux(PWM_ChNum u8IndexPWM, BOOL bMuxPWM);
//-------------------------------------------------------------------------------------------------
/// Description: Set the Rst_Cnt of the specific pwm
/// @param u8IndexPWM    \b which pwm is setting
/// @param u8RstCntPWM    \b the Rst_Cnt value
//-------------------------------------------------------------------------------------------------
void MDrv_PWM_RstCnt(PWM_ChNum u8IndexPWM, U8 u8RstCntPWM);
//-------------------------------------------------------------------------------------------------
/// Description: Set the Bypass Unit of the specific pwm
/// @param u8IndexPWM    \b which pwm is setting
/// @param bBypassPWM    \b True/False for Enable/Disable
//-------------------------------------------------------------------------------------------------
void MDrv_PWM_BypassUnit(PWM_ChNum u8IndexPWM, BOOL bBypassPWM);
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
PWM_Result MDrv_PWM01_CntMode(PWM_CntMode u8CntModePWM);
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
PWM_Result MDrv_PWM23_CntMode(PWM_CntMode u8CntModePWM);
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
PWM_Result MDrv_PWM67_CntMode(PWM_CntMode u8CntModePWM);
//-------------------------------------------------------------------------------------------------
/// Description: Set the Shift of the specific pwm
/// @param u8IndexPWM    \b which pwm is setting
/// @param u16DutyPWM    \b the 18-bit Shift value
/// @return E_PWM_OK : succeed
/// @return E_PWM_FAIL : fail before timeout or illegal parameters
//-------------------------------------------------------------------------------------------------
PWM_Result MDrv_PWM_Shift(PWM_ChNum u8IndexPWM, U32 u32DutyPWM);

void MDrv_PWM_Nvsync(PWM_ChNum u8IndexPWM, BOOL bNvsPWM);

void MDrv_PWM_Align(PWM_ChNum u8IndexPWM, BOOL bAliPWM);

void MDrv_PWM_ForceShitfEn(PWM_ChNum u8IndexPWM, BOOL bForceShiftPWM);

//-------------------------------------------------------------------------------------------------
/// Description: Set debug level for debug message
/// @param eLevel    \b debug level for PWM driver
/// @return E_PWM_OK : succeed
/// @return E_PWM_FAIL : fail before timeout or illegal parameters
//-------------------------------------------------------------------------------------------------
PWM_Result MDrv_PWM_SetDbgLevel(PWM_DbgLv eLevel);

#ifdef __cplusplus
}
#endif

#endif // _DRV_TEMP_H_

