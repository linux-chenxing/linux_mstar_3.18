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

////////////////////////////////////////////////////////////////////////////////
//
/// This file contains MStar ACE Application Interface.
/// brief API for Advanced Color Engine Control
/// author MStar Semiconductor, Inc.
//
////////////////////////////////////////////////////////////////////////////////


#ifndef _API_XC_ACE_C_
#define _API_XC_ACE_C_

/******************************************************************************/
/*                    Header Files                                            */
/* ****************************************************************************/

// Need to remove
//#include "msAPI_Timer.h"


// Must have
#ifdef MSOS_TYPE_LINUX_KERNEL
#include <linux/string.h>
#include <linux/kernel.h>
#else
#include <string.h>
#endif
// Common Definition
#include "MsCommon.h"
#include "MsVersion.h"
#include "MsIRQ.h"
#include "MsOS.h"
#include "drvACE.h"
#include "apiXC_Ace.h"
// Internal Definition
#include "drvMMIO.h"

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

#define MAIN_WINDOW     0
#define SUB_WINDOW      1
#ifdef MULTI_SCALER_SUPPORTED
#define SC1_MAIN_WINDOW     2
#define SC1_SUB_WINDOW      3
#define SC2_MAIN_WINDOW     4
#define SC2_SUB_WINDOW      5
#endif

/******************************************************************************/
/*                      Debug information                                      */
/******************************************************************************/

#define XC_ACE_DBG  0
#define XC_ACE_MUTEX_DBG 0

