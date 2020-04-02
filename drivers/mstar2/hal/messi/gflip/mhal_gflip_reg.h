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
/// @file   mhal_gflip_reg.h
/// @brief  MStar gflip reg defines
/// @author MStar Semiconductor Inc.
/// @attention
/// <b><em>The g(graphics, gop/ge) flip will have related regs with GOP/GE</em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef _MHAL_GFLIP_REG_H
#define _MHAL_GFLIP_REG_H

//=============================================================================
// Includs
//=============================================================================

//=============================================================================
// Defines & Macros
//=============================================================================
#define BMASK(bits)                             (BIT(((1)?bits)+1)-BIT(((0)?bits)))
#define BITS(bits, val)                         ((BIT(((1)?bits)+1)-BIT(((0)?bits))) & (val<<((0)?bits)))
//----------------------------------------------------------------------------
// GFLIP related Reg Defines
//----------------------------------------------------------------------------
#if defined(CONFIG_ARM64)
extern ptrdiff_t mstar_pm_base;
#define RIU_MAP                                 (mstar_pm_base+0x200000UL)
#else
#define RIU_MAP                                 0xFD200000UL
#endif

#define RIU                                     ((unsigned short volatile *) RIU_MAP)
#define RIU8                                    ((unsigned char  volatile *) RIU_MAP)

//----------------------------------------------------------------------------
// GFLIP related GOP Reg Defines
//----------------------------------------------------------------------------
#define REG_GOP_BASE                            (0x20200)
#define REG_MIU0_BASE                           (0x01200)
#define REG_MIU1_BASE                           (0x00600)
#define REG_MIU2_BASE                           (0x62000)
#define GOP_REG(addr, bk)                       GOP_DIR_REG(addr,REG_GOP_BASE+(((MS_U16)bk)<<8))
#define GOP_DIR_REG(addr,REG_GOP_DIR_BASE)      RIU[(addr<<1)+REG_GOP_DIR_BASE]
#define MIU0_REG(addr)                          GOP_DIR_REG(addr,REG_MIU0_BASE)
#define MIU1_REG(addr)                          GOP_DIR_REG(addr,REG_MIU1_BASE)
#define MIU2_REG(addr)                          GOP_DIR_REG(addr,REG_MIU2_BASE)
#define GOP_MIU_REG                             0x7B
#define GOP_MIU_CLIENT                          (BIT(1)|BIT(2)|BIT(3)|BIT(4))

#define GOP_BANK_OFFSET                         0x3
#define GOP0_BK_OFST                            0x0
#define GOP1_BK_OFST                            (0x1*GOP_BANK_OFFSET)
#define GOP2_BK_OFST                            (0x2*GOP_BANK_OFFSET)

#define REG_GOP_DRAM_RBLK_STR_L(widx)           (0x0001+(widx<<5))
#define REG_GOP_DRAM_RBLK_STR_H(widx)           (0x0002+(widx<<5))

#define REG_GOP_3DOSD_SUB_RBLK_L(widx)          (0x001E + (widx<<5))
#define REG_GOP_3DOSD_SUB_RBLK_H(widx)          (0x001F + (widx<<5))

#define REG_GOP4G_GWIN_CTRL0(widx)              (0x0000+(widx<<5))
    #define GOP_GWIN_ENABLE_MASK                BIT(0)

#define REG_GOP4G_CTRL0                         0x0000
    #define GOP_RST_MASK                        BIT(0)

#define REG_GOP_CTRL1                           0x0001
    #define GOP_DST_MASK                        BMASK(2:0)

#define REG_GOP_INT                             0x0008
    #define GOP_INTMASK_VS0                     BIT(0)
    #define GOP_INTMASK_VS1                     BIT(1)
    #define GOP_INT_VS0                         BIT(8)
    #define GOP_INT_VS1                         BIT(9)

#define REG_GOP_BANK_SEL_EX                     0x007c
    #define GOP1GX0_INT_FLAG                    BMASK(0:0)
    #define GOP1GX0_WR_ACK                      BMASK(4:4)

