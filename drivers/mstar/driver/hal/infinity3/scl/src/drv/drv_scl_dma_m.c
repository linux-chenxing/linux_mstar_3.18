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
#define  _MDRV_SCLDMA_C
#include "drv_scl_os.h"
#include "drv_scl_dbg.h"
#include "hal_scl_util.h"
#include "drv_scl_dma_st.h"
#include "drv_scl_dma.h"
#include "drv_scl_dma_m.h"

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define BUFFER_NUMBER_TO_HWIDX_OFFSET 1
#define _ISQueueNeedCopyToUser(enUsedType) (enUsedType==EN_MDRV_SCLDMA_BUFFER_QUEUE_TYPE_PEEKQUEUE \
        || enUsedType==EN_MDRV_SCLDMA_BUFFER_QUEUE_TYPE_DEQUEUE)
//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Function
//-------------------------------------------------------------------------------------------------
void _MDrvSclDmaBufferQueueHandlerByUsedType
(MDrvSclDMaUsedBufferQueueType_e  enUsedType,DrvSclDmaBufferQueueConfig_t *pstCfg)
{
    switch(enUsedType)
    {
        case EN_MDRV_SCLDMA_BUFFER_QUEUE_TYPE_PEEKQUEUE:
                DrvSclDmaPeekBufferQueue(pstCfg);
            break;
        case EN_MDRV_SCLDMA_BUFFER_QUEUE_TYPE_DEQUEUE:
                DrvSclDmaBufferDeQueue(pstCfg);
            break;
        case EN_MDRV_SCLDMA_BUFFER_QUEUE_TYPE_ENQUEUE:
                DrvSclDmaEnableBufferAccess(pstCfg);
            break;
        default :
            SCL_ERR( "[SCLDMA]%s NO this Queue Handler Type\n", __FUNCTION__);
            break;
    }
}

