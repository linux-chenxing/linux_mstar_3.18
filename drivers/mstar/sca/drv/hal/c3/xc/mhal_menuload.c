//////////////////////////////////////////////////////////////////////////////
//
//
// Copyright (c) 2008-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (!¡±MStar Confidential Information!¡L) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////
//==============================================================================
#define MHAL_MENULOAD_C
// Common Definition
#if defined(MSOS_TYPE_LINUX_KERNEL)
#include <linux/wait.h>
#endif

#include "MsCommon.h"
#include "MsOS.h"
#include "mhal_xc_chip_config.h"

// Registers
#include "drvXC_IOPort.h"
#include "xc_hwreg_utility2.h"
#include "hwreg_sc.h"

#include "apiXC.h"
#include "mvideo_context.h"
#include "drv_sc_ip.h"

#include "mhal_sc.h"

#include "mhal_menuload.h"

MS_BOOL Hal_XC_MLoad_GetCaps(SCALER_WIN eWindow)
{
    MS_BOOL bret;
    if(eWindow == MAIN_WINDOW || eWindow == SUB_WINDOW)// || eWindow == SC1_MAIN_WINDOW)
    {
        bret = TRUE;
    }
    else
    {
        bret = FALSE;
    }
    return bret;
}

MS_U16 Hal_XC_MLoad_get_status(SCALER_WIN eWindow)
{
    MS_U32 u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK1F_02_L :
                    eWindow == SUB_WINDOW      ? REG_SC_BK1F_02_L :
                    eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK1F_02_L :
                    eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK1F_02_L :
                    eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK1F_02_L :
                                                 REG_SC_DUMMY;

    return ((SC_R2BYTE(u32Reg) & 0x8000)>>15);
}

void Hal_XC_MLoad_set_on_off(MS_BOOL bEn, SCALER_WIN eWindow)
{
    MS_U32 u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK1F_02_L :
                    eWindow == SUB_WINDOW      ? REG_SC_BK1F_02_L :
                    eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK1F_02_L :
                    eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK1F_02_L :
                    eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK1F_02_L :
                                                 REG_SC_DUMMY;

    if(bEn)
    {
        SC_W2BYTEMSK(u32Reg, 0x8000, 0x8000);
    }
    else
    {
        SC_W2BYTEMSK(u32Reg, 0x0000, 0x8000);
    }
}

void Hal_XC_MLoad_set_len(MS_U16 u16Len, SCALER_WIN eWindow)
{
    MS_U32 u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK1F_02_L :
                    eWindow == SUB_WINDOW      ? REG_SC_BK1F_02_L :
                    eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK1F_02_L :
                    eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK1F_02_L :
                    eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK1F_02_L :
                                                 REG_SC_DUMMY;

    u16Len &= 0x7FF;
    SC_W2BYTEMSK(u32Reg, u16Len, 0x7FF);
}

void Hal_XC_MLoad_set_depth(MS_U16 u16depth, SCALER_WIN eWindow)
{
    MS_U32 u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK1F_01_L :
                    eWindow == SUB_WINDOW      ? REG_SC_BK1F_01_L :
                    eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK1F_01_L :
                    eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK1F_01_L :
                    eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK1F_01_L :
                                                 REG_SC_DUMMY;

    SC_W2BYTE(u32Reg, u16depth);
}

void Hal_XC_MLoad_set_miusel(MS_U8 u8MIUSel, SCALER_WIN eWindow)
{
    MS_U32 u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK1F_10_L :
                    eWindow == SUB_WINDOW      ? REG_SC_BK1F_10_L :
                    eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK1F_10_L :
                    eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK1F_10_L :
                    eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK1F_10_L :
                                                 REG_SC_DUMMY;


    if (u8MIUSel == 0)
    {
        SC_W2BYTEMSK(u32Reg, 0x0000, 0x2000);
    }
    else
    {
        SC_W2BYTEMSK(u32Reg, 0x2000, 0x2000);
    }
}

