////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2007 MStar Semiconductor, Inc.
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

#ifndef _REG_GPIO_H_
#define _REG_GPIO_H_

//-------------------------------------------------------------------------------------------------
//  Hardware Capability
//-------------------------------------------------------------------------------------------------
#define GPIO_UNIT_NUM               198

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define REG_MIPS_BASE               0xFD000000//Use 8 bit addressing

#define REG_ALL_PAD_IN      (0xA1)      //  (0x101ea0)   //set all pads (except SPI) as input
#define REG_LVDS_BASE               (0x103200)
#define REG_LVDS_BANK               REG_LVDS_BASE

////8 bit define
#define REG_FUART_MODE   0x06
#define REG_UART0_MODE   0x06
#define REG_UART1_MODE   0x07
#define REG_SR_MODE      0x0C
#define REG_SR_I2C_MODE  0x0C
#define REG_PWM0_MODE    0x0E
#define REG_PWM1_MODE    0x0E
#define REG_PWM2_MODE    0x0E
#define REG_PWM3_MODE    0x0E
#define REG_NAND_MODE    0x10
#define REG_SD_MODE      0x10
#define REG_SDIO_MODE    0x11
#define REG_I2C0_MODE    0x12
#define REG_I2C1_MODE    0x12
#define REG_SPI0_MODE    0x18
#define REG_SPI1_MODE    0x18
#define REG_EJ_MODE      0x1E
#define REG_ETH_MODE     0x1E
#define REG_CCIR_MODE    0x1E
#define REG_TTL_MODE     0x1E
#define REG_I2S_MODE     0x1F
#define REG_DMIC_MODE    0x1F

#define REG_IRIN_MODE    0x38

#define REG_SAR_MODE    0x22

#define REG_TEST_IN_MODE     0x24
#define REG_TEST_OUT_MODE    0x24

#define GPIO_OEN    0   //set o to nake output
#define GPIO_ODN    1

#define IN_HIGH    1   //input high
#define IN_LOW     0   //input low

#define OUT_HIGH    1   //output high
#define OUT_LOW     0   //output low


//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

#endif // _REG_GPIO_H_

