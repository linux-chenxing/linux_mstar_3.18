//<MStar Software>
//******************************************************************************
// MStar Software
// Copyright (c) 2010 - 2012 MStar Semiconductor, Inc. All rights reserved.
// All software, firmware and related documentation herein ("MStar Software") are
// intellectual property of MStar Semiconductor, Inc. ("MStar") and protected by
// law, including, but not limited to, copyright law and international treaties.
// Any use, modification, reproduction, retransmission, or republication of all
// or part of MStar Software is expressly prohibited, unless prior written
// permission has been granted by MStar.
//
// By accessing, browsing and/or using MStar Software, you acknowledge that you
// have read, understood, and agree, to be bound by below terms ("Terms") and to
// comply with all applicable laws and regulations:
//
// 1. MStar shall retain any and all right, ownership and interest to MStar
//    Software and any modification/derivatives thereof.
//    No right, ownership, or interest to MStar Software and any
//    modification/derivatives thereof is transferred to you under Terms.
//
// 2. You understand that MStar Software might include, incorporate or be
//    supplied together with third party`s software and the use of MStar
//    Software may require additional licenses from third parties.
//    Therefore, you hereby agree it is your sole responsibility to separately
//    obtain any and all third party right and license necessary for your use of
//    such third party`s software.
//
// 3. MStar Software and any modification/derivatives thereof shall be deemed as
//    MStar`s confidential information and you agree to keep MStar`s
//    confidential information in strictest confidence and not disclose to any
//    third party.
//
// 4. MStar Software is provided on an "AS IS" basis without warranties of any
//    kind. Any warranties are hereby expressly disclaimed by MStar, including
//    without limitation, any warranties of merchantability, non-infringement of
//    intellectual property rights, fitness for a particular purpose, error free
//    and in conformity with any international standard.  You agree to waive any
//    claim against MStar for any loss, damage, cost or expense that you may
//    incur related to your use of MStar Software.
//    In no event shall MStar be liable for any direct, indirect, incidental or
//    consequential damages, including without limitation, lost of profit or
//    revenues, lost or damage of data, and unauthorized system use.
//    You agree that this Section 4 shall still apply without being affected
//    even if MStar Software has been modified by MStar in accordance with your
//    request or instruction for your use, except otherwise agreed by both
//    parties in writing.
//
// 5. If requested, MStar may from time to time provide technical supports or
//    services in relation with MStar Software to you for your use of
//    MStar Software in conjunction with your or your customer`s product
//    ("Services").
//    You understand and agree that, except otherwise agreed by both parties in
//    writing, Services are provided on an "AS IS" basis and the warranty
//    disclaimer set forth in Section 4 above shall apply.
//
// 6. Nothing contained herein shall be construed as by implication, estoppels
//    or otherwise:
//    (a) conferring any license or right to use MStar name, trademark, service
//        mark, symbol or any other identification;
//    (b) obligating MStar or any of its affiliates to furnish any person,
//        including without limitation, you and your customers, any assistance
//        of any kind whatsoever, or any information; or
//    (c) conferring any license or right under any intellectual property right.
//
// 7. These terms shall be governed by and construed in accordance with the laws
//    of Taiwan, R.O.C., excluding its conflict of law rules.
//    Any and all dispute arising out hereof or related hereto shall be finally
//    settled by arbitration referred to the Chinese Arbitration Association,
//    Taipei in accordance with the ROC Arbitration Law and the Arbitration
//    Rules of the Association by three (3) arbitrators appointed in accordance
//    with the said Rules.
//    The place of arbitration shall be in Taipei, Taiwan and the language shall
//    be English.
//    The arbitration award shall be final and binding to both parties.
//
//******************************************************************************
//<MStar Software>
////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2008-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (!¡±MStar Confidential Information!¡L) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////
//==============================================================================
#ifndef __DRV_SCL_HVSP_ST_H__
#define __DRV_SCL_HVSP_ST_H__

#define DRV_HVSP_CROP_1     0
#define DRV_HVSP_CROP_2     1
#define DRV_HVSP_CROP_NUM   2
#define SetPostCrop 1
#define HVSP_PRIMASK_SIZE 11*84
#define HVSP_PRIMASK_Width_SIZE 11
#define HVSP_PRIMASK_Height_SIZE 84
#define HVSP_PRIMASK_BLOCK_SIZE 32
#define BYTESIZE 8
//-------------------------------------------------------------------------------------------------
//  Defines & enum
//-------------------------------------------------------------------------------------------------
typedef enum
{
    E_DRV_SCLHVSP_POLL_ID_1 = 0,
    E_DRV_SCLHVSP_POLL_ID_2,
    E_DRV_SCLHVSP_POLL_ID_3,
    E_DRV_SCLHVSP_POLL_ID_VIP,
    E_DRV_SCLHVSP_POLL_ID_MAX,      //I1 has 3 HVSP
}DrvSclHvspPollIdType_e;

