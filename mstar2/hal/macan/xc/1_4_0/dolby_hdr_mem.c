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
// @file   hdr_share_mem.c
// @brief  XC KMD Driver Interface
// @author MStar Semiconductor Inc.
//////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(CONFIG_MIPS)
#elif defined(CONFIG_ARM) || defined(CONFIG_ARM64)
#include <asm/io.h>
#endif
#include <linux/kernel.h>
#include <asm/string.h>
#include "mdrv_mstypes.h"
#include "dolby_hdr_mem.h"
#include "mhal_xc.h"
extern MS_U8 u8tmpCurrentIndex;
MS_BOOL bJump=FALSE;
void initDolbyHDRMem(ST_KDRV_XC_HDR_DOLBY_MEMORY_FORMAT_EX* pstHDRShareMem)
{
    memset((void *)pstHDRShareMem, 0, sizeof(ST_KDRV_XC_HDR_DOLBY_MEMORY_FORMAT_EX));
    pstHDRShareMem->u16Version = HDR_MEM_VER;
    pstHDRShareMem->u16CommInfoSize = HDR_COMMON_INFO_SIZE;
}

void flushDolbyHDRMemPtr(ST_KDRV_XC_HDR_DOLBY_MEMORY_FORMAT_EX* pstHDRShareMem)
{
    pstHDRShareMem->u32RdPtr = pstHDRShareMem->u32WdPtr = 0;
}

MS_BOOL isHDRInfoEntryEmpty(ST_KDRV_XC_HDR_DOLBY_MEMORY_FORMAT_EX* pstHDRShareMem)
{
    return (pstHDRShareMem->u32RdPtr == pstHDRShareMem->u32WdPtr);
}

MS_BOOL isHDRInfoEntryFull(ST_KDRV_XC_HDR_DOLBY_MEMORY_FORMAT_EX* pstHDRShareMem)
{
    MS_U32 u32NextWPtr = pstHDRShareMem->u32WdPtr + 1;
    if (u32NextWPtr >= HDR_MEM_ENTRY_NUM)
        u32NextWPtr = 0;

    if (u32NextWPtr == pstHDRShareMem->u32RdPtr)
        return TRUE; // Queue is full
    else
        return FALSE;
}

MS_U32 getDolbyHDRMemRPtr(ST_KDRV_XC_HDR_DOLBY_MEMORY_FORMAT_EX* pstHDRShareMem)
{
    return pstHDRShareMem->u32RdPtr;
}

MS_BOOL incDolbyHDRMemRPtr(ST_KDRV_XC_HDR_DOLBY_MEMORY_FORMAT_EX* pstHDRShareMem,MS_U16 u16Skip)
{
    if (pstHDRShareMem->u32RdPtr == pstHDRShareMem->u32WdPtr)
        return FALSE; // Queue is empty
    MS_BOOL frame_count_protect = FALSE;
    if(pstHDRShareMem->u32RdPtr > pstHDRShareMem->u32WdPtr)
    {
        if(((pstHDRShareMem->u32WdPtr+HDR_MEM_ENTRY_NUM)-pstHDRShareMem->u32RdPtr)>HDR_VDEC_REORDER_NUM)
            frame_count_protect = TRUE;
    }else
    {
        if((pstHDRShareMem->u32WdPtr-pstHDRShareMem->u32RdPtr)>HDR_VDEC_REORDER_NUM)
            frame_count_protect = TRUE;
    }

    if(u16Skip && u8tmpCurrentIndex && frame_count_protect && !bJump)
    {
        if((u8tmpCurrentIndex-HDR_VDEC_REORDER_NUM)<0)
            pstHDRShareMem->u32RdPtr=u8tmpCurrentIndex+HDR_MEM_ENTRY_NUM-HDR_VDEC_REORDER_NUM;
        else
            pstHDRShareMem->u32RdPtr=u8tmpCurrentIndex-HDR_VDEC_REORDER_NUM;
        bJump =TRUE;
    }
    else
    ++pstHDRShareMem->u32RdPtr;
    if (pstHDRShareMem->u32RdPtr >= HDR_MEM_ENTRY_NUM)
        pstHDRShareMem->u32RdPtr = 0;

    return TRUE;
}

MS_U32 getDolbyHDRMemWPtr(ST_KDRV_XC_HDR_DOLBY_MEMORY_FORMAT_EX* pstHDRShareMem)
{
    return pstHDRShareMem->u32WdPtr;
}

