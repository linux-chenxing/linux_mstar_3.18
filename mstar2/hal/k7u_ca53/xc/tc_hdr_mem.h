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

#ifndef _TC_HDR_MEM_H_
#define _TC_HDR_MEM_H_

#define TC_HDR_MEM_VER 0
#define TC_HDR_COMMON_INFO_SIZE 32
#define TC_HDR_MEM_ENTRY_NUM 48
#define TC_HDR_ENTRY_SIZE 24
#define TC_HDR_TOTAL_ENTRY_SIZE (TC_HDR_MEM_ENTRY_NUM * TC_HDR_ENTRY_SIZE)
#define TC_HDR_MEM_COMMON_ENTRY_SIZE (TC_HDR_COMMON_INFO_SIZE + TC_HDR_TOTAL_ENTRY_SIZE) // 0x320
#define TC_HDR_INPUT_MD_SIZE 32
#define TC_HDR_TOTAL_INPUT_MD_SIZE (TC_HDR_INPUT_MD_SIZE * TC_HDR_MEM_ENTRY_NUM)
#define TC_HDR_OUTPUT_REG_SIZE 0x2D00 //(720*16)         (430 << 5)
#define TC_HDR_TOTAL_REGSET_SIZE (TC_HDR_OUTPUT_REG_SIZE * TC_HDR_MEM_ENTRY_NUM)
#define TC_HDR_DV_3DLUT_NUM 4944
#define TC_HDR_DV_TMO_NUM 512

//  +-------------+-----------------+------------------+------------------+---------------+-----------------+
//  |      4      |         4       |         4        |         4        |        4      |        4        |
//  | u32CFDReady |  u32InputMDAddr | u32HDRRegsetAddr | u32HDRRegsetSize | u32HDRLutAddr |  u32HDRLutSize  |
//  +-------------+-----------------+------------------+------------------+---------------+-----------------+
typedef struct
{
    //// Entries Start ////
    MS_U32 u32CFDReady;
    MS_U32 u32InputMDAddr;      // pointer to ST_KDRV_HDR_INPUT_MD_FORMAT_INFO
    MS_U32 u32HDRRegsetAddr;
    MS_U32 u32HDRRegsetSize;
    MS_U32 u32HDRLutAddr;       //No use
    MS_U32 u32HDRLutSize;       //No use
    //// Entries End ////
} ST_KDRV_XC_TC_HDR_INFO_ENTRY;

//  +---------+-------+-----------+---------+--------+-------------+-----------+----------+-------------+
//  |    1    |   1   |     4     |    4    |    1   |      4      |      4    |     1    |      1      |
//  | Version | Index | DM_length | DM_Addr | DM_MIU | comp_length | comp_addr | comp_miu | comp_enable |
//  +---------+-------+-----------+---------+--------+-------------+-----------+----------+-------------+
typedef struct __attribute__((packed))
{
    MS_U8   u8Version;            /// current shared memory format version
    MS_U8   u8CurrentIndex;       /// current metadata index, which is the same as DS index if DS is turned on.
    MS_U8   u8T35Enable;          /// T35 enable
    MS_U8   u8T35MiuNo;            /// T35 miu no
    MS_U32  u32T35Length;            /// T35 metadata length
    MS_U32  u32T35Addr;            /// T35 metadata lies
    MS_U8   u8CREnable;          /// CR enable
    MS_U8   u8CRMiuNo;            /// CR miu no
    MS_U32  u32CRLength;            /// CR metadata length
    MS_U32  u32CRAddr;            /// CR metadata lies
    MS_U8   au8Reserved[10];      /// reserved
} ST_KDRV_XC_TC_HDR_INPUT_MD_FORMAT;

//  +--------------+--------------------+--------------------+------------------------+---------------------------+
//  |      32      |       24 * 48      |       32 * 48      |     (430 << 5) * 48    | 4944 * 32 + 512 * 32 * 47 |
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
    ST_KDRV_XC_TC_HDR_INFO_ENTRY stEntries[TC_HDR_MEM_ENTRY_NUM];
    //// Entries End ////
} ST_KDRV_XC_HDR_TC_MEMORY_FORMAT_EX;

void initTCHDRMem(ST_KDRV_XC_HDR_TC_MEMORY_FORMAT_EX* pstHDRShareMem);
void flushTCHDRMemPtr(ST_KDRV_XC_HDR_TC_MEMORY_FORMAT_EX* pstHDRShareMem);
MS_BOOL isTCHDRInfoEntryEmpty(ST_KDRV_XC_HDR_TC_MEMORY_FORMAT_EX* pstHDRShareMem);
MS_BOOL isTCHDRInfoEntryFull(ST_KDRV_XC_HDR_TC_MEMORY_FORMAT_EX* pstHDRShareMem);
MS_U32 getTCHDRMemRPtr(ST_KDRV_XC_HDR_TC_MEMORY_FORMAT_EX* pstHDRShareMem);
MS_BOOL incTCHDRMemRPtr(ST_KDRV_XC_HDR_TC_MEMORY_FORMAT_EX* pstHDRShareMem);
MS_U32 getTCHDRMemWPtr(ST_KDRV_XC_HDR_TC_MEMORY_FORMAT_EX* pstHDRShareMem);
MS_BOOL incTCHDRMemWPtr(ST_KDRV_XC_HDR_TC_MEMORY_FORMAT_EX* pstHDRShareMem);
MS_BOOL isCFDTCHDone(ST_KDRV_XC_HDR_TC_MEMORY_FORMAT_EX* pstHDRShareMem, MS_U32 u32Idx);
MS_U16 getTCHDRMDMiuSel(ST_KDRV_XC_HDR_TC_MEMORY_FORMAT_EX* pstHDRShareMem);
MS_U16 getTCHDRLutMiuSel(ST_KDRV_XC_HDR_TC_MEMORY_FORMAT_EX* pstHDRShareMem);
void setTCHDRInfoEntry(ST_KDRV_XC_HDR_TC_MEMORY_FORMAT_EX* pstHDRShareMem, MS_U32 u32Idx, ST_KDRV_XC_TC_HDR_INFO_ENTRY *pstHDREntry);
MS_BOOL getTCHDRInfoEntry(ST_KDRV_XC_HDR_TC_MEMORY_FORMAT_EX* pstHDRShareMem, MS_U32 u32Idx, ST_KDRV_XC_TC_HDR_INFO_ENTRY *pstHDREntry);
MS_U32 getTCHDRInputMDAddr(MS_U32 u32ShareMemBaseAddr, MS_U32 u32Idx);
MS_U32 getTCHDRRegsetAddr(MS_U32 u32ShareMemBaseAddr, MS_U32 u32Idx);
MS_U32 getTCHDRLutAddr(MS_U32 u32ShareMemBaseAddr, MS_U32 u32Idx);
MS_U32 getTCHDRLutSize(MS_U32 u32Idx);
MS_BOOL getTCHDRInputMDFormatInfo(MS_U8 *pu8Data, ST_KDRV_XC_TC_HDR_INPUT_MD_FORMAT *pstFormatInfo);
#endif // _DOLBY_HDR_MEM_H_