///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2010 - 2012 MStar Semiconductor, Inc.
// This program is free software.
// You can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation;
// either version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with this program;
// if not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//
// @file   mdrv_xc_st.h
// @brief  XC KMD Driver Interface
// @author MStar Semiconductor Inc.
//////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef _MDRV_XC_ST_H
#define _MDRV_XC_ST_H

//=============================================================================
// Includs
//=============================================================================

//=============================================================================
// Type and Structure Declaration
//=============================================================================
#define KDRV_XC_HDMI_HDR_METADATA_VERSION 0
#define KDRV_XC_PANEL_FORMAT_VERSION 0
#define KDRV_XC_OSD_CONTROL_VERSION 0
#define KDRV_XC_MAIN_CONTROL_VERSION 0
#define KDRV_XC_HDMI_INFOFRAME_PARSER_VERSION 0
#define KDRV_XC_HDMI_EDID_PARSER_VERSION 0
#define KDRV_XC_MM_PARSER_VERSION 0

#define KDRV_XC_CFD_HDMI_VERSION 0
/// CFD Panel structure version
/// Version 0: Init structure
/// Version 1: Add Linear RGB and customer color primaries setting
/// Version 2: Add PWM port & dolby global dimming on/off & dolby global delay frame count.
#define KDRV_XC_CFD_PANEL_VERSION 2
#define KDRV_XC_CFD_HDR_VERSION 0
#define KDRV_XC_CFD_EDID_VERSION 0
#define KDRV_XC_CFD_FIRE_VERSION 0

typedef struct
{
    MS_BOOL bCurveChange;  // Curve Change
    MS_U8  u8LumaCurve[16];  // Default luma curve from Supernova DLC.ini
    MS_U8  u8LumaCurve2_a[16];  // Default luma curve 2a  from Supernova DLC.ini
    MS_U8  u8LumaCurve2_b[16];  // Default luma curve 2b  from Supernova DLC.ini
    MS_U8  u8Result; //Out
} ST_KDRV_XC_DLC_INFO;

typedef struct
{
    MS_BOOL bBLEPointChange;  // BLE Point Change
    MS_U16  u16BLESlopPoint[6];  // Default BLE Slop Point from Supernova DLC.ini
    MS_U8  u8Result; //Out
} ST_KDRV_XC_DLC_BLE_INFO;

typedef struct
{
    MS_BOOL bInitInfoChange;  // Init Info Change

    // Default luma curve
    MS_U8 au8LumaCurve[16];
    MS_U8 au8LumaCurve2_a[16];
    MS_U8 au8LumaCurve2_b[16];
    MS_U8 au8DlcHistogramLimitCurve[17];

    MS_U8 u8DlcPureImageMode; // Compare difference of max and min bright
    MS_U8 u8DlcLevelLimit; // n = 0 ~ 4 => Limit n levels => ex. n=2, limit 2 level 0xF7, 0xE7
    MS_U8 u8DlcAvgDelta; // n = 0 ~ 50, default value: 12
    MS_U8 u8DlcAvgDeltaStill; // n = 0 ~ 15 => 0: disable still curve, 1 ~ 15 => 0.1 ~ 1.5 enable still curve
    MS_U8 u8DlcFastAlphaBlending; // min 17 ~ max 32
    MS_U8 u8DlcYAvgThresholdL; // default value: 0
    MS_U8 u8DlcYAvgThresholdH; // default value: 128

    MS_U8 u8DlcBLEPoint; // n = 24 ~ 64, default value: 48
    MS_U8 u8DlcWLEPoint; // n = 24 ~ 64, default value: 48
    MS_U8 bEnableBLE : 1; // 1: enable; 0: disable
    MS_U8 bEnableWLE : 1; // 1: enable; 0: disable
    MS_U8 u8DlcYAvgThresholdM;
    MS_U8 u8DlcCurveMode;
    MS_U8 u8DlcCurveModeMixAlpha;
    MS_U8 u8DlcAlgorithmMode;
    MS_U8 u8DlcSepPointH;
    MS_U8 u8DlcSepPointL;
    MS_U16 u16DlcBleStartPointTH;
    MS_U16 u16DlcBleEndPointTH;
    MS_U8 u8DlcCurveDiff_L_TH;
    MS_U8 u8DlcCurveDiff_H_TH;
    MS_U16 u16DlcBLESlopPoint_1;
    MS_U16 u16DlcBLESlopPoint_2;
    MS_U16 u16DlcBLESlopPoint_3;
    MS_U16 u16DlcBLESlopPoint_4;
    MS_U16 u16DlcBLESlopPoint_5;
    MS_U16 u16DlcDark_BLE_Slop_Min;
    MS_U8 u8DlcCurveDiffCoringTH;
    MS_U8 u8DlcAlphaBlendingMin;
    MS_U8 u8DlcAlphaBlendingMax;
    MS_U8 u8DlcFlicker_alpha;
    MS_U8 u8DlcYAVG_L_TH;
    MS_U8 u8DlcYAVG_H_TH;
    MS_U8 u8DlcDiffBase_L;
    MS_U8 u8DlcDiffBase_M;
    MS_U8 u8DlcDiffBase_H;
    // NEW DLC
    MS_U8 u8LMaxThreshold;
    MS_U8 u8LMinThreshold;
    MS_U8 u8LMaxCorrection;
    MS_U8 u8LMinCorrection;
    MS_U8 u8RMaxThreshold;
    MS_U8 u8RMinThreshold;
    MS_U8 u8RMaxCorrection;
    MS_U8 u8RMinCorrection;
    MS_U8 u8AllowLoseContrast;
} ST_KDRV_XC_DLC_INIT_INFO;

