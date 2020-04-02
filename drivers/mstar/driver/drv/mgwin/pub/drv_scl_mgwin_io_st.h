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

/**
 *  @file drv_scl_hvsp_io_st.h
 *  @brief SclMgwin Driver struct parameter interface
 */

/**
* \ingroup sclmgwin_group
* @{
*/

#ifndef __DRV_SCL_MGWIN_IO_ST_H__
#define __DRV_SCL_MGWIN_IO_ST_H__

//=============================================================================
// Defines
//=============================================================================
#define DRV_SCLMGWIN_VERSION                        0x0100


//=============================================================================
// enum
//=============================================================================

/// @cond
/**
* The ID type of SclMgwin
*/
typedef enum
{
    E_DRV_SCLMGWIN_IO_WIN_ID_1,      ///< ID_1
    E_DRV_SCLMGWIN_IO_WIN_ID_2,      ///< ID_2
    E_DRV_SCLMGWIN_IO_WIN_ID_NUM,    ///< The max number of ID
} DrvSclMgwinIoWinIdType_e;
/// @endcond

typedef enum
{
    E_DRV_SCLMGWIN_IO_SUBWIN_ID_0,  ///< ID_0
    E_DRV_SCLMGWIN_IO_SUBWIN_ID_1,  ///< ID_1
    E_DRV_SCLMGWIN_IO_SUBWIN_ID_2,  ///< ID_2
    E_DRV_SCLMGWIN_IO_SUBWIN_ID_3,  ///< ID_3
    E_DRV_SCLMGWIN_IO_SUBWIN_ID_4,  ///< ID_4
    E_DRV_SCLMGWIN_IO_SUBWIN_ID_5,  ///< ID_5
    E_DRV_SCLMGWIN_IO_SUBWIN_ID_6,  ///< ID_6
    E_DRV_SCLMGWIN_IO_SUBWIN_ID_7,  ///< ID_7
    E_DRV_SCLMGWIN_IO_SUBWIN_ID_8,  ///< ID_8
    E_DRV_SCLMGWIN_IO_SUBWIN_ID_9,  ///< ID_9
    E_DRV_SCLMGWIN_IO_SUBWIN_ID_10, ///< ID_10
    E_DRV_SCLMGWIN_IO_SUBWIN_ID_11, ///< ID_11
    E_DRV_SCLMGWIN_IO_SUBWIN_ID_12, ///< ID_12
    E_DRV_SCLMGWIN_IO_SUBWIN_ID_13, ///< ID_13
    E_DRV_SCLMGWIN_IO_SUBWIN_ID_14, ///< ID_14
    E_DRV_SCLMGWIN_IO_SUBWIN_ID_15, ///< ID_15
    E_DRV_SCLMGWIN_IO_SUBWIN_ID_NUM,
}DrvSclMgwinIoSubWinIdType_e;

/**
* The Memory Format type of SclMgwin
*/
typedef enum
{
    E_DRV_SCLMGWIN_IO_MEM_FMT_ARGB8888,         ///< Memory Format: ARGB8888
    E_DRV_SCLMGWIN_IO_MEM_FMT_ABGR8888,         ///< Memory Format: ABGR8888
    E_DRV_SCLMGWIN_IO_MEM_FMT_RGBA8888,         ///< Memory Format: RGBA8888
    E_DRV_SCLMGWIN_IO_MEM_FMT_BGRA8888,         ///< Memory Format: BGRA8888
    E_DRV_SCLMGWIN_IO_MEM_FMT_RGB565,           ///< Memory Format: RGB565
    E_DRV_SCLMGWIN_IO_MEM_FMT_YUV444_8B,        ///< Memory Format: YUV444 8b
    E_DRV_SCLMGWIN_IO_MEM_FMT_YUV444_10B,       ///< Memory Format: YUV444 10b
    E_DRV_SCLMGWIN_IO_MEM_FMT_YUV422_8B,        ///< Memory Format: YUV422 8B
    E_DRV_SCLMGWIN_IO_MEM_FMT_YUV422_8CE,       ///< Memory Format: YUV422 8CE
    E_DRV_SCLMGWIN_IO_MEM_FMT_YUV422_6CE,       ///< Memory Format: YUV422 6CE
    E_DRV_SCLMGWIN_IO_MEM_FMT_YUV420_8B,        ///< Memory Format: YUV420 8B
    E_DRV_SCLMGWIN_IO_MEM_FMT_YUV420_8CE,       ///< Memory Format: YUV420 8CE
    E_DRV_SCLMGWIN_IO_MEM_FMT_YUV420_6CE,       ///< Memory Format: YUV420 6CE
    E_DRV_SCLMGWIN_IO_MEM_FMT_NUM,
} DrvSclMgwinIoMemFormat_e;

