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

#ifndef _MDRV_PNL_H
#define _MDRV_PNL_H


//-------------------------------------------------------------------------------------------------
//  Defines & Structure
//-------------------------------------------------------------------------------------------------
typedef struct
{
    u32 u32RiuBase;
}MDrvSclPnlInitConfig_t;



typedef struct
{
    u16 u16Vsync_St; ///< vsync start point
    u16 u16Vsync_End;///< vsync end point
    u16 u16Vde_St;   ///< Vdata enable start point
    u16 u16Vde_End;  ///< Vdata enable end point
    u16 u16Vfde_St;  ///< V framecolor data enable start point
    u16 u16Vfde_End; ///< V framecolor data enable end point
    u16 u16Vtt;      ///< V total
    u16 u16Hsync_St; ///< hsync start point
    u16 u16Hsync_End;///< hsync end point
    u16 u16Hde_St;   ///< Hdata enable start point
    u16 u16Hde_End;  ///< Hdata enable end point
    u16 u16Hfde_St;  ///< H framecolor data enable start point
    u16 u16Hfde_End; ///< H framecolor data enable end point
    u16 u16Htt;      ///< H total
    u16 u16VFreqx10; ///< FPS x10
}MDrvSclPnlTimingConfig_t;


//-------------------------------------------------------------------------------------------------
//  Prototype
//-------------------------------------------------------------------------------------------------
#ifndef _MDRV_PNL_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

INTERFACE void MDrvSclPnlRelease(void);
INTERFACE bool MDrvSclPnlInit(MDrvSclPnlInitConfig_t *pCfg);
INTERFACE void MDrvSclPnlExit(bool bCloseISR);
INTERFACE bool MDrvSclPnlSetTimingConfig(MDrvSclPnlTimingConfig_t *pCfg);
INTERFACE bool MDrvSclPnlSetLPLLConfig(MDrvSclPnlTimingConfig_t *pCfg);
INTERFACE void MDrvSclPnlResume(void);
#undef INTERFACE

#endif