#define REG_GOP_MUX                             0x007e
    #define GOPD_INT_FLAG_MASK                  BMASK(12:12)
    #define GOPD_INT_FLAG                       BIT(12)
    #define GOPD_WR_ACK                         BIT(13)

#define REG_GOP_BANK_SEL                        0x007f
    #define GOP_BANK_SEL                        BMASK(3:0)
    #define GOP_INT_FLAG_MASK                   BMASK(7:4)
    #define GOP2G_INT_FLAG                      BIT(4)
    #define GOP2GX_INT_FLAG                     BIT(5)
    #define GOP1G_INT_FLAG                      BIT(6)
    #define GOP1GX_INT_FLAG                     BIT(7)
    #define GOP_WR                              BIT(8)
    #define GOP_FWR                             BIT(9)
    #define GOP_BK_WR                           BIT(10)
    #define GOP_FCLR                            BIT(11)
    #define GOP2G_WR_ACK                        BIT(12)
    #define GOP2GX_WR_ACK                       BIT(13)
    #define GOP1G_WR_ACK                        BIT(14)
    #define GOP1GX_WR_ACK                       BIT(15)

#define REG_GOP_DWIN_INT                        0x0002
    #define GOP_DWIN_INTMASK_WADR               BIT(3)
    #define GOP_DWIN_INTMASK_PROG               BIT(4)
    #define GOP_DWIN_INTMASK_TF                 BIT(5)
    #define GOP_DWIN_INTMASK_BF                 BIT(6)
    #define GOP_DWIN_INTMASK_VS                 BIT(7)
    #define GOP_DWIN_INT_WADR                   BIT(11)
    #define GOP_DWIN_INT_PROG                   BIT(12)
    #define GOP_DWIN_INT_TF                     BIT(13)
    #define GOP_DWIN_INT_BF                     BIT(14)
    #define GOP_DWIN_INT_VS                     BIT(15)

#define REG_GOP_TLB_MAIN_ADDR_L                 0x0058
#define REG_GOP_TLB_MAIN_ADDR_H                 0x0059
#define REG_GOP_TLB_SUB_ADDR_L                  0x005a
#define REG_GOP_TLB_SUB_ADDR_H                  0x005b

//----------------------------------------------------------------------------
// GFLIP related chiptop Reg Defines
//----------------------------------------------------------------------------
#define CKG_REG_BASE                            0xB00
#define CKG_REG(addr)                           RIU[(addr<<1)+CKG_REG_BASE]

/* GOP0 and GOP1 CLK */
#define GOP_GOPCLK                              0x40
    #define CKG_GOPG0_DISABLE_CLK               ~(BIT(0))
    #define CKG_GOPG0_MASK                      BMASK(4:2)
    #define CKG_GOPG0_ODCLK                     BITS(4:2, 0)
    #define CKG_GOPG0_IDCLK2                    BITS(4:2, 1)
    #define CKG_GOPG0_IDCLK1                    BITS(4:2, 3)

    #define CKG_GOPG1_DISABLE_CLK               ~(BIT(8))
    #define CKG_GOPG1_MASK                      BMASK(12:10)
    #define CKG_GOPG1_ODCLK                     BITS(12:10, 0)
    #define CKG_GOPG1_IDCLK2                    BITS(12:10, 1)
    #define CKG_GOPG1_IDCLK1                    BITS(12:10, 3)

/* GOP2 and GOPDWIN CLK */
#define GOP_GOP2CLK                             0x41
    #define CKG_GOPG2_DISABLE_CLK               ~(BIT(0))
    #define CKG_GOPG2_MASK                      BMASK(4:2)
    #define CKG_GOPG2_ODCLK                     BITS(4:2, 0)
    #define CKG_GOPG2_IDCLK2                    BITS(4:2, 1)
    #define CKG_GOPG2_IDCLK1                    BITS(4:2, 3)

    #define CKG_GOPD_MASK                       BMASK(12:10)
    #define CKG_GOPD_CLK_ODCLK                  BMASK(12:10)

