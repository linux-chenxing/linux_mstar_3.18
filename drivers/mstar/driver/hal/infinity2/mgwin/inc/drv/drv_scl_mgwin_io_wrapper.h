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


//=============================================================================
#ifndef _DRV_SCL_MGWIN_IO_WRAPPER_H__
#define _DRV_SCL_MGWIN_IO_WRAPPER_H__

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define DRV_SCLMGWIN_HANDLER_PRE_FIX         0x12330000
#define DRV_SCLMGWIN_HANDLER_PRE_MASK        0xFFFF0000
#define DRV_SCLMGWIN_HANDLER_INSTANCE_NUM    (1)
#define DRV_SCLMGWIN_HANDLER_MAX             (2 * DRV_SCLMGWIN_HANDLER_INSTANCE_NUM)


//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------
typedef struct
{
    void *pCtx;
}DrvSclMgwinIoCtxConfig_t;


typedef struct
{
    s32 s32Handle;
    DrvSclMgwinIoCtxConfig_t stCtxCfg;
} DrvSclMgwinHandleConfig_t;

typedef struct
{
    u32 u32StructSize;
    u32 *pVersion;
    u32 u32VersionSize;
} DrvSclMgwinVersionChkConfig_t;


//------------------------------------------------------------------------------------------------
//  Prototype
//-------------------------------------------------------------------------------------------------

#ifndef __DRV_SCL_MGWIN_IO_WRAPPER_C__
#define INTERFACE extern
#else
#define INTERFACE
#endif

INTERFACE bool                   _DrvSclMgwinIoInit(void);
INTERFACE s32                    _DrvSclMgwinIoOpen(void);
INTERFACE DrvSclMgwinIoErrType_e _DrvSclMgwinIoRelease(s32 s32Handler);
INTERFACE DrvSclMgwinIoErrType_e _DrvSclMgwinIoSetFrameConfig(s32 s32Handler, DrvSclMgwinIoFrameConfig_t *pstIoFrameCfg);
INTERFACE DrvSclMgwinIoErrType_e _DrvSclMgwinIoSetFrameOnOffConfig(s32 s32Handler, DrvSclMgwinIoFrameOnOffConfig_t *pstIoOnOffCfg);
INTERFACE DrvSclMgwinIoErrType_e _DrvSclMgwinIoSetSubWinConfig(s32 s32Handler, DrvSclMgwinIoSubWinConfig_t *pstIoSubWinCfg);
INTERFACE DrvSclMgwinIoErrType_e _DrvSclMgwinIoSetSubWinOnOffConfig(s32 s32Handler, DrvSclMgwinIoSubWinOnOffConfig_t *pstIoSubWinOnOffCfg);
INTERFACE DrvSclMgwinIoErrType_e _DrvSclMgwinIoSetSubWinFlipConfig(s32 s32Handler, DrvSclMgwinIoSubWinFlipConfig_t *pstIoSubWinFlipCfg);
INTERFACE DrvSclMgwinIoErrType_e _DrvSclMgwinIoSetDbfConfig(s32 s32Handler, DrvSclMgwinIoDbfConfig_t *pstIoDbfCfg);
INTERFACE DrvSclMgwinIoErrType_e _DrvSclMgwinIoSetFramePathConfig(s32 s32Handler, DrvSclMgwinIoFramePathConfig_t *pstIoFramePathCfg);
INTERFACE DrvSclMgwinIoErrType_e _DrvSclMgwinIoGetDbfConfig(s32 s32Handler, DrvSclMgwinIoDbfConfig_t *pstIoDbfCfg);
INTERFACE DrvSclMgwinIoErrType_e _DrvSclMgwinIoSetRegisterConfig(s32 s32Handler, DrvSclMgwinIoRegisterConfig_t *pstIoRegCfg);

#undef INTERFACE

#endif