MS_BOOL incDolbyHDRMemWPtr(ST_KDRV_XC_HDR_DOLBY_MEMORY_FORMAT_EX* pstHDRShareMem)
{
    MS_U32 u32NextWPtr = pstHDRShareMem->u32WdPtr + 1;
    if (u32NextWPtr >= HDR_MEM_ENTRY_NUM)
        u32NextWPtr = 0;

    if (u32NextWPtr == pstHDRShareMem->u32RdPtr)
        return FALSE; // Queue is full

    pstHDRShareMem->u32WdPtr = u32NextWPtr;

    return TRUE;
}

MS_BOOL isCFDDone(ST_KDRV_XC_HDR_DOLBY_MEMORY_FORMAT_EX* pstHDRShareMem, MS_U32 u32Idx)
{
    if (u32Idx >= HDR_MEM_ENTRY_NUM)
    {
        printk("[ERROR] %s invalid idx %d\n", __FUNCTION__, u32Idx);
        return;
    }

    if (pstHDRShareMem->stEntries[u32Idx].u16CFDReady)
        return TRUE;
    else
        return FALSE;
}


MS_U16 getDolbyHDRMDMiuSel(ST_KDRV_XC_HDR_DOLBY_MEMORY_FORMAT_EX* pstHDRShareMem)
{
    return pstHDRShareMem->u16MDMiuSel;
}

MS_U16 getDolbyHDRLutMiuSel(ST_KDRV_XC_HDR_DOLBY_MEMORY_FORMAT_EX* pstHDRShareMem)
{
    return pstHDRShareMem->u16LutMiuSel;
}

void setDolbyHDRInfoEntry(ST_KDRV_XC_HDR_DOLBY_MEMORY_FORMAT_EX* pstHDRShareMem, MS_U32 u32Idx, ST_KDRV_XC_HDR_INFO_ENTRY *pstHDREntry)
{
    if (u32Idx >= HDR_MEM_ENTRY_NUM)
    {
        printk("[ERROR] %s invalid idx %d\n", __FUNCTION__, u32Idx);
        return FALSE;
    }

    pstHDRShareMem->stEntries[u32Idx].u16CFDReady = pstHDREntry->u16CFDReady;
    pstHDRShareMem->stEntries[u32Idx].u16Entry_Skip = pstHDREntry->u16Entry_Skip;
    pstHDRShareMem->stEntries[u32Idx].u32InputMDAddr = pstHDREntry->u32InputMDAddr;
    pstHDRShareMem->stEntries[u32Idx].u32HDRRegsetAddr = pstHDREntry->u32HDRRegsetAddr;
    pstHDRShareMem->stEntries[u32Idx].u32HDRRegsetSize = pstHDREntry->u32HDRRegsetSize;
    pstHDRShareMem->stEntries[u32Idx].u32HDRLutAddr = pstHDREntry->u32HDRLutAddr;
    pstHDRShareMem->stEntries[u32Idx].u32HDRLutSize = pstHDREntry->u32HDRLutSize;
    pstHDRShareMem->u64Reserved |= (((MS_U64)0x1) << u32Idx);
}

MS_BOOL getDolbyHDRInfoEntry(ST_KDRV_XC_HDR_DOLBY_MEMORY_FORMAT_EX* pstHDRShareMem, MS_U32 u32Idx, ST_KDRV_XC_HDR_INFO_ENTRY *pstHDREntry)
{
    if (u32Idx >= HDR_MEM_ENTRY_NUM)
    {
        printk("[ERROR] %s invalid idx %d\n", __FUNCTION__, u32Idx);
        return FALSE;
    }

    pstHDREntry->u16CFDReady = pstHDRShareMem->stEntries[u32Idx].u16CFDReady;
    pstHDREntry->u16Entry_Skip = pstHDRShareMem->stEntries[u32Idx].u16Entry_Skip;
    pstHDREntry->u32InputMDAddr = pstHDRShareMem->stEntries[u32Idx].u32InputMDAddr;
    pstHDREntry->u32HDRRegsetAddr = pstHDRShareMem->stEntries[u32Idx].u32HDRRegsetAddr;
    pstHDREntry->u32HDRRegsetSize = pstHDRShareMem->stEntries[u32Idx].u32HDRRegsetSize;
    pstHDREntry->u32HDRLutAddr = pstHDRShareMem->stEntries[u32Idx].u32HDRLutAddr;
    pstHDREntry->u32HDRLutSize = pstHDRShareMem->stEntries[u32Idx].u32HDRLutSize;

    return TRUE;
}

