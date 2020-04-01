////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2009 MStar Semiconductor, Inc.
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

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    apiXC_PCMonitor.c
/// @brief  Scaler API layer Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _API_XC_PC_MONITOR_C_
#define _API_XC_PC_MONITOR_C_

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
// Common Definition
#define PCMSG(x)    //x

// Common
#ifdef MSOS_TYPE_LINUX_KERNEL
#include <linux/string.h>
#include <linux/kernel.h>
#else
#include <string.h>
#endif
#include "MsCommon.h"

// Driver
#include "drvXC_IOPort.h"
#include "apiXC.h"
#include "drvXC_HDMI_if.h"
#include "mvideo_context.h"
#include "drv_sc_ip.h"
#include "apiXC_Adc.h"
#include "drvXC_ADC_Internal.h"
#include "drv_sc_scaling.h"
// API
#include "apiXC_PCMonitor.h"

//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

#define PC_POLLING_COUNTER          30  // (30/TIMER_PERIOD), unit ms

// mode change torlance for timing monitor, also used in auto procedure
#define MD_HPERIOD_TORLANCE         10      // horizontal period torlance
#define MD_VTOTAL_TORLANCE          10      // vertical total torlance
#define MD_HDE_TORLANCE             5
#define MD_VDE_TORLANCE             5

#if 1//(INPUT_HDMI_VIDEO_COUNT > 0)
  #if 1
    // Change these count value just for HDMI fast to recover video,
    // But have to delay 200mSec to reduce flash blue screen when plug-in/out HDMI fast,
    #define MD_TIMING_STABLE_COUNT    ((IsSrcTypeHDMI(psPCMONITOR_HK_Info[eWindow]->eCurrentSrc)?25:25))
    #define MD_TIMING_NOSYNC_COUNT    ((IsSrcTypeHDMI(psPCMONITOR_HK_Info[eWindow]->eCurrentSrc)?40:40))
  #else
    #define MD_TIMING_STABLE_COUNT    (((IsSrcTypeHDMI(psPCMONITOR_HK_Info[eWindow]->eCurrentSrc)?120:25))
    #define MD_TIMING_NOSYNC_COUNT    (((IsSrcTypeHDMI(psPCMONITOR_HK_Info[eWindow]->eCurrentSrc)?80:40))
  #endif
#else
    #define MD_TIMING_STABLE_COUNT    25 // input timing stable counter
    #define MD_TIMING_NOSYNC_COUNT    40
#endif

//-------------------------------------------------------------------------------------------------
//  Local Structurs
//-------------------------------------------------------------------------------------------------

typedef struct
{
    INPUT_SOURCE_TYPE_t eCurrentSrc;    ///< indicate current input source type

    XC_PCMONITOR_STATUS eCurrentState;  ///< timing monitor state

    MS_U32 u32LastExecutedTime;            ///< 30ms execute timing monitor once
    MS_BOOL bInputTimingChange;         ///< indicate input timing changed or not
    MS_U8  ucInputTimingStableCounter;     ///< input timing stable counter
    MS_U8  ucTimingChangeDebounce;         ///< debounce counter when timing un-stable
    MS_BOOL bInputTimingStable;         ///< input timing stable or not
    MS_BOOL bDoModeSetting;             ///< indicate the timing is stable (either with sync or without sync) and can do mode setting

    // Analog
    MS_U16 u16Hperiod;                     ///< Hperiod got from scaler
    MS_U16 u16VSyncTime;                   ///< vsync time calculated from Hperiod & Vtotal

    MS_U16 u16Vtotal;                      ///< Vertical total
    MS_U8  u8SyncStatus;                   ///< from XC driver, including H/V sync polarity, interlaced or progressive

    // Digital
    MS_WINDOW_TYPE sDVI_HDMI_DE_Info;   ///< DVI/HDMI input DE information

} XC_PCMONITOR_HK_INFO;

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------

// Daten FixMe, MAX_WINDOW will be removed and can be obtained from MApi_XC_GetInfo()
static XC_PCMONITOR_HK_INFO sPCMonitor_HK_Info[MAX_WINDOW];
static XC_PCMONITOR_HK_INFO *psPCMONITOR_HK_Info[MAX_WINDOW];       ///< point to sPCMonitor_HK_Info, this is for fast swap MAIN/SUB
static MS_U16 _u16TimingStableCounter = 0;
static MS_U16 _u16TimingNosyncCounter = 0;
static MS_U32 _u32PollingPeroid = 0;
static MS_U8 s_u8LastHDMIPixelRep = 0xff;

//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

//*************************************************************************
//Function name:    _MApi_XC_PCMonitor_SetSyncStatusFlag
//Passing parameter:    MS_U8 u8TempSyncStatus:    b0: VSync polarity
//                                      b1: HSync polarity
//                                      b2: Hsync active
//                                      b3: Vsync active
//Return parameter: none
//Description:          Set signal sync status flags
//*************************************************************************
static void _MApi_XC_PCMonitor_SetSyncStatusFlag (MS_U8 u8TempSyncStatus, SCALER_WIN eWindow)
{
    psPCMONITOR_HK_Info[eWindow]->u8SyncStatus = u8TempSyncStatus;
}

