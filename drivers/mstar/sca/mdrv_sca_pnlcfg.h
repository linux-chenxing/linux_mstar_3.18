///////////////////////////////////////////////////////////////////////////////
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
///////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_SCA_PNLCFG_H
#define _MDRV_SCA_PNLCFG_H


//------------------------------------------------------------------------------
#define PANEL_PDP_10BIT                 1
#define PANEL_CONNECTOR_SWAP_PORT       1
#define PANEL_SWAP_LVDS_POL             1
#define PANEL_SWAP_LVDS_CH              0

#define LVDS_PN_SWAP_L                  0x00
#define LVDS_PN_SWAP_H                  0x00


PanelType  tPanelHSD_070I_DW1_MST154A_D02A[] =
{
    {
        "HSD_070I_DW1",//m_pPanelName
        //////////////////////////////////////////////
        // Panel output
        //////////////////////////////////////////////
        0,          //BOOL m_bPanelDither :1;           //PANEL_DITHER          // 8/6 bits panel
        LINK_TTL,  //BOOL m_ePanelLinkType :2;         //PANEL_LINK

        0,          //BOOL m_bPanelDualPort :1;         //PANEL_DUAL_PORT

        1^PANEL_CONNECTOR_SWAP_PORT,                    // shall swap if
                                                        // (PANEL_SWAP_PORT XOR Board_Connect_Swap) is TRUE

        1,          //BOOL m_bPanelSwapOdd_ML   :1;     //PANEL_SWAP_ODD_ML
        1,          //BOOL m_bPanelSwapEven_ML  :1;     //PANEL_SWAP_EVEN_ML
        0,          //BOOL m_bPanelSwapOdd_RB   :1;     //PANEL_SWAP_ODD_RB
        0,          //BOOL m_bPanelSwapEven_RB  :1;     //PANEL_SWAP_EVEN_RB

        PANEL_SWAP_LVDS_POL,

        PANEL_SWAP_LVDS_CH,          //BOOL m_bPanelSwapLVDS_CH  :1;     //PANEL_SWAP_LVDS_CH
        PANEL_PDP_10BIT,          //BOOL m_bPanelPDP10BIT     :1;     //PANEL_PDP_10BIT
    	0,          //BOOL m_bPanelLVDS_TI_MODE :1;     //PANEL_LVDS_TI_MODE

        0x00,       //BYTE m_ucPanelDCLKDelay;          //PANEL_DCLK_DELAY
        0,          //BOOL m_bPanelInvDCLK  :1;         //PANEL_INV_DCLK
        0,          //BOOL m_bPanelInvDE        :1;     //PANEL_INV_DE
        0,          //BOOL m_bPanelInvHSync :1;         //PANEL_INV_HSYNC
        0,          //BOOL m_bPanelInvVSync :1;         //PANEL_INV_VSYNC

        ///////////////////////////////////////////////
        // Output tmming setting
        ///////////////////////////////////////////////
        // driving current setting (0x00=4mA, 0x01=6mA, 0x02=8mA, 0x03=12mA)
        0x00,       //BYTE m_ucPanelDCKLCurrent;        //PANEL_DCLK_CURRENT         // DCLK current
        0x00,       //BYTE m_ucPanelDECurrent;          //PANEL_DE_CURRENT           // DE signal current
        0x00,       //BYTE m_ucPanelODDDataCurrent;     //PANEL_ODD_DATA_CURRENT     // odd data current
        0x00,       //BYTE m_ucPanelEvenDataCurrent;    //PANEL_EVEN_DATA_CURRENT    // even data current

        30,         //BYTE m_ucPanelOnTiming1;          //PANEL_ON_TIMING1          // time between panel & data while turn on power
        200,        //BYTE m_ucPanelOnTiming2;          //PANEL_ON_TIMING2          // time between data & back light while turn on power
        220,        //BYTE m_ucPanelOffTiming1;         //PANEL_OFF_TIMING1         // time between back light & data while turn off power
        10,         //BYTE m_ucPanelOffTiming2;         //PANEL_OFF_TIMING2         // time between data & panel while turn off power

        48,         //BYTE m_ucPanelHSyncWidth;         //PANEL_HSYNC_WIDTH
        40,         //BYTE m_ucPanelHSyncBackPorch;     //PANEL_HSYNC_BACK_PORCH

        3,          //BYTE m_ucPanelVSyncWidth;         //PANEL_VSYNC_WIDTH
        13,         //BYTE m_ucPanelBackPorch;          //PANEL_VSYNC_BACK_PORCH

        40+40,      //WORD m_wPanelHStart;              //PANEL_HSTART             (PANEL_HSYNC_WIDTH + PANEL_HSYNC_BACK_PORCH)
        29+13,       //WORD m_wPanelVStart;              //PANEL_VSTART             (PANEL_VSYNC_WIDTH + PANEL_VSYNC_BACK_PORCH)

        800,        //WORD m_wPanelWidth;               //PANEL_WIDTH
        480,       //WORD m_wPanelHeight;              //PANEL_HEIGHT

        1070,      //WORD m_wPanelMaxHTotal;           //PANEL_MAX_HTOTAL
        928,       //WORD m_wPanelHTotal;              //PANEL_HTOTAL
        878,       //WORD m_wPanelMinHTotal;           //PANEL_MIN_HTOTAL

        625,        //WORD m_wPanelMaxVTotal;           //PANEL_MAX_VTOTAL
        525,        //WORD m_wPanelVTotal;              //PANEL_VTOTAL 20060511 chris :for Frame Lock operation
        475,        //WORD m_wPanelMinVTotal;           //PANEL_MIN_VTOTAL

        40, //DWORD m_dwPanelMaxDCLK;           //PANEL_MAX_DCLK
        30, //DWORD m_dwPanelDCLK;              //PANEL_DCLK
        25, //DWORD m_dwPanelMinDCLK;           //PANEL_MIN_DCLK

        0x0019,     //m_wSpreadSpectrumStep;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)
        0x00C0,     //m_wSpreadSpectrumSpan;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)

        0x6D,       //m_ucDimmingCtl

        255,        //m_ucMaxPWMVal;
        0x20,//63,  //m_ucMinPWMVal;
        0,          //BOOL m_bPanelDeinterMode  :1;     //PANEL_DEINTER_MODE
        E_PNL_ASPECT_RATIO_WIDE,
        //
        //  Board related params.
        //
       (LVDS_PN_SWAP_H<<8) | LVDS_PN_SWAP_L,            //MS_U16 m_u16LVDSTxSwapValue
       TI_10BIT_MODE,          //10bit ti bit mode
       OUTPUT_8BIT_MODE,
       0,          //   PANEL_SWAP_ODD_RG
       0,          //   PANEL_SWAP_EVEN_RG
       0,          //   PANEL_SWAP_ODD_GB
       0,          //   PANEL_SWAP_EVEN_GB
       0,          //   double clock
       0x4EEBF3,
       0x3137B6,
       E_PNL_CHG_VTOTAL,
       1,///<  PAFRC mixed with noise dither disable
    },
};
//! C3 LVDS panel: HSD070PFW3-A00 !//
PanelType  tPanelHSD_070P_FW3_MST154A[] =
{
    {
        "HSD_070P_FW3",//m_pPanelName
        //////////////////////////////////////////////
        // Panel output
        //////////////////////////////////////////////
        0,          //BOOL m_bPanelDither :1;           //PANEL_DITHER          // 8/6 bits panel
        LINK_LVDS,  //LINK_TTL,  //BOOL m_ePanelLinkType :2;         //PANEL_LINK

        0,          //BOOL m_bPanelDualPort :1;         //PANEL_DUAL_PORT

        0,                    // shall swap if
                                                        // (PANEL_SWAP_PORT XOR Board_Connect_Swap) is TRUE

        0,          //BOOL m_bPanelSwapOdd_ML   :1;     //PANEL_SWAP_ODD_ML
        0,          //BOOL m_bPanelSwapEven_ML  :1;     //PANEL_SWAP_EVEN_ML
        0,          //BOOL m_bPanelSwapOdd_RB   :1;     //PANEL_SWAP_ODD_RB
        0,          //BOOL m_bPanelSwapEven_RB  :1;     //PANEL_SWAP_EVEN_RB

        0,

        0,          //BOOL m_bPanelSwapLVDS_CH  :1;     //PANEL_SWAP_LVDS_CH
        PANEL_PDP_10BIT,          //BOOL m_bPanelPDP10BIT     :1;     //PANEL_PDP_10BIT
    	1,          //BOOL m_bPanelLVDS_TI_MODE :1;     //PANEL_LVDS_TI_MODE

        0x00,       //BYTE m_ucPanelDCLKDelay;          //PANEL_DCLK_DELAY
        0,          //BOOL m_bPanelInvDCLK  :1;         //PANEL_INV_DCLK
        0,          //BOOL m_bPanelInvDE        :1;     //PANEL_INV_DE
        0,          //BOOL m_bPanelInvHSync :1;         //PANEL_INV_HSYNC
        0,          //BOOL m_bPanelInvVSync :1;         //PANEL_INV_VSYNC

        ///////////////////////////////////////////////
        // Output tmming setting
        ///////////////////////////////////////////////
        // driving current setting (0x00=4mA, 0x01=6mA, 0x02=8mA, 0x03=12mA)
        0x00,       //BYTE m_ucPanelDCKLCurrent;        //PANEL_DCLK_CURRENT         // DCLK current
        0x00,       //BYTE m_ucPanelDECurrent;          //PANEL_DE_CURRENT           // DE signal current
        0x00,       //BYTE m_ucPanelODDDataCurrent;     //PANEL_ODD_DATA_CURRENT     // odd data current
        0x00,       //BYTE m_ucPanelEvenDataCurrent;    //PANEL_EVEN_DATA_CURRENT    // even data current

        30,         //BYTE m_ucPanelOnTiming1;          //PANEL_ON_TIMING1          // time between panel & data while turn on power
        200,        //BYTE m_ucPanelOnTiming2;          //PANEL_ON_TIMING2          // time between data & back light while turn on power
        220,        //BYTE m_ucPanelOffTiming1;         //PANEL_OFF_TIMING1         // time between back light & data while turn off power
        10,         //BYTE m_ucPanelOffTiming2;         //PANEL_OFF_TIMING2         // time between data & panel while turn off power

        32,         //BYTE m_ucPanelHSyncWidth;         //PANEL_HSYNC_WIDTH
        32,         //BYTE m_ucPanelHSyncBackPorch;     //PANEL_HSYNC_BACK_PORCH

        8,          //BYTE m_ucPanelVSyncWidth;         //PANEL_VSYNC_WIDTH
        16,         //BYTE m_ucPanelBackPorch;          //PANEL_VSYNC_BACK_PORCH

        64,      //WORD m_wPanelHStart;              //PANEL_HSTART             (PANEL_HSYNC_WIDTH + PANEL_HSYNC_BACK_PORCH)
        24,       //WORD m_wPanelVStart;              //PANEL_VSTART             (PANEL_VSYNC_WIDTH + PANEL_VSYNC_BACK_PORCH)

        1024, //800,        //WORD m_wPanelWidth;               //PANEL_WIDTH
        600,  //480,       //WORD m_wPanelHeight;              //PANEL_HEIGHT

		1400, //978,       //WORD m_wPanelMaxHTotal;           //PANEL_MAX_HTOTAL
        1344, //928,       //WORD m_wPanelHTotal;              //PANEL_HTOTAL
        1114, //878,       //WORD m_wPanelMinHTotal;           //PANEL_MIN_HTOTAL

        800,  //575,        //WORD m_wPanelMaxVTotal;           //PANEL_MAX_VTOTAL
        635,  //525,        //WORD m_wPanelVTotal;              //PANEL_VTOTAL 20060511 chris :for Frame Lock operation
        610,  //475,        //WORD m_wPanelMinVTotal;           //PANEL_MIN_VTOTAL

        67, //DWORD m_dwPanelMaxDCLK;           //PANEL_MAX_DCLK
        51, //DWORD m_dwPanelDCLK;              //PANEL_DCLK
        41, //DWORD m_dwPanelMinDCLK;           //PANEL_MIN_DCLK

        0x0019,     //m_wSpreadSpectrumStep;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)
        0x00C0,     //m_wSpreadSpectrumSpan;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)

        0x6D,       //m_ucDimmingCtl

        255,        //m_ucMaxPWMVal;
        0x20,//63,  //m_ucMinPWMVal;
        0,          //BOOL m_bPanelDeinterMode  :1;     //PANEL_DEINTER_MODE
        E_PNL_ASPECT_RATIO_WIDE,
        //
        //  Board related params.
        //
       (LVDS_PN_SWAP_H<<8) | LVDS_PN_SWAP_L,            //MS_U16 m_u16LVDSTxSwapValue
       TI_8BIT_MODE,          //10bit ti bit mode
       OUTPUT_10BIT_MODE,
       0,          //   PANEL_SWAP_ODD_RG
       0,          //   PANEL_SWAP_EVEN_RG
       0,          //   PANEL_SWAP_ODD_GB
       0,          //   PANEL_SWAP_EVEN_GB
       0,          //   double clock
       0x306FA6,
       0x1D6343,
       E_PNL_CHG_VTOTAL,
       1,///<  PAFRC mixed with noise dither disable
    },
};

