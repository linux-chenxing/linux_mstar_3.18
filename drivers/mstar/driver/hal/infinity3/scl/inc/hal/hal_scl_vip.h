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
// (!¡±MStar Confidential Information!¡L) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////
//==============================================================================
#ifndef _HAL_VIP_H
#define _HAL_VIP_H


//-------------------------------------------------------------------------------------------------
//  Defines & ENUM
//-------------------------------------------------------------------------------------------------
typedef enum
{
    E_HAL_SCLVIP_SRAM_SEC_0 =289,
    E_HAL_SCLVIP_SRAM_SEC_1 =272,
    E_HAL_SCLVIP_SRAM_SEC_2 =272,
    E_HAL_SCLVIP_SRAM_SEC_3 =256,
}HalSclVipSramSecNum_e;

typedef enum
{
    E_HAL_SCLVIP_SRAM_DUMP_ICC,
    E_HAL_SCLVIP_SRAM_DUMP_IHC,
    E_HAL_SCLVIP_SRAM_DUMP_HVSP_V,
    E_HAL_SCLVIP_SRAM_DUMP_HVSP_V_1,
    E_HAL_SCLVIP_SRAM_DUMP_HVSP_V_2,
    E_HAL_SCLVIP_SRAM_DUMP_WDR,
    E_HAL_SCLVIP_SRAM_DUMP_GAMMA_Y,
    E_HAL_SCLVIP_SRAM_DUMP_GAMMA_U,
    E_HAL_SCLVIP_SRAM_DUMP_GAMMA_V,
    E_HAL_SCLVIP_SRAM_DUMP_GM10to12_R,
    E_HAL_SCLVIP_SRAM_DUMP_GM10to12_G,
    E_HAL_SCLVIP_SRAM_DUMP_GM10to12_B,
    E_HAL_SCLVIP_SRAM_DUMP_GM12to10_R,
    E_HAL_SCLVIP_SRAM_DUMP_GM12to10_G,
    E_HAL_SCLVIP_SRAM_DUMP_GM12to10_B,
    E_HAL_SCLVIP_SRAM_DUMP_HVSP_H,
    E_HAL_SCLVIP_SRAM_DUMP_HVSP_H_1,
    E_HAL_SCLVIP_SRAM_DUMP_HVSP_H_2,
    E_HAL_SCLVIP_SRAM_DUMP_NUM,
}HalSclVipSramDumpType_e;
typedef enum
{
    E_HAL_SCLVIP_VTRACK_ENABLE_ON,
    E_HAL_SCLVIP_VTRACK_ENABLE_OFF,
    E_HAL_SCLVIP_VTRACK_ENABLE_DEBUG,
}HalSclVipVtrackEnableType_e;
//-------------------------------------------------------------------------------------------------
//  Structure
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Prototype
//-------------------------------------------------------------------------------------------------
#ifndef HAL_VIP_C
#define INTERFACE extern
#else
#define INTERFACE
#endif
INTERFACE void HalSclVipSetLdcHwRwDiff(u16 u8Val);
INTERFACE void HalSclVipSetLdcSwMode(u8 bEn);
INTERFACE void HalSclVipSetLdcBankMode(DrvSclVipLdcLcBankModeType_e enType);
INTERFACE void HalSclVipSetLdcSwIdx(u8 idx);
INTERFACE void HalSclVipSetLdc422To444Md(u8 u8md);
INTERFACE void HalSclVipSetLdc444To422Md(u8 u8md);
INTERFACE void HalSclVipAipDB(u8 u8En);
INTERFACE void HalSclVipMcnrInit(void);
INTERFACE bool HalSclVipGetSramDumpGammaYUV(HalSclVipSramDumpType_e endump,u32 u32Sram,u32 u32reg);
INTERFACE bool HalSclVipGetSramDumpGammaYUVCallback(HalSclVipSramDumpType_e endump,u32 u32Sram,u32 u32reg);
INTERFACE bool HalSclVipGetSramDumpGammaRGB(HalSclVipSramDumpType_e endump,u32 u32Sram,u32 u32reg);
INTERFACE bool HalSclVipGetSramDumpGammaRGBCallback(HalSclVipSramDumpType_e endump,u32 u32Sram,u32 u32reg);
INTERFACE void HalSclVipLdcEco(void);
INTERFACE void HalSclVipExit(void);
INTERFACE void HalSclVipSetRiuBase(u32 u32riubase);
INTERFACE void HalSclVipDlcHistVarOnOff(u16 u16var);
INTERFACE void HalSclVipSetDlcstatMIU(u8 u8value,u32 u32addr1,u32 u32addr2);
INTERFACE void HalSclVipSetDlcShift(u8 u8value);
INTERFACE void HAlSclVipSetDlcMode(u8 u8value);
INTERFACE void HalSclVipDlcHistSetRange(u8 u8value,u8 u8range);
INTERFACE u32  HalSclVipDlcHistGetRange(u8 u8range);
INTERFACE void HalSclVipSetLdcBypass(bool bEn);
INTERFACE void HalSclVipSetLdcDmapPitch(u32 u32Pitch);
INTERFACE void HalSclVipSetLdcDmapBase(u32 u32Base);
INTERFACE void HalSclVipSetLdcDmapPS(u8 bEn);
INTERFACE void HalSclVipSetLdcDmapOffset(u8 u8offset);
INTERFACE void HalSclVipSetLdcSramAmount(u16 u16hor,u16 u16ver);
INTERFACE void HalSclVipSetLdcSramBase(u32 u32hor,u32 u32ver);
INTERFACE void HalSclVipSetLdcSramStr(u16 u16hor,u16 u16ver);
INTERFACE u8   HalSclVipDlcGetBaseIdx(void);
INTERFACE u32  HalSclVipDlcGetPC(void);
INTERFACE u32  HalSclVipDlcGetPW(void);
INTERFACE u8   HalSclVipDlcGetMinP(void);
INTERFACE u8   HalSclVipDlcGetMaxP(void);
INTERFACE void HalSclVipSetDlcActWin(bool bEn,u16 u16Vst,u16 u16Hst,u16 u16Vnd,u16 u16Hnd);
INTERFACE void HalSclVipSeMcnrIPMRead(bool bEn);
INTERFACE void HalSclVipSetIPMConpress(bool bEn);
INTERFACE void HalSclVipSeCiirRead(bool bEn);
INTERFACE void HalSclVipSeCiirWrite(bool bEn);

