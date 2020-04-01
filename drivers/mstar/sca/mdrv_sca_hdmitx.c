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
#define MDRV_SCA_HDMITX_C

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
#include "apiPNL.h"
#include "apiHDMITx.h"

#include "drvMMIO.h"
#include "mdrv_sca_st.h"
#include "mdrv_sca.h"
#include "mdrv_sca_hdmitx.h"
//=============================================================================
// Local Defines
//=============================================================================
static XC_HDMITX_INITDATA g_stHDMI_InitData;
static HDMITX_ANALOG_TUNING _stXC_HDMITx_AnalogInfo;
static HDMITX_OUTPUT_MODE _eXC_HDMITx_OutputMode = HDMITX_HDMI;
static HDMITX_VIDEO_COLORDEPTH_VAL _eXC_HDMITx_CurColorDepth = HDMITX_VIDEO_CD_24Bits;

//=============================================================================
// Local Function Prototypes
//=============================================================================
HDMITX_VIDEO_TIMING MApi_Get_HDMITx_VideoTiming(E_XC_HDMITX_OUTPUT_TIMING_TYPE enHDMITxType)
{
    HDMITX_VIDEO_TIMING enTiming;

    switch(enHDMITxType)
    {
    case E_XC_HDMITX_480_60I:
        enTiming = HDMITX_RES_720x480i;
        break;

    case E_XC_HDMITX_480_60P:
        enTiming = HDMITX_RES_720x480p;
        break;

    case E_XC_HDMITX_576_50I:
        enTiming = HDMITX_RES_720x576i;
        break;

    case E_XC_HDMITX_576_50P:
        enTiming = HDMITX_RES_720x576p;
        break;

    case E_XC_HDMITX_720_50P:
        enTiming = HDMITX_RES_1280x720p_50Hz;
        break;

    case E_XC_HDMITX_720_60P:
        enTiming = HDMITX_RES_1280x720p_60Hz;
        break;

    case E_XC_HDMITX_1080_50I:
        enTiming = HDMITX_RES_1920x1080i_50Hz;
        break;

    case E_XC_HDMITX_1080_50P:
        enTiming = HDMITX_RES_1920x1080p_50Hz;
        break;

    case E_XC_HDMITX_1080_60I:
        enTiming = HDMITX_RES_1920x1080i_60Hz;
        break;

    case E_XC_HDMITX_1080_60P:
        enTiming = HDMITX_RES_1920x1080p_60Hz;
        break;

    case E_XC_HDMITX_1080_30P:
        enTiming = HDMITX_RES_1920x1080p_30Hz;
        break;

    case E_XC_HDMITX_1080_25P:
        enTiming = HDMITX_RES_1920x1080p_25Hz;
        break;

    case E_XC_HDMITX_1080_24P:
        enTiming = HDMITX_RES_1920x1080p_24Hz;
        break;

    default:
        SCA_DBG(SCA_DBG_LV_HDMITX, "MApi_Get_HDMITx_VideoTiming :: ERROR E_XC_HDMITX_OUTPUT_TIMING_TYPE \n");
        enTiming = HDMITX_RES_1280x720p_50Hz;
        break;
    }

    return enTiming;
}


XC_TGEN_TIMING_TYPE MApi_Get_TgenType(E_XC_HDMITX_OUTPUT_TIMING_TYPE enHDMITxType)
{
    XC_TGEN_TIMING_TYPE enType;


    switch(enHDMITxType)
    {
    case E_XC_HDMITX_480_60I:
        enType = E_XC_TGEN_HDMI_480_I_60;
        break;

    case E_XC_HDMITX_480_60P:
        enType = E_XC_TGEN_HDMI_480_P_60;
        break;

    case E_XC_HDMITX_576_50I:
        enType = E_XC_TGEN_HDMI_576_I_50;
        break;

    case E_XC_HDMITX_576_50P:
        enType = E_XC_TGEN_HDMI_576_P_50;
        break;

    case E_XC_HDMITX_720_50P:
        enType = E_XC_TGEN_HDMI_720_P_50;
        break;

    case E_XC_HDMITX_720_60P:
        enType = E_XC_TGEN_HDMI_720_P_60;
        break;

    case E_XC_HDMITX_1080_50I:
        enType = E_XC_TGEN_HDMI_1080_I_50;
        break;

    case E_XC_HDMITX_1080_50P:
        enType = E_XC_TGEN_HDMI_1080_P_50;
        break;

    case E_XC_HDMITX_1080_60I:
        enType = E_XC_TGEN_HDMI_1080_I_60;
        break;

    case E_XC_HDMITX_1080_60P:
        enType = E_XC_TGEN_HDMI_1080_P_60;
        break;

    case E_XC_HDMITX_1080_30P:
        enType = E_XC_TGEN_HDMI_1080_P_30;
        break;

    case E_XC_HDMITX_1080_25P:
        enType = E_XC_TGEN_HDMI_1080_P_25;
        break;

    case E_XC_HDMITX_1080_24P:
        enType = E_XC_TGEN_HDMI_1080_P_24;
        break;

    default:
        SCA_DBG(SCA_DBG_LV_HDMITX, "MApi_Get_TgenType :: ERROR E_XC_HDMITX_OUTPUT_TIMING_TYPE \n");
        enType = E_XC_TGEN_HDMI_720_P_50;
        break;
    }
    return enType;
}



