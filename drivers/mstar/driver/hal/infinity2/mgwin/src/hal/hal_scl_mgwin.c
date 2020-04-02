//<MStar Software>
//******************************************************************************
// MStar Software
// Copyright (c) 2010 - 2012 MStar Semiconductor, Inc. All rights reserved.
// All software, firmware and related documentation herein ("MStar Software") are
// intellectual property of MStar Semiconductor, Inc. ("MStar") and protected by
// law, including, but not limited to, copyright law and international treaties.
// Any use, modification, reproduction, retransmission, or republication of all
// or part of MStar Software is expressly prohibited, unless prior written
// permission has been granted by MStar.
//
// By accessing, browsing and/or using MStar Software, you acknowledge that you
// have read, understood, and agree, to be bound by below terms ("Terms") and to
// comply with all applicable laws and regulations:
//
// 1. MStar shall retain any and all right, ownership and interest to MStar
//    Software and any modification/derivatives thereof.
//    No right, ownership, or interest to MStar Software and any
//    modification/derivatives thereof is transferred to you under Terms.
//
// 2. You understand that MStar Software might include, incorporate or be
//    supplied together with third party`s software and the use of MStar
//    Software may require additional licenses from third parties.
//    Therefore, you hereby agree it is your sole responsibility to separately
//    obtain any and all third party right and license necessary for your use of
//    such third party`s software.
//
// 3. MStar Software and any modification/derivatives thereof shall be deemed as
//    MStar`s confidential information and you agree to keep MStar`s
//    confidential information in strictest confidence and not disclose to any
//    third party.
//
// 4. MStar Software is provided on an "AS IS" basis without warranties of any
//    kind. Any warranties are hereby expressly disclaimed by MStar, including
//    without limitation, any warranties of merchantability, non-infringement of
//    intellectual property rights, fitness for a particular purpose, error free
//    and in conformity with any international standard.  You agree to waive any
//    claim against MStar for any loss, damage, cost or expense that you may
//    incur related to your use of MStar Software.
//    In no event shall MStar be liable for any direct, indirect, incidental or
//    consequential damages, including without limitation, lost of profit or
//    revenues, lost or damage of data, and unauthorized system use.
//    You agree that this Section 4 shall still apply without being affected
//    even if MStar Software has been modified by MStar in accordance with your
//    request or instruction for your use, except otherwise agreed by both
//    parties in writing.
//
// 5. If requested, MStar may from time to time provide technical supports or
//    services in relation with MStar Software to you for your use of
//    MStar Software in conjunction with your or your customer`s product
//    ("Services").
//    You understand and agree that, except otherwise agreed by both parties in
//    writing, Services are provided on an "AS IS" basis and the warranty
//    disclaimer set forth in Section 4 above shall apply.
//
// 6. Nothing contained herein shall be construed as by implication, estoppels
//    or otherwise:
//    (a) conferring any license or right to use MStar name, trademark, service
//        mark, symbol or any other identification;
//    (b) obligating MStar or any of its affiliates to furnish any person,
//        including without limitation, you and your customers, any assistance
//        of any kind whatsoever, or any information; or
//    (c) conferring any license or right under any intellectual property right.
//
// 7. These terms shall be governed by and construed in accordance with the laws
//    of Taiwan, R.O.C., excluding its conflict of law rules.
//    Any and all dispute arising out hereof or related hereto shall be finally
//    settled by arbitration referred to the Chinese Arbitration Association,
//    Taipei in accordance with the ROC Arbitration Law and the Arbitration
//    Rules of the Association by three (3) arbitrators appointed in accordance
//    with the said Rules.
//    The place of arbitration shall be in Taipei, Taiwan and the language shall
//    be English.
//    The arbitration award shall be final and binding to both parties.
//
//******************************************************************************
//<MStar Software>
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
#define __HAL_SCL_MGWIN_C__

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
// Common Definition
// Internal Definition
#include "ms_platform.h"
#include "cam_os_wrapper.h"
#include "hal_scl_mgwin_util.h"
#include "hal_scl_mgwin_reg.h"
#include "drv_scl_mgwin_dbg.h"
#include "drv_scl_mgwin.h"
//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define SCLMGWIN_REG(WinId, REG)                        ((WinId == E_DRV_SCLMGWIN_WIN_ID_1) ? (REG + 0x0000) : (REG + 0x0400))
#define SCLMGWIN_SUBWIN_SIZE_REG(SubWinId, REG)         (REG + (SubWinId*2))
#define SCLMGWIN_SUBWIN_BASE_REG(SubWinId, REG)         (REG + (SubWinId*4))
#define SCLMGWIN_SUBWIN_LINE_OFFSET_REG(SubWinId, REG)  (REG + (SubWinId*2))
#define SCLMGWIN_SUBWIN_MEM_CFG_REG(SubWinId, REG)      (REG + (SubWinId*2))