//*************************************************************************
//------------------------------ Warning ----------------------------------
//Attention: !!!!!!------ Don't add UI variable or unneeded code in this file
//
//           Don't modify this procedure !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//*************************************************************************
//*************************************************************************
//Function name:    _MApi_XC_PCMonitor_DetectSync
//Passing parameter:    none
//Return parameter: MS_BOOL:  true: sync loss    false: sync active
//Description:          Get signal sync status, true for sync loss, false for sync active
//*************************************************************************
static MS_BOOL _MApi_XC_PCMonitor_DetectSync (INPUT_SOURCE_TYPE_t eCurrentSrc , SCALER_WIN eWindow )
{
    PCMSG(static MS_BOOL bSyncLoss = TRUE;)

    MS_BOOL bResult = FALSE;
    XC_IP_SYNC_STATUS sXC_Sync_Status;
    MApi_XC_GetSyncStatus(eCurrentSrc, &sXC_Sync_Status, eWindow);

    if ( sXC_Sync_Status.u8SyncStatus & XC_MD_SYNC_LOSS ) // check no signal
    {
        PCMSG(if(!bSyncLoss) printf("XC_MD_SYNC_LOSS 0x%x\n", sXC_Sync_Status.u8SyncStatus); bSyncLoss = TRUE;)

#if 0//(MS_VGA_SOG_EN)   // Daten FixMe -> Please refine VGA SoG flow
        if( IsSrcTypeVga(SYS_INPUT_SOURCE_TYPE(MAIN_WINDOW)) &&
            ((g_ucInputTimingStableCounter%MD_TIMING_SOG_DETECT)==0) )
        {
            msAPI_Scaler_ProgAnalogInputPort(); // toggle analog input and detect next time
        }
#endif // #if (MS_VGA_SOG_EN)

//#if(SUPPORT_EURO_HDTV)
    #ifdef MSOS_TYPE_CE
        if (MApi_XC_Get_EuroHdtvSupport())
    #else
        if (_bEnableEuro)
    #endif

        {
            if( IsSrcTypeYPbPr(eCurrentSrc) )
            {
                MApi_XC_EuroHdtvHandler(eWindow);
            }
            else
            {
                MDrv_XC_HWAuto_SyncSeparate(eWindow);
            }
        }
//#endif
    }
    else // have signal
    {
        bResult = TRUE;

        PCMSG(if(bSyncLoss) printf("MD_SYNC_DETECT 0x%x\n", sXC_Sync_Status.u8SyncStatus); bSyncLoss = FALSE;)
    }

    if ( bResult ) // neet to update flag
    {
        _MApi_XC_PCMonitor_SetSyncStatusFlag(sXC_Sync_Status.u8SyncStatus, eWindow);
    } // set global sync status flag

    return bResult;
}

static void _MApi_XC_PCMonitor_InitRegInTimingChange(SCALER_WIN eWindow)
{
    if(IsSrcTypeHDMI(psPCMONITOR_HK_Info[eWindow]->eCurrentSrc) || IsSrcTypeDVI(psPCMONITOR_HK_Info[eWindow]->eCurrentSrc))
    {
        //ToDo HDMI
        MDrv_HDMI_audio_output(DISABLE);
    }

    if(eWindow == MAIN_WINDOW)
    {
        MApi_XC_SoftwareReset(REST_IP_F2, eWindow);
    }
    else
    {
        MApi_XC_SoftwareReset(REST_IP_F1, eWindow);
    }
// 20081226 - DVI+HDCP snow noise patch - start ...
//Note: Don't use rest function after HPD Hi/Low or it will cause snow image in a while.
//    MDrv_XC_ADC_reset(REST_DVI|REST_HDCP|REST_HDMI); // Bright20080918
// 20081226 - DVI+HDCP snow noise patch - end ...
    MApi_XC_TimingChangeInit(eWindow);

    if(IsSrcTypeHDMI(psPCMONITOR_HK_Info[eWindow]->eCurrentSrc))
        MApi_XC_VsyncPolarityDetectMode(ENABLE, eWindow);
    else
        MApi_XC_VsyncPolarityDetectMode(DISABLE, eWindow);

}

