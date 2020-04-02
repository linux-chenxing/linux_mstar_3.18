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
//////////////////////////////////////////////////////////////////////////////////////////////////
//
// @file   mdrv_sca_io.c
// @brief  SCA KMD Driver Interface
// @author MStar Semiconductor Inc.
//////////////////////////////////////////////////////////////////////////////////////////////////

//=============================================================================
// Include Files
//=============================================================================
#define MDRV_SCA_INPUTSOURCE_C

#include <linux/module.h>
#include <linux/kernel.h>



#include "ms_platform.h"

#include "MsTypes.h"
#include "MsDevice.h"
#include "MsCommon.h"
#include "drvXC_IOPort.h"
#include "drvPQ.h"
#include "drvPQ_Define.h"
#include "apiXC.h"
#include "apiXC_PCMonitor.h"
#include "apiXC_ModeParse.h"
#include "apiXC_Adc.h"
#include "apiXC_Auto.h"
#include "apiPNL.h"
#include "apiXC_Ace.h"
#include "drvTVEncoder.h"
#include "drvMVOP.h"
#include "drvMMIO.h"
#include "apiXC_Dlc.h"

#include "mdrv_sca_st.h"
#include "mdrv_sca.h"
#include "mdrv_sca_modeparse.h"
#include "mdrv_sca_inputsource.h"
#include "mdrv_sca_color.h"
#include "msapi_vd.h"
//drver header files

//=============================================================================
// Local Defines
//=============================================================================

//-------------------------------------------------------------------------------------------------
// Macros
//-------------------------------------------------------------------------------------------------
#define INPUT_DBG_MSG(x)  //

//------------------------------------------------------------------------------
// Over Scan
//------------------------------------------------------------------------------
#define OVERSCAN_DEFAULT_H              30 // 1.0%
#define OVERSCAN_DEFAULT_V              35 // 1.0%

//------------------------------------------------------------------------------
//  variable
//------------------------------------------------------------------------------

static MS_U8 _u8H_OverScanRatio[MAX_WINDOW];
static MS_U8 _u8V_OverScanRatio[MAX_WINDOW];
static MS_U8 _u8LoadBin = 0;
static MS_INPUT_TIMING_CONFIG gstCameraTimingCfg = {0};

MS_USER_DISPLAY_CONFIG g_stUserDisplayCfg[MAX_WINDOW][MS_USER_DISPLAY_NUM] =
{
    {
    {0, 0, 0, 0, 0}, // YPbPr
    {0, 0, 0, 0, 0}, // NTSC
    {0, 0, 0, 0, 0}, // PAL
    {0, 0, 0, 0, 0}, // VGA
    },

    {
    {0, 0, 0, 0, 0}, // YPbPr
    {0, 0, 0, 0, 0}, // NTSC
    {0, 0, 0, 0, 0}, // PAL
    {0, 0, 0, 0, 0}, // VGA
    },

    {
    {0, 0, 0, 0, 0}, // YPbPr
    {0, 0, 0, 0, 0}, // NTSC
    {0, 0, 0, 0, 0}, // PAL
    {0, 0, 0, 0, 0}, // VGA
    },

    {
    {0, 0, 0, 0, 0}, // YPbPr
    {0, 0, 0, 0, 0}, // NTSC
    {0, 0, 0, 0, 0}, // PAL
    {0, 0, 0, 0, 0}, // VGA
    },

    {
    {0, 0, 0, 0, 0}, // YPbPr
    {0, 0, 0, 0, 0}, // NTSC
    {0, 0, 0, 0, 0}, // PAL
    {0, 0, 0, 0, 0}, // VGA
    },

    {
    {0, 0, 0, 0, 0}, // YPbPr
    {0, 0, 0, 0, 0}, // NTSC
    {0, 0, 0, 0, 0}, // PAL
    {0, 0, 0, 0, 0}, // VGA
    },
};



XC_FREERUN_COLOR g_displaymute_color[MAX_WINDOW] =
{
    E_XC_FREE_RUN_COLOR_BLACK,
    E_XC_FREE_RUN_COLOR_BLACK,
    E_XC_FREE_RUN_COLOR_BLACK,
    E_XC_FREE_RUN_COLOR_BLACK,
    E_XC_FREE_RUN_COLOR_BLACK,
    E_XC_FREE_RUN_COLOR_BLACK,
};

MS_U8 MApi_Get_SC1_LoadPQ(void)
{
    return _u8LoadBin;
}

void MApi_Set_SC1_LoadPQ(MS_U8 u8value)
{
    _u8LoadBin = u8value;
}

//static E_VE_MUTE_STATUS s_eVEMuteStatus[2] = {E_VE_MUTE_INVALID, E_VE_MUTE_INVALID};
//static MS_VE_INFO  stVEInfo[2] = {{INPUT_SOURCE_NONE},  {INPUT_SOURCE_NONE}};

MS_VIDEO_Window_Info CVBS_WinInfo[SIG_NUMS][1] =
{
    {  //NTSC, 0
        {MSVD_HSTART_NTSC, MSVD_VSTART_NTSC,   40, 40, 28, 28},  // Default
    },
    {  //PAL, 1
        {MSVD_HSTART_PAL, MSVD_VSTART_PAL,   42, 42, 18, 18},  // Default
    },
    {  //SECAM, 2
        {MSVD_HSTART_SECAM, MSVD_VSTART_SECAM,   25, 25, 13, 13},  // Default
    },
    {  //NTSC-443/PAL-60, 3
        {MSVD_HSTART_NTSC_443, MSVD_VSTART_NTSC_443,   25, 25, 11, 11},  // Default
    },
    {  //PAL-M, 4
        {MSVD_HSTART_PAL_M, MSVD_VSTART_PAL_M,   22, 22, 11, 11},  // Default
    },
    {  //PAL-NC, 5
        {MSVD_HSTART_PAL_NC, MSVD_VSTART_PAL_NC,   25, 25, 14, 14},  // Default
    },
};

//=============================================================================
// Local Function Prototypes
//=============================================================================

void MApi_InputSource_InputPort_Mapping(INPUT_SOURCE_TYPE_t src_ids , E_MUX_INPUTPORT* port_ids , MS_U8* u8port_count )
{
	// prog input soruce
    switch (src_ids)
    {
        default:
            *u8port_count = 0;
            return;

		case INPUT_SOURCE_VGA:
		//case INPUT_SOURCE_YPBPR:
			*u8port_count = 2;
            port_ids[0] = INPUT_VGA_MUX;
            port_ids[1] = INPUT_VGA_SYNC_MUX;
            break;
        case INPUT_SOURCE_YPBPR:
            *u8port_count = 1;
            port_ids[0] = INPUT_YPBPR_MUX;
            break;
        case INPUT_SOURCE_DTV:
            *u8port_count = 1;
            port_ids[0] = INPUT_PORT_MVOP;
            break;

        case INPUT_SOURCE_STORAGE:
            *u8port_count = 1;
            port_ids[0] = INPUT_PORT_MVOP;
            break;

        case INPUT_SOURCE_CVBS:
            *u8port_count = 1;
            port_ids[0] = INPUT_AV_YMUX;
            break;

        case INPUT_SOURCE_CVBS2:
            *u8port_count = 1;
            port_ids[0] = INPUT_AV2_YMUX;
            break;

        case INPUT_SOURCE_CVBS3:
            *u8port_count = 1;
            port_ids[0] = INPUT_AV3_YMUX;
            break;

        case INPUT_SOURCE_SVIDEO:
            *u8port_count = 2;
            port_ids[0] = INPUT_SV_YMUX;
            port_ids[1] = INPUT_SV_CMUX;
			break;

        case  INPUT_SOURCE_BT656_0:
            *u8port_count = 1;
            port_ids[0] = INPUT_PORT_BT656_0;
			break;

        case  INPUT_SOURCE_BT656_1:
            *u8port_count = 1;
            port_ids[0] = INPUT_PORT_BT656_1;
			break;

        case INPUT_SOURCE_CAMERA:
            *u8port_count = 1;
            port_ids[0] = INPUT_PORT_ICP;
            break;

        case INPUT_SOURCE_SC0_VOP:
            *u8port_count = 1;
            port_ids[0] = INPUT_PORT_SC0_VOP;
            break;

        case INPUT_SOURCE_SC1_VOP:
            *u8port_count = 1;
            port_ids[0] = INPUT_PORT_SC1_VOP;
            break;

        case INPUT_SOURCE_SC2_VOP:
            *u8port_count = 1;
            port_ids[0] = INPUT_PORT_SC2_VOP;
            break;

        case INPUT_SOURCE_NONE:
            *u8port_count = 1;
            port_ids[0] = INPUT_PORT_NONE_PORT;
            break;
    }
    //printf(" app : port 0 : %d  port 1 : %d \n",  port_ids[0]  , port_ids[1]);
}

//------------------------------------------------------------------------------------------------

static void _MApi_InputSource_ClearSysInputSourceFlag(SCALER_WIN eWindow)
{
    if(stSystemInfo[eWindow].enInputSourceType < INPUT_SOURCE_NUM)
    {
        EN_SYS_INPUT_SOURCE_TYPE enFlag = EN_SYS_INPUT_SOURCE_TYPE_NONE;
        //+sid, change from stGenSetting to stSystemInfo
        switch(stSystemInfo[eWindow].enInputSourceType)
        {
            case INPUT_SOURCE_DTV:
            case INPUT_SOURCE_CAMERA:
                enFlag = EN_SYS_INPUT_SOURCE_TYPE_DTV;
                break;
            case INPUT_SOURCE_TV:
                enFlag = EN_SYS_INPUT_SOURCE_TYPE_ATV;
                break;
            case INPUT_SOURCE_CVBS:
            case INPUT_SOURCE_CVBS2:
            case INPUT_SOURCE_CVBS3:
            case INPUT_SOURCE_CVBS4:
            case INPUT_SOURCE_CVBS5:
            case INPUT_SOURCE_CVBS6:
            case INPUT_SOURCE_CVBS7:
            case INPUT_SOURCE_CVBS8:
                enFlag = EN_SYS_INPUT_SOURCE_TYPE_CVBS;
                break;
            case INPUT_SOURCE_SVIDEO:
            case INPUT_SOURCE_SVIDEO2:
            case INPUT_SOURCE_SVIDEO3:
            case INPUT_SOURCE_SVIDEO4:
                enFlag = EN_SYS_INPUT_SOURCE_TYPE_SVIDEO;
                break;
            case INPUT_SOURCE_YPBPR:
            case INPUT_SOURCE_YPBPR2:
            case INPUT_SOURCE_YPBPR3:
                enFlag = EN_SYS_INPUT_SOURCE_TYPE_YPBPR;
                break;
            case INPUT_SOURCE_SCART:
            case INPUT_SOURCE_SCART2:
                enFlag = EN_SYS_INPUT_SOURCE_TYPE_SCART;
                break;
            case INPUT_SOURCE_HDMI:
            case INPUT_SOURCE_HDMI2:
            case INPUT_SOURCE_HDMI3:
            case INPUT_SOURCE_HDMI4:
                enFlag = EN_SYS_INPUT_SOURCE_TYPE_HDMI;
                break;
            case INPUT_SOURCE_VGA:
                enFlag = EN_SYS_INPUT_SOURCE_TYPE_VGA;
                break;
            case INPUT_SOURCE_STORAGE:
                enFlag = EN_SYS_INPUT_SOURCE_TYPE_STORAGE;
                break;
			case INPUT_SOURCE_BT656_0:
			case INPUT_SOURCE_BT656_1:
                enFlag = EN_SYS_INPUT_SOURCE_TYPE_BT656;
				break;
            default:
                break;
        }
        msAPI_InputSrcType_ClrType(enFlag);
        SCA_DBG(SCA_DBG_LV_INPUTSRC, "[Clear] _enSysInputSource = 0x%x\n", (MS_U16)msAPI_InputSrcType_GetType());

    }
}

