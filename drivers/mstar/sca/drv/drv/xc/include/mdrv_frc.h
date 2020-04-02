////////////////////////////////////////////////////////////////////////////////

//

// Copyright (c) 2008-2010 MStar Semiconductor, Inc.

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



///////////////////////////////////////////////////////////////////////////////////////////////////

/// file    mdrv_frc.h

/// @brief  Driver Interface

/// @author MStar Semiconductor Inc.

///////////////////////////////////////////////////////////////////////////////////////////////////



#ifndef _MDRV_FRC_H_

#define _MDRV_FRC_H_

#include "mhal_frc.h"

//-------------------------------------------------------------------------------------------------

//  Macro and Define

//-------------------------------------------------------------------------------------------------



typedef struct MST_PANEL_INFO_s

{

    // Basic

    MS_U16 u16HStart; //ursa scaler

    MS_U16 u16VStart; //ursa scaler

    MS_U16 u16Width; //ursa scaler

    MS_U16 u16Height; //ursa scaler

    MS_U16 u16HTotal; //ursa scaler

    MS_U16 u16VTotal; //ursa scaler



    MS_U16 u16DE_VStart;



    MS_U16 u16DefaultVFreq;



    // LPLL

    MS_U16 u16LPLL_InputDiv;

    MS_U16 u16LPLL_LoopDiv;

    MS_U16 u16LPLL_OutputDiv;



    MS_U8  u8LPLL_Type;

    MS_U8  u8LPLL_Mode;



    // sync

    MS_U8  u8HSyncWidth;

    MS_BOOL bPnlDblVSync;



    // output control

    MS_U16 u16OCTRL;

    MS_U16 u16OSTRL;

    MS_U16 u16ODRV;

    MS_U16 u16DITHCTRL;



    // MOD

    MS_U16 u16MOD_CTRL0;  // BIT2: tiMode, BIT5: lvdsSwapPol, BIT6: lvdsSwapCh

    MS_U8  u8MOD_CTRL2;

    MS_U16 u16MOD_CTRL9;

    MS_U16 u16MOD_CTRLA;  // BIT2: invertDE, BIT3: invertVS, BIT12: invertHS

    MS_U8  u8MOD_CTRLB;   // BIT0~1: panelBitNums



    MS_U8  u8MOD_CTRL77;    //pre-emphasis level

    MS_U8  u8MOD_CTRL78;

    //LGE [vivakjh] 2008/11/12     Add for DVB PDP Panel

    //Additional Info.(V Total)

    MS_U16 u16VTotal60Hz; //ursa scaler

    MS_U16 u16VTotal50Hz; //ursa scaler

    MS_U16 u16VTotal48Hz; //ursa scaler

    //[vivakjh] 2008/12/23    Add for adjusting the MRE in PDP S6

    MS_U16 u16VStart60Hz;

    MS_U16 u16VStart50Hz;

    MS_U16 u16VStart48Hz;

    MS_U16 u16VBackPorch60Hz;

    MS_U16 u16VBackPorch50Hz;

    MS_U16 u16VBackPorch48Hz;



    /*Panel Option

    0: LCD

    1: PDP

    2: LCD_NO_FRC

    3: LCD_TCON

    */

    MS_U8    u8LCDorPDP;



    MS_U32 u32LimitD5d6d7; //thchen 20081216

    MS_U16 u16LimitOffset; //thchen 20081216

    MS_U8  u8LvdsSwingUp;

    MS_BOOL bTTL_10BIT;

    MS_BOOL bOD_DataPath;

    

//-------------------------------------------------------------------------------------------------

// FRC Control

//-------------------------------------------------------------------------------------------------

    MS_BOOL bFRC;

    MS_U16 u16MOD_SwingLevel;

    MS_U16 u16MOD_SwingCLK;

    MS_U16 u16output_cfg_10;

    MS_U16 u16output_cfg_11;

    MS_U16 u16output_cfg_12;

    MS_U8 u8output_cfg_13;

    MS_U8 u8PanelNoiseDith;

    MS_U8 u8lvdsSwapCh;

    MS_U8 u8FRC3DPanelType;

    

    MS_BOOL bdither6Bits;

    MS_BOOL blvdsShiftPair;

    MS_BOOL blvdsSwapPair;



// TGEN

    MS_U8  u8HSyncStart;

    MS_U8  u8HSyncEnd;

    MS_U8  u8VSyncStart;

    MS_U8  u8VSyncEnd;

    MS_U16 u16VTrigX;

    MS_U16 u16VTrigY;



// EPI    

    MS_BOOL bepiLRSwap;

    MS_BOOL bepiLMirror;

    MS_BOOL bepiRMirror;



} MST_PANEL_INFO_t, *PMST_PANEL_INFO_t;



