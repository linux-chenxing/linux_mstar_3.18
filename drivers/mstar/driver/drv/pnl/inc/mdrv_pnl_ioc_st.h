///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2017 MStar Semiconductor, Inc.
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
// @file   mdrv_pnl_io_st.h
// @brief  PNL KMD Driver Interface
// @author MStar Semiconductor Inc.
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_PNL_IOC_ST_H
#define _MDRV_PNL_IOC_ST_H

//=============================================================================
// enum
//=============================================================================
typedef enum _LCD_PIX_ORDER
{
    LCD_PIX_ORDER_RGB,
    LCD_PIX_ORDER_RBG,
    LCD_PIX_ORDER_GRB,
    LCD_PIX_ORDER_GBR,
    LCD_PIX_ORDER_BRG,
    LCD_PIX_ORDER_BGR
} LCD_PIX_ORDER;

typedef enum _LCD_TOP_CLK_SEL {
    LCD_TOP_CLK_LPLL,
	LCD_TOP_CLK_MPLL_108M,
	LCD_TOP_CLK_MPLL_144M,
	LCD_TOP_CLK_MPLL_160M,
	LCD_TOP_CLK_MPLL_216M,
	LCD_TOP_CLK_MPLL_240M
} LCD_TOP_CLK_SEL;

/* LCD Bus Width */
typedef enum _LCD_DATABUS_CFG
{
    LCD_RGB888,
    LCD_RGB666,
    LCD_RGB565,
    LCD_RGB666_LOOSE_PACKED_MSB_ALIGN,
    LCD_RGB666_LOOSE_PACKED_LSB_ALIGN,
    LCD_RGB332,
    LCD_BGR888,
    LCD_BGR666,
    LCD_BGR565,
    LCD_BGR666_LOOSE_PACKED_MSB_ALIGN,
    LCD_BGR666_LOOSE_PACKED_LSB_ALIGN,
    LCD_BGR332,

    LCD_RGB888_SWAP,    //R7...R0, G7...G0, B7...B0
    LCD_RGB666_SWAP,
    LCD_RGB565_SWAP,
    LCD_RGB666_LOOSE_PACKED_MSB_ALIGN_SWAP,
    LCD_RGB666_LOOSE_PACKED_LSB_ALIGN_SWAP,
    LCD_RGB332_SWAP,
    LCD_BGR888_SWAP,
    LCD_BGR666_SWAP,
    LCD_BGR565_SWAP,
    LCD_BGR666_LOOSE_PACKED_MSB_ALIGN_SWAP,
    LCD_BGR666_LOOSE_PACKED_LSB_ALIGN_SWAP,
    LCD_BGR332_SWAP,
} LCD_DATABUS_CFG;

// Porting from lcd_common.h
/* LCD Aspect Ratio  */
typedef enum _LCD_ASPECT_RATIO {
	LCD_RATIO_4_3 = 0,
	LCD_RATIO_16_9,
	LCD_RATIO_MAX
} LCD_ASPECT_RATIO;

/* LCD Rotate Direction  */
typedef enum _LCD_DIR {
	LCD_DIR_UPLEFT_DOWNRIGHT = 0,
	LCD_DIR_DOWNLEFT_UPRIGHT,
	LCD_DIR_UPRIGHT_DOWNLEFT,
	LCD_DIR_DOWNRIGHT_UPLEFT
} LCD_DIR;

/* LCD Interface Type */
typedef enum _LCD_TYPE {
	LCD_TYPE_PLCD,
	LCD_TYPE_SLCD,
	LCD_TYPE_PLCD_FLM,
	LCD_TYPE_RGBLCD,
	LCD_TYPE_RGB2LCD,
	LCD_TYPE_MIPIDSI,
	LCD_TYPE_MIPICSI,
	LCD_TYPE_MAX
} LCD_TYPE;

/* LCD Controller */
typedef enum _LCD_CONTROLER {
	LCD_PRM_CONTROLER,
	LCD_SCD_CONTROLER
} LCD_CONTROLER;

/* LCD Bus Width */
typedef enum _LCD_BUS_WIDTH {
	LCD_BUS_WIDTH_8,
	LCD_BUS_WIDTH_12,
	LCD_BUS_WIDTH_16,
	LCD_BUS_WIDTH_18,
	LCD_BUS_WIDTH_24
} LCD_BUS_WIDTH;