INTERFACE void HalSclVipSetAutoDownloadAddr(u32 u32baseadr,u16 u16iniaddr,u8 u8cli);
INTERFACE void HalSclVipSetAutoDownloadReq(u16 u16depth,u16 u16reqlen,u8 u8cli);
INTERFACE void HalSclVipSetAutoDownload(u8 bCLientEn,u8 btrigContinue,u8 u8cli);
INTERFACE void HalSclVipSetAutoDownloadTimer(u8 bCLientEn);
INTERFACE void HalSclVipGetNlmSram(u16 u16entry);
INTERFACE void HalSclVipSetNlmSrambyCPU(u16 u16entry,u32 u32tvalue);
INTERFACE u8   HalSclVipSramDump(HalSclVipSramDumpType_e endump,u32 u32Sram);
INTERFACE void HalSclVipVtrackEnable(u8 u8FrameRate, HalSclVipVtrackEnableType_e bEnable);
INTERFACE void HalSclVipVtrackSetUserDefindedSetting(bool bUserDefinded, u8 *pu8Setting);
INTERFACE void HalSclVipVtrackSetKey(bool bUserDefinded, u8 *pu8Setting);
INTERFACE void HalSclVipVtrackSetPayloadData(u16 u16Timecode, u8 u8OperatorID);
INTERFACE bool HalSclVipGetVipBypass(void);
INTERFACE void HalSclVipInitY2R(void);
INTERFACE bool HalSclVipGetMcnrBypass(void);
INTERFACE bool HalSclVipGetLdcBypass(void);
INTERFACE bool HalSclVipGetNlmBypass(void);
    #undef INTERFACE

#endif