PanelType  tPanelHSD_070I_DW1_MST786_SZDEMO[] =
{
    {
        "HSD_070I_DW1",//m_pPanelName
        //////////////////////////////////////////////
        // Panel output
        //////////////////////////////////////////////
        0,          //BOOL m_bPanelDither :1;           //PANEL_DITHER          // 8/6 bits panel
        LINK_TTL,  //BOOL m_ePanelLinkType :2;         //PANEL_LINK

        0,          //BOOL m_bPanelDualPort :1;         //PANEL_DUAL_PORT

        1^PANEL_CONNECTOR_SWAP_PORT,                    // shall swap if
                                                        // (PANEL_SWAP_PORT XOR Board_Connect_Swap) is TRUE

        0,          //BOOL m_bPanelSwapOdd_ML   :1;     //PANEL_SWAP_ODD_ML
        0,          //BOOL m_bPanelSwapEven_ML  :1;     //PANEL_SWAP_EVEN_ML
        1,          //BOOL m_bPanelSwapOdd_RB   :1;     //PANEL_SWAP_ODD_RB
        0,          //BOOL m_bPanelSwapEven_RB  :1;     //PANEL_SWAP_EVEN_RB

        PANEL_SWAP_LVDS_POL,

        PANEL_SWAP_LVDS_CH,          //BOOL m_bPanelSwapLVDS_CH  :1;     //PANEL_SWAP_LVDS_CH
        PANEL_PDP_10BIT,          //BOOL m_bPanelPDP10BIT     :1;     //PANEL_PDP_10BIT
    	0,          //BOOL m_bPanelLVDS_TI_MODE :1;     //PANEL_LVDS_TI_MODE

        0x00,       //BYTE m_ucPanelDCLKDelay;          //PANEL_DCLK_DELAY
        0,          //BOOL m_bPanelInvDCLK  :1;         //PANEL_INV_DCLK
        0,          //BOOL m_bPanelInvDE        :1;     //PANEL_INV_DE
        0,          //BOOL m_bPanelInvHSync :1;         //PANEL_INV_HSYNC
        0,          //BOOL m_bPanelInvVSync :1;         //PANEL_INV_VSYNC

        ///////////////////////////////////////////////
        // Output tmming setting
        ///////////////////////////////////////////////
        // driving current setting (0x00=4mA, 0x01=6mA, 0x02=8mA, 0x03=12mA)
        0x00,       //BYTE m_ucPanelDCKLCurrent;        //PANEL_DCLK_CURRENT         // DCLK current
        0x00,       //BYTE m_ucPanelDECurrent;          //PANEL_DE_CURRENT           // DE signal current
        0x00,       //BYTE m_ucPanelODDDataCurrent;     //PANEL_ODD_DATA_CURRENT     // odd data current
        0x00,       //BYTE m_ucPanelEvenDataCurrent;    //PANEL_EVEN_DATA_CURRENT    // even data current

        30,         //BYTE m_ucPanelOnTiming1;          //PANEL_ON_TIMING1          // time between panel & data while turn on power
        200,        //BYTE m_ucPanelOnTiming2;          //PANEL_ON_TIMING2          // time between data & back light while turn on power
        220,        //BYTE m_ucPanelOffTiming1;         //PANEL_OFF_TIMING1         // time between back light & data while turn off power
        10,         //BYTE m_ucPanelOffTiming2;         //PANEL_OFF_TIMING2         // time between data & panel while turn off power

        48,         //BYTE m_ucPanelHSyncWidth;         //PANEL_HSYNC_WIDTH
        40,         //BYTE m_ucPanelHSyncBackPorch;     //PANEL_HSYNC_BACK_PORCH

        3,          //BYTE m_ucPanelVSyncWidth;         //PANEL_VSYNC_WIDTH
        13,         //BYTE m_ucPanelBackPorch;          //PANEL_VSYNC_BACK_PORCH

        40+40,      //WORD m_wPanelHStart;              //PANEL_HSTART             (PANEL_HSYNC_WIDTH + PANEL_HSYNC_BACK_PORCH)
        29+13,       //WORD m_wPanelVStart;              //PANEL_VSTART             (PANEL_VSYNC_WIDTH + PANEL_VSYNC_BACK_PORCH)

        800,        //WORD m_wPanelWidth;               //PANEL_WIDTH
        480,       //WORD m_wPanelHeight;              //PANEL_HEIGHT

        1070,      //WORD m_wPanelMaxHTotal;           //PANEL_MAX_HTOTAL
        928,       //WORD m_wPanelHTotal;              //PANEL_HTOTAL
        878,       //WORD m_wPanelMinHTotal;           //PANEL_MIN_HTOTAL

        625,        //WORD m_wPanelMaxVTotal;           //PANEL_MAX_VTOTAL
        525,        //WORD m_wPanelVTotal;              //PANEL_VTOTAL 20060511 chris :for Frame Lock operation
        475,        //WORD m_wPanelMinVTotal;           //PANEL_MIN_VTOTAL

        40, //DWORD m_dwPanelMaxDCLK;           //PANEL_MAX_DCLK
        30, //DWORD m_dwPanelDCLK;              //PANEL_DCLK
        25, //DWORD m_dwPanelMinDCLK;           //PANEL_MIN_DCLK

        0x0019,     //m_wSpreadSpectrumStep;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)
        0x00C0,     //m_wSpreadSpectrumSpan;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)

        0x6D,       //m_ucDimmingCtl

        255,        //m_ucMaxPWMVal;
        0x20,//63,  //m_ucMinPWMVal;
        0,          //BOOL m_bPanelDeinterMode  :1;     //PANEL_DEINTER_MODE
        E_PNL_ASPECT_RATIO_WIDE,
        //
        //  Board related params.
        //
       (LVDS_PN_SWAP_H<<8) | LVDS_PN_SWAP_L,            //MS_U16 m_u16LVDSTxSwapValue
       TI_10BIT_MODE,          //10bit ti bit mode
       OUTPUT_10BIT_MODE,
       0,          //   PANEL_SWAP_ODD_RG
       0,          //   PANEL_SWAP_EVEN_RG
       0,          //   PANEL_SWAP_ODD_GB
       0,          //   PANEL_SWAP_EVEN_GB
       0,          //   double clock
       0x4EEBF3,
       0x3137B6,
       E_PNL_CHG_VTOTAL,
       1,///<  PAFRC mixed with noise dither disable
    },
};