//*************************************************************************
//------------------------------ Warning ----------------------------------
//Attention: !!!!!!------ Don't add UI variable or unneeded code in this file
//
//           Don't modify this procedure !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//*************************************************************************
//*************************************************************************
//Function name:    _MApi_XC_PCMonitor_PrepareForTimingChange
//Passing parameter:    none
//Return parameter:     none
//Description:  This function will pre-set flags and registers for timing change
//*************************************************************************
static void _MApi_XC_PCMonitor_PrepareForTimingChange(SCALER_WIN eWindow)
{
    //MS_U8 u8TurnOffDestination = DISABLE;

    PCMSG(printf("PrepareForTimingChange\n"));


    if ( psPCMONITOR_HK_Info[eWindow]->bInputTimingStable )
    {
        _MApi_XC_PCMonitor_InitRegInTimingChange(eWindow);

    }

    if( IsSrcTypeYPbPr(psPCMONITOR_HK_Info[eWindow]->eCurrentSrc) )
    {
        MApi_XC_InitRegInTimingChange_Post( psPCMONITOR_HK_Info[eWindow]->u8SyncStatus );
    }

    // reset input timing stable and wait for timing stable
    psPCMONITOR_HK_Info[eWindow]->bInputTimingStable = FALSE;

    // reset timing stable counter
    psPCMONITOR_HK_Info[eWindow]->ucInputTimingStableCounter = 0;

}

static MS_BOOL _MApi_XC_PCMonitor_IsStableSourceUsed(INPUT_SOURCE_TYPE_t eTargetSrc, SCALER_WIN eTargetWin)
{
	MS_BOOL bRet = FALSE;
	MS_U32  index;

	for (index = 0; index < MAX_WINDOW; ++index)
	{
		if ( eTargetWin == index )
			continue;

		if (  ( eTargetSrc == psPCMONITOR_HK_Info[index]->eCurrentSrc) &&
			  ( E_XC_PCMONITOR_STABLE_SYNC == psPCMONITOR_HK_Info[index]->eCurrentState) )
		{
			bRet = TRUE;
			break;
		}
	}

	return bRet;
}

/*
void _MApi_XC_PCMonitor_ModeParse(SCALER_WIN eWindow)
{


    // to match mode, we need these information
    // H/V freq., Vtt
    // Sync polarity
    // Interlaced or Progrssive
    psPCMONITOR_HK_Info[eWindow]->sModeParseInputInfo.eSrcType = psPCMONITOR_HK_Info[eWindow]->eCurrentSrc;
    psPCMONITOR_HK_Info[eWindow]->sModeParseInputInfo.u16HFreqx10 = MApi_XC_CalculateHFreqx10(psPCMONITOR_HK_Info[eWindow]->u16Hperiod);
    psPCMONITOR_HK_Info[eWindow]->sModeParseInputInfo.u16VFreqx10 = MApi_XC_CalculateVFreqx10(psPCMONITOR_HK_Info[eWindow]->sModeParseInputInfo.u16HFreqx10, psPCMONITOR_HK_Info[eWindow]->u16Vtotal);

    if(psPCMONITOR_HK_Info[eWindow]->u8SyncStatus & XC_MD_INTERLACE_BIT)
    {
        psPCMONITOR_HK_Info[eWindow]->sModeParseInputInfo.u16VFreqx10 *=2;
    }

    psPCMONITOR_HK_Info[eWindow]->sModeParseInputInfo.u16Vtotal = psPCMONITOR_HK_Info[eWindow]->u16Vtotal;
    psPCMONITOR_HK_Info[eWindow]->sModeParseInputInfo.u8SyncStatus = psPCMONITOR_HK_Info[eWindow]->u8SyncStatus;
    //psPCMONITOR_HK_Info[eWindow]->sModeParseInputInfo.eEuroHDTVStatus = EURO_AUS_HDTV_NORMAL;
    memcpy(&(psPCMONITOR_HK_Info[eWindow]->sModeParseInputInfo.sDVI_HDMI_DE_Info), &(psPCMONITOR_HK_Info[eWindow]->sDVI_HDMI_DE_Info), sizeof(MS_WINDOW_TYPE));

#if (INPUT_HDMI_VIDEO_COUNT > 0)
    // search mode
    if(IsSrcTypeHDMI(psPCMONITOR_HK_Info[eWindow]->eCurrentSrc))
    {
        // check if HDMI or DVI
        MApi_XC_HDMI_CheckModeChanged(TRUE);
    }
#endif

    psPCMONITOR_HK_Info[eWindow]->eModeParseResult = MApi_XC_ModeParse_SearchMode(&(psPCMONITOR_HK_Info[eWindow]->sModeParseInputInfo));

    if(psPCMONITOR_HK_Info[eWindow]->sModeParseInputInfo.u8SyncStatus & XC_MD_USER_MODE_BIT)
    {
        // mode parse indicate this is a user mode
        psPCMONITOR_HK_Info[eWindow]->u8SyncStatus |= XC_MD_USER_MODE_BIT;
    }

    if(psPCMONITOR_HK_Info[eWindow]->eModeParseResult == XC_MODEPARSE_SUPPORT_MODE)
    {
        psPCMONITOR_HK_Info[eWindow]->eCurrentState = E_XC_PCMONITOR_STABLE_SUPPORT_MODE;
    }
    else
    {
        psPCMONITOR_HK_Info[eWindow]->eCurrentState = E_XC_PCMONITOR_STABLE_UN_SUPPORT;
    }
}
*/

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