typedef struct
{
    MS_U32 u32OutputNits[512];
    MS_U16 u16InputPoint[512];
    MS_U16 u16ControlPoints;
    MS_S16 s16LastLess1IndexOutputnits;
    MS_S16 s16LastIndexM10000Outputs;
    MS_S16 s16LastIndexM100Outputs;
}ST_KDRV_XC_DLC_TMO_1DLUT;


// HDR new tone mapping parameters.
typedef struct
{
    // TMO
    MS_U16 u16SrcMinRatio;          //default 10
    MS_U16 u16SrcMedRatio;          //default 512
    MS_U16 u16SrcMaxRatio;          //default 990

    MS_U16 u16TgtMin;               //default 500
    MS_U16 u16TgtMed;
    MS_U16 u16TgtMax;               //default 300

    MS_U16 u16FrontSlopeMin;        //default 256
    MS_U16 u16FrontSlopeMax;        //default 512
    MS_U16 u16BackSlopeMin;         //default 128
    MS_U16 u16BackSlopeMax;         //default 256

    MS_U16 u16SceneChangeThrd;      //default 1024
    MS_U16 u16SceneChangeRatioMax;  //default 1024

    MS_U8 u8IIRRatio;            //default 31
    MS_U8 u8TMOTargetMode;      // default 0. 0 : keeps the value in initial function  1 : from output source
    MS_U16 u16SDRPanelGain;

    MS_U16 u16Smin;
    MS_U16 u16Smed;
    MS_U16 u16Smax;
    MS_U16 u16Tmin;
    MS_U16 u16Tmed;
    MS_U16 u16Tmax;

    MS_BOOL bRefMode;

    MS_U8 u8TMOUseIniControls;
    ST_KDRV_XC_DLC_TMO_1DLUT stTMO1DLUT;
} ST_KDRV_XC_TMO_INFO;

typedef enum
{
    EN_KDRV_XC_HDR_DMA_BYPASS,
    EN_KDRV_XC_HDR_DMA_ENABLE,
    EN_KDRV_XC_HDR_DMA_MAX,
} EN_KDRV_XC_HDR_DMA_PATH;

//IO Ctrl struct defines:
typedef enum
{
    E_KDRV_XC_HDR_NONE,
    E_KDRV_XC_HDR_DOLBY,
    E_KDRV_XC_HDR_OPEN,
    E_KDRV_XC_HDR_TECHNICOLOR,
    E_KDRV_XC_HDR_MAX,
} EN_KDRV_XC_HDR_TYPE;

typedef enum
{
    E_KDRV_XC_INPUT_SOURCE_NONE,
    E_KDRV_XC_INPUT_SOURCE_HDMI,
    E_KDRV_XC_INPUT_SOURCE_OTT,
    E_KDRV_XC_INPUT_SOURCE_MAX,
} EN_KDRV_XC_INPUT_SOURCE_TYPE;

