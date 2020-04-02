////////////////////////////////////////////////////////////////////////////////
// $Change: 617839 $
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
#define MHAL_PQ_C


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
// Common Definition
#if defined(MSOS_TYPE_LINUX_KERNEL)
#include <linux/string.h>
#include <linux/kernel.h>
#else
#include <string.h>
#endif

#include "MsCommon.h"
#include "MsOS.h"

// Internal Definition
#include "hwreg_utility2.h"
#include "color_reg.h"
#include "color_sc1_reg.h"
#include "color_sc2_reg.h"

#include "drvXC_IOPort.h"
#include "apiXC.h"

#include "drvPQ_Define.h"
// SC0
#include "Cedric_Main.h"             // table config parameter
#include "Cedric_Sub.h"              // table config parameter

// SC1
#include "Cedric_SC1_Main.h"         // table config parameter

// SC2
#include "Cedric_SC2_Main.h"         // table config parameter
#include "Cedric_SC2_Sub.h"          // table config parameter

#include "drvPQ_Datatypes.h"
#include "mhal_pq.h"
//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#ifndef UNUSED //to avoid compile warnings...
#define UNUSED(var) (void)((var) = (var))
#endif

//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------


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
MS_U32 PQ_RIU_BASE;

// Put this function here because hwreg_utility2 only for hal.
void Hal_PQ_init_riu_base(MS_U32 u32riu_base)
{
    PQ_RIU_BASE = u32riu_base;
}


MS_U8 Hal_PQ_get_sync_flag(PQ_WIN ePQWin)
{
    MS_U16 u16val;
    MS_U8 u8SyncFlag;
    MS_U32 u32Reg;

    u32Reg = ePQWin == PQ_MAIN_WINDOW     ? REG_SC_BK01_1E_L :
             ePQWin == PQ_SUB_WINDOW      ? REG_SC_BK03_1E_L :
             #if ENABLE_MULTI_SCALER
             ePQWin == PQ_SC1_MAIN_WINDOW ? REG_SC1_BK01_1E_L :
             ePQWin == PQ_SC2_MAIN_WINDOW ? REG_SC2_BK01_1E_L :
             ePQWin == PQ_SC2_SUB_WINDOW  ? REG_SC2_BK03_1E_L :
             #endif
                                            REG_SC_DUMMY;

    u16val = MApi_XC_R2BYTE(u32Reg) & 0x00FF;

    u8SyncFlag = (MS_U8)u16val;
    return u8SyncFlag;
}


MS_U8 Hal_PQ_get_input_vsync_value(PQ_WIN ePQWin)
{
    return (Hal_PQ_get_sync_flag(ePQWin) & 0x04) ? 1:0;
}

MS_U8 Hal_PQ_get_output_vsync_value(PQ_WIN ePQWin)
{
    return (Hal_PQ_get_sync_flag(ePQWin) & 0x01) ? 1 : 0;
}

MS_U8 Hal_PQ_get_input_vsync_polarity(PQ_WIN ePQWin)
{
    MS_U32 u32Reg;

    u32Reg = ePQWin == PQ_MAIN_WINDOW     ? REG_SC_BK01_1E_L :
             ePQWin == PQ_SUB_WINDOW      ? REG_SC_BK03_1E_L :
             #if ENABLE_MULTI_SCALER
             ePQWin == PQ_SC1_MAIN_WINDOW ? REG_SC1_BK01_1E_L :
             ePQWin == PQ_SC2_MAIN_WINDOW ? REG_SC2_BK01_1E_L :
             ePQWin == PQ_SC2_SUB_WINDOW  ? REG_SC2_BK03_1E_L :
             #endif
                                            REG_SC_DUMMY;

    return (MApi_XC_R2BYTE(u32Reg) & 0x100) ? 1:0;
}

void Hal_PQ_set_memfmt_doublebuffer(PQ_WIN ePQWin, MS_BOOL bEn)
{
    MS_U32 u32Reg;

    u32Reg = ePQWin == PQ_MAIN_WINDOW     ? REG_SC_BK12_40_L :
             ePQWin == PQ_SUB_WINDOW      ? REG_SC_BK12_40_L :
             #if ENABLE_MULTI_SCALER
             ePQWin == PQ_SC1_MAIN_WINDOW ? REG_SC1_BK12_40_L :
             ePQWin == PQ_SC2_MAIN_WINDOW ? REG_SC2_BK12_40_L :
             ePQWin == PQ_SC2_SUB_WINDOW  ? REG_SC2_BK12_40_L :
             #endif
                                            REG_SC_DUMMY;

    MApi_XC_W2BYTEMSK(u32Reg, bEn ? 0x03 : 0x00, 0x0003);
}

void Hal_PQ_set_sourceidx(PQ_WIN ePQWin, MS_U16 u16Idx)
{
    MS_U32 u32Reg;

    u32Reg = ePQWin == PQ_MAIN_WINDOW     ? REG_SC_BK00_36_L :
             ePQWin == PQ_SUB_WINDOW      ? REG_SC_BK00_37_L :
             #if ENABLE_MULTI_SCALER
             ePQWin == PQ_SC1_MAIN_WINDOW ? REG_SC1_BK00_36_L :
             ePQWin == PQ_SC2_MAIN_WINDOW ? REG_SC2_BK00_36_L :
             ePQWin == PQ_SC2_SUB_WINDOW  ? REG_SC2_BK00_37_L :
             #endif
                                            REG_SC_DUMMY;

    MApi_XC_W2BYTE(u32Reg, u16Idx);
}


void Hal_PQ_set_mem_fmt(PQ_WIN ePQWin, MS_U16 u16val, MS_U16 u16Mask)
{
    MS_U32 u32Reg;

    u32Reg = ePQWin == PQ_MAIN_WINDOW     ? REG_SC_BK12_02_L :
             ePQWin == PQ_SUB_WINDOW      ? REG_SC_BK12_42_L :
             #if ENABLE_MULTI_SCALER
             ePQWin == PQ_SC1_MAIN_WINDOW ? REG_SC1_BK12_02_L :
             ePQWin == PQ_SC2_MAIN_WINDOW ? REG_SC2_BK12_02_L :
             ePQWin == PQ_SC2_SUB_WINDOW  ? REG_SC2_BK12_42_L :
             #endif
                                            REG_SC_DUMMY;

    MApi_XC_W2BYTEMSK(u32Reg, u16val, u16Mask);
}

void Hal_PQ_set_mem_fmt_en(PQ_WIN ePQWin, MS_U16 u16val, MS_U16 u16Mask)
{
    MS_U32 u32Reg;

    u32Reg = ePQWin == PQ_MAIN_WINDOW     ? REG_SC_BK12_02_L :
             ePQWin == PQ_SUB_WINDOW      ? REG_SC_BK12_42_L :
             #if ENABLE_MULTI_SCALER
             ePQWin == PQ_SC1_MAIN_WINDOW ? REG_SC1_BK12_02_L :
             ePQWin == PQ_SC2_MAIN_WINDOW ? REG_SC2_BK12_02_L :
             ePQWin == PQ_SC2_SUB_WINDOW  ? REG_SC2_BK12_42_L :
             #endif
                                            REG_SC_DUMMY;

    MApi_XC_W2BYTEMSK(u32Reg, u16val, u16Mask);
}

void Hal_PQ_set_420upSample(PQ_WIN ePQWin, MS_U16 u16value)
{
    MS_U32 u32Reg;

    u32Reg = ePQWin == PQ_MAIN_WINDOW     ? REG_SC_BK21_76_L :
             ePQWin == PQ_SUB_WINDOW      ? REG_SC_BK21_76_L :
             #if ENABLE_MULTI_SCALER
             ePQWin == PQ_SC1_MAIN_WINDOW ? REG_SC1_BK21_76_L :
             ePQWin == PQ_SC2_MAIN_WINDOW ? REG_SC2_BK21_76_L :
             ePQWin == PQ_SC2_SUB_WINDOW  ? REG_SC2_BK21_76_L :
             #endif
                                            REG_SC_DUMMY;

    MApi_XC_W2BYTE(u32Reg, u16value);
}

void Hal_PQ_set_force_y_motion(PQ_WIN ePQWin, MS_U16 u16value)
{
    MS_U32 u32Reg;
    MS_U16 u16Mask;

    u32Reg = ePQWin == PQ_MAIN_WINDOW     ? REG_SC_BK22_78_L :
             ePQWin == PQ_SUB_WINDOW      ? REG_SC_BK22_78_L :
             #if ENABLE_MULTI_SCALER
             ePQWin == PQ_SC1_MAIN_WINDOW ? REG_SC1_BK22_78_L :
             ePQWin == PQ_SC2_MAIN_WINDOW ? REG_SC2_BK22_78_L :
             ePQWin == PQ_SC2_SUB_WINDOW  ? REG_SC2_BK22_78_L :
             #endif
                                            REG_SC_DUMMY;

    u16Mask = ePQWin == PQ_MAIN_WINDOW     ? 0x00FF :
              ePQWin == PQ_SUB_WINDOW      ? 0xFF00 :
              #if ENABLE_MULTI_SCALER
              ePQWin == PQ_SC1_MAIN_WINDOW ? 0x00FF :
              ePQWin == PQ_SC2_MAIN_WINDOW ? 0x00FF :
              ePQWin == PQ_SC2_SUB_WINDOW  ? 0xFF00 :
              #endif
                                             0x0000;

    MApi_XC_W2BYTEMSK(u32Reg, u16value, u16Mask);
}

MS_U8 Hal_PQ_get_force_y_motion(PQ_WIN ePQWin)
{
    MS_U8  u8val;
    MS_U32 u32Reg;
    MS_U16 u16Mask;
    MS_U8  u8shift;

    u32Reg = ePQWin == PQ_MAIN_WINDOW     ? REG_SC_BK22_78_L :
             ePQWin == PQ_SUB_WINDOW      ? REG_SC_BK22_78_L :
             #if ENABLE_MULTI_SCALER
             ePQWin == PQ_SC1_MAIN_WINDOW ? REG_SC1_BK22_78_L :
             ePQWin == PQ_SC2_MAIN_WINDOW ? REG_SC2_BK22_78_L :
             ePQWin == PQ_SC2_SUB_WINDOW  ? REG_SC2_BK22_78_L :
             #endif
                                            REG_SC_DUMMY;

    u16Mask = ePQWin == PQ_MAIN_WINDOW     ? 0x00FF :
              ePQWin == PQ_SUB_WINDOW      ? 0xFF00 :
              #if ENABLE_MULTI_SCALER
              ePQWin == PQ_SC1_MAIN_WINDOW ? 0x00FF :
              ePQWin == PQ_SC2_MAIN_WINDOW ? 0x00FF :
              ePQWin == PQ_SC2_SUB_WINDOW  ? 0xFF00 :
              #endif
                                             0x0000;

    u8shift = ePQWin == PQ_MAIN_WINDOW     ? 0 :
              ePQWin == PQ_SUB_WINDOW      ? 8 :
              #if ENABLE_MULTI_SCALER
              ePQWin == PQ_SC1_MAIN_WINDOW ? 0 :
              ePQWin == PQ_SC2_MAIN_WINDOW ? 0 :
              ePQWin == PQ_SC2_SUB_WINDOW  ? 8 :
              #endif
                                             0;

    u8val = (MS_U8)((MApi_XC_R2BYTE(u32Reg) & u16Mask)>>u8shift);

    return u8val;
}

