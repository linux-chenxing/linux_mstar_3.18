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
#ifndef __DRV_SCL_MGWIN_H__
#define __DRV_SCL_MGWIN_H__

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define DRV_SCLMGWIN_NUM            2
#define DRV_SCLMGWIN_SUBWIN_NUM     16

#define DRV_SCLMGWIN_INSTANT_MAX    1
//-------------------------------------------------------------------------------------------------
//  enum
//-------------------------------------------------------------------------------------------------
typedef enum
{
    E_DRV_SCLMGWIN_WIN_ID_1,
    E_DRV_SCLMGWIN_WIN_ID_2,
    E_DRV_SCLMGWIN_WIN_ID_NUM,
}DrvSclMgwinWinIdType_e;

typedef enum
{
    E_DRV_SCLMGWIN_SUBWIN_ID_0,  ///< ID_0
    E_DRV_SCLMGWIN_SUBWIN_ID_1,  ///< ID_1
    E_DRV_SCLMGWIN_SUBWIN_ID_2,  ///< ID_2
    E_DRV_SCLMGWIN_SUBWIN_ID_3,  ///< ID_3
    E_DRV_SCLMGWIN_SUBWIN_ID_4,  ///< ID_4
    E_DRV_SCLMGWIN_SUBWIN_ID_5,  ///< ID_5
    E_DRV_SCLMGWIN_SUBWIN_ID_6,  ///< ID_6
    E_DRV_SCLMGWIN_SUBWIN_ID_7,  ///< ID_7
    E_DRV_SCLMGWIN_SUBWIN_ID_8,  ///< ID_8
    E_DRV_SCLMGWIN_SUBWIN_ID_9,  ///< ID_9
    E_DRV_SCLMGWIN_SUBWIN_ID_10, ///< ID_10
    E_DRV_SCLMGWIN_SUBWIN_ID_11, ///< ID_11
    E_DRV_SCLMGWIN_SUBWIN_ID_12, ///< ID_12
    E_DRV_SCLMGWIN_SUBWIN_ID_13, ///< ID_13
    E_DRV_SCLMGWIN_SUBWIN_ID_14, ///< ID_14
    E_DRV_SCLMGWIN_SUBWIN_ID_15, ///< ID_15
    E_DRV_SCLMGWIN_SUBWIN_ID_NUM,
}DrvSclMgwinSubWinIdType_e;

typedef enum
{
    E_DRV_SCLMGWIN_MEM_FMT_ARGB8888,         ///< Memory Format: ARGB8888
    E_DRV_SCLMGWIN_MEM_FMT_ABGR8888,         ///< Memory Format: ABGR8888
    E_DRV_SCLMGWIN_MEM_FMT_RGBA8888,         ///< Memory Format: RGBA8888
    E_DRV_SCLMGWIN_MEM_FMT_BGRA8888,         ///< Memory Format: BGRA8888
    E_DRV_SCLMGWIN_MEM_FMT_RGB565,           ///< Memory Format: RGB565
    E_DRV_SCLMGWIN_MEM_FMT_YUV444_8B,        ///< Memory Format: YUV444 8b
    E_DRV_SCLMGWIN_MEM_FMT_YUV444_10B,       ///< Memory Format: YUV444 10b
    E_DRV_SCLMGWIN_MEM_FMT_YUV422_8B,        ///< Memory Format: YUV422 8B
    E_DRV_SCLMGWIN_MEM_FMT_YUV422_8CE,       ///< Memory Format: YUV422 8CE
    E_DRV_SCLMGWIN_MEM_FMT_YUV422_6CE,       ///< Memory Format: YUV422 6CE
    E_DRV_SCLMGWIN_MEM_FMT_YUV420_8B,        ///< Memory Format: YUV420 8B
    E_DRV_SCLMGWIN_MEM_FMT_YUV420_8CE,       ///< Memory Format: YUV420 8CE
    E_DRV_SCLMGWIN_MEM_FMT_YUV420_6CE,       ///< Memory Format: YUV420 6CE
    E_DRV_SCLMGWIN_MEM_FMT_NUM,
} DrvSclMgwinMemFormat_e;

typedef enum
{
    E_DRV_SCLMGWIN_PATH_SC1,
    E_DRV_SCLMGWIN_PATH_SC2,
    E_DRV_SCLMGWIN_PATH_NUM,
}DrvSclMgwinFramePath_e;


//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------

