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
#ifndef _HWREG_UTILITY_H_
#define _HWREG_UTILITY_H_


//#include "MsCommon.h"

#define _PK_L_(bank, addr)   (((MS_U16)(bank) << 8) | (MS_U16)((addr)*2))
#define _PK_H_(bank, addr)   (((MS_U16)(bank) << 8) | (MS_U16)((addr)*2+1))

//=============================================================

//----------------------------------------------
//  BK0F: VOP
//----------------------------------------------
#define REG_SC_BK0F_18_L      _PK_L_(0x0F, 0x18)
#define REG_SC_BK0F_18_H      _PK_H_(0x0F, 0x18)


//----------------------------------------------
//  BK10: VOP
//----------------------------------------------
#define REG_SC_BK10_00_L      _PK_L_(0x10, 0x00)
#define REG_SC_BK10_00_H      _PK_H_(0x10, 0x00)
#define REG_SC_BK10_01_L      _PK_L_(0x10, 0x01)
#define REG_SC_BK10_01_H      _PK_H_(0x10, 0x01)
#define REG_SC_BK10_02_L      _PK_L_(0x10, 0x02)
#define REG_SC_BK10_02_H      _PK_H_(0x10, 0x02)
#define REG_SC_BK10_03_L      _PK_L_(0x10, 0x03)
#define REG_SC_BK10_03_H      _PK_H_(0x10, 0x03)
#define REG_SC_BK10_04_L      _PK_L_(0x10, 0x04)
#define REG_SC_BK10_04_H      _PK_H_(0x10, 0x04)
#define REG_SC_BK10_05_L      _PK_L_(0x10, 0x05)
#define REG_SC_BK10_05_H      _PK_H_(0x10, 0x05)
#define REG_SC_BK10_06_L      _PK_L_(0x10, 0x06)
#define REG_SC_BK10_06_H      _PK_H_(0x10, 0x06)
#define REG_SC_BK10_07_L      _PK_L_(0x10, 0x07)
#define REG_SC_BK10_07_H      _PK_H_(0x10, 0x07)
#define REG_SC_BK10_08_L      _PK_L_(0x10, 0x08)
#define REG_SC_BK10_08_H      _PK_H_(0x10, 0x08)
#define REG_SC_BK10_09_L      _PK_L_(0x10, 0x09)
#define REG_SC_BK10_09_H      _PK_H_(0x10, 0x09)
#define REG_SC_BK10_0A_L      _PK_L_(0x10, 0x0A)
#define REG_SC_BK10_0A_H      _PK_H_(0x10, 0x0A)
#define REG_SC_BK10_0B_L      _PK_L_(0x10, 0x0B)
#define REG_SC_BK10_0B_H      _PK_H_(0x10, 0x0B)
#define REG_SC_BK10_0C_L      _PK_L_(0x10, 0x0C)
#define REG_SC_BK10_0C_H      _PK_H_(0x10, 0x0C)
#define REG_SC_BK10_0D_L      _PK_L_(0x10, 0x0D)
#define REG_SC_BK10_0D_H      _PK_H_(0x10, 0x0D)

#define REG_SC_BK10_10_L      _PK_L_(0x10, 0x10)
#define REG_SC_BK10_10_H      _PK_H_(0x10, 0x10)

#define REG_SC_BK10_1B_L      _PK_L_(0x10, 0x1B)
#define REG_SC_BK10_1B_H      _PK_H_(0x10, 0x1B)

#define REG_SC_BK10_21_L      _PK_L_(0x10, 0x21)
#define REG_SC_BK10_21_H      _PK_H_(0x10, 0x21)
#define REG_SC_BK10_22_L      _PK_L_(0x10, 0x22)
#define REG_SC_BK10_22_H      _PK_H_(0x10, 0x22)

#define REG_SC_BK10_23_L      _PK_L_(0x10, 0x23)
#define REG_SC_BK10_23_H      _PK_H_(0x10, 0x23)

#define REG_SC_BK10_46_L      _PK_L_(0x10, 0x46)
#define REG_SC_BK10_46_H      _PK_H_(0x10, 0x46)
#define REG_SC_BK10_47_L      _PK_L_(0x10, 0x47)
#define REG_SC_BK10_47_H      _PK_H_(0x10, 0x47)

#define REG_SC_BK10_50_L      _PK_L_(0x10, 0x50)
#define REG_SC_BK10_50_H      _PK_H_(0x10, 0x50)

#define REG_SC_BK10_6C_L      _PK_L_(0x10, 0x6C)
#define REG_SC_BK10_6C_H      _PK_H_(0x10, 0x6C)
#define REG_SC_BK10_6D_L      _PK_L_(0x10, 0x6D)
#define REG_SC_BK10_6D_H      _PK_H_(0x10, 0x6D)
#define REG_SC_BK10_6E_L      _PK_L_(0x10, 0x6E)
#define REG_SC_BK10_6E_H      _PK_H_(0x10, 0x6E)


#define REG_SC1_BK10_6C_L      _PK_L_(0x90, 0x6C)
#define REG_SC1_BK10_6C_H      _PK_H_(0x90, 0x6C)
#define REG_SC1_BK10_6D_L      _PK_L_(0x90, 0x6D)
#define REG_SC1_BK10_6D_H      _PK_H_(0x90, 0x6D)
#define REG_SC1_BK10_6E_L      _PK_L_(0x90, 0x6E)
#define REG_SC1_BK10_6E_H      _PK_H_(0x90, 0x6E)

#define REG_SC_BK10_6F_L      _PK_L_(0x10, 0x6F)

#define REG_SC_BK10_70_L      _PK_L_(0x10, 0x70)

#define REG_SC_BK10_71_L      _PK_L_(0x10, 0x71)

#define REG_SC_BK10_72_L      _PK_L_(0x10, 0x72)

#define REG_SC_BK10_73_L      _PK_L_(0x10, 0x73)

#define REG_SC_BK10_74_L      _PK_L_(0x10, 0x74)
#define REG_SC_BK10_74_H      _PK_H_(0x10, 0x74)

#define REG_SC_BK10_75_L      _PK_L_(0x10, 0x75)
#define REG_SC_BK10_75_H      _PK_H_(0x10, 0x75)

#define REG_SC_BK10_76_L      _PK_L_(0x10, 0x76)
#define REG_SC_BK10_76_H      _PK_H_(0x10, 0x76)

#define REG_SC_BK10_77_L      _PK_L_(0x10, 0x77)
#define REG_SC_BK10_77_H      _PK_H_(0x10, 0x77)

#define REG_SC_BK10_78_L      _PK_L_(0x10, 0x78)

#define REG_SC_BK10_7A_L      _PK_L_(0x10, 0x7A)
#define REG_SC_BK10_7A_H      _PK_H_(0x10, 0x7A)
#define REG_SC_BK10_7B_L      _PK_L_(0x10, 0x7B)
#define REG_SC_BK10_7B_H      _PK_H_(0x10, 0x7B)

#define REG_SC_BK10_7D_L      _PK_L_(0x10, 0x7D)
#define REG_SC_BK10_7E_L      _PK_L_(0x10, 0x7E)
#define REG_SC_BK10_7F_L      _PK_L_(0x10, 0x7F)