MDrvSclDmaMemType_e _MDrvSclDmaSwitchIDtoReadModeForDriverlayer
(MDrvSclDmaIdType_e enSCLDMA_ID,MDrvSclDmaMemType_e enMemType,DrvSclDmaIdType_e *enID)
{
    DrvSclDmaRwModeType_e enRWMode;
    switch(enSCLDMA_ID)
    {
        case E_MDRV_SCLDMA_ID_3:
            *enID = E_DRV_SCLDMA_ID_3_R;
            enRWMode = enMemType == E_MDRV_SCLDMA_MEM_FRM ?  E_DRV_SCLDMA_FRM_R :
                                                              E_DRV_SCLDMA_RW_NUM;
            break;

        case E_MDRV_SCLDMA_ID_PNL:
            *enID = E_DRV_SCLDMA_ID_PNL_R;
            enRWMode = enMemType == E_MDRV_SCLDMA_MEM_FRM ? E_DRV_SCLDMA_DBG_R :
                                                             E_DRV_SCLDMA_RW_NUM;
            break;

        default:
        case E_MDRV_SCLDMA_ID_1:
        case E_MDRV_SCLDMA_ID_2:
            enRWMode = E_DRV_SCLDMA_RW_NUM;
            SCL_ERR( "[SCLDMA]%s %d::Not support In TRIGGER\n",__FUNCTION__, __LINE__);
            break;
    }
    return enRWMode;
}
MDrvSclDmaMemType_e _MDrvSclDmaSwitchIDtoWriteModeForDriverlayer
(MDrvSclDmaIdType_e enSCLDMA_ID,MDrvSclDmaMemType_e enMemType,DrvSclDmaIdType_e *enID)
{
    DrvSclDmaRwModeType_e enRWMode;
    switch(enSCLDMA_ID)
    {
        case E_MDRV_SCLDMA_ID_1:
            *enID = E_DRV_SCLDMA_ID_1_W;
            enRWMode = enMemType == E_MDRV_SCLDMA_MEM_FRM ? E_DRV_SCLDMA_FRM_W :
                        enMemType == E_MDRV_SCLDMA_MEM_SNP ? E_DRV_SCLDMA_SNP_W :
                        enMemType == E_MDRV_SCLDMA_MEM_IMI ? E_DRV_SCLDMA_IMI_W :
                                                              E_DRV_SCLDMA_RW_NUM;
            break;

        case E_MDRV_SCLDMA_ID_2:
            *enID = E_DRV_SCLDMA_ID_2_W;
            enRWMode = enMemType == E_MDRV_SCLDMA_MEM_FRM ? E_DRV_SCLDMA_FRM_W :
                        enMemType == E_MDRV_SCLDMA_MEM_FRM2 ? E_DRV_SCLDMA_FRM2_W :
                        enMemType == E_MDRV_SCLDMA_MEM_IMI ? E_DRV_SCLDMA_IMI_W :
                                                              E_DRV_SCLDMA_RW_NUM;

            break;

        case E_MDRV_SCLDMA_ID_3:
            *enID = E_DRV_SCLDMA_ID_3_W;
            enRWMode = enMemType == E_MDRV_SCLDMA_MEM_FRM ? E_DRV_SCLDMA_FRM_W :
                                                              E_DRV_SCLDMA_RW_NUM;
            break;

        case E_MDRV_SCLDMA_ID_PNL:
            *enID = E_DRV_SCLDMA_ID_PNL_R;
            enRWMode = enMemType == E_MDRV_SCLDMA_MEM_FRM ? E_DRV_SCLDMA_DBG_R:
                                                              E_DRV_SCLDMA_RW_NUM;
            break;
        default:
            SCL_ERR( "[SCLDMA]%s %d::Not support In SCLDMA\n",__FUNCTION__, __LINE__);
            enRWMode = E_DRV_SCLDMA_RW_NUM;
            break;
    }
    return enRWMode;
}
MDrvSclDmaMemType_e _MDrvSclDmaSwitchIDForDriverlayer
    (MDrvSclDmaIdType_e enSCLDMA_ID,MDrvSclDmaMemType_e enMemType,DrvSclDmaIdType_e *enID,bool bReadDMAMode)
{
    DrvSclDmaRwModeType_e enRWMode;
    if(bReadDMAMode)
    {
        enRWMode = _MDrvSclDmaSwitchIDtoReadModeForDriverlayer(enSCLDMA_ID,enMemType,enID);
    }
    else
    {
        enRWMode = _MDrvSclDmaSwitchIDtoWriteModeForDriverlayer(enSCLDMA_ID,enMemType,enID);
    }
    return enRWMode;
}
MDrvSclDmaAttrType_t _MDrvSclDmaFillDmaInfoStruct(DrvSclDmaAttrType_t stDrvDMACfg)
{
    MDrvSclDmaAttrType_t stSendToIOCfg;
    int u32BufferIdx;
    stSendToIOCfg.u16DMAcount  = stDrvDMACfg.u16DMAcount;
    stSendToIOCfg.u32Trigcount = stDrvDMACfg.u32Trigcount;
    stSendToIOCfg.u16DMAH      = stDrvDMACfg.u16DMAH;
    stSendToIOCfg.u16DMAV      = stDrvDMACfg.u16DMAV;
    stSendToIOCfg.enBufMDType  = stDrvDMACfg.enBuffMode;
    stSendToIOCfg.enColorType  = stDrvDMACfg.enColor;
    stSendToIOCfg.u16BufNum    = stDrvDMACfg.u8MaxIdx+1;
    stSendToIOCfg.bDMAEn       = stDrvDMACfg.bDMAEn;
    for(u32BufferIdx=0;u32BufferIdx<stSendToIOCfg.u16BufNum;u32BufferIdx++)
    {
        stSendToIOCfg.u32Base_Y[u32BufferIdx] = stDrvDMACfg.u32Base_Y[u32BufferIdx];
        stSendToIOCfg.u32Base_C[u32BufferIdx] = stDrvDMACfg.u32Base_C[u32BufferIdx];
        stSendToIOCfg.u32Base_V[u32BufferIdx] = stDrvDMACfg.u32Base_V[u32BufferIdx];
    }
    stSendToIOCfg.bDMAFlag = stDrvDMACfg.bDMAFlag;
    stSendToIOCfg.bDMAReadIdx = stDrvDMACfg.bDMAReadIdx;
    stSendToIOCfg.bDMAWriteIdx = stDrvDMACfg.bDMAWriteIdx;
    stSendToIOCfg.bSendPoll = stDrvDMACfg.bSendPoll;
    stSendToIOCfg.u32FrameDoneTime = stDrvDMACfg.u32FrameDoneTime;
    stSendToIOCfg.u32SendTime = stDrvDMACfg.u32SendTime;
    stSendToIOCfg.u8Count= stDrvDMACfg.u8Count;
    stSendToIOCfg.u8ResetCount= stDrvDMACfg.u8ResetCount;
    stSendToIOCfg.u8DMAErrCount= stDrvDMACfg.u8DMAErrCount;
    return stSendToIOCfg;
}

DrvSclDmaActiveBufferConfig_t _MDrvSclDmaFillActiveBufferStruct
    (MDrvSclDmaIdType_e enSCLDMA_ID, MDrvSclDmaActiveBufferConfig_t *pstCfg,
    DrvSclDmaIdType_e *enID,bool bReadDMAMode)
{
    DrvSclDmaActiveBufferConfig_t stActiveCfg;
    stActiveCfg.u8ActiveBuffer = pstCfg->u8ActiveBuffer;
    stActiveCfg.enRWMode = _MDrvSclDmaSwitchIDForDriverlayer(enSCLDMA_ID,pstCfg->enMemType,enID,bReadDMAMode);
    stActiveCfg.stOnOff.bEn = pstCfg->stOnOff.bEn;
    stActiveCfg.stOnOff.enRWMode =stActiveCfg.enRWMode;
    stActiveCfg.stOnOff.stclk=(DrvSclDmaClkConfig_t *)(pstCfg->stOnOff.stclk);
    return stActiveCfg;
}