typedef struct

{

//-------------------------------------------------------------------------------------------------

// FRC Control

//-------------------------------------------------------------------------------------------------

    MS_U16 u16MOD_SwingLevel;

    MS_U16 u16MOD_SwingCLK;

    MS_U8 u8PanelNoiseDith;

    MS_U8 u8lvdsSwapCh;

    MS_BOOL bdither6Bits;

    MS_BOOL blvdsShiftPair;

    MS_BOOL blvdsSwapPair;



// EPI    

    MS_BOOL bepiLRSwap;

    MS_BOOL bepiLMirror;

    MS_BOOL bepiRMirror;

} MST_PANEL_FRC_INFO_t, *PMST_PANEL_FRC_INFO_t;





typedef struct 

{

    MS_U32 u32MfcBase; 

    MS_U32 u32MfcSize;

    

    MS_U32 u32IpYcoutBase;

    MS_U32 u32IpYcoutSize;

    MS_U32 u32IpMrBase;

    MS_U32 u32IpMrSize;



    MS_U32 u32GammaBase;

    MS_U32 u32GammaSize;

    

    MS_U32 u32OdBase;

    MS_U32 u32OdSize;

    MS_U32 u32OdLsbBase;

    MS_U32 u32OdLsbSize;



    MS_U32 u32AutoBist;



    MS_U8  u8MfcMode;

    MS_U8  u8IpMode;    

    MS_U8  u8GammaMode;

    MS_U8  u8OdMode;    

}MST_MFC_MiuBaseAddr_t, *PMST_MFC_MiuBaseAddr_t;



typedef struct MST_FRC_SYS_INFO_s

{    

    //PMST_PANEL_INFO_t pPanelInfo;    // panel information

    MS_U16 u16HStart; //ursa scaler

    MS_U16 u16VStart; //ursa scaler

    MS_U16 u16Width; //ursa scaler

    MS_U16 u16Height; //ursa scaler

    MS_U16 u16HTotal; //ursa scaler

    MS_U16 u16VTotal; //ursa scaler



    //titania to URSA    

    MS_U8  u8LVDSTxChannel;         //Single, Dual

    MS_U8  u8LVDSTxBitNum;          //8bits, 10 bits

    MS_U8  u8LVDSTxTiMode;          //Thin/Ti mode scaler 40-bit2

    MS_U8  u8LVDSTxSwapMsbLsb;     //ursa scaler

    MS_U8  u8LVDSTxSwap_P_N;         //ursa scaler

    MS_U8  u8LVDSTxSwapOddEven;     //ursa scaler



    //URSA to Panel info

    MS_U8  u8PanelVfreq;             //Panel frame rate 60Hz, 120Hz, 240Hz

    MS_U8  u8PanelChannel;         //Single, Dual, Quad, Quad_LR

    MS_U8    u8PanelLVDSSwapCH;         //LVDS chenel swap ABCD

    MS_U8  u8PanelBitNum;             //Panel bit number

    MS_U8    u8PanelLVDSShiftPair;     //ursa scaler

    MS_U8    u8PanelLVDSTiMode;         //Panel TI/Thin mode

    MS_U8    u8PanelLVDSSwapPol;     //Panel LVDS polarity swap

    MS_U8    u8PanelLVDSSwapPair;     //Panel LVDS pair swap

    

    MS_U16    u16MFCMemoryClk;                //MFC memory clock MHz

    MS_U16    u16MFCMemoryType;            //MFC memory type                 

    MS_U8    u8PanelIncVtotalFor50Hz;    //Change Vtotal for DCLK

    MS_U8    u8PanelType;                  //TTL, Mini_LVDS, LVDS

    MS_U8    u8PanelDither;

    MS_U8    u8PanelBlankCPVC;            //Panel Mini LVDS use

    MS_U8    u8PanelBlankOEC;            //Panel Mini LVDS use

    MS_U8    u8PanelBlankTPC;            //Panel Mini LVDS use

    MS_U8    u8PanelBlankSTHC;            //Panel Mini LVDS use

    MS_U8    u8PanelCSC;                    //LVDS CSC enable/disable

    MS_U8    u8PanelGAMMA;                //Panel GAMMA enable/disable    

    MS_U8  u8UseMPIF;

}MST_FRC_SYS_INFO_t, *PMST_FRC_SYS_INFO_t;



