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
#define _MDRV_MULTI_INST_C
#include "drv_scl_os.h"
#include "drv_scl_dbg.h"
#include "hal_scl_util.h"
#include "drv_scl_verchk.h"

#include "drv_scl_hvsp_st.h"
#include "drv_scl_hvsp.h"
#include "drv_scl_hvsp_m.h"

#include "drv_scl_dma_st.h"
#include "drv_scl_dma.h"
#include "drv_scl_dma_m.h"

#include "drv_scl_multiinst_m_st.h"
#include "drv_scl_multiinst_m.h"

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define MULTIINST_SEM_DBG 0
#define _MULTIINST_SEM_TIMIE_OUT 5000

#define DRV_SC1_MUTEX_LOCK()            DrvSclOsObtainTsem(_LOCK_Mutex[E_MDRV_SCLMULTI_INST_LOCK_ID_SC_1_2])
#define DRV_SC1_MUTEX_UNLOCK()          DrvSclOsReleaseTsem(_LOCK_Mutex[E_MDRV_SCLMULTI_INST_LOCK_ID_SC_1_2])
#define DRV_SC3_MUTEX_LOCK()            DrvSclOsObtainTsem(_LOCK_Mutex[E_MDRV_SCLMULTI_INST_LOCK_ID_SC_3])
#define DRV_SC3_MUTEX_UNLOCK()          DrvSclOsReleaseTsem(_LOCK_Mutex[E_MDRV_SCLMULTI_INST_LOCK_ID_SC_3])
#define DRV_DSP_MUTEX_LOCK()            DrvSclOsObtainTsem(_LOCK_Mutex[E_MDRV_SCLMULTI_INST_LOCK_ID_DISP])
#define DRV_DSP_MUTEX_UNLOCK()          DrvSclOsReleaseTsem(_LOCK_Mutex[E_MDRV_SCLMULTI_INST_LOCK_ID_DISP])
#if MULTIINST_SEM_DBG

#define MULTIINST_WAIT_SEM(_sem)                                               \
        SCL_ERR("+++ [LOCK][%s]_1_[%d] \n", __FUNCTION__, __LINE__);         \
        DrvSclOsObtainMutex(_sem, SCLOS_WAIT_FOREVER);                             \
        SCL_ERR("+++ [LOCK][%s]_2_[%d] \n", __FUNCTION__, __LINE__);


#define MULTIINST_RELEASE_SEM(_sem)                                              \
        SCL_ERR("---  [LOCK][%s] [%d] \n", __FUNCTION__, __LINE__);          \
        DrvSclOsReleaseMutex(_sem);


#else
#define MULTIINST_WAIT_SEM(_sem)     DrvSclOsObtainMutex(_sem,SCLOS_WAIT_FOREVER);

#define MULTIINST_RELEASE_SEM(_sem)  DrvSclOsReleaseMutex(_sem);

#endif //MULTIINST_SEM_DBG


#define MDRV_MULTI_INST_HVSP_PRIVATE_ID_HEADER       0xA00000
#define MDRV_MULTI_INST_SCLDMA_PRIVATE_ID_HEADER     0xD00000
//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------

s32 gMultiInstLockSem[E_MDRV_SCLMULTI_INST_LOCK_ID_MAX];
s32 gMultiInstHvspEntrySem[E_MDRV_SCLMULTI_INST_HVSP_DATA_ID_MAX];
s32 gMultiInstScldmaEntrySem[E_MDRV_SCLMULTI_INST_DMA_DATA_ID_MAX];

MDrvSclMultiInstLockConfig_t gstMultiInstLockCfg[E_MDRV_SCLMULTI_INST_LOCK_ID_MAX];
MDrvSclMultiInstDmaEntryConfig_t gstMultiInstScldmaEntryCfg[E_MDRV_SCLMULTI_INST_DMA_DATA_ID_MAX][MDRV_SCLMULTI_INST_SCLDMA_NUM];
MDrvSclMultiInstHvspEntryConfig_t   gstMultiInstHvspEntryCfg[E_MDRV_SCLMULTI_INST_HVSP_DATA_ID_MAX][MDRV_SCLMULTI_INST_HVSP_NUM];
s32 gs32PreMultiInstScldmaPrivateId[E_MDRV_SCLMULTI_INST_DMA_DATA_ID_MAX];
s32 gs32PreMultiInstHvspPrivateId[E_MDRV_SCLMULTI_INST_HVSP_DATA_ID_MAX];

s32  _LOCK_Mutex[E_MDRV_SCLMULTI_INST_LOCK_ID_MAX]={-1,-1,-1};
bool _LOCK_Mutex_flag[E_MDRV_SCLMULTI_INST_LOCK_ID_MAX]={0,0,0};
//-------------------------------------------------------------------------------------------------
//  Function
//-------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------
//
// Multi Instance Lock
//
//----------------------------------------------------------------------------------------------------------------

u8 _MDrv_MultiInst_Lock_Get_PrivateID_Num(MDrvSclMultiInstLockIdType_e enLock_ID)
{
    u8 u8Num;

    switch(enLock_ID)
    {
        case E_MDRV_SCLMULTI_INST_LOCK_ID_SC_1_2:
            u8Num = 4;
            break;
        case E_MDRV_SCLMULTI_INST_LOCK_ID_SC_3:
            u8Num = 2;
            break;
        case E_MDRV_SCLMULTI_INST_LOCK_ID_DISP:
            u8Num = 1;
            break;
        default:
            u8Num = 0;
            break;
    }

    return u8Num;
}


bool MDrvSclMultiInstLockInit(MDrvSclMultiInstLockIdType_e enLock_ID)
{
    bool bRet = 1;
    u8 i;
    u8 u8PrivateIDNum = 0;
    char word[]     = {"_SC1_Mutex"};
    char word2[]    = {"_SC3_Mutex"};
    char word3[]    = {"_DSP_Mutex"};
    SCL_DBG(SCL_DBG_LV_MULTI_INST_FUNC()&(Get_DBGMG_MULTI(enLock_ID)), "[MULTIINST]%s(%d)\n", __FUNCTION__, __LINE__);

    u8PrivateIDNum = _MDrv_MultiInst_Lock_Get_PrivateID_Num(enLock_ID);
    if(_LOCK_Mutex[E_MDRV_SCLMULTI_INST_LOCK_ID_SC_1_2] == -1)
    {
        _LOCK_Mutex[E_MDRV_SCLMULTI_INST_LOCK_ID_SC_1_2] = DrvSclOsTsemInit(1);
    }
    if(_LOCK_Mutex[E_MDRV_SCLMULTI_INST_LOCK_ID_SC_3] == -1)
    {
        _LOCK_Mutex[E_MDRV_SCLMULTI_INST_LOCK_ID_SC_3] = DrvSclOsTsemInit( 1);
    }
    if(_LOCK_Mutex[E_MDRV_SCLMULTI_INST_LOCK_ID_DISP] == -1)
    {
        _LOCK_Mutex[E_MDRV_SCLMULTI_INST_LOCK_ID_DISP] = DrvSclOsTsemInit(1);
    }
    if(gstMultiInstLockCfg[enLock_ID].ps32PrivateID == NULL)
    {
        gstMultiInstLockCfg[enLock_ID].u8IDNum = u8PrivateIDNum;
        gstMultiInstLockCfg[enLock_ID].ps32PrivateID = DrvSclOsMemalloc(sizeof(s32)*u8PrivateIDNum, GFP_KERNEL);
        if(gstMultiInstLockCfg[enLock_ID].ps32PrivateID)
        {
            for(i=0; i<u8PrivateIDNum; i++)
            {
                gstMultiInstLockCfg[enLock_ID].ps32PrivateID[i] = -1;
            }
            if(enLock_ID == E_MDRV_SCLMULTI_INST_LOCK_ID_SC_1_2)
            {
                gMultiInstLockSem[enLock_ID] = DrvSclOsCreateMutex(E_DRV_SCLOS_FIFO, word, SCLOS_PROCESS_SHARED);
            }
            else if(enLock_ID == E_MDRV_SCLMULTI_INST_LOCK_ID_SC_3)
            {
                gMultiInstLockSem[enLock_ID] = DrvSclOsCreateMutex(E_DRV_SCLOS_FIFO, word2, SCLOS_PROCESS_SHARED);
            }
            else if(enLock_ID == E_MDRV_SCLMULTI_INST_LOCK_ID_DISP)
            {
                gMultiInstLockSem[enLock_ID] = DrvSclOsCreateMutex(E_DRV_SCLOS_FIFO, word3, SCLOS_PROCESS_SHARED);
            }
            bRet = 1;
        }
        else
        {

            SCL_DBG(SCL_DBG_LV_MULTI_INST_ERR, "[MULTIINST]%s(%d) LockInit fail: LockId=%d\n", __FUNCTION__, __LINE__, enLock_ID);
            bRet = 0;
        }
    }

    return bRet;
}

