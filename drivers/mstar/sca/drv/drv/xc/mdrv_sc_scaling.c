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
#define DRV_SC_SCALING_C
#ifdef MSOS_TYPE_LINUX_KERNEL
#include <linux/wait.h>
#include <linux/string.h>
#else
#include <string.h>
#endif

// Common Definition
#include "MsCommon.h"
#include "MsOS.h"
#include "mhal_xc_chip_config.h"
//#include "Debug.h"
#include "drvXC_IOPort.h"
#include "apiXC.h"
#include "mvideo_context.h"
#include "drv_sc_ip.h"
#include "drv_sc_menuload.h"
#include "drv_sc_display.h"
#include "mdrv_sc_3d.h"

#include "mhal_sc.h"
#include "mhal_pip.h"
#include "halCHIP.h"
#include "drv_sc_scaling.h"
#include "xc_Analog_Reg.h"
#include "xc_hwreg_utility2.h"

//////////////////////////Remove when drvpq change us Mscommon.h /////////////
#define E_XRULE_HSD         0
#define E_XRULE_VSD         1
#define E_XRULE_HSP         2
#define E_XRULE_VSP         3
#define E_XRULE_CSC         4
#define E_XRULE_NUM         5

#define E_GRULE_NR          0
#define E_GRULE_NUM         1

#define PIP_SUPPORTED (MAX_WINDOW_NUM > 1)

typedef enum
{
   E_DRVSC_FBNUM  = 0,
}DRV_SC_SRCINFO ;


extern PQ_Function_Info    s_PQ_Function_Info;
///////////////////////////////////////

#define SC_DBG(x)               //x
#define SC_FNLDBG(x)            //x
#define SC_RWBANK_DBG(x)        //x
#define SC_3D_DBG(x)            //x
/******************************************************************************/
/*                     Local                                                  */
/* ****************************************************************************/

static MS_PHYADDR _u32DNRBaseAddr0[MAX_WINDOW];
static MS_PHYADDR _u32DNRBufSize[MAX_WINDOW];

static MS_BOOL _bHAutoFitPrescalingEna = FALSE, _bVAutoFitPrescalingEna = FALSE; //TRUE: Means driver auto prescaling to fit display window
static MS_BOOL _bHSizeChangedManually = FALSE; //HSize After prescaling is changed in Driver logic(3D, ForcePrescaling, etc), in this case, PQ HSD setting should be skipped

static MS_U32 u32DNRBaseOffset[MAX_WINDOW];
static MS_U8  u8StoreFrameNum[MAX_WINDOW];

static MS_U16  u16MaxSizeOfLB[MAX_WINDOW];

static MS_BOOL _bMainWinSkipWaitOutputVsync = FALSE, _bSubWinSkipWaitOutputVsync = FALSE;

static MS_BOOL _bPQSetHPreScalingratio[MAX_WINDOW] = {FALSE, FALSE, FALSE, FALSE, FALSE, FALSE};

#define MS_IPM_BASE0(win)     (_u32DNRBaseAddr0[win] / BYTE_PER_WORD )
#define MS_IPM_BASE1(win)     ((_u32DNRBaseAddr0[win] + (_u32DNRBufSize[win]/2)) / BYTE_PER_WORD)

static void   AdjustWinLineBufferSize(MS_U16 u16DispWinWidth, SCALER_WIN eWindow);
static MS_U16 GetSubStartOffsetLB(SCALER_WIN eWindow, MS_BOOL bForceChange, MS_BOOL *pbTodo);
static MS_U16 GetMaxLineBFF(SCALER_WIN);

/******************************************************************************/
/*                   Functions                                                */
/******************************************************************************/

extern MS_BOOL MDrv_VD_IsSyncLocked(void);

void MDrv_SC_set_write_limit(MS_U32 u32WritelimitAddrBase, SCALER_WIN eWindow)
{
    Hal_SC_set_write_limit(u32WritelimitAddrBase, eWindow);
}

void MDrv_SC_SetPQHSDFlag(MS_BOOL bEnable, SCALER_WIN eWindow)
{
    _bPQSetHPreScalingratio[eWindow] = bEnable;
}

MS_BOOL MDrv_SC_GetPQHSDFlag(SCALER_WIN eWindow)
{
    return _bPQSetHPreScalingratio[eWindow];
}

void MDrv_SC_set_opm_write_limit(MS_BOOL bEnable, MS_BOOL bFlag, MS_U32 u32OPWlimitAddr, SCALER_WIN eWindow)
{
#if (ENABLE_OPM_WRITE_SUPPORTED)
    Hal_SC_set_opm_write_limit(bEnable, bFlag, u32OPWlimitAddr, eWindow);
#endif //(ENABLE_OPM_WRITE_SUPPORTED)
}

//-------------------------------------------------------------------------------------------------
// Skip Wait Vsync
/// @param eWindow               \b IN: Enable
/// @param Skip wait Vsync      \b IN: Disable wait Vsync
//-------------------------------------------------------------------------------------------------
void MDrv_SC_SetSkipWaitVsync( MS_BOOL eWindow,MS_BOOL bIsSkipWaitVsyn)
{
   if(eWindow == MAIN_WINDOW )
   {
       _bMainWinSkipWaitOutputVsync = bIsSkipWaitVsyn;
   }
   else
   {
       _bSubWinSkipWaitOutputVsync = bIsSkipWaitVsyn;
   }
}

MS_BOOL MDrv_SC_GetSkipWaitVsync( MS_BOOL bScalerWin)
{
    if(bScalerWin == MAIN_WINDOW)
    {
        return  _bMainWinSkipWaitOutputVsync;
    }
    else
    {
        return _bSubWinSkipWaitOutputVsync;
    }
}


void MDrv_SC_sw_db( P_SC_SWDB_INFO pDBreg, SCALER_WIN eWindow )
{
    // Set ficlk -> sw_db  for prevent garbage when no Vpre-scaling down

    if ( !gSrcInfo[eWindow].bPreV_ScalingDown )
    {
        Hal_SC_set_ficlk( FALSE, eWindow );
    }

    if ((MApi_XC_MLoad_GetStatus(eWindow) == E_MLOAD_ENABLED) && (!MDrv_SC_GetSkipWaitVsync(eWindow)) )
    {
        Hal_SC_sw_db_burst( pDBreg, eWindow );
    }
    else
    {
        if(FALSE == MDrv_SC_GetSkipWaitVsync(eWindow))
        {
            MDrv_XC_wait_output_vsync(2, 200, eWindow);
        }
        Hal_SC_sw_db( pDBreg, eWindow );
    }

    // Set sw_db -> ficlk for prevent garbage when Vpre-scaling down
    if ( gSrcInfo[eWindow].bPreV_ScalingDown )
    {
        Hal_SC_set_ficlk( TRUE, eWindow );
    }

}

void MDrv_XC_init_fbn_win(SCALER_WIN eWindow)
{
	MS_U32 u32OPWLimitBase = 0x00;
    SC_SWDB_INFO SWDBreg;

    SWDBreg.u32H_PreScalingRatio = 0x0;
    SWDBreg.u32V_PreScalingRatio = 0x0;
    SWDBreg.u32H_PostScalingRatio = 0x0;
    SWDBreg.u32V_PostScalingRatio = 0x0;
    SWDBreg.u16VLen = 0x240;
    if(eWindow == MAIN_WINDOW)
        SWDBreg.u16VWritelimit = 0x240 | F2_V_WRITE_LIMIT_EN;
    else
        SWDBreg.u16VWritelimit = 0x240 | F1_V_WRITE_LIMIT_EN;

    SWDBreg.u16LBOffset  = 0x0;
    SWDBreg.u16DispOffset = 0x0;
    SWDBreg.u16DNROffset = 0x400;
    SWDBreg.u16OPMOffset = 0x400;
    SWDBreg.u16DNRFetch  = 0x2D0;
    SWDBreg.u16OPMFetch  = 0x2D0;

    // Attention! Writelimit base also set in MDrv_SC_set_mirror for upper layer usage.
    if( g_XC_InitData.bMirror[eWindow] )
    {
        SWDBreg.u32OPMBase0 = _u32DNRBaseAddr0[eWindow] / BYTE_PER_WORD;
        SWDBreg.u32OPMBase1 = (_u32DNRBaseAddr0[eWindow] + _u32DNRBufSize[eWindow]/2) / BYTE_PER_WORD;
        SWDBreg.u32OPMBase2 = 0;
        SWDBreg.u32DNRBase0 = SWDBreg.u32OPMBase0 + SWDBreg.u16DNROffset * (SWDBreg.u16VLen-1) * 3 / BYTE_PER_WORD;
        SWDBreg.u32DNRBase1 = SWDBreg.u32OPMBase1 + SWDBreg.u16DNROffset * (SWDBreg.u16VLen-1) * 3 / BYTE_PER_WORD;
        SWDBreg.u32DNRBase2 = SWDBreg.u32OPMBase2;
        if(eWindow == MAIN_WINDOW)
            SWDBreg.u32WritelimitBase = ((_u32DNRBaseAddr0[eWindow]) / BYTE_PER_WORD - 1) | (F2_WRITE_LIMIT_EN | F2_WRITE_LIMIT_MIN);
        else
            SWDBreg.u32WritelimitBase = ((_u32DNRBaseAddr0[eWindow]) / BYTE_PER_WORD - 1) | (F1_WRITE_LIMIT_EN | F1_WRITE_LIMIT_MIN);
    }
    else
    {
        SWDBreg.u32DNRBase0 = SWDBreg.u32OPMBase0 =
        SWDBreg.u32DNRBase1 = SWDBreg.u32OPMBase1 =
        SWDBreg.u32DNRBase2 = SWDBreg.u32OPMBase2 = MS_IPM_BASE0(eWindow);
        if(eWindow == MAIN_WINDOW)
            SWDBreg.u32WritelimitBase = ((_u32DNRBaseAddr0[eWindow] + _u32DNRBufSize[eWindow]) / BYTE_PER_WORD - 1) | F2_WRITE_LIMIT_EN;
        else
            SWDBreg.u32WritelimitBase = ((_u32DNRBaseAddr0[eWindow] + _u32DNRBufSize[eWindow]) / BYTE_PER_WORD - 1) | F1_WRITE_LIMIT_EN;
    }

    #if (ENABLE_OPM_WRITE_SUPPORTED)
        //MS_U32 u32OPWLimitBase = 0x00;
        u32OPWLimitBase = ((_u32DNRBaseAddr0[eWindow] + _u32DNRBufSize[eWindow]) / BYTE_PER_WORD - 1) ;
        MDrv_SC_set_opm_write_limit(ENABLE, 0 , u32OPWLimitBase , eWindow);
    #endif

    SWDBreg.u16H_CapStart = 0x1;
    SWDBreg.u16H_CapSize  = 0x10;
    SWDBreg.u16V_CapStart = 0x1;
    SWDBreg.u16V_CapSize  = 0x10;
    SWDBreg.u16H_DisStart = g_XC_InitData.stPanelInfo.u16HStart;
    SWDBreg.u16H_DisEnd   = g_XC_InitData.stPanelInfo.u16HStart + g_XC_InitData.stPanelInfo.u16Width - 1;
    SWDBreg.u16V_DisStart = g_XC_InitData.stPanelInfo.u16VStart;
    SWDBreg.u16V_DisEnd   = g_XC_InitData.stPanelInfo.u16VStart + g_XC_InitData.stPanelInfo.u16Height - 1;

    MDrv_SC_sw_db(&SWDBreg, eWindow);
}

E_APIXC_ReturnValue MApi_XC_SetMemoryWriteRequest(MS_BOOL bEnable, SCALER_WIN eWindow)
{
    E_APIXC_ReturnValue bRet = E_APIXC_RET_FAIL;
    _XC_ENTRY();
    bRet = Hal_SC_memory_write_request(bEnable, eWindow);
    _XC_RETURN();
    return bRet;
}

//-------------------------------------------------------------------------------------------------
/// Set DNR base0 address
/// @param  u32FBAddress      \b IN: the framebuffer address
/// @param  u32FBSize         \b IN: the framebuffer size
//-------------------------------------------------------------------------------------------------
void MApi_XC_SetFrameBufferAddress(MS_PHYADDR u32FBAddress, MS_PHYADDR u32FBSize, SCALER_WIN eWindow)
{
    _XC_ENTRY();

    MDrv_XC_SetFrameBufferAddress(u32FBAddress, u32FBSize, eWindow);

    _XC_RETURN();
}
void MDrv_XC_SetFrameBufferAddressSilently(MS_PHYADDR u32FBAddress, MS_PHYADDR u32FBSize, SCALER_WIN eWindow)
{
    _u32DNRBaseAddr0[eWindow] = u32FBAddress;
    _u32DNRBufSize[eWindow] = u32FBSize;

    if(eWindow == MAIN_WINDOW)
    {
        g_XC_InitData.u32Main_FB_Start_Addr = u32FBAddress;
        g_XC_InitData.u32Main_FB_Size = u32FBSize;
        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"Main_FB_Start_Addr=%lx\r\n",u32FBAddress);
    }
    else
    {
        g_XC_InitData.u32Sub_FB_Start_Addr = u32FBAddress;
        g_XC_InitData.u32Sub_FB_Size = u32FBSize;
        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"Sub_FB_Start_Addr=%lx\r\n",u32FBAddress);
    }
}
void MDrv_XC_SetFrameBufferAddress(MS_PHYADDR u32FBAddress, MS_PHYADDR u32FBSize, SCALER_WIN eWindow)
{
	MS_U32 u32OPWLimitBase = 0x00;
    MS_U8  u8MIUSel;

    if (Hal_SC_Is_InputSource_Disable(eWindow)==0)
    {

        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"input source (Window:%d) must be disabled before set frame buffer address. Ex:when video play, you can¡¦t use this function. \n",eWindow);
        return;
    }

    if(u32FBAddress >= HAL_MIU1_BASE)
    {
        u32FBAddress -= HAL_MIU1_BASE;
        u8MIUSel = 1;
    }
    else
    {
        u8MIUSel = 0;
    }

    _u32DNRBaseAddr0[eWindow] = u32FBAddress;
    _u32DNRBufSize[eWindow] = u32FBSize;

    Hal_SC_Enable_MiuMask();

    Hal_SC_set_freezeimg(ENABLE, eWindow);
    Hal_SC_set_miusel(u8MIUSel);

    if(g_XC_InitData.bMirror[eWindow])
    {
        Hal_SC_set_memoryaddress(u32FBAddress+ u32FBSize-1-0x100000, u32FBAddress+ u32FBSize-1-0x100000, u32FBAddress+ u32FBSize-1-0x100000,
                                 u32FBAddress+ u32FBSize-1-0x100000, u32FBAddress+ u32FBSize-1-0x100000, u32FBAddress+ u32FBSize-1-0x100000, eWindow);
    }
    else
    {
        Hal_SC_set_memoryaddress(u32FBAddress, u32FBAddress, u32FBAddress,
                                 u32FBAddress, u32FBAddress, u32FBAddress, eWindow);
    }
    MDrv_SC_set_mirror(g_XC_InitData.bMirror[eWindow], eWindow);     // set write limit address

    #if (ENABLE_OPM_WRITE_SUPPORTED)
        //MS_U32 u32OPWLimitBase = 0x00;

        u32OPWLimitBase = ((_u32DNRBaseAddr0[eWindow] + _u32DNRBufSize[eWindow]) / BYTE_PER_WORD - 1) ;
        MDrv_SC_set_opm_write_limit(ENABLE, 0 , u32OPWLimitBase , eWindow);
    #endif


    Hal_SC_set_freezeimg(DISABLE, eWindow);

    Hal_SC_Disable_MiuMask();

    if(eWindow == MAIN_WINDOW)
    {
        g_XC_InitData.u32Main_FB_Start_Addr = u32FBAddress;
        g_XC_InitData.u32Main_FB_Size = u32FBSize;
        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"Change Main_FB_Start_Addr=%lx, Size=%lx\r\n", u32FBAddress, u32FBSize);
    }
    else
    {
        g_XC_InitData.u32Sub_FB_Start_Addr = u32FBAddress;
        g_XC_InitData.u32Sub_FB_Size = u32FBSize;
        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"Change Sub_FB_Start_Addr=%lx, Size=%lx\r\n", u32FBAddress, u32FBSize);
    }

#if ENABLE_DYNAMIC_SUB_LB_OFFSET
	for ( u8MIUSel =0; u8MIUSel <MAX_WINDOW; ++u8MIUSel)
	{
		u16MaxSizeOfLB[u8MIUSel] = 0;
	}
#endif
}