typedef enum
{
    E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR,
    E_KDRV_XC_AUTODOWNLOAD_CLIENT_OP2GAMMA,
    E_KDRV_XC_AUTODOWNLOAD_CLIENT_FRCOP2GAMMA,
    E_KDRV_XC_AUTODOWNLOAD_CLIENT_XVYCC,
    E_KDRV_XC_AUTODOWNLOAD_CLIENT_ODTABLE1,
    E_KDRV_XC_AUTODOWNLOAD_CLIENT_ODTABLE2,
    E_KDRV_XC_AUTODOWNLOAD_CLIENT_ODTABLE3,
    E_KDRV_XC_AUTODOWNLOAD_CLIENT_DEMURA,
    E_KDRV_XC_AUTODOWNLOAD_CLIENT_OP2LUT,
    E_KDRV_XC_AUTODOWNLOAD_CLIENT_T3D_0,        /// t3d of sc0
    E_KDRV_XC_AUTODOWNLOAD_CLIENT_T3D_1,        /// t3d of sc1
    E_KDRV_XC_AUTODOWNLOAD_CLIENT_FRCSPTPOPM,
    E_KDRV_XC_AUTODOWNLOAD_CLIENT_FOOPM,
    E_KDRV_XC_AUTODOWNLOAD_CLIENT_MAX,
} EN_KDRV_XC_AUTODOWNLOAD_CLIENT;

typedef enum
{
    E_KDRV_XC_AUTODOWNLOAD_TRIGGER_MODE,
    E_KDRV_XC_AUTODOWNLOAD_ENABLE_MODE,
} EN_KDRV_XC_AUTODOWNLOAD_MODE;

typedef enum
{
    E_KDRV_XC_AUTODOWNLOAD_HDR_SUB_TMO,
    E_KDRV_XC_AUTODOWNLOAD_HDR_SUB_3DLUT,
    E_KDRV_XC_AUTODOWNLOAD_HDR_SUB_GAMMA,
    E_KDRV_XC_AUTODOWNLOAD_HDR_SUB_DEGAMMA,
} EN_KDRV_XC_AUTODOWNLOAD_SUB_CLIENT;

typedef enum
{
    E_KDRV_XC_AUTODOWNLOAD_XVYCC_SUB_GAMMA,
    E_KDRV_XC_AUTODOWNLOAD_XVYCC_SUB_DEGAMMA
} EN_KDRV_XC_AUTODOWNLOAD_XVYCC_SUB_CLIENT;

typedef enum
{
    E_KDRV_XC_HDR_COLOR_NONE,
    E_KDRV_XC_HDR_COLOR_YUV420,
    E_KDRV_XC_HDR_COLOR_YUV422,
    E_KDRV_XC_HDR_COLOR_YUV444,
    E_KDRV_XC_HDR_COLOR_RGB,
    E_KDRV_XC_HDR_COLOR_MAX,
} EN_KDRV_XC_HDR_COLOR_FORMAT;

typedef enum
{
    E_KDRV_MLOAD_UNSUPPORTED = 0,
    E_KDRV_MLOAD_DISABLED    = 1,
    E_KDRV_MLOAD_ENABLED     = 2,
} EN_KDRV_MLOAD_TYPE;

typedef enum
{
    E_CLIENT_MAIN_XC = 0,
    E_CLIENT_MAIN_HDR = 1,
    E_CLIENT_SUB_XC = 2,
    E_CLIENT_SUB_HDR = 3,
    E_CLIENT_MAX,
} EN_MLOAD_CLIENT_TYPE;

typedef enum
{
    E_KDRV_MAIN_WINDOW = 0,
    E_KDRV_SUB_WINDOW,
    E_KDRV_MAX_WINDOW,
}EN_KDRV_WINDOW;

typedef enum
{
    E_KDRV_XC = 0,
    E_KDRV_HDR,
}EN_KDRV_DS_CLIENT_TYPE;

typedef enum
{
    E_KDRV_XC_SC0 = 0,
    E_KDRV_XC_SC1,
    E_KDRV_XC_MAX,
}EN_KDRV_SC_DEVICE;

typedef enum
{
    E_K_APIXC_RET_FAIL = 0,                   ///<Fail
    E_K_APIXC_RET_OK,                         ///<OK
    E_K_APIXC_RET_FAIL_FUN_NOT_SUPPORT,       ///<Fail
    E_K_APIXC_RET_FAIL_ENUM_NOT_SUPPORT,      ///<Fail
    E_K_APIXC_RET_FAIL_INVALID_PARAMETER,     ///<Fail
} E_K_APIXC_ReturnValue;

