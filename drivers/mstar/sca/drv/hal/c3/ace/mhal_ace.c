////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2008-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// ("MStar Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////
#define MHAL_ACE_C


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
// Common Definition
#include "MsCommon.h"
#include "MsOS.h"

// Internal Definition
#include "ace_hwreg_utility2.h"
#include "mhal_ace.h"
#include "hwreg_ace.h"
#include "MsAce_LIB.h"

#include "hwreg_ace1.h"
#include "hwreg_ace2.h"

//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

#define MAIN_WINDOW     0
#define SUB_WINDOW      1
#define SC1_MAIN_WINDOW     2
#define SC1_SUB_WINDOW      3
#define SC2_MAIN_WINDOW     4
#define SC2_SUB_WINDOW      5


//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

//static MS_BOOL  bMainWinSkipWaitVsyn = 0, bSubWinSkipWaitVsyn = 0;
static MS_BOOL bWinSkipWaintSync[MAX_WINDOW_NUM] = {0};

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

void Hal_ACE_DMS( MS_BOOL bScalerWin, MS_BOOL bisATV )
{
    if(bisATV)
    {
        if(bScalerWin == MAIN_WINDOW)
        {
            SC_W2BYTEMSK(REG_SC_BK18_70_L, 0x01, 0x01);
            SC_W2BYTEMSK(REG_SC_BK18_71_L, 0x20, LBMASK);
            SC_W2BYTEMSK(REG_SC_BK18_72_L, 0x06, LBMASK);
            SC_W2BYTEMSK(REG_SC_BK18_72_L, 0x01 <<8, HBMASK);

            SC_W2BYTEMSK(REG_SC_BK19_7B_L, 0x01, 0x01);
            SC_W2BYTEMSK(REG_SC_BK19_7C_L, 0x20, LBMASK);
            SC_W2BYTEMSK(REG_SC_BK19_7D_L, 0x06, LBMASK);
        }
        else if (SUB_WINDOW == bScalerWin)
        {
            SC_W2BYTEMSK(REG_SC_BK18_70_L, 0x10, 0x10);
            SC_W2BYTEMSK(REG_SC_BK18_73_L, 0x20, LBMASK);
            SC_W2BYTEMSK(REG_SC_BK18_74_L, 0x06, LBMASK);
            SC_W2BYTEMSK(REG_SC_BK18_74_L, 0x01 <<8, HBMASK);

            SC_W2BYTEMSK(REG_SC_BK19_7B_L, 0x10, 0x10);
            SC_W2BYTEMSK(REG_SC_BK19_7E_L, 0x20, LBMASK);
            SC_W2BYTEMSK(REG_SC_BK19_7F_L, 0x06, LBMASK);
        }
        else if( SC1_MAIN_WINDOW == bScalerWin)
        {
            SC_W2BYTEMSK(REG_SC1_BK18_70_L, 0x01, 0x01);
            SC_W2BYTEMSK(REG_SC1_BK18_71_L, 0x20, LBMASK);
            SC_W2BYTEMSK(REG_SC1_BK18_72_L, 0x06, LBMASK);
            SC_W2BYTEMSK(REG_SC1_BK18_72_L, 0x01 <<8, HBMASK);

            SC_W2BYTEMSK(REG_SC1_BK19_7B_L, 0x01, 0x01);
            SC_W2BYTEMSK(REG_SC1_BK19_7C_L, 0x20, LBMASK);
            SC_W2BYTEMSK(REG_SC1_BK19_7D_L, 0x06, LBMASK);
        }
        else if ( SC1_SUB_WINDOW == bScalerWin)
        {
            SC_W2BYTEMSK(REG_SC1_BK18_70_L, 0x10, 0x10);
            SC_W2BYTEMSK(REG_SC1_BK18_73_L, 0x20, LBMASK);
            SC_W2BYTEMSK(REG_SC1_BK18_74_L, 0x06, LBMASK);
            SC_W2BYTEMSK(REG_SC1_BK18_74_L, 0x01 <<8, HBMASK);

            SC_W2BYTEMSK(REG_SC1_BK19_7B_L, 0x10, 0x10);
            SC_W2BYTEMSK(REG_SC1_BK19_7E_L, 0x20, LBMASK);
            SC_W2BYTEMSK(REG_SC1_BK19_7F_L, 0x06, LBMASK);
        }
        else if( SC2_MAIN_WINDOW== bScalerWin )
        {
            SC_W2BYTEMSK(REG_SC2_BK18_70_L, 0x01, 0x01);
            SC_W2BYTEMSK(REG_SC2_BK18_71_L, 0x20, LBMASK);
            SC_W2BYTEMSK(REG_SC2_BK18_72_L, 0x06, LBMASK);
            SC_W2BYTEMSK(REG_SC2_BK18_72_L, 0x01 <<8, HBMASK);

            SC_W2BYTEMSK(REG_SC2_BK19_7B_L, 0x01, 0x01);
            SC_W2BYTEMSK(REG_SC2_BK19_7C_L, 0x20, LBMASK);
            SC_W2BYTEMSK(REG_SC2_BK19_7D_L, 0x06, LBMASK);
        }
        else // SC2_SUB_WINDOW
        {
            SC_W2BYTEMSK(REG_SC2_BK18_70_L, 0x10, 0x10);
            SC_W2BYTEMSK(REG_SC2_BK18_73_L, 0x20, LBMASK);
            SC_W2BYTEMSK(REG_SC2_BK18_74_L, 0x06, LBMASK);
            SC_W2BYTEMSK(REG_SC2_BK18_74_L, 0x01 <<8, HBMASK);

            SC_W2BYTEMSK(REG_SC2_BK19_7B_L, 0x10, 0x10);
            SC_W2BYTEMSK(REG_SC2_BK19_7E_L, 0x20, LBMASK);
            SC_W2BYTEMSK(REG_SC2_BK19_7F_L, 0x06, LBMASK);
        }
    }
    else
    {
        if(bScalerWin == MAIN_WINDOW)
        {
            SC_W2BYTEMSK(REG_SC_BK18_70_L, 0x00, 0x01);
            SC_W2BYTEMSK(REG_SC_BK18_71_L, 0x00, LBMASK);
            SC_W2BYTEMSK(REG_SC_BK18_72_L, 0x00, LBMASK);
            SC_W2BYTEMSK(REG_SC_BK18_72_L, 0x00 <<8, HBMASK);

            SC_W2BYTEMSK(REG_SC_BK19_7B_L, 0x00, 0x01);
            SC_W2BYTEMSK(REG_SC_BK19_7C_L, 0x00, LBMASK);
            SC_W2BYTEMSK(REG_SC_BK19_7D_L, 0x00, LBMASK);
        }
        else if (SUB_WINDOW == bScalerWin)
        {
            SC_W2BYTEMSK(REG_SC_BK18_70_L, 0x00, 0x10);
            SC_W2BYTEMSK(REG_SC_BK18_73_L, 0x00, LBMASK);
            SC_W2BYTEMSK(REG_SC_BK18_74_L, 0x00, LBMASK);
            SC_W2BYTEMSK(REG_SC_BK18_74_L, 0x00 <<8, HBMASK);

            SC_W2BYTEMSK(REG_SC_BK19_7B_L, 0x00, 0x10);
            SC_W2BYTEMSK(REG_SC_BK19_7E_L, 0x00, LBMASK);
            SC_W2BYTEMSK(REG_SC_BK19_7F_L, 0x00, LBMASK);
        }
        else if (SC1_MAIN_WINDOW == bScalerWin)
        {
            SC_W2BYTEMSK(REG_SC1_BK18_70_L, 0x00, 0x01);
            SC_W2BYTEMSK(REG_SC1_BK18_71_L, 0x00, LBMASK);
            SC_W2BYTEMSK(REG_SC1_BK18_72_L, 0x00, LBMASK);
            SC_W2BYTEMSK(REG_SC1_BK18_72_L, 0x00 <<8, HBMASK);

            SC_W2BYTEMSK(REG_SC1_BK19_7B_L, 0x00, 0x01);
            SC_W2BYTEMSK(REG_SC1_BK19_7C_L, 0x00, LBMASK);
            SC_W2BYTEMSK(REG_SC1_BK19_7D_L, 0x00, LBMASK);
        }
        else if (SC1_SUB_WINDOW == bScalerWin)
        {
            SC_W2BYTEMSK(REG_SC1_BK18_70_L, 0x00, 0x10);
            SC_W2BYTEMSK(REG_SC1_BK18_73_L, 0x00, LBMASK);
            SC_W2BYTEMSK(REG_SC1_BK18_74_L, 0x00, LBMASK);
            SC_W2BYTEMSK(REG_SC1_BK18_74_L, 0x00 <<8, HBMASK);

            SC_W2BYTEMSK(REG_SC1_BK19_7B_L, 0x00, 0x10);
            SC_W2BYTEMSK(REG_SC1_BK19_7E_L, 0x00, LBMASK);
            SC_W2BYTEMSK(REG_SC1_BK19_7F_L, 0x00, LBMASK);
        }
        else if (SC2_MAIN_WINDOW == bScalerWin)
        {
            SC_W2BYTEMSK(REG_SC2_BK18_70_L, 0x00, 0x01);
            SC_W2BYTEMSK(REG_SC2_BK18_71_L, 0x00, LBMASK);
            SC_W2BYTEMSK(REG_SC2_BK18_72_L, 0x00, LBMASK);
            SC_W2BYTEMSK(REG_SC2_BK18_72_L, 0x00 <<8, HBMASK);

            SC_W2BYTEMSK(REG_SC2_BK19_7B_L, 0x00, 0x01);
            SC_W2BYTEMSK(REG_SC2_BK19_7C_L, 0x00, LBMASK);
            SC_W2BYTEMSK(REG_SC2_BK19_7D_L, 0x00, LBMASK);
        }
        else // SC2_SUB_WINDOW
        {
            SC_W2BYTEMSK(REG_SC2_BK18_70_L, 0x00, 0x10);
            SC_W2BYTEMSK(REG_SC2_BK18_73_L, 0x00, LBMASK);
            SC_W2BYTEMSK(REG_SC2_BK18_74_L, 0x00, LBMASK);
            SC_W2BYTEMSK(REG_SC2_BK18_74_L, 0x00 <<8, HBMASK);

            SC_W2BYTEMSK(REG_SC2_BK19_7B_L, 0x00, 0x10);
            SC_W2BYTEMSK(REG_SC2_BK19_7E_L, 0x00, LBMASK);
            SC_W2BYTEMSK(REG_SC2_BK19_7F_L, 0x00, LBMASK);
        }
    }
}

// MS_U32 _ACE_RIU_BASE;

// Put this function here because hwreg_utility2 only for hal.
void Hal_ACE_init_riu_base(MS_U32 u32riu_base)
{
    _ACE_RIU_BASE = u32riu_base;
}

void Hal_ACE_SetSharpness( MS_BOOL bScalerWin, MS_U8 u8Sharpness)
{
    if((MApi_XC_MLoad_GetStatus(bScalerWin) == E_MLOAD_ENABLED) && (!Hal_ACE_GetSkipWaitVsync( bScalerWin)))
    {

        _MLOAD_ENTRY();

        if(bScalerWin == MAIN_WINDOW)
        {
            MApi_XC_MLoad_WriteCmd(REG_SC_BK19_13_L, (u8Sharpness)<<8, 0x7F00); // 7 bit sharpness!
        }
        else if (SUB_WINDOW == bScalerWin)
        {
            MApi_XC_MLoad_WriteCmd(REG_SC_BK19_17_L, (u8Sharpness)<<8, 0x7F00);
        }
        else if( SC1_MAIN_WINDOW == bScalerWin)
        {
            MApi_XC_MLoad_WriteCmd(REG_SC1_BK19_13_L, (u8Sharpness)<<8, 0x7F00); // 7 bit sharpness!
        }
        else if ( SC1_SUB_WINDOW == bScalerWin)
        {
            MApi_XC_MLoad_WriteCmd(REG_SC1_BK19_17_L, (u8Sharpness)<<8, 0x7F00);
        }
        else if( SC2_MAIN_WINDOW == bScalerWin)
        {
            MApi_XC_MLoad_WriteCmd(REG_SC2_BK19_13_L, (u8Sharpness)<<8, 0x7F00); // 7 bit sharpness!
        }
        else // SC2_SUB_WINDOW
        {
            MApi_XC_MLoad_WriteCmd(REG_SC2_BK19_17_L, (u8Sharpness)<<8, 0x7F00);
        }

        MApi_XC_MLoad_Fire(TRUE);

        _MLOAD_RETURN();
    }
    else
    {
        if( MAIN_WINDOW == bScalerWin)
        {
            SC_W2BYTEMSK(REG_SC_BK19_13_L, (u8Sharpness)<<8, 0x7F00); // 7 bit sharpness!
        }
        else if (SUB_WINDOW == bScalerWin)
        {
            SC_W2BYTEMSK(REG_SC_BK19_17_L, (u8Sharpness)<<8, 0x7F00);
        }
        else if( SC1_MAIN_WINDOW == bScalerWin)
        {
            SC_W2BYTEMSK(REG_SC1_BK19_13_L, (u8Sharpness)<<8, 0x7F00); // 7 bit sharpness!
        }
        else if ( SC1_SUB_WINDOW == bScalerWin)
        {
            SC_W2BYTEMSK(REG_SC1_BK19_17_L, (u8Sharpness)<<8, 0x7F00);
        }
        else if( SC2_MAIN_WINDOW == bScalerWin)
        {
            SC_W2BYTEMSK(REG_SC2_BK19_13_L, (u8Sharpness)<<8, 0x7F00); // 7 bit sharpness!
        }
        else // SC2_SUB_WINDOW
        {
            SC_W2BYTEMSK(REG_SC2_BK19_17_L, (u8Sharpness)<<8, 0x7F00);
        }

    }
}

