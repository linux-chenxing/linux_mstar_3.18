////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2008 MStar Semiconductor, Inc.
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
#include <linux/kernel.h>
#include "cam_os_wrapper.h"
#include "mdrv_mipi_dsi_io_st.h"
#include "reg_mipi_dsi.h"
#include "hal_mipi_dsi.h"
#include "hal_mipi_dsi_util.h"

//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define ALIGN_TO(x, n)  \
	(((x) + ((n) - 1)) & ~((n) - 1))

#define AS_UINT32(x)    (*(volatile u32 *)((void*)x))

#define MPLL_2_LPLL_FRACT   (432UL*(1<<19)*10)
//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
static LCM_PARAMS glMIPIDSI_Params={0};

//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------
#define HAL_MIPI_DSI_DEBUG 1
#define HAL_MIPI_DSI_DEBUG_LEVEL 0
#if HAL_MIPI_DSI_DEBUG
#define HAL_MIPI_DSI_MSG(format, args...) do{if (1) CamOsPrintf(format, ##args);}while(0)
#define HAL_MIPI_DSI_DBG(Level, format, args...) do{if (Level > HAL_MIPI_DSI_DEBUG_LEVEL) CamOsPrintf(format, ##args);}while(0)
#else
#define HAL_MIPI_DSI_MSG(format, args...)
#define HAL_MIPI_DSI_DBG(Level, format, args...)
#endif
//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
static bool _HalIsEngineBusy(void)
{

    #if 0
	DSI_INT_STATUS_REG status;

	status = DSI_REG->DSI_INTSTA;

	if (status.BUSY)
		return TRUE;
	return FALSE;
    #else
        return 0;
    #endif
}

static DSI_STATUS _Hal_DSI_Reset(void)
{
    #if 0
	//DSI_REG->DSI_COM_CTRL.DSI_RESET = 1;
	OUTREGBIT(DSI_COM_CTRL_REG,DSI_REG->DSI_COM_CTRL,DSI_RESET,1);
//	lcm_mdelay(5);
	//DSI_REG->DSI_COM_CTRL.DSI_RESET = 0;
	OUTREGBIT(DSI_COM_CTRL_REG,DSI_REG->DSI_COM_CTRL,DSI_RESET,0);
#else
    #if 0
        WriteLongRegBit(DSI_REG_BASE +(REG_DSI_COM_CON>>1), 1, DSI_DSI_RESET);
        WriteLongRegBit(DSI_REG_BASE +(REG_DSI_COM_CON>>1), 0, DSI_DSI_RESET);
    #else
        W4BYTE(DSI_REG_BASE +(REG_DSI_COM_CON>>1), DSI_DSI_RESET | DSI_DPHY_RESET);
        W4BYTE(DSI_REG_BASE +(REG_DSI_COM_CON>>1), 0);
    #endif
#endif
    return DSI_STATUS_OK;
}

//-------------------------------------------------------------------------------------------------
// Type and Structure Declaration
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Test Functions
//-------------------------------------------------------------------------------------------------
u32 Hal_MIPI_DSI_RegisterRead(u32 u32Addr)
{
   return R4BYTE(u32Addr);
}

void Hal_MIPI_DSI_RegisterWriteBit(u32 u32Addr, bool bitValue, u8  u8bitpos)
{
    u32 u32Msk=1<<u8bitpos;

    HAL_MIPI_DSI_DBG(1, "[B] u32Addr = %d,  bit = %d,  bEn = %d", R4BYTE(u32Addr), u8bitpos, bitValue);
    WriteLongRegBit(u32Addr, bitValue, u32Msk);
    HAL_MIPI_DSI_DBG(1, "[A] u32Addr = %d,  bit = %d,  bEn = %d", R4BYTE(u32Addr), u8bitpos, bitValue);
}

void Hal_MIPI_DSI_RegisterWrite(u32 u32Addr, u32  u32value)
{
    //HAL_MIPI_DSI_DBG(1, "[B] u32Addr = %d", R4BYTE(u32Addr));
    W4BYTE(u32Addr, u32value);
    //HAL_MIPI_DSI_DBG(1, "[A] u32Addr = %d", R4BYTE(u32Addr));
}

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
#if 0
DSI_STATUS Hal_DSI_EnableInterrupt(DISP_INTERRUPT_EVENTS eventID)
{

    switch(eventID)
    {
        case DISP_DSI_READ_RDY_INT:
            //DSI_REG->DSI_INTEN.RD_RDY = 1;
            OUTREGBIT(DSI_INT_ENABLE_REG,DSI_REG->DSI_INTEN,RD_RDY,1);
            break;
        case DISP_DSI_CMD_DONE_INT:
            //DSI_REG->DSI_INTEN.CMD_DONE = 1;
            OUTREGBIT(DSI_INT_ENABLE_REG,DSI_REG->DSI_INTEN,CMD_DONE,1);
            break;
        case DISP_DSI_VMDONE_INT:
            OUTREGBIT(DSI_INT_ENABLE_REG,DSI_REG->DSI_INTEN,VM_DONE,1);
            break;
        case DISP_DSI_VSYNC_INT:
            disp_register_irq(DISP_MODULE_RDMA0, _DSI_RDMA0_IRQ_Handler);
            break;
        case DISP_DSI_TARGET_LINE_INT:
            disp_register_irq(DISP_MODULE_RDMA0, _DSI_RDMA0_IRQ_Handler);
            break;
        case DISP_DSI_REG_UPDATE_INT:
            //wake_up_interruptible(&_dsi_reg_update_wq);
            disp_register_irq(DISP_MODULE_MUTEX, _DSI_MUTEX_IRQ_Handler);
            break;
        default:
            return DSI_STATUS_ERROR;
    }

    return DSI_STATUS_OK;

}

 #endif


void  Hal_DSI_PowerOn(void)
{
    //TBD
}
void  Hal_DSI_PowerOff(void)
{
  //TBD
}


void  Hal_DSI_EnableClk(void)
{
    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_START>>1), 0, DSI_DSI_START); //clear first
    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_START>>1), 1, DSI_DSI_START);

}
void  Hal_DSI_DisableClk(void)
{
    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_START>>1), 0, DSI_DSI_START); //

}


DSI_STATUS Hal_DSI_DHY_Init(void)
{
    W2BYTE(DPHY_DSI_REG00, 0x0);   //local sw reset, active low
    W2BYTE(DPHY_DSI_REG01, 0x0);  // clear power down hs mode, power down whole dphy analog
    //TBD: Not need to setup cken?
    W2BYTE(DPHY_DSI_REG04, 0x0);   //disable ch0 sw setting
    W2BYTE(DPHY_DSI_REG08, 0x0);   //disable ch1 sw setting
    W2BYTE(DPHY_DSI_REG0A, 0x0180);   //disable ch2 sw setting
    W2BYTE(DPHY_DSI_REG0E, 0x80);  //dummy register 0
    W2BYTE(DPHY_DSI_REG11, 0x00);  //disable ch3 sw setting
    W2BYTE(DPHY_DSI_REG14, 0x00);  //disable ch4 sw setting
    W2BYTE(DPHY_DSI_REG17, 0xC000); //
    W2BYTE(DPHY_DSI_REG18, 0x80);  //LP bist pattern enable
    W2BYTE(DPHY_DSI_REG22, 0x00);  //clk csi dsi phy
    W2BYTE(DPHY_DSI_REG28, 0x001F);
    W2BYTE(DPHY_DSI_REG29, 0x6760);

    return DSI_STATUS_OK;
}

void Hal_DSI_ClkSrcSel(E_DSI_TOP_CLK_SEL eTOP_DSI_CLK_SEL)
{
    //WriteRegBit(CLKG_ODCLK_CTRL0, 0, );
    switch(eTOP_DSI_CLK_SEL){
    case DSI_TOP_CLK_MPLL_216M:
        W2BYTE(CLKG_TX_DSI_CLK_CTRL0, CKG_TX_DSI_CLK_MPLL_216M );
        break;
    case DSI_TOP_CLK_MPLL_160M:
        W2BYTE(CLKG_TX_DSI_CLK_CTRL0, CKG_TX_DSI_CLK_MPLL_160M );
        break;
     case DSI_TOP_CLK_MPLL_144M:
        W2BYTE(CLKG_TX_DSI_CLK_CTRL0, CKG_TX_DSI_CLK_MPLL_144M );
        break;
     case DSI_TOP_CLK_MPLL_108M:
        W2BYTE(CLKG_TX_DSI_CLK_CTRL0, CKG_TX_DSI_CLK_MPLL_108M );
        break;
     case DSI_TOP_CLK_MPLL_240M:
        W2BYTE(CLKG_TX_DSI_CLK_CTRL0, CKG_TX_DSI_CLK_MPLL_240M );
        break;
    case DSI_TOP_CLK_LPLL:
        W2BYTE(CLKG_TX_DSI_CLK_CTRL0, CKG_TX_DSI_CLK_LPLL_CLK );
        break;
    }
}

LCM_PARAMS  Hal_DSI_GetInfo(void)
{
    return glMIPIDSI_Params;
}