bool MDrvSclMultiInstLockExit(MDrvSclMultiInstLockIdType_e enLock_ID)
{
    SCL_DBG(SCL_DBG_LV_MULTI_INST_FUNC()&(Get_DBGMG_MULTI(enLock_ID)), "[MULTIINST]%s(%d)\n", __FUNCTION__, __LINE__);

    if(gstMultiInstLockCfg[enLock_ID].ps32PrivateID)
    {
        DrvSclOsMemFree(gstMultiInstLockCfg[enLock_ID].ps32PrivateID);
    }
    else
    {
        SCL_DBG(SCL_DBG_LV_MULTI_INST_ERR, "[MULTIINST]%s(%d) LockExit fail : LockId:%d\n", __FUNCTION__, __LINE__, enLock_ID);
    }
    if(_LOCK_Mutex[E_MDRV_SCLMULTI_INST_LOCK_ID_SC_1_2] != -1)
    {
        _LOCK_Mutex[E_MDRV_SCLMULTI_INST_LOCK_ID_SC_1_2] = DrvSclOsTsemDeinit(_LOCK_Mutex[E_MDRV_SCLMULTI_INST_LOCK_ID_SC_1_2]);
    }
    if(_LOCK_Mutex[E_MDRV_SCLMULTI_INST_LOCK_ID_SC_3] != -1)
    {
        _LOCK_Mutex[E_MDRV_SCLMULTI_INST_LOCK_ID_SC_3] = DrvSclOsTsemDeinit(_LOCK_Mutex[E_MDRV_SCLMULTI_INST_LOCK_ID_SC_3]);
    }
    if(_LOCK_Mutex[E_MDRV_SCLMULTI_INST_LOCK_ID_DISP] != -1)
    {
        _LOCK_Mutex[E_MDRV_SCLMULTI_INST_LOCK_ID_DISP] = DrvSclOsTsemDeinit(_LOCK_Mutex[E_MDRV_SCLMULTI_INST_LOCK_ID_DISP]);
    }

    return 1;
}


bool MDrvSclMultiInstLockIsFree(MDrvSclMultiInstLockIdType_e enLock_ID, s32 s32PrivateId)
{
    u8 i, bIsFree;
    bool bEmptyLock;
    u8 u8PrivateIDNum = _MDrv_MultiInst_Lock_Get_PrivateID_Num(enLock_ID);

    SCL_DBG(SCL_DBG_LV_MULTI_INST_FUNC()&(Get_DBGMG_MULTI(enLock_ID)), "[MULTIINST]%s(%d)\n", __FUNCTION__, __LINE__);

    MULTIINST_WAIT_SEM(gMultiInstLockSem[enLock_ID]);

    bEmptyLock = 1;
    for(i=0;i<u8PrivateIDNum;i++)
    {
        if(gstMultiInstLockCfg[enLock_ID].ps32PrivateID[i] != -1)
        {
            bEmptyLock = 0;
        }
    }

    if(bEmptyLock == 0)
    {
        bIsFree = 0;
        for(i=0;i<u8PrivateIDNum;i++)
        {
            if(gstMultiInstLockCfg[enLock_ID].ps32PrivateID[i] == s32PrivateId)
            {
                bIsFree = 1;
                SCL_DBG(SCL_DBG_LV_MULTI_INST_LOCK()&(Get_DBGMG_MULTI(enLock_ID)), "[MULTIINST]%s(%d) LockID:%d, idx:%d PirvateID= %lx \n",
                    __FUNCTION__, __LINE__, enLock_ID, i, s32PrivateId);
                break;
            }
        }
    }
    else
    {
        bIsFree = 1;
    }
    SCL_DBG(SCL_DBG_LV_MULTI_INST_LOCK()&(Get_DBGMG_MULTI(enLock_ID)), "[MULTIINST]%s(%d),LockID:%d IsFree:%d, PrivateId:%lx\n",
        __FUNCTION__, __LINE__,
        enLock_ID, bIsFree, s32PrivateId);

    MULTIINST_RELEASE_SEM(gMultiInstLockSem[enLock_ID]);
    return bIsFree;

}

bool MDrvSclMultiInstLockAlloc(MDrvSclMultiInstLockIdType_e enLock_ID, MDrvSclMultiInstLockConfig_t stCfg)
{
    u8 i, bRet,bFree = 1;
    u8 u8PrivateIDNum = _MDrv_MultiInst_Lock_Get_PrivateID_Num(enLock_ID);

    SCL_DBG(SCL_DBG_LV_MULTI_INST_FUNC()&(Get_DBGMG_MULTI(enLock_ID)), "[MULTIINST]%s(%d)Id:%ld\n", __FUNCTION__, __LINE__,stCfg.ps32PrivateID[0]);
    if(stCfg.u8IDNum != u8PrivateIDNum)
    {
        SCL_DBG(SCL_DBG_LV_MULTI_INST_ERR, "[MULTIINST]%s(%d) LockAlloc fail LockID:%d, %d!=%d\n",
            __FUNCTION__, __LINE__, enLock_ID, stCfg.u8IDNum, u8PrivateIDNum);

        return 0;
    }
    if(enLock_ID == E_MDRV_SCLMULTI_INST_LOCK_ID_SC_1_2)
    {
        DRV_SC1_MUTEX_LOCK();
        MULTIINST_WAIT_SEM(gMultiInstLockSem[enLock_ID]);
        _LOCK_Mutex_flag[E_MDRV_SCLMULTI_INST_LOCK_ID_SC_1_2] = 1;
        MULTIINST_RELEASE_SEM(gMultiInstLockSem[enLock_ID]);
    }
    else if(enLock_ID == E_MDRV_SCLMULTI_INST_LOCK_ID_SC_3)
    {
        DRV_SC3_MUTEX_LOCK();
        MULTIINST_WAIT_SEM(gMultiInstLockSem[enLock_ID]);
        _LOCK_Mutex_flag[E_MDRV_SCLMULTI_INST_LOCK_ID_SC_3] = 1;
        MULTIINST_RELEASE_SEM(gMultiInstLockSem[enLock_ID]);
        SCL_DBG(SCL_DBG_LV_MULTI_INST_FUNC()&(Get_DBGMG_MULTI(enLock_ID)), "[MULTIINST]LOCK !! Id:%ld\n",stCfg.ps32PrivateID[0]);
    }
    else if(enLock_ID == E_MDRV_SCLMULTI_INST_LOCK_ID_DISP)
    {
        DRV_DSP_MUTEX_LOCK();
        MULTIINST_WAIT_SEM(gMultiInstLockSem[enLock_ID]);
        _LOCK_Mutex_flag[E_MDRV_SCLMULTI_INST_LOCK_ID_DISP] = 1;
        MULTIINST_RELEASE_SEM(gMultiInstLockSem[enLock_ID]);
    }


    MULTIINST_WAIT_SEM(gMultiInstLockSem[enLock_ID]);

    for(i=0; i<u8PrivateIDNum; i++)
    {
        if(gstMultiInstLockCfg[enLock_ID].ps32PrivateID[i] != -1)
        {
            bFree = 0;
            break;
        }
    }

    if( bFree )
    {
        for(i=0;i<u8PrivateIDNum;i++)
        {
            SCL_DBG(SCL_DBG_LV_MULTI_INST_LOCK()&(Get_DBGMG_MULTI(enLock_ID)), "[MULTIINST]%s(%d): LockID:%d, idx:%d, PirvateId:%lx\n",
                __FUNCTION__, __LINE__, enLock_ID, i, stCfg.ps32PrivateID[i]);

            gstMultiInstLockCfg[enLock_ID].ps32PrivateID[i] = stCfg.ps32PrivateID[i];
        }

        bRet = 1;
    }
    else
    {

    #if SCL_DBG_LV_MULTI_INST_LOCK_LOG
        for(i=0;i<u8PrivateIDNum;i++)
        {
            SCL_DBG(SCL_DBG_LV_MULTI_INST_LOCK()&(Get_DBGMG_MULTI(enLock_ID)), "[MULTIINST]%s(%d) LockAlloc fail: idx:%d, PrivateId:%lx\n",
                __FUNCTION__, __LINE__, i, stCfg.ps32PrivateID[i]);
        }
    #endif
        bRet = 0;
    }

    MULTIINST_RELEASE_SEM(gMultiInstLockSem[enLock_ID]);
    return bRet;

}

//----------------------------------------------------------------------------------------------------------------
//
// Multi Instance Data
//
//----------------------------------------------------------------------------------------------------------------
bool MDrvSclMultiInstLockFree(MDrvSclMultiInstLockIdType_e enLock_ID, MDrvSclMultiInstLockConfig_t *pstCfg)
{
    u8 i;
    bool bRet = 1;
    u8 u8PrivateIDNum = _MDrv_MultiInst_Lock_Get_PrivateID_Num(enLock_ID);

    SCL_DBG(SCL_DBG_LV_MULTI_INST_FUNC()&(Get_DBGMG_MULTI(enLock_ID)), "[MULTIINST]%s(%d)\n", __FUNCTION__, __LINE__);

    MULTIINST_WAIT_SEM(gMultiInstLockSem[enLock_ID]);

    if(pstCfg)
    {
        for(i=0; i<u8PrivateIDNum; i++)
        {
            if(gstMultiInstLockCfg[enLock_ID].ps32PrivateID[i] != pstCfg->ps32PrivateID[i])
            {
                bRet = 0;
                break;
            }
        }
    }

    if(bRet)
    {
        for(i=0;i<u8PrivateIDNum; i++)
        {
        #if SCL_DBG_LV_MULTI_INST_LOCK_LOG
            SCL_DBG(SCL_DBG_LV_MULTI_INST_LOCK()&(Get_DBGMG_MULTI(enLock_ID)), "[MULTIINST]%s(%d): idx=%d, PrivateId:%lx\n", __FUNCTION__, __LINE__, i, gstMultiInstLockCfg[enLock_ID].ps32PrivateID[i]);
        #endif
            gstMultiInstLockCfg[enLock_ID].ps32PrivateID[i] = -1;
        }
    }
    else
    {
        SCL_DBG(SCL_DBG_LV_MULTI_INST_LOCK()&(Get_DBGMG_MULTI(enLock_ID)), "[MULTIINST]%s(%d) LockFree fail LockID:%d\n", __FUNCTION__, __LINE__, enLock_ID);

    }

    if(enLock_ID == E_MDRV_SCLMULTI_INST_LOCK_ID_SC_1_2 && _LOCK_Mutex_flag[E_MDRV_SCLMULTI_INST_LOCK_ID_SC_1_2])
    {
        _LOCK_Mutex_flag[E_MDRV_SCLMULTI_INST_LOCK_ID_SC_1_2]=0;
        DRV_SC1_MUTEX_UNLOCK();
        MULTIINST_RELEASE_SEM(gMultiInstLockSem[enLock_ID]);
    }
    else if(enLock_ID == E_MDRV_SCLMULTI_INST_LOCK_ID_SC_3 && _LOCK_Mutex_flag[E_MDRV_SCLMULTI_INST_LOCK_ID_SC_3])
    {
        _LOCK_Mutex_flag[E_MDRV_SCLMULTI_INST_LOCK_ID_SC_3]=0;
        DRV_SC3_MUTEX_UNLOCK();
        MULTIINST_RELEASE_SEM(gMultiInstLockSem[enLock_ID]);
    }
    else if(enLock_ID == E_MDRV_SCLMULTI_INST_LOCK_ID_DISP && _LOCK_Mutex_flag[E_MDRV_SCLMULTI_INST_LOCK_ID_DISP])
    {
        _LOCK_Mutex_flag[E_MDRV_SCLMULTI_INST_LOCK_ID_DISP]=0;
        DRV_DSP_MUTEX_UNLOCK();
        MULTIINST_RELEASE_SEM(gMultiInstLockSem[enLock_ID]);
    }
    else
    {
        MULTIINST_RELEASE_SEM(gMultiInstLockSem[enLock_ID]);
    }
    return bRet;
}