void Hal_ACE_SetBrightness( MS_BOOL bScalerWin, MS_U8 u8RedBrightness, MS_U8 u8GreenBrightness, MS_U8 u8BlueBrightness )
{
    if((MApi_XC_MLoad_GetStatus(bScalerWin) == E_MLOAD_ENABLED) && (!Hal_ACE_GetSkipWaitVsync( bScalerWin)))
    {
        _MLOAD_ENTRY();

        if( MAIN_WINDOW == bScalerWin)
        {
            MApi_XC_MLoad_WriteCmd(REG_SC_BK0F_36_L, ((MS_U16)u8RedBrightness<<3), 0xFFFF );
            MApi_XC_MLoad_WriteCmd(REG_SC_BK0F_37_L, ((MS_U16)u8GreenBrightness<<3), 0xFFFF );
            MApi_XC_MLoad_WriteCmd(REG_SC_BK0F_38_L, ((MS_U16)u8BlueBrightness<<3), 0xFFFF );
        }
        else if (SUB_WINDOW == bScalerWin)
        {
            MApi_XC_MLoad_WriteCmd(REG_SC_BK0F_39_L, ((MS_U16)u8RedBrightness<<3), 0xFFFF );
            MApi_XC_MLoad_WriteCmd(REG_SC_BK0F_3A_L, ((MS_U16)u8GreenBrightness<<3), 0xFFFF );
            MApi_XC_MLoad_WriteCmd(REG_SC_BK0F_3B_L, ((MS_U16)u8BlueBrightness<<3), 0xFFFF );
        }
        else if( SC1_MAIN_WINDOW == bScalerWin)
        {
            MApi_XC_MLoad_WriteCmd(REG_SC1_BK0F_36_L, ((MS_U16)u8RedBrightness<<3), 0xFFFF );
            MApi_XC_MLoad_WriteCmd(REG_SC1_BK0F_37_L, ((MS_U16)u8GreenBrightness<<3), 0xFFFF );
            MApi_XC_MLoad_WriteCmd(REG_SC1_BK0F_38_L, ((MS_U16)u8BlueBrightness<<3), 0xFFFF );
        }
        else if (SC1_SUB_WINDOW == bScalerWin)
        {
            MApi_XC_MLoad_WriteCmd(REG_SC1_BK0F_39_L, ((MS_U16)u8RedBrightness<<3), 0xFFFF );
            MApi_XC_MLoad_WriteCmd(REG_SC1_BK0F_3A_L, ((MS_U16)u8GreenBrightness<<3), 0xFFFF );
            MApi_XC_MLoad_WriteCmd(REG_SC1_BK0F_3B_L, ((MS_U16)u8BlueBrightness<<3), 0xFFFF );
        }
        else if( SC2_MAIN_WINDOW == bScalerWin)
        {
            MApi_XC_MLoad_WriteCmd(REG_SC2_BK0F_36_L, ((MS_U16)u8RedBrightness<<3), 0xFFFF );
            MApi_XC_MLoad_WriteCmd(REG_SC2_BK0F_37_L, ((MS_U16)u8GreenBrightness<<3), 0xFFFF );
            MApi_XC_MLoad_WriteCmd(REG_SC2_BK0F_38_L, ((MS_U16)u8BlueBrightness<<3), 0xFFFF );
        }
        else // SC2_SUB_WINDOW
        {
            MApi_XC_MLoad_WriteCmd(REG_SC2_BK0F_39_L, ((MS_U16)u8RedBrightness<<3), 0xFFFF );
            MApi_XC_MLoad_WriteCmd(REG_SC2_BK0F_3A_L, ((MS_U16)u8GreenBrightness<<3), 0xFFFF );
            MApi_XC_MLoad_WriteCmd(REG_SC2_BK0F_3B_L, ((MS_U16)u8BlueBrightness<<3), 0xFFFF );
        }

        MApi_XC_MLoad_Fire(TRUE);

        _MLOAD_RETURN();

    }
    else
    {
        if(MAIN_WINDOW == bScalerWin)
        {
            SC_W2BYTE(REG_SC_BK0F_36_L, ((MS_U16)u8RedBrightness<<3) );
            SC_W2BYTE(REG_SC_BK0F_37_L, ((MS_U16)u8GreenBrightness<<3) );
            SC_W2BYTE(REG_SC_BK0F_38_L, ((MS_U16)u8BlueBrightness<<3) );
        }
        else if (SUB_WINDOW==bScalerWin)
        {
            SC_W2BYTE(REG_SC_BK0F_39_L, ((MS_U16)u8RedBrightness<<3) );
            SC_W2BYTE(REG_SC_BK0F_3A_L, ((MS_U16)u8GreenBrightness<<3) );
            SC_W2BYTE(REG_SC_BK0F_3B_L, ((MS_U16)u8BlueBrightness<<3) );
        }

    }
}

void Hal_ACE_SetBrightnessPrecise(MS_BOOL bScalerWin, MS_U16 u16RedBrightness, MS_U16 u16GreenBrightness, MS_U16 u16BlueBrightness)
{
    if(MApi_XC_MLoad_GetStatus(bScalerWin) == E_MLOAD_ENABLED)
    {
        _MLOAD_ENTRY();

        if(bScalerWin == MAIN_WINDOW)
        {
            MApi_XC_MLoad_WriteCmd(REG_SC_BK0F_36_L, u16RedBrightness, 0xFFFF);
            MApi_XC_MLoad_WriteCmd(REG_SC_BK0F_37_L, u16GreenBrightness, 0xFFFF);
            MApi_XC_MLoad_WriteCmd(REG_SC_BK0F_38_L, u16BlueBrightness, 0xFFFF);
        }
        else
        {
            MApi_XC_MLoad_WriteCmd(REG_SC_BK0F_39_L, u16RedBrightness, 0xFFFF);
            MApi_XC_MLoad_WriteCmd(REG_SC_BK0F_3A_L, u16GreenBrightness, 0xFFFF);
            MApi_XC_MLoad_WriteCmd(REG_SC_BK0F_3B_L, u16BlueBrightness, 0xFFFF);
        }

        MApi_XC_MLoad_Fire(TRUE);

        _MLOAD_RETURN();
    }
    else
    {
        if ( MAIN_WINDOW == bScalerWin )
        {
            SC_W2BYTE(REG_SC_BK0F_36_L, u16RedBrightness);
            SC_W2BYTE(REG_SC_BK0F_37_L, u16GreenBrightness);
            SC_W2BYTE(REG_SC_BK0F_38_L, u16BlueBrightness);
        }
        else if (SUB_WINDOW == bScalerWin)
        {
            SC_W2BYTE(REG_SC_BK0F_39_L, u16RedBrightness);
            SC_W2BYTE(REG_SC_BK0F_3A_L, u16GreenBrightness);
            SC_W2BYTE(REG_SC_BK0F_3B_L, u16BlueBrightness);
        }
        else if (SC1_MAIN_WINDOW == bScalerWin)
        {
            SC_W2BYTE(REG_SC1_BK0F_36_L, u16RedBrightness);
            SC_W2BYTE(REG_SC1_BK0F_37_L, u16GreenBrightness);
            SC_W2BYTE(REG_SC1_BK0F_38_L, u16BlueBrightness);
        }
        else if (SC1_SUB_WINDOW == bScalerWin)
        {
            SC_W2BYTE(REG_SC1_BK0F_39_L, u16RedBrightness);
            SC_W2BYTE(REG_SC1_BK0F_3A_L, u16GreenBrightness);
            SC_W2BYTE(REG_SC1_BK0F_3B_L, u16BlueBrightness);
        }
        else if (SC2_MAIN_WINDOW == bScalerWin)
        {
            SC_W2BYTE(REG_SC2_BK0F_36_L, u16RedBrightness);
            SC_W2BYTE(REG_SC2_BK0F_37_L, u16GreenBrightness);
            SC_W2BYTE(REG_SC2_BK0F_38_L, u16BlueBrightness);
        }
        else // SC2_SUB_WINDOW
        {
            SC_W2BYTE(REG_SC2_BK0F_39_L, u16RedBrightness);
            SC_W2BYTE(REG_SC2_BK0F_3A_L, u16GreenBrightness);
            SC_W2BYTE(REG_SC2_BK0F_3B_L, u16BlueBrightness);
        }
    }
}

void Hal_ACE_SetPostRGBGain(MS_BOOL bScalerWin, MS_U16 u16RedGain, MS_U16 u16GreenGain, MS_U16 u16BlueGain)
{
    if(MAIN_WINDOW == bScalerWin)
    {
        SC_W2BYTE(REG_SC_BK25_24_L, u16RedGain);
        SC_W2BYTE(REG_SC_BK25_25_L, u16GreenGain);
        SC_W2BYTE(REG_SC_BK25_26_L, u16BlueGain);
        SC_W2BYTEMSK(REG_SC_BK25_01_L, BIT(6) | BIT(5) | BIT(4), BIT(6) | BIT(5) | BIT(4));
    }
    else if (SUB_WINDOW == bScalerWin)
    {
        SC_W2BYTE(REG_SC_BK25_2A_L, u16RedGain);
        SC_W2BYTE(REG_SC_BK25_2B_L, u16GreenGain);
        SC_W2BYTE(REG_SC_BK25_2C_L, u16BlueGain);
        SC_W2BYTEMSK(REG_SC_BK25_11_L, BIT(6) | BIT(5) | BIT(4), BIT(6) | BIT(5) | BIT(4));
    }
    else if( SC1_MAIN_WINDOW == bScalerWin)
    {
        SC_W2BYTE(REG_SC1_BK25_24_L, u16RedGain);
        SC_W2BYTE(REG_SC1_BK25_25_L, u16GreenGain);
        SC_W2BYTE(REG_SC1_BK25_26_L, u16BlueGain);
        SC_W2BYTEMSK(REG_SC1_BK25_01_L, BIT(6) | BIT(5) | BIT(4), BIT(6) | BIT(5) | BIT(4));
    }
    else if ( SC1_SUB_WINDOW == bScalerWin)
    {
        SC_W2BYTE(REG_SC1_BK25_2A_L, u16RedGain);
        SC_W2BYTE(REG_SC1_BK25_2B_L, u16GreenGain);
        SC_W2BYTE(REG_SC1_BK25_2C_L, u16BlueGain);
        SC_W2BYTEMSK(REG_SC1_BK25_11_L, BIT(6) | BIT(5) | BIT(4), BIT(6) | BIT(5) | BIT(4));
    }
    else if( SC2_MAIN_WINDOW == bScalerWin)
    {
        SC_W2BYTE(REG_SC2_BK25_24_L, u16RedGain);
        SC_W2BYTE(REG_SC2_BK25_25_L, u16GreenGain);
        SC_W2BYTE(REG_SC2_BK25_26_L, u16BlueGain);
        SC_W2BYTEMSK(REG_SC2_BK25_01_L, BIT(6) | BIT(5) | BIT(4), BIT(6) | BIT(5) | BIT(4));
    }
    else // SC2_SUB_WINDOW
    {
        SC_W2BYTE(REG_SC2_BK25_2A_L, u16RedGain);
        SC_W2BYTE(REG_SC2_BK25_2B_L, u16GreenGain);
        SC_W2BYTE(REG_SC2_BK25_2C_L, u16BlueGain);
        SC_W2BYTEMSK(REG_SC2_BK25_11_L, BIT(6) | BIT(5) | BIT(4), BIT(6) | BIT(5) | BIT(4));
    }

}

void Hal_ACE_SetPostRGBOffset(MS_BOOL bScalerWin, MS_U16 u16RedOffset, MS_U16 u16GreenOffset, MS_U16 u16BlueOffset)
{
    if(MAIN_WINDOW == bScalerWin)
    {
        SC_W2BYTE(REG_SC_BK25_21_L, u16RedOffset);
        SC_W2BYTE(REG_SC_BK25_22_L, u16GreenOffset);
        SC_W2BYTE(REG_SC_BK25_23_L, u16BlueOffset);
        SC_W2BYTEMSK(REG_SC_BK25_01_L, BIT(6) | BIT(5) | BIT(4), BIT(6) | BIT(5) | BIT(4));
    }
    else if (SUB_WINDOW == bScalerWin)
    {
        SC_W2BYTE(REG_SC_BK25_27_L, u16RedOffset);
        SC_W2BYTE(REG_SC_BK25_28_L, u16GreenOffset);
        SC_W2BYTE(REG_SC_BK25_29_L, u16BlueOffset);
        SC_W2BYTEMSK(REG_SC_BK25_11_L, BIT(6) | BIT(5) | BIT(4), BIT(6) | BIT(5) | BIT(4));
    }
    else if( SC1_MAIN_WINDOW == bScalerWin)
    {
        SC_W2BYTE(REG_SC1_BK25_21_L, u16RedOffset);
        SC_W2BYTE(REG_SC1_BK25_22_L, u16GreenOffset);
        SC_W2BYTE(REG_SC1_BK25_23_L, u16BlueOffset);
        SC_W2BYTEMSK(REG_SC1_BK25_01_L, BIT(6) | BIT(5) | BIT(4), BIT(6) | BIT(5) | BIT(4));
    }
    else if ( SC1_SUB_WINDOW == bScalerWin)
    {
        SC_W2BYTE(REG_SC1_BK25_27_L, u16RedOffset);
        SC_W2BYTE(REG_SC1_BK25_28_L, u16GreenOffset);
        SC_W2BYTE(REG_SC1_BK25_29_L, u16BlueOffset);
        SC_W2BYTEMSK(REG_SC1_BK25_11_L, BIT(6) | BIT(5) | BIT(4), BIT(6) | BIT(5) | BIT(4));
    }
    else if( SC2_MAIN_WINDOW == bScalerWin)
    {
        SC_W2BYTE(REG_SC2_BK25_21_L, u16RedOffset);
        SC_W2BYTE(REG_SC2_BK25_22_L, u16GreenOffset);
        SC_W2BYTE(REG_SC2_BK25_23_L, u16BlueOffset);
        SC_W2BYTEMSK(REG_SC2_BK25_01_L, BIT(6) | BIT(5) | BIT(4), BIT(6) | BIT(5) | BIT(4));
    }
    else // SC2_SUB_WINDOW
    {
        SC_W2BYTE(REG_SC2_BK25_27_L, u16RedOffset);
        SC_W2BYTE(REG_SC2_BK25_28_L, u16GreenOffset);
        SC_W2BYTE(REG_SC2_BK25_29_L, u16BlueOffset);
        SC_W2BYTEMSK(REG_SC2_BK25_11_L, BIT(6) | BIT(5) | BIT(4), BIT(6) | BIT(5) | BIT(4));
    }

}