void Hal_DSI_LPLL(LCM_PARAMS *lcm_params)
{
        u32 u32dclk=0;
        u32 u32Mbps=0;
        u32 u32dclkHz=0;
        u32  u32HTotal=0;
        u32  u32VTotal=0;
        u8 BitsPerPixel;
        u8 u8LaneNum=lcm_params->LANE_NUM;
        u32 u32LpllInput=0;
        u8 LPLL_LOOP_DIV=16;
        u32 u32Fract=0;

        //u32  u32RGB_H_BLANK=0;
        u32  u32RGB_H=0;
        //u32  u32RGB_V_BLANK=0;
        u32  u32RGB_V=0;
        u32  u32RGB_HPW=0;
        u32  u32RGB_H_BP=0;
        u32  u32RGB_H_FP=0;
        u32  u32RGB_VPW=0;
        u32  u32RGB_V_BP=0;
        u32  u32RGB_V_FP=0;

        glMIPIDSI_Params = *lcm_params;

        BitsPerPixel=(lcm_params->format == DSI_FORMAT_RGB565)?16:((lcm_params->format == DSI_FORMAT_RGB666)?18:24);

        u32HTotal= (lcm_params->u32HActive + lcm_params->u32HPW + lcm_params->u32HBP+lcm_params->u32HFP);
        u32VTotal= (lcm_params->u32VActive + lcm_params->u32VPW + lcm_params->u32VBP+lcm_params->u32VFP);
        u32dclk = u32HTotal* u32VTotal * lcm_params->u8FPS;

        u32dclkHz= u32dclk/1000;

        u32Mbps = (u32dclkHz * BitsPerPixel)/u8LaneNum;

        HAL_MIPI_DSI_DBG(1, "DSI DCLK= %d Hz\n", u32dclkHz);
        HAL_MIPI_DSI_DBG(1, "BIT DCLK= %d Mbps\n",  u32Mbps/1000);

        u32RGB_HPW= (lcm_params->u32HPW*54000)/u32dclkHz;
        u32RGB_H_BP= (lcm_params->u32HBP*54000)/u32dclkHz;
        u32RGB_H_FP= (lcm_params->u32HFP*54000)/u32dclkHz;
        u32RGB_H= ((lcm_params->u32HActive+lcm_params->u32BLLP)*54000)/u32dclkHz;

        u32RGB_VPW= lcm_params->u32VPW;
        u32RGB_V_BP= lcm_params->u32VBP;
        u32RGB_V_FP= lcm_params->u32VFP + 4;
        u32RGB_V= lcm_params->u32VActive;

        HAL_MIPI_DSI_DBG(1, "HTotal=%d ,  HPW= %d,   HBP= %d,    HFP= %d, BLLP=%d    \n", u32HTotal, lcm_params->u32HPW,
                                                                                                                        lcm_params->u32HBP, lcm_params->u32HFP, lcm_params->u32BLLP);
        HAL_MIPI_DSI_DBG(1, "VTotal=%d ,  VPW= %d,   VBP= %d,    VFP= %d    \n", u32VTotal, lcm_params->u32VPW,
                                                                                                                        lcm_params->u32VBP, lcm_params->u32VFP);

        HAL_MIPI_DSI_DBG(1, "RGB HTotal=%d ,  HPW= %d,   HBP= %d,    HFP= %d  \n",
                    (u32HTotal*54000)/u32dclkHz, u32RGB_HPW,
                    u32RGB_H_BP,
                    u32RGB_H_FP);

        HAL_MIPI_DSI_DBG(1, "RGB VTotal=%d ,  VPW= %d,   VBP= %d,    VFP= %d  \n",
                    (u32VTotal), u32RGB_VPW,
                    u32RGB_V_BP,
                    u32RGB_V_FP);
#if 0
        W2BYTE(PNL_RGB_V_BPORCH, u32RGB_V_BP);
	 W2BYTEMSK(PNL_RGB_SYNC_WIDTH, (u32RGB_VPW-1)<<8, PNL_RGB_VSYNC_W_MASK);
	 W2BYTE(PNL_RGB_V_BLANK, (u32RGB_V_BP+u32RGB_V_FP+u32RGB_VPW));
        W2BYTE(DEC_MISC_MVOP_TRIG_OFF, (2+(u32RGB_V_BP+u32RGB_V_FP+u32RGB_VPW)-(u32RGB_V_BP)));

	 W2BYTE(PNL_RGB_H_BPORCH, u32RGB_H_BP);
	 W2BYTEMSK(PNL_RGB_SYNC_WIDTH, (u32RGB_HPW-1)<<0, PNL_RGB_HSYNC_W_MASK);
	 W2BYTE(PNL_RGB_H_BLANK, (u32RGB_H_BP+u32RGB_H_FP+u32RGB_HPW));

        W2BYTE(PNL_LCD_WIDTH, u32RGB_H);
	 W2BYTE(PNL_LCD_HEIGHT, u32RGB_V);
#endif
        u32LpllInput= (u32Mbps*lcm_params->LANE_NUM)/(LPLL_LOOP_DIV);

        u32Fract=(u32)((MPLL_2_LPLL_FRACT)/(u32LpllInput));

        //multiply/=1000000;
        //multiply*=(1<<19);

        #if 0
        HAL_MIPI_DSI_DBG(1, "MPLL_2_LPLL_FRACT= %x \n", (MPLL_2_LPLL_FRACT) & 0xFFFFFFFF);
        HAL_MIPI_DSI_DBG(1, "LpllInput= %d  \n", u32LpllInput);
        #endif
        HAL_MIPI_DSI_DBG(1, "5.19 fraction divisor= %d (dec), %x (hex)  \n", u32Fract*100,u32Fract*100);

        u32Fract =u32Fract*100;

        W2BYTE(REG_LPLL_BASE + 0x1E, 0x0000);
        W2BYTE(REG_LPLL_BASE + 0x20, 0x0040);

        W2BYTE(REG_LPLL_BASE + 0x90, u32Fract);
        W2BYTE(REG_LPLL_BASE + 0x92, (u32Fract>>16) & 0xFFFF);

        //HAL_MIPI_DSI_DBG(1, "%x, %x  \n",  R2BYTE(REG_LPLL_BASE + 0x90 ),R2BYTE(REG_LPLL_BASE + 0x92 ));

        if( (u32Mbps >= 100000) && (u32Mbps <= 200000)) {
            //reg_lpll_ext
            W2BYTE(REG_LPLL_BASE + 0x80, 0x2201 );
            //reg_lpll_ext_loop_div_sec VCO = LPLL input x 4 x 4 = 1.6G
            W2BYTE(REG_LPLL_BASE + 0x82, 0x0420 );
            // LPLL_CLKOUT_SCALAR = VCO / 32 = 1.6G/32 = 50MHz.
            W2BYTE(REG_LPLL_BASE + 0x84, 0x0083 );
            //reg_lpll_ext_skew_en_fixclk   & reg_lpll_ext_skew_en_skewclk
            W2BYTE(REG_LPLL_BASE + 0x86, 0x0003 );
            W2BYTE(REG_LPLL_BASE + 0x94, 0x0001 );
             W2BYTE(REG_LPLL_BASE + 0x96, 0x0000 );
            W2BYTEMSK(REG_LPLL_BASE + 0x88, 0x04<<8,0xFF00 );
        }
        else if( (u32Mbps >= 200000) && (u32Mbps < 400000)) {
            //reg_lpll_ext
            //W2BYTE(REG_LPLL_BASE + 0x80, 0x2201 );
            W2BYTE(REG_LPLL_BASE + 0x80, 0x2601 );
            //reg_lpll_ext_loop_div_sec VCO = LPLL input x 4 x 4 = 1.6G
            W2BYTE(REG_LPLL_BASE + 0x82, 0x0420 );
            // LPLL_CLKOUT_SCALAR = VCO / 32 = 1.6G/32 = 50MHz.
            //W2BYTE(REG_LPLL_BASE + 0x84, 0x0043 );
            W2BYTE(REG_LPLL_BASE + 0x84, 0x0232 );
            //reg_lpll_ext_skew_en_fixclk   & reg_lpll_ext_skew_en_skewclk
            //W2BYTE(REG_LPLL_BASE + 0x86, 0x0002 );
            W2BYTE(REG_LPLL_BASE + 0x86, 0x0001 );
            W2BYTE(REG_LPLL_BASE + 0x88, 0x0001 );
            //W2BYTE(REG_LPLL_BASE + 0x94, 0x0001 );
            W2BYTE(REG_LPLL_BASE + 0x94, 0x0000 );
            W2BYTE(REG_LPLL_BASE + 0x96, 0x0000 );
            W2BYTEMSK(REG_LPLL_BASE + 0x88, 0x04<<8,0xFF00 );
        }
        else if((u32Mbps >= 400000) && (u32Mbps <800000))
        {
             //reg_lpll_ext
            W2BYTE(REG_LPLL_BASE + 0x80, 0x2201 );
            //reg_lpll_ext_loop_div_sec VCO = LPLL input x 4 x 4 = 1.6G
            W2BYTE(REG_LPLL_BASE + 0x82, 0x0420 );
            // LPLL_CLKOUT_SCALAR = VCO / 32 = 1.6G/32 = 50MHz.
            W2BYTE(REG_LPLL_BASE + 0x84, 0x0042 );
            //reg_lpll_ext_skew_en_fixclk   & reg_lpll_ext_skew_en_skewclk
            W2BYTE(REG_LPLL_BASE + 0x86, 0x0001 );
            W2BYTE(REG_LPLL_BASE + 0x94, 0x0001 );
             W2BYTE(REG_LPLL_BASE + 0x96, 0x0000 );
            W2BYTEMSK(REG_LPLL_BASE + 0x88, 0x04<<8,0xFF00 );    //TBD

       }
       else if((u32Mbps >= 800000) && (u32Mbps <1500000))
        {
             //reg_lpll_ext
            W2BYTE(REG_LPLL_BASE + 0x80, 0x2201 );
            //reg_lpll_ext_loop_div_sec VCO = LPLL input x 4 x 4 = 1.6G
            W2BYTE(REG_LPLL_BASE + 0x82, 0x0420 );
            // LPLL_CLKOUT_SCALAR = VCO / 32 = 1.6G/32 = 50MHz.
            W2BYTE(REG_LPLL_BASE + 0x84, 0x0041 );
            //reg_lpll_ext_skew_en_fixclk   & reg_lpll_ext_skew_en_skewclk
            W2BYTE(REG_LPLL_BASE + 0x86, 0x0000 );
            W2BYTE(REG_LPLL_BASE + 0x94, 0x0001 );
             W2BYTE(REG_LPLL_BASE + 0x96, 0x0000 );
            W2BYTEMSK(REG_LPLL_BASE + 0x88, 0x04<<8,0xFF00 );

       }
}