//*************************************************************************
//Function name:    MApi_XC_PCMonitor_InvalidTimingDetect
//Passing parameter:    none
//Return parameter: MS_BOOL : timing is valid or not
//Description:          Detect invalid timing. true: timing invalid   false : timing valid
//*************************************************************************
MS_BOOL MApi_XC_PCMonitor_InvalidTimingDetect(  MS_BOOL bPollingOnly, SCALER_WIN eWindow)
{
    MS_BOOL bResult = FALSE;
    XC_IP_SYNC_STATUS sXC_Sync_Status;

    MApi_XC_GetSyncStatus(psPCMONITOR_HK_Info[eWindow]->eCurrentSrc, &sXC_Sync_Status, eWindow);

    if ( ( sXC_Sync_Status.u8SyncStatus & XC_MD_SYNC_LOSS ) ||  // no sync
        ( (psPCMONITOR_HK_Info[eWindow]->u8SyncStatus & XC_MD_VSYNC_POR_BIT) != (sXC_Sync_Status.u8SyncStatus & XC_MD_VSYNC_POR_BIT) ) ||  // sync polarity changed
        ( (psPCMONITOR_HK_Info[eWindow]->u8SyncStatus & XC_MD_HSYNC_POR_BIT) != (sXC_Sync_Status.u8SyncStatus & XC_MD_HSYNC_POR_BIT) ) ||
        ( (psPCMONITOR_HK_Info[eWindow]->u8SyncStatus & XC_MD_INTERLACE_BIT) != (sXC_Sync_Status.u8SyncStatus & XC_MD_INTERLACE_BIT) ) )
    {
        bResult = TRUE;

        PCMSG(printf("oldSyncStatus=0x%x, new=0x%x\n",
                        psPCMONITOR_HK_Info[eWindow]->u8SyncStatus, sXC_Sync_Status.u8SyncStatus));
    }
    else // have signal
    {
        if ( abs( sXC_Sync_Status.u16Hperiod - psPCMONITOR_HK_Info[eWindow]->u16Hperiod ) > MD_HPERIOD_TORLANCE )
        {
            PCMSG(printf("MD_HPERIOD_TORLANCE %d\n", (MS_U8)abs( sXC_Sync_Status.u16Hperiod - psPCMONITOR_HK_Info[eWindow]->u16Hperiod )   ));
            bResult = TRUE;
        }
        else
        {
            //PCMSG(printf("Hperiod %x\n", sXC_Sync_Status.u16Hperiod);)
        }

        // HSync
        if ( bPollingOnly != TRUE )
            psPCMONITOR_HK_Info[eWindow]->u16Hperiod = sXC_Sync_Status.u16Hperiod;

        if ( abs( sXC_Sync_Status.u16Vtotal - psPCMONITOR_HK_Info[eWindow]->u16Vtotal ) > MD_VTOTAL_TORLANCE )
        {
            PCMSG(printf("MD_VTOTAL_TORLANCE %d\n", (MS_U8)abs( sXC_Sync_Status.u16Vtotal - psPCMONITOR_HK_Info[eWindow]->u16Vtotal )   ));
            bResult = TRUE;
        }
        else
        {
            //printf("Vtt %x\n", sXC_Sync_Status.u16Vtotal);
        }

        // VSync
        if ( bPollingOnly != TRUE )
            psPCMONITOR_HK_Info[eWindow]->u16Vtotal = sXC_Sync_Status.u16Vtotal;

        if( (IsSrcTypeHDMI(psPCMONITOR_HK_Info[eWindow]->eCurrentSrc) || IsSrcTypeDVI(psPCMONITOR_HK_Info[eWindow]->eCurrentSrc))
          &&(MApi_XC_GetHdmiSyncMode() == HDMI_SYNC_DE) )
        {
            MS_WINDOW_TYPE sWin;

            MApi_XC_GetDEWindow(&sWin, eWindow);

            if (abs( sWin.width - psPCMONITOR_HK_Info[eWindow]->sDVI_HDMI_DE_Info.width) > MD_HDE_TORLANCE)
            {
                PCMSG(printf("g_SrcInfo.u16ModeHorizontalDE --->  %u\n", sWin.width));
                bResult = TRUE;
            }

            if (abs(sWin.height - psPCMONITOR_HK_Info[eWindow]->sDVI_HDMI_DE_Info.height) > MD_VDE_TORLANCE)
            {
                PCMSG(printf("g_SrcInfo.u16ModeVerticalDE ----> %u\n", sWin.height));
                bResult = TRUE;
            }

            if ( bPollingOnly != TRUE )
                memcpy(&(psPCMONITOR_HK_Info[eWindow]->sDVI_HDMI_DE_Info), &sWin, sizeof(MS_WINDOW_TYPE));

        }
    }

    _MApi_XC_PCMonitor_SetSyncStatusFlag(sXC_Sync_Status.u8SyncStatus, eWindow); // set global sync status flag

    return bResult; // stable sync
}