#define FPGA_MGWIN_SCRIPT           0


#if FPGA_MGWIN_SCRIPT == 1

#ifdef W2BYTEMSK
    #undef W2BYTEMSK
    #define W2BYTEMSK(addr, val, msk) FpgaScriptW2BYTEMSK(addr, val, msk)
#endif

#ifdef W2BYTE
    #undef W2BYTE
    #define W2BYTE(addr, val)   FpgaScriptW2BYTE(addr, val)
#endif


static u16 u16CurBank = 0;

void FpgaScriptW2BYTEMSK(u32 u32Adr, u16 u16Val, u16 u16Msk)
{
    u8 u8Msk, u8Val, u8Adr;
    if(u16CurBank != ((u32Adr & 0x00FF00) >> 8))
    {
        u16CurBank = ((u32Adr & 0x00FF00) >> 8);
        SCL_DBG(SCL_DBG_LV_DRVSCLMGWIN() & EN_DBGMG_SCLMGWINLEVEL_FPGASCRIPT, "\n");
        SCL_DBG(SCL_DBG_LV_DRVSCLMGWIN() & EN_DBGMG_SCLMGWINLEVEL_FPGASCRIPT, "wriu -b 0x2f00 0xff 0x%02x \n", u16CurBank);
    }

    u8Adr = (u8)(u32Adr & 0x0000FF);
    u8Msk = (u8)(u16Msk & 0x00FF);
    u8Val = (u8)(u16Val & 0x00FF);
    SCL_DBG(SCL_DBG_LV_DRVSCLMGWIN() & EN_DBGMG_SCLMGWINLEVEL_FPGASCRIPT, "wriu -b 0x2f%02x 0x%02x 0x%02X \n", u8Adr, u8Msk, u8Val);
    u8Adr = u8Adr + 1;
    u8Msk = (u8)(u16Msk >> 8);
    u8Val = (u8)(u16Val >> 8);
    SCL_DBG(SCL_DBG_LV_DRVSCLMGWIN() & EN_DBGMG_SCLMGWINLEVEL_FPGASCRIPT, "wriu -b 0x2f%02x 0x%02x 0x%02x \n", u8Adr, u8Msk, u8Val);

}


void FpgaScriptW2BYTE(u32 u32Adr, u16 u16Val)
{
    u8 u8Val, u8Adr;

    if(u16CurBank != ((u32Adr & 0x00FF00) >> 8))
    {
        u16CurBank = ((u32Adr & 0x00FF00) >> 8);
        SCL_DBG(SCL_DBG_LV_DRVSCLMGWIN() & EN_DBGMG_SCLMGWINLEVEL_FPGASCRIPT, "\n");
        SCL_DBG(SCL_DBG_LV_DRVSCLMGWIN() & EN_DBGMG_SCLMGWINLEVEL_FPGASCRIPT, "wriu -b 0x2f00 0xff 0x%02x \n", u16CurBank);
    }

    u8Adr = (u8)(u32Adr & 0x0000FF);
    u8Val = (u8)(u16Val & 0x00FF);
    SCL_DBG(SCL_DBG_LV_DRVSCLMGWIN() & EN_DBGMG_SCLMGWINLEVEL_FPGASCRIPT, "wriu -b 0x2f%02x 0xff 0x%02x \n", u8Adr, u8Val);
    u8Adr = u8Adr + 1;
    u8Val = (u8)(u16Val >> 8);
    SCL_DBG(SCL_DBG_LV_DRVSCLMGWIN() & EN_DBGMG_SCLMGWINLEVEL_FPGASCRIPT, "wriu -b 0x2f%02x 0xff 0x%02x \n", u8Adr, u8Val);
}

#endif

//-------------------------------------------------------------------------------------------------
//  Variable


//-------------------------------------------------------------------------------------------------
//  Functions
//-------------------------------------------------------------------------------------------------
void HalSclMgwinInit(DrvSclMgwinWinIdType_e enWinId)
{
    W2BYTEMSK(SCLMGWIN_REG(enWinId, REG_MGWIN1_33_L), BIT7|BIT1, BIT7|BIT1); //[7]reg_dbf_sw_trig_md, [1]reg_mgwin_ctrl_sw_mode
    W2BYTEMSK(SCLMGWIN_REG(enWinId, REG_MGWIN1_34_L), 0xFFFF, 0xFFFF); // reg_mgwin_sw_mode
    W2BYTEMSK(SCLMGWIN_REG(enWinId, REG_MGWIN1_35_L), 0xFFFF, 0xFFFF); // reg_mgwin_mem_sw_mode
    W2BYTEMSK(SCLMGWIN_REG(enWinId, REG_MGWIN2_0F_L), 0x0801, 0xFFFF); // reg_out_flow_ctrl_???
    W2BYTEMSK(SCLMGWIN_REG(enWinId, REG_MGWIN2_12_L), 0xFF80, 0xFFFF); // [15:8]reg_fill_a [7:0]reg_fill_r
    W2BYTEMSK(SCLMGWIN_REG(enWinId, REG_MGWIN2_13_L), 0x0080, 0xFFFF); //[15:8]reg_fill_g [7:0]reg_fill_b
    W2BYTEMSK(SCLMGWIN_REG(enWinId, REG_MGWIN2_14_L), 0x0000, 0x00FF); //                 [7:0]reg_fill_a_bg
}