bool _MDrvSclMultiInstTransHvspId(MDrvSclMultiInstEntryIdType_e enID, MDrvSclMultiInstHvspDataIdType_e *pHvspID)
{
    bool bRet = 1;
    switch(enID)
    {
        case E_MDRV_SCLMULTI_INST_ENTRY_ID_HVSP1:
            *pHvspID = E_MDRV_SCLMULTI_INST_HVSP_DATA_ID_1;
            break;
        case E_MDRV_SCLMULTI_INST_ENTRY_ID_HVSP2:
            *pHvspID = E_MDRV_SCLMULTI_INST_HVSP_DATA_ID_2;
            break;

        case E_MDRV_SCLMULTI_INST_ENTRY_ID_HVSP3:
            *pHvspID = E_MDRV_SCLMULTI_INST_HVSP_DATA_ID_3;
            break;

        default:
            *pHvspID = E_MDRV_SCLMULTI_INST_HVSP_DATA_ID_MAX;
            bRet = 0;
            break;
    }
    return bRet;
}

bool _MDrvSclMultiInstTransDmaId(MDrvSclMultiInstEntryIdType_e enID, MDrvSclMultiInstDmaDataIdType_e *pScldmaID)
{
    bool bRet = 1;
    switch(enID)
    {
        case E_MDRV_SCLMULTI_INST_ENTRY_ID_SCLDMA1:
            *pScldmaID = E_MDRV_SCLMULTI_INST_DMA_DATA_ID_1;
            break;
        case E_MDRV_SCLMULTI_INST_ENTRY_ID_SCLDMA2:
            *pScldmaID = E_MDRV_SCLMULTI_INST_DMA_DATA_ID_2;
            break;

        case E_MDRV_SCLMULTI_INST_ENTRY_ID_SCLDMA3:
            *pScldmaID = E_MDRV_SCLMULTI_INST_DMA_DATA_ID_3;
            break;

        case E_MDRV_SCLMULTI_INST_ENTRY_ID_DISP:
            *pScldmaID = E_MDRV_SCLMULTI_INST_DMA_DATA_ID_4;
            break;

        default:
            *pScldmaID = E_MDRV_SCLMULTI_INST_DMA_DATA_ID_MAX;
            bRet = 0;
            break;
    }
    return bRet;
}


bool _MDrvSclMultiInstEntrySaveHvspData(
    MDrvSclMultiInstHvspDataIdType_e enID,
    MDrvSclMultiInstHvspEntryConfig_t *pEntry,
    MDrvSclMultiInstCmdType_e enCmd,
    void *pData)
{
    bool bRet = 1;

    if(pEntry)
    {
        MULTIINST_WAIT_SEM(gMultiInstHvspEntrySem[enID]);

        if(enCmd == E_MDRV_SCLMULTI_INST_CMD_HVSP_IN_CONFIG)
        {
            pEntry->stData.stFlag.bInCfg = 1;
            DrvSclOsMemcpy(&pEntry->stData.stInCfg, (MDrvSclHvspInputConfig_t *)pData, sizeof(MDrvSclHvspInputConfig_t));
        }
        else if(enCmd == E_MDRV_SCLMULTI_INST_CMD_HVSP_SCALING_CONFIG)
        {
            pEntry->stData.stFlag.bScaleCfg = 1;
            DrvSclOsMemcpy(&pEntry->stData.stScaleCfg, (MDrvSclHvspScalingConfig_t *)pData, sizeof(MDrvSclHvspScalingConfig_t));
        }
        else if(enCmd == E_MDRV_SCLMULTI_INST_CMD_HVSP_POST_CROP_CONFIG)
        {
            pEntry->stData.stFlag.bPostCropCfg = 1;
            DrvSclOsMemcpy(&pEntry->stData.stPostCropCfg, (MDrvSclHvspPostCropConfig_t *)pData, sizeof(MDrvSclHvspPostCropConfig_t));
        }
        else if(enCmd == E_MDRV_SCLMULTI_INST_CMD_HVSP_MEM_REQ_CONFIG)
        {
            pEntry->stData.stFlag.bMemCfg = 1;
            DrvSclOsMemcpy(&pEntry->stData.stMemCfg, (MDrvSclHvspIpmConfig_t *)pData, sizeof(MDrvSclHvspIpmConfig_t));
        }
        else if(enCmd == E_MDRV_SCLMULTI_INST_CMD_HVSP_SET_OSD_CONFIG)
        {
            pEntry->stData.stFlag.bOSDCfg= 1;
            DrvSclOsMemcpy(&pEntry->stData.stOSDCfg, (MDrvSclHvspOsdConfig_t*)pData, sizeof(MDrvSclHvspOsdConfig_t));
        }

        SCL_DBG(SCL_DBG_LV_MULTI_INST_ENTRY()&(Get_DBGMG_MULTI(enID)), "[MULTIINST]%s(%d): PrivateId:%lx, bMem:%d, bIn:%d, bScale:%d\n",
            __FUNCTION__, __LINE__,
            pEntry->s32PivateId, pEntry->stData.stFlag.bMemCfg, pEntry->stData.stFlag.bInCfg,
            pEntry->stData.stFlag.bScaleCfg);

        MULTIINST_RELEASE_SEM(gMultiInstHvspEntrySem[enID]);
        bRet = 1;
    }
    else
    {
        bRet = 0;
        SCL_DBG(SCL_DBG_LV_MULTI_INST_ERR, "[MULTIINST]%s(%d): HvspSaveData fail: PrivateId=%lx\n", __FUNCTION__, __LINE__, pEntry->s32PivateId);
    }

    return bRet;
}

bool _MDrvSclMultiInstEntrySaveDmaData(
    MDrvSclMultiInstDmaDataIdType_e enID,
    MDrvSclMultiInstDmaEntryConfig_t *pEntry,
    MDrvSclMultiInstCmdType_e enCmd,
    void *pData)
{
    bool bRet = 1;
    u8 index = 0;
    if(pEntry)
    {

        MULTIINST_WAIT_SEM(gMultiInstScldmaEntrySem[enID]);
        if(enCmd == E_MDRV_SCLMULTI_INST_CMD_SCLDMA_IN_BUFFER_CONFIG)
        {
            pEntry->stData.stFlag.bInBufCfg = 1;
            DrvSclOsMemcpy(&pEntry->stData.stInBufCfg, (MDrvSclDmaBufferConfig_t *)pData, sizeof(MDrvSclDmaBufferConfig_t));
        }
        else if(enCmd == E_MDRV_SCLMULTI_INST_CMD_SCLDMA_OUT_BUFFER_CONFIG)
        {
            MDrvSclDmaBufferConfig_t stCfg;
            DrvSclOsMemcpy(&stCfg, (MDrvSclDmaBufferConfig_t *)pData, sizeof(MDrvSclDmaBufferConfig_t));
            index = stCfg.enMemType;
            if(stCfg.enMemType == E_MDRV_SCLDMA_MEM_FRM)
            {
                pEntry->stData.stFlag.bOutBufCfg[E_MDRV_SCLDMA_MEM_FRM]= 1;
                DrvSclOsMemcpy(&pEntry->stData.stOutBufCfg[E_MDRV_SCLDMA_MEM_FRM], (MDrvSclDmaBufferConfig_t *)pData, sizeof(MDrvSclDmaBufferConfig_t));
            }
            else if(stCfg.enMemType == E_MDRV_SCLDMA_MEM_SNP)
            {
                pEntry->stData.stFlag.bOutBufCfg[E_MDRV_SCLDMA_MEM_SNP]= 1;
                DrvSclOsMemcpy(&pEntry->stData.stOutBufCfg[E_MDRV_SCLDMA_MEM_SNP], (MDrvSclDmaBufferConfig_t *)pData, sizeof(MDrvSclDmaBufferConfig_t));
            }
            else if(stCfg.enMemType == E_MDRV_SCLDMA_MEM_IMI)
            {
                pEntry->stData.stFlag.bOutBufCfg[E_MDRV_SCLDMA_MEM_IMI]= 1;
                DrvSclOsMemcpy(&pEntry->stData.stOutBufCfg[E_MDRV_SCLDMA_MEM_IMI], (MDrvSclDmaBufferConfig_t *)pData, sizeof(MDrvSclDmaBufferConfig_t));
            }
        }
        else if(enCmd == E_MDRV_SCLMULTI_INST_CMD_SCLDMA_IN_TRIGGER_CONFIG)
        {
            pEntry->stData.stFlag.bInTrigCfg = 1;
            DrvSclOsMemcpy(&pEntry->stData.stInTrigCfg, (MDrvSclDmaTriggerConfig_t *)pData, sizeof(MDrvSclDmaTriggerConfig_t));
        }
        else if(enCmd == E_MDRV_SCLMULTI_INST_CMD_SCLDMA_OUT_TRIGGER_CONFIG)
        {
            MDrvSclDmaTriggerConfig_t stCfg;
            DrvSclOsMemcpy(&stCfg, (MDrvSclDmaTriggerConfig_t *)pData, sizeof(MDrvSclDmaTriggerConfig_t));
            index = stCfg.enMemType;
            if(stCfg.enMemType == E_MDRV_SCLDMA_MEM_FRM)
            {
                pEntry->stData.stFlag.bOutTrigCfg[E_MDRV_SCLDMA_MEM_FRM]= 1;
                DrvSclOsMemcpy(&pEntry->stData.stOutTrigCfg[E_MDRV_SCLDMA_MEM_FRM], (MDrvSclDmaTriggerConfig_t *)pData, sizeof(MDrvSclDmaTriggerConfig_t));
            }
            else if(stCfg.enMemType == E_MDRV_SCLDMA_MEM_SNP)
            {
                pEntry->stData.stFlag.bOutTrigCfg[E_MDRV_SCLDMA_MEM_SNP]= 1;
                DrvSclOsMemcpy(&pEntry->stData.stOutTrigCfg[E_MDRV_SCLDMA_MEM_SNP], (MDrvSclDmaTriggerConfig_t *)pData, sizeof(MDrvSclDmaTriggerConfig_t));
            }
            else if(stCfg.enMemType == E_MDRV_SCLDMA_MEM_IMI)
            {
                pEntry->stData.stFlag.bOutTrigCfg[E_MDRV_SCLDMA_MEM_IMI]= 1;
                DrvSclOsMemcpy(&pEntry->stData.stOutTrigCfg[E_MDRV_SCLDMA_MEM_IMI], (MDrvSclDmaTriggerConfig_t *)pData, sizeof(MDrvSclDmaTriggerConfig_t));
            }
        }

        SCL_DBG(SCL_DBG_LV_MULTI_INST_ENTRY()&(Get_DBGMG_MULTI(enID)), "[MULTIINST]%s(%d): PrivateId:%lx, bInBuf:%d, bOutBuf[%hhd]:%d\n",
            __FUNCTION__, __LINE__,
            pEntry->s32PivateId, pEntry->stData.stFlag.bInBufCfg, index,pEntry->stData.stFlag.bOutBufCfg[index]);

        MULTIINST_RELEASE_SEM(gMultiInstScldmaEntrySem[enID]);
        bRet = 1;
    }
    else
    {
        bRet = 0;
        SCL_DBG(SCL_DBG_LV_MULTI_INST_ERR, "[MULTIINST]%s(%d): ScldmaSaveData fail: PrivateId:%lx\n", __FUNCTION__, __LINE__, pEntry->s32PivateId);
    }

    return bRet;
}

