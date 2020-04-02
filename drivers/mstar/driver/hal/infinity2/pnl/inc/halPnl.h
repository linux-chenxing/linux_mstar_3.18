////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2017 MStar Semiconductor, Inc.
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


/**
* @file     halPnl.h
* @version
* @Platform I2
* @brief    This file defines the HAL PNL interface
*
*/

#ifndef __HALPNL_H__
#define __HALPNL_H__

void HalLCDSWRest(void);
bool HalLCDQuerryPnlF(LCD_TYPE pnlType);
LCD_BUS_WIDTH  HalLCDGeBusWidth(void);
LCD_TYPE  HalLCDGeCurPanelType(void);
u16  HalLCDGePanelWidth(void);
u16  HalLCDGePanelHeight(void);

void HalLCDOpenClk(LCD_TOP_CLK_SEL eTOP_LCD_CLK_SEL);
void HalLCDSetWidth(u16 u16Width);
void HalLCDSetHeight(u16 u16Height);
void HalLCDSetPixelCnt(u32 u32Cnt);

void HalLCDSetBGRegion(u16 XOffset, u16 YOffset, u16 u16Width, u16 u16Height, u16 u16OutWidth);
void HalLCDEnableBG(bool bEnBG);
void HalLCDSetBGMode(bool bBGMode);
void HalLCDSetBGColor(u16 u16RBits, u16 u16GBits, u16 u16BBits);

void HalLCDShareBusSwitch(LCD_TYPE bPanelType);
void HalLCD_SetOutPanelType(LCD_TYPE ubDevType);

u32 HalLCDSetRGB_CLK(u32 Frequency);
void HalLCDSetRGBBus(LCD_DATABUS_CFG buscfg);
void HalLCD_RGB_EN(bool uEnable);
void HalLCDSetRGB_V_Timing(u16 u8V_B_PORCH, u16 u8V_SYNC_W, u16 u8V_BLK);
void HalLCDSetRGB_H_Timing(u16 u8V_B_PORCH, u16 u8V_SYNC_W, u16 u8V_BLK);
u64 HalLCDGetLpll_Idx(void);
void HalLCDSetRGB_V_Pol(bool bVPOL);
void HalLCDSetRGB_H_Pol(bool bHPOL);
void HalLCDSetRGB_DCLK_Pol(bool bDCLK_POL);
void HalLCD_DeltaRGB_EN(bool bEnable);
void HalLCD_DummyRGB_EN(bool bEnable);
void HalLCDSetRGB_LnOrder(LCD_PIX_ORDER EvenLnOrder, LCD_PIX_ORDER OddLnOrder);

void HalPLCD_Update(void);
void HalPLCD_Ctl(void);
void HalPLCD_SendCmd(void);
void HalPLCD_SendIndex(void);
void HalPLCD_AutoConfigIDXCmd(u32 u32Val, u8 u8TXNum, bool bIdxCmd);
void HalPLCD_SetMCUType(LCD_MCU_SYS McuSys);
void HalPLCD_SetCycleCnt(u16 RSLeadCs, u16 CSLeadRW, u16 RW_Cnt);
void HalPLCD_SetBUS(LCD_BUS_WIDTH busWidth,  LCD_PIX_ORDER Order);
void HalPLCD_SetPHA(bool bPHA);
void HalPLCD_SetPOR(bool bPOR);
void HalPLCD_Set_IDX_CMD_NUM(u8 u8Num);

u32 HalLCDRegisterRead(u32 u32Addr);
void HalLCDRegisterWrite(u32 u32Addr, u16  u16value);
void HalLCDRegisterWriteBit(u32 u32Addr, bool bitValue, u16  u16bitfield);
#endif /*__HALPNL_H__*/
