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


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------

#include <linux/types.h>
#include "cam_os_wrapper.h"
#include "mdrv_pnl_ioc_st.h"
#include "regPnl.h"
#include "halPnl.h"
#include "hal_Pnl_util.h"


//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------
#define HAL_PNL_DEBUG 1
#if HAL_PNL_DEBUG
#define HALPNLDBG(fmt, arg...) CamOsPrintf(fmt, ##arg)

#else
#define HALPNLDBG(fmt, arg...)
#endif
//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Type and Structure Declaration
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Global setting
//-------------------------------------------------------------------------------------------------

void HalLCDSWRest(void)
{
    WBYTEMSK(PNL_TRANS_CTL, 1 << PNL_LCD_SW_RST, PNL_LCD_SW_RST);
}

bool HalLCDQuerryPnlF(LCD_TYPE pnlType)
{
    if(pnlType != LCD_TYPE_SLCD)
        return 1;
    else
        return 0;
}
LCD_BUS_WIDTH  HalLCDGeBusWidth(void)
{
    LCD_BUS_WIDTH buswidth = RGB_D24BIT_RGB888;
    u8    u8RegVal;
    u8RegVal = R2BYTEMSK(PNL_RGB_DTYPE, RGB_DTYPE_MASK);
    switch(u8RegVal)
    {
        case RGB_D24BIT_RGB332:
        case RGB_D24BIT_BGR332:
            buswidth = LCD_BUS_WIDTH_8;
            break;
        case RGB_D24BIT_BGR666_LSB_PACKED:
        case RGB_D24BIT_BGR666_MSB_PACKED:
        case RGB_D24BIT_BGR666:
        case RGB_D24BIT_RGB666_LSB_PACKED:
        case RGB_D24BIT_RGB666_MSB_PACKED:
        case RGB_D24BIT_RGB666:
            buswidth = LCD_BUS_WIDTH_18;
            break;
        case RGB_D24BIT_BGR565:
        case RGB_D24BIT_RGB565:
            buswidth = LCD_BUS_WIDTH_16;
            break;
        case RGB_D24BIT_RGB888:
        case RGB_D24BIT_BGR888:
            buswidth = LCD_BUS_WIDTH_24;
            break;
    }

    HALPNLDBG("Pnl bus width=%d", buswidth);
    return buswidth;
}

LCD_TYPE HalLCDGeCurPanelType(void)
{
    HALPNLDBG("%d  \n",R2BYTEMSK(DEC_MISC_OUT_MODE, DEC_MISC_LB_OUT));
    if(R2BYTEMSK(DEC_MISC_OUT_MODE, DEC_MISC_LB_OUT) == DEC_MISC_LCD)
    {
        if(R2BYTEMSK(DEC_MISC_OUT_MODE, PNL_DSPY1_MASK) == PNL_DSPY1_I80_SEL)
        {
            HALPNLDBG("Current interface : I80\n");
            return LCD_TYPE_PLCD;
        }
        else if (R2BYTEMSK(DEC_MISC_OUT_MODE, PNL_DSPY1_MASK) == PNL_DSPY1_RGB_SEL)
        {
            HALPNLDBG("Current interface : RGB LCD\n");
            return LCD_TYPE_RGBLCD;
        }
    }
    else if(R2BYTEMSK(DEC_MISC_OUT_MODE, DEC_MISC_LB_OUT) == DEC_MISC_MIPI_DSI)
    {
        HALPNLDBG("Current interface : MIPI DSI\n");
        return LCD_TYPE_MIPIDSI;
    }
    else if(R2BYTEMSK(DEC_MISC_OUT_MODE, DEC_MISC_LB_OUT) == DEC_MISC_MIPI_CSI)
    {
        return LCD_TYPE_MIPICSI;
    }
    return LCD_TYPE_MAX;
}
u16  HalLCDGePanelWidth(void)
{
    return R2BYTE(PNL_LCD_WIDTH);
}

u16  HalLCDGePanelHeight(void)
{
    return R2BYTE(PNL_LCD_HEIGHT);
}

void HalLCDSetWidth(u16 u16Width)
{
    W2BYTE(PNL_LCD_WIDTH, u16Width);
}

void HalLCDSetHeight(u16 u16Height)
{
    W2BYTE(PNL_LCD_HEIGHT, u16Height);
}