static void _MApi_InputSource_SetSysInputSourceFlag(SCALER_WIN eWindow)
{
    if(stSystemInfo[eWindow].enInputSourceType < INPUT_SOURCE_NUM)
    {
        EN_SYS_INPUT_SOURCE_TYPE enFlag = EN_SYS_INPUT_SOURCE_TYPE_NONE;
        switch(stSystemInfo[eWindow].enInputSourceType)
        {
            case INPUT_SOURCE_DTV:
            case INPUT_SOURCE_CAMERA:
                enFlag = EN_SYS_INPUT_SOURCE_TYPE_DTV;
                break;
            case INPUT_SOURCE_TV:
                enFlag = EN_SYS_INPUT_SOURCE_TYPE_ATV;
                break;
            case INPUT_SOURCE_CVBS:
            case INPUT_SOURCE_CVBS2:
            case INPUT_SOURCE_CVBS3:
            case INPUT_SOURCE_CVBS4:
            case INPUT_SOURCE_CVBS5:
            case INPUT_SOURCE_CVBS6:
            case INPUT_SOURCE_CVBS7:
            case INPUT_SOURCE_CVBS8:
                enFlag = EN_SYS_INPUT_SOURCE_TYPE_CVBS;
                break;
            case INPUT_SOURCE_SVIDEO:
            case INPUT_SOURCE_SVIDEO2:
            case INPUT_SOURCE_SVIDEO3:
            case INPUT_SOURCE_SVIDEO4:
                enFlag = EN_SYS_INPUT_SOURCE_TYPE_SVIDEO;
                break;
            case INPUT_SOURCE_YPBPR:
            case INPUT_SOURCE_YPBPR2:
            case INPUT_SOURCE_YPBPR3:
                enFlag = EN_SYS_INPUT_SOURCE_TYPE_YPBPR;
                break;
            case INPUT_SOURCE_SCART:
            case INPUT_SOURCE_SCART2:
                enFlag = EN_SYS_INPUT_SOURCE_TYPE_SCART;
                break;
            case INPUT_SOURCE_HDMI:
            case INPUT_SOURCE_HDMI2:
            case INPUT_SOURCE_HDMI3:
            case INPUT_SOURCE_HDMI4:
                enFlag = EN_SYS_INPUT_SOURCE_TYPE_HDMI;
                break;
            case INPUT_SOURCE_VGA:
                enFlag = EN_SYS_INPUT_SOURCE_TYPE_VGA;
                break;
            case INPUT_SOURCE_STORAGE:
                enFlag = EN_SYS_INPUT_SOURCE_TYPE_STORAGE;
                break;
			case INPUT_SOURCE_BT656_0:
			case INPUT_SOURCE_BT656_1:
                enFlag = EN_SYS_INPUT_SOURCE_TYPE_BT656;
				break;
            default:
                break;
        }
        msAPI_InputSrcType_SetType(enFlag);
        SCA_DBG(SCA_DBG_LV_INPUTSRC, "[Set] _enSysInputSource = 0x%x\r\n", (MS_U16)msAPI_InputSrcType_GetType());
    }
}

//*************************************************************************
//Function name:    _MApi_InputSource_SetSystemmInfo
//Passing parameter:    MS_U8  u8InputSrcType: current input source type
//                        MS_SYS_INFO *penMsSysInfo
//Return parameter:    none
//Description:            get system info. by enInputSourceType
//*************************************************************************
void _MApi_InputSource_SetSystemmInfo( E_UI_INPUT_SOURCE enUiInputSourceType, MS_SYS_INFO *penMsSysInfo)
{
    switch ( enUiInputSourceType )
    {
        case UI_INPUT_SOURCE_RGB:
            penMsSysInfo->enInputSourceType = INPUT_SOURCE_VGA;
            break;

        #if (INPUT_YPBPR_VIDEO_COUNT >= 1)
        case UI_INPUT_SOURCE_COMPONENT:
            penMsSysInfo->enInputSourceType = INPUT_SOURCE_YPBPR;
            break;
        #endif

        #if (INPUT_YPBPR_VIDEO_COUNT >= 2)
        case UI_INPUT_SOURCE_COMPONENT2:
            penMsSysInfo->enInputSourceType = INPUT_SOURCE_YPBPR2;
            break;
        #endif

        #if (INPUT_AV_VIDEO_COUNT >= 1)
        case UI_INPUT_SOURCE_AV:
            penMsSysInfo->enInputSourceType = INPUT_SOURCE_CVBS;
            break;
        #endif

        #if (INPUT_AV_VIDEO_COUNT >= 2)
        case UI_INPUT_SOURCE_AV2:
            penMsSysInfo->enInputSourceType = INPUT_SOURCE_CVBS2;
            break;
        #endif

        #if (INPUT_AV_VIDEO_COUNT >= 3)
        case UI_INPUT_SOURCE_AV3:
            penMsSysInfo->enInputSourceType = INPUT_SOURCE_CVBS3;
            break;
        #endif

        #if (INPUT_SV_VIDEO_COUNT >= 1)
        case UI_INPUT_SOURCE_SVIDEO:
            penMsSysInfo->enInputSourceType = INPUT_SOURCE_SVIDEO;
            break;
        #endif

        #if (INPUT_SV_VIDEO_COUNT >= 2)
        case UI_INPUT_SOURCE_SVIDEO2:
            penMsSysInfo->enInputSourceType = INPUT_SOURCE_SVIDEO2;
            break;
        #endif

        case UI_INPUT_SOURCE_DTV:
            penMsSysInfo->enInputSourceType = INPUT_SOURCE_DTV;
            break;
        case UI_INPUT_SOURCE_TV:
            penMsSysInfo->enInputSourceType = INPUT_SOURCE_TV;
            break;

        default:
            penMsSysInfo->enInputSourceType = INPUT_SOURCE_NONE;
            break;
    }
}

static void _MApi_InputSource_InitModeVariables ( SCALER_WIN eWindow )
{
    // restart timing monitor
    MApi_XC_PCMonitor_Restart(eWindow);
}

static void _MApi_ProgDigigalWin(INPUT_SOURCE_TYPE_t enInputSourceType, EN_VD_SIGNALTYPE enVideoSystem,
                                 XC_SETWIN_INFO *pstXC_SetWin_Info, SCALER_WIN eWindow)
{
#if 1
    MS_U16 u16InputHFreq;
    PQ_WIN enPQWin;
    MS_PQ_Vd_Info stPQVDInfo;
    EN_VD_SIGNALTYPE enBackupVideoSystem;
#if(PQ_ENABLE_VD_SAMPLING)
    MS_PQ_VD_Sampling_Info stVDSamplingInfo;
#endif
    MS_U8 u8Idx;

    SCA_DBG(SCA_DBG_LV_PROGWIN, "ProgDigitalWin: Vid%d Src:%d \n", enVideoSystem, enInputSourceType);

    switch(eWindow)
    {
    case MAIN_WINDOW:
        enPQWin = PQ_MAIN_WINDOW;
        break;
    case SUB_WINDOW:
        enPQWin = PQ_SUB_WINDOW;
        break;
    case SC1_MAIN_WINDOW:
        enPQWin = PQ_SC1_MAIN_WINDOW;
        break;
    case SC2_MAIN_WINDOW:
        enPQWin = PQ_SC2_MAIN_WINDOW;
        break;
    case SC2_SUB_WINDOW:
        enPQWin = PQ_SC2_SUB_WINDOW;
        break;

    default:
        SCA_DBG(SCA_DBG_LV_0, "_MApi_ProgDigigalWin: Unknown eWindow:%d \n", eWindow);
        return;
    }

    switch(msAPI_AVD_GetVideoStandard())
    {
    case E_VIDEOSTANDARD_NTSC_M:
        stPQVDInfo.enVideoStandard = E_PQ_VIDEOSTANDARD_NTSC_M;
        break;

    case E_VIDEOSTANDARD_PAL_BGHI:
        stPQVDInfo.enVideoStandard = E_PQ_VIDEOSTANDARD_PAL_BGHI;
        break;

    case E_VIDEOSTANDARD_SECAM:
        stPQVDInfo.enVideoStandard = E_PQ_VIDEOSTANDARD_SECAM;
        break;

    case E_VIDEOSTANDARD_NTSC_44:
        stPQVDInfo.enVideoStandard = E_PQ_VIDEOSTANDARD_NTSC_44;
        break;

    case E_VIDEOSTANDARD_PAL_M:
        stPQVDInfo.enVideoStandard = E_PQ_VIDEOSTANDARD_PAL_M;
        break;

    case E_VIDEOSTANDARD_PAL_N:
        stPQVDInfo.enVideoStandard = E_PQ_VIDEOSTANDARD_PAL_N;
        break;

    case E_VIDEOSTANDARD_PAL_60:
        stPQVDInfo.enVideoStandard = E_PQ_VIDEOSTANDARD_PAL_60;
        break;

    default:
        MS_ASSERT(0);
        stPQVDInfo.enVideoStandard = E_PQ_VIDEOSTANDARD_PAL_BGHI;
        break;
    }

    stPQVDInfo.bIsSCART_RGB = FALSE;
    stPQVDInfo.bIsVIFIN = TRUE;
    MDrv_PQ_Set_VDInfo(enPQWin, &stPQVDInfo);
    if ( enVideoSystem == SIG_NONE )
    {
        enVideoSystem = SIG_PAL;
    }

    enBackupVideoSystem = enVideoSystem;

#if(PQ_ENABLE_VD_SAMPLING)
    memset(&stVDSamplingInfo, 0, sizeof(MS_PQ_VD_Sampling_Info));

    if(IsSrcTypeATV(enInputSourceType))
    {
        MDrv_PQ_Get_VDSampling_Info(PQ_MAIN_WINDOW, PQ_INPUT_SOURCE_TV, stPQVDInfo.enVideoStandard, &stVDSamplingInfo);
    }
    else if(IsSrcTypeAV(enInputSourceType))
    {
        MDrv_PQ_Get_VDSampling_Info(PQ_MAIN_WINDOW, PQ_INPUT_SOURCE_CVBS, stPQVDInfo.enVideoStandard, &stVDSamplingInfo);
    }
    else if(IsSrcTypeSV(enInputSourceType))
    {
        MDrv_PQ_Get_VDSampling_Info(PQ_MAIN_WINDOW, PQ_INPUT_SOURCE_SVIDEO, stPQVDInfo.enVideoStandard, &stVDSamplingInfo);
    }
    else if(IsSrcTypeScart(enInputSourceType))
    {
        MDrv_PQ_Get_VDSampling_Info(PQ_MAIN_WINDOW, PQ_INPUT_SOURCE_SCART, stPQVDInfo.enVideoStandard, &stVDSamplingInfo);
    }

    msAPI_AVD_GetCaptureWindowByPQ(&(pstXC_SetWin_Info->stCapWin), enVideoSystem, &stVDSamplingInfo);
#else
    msAPI_AVD_GetCaptureWindow(&(pstXC_SetWin_Info->stCapWin), enVideoSystem, FALSE);
#endif

    pstXC_SetWin_Info->bHDuplicate    = FALSE;     // not set in this path

    u16InputHFreq = MApi_XC_CalculateHFreqx10(msAPI_Scaler_VD_GetHPeriod(eWindow, enInputSourceType, GET_SYNC_VIRTUAL));

    pstXC_SetWin_Info->u16InputVTotal = msAPI_Scaler_VD_GetVTotal(eWindow, enInputSourceType, GET_SYNC_VIRTUAL, u16InputHFreq);
    pstXC_SetWin_Info->bInterlace     = TRUE;
    pstXC_SetWin_Info->u16InputVFreq  = MApi_XC_CalculateVFreqx10(u16InputHFreq, pstXC_SetWin_Info->u16InputVTotal)*2;


    switch ( enVideoSystem )
    {
        case SIG_NTSC:
        case SIG_NTSC_443:
        case SIG_PAL_M:
            if ( (pstXC_SetWin_Info->u16InputVFreq > 610) || (pstXC_SetWin_Info->u16InputVFreq < 590) )
            {
                pstXC_SetWin_Info->u16InputVFreq = 600;
            }
            break;

        default:
        case SIG_PAL:
        case SIG_SECAM:
        case SIG_PAL_NC:
            if ( (pstXC_SetWin_Info->u16InputVFreq > 510) || (pstXC_SetWin_Info->u16InputVFreq < 490) )
            {
                pstXC_SetWin_Info->u16InputVFreq = 500;
            }
            break;
    }

    enVideoSystem = enBackupVideoSystem;

#if (PQ_ENABLE_VD_SAMPLING)
    if(stVDSamplingInfo.eType == PQ_VD_SAMPLING_ON)
    {
        pstXC_SetWin_Info->stCapWin.x = stVDSamplingInfo.u16Hstart;
        pstXC_SetWin_Info->stCapWin.y = stVDSamplingInfo.u16Vstart;
    }
    else
#endif
    {
        pstXC_SetWin_Info->stCapWin.x = CVBS_WinInfo[enVideoSystem][0].u16H_CapStart;
        pstXC_SetWin_Info->stCapWin.y = CVBS_WinInfo[enVideoSystem][0].u16V_CapStart;
    }

    u8Idx = enVideoSystem == SIG_NTSC ?
            MS_USER_DISPLAY_CVBS_NTSC :
            MS_USER_DISPLAY_CVBS_PAL;

    if(g_stUserDisplayCfg[eWindow][u8Idx].bEn)
    {
        MS_S16 s16tmp;

        s16tmp =(MS_S16) pstXC_SetWin_Info->stCapWin.x;
        s16tmp += g_stUserDisplayCfg[eWindow][u8Idx].s16H_Offset;
        s16tmp =  s16tmp < 0 ?  0 : s16tmp;
        pstXC_SetWin_Info->stCapWin.x = (MS_U16)s16tmp;

        s16tmp =(MS_S16) pstXC_SetWin_Info->stCapWin.y;
        s16tmp += g_stUserDisplayCfg[eWindow][u8Idx].s16V_Offset;
        s16tmp =  s16tmp < 0 ?  0 : s16tmp;
        pstXC_SetWin_Info->stCapWin.y = (MS_U16)s16tmp;
    }


    _u8H_OverScanRatio[eWindow] = OVERSCAN_DEFAULT_H;
    _u8V_OverScanRatio[eWindow] = OVERSCAN_DEFAULT_V;
#endif
}

