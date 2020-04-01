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
//==============================================================================
// [mhal_sc.c]
// Date: 20081203
// Descriptions: Add a new layer for HW setting
//==============================================================================
#define  MHAL_PIP_C
#if defined(MSOS_TYPE_LINUX_KERNEL)
#include <linux/wait.h>
#endif

#include "MsCommon.h"
#include "mhal_xc_chip_config.h"
#include "drvXC_IOPort.h"
#include "apiXC.h"
#include "xc_hwreg_utility2.h"
#include "hwreg_sc.h"
#include "hwreg_sc1.h"
#include "hwreg_sc2.h"
#include "drv_sc_menuload.h"

//==============================================================================
void Hal_SC_Sub_SetDisplayWindow_burst(MS_WINDOW_TYPE *pstDispWin, SCALER_WIN eWindow)
{
    MS_U16 u16extra_req;
    MS_U32 u32Reg_BK20_11, u32Reg_BK03_02, u32Reg_BK0F_07, u32Reg_BK0F_08, u32Reg_BK0F_09, u32Reg_BK0F_0A;
    _MLOAD_ENTRY();

    if(eWindow == SUB_WINDOW)
    {
        u32Reg_BK20_11 = REG_SC_BK20_11_L;
        u32Reg_BK03_02 = REG_SC_BK03_02_L;
        u32Reg_BK0F_07 = REG_SC_BK0F_07_L;
        u32Reg_BK0F_08 = REG_SC_BK0F_08_L;
        u32Reg_BK0F_09 = REG_SC_BK0F_09_L;
        u32Reg_BK0F_0A = REG_SC_BK0F_0A_L;
    }
    else if(eWindow == SC2_SUB_WINDOW)
    {
        u32Reg_BK20_11 = REG_SC2_BK20_11_L;
        u32Reg_BK03_02 = REG_SC2_BK03_02_L;
        u32Reg_BK0F_07 = REG_SC2_BK0F_07_L;
        u32Reg_BK0F_08 = REG_SC2_BK0F_08_L;
        u32Reg_BK0F_09 = REG_SC2_BK0F_09_L;
        u32Reg_BK0F_0A = REG_SC2_BK0F_0A_L;
    }
    else
    {
        u32Reg_BK20_11 = REG_SC_DUMMY;
        u32Reg_BK03_02 = REG_SC_DUMMY;
        u32Reg_BK0F_07 = REG_SC_DUMMY;
        u32Reg_BK0F_08 = REG_SC_DUMMY;
        u32Reg_BK0F_09 = REG_SC_DUMMY;
        u32Reg_BK0F_0A = REG_SC_DUMMY;
    }

    // When Main is foreground, sub is background, extra must enable.

    u16extra_req = 0x8000 | 0x01;

    //this is moved outside this function to mdrv_sc_pip.c
    //MApi_XC_MLoad_WriteCmd(REG_SC_BK20_10_L, (TRUE<<1), BIT(1));

    // HW PIP architeucture
    // Becasue BK3_02[8] and BK20_11[15] can not enable toghter, otherwise garbage will be showed,
    // we need to use BK12_47[0] to instead. And the default value of BK3_02[8] is 1. So, we need
    // to set it to 0 in this function.
    MApi_XC_MLoad_WriteCmd(u32Reg_BK20_11, u16extra_req , 0x800F);
    MApi_XC_MLoad_WriteCmd(u32Reg_BK03_02, 0, 0x0080);


    //Display window
    MApi_XC_MLoad_WriteCmd(u32Reg_BK0F_07, pstDispWin->x, 0xFFFF);                             // Display H start
    MApi_XC_MLoad_WriteCmd(u32Reg_BK0F_08, pstDispWin->width + pstDispWin->x - 1, 0xFFFF);    // Display H end
    MApi_XC_MLoad_WriteCmd(u32Reg_BK0F_09, pstDispWin->y, 0xFFFF);                             // Display V start
    MApi_XC_MLoad_WriteCmd(u32Reg_BK0F_0A, pstDispWin->height + pstDispWin->y - 1, 0xFFFF);   // Display V end

    MApi_XC_MLoad_Fire(TRUE);
    _MLOAD_RETURN();
}