//----------------------------------------------
//  BK16: OD
//----------------------------------------------
#define REG_SC_BK16_00_L      _PK_L_(0x16, 0x00)
#define REG_SC_BK16_00_H      _PK_H_(0x16, 0x00)
#define REG_SC_BK16_01_L      _PK_L_(0x16, 0x01)
#define REG_SC_BK16_01_H      _PK_H_(0x16, 0x01)
#define REG_SC_BK16_02_L      _PK_L_(0x16, 0x02)
#define REG_SC_BK16_02_H      _PK_H_(0x16, 0x02)
#define REG_SC_BK16_03_L      _PK_L_(0x16, 0x03)
#define REG_SC_BK16_03_H      _PK_H_(0x16, 0x03)
#define REG_SC_BK16_04_L      _PK_L_(0x16, 0x04)
#define REG_SC_BK16_04_H      _PK_H_(0x16, 0x04)
#define REG_SC_BK16_05_L      _PK_L_(0x16, 0x05)
#define REG_SC_BK16_05_H      _PK_H_(0x16, 0x05)
#define REG_SC_BK16_06_L      _PK_L_(0x16, 0x06)
#define REG_SC_BK16_06_H      _PK_H_(0x16, 0x06)
#define REG_SC_BK16_07_L      _PK_L_(0x16, 0x07)
#define REG_SC_BK16_07_H      _PK_H_(0x16, 0x07)
#define REG_SC_BK16_08_L      _PK_L_(0x16, 0x08)
#define REG_SC_BK16_08_H      _PK_H_(0x16, 0x08)
#define REG_SC_BK16_09_L      _PK_L_(0x16, 0x09)
#define REG_SC_BK16_09_H      _PK_H_(0x16, 0x09)
#define REG_SC_BK16_0A_L      _PK_L_(0x16, 0x0A)
#define REG_SC_BK16_0A_H      _PK_H_(0x16, 0x0A)
#define REG_SC_BK16_0B_L      _PK_L_(0x16, 0x0B)
#define REG_SC_BK16_0B_H      _PK_H_(0x16, 0x0B)
#define REG_SC_BK16_0C_L      _PK_L_(0x16, 0x0C)
#define REG_SC_BK16_0C_H      _PK_H_(0x16, 0x0C)
#define REG_SC_BK16_0D_L      _PK_L_(0x16, 0x0D)
#define REG_SC_BK16_0D_H      _PK_H_(0x16, 0x0D)
#define REG_SC_BK16_0E_L      _PK_L_(0x16, 0x0E)
#define REG_SC_BK16_0E_H      _PK_H_(0x16, 0x0E)
#define REG_SC_BK16_0F_L      _PK_L_(0x16, 0x0F)
#define REG_SC_BK16_0F_H      _PK_H_(0x16, 0x0F)
#define REG_SC_BK16_10_L      _PK_L_(0x16, 0x10)
#define REG_SC_BK16_10_H      _PK_H_(0x16, 0x10)
#define REG_SC_BK16_11_L      _PK_L_(0x16, 0x11)
#define REG_SC_BK16_11_H      _PK_H_(0x16, 0x11)
#define REG_SC_BK16_12_L      _PK_L_(0x16, 0x12)
#define REG_SC_BK16_12_H      _PK_H_(0x16, 0x12)
#define REG_SC_BK16_13_L      _PK_L_(0x16, 0x13)
#define REG_SC_BK16_13_H      _PK_H_(0x16, 0x13)
#define REG_SC_BK16_14_L      _PK_L_(0x16, 0x14)
#define REG_SC_BK16_14_H      _PK_H_(0x16, 0x14)
#define REG_SC_BK16_15_L      _PK_L_(0x16, 0x15)
#define REG_SC_BK16_15_H      _PK_H_(0x16, 0x15)
#define REG_SC_BK16_16_L      _PK_L_(0x16, 0x16)
#define REG_SC_BK16_16_H      _PK_H_(0x16, 0x16)
#define REG_SC_BK16_17_L      _PK_L_(0x16, 0x17)
#define REG_SC_BK16_17_H      _PK_H_(0x16, 0x17)
#define REG_SC_BK16_18_L      _PK_L_(0x16, 0x18)
#define REG_SC_BK16_18_H      _PK_H_(0x16, 0x18)
#define REG_SC_BK16_19_L      _PK_L_(0x16, 0x19)
#define REG_SC_BK16_19_H      _PK_H_(0x16, 0x19)
#define REG_SC_BK16_1A_L      _PK_L_(0x16, 0x1A)
#define REG_SC_BK16_1A_H      _PK_H_(0x16, 0x1A)
#define REG_SC_BK16_1B_L      _PK_L_(0x16, 0x1B)
#define REG_SC_BK16_1B_H      _PK_H_(0x16, 0x1B)
#define REG_SC_BK16_1C_L      _PK_L_(0x16, 0x1C)
#define REG_SC_BK16_1C_H      _PK_H_(0x16, 0x1C)
#define REG_SC_BK16_1D_L      _PK_L_(0x16, 0x1D)
#define REG_SC_BK16_1D_H      _PK_H_(0x16, 0x1D)
#define REG_SC_BK16_1E_L      _PK_L_(0x16, 0x1E)
#define REG_SC_BK16_1E_H      _PK_H_(0x16, 0x1E)
#define REG_SC_BK16_1F_L      _PK_L_(0x16, 0x1F)
#define REG_SC_BK16_1F_H      _PK_H_(0x16, 0x1F)
#define REG_SC_BK16_20_L      _PK_L_(0x16, 0x20)
#define REG_SC_BK16_20_H      _PK_H_(0x16, 0x20)
#define REG_SC_BK16_21_L      _PK_L_(0x16, 0x21)
#define REG_SC_BK16_21_H      _PK_H_(0x16, 0x21)
#define REG_SC_BK16_22_L      _PK_L_(0x16, 0x22)
#define REG_SC_BK16_22_H      _PK_H_(0x16, 0x22)
#define REG_SC_BK16_23_L      _PK_L_(0x16, 0x23)
#define REG_SC_BK16_23_H      _PK_H_(0x16, 0x23)
#define REG_SC_BK16_24_L      _PK_L_(0x16, 0x24)
#define REG_SC_BK16_24_H      _PK_H_(0x16, 0x24)
#define REG_SC_BK16_25_L      _PK_L_(0x16, 0x25)
#define REG_SC_BK16_25_H      _PK_H_(0x16, 0x25)
#define REG_SC_BK16_26_L      _PK_L_(0x16, 0x26)
#define REG_SC_BK16_26_H      _PK_H_(0x16, 0x26)
#define REG_SC_BK16_27_L      _PK_L_(0x16, 0x27)
#define REG_SC_BK16_27_H      _PK_H_(0x16, 0x27)
#define REG_SC_BK16_28_L      _PK_L_(0x16, 0x28)
#define REG_SC_BK16_28_H      _PK_H_(0x16, 0x28)
#define REG_SC_BK16_29_L      _PK_L_(0x16, 0x29)
#define REG_SC_BK16_29_H      _PK_H_(0x16, 0x29)
#define REG_SC_BK16_2A_L      _PK_L_(0x16, 0x2A)
#define REG_SC_BK16_2A_H      _PK_H_(0x16, 0x2A)
#define REG_SC_BK16_2B_L      _PK_L_(0x16, 0x2B)
#define REG_SC_BK16_2B_H      _PK_H_(0x16, 0x2B)
#define REG_SC_BK16_2C_L      _PK_L_(0x16, 0x2C)
#define REG_SC_BK16_2C_H      _PK_H_(0x16, 0x2C)
#define REG_SC_BK16_2D_L      _PK_L_(0x16, 0x2D)
#define REG_SC_BK16_2D_H      _PK_H_(0x16, 0x2D)
#define REG_SC_BK16_2E_L      _PK_L_(0x16, 0x2E)
#define REG_SC_BK16_2E_H      _PK_H_(0x16, 0x2E)
#define REG_SC_BK16_2F_L      _PK_L_(0x16, 0x2F)
#define REG_SC_BK16_2F_H      _PK_H_(0x16, 0x2F)
#define REG_SC_BK16_30_L      _PK_L_(0x16, 0x30)
#define REG_SC_BK16_30_H      _PK_H_(0x16, 0x30)
#define REG_SC_BK16_31_L      _PK_L_(0x16, 0x31)
#define REG_SC_BK16_31_H      _PK_H_(0x16, 0x31)
#define REG_SC_BK16_32_L      _PK_L_(0x16, 0x32)
#define REG_SC_BK16_32_H      _PK_H_(0x16, 0x32)
#define REG_SC_BK16_33_L      _PK_L_(0x16, 0x33)
#define REG_SC_BK16_33_H      _PK_H_(0x16, 0x33)
#define REG_SC_BK16_34_L      _PK_L_(0x16, 0x34)
#define REG_SC_BK16_34_H      _PK_H_(0x16, 0x34)
#define REG_SC_BK16_35_L      _PK_L_(0x16, 0x35)
#define REG_SC_BK16_35_H      _PK_H_(0x16, 0x35)
#define REG_SC_BK16_36_L      _PK_L_(0x16, 0x36)
#define REG_SC_BK16_36_H      _PK_H_(0x16, 0x36)
#define REG_SC_BK16_37_L      _PK_L_(0x16, 0x37)
#define REG_SC_BK16_37_H      _PK_H_(0x16, 0x37)
#define REG_SC_BK16_38_L      _PK_L_(0x16, 0x38)
#define REG_SC_BK16_38_H      _PK_H_(0x16, 0x38)
#define REG_SC_BK16_39_L      _PK_L_(0x16, 0x39)
#define REG_SC_BK16_39_H      _PK_H_(0x16, 0x39)
#define REG_SC_BK16_3A_L      _PK_L_(0x16, 0x3A)
#define REG_SC_BK16_3A_H      _PK_H_(0x16, 0x3A)
#define REG_SC_BK16_3B_L      _PK_L_(0x16, 0x3B)
#define REG_SC_BK16_3B_H      _PK_H_(0x16, 0x3B)
#define REG_SC_BK16_3C_L      _PK_L_(0x16, 0x3C)
#define REG_SC_BK16_3C_H      _PK_H_(0x16, 0x3C)
#define REG_SC_BK16_3D_L      _PK_L_(0x16, 0x3D)
#define REG_SC_BK16_3D_H      _PK_H_(0x16, 0x3D)
#define REG_SC_BK16_3E_L      _PK_L_(0x16, 0x3E)
#define REG_SC_BK16_3E_H      _PK_H_(0x16, 0x3E)
#define REG_SC_BK16_3F_L      _PK_L_(0x16, 0x3F)
#define REG_SC_BK16_3F_H      _PK_H_(0x16, 0x3F)
#define REG_SC_BK16_40_L      _PK_L_(0x16, 0x40)
#define REG_SC_BK16_40_H      _PK_H_(0x16, 0x40)
#define REG_SC_BK16_41_L      _PK_L_(0x16, 0x41)
#define REG_SC_BK16_41_H      _PK_H_(0x16, 0x41)
#define REG_SC_BK16_42_L      _PK_L_(0x16, 0x42)
#define REG_SC_BK16_42_H      _PK_H_(0x16, 0x42)
#define REG_SC_BK16_43_L      _PK_L_(0x16, 0x43)
#define REG_SC_BK16_43_H      _PK_H_(0x16, 0x43)
#define REG_SC_BK16_44_L      _PK_L_(0x16, 0x44)
#define REG_SC_BK16_44_H      _PK_H_(0x16, 0x44)
#define REG_SC_BK16_45_L      _PK_L_(0x16, 0x45)
#define REG_SC_BK16_45_H      _PK_H_(0x16, 0x45)
#define REG_SC_BK16_46_L      _PK_L_(0x16, 0x46)
#define REG_SC_BK16_46_H      _PK_H_(0x16, 0x46)
#define REG_SC_BK16_47_L      _PK_L_(0x16, 0x47)
#define REG_SC_BK16_47_H      _PK_H_(0x16, 0x47)
#define REG_SC_BK16_48_L      _PK_L_(0x16, 0x48)
#define REG_SC_BK16_48_H      _PK_H_(0x16, 0x48)
#define REG_SC_BK16_49_L      _PK_L_(0x16, 0x49)
#define REG_SC_BK16_49_H      _PK_H_(0x16, 0x49)
#define REG_SC_BK16_4A_L      _PK_L_(0x16, 0x4A)
#define REG_SC_BK16_4A_H      _PK_H_(0x16, 0x4A)
#define REG_SC_BK16_4B_L      _PK_L_(0x16, 0x4B)
#define REG_SC_BK16_4B_H      _PK_H_(0x16, 0x4B)
#define REG_SC_BK16_4C_L      _PK_L_(0x16, 0x4C)
#define REG_SC_BK16_4C_H      _PK_H_(0x16, 0x4C)
#define REG_SC_BK16_4D_L      _PK_L_(0x16, 0x4D)
#define REG_SC_BK16_4D_H      _PK_H_(0x16, 0x4D)
#define REG_SC_BK16_4E_L      _PK_L_(0x16, 0x4E)
#define REG_SC_BK16_4E_H      _PK_H_(0x16, 0x4E)
#define REG_SC_BK16_4F_L      _PK_L_(0x16, 0x4F)
#define REG_SC_BK16_4F_H      _PK_H_(0x16, 0x4F)
#define REG_SC_BK16_50_L      _PK_L_(0x16, 0x50)
#define REG_SC_BK16_50_H      _PK_H_(0x16, 0x50)
#define REG_SC_BK16_51_L      _PK_L_(0x16, 0x51)
#define REG_SC_BK16_51_H      _PK_H_(0x16, 0x51)
#define REG_SC_BK16_52_L      _PK_L_(0x16, 0x52)
#define REG_SC_BK16_52_H      _PK_H_(0x16, 0x52)
#define REG_SC_BK16_53_L      _PK_L_(0x16, 0x53)
#define REG_SC_BK16_53_H      _PK_H_(0x16, 0x53)
#define REG_SC_BK16_54_L      _PK_L_(0x16, 0x54)
#define REG_SC_BK16_54_H      _PK_H_(0x16, 0x54)
#define REG_SC_BK16_55_L      _PK_L_(0x16, 0x55)
#define REG_SC_BK16_55_H      _PK_H_(0x16, 0x55)
#define REG_SC_BK16_56_L      _PK_L_(0x16, 0x56)
#define REG_SC_BK16_56_H      _PK_H_(0x16, 0x56)
#define REG_SC_BK16_57_L      _PK_L_(0x16, 0x57)
#define REG_SC_BK16_57_H      _PK_H_(0x16, 0x57)
#define REG_SC_BK16_58_L      _PK_L_(0x16, 0x58)
#define REG_SC_BK16_58_H      _PK_H_(0x16, 0x58)
#define REG_SC_BK16_59_L      _PK_L_(0x16, 0x59)
#define REG_SC_BK16_59_H      _PK_H_(0x16, 0x59)
#define REG_SC_BK16_5A_L      _PK_L_(0x16, 0x5A)
#define REG_SC_BK16_5A_H      _PK_H_(0x16, 0x5A)
#define REG_SC_BK16_5B_L      _PK_L_(0x16, 0x5B)
#define REG_SC_BK16_5B_H      _PK_H_(0x16, 0x5B)
#define REG_SC_BK16_5C_L      _PK_L_(0x16, 0x5C)
#define REG_SC_BK16_5C_H      _PK_H_(0x16, 0x5C)
#define REG_SC_BK16_5D_L      _PK_L_(0x16, 0x5D)
#define REG_SC_BK16_5D_H      _PK_H_(0x16, 0x5D)
#define REG_SC_BK16_5E_L      _PK_L_(0x16, 0x5E)
#define REG_SC_BK16_5E_H      _PK_H_(0x16, 0x5E)
#define REG_SC_BK16_5F_L      _PK_L_(0x16, 0x5F)
#define REG_SC_BK16_5F_H      _PK_H_(0x16, 0x5F)
#define REG_SC_BK16_60_L      _PK_L_(0x16, 0x60)
#define REG_SC_BK16_60_H      _PK_H_(0x16, 0x60)
#define REG_SC_BK16_61_L      _PK_L_(0x16, 0x61)
#define REG_SC_BK16_61_H      _PK_H_(0x16, 0x61)
#define REG_SC_BK16_62_L      _PK_L_(0x16, 0x62)
#define REG_SC_BK16_62_H      _PK_H_(0x16, 0x62)
#define REG_SC_BK16_63_L      _PK_L_(0x16, 0x63)
#define REG_SC_BK16_63_H      _PK_H_(0x16, 0x63)
#define REG_SC_BK16_64_L      _PK_L_(0x16, 0x64)
#define REG_SC_BK16_64_H      _PK_H_(0x16, 0x64)
#define REG_SC_BK16_65_L      _PK_L_(0x16, 0x65)
#define REG_SC_BK16_65_H      _PK_H_(0x16, 0x65)
#define REG_SC_BK16_66_L      _PK_L_(0x16, 0x66)
#define REG_SC_BK16_66_H      _PK_H_(0x16, 0x66)
#define REG_SC_BK16_67_L      _PK_L_(0x16, 0x67)
#define REG_SC_BK16_67_H      _PK_H_(0x16, 0x67)
#define REG_SC_BK16_68_L      _PK_L_(0x16, 0x68)
#define REG_SC_BK16_68_H      _PK_H_(0x16, 0x68)
#define REG_SC_BK16_69_L      _PK_L_(0x16, 0x69)
#define REG_SC_BK16_69_H      _PK_H_(0x16, 0x69)
#define REG_SC_BK16_6A_L      _PK_L_(0x16, 0x6A)
#define REG_SC_BK16_6A_H      _PK_H_(0x16, 0x6A)
#define REG_SC_BK16_6B_L      _PK_L_(0x16, 0x6B)
#define REG_SC_BK16_6B_H      _PK_H_(0x16, 0x6B)
#define REG_SC_BK16_6C_L      _PK_L_(0x16, 0x6C)
#define REG_SC_BK16_6C_H      _PK_H_(0x16, 0x6C)
#define REG_SC_BK16_6D_L      _PK_L_(0x16, 0x6D)
#define REG_SC_BK16_6D_H      _PK_H_(0x16, 0x6D)
#define REG_SC_BK16_6E_L      _PK_L_(0x16, 0x6E)
#define REG_SC_BK16_6E_H      _PK_H_(0x16, 0x6E)
#define REG_SC_BK16_6F_L      _PK_L_(0x16, 0x6F)
#define REG_SC_BK16_6F_H      _PK_H_(0x16, 0x6F)
#define REG_SC_BK16_70_L      _PK_L_(0x16, 0x70)
#define REG_SC_BK16_70_H      _PK_H_(0x16, 0x70)
#define REG_SC_BK16_71_L      _PK_L_(0x16, 0x71)
#define REG_SC_BK16_71_H      _PK_H_(0x16, 0x71)
#define REG_SC_BK16_72_L      _PK_L_(0x16, 0x72)
#define REG_SC_BK16_72_H      _PK_H_(0x16, 0x72)
#define REG_SC_BK16_73_L      _PK_L_(0x16, 0x73)
#define REG_SC_BK16_73_H      _PK_H_(0x16, 0x73)
#define REG_SC_BK16_74_L      _PK_L_(0x16, 0x74)
#define REG_SC_BK16_74_H      _PK_H_(0x16, 0x74)
#define REG_SC_BK16_75_L      _PK_L_(0x16, 0x75)
#define REG_SC_BK16_75_H      _PK_H_(0x16, 0x75)
#define REG_SC_BK16_76_L      _PK_L_(0x16, 0x76)
#define REG_SC_BK16_76_H      _PK_H_(0x16, 0x76)
#define REG_SC_BK16_77_L      _PK_L_(0x16, 0x77)
#define REG_SC_BK16_77_H      _PK_H_(0x16, 0x77)
#define REG_SC_BK16_78_L      _PK_L_(0x16, 0x78)
#define REG_SC_BK16_78_H      _PK_H_(0x16, 0x78)
#define REG_SC_BK16_79_L      _PK_L_(0x16, 0x79)
#define REG_SC_BK16_79_H      _PK_H_(0x16, 0x79)
#define REG_SC_BK16_7A_L      _PK_L_(0x16, 0x7A)
#define REG_SC_BK16_7A_H      _PK_H_(0x16, 0x7A)
#define REG_SC_BK16_7B_L      _PK_L_(0x16, 0x7B)
#define REG_SC_BK16_7B_H      _PK_H_(0x16, 0x7B)
#define REG_SC_BK16_7C_L      _PK_L_(0x16, 0x7C)
#define REG_SC_BK16_7C_H      _PK_H_(0x16, 0x7C)
#define REG_SC_BK16_7D_L      _PK_L_(0x16, 0x7D)
#define REG_SC_BK16_7D_H      _PK_H_(0x16, 0x7D)
#define REG_SC_BK16_7E_L      _PK_L_(0x16, 0x7E)
#define REG_SC_BK16_7E_H      _PK_H_(0x16, 0x7E)
#define REG_SC_BK16_7F_L      _PK_L_(0x16, 0x7F)
#define REG_SC_BK16_7F_H      _PK_H_(0x16, 0x7F)