void MApi_XC_PCMonitor_Restart(SCALER_WIN eWindow)
{
    psPCMONITOR_HK_Info[eWindow]->eCurrentState = E_XC_PCMONITOR_STABLE_NOSYNC;           // no sync detected yet

    psPCMONITOR_HK_Info[eWindow]->u32LastExecutedTime = MsOS_GetSystemTime();           // set to current time, because bInputTimingChange is TRUE, will execute timing monitor anyway
    psPCMONITOR_HK_Info[eWindow]->bInputTimingChange = TRUE;                              // force timing monitor to detect timing
    psPCMONITOR_HK_Info[eWindow]->u8SyncStatus = XC_MD_SYNC_LOSS;
    psPCMONITOR_HK_Info[eWindow]->ucInputTimingStableCounter = 0;
    psPCMONITOR_HK_Info[eWindow]->ucTimingChangeDebounce = 0;
    psPCMONITOR_HK_Info[eWindow]->bInputTimingStable = TRUE;
    psPCMONITOR_HK_Info[eWindow]->bDoModeSetting = FALSE;

    // analog
    psPCMONITOR_HK_Info[eWindow]->u16Hperiod = 0;
    psPCMONITOR_HK_Info[eWindow]->u16VSyncTime = 0;

    psPCMONITOR_HK_Info[eWindow]->u16Vtotal = 0;
    s_u8LastHDMIPixelRep = 0xFF;
    // digital
    memset(&(psPCMONITOR_HK_Info[eWindow]->sDVI_HDMI_DE_Info), 0, sizeof(MS_WINDOW_TYPE));

}

MS_BOOL MApi_XC_PCMonitor_Init(MS_U8 u8MaxWindowNum)
{
    MS_U8 i;
    SCALER_WIN eWindow = MAIN_WINDOW;

    // initialize House keeping info
    for(i = 0; i < u8MaxWindowNum; i++)
    {
        psPCMONITOR_HK_Info[i] = &(sPCMonitor_HK_Info[i]);

        psPCMONITOR_HK_Info[i]->eCurrentSrc = INPUT_SOURCE_NONE;

        switch(i)
        {
            case 0:
                MApi_XC_PCMonitor_Restart(MAIN_WINDOW);
                eWindow = MAIN_WINDOW;
                break;

            case 1:
                MApi_XC_PCMonitor_Restart(SUB_WINDOW);
                eWindow = SUB_WINDOW;
                break;

            default:
                // un-supported
                break;
        }
    }
    _u16TimingStableCounter = MD_TIMING_STABLE_COUNT;
    _u16TimingNosyncCounter = MD_TIMING_NOSYNC_COUNT;
    _u32PollingPeroid = PC_POLLING_COUNTER;
    return TRUE;
}

void MApi_XC_PCMonitor_SetPollingPeriod(MS_U32 u32PollingPeriod)
{
    _u32PollingPeroid = u32PollingPeriod;
}

MS_U32 MApi_XC_PCMonitor_GetPollingPeroid(void)
{
    return _u32PollingPeroid;
}

void MApi_XC_PCMonitor_SetTimingCount(MS_U16 u16TimingStableCounter, MS_U16 u16TimingNosyncCounter)
{
    _u16TimingStableCounter = u16TimingStableCounter;
    _u16TimingNosyncCounter = u16TimingNosyncCounter;
}

XC_PCMONITOR_STATUS MApi_XC_PCMonitor_GetCurrentState(SCALER_WIN eWindow)
{
    return psPCMONITOR_HK_Info[eWindow]->eCurrentState;
}


//*************************************************************************
//------------------------------ Warning ----------------------------------
//Attention: !!!!!!------ Don't add UI variable or unneeded code in this file
//
//           Don't modify this procedure !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//*************************************************************************
//*************************************************************************
//Function name:    MApi_XC_PCMonitor_SyncLoss
//Passing parameter:    none
//Return parameter: MS_BOOL:  true: sync loss    false: sync active
//Description:          Get signal sync status, true for sync loss, false for sync active
//*************************************************************************
MS_BOOL MApi_XC_PCMonitor_SyncLoss(SCALER_WIN eWindow)
{
    if(psPCMONITOR_HK_Info[eWindow]->u8SyncStatus & XC_MD_SYNC_LOSS)
        return TRUE;
    else
        return FALSE;
}

MS_U8 MApi_XC_PCMonitor_GetSyncStatus(SCALER_WIN eWindow)
{
    return psPCMONITOR_HK_Info[eWindow]->u8SyncStatus;
}