void Hal_ACE_GetColorMatrix( MS_BOOL bScalerWin, MS_U16* pu16Matrix)
{
    MS_U8 i;

    if(MAIN_WINDOW == bScalerWin)
    {
        for (i=0; i<9; i++)
        {
            *pu16Matrix = SC_R2BYTE((REG_SC_BK10_26_L + (2 * i)));
            pu16Matrix++;
        }
    }
    else if (SUB_WINDOW==bScalerWin)
    {
        for (i=0; i<9; i++)
        {
            *pu16Matrix = SC_R2BYTE((REG_SC_BK0F_1D_L + (2 * i)));
            pu16Matrix++;
        }
    }
    else if( SC1_MAIN_WINDOW == bScalerWin)
    {
        for (i=0; i<9; i++)
        {
            *pu16Matrix = SC_R2BYTE((REG_SC1_BK10_26_L + (2 * i)));
            pu16Matrix++;
        }
    }
    else if ( SC1_SUB_WINDOW==bScalerWin)
    {
        for (i=0; i<9; i++)
        {
            *pu16Matrix = SC_R2BYTE((REG_SC1_BK0F_1D_L + (2 * i)));
            pu16Matrix++;
        }
    }
    else if( SC2_MAIN_WINDOW == bScalerWin)
    {
        for (i=0; i<9; i++)
        {
            *pu16Matrix = SC_R2BYTE((REG_SC2_BK10_26_L + (2 * i)));
            pu16Matrix++;
        }
    }
    else // SC2_SUB_WINDOW
    {
        for (i=0; i<9; i++)
        {
            *pu16Matrix = SC_R2BYTE((REG_SC2_BK0F_1D_L + (2 * i)));
            pu16Matrix++;
        }
    }

}

void Hal_ACE_SetColorMatrix( MS_BOOL bScalerWin, MS_U16* pu16Matrix)
{
    MS_U8 i;

    if(MAIN_WINDOW == bScalerWin)
    {
        for (i=0; i<9; i++)
        {
            SC_W2BYTE( (REG_SC_BK10_26_L+(2 * i)), *pu16Matrix);
            pu16Matrix++;
        }
    }
    else if ( SUB_WINDOW == bScalerWin)
    {
        for (i=0; i<9; i++)
        {
            SC_W2BYTE( (REG_SC_BK0F_1D_L+(2 * i)), *pu16Matrix);
            pu16Matrix++;
        }
    }
    else if( SC1_MAIN_WINDOW == bScalerWin)
    {
        for (i=0; i<9; i++)
        {
            SC_W2BYTE( (REG_SC1_BK10_26_L+(2 * i)), *pu16Matrix);
            pu16Matrix++;
        }
    }
    else if ( SC1_SUB_WINDOW == bScalerWin)
    {
        for (i=0; i<9; i++)
        {
            SC_W2BYTE( (REG_SC1_BK0F_1D_L+(2 * i)), *pu16Matrix);
            pu16Matrix++;
        }
    }
    else if( SC2_MAIN_WINDOW == bScalerWin)
    {
        for (i=0; i<9; i++)
        {
            SC_W2BYTE( (REG_SC2_BK10_26_L+(2 * i)), *pu16Matrix);
            pu16Matrix++;
        }
    }
    else // SC2_SUB_WINDOW
    {
        for (i=0; i<9; i++)
        {
            SC_W2BYTE( (REG_SC2_BK0F_1D_L+(2 * i)), *pu16Matrix);
            pu16Matrix++;
        }
    }
}

void Hal_ACE_PatchDTGColorChecker(MS_U8 u8Mode)
{
    static MS_U8 u8Red, u8Green, u8Blue;
    static MS_U8 u8VIP_10L, u8VIP_30L, u8VIP_40L, u8VIP_48L,
          u8VIP_60L, u8DLC_10H, u8DLC_14L, u8DLC_14H,
          u8DLC_0FL, u8VOP_50L;
    if(u8Mode)
    {
        // specific settings for MHEG5
        u8VIP_10L  = SC_R2BYTEMSK(REG_SC_BK18_10_L, LBMASK);
        u8VIP_30L  = SC_R2BYTEMSK(REG_SC_BK18_30_L, LBMASK);
        u8VIP_40L  = SC_R2BYTEMSK(REG_SC_BK18_40_L, LBMASK);
        u8VIP_48L  = SC_R2BYTEMSK(REG_SC_BK18_48_L, LBMASK);
        u8VIP_60L  = SC_R2BYTEMSK(REG_SC_BK18_60_L, LBMASK);

        u8DLC_0FL  = SC_R2BYTEMSK(REG_SC_BK1A_0F_L, LBMASK);
        u8DLC_10H  = SC_R2BYTEMSK(REG_SC_BK1A_10_L, HBMASK);
        u8DLC_14L  = SC_R2BYTEMSK(REG_SC_BK1A_14_L, LBMASK);
        u8DLC_14H  = SC_R2BYTEMSK(REG_SC_BK1A_14_L, HBMASK);

        u8Red      = SC_R2BYTEMSK(REG_SC_BK10_17_L, LBMASK);
        u8Green    = SC_R2BYTEMSK(REG_SC_BK10_17_L, HBMASK);
        u8Blue     = SC_R2BYTEMSK(REG_SC_BK10_18_L, LBMASK);
        u8VOP_50L  = SC_R2BYTEMSK(REG_SC_BK10_50_L, LBMASK);

        SC_W2BYTEMSK(REG_SC_BK18_10_L, 0x00, LBMASK);
        SC_W2BYTEMSK(REG_SC_BK18_30_L, 0x80, LBMASK);
        SC_W2BYTEMSK(REG_SC_BK18_40_L, 0x00, LBMASK);
        SC_W2BYTEMSK(REG_SC_BK18_48_L, 0x00, LBMASK);
        SC_W2BYTEMSK(REG_SC_BK18_60_L, 0x00, LBMASK);

        SC_W2BYTEMSK(REG_SC_BK1A_0F_L, 0xF0, LBMASK);
        SC_W2BYTEMSK(REG_SC_BK1A_10_L, 0x80 <<8, HBMASK);
        SC_W2BYTEMSK(REG_SC_BK1A_14_L, 0x40, LBMASK);
        SC_W2BYTEMSK(REG_SC_BK1A_14_L, 0x40 <<8, HBMASK);

        SC_W2BYTEMSK(REG_SC_BK10_17_L, 0x80, LBMASK);
        SC_W2BYTEMSK(REG_SC_BK10_17_L, 0x80 <<8, HBMASK);
        SC_W2BYTEMSK(REG_SC_BK10_18_L, 0x80, LBMASK);
        SC_W2BYTEMSK(REG_SC_BK10_50_L, 0x00, LBMASK);

    }
    else
    {
        SC_W2BYTEMSK(REG_SC_BK18_10_L, u8VIP_10L, LBMASK);
        SC_W2BYTEMSK(REG_SC_BK18_30_L, u8VIP_30L, LBMASK);
        SC_W2BYTEMSK(REG_SC_BK18_40_L, u8VIP_40L, LBMASK);
        SC_W2BYTEMSK(REG_SC_BK18_48_L, u8VIP_48L, LBMASK);
        SC_W2BYTEMSK(REG_SC_BK18_60_L, u8VIP_60L, LBMASK);

        SC_W2BYTEMSK(REG_SC_BK1A_0F_L, u8DLC_10H, LBMASK);
        SC_W2BYTEMSK(REG_SC_BK1A_10_L, ((MS_U16)u8DLC_14L) <<8, HBMASK);
        SC_W2BYTEMSK(REG_SC_BK1A_14_L, u8DLC_14H, LBMASK);
        SC_W2BYTEMSK(REG_SC_BK1A_14_L, ((MS_U16)u8DLC_0FL) <<8, HBMASK);

        SC_W2BYTEMSK(REG_SC_BK10_17_L, u8Red, LBMASK);
        SC_W2BYTEMSK(REG_SC_BK10_17_L, ((MS_U16)u8Green) <<8, HBMASK);
        SC_W2BYTEMSK(REG_SC_BK10_18_L, u8Blue, LBMASK);
        SC_W2BYTEMSK(REG_SC_BK10_50_L, u8VOP_50L, LBMASK);
    }
}

void Hal_ACE_SetSlopValue( MS_BOOL bScalerWin, MS_U8 u8SlopValue)
{
    if( MAIN_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC_BK1A_10_L, ((MS_U16)u8SlopValue) <<8, HBMASK);
    }
    else if (SUB_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC_BK1A_12_L, ((MS_U16)u8SlopValue) <<8, HBMASK);
    }
    else if( SC1_MAIN_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC1_BK1A_10_L, ((MS_U16)u8SlopValue) <<8, HBMASK);
    }
    else if ( SC1_SUB_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC1_BK1A_12_L, ((MS_U16)u8SlopValue) <<8, HBMASK);
    }
    else if( SC2_MAIN_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC2_BK1A_10_L, ((MS_U16)u8SlopValue) <<8, HBMASK);
    }
    else // SC2_SUB_WINDOW
    {
        SC_W2BYTEMSK(REG_SC2_BK1A_12_L, ((MS_U16)u8SlopValue) <<8, HBMASK);
    }

}

void Hal_ACE_SetFCC_En( MS_BOOL bScalerWin, MS_U8 u8Reg, MS_BOOL bEn)
{
    MS_U16 u16value;

    if(u8Reg > 7)
        u8Reg = 7;

    if (bEn)
    {
        if ( MAIN_WINDOW== bScalerWin)
            u16value = (SC_R2BYTE(REG_SC_BK18_10_L) & 0x000F) | (1 << u8Reg);
        else if (SUB_WINDOW==bScalerWin)
            u16value = (SC_R2BYTE(REG_SC_BK18_11_L) & 0x000F) | (1 << u8Reg);
        else if ( SC1_MAIN_WINDOW== bScalerWin)
            u16value = (SC_R2BYTE(REG_SC1_BK18_10_L) & 0x000F) | (1 << u8Reg);
        else if ( SC1_SUB_WINDOW==bScalerWin)
            u16value = (SC_R2BYTE(REG_SC1_BK18_11_L) & 0x000F) | (1 << u8Reg);
        else if ( SC2_MAIN_WINDOW== bScalerWin)
            u16value = (SC_R2BYTE(REG_SC2_BK18_10_L) & 0x000F) | (1 << u8Reg);
        else // SC2_SUB_WINDOW
            u16value = (SC_R2BYTE(REG_SC2_BK18_11_L) & 0x000F) | (1 << u8Reg);

    }
    else
    {
        if(MAIN_WINDOW == bScalerWin)
            u16value = (SC_R2BYTE(REG_SC_BK18_10_L) & 0x000F) & ~(1 << u8Reg);
        else if ( SUB_WINDOW == bScalerWin)
            u16value = (SC_R2BYTE(REG_SC_BK18_11_L) & 0x000F) & ~(1 << u8Reg);
        else if ( SC1_MAIN_WINDOW == bScalerWin)
            u16value = (SC_R2BYTE(REG_SC1_BK18_10_L) & 0x000F) & ~(1 << u8Reg);
        else if ( SC1_SUB_WINDOW == bScalerWin)
            u16value = (SC_R2BYTE(REG_SC1_BK18_11_L) & 0x000F) & ~(1 << u8Reg);
        else if ( SC2_MAIN_WINDOW == bScalerWin)
            u16value = (SC_R2BYTE(REG_SC2_BK18_10_L) & 0x000F) & ~(1 << u8Reg);
        else // SC2_SUB_WINDOW
            u16value = (SC_R2BYTE(REG_SC2_BK18_11_L) & 0x000F) & ~(1 << u8Reg);

    }

    SC_W2BYTEMSK(REG_SC_BK18_10_L, u16value, 0x000F);
}


void Hal_ACE_SetFCC_Cb(MS_U8 u8Reg, MS_U8 u8value)
{
    MS_U32 u32reg_addr;

    if(u8Reg > 7)
        u8Reg = 7;

    u32reg_addr = REG_SC_BK18_18_L + u8Reg * 2;
    SC_W2BYTEMSK(u32reg_addr, (MS_U16)u8value, LBMASK);
}