bool _MDrvSclDmaGetDmaBufferActiveIdx
    (DrvSclDmaIdType_e enID,MDrvSclDmaActiveBufferConfig_t *pstCfg,DrvSclDmaActiveBufferConfig_t stActiveCfg)
{
    if(DoubleBufferStatus)
    {
        if(DrvSclDmaGetDmaBufferDoneIdx(enID, &stActiveCfg))
        {
            pstCfg->u8ActiveBuffer  = stActiveCfg.u8ActiveBuffer;
            pstCfg->u8ISPcount      = stActiveCfg.u8ISPcount;
            pstCfg->u64FRMDoneTime  = stActiveCfg.u64FRMDoneTime;
            return TRUE;
        }
        else
        {
            pstCfg->u8ActiveBuffer  = 0xFF;
            pstCfg->u8ISPcount      = 0;
            pstCfg->u64FRMDoneTime  = 0;
            return FALSE;
        }
    }
    else
    {
        if(DrvSclDmaGetDmaBufferDoneIdxWithoutDoublebuffer(enID, &stActiveCfg))
        {
            pstCfg->u8ActiveBuffer  = stActiveCfg.u8ActiveBuffer;
            pstCfg->u8ISPcount      = stActiveCfg.u8ISPcount;
            pstCfg->u64FRMDoneTime  = stActiveCfg.u64FRMDoneTime;
            return TRUE;
        }
        else
        {
            pstCfg->u8ActiveBuffer  = 0xFF;
            pstCfg->u8ISPcount      = 0;
            pstCfg->u64FRMDoneTime  = 0;
            return FALSE;
        }
    }
}
DrvSclDmaRwConfig_t _MDrvSclDmaFillRWCfgStruct
    (MDrvSclDmaIdType_e enSCLDMA_ID,MDrvSclDmaBufferConfig_t *pCfg,
    DrvSclDmaIdType_e *enID,bool bReadDMAMode)
{
    DrvSclDmaRwConfig_t stSCLDMACfg;
    u16 u16BufferIdx;
    DrvSclOsMemset(&stSCLDMACfg, 0, sizeof(DrvSclDmaRwConfig_t));
    stSCLDMACfg.enRWMode = _MDrvSclDmaSwitchIDForDriverlayer(enSCLDMA_ID,pCfg->enMemType,enID,bReadDMAMode);
    stSCLDMACfg.u16Height = pCfg->u16Height;
    stSCLDMACfg.u16Width  = pCfg->u16Width;
    stSCLDMACfg.u8MaxIdx  = pCfg->u16BufNum -BUFFER_NUMBER_TO_HWIDX_OFFSET;
    stSCLDMACfg.u8Flag    = pCfg->u8Flag;
    stSCLDMACfg.enBuffMode= (pCfg->enBufMDType == E_MDRV_SCLDMA_BUFFER_MD_RING) ? E_DRV_SCLDMA_BUF_MD_RING :
                            (pCfg->enBufMDType == E_MDRV_SCLDMA_BUFFER_MD_SWRING) ? E_DRV_SCLDMA_BUF_MD_SWRING :
                                                                                E_DRV_SCLDMA_BUF_MD_SINGLE;

    for(u16BufferIdx=0; u16BufferIdx<pCfg->u16BufNum; u16BufferIdx++)
    {
        stSCLDMACfg.u32Base_Y[u16BufferIdx] = _Phys2Miu(pCfg->u32Base_Y[u16BufferIdx]);
        stSCLDMACfg.u32Base_C[u16BufferIdx] = _Phys2Miu(pCfg->u32Base_C[u16BufferIdx]);
        stSCLDMACfg.u32Base_V[u16BufferIdx] = _Phys2Miu(pCfg->u32Base_V[u16BufferIdx]);
    }

    stSCLDMACfg.enColor = pCfg->enColorType == E_MDRV_SCLDMA_COLOR_YUV422 ? E_DRV_SCLDMA_COLOR_YUV422 :
                          pCfg->enColorType == E_MDRV_SCLDMA_COLOR_YUV420 ? E_DRV_SCLDMA_COLOR_YUV420 :
                          pCfg->enColorType == E_MDRV_SCLDMA_COLOR_YCSep422 ? E_DRV_SCLDMA_COLOR_YCSep422 :
                          pCfg->enColorType == E_MDRV_SCLDMA_COLOR_YUVSep422 ? E_DRV_SCLDMA_COLOR_YUVSep422 :
                          pCfg->enColorType == E_MDRV_SCLDMA_COLOR_YUVSep420 ? E_DRV_SCLDMA_COLOR_YUVSep420 :
                            E_DRV_SCLDMA_COLOR_NUM;
    return stSCLDMACfg;
}

bool MDrvSclDmaSetPollWait(DrvSclOsPollWaitConfig_t stPollWait)
{
    return (bool)DrvSclOsSetPollWait(stPollWait);
}

bool MDrvSclDmaSuspend(MDrvSclDmaIdType_e enSCLDMA_ID)
{
    DrvSclDmaSuspendResumeConfig_t stSCLDMASuspendResumeCfg;
    DrvSclDmaIdType_e enID;
    bool bRet = TRUE;

    enID =  enSCLDMA_ID == E_MDRV_SCLDMA_ID_1   ? E_DRV_SCLDMA_ID_1_W   :
            enSCLDMA_ID == E_MDRV_SCLDMA_ID_2   ? E_DRV_SCLDMA_ID_2_W   :
            enSCLDMA_ID == E_MDRV_SCLDMA_ID_3   ? E_DRV_SCLDMA_ID_3_W   :
            enSCLDMA_ID == E_MDRV_SCLDMA_ID_PNL ? E_DRV_SCLDMA_ID_PNL_R :
                                                  E_DRV_SCLDMA_ID_MAX;
    DrvSclOsMemset(&stSCLDMASuspendResumeCfg, 0, sizeof(DrvSclDmaSuspendResumeConfig_t));


    if(DrvSclDmaSuspend(enID, &stSCLDMASuspendResumeCfg))
    {
        bRet = TRUE;
    }
    else
    {
        SCL_ERR( "[SCLDMA]%s %d::Suspend Fail\n",__FUNCTION__, __LINE__);
        bRet = FALSE;
    }

    return bRet;
}