MS_U8 MApi_XC_PCMonitor_GetVSyncTime(SCALER_WIN eWindow)
{
    if((psPCMONITOR_HK_Info[eWindow]->eCurrentState == E_XC_PCMONITOR_UNSTABLE) || (psPCMONITOR_HK_Info[eWindow]->eCurrentState == E_XC_PCMONITOR_STABLE_NOSYNC))
    {
        return 0;
    }
    else
    {
        return (MS_U8)(psPCMONITOR_HK_Info[eWindow]->u16VSyncTime);
    }
}
void MApi_XC_PCMonitor_Get_Dvi_Hdmi_De_Info(SCALER_WIN eWindow,MS_WINDOW_TYPE* msWin)
{
    if((psPCMONITOR_HK_Info[eWindow]->eCurrentState == E_XC_PCMONITOR_UNSTABLE) || (psPCMONITOR_HK_Info[eWindow]->eCurrentState == E_XC_PCMONITOR_STABLE_NOSYNC))
    {
        return;
    }
    else
    {
        msWin->x = psPCMONITOR_HK_Info[eWindow]->sDVI_HDMI_DE_Info.x;
        msWin->y = psPCMONITOR_HK_Info[eWindow]->sDVI_HDMI_DE_Info.y;
        msWin->height= psPCMONITOR_HK_Info[eWindow]->sDVI_HDMI_DE_Info.height;
        msWin->width= psPCMONITOR_HK_Info[eWindow]->sDVI_HDMI_DE_Info.width;
    }

}

MS_U16 MApi_XC_PCMonitor_Get_HFreqx10(SCALER_WIN eWindow)
{
    if((psPCMONITOR_HK_Info[eWindow]->eCurrentState == E_XC_PCMONITOR_UNSTABLE) || (psPCMONITOR_HK_Info[eWindow]->eCurrentState == E_XC_PCMONITOR_STABLE_NOSYNC))
    {
        return 0;
    }
    else
    {
        return MApi_XC_CalculateHFreqx10(psPCMONITOR_HK_Info[eWindow]->u16Hperiod);
    }
}

MS_U16 MApi_XC_PCMonitor_Get_VFreqx10(SCALER_WIN eWindow)
{
    MS_U16 u16VFreq;

    if((psPCMONITOR_HK_Info[eWindow]->eCurrentState == E_XC_PCMONITOR_UNSTABLE) || (psPCMONITOR_HK_Info[eWindow]->eCurrentState == E_XC_PCMONITOR_STABLE_NOSYNC))
    {
        return 0;
    }
    else
    {
        u16VFreq = MApi_XC_CalculateVFreqx10(MApi_XC_PCMonitor_Get_HFreqx10(eWindow), psPCMONITOR_HK_Info[eWindow]->u16Vtotal);
        if(MApi_XC_PCMonitor_GetSyncStatus(eWindow) & XC_MD_INTERLACE_BIT)
        {
            u16VFreq *= 2;
        }
        return u16VFreq;
    }
}

MS_U16 MDrv_XC_PCMonitor_Get_Vtotal(SCALER_WIN eWindow)
{
    if((psPCMONITOR_HK_Info[eWindow]->eCurrentState == E_XC_PCMONITOR_UNSTABLE) || (psPCMONITOR_HK_Info[eWindow]->eCurrentState == E_XC_PCMONITOR_STABLE_NOSYNC))
    {
        return 0;
    }
    else
    {
        return psPCMONITOR_HK_Info[eWindow]->u16Vtotal;
    }
}

MS_U16 MApi_XC_PCMonitor_Get_Vtotal(SCALER_WIN eWindow)
{
    return MDrv_XC_PCMonitor_Get_Vtotal(eWindow);
}

