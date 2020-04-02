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
#ifndef __DRV_SCL_PNL_IO_WRAPPER_H__
#define __DRV_SCL_PNL_IO_WRAPPER_H__

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define DRV_SCLPNL_HANDLER_PRE_FIX     0x12340000
#define DRV_SCLPNL_HANDLER_PRE_MASK    0xFFFF0000
#define DRV_SCLPNL_HANDLER_MAX         4

#define IsSclPnlIdValid(x)             (x == E_DRV_SCLPNL_IO_ID_1)


//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------
typedef struct
{
    s32 s32Handle;
    void *pPrivate_Data;
    DrvSclPnlIoIdType_e enSclPnlId;
} DrvSclPnlHandleConfig_t;


typedef struct
{
    u32 u32StructSize;
    u32 *pVersion;
    u32 u32VersionSize;
} DrvSclPnlVersionChkConfig_t;

//-------------------------------------------------------------------------------------------------
//  Prototype
//-------------------------------------------------------------------------------------------------

#ifndef __DRV_SCL_PNL_IO_WRAPPER_C__
#define INTERFACE extern
#else
#define INTERFACE
#endif

INTERFACE void _DrvSclPnlIoInitHandler(void);
INTERFACE s32  _DrvSclPnlIoOpen(DrvSclPnlIoIdType_e enSclPnlId);
INTERFACE DrvSclPnlIoErrType_e _DrvSclPnlIoRelease(s32 s32Handler);
INTERFACE DrvSclPnlIoErrType_e _DrvSclPnlIoSetTimingConfig(s32 s32Handler, DrvSclPnlIoTimingConfig_t *pstIOPnlTimingCfg);
INTERFACE DrvSclPnlIoErrType_e _DrvSclPnlIoSetLpllConfig(s32 s32Handler, DrvSclPnlIoTimingConfig_t *pstIOPnlTimingCfg);
INTERFACE DrvSclPnlIoErrType_e _DrvSclPnlIoGetVersionConfig(s32 s32Handler, DrvSclPnlIoVersionConfig_t *psIOVersionCfg);

#undef INTERFACE

#endif