void Hal_DSI_PadOutSel(DSI_TOTAL_LANE_NUM LaneNum)
{
    //disable RGB pad
    W2BYTEMSK(PADTOP1_LCDPAD_MUX, 0, RGB_PAD_MASK);

    switch(LaneNum)
    {
        case DSI_LANE_NUM_4:
            W2BYTEMSK(PADTOP1_TX_MODE, MIPITX_4LANE_MODE , MIPITX_PAD_MASK);
            break;
        case DSI_LANE_NUM_2:
            W2BYTEMSK(PADTOP1_TX_MODE, MIPITX_2LANE_MODE , MIPITX_PAD_MASK);
            break;
        default:
            HAL_MIPI_DSI_DBG(3, "[MIPI DSI] \n", __FUNCTION__, __LINE__);
            break;
    }

    W2BYTE(PADTOP1_TX_MODE + REG_PADTOP1_46, 0 ); //lcd ie
    W2BYTE(PADTOP1_TX_MODE + REG_PADTOP1_47, 0 ); //lcd ie
    W2BYTE(PADTOP1_TX_MODE + REG_PADTOP1_48, 0 ); //lcd pe
    W2BYTE(PADTOP1_TX_MODE + REG_PADTOP1_49, 0 ); //lcd pe

}

bool Hal_DSI_clk_HS_state(void)
{
    return (ReadRegBit(DSI_REG_BASE +(REG_DSI_PHY_LCCON>>1), DSI_LC_HSTX_EN))? TRUE : FALSE;
}

void Hal_DSI_clk_HS_mode(bool enter)
{
    if(enter && !Hal_DSI_clk_HS_state()) {
                 W4BYTEMSK(DSI_REG_BASE +(REG_DSI_PHY_LCCON>>1), 1, DSI_LC_HSTX_EN); //
    }
    else if (!enter && Hal_DSI_clk_HS_state()) {
               W4BYTEMSK(DSI_REG_BASE +(REG_DSI_PHY_LCCON>>1), 0, DSI_LC_HSTX_EN); //
    }
}


static u16 u16MCUClkSetting;

static void _Hal_DSI_StoreMCUClk(void)
{
    u16MCUClkSetting = R2BYTE(CLKG_MCU_CLK_CTRL0);
    W2BYTE(CLKG_MCU_CLK_CTRL0, 0 );
}

static void _Hal_DSI_BackupMCUClk(void)
{
    W2BYTE(CLKG_MCU_CLK_CTRL0, u16MCUClkSetting );
}

DSI_STATUS Hal_DSI_Switch_Mode(DSI_MODE_CTL mode)
{
    _Hal_DSI_Reset();
CamOsPrintf("%s: Switch DSI Mode to %d\n", __FUNCTION__, mode);
    switch(mode)
    {
    case DSI_CMD_MODE:
        _Hal_DSI_StoreMCUClk();
        W4BYTEMSK(DSI_REG_BASE +(REG_DSI_MODE_CON>>1), DSI_MODE_CON_CMD, DSI_MODE_MASK);
        break;
    case DSI_SYNC_PULSE_MODE:
        _Hal_DSI_BackupMCUClk();
        W4BYTEMSK(DSI_REG_BASE +(REG_DSI_MODE_CON>>1), DSI_MODE_CON_VID_SYNC_PULSE, DSI_MODE_MASK);
        break;
    case DSI_SYNC_EVENT_MODE:
        _Hal_DSI_BackupMCUClk();
        W4BYTEMSK(DSI_REG_BASE +(REG_DSI_MODE_CON>>1), DSI_MODE_CON_VID_SYNC_EVENT, DSI_MODE_MASK);
        break;
    case DSI_BURST_MODE:
        _Hal_DSI_BackupMCUClk();
        W4BYTEMSK(DSI_REG_BASE +(REG_DSI_MODE_CON>>1), DSI_MODE_CON_BURST, DSI_MODE_MASK);
        break;
    }

    return DSI_STATUS_OK;
}


DSI_STATUS Hal_DSI_SetLaneNum(DSI_TOTAL_LANE_NUM LaneNum)
{
    switch(LaneNum)
    {
    case DSI_LANE_NUM_1:
        W4BYTEMSK(DSI_REG_BASE +(REG_DSI_TXRX_CON>>1), 0x1<<2, DSI_LANE_NUM);
        break;
    case DSI_LANE_NUM_2:
        W4BYTEMSK(DSI_REG_BASE +(REG_DSI_TXRX_CON>>1), 0x3<<2, DSI_LANE_NUM);
        break;
    case DSI_LANE_NUM_3:
        W4BYTEMSK(DSI_REG_BASE +(REG_DSI_TXRX_CON>>1), 0x7<<2, DSI_LANE_NUM);
        break;
    case DSI_LANE_NUM_4:
        W4BYTEMSK(DSI_REG_BASE +(REG_DSI_TXRX_CON>>1), 0xF<<2, DSI_LANE_NUM);
        break;
    case DSI_LANE_DIS:
        W4BYTEMSK(DSI_REG_BASE +(REG_DSI_TXRX_CON>>1), 0x0<<2, DSI_LANE_NUM);
        break;
    }
    return DSI_STATUS_OK;
}

//-------------------------------------------------------------------------------------------------
//  Command mode Functions
//-------------------------------------------------------------------------------------------------
DSI_STATUS  Hal_DSI_WriteShortPacket(u8 count, u8 cmd, u8 *para_list)
{
    u16 cmdQIdx=0;
    //PDSI_T0_INS ptrT0;
    DSI_CMDQ_CONFG cmdqCfg;
    DSI_T0_INS t0;
    u32 u32DsiIntSta=0;

	cmdQIdx = 0;
    cmdqCfg.BTA=0;
    cmdqCfg.HS=0; //Low power mode transfer
    cmdqCfg.type=0; //type0
    cmdqCfg.TE=0;
    cmdqCfg.Rsv=0;
    cmdqCfg.CL=0;
    cmdqCfg.RPT=0;

    t0.CONFG= 0;

    if((para_list == NULL)  && (count != 0))
           return DSI_STATUS_PACKET_TYPE_ERROR;

    if(count >2) {
        return DSI_STATUS_PACKET_TYPE_ERROR;
    }
    else {
        if(count==2) {
             t0.Data_ID= DSI_DCS_SHORT_PACKET_ID_1 ;  //
             t0.Data0 =cmd;
             t0.Data1 =*(para_list);
        }
        else if(count==1) {
             t0.Data_ID= DSI_DCS_SHORT_PACKET_ID_0; // ;
             t0.Data0 =cmd;
             t0.Data1 =0;
        }
    }
    HAL_MIPI_DSI_DBG(1, "t0 =%x\n ",AS_UINT32(&t0));
    //clear cmd done flag first
    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_INTSTA>>1), 0, DSI_CMD_DONE_INT_FLAG);

    W4BYTE(DSI_CMDQ_BASE +(cmdQIdx*2),  AS_UINT32(&t0));

    HAL_MIPI_DSI_DBG(1, "Read result =%x\n ",R4BYTE(DSI_CMDQ_BASE +(cmdQIdx*2)));

    W4BYTEMSK(DSI_REG_BASE +(REG_DSI_CMDQ_CON>>1), 1, DSI_CMDQ_SIZE);
    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_START>>1), 0, DSI_DSI_START); //clear first
    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_START>>1), 1, DSI_DSI_START);

    do{
        u32DsiIntSta =R4BYTE(DSI_REG_BASE + (REG_DSI_INTSTA>>1));
        HAL_MIPI_DSI_DBG(1, " u32DsiIntSta = %x \n ", u32DsiIntSta);
     } while( !(u32DsiIntSta & DSI_CMD_DONE_INT_FLAG));
      WriteLongRegBit(DSI_REG_BASE +(REG_DSI_INTSTA>>1), 0, DSI_CMD_DONE_INT_FLAG);

    #if 0
    W4BYTEMSK(DSI_CMDQ_BASE +offset, 0, DSI_TYPE);

    W4BYTEMSK(DSI_CMDQ_BASE +offset,  dataID<<8, DSI_DATA_ID);
    WriteLongRegBit(DSI_CMDQ_BASE +offset, 0, DSI_HS);
    WriteLongRegBit(DSI_CMDQ_BASE +offset, 0, DSI_BTA);

    W4BYTEMSK(DSI_CMDQ_BASE +offset,  data0<<16, DSI_DATA_0);


    W4BYTEMSK(DSI_REG_BASE +(REG_DSI_CMDQ_CON>>1), count, DSI_CMDQ_SIZE);

    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_START>>1), 0, DSI_DSI_START); //clear first
    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_START>>1), 1, DSI_DSI_START);
    #endif
    return DSI_STATUS_OK;
}