void Hal_ACE_Set_IHC_SRAM(MS_U8 *pBuf, MS_U8 u8SRAM_Idx, MS_U16 u16Cnt)
{
    MS_U16 i;

    if(u8SRAM_Idx > 3)
    {
        u8SRAM_Idx = 0;
    }

    SC_W2BYTEMSK(REG_SC_BK18_7C_L, BIT(0), BIT(0)); // io_en disable
    SC_W2BYTEMSK(REG_SC_BK18_7C_L, u8SRAM_Idx<<1, BIT(2)|BIT(1)); // sram select

    for(i=0; i<u16Cnt; i++)
    {
        while (SC_R2BYTE(REG_SC_BK18_7E_L) & BIT(8));

        SC_W2BYTEMSK(REG_SC_BK18_7D_L, i, 0x00FF); // address
        SC_W2BYTEMSK(REG_SC_BK18_7E_L, pBuf[i], 0x00FF); //data

        SC_W2BYTEMSK(REG_SC_BK18_7E_L, BIT(8), BIT(8)); // io_w enable
    }

    SC_W2BYTEMSK(REG_SC_BK18_7C_L, 0, BIT(0)); // io_en enable
}

void Hal_ACE_Set_ICC_SRAM(MS_U16 *pBuf, MS_U16 u16Cnt)
{
    MS_U16 i;
    SC_W2BYTEMSK(REG_SC_BK18_78_L, BIT(0), BIT(0)); // io_en disable

    for(i=0; i<u16Cnt; i++)
    {
        while (SC_R2BYTE(REG_SC_BK18_7A_L) & BIT(8));

        SC_W2BYTEMSK(REG_SC_BK18_79_L, i, 0x00FF); // address
        SC_W2BYTEMSK(REG_SC_BK18_7A_L, (pBuf[i] & 0xFF), 0x00FF);       //data
        SC_W2BYTEMSK(REG_SC_BK18_7B_L, ((pBuf[i] & 0x100)<<4), 0x1000); //sign data

        SC_W2BYTEMSK(REG_SC_BK18_7A_L, BIT(8), BIT(8)); // io_w enable
    }

    SC_W2BYTEMSK(REG_SC_BK18_78_L, 0, BIT(0)); // io_en enable
}

////////////////////////////////////////////////////////////////////////////////
//
//  MWE start
//
////////////////////////////////////////////////////////////////////////////////
void MHal_SC_SetMWEQuality(void)
{
    MS_U16 value ;

    //////////////////////////////////////
    /// Copy main window setting
    //////////////////////////////////////
    // Copy H-Peak
    value = SC_R2BYTE(REG_SC_BK19_10_L);
    SC_W2BYTE(REG_SC_BK19_14_L, value);
    value = SC_R2BYTE(REG_SC_BK19_11_L);
    SC_W2BYTE(REG_SC_BK19_15_L, value );
    value = SC_R2BYTE(REG_SC_BK19_12_L);
    SC_W2BYTE(REG_SC_BK19_16_L, value );
    value = SC_R2BYTE(REG_SC_BK19_13_L);
    SC_W2BYTE(REG_SC_BK19_17_L, value );
    value = SC_R2BYTE(REG_SC_BK19_18_L);
    SC_W2BYTE(REG_SC_BK19_28_L, value );
    value = SC_R2BYTE(REG_SC_BK19_19_L);
    SC_W2BYTE(REG_SC_BK19_29_L, value );
    value = SC_R2BYTE(REG_SC_BK19_1A_L);
    SC_W2BYTE(REG_SC_BK19_2A_L, value );
    value = SC_R2BYTE(REG_SC_BK19_1B_L);
    SC_W2BYTE(REG_SC_BK19_2B_L, value );
    value = SC_R2BYTE(REG_SC_BK19_1C_L);
    SC_W2BYTE(REG_SC_BK19_2C_L, value );
    value = SC_R2BYTE(REG_SC_BK19_1D_L);
    SC_W2BYTE(REG_SC_BK19_2D_L, value );
    value = SC_R2BYTE(REG_SC_BK19_1E_L);
    SC_W2BYTE(REG_SC_BK19_2E_L, value );
    value = SC_R2BYTE(REG_SC_BK19_1F_L);
    SC_W2BYTE(REG_SC_BK19_2F_L, value );


    // Copy FCC from main window
    value = SC_R2BYTE(REG_SC_BK18_10_L);
    SC_W2BYTE(REG_SC_BK18_11_L, value);

    // Copy ICC
    value = SC_R2BYTE(REG_SC_BK18_30_L) ;
    SC_W2BYTEMSK(REG_SC_BK18_30_L, value >> 4, (BIT(2)|BIT(3)));
    value = SC_R2BYTE(REG_SC_BK18_31_L);
    SC_W2BYTEMSK(REG_SC_BK18_31_L, (value  << 4), (BIT(4)|BIT(5)|BIT(6)|BIT(7)|BIT(12)|BIT(13)|BIT(14)|BIT(15)));
    value = SC_R2BYTE(REG_SC_BK18_32_L);
    SC_W2BYTEMSK(REG_SC_BK18_32_L, ( value << 4), (BIT(4)|BIT(5)|BIT(6)|BIT(7)|BIT(12)|BIT(13)|BIT(14)|BIT(15)));
    value = SC_R2BYTE(REG_SC_BK18_33_L);
    SC_W2BYTEMSK(REG_SC_BK18_33_L, ( value << 4), (BIT(4)|BIT(5)|BIT(6)|BIT(7)|BIT(12)|BIT(13)|BIT(14)|BIT(15)));
    value = SC_R2BYTE(REG_SC_BK18_34_L);
    SC_W2BYTEMSK(REG_SC_BK18_34_L, (value  << 4), (BIT(4)|BIT(5)|BIT(6)|BIT(7)));
    value = SC_R2BYTE(REG_SC_BK18_35_L);
    SC_W2BYTEMSK(REG_SC_BK18_35_L, (value << 8), 0xFF00);

    // IBC
    value = SC_R2BYTE(REG_SC_BK18_40_L);
    SC_W2BYTEMSK(REG_SC_BK18_40_L, value >> 1, BIT(6));
    value = SC_R2BYTE(REG_SC_BK18_41_L);
    SC_W2BYTE(REG_SC_BK18_45_L, value );
    value = SC_R2BYTE(REG_SC_BK18_42_L);
    SC_W2BYTE(REG_SC_BK18_46_L, value );
    value = SC_R2BYTE(REG_SC_BK18_43_L);
    SC_W2BYTE(REG_SC_BK18_47_L, value );
    value = SC_R2BYTE(REG_SC_BK18_44_L);
    SC_W2BYTEMSK(REG_SC_BK18_48_L, value ,(BIT(0)|BIT(1)|BIT(2)|BIT(3)|BIT(4)|BIT(5)));

    // Y/C noise masking
    value = SC_R2BYTE(REG_SC_BK18_55_L);
    SC_W2BYTEMSK(REG_SC_BK18_5D_L, value ,(BIT(0)|BIT(1)|BIT(2)|BIT(3)|BIT(4)|BIT(5)|BIT(7)|BIT(8)|BIT(9)|BIT(10)|BIT(11)|BIT(12)|BIT(13)|BIT(15)));

    // IHC
    value = SC_R2BYTE(REG_SC_BK18_60_L);
    SC_W2BYTEMSK(REG_SC_BK18_60_L, value >> 1, BIT(6));
    // YC-Coring
    value = SC_R2BYTE(REG_SC_BK18_50_L);
    SC_W2BYTE(REG_SC_BK18_58_L, value );
    value = SC_R2BYTE(REG_SC_BK18_51_L);
    SC_W2BYTE(REG_SC_BK18_59_L, value);
    value = SC_R2BYTE(REG_SC_BK18_52_L);
    SC_W2BYTE(REG_SC_BK18_5A_L, value);
    value = SC_R2BYTE(REG_SC_BK18_53_L);
    SC_W2BYTE(REG_SC_BK18_5B_L, value);
    value = SC_R2BYTE(REG_SC_BK18_54_L);
    SC_W2BYTE(REG_SC_BK18_5C_L, value);

    // copy Y-Adjust
    value = SC_R2BYTE(REG_SC_BK1A_0E_L);
    SC_W2BYTEMSK(REG_SC_BK1A_0E_L, (value << 8), BIT(8) | BIT(9));
    value = SC_R2BYTE(REG_SC_BK1A_0F_L);
    SC_W2BYTEMSK(REG_SC_BK1A_0F_L, (value << 8), 0xFF00);

    // copy BLE
    value = SC_R2BYTE(REG_SC_BK1A_10_L);
    SC_W2BYTE(REG_SC_BK1A_12_L, value );
    // copy WLE
    value =  SC_R2BYTE(REG_SC_BK1A_11_L);
    SC_W2BYTE(REG_SC_BK1A_13_L, value );
    // copy Y/C gain control
    value = SC_R2BYTE(REG_SC_BK1A_14_L);
    SC_W2BYTE(REG_SC_BK1A_15_L, value );
    // Histogram data
    value = SC_R2BYTE(REG_SC_BK1A_01_L);
    SC_W2BYTE(REG_SC_BK1A_03_L, value );

    // Copy DLC table
    value = SC_R2BYTE(REG_SC_BK1A_30_L);
    SC_W2BYTE(REG_SC_BK1A_38_L, value);
    value = SC_R2BYTE(REG_SC_BK1A_31_L);
    SC_W2BYTE(REG_SC_BK1A_39_L, value );
    value = SC_R2BYTE(REG_SC_BK1A_32_L);
    SC_W2BYTE(REG_SC_BK1A_3A_L, value );
    value = SC_R2BYTE(REG_SC_BK1A_33_L);
    SC_W2BYTE(REG_SC_BK1A_3B_L, value );
    value = SC_R2BYTE(REG_SC_BK1A_34_L);
    SC_W2BYTE(REG_SC_BK1A_3C_L, value );
    value = SC_R2BYTE(REG_SC_BK1A_35_L);
    SC_W2BYTE(REG_SC_BK1A_3D_L, value);
    value = SC_R2BYTE(REG_SC_BK1A_36_L);
    SC_W2BYTE(REG_SC_BK1A_3E_L, value );
    value = SC_R2BYTE(REG_SC_BK1A_37_L);
    SC_W2BYTE(REG_SC_BK1A_3F_L, value);

    // Copy Statistic
    value = SC_R2BYTE(REG_SC_BK1A_04_L);
    SC_W2BYTEMSK(REG_SC_BK1A_04_L, value >> 1 , BIT(0));

    // Copy Luma curve
    value = SC_R2BYTE(REG_SC_BK1A_04_L);
    SC_W2BYTEMSK(REG_SC_BK1A_04_L, value  >> 1 , BIT(6));
    // Read Brightness setting from main window
    value = SC_R2BYTE(REG_SC_BK0F_36_L);
    SC_W2BYTE(REG_SC_BK0F_39_L, value );
    value = SC_R2BYTE(REG_SC_BK0F_37_L);
    SC_W2BYTE(REG_SC_BK0F_3A_L, value );
    value = SC_R2BYTE(REG_SC_BK0F_38_L);
    SC_W2BYTE(REG_SC_BK0F_3B_L, value );

    // copy Post Scaling filter selection from main to sub
    value = SC_R2BYTE(REG_SC_BK23_0B_L);
    SC_W2BYTE(REG_SC_BK23_2B_L, value );

    //copy rgb2yuv enable flag
    value = SC_R2BYTE(REG_SC_BK18_6E_L);
    SC_W2BYTEMSK(REG_SC_BK18_6E_L, value << 4, BIT(4));
    value = SC_R2BYTE(REG_SC_BK18_6F_L);
    SC_W2BYTEMSK(REG_SC_BK18_6F_L, value << 4, BIT(4) | BIT(5));

    // copy pre Y gain control
    value = SC_R2BYTE(REG_SC_BK1A_16_L);
    SC_W2BYTEMSK(REG_SC_BK1A_16_L, (value << 8), 0xFF00);
    //clear sub window low 4bit to keep difference with main
    SC_W2BYTEMSK(REG_SC_BK1A_16_L, 0x0000, 0x0F00);

    // color matrix

    {

        value = SC_R2BYTE(REG_SC_BK10_26_L);
        SC_W2BYTE(REG_SC_BK0F_1D_L, value );

        value = SC_R2BYTE(REG_SC_BK10_27_L);
        SC_W2BYTE(REG_SC_BK0F_1E_L, value );

        value = SC_R2BYTE(REG_SC_BK10_28_L);
        SC_W2BYTE(REG_SC_BK0F_1F_L, value );

        value = SC_R2BYTE(REG_SC_BK10_29_L);
        SC_W2BYTE(REG_SC_BK0F_20_L, value );

        value = SC_R2BYTE(REG_SC_BK10_2A_L);
        SC_W2BYTE(REG_SC_BK0F_21_L, value );

        value = SC_R2BYTE(REG_SC_BK10_2B_L);
        SC_W2BYTE(REG_SC_BK0F_22_L,value);

        value = SC_R2BYTE(REG_SC_BK10_2C_L);
        SC_W2BYTE(REG_SC_BK0F_23_L, value );

        value = SC_R2BYTE(REG_SC_BK10_2D_L);
        SC_W2BYTE(REG_SC_BK0F_24_L,value );

        value = SC_R2BYTE(REG_SC_BK10_2E_L);
        SC_W2BYTE(REG_SC_BK0F_25_L,value );

        value = SC_R2BYTE(REG_SC_BK10_2F_L);
        SC_W2BYTE(REG_SC_BK0F_26_L,value);

    }

    //////////////////////////////////////
    /// Disable
    //////////////////////////////////////
    //disable ICC

    SC_W2BYTEMSK(REG_SC_BK18_30_L, 0x00 ,BIT(2));
    //disable IBC
    SC_W2BYTEMSK(REG_SC_BK18_40_L, 0x00 ,BIT(6));
    //disable IHC
    SC_W2BYTEMSK(REG_SC_BK18_60_L, 0x00 ,BIT(6));
    //disable DLC ( fresh contrast )
    SC_W2BYTEMSK(REG_SC_BK18_40_L, 0x00 ,BIT(6));

    // main window DLC
    //REG_WI(REG_SC_DLC(0x04), 1, BIT(7));

}