MS_BOOL MApi_XC_IsFrameBufferEnoughForCusScaling(XC_SETWIN_INFO *pstXC_SetWin_Info, SCALER_WIN eWindow)
{
    //DTV max u8BitsPerPixel is 24. And here only consider 2 frame mode.
    if ((MS_U32)(pstXC_SetWin_Info->u16PreVCusScalingDst *
            pstXC_SetWin_Info->u16PreHCusScalingDst * 24 * 2) > _u32DNRBufSize[eWindow] * 8)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

#ifdef MULTI_SCALER_SUPPORTED
void MDrv_SC_set_fbl(MS_BOOL bEn, SCALER_WIN eWindow)
{
    Hal_SC_set_fbl(bEn, eWindow);
}

MS_BOOL MDrv_SC_get_fbl(SCALER_WIN eWindow)
{
    return Hal_SC_get_fbl(eWindow);
}
#else	// #ifdef MULTI_SCALER_SUPPORTED
void MDrv_SC_set_fbl(MS_BOOL bEn)
{
    Hal_SC_set_fbl(bEn);
}

MS_BOOL MDrv_SC_get_fbl(void)
{
    return Hal_SC_get_fbl();
}
#endif

/******************************************************************************/
///Set Capture window (Input window) information.
///@param pstWindow \b IN
///- pointer to window information
/******************************************************************************/
void MDrv_sc_set_capture_window(SCALER_WIN eWindow)
{

    // HDMI HV mode uses DE by pass, so capture win should be set as full range
    if( ( IsSrcTypeHDMI( gSrcInfo[eWindow].enInputSourceType ) ) && (MApi_XC_GetHdmiSyncMode() == HDMI_SYNC_HV) )
    {
        // Capture win must keep full range in HV mode
        stDBreg.u16V_CapStart = 0xC;
        stDBreg.u16H_CapStart = 0xC;
        stDBreg.u16V_CapSize = 0x1FFF;
        stDBreg.u16H_CapSize = 0x1FFF;
    }
    else
    {
        stDBreg.u16V_CapStart = gSrcInfo[eWindow].stCapWin.y;
        stDBreg.u16H_CapStart = gSrcInfo[eWindow].stCapWin.x;
        stDBreg.u16V_CapSize  = gSrcInfo[eWindow].stCapWin.height;
        stDBreg.u16H_CapSize  = gSrcInfo[eWindow].stCapWin.width;
    }

}


/******************************************************************************/
///Set output display window size
///@param pstWindow \b IN
///-  pointer to window information
/******************************************************************************/
void MDrv_SC_set_display_window(SCALER_WIN eWindow)
{
    if(gSrcInfo[eWindow].bDisplayNineLattice)
    {
        stDBreg.u16H_DisStart = g_XC_InitData.stPanelInfo.u16HStart;
        stDBreg.u16H_DisEnd = g_XC_InitData.stPanelInfo.u16HStart +g_XC_InitData.stPanelInfo.u16Width-1;
        stDBreg.u16V_DisStart = g_XC_InitData.stPanelInfo.u16VStart;
        stDBreg.u16V_DisEnd = g_XC_InitData.stPanelInfo.u16VStart + g_XC_InitData.stPanelInfo.u16Height-1;
    }
    else
    {
        stDBreg.u16H_DisStart = gSrcInfo[eWindow].stDispWin.x;

        if ( gSrcInfo[eWindow].stDispWin.width == 0 )
            stDBreg.u16H_DisEnd = gSrcInfo[eWindow].stDispWin.x;
        else
            stDBreg.u16H_DisEnd = gSrcInfo[eWindow].stDispWin.x +  gSrcInfo[eWindow].stDispWin.width - 1;

        stDBreg.u16V_DisStart = gSrcInfo[eWindow].stDispWin.y;

        if ( gSrcInfo[eWindow].stDispWin.height == 0 )
            stDBreg.u16V_DisEnd = gSrcInfo[eWindow].stDispWin.y;
        else
        {
            if( MDrv_SC_3D_Is_LR_Sbs2Line() &&
                (MApi_XC_Get_3D_Output_Mode() != E_XC_3D_OUTPUT_MODE_NONE) &&
                (MAIN_WINDOW == eWindow))
            {
                //main window need set to fullscreen to prevent show garbage when atv mirror channel change
                stDBreg.u16V_DisEnd = gSrcInfo[eWindow].stDispWin.y +  gSrcInfo[eWindow].stDispWin.height * 2 - 1;
            }
            else
            {
                stDBreg.u16V_DisEnd = gSrcInfo[eWindow].stDispWin.y +  gSrcInfo[eWindow].stDispWin.height - 1;
            }
        }
    }
}


/******************************************************************************/
///Fill display window according to panel
/******************************************************************************/

void MDrv_SC_set_std_display_window(SCALER_WIN eWindow)
{
    gSrcInfo[eWindow].stDispWin.x       = g_XC_InitData.stPanelInfo.u16HStart;
    gSrcInfo[eWindow].stDispWin.y       = g_XC_InitData.stPanelInfo.u16VStart;
    gSrcInfo[eWindow].stDispWin.width   = g_XC_InitData.stPanelInfo.u16Width;
    gSrcInfo[eWindow].stDispWin.height  = g_XC_InitData.stPanelInfo.u16Height;
}

//////////////////////////////////////////////////////////////////
// Compute ScaledCropWin according to CropWin and prescaling ratio
// parameter:
//    [IN/OUT] pSrcInfo->stCropWin
//    [IN/OUT] pSrcInfo->stScaledCropWin
//    [IN]     pSrcInfo->u16H_CapSize
//    [IN]     pSrcInfo->u16H_SizeAfterPreScaling
//    [IN]     pSrcInfo->u16V_CapSize
//    [IN]     pSrcInfo->u16V_SizeAfterPreScaling
//
void MDrv_SC_set_crop_window(XC_InternalStatus *pSrcInfo, SCALER_WIN eWindow)
{
    MS_WINDOW_TYPE *pCropWin = &pSrcInfo->stCropWin;

    if(pSrcInfo->stCapWin.width == 0 || pSrcInfo->stCapWin.height == 0)
    {
        printf("XC CapWin is error ! (Width=%d,Height=%d)\n",pSrcInfo->stCapWin.width,pSrcInfo->stCapWin.height);
        return;
    }

    if(!pSrcInfo->bDisplayNineLattice) // need refine for dircet display
    {
        gSrcInfo[eWindow].ScaledCropWin.x     = (MS_U16)((MS_U32)pCropWin->x * pSrcInfo->u16H_SizeAfterPreScaling / pSrcInfo->stCapWin.width);
        gSrcInfo[eWindow].ScaledCropWin.width = (MS_U16)( ( (MS_U32)pCropWin->width * pSrcInfo->u16H_SizeAfterPreScaling + ( pSrcInfo->stCapWin.width / 2  ) ) / pSrcInfo->stCapWin.width);

        gSrcInfo[eWindow].ScaledCropWin.y     = (MS_U16)( (MS_U32) pCropWin->y * pSrcInfo->u16V_SizeAfterPreScaling / pSrcInfo->stCapWin.height);
        gSrcInfo[eWindow].ScaledCropWin.height= (MS_U16)( ( (MS_U32)pCropWin->height * pSrcInfo->u16V_SizeAfterPreScaling + ( pSrcInfo->stCapWin.height / 2 ) ) / pSrcInfo->stCapWin.height);
        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"W=%u, W PSD=%u\r\n", pSrcInfo->stCapWin.width  , pSrcInfo->u16H_SizeAfterPreScaling);
        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"Scaled crop[1] x,y,w,h=%u,%u,%u,%u\r\n",gSrcInfo[eWindow].ScaledCropWin.x, gSrcInfo[eWindow].ScaledCropWin.y, gSrcInfo[eWindow].ScaledCropWin.width, gSrcInfo[eWindow].ScaledCropWin.height);

        // Scaled crop win may introduce imprecise calculation result, which may lead to pre and post scaling both working
        // When driver do auto fit, we only want one of pre and post working
        // Here we ensure only pre scaling is working
        // This may conflict with 3D, need refine later
        if((gSrcInfo[eWindow].ScaledCropWin.width != pSrcInfo->stDispWin.width) &&
           (_bHAutoFitPrescalingEna == TRUE))
        {
            //XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"Force change ScaledCropWin.width: %u->%u\r\n", gSrcInfo[eWindow].ScaledCropWin.width, pSrcInfo->stDispWin.width);
            gSrcInfo[eWindow].ScaledCropWin.width = pSrcInfo->stDispWin.width;
        }

        if((gSrcInfo[eWindow].ScaledCropWin.height != pSrcInfo->stDispWin.height) &&
           (_bVAutoFitPrescalingEna == TRUE))
        {
            //XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"Force change ScaledCropWin.height: %u->%u\r\n", gSrcInfo[eWindow].ScaledCropWin.height, pSrcInfo->stDispWin.height);
            gSrcInfo[eWindow].ScaledCropWin.height= pSrcInfo->stDispWin.height;
        }
        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"Scaled crop[2] x,y,w,h=%u,%u,%u,%u\r\n",gSrcInfo[eWindow].ScaledCropWin.x, gSrcInfo[eWindow].ScaledCropWin.y, gSrcInfo[eWindow].ScaledCropWin.width, gSrcInfo[eWindow].ScaledCropWin.height);

        // Error handling.
        if (g_XC_InitData.bMirror[eWindow])//Error handling for mirror case
        {
            MDrv_SC_3D_AdjustMirrorCrop(pSrcInfo,eWindow);
        }
        else//Error handling for Non-mirror case
        {
            if( pSrcInfo->u16H_SizeAfterPreScaling < (gSrcInfo[eWindow].ScaledCropWin.width + gSrcInfo[eWindow].ScaledCropWin.x) )
            {
                if(gSrcInfo[eWindow].ScaledCropWin.width <= pSrcInfo->u16H_SizeAfterPreScaling)
                {
                    gSrcInfo[eWindow].ScaledCropWin.x = (pSrcInfo->u16H_SizeAfterPreScaling - gSrcInfo[eWindow].ScaledCropWin.width);
                }
                else
                {
                    gSrcInfo[eWindow].ScaledCropWin.x = 0;
                    gSrcInfo[eWindow].ScaledCropWin.width = pSrcInfo->u16H_SizeAfterPreScaling;
                }
                XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"Scaled crop[2.c] Force x=%u, width=%u\r\n",gSrcInfo[eWindow].ScaledCropWin.x, gSrcInfo[eWindow].ScaledCropWin.width);
            }

            // for interlace, keep y in multiple of 2
            gSrcInfo[eWindow].ScaledCropWin.y = (gSrcInfo[eWindow].ScaledCropWin.y + 1) & ~0x1;
            if( pSrcInfo->u16V_SizeAfterPreScaling < (gSrcInfo[eWindow].ScaledCropWin.height + gSrcInfo[eWindow].ScaledCropWin.y) )
            {
                if(gSrcInfo[eWindow].ScaledCropWin.height <= pSrcInfo->u16V_SizeAfterPreScaling)
                {
                    gSrcInfo[eWindow].ScaledCropWin.y = (pSrcInfo->u16V_SizeAfterPreScaling - gSrcInfo[eWindow].ScaledCropWin.height) & ~0x1;
                }
                else
                {
                    gSrcInfo[eWindow].ScaledCropWin.height = pSrcInfo->u16V_SizeAfterPreScaling;
                }
                XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"Scaled crop[2.d] Force y=%u, height=%u\r\n",gSrcInfo[eWindow].ScaledCropWin.y, gSrcInfo[eWindow].ScaledCropWin.height);
            }
        }
        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"Scaled crop[3] x,y,w,h=%u,%u,%u,%u\r\n",gSrcInfo[eWindow].ScaledCropWin.x, gSrcInfo[eWindow].ScaledCropWin.y, gSrcInfo[eWindow].ScaledCropWin.width, gSrcInfo[eWindow].ScaledCropWin.height);
        MDrv_SC_3D_Disable_Crop(pSrcInfo, eWindow);
        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"Scaled crop[4] x,y,w,h=%u,%u,%u,%u\r\n",pSrcInfo->ScaledCropWin.x, pSrcInfo->ScaledCropWin.y, pSrcInfo->ScaledCropWin.width, pSrcInfo->ScaledCropWin.height);
    }
}

MS_U16 MDrv_SC_GetAutoPrescalingDst(XC_InternalStatus *pSrcInfo, MS_BOOL bHorizontal)
{
    MS_U16 u16ScaleDst;
    if(bHorizontal)
    {
        if ( pSrcInfo->stCropWin.width == 0 )
        {
            u16ScaleDst = 32;
        }
        else
        {
            u16ScaleDst = (MS_U16)(( (MS_U32)pSrcInfo->stCapWin.width * pSrcInfo->stDispWin.width + ( pSrcInfo->stCropWin.width / 2 ) ) / (MS_U32)pSrcInfo->stCropWin.width);
        }
    }
    else
    {
        if (pSrcInfo->stCropWin.height == 0) // error handling
        {
            u16ScaleDst = 32;
        }
        else
        {
            if( !pSrcInfo->bInterlace)
            {
                // Progressive
                // Pre-scaling to specific size. This formula will make sure driver only use one of Presacling or PostScaling
                u16ScaleDst = (MS_U16)(( (MS_U32)pSrcInfo->stCapWin.height * pSrcInfo->stDispWin.height + ( pSrcInfo->stCropWin.height / 2 ) ) / (MS_U32)pSrcInfo->stCropWin.height);
            }
            else
            {
                // Interlace
                u16ScaleDst = (pSrcInfo->stCapWin.height / 2);
            }
        }
    }
    return u16ScaleDst;
}


static void _MDrv_SC_set_preVSD_FIDCLK(SCALER_WIN eWindow)
{
    if(eWindow == MAIN_WINDOW || eWindow == SUB_WINDOW)
    {
        Hal_SC_set_FICLK(eWindow, gSrcInfo[MAIN_WINDOW].bPreV_ScalingDown, gSrcInfo[SUB_WINDOW].bPreV_ScalingDown);
    }
    else if(eWindow == SC2_MAIN_WINDOW || eWindow == SC2_SUB_WINDOW)
    {
        Hal_SC_set_FICLK(eWindow, gSrcInfo[SC2_MAIN_WINDOW].bPreV_ScalingDown, gSrcInfo[SC2_SUB_WINDOW].bPreV_ScalingDown);
    }
}



