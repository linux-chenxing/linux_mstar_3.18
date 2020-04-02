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
// ("MStar Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _API_XC_DLC_C_
#define _API_XC_DLC_C_

/******************************************************************************/
/*                   Header Files                                             */
/* ****************************************************************************/
// Common Definition
#ifdef MSOS_TYPE_LINUX_KERNEL
#include <linux/string.h>
#include <linux/wait.h>
#include <linux/irqreturn.h>
#else
#include <string.h>
#endif

#include "MsCommon.h"
#include "MsVersion.h"
#include "drvDLC.h"
#include "drvDLC_Cus.h"
#include "drvDBC_Cus.h"
#include "apiXC_Dlc.h"
#include "apiXC_DBC.h"
// Internal Definition
#include "drvMMIO.h"


/******************************************************************************/
/*                      Debug information                                      */
/******************************************************************************/

#define XC_DCL_DBG 0

#if XC_DCL_DBG
#define XC_DLC_DBG_MSG(fmt,...) \
    MS_DEBUG_MSG(do{printf( "[PNL_DBG_MSG]: %s: %d \n"  fmt, __FUNCTION__, __LINE__, ## __VA_ARGS__);} while(0))

#define XC_DLC_PRINT_VAR(var)  \
    do{\
        XC_DLC_DBG_MSG("%30s: %d\n", #var, (var));\
        }while(0)

#define XC_DLC_PRINT_FUN(fun)  \
    do{\
        XC_DLC_DBG_MSG("\n");\
        XC_DLC_DBG_MSG("**************************************************\n");\
        XC_DLC_DBG_MSG("*\t\t%s \t\t\n", #fun);\
        XC_DLC_DBG_MSG("**************************************************\n");\
        fun;\
        XC_DLC_DBG_MSG("*                                                *\n");\
        XC_DLC_DBG_MSG("*                                                *\n");\
        XC_DLC_DBG_MSG("**************************************************\n");\
        }while(0)

#else
  #define XC_DLC_DBG_MSG(str, ...)
  #define XC_DLC_PRINT_VAR(var)
  #define XC_DLC_PRINT_FUN(fun) fun
#endif

#define DLC_MUTEX
#define DLC_MUTEX_DBG    0

#ifdef DLC_MUTEX
    #if(DLC_MUTEX_DBG)
    #define Dlc_FuncEnter() \
        printf("1,==========================\n");                              \
        printf("[%s][%s][%06d]\n",__FILE__,__FUNCTION__,__LINE__);    \
        if(!MsOS_ObtainMutex(_DLC_Mutex, MSOS_WAIT_FOREVER)) \
        {                                                     \
            printf("==========================\n");            \
            printf("[%s][%s][%06d] Mutex taking timeout\n",__FILE__,__FUNCTION__,__LINE__);    \
        }
    #define Dlc_FuncExit()    \
        printf("0,==========================\n");                              \
        printf("[%s][%s][%06d] \n",__FILE__,__FUNCTION__,__LINE__);    \
        MsOS_ReleaseMutex(_DLC_Mutex);
    #else
    #define Dlc_FuncEnter() \
        if(!MsOS_ObtainMutex(_DLC_Mutex, MSOS_WAIT_FOREVER)) \
        {                                                     \
        }

    #define Dlc_FuncExit()    \
        MsOS_ReleaseMutex(_DLC_Mutex);

    #endif
#else
    #define Dlc_FuncEnter() while(0)
    #define Dlc_FuncExit()  while(0)
#endif

#if defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_LINUX_KERNEL)
static MSIF_Version _api_xc_dlc_version = {
    { XC_DLC_API_VERSION },
};

#else
static MSIF_Version _api_xc_dlc_version = {
    .DDI = { XC_DLC_API_VERSION },
};
#endif


/******************************************************************************/
/*                   Local Function Prototypes                                */
/******************************************************************************/

/******************************************************************************/
/*                   Functions                                                */
/******************************************************************************/

/******************************************************************************/
/*                   Constant                                                 */
/******************************************************************************/

#define DLC_DBG(x)     // x

static const XC_DLC_ApiInfo _stXC_DLC_ApiInfo = {0};
static MS_U16 _u16XC_DLCDbgSwitch = 0;
static MS_BOOL g_bDBCEnable;

/******************************************************************************/
// Initialize DLC
/******************************************************************************/
#if defined(MSOS_TYPE_CE)

#pragma data_seg(".DLCApi")
    MS_S32 _DLC_Mutex = -1;
#pragma data_seg()
#pragma comment(linker,"/SECTION:.DLCApi,RWS")

#else

MS_S32 _DLC_Mutex = -1;

#endif



static MS_BOOL g_bDLC_CGC_ready = FALSE;
static MS_BOOL g_bDLC_DBC_ready = FALSE;
static MS_BOOL g_bDLC_CGC_CGainPQCom;
static MS_BOOL g_bDLC_CGC_YGainPQCom;
static MS_U8   g_u8DLC_CGC_CGain;
static MS_U8   g_u8DLC_CGC_YGain;
static MS_U8   g_u8DLC_CGC_YGain_Backup;
static MS_U8   g_u8DLC_CGC_CGain_Backup;

void MApi_XC_DLC_CGC_ResetCGain(void)
{
    Dlc_FuncEnter();
    g_u8DLC_CGC_CGain_Backup = MDrv_DLC_CGC_GetCGain();
    MDrv_DLC_CGC_ResetCGain();
    Dlc_FuncExit();
}

void MApi_XC_DLC_CGC_CheckCGainInPQCom(void)
{
    Dlc_FuncEnter();
    g_u8DLC_CGC_CGain = MDrv_DLC_CGC_GetCGain();
    if((g_u8DLC_CGC_CGain == 0xFF))
    {
        g_bDLC_CGC_CGainPQCom = FALSE;
        MDrv_DLC_CGC_SetCGain(g_u8DLC_CGC_CGain_Backup);
        g_u8DLC_CGC_CGain = g_u8DLC_CGC_CGain_Backup;
    }
    else
    {
        g_bDLC_CGC_CGainPQCom = TRUE;
#if 0
        if(g_u8DLC_CGC_CGain == 0x00)
        {
            g_bDLC_CGC_CGainPQCom = TRUE;
            MDrv_DLC_CGC_SetCGain(g_u8DLC_CGC_CGain_Backup);
            g_u8DLC_CGC_CGain = g_u8DLC_CGC_CGain_Backup;
        }
        else
        {
        g_bDLC_CGC_CGainPQCom = FALSE;
        }
#endif
    }
    Dlc_FuncExit();
}

void MApi_XC_DLC_CGC_ResetYGain(void)
{
    Dlc_FuncEnter();
    g_u8DLC_CGC_YGain_Backup = MDrv_DLC_CGC_GetYGain();
    MDrv_DLC_CGC_ResetYGain();
    Dlc_FuncExit();
}

void MApi_XC_DLC_CGC_CheckYGainInPQCom(void)
{
    Dlc_FuncEnter();
    g_u8DLC_CGC_YGain = MDrv_DLC_CGC_GetYGain();
    if((g_u8DLC_CGC_YGain == 0xFF))
    {
        g_bDLC_CGC_YGainPQCom = FALSE;
        MDrv_DLC_CGC_SetYGain(g_u8DLC_CGC_YGain_Backup);
        g_u8DLC_CGC_YGain = g_u8DLC_CGC_YGain_Backup;
    }
    else
    {
        g_bDLC_CGC_YGainPQCom = TRUE;
#if 0
        if(g_u8DLC_CGC_YGain == 0x00)
        {
            g_bDLC_CGC_YGainPQCom = TRUE;
            MDrv_DLC_CGC_SetYGain(g_u8DLC_CGC_YGain_Backup);
            g_u8DLC_CGC_YGain = g_u8DLC_CGC_YGain_Backup;
        }
        else
        {
        g_bDLC_CGC_YGainPQCom = FALSE;
        }
#endif
    }
    Dlc_FuncExit();
}

void MApi_XC_DLC_CGC_Reset(void)
{
    g_bDLC_CGC_ready = FALSE;
}

void MApi_XC_DLC_CGC_Init(void)
{
    Dlc_FuncEnter();
    if(g_bDLC_CGC_CGainPQCom)
    {
        msDlc_CGC_SetGain(g_u8DLC_CGC_CGain); //MDrv_DLC_CGC_SetCGain(g_u8DLC_CGC_CGain);
    }
    g_bDLC_CGC_ready = TRUE;
    MDrv_DLC_CGC_Init();
    Dlc_FuncExit();
}

void MApi_XC_DLC_CGC_ReInit(void)
{
    Dlc_FuncEnter();
    MDrv_DLC_CGC_ReInit();
    Dlc_FuncExit();
}

void MApi_XC_DLC_CGC_Handler(void)
{
    Dlc_FuncEnter();
    if(g_bDLC_CGC_ready)
    {
        MDrv_DLC_CGC_Handler();
    }
    Dlc_FuncExit();
}

void MApi_XC_DLC_SetCurve(MS_U8 *pNormal, MS_U8 *pLight, MS_U8 *pDark)
{

    MDrv_DLC_SendDlcInitCurveChangeInfo(ENABLE, pNormal, pLight, pDark );
    MDrv_DLC_SetCurve(pNormal, pLight, pDark);
}

void MApi_XC_DLC_SetBleSlopPoint(MS_U16 *pBLESlopPoint)
{
    MDrv_DLC_SendBleChangeInfo(ENABLE, pBLESlopPoint);
}

MS_BOOL MApi_XC_DLC_Init(XC_DLC_init *pstXC_DLC_InitData, MS_U32 u32InitDataLen)
{
    MS_U8 i;
    StuDlc_FinetuneParamaters DLC_MFinit;
    MS_U32 u32NonPMBankSize = 0, u32DLCRiuBaseAddr=0;
    DLC_init_Ext DLC_InitExt;

    // get MMIO base
    if(MDrv_MMIO_GetBASE( &u32DLCRiuBaseAddr, &u32NonPMBankSize, MS_MODULE_PM ) != TRUE)
    {
        //OS_DELETE_MUTEX(_s32ACEMutex);
        //PNL_ASSERT(0, "%s\n", "Get base address failed\n");
        //printf("XC_DLC GetBase failed\n");
        return FALSE;
    }
    else
    {
    }
    memset(&DLC_InitExt, 0 , sizeof(DLC_InitExt) );
    MDrv_DLC_init_riu_base( u32DLCRiuBaseAddr);

    if(u32InitDataLen != sizeof(XC_DLC_init))
    {
        return FALSE;
    }
    memset(&DLC_MFinit, 0 , sizeof(DLC_MFinit) );

    for (i=0; i<16; ++i)
    {
        DLC_InitExt.uwLumaCurve[i]     = ((MS_U16)pstXC_DLC_InitData->DLC_MFinit.ucLumaCurve[i])<<2;
        DLC_InitExt.uwLumaCurve2_a[i]  = ((MS_U16)pstXC_DLC_InitData->DLC_MFinit.ucLumaCurve2_a[i])<<2;
        DLC_InitExt.uwLumaCurve2_b[i]  = ((MS_U16)pstXC_DLC_InitData->DLC_MFinit.ucLumaCurve2_b[i])<<2;

        DLC_MFinit.ucLumaCurve[i]     = pstXC_DLC_InitData->DLC_MFinit.ucLumaCurve[i];
        DLC_MFinit.ucLumaCurve2_a[i]  = pstXC_DLC_InitData->DLC_MFinit.ucLumaCurve2_a[i];
        DLC_MFinit.ucLumaCurve2_b[i]  = pstXC_DLC_InitData->DLC_MFinit.ucLumaCurve2_b[i];
    }

#if (ENABLE_10_BIT_DLC)
    DLC_InitExt.b10BitsCruveEn        = pstXC_DLC_InitData->b10BitsEn;
#else
    DLC_InitExt.b10BitsCruveEn        = 0;
#endif

    DLC_MFinit.u8_L_L_U               = pstXC_DLC_InitData->DLC_MFinit.u8_L_L_U;
    DLC_MFinit.u8_L_L_D               = pstXC_DLC_InitData->DLC_MFinit.u8_L_L_D;
    DLC_MFinit.u8_L_H_U               = pstXC_DLC_InitData->DLC_MFinit.u8_L_H_U;
    DLC_MFinit.u8_L_H_D               = pstXC_DLC_InitData->DLC_MFinit.u8_L_H_D;
    DLC_MFinit.u8_S_L_U               = pstXC_DLC_InitData->DLC_MFinit.u8_S_L_U;
    DLC_MFinit.u8_S_L_D               = pstXC_DLC_InitData->DLC_MFinit.u8_S_L_D;
    DLC_MFinit.u8_S_H_U               = pstXC_DLC_InitData->DLC_MFinit.u8_S_H_U;
    DLC_MFinit.u8_S_H_D               = pstXC_DLC_InitData->DLC_MFinit.u8_S_H_D ;

    DLC_MFinit.ucDlcPureImageMode     = pstXC_DLC_InitData->DLC_MFinit.ucDlcPureImageMode;
    DLC_MFinit.ucDlcLevelLimit        = pstXC_DLC_InitData->DLC_MFinit.ucDlcLevelLimit;
    DLC_MFinit.ucDlcAvgDelta          = pstXC_DLC_InitData->DLC_MFinit.ucDlcAvgDelta;
    DLC_MFinit.ucDlcAvgDeltaStill     = pstXC_DLC_InitData->DLC_MFinit.ucDlcAvgDeltaStill;
    DLC_MFinit.ucDlcFastAlphaBlending = pstXC_DLC_InitData->DLC_MFinit.ucDlcFastAlphaBlending;
    DLC_MFinit.ucDlcYAvgThresholdL    = pstXC_DLC_InitData->DLC_MFinit.ucDlcYAvgThresholdL;
    DLC_MFinit.ucDlcYAvgThresholdH    = pstXC_DLC_InitData->DLC_MFinit.ucDlcYAvgThresholdH;
    DLC_MFinit.ucDlcBLEPoint          = pstXC_DLC_InitData->DLC_MFinit.ucDlcBLEPoint;
    DLC_MFinit.ucDlcWLEPoint          = pstXC_DLC_InitData->DLC_MFinit.ucDlcWLEPoint;
    DLC_MFinit.bEnableBLE             = pstXC_DLC_InitData->DLC_MFinit.bEnableBLE;
    DLC_MFinit.bEnableWLE             = pstXC_DLC_InitData->DLC_MFinit.bEnableWLE;
    DLC_MFinit.ucCGCYth               = pstXC_DLC_InitData->DLC_MFinit.ucCGCYth;
    DLC_MFinit.bCGCCGainCtrl          = pstXC_DLC_InitData->DLC_MFinit.bCGCCGainCtrl;

    // CGC
    DLC_MFinit.ucCGCCGain_offset      = pstXC_DLC_InitData->DLC_MFinit.ucCGCCGain_offset;
    DLC_MFinit.ucCGCChroma_GainLimitH = pstXC_DLC_InitData->DLC_MFinit.ucCGCChroma_GainLimitH;
    DLC_MFinit.ucCGCChroma_GainLimitL = pstXC_DLC_InitData->DLC_MFinit.ucCGCChroma_GainLimitL;
    DLC_MFinit.ucCGCYCslope           = pstXC_DLC_InitData->DLC_MFinit.ucCGCYCslope;

    MDrv_DLC_Init(DLC_MFinit);
    MDrv_DLC_Init_Ext(&DLC_InitExt);

    Dlc_FuncEnter();
    MDrv_DLC_InitCurve (pstXC_DLC_InitData->u16CurveHStart, pstXC_DLC_InitData->u16CurveHEnd, pstXC_DLC_InitData->u16CurveVStart, pstXC_DLC_InitData->u16CurveVEnd);
    Dlc_FuncExit();

    return TRUE;
}

MS_BOOL MApi_XC_DLC_Init_Ex(XC_DLC_init *pstXC_DLC_InitData, MS_U32 u32InitDataLen)
{
    MS_U8 i;
    StuDlc_FinetuneParamaters DLC_MFinit;
    MS_U32 u32NonPMBankSize = 0, u32DLCRiuBaseAddr=0;
    DLC_init_Ext DLC_InitExt;


    // get MMIO base
    if(MDrv_MMIO_GetBASE( &u32DLCRiuBaseAddr, &u32NonPMBankSize, MS_MODULE_PM ) != TRUE)
    {
        //OS_DELETE_MUTEX(_s32ACEMutex);
        //PNL_ASSERT(0, "%s\n", "Get base address failed\n");
        //printf("XC_DLC GetBase failed\n");
        return FALSE;
    }
    else
    {
    }
    memset(&DLC_InitExt, 0 , sizeof(DLC_InitExt) );
    MDrv_DLC_init_riu_base( u32DLCRiuBaseAddr);

    if(u32InitDataLen != sizeof(XC_DLC_init))
    {
        return FALSE;
    }
    memset(&DLC_MFinit, 0 , sizeof(DLC_MFinit) );

    for (i=0; i<16; ++i)
    {
        DLC_InitExt.uwLumaCurve[i]     = ((MS_U16)pstXC_DLC_InitData->DLC_MFinit_Ex.ucLumaCurve[i])<<2;
        DLC_InitExt.uwLumaCurve2_a[i]  = ((MS_U16)pstXC_DLC_InitData->DLC_MFinit_Ex.ucLumaCurve2_a[i])<<2;
        DLC_InitExt.uwLumaCurve2_b[i]  = ((MS_U16)pstXC_DLC_InitData->DLC_MFinit_Ex.ucLumaCurve2_b[i])<<2;

        DLC_MFinit.ucLumaCurve[i]     = pstXC_DLC_InitData->DLC_MFinit_Ex.ucLumaCurve[i];
        DLC_MFinit.ucLumaCurve2_a[i]  = pstXC_DLC_InitData->DLC_MFinit_Ex.ucLumaCurve2_a[i];
        DLC_MFinit.ucLumaCurve2_b[i]  = pstXC_DLC_InitData->DLC_MFinit_Ex.ucLumaCurve2_b[i];
    }


#if (ENABLE_10_BIT_DLC)
    DLC_InitExt.b10BitsCruveEn        = pstXC_DLC_InitData->b10BitsEn;
#else
    DLC_InitExt.b10BitsCruveEn        = 0;
#endif

    DLC_MFinit.u8_L_L_U               = pstXC_DLC_InitData->DLC_MFinit_Ex.u8_L_L_U;
    DLC_MFinit.u8_L_L_D               = pstXC_DLC_InitData->DLC_MFinit_Ex.u8_L_L_D;
    DLC_MFinit.u8_L_H_U               = pstXC_DLC_InitData->DLC_MFinit_Ex.u8_L_H_U;
    DLC_MFinit.u8_L_H_D               = pstXC_DLC_InitData->DLC_MFinit_Ex.u8_L_H_D;
    DLC_MFinit.u8_S_L_U               = pstXC_DLC_InitData->DLC_MFinit_Ex.u8_S_L_U;
    DLC_MFinit.u8_S_L_D               = pstXC_DLC_InitData->DLC_MFinit_Ex.u8_S_L_D;
    DLC_MFinit.u8_S_H_U               = pstXC_DLC_InitData->DLC_MFinit_Ex.u8_S_H_U;
    DLC_MFinit.u8_S_H_D               = pstXC_DLC_InitData->DLC_MFinit_Ex.u8_S_H_D ;

    DLC_MFinit.ucDlcPureImageMode     = pstXC_DLC_InitData->DLC_MFinit_Ex.ucDlcPureImageMode;
    DLC_MFinit.ucDlcLevelLimit        = pstXC_DLC_InitData->DLC_MFinit_Ex.ucDlcLevelLimit;
    DLC_MFinit.ucDlcAvgDelta          = pstXC_DLC_InitData->DLC_MFinit_Ex.ucDlcAvgDelta;
    DLC_MFinit.ucDlcAvgDeltaStill     = pstXC_DLC_InitData->DLC_MFinit_Ex.ucDlcAvgDeltaStill;
    DLC_MFinit.ucDlcFastAlphaBlending = pstXC_DLC_InitData->DLC_MFinit_Ex.ucDlcFastAlphaBlending;
    DLC_MFinit.ucDlcYAvgThresholdL    = pstXC_DLC_InitData->DLC_MFinit_Ex.ucDlcYAvgThresholdL;
    DLC_MFinit.ucDlcYAvgThresholdH    = pstXC_DLC_InitData->DLC_MFinit_Ex.ucDlcYAvgThresholdH;
    DLC_MFinit.ucDlcBLEPoint          = pstXC_DLC_InitData->DLC_MFinit_Ex.ucDlcBLEPoint;
    DLC_MFinit.ucDlcWLEPoint          = pstXC_DLC_InitData->DLC_MFinit_Ex.ucDlcWLEPoint;
    DLC_MFinit.bEnableBLE             = pstXC_DLC_InitData->DLC_MFinit_Ex.bEnableBLE;
    DLC_MFinit.bEnableWLE             = pstXC_DLC_InitData->DLC_MFinit_Ex.bEnableWLE;
    DLC_MFinit.ucCGCYth               = pstXC_DLC_InitData->DLC_MFinit_Ex.ucCGCYth;
    DLC_MFinit.bCGCCGainCtrl          = pstXC_DLC_InitData->DLC_MFinit_Ex.bCGCCGainCtrl;

    // CGC
    DLC_MFinit.ucCGCCGain_offset      = pstXC_DLC_InitData->DLC_MFinit_Ex.ucCGCCGain_offset;
    DLC_MFinit.ucCGCChroma_GainLimitH = pstXC_DLC_InitData->DLC_MFinit_Ex.ucCGCChroma_GainLimitH;
    DLC_MFinit.ucCGCChroma_GainLimitL = pstXC_DLC_InitData->DLC_MFinit_Ex.ucCGCChroma_GainLimitL;
    DLC_MFinit.ucCGCYCslope           = pstXC_DLC_InitData->DLC_MFinit_Ex.ucCGCYCslope;


    if(pstXC_DLC_InitData->DLC_MFinit_Ex.u32DLC_MFinit_Ex_Version>=1)
    {
#if (DLC_MFINIT_EX_VERSION>=1)

        for (i=0; i<DLC_HISTOGRAM_LIMIT_CURVE_ARRARY_NUM; ++i)
        {
            DLC_MFinit.ucDlcHistogramLimitCurve[i]     = pstXC_DLC_InitData->DLC_MFinit_Ex.ucDlcHistogramLimitCurve[i];
        }

        DLC_MFinit.ucDlcYAvgThresholdM    = pstXC_DLC_InitData->DLC_MFinit_Ex.ucDlcYAvgThresholdM;
        DLC_MFinit.ucDlcCurveMode         = pstXC_DLC_InitData->DLC_MFinit_Ex.ucDlcCurveMode;
        DLC_MFinit.ucDlcCurveModeMixAlpha = pstXC_DLC_InitData->DLC_MFinit_Ex.ucDlcCurveModeMixAlpha;
        DLC_MFinit.ucDlcAlgorithmMode     = pstXC_DLC_InitData->DLC_MFinit_Ex.ucDlcAlgorithmMode;
        DLC_MFinit.ucDlcSepPointH         = pstXC_DLC_InitData->DLC_MFinit_Ex.ucDlcSepPointH;
        DLC_MFinit.ucDlcSepPointL         = pstXC_DLC_InitData->DLC_MFinit_Ex.ucDlcSepPointL;
        DLC_MFinit.uwDlcBleStartPointTH   = pstXC_DLC_InitData->DLC_MFinit_Ex.uwDlcBleStartPointTH;
        DLC_MFinit.uwDlcBleEndPointTH     = pstXC_DLC_InitData->DLC_MFinit_Ex.uwDlcBleEndPointTH;
        DLC_MFinit.ucDlcCurveDiff_L_TH    = pstXC_DLC_InitData->DLC_MFinit_Ex.ucDlcCurveDiff_L_TH;
        DLC_MFinit.ucDlcCurveDiff_H_TH     = pstXC_DLC_InitData->DLC_MFinit_Ex.ucDlcCurveDiff_H_TH;
        DLC_MFinit.uwDlcBLESlopPoint_1     = pstXC_DLC_InitData->DLC_MFinit_Ex.uwDlcBLESlopPoint_1;
        DLC_MFinit.uwDlcBLESlopPoint_2     = pstXC_DLC_InitData->DLC_MFinit_Ex.uwDlcBLESlopPoint_2;
        DLC_MFinit.uwDlcBLESlopPoint_3     = pstXC_DLC_InitData->DLC_MFinit_Ex.uwDlcBLESlopPoint_3;
        DLC_MFinit.uwDlcBLESlopPoint_4     = pstXC_DLC_InitData->DLC_MFinit_Ex.uwDlcBLESlopPoint_4;
        DLC_MFinit.uwDlcBLESlopPoint_5     = pstXC_DLC_InitData->DLC_MFinit_Ex.uwDlcBLESlopPoint_5;
        DLC_MFinit.uwDlcDark_BLE_Slop_Min  = pstXC_DLC_InitData->DLC_MFinit_Ex.uwDlcDark_BLE_Slop_Min;
        DLC_MFinit.ucDlcCurveDiffCoringTH  = pstXC_DLC_InitData->DLC_MFinit_Ex.ucDlcCurveDiffCoringTH;
        DLC_MFinit.ucDlcAlphaBlendingMin   = pstXC_DLC_InitData->DLC_MFinit_Ex.ucDlcAlphaBlendingMin;
        DLC_MFinit.ucDlcAlphaBlendingMax   = pstXC_DLC_InitData->DLC_MFinit_Ex.ucDlcAlphaBlendingMax;
        DLC_MFinit.ucDlcFlicker_alpha      = pstXC_DLC_InitData->DLC_MFinit_Ex.ucDlcFlicker_alpha;
        DLC_MFinit.ucDlcYAVG_L_TH          = pstXC_DLC_InitData->DLC_MFinit_Ex.ucDlcYAVG_L_TH;
        DLC_MFinit.ucDlcYAVG_H_TH          = pstXC_DLC_InitData->DLC_MFinit_Ex.ucDlcYAVG_H_TH;

        DLC_MFinit.ucDlcDiffBase_L         = pstXC_DLC_InitData->DLC_MFinit_Ex.ucDlcDiffBase_L;
        DLC_MFinit.ucDlcDiffBase_M         = pstXC_DLC_InitData->DLC_MFinit_Ex.ucDlcDiffBase_M;
        DLC_MFinit.ucDlcDiffBase_H         = pstXC_DLC_InitData->DLC_MFinit_Ex.ucDlcDiffBase_H;
#endif
    }

    MDrv_DLC_Init(DLC_MFinit);
    MDrv_DLC_Init_Ext(&DLC_InitExt);

    Dlc_FuncEnter();
    MDrv_DLC_InitCurve (pstXC_DLC_InitData->u16CurveHStart, pstXC_DLC_InitData->u16CurveHEnd, pstXC_DLC_InitData->u16CurveVStart, pstXC_DLC_InitData->u16CurveVEnd);
    Dlc_FuncExit();

    return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// Setting DLC
// @param  DLC_MFinit             \b IN: DLC data
//-------------------------------------------------------------------------------------------------
void MApi_XC_DLC_SetSetting(XC_DLC_MFinit DLC_MFinit)
{

    MS_U8 i;
    StuDlc_FinetuneParamaters DLC_MFinit_new;

    memset(&DLC_MFinit_new, 0 , sizeof(DLC_MFinit_new) );
    for (i=0; i<16; ++i)
    {
        DLC_MFinit_new.ucLumaCurve[i]     = DLC_MFinit.ucLumaCurve[i];
        DLC_MFinit_new.ucLumaCurve2_a[i]  = DLC_MFinit.ucLumaCurve2_a[i];
        DLC_MFinit_new.ucLumaCurve2_b[i]  = DLC_MFinit.ucLumaCurve2_b[i];
    }

    DLC_MFinit_new.u8_L_L_U               = DLC_MFinit.u8_L_L_U;
    DLC_MFinit_new.u8_L_L_D               = DLC_MFinit.u8_L_L_D;
    DLC_MFinit_new.u8_L_H_U               = DLC_MFinit.u8_L_H_U;
    DLC_MFinit_new.u8_L_H_D               = DLC_MFinit.u8_L_H_D;
    DLC_MFinit_new.u8_S_L_U               = DLC_MFinit.u8_S_L_U;
    DLC_MFinit_new.u8_S_L_D               = DLC_MFinit.u8_S_L_D;
    DLC_MFinit_new.u8_S_H_U               = DLC_MFinit.u8_S_H_U;
    DLC_MFinit_new.u8_S_H_D               = DLC_MFinit.u8_S_H_D ;

    DLC_MFinit_new.ucDlcPureImageMode     = DLC_MFinit.ucDlcPureImageMode;
    DLC_MFinit_new.ucDlcLevelLimit        = DLC_MFinit.ucDlcLevelLimit;
    DLC_MFinit_new.ucDlcAvgDelta          = DLC_MFinit.ucDlcAvgDelta;
    DLC_MFinit_new.ucDlcAvgDeltaStill     = DLC_MFinit.ucDlcAvgDeltaStill;
    DLC_MFinit_new.ucDlcFastAlphaBlending = DLC_MFinit.ucDlcFastAlphaBlending;
    DLC_MFinit_new.ucDlcYAvgThresholdL    = DLC_MFinit.ucDlcYAvgThresholdL;
    DLC_MFinit_new.ucDlcYAvgThresholdH    = DLC_MFinit.ucDlcYAvgThresholdH;
    DLC_MFinit_new.ucDlcBLEPoint          = DLC_MFinit.ucDlcBLEPoint;
    DLC_MFinit_new.ucDlcWLEPoint          = DLC_MFinit.ucDlcWLEPoint;
    DLC_MFinit_new.bEnableBLE             = DLC_MFinit.bEnableBLE;
    DLC_MFinit_new.bEnableWLE             = DLC_MFinit.bEnableWLE;

    DLC_MFinit_new.ucCGCYth                  = DLC_MFinit.ucCGCYth;
    DLC_MFinit_new.bCGCCGainCtrl             = DLC_MFinit.bCGCCGainCtrl;

    // CGC
    DLC_MFinit_new.ucCGCCGain_offset         = DLC_MFinit.ucCGCCGain_offset;
    DLC_MFinit_new.ucCGCChroma_GainLimitH    = DLC_MFinit.ucCGCChroma_GainLimitH;
    DLC_MFinit_new.ucCGCChroma_GainLimitL    = DLC_MFinit.ucCGCChroma_GainLimitL;
    DLC_MFinit_new.ucCGCYCslope              = DLC_MFinit.ucCGCYCslope;

    MDrv_DLC_Init(DLC_MFinit_new);
}

//-------------------------------------------------------------------------------------------------
/// Setting DLC
// @param  DLC_MFinit             \b IN: DLC data
//-------------------------------------------------------------------------------------------------
void MApi_XC_DLC_SetSetting_Ex(XC_DLC_MFinit_Ex *DLC_MFinit_Ex)
{

    MS_U8 i;
    StuDlc_FinetuneParamaters DLC_MFinit_new;

    memset(&DLC_MFinit_new, 0 , sizeof(DLC_MFinit_new) );
    for (i=0; i<16; ++i)
    {
        DLC_MFinit_new.ucLumaCurve[i]     = DLC_MFinit_Ex->ucLumaCurve[i];
        DLC_MFinit_new.ucLumaCurve2_a[i]  = DLC_MFinit_Ex->ucLumaCurve2_a[i];
        DLC_MFinit_new.ucLumaCurve2_b[i]  = DLC_MFinit_Ex->ucLumaCurve2_b[i];
    }

    DLC_MFinit_new.u8_L_L_U               = DLC_MFinit_Ex->u8_L_L_U;
    DLC_MFinit_new.u8_L_L_D               = DLC_MFinit_Ex->u8_L_L_D;
    DLC_MFinit_new.u8_L_H_U               = DLC_MFinit_Ex->u8_L_H_U;
    DLC_MFinit_new.u8_L_H_D               = DLC_MFinit_Ex->u8_L_H_D;
    DLC_MFinit_new.u8_S_L_U               = DLC_MFinit_Ex->u8_S_L_U;
    DLC_MFinit_new.u8_S_L_D               = DLC_MFinit_Ex->u8_S_L_D;
    DLC_MFinit_new.u8_S_H_U               = DLC_MFinit_Ex->u8_S_H_U;
    DLC_MFinit_new.u8_S_H_D               = DLC_MFinit_Ex->u8_S_H_D ;

    DLC_MFinit_new.ucDlcPureImageMode     = DLC_MFinit_Ex->ucDlcPureImageMode;
    DLC_MFinit_new.ucDlcLevelLimit        = DLC_MFinit_Ex->ucDlcLevelLimit;
    DLC_MFinit_new.ucDlcAvgDelta          = DLC_MFinit_Ex->ucDlcAvgDelta;
    DLC_MFinit_new.ucDlcAvgDeltaStill     = DLC_MFinit_Ex->ucDlcAvgDeltaStill;
    DLC_MFinit_new.ucDlcFastAlphaBlending = DLC_MFinit_Ex->ucDlcFastAlphaBlending;
    DLC_MFinit_new.ucDlcYAvgThresholdL    = DLC_MFinit_Ex->ucDlcYAvgThresholdL;
    DLC_MFinit_new.ucDlcYAvgThresholdH    = DLC_MFinit_Ex->ucDlcYAvgThresholdH;
    DLC_MFinit_new.ucDlcBLEPoint          = DLC_MFinit_Ex->ucDlcBLEPoint;
    DLC_MFinit_new.ucDlcWLEPoint          = DLC_MFinit_Ex->ucDlcWLEPoint;
    DLC_MFinit_new.bEnableBLE             = DLC_MFinit_Ex->bEnableBLE;
    DLC_MFinit_new.bEnableWLE             = DLC_MFinit_Ex->bEnableWLE;

    DLC_MFinit_new.ucCGCYth               = DLC_MFinit_Ex->ucCGCYth;
    DLC_MFinit_new.bCGCCGainCtrl          = DLC_MFinit_Ex->bCGCCGainCtrl;

    // CGC
    DLC_MFinit_new.ucCGCCGain_offset      = DLC_MFinit_Ex->ucCGCCGain_offset;
    DLC_MFinit_new.ucCGCChroma_GainLimitH = DLC_MFinit_Ex->ucCGCChroma_GainLimitH;
    DLC_MFinit_new.ucCGCChroma_GainLimitL = DLC_MFinit_Ex->ucCGCChroma_GainLimitL;
    DLC_MFinit_new.ucCGCYCslope           = DLC_MFinit_Ex->ucCGCYCslope;

    if(DLC_MFinit_Ex->u32DLC_MFinit_Ex_Version>=1)
    {
        for (i=0; i<DLC_HISTOGRAM_LIMIT_CURVE_ARRARY_NUM; ++i)
        {
            DLC_MFinit_new.ucDlcHistogramLimitCurve[i]  = DLC_MFinit_Ex->ucDlcHistogramLimitCurve[i];
        }

        DLC_MFinit_new.ucDlcYAvgThresholdM    = DLC_MFinit_Ex->ucDlcYAvgThresholdM;
        DLC_MFinit_new.ucDlcCurveMode         = DLC_MFinit_Ex->ucDlcCurveMode;
        DLC_MFinit_new.ucDlcCurveModeMixAlpha = DLC_MFinit_Ex->ucDlcCurveModeMixAlpha;
        DLC_MFinit_new.ucDlcAlgorithmMode     = DLC_MFinit_Ex->ucDlcAlgorithmMode;
        DLC_MFinit_new.ucDlcSepPointH         = DLC_MFinit_Ex->ucDlcSepPointH;
        DLC_MFinit_new.ucDlcSepPointL         = DLC_MFinit_Ex->ucDlcSepPointL;
        DLC_MFinit_new.uwDlcBleStartPointTH   = DLC_MFinit_Ex->uwDlcBleStartPointTH;
        DLC_MFinit_new.uwDlcBleEndPointTH     = DLC_MFinit_Ex->uwDlcBleEndPointTH;
        DLC_MFinit_new.ucDlcCurveDiff_L_TH    = DLC_MFinit_Ex->ucDlcCurveDiff_L_TH;
        DLC_MFinit_new.ucDlcCurveDiff_H_TH    = DLC_MFinit_Ex->ucDlcCurveDiff_H_TH;
        DLC_MFinit_new.uwDlcBLESlopPoint_1    = DLC_MFinit_Ex->uwDlcBLESlopPoint_1;
        DLC_MFinit_new.uwDlcBLESlopPoint_2    = DLC_MFinit_Ex->uwDlcBLESlopPoint_2;
        DLC_MFinit_new.uwDlcBLESlopPoint_3    = DLC_MFinit_Ex->uwDlcBLESlopPoint_3;
        DLC_MFinit_new.uwDlcBLESlopPoint_4    = DLC_MFinit_Ex->uwDlcBLESlopPoint_4;
        DLC_MFinit_new.uwDlcBLESlopPoint_5    = DLC_MFinit_Ex->uwDlcBLESlopPoint_5;
        DLC_MFinit_new.uwDlcDark_BLE_Slop_Min = DLC_MFinit_Ex->uwDlcDark_BLE_Slop_Min;
        DLC_MFinit_new.ucDlcCurveDiffCoringTH = DLC_MFinit_Ex->ucDlcCurveDiffCoringTH;
        DLC_MFinit_new.ucDlcAlphaBlendingMin  = DLC_MFinit_Ex->ucDlcAlphaBlendingMin;
        DLC_MFinit_new.ucDlcAlphaBlendingMax  = DLC_MFinit_Ex->ucDlcAlphaBlendingMax;
        DLC_MFinit_new.ucDlcFlicker_alpha     = DLC_MFinit_Ex->ucDlcFlicker_alpha;
        DLC_MFinit_new.ucDlcYAVG_L_TH         = DLC_MFinit_Ex->ucDlcYAVG_L_TH;
        DLC_MFinit_new.ucDlcYAVG_H_TH         = DLC_MFinit_Ex->ucDlcYAVG_H_TH;

        DLC_MFinit_new.ucDlcDiffBase_L        = DLC_MFinit_Ex->ucDlcDiffBase_L;
        DLC_MFinit_new.ucDlcDiffBase_M        = DLC_MFinit_Ex->ucDlcDiffBase_M;
        DLC_MFinit_new.ucDlcDiffBase_H        = DLC_MFinit_Ex->ucDlcDiffBase_H;
    }

    MDrv_DLC_Init(DLC_MFinit_new);
}

MS_BOOL MApi_XC_DLC_Exit(void)
{
    return TRUE;
}
/******************************************************************************/
//Enable/disable DLC to control MDrv_DLC_SetOnOff ,used DLC enbale register to control.
//@param bSwitch \b IN
//   - # TRUE  Enable
//   - # FALSE Disable
/******************************************************************************/
void MApi_XC_DLC_SetOnOff ( MS_BOOL bSwitch, MS_BOOL bWindow )
{
    Dlc_FuncEnter();
    MDrv_DLC_SetOnOff( bSwitch, bWindow );
    Dlc_FuncExit();
}

/******************************************************************************/
//Enable/disable DLC to control MDrv_DLC_Handler function.
//@param bSwitch \b IN
//   - # TRUE  Enable
//   - # FALSE Disable
/******************************************************************************/
MS_BOOL MApi_XC_DLC_SetDlcHandlerOnOff( MS_BOOL bSwitch )
{
    MDrv_DLC_SetDlcHandlerOnOff( bSwitch);
    return TRUE;
}

/******************************************************************************/
//Enable/disable BLE to control BLE function.
//@param bSwitch \b IN
//   - # TRUE  Enable
//   - # FALSE Disable
/******************************************************************************/
MS_BOOL MApi_XC_DLC_SetBleOnOff( MS_BOOL bSwitch )
{
    MDrv_DLC_SetBleOnOff( bSwitch);
    return TRUE;
}

/******************************************************************************/
//Enable/disable DLC Set Curve Both for main and sub.
//@param bEnable \b IN
//   - # TRUE  Enable
//   - # FALSE Disable
/******************************************************************************/
void MApi_XC_DLC_EnableMainSubCurveSynchronization (MS_BOOL bEnable)
{
    Dlc_FuncEnter();
    MDrv_DLC_EnableMainSubCurveSynchronization( bEnable );
    Dlc_FuncExit();
}

 /******************************************************************************/
// DLC handler
// @param bWindow \b IN:
// - @see
/******************************************************************************/
void MApi_XC_DLC_Handler(MS_BOOL bWindow)
{
    Dlc_FuncEnter();
    MDrv_DLC_Handler(bWindow);
    Dlc_FuncExit();
}

 /******************************************************************************/
// DLC get histogram handler
// @param bWindow \b IN:
// - @see
/******************************************************************************/
MS_BOOL MApi_XC_DLC_GetHistogramHandler(MS_BOOL bWindow)
{
    MS_BOOL bResult;

    Dlc_FuncEnter();

    bResult = MDrv_DLC_GetHistogram(bWindow);
    Dlc_FuncExit();

    return bResult;
}


/******************************************************************************/
//Get average value of histogram
//@return Average value from 0 to 255
/******************************************************************************/
MS_U8 MApi_XC_DLC_GetAverageValue(void)
{
    MS_U8 bResult;

    Dlc_FuncEnter();

    bResult = MDrv_DLC_GetAverageValue();

    Dlc_FuncExit();
    return bResult;
}

/******************************************************************************/
//Get average value exactness of histogram
//@return Average value from 0 to 1023
/******************************************************************************/
MS_U16 MApi_XC_DLC_GetAverageValue_Ex(void)
{
    MS_U16 bResult;

    Dlc_FuncEnter();

    bResult = MDrv_DLC_GetAverageValue_x4();

    Dlc_FuncExit();
    return bResult;
}

void MApi_XC_DLC_InitCurve (MS_U16 u16HStart, MS_U16 u16HEnd, MS_U16 u16VStart, MS_U16 u16VEnd)
{
    Dlc_FuncEnter();
    MDrv_DLC_InitCurve ( u16HStart, u16HEnd, u16VStart, u16VEnd );
    Dlc_FuncExit();
}

MS_BOOL MApi_XC_DLC_GetLumaCurveStatus( void )
{
    MS_BOOL bResult;

    Dlc_FuncEnter();
    bResult = MDrv_DLC_GetLumaCurveStatus();
    Dlc_FuncExit();
    return bResult;
}

void MApi_XC_DLC_SpeedupTrigger (MS_U8 u8Loop)
{
    Dlc_FuncEnter();
    MDrv_DLC_SpeedupTrigger ( u8Loop );
    Dlc_FuncExit();
}

E_XC_DLC_RESULT MApi_XC_DLC_GetLibVer(const MSIF_Version **ppVersion)
{
    if (!ppVersion)
    {
        return E_XC_DLC_FAIL;
    }

    *ppVersion = &_api_xc_dlc_version;
    return E_XC_DLC_OK;
}

const XC_DLC_ApiInfo * MApi_XC_DLC_GetInfo(void)                                   ///< Get info from driver
{
    return &_stXC_DLC_ApiInfo;
}

MS_BOOL MApi_XC_DLC_GetStatus(XC_DLC_ApiStatus *pDrvStatus, MS_BOOL bWindow)
{
    MS_U8 i;
    StuDlc_FinetuneParamaters DLC_MFinit;
//    StuDbc_FinetuneParamaters_Lib DLC_DBC_MFinit;
    DLC_LUMADATA DLCLumaData;

    Dlc_FuncEnter();
    if(bWindow == 0)
    {
        MDrv_DLC_GetDLCInfo(&DLC_MFinit, sizeof(StuDlc_FinetuneParamaters));
        MDrv_DLC_GetLumaInfo( &DLCLumaData, sizeof(DLC_LUMADATA));

        for (i=0; i<16; ++i)
        {
            pDrvStatus->DLCinit.DLC_MFinit.ucLumaCurve[i]     = DLC_MFinit.ucLumaCurve[i];
            pDrvStatus->DLCinit.DLC_MFinit.ucLumaCurve2_a[i]  = DLC_MFinit.ucLumaCurve2_a[i];
            pDrvStatus->DLCinit.DLC_MFinit.ucLumaCurve2_b[i]  = DLC_MFinit.ucLumaCurve2_b[i];
            pDrvStatus->DLCinit.DLC_MFinit.ucLumaCurve2[i]    = DLC_MFinit.ucLumaCurve2[i];
        }

        pDrvStatus->DLCinit.DLC_MFinit.u8_L_L_U               = DLC_MFinit.u8_L_L_U;
        pDrvStatus->DLCinit.DLC_MFinit.u8_L_L_D               = DLC_MFinit.u8_L_L_D;
        pDrvStatus->DLCinit.DLC_MFinit.u8_L_H_U               = DLC_MFinit.u8_L_H_U;
        pDrvStatus->DLCinit.DLC_MFinit.u8_L_H_D               = DLC_MFinit.u8_L_H_D;
        pDrvStatus->DLCinit.DLC_MFinit.u8_S_L_U               = DLC_MFinit.u8_S_L_U;
        pDrvStatus->DLCinit.DLC_MFinit.u8_S_L_D               = DLC_MFinit.u8_S_L_D;
        pDrvStatus->DLCinit.DLC_MFinit.u8_S_H_U               = DLC_MFinit.u8_S_H_U;
        pDrvStatus->DLCinit.DLC_MFinit.u8_S_H_D               = DLC_MFinit.u8_S_H_D ;
        pDrvStatus->DLCinit.DLC_MFinit.ucDlcPureImageMode     = DLC_MFinit.ucDlcPureImageMode;
        pDrvStatus->DLCinit.DLC_MFinit.ucDlcLevelLimit        = DLC_MFinit.ucDlcLevelLimit;
        pDrvStatus->DLCinit.DLC_MFinit.ucDlcAvgDelta          = DLC_MFinit.ucDlcAvgDelta;
        pDrvStatus->DLCinit.DLC_MFinit.ucDlcAvgDeltaStill     = DLC_MFinit.ucDlcAvgDeltaStill;
        pDrvStatus->DLCinit.DLC_MFinit.ucDlcFastAlphaBlending = DLC_MFinit.ucDlcFastAlphaBlending;
        pDrvStatus->DLCinit.DLC_MFinit.ucDlcSlowEvent         = DLC_MFinit.ucDlcSlowEvent;
        pDrvStatus->DLCinit.DLC_MFinit.ucDlcTimeOut           = DLC_MFinit.ucDlcTimeOut;
        pDrvStatus->DLCinit.DLC_MFinit.ucDlcFlickAlphaStart   = DLC_MFinit.ucDlcFlickAlphaStart;
        pDrvStatus->DLCinit.DLC_MFinit.ucDlcYAvgThresholdH    = DLC_MFinit.ucDlcYAvgThresholdH;
        pDrvStatus->DLCinit.DLC_MFinit.ucDlcYAvgThresholdL    = DLC_MFinit.ucDlcYAvgThresholdL;
        pDrvStatus->DLCinit.DLC_MFinit.ucDlcBLEPoint          = DLC_MFinit.ucDlcBLEPoint;
        pDrvStatus->DLCinit.DLC_MFinit.ucDlcWLEPoint          = DLC_MFinit.ucDlcWLEPoint;
        pDrvStatus->DLCinit.DLC_MFinit.bEnableBLE             = DLC_MFinit.bEnableBLE;
        pDrvStatus->DLCinit.DLC_MFinit.bEnableWLE             = DLC_MFinit.bEnableWLE;

        for(i=0; i<32; i++)
        {
            pDrvStatus->DLCluma.g_wLumaHistogram32H[i]        = DLCLumaData.g_wLumaHistogram32H[i];
        }
        for(i=0; i<16; i++)
        {
           pDrvStatus->DLCluma.g_ucTable[i]                   = DLCLumaData.g_ucTable[i];
        }
        pDrvStatus->DLCluma.g_wLumiTotalCount                 = DLCLumaData.g_wLumiTotalCount;
        pDrvStatus->DLCluma.g_wLumiAverageTemp                = DLCLumaData.g_wLumiAverageTemp;
        pDrvStatus->DLCluma.g_ucHistogramMax                  = DLCLumaData.g_ucHistogramMax;
        pDrvStatus->DLCluma.g_ucHistogramMin                  = DLCLumaData.g_ucHistogramMin;
        pDrvStatus->u16DLC_MFVer                              = MDrv_DLC_GetMFVer();
        pDrvStatus->u16DLC_CGC_MFVer                          = MDrv_DLC_CGC_GetMFVer();

        // get DBC Info, this function has problem, mark out temprary
        //MDrv_DLC_GetDBCInfo( &DLC_DBC_MFinit, sizeof(StuDbc_FinetuneParamaters_Lib));
        pDrvStatus->u16DLC_DBC_MFVer                          = MDrv_DLC_DBC_GetMFVer();
    }
    Dlc_FuncExit();
    return TRUE;
}

MS_BOOL MApi_XC_DLC_GetStatus_Ex(XC_DLC_ApiStatus *pDrvStatus, MS_BOOL bWindow)
{
    MS_U8 i;
    StuDlc_FinetuneParamaters DLC_MFinit;
//    StuDbc_FinetuneParamaters_Lib DLC_DBC_MFinit;
    DLC_LUMADATA DLCLumaData;

    Dlc_FuncEnter();
    if(bWindow == 0)
    {
        MDrv_DLC_GetDLCInfo(&DLC_MFinit, sizeof(StuDlc_FinetuneParamaters));
        MDrv_DLC_GetLumaInfo( &DLCLumaData, sizeof(DLC_LUMADATA));

        for (i=0; i<16; ++i)
        {
            pDrvStatus->DLCinit.DLC_MFinit_Ex.ucLumaCurve[i]     = DLC_MFinit.ucLumaCurve[i];
            pDrvStatus->DLCinit.DLC_MFinit_Ex.ucLumaCurve2_a[i]  = DLC_MFinit.ucLumaCurve2_a[i];
            pDrvStatus->DLCinit.DLC_MFinit_Ex.ucLumaCurve2_b[i]  = DLC_MFinit.ucLumaCurve2_b[i];
            pDrvStatus->DLCinit.DLC_MFinit_Ex.ucLumaCurve2[i]    = DLC_MFinit.ucLumaCurve2[i];
        }

        pDrvStatus->DLCinit.DLC_MFinit_Ex.u8_L_L_U               = DLC_MFinit.u8_L_L_U;
        pDrvStatus->DLCinit.DLC_MFinit_Ex.u8_L_L_D               = DLC_MFinit.u8_L_L_D;
        pDrvStatus->DLCinit.DLC_MFinit_Ex.u8_L_H_U               = DLC_MFinit.u8_L_H_U;
        pDrvStatus->DLCinit.DLC_MFinit_Ex.u8_L_H_D               = DLC_MFinit.u8_L_H_D;
        pDrvStatus->DLCinit.DLC_MFinit_Ex.u8_S_L_U               = DLC_MFinit.u8_S_L_U;
        pDrvStatus->DLCinit.DLC_MFinit_Ex.u8_S_L_D               = DLC_MFinit.u8_S_L_D;
        pDrvStatus->DLCinit.DLC_MFinit_Ex.u8_S_H_U               = DLC_MFinit.u8_S_H_U;
        pDrvStatus->DLCinit.DLC_MFinit_Ex.u8_S_H_D               = DLC_MFinit.u8_S_H_D ;
        pDrvStatus->DLCinit.DLC_MFinit_Ex.ucDlcPureImageMode     = DLC_MFinit.ucDlcPureImageMode;
        pDrvStatus->DLCinit.DLC_MFinit_Ex.ucDlcLevelLimit        = DLC_MFinit.ucDlcLevelLimit;
        pDrvStatus->DLCinit.DLC_MFinit_Ex.ucDlcAvgDelta          = DLC_MFinit.ucDlcAvgDelta;
        pDrvStatus->DLCinit.DLC_MFinit_Ex.ucDlcAvgDeltaStill     = DLC_MFinit.ucDlcAvgDeltaStill;
        pDrvStatus->DLCinit.DLC_MFinit_Ex.ucDlcFastAlphaBlending = DLC_MFinit.ucDlcFastAlphaBlending;
        pDrvStatus->DLCinit.DLC_MFinit_Ex.ucDlcSlowEvent         = DLC_MFinit.ucDlcSlowEvent;
        pDrvStatus->DLCinit.DLC_MFinit_Ex.ucDlcTimeOut           = DLC_MFinit.ucDlcTimeOut;
        pDrvStatus->DLCinit.DLC_MFinit_Ex.ucDlcFlickAlphaStart   = DLC_MFinit.ucDlcFlickAlphaStart;
        pDrvStatus->DLCinit.DLC_MFinit_Ex.ucDlcYAvgThresholdH    = DLC_MFinit.ucDlcYAvgThresholdH;
        pDrvStatus->DLCinit.DLC_MFinit_Ex.ucDlcYAvgThresholdL    = DLC_MFinit.ucDlcYAvgThresholdL;
        pDrvStatus->DLCinit.DLC_MFinit_Ex.ucDlcBLEPoint          = DLC_MFinit.ucDlcBLEPoint;
        pDrvStatus->DLCinit.DLC_MFinit_Ex.ucDlcWLEPoint          = DLC_MFinit.ucDlcWLEPoint;
        pDrvStatus->DLCinit.DLC_MFinit_Ex.bEnableBLE             = DLC_MFinit.bEnableBLE;
        pDrvStatus->DLCinit.DLC_MFinit_Ex.bEnableWLE             = DLC_MFinit.bEnableWLE;

        if(pDrvStatus->DLCinit.DLC_MFinit_Ex.u32DLC_MFinit_Ex_Version>=1)
        {
            for (i=0; i<DLC_HISTOGRAM_LIMIT_CURVE_ARRARY_NUM; ++i)
            {
                pDrvStatus->DLCinit.DLC_MFinit_Ex.ucDlcHistogramLimitCurve[i]  = DLC_MFinit.ucDlcHistogramLimitCurve[i];
            }

            pDrvStatus->DLCinit.DLC_MFinit_Ex.ucDlcYAvgThresholdM    = DLC_MFinit.ucDlcYAvgThresholdM;
            pDrvStatus->DLCinit.DLC_MFinit_Ex.ucDlcCurveMode         = DLC_MFinit.ucDlcCurveMode;
            pDrvStatus->DLCinit.DLC_MFinit_Ex.ucDlcCurveModeMixAlpha = DLC_MFinit.ucDlcCurveModeMixAlpha;
            pDrvStatus->DLCinit.DLC_MFinit_Ex.ucDlcAlgorithmMode     = DLC_MFinit.ucDlcAlgorithmMode;
            pDrvStatus->DLCinit.DLC_MFinit_Ex.ucDlcSepPointH         = DLC_MFinit.ucDlcSepPointH;
            pDrvStatus->DLCinit.DLC_MFinit_Ex.ucDlcSepPointL         = DLC_MFinit.ucDlcSepPointL;
            pDrvStatus->DLCinit.DLC_MFinit_Ex.uwDlcBleStartPointTH   = DLC_MFinit.uwDlcBleStartPointTH;
            pDrvStatus->DLCinit.DLC_MFinit_Ex.uwDlcBleEndPointTH     = DLC_MFinit.uwDlcBleEndPointTH;
            pDrvStatus->DLCinit.DLC_MFinit_Ex.ucDlcCurveDiff_L_TH    = DLC_MFinit.ucDlcCurveDiff_L_TH;
            pDrvStatus->DLCinit.DLC_MFinit_Ex.ucDlcCurveDiff_H_TH     = DLC_MFinit.ucDlcCurveDiff_H_TH;
            pDrvStatus->DLCinit.DLC_MFinit_Ex.uwDlcBLESlopPoint_1     = DLC_MFinit.uwDlcBLESlopPoint_1;
            pDrvStatus->DLCinit.DLC_MFinit_Ex.uwDlcBLESlopPoint_2     = DLC_MFinit.uwDlcBLESlopPoint_2;
            pDrvStatus->DLCinit.DLC_MFinit_Ex.uwDlcBLESlopPoint_3     = DLC_MFinit.uwDlcBLESlopPoint_3;
            pDrvStatus->DLCinit.DLC_MFinit_Ex.uwDlcBLESlopPoint_4     = DLC_MFinit.uwDlcBLESlopPoint_4;
            pDrvStatus->DLCinit.DLC_MFinit_Ex.uwDlcBLESlopPoint_5     = DLC_MFinit.uwDlcBLESlopPoint_5;
            pDrvStatus->DLCinit.DLC_MFinit_Ex.uwDlcDark_BLE_Slop_Min  = DLC_MFinit.uwDlcDark_BLE_Slop_Min;
            pDrvStatus->DLCinit.DLC_MFinit_Ex.ucDlcCurveDiffCoringTH  = DLC_MFinit.ucDlcCurveDiffCoringTH;
            pDrvStatus->DLCinit.DLC_MFinit_Ex.ucDlcAlphaBlendingMin   = DLC_MFinit.ucDlcAlphaBlendingMin;
            pDrvStatus->DLCinit.DLC_MFinit_Ex.ucDlcAlphaBlendingMax   = DLC_MFinit.ucDlcAlphaBlendingMax;
            pDrvStatus->DLCinit.DLC_MFinit_Ex.ucDlcFlicker_alpha      = DLC_MFinit.ucDlcFlicker_alpha;
            pDrvStatus->DLCinit.DLC_MFinit_Ex.ucDlcYAVG_L_TH          = DLC_MFinit.ucDlcYAVG_L_TH;
            pDrvStatus->DLCinit.DLC_MFinit_Ex.ucDlcYAVG_H_TH          = DLC_MFinit.ucDlcYAVG_H_TH;

            pDrvStatus->DLCinit.DLC_MFinit_Ex.ucDlcDiffBase_L         = DLC_MFinit.ucDlcDiffBase_L;
            pDrvStatus->DLCinit.DLC_MFinit_Ex.ucDlcDiffBase_M         = DLC_MFinit.ucDlcDiffBase_M;
            pDrvStatus->DLCinit.DLC_MFinit_Ex.ucDlcDiffBase_H         = DLC_MFinit.ucDlcDiffBase_H;
        }

        for(i=0; i<32; i++)
        {
            pDrvStatus->DLCluma.g_wLumaHistogram32H[i]        = DLCLumaData.g_wLumaHistogram32H[i];
        }
        for(i=0; i<16; i++)
        {
           pDrvStatus->DLCluma.g_ucTable[i]                   = DLCLumaData.g_ucTable[i];
        }
        pDrvStatus->DLCluma.g_wLumiTotalCount                 = DLCLumaData.g_wLumiTotalCount;
        pDrvStatus->DLCluma.g_wLumiAverageTemp                = DLCLumaData.g_wLumiAverageTemp;
        pDrvStatus->DLCluma.g_ucHistogramMax                  = DLCLumaData.g_ucHistogramMax;
        pDrvStatus->DLCluma.g_ucHistogramMin                  = DLCLumaData.g_ucHistogramMin;
        pDrvStatus->u16DLC_MFVer                              = MDrv_DLC_GetMFVer();
        pDrvStatus->u16DLC_CGC_MFVer                          = MDrv_DLC_CGC_GetMFVer();

        // get DBC Info, this function has problem, mark out temprary
        //MDrv_DLC_GetDBCInfo( &DLC_DBC_MFinit, sizeof(StuDbc_FinetuneParamaters_Lib));
        pDrvStatus->u16DLC_DBC_MFVer                          = MDrv_DLC_DBC_GetMFVer();
    }
    Dlc_FuncExit();
    return TRUE;
}

MS_BOOL MApi_XC_DLC_SetDbgLevel(MS_U16 u16DbgSwitch)
{
    _u16XC_DLCDbgSwitch = u16DbgSwitch;
//    _u16XC_DLCDbgSwitch = _u16XC_DLCDbgSwitch;
    return TRUE;
}

/******************************************************************************/
// API DLC DBC library start
/******************************************************************************/
void MApi_XC_DLC_DBC_Init(XC_DLC_DBC_MFinit DLC_DBC_MFinit)
{
    StuDbc_FinetuneParamaters_Lib DbcParameters = {0};
    DbcParameters.ucMax_Video           = DLC_DBC_MFinit.ucMax_Video;
    DbcParameters.ucMid_Video           = DLC_DBC_MFinit.ucMid_Video;
    DbcParameters.ucMin_Video           = DLC_DBC_MFinit.ucMin_Video;
    DbcParameters.ucMaxPWM              = DLC_DBC_MFinit.ucMaxPWM;
    DbcParameters.ucBackLight_Thres     = DLC_DBC_MFinit.ucBackLight_Thres;
    DbcParameters.ucMinPWM              = DLC_DBC_MFinit.ucMinPWM;
    DbcParameters.ucAlpha               = DLC_DBC_MFinit.ucAlpha;
    DbcParameters.ucAvgDelta            = DLC_DBC_MFinit.ucAvgDelta;
    DbcParameters.ucFastAlphaBlending   = DLC_DBC_MFinit.ucFastAlphaBlending;
    DbcParameters.ucLoop_Dly_H_Init     = DLC_DBC_MFinit.ucLoop_Dly_H_Init;
    DbcParameters.ucLoop_Dly_MH_Init    = DLC_DBC_MFinit.ucLoop_Dly_MH_Init;
    DbcParameters.ucLoop_Dly_ML_Init    = DLC_DBC_MFinit.ucLoop_Dly_ML_Init;
    DbcParameters.ucLoop_Dly_L_Init     = DLC_DBC_MFinit.ucLoop_Dly_L_Init;
    DbcParameters.bYGainCtrl            = DLC_DBC_MFinit.bYGainCtrl;
    DbcParameters.bCGainCtrl            = DLC_DBC_MFinit.bCGainCtrl;


    MDrv_DLC_DBC_Init(DbcParameters);
    g_bDLC_DBC_ready = FALSE;
    g_bDBCEnable = FALSE;
}

void MApi_XC_DBC_Init(void)
{
    msDBCInit();
}

void MApi_XC_DLC_DBC_Setstatus(MS_BOOL bDBCEnable)
{
    g_bDBCEnable = bDBCEnable;
}

MS_BOOL MApi_XC_DLC_DBC_Getstatus(void)
{
    return g_bDBCEnable;
}

void MApi_XC_DLC_DBC_SetReady(MS_BOOL bDBCReady)
{
    g_bDLC_DBC_ready = bDBCReady;
}

void MApi_XC_DLC_DBC_SetDebugMode(MS_U8 ucDBC_DebugMode)
{
    MDrv_DLC_DBC_SetDebugMode(ucDBC_DebugMode);
}

MS_U8 MApi_XC_DLC_DBC_GetDebugMode(void)
{
    return MDrv_DLC_DBC_GetDebugMode();
}

void MApi_XC_DLC_DBC_UpdatePWM(MS_U8 u8PWMvalue)
{
    MDrv_DLC_DBC_UpdatePWM(u8PWMvalue);
}

MS_U16 MApi_XC_DLC_DBC_Handler(void)
{
    if(g_bDLC_DBC_ready && g_bDBCEnable)
        return MDrv_DLC_DBC_Handler();
    else
        return 0xFFFF;
}

void MApi_XC_DLC_DBC_AdjustYCGain(void)
{
        MDrv_DLC_DBC_AdjustYCGain();
}

void MApi_XC_DLC_DBC_YCGainInit(MS_U8 u8YGain_M, MS_U8 u8YGain_L, MS_U8 u8CGain_M, MS_U8 u8CGain_L)
{
    MS_U8 u8YGain_H, u8CGain_H;
    if(g_bDLC_CGC_YGainPQCom)
    {
        u8YGain_H = g_u8DLC_CGC_YGain;
    }
    else
    {
        u8YGain_H = MDrv_DLC_CGC_GetYGain();
    }
    if(g_bDLC_CGC_CGainPQCom)
    {
        u8CGain_H = g_u8DLC_CGC_CGain;
    }
    else
    {
        u8CGain_H = MDrv_DLC_CGC_GetCGain();
    }
    msAdjustYCGain(u8YGain_H, u8CGain_H);

    MDrv_DLC_DBC_YCGainInit(u8YGain_M, u8YGain_L, u8CGain_M, u8CGain_L, u8YGain_H, u8CGain_H);
    g_bDLC_DBC_ready = TRUE;
}

void MApi_XC_DLC_DBC_Reset(void)
{
    g_bDLC_DBC_ready = FALSE;
}


#if defined(MSOS_TYPE_CE)

#pragma data_seg(".DLCApi")
    MS_U16 gu16PNL_Width      = 0;
    MS_U16 gu16PNL_Height     = 0;
    MApi_XC_DLC_Print_Callback gfnPutchar = NULL;
#pragma data_seg()
#pragma comment(linker,"/SECTION:.DLCApi,RWS")

#else

MS_U16 gu16PNL_Width      = 0;
MS_U16 gu16PNL_Height     = 0;
MApi_XC_DLC_Print_Callback gfnPutchar = NULL;

#endif


void MApi_XC_DLC_DecodeExtCmd(tDLC_CONTROL_PARAMS* pValue)
{
    gu16PNL_Width      = pValue->u16PNL_Width;
    gu16PNL_Height     = pValue->u16PNL_Height;
    gfnPutchar         = pValue->fnDLC_Putchar;

    Dlc_FuncEnter();

    msDLC_ParametersTool(pValue->pCmdBuff);
    Dlc_FuncExit();
}

void MApi_XC_DBC_DecodeExtCmd(tDBC_CONTROL_PARAMS* pValue )
{
    msDBC_ParametersTool(pValue->pCmdBuff, pValue->fnDBC_AP_OnOff, pValue->fnDBC_AdjustBacklight, pValue->fnDBC_Putchar);
}


MS_BOOL MApi_XC_DLC_WriteCurve(MS_U8 *pu8Table)
{
    Dlc_FuncEnter();
    MDrv_DLC_WriteCurve(pu8Table);
    Dlc_FuncExit();
    return TRUE;
}

MS_BOOL MApi_XC_DLC_WriteCurve_Sub(MS_U8 *pu8Table)
{
    Dlc_FuncEnter();
    MDrv_DLC_WriteCurve_Sub(pu8Table);
    Dlc_FuncExit();
    return TRUE;
}


MS_BOOL MApi_XC_DLC_GetHistogram(MS_U16 *pu16Histogram, E_XC_DLC_HISTOGRAM_TYPE enHistogramType)
{
    MS_BOOL bret = FALSE;

    Dlc_FuncEnter();
    if(enHistogramType == E_XC_DLC_HISTOGRAM_32)
    {
        bret = MDrv_DLC_get_histogram32(pu16Histogram);
    }
    Dlc_FuncExit();
    return bret;
}

void MApi_XC_DLC_SetCaptureRange(XC_DLC_CAPTURE_Range *pu16_Range)
{
    StuDbc_CAPTURE_Range DlcCapRange = {0};

    memcpy(&DlcCapRange, pu16_Range,sizeof(XC_DLC_CAPTURE_Range));

    Dlc_FuncEnter();

    MDrv_DLC_set_CaptureRange(&DlcCapRange);
    Dlc_FuncExit();

}
/******************************************************************************/
// API DLC DBC library end
/******************************************************************************/

#undef _API_XC_DLC_C_
#endif  // _API_XC_DLC_C_