PanelType  tPanelHSD_062I_DW1_MST786_SZDEMO[] =
{
    {
        "HSD_062I_DW1",//m_pPanelName
        //////////////////////////////////////////////
        // Panel output
        //////////////////////////////////////////////
        0,          //BOOL m_bPanelDither :1;           //PANEL_DITHER          // 8/6 bits panel
        LINK_TTL,  //BOOL m_ePanelLinkType :2;         //PANEL_LINK

        0,          //BOOL m_bPanelDualPort :1;         //PANEL_DUAL_PORT

        1^PANEL_CONNECTOR_SWAP_PORT,                    // shall swap if
                                                        // (PANEL_SWAP_PORT XOR Board_Connect_Swap) is TRUE

        0,          //BOOL m_bPanelSwapOdd_ML   :1;     //PANEL_SWAP_ODD_ML
        0,          //BOOL m_bPanelSwapEven_ML  :1;     //PANEL_SWAP_EVEN_ML
        1,          //BOOL m_bPanelSwapOdd_RB   :1;     //PANEL_SWAP_ODD_RB
        0,          //BOOL m_bPanelSwapEven_RB  :1;     //PANEL_SWAP_EVEN_RB

        PANEL_SWAP_LVDS_POL,

        PANEL_SWAP_LVDS_CH,          //BOOL m_bPanelSwapLVDS_CH  :1;     //PANEL_SWAP_LVDS_CH
        PANEL_PDP_10BIT,          //BOOL m_bPanelPDP10BIT     :1;     //PANEL_PDP_10BIT
    	0,          //BOOL m_bPanelLVDS_TI_MODE :1;     //PANEL_LVDS_TI_MODE

        0x00,       //BYTE m_ucPanelDCLKDelay;          //PANEL_DCLK_DELAY
        0,          //BOOL m_bPanelInvDCLK  :1;         //PANEL_INV_DCLK
        0,          //BOOL m_bPanelInvDE        :1;     //PANEL_INV_DE
        1,          //BOOL m_bPanelInvHSync :1;         //PANEL_INV_HSYNC
        0,          //BOOL m_bPanelInvVSync :1;         //PANEL_INV_VSYNC

        ///////////////////////////////////////////////
        // Output tmming setting
        ///////////////////////////////////////////////
        // driving current setting (0x00=4mA, 0x01=6mA, 0x02=8mA, 0x03=12mA)
        0x00,       //BYTE m_ucPanelDCKLCurrent;        //PANEL_DCLK_CURRENT         // DCLK current
        0x00,       //BYTE m_ucPanelDECurrent;          //PANEL_DE_CURRENT           // DE signal current
        0x00,       //BYTE m_ucPanelODDDataCurrent;     //PANEL_ODD_DATA_CURRENT     // odd data current
        0x00,       //BYTE m_ucPanelEvenDataCurrent;    //PANEL_EVEN_DATA_CURRENT    // even data current

        30,         //BYTE m_ucPanelOnTiming1;          //PANEL_ON_TIMING1          // time between panel & data while turn on power
        200,        //BYTE m_ucPanelOnTiming2;          //PANEL_ON_TIMING2          // time between data & back light while turn on power
        220,        //BYTE m_ucPanelOffTiming1;         //PANEL_OFF_TIMING1         // time between back light & data while turn off power
        10,         //BYTE m_ucPanelOffTiming2;         //PANEL_OFF_TIMING2         // time between data & panel while turn off power

        48,         //BYTE m_ucPanelHSyncWidth;         //PANEL_HSYNC_WIDTH
        40,         //BYTE m_ucPanelHSyncBackPorch;     //PANEL_HSYNC_BACK_PORCH

        3,          //BYTE m_ucPanelVSyncWidth;         //PANEL_VSYNC_WIDTH
        13,         //BYTE m_ucPanelBackPorch;          //PANEL_VSYNC_BACK_PORCH

        40+40,      //WORD m_wPanelHStart;              //PANEL_HSTART             (PANEL_HSYNC_WIDTH + PANEL_HSYNC_BACK_PORCH)
        29+13,       //WORD m_wPanelVStart;              //PANEL_VSTART             (PANEL_VSYNC_WIDTH + PANEL_VSYNC_BACK_PORCH)

        800,        //WORD m_wPanelWidth;               //PANEL_WIDTH
        480,       //WORD m_wPanelHeight;              //PANEL_HEIGHT

        1070,      //WORD m_wPanelMaxHTotal;           //PANEL_MAX_HTOTAL
        928,       //WORD m_wPanelHTotal;              //PANEL_HTOTAL
        878,       //WORD m_wPanelMinHTotal;           //PANEL_MIN_HTOTAL

        625,        //WORD m_wPanelMaxVTotal;           //PANEL_MAX_VTOTAL
        525,        //WORD m_wPanelVTotal;              //PANEL_VTOTAL 20060511 chris :for Frame Lock operation
        475,        //WORD m_wPanelMinVTotal;           //PANEL_MIN_VTOTAL

        40, //DWORD m_dwPanelMaxDCLK;           //PANEL_MAX_DCLK
        30, //DWORD m_dwPanelDCLK;              //PANEL_DCLK
        25, //DWORD m_dwPanelMinDCLK;           //PANEL_MIN_DCLK

        0x0019,     //m_wSpreadSpectrumStep;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)
        0x00C0,     //m_wSpreadSpectrumSpan;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)

        0x6D,       //m_ucDimmingCtl

        255,        //m_ucMaxPWMVal;
        0x20,//63,  //m_ucMinPWMVal;
        0,          //BOOL m_bPanelDeinterMode  :1;     //PANEL_DEINTER_MODE
        E_PNL_ASPECT_RATIO_WIDE,
        //
        //  Board related params.
        //
       (LVDS_PN_SWAP_H<<8) | LVDS_PN_SWAP_L,            //MS_U16 m_u16LVDSTxSwapValue
       TI_10BIT_MODE,          //10bit ti bit mode
       OUTPUT_10BIT_MODE,
       0,          //   PANEL_SWAP_ODD_RG
       0,          //   PANEL_SWAP_EVEN_RG
       0,          //   PANEL_SWAP_ODD_GB
       0,          //   PANEL_SWAP_EVEN_GB
       0,          //   double clock
       0x4EEBF3,
       0x3137B6,
       E_PNL_CHG_VTOTAL,
       1,///<  PAFRC mixed with noise dither disable
    },
};
PanelType  tPanelHSD_070I_DW2_MST786_SZDEMO[] =
{
    {
        "HSD_070I_DW2",//m_pPanelName
        //////////////////////////////////////////////
        // Panel output
        //////////////////////////////////////////////
        0,          //BOOL m_bPanelDither :1;           //PANEL_DITHER          // 8/6 bits panel
        LINK_TTL,  //BOOL m_ePanelLinkType :2;         //PANEL_LINK

        0,          //BOOL m_bPanelDualPort :1;         //PANEL_DUAL_PORT

        1^PANEL_CONNECTOR_SWAP_PORT,                    // shall swap if
                                                        // (PANEL_SWAP_PORT XOR Board_Connect_Swap) is TRUE

        0,          //BOOL m_bPanelSwapOdd_ML   :1;     //PANEL_SWAP_ODD_ML
        0,          //BOOL m_bPanelSwapEven_ML  :1;     //PANEL_SWAP_EVEN_ML
        0,          //BOOL m_bPanelSwapOdd_RB   :1;     //PANEL_SWAP_ODD_RB
        0,          //BOOL m_bPanelSwapEven_RB  :1;     //PANEL_SWAP_EVEN_RB

        PANEL_SWAP_LVDS_POL,

        PANEL_SWAP_LVDS_CH,          //BOOL m_bPanelSwapLVDS_CH  :1;     //PANEL_SWAP_LVDS_CH
        PANEL_PDP_10BIT,          //BOOL m_bPanelPDP10BIT     :1;     //PANEL_PDP_10BIT
    	0,          //BOOL m_bPanelLVDS_TI_MODE :1;     //PANEL_LVDS_TI_MODE

        0x00,       //BYTE m_ucPanelDCLKDelay;          //PANEL_DCLK_DELAY
        0,          //BOOL m_bPanelInvDCLK  :1;         //PANEL_INV_DCLK
        0,          //BOOL m_bPanelInvDE        :1;     //PANEL_INV_DE
        0,          //BOOL m_bPanelInvHSync :1;         //PANEL_INV_HSYNC
        0,          //BOOL m_bPanelInvVSync :1;         //PANEL_INV_VSYNC

        ///////////////////////////////////////////////
        // Output tmming setting
        ///////////////////////////////////////////////
        // driving current setting (0x00=4mA, 0x01=6mA, 0x02=8mA, 0x03=12mA)
        0x00,       //BYTE m_ucPanelDCKLCurrent;        //PANEL_DCLK_CURRENT         // DCLK current
        0x00,       //BYTE m_ucPanelDECurrent;          //PANEL_DE_CURRENT           // DE signal current
        0x00,       //BYTE m_ucPanelODDDataCurrent;     //PANEL_ODD_DATA_CURRENT     // odd data current
        0x00,       //BYTE m_ucPanelEvenDataCurrent;    //PANEL_EVEN_DATA_CURRENT    // even data current

        30,         //BYTE m_ucPanelOnTiming1;          //PANEL_ON_TIMING1          // time between panel & data while turn on power
        200,        //BYTE m_ucPanelOnTiming2;          //PANEL_ON_TIMING2          // time between data & back light while turn on power
        220,        //BYTE m_ucPanelOffTiming1;         //PANEL_OFF_TIMING1         // time between back light & data while turn off power
        10,         //BYTE m_ucPanelOffTiming2;         //PANEL_OFF_TIMING2         // time between data & panel while turn off power

        48,         //BYTE m_ucPanelHSyncWidth;         //PANEL_HSYNC_WIDTH
        40,         //BYTE m_ucPanelHSyncBackPorch;     //PANEL_HSYNC_BACK_PORCH

        3,          //BYTE m_ucPanelVSyncWidth;         //PANEL_VSYNC_WIDTH
        13,         //BYTE m_ucPanelBackPorch;          //PANEL_VSYNC_BACK_PORCH

        40+40,      //WORD m_wPanelHStart;              //PANEL_HSTART             (PANEL_HSYNC_WIDTH + PANEL_HSYNC_BACK_PORCH)
        29+13,       //WORD m_wPanelVStart;              //PANEL_VSTART             (PANEL_VSYNC_WIDTH + PANEL_VSYNC_BACK_PORCH)

        800,        //WORD m_wPanelWidth;               //PANEL_WIDTH
        480,       //WORD m_wPanelHeight;              //PANEL_HEIGHT

        1070,      //WORD m_wPanelMaxHTotal;           //PANEL_MAX_HTOTAL
        928,       //WORD m_wPanelHTotal;              //PANEL_HTOTAL
        878,       //WORD m_wPanelMinHTotal;           //PANEL_MIN_HTOTAL

        625,        //WORD m_wPanelMaxVTotal;           //PANEL_MAX_VTOTAL
        525,        //WORD m_wPanelVTotal;              //PANEL_VTOTAL 20060511 chris :for Frame Lock operation
        475,        //WORD m_wPanelMinVTotal;           //PANEL_MIN_VTOTAL

        40, //DWORD m_dwPanelMaxDCLK;           //PANEL_MAX_DCLK
        30, //DWORD m_dwPanelDCLK;              //PANEL_DCLK
        25, //DWORD m_dwPanelMinDCLK;           //PANEL_MIN_DCLK

        0x0019,     //m_wSpreadSpectrumStep;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)
        0x00C0,     //m_wSpreadSpectrumSpan;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)

        0x6D,       //m_ucDimmingCtl

        255,        //m_ucMaxPWMVal;
        0x20,//63,  //m_ucMinPWMVal;
        0,          //BOOL m_bPanelDeinterMode  :1;     //PANEL_DEINTER_MODE
        E_PNL_ASPECT_RATIO_WIDE,
        //
        //  Board related params.
        //
       (LVDS_PN_SWAP_H<<8) | LVDS_PN_SWAP_L,            //MS_U16 m_u16LVDSTxSwapValue
       TI_10BIT_MODE,          //10bit ti bit mode
       OUTPUT_10BIT_MODE,
       0,          //   PANEL_SWAP_ODD_RG
       0,          //   PANEL_SWAP_EVEN_RG
       0,          //   PANEL_SWAP_ODD_GB
       0,          //   PANEL_SWAP_EVEN_GB
       0,          //   double clock
       0x4EEBF3,
       0x3137B6,
       E_PNL_CHG_VTOTAL,
       1,///<  PAFRC mixed with noise dither disable
    },
};
PanelType  tPanelHSD_070P_LVDS_800480[] =
{
    {
        "HSD_070P_LVDS800480",//m_pPanelName
        //////////////////////////////////////////////
        // Panel output
        //////////////////////////////////////////////
        0,          //BOOL m_bPanelDither :1;           //PANEL_DITHER          // 8/6 bits panel
        LINK_LVDS,  //LINK_TTL,  //BOOL m_ePanelLinkType :2;         //PANEL_LINK

        0,          //BOOL m_bPanelDualPort :1;         //PANEL_DUAL_PORT

        0,                    // shall swap if
                                                        // (PANEL_SWAP_PORT XOR Board_Connect_Swap) is TRUE

        0,          //BOOL m_bPanelSwapOdd_ML   :1;     //PANEL_SWAP_ODD_ML
        0,          //BOOL m_bPanelSwapEven_ML  :1;     //PANEL_SWAP_EVEN_ML
        0,          //BOOL m_bPanelSwapOdd_RB   :1;     //PANEL_SWAP_ODD_RB
        0,          //BOOL m_bPanelSwapEven_RB  :1;     //PANEL_SWAP_EVEN_RB

        0,

        0,          //BOOL m_bPanelSwapLVDS_CH  :1;     //PANEL_SWAP_LVDS_CH
        PANEL_PDP_10BIT,          //BOOL m_bPanelPDP10BIT     :1;     //PANEL_PDP_10BIT
    	1,          //BOOL m_bPanelLVDS_TI_MODE :1;     //PANEL_LVDS_TI_MODE

        0x00,       //BYTE m_ucPanelDCLKDelay;          //PANEL_DCLK_DELAY
        0,          //BOOL m_bPanelInvDCLK  :1;         //PANEL_INV_DCLK
        0,          //BOOL m_bPanelInvDE        :1;     //PANEL_INV_DE
        0,          //BOOL m_bPanelInvHSync :1;         //PANEL_INV_HSYNC
        0,          //BOOL m_bPanelInvVSync :1;         //PANEL_INV_VSYNC

        ///////////////////////////////////////////////
        // Output tmming setting
        ///////////////////////////////////////////////
        // driving current setting (0x00=4mA, 0x01=6mA, 0x02=8mA, 0x03=12mA)
        0x00,       //BYTE m_ucPanelDCKLCurrent;        //PANEL_DCLK_CURRENT         // DCLK current
        0x00,       //BYTE m_ucPanelDECurrent;          //PANEL_DE_CURRENT           // DE signal current
        0x00,       //BYTE m_ucPanelODDDataCurrent;     //PANEL_ODD_DATA_CURRENT     // odd data current
        0x00,       //BYTE m_ucPanelEvenDataCurrent;    //PANEL_EVEN_DATA_CURRENT    // even data current

        30,         //BYTE m_ucPanelOnTiming1;          //PANEL_ON_TIMING1          // time between panel & data while turn on power
        200,        //BYTE m_ucPanelOnTiming2;          //PANEL_ON_TIMING2          // time between data & back light while turn on power
        220,        //BYTE m_ucPanelOffTiming1;         //PANEL_OFF_TIMING1         // time between back light & data while turn off power
        10,         //BYTE m_ucPanelOffTiming2;         //PANEL_OFF_TIMING2         // time between data & panel while turn off power

        48,         //BYTE m_ucPanelHSyncWidth;         //PANEL_HSYNC_WIDTH
        40,         //BYTE m_ucPanelHSyncBackPorch;     //PANEL_HSYNC_BACK_PORCH

        3,          //BYTE m_ucPanelVSyncWidth;         //PANEL_VSYNC_WIDTH
        13,         //BYTE m_ucPanelBackPorch;          //PANEL_VSYNC_BACK_PORCH

        40+40,      //WORD m_wPanelHStart;              //PANEL_HSTART             (PANEL_HSYNC_WIDTH + PANEL_HSYNC_BACK_PORCH)
        29+13,       //WORD m_wPanelVStart;              //PANEL_VSTART             (PANEL_VSYNC_WIDTH + PANEL_VSYNC_BACK_PORCH)


        800, //800,        //WORD m_wPanelWidth;               //PANEL_WIDTH
        480,  //480,       //WORD m_wPanelHeight;              //PANEL_HEIGHT

        1070,      //WORD m_wPanelMaxHTotal;           //PANEL_MAX_HTOTAL
        928,       //WORD m_wPanelHTotal;              //PANEL_HTOTAL
        878,       //WORD m_wPanelMinHTotal;           //PANEL_MIN_HTOTAL

        625,        //WORD m_wPanelMaxVTotal;           //PANEL_MAX_VTOTAL
        525,        //WORD m_wPanelVTotal;              //PANEL_VTOTAL 20060511 chris :for Frame Lock operation
        475,        //WORD m_wPanelMinVTotal;           //PANEL_MIN_VTOTAL

        40, //DWORD m_dwPanelMaxDCLK;           //PANEL_MAX_DCLK
        30, //DWORD m_dwPanelDCLK;              //PANEL_DCLK
        25, //DWORD m_dwPanelMinDCLK;           //PANEL_MIN_DCLK

        0x0019,     //m_wSpreadSpectrumStep;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)
        0x00C0,     //m_wSpreadSpectrumSpan;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)

        0x6D,       //m_ucDimmingCtl

        255,        //m_ucMaxPWMVal;
        0x20,//63,  //m_ucMinPWMVal;
        0,          //BOOL m_bPanelDeinterMode  :1;     //PANEL_DEINTER_MODE
        E_PNL_ASPECT_RATIO_WIDE,
        //
        //  Board related params.
        //
       (LVDS_PN_SWAP_H<<8) | LVDS_PN_SWAP_L,            //MS_U16 m_u16LVDSTxSwapValue
       TI_8BIT_MODE,          //10bit ti bit mode
       OUTPUT_10BIT_MODE,
       0,          //   PANEL_SWAP_ODD_RG
       0,          //   PANEL_SWAP_EVEN_RG
       0,          //   PANEL_SWAP_ODD_GB
       0,          //   PANEL_SWAP_EVEN_GB
       0,          //   double clock
       0x4EEBF3,
       0x3137B6,
       E_PNL_CHG_VTOTAL,
       1,///<  PAFRC mixed with noise dither disable
    },
};

