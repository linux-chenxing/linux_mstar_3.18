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

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    regMIU.h
/// @brief  MIU Control Register Definition
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _REG_MIU_H_
#define _REG_MIU_H_

#include<linux/bitops.h>

//-------------------------------------------------------------------------------------------------
//  Hardware Capability
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define BITS_RANGE(range)                       (BIT(((1)?range)+1) - BIT((0)?range))
#define BITS_RANGE_VAL(x, range)                ((x & BITS_RANGE(range)) >> ((0)?range))

#define MIU_REG_BASE                            (0x1200UL)
#define MIU1_REG_BASE                           (0x0600UL)
#define MIU2_REG_BASE                           (0x62000UL)
#define MIU3_REG_BASE                           (0x62400UL)
#define PM_REG_BASE                             (0x1E00UL)
#define MIU_ATOP_BASE                           (0x10D00UL)
#define MIU1_ATOP_BASE                          (0x61600UL)
#define MIU2_ATOP_BASE                          (0x62100UL)
#define MIU3_ATOP_BASE                          (0x62500UL)
#define CHIP_TOP_BASE                           (0x1E00UL)
#define MIU_ARB_REG_BASE                        (0x61500UL)
#define MIU1_ARB_REG_BASE                       (0x62200UL)

#define PM_CHIP_REVISION                        (PM_REG_BASE + 0x03)    //0x001E03
//#define DDR_FREQ_SET_0                        (MIU_REG_BASE + 0x20)   //0x101220
//#define DDR_FREQ_SET_1                        (MIU_REG_BASE + 0x21)   //0x101221
//#define DDR_FREQ_DIV_1                        (MIU_REG_BASE + 0x25)   //0x101225
//#define DDR_FREQ_INPUT_DIV_2                  (MIU_REG_BASE + 0x26)   //0x101226
//#define DDR_FREQ_LOOP_DIV_2                   (MIU_REG_BASE + 0x27)   //0x101227
//#define DDR_CLK_SELECT                        (MIU_REG_BASE + 0x3e)   //0x10123E
//#define DDR_FREQ_STATUS                       (MIU_REG_BASE + 0x3f)   //0x10123F

#define MIU_RQ0L_MASK                           (MIU_REG_BASE + 0x46)
#define MIU_RQ0H_MASK                           (MIU_REG_BASE + 0x47)
#define MIU_RQ1L_MASK                           (MIU_REG_BASE + 0x66)
#define MIU_RQ1H_MASK                           (MIU_REG_BASE + 0x67)
#define MIU_RQ2L_MASK                           (MIU_REG_BASE + 0x86)
#define MIU_RQ2H_MASK                           (MIU_REG_BASE + 0x87)
#define REG_MIU_RQX_MASK(x)                     (0x46 + 0x20 * x)
#define REG_MIU_RQX_HPMASK(x)                   (0x48 + 0x20 * x)

#define MIU_PROTECT_EN                          (MIU_REG_BASE + 0xD2)
#define MIU_PROTECT_DDR_SIZE                    (MIU_REG_BASE + 0xD3)
//#define MIU_PROTECT_DDR_SIZE_MASK             BITS_RANGE(11:8)
#define MIU_PROTECT_DDR_32MB                    (0x50)
#define MIU_PROTECT_DDR_64MB                    (0x60)
#define MIU_PROTECT_DDR_128MB                   (0x70)
#define MIU_PROTECT_DDR_256MB                   (0x80)
#define MIU_PROTECT_DDR_512MB                   (0x90)
#define MIU_PROTECT_DDR_1024MB                  (0xA0)
#define MIU_PROTECT_DDR_2048MB                  (0xB0)

#define MIU_PROTECT0_ID0                        (MIU_REG_BASE + 0x2E)
#define MIU_BW_REQUEST                          (MIU_REG_BASE + 0x1A)
#define MIU_BW_RESULT                           (MIU_REG_BASE + 0x1C)
#define MIU_PROTECT0_ID_ENABLE                  (MIU_REG_BASE + 0x20)
#define MIU_PROTECT1_ID_ENABLE                  (MIU_REG_BASE + 0x22)
#define MIU_PROTECT2_ID_ENABLE                  (MIU_REG_BASE + 0x24)
#define MIU_PROTECT3_ID_ENABLE                  (MIU_REG_BASE + 0x26)
#define MIU_PROTECT0_MSB                        (MIU_REG_BASE + 0xD0)
#define MIU_PROTECT0_START                      (MIU_REG_BASE + 0xC0)
#define MIU_PROTECT1_START                      (MIU_REG_BASE + 0xC4)
#define MIU_PROTECT2_START                      (MIU_REG_BASE + 0xC8)
#define MIU_PROTECT3_START                      (MIU_REG_BASE + 0xCC)
#define REG_MIU_PROTECT_LOADDR                  (0x6D << 1) //0xDE
#define REG_MIU_PROTECT_HIADDR                  (0x6E << 1) //0xDE
#define REG_MIU_GROUP_PRIORITY                  (0x7F << 1)
#define REG_MIU_PROTECT_STATUS                  (0x6F << 1) //0xDE
#define REG_MIU_PROTECT_LOG                     (MIU_REG_BASE + 0xDE)

