///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2006 - 2007 MStar Semiconductor, Inc.
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
///
/// @file   Mhal_mtlb.h
/// @author MStar Semiconductor Inc.
/// @brief  MTLB Driver Interface
///////////////////////////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// Linux Mhal_miu.h define start
// -----------------------------------------------------------------------------
#ifndef _HAL_MIU_H_
#define _HAL_MIU_H_

#include "chip_setup.h"

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define _FUNC_NOT_USED()        do {} while ( 0 )

#define MIU_MAX_DEVICE           (1)
//Max MIU Group number
#define MIU_MAX_GROUP            (8)
#define MIU_MAX_GP_CLIENT        (16)
#define MIU_MAX_TBL_CLIENT       (MIU_MAX_GROUP*MIU_MAX_GP_CLIENT)
#define MIU_PAGE_SHIFT           (13) //Unit for MIU protect
#define MIU_SLIT_SHIFT           (20) //Unit for MIU slit
#define MIU_FLAGSRAM_BASE_SHIFT  (14) //Unit for flag sram base
#define MIU_PROTECT_ADDRESS_UNIT (0x20UL) //Unit for MIU hitted address
#define MIU_MAX_PROTECT_BLOCK    (5)
#define MIU_MAX_PROTECT_ID       (16)
#define MIU_BLOCK0_CLIENT_NUMBER (16)
#define MIU_BLOCK1_CLIENT_NUMBER (16)
#define MIU_BLOCK2_CLIENT_NUMBER (16)
#define MIU_BLOCK3_CLIENT_NUMBER (16)
#define MIU_BLOCK4_CLIENT_NUMBER (16)
#define MIU_BLOCK5_CLIENT_NUMBER (16)

#define MIU_FLAGSRAM_BASE           0x00000000
#define MIU_FLAGSRAM_BUS_BASE       0x20000000
#define MIU1_FLAGSRAM_BASE          0x00000000
#define MIU1_FLAGSRAM_BUS_BASE      0xA0000000

#ifndef BIT0
#define BIT0  0x0001UL
#define BIT1  0x0002UL
#define BIT2  0x0004UL
#define BIT3  0x0008UL
#define BIT4  0x0010UL
#define BIT5  0x0020UL
#define BIT6  0x0040UL
#define BIT7  0x0080UL
#define BIT8  0x0100UL
#define BIT9  0x0200UL
#define BIT10 0x0400UL
#define BIT11 0x0800UL
#define BIT12 0x1000UL
#define BIT13 0x2000UL
#define BIT14 0x4000UL
#define BIT15 0x8000UL
#endif

#define MIU_OPM_R_MASK 0x0667UL
#define MIU_OPM_W_MASK 0x0666UL
#define MIU_MVD_R_MASK 0x06F6UL
#define MIU_MVD_W_MASK 0x06F7UL

//$ MIU0 Request Mask functions
#define _MaskMiuReq_OPM_R( m )     HAL_MIU_WriteRegBit(MIU_RQ1H_MASK, m, BIT1)

#define _MaskMiuReq_DNRB_W( m )    HAL_MIU_WriteRegBit(MIU_RQ1H_MASK, m, BIT2)
#define _MaskMiuReq_DNRB_R( m )    HAL_MIU_WriteRegBit(MIU_RQ1H_MASK, m, BIT3)
#define _MaskMiuReq_DNRB_RW( m )   HAL_MIU_WriteRegBit(MIU_RQ1H_MASK, m, BIT2|BIT3)

#define _MaskMiuReq_SC_RW( m )     HAL_MIU_WriteRegBit(MIU_RQ1H_MASK, m, BIT1|BIT2|BIT3)

#define _MaskMiuReq_MVOP_R( m )    HAL_MIU_WriteRegBit(MIU_RQ1L_MASK, m, BIT3)

#define _MaskMiuReq_MVD_R( m )     do { HAL_MIU_WriteRegBit(MIU_RQ3L_MASK, m, BIT4); HAL_MIU_WriteRegBit(MIU_RQ3L_MASK, m, BIT5); } while(0)
#define _MaskMiuReq_MVD_W( m )     do { HAL_MIU_WriteRegBit(MIU_RQ3L_MASK, m, BIT4); HAL_MIU_WriteRegBit(MIU_RQ3L_MASK, m, BIT5); } while(0)
#define _MaskMiuReq_MVD_RW( m )    do { _MaskMiuReq_MVD_R( m ); _MaskMiuReq_MVD_W( m ); } while (0)