DSI_STATUS  Hal_DSI_ReadShortPacket(u8 u8ReadBackCount, u8 u8RegAddr)
{

    DSI_T0_INS t0;
    u32 u32DsiIntSta=0;
    u32 u32RXData;

    //Step1: Send Maxmum return packet with expected returned byte count
    t0.CONFG= 0;
    t0.Data_ID=DSI_DCS_MAXIMUM_RETURN_PACKET_SIZE ;
    t0.Data0 =u8ReadBackCount;

    //clear cmd done flag first
    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_INTSTA>>1), 0, DSI_CMD_DONE_INT_FLAG);

    W4BYTE(DSI_CMDQ_BASE + 0,  AS_UINT32(&t0));
    W4BYTEMSK(DSI_REG_BASE +(REG_DSI_CMDQ_CON>>1), 1, DSI_CMDQ_SIZE);
    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_START>>1), 0, DSI_DSI_START); //clear first
    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_START>>1), 1, DSI_DSI_START);

    //wait cmd done flag
    do{
        u32DsiIntSta =R4BYTE(DSI_REG_BASE + (REG_DSI_INTSTA>>1));
    } while( !(u32DsiIntSta & DSI_CMD_DONE_INT_FLAG));
    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_INTSTA>>1), 0, DSI_CMD_DONE_INT_FLAG);

    //Step2: Set Tx Maxmum return packet size
    W4BYTEMSK(DSI_REG_BASE +(REG_DSI_TXRX_CON>>1), u8ReadBackCount<<12, DSI_MAX_RTN_SIZE);

     //Step3: Send DCS long read cmd

    t0.CONFG= 0x03;
    t0.Data_ID=DSI_DCS_READ_PACKET_ID ;
    t0.Data0 =u8RegAddr;

    W4BYTE(DSI_CMDQ_BASE + 0,  AS_UINT32(&t0));
    W4BYTEMSK(DSI_REG_BASE +(REG_DSI_CMDQ_CON>>1), 1, DSI_CMDQ_SIZE);
    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_START>>1), 0, DSI_DSI_START); //clear first
    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_START>>1), 1, DSI_DSI_START);
     //wait cmd done flag
    do{
        u32DsiIntSta =R4BYTE(DSI_REG_BASE + (REG_DSI_INTSTA>>1));
    } while( !(u32DsiIntSta & DSI_CMD_DONE_INT_FLAG));
    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_INTSTA>>1), 0, DSI_CMD_DONE_INT_FLAG);

    u32RXData = R4BYTE(DSI_REG_BASE + (REG_DSI_RX_DATA03>>1));
    HAL_MIPI_DSI_DBG(1, "Byte0~3 =%x\n",u32RXData);
     u32RXData = R4BYTE(DSI_REG_BASE + (REG_DSI_RX_DATA47>>1));
    HAL_MIPI_DSI_DBG(1, "Byte4~7 =%x\n",u32RXData);
    u32RXData = R4BYTE(DSI_REG_BASE + (REG_DSI_RX_DATA8B>>1));
    HAL_MIPI_DSI_DBG(1, "Byte8~b =%x\n",u32RXData);
    u32RXData = R4BYTE(DSI_REG_BASE + (REG_DSI_RX_DATAC>>1));
    HAL_MIPI_DSI_DBG(1, "Bytec~f =%x\n",u32RXData);

   return DSI_STATUS_OK;
}


DSI_STATUS  Hal_DSI_WriteLongPacket(u8 count, u8 cmd, u8 *para_list)
{
    u8 i;
    //u8 u8Index=0;
    //u8 data0=0;
    u16 offset=0;
    //u32 u32data=0;
    u8 u8CmdStIdx=0;

    PDSI_T2_INS t2;
    DSI_CMDQ_CONFG cmdqCfg;
    DSI_CMDQ CmdqDataSeq;
    PDSI_CMDQ PCmdqDataSeq;
    u8 u8CMDQNum=0;
    u32 u32DsiIntSta=0;
    bool firstCmd =FALSE;

    if(para_list == NULL)
           return DSI_STATUS_ERROR;

    t2=(PDSI_T2_INS) (CHIP_BASE(u32, DSI_CMDQ_BASE_2, offset, 2));

    cmdqCfg.BTA=0;
    cmdqCfg.HS=0; //Low power mode transfer
    cmdqCfg.type=2; //type2
    cmdqCfg.TE=0;

    t2->CONFG= 0x02; //Low power mode transfer, type2

     t2->Data_ID=DSI_DCS_LONG_PACKET_ID;
     t2->WC16 =count;

     //HAL_MIPI_DSI_DBG(1, "u32Addr =%x,  val=%x ", CHIP_BASE(u32, DSI_CMDQ_BASE_2, offset, 2) , R4BYTE(DSI_CMDQ_BASE_2+offset<<1) );
    offset = 1;
    PCmdqDataSeq=(PDSI_CMDQ) (CHIP_BASE(u32, DSI_CMDQ_BASE_2, offset, 2));

    u8CMDQNum = (count/4);
    u8CmdStIdx =1;



    for(i=0; i <u8CMDQNum ; i++)
    {
        if(!firstCmd) {
            CmdqDataSeq.byte0=cmd;
            firstCmd =1;
        }else {
            CmdqDataSeq.byte0=*para_list;
            para_list+=1;
        }
        CmdqDataSeq.byte1=*para_list;
        para_list+=1;
        CmdqDataSeq.byte2=*para_list;
        para_list+=1;
        CmdqDataSeq.byte3=*para_list;
        para_list+=1;
        W4BYTE(DSI_CMDQ_BASE +((u8CmdStIdx)*2),  AS_UINT32(&CmdqDataSeq));
        //if (i==0)
        //    HAL_MIPI_DSI_DBG(1, "byte0=%x, byte1=%x, byte2=%x, byte3=%x, \n ", CmdqDataSeq.byte0, CmdqDataSeq.byte1, CmdqDataSeq.byte2, CmdqDataSeq.byte3);
        //HAL_MIPI_DSI_DBG(1, " u8CmdStIdx =%d, CMDQ=%x \n ", u8CmdStIdx, R4BYTE(DSI_CMDQ_BASE +(u8CmdStIdx*2)));
        u8CmdStIdx++;

        //u8Index++;
    }

    u8CMDQNum += (count%4) ?1:0;

    CmdqDataSeq.byte0=0x0;
    CmdqDataSeq.byte1=0x0;
    CmdqDataSeq.byte2=0x0;
    CmdqDataSeq.byte3=0x0;

    if((count%4) == 3) {
         if(!firstCmd) {
             CmdqDataSeq.byte0=cmd;
            firstCmd =1;
        }else {
             CmdqDataSeq.byte0=*para_list;
            para_list+=1;
        }
        CmdqDataSeq.byte1=*para_list;
        para_list+=1;
        CmdqDataSeq.byte2=*para_list;
        para_list+=1;
       //u8CmdStIdx++;

        //HAL_MIPI_DSI_DBG(1, "byte0=%x, byte1=%x, byte2=%x, byte3=%x, \n ", CmdqDataSeq.byte0, CmdqDataSeq.byte1, CmdqDataSeq.byte2, CmdqDataSeq.byte3);
        if(count ==3)
            u8CmdStIdx =1;
        W4BYTE(DSI_CMDQ_BASE +(u8CmdStIdx*2),  AS_UINT32(&CmdqDataSeq));
        //HAL_MIPI_DSI_DBG(1, " u8CmdStIdx =%d, CMDQ=%x \n ", u8CmdStIdx, R4BYTE(DSI_CMDQ_BASE +(u8CmdStIdx*2)));
    }
    else if((count%4) == 2){
         if(!firstCmd) {
             CmdqDataSeq.byte0=cmd;
            firstCmd =1;
        }else {
             CmdqDataSeq.byte0=*para_list;
            para_list+=1;
        }
        CmdqDataSeq.byte1=*para_list;
        para_list+=1;
        //PCmdqDataSeq[u8Index]=CmdqDataSeq ;
       // u8CmdStIdx++;
       //HAL_MIPI_DSI_DBG(1, "Case 2 byte0=%x, byte1=%x, byte2=%x, byte3=%x, \n ", CmdqDataSeq.byte0, CmdqDataSeq.byte1, CmdqDataSeq.byte2, CmdqDataSeq.byte3);
        W4BYTE(DSI_CMDQ_BASE +(u8CmdStIdx*2),  AS_UINT32(&CmdqDataSeq));
        //HAL_MIPI_DSI_DBG(1, "Case 2   u8CmdStIdx =%d, CMDQ=%x \n ", u8CmdStIdx, R4BYTE(DSI_CMDQ_BASE +(u8CmdStIdx*2)));

    }
    else if((count%4) == 1) {
         if(!firstCmd) {
             CmdqDataSeq.byte0=cmd;
            firstCmd =1;
        }else {
             CmdqDataSeq.byte0=*para_list;
            para_list+=1;
        }
        //PCmdqDataSeq[u8Index]=CmdqDataSeq ;
       // u8CmdStIdx++;
        //HAL_MIPI_DSI_DBG(1, "byte0=%x, byte1=%x, byte2=%x, byte3=%x, \n ", CmdqDataSeq.byte0, CmdqDataSeq.byte1, CmdqDataSeq.byte2, CmdqDataSeq.byte3);
        W4BYTE(DSI_CMDQ_BASE +(u8CmdStIdx*2),  AS_UINT32(&CmdqDataSeq));
       //HAL_MIPI_DSI_DBG(1, "Case3 u8CmdStIdx =%d, CMDQ=%x \n ", u8CmdStIdx, R4BYTE(DSI_CMDQ_BASE +(u8CmdStIdx*2)));

    }

    u8CMDQNum +=1;
    //HAL_MIPI_DSI_DBG(1, " u8CMDQNum =%d \n ", u8CMDQNum);
    //if(u8CmdStIdx !=u8CMDQNum )
     //   HAL_MIPI_DSI_DBG(1, " Wrong CMDQNum\n ");

    //clear cmd done flag first
	#if 0
        {
    UartSendTrace("------------------------------------\n");
    UartSendTrace("Bank Addr : 0x1A2200     \n");
    UartSendTrace("------------------------------------\n");
    offset =0;

    UartSendTrace("        ", 0);
    for(i=0 ; i < 8; i++) {
        UartSendTrace("%08X ",i*4);
    }
    UartSendTrace("\n");
    for(i=0 ; i < 16; i++) {
        UartSendTrace("%03X     ", offset<<1);
        for(j=0; j<8 ; j++) {
            UartSendTrace("%08X ", Hal_MIPI_DSI_RegisterRead(0x1A2200+offset));
            offset +=2;
        }
        UartSendTrace("  \n");
    }
    }
#endif
    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_INTSTA>>1), 0, DSI_CMD_DONE_INT_FLAG);

    W4BYTEMSK(DSI_REG_BASE +(REG_DSI_CMDQ_CON>>1), u8CMDQNum, DSI_CMDQ_SIZE);

    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_START>>1), 0, DSI_DSI_START); //clear first
    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_START>>1), 1, DSI_DSI_START);

    do{
        u32DsiIntSta =R4BYTE(DSI_REG_BASE + (REG_DSI_INTSTA>>1));
        //HAL_MIPI_DSI_DBG(1, " u32DsiIntSta = %x \n ", u32DsiIntSta);
     } while( !(u32DsiIntSta & DSI_CMD_DONE_INT_FLAG));
      WriteLongRegBit(DSI_REG_BASE +(REG_DSI_INTSTA>>1), 0, DSI_CMD_DONE_INT_FLAG);

    _HalIsEngineBusy();
    return DSI_STATUS_OK;
}