void HalLCDSetPixelCnt(u32 u32Cnt)
{
    W2BYTE(PNL_RGB_PIXL_CNT_H, (u32Cnt >> 16) & 0xFF);
    W2BYTE(PNL_RGB_PIXL_CNT_L, u32Cnt & 0xFF);
}

void HalLCDShareBusSwitch(LCD_TYPE bPanelType)
{
    if(LCD_TYPE_RGBLCD == bPanelType)
    {
        //BUS only for RGB  LCD
        W2BYTE(PNL_RGB_SHARE_P_LCD_BUS,  PNL_RGB_LCD_ONLY);
    }
    else if((LCD_TYPE_PLCD == bPanelType) || (LCD_TYPE_PLCD_FLM == bPanelType))
    {
        //BUS only fo Parallel  LCD
        W2BYTE(PNL_RGB_SHARE_P_LCD_BUS, PNL_P_LCD_ONLY);
        WriteRegBit(PNL_RGB_CTL_4, 1,  LCD_OUT_SEL_LCD1);
        //WriteRegBit(PNL_RGB_CTL_4, LCD_OUT_RGB,  LCD_OUT_SEL_LCD1);
    }
}

static void  _HalLCD_DecMisc_Type(LCD_TYPE ubDevType)
{
    if((ubDevType == LCD_TYPE_PLCD) ||
       (ubDevType == LCD_TYPE_SLCD) ||
       (ubDevType == LCD_TYPE_PLCD_FLM) ||
       (ubDevType == LCD_TYPE_RGBLCD))
    {
        W2BYTE(DEC_MISC_OUT_MODE, DEC_MISC_LCD);
        W2BYTE(DEC_MISC_MVOP_VSYNC_OFF, 0x0);
        W2BYTE( (REG_CHIPTOP_BASE+REG_CHIPTOP_4A), 0x0);
        W2BYTE( (REG_CHIPTOP_BASE+REG_CHIPTOP_4B), 0x0);
        W2BYTE( (REG_CHIPTOP_BASE+REG_CHIPTOP_4C), 0x0);
        W2BYTE( (REG_CHIPTOP_BASE+REG_CHIPTOP_4D), 0x0);
    }
    else if ( (ubDevType == LCD_TYPE_MIPIDSI))
    {
        W2BYTE(DEC_MISC_MVOP_VSYNC_OFF, 0x0);  //MVOP vsync trig line offset set to 0
        WriteRegBit(DEC_MISC_RGB565_EC0, 1 , DEC_MISC_MIPI_DSI_EC0_SEL); //ECO: DSI vsync will reset RGB vsync after RGB VSYNC (must be set )
        W2BYTE(DEC_MISC_OUT_MODE, DEC_MISC_MIPI_DSI);
    }
    else if ( (ubDevType == LCD_TYPE_MIPICSI))
    {
        W2BYTE(DEC_MISC_OUT_MODE, DEC_MISC_MIPI_CSI);
    }
}

void  HalLCD_SetOutPanelType(LCD_TYPE ubDevType)
{
    if((ubDevType == LCD_TYPE_PLCD) ||
            (ubDevType == LCD_TYPE_SLCD) ||
            (ubDevType == LCD_TYPE_PLCD_FLM) ||
            (ubDevType == LCD_TYPE_RGBLCD))
    {
        WriteRegBit(PNL_DSPY_USED, 1, PNL_DSPY1_SEL);
        if((ubDevType == LCD_TYPE_PLCD) ||
                (ubDevType == LCD_TYPE_PLCD_FLM))
        {
            W2BYTEMSK(PNL_DSPY_USED, PNL_DSPY1_I80_SEL << 0, PNL_DSPY1_MASK);
        }
        if(ubDevType == LCD_TYPE_RGBLCD)
        {
            W2BYTEMSK(PNL_DSPY_USED, PNL_DSPY1_RGB_SEL << 0, PNL_DSPY1_MASK);
        }
    }
    else if((ubDevType == LCD_TYPE_MIPIDSI))
    {
        //W2BYTE(DEC_MISC_MVOP_VSYNC_OFF, 0x0);
        WriteRegBit(PNL_DSPY_USED, 1, PNL_DSPY1_SEL);
        W2BYTEMSK(PNL_DSPY_USED, PNL_DSPY1_RGB_SEL<<0, PNL_DSPY1_MASK);
        //WriteRegBit(DEC_MISC_RGB565_EC0, 1 , DEC_MISC_MIPI_DSI_EC0_SEL);
        //W2BYTE(DEC_MISC_OUT_MODE, DEC_MISC_MIPI_DSI);
        //W2BYTE(CHIPTOP_LCDPAD_MUX, 0); //disable LCD pad
    }
    _HalLCD_DecMisc_Type(ubDevType);
}