bool MDrvSclDmaResume(MDrvSclDmaIdType_e enSCLDMA_ID)
{
    DrvSclDmaSuspendResumeConfig_t stSCLDMASuspendResumeCfg;
    DrvSclDmaIdType_e enID;
    bool bRet = TRUE;;

    enID =  enSCLDMA_ID == E_MDRV_SCLDMA_ID_1   ? E_DRV_SCLDMA_ID_1_W   :
            enSCLDMA_ID == E_MDRV_SCLDMA_ID_2   ? E_DRV_SCLDMA_ID_2_W   :
            enSCLDMA_ID == E_MDRV_SCLDMA_ID_3   ? E_DRV_SCLDMA_ID_3_W   :
            enSCLDMA_ID == E_MDRV_SCLDMA_ID_PNL ? E_DRV_SCLDMA_ID_PNL_R :
                                                  E_DRV_SCLDMA_ID_MAX;

    DrvSclOsMemset(&stSCLDMASuspendResumeCfg, 0, sizeof(DrvSclDmaSuspendResumeConfig_t));

    if(DrvSclDmaResume(enID, &stSCLDMASuspendResumeCfg))
    {
        bRet = TRUE;
    }
    else
    {
        SCL_ERR( "[SCLDMA]%s %d::Resume Fail\n",__FUNCTION__, __LINE__);
        bRet = FALSE;
    }

    return bRet;
}
bool MDrvSclDmaExit(bool bCloseISR)
{
    DrvSclDmaExit(bCloseISR);
    return 1;
}