DSI_STATUS  Hal_DSI_ReadLongPacket(u8 u8ReadBackCount, u8 u8RegAddr)
{
    DSI_T0_INS t0;
    u32 u32DsiIntSta=0;
    u32 u32RXData;

    //Step1: Send Maxmum return packet(0x37) with expected returned byte count
    t0.CONFG= 0;
    t0.Data_ID=DSI_DCS_MAXIMUM_RETURN_PACKET_SIZE ;
    t0.Data0 =u8ReadBackCount;

    _Hal_DSI_Reset();
    Hal_DSI_Switch_Mode(DSI_CMD_MODE);

    W4BYTEMSK(DSI_REG_BASE +(REG_DSI_MEM_CONTI>>1), 0x3e, DSI_DSI_RWMEM_CONTI);

    //clear cmd done flag first
    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_INTSTA>>1), 0, DSI_CMD_DONE_INT_FLAG);

    W4BYTE(DSI_CMDQ_BASE + 0,  AS_UINT32(&t0));
    W4BYTEMSK(DSI_REG_BASE +(REG_DSI_CMDQ_CON>>1), 1, DSI_CMDQ_SIZE);
    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_START>>1), 0, DSI_DSI_START); //clear first
    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_START>>1), 1, DSI_DSI_START);

    HAL_MIPI_DSI_DBG(1, "wait cmd done flag\n");

    //wait cmd done flag
    do{
        u32DsiIntSta =R4BYTE(DSI_REG_BASE + (REG_DSI_INTSTA>>1));
    } while( !(u32DsiIntSta & DSI_CMD_DONE_INT_FLAG));
    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_INTSTA>>1), 0, DSI_CMD_DONE_INT_FLAG);

    //Step2: Set Tx Maxmum return packet size
    W4BYTEMSK(DSI_REG_BASE +(REG_DSI_TXRX_CON>>1), u8ReadBackCount<<12, DSI_MAX_RTN_SIZE);

     //Step3: Send DCS long read cmd
    t0.CONFG= 0x03;
    t0.Data_ID=DSI_DCS_READ_PACKET_ID ;
    t0.Data0 =u8RegAddr;

    W4BYTE(DSI_CMDQ_BASE + 0,  AS_UINT32(&t0));
    W4BYTEMSK(DSI_REG_BASE +(REG_DSI_CMDQ_CON>>1), 1, DSI_CMDQ_SIZE);
    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_START>>1), 0, DSI_DSI_START); //clear first
    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_START>>1), 1, DSI_DSI_START);


     //Step4: Read Receive packet data
    u32RXData = R4BYTE(DSI_REG_BASE + (REG_DSI_RX_DATA03>>1));
    HAL_MIPI_DSI_DBG(1, "Byte0~3 =%x\n",u32RXData);
     u32RXData = R4BYTE(DSI_REG_BASE + (REG_DSI_RX_DATA47>>1));
    HAL_MIPI_DSI_DBG(1, "Byte4~7 =%x\n",u32RXData);
    u32RXData = R4BYTE(DSI_REG_BASE + (REG_DSI_RX_DATA8B>>1));
    HAL_MIPI_DSI_DBG(1, "Byte8~b =%x\n",u32RXData);
    u32RXData = R4BYTE(DSI_REG_BASE + (REG_DSI_RX_DATAC>>1));
    HAL_MIPI_DSI_DBG(1, "Bytec~f =%x\n",u32RXData);

    u32DsiIntSta =R4BYTE(DSI_REG_BASE + (REG_DSI_INTSTA>>1));
    HAL_MIPI_DSI_DBG(1, " u32DsiIntSta = %x \n ", u32DsiIntSta);
    //wait cmd done flag

    //Step3: wait RX rdy  flag, Issue interrupt and receive slave response
    HAL_MIPI_DSI_DBG(1, "wait RX rdy  flag\n");
    do{
        u32DsiIntSta =R4BYTE(DSI_REG_BASE + (REG_DSI_INTSTA>>1));
        HAL_MIPI_DSI_DBG(1, " u32DsiIntSta = %x \n ", u32DsiIntSta);
    } while( !(u32DsiIntSta & DSI_LPRX_RD_RDY_INT_FLAG));

     //Step4: Read Receive packet data
    u32RXData = R4BYTE(DSI_REG_BASE + (REG_DSI_RX_DATA03>>1));
    HAL_MIPI_DSI_DBG(1, "Byte0~3 =%x\n",u32RXData);
     u32RXData = R4BYTE(DSI_REG_BASE + (REG_DSI_RX_DATA47>>1));
    HAL_MIPI_DSI_DBG(1, "Byte4~7 =%x\n",u32RXData);
    u32RXData = R4BYTE(DSI_REG_BASE + (REG_DSI_RX_DATA8B>>1));
    HAL_MIPI_DSI_DBG(1, "Byte8~b =%x\n",u32RXData);
    u32RXData = R4BYTE(DSI_REG_BASE + (REG_DSI_RX_DATAC>>1));
    HAL_MIPI_DSI_DBG(1, "Bytec~f =%x\n",u32RXData);
    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_INTSTA>>1), 0, DSI_LPRX_RD_RDY_INT_FLAG);

    //Step5: Reponse read ack to module and go to next commends in queue
    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_RX_RACK>>1), 1, DSI_RACK);

    //Step6: wait cmd done flag
   do{
        u32DsiIntSta =R4BYTE(DSI_REG_BASE + (REG_DSI_INTSTA>>1));
    } while( !(u32DsiIntSta & DSI_CMD_DONE_INT_FLAG));
    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_INTSTA>>1), 0, DSI_CMD_DONE_INT_FLAG);

    return DSI_STATUS_OK;
}