void MDrv_SC_set_prescaling_ratio(INPUT_SOURCE_TYPE_t enInputSourceType, XC_InternalStatus *pSrcInfo, SCALER_WIN eWindow)
{
    MS_U32 u32TmpPre;
    MS_U16 u16ScaleSrc = 0, u16ScaleDst = 0;
    MS_BOOL bEnPreDown = 0, bAdvMode = 0 , bBilinearMode = 0, bPQSupportBilinear = 0;
    MS_BOOL bInterlace = pSrcInfo->bInterlace;
    MS_U16 u16SrcHWidth = 0;
    MS_BOOL bForcePreVScalingDown = FALSE;
    MS_BOOL bForcePreHScalingDown = FALSE;

    MS_U16  u16LineBuffOffset = 0;
    MS_BOOL bTodo = FALSE, bForceChange = FALSE;

    // Store Pre-scaling source / dest size of previouse
    static MS_U16 _u16HScaleSrc[MAX_WINDOW], _u16HScaleDst[MAX_WINDOW];
    static MS_U16 _u16VScaleSrc[MAX_WINDOW], _u16VScaleDst[MAX_WINDOW];
    static MS_U8  _u8HSD_Idx = 0;

    // PQ related.
    PQ_HSD_SAMPLING_INFO stHSDSamplingInfo = {0,0};
    MS_U32 _u32HSD_Ratio[9] = {0x100000, 0x120000, 0x140000, 0x160000, 0x180000, 0x1A0000, 0x1C0000, 0x1E0000, 0x200000};
    MS_U32 _u32HSD_Out[9] = {1000, 888, 800, 728, 666, 616, 572, 534, 500};

	memset( _u16HScaleSrc, 0, MAX_WINDOW*sizeof(_u16HScaleSrc[0]) );
	memset( _u16HScaleSrc, 0, MAX_WINDOW*sizeof(_u16HScaleDst[0]) );
	memset( _u16HScaleSrc, 0, MAX_WINDOW*sizeof(_u16VScaleSrc[0]) );
	memset( _u16HScaleSrc, 0, MAX_WINDOW*sizeof(_u16VScaleDst[0]) );

    _bVAutoFitPrescalingEna = FALSE;
    _bHAutoFitPrescalingEna = FALSE;
    _bHSizeChangedManually = FALSE;


    // check HW limitation of post-scaling and return TRUE/FALSE flag of force H/V pre-scaling.
    Hal_XC_IsForcePrescaling(pSrcInfo, &bForcePreVScalingDown, &bForcePreHScalingDown, eWindow);

    //-----------------------------------------
    // Vertical
    //-----------------------------------------
    if (pSrcInfo->Status2.bPreVCusScaling == TRUE)
    {
        u16ScaleSrc = pSrcInfo->Status2.u16PreVCusScalingSrc;
        u16ScaleDst = pSrcInfo->Status2.u16PreVCusScalingDst;
    }
    else
    {
        // Put full image into memory as default
        u16ScaleSrc = u16ScaleDst = pSrcInfo->stCapWin.height;

        if ( !pSrcInfo->bFBL )
        {

            // Nine Lattice case
            if(pSrcInfo->bDisplayNineLattice)
            {
                u16ScaleDst = pSrcInfo->stDispWin.height;
            }
            else
            {
                if (bForcePreVScalingDown)
                {
                    u16ScaleDst = MDrv_SC_GetAutoPrescalingDst(pSrcInfo, FALSE);
                    if(!pSrcInfo->bInterlace)
                    {
                        _bVAutoFitPrescalingEna = TRUE;
                    }
                }

            }

            // Adjust for 3D
            u16ScaleDst = MDrv_SC_3D_Adjust_PreVerDstSize(enInputSourceType, pSrcInfo, u16ScaleDst, eWindow);
        }


    }

    if( bInterlace )
    {
        if (u16ScaleDst % 2 )
        {
            XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"==============================================================\n");
            XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"======= Pre-scaling V size have to align 2 !!!!!!!!===========\n");
            XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"==============================================================\n");
            MS_ASSERT(0);
            u16ScaleDst = ( u16ScaleDst + 1) & ~0x1;
        }
    }


    // Check prescaling status. Set bPreScalingRatioChanged as true only when source/dest is different with previous source/dest
    // We must update this information before pre-scaling filter.
    //(1. We can skip pre-scaling filter update. 2. We can decide which method we can use for load registers. DS or Menuload.)
    if ( (_u16VScaleSrc[eWindow] != u16ScaleSrc) || (_u16VScaleDst[eWindow] != u16ScaleDst) )
    {
        pSrcInfo->bPreScalingRatioChanged = TRUE;

        _u16VScaleSrc[eWindow] = u16ScaleSrc;
        _u16VScaleDst[eWindow] = u16ScaleDst;

    }
    else
    {
        pSrcInfo->bPreScalingRatioChanged = FALSE;
    }

    if(s_PQ_Function_Info.pq_ioctl)
    {
        s_PQ_Function_Info.pq_ioctl((PQ_WIN)eWindow,
                                   E_PQ_IOCTL_PREVSD_BILINEAR,
                                   (void *)&bPQSupportBilinear,
                                   sizeof(bPQSupportBilinear));
    }

    if(s_PQ_Function_Info.pq_load_scalingtable)
    {
        // dump prescaling filter
        bBilinearMode = s_PQ_Function_Info.pq_load_scalingtable((PQ_WIN)eWindow,
                                E_XRULE_VSD,
                                (u16ScaleSrc != u16ScaleDst ? TRUE : FALSE),
                                bInterlace,
                                pSrcInfo->bMemYUVFmt,
                                u16ScaleSrc,
                                u16ScaleDst);
    }
    else
    {
        bBilinearMode = 0;
    }

    if(bPQSupportBilinear == 0)
        bBilinearMode = 0;


    _XC_ENTRY();

    if (pSrcInfo->bFBL && pSrcInfo->bInterlace)
    {
        //HAL_SC_Enable_VInitFactor(FALSE,eWindow);//Let AP layer to decide enable/dsiable for FBL interlace
        HAL_SC_Set_VInitFactorOne(0x0,eWindow);
    }
    else
    {
        HAL_SC_Enable_VInitFactor(FALSE,eWindow);
        HAL_SC_Set_VInitFactorOne(0x0,eWindow);
    }

    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[PRE]eWindow[%u] VSrc=%u, VDst=%u, bBilinear=%u\r\n",eWindow , u16ScaleSrc, u16ScaleDst, bBilinearMode);

    if (u16ScaleSrc > u16ScaleDst) //pre scaling only support scaling down
    {
        MS_U32 u32Value;
        if(MDrv_SC_3D_AdjustPreVerSrcForFramepacking(eWindow))
        {
            if(bBilinearMode)
            {
                if (u16ScaleDst == 0 )
                    u32TmpPre = 0;
                else
                    u32TmpPre = V_PreScalingDownRatioBilinear(u16ScaleSrc+1, u16ScaleDst);
            }
            else
            {
                u32TmpPre = V_PreScalingDownRatio(u16ScaleSrc+1, u16ScaleDst);
            }
            SC_3D_DBG(printf("3D: special handle for 1080P double HD case, u16ScaleSrc+1=%u, u16ScaleDst=%u\r\n", u16ScaleSrc+1, u16ScaleDst);)
        }
        else
        {
            if(bBilinearMode)
            {
                if (u16ScaleDst == 0 )
                    u32TmpPre = 0;
                else
                    u32TmpPre = V_PreScalingDownRatioBilinear(u16ScaleSrc, u16ScaleDst);
            }
            else
            {
                if (u16ScaleSrc == 0)
                    u32TmpPre = 0;
                else
                    u32TmpPre = V_PreScalingDownRatio(u16ScaleSrc, u16ScaleDst);
            }
        }
        u32TmpPre &= 0xFFFFFFL;

        // Enable Bilinear mode.
        if (bBilinearMode)
        {
            u32TmpPre |= 0x40000000L;
        }

        u32TmpPre |= 0x80000000L;

        pSrcInfo->bPreV_ScalingDown = TRUE;

        // Error checking
        if (u16ScaleSrc == 0)
            u32Value = 0;
        else
            u32Value = (MS_U32)u16ScaleDst * 0x80000 / u16ScaleSrc;

        HAL_SC_Set_VInitFactorTwo(u32Value,eWindow);
        HAL_SC_Set_vsd_output_line_count(DISABLE,u16ScaleDst,eWindow);
        HAL_SC_Set_vsd_input_line_count(DISABLE,FALSE,0,eWindow);
    }
    else
    {
        _bVAutoFitPrescalingEna = FALSE;
        if ((eWindow == SUB_WINDOW)&&(MApi_XC_Get_3D_Output_Mode()==E_XC_3D_OUTPUT_MODE_NONE))
        {
            pSrcInfo->bPreV_ScalingDown = TRUE;
            // In T12/T13, when in PIP mode, sub window need to the same with the main window's pre scaling setting.
            // It means that if main window use pre-scaling, sub window must enable pre-scaling.
            // But in CB mode, it doesn't have x1 ratio, it need to use Adv mode to instead!!
            u32TmpPre = 0xC0100000L;
            // vsd_output_line let HW outputs the latest line of pre-scaling.
            // Otherwise HW won't output the latest line in Bi-linear mode..
            //when input source is interlace, don't refer to capture V size. it is (V capture size/2) //fixed DMS00956905/154423
            if (!pSrcInfo->bInterlace)
            {
                HAL_SC_Set_vsd_output_line_count(ENABLE,u16ScaleDst,eWindow);
                HAL_SC_Set_vsd_input_line_count(ENABLE,FALSE,0,eWindow);
            }
            else
            {
                HAL_SC_Set_vsd_output_line_count(ENABLE,u16ScaleDst / 2,eWindow);
                HAL_SC_Set_vsd_input_line_count(ENABLE,TRUE,u16ScaleDst / 2,eWindow);
            }
        }
        else
        {
            u16ScaleDst = u16ScaleSrc;
            u32TmpPre = 0;
            pSrcInfo->bPreV_ScalingDown = FALSE;
            HAL_SC_Set_vsd_output_line_count(DISABLE,u16ScaleDst,eWindow);
            HAL_SC_Set_vsd_input_line_count(DISABLE,FALSE,0,eWindow);

        }
        HAL_SC_Set_VInitFactorTwo(0x80000,eWindow);
    }

     _MDrv_SC_set_preVSD_FIDCLK(eWindow);

     _XC_RETURN();

    //store prescaling info here ( Why using u16PreVCusScalingSrc and u16PreVCusScalingDst??)
    pSrcInfo->Status2.u16PreVCusScalingSrc = u16ScaleSrc;
    pSrcInfo->Status2.u16PreVCusScalingDst = u16ScaleDst;
    pSrcInfo->u16V_SizeAfterPreScaling = u16ScaleDst;
    pSrcInfo->u16V_Length = pSrcInfo->u16V_SizeAfterPreScaling;

    MDrv_SC_3D_Adjust_FillLBMode(eWindow);
    MDrv_SC_3D_SetWriteMemory(eWindow);
    MDrv_SC_3D_CloneMainSettings(pSrcInfo, eWindow);
    MDrv_SC_3D_Adjust_PreVLength(enInputSourceType, pSrcInfo, eWindow);

    stDBreg.u32V_PreScalingRatio = u32TmpPre;

    //-----------------------------------------
    // Horizontal
    // HSD rule: 3D scaling > customer scaling > QMap HSD sampling > auto fit
    //-----------------------------------------

    // Note!! Formula:  FHD - If  (H Crop size /2 ) > ( H display size ), Must-do H pre-scaling down.

    if (pSrcInfo->bDisplayNineLattice)
    {
        u16SrcHWidth = pSrcInfo->stCapWin.width;
        pSrcInfo->u16H_SizeAfterPreScaling = pSrcInfo->stDispWin.width;
    }
    else
    {
        // Custimize
        if(pSrcInfo->Status2.bPreHCusScaling)
        {
            u16SrcHWidth = pSrcInfo->Status2.u16PreHCusScalingSrc;
            if (u16SrcHWidth > pSrcInfo->Status2.u16PreHCusScalingDst)
            {
                pSrcInfo->u16H_SizeAfterPreScaling = pSrcInfo->Status2.u16PreHCusScalingDst;
            }
            else
            {
                pSrcInfo->u16H_SizeAfterPreScaling = u16SrcHWidth;
            }
            MDrv_SC_3D_Adjust_PreHorDstSize(enInputSourceType, pSrcInfo, eWindow);
            // 3d adjust may make the pre h dst bigger than cus_pre_H, we need adjust it again.
            if (pSrcInfo->u16H_SizeAfterPreScaling > pSrcInfo->Status2.u16PreHCusScalingDst)
            {
                pSrcInfo->u16H_SizeAfterPreScaling = pSrcInfo->Status2.u16PreHCusScalingDst;
            }
        }
        else
        {
            u16SrcHWidth = pSrcInfo->stCapWin.width;
            pSrcInfo->u16H_SizeAfterPreScaling = pSrcInfo->stCapWin.width;

            // this DTV flow don't obey the HSD rule, but HDuplicate won't affect QMap non-prescaling case,
            // because the HDuplicate condition is no bigger than 704, refer to MVOP_AutoGenMPEGTiming()
            // need to refine later
            if(IsSrcTypeDTV(enInputSourceType) && pSrcInfo->bHDuplicate) // for better quality
            {
                MS_U16 capwidth = pSrcInfo->stCapWin.width / 2;
                if(eWindow == SUB_WINDOW)
                {
                    if (capwidth > pSrcInfo->stDispWin.width && g_XC_InitData.stPanelInfo.u16Width < 1800)
                        pSrcInfo->u16H_SizeAfterPreScaling = pSrcInfo->stCapWin.width;
                    else if(capwidth > pSrcInfo->stDispWin.width)
                        pSrcInfo->u16H_SizeAfterPreScaling = pSrcInfo->stDispWin.width;
                }
                else
                {
                    pSrcInfo->u16H_SizeAfterPreScaling = capwidth;
                }
                _bHSizeChangedManually = TRUE;
            }
         //!!NOTE START:
         //if QMAP support HSD Sampling, this item need to disable and the scaling size will control by QMAP
         //If we want to follow the HSD rule, we cannot enable ENABLE_YPBPR_PRESCALING_TO_ORIGINAL and
         // ENABLE_VD_PRESCALING_TO_DOT75
         #if (ENABLE_YPBPR_PRESCALING_TO_ORIGINAL)
            else if(IsSrcTypeYPbPr(enInputSourceType) && pSrcInfo->bHDuplicate)
            {
               pSrcInfo->u16H_SizeAfterPreScaling = pSrcInfo->stCapWin.width / 2;
               _bHSizeChangedManually = TRUE;
            }
         #endif
         //!!NOTE END
         #if (ENABLE_VD_PRESCALING_TO_DOT75)
            else if(IsSrcTypeDigitalVD(enInputSourceType))
            {

               pSrcInfo->u16H_SizeAfterPreScaling = pSrcInfo->stCapWin.width * 3 / 4;
               _bHSizeChangedManually = TRUE;
            }
         #endif
            else
            {
                // H scaling controlled by Qmap.

                if((eWindow == MAIN_WINDOW || eWindow == SC1_MAIN_WINDOW || eWindow == SC2_MAIN_WINDOW) && (s_PQ_Function_Info.pq_ioctl != NULL))
                {
                    s_PQ_Function_Info.pq_ioctl((PQ_WIN)eWindow, E_PQ_IOCTL_HSD_SAMPLING, &stHSDSamplingInfo, sizeof(PQ_HSD_SAMPLING_INFO));

                    // Using ratio from Qmap only in acceptable post-scaling
                    if(stHSDSamplingInfo.u32ratio)
                    {
                        switch(stHSDSamplingInfo.u32ratio)
                        {
                            default:
                            case 1000:
                                _u8HSD_Idx = 0;
                                break;

                            case 875:
                                _u8HSD_Idx = 1;
                                break;

                            case 750:
                                _u8HSD_Idx = 2;
                                break;

                            case 625:
                                _u8HSD_Idx = 3;
                                break;

                            case 500:
                                _u8HSD_Idx = 4;
                                break;

                            case 375:
                                _u8HSD_Idx = 5;
                                break;

                            case 250:
                                _u8HSD_Idx = 6;
                                break;

                            case 125:
                                _u8HSD_Idx = 7;
                                break;

                            case 1:
                                _u8HSD_Idx = 8;
                                break;
                         }
                         if(IsSrcTypeYPbPr(enInputSourceType) && pSrcInfo->bHDuplicate == 0)// Don't folow HSD sample when source is Ypbpr and HDuplicate is 0
                            pSrcInfo->u16H_SizeAfterPreScaling = (MS_U16)(((MS_U32)pSrcInfo->stCapWin.width * _u32HSD_Out[0])/1000);
                         else
                            pSrcInfo->u16H_SizeAfterPreScaling = (MS_U16)(((MS_U32)pSrcInfo->stCapWin.width * _u32HSD_Out[_u8HSD_Idx])/1000);
                         MDrv_SC_SetPQHSDFlag(TRUE, eWindow);
                         XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[PRE]PQ PreHDown %d\n",pSrcInfo->u16H_SizeAfterPreScaling);

                     }

                }


                 // Scaling donw only when special case.
                 if ((!MDrv_SC_GetPQHSDFlag(eWindow))&& bForcePreHScalingDown)
                 {
                     pSrcInfo->u16H_SizeAfterPreScaling = MDrv_SC_GetAutoPrescalingDst(pSrcInfo, TRUE);
                     _bHAutoFitPrescalingEna = TRUE;
                 }

                if (eWindow == SUB_WINDOW )
                {
                    // Notice!!!! Sub win only have 960 pixels.
                    // If we use 1280 pixel for Sub Win, Main win has 1280 pixels remain. (Total line buffer : 1920 + 960 )
                    #if (HW_DESIGN_3D_VER == 4)
                    if(!((MApi_XC_Get_3D_Input_Mode(eWindow)==E_XC_3D_INPUT_FRAME_PACKING)&&
                       (MApi_XC_Get_3D_Output_Mode()==E_XC_3D_OUTPUT_TOP_BOTTOM)))
                    #endif
                    {
                        if ( pSrcInfo->u16H_SizeAfterPreScaling > 960 )
                        {
                            pSrcInfo->u16H_SizeAfterPreScaling = 960 ;
                        }
                    }
                }

                bForceChange = (g_XC_InitData.stPanelInfo.u16Width==pSrcInfo->stDispWin.width)?TRUE:FALSE;

                AdjustWinLineBufferSize( MAX(pSrcInfo->stDispWin.width, pSrcInfo->u16H_SizeAfterPreScaling), eWindow);

                XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[PRE]Force Prescaling Down : %d  _bPQSetPreScalingrate : %d \r\r\n",bForcePreHScalingDown, MDrv_SC_GetPQHSDFlag());
            }

            {
                MS_U16 u16OriHSize = pSrcInfo->u16H_SizeAfterPreScaling;
                MDrv_SC_3D_Adjust_PreHorDstSize(enInputSourceType, pSrcInfo, eWindow);
                if(u16OriHSize != pSrcInfo->u16H_SizeAfterPreScaling)
                {
                    _bHSizeChangedManually = TRUE;
                    printf("xc: 3D has changed the H prescaling setting, need to skip PQ HSD_Sampling/HSD_Y/HSD_C loading\r\n");
                }
            }
         }
    }

    //Setting Line Buffer Start offset for Sub Win

    _XC_ENTRY();

    u16LineBuffOffset = GetSubStartOffsetLB(eWindow, bForceChange, &bTodo);

    if(bTodo)
    {
		#ifdef MULTI_SCALER_SUPPORTED
		HAL_SC_SetSubLineBufferOffset(u16LineBuffOffset, eWindow);
		#else
        HAL_SC_SetSubLineBufferOffset(u16LineBuffOffset);
		#endif

		#if (HW_DESIGN_3D_VER <= 1)
        MDrv_SC_3D_Adjust_SubLineBufferOffset(enInputSourceType, pSrcInfo, eWindow);
		#endif
    }

    _XC_RETURN();

    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"eWindow[%u] HSrc=%u, HDst=%u\r\n", eWindow, u16SrcHWidth, pSrcInfo->u16H_SizeAfterPreScaling);

    if(pSrcInfo->u16H_SizeAfterPreScaling & BIT(0))
    {
        //YUV422, h size must be even to prevent showing of garbage at the end of line
        if(pSrcInfo->u16H_SizeAfterPreScaling < pSrcInfo->stCapWin.width)
        {
            if(MDrv_SC_GetPQHSDFlag(eWindow) == TRUE)
            {
                pSrcInfo->u16H_SizeAfterPreScaling = (pSrcInfo->u16H_SizeAfterPreScaling - 1) & ~0x1;
            }
            else
            {
                pSrcInfo->u16H_SizeAfterPreScaling = (pSrcInfo->u16H_SizeAfterPreScaling + 1) & ~0x1;
            }
        }
        else
        {
            pSrcInfo->u16H_SizeAfterPreScaling = u16SrcHWidth;
        }
    }

    // Check prescaling status. Set bPreScalingRatioChanged as true only when src != dest.
    // We update this information before pre-scaling filter.
    if ( (_u16HScaleSrc[eWindow] != u16SrcHWidth) || (_u16HScaleDst[eWindow] != pSrcInfo->u16H_SizeAfterPreScaling) )
    {
        gSrcInfo[eWindow].bPreScalingRatioChanged = TRUE;

        _u16HScaleSrc[eWindow] = u16SrcHWidth;
        _u16HScaleDst[eWindow] = pSrcInfo->u16H_SizeAfterPreScaling;
    }
    else
    {
        gSrcInfo[eWindow].bPreScalingRatioChanged |= FALSE;
    }

    //always load scaling table.
    {

        if(s_PQ_Function_Info.pq_load_scalingtable == NULL)
        {
            bAdvMode = 0;
        }
        else
        {
            bAdvMode =
                s_PQ_Function_Info.pq_load_scalingtable((PQ_WIN)eWindow,
                                    E_XRULE_HSD,
                                    pSrcInfo->bPreV_ScalingDown,
                                    bInterlace,
                                    pSrcInfo->bMemYUVFmt,
                                    u16SrcHWidth,
                                    pSrcInfo->u16H_SizeAfterPreScaling);
        }
    }

    //if customer scaling case, ratio and filter is decided by Customer and Xrule
    //if not customer and PQ HSD sampling case, ratio and filter is decided by PQ
    //if not customer and not PQ HSD sampling case, ratio and filter is decided by driver autofit and Xrule
    //Note: if HSD_Y, HSD_C is in PQ skip rule, and the skip is Y, the driver logic will be very carefully.
    //we should treat it as "none PQ setting HSD sampling" case.
    //because HSD filter is related with bAdvMode, which are paired.
    if((MDrv_SC_GetPQHSDFlag(eWindow)) && (!pSrcInfo->Status2.bPreHCusScaling) && (!_bHSizeChangedManually))
    {
        bAdvMode = stHSDSamplingInfo.bADVMode;
    }