bool _MDrvSclMultiInstEntryReloadHvspData(
    MDrvSclMultiInstHvspDataIdType_e enID,
    MDrvSclMultiInstHvspEntryConfig_t *pEntry,
    MDrvSclMultiInstCmdType_e enCmd)
{
    bool bRet = 1;
    MDrvSclMultiInstHvspFlagType_t stFlag;
    MDrvSclHvspIdType_e enHvspId = enID == E_MDRV_SCLMULTI_INST_HVSP_DATA_ID_1 ? E_MDRV_SCLHVSP_ID_1 :
                                    enID == E_MDRV_SCLMULTI_INST_HVSP_DATA_ID_2 ? E_MDRV_SCLHVSP_ID_2 :
                                    enID == E_MDRV_SCLMULTI_INST_HVSP_DATA_ID_3 ? E_MDRV_SCLHVSP_ID_3 :
                                                                               E_MDRV_SCLHVSP_ID_MAX ;
    DrvSclOsMemset(&stFlag, 0, sizeof(MDrvSclMultiInstHvspFlagType_t));

    MULTIINST_WAIT_SEM(gMultiInstHvspEntrySem[enID]);

    if(pEntry)
    {
        if(enCmd == E_MDRV_SCLMULTI_INST_CMD_FORCE_RELOAD_CONFIG)
        {
            //Reload
            stFlag.bInCfg           = pEntry->stData.stFlag.bInCfg;
            stFlag.bScaleCfg        = pEntry->stData.stFlag.bScaleCfg;
            stFlag.bPostCropCfg     = pEntry->stData.stFlag.bPostCropCfg;
            stFlag.bMemCfg          = pEntry->stData.stFlag.bMemCfg;
            stFlag.bOSDCfg          = pEntry->stData.stFlag.bOSDCfg;
        }
        else if(pEntry->s32PivateId != gs32PreMultiInstHvspPrivateId[enID] &&
           gs32PreMultiInstHvspPrivateId[enID] != -1)
        {
            //if not the same fd,reload others setting.(ioctl will set itself,so just set others configuration)
            if(enCmd == E_MDRV_SCLMULTI_INST_CMD_HVSP_IN_CONFIG)
            {
                stFlag.bScaleCfg        = pEntry->stData.stFlag.bScaleCfg;
                stFlag.bPostCropCfg     = pEntry->stData.stFlag.bPostCropCfg;
                stFlag.bMemCfg          = pEntry->stData.stFlag.bMemCfg;
                stFlag.bOSDCfg          = pEntry->stData.stFlag.bOSDCfg;
            }
            else if(enCmd == E_MDRV_SCLMULTI_INST_CMD_HVSP_SCALING_CONFIG)
            {
                stFlag.bInCfg           = pEntry->stData.stFlag.bInCfg;
                stFlag.bPostCropCfg     = pEntry->stData.stFlag.bPostCropCfg;
                stFlag.bMemCfg          = pEntry->stData.stFlag.bMemCfg;
                stFlag.bOSDCfg          = pEntry->stData.stFlag.bOSDCfg;
            }
            else if(enCmd == E_MDRV_SCLMULTI_INST_CMD_HVSP_POST_CROP_CONFIG)
            {
                stFlag.bInCfg           = pEntry->stData.stFlag.bInCfg;
                stFlag.bScaleCfg        = pEntry->stData.stFlag.bScaleCfg;
                stFlag.bMemCfg          = pEntry->stData.stFlag.bMemCfg;
                stFlag.bOSDCfg          = pEntry->stData.stFlag.bOSDCfg;
            }
            else if(enCmd == E_MDRV_SCLMULTI_INST_CMD_HVSP_SET_OSD_CONFIG)
            {
                stFlag.bInCfg           = pEntry->stData.stFlag.bInCfg;
                stFlag.bScaleCfg        = pEntry->stData.stFlag.bScaleCfg;
                stFlag.bPostCropCfg     = pEntry->stData.stFlag.bPostCropCfg;
                stFlag.bMemCfg          = pEntry->stData.stFlag.bMemCfg;
            }
        }
        else
        {
            SCL_DBG(SCL_DBG_LV_MULTI_INST_ENTRY()&(Get_DBGMG_MULTI(enID)), "[MULTIINST]%s(%d)::The first time\n", __FUNCTION__, __LINE__);
        }

        SCL_DBG(SCL_DBG_LV_MULTI_INST_ENTRY()&(Get_DBGMG_MULTI(enID)), "[MULTIINST]%s(%d)::PrivateId:%lx, bMem:%d, bIn:%d, bScale:%d,bPostCrop:%d\n",
            __FUNCTION__, __LINE__,
            pEntry->s32PivateId, stFlag.bMemCfg, stFlag.bInCfg,
            stFlag.bScaleCfg, stFlag.bPostCropCfg);

        bRet = 1;

        if(stFlag.bMemCfg)
        {
            DrvSclHvspCmdTrigConfig_t stHvspCmdTrigCfg;
            stHvspCmdTrigCfg = DrvSclHvspSetCmdqTrigTypeByRIU();

            bRet &= MDrvSclHvspSetInitIpmConfig(enHvspId, &pEntry->stData.stMemCfg);

            DrvSclHvspSetCmdqTrigType(stHvspCmdTrigCfg);
        }

        if(stFlag.bInCfg)
        {
            bRet &= MDrvSclHvspSetInputConfig(enHvspId, &pEntry->stData.stInCfg);
        }


        if(stFlag.bScaleCfg)
        {
            bRet &= MDrvSclHvspSetScalingConfig(enHvspId, &pEntry->stData.stScaleCfg);
        }

        if(stFlag.bPostCropCfg)
        {
            bRet &= MDrvSclHvspSetPostCropConfig(enHvspId, &pEntry->stData.stPostCropCfg);
        }

        if(stFlag.bOSDCfg)
        {
            bRet &= MDrvSclHvspSetOsdConfig(enHvspId, &pEntry->stData.stOSDCfg);
        }

        gs32PreMultiInstHvspPrivateId[enID] = pEntry->s32PivateId;
    }
    else
    {
        bRet = 0;
        SCL_DBG(SCL_DBG_LV_MULTI_INST_ERR, "[MULTIINST]%s(%d):: ReloadHvsp Fail, hvspId:%d\n",
            __FUNCTION__, __LINE__, enID);
    }

    MULTIINST_RELEASE_SEM(gMultiInstHvspEntrySem[enID]);

    return bRet;
}


