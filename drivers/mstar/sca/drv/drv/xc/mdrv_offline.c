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

/******************************************************************************/
/*                           Header Files                                     */
/******************************************************************************/
#if defined(MSOS_TYPE_LINUX_KERNEL)
#include <linux/wait.h>
#endif

// Common Definition
#include "MsCommon.h"
#include "mhal_xc_chip_config.h"
#include "drvXC_IOPort.h"
#include "apiXC.h"
#include "mhal_offline.h"
#include "xc_hwreg_utility2.h"
#include "apiXC_Adc.h"
#include "drvXC_ADC_Internal.h"
#include "mhal_mux.h"

//----------------------------
//
//----------------------------
#define    AIS_Debug(x)    //x


//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
MS_U8 MDrv_XC_GetOffLineDetection( INPUT_SOURCE_TYPE_t u8InputSourceType )
{
    MS_U8 u8CheckSrc = INPUT_PORT_NONE_PORT;

    u8CheckSrc = MApi_XC_MUX_MapInputSourceToVDYMuxPORT(u8InputSourceType);

    AIS_Debug(printf("u8CheckSrc=%u\n", u8CheckSrc);)

    return Hal_XC_GetOffLineDetection(u8CheckSrc);
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
void MDrv_XC_SetOffLineDetection ( INPUT_SOURCE_TYPE_t u8InputSourceType )
{
    MS_U8 u8CheckSrc = INPUT_PORT_NONE_PORT;

    _XC_ENTRY();

    u8CheckSrc = MApi_XC_MUX_MapInputSourceToVDYMuxPORT(u8InputSourceType);
    AIS_Debug(printf("u8InputSourceType = %x, u8CheckSrc = %x\n", u8InputSourceType, (MS_U16)u8CheckSrc);)

    Hal_XC_SetOfflineDetectClk();

    if (u8CheckSrc == INPUT_PORT_NONE_PORT)
    {
        Hal_XC_OffLineExit();
    }
    else
    {
        Hal_XC_OffLineInit();
    }

    _XC_RETURN();

    MsOS_DelayTask(10);

    if ((u8CheckSrc >= (MS_U8)INPUT_PORT_YMUX_CVBS0) &&
        (u8CheckSrc <= (MS_U8)INPUT_PORT_YMUX_CVBS7))
    {
        _XC_ENTRY();
        AIS_Debug(printf("MDrv_SetOffLineToSog VD Related Signal!\n");)
        Hal_XC_SetOffLineToSog_AV(u8CheckSrc - INPUT_PORT_YMUX_CVBS0);
        _XC_RETURN();

        MsOS_DelayTask(20);

        _XC_ENTRY();
        Hal_XC_WaitForHVCleared_AV(u8CheckSrc - INPUT_PORT_YMUX_CVBS0);
        Hal_XC_TurnOnDetectClkPath(SC_SUBWIN_IPMUX_ADC_B);
         _XC_RETURN();

       MsOS_DelayTask(50);
    }
    else if((u8CheckSrc >= (MS_U8)INPUT_PORT_ANALOG0)&&
           (u8CheckSrc <= (MS_U8)INPUT_PORT_ANALOG4))
    {
        _XC_ENTRY();
        AIS_Debug(printf("MDrv_SetOffLineToSog_YUV Component!\n");)
        Hal_XC_SetOffLineToSog_YUV(u8CheckSrc - INPUT_PORT_ANALOG0);
        _XC_RETURN();

        MsOS_DelayTask(20);

        _XC_ENTRY();
        Hal_XC_WaitForHVCleared(u8CheckSrc - INPUT_PORT_ANALOG0);
        Hal_XC_TurnOnDetectClkPath(SC_SUBWIN_IPMUX_ADC_B);
        _XC_RETURN();

        MsOS_DelayTask(50);
    }
    else if ((u8CheckSrc >= INPUT_PORT_ANALOG0_SYNC)  &&
             (u8CheckSrc <= (MS_U8)INPUT_PORT_ANALOG4_SYNC) )
    {
        _XC_ENTRY();
        AIS_Debug(printf("MDrv_SetOffLineToHV VGA!\n");)
        Hal_XC_SetOffLineToHv(u8CheckSrc - INPUT_PORT_ANALOG0_SYNC);
        _XC_RETURN();

        MsOS_DelayTask(20);

        _XC_ENTRY();
        Hal_XC_WaitForHVCleared(u8CheckSrc - INPUT_PORT_ANALOG0_SYNC);
        Hal_XC_TurnOnDetectClkPath(SC_SUBWIN_IPMUX_ADC_B);
        _XC_RETURN();

        MsOS_DelayTask(50);
    }
    else if ((u8CheckSrc >= INPUT_PORT_DVI0) && (u8CheckSrc <= INPUT_PORT_DVI3))
    {
        AIS_Debug(printf("MDrv_SetOffLineToHDMI 1~3!\n");)
        Hal_XC_SetOffLineToHDMI(u8CheckSrc);
    }
    else if (u8CheckSrc == INPUT_PORT_MVOP )
    {
        AIS_Debug(printf("MDrv_SetOffLineToUSB!\n");)
        Hal_XC_SetOffLineToUSB(u8CheckSrc);
    }
}

MS_BOOL MDrv_XC_SetOffLineSogThreshold(MS_U8 u8Threshold)
{
    return Hal_XC_SetOffLineSogThreshold(u8Threshold);
}

MS_BOOL MDrv_XC_SetOffLineSogBW(MS_U8 u8BW)
{
    return Hal_XC_SetOffLineSogBW(u8BW);
}

//-------------------------------------------------------------------------------------------------
/// Get Offline detection status to specific input source
/// @param  u8InputSourceType                \b IN: the specific input source type to do offline detection
//-------------------------------------------------------------------------------------------------
MS_U8 MApi_XC_GetOffLineDetection( INPUT_SOURCE_TYPE_t u8InputSourceType )
{
    MS_U8 u8Ret = 0;
    _XC_ENTRY();
    u8Ret = MDrv_XC_GetOffLineDetection(u8InputSourceType);
    _XC_RETURN();
    return u8Ret;
}

//-------------------------------------------------------------------------------------------------
/// Set Offline detection to specific input source
/// @param  u8InputSourceType                \b IN: the specific input source type to do offline detection
//-------------------------------------------------------------------------------------------------
void MApi_XC_SetOffLineDetection ( INPUT_SOURCE_TYPE_t u8InputSourceType )
{
    MDrv_XC_SetOffLineDetection(u8InputSourceType);
}

//-------------------------------------------------------------------------------------------------
/// Set Offline detection Sog Threshold
/// @param  u8Threshold                \b IN: threshold value t2,u3,u4: 0~31 other chip: 0~255
/// @Return TRUE: Successful FALSE: Fail
//-------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_SetOffLineSogThreshold(MS_U8 u8Threshold)
{
    MS_BOOL bRet = FALSE;
    _XC_ENTRY();
    bRet = MDrv_XC_SetOffLineSogThreshold(u8Threshold);
    _XC_RETURN();
    return bRet;
}

//-------------------------------------------------------------------------------------------------
/// Set Offline Detection Sog filter bandwidth
/// @param  u8BW                \b IN: bandwidth value t2,u3,u4: 0~7 other chip: 0~31
/// @Return TRUE: Successful FALSE: Fail
//-------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_SetOffLineSogBW(MS_U8 u8BW)
{
    MS_BOOL bRet = FALSE;
    _XC_ENTRY();
    bRet = MDrv_XC_SetOffLineSogBW(u8BW);
    _XC_RETURN();
    return bRet;
}

MS_BOOL MApi_XC_OffLineInit(void)
{
    _XC_ENTRY();
    Hal_XC_OffLineInit();
    MDrv_XC_SetOffLineSogThreshold(0x40);
    MDrv_XC_SetOffLineSogBW(0x1F);
    _XC_RETURN();
    return TRUE;
}

