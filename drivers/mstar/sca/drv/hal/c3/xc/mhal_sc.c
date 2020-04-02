////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2008-2009 MStar Semiconductor, Inc.
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
////////////////////////////////////////////////////////////////////////////////
//==============================================================================
// [mhal_sc.c]
// Date: 20081203
// Descriptions: Add a new layer for HW setting
//==============================================================================
#define  MHAL_SC_C

#ifdef MSOS_TYPE_LINUX_KERNEL
#include <linux/wait.h>
#include <linux/irqreturn.h>
#include <asm/div64.h>
#else
#define do_div(x,y) ((x)/=(y))
#endif

// Common Definition
#include "MsCommon.h"
#include "MsIRQ.h"
#include "MsOS.h"
#include "mhal_xc_chip_config.h"

#include "xc_hwreg_utility2.h"
#include "xc_Analog_Reg.h"
#include "drvXC_IOPort.h"
#include "apiXC.h"

#include "drv_sc_ip.h"
#include "mvideo_context.h"
#include "mhal_sc.h"
#include "mhal_menuload.h"
#include "halCHIP.h"
#include "drv_sc_menuload.h"
#include "drv_sc_isr.h"


#define SC_DBG(x)   //x
#define FPLL_DBG(x) //x

#define new_chakra

//==============================================================================
//==============================================================================
//MS_U32 _XC_RIU_BASE;
//MS_U32 _HDCP_RIU_BASE;

MS_U32 _DDC_RIU_BASE;
MS_U32 _PM_RIU_BASE;


MS_U32 _DVI_ATOP_RIU_BASE;
MS_U32 _DVI_DTOP_RIU_BASE;
MS_U32 _DVI_EQ_RIU_BASE;
MS_U32 _ADC_DTOPB_RIU_BASE;


MS_U32 _PM_SLEEP_RIU_BASE;

extern PQ_Function_Info    s_PQ_Function_Info;
extern MS_BOOL             s_bKeepPixelPointerAppear;
static SC_MIUMASK_t Miu0Mask, Miu1Mask;
static SC_MIUMASK_t Miu0MaskOld, Miu1MaskOld;
static MS_BOOL bDynamicScalingEnable = 0;

// Put this function here because hwreg_utility2 only for hal.
void Hal_SC_init_riu_base(MS_U32 u32riu_base, MS_U32 u32PMriu_base)
{
    _XC_RIU_BASE = u32PMriu_base;
    _PM_RIU_BASE = u32PMriu_base;

    _DVI_ATOP_RIU_BASE = u32PMriu_base;
    _DVI_DTOP_RIU_BASE = u32PMriu_base;
    _HDCP_RIU_BASE = u32PMriu_base;
    _DVI_EQ_RIU_BASE = u32PMriu_base;
    _ADC_DTOPB_RIU_BASE = u32PMriu_base;

    _DDC_RIU_BASE =u32PMriu_base;
    _PM_SLEEP_RIU_BASE =u32PMriu_base;

}

//=========================================================//
// Function : Hal_SC_setfield
// Description:
//=========================================================//
void Hal_SC_setfield( MS_U16 reg_1D, MS_U16 reg_21, MS_U16 reg_23, SCALER_WIN eWindow )
{
    //printf("reg_1D=%x, reg_21=%x, reg_23=%x\n", reg_1D, reg_21, reg_23);

    if( eWindow == MAIN_WINDOW )
    {
        SC_W2BYTEMSK(REG_SC_BK01_1D_L, (reg_1D<<8), 0xEF00);
        SC_W2BYTEMSK(REG_SC_BK01_21_L, reg_21, 0x3FFF);
        SC_W2BYTEMSK(REG_SC_BK01_23_L, (reg_23<<8), HBMASK);
    }
    else if ( eWindow == SUB_WINDOW )
    {
        SC_W2BYTEMSK(REG_SC_BK03_1D_L, (reg_1D<<8), 0xEF00);
        SC_W2BYTEMSK(REG_SC_BK03_21_L, reg_21, 0x3FFF);
        SC_W2BYTEMSK(REG_SC_BK03_23_L, (reg_23<<8), HBMASK);
    }
    else if( eWindow == SC1_MAIN_WINDOW)
    {
        SC_W2BYTEMSK(REG_SC1_BK01_1D_L, (reg_1D<<8), 0xEF00);
        SC_W2BYTEMSK(REG_SC1_BK01_21_L, reg_21, 0x3FFF);
        SC_W2BYTEMSK(REG_SC1_BK01_23_L, (reg_23<<8), HBMASK);
    }
    else if( eWindow == SC2_MAIN_WINDOW)
    {
        SC_W2BYTEMSK(REG_SC2_BK01_1D_L, (reg_1D<<8), 0xEF00);
        SC_W2BYTEMSK(REG_SC2_BK01_21_L, reg_21, 0x3FFF);
        SC_W2BYTEMSK(REG_SC2_BK01_23_L, (reg_23<<8), HBMASK);
    }
    else if ( eWindow == SC2_SUB_WINDOW )
    {
        SC_W2BYTEMSK(REG_SC2_BK03_1D_L, (reg_1D<<8), 0xEF00);
        SC_W2BYTEMSK(REG_SC2_BK03_21_L, reg_21, 0x3FFF);
        SC_W2BYTEMSK(REG_SC2_BK03_23_L, (reg_23<<8), HBMASK);
    }
    else
    {

    }

//    printf("%x ",SC_R2BYTEMSK(REG_SC_BK01_1D_L, HBMASK));
//    printf("%x ",SC_R2BYTEMSK(REG_SC_BK01_21_L, 0x3FFF));
//    printf("%x ",SC_R2BYTEMSK(REG_SC_BK01_23_L, HBMASK));

//    printf("%x ",SC_R4BYTE(REG_SC_BK01_21_L));
//    printf("%x ",SC_R4BYTE(REG_SC_BK01_23_L));
}

//=========================================================//
// Function : Hal_SC_de_only_en
// Description: DE Only. HSYNC and VSYNC are ignored.
//#0: Disable.
//#1: Enable.
//=========================================================//
void Hal_SC_de_only_en(MS_BOOL benable, SCALER_WIN eWindow)
{
    if(eWindow == MAIN_WINDOW)
    {
        SC_W2BYTEMSK(REG_SC_BK01_03_L, (benable ? BIT(6):0), BIT(6));
    }
    else if ( eWindow == SUB_WINDOW )
    {
        SC_W2BYTEMSK(REG_SC_BK03_03_L, (benable ? BIT(6):0), BIT(6));
    }
    else if ( eWindow == SC1_MAIN_WINDOW)
    {
        SC_W2BYTEMSK(REG_SC1_BK01_03_L, (benable ? BIT(6):0), BIT(6));
    }
    else if ( eWindow == SC2_MAIN_WINDOW)
    {
        SC_W2BYTEMSK(REG_SC2_BK01_03_L, (benable ? BIT(6):0), BIT(6));
    }
    else if ( eWindow == SC2_SUB_WINDOW)
    {
        SC_W2BYTEMSK(REG_SC2_BK03_03_L, (benable ? BIT(6):0), BIT(6));
    }
    else
    {
    }
}

//=========================================================//
// Function : Hal_SC_de_direct_en
// Description: Digital Input Horizontal Sample Range
//#0: Use DE as sample range, only V position can be adjusted.
//#1: Use SPRHST and SPRHDC as sample range, both H and V position can be adjusted.
//=========================================================//
void Hal_SC_de_direct_en(MS_BOOL benable, SCALER_WIN eWindow)
{
    if(eWindow == MAIN_WINDOW)
    {
        SC_W2BYTEMSK(REG_SC_BK01_03_L, (benable ? BIT(7):0), BIT(7));
    }
    else if ( eWindow == SUB_WINDOW )
    {
        SC_W2BYTEMSK(REG_SC_BK03_03_L, (benable ? BIT(7):0), BIT(7));
    }
    else if ( eWindow == SC1_MAIN_WINDOW)
    {
        SC_W2BYTEMSK(REG_SC1_BK01_03_L, (benable ? BIT(7):0), BIT(7));
    }
    else if ( eWindow == SC2_MAIN_WINDOW)
    {
        SC_W2BYTEMSK(REG_SC2_BK01_03_L, (benable ? BIT(7):0), BIT(7));
    }
    else if ( eWindow == SC2_SUB_WINDOW)
    {
        SC_W2BYTEMSK(REG_SC2_BK03_03_L, (benable ? BIT(7):0), BIT(7));
    }
    else
    {
    }
}

void Hal_SC_set_ficlk(MS_BOOL bPreDown, SCALER_WIN eWindow)
{
    if( eWindow == MAIN_WINDOW )
    {
        if (bPreDown)
        {
            W2BYTEMSK(REG_CLKGEN0_51_L, CLK_SRC_IDCLK2<<10, (BIT(11)|BIT(10))); // clk_idclk2
        }
        else
        {
            W2BYTEMSK(REG_CLKGEN0_51_L, CLK_SRC_FCLK<<10, (BIT(11)|BIT(10))); // clk_fclk
        }
    }
    else if ( eWindow == SUB_WINDOW )
    {
        if (bPreDown)
        {
            W2BYTEMSK(REG_CLKGEN0_51_L, CLK_SRC_IDCLK2<<2, (BIT(3)|BIT(2))); // clk_idclk2
        }
        else
        {
            W2BYTEMSK(REG_CLKGEN0_51_L, CLK_SRC_FCLK<<2, (BIT(3)|BIT(2))); // clk_fclk
        }
    }
    else if ( eWindow == SC1_MAIN_WINDOW)
    {
        //!!ToDo!!
    }
    else if ( eWindow == SC2_MAIN_WINDOW)
    {
        //!!ToDo!!
    }
    else if ( eWindow == SC2_SUB_WINDOW)
    {
        //!!ToDo!!
    }
    else
    {
    }

}

void Hal_SC_set_shiftline( MS_U8 u8Val, SCALER_WIN eWindow )
{
    if( eWindow == MAIN_WINDOW )
    {
        SC_W2BYTEMSK(REG_SC_BK10_2F_L, (u8Val & 0x07)<<8, (BIT(10)|BIT(9)|BIT(8)));
    }
    else if ( eWindow == SUB_WINDOW )
    {
        SC_W2BYTEMSK(REG_SC_BK10_2F_L, (u8Val & 0x03)<<13, (BIT(14)|BIT(13)));
    }
    else if ( eWindow == SC1_MAIN_WINDOW)
    {
        SC_W2BYTEMSK(REG_SC1_BK10_2F_L, (u8Val & 0x07)<<8, (BIT(10)|BIT(9)|BIT(8)));
    }
    else if ( eWindow == SC2_MAIN_WINDOW)
    {
        SC_W2BYTEMSK(REG_SC2_BK10_2F_L, (u8Val & 0x07)<<8, (BIT(10)|BIT(9)|BIT(8)));
    }
    else if ( eWindow == SC2_SUB_WINDOW)
    {
        SC_W2BYTEMSK(REG_SC2_BK10_2F_L, (u8Val & 0x03)<<13, (BIT(14)|BIT(13)));
    }
    else
    {
    }

}

void Hal_SC_set_422_cbcr_swap(MS_BOOL bEnable, SCALER_WIN eWindow)
{
    if(eWindow == MAIN_WINDOW)
    {
        SC_W2BYTEMSK(REG_SC_BK02_0A_L, bEnable ? BIT(6) : 0, BIT(6));
    }
    else if ( eWindow == SUB_WINDOW )
    {
        SC_W2BYTEMSK(REG_SC_BK04_0A_L, bEnable ? BIT(6) : 0, BIT(6));
    }
    else if ( eWindow == SC1_MAIN_WINDOW)
    {
        SC_W2BYTEMSK(REG_SC1_BK02_0A_L, bEnable ? BIT(6) : 0, BIT(6));
    }
    else if ( eWindow == SC2_MAIN_WINDOW)
    {
        SC_W2BYTEMSK(REG_SC2_BK02_0A_L, bEnable ? BIT(6) : 0, BIT(6));
    }
    else if ( eWindow == SC2_SUB_WINDOW)
    {
        SC_W2BYTEMSK(REG_SC2_BK04_0A_L, bEnable ? BIT(6) : 0, BIT(6));
    }
    else
    {
    }

}

void Hal_SC_set_pre_align_pixel(MS_BOOL bEnable, MS_U16 pixels, SCALER_WIN eWindow)
{
    if (bEnable)
    {
        if ( eWindow == MAIN_WINDOW )
        {
            // Enable pre align pixel for mirror mode.
            SC_W2BYTEMSK(REG_SC_BK02_2A_L, BIT(15), BIT(15));

            // Number of pixels need to be inserted. (U3 has 15 pixels)
            SC_W2BYTEMSK(REG_SC_BK02_2A_L, pixels << 8 , 0x1F00);
        }
        else if ( eWindow == SUB_WINDOW )
        {
            // Enable pre align pixel for mirror mode.
            SC_W2BYTEMSK(REG_SC_BK04_2A_L, BIT(15), BIT(15));

            // Number of pixels need to be inserted. (U3 has 15 pixels)
            SC_W2BYTEMSK(REG_SC_BK04_2A_L, pixels << 8 , 0x1F00);
        }
        else if ( eWindow == SC1_MAIN_WINDOW )
        {
            SC_W2BYTEMSK(REG_SC1_BK02_2A_L, BIT(15), BIT(15));
            SC_W2BYTEMSK(REG_SC1_BK02_2A_L, pixels << 8 , 0x1F00);
        }
        else if ( eWindow == SC2_MAIN_WINDOW )
        {
            SC_W2BYTEMSK(REG_SC2_BK02_2A_L, BIT(15), BIT(15));
            SC_W2BYTEMSK(REG_SC2_BK02_2A_L, pixels << 8 , 0x1F00);
        }
        else if ( eWindow == SC2_SUB_WINDOW )
        {
            SC_W2BYTEMSK(REG_SC2_BK04_2A_L, BIT(15), BIT(15));
            SC_W2BYTEMSK(REG_SC2_BK04_2A_L, pixels << 8 , 0x1F00);
        }
        else
        {
        }

    }
    else
    {
        if ( eWindow == MAIN_WINDOW )
        {
            // Disable pre align pixel for mirror mode.
            SC_W2BYTEMSK(REG_SC_BK02_2A_L, 0x00 , BIT(15));
        }
        else if ( eWindow == SUB_WINDOW )
        {
            // Disable pre align pixel for mirror mode.
            SC_W2BYTEMSK(REG_SC_BK04_2A_L, 0x00 , BIT(15));
        }
        else if ( eWindow == SC1_MAIN_WINDOW )
        {
            SC_W2BYTEMSK(REG_SC1_BK02_2A_L, 0x00 , BIT(15));
        }
        else if ( eWindow == SC2_MAIN_WINDOW )
        {
            SC_W2BYTEMSK(REG_SC2_BK02_2A_L, 0x00 , BIT(15));
        }
        else if ( eWindow == SC2_SUB_WINDOW )
        {
            SC_W2BYTEMSK(REG_SC2_BK04_2A_L, 0x00 , BIT(15));
        }
        else
        {
        }

    }
}

void Hal_XC_Set_FreeFRCMD(MS_BOOL bEnable)
{
    SC_W2BYTEMSK(REG_SC_BK12_05_L, bEnable? BIT(5) : 0, BIT(5));
}

// Obsolete in M10/J2/A5
void Hal_SC_set_linearmem_mode(MS_BOOL bEnable, SCALER_WIN eWindow)
{
}

// This function will return 8/10/12/14/16 field mode or 8 frame mode.
// Otherwise it return IMAGE_STORE_2_FRAMES
XC_FRAME_STORE_NUMBER Hal_SC_GetFrameStoreMode(SCALER_WIN eWindow)
{
    MS_U16 u16FrameCount = 0x00;
    MS_BOOL bInterlace = gSrcInfo[eWindow].bInterlace;

	XC_FRAME_STORE_NUMBER	eFrameStoreNum = IMAGE_STORE_UNDEFINE;

    if (eWindow == MAIN_WINDOW)
    {
        u16FrameCount = 0x1F & SC_R2BYTE(REG_SC_BK12_19_L);
    }
    else if (eWindow == SUB_WINDOW)
    {
        u16FrameCount = 0x1F & SC_R2BYTE(REG_SC_BK12_59_L);
    }
    else if (eWindow == SC1_MAIN_WINDOW)
    {
        u16FrameCount = 0x1F & SC_R2BYTE(REG_SC1_BK12_19_L);
    }
    else if (eWindow == SC2_MAIN_WINDOW)
    {
        u16FrameCount = 0x1F & SC_R2BYTE(REG_SC2_BK12_19_L);
    }
    else if (eWindow == SC2_SUB_WINDOW)
    {
        u16FrameCount = 0x1F & SC_R2BYTE(REG_SC2_BK12_59_L);
    }
    else
    {
        u16FrameCount = 0x00;
    }

    switch( u16FrameCount) // Get field type.
    {
        default:
            if (bInterlace)
            {
                eFrameStoreNum = IMAGE_STORE_4_FIELDS;
            }
            else
            {
                eFrameStoreNum = IMAGE_STORE_2_FRAMES;
            }
			break;

        case 3:
            eFrameStoreNum = IMAGE_STORE_3_FRAMES;
			break;

        case 4:
            if (bInterlace)
            {
                eFrameStoreNum = IMAGE_STORE_4_FIELDS;
            }
            else
            {
                eFrameStoreNum = IMAGE_STORE_4_FRAMES;
            }
			break;

        case 6:
            if (bInterlace)
            {
                eFrameStoreNum = IMAGE_STORE_6_FIELDS;
            }
            else
            {
                eFrameStoreNum = IMAGE_STORE_6_FRAMES;
            }
			break;

        case 8:
            if (bInterlace)
            {
                eFrameStoreNum = IMAGE_STORE_8_FIELDS;
            }
            else
            {
                eFrameStoreNum = IMAGE_STORE_8_FRAMES;
            }
			break;

        case 10:
            eFrameStoreNum = IMAGE_STORE_10_FIELDS;
			break;

		case 12:
            if (bInterlace)
            {
                eFrameStoreNum = IMAGE_STORE_12_FIELDS;
            }
            else
            {
                eFrameStoreNum = IMAGE_STORE_12_FRAMES;
            }
			break;

        case 14:
            eFrameStoreNum = IMAGE_STORE_14_FIELDS;
			break;

        case 16:
            eFrameStoreNum = IMAGE_STORE_16_FIELDS;
			break;
    }

    return eFrameStoreNum;
}

void Hal_SC_set_wr_bank_mapping(MS_U8 u8val, SCALER_WIN eWindow)
{
    SC_DBG(printf("Set WR bank mapping SC_BK12_07/47_L[15:13]= 0x%x\r\n", ((MS_U16)u8val)<<13));

    if( eWindow == MAIN_WINDOW )
    {
        SC_W2BYTEMSK(REG_SC_BK12_07_L, ((MS_U16)u8val)<<13, BIT(15)|BIT(14)|BIT(13));
    }
    else if( eWindow == SUB_WINDOW )
    {
        SC_W2BYTEMSK(REG_SC_BK12_47_L, ((MS_U16)u8val)<<13, BIT(15)|BIT(14)|BIT(13));
    }
    else if( eWindow == SC1_MAIN_WINDOW )
    {
        SC_W2BYTEMSK(REG_SC1_BK12_07_L, ((MS_U16)u8val)<<13, BIT(15)|BIT(14)|BIT(13));
    }
    else if( eWindow == SC2_MAIN_WINDOW )
    {
        SC_W2BYTEMSK(REG_SC2_BK12_07_L, ((MS_U16)u8val)<<13, BIT(15)|BIT(14)|BIT(13));
    }
    else if( eWindow == SC2_SUB_WINDOW )
    {
        SC_W2BYTEMSK(REG_SC2_BK12_47_L, ((MS_U16)u8val)<<13, BIT(15)|BIT(14)|BIT(13));
    }
    else
    {
    }
}

void Hal_SC_set_csc( MS_BOOL bEnable, SCALER_WIN eWindow )
{
    if( eWindow == MAIN_WINDOW )
    {
        SC_W2BYTEMSK(REG_SC_BK02_40_L, (bEnable ? BIT(3):0), BIT(3));
        SC_W2BYTEMSK(REG_SC_BK02_01_L, (bEnable ? BIT(2):0), BIT(2));
    }
    else if( eWindow == SUB_WINDOW )
    {
        SC_W2BYTEMSK(REG_SC_BK04_40_L, (bEnable ? BIT(3):0), BIT(3));
        SC_W2BYTEMSK(REG_SC_BK04_01_L, (bEnable ? BIT(2):0), BIT(2));
    }
    else if( eWindow == SC1_MAIN_WINDOW )
    {
        SC_W2BYTEMSK(REG_SC1_BK02_40_L, (bEnable ? BIT(3):0), BIT(3));
        SC_W2BYTEMSK(REG_SC1_BK02_01_L, (bEnable ? BIT(2):0), BIT(2));
    }
    else if( eWindow == SC2_MAIN_WINDOW )
    {
        SC_W2BYTEMSK(REG_SC2_BK02_40_L, (bEnable ? BIT(3):0), BIT(3));
        SC_W2BYTEMSK(REG_SC2_BK02_01_L, (bEnable ? BIT(2):0), BIT(2));
    }
    else if( eWindow == SC2_SUB_WINDOW )
    {
        SC_W2BYTEMSK(REG_SC2_BK04_40_L, (bEnable ? BIT(3):0), BIT(3));
        SC_W2BYTEMSK(REG_SC2_BK04_01_L, (bEnable ? BIT(2):0), BIT(2));
    }
    else
    {
    }
}

MS_BOOL Hal_SC_get_csc(SCALER_WIN eWindow)
{
    MS_BOOL bYUVInput;

    if(eWindow == MAIN_WINDOW)
    {
        // If it has CSC, it shows it must be RGB input.
        // Because in scaler, it must be YUV. So if input is RGB, then we need to do CSC
        bYUVInput = ( SC_R2BYTEMSK(REG_SC_BK02_40_L, BIT(3) ) ||
                                SC_R2BYTEMSK(REG_SC_BK18_6E_L, BIT(0) ) )? FALSE:TRUE;
    }
    else if( eWindow == SUB_WINDOW )
    {
        bYUVInput = ( SC_R2BYTEMSK(REG_SC_BK04_40_L, BIT(3) ) ||
                                SC_R2BYTEMSK(REG_SC_BK18_6E_L, BIT(4) ) )? FALSE:TRUE;
    }
    else if( eWindow == SC1_MAIN_WINDOW )
    {
        bYUVInput = ( SC_R2BYTEMSK(REG_SC1_BK02_40_L, BIT(3) ) ||
                                SC_R2BYTEMSK(REG_SC1_BK18_6E_L, BIT(0) ) )? FALSE:TRUE;
    }
    else if( eWindow == SC2_MAIN_WINDOW )
    {
        bYUVInput = ( SC_R2BYTEMSK(REG_SC2_BK02_40_L, BIT(3) ) ||
                                SC_R2BYTEMSK(REG_SC2_BK18_6E_L, BIT(0) ) )? FALSE:TRUE;
    }
    else if( eWindow == SC2_SUB_WINDOW )
    {
        bYUVInput = ( SC_R2BYTEMSK(REG_SC2_BK04_40_L, BIT(3) ) ||
                                SC_R2BYTEMSK(REG_SC2_BK18_6E_L, BIT(4) ) )? FALSE:TRUE;
    }
    else
    {
        bYUVInput = FALSE;
    }

    return bYUVInput;
}

void Hal_SC_set_delayline( MS_U8 u8DelayLines, SCALER_WIN eWindow )
{
    //skip bit7, which is other usage
    MS_U16 u16DelayLines = ((MS_U16)u8DelayLines) & 0xFF;
    if( eWindow == MAIN_WINDOW )
    {
        SC_W2BYTEMSK(REG_SC_BK01_1C_L, (u16DelayLines<<4), (BIT(6)|BIT(5)|BIT(4)));
        SC_W2BYTEMSK(REG_SC_BK01_1C_L, (u16DelayLines<<5), (BIT(9)|BIT(8)));
    }
    else if( eWindow == SUB_WINDOW )
    {
        SC_W2BYTEMSK(REG_SC_BK03_1C_L, (u16DelayLines<<4), (BIT(6)|BIT(5)|BIT(4)));
        SC_W2BYTEMSK(REG_SC_BK03_1C_L, (u16DelayLines<<5), (BIT(9)|BIT(8)));
    }
    else if( eWindow == SC1_MAIN_WINDOW )
    {
        SC_W2BYTEMSK(REG_SC1_BK01_1C_L, (u16DelayLines<<4), (BIT(6)|BIT(5)|BIT(4)));
        SC_W2BYTEMSK(REG_SC1_BK01_1C_L, (u16DelayLines<<5), (BIT(9)|BIT(8)));
    }
    else if( eWindow == SC2_MAIN_WINDOW )
    {
        SC_W2BYTEMSK(REG_SC2_BK01_1C_L, (u16DelayLines<<4), (BIT(6)|BIT(5)|BIT(4)));
        SC_W2BYTEMSK(REG_SC2_BK01_1C_L, (u16DelayLines<<5), (BIT(9)|BIT(8)));
    }
    else if( eWindow == SC2_SUB_WINDOW )
    {
        SC_W2BYTEMSK(REG_SC2_BK03_1C_L, (u16DelayLines<<4), (BIT(6)|BIT(5)|BIT(4)));
        SC_W2BYTEMSK(REG_SC2_BK03_1C_L, (u16DelayLines<<5), (BIT(9)|BIT(8)));
    }
    else
    {
    }

}

