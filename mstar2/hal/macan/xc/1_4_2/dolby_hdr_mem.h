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
//////////////////////////////////////////////////////////////////////////////////////////////////
//
// @file   hdr_share_mem.h
// @brief  XC KMD Driver Interface
// @author MStar Semiconductor Inc.
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _DOLBY_HDR_MEM_H_
#define _DOLBY_HDR_MEM_H_

#define HDR_MEM_VER 0
#define HDR_COMMON_INFO_SIZE 32
#define HDR_MEM_ENTRY_NUM 48
#define HDR_ENTRY_SIZE 24
#define HDR_TOTAL_ENTRY_SIZE (HDR_MEM_ENTRY_NUM * HDR_ENTRY_SIZE)
#define HDR_MEM_COMMON_ENTRY_SIZE (HDR_COMMON_INFO_SIZE + HDR_TOTAL_ENTRY_SIZE) // 0x320
#define HDR_INPUT_MD_SIZE 32
#define HDR_TOTAL_INPUT_MD_SIZE (HDR_INPUT_MD_SIZE * HDR_MEM_ENTRY_NUM)
#define HDR_OUTPUT_REG_SIZE (430 * BYTE_PER_WORD)
#define HDR_TOTAL_REGSET_SIZE (HDR_OUTPUT_REG_SIZE * HDR_MEM_ENTRY_NUM)
#define HDR_DV_3DLUT_NUM 4944
#define HDR_DV_TMO_NUM 1248
#define HDR_VDEC_REORDER_NUM 10
#define HDR_TOTAL_LUTS_SIZE (HDR_DV_3DLUT_NUM * BYTE_PER_WORD * HDR_MEM_ENTRY_NUM)
//  +-------------+-----------------+------------------+------------------+---------------+-----------------+
//  |      4      |         4       |         4        |         4        |        4      |        4        |
//  | u32CFDReady |  u32InputMDAddr | u32HDRRegsetAddr | u32HDRRegsetSize | u32HDRLutAddr |  u32HDRLutSize  |
//  +-------------+-----------------+------------------+------------------+---------------+-----------------+
typedef struct
{
    //// Entries Start ////
    MS_U16 u16CFDReady;
    MS_U16 u16Entry_Skip;
    MS_U32 u32InputMDAddr;      // pointer to ST_KDRV_HDR_INPUT_MD_FORMAT_INFO
    MS_U32 u32HDRRegsetAddr;
    MS_U32 u32HDRRegsetSize;
    MS_U32 u32HDRLutAddr;
    MS_U32 u32HDRLutSize;
    //// Entries End ////
} ST_KDRV_XC_HDR_INFO_ENTRY;

//  +---------+-------+-----------+---------+--------+-------------+-----------+----------+-------------+
//  |    1    |   1   |     4     |    4    |    1   |      4      |      4    |     1    |      1      |
//  | Version | Index | DM_length | DM_Addr | DM_MIU | comp_length | comp_addr | comp_miu | comp_enable |
//  +---------+-------+-----------+---------+--------+-------------+-----------+----------+-------------+
typedef struct __attribute__((packed))
{
    MS_U8   u8Version;            /// current shared memory format version
    MS_U8   u8CurrentIndex;       /// current metadata index, which is the same as DS index if DS is turned on.
    MS_U32  u32DmLength;          /// display management metadata length
    MS_U32  u32DmAddr;            /// where display management metadata lies
    MS_U8   u8DmMiuNo;            /// dm miu no
    MS_BOOL bEnableDM;            /// DM enable
    MS_U32  u32ComposerLength;    /// Composer data length
    MS_U32  u32ComposerAddr;      /// where composer data lies
    MS_U8   u8ComposerMiuNo;      /// composer miu no
    MS_BOOL bEnableComposer;      /// composer enable
    MS_U8   au8Reserved[10];      /// reserved
} ST_KDRV_XC_HDR_INPUT_MD_FORMAT;

typedef struct __attribute__((packed))
{
    MS_U16 u16GDReady;
    MS_U16 u16GDValue;
    MS_U32 u32Reserved;
} ST_KDRV_XC_HDR_GD_FORMAT;