#if 1    // Dixon, FixMe, subwindow 960 no pre-scaling down can't use ADV mode
    if(eWindow == SUB_WINDOW)
    {
            bAdvMode = FALSE;
    }
#endif

    //printf("bEnPreDown=%u, bAdvMode=%u\r\n", (MS_U16)bEnPreDown, (MS_U16)bAdvMode);
    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[PRE]eWindow[%u] HSrc=%u, HDst=%u, bAdvMode=%u\r\n", eWindow, u16SrcHWidth, pSrcInfo->u16H_SizeAfterPreScaling, bAdvMode);

    if (u16SrcHWidth > pSrcInfo->u16H_SizeAfterPreScaling)
    {
        MS_U16 u16div = 1;

        // To avoid overflow when do H_PreScalingDownRatioAdv/H_PreScalingDownRatioAdv_No_Minus1
        if(u16SrcHWidth > 4096)
        {
            while(1)
            {
                MS_U16 u16ret;

                u16div <<= 1;

                if (u16div == 0)
                {
                    break;
                }

                u16ret = u16SrcHWidth/u16div;

                if(u16ret == 0)
                {
                    u16div = 1;
                    break;
                }
                else if(u16ret< 4096 && u16ret > pSrcInfo->u16H_SizeAfterPreScaling)
                {
                    break;
                }
            }
        }

        if(MDrv_SC_GetPQHSDFlag(eWindow))
        {
            //PQ set HPrescaling case, don't use bAdvMode
            u32TmpPre = _u32HSD_Ratio[_u8HSD_Idx];
        }
        else
        {
            if (bAdvMode)
            { // enable prescaling fitler
                u32TmpPre = (H_PreScalingDownRatioAdv((u16SrcHWidth/u16div), pSrcInfo->u16H_SizeAfterPreScaling)) * u16div;
            }
            else
            { // disable prescaling filter, use cb mode scaling ratio
                if (u16SrcHWidth == 0 )
                    u32TmpPre = 0;
                else
                    u32TmpPre = (H_PreScalingDownRatio(u16SrcHWidth, pSrcInfo->u16H_SizeAfterPreScaling));
            }
        }
        bEnPreDown = 1;
        u32TmpPre &= 0x7FFFFFL;
    }
    else
    {
        _bHAutoFitPrescalingEna = FALSE;
        pSrcInfo->u16H_SizeAfterPreScaling = u16SrcHWidth;
        if(bAdvMode)
        {
            bEnPreDown = 1;
            u32TmpPre = 0x100000L;
        }
        else
        {
            bEnPreDown = 0;
            u32TmpPre = 0x100000L;
        }

#if 1    // Dixon, FixMe, subwindow 960 no pre-scaling down can't turn on scaling and set ratio to 1
        if(eWindow==SUB_WINDOW)
            bEnPreDown = 0;
#endif
    }
    //store prescaling info here
    pSrcInfo->Status2.u16PreHCusScalingSrc = u16SrcHWidth;
    pSrcInfo->Status2.u16PreHCusScalingDst = pSrcInfo->u16H_SizeAfterPreScaling;

    if (bEnPreDown)
    {
        if(bAdvMode)
        {
            u32TmpPre |= 0x40000000L;
        }

        u32TmpPre |= 0x80000000L;
    }

    stDBreg.u32H_PreScalingRatio = u32TmpPre;
    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[PRE]HRatio=0x%lx, VRatio=0x%lx, u16V_Length = %u\r\n", stDBreg.u32H_PreScalingRatio, stDBreg.u32V_PreScalingRatio, pSrcInfo->u16V_Length);
}

void MDrv_SC_set_postscaling_ratio(XC_InternalStatus *pSrcInfo, SCALER_WIN eWindow)
{
    #define MAX_PST_SCALING_DOWN_PERCENTAGE_FCLK170M (80)
    MS_U32 u32TmpPost=0;
    MS_U16 u16ScaleSrc, u16ScaleDst;
    MS_BOOL bInterlace = pSrcInfo->bInterlace, bPstScalingDownCheck;
    //Check pst scaling limit for FULL HD panel, and currently only check FULL HD input
    bPstScalingDownCheck = ((MDrv_ReadByte(REG_CKG_FCLK) & CKG_FCLK_MASK) == CKG_FCLK_170MHZ) ?
                            (g_XC_InitData.stPanelInfo.u16Width == 1920 && pSrcInfo->ScaledCropWin.width > 1280) : FALSE;
    //-----------------------------------------
    // Horizontal
    //-----------------------------------------
    if (pSrcInfo->bHCusScaling == FALSE)
    {
        u16ScaleSrc = pSrcInfo->ScaledCropWin.width;
        u16ScaleDst = pSrcInfo->stDispWin.width;

        u16ScaleSrc = MDrv_SC_3D_Adjust_PostHorSrcSize(pSrcInfo, u16ScaleSrc, eWindow);

        u16ScaleDst = MDrv_SC_3D_Adjust_PostHorDstSize(pSrcInfo, u16ScaleDst, eWindow);
    }
    else
    {
        u16ScaleSrc = pSrcInfo->u16HCusScalingSrc;
        u16ScaleDst = pSrcInfo->u16HCusScalingDst;
        //make sure crop window in the range of window after prescaling
        if((u16ScaleSrc > pSrcInfo->u16H_SizeAfterPreScaling) ||
           (u16ScaleSrc + pSrcInfo->ScaledCropWin.x > pSrcInfo->u16H_SizeAfterPreScaling))
        {
            MS_U16 u16PstScaledWidth;
            u16PstScaledWidth = (MS_U16)((MS_U32)(u16ScaleDst*(pSrcInfo->u16H_SizeAfterPreScaling - pSrcInfo->ScaledCropWin.x))/u16ScaleSrc);
            //Check if the scaled content can full fill the display window, otherwise garbage will show out.
            if(u16PstScaledWidth < pSrcInfo->stDispWin.width)
            {
                if(u16ScaleSrc > pSrcInfo->u16H_SizeAfterPreScaling)
                {
                    u16ScaleSrc = pSrcInfo->u16H_SizeAfterPreScaling & ~0x01;
                    pSrcInfo->ScaledCropWin.x = 0;
                    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[PST]Limit ScaledCropWin.x to x=%u, HScaleSrc to: Src=%u\r\n", pSrcInfo->ScaledCropWin.x, u16ScaleSrc);
                }
                else
                {
                    pSrcInfo->ScaledCropWin.x = (pSrcInfo->u16H_SizeAfterPreScaling - u16ScaleSrc) & ~0x01;
                    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[PST]Limit ScaledCropWin.x to x=%u\r\n", pSrcInfo->ScaledCropWin.x);
                }
            }
        }
    }
    if(pSrcInfo->bDisplayNineLattice)
    {
        // nine lattice disable post scaling
        u16ScaleSrc = u16ScaleDst = 1920;
    }
    //store pst scaling info here
    pSrcInfo->u16HCusScalingSrc = u16ScaleSrc;
    pSrcInfo->u16HCusScalingDst = u16ScaleDst;
    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[PST]HSrc=%u, HDst=%u\r\n", u16ScaleSrc, u16ScaleDst);

    if(bPstScalingDownCheck)
    {
        if((u16ScaleDst < u16ScaleSrc) && (u16ScaleSrc*MAX_PST_SCALING_DOWN_PERCENTAGE_FCLK170M/100) > u16ScaleDst)
        {
            MS_ASSERT(0);
            printf("[PST]!!!!!! ALERT: This FCLK can not support H PST scaling down too much[%u -> %u] !!!!!! \n", u16ScaleSrc, u16ScaleDst);
        }
    }
    if (u16ScaleDst != u16ScaleSrc){
        u32TmpPost = H_PostScalingRatio(u16ScaleSrc, u16ScaleDst);
        u32TmpPost &= 0xFFFFFFL;
        #if 1//( CHIP_FAMILY_TYPE == CHIP_FAMILY_S4LE )
        u32TmpPost |= 0x1000000L;
        #else
        u32TmpPost |= 0x800000L;
        #endif
    }
    else
    {
        u32TmpPost = 0x100000L;
    }
    if (s_PQ_Function_Info.pq_load_scalingtable != NULL)
    {
        s_PQ_Function_Info.pq_load_scalingtable((PQ_WIN)eWindow,
                                E_XRULE_HSP,
                                pSrcInfo->bPreV_ScalingDown,
                                bInterlace,
                                pSrcInfo->bMemYUVFmt,
                                u16ScaleSrc,
                                u16ScaleDst);
    }

    stDBreg.u32H_PostScalingRatio = u32TmpPost;

    //-----------------------------------------
    // Vertical
    //-----------------------------------------
    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[PST]VCusScaling=%s\r\n",pSrcInfo->bVCusScaling ? "TRUE":"FALSE");
    if (pSrcInfo->bVCusScaling == FALSE)
    {
        u16ScaleSrc = pSrcInfo->ScaledCropWin.height;
        u16ScaleDst = pSrcInfo->stDispWin.height;
        u16ScaleSrc = MDrv_SC_3D_Adjust_PostVerSrcSize(pSrcInfo, u16ScaleSrc, eWindow);
        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[PST]VSrc=%u, VDst=%u, eDeInterlaceMode=%u, bFBL=%u, bInterlace=%u\r\n", u16ScaleSrc, u16ScaleDst, pSrcInfo->eDeInterlaceMode, pSrcInfo->bFBL, bInterlace)
    }
    else
    {
        u16ScaleSrc = pSrcInfo->u16VCusScalingSrc;
        u16ScaleDst = pSrcInfo->u16VCusScalingDst;
        //make sure crop window in the range of window after prescaling
        if((u16ScaleSrc > pSrcInfo->u16V_SizeAfterPreScaling) ||
           (u16ScaleSrc + pSrcInfo->ScaledCropWin.y > pSrcInfo->u16V_SizeAfterPreScaling))
        {
            MS_U16 u16PstScaledHeight;
            u16PstScaledHeight = (MS_U16)((MS_U32)(u16ScaleDst*(pSrcInfo->u16V_SizeAfterPreScaling - pSrcInfo->ScaledCropWin.y))/u16ScaleSrc);
            //Check if the scaled content can full fill the display window, otherwise garbage will show out.
            if(u16PstScaledHeight < pSrcInfo->stDispWin.height)
            {
                if(u16ScaleSrc > pSrcInfo->u16V_SizeAfterPreScaling)
                {
                    u16ScaleSrc = pSrcInfo->u16V_SizeAfterPreScaling & ~0x01;
                    pSrcInfo->ScaledCropWin.y = 0;
                    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[PST]Limit ScaledCropWin.y to y=%u, VScaleSrc to: Src=%u\r\n", pSrcInfo->ScaledCropWin.y, u16ScaleSrc);
                }
                else
                {
                    pSrcInfo->ScaledCropWin.y = (pSrcInfo->u16V_SizeAfterPreScaling - u16ScaleSrc) & ~0x01;
                    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[PST]Limit ScaledCropWin.y to y=%u\r\n", pSrcInfo->ScaledCropWin.y);
                }
            }
        }
    }

    if(pSrcInfo->bDisplayNineLattice)
    {
        // nine lattice disable post scaling
        u16ScaleSrc = u16ScaleDst = 1080;
    }
    //store pst scaling info here
    pSrcInfo->u16VCusScalingSrc = u16ScaleSrc;
    pSrcInfo->u16VCusScalingDst = u16ScaleDst;
    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[PST]VSrc=%u, VDst=%u, eDeInterlaceMode=%u, bFBL=%u, bInterlace=%u\r\n", u16ScaleSrc, u16ScaleDst, pSrcInfo->eDeInterlaceMode, pSrcInfo->bFBL, bInterlace);
    if(bPstScalingDownCheck)
    {
        if((u16ScaleDst < u16ScaleSrc) && (u16ScaleSrc*MAX_PST_SCALING_DOWN_PERCENTAGE_FCLK170M/100) > u16ScaleDst)
        {
            MS_ASSERT(0);
            printf("[PST]!!!!!! ALERT: This FCLK can not support V PST scaling down too much[%u -> %u] !!!!!! \n", u16ScaleSrc, u16ScaleDst);
        }
    }
    if (u16ScaleSrc == u16ScaleDst)
    {
        // no need to do V post scaling
        if( bInterlace && (pSrcInfo->eDeInterlaceMode <= MS_DEINT_2DDI_BOB) )
        {
            if ( (u16ScaleDst-1) == 0 )
                u32TmpPost = 0;
            else
                u32TmpPost = V_PostScalingRatio(u16ScaleSrc/2, u16ScaleDst);
        }
        else
        {
            u32TmpPost = 0x100000L;
        }
    }
    else
    {
        // need to do V post scaling
        if( bInterlace && (pSrcInfo->eDeInterlaceMode <= MS_DEINT_2DDI_BOB))
        // for T2, 25/27 mode input line is full lines, BOB mode is half lines
        {
            if ( (u16ScaleDst-1) == 0)
                u32TmpPost = 0 ;
            else
                u32TmpPost = V_PostScalingRatio(u16ScaleSrc/2, u16ScaleDst);
        }
        else
        {
            if ( (u16ScaleDst-1) == 0)
                u32TmpPost = 0 ;
            else
                u32TmpPost = V_PostScalingRatio(u16ScaleSrc, u16ScaleDst);
        }
    }

    u32TmpPost &= 0xFFFFFFL;    //scaling factor support 24bits

    if (s_PQ_Function_Info.pq_load_scalingtable != NULL)
    {
        s_PQ_Function_Info.pq_load_scalingtable((PQ_WIN)eWindow,
                                E_XRULE_VSP,
                                pSrcInfo->bPreV_ScalingDown,
                                bInterlace,
                                pSrcInfo->bMemYUVFmt,
                                u16ScaleSrc,
                                u16ScaleDst);
    }

    if (u32TmpPost != 0x100000L)
    #if 1//( CHIP_FAMILY_TYPE == CHIP_FAMILY_S4LE )
        u32TmpPost |= 0x1000000L;
    #else
        u32TmpPost |= 0x800000L;
    #endif

    pSrcInfo->u16V_Length = u16ScaleSrc;
    MDrv_SC_3D_Adjust_PostVLength(pSrcInfo, eWindow);

    stDBreg.u32V_PostScalingRatio = u32TmpPost;
    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"[PST]HRatio=0x%lx, VRatio=0x%lx, u16V_Length=%u\r\n", stDBreg.u32H_PostScalingRatio, stDBreg.u32V_PostScalingRatio, pSrcInfo->u16V_Length);
}