MS_BOOL MApi_Set_HDMITxMode(E_XC_HDMITX_OUTPUT_MODE eType)
{
    g_stHDMI_InitData.eHdmitxOutputMode = eType;

    switch(eType)
    {
    case E_XC_HDMITX_OUTPUT_HDMI:
        _eXC_HDMITx_OutputMode = HDMITX_HDMI;
        break;

    case E_XC_HDMITX_OUTPUT_HDMI_HDCP:
        _eXC_HDMITx_OutputMode = HDMITX_HDMI_HDCP;
        break;

    case E_XC_HDMITX_OUTPUT_DVI:
        _eXC_HDMITx_OutputMode = HDMITX_DVI;
        break;

    case E_XC_HDMITX_OUTPUT_DVI_HDCP:
        _eXC_HDMITx_OutputMode = HDMITX_DVI_HDCP;
        break;

    default:
        SCA_DBG(SCA_DBG_LV_HDMITX,"Wrong Mode of E_XC_HDMITX_OUTPUT_MODE \n");
        return FALSE;
    }

    SCA_DBG(SCA_DBG_LV_HDMITX,"SET HDMITx Mode to:%s \n",
            eType ==  E_XC_HDMITX_OUTPUT_HDMI ? "HDMI" :
            eType ==  E_XC_HDMITX_OUTPUT_HDMI_HDCP ? "HDMI_HDCP" :
            eType ==  E_XC_HDMITX_OUTPUT_DVI ? "DVI" :
                                                "DVI_HDCP");

    MApi_HDMITx_SetHDMITxMode(_eXC_HDMITx_OutputMode);
    return TRUE;
}