/**
* The Frame Path type of SclMgwin
*/
typedef enum
{
    E_DRV_SCLMGWIN_IO_PATH_SC1,
    E_DRV_SCLMGWIN_IO_PATH_SC2,
    E_DRV_SCLMGWIN_IO_PATH_NUM,
}DrvSclMgwinIoFramePath_e;

///@cond
/**
* The error type of SclHvsp
*/
typedef enum
{
    E_DRV_SCLMGWIN_IO_ERR_OK    =  0, ///< No Error
    E_DRV_SCLMGWIN_IO_ERR_FAULT = -1, ///< Fault
    E_DRV_SCLMGWIN_IO_ERR_INVAL = -2, ///< Invalid value
    E_DRV_SCLMGWIN_IO_ERR_MULTI = -3, ///< MultiInstance Fault
}DrvSclMgwinIoErrType_e;
/// @endcond


//=============================================================================
// struct
//=============================================================================
/**
*  The Version of SclMgwin
*/
typedef struct
{
    u32   VerChk_Version ; ///< structure version
    u32   u32Version;      ///< version
    u32   VerChk_Size;     ///< structure size for version checking
} __attribute__ ((__packed__))DrvSclMgwinIoVersionConfig_t;

/**
* The rectangle configuration for Crop Window, Capture Window, Display Window of SclMgwin
*/
typedef struct
{
    u16 u16X;        ///< horizontal starting position
    u16 u16Y;        ///< vertical starting position
    u16 u16Width;    ///< horizontal size
    u16 u16Height;   ///< vertical size
} __attribute__ ((__packed__))DrvSclMgwinIoWindowConfig_t;


/**
*  The Frame configuration of SclMgwin
*/
typedef struct
{
    u32   VerChk_Version ;                      ///< structure version
    DrvSclMgwinIoWinIdType_e enWinId;
    DrvSclMgwinIoWindowConfig_t stWin;
    // VerChk_Version & VerChk_Size must be the latest 2 parameter and
    // the order can't be changed
    u32   VerChk_Size;                          ///< structure size for version checking
} __attribute__ ((__packed__))DrvSclMgwinIoFrameConfig_t;


/**
*  The Frame OnOff configuration of SclMgwin
*/
typedef struct
{
    u32   VerChk_Version ;                      ///< structure version
    DrvSclMgwinIoWinIdType_e enWinId;
    bool bEn;                                   ///< On/Off
    // VerChk_Version & VerChk_Size must be the latest 2 parameter and
    // the order can't be changed
    u32   VerChk_Size;                          ///< structure size for version checking
}__attribute__ ((__packed__))DrvSclMgwinIoFrameOnOffConfig_t;

/**
*  The subwin configuration of SclMgwin
*/
typedef struct
{
    u32   VerChk_Version ;                      ///< structure version
    DrvSclMgwinIoWinIdType_e enWinId;           ///< Id of Frame
    DrvSclMgwinIoSubWinIdType_e enSubWinId;     ///< Id of SubWin
    DrvSclMgwinIoWindowConfig_t stWin;          ///< Display window
    // VerChk_Version & VerChk_Size must be the latest 2 parameter and
    // the order can't be changed
    u32   VerChk_Size;                          ///< structure size for version checking
} __attribute__ ((__packed__))DrvSclMgwinIoSubWinConfig_t;


/**
*  The show configuration of SclMgwin
*/
typedef struct
{
    u32   VerChk_Version ;                      ///< structure version
    DrvSclMgwinIoWinIdType_e enWinId;           ///< Id of Frame
    DrvSclMgwinIoSubWinIdType_e enSubWinId;     ///< Id of SubWin
    bool bEnable;                               ///< enable/disable SubWin
    // VerChk_Version & VerChk_Size must be the latest 2 parameter and
    // the order can't be changed
    u32   VerChk_Size;                          ///< structure size for version checking
} __attribute__ ((__packed__))DrvSclMgwinIoSubWinOnOffConfig_t;