void  HalLCDSetBGRegion(u16 XOffset, u16 YOffset, u16 u16Width, u16 u16Height, u16 u16OutWidth)
{
    W2BYTE(DEC_MISC_BG_OUT_X_ST, (XOffset));
    W2BYTE(DEC_MISC_BG_OUT_Y_ST, (YOffset));
    W2BYTE(DEC_MISC_BLK_BG_IN_WIDTH, (u16Width - 1));
    W2BYTE(DEC_MISC_BLK_BG_IN_HEIGHT, (u16Height - 1));
    W2BYTE(DEC_MISC_BLK_BG_OUT_WIDTH, (u16OutWidth - 1));

}

void  HalLCDEnableBG(bool bEnBG)
{
    if(bEnBG == 1)
        W2BYTE(DEC_MISC_BG_FUNC,  0);
    else
        W2BYTE(DEC_MISC_BG_FUNC,  BG_FUNC_BYPASS);
}

void  HalLCDSetBGMode(bool bBGMode)
{
    if(bBGMode == 1){
        W2BYTE(DEC_MISC_BG_FUNC,  0);
        W2BYTEMSK(DEC_MISC_BG_FUNC,  1, BG_FORCE_COL);
    }
    else
        W2BYTEMSK(DEC_MISC_BG_FUNC,  0, BG_FORCE_COL);
}

void  HalLCDSetBGColor(u16 u16RBits, u16 u16GBits, u16 u16BBits)
{
        W2BYTE(DEC_MISC_BG_OUT_COL_R, u16RBits<<2);
        W2BYTE(DEC_MISC_BG_OUT_COL_G, u16GBits<<2);
        W2BYTE(DEC_MISC_BG_OUT_COL_B, u16BBits<<2);
}
//-------------------------------------------------------------------------------------------------
//  RGB panel related Setting
//-------------------------------------------------------------------------------------------------

/*Enable control*/
void  HalLCD_RGB_EN(bool uEnable)
{
    if(uEnable)
    {
        WriteRegBit(PNL_RGB_CTL, uEnable, PNL_RGB_IF_EN);
    }
    else
    {
        WriteRegBit(PNL_RGB_CTL, uEnable, PNL_RGB_IF_EN);
        while(ReadRegBit(PNL_RGB_CTL, PNL_RGB_IF_EN));
    }
}
/*Timing control*/
void HalLCDSetRGB_V_Timing(u16 u8V_BK_PORCH, u16  u8VSYNC_W, u16 u8V_BLK)
{
    W2BYTE(PNL_RGB_V_BPORCH, u8V_BK_PORCH);
    W2BYTEMSK(PNL_RGB_SYNC_WIDTH, u8VSYNC_W << 8, PNL_RGB_VSYNC_W_MASK);
    W2BYTE(PNL_RGB_V_BLANK, u8V_BLK);
    W2BYTE(DEC_MISC_MVOP_TRIG_OFF, (u8V_BLK-u8V_BK_PORCH));
}
void  HalLCDSetRGB_H_Timing(u16 u8H_BK_PORCH, u16 u8HSYNC_W, u16 u8H_BLK)
{
    W2BYTE(PNL_RGB_H_BPORCH, u8H_BK_PORCH);
    W2BYTEMSK(PNL_RGB_SYNC_WIDTH, u8HSYNC_W << 0, PNL_RGB_HSYNC_W_MASK);
    W2BYTE(PNL_RGB_H_BLANK, u8H_BLK);

}
u64 HalLCDGetLpll_Idx(void)
{

    u64 u64Dclk = 0;
    //get dotclock TODO
    return u64Dclk;
}