/* Idle Status */
typedef enum _LCD_POLARITY {
	LCD_POLARITY0,
	LCD_POLARITY1
} LCD_POLARITY;

/* Latch Data Edge */
typedef enum _LCD_PHASE {
	LCD_PHASE0,
	LCD_PHASE1
} LCD_PHASE;

/* MCU I/F system */
typedef enum _LCD_MCU_SYS {
	LCD_MCU_68SYS,
	LCD_MCU_80SYS
} LCD_MCU_SYS;

/* Output RGB Order */
typedef enum _LCD_RGB_ORDER {
	LCD_RGB_ORDER_RGB,
	LCD_RGB_ORDER_BGR
} LCD_RGB_ORDER;

/* Display Window */
typedef enum _LCD_DISP_WIN {
	LCD_DISP_WIN_MAIN = 0,
	LCD_DISP_WIN_PIP  = 1,
	LCD_DISP_WIN_OSD  = 3,
	LCD_DISP_WIN_OVLY = 4
} LCD_DISP_WIN;

/* Signal Polarity */
typedef enum _LCD_SIG_POLARITY {
	LCD_SIG_POLARITY_H,
	LCD_SIG_POLARITY_L
} LCD_SIG_POLARITY;
#if 0
/* Signal Polarity */
typedef enum _LCD_SPI_PIX_ORDER {
	LCD_SPI_PIX_ORDER_RGB,
	LCD_SPI_PIX_ORDER_RBG,
	LCD_SPI_PIX_ORDER_GRB,
	LCD_SPI_PIX_ORDER_GBR,
	LCD_SPI_PIX_ORDER_BRG,
	LCD_SPI_PIX_ORDER_BGR
} LCD_SPI_PIX_ORDER;

/* Window color format */
typedef enum _LCD_WIN_FMT {
	LCD_WIN_FMT_4BPP,
	LCD_WIN_FMT_8BPP,
	LCD_WIN_FMT_16BPP,
	LCD_WIN_FMT_24BPP,
	LCD_WIN_FMT_YUV420,
	LCD_WIN_FMT_YUV422,
	LCD_WIN_FMT_32BPP
} LCD_WIN_FMT;
#endif
//=============================================================================
// struct
//=============================================================================

// Porting from drv_pnl.h
typedef struct
{
    u16 u16HBPorch;
    u16 u16HBlanking;

    u16 u16VBPorch;
    u16 u16VBlanking;

    u16 u16HSyncW;
    u16 u16VSyncW;
    u16 u16VFreqx10;

    LCD_SIG_POLARITY	ubDclkPor;
    LCD_SIG_POLARITY	ubHsyncPor;
    LCD_SIG_POLARITY	ubVsyncPor;

    bool			bDeltaRBG;
    bool			bDummyRBG;
    LCD_PIX_ORDER  OddOrder;
    LCD_PIX_ORDER  EvenOrder;
} ST_RGB_LCD_ATTR;

typedef struct
{
	LCD_MCU_SYS MCUSYS;
	LCD_BUS_WIDTH busWidth;
	LCD_SIG_POLARITY	ubDataPor;
	LCD_SIG_POLARITY	ubDataPHA;
	u8			  u8RSLeadCs;
	u8			  u8CSLeadRW;
	u8			  u8RW_Cnt;
} ST_PLCD_ATTR;

typedef struct
{
    u16 u16PanelWidth;
    u16 u16PanelHeight;
    LCD_TYPE ubDevType;
    LCD_RGB_ORDER Order;
    LCD_DATABUS_CFG BusCfg;
    LCD_BUS_WIDTH bBusWidth;
    bool bBitReverse;
}ST_PNL_ATTR;

typedef struct
{
    ST_PNL_ATTR tPnlAttr;
    union
    {
        ST_PLCD_ATTR    tPLcdAttr;
        ST_RGB_LCD_ATTR tRgbLcdAttr;
    };
} PnlConfig_t, *pPnlConfig;

typedef struct
{
    u32 nRegAddr;
    u32 nRegVal;
    u32 nRegMask;
} PnlReg_t, *pPnlReg;

//=============================================================================

//=============================================================================



#endif //_MDRV_PNL_IOC_ST_H