void Hal_XC_MLoad_Set_req_th(MS_U8 u8val, SCALER_WIN eWindow)
{
    MS_U32 u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK1F_10_L :
                    eWindow == SUB_WINDOW      ? REG_SC_BK1F_10_L :
                    eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK1F_10_L :
                    eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK1F_10_L :
                    eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK1F_10_L :
                                                 REG_SC_DUMMY;

    SC_W2BYTEMSK(u32Reg, u8val, 0x00FF);
}


void Hal_XC_MLoad_set_base_addr(MS_U32 u32addr, SCALER_WIN eWindow)
{
    MS_U32 u32Reg03 = eWindow == MAIN_WINDOW     ? REG_SC_BK1F_03_L :
                      eWindow == SUB_WINDOW      ? REG_SC_BK1F_03_L :
                      eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK1F_03_L :
                      eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK1F_03_L :
                      eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK1F_03_L :
                                                   REG_SC_DUMMY;

    MS_U32 u32Reg04 = eWindow == MAIN_WINDOW     ? REG_SC_BK1F_04_L :
                      eWindow == SUB_WINDOW      ? REG_SC_BK1F_04_L :
                      eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK1F_04_L :
                      eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK1F_04_L :
                      eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK1F_04_L :
                                                   REG_SC_DUMMY;

    u32addr /= MS_MLOAD_MEM_BASE_UNIT;

    SC_W2BYTE(u32Reg03, (MS_U16)(u32addr & 0xFFFF));
    SC_W2BYTEMSK(u32Reg04, (MS_U16)((u32addr & 0x3FF0000)>>16), 0x03FF);
}

void Hal_XC_MLoad_Set_riu(MS_BOOL bEn, SCALER_WIN eWindow)
{
    MS_U32 u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK1F_10_L :
                    eWindow == SUB_WINDOW      ? REG_SC_BK1F_10_L :
                    eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK1F_10_L :
                    eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK1F_10_L :
                    eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK1F_10_L :
                                                 REG_SC_DUMMY;

    if (bEn)
    {
        SC_W2BYTEMSK(u32Reg, 0x1000, 0x1000);
    }
    else
    {
        SC_W2BYTEMSK(u32Reg, 0x0000, 0x1000);
    }
}

void Hal_XC_MLoad_set_trigger_timing(MS_U16 u16sel, SCALER_WIN eWindow)
{
    MS_U32 u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK20_19_L :
                    eWindow == SUB_WINDOW      ? REG_SC_BK20_19_L :
                    eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK20_19_L :
                    eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK20_19_L :
                    eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK20_19_L :
                                                 REG_SC_DUMMY;


    u16sel = (u16sel & 0x0003)<<12;
    SC_W2BYTEMSK(u32Reg, u16sel, 0x3000);
}

void Hal_XC_MLoad_set_opm_lock(MS_U16 u16sel, SCALER_WIN eWindow)
{
    MS_U32 u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK20_19_L :
                    eWindow == SUB_WINDOW      ? REG_SC_BK20_19_L :
                    eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK20_19_L :
                    eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK20_19_L :
                    eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK20_19_L :
                                                 REG_SC_DUMMY;

    u16sel = (u16sel & 0x0003)<<8;
    SC_W2BYTEMSK(u32Reg, u16sel, 0x0300);
}

void Hal_XC_MLoad_set_trigger_delay(MS_U16 u16delay, SCALER_WIN eWindow)
{
    MS_U32 u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK20_22_L :
                    eWindow == SUB_WINDOW      ? REG_SC_BK20_22_L :
                    eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK20_22_L :
                    eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK20_22_L :
                    eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK20_22_L :
                                                 REG_SC_DUMMY;

    SC_W2BYTEMSK(u32Reg, u16delay, 0x0FFF);
}