void Hal_ACE_MWESetWin(MS_U16 u16hstart, MS_U16 u16hend, MS_U16 u16vstart, MS_U16 u16vend)
{
    SC_W2BYTEMSK(REG_SC_BK0F_07_L, u16hstart, 0xFFF);
    SC_W2BYTEMSK(REG_SC_BK0F_08_L, u16hend,   0xFFF);
    SC_W2BYTEMSK(REG_SC_BK0F_09_L, u16vstart, 0xFFF);
    SC_W2BYTEMSK(REG_SC_BK0F_0A_L, u16vend,   0xFFF);
}

void Hal_ACE_MWECloneVisualEffect()
{
    MS_U16 value ;

    //////////////////////////////////////
    /// Clone Main Window
    //////////////////////////////////////

    // copy pre Y gain control
    value = SC_R2BYTE(REG_SC_BK1A_16_L);
    SC_W2BYTEMSK(REG_SC_BK1A_16_L, (value << 8), 0xFF00);
    //copy rgb2yuv enable flag
    value = SC_R2BYTE(REG_SC_BK18_6E_L);
    SC_W2BYTEMSK(REG_SC_BK18_6E_L, value << 4, BIT(4));
    value = SC_R2BYTE(REG_SC_BK18_6F_L);
    SC_W2BYTEMSK(REG_SC_BK18_6F_L, value << 4, BIT(4) | BIT(5));

    //copy color temp
    value = SC_R2BYTE(REG_SC_BK25_21_L);
    SC_W2BYTEMSK(REG_SC_BK25_27_L, value, 0x07ff);
    value = SC_R2BYTE(REG_SC_BK25_22_L);
    SC_W2BYTEMSK(REG_SC_BK25_28_L, value, 0x07ff);
    value = SC_R2BYTE(REG_SC_BK25_23_L);
    SC_W2BYTEMSK(REG_SC_BK25_29_L, value, 0x07ff);

    value = SC_R2BYTE(REG_SC_BK25_24_L);
    SC_W2BYTEMSK(REG_SC_BK25_2A_L, value, 0x0fff);
    value = SC_R2BYTE(REG_SC_BK25_25_L);
    SC_W2BYTEMSK(REG_SC_BK25_2B_L, value, 0x0fff);
    value = SC_R2BYTE(REG_SC_BK25_26_L);
    SC_W2BYTEMSK(REG_SC_BK25_2C_L, value, 0x0fff);
    value = SC_R2BYTE(REG_SC_BK25_01_L);
    SC_W2BYTEMSK(REG_SC_BK25_11_L, value, BIT(6) | BIT(5) | BIT(4));
    // copy brightness enable flag
    value = SC_R2BYTE(REG_SC_BK10_16_L);
    SC_W2BYTEMSK(REG_SC_BK0F_18_L, value, 0xFD00);
    // copy color table enable flag
    value = SC_R2BYTE(REG_SC_BK10_2F_L);
    SC_W2BYTEMSK(REG_SC_BK0F_26_L, value, 0x0037);
    //clone ICC
    value = SC_R2BYTE(REG_SC_BK18_30_L);
    SC_W2BYTEMSK(REG_SC_BK18_30_L, value >> 4, BIT(2));
    //clone IBC
    value = SC_R2BYTE(REG_SC_BK18_40_L);
    SC_W2BYTEMSK(REG_SC_BK18_40_L, value >> 1, BIT(6));
    //clone IHC
    value = SC_R2BYTE(REG_SC_BK18_60_L);
    SC_W2BYTEMSK(REG_SC_BK18_60_L, value >> 1, BIT(6));
}