//----------------------------------------------
//  BK24
//----------------------------------------------
#define REG_SC_BK24_3F_L       _PK_L_(0x24, 0x3F)
#define REG_SC_BK24_3F_H       _PK_H_(0x24, 0x3F)

//----------------------------------------------
//  MOD
//----------------------------------------------

#define REG_MOD_BK00_00_L      _PK_L_(0x00, 0x00)
#define REG_MOD_BK00_00_H      _PK_H_(0x00, 0x00)
#define REG_MOD_BK00_01_L      _PK_L_(0x00, 0x01)
#define REG_MOD_BK00_01_H      _PK_H_(0x00, 0x01)
#define REG_MOD_BK00_02_L      _PK_L_(0x00, 0x02)
#define REG_MOD_BK00_02_H      _PK_H_(0x00, 0x02)
#define REG_MOD_BK00_03_L      _PK_L_(0x00, 0x03)
#define REG_MOD_BK00_03_H      _PK_H_(0x00, 0x03)
#define REG_MOD_BK00_04_L      _PK_L_(0x00, 0x04)
#define REG_MOD_BK00_04_H      _PK_H_(0x00, 0x04)
#define REG_MOD_BK00_05_L      _PK_L_(0x00, 0x05)
#define REG_MOD_BK00_05_H      _PK_H_(0x00, 0x05)
#define REG_MOD_BK00_06_L      _PK_L_(0x00, 0x06)
#define REG_MOD_BK00_06_H      _PK_H_(0x00, 0x06)
#define REG_MOD_BK00_07_L      _PK_L_(0x00, 0x07)
#define REG_MOD_BK00_07_H      _PK_H_(0x00, 0x07)
#define REG_MOD_BK00_08_L      _PK_L_(0x00, 0x08)
#define REG_MOD_BK00_08_H      _PK_H_(0x00, 0x08)
#define REG_MOD_BK00_09_L      _PK_L_(0x00, 0x09)
#define REG_MOD_BK00_09_H      _PK_H_(0x00, 0x09)
#define REG_MOD_BK00_0A_L      _PK_L_(0x00, 0x0A)
#define REG_MOD_BK00_0A_H      _PK_H_(0x00, 0x0A)
#define REG_MOD_BK00_0B_L      _PK_L_(0x00, 0x0B)
#define REG_MOD_BK00_0B_H      _PK_H_(0x00, 0x0B)
#define REG_MOD_BK00_0C_L      _PK_L_(0x00, 0x0C)
#define REG_MOD_BK00_0C_H      _PK_H_(0x00, 0x0C)
#define REG_MOD_BK00_0D_L      _PK_L_(0x00, 0x0D)
#define REG_MOD_BK00_0D_H      _PK_H_(0x00, 0x0D)
#define REG_MOD_BK00_0E_L      _PK_L_(0x00, 0x0E)
#define REG_MOD_BK00_0E_H      _PK_H_(0x00, 0x0E)
#define REG_MOD_BK00_0F_L      _PK_L_(0x00, 0x0F)
#define REG_MOD_BK00_0F_H      _PK_H_(0x00, 0x0F)
#define REG_MOD_BK00_10_L      _PK_L_(0x00, 0x10)
#define REG_MOD_BK00_10_H      _PK_H_(0x00, 0x10)
#define REG_MOD_BK00_11_L      _PK_L_(0x00, 0x11)
#define REG_MOD_BK00_11_H      _PK_H_(0x00, 0x11)
#define REG_MOD_BK00_12_L      _PK_L_(0x00, 0x12)
#define REG_MOD_BK00_12_H      _PK_H_(0x00, 0x12)
#define REG_MOD_BK00_13_L      _PK_L_(0x00, 0x13)
#define REG_MOD_BK00_13_H      _PK_H_(0x00, 0x13)
#define REG_MOD_BK00_14_L      _PK_L_(0x00, 0x14)
#define REG_MOD_BK00_14_H      _PK_H_(0x00, 0x14)
#define REG_MOD_BK00_15_L      _PK_L_(0x00, 0x15)
#define REG_MOD_BK00_15_H      _PK_H_(0x00, 0x15)
#define REG_MOD_BK00_16_L      _PK_L_(0x00, 0x16)
#define REG_MOD_BK00_16_H      _PK_H_(0x00, 0x16)
#define REG_MOD_BK00_17_L      _PK_L_(0x00, 0x17)
#define REG_MOD_BK00_17_H      _PK_H_(0x00, 0x17)
#define REG_MOD_BK00_18_L      _PK_L_(0x00, 0x18)
#define REG_MOD_BK00_18_H      _PK_H_(0x00, 0x18)
#define REG_MOD_BK00_19_L      _PK_L_(0x00, 0x19)
#define REG_MOD_BK00_19_H      _PK_H_(0x00, 0x19)
#define REG_MOD_BK00_1A_L      _PK_L_(0x00, 0x1A)
#define REG_MOD_BK00_1A_H      _PK_H_(0x00, 0x1A)
#define REG_MOD_BK00_1B_L      _PK_L_(0x00, 0x1B)
#define REG_MOD_BK00_1B_H      _PK_H_(0x00, 0x1B)
#define REG_MOD_BK00_1C_L      _PK_L_(0x00, 0x1C)
#define REG_MOD_BK00_1C_H      _PK_H_(0x00, 0x1C)
#define REG_MOD_BK00_1D_L      _PK_L_(0x00, 0x1D)
#define REG_MOD_BK00_1D_H      _PK_H_(0x00, 0x1D)
#define REG_MOD_BK00_1E_L      _PK_L_(0x00, 0x1E)
#define REG_MOD_BK00_1E_H      _PK_H_(0x00, 0x1E)
#define REG_MOD_BK00_1F_L      _PK_L_(0x00, 0x1F)
#define REG_MOD_BK00_1F_H      _PK_H_(0x00, 0x1F)
#define REG_MOD_BK00_20_L      _PK_L_(0x00, 0x20)
#define REG_MOD_BK00_20_H      _PK_H_(0x00, 0x20)
#define REG_MOD_BK00_21_L      _PK_L_(0x00, 0x21)
#define REG_MOD_BK00_21_H      _PK_H_(0x00, 0x21)
#define REG_MOD_BK00_22_L      _PK_L_(0x00, 0x22)
#define REG_MOD_BK00_22_H      _PK_H_(0x00, 0x22)
#define REG_MOD_BK00_23_L      _PK_L_(0x00, 0x23)
#define REG_MOD_BK00_23_H      _PK_H_(0x00, 0x23)
#define REG_MOD_BK00_24_L      _PK_L_(0x00, 0x24)
#define REG_MOD_BK00_24_H      _PK_H_(0x00, 0x24)
#define REG_MOD_BK00_25_L      _PK_L_(0x00, 0x25)
#define REG_MOD_BK00_25_H      _PK_H_(0x00, 0x25)
#define REG_MOD_BK00_26_L      _PK_L_(0x00, 0x26)
#define REG_MOD_BK00_26_H      _PK_H_(0x00, 0x26)
#define REG_MOD_BK00_27_L      _PK_L_(0x00, 0x27)
#define REG_MOD_BK00_27_H      _PK_H_(0x00, 0x27)
#define REG_MOD_BK00_28_L      _PK_L_(0x00, 0x28)
#define REG_MOD_BK00_28_H      _PK_H_(0x00, 0x28)
#define REG_MOD_BK00_29_L      _PK_L_(0x00, 0x29)
#define REG_MOD_BK00_29_H      _PK_H_(0x00, 0x29)
#define REG_MOD_BK00_2A_L      _PK_L_(0x00, 0x2A)
#define REG_MOD_BK00_2A_H      _PK_H_(0x00, 0x2A)
#define REG_MOD_BK00_2B_L      _PK_L_(0x00, 0x2B)
#define REG_MOD_BK00_2B_H      _PK_H_(0x00, 0x2B)
#define REG_MOD_BK00_2C_L      _PK_L_(0x00, 0x2C)
#define REG_MOD_BK00_2C_H      _PK_H_(0x00, 0x2C)
#define REG_MOD_BK00_2D_L      _PK_L_(0x00, 0x2D)
#define REG_MOD_BK00_2D_H      _PK_H_(0x00, 0x2D)
#define REG_MOD_BK00_2E_L      _PK_L_(0x00, 0x2E)
#define REG_MOD_BK00_2E_H      _PK_H_(0x00, 0x2E)
#define REG_MOD_BK00_2F_L      _PK_L_(0x00, 0x2F)
#define REG_MOD_BK00_2F_H      _PK_H_(0x00, 0x2F)
#define REG_MOD_BK00_30_L      _PK_L_(0x00, 0x30)
#define REG_MOD_BK00_30_H      _PK_H_(0x00, 0x30)
#define REG_MOD_BK00_31_L      _PK_L_(0x00, 0x31)
#define REG_MOD_BK00_31_H      _PK_H_(0x00, 0x31)
#define REG_MOD_BK00_32_L      _PK_L_(0x00, 0x32)
#define REG_MOD_BK00_32_H      _PK_H_(0x00, 0x32)
#define REG_MOD_BK00_33_L      _PK_L_(0x00, 0x33)
#define REG_MOD_BK00_33_H      _PK_H_(0x00, 0x33)
#define REG_MOD_BK00_34_L      _PK_L_(0x00, 0x34)
#define REG_MOD_BK00_34_H      _PK_H_(0x00, 0x34)
#define REG_MOD_BK00_35_L      _PK_L_(0x00, 0x35)
#define REG_MOD_BK00_35_H      _PK_H_(0x00, 0x35)
#define REG_MOD_BK00_36_L      _PK_L_(0x00, 0x36)
#define REG_MOD_BK00_36_H      _PK_H_(0x00, 0x36)
#define REG_MOD_BK00_37_L      _PK_L_(0x00, 0x37)
#define REG_MOD_BK00_37_H      _PK_H_(0x00, 0x37)
#define REG_MOD_BK00_38_L      _PK_L_(0x00, 0x38)
#define REG_MOD_BK00_38_H      _PK_H_(0x00, 0x38)
#define REG_MOD_BK00_39_L      _PK_L_(0x00, 0x39)
#define REG_MOD_BK00_39_H      _PK_H_(0x00, 0x39)
#define REG_MOD_BK00_3A_L      _PK_L_(0x00, 0x3A)
#define REG_MOD_BK00_3A_H      _PK_H_(0x00, 0x3A)
#define REG_MOD_BK00_3B_L      _PK_L_(0x00, 0x3B)
#define REG_MOD_BK00_3B_H      _PK_H_(0x00, 0x3B)
#define REG_MOD_BK00_3C_L      _PK_L_(0x00, 0x3C)
#define REG_MOD_BK00_3C_H      _PK_H_(0x00, 0x3C)
#define REG_MOD_BK00_3D_L      _PK_L_(0x00, 0x3D)
#define REG_MOD_BK00_3D_H      _PK_H_(0x00, 0x3D)
#define REG_MOD_BK00_3E_L      _PK_L_(0x00, 0x3E)
#define REG_MOD_BK00_3E_H      _PK_H_(0x00, 0x3E)
#define REG_MOD_BK00_3F_L      _PK_L_(0x00, 0x3F)
#define REG_MOD_BK00_3F_H      _PK_H_(0x00, 0x3F)
#define REG_MOD_BK00_40_L      _PK_L_(0x00, 0x40)
#define REG_MOD_BK00_40_H      _PK_H_(0x00, 0x40)
#define REG_MOD_BK00_41_L      _PK_L_(0x00, 0x41)
#define REG_MOD_BK00_41_H      _PK_H_(0x00, 0x41)
#define REG_MOD_BK00_42_L      _PK_L_(0x00, 0x42)
#define REG_MOD_BK00_42_H      _PK_H_(0x00, 0x42)
#define REG_MOD_BK00_43_L      _PK_L_(0x00, 0x43)
#define REG_MOD_BK00_43_H      _PK_H_(0x00, 0x43)
#define REG_MOD_BK00_44_L      _PK_L_(0x00, 0x44)
#define REG_MOD_BK00_44_H      _PK_H_(0x00, 0x44)
#define REG_MOD_BK00_45_L      _PK_L_(0x00, 0x45)
#define REG_MOD_BK00_45_H      _PK_H_(0x00, 0x45)
#define REG_MOD_BK00_46_L      _PK_L_(0x00, 0x46)
#define REG_MOD_BK00_46_H      _PK_H_(0x00, 0x46)
#define REG_MOD_BK00_47_L      _PK_L_(0x00, 0x47)
#define REG_MOD_BK00_47_H      _PK_H_(0x00, 0x47)
#define REG_MOD_BK00_48_L      _PK_L_(0x00, 0x48)
#define REG_MOD_BK00_48_H      _PK_H_(0x00, 0x48)
#define REG_MOD_BK00_49_L      _PK_L_(0x00, 0x49)
#define REG_MOD_BK00_49_H      _PK_H_(0x00, 0x49)
#define REG_MOD_BK00_4A_L      _PK_L_(0x00, 0x4A)
#define REG_MOD_BK00_4A_H      _PK_H_(0x00, 0x4A)
#define REG_MOD_BK00_4B_L      _PK_L_(0x00, 0x4B)
#define REG_MOD_BK00_4B_H      _PK_H_(0x00, 0x4B)
#define REG_MOD_BK00_4C_L      _PK_L_(0x00, 0x4C)
#define REG_MOD_BK00_4C_H      _PK_H_(0x00, 0x4C)
#define REG_MOD_BK00_4D_L      _PK_L_(0x00, 0x4D)
#define REG_MOD_BK00_4D_H      _PK_H_(0x00, 0x4D)
#define REG_MOD_BK00_4E_L      _PK_L_(0x00, 0x4E)
#define REG_MOD_BK00_4E_H      _PK_H_(0x00, 0x4E)
#define REG_MOD_BK00_4F_L      _PK_L_(0x00, 0x4F)
#define REG_MOD_BK00_4F_H      _PK_H_(0x00, 0x4F)
#define REG_MOD_BK00_50_L      _PK_L_(0x00, 0x50)
#define REG_MOD_BK00_50_H      _PK_H_(0x00, 0x50)
#define REG_MOD_BK00_51_L      _PK_L_(0x00, 0x51)
#define REG_MOD_BK00_51_H      _PK_H_(0x00, 0x51)
#define REG_MOD_BK00_52_L      _PK_L_(0x00, 0x52)
#define REG_MOD_BK00_52_H      _PK_H_(0x00, 0x52)
#define REG_MOD_BK00_53_L      _PK_L_(0x00, 0x53)
#define REG_MOD_BK00_53_H      _PK_H_(0x00, 0x53)
#define REG_MOD_BK00_54_L      _PK_L_(0x00, 0x54)
#define REG_MOD_BK00_54_H      _PK_H_(0x00, 0x54)
#define REG_MOD_BK00_55_L      _PK_L_(0x00, 0x55)
#define REG_MOD_BK00_55_H      _PK_H_(0x00, 0x55)
#define REG_MOD_BK00_56_L      _PK_L_(0x00, 0x56)
#define REG_MOD_BK00_56_H      _PK_H_(0x00, 0x56)
#define REG_MOD_BK00_57_L      _PK_L_(0x00, 0x57)
#define REG_MOD_BK00_57_H      _PK_H_(0x00, 0x57)
#define REG_MOD_BK00_58_L      _PK_L_(0x00, 0x58)
#define REG_MOD_BK00_58_H      _PK_H_(0x00, 0x58)
#define REG_MOD_BK00_59_L      _PK_L_(0x00, 0x59)
#define REG_MOD_BK00_59_H      _PK_H_(0x00, 0x59)
#define REG_MOD_BK00_5A_L      _PK_L_(0x00, 0x5A)
#define REG_MOD_BK00_5A_H      _PK_H_(0x00, 0x5A)
#define REG_MOD_BK00_5B_L      _PK_L_(0x00, 0x5B)
#define REG_MOD_BK00_5B_H      _PK_H_(0x00, 0x5B)
#define REG_MOD_BK00_5C_L      _PK_L_(0x00, 0x5C)
#define REG_MOD_BK00_5C_H      _PK_H_(0x00, 0x5C)
#define REG_MOD_BK00_5D_L      _PK_L_(0x00, 0x5D)
#define REG_MOD_BK00_5D_H      _PK_H_(0x00, 0x5D)
#define REG_MOD_BK00_5E_L      _PK_L_(0x00, 0x5E)
#define REG_MOD_BK00_5E_H      _PK_H_(0x00, 0x5E)
#define REG_MOD_BK00_5F_L      _PK_L_(0x00, 0x5F)
#define REG_MOD_BK00_5F_H      _PK_H_(0x00, 0x5F)
#define REG_MOD_BK00_60_L      _PK_L_(0x00, 0x60)
#define REG_MOD_BK00_60_H      _PK_H_(0x00, 0x60)
#define REG_MOD_BK00_61_L      _PK_L_(0x00, 0x61)
#define REG_MOD_BK00_61_H      _PK_H_(0x00, 0x61)
#define REG_MOD_BK00_62_L      _PK_L_(0x00, 0x62)
#define REG_MOD_BK00_62_H      _PK_H_(0x00, 0x62)
#define REG_MOD_BK00_63_L      _PK_L_(0x00, 0x63)
#define REG_MOD_BK00_63_H      _PK_H_(0x00, 0x63)
#define REG_MOD_BK00_64_L      _PK_L_(0x00, 0x64)
#define REG_MOD_BK00_64_H      _PK_H_(0x00, 0x64)
#define REG_MOD_BK00_65_L      _PK_L_(0x00, 0x65)
#define REG_MOD_BK00_65_H      _PK_H_(0x00, 0x65)
#define REG_MOD_BK00_66_L      _PK_L_(0x00, 0x66)
#define REG_MOD_BK00_66_H      _PK_H_(0x00, 0x66)
#define REG_MOD_BK00_67_L      _PK_L_(0x00, 0x67)
#define REG_MOD_BK00_67_H      _PK_H_(0x00, 0x67)
#define REG_MOD_BK00_68_L      _PK_L_(0x00, 0x68)
#define REG_MOD_BK00_68_H      _PK_H_(0x00, 0x68)
#define REG_MOD_BK00_69_L      _PK_L_(0x00, 0x69)
#define REG_MOD_BK00_69_H      _PK_H_(0x00, 0x69)
#define REG_MOD_BK00_6A_L      _PK_L_(0x00, 0x6A)
#define REG_MOD_BK00_6A_H      _PK_H_(0x00, 0x6A)
#define REG_MOD_BK00_6B_L      _PK_L_(0x00, 0x6B)
#define REG_MOD_BK00_6B_H      _PK_H_(0x00, 0x6B)
#define REG_MOD_BK00_6C_L      _PK_L_(0x00, 0x6C)
#define REG_MOD_BK00_6C_H      _PK_H_(0x00, 0x6C)
#define REG_MOD_BK00_6D_L      _PK_L_(0x00, 0x6D)
#define REG_MOD_BK00_6D_H      _PK_H_(0x00, 0x6D)
#define REG_MOD_BK00_6E_L      _PK_L_(0x00, 0x6E)
#define REG_MOD_BK00_6E_H      _PK_H_(0x00, 0x6E)
#define REG_MOD_BK00_6F_L      _PK_L_(0x00, 0x6F)
#define REG_MOD_BK00_6F_H      _PK_H_(0x00, 0x6F)
#define REG_MOD_BK00_70_L      _PK_L_(0x00, 0x70)
#define REG_MOD_BK00_70_H      _PK_H_(0x00, 0x70)
#define REG_MOD_BK00_71_L      _PK_L_(0x00, 0x71)
#define REG_MOD_BK00_71_H      _PK_H_(0x00, 0x71)
#define REG_MOD_BK00_72_L      _PK_L_(0x00, 0x72)
#define REG_MOD_BK00_72_H      _PK_H_(0x00, 0x72)
#define REG_MOD_BK00_73_L      _PK_L_(0x00, 0x73)
#define REG_MOD_BK00_73_H      _PK_H_(0x00, 0x73)
#define REG_MOD_BK00_74_L      _PK_L_(0x00, 0x74)
#define REG_MOD_BK00_74_H      _PK_H_(0x00, 0x74)
#define REG_MOD_BK00_75_L      _PK_L_(0x00, 0x75)
#define REG_MOD_BK00_75_H      _PK_H_(0x00, 0x75)
#define REG_MOD_BK00_76_L      _PK_L_(0x00, 0x76)
#define REG_MOD_BK00_76_H      _PK_H_(0x00, 0x76)
#define REG_MOD_BK00_77_L      _PK_L_(0x00, 0x77)
#define REG_MOD_BK00_77_H      _PK_H_(0x00, 0x77)
#define REG_MOD_BK00_78_L      _PK_L_(0x00, 0x78)
#define REG_MOD_BK00_78_H      _PK_H_(0x00, 0x78)
#define REG_MOD_BK00_79_L      _PK_L_(0x00, 0x79)
#define REG_MOD_BK00_79_H      _PK_H_(0x00, 0x79)
#define REG_MOD_BK00_7A_L      _PK_L_(0x00, 0x7A)
#define REG_MOD_BK00_7A_H      _PK_H_(0x00, 0x7A)
#define REG_MOD_BK00_7B_L      _PK_L_(0x00, 0x7B)
#define REG_MOD_BK00_7B_H      _PK_H_(0x00, 0x7B)
#define REG_MOD_BK00_7C_L      _PK_L_(0x00, 0x7C)
#define REG_MOD_BK00_7C_H      _PK_H_(0x00, 0x7C)
#define REG_MOD_BK00_7D_L      _PK_L_(0x00, 0x7D)
#define REG_MOD_BK00_7D_H      _PK_H_(0x00, 0x7D)
#define REG_MOD_BK00_7E_L      _PK_L_(0x00, 0x7E)
#define REG_MOD_BK00_7E_H      _PK_H_(0x00, 0x7E)
#define REG_MOD_BK00_7F_L      _PK_L_(0x00, 0x7F)
#define REG_MOD_BK00_7F_H      _PK_H_(0x00, 0x7F)