void Hal_PQ_set_force_c_motion(PQ_WIN ePQWin, MS_U16 u16value)
{
    MS_U32 u32Reg;
    MS_U16 u16Mask;

    u32Reg = ePQWin == PQ_MAIN_WINDOW     ? REG_SC_BK22_79_L :
             ePQWin == PQ_SUB_WINDOW      ? REG_SC_BK22_79_L :
             #if ENABLE_MULTI_SCALER
             ePQWin == PQ_SC1_MAIN_WINDOW ? REG_SC1_BK22_79_L :
             ePQWin == PQ_SC2_MAIN_WINDOW ? REG_SC2_BK22_79_L :
             ePQWin == PQ_SC2_SUB_WINDOW  ? REG_SC2_BK22_79_L :
             #endif
                                            REG_SC_DUMMY;

    u16Mask = ePQWin == PQ_MAIN_WINDOW     ? 0x00FF :
              ePQWin == PQ_SUB_WINDOW      ? 0xFF00 :
              #if ENABLE_MULTI_SCALER
              ePQWin == PQ_SC1_MAIN_WINDOW ? 0x00FF :
              ePQWin == PQ_SC2_MAIN_WINDOW ? 0x00FF :
              ePQWin == PQ_SC2_SUB_WINDOW  ? 0xFF00 :
              #endif
                                             0x0000;

    MApi_XC_W2BYTEMSK(u32Reg, u16value, u16Mask);
}

MS_U8 Hal_PQ_get_force_c_motion(PQ_WIN ePQWin)
{
    MS_U8 u8val;
    MS_U32 u32Reg;
    MS_U16 u16Mask;
    MS_U8  u8shift;

    u32Reg = ePQWin == PQ_MAIN_WINDOW     ? REG_SC_BK22_79_L :
             ePQWin == PQ_SUB_WINDOW      ? REG_SC_BK22_79_L :
             #if ENABLE_MULTI_SCALER
             ePQWin == PQ_SC1_MAIN_WINDOW ? REG_SC1_BK22_79_L :
             ePQWin == PQ_SC2_MAIN_WINDOW ? REG_SC2_BK22_79_L :
             ePQWin == PQ_SC2_SUB_WINDOW  ? REG_SC2_BK22_79_L :
             #endif
                                            REG_SC_DUMMY;

    u16Mask = ePQWin == PQ_MAIN_WINDOW     ? 0x00FF :
              ePQWin == PQ_SUB_WINDOW      ? 0xFF00 :
              #if ENABLE_MULTI_SCALER
              ePQWin == PQ_SC1_MAIN_WINDOW ? 0x00FF :
              ePQWin == PQ_SC2_MAIN_WINDOW ? 0x00FF :
              ePQWin == PQ_SC2_SUB_WINDOW  ? 0xFF00 :
              #endif
                                             0x0000;

    u8shift = ePQWin == PQ_MAIN_WINDOW     ? 0 :
              ePQWin == PQ_SUB_WINDOW      ? 8 :
              #if ENABLE_MULTI_SCALER
              ePQWin == PQ_SC1_MAIN_WINDOW ? 0 :
              ePQWin == PQ_SC2_MAIN_WINDOW ? 0 :
              ePQWin == PQ_SC2_SUB_WINDOW  ? 8 :
              #endif
                                             0;

    u8val = (MS_U8)((MApi_XC_R2BYTE(u32Reg) & u16Mask)>> u8shift);
    return u8val;
}

void Hal_PQ_set_dipf_temporal(PQ_WIN ePQWin, MS_U16 u16val)
{
    MS_U32 u32Reg;

    u32Reg = ePQWin == PQ_MAIN_WINDOW     ? REG_SC_BK22_14_L :
             ePQWin == PQ_SUB_WINDOW      ? REG_SC_BK22_44_L :
             #if ENABLE_MULTI_SCALER
             ePQWin == PQ_SC1_MAIN_WINDOW ? REG_SC1_BK22_14_L :
             ePQWin == PQ_SC2_MAIN_WINDOW ? REG_SC2_BK22_14_L :
             ePQWin == PQ_SC2_SUB_WINDOW  ? REG_SC2_BK22_44_L :
             #endif
                                            REG_SC_DUMMY;

    MApi_XC_W2BYTE(u32Reg, u16val);
}

MS_U16 Hal_PQ_get_dipf_temporal(PQ_WIN ePQWin)
{
    MS_U32 u32Reg;

    u32Reg = ePQWin == PQ_MAIN_WINDOW     ? REG_SC_BK22_14_L :
             ePQWin == PQ_SUB_WINDOW      ? REG_SC_BK22_44_L :
             #if ENABLE_MULTI_SCALER
             ePQWin == PQ_SC1_MAIN_WINDOW ? REG_SC1_BK22_14_L :
             ePQWin == PQ_SC2_MAIN_WINDOW ? REG_SC2_BK22_14_L :
             ePQWin == PQ_SC2_SUB_WINDOW  ? REG_SC2_BK22_44_L :
             #endif
                                            REG_SC_DUMMY;


    return MApi_XC_R2BYTE(u32Reg);
}

void Hal_PQ_set_dipf_spatial(PQ_WIN ePQWin, MS_U16 u16val)
{
    MS_U32 u32Reg;

    u32Reg = ePQWin == PQ_MAIN_WINDOW     ? REG_SC_BK22_15_L :
             ePQWin == PQ_SUB_WINDOW      ? REG_SC_BK22_45_L :
             #if ENABLE_MULTI_SCALER
             ePQWin == PQ_SC1_MAIN_WINDOW ? REG_SC1_BK22_15_L :
             ePQWin == PQ_SC2_MAIN_WINDOW ? REG_SC2_BK22_15_L :
             ePQWin == PQ_SC2_SUB_WINDOW  ? REG_SC2_BK22_45_L :
             #endif
                                            REG_SC_DUMMY;

    MApi_XC_W2BYTEMSK(u32Reg, u16val, 0x00FF);
}

MS_U8 Hal_PQ_get_dipf_spatial(PQ_WIN ePQWin)
{
    MS_U32 u32Reg;

    u32Reg = ePQWin == PQ_MAIN_WINDOW     ? REG_SC_BK22_15_L :
             ePQWin == PQ_SUB_WINDOW      ? REG_SC_BK22_45_L :
             #if ENABLE_MULTI_SCALER
             ePQWin == PQ_SC1_MAIN_WINDOW ? REG_SC1_BK22_15_L :
             ePQWin == PQ_SC2_MAIN_WINDOW ? REG_SC2_BK22_15_L :
             ePQWin == PQ_SC2_SUB_WINDOW  ? REG_SC2_BK22_45_L :
             #endif
                                            REG_SC_DUMMY;


    return (MS_U8)(MApi_XC_R2BYTE(u32Reg) & 0x00FF);
}

void Hal_PQ_set_vsp_sram_filter(PQ_WIN ePQWin, MS_U8 u8vale)
{
    MS_U32 u32Reg;

    u32Reg = ePQWin == PQ_MAIN_WINDOW     ? REG_SC_BK23_0B_L :
             ePQWin == PQ_SUB_WINDOW      ? REG_SC_BK23_0B_L :
             #if ENABLE_MULTI_SCALER
             ePQWin == PQ_SC1_MAIN_WINDOW ? REG_SC1_BK23_0B_L :
             ePQWin == PQ_SC2_MAIN_WINDOW ? REG_SC2_BK23_0B_L :
             ePQWin == PQ_SC2_SUB_WINDOW  ? REG_SC2_BK23_0B_L :
             #endif
                                            REG_SC_DUMMY;

    MApi_XC_W2BYTEMSK(u32Reg, ((MS_U16)u8vale)<<8, 0xFF00);
}

MS_U8 Hal_PQ_get_vsp_sram_filter(PQ_WIN ePQWin)
{
    MS_U8 u8val;
    MS_U32 u32Reg;

    u32Reg = ePQWin == PQ_MAIN_WINDOW     ? REG_SC_BK23_0B_L :
             ePQWin == PQ_SUB_WINDOW      ? REG_SC_BK23_0B_L :
             #if ENABLE_MULTI_SCALER
             ePQWin == PQ_SC1_MAIN_WINDOW ? REG_SC1_BK23_0B_L :
             ePQWin == PQ_SC2_MAIN_WINDOW ? REG_SC2_BK23_0B_L :
             ePQWin == PQ_SC2_SUB_WINDOW  ? REG_SC2_BK23_0B_L :
             #endif
                                            REG_SC_DUMMY;

    u8val = (MS_U8)((MApi_XC_R2BYTE(u32Reg) & 0xFF00) >> 8);
    return u8val;
}

void Hal_PQ_set_dnr(PQ_WIN ePQWin, MS_U8 u8val)
{
    MS_U32 u32Reg;

    u32Reg = ePQWin == PQ_MAIN_WINDOW     ? REG_SC_BK06_21_L :
             ePQWin == PQ_SUB_WINDOW      ? REG_SC_BK06_01_L :
             #if ENABLE_MULTI_SCALER
             ePQWin == PQ_SC1_MAIN_WINDOW ? REG_SC1_BK06_21_L :
             ePQWin == PQ_SC2_MAIN_WINDOW ? REG_SC2_BK06_21_L :
             ePQWin == PQ_SC2_SUB_WINDOW  ? REG_SC2_BK06_01_L :
             #endif
                                            REG_SC_DUMMY;


    MApi_XC_W2BYTEMSK(u32Reg, u8val, 0x00FF);
}