static void _MApi_ProgCameraWin(SCALER_WIN eWindow, INPUT_SOURCE_TYPE_t enInputSourceType, MS_INPUT_TIMING_CONFIG *ptiming, XC_SETWIN_INFO *pstXC_SetWin_Info)
{
    PQ_WIN enPQWin;
    MS_PQ_Dtv_Info stPQDTVInfo;

    pstXC_SetWin_Info->stCapWin.x = ptiming->u16x;
    pstXC_SetWin_Info->stCapWin.y = ptiming->u16y;
    pstXC_SetWin_Info->stCapWin.width = ptiming->u16Width;
    pstXC_SetWin_Info->stCapWin.height = ptiming->u16Height;

    pstXC_SetWin_Info->bHDuplicate    = FALSE;
    pstXC_SetWin_Info->u16InputVTotal = ptiming->u16Vtotal;
    pstXC_SetWin_Info->bInterlace     = ptiming->bInterlace;
    pstXC_SetWin_Info->u16InputVFreq  = ptiming->u16VFreq;


    _u8H_OverScanRatio[eWindow] = OVERSCAN_DEFAULT_H;
    _u8V_OverScanRatio[eWindow] = OVERSCAN_DEFAULT_V;


    switch(eWindow)
    {
    case MAIN_WINDOW:
        enPQWin = PQ_MAIN_WINDOW;
        break;
    case SUB_WINDOW:
        enPQWin = PQ_SUB_WINDOW;
        break;
    case SC1_MAIN_WINDOW:
        enPQWin = PQ_SC1_MAIN_WINDOW;
        break;
    case SC2_MAIN_WINDOW:
        enPQWin = PQ_SC2_MAIN_WINDOW;
        break;
    case SC2_SUB_WINDOW:
        enPQWin = PQ_SC2_SUB_WINDOW;
        break;

    default:
        SCA_DBG(SCA_DBG_LV_0, "_MApi_ProgMVDWin: Unknown eWindow:%d \n", eWindow);
        return;
    }

    stPQDTVInfo.eType = E_PQ_DTV_MPEG2;
    MDrv_PQ_Set_DTVInfo(enPQWin, &stPQDTVInfo);
}


static void _MApi_ProgMVDWin(SCALER_WIN eWindow, INPUT_SOURCE_TYPE_t enInputSourceType, MVOP_Timing *ptiming, XC_SETWIN_INFO *pstXC_SetWin_Info)
{
    PQ_WIN enPQWin;
    MS_PQ_Dtv_Info stPQDTVInfo;
    MS_PQ_MuliMedia_Info stPQMMInfo;

    pstXC_SetWin_Info->stCapWin.x = MDrv_MVOP_GetHStart();
    pstXC_SetWin_Info->stCapWin.y = MDrv_MVOP_GetVStart();
    pstXC_SetWin_Info->stCapWin.width = ptiming->u16Width;
    pstXC_SetWin_Info->stCapWin.height = ptiming->u16Height;

    pstXC_SetWin_Info->bHDuplicate    = ptiming->bHDuplicate;
    pstXC_SetWin_Info->u16InputVTotal = ptiming->u16V_TotalCount;
    pstXC_SetWin_Info->bInterlace     = ptiming->bInterlace;
    if(pstXC_SetWin_Info->bInterlace)
    {
        pstXC_SetWin_Info->u16InputVFreq = (MS_U16)((ptiming->u16ExpFrameRate * 2 + 50) / 100);
    }
    else
    {
        pstXC_SetWin_Info->u16InputVFreq = (MS_U16)((ptiming->u16ExpFrameRate + 50) / 100);
    }

    _u8H_OverScanRatio[eWindow] = OVERSCAN_DEFAULT_H;
    _u8V_OverScanRatio[eWindow] = OVERSCAN_DEFAULT_V;


    switch(eWindow)
    {
    case MAIN_WINDOW:
        enPQWin = PQ_MAIN_WINDOW;
        break;
    case SUB_WINDOW:
        enPQWin = PQ_SUB_WINDOW;
        break;
    case SC1_MAIN_WINDOW:
        enPQWin = PQ_SC1_MAIN_WINDOW;
        break;
    case SC2_MAIN_WINDOW:
        enPQWin = PQ_SC2_MAIN_WINDOW;
        break;
    case SC2_SUB_WINDOW:
        enPQWin = PQ_SC2_SUB_WINDOW;
        break;

    default:
        SCA_DBG(SCA_DBG_LV_0, "_MApi_ProgMVDWin: Unknown eWindow:%d \n", eWindow);
        return;
    }

    if(IsSrcTypeDTV(enInputSourceType))
    {
    #if 0 //ToDo!!
        if(g_eCodecType == E_VDEC_CODEC_TYPE_H264)
            stPQDTVInfo.eType = E_PQ_DTV_H264;
        else
    #endif
             stPQDTVInfo.eType = E_PQ_DTV_MPEG2;

        MDrv_PQ_Set_DTVInfo(enPQWin, &stPQDTVInfo);
    }

    if(IsSrcTypeStorage(enInputSourceType) || IsSrcTypeJpeg(enInputSourceType))
    {
        stPQMMInfo.eType = E_PQ_MULTIMEDIA_MOVIE;
        MDrv_PQ_Set_MultiMediaInfo(enPQWin, &stPQMMInfo);
    }
}

static void _MApi_ProgSC_VOP_Win(INPUT_SOURCE_TYPE_t enInputSourceType, XC_SETWIN_INFO *pstXC_SetWin_Info, SCALER_WIN eSC_VopWin, SCALER_WIN eWindow)
{
    PQ_WIN enPQWin;
    XC_OUPUT_CFG stOutputCfg;
    MS_PQ_Dtv_Info stPQDTVInfo;

    MApi_XC_Get_OuputCfg(&stOutputCfg, eSC_VopWin);

    pstXC_SetWin_Info->stCapWin.x = stOutputCfg.u16Hstart;
    pstXC_SetWin_Info->stCapWin.y = stOutputCfg.u16Vstart;
    pstXC_SetWin_Info->stCapWin.width = stOutputCfg.u16Width;
    pstXC_SetWin_Info->stCapWin.height = stOutputCfg.u16Height;

    pstXC_SetWin_Info->bHDuplicate    = FALSE;
    pstXC_SetWin_Info->u16InputVTotal = stOutputCfg.u16Vtt;
    pstXC_SetWin_Info->bInterlace     = stOutputCfg.bInterlace;
    pstXC_SetWin_Info->u16InputVFreq  = stOutputCfg.u16OutputFreq;


    switch(eWindow)
    {
    case MAIN_WINDOW:
        enPQWin = PQ_MAIN_WINDOW;
        break;
    case SUB_WINDOW:
        enPQWin = PQ_SUB_WINDOW;
        break;
    case SC1_MAIN_WINDOW:
        enPQWin = PQ_SC1_MAIN_WINDOW;
        break;
    case SC2_MAIN_WINDOW:
        enPQWin = PQ_SC2_MAIN_WINDOW;
        break;
    case SC2_SUB_WINDOW:
        enPQWin = PQ_SC2_SUB_WINDOW;
        break;

    default:
        SCA_DBG(SCA_DBG_LV_0, "_MApi_ProgSC_VOP_Win: Unknown eWindow:%d \n", eWindow);
        return;
    }

    stPQDTVInfo.eType = E_PQ_DTV_MPEG2;

    MDrv_PQ_Set_DTVInfo(enPQWin, &stPQDTVInfo);

    SCA_DBG(SCA_DBG_LV_PROGWIN, "ProgSC_VOP: x:%d, y:%d, w:%d, h:%d, inter:%d, Freq:%d, Vtt:%d \n",
                                    pstXC_SetWin_Info->stCapWin.x, pstXC_SetWin_Info->stCapWin.y,
                                    pstXC_SetWin_Info->stCapWin.width, pstXC_SetWin_Info->stCapWin.height,
                                    pstXC_SetWin_Info->bInterlace, pstXC_SetWin_Info->u16InputVFreq, pstXC_SetWin_Info->u16InputVTotal);

    _u8H_OverScanRatio[eWindow] = OVERSCAN_DEFAULT_H;
    _u8V_OverScanRatio[eWindow] = OVERSCAN_DEFAULT_V;

}