/**
*  The Flip configuration of SclMgwin
*/
typedef struct
{
    u32   VerChk_Version ;                      ///< structure version
    DrvSclMgwinIoWinIdType_e enWinId;           ///< Id of Frame
    DrvSclMgwinIoSubWinIdType_e enSubWinId;     ///< Id of SubWin
    DrvSclMgwinIoMemFormat_e enMemFmt;          ///< memory format
    u32 u32BaseAddr;                            ///< base address
    u32 u32Pitch;                               ///< Pitch
    // VerChk_Version & VerChk_Size must be the latest 2 parameter and
    // the order can't be changed
    u32   VerChk_Size;                          ///< structure size for version checking
} __attribute__ ((__packed__))DrvSclMgwinIoSubWinFlipConfig_t;

/**
*  The Register Update configuration of SclMgwin
*/
typedef struct
{
    u32   VerChk_Version ;                      ///< structure version
    DrvSclMgwinIoWinIdType_e enWinId;           ///< Id of Frame
    bool bTrigger;                              ///< status of Reigster trigger
    // VerChk_Version & VerChk_Size must be the latest 2 parameter and
    // the order can't be changed
    u32   VerChk_Size;                          ///< structure size for version checking
} __attribute__ ((__packed__))DrvSclMgwinIoDbfConfig_t;

typedef struct
{
    u32   VerChk_Version ;                      ///< structure version
    DrvSclMgwinIoWinIdType_e enWinId;           ///< Id of Frame
    DrvSclMgwinIoFramePath_e enPathType;        ///< Path type
    // VerChk_Version & VerChk_Size must be the latest 2 parameter and
    // the order can't be changed
    u32   VerChk_Size;
}__attribute__ ((__packed__))DrvSclMgwinIoFramePathConfig_t;

typedef struct
{
    u32 u32Addr;
    u16 u16Val;
    u16 u16Msk;
}__attribute__ ((__packed__))DrvSclMgwinIoRegisterConfig_t;

///@cond
/**
* The configuration of rotate for SCLPNL
*/
typedef struct
{
    DrvSclMgwinIoErrType_e (*DrvSclMgwinIoSetFrameConfig)(s32 s32Handler, DrvSclMgwinIoFrameConfig_t *pstIoFrameCfg);
    DrvSclMgwinIoErrType_e (*DrvSclMgwinIoSetFrameOnOffConfig)(s32 s32Handler, DrvSclMgwinIoFrameOnOffConfig_t *pstIoOnOffCfg);
    DrvSclMgwinIoErrType_e (*DrvSclMgwinIoSetSubWinConfig)(s32 s32Handler, DrvSclMgwinIoSubWinConfig_t *pstIoSubWinCfg);
    DrvSclMgwinIoErrType_e (*DrvSclMgwinIoSetSubWinOnOffConfig)(s32 s32Handler, DrvSclMgwinIoSubWinOnOffConfig_t *pstIoSubWinOnOffCfg);
    DrvSclMgwinIoErrType_e (*DrvSclMgwinIoSetSubWinFlipConfig)(s32 s32Handler, DrvSclMgwinIoSubWinFlipConfig_t *pstIoSubWinFlipCfg);
    DrvSclMgwinIoErrType_e (*DrvSclMgwinIoSetFramePathConfig)(s32 s32Handler, DrvSclMgwinIoFramePathConfig_t *pstIoFramePathCfg);
    DrvSclMgwinIoErrType_e (*DrvSclMgwinIoSetDbfConfig)(s32 s32Handler, DrvSclMgwinIoDbfConfig_t *pstDbfCfg);
    DrvSclMgwinIoErrType_e (*DrvSclMgwinIoGetDbfConfig)(s32 s32Handler, DrvSclMgwinIoDbfConfig_t *pstDbfCfg);
    DrvSclMgwinIoErrType_e (*DrvSclMgwinIoSetRegisterConfig)(s32 s32Handler, DrvSclMgwinIoRegisterConfig_t *pstRegisterCfg);
}DrvSclMgwinIoFunctionConfig_t;
///@endcond


//=============================================================================

#endif //
/** @} */ // end of sclmgwin_group