MS_U8 Hal_PQ_get_dnr(PQ_WIN ePQWin)
{
    MS_U8 u8val;
    MS_U32 u32Reg;

    u32Reg = ePQWin == PQ_MAIN_WINDOW     ? REG_SC_BK06_21_L :
             ePQWin == PQ_SUB_WINDOW      ? REG_SC_BK06_01_L :
             #if ENABLE_MULTI_SCALER
             ePQWin == PQ_SC1_MAIN_WINDOW ? REG_SC1_BK06_21_L :
             ePQWin == PQ_SC2_MAIN_WINDOW ? REG_SC2_BK06_21_L :
             ePQWin == PQ_SC2_SUB_WINDOW  ? REG_SC2_BK06_01_L :
             #endif
                                            REG_SC_DUMMY;


    u8val = (MS_U8)(MApi_XC_R2BYTE(u32Reg) & 0x00FF);
    return u8val;
}


void Hal_PQ_set_presnr(PQ_WIN ePQWin, MS_U8 u8val)
{
    MS_U32 u32Reg;

    u32Reg = ePQWin == PQ_MAIN_WINDOW     ? REG_SC_BK06_22_L :
             ePQWin == PQ_SUB_WINDOW      ? REG_SC_BK06_02_L :
             #if ENABLE_MULTI_SCALER
             ePQWin == PQ_SC1_MAIN_WINDOW ? REG_SC1_BK06_22_L :
             ePQWin == PQ_SC2_MAIN_WINDOW ? REG_SC2_BK06_22_L :
             ePQWin == PQ_SC2_SUB_WINDOW  ? REG_SC2_BK06_02_L :
             #endif
                                            REG_SC_DUMMY;

    MApi_XC_W2BYTEMSK(u32Reg, u8val, 0x00FF);
}

MS_U8 Hal_PQ_get_presnr(PQ_WIN ePQWin)
{
    MS_U8 u8val;
    MS_U32 u32Reg;

    u32Reg = ePQWin == PQ_MAIN_WINDOW     ? REG_SC_BK06_22_L :
             ePQWin == PQ_SUB_WINDOW      ? REG_SC_BK06_02_L :
             #if ENABLE_MULTI_SCALER
             ePQWin == PQ_SC1_MAIN_WINDOW ? REG_SC1_BK06_22_L :
             ePQWin == PQ_SC2_MAIN_WINDOW ? REG_SC2_BK06_22_L :
             ePQWin == PQ_SC2_SUB_WINDOW  ? REG_SC2_BK06_02_L :
             #endif
                                            REG_SC_DUMMY;


    u8val = (MS_U8)(MApi_XC_R2BYTE(u32Reg) & 0x00FF);

    return u8val;
}

void Hal_PQ_set_film(PQ_WIN ePQWin, MS_U16 u16val)
{
    MS_U32 u32Reg;
    MS_U16 u16Mask;

    u32Reg = ePQWin == PQ_MAIN_WINDOW     ? REG_SC_BK0A_10_L :
             ePQWin == PQ_SUB_WINDOW      ? REG_SC_BK0A_10_L :
             #if ENABLE_MULTI_SCALER
             ePQWin == PQ_SC1_MAIN_WINDOW ? REG_SC1_BK0A_10_L :
             ePQWin == PQ_SC2_MAIN_WINDOW ? REG_SC2_BK0A_10_L :
             ePQWin == PQ_SC2_SUB_WINDOW  ? REG_SC2_BK0A_10_L :
             #endif
                                            REG_SC_DUMMY;

    u16Mask = ePQWin == PQ_MAIN_WINDOW     ? 0xC800 :
              ePQWin == PQ_SUB_WINDOW      ? 0x0700 :
              #if ENABLE_MULTI_SCALER
              ePQWin == PQ_SC1_MAIN_WINDOW ? 0xC800 :
              ePQWin == PQ_SC2_MAIN_WINDOW ? 0xC800 :
              ePQWin == PQ_SC2_SUB_WINDOW  ? 0x0700 :
              #endif
                                             0x0000;

    MApi_XC_W2BYTEMSK(u32Reg, u16val, u16Mask);
}

MS_U8 Hal_PQ_get_film(PQ_WIN ePQWin)
{
    MS_U8 u8val;
    MS_U32 u32Reg;
    MS_U8  u8Mask;

    u32Reg = ePQWin == PQ_MAIN_WINDOW     ? REG_SC_BK0A_10_L :
             ePQWin == PQ_SUB_WINDOW      ? REG_SC_BK0A_10_L :
             #if ENABLE_MULTI_SCALER
             ePQWin == PQ_SC1_MAIN_WINDOW ? REG_SC1_BK0A_10_L :
             ePQWin == PQ_SC2_MAIN_WINDOW ? REG_SC2_BK0A_10_L :
             ePQWin == PQ_SC2_SUB_WINDOW  ? REG_SC2_BK0A_10_L :
             #endif
                                            REG_SC_DUMMY;

    u8Mask = ePQWin == PQ_MAIN_WINDOW     ? 0xC8 :
             ePQWin == PQ_SUB_WINDOW      ? 0x07 :
             #if ENABLE_MULTI_SCALER
             ePQWin == PQ_SC1_MAIN_WINDOW ? 0xC8 :
             ePQWin == PQ_SC2_MAIN_WINDOW ? 0xC8 :
             ePQWin == PQ_SC2_SUB_WINDOW  ? 0x07 :
             #endif
                                            0x00;

    u8val = (MS_U8)((MApi_XC_R2BYTE(u32Reg) & 0xFF00) >> 8);
    u8val &= u8Mask;
    return u8val;
}

void Hal_PQ_set_c_sram_table(PQ_WIN ePQWin, MS_U8 u8sramtype, void *pData)
{
    MS_U16 i, j, x;
    MS_U8 u8Ramcode[8];
    MS_U32 u32Addr;
    MS_U32 u32BK00_03, u32BK23_41, u32BK23_42, u32BK23_43, u32BK23_44, u32BK23_45;
    MS_U16 u16IdxBase;

    if(u8sramtype != SC_FILTER_C_SRAM1 &&
       u8sramtype != SC_FILTER_C_SRAM2 &&
       u8sramtype != SC_FILTER_C_SRAM3 &&
       u8sramtype != SC_FILTER_C_SRAM4)
    {
        printf("Unknown c sram type %u\r\n", u8sramtype);
        return;
    }

    if(ePQWin == PQ_MAIN_WINDOW || ePQWin == PQ_SUB_WINDOW)
    {
        u32BK00_03 = REG_SC_BK00_03_L;
        u32BK23_41 = REG_SC_BK23_41_L;
        u32BK23_42 = REG_SC_BK23_42_L;
        u32BK23_43 = REG_SC_BK23_43_L;
        u32BK23_44 = REG_SC_BK23_44_L;
        u32BK23_45 = REG_SC_BK23_45_L;
    }
#if ENABLE_MULTI_SCALER
    else if(ePQWin == PQ_SC1_MAIN_WINDOW)
    {
        u32BK00_03 = REG_SC1_BK00_03_L;
        u32BK23_41 = REG_SC1_BK23_41_L;
        u32BK23_42 = REG_SC1_BK23_42_L;
        u32BK23_43 = REG_SC1_BK23_43_L;
        u32BK23_44 = REG_SC1_BK23_44_L;
        u32BK23_45 = REG_SC1_BK23_45_L;
    }
    else if(ePQWin == PQ_SC2_MAIN_WINDOW || ePQWin == PQ_SC2_SUB_WINDOW)
    {
        u32BK00_03 = REG_SC2_BK00_03_L;
        u32BK23_41 = REG_SC2_BK23_41_L;
        u32BK23_42 = REG_SC2_BK23_42_L;
        u32BK23_43 = REG_SC2_BK23_43_L;
        u32BK23_44 = REG_SC2_BK23_44_L;
        u32BK23_45 = REG_SC2_BK23_45_L;
    }
#endif
    else
    {
        u32BK00_03 = REG_SC_DUMMY;
        u32BK23_41 = REG_SC_DUMMY;
        u32BK23_42 = REG_SC_DUMMY;
        u32BK23_43 = REG_SC_DUMMY;
        u32BK23_44 = REG_SC_DUMMY;
        u32BK23_45 = REG_SC_DUMMY;
    }
    u32Addr = (MS_U32)pData;
    if(u8sramtype == SC_FILTER_C_SRAM1)
        u16IdxBase = 0x00;
    else if(u8sramtype == SC_FILTER_C_SRAM2)
        u16IdxBase = 0x40;
    else if(u8sramtype == SC_FILTER_C_SRAM3)
        u16IdxBase = 0x80;
    else
        u16IdxBase = 0xC0;

    SC_BK_STORE_NOMUTEX;

    MApi_XC_W2BYTEMSK(u32BK00_03, 0x0000, 0x1000);
    MApi_XC_W2BYTEMSK(u32BK23_41, 0x0002, 0x0002); // enable c_sram_rw

    for(i=0; i<64; i++)
    {
        while(MApi_XC_R2BYTE(u32BK23_41) & 0x0100);
        j=i*5;

        MApi_XC_W2BYTEMSK(u32BK23_42, (i|u16IdxBase), 0x00FF);
        for ( x=0;x<5;x++ )
        {
            u8Ramcode[x] = *((MS_U8 *)(u32Addr + (j+x)));
            PQ_DUMP_FILTER_DBG(printf(" %02x ", u8Ramcode[x] ));
        }
        PQ_DUMP_FILTER_DBG(printf("\r\n"));

        MApi_XC_W2BYTEMSK(u32BK23_43, (((MS_U16)u8Ramcode[1])<<8|(MS_U16)u8Ramcode[0]), 0xFFFF);
        MApi_XC_W2BYTEMSK(u32BK23_44, (((MS_U16)u8Ramcode[3])<<8|(MS_U16)u8Ramcode[2]), 0xFFFF);
        MApi_XC_W2BYTEMSK(u32BK23_45, u8Ramcode[4], 0x00FF);

        // enable write
        MApi_XC_W2BYTEMSK(u32BK23_41, 0x0100, 0x0100);
    }

    MApi_XC_W2BYTEMSK(u32BK23_41, 0x00, 0x00FF);
    MApi_XC_W2BYTEMSK(u32BK00_03, 0x1000, 0x1000);
    SC_BK_RESTORE_NOMUTEX;

}