#define _NOP_(_loop)           { volatile int i; for (i=0; i<(_loop); i++); }

/// Decide best frame buffer number used.
void _Mdrv_SC_CalcFBNum(  MS_BOOL *pIsLinearMode,
                        XC_FRAME_STORE_NUMBER *pFrameStoreNumber,
                        MS_BOOL bInterlace,
                        MS_U32 u32Framesize,
                        MS_U32 u32DNRBufSize,
                        SCALER_WIN eWindow)
{
    MS_U32 u32Dynamic_scale_size = (MS_U32)gSrcInfo[eWindow].Status2.u16PreHCusScalingSrc*
                                gSrcInfo[eWindow].Status2.u16PreVCusScalingSrc*
                                gSrcInfo[eWindow].u8BitPerPixel * 2 / 8 /2;
    *pIsLinearMode = FALSE;

#if (HW_DESIGN_3D_VER > 2)
    if(E_XC_3D_INPUT_FRAME_ALTERNATIVE == MApi_XC_Get_3D_Input_Mode(eWindow))
    {//per cc, input 3d:framealternative will double data at dnr
        u32Framesize *= 2;
    }
#endif

    if(bInterlace)
    {
        *pFrameStoreNumber = IMAGE_STORE_4_FIELDS;
    }
    else
    {
        *pFrameStoreNumber = IMAGE_STORE_2_FRAMES;
    }

// no interlace fall into dynamic scaling.
    if(HAL_SC_Get_DynamicScaling_Status()&&(eWindow == MAIN_WINDOW))
    {
        if(u32Dynamic_scale_size*4<u32DNRBufSize)
        {
            *pFrameStoreNumber = IMAGE_STORE_4_FRAMES;
        }
        #if (_3FRAME_BUFFER_PMODE_SUPPORTED)
        else if(u32Dynamic_scale_size*3 < u32DNRBufSize)
        {
            *pFrameStoreNumber = IMAGE_STORE_3_FRAMES;
            #if (_LINEAR_ADDRESS_MODE_SUPPORTED)
                *pIsLinearMode = TRUE;
            #endif
        }
        #endif
        else
        {
            *pFrameStoreNumber = IMAGE_STORE_2_FRAMES;
        }

        return;
    }

    if(!bInterlace )
    {

        #if (_12FRAME_BUFFER_PMODE_SUPPORTED )
            // Automatically change to 12frame mode if DNR buffer is enought.
            if(u32Framesize*12 < u32DNRBufSize  && Hal_SC_GetFrameStoreMode(eWindow) == IMAGE_STORE_12_FRAMES)
            {
                // 12 frame mode need to be set as linear mode.
                #if  (_LINEAR_ADDRESS_MODE_SUPPORTED)
                *pIsLinearMode = TRUE;
                #endif

                *pFrameStoreNumber = IMAGE_STORE_12_FRAMES;
                XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"%ld*3 < %ld\r\n", u32Framesize, u32DNRBufSize );
            }
            else
        #endif

        #if (_8FRAME_BUFFER_PMODE_SUPPORTED)
            // 8 frame mode is set by Qmap.
            if (u32Framesize*8 < u32DNRBufSize && Hal_SC_GetFrameStoreMode(eWindow) == IMAGE_STORE_8_FRAMES)
            {
                XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"%ld*8 < %ld\r\n", u32Framesize, u32DNRBufSize );
                *pFrameStoreNumber = IMAGE_STORE_8_FRAMES;
            }
            else
        #endif

        #if (_6FRAME_BUFFER_PMODE_SUPPORTED )
            // Automatically change to 6frame mode if DNR buffer is enought.
            if(u32Framesize*6 < u32DNRBufSize  && Hal_SC_GetFrameStoreMode(eWindow) == IMAGE_STORE_6_FRAMES)
            {
                // 6 frame mode need to be set as linear mode.
                #if (_LINEAR_ADDRESS_MODE_SUPPORTED)
                *pIsLinearMode = TRUE;
                #endif

                *pFrameStoreNumber = IMAGE_STORE_6_FRAMES;
                XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"%ld*3 < %ld\r\n", u32Framesize, u32DNRBufSize );
            }
            else
        #endif

        #if (_4FRAME_BUFFER_PMODE_SUPPORTED)
            // Automatically change to 4 frame mode if DNR buffer is enought.
            if (u32Framesize*4 < u32DNRBufSize && Hal_SC_GetFrameStoreMode(eWindow) == IMAGE_STORE_4_FRAMES)
            {
                XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"%ld*4 < %ld\r\n", u32Framesize, u32DNRBufSize );

                *pFrameStoreNumber = IMAGE_STORE_4_FRAMES;
            }
            else
        #endif


        #if (_3FRAME_BUFFER_PMODE_SUPPORTED)
            // Automatically change to 3frame mode if DNR buffer is enought.
            if(u32Framesize*3 < u32DNRBufSize)
            {
                // 3 frame mode need to be set as linear mode.
                #if (_LINEAR_ADDRESS_MODE_SUPPORTED)
                *pIsLinearMode = TRUE;
                #endif

                *pFrameStoreNumber = IMAGE_STORE_3_FRAMES;
                XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"%ld*3 < %ld\r\n", u32Framesize, u32DNRBufSize );
            }
        #endif
    }

#if (ENABLE_16_FIELD_SUPPORTED)
    else if(ENABLE_16_FIELD_SUPPORTED && bInterlace)
    {
        XC_FRAME_STORE_NUMBER enFrameMode = Hal_SC_GetFrameStoreMode(eWindow);

        switch( enFrameMode )
        {
            case IMAGE_STORE_6_FIELDS:
                if ((u32Framesize*3) < u32DNRBufSize )
                {
                    *pFrameStoreNumber = IMAGE_STORE_6_FIELDS;
                    XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"ENABLE_6_FIELD_SUPPORTED %d  \n",ENABLE_16_FIELD_SUPPORTED);
                }
                break;

            case IMAGE_STORE_8_FIELDS:
                if ((u32Framesize*4) < u32DNRBufSize )
                {
                    *pFrameStoreNumber = IMAGE_STORE_8_FIELDS;
                    XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"ENABLE_8_FIELD_SUPPORTED %d  \n",ENABLE_16_FIELD_SUPPORTED);
                }
                break;

            case IMAGE_STORE_12_FIELDS:
                if ((u32Framesize*6) < u32DNRBufSize )
                {
                    *pFrameStoreNumber = IMAGE_STORE_12_FIELDS;
                    XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"ENABLE_12_FIELD_SUPPORTED %d  \n",ENABLE_16_FIELD_SUPPORTED);
                }
                break;

            case IMAGE_STORE_14_FIELDS:
                if ((u32Framesize*7) < u32DNRBufSize )
                {
                    *pFrameStoreNumber = IMAGE_STORE_14_FIELDS;
                    XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"ENABLE_14_FIELD_SUPPORTED %d  \n",ENABLE_16_FIELD_SUPPORTED);
                }
                break;

            case IMAGE_STORE_16_FIELDS:
                if ((u32Framesize*8) < u32DNRBufSize )
                {
                    *pFrameStoreNumber = IMAGE_STORE_16_FIELDS;
                    XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"ENABLE_16_FIELD_SUPPORTED %d  \n",ENABLE_16_FIELD_SUPPORTED);
                }
                break;
            default:
                break;
        }
    }
#endif

    // Adjust frame mode by 3D content
    *pFrameStoreNumber = MDrv_SC_3D_Adjust_FBNum(*pFrameStoreNumber, eWindow);
    if(*pFrameStoreNumber != IMAGE_STORE_3_FRAMES)
    {
        *pIsLinearMode = FALSE;
        SC_3D_DBG(printf("if it's not 3 frame mode, linear mode should also be closed\r\n");)
    }

    XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"u8BestFBNum %d \n",*pFrameStoreNumber);

}

// for mirror mode and dynamic scaling store u32BaseOffset
void MDrv_SC_set_DNRBaseOffset(SCALER_WIN eWindow,MS_U32 u32BaseOffset)
{
     u32DNRBaseOffset[eWindow] = u32BaseOffset * BYTE_PER_WORD;
}

MS_U32  MApi_XC_Get_DNRBaseOffset(SCALER_WIN eWindow)
{
    return u32DNRBaseOffset[eWindow];
}

void MDrv_SC_set_storeFrameNUM(SCALER_WIN eWindow,MS_U8 u8FrameNum)
{
    u8StoreFrameNum[eWindow] = u8FrameNum;
}

MS_U8 MApi_XC_Get_StoreFrameNum(SCALER_WIN eWindow)
{
    return u8StoreFrameNum[eWindow];
}

MS_U8 MDrv_SC_Get_FrameStoreNum_Factor(XC_FRAME_STORE_NUMBER eFrameStoreNumber, MS_BOOL bLinearMode)
{
    MS_U8 u8Factor = 0;

    switch(eFrameStoreNumber)
    {
#if ENABLE_16_FIELD_SUPPORTED
        case IMAGE_STORE_6_FIELDS:

        #if (_FIELD_PACKING_MODE_SUPPORTED)
            u8Factor = 6;
        #else
            u8Factor = 3;
        #endif
            break;

        case IMAGE_STORE_8_FIELDS:

            #if (_FIELD_PACKING_MODE_SUPPORTED)
            u8Factor = 8;
            #else
            u8Factor = 4;
            #endif

            break;

        case IMAGE_STORE_10_FIELDS:

            #if (_FIELD_PACKING_MODE_SUPPORTED)
            u8Factor = 10;
            #else
            u8Factor =  5;
            #endif

            break;

        case IMAGE_STORE_12_FIELDS:

            #if (_FIELD_PACKING_MODE_SUPPORTED)
            u8Factor =  12;
            #else
            u8Factor =  6;
            #endif

        break;
        case IMAGE_STORE_14_FIELDS:

             #if (_FIELD_PACKING_MODE_SUPPORTED)
             u8Factor =  14;
             #else
             u8Factor =  7;
             #endif

             break;
        case IMAGE_STORE_16_FIELDS:

            #if (_FIELD_PACKING_MODE_SUPPORTED)
            u8Factor =  16;
            #else
            u8Factor =  8;
            #endif

        break;
#endif

        case IMAGE_STORE_3_FRAMES:

            if (bLinearMode)
            {
                u8Factor =  1;
            }
            else
            {
                // Put all field together.
                #if (_FIELD_PACKING_MODE_SUPPORTED)
                u8Factor =  3;
                #endif
            }

        break;

        case IMAGE_STORE_4_FRAMES:

            if (bLinearMode)
            {
                u8Factor =  1;
            }
            else
            {
                // Put all field together.
                #if (_FIELD_PACKING_MODE_SUPPORTED)
                u8Factor =  4;
                #else
                u8Factor =  2;
                #endif
            }

        break;
        case IMAGE_STORE_5_FRAMES:

            if (bLinearMode)
            {
                u8Factor =  1;
            }
            else
            {
                // Put all field together.
                #if (_FIELD_PACKING_MODE_SUPPORTED)
                u8Factor =  5;
                #else
                u8Factor =  2;
                #endif
            }

        break;
        case IMAGE_STORE_6_FRAMES:

            if (bLinearMode)
            {
                u8Factor =  1;
            }
            else
            {
                // Put all field together.
                #if (_FIELD_PACKING_MODE_SUPPORTED)
                u8Factor =  6;
                #else
                u8Factor =  2;
                #endif
            }

        break;
        case IMAGE_STORE_7_FRAMES:

            if (bLinearMode)
            {
                u8Factor =  1;
            }
            else
            {
                // Put all field together.
                #if (_FIELD_PACKING_MODE_SUPPORTED)
                u8Factor =  7;
                #else
                u8Factor =  2;
                #endif
            }

        break;

        case IMAGE_STORE_8_FRAMES:

            if (bLinearMode)
            {
                u8Factor =  1;
            }
            else
            {
                #if (_FIELD_PACKING_MODE_SUPPORTED)
                u8Factor =  8;
                #else
                u8Factor =  4;
                #endif
            }
            break;

        case IMAGE_STORE_10_FRAMES:

            if (bLinearMode)
            {
                u8Factor =  1;
            }
            else
            {
                // Put all field together.
                #if (_FIELD_PACKING_MODE_SUPPORTED)
                u8Factor =  10;
                #else
                u8Factor =  2;
                #endif
            }

        break;

        case IMAGE_STORE_12_FRAMES:

            if (bLinearMode)
            {
                u8Factor =  1;
            }
            else
            {
                // Put all field together.
                #if (_FIELD_PACKING_MODE_SUPPORTED)
                u8Factor =  12;
                #else
                u8Factor =  2;
                #endif
            }

        break;

        case IMAGE_STORE_4_FIELDS:
            if (bLinearMode)
            {
                u8Factor =  1;
            }
            else
            {
                // Put all field together.
                #if (_FIELD_PACKING_MODE_SUPPORTED)
                u8Factor =  4;
                #else
                u8Factor =  2;
                #endif
            }
            break;

        case IMAGE_STORE_2_FRAMES:
        default:
            if (bLinearMode)
            {
                u8Factor =  1;
            }
            else
            {
                // 2 frame mode is same for packet or non packet mode
                u8Factor =  2;
            }
            break;
    }

    return u8Factor;
}


/******************************************************************************/
///This function will calculate:
// [DNR]
// DNROffset, DNRFetch, DNRBase1, u32WritelimitBase
// [OPM]
// OPMFetch, OPMBase0, OPMBase1, LBOffset
// [OP1]
// u16VLen
/******************************************************************************/
void MDrv_SC_set_fetch_number_limit(XC_InternalStatus *pSrcInfo, SCALER_WIN eWindow)
{
    MS_U16 u16V_Writelimit, u16LBOffset, u16DispOffset, u16OPMFetch;
    MS_U32 u32Offset, u32DNRBase0, u32DNRBase1, u32DNRBase2, u32WritelimitBase=0;
    MS_U32 u32OPMBase0, u32OPMBase1, u32OPMBase2, u32OPMCropOffset =0, u32OPMPixelOffset;
    MS_BOOL bInterlace = pSrcInfo->bInterlace;
    MS_U16 u16AlignWidth=0;
    MS_U32 u32BaseOffset;
    MS_U32          u32Framesize;
    MS_U8           u8BytesPer2Pixel = pSrcInfo->u8BitPerPixel * 2 / 8;
    MS_U16 u16DNROffset;
    MS_U8 u8FrameNum=0;
	MS_U16 CropWinWidthTemp;
//TODO: buffer addr for sub win
//printf("MDrv_SC_set_fetch_number_limit  %d \n",u8BytesPer2Pixel );
    /////////////////////////////////////////////////
    // Set DNR Memeory Settings                    //
    /////////////////////////////////////////////////
    // Cal DNR Fetch/Offset
    gSrcInfo[eWindow].u16IPMFetch = pSrcInfo->u16H_SizeAfterPreScaling;
    XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"===================================================\n");
    XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"=> u16DNRFetch=%d ->",gSrcInfo[eWindow].u16IPMFetch);
#if (HW_IPM_FETCH_ALIGNMENT==FALSE)
    gSrcInfo[eWindow].u16IPMFetch = (gSrcInfo[eWindow].u16IPMFetch + (OFFSET_PIXEL_ALIGNMENT - 1)) & ~(OFFSET_PIXEL_ALIGNMENT - 1);
#endif
    XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC," %d \n",gSrcInfo[eWindow].u16IPMFetch);

    if(eWindow == MAIN_WINDOW || eWindow == SC1_MAIN_WINDOW || eWindow == SC2_MAIN_WINDOW)
    {
        MS_U16 u16MaxLineBFF = GetMaxLineBFF(eWindow);

        if (gSrcInfo[eWindow].u16IPMFetch > u16MaxLineBFF)
            gSrcInfo[eWindow].u16IPMFetch = u16MaxLineBFF;
    }
    else
    {
        MS_U16 u16SubMaxLineBFF = GetMaxLineBFF(eWindow);

#if (HW_DESIGN_3D_VER < 2)
        if(MDrv_SC_3D_IsIPMFetchNoLimitCase(eWindow))
        {
            //no any limits
        }
        else if((E_XC_3D_OUTPUT_TOP_BOTTOM == MApi_XC_Get_3D_Output_Mode())
                || (E_XC_3D_OUTPUT_SIDE_BY_SIDE_HALF == MApi_XC_Get_3D_Output_Mode())
               )
        {
            if(gSrcInfo[SUB_WINDOW].u16IPMFetch > MS_3D_LINE_BFF_MAX)
            {
                gSrcInfo[SUB_WINDOW].u16IPMFetch = MS_3D_LINE_BFF_MAX;
            }
        }
        else if (gSrcInfo[SUB_WINDOW].u16IPMFetch > SUB_MST_LINE_BFF_MAX)
#else
        if((E_XC_3D_PANEL_NONE == MApi_XC_Get_3D_Panel_Type())
          &&(gSrcInfo[eWindow].u16IPMFetch > u16SubMaxLineBFF))
#endif
        {
            //Normal PIP case
            gSrcInfo[eWindow].u16IPMFetch = u16SubMaxLineBFF;
        }
    }

    //printf("==>NineLattice = %x, bMirror = %x\r\n", pSrcInfo->bDisplayNineLattice,g_XC_InitData.bMirror[eWindow]);

    if(pSrcInfo->bDisplayNineLattice)
    {
        u16DNROffset = (g_XC_InitData.stPanelInfo.u16Width + OFFSET_PIXEL_ALIGNMENT - 1) & ~(OFFSET_PIXEL_ALIGNMENT-1);  //TODO: T3 should be 32 alignment
    }
    else
    {
        u16DNROffset = (gSrcInfo[eWindow].u16IPMFetch + OFFSET_PIXEL_ALIGNMENT - 1) & ~(OFFSET_PIXEL_ALIGNMENT-1);  //TODO: T3 should be 32 alignment
    }

    XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"u16DNROffset=%u\r\n",u16DNROffset);
    XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC," %d \n",gSrcInfo[eWindow].u16IPMFetch);