typedef enum
{
    E_DRV_SCLHVSP_CROP_ID_1,
    E_DRV_SCLHVSP_CROP_ID_2,
    E_DRV_SCLHVSP_CROP_ID_MAX,     // I1 has 2 crop IP
}DrvSclHvspCropIdType_e;

typedef enum
{
    E_DRV_SCLHVSP_IP_MUX_BT656 ,   //I1 HW only has BT656 and ISP input source
    E_DRV_SCLHVSP_IP_MUX_ISP ,
    E_DRV_SCLHVSP_IP_MUX_HVSP ,    //SW used in HVSP 2
    E_DRV_SCLHVSP_IP_MUX_PAT_TGEN, // SW used in debug pat
    E_DRV_SCLHVSP_IP_MUX_MAX,      // 4 main type, 2 hw type, 2 sw type
}DrvSclHvspIpMuxType_e;

typedef enum
{
    E_DRV_SCLHVSP_MONITOR_CROPCHECK  = 1,    ///< crop
    E_DRV_SCLHVSP_MONITOR_DMA1FRMCHECK = 2,    ///< dma1frm
    E_DRV_SCLHVSP_MONITOR_DMA1SNPCHECK = 3,    ///< dma1frm
    E_DRV_SCLHVSP_MONITOR_DMA2FRMCHECK = 4,    ///< dma1frm
    E_DRV_SCLHVSP_MONITOR_DMA3FRMCHECK = 5,    ///< dma1frm
}DrvSclHvspMonitorType_e;
typedef enum
{
    E_DRV_SCLHVSP_OSD_LOC_AFTER  = 0,    ///< after hvsp
    E_DRV_SCLHVSP_OSD_LOC_BEFORE = 1,    ///< before hvsp
}DrvSclHvspOsdLocType_e;


typedef enum
{
    E_DRV_SCLHVSP_ID_1,
    E_DRV_SCLHVSP_ID_2,
    E_DRV_SCLHVSP_ID_3,
    E_DRV_SCLHVSP_ID_MAX,      //I1 has 3 HVSP
}DrvSclHvspIdType_e;

typedef enum
{
    E_DRV_SCLHVSP_PRIMASK_DISABLE =0,
    E_DRV_SCLHVSP_PRIMASK_ENABLE,
    E_DRV_SCLHVSP_PRIMASK_PENDING,
    E_DRV_SCLHVSP_PRIMASK_ONLYHWOFF,
    E_DRV_SCLHVSP_PRIMASK_MAX,      //I3 has 3mode
}DrvSclHvspPriMaskType_e;