void Hal_PQ_set_y_sram_table(PQ_WIN ePQWin, MS_U8 u8sramtype, void *pData)
{
    MS_U16 i, j, x;
    MS_U8 u8Ramcode[10];
    MS_U32 u32Addr;
    MS_U32 u32BK00_03, u32BK23_41, u32BK23_42, u32BK23_43, u32BK23_44, u32BK23_45;
    MS_U16 u16IdxBase;

    //printf("y_sram table: ePQWin:%d , sramType:%d \r\n", ePQWin, u8sramtype);
    if(u8sramtype != SC_FILTER_Y_SRAM1 &&
       u8sramtype != SC_FILTER_Y_SRAM2 &&
       u8sramtype != SC_FILTER_Y_SRAM3 &&
       u8sramtype != SC_FILTER_Y_SRAM4)
    {
        printf("Unknown y sram type %u\r\n", u8sramtype);
        return;
    }

    if(ePQWin == PQ_MAIN_WINDOW || ePQWin == PQ_SUB_WINDOW)
    {
        u32BK00_03 = REG_SC_BK00_03_L;
        u32BK23_41 = REG_SC_BK23_41_L;
        u32BK23_42 = REG_SC_BK23_42_L;
        u32BK23_43 = REG_SC_BK23_43_L;
        u32BK23_44 = REG_SC_BK23_44_L;
        u32BK23_45 = REG_SC_BK23_45_L;
    }
#if ENABLE_MULTI_SCALER
    else if(ePQWin == PQ_SC1_MAIN_WINDOW)
    {
        u32BK00_03 = REG_SC1_BK00_03_L;
        u32BK23_41 = REG_SC1_BK23_41_L;
        u32BK23_42 = REG_SC1_BK23_42_L;
        u32BK23_43 = REG_SC1_BK23_43_L;
        u32BK23_44 = REG_SC1_BK23_44_L;
        u32BK23_45 = REG_SC1_BK23_45_L;

    }
    else if(ePQWin == PQ_SC2_MAIN_WINDOW || ePQWin == PQ_SC2_SUB_WINDOW)
    {
        u32BK00_03 = REG_SC2_BK00_03_L;
        u32BK23_41 = REG_SC2_BK23_41_L;
        u32BK23_42 = REG_SC2_BK23_42_L;
        u32BK23_43 = REG_SC2_BK23_43_L;
        u32BK23_44 = REG_SC2_BK23_44_L;
        u32BK23_45 = REG_SC2_BK23_45_L;
    }
#endif
    else
    {
        u32BK00_03 = REG_SC_DUMMY;
        u32BK23_41 = REG_SC_DUMMY;
        u32BK23_42 = REG_SC_DUMMY;
        u32BK23_43 = REG_SC_DUMMY;
        u32BK23_44 = REG_SC_DUMMY;
        u32BK23_45 = REG_SC_DUMMY;
    }

    u32Addr = (MS_U32)pData;
    SC_BK_STORE_NOMUTEX;

    if(u8sramtype == SC_FILTER_Y_SRAM1)
        u16IdxBase = 0x00;
    else if(u8sramtype == SC_FILTER_Y_SRAM2)
        u16IdxBase = 0x40;
    else if(u8sramtype == SC_FILTER_Y_SRAM3)
        u16IdxBase = 0x80;
    else
        u16IdxBase = 0xC0;

    MApi_XC_W2BYTEMSK(u32BK00_03, 0x0000, 0x1000);
    MApi_XC_W2BYTEMSK(u32BK23_41, 0x0001, 0x0001); // enable y_sram_rw
    for ( i=0; i<64; i++)
    {
        while(MApi_XC_R2BYTE(u32BK23_41) & 0x0100);
        j=i*5;
        // address

        MApi_XC_W2BYTEMSK(u32BK23_42, (i|u16IdxBase), 0x00FF);

        for ( x=0;x<5;x++ )
        {
            u8Ramcode[x] = *((MS_U8 *)(u32Addr + (j+x)));
            PQ_DUMP_FILTER_DBG(printf(" %02x ", u8Ramcode[x] ));
        }
        PQ_DUMP_FILTER_DBG(printf("\r\n"));

        MApi_XC_W2BYTEMSK(u32BK23_43, (((MS_U16)u8Ramcode[1])<<8|(MS_U16)u8Ramcode[0]), 0xFFFF);
        MApi_XC_W2BYTEMSK(u32BK23_44, (((MS_U16)u8Ramcode[3])<<8|(MS_U16)u8Ramcode[2]), 0xFFFF);
        MApi_XC_W2BYTEMSK(u32BK23_45, ((MS_U16)u8Ramcode[4]), 0x00FF);

        // enable write
        MApi_XC_W2BYTEMSK(u32BK23_41, 0x0100, 0x0100);
    }
    MApi_XC_W2BYTEMSK(u32BK23_41, 0x00, 0x00FF);
    MApi_XC_W2BYTEMSK(u32BK00_03, 0x1000, 0x1000);
    SC_BK_RESTORE_NOMUTEX;
}


void Hal_PQ_set_sram_color_index_table(PQ_WIN ePQWin, MS_U8 u8sramtype, void *pData)
{
    MS_U16 i;
    MS_U8 u8Ramcode;
    MS_U32 u32Addr;
    MS_U32 u32BK00_03, u32BK06_61, u32BK06_62, u32BK06_63;

    if(u8sramtype != SC_FILTER_SRAM_COLOR_INDEX)
    {
        return;
    }

    if(ePQWin == PQ_MAIN_WINDOW || ePQWin == PQ_SUB_WINDOW)
    {
        u32BK00_03 = REG_SC_BK00_03_L;
        u32BK06_61 = REG_SC_BK06_61_L;
        u32BK06_62 = REG_SC_BK06_62_L;
        u32BK06_63 = REG_SC_BK06_63_L;
    }
#if ENABLE_MULTI_SCALER
    else if(ePQWin == PQ_SC1_MAIN_WINDOW)
    {
        u32BK00_03 = REG_SC1_BK00_03_L;
        u32BK06_61 = REG_SC1_BK06_61_L;
        u32BK06_62 = REG_SC1_BK06_62_L;
        u32BK06_63 = REG_SC1_BK06_63_L;


    }
    else if(ePQWin == PQ_SC2_MAIN_WINDOW || ePQWin == PQ_SC2_SUB_WINDOW)
    {
        u32BK00_03 = REG_SC2_BK00_03_L;
        u32BK06_61 = REG_SC2_BK06_61_L;
        u32BK06_62 = REG_SC2_BK06_62_L;
        u32BK06_63 = REG_SC2_BK06_63_L;

    }
#endif
    else
    {
        u32BK00_03 = REG_SC_DUMMY;
        u32BK06_61 = REG_SC_DUMMY;
        u32BK06_62 = REG_SC_DUMMY;
        u32BK06_63 = REG_SC_DUMMY;
    }

    u32Addr = (MS_U32)pData;
    SC_BK_STORE_NOMUTEX;
    MApi_XC_W2BYTEMSK(u32BK00_03, 0x0000, 0x1000);

    for(i=0; i<256; i++)
    {
        while(MApi_XC_R2BYTE(u32BK06_61) & 0x0100);

        MApi_XC_W2BYTEMSK(u32BK06_63, i, 0x00FF);
        u8Ramcode = *((MS_U8 *)(u32Addr + (i)));
        PQ_DUMP_FILTER_DBG(printf(" %02x\r\n", u8Ramcode ));

        MApi_XC_W2BYTEMSK(u32BK06_62, u8Ramcode, 0x0007);
        MApi_XC_W2BYTEMSK(u32BK06_61, 0x0100, 0x0100);
    }
    MApi_XC_W2BYTEMSK(u32BK00_03, 0x1000, 0x1000);
    SC_BK_RESTORE_NOMUTEX;

}

void Hal_PQ_set_sram_color_gain_snr_table(PQ_WIN ePQWin, MS_U8 u8sramtype, void *pData)
{
    MS_U16 i;
    MS_U16 u16Ramcode;
    MS_U32 u32Addr;
    MS_U16 u16Gain_DNR;
    MS_U32 u32BK00_03, u32BK06_61, u32BK06_64, u32BK06_65, u32BK06_66;

    if(u8sramtype != SC_FILTER_SRAM_COLOR_GAIN_SNR)
    {
        return;
    }

    if(ePQWin == PQ_MAIN_WINDOW || ePQWin == PQ_SUB_WINDOW)
    {
        u32BK00_03 = REG_SC_BK00_03_L;
        u32BK06_61 = REG_SC_BK06_61_L;
        u32BK06_64 = REG_SC_BK06_64_L;
        u32BK06_65 = REG_SC_BK06_65_L;
        u32BK06_66 = REG_SC_BK06_66_L;
    }
#if ENABLE_MULTI_SCALER
    else if(ePQWin == PQ_SC1_MAIN_WINDOW)
    {
        u32BK00_03 = REG_SC1_BK00_03_L;
        u32BK06_61 = REG_SC1_BK06_61_L;
        u32BK06_64 = REG_SC1_BK06_64_L;
        u32BK06_65 = REG_SC1_BK06_65_L;
        u32BK06_66 = REG_SC1_BK06_66_L;
    }
    else if(ePQWin == PQ_SC2_MAIN_WINDOW || ePQWin == PQ_SC2_SUB_WINDOW)
    {
        u32BK00_03 = REG_SC2_BK00_03_L;
        u32BK06_61 = REG_SC2_BK06_61_L;
        u32BK06_64 = REG_SC2_BK06_64_L;
        u32BK06_65 = REG_SC2_BK06_65_L;
        u32BK06_66 = REG_SC2_BK06_66_L;
    }
#endif
    else
    {
        u32BK00_03 = REG_SC_DUMMY;
        u32BK06_61 = REG_SC_DUMMY;
        u32BK06_64 = REG_SC_DUMMY;
        u32BK06_65 = REG_SC_DUMMY;
        u32BK06_66 = REG_SC_DUMMY;
    }

    u32Addr = (MS_U32)pData;
    SC_BK_STORE_NOMUTEX;
    MApi_XC_W2BYTEMSK(u32BK00_03, 0x0000, 0x1000);

    for(i=0; i<8; i++)
    {
        while(MApi_XC_R2BYTE(u32BK06_61) & 0x0200);

        MApi_XC_W2BYTEMSK(u32BK06_66, i, 0x00FF);

        u16Gain_DNR = MApi_XC_R2BYTE(u32BK06_65) & 0x001F;
        u16Ramcode = *((MS_U8 *)(u32Addr + (i)));
        u16Ramcode = u16Ramcode<<8|u16Gain_DNR;

        PQ_DUMP_FILTER_DBG(printf(" %02x\r\n", u16Ramcode ));

        MApi_XC_W2BYTEMSK(u32BK06_64, u16Ramcode, 0x1F1F);
        MApi_XC_W2BYTEMSK(u32BK06_61, 0x0200, 0x0200);
    }
    MApi_XC_W2BYTEMSK(u32BK00_03, 0x1000, 0x1000);
    SC_BK_RESTORE_NOMUTEX;
}