typedef enum
{
    E_K_XC_DS_INDEX_MVOP,
    E_K_XC_DS_INDEX_SCALER,
    E_K_XC_DS_INDEX_MAX,
}E_K_XC_DS_INDEX_SOURCE;

typedef struct
{
    MS_U16 u16CMDCNT_IPM;
    MS_U16 u16CMDCNT_IPS;
    MS_U16 u16CMDCNT_OPM;
    MS_U16 u16CMDCNT_OPS;
} K_XC_DS_CMDCNT;

typedef enum
{
    E_MLOAD_TRIGGER_BY_OP_SYNC=0,
    E_MLOAD_TRIGGER_BY_IP_MAIN_SYNC=1,
    E_MLOAD_TRIGGER_BY_IP_SUB_SYNC=2,
    E_MLOAD_TRIGGER_BY_SW=3,
    E_MLOAD_TRIGGER_MAX,
} EN_MLOAD_TRIG_SYNC;

typedef enum
{
    // New
    /// Initialize
    E_KDRV_XC_CFD_CTRL_SET_INIT = 0,
    /// VGA
    E_KDRV_XC_CFD_CTRL_SET_VGA = 1,
    /// TV (ATV)
    E_KDRV_XC_CFD_CTRL_SET_TV = 2,
    /// CVBS (AV)
    E_KDRV_XC_CFD_CTRL_SET_CVBS = 3,
    /// S-Video
    E_KDRV_XC_CFD_CTRL_SET_SVIDEO = 4,
    /// YPbPr
    E_KDRV_XC_CFD_CTRL_SET_YPBPR = 5,
    /// Scart
    E_KDRV_XC_CFD_CTRL_SET_SCART = 6,
    /// HDMI
    E_KDRV_XC_CFD_CTRL_SET_HDMI = 7,
    /// DTV
    E_KDRV_XC_CFD_CTRL_SET_DTV = 8,
    /// DVI
    E_KDRV_XC_CFD_CTRL_SET_DVI = 9,
    /// MM
    E_KDRV_XC_CFD_CTRL_SET_MM = 10,
    /// Panel
    E_KDRV_XC_CFD_CTRL_SET_PANEL = 11,
    /// HDR
    E_KDRV_XC_CFD_CTRL_SET_HDR = 12,
    /// EDID
    E_KDRV_XC_CFD_CTRL_SET_EDID = 13,
    /// OSD
    E_KDRV_XC_CFD_CTRL_SET_OSD = 14,
    /// Fire
    E_KDRV_XC_CFD_CTRL_SET_FIRE = 15,
    /// DLC
    E_KDRV_XC_CFD_CTRL_SET_DLC = 16,
    /// Linear RGB
    E_KDRV_XC_CFD_CTRL_SET_LINEAR_RGB = 17,
    /// Get CFD Output to HDMI Status and dicison.
    E_KDRV_XC_CFD_CTRL_GET_HDMI_STATUS = 18,
    /// Set force HDR Onoff or auto select setting
    E_KDRV_XC_CFD_CTRL_SET_HDR_ONOFF_SETTING = 19,
    /// Status
    E_KDRV_XC_CFD_CTRL_GET_STATUS = 20,
    /// TMO
    E_KDRV_XC_CFD_CTRL_SET_TMO = 21,
    /// Update Status
    E_KDRV_XC_CFD_CTRL_UPDATE_STATUS = 22,
    /// CFD CTRL_TYPE_EX start
    /// (Set this base is 0x40000000 for int type)
    E_KDRV_XC_CFD_CTRL_TYPE_EX_BASE = 0x40000000,
    /// OSD_STATUS
    E_KDRV_XC_CFD_CTRL_GET_OSD_PROCESS_CONFIGS= 0x40000001,
    E_KDRV_XC_CFD_CTRL_MAX,
} EN_KDRV_XC_CFD_CTRL_TYPE;

typedef struct __attribute__((packed))
{
    MS_U64 u64DS_Info_BaseAddr;         ///< The memory base address to update IP/OP dynamic scaling registers. Absolute without any alignment.
    MS_U8  u8MIU_Select;                ///< 0: MIU0, 1: MIU1, etc.
    MS_U8  u8DS_Index_Depth;            ///< The index depth (how many entries to fire per MVOP Vsync falling) of DS
    MS_BOOL bOP_DS_On;                  ///< OP dynamic scaling on/off
    MS_BOOL bIPS_DS_On;                 ///< IPS dynamic scaling on/off
    MS_BOOL bIPM_DS_On;                 ///< IPM dynamic scaling on/off
    MS_U32  u32DSBufferSize;            ///< Ds buffer size
    MS_BOOL bEnable_ForceP;             ///< bEnable_ForceP on/off
} K_XC_DynamicScaling_Info;