bool _MDrvSclMultiInstEntryReloadDmaData(
    MDrvSclMultiInstDmaDataIdType_e enID,
    MDrvSclMultiInstDmaEntryConfig_t *pEntry,
    MDrvSclMultiInstCmdType_e enCmd)
{
    bool bRet;
    MDrvSclMultiInstDmaFlagType_t stFlag;
    MDrvSclDmaIdType_e enScldmaId = enID == E_MDRV_SCLMULTI_INST_DMA_DATA_ID_1 ? E_MDRV_SCLDMA_ID_1 :
                                        enID == E_MDRV_SCLMULTI_INST_DMA_DATA_ID_2 ? E_MDRV_SCLDMA_ID_2 :
                                        enID == E_MDRV_SCLMULTI_INST_DMA_DATA_ID_3 ? E_MDRV_SCLDMA_ID_3 :
                                        enID == E_MDRV_SCLMULTI_INST_DMA_DATA_ID_4 ? E_MDRV_SCLDMA_ID_PNL :
                                                                                     E_MDRV_SCLHVSP_ID_MAX ;
    DrvSclOsMemset(&stFlag, 0, sizeof(MDrvSclMultiInstDmaFlagType_t));

    MULTIINST_WAIT_SEM(gMultiInstScldmaEntrySem[enID]);

    if(pEntry)
    {
        if(enCmd == E_MDRV_SCLMULTI_INST_CMD_FORCE_RELOAD_CONFIG)
        {
            stFlag.bOutBufCfg[E_MDRV_SCLDMA_MEM_FRM]  = pEntry->stData.stFlag.bOutBufCfg[E_MDRV_SCLDMA_MEM_FRM];
            stFlag.bOutBufCfg[E_MDRV_SCLDMA_MEM_SNP]  = pEntry->stData.stFlag.bOutBufCfg[E_MDRV_SCLDMA_MEM_SNP];
            stFlag.bOutBufCfg[E_MDRV_SCLDMA_MEM_IMI]  = pEntry->stData.stFlag.bOutBufCfg[E_MDRV_SCLDMA_MEM_IMI];
            stFlag.bInBufCfg   = pEntry->stData.stFlag.bInBufCfg;
            stFlag.bInTrigCfg  = pEntry->stData.stFlag.bInTrigCfg;
            if(pEntry->stData.stOutBufCfg[E_MDRV_SCLDMA_MEM_FRM].enBufMDType==E_MDRV_SCLDMA_BUFFER_MD_RING
                &&(enScldmaId==E_MDRV_SCLDMA_ID_1 || enScldmaId==E_MDRV_SCLDMA_ID_2))
            {
                stFlag.bOutTrigCfg[E_MDRV_SCLDMA_MEM_FRM] = 0;
            }
            else
            {
                stFlag.bOutTrigCfg[E_MDRV_SCLDMA_MEM_FRM] =  pEntry->stData.stFlag.bOutTrigCfg[E_MDRV_SCLDMA_MEM_FRM];
            }
            stFlag.bOutTrigCfg[E_MDRV_SCLDMA_MEM_SNP] = pEntry->stData.stFlag.bOutTrigCfg[E_MDRV_SCLDMA_MEM_SNP];
            stFlag.bOutTrigCfg[E_MDRV_SCLDMA_MEM_IMI] = pEntry->stData.stFlag.bOutTrigCfg[E_MDRV_SCLDMA_MEM_IMI];
        }
        else if(pEntry->s32PivateId != gs32PreMultiInstScldmaPrivateId[enID] &&
                gs32PreMultiInstScldmaPrivateId[enID] != -1)
        {
            if(enCmd == E_MDRV_SCLMULTI_INST_CMD_SCLDMA_IN_BUFFER_CONFIG)
            {
                //for Sc3 ,   Sc1,Sc2 ignore
                stFlag.bOutBufCfg[E_MDRV_SCLDMA_MEM_FRM] = pEntry->stData.stFlag.bOutBufCfg[E_MDRV_SCLDMA_MEM_FRM];
                stFlag.bOutBufCfg[E_MDRV_SCLDMA_MEM_SNP] = pEntry->stData.stFlag.bOutBufCfg[E_MDRV_SCLDMA_MEM_SNP];
                stFlag.bOutBufCfg[E_MDRV_SCLDMA_MEM_IMI] = pEntry->stData.stFlag.bOutBufCfg[E_MDRV_SCLDMA_MEM_IMI];
            }
            else if(enCmd == E_MDRV_SCLMULTI_INST_CMD_SCLDMA_OUT_BUFFER_CONFIG)
            {
                //for Sc3 ,  Sc1,Sc2 ignore
                stFlag.bInBufCfg  = pEntry->stData.stFlag.bInBufCfg;
            }
            else if(enCmd == E_MDRV_SCLMULTI_INST_CMD_SCLDMA_IN_TRIGGER_CONFIG ||
                    enCmd == E_MDRV_SCLMULTI_INST_CMD_SCLDMA_OUT_TRIGGER_CONFIG
            )
            {
                stFlag.bInBufCfg  = pEntry->stData.stFlag.bInBufCfg;
                stFlag.bOutBufCfg[E_MDRV_SCLDMA_MEM_FRM] = pEntry->stData.stFlag.bOutBufCfg[E_MDRV_SCLDMA_MEM_FRM];
                stFlag.bOutBufCfg[E_MDRV_SCLDMA_MEM_SNP] = pEntry->stData.stFlag.bOutBufCfg[E_MDRV_SCLDMA_MEM_SNP];
                stFlag.bOutBufCfg[E_MDRV_SCLDMA_MEM_IMI] = pEntry->stData.stFlag.bOutBufCfg[E_MDRV_SCLDMA_MEM_IMI];
            }
        }
        else
        {
            SCL_DBG(SCL_DBG_LV_MULTI_INST_ENTRY()&(Get_DBGMG_MULTI(enID)), "[MULTIINST]%s(%d)::The first time\n", __FUNCTION__, __LINE__);
        }

        SCL_DBG(SCL_DBG_LV_MULTI_INST_ENTRY()&(Get_DBGMG_MULTI(enID)), "[MULTIINST]%s(%d)::PrivateId:%lx, bInBuf:%d,bOutBuf_FRM:%d, bOutBuf_SNP:%d, bOutBuf_IMI:%d, bInTrig:%d, bOutTrig_FRM:%d, bOutTrig_SNP:%d, bOutTrig_IMI:%d\n",
            __FUNCTION__, __LINE__,
            pEntry->s32PivateId, stFlag.bInBufCfg,
            stFlag.bOutBufCfg[E_MDRV_SCLDMA_MEM_FRM],stFlag.bOutBufCfg[E_MDRV_SCLDMA_MEM_SNP],stFlag.bOutBufCfg[E_MDRV_SCLDMA_MEM_IMI],
            stFlag.bInTrigCfg,
            stFlag.bOutTrigCfg[E_MDRV_SCLDMA_MEM_FRM],stFlag.bOutTrigCfg[E_MDRV_SCLDMA_MEM_SNP],stFlag.bOutTrigCfg[E_MDRV_SCLDMA_MEM_IMI]);

        bRet = 1;
        if(stFlag.bInBufCfg)
        {
            bRet &= MDrvSclDmaSetDmaReadClientConfig(enScldmaId, &pEntry->stData.stInBufCfg);
        }

        if(stFlag.bOutBufCfg[E_MDRV_SCLDMA_MEM_FRM])
        {
            bRet &= MDrvSclDmaSetDmaWriteClientConfig(enScldmaId, &pEntry->stData.stOutBufCfg[E_MDRV_SCLDMA_MEM_FRM]);
        }


        if(stFlag.bOutBufCfg[E_MDRV_SCLDMA_MEM_SNP])
        {
            bRet &= MDrvSclDmaSetDmaWriteClientConfig(enScldmaId, &pEntry->stData.stOutBufCfg[E_MDRV_SCLDMA_MEM_SNP]);
        }


        if(stFlag.bOutBufCfg[E_MDRV_SCLDMA_MEM_IMI])
        {
            bRet &= MDrvSclDmaSetDmaWriteClientConfig(enScldmaId, &pEntry->stData.stOutBufCfg[E_MDRV_SCLDMA_MEM_IMI]);
        }

        if(stFlag.bInTrigCfg)
        {
            bRet &= MDrvSclDmaSetDmaReadClientTrigger(enScldmaId, &pEntry->stData.stInTrigCfg);
        }

        if(stFlag.bOutTrigCfg[E_MDRV_SCLDMA_MEM_FRM])
        {
            bRet &= MDrvSclDmaSetDmaWriteClientTrigger(enScldmaId, &pEntry->stData.stOutTrigCfg[E_MDRV_SCLDMA_MEM_FRM]);
        }

        if(stFlag.bOutTrigCfg[E_MDRV_SCLDMA_MEM_SNP])
        {
            bRet &= MDrvSclDmaSetDmaWriteClientTrigger(enScldmaId, &pEntry->stData.stOutTrigCfg[E_MDRV_SCLDMA_MEM_SNP]);
        }

        if(stFlag.bOutTrigCfg[E_MDRV_SCLDMA_MEM_IMI])
        {
            bRet &= MDrvSclDmaSetDmaWriteClientTrigger(enScldmaId, &pEntry->stData.stOutTrigCfg[E_MDRV_SCLDMA_MEM_IMI]);
        }

        gs32PreMultiInstScldmaPrivateId[enID] = pEntry->s32PivateId;
    }
    else
    {
        SCL_DBG(SCL_DBG_LV_MULTI_INST_ERR, "[MULTIINST]%s(%d):: ReloadScldmaFail, ScldmaId:%d\n",
            __FUNCTION__, __LINE__, enID);

        bRet = 0;
    }

    MULTIINST_RELEASE_SEM(gMultiInstScldmaEntrySem[enID]);

    return bRet;
}