//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#ifdef MSOS_TYPE_CE

	#if (1==WITHIN_MHAL_PNL_DLL)
	#define INTERFACE	extern
	#else
	#define INTERFACE	__declspec(dllimport)
	#endif


	#ifdef _HAL_PNL_C_

	#pragma data_seg(".PNLRiu")
	MS_U32 gu32PnlRiuBaseAddr 	=0x0;	 // This should be inited before XC library starting.
	MS_U32 gu32PMRiuBaseAddr	=0x0;
	#pragma data_seg()
	#pragma comment(linker,"/SECTION:.PNLRiu,RWS")

	#else

	INTERFACE MS_U32 gu32PnlRiuBaseAddr;     // This should be inited before XC library starting.
	INTERFACE MS_U32 gu32PMRiuBaseAddr;

	#endif

#else

	#ifdef _HAL_PNL_C_
	#define INTERFACE
	#else
	#define INTERFACE	extern
	#endif


	INTERFACE MS_U32 gu32PnlRiuBaseAddr;     // This should be inited before XC library starting.
	INTERFACE MS_U32 gu32PMRiuBaseAddr;

#endif
#undef INTERFACE



#define _BITMASK(loc_msb, loc_lsb) ((1U << (loc_msb)) - (1U << (loc_lsb)) + (1U << (loc_msb)))
#define BITMASK(x) _BITMASK(1?x, 0?x)
#define HBMASK    0xFF00
#define LBMASK    0x00FF

