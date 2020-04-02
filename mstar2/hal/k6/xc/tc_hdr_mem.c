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
#include "mdrv_mstypes.h"
#include "tc_hdr_mem.h"
#include "mhal_xc.h"

#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/string.h>
#include <linux/poll.h>
#include <linux/wait.h>
#if defined(CONFIG_MIPS)
#elif defined(CONFIG_ARM) || defined(CONFIG_ARM64)
#include <asm/io.h>
#endif
#include <asm/uaccess.h>
//#include <linux/sched.h>
#include <linux/version.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/jiffies.h>
#include <linux/slab.h>
#include <linux/time.h>
//#include <linux/sched.h>
#include <linux/proc_fs.h>
//#include <linux/kthread.h>

void initTCHDRMem(ST_KDRV_XC_HDR_TC_MEMORY_FORMAT_EX* pstHDRShareMem)
{
    memset((void *)pstHDRShareMem, 0, sizeof(ST_KDRV_XC_HDR_TC_MEMORY_FORMAT_EX));
    pstHDRShareMem->u16Version = TC_HDR_MEM_VER;
    pstHDRShareMem->u16CommInfoSize = TC_HDR_COMMON_INFO_SIZE;
}

void flushTCHDRMemPtr(ST_KDRV_XC_HDR_TC_MEMORY_FORMAT_EX* pstHDRShareMem)
{
    pstHDRShareMem->u32RdPtr = pstHDRShareMem->u32WdPtr = 0;
}

MS_BOOL isTCHDRInfoEntryEmpty(ST_KDRV_XC_HDR_TC_MEMORY_FORMAT_EX* pstHDRShareMem)
{
    return (pstHDRShareMem->u32RdPtr == pstHDRShareMem->u32WdPtr);
}

MS_BOOL isTCHDRInfoEntryFull(ST_KDRV_XC_HDR_TC_MEMORY_FORMAT_EX* pstHDRShareMem)
{
    MS_U32 u32NextWPtr = pstHDRShareMem->u32WdPtr + 1;
    if (u32NextWPtr >= TC_HDR_MEM_ENTRY_NUM)
        u32NextWPtr = 0;

    if (u32NextWPtr == pstHDRShareMem->u32RdPtr)
        return TRUE; // Queue is full
    else
        return FALSE;
}

MS_U32 getTCHDRMemRPtr(ST_KDRV_XC_HDR_TC_MEMORY_FORMAT_EX* pstHDRShareMem)
{
    return pstHDRShareMem->u32RdPtr;
}

MS_BOOL incTCHDRMemRPtr(ST_KDRV_XC_HDR_TC_MEMORY_FORMAT_EX* pstHDRShareMem)
{
    if (pstHDRShareMem->u32RdPtr == pstHDRShareMem->u32WdPtr)
        return FALSE; // Queue is empty

    ++pstHDRShareMem->u32RdPtr;
    if (pstHDRShareMem->u32RdPtr >= TC_HDR_MEM_ENTRY_NUM)
        pstHDRShareMem->u32RdPtr = 0;

    return TRUE;
}

MS_U32 getTCHDRMemWPtr(ST_KDRV_XC_HDR_TC_MEMORY_FORMAT_EX* pstHDRShareMem)
{
    return pstHDRShareMem->u32WdPtr;
}

MS_BOOL incTCHDRMemWPtr(ST_KDRV_XC_HDR_TC_MEMORY_FORMAT_EX* pstHDRShareMem)
{
    MS_U32 u32NextWPtr = pstHDRShareMem->u32WdPtr + 1;
    if (u32NextWPtr >= TC_HDR_MEM_ENTRY_NUM)
        u32NextWPtr = 0;

    if (u32NextWPtr == pstHDRShareMem->u32RdPtr)
        return FALSE; // Queue is full

    pstHDRShareMem->u32WdPtr = u32NextWPtr;

    return TRUE;
}

MS_BOOL isCFDTCHDone(ST_KDRV_XC_HDR_TC_MEMORY_FORMAT_EX* pstHDRShareMem, MS_U32 u32Idx)
{
    if (u32Idx >= TC_HDR_MEM_ENTRY_NUM)
    {
        printk("[ERROR] %s invalid idx %d\n", __FUNCTION__, u32Idx);
        return FALSE;
    }

    if (pstHDRShareMem->stEntries[u32Idx].u32CFDReady)
        return TRUE;
    else
        return FALSE;
}


MS_U16 getTCHDRMDMiuSel(ST_KDRV_XC_HDR_TC_MEMORY_FORMAT_EX* pstHDRShareMem)
{
    return pstHDRShareMem->u16MDMiuSel;
}

MS_U16 getTCHDRLutMiuSel(ST_KDRV_XC_HDR_TC_MEMORY_FORMAT_EX* pstHDRShareMem)
{
    printk("[ERROR] %s invalid , TCH not support\n", __FUNCTION__);
    return pstHDRShareMem->u16LutMiuSel;
}