#if XC_ACE_DBG
#define XC_ACE_DBG_MSG(fmt,...) \
    MS_DEBUG_MSG(do{printf( "[ACE_DBG_MSG]: %s: %d \n"  fmt, __FUNCTION__, __LINE__, ## __VA_ARGS__);} while(0))

#define XC_ACE_PRINT_VAR(var)  \
    MS_DEBUG_MSG(do{\
        XC_ACE_DBG_MSG("%30s: %d\n", #var, (var));\
        }while(0))

#define XC_ACE_PRINT_FUN(fun)  \
    MS_DEBUG_MSG(do{\
        XC_ACE_DBG_MSG("\n");\
        XC_ACE_DBG_MSG("**************************************************\n");\
        XC_ACE_DBG_MSG("*\t\t%s \t\t\n", #fun);\
        XC_ACE_DBG_MSG("**************************************************\n");\
        fun;\
        XC_ACE_DBG_MSG("*                                                *\n");\
        XC_ACE_DBG_MSG("*                                                *\n");\
        XC_ACE_DBG_MSG("**************************************************\n");\
        }while(0))

#else
  #define XC_ACE_DBG_MSG(str, ...)
  #define XC_ACE_PRINT_VAR(var)
  #define XC_ACE_PRINT_FUN(fun) fun
#endif

#define ACE_DBG_MSG(fmt,...)                                                                 \
    MS_DEBUG_MSG(do{                                                                         \
        if(_u16XC_ACEDbgSwitch){                                                             \
           printf( "[ACE_DBG_MSG]: %s: %d \n"  fmt, __FUNCTION__, __LINE__, ## __VA_ARGS__); \
          }                                                                                  \
      } while(0))


#ifndef UNUSED
	#if defined(MSOS_TYPE_NOS)
	#define UNUSED(x) ((x)=(x))
	#else
	#define UNUSED(x)
	#endif
#endif


#define MWE_TIMER           60
#define MWE_H_STEP          6
#define MWE_MOVE_RIGHT      1
#define MWE_MOVE_LEFT       0
static E_XC_ACE_MWE_FUNC  _MWEFunc = E_XC_ACE_MWE_MODE_OFF;
static MS_U32       _MWETimer = 0;
static MS_U32       _MWEFlag = MWE_MOVE_LEFT;
static MS_BOOL bMWEStatus = FALSE;
static MWE_RECT gMWERect;

static XC_ACE_InitData g_XC_ACE_InitData;

static const XC_ACE_ApiInfo _stXC_ACE_ApiInfo = {0};
static MS_U16 _u16XC_ACEDbgSwitch = 0;
static MS_BOOL _bXC_ACE_LoadFromTable = FALSE;
static E_XC_ACE_MWE_MIRROR_TYPE _enMirrorMode = E_XC_ACE_MWE_MIRROR_NORMAL;

// OS related
#define XC_ACE_WAIT_MUTEX              (TICK_PER_ONE_MS * 50)              // 50 ms

// Mutex & Lock
static MS_S32 _s32XC_ACEMutex = -1;

#if XC_ACE_MUTEX_DBG
#define XC_ACE_ENTRY()      if(!MsOS_ObtainMutex(_s32XC_ACEMutex, MSOS_WAIT_FOREVER))                     \
                            {                                                                        \
                                printf("==========================\n");                              \
                                printf("[%s][%s][%06d] Mutex taking timeout\n",__FILE__,__FUNCTION__,__LINE__);    \
                            }
#else
#define XC_ACE_ENTRY()      if(!MsOS_ObtainMutex(_s32XC_ACEMutex, MSOS_WAIT_FOREVER))                     \
                            {                                                                        \
                            }
#endif

#define XC_ACE_RETURN()     MsOS_ReleaseMutex(_s32XC_ACEMutex);


static MSIF_Version _api_xc_ace_version = {
  { XC_ACE_API_VERSION },
};

void MApi_XC_ACE_ResetGlobalVar(void)
{
    if(_s32XC_ACEMutex)
    {
		OS_DELETE_MUTEX(_s32XC_ACEMutex);
		_s32XC_ACEMutex = -1;
    }
}

//******************************************************************************
///-initialize ACE
//*************************************************************************
MS_BOOL MApi_XC_ACE_Init(XC_ACE_InitData *pstXC_ACE_InitData, MS_U32 u32InitDataLen)
{
    MS_U32 u32NonPMBankSize, u32ACERiuBaseAddr=0;
    char word[] = {"_XC_Mutex"};

    if(u32InitDataLen != sizeof(XC_ACE_InitData) || pstXC_ACE_InitData == NULL)
    {
        return FALSE;
    }

    if (_s32XC_ACEMutex != -1)
    {
        (printf("MApi_XC_ACE_Init Initialization more than once\n"));
        return FALSE;
    }

    _s32XC_ACEMutex = MsOS_CreateMutex(E_MSOS_FIFO, word, MSOS_PROCESS_SHARED);
    if (_s32XC_ACEMutex == -1)
    {
        //(printf("[MAPI ACE][%06d] create mutex fail\n", __LINE__));
        return FALSE;
    }

    // get MMIO base
    if(MDrv_MMIO_GetBASE( &u32ACERiuBaseAddr, &u32NonPMBankSize, MS_MODULE_PM ) != TRUE)
    {
        //OS_DELETE_MUTEX(_s32ACEMutex);
        //PNL_ASSERT(0, "%s\n", "Get base address failed\n");
        //(printf("XC_ACE GetBase failed\n"));
        return FALSE;
    }
    else
    {
    }

    MDrv_ACE_init_riu_base( u32ACERiuBaseAddr);

    if(pstXC_ACE_InitData->S16ColorCorrectionMatrix == NULL ||
       pstXC_ACE_InitData->S16RGB == NULL)
    {
        return FALSE;
    }
    else
    {
        MDrv_ACE_Init(pstXC_ACE_InitData->eWindow, pstXC_ACE_InitData->S16ColorCorrectionMatrix, pstXC_ACE_InitData->S16RGB );
    }

    XC_ACE_ENTRY();
    MDrv_ACE_SetColorCorrectionTable( pstXC_ACE_InitData->eWindow);
    MDrv_ACE_SetPCsRGBTable( pstXC_ACE_InitData->eWindow);
    XC_ACE_RETURN();

	// MWE init start
    memcpy(&g_XC_ACE_InitData, pstXC_ACE_InitData, u32InitDataLen);
    _bXC_ACE_LoadFromTable = FALSE;


    if(pstXC_ACE_InitData->bMWE_Enable)
    {
        XC_ACE_ENTRY();

        MDrv_XC_ACE_MWEinit(pstXC_ACE_InitData->u16MWEHstart, pstXC_ACE_InitData->u16MWEVstart, pstXC_ACE_InitData->u16MWEWidth, pstXC_ACE_InitData->u16MWEHeight);

        XC_ACE_RETURN();

        MApi_XC_ACE_MWEFuncSel(SUB_WINDOW, E_XC_ACE_MWE_MODE_H_SPLIT);
        MApi_XC_ACE_EnableMWE(FALSE);
    }

    return TRUE;
}

MS_BOOL MApi_XC_ACE_Exit(void)
{
    memset(&g_XC_ACE_InitData, 0 , sizeof(XC_ACE_InitData) );
    _bXC_ACE_LoadFromTable = FALSE;
    //_s32XC_ACEMutex = -1;
    return TRUE;
}

void MApi_XC_ACE_DMS(MS_BOOL eWindow, MS_BOOL bisATV)
{
    XC_ACE_ENTRY();
    MDrv_ACE_DMS(eWindow, bisATV);
    XC_ACE_RETURN();
}

void MApi_XC_ACE_SetPCYUV2RGB(MS_BOOL eWindow, MS_BOOL bEnable)
{
    XC_ACE_ENTRY();
    MDrv_ACE_PCForceYUVtoRGB(eWindow , bEnable);
    XC_ACE_RETURN();
}

void MApi_XC_ACE_SelectYUVtoRGBMatrix(MS_BOOL eWindow, MS_U8 ucMatrix, MS_S16* psUserYUVtoRGBMatrix)
{
    XC_ACE_ENTRY();
    MDrv_ACE_SelectYUVtoRGBMatrix( eWindow, ucMatrix, psUserYUVtoRGBMatrix );
    XC_ACE_RETURN();
}

void MApi_XC_ACE_SetColorCorrectionTable( MS_BOOL bScalerWin)
{
    XC_ACE_ENTRY();
    MDrv_ACE_SetColorCorrectionTable( bScalerWin );
    XC_ACE_RETURN();
}

void MApi_XC_ACE_SetPCsRGBTable( MS_BOOL bScalerWin)
{
    XC_ACE_ENTRY();
    MDrv_ACE_SetPCsRGBTable( bScalerWin);
    XC_ACE_RETURN();
}

void MApi_XC_ACE_GetColorMatrix( MS_BOOL eWindow, MS_U16* pu16Matrix)
{
    XC_ACE_ENTRY();
    MDrv_ACE_GetColorMatrix(eWindow, pu16Matrix);
    XC_ACE_RETURN();
}

void MApi_XC_ACE_SetColorMatrix( MS_BOOL eWindow, MS_U16* pu16Matrix)
{
    XC_ACE_ENTRY();
    MDrv_ACE_SetColorMatrix(eWindow, pu16Matrix);
    XC_ACE_RETURN();
}


void MApi_XC_ACE_PatchDTGColorChecker( MS_U8 u8Mode)
{
    XC_ACE_ENTRY();
    MDrv_ACE_PatchDTGColorChecker(u8Mode);
    XC_ACE_RETURN();
}

MS_U16 MApi_XC_ACE_GetACEInfo( MS_BOOL bWindow, E_XC_ACE_INFOTYPE eXCACEInfoType )
{
    MS_U16 u16ACEinfo;
    u16ACEinfo = eXCACEInfoType;
    XC_ACE_ENTRY();
    u16ACEinfo = MDrv_ACE_GetACEInfo(bWindow, u16ACEinfo);
    XC_ACE_RETURN();

    return u16ACEinfo;
}

void MApi_XC_ACE_GetACEInfoEx( MS_BOOL bWindow, XC_ACE_InfoEx *pInfoEx)
{
    memset(pInfoEx, 0, sizeof(XC_ACE_InfoEx));

    XC_ACE_ENTRY();

    if (MDrv_ACE_IsPostRGBGain(bWindow))
    {
        pInfoEx->u32Mode |= ACE_INFOEX_MODE_POST_RGB_GAIN;
    }

    if (MDrv_ACE_IsPostRGBOffset(bWindow))
    {
        pInfoEx->u32Mode |= ACE_INFOEX_MODE_POST_RGB_OFFSET;
    }

    XC_ACE_RETURN();
}

/********************************************************************************/
/*                   msAPI_ACE_Ext.c                   */
/********************************************************************************/

/********************************************************************************/
/*                   Functions                      */
/********************************************************************************/


/******************************************************************************/
///-This function will set up brightness for YPbPr and digital port
///@param eWindow        \b IN: Window type
///@param u8Brightness_R \b IN: brightness R value
///@param u8Brightness_G \b IN: brightness G value
///@param u8Brightness_B \b IN: brightness B value
/******************************************************************************/
void MApi_XC_ACE_PicSetBrightness( MS_BOOL eWindow, MS_U8 u8Brightness_R, MS_U8 u8Brightness_G, MS_U8 u8Brightness_B)
{
    XC_ACE_ENTRY();
    MDrv_ACE_SetBrightness( eWindow, u8Brightness_R, u8Brightness_G, u8Brightness_B);
    XC_ACE_RETURN();
}

/******************************************************************************/
///-This function will set up Precise brightness for YPbPr and digital port
///@param eWindow        \b IN: Window type
///@param u16Brightness_R \b IN: brightness R value
///@param u16Brightness_G \b IN: brightness G value
///@param u16Brightness_B \b IN: brightness B value
/******************************************************************************/
void MApi_XC_ACE_PicSetBrightnessPrecise(MS_BOOL eWindow, MS_U16 u16Brightness_R, MS_U16 u16Brightness_G, MS_U16 u16Brightness_B)
{
    XC_ACE_ENTRY();
    MDrv_ACE_SetBrightnessPrecise(eWindow, u16Brightness_R, u16Brightness_G, u16Brightness_B);
    XC_ACE_RETURN();
}

/******************************************************************************/
//-Adjust sharpness
///@param eWindow        \b IN: Window type
///@param u8Sharpness    \b IN: sharpness value
//- 0 -0x3f
/******************************************************************************/
void MApi_XC_ACE_PicSetSharpness( MS_BOOL eWindow, MS_U8 u8Sharpness )
{
    XC_ACE_ENTRY();
    MDrv_ACE_SetSharpness(eWindow, u8Sharpness);
    XC_ACE_RETURN();
}

/******************************************************************************/
//-Adjust contrast for YPbPr and digital port
//@param u8Contrast \b IN:
//- video color Contrast setting 0 -0xff
//@param u8SubContrast \b IN:
//- video color SubContrast setting 0 -0xff
/******************************************************************************/

void MApi_XC_ACE_PicSetContrast( MS_BOOL eWindow, MS_BOOL bUseYUVSpace, MS_U8 u8Contrast)
{
    XC_ACE_ENTRY();
    MDrv_ACE_PicSetContrast(eWindow, bUseYUVSpace, u8Contrast);
    XC_ACE_RETURN();
}

/******************************************************************************/
//-Adjust contrast for YPbPr and digital port only on G channel
//@param u8Contrast \b IN:
//- video color Contrast setting 0 -0xff
//@param u8SubContrast \b IN:
//- video color SubContrast setting 0 -0xff
/******************************************************************************/
void MApi_XC_ACE_PicSetContrast_G( MS_BOOL eWindow, MS_BOOL bUseYUVSpace, MS_U8 u8Contrast)
{
    XC_ACE_ENTRY();
    MDrv_ACE_PicSetContrast_G(eWindow, bUseYUVSpace, u8Contrast);
    XC_ACE_RETURN();
}

/******************************************************************************/
///-Adjust Hue for YPbPr and digital port
///@param eWindow \b IN: Enable Hue
///@param bUseYUVSpace \b IN: Is Yuv space
///@param u8Hue \b IN: Hue value
/******************************************************************************/
void MApi_XC_ACE_PicSetHue( MS_BOOL eWindow, MS_BOOL bUseYUVSpace, MS_U8 u8Hue )
{
    if(bUseYUVSpace)
    {
        XC_ACE_ENTRY();
        MDrv_ACE_SetHue(eWindow, u8Hue);
        XC_ACE_RETURN();
    }
    else
    {
        ;
    }
}

//-------------------------------------------------------------------------------------------------
// Skip Wait Vsync
/// @param eWindow               \b IN: Enable
/// @param Skip wait Vsync      \b IN: Disable wait Vsync
//-------------------------------------------------------------------------------------------------
E_XC_ACE_RESULT MApi_XC_ACE_SkipWaitVsync( MS_BOOL eWindow,MS_BOOL bIsSkipWaitVsyn)
{
    MDrv_ACE_SetSkipWaitVsync( eWindow, bIsSkipWaitVsyn);
    return E_XC_ACE_OK;
}

/******************************************************************************/
//-Adjust Saturation for YPbPr and digital port
//@param u8Saturation \b IN:
//- video color setting 0 -100
/******************************************************************************/
void MApi_XC_ACE_PicSetSaturation(MS_BOOL eWindow, MS_BOOL bUseYUVSpace, MS_U8 u8Saturation )
{
    if(bUseYUVSpace)
    {
        XC_ACE_ENTRY();
        MDrv_ACE_SetSaturation(eWindow, u8Saturation);
        XC_ACE_RETURN();
    }
    else
    {
        ;
    }
}

void MApi_XC_ACE_SetFleshTone(MS_BOOL eWindow, MS_BOOL bEn, MS_U8 u8FleshTone)
{
    XC_ACE_ENTRY();
    MDrv_ACE_SetFleshTone(eWindow, bEn, u8FleshTone);
    XC_ACE_RETURN();
}

void MApi_XC_ACE_SetBlackAdjust(MS_BOOL eWindow, MS_U8 u8BlackAdjust)
{
    XC_ACE_ENTRY();
    MDrv_ACE_SetBlackAdjust(eWindow, u8BlackAdjust);
    XC_ACE_RETURN();
}

//*************************************************************************
///-set video color
///@param eWindow \b IN: Enable Color temp
///@param bUseYUVSpace \b IN: Is in YUV space
///@param pstColorTemp \b IN: Color temp need to be set
//*************************************************************************
void MApi_XC_ACE_PicSetColorTemp( MS_BOOL eWindow, MS_BOOL bUseYUVSpace, XC_ACE_color_temp* pstColorTemp)
{
    XC_ACE_ENTRY();
    MDrv_ACE_PicSetRGB( eWindow, bUseYUVSpace , pstColorTemp->cRedColor, pstColorTemp->cGreenColor, pstColorTemp->cBlueColor );
    XC_ACE_RETURN();
}

//*************************************************************************
///-set video color ex
///@param eWindow \b IN: Enable Color temp
///@param bUseYUVSpace \b IN: Is in YUV space
///@param pstColorTemp \b IN: Color temp need to be set
//*************************************************************************
void MApi_XC_ACE_PicSetColorTempEx( MS_BOOL eWindow, MS_BOOL bUseYUVSpace, XC_ACE_color_temp_ex* pstColorTemp)
{
    XC_ACE_ENTRY();
    MDrv_ACE_PicSetRGB( eWindow, bUseYUVSpace , pstColorTemp->cRedColor, pstColorTemp->cGreenColor, pstColorTemp->cBlueColor );
    XC_ACE_RETURN();
}

//*************************************************************************
///- set post color temp Version 2
///@param eWindow \b IN: Enable Color temp
///@param pstColorTemp \b IN: Color temp need to be set
//*************************************************************************
void MApi_XC_ACE_PicSetPostColorTemp_V02(MS_BOOL eWindow, XC_ACE_color_temp_ex2* pstColorTemp)
{
    XC_ACE_ENTRY();
    MDrv_ACE_PicSetPostRGBGain(eWindow, pstColorTemp->cRedColor, pstColorTemp->cGreenColor, pstColorTemp->cBlueColor);
    MDrv_ACE_PicSetPostRGBOffset(eWindow, pstColorTemp->cRedOffset, pstColorTemp->cGreenOffset, pstColorTemp->cBlueOffset, 0);
    XC_ACE_RETURN();
}

//*************************************************************************
///-obosolte!! use MApi_XC_ACE_PicSetPostColorTemp_V02 instead
//*************************************************************************
void MApi_XC_ACE_PicSetPostColorTemp(MS_BOOL eWindow, XC_ACE_color_temp* pstColorTemp)
{
    XC_ACE_color_temp_ex2 pstColorTemp2;
    pstColorTemp2.cRedOffset = pstColorTemp->cRedOffset<<3 ;
    pstColorTemp2.cGreenOffset= pstColorTemp->cGreenOffset<<3;
    pstColorTemp2.cBlueOffset= pstColorTemp->cBlueOffset<<3;

    pstColorTemp2.cRedColor= pstColorTemp->cRedColor << 3;
    pstColorTemp2.cGreenColor= pstColorTemp->cGreenColor << 3;
    pstColorTemp2.cBlueColor= pstColorTemp->cBlueColor << 3;

    pstColorTemp2.cRedScaleValue= pstColorTemp->cRedScaleValue;
    pstColorTemp2.cGreenScaleValue= pstColorTemp->cGreenScaleValue;
    pstColorTemp2.cBlueScaleValue= pstColorTemp->cBlueScaleValue;

    MApi_XC_ACE_PicSetPostColorTemp_V02(eWindow, &pstColorTemp2);
}

//*************************************************************************
///-obosolte!! use MApi_XC_ACE_PicSetPostColorTemp_V02 instead
//*************************************************************************
void MApi_XC_ACE_PicSetPostColorTempEx(MS_BOOL eWindow, XC_ACE_color_temp_ex* pstColorTemp)
{
    XC_ACE_color_temp_ex2 pstColorTemp2;
    pstColorTemp2.cRedOffset = pstColorTemp->cRedOffset ;
    pstColorTemp2.cGreenOffset= pstColorTemp->cGreenOffset;
    pstColorTemp2.cBlueOffset= pstColorTemp->cBlueOffset;

    pstColorTemp2.cRedColor= pstColorTemp->cRedColor << 3 ;
    pstColorTemp2.cGreenColor= pstColorTemp->cGreenColor << 3;
    pstColorTemp2.cBlueColor= pstColorTemp->cBlueColor << 3;

    pstColorTemp2.cRedScaleValue= pstColorTemp->cRedScaleValue;
    pstColorTemp2.cGreenScaleValue= pstColorTemp->cGreenScaleValue;
    pstColorTemp2.cBlueScaleValue= pstColorTemp->cBlueScaleValue;

    MApi_XC_ACE_PicSetPostColorTemp_V02(eWindow, &pstColorTemp2);
}

//*************************************************************************
///-set video color
///@param eWindow \b IN: Enable Color temp
///@param bUseYUVSpace \b IN: Is in YUV space
///@param pstColorTemp \b IN: Color temp need to be set
//*************************************************************************
void MApi_XC_ACE_PicSetPostColorTemp2(MS_BOOL eWindow, MS_BOOL bUseYUVSpace, XC_ACE_color_temp* pstColorTemp)
{
        XC_ACE_color_temp tmp = {0};
        // error handle & data prepare
        if(pstColorTemp == NULL)
            return;
        tmp = *pstColorTemp;
        tmp.cRedColor =  tmp.cGreenColor = tmp.cBlueColor = XC_ACE_BYPASS_COLOR_GAIN;

        // bypass ACE color setting
        MApi_XC_ACE_PicSetColorTemp( eWindow, bUseYUVSpace, (XC_ACE_color_temp *)&tmp );

        // use HW color gain setting
        MApi_XC_ACE_PicSetPostColorTemp(eWindow, pstColorTemp);
}

//*************************************************************************
///-set video color ex
///@param eWindow \b IN: Enable Color temp
///@param bUseYUVSpace \b IN: Is in YUV space
///@param pstColorTemp \b IN: Color temp need to be set
//*************************************************************************
void MApi_XC_ACE_PicSetPostColorTemp2Ex(MS_BOOL eWindow, MS_BOOL bUseYUVSpace, XC_ACE_color_temp_ex* pstColorTemp)
{
        XC_ACE_color_temp_ex tmp = {0};

        // error handle & data prepare
        if(pstColorTemp == NULL)
            return;

        tmp = *pstColorTemp;
        tmp.cRedColor =  tmp.cGreenColor = tmp.cBlueColor = XC_ACE_BYPASS_COLOR_GAIN;

        // bypass ACE color setting
        MApi_XC_ACE_PicSetColorTempEx( eWindow, bUseYUVSpace, (XC_ACE_color_temp_ex *)&tmp );

        // use HW color gain setting
        MApi_XC_ACE_PicSetPostColorTempEx(eWindow, pstColorTemp);
}

/******************************************************************************/
/*                     Local                                                  */
/* ****************************************************************************/

E_XC_ACE_RESULT MApi_XC_ACE_GetLibVer(const MSIF_Version **ppVersion)              // Get version
{
    if (!ppVersion)
    {
        return E_XC_ACE_FAIL;
    }

    *ppVersion = &_api_xc_ace_version;
    return E_XC_ACE_OK;
}

const XC_ACE_ApiInfo * MApi_XC_ACE_GetInfo(void)          // Get info from driver
{
    return &_stXC_ACE_ApiInfo;
}

MS_BOOL MApi_XC_ACE_GetStatus(XC_ACE_ApiStatus *pDrvStatus, MS_BOOL bWindow)    // Get DLC current status
{
    if(bWindow == 0)
    {
        pDrvStatus->ACEinit = g_XC_ACE_InitData;
    }
    return TRUE;
}

MS_BOOL MApi_XC_ACE_SetDbgLevel(MS_U16 u16DbgSwitch)
{
    _u16XC_ACEDbgSwitch = u16DbgSwitch;

	//!!! warning fix: remove "variable was set but never used" !!!
	UNUSED(_u16XC_ACEDbgSwitch);

    return TRUE;
}

/******************************************************************************/
/*                   Functions                                                */
/******************************************************************************/

void MApi_XC_ACE_EnableMWE(MS_BOOL bEnable)
{
    bMWEStatus = bEnable;

    XC_ACE_ENTRY();
    MDrv_XC_ACE_MWEEnable(bEnable, _bXC_ACE_LoadFromTable);
    XC_ACE_RETURN();
}

void MApi_XC_ACE_MWESetMirrorMode( E_XC_ACE_MWE_MIRROR_TYPE eMirrorMode )
{
    _enMirrorMode = eMirrorMode;
}

void MApi_XC_ACE_MWECloneVisualEffect()
{
    XC_ACE_ENTRY();
    MDrv_XC_ACE_MWECloneVisualEffect();
    XC_ACE_RETURN();
}

void MApi_XC_ACE_3DClonePQMap(XC_ACE_WEAVETYPE enWeaveType)
{
    XC_ACE_ENTRY();
    MDrv_ACE_3DClonePQMap(((enWeaveType & E_ACE_WEAVETYPE_H) == E_ACE_WEAVETYPE_H), ((enWeaveType & E_ACE_WEAVETYPE_V) == E_ACE_WEAVETYPE_V));
    XC_ACE_RETURN();
}

MS_BOOL MApi_XC_ACE_MWESetRegTable(XC_ACE_TAB_Info *pMWETable)
{
    MS_U32 u32Addr;
    MS_U8  u8Mask;
    MS_U8  u8Value;
    MS_U16 i;
    MS_U8  u8DoNotSet;
    if((NULL == pMWETable->pTable) || (pMWETable->u8TabRows < 2))
    {
        XC_ACE_DBG_MSG("Invalid Table Format!\n");
        return FALSE;
    }

    if((ACE_TABINFO_REG_ADDR_SIZE + ACE_TABINFO_REG_MASK_SIZE + pMWETable->u8TabIdx * ACE_TABINFO_REG_DATA_SIZE) >= pMWETable->u8TabCols)
    {
        XC_ACE_DBG_MSG("Invalid Table Format!\n");
        return FALSE;
    }

    for(i=0; i < pMWETable->u8TabRows - 1; i++)
    {
        u32Addr = (MS_U32)((pMWETable->pTable[0] << 8) + pMWETable->pTable[1]);
        u8Mask  = pMWETable->pTable[ACE_TABINFO_REG_ADDR_SIZE];
        u8DoNotSet = pMWETable->pTable[ACE_TABINFO_REG_ADDR_SIZE + ACE_TABINFO_REG_MASK_SIZE + pMWETable->u8TabIdx * ACE_TABINFO_REG_DATA_SIZE];
        u8Value = pMWETable->pTable[ACE_TABINFO_REG_ADDR_SIZE + ACE_TABINFO_REG_MASK_SIZE + pMWETable->u8TabIdx * ACE_TABINFO_REG_DATA_SIZE + 1];
        XC_ACE_DBG_MSG("[addr=%04lx, msk=%02x, enb=%02x, val=%02x]\n", u32Addr, u8Mask, u8DoNotSet, u8Value);
        if(u32Addr == 0xFFFF) //check end of table
        {
            XC_ACE_DBG_MSG("end of table!\n");
            break;
        }
        if(!u8DoNotSet)
        {
            XC_ACE_ENTRY();
            MDrv_XC_ACE_MWESetRegRow(u32Addr, u8Value, u8Mask);
            XC_ACE_RETURN();
        }
        pMWETable->pTable += pMWETable->u8TabCols; //next
    }
    _bXC_ACE_LoadFromTable = TRUE;
    return TRUE;
}

void MApi_XC_ACE_MWESetDispWin(MS_U16 u16MWE_Disp_Hstart, MS_U16 u16MWE_Disp_Vstart, MS_U16 u16MWE_Disp_Width, MS_U16 u16MWE_Disp_Height)
{
    g_XC_ACE_InitData.u16MWE_Disp_Hstart = u16MWE_Disp_Hstart;
    g_XC_ACE_InitData.u16MWE_Disp_Vstart = u16MWE_Disp_Vstart;
    g_XC_ACE_InitData.u16MWE_Disp_Width  = u16MWE_Disp_Width;
    g_XC_ACE_InitData.u16MWE_Disp_Height = u16MWE_Disp_Height;

}

MS_BOOL MApi_XC_ACE_MWEStatus(void)
{
    return bMWEStatus;
}

void MApi_XC_ACE_MWEFuncSel( MS_BOOL eWindow, E_XC_ACE_MWE_FUNC  mwe_func)
{
    MS_U8 u8border_color;
    MS_U16 u16border_l, u16border_r, u16border_t, u16border_d;
    MS_BOOL border_en;

    if (mwe_func == E_XC_ACE_MWE_MODE_OFF && _MWEFunc == E_XC_ACE_MWE_MODE_OFF)
    {
        return;
    }

    XC_ACE_ENTRY();

	_MWEFunc = mwe_func;
    u16border_l = 0x00;
    u16border_r = 0x00;
    u16border_t = 0x00;
    u16border_d = 0x00;

    switch (_MWEFunc)
    {
    default:
    case E_XC_ACE_MWE_MODE_OFF:
        gMWERect.Xsize = 0;
        gMWERect.Ysize = 0;
        gMWERect.Xpos = 0;
        gMWERect.Ypos = 0;
        u8border_color = MWE_BORDER_COLOR_BLACK;
        border_en = DISABLE;
        break;

    case E_XC_ACE_MWE_MODE_H_SCAN:
    case E_XC_ACE_MWE_MODE_H_SPLIT:
    	//printf(" [MWE] H-SPLIT-RIGHT/H-SCAN Function \n");

    	if(_enMirrorMode == E_XC_ACE_MWE_MIRROR_NORMAL ||
    	   _enMirrorMode == E_XC_ACE_MWE_MIRROR_V_ONLY)
    	{
    	    gMWERect.Xpos = g_XC_ACE_InitData.u16MWE_Disp_Hstart;
    	}
    	else
    	{
            gMWERect.Xpos = g_XC_ACE_InitData.u16MWE_Disp_Hstart+g_XC_ACE_InitData.u16MWE_Disp_Width - gMWERect.Xsize;
    	}

        gMWERect.Xsize = g_XC_ACE_InitData.u16MWE_Disp_Width/2;
        gMWERect.Ysize = g_XC_ACE_InitData.u16MWE_Disp_Height;
        gMWERect.Ypos = g_XC_ACE_InitData.u16MWE_Disp_Vstart+g_XC_ACE_InitData.u16MWE_Disp_Height - gMWERect.Ysize;
        u8border_color = MWE_BORDER_COLOR_BLUE;
        u16border_l = 0x02;
        border_en = ENABLE;
        break;

    case E_XC_ACE_MWE_MODE_H_SPLIT_LEFT:
    	//printf(" [MWE] H-SPLIT-LEFT\n");
        gMWERect.Xsize = g_XC_ACE_InitData.u16MWE_Disp_Width / 2;
        gMWERect.Ysize = g_XC_ACE_InitData.u16MWE_Disp_Height;
        gMWERect.Xpos = g_XC_ACE_InitData.u16MWE_Disp_Hstart;
        gMWERect.Ypos = g_XC_ACE_InitData.u16MWE_Disp_Vstart;
        u8border_color = MWE_BORDER_COLOR_BLUE;
        u16border_l = 0x02;
        border_en = ENABLE;
        break;

    case E_XC_ACE_MWE_MODE_MOVE:
        gMWERect.Xsize = g_XC_ACE_InitData.u16MWE_Disp_Width/3;
        gMWERect.Ysize =g_XC_ACE_InitData.u16MWE_Disp_Height/3;
        gMWERect.Xpos = g_XC_ACE_InitData.u16MWE_Disp_Hstart+g_XC_ACE_InitData.u16MWE_Disp_Width - gMWERect.Xsize;
        gMWERect.Ypos = g_XC_ACE_InitData.u16MWE_Disp_Vstart+g_XC_ACE_InitData.u16MWE_Disp_Height - gMWERect.Ysize;
        u8border_color = MWE_BORDER_COLOR_BLUE;
        u16border_l = 0x11;
        u16border_r = 0x11;
        u16border_t = 0x11;
        u16border_d = 0x11;
        border_en = ENABLE;
        break;

    case E_XC_ACE_MWE_MODE_ZOOM:
        gMWERect.Xsize = g_XC_ACE_InitData.u16MWE_Disp_Width/2;
        gMWERect.Ysize = g_XC_ACE_InitData.u16MWE_Disp_Height/2;
        gMWERect.Xpos = g_XC_ACE_InitData.u16MWE_Disp_Hstart+g_XC_ACE_InitData.u16MWE_Disp_Width - gMWERect.Xsize;
        gMWERect.Ypos = g_XC_ACE_InitData.u16MWE_Disp_Vstart+g_XC_ACE_InitData.u16MWE_Disp_Height - gMWERect.Ysize;
        u8border_color = MWE_BORDER_COLOR_BLUE;
        border_en = DISABLE;
        break;
    }

    //ToDo: need to use drvPQ to adjsut picture quality
    MDrv_XC_ACE_MWESetWin(&gMWERect);
    MDrv_XC_ACE_MWESetBorder(eWindow, border_en, u8border_color,
        u16border_l, u16border_r, u16border_t, u16border_d);

    XC_ACE_RETURN();
}

static void MApi_XC_ACE_MWEScan(void)
{
    MS_U32 xpos;
    MS_U32 MWE_MAX_XPOS, MWE_MIN_XPOS;
    MWE_RECT *pRect;

    pRect = &gMWERect;

    MWE_MIN_XPOS = g_XC_ACE_InitData.u16MWE_Disp_Hstart;
    MWE_MAX_XPOS = MWE_MIN_XPOS + g_XC_ACE_InitData.u16MWE_Disp_Width -1;
    xpos  = pRect->Xpos;

    if(_MWEFlag& MWE_MOVE_RIGHT)
    {
        if( (xpos  + MWE_H_STEP) > ((MS_U32)MWE_MAX_XPOS))
        {
            xpos = MWE_MAX_XPOS;
            _MWEFlag &= ~MWE_MOVE_RIGHT;
        }
        else
        {
            xpos += MWE_H_STEP;
        }
    }
    else
    {
        if(xpos > MWE_H_STEP)
        {
            if ( (xpos - MWE_H_STEP) > ((MS_U32)MWE_MIN_XPOS))
            {
                xpos -= MWE_H_STEP;
            }
            else
            {
                xpos = MWE_MIN_XPOS;
                _MWEFlag |= MWE_MOVE_RIGHT;
            }
        }
        else
        {
            _MWEFlag |= MWE_MOVE_RIGHT;
        }
    }

    if (xpos > pRect->Xpos)
    {
        pRect->Xsize -= (xpos-pRect->Xpos);
    }
    else
    {
        pRect->Xsize += (pRect->Xpos-xpos);
    }
    pRect->Xpos = xpos;

    //printf(" HScan %lu, %lu\n", pRect->Xpos, pRect->Xsize);

}
void MApi_XC_ACE_MWEHandle( void )
{
    if (_MWEFunc != E_XC_ACE_MWE_MODE_H_SCAN)
        return;

    if ((MsOS_GetSystemTime()-_MWETimer) < MWE_TIMER)
        return;
    _MWETimer = MsOS_GetSystemTime();




    switch (_MWEFunc)
    {
    case E_XC_ACE_MWE_MODE_H_SCAN:
        MApi_XC_ACE_MWEScan();
        break;
    case E_XC_ACE_MWE_MODE_MOVE:
    case E_XC_ACE_MWE_MODE_ZOOM:
    default:
        return;
    }

    XC_ACE_ENTRY();

    //MApi_XC_WaitOutputVSync(1, 50);
    MDrv_XC_ACE_MWESetWin(&gMWERect);

    XC_ACE_RETURN();

}

MS_BOOL MApi_XC_ACE_Set_IHC_SRAM(MS_U8 *pBuf, MS_U16 u16ByteSize)
{
    MS_BOOL bRet;
    XC_ACE_ENTRY();

    bRet = MDrv_XC_ACE_Set_IHC_SRAM(pBuf, u16ByteSize);

    XC_ACE_RETURN();

    return  bRet;
}

MS_BOOL MApi_XC_ACE_Set_ICC_SRAM(MS_U16 *pBuf, MS_U16 u16ByteSize)
{
    MS_BOOL bRet;
    XC_ACE_ENTRY();

    bRet = MDrv_XC_ACE_Set_ICC_SRAM(pBuf, u16ByteSize);

    XC_ACE_RETURN();

    return  bRet;
}

////////////////////////////////////////////////////////////////////////////////
//
//  DynamicNR start
//
////////////////////////////////////////////////////////////////////////////////
#define NR_DBG(x)   //x

#define DYNAMIC_NR_TBL_REG_NUM                  8
#define DYNAMICNR_DEFLICKER_PRECISION_SHFIT     64UL
#define DYNAMICNR_DEFLICK_TOTAL                 32UL
#define DYNAMICNR_DEFLICK_STABLE_CNT            0x10


MS_U8  g_u8MotionStableCnt;
MS_U8  g_u8LumaStableCnt;
MS_U8  g_u8FlickStableCnt;

MS_U16 MApi_XC_ACE_DNR_Get_PrecisionShift(void)
{
    return DYNAMICNR_DEFLICKER_PRECISION_SHFIT;
}

void MApi_XC_ACE_DNR_Blending_NRTbl(
    MS_U8 *pu8NewNR,
    MS_U8 u8Weight,
    MS_U8 u8Step,
    MS_U16 *pu16PreY0,
    MS_U16 *pu16PreY1,
    MS_U8 *pu8NRTBL)
{
    MS_U8 i;
    MS_U16 u16TmpBlend[2];

    NR_DBG(printk("CurNR:"));
    for(i=0; i<DYNAMIC_NR_TBL_REG_NUM; i++)
    {
        u16TmpBlend[0] = (MS_U16)(((((MS_U32)u8Weight * ((MS_U32)pu8NewNR[i] & 0x0FL)) * DYNAMICNR_DEFLICKER_PRECISION_SHFIT) +
                    ((MS_U32)DYNAMICNR_DEFLICK_TOTAL - (MS_U32)u8Weight) * (MS_U32)pu16PreY0[i]) / DYNAMICNR_DEFLICK_TOTAL);

        u16TmpBlend[1] = (MS_U16)(((((MS_U32)u8Weight * (((MS_U32)pu8NewNR[i] & 0xF0L)>>4)) * DYNAMICNR_DEFLICKER_PRECISION_SHFIT) +
                    ((MS_U32)DYNAMICNR_DEFLICK_TOTAL - (MS_U32)u8Weight) * (MS_U32)pu16PreY1[i]) / DYNAMICNR_DEFLICK_TOTAL);


        if(u16TmpBlend[0] > pu16PreY0[i])
        {
            if( (u16TmpBlend[0] - (MS_U16)pu16PreY0[i]) > ((MS_U16)DYNAMICNR_DEFLICKER_PRECISION_SHFIT * (MS_U16) u8Step))
                u16TmpBlend[0]  = pu16PreY0[i] + ((MS_U16)DYNAMICNR_DEFLICKER_PRECISION_SHFIT * (MS_U16) u8Step);
        }
        else if(u16TmpBlend[0] < (MS_U16)pu16PreY0[i])
        {
            if( ((MS_U16)pu16PreY0[i] - u16TmpBlend[0]) > ((MS_U16)DYNAMICNR_DEFLICKER_PRECISION_SHFIT * (MS_U16) u8Step))
                u16TmpBlend[0]  = pu16PreY0[i] - ((MS_U16)DYNAMICNR_DEFLICKER_PRECISION_SHFIT * (MS_U16) u8Step);
        }
        pu16PreY0[i] = u16TmpBlend[0];

        if(u16TmpBlend[1] > pu16PreY1[i])
        {
            if( (u16TmpBlend[1] - (MS_U16)pu16PreY1[i]) > ((MS_U16)DYNAMICNR_DEFLICKER_PRECISION_SHFIT * (MS_U16) u8Step))
                u16TmpBlend[1]  = pu16PreY1[i] + ((MS_U16)DYNAMICNR_DEFLICKER_PRECISION_SHFIT * (MS_U16) u8Step);
        }
        else if(u16TmpBlend[1] < (MS_U16)pu16PreY1[i])
        {
            if( ((MS_U16)pu16PreY1[i] - u16TmpBlend[1]) > ((MS_U16)DYNAMICNR_DEFLICKER_PRECISION_SHFIT * (MS_U16) u8Step))
                u16TmpBlend[1]  = pu16PreY1[i] - ((MS_U16)DYNAMICNR_DEFLICKER_PRECISION_SHFIT * (MS_U16) u8Step);
        }
        pu16PreY1[i] = u16TmpBlend[1];
        pu8NRTBL[i] = (MS_U8)(((u16TmpBlend[1] / DYNAMICNR_DEFLICKER_PRECISION_SHFIT)<<4) | (u16TmpBlend[0] / DYNAMICNR_DEFLICKER_PRECISION_SHFIT));

        NR_DBG(printk(" %02bx, ", pu8NRTBL[i]));
    }
    NR_DBG(printk("\n"));
}

MS_U8 MApi_XC_ACE_DNR_Blending(MS_U8 u8NewItem, MS_U16 u16PreItem, MS_U8 u8Weight)
{
    MS_U8 u8Blend;

    u8Blend = (MS_U8)(((((MS_U32)u8Weight * (MS_U32)u8NewItem)) +
                ((MS_U32)DYNAMICNR_DEFLICK_TOTAL - (MS_U32)u8Weight) * (MS_U32)u16PreItem) / DYNAMICNR_DEFLICK_TOTAL);
    return u8Blend;
}

MS_U16 MApi_XC_ACE_DNR_Blending_MISC(
    MS_U8 u8NewItem,
    MS_U16 u16PreItem,
    MS_U8 u8Weight,
    MS_U8 u8Step)
{
    MS_U16 u16Blend;

    u16Blend = (MS_U16)( ((((MS_U32)u8Weight * (MS_U32)u8NewItem) * DYNAMICNR_DEFLICKER_PRECISION_SHFIT) +
                 ((MS_U32)DYNAMICNR_DEFLICK_TOTAL - (MS_U32)u8Weight) * (MS_U32)u16PreItem) / DYNAMICNR_DEFLICK_TOTAL );


    if(u16Blend > u16PreItem)
    {
        if( (u16Blend - u16PreItem) > ((MS_U16)DYNAMICNR_DEFLICKER_PRECISION_SHFIT * (MS_U16) u8Step))
            u16Blend  = u16PreItem + ((MS_U16)DYNAMICNR_DEFLICKER_PRECISION_SHFIT * (MS_U16) u8Step);
    }
    else if(u16Blend < u16PreItem)
    {
        if( ((MS_U16)u16PreItem - u16Blend) > ((MS_U16)DYNAMICNR_DEFLICKER_PRECISION_SHFIT * (MS_U16) u8Step))
            u16Blend  = u16PreItem - ((MS_U16)DYNAMICNR_DEFLICKER_PRECISION_SHFIT * (MS_U16) u8Step);
    }

    return u16Blend;
}


MS_U8 MApi_XC_ACE_DNR_GetMotion(void)
{
    MS_U8 u8Ret;

    XC_ACE_ENTRY();

    u8Ret =  MDrv_XC_ACE_DNR_GetMotion();

    XC_ACE_RETURN();

    return u8Ret;
}


MS_U8 MApi_XC_ACE_DNR_GetMotion_Weight(
    MS_U8 u8CurMotion,
    MS_U8 u8PreMotion,
    MS_U8 u8CurMotionLvl,
    MS_U8 u8PreMotionLvl,
    MS_U8 u8DeFlick_Thre,
    MS_U8 u8DeFilick_Step)
{
    MS_U8 u8Weight;

    if(u8CurMotion > u8PreMotion)
    {
        if((u8CurMotion - u8PreMotion) < u8DeFlick_Thre)
        {
            if((u8CurMotionLvl - u8PreMotionLvl) <= 1)
            {
                if(g_u8MotionStableCnt != 0xFF)
                    g_u8MotionStableCnt++;
            }
            else
            {
                MApi_XC_ACE_DNR_Init_Motion();
            }
            u8Weight = (g_u8MotionStableCnt > DYNAMICNR_DEFLICK_STABLE_CNT) ?
                       DYNAMICNR_DEFLICK_TOTAL : u8DeFlick_Thre;
        }
        else
        {
            MApi_XC_ACE_DNR_Init_Motion();
            u8Weight = DYNAMICNR_DEFLICK_TOTAL;
        }
    }
    else
    {
        if((u8PreMotion - u8CurMotion) < u8DeFilick_Step)
        {
            if((u8CurMotionLvl - u8PreMotionLvl) <= 1)
            {
                if(g_u8MotionStableCnt != 0xFF)
                    g_u8MotionStableCnt++;
            }
            else
            {
            	  MApi_XC_ACE_DNR_Init_Motion();
            }
            u8Weight = (g_u8MotionStableCnt > DYNAMICNR_DEFLICK_STABLE_CNT) ?
                       DYNAMICNR_DEFLICK_TOTAL : u8DeFilick_Step;
        }
        else
        {
            MApi_XC_ACE_DNR_Init_Motion();
            u8Weight = DYNAMICNR_DEFLICK_TOTAL;
        }
    }

    if(u8Weight > DYNAMICNR_DEFLICK_TOTAL)
        u8Weight = DYNAMICNR_DEFLICK_TOTAL;

    return u8Weight;
}


MS_U8  MApi_XC_ACE_DNR_GetLuma_Weight(
    MS_U8 u8CurAvgLuma,
    MS_U8 u8PreAvgLuam,
    MS_U8 u8CurLumaLvl,
    MS_U8 u8PreLumaLvl,
    MS_U8 u8DeFlick_Th,
    MS_U8 u8DeFlick_Step)
{
    MS_U8 u8Weight;

    // Blending for Coring and SNR
    if(u8CurAvgLuma > u8PreAvgLuam)
    {
        if((u8CurAvgLuma - u8PreAvgLuam) < u8DeFlick_Th)
        {
            if((u8CurLumaLvl - u8PreLumaLvl)<=1)
            {
                if(g_u8LumaStableCnt != 0xFF)
                    g_u8LumaStableCnt++;
            }
            else
            {
                MApi_XC_ACE_DNR_Init_Luma();
            }
            u8Weight = (g_u8LumaStableCnt > DYNAMICNR_DEFLICK_STABLE_CNT) ?
                       DYNAMICNR_DEFLICK_TOTAL :
                       u8DeFlick_Step;
        }
        else
        {
            MApi_XC_ACE_DNR_Init_Luma();
            u8Weight = DYNAMICNR_DEFLICK_TOTAL;
        }
    }
    else
    {
        if((u8PreAvgLuam - u8CurAvgLuma) < u8DeFlick_Th)
        {
            if((u8CurLumaLvl - u8PreLumaLvl)<=1)
            {
                if(g_u8LumaStableCnt != 0xFF)
                    g_u8LumaStableCnt++;
            }
            else
            {
                MApi_XC_ACE_DNR_Init_Luma();
            }
            u8Weight = (g_u8LumaStableCnt > DYNAMICNR_DEFLICK_STABLE_CNT) ?
                       DYNAMICNR_DEFLICK_TOTAL :
                       u8DeFlick_Step;
        }
        else
        {
            MApi_XC_ACE_DNR_Init_Luma();
            u8Weight = DYNAMICNR_DEFLICK_TOTAL;
        }
    }

    return u8Weight;
}

MS_U8 MApi_XC_ACE_DNR_GetNoise_Weight(
    MS_U8 u8CurNoise,
    MS_U8 u8PreNoise,
    MS_U8 u8Range,
    MS_U8 u8DeFlick_Th,
    MS_U8 u8DeFlick_Step)
{
    MS_U8 u8Weight;

    if(u8CurNoise > u8PreNoise)
    {
        if((u8CurNoise - u8PreNoise) < (u8DeFlick_Th))
        {
            g_u8FlickStableCnt =
                ((u8CurNoise - u8PreNoise) < u8Range) ?
                g_u8FlickStableCnt + 1 : 0;

            u8Weight = (g_u8FlickStableCnt == DYNAMICNR_DEFLICK_STABLE_CNT) ?
                        DYNAMICNR_DEFLICK_TOTAL :
                        u8DeFlick_Step;
        }
        else
        {
        	  MApi_XC_ACE_DNR_Init_Noise();
            u8Weight = DYNAMICNR_DEFLICK_TOTAL;
        }
    }
    else
    {
        if((u8PreNoise - u8CurNoise) < (u8DeFlick_Th))
        {
            g_u8FlickStableCnt =
                ((u8CurNoise - u8PreNoise) < u8Range) ?
                g_u8FlickStableCnt + 1 : 0;
            u8Weight = (g_u8FlickStableCnt == DYNAMICNR_DEFLICK_STABLE_CNT) ?
                        DYNAMICNR_DEFLICK_TOTAL :
                        u8DeFlick_Step;
        }
        else
        {
            MApi_XC_ACE_DNR_Init_Noise();
            u8Weight = DYNAMICNR_DEFLICK_TOTAL;
        }
    }

    if(u8Weight > DYNAMICNR_DEFLICK_TOTAL)
        u8Weight = DYNAMICNR_DEFLICK_TOTAL;
    if(u8Weight == 0)
        u8Weight = 1;

    return u8Weight;
}

void MApi_XC_ACE_DNR_Init_Motion(void)
{
    g_u8MotionStableCnt = 0;
}



void MApi_XC_ACE_DNR_Init_Luma(void)
{
    g_u8LumaStableCnt = 0;
}

void MApi_XC_ACE_DNR_Init_Noise(void)
{
    g_u8FlickStableCnt = 0;
}


MS_U8 MApi_XC_ACE_DNR_GetParam(MS_BOOL eWindow, XC_ACE_DNR_Param eParam)
{
    MS_U8 u8val;

    XC_ACE_ENTRY();

    switch(eParam)
    {
    case E_ACE_DNR_PEAKING_CORING_THRESHOLD:
        u8val = MDrv_ACE_DNR_GetCoringThreshold(eWindow);
        break;

    case E_ACE_DNR_SHARPNESS_ADJUST:
        u8val = MDrv_ACE_DNR_GetSharpnessAdjust(eWindow);
        break;

    case E_ACE_DNR_GUASSIN_SNR_THRESHOLD:
        u8val = MDrv_ACE_DNR_GetGuassin_SNR_Threshold(eWindow);
        break;

    default:
        //printf("XC_ACE_DNR: unknown get param %u\n", eParam);
        u8val = 0;
        break;
    }

    XC_ACE_RETURN();

    return u8val;
}

void MApi_XC_ACE_DNR_SetParam(MS_BOOL eWindow, XC_ACE_DNR_Param eParam, MS_U16 u16val)
{
    XC_ACE_ENTRY();

    switch(eParam)
    {
    case E_ACE_DNR_PEAKING_BANK_CORING:
        MDrv_ACE_DNR_SetBank_Coring(eWindow, u16val);
        break;

    case E_ACE_DNR_GUASSIN_SNR_THRESHOLD:
        MDrv_ACE_DNR_SetGuassin_SNR_Threshold(eWindow, u16val);
        break;

    case E_ACE_DNR_PEAKING_CORING_THRESHOLD:
        MDrv_ACE_DNR_SetCoringThreshold(eWindow, u16val);
        break;

    case E_ACE_DNR_SHARPNESS_ADJUST:
        MDrv_ACE_DNR_SetSharpnessAdjust(eWindow, u16val);
        break;

    case E_ACE_DNR_NM_V:
        MDrv_ACE_DNR_SetNM_V(eWindow, u16val);
        break;

    case E_ACE_DNR_GNR_0:
        MDrv_ACE_DNR_SetGNR_0(eWindow, u16val);
        break;

    case E_ACE_DNR_GNR_1:
        MDrv_ACE_DNR_SetGNR_1(eWindow, u16val);
        break;

    case E_ACE_DNR_CP:
        MDrv_ACE_DNR_SetCP(eWindow, u16val);
        break;

    case E_ACE_DNR_DP:
        MDrv_ACE_DNR_SetDP(eWindow, u16val);
        break;

    case E_ACE_DNR_NM_H_0:
        MDrv_ACE_DNR_SetNM_H_0(eWindow, u16val);
        break;

    case E_ACE_DNR_NM_H_1:
        MDrv_ACE_DNR_SetNM_H_1(eWindow, u16val);
        break;

    case E_ACE_DNR_GRAY_GROUND_GAIN:
        MDrv_ACE_DNR_SetGray_Ground_Gain(u16val);
        break;

    case E_ACE_DNR_GRAY_GROUND_EN:
        MDrv_ACE_DNR_SetGray_Ground_En(eWindow, u16val);
        break;

    case E_ACE_DNR_SC_CORING:
        MDrv_ACE_DNR_SetSC_Coring(eWindow, u16val);
        break;

    case E_ACE_DNR_SPIKE_NR_0:
        MDrv_ACE_DNR_SetSpikeNR_0(u16val);
        break;

    case E_ACE_DNR_SPIKE_NR_1:
        MDrv_ACE_DNR_SetSpikeNR_1(u16val);
        break;

    case E_ACE_DNR_SNR_NM:
        MDrv_ACE_DNR_SetSNR_NM(u16val);
        break;

    default:
        //printf("XC_ACE_DNR: unknown set Param %u\n", eParam);
        break;
    }

    XC_ACE_RETURN();

}

void MApi_XC_ACE_DNR_SetNRTbl(MS_U8 *pu8Tbl)
{
    XC_ACE_ENTRY();
    MDrv_ACE_DNR_SetNRTable(pu8Tbl);
    XC_ACE_RETURN();
}

void MApi_XC_ACE_ColorCorrectionTable( MS_BOOL bScalerWin, MS_S16 *psColorCorrectionTable )
{
    XC_ACE_ENTRY();
    MDrv_ACE_ColorCorrectionTable( bScalerWin, psColorCorrectionTable );
    XC_ACE_RETURN();
}

void MApi_XC_ACE_PCsRGBTable( MS_BOOL bScalerWin, MS_S16 *psPCsRGBTable )
{
    XC_ACE_ENTRY();
    MDrv_ACE_PCsRGBTable(bScalerWin, psPCsRGBTable);
    XC_ACE_RETURN();
}

void MApi_XC_ACE_SetColorMatrixControl( MS_BOOL bScalerWin, MS_BOOL bEnable )
{
    XC_ACE_ENTRY();
    MDrv_ACE_SetColorMatrixControl(bScalerWin, bEnable );
    XC_ACE_RETURN();
}

void MApi_XC_ACE_SetRBChannelRange( MS_BOOL bScalerWin, MS_BOOL bRange )
{
    XC_ACE_ENTRY();
    MDrv_ACE_SetRBChannelRange(bScalerWin, bRange );
    XC_ACE_RETURN();
}

#undef _API_XC_ACE_C_
#endif  // _API_XC_ACE_C_