#define RIU_MACRO_START     do {
#define RIU_MACRO_END       } while (0)

// Address bus of RIU is 16 bits.
#define RIU_READ_BYTE(addr)         ( READ_BYTE( gu32PMRiuBaseAddr + (addr) ) )
#define RIU_READ_2BYTE(addr)        ( READ_WORD( gu32PMRiuBaseAddr + (addr) ) )
#define RIU_WRITE_BYTE(addr, val)   WRITE_BYTE( gu32PMRiuBaseAddr + (addr), val)
#define RIU_WRITE_2BYTE(addr, val)  WRITE_WORD( gu32PMRiuBaseAddr + (addr), val)

// Address bus of RIU is 16 bits for PM //alex_tung
#define PM_RIU_READ_BYTEM(addr)        ( READ_BYTE( gu32PMRiuBaseAddr + (addr) ) )
#define PM_RIU_READ_2BYTE(addr)        ( READ_WORD( gu32PMRiuBaseAddr + (addr) ) )
#define PM_RIU_WRITE_BYTE(addr, val)   WRITE_BYTE( gu32PMRiuBaseAddr + (addr), val)
#define PM_RIU_WRITE_2BYTE(addr, val)  WRITE_WORD( gu32PMRiuBaseAddr + (addr), val)

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------