u8 MDrvSclMultiInstGetHvspQuantifyPreInstId(void)
{
    u8 i;
    u8 Ret = 0;
    for(i=0; i<MDRV_SCLMULTI_INST_HVSP_NUM; i++)
    {
        if(gs32PreMultiInstHvspPrivateId[E_MDRV_SCLMULTI_INST_HVSP_DATA_ID_1] ==
            gstMultiInstHvspEntryCfg[E_MDRV_SCLMULTI_INST_HVSP_DATA_ID_1][i].s32PivateId)
        {
            Ret = i;
            break;
        }
    }
    return Ret;
}
MDrvSclMultiInstStatusType_e _MDrvSclMultiInstEntryFlashHvspData(
    MDrvSclMultiInstHvspDataIdType_e enID,
    MDrvSclMultiInstHvspEntryConfig_t *pEntry,
    MDrvSclMultiInstCmdType_e enCmd,
    void *pData)
{
    MDrvSclMultiInstStatusType_e enRet = E_MDRV_SCLMULTI_INST_STATUS_SUCCESS;
    MDrvSclMultiInstLockIdType_e enLockID;
    bool bSave, bReload ;

    if(enCmd == E_MDRV_SCLMULTI_INST_CMD_FORCE_RELOAD_CONFIG && pEntry == NULL)
    {
        u8 i;

        SCL_DBG(SCL_DBG_LV_MULTI_INST_ENTRY()&(Get_DBGMG_MULTI(enID)), "[MULTIINST]%s(%d): Force_Relaod, PrivateId=%lx\n",
            __FUNCTION__, __LINE__, gs32PreMultiInstHvspPrivateId[enID]);

        for(i=0; i<MDRV_SCLMULTI_INST_HVSP_NUM; i++)
        {
            if(gs32PreMultiInstHvspPrivateId[enID] == gstMultiInstHvspEntryCfg[enID][i].s32PivateId)
            {
                pEntry = &gstMultiInstHvspEntryCfg[enID][i];
                SCL_DBG(SCL_DBG_LV_MULTI_INST_ENTRY()&(Get_DBGMG_MULTI(enID)), "[MULTIINST]%s(%d): Force_Relaod, idx=%d\n",
                    __FUNCTION__, __LINE__, i);
                break;
            }
        }

        if(pEntry == NULL)
        {
            return E_MDRV_SCLMULTI_INST_STATUS_SUCCESS;
        }
    }

    if(pEntry)
    {
        if(enCmd == E_MDRV_SCLMULTI_INST_CMD_MAX)
        {
            enRet = E_MDRV_SCLMULTI_INST_STATUS_SUCCESS;
        }
        else
        {
            enLockID = enID == E_MDRV_SCLMULTI_INST_HVSP_DATA_ID_1 ? E_MDRV_SCLMULTI_INST_LOCK_ID_SC_1_2 :
                       enID == E_MDRV_SCLMULTI_INST_HVSP_DATA_ID_2 ? E_MDRV_SCLMULTI_INST_LOCK_ID_SC_1_2 :
                       enID == E_MDRV_SCLMULTI_INST_HVSP_DATA_ID_3 ? E_MDRV_SCLMULTI_INST_LOCK_ID_SC_3   :
                                                                  E_MDRV_SCLMULTI_INST_LOCK_ID_MAX;

            if( MDrvSclMultiInstLockIsFree(enLockID, pEntry->s32PivateId) )
            {
                // Reload Data
                bReload = _MDrvSclMultiInstEntryReloadHvspData(enID, pEntry, enCmd);
                // Save Data
                bSave = (enCmd == E_MDRV_SCLMULTI_INST_CMD_FORCE_RELOAD_CONFIG) ?
                        1 :
                        _MDrvSclMultiInstEntrySaveHvspData(enID, pEntry, enCmd, pData);

                if(bReload & bSave)
                {
                    enRet = E_MDRV_SCLMULTI_INST_STATUS_SUCCESS;
                }
                else
                {
                    enRet = E_MDRV_SCLMULTI_INST_STATUS_FAIL;
                    SCL_DBG(SCL_DBG_LV_MULTI_INST_ENTRY()&(Get_DBGMG_MULTI(enID)), "[MULTIINST]%s(%d): FlashHvspData fail, PrivateId=%lx, R:%d, S:%d\n",
                        __FUNCTION__, __LINE__, pEntry->s32PivateId, bReload, bSave);
                }
            }
            else
            {
                enRet = E_MDRV_SCLMULTI_INST_STATUS_LOCKED;
                SCL_DBG(SCL_DBG_LV_MULTI_INST_ENTRY()&(Get_DBGMG_MULTI(enID)), "[MULTIINST]%s(%d): FlashHvspData LOCKED, PrivateId=%lx\n",
                    __FUNCTION__, __LINE__, pEntry->s32PivateId);
            }
        }
    }
    else
    {
        enRet = E_MDRV_SCLMULTI_INST_STATUS_FAIL;
        SCL_DBG(SCL_DBG_LV_MULTI_INST_ERR, "[MULTIINST]%s(%d): FlashHvspData fail, PrivateId= NULL\n", __FUNCTION__, __LINE__);
    }
    return enRet;
}

MDrvSclMultiInstStatusType_e _MDrvSclMultiInstEntryFlashDmaData(
    MDrvSclMultiInstDmaDataIdType_e enID,
    MDrvSclMultiInstDmaEntryConfig_t *pEntry,
    MDrvSclMultiInstCmdType_e enCmd,
    void *pData)
{
    MDrvSclMultiInstStatusType_e enRet = E_MDRV_SCLMULTI_INST_STATUS_SUCCESS;
    MDrvSclMultiInstLockIdType_e enLockID;
    bool bSave, bReload ;

    if(enCmd == E_MDRV_SCLMULTI_INST_CMD_FORCE_RELOAD_CONFIG && pEntry == NULL)
    {
        u8 i;

        SCL_DBG(SCL_DBG_LV_MULTI_INST_ENTRY()&(Get_DBGMG_MULTI(enID)), "[MULTIINST]%s(%d): Force_Relaod, PrivateId=%lx\n",
            __FUNCTION__, __LINE__, gs32PreMultiInstScldmaPrivateId[enID]);

        for(i=0; i<MDRV_SCLMULTI_INST_SCLDMA_NUM; i++)
        {
            if(gs32PreMultiInstScldmaPrivateId[enID] == gstMultiInstScldmaEntryCfg[enID][i].s32PivateId)
            {
                pEntry = &gstMultiInstScldmaEntryCfg[enID][i];
                SCL_DBG(SCL_DBG_LV_MULTI_INST_ENTRY()&(Get_DBGMG_MULTI(enID)), "[MULTIINST]%s(%d): Force_Relaod, idx=%d\n",
                    __FUNCTION__, __LINE__, i);
                break;
            }
        }

        if(pEntry == NULL)
        {
            return E_MDRV_SCLMULTI_INST_STATUS_SUCCESS;
        }

    }

    if(pEntry)
    {
        if(enCmd == E_MDRV_SCLMULTI_INST_CMD_MAX)
        {
            enRet = E_MDRV_SCLMULTI_INST_STATUS_SUCCESS;
        }
        else
        {
            enLockID = enID == E_MDRV_SCLMULTI_INST_DMA_DATA_ID_1 ? E_MDRV_SCLMULTI_INST_LOCK_ID_SC_1_2 :
                       enID == E_MDRV_SCLMULTI_INST_DMA_DATA_ID_2 ? E_MDRV_SCLMULTI_INST_LOCK_ID_SC_1_2 :
                       enID == E_MDRV_SCLMULTI_INST_DMA_DATA_ID_3 ? E_MDRV_SCLMULTI_INST_LOCK_ID_SC_3   :
                       enID == E_MDRV_SCLMULTI_INST_DMA_DATA_ID_4 ? E_MDRV_SCLMULTI_INST_LOCK_ID_DISP   :
                                                                    E_MDRV_SCLMULTI_INST_LOCK_ID_MAX;

            if( MDrvSclMultiInstLockIsFree(enLockID, pEntry->s32PivateId) )
            {
                // Reload Data
                bReload = _MDrvSclMultiInstEntryReloadDmaData(enID, pEntry, enCmd);

                // Save Data
                bSave = (enCmd == E_MDRV_SCLMULTI_INST_CMD_FORCE_RELOAD_CONFIG) ?
                        1 :
                        _MDrvSclMultiInstEntrySaveDmaData(enID, pEntry, enCmd, pData);

                if(bReload && bSave)
                {
                    enRet = E_MDRV_SCLMULTI_INST_STATUS_SUCCESS;
                }
                else
                {
                    enRet = E_MDRV_SCLMULTI_INST_STATUS_FAIL;
                    SCL_DBG(SCL_DBG_LV_MULTI_INST_ENTRY()&(Get_DBGMG_MULTI(enID)), "[MULTIINST]%s(%d): FlashScldmaData fail, PrivateId=%lx, R:%d, S:%d\n",
                        __FUNCTION__, __LINE__, pEntry->s32PivateId, bReload, bSave);
                }
            }
            else
            {
                enRet = E_MDRV_SCLMULTI_INST_STATUS_LOCKED;
                SCL_DBG(SCL_DBG_LV_MULTI_INST_ENTRY()&(Get_DBGMG_MULTI(enID)), "[MULTIINST]%s(%d): FlashScldmaData LOCKED, PrivateId=%lx\n", __FUNCTION__, __LINE__, pEntry->s32PivateId);
            }
        }
    }
    else
    {
        enRet = E_MDRV_SCLMULTI_INST_STATUS_FAIL;
        SCL_DBG(SCL_DBG_LV_MULTI_INST_ERR, "[MULTIINST]%s(%d): FlashScldmaData fail, PrivateId=%lx\n", __FUNCTION__, __LINE__, pEntry->s32PivateId);
    }

    return enRet;
}