void Hal_SC_set_write_limit(MS_U32 u32WritelimitAddrBase, SCALER_WIN eWindow)
{
    if(eWindow == MAIN_WINDOW)
    {
        SC_W4BYTE(REG_SC_BK12_1A_L, u32WritelimitAddrBase);
    }
    else if( eWindow == SUB_WINDOW )
    {
        SC_W4BYTE(REG_SC_BK12_5A_L, u32WritelimitAddrBase);
    }
    else if( eWindow == SC1_MAIN_WINDOW )
    {
        SC_W4BYTE(REG_SC1_BK12_1A_L, u32WritelimitAddrBase);
    }
    else if( eWindow == SC2_MAIN_WINDOW )
    {
        SC_W4BYTE(REG_SC2_BK12_1A_L, u32WritelimitAddrBase);
    }
    else if( eWindow == SC2_SUB_WINDOW )
    {
        SC_W4BYTE(REG_SC2_BK12_5A_L, u32WritelimitAddrBase);
    }
    else
    {
    }

}

void Hal_SC_set_opm_write_limit(MS_BOOL bEnable, MS_BOOL bFlag, MS_U32 u32OPWlimitAddr, SCALER_WIN eWindow)
{
    MS_U16 u16temp = 0x00;
    MS_U32 u32Reg_5E, u32Reg_5F;

    // Only support address of Main_Window
    if (eWindow != MAIN_WINDOW  && eWindow != SC1_MAIN_WINDOW && eWindow != SC2_MAIN_WINDOW)
        return;

    // Bit 0 ~ 24
    if(eWindow == MAIN_WINDOW)
    {
        u32Reg_5E = REG_SC_BK12_5E_L;
        u32Reg_5F = REG_SC_BK12_5F_L;
    }
    else if(eWindow == SC1_MAIN_WINDOW)
    {
        u32Reg_5E = REG_SC1_BK12_5E_L;
        u32Reg_5F = REG_SC1_BK12_5F_L;
    }
    else
    {
        u32Reg_5E = REG_SC2_BK12_5E_L;
        u32Reg_5F = REG_SC2_BK12_5F_L;
    }

    SC_W2BYTE(u32Reg_5E, u32OPWlimitAddr & 0xFFFF);
    SC_W2BYTE(u32Reg_5F, (u32OPWlimitAddr >> 16) & 0x1FF);

    // OPW limit enable
    if (bEnable)
        u16temp = BIT(9);

    // OPW limit flag 0: maximum 1:minimum
    if (bFlag)
        u16temp |= BIT(10);

    SC_W2BYTEMSK(u32Reg_5F,u16temp ,BIT(9) | BIT(10) );
}

MS_BOOL Hal_SC_is_extra_req_en(
    MS_U16 *pu16MainHStart, MS_U16 *pu16MainHEnd,
    MS_U16 *pu16SubHStart,  MS_U16 *pu16SubHEnd,
    SCALER_WIN eWindow)
{
    MS_U16 u16MainHStart, u16SubHStart;
    MS_U16 u16MainHEnd, u16SubHEnd;
    MS_BOOL bEn = FALSE;
    MS_U32 u32Reg_BK10_08, u32Reg_BK10_09, u32Reg_BK0F_07, u32Reg_BK0F_08, u32Reg_BK20_10;

    if(eWindow == MAIN_WINDOW || eWindow == SUB_WINDOW)
    {
        u32Reg_BK10_08 = REG_SC_BK10_08_L;
        u32Reg_BK10_09 = REG_SC_BK10_09_L;
        u32Reg_BK0F_07 = REG_SC_BK0F_07_L;
        u32Reg_BK0F_08 = REG_SC_BK0F_08_L;
        u32Reg_BK20_10 = REG_SC_BK20_10_L;
    }
    else if(eWindow == SC2_MAIN_WINDOW || eWindow == SC2_SUB_WINDOW)
    {
        u32Reg_BK10_08 = REG_SC2_BK10_08_L;
        u32Reg_BK10_09 = REG_SC2_BK10_09_L;
        u32Reg_BK0F_07 = REG_SC2_BK0F_07_L;
        u32Reg_BK0F_08 = REG_SC2_BK0F_08_L;
        u32Reg_BK20_10 = REG_SC2_BK20_10_L;
    }
    else
    {
        u32Reg_BK10_08 = REG_SC1_BK10_08_L;
        u32Reg_BK10_09 = REG_SC1_BK10_09_L;
        u32Reg_BK0F_07 = REG_SC1_BK0F_07_L;
        u32Reg_BK0F_08 = REG_SC1_BK0F_08_L;
        u32Reg_BK20_10 = REG_SC1_BK20_10_L;
    }

    u16MainHStart = pu16MainHStart == NULL ? SC_R2BYTE(u32Reg_BK10_08) : *pu16MainHStart;
    u16MainHEnd   = pu16MainHEnd   == NULL ? SC_R2BYTE(u32Reg_BK10_09) : *pu16MainHEnd;
    u16SubHStart  = pu16SubHStart  == NULL ? SC_R2BYTE(u32Reg_BK0F_07) : *pu16SubHStart;
    u16SubHEnd    = pu16SubHEnd    == NULL ? SC_R2BYTE(u32Reg_BK0F_08) : *pu16SubHEnd;

    if( SC_R2BYTEMSK( u32Reg_BK20_10, BIT(1)))
    {
        if(u16MainHStart > u16SubHStart || u16MainHEnd < u16SubHEnd)
        {
            bEn = FALSE;
        }
        else
        {
            bEn = TRUE;
        }
    }
    else
    {
        bEn = FALSE;
    }

    SC_DBG(printf("Sub=%d, MainHs=%d, SubHs=%d, MainHe=%d, SubHe=%d :: Extra=%d\n",
     SC_R2BYTEMSK( u32Reg_BK20_10, BIT(1)),
     u16MainHStart, u16SubHStart, u16MainHEnd, u16SubHEnd, bEn));

    return bEn;
}

