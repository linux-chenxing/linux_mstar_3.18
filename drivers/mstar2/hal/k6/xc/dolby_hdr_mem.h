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
#define HDR_OUTPUT_REG_SIZE (430 << 5)
#define HDR_TOTAL_REGSET_SIZE (HDR_OUTPUT_REG_SIZE * HDR_MEM_ENTRY_NUM)
#define HDR_DV_3DLUT_NUM 4944
#define HDR_DV_TMO_NUM 512
#define HDR_VDEC_REORDER_NUM 10
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
void setDolbyHDRInfoEntry(ST_KDRV_XC_HDR_DOLBY_MEMORY_FORMAT_EX* pstHDRShareMem, MS_U32 u32Idx, ST_KDRV_XC_HDR_INFO_ENTRY *pstHDREntry);
MS_BOOL getDolbyHDRInfoEntry(ST_KDRV_XC_HDR_DOLBY_MEMORY_FORMAT_EX* pstHDRShareMem, MS_U32 u32Idx, ST_KDRV_XC_HDR_INFO_ENTRY *pstHDREntry);
MS_U32 getDolbyHDRInputMDAddr(MS_U32 u32ShareMemBaseAddr, MS_U32 u32Idx);
MS_U32 getDolbyHDRRegsetAddr(MS_U32 u32ShareMemBaseAddr, MS_U32 u32Idx);
MS_U32 getDolbyHDRLutAddr(MS_U32 u32ShareMemBaseAddr, MS_U32 u32Idx);
MS_U32 getDolbyHDRLutSize(MS_U32 u32Idx);
MS_BOOL getDolbyHDRInputMDFormatInfo(MS_U8 *pu8Data, ST_KDRV_XC_HDR_INPUT_MD_FORMAT *pstFormatInfo);
#endif // _DOLBY_HDR_MEM_H_