void Hal_SC_Sub_SetDisplayWindow(MS_WINDOW_TYPE *pstDispWin, SCALER_WIN eWindow)
{
    //SC_W2BYTEMSK( REG_SC_BK20_10_L, (TRUE<<1), BIT(1));
    MS_U32 u32Reg_BK0F_07, u32Reg_BK0F_08, u32Reg_BK0F_09, u32Reg_BK0F_0A;

    if(eWindow == SUB_WINDOW)
    {
        u32Reg_BK0F_07 = REG_SC_BK0F_07_L;
        u32Reg_BK0F_08 = REG_SC_BK0F_08_L;
        u32Reg_BK0F_09 = REG_SC_BK0F_09_L;
        u32Reg_BK0F_0A = REG_SC_BK0F_0A_L;
    }
    else if(eWindow == SC2_SUB_WINDOW)
    {
        u32Reg_BK0F_07 = REG_SC2_BK0F_07_L;
        u32Reg_BK0F_08 = REG_SC2_BK0F_08_L;
        u32Reg_BK0F_09 = REG_SC2_BK0F_09_L;
        u32Reg_BK0F_0A = REG_SC2_BK0F_0A_L;
    }
    else
    {
        u32Reg_BK0F_07 = REG_SC_DUMMY;
        u32Reg_BK0F_08 = REG_SC_DUMMY;
        u32Reg_BK0F_09 = REG_SC_DUMMY;
        u32Reg_BK0F_0A = REG_SC_DUMMY;
    }
    //Display window
    SC_W2BYTE(u32Reg_BK0F_07, pstDispWin->x);                             // Display H start
    SC_W2BYTE(u32Reg_BK0F_08, pstDispWin->width + pstDispWin->x - 1);    // Display H end
    SC_W2BYTE(u32Reg_BK0F_09, pstDispWin->y);                             // Display V start
    SC_W2BYTE(u32Reg_BK0F_0A, pstDispWin->height + pstDispWin->y - 1);   // Display V end
}


void Hal_SC_subwindow_disable_burst(SCALER_WIN eWindow)
{
    MS_U32 u32Reg_10, u32Reg_11;

    _MLOAD_ENTRY();

    u32Reg_10 = eWindow == SUB_WINDOW     ? REG_SC_BK20_10_L :
                eWindow == SC1_SUB_WINDOW ? REG_SC1_BK20_10_L :
                eWindow == SC2_SUB_WINDOW ? REG_SC2_BK20_10_L :
                                            REG_SC_DUMMY;

    u32Reg_11 = eWindow == SUB_WINDOW     ? REG_SC_BK20_11_L :
                eWindow == SC1_SUB_WINDOW ? REG_SC1_BK20_11_L :
                eWindow == SC2_SUB_WINDOW ? REG_SC2_BK20_11_L :
                                            REG_SC_DUMMY;

    MApi_XC_MLoad_WriteCmd( u32Reg_10, (FALSE<<1), BIT(1));
    MApi_XC_MLoad_WriteCmd(u32Reg_11, 0x0000 , 0x800F);

    MApi_XC_MLoad_Fire(TRUE);
    _MLOAD_RETURN();
}

void Hal_SC_subwindow_disable(SCALER_WIN eWindow)
{
    MS_U32 u32Reg_10, u32Reg_11;

    u32Reg_10 = eWindow == SUB_WINDOW     ? REG_SC_BK20_10_L :
                eWindow == SC1_SUB_WINDOW ? REG_SC1_BK20_10_L :
                eWindow == SC2_SUB_WINDOW ? REG_SC2_BK20_10_L :
                                            REG_SC_DUMMY;

    u32Reg_11 = eWindow == SUB_WINDOW     ? REG_SC_BK20_11_L :
                eWindow == SC1_SUB_WINDOW ? REG_SC1_BK20_11_L :
                eWindow == SC2_SUB_WINDOW ? REG_SC2_BK20_11_L :
                                            REG_SC_DUMMY;

    SC_W2BYTEMSK( u32Reg_10, (FALSE<<1), BIT(1));
    SC_W2BYTEMSK(u32Reg_11, 0x0000 , 0x800F);
}