/******************************************************************************/
///Setup input and output window information including aspect ratio, scaling.(analog part)
///@param enPortType \b IN
///- input Port (RGB or YPbPr) only could be use
///@param pstModesetting \b IN
///- pointer to mode settings
///@param u8CurrentSyncStatus \b IN
///- current mode index
///@return
///- pointer to current window information
/******************************************************************************/
static void _MApi_ProgAnalogWin ( INPUT_SOURCE_TYPE_t enInputSourceType,
                                         MS_PCADC_MODESETTING_TYPE *pstModesetting,
                                         MS_U8 u8CurrentSyncStatus,
                                         XC_SETWIN_INFO *pstXC_SetWin_Info,
                                         SCALER_WIN eWindow)
{

    MS_PQ_Mode_Info stModeInfo;
    MS_PQ_ADC_SAMPLING_TYPE eADC_Sampling_type;
    MS_U16 u16ratio;
    PQ_WIN ePQWin = eWindow == MAIN_WINDOW     ? PQ_MAIN_WINDOW :
                    eWindow == SUB_WINDOW      ? PQ_SUB_WINDOW  :
                    eWindow == SC1_MAIN_WINDOW ? PQ_SC1_MAIN_WINDOW :
                    eWindow == SC2_MAIN_WINDOW ? PQ_SC2_MAIN_WINDOW :
                                                 PQ_SC2_SUB_WINDOW;


    pstXC_SetWin_Info->bHDuplicate    = FALSE;
    pstXC_SetWin_Info->u16InputVTotal = MApi_XC_PCMonitor_Get_Vtotal(eWindow);

    if(IsSrcTypeYPbPr(enInputSourceType) && (g_PcadcModeSetting[eWindow].u8ModeIndex == MD_720x480_60I_P))
    {
        // this is patch for DVD 480i -> Sherwood -> component output -> OSD unsupport mode
        pstXC_SetWin_Info->bInterlace = TRUE;
    }
    else
    {
        pstXC_SetWin_Info->bInterlace = u8CurrentSyncStatus & XC_MD_INTERLACE_BIT ? TRUE : FALSE;
    }

    pstXC_SetWin_Info->u16InputVFreq  = MApi_XC_PCMonitor_Get_VFreqx10(eWindow);

    // VGA or YPbPr
    pstXC_SetWin_Info->stCapWin.x      = pstModesetting->u16DefaultHStart* 2 - pstModesetting->u16HorizontalStart;
    pstXC_SetWin_Info->stCapWin.y      = pstModesetting->u16VerticalStart;

    if(IsSrcTypeYPbPr(enInputSourceType) && g_stUserDisplayCfg[eWindow][MS_USER_DISPLAY_YPBPR].bEn)
    {
        MS_S16 s16tmp;

        s16tmp =(MS_S16) pstXC_SetWin_Info->stCapWin.x;
        s16tmp += g_stUserDisplayCfg[eWindow][MS_USER_DISPLAY_YPBPR].s16H_Offset;
        s16tmp =  s16tmp < 0 ?  0 : s16tmp;
        pstXC_SetWin_Info->stCapWin.x = (MS_U16)s16tmp;

        s16tmp =(MS_S16) pstXC_SetWin_Info->stCapWin.y;
        s16tmp += g_stUserDisplayCfg[eWindow][MS_USER_DISPLAY_YPBPR].s16V_Offset;
        s16tmp =  s16tmp < 0 ?  0 : s16tmp;
        pstXC_SetWin_Info->stCapWin.y = (MS_U16)s16tmp;
    }
    else if(IsSrcTypeVga(enInputSourceType) && g_stUserDisplayCfg[eWindow][MS_USER_DISPLAY_VGA].bEn)
    {
        MS_S16 s16tmp;

        s16tmp =(MS_S16) pstXC_SetWin_Info->stCapWin.x;
        s16tmp += g_stUserDisplayCfg[eWindow][MS_USER_DISPLAY_VGA].s16H_Offset;
        s16tmp =  s16tmp < 0 ?  0 : s16tmp;
        pstXC_SetWin_Info->stCapWin.x = (MS_U16)s16tmp;

        s16tmp =(MS_S16) pstXC_SetWin_Info->stCapWin.y;
        s16tmp += g_stUserDisplayCfg[eWindow][MS_USER_DISPLAY_VGA].s16V_Offset;
        s16tmp =  s16tmp < 0 ?  0 : s16tmp;
        pstXC_SetWin_Info->stCapWin.y = (MS_U16)s16tmp;
    }

    pstXC_SetWin_Info->stCapWin.width  = MApi_PCMode_Get_HResolution( eWindow ,TRUE); // standard display width
    pstXC_SetWin_Info->stCapWin.height = MApi_PCMode_Get_VResolution( eWindow ,TRUE); // standard display height


    stModeInfo.bFBL = FALSE;
    stModeInfo.bInterlace = pstXC_SetWin_Info->bInterlace;
    stModeInfo.u16input_hsize = pstXC_SetWin_Info->stCapWin.width;
    stModeInfo.u16input_vsize = pstXC_SetWin_Info->stCapWin.height;
    stModeInfo.u16input_vfreq = pstXC_SetWin_Info->u16InputVFreq;
    eADC_Sampling_type = MDrv_PQ_Get_ADCSampling_Info(ePQWin, IsSrcTypeYPbPr(enInputSourceType) ? PQ_INPUT_SOURCE_YPBPR : PQ_INPUT_SOURCE_VGA, &stModeInfo);


    switch(eADC_Sampling_type)
    {
    default:
        u16ratio = 1;
        pstXC_SetWin_Info->bHDuplicate = FALSE;
        MS_ASSERT(0);
        break;

    case E_PQ_ADC_SAMPLING_NOT_SUPPORT:
        u16ratio = 1;
        pstXC_SetWin_Info->bHDuplicate = FALSE;
        if(IsSrcTypeYPbPr(enInputSourceType))
        {
            if(g_PcadcModeSetting[eWindow].u8ModeIndex == MD_720x480_60I ||
               g_PcadcModeSetting[eWindow].u8ModeIndex == MD_720x480_60P ||
               g_PcadcModeSetting[eWindow].u8ModeIndex == MD_720x576_50I ||
               g_PcadcModeSetting[eWindow].u8ModeIndex == MD_720x576_50P ||
               g_PcadcModeSetting[eWindow].u8ModeIndex == MD_720x480_60I_P)
            {
                pstXC_SetWin_Info->stCapWin.x <<= 1; // for better quality
                pstXC_SetWin_Info->stCapWin.width <<= 1;
                pstXC_SetWin_Info->bHDuplicate = TRUE;
            }
        }
        break;

    case E_PQ_ADC_SAMPLING_NONE:
        u16ratio = 1;
        pstXC_SetWin_Info->bHDuplicate = FALSE;
        break;

    case E_PQ_ADC_SAMPLING_X_1:
        u16ratio = 1;
        pstXC_SetWin_Info->bHDuplicate = FALSE;
        break;
    case E_PQ_ADC_SAMPLING_X_2:
        u16ratio = 2;
        pstXC_SetWin_Info->bHDuplicate = TRUE;
        break;

    case E_PQ_ADC_SAMPLING_X_4:
        u16ratio = 4;
        pstXC_SetWin_Info->bHDuplicate = TRUE;
        break;

    case E_PQ_ADC_SAMPLING_X_8:
        u16ratio = 8;
        pstXC_SetWin_Info->bHDuplicate = TRUE;
        break;
    }

    pstXC_SetWin_Info->stCapWin.x *= u16ratio;
    pstXC_SetWin_Info->stCapWin.width *= u16ratio;


    // these 2 settings only for VGA/YPbPr
    // must be mode table user data because everytime called set window such as change Aspect Ratio
    // these values will be set again to ADC
    // UI can change VGA Htotal and Phase, so must pass UI setting
    pstXC_SetWin_Info->u16DefaultHtotal = g_PcadcModeSetting[eWindow].u16HorizontalTotal;
    pstXC_SetWin_Info->u8DefaultPhase   = (MS_U8)g_PcadcModeSetting[eWindow].u16Phase;

    if(IsSrcTypeVga(enInputSourceType))
    {
        _u8H_OverScanRatio[eWindow] = 0;
        _u8V_OverScanRatio[eWindow] = 0;
    }
    else
    {
        _u8H_OverScanRatio[eWindow] = OVERSCAN_DEFAULT_H;
        _u8V_OverScanRatio[eWindow] = OVERSCAN_DEFAULT_V;
    }

}

void _MApi_AlignWindow(MS_WINDOW_TYPE *pWindow, MS_U16 u16AlignX, MS_U16 u16AlignY)
{
    MS_U16 u16temp;
    u16temp = ((pWindow->x + u16AlignX-1) & (~(u16AlignX-1))) - pWindow->x;
    pWindow->x += u16temp;
    pWindow->width -= (u16temp*2);

    u16temp = ((pWindow->y + u16AlignY-1) & (~(u16AlignY-1))) - pWindow->y;
    pWindow->y += u16temp;
    pWindow->height -= (u16temp*2);
}


//********************************************************************************************
// Program capture win/crop win/display window
//   parameter:
//     pDisplayWindow  - The display window before adjust it .i.e: adjust aspect ratio to it.
//     eWindow             - Display window id, i.e Main_WINDOW or SUB_WINDOW
//********************************************************************************************
static void _MApi_check_crop_win( XC_SETWIN_INFO *pstXC_SetWin_Info )
{
    if (pstXC_SetWin_Info->stCropWin.width > pstXC_SetWin_Info->stCapWin.width)
    {
        SCAIO_ASSERT(0);
        pstXC_SetWin_Info->stCropWin.width = pstXC_SetWin_Info->stCapWin.width;
    }

    if (pstXC_SetWin_Info->stCropWin.height > pstXC_SetWin_Info->stCapWin.height)
    {
        SCAIO_ASSERT(0);
        pstXC_SetWin_Info->stCropWin.height = pstXC_SetWin_Info->stCapWin.height;
    }

    if (pstXC_SetWin_Info->stCropWin.x > pstXC_SetWin_Info->stCapWin.width - pstXC_SetWin_Info->stCropWin.width)
    {
        SCAIO_ASSERT(0);
        pstXC_SetWin_Info->stCropWin.x = pstXC_SetWin_Info->stCapWin.width - pstXC_SetWin_Info->stCropWin.width;
    }
    if (pstXC_SetWin_Info->stCropWin.y > pstXC_SetWin_Info->stCapWin.height - pstXC_SetWin_Info->stCropWin.height)
    {
        SCAIO_ASSERT(0);
        pstXC_SetWin_Info->stCropWin.y = pstXC_SetWin_Info->stCapWin.height - pstXC_SetWin_Info->stCropWin.height;
    }
}


#if ENABLE_PWS
static E_PWS_SouceInfo _MAPP_InputSource_Source2PWSSource(INPUT_SOURCE_TYPE_t src )
{
    E_PWS_SouceInfo enTargetSource = _UNKNOWN_;

    if ( IsSrcTypeVga(src) )
    {
        enTargetSource = _RGB_;
    }
    else if ( IsSrcTypeYPbPr(src) )
    {
        enTargetSource = _YPbPr_;
    }
    else if ( IsSrcTypeDTV(src) )
    {
        enTargetSource = _DTV_DVB_; // ATSC???
    }
    else if ( src == INPUT_SOURCE_HDMI)
    {
        enTargetSource = _HDMI1_;
    }
    else if ( src == INPUT_SOURCE_HDMI2)
    {
        enTargetSource = _HDMI2_;
    }
    else if ( src == INPUT_SOURCE_HDMI3)
    {
        enTargetSource = _HDMI3_;
    }
    else if ( src == INPUT_SOURCE_HDMI4)
    {
        enTargetSource = _HDMI4_;
    }
    else if ( IsSrcTypeScart(src) )
    {
        enTargetSource = _SCART_;
    }
    else if ( IsSrcTypeAV(src) )
    {
        enTargetSource = _CVBS_;
    }
    else if ( IsSrcTypeSV(src) )
    {
        enTargetSource = _SV_;
    }
    else if ( IsSrcTypeStorage(src) )
    {
        enTargetSource = _USB_;
    }
    else if ( IsSrcTypeATV(src) )
    {
        enTargetSource = _ATV_VIF_;
    }

    return enTargetSource;
}
#endif