MS_U32 getDolbyHDRInputMDAddr(MS_U32 u32ShareMemBaseAddr, MS_U32 u32Idx)
{
    MS_U32 u32RetAddr = NULL;

    if (u32Idx >= HDR_MEM_ENTRY_NUM)
    {
        printk("[ERROR] %s invalid idx %d\n", __FUNCTION__, u32Idx);
        return u32RetAddr;
    }

    u32RetAddr = (u32ShareMemBaseAddr + HDR_MEM_COMMON_ENTRY_SIZE + u32Idx * HDR_INPUT_MD_SIZE);

    return u32RetAddr;
}

MS_U32 getDolbyHDRRegsetAddr(MS_U32 u32ShareMemBaseAddr, MS_U32 u32Idx)
{
    MS_U32 u32RetAddr = NULL;

    if (u32Idx >= HDR_MEM_ENTRY_NUM)
    {
        printk("[ERROR] %s invalid idx %d\n", __FUNCTION__, u32Idx);
        return u32RetAddr;
    }

    u32RetAddr = (u32ShareMemBaseAddr + HDR_MEM_COMMON_ENTRY_SIZE + HDR_TOTAL_INPUT_MD_SIZE + u32Idx * HDR_OUTPUT_REG_SIZE);

    return u32RetAddr;
}

MS_U32 getDolbyHDRLutAddr(MS_U32 u32ShareMemBaseAddr, MS_U32 u32Idx)
{
    MS_U32 u32RetAddr = NULL;

    if (u32Idx >= HDR_MEM_ENTRY_NUM)
    {
        printk("[ERROR] %s invalid idx %d\n", __FUNCTION__, u32Idx);
        return u32RetAddr;
    }

    if (u32Idx == 0)
    {
        u32RetAddr = u32ShareMemBaseAddr + HDR_MEM_COMMON_ENTRY_SIZE + HDR_TOTAL_INPUT_MD_SIZE + HDR_TOTAL_REGSET_SIZE;
    }
    else
    {
        u32RetAddr = u32ShareMemBaseAddr + HDR_MEM_COMMON_ENTRY_SIZE + HDR_TOTAL_INPUT_MD_SIZE + HDR_TOTAL_REGSET_SIZE + HDR_DV_3DLUT_NUM * BYTE_PER_WORD + (u32Idx - 1) * HDR_DV_TMO_NUM * BYTE_PER_WORD;
    }

    return u32RetAddr;
}

MS_U32 getDolbyHDRLutSize(MS_U32 u32Idx)
{
    MS_U32 u32RetSize = 0;

    if (u32Idx >= HDR_MEM_ENTRY_NUM)
    {
        printk("[ERROR] %s invalid idx %d\n", __FUNCTION__, u32Idx);
        return u32RetSize;
    }

    if (u32Idx == 0)
    {
        u32RetSize = HDR_DV_3DLUT_NUM * BYTE_PER_WORD;
    }
    else
    {
        u32RetSize = HDR_DV_TMO_NUM * BYTE_PER_WORD;
    }

    return u32RetSize;
}

MS_BOOL getDolbyHDRInputMDFormatInfo(MS_U8 *pu8Data, ST_KDRV_XC_HDR_INPUT_MD_FORMAT *pstFormatInfo)
{
    if (pu8Data != NULL)
    {
        pstFormatInfo->u8Version = *(pu8Data);
        pu8Data += 1;
        pstFormatInfo->u8CurrentIndex = *(pu8Data);
        pu8Data += 1;
        pstFormatInfo->u32DmLength = (*pu8Data) | (*(pu8Data + 1) << 8) | (*(pu8Data + 2) << 16) | (*(pu8Data + 3) << 24);
        pu8Data += 4;
        pstFormatInfo->u32DmAddr = (*pu8Data) | (*(pu8Data + 1) << 8) | (*(pu8Data + 2) << 16) | (*(pu8Data + 3) << 24);
        pu8Data += 4;
        pstFormatInfo->u8DmMiuNo = *(pu8Data);
        pu8Data += 1;
        pstFormatInfo->bEnableDM = *(pu8Data);
        pu8Data += 1;
        pstFormatInfo->u32ComposerLength = (*pu8Data) | (*(pu8Data + 1) << 8) | (*(pu8Data + 2) << 16) | (*(pu8Data + 3) << 24);
        pu8Data += 4;
        pstFormatInfo->u32ComposerAddr = (*pu8Data) | (*(pu8Data + 1) << 8) | (*(pu8Data + 2) << 16) | (*(pu8Data + 3) << 24);
        pu8Data += 4;
        pstFormatInfo->u8ComposerMiuNo = *(pu8Data);
        pu8Data += 1;
        pstFormatInfo->bEnableComposer = *(pu8Data);
        pu8Data += 1;
    }
    else
    {
        return FALSE;
    }
    return TRUE;
}