#if (HW_IPM_FETCH_ALIGNMENT==FALSE)
    if( g_XC_InitData.bMirror[eWindow] )
    {
        MS_BOOL bEnable;

        u16AlignWidth = u16DNROffset - pSrcInfo->u16H_SizeAfterPreScaling;
        XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"align width = %d\r\n", u16AlignWidth);

        bEnable = ( u16AlignWidth ) ? TRUE : FALSE;
        //these chips: they need sw to decide prealign or not.
        //here set to TRUE, then hw will prealign u16AlignWidth
        //if here is FALSE, hw won't do prealign
        Hal_SC_set_pre_align_pixel(bEnable, u16AlignWidth, eWindow);
    }
#else
    if( g_XC_InitData.bMirror[eWindow] )
    {
        //M10, J2, A2 and later chip, HW will auto insert pixel to alignment u16IPMFetch
        //below u16AlignWidth is used for CBCR swap control
        u16AlignWidth = (MS_U16)(u16DNROffset - gSrcInfo[eWindow].u16IPMFetch);
        //these chips: when sw set it to TRUE, then hw will do the prealign.
        //because hw can calculate by itself, even for the alignwidth==0 case,
        //so sw don't need to worry about it, just set it to TRUE.
        Hal_SC_set_pre_align_pixel(TRUE, u16AlignWidth, eWindow);//M10/J2: insert alignment pixel at start
    }
#endif
    else
    {
        u16AlignWidth = 0;
        Hal_SC_set_pre_align_pixel(FALSE, u16AlignWidth, eWindow);//M10/J2: insert alignment pixel at end
    }

    XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"align width = %d\r\n", u16AlignWidth);
    // alignwidth must be even, other color will be wrong and need to
    // use cb cr swap to correct it.
    if((u16AlignWidth & 0x01) && (HAL_SC_get_memory_bit_fmt(eWindow) == 0))
    {
        Hal_SC_set_422_cbcr_swap(ENABLE, eWindow);
    }
    else
    {
        Hal_SC_set_422_cbcr_swap(DISABLE, eWindow);
    }

    if(pSrcInfo->bDisplayNineLattice)
    {
        pSrcInfo->u16V_Length = g_XC_InitData.stPanelInfo.u16Height;
    }

    u32Framesize = (MS_U32)pSrcInfo->u16V_SizeAfterPreScaling * (MS_U32)u16DNROffset * (MS_U32)u8BytesPer2Pixel/2;
    XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"=>Framesize(%d)= VSizeAfterPreSD(%d) * DNROffset(%d) * BytesPer2Pixel(%d)/2\r\n",(int)u32Framesize,pSrcInfo->u16V_SizeAfterPreScaling,u16DNROffset,u8BytesPer2Pixel);

    if(u32Framesize * 2 > _u32DNRBufSize[eWindow])
    {
        //this case should always not be happened
        printf("Attention! Scaler DNR Buffer is not enough for this timing, eWindow=%u!\n", eWindow);
    }

    // enable 4 frame mode if input is progressive & frame buffer is enough
    // , regardless the input source and timing because 4 frame mode is better in avoid tearing
    //
    if(pSrcInfo->bDisplayNineLattice)
    {
#if (_LINEAR_ADDRESS_MODE_SUPPORTED)
        gSrcInfo[eWindow].bLinearMode = TRUE;
        Hal_SC_set_linearmem_mode(ENABLE,MAIN_WINDOW);
#endif
    }
    else
    {
        /*Update Frame buffer usage status and memory linear mode*/
        _Mdrv_SC_CalcFBNum(&gSrcInfo[eWindow].bLinearMode, &gSrcInfo[eWindow].Status2.eFrameStoreNumber, bInterlace, u32Framesize, _u32DNRBufSize[eWindow], eWindow);

        XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"gSrcInfo[eWindow].bLinearMode %d \n",gSrcInfo[eWindow].bLinearMode);
        XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"FrameMode %d \n",gSrcInfo[eWindow].Status2.eFrameStoreNumber);

        // Set memory frame buffer setting.
        HAL_SC_Set_FB_Num(eWindow, gSrcInfo[eWindow].Status2.eFrameStoreNumber,bInterlace);


#if (_LINEAR_ADDRESS_MODE_SUPPORTED)
        // Set linear mode
        if (gSrcInfo[eWindow].bLinearMode)
            Hal_SC_set_linearmem_mode(ENABLE,eWindow);
        else
            Hal_SC_set_linearmem_mode(DISABLE,eWindow);
#endif
        // if support _FIELD_PACKING_MODE_SUPPORTED, it will control by QMAP(J2/A5/A6/A3)
        #if(!_FIELD_PACKING_MODE_SUPPORTED)
        // avoid tearing problem when we can not do frame lock
        if(pSrcInfo->bRWBankAuto)
        {
            // Set read/write bank as larger than 0x01 if frame count >= 3
            if (( gSrcInfo[eWindow].Status2.eFrameStoreNumber >= IMAGE_STORE_3_FRAMES)&&
                 gSrcInfo[eWindow].Status2.eFrameStoreNumber <= IMAGE_STORE_10_FRAMES)
            {
                Hal_SC_set_wr_bank_mapping(0x01, eWindow);
            }
            else // Interlace
            {
                //For 25_4R_MC mode only
                if((!MDrv_XC_Get_OPWriteOffEnable(eWindow)) && (Hal_SC_GetFrameStoreMode(eWindow) == IMAGE_STORE_4_FIELDS))
                {
                    Hal_SC_set_wr_bank_mapping(0x02, eWindow);
                }
                else
                {
                    Hal_SC_set_wr_bank_mapping(0x00, eWindow);
                }
            }
        }
        #endif
    }

    //u32Offset = (MS_U32)pSrcInfo->u16V_Length * (MS_U32)u16DNROffset * (MS_U32)u8BytesPer2Pixel/2;
    //fix u32DNRBase1 at SCALER_DNR_BUF_LEN/2 for not using blue-screen at channel change in ATV

    if( gSrcInfo[eWindow].Status2.eFrameStoreNumber == IMAGE_STORE_3_FRAMES )
        u32Offset =  _u32DNRBufSize[eWindow]/3;
    else
        u32Offset =  _u32DNRBufSize[eWindow]/2;

    XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"u32Offset :%lu, DNRBufSize=%lu\r\n", u32Offset, _u32DNRBufSize[eWindow]);
    //    SC_FNLDBG(printf("=>u32Offset[%x] = _u32DNRBufSize[%x]/%d\r\n",(int)u32Offset,(int)_u32DNRBufSize, u8BestFBNum));
    u32DNRBase0 = MS_IPM_BASE0(eWindow);
    u32DNRBase1 = MS_IPM_BASE0(eWindow) + (( u32Offset/BYTE_PER_WORD + 0x01) & ~0x01L);
    XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"=>MAIN DNRBase1[%x]= MS_IPM_BASE0[%u]=(%x) + ((u32Offset[%x]/8 + 0x01) & ~0x01L)\n",(int)u32DNRBase1, eWindow,(int)MS_IPM_BASE0(eWindow),(int)u32Offset);

    MDrv_SC_3D_Adjust_DNRBase(&u32DNRBase0,
                              &u32DNRBase1,
                              eWindow);

    if( gSrcInfo[eWindow].Status2.eFrameStoreNumber == IMAGE_STORE_3_FRAMES  )
        u32DNRBase2 = u32DNRBase1 + (( u32Offset/BYTE_PER_WORD + 0x01) & ~0x01L);
    else
        u32DNRBase2 = u32DNRBase0;

    XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"=>DNRBase2[%x])\n", (int)u32DNRBase2);

    if(pSrcInfo->bDisplayNineLattice)
    {
        u32OPMBase1 = u32OPMBase0 = u32OPMBase2 = u32DNRBase0;
    }
    else
    {
        u32OPMBase0 = u32DNRBase0;
        u32OPMBase1 = u32DNRBase1;
        u32OPMBase2 = u32DNRBase2;

        MDrv_SC_3D_Adjust_OPMBase(&u32OPMBase0,
                                  &u32OPMBase1,
                                  &u32OPMBase2,
                                  u32Offset,
                                  eWindow);
    }

    XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"=>u32OPMBase0[%x]= MS_IPM_BASE0[%u][%x] => ",(int)u32OPMBase0, eWindow, (int)MS_IPM_BASE0(eWindow));
    XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"u32OPMBase1[%x]= u32IPMBase1[%x] \n",(int)u32OPMBase1,(int)u32DNRBase1);
    XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"u32OPMBase2[%x] \n",(int)u32OPMBase2);

    // Cal DNR V write limit
    if(pSrcInfo->bDisplayNineLattice)
    {
        u16V_Writelimit = g_XC_InitData.stPanelInfo.u16Height;
    }
    else
    {
        u16V_Writelimit = pSrcInfo->stCapWin.height;
    }

    XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"=> u16V_Writelimit[%d]= u16V_CapSize=%d \n",u16V_Writelimit,pSrcInfo->stCapWin.height);
    if( bInterlace )
        u16V_Writelimit = (u16V_Writelimit+1) / 2;
    XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"=> i=%d, u16V_Writelimit[%d]= (u16V_Writelimit+1) / 2\r\n",bInterlace,u16V_Writelimit);
    // Cal DNR write limit
  #if 1//( CHIP_FAMILY_TYPE == CHIP_FAMILY_S4LE )

    XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"preview =>WritelimitBase[%u][%x] = (MS_IPM_BASE0[%u]=%x) + (((u32Offset[%x] + 0xF) & ~0xFL) * 3 / BYTE_PER_WORD)\n",eWindow,(int)u32WritelimitBase, eWindow, (int)MS_IPM_BASE0(eWindow) ,(int)u32Offset);

    if(gSrcInfo[eWindow].Status2.eFrameStoreNumber == IMAGE_STORE_3_FRAMES)
        u32WritelimitBase = (MS_IPM_BASE0(eWindow)) + (((u32Offset + 0xF) & ~0xFL) * 3 / BYTE_PER_WORD);
    else
        u32WritelimitBase = (MS_IPM_BASE0(eWindow)) + (((u32Offset + 0xF) & ~0xFL) * 2 / BYTE_PER_WORD);

     XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"=>WritelimitBase[%x] = (MS_IPM_BASE0[%u]=%x) + (((u32Offset[%x] + 0xF) & ~0xFL) * 2 / BYTE_PER_WORD)\n",(int)u32WritelimitBase,eWindow,(int)MS_IPM_BASE0(eWindow),(int)u32Offset);

	 #ifdef MULTI_SCALER_SUPPORTED
     u32WritelimitBase = (eWindow == MAIN_WINDOW || eWindow == SC1_MAIN_WINDOW || eWindow == SC2_MAIN_WINDOW) ?
                         (u32WritelimitBase - 1) | (MS_U32)F2_WRITE_LIMIT_EN :
                         (u32WritelimitBase - 1) | (MS_U32)F1_WRITE_LIMIT_EN;
	 #else
     u32WritelimitBase = (eWindow == MAIN_WINDOW) ? (u32WritelimitBase - 1) | F2_WRITE_LIMIT_EN : (u32WritelimitBase - 1) | F1_WRITE_LIMIT_EN;
	 #endif

     XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"=> u32WritelimitBase[%x] = (u32WritelimitBase - 1) | 0x2000000\r\n",(int)u32WritelimitBase);
  #else
    u32WritelimitBase = MS_DNR_F2_BASE0 + ((u32Offset/8 + 0x01) & ~0x01L) * 2;
    u32WritelimitBase = (u32WritelimitBase - 1);
  #endif

    if ( g_XC_InitData.bMirror[eWindow] )
    {
        if(!pSrcInfo->bDisplayNineLattice)
        {
            MS_U16 u16V_SizeAfterPreScaling = pSrcInfo->u16V_SizeAfterPreScaling;
            MS_U16 u16DNROffsetTemp = u16DNROffset;
            MS_U16 u16Ratio = 1;
            MS_U32 u32OneLineOffset = 0;
            if(HAL_SC_Get_DynamicScaling_Status())
            {
                u16V_SizeAfterPreScaling = gSrcInfo[eWindow].Status2.u16PreVCusScalingSrc;
            }
            MDrv_SC_3D_Adjust_MirrorDNROffset(pSrcInfo, &u16V_SizeAfterPreScaling, &u16DNROffsetTemp, &u16Ratio, eWindow);

            u32OneLineOffset = MDrv_SC_Get_FrameStoreNum_Factor(gSrcInfo[eWindow].Status2.eFrameStoreNumber, gSrcInfo[eWindow].bLinearMode)
                               * (MS_U32)u16Ratio * (MS_U32)u16DNROffsetTemp * (MS_U32)u8BytesPer2Pixel/2/BYTE_PER_WORD;

            //base_offset = (frame_line_cnt -2) * line_offset * (N-bits/pix)/64-bits
            if( bInterlace )
            {
                MS_U16 u16Offset;
                u16Offset = u16DNROffsetTemp * MDrv_SC_Get_FrameStoreNum_Factor(gSrcInfo[eWindow].Status2.eFrameStoreNumber, gSrcInfo[eWindow].bLinearMode) / 2;

                u32BaseOffset = (MS_U32)(u16V_SizeAfterPreScaling-2) * (MS_U32)u16Ratio * (MS_U32)u16Offset * (MS_U32)u8BytesPer2Pixel/2/BYTE_PER_WORD;
            }
            else
            {
                u8FrameNum = MDrv_SC_Get_FrameStoreNum_Factor(gSrcInfo[eWindow].Status2.eFrameStoreNumber, gSrcInfo[eWindow].bLinearMode);

                //Progressived mode, non-linear address
                //base_offset = (frame_line_cnt -2) * line_offset * (N-bits/pix)/64-bits
                u32BaseOffset = (MS_U32)((MS_U32)u16Ratio * u8FrameNum*(u16V_SizeAfterPreScaling-1)) * (MS_U32)u16DNROffsetTemp * (MS_U32)u8BytesPer2Pixel/2/BYTE_PER_WORD;
            }

            MDrv_SC_set_DNRBaseOffset(eWindow,u32BaseOffset);

            u32DNRBase0 += u32BaseOffset;

            //to avoid set dnr base1 out of scaler dnr memory when 2 frame mode
            if((u32DNRBase1 + u32BaseOffset) <= (MS_IPM_BASE0(eWindow) + _u32DNRBufSize[eWindow]/BYTE_PER_WORD - u32OneLineOffset))
            {
                u32DNRBase1 += u32BaseOffset;
            }

            if((u32DNRBase2 + u32BaseOffset) <= (MS_IPM_BASE0(eWindow) + _u32DNRBufSize[eWindow]/BYTE_PER_WORD - u32OneLineOffset))
            {
                u32DNRBase2 += u32BaseOffset;
            }
        }

        u32WritelimitBase = MS_IPM_BASE0(eWindow) - 1;

		#ifdef MULTI_SCALER_SUPPORTED
        if(eWindow == MAIN_WINDOW || eWindow == SC1_MAIN_WINDOW || eWindow == SC2_MAIN_WINDOW)
            u32WritelimitBase= u32WritelimitBase | (MS_U32)F2_WRITE_LIMIT_EN | (MS_U32)F2_WRITE_LIMIT_MIN;
        else
            u32WritelimitBase= u32WritelimitBase | (MS_U32)F1_WRITE_LIMIT_EN | (MS_U32)F1_WRITE_LIMIT_MIN;
		#else
        if(eWindow == MAIN_WINDOW)
            u32WritelimitBase= u32WritelimitBase | F2_WRITE_LIMIT_EN | F2_WRITE_LIMIT_MIN;
        else
            u32WritelimitBase= u32WritelimitBase | F1_WRITE_LIMIT_EN | F1_WRITE_LIMIT_MIN;
		#endif
    }

    //store FrameNum
    MDrv_SC_set_storeFrameNUM(eWindow,MDrv_SC_Get_FrameStoreNum_Factor(gSrcInfo[eWindow].Status2.eFrameStoreNumber, gSrcInfo[eWindow].bLinearMode));

    /////////////////////////////////////////////////
    // Set OPM Memeory Settings                    //
    /////////////////////////////////////////////////

    // Cal OPM Base address

    // compute address offset for cropping
    // cropping shift h unit: 16 pixel
    // v unit: 2 line(for interlace), 1 line(for progressive)
    XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"Crop info: (x,y)=(%d,%d),(width,height)=(%d,%d), interlaced = %d\r\n",
        gSrcInfo[eWindow].ScaledCropWin.x, gSrcInfo[eWindow].ScaledCropWin.y,
        gSrcInfo[eWindow].ScaledCropWin.width, gSrcInfo[eWindow].ScaledCropWin.height,
        bInterlace);

    if(pSrcInfo->bDisplayNineLattice)
    {
        if(g_XC_InitData.bMirror[eWindow])
        {
            if(gSrcInfo[eWindow].Status2.eFrameStoreNumber == IMAGE_STORE_8_FIELDS)
            {
                u32OPMPixelOffset = (MS_U32)(g_XC_InitData.stPanelInfo.u16Height - (gSrcInfo[eWindow].stDispWin.y - g_XC_InitData.stPanelInfo.u16VStart))*u16DNROffset;
            }
            else if( bInterlace )
            {
                u32OPMPixelOffset = (MS_U32)((g_XC_InitData.stPanelInfo.u16Height - (gSrcInfo[eWindow].stDispWin.y - g_XC_InitData.stPanelInfo.u16VStart))/2)*u16DNROffset;
            }
            else
            {
                u32OPMPixelOffset = (MS_U32)(g_XC_InitData.stPanelInfo.u16Height - (gSrcInfo[eWindow].stDispWin.y - g_XC_InitData.stPanelInfo.u16VStart))*u16DNROffset;
            }
            u32OPMPixelOffset += (MS_U32)(g_XC_InitData.stPanelInfo.u16Width - (gSrcInfo[eWindow].stDispWin.x - g_XC_InitData.stPanelInfo.u16HStart + gSrcInfo[eWindow].stDispWin.width));
        }
        else
        {
            if(gSrcInfo[eWindow].Status2.eFrameStoreNumber == IMAGE_STORE_8_FIELDS)
            {
                u32OPMPixelOffset = (MS_U32)((gSrcInfo[eWindow].stDispWin.y - g_XC_InitData.stPanelInfo.u16VStart))*u16DNROffset;
            }
            else if( bInterlace )
            {
                u32OPMPixelOffset = (MS_U32)((gSrcInfo[eWindow].stDispWin.y - g_XC_InitData.stPanelInfo.u16VStart)/2)*u16DNROffset;
            }
            else
            {
                u32OPMPixelOffset = (MS_U32)((gSrcInfo[eWindow].stDispWin.y - g_XC_InitData.stPanelInfo.u16VStart))*u16DNROffset;
            }
            u32OPMPixelOffset += (MS_U32)(gSrcInfo[eWindow].stDispWin.x - g_XC_InitData.stPanelInfo.u16HStart);
        }
    }
    else
    {

        MS_U16 u16DNROffsetTemp = u16DNROffset;
        MDrv_SC_3D_Adjust_DNROffset(&u16DNROffsetTemp,&u16DNROffset, eWindow);

        if( bInterlace )
        {
            u32OPMPixelOffset = (MS_U32)(gSrcInfo[eWindow].ScaledCropWin.y/2) * u16DNROffsetTemp;
        }
        else
        {
            u32OPMPixelOffset = (MS_U32)((gSrcInfo[eWindow].ScaledCropWin.y) * u16DNROffsetTemp);
        }
        u32OPMPixelOffset += (MS_U32)(gSrcInfo[eWindow].ScaledCropWin.x);
    }

    XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"Crop total offset pixel counts %ld\r\n",u32OPMPixelOffset);
    XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"Crop info: (x,y)=(%d,%d),(width,height)=(%d,%d), interlaced = %d\r\n",
        gSrcInfo[eWindow].ScaledCropWin.x, gSrcInfo[eWindow].ScaledCropWin.y,
        gSrcInfo[eWindow].ScaledCropWin.width, gSrcInfo[eWindow].ScaledCropWin.height,
        bInterlace);

    // calculate LB offset
    u16LBOffset = (MS_U16)(u32OPMPixelOffset % OFFSET_PIXEL_ALIGNMENT);            // these pixels rely on line buffer offset
    gSrcInfo[eWindow].Status2.u16LBOffset = u16LBOffset;
    u32OPMPixelOffset -= u16LBOffset;                                    // these pixels rely on base offset

    XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"LB offset %d, Pixel offset %d Alighment: %d bytes \n", u16LBOffset, (int)u32OPMPixelOffset,OFFSET_PIXEL_ALIGNMENT);

    // calcuate OPM offset
    XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"eFrameStoreNumber %d\r\n", (gSrcInfo[eWindow].Status2.eFrameStoreNumber));
    u32OPMPixelOffset *= MDrv_SC_Get_FrameStoreNum_Factor(gSrcInfo[eWindow].Status2.eFrameStoreNumber, gSrcInfo[eWindow].bLinearMode);

    XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"bLinearMode = %s\r\n",gSrcInfo[eWindow].bLinearMode ?("TRUE"):("FALSE"));
    XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"Crop total offset pixel counts for field/frame %ld (eFrameStoreNumber: %d)\n", u32OPMPixelOffset, gSrcInfo[eWindow].Status2.eFrameStoreNumber);

    if(u32OPMPixelOffset != 0)
    {
        MS_U8 u8BytesPer2PixelTemp = u8BytesPer2Pixel;
        MDrv_SC_3D_Adjust_BytesPer2Pixel(&u8BytesPer2PixelTemp,eWindow);

        u32OPMCropOffset = u32OPMPixelOffset * u8BytesPer2PixelTemp / 2 / BYTE_PER_WORD;
    }

    XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"f.LineBuf offset %d, OPMOffset 0x%lx\r\n", u16LBOffset, u32OPMCropOffset);

    //KT: T3 PIP HW issue, When SUB ON, the HW operation will decrease "u16LBOffset" 2 times.

    CropWinWidthTemp = gSrcInfo[eWindow].ScaledCropWin.width;

    MDrv_SC_3D_Adjust_CropWinWidth(&CropWinWidthTemp,eWindow);

    u16OPMFetch = CropWinWidthTemp + u16LBOffset; // using LBOffset still need to add some points to avoid generating garbage


    if(pSrcInfo->bDisplayNineLattice) // for display ninelattice no line buffer offset
    {
        u16LBOffset = u16DispOffset=0;
        u16OPMFetch = (g_XC_InitData.stPanelInfo.u16Width + 1) & ~0x1;
    }
    else
    {
        //  IPM offset / IPM fetch /OPM offset: All project --> 32pix align
        //  OPM fetch:
        //            Janus /T7 --> 32pix align
        //            Others ( T8/T9/T12/T13/m10/J2 ) --> 2pix align
        MS_U16 u16HShift = 0, u16tempOPM;
        if((u16LBOffset & 0x1) != 0)
        {
            //HW issue for OPM fetch&422to444: when lboffset not even, OPM should add 2 to avoid right line garbage
            u16tempOPM = (( u16OPMFetch + 1 ) & ~1) + 2;
            if(u16tempOPM > pSrcInfo->u16H_SizeAfterPreScaling - (gSrcInfo[eWindow].ScaledCropWin.x - u16LBOffset))
            {
                u16LBOffset--;//Overflow, set lb offset even and alignment OPM
                u16OPMFetch = u16OPMFetch & ~1; //Make even directly, since lb offset has minus 1
            }
            else
            {
                u16OPMFetch = u16tempOPM;
            }
        }
        else if((u16OPMFetch != gSrcInfo[eWindow].u16IPMFetch)  &&  (E_XC_3D_OUTPUT_MODE_NONE == MApi_XC_Get_3D_Output_Mode()))
        {
            //UCDi need grab two more pixels, not 3D case.
            u16tempOPM = (( u16OPMFetch + 1 ) & ~1) + 2;
            if(u16tempOPM > pSrcInfo->u16H_SizeAfterPreScaling - (gSrcInfo[eWindow].ScaledCropWin.x - u16LBOffset))
            {
                u16OPMFetch = u16OPMFetch & ~1;
            }
            else
            {
                u16OPMFetch = u16tempOPM;
            }
        }
        else
        {
            u16OPMFetch = ( u16OPMFetch + 1 ) & ~1;  //  pixels  alignment
        }

        if(MDrv_SC_3D_Is2Dto3DCase(MApi_XC_Get_3D_Input_Mode(eWindow), MApi_XC_Get_3D_Output_Mode()))
        {
            u16HShift = MApi_XC_Get_3D_HShift();
            if ((u16OPMFetch + u16HShift) > gSrcInfo[eWindow].u16IPMFetch)
            {
                if(gSrcInfo[eWindow].u16IPMFetch > u16HShift)
                {
                    //opmfetch need align although ipmfetch may not need align
                    u16OPMFetch = ( gSrcInfo[eWindow].u16IPMFetch  - u16HShift + 1 ) & ~1;
                }
                else
                {
                    u16OPMFetch = 0;
                }
            }
        }
        else
        {
            if(u16OPMFetch > pSrcInfo->u16H_SizeAfterPreScaling  - (gSrcInfo[eWindow].ScaledCropWin.x - u16LBOffset))
                XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"!!!Alert OPMFetch(%u) Overflowed, check crop setting!!!\n", u16OPMFetch);
        }
    }
    XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"f1. u16OPMFetch after alignment %d, lboffset %d \n", u16OPMFetch, u16LBOffset);
    //printf("1.SC Crop width=0x%x, LBOffset=0x%x\r\n", gSrcInfo[eWindow].ScaledCropWin.width, u16LBOffset);
    u16DispOffset = 0;

    //printf("cropy=%u,cropx=%u,dnroffset=%u,bp2p=%bu\r\n", _stScaledCropWin.y, _stScaledCropWin.x, u16DNROffset, u8BytesPer2Pixel);
    //printf("2.OPM Crop Offset=0x%lx, LBOffset=0x%x\r\n", u32OPMCropOffset, u16LBOffset);

    // adjsut OPMBase address for cropping
    if(pSrcInfo ->bDisplayNineLattice)
    {
    // for linear mode set all base address the same
        u32DNRBase0 += u32OPMCropOffset;
        //u32DNRBase1 += u32OPMCropOffset;
        //u32OPMBase2 += u32OPMCropOffset;
        u32DNRBase2 = u32DNRBase1 = u32DNRBase0;
        //printf("--u32DNRBase0 %lx\r\n",u32DNRBase0);
    }
    else
    {
        XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"h.=> u32OPMBase0,1=%x,%x -> +u32OPMCropOffset[%x] =",(int)u32OPMBase0,(int)u32OPMBase1,(int)u32OPMCropOffset);
        u32OPMBase0 += u32OPMCropOffset;
        XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"0:%x \n",(int)u32OPMBase0);
        u32OPMBase1 += u32OPMCropOffset;
        XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"1:%x \n",(int)u32OPMBase1);
        u32OPMBase2 += u32OPMCropOffset;
        XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"2:%x \n",(int)u32OPMBase2);
    }