typedef enum
{
    K_DS_IP,
    K_DS_OP,
} k_ds_reg_ip_op_sel;

typedef enum
{
    K_DS_XC,
    K_DS_MVOP,
    K_DS_GOP,
} k_ds_reg_source_sel;

//*****************DS NEW*****************//
typedef enum
{
    E_DS_CLIENT_XC,
    E_DS_CLIENT_HDR,
    E_DS_CLIENT_PQ,
} E_DS_CLIENT;

typedef struct
{
    E_DS_CLIENT client;
    MS_U32  max_num;
} K_DS_register_info;
//****************************************//


typedef struct __attribute__((packed))
{
    MS_U8* pu8Data;                     /// pointer to data
#if !defined (__aarch64__)
    void    *pDummy;
#endif
    MS_U32 u32Size;                     /// data size
} ST_KDRV_XC_OPEN_METADATA_INFO;

typedef struct __attribute__((packed))
{
    MS_U8* pu8Data;                     /// pointer to data
#if !defined (__aarch64__)
    void    *pDummy;
#endif
    MS_U32 u32Size;                     /// data size
} ST_KDRV_XC_3DLUT_INFO;

typedef struct
{
    MS_U64  phyBaseAddr;                      /// baseaddr
    MS_U32    u32Size;
    MS_U32    u32MiuNo;
} ST_KDRV_XC_SHARE_MEMORY_INFO;

typedef struct
{
    MS_U16 u16X;           ///<start x of the window
    MS_U16 u16Y;           ///<start y of the window
    MS_U16 u16Width;       ///<width of the window
    MS_U16 u16Height;      ///<height of the window
} ST_KDRV_XC_WINDOW_INFO;

typedef struct
{
    EN_KDRV_XC_AUTODOWNLOAD_CLIENT enClient; /// current client
    MS_U64 phyBaseAddr;                      /// baseaddr
    MS_U32 u32Size;                          /// size
    MS_U32 u32MiuNo;                         /// miu no
    MS_BOOL bEnable;                         /// enable/disable the client
    EN_KDRV_XC_AUTODOWNLOAD_MODE enMode;     /// work mode
} ST_KDRV_XC_AUTODOWNLOAD_CONFIG_INFO;

typedef struct
{
    EN_KDRV_XC_AUTODOWNLOAD_SUB_CLIENT enSubClient; /// sub client
    MS_BOOL bEnableRange;                           /// whether enable range, 0: write data to (0-511), 1: write data to (u32StartAddr-u16EndAddr)
    MS_U16 u16StartAddr;                            /// start addr, range: 0-511, startaddr <= endaddr
    MS_U16 u16EndAddr;                              /// end addr: range: 0-511, startaddr <= endaddr
} ST_KDRV_XC_AUTODOWNLOAD_FORMAT_INFO;

typedef struct
{
    EN_KDRV_XC_AUTODOWNLOAD_XVYCC_SUB_CLIENT enSubClient; /// sub client
} ST_KDRV_XC_AUTODOWNLOAD_XVYCC_FORMAT_INFO;

typedef struct __attribute__((packed))
{
    EN_KDRV_XC_AUTODOWNLOAD_CLIENT enClient;  /// current client
    MS_U8* pu8Data;                           /// pointer to data
#if !defined (__aarch64__)
    void    *pDummy;
#endif
    MS_U32 u32Size;                         /// data size
    void* pParam;                           /// pointer to ST_KDRV_XC_AUTODOWNLOAD_FORMAT_INFO
#if !defined (__aarch64__)
    void    *pDummy2;
#endif
} ST_KDRV_XC_AUTODOWNLOAD_DATA_INFO;

typedef struct
{
    EN_KDRV_XC_AUTODOWNLOAD_CLIENT enClient;  /// current client
    MS_BOOL bSupported;                       /// whether it is supported
} ST_KDRV_XC_AUTODOWNLOAD_CLIENT_SUPPORTED_CAPS;