void Hal_ACE_3DClonePQMap(MS_BOOL bHWeaveOut, MS_BOOL bVWeaveOut)
{
    MS_U16 value = 0;

    if((!bHWeaveOut) && (!bVWeaveOut))
    {
        SC_W2BYTEMSK(REG_SC_BK19_10_L, 0x8F00 ,0xFF00);
        SC_W2BYTE(REG_SC_BK19_12_L, 0x0000);
        SC_W2BYTEMSK(REG_SC_BK19_60_L, 0x00, 0x00FF);
        SC_W2BYTEMSK(REG_SC_BK19_31_L, 0x00, BIT(4));
        SC_W2BYTEMSK(REG_SC_BK19_7B_L, 0x00, BIT(0));
    }

    //FCC
    {
        value = SC_R2BYTE(REG_SC_BK18_10_L);
        SC_W2BYTE(REG_SC_BK18_11_L, value);
    }

    //Y/C noise masking
    {
        value = SC_R2BYTE(REG_SC_BK18_55_L);
        SC_W2BYTEMSK(REG_SC_BK18_5D_L, value ,(BIT(0)|BIT(1)|BIT(2)|BIT(3)|BIT(4)|BIT(5)|BIT(7)|BIT(8)|BIT(9)|BIT(10)|BIT(11)|BIT(12)|BIT(13)|BIT(15)));
    }

    //YC-Coring
    {
        value = SC_R2BYTE(REG_SC_BK18_50_L);
        SC_W2BYTE(REG_SC_BK18_58_L, value );
        value = SC_R2BYTE(REG_SC_BK18_51_L);
        SC_W2BYTE(REG_SC_BK18_59_L, value);
        value = SC_R2BYTE(REG_SC_BK18_52_L);
        SC_W2BYTE(REG_SC_BK18_5A_L, value);
        value = SC_R2BYTE(REG_SC_BK18_53_L);
        SC_W2BYTE(REG_SC_BK18_5B_L, value);
        value = SC_R2BYTE(REG_SC_BK18_54_L);
        SC_W2BYTE(REG_SC_BK18_5C_L, value);
    }

    //Y-Adjust
    {
        value = SC_R2BYTE(REG_SC_BK1A_0E_L);
        SC_W2BYTEMSK(REG_SC_BK1A_0E_L, (value << 8), BIT(8) | BIT(9));
        value = SC_R2BYTE(REG_SC_BK1A_0F_L);
        SC_W2BYTEMSK(REG_SC_BK1A_0F_L, (value << 8), 0xFF00);
    }

    //Histogram data
    {
        value = SC_R2BYTE(REG_SC_BK1A_01_L);
        SC_W2BYTE(REG_SC_BK1A_03_L, value );
    }

    //DLC table
    {
        value = SC_R2BYTE(REG_SC_BK1A_30_L);
        SC_W2BYTE(REG_SC_BK1A_38_L, value);
        value = SC_R2BYTE(REG_SC_BK1A_31_L);
        SC_W2BYTE(REG_SC_BK1A_39_L, value );
        value = SC_R2BYTE(REG_SC_BK1A_32_L);
        SC_W2BYTE(REG_SC_BK1A_3A_L, value );
        value = SC_R2BYTE(REG_SC_BK1A_33_L);
        SC_W2BYTE(REG_SC_BK1A_3B_L, value );
        value = SC_R2BYTE(REG_SC_BK1A_34_L);
        SC_W2BYTE(REG_SC_BK1A_3C_L, value );
        value = SC_R2BYTE(REG_SC_BK1A_35_L);
        SC_W2BYTE(REG_SC_BK1A_3D_L, value);
        value = SC_R2BYTE(REG_SC_BK1A_36_L);
        SC_W2BYTE(REG_SC_BK1A_3E_L, value );
        value = SC_R2BYTE(REG_SC_BK1A_37_L);
        SC_W2BYTE(REG_SC_BK1A_3F_L, value);
    }

    //CTI
    {
        value = SC_R2BYTE(REG_SC_BK23_60_L);
        SC_W2BYTE(REG_SC_BK23_70_L, value );

        value = SC_R2BYTE(REG_SC_BK23_61_L);
        SC_W2BYTE(REG_SC_BK23_71_L, value );

        value = SC_R2BYTE(REG_SC_BK23_62_L);
        SC_W2BYTE(REG_SC_BK23_72_L, value );

        value = SC_R2BYTE(REG_SC_BK23_63_L);
        SC_W2BYTE(REG_SC_BK23_73_L, value );

        value = SC_R2BYTE(REG_SC_BK23_64_L);
        SC_W2BYTE(REG_SC_BK23_74_L, value );

        value = SC_R2BYTE(REG_SC_BK23_65_L);
        SC_W2BYTE(REG_SC_BK23_75_L, value );

        value = SC_R2BYTE(REG_SC_BK23_66_L);
        SC_W2BYTE(REG_SC_BK23_76_L, value );

        value = SC_R2BYTE(REG_SC_BK23_67_L);
        SC_W2BYTE(REG_SC_BK23_77_L, value );
    }


    //rgb2yuv enable flag
    {
        value = SC_R2BYTE(REG_SC_BK18_6E_L);
        SC_W2BYTEMSK(REG_SC_BK18_6E_L, value << 4, BIT(4));
        value = SC_R2BYTE(REG_SC_BK18_6F_L);
        SC_W2BYTEMSK(REG_SC_BK18_6F_L, value << 4, BIT(4) | BIT(5));
    }

    //color matrix
    {
        value = SC_R2BYTE(REG_SC_BK10_26_L);
        SC_W2BYTE(REG_SC_BK0F_1D_L, value );

        value = SC_R2BYTE(REG_SC_BK10_27_L);
        SC_W2BYTE(REG_SC_BK0F_1E_L, value );

        value = SC_R2BYTE(REG_SC_BK10_28_L);
        SC_W2BYTE(REG_SC_BK0F_1F_L, value );

        value = SC_R2BYTE(REG_SC_BK10_29_L);
        SC_W2BYTE(REG_SC_BK0F_20_L, value );

        value = SC_R2BYTE(REG_SC_BK10_2A_L);
        SC_W2BYTE(REG_SC_BK0F_21_L, value );

        value = SC_R2BYTE(REG_SC_BK10_2B_L);
        SC_W2BYTE(REG_SC_BK0F_22_L,value);

        value = SC_R2BYTE(REG_SC_BK10_2C_L);
        SC_W2BYTE(REG_SC_BK0F_23_L, value );

        value = SC_R2BYTE(REG_SC_BK10_2D_L);
        SC_W2BYTE(REG_SC_BK0F_24_L,value );

        value = SC_R2BYTE(REG_SC_BK10_2E_L);
        SC_W2BYTE(REG_SC_BK0F_25_L,value );

        value = SC_R2BYTE(REG_SC_BK10_2F_L);
        SC_W2BYTE(REG_SC_BK0F_26_L,value);
    }

    //DNR
    {
        value = SC_R2BYTE(REG_SC_BK06_21_L);
        SC_W2BYTE(REG_SC_BK06_01_L,value);

        value = SC_R2BYTE(REG_SC_BK06_26_L);
        SC_W2BYTEMSK(REG_SC_BK06_06_L, value, 0x00FF);

        value = SC_R2BYTE(REG_SC_BK06_27_L);
        SC_W2BYTE(REG_SC_BK06_07_L,value);
    }

    //DBK
    {
        value = SC_R2BYTE(REG_SC_BK0C_10_L);
        SC_W2BYTEMSK(REG_SC_BK0C_10_L, value<<4, BIT(6) | BIT(5) | BIT(4));

        value = SC_R2BYTE(REG_SC_BK0C_11_L);
        SC_W2BYTEMSK(REG_SC_BK0C_11_L, value<<8, 0xFF00);

        value = SC_R2BYTE(REG_SC_BK0C_39_L);
        SC_W2BYTE(REG_SC_BK0C_3B_L,value);

        value = SC_R2BYTE(REG_SC_BK0C_3A_L);
        SC_W2BYTEMSK(REG_SC_BK0C_3C_L,value,0x00FF);

        value = SC_R2BYTE(REG_SC_BK0C_40_L);
        SC_W2BYTE(REG_SC_BK0C_43_L,value);

        value = SC_R2BYTE(REG_SC_BK0C_41_L);
        SC_W2BYTE(REG_SC_BK0C_44_L,value);

        value = SC_R2BYTE(REG_SC_BK0C_42_L);
        SC_W2BYTE(REG_SC_BK0C_46_L,value);
    }

    //SNR
    {
        value = SC_R2BYTE(REG_SC_BK0C_30_L);
        SC_W2BYTEMSK(REG_SC_BK0C_30_L, value<<4, BIT(6) | BIT(5) | BIT(4));

        value = SC_R2BYTE(REG_SC_BK0C_31_L);
        SC_W2BYTEMSK(REG_SC_BK0C_31_L, value<<8, 0x0F00);
    }

    //NMR
    {
        value = SC_R2BYTE(REG_SC_BK0C_50_L);
        SC_W2BYTEMSK(REG_SC_BK0C_50_L, value<<4, BIT(5) | BIT(4));

        value = SC_R2BYTE(REG_SC_BK0C_54_L);
        SC_W2BYTEMSK(REG_SC_BK0C_54_L, value<<8, 0xFF00);
    }

    //HSP, VSP
    {
        value = SC_R2BYTE(REG_SC_BK23_0B_L);
        SC_W2BYTE(REG_SC_BK23_2B_L,value);

        // 6 tap clone
        value = SC_R2BYTE(REG_SC_BK23_13_L);
        SC_W2BYTE(REG_SC_BK23_33_L,value);
    }

    //MemFormat
    {
        value = SC_R2BYTE(REG_SC_BK12_01_L);
        SC_W2BYTE(REG_SC_BK12_41_L, value);

        value = SC_R2BYTE(REG_SC_BK12_02_L);
        SC_W2BYTE(REG_SC_BK12_42_L, value);

        value = SC_R2BYTE(REG_SC_BK12_03_L);
        SC_W2BYTEMSK(REG_SC_BK12_43_L, value, 0x00FF);

        value = SC_R2BYTE(REG_SC_BK23_0C_L);
        SC_W2BYTEMSK(REG_SC_BK23_2C_L, value, 0x00FF);
    }

    //Peaking
    {
        value = SC_R2BYTE(REG_SC_BK19_10_L);
        SC_W2BYTE(REG_SC_BK19_14_L,value);

        value = SC_R2BYTE(REG_SC_BK19_11_L);
        SC_W2BYTE(REG_SC_BK19_15_L,value);

        value = SC_R2BYTE(REG_SC_BK19_12_L);
        SC_W2BYTE(REG_SC_BK19_16_L,value);

        value = SC_R2BYTE(REG_SC_BK19_13_L);
        SC_W2BYTE(REG_SC_BK19_17_L,value);
    }

    //Sharpness
    {
        value = SC_R2BYTE(REG_SC_BK19_18_L);
        SC_W2BYTE(REG_SC_BK19_28_L,value);

        value = SC_R2BYTE(REG_SC_BK19_19_L);
        SC_W2BYTE(REG_SC_BK19_29_L,value);

        value = SC_R2BYTE(REG_SC_BK19_1A_L);
        SC_W2BYTE(REG_SC_BK19_2A_L,value);

        value = SC_R2BYTE(REG_SC_BK19_1B_L);
        SC_W2BYTE(REG_SC_BK19_2B_L,value);

        value = SC_R2BYTE(REG_SC_BK19_1C_L);
        SC_W2BYTE(REG_SC_BK19_2C_L,value);

        value = SC_R2BYTE(REG_SC_BK19_1D_L);
        SC_W2BYTE(REG_SC_BK19_2D_L,value);

        value = SC_R2BYTE(REG_SC_BK19_1E_L);
        SC_W2BYTE(REG_SC_BK19_2E_L,value);

        value = SC_R2BYTE(REG_SC_BK19_1F_L);
        SC_W2BYTE(REG_SC_BK19_2F_L,value);
    }

    //ICC
    {
        value = SC_R2BYTE(REG_SC_BK18_30_L) ;
        SC_W2BYTEMSK(REG_SC_BK18_30_L, value >> 4, (BIT(2)|BIT(3)));
        value = SC_R2BYTE(REG_SC_BK18_31_L);
        SC_W2BYTEMSK(REG_SC_BK18_31_L, (value  << 4), (BIT(4)|BIT(5)|BIT(6)|BIT(7)|BIT(12)|BIT(13)|BIT(14)|BIT(15)));
        value = SC_R2BYTE(REG_SC_BK18_32_L);
        SC_W2BYTEMSK(REG_SC_BK18_32_L, ( value << 4), (BIT(4)|BIT(5)|BIT(6)|BIT(7)|BIT(12)|BIT(13)|BIT(14)|BIT(15)));
        value = SC_R2BYTE(REG_SC_BK18_33_L);
        SC_W2BYTEMSK(REG_SC_BK18_33_L, ( value << 4), (BIT(4)|BIT(5)|BIT(6)|BIT(7)|BIT(12)|BIT(13)|BIT(14)|BIT(15)));
        value = SC_R2BYTE(REG_SC_BK18_34_L);
        SC_W2BYTEMSK(REG_SC_BK18_34_L, (value  << 4), (BIT(4)|BIT(5)|BIT(6)|BIT(7)));
        value = SC_R2BYTE(REG_SC_BK18_35_L);
        SC_W2BYTEMSK(REG_SC_BK18_35_L, (value << 8), 0xFF00);
        value = SC_R2BYTE(REG_SC_BK27_30_L) ;
        SC_W2BYTEMSK(REG_SC_BK27_30_L, value >> 1, (BIT(6)|BIT(0)));
    }

    //IBC
    {
        value = SC_R2BYTE(REG_SC_BK18_40_L);
        SC_W2BYTEMSK(REG_SC_BK18_40_L, value >> 1, BIT(6));
        value = SC_R2BYTE(REG_SC_BK18_41_L);
        SC_W2BYTE(REG_SC_BK18_45_L, value );
        value = SC_R2BYTE(REG_SC_BK18_42_L);
        SC_W2BYTE(REG_SC_BK18_46_L, value );
        value = SC_R2BYTE(REG_SC_BK18_43_L);
        SC_W2BYTE(REG_SC_BK18_47_L, value );
        value = SC_R2BYTE(REG_SC_BK18_44_L);
        SC_W2BYTEMSK(REG_SC_BK18_48_L, value ,(BIT(0)|BIT(1)|BIT(2)|BIT(3)|BIT(4)|BIT(5)));
    }

    //IHC
    {
        value = SC_R2BYTE(REG_SC_BK18_60_L);
        SC_W2BYTEMSK(REG_SC_BK18_60_L, value >> 1, BIT(6));
        value = SC_R2BYTE(REG_SC_BK18_61_L);
        SC_W2BYTE(REG_SC_BK18_65_L, value );
        value = SC_R2BYTE(REG_SC_BK18_62_L);
        SC_W2BYTE(REG_SC_BK18_66_L, value );
        value = SC_R2BYTE(REG_SC_BK18_63_L);
        SC_W2BYTE(REG_SC_BK18_67_L, value );
        value = SC_R2BYTE(REG_SC_BK18_64_L);
        SC_W2BYTEMSK(REG_SC_BK18_68_L, value, 0x00FF);
    }

    //ACK
    {
        value = SC_R2BYTE(REG_SC_BK18_38_L);
        SC_W2BYTEMSK(REG_SC_BK18_38_L, value>>4, BIT(3) | BIT(2));
    }

    //Y offset
    {
        value = SC_R2BYTE(REG_SC_BK1A_0F_L);
        SC_W2BYTEMSK(REG_SC_BK1A_0F_L, value<<8, 0xFF00);
    }

    //Y/C gain
    {
        value = SC_R2BYTE(REG_SC_BK1A_14_L);
        SC_W2BYTE(REG_SC_BK1A_15_L,value);

        value = SC_R2BYTE(REG_SC_BK1A_16_L);
        SC_W2BYTEMSK(REG_SC_BK1A_16_L, value<<8, 0xFF00);
    }

    //DMS
    {
        value = SC_R2BYTE(REG_SC_BK26_10_L);
        SC_W2BYTE(REG_SC_BK26_14_L,value);

        value = SC_R2BYTE(REG_SC_BK26_11_L);
        SC_W2BYTE(REG_SC_BK26_15_L,value);

        value = SC_R2BYTE(REG_SC_BK26_12_L);
        SC_W2BYTE(REG_SC_BK26_16_L,value);

        value = SC_R2BYTE(REG_SC_BK26_13_L);
        SC_W2BYTE(REG_SC_BK26_17_L,value);

        value = SC_R2BYTE(REG_SC_BK26_18_L);
        SC_W2BYTE(REG_SC_BK26_1B_L,value);
    }

    //Spike NR
    {
        value = SC_R2BYTE(REG_SC_BK26_50_L);
        SC_W2BYTEMSK(REG_SC_BK26_50_L, value<<4, 0X00F0);
    }

    //Bri
    {
        value = SC_R2BYTE(REG_SC_BK10_16_L);
        SC_W2BYTEMSK(REG_SC_BK0F_18_L, value, BIT(8));

        value = SC_R2BYTE(REG_SC_BK0F_36_L);
        SC_W2BYTE(REG_SC_BK0F_39_L, value );

        value = SC_R2BYTE(REG_SC_BK0F_37_L);
        SC_W2BYTE(REG_SC_BK0F_3A_L, value );

        value = SC_R2BYTE(REG_SC_BK0F_38_L);
        SC_W2BYTE(REG_SC_BK0F_3B_L, value );
    }

    //Gamma
    {
        value = SC_R2BYTE(REG_SC_BK10_50_L);
        SC_W2BYTEMSK(REG_SC_BK0F_18_L, value, BIT(0));
    }

    //DLC
    {
        value = SC_R2BYTE(REG_SC_BK1A_04_L);
        SC_W2BYTEMSK(REG_SC_BK1A_04_L, value >> 1 , BIT(0) | BIT(6));
    }

    //BLE&WLE
    {
        // copy BLE
        value = SC_R2BYTE(REG_SC_BK1A_10_L);
        SC_W2BYTE(REG_SC_BK1A_12_L, value );
        // copy WLE
        value =  SC_R2BYTE(REG_SC_BK1A_11_L);
        SC_W2BYTE(REG_SC_BK1A_13_L, value );
    }

    //HBC
    {
        value = SC_R2BYTE(REG_SC_BK10_16_L);
        SC_W2BYTEMSK(REG_SC_BK0F_18_L, value, 0xFF00);
    }

    //Color temp
    {
        value = SC_R2BYTE(REG_SC_BK25_21_L);
        SC_W2BYTEMSK(REG_SC_BK25_27_L, value, 0x07ff);
        value = SC_R2BYTE(REG_SC_BK25_22_L);
        SC_W2BYTEMSK(REG_SC_BK25_28_L, value, 0x07ff);
        value = SC_R2BYTE(REG_SC_BK25_23_L);
        SC_W2BYTEMSK(REG_SC_BK25_29_L, value, 0x07ff);

        value = SC_R2BYTE(REG_SC_BK25_24_L);
        SC_W2BYTEMSK(REG_SC_BK25_2A_L, value, 0x0fff);
        value = SC_R2BYTE(REG_SC_BK25_25_L);
        SC_W2BYTEMSK(REG_SC_BK25_2B_L, value, 0x0fff);
        value = SC_R2BYTE(REG_SC_BK25_26_L);
        SC_W2BYTEMSK(REG_SC_BK25_2C_L, value, 0x0fff);
        value = SC_R2BYTE(REG_SC_BK25_01_L);
        SC_W2BYTEMSK(REG_SC_BK25_11_L, value, 0x00ff);
    }

    if(bHWeaveOut)
    {
        SC_W2BYTEMSK(REG_SC_BK23_54_L, 0x00 ,0x80);//disable HDSDD
        SC_W2BYTEMSK(REG_SC_BK26_50_L, 0x00 ,0xFF);//disable snr_vertical vertical c low pass and spike NR
        SC_W2BYTEMSK(REG_SC_BK0C_70_L, 0x01 ,0x01);//SPF off
        SC_W2BYTEMSK(REG_SC_BK0C_10_L, 0x00 ,0x17);//disable SPF DBK
        SC_W2BYTE(REG_SC_BK0C_30_L, 0x0000);//disable SPF SNR
        SC_W2BYTE(REG_SC_BK0C_50_L, 0x0000);//disable SPF NMR
        SC_W2BYTE(REG_SC_BK26_10_L, 0x0000);//disable SPF DMS
        SC_W2BYTEMSK(REG_SC_BK23_0B_L, 0x00 ,0xFF00);//disable vsp filter
        SC_W2BYTEMSK(REG_SC_BK23_0C_L, 0x00 ,0x0C00);//disable VSP CoRing
        SC_W2BYTEMSK(REG_SC_BK23_11_L, 0x00 ,0x80);//disable VSP DeRing
        SC_W2BYTEMSK(REG_SC_BK23_13_L, 0x00 ,0x08);//disable VSP PreVBound
        SC_W2BYTE(REG_SC_BK19_12_L, 0x0000);//disable VNMR and VLPF
        SC_W2BYTEMSK(REG_SC_BK19_7B_L, 0x00 ,0x01);//disable VNMR Y lowY
        SC_W2BYTEMSK(REG_SC_BK19_10_L, 0x00 ,0x7000);//disable vertical peaking band
        SC_W2BYTEMSK(REG_SC_BK19_31_L, 0x00 ,0x10);//disable vertical adaptive peaking band
        SC_W2BYTEMSK(REG_SC_BK19_60_L, 0x00 ,0x01);//disable Post SNR
    }
    if(bVWeaveOut)
    {
        SC_W2BYTEMSK(REG_SC_BK22_08_L, 0x00 ,0xFF00);
        // diable H scaling filter
        SC_W2BYTEMSK(REG_SC_BK23_0B_L, 0x00 ,0x00FF);
        // RGB 8bits
        SC_W2BYTEMSK(REG_SC_BK12_01_L, 0x10 ,0x00FF);
        SC_W2BYTEMSK(REG_SC_BK12_02_L, 0x00 ,0xFF00);
        // disable 422To444
        SC_W2BYTEMSK(REG_SC_BK23_0C_L, 0x40 ,0x00FF);
        // disable 444To422
        SC_W2BYTEMSK(REG_SC_BK02_0A_L, 0x00 ,0x00FF);
        // disable DNR
        SC_W2BYTEMSK(REG_SC_BK06_21_L, 0x10 ,0x00FF);
        // disable 2D peaking
        SC_W2BYTEMSK(REG_SC_BK19_10_L, 0x80 ,0x00FF);
        // disable Precti
        SC_W2BYTE(REG_SC_BK23_60_L, 0x00);
        SC_W2BYTE(REG_SC_BK23_61_L, 0x00);
        // disable Postcti
        SC_W2BYTE(REG_SC_BK27_20_L, 0x00);
        SC_W2BYTE(REG_SC_BK27_21_L, 0x00);
        SC_W2BYTE(REG_SC_BK18_50_L, 0x00);
    }
    //disable dms_v12
    SC_W2BYTEMSK(REG_SC_BK26_10_L, 0x0000 ,BIT(15));
}