// MIU selection registers
#define REG_MIU_SEL0                            (MIU_REG_BASE + 0xf0)  //0x12F0
#define REG_MIU_SEL1                            (MIU_REG_BASE + 0xf2)  //0x12F1
#define REG_MIU_SEL2                            (MIU_REG_BASE + 0xf4)  //0x12F2
#define REG_MIU_SEL3                            (MIU_REG_BASE + 0xf6)  //0x12F3
#define REG_MIU_SELX(x)                         (0xF0 + x * 2)

//MIU1
#define MIU1_PROTECT_EN                         (MIU1_REG_BASE + 0xD2)
#define MIU1_PROTECT_DDR_SIZE                   (MIU1_REG_BASE + 0xD3)
#define MIU1_PROTECT0_ID0                       (MIU1_REG_BASE + 0x2E)
#define MIU1_BW_REQUEST                         (MIU1_REG_BASE + 0x1A)
#define MIU1_BW_RESULT                          (MIU1_REG_BASE + 0x1C)
#define MIU1_PROTECT0_ID_ENABLE                 (MIU1_REG_BASE + 0x20)
#define MIU1_PROTECT1_ID_ENABLE                 (MIU1_REG_BASE + 0x22)
#define MIU1_PROTECT2_ID_ENABLE                 (MIU1_REG_BASE + 0x24)
#define MIU1_PROTECT3_ID_ENABLE                 (MIU1_REG_BASE + 0x26)
#define MIU1_PROTECT0_MSB                       (MIU1_REG_BASE + 0xD0)
#define MIU1_PROTECT0_START                     (MIU1_REG_BASE + 0xC0)
#define MIU1_PROTECT1_START                     (MIU1_REG_BASE + 0xC4)
#define MIU1_PROTECT2_START                     (MIU1_REG_BASE + 0xC8)
#define MIU1_PROTECT3_START                     (MIU1_REG_BASE + 0xCC)
#define REG_MIU1_PROTECT_LOG                    (MIU1_REG_BASE + 0xDE)

//MIU2
#define MIU2_PROTECT_EN                          (MIU2_REG_BASE+0xD2)
#define MIU2_PROTECT_DDR_SIZE                    (MIU2_REG_BASE+0xD3)
#define MIU2_PROTECT0_ID0                        (MIU2_REG_BASE+0x2E)
#define MIU2_BW_REQUEST                          (MIU2_REG_BASE+0x1A)
#define MIU2_BW_RESULT                           (MIU2_REG_BASE+0x1C)
#define MIU2_PROTECT0_ID_ENABLE                  (MIU2_REG_BASE+0x20)
#define MIU2_PROTECT1_ID_ENABLE                  (MIU2_REG_BASE+0x22)
#define MIU2_PROTECT2_ID_ENABLE                  (MIU2_REG_BASE+0x24)
#define MIU2_PROTECT3_ID_ENABLE                  (MIU2_REG_BASE+0x26)
#define MIU2_PROTECT0_MSB                        (MIU2_REG_BASE+0xD0)
#define MIU2_PROTECT0_START                      (MIU2_REG_BASE+0xC0)
#define MIU2_PROTECT1_START                      (MIU2_REG_BASE+0xC4)
#define MIU2_PROTECT2_START                      (MIU2_REG_BASE+0xC8)
#define MIU2_PROTECT3_START                      (MIU2_REG_BASE+0xCC)

//MIU2
#define MIU3_PROTECT_EN                         (MIU3_REG_BASE + 0xD2)
#define MIU3_PROTECT_DDR_SIZE                   (MIU3_REG_BASE + 0xD3)
#define MIU3_PROTECT0_ID0                       (MIU3_REG_BASE + 0x2E)
#define MIU3_BW_REQUEST                         (MIU3_REG_BASE + 0x1A)
#define MIU3_BW_RESULT                          (MIU3_REG_BASE + 0x1C)
#define MIU3_PROTECT0_ID_ENABLE                 (MIU3_REG_BASE + 0x20)
#define MIU3_PROTECT1_ID_ENABLE                 (MIU3_REG_BASE + 0x22)
#define MIU3_PROTECT2_ID_ENABLE                 (MIU3_REG_BASE + 0x24)
#define MIU3_PROTECT3_ID_ENABLE                 (MIU3_REG_BASE + 0x26)
#define MIU3_PROTECT0_MSB                       (MIU3_REG_BASE + 0xD0)
#define MIU3_PROTECT0_START                     (MIU3_REG_BASE + 0xC0)
#define MIU3_PROTECT1_START                     (MIU3_REG_BASE + 0xC4)
#define MIU3_PROTECT2_START                     (MIU3_REG_BASE + 0xC8)
#define MIU3_PROTECT3_START                     (MIU3_REG_BASE + 0xCC)