//=============================================================
// Just for Scaler
#if XC_REGISTER_SPREAD

#define SC_W2BYTE( u32Reg, u16Val)\
            RIU_WRITE_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) ) << 1 , u16Val  )

#define SC_R2BYTE( u32Reg ) \
            RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) ) <<1 )

#define SC_W4BYTE( u32Reg, u32Val)\
            RIU_WRITE_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) ) <<1, (MS_U16)((u32Val) & 0x0000FFFF) ) ; \
            RIU_WRITE_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) + 2 ) << 1, (MS_U16)(((u32Val) >> 16) & 0x0000FFFF) )

#define SC_R4BYTE( u32Reg )\
            RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) ) << 1 ) | ((MS_U32)(RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) + 2 ) << 1 )) << 16)

#define SC_R2BYTEMSK( u32Reg, u16mask)\
            (RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) ) << 1) & (u16mask))

#define SC_W2BYTEMSK( u32Reg, u16Val, u16Mask)\
            RIU_WRITE_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) ) << 1, (RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) ) << 1) & ~(u16Mask) ) | ((u16Val) & (u16Mask)) )

#else
#define SC_W2BYTE( u32Reg, u16Val)\
            RIU_WRITE_2BYTE(REG_SCALER_BASE << 1, ((u32Reg) >> 8) & 0x00FF ) ; \
            RIU_WRITE_2BYTE( (REG_SCALER_BASE +((u32Reg) & 0xFF) ) << 1 , u16Val  )