typedef struct
{
    EN_MLOAD_CLIENT_TYPE _client_type;
    MS_U64 PhyAddr;
    MS_U32 u32BufByteLen;
} ST_KDRV_MLOAD_INIT;

typedef struct
{
    EN_MLOAD_CLIENT_TYPE _client_type;
    MS_BOOL bEnable;
} ST_KDRV_MLOAD_ENABLE;

typedef struct
{
    EN_MLOAD_CLIENT_TYPE _client_type;
    EN_KDRV_MLOAD_TYPE eReturn;
} ST_KDRV_MLOAD_GETSTATUS;

typedef struct
{
    EN_MLOAD_CLIENT_TYPE _client_type;
    MS_BOOL bImmeidate;
    MS_BOOL bReturn;
} ST_KDRV_MLOAD_FIRE;

typedef struct
{
    EN_MLOAD_CLIENT_TYPE _client_type;
    MS_U32 u32Addr;
    MS_U16 u16Data;
    MS_U16 u16Mask;
    MS_BOOL bReturn;
} ST_KDRV_MLOAD_WRT_CMD;

typedef struct
{
    EN_MLOAD_CLIENT_TYPE _client_type;
    MS_U32 u32Bank;
    MS_U32 u32Addr;
    MS_U16 u16Data;
    MS_U16 u16Mask;
    MS_BOOL bReturn;
} ST_KDRV_MLOAD_WRT_CMD_NONXC;

typedef struct
{
    EN_MLOAD_CLIENT_TYPE _client_type;
    MS_U16 u16train;
    MS_U16 u16disp;
    MS_BOOL bReturn;
} ST_KDRV_MLOAD_SET_IP_TRIG_P;

typedef struct
{
    EN_MLOAD_CLIENT_TYPE _client_type;
    MS_U16 *pu16Train;
    MS_U16 *pu16Disp;
    MS_BOOL bReturn;
} ST_KDRV_MLOAD_GET_IP_TRIG_P;

typedef struct
{
    EN_MLOAD_CLIENT_TYPE _client_type;
    EN_MLOAD_TRIG_SYNC eTriggerSync;
} ST_KDRV_MLOAD_SET_TRIG_SYNC;

typedef struct  __attribute__((packed))
{
    EN_KDRV_XC_CFD_CTRL_TYPE enCtrlType;
    void *pParam;
#if !defined (__aarch64__)
    void *pDummy;
#endif
    MS_U32 u32ParamLen; //param length
    MS_U16 u16ErrCode;
} ST_KDRV_XC_CFD_CONTROL_INFO;

typedef struct
{
    EN_KDRV_SC_DEVICE u32DeviceID;
    EN_KDRV_WINDOW  eWindow;
    E_K_APIXC_ReturnValue eReturn;
} ST_KDRV_DS_GET_DSForceIndexSupported;

typedef struct
{
    EN_KDRV_SC_DEVICE u32DeviceID;
    E_K_XC_DS_INDEX_SOURCE eDSIdxSrc;
    EN_KDRV_WINDOW  eWindow;
    E_K_APIXC_ReturnValue eReturn;
} ST_KDRV_DS_SET_DSIndexSourceSelect;

typedef struct
{
    EN_KDRV_SC_DEVICE u32DeviceID;
    MS_BOOL bEnable;
    MS_U8 u8Index;
    EN_KDRV_WINDOW  eWindow;
    E_K_APIXC_ReturnValue eReturn;
} ST_KDRV_DS_SET_DSForceIndex;

typedef struct __attribute__((packed))
{
    EN_KDRV_SC_DEVICE u32DeviceID;
    K_XC_DynamicScaling_Info *pstDSInfo;
#if !defined (__aarch64__)
    MS_U32    pDummy;
#endif
    MS_U32 u32DSInfoLen;
    EN_KDRV_WINDOW  eWindow;
    MS_BOOL bReturn;
} ST_KDRV_DS_SetDynamicScaling;

typedef struct
{
    EN_KDRV_SC_DEVICE u32DeviceID;
    MS_BOOL bEnable;
} ST_KDRV_DS_SET_DynamicScalingFlag;

typedef struct
{
    EN_KDRV_SC_DEVICE u32DeviceID;
    MS_BOOL bReturn;
} ST_KDRV_DS_GetDynamicScalingStatus;