typedef enum
{
    E_DRV_SCLHVSP_CMD_TRIG_NONE,           //RIU
    E_DRV_SCLHVSP_CMD_TRIG_POLL_LDC_SYNC,  //polling blanking region RIU
    E_DRV_SCLHVSP_CMD_TRIG_CMDQ_FRMCNT,    //wait framecnt CMDQ
    E_DRV_SCLHVSP_CMD_TRIG_CMDQ_LDC_SYNC,  //used CMDQ in blanking region
    E_DRV_SCLHVSP_CMD_TRIG_MAX,            //HVSP has 4 type to set register,2 RIU,2 CMDQ
}DrvSclHvspCmdTrigType_e;
typedef enum
{
    E_DRV_SCLHVSP_EVENT_RUN = 0x00000001,
    E_DRV_SCLHVSP_EVENT_IRQ = 0x00000002,
} DrvSclHvspTxEvenType_e;
typedef enum
{
    E_DRV_SCLHVSP_FBMG_SET_LDCPATH_ON      = 0x1,
    E_DRV_SCLHVSP_FBMG_SET_LDCPATH_OFF     = 0x2,
    E_DRV_SCLHVSP_FBMG_SET_DNR_Read_ON     = 0x4,
    E_DRV_SCLHVSP_FBMG_SET_DNR_Read_OFF    = 0x8,
    E_DRV_SCLHVSP_FBMG_SET_DNR_Write_ON    = 0x10,
    E_DRV_SCLHVSP_FBMG_SET_DNR_Write_OFF   = 0x20,
    E_DRV_SCLHVSP_FBMG_SET_DNR_BUFFER_1    = 0x40,
    E_DRV_SCLHVSP_FBMG_SET_DNR_BUFFER_2    = 0x80,
    E_DRV_SCLHVSP_FBMG_SET_UNLOCK          = 0x100,
    E_DRV_SCLHVSP_FBMG_SET_DNR_COMDE_ON    = 0x200,
    E_DRV_SCLHVSP_FBMG_SET_DNR_COMDE_OFF   = 0x400,
    E_DRV_SCLHVSP_FBMG_SET_DNR_COMDE_265OFF   = 0x800,
    E_DRV_SCLHVSP_FBMG_SET_PRVCROP_ON      = 0x1000,
    E_DRV_SCLHVSP_FBMG_SET_PRVCROP_OFF     = 0x2000,
    E_DRV_SCLHVSP_FBMG_SET_CIIR_ON      = 0x4000,
    E_DRV_SCLHVSP_FBMG_SET_CIIR_OFF     = 0x8000,
    E_DRV_SCLHVSP_FBMG_SET_LOCK          = 0x10000,
}DrvSclHvspFbmgSetType_e;
typedef enum
{
    E_DRV_SCLHVSP_IQ_H_Tbl0,
    E_DRV_SCLHVSP_IQ_H_Tbl1,
    E_DRV_SCLHVSP_IQ_H_Tbl2,
    E_DRV_SCLHVSP_IQ_H_Tbl3,
    E_DRV_SCLHVSP_IQ_H_BYPASS,
    E_DRV_SCLHVSP_IQ_H_BILINEAR,
    E_DRV_SCLHVSP_IQ_V_Tbl0,
    E_DRV_SCLHVSP_IQ_V_Tbl1,
    E_DRV_SCLHVSP_IQ_V_Tbl2,
    E_DRV_SCLHVSP_IQ_V_Tbl3,
    E_DRV_SCLHVSP_IQ_V_BYPASS,
    E_DRV_SCLHVSP_IQ_V_BILINEAR,
    E_DRV_SCLHVSP_IQ_NUM,
}DrvSclHvspIqType_e;
typedef enum
{
    E_DRV_SCLHVSP_LDCLCBANKMODE_64,
    E_DRV_SCLHVSP_LDCLCBANKMODE_128,
}DrvSclHvspLdcLcBankModeType_e;
//-------------------------------------------------------------------------------------------------
//  Structure
//-------------------------------------------------------------------------------------------------

typedef struct
{
    u32 u32RIUBase;      // I1 :0x1F000000
    u32 u32IRQNUM;       // scl irq num by device tree
    u32 u32CMDQIRQNUM;   // cmdq irq
}DrvSclHvspInitConfig_t;

typedef struct
{
    u32 u32IRQNUM;
    u32 u32CMDQIRQNUM;   // cmdq irq
}DrvSclHvspSuspendResumeConfig_t;

typedef struct
{
    DrvSclOsClkStruct_t* idclk; // I1 scl clk 1.idclk(for before crop)
    DrvSclOsClkStruct_t* fclk1; // 2. fclk1(scl main clk)
    DrvSclOsClkStruct_t* fclk2; // 3.fclk2(scl hvsp3 only)
    DrvSclOsClkStruct_t* odclk; // 4.odclk(ttl only )
}DrvSclHvspClkConfig_t;
typedef struct
{
    bool bYCMRead;      // DNR IP can Read from Buffer (if can read then DNR availability)
    bool bYCMWrite;     // DNR IP can Write to Buffer
    bool bCIIRRead;      // DNR IP can Read from Buffer (if can read then DNR availability)
    bool bCIIRWrite;     // DNR IP can Write to Buffer
    u32  u32YCBaseAddr;// YC Buffer phycal address(unit:Byte)
    u32  u32MBaseAddr;// M Buffer phycal address(unit:Byte)
    u32  u32CIIRBaseAddr;// CIIR Buffer phycal address(unit:Byte)
    u16  u16Vsize;   // DNR Frame Height size
    u16  u16Fetch;   // DNR Frame Width size
    u32  u32MemSize; // DNR buffer size
}DrvSclHvspIpmConfig_t;
typedef struct
{
    u32 u32AffCount;
    u32 u32ISPInCount;
    u32 u32ISPDoneCount;
    u32 u32SC1FrmDoneCount;
    u32 u32SC1SnpDoneCount;
    u32 u32SC2FrmDoneCount;
    u32 u32SC2Frm2DoneCount;
    u32 u32SC3DoneCount;
    u32 u32SCLMainDoneCount;
    u32 u32SC1FrmActiveTime;
    u32 u32SC1SnpActiveTime;
    u32 u32SC2FrmActiveTime;
    u32 u32SC2Frm2ActiveTime;
    u32 u32SC3ActiveTime;
    u32 u32SCLMainActiveTime;
    u32 u32ISPTime;
    u32 u32ISPBlanking;
    u32 u32ISPBlankingTime;
    u32 u32ErrorCount;
    u8 u8CountReset;
}DrvSclHvspScIntsType_t;