//------------------------------------------------------------------------------
//  export function
//------------------------------------------------------------------------------
void MApi_InputSource_SwitchSource ( INPUT_SOURCE_TYPE_t enInputSourceType, SCALER_WIN eWindow )
{
    XC_MUX_PATH_INFO PathInfo;
    MS_S16 s16PathId;
    E_DEST_TYPE enDesType;

    if (eWindow >= MAX_WINDOW)
    {
        SCA_DBG(SCA_DBG_LV_INPUTSRC, "invalid window number assigned!\n");
        eWindow = MAIN_WINDOW;
    }

    enDesType = eWindow == MAIN_WINDOW ? OUTPUT_SCALER_MAIN_WINDOW :
                eWindow == SUB_WINDOW  ? OUTPUT_SCALER_SUB_WINDOW  :
                eWindow == SC1_MAIN_WINDOW ? OUTPUT_SCALER1_MAIN_WINDOW :
                eWindow == SC2_MAIN_WINDOW ? OUTPUT_SCALER2_MAIN_WINDOW :
                                             OUTPUT_SCALER2_SUB_WINDOW;

    MApi_XC_Mux_DeletePath(SYS_INPUT_SOURCE_TYPE(eWindow),enDesType);

#if ENABLE_PWS
	MDrv_PWS_HandleSource(PWS_DEL_SOURCE,_MAPP_InputSource_Source2PWSSource( SYS_INPUT_SOURCE_TYPE(eWindow) ),PWS_FULL);
#endif

    _MApi_InputSource_ClearSysInputSourceFlag(eWindow);
    stSystemInfo[eWindow].enInputSourceType = enInputSourceType;
    //_MApi_InputSource_SetSystemmInfo( enUIInputSourceType, &stSystemInfo[eWindow] );
    _MApi_InputSource_SetSysInputSourceFlag(eWindow);

    // Enable black screen
    MApi_SetBlueScreen( ENABLE, MApi_Scaler_Get_DisplayMute_ColorType(eWindow), DEFAULT_SCREEN_UNMUTE_TIME, eWindow);

    _MApi_InputSource_InitModeVariables(eWindow); // initialize mode variables

    PathInfo.path_thread = NULL;
    PathInfo.Path_Type = PATH_TYPE_SYNCHRONOUS;
    PathInfo.src = SYS_INPUT_SOURCE_TYPE(eWindow);

    PathInfo.dest = eWindow == MAIN_WINDOW     ? OUTPUT_SCALER_MAIN_WINDOW :
                    eWindow == SUB_WINDOW      ? OUTPUT_SCALER_SUB_WINDOW :
                    eWindow == SC1_MAIN_WINDOW ? OUTPUT_SCALER1_MAIN_WINDOW :
                    eWindow == SC2_MAIN_WINDOW ? OUTPUT_SCALER2_MAIN_WINDOW :
                                                 OUTPUT_SCALER2_SUB_WINDOW;

    PathInfo.SyncEventHandler = NULL;
    PathInfo.DestOnOff_Event_Handler = NULL;
    PathInfo.dest_periodic_handler = NULL;

#if ENABLE_PWS
	MDrv_PWS_HandleSource(PWS_ADD_SOURCE,_MAPP_InputSource_Source2PWSSource( stSystemInfo[eWindow].enInputSourceType ),PWS_FULL);
#endif

    s16PathId = MApi_XC_Mux_CreatePath( &PathInfo, sizeof(XC_MUX_PATH_INFO) );
    if (s16PathId == -1)
    {
        SCA_DBG(SCA_DBG_LV_0, "Create path fail src = %d  dest = %d, your structure has wrong size with library \n", PathInfo.src ,PathInfo.dest);
    }
    else
    {
        MApi_XC_Mux_EnablePath( (MS_U16)s16PathId );
    }

    // ToDo!! Create CVBS out path

    // ToDo!! MApp_InputSource_SetInputSource
    MApi_SetSourceType(enInputSourceType, eWindow);

}

void _MApi_Scaler_CalculateOverScan(INPUT_SOURCE_TYPE_t enInputSourceType, MS_WINDOW_TYPE *pstCropWin, SCALER_WIN eWindow)
{
    MS_U32 u32Temp;

    if((eWindow == MAIN_WINDOW || eWindow == SUB_WINDOW) &&
       (IsSrcTypeYPbPr(enInputSourceType) || IsSrcTypeDigitalVD(enInputSourceType) || IsSrcTypeVga(enInputSourceType)))
    {
        MS_U8 u8idx;

        SCA_DBG(SCA_DBG_LV_PROGWIN, "Before OverScan: %d, %d, %d, %d \n", pstCropWin->x, pstCropWin->y, pstCropWin->width, pstCropWin->height);

        u8idx = IsSrcTypeYPbPr(enInputSourceType) ? MS_USER_DISPLAY_YPBPR :
                IsSrcTypeVga(enInputSourceType)    ? MS_USER_DISPLAY_VGA :
                msAPI_AVD_GetVideoStandard() ==  E_VIDEOSTANDARD_NTSC_M ? MS_USER_DISPLAY_CVBS_NTSC:
                                                                          MS_USER_DISPLAY_CVBS_PAL;
        if(g_stUserDisplayCfg[eWindow][u8idx].bEn)
        {
            pstCropWin->x += g_stUserDisplayCfg[eWindow][u8idx].u16H_Overscan;
            pstCropWin->y += g_stUserDisplayCfg[eWindow][u8idx].u16V_Overscan;
            pstCropWin->width =  pstCropWin->width - 2 * g_stUserDisplayCfg[eWindow][u8idx].u16H_Overscan;
            pstCropWin->height = pstCropWin->height - 2 * g_stUserDisplayCfg[eWindow][u8idx].u16V_Overscan;
        }
        else
        {
            if (_u8H_OverScanRatio[eWindow]) // Horizontal
            {
                u32Temp = ((MS_U32)pstCropWin->width * (MS_U32)_u8H_OverScanRatio[eWindow] + 500 )/1000;
                pstCropWin->x += (MS_U16) u32Temp;
                pstCropWin->width = pstCropWin->width - (MS_U16)u32Temp*2;
            }

            if (_u8V_OverScanRatio[eWindow]) // Vertical
            {
                u32Temp = ((MS_U32)pstCropWin->height * (MS_U32)_u8V_OverScanRatio[eWindow] + 500 )/1000;
                pstCropWin->y += (MS_U16) u32Temp;
                pstCropWin->height = pstCropWin->height - (MS_U16)u32Temp*2;
            }
        }
        SCA_DBG(SCA_DBG_LV_PROGWIN, "After OverScan: %d, %d, %d, %d \n", pstCropWin->x, pstCropWin->y, pstCropWin->width, pstCropWin->height);
    }
}


