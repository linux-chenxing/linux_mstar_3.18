////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2008-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// ("MStar Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////



//==============================================================================
/// [MsAce_LIB_Group_DTV1.h]
/// Date: 20090414
/// Descriptions:
//==============================================================================


#ifndef MSACE_LIB_GROUP_DTV1_H
#define MSACE_LIB_GROUP_DTV1_H

#include "ace_hwreg_utility2.h"
#include "hwreg_ace.h"

/////////////// Chakra2 define start

#ifndef BOOL
#define BOOL    MS_BOOL
#else
#undef  BOOL
#define BOOL    MS_BOOL
#endif

#ifndef BYTE
#define BYTE    MS_U8
#else
#undef  BYTE
#define BYTE    MS_U8

#endif

#ifndef WORD
#define WORD    MS_U16
#else
#undef  WORD
#define WORD    MS_U16
#endif

#ifndef DWORD
#define DWORD   MS_U32
#else
#undef  DWORD
#define DWORD   MS_U32
#endif

#ifndef U8
#define U8      MS_U8
#else
#undef  U8
#define U8      MS_U8
#endif
#define code


#define BK_SELECT_00                BK_SCALER_BASE
#define REG_BANK_VOP                0x10
#define REG_BANK_S_VOP              0x0F
#define BK_REG_L( x, y )            ((x) | (((y) << 1)))
#define L_BK_VOP(x)                 BK_REG_L(BK_SCALER_BASE,x)
#define L_BK_SC1_VOP(x)             BK_REG_L(BK_SCALER_BASE,x)
#define L_BK_SC2_VOP(x)             BK_REG_L(BK_SCALER_BASE,x)
//////////////// Chakra2 define end


#define XDATA

#define UNAVAILABLE             0xFF

#define REG_ADDR_SC_BANK_SEL    BK_SELECT_00

#define BANK_CM_MAIN            REG_BANK_VOP
#define BANK_CM_SUB             REG_BANK_S_VOP
#define BANK_CM_SC1_MAIN        ( 0x80 + REG_BANK_VOP   )
#define BANK_CM_SC1_SUB         ( 0x80 + REG_BANK_S_VOP )
#define BANK_CM_SC2_MAIN        ( 0x40 + REG_BANK_VOP   )
#define BANK_CM_SC2_SUB         ( 0x40 + REG_BANK_S_VOP )

#define REG_ADDR_CM_MAIN        L_BK_VOP(0x26)
#define REG_ADDR_CM_SUB         L_BK_VOP(0x1D)
#define REG_ADDR_CM_SC1_MAIN    L_BK_SC1_VOP(0x26)
#define REG_ADDR_CM_SC1_SUB     L_BK_SC1_VOP(0x1D)
#define REG_ADDR_CM_SC2_MAIN    L_BK_SC2_VOP(0x26)
#define REG_ADDR_CM_SC2_SUB     L_BK_SC2_VOP(0x1D)

#define REG_ADDR_CM_CTL_MAIN    L_BK_VOP(0x2F)
#define REG_ADDR_CM_CTL_SUB     L_BK_VOP(0x26)
#define REG_ADDR_CM_CTL_SC1_MAIN  L_BK_SC1_VOP(0x2F)
#define REG_ADDR_CM_CTL_SC1_SUB   L_BK_SC1_VOP(0x26)
#define REG_ADDR_CM_CTL_SC2_MAIN  L_BK_SC2_VOP(0x2F)
#define REG_ADDR_CM_CTL_SC2_SUB   L_BK_SC2_VOP(0x26)

#define ENABLE_CM_CTL1          (0x35)
#define ENABLE_CM_CTL2          (0x30)
#define MASK_CM_CTL             (0x37)
#define MASK_CM_CTL_EN          (0x10)
#define MASK_CM_CTL_RB_RANGE    (0x05)

#define DECLARA_CMCTL_VARIABLE              U8 CTLVal;
#if SCALER_REGISTER_SPREAD
#define DECLARA_BANK_VARIABLE               MS_U32 u32Bank = 0;
#define BACKUP_SC_BANK                      u32Bank = u32Bank;
#define RECOVER_SC_BANK
#define ACE_WRITE_CM(addr, data)	        SC_W2BYTE(((DWORD)addr | (u32Bank << 8)),data)
#define ACE_WRITE_CM_CTL(addr, data, mask)	SC_W2BYTEMSK(((DWORD)addr | (u32Bank << 8)),data,mask)
#else
#define DECLARA_BANK_VARIABLE               U8 u8Bank;
#define BACKUP_SC_BANK                      {u8Bank = GET_SC_BANK();}
#define RECOVER_SC_BANK                     {SET_SC_BANK(u8Bank);}
#define ACE_WRITE_CM(addr, data)		    MDrv_Write2Byte(L_BK_VOP((addr)/2), (data))
#define ACE_WRITE_CM_CTL(addr, data, mask)	MDrv_WriteByteMask(addr, data, mask)
#endif

// For scaler bank switch
#if SCALER_REGISTER_SPREAD
#define GET_SC_BANK()
#define SET_SC_BANK(bank)       u32Bank = bank;
#else
#define GET_SC_BANK()           MDrv_ReadByte(REG_ADDR_SC_BANK_SEL)
#define SET_SC_BANK(bank)       MDrv_WriteByte(REG_ADDR_SC_BANK_SEL, bank)
#endif
// Control sin&cos function
#define USE_ACE_INTERNAL_SIN_COS	0
#define sin     clampsin
#define cos     clampcos

#endif