void HalLCDOpenClk(LCD_TOP_CLK_SEL eTOP_LCD_CLK_SEL)
{
    //WriteRegBit(CLKG_ODCLK_CTRL0, 0, );
    switch(eTOP_LCD_CLK_SEL){
    case LCD_TOP_CLK_MPLL_216M:
        W2BYTE(CLKG_ODCLK_CTRL0, CKG_ODCLK_MPLL_216M );
        break;
    case LCD_TOP_CLK_MPLL_160M:
        W2BYTE(CLKG_ODCLK_CTRL0, CKG_ODCLK_MPLL_160M );
        break;
     case LCD_TOP_CLK_MPLL_144M:
        W2BYTE(CLKG_ODCLK_CTRL0, CKG_ODCLK_MPLL_144M );
        break;
     case LCD_TOP_CLK_MPLL_108M:
        W2BYTE(CLKG_ODCLK_CTRL0, CKG_ODCLK_MPLL_108M );
        break;
     case LCD_TOP_CLK_MPLL_240M:
        W2BYTE(CLKG_ODCLK_CTRL0, CKG_ODCLK_MPLL_240M );
        break;
    case LCD_TOP_CLK_LPLL:
        W2BYTE(CLKG_ODCLK_CTRL0, CKG_ODCLK_LPLL_CLK );
        break;
    }
}

u32 HalLCDGetClk(void)
{
    //u16 u16ClkSrc=R2BYTE(CLKG_ODCLK_CTRL0);
    return 0;
}