//! C3 LVDS panel: N070ICG_LD1 !//
PanelType  tPanelN070ICG_LD1_MST154A[] =
{
    {
        "N070ICG_LD1",//m_pPanelName
        //////////////////////////////////////////////
        // Panel output
        //////////////////////////////////////////////
        0,          //BOOL m_bPanelDither :1;           //PANEL_DITHER          // 8/6 bits panel
        LINK_LVDS,  //LINK_TTL,  //BOOL m_ePanelLinkType :2;         //PANEL_LINK

        0,          //BOOL m_bPanelDualPort :1;         //PANEL_DUAL_PORT

        0,                    // shall swap if
                                                        // (PANEL_SWAP_PORT XOR Board_Connect_Swap) is TRUE

        0,          //BOOL m_bPanelSwapOdd_ML   :1;     //PANEL_SWAP_ODD_ML
        0,          //BOOL m_bPanelSwapEven_ML  :1;     //PANEL_SWAP_EVEN_ML
        0,          //BOOL m_bPanelSwapOdd_RB   :1;     //PANEL_SWAP_ODD_RB
        0,          //BOOL m_bPanelSwapEven_RB  :1;     //PANEL_SWAP_EVEN_RB

        0,

        0,          //BOOL m_bPanelSwapLVDS_CH  :1;     //PANEL_SWAP_LVDS_CH
        PANEL_PDP_10BIT,          //BOOL m_bPanelPDP10BIT     :1;     //PANEL_PDP_10BIT
             1,          //BOOL m_bPanelLVDS_TI_MODE :1;     //PANEL_LVDS_TI_MODE

        0x00,       //BYTE m_ucPanelDCLKDelay;          //PANEL_DCLK_DELAY
        0,          //BOOL m_bPanelInvDCLK  :1;         //PANEL_INV_DCLK
        0,          //BOOL m_bPanelInvDE        :1;     //PANEL_INV_DE
        0,          //BOOL m_bPanelInvHSync :1;         //PANEL_INV_HSYNC
        0,          //BOOL m_bPanelInvVSync :1;         //PANEL_INV_VSYNC

        ///////////////////////////////////////////////
        // Output tmming setting
        ///////////////////////////////////////////////
        // driving current setting (0x00=4mA, 0x01=6mA, 0x02=8mA, 0x03=12mA)
        0x00,       //BYTE m_ucPanelDCKLCurrent;        //PANEL_DCLK_CURRENT         // DCLK current
        0x00,       //BYTE m_ucPanelDECurrent;          //PANEL_DE_CURRENT           // DE signal current
        0x00,       //BYTE m_ucPanelODDDataCurrent;     //PANEL_ODD_DATA_CURRENT     // odd data current
        0x00,       //BYTE m_ucPanelEvenDataCurrent;    //PANEL_EVEN_DATA_CURRENT    // even data current

        30,         //BYTE m_ucPanelOnTiming1;          //PANEL_ON_TIMING1          // time between panel & data while turn on power
        200,        //BYTE m_ucPanelOnTiming2;          //PANEL_ON_TIMING2          // time between data & back light while turn on power
        220,        //BYTE m_ucPanelOffTiming1;         //PANEL_OFF_TIMING1         // time between back light & data while turn off power
        10,         //BYTE m_ucPanelOffTiming2;         //PANEL_OFF_TIMING2         // time between data & panel while turn off power

        32,         //BYTE m_ucPanelHSyncWidth;         //PANEL_HSYNC_WIDTH
        32,         //BYTE m_ucPanelHSyncBackPorch;     //PANEL_HSYNC_BACK_PORCH

        8,          //BYTE m_ucPanelVSyncWidth;         //PANEL_VSYNC_WIDTH
        16,         //BYTE m_ucPanelBackPorch;          //PANEL_VSYNC_BACK_PORCH

        64,      //WORD m_wPanelHStart;              //PANEL_HSTART             (PANEL_HSYNC_WIDTH + PANEL_HSYNC_BACK_PORCH)
        24,       //WORD m_wPanelVStart;              //PANEL_VSTART             (PANEL_VSYNC_WIDTH + PANEL_VSYNC_BACK_PORCH)

        1280, //800,        //WORD m_wPanelWidth;               //PANEL_WIDTH
        800,  //480,       //WORD m_wPanelHeight;              //PANEL_HEIGHT

        1410, //978,       //WORD m_wPanelMaxHTotal;           //PANEL_MAX_HTOTAL
        1440, //928,       //WORD m_wPanelHTotal;              //PANEL_HTOTAL
        1470, //878,       //WORD m_wPanelMinHTotal;           //PANEL_MIN_HTOTAL

        813,  //575,        //WORD m_wPanelMaxVTotal;           //PANEL_MAX_VTOTAL
        823,  //525,        //WORD m_wPanelVTotal;              //PANEL_VTOTAL 20060511 chris :for Frame Lock operation
        833,  //475,        //WORD m_wPanelMinVTotal;           //PANEL_MIN_VTOTAL

        73, //DWORD m_dwPanelMaxDCLK;           //PANEL_MAX_DCLK
        71, //DWORD m_dwPanelDCLK;              //PANEL_DCLK
        68, //DWORD m_dwPanelMinDCLK;           //PANEL_MIN_DCLK

        0x0019,     //m_wSpreadSpectrumStep;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)
        0x00C0,     //m_wSpreadSpectrumSpan;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)

        0x6D,       //m_ucDimmingCtl

        255,        //m_ucMaxPWMVal;
        0x20,//63,  //m_ucMinPWMVal;
        0,          //BOOL m_bPanelDeinterMode  :1;     //PANEL_DEINTER_MODE
        E_PNL_ASPECT_RATIO_WIDE,
        //
        //  Board related params.
        //
       (LVDS_PN_SWAP_H<<8) | LVDS_PN_SWAP_L,            //MS_U16 m_u16LVDSTxSwapValue
       TI_6BIT_MODE,          //10bit ti bit mode
       OUTPUT_10BIT_MODE,
       0,          //   PANEL_SWAP_ODD_RG
       0,          //   PANEL_SWAP_EVEN_RG
       0,          //   PANEL_SWAP_ODD_GB
       0,          //   PANEL_SWAP_EVEN_GB
       0,          //   double clock
       0x1AE12A,
       0x1CB676,
       E_PNL_CHG_VTOTAL,
       1,///<  PAFRC mixed with noise dither disable
    },
};