//  +--------------+--------------------+--------------------+------------------------+---------------------------+
//  |      32      |       24 * 48      |       32 * 48      |     (430 << 5) * 48    |      4944 * 32 * 48       |
//  | Common info  |        Entris      |   Input MD Entris  |       Reg sets         |           Luts            |
//  +--------------+--------------------+--------------------+------------------------+---------------------------+
typedef struct
{
    MS_U16 u16Version;
    MS_U16 u16CommInfoSize;
    MS_U16 u16MDMiuSel;
    MS_U16 u16LutMiuSel;
    MS_U32 u32RdPtr;
    MS_U32 u32WdPtr;
    MS_U64 u64Reserved;
    MS_U32 u32MDAddr;
    MS_U32 au32Reserved[1];
    //// Entries Start ////
    ST_KDRV_XC_HDR_INFO_ENTRY stEntries[HDR_MEM_ENTRY_NUM];
    //// Entries End ////
} ST_KDRV_XC_HDR_DOLBY_MEMORY_FORMAT_EX;

void initDolbyHDRMem(ST_KDRV_XC_HDR_DOLBY_MEMORY_FORMAT_EX* pstHDRShareMem);
void flushDolbyHDRMemPtr(ST_KDRV_XC_HDR_DOLBY_MEMORY_FORMAT_EX* pstHDRShareMem);
MS_BOOL isHDRInfoEntryEmpty(ST_KDRV_XC_HDR_DOLBY_MEMORY_FORMAT_EX* pstHDRShareMem);
MS_BOOL isHDRInfoEntryFull(ST_KDRV_XC_HDR_DOLBY_MEMORY_FORMAT_EX* pstHDRShareMem);
MS_U32 getDolbyHDRMemRPtr(ST_KDRV_XC_HDR_DOLBY_MEMORY_FORMAT_EX* pstHDRShareMem);
MS_BOOL incDolbyHDRMemRPtr(ST_KDRV_XC_HDR_DOLBY_MEMORY_FORMAT_EX* pstHDRShareMem,MS_U16 u16Skip);
MS_U32 getDolbyHDRMemWPtr(ST_KDRV_XC_HDR_DOLBY_MEMORY_FORMAT_EX* pstHDRShareMem);
MS_BOOL incDolbyHDRMemWPtr(ST_KDRV_XC_HDR_DOLBY_MEMORY_FORMAT_EX* pstHDRShareMem);
MS_BOOL isCFDDone(ST_KDRV_XC_HDR_DOLBY_MEMORY_FORMAT_EX* pstHDRShareMem, MS_U32 u32Idx);
MS_U16 getDolbyHDRMDMiuSel(ST_KDRV_XC_HDR_DOLBY_MEMORY_FORMAT_EX* pstHDRShareMem);
MS_U16 getDolbyHDRLutMiuSel(ST_KDRV_XC_HDR_DOLBY_MEMORY_FORMAT_EX* pstHDRShareMem);
void setDolbyHDRGDInfo( MS_U32 u32Idx, ST_KDRV_XC_HDR_GD_FORMAT *pstHDRGDInfo);
void setDolbyHDRInfoEntry(ST_KDRV_XC_HDR_DOLBY_MEMORY_FORMAT_EX* pstHDRShareMem, MS_U32 u32Idx, ST_KDRV_XC_HDR_INFO_ENTRY *pstHDREntry);
MS_BOOL getDolbyHDRInfoEntry(ST_KDRV_XC_HDR_DOLBY_MEMORY_FORMAT_EX* pstHDRShareMem, MS_U32 u32Idx, ST_KDRV_XC_HDR_INFO_ENTRY *pstHDREntry);
MS_BOOL getDolbyHDRGDInfo(MS_U32 u32Idx, ST_KDRV_XC_HDR_GD_FORMAT *pstHDRGDInfo);
MS_U32 getDolbyHDRInputMDAddr(MS_U32 u32ShareMemBaseAddr, MS_U32 u32Idx);
MS_U32 getDolbyHDRRegsetAddr(MS_U32 u32ShareMemBaseAddr, MS_U32 u32Idx);
MS_U32 getDolbyHDRLutAddr(MS_U32 u32ShareMemBaseAddr, MS_U32 u32Idx);
MS_U32 getDolbyHDRLutSize(MS_U32 u32Idx);
MS_BOOL getDolbyHDRInputMDFormatInfo(MS_U8 *pu8Data, ST_KDRV_XC_HDR_INPUT_MD_FORMAT *pstFormatInfo);
#endif // _DOLBY_HDR_MEM_H_