void  HalLCDSetRGBBus(LCD_DATABUS_CFG buscfg)
{
    LCD_BUS_WIDTH buswidth;
    u8  u8BusType;
    bool bBitReverse;
    switch(buscfg)
    {
        case LCD_RGB888:
        case LCD_RGB888_SWAP:
        case LCD_BGR888:
        case LCD_BGR888_SWAP:
            buswidth = LCD_BUS_WIDTH_24;
            break;
        case LCD_RGB666:
        case LCD_BGR666:
        case LCD_RGB666_SWAP:
        case LCD_BGR666_SWAP:
        case LCD_RGB666_LOOSE_PACKED_MSB_ALIGN:
        case LCD_RGB666_LOOSE_PACKED_LSB_ALIGN:
        case LCD_BGR666_LOOSE_PACKED_MSB_ALIGN:
        case LCD_BGR666_LOOSE_PACKED_LSB_ALIGN:
        case LCD_RGB666_LOOSE_PACKED_MSB_ALIGN_SWAP:
        case LCD_RGB666_LOOSE_PACKED_LSB_ALIGN_SWAP:
        case LCD_BGR666_LOOSE_PACKED_MSB_ALIGN_SWAP:
        case LCD_BGR666_LOOSE_PACKED_LSB_ALIGN_SWAP:
            buswidth = LCD_BUS_WIDTH_18;
            break;
        case LCD_RGB565:
        case LCD_BGR565:
        case LCD_RGB565_SWAP:
        case LCD_BGR565_SWAP:
            buswidth = LCD_BUS_WIDTH_16;
            break;
        case LCD_RGB332:
        case LCD_BGR332:
        case LCD_RGB332_SWAP:
        case LCD_BGR332_SWAP:
        default:
            buswidth = LCD_BUS_WIDTH_8;
            break;
    }

    W2BYTE(CHIPTOP_LCDPAD_MUX, 0);

    switch(buswidth)
    {
        case LCD_BUS_WIDTH_8:
            W2BYTE(CHIPTOP_LCDPAD_MUX, RGB8_BITMODE);
            break;
        case LCD_BUS_WIDTH_16:
        case LCD_BUS_WIDTH_12:
            W2BYTE(CHIPTOP_LCDPAD_MUX, RGB16_BITMODE);
            break;
        case LCD_BUS_WIDTH_18:
            W2BYTE(CHIPTOP_LCDPAD_MUX, RGB18_BITMODE);
            break;
        case LCD_BUS_WIDTH_24:
            W2BYTE(CHIPTOP_LCDPAD_MUX, RGB24_BITMODE);
            break;
    }
    bBitReverse = 0;
    switch(buscfg)
    {
        case LCD_RGB888:
            u8BusType = RGB_D24BIT_RGB888;
            break;
        case LCD_RGB888_SWAP:
            u8BusType = RGB_D24BIT_RGB888;
            bBitReverse = 1;
            break;
        case LCD_BGR888:
            u8BusType = RGB_D24BIT_BGR888;
            break;
        case LCD_BGR888_SWAP:
            u8BusType = RGB_D24BIT_BGR888;
            bBitReverse = 1;
            break;
        case LCD_RGB666:
            u8BusType = RGB_D24BIT_RGB666;
            break;
        case LCD_BGR666:
            u8BusType = RGB_D24BIT_BGR666;
            break;
        case LCD_RGB666_SWAP:
            u8BusType = RGB_D24BIT_RGB666;
            bBitReverse = 1;
            break;
        case LCD_BGR666_SWAP:
            u8BusType = RGB_D24BIT_BGR666;
            bBitReverse = 1;
            break;
        case LCD_RGB666_LOOSE_PACKED_MSB_ALIGN:
            u8BusType = RGB_D24BIT_RGB666_MSB_PACKED;
            break;
        case LCD_RGB666_LOOSE_PACKED_LSB_ALIGN:
            u8BusType = RGB_D24BIT_RGB666_LSB_PACKED;
            break;
        case LCD_BGR666_LOOSE_PACKED_MSB_ALIGN:
            u8BusType = RGB_D24BIT_BGR666_MSB_PACKED;
            break;
        case LCD_BGR666_LOOSE_PACKED_LSB_ALIGN:
            u8BusType = RGB_D24BIT_BGR666_LSB_PACKED;
            break;
        case LCD_RGB666_LOOSE_PACKED_MSB_ALIGN_SWAP:
            u8BusType = RGB_D24BIT_RGB666_MSB_PACKED;
            bBitReverse = 1;
            break;
        case LCD_RGB666_LOOSE_PACKED_LSB_ALIGN_SWAP:
            u8BusType = RGB_D24BIT_RGB666_LSB_PACKED;
            bBitReverse = 1;
            break;
        case LCD_BGR666_LOOSE_PACKED_MSB_ALIGN_SWAP:
            u8BusType = RGB_D24BIT_BGR666_MSB_PACKED;
            bBitReverse = 1;
            break;
        case LCD_BGR666_LOOSE_PACKED_LSB_ALIGN_SWAP:
            u8BusType = RGB_D24BIT_BGR666_LSB_PACKED;
            bBitReverse = 1;
            break;
        case LCD_RGB565:
            u8BusType = RGB_D24BIT_RGB565;
            break;
        case LCD_BGR565:
            u8BusType = RGB_D24BIT_BGR565;
            W2BYTEMSK(DEC_MISC_RGB565_EC0, 1, DEC_MISC_RGB565_EC0_SEL); //ECO setting
            break;
        case LCD_RGB565_SWAP:
            u8BusType = RGB_D24BIT_RGB565;
            bBitReverse = 1;
            break;
        case LCD_BGR565_SWAP:
            u8BusType = RGB_D24BIT_BGR565;
            bBitReverse = 1;
            break;
        case LCD_RGB332:
            u8BusType = RGB_D24BIT_RGB332;
            break;
        case LCD_BGR332:
            u8BusType = RGB_D24BIT_BGR332;
            break;
        case LCD_RGB332_SWAP:
            u8BusType = RGB_D24BIT_RGB332;
            bBitReverse = 1;
            break;
        case LCD_BGR332_SWAP:
            u8BusType = RGB_D24BIT_BGR332;
            bBitReverse = 1;
            break;
    }
    W2BYTEMSK(PNL_RGB_DTYPE, u8BusType, RGB_DTYPE_MASK);
    WriteRegBit(PNL_RGB_DTYPE, bBitReverse, RGB_DTYPE_SWAP);
#if 0
    if(LCD_PIX_ORDER_BGR == Order)
    {
        switch(buscfg)
        {
            case LCD_BUS_WIDTH_8:
                W2BYTEMSK(PNL_RGB_DTYPE, RGB_D24BIT_BGR332, RGB_DTYPE_MASK);
                break;
            case LCD_BUS_WIDTH_12:
                W2BYTEMSK(PNL_RGB_DTYPE, RGB_D24BIT_BGR444, RGB_DTYPE_MASK);
                break;
            case LCD_BUS_WIDTH_16:
                W2BYTEMSK(PNL_RGB_DTYPE, RGB_D24BIT_BGR565, RGB_DTYPE_MASK);
                break;
            case LCD_BUS_WIDTH_18:
                W2BYTEMSK(PNL_RGB_DTYPE, RGB_D24BIT_BGR666, RGB_DTYPE_MASK);
                break;
            case LCD_BUS_WIDTH_24:
                W2BYTEMSK(PNL_RGB_DTYPE, RGB_D24BIT_BGR888, RGB_DTYPE_MASK);
                break;
        }
    }
    else if(LCD_PIX_ORDER_RGB == Order)
    {
        switch(buscfg)
        {
            case LCD_BUS_WIDTH_8:
                W2BYTEMSK(PNL_RGB_DTYPE, RGB_D24BIT_RGB332, RGB_DTYPE_MASK);
                break;
            case LCD_BUS_WIDTH_12:
                W2BYTEMSK(PNL_RGB_DTYPE, RGB_D24BIT_RGB444, RGB_DTYPE_MASK);
                break;
            case LCD_BUS_WIDTH_16:
                W2BYTEMSK(PNL_RGB_DTYPE, RGB_D24BIT_RGB565, RGB_DTYPE_MASK);
                break;
            case LCD_BUS_WIDTH_18:
                W2BYTEMSK(PNL_RGB_DTYPE, RGB_D24BIT_RGB666, RGB_DTYPE_MASK);
                break;
            case LCD_BUS_WIDTH_24:
                W2BYTEMSK(PNL_RGB_DTYPE, RGB_D24BIT_RGB888, RGB_DTYPE_MASK);
                break;
        }
    }
#endif

}