void Hal_PQ_set_sram_color_gain_dnr_table(PQ_WIN ePQWin, MS_U8 u8sramtype, void *pData)
{
    MS_U16 i;
    MS_U16 u16Ramcode;
    MS_U32 u32Addr;
    MS_U16 u16Gain_SNR;
    MS_U32 u32BK00_03, u32BK06_61, u32BK06_64, u32BK06_65, u32BK06_66;

    if(u8sramtype != SC_FILTER_SRAM_COLOR_GAIN_DNR)
    {
        return;
    }

    if(ePQWin == PQ_MAIN_WINDOW || ePQWin == PQ_SUB_WINDOW)
    {
        u32BK00_03 = REG_SC_BK00_03_L;
        u32BK06_61 = REG_SC_BK06_61_L;
        u32BK06_64 = REG_SC_BK06_64_L;
        u32BK06_65 = REG_SC_BK06_65_L;
        u32BK06_66 = REG_SC_BK06_66_L;
    }
#if ENABLE_MULTI_SCALER
    else if(ePQWin == PQ_SC1_MAIN_WINDOW)
    {
        u32BK00_03 = REG_SC1_BK00_03_L;
        u32BK06_61 = REG_SC1_BK06_61_L;
        u32BK06_64 = REG_SC1_BK06_64_L;
        u32BK06_65 = REG_SC1_BK06_65_L;
        u32BK06_66 = REG_SC1_BK06_66_L;
    }
    else if(ePQWin == PQ_SC2_MAIN_WINDOW || ePQWin == PQ_SC2_SUB_WINDOW)
    {
        u32BK00_03 = REG_SC2_BK00_03_L;
        u32BK06_61 = REG_SC2_BK06_61_L;
        u32BK06_64 = REG_SC2_BK06_64_L;
        u32BK06_65 = REG_SC2_BK06_65_L;
        u32BK06_66 = REG_SC2_BK06_66_L;
    }
#endif
    else
    {
        u32BK00_03 = REG_SC_DUMMY;
        u32BK06_61 = REG_SC_DUMMY;
        u32BK06_64 = REG_SC_DUMMY;
        u32BK06_65 = REG_SC_DUMMY;
        u32BK06_66 = REG_SC_DUMMY;
    }

    u32Addr = (MS_U32)pData;
    SC_BK_STORE_NOMUTEX;
    MApi_XC_W2BYTEMSK(u32BK00_03, 0x0000, 0x1000);

    for(i=0; i<8; i++)
    {
        while(MApi_XC_R2BYTE(u32BK06_61) & 0x0200);

        MApi_XC_W2BYTEMSK(u32BK06_66, i, 0x00FF);

        u16Gain_SNR = MApi_XC_R2BYTE(u32BK06_65) & 0x1F00;
        u16Ramcode = *((MS_U8 *)(u32Addr + (i)));
        u16Ramcode = u16Gain_SNR | u16Ramcode;

        PQ_DUMP_FILTER_DBG(printf(" %02x\r\n", u16Ramcode ));

        MApi_XC_W2BYTEMSK(u32BK06_64, u16Ramcode, 0x1F1F);
        MApi_XC_W2BYTEMSK(u32BK06_61, 0x0200, 0x0200);
    }
    MApi_XC_W2BYTEMSK(u32BK00_03, 0x1000, 0x1000);
    SC_BK_RESTORE_NOMUTEX;
}

void Hal_PQ_set_sram_icc_crd_table(PQ_WIN ePQWin, MS_U8 u8sramtype, void *pData)
{
    MS_U16 i;
    MS_U16 u16Ramcode;
    MS_U32 u32Addr;
    MS_U32 u32BK18_78, u32BK18_79, u32BK18_7A;

    if(u8sramtype != SC_FILTER_SRAM_ICC_CRD)
    {
        return;
    }

    if(ePQWin == PQ_MAIN_WINDOW || ePQWin == PQ_SUB_WINDOW)
    {
        u32BK18_78 = REG_SC_BK18_78_L;
        u32BK18_79 = REG_SC_BK18_79_L;
        u32BK18_7A = REG_SC_BK18_7A_L;
    }
#if ENABLE_MULTI_SCALER
    else if(ePQWin == PQ_SC1_MAIN_WINDOW)
    {
        u32BK18_78 = REG_SC1_BK18_78_L;
        u32BK18_79 = REG_SC1_BK18_79_L;
        u32BK18_7A = REG_SC1_BK18_7A_L;
    }
    else if(ePQWin == PQ_SC2_MAIN_WINDOW || ePQWin == PQ_SC2_SUB_WINDOW)
    {
        u32BK18_78 = REG_SC2_BK18_78_L;
        u32BK18_79 = REG_SC2_BK18_79_L;
        u32BK18_7A = REG_SC2_BK18_7A_L;
    }
#endif
    else
    {
        u32BK18_78 = REG_SC_DUMMY;
        u32BK18_79 = REG_SC_DUMMY;
        u32BK18_7A = REG_SC_DUMMY;
    }

    u32Addr = (MS_U32)pData;
    SC_BK_STORE_NOMUTEX;

    MApi_XC_W2BYTEMSK(u32BK18_78, BIT(0), BIT(0)); // io_en disable

    for(i=0; i<256; i++)
    {
        u16Ramcode = *((MS_U8 *)(u32Addr + (i)));
        while (MApi_XC_R2BYTE(u32BK18_7A) & BIT(8));
        MApi_XC_W2BYTEMSK(u32BK18_79, i, 0x00FF); // address
        MApi_XC_W2BYTEMSK(u32BK18_7A, (u16Ramcode & 0xFF), 0x00FF); //data
        MApi_XC_W2BYTEMSK(u32BK18_7A, BIT(8), BIT(8)); // io_w enable

    }

    MApi_XC_W2BYTEMSK(u32BK18_78, 0, BIT(0)); // io_en enable
    SC_BK_RESTORE_NOMUTEX;
}


void _Hal_PQ_set_sram_ihc_crd_table(PQ_WIN ePQWin, MS_U8 *pBuf, MS_U8 u8SRAM_Idx, MS_U16 u16Cnt)
{
    MS_U16 i;
    MS_U32 u32BK18_7C, u32BK18_7D, u32BK18_7E;


    if(u8SRAM_Idx > 3)
    {
        u8SRAM_Idx = 0;
    }

    if(ePQWin == PQ_MAIN_WINDOW || ePQWin == PQ_SUB_WINDOW)
    {
        u32BK18_7C = REG_SC_BK18_7C_L;
        u32BK18_7D = REG_SC_BK18_7D_L;
        u32BK18_7E = REG_SC_BK18_7E_L;
    }
#if ENABLE_MULTI_SCALER
    else if(ePQWin == PQ_SC1_MAIN_WINDOW)
    {
        u32BK18_7C = REG_SC1_BK18_7C_L;
        u32BK18_7D = REG_SC1_BK18_7D_L;
        u32BK18_7E = REG_SC1_BK18_7E_L;

    }
    else if(ePQWin == PQ_SC2_MAIN_WINDOW || ePQWin == PQ_SC2_SUB_WINDOW)
    {
        u32BK18_7C = REG_SC2_BK18_7C_L;
        u32BK18_7D = REG_SC2_BK18_7D_L;
        u32BK18_7E = REG_SC2_BK18_7E_L;
    }
#endif
    else
    {
        u32BK18_7C = REG_SC_DUMMY;
        u32BK18_7D = REG_SC_DUMMY;
        u32BK18_7E = REG_SC_DUMMY;
    }

    SC_BK_STORE_NOMUTEX;

    MApi_XC_W2BYTEMSK(u32BK18_7C, BIT(0), BIT(0)); // io_en disable
    MApi_XC_W2BYTEMSK(u32BK18_7C, u8SRAM_Idx<<1, BIT(2)|BIT(1)); // sram select

    for(i=0; i<u16Cnt; i++)
    {
        while (MApi_XC_R2BYTE(u32BK18_7E) & BIT(8));

        MApi_XC_W2BYTEMSK(u32BK18_7D, i, 0x00FF); // address
        MApi_XC_W2BYTEMSK(u32BK18_7E, pBuf[i], 0x00FF); //data

        MApi_XC_W2BYTEMSK(REG_SC_BK18_7E_L, BIT(8), BIT(8)); // io_w enable
    }

    MApi_XC_W2BYTEMSK(u32BK18_7C, 0, BIT(0)); // io_en enable
    SC_BK_RESTORE_NOMUTEX;
}