typedef struct
{
    DrvSclHvspCmdTrigType_e enType;
    u8             u8Fmcnt;
}DrvSclHvspCmdTrigConfig_t;

typedef struct
{
    DrvSclHvspCmdTrigConfig_t stCmdTrigCfg;
    u16 u16Src_Width;                      // Input source width(post crop1)
    u16 u16Src_Height;                     // Input source height(post crop1)
    bool bCropEn[DRV_HVSP_CROP_NUM];       // crop enable
    u16 u16Crop_X[DRV_HVSP_CROP_NUM];      // crop x
    u16 u16Crop_Y[DRV_HVSP_CROP_NUM];      // crop y
    u16 u16Crop_Width[DRV_HVSP_CROP_NUM];  // post crop2 width
    u16 u16Crop_Height[DRV_HVSP_CROP_NUM]; // post crop2 height
    u16 u16Dsp_Width;                      // After scl display width
    u16 u16Dsp_Height;                     // After scl display height
    bool bRet;
}DrvSclHvspScalingConfig_t;

typedef struct
{
    bool bSet;
    u16 u16Width;
    u16 u16Height;
}ST_HVSP_SIZE_CONFIG;

typedef struct
{
    bool             bCropEn;

    ST_HVSP_SIZE_CONFIG stSizeAfterCrop;
    ST_HVSP_SIZE_CONFIG stSizeAfterScaling;
    u32              u32ScalingRatio_H;
    u32              u32ScalingRatio_V;
}DrvSclHvspScalingInfo_t;

typedef struct
{
    u16 u16Width;    // LDC FB width
    u16 u16Height;   // LDC FB Heigh
    u32 u32FBaddr;    // DNR Buffer
    u8 u8FBidx;      // idx assign by sw
    u8 u8FBrwdiff;   // delay frame from DNR buffer
    u8 bEnSWMode;    // enable idx assign sw mode
    u8 bEnDNR;       // DNR enable?
    DrvSclHvspLdcLcBankModeType_e enLDCType;
}DrvSclHvspLdcFrameBufferConfig_t;

typedef struct
{
    u16 u16Vtt;
    u16 u16VBackPorch;
    u16 u16VSyncWidth;
    u16 u16VFrontPorch;
    u16 u16VActive;
    u16 u16Htt;
    u16 u16HBackPorch;
    u16 u16HSyncWidth;
    u16 u16HFrontPorch;
    u16 u16HActive;
}DrvSclHvspPatTgenConfig_t;

typedef struct
{
    u16 u16X;            //isp crop x
    u16 u16Y;            // isp crop y
    u16 u16Width;        // display width
    u16 u16Height;       // display height
    u16 u16crop2inWidth; // after isp crop width
    u16 u16crop2inHeight;// after isp crop height
    u16 u16crop2OutWidth; // after isp crop width
    u16 u16crop2OutHeight;// after isp crop height
    u8  bEn;             //crop En
}DrvSclHvspScInformConfig_t;
typedef struct
{
    u16 u16Width;        // display width
    u16 u16Height;       // display height
    u16 u16inWidth; // after hvsp width
    u16 u16inHeight;// after hvsp height
    u8  bEn;             //function En
}DrvSclHvspInformConfig_t;
typedef struct
{
    DrvSclHvspIpMuxType_e enMux;        // display width
    u16 u16inWidth; // isp width
    u16 u16inHeight;// isp height
    u16 u16inCropWidth; // isp crop width
    u16 u16inCropHeight;// isp crop height
    u16 u16inCropX; // isp crop X
    u16 u16inCropY;// isp crop Y
    u16 u16inWidthcount; // isp width
    u16 u16inHeightcount;// isp height
    u8  bEn;             //function En
}DrvSclHvspInputInformConfig_t;
typedef struct
{
    DrvSclHvspFbmgSetType_e enSet;
}DrvSclHvspSetFbManageConfig_t;
typedef struct
{
    u8 bOSDEn;    ///< OSD en
    u8 bOSDBypass;    ///< OSD en
    u8 bWTMBypass;    ///< OSD en
}DrvSclHvspOsdOnOffConfig_t;

typedef struct
{
    DrvSclHvspOsdLocType_e enOSD_loc;    ///< OSD locate
    DrvSclHvspOsdOnOffConfig_t stOsdOnOff;
}DrvSclHvspOsdConfig_t;
typedef struct
{
    u8 bMask; ///<Mask enable
    u16 u16X;        ///< start x point
    u16 u16Y;        ///<  start y point
    u16 u16Width;    ///< width size
    u16 u16Height;   ///< height size
}DrvSclHvspPriMaskConfig_t;

//-------------------------------------------------------------------------------------------------
//  Prototype
//-------------------------------------------------------------------------------------------------

#endif