typedef struct

{

    MS_PHYADDR PhyAddr;

    MS_U16 u16MaxCmdCnt;

    MS_BOOL bEnable;

}MS_AutoDownLoad_Info;



typedef struct

{

    MS_U32 u32B;

    MS_U32 u32G;

    MS_U32 u32R;

    MS_U32 u32Enable;

} MS_FRC_ADLG_TBL;



typedef enum

{    

    E_FRC_PNL_TYPE_2D = 0,    

    E_FRC_PNL_TYPE_PASSIVE,    

    E_FRC_PNL_TYPE_ACTIVE,    

    E_FRC_PNL_TYPE_PDP,

} E_FRC_3D_PANEL_TYPE;



typedef enum

{

    E_FRC_INPUT_2D = 0,       // 2D

    E_FRC_INPUT_3D_SBS,       // side by side

    E_FRC_INPUT_3D_TD,        // Top-Down

    E_FRC_INPUT_3D_CB,        // Check Board

    E_FRC_INPUT_3D_FI_1080p,  // Frame Interleave

    E_FRC_INPUT_3D_FI_720p,   // Frame Interleave

    E_FRC_INPUT_3D_LA,        // Line Alternative

    E_FRC_INPUT_3D_FPP_1080p, // Frame Packing

    E_FRC_INPUT_3D_FPP_720p,  // Frame Packing

}E_FRC_3D_INPUT_FMT;



typedef enum

{

    E_FRC_3D_CFG_NORMAL = 0,    // normal 3D

    E_FRC_3D_CFG_LR_SWAP,       // L/R swap

    E_FRC_3D_CFG_L_ONLY,        // Only L side

    E_FRC_3D_CFG_R_ONLY,        // Only R side

}E_FRC_3D_MODE_CFG;





//-------------------------------------------------------------------------------------------------

//  Function and Variable

//-------------------------------------------------------------------------------------------------

void MDrv_FRC_ByPass_Enable(MS_BOOL bEnable);



void MDrv_FRC_Tx_SetTgen(PMST_PANEL_INFO_t pPanelInfo);

void MDrv_FRC_SetMemFormat(PFRC_INFO_t pFRCInfo);

void MDrv_FRC_IPM_Init(PMST_PANEL_INFO_t pPanelInfo, PFRC_INFO_t pFRCInfo);

void MDrv_FRC_OPM_Init(void);

void MDrv_FRC_Init(PMST_PANEL_INFO_t pPanelInfo, PFRC_INFO_t pFRCInfo); 

void MDrv_FRC_Set_3D_QMap(MS_U8 u8FRC3DPanelType, MS_U16 u16sc_in_3dformat, MS_U16 u16sc_out_3dformat, MS_U8 u83D_FI_out);

void MDrv_FRC_AutoDownLoad_init(MS_PHYADDR PhyAddr, MS_U32 u32BufByteLen);

void MDrv_FRC_ADLG_Trigger(FRC_CLIENT_TABLE client,MS_PHYADDR startAddr, MS_U16 u16CmdCnt);

void MDrv_FRC_ADLG_WritetoDRAM(FRC_CLIENT_TABLE client,MS_U8 *pR, MS_U8 *pG, MS_U8 *pB, MS_U16 u16Count, MS_U16 *pMaxGammaValue);

void MDrv_FRC_SetGammaTbl(MS_U8* pu8GammaTab[3]);

void MDrv_FRC_ADLG_Fire(FRC_CLIENT_TABLE client,MS_U8 *pR, MS_U8 *pG, MS_U8 *pB, MS_U16 u16Count, MS_U16 *pMaxGammaValue);

void MDrv_FRC_OPM_SetBaseOfset(FRC_INFO_t *FRCInfo, E_XC_3D_OUTPUT_MODE u16out_3dformat);





#endif