#define SC_R2BYTE( u32Reg ) \
            RIU_WRITE_2BYTE(REG_SCALER_BASE << 1, ( (u32Reg) >> 8) & 0x00FF ) ; \
            RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFF) )<<1 )

#define SC_W4BYTE( u32Reg, u32Val)\
            RIU_WRITE_2BYTE(REG_SCALER_BASE<<1, ((u32Reg) >> 8) & 0x00FF ) ; \
            RIU_WRITE_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFF) ) <<1, (MS_U16)((u32Val) & 0x0000FFFF) ) ; \
            RIU_WRITE_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFF) + 2 ) << 1, (MS_U16)(((u32Val) >> 16) & 0x0000FFFF) )

#define SC_R4BYTE( u32Reg )\
            RIU_WRITE_2BYTE(REG_SCALER_BASE << 1, ((u32Reg) >> 8) & 0x00FF ) ; \
            RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFF) ) << 1 ) | ((MS_U32)(RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFF) + 2 ) << 1 )) << 16)

#define SC_R2BYTEMSK( u32Reg, u16mask)\
            RIU_WRITE_2BYTE(REG_SCALER_BASE << 1, ((u32Reg) >> 8) & 0x00FF ) ; \
            RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFF) ) << 1) & (u16mask)

#define SC_W2BYTEMSK( u32Reg, u16Val, u16Mask)\
            RIU_WRITE_2BYTE(REG_SCALER_BASE << 1, ((u32Reg) >> 8) & 0x00FF ) ; \
            RIU_WRITE_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFF) ) << 1, (RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFF) ) << 1) & ~(u16Mask) ) | ((u16Val) & (u16Mask)) )
