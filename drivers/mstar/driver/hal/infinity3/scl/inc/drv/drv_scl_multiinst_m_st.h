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

#ifndef _MDRV_MULTI_INST_ST_H
#define _MDRV_MULTI_INST_ST_H


#define MDRV_SCLMULTI_INST_HVSP_NUM    MDRV_SCLMULTI_INST_PRIVATE_ID_NUM
#define MDRV_SCLMULTI_INST_SCLDMA_NUM  MDRV_SCLMULTI_INST_PRIVATE_ID_NUM
//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
typedef enum
{
    E_MDRV_SCLMULTI_INST_HVSP_DATA_ID_1,
    E_MDRV_SCLMULTI_INST_HVSP_DATA_ID_2,
    E_MDRV_SCLMULTI_INST_HVSP_DATA_ID_3,
    E_MDRV_SCLMULTI_INST_HVSP_DATA_ID_MAX,
}MDrvSclMultiInstHvspDataIdType_e;


typedef enum
{
    E_MDRV_SCLMULTI_INST_DMA_DATA_ID_1,
    E_MDRV_SCLMULTI_INST_DMA_DATA_ID_2,
    E_MDRV_SCLMULTI_INST_DMA_DATA_ID_3,
    E_MDRV_SCLMULTI_INST_DMA_DATA_ID_4,
    E_MDRV_SCLMULTI_INST_DMA_DATA_ID_MAX,
}MDrvSclMultiInstDmaDataIdType_e;


//-------------------------------------------------------------------------------------------------
//  Structure
//-------------------------------------------------------------------------------------------------

//===============================
//      HVSP multi instance
//===============================
typedef struct
{
    bool   bInCfg    : 1;
    bool   bScaleCfg : 1;
    bool   bPostCropCfg : 1;
    bool   bMemCfg      : 1;
    bool   bOSDCfg      : 1;
    bool   Reserved     : 2;
}MDrvSclMultiInstHvspFlagType_t;

typedef struct
{
    MDrvSclMultiInstHvspFlagType_t stFlag;
    MDrvSclHvspInputConfig_t       stInCfg;
    MDrvSclHvspScalingConfig_t    stScaleCfg;
    MDrvSclHvspIpmConfig_t         stMemCfg;
    MDrvSclHvspPostCropConfig_t   stPostCropCfg;
    MDrvSclHvspOsdConfig_t         stOSDCfg;
}MDrvSclMultiInstHvspDataType_t;

typedef struct
{
    s32 s32PivateId;
    bool bUsed;
    MDrvSclMultiInstHvspDataType_t stData;
}MDrvSclMultiInstHvspEntryConfig_t;


//===============================
//      SCLDMA multi instance
//===============================
typedef struct
{
    bool   bInBufCfg      ;
    bool   bOutBufCfg[E_MDRV_SCLDMA_MEM_NUM]  ;
    bool   bInTrigCfg     ;
    bool   bOutTrigCfg[E_MDRV_SCLDMA_MEM_NUM] ;
}MDrvSclMultiInstDmaFlagType_t;

typedef struct
{
    MDrvSclMultiInstDmaFlagType_t stFlag;
    MDrvSclDmaBufferConfig_t stInBufCfg;
    MDrvSclDmaBufferConfig_t stOutBufCfg[E_MDRV_SCLDMA_MEM_NUM];
    MDrvSclDmaTriggerConfig_t stInTrigCfg;
    MDrvSclDmaTriggerConfig_t stOutTrigCfg[E_MDRV_SCLDMA_MEM_NUM];
}MDrvSclMultiInstDmaDataType_t;


typedef struct
{
    s32 s32PivateId;
    bool bUsed;
    MDrvSclMultiInstDmaDataType_t stData;
}MDrvSclMultiInstDmaEntryConfig_t;

//-------------------------------------------------------------------------------------------------
//  Prototype
//-------------------------------------------------------------------------------------------------
#ifndef _MDRV_MULTI_INST_C
#define INTERFACE extern
#else
#define INTERFACE
#endif


#undef INTERFACE

#endif