//___|T_________________........__|T____ VSync
//__________|T__________________         ATP(refer the size befor memory to cal the pip sub and main length)
//_________________|T___________         Disp

//Generate TRAIN_TRIG_P from delayed line of Vsync(Setting the delay line for Auto tune area)
//Generate DISP_TRIG_P from delayed line of Vsync(Setting the delay line for Display area)
void Hal_XC_MLoad_set_trig_p(MS_U16 u16train, MS_U16 u16disp, SCALER_WIN eWindow)
{
    MS_U32 u32Reg1A = eWindow == MAIN_WINDOW     ? REG_SC_BK20_1A_L :
                      eWindow == SUB_WINDOW      ? REG_SC_BK20_1A_L :
                      eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK20_1A_L :
                      eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK20_1A_L :
                      eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK20_1A_L :
                                                   REG_SC_DUMMY;

    MS_U32 u32Reg1B = eWindow == MAIN_WINDOW     ? REG_SC_BK20_1B_L :
                      eWindow == SUB_WINDOW      ? REG_SC_BK20_1B_L :
                      eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK20_1B_L :
                      eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK20_1B_L :
                      eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK20_1B_L :
                                                   REG_SC_DUMMY;

    SC_W2BYTEMSK(u32Reg1A, u16train, 0x0FFF);
    SC_W2BYTEMSK(u32Reg1B, u16disp,  0x0FFF);
}

void Hal_XC_MLoad_set_riu_cs(MS_BOOL bEn)
{
    if(bEn)
    {
        MDrv_WriteByteMask(0x100104, 0x10, 0x10);
    }
    else
    {
        MDrv_WriteByteMask(0x100104, 0x00, 0x10);
    }
}


void Hal_XC_MLoad_set_sw_dynamic_idx_en(MS_BOOL ben)
{
    ben = ben;
}

MS_BOOL Hal_XC_MLG_GetCaps(void)
{
    return TRUE;
}

MS_U16 Hal_XC_MLG_get_status(void)
{
    return ((SC_R2BYTE(REG_SC_BK1F_22_L) & 0x8000)>>15);
}

void Hal_XC_MLG_set_on_off(MS_BOOL bEn)
{
    if(bEn)
    {
        SC_W2BYTEMSK(REG_SC_BK1F_22_L, 0x8000, 0x8000);
    }
    else
    {
        SC_W2BYTEMSK(REG_SC_BK1F_22_L, 0x0000, 0x8000);
    }
}

void Hal_XC_MLG_set_len(MS_U16 u16Len)
{
    u16Len &= 0x7FF;
    SC_W2BYTEMSK(REG_SC_BK1F_22_L, u16Len, 0x7FF);
}


void Hal_XC_MLG_set_depth(MS_U16 u16depth)
{
    SC_W2BYTE(REG_SC_BK1F_21_L, u16depth);
}

void Hal_XC_MLG_set_base_addr(MS_U32 u32addr)
{
    u32addr /= MS_MLG_MEM_BASE_UNIT;
    SC_W2BYTE(REG_SC_BK1F_23_L, (MS_U16)(u32addr & 0xFFFF));
    SC_W2BYTEMSK(REG_SC_BK1F_24_L, (MS_U16)((u32addr & 0xFF0000)>>16), 0x00FF);
}

void Hal_XC_MLG_set_trigger_timing(MS_U16 u16sel)
{
    SC_W2BYTEMSK(REG_SC_BK1F_20_L, ((u16sel & 0x0003)<<14), 0xC000);
}

void Hal_XC_MLG_set_trigger_delay(MS_U16 u16delay)
{
    SC_W2BYTEMSK(REG_SC_BK1F_20_L, (u16delay & 0x0FFF), 0x0FFF);
}

void Hal_XC_MLG_set_init_addr(MS_U16 u16addr)
{
    SC_W2BYTEMSK(REG_SC_BK1F_25_L, (u16addr & 0x03FF), 0x03FF);
}