//*************************************************************************
//Function name:    MApi_XC_PCMonitor
//Passing parameter:    none
//Return parameter: none
//Description:          Monitor signal timing from analog port
//
//Attention: !!!!!!------ Don't put UI/CC code in this function,
//           if need, you can use in "MApp_PCMode_PrepareForTimingChange" ------!!!!!!
//
//*************************************************************************
XC_PCMONITOR_STATUS MApi_XC_PCMonitor(INPUT_SOURCE_TYPE_t eCurrentSrc, SCALER_WIN eWindow)
{
//#ifdef VGA_PATH_WITH_AUDIO
    //static bit bNoSyncStatus=0;
//#endif
    PCMSG(static MS_BOOL bNoSyncStatus = TRUE;)
    if(MDrv_SC_Check_IP_Gen_Timing())
    {
        //it ip gen timing case, we need to return previous state to AP to keep everything unchanged.
        return psPCMONITOR_HK_Info[eWindow]->eCurrentState;
    }

    if(psPCMONITOR_HK_Info[eWindow]->eCurrentSrc != eCurrentSrc)
    {
        PCMSG(printf("Restart src %d\n", eCurrentSrc);)
        psPCMONITOR_HK_Info[eWindow]->eCurrentSrc = eCurrentSrc;
        MApi_XC_PCMonitor_Restart(eWindow);             // source changed, restart monitor
    }

    //printf("PCMonitor %d, %ld, %ld\n", psPCMONITOR_HK_Info[eWindow]->bInputTimingChange,
    //        psPCMONITOR_HK_Info[eWindow]->u32LastExecutedTime,
    //        OS_SYSTEM_TIME());

    if((FALSE == psPCMONITOR_HK_Info[eWindow]->bInputTimingChange) &&
        (MsOS_GetSystemTime() - psPCMONITOR_HK_Info[eWindow]->u32LastExecutedTime < _u32PollingPeroid))
    {
        return psPCMONITOR_HK_Info[eWindow]->eCurrentState;
    }
    else
    {
        // update last executed time
        psPCMONITOR_HK_Info[eWindow]->u32LastExecutedTime = MsOS_GetSystemTime();
    }

    if(IsSrcTypeHDMI(psPCMONITOR_HK_Info[eWindow]->eCurrentSrc) || IsSrcTypeDVI(psPCMONITOR_HK_Info[eWindow]->eCurrentSrc) )
    {
         MDrv_XC_Switch_DE_HV_Mode_By_Timing(eWindow);
    }

    // DE-Bypass is applied in HDMI HV mode. It will ignore pixel repetition package from HDMI.
    // We use the software mointor to monitor this package.
    if( (IsSrcTypeHDMI(psPCMONITOR_HK_Info[eWindow]->eCurrentSrc) || IsSrcTypeDVI(psPCMONITOR_HK_Info[eWindow]->eCurrentSrc) )
      &&(MApi_XC_GetHdmiSyncMode() == HDMI_SYNC_HV)
      &&(eWindow == MAIN_WINDOW) )
    {
        MS_U8 u8CurHDMIPixelRep = MDrv_HDMI_Get_Pixel_Repetition();
        if( u8CurHDMIPixelRep != s_u8LastHDMIPixelRep )
        {
            if( u8CurHDMIPixelRep )
            {
                //printf("daniel_test PC HV mode: Pixel-Rep=1 FIR=81\n");
                // If avi pkg report pixel repetition, IP1 should enable divider.
                MApi_XC_SetFIRDownSampleDivider(ENABLE,0x01,eWindow);
            }
            else
            {
                //printf("daniel_test PC HV mode: Pixel-Rep=0 FIR=0\n");
                MApi_XC_SetFIRDownSampleDivider(DISABLE,0x00,eWindow);
            }
            //_s_u32LastResetTime = MsOS_GetSystemTime();
            //MApi_XC_SoftwareReset(REST_IP_F2, eWindow);
            psPCMONITOR_HK_Info[eWindow]->bInputTimingChange = TRUE;
            //printf("daniel_test HDMI Pixel-Rep change to %u\n", u8CurHDMIPixelRep);
            s_u8LastHDMIPixelRep = u8CurHDMIPixelRep;
        }
    }

    //--------------------------------------------------
    //display active or no signal?
    if ( psPCMONITOR_HK_Info[eWindow]->bInputTimingChange == FALSE ) // check timing change flag have be setting
    {
        if ( MApi_XC_PCMonitor_SyncLoss(eWindow) ) // no sync
        {
            if ( _MApi_XC_PCMonitor_DetectSync(psPCMONITOR_HK_Info[eWindow]->eCurrentSrc,eWindow) ) // check signal active
            {
                psPCMONITOR_HK_Info[eWindow]->bInputTimingChange = TRUE;

                PCMSG(if(bNoSyncStatus) printf("NewSync\n"); bNoSyncStatus = FALSE;)
            }
            else
            {
                PCMSG(if(!bNoSyncStatus) printf("NoSync\n"); bNoSyncStatus = TRUE;)
            }
        }
        else //have sync?
        {
            if ( MApi_XC_PCMonitor_InvalidTimingDetect(FALSE, eWindow) ) // check invalid sync
            {
                PCMSG(printf("InvalidTimingDetect GO TO UNSTABLE! \n" )) ;
                psPCMONITOR_HK_Info[eWindow]->u8SyncStatus |= XC_MD_SYNC_LOSS;
                psPCMONITOR_HK_Info[eWindow]->bInputTimingChange = TRUE;
            }
            #if ENABLE_IP_AUTO_COAST
            if ( IsSrcTypeVideo(psPCMONITOR_HK_Info[eWindow]->eCurrentSrc) )
            {
                msAPI_Scaler_IPAutoCoastHandler();
            }
            #endif
        }
    }

    if ( psPCMONITOR_HK_Info[eWindow]->bInputTimingChange == FALSE )
    {
        // PC monitor have to changed when auto nosignal enabled.
        // If no signal bit is enabled, that's treaded as no-signal
        // Here have to update variables for hw-detected-no-signal.
        // Please Note!! Input source disable function should be disabled from App layer when auto no signal enabled.
        if( gSrcInfo[eWindow].bAutoNoSignalEnabled )
        {
            if( MApi_XC_IsInputSourceDisabled(eWindow)
              &&(psPCMONITOR_HK_Info[eWindow]->eCurrentState == E_XC_PCMONITOR_STABLE_SYNC) )
            {
                PCMSG(printf("No signal detected : GO TO UNSTABLE! \n" )) ;
                psPCMONITOR_HK_Info[eWindow]->u8SyncStatus |= XC_MD_SYNC_LOSS;
                psPCMONITOR_HK_Info[eWindow]->bInputTimingChange = TRUE;
            //    printf("daniel_test bInputTimingChange=1 AutoNoSync t=%lu\n", MsOS_GetSystemTime() );
            }
        }
    }

    //is input timing changed and check debounce
    //--------------------------------------------------
    if ( psPCMONITOR_HK_Info[eWindow]->bInputTimingChange) // input timing is unstable
    {
        PCMSG(printf("TimingChg\n"));

        psPCMONITOR_HK_Info[eWindow]->bInputTimingChange = FALSE;
        psPCMONITOR_HK_Info[eWindow]->ucTimingChangeDebounce++;

        PCMSG(printf("Deb=%d\n", psPCMONITOR_HK_Info[eWindow]->ucTimingChangeDebounce));

        if ( !MApi_XC_PCMonitor_SyncLoss(eWindow) ) // have signal
        {
            if ( psPCMONITOR_HK_Info[eWindow]->ucTimingChangeDebounce <= 3 ) // prevent from SOG
            {
                psPCMONITOR_HK_Info[eWindow]->u8SyncStatus |= XC_MD_SYNC_LOSS;
                return psPCMONITOR_HK_Info[eWindow]->eCurrentState;
            }
        }
        else
        {
            if ( IsSrcTypeYPbPr(psPCMONITOR_HK_Info[eWindow]->eCurrentSrc ) ||
                 IsSrcTypeVga(psPCMONITOR_HK_Info[eWindow]->eCurrentSrc )  )
            {
                // System have to set free run as soon as possible when timing unstable.
				if ( !_MApi_XC_PCMonitor_IsStableSourceUsed(eCurrentSrc, eWindow) )
				{
					MDrv_XC_ADC_Set_Freerun(TRUE);
				}
            }
        }

        psPCMONITOR_HK_Info[eWindow]->eCurrentState = E_XC_PCMONITOR_UNSTABLE;

        _MApi_XC_PCMonitor_PrepareForTimingChange(eWindow);

        // Already enter timing change, turn off AutoNoSignal
        if( gSrcInfo[eWindow].bAutoNoSignalEnabled )
        {
            MApi_XC_EnableIPAutoNoSignal(FALSE, eWindow);
            //printf("daniel_test AutoNoSignal=0 t=%lu %s\n", MsOS_GetSystemTime(), __FUNCTION__);
        }
    }
    else // input timing is stable
    {
        psPCMONITOR_HK_Info[eWindow]->ucTimingChangeDebounce = 0; // reset timing change debounce counter

        psPCMONITOR_HK_Info[eWindow]->ucInputTimingStableCounter++; // timing stable counter

        if ( psPCMONITOR_HK_Info[eWindow]->ucInputTimingStableCounter == 0 )
            psPCMONITOR_HK_Info[eWindow]->ucInputTimingStableCounter = 1;

        if ( !psPCMONITOR_HK_Info[eWindow]->bInputTimingStable )
        {
            MS_ASSERT(_u16TimingStableCounter > 0);
            MS_ASSERT(_u16TimingNosyncCounter > 0);
            if( ((!MApi_XC_PCMonitor_SyncLoss(eWindow)) && (psPCMONITOR_HK_Info[eWindow]->ucInputTimingStableCounter >= _u16TimingStableCounter)) ||
                (MApi_XC_PCMonitor_SyncLoss(eWindow) && (psPCMONITOR_HK_Info[eWindow]->ucInputTimingStableCounter >= _u16TimingNosyncCounter)) )
            {
                psPCMONITOR_HK_Info[eWindow]->bInputTimingStable = TRUE;
                //g_PcadcModeSetting.u8ModeIndex = 0;  // reset mode index

                if(MApi_XC_PCMonitor_SyncLoss(eWindow))
                {
                    psPCMONITOR_HK_Info[eWindow]->eCurrentState = E_XC_PCMONITOR_STABLE_NOSYNC;
                }
                else
                {
                    psPCMONITOR_HK_Info[eWindow]->eCurrentState = E_XC_PCMONITOR_STABLE_SYNC;
                    // Update V sync time when timing stable.
                    psPCMONITOR_HK_Info[eWindow]->u16VSyncTime = MApi_XC_CalculateVsyncTime(psPCMONITOR_HK_Info[eWindow]->u16Vtotal, psPCMONITOR_HK_Info[eWindow]->u16Hperiod);

                    //_MApi_XC_PCMonitor_ModeParse(eWindow);
                }

                PCMSG(printf("*** Do Mode In Analog Path ***\n"));
                return psPCMONITOR_HK_Info[eWindow]->eCurrentState;
            }
        }
    }
    return psPCMONITOR_HK_Info[eWindow]->eCurrentState;
}


#undef _API_XC_PC_MONITOR_C_
#endif  // _API_XC_PC_MONITOR_C_