typedef struct
{
    EN_KDRV_SC_DEVICE u32DeviceID;
    MS_BOOL bEnable;
    MS_BOOL bReturn;
} ST_KDRV_DS_EnableIPMTuneAfterDS;

typedef struct __attribute__((packed))
{
    EN_KDRV_SC_DEVICE u32DeviceID;
    EN_KDRV_WINDOW eWindow;
    E_DS_CLIENT client;
    MS_U32 u32CmdRegAddr;
    MS_U16 u16CmdRegValue;
    k_ds_reg_ip_op_sel IPOP_Sel;
    k_ds_reg_source_sel Source_Select;
    K_XC_DS_CMDCNT *pstXC_DS_CmdCnt;
#if !defined (__aarch64__)
    MS_U32    pDummy;
#endif
} ST_KDRV_DS_WriteSWDSCommand;

typedef struct __attribute__((packed))
{
    EN_KDRV_SC_DEVICE u32DeviceID;
    EN_KDRV_WINDOW eWindow;
    E_DS_CLIENT client;
    MS_U32 u32Bank;
    MS_U32 u32Addr;
    MS_U16 u16Data;
    MS_U16 u16Mask;
    k_ds_reg_ip_op_sel IPOP_Sel;
    K_XC_DS_CMDCNT *pstXC_DS_CmdCnt;
#if !defined (__aarch64__)
    MS_U32    pDummy;
#endif
} ST_KDRV_DS_WriteSWDSCommandNonXC;

typedef struct __attribute__((packed))
{
    EN_KDRV_SC_DEVICE u32DeviceID;
    EN_KDRV_WINDOW eWindow;
    E_DS_CLIENT client;
    MS_U32 u32CmdRegAddr;
    MS_U16 u16CmdRegValue;
    MS_U16 u16Mask;
    k_ds_reg_ip_op_sel IPOP_Sel;
    k_ds_reg_source_sel Source_Select;
    K_XC_DS_CMDCNT *pstXC_DS_CmdCnt;
#if !defined (__aarch64__)
    MS_U32    pDummy;
#endif
} ST_KDRV_DS_WriteSWDSCommand_Mask;

typedef struct __attribute__((packed))
{
    EN_KDRV_SC_DEVICE u32DeviceID;
    EN_KDRV_WINDOW eWindow;
    E_DS_CLIENT client;
    k_ds_reg_ip_op_sel IPOP_Sel;
    K_XC_DS_CMDCNT *pstXC_DS_CmdCnt;
#if !defined (__aarch64__)
    MS_U32    pDummy;
#endif
} ST_KDRV_DS_Add_NullCommand;

typedef struct
{
    EN_KDRV_SC_DEVICE u32DeviceID;
    E_DS_CLIENT client;
    MS_U32 max_num;
    MS_BOOL bReturn;
} ST_KDRV_DS_SET_CLIENT;

typedef struct
{
    MS_U8 u8SupportIndexNum;
} ST_KDRV_DS_GET_SUPPORT_INDEX_NUM;

typedef struct
{
    EN_KDRV_SC_DEVICE u32DeviceID;
    EN_KDRV_WINDOW eWindow;
    MS_U8 u8DSIndex;
} ST_KDRV_DS_GET_INDEX_INFO;

typedef struct
{
    EN_KDRV_SC_DEVICE u32DeviceID;
    EN_KDRV_WINDOW eWindow;
} ST_KDRV_DS_FIRE_INDEX_INFO;

typedef struct
{
    MS_U8     u8CurrentIndex;
    MS_U64  phyRegAddr;      // HDR register
    MS_U32    u32RegSize;
    MS_U64  phyLutAddr;      // look-up table
    MS_U32    u32LutSize;
    MS_BOOL   bDMEnable;
    MS_BOOL   bCompEnable;
} ST_KDRV_XC_DS_HDRInfo;

/// CFD update status
typedef enum
{
    E_KDRV_XC_CFD_STATUS_SYNC_GEN_BLACK_SCREEN_DISABLE,
    E_KDRV_XC_CFD_STATUS_SYNC_GEN_BLACK_SCREEN_ENABLE,
    ///MAX
    E_KDRV_XC_CFD_STATUS_MAX
} EN_KDRV_XC_CFD_UPDATE_STATUS;
typedef struct
{
    EN_KDRV_XC_CFD_UPDATE_STATUS enCFD_status;
} ST_KDRV_XC_CFD_UPDATE_STATUS;
#endif //_MDRV_XC_ST_H