//********************************************************************************************
// Program capture win/crop win/display window
//   parameter:
//     ptSrcWin  - pointer to src window (not including timing hstart/vstart)
//                 NULL to use default value
//     ptCropWin - pointer to crop window
//                 NULL to use default value
//     ptDstWin  - pointer to dst window (not including timing hstart/vstart)
//                 NULL to use default value
//     enVideoScreen - aspect ratio code
//********************************************************************************************
MS_BOOL MApi_Scaler_SetWindow(MS_WINDOW_TYPE *ptSrcWin,               // start from (0,0)
                           MS_WINDOW_TYPE *ptCropWin,              // start from (0,0)
                           MS_WINDOW_TYPE *ptDstWin,               // start from (0,0)
                           INPUT_SOURCE_TYPE_t enInputSourceType,
                           SCALER_WIN eWindow)
{
    MS_U16 u16AlignX=0, u16AlignY=0;
    XC_SETWIN_INFO stXC_SetWin_Info;
    MVOP_Timing stMVOPTiming;

    memset(&stXC_SetWin_Info, 0, sizeof(XC_SETWIN_INFO));

    stXC_SetWin_Info.enInputSourceType = enInputSourceType;

    if( IsSrcTypeAnalog(enInputSourceType))
    {
        _MApi_ProgAnalogWin(enInputSourceType, &g_PcadcModeSetting[eWindow],
                                    MApi_XC_PCMonitor_GetSyncStatus(eWindow), &stXC_SetWin_Info, eWindow);

        u16AlignX = ANALOG_CROP_ALIGN_X;
        u16AlignY = ANALOG_CROP_ALIGN_Y;
    }
    else if( IsSrcTypeDTV(enInputSourceType) )
    {
        MDrv_MVOP_GetOutputTiming(&stMVOPTiming);
        _MApi_ProgMVDWin(eWindow, enInputSourceType, &stMVOPTiming, &stXC_SetWin_Info);
        u16AlignX = DTV_CROP_ALIGN_X;
        u16AlignY = DTV_CROP_ALIGN_Y;
    }
    else if( IsSrcTypeCamera(enInputSourceType) )
    {
        _MApi_ProgCameraWin(eWindow, enInputSourceType, &gstCameraTimingCfg, &stXC_SetWin_Info);
        u16AlignX = DTV_CROP_ALIGN_X;
        u16AlignY = DTV_CROP_ALIGN_Y;
    }
    else if( IsSrcTypeDigitalVD(enInputSourceType) )
    {
        _MApi_ProgDigigalWin( enInputSourceType, mvideo_vd_get_videosystem(), &stXC_SetWin_Info, eWindow );
        u16AlignX = DTV_CROP_ALIGN_X;
        u16AlignY = DTV_CROP_ALIGN_Y;
    }
    else if( IsSrcTypeSC_VOP(enInputSourceType) )
    {

        SCALER_WIN enVOP_Win = enInputSourceType == INPUT_SOURCE_SC0_VOP  ? MAIN_WINDOW :
                               enInputSourceType == INPUT_SOURCE_SC1_VOP  ? SC1_MAIN_WINDOW :
                                                                            SC2_MAIN_WINDOW;
        _MApi_ProgSC_VOP_Win(enInputSourceType, &stXC_SetWin_Info, enVOP_Win, eWindow);
    }
    else if( IsSrcTypeBT656(enInputSourceType))
    {
        _MApi_ProgAnalogWin(enInputSourceType, &g_PcadcModeSetting[eWindow],
                                    MApi_XC_PCMonitor_GetSyncStatus(eWindow), &stXC_SetWin_Info, eWindow);

        u16AlignX = ANALOG_CROP_ALIGN_X;
        u16AlignY = ANALOG_CROP_ALIGN_Y;
    }

    SCA_DBG(SCA_DBG_LV_PROGWIN, "SetWindow: Dest:%d, Vtt:%d, bInterlace:%d, Vfreq:%d, x:%d, y:%d, W:%d, H:%d, Htt:%d, Phase:%d\r\n",
                eWindow,
                stXC_SetWin_Info.u16InputVTotal, stXC_SetWin_Info.bInterlace,
                stXC_SetWin_Info.u16InputVFreq, stXC_SetWin_Info.stCapWin.x, stXC_SetWin_Info.stCapWin.y,
                stXC_SetWin_Info.stCapWin.width, stXC_SetWin_Info.stCapWin.height,
                stXC_SetWin_Info.u16DefaultHtotal, stXC_SetWin_Info.u8DefaultPhase);

    if (ptSrcWin)
    {
        // XC concept: capture window never start from (0,0) because it's base is input signal
        memcpy(&stXC_SetWin_Info.stCapWin, ptSrcWin, sizeof(MS_WINDOW_TYPE));
        SCAIO_ASSERT(stXC_SetWin_Info.stCapWin.x != 0);
        SCAIO_ASSERT(stXC_SetWin_Info.stCapWin.y != 0);
        stXC_SetWin_Info.bHDuplicate = FALSE;
        SCA_DBG(SCA_DBG_LV_PROGWIN, "pSrcWin x:%d, y:%d, H:%d, W:%d \n",
                stXC_SetWin_Info.stCapWin.x, stXC_SetWin_Info.stCapWin.y,
                stXC_SetWin_Info.stCapWin.width, stXC_SetWin_Info.stCapWin.height);
    }

    if (ptCropWin)
    {
        // XC concept: crop window must start from (0,0) because it's base is frame buffer window
        memcpy(&stXC_SetWin_Info.stCropWin, ptCropWin, sizeof(MS_WINDOW_TYPE));
        SCAIO_ASSERT(stXC_SetWin_Info.stCropWin.x == 0);
        SCAIO_ASSERT(stXC_SetWin_Info.stCropWin.y == 0);
        SCA_DBG(SCA_DBG_LV_PROGWIN, "pCropWin x:%d, y:%d, H:%d, W:%d \n",
                stXC_SetWin_Info.stCropWin.x, stXC_SetWin_Info.stCropWin.y,
                stXC_SetWin_Info.stCropWin.width, stXC_SetWin_Info.stCropWin.height);

    }
    else
    {
        stXC_SetWin_Info.stCropWin.x = 0;
        stXC_SetWin_Info.stCropWin.y = 0;
        stXC_SetWin_Info.stCropWin.width = stXC_SetWin_Info.stCapWin.width;
        stXC_SetWin_Info.stCropWin.height= stXC_SetWin_Info.stCapWin.height;
    }

    if (ptDstWin)
    {
        // XC concept: disp window must start from (0,0) because it's base is display and which is aligned to UI, all start from (0,0)
        memcpy(&stXC_SetWin_Info.stDispWin, ptDstWin, sizeof(MS_WINDOW_TYPE));
        SCA_DBG(SCA_DBG_LV_PROGWIN, "ptDstWin x:%d, y:%d, W:%d, H:%d \n",
            stXC_SetWin_Info.stDispWin.x, stXC_SetWin_Info.stDispWin.y,
            stXC_SetWin_Info.stDispWin.width, stXC_SetWin_Info.stDispWin.height);
    }
    else
    {
        stXC_SetWin_Info.stDispWin.x = 0;
        stXC_SetWin_Info.stDispWin.y = 0;
        stXC_SetWin_Info.stDispWin.width = (MApi_GetProjPaneltype()->m_wPanelWidth);
        stXC_SetWin_Info.stDispWin.height = (MApi_GetProjPaneltype()->m_wPanelHeight);
        SCA_DBG(SCA_DBG_LV_PROGWIN, "Disp Win is Panel size \n");
    }

    stXC_SetWin_Info.bHCusScaling = stXC_SetWin_Info.bVCusScaling = FALSE;


	// JoshChiang++, for VGA vertical scaling //
	if ( INPUT_SOURCE_VGA==enInputSourceType )
    {
		if ( PNL_HEIGHT <= stXC_SetWin_Info.stCapWin.height )
		{
			SCA_DBG(SCA_DBG_LV_PROGWIN, "[SetWindow] Use customer V pre-scaling ...\n");
			stXC_SetWin_Info.bPreVCusScaling      = TRUE;
			stXC_SetWin_Info.u16PreVCusScalingSrc = stXC_SetWin_Info.stCapWin.height;
			stXC_SetWin_Info.u16PreVCusScalingDst = stXC_SetWin_Info.stDispWin.height;
		}
	}

    _MApi_Scaler_CalculateOverScan(enInputSourceType, &stXC_SetWin_Info.stCropWin, eWindow);

    _MApi_AlignWindow(&stXC_SetWin_Info.stCropWin, u16AlignX, u16AlignY);
    _MApi_check_crop_win( &stXC_SetWin_Info );

	//!! set HD mode or not, 20120726. !!//
	MApp_Scaler_CheckHDMode(eWindow);

    if(enInputSourceType == INPUT_SOURCE_SC0_VOP && eWindow == SC1_MAIN_WINDOW)
    {
        MApi_XC_EnableForceRGBin(TRUE, SC1_MAIN_WINDOW);
    }
    else
    {
        MApi_XC_EnableForceRGBin(FALSE, eWindow);
    }

    if(MApi_XC_SetWindow(&stXC_SetWin_Info, sizeof(XC_SETWIN_INFO), eWindow) == FALSE)
    {
        SCA_DBG(SCA_DBG_LV_0, "MApi_XC_SetWindow failed because of InitData wrong, please update header file and compile again\n");
        return FALSE;
    }

    return TRUE;
}


void MApi_Scaler_SetTiming(INPUT_SOURCE_TYPE_t enInputSourceType, SCALER_WIN eWindow,  MS_TIMING_CONFIG *pstTimingCfg)
{
    XC_SetTiming_Info stTimingInfo;
    MVOP_Timing stMVOPTiming;

    if(eWindow != MAIN_WINDOW && eWindow != SUB_WINDOW)
    {
        SCA_DBG(SCA_DBG_LV_PROGWIN, "MApi_Scaler_SetTiming only for MAIN_WINDOW: %x !!!!! \n", eWindow);
        return;
    }

    stTimingInfo.bFastFrameLock = FALSE;

    if( IsSrcTypeAnalog(enInputSourceType) || IsSrcTypeHDMI(enInputSourceType))
    {
        stTimingInfo.u16InputVTotal = MApi_XC_PCMonitor_Get_Vtotal(eWindow);
        stTimingInfo.bInterlace     = MApi_XC_PCMonitor_GetSyncStatus(eWindow) & XC_MD_INTERLACE_BIT ? TRUE : FALSE;
        stTimingInfo.u16InputVFreq  = MApi_XC_PCMonitor_Get_VFreqx10(eWindow);
        stTimingInfo.bMVOPSrc       = 0;

    }
    else if( IsSrcTypeATV(enInputSourceType) || IsSrcTypeDigitalVD(enInputSourceType) )
    {
        EN_VD_SIGNALTYPE enVideoSystem = mvideo_vd_get_videosystem();
        MS_U16 u16InputHFreq;

        if(enVideoSystem == SIG_NONE)
        {
            enVideoSystem = SIG_PAL;
        }

        u16InputHFreq = MApi_XC_CalculateHFreqx10(msAPI_Scaler_VD_GetHPeriod(eWindow, enInputSourceType, GET_SYNC_VIRTUAL));
        stTimingInfo.u16InputVTotal = msAPI_Scaler_VD_GetVTotal(eWindow, enInputSourceType, GET_SYNC_VIRTUAL, u16InputHFreq);
        stTimingInfo.bInterlace     = TRUE;
        stTimingInfo.u16InputVFreq  = MApi_XC_CalculateVFreqx10(u16InputHFreq, stTimingInfo.u16InputVTotal)*2;
        stTimingInfo.bMVOPSrc       = FALSE;

#if 0
        if(msAPI_AVD_GetForceVideoStandardFlag())
        {
            MSG(printf("Force Video Standard!\n");)
            if(stTimingInfo.u16InputVTotal > 566)
            {
                MSG(printf("signal in fact is PAL!\n");)
                enVideoSystem = SIG_PAL;
            }
            else
            {
                MSG(printf("signal in fact is NTSC!\n");)
                enVideoSystem = SIG_NTSC;
            }
        }
#endif

        switch ( enVideoSystem )
        {
        case SIG_NTSC:
        case SIG_NTSC_443:
        case SIG_PAL_M:
            if((stTimingInfo.u16InputVFreq > 610) || (stTimingInfo.u16InputVFreq < 590))
            {
                stTimingInfo.u16InputVFreq = 600;
            }
            break;

        default:
        case SIG_PAL:
        case SIG_SECAM:
        case SIG_PAL_NC:
            if((stTimingInfo.u16InputVFreq > 510) || (stTimingInfo.u16InputVFreq < 490) )
            {
                stTimingInfo.u16InputVFreq = 500;
            }
            break;
        }
    }
    else if(IsSrcTypeCamera(enInputSourceType))
    {
        stTimingInfo.u16InputVTotal = gstCameraTimingCfg.u16Vtotal;
        stTimingInfo.bInterlace = gstCameraTimingCfg.bInterlace;
        stTimingInfo.u16InputVFreq = gstCameraTimingCfg.u16VFreq;
        stTimingInfo.bMVOPSrc = FALSE;

    }
    else
    {
        MDrv_MVOP_GetOutputTiming(&stMVOPTiming);
        stTimingInfo.u16InputVTotal = stMVOPTiming.u16V_TotalCount;
        stTimingInfo.bInterlace     = stMVOPTiming.bInterlace;
        if(stTimingInfo.bInterlace)
        {
            stTimingInfo.u16InputVFreq = (MS_U16)((stMVOPTiming.u16ExpFrameRate * 2 + 50) / 100);
        }
        else
        {
            stTimingInfo.u16InputVFreq = (MS_U16)((stMVOPTiming.u16ExpFrameRate + 50) / 100);
        }

        stTimingInfo.bMVOPSrc = TRUE;
    }

    if(pstTimingCfg)
    {
        stTimingInfo.bInterlace = pstTimingCfg->bInterlace;
        stTimingInfo.u16InputVFreq = pstTimingCfg->u16InputVFreq;
        stTimingInfo.u16InputVTotal = pstTimingCfg->u16InputVTotal;
    }


    if((eWindow == MAIN_WINDOW  && MDrv_PQ_Get_DisplayType() == PQ_DISPLAY_ONE )||
       (eWindow == SUB_WINDOW && MDrv_PQ_Get_DisplayType() == PQ_DISPLAY_PIP))
    {
        SCA_DBG(SCA_DBG_LV_PROGWIN, "Set Timing: eWin:%d Vtt=%d, Interlace=%d, InputVFreq=%d \n",
                        eWindow,
                        stTimingInfo.u16InputVTotal,
                        stTimingInfo.bInterlace,
                        stTimingInfo.u16InputVFreq);

        MApi_XC_SetPanelTiming(&stTimingInfo, eWindow);
    }
}

void MApi_Set_Window_And_Timing(INPUT_SOURCE_TYPE_t enInputSourceType, SCALER_WIN eWindow)
{
    MApi_Scaler_SetWindow(NULL, NULL, NULL, enInputSourceType, eWindow);
    MApi_Scaler_SetTiming(enInputSourceType, eWindow, NULL);

}

void MApi_InputSrouce_GetCameraTiming(MS_INPUT_TIMING_CONFIG *pTiming)
{
    memcpy(pTiming, &gstCameraTimingCfg, sizeof(MS_INPUT_TIMING_CONFIG));
}

void MApi_InputSource_SetCameraTiming(MS_INPUT_TIMING_CONFIG stTiming)
{
    memcpy(&gstCameraTimingCfg, &stTiming, sizeof(MS_INPUT_TIMING_CONFIG));

    SCA_DBG(SCA_DBG_LV_PROGWIN, "CameraTiming: (%d %d %d %d),Vtt=%d, Vfreq=%d, bInterlace=%d\n",
        gstCameraTimingCfg.u16x, gstCameraTimingCfg.u16y, gstCameraTimingCfg.u16Width, gstCameraTimingCfg.u16Height,
        gstCameraTimingCfg.u16Vtotal, gstCameraTimingCfg.u16VFreq, gstCameraTimingCfg.bInterlace);
}