MS_BOOL Hal_SC_Is_subwindow_enable(SCALER_WIN eWindow)
{
    MS_U32 u32Reg;

    u32Reg = eWindow == SUB_WINDOW     ? REG_SC_BK20_10_L :
             eWindow == SC1_SUB_WINDOW ? REG_SC1_BK20_10_L :
             eWindow == SC2_SUB_WINDOW ? REG_SC2_BK20_10_L :
                                         REG_SC_DUMMY;

    return SC_R2BYTEMSK( u32Reg, BIT(1));
}


void Hal_SC_set_border_format( MS_U8 u8Left, MS_U8 u8Right, MS_U8 u8Up, MS_U8 u8Down, MS_U8 u8color, SCALER_WIN eWindow )
{
    MS_U32 u32Reg_L, u32Reg_R, u32Reg_U, u32Reg_D, u32Reg_Color;
    MS_U16 u16Msk, u16Shift;

    u32Reg_L = REG_SC_BK0F_02_L;
    u32Reg_R = REG_SC_BK0F_03_L;
    u32Reg_U = REG_SC_BK0F_04_L;
    u32Reg_D = REG_SC_BK0F_05_L;
    if( eWindow == MAIN_WINDOW || eWindow == SC1_MAIN_WINDOW  || eWindow == SC2_MAIN_WINDOW)
    {
        u32Reg_Color = REG_SC_BK10_24_L;
        u16Msk = 0xFF00;
        u16Shift = 8;
    }
    else
    {
        u32Reg_Color = REG_SC_BK0F_17_L;
        u16Msk = 0x00FF;
        u16Shift = 0;
    }


    SC_W2BYTEMSK( u32Reg_L, ((MS_U16)u8Left<<u16Shift), u16Msk);
    SC_W2BYTEMSK( u32Reg_R, ((MS_U16)u8Right<<u16Shift), u16Msk);
    SC_W2BYTEMSK( u32Reg_U, ((MS_U16)u8Up<<u16Shift), u16Msk);
    SC_W2BYTEMSK( u32Reg_D, ((MS_U16)u8Down<<u16Shift), u16Msk);

    SC_W2BYTEMSK( u32Reg_Color, ((MS_U16)u8color<<u16Shift), u16Msk);
}

void Hal_SC_border_enable( MS_BOOL bEnable, SCALER_WIN eWindow )
{
    MS_U32 u32Reg;
    MS_U16 u16Msk, u16Shift;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK10_19_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK0F_01_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK10_19_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK10_19_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK0F_01_L :
                                          REG_SC_DUMMY;

    if( eWindow == MAIN_WINDOW || eWindow == SC1_MAIN_WINDOW  || eWindow == SC2_MAIN_WINDOW)
    {
        u16Msk = BIT(2);
        u16Shift = 2;
    }
    else
    {
        u16Msk = BIT(7);
        u16Shift = 7;
    }

    SC_W2BYTEMSK( u32Reg, (bEnable<<u16Shift), u16Msk);
}

void Hal_SC_set_Zorder_main_first(MS_BOOL bMainFirst, SCALER_WIN eWindow)
{
    MS_U32 u32Reg_BK20_10, u32Reg_BK19_14;

    if(eWindow == MAIN_WINDOW || eWindow == SUB_WINDOW)
    {
        u32Reg_BK20_10 = REG_SC_BK20_10_L;
        u32Reg_BK19_14 = REG_SC_BK19_14_L;
    }
    else if(eWindow == SC1_MAIN_WINDOW || eWindow == SC1_SUB_WINDOW)
    {
        u32Reg_BK20_10 = REG_SC1_BK20_10_L;
        u32Reg_BK19_14 = REG_SC1_BK19_14_L;
    }
    else if(eWindow == SC2_MAIN_WINDOW || eWindow == SC2_SUB_WINDOW)
    {
        u32Reg_BK20_10 = REG_SC2_BK20_10_L;
        u32Reg_BK19_14 = REG_SC2_BK19_14_L;
    }
    else
    {
        u32Reg_BK20_10 = REG_SC_DUMMY;
        u32Reg_BK19_14 = REG_SC_DUMMY;
    }


    SC_W2BYTEMSK( u32Reg_BK20_10, (bMainFirst << 8), BIT(8) );

    if (bMainFirst)
    {
        SC_W2BYTEMSK( u32Reg_BK19_14, BIT(3) , BIT(3) );
    }
    else
    {
        SC_W2BYTEMSK( u32Reg_BK19_14, 0x00 , BIT(3) );
    }
}