typedef struct
{
    u16 u16X;        ///< horizontal starting position
    u16 u16Y;        ///< vertical starting position
    u16 u16Width;    ///< horizontal size
    u16 u16Height;   ///< vertical size
} DrvSclMgwinWindowConfig_t;

typedef struct
{
    DrvSclMgwinWinIdType_e enWinId;
    DrvSclMgwinWindowConfig_t stWin;
 }DrvSclMgwinFrameConfig_t;

typedef struct
{
    DrvSclMgwinWinIdType_e enWinId;
    bool bEn;                                   ///< On/Off
}DrvSclMgwinFrameOnOffConfig_t;


typedef struct
{
    DrvSclMgwinWinIdType_e enWinId;             ///< Id of Frame
    DrvSclMgwinSubWinIdType_e enSubWinId;       ///< Id of SubWin
    DrvSclMgwinWindowConfig_t stWin;            ///< Display WIndow
}DrvSclMgwinSubWinConfig_t;


typedef struct
{
    DrvSclMgwinWinIdType_e enWinId;             ///< Id of Frame
    DrvSclMgwinSubWinIdType_e enSubWinId;       ///< Id of SubWin
    bool bEnable;                               ///< enable/disable SubWin
}DrvSclMgwinSubWinOnOffConfig_t;

typedef struct
{
    DrvSclMgwinWinIdType_e enWinId;             ///< Id of Frame
    DrvSclMgwinSubWinIdType_e enSubWinId;       ///< Id of SubWin
    DrvSclMgwinMemFormat_e enMemFmt;            ///< memory format
    u32 u32BaseAddr;                            ///< base address
    u32 u32Pitch;                               ///< Pitch
} DrvSclMgwinSubWinFlipConfig_t;

typedef struct
{
    DrvSclMgwinWinIdType_e enWinId;             ///< Id of Frame
    bool bTrigger;                              ///< status of HW register tirgger
}DrvSclMgwinDbfConfig_t;

typedef struct
{
    DrvSclMgwinWinIdType_e enWinId;             ///< Id of Frame
    DrvSclMgwinFramePath_e enPathType;          ///< Path type
}DrvSclMgwinFramePathConfig_t;

typedef struct
{
    u32  u32Base[DRV_SCLMGWIN_SUBWIN_NUM];
    bool bValid[DRV_SCLMGWIN_SUBWIN_NUM];
}DrvSclMgwinActiveBaseConfig_t;
//------------------------------------------------------------------------------------------------
//  Prototype
//-------------------------------------------------------------------------------------------------

#ifndef __DRV_SCL_MGWIN_C__
#define INTERFACE extern
#else
#define INTERFACE
#endif
INTERFACE bool DrvSclMgwinInit(void);
INTERFACE void *DrvSclMgwinCtxAllocate(void);
INTERFACE bool DrvSclMgwinCtxFree(void *);
INTERFACE bool DrvSclMgwinSetFrameConfig_t(void *pCtx, DrvSclMgwinFrameConfig_t *pFrameCfg);
INTERFACE bool DrvSclMgwinSetFrameConfig(void *pCtx, DrvSclMgwinFrameConfig_t *pFrameCfg);
INTERFACE bool DrvSclMgwinSetFrameOnOffConfig(void *pCtx, DrvSclMgwinFrameOnOffConfig_t *pFrameOnOffCfg);
INTERFACE bool DrvSclMgwinSetFramePathConfig(void *pCtx, DrvSclMgwinFramePathConfig_t *pFramePathCfg);
INTERFACE bool DrvSclMgwinSetSubWinConfig(void *pCtx, DrvSclMgwinSubWinConfig_t *pSubWinCfg);
INTERFACE bool DrvSclMgwinSetSubWinOnOffConfig(void *pCtx, DrvSclMgwinSubWinOnOffConfig_t *pSubWinOnOfffCfg);
INTERFACE bool DrvSclMgwinSetSubWinFlipConfig(void *pCtx, DrvSclMgwinSubWinFlipConfig_t *pSubWinFlipCfg);
INTERFACE bool DrvSclMgwinSetDbfConfig(void *pCtx, DrvSclMgwinDbfConfig_t *pDbfCfg);
INTERFACE bool DrvSclMgwinGetDbfConfig(void *pCtx, DrvSclMgwinDbfConfig_t *pDbfCfg);
INTERFACE bool DrvSclMgwinSetRegister(u32 u32Addr, u16 u16Val, u16 u16Msk);


#undef INTERFACE

#endif
