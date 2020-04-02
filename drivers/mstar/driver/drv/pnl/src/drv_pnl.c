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
* @file    drv_pnl.c
* @version
*
*/

#define __DRV_PNL_C__


/*=============================================================*/
// Include files
/*=============================================================*/

#include <linux/types.h>
#include <linux/module.h>
#include "cam_os_wrapper.h"
#include "mdrv_pnl_ioc.h"
#include "mdrv_pnl_ioc_st.h"
#include "halPnl.h"
#include "drv_pnl.h"
//#include "sys_sys_isw_uart.h"

/*=============================================================*/
// Macro definition
/*=============================================================*/

#define DRV_PNL_DEBUG 1
#if DRV_PNL_DEBUG
#define DRVPNLDBG(fmt, arg...) CamOsPrintf(fmt, ##arg)

#else
#define DRVPNLDBG(fmt, arg...)
#endif
/*=============================================================*/
// Data type definition
/*=============================================================*/

/*=============================================================*/
// Variable definition
/*=============================================================*/


/*=============================================================*/
// Local function definition
/*=============================================================*/
static ST_PNL_TYPE_LIST gstSupportPnlIF[LCD_TYPE_MAX]=
{
    {"MCU PNL", LCD_TYPE_PLCD, 0},
    {"SERIAL PNL", LCD_TYPE_SLCD, 0},
    {"MCU FLM PNL", LCD_TYPE_PLCD_FLM, 0},
    {"TTL/RGB PNL", LCD_TYPE_RGBLCD, 0},
    {"MIPI DSI PNL", LCD_TYPE_MIPIDSI, 0},
    {"MIPI CSI PNL", LCD_TYPE_MIPICSI, 0},
};
/*=============================================================*/
// Global function definition
/*=============================================================*/

void  DrvLCDSWRest(void)
{
    HalLCDSWRest();
}

ST_PNL_TYPE_LIST DrvLCDQuerryPnlF(LCD_TYPE pnlType)
 {
       gstSupportPnlIF[pnlType].bSupport=HalLCDQuerryPnlF(gstSupportPnlIF[pnlType].type);
       DRVPNLDBG("%s %s\n", gstSupportPnlIF[pnlType].TypeDescript,  gstSupportPnlIF[pnlType].bSupport?"Support":"Not support");
       return gstSupportPnlIF[pnlType];
 }

LCD_BUS_WIDTH DrvLCDGeBusWidth(void)
{
    return HalLCDGeBusWidth();
}
u16 DrvLCDGePanelWidth(void)
 {
	return HalLCDGePanelWidth();
 }
u16 DrvLCDGePanelHeight(void)
 {
	return HalLCDGePanelHeight();
 }

LCD_TYPE DrvLCDGeCurPanelType(void)
 {
	return HalLCDGeCurPanelType();
 }

void DrvLCDSetWidth(u16 u16Width)
 {
	HalLCDSetWidth(u16Width);
 }

void DrvLCDSetHeight(u16 u16Height)
 {
	HalLCDSetHeight(u16Height);
 }

void DrvLCDSetPixlCnt(u32 u32Cnt)
 {
	HalLCDSetPixelCnt(u32Cnt);
 }

void DrvLCDSetBGRegion(u16 XOffset, u16 YOffset, u16 u16Width, u16 u16Height, u16 u16OutWidth)
 {
	HalLCDSetBGRegion(XOffset, YOffset, u16Width, u16Height, u16OutWidth);
       HalLCDEnableBG(1);
 }

void  DrvLCDEnableBG(bool bEnBG)
 {
       HalLCDEnableBG(bEnBG);
 }

void  DrvLCDSetBGMode(bool bBGMode)
 {
       HalLCDSetBGMode(bBGMode);
 }

void  DrvLCDSetBGColor(u16 u16RBits, u16 u16GBits, u16 u16BBits)
 {
       HalLCDSetBGColor(u16RBits, u16GBits, u16BBits);
 }

void DrvLCDOpenClk( LCD_TOP_CLK_SEL eCLK_SEL)
{
	HalLCDOpenClk(eCLK_SEL);
}