void Hal_SC_set_Zorder_main_first_burst(MS_BOOL bMainFirst, SCALER_WIN eWindow)
{
    MS_U32 u32Reg_BK20_10, u32Reg_BK19_14;

    _MLOAD_ENTRY();

    if(eWindow == MAIN_WINDOW || eWindow == SUB_WINDOW)
    {
        u32Reg_BK20_10 = REG_SC_BK20_10_L;
        u32Reg_BK19_14 = REG_SC_BK19_14_L;
    }
    else if(eWindow == SC1_MAIN_WINDOW || eWindow == SC1_SUB_WINDOW)
    {
        u32Reg_BK20_10 = REG_SC1_BK20_10_L;
        u32Reg_BK19_14 = REG_SC1_BK19_14_L;
    }
    else if(eWindow == SC2_MAIN_WINDOW || eWindow == SC2_SUB_WINDOW)
    {
        u32Reg_BK20_10 = REG_SC2_BK20_10_L;
        u32Reg_BK19_14 = REG_SC2_BK19_14_L;
    }
    else
    {
        u32Reg_BK20_10 = REG_SC_DUMMY;
        u32Reg_BK19_14 = REG_SC_DUMMY;
    }


    MApi_XC_MLoad_WriteCmd( u32Reg_BK20_10, (bMainFirst << 8), BIT(8) );

    if (bMainFirst)
        MApi_XC_MLoad_WriteCmd( u32Reg_BK19_14, BIT(3) , BIT(3) );
    else
        MApi_XC_MLoad_WriteCmd( u32Reg_BK19_14, 0x00 , BIT(3) );

    MApi_XC_MLoad_Fire(TRUE);

    _MLOAD_RETURN();
}

void Hal_SC_SetPIP_FRC(MS_BOOL enable, MS_U8 u8IgonreLinesNum)
{
    // Enable FRC for Subwindow
    SC_W2BYTEMSK(REG_SC_BK12_45_L, ( enable ? BIT(3):0 ), BIT(3));

    // Set Threadhold
    SC_W2BYTEMSK(REG_SC_BK12_45_L, u8IgonreLinesNum, BIT(2)|BIT(1)|BIT(0));
}

void Hal_SC_set_Zextra_y_half_burst(MS_BOOL bInterlace, SCALER_WIN eWindow)
{
    MS_U32 u32Reg;

    _MLOAD_ENTRY();

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK20_11_L :
                        SUB_WINDOW      ? REG_SC_BK20_11_L :
                        SC1_MAIN_WINDOW ? REG_SC2_BK20_11_L :
                        SC1_SUB_WINDOW  ? REG_SC2_BK20_11_L :
                        SC2_MAIN_WINDOW ? REG_SC2_BK20_11_L :
                        SC2_SUB_WINDOW  ? REG_SC2_BK20_11_L :
                                          REG_SC_DUMMY;

    MApi_XC_MLoad_WriteCmd(u32Reg, bInterlace ? BIT(13) : 0, BIT(13));

    MApi_XC_MLoad_Fire(TRUE);

    _MLOAD_RETURN();
}

void Hal_SC_set_Zextra_y_half(MS_BOOL bInterlace, SCALER_WIN eWindow)
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK20_11_L :
                        SUB_WINDOW      ? REG_SC_BK20_11_L :
                        SC1_MAIN_WINDOW ? REG_SC2_BK20_11_L :
                        SC1_SUB_WINDOW  ? REG_SC2_BK20_11_L :
                        SC2_MAIN_WINDOW ? REG_SC2_BK20_11_L :
                        SC2_SUB_WINDOW  ? REG_SC2_BK20_11_L :
                                          REG_SC_DUMMY;

    SC_W2BYTEMSK(u32Reg, bInterlace ? BIT(13) : 0, BIT(13));
}

#undef  MHAL_PIP_C