#if 0
    if(!pSrcInfo ->bDisplayNineLattice)
    {
        pSrcInfo->u16V_Length = pSrcInfo->u16V_Length - (gSrcInfo[eWindow].ScaledCropWin.y);
    }
#endif

    //-----------------------------------------------------------------
    stDBreg.u16VLen = pSrcInfo->u16V_Length;

#ifdef MULTI_SCALER_SUPPORTED
	if(eWindow == MAIN_WINDOW || eWindow == SC1_MAIN_WINDOW || eWindow == SC2_MAIN_WINDOW)
#else
    if(eWindow == MAIN_WINDOW)
#endif
    stDBreg.u16VWritelimit = u16V_Writelimit | F2_V_WRITE_LIMIT_EN;
    else
    stDBreg.u16VWritelimit = u16V_Writelimit | F1_V_WRITE_LIMIT_EN;

    stDBreg.u32WritelimitBase = u32WritelimitBase;
    stDBreg.u16DNROffset = u16DNROffset;
    stDBreg.u16DNRFetch  = gSrcInfo[eWindow].u16IPMFetch;
    stDBreg.u16LBOffset  = u16LBOffset;
    stDBreg.u16DispOffset  = u16DispOffset;

    stDBreg.u32DNRBase0  = u32DNRBase0;
    stDBreg.u32DNRBase1  = u32DNRBase1;
    stDBreg.u32DNRBase2  = u32DNRBase2;
    stDBreg.u32OPMBase0  = u32OPMBase0;
    stDBreg.u32OPMBase1  = u32OPMBase1;
    stDBreg.u32OPMBase2  = u32OPMBase2;
    stDBreg.u16OPMFetch  = u16OPMFetch;
    stDBreg.u16OPMOffset = u16DNROffset;
    stDBreg.u16AlignWidth = u16AlignWidth;

    pSrcInfo->u32IPMBase0 = u32DNRBase0 * BYTE_PER_WORD;
    pSrcInfo->u32IPMBase1 = u32DNRBase1 * BYTE_PER_WORD;
    pSrcInfo->u32IPMBase2 = u32DNRBase2 * BYTE_PER_WORD;
    pSrcInfo->u16IPMOffset = u16DNROffset;

    MDrv_SC_3D_Adjust_FetchOffset(pSrcInfo, eWindow);

    //-----------------------------------------------------------------
    XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"Final DNR Offset/Fetch=0x%x, 0x%x, OPM Offset/Fetch=0x%x, 0x%x, V_Length=0x%x\r\n",
                 stDBreg.u16DNROffset, stDBreg.u16DNRFetch, stDBreg.u16OPMOffset, stDBreg.u16OPMFetch, stDBreg.u16VLen);
    XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"Final u32DNRBase0/1/2=0x%lx, 0x%lx, 0x%lx, u32OPMBase0/1/2=0x%lx, 0x%lx, 0x%lx\r\n",
                 stDBreg.u32DNRBase0, stDBreg.u32DNRBase1, stDBreg.u32DNRBase2, stDBreg.u32OPMBase0, stDBreg.u32OPMBase1, stDBreg.u32OPMBase2);
    XC_LOG_TRACE(XC_DGBLEVEL_CROPCALC,"---------------------------------------------------\n");
}

void MDrv_SC_set_shift_line(MS_BOOL bFBL, MS_DEINTERLACE_MODE eDeInterlaceMode, SCALER_WIN eWindow)
{
    MS_U8 u8Val;

    if(!bFBL)
    {  // FB
        u8Val = 0x0;
    }
    else
    {   // FBL
        if((MS_DEINT_2DDI_BOB == eDeInterlaceMode) || (MS_DEINT_2DDI_AVG == eDeInterlaceMode))
            u8Val = 0x0;
        else
            u8Val = 0x2;
    }

    Hal_SC_set_shiftline( u8Val, eWindow );
}

void MDrv_SC_set_mirror(MS_BOOL bEnable, SCALER_WIN eWindow)
{
    MS_U32 u32WritelimitBase;

    if ( bEnable )
    {
        // Set Limit at top of frame buffer
        //limit_min :(0: Maximum   1: Minimum)
#ifdef MULTI_SCALER_SUPPORTED
        if(eWindow == MAIN_WINDOW || eWindow == SC1_MAIN_WINDOW || eWindow == SC2_MAIN_WINDOW)
            u32WritelimitBase = (MS_IPM_BASE0(eWindow) - 1) | (MS_U32)F2_WRITE_LIMIT_EN | (MS_U32)F2_WRITE_LIMIT_MIN;
        else
            u32WritelimitBase = (MS_IPM_BASE0(eWindow) - 1) | (MS_U32)F1_WRITE_LIMIT_EN | (MS_U32)F1_WRITE_LIMIT_MIN;
#else
        if(eWindow == MAIN_WINDOW)
            u32WritelimitBase = (MS_IPM_BASE0(eWindow) - 1) | F2_WRITE_LIMIT_EN | F2_WRITE_LIMIT_MIN;
        else
            u32WritelimitBase = (MS_IPM_BASE0(eWindow) - 1) | F1_WRITE_LIMIT_EN | F1_WRITE_LIMIT_MIN;
#endif
    }
    else
    {
        // Set limit at bottom of frame buffer

#ifdef MULTI_SCALER_SUPPORTED
        if(eWindow == MAIN_WINDOW || eWindow == SC1_MAIN_WINDOW || eWindow == SC2_MAIN_WINDOW)
            u32WritelimitBase = ((_u32DNRBaseAddr0[eWindow] + _u32DNRBufSize[eWindow]) / BYTE_PER_WORD - 1) | (MS_U32)F2_WRITE_LIMIT_EN;
        else
            u32WritelimitBase = ((_u32DNRBaseAddr0[eWindow] + _u32DNRBufSize[eWindow]) / BYTE_PER_WORD - 1) | (MS_U32)F1_WRITE_LIMIT_EN;
#else
        if(eWindow == MAIN_WINDOW)
            u32WritelimitBase = ((_u32DNRBaseAddr0[eWindow] + _u32DNRBufSize[eWindow]) / BYTE_PER_WORD - 1) | F2_WRITE_LIMIT_EN;
        else
            u32WritelimitBase = ((_u32DNRBaseAddr0[eWindow] + _u32DNRBufSize[eWindow]) / BYTE_PER_WORD - 1) | F1_WRITE_LIMIT_EN;
#endif

    }
    Hal_SC_set_write_limit(u32WritelimitBase, eWindow);

    Hal_SC_set_mirror( bEnable, eWindow );
}