//-------------------------------------------------------------------------------------------------
//  RGB panel  Setting
//-------------------------------------------------------------------------------------------------
void DrvLCDSetRGB_CLK(u32 Frequency)
{
    HalLCDSetRGB_CLK(Frequency);
}

void  DrvLCD_RGB_EN(bool uEnable)
{
	HalLCD_RGB_EN(uEnable);
}

void DrvLCDSetRGB_H_Timing(u16 u16H_BK_PORCH, u16 u16HSYNC_W, u16 u16H_BLK)
{
	HalLCDSetRGB_H_Timing(u16H_BK_PORCH, u16HSYNC_W, u16H_BLK);
}
void DrvLCDSetRGB_V_Timing(u16 u16V_BK_PORCH, u16 u16VSYNC_W, u16 u16V_BLK)
{
	HalLCDSetRGB_V_Timing(u16V_BK_PORCH, u16VSYNC_W, u16V_BLK);
}

void DrvLCDSetRGB_V_Pol(bool bVPOL)
{
	HalLCDSetRGB_V_Pol(bVPOL);
}

void DrvLCDSetRGB_H_Pol(bool bHPOL)
{
    HalLCDSetRGB_H_Pol(bHPOL);
}
void DrvLCDSetRGB_DCLK_Pol(bool bDCLK_POL)
{
	HalLCDSetRGB_DCLK_Pol(bDCLK_POL);
}


void DrvLCDDeltaRGB_EN(bool bEnable)
{
	HalLCD_DeltaRGB_EN(bEnable);
}

void DrvLCDDummyRGB_EN(bool bEnable)
{
	HalLCD_DummyRGB_EN(bEnable);
}

void DrvLCDSetRGB_LnOrder(LCD_PIX_ORDER EvenLnOrder, LCD_PIX_ORDER OddLnOrder)
{
	HalLCDSetRGB_LnOrder(EvenLnOrder,OddLnOrder);
}


//-------------------------------------------------------------------------------------------------
//  Parallel panel  Setting
//-------------------------------------------------------------------------------------------------
void DrvPLCD_Update(void)
{
	HalPLCD_Update();
}

void DrvPLCD_Ctl(void)
{
	HalPLCD_Ctl();
}

void DrvPLCD_SendCmd(void)
{
	HalPLCD_SendCmd();
}

void DrvPLCD_SendIndex(void)
{
	HalPLCD_SendIndex();
}

void DrvPLCD_ConfigIDXCmd(u32 u32Val, u8 u8TXNum, bool bIdxCmd)
{
	HalPLCD_AutoConfigIDXCmd(u32Val, u8TXNum, bIdxCmd);
}
void DrvPLCD_SetCycleCnt(u8 RSLeadCs, u8 CSLeadRW, u8 RW_Cnt)
{
	HalPLCD_SetCycleCnt(RSLeadCs,CSLeadRW,RW_Cnt);
}
void DrvPLCD_SetMCUType(LCD_MCU_SYS McuSys)
{
	HalPLCD_SetMCUType(McuSys);
}

void DrvPLCD_SetBUS( LCD_BUS_WIDTH busWidth, LCD_PIX_ORDER Order)
{
	HalPLCD_SetBUS(busWidth,Order);
}

void DrvPLCD_SetPHA( bool bPHA)
{
	HalPLCD_SetPHA(bPHA);
}

void DrvPLCD_SetPOR( bool bPOR)
{
	HalPLCD_SetPOR(bPOR);
}

void DrvPLCD_Set_IDX_CMD_NUM( u8 u8Num)
{
	HalPLCD_Set_IDX_CMD_NUM(u8Num);
}


//-------------------------------------------------------------------------------------------------
//  public panel Setting
//-------------------------------------------------------------------------------------------------
void DrvLCDInitHeightWidth(u16 u16Width, u16 u16Height)
{
 	DrvLCDSetWidth(u16Width);
	DrvLCDSetHeight(u16Height);
	DrvLCDSetPixlCnt(u16Width*u16Height);
}