u32 HalLCDSetRGB_CLK(u32 Frequency)
{

    u32 u32ODclk = 216000000; //FPGA 37.5MHz
    u8 u8ClkDiv = 0;

    if(Frequency != 0)
    {
        u8ClkDiv = u32ODclk / Frequency;
        HALPNLDBG("ODCLK = %d,  CLKDIV = %d \n", u32ODclk, u8ClkDiv);
    }
    W2BYTEMSK(PNL_RGB_PRT_PHS, (u8ClkDiv-1) << 8, PNL_RGB_DOT_CLK_RATIO);
    return u32ODclk;
}
void HalLCDSetRGB_V_Pol(bool bVPOL)
{
    //bVPOL=0, active low, bVPOL=1, active High
    WriteRegBit(PNL_RGB_CTL, bVPOL, PNL_VSYNC_POLAR);
}

void HalLCDSetRGB_H_Pol(bool bHPOL)
{
    //bHPOL=0, active low, bHPOL=1, active High
    WriteRegBit(PNL_RGB_CTL, bHPOL, PNL_HSYNC_POLAR);
}

void HalLCDSetRGB_DCLK_Pol(bool bDCLK_POL)
{
    //DOT clock polarity. This bit specifies the positive edge or negative edge of DOT clock at the central of DOT data.
    //0: positive edge , 1: negative edge
    WriteRegBit(PNL_RGB_CTL, bDCLK_POL, PNL_DOT_POLAR);
}

/*Output type control*/
void HalLCD_DeltaRGB_EN(bool bEnable)
{
    WriteRegBit(PNL_RGB_DELTA_MODE, bEnable, RGB_DELTA_MODE_ENABLE);
}

void HalLCD_DummyRGB_EN(bool bEnable)
{
    WriteRegBit(PNL_RGB_DELTA_MODE, bEnable, RGB_DUMMY_MODE_ENABLE);
}
void HalLCDSetRGB_LnOrder(LCD_PIX_ORDER EvenLnOrder, LCD_PIX_ORDER OddLnOrder)
{
    switch(EvenLnOrder)
    {
        case LCD_PIX_ORDER_RGB:
            W2BYTEMSK(PNL_RGB_DELTA_MODE, RGB_EVEN_LINE_RGB << 5, RGB_EVEN_LINE_MSK);
            break;
        case LCD_PIX_ORDER_RBG:
            W2BYTEMSK(PNL_RGB_DELTA_MODE, RGB_EVEN_LINE_RBG << 5, RGB_EVEN_LINE_MSK);
            break;
        case LCD_PIX_ORDER_GRB:
            W2BYTEMSK(PNL_RGB_DELTA_MODE, RGB_EVEN_LINE_GRB << 5, RGB_EVEN_LINE_MSK);
            break;
        case LCD_PIX_ORDER_GBR:
            W2BYTEMSK(PNL_RGB_DELTA_MODE, RGB_EVEN_LINE_GBR << 5, RGB_EVEN_LINE_MSK);
            break;
        case LCD_PIX_ORDER_BRG:
            W2BYTEMSK(PNL_RGB_DELTA_MODE, RGB_EVEN_LINE_BRG << 5, RGB_EVEN_LINE_MSK);
            break;
        case LCD_PIX_ORDER_BGR:
            W2BYTEMSK(PNL_RGB_DELTA_MODE, RGB_EVEN_LINE_BGR << 5, RGB_EVEN_LINE_MSK);
            break;
    }

    switch(OddLnOrder)
    {
        case LCD_PIX_ORDER_RGB:
            W2BYTEMSK(PNL_RGB_DELTA_MODE, RGB_ODD_LINE_RGB << 2, RGB_ODD_LINE_MSK);
            break;
        case LCD_PIX_ORDER_RBG:
            W2BYTEMSK(PNL_RGB_DELTA_MODE, RGB_ODD_LINE_RBG << 2, RGB_ODD_LINE_MSK);
            break;
        case LCD_PIX_ORDER_GRB:
            W2BYTEMSK(PNL_RGB_DELTA_MODE, RGB_ODD_LINE_GRB << 2, RGB_ODD_LINE_MSK);
            break;
        case LCD_PIX_ORDER_GBR:
            W2BYTEMSK(PNL_RGB_DELTA_MODE, RGB_ODD_LINE_GBR << 2, RGB_ODD_LINE_MSK);
            break;
        case LCD_PIX_ORDER_BRG:
            W2BYTEMSK(PNL_RGB_DELTA_MODE, RGB_ODD_LINE_BRG << 2, RGB_ODD_LINE_MSK);
            break;
        case LCD_PIX_ORDER_BGR:
            W2BYTEMSK(PNL_RGB_DELTA_MODE, RGB_ODD_LINE_BGR << 2, RGB_ODD_LINE_MSK);
            break;
    }
}