void Hal_ACE_MWEEnable(MS_BOOL ben, MS_BOOL bLoadFromTable)
{
    if (ben)
    {
        if(!bLoadFromTable)
        {
            MHal_SC_SetMWEQuality();
        }
        SC_W2BYTEMSK(REG_SC_BK20_10_L,  BIT(1) | BIT(2) , BIT(1) | BIT(2) );
        SC_W2BYTEMSK(REG_SC_BK10_19_L,  0x00, BIT(5));
    }
    else
    {
        SC_W2BYTEMSK(REG_SC_BK20_10_L,  0x00, BIT(1) | BIT(2) );
        SC_W2BYTEMSK(REG_SC_BK10_19_L,  BIT(5), BIT(5));
    }
}

void Hal_ACE_MWESetBorder( MS_BOOL bScalerWin, MS_U16 u16color, MS_U16 u16_l, MS_U16 u16_r, MS_U16 u16_t, MS_U16 u16_d)
{
    if( MAIN_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC_BK10_24_L, ((u16color & 0xFF) <<8), HBMASK);
        SC_W2BYTEMSK(REG_SC_BK0F_02_L, ((u16_l & 0xFF) <<8), HBMASK);
        SC_W2BYTEMSK(REG_SC_BK0F_03_L, ((u16_r & 0xFF) <<8), HBMASK);
        SC_W2BYTEMSK(REG_SC_BK0F_04_L, ((u16_t & 0xFF) <<8), HBMASK);
        SC_W2BYTEMSK(REG_SC_BK0F_05_L, ((u16_d & 0xFF) <<8), HBMASK);
    }
    else if (SUB_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC_BK0F_17_L, u16color & 0xFF,  LBMASK);
        SC_W2BYTEMSK(REG_SC_BK0F_02_L, u16_l & 0xFF, LBMASK);
        SC_W2BYTEMSK(REG_SC_BK0F_03_L, u16_r & 0xFF, LBMASK);
        SC_W2BYTEMSK(REG_SC_BK0F_04_L, u16_t & 0xFF, LBMASK);
        SC_W2BYTEMSK(REG_SC_BK0F_05_L, u16_d & 0xFF, LBMASK);
    }
    else if( SC1_MAIN_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC1_BK10_24_L, ((u16color & 0xFF) <<8), HBMASK);
        SC_W2BYTEMSK(REG_SC1_BK0F_02_L, ((u16_l & 0xFF) <<8), HBMASK);
        SC_W2BYTEMSK(REG_SC1_BK0F_03_L, ((u16_r & 0xFF) <<8), HBMASK);
        SC_W2BYTEMSK(REG_SC1_BK0F_04_L, ((u16_t & 0xFF) <<8), HBMASK);
        SC_W2BYTEMSK(REG_SC1_BK0F_05_L, ((u16_d & 0xFF) <<8), HBMASK);
    }
    else if ( SC1_SUB_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC1_BK0F_17_L, u16color & 0xFF,  LBMASK);
        SC_W2BYTEMSK(REG_SC1_BK0F_02_L, u16_l & 0xFF, LBMASK);
        SC_W2BYTEMSK(REG_SC1_BK0F_03_L, u16_r & 0xFF, LBMASK);
        SC_W2BYTEMSK(REG_SC1_BK0F_04_L, u16_t & 0xFF, LBMASK);
        SC_W2BYTEMSK(REG_SC1_BK0F_05_L, u16_d & 0xFF, LBMASK);
    }
    else if( SC2_MAIN_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC2_BK10_24_L, ((u16color & 0xFF) <<8), HBMASK);
        SC_W2BYTEMSK(REG_SC2_BK0F_02_L, ((u16_l & 0xFF) <<8), HBMASK);
        SC_W2BYTEMSK(REG_SC2_BK0F_03_L, ((u16_r & 0xFF) <<8), HBMASK);
        SC_W2BYTEMSK(REG_SC2_BK0F_04_L, ((u16_t & 0xFF) <<8), HBMASK);
        SC_W2BYTEMSK(REG_SC2_BK0F_05_L, ((u16_d & 0xFF) <<8), HBMASK);
    }
    else // SC2_SUB_WINDOW
    {
        SC_W2BYTEMSK(REG_SC2_BK0F_17_L, u16color & 0xFF,  LBMASK);
        SC_W2BYTEMSK(REG_SC2_BK0F_02_L, u16_l & 0xFF, LBMASK);
        SC_W2BYTEMSK(REG_SC2_BK0F_03_L, u16_r & 0xFF, LBMASK);
        SC_W2BYTEMSK(REG_SC2_BK0F_04_L, u16_t & 0xFF, LBMASK);
        SC_W2BYTEMSK(REG_SC2_BK0F_05_L, u16_d & 0xFF, LBMASK);
    }

}
////////////////////////////////////////////////////////////////////////////////
//
//  MWE end
//
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
//  DynamicNR start
//
////////////////////////////////////////////////////////////////////////////////

MS_U8 Hal_ACE_DNR_GetMotion(void)
{
    MS_U8 u8motion = (MS_U8)(SC_R2BYTE(REG_SC_BK0A_1A_L) & 0x007F);
    return u8motion;
}

MS_U8 Hal_ACE_DNR_GetCoringThreshold(MS_BOOL bScalerWin)
{
    MS_U8 u8val;

    if( MAIN_WINDOW == bScalerWin )
    {
        u8val = SC_R2BYTE(REG_SC_BK19_13_L) & 0xFF;
    }
    else if ( SUB_WINDOW == bScalerWin )
    {
        u8val = SC_R2BYTE(REG_SC_BK19_17_L) & 0xFF;
    }
    else if( SC1_MAIN_WINDOW == bScalerWin )
    {
        u8val = SC_R2BYTE(REG_SC1_BK19_13_L) & 0xFF;
    }
    else if ( SC1_SUB_WINDOW == bScalerWin )
    {
        u8val = SC_R2BYTE(REG_SC1_BK19_17_L) & 0xFF;
    }
    else if( SC2_MAIN_WINDOW == bScalerWin )
    {
        u8val = SC_R2BYTE(REG_SC2_BK19_13_L) & 0xFF;
    }
    else // SC2_SUB_WINDOW
    {
        u8val = SC_R2BYTE(REG_SC2_BK19_17_L) & 0xFF;
    }


    return u8val;
}

MS_U8 Hal_ACE_DNR_GetSharpnessAdjust(MS_BOOL bScalerWin)
{
    MS_U8 u8val;

    if( MAIN_WINDOW == bScalerWin)
    {
        u8val = ((SC_R2BYTE(REG_SC_BK19_13_L) & 0xFF00)>>8);
    }
    else if ( SUB_WINDOW == bScalerWin )
    {
        u8val = ((SC_R2BYTE(REG_SC_BK19_17_L) & 0xFF00)>>8);
    }
    else if( SC1_MAIN_WINDOW == bScalerWin)
    {
        u8val = ((SC_R2BYTE(REG_SC1_BK19_13_L) & 0xFF00)>>8);
    }
    else if ( SC1_SUB_WINDOW == bScalerWin )
    {
        u8val = ((SC_R2BYTE(REG_SC1_BK19_17_L) & 0xFF00)>>8);
    }
    else if( SC2_MAIN_WINDOW == bScalerWin)
    {
        u8val = ((SC_R2BYTE(REG_SC2_BK19_13_L) & 0xFF00)>>8);
    }
    else // SC2_SUB_WINDOW
    {
        u8val = ((SC_R2BYTE(REG_SC2_BK19_17_L) & 0xFF00)>>8);
    }

    return u8val;
}

MS_U8 Hal_ACE_DNR_GetGuassin_SNR_Threshold(MS_BOOL bScalerWin)
{
    MS_U8 u8val;

    if( MAIN_WINDOW==bScalerWin)
    {
        u8val = ((SC_R2BYTE(REG_SC_BK19_60_L) & 0xFF00)>>8);
    }
    else if (SUB_WINDOW == bScalerWin)
    {
        u8val = ((SC_R2BYTE(REG_SC_BK19_61_L) & 0xFF00)>>8);
    }
    else if( SC1_MAIN_WINDOW==bScalerWin)
    {
        u8val = ((SC_R2BYTE(REG_SC1_BK19_60_L) & 0xFF00)>>8);
    }
    else if ( SC1_SUB_WINDOW == bScalerWin)
    {
        u8val = ((SC_R2BYTE(REG_SC1_BK19_61_L) & 0xFF00)>>8);
    }
    else if( SC2_MAIN_WINDOW==bScalerWin)
    {
        u8val = ((SC_R2BYTE(REG_SC2_BK19_60_L) & 0xFF00)>>8);
    }
    else // SC2_SUB_WINDOW
    {
        u8val = ((SC_R2BYTE(REG_SC2_BK19_61_L) & 0xFF00)>>8);
    }


    return u8val;
}

void Hal_ACE_DNR_SetCoringThreshold(MS_BOOL bScalerWin, MS_U16 u16val)
{
    if( MAIN_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC_BK19_13_L, u16val, 0x00FF);
    }
    else if ( SUB_WINDOW == bScalerWin )
    {
        SC_W2BYTEMSK(REG_SC_BK19_17_L, u16val, 0x00FF);
    }
    else if( SC1_MAIN_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC1_BK19_13_L, u16val, 0x00FF);
    }
    else if ( SC1_SUB_WINDOW == bScalerWin )
    {
        SC_W2BYTEMSK(REG_SC1_BK19_17_L, u16val, 0x00FF);
    }
    else if( SC2_MAIN_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC2_BK19_13_L, u16val, 0x00FF);
    }
    else // SC2_SUB_WINDOW
    {
        SC_W2BYTEMSK(REG_SC2_BK19_17_L, u16val, 0x00FF);
    }

}

void Hal_ACE_DNR_SetSharpnessAdjust(MS_BOOL bScalerWin, MS_U16 u16val)
{
    if( MAIN_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC_BK19_13_L, (u16val<<8), 0xFF00);
    }
    else if (SUB_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC_BK19_17_L, (u16val<<8), 0xFF00);
    }
    else if( SC1_MAIN_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC1_BK19_13_L, (u16val<<8), 0xFF00);
    }
    else if ( SC1_SUB_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC1_BK19_17_L, (u16val<<8), 0xFF00);
    }
    else if( SC2_MAIN_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC2_BK19_13_L, (u16val<<8), 0xFF00);
    }
    else // SC2_SUB_WINDOW
    {
        SC_W2BYTEMSK(REG_SC2_BK19_17_L, (u16val<<8), 0xFF00);
    }

}

void Hal_ACE_DNR_SetNM_V(MS_BOOL bScalerWin, MS_U16 u16val)
{
    if( MAIN_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC_BK19_12_L, u16val, 0x00FF);
    }
    else if (SUB_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC_BK19_16_L, u16val, 0x00FF);
    }
    else if( SC1_MAIN_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC1_BK19_12_L, u16val, 0x00FF);
    }
    else if ( SC1_SUB_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC1_BK19_16_L, u16val, 0x00FF);
    }
    else if( SC2_MAIN_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC2_BK19_12_L, u16val, 0x00FF);
    }
    else // SC2_SUB_WINDOW
    {
        SC_W2BYTEMSK(REG_SC2_BK19_16_L, u16val, 0x00FF);
    }


}

void Hal_ACE_DNR_SetGNR_0(MS_BOOL bScalerWin, MS_U16 u16val)
{
    if( MAIN_WINDOW == bScalerWin )
    {
        SC_W2BYTEMSK(REG_SC_BK19_60_L, u16val, 0x00FF);
    }
    else if ( SUB_WINDOW == bScalerWin )
    {
        SC_W2BYTEMSK(REG_SC_BK19_61_L, u16val, 0x00FF);
    }
    if( SC1_MAIN_WINDOW == bScalerWin )
    {
        SC_W2BYTEMSK(REG_SC1_BK19_60_L, u16val, 0x00FF);
    }
    else if ( SC1_SUB_WINDOW == bScalerWin )
    {
        SC_W2BYTEMSK(REG_SC1_BK19_61_L, u16val, 0x00FF);
    }
    if( SC2_MAIN_WINDOW == bScalerWin )
    {
        SC_W2BYTEMSK(REG_SC2_BK19_60_L, u16val, 0x00FF);
    }
    else // SC2_SUB_WINDOW
    {
        SC_W2BYTEMSK(REG_SC2_BK19_61_L, u16val, 0x00FF);
    }

}

void Hal_ACE_DNR_SetGNR_1(MS_BOOL bScalerWin, MS_U16 u16val)
{
    if( MAIN_WINDOW == bScalerWin )
    {
        SC_W2BYTEMSK(REG_SC_BK19_60_L, (u16val<<8), 0xFF00);
    }
    else if (SUB_WINDOW == bScalerWin )
    {
        SC_W2BYTEMSK(REG_SC_BK19_61_L, (u16val<<8), 0xFF00);
    }
    else if( SC1_MAIN_WINDOW == bScalerWin )
    {
        SC_W2BYTEMSK(REG_SC1_BK19_60_L, (u16val<<8), 0xFF00);
    }
    else if ( SC1_SUB_WINDOW == bScalerWin )
    {
        SC_W2BYTEMSK(REG_SC1_BK19_61_L, (u16val<<8), 0xFF00);
    }
    else if( SC2_MAIN_WINDOW == bScalerWin )
    {
        SC_W2BYTEMSK(REG_SC2_BK19_60_L, (u16val<<8), 0xFF00);
    }
    else if ( SC2_SUB_WINDOW == bScalerWin )
    {
        SC_W2BYTEMSK(REG_SC2_BK19_61_L, (u16val<<8), 0xFF00);
    }


}

