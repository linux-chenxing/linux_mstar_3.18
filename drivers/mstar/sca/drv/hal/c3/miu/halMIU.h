////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2008 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (¡§MStar Confidential Information¡¨) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _HAL_MIU_H_
#define _HAL_MIU_H_


//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define _FUNC_NOT_USED()        do {} while ( 0 )

#define MIU_MAX_DEVICE           (2)
//Max MIU Group number
#define MIU_MAX_GROUP            (4)
#define MIU_MAX_GP_CLIENT        (16)
#define MIU_MAX_TBL_CLIENT       (MIU_MAX_GROUP*MIU_MAX_GP_CLIENT)
#define MIU_PAGE_SHIFT           (13) //Unit for MIU protect
#define MIU_PROTECT_ADDRESS_UNIT (0x20) //Unit for MIU hitted address

#ifndef BIT0
#define BIT0  0x0001
#define BIT1  0x0002
#define BIT2  0x0004
#define BIT3  0x0008
#define BIT4  0x0010
#define BIT5  0x0020
#define BIT6  0x0040
#define BIT7  0x0080
#define BIT8  0x0100
#define BIT9  0x0200
#define BIT10 0x0400
#define BIT11 0x0800
#define BIT12 0x1000
#define BIT13 0x2000
#define BIT14 0x4000
#define BIT15 0x8000
#endif

#define MIU_OPM_R_MASK 0x0667
#define MIU_OPM_W_MASK 0x0666
#define MIU_MVD_R_MASK 0x06F6
#define MIU_MVD_W_MASK 0x06F7

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

#define MIU_GET_CLIENT_POS(x)       (x & 0x0F)
#define MIU_GET_CLIENT_GROUP(x)     ((x & 0xF0) >> 4)

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

typedef enum
{
  E_MIU_BLOCK_0 = 0,
  E_MIU_BLOCK_1,
  E_MIU_BLOCK_2,
  E_MIU_BLOCK_3,
  E_MIU_BLOCK_4,
} MIU_BLOCK_ID;

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------
void    HAL_MIU_SetIOMapBase(MS_U32 u32Base);
void    HAL_MIU_SetPMIOMapBase(MS_U32 u32Base);
MS_BOOL HAL_MIU_WriteRegBit(MS_U32 u32RegAddr, MS_U8 u8Mask, MS_BOOL bEnable);
MS_U32  HAL_MIU_ProtectAlign(void);
MS_BOOL HAL_MIU_Dram_Size(MS_U8 MiuID, MS_U8 DramSize);
MS_BOOL HAL_MIU_Protect(MS_U8 u8Blockx, MS_U8 *pu8ProtectId, MS_U32 u32Start,
                        MS_U32 u32End, MS_BOOL  bSetFlag);
MS_BOOL HAL_MIU_SetSsc(MS_U8 u8MiuDev, MS_U16 u16Fmodulation, MS_U16 u16FDeviation, MS_BOOL bEnable);
MS_BOOL HAL_MIU_IsInitMiu1(void);
MS_BOOL HAL_MIU_SelMIU(eMIU_SelType eType, eMIUClientID eClientID);
MS_BOOL HAL_MIU_GetProtectInfo(MS_U8 u8MiuDev, MIU_PortectInfo *pInfo);
MS_BOOL HAL_MIU_SetGroupPriority(MS_U8 u8MiuDev, MIU_GroupPriority sPriority);
MS_BOOL HAL_MIU_SetHPriorityMask(MS_U8 u8MiuDev, eMIUClientID eClientID, MS_BOOL bMask);

MS_U8* HAL_MIU_GetDefaultClientID_KernelProtect(void);

MS_U16  HAL_MIU_GET_MUX(void);
MS_U16  HAL_MIU_IsI64Mode(void);
MS_U16  HAL_MIU_GET_Group0(void);
MS_U16  HAL_MIU_GET_Group1(void);
MS_U16  HAL_MIU_GET_Group2(void);
MS_S16 HAL_MIU_GetClientInfo(MS_U8 u8MiuDev, eMIUClientID eClientID);

void HAL_MIU_MaskReq(MS_U8 u8Miu, eMIUClientID u16ClientID);
void HAL_MIU_UnMaskReq(MS_U8 u8Miu, eMIUClientID u16ClientID);
void HAL_MIU_Mask_Req_OPM_R(MS_U8 u8Mask, MS_U8 u8Miu);
void HAL_MIU_Mask_Req_DNRB_R(MS_U8 u8Mask, MS_U8 u8Miu);
void HAL_MIU_Mask_Req_DNRB_W(MS_U8 u8Mask, MS_U8 u8Miu);
void HAL_MIU_Mask_Req_DNRB_RW(MS_U8 u8Mask, MS_U8 u8Miu);
void HAL_MIU_Mask_Req_SC_RW(MS_U8 u8Mask, MS_U8 u8Miu);
void HAL_MIU_Mask_Req_MVOP_R(MS_U8 u8Mask, MS_U8 u8Miu);
void HAL_MIU_Mask_Req_MVD_R(MS_U8 u8Mask, MS_U8 u8Miu);
void HAL_MIU_Mask_Req_MVD_W(MS_U8 u8Mask, MS_U8 u8Miu);
void HAL_MIU_Mask_Req_MVD_RW(MS_U8 u8Mask, MS_U8 u8Miu);
void HAL_MIU_Mask_Req_AUDIO_RW(MS_U8 u8Mask, MS_U8 u8Miu);
void HAL_MIU_VOP_SwitchMIU(MS_U8 u8MiuID);

#endif // _HAL_MIU_H_