DSI_STATUS Hal_DSI_TXRX_Control(bool cksm_en,
                                                                 bool ecc_en,
                                                                u8 lane_num,
                                                                u8 vc_num,
                                                                bool null_packet_en,
                                                                bool err_correction_en,
                                                                bool dis_eotp_en,
								        bool hstx_cklp_en,
                                                               u32 max_return_size)
{

    //WriteLongRegBit(DSI_REG_BASE +(REG_DSI_TXRX_CON>>1), 0, DSI_LANE_NUM); //clear first
    return DSI_STATUS_OK;

}
DSI_STATUS Hal_DSI_Config_VDO_Timing(LCM_PARAMS *lcm_params)
{

	//unsigned int line_byte;
	unsigned int horizontal_sync_active_byte;
	unsigned int horizontal_backporch_byte;
	unsigned int horizontal_frontporch_byte;
	unsigned int horizontal_bllp_byte;
	unsigned int dsiTmpBufBpp;


	if(lcm_params->format == DSI_FORMAT_RGB565)
		dsiTmpBufBpp = 16;
	else if( (lcm_params->format == DSI_FORMAT_LOOSELY_RGB666) || (lcm_params->format == DSI_FORMAT_RGB888) )
		dsiTmpBufBpp = 24;
    else if( (lcm_params->format == DSI_FORMAT_RGB666) )
		dsiTmpBufBpp = 18;

        HAL_MIPI_DSI_DBG(1, "dsiTmpBufBpp = %d \n ", dsiTmpBufBpp);

        W4BYTE(DSI_REG_BASE +(REG_DSI_VACT_NL>>1),  lcm_params->u32VActive);
        W4BYTE(DSI_REG_BASE +(REG_DSI_VSA_NL>>1),  lcm_params->u32VPW);
        W4BYTE(DSI_REG_BASE +(REG_DSI_VBP_NL>>1),  lcm_params->u32VBP);
        W4BYTE(DSI_REG_BASE +(REG_DSI_VFP_NL>>1),  lcm_params->u32VFP);

        if (lcm_params->vdo_mode == DSI_SYNC_EVENT_MODE || lcm_params->vdo_mode == DSI_BURST_MODE ){
		//ASSERT((lcm_params->dsi.horizontal_backporch + lcm_params->dsi.horizontal_sync_active) * dsiTmpBufBpp> 9);
		horizontal_backporch_byte		=	((lcm_params->u32HBP+ lcm_params->u32HPW)* dsiTmpBufBpp - 10);
	}
	else{
		//ASSERT(lcm_params->dsi.horizontal_sync_active * dsiTmpBufBpp > 9);
		if( ((lcm_params->u32HPW * dsiTmpBufBpp)/8) > 9) {
		    HAL_MIPI_DSI_DBG(0, "[DSI ERROR] horizontal_sync_active byte count less than 9\n ");
        }
		horizontal_sync_active_byte = (((lcm_params->u32HPW * dsiTmpBufBpp)/8) > 9) ? ( (lcm_params->u32HPW* dsiTmpBufBpp)/8 - 10):0;
	    HAL_MIPI_DSI_DBG(1, " horizontal_sync_active_byte = %d \n ", horizontal_sync_active_byte);

		//ASSERT(lcm_params->dsi.horizontal_backporch * dsiTmpBufBpp > 9);
		if( ((lcm_params->u32HBP * dsiTmpBufBpp)/8) > 9) {
            HAL_MIPI_DSI_DBG(0, "[DSI ERROR] horizontal_backporch byte count less than 9\n ");
            HAL_MIPI_DSI_DBG(0, " horizontal_backporch_byte = %d \n ", (lcm_params->u32HBP * dsiTmpBufBpp)/8);
        }
		horizontal_backporch_byte =	(((lcm_params->u32HBP * dsiTmpBufBpp)/8) > 9 )? ( (lcm_params->u32HBP * dsiTmpBufBpp)/8 - 10):0;
	}

	//ASSERT(lcm_params->dsi.horizontal_frontporch * dsiTmpBufBpp > 11);
	horizontal_frontporch_byte			=	((lcm_params->u32HFP* dsiTmpBufBpp)/8 > 12) ?((lcm_params->u32HFP* dsiTmpBufBpp)/8 - 12) : 0;
    HAL_MIPI_DSI_DBG(1, "HFP = %d \n ", lcm_params->u32HFP);
    horizontal_bllp_byte = ((lcm_params->u32BLLP* dsiTmpBufBpp)/8 >6)?	((lcm_params->u32BLLP* dsiTmpBufBpp)/8 -6):0;

//	ASSERT(lcm_params->dsi.horizontal_frontporch * dsiTmpBufBpp > ((300/dsi_cycle_time) * lcm_params->dsi.LANE_NUM));
//	horizontal_frontporch_byte -= ((300/dsi_cycle_time) * lcm_params->dsi.LANE_NUM);

        horizontal_frontporch_byte -= ( (lcm_params->CLK_HS_EXIT+lcm_params->LPX+lcm_params->HS_PRPR+lcm_params->HS_ZERO)* lcm_params->LANE_NUM);

        W4BYTE(DSI_REG_BASE +(REG_DSI_HSA_WC>>1),  ALIGN_TO(horizontal_sync_active_byte,4));
        W4BYTE(DSI_REG_BASE +(REG_DSI_HBP_WC>>1),  ALIGN_TO(horizontal_backporch_byte,4));
        W4BYTE(DSI_REG_BASE +(REG_DSI_HFP_WC>>1),  ALIGN_TO(horizontal_frontporch_byte,4));
        W4BYTE(DSI_REG_BASE +(REG_DSI_BLLP_WC>>1),  ALIGN_TO(horizontal_bllp_byte,4));

        W4BYTEMSK(DSI_REG_BASE +(REG_DSI_PSCON>>1), ALIGN_TO((dsiTmpBufBpp*lcm_params->u32HActive)/8,4) ,DSI_TDSI_PS_WC);

        HAL_MIPI_DSI_DBG(1, " PS_WC= %d \n ",  R4BYTE(DSI_REG_BASE +(REG_DSI_PSCON>>1)));
        HAL_MIPI_DSI_DBG(1, " HSA_WC= %d \n ",  R4BYTE(DSI_REG_BASE +(REG_DSI_HSA_WC>>1)));
        HAL_MIPI_DSI_DBG(1, " HBP_WC= %d \n ",  R4BYTE(DSI_REG_BASE +(REG_DSI_HBP_WC>>1)));
        HAL_MIPI_DSI_DBG(1, " HFP_WC= %d \n ",  R4BYTE(DSI_REG_BASE +(REG_DSI_HFP_WC>>1)));
        if(lcm_params->format == DSI_FORMAT_RGB565)
        {
            W4BYTEMSK(DSI_REG_BASE +(REG_DSI_PSCON>>1), 0<<16 ,DSI_DSI_PS_SEL);
        }
        else if (lcm_params->format == DSI_FORMAT_RGB666)
        {
            W4BYTEMSK(DSI_REG_BASE +(REG_DSI_PSCON>>1), 1<<16 ,DSI_DSI_PS_SEL);
        }
        else if (lcm_params->format == DSI_FORMAT_LOOSELY_RGB666)
        {
            W4BYTEMSK(DSI_REG_BASE +(REG_DSI_PSCON>>1), 2<<16 ,DSI_DSI_PS_SEL);
        }
        else if (lcm_params->format == DSI_FORMAT_RGB888)
        {
            W4BYTEMSK(DSI_REG_BASE +(REG_DSI_PSCON>>1), 3<<16 ,DSI_DSI_PS_SEL);
        }
 #if 0
       #define LINE_PERIOD_US				(8 * line_byte * _dsiContext.bit_time_ns / 1000)

	OUTREG32(&DSI_REG->DSI_VSA_NL, lcm_params->dsi.vertical_sync_active);
	OUTREG32(&DSI_REG->DSI_VBP_NL, lcm_params->dsi.vertical_backporch);
	OUTREG32(&DSI_REG->DSI_VFP_NL, lcm_params->dsi.vertical_frontporch);
	OUTREG32(&DSI_REG->DSI_VACT_NL, lcm_params->dsi.vertical_active_line);

	line_byte							=	(lcm_params->dsi.horizontal_sync_active \
											+ lcm_params->dsi.horizontal_backporch \
											+ lcm_params->dsi.horizontal_frontporch \
											+ lcm_params->dsi.horizontal_active_pixel) * dsiTmpBufBpp;

	if (lcm_params->dsi.mode == SYNC_EVENT_VDO_MODE || lcm_params->dsi.mode == BURST_VDO_MODE ){
		ASSERT((lcm_params->dsi.horizontal_backporch + lcm_params->dsi.horizontal_sync_active) * dsiTmpBufBpp> 9);
		horizontal_backporch_byte		=	((lcm_params->dsi.horizontal_backporch + lcm_params->dsi.horizontal_sync_active)* dsiTmpBufBpp - 10);
	}
	else{
		ASSERT(lcm_params->dsi.horizontal_sync_active * dsiTmpBufBpp > 9);
		horizontal_sync_active_byte 		=	(lcm_params->dsi.horizontal_sync_active * dsiTmpBufBpp - 10);

		ASSERT(lcm_params->dsi.horizontal_backporch * dsiTmpBufBpp > 9);
		horizontal_backporch_byte		=	(lcm_params->dsi.horizontal_backporch * dsiTmpBufBpp - 10);
	}

	ASSERT(lcm_params->dsi.horizontal_frontporch * dsiTmpBufBpp > 11);
	horizontal_frontporch_byte			=	(lcm_params->dsi.horizontal_frontporch * dsiTmpBufBpp - 12);
	horizontal_bllp_byte				=	(lcm_params->dsi.horizontal_bllp * dsiTmpBufBpp);
//	ASSERT(lcm_params->dsi.horizontal_frontporch * dsiTmpBufBpp > ((300/dsi_cycle_time) * lcm_params->dsi.LANE_NUM));
//	horizontal_frontporch_byte -= ((300/dsi_cycle_time) * lcm_params->dsi.LANE_NUM);

	OUTREG32(&DSI_REG->DSI_HSA_WC, ALIGN_TO((horizontal_sync_active_byte), 4));
	OUTREG32(&DSI_REG->DSI_HBP_WC, ALIGN_TO((horizontal_backporch_byte), 4));
	OUTREG32(&DSI_REG->DSI_HFP_WC, ALIGN_TO((horizontal_frontporch_byte), 4));
	OUTREG32(&DSI_REG->DSI_BLLP_WC, ALIGN_TO((horizontal_bllp_byte), 4));

	_dsiContext.vfp_period_us 		= LINE_PERIOD_US * lcm_params->dsi.vertical_frontporch / 1000;
	_dsiContext.vsa_vs_period_us	= LINE_PERIOD_US * 1 / 1000;
	_dsiContext.vsa_hs_period_us	= LINE_PERIOD_US * (lcm_params->dsi.vertical_sync_active - 2) / 1000;
	_dsiContext.vsa_ve_period_us	= LINE_PERIOD_US * 1 / 1000;
	_dsiContext.vbp_period_us		= LINE_PERIOD_US * lcm_params->dsi.vertical_backporch / 1000;


	HAL_MIPI_DSI_DBG(1, "DSI [DISP] kernel - video timing, mode = %d \n", lcm_params->dsi.mode);
	HAL_MIPI_DSI_DBG(1, "DSI [DISP] kernel - VSA : %d %d(us)\n", DSI_REG->DSI_VSA_NL, (_dsiContext.vsa_vs_period_us+_dsiContext.vsa_hs_period_us+_dsiContext.vsa_ve_period_us));
	HAL_MIPI_DSI_DBG(1, "DSI [DISP] kernel - VBP : %d %d(us)\n", DSI_REG->DSI_VBP_NL, _dsiContext.vbp_period_us);
	HAL_MIPI_DSI_DBG(1, "DSI [DISP] kernel - VFP : %d %d(us)\n", DSI_REG->DSI_VFP_NL, _dsiContext.vfp_period_us);
	HAL_MIPI_DSI_DBG(1, "DSI [DISP] kernel - VACT: %d \n", DSI_REG->DSI_VACT_NL);
    #endif
    return DSI_STATUS_OK;
}