void Hal_PQ_set_sram_ihc_crd_table(PQ_WIN ePQWin, MS_U8 u8sramtype, void *pData)
{
    // New method for download IHC
    #if 1

    #define H_SIZE  17
    #define V_SIZE  17
    #define MAX_SRAM_SIZE 0x100
    #define BUF_BYTE_SIZE 17*17

    #define SRAM1_IHC_COUNT 81
    #define SRAM2_IHC_COUNT 72
    #define SRAM3_IHC_COUNT 72
    #define SRAM4_IHC_COUNT 64

    #define SRAM1_IHC_OFFSET 0
    #define SRAM2_IHC_OFFSET SRAM1_IHC_COUNT
    #define SRAM3_IHC_OFFSET (SRAM2_IHC_OFFSET + SRAM2_IHC_COUNT)
    #define SRAM4_IHC_OFFSET (SRAM3_IHC_OFFSET + SRAM3_IHC_COUNT)

    #define SRAM_IHC_TOTAL_COUNT SRAM1_IHC_COUNT + SRAM2_IHC_COUNT + SRAM3_IHC_COUNT + SRAM4_IHC_COUNT

    MS_U8 SRAM1_IHC[MAX_SRAM_SIZE];
    MS_U8 SRAM2_IHC[MAX_SRAM_SIZE];
    MS_U8 SRAM3_IHC[MAX_SRAM_SIZE];
    MS_U8 SRAM4_IHC[MAX_SRAM_SIZE];

    MS_U8* u32Addr = (MS_U8*)pData;
    MS_U16 u16Index = 0 ;
    MS_U16 cont1=0, cont2=0, cont3=0, cont4=0;

    if(u8sramtype != SC_FILTER_SRAM_IHC_CRD)
    {
        return;
    }

    for (;u16Index < SRAM_IHC_TOTAL_COUNT ; u16Index++)
    {
        if (u16Index < SRAM2_IHC_OFFSET)
        {
            SRAM1_IHC[cont1++] = u32Addr[u16Index];
        }
        else if (u16Index < SRAM3_IHC_OFFSET)
        {
            SRAM2_IHC[cont2++] = u32Addr[u16Index];
        }
        else if (u16Index < SRAM4_IHC_OFFSET)
        {
            SRAM3_IHC[cont3++] = u32Addr[u16Index];
        }
        else
        {
            SRAM4_IHC[cont4++] = u32Addr[u16Index];
        }
    }

    _Hal_PQ_set_sram_ihc_crd_table(ePQWin, &SRAM1_IHC[0], 0, SRAM1_IHC_COUNT);
    _Hal_PQ_set_sram_ihc_crd_table(ePQWin, &SRAM2_IHC[0], 1, SRAM2_IHC_COUNT);
    _Hal_PQ_set_sram_ihc_crd_table(ePQWin, &SRAM3_IHC[0], 2, SRAM3_IHC_COUNT);
    _Hal_PQ_set_sram_ihc_crd_table(ePQWin, &SRAM4_IHC[0], 3, SRAM4_IHC_COUNT);

    #else

    #define H_SIZE  17
    #define V_SIZE  17
    #define MAX_SRAM_SIZE 0x100
    #define BUF_BYTE_SIZE 17*17

    MS_U8 v_idx, h_idx;
    MS_U8 data, row, col;
    MS_U16 cont1, cont2, cont3, cont4;
    MS_U8 SRAM1_IHC[MAX_SRAM_SIZE];
    MS_U8 SRAM2_IHC[MAX_SRAM_SIZE];
    MS_U8 SRAM3_IHC[MAX_SRAM_SIZE];
    MS_U8 SRAM4_IHC[MAX_SRAM_SIZE];
    MS_U32 u32Addr;

    if(u8sramtype != SC_FILTER_SRAM_IHC_CRD)
    {
        return;
    }

    u32Addr = (MS_U32)pData;
    cont1 = cont2 = cont3 = cont4 = 0;
    for (v_idx=1; v_idx <= V_SIZE; v_idx++)
    {
        for (h_idx=1; h_idx <= H_SIZE-1; h_idx++)
        {
            data = *((MS_U8 *)(u32Addr + (v_idx-1)*H_SIZE + h_idx-1)); //pBuf[v_idx-1][h_idx-1]

            row = (int)(v_idx-(v_idx/2)*2);
            col = (int)(h_idx-(h_idx/2)*2);

            if ( (row==1) & (col==1) )
            {
                SRAM1_IHC[cont1]=data;
                cont1 = cont1 < MAX_SRAM_SIZE-1 ? cont1+1 :  MAX_SRAM_SIZE-1;
            }

            if ( (row==1) & (col==0) )
            {
                SRAM2_IHC[cont2]=data;
                cont2 = cont2 < MAX_SRAM_SIZE-1 ? cont2+1 :  MAX_SRAM_SIZE-1;
            }

            if ( (row==0) & (col==1) )
            {
                SRAM3_IHC[cont3]=data;
                cont3 = cont3 < MAX_SRAM_SIZE-1 ? cont3+1 :  MAX_SRAM_SIZE-1;
            }

            if ( (row==0) & (col==0) )
            {
                SRAM4_IHC[cont4]=data;
                cont4 = cont4 < MAX_SRAM_SIZE-1 ? cont4+1 :  MAX_SRAM_SIZE-1;
            }

        }
    }

    for (v_idx=1; v_idx <= V_SIZE; v_idx++)
    {
        row = (v_idx-(v_idx/2)*2);

        data = *((MS_U8 *)(u32Addr +(v_idx-1)*H_SIZE+16));

        if (row==1)
        {
            SRAM1_IHC[cont1]=data;
            cont1 = cont1 < MAX_SRAM_SIZE-1 ? cont1+1 :  MAX_SRAM_SIZE-1;
        }
        else
        {
            SRAM3_IHC[cont3]=data;
            cont3 = cont3 < MAX_SRAM_SIZE-1 ? cont3+1 :  MAX_SRAM_SIZE-1;
        }
    }

    _Hal_PQ_set_sram_ihc_crd_table(ePQWin, &SRAM1_IHC[0], 0, cont1);
    _Hal_PQ_set_sram_ihc_crd_table(ePQWin, &SRAM2_IHC[0], 1, cont2);
    _Hal_PQ_set_sram_ihc_crd_table(ePQWin, &SRAM3_IHC[0], 2, cont3);
    _Hal_PQ_set_sram_ihc_crd_table(ePQWin, &SRAM4_IHC[0], 3, cont4);

    #endif
}



MS_U16 Hal_PQ_get_420_cup_idx(MS_420_CUP_TYPE etype)
{
    MS_U16 u16ret;

    switch(etype)
    {
    case MS_420_CUP_OFF:
        u16ret = PQ_IP_420CUP_OFF_Main;
        break;

    case MS_420_CUP_ON:
        u16ret = PQ_IP_420CUP_ON_Main;
        break;

    default:
        u16ret = 0xFFFF;
        break;
    }
    return u16ret;
}

MS_U16 Hal_PQ_get_hnonlinear_idx(MS_HNONLINEAR_TYPE etype)
{
    MS_U16 u16ret;

    switch(etype)
    {
    case MS_HNONLINEAR_1920_0:
        u16ret = PQ_IP_HnonLinear_H_1920_0_Main;
        break;

    case MS_HNONLINEAR_1920_1:
        u16ret = PQ_IP_HnonLinear_H_1920_1_Main;
        break;

    case MS_HNONLINEAR_1920_2:
        u16ret = PQ_IP_HnonLinear_H_1920_2_Main;
        break;

    case MS_HNONLINEAR_1366_0:
        u16ret = PQ_IP_HnonLinear_H_1366_0_Main;
        break;

    case MS_HNONLINEAR_1366_1:
        u16ret = PQ_IP_HnonLinear_H_1366_1_Main;
        break;

    case MS_HNONLINEAR_1366_2:
        u16ret = PQ_IP_HnonLinear_H_1366_2_Main;
        break;

    default:
        u16ret = PQ_IP_HnonLinear_OFF_Main;
        break;
    }
    return u16ret;
}