void DrvLCDInit ( ST_PNL_ATTR panlAttr)
{
	DrvLCDSetPadConfig();

	HalLCD_RGB_EN(0);

    switch (panlAttr.ubDevType)
    {
        case LCD_TYPE_RGBLCD:
            DrvLCDOpenClk(LCD_TOP_CLK_MPLL_144M);
            break;
        case LCD_TYPE_MIPIDSI:
            DrvLCDOpenClk(LCD_TOP_CLK_MPLL_216M);
            break;
        default:
            break;
    }

	DrvLCDShareBusSwitch(panlAttr);
	DrvLCDInitHeightWidth(panlAttr.u16PanelWidth, panlAttr.u16PanelHeight);
 	DrvLCDSetOutPanelType(panlAttr.ubDevType);
	DrvLCDSetBus(panlAttr);
}

void DrvLCDShareBusSwitch(ST_PNL_ATTR panlAttr)
{
	HalLCDShareBusSwitch(panlAttr.ubDevType);
}

void DrvLCDBusSwitch(ST_PNL_ATTR panlAttr)
{
	HalLCDShareBusSwitch(panlAttr.ubDevType);
}

void DrvLCDSetBus(ST_PNL_ATTR panlAttr)
{
	if(panlAttr.ubDevType == LCD_TYPE_RGBLCD) {
		HalLCDSetRGBBus(panlAttr.BusCfg);
	}
	else if((panlAttr.ubDevType == LCD_TYPE_PLCD) || (panlAttr.ubDevType == LCD_TYPE_PLCD_FLM)) {
		HalPLCD_SetBUS(panlAttr.bBusWidth, panlAttr.Order);
	}
}
void DrvLCDConfigRGB_Paras (ST_RGB_LCD_ATTR Config)
{
        HalLCDSetRGB_H_Pol (Config.ubHsyncPor);
        HalLCDSetRGB_V_Pol(Config.ubVsyncPor);
        HalLCDSetRGB_DCLK_Pol(Config.ubDclkPor);

        HalLCDSetRGB_V_Timing(  Config.u16VBPorch,
						         Config.u16VSyncW,
						         Config.u16VBlanking);
        HalLCDSetRGB_H_Timing(  Config.u16HBPorch,
                                                    Config.u16HSyncW,
							   Config.u16HBlanking);
	//Serial RGB
	if( Config.bDeltaRBG) {
		DrvLCDDeltaRGB_EN( Config.bDeltaRBG);
		DrvLCDSetRGB_LnOrder(Config.OddOrder,Config.EvenOrder);
	}

	if(Config.bDummyRBG) {
		DrvLCDDummyRGB_EN(Config.bDummyRBG);
	}
}

void DrvLCDConfigPLCD (ST_PLCD_ATTR Config)
{
	DrvPLCD_Ctl();
	DrvPLCD_SetMCUType(Config.MCUSYS);
	DrvPLCD_SetCycleCnt(Config.u8RSLeadCs, Config.u8CSLeadRW, Config.u8RW_Cnt);

	DrvPLCD_SetPHA(Config.ubDataPHA);
	DrvPLCD_SetPOR(Config.ubDataPor);
}
void DrvLCDSetPadConfig(void)
{
 	//TBD
}
void DrvLCDSetOutPanelType(LCD_TYPE ubDevType)
{
     HalLCD_SetOutPanelType(ubDevType);
}

void DrvLCDClrScreen(void)
{
	//TBD
}

void DrvLCDUpdateScreen(void)
{
	DrvPLCD_Update();
}
void DrvLCDClkSet(u32 ClkSel)
{
      //TBD
}

/*---------------------------
    Test function
    ---------------------------*/
u32 DrvLCDRegisterRead(u32 u32Addr)
{
     return HalLCDRegisterRead(u32Addr);
}

void DrvLCDRegisterWrite(u32 u32Addr, u16 u16Val)
{
      HalLCDRegisterWrite(u32Addr, u16Val);
}

void  DrvLCDRegisterWriteBit(u32 u32Addr, bool bitValue, u16  u16bitfield)
{
     HalLCDRegisterWriteBit(u32Addr, bitValue, u16bitfield);
}

/*---------------------------
    MIPI TX DSI bug: must trigger TTL to keep MIPI TX DSI working
    ---------------------------*/
EXPORT_SYMBOL(DrvLCDSWRest);
EXPORT_SYMBOL(DrvLCDOpenClk);
EXPORT_SYMBOL(DrvLCDSetRGB_CLK);
EXPORT_SYMBOL(DrvLCDSetOutPanelType);