#define REG_MIU_I64_MODE                         (BIT7)
#define REG_MIU_INIT_DONE                        (BIT15)

//Protection Status
#define REG_MIU_PROTECT_LOG_CLR                 (BIT0)
#define REG_MIU_PROTECT_IRQ_MASK                (BIT1)
#define REG_MIU_PROTECT_HIT_FALG                (BIT4)
#define REG_MIU_PROTECT_HIT_ID                  14:8
#define REG_MIU_PROTECT_HIT_NO                  7:5

// MIU Scramble
#define REG_MIU_SCRAMBLE_EN                     (MIU_REG_BASE + 0x06)

//MIU Bus Width
#define REG_MI64_FORCE                          (CHIP_TOP_BASE+0x40UL) 

//MIU Slits control
#define REG_MIU0_SLIT_CTL                       (MIU_ARB_REG_BASE+0x44UL)
#define REG_MIU1_SLIT_CTL                       (MIU1_ARB_REG_BASE+0x44UL)
#define REG_MIU_SLIT_TOP_EN                     BIT0
#define REG_MIU0_SLIT_RW_ID0                    (MIU_ARB_REG_BASE+0x40UL)
#define REG_MIU1_SLIT_RW_ID0                    (MIU1_ARB_REG_BASE+0x40UL)
#define REG_MIU_SLIT_ID_ENABLE                  BIT7
#define REG_MIU0_SLIT_BASE_ADDR_LOW             (MIU_ARB_REG_BASE+0x4E)
#define REG_MIU0_SLIT_BASE_ADDR_HIGH            (MIU_ARB_REG_BASE+0x50)
#define REG_MIU1_SLIT_BASE_ADDR_LOW             (MIU1_ARB_REG_BASE+0x4E)
#define REG_MIU1_SLIT_BASE_ADDR_HIGH            (MIU1_ARB_REG_BASE+0x50)
#define REG_MIU0_SLIT_LOW_BOUND                 (MIU_ARB_REG_BASE+0x4A)
#define REG_MIU0_SLIT_HIGH_BOUND                (MIU_ARB_REG_BASE+0x4C)
#define REG_MIU1_SLIT_LOW_BOUND                 (MIU1_ARB_REG_BASE+0x4A)
#define REG_MIU1_SLIT_HIGH_BOUND                (MIU1_ARB_REG_BASE+0x4C)
#define REG_MIU0_SLIT_PROTECT_EN                (MIU_ARB_REG_BASE+0x53UL)
#define REG_MIU1_SLIT_PROTECT_EN                (MIU1_ARB_REG_BASE+0x53UL)
#define REG_MIU0_SLIT_PROTECT0_ID_ENABLE        (MIU_ARB_REG_BASE+0x58)
#define REG_MIU1_SLIT_PROTECT0_ID_ENABLE        (MIU1_ARB_REG_BASE+0x58)

//-------------------------------------------------------------------------------------------------
//MAU
//
//-------------------------------------------------------------------------------------------------
#define    RIUBASE_MAU0                           0x1840UL
#define    RIUBASE_MAU1                           0x1860UL


//-------------------------------------------------------------------------------------------------
//  MIU ATOP registers
//-------------------------------------------------------------------------------------------------
#define MIU_DDFSTEP                             (0x28)//0x110D28
#define MIU_SSC_EN                              (0x29)//0x110D29
#define MIU_DDFSPAN                             (0x2A)//0x110D2A
#define MIU_DDFSET                              (0x30)
//#define MIU_PLL_INPUT_DIV_2ND                 (0x34) // no this reg in Einstein
#define MIU_PLL_LOOP_DIV_2ND                    (0x34)
//xxx_div_first
#define MIU_PLLCTRL                             (0x36)
#define MIU_DDRPLL_DIV_FIRST                    (0x37)

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
#define SLIT_NUM                                (256)

typedef struct _SRAM_Slit_FlagCluster
{
    volatile MS_U8 flag[1];
    volatile MS_U8 dummy[31] ;
} SRAM_Slit_FlagCluster;

typedef struct _SRAM_Slit_FlagGroup
{
    SRAM_Slit_FlagCluster cluster[SLIT_NUM];
} SRAM_Slit_FlagGroup;

#endif // _REG_MIU_H_