MS_U16 Hal_PQ_get_madi_idx(PQ_WIN ePQWin, MS_MADI_TYPE etype)
{
    MS_U16 u16ret;

   switch(etype)
    {
    case MS_MADI_24_4R:
        u16ret = ePQWin == PQ_MAIN_WINDOW     ? PQ_IP_MADi_24_4R_Main :
                 ePQWin == PQ_SUB_WINDOW      ? PQ_IP_MADi_24_4R_Sub :
        #if ENABLE_MULTI_SCALER
                 ePQWin == PQ_SC1_MAIN_WINDOW ? PQ_IP_MADi_24_4R_SC1_Main :
                 ePQWin == PQ_SC2_MAIN_WINDOW ? PQ_IP_MADi_24_4R_SC2_Main :
                 ePQWin == PQ_SC2_SUB_WINDOW  ? PQ_IP_MADi_24_4R_SC2_Sub :
        #endif
                                                0xFFFF;
        break;
    case MS_MADI_24_2R:
        u16ret = ePQWin == PQ_MAIN_WINDOW     ? PQ_IP_MADi_24_2R_Main :
                 ePQWin == PQ_SUB_WINDOW      ? PQ_IP_MADi_24_2R_Sub :
        #if ENABLE_MULTI_SCALER
                 ePQWin == PQ_SC1_MAIN_WINDOW ? PQ_IP_MADi_24_2R_SC1_Main :
                 ePQWin == PQ_SC2_MAIN_WINDOW ? PQ_IP_MADi_24_2R_SC2_Main :
                 ePQWin == PQ_SC2_SUB_WINDOW  ? PQ_IP_MADi_24_2R_SC2_Sub :
        #endif
                                                0xFFFF;
        break;

    case MS_MADI_25_4R:
        u16ret = ePQWin == PQ_MAIN_WINDOW     ? PQ_IP_MADi_25_4R_Main :
                 ePQWin == PQ_SUB_WINDOW      ? PQ_IP_MADi_25_4R_Sub :
        #if ENABLE_MULTI_SCALER
                 ePQWin == PQ_SC1_MAIN_WINDOW ? PQ_IP_MADi_25_4R_SC1_Main :
                 ePQWin == PQ_SC2_MAIN_WINDOW ? PQ_IP_MADi_25_4R_SC2_Main :
                 ePQWin == PQ_SC2_SUB_WINDOW  ? PQ_IP_MADi_25_4R_SC2_Sub :
        #endif
                                                0xFFFF;
        break;

    case MS_MADI_25_2R:
        u16ret = ePQWin == PQ_MAIN_WINDOW     ? PQ_IP_MADi_25_2R_Main :
                 ePQWin == PQ_SUB_WINDOW      ? PQ_IP_MADi_25_2R_Sub :
        #if ENABLE_MULTI_SCALER
                 ePQWin == PQ_SC1_MAIN_WINDOW ? PQ_IP_MADi_25_2R_SC1_Main :
                 ePQWin == PQ_SC2_MAIN_WINDOW ? PQ_IP_MADi_25_2R_SC2_Main :
                 ePQWin == PQ_SC2_SUB_WINDOW  ? PQ_IP_MADi_25_2R_SC2_Sub :
        #endif
                                                0xFFFF;
        break;

    case MS_MADI_27_4R:
        u16ret = ePQWin == PQ_MAIN_WINDOW     ? PQ_IP_MADi_27_4R_Main :
                 ePQWin == PQ_SUB_WINDOW      ? PQ_IP_MADi_27_4R_Sub :
        #if ENABLE_MULTI_SCALER
                 ePQWin == PQ_SC1_MAIN_WINDOW ? PQ_IP_MADi_27_4R_SC1_Main :
                 ePQWin == PQ_SC2_MAIN_WINDOW ? PQ_IP_MADi_27_4R_SC2_Main :
                 ePQWin == PQ_SC2_SUB_WINDOW  ? PQ_IP_MADi_27_4R_SC2_Sub :
        #endif
                                                0xFFFF;
        break;

    case MS_MADI_27_2R:
        u16ret = ePQWin == PQ_MAIN_WINDOW     ? PQ_IP_MADi_27_2R_Main :
                 ePQWin == PQ_SUB_WINDOW      ? PQ_IP_MADi_27_2R_Sub :
        #if ENABLE_MULTI_SCALER
                 ePQWin == PQ_SC1_MAIN_WINDOW ? PQ_IP_MADi_27_2R_SC1_Main :
                 ePQWin == PQ_SC2_MAIN_WINDOW ? PQ_IP_MADi_27_2R_SC2_Main :
                 ePQWin == PQ_SC2_SUB_WINDOW  ? PQ_IP_MADi_27_2R_SC2_Sub :
        #endif
                                                0xFFFF;
        break;

    case MS_MADI_P_MODE8:
        u16ret = ePQWin == PQ_MAIN_WINDOW     ? PQ_IP_MADi_P_MODE8_Main :
                 ePQWin == PQ_SUB_WINDOW      ? PQ_IP_MADi_P_MODE8_Sub :
        #if ENABLE_MULTI_SCALER
                 ePQWin == PQ_SC1_MAIN_WINDOW ? PQ_IP_MADi_P_MODE8_SC1_Main :
                 ePQWin == PQ_SC2_MAIN_WINDOW ? PQ_IP_MADi_P_MODE8_SC2_Main :
                 ePQWin == PQ_SC2_SUB_WINDOW  ? PQ_IP_MADi_P_MODE8_SC2_Sub :
        #endif
                                                0xFFFF;
        break;

    case MS_MADI_P_MODE10:
        u16ret = ePQWin == PQ_MAIN_WINDOW     ? PQ_IP_MADi_P_MODE10_Main :
                 ePQWin == PQ_SUB_WINDOW      ? PQ_IP_MADi_P_MODE10_Sub :
        #if ENABLE_MULTI_SCALER
                 ePQWin == PQ_SC1_MAIN_WINDOW ? PQ_IP_MADi_P_MODE10_SC1_Main :
                 ePQWin == PQ_SC2_MAIN_WINDOW ? PQ_IP_MADi_P_MODE10_SC2_Main :
        #endif
                 ePQWin == PQ_SC2_SUB_WINDOW  ? PQ_IP_MADi_P_MODE10_SC2_Sub :
                                                0xFFFF;
        break;

    case MS_MADI_P_MODE_MOT10:
        u16ret = ePQWin == PQ_MAIN_WINDOW     ? PQ_IP_MADi_P_MODE_MOT10_Main :
                 ePQWin == PQ_SUB_WINDOW      ? PQ_IP_MADi_P_MODE_MOT10_Sub :
        #if ENABLE_MULTI_SCALER
                 ePQWin == PQ_SC1_MAIN_WINDOW ? PQ_IP_MADi_P_MODE_MOT10_SC1_Main :
                 ePQWin == PQ_SC2_MAIN_WINDOW ? PQ_IP_MADi_P_MODE_MOT10_SC2_Main :
                 ePQWin == PQ_SC2_SUB_WINDOW  ? PQ_IP_MADi_P_MODE_MOT10_SC2_Sub :
        #endif
                                                0xFFFF;
        break;

    case MS_MADI_P_MODE_MOT8:
        u16ret = ePQWin == PQ_MAIN_WINDOW     ? PQ_IP_MADi_P_MODE_MOT8_Main :
                 ePQWin == PQ_SUB_WINDOW      ? PQ_IP_MADi_P_MODE_MOT8_Sub :
        #if ENABLE_MULTI_SCALER
                 ePQWin == PQ_SC1_MAIN_WINDOW ? PQ_IP_MADi_P_MODE_MOT8_SC1_Main :
                 ePQWin == PQ_SC2_MAIN_WINDOW ? PQ_IP_MADi_P_MODE_MOT8_SC2_Main :
                 ePQWin == PQ_SC2_SUB_WINDOW  ? PQ_IP_MADi_P_MODE_MOT8_SC2_Sub :
        #endif
                                                0xFFFF;
        break;

    case MS_MADI_P_MODE8_444:
        u16ret = ePQWin == PQ_MAIN_WINDOW     ? PQ_IP_MADi_P_MODE8_444_Main :
        #if defined(AMBER3)
                 ePQWin == PQ_SUB_WINDOW      ? PQ_IP_MADi_P_MODE8_444_Sub :
        #else
                 ePQWin == PQ_SUB_WINDOW      ? 0xFFFF :
        #endif
        #if ENABLE_MULTI_SCALER
                 ePQWin == PQ_SC1_MAIN_WINDOW ? PQ_IP_MADi_P_MODE8_444_SC1_Main :
                 ePQWin == PQ_SC2_MAIN_WINDOW ? PQ_IP_MADi_P_MODE8_444_SC2_Main :
                 ePQWin == PQ_SC2_SUB_WINDOW  ? 0xFFFF :
        #endif
                                                0xFFFF;
        break;

    case MS_MADI_P_MODE10_444:
        u16ret = ePQWin == PQ_MAIN_WINDOW     ? PQ_IP_MADi_P_MODE10_444_Main :
        #if defined(AMBER3)
                 ePQWin == PQ_SUB_WINDOW      ? PQ_IP_MADi_P_MODE10_444_Sub :
        #else
                 ePQWin == PQ_SUB_WINDOW      ? 0xFFFF :
        #endif
        #if ENABLE_MULTI_SCALER
                 ePQWin == PQ_SC1_MAIN_WINDOW ? PQ_IP_MADi_P_MODE10_444_SC1_Main :
                 ePQWin == PQ_SC2_MAIN_WINDOW ? PQ_IP_MADi_P_MODE10_444_SC2_Main :
                 ePQWin == PQ_SC2_SUB_WINDOW  ? 0xFFFF :
        #endif
                                                0xFFFF;
        break;

    default:
        u16ret = 0xFFFF;
        MS_ASSERT(0);
        break;
    }


    return u16ret;
}


MS_U16 Hal_PQ_get_sram_size(MS_U16 u16sramtype)
{
    MS_U16 u16ret;

    switch(u16sramtype)
    {
    case SC_FILTER_Y_SRAM1:
        u16ret = PQ_IP_SRAM1_SIZE_Main;
        break;

    case SC_FILTER_Y_SRAM2:
        u16ret = PQ_IP_SRAM2_SIZE_Main;
        break;

    case SC_FILTER_Y_SRAM3:
        u16ret = PQ_IP_SRAM3_SIZE_Main;
        break;

    case SC_FILTER_Y_SRAM4:
        u16ret = PQ_IP_SRAM4_SIZE_Main;
        break;

    case SC_FILTER_C_SRAM1:
        u16ret = PQ_IP_C_SRAM1_SIZE_Main;
        break;

    case SC_FILTER_C_SRAM2:
        u16ret = PQ_IP_C_SRAM2_SIZE_Main;
        break;

    case SC_FILTER_C_SRAM3:
        u16ret = PQ_IP_C_SRAM3_SIZE_Main;
        break;

    case SC_FILTER_C_SRAM4:
        u16ret = PQ_IP_C_SRAM4_SIZE_Main;
        break;

    case SC_FILTER_SRAM_COLOR_INDEX:
        u16ret = PQ_IP_SRAM_COLOR_INDEX_SIZE_Main;
        break;
#if 0
    case SC_FILTER_SRAM_COLOR_GAIN_SNR:
        u16ret = PQ_IP_SRAM_COLOR_GAIN_SNR_SIZE_Main;
        break;

    case SC_FILTER_SRAM_COLOR_GAIN_DNR:
        u16ret = PQ_IP_SRAM_COLOR_GAIN_DNR_SIZE_Main;
        break;

    case SC_FILTER_SRAM_ICC_CRD:
        u16ret = PQ_IP_VIP_ICC_CRD_SRAM_SIZE_Main;
        break;

    case SC_FILTER_SRAM_IHC_CRD:
        u16ret = PQ_IP_VIP_IHC_CRD_SRAM_SIZE_Main;
        break;
#endif

    default:
        u16ret = 0;
        break;
    }

    return u16ret;
}

MS_U16 Hal_PQ_get_ip_idx(MS_PQ_IP_TYPE eType)
{
    MS_U16 u16ret;

    switch(eType)
    {
    case MS_PQ_IP_VD_SAMPLING:
        u16ret = PQ_IP_VD_Sampling_no_comm_Main;
        break;
    case MS_PQ_IP_HSD_SAMPLING:
        u16ret = PQ_IP_HSD_Sampling_Main;
        break;
    case MS_PQ_IP_ADC_SAMPLING:
        u16ret = PQ_IP_ADC_Sampling_Main;
        break;
    default:
        u16ret = 0xFFFF;
        break;
    }

    return u16ret;
}


MS_U16 Hal_PQ_get_hsd_sampling_idx(MS_HSD_SAMPLING_TYPE eType)
{
    MS_U16 u16ret;

    switch(eType)
    {

    case MS_HSD_SAMPLING_Div_1_000:
        u16ret = PQ_IP_HSD_Sampling_Div_1o000_Main;
        break;

    case MS_HSD_SAMPLING_Div_1_125:
        u16ret = PQ_IP_HSD_Sampling_Div_1o125_Main;
        break;

    case MS_HSD_SAMPLING_Div_1_250:
        u16ret = PQ_IP_HSD_Sampling_Div_1o250_Main;
        break;

    case MS_HSD_SAMPLING_Div_1_375:
        u16ret = PQ_IP_HSD_Sampling_Div_1o375_Main;
        break;

    case MS_HSD_SAMPLING_Div_1_500:
        u16ret = PQ_IP_HSD_Sampling_Div_1o500_Main;
        break;

    case MS_HSD_SAMPLING_Div_1_625:
        u16ret = PQ_IP_HSD_Sampling_Div_1o625_Main;
        break;

    case MS_HSD_SAMPLING_Div_1_750:
        u16ret = PQ_IP_HSD_Sampling_Div_1o750_Main;
        break;

    case MS_HSD_SAMPLING_Div_1_875:
        u16ret = PQ_IP_HSD_Sampling_Div_1o875_Main;
        break;

    case MS_HSD_SAMPLING_Div_2_000:
        u16ret = PQ_IP_HSD_Sampling_Div_2o000_Main;
        break;

    default:
        u16ret = 0xFFFF;
        break;
    }

    return u16ret;
}

void Hal_PQ_OPWriteOff_Enable(MS_BOOL bEna)
{
    MApi_XC_W2BYTEMSK(REG_SC_BK12_27_L, bEna, BIT(0));
}

MS_U16 Hal_PQ_get_adc_sampling_idx(MS_ADC_SAMPLING_TYPE eType)
{
    MS_U16 u16ret;

    switch(eType)
    {
    case MS_ADC_SAMPLING_X_1:
        u16ret = PQ_IP_ADC_Sampling_x1_Main;
        break;

    case MS_ADC_SAMPLING_X_2:
        u16ret = PQ_IP_ADC_Sampling_x2_Main;
        break;
    case MS_ADC_SAMPLING_X_4:
        u16ret = PQ_IP_ADC_Sampling_x4_Main;
        break;
    default:
        u16ret = 0xFFFF;
        break;
    }

    return u16ret;
}