bool MDrvSclDmaInit(MDrvSclDmaIdType_e enSCLDMA_ID, MDrvSclDmaInitConfig_t *pCfg)
{
    bool bRet = FALSE;
    DrvSclDmaInitConfig_t stDMAInitCfg;
    SCL_DBG(SCL_DBG_LV_SCLDMA()&(Get_DBGMG_SCLDMA(enSCLDMA_ID,0,0)), "[SCLDMA]%s\n", __FUNCTION__);

    stDMAInitCfg.u32RIUBase     = pCfg->u32Riubase;
    stDMAInitCfg.u32IRQNUM      = pCfg->u32IRQNUM;
    stDMAInitCfg.u32CMDQIRQNUM  = pCfg->u32CMDQIRQNUM;
    if(DrvSclDmaInit(&stDMAInitCfg) == FALSE)
    {
        SCL_ERR( "[SCLDMA]%s %d::Init Fail\n",__FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        bRet = TRUE;
    }

    return bRet;
}
void MDrvSclDmaSysInit(bool bEn)
{
    DrvSclDmaSysInit(bEn);
}
bool MDrvSclDmaGetDoubleBufferStatus(void)
{
    return DoubleBufferStatus;
}
void MDrvSclDmaSetDoubleBufferConfig(bool bEn)
{
    gbDBStatus = bEn;
}
void MDrvSclDmaRelease(MDrvSclDmaIdType_e enSCLDMA_ID,MDrvSclDmaClkConfig_t *stclkcfg)
{
    DrvSclDmaIdType_e enID;
    DrvSclDmaClkConfig_t *stclk;
    enID =  enSCLDMA_ID == E_MDRV_SCLDMA_ID_1   ? E_DRV_SCLDMA_ID_1_W   :
            enSCLDMA_ID == E_MDRV_SCLDMA_ID_2   ? E_DRV_SCLDMA_ID_2_W   :
            enSCLDMA_ID == E_MDRV_SCLDMA_ID_3   ? E_DRV_SCLDMA_ID_3_W   :
            enSCLDMA_ID == E_MDRV_SCLDMA_ID_PNL ? E_DRV_SCLDMA_ID_PNL_R :
                                                  E_DRV_SCLDMA_ID_MAX;

    stclk = (DrvSclDmaClkConfig_t *)(stclkcfg);
    DrvSclDmaRelease(enID,stclk);
}

bool MDrvSclDmaSetDmaReadClientConfig
    (MDrvSclDmaIdType_e enSCLDMA_ID, MDrvSclDmaBufferConfig_t *pCfg)
{
    DrvSclDmaIdType_e enID;
    DrvSclDmaRwConfig_t stSCLDMACfg;
    bool  bRet;

    SCL_DBG(SCL_DBG_LV_SCLDMA()&(Get_DBGMG_SCLDMA(enSCLDMA_ID,pCfg->enMemType,0)), "[SCLDMA]%s\n", __FUNCTION__);

    stSCLDMACfg = _MDrvSclDmaFillRWCfgStruct(enSCLDMA_ID,pCfg,&enID,1);
    bRet = (bool)DrvSclDmaSetDmaClientConfig(enID, stSCLDMACfg);

    return bRet;
}

bool MDrvSclDmaSetDmaReadClientTrigger
    (MDrvSclDmaIdType_e enSCLDMA_ID, MDrvSclDmaTriggerConfig_t *pCfg)
{
    DrvSclDmaIdType_e enID;
    DrvSclDmaOnOffConfig_t stOnOffCfg;
    bool  bRet;

    SCL_DBG(SCL_DBG_LV_SCLDMA()&(Get_DBGMG_SCLDMA(enSCLDMA_ID,pCfg->enMemType,1)), "[SCLDMA]%s\n", __FUNCTION__);
    stOnOffCfg.enRWMode = _MDrvSclDmaSwitchIDtoReadModeForDriverlayer(enSCLDMA_ID,pCfg->enMemType,&enID);
    stOnOffCfg.bEn = pCfg->bEn;
    stOnOffCfg.stclk = (DrvSclDmaClkConfig_t *)(pCfg->stclk);
    bRet = (bool)DrvSclDmaSetDmaClientOnOff(enID ,stOnOffCfg);

    return bRet;
}

bool MDrvSclDmaSetDmaWriteClientConfig
    (MDrvSclDmaIdType_e enSCLDMA_ID, MDrvSclDmaBufferConfig_t *pCfg)
{
    DrvSclDmaIdType_e enID;
    DrvSclDmaRwConfig_t stSCLDMACfg;
    bool  bRet;

    SCL_DBG(SCL_DBG_LV_SCLDMA()&(Get_DBGMG_SCLDMA(enSCLDMA_ID,pCfg->enMemType,0)),
        "[SCLDMA]%s  ID:%d\n", __FUNCTION__,enSCLDMA_ID);

    stSCLDMACfg = _MDrvSclDmaFillRWCfgStruct(enSCLDMA_ID,pCfg,&enID,0);

    bRet = (bool)DrvSclDmaSetDmaClientConfig(enID, stSCLDMACfg);

    return bRet;
}

bool MDrvSclDmaSetDmaWriteClientTrigger
    (MDrvSclDmaIdType_e enSCLDMA_ID, MDrvSclDmaTriggerConfig_t *pCfg)
{
    DrvSclDmaIdType_e enID;
    DrvSclDmaOnOffConfig_t stOnOffCfg;
    bool bRet;

    SCL_DBG(SCL_DBG_LV_SCLDMA()&(Get_DBGMG_SCLDMA(enSCLDMA_ID,pCfg->enMemType,1)), "[SCLDMA]%s\n", __FUNCTION__);

    stOnOffCfg.enRWMode = _MDrvSclDmaSwitchIDtoWriteModeForDriverlayer(enSCLDMA_ID,pCfg->enMemType,&enID);
    stOnOffCfg.bEn = pCfg->bEn;
    stOnOffCfg.stclk = (DrvSclDmaClkConfig_t *)(pCfg->stclk);

    bRet = (bool)DrvSclDmaSetDmaClientOnOff(enID ,stOnOffCfg);

    return bRet;
}

bool MDrvSclDmaGetInBufferDoneEvent
    (MDrvSclDmaIdType_e enSCLDMA_ID, MDrvSclDmaMemType_e enMemType, MDrvSclDmaBufferDoneConfig_t *pCfg)
{
    bool bRet;
    DrvSclDmaDoneConfig_t stDonCfg;
    DrvSclDmaIdType_e enID;

    stDonCfg.enRWMode = _MDrvSclDmaSwitchIDtoReadModeForDriverlayer(enSCLDMA_ID,enMemType,&enID);
    bRet = (bool)DrvSclDmaGetDmaDoneEvent(enID, &stDonCfg);
    pCfg->bDone = (bool)stDonCfg.bDone;

    SCL_DBG(SCL_DBG_LV_SCLDMA()&(Get_DBGMG_SCLDMA(enSCLDMA_ID,enMemType,1)),
        "[SCLDMA]%s: ID:%d, bRet: %d, BufferDone:%d\n", __FUNCTION__, enID, bRet, stDonCfg.bDone);
    return bRet;
}

bool MDrvSclDmaGetOutBufferDoneEvent
    (MDrvSclDmaIdType_e enSCLDMA_ID, MDrvSclDmaMemType_e enMemType, MDrvSclDmaBufferDoneConfig_t *pCfg)
{
    bool bRet;
    DrvSclDmaDoneConfig_t stDonCfg;
    DrvSclDmaIdType_e enID;

    stDonCfg.enRWMode = _MDrvSclDmaSwitchIDtoWriteModeForDriverlayer(enSCLDMA_ID,enMemType,&enID);
    bRet = (bool)DrvSclDmaGetDmaDoneEvent(enID, &stDonCfg);
    pCfg->bDone = (bool)stDonCfg.bDone;

    SCL_DBG(SCL_DBG_LV_SCLDMA()&(Get_DBGMG_SCLDMA(enSCLDMA_ID,enMemType,1)), "[SCLDMA]%s: ID:%d, bRet: %d, BufferDone:%d\n", __FUNCTION__, enID, bRet, stDonCfg.bDone);
    return bRet;
}

bool MDrvSclDmaGetDmaReadBufferActiveIdx
    (MDrvSclDmaIdType_e enSCLDMA_ID, MDrvSclDmaActiveBufferConfig_t *pstCfg)
{
    DrvSclDmaActiveBufferConfig_t stActiveCfg;
    DrvSclDmaIdType_e enID;
    bool bRet;
    SCL_DBG(SCL_DBG_LV_SCLDMA()&(Get_DBGMG_SCLDMA(enSCLDMA_ID,pstCfg->enMemType,1)), "[SCLDMA]%s\n", __FUNCTION__);
    stActiveCfg = _MDrvSclDmaFillActiveBufferStruct(enSCLDMA_ID,pstCfg,&enID,1);
    bRet = _MDrvSclDmaGetDmaBufferActiveIdx(enID, pstCfg, stActiveCfg);
    return bRet;

}
bool MDrvSclDmaBufferQueueHandle
    (MDrvSclDmaIdType_e enSCLDMA_ID, MDrvSclDmaBUfferQueueConfig_t *pstCfg)
{
    bool bRet =0;
    DrvSclDmaIdType_e enID;
    DrvSclDmaBufferQueueConfig_t stCfg;
    SCL_DBG(SCL_DBG_LV_SCLDMA()&(Get_DBGMG_SCLDMA(enSCLDMA_ID,pstCfg->enMemType,1)), "[SCLDMA]%s\n", __FUNCTION__);
    stCfg.enRWMode = _MDrvSclDmaSwitchIDtoWriteModeForDriverlayer(enSCLDMA_ID,pstCfg->enMemType,&enID);
    stCfg.enID = enID;
    stCfg.u8AccessId = pstCfg->u8EnqueueIdx;
    _MDrvSclDmaBufferQueueHandlerByUsedType(pstCfg->enUsedType,&stCfg);
    if(_ISQueueNeedCopyToUser(pstCfg->enUsedType))
    {
        if(stCfg.pstRead != NULL)
        {
            DrvSclOsMemcpy(&pstCfg->stRead,stCfg.pstRead,MDRV_SCLDMA_BUFFER_QUEUE_OFFSET);
            pstCfg->u8InQueueCount = stCfg.u8InQueueCount;
            pstCfg->u8EnqueueIdx   = stCfg.u8AccessId;
            bRet = 1;
        }
    }
    return bRet;
}
bool MDrvSclDmaGetDmaWriteBufferAcitveIdx
    (MDrvSclDmaIdType_e enSCLDMA_ID, MDrvSclDmaActiveBufferConfig_t *pstCfg)
{
    DrvSclDmaActiveBufferConfig_t stActiveCfg;
    DrvSclDmaIdType_e enID;
    bool bRet;
    SCL_DBG(SCL_DBG_LV_SCLDMA()&(Get_DBGMG_SCLDMA(enSCLDMA_ID, pstCfg->enMemType,1)), "[SCLDMA]%s\n", __FUNCTION__);
    stActiveCfg = _MDrvSclDmaFillActiveBufferStruct(enSCLDMA_ID, pstCfg, &enID,0);
    bRet = _MDrvSclDmaGetDmaBufferActiveIdx(enID, pstCfg, stActiveCfg);
    return bRet;
}
void MDrvSclDmaSetForceCloseDmaClient
    (MDrvSclDmaIdType_e enSCLDMA_ID,MDrvSclDmaMemType_e enMemType,
    bool bReadDMAMode,bool bEnForceClose)
{
    DrvSclDmaIdType_e enID;
    DrvSclDmaRwModeType_e enRWMode;
    enRWMode = _MDrvSclDmaSwitchIDForDriverlayer(enSCLDMA_ID,enMemType,&enID,bReadDMAMode);
    DrvSclDmaSetForceCloseDma(enID,enRWMode,bEnForceClose);
}
void MDrvSclDmaResetTrigCountByClient
    (MDrvSclDmaIdType_e enSCLDMA_ID,MDrvSclDmaMemType_e enMemType,bool bReadDMAMode)
{
    DrvSclDmaIdType_e enID;
    DrvSclDmaRwModeType_e enRWMode;
    enRWMode = _MDrvSclDmaSwitchIDForDriverlayer(enSCLDMA_ID,enMemType,&enID,bReadDMAMode);
    DrvSclDmaResetTrigCountByClient(enID,enRWMode);
}

void MDrvSclDmaResetTrigCountAllClient(void)
{
    DrvSclDmaResetTrigCountByClient(E_DRV_SCLDMA_ID_MAX,E_DRV_SCLDMA_RW_NUM);
}

void MDrvSclDmaClkClose(MDrvSclDmaClkConfig_t* stclk)
{
   DrvSclDmaClkClose((DrvSclDmaClkConfig_t *)stclk);
}
void * MDrvSclDmaGetWaitQueueHead(MDrvSclDmaIdType_e enSCLDMA_ID)
{
    DrvSclDmaIdType_e enID;

    enID =  enSCLDMA_ID == E_MDRV_SCLDMA_ID_1   ? E_DRV_SCLDMA_ID_1_W   :
            enSCLDMA_ID == E_MDRV_SCLDMA_ID_2   ? E_DRV_SCLDMA_ID_2_W   :
            enSCLDMA_ID == E_MDRV_SCLDMA_ID_3   ? E_DRV_SCLDMA_ID_3_W   :
                                                  E_DRV_SCLDMA_ID_MAX;
    if(enID == E_DRV_SCLDMA_ID_MAX)
    {
        return 0;
    }
    else
    {
        return DrvSclDmaGetWaitQueueHead(enID);
    }
}

u32 MDrvSclDmaGetWaitEvent(MDrvSclDmaIdType_e enSCLDMA_ID)
{
    DrvSclDmaIdType_e enID;

    enID =  enSCLDMA_ID == E_MDRV_SCLDMA_ID_1   ? E_DRV_SCLDMA_ID_1_W   :
            enSCLDMA_ID == E_MDRV_SCLDMA_ID_2   ? E_DRV_SCLDMA_ID_2_W   :
            enSCLDMA_ID == E_MDRV_SCLDMA_ID_3   ? E_DRV_SCLDMA_ID_3_W   :
                                                  E_DRV_SCLDMA_ID_MAX;
    if(enID == E_DRV_SCLDMA_ID_MAX)
    {
        return 0;
    }
    else
    {
        return DrvSclDmaGetWaitEvent(enID);
    }
}

MDrvSclDmaAttrType_t MDrvSclDmaGetDmaInformationByClient
    (MDrvSclDmaIdType_e enSCLDMA_ID,MDrvSclDmaMemType_e enMemType,bool bReadDMAMode)
{
    DrvSclDmaIdType_e enID;
    DrvSclDmaRwModeType_e enRWMode;
    DrvSclDmaAttrType_t stDrvDMACfg;
    MDrvSclDmaAttrType_t stSendToIOCfg;
    enRWMode = _MDrvSclDmaSwitchIDForDriverlayer(enSCLDMA_ID,enMemType,&enID,bReadDMAMode);
    stDrvDMACfg = DrvSclDmaGetDmaInformationByClient(enID, enRWMode);
    stSendToIOCfg = _MDrvSclDmaFillDmaInfoStruct(stDrvDMACfg);
    return stSendToIOCfg;
}
bool _MDrvSclDmaIsClosePacking(MDrvSclDmaAttrType_t stScldmaAttr)
{
    bool bEn = 0;
    if(stScldmaAttr.enColorType == E_MDRV_SCLDMA_COLOR_YUV422 )
    {
        bEn = 1;
    }
    else if(stScldmaAttr.enColorType == E_MDRV_SCLDMA_COLOR_YCSep422 ||
        stScldmaAttr.enColorType == E_MDRV_SCLDMA_COLOR_YUV420)
    {
        bEn = ((stScldmaAttr.u32Base_Y[0]+(stScldmaAttr.u16DMAH*stScldmaAttr.u16DMAV))==stScldmaAttr.u32Base_C[0]) ? 1 :0 ;
    }
    else if(stScldmaAttr.enColorType == E_MDRV_SCLDMA_COLOR_YUVSep422 ||
        stScldmaAttr.enColorType == E_MDRV_SCLDMA_COLOR_YUVSep420)
    {
        bEn = ((stScldmaAttr.u32Base_Y[0]+(stScldmaAttr.u16DMAH*stScldmaAttr.u16DMAV))==stScldmaAttr.u32Base_C[0]) ? 1 :0 ;
        if(stScldmaAttr.enColorType == E_MDRV_SCLDMA_COLOR_YUVSep422 && bEn)
        {
            bEn = ((stScldmaAttr.u32Base_C[0]+(stScldmaAttr.u16DMAH*stScldmaAttr.u16DMAV/2))==stScldmaAttr.u32Base_V[0]) ? 1 :0 ;
        }
        else if(stScldmaAttr.enColorType == E_MDRV_SCLDMA_COLOR_YUVSep420 && bEn)
        {
            bEn = ((stScldmaAttr.u32Base_C[0]+(stScldmaAttr.u16DMAH*stScldmaAttr.u16DMAV/4))==stScldmaAttr.u32Base_V[0]) ? 1 :0 ;
        }
    }
    return bEn;
}

#if defined (SCLOS_TYPE_LINUX_KERNEL)
ssize_t MDrvSclDmaProcShow
    (char *buf, MDrvSclDmaIdType_e enID, MDrvSclDmaMemType_e enMem, bool bRread)
{
    MDrvSclDmaAttrType_t stScldmaAttr;
    //out =0,in=1
    char *p8StrBuf = buf;
    char *p8StrEnd = buf + PAGE_SIZE;
    int u32idx;
    u8 u8bCP;
    //out =0,in=1
    stScldmaAttr = MDrvSclDmaGetDmaInformationByClient(enID, enMem,bRread);
    p8StrBuf += scnprintf(p8StrBuf, p8StrEnd - p8StrBuf
        , "========================SCL PROC FRAMEWORK======================\n");
    p8StrBuf += scnprintf(p8StrBuf, p8StrEnd - p8StrBuf
        , "------------------------ %s %s CLIENT----------------------\n",
        PARSING_SCLDMA_IOID(enID),PARSING_SCLDMA_IOMEM(enMem));
    p8StrBuf += scnprintf(p8StrBuf, p8StrEnd - p8StrBuf
        , "DMA Enable: %hhd\n",stScldmaAttr.bDMAEn);
    p8StrBuf += scnprintf(p8StrBuf, p8StrEnd - p8StrBuf
        , "output width: %hd, output height: %hd\n",stScldmaAttr.u16DMAH,stScldmaAttr.u16DMAV);
    p8StrBuf += scnprintf(p8StrBuf, p8StrEnd - p8StrBuf
        , "DMA color format: %s\n",PARSING_SCLDMA_IOCOLOR(stScldmaAttr.enColorType));
    p8StrBuf += scnprintf(p8StrBuf, p8StrEnd - p8StrBuf
        , "DMA trigger mode: %s\n",PARSING_SCLDMA_IOBUFMD(stScldmaAttr.enBufMDType));
    p8StrBuf += scnprintf(p8StrBuf, p8StrEnd - p8StrBuf
        , "DMA Buffer Num: %hd\n",stScldmaAttr.u16BufNum);
    if(stScldmaAttr.enColorType == E_MDRV_SCLDMA_COLOR_YUV422 ||
        stScldmaAttr.enColorType == E_MDRV_SCLDMA_COLOR_YUVSep422||
        stScldmaAttr.enColorType == E_MDRV_SCLDMA_COLOR_YCSep422)
    {
        p8StrBuf += scnprintf(p8StrBuf, p8StrEnd - p8StrBuf
            , "DMA Buffer length(presume): %d\n",(int)(stScldmaAttr.u16DMAH*stScldmaAttr.u16DMAV*2));
    }
    else
    {
        p8StrBuf += scnprintf(p8StrBuf, p8StrEnd - p8StrBuf
            , "DMA Buffer length(presume): %d\n",(int)(stScldmaAttr.u16DMAH*stScldmaAttr.u16DMAV*3/2));
    }
    for(u32idx=0 ;u32idx<stScldmaAttr.u16BufNum;u32idx++)
    {
        p8StrBuf += scnprintf(p8StrBuf, p8StrEnd - p8StrBuf
            , "DMA Buffer Y Address[%d]: 2%lx\n",u32idx,stScldmaAttr.u32Base_Y[u32idx]);
        p8StrBuf += scnprintf(p8StrBuf, p8StrEnd - p8StrBuf
            , "DMA Buffer C Address[%d]: 2%lx\n",u32idx,stScldmaAttr.u32Base_C[u32idx]);
        p8StrBuf += scnprintf(p8StrBuf, p8StrEnd - p8StrBuf
            , "DMA Buffer V Address[%d]: 2%lx\n",u32idx,stScldmaAttr.u32Base_V[u32idx]);
    }
    u8bCP = _MDrvSclDmaIsClosePacking(stScldmaAttr);
    p8StrBuf += scnprintf(p8StrBuf, p8StrEnd - p8StrBuf
        , "DMA Buffer Closed Packing: %hhx\n",u8bCP);
    p8StrBuf += scnprintf(p8StrBuf, p8StrEnd - p8StrBuf
        , "DMA Buffer Read Pointer: %hhx\n",stScldmaAttr.bDMAReadIdx);
    p8StrBuf += scnprintf(p8StrBuf, p8StrEnd - p8StrBuf
        , "DMA Buffer Write Pointer: %hhx\n",stScldmaAttr.bDMAWriteIdx);
    p8StrBuf += scnprintf(p8StrBuf, p8StrEnd - p8StrBuf
        , "DMA Buffer Count: %hhx\n",stScldmaAttr.u8Count);
    p8StrBuf += scnprintf(p8StrBuf, p8StrEnd - p8StrBuf
        , "DMA Buffer Status: %hhx\n",stScldmaAttr.bDMAFlag);
    p8StrBuf += scnprintf(p8StrBuf, p8StrEnd - p8StrBuf
        , "FrameDoneTime     : %lu\n",stScldmaAttr.u32FrameDoneTime);
    p8StrBuf += scnprintf(p8StrBuf, p8StrEnd - p8StrBuf
        , "ToGetPollTime     : %lu\n",stScldmaAttr.u32SendTime);
    if(stScldmaAttr.bSendPoll &0x10)
    {
        p8StrBuf += scnprintf(p8StrBuf, p8StrEnd - p8StrBuf
            , "Poll hold on, last Status: %hhx\n",(stScldmaAttr.bSendPoll&0xf));
    }
    else
    {
        p8StrBuf += scnprintf(p8StrBuf, p8StrEnd - p8StrBuf
            , "Poll on time, Status: %hhx\n",(stScldmaAttr.bSendPoll&0xf));
    }
    p8StrBuf += scnprintf(p8StrBuf, p8StrEnd - p8StrBuf
        , "output V line: %hd trig Count: %ld \n",stScldmaAttr.u16DMAcount,stScldmaAttr.u32Trigcount);
    p8StrBuf += scnprintf(p8StrBuf, p8StrEnd - p8StrBuf
        , "SWReTrig Count: %hhu \n",stScldmaAttr.u8ResetCount);
    p8StrBuf += scnprintf(p8StrBuf, p8StrEnd - p8StrBuf
        , "DMA ignore Count: %hhu \n",stScldmaAttr.u8DMAErrCount);
    p8StrBuf += scnprintf(p8StrBuf, p8StrEnd - p8StrBuf
        , "========================SCL PROC FRAMEWORK======================\n");
    return (p8StrBuf - buf);
}
#endif