MS_BOOL MApi_Set_HDMITxCD(E_XC_HDMITX_CD_TYPE eType)
{
    HDMITX_VIDEO_COLORDEPTH_VAL stXC_HDMITx_EDIDColorDepth = HDMITX_VIDEO_CD_NoID;
    XC_TGEN_INFO_t stTgenInfo;

    g_stHDMI_InitData.eHdmitxCDType = eType;

    if(!MApi_HDMITx_GetRxDCInfoFromEDID(&stXC_HDMITx_EDIDColorDepth))
    {
        // Need to check EDID
        if(MApi_HDMITx_EdidChecking())
        {
            MApi_HDMITx_GetRxDCInfoFromEDID(&stXC_HDMITx_EDIDColorDepth);
            SCA_DBG(SCA_DBG_LV_HDMITX, "GetRxDCInfoFromEDID: stXC_HDMITx_EDIDColorDepth = %d \n", stXC_HDMITx_EDIDColorDepth);
        }
        else
        {
            SCA_DBG(SCA_DBG_LV_HDMITX, "EDID checking failed!!\n");
        }
    }

    if(stXC_HDMITx_EDIDColorDepth <= HDMITX_VIDEO_CD_24Bits)
    {
        _eXC_HDMITx_CurColorDepth = HDMITX_VIDEO_CD_NoID;
    }
    else
    {
        // Support deep color, EDID color depth at least 30 bits
        switch(g_stHDMI_InitData.eHdmitxCDType)
        {
        case E_XC_HDMITX_CD_8BITS:
        {
            _eXC_HDMITx_CurColorDepth = HDMITX_VIDEO_CD_24Bits;
        }
        break;
        case E_XC_HDMITX_CD_10BITS:
        {
            _eXC_HDMITx_CurColorDepth = HDMITX_VIDEO_CD_30Bits;
        }
        break;
        case E_XC_HDMITX_CD_12BITS:
        {
            if(0)//stXC_HDMITx_EDIDColorDepth == HDMITX_VIDEO_CD_36Bits)
            {
                _eXC_HDMITx_CurColorDepth = HDMITX_VIDEO_CD_36Bits;
            }
            else
            {
                _eXC_HDMITx_CurColorDepth = HDMITX_VIDEO_CD_30Bits;
            }
        }
        break;
        case E_XC_HDMITX_CD_AUTO:
        {
            _eXC_HDMITx_CurColorDepth = stXC_HDMITx_EDIDColorDepth == HDMITX_VIDEO_CD_36Bits ?
                                        HDMITX_VIDEO_CD_30Bits :
                                        stXC_HDMITx_EDIDColorDepth;
            SCA_DBG(SCA_DBG_LV_HDMITX, "Set HDMITx Color Depth to auto-mode \n");
        }
        break;
        default:
        {
            SCA_DBG(SCA_DBG_LV_HDMITX, "Wrong Mode of E_XC_HDMITX_CD_TYPE \n");
            return FALSE;
        }
        break;
        }
    }

    SCA_DBG(SCA_DBG_LV_HDMITX, "ColorDepth:Rx:%d, CurCD:%d !!\n",stXC_HDMITx_EDIDColorDepth, _eXC_HDMITx_CurColorDepth);

    stTgenInfo.enTimeType = MApi_Get_TgenType(g_stHDMI_InitData.eHdmitxOutputTiming);

    switch(_eXC_HDMITx_CurColorDepth)
    {
    case HDMITX_VIDEO_CD_NoID:
        stTgenInfo.enBits = E_XC_HDMITX_8_BITS;
        break;

    default:
    case HDMITX_VIDEO_CD_24Bits:
        stTgenInfo.enBits = E_XC_HDMITX_8_BITS;
        break;

    case HDMITX_VIDEO_CD_30Bits:
        stTgenInfo.enBits = E_XC_HDMITX_10_BITS;
        break;

    case HDMITX_VIDEO_CD_36Bits:
        SCA_DBG(SCA_DBG_LV_HDMITX, "CD_36_Bits !!!\n");
        return FALSE;
    }

    MApi_XC_Set_TGen(stTgenInfo, SC1_MAIN_WINDOW);

    MApi_HDMITx_SetHDMITxMode_CD(_eXC_HDMITx_OutputMode, _eXC_HDMITx_CurColorDepth);
    return TRUE;

}


static void _HDMITx_AnalogTuning(HDMITX_ANALOG_TUNING *pInfo)
{
    //normal usage, customizing is needed for different BOARD!!
    //For the general MSTAR 2 layer board


    switch(g_stHDMI_InitData.eHdmitxOutputTiming)
    {
    // >= 1080p@50/60Hz_8bits
    // (like 1080p@50/60Hz_8/10/12bits)
    case E_XC_HDMITX_1080_50P:
    case E_XC_HDMITX_1080_60P:
    {
        SCA_DBG(SCA_DBG_LV_HDMITX, "HDMITx output is >= 1080P50\n");
        if(_eXC_HDMITx_CurColorDepth >= HDMITX_VIDEO_CD_30Bits)
        {
            //clock > 165MHz
            memcpy(pInfo, &g_stHDMI_InitData.stHdmitxBoardInfo.stDeepHDInfo, sizeof(HDMITX_ANALOG_TUNING));
        }
        else
        {
            // clock < 165MHz
            memcpy(pInfo, &g_stHDMI_InitData.stHdmitxBoardInfo.stHDInfo, sizeof(HDMITX_ANALOG_TUNING));
        }
    }
    break;

    // < 1080p@50/60Hz_8bits
    // (like 480p, 720p, 1080p@24/25/30Hz)
    default:
    {
        // clock < 75MHz
        SCA_DBG(SCA_DBG_LV_HDMITX, "HDMITx output is < 1080P50\n");
        memcpy(pInfo, &g_stHDMI_InitData.stHdmitxBoardInfo.stSDInfo, sizeof(HDMITX_ANALOG_TUNING));
    }
    break;
    }
}