void MApi_SetSourceType(INPUT_SOURCE_TYPE_t enInputSourceType, SCALER_WIN eWindow)
{

	//RETAILMSG(TRUE,(TEXT("[Set source type] INPUT: %u, WIN: %u \n"), enInputSourceType, eWindow  ) );

    //XC_INITMISC sXC_Init_Misc;
#if 0 //ToDo!!!
    //TODO: need to move gpio settings before scaler LIB
    #if (INPUT_SCART_VIDEO_COUNT > 0)
    if ( IsSrcTypeScart(SYS_INPUT_SOURCE_TYPE(eWindow)) )
        SwitchRGBToSCART();
    else
    #endif
        SwitchRGBToDSUB();
#endif
    // Template of ADC set input source
    //MApi_XC_ADC_SetInputSource( _InputSource, _SourceCount);

    // set input port
    MApi_XC_SetInputSource( enInputSourceType, eWindow );

    //MsOS_DelayTask(50);


	if ( SUB_WINDOW==eWindow &&
		 INPUT_SOURCE_VGA==enInputSourceType )
	{
		 MApi_XC_EnableForceRGBin(TRUE, SUB_WINDOW);
		 SCA_DBG(SCA_DBG_LV_INPUTSRC, "Force VGA-Sub RGB in !!!\n");
	}
	else
	{
		MApi_XC_EnableForceRGBin(FALSE, SUB_WINDOW);
	}


#if 0 //ToDo!!!
    #if (SUPPORT_EURO_HDTV)
    if(IsSrcTypeYPbPr(enInputSourceType))
    {
        MApi_XC_EnableEuroHdtvDetection(ENABLE, eWindow);
    }
    else
    {
        MApi_XC_EnableEuroHdtvDetection(DISABLE, eWindow);
    }
    #endif

#if (INPUT_HDMI_VIDEO_COUNT > 0)

    MApi_XC_GetMISCStatus(&sXC_Init_Misc);

    if ((sXC_Init_Misc.u32MISC_A & E_XC_INIT_MISC_A_IMMESWITCH) == 0)
    {
        // printf("\n g_HdcpPollingStatus.bIsPullHighHPD=%bx", g_HdcpPollingStatus.bIsPullHighHPD);
        if ( IsSrcTypeHDMI(enInputSourceType) || IsSrcTypeDVI(enInputSourceType) )
        {
            MApi_XC_Sys_Do_HPD(enInputSourceType);
        }
    }
#endif
#endif

}

void MApi_Set_Window_OnOFF(MS_BOOL bMainEn, MS_BOOL bSubEn)
{
    PQ_DISPLAY_TYPE enDisplaType = PQ_DISPLAY_ONE;

#if 0 // we don't maintain PIP/POP PQ Table
    if(bMainEn && bSubEn)
    {
        enDisplaType = PQ_DISPLAY_PIP;
    }
    else
#endif
    {
        enDisplaType = PQ_DISPLAY_ONE;
    }

    MDrv_PQ_Set_DisplayType((MApi_GetProjPaneltype()->m_wPanelWidth), enDisplaType, PQ_MAIN_WINDOW);
    MDrv_PQ_Set_DisplayType((MApi_GetProjPaneltype()->m_wPanelWidth), enDisplaType, PQ_SUB_WINDOW);
    MApi_XC_EnableWindow(bMainEn, MAIN_WINDOW);
    MApi_XC_EnableWindow(bSubEn, SUB_WINDOW);
}

void MApi_SetColor( INPUT_SOURCE_TYPE_t enInputSourceType, SCALER_WIN eWindow )
{
    PQ_WIN ePQWin;
    //PQ_DISPLAY_TYPE enDisplaType = PQ_DISPLAY_ONE;
    T_MS_COLOR_TEMP stColorTemp;
    MS_U8 u8Brightness, u8Contrast;
    MS_U8 u8SubBrightness, u8SubContrast;
    MS_BOOL bYUVSpace;
    MS_COLOR_TRANS_TYPE enColorTrans;
    MS_U8 u8Hue, u8Saturation;
    MS_U8 u8R, u8B, u8G;

    switch(eWindow)
    {
    case MAIN_WINDOW:
        ePQWin = PQ_MAIN_WINDOW;
        break;
    case SUB_WINDOW:
        ePQWin = PQ_SUB_WINDOW;
        break;

    case SC1_MAIN_WINDOW:
        ePQWin = PQ_SC1_MAIN_WINDOW;
        break;
    case SC2_MAIN_WINDOW:
        ePQWin = PQ_SC2_MAIN_WINDOW;
        break;
    case SC2_SUB_WINDOW:
        ePQWin = PQ_SC2_SUB_WINDOW;
        break;

    default:
        SCA_DBG(SCA_DBG_LV_0, "MApi_SetColor: eWindow is not correct !!!!! \n");
        return;
        break;
    }


    if(ePQWin == PQ_SC1_MAIN_WINDOW)
    {
        if(MApi_Get_SC1_LoadPQ() == 0)
        {
            MDrv_BW_LoadTableByContext(ePQWin);
            MDrv_PQ_LoadSettings(ePQWin);
            MApi_Set_SC1_LoadPQ(1);
        }
    }
    else
    {
        MDrv_BW_LoadTableByContext(ePQWin);
        MDrv_PQ_LoadSettings(ePQWin);
    }

#if 0  //ToDo!!!
    if(MDrv_PQ_Get_PointToPoint(PQ_MAIN_WINDOW))
    {
        MDrv_PQ_LoadPTPTable(PQ_MAIN_WINDOW, PQ_PTP_PTP);
    }


#if ENABLE_DBC
    if(eWindow == MAIN_WINDOW)
    {
        MApi_XC_Sys_DLC_DBC_YCGainInit();
    }
#else
    #if defined(SCA_ENABLE_DLC)
        if(eWindow == MAIN_WINDOW)
        {
            MApi_XC_DLC_CGC_Init();
        }
    #endif
#endif

    if(IsSrcTypeATV(enInputSourceType))
    {
        MApi_XC_ACE_DMS(MAIN_WINDOW, TRUE) ;
    }
    else
    {
        MApi_XC_ACE_DMS(MAIN_WINDOW, FALSE) ;
    }

    #if defined(SCA_ENABLE_DLC)
    if( MApi_XC_DLC_GetLumaCurveStatus())
    {
        MApi_XC_DLC_SetOnOff(ENABLE, MAIN_WINDOW);
        g_bEnableDLC = TRUE;
    }
    else
    {
        g_bEnableDLC = FALSE;
    }
    #endif
    #if (ENABLE_DBC)
    if( MApi_XC_DLC_GetLumaCurveStatus())
    {
        MApi_XC_Sys_DLC_DBC_OnOff(ENABLE);
    }
    else
    {
        MApi_XC_Sys_DLC_DBC_OnOff(DISABLE);
    }
    #endif
#endif
    // The color space of default path, show on panel, is RGB
    // If the source is YUV then we need to use 3x3 matrix to translate it to RGB.

    bYUVSpace = MApi_XC_IsYUVSpace(eWindow);
    SCA_DBG(SCA_DBG_LV_COLOR, "Win:%d, Output <%s -> %s> \n",
            eWindow,
            bYUVSpace ? "YUV" : "RGB",
            (MApi_XC_Get_OutputColor(eWindow) == E_XC_OUTPUT_COLOR_YUV) ? "YUV" : "RGB");

    // MApi_XC_IsYUVSpace: the color space before 3x3
    if(eWindow == MAIN_WINDOW || eWindow == SUB_WINDOW)
    {

        enColorTrans = bYUVSpace ? MS_COLOR_YUV_TO_RGB : MS_COLOR_RGB_TO_RGB;
    }
    else
    {
        if(MApi_XC_Get_OutputColor(eWindow) == E_XC_OUTPUT_COLOR_YUV)
        {
            enColorTrans = bYUVSpace ? MS_COLOR_YUV_TO_YUV : MS_COLOR_RGB_TO_YUV;
        }
        else
        {
            enColorTrans = bYUVSpace ? MS_COLOR_YUV_TO_RGB : MS_COLOR_RGB_TO_RGB;
        }


    #if PQ_ENABLE_COLOR
        if(eWindow == SC1_MAIN_WINDOW)
        {
            MApi_XC_ACE_ColorCorrectionTable(eWindow, (MS_S16 *)S16SC1_ColorCorrectMatrix);
            MApi_XC_ACE_PCsRGBTable(eWindow, (MS_S16 *)S16SC1_ColorCorrectMatrix);
        }
        else if(eWindow == SC2_MAIN_WINDOW)
        {
            MApi_XC_ACE_ColorCorrectionTable(eWindow, (MS_S16 *)S16SC2_ColorCorrectMatrix);
            MApi_XC_ACE_PCsRGBTable(eWindow, (MS_S16 *)S16SC2_ColorCorrectMatrix);
        }
    #else
        MApi_XC_ACE_ColorCorrectionTable(eWindow, (MS_S16 *)S16ByPassColorCorrectionMatrix);
        MApi_XC_ACE_PCsRGBTable(eWindow, (MS_S16 *)S16ByPassDefaultRGB);
    #endif
    }

    if(enColorTrans == MS_COLOR_YUV_TO_RGB ||  enColorTrans == MS_COLOR_YUV_TO_YUV) // do YUV -> RGB
    {
        if(enColorTrans == MS_COLOR_YUV_TO_YUV)
        {
            MApi_XC_ACE_SetPCYUV2RGB( eWindow, FALSE);
            MApi_XC_ACE_SelectYUVtoRGBMatrix( eWindow, E_XC_ACE_YUV_TO_RGB_MATRIX_USER, (MS_S16 *)S16ByPassDefaultRGB);
        }
        else
        {
            MApi_XC_ACE_SetPCYUV2RGB( eWindow, TRUE);
            MApi_XC_ACE_SelectYUVtoRGBMatrix( eWindow, E_XC_ACE_YUV_TO_RGB_MATRIX_SDTV, NULL);
        }

        // Hue
        u8Hue = MApi_PictureHueN100toReallyValue( eWindow, enInputSourceType,  MApi_GetPictureSetting(enInputSourceType, PICTURE_HUE, eWindow));
        SCA_DBG(SCA_DBG_LV_COLOR_MATRIX, "win:%d, Hue:%d \n", eWindow, u8Hue);
        MApi_XC_ACE_PicSetHue( eWindow, bYUVSpace, u8Hue);

        // Saturation
        u8Saturation = MApi_PictureSaturationN100toReallyValue( eWindow, enInputSourceType, MApi_GetPictureSetting(enInputSourceType, PICTURE_SATURATION, eWindow));
        SCA_DBG(SCA_DBG_LV_COLOR_MATRIX, "win:%d, Saturation:%d \n", eWindow, u8Saturation);
        MApi_XC_ACE_PicSetSaturation( eWindow, bYUVSpace, u8Saturation);


    }
    else if(enColorTrans == MS_COLOR_RGB_TO_RGB)// don't YUV -> RGB, need bypass
    {
        MApi_XC_ACE_SetPCYUV2RGB( eWindow, FALSE);
    }
    else // MS_COLOR_RGB_2_YUV
    {

    }

    if(0)//eWindow == SUB_WINDOW || eWindow == SC2_SUB_WINDOW)
    {
        return;
    }
    else
    {

        MApi_XC_ACE_PicSetSharpness( eWindow, MApi_PictureSharpnessN100toReallyValue( eWindow, enInputSourceType, MApi_GetPictureSetting(enInputSourceType, PICTURE_SHARPNESS, eWindow)) );


        u8Brightness = MApi_GetPictureSetting(enInputSourceType, PICTURE_BRIGHTNESS, eWindow);
        u8SubBrightness = MApi_GetSubColor(enInputSourceType, SUB_COLOR_BRIGHTNESS, eWindow);

        u8R = MApi_ACE_transfer_Bri((MApi_FactoryAdjBrightness(MApi_PictureBrightnessN100toReallyValue(eWindow, enInputSourceType, u8Brightness), u8SubBrightness)), BRIGHTNESS_R);
        u8G = MApi_ACE_transfer_Bri((MApi_FactoryAdjBrightness(MApi_PictureBrightnessN100toReallyValue(eWindow, enInputSourceType, u8Brightness), u8SubBrightness)), BRIGHTNESS_G);
        u8B = MApi_ACE_transfer_Bri((MApi_FactoryAdjBrightness(MApi_PictureBrightnessN100toReallyValue(eWindow, enInputSourceType, u8Brightness), u8SubBrightness)), BRIGHTNESS_B);
        SCA_DBG(SCA_DBG_LV_COLOR_MATRIX, "Win:%d, brightness:%d %d %d \n", eWindow, u8R, u8G, u8B);
        MApi_XC_ACE_PicSetBrightnessInVsync(eWindow, u8R, u8G, u8B);

    #if PQ_ENABLE_COLOR
        if( !MApi_GetColorTempTBL_From_PQ(eWindow, enInputSourceType, &stColorTemp))
        {
            stColorTemp = MApi_GetColorTempTBL(enInputSourceType, eWindow);
        }
    #else
        stColorTemp = MApi_GetColorTempTBL(enInputSourceType, eWindow);
    #endif

        MApi_XC_ACE_PicSetColorTemp( eWindow, bYUVSpace, (XC_ACE_color_temp *) &stColorTemp );


        MApi_XC_ACE_PicSetBrightnessInVsync(eWindow,
                                            MApi_ACE_transferRGB_Bri(stColorTemp.cRedOffset, BRIGHTNESS_R ),
                                            MApi_ACE_transferRGB_Bri(stColorTemp.cGreenOffset, BRIGHTNESS_G),
                                            MApi_ACE_transferRGB_Bri(stColorTemp.cBlueOffset, BRIGHTNESS_B));


        u8Contrast = MApi_GetPictureSetting(enInputSourceType, PICTURE_CONTRAST, eWindow);
        u8SubContrast = MApi_GetSubColor(enInputSourceType, SUB_COLOR_CONTRAST, eWindow);

        if(enColorTrans == MS_COLOR_YUV_TO_YUV)
        {
            MS_U8 u8val;
            u8val = MApi_FactoryContrast(MApi_PictureContrastN100toReallyValue(eWindow, enInputSourceType, u8Contrast),u8SubContrast);
            SCA_DBG(SCA_DBG_LV_COLOR_MATRIX, "Win:%d, Contrast:%d  \n", eWindow, u8val);
            SCA_DBG(SCA_DBG_LV_COLOR_MATRIX, "Only apply on Y Contrast \n");
            MApi_XC_ACE_PicSetContrast_G(eWindow, bYUVSpace, u8val);
        }
        else
        {
            MS_U8 u8val;
            u8val = MApi_FactoryContrast(MApi_PictureContrastN100toReallyValue(eWindow, enInputSourceType, u8Contrast),u8SubContrast);
            SCA_DBG(SCA_DBG_LV_COLOR_MATRIX, "Win:%d, Contrast: %d  \n", eWindow, u8val);
            MApi_XC_ACE_PicSetContrast(eWindow, bYUVSpace, u8val);
        }
    }


#if defined(SCA_ENABLE_DLC)

    if(eWindow == MAIN_WINDOW)
    {
        MApi_XC_DLC_CGC_Init();

        if( MApi_XC_DLC_GetLumaCurveStatus())
        {
            SCA_DBG(SCA_DBG_LV_DLC, "DLC GetLumaCruve OK \n");

            if(g_stDLCMonitorCfg.bUserControl)
            {
                if(g_stDLCMonitorCfg.bUserOnOff)
                {
                    SCA_DBG(SCA_DBG_LV_DLC, "Enable DLC by UserControl\n");
                    g_stDLCMonitorCfg.bEn = TRUE;
                    MApi_XC_DLC_SetOnOff(ENABLE, MAIN_WINDOW);
                }
                else
                {
                    SCA_DBG(SCA_DBG_LV_DLC, "DISABLE DLC by UserControl\n");
                    g_stDLCMonitorCfg.bEn = FALSE;
                    MApi_XC_DLC_SetOnOff(DISABLE, MAIN_WINDOW);
                }
            }
            else
            {
                if (!g_stDLCMonitorCfg.bEn)
                {
                    SCA_DBG(SCA_DBG_LV_DLC, "Enable DLC \n");
                    g_stDLCMonitorCfg.bEn = TRUE;
                    MApi_XC_DLC_SetOnOff(ENABLE, MAIN_WINDOW);
                }
            }
        }
        else
        {
            SCA_DBG(SCA_DBG_LV_DLC, "DLC GetLumaCruve Fail \n");;
            g_stDLCMonitorCfg.bEn = FALSE;
            MApi_XC_DLC_SetOnOff(DISABLE, MAIN_WINDOW);
        }

    }

#endif
}