void _MDrv_SC_Set_Mirror(DrvMirrorMode_t eMirrorMode, SCALER_WIN eWindow)
{
    MS_U32 u32NoSigReg, u32MirrorReg;


    u32NoSigReg = eWindow == MAIN_WINDOW     ? REG_SC_BK12_07_L :
                  eWindow == SUB_WINDOW      ? REG_SC_BK12_47_L :
                  eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK12_07_L :
                  eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK12_07_L :
                  eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK12_47_L :
                                               REG_SC_DUMMY;

    u32MirrorReg = eWindow == MAIN_WINDOW     ? REG_SC_BK12_03_L :
                   eWindow == SUB_WINDOW      ? REG_SC_BK12_43_L :
                   eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK12_03_L :
                   eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK12_03_L :
                   eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK12_43_L :
                                                REG_SC_DUMMY;


    SC_W2BYTEMSK(u32NoSigReg, BIT(0), BIT(0));


    if (eMirrorMode == DRV_MIRROR_NORMAL)
    {
        SC_W2BYTEMSK(u32MirrorReg, 0, BIT(12));

        SC_W2BYTEMSK(u32MirrorReg, 0, BIT(13));
    }
    else if (eMirrorMode == DRV_MIRROR_H_ONLY)
    {
        SC_W2BYTEMSK(u32MirrorReg, BIT(12), BIT(12));

        SC_W2BYTEMSK(u32MirrorReg, 0, BIT(13));
    }
    else if (eMirrorMode == DRV_MIRROR_V_ONLY)
    {
        SC_W2BYTEMSK(u32MirrorReg, 0, BIT(12));

        SC_W2BYTEMSK(u32MirrorReg, BIT(13), BIT(13));
    }
    else// DRV_MIRROR_HV
    {
        SC_W2BYTEMSK(u32MirrorReg, (BIT(12)|BIT(13)), (BIT(12)|BIT(13)));
    }

    SC_W2BYTEMSK(u32NoSigReg, 0, BIT(0));
}


void MDrv_SC_set_mirror2(DrvMirrorMode_t eMirrorMode, SCALER_WIN eWindow)
{
    MS_U32 u32WritelimitBase;

    if ( (eMirrorMode == DRV_MIRROR_HV) || (eMirrorMode == DRV_MIRROR_V_ONLY) )
    {
        // Set Limit at top of frame buffer
        //limit_min :(0: Maximum   1: Minimum)
#ifdef MULTI_SCALER_SUPPORTED
        if(eWindow == MAIN_WINDOW || eWindow == SC1_MAIN_WINDOW || eWindow == SC2_MAIN_WINDOW)
            u32WritelimitBase = (MS_IPM_BASE0(eWindow) - 1) | (MS_U32)F2_WRITE_LIMIT_EN | (MS_U32)F2_WRITE_LIMIT_MIN;
        else
            u32WritelimitBase = (MS_IPM_BASE0(eWindow) - 1) | (MS_U32)F1_WRITE_LIMIT_EN | (MS_U32)F1_WRITE_LIMIT_MIN;
#else
        if(eWindow == MAIN_WINDOW)
            u32WritelimitBase = (MS_IPM_BASE0(eWindow) - 1) | F2_WRITE_LIMIT_EN | F2_WRITE_LIMIT_MIN;
        else
            u32WritelimitBase = (MS_IPM_BASE0(eWindow) - 1) | F1_WRITE_LIMIT_EN | F1_WRITE_LIMIT_MIN;
#endif
    }
    else // NORMAL or H_MIRROR case
    {
#ifdef MULTI_SCALER_SUPPORTED
		if(eWindow == MAIN_WINDOW || eWindow == SC1_MAIN_WINDOW || eWindow == SC2_MAIN_WINDOW)
			u32WritelimitBase = ((_u32DNRBaseAddr0[eWindow] + _u32DNRBufSize[eWindow]) / BYTE_PER_WORD - 1) | (MS_U32)F2_WRITE_LIMIT_EN;
		else
			u32WritelimitBase = ((_u32DNRBaseAddr0[eWindow] + _u32DNRBufSize[eWindow]) / BYTE_PER_WORD - 1) | (MS_U32)F1_WRITE_LIMIT_EN;
#else
        // Set limit at bottom of frame buffer
        if(eWindow == MAIN_WINDOW)
            u32WritelimitBase = ((_u32DNRBaseAddr0[eWindow] + _u32DNRBufSize[eWindow]) / BYTE_PER_WORD - 1) | F2_WRITE_LIMIT_EN;
        else
            u32WritelimitBase = ((_u32DNRBaseAddr0[eWindow] + _u32DNRBufSize[eWindow]) / BYTE_PER_WORD - 1) | F1_WRITE_LIMIT_EN;
#endif
    }

    Hal_SC_set_write_limit(u32WritelimitBase, eWindow);

    _MDrv_SC_Set_Mirror ( eMirrorMode, eWindow);

}

MS_BOOL MDrv_XC_Set_DynamicScaling(MS_U32 u32MemBaseAddr, MS_U8 u8MIU_Select, MS_U8 u8IdxDepth, MS_BOOL bOP_On, MS_BOOL bIPS_On, MS_BOOL bIPM_On)
{
    return HAL_SC_Set_DynamicScaling(u32MemBaseAddr, u8MIU_Select, u8IdxDepth, bOP_On, bIPS_On, bIPM_On);
}

void MDrv_XC_FilLineBuffer(MS_BOOL bEnable, SCALER_WIN eWindow)
{
    HAL_SC_FillLineBuffer(bEnable, eWindow);
}

MS_BOOL MDrv_XC_GetHSizeChangeManuallyFlag(SCALER_WIN eWindow)
{
    return _bHSizeChangedManually;
}

MS_PHYADDR MDrv_XC_GetIPMBase(MS_U8 num, SCALER_WIN eWindow)
{
    MS_PHYADDR Addr = 0;

    if (num == 0)
    {
        Addr = MS_IPM_BASE0(eWindow);
    }
    else if (num == 1)
    {
        Addr = MS_IPM_BASE1(eWindow);
    }
    return Addr;
}

MS_U32 MDrv_XC_GetDNRBufSize(SCALER_WIN eWindow)
{
    return _u32DNRBufSize[eWindow];
}

void MDrv_XC_SetDNRBufSize(MS_U32 u32DNRBufSize, SCALER_WIN eWindow)
{
    _u32DNRBufSize[eWindow] = u32DNRBufSize;
}

// calc widht or height based on existed dnr buffer and byte/pixel
MS_U32 MApi_XC_GetAvailableSize(SCALER_WIN eWindow, MS_U8 u8FBNum, MS_U32 u32InputSize)
{
    return (_u32DNRBufSize[eWindow] * 8 / u8FBNum / gSrcInfo[eWindow].u8BitPerPixel / u32InputSize);
}

void MDrv_SC_Set_pre_align_pixel(MS_BOOL bEnable, MS_U16 pixels, SCALER_WIN eWindow)
{
    Hal_SC_set_pre_align_pixel(bEnable, pixels, eWindow );
}

void MDrv_SC_Set_FB_Num(SCALER_WIN eWindow, XC_FRAME_STORE_NUMBER enFBType, MS_BOOL bInterlace)
{
    HAL_SC_Set_FB_Num(eWindow, enFBType, bInterlace);
}

void MDrv_SC_ClearScalingFactorForInternalCalib(void)
{
    Hal_XC_ClearScalingFactorForInternalCalib();
}

void MDrv_SC_Set_wr_bank_mapping(MS_U8 u8val, SCALER_WIN eWindow)
{
    Hal_SC_set_wr_bank_mapping(u8val, eWindow);
}

void MDrv_SC_Enable_PreScaling(MS_BOOL bHSDEnable, MS_BOOL bVSDEnable, SCALER_WIN eWindow)
{
    MS_U32 u32Reg05, u32Reg09;

    u32Reg05 = eWindow == MAIN_WINDOW     ? REG_SC_BK02_05_L :
               eWindow == SUB_WINDOW      ? REG_SC_BK04_05_L :
			   #ifdef MULTI_SCALER_SUPPORTED
               eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK02_05_L :
               eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK02_05_L :
               eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK04_05_L :
			   #endif
                                            REG_SC_DUMMY;

    u32Reg09 = eWindow == MAIN_WINDOW     ? REG_SC_BK02_09_L :
               eWindow == SUB_WINDOW      ? REG_SC_BK04_09_L :
			   #ifdef MULTI_SCALER_SUPPORTED
               eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK02_09_L :
               eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK02_09_L :
               eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK04_09_L :
			   #endif
                                            REG_SC_DUMMY;


    SC_W2BYTEMSK(u32Reg05, bHSDEnable<<15, BIT(15));
    SC_W2BYTEMSK(u32Reg09,  bVSDEnable<<15, BIT(15));
}

void MDrv_SC_SetFullRangeCapture(SCALER_WIN eWindow)
{
    Hal_SC_ip_set_capture_v_start(0x0C,eWindow);
    Hal_SC_ip_set_capture_h_start(0x0C,eWindow);
    Hal_SC_ip_set_capture_v_size(0x1FFF,eWindow);
    Hal_SC_ip_set_capture_h_size(0x1FFF,eWindow);
}

static void AdjustWinLineBufferSize(MS_U16 u16DispWinWidth, SCALER_WIN eWindow)
{
	if ( SUB_WINDOW==eWindow || SC2_SUB_WINDOW==eWindow)
	{
		u16MaxSizeOfLB[eWindow] = MIN( u16DispWinWidth+SUB_MAIN_LINEOFFSET_GUARD_BAND, (SCALER_LB_SIZE-1) );
	}
	else if( SC1_MAIN_WINDOW==eWindow)
	{
        u16MaxSizeOfLB[eWindow] = MST_LINE_BFF_MAX_SC1;
	}
	else
	{
		u16MaxSizeOfLB[eWindow] = MIN( u16DispWinWidth, SCALER_LINE_BUFFER_MAX );
	}
}

static MS_U16 GetSubStartOffsetLB_dynamic(SCALER_WIN eWindow, MS_BOOL bForceChange, MS_BOOL * pbTodo)
{
	MS_U16 u16LineBuffOffset = -1;

	if (!pbTodo)
		return -1;

	*pbTodo = FALSE;

	if (SUB_WINDOW==eWindow)
	{
		if ( bForceChange || 0==u16MaxSizeOfLB[MAIN_WINDOW] )
		{
			*pbTodo = TRUE;
			if ( 0!=u16MaxSizeOfLB[SUB_WINDOW])
			{
				u16LineBuffOffset = SCALER_LB_SIZE - u16MaxSizeOfLB[SUB_WINDOW];
			}
			else
			{
				u16LineBuffOffset = SCALER_LINE_BUFFER_MAX+ SUB_MAIN_LINEOFFSET_GUARD_BAND;
			}
		}
	}
	else if ( MAIN_WINDOW==eWindow)
	{
		if ( bForceChange || 0==u16MaxSizeOfLB[SUB_WINDOW])
		{
			*pbTodo = TRUE;
			if ( 0!=u16MaxSizeOfLB[MAIN_WINDOW])
			{
				u16LineBuffOffset =  u16MaxSizeOfLB[MAIN_WINDOW] + SUB_MAIN_LINEOFFSET_GUARD_BAND;
			}
			else
			{
				u16LineBuffOffset = SCALER_LINE_BUFFER_MAX+ SUB_MAIN_LINEOFFSET_GUARD_BAND;
			}
		}
	}
	else if (SC2_MAIN_WINDOW==eWindow)
	{
		*pbTodo = TRUE;
		u16LineBuffOffset = SCALER_LINE_BUFFER_MAX_SC2 + SUB_MAIN_LINEOFFSET_GUARD_BAND_SC2;
	}

//	printf("[%s][%04d] fin(LB) = %d, eWin = %d, bForce = %d, bTodo = %d !!!\n", __FUNCTION__, __LINE__, u16LineBuffOffset, eWindow, bForceChange, *pbTodo);

	return u16LineBuffOffset;
}

#if ENABLE_DYNAMIC_SUB_LB_OFFSET == 0

static MS_U16 GetSubStartOffsetLB_fix(SCALER_WIN eWindow, MS_BOOL * pbTodo)
{
	MS_U16 u16LineBuffOffset;

	if (!pbTodo)
		return -1;

	*pbTodo = FALSE;

	if(eWindow==MAIN_WINDOW || eWindow == SC2_MAIN_WINDOW)
	{
		*pbTodo = TRUE;
		#if (HW_DESIGN_3D_VER == 4)

		if ( (MApi_XC_Get_3D_Input_Mode(eWindow)==E_XC_3D_INPUT_FRAME_PACKING) &&
			 (MApi_XC_Get_3D_Output_Mode()==E_XC_3D_OUTPUT_TOP_BOTTOM) )
		{
			u16LineBuffOffset = 0;
		}
		else
		{
			u16LineBuffOffset = eWindow==MAIN_WINDOW ?
						   (SCALER_LINE_BUFFER_MAX + SUB_MAIN_LINEOFFSET_GUARD_BAND) :
						   (SCALER_LINE_BUFFER_MAX_SC2 + SUB_MAIN_LINEOFFSET_GUARD_BAND_SC2);
		}
		#else

		u16LineBuffOffset = eWindow==MAIN_WINDOW ?
						   (SCALER_LINE_BUFFER_MAX + SUB_MAIN_LINEOFFSET_GUARD_BAND) :
						   (SCALER_LINE_BUFFER_MAX_SC2 + SUB_MAIN_LINEOFFSET_GUARD_BAND_SC2);

		#endif
	}

	return u16LineBuffOffset;
}
#endif

static MS_U16 GetSubStartOffsetLB(SCALER_WIN eWindow, MS_BOOL bForceChange, MS_BOOL * pbTodo)
{
#if ENABLE_DYNAMIC_SUB_LB_OFFSET
	return GetSubStartOffsetLB_dynamic(eWindow, bForceChange, pbTodo);
#else
	return GetSubStartOffsetLB_fix(eWindow, pbTodo);
#endif
}

#if ENABLE_DYNAMIC_SUB_LB_OFFSET == 0

static MS_U16 GetMaxLineBFF_fix(SCALER_WIN eWindow)
{
	MS_U16 u16MaxLineBFF = -1;

	switch (eWindow)
	{
		case SUB_WINDOW:
			u16MaxLineBFF = SUB_MST_LINE_BFF_MAX;
			break;

		case SC2_SUB_WINDOW:
			u16MaxLineBFF = SUB_MST_LINE_BFF_MAX_SC2;
			break;

		case SC1_MAIN_WINDOW:
			u16MaxLineBFF = MST_LINE_BFF_MAX_SC1;
			break;

		case SC2_MAIN_WINDOW:
			u16MaxLineBFF = MST_LINE_BFF_MAX_SC2;
			break;

		default:
		case MAIN_WINDOW:
			u16MaxLineBFF = MST_LINE_BFF_MAX;
			break;
	}

	return u16MaxLineBFF;
}

#endif

static MS_U16 GetMaxLineBFF_dynamic(SCALER_WIN eWindow)
{
	return u16MaxSizeOfLB[eWindow];
}

static MS_U16 GetMaxLineBFF(SCALER_WIN eWindow)
{
#if ENABLE_DYNAMIC_SUB_LB_OFFSET
	return GetMaxLineBFF_dynamic(eWindow);
#else
	return GetMaxLineBFF_fix(eWindow);
#endif
}

void MDrv_XC_ClearSizeOfLB(SCALER_WIN eWindow)
{
#if ENABLE_DYNAMIC_SUB_LB_OFFSET
	if ( MAX_WINDOW <= eWindow )
		return;

	u16MaxSizeOfLB[eWindow] = 0;
#endif
}

//-------------------------------------------------------------------------------------------------
/// Set two initial factors mode to improve quality in FBL
/// @param  bEnable                \b IN: enable or disable two initial factors mode
/// @param  eWindow                \b IN: @ref SCALER_WIN
//-------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_Enable_TwoInitFactor(MS_BOOL bEnable, SCALER_WIN eWindow)
{
    MS_BOOL bRet = FALSE;

    _XC_ENTRY();

    if(bEnable)
    {
        if(gSrcInfo[MAIN_WINDOW].bFBL && gSrcInfo[MAIN_WINDOW].bInterlace)
        {
            HAL_SC_Enable_VInitFactor(TRUE, eWindow);
            bRet = TRUE;
        }
    }
    else
    {
        HAL_SC_Enable_VInitFactor(FALSE, eWindow);
        bRet = TRUE;
    }

    _XC_RETURN();

    return bRet;
}

