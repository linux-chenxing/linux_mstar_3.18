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
/// file    reg_ldm.h
/// @brief  local dimming Module Register Definition
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _REG_LDM_DMA_H_
#define _REG_LDM_DMA_H_

#define LDMA_MAX_SPI_CMD_NUM            (10)

//#define LD_DMA_PACKLENGTH               (32)

#define REG_RIU_BASE                    (0xFD000000)
#define REG_LDMA1_BASE                  (0x102D00*2)
#define REG_LDMA2_BASE                  (0x102D00*2+0x40*2)

#define REG_MENULOAD_CTRL_MODE          (0x00)
#define REG_MENULOAD_CTRL_MODE_MSK      (BIT0|BIT1)

#define REG_SPI_8BIT_MD                 (0x00)
#define REG_SPI_8BIT_MD_MSK             (BIT2)

#define REG_RGB_MODE                    (0x00)
#define REG_RGB_MODE_MSK                (BIT3)

#define REG_HYBRID_MODE                 (0x00)
#define REG_HYBRID_MODE_MSK             (BIT4)

#define REG_SPI_10BIT_MD                (0x00)
#define REG_SPI_10BIT_MD_MSK            (BIT5)

#define REG_SPI_16BIT_MD                (0x00)
#define REG_SPI_16BIT_MD_MSK            (BIT6)

#define REG_USE_HW_LOOP_MD              (0x00)
#define REG_USE_HW_LOOP_MD_MSK          (BIT7)

#define REG_MENULOAD_BUSY               (0x00)
#define REG_MENULOAD_BUSY_MSK           (BIT8)

#define REG_MENULOAD_DONE               (0x00)
#define REG_MENULOAD_DONE_MSK           (BIT9)

#define REG_MENULOAD_NUMBER             (0x01)
#define REG_MENULOAD_NUMBER_MSK         (0xFFF)

#define REG_MENULOAD_ABORTED_ONCE       (0x02)
#define REG_MENULOAD_ABORTED_ONCE_MSK   (BIT0)

#define REG_DMA2LD_BUSY                 (0x02)
#define REG_DMA2LD_BUSY_MSK             (BIT1)

#define REG_SYNC_TRIG0_SEL              (0x03)
#define REG_SYNC_TRIG0_SEL_MSK          (BIT0)
#define REG_SYNC_TRIG1_SEL              (0x03)
#define REG_SYNC_TRIG1_SEL_MSK          (BIT1)
#define REG_SYNC_TRIG2_SEL              (0x03)
#define REG_SYNC_TRIG2_SEL_MSK          (BIT2)

#define REG_MENULOAD_REALTIME_TRIG      (0x04)
#define REG_MENULOAD_ABORT              (0x05)
#define REG_MENULOAD_ENABLE             (0x06)
#define REG_DEST_BASE_ADDR              (0x8)
#define REG_SRC_BASE_ADDR               (0x9)

#define REG_START_CODE                  (0x0b)
#define REG_COMMAND                     (0x0c)
#define REG_CHKSUM_MD                   (0x0d)
#define REG_CHKSUM_MD_MSK               (BIT0|BIT1|BIT2)

#define REG_CMD_LENGTH                  (0x20)
#define REG_COMMAND1                    (0x21)
#define REG_COMMAND2                    (0x22)
#define REG_COMMAND3                    (0x23)
#define REG_COMMAND4                    (0x24)
#define REG_COMMAND5                    (0x25)
#define REG_COMMAND6                    (0x26)
#define REG_COMMAND7                    (0x27)
#define REG_COMMAND8                    (0x28)

#define REG_TRIG_DELAY0_CNT0            (0x2a)
#define REG_TRIG_DELAY0_CNT1            (0x2b)
#define REG_TRIG_DELAY1_CNT0            (0x2c)
#define REG_TRIG_DELAY1_CNT1            (0x2d)
#define REG_TRIG_DELAY_CTRL             (0x2e)

#endif // _REG_LDM_DMA_H_