void MApi_VE_SourceSwitch(INPUT_SOURCE_TYPE_t src, E_DEST_TYPE enOutputType)
{
}

MS_BOOL MApi_SetVE(INPUT_SOURCE_TYPE_t src,E_DEST_TYPE enOutputType, MS_VE_VIDEOSYS enVideoSys, MS_VE_Set_Mode_Type SetModeType)
{
    XC_TGEN_INFO_t stTGenInfo;

    MDrv_VE_SetOutputVideoStd(enVideoSys);
    switch(enVideoSys)
    {
    default:
    case MS_VE_NTSC:
        stTGenInfo.enTimeType = E_XC_TGEN_VE_480_I_60;

        break;

    case MS_VE_PAL:
        stTGenInfo.enTimeType = E_XC_TGEN_VE_576_I_50;
        break;
    }

    MApi_XC_Set_TGen(stTGenInfo, SC1_MAIN_WINDOW);

    return TRUE;
}

void MApi_SetCVBSMute( MS_BOOL bEn , E_VE_MUTE_STATUS eMuteStatus , INPUT_SOURCE_TYPE_t enInputPortType,E_DEST_TYPE OutputPortType)
{
}


void MApi_Set_MVOP(void)
{
    MVOP_InputCfg dc_param;
    MVOP_VidStat videoSetting;

    SCA_DBG(SCA_DBG_LV_0, "MDrv_MVOP_Init \n");
    MDrv_MVOP_Init();

    SCA_DBG(SCA_DBG_LV_0, "MDrv_MVOP_EnableBlackBG \n");
    MDrv_MVOP_EnableBlackBG();

    SCA_DBG(SCA_DBG_LV_0, "MDrv_MVOP_Enable \n");
    MDrv_MVOP_Enable(FALSE);

    dc_param.u16HSize = 720;
    dc_param.u16VSize = 480;
    dc_param.u32YOffset = 0xFEC4000;
    dc_param.u32UVOffset = 0xFFC4000;
    dc_param.bSD = 1;
    dc_param.bYUV422 = 1;
    dc_param.bProgressive = 1;
    dc_param.bUV7bit = 0;
    dc_param.bDramRdContd = 1;
    dc_param.bField = 0;
    dc_param.b422pack = 1;
    dc_param.u16StripSize = dc_param.u16HSize;

    SCA_DBG(SCA_DBG_LV_0, "MDrv_MVOP_SetInputCfg \n");

    MDrv_MVOP_SetInputCfg(MVOP_INPUT_DRAM, &dc_param);

    memset(&videoSetting, 0, sizeof(MVOP_VidStat));
    videoSetting.u16HorSize = dc_param.u16HSize;
    videoSetting.u16VerSize = dc_param.u16VSize;
    videoSetting.u16FrameRate = 30000;  // frame rate to 60Hz
    videoSetting.u8AspectRate = 2; //ASP_4TO3;//ASP_16TO9;
    videoSetting.u8Interlace = 0;

    SCA_DBG(SCA_DBG_LV_0, "MDrv_MVOP_SetOutputCfg \n");

    MDrv_MVOP_SetOutputCfg(&videoSetting, FALSE);

    SCA_DBG(SCA_DBG_LV_0, "MDrv_MVOP_MiuSwitch \n");

    MDrv_MVOP_MiuSwitch(0);

    SCA_DBG(SCA_DBG_LV_0, "MDrv_MVOP_Enable \n");

    MDrv_MVOP_Enable(TRUE);

    SCA_DBG(SCA_DBG_LV_0, "------------------- \n");
}


void MApi_InputSource_Set_User_Display_Config(
    SCALER_WIN eWindow,
    INPUT_SOURCE_TYPE_t enInputPortType,
    MS_U16 u16H_Overscan, MS_U16 u16V_Overscan,
    MS_S16 s16H_Offset, MS_S16 s16V_Offset,
    MS_BOOL bCVBS_NTSC,
    MS_BOOL bEn)
{
    MS_U8 i;
    MS_U8 u8Idx = IsSrcTypeYPbPr(enInputPortType) ? MS_USER_DISPLAY_YPBPR :
                  IsSrcTypeVga(enInputPortType)   ? MS_USER_DISPLAY_VGA :
                  bCVBS_NTSC ? MS_USER_DISPLAY_CVBS_NTSC : MS_USER_DISPLAY_CVBS_PAL ;

    SCA_DBG(SCA_DBG_LV_PROGWIN, "SetOverScan: Src:%d, idx:%d, En:%d, H:%d, V:%d \n", enInputPortType, u8Idx, bEn, u16H_Overscan, u16V_Overscan);

    if(eWindow == MAX_WINDOW)
    {
        for(i=0; i<MAX_WINDOW; i++)
        {
            g_stUserDisplayCfg[i][u8Idx].bEn = bEn;
            g_stUserDisplayCfg[i][u8Idx].u16H_Overscan = u16H_Overscan;
            g_stUserDisplayCfg[i][u8Idx].u16V_Overscan = u16V_Overscan;
            g_stUserDisplayCfg[i][u8Idx].s16H_Offset = s16H_Offset;
            g_stUserDisplayCfg[i][u8Idx].s16V_Offset = s16V_Offset;
        }

    }
    else
    {
        g_stUserDisplayCfg[eWindow][u8Idx].bEn = bEn;
        g_stUserDisplayCfg[eWindow][u8Idx].u16H_Overscan = u16H_Overscan;
        g_stUserDisplayCfg[eWindow][u8Idx].u16V_Overscan = u16V_Overscan;
        g_stUserDisplayCfg[eWindow][u8Idx].s16H_Offset = s16H_Offset;
        g_stUserDisplayCfg[eWindow][u8Idx].s16V_Offset = s16V_Offset;
    }
}



void MApi_Scaler_set_DisplayMute_Color(XC_FREERUN_COLOR enColor, SCALER_WIN eWindow)
{
    g_displaymute_color[eWindow] = enColor;
}


XC_FREERUN_COLOR MApi_Scaler_Get_DisplayMute_ColorType(SCALER_WIN eWindow)
{
    return g_displaymute_color[eWindow];
}

