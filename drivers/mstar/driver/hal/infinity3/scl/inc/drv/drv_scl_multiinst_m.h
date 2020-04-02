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

#ifndef _MDRV_MULTI_INST_H
#define _MDRV_MULTI_INST_H


#define MDRV_SCLMULTI_INST_PRIVATE_ID_NUM    4
//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
typedef enum
{
    E_MDRV_SCLMULTI_INST_ENTRY_ID_HVSP1,
    E_MDRV_SCLMULTI_INST_ENTRY_ID_HVSP2,
    E_MDRV_SCLMULTI_INST_ENTRY_ID_HVSP3,
    E_MDRV_SCLMULTI_INST_ENTRY_ID_SCLDMA1,
    E_MDRV_SCLMULTI_INST_ENTRY_ID_SCLDMA2,
    E_MDRV_SCLMULTI_INST_ENTRY_ID_SCLDMA3,
    E_MDRV_SCLMULTI_INST_ENTRY_ID_DISP,
    E_MDRV_SCLMULTI_INST_ENTRY_ID_MAX,
}MDrvSclMultiInstEntryIdType_e;


typedef enum
{
    E_MDRV_SCLMULTI_INST_LOCK_ID_SC_1_2,
    E_MDRV_SCLMULTI_INST_LOCK_ID_SC_3,
    E_MDRV_SCLMULTI_INST_LOCK_ID_DISP,
    E_MDRV_SCLMULTI_INST_LOCK_ID_MAX,
}MDrvSclMultiInstLockIdType_e;


typedef enum
{
    E_MDRV_SCLMULTI_INST_CMD_HVSP_IN_CONFIG,
    E_MDRV_SCLMULTI_INST_CMD_HVSP_SCALING_CONFIG,
    E_MDRV_SCLMULTI_INST_CMD_HVSP_MEM_REQ_CONFIG,
    E_MDRV_SCLMULTI_INST_CMD_HVSP_POST_CROP_CONFIG,
    E_MDRV_SCLMULTI_INST_CMD_HVSP_SET_OSD_CONFIG,
    E_MDRV_SCLMULTI_INST_CMD_SCLDMA_IN_BUFFER_CONFIG,
    E_MDRV_SCLMULTI_INST_CMD_SCLDMA_OUT_BUFFER_CONFIG,
    E_MDRV_SCLMULTI_INST_CMD_SCLDMA_IN_TRIGGER_CONFIG,
    E_MDRV_SCLMULTI_INST_CMD_SCLDMA_OUT_TRIGGER_CONFIG,
    E_MDRV_SCLMULTI_INST_CMD_FORCE_RELOAD_CONFIG,
    E_MDRV_SCLMULTI_INST_CMD_MAX,
}MDrvSclMultiInstCmdType_e;


typedef enum
{
    E_MDRV_SCLMULTI_INST_STATUS_FAIL,
    E_MDRV_SCLMULTI_INST_STATUS_SUCCESS,
    E_MDRV_SCLMULTI_INST_STATUS_LOCKED,
}MDrvSclMultiInstStatusType_e;

typedef struct
{
    u32 u32StructSize;
    u32 *pVersion;
    u32 u32VersionSize;
}MDrvSclDmaVersionChkConfig_t;
typedef struct
{
    u32 u32StructSize;
    u32 *pVersion;
    u32 u32VersionSize;
}MDrvSclHvspVersionChkConfig_t;

//-------------------------------------------------------------------------------------------------
//  Structure
//-------------------------------------------------------------------------------------------------

//===============================
//      LOCK multi instance
//===============================
typedef struct
{
    s32 *ps32PrivateID;
    u8 u8IDNum;
}MDrvSclMultiInstLockConfig_t;



//-------------------------------------------------------------------------------------------------
//  Prototype
//-------------------------------------------------------------------------------------------------
#ifndef _MDRV_MULTI_INST_C
#define INTERFACE extern
#else
#define INTERFACE
#endif

INTERFACE bool MDrvSclMultiInstLockInit(MDrvSclMultiInstLockIdType_e enLock_ID);
INTERFACE bool MDrvSclMultiInstLockExit(MDrvSclMultiInstLockIdType_e enLock_ID);
INTERFACE bool MDrvSclMultiInstLockIsFree(MDrvSclMultiInstLockIdType_e enLock_ID, s32 s32PrivateId);
INTERFACE bool MDrvSclMultiInstLockAlloc(MDrvSclMultiInstLockIdType_e enLock_ID, MDrvSclMultiInstLockConfig_t stCfg);
INTERFACE bool MDrvSclMultiInstLockFree(MDrvSclMultiInstLockIdType_e enLock_ID, MDrvSclMultiInstLockConfig_t *pstCfg);

INTERFACE bool MDrvSclMultiInstGetHvspQuantifyPreInstId(void);

INTERFACE bool MDrvSclMultiInstEntryInitVariable(MDrvSclMultiInstEntryIdType_e enID);
INTERFACE bool MDrvSclMultiInstEntryAlloc(MDrvSclMultiInstEntryIdType_e enID, void **pPrivate_Data);
INTERFACE bool MDrvSclMultiInstEntryFree(MDrvSclMultiInstEntryIdType_e enID, void *pPrivate_Data);
INTERFACE bool MDrvSclMultiInstEntryGetPirvateId(MDrvSclMultiInstEntryIdType_e enID, void *pPrivateData, s32 *ps32PrivatId);
INTERFACE MDrvSclMultiInstStatusType_e MDrvSclMultiInstEntryFlashData(MDrvSclMultiInstEntryIdType_e enID, void *pPrivateData, MDrvSclMultiInstCmdType_e enCmd, void *pData);
INTERFACE MDrvSclMultiInstStatusType_e MDrvSclMultiInstEtnryIsFree(MDrvSclMultiInstEntryIdType_e enID, void *pPrivateData);

#undef INTERFACE

#endif