void HalSclMgwinSetFrameOnOff(DrvSclMgwinWinIdType_e enWinId, bool bEn)
{
    W2BYTEMSK(SCLMGWIN_REG(enWinId, REG_MGWIN1_07_L), bEn ? BIT15|BIT14 : 0 , BIT15|BIT14);
}

void HalSclMgwinSetFrameTrigSel(DrvSclMgwinWinIdType_e enWinId, DrvSclMgwinFramePath_e enPath)
{
    W2BYTEMSK(SCLMGWIN_REG(enWinId, REG_MGWIN1_07_L), (enPath == E_DRV_SCLMGWIN_PATH_SC1 ? 0 : BIT11), BIT11);
}

void HalSclMgwinSetFrameSc1ForSc0En(DrvSclMgwinWinIdType_e enWinId, bool bEn)
{
    W2BYTEMSK(SCLMGWIN_REG(enWinId, REG_MGWIN1_07_L), bEn ? BIT13 : 0, BIT13);
}

void HalSclMgwinSetFrameLayOff(DrvSclMgwinWinIdType_e enWinId, bool bEn)
{
    W2BYTEMSK(SCLMGWIN_REG(enWinId, REG_MGWIN1_14_L), bEn ? BIT0 : 0, BIT0);
}

void HalSclMgwinSetFrameLayAlpha(DrvSclMgwinWinIdType_e enWinId, u8 u8Alpha)
{
    W2BYTEMSK(SCLMGWIN_REG(enWinId, REG_MGWIN1_12_L), (u16)u8Alpha << 8, 0xFF00);
}

void HalSclMgwinSetFrameSize( DrvSclMgwinWinIdType_e enWinId, u16 u16Hsize, u16 u16Vsize)
{
    W2BYTE(SCLMGWIN_REG(enWinId, REG_MGWIN1_30_L), u16Hsize);
    W2BYTE(SCLMGWIN_REG(enWinId, REG_MGWIN1_31_L), u16Vsize);
}


void HalSclMgwinSetSubWinOnOff(DrvSclMgwinWinIdType_e enWinId, DrvSclMgwinSubWinIdType_e enSubWinId, bool bEn)
{
    u32 u32Reg = SCLMGWIN_REG(enWinId, REG_MGWIN1_32_L);
    u16 u16Msk = (1 << enSubWinId);
    u16 u16Val = bEn ? (1 << enSubWinId) : 0;
    W2BYTEMSK(u32Reg, u16Val, u16Msk);
}

void HalSclMgwinSetSubWinMemFmt(DrvSclMgwinWinIdType_e enWinId, DrvSclMgwinSubWinIdType_e enSubWinId, DrvSclMgwinMemFormat_e enMemFmt, bool bRGBOut)
{
    bool bY2R = 0;
    u8 u16MemFmt;
    u16MemFmt =  enMemFmt == E_DRV_SCLMGWIN_MEM_FMT_ARGB8888   ? 0x00 :
                 enMemFmt == E_DRV_SCLMGWIN_MEM_FMT_ABGR8888   ? 0x04 :
                 enMemFmt == E_DRV_SCLMGWIN_MEM_FMT_RGBA8888   ? 0x08 :
                 enMemFmt == E_DRV_SCLMGWIN_MEM_FMT_BGRA8888   ? 0x0C :
                 enMemFmt == E_DRV_SCLMGWIN_MEM_FMT_RGB565     ? 0x1C :
                 enMemFmt == E_DRV_SCLMGWIN_MEM_FMT_YUV444_8B  ? 0x20 :
                 enMemFmt == E_DRV_SCLMGWIN_MEM_FMT_YUV444_10B ? 0x28 :
                 enMemFmt == E_DRV_SCLMGWIN_MEM_FMT_YUV422_8B  ? 0x30 :
                 enMemFmt == E_DRV_SCLMGWIN_MEM_FMT_YUV422_8CE ? 0x31 :
                 enMemFmt == E_DRV_SCLMGWIN_MEM_FMT_YUV422_6CE ? 0x35 :
                 enMemFmt == E_DRV_SCLMGWIN_MEM_FMT_YUV420_8B  ? 0x32 :
                 enMemFmt == E_DRV_SCLMGWIN_MEM_FMT_YUV420_8CE ? 0x33 :
                 enMemFmt == E_DRV_SCLMGWIN_MEM_FMT_YUV420_6CE ? 0x37 :
                                                                 0x00;
    bY2R = (bRGBOut && (enMemFmt >= E_DRV_SCLMGWIN_MEM_FMT_YUV444_8B)) ? 1 : 0;
    W2BYTEMSK(SCLMGWIN_SUBWIN_MEM_CFG_REG(enSubWinId, SCLMGWIN_REG(enWinId, REG_MGWIN2_70_L)), u16MemFmt, 0x003F);
    W2BYTEMSK(SCLMGWIN_REG(enWinId, REG_MGWIN2_10_L), bY2R ? BIT4 : 0, BIT4);

}

