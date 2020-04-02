//<MStar Software>
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2010 - 2012 MStar Semiconductor, Inc.
// This program is free software.
// You can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation;
// either version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with this program;
// if not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
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
#include "dolby_hdr_mem.h"
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
