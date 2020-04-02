///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2016-2017 MStar Semiconductor, Inc.
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
///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//
// @file   mhal_graphic_reg.h
// @brief  Graphic Driver Interface
// @author MStar Semiconductor Inc.
//////////////////////////////////////////////////////////////////////////////////////////////////


//=============================================================================
// Defines & Macros
//=============================================================================
#define BMASK(bits)                             (BIT(((1)?bits)+1)-BIT(((0)?bits)))
#define GOP_REG_VAL(x)                          (1<<x)

#if defined(CONFIG_ARM64)
extern ptrdiff_t mstar_pm_base;
#define RIU_MAP                                 (mstar_pm_base+0x200000UL)
#else
#define RIU_MAP                                 0xFD200000UL
#endif

#define RIU                                     ((unsigned short volatile *) RIU_MAP)

#define GOP_WRITE2BYTE(addr, val)    { RIU[addr] = val; }
#define GOP_READ2BYTE(addr)            RIU[addr]


//----------------------------------------------------------------------------
// MIU Reg
//----------------------------------------------------------------------------
#define GOP_MIU_CLIENT_GOP0     0x5
#define GOP_MIU_CLIENT_GOP1     0x6
#define GOP_MIU_CLIENT_GOP2     0x7
#define GOP_MIU_CLIENT_GOP3     0x8
#define GOP_MIU_CLIENT_GOP4     0xff

//----------------------------------------------------------------------------
// GOP related GOP Reg Defines
//----------------------------------------------------------------------------
#define GOP_REG_DIRECT_BASE                            (0x20200)
#define GOP_REG(bk, reg)                    (GOP_REG_DIRECT_BASE+((MS_U32)(bk)<<8) + (reg) * 2)

#define GOP_OFFSET_WR                       8
#define GOP_VAL_WR                          GOP_REG_VAL(GOP_OFFSET_WR)
#define GOP_OFFSET_FWR                      9
#define GOP_VAL_FWR                         GOP_REG_VAL(GOP_OFFSET_FWR)


#define GOP_4G_OFST                           0x0
#define GOP_2G_OFST                           0x3
#define GOP_1G_OFST                           0x6
#define GOP_1GX_OFST                          0x9
#define GOP_DW_OFST                           0xC
#define GOP_1GS0_OFST                         0x15


#define GOP_4G_CTRL0                        GOP_REG(0, 0x00)
#define GOP_4G_CTRL1                        GOP_REG(0, 0x01)
#define GOP_4G_RATE                         GOP_REG(0, 0x02)
#define GOP_4G_RDMA_HT                      GOP_REG(GOP_4G_OFST, 0x0e)
#define GOP_4G_BW                           GOP_REG(GOP_4G_OFST, 0x19)
#define GOP_4G_SRAM_BORROW                  GOP_REG(GOP_4G_OFST, 0x1D)
#define GOP_4G_MIU_SEL                      GOP_REG(GOP_4G_OFST, 0x1F)
#define GOP_4G_STRCH_HSZ                    GOP_REG(GOP_4G_OFST, 0x30)
#define GOP_4G_STRCH_VSZ                    GOP_REG(GOP_4G_OFST, 0x31)
#define GOP_4G_STRCH_HSTR                   GOP_REG(GOP_4G_OFST, 0x32)
#define GOP_4G_STRCH_VSTR                   GOP_REG(GOP_4G_OFST, 0x34)
#define GOP_4G_HSTRCH                       GOP_REG(0, 0x35)
#define GOP_4G_VSTRCH                       GOP_REG(0, 0x36)
#define GOP_4G_HSTRCH_INI                   GOP_REG(GOP_4G_OFST, 0x38)
#define GOP_4G_VSTRCH_INI                   GOP_REG(GOP_4G_OFST, 0x39)
#define GOP_4G_OLDADDR                      GOP_REG(GOP_4G_OFST, 0x3b)
#define GOP_BAK_SEL                         GOP_REG(GOP_4G_OFST, 0x7f)
#define GOP_4G_TRSCLR_L                     GOP_REG(GOP_4G_OFST, 0x24)
#define GOP_4G_TRSCLR_H                     GOP_REG(GOP_4G_OFST, 0x25)
#define GOP_4G_MULTI_ALPHA                  GOP_REG(GOP_4G_OFST, 0x3c)