void Hal_ACE_DNR_SetCP(MS_BOOL bScalerWin, MS_U16 u16val)
{
    if( MAIN_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC_BK19_30_L, u16val, 0x00C1); // BIT(0), BIT(1) are reserved.
    }
    else if (SUB_WINDOW == bScalerWin )
    {
        SC_W2BYTEMSK(REG_SC_BK19_30_L, u16val, 0x000E); // BIT(0), BIT(1) are reserved.
    }
    else if( SC1_MAIN_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC1_BK19_30_L, u16val, 0x00C1); // BIT(0), BIT(1) are reserved.
    }
    else if ( SC1_SUB_WINDOW == bScalerWin )
    {
        SC_W2BYTEMSK(REG_SC1_BK19_30_L, u16val, 0x000E); // BIT(0), BIT(1) are reserved.
    }
    else if( SC2_MAIN_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC2_BK19_30_L, u16val, 0x00C1); // BIT(0), BIT(1) are reserved.
    }
    else // SC2_SUB_WINDOW
    {
        SC_W2BYTEMSK(REG_SC2_BK19_30_L, u16val, 0x000E); // BIT(0), BIT(1) are reserved.
    }
}

void Hal_ACE_DNR_SetDP(MS_BOOL bScalerWin, MS_U16 u16val)
{
    if( MAIN_WINDOW == bScalerWin )
    {
        SC_W2BYTEMSK(REG_SC_BK19_31_L, u16val, 0x00FF);
    }
    else if (SUB_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC_BK19_31_L, (u16val<<8), 0xFF00);
    }
    else if( SC1_MAIN_WINDOW == bScalerWin )
    {
        SC_W2BYTEMSK(REG_SC1_BK19_31_L, u16val, 0x00FF);
    }
    else if ( SC1_SUB_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC1_BK19_31_L, (u16val<<8), 0xFF00);
    }
    else if( SC2_MAIN_WINDOW == bScalerWin )
    {
        SC_W2BYTEMSK(REG_SC2_BK19_31_L, u16val, 0x00FF);
    }
    else // SC2_SUB_WINDOW
    {
        SC_W2BYTEMSK(REG_SC2_BK19_31_L, (u16val<<8), 0xFF00);
    }
}

void Hal_ACE_DNR_SetNM_H_0(MS_BOOL bScalerWin, MS_U16 u16val)
{
    if( MAIN_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC_BK18_55_L, u16val, 0x00FF);
    }
    else if ( SUB_WINDOW == bScalerWin )
    {
        SC_W2BYTEMSK(REG_SC_BK18_5D_L, u16val, 0x00FF);
    }
    else if( SC1_MAIN_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC1_BK18_55_L, u16val, 0x00FF);
    }
    else if ( SC1_SUB_WINDOW == bScalerWin )
    {
        SC_W2BYTEMSK(REG_SC1_BK18_5D_L, u16val, 0x00FF);
    }
    else if( SC2_MAIN_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC2_BK18_55_L, u16val, 0x00FF);
    }
    else // SC2_SUB_WINDOW
    {
        SC_W2BYTEMSK(REG_SC2_BK18_5D_L, u16val, 0x00FF);
    }


}

void Hal_ACE_DNR_SetNM_H_1(MS_BOOL bScalerWin, MS_U16 u16val)
{
    if( MAIN_WINDOW == bScalerWin )
    {
        SC_W2BYTEMSK(REG_SC_BK18_55_L, (u16val<<8), 0xFF00);
    }
    else if (SUB_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC_BK18_5D_L, (u16val<<8), 0xFF00);
    }
    else if( SC1_MAIN_WINDOW == bScalerWin )
    {
        SC_W2BYTEMSK(REG_SC1_BK18_55_L, (u16val<<8), 0xFF00);
    }
    else if ( SC1_SUB_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC1_BK18_5D_L, (u16val<<8), 0xFF00);
    }
    else if( SC2_MAIN_WINDOW == bScalerWin )
    {
        SC_W2BYTEMSK(REG_SC2_BK18_55_L, (u16val<<8), 0xFF00);
    }
    else // SC2_SUB_WINDOW
    {
        SC_W2BYTEMSK(REG_SC2_BK18_5D_L, (u16val<<8), 0xFF00);
    }


}

void Hal_ACE_DNR_SetGray_Ground_Gain(MS_U16 u16val)
{
    SC_W2BYTEMSK(REG_SC_BK18_21_L, (u16val<<8), 0xF000);
}

void Hal_ACE_DNR_SetGray_Ground_En(MS_BOOL bScalerWin, MS_U16 u16val)
{
    if( MAIN_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC_BK18_10_L, (u16val?(BIT(7)):(0)), 0x0080);
    }
    else if (SUB_WINDOW == bScalerWin )
    {
        SC_W2BYTEMSK(REG_SC_BK18_11_L, (u16val?(BIT(7)):(0)), 0x0080);
    }
    else if( SC1_MAIN_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC1_BK18_10_L, (u16val?(BIT(7)):(0)), 0x0080);
    }
    else if ( SC1_SUB_WINDOW == bScalerWin )
    {
        SC_W2BYTEMSK(REG_SC1_BK18_11_L, (u16val?(BIT(7)):(0)), 0x0080);
    }
    else if( SC2_MAIN_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC2_BK18_10_L, (u16val?(BIT(7)):(0)), 0x0080);
    }
    else // SC2_SUB_WINDOW
    {
        SC_W2BYTEMSK(REG_SC2_BK18_11_L, (u16val?(BIT(7)):(0)), 0x0080);
    }

}

void Hal_ACE_DNR_SetSC_Coring(MS_BOOL bScalerWin, MS_U16 u16val)
{
    if( MAIN_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC_BK23_0D_L, (u16val<<8), 0xFF00); // BK23_1B (8bit address)
    }
    else if ( SUB_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC_BK23_2D_L, (u16val<<8), 0xFF00);
    }
    else if( SC1_MAIN_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC1_BK23_0D_L, (u16val<<8), 0xFF00); // BK23_1B (8bit address)
    }
    else if ( SC1_SUB_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC1_BK23_2D_L, (u16val<<8), 0xFF00);
    }
    else if( SC2_MAIN_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC2_BK23_0D_L, (u16val<<8), 0xFF00); // BK23_1B (8bit address)
    }
    else // SC2_SUB_WINDOW
    {
        SC_W2BYTEMSK(REG_SC2_BK23_2D_L, (u16val<<8), 0xFF00);
    }


}

void Hal_ACE_DNR_SetSpikeNR_0(MS_U16 u16val)
{
    SC_W2BYTEMSK(REG_SC_BK26_50_L, u16val, 0x0001);
}

void Hal_ACE_DNR_SetSpikeNR_1(MS_U16 u16val)
{
    SC_W2BYTEMSK(REG_SC_BK26_50_L, (u16val<<8), 0x0F00);  //BK26_A0 (8bit address)
    SC_W2BYTEMSK(REG_SC_BK26_53_L, (u16val>>5), 0x0007);  //BK26_A6 (8bit address)
}

void Hal_ACE_DNR_SetSNR_NM(MS_U16 u16val)
{
    SC_W2BYTEMSK(REG_SC_BK0C_54_L, u16val, 0x00FF);
}

void Hal_ACE_DNR_SetBank_Coring(MS_BOOL bScalerWin, MS_U16 u16val)
{
    if(MAIN_WINDOW == bScalerWin)
    {
        SC_W2BYTE(REG_SC_BK19_33_L, u16val);
        SC_W2BYTE(REG_SC_BK19_34_L, u16val);
    }
    else if (SUB_WINDOW==bScalerWin)
    {
        SC_W2BYTE(REG_SC_BK19_35_L, u16val);
        SC_W2BYTE(REG_SC_BK19_36_L, u16val);
    }
    else if( SC1_MAIN_WINDOW == bScalerWin)
    {
        SC_W2BYTE(REG_SC1_BK19_33_L, u16val);
        SC_W2BYTE(REG_SC1_BK19_34_L, u16val);
    }
    else if (SC1_SUB_WINDOW==bScalerWin)
    {
        SC_W2BYTE(REG_SC1_BK19_35_L, u16val);
        SC_W2BYTE(REG_SC1_BK19_36_L, u16val);
    }
    else if (SC2_MAIN_WINDOW == bScalerWin)
    {
        SC_W2BYTE(REG_SC2_BK19_33_L, u16val);
        SC_W2BYTE(REG_SC2_BK19_34_L, u16val);
    }
    else // SC2_SUB_WINDOW
    {
        SC_W2BYTE(REG_SC2_BK19_35_L, u16val);
        SC_W2BYTE(REG_SC2_BK19_36_L, u16val);
    }


}


void Hal_ACE_DNR_SetGuassin_SNR_Threshold(MS_BOOL bScalerWin, MS_U16 u16val)
{
    if( MAIN_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC_BK19_60_L, (u16val<<8), 0xFF00);
    }
    else if (SUB_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC_BK19_61_L, (u16val<<8), 0xFF00);
    }
    else if( SC1_MAIN_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC1_BK19_60_L, (u16val<<8), 0xFF00);
    }
    else if ( SC1_SUB_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC1_BK19_61_L, (u16val<<8), 0xFF00);
    }
    else if( SC2_MAIN_WINDOW == bScalerWin)
    {
        SC_W2BYTEMSK(REG_SC2_BK19_60_L, (u16val<<8), 0xFF00);
    }
    else // SC2_SUB_WINDOW
    {
        SC_W2BYTEMSK(REG_SC2_BK19_61_L, (u16val<<8), 0xFF00);
    }


}

void Hal_ACE_DNR_SetNRTbl_Y(MS_U8 u8Idx, MS_U16 u16val)
{
    SC_W2BYTE((REG_SC_BK06_40_L+u8Idx), u16val);
}

void Hal_ACE_DNR_SetNRTbl_C(MS_U8 u8Idx, MS_U16 u16val)
{
    SC_W2BYTE((REG_SC_BK06_44_L+u8Idx), u16val);
}

// Write command by Menuload
void Hal_ACE_Write_Color_Matrix_Burst( MS_BOOL bWindow, MS_U16* psMatrix )
{
    MS_U8 i,j;
    MS_U32 ucAddr;
    MS_S16 sTmp;
    //DECLARA_BANK_VARIABLE

    //BACKUP_SC_BANK
#if 0
    if( bWindow == MAIN_WINDOW )
    {
        //SET_SC_BANK(BANK_CM_MAIN);
        ucAddr = REG_SC_BK10_26_L;

    }
    else
    {
        //SET_SC_BANK(BANK_CM_SUB);
        ucAddr = REG_SC_BK0F_1D_L;

    }
#else

    switch (bWindow)
    {
        case MAIN_WINDOW:
            ucAddr = REG_SC_BK10_26_L;
            break;
        case SUB_WINDOW:
            ucAddr = REG_SC_BK0F_1D_L;
            break;

        case SC1_MAIN_WINDOW:
            ucAddr = REG_SC1_BK10_26_L;
            break;
        case SC1_SUB_WINDOW:
            ucAddr = REG_SC_BK0F_1D_L;
            break;

        case SC2_MAIN_WINDOW:
            ucAddr = REG_SC2_BK10_26_L;
            break;
        case SC2_SUB_WINDOW:
        default:
            ucAddr = REG_SC2_BK0F_1D_L;
            break;
    }

#endif

    _MLOAD_ENTRY();

    for(i=0; i!=3; i++)
    {
        for(j=0; j!=3; j++)
        {
            sTmp = psMatrix[i*3+j];
            if( sTmp >= 0 )
            {
                if( sTmp > 0xfff )
                {
                    sTmp = 0xfff;
                }

            }
            else
            {
                sTmp = sTmp * -1;
                if( sTmp > 0xfff )
                {
                    sTmp = 0xfff;
                }
				#ifdef USE_NEW_ACE_MATRIX_RULE
                    sTmp = ~sTmp +1;
				#else
                sTmp |= 0x1000;
				#endif

            }
            MApi_XC_MLoad_WriteCmd( ucAddr, sTmp , 0x1FFF);

            ucAddr += 2;
        } // for
    } // for
    MApi_XC_MLoad_Fire(TRUE);

    _MLOAD_RETURN();


}

// Write command by Menuload
MS_BOOL Hal_ACE_Is_Support_MLoad( MS_BOOL eWindow )
{
    if (MApi_XC_MLoad_GetStatus(eWindow) == E_MLOAD_ENABLED)
        return TRUE;
    else
        return FALSE;
}

//-------------------------------------------------------------------------------------------------
// Skip Wait Vsync
// @param	eWindow      \b IN: Indicates the window where the ACE function applies to.
// @param Skip wait Vsync    \b IN: Disable wait Vsync
//-------------------------------------------------------------------------------------------------
void Hal_ACE_SetSkipWaitVsync( MS_BOOL eWindow,MS_BOOL bIsSkipWaitVsyn)
{

#if 0
    if(eWindow ==  MAIN_WINDOW )
    {
        bMainWinSkipWaitVsyn= bIsSkipWaitVsyn;
    }
    else
    {
        bSubWinSkipWaitVsyn = bIsSkipWaitVsyn;
    }
#else

    if (MAX_WINDOW_NUM > eWindow)
        bWinSkipWaintSync[eWindow] = bIsSkipWaitVsyn;

#endif
}

MS_BOOL Hal_ACE_GetSkipWaitVsync( MS_BOOL bScalerWin)
{
#if 0
    if(bScalerWin == MAIN_WINDOW)
    {
        return bMainWinSkipWaitVsyn;
    }
    else
    {
        return bSubWinSkipWaitVsyn;
    }
#else

    MS_BOOL bRet = FALSE;

    if (MAX_WINDOW_NUM > bScalerWin)
        bRet = bWinSkipWaintSync[bScalerWin];

    return bRet;

#endif
}

////////////////////////////////////////////////////////////////////////////////
//
//  DynamicNR end
//
////////////////////////////////////////////////////////////////////////////////

#undef MHAL_ACE_C