#define _MaskMiuReq_AUDIO_RW( m )  _FUNC_NOT_USED()


//$ MIU1 Request Mask functions
#define _MaskMiu1Req_OPM_R( m )     HAL_MIU_WriteRegBit(MIU_RQ1H_MASK, m, BIT1)

#define _MaskMiu1Req_DNRB_W( m )    HAL_MIU_WriteRegBit(MIU_RQ1H_MASK, m, BIT2)
#define _MaskMiu1Req_DNRB_R( m )    HAL_MIU_WriteRegBit(MIU_RQ1H_MASK, m, BIT3)
#define _MaskMiu1Req_DNRB_RW( m )   HAL_MIU_WriteRegBit(MIU_RQ1H_MASK, m, BIT2|BIT3)

#define _MaskMiu1Req_SC_RW( m )     HAL_MIU_WriteRegBit(MIU_RQ1H_MASK, m, BIT1|BIT2|BIT3)

#define _MaskMiu1Req_MVOP_R( m )    HAL_MIU_WriteRegBit(MIU_RQ1L_MASK, m, BIT3)

#define _MaskMiu1Req_MVD_R( m )     do { HAL_MIU_WriteRegBit(MIU_RQ3L_MASK, m, BIT4); HAL_MIU_WriteRegBit(MIU_RQ3L_MASK, m, BIT5); } while(0)
#define _MaskMiu1Req_MVD_W( m )     do { HAL_MIU_WriteRegBit(MIU_RQ3L_MASK, m, BIT4); HAL_MIU_WriteRegBit(MIU_RQ3L_MASK, m, BIT5); } while(0)
#define _MaskMiu1Req_MVD_RW( m )    do { _MaskMiuReq_MVD_R( m ); _MaskMiuReq_MVD_W( m ); } while (0)

#define _MaskMiu1Req_AUDIO_RW( m )  _FUNC_NOT_USED()

#define MIU_GET_CLIENT_POS(x)       (x & 0x0FUL)
#define MIU_GET_CLIENT_GROUP(x)     ((x & 0xF0UL) >> 4)

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
typedef enum
{
  E_MIU_BLOCK_0 = 0,
  E_MIU_BLOCK_1,
  E_MIU_BLOCK_2,
  E_MIU_BLOCK_3,
  E_MIU_SLIT_0 = 16,
  E_MIU_BLOCK_NUM,
} MIU_BLOCK_ID;

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------
MS_U8* HAL_MIU_GetDefaultClientID_KernelProtect(void);
MS_BOOL HAL_MIU_Protect(MS_U8 u8Blockx, MS_U8 *pu8ProtectId, MS_U32 u32BusStart,
                        MS_U32 u32BusEnd, MS_BOOL  bSetFlag);
MS_BOOL HAL_MIU_ParseOccupiedResource(void);

#ifdef CONFIG_MP_CMA_PATCH_DEBUG_STATIC_MIU_PROTECT
MS_BOOL HAL_MIU_GetProtectInfo(MS_U8 u8MiuDev, MIU_PortectInfo *pInfo);
#endif
MS_BOOL HAL_MIU_Save(void);
MS_BOOL HAL_MIU_Restore(void);
MS_BOOL HAL_MIU_SlitInit(void);
MS_BOOL HAL_MIU_SetSlitRange(
    MS_U8    u8Blockx,
    MS_U8    *pu8ProtectId,
    MS_PHY   u64BusStart,
    MS_PHY   u64BusEnd,
    MS_BOOL  bSetFlag
);
MS_BOOL HAL_MIU_Slits(
    MS_U8 u8Blockx,
    MS_PHY u64SlitsStart,
    MS_PHY u64SlitsEnd,
    MS_BOOL bSetFlag
);
void HAL_MIU_DumpMem(phys_addr_t addr, MS_U32 len);
void HAL_MIU_FlagSramDump(MS_U8 u8MiuSel);
#endif // _HAL_MIU_H_