void Hal_SC_sw_db_burst( P_SC_SWDB_INFO pDBreg, SCALER_WIN eWindow )
{
    _MLOAD_ENTRY();
    if( eWindow == MAIN_WINDOW )
    {
        if(Hal_SC_is_extra_req_en(&pDBreg->u16H_DisStart, &pDBreg->u16H_DisEnd, NULL, NULL, eWindow))
        {
            MApi_XC_MLoad_WriteCmd(REG_SC_BK20_11_L, BIT(15), BIT(15));
        }
        else
        {
             MApi_XC_MLoad_WriteCmd(REG_SC_BK20_11_L, 0, BIT(15));
        }

        //H pre-scaling
        MApi_XC_MLoad_WriteCmd(REG_SC_BK02_04_L, (MS_U16)(pDBreg->u32H_PreScalingRatio>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK02_05_L, (MS_U16)(pDBreg->u32H_PreScalingRatio>>16), 0xFFFF);
        //V pre-scaling
        MApi_XC_MLoad_WriteCmd(REG_SC_BK02_08_L, (MS_U16)(pDBreg->u32V_PreScalingRatio>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK02_09_L, (MS_U16)(pDBreg->u32V_PreScalingRatio>>16), 0xFFFF);
        //H post-scaling
        MApi_XC_MLoad_WriteCmd(REG_SC_BK23_07_L, (MS_U16)(pDBreg->u32H_PostScalingRatio>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK23_08_L, (MS_U16)(pDBreg->u32H_PostScalingRatio>>16), 0xFFFF);
        //V post-scaling
        MApi_XC_MLoad_WriteCmd(REG_SC_BK23_09_L, (MS_U16)(pDBreg->u32V_PostScalingRatio>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK23_0A_L, (MS_U16)(pDBreg->u32V_PostScalingRatio>>16), 0xFFFF);
        //SCMI: vertical limitation
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_18_L, (MS_U16)(pDBreg->u16VWritelimit), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_1A_L, (MS_U16)(pDBreg->u32WritelimitBase>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_1B_L, (MS_U16)(pDBreg->u32WritelimitBase>>16), 0xFFFF);
        //SCMI: IPM address 0
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_08_L, (MS_U16)(pDBreg->u32DNRBase0>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_09_L, (MS_U16)(pDBreg->u32DNRBase0>>16), 0xFFFF);
        //SCMI: DNR address 1
        //MApi_XC_MLoad_WriteCmd(REG_SC_BK12_0A_L, (MS_U16)(pDBreg->u32DNRBase1>>00), 0xFFFF);
        //MApi_XC_MLoad_WriteCmd(REG_SC_BK12_0B_L, (MS_U16)(pDBreg->u32DNRBase1>>16), 0xFFFF);
        //SCMI: DNR address 2
        //MApi_XC_MLoad_WriteCmd(REG_SC_BK12_0C_L, (MS_U16)(pDBreg->u32DNRBase2>>00), 0xFFFF);
        //MApi_XC_MLoad_WriteCmd(REG_SC_BK12_0D_L, (MS_U16)(pDBreg->u32DNRBase2>>16), 0xFFFF);
        //SCMI: DNR offset
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_0E_L, (MS_U16)(pDBreg->u16DNROffset>>00), 0xFFFF);
        //SCMI: DNR fetch
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_0F_L, (MS_U16)(pDBreg->u16DNRFetch>>00), 0xFFFF);
        //SCMI: OPM address 0
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_10_L, (MS_U16)(pDBreg->u32OPMBase0>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_11_L, (MS_U16)(pDBreg->u32OPMBase0>>16), 0xFFFF);
        //SCMI: OPM address 1
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_12_L, (MS_U16)(pDBreg->u32OPMBase1>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_13_L, (MS_U16)(pDBreg->u32OPMBase1>>16), 0xFFFF);
        //SCMI: OPM address 2
//        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_14_L, (MS_U16)(pDBreg->u32OPMBase2>>00), 0xFFFF);
//        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_15_L, (MS_U16)(pDBreg->u32OPMBase2>>16), 0xFFFF);
        //SCMI: OPM offset
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_16_L, (MS_U16)(pDBreg->u16OPMOffset>>00), 0xFFFF);
        //SCMI: OPM fetch
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_17_L, (MS_U16)(pDBreg->u16OPMFetch>>00), 0xFFFF);

        //PIP
        MApi_XC_MLoad_WriteCmd(REG_SC_BK20_15_L, (MS_U16)(pDBreg->u16VLen>>00), 0xFFFF); // Write V length
        MApi_XC_MLoad_WriteCmd(REG_SC_BK20_1D_L, (MS_U16)(pDBreg->u16LBOffset&0xFF), 0x00FF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK20_1C_L, (MS_U16)(pDBreg->u16DispOffset&0xFF), 0x00FF);

        //capture window
        MApi_XC_MLoad_WriteCmd(REG_SC_BK01_04_L, (MS_U16)(pDBreg->u16V_CapStart>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK01_05_L, (MS_U16)(pDBreg->u16H_CapStart>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK01_06_L, (MS_U16)(pDBreg->u16V_CapSize>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK01_07_L, (MS_U16)(pDBreg->u16H_CapSize>>00), 0xFFFF);

        //Display window
        MApi_XC_MLoad_WriteCmd(REG_SC_BK10_08_L, (MS_U16)(pDBreg->u16H_DisStart>>00), VOP_DISPLAY_HSTART_MASK);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK10_09_L, (MS_U16)(pDBreg->u16H_DisEnd>>00), VOP_DISPLAY_HEND_MASK);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK10_0A_L, (MS_U16)(pDBreg->u16V_DisStart>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK10_0B_L, (MS_U16)(pDBreg->u16V_DisEnd>>00), 0xFFFF);
        MApi_XC_MLoad_Fire(TRUE);

    }
    else if( eWindow == SUB_WINDOW )
    {
        if(Hal_SC_is_extra_req_en(NULL, NULL, &pDBreg->u16H_DisStart, &pDBreg->u16H_DisEnd, eWindow))
        {
            MApi_XC_MLoad_WriteCmd(REG_SC_BK20_11_L, BIT(15), BIT(15));
        }
        else
        {
             MApi_XC_MLoad_WriteCmd(REG_SC_BK20_11_L, 0, BIT(15));
        }

        //H pre-scaling
        MApi_XC_MLoad_WriteCmd(REG_SC_BK04_04_L, (MS_U16)(pDBreg->u32H_PreScalingRatio>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK04_05_L, (MS_U16)(pDBreg->u32H_PreScalingRatio>>16), 0xFFFF);
        //V pre-scaling
        MApi_XC_MLoad_WriteCmd(REG_SC_BK04_08_L, (MS_U16)(pDBreg->u32V_PreScalingRatio>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK04_09_L, (MS_U16)(pDBreg->u32V_PreScalingRatio>>16), 0xFFFF);

        //H post-scaling
        MApi_XC_MLoad_WriteCmd(REG_SC_BK23_27_L, (MS_U16)(pDBreg->u32H_PostScalingRatio>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK23_28_L, (MS_U16)(pDBreg->u32H_PostScalingRatio>>16), 0xFFFF);
        //V post-scaling
        MApi_XC_MLoad_WriteCmd(REG_SC_BK23_29_L, (MS_U16)(pDBreg->u32V_PostScalingRatio>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK23_2A_L, (MS_U16)(pDBreg->u32V_PostScalingRatio>>16), 0xFFFF);

        //SCMI: vertical limitation
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_58_L, (MS_U16)(pDBreg->u16VWritelimit), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_5A_L, (MS_U16)(pDBreg->u32WritelimitBase>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_5B_L, (MS_U16)(pDBreg->u32WritelimitBase>>16), 0xFFFF);
        //SCMI: IPM address 0
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_48_L, (MS_U16)(pDBreg->u32DNRBase0>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_49_L, (MS_U16)(pDBreg->u32DNRBase0>>16), 0xFFFF);
        //SCMI: DNR address 1
        //MApi_XC_MLoad_WriteCmd(REG_SC_BK12_4A_L, (MS_U16)(pDBreg->u32DNRBase1>>00), 0xFFFF);
        //MApi_XC_MLoad_WriteCmd(REG_SC_BK12_4B_L, (MS_U16)(pDBreg->u32DNRBase1>>16), 0xFFFF);
        //SCMI: DNR address 2
        //MApi_XC_MLoad_WriteCmd(REG_SC_BK12_4C_L, (MS_U16)(pDBreg->u32DNRBase2>>00), 0xFFFF);
        //MApi_XC_MLoad_WriteCmd(REG_SC_BK12_4D_L, (MS_U16)(pDBreg->u32DNRBase2>>16), 0xFFFF);
        //SCMI: DNR offset
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_4E_L, (MS_U16)(pDBreg->u16DNROffset>>00), 0xFFFF);
        //SCMI: DNR fetch
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_4F_L, (MS_U16)(pDBreg->u16DNRFetch>>00), 0xFFFF);
        //SCMI: OPM address 0
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_50_L, (MS_U16)(pDBreg->u32OPMBase0>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_51_L, (MS_U16)(pDBreg->u32OPMBase0>>16), 0xFFFF);
        //SCMI: OPM address 1
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_52_L, (MS_U16)(pDBreg->u32OPMBase1>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_53_L, (MS_U16)(pDBreg->u32OPMBase1>>16), 0xFFFF);
        //SCMI: OPM address 2
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_54_L, (MS_U16)(pDBreg->u32OPMBase2>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_55_L, (MS_U16)(pDBreg->u32OPMBase2>>16), 0xFFFF);
        //SCMI: OPM offset
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_56_L, (MS_U16)(pDBreg->u16OPMOffset>>00), 0xFFFF);
        //SCMI: OPM fetch
        MApi_XC_MLoad_WriteCmd(REG_SC_BK12_57_L, (MS_U16)(pDBreg->u16OPMFetch>>00), 0xFFFF);

        //PIP
        MApi_XC_MLoad_WriteCmd(REG_SC_BK20_16_L, (MS_U16)(pDBreg->u16VLen>>00), 0xFFFF); // Write V length
        MApi_XC_MLoad_WriteCmd(REG_SC_BK20_1D_L, (MS_U16)((pDBreg->u16LBOffset&0xFF)<<8), 0xFF00);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK20_1C_L, (MS_U16)((pDBreg->u16DispOffset&0xFF)<<8), 0xFF00);

        // capture window
        MApi_XC_MLoad_WriteCmd(REG_SC_BK03_04_L, (MS_U16)(pDBreg->u16V_CapStart>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK03_05_L, (MS_U16)(pDBreg->u16H_CapStart>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK03_06_L, (MS_U16)(pDBreg->u16V_CapSize>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK03_07_L, (MS_U16)(pDBreg->u16H_CapSize>>00), 0xFFFF);

        //Display window
        MApi_XC_MLoad_WriteCmd(REG_SC_BK0F_07_L, (MS_U16)(pDBreg->u16H_DisStart>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK0F_08_L, (MS_U16)(pDBreg->u16H_DisEnd>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK0F_09_L, (MS_U16)(pDBreg->u16V_DisStart>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC_BK0F_0A_L, (MS_U16)(pDBreg->u16V_DisEnd>>00), 0xFFFF);
        MApi_XC_MLoad_Fire(TRUE);
        //while(!MDrv_XC_MLoad_BufferEmpty()){}
    }
    else if( eWindow == SC1_MAIN_WINDOW)
    {
        if(Hal_SC_is_extra_req_en(&pDBreg->u16H_DisStart, &pDBreg->u16H_DisEnd, NULL, NULL, eWindow))
        {
            MApi_XC_MLoad_WriteCmd(REG_SC1_BK20_11_L, BIT(15), BIT(15));
        }
        else
        {
             MApi_XC_MLoad_WriteCmd(REG_SC1_BK20_11_L, 0, BIT(15));
        }

        //H pre-scaling
        MApi_XC_MLoad_WriteCmd(REG_SC1_BK02_04_L, (MS_U16)(pDBreg->u32H_PreScalingRatio>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC1_BK02_05_L, (MS_U16)(pDBreg->u32H_PreScalingRatio>>16), 0xFFFF);
        //V pre-scaling
        MApi_XC_MLoad_WriteCmd(REG_SC1_BK02_08_L, (MS_U16)(pDBreg->u32V_PreScalingRatio>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC1_BK02_09_L, (MS_U16)(pDBreg->u32V_PreScalingRatio>>16), 0xFFFF);
        //H post-scaling
        MApi_XC_MLoad_WriteCmd(REG_SC1_BK23_07_L, (MS_U16)(pDBreg->u32H_PostScalingRatio>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC1_BK23_08_L, (MS_U16)(pDBreg->u32H_PostScalingRatio>>16), 0xFFFF);
        //V post-scaling
        MApi_XC_MLoad_WriteCmd(REG_SC1_BK23_09_L, (MS_U16)(pDBreg->u32V_PostScalingRatio>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC1_BK23_0A_L, (MS_U16)(pDBreg->u32V_PostScalingRatio>>16), 0xFFFF);
        //SCMI: vertical limitation
        MApi_XC_MLoad_WriteCmd(REG_SC1_BK12_18_L, (MS_U16)(pDBreg->u16VWritelimit), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC1_BK12_1A_L, (MS_U16)(pDBreg->u32WritelimitBase>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC1_BK12_1B_L, (MS_U16)(pDBreg->u32WritelimitBase>>16), 0xFFFF);
        //SCMI: IPM address 0
        MApi_XC_MLoad_WriteCmd(REG_SC1_BK12_08_L, (MS_U16)(pDBreg->u32DNRBase0>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC1_BK12_09_L, (MS_U16)(pDBreg->u32DNRBase0>>16), 0xFFFF);
        //SCMI: DNR address 1
        //MApi_XC_MLoad_WriteCmd(REG_SC1_BK12_0A_L, (MS_U16)(pDBreg->u32DNRBase1>>00), 0xFFFF);
        //MApi_XC_MLoad_WriteCmd(REG_SC1_BK12_0B_L, (MS_U16)(pDBreg->u32DNRBase1>>16), 0xFFFF);
        //SCMI: DNR address 2
        //MApi_XC_MLoad_WriteCmd(REG_SC1_BK12_0C_L, (MS_U16)(pDBreg->u32DNRBase2>>00), 0xFFFF);
        //MApi_XC_MLoad_WriteCmd(REG_SC1_BK12_0D_L, (MS_U16)(pDBreg->u32DNRBase2>>16), 0xFFFF);
        //SCMI: DNR offset
        MApi_XC_MLoad_WriteCmd(REG_SC1_BK12_0E_L, (MS_U16)(pDBreg->u16DNROffset>>00), 0xFFFF);
        //SCMI: DNR fetch
        MApi_XC_MLoad_WriteCmd(REG_SC1_BK12_0F_L, (MS_U16)(pDBreg->u16DNRFetch>>00), 0xFFFF);
        //SCMI: OPM address 0
        MApi_XC_MLoad_WriteCmd(REG_SC1_BK12_10_L, (MS_U16)(pDBreg->u32OPMBase0>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC1_BK12_11_L, (MS_U16)(pDBreg->u32OPMBase0>>16), 0xFFFF);
        //SCMI: OPM address 1
        MApi_XC_MLoad_WriteCmd(REG_SC1_BK12_12_L, (MS_U16)(pDBreg->u32OPMBase1>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC1_BK12_13_L, (MS_U16)(pDBreg->u32OPMBase1>>16), 0xFFFF);
        //SCMI: OPM address 2
//        MApi_XC_MLoad_WriteCmd(REG_SC1_BK12_14_L, (MS_U16)(pDBreg->u32OPMBase2>>00), 0xFFFF);
//        MApi_XC_MLoad_WriteCmd(REG_SC1_BK12_15_L, (MS_U16)(pDBreg->u32OPMBase2>>16), 0xFFFF);
        //SCMI: OPM offset
        MApi_XC_MLoad_WriteCmd(REG_SC1_BK12_16_L, (MS_U16)(pDBreg->u16OPMOffset>>00), 0xFFFF);
        //SCMI: OPM fetch
        MApi_XC_MLoad_WriteCmd(REG_SC1_BK12_17_L, (MS_U16)(pDBreg->u16OPMFetch>>00), 0xFFFF);

        //PIP
        MApi_XC_MLoad_WriteCmd(REG_SC1_BK20_15_L, (MS_U16)(pDBreg->u16VLen>>00), 0xFFFF); // Write V length
        MApi_XC_MLoad_WriteCmd(REG_SC1_BK20_1D_L, (MS_U16)(pDBreg->u16LBOffset&0xFF), 0x00FF);
        MApi_XC_MLoad_WriteCmd(REG_SC1_BK20_1C_L, (MS_U16)(pDBreg->u16DispOffset&0xFF), 0x00FF);

        //capture window
        MApi_XC_MLoad_WriteCmd(REG_SC1_BK01_04_L, (MS_U16)(pDBreg->u16V_CapStart>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC1_BK01_05_L, (MS_U16)(pDBreg->u16H_CapStart>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC1_BK01_06_L, (MS_U16)(pDBreg->u16V_CapSize>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC1_BK01_07_L, (MS_U16)(pDBreg->u16H_CapSize>>00), 0xFFFF);

        //Display window
        MApi_XC_MLoad_WriteCmd(REG_SC1_BK10_08_L, (MS_U16)(pDBreg->u16H_DisStart>>00), VOP_DISPLAY_HSTART_MASK);
        MApi_XC_MLoad_WriteCmd(REG_SC1_BK10_09_L, (MS_U16)(pDBreg->u16H_DisEnd>>00), VOP_DISPLAY_HEND_MASK);
        MApi_XC_MLoad_WriteCmd(REG_SC1_BK10_0A_L, (MS_U16)(pDBreg->u16V_DisStart>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC1_BK10_0B_L, (MS_U16)(pDBreg->u16V_DisEnd>>00), 0xFFFF);
        MApi_XC_MLoad_Fire(TRUE);
    }
    else if( eWindow == SC2_MAIN_WINDOW)
    {
        if(Hal_SC_is_extra_req_en(&pDBreg->u16H_DisStart, &pDBreg->u16H_DisEnd, NULL, NULL, eWindow))
        {
            MApi_XC_MLoad_WriteCmd(REG_SC2_BK20_11_L, BIT(15), BIT(15));
        }
        else
        {
             MApi_XC_MLoad_WriteCmd(REG_SC2_BK20_11_L, 0, BIT(15));
        }

        //H pre-scaling
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK02_04_L, (MS_U16)(pDBreg->u32H_PreScalingRatio>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK02_05_L, (MS_U16)(pDBreg->u32H_PreScalingRatio>>16), 0xFFFF);
        //V pre-scaling
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK02_08_L, (MS_U16)(pDBreg->u32V_PreScalingRatio>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK02_09_L, (MS_U16)(pDBreg->u32V_PreScalingRatio>>16), 0xFFFF);
        //H post-scaling
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK23_07_L, (MS_U16)(pDBreg->u32H_PostScalingRatio>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK23_08_L, (MS_U16)(pDBreg->u32H_PostScalingRatio>>16), 0xFFFF);
        //V post-scaling
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK23_09_L, (MS_U16)(pDBreg->u32V_PostScalingRatio>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK23_0A_L, (MS_U16)(pDBreg->u32V_PostScalingRatio>>16), 0xFFFF);
        //SCMI: vertical limitation
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK12_18_L, (MS_U16)(pDBreg->u16VWritelimit), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK12_1A_L, (MS_U16)(pDBreg->u32WritelimitBase>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK12_1B_L, (MS_U16)(pDBreg->u32WritelimitBase>>16), 0xFFFF);
        //SCMI: IPM address 0
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK12_08_L, (MS_U16)(pDBreg->u32DNRBase0>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK12_09_L, (MS_U16)(pDBreg->u32DNRBase0>>16), 0xFFFF);
        //SCMI: DNR address 1
        //MApi_XC_MLoad_WriteCmd(REG_SC2_BK12_0A_L, (MS_U16)(pDBreg->u32DNRBase1>>00), 0xFFFF);
        //MApi_XC_MLoad_WriteCmd(REG_SC2_BK12_0B_L, (MS_U16)(pDBreg->u32DNRBase1>>16), 0xFFFF);
        //SCMI: DNR address 2
        //MApi_XC_MLoad_WriteCmd(REG_SC2_BK12_0C_L, (MS_U16)(pDBreg->u32DNRBase2>>00), 0xFFFF);
        //MApi_XC_MLoad_WriteCmd(REG_SC2_BK12_0D_L, (MS_U16)(pDBreg->u32DNRBase2>>16), 0xFFFF);
        //SCMI: DNR offset
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK12_0E_L, (MS_U16)(pDBreg->u16DNROffset>>00), 0xFFFF);
        //SCMI: DNR fetch
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK12_0F_L, (MS_U16)(pDBreg->u16DNRFetch>>00), 0xFFFF);
        //SCMI: OPM address 0
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK12_10_L, (MS_U16)(pDBreg->u32OPMBase0>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK12_11_L, (MS_U16)(pDBreg->u32OPMBase0>>16), 0xFFFF);
        //SCMI: OPM address 1
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK12_12_L, (MS_U16)(pDBreg->u32OPMBase1>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK12_13_L, (MS_U16)(pDBreg->u32OPMBase1>>16), 0xFFFF);
        //SCMI: OPM address 2
//        MApi_XC_MLoad_WriteCmd(REG_SC2_BK12_14_L, (MS_U16)(pDBreg->u32OPMBase2>>00), 0xFFFF);
//        MApi_XC_MLoad_WriteCmd(REG_SC2_BK12_15_L, (MS_U16)(pDBreg->u32OPMBase2>>16), 0xFFFF);
        //SCMI: OPM offset
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK12_16_L, (MS_U16)(pDBreg->u16OPMOffset>>00), 0xFFFF);
        //SCMI: OPM fetch
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK12_17_L, (MS_U16)(pDBreg->u16OPMFetch>>00), 0xFFFF);

        //PIP
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK20_15_L, (MS_U16)(pDBreg->u16VLen>>00), 0xFFFF); // Write V length
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK20_1D_L, (MS_U16)(pDBreg->u16LBOffset&0xFF), 0x00FF);
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK20_1C_L, (MS_U16)(pDBreg->u16DispOffset&0xFF), 0x00FF);

        //capture window
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK01_04_L, (MS_U16)(pDBreg->u16V_CapStart>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK01_05_L, (MS_U16)(pDBreg->u16H_CapStart>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK01_06_L, (MS_U16)(pDBreg->u16V_CapSize>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK01_07_L, (MS_U16)(pDBreg->u16H_CapSize>>00), 0xFFFF);

        //Display window
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK10_08_L, (MS_U16)(pDBreg->u16H_DisStart>>00), VOP_DISPLAY_HSTART_MASK);
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK10_09_L, (MS_U16)(pDBreg->u16H_DisEnd>>00), VOP_DISPLAY_HEND_MASK);
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK10_0A_L, (MS_U16)(pDBreg->u16V_DisStart>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK10_0B_L, (MS_U16)(pDBreg->u16V_DisEnd>>00), 0xFFFF);
        MApi_XC_MLoad_Fire(TRUE);
    }
    else if( eWindow == SC2_SUB_WINDOW)
    {
        if(Hal_SC_is_extra_req_en(NULL, NULL, &pDBreg->u16H_DisStart, &pDBreg->u16H_DisEnd, eWindow))
        {
            MApi_XC_MLoad_WriteCmd(REG_SC2_BK20_11_L, BIT(15), BIT(15));
        }
        else
        {
             MApi_XC_MLoad_WriteCmd(REG_SC2_BK20_11_L, 0, BIT(15));
        }

        //H pre-scaling
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK04_04_L, (MS_U16)(pDBreg->u32H_PreScalingRatio>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK04_05_L, (MS_U16)(pDBreg->u32H_PreScalingRatio>>16), 0xFFFF);
        //V pre-scaling
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK04_08_L, (MS_U16)(pDBreg->u32V_PreScalingRatio>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK04_09_L, (MS_U16)(pDBreg->u32V_PreScalingRatio>>16), 0xFFFF);

        //H post-scaling
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK23_27_L, (MS_U16)(pDBreg->u32H_PostScalingRatio>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK23_28_L, (MS_U16)(pDBreg->u32H_PostScalingRatio>>16), 0xFFFF);
        //V post-scaling
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK23_29_L, (MS_U16)(pDBreg->u32V_PostScalingRatio>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK23_2A_L, (MS_U16)(pDBreg->u32V_PostScalingRatio>>16), 0xFFFF);

        //SCMI: vertical limitation
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK12_58_L, (MS_U16)(pDBreg->u16VWritelimit), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK12_5A_L, (MS_U16)(pDBreg->u32WritelimitBase>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK12_5B_L, (MS_U16)(pDBreg->u32WritelimitBase>>16), 0xFFFF);
        //SCMI: IPM address 0
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK12_48_L, (MS_U16)(pDBreg->u32DNRBase0>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK12_49_L, (MS_U16)(pDBreg->u32DNRBase0>>16), 0xFFFF);
        //SCMI: DNR address 1
        //MApi_XC_MLoad_WriteCmd(REG_SC2_BK12_4A_L, (MS_U16)(pDBreg->u32DNRBase1>>00), 0xFFFF);
        //MApi_XC_MLoad_WriteCmd(REG_SC2_BK12_4B_L, (MS_U16)(pDBreg->u32DNRBase1>>16), 0xFFFF);
        //SCMI: DNR address 2
        //MApi_XC_MLoad_WriteCmd(REG_SC_BK12_4C_L, (MS_U16)(pDBreg->u32DNRBase2>>00), 0xFFFF);
        //MApi_XC_MLoad_WriteCmd(REG_SC_BK12_4D_L, (MS_U16)(pDBreg->u32DNRBase2>>16), 0xFFFF);
        //SCMI: DNR offset
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK12_4E_L, (MS_U16)(pDBreg->u16DNROffset>>00), 0xFFFF);
        //SCMI: DNR fetch
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK12_4F_L, (MS_U16)(pDBreg->u16DNRFetch>>00), 0xFFFF);
        //SCMI: OPM address 0
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK12_50_L, (MS_U16)(pDBreg->u32OPMBase0>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK12_51_L, (MS_U16)(pDBreg->u32OPMBase0>>16), 0xFFFF);
        //SCMI: OPM address 1
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK12_52_L, (MS_U16)(pDBreg->u32OPMBase1>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK12_53_L, (MS_U16)(pDBreg->u32OPMBase1>>16), 0xFFFF);
        //SCMI: OPM address 2
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK12_54_L, (MS_U16)(pDBreg->u32OPMBase2>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK12_55_L, (MS_U16)(pDBreg->u32OPMBase2>>16), 0xFFFF);
        //SCMI: OPM offset
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK12_56_L, (MS_U16)(pDBreg->u16OPMOffset>>00), 0xFFFF);
        //SCMI: OPM fetch
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK12_57_L, (MS_U16)(pDBreg->u16OPMFetch>>00), 0xFFFF);

        //PIP
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK20_16_L, (MS_U16)(pDBreg->u16VLen>>00), 0xFFFF); // Write V length
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK20_1D_L, (MS_U16)((pDBreg->u16LBOffset&0xFF)<<8), 0xFF00);
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK20_1C_L, (MS_U16)((pDBreg->u16DispOffset&0xFF)<<8), 0xFF00);

        // capture window
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK03_04_L, (MS_U16)(pDBreg->u16V_CapStart>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK03_05_L, (MS_U16)(pDBreg->u16H_CapStart>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK03_06_L, (MS_U16)(pDBreg->u16V_CapSize>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK03_07_L, (MS_U16)(pDBreg->u16H_CapSize>>00), 0xFFFF);

        //Display window
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK0F_07_L, (MS_U16)(pDBreg->u16H_DisStart>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK0F_08_L, (MS_U16)(pDBreg->u16H_DisEnd>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK0F_09_L, (MS_U16)(pDBreg->u16V_DisStart>>00), 0xFFFF);
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK0F_0A_L, (MS_U16)(pDBreg->u16V_DisEnd>>00), 0xFFFF);
        MApi_XC_MLoad_Fire(TRUE);
        //while(!MDrv_XC_MLoad_BufferEmpty()){}
    }
    else
    {
    }

    _MLOAD_RETURN();
}

void Hal_SC_sw_db( P_SC_SWDB_INFO pDBreg, SCALER_WIN eWindow )
{
    if( eWindow == MAIN_WINDOW )
    {
        if(Hal_SC_is_extra_req_en(&pDBreg->u16H_DisStart, &pDBreg->u16H_DisEnd, NULL, NULL, eWindow))
        {
            SC_W2BYTEMSK(REG_SC_BK20_11_L, BIT(15), BIT(15));
        }
        else
        {
             SC_W2BYTEMSK(REG_SC_BK20_11_L, 0, BIT(15));
        }

        SC_W4BYTE(REG_SC_BK02_04_L, pDBreg->u32H_PreScalingRatio);   // H pre-scaling
        SC_W4BYTE(REG_SC_BK02_08_L, pDBreg->u32V_PreScalingRatio);   // V pre-scaling
        //HVSP
        SC_W4BYTE(REG_SC_BK23_07_L, pDBreg->u32H_PostScalingRatio);  // H post-scaling
        SC_W4BYTE(REG_SC_BK23_09_L, pDBreg->u32V_PostScalingRatio);  // V post-scaling
        //SCMI
        SC_W2BYTE(REG_SC_BK12_18_L, pDBreg->u16VWritelimit); // Set vertical limitation
        SC_W4BYTE(REG_SC_BK12_1A_L, pDBreg->u32WritelimitBase);
        SC_W4BYTE(REG_SC_BK12_08_L, pDBreg->u32DNRBase0);     // input address0
        //SC_W4BYTE(REG_SC_BK12_0A_L, pDBreg->u32DNRBase1);     // input address1
        //SC_W4BYTE(REG_SC_BK12_0C_L, pDBreg->u32DNRBase2);     // input address2
        SC_W2BYTE(REG_SC_BK12_0E_L, pDBreg->u16DNROffset);// input Offset
        SC_W2BYTE(REG_SC_BK12_0F_L, pDBreg->u16DNRFetch); // input fetch
        SC_W4BYTE(REG_SC_BK12_10_L, pDBreg->u32OPMBase0); // output address0
        SC_W4BYTE(REG_SC_BK12_12_L, pDBreg->u32OPMBase1); // output address1
//        SC_W4BYTE(REG_SC_BK12_14_L, pDBreg->u32OPMBase2); // output address2
        SC_W2BYTE(REG_SC_BK12_16_L, pDBreg->u16OPMOffset); // output offset
        SC_W2BYTE(REG_SC_BK12_17_L, pDBreg->u16OPMFetch);  // output fetch

        //PIP
        SC_W2BYTE(REG_SC_BK20_15_L, pDBreg->u16VLen); // Write V length
        SC_W2BYTEMSK(REG_SC_BK20_1D_L, (pDBreg->u16LBOffset&0xFF), 0xFF);
        SC_W2BYTEMSK(REG_SC_BK20_1C_L, (pDBreg->u16DispOffset&0xFF), 0xFF);

        // capture window
        SC_W2BYTE(REG_SC_BK01_04_L, pDBreg->u16V_CapStart); // Capture V start
        SC_W2BYTE(REG_SC_BK01_05_L, pDBreg->u16H_CapStart); // Capture H start
        SC_W2BYTE(REG_SC_BK01_06_L, pDBreg->u16V_CapSize);  // Capture V size
        SC_W2BYTE(REG_SC_BK01_07_L, pDBreg->u16H_CapSize);  // Capture H size

        //Display window
        SC_W2BYTE(REG_SC_BK10_08_L, pDBreg->u16H_DisStart); // Display H start
        SC_W2BYTE(REG_SC_BK10_09_L, pDBreg->u16H_DisEnd);   // Display H end
        SC_W2BYTE(REG_SC_BK10_0A_L, pDBreg->u16V_DisStart); // Display V start
        SC_W2BYTE(REG_SC_BK10_0B_L, pDBreg->u16V_DisEnd);   // Display V end
    }
    else if(eWindow == SUB_WINDOW)
    {

        if(Hal_SC_is_extra_req_en(&pDBreg->u16H_DisStart, &pDBreg->u16H_DisEnd, NULL, NULL, eWindow))
        {
            SC_W2BYTEMSK(REG_SC_BK20_11_L, BIT(15), BIT(15));
        }
        else
        {
             SC_W2BYTEMSK(REG_SC_BK20_11_L, 0, BIT(15));
        }

        SC_W4BYTE(REG_SC_BK04_04_L, pDBreg->u32H_PreScalingRatio);   // H pre-scaling
        SC_W4BYTE(REG_SC_BK04_08_L, pDBreg->u32V_PreScalingRatio);   // V pre-scaling

        //HVSP
        SC_W4BYTE(REG_SC_BK23_27_L, pDBreg->u32H_PostScalingRatio);  // H post-scaling
        SC_W4BYTE(REG_SC_BK23_29_L, pDBreg->u32V_PostScalingRatio);  // V post-scaling

        //SCMI
        SC_W2BYTE(REG_SC_BK12_58_L, pDBreg->u16VWritelimit); // Set vertical limitation
        SC_W4BYTE(REG_SC_BK12_5A_L, pDBreg->u32WritelimitBase);
        SC_W4BYTE(REG_SC_BK12_48_L, pDBreg->u32DNRBase0);     // input address0
        //SC_W4BYTE(REG_SC_BK12_4A_L, pDBreg->u32DNRBase1); // input address1
        //SC_W4BYTE(REG_SC_BK12_4C_L, pDBreg->u32DNRBase2); // input address2
        SC_W2BYTE(REG_SC_BK12_4E_L, pDBreg->u16DNROffset);// input Offset
        SC_W2BYTE(REG_SC_BK12_4F_L, pDBreg->u16DNRFetch); // input fetch
        SC_W4BYTE(REG_SC_BK12_50_L, pDBreg->u32OPMBase0); // output address0
        SC_W4BYTE(REG_SC_BK12_52_L, pDBreg->u32OPMBase1); // output address1
        SC_W4BYTE(REG_SC_BK12_54_L, pDBreg->u32OPMBase2); // output address2
        SC_W2BYTE(REG_SC_BK12_56_L, pDBreg->u16OPMOffset); // output offset
        SC_W2BYTE(REG_SC_BK12_57_L, pDBreg->u16OPMFetch);  // output fetch

        //PIP
        SC_W2BYTE(REG_SC_BK20_16_L, pDBreg->u16VLen); // Write V length
        SC_W2BYTEMSK(REG_SC_BK20_1D_L, (pDBreg->u16LBOffset&0xFF)<<8, 0xFF00);
        SC_W2BYTEMSK(REG_SC_BK20_1C_L, (pDBreg->u16DispOffset&0xFF)<<8, 0xFF00);

        // capture window
        SC_W2BYTE(REG_SC_BK03_04_L, pDBreg->u16V_CapStart); // Capture V start
        SC_W2BYTE(REG_SC_BK03_05_L, pDBreg->u16H_CapStart); // Capture H start
        SC_W2BYTE(REG_SC_BK03_06_L, pDBreg->u16V_CapSize);  // Capture V size
        SC_W2BYTE(REG_SC_BK03_07_L, pDBreg->u16H_CapSize);  // Capture H size

        //Display window
        SC_W2BYTE(REG_SC_BK0F_07_L, pDBreg->u16H_DisStart); // Display H start
        SC_W2BYTE(REG_SC_BK0F_08_L, pDBreg->u16H_DisEnd);   // Display H end
        SC_W2BYTE(REG_SC_BK0F_09_L, pDBreg->u16V_DisStart); // Display V start
        SC_W2BYTE(REG_SC_BK0F_0A_L, pDBreg->u16V_DisEnd);   // Display V end
    }
    else if(eWindow == SC1_MAIN_WINDOW)
    {
        if(Hal_SC_is_extra_req_en(&pDBreg->u16H_DisStart, &pDBreg->u16H_DisEnd, NULL, NULL, eWindow))
        {
            SC_W2BYTEMSK(REG_SC1_BK20_11_L, BIT(15), BIT(15));
        }
        else
        {
             SC_W2BYTEMSK(REG_SC1_BK20_11_L, 0, BIT(15));
        }

        SC_W4BYTE(REG_SC1_BK02_04_L, pDBreg->u32H_PreScalingRatio);   // H pre-scaling
        SC_W4BYTE(REG_SC1_BK02_08_L, pDBreg->u32V_PreScalingRatio);   // V pre-scaling
        //HVSP
        SC_W4BYTE(REG_SC1_BK23_07_L, pDBreg->u32H_PostScalingRatio);  // H post-scaling
        SC_W4BYTE(REG_SC1_BK23_09_L, pDBreg->u32V_PostScalingRatio);  // V post-scaling
        //SCMI
        SC_W2BYTE(REG_SC1_BK12_18_L, pDBreg->u16VWritelimit); // Set vertical limitation
        SC_W4BYTE(REG_SC1_BK12_1A_L, pDBreg->u32WritelimitBase);
        SC_W4BYTE(REG_SC1_BK12_08_L, pDBreg->u32DNRBase0);     // input address0
        //SC_W4BYTE(REG_SC1_BK12_0A_L, pDBreg->u32DNRBase1);     // input address1
        //SC_W4BYTE(REG_SC1_BK12_0C_L, pDBreg->u32DNRBase2);     // input address2
        SC_W2BYTE(REG_SC1_BK12_0E_L, pDBreg->u16DNROffset);// input Offset
        SC_W2BYTE(REG_SC1_BK12_0F_L, pDBreg->u16DNRFetch); // input fetch
        SC_W4BYTE(REG_SC1_BK12_10_L, pDBreg->u32OPMBase0); // output address0
        SC_W4BYTE(REG_SC1_BK12_12_L, pDBreg->u32OPMBase1); // output address1
//        SC_W4BYTE(REG_SC1_BK12_14_L, pDBreg->u32OPMBase2); // output address2
        SC_W2BYTE(REG_SC1_BK12_16_L, pDBreg->u16OPMOffset); // output offset
        SC_W2BYTE(REG_SC1_BK12_17_L, pDBreg->u16OPMFetch);  // output fetch

        //PIP
        SC_W2BYTE(REG_SC1_BK20_15_L, pDBreg->u16VLen); // Write V length
        SC_W2BYTEMSK(REG_SC1_BK20_1D_L, (pDBreg->u16LBOffset&0xFF), 0xFF);
        SC_W2BYTEMSK(REG_SC1_BK20_1C_L, (pDBreg->u16DispOffset&0xFF), 0xFF);

        // capture window
        SC_W2BYTE(REG_SC1_BK01_04_L, pDBreg->u16V_CapStart); // Capture V start
        SC_W2BYTE(REG_SC1_BK01_05_L, pDBreg->u16H_CapStart); // Capture H start
        SC_W2BYTE(REG_SC1_BK01_06_L, pDBreg->u16V_CapSize);  // Capture V size
        SC_W2BYTE(REG_SC1_BK01_07_L, pDBreg->u16H_CapSize);  // Capture H size

        //Display window
        SC_W2BYTE(REG_SC1_BK10_08_L, pDBreg->u16H_DisStart); // Display H start
        SC_W2BYTE(REG_SC1_BK10_09_L, pDBreg->u16H_DisEnd);   // Display H end
        SC_W2BYTE(REG_SC1_BK10_0A_L, pDBreg->u16V_DisStart); // Display V start
        SC_W2BYTE(REG_SC1_BK10_0B_L, pDBreg->u16V_DisEnd);   // Display V end
    }
    else if(eWindow == SC2_MAIN_WINDOW)
    {
        if(Hal_SC_is_extra_req_en(&pDBreg->u16H_DisStart, &pDBreg->u16H_DisEnd, NULL, NULL, eWindow))
        {
            SC_W2BYTEMSK(REG_SC2_BK20_11_L, BIT(15), BIT(15));
        }
        else
        {
             SC_W2BYTEMSK(REG_SC2_BK20_11_L, 0, BIT(15));
        }

        SC_W4BYTE(REG_SC2_BK02_04_L, pDBreg->u32H_PreScalingRatio);   // H pre-scaling
        SC_W4BYTE(REG_SC2_BK02_08_L, pDBreg->u32V_PreScalingRatio);   // V pre-scaling
        //HVSP
        SC_W4BYTE(REG_SC2_BK23_07_L, pDBreg->u32H_PostScalingRatio);  // H post-scaling
        SC_W4BYTE(REG_SC2_BK23_09_L, pDBreg->u32V_PostScalingRatio);  // V post-scaling
        //SCMI
        SC_W2BYTE(REG_SC2_BK12_18_L, pDBreg->u16VWritelimit); // Set vertical limitation
        SC_W4BYTE(REG_SC2_BK12_1A_L, pDBreg->u32WritelimitBase);
        SC_W4BYTE(REG_SC2_BK12_08_L, pDBreg->u32DNRBase0);     // input address0
        //SC_W4BYTE(REG_SC2_BK12_0A_L, pDBreg->u32DNRBase1);     // input address1
        //SC_W4BYTE(REG_SC2_BK12_0C_L, pDBreg->u32DNRBase2);     // input address2
        SC_W2BYTE(REG_SC2_BK12_0E_L, pDBreg->u16DNROffset);// input Offset
        SC_W2BYTE(REG_SC2_BK12_0F_L, pDBreg->u16DNRFetch); // input fetch
        SC_W4BYTE(REG_SC2_BK12_10_L, pDBreg->u32OPMBase0); // output address0
        SC_W4BYTE(REG_SC2_BK12_12_L, pDBreg->u32OPMBase1); // output address1
//        SC_W4BYTE(REG_SC2_BK12_14_L, pDBreg->u32OPMBase2); // output address2
        SC_W2BYTE(REG_SC2_BK12_16_L, pDBreg->u16OPMOffset); // output offset
        SC_W2BYTE(REG_SC2_BK12_17_L, pDBreg->u16OPMFetch);  // output fetch

        //PIP
        SC_W2BYTE(REG_SC2_BK20_15_L, pDBreg->u16VLen); // Write V length
        SC_W2BYTEMSK(REG_SC2_BK20_1D_L, (pDBreg->u16LBOffset&0xFF), 0xFF);
        SC_W2BYTEMSK(REG_SC2_BK20_1C_L, (pDBreg->u16DispOffset&0xFF), 0xFF);

        // capture window
        SC_W2BYTE(REG_SC2_BK01_04_L, pDBreg->u16V_CapStart); // Capture V start
        SC_W2BYTE(REG_SC2_BK01_05_L, pDBreg->u16H_CapStart); // Capture H start
        SC_W2BYTE(REG_SC2_BK01_06_L, pDBreg->u16V_CapSize);  // Capture V size
        SC_W2BYTE(REG_SC2_BK01_07_L, pDBreg->u16H_CapSize);  // Capture H size

        //Display window
        SC_W2BYTE(REG_SC2_BK10_08_L, pDBreg->u16H_DisStart); // Display H start
        SC_W2BYTE(REG_SC2_BK10_09_L, pDBreg->u16H_DisEnd);   // Display H end
        SC_W2BYTE(REG_SC2_BK10_0A_L, pDBreg->u16V_DisStart); // Display V start
        SC_W2BYTE(REG_SC2_BK10_0B_L, pDBreg->u16V_DisEnd);   // Display V end
    }
    else if(eWindow == SC2_SUB_WINDOW)
    {

        if(Hal_SC_is_extra_req_en(&pDBreg->u16H_DisStart, &pDBreg->u16H_DisEnd, NULL, NULL, eWindow))
        {
            SC_W2BYTEMSK(REG_SC2_BK20_11_L, BIT(15), BIT(15));
        }
        else
        {
             SC_W2BYTEMSK(REG_SC2_BK20_11_L, 0, BIT(15));
        }

        SC_W4BYTE(REG_SC2_BK04_04_L, pDBreg->u32H_PreScalingRatio);   // H pre-scaling
        SC_W4BYTE(REG_SC2_BK04_08_L, pDBreg->u32V_PreScalingRatio);   // V pre-scaling

        //HVSP
        SC_W4BYTE(REG_SC2_BK23_27_L, pDBreg->u32H_PostScalingRatio);  // H post-scaling
        SC_W4BYTE(REG_SC2_BK23_29_L, pDBreg->u32V_PostScalingRatio);  // V post-scaling

        //SCMI
        SC_W2BYTE(REG_SC2_BK12_58_L, pDBreg->u16VWritelimit); // Set vertical limitation
        SC_W4BYTE(REG_SC2_BK12_5A_L, pDBreg->u32WritelimitBase);
        SC_W4BYTE(REG_SC2_BK12_48_L, pDBreg->u32DNRBase0);     // input address0
        //SC_W4BYTE(REG_SC2_BK12_4A_L, pDBreg->u32DNRBase1); // input address1
        //SC_W4BYTE(REG_SC2_BK12_4C_L, pDBreg->u32DNRBase2); // input address2
        SC_W2BYTE(REG_SC2_BK12_4E_L, pDBreg->u16DNROffset);// input Offset
        SC_W2BYTE(REG_SC2_BK12_4F_L, pDBreg->u16DNRFetch); // input fetch
        SC_W4BYTE(REG_SC2_BK12_50_L, pDBreg->u32OPMBase0); // output address0
        SC_W4BYTE(REG_SC2_BK12_52_L, pDBreg->u32OPMBase1); // output address1
        SC_W4BYTE(REG_SC2_BK12_54_L, pDBreg->u32OPMBase2); // output address2
        SC_W2BYTE(REG_SC2_BK12_56_L, pDBreg->u16OPMOffset); // output offset
        SC_W2BYTE(REG_SC2_BK12_57_L, pDBreg->u16OPMFetch);  // output fetch

        //PIP
        SC_W2BYTE(REG_SC2_BK20_16_L, pDBreg->u16VLen); // Write V length
        SC_W2BYTEMSK(REG_SC2_BK20_1D_L, (pDBreg->u16LBOffset&0xFF)<<8, 0xFF00);
        SC_W2BYTEMSK(REG_SC2_BK20_1C_L, (pDBreg->u16DispOffset&0xFF)<<8, 0xFF00);

        // capture window
        SC_W2BYTE(REG_SC2_BK03_04_L, pDBreg->u16V_CapStart); // Capture V start
        SC_W2BYTE(REG_SC2_BK03_05_L, pDBreg->u16H_CapStart); // Capture H start
        SC_W2BYTE(REG_SC2_BK03_06_L, pDBreg->u16V_CapSize);  // Capture V size
        SC_W2BYTE(REG_SC2_BK03_07_L, pDBreg->u16H_CapSize);  // Capture H size

        //Display window
        SC_W2BYTE(REG_SC2_BK0F_07_L, pDBreg->u16H_DisStart); // Display H start
        SC_W2BYTE(REG_SC2_BK0F_08_L, pDBreg->u16H_DisEnd);   // Display H end
        SC_W2BYTE(REG_SC2_BK0F_09_L, pDBreg->u16V_DisStart); // Display V start
        SC_W2BYTE(REG_SC2_BK0F_0A_L, pDBreg->u16V_DisEnd);   // Display V end
    }
    else
    {
    }

}

E_APIXC_ReturnValue Hal_SC_support_source_to_ve(MS_U16* pOutputCapability)
{
	*pOutputCapability = E_XC_IP | E_XC_OP2 | E_XC_OVERLAP;
	return E_APIXC_RET_OK;
}

E_APIXC_ReturnValue Hal_SC_set_output_capture_enable(MS_BOOL bEnable,E_XC_SOURCE_TO_VE eSourceToVE)
{
    MS_BOOL bCaptureEna;
    MS_U16 u16CaptureSrc;
	// Current only support IP and OP2
	bCaptureEna = bEnable;
	switch(eSourceToVE)
	{
	    case E_XC_OP2:
            u16CaptureSrc = 8;
	        break;
	    case E_XC_GAM:
            u16CaptureSrc = 5;
	        break;
	    case E_XC_DITHER:
            u16CaptureSrc = 6;
	        break;
	    case E_XC_OVERLAP:
            u16CaptureSrc = 7;
	        break;
        default:
            u16CaptureSrc = 0;
            bCaptureEna = FALSE;
            break;
	}

	if(bCaptureEna)
	{
    	SC_W2BYTEMSK(REG_SC_BK0F_57_L, BIT(11), BIT(11));	 // Disable
    	SC_W2BYTEMSK(REG_SC_BK0F_46_L, u16CaptureSrc<<4, BMASK(7:4)); 	         // Disable
	}
	else
	{
    	SC_W2BYTEMSK(REG_SC_BK0F_57_L, 0, BIT(11));	// Disable
	}
	return E_APIXC_RET_OK; // Support Scaler OP -> VE
}


E_APIXC_ReturnValue Hal_SC_get_Htt_Vtt(MS_U16 *u16Htt, MS_U16 *u16Vtt, SCALER_WIN eWindow)
{
    MS_U32 u32RegHtt, u32RegVtt;

    u32RegHtt = eWindow == MAIN_WINDOW     ? REG_SC_BK10_0C_L :
                eWindow == SUB_WINDOW      ? REG_SC_BK10_0C_L :
                eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK10_0C_L :
                eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK10_0C_L :
                eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK10_0C_L :
                                             REG_SC_DUMMY;

    u32RegVtt = eWindow == MAIN_WINDOW     ? REG_SC_BK10_0D_L :
                eWindow == SUB_WINDOW      ? REG_SC_BK10_0D_L :
                eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK10_0D_L :
                eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK10_0D_L :
                eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK10_0D_L :
                                             REG_SC_DUMMY;

    *u16Htt = SC_R2BYTE(u32RegHtt) + 1;
    *u16Vtt = SC_R2BYTE(u32RegVtt) + 1;

	return E_APIXC_RET_OK;
}
/******************************************************************************/
///Set output DE window size
///@param pstWindow \b IN
///-  pointer to window information
/******************************************************************************/
void Hal_SC_set_de_window(XC_PANEL_INFO *pPanel_Info, SCALER_WIN eWindow)
{
    MS_U32 u32Reg04, u32Reg05, u32Reg06, u32Reg07;

    u32Reg04 = eWindow == MAIN_WINDOW     ? REG_SC_BK10_04_L :
               eWindow == SUB_WINDOW      ? REG_SC_BK10_04_L :
               eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK10_04_L :
               eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK10_04_L :
               eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK10_04_L :
                                            REG_SC_DUMMY;

    u32Reg05 = eWindow == MAIN_WINDOW     ? REG_SC_BK10_05_L :
               eWindow == SUB_WINDOW      ? REG_SC_BK10_05_L :
               eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK10_05_L :
               eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK10_05_L :
               eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK10_05_L :
                                            REG_SC_DUMMY;

    u32Reg06 = eWindow == MAIN_WINDOW     ? REG_SC_BK10_06_L :
               eWindow == SUB_WINDOW      ? REG_SC_BK10_06_L :
               eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK10_06_L :
               eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK10_06_L :
               eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK10_06_L :
                                            REG_SC_DUMMY;

    u32Reg07 = eWindow == MAIN_WINDOW     ? REG_SC_BK10_07_L :
               eWindow == SUB_WINDOW      ? REG_SC_BK10_07_L :
               eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK10_07_L :
               eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK10_07_L :
               eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK10_07_L :
                                            REG_SC_DUMMY;
#ifdef new_chakra
    SC_W2BYTE(u32Reg04, pPanel_Info->u16HStart);                                // DE H start
    SC_W2BYTE(u32Reg05, pPanel_Info->u16HStart + pPanel_Info->u16Width - 1);    // DE H end
    SC_W2BYTE(u32Reg06, pPanel_Info->u16VStart);                                // DE V start
    SC_W2BYTE(u32Reg07, pPanel_Info->u16VStart + pPanel_Info->u16Height - 1);   // DE V end
#else
    SC_W2BYTE(REG_SC_BK10_04_L, pPanel_Info->u16HStart);                                // DE H start
    SC_W2BYTE(REG_SC_BK10_05_L, pPanel_Info->u16HStart + pPanel_Info->u16Width - 1);    // DE H end
    SC_W2BYTE(REG_SC_BK10_06_L, pSrcInfo->u8DE_V_Start);                                // DE V start
    SC_W2BYTE(REG_SC_BK10_07_L, pSrcInfo->u16DE_V_End);   // DE V end
#endif
}

void Hal_SC_get_disp_de_window(MS_WINDOW_TYPE *pWin, SCALER_WIN eWindow)
{
    MS_U32 u32Reg04, u32Reg05, u32Reg06, u32Reg07;

    u32Reg04 = eWindow == MAIN_WINDOW     ? REG_SC_BK10_04_L :
               eWindow == SUB_WINDOW      ? REG_SC_BK10_04_L :
               eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK10_04_L :
               eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK10_04_L :
               eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK10_04_L :
                                            REG_SC_DUMMY;

    u32Reg05 = eWindow == MAIN_WINDOW     ? REG_SC_BK10_05_L :
               eWindow == SUB_WINDOW      ? REG_SC_BK10_05_L :
               eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK10_05_L :
               eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK10_05_L :
               eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK10_05_L :
                                            REG_SC_DUMMY;

    u32Reg06 = eWindow == MAIN_WINDOW     ? REG_SC_BK10_06_L :
               eWindow == SUB_WINDOW      ? REG_SC_BK10_06_L :
               eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK10_06_L :
               eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK10_06_L :
               eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK10_06_L :
                                            REG_SC_DUMMY;

    u32Reg07 = eWindow == MAIN_WINDOW     ? REG_SC_BK10_07_L :
               eWindow == SUB_WINDOW      ? REG_SC_BK10_07_L :
               eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK10_07_L :
               eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK10_07_L :
               eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK10_07_L :
                                            REG_SC_DUMMY;


    pWin->x = SC_R2BYTE(u32Reg04);
    pWin->y = SC_R2BYTE(u32Reg06);
    pWin->width = SC_R2BYTE(u32Reg05) - pWin->x + 1;
    pWin->height= SC_R2BYTE(u32Reg07) - pWin->y + 1;
}

MS_U32 Hal_SC_Get_DNRBase0(SCALER_WIN eWindow)
{
    if(eWindow == MAIN_WINDOW)
    {
        return SC_R4BYTE(REG_SC_BK12_08_L);     // input address0
    }
    else if(eWindow == SUB_WINDOW)
    {
        return SC_R4BYTE(REG_SC_BK12_48_L);     // input address0
    }
    else if(eWindow == SC1_MAIN_WINDOW)
    {
        return SC_R4BYTE(REG_SC1_BK12_08_L);     // input address0
    }
    else if(eWindow == SC2_MAIN_WINDOW)
    {
        return SC_R4BYTE(REG_SC2_BK12_08_L);     // input address0
    }
    else if(eWindow == SC2_SUB_WINDOW)
    {
        return SC_R4BYTE(REG_SC2_BK12_48_L);     // input address0
    }
    else
    {
        return 0;
    }

}

MS_U32 Hal_SC_Get_DNRBase1(SCALER_WIN eWindow)
{
    if(eWindow == MAIN_WINDOW)
    {
        return SC_R4BYTE(REG_SC_BK12_0A_L);     // input address1
    }
    else if(eWindow == SUB_WINDOW)
    {
        return SC_R4BYTE(REG_SC_BK12_4A_L);     // input address1
    }
    else if(eWindow == SC1_MAIN_WINDOW)
    {
        return SC_R4BYTE(REG_SC1_BK12_0A_L);     // input address1
    }
    else if(eWindow == SC2_MAIN_WINDOW)
    {
        return SC_R4BYTE(REG_SC2_BK12_0A_L);     // input address1
    }
    else if(eWindow == SC2_SUB_WINDOW)
    {
        return SC_R4BYTE(REG_SC2_BK12_4A_L);     // input address1
    }
    else
    {
        return 0;
    }

}

MS_U32 Hal_SC_Get_OPMBase0(SCALER_WIN eWindow)
{
    if(eWindow == MAIN_WINDOW)
    {
        return SC_R4BYTE(REG_SC_BK12_10_L);     // output address0
    }
    else if(eWindow == SUB_WINDOW)
    {
        return SC_R4BYTE(REG_SC_BK12_50_L);     // output address0
    }
    else if(eWindow == SC1_MAIN_WINDOW)
    {
        return SC_R4BYTE(REG_SC1_BK12_10_L);     // output address0
    }
    else if(eWindow == SC2_MAIN_WINDOW)
    {
        return SC_R4BYTE(REG_SC2_BK12_10_L);     // output address0
    }
    else if(eWindow == SC2_SUB_WINDOW)
    {
        return SC_R4BYTE(REG_SC2_BK12_50_L);     // output address0
    }
    else
    {
        return 0;
    }

}

MS_U32 Hal_SC_Get_OPMBase1(SCALER_WIN eWindow)
{
    if(eWindow == MAIN_WINDOW)
    {
        return SC_R4BYTE(REG_SC_BK12_12_L);     // output address1
    }
    else if(eWindow == SUB_WINDOW)
    {
        return SC_R4BYTE(REG_SC_BK12_52_L);     // output address1
    }
    else if(eWindow == SC1_MAIN_WINDOW)
    {
        return SC_R4BYTE(REG_SC1_BK12_12_L);     // output address1
    }
    else if(eWindow == SC2_MAIN_WINDOW)
    {
        return SC_R4BYTE(REG_SC2_BK12_12_L);     // output address1
    }
    else if(eWindow == SC2_SUB_WINDOW)
    {
        return SC_R4BYTE(REG_SC2_BK12_52_L);     // output address1
    }
    else
    {
        return 0;
    }

}

MS_U8 Hal_SC_Get_LBOffset(SCALER_WIN eWindow)
{
    if(eWindow == MAIN_WINDOW)
    {
        return SC_R2BYTEMSK(REG_SC_BK20_1D_L, 0x00FF);
    }
    else if(eWindow == SUB_WINDOW)
    {
        return (SC_R2BYTEMSK(REG_SC_BK20_1D_L, 0xFF00) >> 8);
    }
    else if(eWindow == SC1_MAIN_WINDOW)
    {
        return SC_R2BYTEMSK(REG_SC1_BK20_1D_L, 0x00FF);
    }
    else if(eWindow == SC2_MAIN_WINDOW)
    {
        return SC_R2BYTEMSK(REG_SC2_BK20_1D_L, 0x00FF);
    }
    else if(eWindow == SC2_SUB_WINDOW)
    {
        return (SC_R2BYTEMSK(REG_SC2_BK20_1D_L, 0xFF00) >> 8);
    }
    else
    {
        return 0;
    }

}

void Hal_SC_Set_LBOffset(MS_U8 u8LBOffset, SCALER_WIN eWindow)
{
    if(eWindow == MAIN_WINDOW)
    {
        SC_W2BYTEMSK(REG_SC_BK20_1D_L, u8LBOffset, 0x00FF);
    }
    else if(eWindow == SUB_WINDOW)
    {
        SC_W2BYTEMSK(REG_SC_BK20_1D_L, u8LBOffset << 8, 0xFF00);
    }
    else if(eWindow == SC1_MAIN_WINDOW)
    {
        SC_W2BYTEMSK(REG_SC1_BK20_1D_L, u8LBOffset, 0x00FF);
    }
    else if(eWindow == SC2_MAIN_WINDOW)
    {
        SC_W2BYTEMSK(REG_SC2_BK20_1D_L, u8LBOffset, 0x00FF);
    }
    else if(eWindow == SC2_SUB_WINDOW)
    {
        SC_W2BYTEMSK(REG_SC2_BK20_1D_L, u8LBOffset << 8, 0xFF00);
    }
    else
    {
    }

}

void Hal_SC_set_DNRBase0(MS_U32 u32DNRBase0, SCALER_WIN eWindow)
{
    if(eWindow == MAIN_WINDOW)
    {
        SC_W4BYTE(REG_SC_BK12_08_L, u32DNRBase0 / BYTE_PER_WORD);     // input address0
    }
    else if(eWindow == SUB_WINDOW)
    {
        SC_W4BYTE(REG_SC_BK12_48_L, u32DNRBase0 / BYTE_PER_WORD);     // input address0
    }
    else if(eWindow == SC1_MAIN_WINDOW)
    {
        SC_W4BYTE(REG_SC1_BK12_08_L, u32DNRBase0 / BYTE_PER_WORD);     // input address0
    }
    else if(eWindow == SC2_MAIN_WINDOW)
    {
        SC_W4BYTE(REG_SC2_BK12_08_L, u32DNRBase0 / BYTE_PER_WORD);     // input address0
    }
    else if(eWindow == SC2_SUB_WINDOW)
    {
        SC_W4BYTE(REG_SC2_BK12_48_L, u32DNRBase0 / BYTE_PER_WORD);     // input address0
    }
    else
    {
    }

}

void Hal_SC_set_DNRBase1(MS_U32 u32DNRBase1, SCALER_WIN eWindow)
{
    UNUSED(u32DNRBase1);
    UNUSED(eWindow);
}

void Hal_SC_set_DNRBase2(MS_U32 u32DNRBase2, SCALER_WIN eWindow)
{
    UNUSED(u32DNRBase2);
    UNUSED(eWindow);
}

void Hal_SC_set_OPMBase0(MS_U32 u32OPMBase0, SCALER_WIN eWindow)
{
    if(eWindow == MAIN_WINDOW)
    {
        SC_W4BYTE(REG_SC_BK12_10_L, u32OPMBase0 / BYTE_PER_WORD);     // input address0
    }
    else if(eWindow == SUB_WINDOW)
    {
        SC_W4BYTE(REG_SC_BK12_50_L, u32OPMBase0 / BYTE_PER_WORD);     // input address0
    }
    else if(eWindow == SC1_MAIN_WINDOW)
    {
        SC_W4BYTE(REG_SC1_BK12_10_L, u32OPMBase0 / BYTE_PER_WORD);     // input address0
    }
    else if(eWindow == SC2_MAIN_WINDOW)
    {
        SC_W4BYTE(REG_SC2_BK12_10_L, u32OPMBase0 / BYTE_PER_WORD);     // input address0
    }
    else if(eWindow == SC2_SUB_WINDOW)
    {
        SC_W4BYTE(REG_SC2_BK12_50_L, u32OPMBase0 / BYTE_PER_WORD);     // input address0
    }
    else
    {
    }

}

void Hal_SC_set_OPMBase1(MS_U32 u32OPMBase1, SCALER_WIN eWindow)
{
    if(eWindow == MAIN_WINDOW)
    {
        SC_W4BYTE(REG_SC_BK12_12_L, u32OPMBase1 / BYTE_PER_WORD);     // input address1
    }
    else if(eWindow == SUB_WINDOW)
    {
        SC_W4BYTE(REG_SC_BK12_52_L, u32OPMBase1 / BYTE_PER_WORD);     // input address1
    }
    else if(eWindow == SC1_MAIN_WINDOW)
    {
        SC_W4BYTE(REG_SC1_BK12_12_L, u32OPMBase1 / BYTE_PER_WORD);     // input address1
    }
    else if(eWindow == SC2_MAIN_WINDOW)
    {
        SC_W4BYTE(REG_SC2_BK12_12_L, u32OPMBase1 / BYTE_PER_WORD);     // input address1
    }
    else if(eWindow == SC2_SUB_WINDOW)
    {
        SC_W4BYTE(REG_SC2_BK12_52_L, u32OPMBase1 / BYTE_PER_WORD);     // input address1
    }
    else
    {
    }

}

void Hal_SC_set_OPMBase2(MS_U32 u32OPMBase2, SCALER_WIN eWindow)
{
    UNUSED(u32OPMBase2);
    UNUSED(eWindow);
}

void Hal_SC_set_WriteVLength(SCALER_WIN eWindow)
{
    if(eWindow == MAIN_WINDOW)
    {
        SC_W2BYTE(REG_SC_BK20_15_L, 0x0);     // Write V length
    }
    else if(eWindow == SUB_WINDOW)
    {
        SC_W2BYTE(REG_SC_BK20_16_L, 0x0);     // Write V length
    }
    else if(eWindow == SC1_MAIN_WINDOW)
    {
        SC_W2BYTE(REG_SC1_BK20_15_L, 0x0);     // Write V length
    }
    else if(eWindow == SC2_MAIN_WINDOW)
    {
        SC_W2BYTE(REG_SC1_BK20_15_L, 0x0);     // Write V length
    }
    else if(eWindow == SC2_SUB_WINDOW)
    {
        SC_W2BYTE(REG_SC2_BK20_16_L, 0x0);     // Write V length
    }
    else
    {
    }

}

void Hal_SC_set_memoryaddress(MS_U32 u32DNRBase0, MS_U32 u32DNRBase1, MS_U32 u32DNRBase2, MS_U32 u32OPMBase0, MS_U32 u32OPMBase1, MS_U32 u32OPMBase2, SCALER_WIN eWindow)
{
    Hal_SC_set_DNRBase0(u32DNRBase0, eWindow);
    Hal_SC_set_DNRBase1(u32DNRBase1, eWindow);
    Hal_SC_set_DNRBase2(u32DNRBase2, eWindow);
    Hal_SC_set_OPMBase0(u32OPMBase0, eWindow);
    Hal_SC_set_OPMBase1(u32OPMBase1, eWindow);
    Hal_SC_set_OPMBase2(u32OPMBase2, eWindow);
    Hal_SC_set_WriteVLength(eWindow); // Write V length
}

static SC_MIUMASK_t Hal_SC_get_miu0mask(void)
{
    SC_MIUMASK_t mask;

    mask.u16MiuG0Mask = MDrv_Read2Byte(MIU0_G0_REQUEST_MASK);
    mask.u16MiuG1Mask = MDrv_Read2Byte(MIU0_G1_REQUEST_MASK);
    mask.u16MiuG2Mask = MDrv_Read2Byte(MIU0_G2_REQUEST_MASK);
    mask.u16MiuG3Mask = MDrv_Read2Byte(MIU0_G3_REQUEST_MASK);

    return mask;
}

static SC_MIUMASK_t Hal_SC_get_miu1mask(void)
{
    SC_MIUMASK_t mask;

    mask.u16MiuG0Mask = MDrv_Read2Byte(MIU1_G0_REQUEST_MASK);
    mask.u16MiuG1Mask = MDrv_Read2Byte(MIU1_G1_REQUEST_MASK);
    mask.u16MiuG2Mask = MDrv_Read2Byte(MIU1_G2_REQUEST_MASK);
    mask.u16MiuG3Mask = MDrv_Read2Byte(MIU1_G3_REQUEST_MASK);

    return mask;
}

static void Hal_SC_set_miu0mask(SC_MIUMASK_t mask)
{
    MDrv_Write2Byte(MIU0_G0_REQUEST_MASK, mask.u16MiuG0Mask);
    MDrv_Write2Byte(MIU0_G1_REQUEST_MASK, mask.u16MiuG1Mask);
    MDrv_Write2Byte(MIU0_G2_REQUEST_MASK, mask.u16MiuG2Mask);
    MDrv_Write2Byte(MIU0_G3_REQUEST_MASK, mask.u16MiuG3Mask);
}

static void Hal_SC_set_miu1mask(SC_MIUMASK_t mask)
{
    MDrv_Write2Byte(MIU1_G0_REQUEST_MASK, mask.u16MiuG0Mask);
    MDrv_Write2Byte(MIU1_G1_REQUEST_MASK, mask.u16MiuG1Mask);
    MDrv_Write2Byte(MIU1_G2_REQUEST_MASK, mask.u16MiuG2Mask);
    MDrv_Write2Byte(MIU1_G3_REQUEST_MASK, mask.u16MiuG3Mask);
}

void Hal_SC_Enable_MiuMask(void)
{
    Miu0MaskOld = Hal_SC_get_miu0mask();
    Miu1MaskOld = Hal_SC_get_miu1mask();

    Miu0Mask = Miu0MaskOld;
    Miu1Mask = Miu1MaskOld;

    Miu0Mask.u16MiuG0Mask |= MIU_SC_G0REQUEST_MASK;
    Miu0Mask.u16MiuG1Mask |= MIU_SC_G1REQUEST_MASK;
    Miu0Mask.u16MiuG2Mask |= MIU_SC_G2REQUEST_MASK;
    Miu0Mask.u16MiuG3Mask |= MIU_SC_G3REQUEST_MASK;

    Miu1Mask.u16MiuG0Mask |= MIU_SC_G0REQUEST_MASK;
    Miu1Mask.u16MiuG1Mask |= MIU_SC_G1REQUEST_MASK;
    Miu1Mask.u16MiuG2Mask |= MIU_SC_G2REQUEST_MASK;
    Miu1Mask.u16MiuG3Mask |= MIU_SC_G3REQUEST_MASK;

    Hal_SC_set_miu0mask(Miu0Mask);
    Hal_SC_set_miu1mask(Miu1Mask);
}

void Hal_SC_Disable_MiuMask(void)
{
    Hal_SC_set_miu0mask(Miu0MaskOld);
    Hal_SC_set_miu1mask(Miu1MaskOld);
}

void Hal_SC_set_miusel(MS_U8 u8MIUSel)
{
    // Scaler control MIU by itself
    // So MIU1 IP-select is set to 1. At this moment, MIU0 Miu select is not working.
    if (u8MIUSel==0)
    {
        SC_W2BYTEMSK(REG_SC_BK12_05_L, 0x0000, 0x0010);     // reg_miu_select_f2 to MIU0
        SC_W2BYTEMSK(REG_SC_BK12_44_L, 0x0000, 0x4000);     // reg_miu_select_f1 to MIU0

        MDrv_WriteByteMask(REG_MIU0_BASE + 0xF6, 0x00, 0xC0); // MIU select (Group3 BIT6 BIT7 BIT8)
        MDrv_WriteByteMask(REG_MIU0_BASE + 0xF7, 0x00, 0x01); // MIU select (Group3 BIT6 BIT7 BIT8)

        MDrv_WriteByteMask(REG_MIU1_BASE + 0xF6, 0xC0, 0xC0); // IP select
        MDrv_WriteByteMask(REG_MIU1_BASE + 0xF7, 0x01, 0x01); // IP select
    }
    else
    {
        SC_W2BYTEMSK(REG_SC_BK12_05_L, 0x0010, 0x0010);     // reg_miu_select_f2 to MIU1
        SC_W2BYTEMSK(REG_SC_BK12_44_L, 0x4000, 0x4000);     // reg_miu_select_f1 to MIU1

        MDrv_WriteByteMask(REG_MIU0_BASE + 0xF6, 0x00, 0xC0);
        MDrv_WriteByteMask(REG_MIU0_BASE + 0xF7, 0x00, 0x01);

        MDrv_WriteByteMask(REG_MIU1_BASE + 0xF6, 0xC0, 0xC0);
        MDrv_WriteByteMask(REG_MIU1_BASE + 0xF7, 0x01, 0x01);
    }
}

void Hal_SC_set_blsk(MS_BOOL bEn, SCALER_WIN eWindow)
{
    MS_U32 u32Reg_21;

    u32Reg_21 = eWindow == MAIN_WINDOW     ? REG_SC_BK10_21_L :
                eWindow == SUB_WINDOW      ? REG_SC_BK10_21_L :
                eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK10_21_L :
                eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK10_21_L :
                eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK10_21_L :
                                             REG_SC_DUMMY;

    SC_W2BYTEMSK(u32Reg_21, bEn ? BIT(12) : 0, BIT(12));
}

void Hal_SC_set_blsk_burst(MS_BOOL bEn, SCALER_WIN eWindow)
{
    MS_U32 u32Reg_21;

    _MLOAD_ENTRY();
    u32Reg_21 = eWindow == MAIN_WINDOW     ? REG_SC_BK10_21_L :
                eWindow == SUB_WINDOW      ? REG_SC_BK10_21_L :
                eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK10_21_L :
                eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK10_21_L :
                eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK10_21_L :
                                             REG_SC_DUMMY;

    MApi_XC_MLoad_WriteCmd(u32Reg_21, bEn ? BIT(12) : 0, BIT(12));
    MApi_XC_MLoad_Fire(TRUE);
    _MLOAD_RETURN();
}

void Hal_SC_set_main_black_screen_burst(MS_BOOL bEn, SCALER_WIN eWindow)
{
    MS_U32 u32Reg_19;

    _MLOAD_ENTRY();

    u32Reg_19 = eWindow == MAIN_WINDOW     ? REG_SC_BK10_19_L :
                eWindow == SUB_WINDOW      ? REG_SC_BK10_19_L :
                eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK10_19_L :
                eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK10_19_L :
                eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK10_19_L :
                                             REG_SC_DUMMY;

    MApi_XC_MLoad_WriteCmd(u32Reg_19, bEn ? BIT(1) : 0, BIT(1));
    MApi_XC_MLoad_Fire(TRUE);

    _MLOAD_RETURN();
}

void Hal_SC_set_main_sub_black_screen_burst(MS_BOOL bEn, SCALER_WIN eWindow)
{
    MS_U32 u32Reg_19;

    _MLOAD_ENTRY();

    u32Reg_19 = eWindow == MAIN_WINDOW     ? REG_SC_BK10_19_L :
                eWindow == SUB_WINDOW      ? REG_SC_BK10_19_L :
                eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK10_19_L :
                eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK10_19_L :
                eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK10_19_L :
                                             REG_SC_DUMMY;

    MApi_XC_MLoad_WriteCmd(u32Reg_19, bEn ? (BIT(5)|BIT(1)) : 0, (BIT(6)|BIT(5)|BIT(1)));
    MApi_XC_MLoad_Fire(TRUE);

    _MLOAD_RETURN();
}

void Hal_SC_set_main_black_screen(MS_BOOL bEn, SCALER_WIN eWindow)
{
    MS_U32 u32Reg_19;
    u32Reg_19 = eWindow == MAIN_WINDOW     ? REG_SC_BK10_19_L :
                eWindow == SUB_WINDOW      ? REG_SC_BK10_19_L :
                eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK10_19_L :
                eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK10_19_L :
                eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK10_19_L :
                                             REG_SC_DUMMY;

    SC_W2BYTEMSK(u32Reg_19, bEn ? BIT(1) : 0, BIT(1));
}

void Hal_SC_set_main_sub_black_screen(MS_BOOL bEn, SCALER_WIN eWindow)
{
    MS_U32 u32Reg_19;

    u32Reg_19 = eWindow == MAIN_WINDOW     ? REG_SC_BK10_19_L :
                eWindow == SUB_WINDOW      ? REG_SC_BK10_19_L :
                eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK10_19_L :
                eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK10_19_L :
                eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK10_19_L :
                                             REG_SC_DUMMY;

    SC_W2BYTEMSK(u32Reg_19, bEn ? (BIT(5)|BIT(1)) : 0, (BIT(6)|BIT(5)|BIT(1)));
}

void Hal_SC_set_sub_blue_screen_burst(MS_BOOL bEn, MS_BOOL bBlue, SCALER_WIN eWindow)
{
    MS_U32 u32Reg_19;

    _MLOAD_ENTRY();

    u32Reg_19 = eWindow == MAIN_WINDOW     ? REG_SC_BK10_19_L :
                eWindow == SUB_WINDOW      ? REG_SC_BK10_19_L :
                eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK10_19_L :
                eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK10_19_L :
                eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK10_19_L :
                                             REG_SC_DUMMY;

    MApi_XC_MLoad_WriteCmd(u32Reg_19, bBlue ? BIT(6) : 0, BIT(6));
    MApi_XC_MLoad_WriteCmd(u32Reg_19, bEn ? BIT(5) : 0, BIT(5));
    MApi_XC_MLoad_Fire(TRUE);

    _MLOAD_RETURN();
}

void Hal_SC_set_sub_blue_screen(MS_BOOL bEn, MS_BOOL bBlue, SCALER_WIN eWindow)
{
    MS_U32 u32Reg_19;

    u32Reg_19 = eWindow == MAIN_WINDOW     ? REG_SC_BK10_19_L :
                eWindow == SUB_WINDOW      ? REG_SC_BK10_19_L :
                eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK10_19_L :
                eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK10_19_L :
                eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK10_19_L :
                                             REG_SC_DUMMY;

    SC_W2BYTEMSK(u32Reg_19, bBlue ? BIT(6) : 0, BIT(6));
    SC_W2BYTEMSK(u32Reg_19, bEn ? BIT(5) : 0, BIT(5));
}

E_APIXC_ReturnValue Hal_SC_memory_write_request(MS_BOOL bEnable, SCALER_WIN eWindow)
{
    MS_U32 u32Reg_17;

    u32Reg_17 = eWindow == MAIN_WINDOW     ? REG_SC_BK12_07_L :
                eWindow == SUB_WINDOW      ? REG_SC_BK12_47_L :
                eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK12_07_L :
                eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK12_07_L :
                eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK12_47_L :
                                             REG_SC_DUMMY;
    SC_W2BYTEMSK(u32Reg_17, bEnable? 0 : BIT(0) , BIT(0));

    return E_APIXC_RET_OK;
}

E_APIXC_ReturnValue Hal_SC_memory_read_request(MS_BOOL bEnable, SCALER_WIN eWindow)
{
    MS_U32 u32Reg_17;

    u32Reg_17 = eWindow == MAIN_WINDOW     ? REG_SC_BK12_07_L :
                eWindow == SUB_WINDOW      ? REG_SC_BK12_47_L :
                eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK12_07_L :
                eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK12_07_L :
                eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK12_47_L :
                                             REG_SC_DUMMY;

    SC_W2BYTEMSK(u32Reg_17, bEnable? 0 : BIT(1) , BIT(1));

    return E_APIXC_RET_OK;
}

void Hal_SC_disable_inputsource_burst(MS_BOOL bDisable, SCALER_WIN eWindow)
{
    _MLOAD_ENTRY();

    // HW PIP architeucture
    // Becasue BK3_02[8] and BK20_11[15] can not be enabled toghter,
    // otherwise garbage will be showed,we need to use BK12_47[0] to instead.
    if( eWindow == MAIN_WINDOW )
    {
        MApi_XC_MLoad_WriteCmd(REG_SC_BK01_02_L, (bDisable ? BIT(7):0), BIT(7));
    }
    else if( eWindow == SUB_WINDOW )
    {
    	//!! to enabe sub input source directly, 20120723 !!//
        MApi_XC_MLoad_WriteCmd(REG_SC_BK03_02_L, (bDisable ? BIT(7):0), BIT(7));
        ////MApi_XC_MLoad_WriteCmd(REG_SC_BK12_47_L, (bDisable ? BIT(0) : 0), BIT(0));
    }
    else if( eWindow == SC1_MAIN_WINDOW)
    {
        MApi_XC_MLoad_WriteCmd(REG_SC1_BK01_02_L, (bDisable ? BIT(7):0), BIT(7));
    }
    else if( eWindow == SC2_MAIN_WINDOW)
    {
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK01_02_L, (bDisable ? BIT(7):0), BIT(7));
    }
    else if( eWindow == SC2_SUB_WINDOW)
    {
        MApi_XC_MLoad_WriteCmd(REG_SC2_BK12_47_L, (bDisable ? BIT(0) : 0), BIT(0));
    }
    else
    {
    }

    MApi_XC_MLoad_Fire(TRUE);
    _MLOAD_RETURN();
}

void Hal_SC_disable_inputsource(MS_BOOL bDisable, SCALER_WIN eWindow)
{
    MS_U32 u32Reg_02;

    u32Reg_02 = eWindow == MAIN_WINDOW     ? REG_SC_BK01_02_L :
                eWindow == SUB_WINDOW      ? REG_SC_BK03_02_L :
                eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_02_L :
                eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_02_L :
                eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_02_L :
                                             REG_SC_DUMMY;

    SC_W2BYTEMSK(u32Reg_02, (bDisable ? BIT(7):0), BIT(7));
}

MS_U16 Hal_SC_Is_InputSource_Disable(SCALER_WIN eWindow)
{
    MS_U32 u32Reg_02;

    u32Reg_02 = eWindow == MAIN_WINDOW     ? REG_SC_BK01_02_L :
                eWindow == SUB_WINDOW      ? REG_SC_BK03_02_L :
                eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_02_L :
                eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_02_L :
                eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_02_L :
                                             REG_SC_DUMMY;
    return (SC_R2BYTE(u32Reg_02)&0x80);
}

void Hal_SC_set_nosignal_color(MS_U8 u8Color,SCALER_WIN eWindow)
{
    if ( eWindow == MAIN_WINDOW )
    {
#ifdef new_chakra
        SC_W2BYTEMSK(REG_SC_BK10_24_L, u8Color, LBMASK);
#else
        SC_W2BYTEMSK(REG_SC_BK10_24_L, u8Color, LBMASK);
#endif
    }
    else if( eWindow == SUB_WINDOW)
    {
        SC_W2BYTEMSK(REG_SC_BK0F_17_L, ((MS_U16)u8Color)<<8, HBMASK);
    }
    else if( eWindow == SC1_MAIN_WINDOW)
    {
        SC_W2BYTEMSK(REG_SC1_BK10_24_L, u8Color, LBMASK);
    }
    else if( eWindow == SC2_MAIN_WINDOW)
    {
        SC_W2BYTEMSK(REG_SC2_BK10_24_L, u8Color, LBMASK);
    }
    else if( eWindow == SC2_SUB_WINDOW)
    {
        SC_W2BYTEMSK(REG_SC2_BK0F_17_L, ((MS_U16)u8Color)<<8, HBMASK);
    }
    else
    {
    }

}

void Hal_SC_set_fbl(MS_BOOL bEn, SCALER_WIN eWindow)
{
    MS_U32 u32Reg_01, u32Reg_02, u32Reg_07;

    if(eWindow == MAIN_WINDOW || eWindow == SUB_WINDOW)
    {
        u32Reg_01 = REG_SC_BK12_01_L;
        u32Reg_02 = REG_SC_BK12_02_L;
        u32Reg_07 = REG_SC_BK12_07_L;
    }
    else if(eWindow == SC1_MAIN_WINDOW || eWindow == SC1_SUB_WINDOW)
    {
        u32Reg_01 = REG_SC1_BK12_01_L;
        u32Reg_02 = REG_SC1_BK12_02_L;
        u32Reg_07 = REG_SC1_BK12_07_L;
    }
    else if(eWindow == SC2_MAIN_WINDOW || eWindow == SC2_SUB_WINDOW)
    {
        u32Reg_01 = REG_SC2_BK12_01_L;
        u32Reg_02 = REG_SC2_BK12_02_L;
        u32Reg_07 = REG_SC2_BK12_07_L;
    }
    else
    {
        u32Reg_01 = REG_SC_DUMMY;
        u32Reg_02 = REG_SC_DUMMY;
        u32Reg_07 = REG_SC_DUMMY;
    }


    SC_W2BYTEMSK(u32Reg_01, (bEn ? BIT(7): 0), BIT(7));  //Enable/Disable FBL
    SC_W2BYTEMSK(u32Reg_02, (bEn ? BIT(14):0), BIT(14)); //F2 force IP read request disable

    SC_W2BYTEMSK(u32Reg_07, (bEn ? 0x3:0), 0x3); //F2 IP read/write request disable
}

MS_BOOL Hal_SC_get_fbl(SCALER_WIN eWindow)
{
    MS_U32 u32Reg_01;

    u32Reg_01 = eWindow == MAIN_WINDOW     ? REG_SC_BK12_01_L :
                eWindow == SUB_WINDOW      ? REG_SC_BK12_01_L :
                eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK12_01_L :
                eWindow == SC1_SUB_WINDOW  ? REG_SC1_BK12_01_L :
                eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK12_01_L :
                                             REG_SC_DUMMY;

    return SC_R2BYTEMSK(u32Reg_01, BIT(7));  //Enable/Disable FBL
}

void Hal_SC_set_freezeimg_burst(MS_BOOL bEn, SCALER_WIN eWindow)
{
    MS_U32 u32Reg_01;

    _MLOAD_ENTRY();

    u32Reg_01 = eWindow == MAIN_WINDOW     ? REG_SC_BK12_01_L :
                eWindow == SUB_WINDOW      ? REG_SC_BK12_41_L :
                eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK12_01_L :
                eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK12_01_L :
                eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK12_41_L :
                                             REG_SC_DUMMY;

    MApi_XC_MLoad_WriteCmd(u32Reg_01, (bEn<<11), BIT(11));

    MApi_XC_MLoad_Fire(TRUE);
    _MLOAD_RETURN();
}


void Hal_SC_set_freezeimg(MS_BOOL bEn, SCALER_WIN eWindow)
{
    MS_U32 u32Reg_01;

    u32Reg_01 = eWindow == MAIN_WINDOW     ? REG_SC_BK12_01_L :
                eWindow == SUB_WINDOW      ? REG_SC_BK12_41_L :
                eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK12_01_L :
                eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK12_01_L :
                eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK12_41_L :
                                             REG_SC_DUMMY;
    SC_W2BYTEMSK(u32Reg_01, (bEn<<11), BIT(11));
}

MS_BOOL Hal_SC_get_freezeimg(SCALER_WIN eWindow)
{
    MS_U32 u32Reg_01;

    u32Reg_01 = eWindow == MAIN_WINDOW     ? REG_SC_BK12_01_L :
                eWindow == SUB_WINDOW      ? REG_SC_BK12_41_L :
                eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK12_01_L :
                eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK12_01_L :
                eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK12_41_L :
                                             REG_SC_DUMMY;

    return (SC_R2BYTEMSK(u32Reg_01, BIT(11)) ? TRUE: FALSE);
}

void Hal_SC_exwith_miuprotect_swreset(MS_U8 x)
{
    //reset IP1F2
    //select pattern generator source
    SC_W2BYTEMSK(REG_SC_BK00_02_L, x, x);
    //MsOS_DelayTask(2);

    //select pattern generator source
    SC_W2BYTEMSK(REG_SC_BK00_02_L, 0, x);
    //MsOS_DelayTask(2);
}

void Hal_SC_ip_Init_for_internal_timing(XC_Internal_TimingType timingtype)
{

    SC_W2BYTE(REG_SC_BK01_02_L, 0x104) ;  //Video source
    Hal_SC_exwith_miuprotect_swreset(REST_IP_F2);  //when source is changed, we must reset it to make it work
    SC_W2BYTE(REG_SC_BK01_03_L, 0x890);

    SC_W2BYTEMSK(REG_SC_BK01_21_L, 0x01, 0x03);   //force progressive mode
    SC_W2BYTEMSK(REG_SC_BK01_0E_L, BIT(15), BIT(15));   //auto gain 10bit

    SC_W2BYTE(REG_SC_BK01_04_L, 0x020);  //capture V start
    SC_W2BYTE(REG_SC_BK01_05_L, 0x50) ;  //capture H start

    switch (timingtype)
    {
        case E_XC_480P:
        default:

            SC_W2BYTE(REG_SC_BK01_06_L, 0x1c0) ; // V size
            SC_W2BYTE(REG_SC_BK01_07_L, 0x280) ; // h size

            break;
        case E_XC_720P:
            SC_W2BYTE(REG_SC_BK01_06_L, 0x290) ; // V size (720 - 64) = 656 = 0x290
            SC_W2BYTE(REG_SC_BK01_07_L, 0x460) ; // h size (1280 - 160) = 1120 = 0x460

            SC_W2BYTE(REG_SC_BK12_0E_L, 0x0800); //DNR Offset
            SC_W2BYTE(REG_SC_BK12_0F_L, 0x0460); //DNR Fetch
            SC_W2BYTE(REG_SC_BK12_16_L, 0x0800); //OPM Offsest
            SC_W2BYTE(REG_SC_BK12_17_L, 0x0460); //OPM Fetch

            break;
        case E_XC_1080P:
            SC_W2BYTE(REG_SC_BK01_06_L, 0x3F8) ; // V size (1080 - 64) = 1016 = 0x3F8
            SC_W2BYTE(REG_SC_BK01_07_L, 0x6E0) ; // h size (1920 - 160) = 1760 = 0x6E0

            SC_W2BYTE(REG_SC_BK12_0E_L, 0x0800); //DNR Offset
            SC_W2BYTE(REG_SC_BK12_0F_L, 0x06E0); //DNR Fetch
            SC_W2BYTE(REG_SC_BK12_16_L, 0x0800); //OPM Offsest
            SC_W2BYTE(REG_SC_BK12_17_L, 0x06E0); //OPM Fetch
            break;
    }
}

MS_U16 Hal_SC_rgb_average_info(MS_U16 u16mask, SCALER_WIN eWindow)
{
    MS_U16 u16value;
    MS_U32 u32Reg_0F, u32Reg_11;

    if(eWindow == MAIN_WINDOW)
    {
        u32Reg_0F = REG_SC_BK01_0F_L;
        u32Reg_11 = REG_SC_BK01_11_L;
    }
    else if(eWindow == SUB_WINDOW)
    {
        u32Reg_0F = REG_SC_BK03_0F_L;
        u32Reg_11 = REG_SC_BK03_11_L;
    }
    else if(eWindow == SC1_MAIN_WINDOW)
    {
        u32Reg_0F = REG_SC1_BK01_0F_L;
        u32Reg_11 = REG_SC1_BK01_11_L;
    }
    else if(eWindow == SC2_MAIN_WINDOW)
    {
        u32Reg_0F = REG_SC2_BK01_0F_L;
        u32Reg_11 = REG_SC2_BK01_11_L;
    }
    else if(eWindow == SC2_SUB_WINDOW)
    {
        u32Reg_0F = REG_SC2_BK03_0F_L;
        u32Reg_11 = REG_SC2_BK03_11_L;
    }
    else
    {
        u32Reg_0F = REG_SC_DUMMY;
        u32Reg_11 = REG_SC_DUMMY;

    }

    SC_W2BYTEMSK(u32Reg_0F, u16mask, 0x0F);
    u16value=SC_R2BYTE(u32Reg_11);

    return u16value;
}


void Hal_SC_autogain_enable(MS_BOOL bEnable, SCALER_WIN eWindow)
{
    //auto gain enable
    MS_U32 u32Reg_0E;

    u32Reg_0E = eWindow == MAIN_WINDOW     ? REG_SC_BK01_0E_L :
                eWindow == SUB_WINDOW      ? REG_SC_BK03_0E_L :
                eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_0E_L :
                eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_0E_L :
                eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_0E_L :
                                             REG_SC_DUMMY;

    if(bEnable)
    {
        W2BYTEMSK(u32Reg_0E, 0x11, 0x11);
    }
    else
    {
        W2BYTEMSK(u32Reg_0E, 0x00, 0x11);
    }
}

MS_BOOL Hal_SC_autogain_status(SCALER_WIN eWindow)
{
    MS_BOOL bvalue;
    MS_U32 u32Reg_0E;

    u32Reg_0E = eWindow == MAIN_WINDOW     ? REG_SC_BK01_0E_L :
                eWindow == SUB_WINDOW      ? REG_SC_BK03_0E_L :
                eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_0E_L :
                eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_0E_L :
                eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_0E_L :
                                             REG_SC_DUMMY;

    bvalue = SC_R2BYTEMSK(u32Reg_0E, BIT(1));

    return bvalue;

}

void Hal_SC_set_mirror(MS_BOOL bEnable, SCALER_WIN eWindow)
{
    MS_U32 u32NoSigReg, u32MirrorReg;

    if(eWindow == MAIN_WINDOW)
    {
        u32NoSigReg = REG_SC_BK01_02_L;
        u32MirrorReg = REG_SC_BK12_03_L;
    }
    else if(eWindow == SUB_WINDOW)
    {
        u32NoSigReg = REG_SC_BK03_02_L;
        u32MirrorReg = REG_SC_BK12_43_L;
    }
    else if(eWindow == SC1_MAIN_WINDOW)
    {
        u32NoSigReg = REG_SC1_BK01_02_L;
        u32MirrorReg = REG_SC1_BK12_03_L;
    }
    else if(eWindow == SC2_MAIN_WINDOW)
    {
        u32NoSigReg = REG_SC2_BK01_02_L;
        u32MirrorReg = REG_SC2_BK12_03_L;
    }
    else if(eWindow == SC2_SUB_WINDOW)
    {
        u32NoSigReg = REG_SC2_BK03_02_L;
        u32MirrorReg = REG_SC2_BK12_43_L;
    }
    else
    {
        u32NoSigReg = REG_SC_DUMMY;
        u32MirrorReg = REG_SC_DUMMY;
    }


    SC_W2BYTEMSK(u32NoSigReg, BIT(7), BIT(7));

    if( bEnable )
    {
        SC_W2BYTEMSK(u32MirrorReg, (BIT(12)|BIT(13)), (BIT(12)|BIT(13)));
    }
    else
    {
        SC_W2BYTEMSK(u32MirrorReg, 0x0, (BIT(12)|BIT(13)));
    }
}

MS_U16 Hal_SC_GetAVDStatus(void)
{
    return MDrv_Read2Byte(L_BK_AFEC(0x66));
}

void Hal_SC_set_rep_window(MS_BOOL bEnable,MS_U16 x,MS_U16 y,MS_U16 w,MS_U16 h,MS_U8 u8Color, SCALER_WIN eWindow)
{
    MS_U32 u32Reg_57, u32Reg_42, u32Reg_43, u32Reg_44, u32Reg_45, u32Reg_46;

    if(eWindow == MAIN_WINDOW || eWindow == SUB_WINDOW)
    {
        u32Reg_57 = REG_SC_BK0F_57_L;
        u32Reg_42 = REG_SC_BK0F_42_L;
        u32Reg_43 = REG_SC_BK0F_43_L;
        u32Reg_44 = REG_SC_BK0F_44_L;
        u32Reg_45 = REG_SC_BK0F_45_L;
        u32Reg_46 = REG_SC_BK0F_46_L;
    }
    else if(eWindow == SC1_MAIN_WINDOW || eWindow == SC1_SUB_WINDOW)
    {
        u32Reg_57 = REG_SC1_BK0F_57_L;
        u32Reg_42 = REG_SC1_BK0F_42_L;
        u32Reg_43 = REG_SC1_BK0F_43_L;
        u32Reg_44 = REG_SC1_BK0F_44_L;
        u32Reg_45 = REG_SC1_BK0F_45_L;
        u32Reg_46 = REG_SC1_BK0F_46_L;
    }
    else if(eWindow == SC2_MAIN_WINDOW || eWindow == SC2_SUB_WINDOW)
    {
        u32Reg_57 = REG_SC2_BK0F_57_L;
        u32Reg_42 = REG_SC2_BK0F_42_L;
        u32Reg_43 = REG_SC2_BK0F_43_L;
        u32Reg_44 = REG_SC2_BK0F_44_L;
        u32Reg_45 = REG_SC2_BK0F_45_L;
        u32Reg_46 = REG_SC2_BK0F_46_L;
    }
    else
    {
        u32Reg_57 = REG_SC_DUMMY;
        u32Reg_42 = REG_SC_DUMMY;
        u32Reg_43 = REG_SC_DUMMY;
        u32Reg_44 = REG_SC_DUMMY;
        u32Reg_45 = REG_SC_DUMMY;
        u32Reg_46 = REG_SC_DUMMY;
    }

    SC_W2BYTEMSK(u32Reg_57, bEnable?BIT(9):0x0, BIT(9));
    SC_W2BYTE(u32Reg_42,x);
    SC_W2BYTE(u32Reg_43,x + w);
    SC_W2BYTE(u32Reg_44,y);
    SC_W2BYTE(u32Reg_45,y + h);
    SC_W2BYTEMSK(u32Reg_46, (~u8Color)<<8, 0xFF00);

}

//--------------------------------------------------
//update display window registers with input window
//IN:
//   pdspwin: input window info
//--------------------------------------------------
void Hal_SC_set_disp_window(SCALER_WIN eWindow, MS_WINDOW_TYPE *pdspwin)
{
    MS_U32 u32RegH_start;
    MS_U32 u32RegH_end;
    MS_U32 u32RegV_start;
    MS_U32 u32RegV_end;

    if(eWindow == MAIN_WINDOW)
    {
        u32RegH_start = REG_SC_BK10_08_L;
        u32RegH_end   = REG_SC_BK10_09_L;
        u32RegV_start = REG_SC_BK10_0A_L;
        u32RegV_end   = REG_SC_BK10_0B_L;
    }
    else if(eWindow == SUB_WINDOW)
    {
        u32RegH_start = REG_SC_BK0F_07_L;
        u32RegH_end   = REG_SC_BK0F_08_L;
        u32RegV_start = REG_SC_BK0F_09_L;
        u32RegV_end   = REG_SC_BK0F_0A_L;
    }
    else if(eWindow == SC1_MAIN_WINDOW)
    {
        u32RegH_start = REG_SC1_BK10_08_L;
        u32RegH_end   = REG_SC1_BK10_09_L;
        u32RegV_start = REG_SC1_BK10_0A_L;
        u32RegV_end   = REG_SC1_BK10_0B_L;
    }
    else if(eWindow == SC2_MAIN_WINDOW)
    {
        u32RegH_start = REG_SC2_BK10_08_L;
        u32RegH_end   = REG_SC2_BK10_09_L;
        u32RegV_start = REG_SC2_BK10_0A_L;
        u32RegV_end   = REG_SC2_BK10_0B_L;
    }
    else if(eWindow == SC2_SUB_WINDOW)
    {
        u32RegH_start = REG_SC2_BK0F_07_L;
        u32RegH_end   = REG_SC2_BK0F_08_L;
        u32RegV_start = REG_SC2_BK0F_09_L;
        u32RegV_end   = REG_SC2_BK0F_0A_L;
    }
    else
    {
        u32RegH_start = REG_SC_DUMMY;
        u32RegH_end   = REG_SC_DUMMY;
        u32RegV_start = REG_SC_DUMMY;
        u32RegV_end   = REG_SC_DUMMY;
    }

    //Display window
    SC_W2BYTE(u32RegH_start, pdspwin->x);                  // Display H start
    SC_W2BYTE(u32RegH_end, pdspwin->x+pdspwin->width-1);   // Display H end
    SC_W2BYTE(u32RegV_start, pdspwin->y);                  // Display V start
    SC_W2BYTE(u32RegV_end, pdspwin->y+pdspwin->height-1);  // Display V end
}

//--------------------------------------------------
//get display window registers setting
//OUT:
//   pdspwin: Pointer for ouput disp window register
//--------------------------------------------------
void Hal_SC_get_disp_window(SCALER_WIN eWindow, MS_WINDOW_TYPE *pdspwin)
{
    MS_U32 u32RegH_start;
    MS_U32 u32RegH_end;
    MS_U32 u32RegV_start;
    MS_U32 u32RegV_end;

    if(eWindow == MAIN_WINDOW)
    {
        u32RegH_start = REG_SC_BK10_08_L;
        u32RegH_end   = REG_SC_BK10_09_L;
        u32RegV_start = REG_SC_BK10_0A_L;
        u32RegV_end   = REG_SC_BK10_0B_L;
    }
    else if(eWindow == SUB_WINDOW)
    {
        u32RegH_start = REG_SC_BK0F_07_L;
        u32RegH_end   = REG_SC_BK0F_08_L;
        u32RegV_start = REG_SC_BK0F_09_L;
        u32RegV_end   = REG_SC_BK0F_0A_L;
    }
    else if(eWindow == SC1_MAIN_WINDOW)
    {
        u32RegH_start = REG_SC1_BK10_08_L;
        u32RegH_end   = REG_SC1_BK10_09_L;
        u32RegV_start = REG_SC1_BK10_0A_L;
        u32RegV_end   = REG_SC1_BK10_0B_L;
    }
    else if(eWindow == SC2_MAIN_WINDOW)
    {
        u32RegH_start = REG_SC2_BK10_08_L;
        u32RegH_end   = REG_SC2_BK10_09_L;
        u32RegV_start = REG_SC2_BK10_0A_L;
        u32RegV_end   = REG_SC2_BK10_0B_L;
    }
    else if(eWindow == SC2_SUB_WINDOW)
    {
        u32RegH_start = REG_SC2_BK0F_07_L;
        u32RegH_end   = REG_SC2_BK0F_08_L;
        u32RegV_start = REG_SC2_BK0F_09_L;
        u32RegV_end   = REG_SC2_BK0F_0A_L;
    }
    else
    {
        u32RegH_start = REG_SC_DUMMY;
        u32RegH_end   = REG_SC_DUMMY;
        u32RegV_start = REG_SC_DUMMY;
        u32RegV_end   = REG_SC_DUMMY;
    }

    //Display window
    pdspwin->x = SC_R2BYTE(u32RegH_start);                   // Display H start
    pdspwin->width  = SC_R2BYTE(u32RegH_end) - pdspwin->x + 1; // Display H end
    pdspwin->y = SC_R2BYTE(u32RegV_start);                   // Display V start
    pdspwin->height = SC_R2BYTE(u32RegV_end) - pdspwin->y + 1; // Display V end

}

void Hal_SC_set_Fclk(EN_SET_FCLK_CASE enCase)
{
    if (enCase == EN_FCLK_FBL)
        MDrv_WriteByteMask(REG_CKG_FCLK, CKG_FCLK_216MHZ, CKG_FCLK_MASK);
    else
        MDrv_WriteByteMask(REG_CKG_FCLK, CKG_FCLK_192MHZ, CKG_FCLK_MASK);
}

void Hal_SC_set_Mclk(MS_BOOL bEn, SCALER_WIN eWindow)
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW      ? REG_SC_BK00_03_L :
             eWindow == SUB_WINDOW       ? REG_SC_BK00_03_L :
             eWindow == SC1_MAIN_WINDOW  ? REG_SC1_BK00_03_L :
             eWindow == SC2_MAIN_WINDOW  ? REG_SC2_BK00_03_L :
             eWindow == SC2_SUB_WINDOW   ? REG_SC2_BK00_03_L :
                                           REG_SC_DUMMY;

    if(bEn)
    {
        SC_W2BYTEMSK(u32Reg, BIT(7), BIT(7));
    }
    else
    {
        SC_W2BYTEMSK(u32Reg, 0, BIT(7));
    }
}


void Hal_SC1_set_ODCLK(MS_U8 u8val)
{
    // setup output dot clock
    MDrv_WriteByteMask(REG_CKG_SC1_ODCLK, u8val, CKG_SC1_ODCLK_MASK);      // select source tobe LPLL clock
    MDrv_WriteByteMask(REG_CKG_SC1_ODCLK, DISABLE, CKG_SC1_ODCLK_INVERT);               // clock not invert
    MDrv_WriteByteMask(REG_CKG_SC1_ODCLK, DISABLE, CKG_SC1_ODCLK_GATED);                // enable clock
}

void Hal_SC1_set_ODCLK_src(MS_BOOL bHDMI_Out, MS_HDMIT_OUTCLK_SRC_TYPE enHDMI_Out_Type)
{
    if(bHDMI_Out)
    {
        MS_BOOL bset = TRUE;
        MS_U16 u16HDMIPLL_CLK, u16HDMI_TX_CLK;

        switch(enHDMI_Out_Type)
        {
        case HDMI_OUT_480_I:
        case HDMI_OUT_576_I:
            u16HDMIPLL_CLK = CKG_SC_HDMIPLL_CLK_HDGEN_SRC_2;
            u16HDMI_TX_CLK = CKG_HDMI_TX_CLK_HDGEN_SRC_1;
            break;

        case HDMI_OUT_480_P:
            u16HDMIPLL_CLK = CKG_SC_HDMIPLL_CLK_HDGEN_SRC_1;
            u16HDMI_TX_CLK = CKG_HDMI_TX_CLK_HDGEN_SRC_1;
            break;
        case HDMI_OUT_720_P:
            u16HDMIPLL_CLK = CKG_SC_HDMIPLL_CLK_HDGEN_SRC_1;
            u16HDMI_TX_CLK = CKG_HDMI_TX_CLK_HDGEN_SRC_1;
            break;

        case HDMI_OUT_1080_I:
            u16HDMIPLL_CLK = CKG_SC_HDMIPLL_CLK_HDGEN_SRC_2;
            u16HDMI_TX_CLK = CKG_HDMI_TX_CLK_HDGEN_SRC_2;
            break;

        case HDMI_OUT_1080_P:
            u16HDMIPLL_CLK = CKG_SC_HDMIPLL_CLK_HDGEN_SRC_1;
            u16HDMI_TX_CLK = CKG_HDMI_TX_CLK_HDGEN_SRC_1;
            break;

        default:
            bset = FALSE;
            break;
        }

        if(bset)
        {
            MDrv_WriteByteMask(REG_CKG_HDGEN_SRC_SEL, CKG_HDGEN_SRC_SEL_CLK_HDMI_OUT_DAC, CKG_HDGEN_SRC_SEL_MASK);

            MDrv_WriteByteMask(REG_CKG_SC_HDMIPLL, u16HDMIPLL_CLK, CKG_SC_HDMIPLL_CLK_MSK);
            MDrv_WriteByteMask(REG_CKG_HDMI_TX_CLK, u16HDMI_TX_CLK, CKG_HDMI_TX_CLK_GATED);

            MDrv_WriteByteMask(REG_CKG_HDMI_TX_CLK, DISABLE, CKG_HDMI_TX_CLK_INVERT);
            MDrv_WriteByteMask(REG_CKG_HDMI_TX_CLK, DISABLE, CKG_HDMI_TX_CLK_GATED);
        }
    }
    else
    {
        MDrv_WriteByteMask(REG_CKG_HDGEN_SRC_SEL, CKG_HDGEN_SRC_SEL_CLK_HDMI_OUT_DAC, CKG_HDGEN_SRC_SEL_MASK);
        MDrv_WriteByteMask(REG_CKG_SC_HDMIPLL, CKG_SC_HDMIPLL_CLK_HDGEN_SRC_8, CKG_SC_HDMIPLL_CLK_MSK);

        MDrv_WriteByteMask(REG_CKG_VE_CLK, CKG_VE_CLK_27MHZ, CKG_VE_CLK_MASK);
        MDrv_WriteByteMask(REG_CKG_VE_CLK, DISABLE, CKG_VE_CLK_INVERT);
        MDrv_WriteByteMask(REG_CKG_VE_CLK, DISABLE, CKG_VE_CLK_GATED);

        MDrv_WriteByteMask(REG_CKG_VEDAC_CLK, CKG_VEDAC_CLK_108M, CKG_VEDAC_CLK_MASK);
        MDrv_WriteByteMask(REG_CKG_VEDAC_CLK, DISABLE, CKG_VEDAC_CLK_INVERT);
        MDrv_WriteByteMask(REG_CKG_VEDAC_CLK, DISABLE, CKG_VEDAC_CLK_GATED);

        MDrv_WriteByteMask(REG_CKG_DAC1_CLK, CKG_DAC1_CLK_VDEAC_DIGI, CKG_DAC1_CLK_MASK);
        MDrv_WriteByteMask(REG_CKG_DAC1_CLK, DISABLE, CKG_DAC1_CLK_INVERT);
        MDrv_WriteByteMask(REG_CKG_DAC1_CLK, DISABLE, CKG_DAC1_CLK_GATED);


    }

}


void Hal_SC2_set_ODCLK(MS_U32 u32SynthCtl, MS_U8 u8Synth)
{
    MS_U16 u16val_L, u16val_H;

    u16val_L = (MS_U16)(u32SynthCtl & 0xFFFF);
    u16val_H = (MS_U16)((u32SynthCtl & 0x3FFF0000) >> 16);
    MDrv_Write2Byte(REG_SC2_ODCLK_SYNTH_CTRL_0, u16val_L);
    MDrv_Write2Byte(REG_SC2_ODCLK_SYNTH_CTRL_1, u16val_H);

    MDrv_Write2Byte(REG_SC2_ODCLK_SYNTH_CTRL_1, u16val_H | 0x4000);
    MDrv_Write2Byte(REG_SC2_ODCLK_SYNTH_CTRL_1, u16val_H | 0x8000);

    MDrv_WriteByteMask(REG_CKG_SC2_ODCLK_SYN, u8Synth, CKG_SC2_ODCLK_SYN_MASK);
    MDrv_WriteByteMask(REG_CKG_SC2_ODCLK_SYN, DISABLE, CKG_SC2_ODCLK_SYN_INVERT);
    MDrv_WriteByteMask(REG_CKG_SC2_ODCLK_SYN, DISABLE, CKG_SC2_ODCLK_SYN_GATED);

    MDrv_WriteByteMask(REG_CKG_SC2_ODCLK, DISABLE, CKG_SC2_ODCLK_INVERT);
    MDrv_WriteByteMask(REG_CKG_SC2_ODCLK, DISABLE, CKG_SC2_ODCLK_GATE);
}

void Hal_SC_get_framebuf_Info(SC_FRAMEBUF_INFO_t *pFrameBufInfo)
{
    MS_U8 u8Reg;

    if (!pFrameBufInfo) return;

    pFrameBufInfo->u32IPMBase0 = ((SC_R2BYTEMSK(REG_SC_BK12_09_L, 0x00FF)<<16) |
                                    (SC_R2BYTE(REG_SC_BK12_08_L))) * BYTE_PER_WORD;
    pFrameBufInfo->u32IPMBase1 = ((SC_R2BYTEMSK(REG_SC_BK12_0B_L, 0x00FF)<<16) |
                                    (SC_R2BYTE(REG_SC_BK12_0A_L))) * BYTE_PER_WORD;
    pFrameBufInfo->u32IPMBase2 = ((SC_R2BYTEMSK(REG_SC_BK12_0D_L, 0x00FF)<<16) |
                                    (SC_R2BYTE(REG_SC_BK12_0C_L))) * BYTE_PER_WORD;
    pFrameBufInfo->u16IPMOffset = SC_R2BYTE(REG_SC_BK12_0E_L);
    pFrameBufInfo->u16IPMFetch = SC_R2BYTE(REG_SC_BK12_0F_L);
    pFrameBufInfo->u16VLength = SC_R2BYTEMSK(REG_SC_BK12_18_L, 0x0FFF);
    pFrameBufInfo->bLinearAddrMode = SC_R2BYTEMSK(REG_SC_BK12_03_L, BIT(4));
    pFrameBufInfo->bYCSeparate = ( SC_R2BYTEMSK(REG_SC_BK12_02_L, BIT(10)) )?TRUE:FALSE;
    pFrameBufInfo->u32WriteLimitBase = SC_R4BYTE(REG_SC_BK12_1A_L);

    u8Reg = SC_R2BYTEMSK(REG_SC_BK12_01_L, 0x00FF);
    if (u8Reg & 0x30)
    {//444
        pFrameBufInfo->bMemFormat422 = FALSE;
        if (u8Reg & 0x10)
        { // 444 8BIT
            pFrameBufInfo->u8BitPerPixel = 24;
        }
        else
        { // 444 10BIT
            pFrameBufInfo->u8BitPerPixel = 32;
        }
        pFrameBufInfo->bInterlace = FALSE;
    }
    else
    {//422
        pFrameBufInfo->bMemFormat422 = TRUE;
        u8Reg = SC_R2BYTEMSK(REG_SC_BK12_01_L, 0xFF00);

        switch(u8Reg & 0x77)
        {
            case 0x25:
            pFrameBufInfo->u8BitPerPixel = 24;
            pFrameBufInfo->bInterlace = TRUE;
            break;
	    case 0x24:
            pFrameBufInfo->u8BitPerPixel = 20;
            pFrameBufInfo->bInterlace = TRUE;
            break;
            case 0x27:
            pFrameBufInfo->u8BitPerPixel = 16;
            pFrameBufInfo->bInterlace = TRUE;
            break;
            case 0x21:
            pFrameBufInfo->u8BitPerPixel = 24;
            pFrameBufInfo->bInterlace = FALSE;
            break;
            case 0x00:
            default:
            pFrameBufInfo->u8BitPerPixel = 16;
            pFrameBufInfo->bInterlace = FALSE;
            break;
        }
    }

    //In M10/J2/A5/A6/A3, New SCMI
    pFrameBufInfo->u8FBNum = (MS_U8)(SC_R2BYTEMSK(REG_SC_BK12_19_L, 0x1F));

#if 0
    printf("base0=0x%x, base1=0x%x, base2=0x%x, offset=%u, 422=%u, i=%u, bpp=%u, fbnum=%u\n",
        (unsigned int)pFrameBufInfo->u32IPMBase0,
        (unsigned int)pFrameBufInfo->u32IPMBase1,
        (unsigned int)pFrameBufInfo->u32IPMBase2,
        pFrameBufInfo->u16IPMOffset,
        pFrameBufInfo->bMemFormat422,
        pFrameBufInfo->bInterlace,
        pFrameBufInfo->u8BitPerPixel,
        pFrameBufInfo->u8FBNum);
#endif
}
void Hal_SC_set_framebuf_Info(SC_FRAMEBUF_INFO_t pFrameBufInfo)
{
    MS_U32 u32IPMBase0, u32IPMBase1, u32IPMBase2;

    u32IPMBase0=pFrameBufInfo.u32IPMBase0/BYTE_PER_WORD;
    u32IPMBase1=pFrameBufInfo.u32IPMBase1/BYTE_PER_WORD;
    u32IPMBase2=pFrameBufInfo.u32IPMBase2/BYTE_PER_WORD;

    //Step 5.4, Memory Limit V : BK12_18[12] = 1, BK12_18[11:0] to the real V line number
    SC_W2BYTEMSK(REG_SC_BK12_18_L, (pFrameBufInfo.u16VLength|0x1000),0x1FFF);

//    u32WritelimitBase = ((u32IPMBase0 + u32MemSize) / BYTE_PER_WORD - 1) | 0x2000000;
    SC_W4BYTE(REG_SC_BK12_1A_L, pFrameBufInfo.u32WriteLimitBase);

    SC_W2BYTEMSK(REG_SC_BK12_09_L, (MS_U16)((u32IPMBase0 & 0xFF0000)>>16),0x00FF);
    SC_W2BYTE(REG_SC_BK12_08_L, (MS_U16)(u32IPMBase0 & 0xFFFF));

    SC_W2BYTEMSK(REG_SC_BK12_0B_L, (MS_U16)((u32IPMBase1 & 0xFF0000)>>16),0x00FF);
    SC_W2BYTE(REG_SC_BK12_0A_L, (MS_U16)(u32IPMBase1 & 0xFFFF));

    SC_W2BYTEMSK(REG_SC_BK12_0D_L, (MS_U16)((u32IPMBase2 & 0xFF0000)>>16),0x00FF);
    SC_W2BYTE(REG_SC_BK12_0C_L, (MS_U16)(u32IPMBase2 & 0xFFFF));


    SC_W2BYTE(REG_SC_BK12_0E_L, pFrameBufInfo.u16IPMOffset);
    SC_W2BYTE(REG_SC_BK12_0F_L, pFrameBufInfo.u16IPMFetch);
//    SC_W2BYTEMSK(REG_SC_BK12_18_L, (pFrameBufInfo.u16VLength|0x1000),0x1FFF);
    SC_W2BYTEMSK(REG_SC_BK12_03_L, ((pFrameBufInfo.bLinearAddrMode)?BIT(4):0),BIT(4));

}

MS_U8 Hal_SC_get_cs_det_cnt(SCALER_WIN eWindow)
{
    MS_U8 u8val;
    MS_U32 u32Reg_0C;

    u32Reg_0C = eWindow == MAIN_WINDOW     ? REG_SC_BK01_0C_L :
                eWindow == SUB_WINDOW      ? REG_SC_BK03_0C_L :
                eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_0C_L :
                eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_0C_L :
                eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_0C_L :
                                             REG_SC_DUMMY;

    u8val = ((SC_R2BYTE(u32Reg_0C) & 0xFF00) >> 8);

    return u8val;
}

void Hal_SC_set_cs_det_cnt(MS_U8 u8val, SCALER_WIN eWindow)
{
    MS_U32 u32Reg_0C;

    u32Reg_0C = eWindow == MAIN_WINDOW     ? REG_SC_BK01_0C_L :
                eWindow == SUB_WINDOW      ? REG_SC_BK03_0C_L :
                eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_0C_L :
                eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_0C_L :
                eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_0C_L :
                                             REG_SC_DUMMY;

    SC_W2BYTEMSK(u32Reg_0C, (((MS_U16)u8val)<<8), 0xFF00);
}

MS_U8 Hal_SC_get_plus_width(SCALER_WIN eWindow)
{
    MS_U8 u8val;
    MS_U32 u32Reg_22;

    u32Reg_22 = eWindow == MAIN_WINDOW     ? REG_SC_BK01_22_L :
                eWindow == SUB_WINDOW      ? REG_SC_BK03_22_L :
                eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_22_L :
                eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_22_L :
                eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_22_L :
                                             REG_SC_DUMMY;

    u8val = (SC_R2BYTE(u32Reg_22) & 0x00FF);
    return u8val;
}

void Hal_SC_set_opm_fetch(SCALER_WIN eWindow, MS_U16 u16OPMFetch)
{
    MS_U32 u32Reg_17;

    u32Reg_17 = eWindow == MAIN_WINDOW     ? REG_SC_BK12_17_L :
                eWindow == SUB_WINDOW      ? REG_SC_BK12_57_L :
                eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK12_17_L :
                eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK12_17_L :
                eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK12_57_L :
                                             REG_SC_DUMMY;

    SC_W2BYTEMSK(u32Reg_17, u16OPMFetch, 0x0FFF);
}

MS_U16 Hal_SC_get_opm_fetch(SCALER_WIN eWindow)
{
    MS_U16 u16Val = 0;
    MS_U32 u32Reg_17;

    u32Reg_17 = eWindow == MAIN_WINDOW     ? REG_SC_BK12_17_L :
                eWindow == SUB_WINDOW      ? REG_SC_BK12_57_L :
                eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK12_17_L :
                eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK12_17_L :
                eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK12_57_L :
                                             REG_SC_DUMMY;

    u16Val = SC_R2BYTE(u32Reg_17) & 0x0FFF;
    return u16Val;
}

MS_U8 HAL_SC_get_dnr_setting(SCALER_WIN eWindow)
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK06_21_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK06_01_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK06_21_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK06_21_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK06_01_L :
                                          REG_SC_DUMMY;

    return (MS_U8)SC_R2BYTEMSK(u32Reg, BIT(1)|BIT(0));
}

void HAL_SC_enable_field_avg_y(MS_BOOL bEnable, SCALER_WIN eWindow)
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK05_21_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK05_01_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK05_21_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK05_21_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK05_01_L :
                                          REG_SC_DUMMY;

    SC_W2BYTEMSK(u32Reg, bEnable ? BIT(6) : 0, BIT(6));
}

void HAL_SC_enable_field_avg_c(MS_BOOL bEnable, SCALER_WIN eWindow)
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK05_21_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK05_01_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK05_21_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK05_21_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK05_01_L :
                                          REG_SC_DUMMY;

    SC_W2BYTEMSK(u32Reg, bEnable ? BIT(7) : 0, BIT(7));
}

void HAL_SC_enable_dnr(MS_U8 u8Val, SCALER_WIN eWindow)
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK06_21_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK06_01_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK06_21_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK06_21_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK06_01_L :
                                          REG_SC_DUMMY;
    SC_W2BYTEMSK(u32Reg, u8Val, BIT(1)|BIT(0));
}

void HAL_SC_VOP_Set_Contrast_En(MS_BOOL bEenable, SCALER_WIN eWindow)
{
    if(eWindow == MAIN_WINDOW)
    {
        SC_W2BYTEMSK(REG_SC_BK0F_53_L, (MS_U16)(bEenable?BIT(0):0), BIT(0));
    }
    else if(eWindow == SUB_WINDOW)
    {
        SC_W2BYTEMSK(REG_SC_BK0F_53_L, (MS_U16)(bEenable?BIT(8):0), BIT(8));
    }
    else if(eWindow == SC1_MAIN_WINDOW)
    {
        SC_W2BYTEMSK(REG_SC1_BK0F_53_L, (MS_U16)(bEenable?BIT(0):0), BIT(0));
    }
    else if(eWindow == SC2_MAIN_WINDOW)
    {
        SC_W2BYTEMSK(REG_SC2_BK0F_53_L, (MS_U16)(bEenable?BIT(0):0), BIT(0));
    }
    else if(eWindow == SC2_SUB_WINDOW)
    {
        SC_W2BYTEMSK(REG_SC2_BK0F_53_L, (MS_U16)(bEenable?BIT(8):0), BIT(8));
    }
    else
    {
    }
}
void HAL_SC_VOP_Set_Contrast_Value(MS_XC_VOP_CHANNEL_t eVop_Channel, MS_U16 u16Val, SCALER_WIN eWindow)
{
    MS_U32 u32RegCHR, u32RegCHG, u32RegCHB;

    if(eWindow == MAIN_WINDOW)
    {
        u32RegCHR = REG_SC_BK0F_47_L;
        u32RegCHG = REG_SC_BK0F_48_L;
        u32RegCHB = REG_SC_BK0F_49_L;
    }
    else if(eWindow == SUB_WINDOW)
    {
        u32RegCHR = REG_SC_BK0F_4A_L;
        u32RegCHG = REG_SC_BK0F_4B_L;
        u32RegCHB = REG_SC_BK0F_4C_L;
    }
    else if(eWindow == SC1_MAIN_WINDOW)
    {
        u32RegCHR = REG_SC1_BK0F_47_L;
        u32RegCHG = REG_SC1_BK0F_48_L;
        u32RegCHB = REG_SC1_BK0F_49_L;
    }
    else if(eWindow == SC2_MAIN_WINDOW)
    {
        u32RegCHR = REG_SC2_BK0F_47_L;
        u32RegCHG = REG_SC2_BK0F_48_L;
        u32RegCHB = REG_SC2_BK0F_49_L;
    }
    else if(eWindow == SC2_SUB_WINDOW)
    {
        u32RegCHR = REG_SC2_BK0F_4A_L;
        u32RegCHG = REG_SC2_BK0F_4B_L;
        u32RegCHB = REG_SC2_BK0F_4C_L;
    }
    else
    {
        u32RegCHR = REG_SC_DUMMY;
        u32RegCHG = REG_SC_DUMMY;
        u32RegCHB = REG_SC_DUMMY;
    }

    switch(eVop_Channel)
    {
        default:
        case XC_VOP_CHR:
            SC_W2BYTEMSK(u32RegCHR, u16Val, 0x0FFF);
            break;
        case XC_VOP_CHG:
            SC_W2BYTEMSK(u32RegCHG, u16Val, 0x0FFF);
            break;
        case XC_VOP_CHB:
            SC_W2BYTEMSK(u32RegCHB, u16Val, 0x0FFF);
            break;
        case XC_VOP_ALL:
            SC_W2BYTEMSK(u32RegCHR, u16Val, 0x0FFF);
            SC_W2BYTEMSK(u32RegCHG, u16Val, 0x0FFF);
            SC_W2BYTEMSK(u32RegCHB, u16Val, 0x0FFF);
            break;
    }
}

void HAL_SC_VOP_Set_Brightness_En(MS_BOOL bEenable, SCALER_WIN eWindow)
{
    if(eWindow == MAIN_WINDOW)
    {
        SC_W2BYTEMSK(REG_SC_BK0F_53_L, (MS_U16)(bEenable?BIT(1):0), BIT(1));
    }
    else if(eWindow == SUB_WINDOW)
    {
        SC_W2BYTEMSK(REG_SC_BK0F_53_L, (MS_U16)(bEenable?BIT(9):0), BIT(9));
    }
    else if(eWindow == SC1_MAIN_WINDOW)
    {
        SC_W2BYTEMSK(REG_SC1_BK0F_53_L, (MS_U16)(bEenable?BIT(1):0), BIT(1));
    }
    else if(eWindow == SC2_MAIN_WINDOW)
    {
        SC_W2BYTEMSK(REG_SC2_BK0F_53_L, (MS_U16)(bEenable?BIT(1):0), BIT(1));
    }
    else if(eWindow == SC2_SUB_WINDOW)
    {
        SC_W2BYTEMSK(REG_SC2_BK0F_53_L, (MS_U16)(bEenable?BIT(9):0), BIT(9));
    }
    else
    {
    }

}
void HAL_SC_VOP_Set_Brightness_Value(MS_XC_VOP_CHANNEL_t eVop_Channel, MS_U16 u16Val, SCALER_WIN eWindow)
{
    MS_U32 u32RegCHR, u32RegCHG, u32RegCHB;

    if(eWindow == MAIN_WINDOW)
    {
        u32RegCHR = REG_SC_BK0F_4D_L;
        u32RegCHG = REG_SC_BK0F_4E_L;
        u32RegCHB = REG_SC_BK0F_4F_L;
    }
    else if(eWindow == SUB_WINDOW)
    {
        u32RegCHR = REG_SC_BK0F_50_L;
        u32RegCHG = REG_SC_BK0F_51_L;
        u32RegCHB = REG_SC_BK0F_52_L;
    }
    else if(eWindow == SC1_MAIN_WINDOW)
    {
        u32RegCHR = REG_SC1_BK0F_4D_L;
        u32RegCHG = REG_SC1_BK0F_4E_L;
        u32RegCHB = REG_SC1_BK0F_4F_L;
    }
    else if(eWindow == SC2_MAIN_WINDOW)
    {
        u32RegCHR = REG_SC2_BK0F_4D_L;
        u32RegCHG = REG_SC2_BK0F_4E_L;
        u32RegCHB = REG_SC2_BK0F_4F_L;
    }
    else if(eWindow == SC2_SUB_WINDOW)
    {
        u32RegCHR = REG_SC2_BK0F_50_L;
        u32RegCHG = REG_SC2_BK0F_51_L;
        u32RegCHB = REG_SC2_BK0F_52_L;
    }
    else
    {
        u32RegCHR = REG_SC_DUMMY;
        u32RegCHG = REG_SC_DUMMY;
        u32RegCHB = REG_SC_DUMMY;
    }

    switch(eVop_Channel)
    {
        default:
        case XC_VOP_CHR:
            SC_W2BYTEMSK(u32RegCHR, u16Val, 0x07FF);
            break;
        case XC_VOP_CHG:
            SC_W2BYTEMSK(u32RegCHG, u16Val, 0x07FF);
            break;
        case XC_VOP_CHB:
            SC_W2BYTEMSK(u32RegCHB, u16Val, 0x07FF);
            break;
        case XC_VOP_ALL:
            SC_W2BYTEMSK(u32RegCHR, u16Val, 0x07FF);
            SC_W2BYTEMSK(u32RegCHG, u16Val, 0x07FF);
            SC_W2BYTEMSK(u32RegCHB, u16Val, 0x07FF);
            break;
    }
}

MS_BOOL HAL_SC_Set_DynamicScaling(MS_U32 u32MemBaseAddr, MS_U8 u8MIU_Select, MS_U8 u8IdxDepth, MS_BOOL bOP_On, MS_BOOL bIPS_On, MS_BOOL bIPM_On)
{
    MS_U32 u32Base = u32MemBaseAddr;// / BYTE_PER_WORD;
    MS_U16 u16OnOff;
    static MS_U16 su16OldValue = 0;
    static MS_BOOL sbOldValueReaded = FALSE;

    if( u32Base > HAL_MIU1_BASE)
    {
        u32Base = u32Base - HAL_MIU1_BASE;
    }
    u32Base = u32Base/BYTE_PER_WORD;

    // set base address
    SC_W2BYTE(REG_SC_BK1F_11_L, u32Base & 0xFFFF);
    SC_W2BYTEMSK(REG_SC_BK1F_12_L, (u32Base >> 16) & 0x1FF, 0x01FF);

    // set index depth
    SC_W2BYTEMSK(REG_SC_BK1F_13_L, u8IdxDepth , 0x00FF);

    // enable/disable
    u16OnOff = SC_R2BYTE(REG_SC_BK1F_10_L) & 0x3000;
    u16OnOff |= ((bOP_On << 9) | (bIPS_On << 10) | (bIPM_On << 11));

    if(u16OnOff & 0x0E00)
    {
        // ENABLE DS
        u16OnOff |= BIT(12);     // enable write register through RIU
        bDynamicScalingEnable = TRUE;

        // currently, these 3 will use same MIU
        u16OnOff |= ((u8MIU_Select & 0x01) << 13);                       // MIU select of OP
        u16OnOff |= ((u8MIU_Select & 0x01) << 14);                       // MIU select of IPS
        u16OnOff |= ((u8MIU_Select & 0x01) << 15);                       // MIU select of IPM

        // patch IP1F2_21[15:14] r/w bank
        su16OldValue = SC_R2BYTE(REG_SC_BK01_21_L);
        sbOldValueReaded = TRUE;

        SC_W2BYTEMSK(REG_SC_BK01_21_L, 0x4000, 0xC000);
        Hal_XC_MLoad_set_opm_lock(OPM_LOCK_DS, MAIN_WINDOW);
    }
    else
    {
        bDynamicScalingEnable = FALSE;
        // DISABLE DS
        if(sbOldValueReaded)
        {
            SC_W2BYTE(REG_SC_BK01_21_L, su16OldValue);
            sbOldValueReaded = FALSE;
        }
        Hal_XC_MLoad_set_opm_lock(OPM_LOCK_INIT_STATE, MAIN_WINDOW);
    }

    SC_W2BYTEMSK(REG_SC_BK1F_10_L, 0x88, 0x00FF);               // set DMA threthold, length, fixed value, no need to change
    SC_W2BYTEMSK(REG_SC_BK1F_10_L, u16OnOff & 0xFF00, 0xFF00);  // set on/off

    // if disabled, need to set dynamic scaling ratio to 1:1
    if(!bOP_On)
    {
        SC_W4BYTE(REG_SC_BK23_1C_L, 0x00100000);
        SC_W4BYTE(REG_SC_BK23_1E_L, 0x00100000);
    }

    // set mload timing
    Hal_XC_MLoad_set_trig_p(0x14, 0x18, MAIN_WINDOW);
    SC_W2BYTEMSK(REG_SC_BK20_21_L, 0x08, 0xFF);
    SC_W2BYTEMSK(REG_SC_BK20_19_L, 0x08B8, 0x08BC);
    return TRUE;
}

MS_BOOL HAL_SC_Get_DynamicScaling_Status(void)
{
    return bDynamicScalingEnable;
}

void HAL_SC_Set_FB_Num(SCALER_WIN eWindow, XC_FRAME_STORE_NUMBER enFBType, MS_BOOL bInterlace)
{
    MS_U8 u8FrameCount = 2;
    MS_U32 u32Reg;

    switch( enFBType )
    {
        case IMAGE_STORE_2_FRAMES:
            u8FrameCount = 2;
            break;
        case IMAGE_STORE_3_FRAMES:
            u8FrameCount = 3;
            break;
        case IMAGE_STORE_4_FRAMES:
            u8FrameCount = 4;
            break;
        case IMAGE_STORE_6_FRAMES:
            u8FrameCount = 6;
            break;
        case IMAGE_STORE_8_FRAMES:
            u8FrameCount = 8;
            break;
        case IMAGE_STORE_12_FRAMES:
            u8FrameCount = 12;
            break;
        default:
            break;
    }

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK12_19_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK12_59_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK12_19_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK12_19_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK12_59_L :
                                          REG_SC_DUMMY;

    if (!bInterlace)
    {
        SC_W2BYTEMSK(u32Reg, u8FrameCount , BITMASK(4:0) );
    }
}

void HAL_SC_Enable_VInitFactor(MS_BOOL bEnable,SCALER_WIN eWindow)
{
    MS_U32 u32Reg;
    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK23_13_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK23_33_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK23_13_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK23_13_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK23_33_L :
                                          REG_SC_DUMMY;

    SC_W2BYTEMSK(u32Reg, (MS_U16)(bEnable?BIT(0):0), BIT(0));
}

void HAL_SC_Set_VInitFactorOne(MS_U32 u32Value,SCALER_WIN eWindow)
{
    MS_U32 u32Reg;
    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK23_03_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK23_23_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK23_03_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK23_03_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK23_23_L :
                                          REG_SC_DUMMY;

    SC_W4BYTE(u32Reg, u32Value);
}

void HAL_SC_Set_VInitFactorTwo(MS_U32 u32Value,SCALER_WIN eWindow)
{
    MS_U32 u32Reg;
    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK23_05_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK23_25_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK23_05_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK23_05_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK23_25_L :
                                          REG_SC_DUMMY;

    SC_W4BYTE(u32Reg, u32Value);
}

void HAL_SC_Set_vsd_output_line_count(MS_BOOL bEnable,MS_U32 u32LineCount,SCALER_WIN eWindow)
{
    MS_U16 u16OutputLineCount = 0x00;
    MS_U32 u32Reg;
    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK02_37_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK04_37_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK02_37_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK02_37_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK04_37_L :
                                          REG_SC_DUMMY;

    if (bEnable)
    {
        u16OutputLineCount = BIT(12);
        u16OutputLineCount |= (MS_U16)(u32LineCount & 0x7FF);
    }

    SC_W2BYTE(u32Reg, u16OutputLineCount);
}

/*
 bEnable : Enable input line count.
 bUserMode : 1 -> the input line count will ref u32UserLineCount
                   0 -> the input line count will ref V capture win
*/
void HAL_SC_Set_vsd_input_line_count(MS_BOOL bEnable,MS_BOOL bUserMode,MS_U32 u32UserLineCount,SCALER_WIN eWindow)
{
    MS_U16 u16InputLineCount = 0x00;
    MS_U32 u32Reg;
    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK02_36_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK04_36_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK02_36_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK02_36_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK04_36_L :
                                          REG_SC_DUMMY;
    if (bEnable)
    {
        u16InputLineCount = BIT(15);

        if (bUserMode)
        {
            u16InputLineCount |= BIT(14);
            u16InputLineCount |= (MS_U16)(u32UserLineCount & 0x1FFF);
        }
    }

    SC_W2BYTE(u32Reg, u16InputLineCount);
}

MS_U16  g_u16TmpValue = 0;
MS_BOOL g_bCntFlg = 1;
void HAL_SC_FilmMode_Patch1(void)
{
#if 1
    MS_U16 u16Value;
    MS_U16 u16RegValue;

    static MS_U8 u8Time_count = 0;

    u16RegValue = SC_R2BYTEMSK(REG_SC_BK22_7D_L, 0xFF);
    if(g_bCntFlg)
        g_u16TmpValue = SC_R2BYTEMSK(REG_SC_BK0A_02_L, 0xFF);

    if(SC_R2BYTEMSK(REG_SC_BK0A_21_L, BIT(6)))
    {
        //Counter
        if(u8Time_count != 0xFF)
            u8Time_count ++;

        if(u8Time_count < u16RegValue)  // fake out duration
           u16Value = 0x06;      // madi fake out enable
        else
           u16Value = 0x00;      // madi fake out disable

        SC_W2BYTEMSK(REG_SC_BK0A_25_L, (0x10<<8), 0xFF00);
        SC_W2BYTEMSK(REG_SC_BK0A_02_L, 0x22, 0xFF);
        g_bCntFlg = 0;
    }
    else
    {
        u8Time_count = 0;
        u16Value = 0x00;      // madi fake out disable
        SC_W2BYTEMSK(REG_SC_BK0A_25_L, (0x08<<8), 0xFF00);
        SC_W2BYTEMSK(REG_SC_BK0A_02_L, g_u16TmpValue, 0xFF);
        g_bCntFlg = 1;
    }
    SC_W2BYTEMSK(REG_SC_BK0A_21_L, u16Value, BIT(2)|BIT(1));

#else
    MS_U8 u8Bank, u8Value;
    MS_U8 u8RegValue;

    static MS_U8 u8Time_count = 0;

    MDrv_WriteByte(BK_SELECT_00, REG_BANK_MADI); //bk22
    u8RegValue = MDrv_ReadByte(L_BK_MADI(0x7D));

    MDrv_WriteByte(BK_SELECT_00, REG_BANK_FILM);


    if(MDrv_ReadRegBit(L_BK_FILM(0x21), BIT(6)))
    {
        //Counter
        if(u8Time_count != 0xFF)
            u8Time_count ++;

        if(u8Time_count < u8RegValue)  // fake out duration
           u8Value = 0x06;      // madi fake out enable
        else
           u8Value = 0x00;      // madi fake out disable

        MDrv_WriteByte(H_BK_FILM(0x25), 0x10);

    }
    else
    {
        u8Time_count = 0;
        u8Value = 0x00;      // madi fake out disable
        MDrv_WriteByte(H_BK_FILM(0x25), 0x08);
    }

    MDrv_WriteByteMask(L_BK_FILM(0x21), u8Value, BIT(2)|BIT(1));
    MDrv_WriteByte(BK_SELECT_00, u8Bank);
#endif
}

void HAL_SC_EnableFPLL(void)
{
    MDrv_WriteByteMask(L_BK_LPLL(0x0C), BIT(3), BIT(3));
    MDrv_WriteByteMask(L_BK_LPLL(0x0C),      0, BIT(6));
}


static void _HAL_SC_GetFPLLPhaseDiffISR(SC_INT_SRC eIntNum, void *pParam)
{
    volatile MS_XC_GET_FPLL_PHASEDIFF * pFpllPhaseDiff = (volatile MS_XC_GET_FPLL_PHASEDIFF *) pParam;

    pFpllPhaseDiff->u16PhaseDiff = MDrv_Read2Byte(L_BK_LPLL(0x11));
    pFpllPhaseDiff->eFpllDir = (MDrv_Read2Byte(L_BK_LPLL(0x12)) & BIT(0)) == BIT(0) ? E_XC_FPLL_DIR_UP : E_XC_FPLL_DIR_DOWN;
    pFpllPhaseDiff->u8Debounce++;

    if(pFpllPhaseDiff->u8Debounce > 3)
    {
        if(pFpllPhaseDiff->u16PhaseDiff < 0x200)
        {
            pFpllPhaseDiff->eFpllResult = E_XC_FPLL_RES_FINISHED;
        }
        else
        {
            if(pFpllPhaseDiff->u8Debounce > 90)
            {
                pFpllPhaseDiff->eFpllResult = E_XC_FPLL_RES_TIMEOUT;
            }
        }
    }
}

MS_BOOL HAL_SC_WaitFPLLDone(void)
{
    MS_XC_GET_FPLL_PHASEDIFF stGetFpllPhaseDiff = {0, 0, E_XC_FPLL_DIR_UNKNOWN, E_XC_FPLL_RES_WAITING};

    MDrv_XC_InterruptAttach(SC_INT_VSINT, _HAL_SC_GetFPLLPhaseDiffISR, (void *) &stGetFpllPhaseDiff);
    while(*(volatile MS_BOOL*)(&(stGetFpllPhaseDiff.eFpllResult)) == E_XC_FPLL_RES_WAITING);
    MDrv_XC_InterruptDeAttach(SC_INT_VSINT, _HAL_SC_GetFPLLPhaseDiffISR, (void *) &stGetFpllPhaseDiff);

    if(stGetFpllPhaseDiff.eFpllResult == E_XC_FPLL_RES_FINISHED)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

MS_U16 HAL_SC_GetOutputVFreqX100(MS_U32 u32XTAL_Clock)
{
    return ((MS_U16) ((u32XTAL_Clock * 100) / MDrv_Read4Byte(L_BK_LPLL(0x23))));
}

//Line buffer offset between Main and Sub, MAX(Main+Sub) = 1920+960
//Sub Win Line buffer offset , start from max size of main's
void HAL_SC_SetSubLineBufferOffset(MS_U16 u16Linebuffer, SCALER_WIN eWindow)
{
    MS_U32 u32Reg;
    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK20_13_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK20_13_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK20_13_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK20_13_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK20_13_L :
                                          REG_SC_DUMMY;

    SC_W2BYTE(u32Reg, u16Linebuffer);
}

//Select the trig mode
//0: Line base(Line Buffer Mode)
//1: Fill line buffer(Ring Buffer Mode)
void HAL_SC_SetDisplay_LineBuffer_Mode(MS_BOOL bEnable, SCALER_WIN eWindow)
{
    MS_U32 u32Reg;
    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK20_1F_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK20_1F_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK20_1F_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK20_1F_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK20_1F_L :
                                          REG_SC_DUMMY;

    SC_W2BYTEMSK(u32Reg, (bEnable ?  0 : BIT(4)), BIT(4));
}

//bk20_10[10]: Fill the main window's line buffer in vertical blanking for pip left and up corner
//bk20_10[11]: Fill the sub window's line buffer in vertical blanking for pip left and down corner
void HAL_SC_FillLineBuffer(MS_BOOL bEnable, SCALER_WIN eWindow)
{
    MS_U32 u32Reg;
    MS_U16 u16val;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK20_10_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK20_10_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK20_10_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK20_10_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK20_10_L :
                                          REG_SC_DUMMY;

    if(eWindow == MAIN_WINDOW || eWindow == SC1_MAIN_WINDOW || eWindow == SC2_MAIN_WINDOW)
    {
        u16val = BIT(10);
    }
    else
    {
        u16val = BIT(11);
    }

    SC_W2BYTEMSK(u32Reg, bEnable ? u16val : 0, u16val);

}

void Hal_SC_enable_window(MS_BOOL bEn,SCALER_WIN eWindow)
{
    MS_U32 u32Reg;
    MS_U16 u16val;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK20_10_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK20_10_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK20_10_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK20_10_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK20_10_L :
                                          REG_SC_DUMMY;

    if(eWindow == MAIN_WINDOW || eWindow == SC1_MAIN_WINDOW || eWindow == SC2_MAIN_WINDOW)
    {
        u16val = BIT(0);
    }
    else
    {
        u16val = BIT(1);
    }

    SC_W2BYTEMSK(u32Reg, bEn ? u16val : 0, u16val);
}

void Hal_SC_enable_window_burst(MS_BOOL bEn,SCALER_WIN eWindow)
{
    MS_U32 u32Reg;
    MS_U16 u16val;

    _MLOAD_ENTRY();

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK20_10_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK20_10_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK20_10_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK20_10_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK20_10_L :
                                          REG_SC_DUMMY;

    if(eWindow == MAIN_WINDOW || eWindow == SC1_MAIN_WINDOW || eWindow == SC2_MAIN_WINDOW)
    {
        u16val = BIT(0);
    }
    else
    {
        u16val = BIT(1);
    }

    MApi_XC_MLoad_WriteCmd(u32Reg, bEn ? u16val : 0, u16val);
    MApi_XC_MLoad_Fire(TRUE);
    _MLOAD_RETURN();
}

void Hal_SC_set_trigger_signal(void)
{
    // only used in U4
}

void Hal_SC_Set_OSD2VE(EN_VOP_SEL_OSD_XC2VE_MUX  eVOPSelOSD_MUX)
{
    UNUSED(eVOPSelOSD_MUX);
}

MS_BOOL Hal_SC_IsOPMFetchPatch_Enable(void)
{
    //Patch for Janus u01 and T4 U01/02: OPMFetch need +2
    return FALSE;
}
MS_BOOL Hal_SC_Check_HNonLinearScaling(void)
{
    MS_U16 u16Offset = (SC_R2BYTE(REG_SC_BK23_15_L)>>8);
    MS_U16 u16MaxOffset = (MS_U16)((SC_R4BYTE(REG_SC_BK23_07_L) & 0xFFFFFF)>>12);
    //Non-Linear scaling Postive offset, the maximum offset is HPstRatio[23:12]
    if(((u16Offset & BIT(7)) == 0) && (u16Offset > u16MaxOffset))
    {
        return FALSE;
    }
    return TRUE;
}

//IP2 Pre-Filter enable
void Hal_SC_IP2_PreFilter_Enable(MS_BOOL bEnable)
{
    SC_W2BYTEMSK(REG_SC_BK02_48_L, (bEnable?BIT(15):0), BIT(15));
}

void Hal_SC_Set_extra_fetch_line(MS_U8 u8val, SCALER_WIN eWindow)
{
    MS_U32 u32Reg;
    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK20_17_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK20_17_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK20_17_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK20_17_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK20_17_L :
                                          REG_SC_DUMMY;

    SC_W2BYTEMSK(u32Reg, ((MS_U16)(u8val & 0x0F))<<8, 0x0F00);
}

void Hal_SC_Set_extra_adv_line(MS_U8 u8val, SCALER_WIN eWindow)
{
    MS_U32 u32Reg;
    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK20_17_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK20_17_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK20_17_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK20_17_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK20_17_L :
                                          REG_SC_DUMMY;

    SC_W2BYTEMSK(u32Reg, ((MS_U16)(u8val & 0x0F))<<12, 0xF000);
}

void HAL_SC_Set_FPLL_Limit(MS_U32 *u32PllSet, MS_U32 u32LowBound, MS_U32 u32UpBound, MS_BOOL _bInFPLLDbgMode, MS_U32 _U32LimitD5D6D7)
{
    MS_U32 u32LpllLimitLow, u32LpllLimitHigh;
    MS_BOOL bEnableFPLLManualDebug;

    bEnableFPLLManualDebug = g_pSContext->bEnableFPLLManualDebug;

    //printf("Fpll_Limit: Set %lx, bound (%lx -> %lx)\n", *u32PllSet, u32UpBound, u32LowBound);

    if(_bInFPLLDbgMode)
    {
        MDrv_WriteByteMask(L_BK_LPLL(0x0D), 0x00, BIT(4));          // turn off 2 limit

        if(!bEnableFPLLManualDebug)
        {
            MDrv_Write3Byte(L_BK_LPLL(0x06), _U32LimitD5D6D7);
        }
    }
    else
    {
        MDrv_WriteByteMask(L_BK_LPLL(0x0D), BIT(4), BIT(4));        // turn on 2 limit

        // check if u32PllSet out of range and also the FPLL limit
        if (*u32PllSet <= u32LowBound)
        {
            *u32PllSet = u32LowBound;
            gSrcInfo[MAIN_WINDOW].bEnableFPLL = FALSE;
        }
        else if (*u32PllSet >= u32UpBound)
        {
            *u32PllSet = u32UpBound;
            gSrcInfo[MAIN_WINDOW].bEnableFPLL = FALSE;
        }

        u32LpllLimitHigh = u32UpBound - *u32PllSet;
        u32LpllLimitLow = *u32PllSet - u32LowBound - 1;

        //printf("Fpll set %lx, limit (high %lx, low %lx)\n", *u32PllSet, u32LpllLimitHigh, u32LpllLimitLow);

        if(!bEnableFPLLManualDebug)
        {
            MDrv_Write3Byte(L_BK_LPLL(0x06), u32LpllLimitHigh);
            MDrv_Write3Byte(L_BK_LPLL(0x08), u32LpllLimitLow);
        }

        //printf("Fpll limit reg (high %lx, low %lx)\n", MDrv_Read4Byte(L_BK_LPLL(0x06)), MDrv_Read4Byte(L_BK_LPLL(0x08)));
    }
}

void Hal_XC_H3D_Enable(MS_BOOL bEn)
{
    SC_W2BYTEMSK(REG_SC_BK01_70_L, (bEn<<15), (BIT(15)));
}

void Hal_XC_H3D_Input3DType(E_XC_3D_INPUT_MODE e3DType)
{
    SC_W2BYTEMSK(REG_SC_BK01_7B_L, (e3DType), (BIT(1) | BIT(0)));
}

void Hal_XC_H3D_Breakline_Enable(MS_BOOL bEn)
{
    SC_W2BYTEMSK(REG_SC_BK01_7B_L, (bEn<<7), (BIT(7)));
    if(bEn)
    {
        SC_W2BYTEMSK(REG_SC_BK01_7B_L, (8<<8), HBMASK);
    }
    else
    {
        SC_W2BYTEMSK(REG_SC_BK01_7B_L, (0<<8), HBMASK);
    }
}

void Hal_XC_H3D_HDE(MS_U16 u16Hde)
{
    SC_W2BYTEMSK(REG_SC_BK01_71_L, (u16Hde), 0x0FFF);
}

void Hal_XC_H3D_VDE_F0(MS_U16 u16Vde)
{
    SC_W2BYTEMSK(REG_SC_BK01_73_L, (u16Vde), 0x0FFF);
}

void Hal_XC_H3D_VDE_F2(MS_U16 u16Vde)
{
    SC_W2BYTEMSK(REG_SC_BK01_77_L, (u16Vde), 0x0FFF);
}

void Hal_XC_H3D_HBLANK(MS_U16 u16Hblank)
{
    SC_W2BYTEMSK(REG_SC_BK01_70_L, (u16Hblank), 0x03FF);
}

void Hal_XC_H3D_INIT_VBLANK(MS_U8 u8Vblank)
{
    SC_W2BYTEMSK(REG_SC_BK01_72_L, (u8Vblank), LBMASK);
}

void Hal_XC_H3D_VBLANK0(MS_U8 u8Vblank)
{
    SC_W2BYTEMSK(REG_SC_BK01_74_L, (u8Vblank), LBMASK);
}

void Hal_XC_H3D_VBLANK1(MS_U8 u8Vblank)
{
    SC_W2BYTEMSK(REG_SC_BK01_76_L, (u8Vblank), LBMASK);
}

void Hal_XC_H3D_VBLANK2(MS_U8 u8Vblank)
{
    SC_W2BYTEMSK(REG_SC_BK01_78_L, (u8Vblank), LBMASK);
}

void Hal_XC_H3D_VSYNC_WIDTH(MS_U8 u8Width)
{
    SC_W2BYTEMSK(REG_SC_BK01_7A_L, (u8Width), 0x001F);
}

void Hal_XC_H3D_VSYNC_POSITION(MS_U16 u16Position)
{
    SC_W2BYTEMSK(REG_SC_BK01_7A_L, (u16Position), HBMASK);
}

void Hal_XC_H3D_SELECT_REGEN_TIMING(MS_BOOL bEn)
{
    SC_W2BYTEMSK(REG_SC_BK01_7B_L, (bEn<<4), (BIT(4)));
}

void Hal_XC_H3D_LR_Toggle_Enable(MS_BOOL bEn)
{
    if(bEn)
    {
        //Enanble I2S_IN_SD to send out lr flag
        MDrv_WriteByteMask(0x101E24, 0x00, BIT(4)|BIT(5)|BIT(6));
        MDrv_WriteByteMask(0x101EC9, 0x00, BIT(0)|BIT(1));
        MDrv_WriteByteMask(0x101EDC, 0x00, BIT(2));
        MDrv_WriteByteMask(0x101EB3, 0xC0, 0xC0);
    }
    else
    {
        MDrv_WriteByteMask(0x101EB3, 0x00, 0xC0);
    }
    SC_W2BYTEMSK(REG_SC_BK12_33_L, (bEn<<1), (BIT(1)));
}

void HAL_XC_H3D_OPM_SBYS_PIP_Enable(MS_BOOL bEn)
{
    SC_W2BYTEMSK(REG_SC_BK12_34_L, (bEn<<7), (BIT(7)));
}

void Hal_SC_enable_cursor_report(MS_BOOL bEn)
{
    if(bEn)
    {
        SC_W2BYTEMSK(REG_SC_BK0F_57_L, BIT(8) | BIT(10), BIT(8) | BIT(10));
    }
    else
    {
        SC_W2BYTEMSK(REG_SC_BK0F_57_L, 0x00 , BIT(8) | BIT(10));
    }
}

MS_BOOL Hal_SC_get_pixel_rgb(XC_Get_Pixel_RGB *pData)
{
    MS_BOOL bret = TRUE;


    switch(pData->enStage)
    {
    case E_XC_GET_PIXEL_STAGE_AFTER_DLC:
        SC_W2BYTEMSK(REG_SC_BK0F_46_L, 0x00 , BMASK(3:0));
        break;
    case E_XC_GET_PIXEL_STAGE_PRE_GAMMA:
        SC_W2BYTEMSK(REG_SC_BK0F_46_L, 0x08, BMASK(3:0));
        break;
    default:
        bret = FALSE;
        break;
    }

    if(bret)
    {
        // Enable
        if(!s_bKeepPixelPointerAppear)
        {
            Hal_SC_enable_cursor_report(TRUE);
        }

        // Initial position (x,y)  Set point you want to read.
        SC_W2BYTE(REG_SC_BK0F_19_L, pData->u16x);
        SC_W2BYTE(REG_SC_BK0F_1A_L, pData->u16y);

        MsOS_DelayTask(32);  // Must wait .

        pData->u32r = SC_R2BYTE(REG_SC_BK0F_2D_L);
        pData->u32g = SC_R2BYTE(REG_SC_BK0F_2E_L);
        pData->u32b = SC_R2BYTE(REG_SC_BK0F_2F_L);

        // Disable
        if(!s_bKeepPixelPointerAppear)
        {
            Hal_SC_enable_cursor_report(FALSE);
        }
    }


    return bret;
}

void Hal_SC_SetOPWriteOff(MS_BOOL bEna)
{
    MS_BOOL u16OldReg = (MS_BOOL)(SC_R2BYTEMSK(REG_SC_BK12_27_L, BIT(0)));
    if(u16OldReg != bEna)
    {
        SC_W2BYTEMSK(REG_SC_BK12_27_L, bEna, BIT(0));
    }
}


MS_BOOL Hal_SC_GetOPWriteOff(void)
{
    return (MS_BOOL)SC_R2BYTEMSK(REG_SC_BK12_27_L, BIT(0));
}

MS_BOOL Hal_SC_Detect_RequestFBL_Mode(void)
{
	return ENABLE_REQUEST_FBL;
}

void Hal_XC_SetFrameColor_En(MS_BOOL bEn, SCALER_WIN eWindow)
{

    MS_U32 u32Reg;
    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK10_19_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK10_19_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK10_19_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK10_19_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK10_19_L :
                                             REG_SC_DUMMY;

    SC_W2BYTEMSK(u32Reg, bEn ? BIT(0) : 0, BIT(0));
}

void Hal_XC_SetFrameColor(MS_U32 u32aRGB, SCALER_WIN eWindow)
{
    MS_U32 u32Reg_19, u32Reg_1A;
    u32Reg_19 = eWindow == MAIN_WINDOW     ? REG_SC_BK10_19_L :
                eWindow == SUB_WINDOW      ? REG_SC_BK10_19_L :
                eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK10_19_L :
                eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK10_19_L :
                eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK10_19_L :
                                             REG_SC_DUMMY;

    u32Reg_1A = eWindow == MAIN_WINDOW     ? REG_SC_BK10_1A_L :
                eWindow == SUB_WINDOW      ? REG_SC_BK10_1A_L :
                eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK10_1A_L :
                eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK10_1A_L :
                eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK10_1A_L :
                                             REG_SC_DUMMY;

    SC_W2BYTEMSK(u32Reg_19,  (u32aRGB >> 8),0xFF00);            ///< R
    SC_W2BYTEMSK(u32Reg_1A,  (u32aRGB >> 8),0x00FF);            ///< G
    SC_W2BYTEMSK(u32Reg_1A,  (u32aRGB << 8),0xFF00);            ///< B
}
MS_U8 Hal_SC_getVSyncWidth(SCALER_WIN eWindow)
{
    MS_U8 u8val = 0;

    if(eWindow == MAIN_WINDOW)
    {
        u8val = ((SC_R2BYTE(REG_SC_BK01_22_L) & 0xFF00) >> 8);
    }
    else
    {
    }
    return u8val;
}

E_APIXC_ReturnValue Hal_SC_Set_OSDLayer(E_VOP_OSD_LAYER_SEL  eVOPOSDLayer, SCALER_WIN eWindow)
{
    //do nothing
    return E_APIXC_RET_FAIL;
}

E_VOP_OSD_LAYER_SEL Hal_SC_Get_OSDLayer(SCALER_WIN eWindow)
{
    //do nothing
    return E_VOP_LAYER_RESERVED;
}

E_APIXC_ReturnValue Hal_SC_Set_VideoAlpha(MS_U8 u8Val, SCALER_WIN eWindow)
{
    //do nothing
    return E_APIXC_RET_FAIL;
}

E_APIXC_ReturnValue Hal_SC_Get_VideoAlpha(MS_U8 *pu8Val, SCALER_WIN eWindow)
{
    *pu8Val += 0;

    //do nothing
    return E_APIXC_RET_FAIL;
}

void Hal_XC_ClearScalingFactorForInternalCalib(void)
{
    //MDrv_WriteRegBit(H_BK_SCMI(0x09), FALSE, BIT(7));
    SC_W2BYTE(REG_SC_BK02_04_L, 0x0000);   // H pre-scaling
    SC_W2BYTE(REG_SC_BK02_05_L, 0xC010);   // H pre-scaling
    SC_W2BYTE(REG_SC_BK02_08_L, 0x0000);   // V pre-scaling
    SC_W2BYTE(REG_SC_BK02_09_L, 0x0000);   // V pre-scaling

    //HVSP
    SC_W2BYTE(REG_SC_BK23_07_L, 0x0000);  // H post-scaling
    SC_W2BYTE(REG_SC_BK23_08_L, 0x0100);  // H post-scaling
    SC_W2BYTE(REG_SC_BK23_09_L, 0x0000);  // V post-scaling
    SC_W2BYTE(REG_SC_BK23_0A_L, 0x0100);  // V post-scaling
}

MS_U8 MHal_SC_cal_usedgain_to_reggain(MS_U32 u16UsedGain_x32)
{
	MS_U8 u8Value = 0;
    if      (u16UsedGain_x32 >=65536) u8Value = 16;
    else if (u16UsedGain_x32 >=32768) u8Value = 15;
    else if (u16UsedGain_x32 >=16384) u8Value = 14;
    else if (u16UsedGain_x32 >= 8192) u8Value = 13;
    else if (u16UsedGain_x32 >= 4096) u8Value = 12;
    else if (u16UsedGain_x32 >= 2048) u8Value = 11;
    else if (u16UsedGain_x32 >= 1024) u8Value = 10;
    else if (u16UsedGain_x32 >=  512) u8Value = 9;
    else if (u16UsedGain_x32 >=  256) u8Value = 8;
    else if (u16UsedGain_x32 >=  128) u8Value = 7;
    else if (u16UsedGain_x32 >=   64) u8Value = 6;
    else if (u16UsedGain_x32 >=   32) u8Value = 5;
    else if (u16UsedGain_x32 >=   16) u8Value = 4;
    else if (u16UsedGain_x32 >=    8) u8Value = 3;
    else if (u16UsedGain_x32 >=    4) u8Value = 2;
    else if (u16UsedGain_x32 >=    2) u8Value = 1;
    else if (u16UsedGain_x32 >=    1) u8Value = 0;
    else {
        MS_ASSERT(0);
        u8Value = 0;
    }

    u8Value += IPGAIN_REFACTOR;

    return u8Value;
}

E_APIXC_ReturnValue Hal_SC_OP2VOPDESel(E_OP2VOP_DE_SEL eVopDESel)
{
    //Select op2vop de, for DWIN capture etc
    SC_W2BYTEMSK(REG_SC_BK10_6B_L, (eVopDESel << 12), (BIT(12)+BIT(13)));
    return E_APIXC_RET_OK;
}

MS_BOOL Hal_XC_SVOutput_GetCaps(void)
{
    return FALSE;
}

E_APIXC_ReturnValue Hal_XC_IsForcePrescaling(const XC_InternalStatus *pSrcInfo, MS_BOOL *pbForceV, MS_BOOL *pbForceH, SCALER_WIN eWindow)
{
    //MS_U16 u16Height = g_XC_InitData.stPanelInfo.u16Height ;
    //MS_U16 u16Widht  = g_XC_InitData.stPanelInfo.u16Width;

    *pbForceV = FALSE;
    *pbForceH = FALSE;

    //-----------------------------------------
    // Vertical
    //-----------------------------------------
#if 0
    //FHD case. FHD has tight bandwidth in FCLK (post scaling), so force pre-scaling if disp.v size < 80% of crop.v size.
    if ( (u16Height > 1000) &&
         (u16Widht > 1900) )
    {
        // 80%
        if ( ( pSrcInfo->stDispWin.height * 10 ) <  ( pSrcInfo->stCropWin.height * 8 ) )
        {
            *pbForceV = TRUE;
        }
    }
    else // SD, HD panel.
    {
        // 60%
        if ( ( pSrcInfo->stDispWin.height * 10 ) <  ( pSrcInfo->stCropWin.height * 6 ) )
        {
            *pbForceV = TRUE;
        }
    }

    //-----------------------------------------
    // Horizontal
    //-----------------------------------------

    // 80% ( SEC using 80% for all panel ). Increase % if meet FCLK bandwidth issue in the future.
    if ( ( pSrcInfo->stDispWin.width * 10 ) < ( pSrcInfo->stCropWin.width * 8 ) )
    {
        *pbForceH = TRUE;
    }
#else
    if(0)//eWindow == SC1_MAIN_WINDOW)
    {
        *pbForceV = FALSE;
        *pbForceH = FALSE;
    }
    else
    {
        *pbForceV = pSrcInfo->bInterlace ? FALSE : TRUE;
        *pbForceH = TRUE;
    }

#endif
    return E_APIXC_RET_OK;
}

MS_BOOL Hal_SC_IsHW2Dto3DPatch_Enable(void)
{
    MS_BOOL bRet = FALSE;
    if(PM_R1BYTE(REG_CHIP_REVISION, 7:0) < 1)//a1 u01 2d to 3d hw bug
    {
        bRet = TRUE;
    }
    return bRet;
}

MS_U32 MHal_SC_Get_LpllSet_Factor(MS_U8 u8LPLL_Mode)
{
    MS_U32 u32Factor = 0;

    u32Factor  = LPLL_LOOPGAIN/8/2;

    return u32Factor;
}


// prevdown_f2 = 1      prevdown_f2 = 0     prevdown_f2 = 1
// prevdown_f1 = 0      prevdown_f1 = 1     prevdown_f1 = 1
// feclk_f1 = 4'b0000   feclk_f1 = 4'b0000  feclk_f1 = 4'b0000
// feclk_f2 = 4'b0100   feclk_f2 = 4'b0100  feclk_f2 = 4'b0000
void Hal_SC_set_FICLK(SCALER_WIN eWindow, MS_BOOL bMainPreVSD, MS_BOOL bSubPreVSD)
{

    if(eWindow == MAIN_WINDOW || eWindow == SUB_WINDOW)
    {
        if(bMainPreVSD && bSubPreVSD)
        {
            MDrv_WriteByteMask(REG_CKG_FICLK_F2, CKG_FICLK_F2_IDCLK2, CKG_FICLK_F2_MASK);

        }
        else
        {
            MDrv_WriteByteMask(REG_CKG_FICLK_F2, CKG_FICLK_F2_FLK, CKG_FICLK_F2_MASK);
        }


        if(bMainPreVSD || bSubPreVSD)
        {
            MDrv_WriteRegBit(REG_CKG_FICLK_F1, DISABLE, CKG_FICLK_F1_INVERT);
            MDrv_WriteRegBit(REG_CKG_FICLK_F1, DISABLE, CKG_FICLK_F1_GATED);
       }

    }
    else if(eWindow == SC2_MAIN_WINDOW || eWindow == SC2_SUB_WINDOW)
    {
        if(bMainPreVSD && bSubPreVSD)
        {
            MDrv_WriteByteMask(REG_CKG_SC2_FICLK_F2, CKG_SC2_FICLK_F2_IDCLK2, CKG_SC2_FICLK_F2_MASK);

        }
        else
        {
            MDrv_WriteByteMask(REG_CKG_SC2_FICLK_F2, CKG_SC2_FICLK_F2_FLK, CKG_SC2_FICLK_F2_MASK);
        }


        if(bMainPreVSD || bSubPreVSD)
        {
            MDrv_WriteRegBit(REG_CKG_SC2_FICLK_F1, DISABLE, CKG_SC2_FICLK_F1_INVERT);
            MDrv_WriteRegBit(REG_CKG_SC2_FICLK_F1, DISABLE, CKG_SC2_FICLK_F1_GATED);
       }

    }
}


void Hal_SC_Set_Capture_Enable(SCALER_WIN eWindow, MS_BOOL bEn)
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK0F_57_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK0F_57_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK0F_57_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK0F_57_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK0F_57_L :
                                          REG_SC_DUMMY;

    if(bEn)
    {
        SC_W2BYTEMSK(u32Reg, 0x800, 0x800);
    }
    else
    {
        SC_W2BYTEMSK(u32Reg, 0x000, 0x800);
    }
}



MS_U16 Hal_SC_Get_Ouput_HSync_End(SCALER_WIN eWindow)
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK10_01_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK10_01_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK10_01_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK10_01_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK10_01_L :
                                          REG_SC_DUMMY;


    return (SC_R2BYTE(u32Reg) & 0xFF);
}

MS_U16 Hal_SC_Get_Ouput_VSync_End(SCALER_WIN eWindow)
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK10_03_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK10_03_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK10_03_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK10_03_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK10_03_L :
                                          REG_SC_DUMMY;


    return (SC_R2BYTE(u32Reg) & 0xFFF);
}


MS_BOOL Hal_SC_Get_Ouput_Interlace(SCALER_WIN eWindow)
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK10_14_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK10_14_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK10_14_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK10_14_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK10_14_L :
                                          REG_SC_DUMMY;


    return (MS_BOOL)(SC_R2BYTE(u32Reg) & 0x01);

}

#undef  MHAL_SC_C
