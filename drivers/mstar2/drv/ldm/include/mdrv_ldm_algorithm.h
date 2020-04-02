///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2008 - 2009 Mstar Semiconductor, Inc.
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
/// @file   mdrv_ldm_algorithm.h
/// @brief  MStar local dimming Interface header file
/// @author MStar Semiconductor Inc.
/// @attention
/// <b><em></em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_LD_ALGO_H
#define _MDRV_LD_ALGO_H

#ifdef _MDRV_LDM_INTERRUPT_C
#define INTERFACE
#else
#define INTERFACE extern
#endif

#if defined(__cplusplus)
extern "C" {
#endif

//=============================================================================
// Defines & Macros
//=============================================================================
#define LD_MAX_BW       (48)//(32)
#define LD_MAX_BH       (48)//(32)
#define LD_MAX_BLOCK    (10*1024)//(1024)


#define BUFF_LENGTH       64


//=============================================================================
// Type and Structure Declaration
//=============================================================================

typedef enum
{
    //Aeon-->51 Command Index
    E_PM_CMDIDX_INIT = 0x01,
    E_PM_CMDIDX_GET_STATUS = 0x02,
    E_PM_CMDIDX_GET_LIBVER = 0x03,
    E_PM_CMDIDX_POWER_DOWN = 0x04,
    E_PM_CMDIDX_RTC_INIT = 0x05,
    E_PM_CMDIDX_RTC_SETCOUNTER = 0x06,
    E_PM_CMDIDX_RTC_GETCOUNTER = 0x07,
    E_PM_CMDIDX_RTC_SETMATCHCOUNTER = 0x08,
    E_PM_CMDIDX_RTC_GETMATCHCOUNTER = 0x09,
    E_PM_CMDIDX_GET_INFO = 0x0A,
    E_PM_CMDIDX_GET_MASTERKEY = 0x0B,
    E_PM_CMDIDX_GET_DEVICEID = 0x0C,
    E_PM_CMDIDX_GET_CHIPID = 0x0D,
    E_PM_CMDIDX_PWM_INIT =0x0F,
    E_PM_CMDIDX_PWM_CONFIG =0x10,
    E_PM_CMDIDX_PWM_IRRECORD_RECEIVED_SETCALLBACK     = 0x11,
    E_PM_CMDIDX_PWM_IRRECORD_RECEIVED_REQUIRE        = 0x12,
    E_PM_CMDIDX_PWM_IRRECORD_RECEIVED_COMPLETE      =  0x13,
    E_PM_CMDIDX_PWM_IRRECORD_TRANSMIT               =  0x14,
    E_PM_CMDIDX_PWM_IRRECORD_RECEIVED_EXIT           =  0x15,
    E_PM_CMDIDX_LD_SEND =0x16,

    E_PM_CMDIDX_GLOBAL_CHIP_RESET = 0x21,


    E_PM_CMDIDX_ACK_51ToAEON = 0x30,
    E_PM_CMDIDX_ACK_51ToARM = 0x31,

    //51->Aeon Command Index
    E_PM_CMDIDX_ACK_AEONTo51 = 0xA0,

} EN_PM_CmdIndex;

typedef enum
{
    E_LD_PM_CMD_READY = 0x00,
    E_LD_PM_CMD_INIT_SPI = 0x01,
    E_LD_PM_CMD_INIT_PWM = 0x02,
    E_LD_PM_CMD_INIT_SPI_PWM = 0x03,
    E_LD_PM_CMD_POWER_DOWN_SPI = 0x04,
    E_LD_PM_CMD_POWER_DOWN_PWM = 0x05,
    E_LD_PM_CMD_POWER_DOWN_SPI_PWM = 0x06,
    E_LD_PM_CMD_MAX
} EN_LD_PM_Cmd;


//=============================================================================
// Variable
//=============================================================================


//=============================================================================
// Function
//=============================================================================
INTERFACE MS_S8 MDrv_LD_Setup(void);
INTERFACE void MDrv_LD_CleanupModule(void);
INTERFACE void MDrv_LD_Suspend(void);
INTERFACE void MDrv_LD_Resume(void);
INTERFACE void MDrv_LD_SetGlobalStrength(MS_U8 u8GBStrength);
INTERFACE void MDrv_LD_SetLocalStrength(MS_U8 u8LDStrength);
INTERFACE MS_PHY MDrv_LD_GetDataAddr(EN_LD_GET_DATA_TYPE type);
INTERFACE void MDrv_LD_Enable(MS_BOOL bOn, MS_U8 u8Luma);
INTERFACE void MDrv_LD_SetDemoPattern(MS_U8 u8Pattern, MS_BOOL bOn, MS_U16 u16LEDNum);
INTERFACE MS_S8 MDrv_LD_Init(MS_PHY mmapaddr);
INTERFACE void MDrv_LD_DumpDRAM(EN_LD_GET_DATA_TYPE type);



#if defined(__cplusplus)
}
#endif


#endif //_MDRV_LD_ALGO_H

