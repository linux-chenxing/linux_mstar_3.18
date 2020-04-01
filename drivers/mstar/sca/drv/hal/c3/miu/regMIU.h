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

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    regMIU.h
/// @brief  MIU Control Register Definition
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _REG_MIU_H_
#define _REG_MIU_H_


//-------------------------------------------------------------------------------------------------
//  Hardware Capability
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define BITS_RANGE(range)                       (BIT(((1)?range)+1) - BIT((0)?range))
#define BITS_RANGE_VAL(x, range)                ((x & BITS_RANGE(range)) >> ((0)?range))

#define MIU_REG_BASE                            (0x1200)
#define MIU1_REG_BASE                           (0x0600)
#define PM_REG_BASE                             (0x1E00)
#define MIU_ATOP_BASE                           (0x10D00)

#define PM_CHIP_REVISION                        (PM_REG_BASE+0x03) // 0x1E03
#define DDR_FREQ_SET_0                          (MIU_REG_BASE+0x20) // 0x1220
#define DDR_FREQ_SET_1                          (MIU_REG_BASE+0x21) //0x1221
#define DDR_FREQ_DIV_1                          (MIU_REG_BASE+0x25) //0x1225
#define DDR_FREQ_INPUT_DIV_2                    (MIU_REG_BASE+0x26) //0x1226
#define DDR_FREQ_LOOP_DIV_2                     (MIU_REG_BASE+0x27) //0x1227
#define DDR_CLK_SELECT                          (MIU_REG_BASE+0x3e) //0x123E
#define DDR_FREQ_STATUS                         (MIU_REG_BASE+0x3f) //0x123F

#define MIU_RQ0L_MASK                           (MIU_REG_BASE+0x46)
#define MIU_RQ0H_MASK                           (MIU_REG_BASE+0x47)
#define MIU_RQ1L_MASK                           (MIU_REG_BASE+0x66)
#define MIU_RQ1H_MASK                           (MIU_REG_BASE+0x67)
#define MIU_RQ2L_MASK                           (MIU_REG_BASE+0x86)
#define MIU_RQ2H_MASK                           (MIU_REG_BASE+0x87)
#define REG_MIU_RQX_MASK(x)                     (0x46+0x20*x)
#define REG_MIU_RQX_HPMASK(x)                   (0x48+0x20*x)

#define MIU_PROTECT_L_EN                        (MIU_REG_BASE+0xC0)
#define MIU_PROTECT_H_EN                        (MIU_REG_BASE+0x30)
#define MIU_PROTECT_DDR_SIZE                    (MIU_REG_BASE+0xC1)
#define MIU_PROTECT_DDR_SIZE_MASK               BITS_RANGE(11:8)
#define MIU_PROTECT_DDR_32MB                    (0x50)
#define MIU_PROTECT_DDR_64MB                    (0x60)
#define MIU_PROTECT_DDR_128MB                   (0x70)
#define MIU_PROTECT_DDR_256MB                   (0x80)
#define MIU_PROTECT_DDR_512MB                   (0x90)
#define MIU_PROTECT_DDR_1024MB                  (0xA0)
#define MIU_PROTECT_DDR_2048MB                  (0xB0)

#define MIU_PROTECT0_ID0                        (MIU_REG_BASE+0xC2)
#define MIU_PROTECT1_ID0                        (MIU_REG_BASE+0xCE)
#define MIU_PROTECT2_ID0                        (MIU_REG_BASE+0xD4)
#define MIU_PROTECT3_ID0                        (MIU_REG_BASE+0x32)
#define MIU_PROTECT4_ID0                        (MIU_REG_BASE+0x38)
#define REG_MIU_PROTECT_LOADDR                  (0x6D << 1)	//0xDE
#define REG_MIU_PROTECT_HIADDR                  (0x6E << 1)	//0xDE
#define REG_MIU_GROUP_PRIORITY                  (0x6E << 1)
#define REG_MIU_PROTECT_STATUS                  (0x6F << 1)	//0xDE

// MIU selection registers
#define REG_MIU_SEL0                            (MIU_REG_BASE+0xf0)  //0x12F0
#define REG_MIU_SEL1                            (MIU_REG_BASE+0xf1)  //0x12F1
#define REG_MIU_SEL2                            (MIU_REG_BASE+0xf2)  //0x12F2
#define REG_MIU_SEL3                            (MIU_REG_BASE+0xf3)  //0x12F3
#define REG_MIU_SEL4                            (MIU_REG_BASE+0xf4)  //0x12F4
#define REG_MIU_SEL5                            (MIU_REG_BASE+0xf5)  //0x12F5
#define REG_MIU_SEL6                            (MIU_REG_BASE+0xf6)  //0x12F6
#define REG_MIU_SEL7                            (MIU_REG_BASE+0xf7)  //0x12F7
#define REG_MIU_SELX(x)                         (0xF0+x*2)
//MIU1
#define MIU1_RQ1L_MASK                           (MIU1_REG_BASE+0x66)
#define MIU1_PROTECT_L_EN                        (MIU1_REG_BASE+0xC0)
#define MIU1_PROTECT_H_EN                        (MIU1_REG_BASE+0x30)
#define MIU1_PROTECT_SDR_LIKE                    (MIU1_REG_BASE+0xC1)
#define MIU1_PROTECT_DDR_SIZE                    (MIU1_REG_BASE+0xC1)

#define MIU1_PROTECT0_ID0                        (MIU1_REG_BASE+0xC2)
#define MIU1_PROTECT1_ID0                        (MIU1_REG_BASE+0xCE)
#define MIU1_PROTECT2_ID0                        (MIU1_REG_BASE+0xD4)
#define MIU1_PROTECT3_ID0                        (MIU1_REG_BASE+0x32)
#define MIU1_PROTECT4_ID0                        (MIU1_REG_BASE+0x38)

#define REG_MIU_I64_MODE                         (BIT7)
#define REG_MIU_INIT_DONE                        (BIT15)

//Protection Status
#define REG_MIU_PROTECT_LOG_CLR                 (BIT0)
#define REG_MIU_PROTECT_IRQ_MASK                (BIT1)
#define REG_MIU_PROTECT_HIT_FALG                (BIT4)
#define REG_MIU_PROTECT_HIT_ID                  13:8
#define REG_MIU_PROTECT_HIT_NO                  7:5

//-------------------------------------------------------------------------------------------------
//MAU
//
//-------------------------------------------------------------------------------------------------
#define    RIUBASE_MAU0                           0x1840
#define    RIUBASE_MAU1                           0x1860


//-------------------------------------------------------------------------------------------------
//  MIU ATOP registers
//-------------------------------------------------------------------------------------------------
#define MIU_DDFSTEP                             (0x28)//0x110D28
#define MIU_SSC_EN                              (0x29)//0x110D29
#define MIU_DDFSPAN                             (0x2A)//0x110D2A
#define MIU_DDFSET                              (0x30)
#define MIU_PLL_INPUT_DIV_2ND                   (0x34)
#define MIU_PLL_LOOP_DIV_2ND                    (0x35)
//xxx_div_first
#define MIU_PLLCTRL                             (0x36)
#define MIU_DDRPLL_DIV_FIRST                    (0x37)

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------


#endif // _REG_MIU_H_

