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
/// file    reg_mspi.h
/// @brief  local dimming Module Register Definition
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _REG_MSPI_H_
#define _REG_MSPI_H_
//-------------------------------------------------------------------------------------------------
//  Hardware Capability
//-------------------------------------------------------------------------------------------------

#define REG_MSPI1_BK                0x153A00*2
#define REG_MSPI2_BK                0x153B00*2
#define BK_CLK0                        0x100B00*2
#if defined(CONFIG_ARM64)
extern ptrdiff_t mstar_pm_base;
#define REG_MSPI1_BASE           (mstar_pm_base + REG_MSPI1_BK)
#define REG_MSPI2_BASE           (mstar_pm_base + REG_MSPI2_BK)
#else
#define REG_RIU_BASE                0xFD000000

#define REG_MSPI1_BASE           (REG_RIU_BASE + REG_MSPI1_BK)
#define REG_MSPI2_BASE           (REG_RIU_BASE + REG_MSPI2_BK)
#endif

#define MSPI_WRITE_BUF_OFFSET          0x40
#define MSPI_READ_BUF_OFFSET           0x44
#define MSPI_WBF_SIZE_OFFSET           0x48
#define MSPI_RBF_SIZE_OFFSET           0x48

// read/ write buffer size
#define MSPI_RWSIZE_MASK               0xFF
#define MSPI_RSIZE_BIT_OFFSET          0x8
#define MAX_READ_BUF_SIZE              0x8
#define MAX_WRITE_BUF_SIZE             0x8

// CLK config 
#define MSPI_CTRL_OFFSET               0x49
#define MSPI_CLK_CLOCK_OFFSET          0x49
#define MSPI_CLK_CLOCK_BIT_OFFSET      0x08
#define MSPI_CLK_CLOCK_MASK            0xFF
#define MSPI_CLK_PHASE_MASK            0x40
#define MSPI_CLK_PHASE_BIT_OFFSET      0x06
#define MSPI_CLK_POLARITY_MASK         0x80
#define MSPI_CLK_POLARITY_BIT_OFFSET   0x07
#define MSPI_CLK_PHASE_MAX             0x1
#define MSPI_CLK_POLARITY_MAX          0x1
#define MSPI_CLK_CLOCK_MAX             0x7

// DC config 
#define MSPI_DC_MASK                   0xFF
#define MSPI_DC_BIT_OFFSET             0x08
#define MSPI_DC_TR_START_OFFSET        0x4A
#define MSPI_DC_TRSTART_MAX            0xFF
#define MSPI_DC_TR_END_OFFSET          0x4A
#define MSPI_DC_TREND_MAX              0xFF   
#define MSPI_DC_TB_OFFSET              0x4B
#define MSPI_DC_TB_MAX                 0xFF
#define MSPI_DC_TRW_OFFSET             0x4B
#define MSPI_DC_TRW_MAX                0xFF

// Frame Config 
#define MSPI_FRAME_WBIT_OFFSET         0x4C
#define MSPI_FRAME_RBIT_OFFSET         0x4E
#define MSPI_FRAME_BIT_MAX             0x07
#define MSPI_FRAME_BIT_MASK            0x07
#define MSPI_FRAME_BIT_FIELD           0x03
#define MSPI_LSB_FIRST_OFFSET          0x50
#define MSPI_TRIGGER_OFFSET            0x5A
#define MSPI_DONE_OFFSET               0x5B
#define MSPI_DONE_CLEAR_OFFSET         0x5C
#define MSPI_CHIP_SELECT_OFFSET        0x5F

#define MSPI_FULL_DEPLUX_RD00 (0x78)
#define MSPI_FULL_DEPLUX_RD01 (0x78)
#define MSPI_FULL_DEPLUX_RD02 (0x79
#define MSPI_FULL_DEPLUX_RD03 (0x79)
#define MSPI_FULL_DEPLUX_RD04 (0x7a)
#define MSPI_FULL_DEPLUX_RD05 (0x7a)
#define MSPI_FULL_DEPLUX_RD06 (0x7b)
#define MSPI_FULL_DEPLUX_RD07 (0x7b)

#define MSPI_FULL_DEPLUX_RD08 (0x7c)
#define MSPI_FULL_DEPLUX_RD09 (0x7c)
#define MSPI_FULL_DEPLUX_RD10 (0x7d)
#define MSPI_FULL_DEPLUX_RD11 (0x7d)
#define MSPI_FULL_DEPLUX_RD12 (0x7e)
#define MSPI_FULL_DEPLUX_RD13 (0x7e)
#define MSPI_FULL_DEPLUX_RD14 (0x7f)
#define MSPI_FULL_DEPLUX_RD15 (0x7f)


//chip select bit map
#define MSPI_CHIP_SELECT_MAX           0x07  
  
// control bit
#define MSPI_DONE_FLAG                 0x01
#define MSPI_TRIGGER                   0x01
#define MSPI_CLEAR_DONE                0x01
#define MSPI_INT_ENABLE                0x04
#define MSPI_RESET                     0x02
#define MSPI_ENABLE                    0x01

// CLKGEN0
#define  MSPI_CLK_CFG                  0x17
#define  MSPI_CLK_DIV                  0x13
#define  MSPI_CLK_DIV_MASK             0x0F
#define  MSPI_CLK_CFG_OFFSET           0x02
#define  MSPI_CLK_MASK                 0x1F
#define  MSPI_CLK_DEFAULT              0x20
#define  MSPI_MAXCLKLEVEL              0x07


//CLKGEN1 for mspi1
    // clk_spi_m_p1
#define  MSPI1_CLK_DIV                  0x21
#define  MSPI1_CLK_DIV_MASK             0x0F

#define  MSPI1_CLK_CFG                  0x21
#define  MSPI1_CLK_CFG_OFFSET           0x02
#define  MSPI1_CLK_MASK                 0x1F00
#define  MSPI1_CLK_DEFAULT              0x20
#define  MSPI1_MAXCLKLEVEL			    0x07



#endif // _REG_MSPI_H_