void setTCHDRInfoEntry(ST_KDRV_XC_HDR_TC_MEMORY_FORMAT_EX* pstHDRShareMem, MS_U32 u32Idx, ST_KDRV_XC_TC_HDR_INFO_ENTRY *pstHDREntry)
{
    if (u32Idx >= TC_HDR_MEM_ENTRY_NUM)
    {
        printk("[ERROR] %s invalid idx %d\n", __FUNCTION__, u32Idx);
        return;
    }

    pstHDRShareMem->stEntries[u32Idx].u32CFDReady = pstHDREntry->u32CFDReady;
    pstHDRShareMem->stEntries[u32Idx].u32InputMDAddr = pstHDREntry->u32InputMDAddr;
    pstHDRShareMem->stEntries[u32Idx].u32HDRRegsetAddr = pstHDREntry->u32HDRRegsetAddr;
    pstHDRShareMem->stEntries[u32Idx].u32HDRRegsetSize = pstHDREntry->u32HDRRegsetSize;
    pstHDRShareMem->stEntries[u32Idx].u32HDRLutAddr = pstHDREntry->u32HDRLutAddr;
    pstHDRShareMem->stEntries[u32Idx].u32HDRLutSize = pstHDREntry->u32HDRLutSize;
    pstHDRShareMem->u64Reserved |= (((MS_U64)0x1) << u32Idx);
}

MS_BOOL getTCHDRInfoEntry(ST_KDRV_XC_HDR_TC_MEMORY_FORMAT_EX* pstHDRShareMem, MS_U32 u32Idx, ST_KDRV_XC_TC_HDR_INFO_ENTRY *pstHDREntry)
{
    if (u32Idx >= TC_HDR_MEM_ENTRY_NUM)
    {
        printk("[ERROR] %s invalid idx %d\n", __FUNCTION__, u32Idx);
        return FALSE;
    }

    pstHDREntry->u32CFDReady = pstHDRShareMem->stEntries[u32Idx].u32CFDReady;
    pstHDREntry->u32InputMDAddr = pstHDRShareMem->stEntries[u32Idx].u32InputMDAddr;
    pstHDREntry->u32HDRRegsetAddr = pstHDRShareMem->stEntries[u32Idx].u32HDRRegsetAddr;
    pstHDREntry->u32HDRRegsetSize = pstHDRShareMem->stEntries[u32Idx].u32HDRRegsetSize;
    pstHDREntry->u32HDRLutAddr = pstHDRShareMem->stEntries[u32Idx].u32HDRLutAddr;
    pstHDREntry->u32HDRLutSize = pstHDRShareMem->stEntries[u32Idx].u32HDRLutSize;

    return TRUE;
}

MS_U32 getTCHDRInputMDAddr(MS_U32 u32ShareMemBaseAddr, MS_U32 u32Idx)
{
    MS_U32 u32RetAddr = NULL;

    if (u32Idx >= TC_HDR_MEM_ENTRY_NUM)
    {
        printk("[ERROR] %s invalid idx %d\n", __FUNCTION__, u32Idx);
        return u32RetAddr;
    }

    u32RetAddr = (u32ShareMemBaseAddr + TC_HDR_MEM_COMMON_ENTRY_SIZE + u32Idx * TC_HDR_INPUT_MD_SIZE);

    return u32RetAddr;
}

MS_U32 getTCHDRRegsetAddr(MS_U32 u32ShareMemBaseAddr, MS_U32 u32Idx)
{
    MS_U32 u32RetAddr = NULL;

    if (u32Idx >= TC_HDR_MEM_ENTRY_NUM)
    {
        printk("[ERROR] %s invalid idx %d\n", __FUNCTION__, u32Idx);
        return u32RetAddr;
    }

    u32RetAddr = (u32ShareMemBaseAddr + TC_HDR_MEM_COMMON_ENTRY_SIZE + TC_HDR_TOTAL_INPUT_MD_SIZE + u32Idx * TC_HDR_OUTPUT_REG_SIZE);

    return u32RetAddr;
}

MS_U32 getTCHDRLutAddr(MS_U32 u32ShareMemBaseAddr, MS_U32 u32Idx)
{
    printk("[ERROR] %s invalid , TCH not support\n", __FUNCTION__);
    return 0;
}

MS_U32 getTCHDRLutSize(MS_U32 u32Idx)
{
    printk("[ERROR] %s invalid , TCH not support\n", __FUNCTION__);
    return 0;
}

MS_BOOL getTCHDRInputMDFormatInfo(MS_U8 *pu8Data, ST_KDRV_XC_TC_HDR_INPUT_MD_FORMAT *pstFormatInfo)
{
    if (pu8Data != NULL)
    {
        pstFormatInfo->u8Version = *(pu8Data);
        pu8Data += 1;
        pstFormatInfo->u8CurrentIndex = *(pu8Data);
        pu8Data += 1;
        pstFormatInfo->u8T35Enable = *(pu8Data);
        pu8Data += 1;
        pstFormatInfo->u8T35MiuNo = *(pu8Data);
        pu8Data += 1;
        pstFormatInfo->u32T35Length = (*pu8Data) | (*(pu8Data + 1) << 8) | (*(pu8Data + 2) << 16) | (*(pu8Data + 3) << 24);
        pu8Data += 4;
        pstFormatInfo->u32T35Addr = (*pu8Data) | (*(pu8Data + 1) << 8) | (*(pu8Data + 2) << 16) | (*(pu8Data + 3) << 24);
        pu8Data += 4;
        pstFormatInfo->u8CREnable = *(pu8Data);
        pu8Data += 1;
        pstFormatInfo->u8CRMiuNo = *(pu8Data);
        pu8Data += 1;
        pstFormatInfo->u32CRLength = (*pu8Data) | (*(pu8Data + 1) << 8) | (*(pu8Data + 2) << 16) | (*(pu8Data + 3) << 24);
        pu8Data += 4;
        pstFormatInfo->u32CRAddr = (*pu8Data) | (*(pu8Data + 1) << 8) | (*(pu8Data + 2) << 16) | (*(pu8Data + 3) << 24);
        pu8Data += 4;
    }
    else
    {
        return FALSE;
    }
    return TRUE;
}