//-------------------------------------------------------------------------------------------------
//  Parallel panel related Setting
//-------------------------------------------------------------------------------------------------
void HalPLCD_Update(void)
{
    if(!(ReadRegBit(PNL_LCD_IRQ_FIN_ST, 0x20)))
    {
        WriteRegBit(PNL_TRANS_CTL, 1, PNL_LCD_TRANS_START);
    }
}

void HalPLCD_Ctl(void)
{
    WriteRegBit(PNL_TRANS_CTL, 1, PNL_FRAME_TX_SETADDR_EN);
}


void HalPLCD_SendCmd(void)
{
    u32 u32TimeOutTick = 100;
    WriteRegBit(PNL_TRANS_CTL, 1, PNL_CMD_RDY);
    while(ReadRegBit(PNL_TRANS_CTL, PNL_CMD_RDY) && (--u32TimeOutTick > 0)); //TBD
}


void HalPLCD_SendIndex(void)
{
    u32 u32TimeOutTick = 100;
    WriteRegBit(PNL_TRANS_CTL, 1, PNL_IDX_RDY);
    while(ReadRegBit(PNL_TRANS_CTL, PNL_IDX_RDY) && (--u32TimeOutTick > 0)); //TBD
}


void HalPLCD_AutoConfigIDXCmd(u32 u32Val, u8 u8TXNum, bool bIdxCmd)
{
    switch(u8TXNum)
    {
        case 1:
            W2BYTE(PNL_PL_LCD_TX_1_L, ((u32Val) >> 16 & 0xFF));
            W2BYTE(PNL_PL_LCD_TX_1_H, u32Val);

            break;
        case 2:
            W2BYTE(PNL_PL_LCD_TX_2_L, ((u32Val) >> 16 & 0xFF));
            W2BYTE(PNL_PL_LCD_TX_2_H, u32Val);
            break;
        case 3:
            W2BYTE(PNL_PL_LCD_TX_3_L, ((u32Val) >> 16 & 0xFF));
            W2BYTE(PNL_PL_LCD_TX_3_H, u32Val);
            break;
        case 4:
            W2BYTE(PNL_PL_LCD_TX_4_L, ((u32Val) >> 16 & 0xFF));
            W2BYTE(PNL_PL_LCD_TX_4_H, u32Val);
            break;
        case 5:
            W2BYTE(PNL_PL_LCD_TX_5_L, ((u32Val) >> 16 & 0xFF));
            W2BYTE(PNL_PL_LCD_TX_5_H, u32Val);
            break;
        case 6:
            W2BYTE(PNL_PL_LCD_TX_6_L, ((u32Val) >> 16 & 0xFF));
            W2BYTE(PNL_PL_LCD_TX_6_H, u32Val);
            break;
        case 7:
            W2BYTE(PNL_PL_LCD_TX_7_L, ((u32Val) >> 16 & 0xFF));
            W2BYTE(PNL_PL_LCD_TX_7_H, u32Val);
            break;
    }
    WriteRegBit(PNL_PL_LCD_AUTO_CFG, bIdxCmd ? (AUTO_TX_TYPE_IDX) : (AUTO_TX_TYPE_CMD),
                bIdxCmd ? (LCD_TX_TYPE_IDX((u8TXNum - 1))) : (LCD_TX_TYPE_CMD((u8TXNum - 1))));
}



