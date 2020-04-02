////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2011 MStar Semiconductor, Inc.
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
#define _MDRV_PNL_C
#include "drv_scl_os.h"
#include "drv_scl_dbg.h"
#include "hal_scl_util.h"
#include "drv_scl_pnl.h"
#include "drv_scl_pnl_m.h"


//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
/////////////////
/// gstCfg
/// use in suspend/resume to save information
////////////////
MDrvSclPnlTimingConfig_t gstCfg;
MDrvSclPnlTimingConfig_t gstLPLLCfg;
/////////////////
/// gbResume
/// use to determine whether need to set timing config as resume.
////////////////
bool gbResume=0;
bool gbResumeLPLL=0;
//-------------------------------------------------------------------------------------------------
//  Function
//-------------------------------------------------------------------------------------------------
void MDrvSclPnlRelease(void)
{
    if(gbResume|| gbResumeLPLL)
    {
        DrvSclPnlRelease();
    }
}
void MDrvSclPnlExit(bool bCloseISR)
{
    DrvSclPnlExit(bCloseISR);
}
bool MDrvSclPnlInit(MDrvSclPnlInitConfig_t *pCfg)
{
    DrvSclPnlInitConfig_t stInitCfg;
    stInitCfg.u32RiuBase = pCfg->u32RiuBase;

    if(DrvSclPnlInit(&stInitCfg) == 0)
    {
        SCL_ERR("[PNL]%s, Init Fail\n", __FUNCTION__);
        return 0;
    }
    else
    {
        return 1;
    }
}
void MDrvSclPnlResume(void)
{
    DrvSclPnlResume();
    if(gbResume)
    {
        MDrvSclPnlSetTimingConfig(&gstCfg);
    }
    if(gbResumeLPLL)
    {
        MDrvSclPnlSetLPLLConfig(&gstLPLLCfg);
    }
}

bool MDrvSclPnlSetLPLLConfig(MDrvSclPnlTimingConfig_t *pCfg)
{
    DrvSclPnlTimingConfig_t stTimingCfg;

    stTimingCfg.u16Vtt       = pCfg->u16Vtt;
    stTimingCfg.u16Htt       = pCfg->u16Htt;
    stTimingCfg.u16VFreqx10  = pCfg->u16VFreqx10;
    DrvSclOsMemcpy(&gstLPLLCfg, pCfg,sizeof(MDrvSclPnlTimingConfig_t));
    gbResumeLPLL=1;
    if(DrvSclPnlSetLPLLConfig(&stTimingCfg) == 0)
    {
        SCL_ERR("[PNL]%s, Set Timing Fail\n", __FUNCTION__);
        return 0;
    }
    else
    {
        return 1;
    }

}

bool MDrvSclPnlSetTimingConfig(MDrvSclPnlTimingConfig_t *pCfg)
{
    DrvSclPnlTimingConfig_t stTimingCfg;

    stTimingCfg.u16Vsync_St  = pCfg->u16Vsync_St;
    stTimingCfg.u16Vsync_End = pCfg->u16Vsync_End;
    stTimingCfg.u16Vde_St    = pCfg->u16Vde_St;
    stTimingCfg.u16Vde_End   = pCfg->u16Vde_End;
    stTimingCfg.u16Vfde_St   = pCfg->u16Vfde_St;
    stTimingCfg.u16Vfde_End  = pCfg->u16Vfde_End;
    stTimingCfg.u16Vtt       = pCfg->u16Vtt;

    stTimingCfg.u16Hsync_St  = pCfg->u16Hsync_St;
    stTimingCfg.u16Hsync_End = pCfg->u16Hsync_End;
    stTimingCfg.u16Hde_St    = pCfg->u16Hde_St;
    stTimingCfg.u16Hde_End   = pCfg->u16Hde_End;
    stTimingCfg.u16Hfde_St   = pCfg->u16Hfde_St;
    stTimingCfg.u16Hfde_End  = pCfg->u16Hfde_End;

    stTimingCfg.u16Htt       = pCfg->u16Htt;

    stTimingCfg.u16VFreqx10  = pCfg->u16VFreqx10;
    DrvSclOsMemcpy(&gstCfg, pCfg,sizeof(MDrvSclPnlTimingConfig_t));
    gbResume=1;
    if( DrvSclPnlSetTimingConfig(&stTimingCfg) == 0)
    {
        SCL_ERR("[PNL]%s, Set Timing Fail\n", __FUNCTION__);
        return 0;
    }
    else
    {
        return 1;
    }

}