#define GOP_4G_GWIN0_CTRL(id)               GOP_REG(GOP_4G_OFST+1, 0x00 + (0x20*((id)%MAX_GOP0_GWIN)))
#define GOP_4G_DRAM_RBLK_L(id)              GOP_REG(GOP_4G_OFST+1, 0x01 + (0x20*((id)%MAX_GOP0_GWIN)))
#define GOP_4G_DRAM_RBLK_H(id)              GOP_REG(GOP_4G_OFST+1, 0x02 + (0x20*((id)%MAX_GOP0_GWIN)))
#define GOP_4G_HSTR(id)                     GOP_REG(GOP_4G_OFST+1, 0x04 + (0x20*((id)%MAX_GOP0_GWIN)))
#define GOP_4G_HEND(id)                     GOP_REG(GOP_4G_OFST+1, 0x05 + (0x20*((id)%MAX_GOP0_GWIN)))
#define GOP_4G_VSTR(id)                     GOP_REG(GOP_4G_OFST+1, 0x06 + (0x20*((id)%MAX_GOP0_GWIN)))
#define GOP_4G_VEND(id)                     GOP_REG(GOP_4G_OFST+1, 0x08 + (0x20*((id)%MAX_GOP0_GWIN)))
#define GOP_4G_DRAM_RBLK_HSIZE(id)          GOP_REG(GOP_4G_OFST+1, 0x09 + (0x20*((id)%MAX_GOP0_GWIN)))
#define GOP_4G_DRAM_RBLK_SIZE_L(id)         GOP_REG(GOP_4G_OFST+1, 0x10 + (0x20*((id)%MAX_GOP0_GWIN)))
#define GOP_4G_DRAM_RBLK_SIZE_H(id)         GOP_REG(GOP_4G_OFST+1, 0x11 + (0x20*((id)%MAX_GOP0_GWIN)))
#define GOP_4G_GWIN_ALPHA01(id)             GOP_REG(GOP_4G_OFST+1, 0x0A + (0x20*((id)%MAX_GOP0_GWIN)))
#define GOP_4G_DRAM_VSTR_L(id)              GOP_REG(GOP_4G_OFST+1, 0x0C + (0x20*((id)%MAX_GOP0_GWIN)))
#define GOP_4G_DRAM_VSTR_H(id)              GOP_REG(GOP_4G_OFST+1, 0x0D + (0x20*((id)%MAX_GOP0_GWIN)))
#define GOP_4G_DRAM_HSTR(id)                GOP_REG(GOP_4G_OFST+1, 0x0E + (0x20*((id)%MAX_GOP0_GWIN)))
#define GOP_4G_DRAM_HVSTOP_L(id)            GOP_REG(GOP_4G_OFST+1, 0x14 + (0x20*((id)%MAX_GOP0_GWIN)))
#define GOP_4G_DRAM_HVSTOP_H(id)            GOP_REG(GOP_4G_OFST+1, 0x15 + (0x20*((id)%MAX_GOP0_GWIN)))

#define GOP_2G_GWIN_ALPHA01(id)             GOP_REG(GOP_2G_OFST+1, 0x0A + (0x20*((id)%MAX_GOP1_GWIN)))
#define GOP_1G_GWIN_ALPHA01                 GOP_REG(GOP_1G_OFST+1, 0xA)
#define GOP_1GX_GWIN_ALPHA01                 GOP_REG(GOP_1GX_OFST+1, 0x0A)
#define GOP_1GS0_GWIN_ALPHA01                 GOP_REG(GOP_1GS0_OFST+1, 0x0A)

#define GOP_MUX4_MIX_VE                        GOP_REG(0, 0x7B)
//----------------------------------------------------------------------------
// Scaler Reg
//----------------------------------------------------------------------------
#define SC1_DIRREG_BASE                      (0x30000)
#define XC_REG(bk, reg)                        (SC1_DIRREG_BASE+((MS_U32)(bk)<<8) + (reg) * 2)

//Mux setting
#define GOP_MUX_SHIFT                          0x3
#define GOP_REGMUX_MASK                     BMASK((GOP_MUX_SHIFT-1):0)
#define GOP_MUX                                GOP_REG(0, 0x7e)
#define GOP_MUX_SC1                           GOP_REG(0, 0x7A)
#define GOP_4G_HS_PIPE                      GOP_REG(GOP_4G_OFST, 0x0f)
#define GOP_SC1_VOPNBL                      XC_REG(0x80+0x10, 0x23)
