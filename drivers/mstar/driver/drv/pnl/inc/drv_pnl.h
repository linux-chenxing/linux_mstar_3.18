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
* @file    drv_pnl.h
* @version
* @brief   LCD panel support functions
*
*/

#ifndef __DRV_PNL_H__
#define __DRV_PNL_H__

/*=============================================================*/
// Include files
/*=============================================================*/


/*=============================================================*/
// Extern definition
/*=============================================================*/

/*=============================================================*/
// Macro definition
/*=============================================================*/

/*=============================================================*/
// Data type definition
/*=============================================================*/
typedef struct
{
       char TypeDescript[20];
    	LCD_TYPE type;
       bool bSupport;
}ST_PNL_TYPE_LIST;


/*=============================================================*/
// Variable definition
/*=============================================================*/



/*=============================================================*/
// Global function definition
/*=============================================================*/
void  DrvLCDSWRest(void);
ST_PNL_TYPE_LIST DrvLCDQuerryPnlF(LCD_TYPE pnlType);
LCD_BUS_WIDTH DrvLCDGeBusWidth(void);
u16 DrvLCDGePanelWidth(void);
u16 DrvLCDGePanelHeight(void);

void DrvLCDSetWidth(u16 u16Width);
void DrvLCDSetHeight(u16 u16Height);
void DrvLCDSetPixlCnt(u32 u32Cnt);
void DrvLCDSetBGRegion(u16 XOffset, u16 YOffset, u16 u16Width, u16 u16Height, u16 u16OutWidth);
void  DrvLCDEnableBG(bool bEnBG);

void DrvLCDSetRGB_CLK(u32 Frequency);
void  DrvLCD_RGB_EN(bool uEnable);
void DrvLCDSetRGB_H_Timing(u16 u16H_BK_PORCH, u16 u16HSYNC_W, u16 u16H_BLK);
void DrvLCDSetRGB_V_Timing(u16 u16V_BK_PORCH, u16 u16VSYNC_W, u16 u16V_BLK);
void DrvLCDSetRGB_V_Pol(bool bVPOL);
void DrvLCDSetRGB_H_Pol(bool bHPOL);
void DrvLCDSetRGB_DCLK_Pol(bool bDCLK_POL);

void DrvLCDDeltaRGB_EN(bool bEnable);
void DrvLCDDummyRGB_EN(bool bEnable);
void DrvLCDSetRGB_LnOrder(LCD_PIX_ORDER EvenLnOrder, LCD_PIX_ORDER OddLnOrder);
void DrvPLCD_Update(void);
void DrvPLCD_Ctl(void);
void DrvPLCD_SendCmd(void);
void DrvPLCD_SendIndex(void);

void DrvPLCD_ConfigIDXCmd(u32 u32Val, u8 u8TXNum, bool bIdxCmd);
void DrvPLCD_SetCycleCnt(u8 RSLeadCs, u8 CSLeadRW, u8 RW_Cnt);

void DrvPLCD_SetMCUType(LCD_MCU_SYS McuSys);
void DrvPLCD_SetBUS( LCD_BUS_WIDTH busWidth, LCD_PIX_ORDER Order);
void DrvPLCD_SetPHA( bool bPHA);
void DrvPLCD_SetPOR( bool bPOR);
void DrvPLCD_Set_IDX_CMD_NUM( u8 u8Num);

void DrvLCDInitHeightWidth(u16 u16Width, u16 u16Height);
void DrvLCDInit (ST_PNL_ATTR panlAttr);
void DrvLCDShareBusSwitch(ST_PNL_ATTR panlAttr);
void DrvLCDSetBus(ST_PNL_ATTR panlAttr);
void DrvLCDConfigRGB_Paras (ST_RGB_LCD_ATTR Config);
void DrvLCDConfigPLCD (ST_PLCD_ATTR Config);
void DrvLCDSetPadConfig(void);
void DrvLCDSetOutPanelType(LCD_TYPE ubDevType);
void DrvLCDClrScreen(void);
void DrvLCDUpdateScreen(void);
void DrvLCDClkSet(u32 ClkSel);
void DrvLCDBusSwitch(ST_PNL_ATTR panlAttr);

u32     DrvLCDRegisterRead(u32 u32Addr);
void    DrvLCDRegisterWrite(u32 u32Addr, u16 u16Val);
void    DrvLCDRegisterWriteBit(u32 u32Addr, bool bitValue, u16  u16bitfield);
#endif //__DRV_PNL_H__