PanelType  tPanelHSD_062I_DW2__MST786_SZDEMO[] =
{
    {
        "HSD_062I_DW2",//m_pPanelName
        //////////////////////////////////////////////
        // Panel output
        //////////////////////////////////////////////
        0,          //BOOL m_bPanelDither :1;           //PANEL_DITHER          // 8/6 bits panel
        LINK_TTL,  //BOOL m_ePanelLinkType :2;         //PANEL_LINK

        0,          //BOOL m_bPanelDualPort :1;         //PANEL_DUAL_PORT

        1^PANEL_CONNECTOR_SWAP_PORT,                    // shall swap if
                                                        // (PANEL_SWAP_PORT XOR Board_Connect_Swap) is TRUE

        0,          //BOOL m_bPanelSwapOdd_ML   :1;     //PANEL_SWAP_ODD_ML
        0,          //BOOL m_bPanelSwapEven_ML  :1;     //PANEL_SWAP_EVEN_ML
        0,          //BOOL m_bPanelSwapOdd_RB   :1;     //PANEL_SWAP_ODD_RB
        0,          //BOOL m_bPanelSwapEven_RB  :1;     //PANEL_SWAP_EVEN_RB

        PANEL_SWAP_LVDS_POL,

        PANEL_SWAP_LVDS_CH,          //BOOL m_bPanelSwapLVDS_CH  :1;     //PANEL_SWAP_LVDS_CH
        PANEL_PDP_10BIT,          //BOOL m_bPanelPDP10BIT     :1;     //PANEL_PDP_10BIT
    	0,          //BOOL m_bPanelLVDS_TI_MODE :1;     //PANEL_LVDS_TI_MODE

        0x00,       //BYTE m_ucPanelDCLKDelay;          //PANEL_DCLK_DELAY
        0,          //BOOL m_bPanelInvDCLK  :1;         //PANEL_INV_DCLK
        0,          //BOOL m_bPanelInvDE        :1;     //PANEL_INV_DE
        1,          //BOOL m_bPanelInvHSync :1;         //PANEL_INV_HSYNC
        0,          //BOOL m_bPanelInvVSync :1;         //PANEL_INV_VSYNC

        ///////////////////////////////////////////////
        // Output tmming setting
        ///////////////////////////////////////////////
        // driving current setting (0x00=4mA, 0x01=6mA, 0x02=8mA, 0x03=12mA)
        0x01,       //BYTE m_ucPanelDCKLCurrent;        //PANEL_DCLK_CURRENT         // DCLK current
        0x01,       //BYTE m_ucPanelDECurrent;          //PANEL_DE_CURRENT           // DE signal current
        0x01,       //BYTE m_ucPanelODDDataCurrent;     //PANEL_ODD_DATA_CURRENT     // odd data current
        0x01,       //BYTE m_ucPanelEvenDataCurrent;    //PANEL_EVEN_DATA_CURRENT    // even data current

        30,         //BYTE m_ucPanelOnTiming1;          //PANEL_ON_TIMING1          // time between panel & data while turn on power
        200,        //BYTE m_ucPanelOnTiming2;          //PANEL_ON_TIMING2          // time between data & back light while turn on power
        220,        //BYTE m_ucPanelOffTiming1;         //PANEL_OFF_TIMING1         // time between back light & data while turn off power
        10,         //BYTE m_ucPanelOffTiming2;         //PANEL_OFF_TIMING2         // time between data & panel while turn off power

        48,         //BYTE m_ucPanelHSyncWidth;         //PANEL_HSYNC_WIDTH
        40,         //BYTE m_ucPanelHSyncBackPorch;     //PANEL_HSYNC_BACK_PORCH

        3,          //BYTE m_ucPanelVSyncWidth;         //PANEL_VSYNC_WIDTH
        13,         //BYTE m_ucPanelBackPorch;          //PANEL_VSYNC_BACK_PORCH

        40+40,      //WORD m_wPanelHStart;              //PANEL_HSTART             (PANEL_HSYNC_WIDTH + PANEL_HSYNC_BACK_PORCH)
        29+13,       //WORD m_wPanelVStart;              //PANEL_VSTART             (PANEL_VSYNC_WIDTH + PANEL_VSYNC_BACK_PORCH)

        800,        //WORD m_wPanelWidth;               //PANEL_WIDTH
        480,       //WORD m_wPanelHeight;              //PANEL_HEIGHT

        1070,      //WORD m_wPanelMaxHTotal;           //PANEL_MAX_HTOTAL
        928,       //WORD m_wPanelHTotal;              //PANEL_HTOTAL
        878,       //WORD m_wPanelMinHTotal;           //PANEL_MIN_HTOTAL

        625,        //WORD m_wPanelMaxVTotal;           //PANEL_MAX_VTOTAL
        525,        //WORD m_wPanelVTotal;              //PANEL_VTOTAL 20060511 chris :for Frame Lock operation
        475,        //WORD m_wPanelMinVTotal;           //PANEL_MIN_VTOTAL

        40, //DWORD m_dwPanelMaxDCLK;           //PANEL_MAX_DCLK
        30, //DWORD m_dwPanelDCLK;              //PANEL_DCLK
        25, //DWORD m_dwPanelMinDCLK;           //PANEL_MIN_DCLK

        0x0019,     //m_wSpreadSpectrumStep;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)
        0x00C0,     //m_wSpreadSpectrumSpan;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)

        0x6D,       //m_ucDimmingCtl

        255,        //m_ucMaxPWMVal;
        0x20,//63,  //m_ucMinPWMVal;
        0,          //BOOL m_bPanelDeinterMode  :1;     //PANEL_DEINTER_MODE
        E_PNL_ASPECT_RATIO_WIDE,
        //
        //  Board related params.
        //
       (LVDS_PN_SWAP_H<<8) | LVDS_PN_SWAP_L,            //MS_U16 m_u16LVDSTxSwapValue
       TI_10BIT_MODE,          //10bit ti bit mode
       OUTPUT_10BIT_MODE,
       0,          //   PANEL_SWAP_ODD_RG
       0,          //   PANEL_SWAP_EVEN_RG
       0,          //   PANEL_SWAP_ODD_GB
       0,          //   PANEL_SWAP_EVEN_GB
       0,          //   double clock
       0x4EEBF3,
       0x3137B6,
       E_PNL_CHG_VTOTAL,
       1,///<  PAFRC mixed with noise dither disable
    },
};
PanelType  tPanelHSD_070I_DW1__MST786_MODE2[] =
{
    {
        "HSD_070I_DW1",//m_pPanelName
        //////////////////////////////////////////////
        // Panel output
        //////////////////////////////////////////////
        0,          //BOOL m_bPanelDither :1;           //PANEL_DITHER          // 8/6 bits panel
        LINK_TTL,  //BOOL m_ePanelLinkType :2;         //PANEL_LINK

        0,          //BOOL m_bPanelDualPort :1;         //PANEL_DUAL_PORT

        1^PANEL_CONNECTOR_SWAP_PORT,                    // shall swap if
                                                        // (PANEL_SWAP_PORT XOR Board_Connect_Swap) is TRUE

        1,          //BOOL m_bPanelSwapOdd_ML   :1;     //PANEL_SWAP_ODD_ML
        0,          //BOOL m_bPanelSwapEven_ML  :1;     //PANEL_SWAP_EVEN_ML
        0,          //BOOL m_bPanelSwapOdd_RB   :1;     //PANEL_SWAP_ODD_RB
        0,          //BOOL m_bPanelSwapEven_RB  :1;     //PANEL_SWAP_EVEN_RB

        PANEL_SWAP_LVDS_POL,

        PANEL_SWAP_LVDS_CH,          //BOOL m_bPanelSwapLVDS_CH  :1;     //PANEL_SWAP_LVDS_CH
        PANEL_PDP_10BIT,          //BOOL m_bPanelPDP10BIT     :1;     //PANEL_PDP_10BIT
    	0,          //BOOL m_bPanelLVDS_TI_MODE :1;     //PANEL_LVDS_TI_MODE

        0x00,       //BYTE m_ucPanelDCLKDelay;          //PANEL_DCLK_DELAY
        0,          //BOOL m_bPanelInvDCLK  :1;         //PANEL_INV_DCLK
        0,          //BOOL m_bPanelInvDE        :1;     //PANEL_INV_DE
        0,          //BOOL m_bPanelInvHSync :1;         //PANEL_INV_HSYNC
        0,          //BOOL m_bPanelInvVSync :1;         //PANEL_INV_VSYNC

        ///////////////////////////////////////////////
        // Output tmming setting
        ///////////////////////////////////////////////
        // driving current setting (0x00=4mA, 0x01=6mA, 0x02=8mA, 0x03=12mA)
        0x01,       //BYTE m_ucPanelDCKLCurrent;        //PANEL_DCLK_CURRENT         // DCLK current
        0x01,       //BYTE m_ucPanelDECurrent;          //PANEL_DE_CURRENT           // DE signal current
        0x01,       //BYTE m_ucPanelODDDataCurrent;     //PANEL_ODD_DATA_CURRENT     // odd data current
        0x01,       //BYTE m_ucPanelEvenDataCurrent;    //PANEL_EVEN_DATA_CURRENT    // even data current

        30,         //BYTE m_ucPanelOnTiming1;          //PANEL_ON_TIMING1          // time between panel & data while turn on power
        200,        //BYTE m_ucPanelOnTiming2;          //PANEL_ON_TIMING2          // time between data & back light while turn on power
        220,        //BYTE m_ucPanelOffTiming1;         //PANEL_OFF_TIMING1         // time between back light & data while turn off power
        10,         //BYTE m_ucPanelOffTiming2;         //PANEL_OFF_TIMING2         // time between data & panel while turn off power

        48,         //BYTE m_ucPanelHSyncWidth;         //PANEL_HSYNC_WIDTH
        40,         //BYTE m_ucPanelHSyncBackPorch;     //PANEL_HSYNC_BACK_PORCH

        3,          //BYTE m_ucPanelVSyncWidth;         //PANEL_VSYNC_WIDTH
        13,         //BYTE m_ucPanelBackPorch;          //PANEL_VSYNC_BACK_PORCH

        40+40,      //WORD m_wPanelHStart;              //PANEL_HSTART             (PANEL_HSYNC_WIDTH + PANEL_HSYNC_BACK_PORCH)
        29+13,       //WORD m_wPanelVStart;              //PANEL_VSTART             (PANEL_VSYNC_WIDTH + PANEL_VSYNC_BACK_PORCH)

        800,        //WORD m_wPanelWidth;               //PANEL_WIDTH
        480,       //WORD m_wPanelHeight;              //PANEL_HEIGHT

        1070,      //WORD m_wPanelMaxHTotal;           //PANEL_MAX_HTOTAL
        928,       //WORD m_wPanelHTotal;              //PANEL_HTOTAL
        878,       //WORD m_wPanelMinHTotal;           //PANEL_MIN_HTOTAL

        625,        //WORD m_wPanelMaxVTotal;           //PANEL_MAX_VTOTAL
        525,        //WORD m_wPanelVTotal;              //PANEL_VTOTAL 20060511 chris :for Frame Lock operation
        475,        //WORD m_wPanelMinVTotal;           //PANEL_MIN_VTOTAL

        40, //DWORD m_dwPanelMaxDCLK;           //PANEL_MAX_DCLK
        30, //DWORD m_dwPanelDCLK;              //PANEL_DCLK
        25, //DWORD m_dwPanelMinDCLK;           //PANEL_MIN_DCLK

        0x0019,     //m_wSpreadSpectrumStep;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)
        0x00C0,     //m_wSpreadSpectrumSpan;        //Value for Spread_Spectrum_Control register(B7..3:Period,B2..0:Amplitude)

        0x6D,       //m_ucDimmingCtl

        255,        //m_ucMaxPWMVal;
        0x20,//63,  //m_ucMinPWMVal;
        0,          //BOOL m_bPanelDeinterMode  :1;     //PANEL_DEINTER_MODE
        E_PNL_ASPECT_RATIO_WIDE,
        //
        //  Board related params.
        //
       (LVDS_PN_SWAP_H<<8) | LVDS_PN_SWAP_L,            //MS_U16 m_u16LVDSTxSwapValue
       TI_10BIT_MODE,          //10bit ti bit mode
       OUTPUT_8BIT_MODE,
       0,          //   PANEL_SWAP_ODD_RG
       0,          //   PANEL_SWAP_EVEN_RG
       0,          //   PANEL_SWAP_ODD_GB
       0,          //   PANEL_SWAP_EVEN_GB
       0,          //   double clock
       0x4EEBF3,
       0x3137B6,
       E_PNL_CHG_VTOTAL,
       1,///<  PAFRC mixed with noise dither disable
    },
};
#endif