static void _MApi_ConfigurateHDMITx(void)
{
    MApi_Set_HDMITxMode(g_stHDMI_InitData.eHdmitxOutputMode);

    MApi_Set_HDMITxCD(g_stHDMI_InitData.eHdmitxCDType);

    MApi_HDMITx_SetVideoOutputTiming( MApi_Get_HDMITx_VideoTiming(g_stHDMI_InitData.eHdmitxOutputTiming));

    _HDMITx_AnalogTuning(&_stXC_HDMITx_AnalogInfo);
    MApi_HDMITx_AnalogTuning(&_stXC_HDMITx_AnalogInfo);
}


void MApi_Init_HDMITx(XC_HDMITX_INITDATA stInitData)
{
    memcpy(&g_stHDMI_InitData, &stInitData, sizeof(XC_HDMITX_INITDATA));

    MApi_HDMITx_SetHPDGpioPin(g_stHDMI_InitData.u16HdmitxHpdPin);

    MApi_HDMITx_Init();
    if ( stInitData.bDisableHDMI == TRUE )
    {
        MApi_HDMITx_TurnOnOff(FALSE);
        MApi_HDMITx_SetVideoOnOff(FALSE);
        SCA_DBG(SCA_DBG_LV_HDMITX, "Turn Off HDMITx! \n");
    }
    else
    {
        MApi_HDMITx_TurnOnOff(TRUE);
        MApi_HDMITx_SetVideoOnOff(TRUE);
        SCA_DBG(SCA_DBG_LV_HDMITX, "Turn On HDMITx! \n");
    }
    MApi_HDMITx_SetColorFormat(HDMITX_VIDEO_COLOR_YUV444, HDMITX_VIDEO_COLOR_YUV444);

    MApi_HDMITx_SetAudioOnOff(TRUE);

    MApi_HDMITx_SetAudioConfiguration(HDMITX_AUDIO_48K, HDMITX_AUDIO_CH_2, HDMITX_AUDIO_PCM);

    if(g_stHDMI_InitData.eHdmitxOutputMode == E_XC_HDMITX_OUTPUT_HDMI_HDCP ||
       g_stHDMI_InitData.eHdmitxOutputMode == E_XC_HDMITX_OUTPUT_DVI_HDCP)
    {
        MApi_HDMITx_GetHdcpKey(TRUE, NULL);
    }

    _MApi_ConfigurateHDMITx();

    MApi_HDMITx_SetTMDSOnOff(FALSE);
    SCA_DBG(SCA_DBG_LV_HDMITX, "Turn Off HDMITx TMDS! \n");

}

void MApi_Config_HDMITx(MS_BOOL bEn, E_XC_HDMITX_OUTPUT_MODE enOutputMode, E_XC_HDMITX_OUTPUT_TIMING_TYPE enOutPutTiming)
{
    g_stHDMI_InitData.bDisableHDMI = bEn ? FALSE : TRUE;
    g_stHDMI_InitData.eHdmitxOutputTiming = enOutPutTiming;
    g_stHDMI_InitData.eHdmitxOutputMode = enOutputMode;

    if ( g_stHDMI_InitData.bDisableHDMI == TRUE )
    {
        MApi_HDMITx_TurnOnOff(FALSE);
        MApi_HDMITx_SetVideoOnOff(FALSE);
        SCA_DBG(SCA_DBG_LV_HDMITX, "Turn Off HDMITx! \n");
        MApi_HDMITx_SetTMDSOnOff(FALSE);
        SCA_DBG(SCA_DBG_LV_HDMITX, "Turn Off HDMITx TMDS! \n");

    }
    else
    {
        MApi_HDMITx_TurnOnOff(TRUE);
        MApi_HDMITx_SetVideoOnOff(TRUE);
        SCA_DBG(SCA_DBG_LV_HDMITX, "Turn On HDMITx! \n");

        MApi_HDMITx_SetAudioOnOff(TRUE);

        MApi_HDMITx_SetAudioConfiguration(HDMITX_AUDIO_48K, HDMITX_AUDIO_CH_2, HDMITX_AUDIO_PCM);
        _MApi_ConfigurateHDMITx();

        MApi_HDMITx_SetTMDSOnOff(TRUE);
        SCA_DBG(SCA_DBG_LV_HDMITX, "Turn On HDMITx TMDS! \n");

        //Exhibit function is needed after video, audio setting.
        //And it could send AVMute disable packet to HDMIRx to disable Blue Screen if AVMute is set earlier.
        MApi_HDMITx_Exhibit();
    }
}

HDMITX_VIDEO_COLORDEPTH_VAL MApi_Get_HDMITx_CD(void)
{
    return _eXC_HDMITx_CurColorDepth;
}