void HalSclMgwinSetSubWinBaseAddr(DrvSclMgwinWinIdType_e enWinId, DrvSclMgwinSubWinIdType_e enSubWinId, u32 u32BaseAddr)
{
    u16 u16Val;
    u32BaseAddr = (u32BaseAddr >> 5); // MIU Bus 32
    u16Val = (u16)(u32BaseAddr & 0x0000FFFF);
    W2BYTE(SCLMGWIN_SUBWIN_BASE_REG(enSubWinId, SCLMGWIN_REG(enWinId, REG_MGWIN2_40_L)), u16Val);
    u16Val = (u16)(u32BaseAddr >> 16);
    W2BYTE(SCLMGWIN_SUBWIN_BASE_REG(enSubWinId, SCLMGWIN_REG(enWinId, REG_MGWIN2_41_L)), u16Val);
}

void HalSclMgwinSetSubWinPitch(DrvSclMgwinWinIdType_e enWinId, DrvSclMgwinSubWinIdType_e enSubWinId, u32 u32Pitch)
{
    W2BYTE(SCLMGWIN_SUBWIN_SIZE_REG(enSubWinId, SCLMGWIN_REG(enWinId, REG_MGWIN2_60_L)), u32Pitch);
}

void HalSclMgwinSetSubWinSize(DrvSclMgwinWinIdType_e enWinId, DrvSclMgwinSubWinIdType_e enSubWinId, u16 u16Xstart, u16 u16Xend, u16 u16Ystart, u16 u16Yend)
{
    W2BYTE(SCLMGWIN_SUBWIN_SIZE_REG(enSubWinId, SCLMGWIN_REG(enWinId, REG_MGWIN1_40_L)), u16Xstart);
    W2BYTE(SCLMGWIN_SUBWIN_SIZE_REG(enSubWinId, SCLMGWIN_REG(enWinId, REG_MGWIN1_50_L)), u16Xend);
    W2BYTE(SCLMGWIN_SUBWIN_SIZE_REG(enSubWinId, SCLMGWIN_REG(enWinId, REG_MGWIN1_60_L)), u16Ystart);
    W2BYTE(SCLMGWIN_SUBWIN_SIZE_REG(enSubWinId, SCLMGWIN_REG(enWinId, REG_MGWIN1_70_L)), u16Yend);
}

void HalSclMgwinSetFrameDbfSwTrig(DrvSclMgwinWinIdType_e enWinId)
{
    W2BYTEMSK(SCLMGWIN_REG(enWinId, REG_MGWIN1_33_L), BIT11, BIT11);
}

u8 HalSclMgwinGetFrameDbfSwTrig(DrvSclMgwinWinIdType_e enWinId)
{
    u8 u8Val = (R2BYTE(SCLMGWIN_REG(enWinId, REG_MGWIN1_33_L)) & BIT11) ? 1 : 0;
    return u8Val;
}

void HalSclMgwinSetFrameStamp(DrvSclMgwinWinIdType_e enWinId, u8 u8FrameId)
{
    W2BYTEMSK( SCLMGWIN_REG(enWinId, REG_MGWIN1_36_L), u8FrameId, 0x00FF);
}

u8 HalSclMgwinGetFrameStamp(DrvSclMgwinWinIdType_e enWinId)
{
    u8 u8Val = (R2BYTE( SCLMGWIN_REG(enWinId, REG_MGWIN1_36_L)) & 0xFF00) >> 8;
    return u8Val;
}

void HalSclMgwinSetRegister(u32 u32Addr, u16 u16Val, u16 u16Msk)
{
    W2BYTEMSK(u32Addr, u16Val, u16Msk);
}



#undef __HAL_SCL_MGWIN_C__