DSI_STATUS Hal_DSI_PHY_INIT(void)
{
    return DSI_STATUS_OK;
}


DSI_STATUS Hal_DSI_PHY_TIMCONFIG(LCM_PARAMS *lcm_params)
{

	DSI_PHY_TIMCON0_REG timcon0;
       //PDSI_PHY_TIMCON0_REG Ptimcon0;
	DSI_PHY_TIMCON1_REG timcon1;
       //PDSI_PHY_TIMCON1_REG Ptimcon1;
	DSI_PHY_TIMCON2_REG timcon2;
       //PDSI_PHY_TIMCON2_REG Ptimcon2;

	DSI_PHY_TIMCON3_REG timcon3;
       //PDSI_PHY_TIMCON3_REG Ptimcon3;
        u32 lane_no = lcm_params->LANE_NUM;
        //	unsigned int div2_real;
	u32 cycle_time;
	u32 ui;
	//u32 hs_trail_m, hs_trail_n;
	//u32 div1 = 0;
	//u32 div2 = 0;
	//u32 fbk_sel = 0;
	//u32 pre_div = 0;
	//u32 fbk_div = 0;

#if 0
//	div2_real=div2 ? div2*0x02 : 0x1;
	cycle_time = (8 * 1000 * div2 * div1 * pre_div)/ (26 * (fbk_div+0x01) * fbk_sel * 2);
	ui = (1000 * div2 * div1 * pre_div)/ (26 * (fbk_div+0x01) * fbk_sel * 2) + 1;

	//DISP_LOG_PRINT(ANDROID_LOG_INFO, "DSI", "[DISP] - kernel - DSI_PHY_TIMCONFIG, Cycle Time = %d(ns), Unit Interval = %d(ns). div1 = %d, div2 = %d, fbk_div = %d, lane# = %d \n", cycle_time, ui, div1, div2, fbk_div, lane_no);

	#define NS_TO_CYCLE(n, c)	((n) / c + (( (n) % c) ? 1 : 0))

	hs_trail_m=lane_no;
	hs_trail_n= (lcm_params->dsi.HS_TRAIL == 0) ? NS_TO_CYCLE(((lane_no * 4 * ui) + 60), cycle_time) : lcm_params->dsi.HS_TRAIL;

	// +3 is recommended from designer becauase of HW latency
	timcon0.HS_TRAIL	= ((hs_trail_m > hs_trail_n) ? hs_trail_m : hs_trail_n) + 0x0a;

	timcon0.HS_PRPR 	= (lcm_params->dsi.HS_PRPR == 0) ? NS_TO_CYCLE((60 + 5 * ui), cycle_time) : lcm_params->dsi.HS_PRPR;
	// HS_PRPR can't be 1.
	if (timcon0.HS_PRPR == 0)
		timcon0.HS_PRPR = 1;

	timcon0.HS_ZERO 	= (lcm_params->dsi.HS_ZERO == 0) ? NS_TO_CYCLE((0xC8 + 0x0a * ui - timcon0.HS_PRPR * cycle_time), cycle_time) : lcm_params->dsi.HS_ZERO;

	timcon0.LPX 		= (lcm_params->dsi.LPX == 0) ? NS_TO_CYCLE(65, cycle_time) : lcm_params->dsi.LPX;
	if(timcon0.LPX == 0) timcon0.LPX = 1;
//	timcon1.TA_SACK 	= (lcm_params->dsi.TA_SACK == 0) ? 1 : lcm_params->dsi.TA_SACK;
	timcon1.TA_GET 		= (lcm_params->dsi.TA_GET == 0) ? (5 * timcon0.LPX) : lcm_params->dsi.TA_GET;
	timcon1.TA_SURE 	= (lcm_params->dsi.TA_SURE == 0) ? (3 * timcon0.LPX / 2) : lcm_params->dsi.TA_SURE;
	timcon1.TA_GO 		= (lcm_params->dsi.TA_GO == 0) ? (4 * timcon0.LPX) : lcm_params->dsi.TA_GO;
	timcon1.DA_HS_EXIT  = (lcm_params->dsi.DA_HS_EXIT == 0) ? (2 * timcon0.LPX) : lcm_params->dsi.DA_HS_EXIT;

	timcon2.CLK_TRAIL 	= ((lcm_params->dsi.CLK_TRAIL == 0) ? NS_TO_CYCLE(0x64, cycle_time) : lcm_params->dsi.CLK_TRAIL) + 0x0a;
	// CLK_TRAIL can't be 1.
	if (timcon2.CLK_TRAIL < 2)
		timcon2.CLK_TRAIL = 2;

//	timcon2.LPX_WAIT 	= (lcm_params->dsi.LPX_WAIT == 0) ? 1 : lcm_params->dsi.LPX_WAIT;
	timcon2.CONT_DET 	= lcm_params->dsi.CONT_DET;

	timcon3.CLK_HS_PRPR	= (lcm_params->dsi.CLK_HS_PRPR == 0) ? NS_TO_CYCLE(0x40, cycle_time) : lcm_params->dsi.CLK_HS_PRPR;
	if(timcon3.CLK_HS_PRPR == 0) timcon3.CLK_HS_PRPR = 1;

	timcon2.CLK_ZERO	= (lcm_params->dsi.CLK_ZERO == 0) ? NS_TO_CYCLE(0x190 - timcon3.CLK_HS_PRPR * cycle_time, cycle_time) : lcm_params->dsi.CLK_ZERO;

	timcon3.CLK_HS_EXIT= (lcm_params->dsi.CLK_HS_EXIT == 0) ? (2 * timcon0.LPX) : lcm_params->dsi.CLK_HS_EXIT;

	timcon3.CLK_HS_POST= (lcm_params->dsi.CLK_HS_POST == 0) ? NS_TO_CYCLE((80 + 52 * ui), cycle_time) : lcm_params->dsi.CLK_HS_POST;

	DISP_LOG_PRINT(ANDROID_LOG_INFO, "DSI", "[DISP] - kernel - DSI_PHY_TIMCONFIG, HS_TRAIL = %d, HS_ZERO = %d, HS_PRPR = %d, LPX = %d, TA_GET = %d, TA_SURE = %d, TA_GO = %d, CLK_TRAIL = %d, CLK_ZERO = %d, CLK_HS_PRPR = %d \n", \
			timcon0.HS_TRAIL, timcon0.HS_ZERO, timcon0.HS_PRPR, timcon0.LPX, timcon1.TA_GET, timcon1.TA_SURE, timcon1.TA_GO, timcon2.CLK_TRAIL, timcon2.CLK_ZERO, timcon3.CLK_HS_PRPR);

	//DSI_REG->DSI_PHY_TIMECON0=timcon0;
	//DSI_REG->DSI_PHY_TIMECON1=timcon1;
	//DSI_REG->DSI_PHY_TIMECON2=timcon2;
	//DSI_REG->DSI_PHY_TIMECON3=timcon3;
	OUTREG32(&DSI_REG->DSI_PHY_TIMECON0,AS_UINT32(&timcon0));
	OUTREG32(&DSI_REG->DSI_PHY_TIMECON1,AS_UINT32(&timcon1));
	OUTREG32(&DSI_REG->DSI_PHY_TIMECON2,AS_UINT32(&timcon2));
	OUTREG32(&DSI_REG->DSI_PHY_TIMECON3,AS_UINT32(&timcon3));
	dsi_cycle_time = cycle_time;
    #else
        //	div2_real=div2 ? div2*0x02 : 0x1;

        #if 0
	    cycle_time = (8 * 1000 * div2 * div1 * pre_div)/ (26 * (fbk_div+0x01) * fbk_sel * 2);
	    ui = (1000 * div2 * div1 * pre_div)/ (26 * (fbk_div+0x01) * fbk_sel * 2) + 1;
        #else

        #endif

    Hal_DSI_LPLL(lcm_params);
	//DISP_LOG_PRINT(ANDROID_LOG_INFO, "DSI", "[DISP] - kernel - DSI_PHY_TIMCONFIG, Cycle Time = %d(ns), Unit Interval = %d(ns). div1 = %d, div2 = %d, fbk_div = %d, lane# = %d \n", cycle_time, ui, div1, div2, fbk_div, lane_no);

	#define NS_TO_CYCLE(n, c)	((n) / c + (( (n) % c) ? 1 : 0))

       #if 0  //TBD
	hs_trail_m=lane_no;
	hs_trail_n= (lcm_params->HS_TRAIL == 0) ? NS_TO_CYCLE(((lane_no * 4 * ui) + 60), cycle_time) : lcm_params->HS_TRAIL;

	// +3 is recommended from designer becauase of HW latency
	timcon0.HS_TRAIL	= ((hs_trail_m > hs_trail_n) ? hs_trail_m : hs_trail_n) + 0x0a;
       #else
	timcon0.HS_TRAIL	=(lcm_params->HS_TRAIL == 0) ? NS_TO_CYCLE(((lane_no * 4 * ui) + 60), cycle_time) : lcm_params->HS_TRAIL;
	#endif
	timcon0.HS_PRPR 	= (lcm_params->HS_PRPR == 0) ? NS_TO_CYCLE((60 + 5 * ui), cycle_time) : lcm_params->HS_PRPR;

       // HS_PRPR can't be 1.
	if (timcon0.HS_PRPR == 0)
		timcon0.HS_PRPR = 1;

	timcon0.HS_ZERO 	= (lcm_params->HS_ZERO == 0) ? NS_TO_CYCLE((0xC8 + 0x0a * ui - timcon0.HS_PRPR * cycle_time), cycle_time) : lcm_params->HS_ZERO;

	timcon0.LPX 		= (lcm_params->LPX == 0) ? NS_TO_CYCLE(65, cycle_time) : lcm_params->LPX;
	if(timcon0.LPX == 0) timcon0.LPX = 1;

       //timcon1.TA_SACK 	= (lcm_params->dsi.TA_SACK == 0) ? 1 : lcm_params->dsi.TA_SACK;
	timcon1.TA_GET 		= (lcm_params->TA_GET == 0) ? (5 * timcon0.LPX) : lcm_params->TA_GET;
	timcon1.TA_SURE 	= (lcm_params->TA_SURE == 0) ? (3 * timcon0.LPX / 2) : lcm_params->TA_SURE;
	timcon1.TA_GO 		= (lcm_params->TA_GO == 0) ? (4 * timcon0.LPX) : lcm_params->TA_GO;
	timcon1.DA_HS_EXIT  = (lcm_params->DA_HS_EXIT == 0) ? (2 * timcon0.LPX) : lcm_params->DA_HS_EXIT;

	timcon2.CLK_TRAIL 	= ((lcm_params->CLK_TRAIL == 0) ? NS_TO_CYCLE(0x64, cycle_time) : lcm_params->CLK_TRAIL) + 0x0a;
	// CLK_TRAIL can't be 1.
	if (timcon2.CLK_TRAIL < 2)
		timcon2.CLK_TRAIL = 2;

//	timcon2.LPX_WAIT 	= (lcm_params->dsi.LPX_WAIT == 0) ? 1 : lcm_params->dsi.LPX_WAIT;
	timcon2.CONT_DET 	= lcm_params->CONT_DET;

	timcon3.CLK_HS_PRPR	= (lcm_params->CLK_HS_PRPR == 0) ? NS_TO_CYCLE(0x40, cycle_time) : lcm_params->CLK_HS_PRPR;

        if(timcon3.CLK_HS_PRPR == 0)
            timcon3.CLK_HS_PRPR = 1;

        timcon2.CLK_ZERO	= (lcm_params->CLK_ZERO == 0) ? NS_TO_CYCLE(0x190 - timcon3.CLK_HS_PRPR * cycle_time, cycle_time) : lcm_params->CLK_ZERO;
        timcon2.RSV8 =0x01;
        timcon3.CLK_HS_EXIT= (lcm_params->CLK_HS_EXIT == 0) ? (2 * timcon0.LPX) : lcm_params->CLK_HS_EXIT;

        timcon3.CLK_HS_POST= (lcm_params->CLK_HS_POST == 0) ? NS_TO_CYCLE((80 + 52 * ui), cycle_time) : lcm_params->CLK_HS_POST;

        HAL_MIPI_DSI_DBG(1, "[DISP] - kernel - DSI_PHY_TIMCONFIG, HS_TRAIL = %d, HS_ZERO = %d, HS_PRPR = %d, LPX = %d, TA_GET = %d, TA_SURE = %d, TA_GO = %d, CLK_TRAIL = %d, CLK_ZERO = %d, CLK_HS_PRPR = %d \n", \
        	timcon0.HS_TRAIL, timcon0.HS_ZERO, timcon0.HS_PRPR, timcon0.LPX, timcon1.TA_GET, timcon1.TA_SURE, timcon1.TA_GO, timcon2.CLK_TRAIL, timcon2.CLK_ZERO, timcon3.CLK_HS_PRPR);
         W4BYTE(DSI_REG_BASE + (REG_DSI_PHY_TIMCON0>>1)  ,  AS_UINT32(&timcon0));
         W4BYTE(DSI_REG_BASE + (REG_DSI_PHY_TIMCON1>>1)  ,  AS_UINT32(&timcon1));
        //HAL_MIPI_DSI_DBG( "[MIPI DSI] %s,  %d,    %x \n", __FUNCTION__, __LINE__, R4BYTE(DSI_REG_BASE+(REG_DSI_PHY_TIMCON2>>1)));
        //HAL_MIPI_DSI_DBG("AS_UINT32(&timcon2) =%x \n", AS_UINT32(&timcon2) );
        //Ptimcon2=(PDSI_PHY_TIMCON2_REG) (CHIP_BASE(u32, DSI_REG_BASE_2, REG_DSI_PHY_TIMCON2, 0));
         W4BYTE(DSI_REG_BASE + (REG_DSI_PHY_TIMCON2>>1)  ,  AS_UINT32(&timcon2));
        W4BYTE(DSI_REG_BASE + (REG_DSI_PHY_TIMCON3>>1)  ,  AS_UINT32(&timcon3));
         #if 0
        W4BYTE(DSI_REG_BASE +(REG_DSI_PHY_TIMCON0>>1), (u32) timcon0);
        W4BYTE(DSI_REG_BASE +(REG_DSI_PHY_TIMCON1>>1), (u32) timcon1);
        W4BYTE(DSI_REG_BASE +(REG_DSI_PHY_TIMCON2>>1), (u32) timcon2);
        W4BYTE(DSI_REG_BASE +(REG_DSI_PHY_TIMCON3>>1), (u32) timcon3);
         #endif
        //dsi_cycle_time = cycle_time;

    #endif
    return DSI_STATUS_OK;
}
//-------------------------------------------------------------------------------------------------
//  Global setting
//-------------------------------------------------------------------------------------------------