#endif

//=============================================================
/// for MOD
#if REG_MOD_BASE != 0x303200
#define MOD_W2BYTE( u32Reg, u16Val)\
            RIU_WRITE_2BYTE(REG_MOD_BASE << 1, ((u32Reg) >> 8) & 0x00FF ); \
            RIU_WRITE_2BYTE( (REG_MOD_BASE + ((u32Reg) & 0xFF) ) << 1, u16Val )


#if ( defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_NOS) || defined(MSOS_TYPE_LINUX_KERNEL))
static MS_U16 MOD_R2BYTE( MS_U32 u32Reg )
{
    RIU_WRITE_2BYTE(REG_MOD_BASE << 1, ((u32Reg) >> 8) & 0x00FF );
    return RIU_READ_2BYTE( (REG_MOD_BASE + ((u32Reg) & 0xFF) ) << 1);
}

static MS_U16 MOD_R2BYTEMSK( MS_U32 u32Reg, MS_U16 u16mask)
{
    RIU_WRITE_2BYTE(REG_MOD_BASE << 1, ((u32Reg) >> 8) & 0x00FF );
    return RIU_READ_2BYTE( (REG_MOD_BASE + ((u32Reg) & 0xFF) ) << 1) & (u16mask);
}

#else
#define MOD_R2BYTE( u32Reg ) \
            RIU_WRITE_2BYTE(REG_MOD_BASE << 1, ((u32Reg) >> 8) & 0x00FF ); \
            RIU_READ_2BYTE( (REG_MOD_BASE + ((u32Reg) & 0xFF) ) << 1)

#define MOD_R2BYTEMSK( u32Reg, u16mask)\
            RIU_WRITE_2BYTE(REG_MOD_BASE << 1, ((u32Reg) >> 8) & 0x00FF ); \
            RIU_READ_2BYTE( (REG_MOD_BASE + ((u32Reg) & 0xFF) ) << 1) & (u16mask)

#endif



#define MOD_W2BYTEMSK( u32Reg, u16Val, u16Mask)\
            RIU_WRITE_2BYTE(REG_MOD_BASE << 1, ((u32Reg) >> 8) & 0x00FF ); \
            RIU_WRITE_2BYTE( (REG_MOD_BASE + ((u32Reg) & 0xFF) )<<1 , (RIU_READ_2BYTE( (REG_MOD_BASE + ((u32Reg) & 0xFF) ) << 1 ) & ~(u16Mask)) | ((u16Val) & (u16Mask)) )
#else
#define MOD_W2BYTE( u32Reg, u16Val)\
            RIU_WRITE_2BYTE( (REG_MOD_BASE + ((u32Reg) & 0xFF) ) << 1, u16Val )

#define MOD_R2BYTE( u32Reg ) \
            RIU_READ_2BYTE( (REG_MOD_BASE + ((u32Reg) & 0xFF) ) << 1)

#define MOD_R2BYTEMSK( u32Reg, u16mask)\
            RIU_READ_2BYTE( (REG_MOD_BASE + ((u32Reg) & 0xFF) ) << 1) & (u16mask)

#define MOD_W2BYTEMSK( u32Reg, u16Val, u16Mask)\
            RIU_WRITE_2BYTE( (REG_MOD_BASE + ((u32Reg) & 0xFF) )<<1 , (RIU_READ_2BYTE( (REG_MOD_BASE + ((u32Reg) & 0xFF) ) << 1 ) & ~(u16Mask)) | ((u16Val) & (u16Mask)) )

#endif

//=============================================================
//General ( Make sure u32Reg is not ODD
#define W2BYTE( u32Reg, u16Val) RIU_WRITE_2BYTE( (u32Reg) << 1 , u16Val )


#define R2BYTE( u32Reg ) RIU_READ_2BYTE( (u32Reg) << 1)

#define W4BYTE( u32Reg, u32Val)\
            RIU_WRITE_2BYTE( (u32Reg) << 1, ((u32Val) & 0x0000FFFF) ); \
            RIU_WRITE_2BYTE( ( (u32Reg) + 2) << 1 , (((u32Val) >> 16) & 0x0000FFFF))

#define R4BYTE( u32Reg )\
            ((RIU_READ_2BYTE( (u32Reg) << 1)) | ((MS_U32)(RIU_READ_2BYTE( ( (u32Reg) + 2 ) << 1) ) << 16))

#define R1BYTEMSK( u32Reg, u16mask)\
            RIU_READ_BYTE(((u32Reg) << 1) - ((u32Reg) & 1)) & u16mask

#define W1BYTEMSK( u32Addr, u8Val, u8mask)     \
            RIU_WRITE_BYTE( (((u32Addr) <<1) - ((u32Addr) & 1)), (RIU_READ_BYTE((((u32Addr) <<1) - ((u32Addr) & 1))) & ~(u8mask)) | ((u8Val) & (u8mask)))


#define R2BYTEMSK( u32Reg, u16mask)\
            RIU_READ_2BYTE( (u32Reg)<< 1) & u16mask

#define W2BYTEMSK( u32Reg, u16Val, u16Mask)\
            RIU_WRITE_2BYTE( (u32Reg)<< 1 , (RIU_READ_2BYTE((u32Reg) << 1) & ~(u16Mask)) | ((u16Val) & (u16Mask)) )

//=============================================================
// Just for PM registers

/*
    R/W register forced to use 8 bit address, everytime need to r/w 2 bytes with mask

    32bit address     16 bit address        8 bit address
        0                   0                       0
        1                   x                       1
        2
        3
        4                   1                       2
        5                   x                       3
        6
        7
        8                   2                       4
        9                   x                       5
        A
        B                                                                                  */

// to read 0x2F03[3], please use R1BYTE(0x2F03, 3:3)
#define PM_R1BYTE(u32Addr, u8mask)            \
    (READ_BYTE (gu32PMRiuBaseAddr + (u32Addr << 1) - (u32Addr & 1)) & BMASK(u8mask))

// to write 0x2F02[4:3] with 2'b10, please use W1BYTE(0x2F02, 0x02, 4:3)
#define PM_W1BYTE(u32Addr, u8Val, u8mask)     \
    (WRITE_BYTE(gu32PMRiuBaseAddr + (u32Addr << 1) - (u32Addr & 1), (PM_R1BYTE(u32Addr, 7:0) & ~BMASK(u8mask)) | (BITS(u8mask, u8Val) & BMASK(u8mask))))
#if 1//alex_tung
// access PM registers
// u32Addr must be 16bit aligned
#define PM_R2BYTE( u32Reg, u16mask)\
            ( PM_RIU_READ_2BYTE( (u32Reg)<< 1) & u16mask )
    // u32Addr must be 16bit aligned
#define PM_W2BYTE( u32Reg, u16Val, u16Mask)\
            (  PM_RIU_WRITE_2BYTE( (u32Reg)<< 1 , (PM_RIU_READ_2BYTE((u32Reg) << 1) & ~(u16Mask)) | ((u16Val) & (u16Mask)) )  )
#else
// u32Addr must be 16bit aligned
#define PM_R2BYTE(u32Addr, u16mask)            \
    (READ_WORD (_PM_RIU_BASE + (u32Addr << 1)) & BMASK(u16mask))

// u32Addr must be 16bit aligned
#define PM_W2BYTE(u32Addr, u16Val, u16mask)    \
    (WRITE_WORD(_PM_RIU_BASE + (u32Addr << 1), (PM_R2BYTE(u32Addr, 15:0) & ~BMASK(u16mask)) | (BITS(u16mask, u16Val) & BMASK(u16mask))))
#endif

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------

#undef INTERFACE

#endif