MS_U16 Hal_PQ_get_rw_method(PQ_WIN ePQWin)
{
    MS_U16 u16method;
    MS_U32 u32Reg;

    u32Reg = ePQWin == PQ_MAIN_WINDOW     ? REG_SC_BK01_21_L :
             ePQWin == PQ_SUB_WINDOW      ? REG_SC_BK03_21_L :
             #if ENABLE_MULTI_SCALER
             ePQWin == PQ_SC1_MAIN_WINDOW ? REG_SC1_BK01_21_L :
             ePQWin == PQ_SC2_MAIN_WINDOW ? REG_SC1_BK01_21_L :
             ePQWin == PQ_SC2_SUB_WINDOW  ? REG_SC2_BK03_21_L :
             #endif
                                            REG_SC_DUMMY;

    u16method  = (MApi_XC_R2BYTE(u32Reg) & 0xC000);

    return u16method;
}

void Hal_PQ_set_sram_xvycc_gamma_table(MS_U8 u8sramtype, void *pData)
{
#if defined(AMBER3)

    MS_U16 i;
    MS_U16 u16SRamcode;
    MS_U16 u16Count = 0 ;
    MS_U8* u8Addr = (MS_U8*) pData;

    if(u8sramtype != SC_FILTER_SRAM_XVYCC_DE_GAMMA &&
       u8sramtype != SC_FILTER_SRAM_XVYCC_GAMMA )
    {
        return;
    }
    SC_BK_STORE_NOMUTEX;

    if (u8sramtype == SC_FILTER_SRAM_XVYCC_DE_GAMMA)
    {
        u16Count = PQ_IP_XVYCC_DE_GAMMA_SRAM_SIZE_Main >> 1;
        // address port initial as 0
        MApi_XC_W2BYTE(REG_SC_BK25_76_L, 0x0 );
        // pre set value
        MApi_XC_W2BYTE(REG_SC_BK25_77_L, 0x0 );

        // RGB write together and write one time for clear write bit.
        MApi_XC_W2BYTEMSK(REG_SC_BK25_75_L, BIT(3) | BIT(5) | BIT(6) , BIT(3) | BIT(5) | BIT(6) ); // io_en disable


        for(i=0; i<u16Count; i++)
        {
            u16SRamcode = u8Addr[i*2] | u8Addr[i*2+1] << 8;

            // waiting ready.
            while (MApi_XC_R2BYTE(REG_SC_BK25_75_L) & BIT(3));

            MApi_XC_W2BYTE(REG_SC_BK25_76_L, i); // address
            MApi_XC_W2BYTE(REG_SC_BK25_77_L, u16SRamcode); //data
            MApi_XC_W2BYTEMSK(REG_SC_BK25_75_L, BIT(3), BIT(3)); // io_w enable

        }

        MApi_XC_W2BYTE(REG_SC_BK25_75_L, 0 ); // io_en enable
    }
    else
    {
        u16Count = PQ_IP_XVYCC_GAMMA_SRAM_SIZE_Main >> 1;

        // address port initial as 0
        MApi_XC_W2BYTE(REG_SC_BK25_79_L, 0x0 );
        // pre set value
        MApi_XC_W2BYTE(REG_SC_BK25_7A_L, 0x0 );

        // RGB write together and write one time for clear write bit.
        MApi_XC_W2BYTEMSK(REG_SC_BK25_78_L, BIT(3) | BIT(5) | BIT(6) , BIT(3) | BIT(5) | BIT(6) ); // io_en disable


        for(i=0; i<u16Count; i++)
        {
            u16SRamcode = u8Addr[i*2] | u8Addr[i*2+1] << 8;

            // waiting ready.
            while (MApi_XC_R2BYTE(REG_SC_BK25_78_L) & BIT(3));

            MApi_XC_W2BYTE(REG_SC_BK25_79_L, i); // address
            MApi_XC_W2BYTE(REG_SC_BK25_7A_L, u16SRamcode); //data
            MApi_XC_W2BYTEMSK(REG_SC_BK25_78_L, BIT(3), BIT(3)); // io_w enable

        }

        MApi_XC_W2BYTE(REG_SC_BK25_78_L, 0 ); // io_en enable
    }
    SC_BK_RESTORE_NOMUTEX;

#endif
}

void Hal_PQ_set_rw_method(PQ_WIN ePQWin, MS_U16 u16method)
{
    MS_U32 u32Reg;

    u32Reg = ePQWin == PQ_MAIN_WINDOW     ? REG_SC_BK01_21_L :
             ePQWin == PQ_SUB_WINDOW      ? REG_SC_BK03_21_L :
             #if ENABLE_MULTI_SCALER
             ePQWin == PQ_SC1_MAIN_WINDOW ? REG_SC1_BK01_21_L :
             ePQWin == PQ_SC2_MAIN_WINDOW ? REG_SC1_BK01_21_L :
             ePQWin == PQ_SC2_SUB_WINDOW  ? REG_SC2_BK03_21_L :
             #endif
                                            REG_SC_DUMMY;
    MApi_XC_W2BYTEMSK(u32Reg, u16method, 0xC000);
}

MS_BOOL HAL_PQ_user_mode_control(void)
{
	return FALSE;
}

//-------------------------------------------------------------------------------------------------
// set equation to VIP CSC
// Both equation selection rules are
// 0: SDTV(601) R  G  B  : 16-235
// 1: SDTV(601) R  G  B  : 0-255
/// @param bMainWin       \b IN: Enable
//-------------------------------------------------------------------------------------------------
void HAL_PQ_set_SelectCSC(MS_U16 u16selection, PQ_WIN ePQWin)
{
    MS_BOOL bMainWin;
    MS_U32 u32Reg0F, u32Reg0E, u32Reg16;

    bMainWin = (ePQWin == PQ_MAIN_WINDOW
        #if ENABLE_MULTI_SCALER
                || ePQWin == PQ_SC1_MAIN_WINDOW ||
                ePQWin == PQ_SC2_MAIN_WINDOW
        #endif
                ) ? TRUE : FALSE;

    u32Reg0F = ePQWin == PQ_MAIN_WINDOW     ? REG_SC_BK1A_0F_L :
               ePQWin == PQ_SUB_WINDOW      ? REG_SC_BK1A_0F_L :
        #if ENABLE_MULTI_SCALER
               ePQWin == PQ_SC1_MAIN_WINDOW ? REG_SC1_BK1A_0F_L :
               ePQWin == PQ_SC2_MAIN_WINDOW ? REG_SC2_BK1A_0F_L :
               ePQWin == PQ_SC2_SUB_WINDOW  ? REG_SC2_BK1A_0F_L :
        #endif
                                              REG_SC_DUMMY;

    u32Reg0E = ePQWin == PQ_MAIN_WINDOW     ? REG_SC_BK1A_0E_L :
               ePQWin == PQ_SUB_WINDOW      ? REG_SC_BK1A_0E_L :
        #if ENABLE_MULTI_SCALER
               ePQWin == PQ_SC1_MAIN_WINDOW ? REG_SC1_BK1A_0E_L :
               ePQWin == PQ_SC2_MAIN_WINDOW ? REG_SC2_BK1A_0E_L :
               ePQWin == PQ_SC2_SUB_WINDOW  ? REG_SC2_BK1A_0E_L :
        #endif
                                              REG_SC_DUMMY;

    u32Reg16 = ePQWin == PQ_MAIN_WINDOW     ? REG_SC_BK1A_16_L :
               ePQWin == PQ_SUB_WINDOW      ? REG_SC_BK1A_16_L :
        #if ENABLE_MULTI_SCALER
               ePQWin == PQ_SC1_MAIN_WINDOW ? REG_SC1_BK1A_16_L :
               ePQWin == PQ_SC2_MAIN_WINDOW ? REG_SC2_BK1A_16_L :
               ePQWin == PQ_SC2_SUB_WINDOW  ? REG_SC2_BK1A_16_L :
        #endif
                                              REG_SC_DUMMY;


    if (bMainWin)
    {
        if (u16selection==0)
        {
            MApi_XC_W2BYTEMSK(u32Reg0F, 0xF0, 0xFF);
            MApi_XC_W2BYTEMSK(u32Reg0E, 0x00, 0x03);
            MApi_XC_W2BYTEMSK(u32Reg16, 0x4A, 0xFF);
        }
        else
        {
            MApi_XC_W2BYTEMSK(u32Reg0F, 0x00, 0xFF);
            MApi_XC_W2BYTEMSK(u32Reg0E, 0x00, 0x03);
            MApi_XC_W2BYTEMSK(u32Reg16, 0x40, 0xFF);
        }
    }
    else
    {
        if (u16selection==0)
        {
            MApi_XC_W2BYTEMSK(u32Reg0F, 0xF000, 0xFF00);
            MApi_XC_W2BYTEMSK(u32Reg0E, 0x0000, 0x0300);
            MApi_XC_W2BYTEMSK(u32Reg16, 0x4A00, 0xFF00);
        }
        else
        {
            MApi_XC_W2BYTEMSK(u32Reg0F, 0x0000, 0xFF00);
            MApi_XC_W2BYTEMSK(u32Reg0E, 0x0000, 0x0300);
            MApi_XC_W2BYTEMSK(u32Reg16, 0x4000, 0xFF00);
        }
    }
}

MS_U8 Hal_PQ_get_madi_fbl_mode(MS_BOOL bMemFmt422,MS_BOOL bInterlace)
{
    UNUSED(bInterlace);
    if(bMemFmt422)
    {
        return MS_MADI_P_MODE_MOT10;
    }
    else
    {
        return MS_MADI_P_MODE8;
    }
}

MS_U16 Hal_PQ_get_csc_ip_idx(MS_CSC_IP_TYPE enCSC)
{
    MS_U16 u16Ret;

    switch(enCSC)
    {
    case MS_CSC_IP_CSC:
        u16Ret = PQ_IP_CSC_Main;
        break;

    default:
    case MS_CSC_IP_VIP_CSC:
        u16Ret = PQ_IP_VIP_CSC_Main;
        break;
    }
    return u16Ret;
}

MS_U16 Hal_PQ_get_422To444_idx(MS_422To444_TYPE etype)
{
    MS_U16 u16ret;
    switch(etype)
    {
    case MS_422To444_ON:
        u16ret = PQ_IP_422To444_ON_Main;
        break;
    case MS_422To444_OFF:
        u16ret = PQ_IP_422To444_OFF_Main;
        break;

    default:
        u16ret = 0xFFFF;
        MS_ASSERT(0)
        break;
    }
    return u16ret;
}

MS_U8 Hal_PQ_get_422To444_mode(MS_BOOL bMemFmt422)
{
    if(bMemFmt422)
    {
        return MS_422To444_ON;
    }
    else
    {
        return MS_422To444_OFF;
    }
}