bool MDrvSclMultiInstEntryInitVariable(MDrvSclMultiInstEntryIdType_e enID)
{
    bool bRet = 1;
    u16 i;
    char word[]     = {"_HVSP_Mutex"};
    char word2[]     = {"_SCLDMA_Mutex"};

    SCL_DBG(SCL_DBG_LV_MULTI_INST_FUNC()&(Get_DBGMG_MULTI(enID)), "[MULTIINST]%s(%d)\n", __FUNCTION__, __LINE__);

    if(enID <= E_MDRV_SCLMULTI_INST_ENTRY_ID_HVSP3)
    {
        MDrvSclMultiInstHvspDataIdType_e enHvspId;

        if( _MDrvSclMultiInstTransHvspId(enID, &enHvspId))
        {
            for(i=0;i<MDRV_SCLMULTI_INST_HVSP_NUM; i++)
            {
                gstMultiInstHvspEntryCfg[enHvspId][i].s32PivateId = MDRV_MULTI_INST_HVSP_PRIVATE_ID_HEADER | (enHvspId << 8) | i;
                gstMultiInstHvspEntryCfg[enHvspId][i].bUsed = 0;
                DrvSclOsMemset(&gstMultiInstHvspEntryCfg[enHvspId][i].stData,  0, sizeof(MDrvSclMultiInstHvspDataType_t));
            }

            gs32PreMultiInstHvspPrivateId[enHvspId] = -1;
            gMultiInstHvspEntrySem[enHvspId] = DrvSclOsCreateMutex(E_DRV_SCLOS_FIFO, word, SCLOS_PROCESS_SHARED);
            bRet = 1;
        }
        else
        {
            SCL_DBG(SCL_DBG_LV_MULTI_INST_ERR, "[MULTIINST]%s(%d): Init fail, EntryId=%d\n", __FUNCTION__, __LINE__, enID);
            bRet = 0;
        }

    }
    else if(enID <= E_MDRV_SCLMULTI_INST_ENTRY_ID_DISP)
    {
        MDrvSclMultiInstDmaDataIdType_e enScldmaId;
        if( _MDrvSclMultiInstTransDmaId(enID, &enScldmaId))
        {
            for(i=0;i<MDRV_SCLMULTI_INST_SCLDMA_NUM; i++)
            {
                gstMultiInstScldmaEntryCfg[enScldmaId][i].s32PivateId = MDRV_MULTI_INST_SCLDMA_PRIVATE_ID_HEADER | (enScldmaId << 8) | i;
                gstMultiInstScldmaEntryCfg[enScldmaId][i].bUsed = 0;
                DrvSclOsMemset(&gstMultiInstScldmaEntryCfg[enScldmaId][i].stData,  0, sizeof(MDrvSclMultiInstDmaDataType_t));
            }
            gs32PreMultiInstScldmaPrivateId[enScldmaId] = -1;
            gMultiInstScldmaEntrySem[enScldmaId] = DrvSclOsCreateMutex(E_DRV_SCLOS_FIFO, word2, SCLOS_PROCESS_SHARED);
            bRet = 1;
        }
        else
        {
            SCL_DBG(SCL_DBG_LV_MULTI_INST_ERR, "[MULTIINST]%s(%d): Init fail, EntryId=%d\n", __FUNCTION__, __LINE__, enID);
            bRet = 0;
        }
    }
    else
    {
        SCL_DBG(SCL_DBG_LV_MULTI_INST_ERR, "[MULTIINST]%s(%d): Init fail, EntyrId=%d\n", __FUNCTION__, __LINE__, enID);
        bRet = 0;
    }

    return bRet;
}


bool MDrvSclMultiInstEntryAlloc(MDrvSclMultiInstEntryIdType_e enID, void **pPrivate_Data)
{
    signed short i;
    signed short s16Entry = -1;
    bool bRet = 1;

    SCL_DBG(SCL_DBG_LV_MULTI_INST_FUNC()&(Get_DBGMG_MULTI(enID)), "[MULTIINST]%s(%d)\n", __FUNCTION__, __LINE__);

    if(enID <= E_MDRV_SCLMULTI_INST_ENTRY_ID_HVSP3)
    {
        MDrvSclMultiInstHvspDataIdType_e enHvspId;
        if( _MDrvSclMultiInstTransHvspId(enID, &enHvspId))
        {
            MULTIINST_WAIT_SEM(gMultiInstHvspEntrySem[enHvspId]);
            for(i=0; i<MDRV_SCLMULTI_INST_HVSP_NUM; i++)
            {
                if(gstMultiInstHvspEntryCfg[enHvspId][i].bUsed == 0)
                {
                    s16Entry = i;
                    break;
                }
            }
            if(s16Entry < 0)
            {
                *pPrivate_Data = NULL;
                bRet = 0;
            }
            else
            {
                gstMultiInstHvspEntryCfg[enHvspId][s16Entry].bUsed = 1;
                *pPrivate_Data =  (void *)&gstMultiInstHvspEntryCfg[enHvspId][s16Entry];
                bRet = 1;
            }
            MULTIINST_RELEASE_SEM(gMultiInstHvspEntrySem[enHvspId]);
            SCL_DBG(SCL_DBG_LV_MULTI_INST_ENTRY()&(Get_DBGMG_MULTI(enID)), "[MULTIINST]%s(%d)::PrivateId:%lx, %lx\n",
                __FUNCTION__, __LINE__, gstMultiInstHvspEntryCfg[enHvspId][s16Entry].s32PivateId, (u32)*pPrivate_Data);
        }
        else
        {
            SCL_DBG(SCL_DBG_LV_MULTI_INST_ERR, "[MULTIINST]%s(%d): Alloc fail, EntryId=%d\n", __FUNCTION__, __LINE__, enID);
            bRet = 0;
        }
    }
    else if(enID <= E_MDRV_SCLMULTI_INST_ENTRY_ID_DISP)
    {
        MDrvSclMultiInstDmaDataIdType_e enScldmaId;
        if( _MDrvSclMultiInstTransDmaId(enID, &enScldmaId))
        {
            MULTIINST_WAIT_SEM(gMultiInstScldmaEntrySem[enScldmaId]);

            for(i=0;i<MDRV_SCLMULTI_INST_SCLDMA_NUM; i++)
            {
                if(gstMultiInstScldmaEntryCfg[enScldmaId][i].bUsed == 0)
                {
                    s16Entry = i;
                    break;
                }
            }

            if(s16Entry < 0)
            {
                *pPrivate_Data = NULL;
                bRet = 0;
            }
            else
            {
                gstMultiInstScldmaEntryCfg[enScldmaId][s16Entry].bUsed = 1;
                *pPrivate_Data =  (void *)&gstMultiInstScldmaEntryCfg[enScldmaId][i];
                bRet = 1;
            }
            MULTIINST_RELEASE_SEM(gMultiInstScldmaEntrySem[enScldmaId]);
            SCL_DBG(SCL_DBG_LV_MULTI_INST_ENTRY()&(Get_DBGMG_MULTI(enID)), "[MULTIINST]%s(%d):: PrivateId:%lx, %lx\n",
                __FUNCTION__, __LINE__, gstMultiInstScldmaEntryCfg[enScldmaId][s16Entry].s32PivateId, (u32)*pPrivate_Data);

        }
        else
        {
            SCL_DBG(SCL_DBG_LV_MULTI_INST_ERR, "[MULTIINST]%s(%d): Alloc fail EntryId=%d\n", __FUNCTION__, __LINE__, enID);
            bRet = 0;
        }
    }
    else
    {
        SCL_DBG(SCL_DBG_LV_MULTI_INST_ERR, "[MULTIINST]%s(%d): Alloc fail, EntryId=%d\n", __FUNCTION__, __LINE__, enID);
        bRet = 0;
    }

    return bRet;
}

bool MDrvSclMultiInstEntryFree(MDrvSclMultiInstEntryIdType_e enID, void *pPrivate_Data)
{
    bool bRet;
    SCL_DBG(SCL_DBG_LV_MULTI_INST_FUNC()&(Get_DBGMG_MULTI(enID)), "[MULTIINST]%s(%d)\n", __FUNCTION__, __LINE__);

    if(enID <= E_MDRV_SCLMULTI_INST_ENTRY_ID_HVSP3)
    {
        MDrvSclMultiInstHvspDataIdType_e enHvspId;
        if( _MDrvSclMultiInstTransHvspId(enID, &enHvspId) && pPrivate_Data)
        {
            MDrvSclMultiInstHvspEntryConfig_t *pEntry = NULL;

            MULTIINST_WAIT_SEM(gMultiInstHvspEntrySem[enHvspId]);
            pEntry = (MDrvSclMultiInstHvspEntryConfig_t *)pPrivate_Data;
            pEntry->bUsed = 0;
            DrvSclOsMemset(&pEntry->stData, 0, sizeof(MDrvSclMultiInstHvspDataType_t));
            MULTIINST_RELEASE_SEM(gMultiInstHvspEntrySem[enHvspId]);

            bRet = 1;
        }
        else
        {
            SCL_DBG(SCL_DBG_LV_MULTI_INST_ERR, "[MULTIINST]%s(%d): Free fail, EntryId=%d\n", __FUNCTION__, __LINE__, enID);
            bRet = 0;
        }
    }
    else if(enID <= E_MDRV_SCLMULTI_INST_ENTRY_ID_DISP)
    {
        MDrvSclMultiInstDmaDataIdType_e enSlcmdaId;
        if( _MDrvSclMultiInstTransDmaId(enID, &enSlcmdaId) && pPrivate_Data)
        {
            MDrvSclMultiInstDmaEntryConfig_t *pEntry = NULL;

            MULTIINST_WAIT_SEM(gMultiInstScldmaEntrySem[enSlcmdaId]);
            pEntry = (MDrvSclMultiInstDmaEntryConfig_t *)pPrivate_Data;
            pEntry->bUsed = 0;
            DrvSclOsMemset(&pEntry->stData, 0, sizeof(MDrvSclMultiInstDmaDataType_t));
            MULTIINST_RELEASE_SEM(gMultiInstScldmaEntrySem[enSlcmdaId]);

            bRet = 1;
        }
        else
        {
            SCL_DBG(SCL_DBG_LV_MULTI_INST_ERR, "[MULTIINST]%s(%d): Free fail EntryId=%d\n", __FUNCTION__, __LINE__, enID);
            bRet = 0;
        }
    }
    else
    {
        SCL_DBG(SCL_DBG_LV_MULTI_INST_ERR, "[MULTIINST]%s(%d): Free fail, EntryId=%d\n", __FUNCTION__, __LINE__, enID);
        bRet = 0;
    }

    return bRet;
}