/* GOP3 CLK*/
#define GOP_GOP3CLK                             0x42
    #define CKG_GOPG3_MASK                      BMASK(4:2)
    #define CKG_GOPG3_ODCLK                     BITS(4:2, 0)
    #define CKG_GOPG3_IDCLK2                    BITS(4:2, 1)
    #define CKG_GOPG3_IDCLK1                    BITS(4:2, 2)

/* GOP4 CLK*/
#define GOP_GOP4CLK                             0x7E
    #define CKG_GOPG4_MASK                      BMASK(12:10)
    #define CKG_GOPG4_ODCLK                     BITS(12:10, 0)
    #define CKG_GOPG4_IDCLK2                    BITS(12:10, 1)
    #define CKG_GOPG4_IDCLK1                    BITS(12:10, 2)

    #define CKG_GOPD_DISABLE_CLK                ~(BIT(8))

#define GOP_SRAMCLK             0x43
/* GOP SRAM CLK*/
#define GOP_SCLCLK              0x44
#define GOP_LB_SRAMCLK          0x45
//----------------------------------------------------------------------------
// GFLIP related GE Reg Defines
//----------------------------------------------------------------------------
#define REG_GE_BASE                             (0x2800)
#define GE_REG(addr)                            RIU[(addr<<1)+REG_GE_BASE]

#define REG_GE_FMT_BLT                          0x01
    #define GE_EN_CMDQ                          BIT(0)
    #define GE_EN_VCMDQ                         BIT(1)

#define REG_GE_VQ_FIFO_STATUS_L                 0x0004
#define REG_GE_VQ_FIFO_STATUS_H                 0x0005

#define REG_GE_STATUS                           0x0007
    #define GE_BUSY                             BIT(0)
    #define GE_CMDQ1_STATUS                     BMASK(7:3)
    #define GE_CMDQ2_STATUS                     BMASK(15:11)

#define REG_GE_TAG                              0x0032

//----------------------------------------------------------------------------
// GFLIP related VE Reg Defines, Used for VE capture
//----------------------------------------------------------------------------
#define REG_VE_BASE                             (0x3B00)
#define VE_REG(addr)                            RIU[(addr<<1)+REG_VE_BASE]

#define REG_VE_CTRL                             0x00
    #define BIT_VE_ENABLE                       BIT(0)
    #define BIT_VE_RESET                        BIT(4)
    #define BIT_VE_REG_RELOAD                   BIT(5)

#define REG_VE_STATE                            0x7F
    #define BIT_VE_START_DOWNSCALING            BIT(0)

#define REG_VE_COUNTER                          0x70
    #define BIT_VE_FIELD_IDX                    BMASK(15:14)
    #define BIT_VE_FIELD_IDX_SHIFT_BITS         14

//----------------------------------------------------------------------------
// GFLIP related GS Reg Defines
//----------------------------------------------------------------------------
#define CKG_GS_BASE                           0x33D00
#define GS_REG(addr)                           RIU[(addr<<1)+CKG_GS_BASE]
#define REG_GS_VSP_SRAM                  0x0D

#define SC_REG_BASE                            0x2F00
#define SC_REG(addr)                           RIU[(addr<<1)+SC_REG_BASE]
#define GOP_SC_BANKSEL                          0x0
#define GOP_SC_GOPEN                            0x0
#define GOP_SC1_GOPEN                           0x80
#define GOP_SC_IP2SC                            0x5
#define GOP_SC_VOP2BLENDING_L                   0x6
#define GOP_SC_VOP2BLENDING_H                   0x6
#define GOP_SC_OCBANKSEL                        0x37
#define GOP_SC_OCMIXER_ALPHA                    0x28

#define GOP_SC_GOPBLENDING                      GOP_SC_GOPEN
#define GOP_SC_GOPBLENDING_L                    GOP_SC_VOP2BLENDING_L
#define GOP_SC_GOPBLENDING_H                    GOP_SC_VOP2BLENDING_H
#endif //_MHAL_GFLIP_REG_H