void HalPLCD_SetMCUType(LCD_MCU_SYS McuSys)
{
    switch(McuSys)
    {
        case LCD_MCU_68SYS:
            WriteRegBit(PNL_PL_LCD_CTL, 0, PLCD_TYPE_80_68_SEL);
            break;
        case LCD_MCU_80SYS:
            WriteRegBit(PNL_PL_LCD_CTL, 1, PLCD_TYPE_80_68_SEL);
            break;
    }
}

void HalPLCD_SetCycleCnt(u16 RSLeadCs, u16 CSLeadRW, u16 RW_Cnt)
{
    W2BYTE(PNL_P_LCD_RS_LEAD_CS_CYC, RSLeadCs);
    W2BYTE(PNL_P_LCD_CS_LEAD_RW_CYC, CSLeadRW);
    W2BYTE(PNL_P_LCD_RW_CYC, RW_Cnt);
}


void HalPLCD_SetBUS(LCD_BUS_WIDTH busWidth,  LCD_PIX_ORDER Order)
{
    switch(busWidth)
    {
        case LCD_BUS_WIDTH_8:
            W2BYTE(CHIPTOP_LCDPAD_MUX, PL8_BITMODE);
            break;
        case LCD_BUS_WIDTH_16:
            W2BYTE(CHIPTOP_LCDPAD_MUX, PL16_BITMODE);
            break;
        case LCD_BUS_WIDTH_18:
            W2BYTE(CHIPTOP_LCDPAD_MUX, PL18_BITMODE);
            break;
        case LCD_BUS_WIDTH_12:
        case LCD_BUS_WIDTH_24:
        default:
            HALPNLDBG("Not support Pnl bus width(%d)", busWidth);
            break;
    }

    if(LCD_PIX_ORDER_RGB == Order)
    {
        WriteRegBit(PNL_RGB_CTL_4, LCD_OUT_RGB,  LCD_OUT_SEL_LCD1);
    }
    else if(LCD_PIX_ORDER_BGR == Order)
    {
        WriteRegBit(PNL_RGB_CTL_4, LCD_OUT_BGR,  LCD_OUT_SEL_LCD1);
    }

    switch(busWidth)
    {
        case LCD_BUS_WIDTH_8:
            W2BYTEMSK(PNL_PL_LCD_CTL, PLCD_BUS_8BPP << 0, PLCD_BUS_MASK);
            break;
        case LCD_BUS_WIDTH_12:
            W2BYTEMSK(PNL_PL_LCD_CTL, PLCD_BUS_12BPP << 0, PLCD_BUS_MASK);
            break;
        case LCD_BUS_WIDTH_16:
            W2BYTEMSK(PNL_PL_LCD_CTL, PLCD_BUS_16BPP << 0, PLCD_BUS_MASK);
            break;
        case LCD_BUS_WIDTH_18:
            W2BYTEMSK(PNL_PL_LCD_CTL, PLCD_BUS_18BPP << 0, PLCD_BUS_MASK);
            break;
        case LCD_BUS_WIDTH_24:
            break;
    }
}

void HalPLCD_SetPHA(bool bPHA)
{
    WriteRegBit(PNL_PL_LCD_CTL, bPHA, PLCD_PHA);
}

void HalPLCD_SetPOR(bool bPOR)
{
    WriteRegBit(PNL_PL_LCD_CTL, bPOR, PLCD_POR);
}

void HalPLCD_Set_IDX_CMD_NUM(u8 u8Num)
{
    W2BYTE(PNL_P_LCD_IDX_CMD_NUM, u8Num);
}


// Test Function
u32 HalLCDRegisterRead(u32 u32Addr)
{
    return R2BYTE(u32Addr);
}

void HalLCDRegisterWrite(u32 u32Addr, u16  u16value)
{
    W2BYTE(u32Addr, u16value);
}

void HalLCDRegisterWriteBit(u32 u32Addr, bool bitValue, u16  u16bitfield)
{
    WriteRegBit(u32Addr, bitValue, u16bitfield);
}