MDrvSclMultiInstStatusType_e MDrvSclMultiInstEntryFlashData(
    MDrvSclMultiInstEntryIdType_e enID,
    void *pPrivateData,
    MDrvSclMultiInstCmdType_e enCmd,
    void *pData)
{
    MDrvSclMultiInstStatusType_e enRet = E_MDRV_SCLMULTI_INST_STATUS_SUCCESS;
    SCL_DBG(SCL_DBG_LV_MULTI_INST_FUNC()&(Get_DBGMG_MULTI(enID)), "[MULTIINST]%s(%d)\n", __FUNCTION__, __LINE__);

    if(enID <= E_MDRV_SCLMULTI_INST_ENTRY_ID_HVSP3)
    {
        MDrvSclMultiInstHvspDataIdType_e enHvspId;
        MDrvSclMultiInstHvspEntryConfig_t *pEntry =  (MDrvSclMultiInstHvspEntryConfig_t *)pPrivateData;
        if(_MDrvSclMultiInstTransHvspId(enID, &enHvspId))
        {
            enRet = _MDrvSclMultiInstEntryFlashHvspData(enHvspId, pEntry, enCmd, pData);
        }
        else
        {
            SCL_DBG(SCL_DBG_LV_MULTI_INST_ENTRY()&(Get_DBGMG_MULTI(enID)), "[MULTIINST]%s(%d): FlashData fail, PrivateId=%lx\n", __FUNCTION__, __LINE__, pEntry->s32PivateId);
            enRet = E_MDRV_SCLMULTI_INST_STATUS_FAIL;
        }
    }
    else if(enID <= E_MDRV_SCLMULTI_INST_ENTRY_ID_DISP)
    {
        MDrvSclMultiInstDmaDataIdType_e enSclmdaId;
        MDrvSclMultiInstDmaEntryConfig_t *pEntry =  (MDrvSclMultiInstDmaEntryConfig_t *)pPrivateData;
        if(_MDrvSclMultiInstTransDmaId(enID, &enSclmdaId ))
        {
            enRet = _MDrvSclMultiInstEntryFlashDmaData(enSclmdaId, pEntry, enCmd, pData);
        }
        else
        {
            SCL_DBG(SCL_DBG_LV_MULTI_INST_ENTRY()&(Get_DBGMG_MULTI(enID)), "[MULTIINST]%s(%d): FlashData fail, PrivateId=%lx\n", __FUNCTION__, __LINE__, pEntry->s32PivateId);
            enRet = E_MDRV_SCLMULTI_INST_STATUS_FAIL;
        }
    }
    else
    {
        SCL_DBG(SCL_DBG_LV_MULTI_INST_ERR, "[MULTIINST]%s(%d): FlashData fail, EntryId=%d\n", __FUNCTION__, __LINE__, enID);
        enRet = E_MDRV_SCLMULTI_INST_STATUS_FAIL;
    }

    return enRet;
}


bool MDrvSclMultiInstEntryGetPirvateId(MDrvSclMultiInstEntryIdType_e enID, void *pPrivateData, s32 *ps32PrivateId)
{
    bool bRet;

    SCL_DBG(SCL_DBG_LV_MULTI_INST_FUNC()&(Get_DBGMG_MULTI(enID)), "[MULTIINST]%s(%d)\n", __FUNCTION__, __LINE__);

    if(enID <= E_MDRV_SCLMULTI_INST_ENTRY_ID_HVSP3)
    {
        MDrvSclMultiInstHvspEntryConfig_t *pEntry =  (MDrvSclMultiInstHvspEntryConfig_t *)pPrivateData;

        if(pEntry)
        {
            *ps32PrivateId = pEntry->s32PivateId;
             bRet = 1;
        }
        else
        {
            SCL_DBG(SCL_DBG_LV_MULTI_INST_ERR, "[MULTIINST]%s(%d): GetPrivateID  fail, EntryId=%d\n", __FUNCTION__, __LINE__, enID);
            *ps32PrivateId = -1;
            bRet = 0;
        }
    }
    else if(enID <= E_MDRV_SCLMULTI_INST_ENTRY_ID_DISP)
    {
        MDrvSclMultiInstDmaEntryConfig_t *pEntry =  (MDrvSclMultiInstDmaEntryConfig_t *)pPrivateData;
        if(pEntry)
        {
            *ps32PrivateId = pEntry->s32PivateId;
            bRet = 1;
        }
        else
        {
            SCL_DBG(SCL_DBG_LV_MULTI_INST_ERR, "[MULTIINST]%s(%d): GetPrivateID  fail, EntryId=%d\n", __FUNCTION__, __LINE__, enID);
            *ps32PrivateId = -1;
            bRet = 0;
        }
    }
    else
    {
        SCL_DBG(SCL_DBG_LV_MULTI_INST_ERR, "[MULTIINST]%s(%d): GetPrivateID  fail, EntryId:%d\n", __FUNCTION__, __LINE__, enID);
        bRet = 0;
    }

    SCL_DBG(SCL_DBG_LV_MULTI_INST_ENTRY()&(Get_DBGMG_MULTI(enID)), "[MULTIINST]%s(%d) EntryId:%x PrivateID=%lx\n",
        __FUNCTION__, __LINE__, enID, *ps32PrivateId);
    return bRet;
}

MDrvSclMultiInstStatusType_e MDrvSclMultiInstEtnryIsFree(MDrvSclMultiInstEntryIdType_e enID, void *pPrivateData)
{
    MDrvSclMultiInstStatusType_e enIsFree;
    MDrvSclMultiInstLockIdType_e enLock_ID;

    SCL_DBG(SCL_DBG_LV_MULTI_INST_FUNC()&(Get_DBGMG_MULTI(enID)), "[MULTIINST]%s(%d)\n", __FUNCTION__, __LINE__);

    if(enID <= E_MDRV_SCLMULTI_INST_ENTRY_ID_HVSP3)
    {
        MDrvSclMultiInstHvspEntryConfig_t *pEntry =  (MDrvSclMultiInstHvspEntryConfig_t *)pPrivateData;

        enLock_ID = (enID == E_MDRV_SCLMULTI_INST_ENTRY_ID_HVSP1) ? E_MDRV_SCLMULTI_INST_LOCK_ID_SC_1_2 :
                    (enID == E_MDRV_SCLMULTI_INST_ENTRY_ID_HVSP2) ? E_MDRV_SCLMULTI_INST_LOCK_ID_SC_1_2 :
                    (enID == E_MDRV_SCLMULTI_INST_ENTRY_ID_HVSP3) ? E_MDRV_SCLMULTI_INST_LOCK_ID_SC_3   :
                                                                 E_MDRV_SCLMULTI_INST_LOCK_ID_MAX;
        if(pEntry)
        {
            if( MDrvSclMultiInstLockIsFree(enLock_ID, pEntry->s32PivateId) )
            {
                enIsFree = E_MDRV_SCLMULTI_INST_STATUS_SUCCESS;
            }
            else
            {
                enIsFree = E_MDRV_SCLMULTI_INST_STATUS_LOCKED;
            }
        }
        else
        {
            SCL_DBG(SCL_DBG_LV_MULTI_INST_ERR, "[MULTIINST]%s(%d): EntryIsFree  fail, EntryId:%d\n", __FUNCTION__, __LINE__, enID);
            enIsFree = E_MDRV_SCLMULTI_INST_STATUS_FAIL;
        }
    }
    else if(enID <= E_MDRV_SCLMULTI_INST_ENTRY_ID_DISP)
    {
        MDrvSclMultiInstDmaEntryConfig_t *pEntry =  (MDrvSclMultiInstDmaEntryConfig_t *)pPrivateData;
        enLock_ID = (enID == E_MDRV_SCLMULTI_INST_ENTRY_ID_SCLDMA1) ? E_MDRV_SCLMULTI_INST_LOCK_ID_SC_1_2 :
                    (enID == E_MDRV_SCLMULTI_INST_ENTRY_ID_SCLDMA2) ? E_MDRV_SCLMULTI_INST_LOCK_ID_SC_1_2 :
                    (enID == E_MDRV_SCLMULTI_INST_ENTRY_ID_SCLDMA3) ? E_MDRV_SCLMULTI_INST_LOCK_ID_SC_3   :
                    (enID == E_MDRV_SCLMULTI_INST_ENTRY_ID_DISP)    ? E_MDRV_SCLMULTI_INST_LOCK_ID_DISP   :
                                                                   E_MDRV_SCLMULTI_INST_LOCK_ID_MAX;
        if(pEntry)
        {
            if( MDrvSclMultiInstLockIsFree(enLock_ID, pEntry->s32PivateId) )
            {
                enIsFree = E_MDRV_SCLMULTI_INST_STATUS_SUCCESS;
            }
            else
            {
                enIsFree = E_MDRV_SCLMULTI_INST_STATUS_LOCKED;
            }
        }
        else
        {
            SCL_DBG(SCL_DBG_LV_MULTI_INST_ERR, "[MULTIINST]%s(%d): EntryIsFree  fail, EntryId:%d\n", __FUNCTION__, __LINE__, enID);
            enIsFree = E_MDRV_SCLMULTI_INST_STATUS_FAIL;
        }
    }
    else
    {
        SCL_DBG(SCL_DBG_LV_MULTI_INST_ERR, "[MULTIINST]%s(%d): EntryIsFree  fail, EntryId:%d\n", __FUNCTION__, __LINE__, enID);
        enIsFree = E_MDRV_SCLMULTI_INST_STATUS_FAIL;
    }

    return enIsFree;
}
