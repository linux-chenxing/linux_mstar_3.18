///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2008 - 2009 MStar Semiconductor, Inc.
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

///////////////////////////////////////////////////////////////////////////////////////////////////
/// @file   mhal_gflip.h
/// @brief  MStar GFLIP Driver DDI HAL Level
/// @author MStar Semiconductor Inc.
/// @attention
/// <b>(OBSOLETED) <em></em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _HAL_GFLIP_H
#define _HAL_GFLIP_H

#ifdef _HAL_GFLIP_C
#define INTERFACE
#else
#define INTERFACE                               extern
#endif

//=============================================================================
// Includs
//=============================================================================

//=============================================================================
// Defines & Macros
//=============================================================================
#define MAX_GOP_SUPPORT                         3
#define MAX_GOP0_GWIN                           2
#define MAX_GOP1_GWIN                           2
#define MAX_GOP2_GWIN                           1
#define MAX_GOP3_GWIN                           0
#define MAX_GOP4_GWIN                           0
#define MAX_GOP_GWIN                            MAX_GOP0_GWIN

#define GFLIP_GOP_BANKOFFSET                    (0x3)
#define GFLIP_GOP4_BANK0                        (0x19)
#define GFLIP_GOP4_BANK1                        (0x1C)
#define GFLIP_GOP4_BANK2                        (0x1D)

#define GFLIP_ADDR_ALIGN_RSHIFT                 (0)

#define GFLIP_GOP_IDX_2G                        (0x0)
#define GFLIP_GOP_IDX_2GX                       (0x1)
#define GFLIP_GOP_IDX_1G                        (0x2)
#define GFLIP_GOP_IDX_1GX                       (0x3)
#define GFLIP_GOP_IDX_1GX0                      (0x4)
#define GFLIP_GOP_IDX_DWIN                      (0x5)
#define GFLIP_GOP_IDX_INVALID                   (0xFFFFFFFF)

#define GFLIP_GOP_DWIN_BANKID                   0xc

#define GFLIP_GOP_BANK_IDX_0                    (0x0)
#define GFLIP_GOP_BANK_IDX_1                    (0x1)
#define GFLIP_GOP_BANK_IDX_2                    (0x2)

#define GFLIP_GOP_DST_IPSUB                     0x0
#define GFLIP_GOP_DST_IPMAIN                    0x1
#define GFLIP_GOP_DST_OP0                       0x2
#define GFLIP_GOP_DST_VOP                       0x3
#define GFLIP_GOP_DST_VOPSUB                    0x4

#define GFLIP_MULTI_FLIP                        1

#define GS_REG_RESTORE_FUNCTION
#define GOP_MIU_IN_SC

#define GFLIP_GOP_TLB

#define GOP_TLB_PAGE_SIZE                      0x1000
#define PER_MIU_TLB_ENTRY_COUNT                8
#define TLB_PER_ENTRY_SIZE                     4
#define ADDRESSING_8BYTE_UNIT                  8

//=============================================================================
// HAL Driver Function
//=============================================================================

INTERFACE MS_BOOL   MHal_GFLIP_IntEnable(MS_U32 u32GopIdx, MS_BOOL bEnable);
INTERFACE MS_BOOL   MHal_GFLIP_IsVSyncInt(MS_U32 u32GopIdx);
INTERFACE MS_U32    MHal_GFLIP_GetIntGopIdx(void);
INTERFACE MS_BOOL MHal_GFLIP_IsGOPACK(MS_U32 u32GopIdx);
INTERFACE void      MHal_GFLIP_Fire(MS_U32 u32GopBitMask);
INTERFACE MS_BOOL MHal_GFLIP_SetFlipToGop(MS_U32 u32GopIdx, MS_U32 u32GwinIdx, MS_PHY64 u32MainAddr, MS_PHY64 u32SubAddr, MS_BOOL bForceWriteIn);
INTERFACE MS_BOOL MHal_GFLIP_SetTLBFlipToGop(MS_U32 u32GopIdx, MS_U32 u32GwinIdx, MS_PHY64 u32MainAddr, MS_PHY64 u32SubAddr, MS_BOOL bForceWriteIn);
#ifdef	GFLIP_MULTI_FLIP
INTERFACE MS_BOOL MHal_GFLIP_SetMultiFlipToGop(MS_GFLIP_MULTIINFO* pMultiFlipInfo, MS_BOOL bForceWriteIn);
INTERFACE MS_BOOL MHal_GFLIP_SetTLBMultiFlipToGop(MS_TLB_GFLIP_MULTIINFO* pTLBMultiFlipInfo,MS_BOOL bForceWriteIn);
INTERFACE void MHal_GFLIP_SetMultiFire(MS_U32 u32GopBitMask,MS_BOOL bForceWriteIn);
#endif
INTERFACE MS_BOOL   MHal_GFLIP_ClearDWINIRQ(GFLIP_DWININT_INFO *pGFlipDWinIntInfo);

INTERFACE MS_BOOL   MHal_GFLIP_HandleVsyncLimitation(MS_U32 u32GopIdx);
INTERFACE MS_BOOL   MHal_GFLIP_RestoreFromVsyncLimitation(MS_U32 u32GopIdx);

INTERFACE MS_BOOL   MHal_GFLIP_IsTagIDBack(MS_U16 u16TagId);
INTERFACE MS_BOOL   MHal_GFLIP_VECaptureEnable(MS_BOOL bEnable);
INTERFACE MS_BOOL   MHal_GFLIP_CheckVEReady(void);
INTERFACE MS_U16    MHal_GFLIP_GetGopDst(MS_U32 u32GopIdx);
INTERFACE MS_U8     MHal_GFLIP_GetFrameIdx(void);
INTERFACE MS_U32    MHal_GFLIP_GetValidGWinIDPerGOPIdx(MS_U32 u32GopIdx, MS_U32 u32GwinIdx);
INTERFACE MS_U8     MHal_GFLIP_GetBankOffset(MS_U32 u32GopIdx, MS_U16 u16BankIdx);
INTERFACE void MHal_GFLIP_WriteGopReg(MS_U32 u32GopIdx, MS_U16 u16BankIdx, MS_U16 u16Addr, MS_U16 u16Val, MS_U16 u16Mask);
INTERFACE void MHal_GFLIP_ReadGopReg(MS_U32 u32GopIdx, MS_U16 u16BankIdx, MS_U16 u16Addr, MS_U16* u16Val);



#undef INTERFACE
#endif //_HAL_GFLIP_H

