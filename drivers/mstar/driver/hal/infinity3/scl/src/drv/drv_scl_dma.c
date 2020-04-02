//<MStar Software>
//******************************************************************************
// MStar Software
// Copyright (c) 2010 - 2012 MStar Semiconductor, Inc. All rights reserved.
// All software, firmware and related documentation herein ("MStar Software") are
// intellectual property of MStar Semiconductor, Inc. ("MStar") and protected by
// law, including, but not limited to, copyright law and international treaties.
// Any use, modification, reproduction, retransmission, or republication of all
// or part of MStar Software is expressly prohibited, unless prior written
// permission has been granted by MStar.
//
// By accessing, browsing and/or using MStar Software, you acknowledge that you
// have read, understood, and agree, to be bound by below terms ("Terms") and to
// comply with all applicable laws and regulations:
//
// 1. MStar shall retain any and all right, ownership and interest to MStar
//    Software and any modification/derivatives thereof.
//    No right, ownership, or interest to MStar Software and any
//    modification/derivatives thereof is transferred to you under Terms.
//
// 2. You understand that MStar Software might include, incorporate or be
//    supplied together with third party`s software and the use of MStar
//    Software may require additional licenses from third parties.
//    Therefore, you hereby agree it is your sole responsibility to separately
//    obtain any and all third party right and license necessary for your use of
//    such third party`s software.
//
// 3. MStar Software and any modification/derivatives thereof shall be deemed as
//    MStar`s confidential information and you agree to keep MStar`s
//    confidential information in strictest confidence and not disclose to any
//    third party.
//
// 4. MStar Software is provided on an "AS IS" basis without warranties of any
//    kind. Any warranties are hereby expressly disclaimed by MStar, including
//    without limitation, any warranties of merchantability, non-infringement of
//    intellectual property rights, fitness for a particular purpose, error free
//    and in conformity with any international standard.  You agree to waive any
//    claim against MStar for any loss, damage, cost or expense that you may
//    incur related to your use of MStar Software.
//    In no event shall MStar be liable for any direct, indirect, incidental or
//    consequential damages, including without limitation, lost of profit or
//    revenues, lost or damage of data, and unauthorized system use.
//    You agree that this Section 4 shall still apply without being affected
//    even if MStar Software has been modified by MStar in accordance with your
//    request or instruction for your use, except otherwise agreed by both
//    parties in writing.
//
// 5. If requested, MStar may from time to time provide technical supports or
//    services in relation with MStar Software to you for your use of
//    MStar Software in conjunction with your or your customer`s product
//    ("Services").
//    You understand and agree that, except otherwise agreed by both parties in
//    writing, Services are provided on an "AS IS" basis and the warranty
//    disclaimer set forth in Section 4 above shall apply.
//
// 6. Nothing contained herein shall be construed as by implication, estoppels
//    or otherwise:
//    (a) conferring any license or right to use MStar name, trademark, service
//        mark, symbol or any other identification;
//    (b) obligating MStar or any of its affiliates to furnish any person,
//        including without limitation, you and your customers, any assistance
//        of any kind whatsoever, or any information; or
//    (c) conferring any license or right under any intellectual property right.
//
// 7. These terms shall be governed by and construed in accordance with the laws
//    of Taiwan, R.O.C., excluding its conflict of law rules.
//    Any and all dispute arising out hereof or related hereto shall be finally
//    settled by arbitration referred to the Chinese Arbitration Association,
//    Taipei in accordance with the ROC Arbitration Law and the Arbitration
//    Rules of the Association by three (3) arbitrators appointed in accordance
//    with the said Rules.
//    The place of arbitration shall be in Taipei, Taiwan and the language shall
//    be English.
//    The arbitration award shall be final and binding to both parties.
//
//******************************************************************************
//<MStar Software>
////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2008-2009 MStar Semiconductor, Inc.
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
#define DRV_SCLDMA_C

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "drv_scl_os.h"
#include "hal_scl_util.h"
#include "drv_scl_irq_st.h"
#include "drv_scl_irq.h"
#include "drv_scl_dma_st.h"
#include "hal_scl_dma.h"
#include "drv_scl_dma.h"
#include "drv_scl_cmdq.h" //add later
#include "hal_scl_reg.h"
#include "drv_scl_dbg.h"

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define DRV_SCLDMA_DBG(x)
#define DRV_SCLDMA_DBG_H(x)
#define DRV_SCLDMA_ERR(x)     x
#define FHD_Width   1920
#define FHD_Height  1080
#define HD_Width    1280
#define HD_Height   720
#define PNL_Width   800
#define PNL_Height  480
#define SINGLE_SKIP 0xF
#define FRM_POLLIN  0x1
#define SNP_POLLIN  0x2
#define FRM2_POLLIN  0x2
#define SCL_IMIinitAddr 0x14000

#define SINGLE_BUFF_ACTIVE_TIMIEOUT      100
#define _Is_SWRingModeBufferNotReady(enClientType)  (gstScldmaBufferQueue[enClientType].bUsed ==0)
#define _Is_SWRingModeBufferReady(enClientType)  (gstScldmaBufferQueue[enClientType].bUsed)
#define _Is_SWRingModeBufferNotFull(enClientType)   (!gstScldmaBufferQueue[enClientType].bFull)
#define _Is_SWRingModeBufferFull(enClientType)   (gstScldmaBufferQueue[enClientType].bFull)
#define _Is_RingMode(enClientType)                  (gstScldmaInfo.enBuffMode[(enClientType)] == E_DRV_SCLDMA_BUF_MD_RING)
#define _Is_SingleMode(enClientType)                (gstScldmaInfo.enBuffMode[(enClientType)] == E_DRV_SCLDMA_BUF_MD_SINGLE)
#define _Is_SWRingMode(enClientType)            (gstScldmaInfo.enBuffMode[(enClientType)] == E_DRV_SCLDMA_BUF_MD_SWRING)
#define _Is_DMACanReOpen(bReOpen,bRpchange) ((bReOpen)&&(bRpchange))
#define _Is_DMAClientOn(enClientType)           (gstScldmaInfo.bDMAOnOff[enClientType])
#define _Is_DMAClientOff(enClientType)           (!gstScldmaInfo.bDMAOnOff[enClientType])
#define _IsQueueWriteLargeRead(enClientType) \
    (gstScldmaBufferQueue[enClientType].pstWrite > gstScldmaBufferQueue[enClientType].pstRead)
#define _IsQueueWriteSmallRead(enClientType) \
        (gstScldmaBufferQueue[enClientType].pstWrite < gstScldmaBufferQueue[enClientType].pstRead)
#define _GetInQueueCountIfLarge(enClientType)\
        gstScldmaBufferQueue[enClientType].u8InQueueCount =\
    (gstScldmaBufferQueue[enClientType].pstWrite - gstScldmaBufferQueue[enClientType].pstRead)/1;
#define _GetInQueueCountIfSmall(enClientType)\
        gstScldmaBufferQueue[enClientType].u8InQueueCount =\
    ((gstScldmaBufferQueue[enClientType].pstWrite- gstScldmaBufferQueue[enClientType].pstHead)+\
    (gstScldmaBufferQueue[enClientType].pstTail- gstScldmaBufferQueue[enClientType].pstRead))\
    /1;
#define _Is_IdxRingCircuit(enClientType,idx) (idx == (gstScldmaInfo.bMaxid[enClientType]-1))

#define _Is_SC3Singlemode(enClientType) (_Is_SingleMode(enClientType) && \
    (enClientType == E_DRV_SCLDMA_3_FRM_R || enClientType == E_DRV_SCLDMA_3_FRM_W))
#define _Is_SC3Ringmode(enClientType) (_Is_RingMode(enClientType) && \
        (enClientType == E_DRV_SCLDMA_3_FRM_R || enClientType == E_DRV_SCLDMA_3_FRM_W))
#define _Is_SC3SWRingmode(enClientType) (_Is_SWRingMode(enClientType) && \
            (enClientType == E_DRV_SCLDMA_3_FRM_R || enClientType == E_DRV_SCLDMA_3_FRM_W))

#define _Is_OnlySC1SNPSingleDone(u32Event) (u32Event == E_DRV_SCLIRQ_EVENT_ISTSC1SNP &&_Is_SingleMode(E_DRV_SCLDMA_1_SNP_W))
#define _Is_OnlySC1FRMSingleDone(u32Event) (u32Event == E_DRV_SCLIRQ_EVENT_ISTSC1FRM &&_Is_SingleMode(E_DRV_SCLDMA_1_FRM_W))
#define _Is_OnlySC2FRMSingleDone(u32Event) (u32Event == E_DRV_SCLIRQ_EVENT_ISTSC2FRM &&_Is_SingleMode(E_DRV_SCLDMA_2_FRM_W))
#define _Is_VsrcId(enSCLDMA_ID) ((enSCLDMA_ID ==E_DRV_SCLDMA_ID_1_W)||(enSCLDMA_ID ==E_DRV_SCLDMA_ID_2_W))
#define _Is_VsrcDoubleBufferNotOpen() (gVsrcDBnotOpen)
#define _Is_VsrcDoubleBufferOpen() (!gVsrcDBnotOpen)
#define _ResetTrigCount(enClientType) (gu32TrigCount[enClientType]--)
#define SCLDMA_SIZE_ALIGN(x, align)                 ((x+align) & ~(align-1))
#define SCLDMA_CHECK_ALIGN(x, align)                (x & (align-1))

#define DRV_SCLDMA_MUTEX_LOCK()            DrvSclOsObtainMutex(_SCLDMA_Mutex,SCLOS_WAIT_FOREVER)
#define DRV_SCLDMA_MUTEX_UNLOCK()          DrvSclOsReleaseMutex(_SCLDMA_Mutex)
#define DRV_SCLDMA_MUTEX_LOCK_ISR()        DrvSclOsObtainMutexIrq(_SCLIRQ_SCLDMA_Mutex);
#define DRV_SCLDMA_MUTEX_UNLOCK_ISR()      DrvSclOsReleaseMutexIrq(_SCLIRQ_SCLDMA_Mutex);

#define PARSING_SCLDMA_ID(x)           (x==E_DRV_SCLDMA_ID_1_W   ? "SCLDMA_1_W" : \
                                        x==E_DRV_SCLDMA_ID_2_W   ? "SCLDMA_2_W" : \
                                        x==E_DRV_SCLDMA_ID_3_W   ? "SCLDMA_3_W" : \
                                        x==E_DRV_SCLDMA_ID_3_R   ? "SCLDMA_3_R" : \
                                        x==E_DRV_SCLDMA_ID_PNL_R ? "SCLDMA_PNL_R" : \
                                                               "UNKNOWN")



#define PARSING_SCLDMA_RWMD(x)        (x==E_DRV_SCLDMA_FRM_W ? "FRM_W" : \
                                       x==E_DRV_SCLDMA_SNP_W ? "SNP_W" : \
                                       x==E_DRV_SCLDMA_IMI_W ? "IMI_W" : \
                                       x==E_DRV_SCLDMA_FRM_R ? "FRM_R" : \
                                       x==E_DRV_SCLDMA_DBG_R ? "DBG_R" : \
                                       x==E_DRV_SCLDMA_FRM2_W ? "FRM2_W" : \
                                                           "UNKNOWN")

#define PARSING_SCLDMA_BUFMD(x)       (x==E_DRV_SCLDMA_BUF_MD_RING   ? "RING" : \
                                       x==E_DRV_SCLDMA_BUF_MD_SWRING ? "SWRING" : \
                                       x==E_DRV_SCLDMA_BUF_MD_SINGLE ? "SINGLE" : \
                                                                   "UNKNOWN")

#define PARSING_SCLDMA_COLOR(x)       (x==E_DRV_SCLDMA_COLOR_YUV422? "422PACK" : \
                                       x==E_DRV_SCLDMA_COLOR_YUV420 ? "YCSep420" : \
                                       x==E_DRV_SCLDMA_COLOR_YCSep422 ? "YCSep422" : \
                                       x==E_DRV_SCLDMA_COLOR_YUVSep422 ? "YUVSep422" : \
                                       x==E_DRV_SCLDMA_COLOR_YUVSep420 ? "YUVSep420" : \
                                                                  "UNKNOWN")

#define PARSING_SCLDMA_CLIENT(x)      (x==E_DRV_SCLDMA_1_FRM_W ? "E_DRV_SCLDMA_1_FRM_W" : \
                                       x==E_DRV_SCLDMA_1_SNP_W ? "E_DRV_SCLDMA_1_SNP_W" : \
                                       x==E_DRV_SCLDMA_1_IMI_W ? "E_DRV_SCLDMA_1_IMI_W" : \
                                       x==E_DRV_SCLDMA_2_FRM_W ? "E_DRV_SCLDMA_2_FRM_W" : \
                                       x==E_DRV_SCLDMA_2_FRM2_W ? "E_DRV_SCLDMA_2_FRM2_W" : \
                                       x==E_DRV_SCLDMA_2_IMI_W ? "E_DRV_SCLDMA_2_IMI_W" : \
                                       x==E_DRV_SCLDMA_3_FRM_W ? "E_DRV_SCLDMA_3_FRM_W" : \
                                       x==E_DRV_SCLDMA_3_FRM_R ? "E_DRV_SCLDMA_3_FRM_R" : \
                                       x==E_DRV_SCLDMA_4_FRM_R ? "E_DRV_SCLDMA_4_FRM_R" : \
                                                             "UNKNOWN")

#define PARSING_SCLDMA_ISR_LOG(x)       (x==E_DRV_SCLDMA_ISR_LOG_ISPOFF   ? "ISPOFF" : \
                                        x==E_DRV_SCLDMA_ISR_LOG_SNPONLY   ? "SNPONLY" : \
                                        x==E_DRV_SCLDMA_ISR_LOG_SNPISR   ? "SNPISR" : \
                                        x==E_DRV_SCLDMA_ISR_LOG_ISPON   ? "ISPON" : \
                                        x==E_DRV_SCLDMA_ISR_LOG_SC1ON ? "SC1ON" : \
                                        x==E_DRV_SCLDMA_ISR_LOG_SC1OFF ? "SC1OFF" : \
                                        x==E_DRV_SCLDMA_ISR_LOG_SC2ON ? "SC2ON" : \
                                        x==E_DRV_SCLDMA_ISR_LOG_SC2OFF ? "SC2OFF" : \
                                        x==E_DRV_SCLDMA_ISR_LOG_SC1A ? "SC1A" : \
                                        x==E_DRV_SCLDMA_ISR_LOG_SC1N ? "SC1N" : \
                                        x==E_DRV_SCLDMA_ISR_LOG_SC2A ? "SC2A" : \
                                        x==E_DRV_SCLDMA_ISR_LOG_SC2N ? "SC2N" : \
                                        x==E_DRV_SCLDMA_ISR_LOG_FRMEND ? "FRMEND" : \
                                                               "UNKNOWN")

#define OMX_VSPL_POLLTIME 80*1000

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
/////////////////
/// _SCLDMA_Mutex
/// use in scldma mutex,not include isr
////////////////
s32 _SCLDMA_Mutex        = -1;

/////////////////
/// _SCLIRQ_SCLDMA_Mutex
/// scldma and sclirq mutex, include isr
////////////////
s32 _SCLIRQ_SCLDMA_Mutex = -1;

/////////////////
/// gVsrcDBnotOpen
/// if True ,DMA switch Double buffer not open
////////////////
bool gVsrcDBnotOpen;

/////////////////
/// gu32FRMEvents
/// used to save DMA event for diffenent client
////////////////

/////////////////
/// gu8ISPcount
/// save frame count from ISP_hw - scl_delay
////////////////
u8 gu8ISPcount;

/////////////////
/// gu64FRMDoneTime
/// save frame done time.
////////////////
u64 gu64FRMDoneTime[E_DRV_SCLDMA_CLIENT_NUM][MAX_BUFFER_COUNT+1];
u32 gu32SendTime[E_DRV_SCLDMA_CLIENT_NUM];
u32 gbSendPoll[E_DRV_SCLDMA_CLIENT_NUM];

/////////////////
/// gu32TrigCount
/// save dma trig off times.
////////////////
u32 gu32TrigCount[E_DRV_SCLDMA_CLIENT_NUM];
bool gbForceClose[E_DRV_SCLDMA_CLIENT_NUM];
bool gu8ResetCount[E_DRV_SCLDMA_CLIENT_NUM];
bool gu8DMAErrCount[E_DRV_SCLDMA_CLIENT_NUM];
DrvSclDmaInfoType_t  gstScldmaInfo;
DrvSclDmaBufferQueueConfig_t gstScldmaBufferQueue[E_DRV_SCLDMA_CLIENT_NUM];

bool gbScldmaSuspend;
bool gbDBStatus;
#if SCLDMA_IRQ_EN
DrvSclDmaIrqModeType_e genIrqMode[E_DRV_SCLDMA_CLIENT_NUM];
#endif
//-------------------------------------------------------------------------------------------------
//  Functions/
//-------------------------------------------------------------------------------------------------
void DrvSclDmaSetThreadOnOffFlag(bool bEn,DrvSclDmaThreadConfig_t *stthcfg)
{
    stthcfg->flag = bEn;
}
void _Drv_SCLDMA_SC1OnOff(DrvSclDmaRwModeType_e enRW,bool bEn)
{
    HalSclDmaSetSC1HandshakeForce(enRW, bEn);
    HalSclDmaSetSC1DMAEn(enRW, bEn);
}
void _Drv_SCLDMA_SC1OnOffWithoutDoubleBuffer
    (DrvSclDmaRwModeType_e enRW,bool bEn,DrvSclDmaClientType_e enClientType)
{
    if(DrvSclIrqGetIsBlankingRegion())
    {
        _Drv_SCLDMA_SC1OnOff(enRW, bEn);
        SCL_RTKDBG(0,"[SC1ON]blank @:%lu\n",DrvSclOsGetSystemTimeStamp());
    }
    else
    {
        if(bEn)
        {
            DRV_SCLDMA_MUTEX_LOCK_ISR();
            _SetFlagType(enClientType,E_DRV_SCLDMA_FLAG_NEXT_ON);
            _ReSetFlagType(enClientType,E_DRV_SCLDMA_FLAG_NEXT_OFF);
            DRV_SCLDMA_MUTEX_UNLOCK_ISR();
        }
        else
        {
            DRV_SCLDMA_MUTEX_LOCK_ISR();
            _SetFlagType(enClientType,E_DRV_SCLDMA_FLAG_NEXT_OFF);
            _ReSetFlagType(enClientType,E_DRV_SCLDMA_FLAG_NEXT_ON);
            DRV_SCLDMA_MUTEX_UNLOCK_ISR();
        }
        SCL_RTKDBG(0,"[SC1ON]wait for blanking@:%lu\n",DrvSclOsGetSystemTimeStamp());
        SCL_DBG(SCL_DBG_LV_DRVSCLDMA()&(Get_DBGMG_SCLDMAclient(enClientType,1)),
            "[DRMSCLDMA]%s %d wait for blanking\n",
            __FUNCTION__,enClientType);
    }
}
void _Drv_SCLDMA_SC2OnOff(DrvSclDmaRwModeType_e enRW,bool bEn)
{
    HalSclDmaSetSC2HandshakeForce(enRW, bEn);
    HalSclDmaSetSC1ToSC2HandshakeForce(bEn);
    HalSclDmaSetSC2DMAEn(enRW, bEn);
}
void _Drv_SCLDMA_SC2OnOffWithoutDoubleBuffer
    (DrvSclDmaRwModeType_e enRW,bool bEn,DrvSclDmaClientType_e enClientType)
{
    if(DrvSclIrqGetIsBlankingRegion())
    {
        _Drv_SCLDMA_SC2OnOff(enRW, bEn);
    }
    else
    {
        if(bEn)
        {
            DRV_SCLDMA_MUTEX_LOCK_ISR();
            _SetFlagType(enClientType,E_DRV_SCLDMA_FLAG_NEXT_ON);
            _ReSetFlagType(enClientType,E_DRV_SCLDMA_FLAG_NEXT_OFF);
            DRV_SCLDMA_MUTEX_UNLOCK_ISR();
        }
        else
        {
            DRV_SCLDMA_MUTEX_LOCK_ISR();
            _SetFlagType(enClientType,E_DRV_SCLDMA_FLAG_NEXT_OFF);
            _ReSetFlagType(enClientType,E_DRV_SCLDMA_FLAG_NEXT_ON);
            DRV_SCLDMA_MUTEX_UNLOCK_ISR();
        }
        SCL_DBG(SCL_DBG_LV_DRVSCLDMA()&(Get_DBGMG_SCLDMAclient(enClientType,1)),
            "[DRMSCLDMA]%s %d wait for blanking\n",
            __FUNCTION__,enClientType);
    }
}
void _Drv_SCLDMA_SWRegenVSyncTrigger(DrvSclDmaVsIdType_e enIDType)
{
    HalSclDmaTrigRegenVSync(enIDType,1);
}
void _Drv_SCLDMA_SetVsyncRegenMode(DrvSclDmaVsIdType_e enIDType, DrvSclDmaVsTrigModeType_e enTrigType)
{
    HalSclDmaSetVSyncRegenMode(enIDType,enTrigType);
}
void _Drv_SCLDMA_SetVsyncTrigConfig(DrvSclDmaVsIdType_e enIDType)
{
    //hard code by hw setting
    HalSclDmaSetRegenVSyncStartPoint(enIDType,10);
    HalSclDmaSetRegenVSyncWidth(enIDType,40);
}
void _Drv_SCLDMA_ResetGlobalParameter(void)
{
    u8 u8ClientIdx;
    u8 y;
    DRV_SCLDMA_MUTEX_LOCK();
    for(u8ClientIdx=0; u8ClientIdx<E_DRV_SCLDMA_VS_ID_NUM; u8ClientIdx++)
    {
        gstScldmaInfo.stVsCfg[u8ClientIdx].enTrigMd = E_DRV_SCLDMA_VS_TRIG_MODE_HW_IN_VSYNC;
        for(y =0;y<=MAX_BUFFER_COUNT;y++)
        {
            gu64FRMDoneTime[u8ClientIdx][y] = 0;
        }
        gu8ResetCount[u8ClientIdx] = 0;
        gu8DMAErrCount[u8ClientIdx] = 0;
    }
    gVsrcDBnotOpen = 1;
    gu8ISPcount = 0;
    gbScldmaSuspend = 0;
    DRV_SCLDMA_MUTEX_UNLOCK();
}
void _Drv_SCLDMA_ResetGlobalParameterByClient(DrvSclDmaClientType_e u8ClientIdx)
{
    u8 u8BufferIdx;
    DRV_SCLDMA_MUTEX_LOCK_ISR();
    if(u8ClientIdx<E_DRV_SCLDMA_3_FRM_R)
    {
        gstScldmaInfo.bDMAidx[u8ClientIdx]    = (E_DRV_SCLDMA_ACTIVE_BUFFER_SCL|E_DRV_SCLDMA_ACTIVE_BUFFER_OMX_RINGFULL);
    }
    else
    {
        gstScldmaInfo.bDMAidx[u8ClientIdx]    = (E_DRV_SCLDMA_ACTIVE_BUFFER_SCL|0x50);
    }
    gstScldmaInfo.bDMAOnOff[u8ClientIdx]  = 0;
    gstScldmaInfo.bDmaflag[u8ClientIdx]   = E_DRV_SCLDMA_FLAG_BLANKING;
    if(_Is_SWRingModeBufferReady(u8ClientIdx))
    {
        if(gstScldmaBufferQueue[u8ClientIdx].pstHead)
        {
            DrvSclOsVirMemFree(gstScldmaBufferQueue[u8ClientIdx].pstHead);
        }
        gstScldmaBufferQueue[u8ClientIdx].bFull = 0;
        gstScldmaBufferQueue[u8ClientIdx].bUsed = 0;
        gstScldmaBufferQueue[u8ClientIdx].enID = 0;
        gstScldmaBufferQueue[u8ClientIdx].enRWMode = 0;
        gstScldmaBufferQueue[u8ClientIdx].pstHead = 0;
        gstScldmaBufferQueue[u8ClientIdx].pstRead = 0;
        gstScldmaBufferQueue[u8ClientIdx].pstTail = 0;
        gstScldmaBufferQueue[u8ClientIdx].pstWrite = 0;
        gstScldmaBufferQueue[u8ClientIdx].pstWriteAlready = 0;
        gstScldmaBufferQueue[u8ClientIdx].u8Bufferflag = 0;
        gstScldmaBufferQueue[u8ClientIdx].u8InQueueCount = 0;
        gstScldmaBufferQueue[u8ClientIdx].u8NextActiveId = 0;
    }
    DRV_SCLDMA_MUTEX_UNLOCK_ISR();
    DRV_SCLDMA_MUTEX_LOCK();
    gstScldmaInfo.enBuffMode[u8ClientIdx] = E_DRV_SCLDMA_BUF_MD_NUM;
    gstScldmaInfo.bMaxid[u8ClientIdx]     = 0;
    gstScldmaInfo.enColor[u8ClientIdx]    = 0;
    for(u8BufferIdx=0;u8BufferIdx<4;u8BufferIdx++)
    {
        gstScldmaInfo.u32Base_Y[u8ClientIdx][u8BufferIdx]     = 0;
        gstScldmaInfo.u32Base_C[u8ClientIdx][u8BufferIdx]     = 0;
        gstScldmaInfo.u32Base_V[u8ClientIdx][u8BufferIdx]     = 0;
    }
    gu32TrigCount[u8ClientIdx]             = 0;
    DRV_SCLDMA_MUTEX_UNLOCK();
}

static void _DrvSclDmaInitVariable(void)
{
    u8 u8ClientIdx;
    DrvSclOsMemset(&gstScldmaInfo, 0, sizeof(DrvSclDmaInfoType_t));
    DrvSclOsMemset(&gstScldmaBufferQueue, 0, sizeof(DrvSclDmaBufferQueueConfig_t)*E_DRV_SCLDMA_CLIENT_NUM);
    _Drv_SCLDMA_ResetGlobalParameter();
    gbDBStatus = DoubleBufferDefaultSet;
    for(u8ClientIdx=0; u8ClientIdx<E_DRV_SCLDMA_CLIENT_NUM; u8ClientIdx++)
    {
        _Drv_SCLDMA_ResetGlobalParameterByClient(u8ClientIdx);
        gbForceClose[u8ClientIdx]                 = 0;
        #if SCLDMA_IRQ_EN
            genIrqMode[u8ClientIdx]              = E_SCLDMA_IRQ_MODE_PENDING;
        #endif
    }
}


static DrvSclDmaClientType_e _Drv_SCLDMA_TransToClientType
    (DrvSclDmaIdType_e enSCLDMA_ID, DrvSclDmaRwModeType_e enRWMode)
{
    DrvSclDmaClientType_e enClientType;

    if(enSCLDMA_ID == E_DRV_SCLDMA_ID_1_W)
    {
        enClientType = enRWMode == E_DRV_SCLDMA_FRM_W ? E_DRV_SCLDMA_1_FRM_W :
                       enRWMode == E_DRV_SCLDMA_SNP_W ? E_DRV_SCLDMA_1_SNP_W :
                       enRWMode == E_DRV_SCLDMA_IMI_W ? E_DRV_SCLDMA_1_IMI_W :
                                                    E_DRV_SCLDMA_CLIENT_NUM;

    }
    else if(enSCLDMA_ID == E_DRV_SCLDMA_ID_2_W)
    {
        enClientType = enRWMode == E_DRV_SCLDMA_FRM_W ? E_DRV_SCLDMA_2_FRM_W :
                       enRWMode == E_DRV_SCLDMA_FRM2_W ? E_DRV_SCLDMA_2_FRM2_W :
                       enRWMode == E_DRV_SCLDMA_IMI_W ? E_DRV_SCLDMA_2_IMI_W :
                                                    E_DRV_SCLDMA_CLIENT_NUM;
    }
    else if(enSCLDMA_ID == E_DRV_SCLDMA_ID_3_R)
    {
        enClientType = enRWMode == E_DRV_SCLDMA_FRM_R ? E_DRV_SCLDMA_3_FRM_R :
                       enRWMode == E_DRV_SCLDMA_IMI_R ? E_DRV_SCLDMA_3_IMI_R :
                                                    E_DRV_SCLDMA_CLIENT_NUM;
    }
    else if(enSCLDMA_ID == E_DRV_SCLDMA_ID_3_W)
    {
        enClientType = enRWMode == E_DRV_SCLDMA_FRM_W ? E_DRV_SCLDMA_3_FRM_W :
                                                    E_DRV_SCLDMA_CLIENT_NUM;

    }
    else if(enSCLDMA_ID == E_DRV_SCLDMA_ID_PNL_R)
    {
        enClientType = enRWMode == E_DRV_SCLDMA_DBG_R ? E_DRV_SCLDMA_4_FRM_R :
                                                    E_DRV_SCLDMA_CLIENT_NUM;
    }
    else
    {
        enClientType = enRWMode == E_DRV_SCLDMA_FRM_R ? E_DRV_SCLDMA_4_FRM_R :
                                                    E_DRV_SCLDMA_CLIENT_NUM;
    }

    return enClientType;
}

static void _Drv_SCLDMA_SetDoubleBufferOn
    (DrvSclDmaIdType_e enSCLDMA_ID, u16 u16IrqNum, DrvSclDmaClientType_e enClientType)
{
    u32 u32Time;
    u64 u64Active = 0;
    if((enClientType == E_DRV_SCLDMA_1_IMI_W))
    {
        DrvSclIrqGetFlag(u16IrqNum, &u64Active);
        if(!u64Active)
        {
            DrvSclIrqGetFlag(u16IrqNum+1, &u64Active);
            if(u64Active)
            {
                DrvSclIrqSetClear(u16IrqNum+1);  // clear xxxX_ACTIVE_N
            }
        }

        u32Time = ((u32)DrvSclOsGetSystemTimeStamp());
        while(DrvSclOsTimerDiffTimeFromNow(u32Time) < 100)
        {
            DrvSclIrqGetFlag(u16IrqNum, &u64Active);
            if(u64Active)
            {
                DrvSclDmaDoubleBufferOnOffById(TRUE,enSCLDMA_ID);
                break;
            }
        }
#if ENABLE_RING_DB
        if(u64Active == 0)
        {
            gVsrcDBnotOpen = 1;
        }
#endif
    }
    SCL_DBG(SCL_DBG_LV_DRVSCLDMA()&(Get_DBGMG_SCLDMAclient(enClientType,1)), "[DRMSCLDMA]%s DiffTimeFromNow:%ld DmaId:%d\n",
        __FUNCTION__,DrvSclOsTimerDiffTimeFromNow(u32Time),enSCLDMA_ID);
}

void _Drv_SCLDMA_HWInitProcess(void)
{
    //_DrvSclDmaInit_FHD();
    SCL_DBG(SCL_DBG_LV_DRVSCLDMA()&EN_DBGMG_SCLDMALEVEL_ELSE, "[DRMSCLDMA]%s \n",__FUNCTION__);
    HalSclDmaHWInit();
    _Drv_SCLDMA_SetVsyncRegenMode(E_DRV_SCLDMA_VS_ID_SC3, E_DRV_SCLDMA_VS_TRIG_MODE_HW_IN_VSYNC);
    _Drv_SCLDMA_SetVsyncRegenMode(E_DRV_SCLDMA_VS_ID_SC, E_DRV_SCLDMA_VS_TRIG_MODE_HW_IN_VSYNC);
    _Drv_SCLDMA_SetVsyncRegenMode(E_DRV_SCLDMA_VS_ID_AFF, E_DRV_SCLDMA_VS_TRIG_MODE_HW_IN_VSYNC);
    _Drv_SCLDMA_SetVsyncRegenMode(E_DRV_SCLDMA_VS_ID_LDC, E_DRV_SCLDMA_VS_TRIG_MODE_HW_IN_VSYNC);
    _Drv_SCLDMA_SetVsyncRegenMode(E_DRV_SCLDMA_VS_ID_DISP, E_DRV_SCLDMA_VS_TRIG_MODE_HW_IN_VSYNC);
    HalSclDmaSetRegenVSyncVariableWidthEn(FALSE);

    HalSclDmaSetSC1HandshakeForce(E_DRV_SCLDMA_FRM_W, FALSE);
    HalSclDmaSetSC1HandshakeForce(E_DRV_SCLDMA_SNP_W, FALSE);
    HalSclDmaSetSC1HandshakeForce(E_DRV_SCLDMA_IMI_W, FALSE);
    HalSclDmaSetSC2HandshakeForce(E_DRV_SCLDMA_FRM_W, FALSE);
    HalSclDmaSetSC2HandshakeForce(E_DRV_SCLDMA_FRM2_W, FALSE);
    HalSclDmaSetSC2HandshakeForce(E_DRV_SCLDMA_IMI_W, FALSE);
    HalSclDmaSetSC1ToSC2HandshakeForce(FALSE);
    HalSclDmaSetCheckFrmEndSignal(E_DRV_SCLDMA_2_IMI_W, TRUE);
    HalSclDmaSetCheckFrmEndSignal(E_DRV_SCLDMA_1_IMI_W, TRUE);
    HalSclDmaSetCheckFrmEndSignal(E_DRV_SCLDMA_2_FRM_W, TRUE);
    HalSclDmaSetCheckFrmEndSignal(E_DRV_SCLDMA_1_FRM_W, TRUE);
    HalSclDmaSetCheckFrmEndSignal(E_DRV_SCLDMA_1_SNP_W, TRUE);
    HalSclDmaSetCheckFrmEndSignal(E_DRV_SCLDMA_3_FRM_W, TRUE);
    HalSclDmaSetCheckFrmEndSignal(E_DRV_SCLDMA_2_FRM2_W, TRUE);
    DrvSclIrqInterruptEnable(SCLIRQ_AFF_FULL);
    DrvSclIrqInterruptEnable(SCLIRQ_SC_IN_FRM_END);
    DrvSclIrqInterruptEnable(SCLIRQ_SC3_ENG_FRM_END);
    DrvSclIrqInterruptEnable(SCLIRQ_SC1_ENG_FRM_END);
    DrvSclIrqInterruptEnable(SCLIRQ_DISP_DMA_END);
    //DrvSclIrqInterruptEnable(SCLIRQ_SC1_HVSP_FINISH);
    DrvSclIrqInterruptEnable(SCLIRQ_SC1_FRM_W_ACTIVE);
    DrvSclIrqInterruptEnable(SCLIRQ_SC1_FRM_W_ACTIVE_N);
    DrvSclIrqInterruptEnable(SCLIRQ_SC1_SNP_W_ACTIVE);
    DrvSclIrqInterruptEnable(SCLIRQ_SC1_SNP_W_ACTIVE_N);
    DrvSclIrqInterruptEnable(SCLIRQ_SC1_SNPI_W_ACTIVE);
    DrvSclIrqInterruptEnable(SCLIRQ_SC1_SNPI_W_ACTIVE_N);
    DrvSclIrqInterruptEnable(SCLIRQ_SC1_DBG_R_ACTIVE);
    DrvSclIrqInterruptEnable(SCLIRQ_SC1_DBG_R_ACTIVE_N);
    DrvSclIrqInterruptEnable(SCLIRQ_SC2_FRM_W_ACTIVE);
    DrvSclIrqInterruptEnable(SCLIRQ_SC2_FRM_W_ACTIVE_N);
    DrvSclIrqInterruptEnable(SCLIRQ_SC2_FRM2_W_ACTIVE);
    DrvSclIrqInterruptEnable(SCLIRQ_SC2_FRM2_W_ACTIVE_N);
    DrvSclIrqInterruptEnable(SCLIRQ_SC2_FRMI_W_ACTIVE);
    DrvSclIrqInterruptEnable(SCLIRQ_SC2_FRMI_W_ACTIVE_N);
    DrvSclIrqInterruptEnable(SCLIRQ_SC3_DMA_R_ACTIVE);
    DrvSclIrqInterruptEnable(SCLIRQ_SC3_DMA_R_ACTIVE_N);
    DrvSclIrqInterruptEnable(SCLIRQ_SC3_DMA_W_ACTIVE);
    DrvSclIrqInterruptEnable(SCLIRQ_SC3_DMA_W_ACTIVE_N);

}
s32* DrvSclDmaGetDmaandIRQCommonMutex(void)
{
    return &_SCLIRQ_SCLDMA_Mutex;
}

void _Drv_SCLDMA_SetSuspendFlagByClient(DrvSclDmaClientType_e enClientType)
{
    if(_Is_RingMode(enClientType))
    {
        if(_Is_DMAClientOn(enClientType))
        {
            DrvSclDmaSetISRHandlerDmaOff(enClientType,0);
            DRV_SCLDMA_MUTEX_LOCK_ISR();
            _SetFlagType(enClientType,E_DRV_SCLDMA_FLAG_EVERDMAON);
            DRV_SCLDMA_MUTEX_UNLOCK_ISR();
        }
        DRV_SCLDMA_MUTEX_LOCK_ISR();
        _ReSetFlagType(enClientType,E_DRV_SCLDMA_FLAG_ACTIVE);
        _SetFlagType(enClientType,E_DRV_SCLDMA_FLAG_DMAOFF);
        DRV_SCLDMA_MUTEX_UNLOCK_ISR();
    }
    else
    {
        DRV_SCLDMA_MUTEX_LOCK_ISR();
        gstScldmaInfo.bDMAOnOff[enClientType] = FALSE;
        _SetANDGetFlagType(enClientType,E_DRV_SCLDMA_FLAG_FRMDONE,(~E_DRV_SCLDMA_FLAG_ACTIVE));
        DRV_SCLDMA_MUTEX_UNLOCK_ISR();
    }
}

bool DrvSclDmaSuspend(DrvSclDmaIdType_e enSCLDMA_ID, DrvSclDmaSuspendResumeConfig_t *pCfg)
{
    DrvSclIrqSuspendResumeConfig_t stSclIrqCfg;
    u8 u8Clientidx;
    bool bRet = TRUE;
    bool bAllClientOn = 0;
    SCL_DBG(SCL_DBG_LV_DRVSCLDMA()&EN_DBGMG_SCLDMALEVEL_ELSE, "[DRVSCLDMA]%s(%d), ID:%s(%d), bSuspend=%d\n",
        __FUNCTION__, __LINE__,PARSING_SCLDMA_ID(enSCLDMA_ID), enSCLDMA_ID,gbScldmaSuspend);

    switch(enSCLDMA_ID)
    {
        case E_DRV_SCLDMA_ID_1_W:
            _Drv_SCLDMA_SetSuspendFlagByClient(E_DRV_SCLDMA_1_FRM_W);
            _Drv_SCLDMA_SetSuspendFlagByClient(E_DRV_SCLDMA_1_SNP_W);
            _Drv_SCLDMA_SetSuspendFlagByClient(E_DRV_SCLDMA_1_IMI_W);
            break;

        case E_DRV_SCLDMA_ID_2_W:
            _Drv_SCLDMA_SetSuspendFlagByClient(E_DRV_SCLDMA_2_FRM_W);
            _Drv_SCLDMA_SetSuspendFlagByClient(E_DRV_SCLDMA_2_FRM2_W);
            _Drv_SCLDMA_SetSuspendFlagByClient(E_DRV_SCLDMA_2_IMI_W);
            break;

        case E_DRV_SCLDMA_ID_3_W:
        case E_DRV_SCLDMA_ID_3_R:
            _Drv_SCLDMA_SetSuspendFlagByClient(E_DRV_SCLDMA_3_FRM_W);
            _Drv_SCLDMA_SetSuspendFlagByClient(E_DRV_SCLDMA_3_FRM_R);
            break;

        case E_DRV_SCLDMA_ID_PNL_R:
            DRV_SCLDMA_MUTEX_LOCK_ISR();
                gstScldmaInfo.bDMAOnOff[E_DRV_SCLDMA_4_FRM_R] = FALSE;
            DRV_SCLDMA_MUTEX_UNLOCK_ISR();
            break;

        default:
            DRV_SCLDMA_ERR(sclprintf("[DRVSCLDMA]%s Suspend fail\n", __FUNCTION__));
            return FALSE;
    }

    for(u8Clientidx=E_DRV_SCLDMA_1_FRM_W ;u8Clientidx<E_DRV_SCLDMA_CLIENT_NUM; u8Clientidx++)
    {
        DRV_SCLDMA_MUTEX_LOCK_ISR();
        bAllClientOn |= gstScldmaInfo.bDMAOnOff[u8Clientidx];
        DRV_SCLDMA_MUTEX_UNLOCK_ISR();
    }

    if(gbScldmaSuspend == 0)
    {
        if(bAllClientOn == 0)
        {
            stSclIrqCfg.u32IRQNUM = pCfg->u32IRQNum;
            if(DrvSclIrqSuspend(&stSclIrqCfg))
            {
                bRet = TRUE;
                gbScldmaSuspend = 1;
            }
            else
            {
                DRV_SCLDMA_ERR(sclprintf("[DRVSCLDMA]%s Suspend fail\n", __FUNCTION__));
                bRet = FALSE;
            }
        }
        else
        {
            SCL_DBG(SCL_DBG_LV_DRVSCLDMA()&EN_DBGMG_SCLDMALEVEL_ELSE, "[DRVSCLDMA]%s not all scldma suspend\n",__FUNCTION__);
            bRet = TRUE;
        }
    }
    else
    {
        SCL_DBG(SCL_DBG_LV_DRVSCLDMA()&EN_DBGMG_SCLDMALEVEL_ELSE, "[DRVSCLDMA]%s already suspend\n",__FUNCTION__);
        bRet = TRUE;
    }

    return bRet;
}

bool DrvSclDmaResume(DrvSclDmaIdType_e enSCLDMA_ID, DrvSclDmaSuspendResumeConfig_t *pCfg)
{
    DrvSclIrqSuspendResumeConfig_t stSclIrqCfg;
    bool bRet = TRUE;
    SCL_DBG(SCL_DBG_LV_DRVSCLDMA()&EN_DBGMG_SCLDMALEVEL_ELSE, "[DRVSCLDMA]%s(%d), ID:%s(%d), bSuspend=%d\n",
        __FUNCTION__, __LINE__, PARSING_SCLDMA_ID(enSCLDMA_ID), enSCLDMA_ID, gbScldmaSuspend);

    if(gbScldmaSuspend == 1)
    {
        stSclIrqCfg.u32IRQNUM = pCfg->u32IRQNum;
        if(DrvSclIrqResume(&stSclIrqCfg))
        {
            DRV_SCLDMA_MUTEX_LOCK();
            gstScldmaInfo.u64mask = 0;
            DRV_SCLDMA_MUTEX_UNLOCK();
            _Drv_SCLDMA_HWInitProcess();
            gbScldmaSuspend = 0;
            DrvSclOsSetEventIrq(DrvSclIrqGetIrqEventID(),  E_DRV_SCLIRQ_EVENT_RESUME);
            DrvSclOsSetEventIrq(DrvSclIrqGetIrqSC3EventID(),  E_DRV_SCLIRQ_SC3EVENT_RESUME);
            bRet = TRUE;
        }
        else
        {
            DRV_SCLDMA_ERR(sclprintf("[DRVSCLDMA]%s Resume fail\n", __FUNCTION__));
            bRet = FALSE;
        }
    }
    else
    {
        SCL_DBG(SCL_DBG_LV_DRVSCLDMA()&EN_DBGMG_SCLDMALEVEL_ELSE, "[DRVSCLDMA]%s already Resume\n",__FUNCTION__);
        bRet = TRUE;
    }

    return bRet;
}
void DrvSclDmaExit(bool bCloseISR)
{
    if(_SCLDMA_Mutex != -1)
    {
        DrvSclOsDeleteMutex(_SCLDMA_Mutex);
        _SCLDMA_Mutex = -1;
    }
    if(_SCLIRQ_SCLDMA_Mutex != -1)
    {
        DrvSclOsDeleteSpinlock(_SCLIRQ_SCLDMA_Mutex);
        _SCLDMA_Mutex = -1;
    }
    if(bCloseISR)
    {
        DrvSclIrqExit();
    }
}
bool DrvSclDmaInit(DrvSclDmaInitConfig_t *pInitCfg)
{
    char word[]     = {"_SCLDMA_Mutex"};
    char word2[]    = {"_IRQDMA_Mutex"};
    //int i;
    DrvSclIrqInitConfig_t stIRQInitCfg;

    if(_SCLDMA_Mutex != -1)
    {
        SCL_DBG(SCL_DBG_LV_DRVSCLDMA()&EN_DBGMG_SCLDMALEVEL_ELSE, "[DRVSCLDMA]%s already done\n",__FUNCTION__);
        return TRUE;
    }

    if(DrvSclOsInit() == FALSE)
    {
        DRV_SCLDMA_ERR(sclprintf("[DRVSCLDMA]%s DrvSclOsInit Fail\n", __FUNCTION__));
        return FALSE;
    }

    _SCLDMA_Mutex           = DrvSclOsCreateMutex(E_DRV_SCLOS_FIFO, word, SCLOS_PROCESS_SHARED);
    _SCLIRQ_SCLDMA_Mutex    = DrvSclOsCreateSpinlock(E_DRV_SCLOS_FIFO, word2, SCLOS_PROCESS_SHARED);
    if (_SCLDMA_Mutex == -1)
    {
        DRV_SCLDMA_ERR(sclprintf("[DRVSCLDMA]%s create mutex fail\n", __FUNCTION__));
        return FALSE;
    }

    stIRQInitCfg.u32RiuBase = pInitCfg->u32RIUBase;
    stIRQInitCfg.u32IRQNUM     = pInitCfg->u32IRQNUM;
    stIRQInitCfg.u32CMDQIRQNUM = pInitCfg->u32CMDQIRQNUM;

    // init processing

    HalSclDmaSetRiuBase(pInitCfg->u32RIUBase);
    if(DrvSclIrqInit(&stIRQInitCfg) == FALSE)
    {
        DRV_SCLDMA_ERR(sclprintf("[DRVSCLDMA]%s Init IRQ Fail\n", __FUNCTION__));
        return FALSE;
    }
    _DrvSclDmaInitVariable();

#if SCLDMA_IRQ_EN

        gstScldmaInfo.s32IrqEventId = DrvSclOsCreateEventGroup("SCLDMA_IRQ_EVENT");

        if(gstScldmaInfo.s32IrqEventId < 0)
        {
            DRV_SCLDMA_ERR(sclprintf("[DRVSCLDMA]%s(%d) Create Event Fail\n", __FUNCTION__, __LINE__));
            return FALSE;
        }

        gstScldmaInfo.s32IrqTaskid  = DrvSclOsCreateTask((TaskEntry)_Drv_SCLDMA_Irq_Event_Task,
                                                             (u32)NULL,
                                                              TRUE,
                                                              (char*)"DrvScldma_Event_Task");

        if(gstScldmaInfo.s32IrqTaskid < 0)
        {
            DRV_SCLDMA_ERR(sclprintf("[DRVSCLDMA]%s(%d) Create Task Fail\n", __FUNCTION__, __LINE__));
            DrvSclOsDeleteEventGroup(gstScldmaInfo.s32IrqEventId);
            return FALSE;
        }

        gstScldmaInfo.s32IrqTimerId = DrvSclOsCreateTimer( _Drv_SCLDMA_CheckIrq_TimerCallback,
                                                        5,
                                                        5,
                                                        TRUE,
                                                        (char*)"SCLDMA_IRQ_TIMER");

        if(gstScldmaInfo.s32IrqTimerId < 0)
        {
            DRV_SCLDMA_ERR(sclprintf("[DRVSCLDMA]%s(%d) Create Timer Fail\n", __FUNCTION__, __LINE__));
            DrvSclOsDeleteEventGroup(gstScldmaInfo.s32IrqEventId);
            DrvSclOsDeleteTask(gstScldmaInfo.s32IrqTaskid);
            return FALSE;
        }
#endif

    _Drv_SCLDMA_HWInitProcess();

    return TRUE;
}

void DrvSclDmaSysInit(bool bEn)
{
#if ENABLE_RING_DB
    if(bEn)
    {
        DrvSclDmaRwConfig_t stIMICfg;
        stIMICfg.enRWMode       = E_DRV_SCLDMA_IMI_W;
        stIMICfg.u16Height      = HD_Height;
        stIMICfg.u16Width       = HD_Width;
        stIMICfg.u8MaxIdx       = 0;
        stIMICfg.u8Flag         = 1;
        stIMICfg.enBuffMode     = E_DRV_SCLDMA_BUF_MD_RING;
        stIMICfg.u32Base_Y[0]   = SCL_IMIinitAddr;
        stIMICfg.u32Base_C[0]   = SCL_IMIinitAddr;
        stIMICfg.u32Base_V[0]   = SCL_IMIinitAddr;
        stIMICfg.enColor        = E_DRV_SCLDMA_COLOR_YUV420 ;
        DrvSclDmaSetDmaClientConfig(E_DRV_SCLDMA_ID_1_W,stIMICfg);
    }
    else
    {
        HalSclDmaSetIMIClientReset();
    }
#endif

}
void _Drv_SCLDMA_ResetGlobalSwitchByID(DrvSclDmaIdType_e enSCLDMA_ID)
{
    u8 u8ClientIdx;
    switch(enSCLDMA_ID)
    {
        case E_DRV_SCLDMA_ID_1_W:
            for(u8ClientIdx=0; u8ClientIdx<E_DRV_SCLDMA_2_FRM_W; u8ClientIdx++)
            {
                _Drv_SCLDMA_ResetGlobalParameterByClient(u8ClientIdx);
            #if SCLDMA_IRQ_EN
                    genIrqMode[u8ClientIdx] = E_SCLDMA_IRQ_MODE_PENDING;
            #endif
            }
            _Drv_SCLDMA_ResetGlobalParameter();
            DrvSclOsClearEventIRQ(DrvSclIrqGetIrqEventID(),(0xFFFF));
            break;

        case E_DRV_SCLDMA_ID_2_W:
            for(u8ClientIdx=E_DRV_SCLDMA_2_FRM_W; u8ClientIdx<E_DRV_SCLDMA_3_FRM_R; u8ClientIdx++)
            {
                _Drv_SCLDMA_ResetGlobalParameterByClient(u8ClientIdx);
            #if SCLDMA_IRQ_EN
                    genIrqMode[u8ClientIdx] = E_SCLDMA_IRQ_MODE_PENDING;
            #endif
            }
            break;

        case E_DRV_SCLDMA_ID_3_W:
        case E_DRV_SCLDMA_ID_3_R:
            for(u8ClientIdx=E_DRV_SCLDMA_3_FRM_R; u8ClientIdx<E_DRV_SCLDMA_4_FRM_R; u8ClientIdx++)
            {
                _Drv_SCLDMA_ResetGlobalParameterByClient(u8ClientIdx);
            #if SCLDMA_IRQ_EN
                    genIrqMode[u8ClientIdx] = E_SCLDMA_IRQ_MODE_PENDING;
            #endif
                DrvSclOsClearEventIRQ(DrvSclIrqGetIrqSC3EventID(),(0xFFFF));
            }
            break;

        case E_DRV_SCLDMA_ID_PNL_R:
            _Drv_SCLDMA_ResetGlobalParameterByClient(E_DRV_SCLDMA_4_FRM_R);
        #if SCLDMA_IRQ_EN
                genIrqMode[E_DRV_SCLDMA_4_FRM_R] = E_SCLDMA_IRQ_MODE_PENDING;
        #endif
            break;

        default:
            DRV_SCLDMA_ERR(sclprintf("[DRVSCLDMA]%s Release fail\n", __FUNCTION__));
            break;
    }
}
void _Drv_SCLDMA_SetClkOnOff(DrvSclDmaIdType_e enSCLDMA_ID,DrvSclDmaClkConfig_t *stclk,bool bEn)
{
    static bool bFClk1 = 0;
    static bool bFClk2 = 0;
    static bool bODClk2 = 0;
    if(!gbclkforcemode)
    {
        if(enSCLDMA_ID<= E_DRV_SCLDMA_ID_2_W)
        {
            if(bFClk1 != bEn)
            {
                bFClk1 = bEn;
                HalSclDmaCLKInit(bFClk1,stclk);
            }
        }
        else if(enSCLDMA_ID<= E_DRV_SCLDMA_ID_3_R)
        {
            if(bFClk2 != bEn)
            {
                bFClk2 = bEn;
                HalSclDmaSC3CLKInit(bFClk2,stclk);
            }
        }
        else if(enSCLDMA_ID == E_DRV_SCLDMA_ID_PNL_R)
        {
            if(bODClk2 != bEn)
            {
                bODClk2 = bEn;
                HalSclDmaODCLKInit(bODClk2,stclk);
            }
        }
    }
}
void DrvSclDmaRelease(DrvSclDmaIdType_e enSCLDMA_ID,DrvSclDmaClkConfig_t *stclk)
{
    u8 u8idx;
    SCL_DBG(SCL_DBG_LV_DRVSCLDMA()&EN_DBGMG_SCLDMALEVEL_ELSE, "[DRVSCLDMA]%s \n",__FUNCTION__);
    DrvSclDmaDoubleBufferOnOffById(FALSE,enSCLDMA_ID);
    _Drv_SCLDMA_ResetGlobalSwitchByID(enSCLDMA_ID);
    DrvSclIrqInitVariable();
    for(u8idx = 0;u8idx<E_DRV_SCLDMA_3_IMI_R;u8idx++)
    {
        if(_Is_SingleMode(u8idx))
        {
            DrvSclDmaSetISRHandlerDmaOff(u8idx,0);
            DRV_SCLDMA_MUTEX_LOCK_ISR();
            gstScldmaInfo.bDMAOnOff[u8idx] = 0;
            DRV_SCLDMA_MUTEX_UNLOCK_ISR();
        }
    }
    _Drv_SCLDMA_SetClkOnOff(enSCLDMA_ID,stclk,0);
}


u64 _DrvSclDmaGetIrqMask(void)
{
    DrvSclDmaClientType_e enclient;
    u64 u64Flag = 0;
    u64Flag |= SCLIRQ_MSK_SC1_SNPI_W_ACTIVE;
    u64Flag |= SCLIRQ_MSK_SC1_SNPI_W_ACTIVE_N;
    u64Flag |= SCLIRQ_MSK_SC3_ENG_FRM_END;
    for(enclient = 0; enclient<E_DRV_SCLDMA_3_IMI_R; enclient++)
    {
        if(_Is_SingleMode(enclient))
        {
            switch(enclient)
            {
                case E_DRV_SCLDMA_1_FRM_W:
                case E_DRV_SCLDMA_1_SNP_W:
                case E_DRV_SCLDMA_2_FRM_W:
                case E_DRV_SCLDMA_2_FRM2_W:
                case E_DRV_SCLDMA_3_FRM_W:
                    break;
                case E_DRV_SCLDMA_1_IMI_W:
                    u64Flag |= (SCLIRQ_MSK_SC1_SNPI_W_ACTIVE|SCLIRQ_MSK_SC1_SNPI_W_ACTIVE_N);
                    break;
                case E_DRV_SCLDMA_2_IMI_W:
                    u64Flag |= (SCLIRQ_MSK_SC2_FRMI_W_ACTIVE|SCLIRQ_MSK_SC2_FRMI_W_ACTIVE_N);
                    break;
                case E_DRV_SCLDMA_3_FRM_R:
                    u64Flag |= (SCLIRQ_MSK_SC3_DMA_R_ACTIVE|SCLIRQ_MSK_SC3_DMA_R_ACTIVE_N);
                    break;
                case E_DRV_SCLDMA_4_FRM_R:
                    u64Flag |= (SCLIRQ_MSK_SC1_DBG_R_ACTIVE|SCLIRQ_MSK_SC1_DBG_R_ACTIVE_N);
                    break;
                default:
                    break;
            }
        }
    }
    return u64Flag;
}
void _DrvSclDmaSetDmaInformationForGlobal(DrvSclDmaClientType_e enClientType, DrvSclDmaRwConfig_t stCfg)
{
    u8 u8BufferIdx;
    DRV_SCLDMA_MUTEX_LOCK();
    gstScldmaInfo.enBuffMode[enClientType] = stCfg.enBuffMode;
    gstScldmaInfo.bMaxid[enClientType] = stCfg.u8MaxIdx;
    gstScldmaInfo.u64mask              = _DrvSclDmaGetIrqMask();
    gstScldmaInfo.enColor[enClientType] = stCfg.enColor;
    gstScldmaInfo.u16FrameWidth[enClientType] =  stCfg.u16Width;
    gstScldmaInfo.u16FrameHeight[enClientType] = stCfg.u16Height;
    for (u8BufferIdx = 0; u8BufferIdx <= stCfg.u8MaxIdx; u8BufferIdx++)
    {
        gstScldmaInfo.u32Base_Y[enClientType][u8BufferIdx] = stCfg.u32Base_Y[u8BufferIdx];
        gstScldmaInfo.u32Base_C[enClientType][u8BufferIdx] = stCfg.u32Base_C[u8BufferIdx];
        gstScldmaInfo.u32Base_V[enClientType][u8BufferIdx] = stCfg.u32Base_V[u8BufferIdx];
    }
    DrvSclIrqSetMask(gstScldmaInfo.u64mask);
    DRV_SCLDMA_MUTEX_UNLOCK();
    DRV_SCLDMA_MUTEX_LOCK_ISR();
    if(_Is_SWRingMode(enClientType) && _Is_SWRingModeBufferNotReady(enClientType))
    {
        gstScldmaBufferQueue[enClientType].pstHead = DrvSclOsVirMemalloc(SCLDMA_BUFFER_QUEUE_OFFSET*stCfg.u8MaxIdx);
        DrvSclOsMemset(gstScldmaBufferQueue[enClientType].pstHead,0,(SCLDMA_BUFFER_QUEUE_OFFSET*stCfg.u8MaxIdx));
        gstScldmaBufferQueue[enClientType].pstTail = gstScldmaBufferQueue[enClientType].pstHead
            + 1* stCfg.u8MaxIdx;
        gstScldmaBufferQueue[enClientType].pstWrite = gstScldmaBufferQueue[enClientType].pstHead;
        gstScldmaBufferQueue[enClientType].pstRead = gstScldmaBufferQueue[enClientType].pstHead;
        gstScldmaBufferQueue[enClientType].pstWriteAlready = gstScldmaBufferQueue[enClientType].pstHead;
        gstScldmaBufferQueue[enClientType].bUsed = 1;
        gstScldmaBufferQueue[enClientType].u8Bufferflag= stCfg.u8Flag;
        stCfg.u8MaxIdx = 0;//single change buffer
    }
    DRV_SCLDMA_MUTEX_UNLOCK_ISR();
}
void _DrvSclDmaSetVsyncTrigMode(DrvSclDmaClientType_e enClientType)
{
    if(_Is_SC3Singlemode(enClientType))
    {
        _Drv_SCLDMA_SetVsyncTrigConfig(E_DRV_SCLDMA_VS_ID_SC3);
        _Drv_SCLDMA_SetVsyncRegenMode(E_DRV_SCLDMA_VS_ID_SC3, E_DRV_SCLDMA_VS_TRIG_MODE_SWTRIGGER);
    }
    else if(_Is_SC3Ringmode(enClientType)|| _Is_SC3SWRingmode(enClientType))
    {
        _Drv_SCLDMA_SetVsyncRegenMode(E_DRV_SCLDMA_VS_ID_SC3, E_DRV_SCLDMA_VS_TRIG_MODE_HW_IN_VSYNC);
    }
}
bool DrvSclDmaSetDmaClientConfig(DrvSclDmaIdType_e enSCLDMA_ID, DrvSclDmaRwConfig_t stCfg)
{
    bool bRet = TRUE;
    u8 u8BufferIdx;
    u32 u32Time = 0;
    DrvSclDmaClientType_e enClientType;
    u32Time = (((u32)DrvSclOsGetSystemTimeStamp()));
    enClientType = _Drv_SCLDMA_TransToClientType(enSCLDMA_ID ,stCfg.enRWMode);
    SCL_DBG(SCL_DBG_LV_DRVSCLDMA()&Get_DBGMG_SCLDMAclient(enClientType,0),
        "[DRVSCLDMA]%s %d, DmaID:%s(%d), Flag:%x, RW:%s(%d), Buf:%s(%d), color:%s(%d), (W:%d, H:%d)\n",
        __FUNCTION__, __LINE__,
        PARSING_SCLDMA_ID(enSCLDMA_ID), enSCLDMA_ID,
        stCfg.u8Flag,
        PARSING_SCLDMA_RWMD(stCfg.enRWMode), stCfg.enRWMode,
        PARSING_SCLDMA_BUFMD(stCfg.enBuffMode), stCfg.enBuffMode,
        PARSING_SCLDMA_COLOR(stCfg.enColor), stCfg.enColor,
        stCfg.u16Width, stCfg.u16Height);
    SCL_DBG(SCL_DBG_LV_DRVSCLDMA()&Get_DBGMG_SCLDMAclient(enClientType,0),
        "[DRVSCLDMA]%s %d, maxbuf:%d, (%lx, %lx, %lx), (%lx, %lx, %lx),(%lx, %lx, %lx), (%lx, %lx, %lx) \n",
        __FUNCTION__, __LINE__,
        stCfg.u8MaxIdx,
        stCfg.u32Base_Y[0], stCfg.u32Base_C[0],stCfg.u32Base_V[0],
        stCfg.u32Base_Y[1], stCfg.u32Base_C[1],stCfg.u32Base_V[1],
        stCfg.u32Base_Y[2], stCfg.u32Base_C[2],stCfg.u32Base_V[2],
        stCfg.u32Base_Y[3], stCfg.u32Base_C[3],stCfg.u32Base_V[3]);

    // check
    if(SCLDMA_CHECK_ALIGN(stCfg.u16Height, 2))
    {
        DRV_SCLDMA_ERR(sclprintf("[DRVSCLDMA]: Height must be align 2\n"));
        bRet = FALSE;
    }

    if(stCfg.enColor == E_DRV_SCLDMA_COLOR_YUV422 && SCLDMA_CHECK_ALIGN(stCfg.u16Width, 8))
    {
        DRV_SCLDMA_ERR(sclprintf("[DRVSCLDMA]%s %d: Width must be align 8\n", __FUNCTION__, __LINE__));
        bRet =  FALSE;
    }
    else if(stCfg.enColor == E_DRV_SCLDMA_COLOR_YUV420 &&SCLDMA_CHECK_ALIGN(stCfg.u16Width, 16))
    {
        DRV_SCLDMA_ERR(sclprintf("[DRVSCLDMA]%s %d: Width must be align 16\n", __FUNCTION__, __LINE__));
        bRet =  FALSE;
    }
    else if(stCfg.enColor == E_DRV_SCLDMA_COLOR_YCSep422 &&SCLDMA_CHECK_ALIGN(stCfg.u16Width, 16))
    {
        DRV_SCLDMA_ERR(sclprintf("[DRVSCLDMA]%s %d: Width must be align 16\n", __FUNCTION__, __LINE__));
        bRet =  FALSE;
    }
    else if(stCfg.enColor == E_DRV_SCLDMA_COLOR_YUVSep422 &&SCLDMA_CHECK_ALIGN(stCfg.u16Width, 16))
    {
        DRV_SCLDMA_ERR(sclprintf("[DRVSCLDMA]%s %d: Width must be align 16\n", __FUNCTION__, __LINE__));
        bRet =  FALSE;
    }
    else if(stCfg.enColor == E_DRV_SCLDMA_COLOR_YUVSep420 &&SCLDMA_CHECK_ALIGN(stCfg.u16Width, 16))
    {
        DRV_SCLDMA_ERR(sclprintf("[DRVSCLDMA]%s %d: Width must be align 16\n", __FUNCTION__, __LINE__));
        bRet =  FALSE;
    }

    if(stCfg.enBuffMode == E_DRV_SCLDMA_BUF_MD_SINGLE &&  stCfg.u8MaxIdx > 1)
    {
        DRV_SCLDMA_ERR(sclprintf("[DRVSCLDMA]%s %d: More than 1 buffer to SINGLE mode\n", __FUNCTION__, __LINE__));
        bRet =  FALSE;
    }

    if(stCfg.enRWMode == E_DRV_SCLDMA_RW_NUM)
    {
        DRV_SCLDMA_ERR(sclprintf("[DRVSCLDMA]%s %d: RW mode is not coreect\n", __FUNCTION__, __LINE__));
        bRet =  FALSE;
    }

    for(u8BufferIdx=0; u8BufferIdx<=stCfg.u8MaxIdx; u8BufferIdx++)
    {
        if( SCLDMA_CHECK_ALIGN(stCfg.u32Base_Y[u8BufferIdx], 8))
        {
            DRV_SCLDMA_ERR(sclprintf("[DRVSCLDMA]%s %d: YBase must be 8 byte align:%lx\n", __FUNCTION__, __LINE__,stCfg.u32Base_Y[u8BufferIdx]));
            bRet = FALSE;
            break;
        }

        if(((stCfg.enColor == E_DRV_SCLDMA_COLOR_YUV420)||(stCfg.enColor == E_DRV_SCLDMA_COLOR_YCSep422)) &&
            SCLDMA_CHECK_ALIGN(stCfg.u32Base_C[u8BufferIdx], 8))
        {
            DRV_SCLDMA_ERR(sclprintf("[DRVSCLDMA]%s %d: CBase must be 8 byte align DmaID:%s(%d)\n", __FUNCTION__, __LINE__,PARSING_SCLDMA_ID(enSCLDMA_ID), enSCLDMA_ID));
            bRet = FALSE;
            break;
        }
    }

    if(stCfg.enColor == E_DRV_SCLDMA_COLOR_YUV422)
    {
        for(u8BufferIdx=0; u8BufferIdx<=stCfg.u8MaxIdx; u8BufferIdx++)
        {
            if((stCfg.u32Base_C[u8BufferIdx] != (stCfg.u32Base_Y[u8BufferIdx] + 16) ))
            {
                DRV_SCLDMA_ERR(sclprintf("[DRVSCLDMA]Error:%s %d: YUV422 CBase_%08lx, YBase_%08lx, \n",
                    __FUNCTION__, __LINE__, stCfg.u32Base_Y[u8BufferIdx], stCfg.u32Base_C[u8BufferIdx]));
                bRet = FALSE;
            }
        }
    }
    if(_IsFlagType(enClientType,E_DRV_SCLDMA_FLAG_ACTIVE))
    {
        SCL_DBGERR("[DRVSCLDMA] %s::%d %s_%s still R/W @:%lu\n",
            __FUNCTION__,enClientType, PARSING_SCLDMA_ID(enSCLDMA_ID), PARSING_SCLDMA_RWMD(stCfg.enRWMode),u32Time);
        bRet = FALSE;
    }

    if(bRet == FALSE)
    {
        return FALSE;
    }

    _DrvSclDmaSetDmaInformationForGlobal(enClientType, stCfg);
    _DrvSclDmaSetVsyncTrigMode(enClientType);

    switch(enSCLDMA_ID)
    {
        case E_DRV_SCLDMA_ID_1_W:
            DRV_SCLDMA_MUTEX_LOCK();
             HalSclDmaSetSC1DMAConfig(stCfg);
             DRV_SCLDMA_MUTEX_UNLOCK();
            break;

        case E_DRV_SCLDMA_ID_2_W:
            DRV_SCLDMA_MUTEX_LOCK();
            HalSclDmaSetSC2DMAConfig(stCfg);
            DRV_SCLDMA_MUTEX_UNLOCK();
            break;

        case E_DRV_SCLDMA_ID_3_W:
        case E_DRV_SCLDMA_ID_3_R:
            DRV_SCLDMA_MUTEX_LOCK();
            HalSclDmaSetSC3DMAConfig(stCfg);
            DRV_SCLDMA_MUTEX_UNLOCK();
            break;

        case E_DRV_SCLDMA_ID_PNL_R:
            DRV_SCLDMA_MUTEX_LOCK();
            HalSclDmaSetDisplayDMAConfig(stCfg);
            DRV_SCLDMA_MUTEX_UNLOCK();
            break;

        default:
            return FALSE;
    }

    return TRUE;
}
u16 _DrvSclDmaGetActiveIrqNum(DrvSclDmaClientType_e enClientType)
{
    u16 u16IrqNum;

    switch(enClientType)
    {
        case E_DRV_SCLDMA_1_FRM_W:
            u16IrqNum = SCLIRQ_SC1_FRM_W_ACTIVE;
            break;
        case E_DRV_SCLDMA_1_SNP_W:
            u16IrqNum = SCLIRQ_SC1_SNP_W_ACTIVE;
            break;
        case E_DRV_SCLDMA_1_IMI_W:
            u16IrqNum = SCLIRQ_SC1_SNPI_W_ACTIVE;
            break;
        case E_DRV_SCLDMA_2_FRM_W:
            u16IrqNum = SCLIRQ_SC2_FRM_W_ACTIVE;
            break;
        case E_DRV_SCLDMA_2_FRM2_W:
            u16IrqNum = SCLIRQ_SC2_FRM2_W_ACTIVE;
            break;
        case E_DRV_SCLDMA_2_IMI_W:
            u16IrqNum = SCLIRQ_SC2_FRMI_W_ACTIVE;
            break;
        case E_DRV_SCLDMA_3_FRM_W:
            u16IrqNum = SCLIRQ_SC3_DMA_W_ACTIVE;
            break;
        case E_DRV_SCLDMA_3_FRM_R:
            u16IrqNum = SCLIRQ_SC3_DMA_R_ACTIVE;
            break;
        case E_DRV_SCLDMA_4_FRM_R:
            u16IrqNum = SCLIRQ_SC1_DBG_R_ACTIVE;
            break;
        default:
            u16IrqNum = SCLIRQ_RESERVED;
            break;
    }

    return u16IrqNum;
}
void _DrvSclDmaResetTrigCount(DrvSclDmaClientType_e enClientType,bool bEn)
{
    if(!bEn)
    {
        gu32TrigCount[enClientType]++;
        if(gu32TrigCount[enClientType]==0xEFFFFFFE)
        {
            gu32TrigCount[enClientType] = 1;
        }
    }
}
void DrvSclDmaSetSWReTrigCount(DrvSclDmaClientType_e enClientType,bool bEn)
{
    //for ISR don;t need to lock
    if(bEn)
    {
        gu8ResetCount[enClientType]++;
    }
    else
    {
        if(gu8ResetCount[enClientType])
        {
            gu8ResetCount[enClientType]--;
        }

    }
}
void DrvSclDmaSetDMAIgnoreCount(DrvSclDmaClientType_e enClientType,bool bEn)
{
    //for ISR don;t need to lock
    if(bEn)
    {
        gu8DMAErrCount[enClientType]++;
    }
    else
    {
        if(gu8DMAErrCount[enClientType])
        {
            gu8DMAErrCount[enClientType]--;
        }

    }
}
bool DrvSclDmaSetISRHandlerDmaOff(DrvSclDmaClientType_e enClientType,bool bEn)
{
    DrvSclDmaRwModeType_e enRWMode;
    u64 u64ActN;
    bool bRet = 0;
    if(gstScldmaInfo.bDMAOnOff[enClientType] == 0 &&_Is_RingMode(enClientType))
    {
        SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&Get_DBGMG_SCLIRQclient(enClientType,1),
            "[DRVSCLDMA] %s:: enClientType:%d still ON/OFF %hhd\n",
        __FUNCTION__,enClientType,bEn);
    }
    switch(enClientType)
    {
        case E_DRV_SCLDMA_1_FRM_W:
        case E_DRV_SCLDMA_2_FRM_W:
        case E_DRV_SCLDMA_3_FRM_W:
            enRWMode = E_DRV_SCLDMA_FRM_W;
            break;
        case E_DRV_SCLDMA_1_SNP_W:
            enRWMode = E_DRV_SCLDMA_SNP_W;
            break;
        case E_DRV_SCLDMA_2_FRM2_W:
            enRWMode = E_DRV_SCLDMA_FRM2_W;
            break;
        case E_DRV_SCLDMA_1_IMI_W:
        case E_DRV_SCLDMA_2_IMI_W:
            enRWMode = E_DRV_SCLDMA_IMI_W;
            break;
        case E_DRV_SCLDMA_3_FRM_R:
            enRWMode = E_DRV_SCLDMA_FRM_R;
            break;
        case E_DRV_SCLDMA_4_FRM_R:
            enRWMode = E_DRV_SCLDMA_DBG_R;
            break;
        default:
            enRWMode = E_DRV_SCLDMA_RW_NUM;
            break;

    }
    if(enClientType == E_DRV_SCLDMA_1_FRM_W || enClientType == E_DRV_SCLDMA_1_SNP_W || enClientType == E_DRV_SCLDMA_1_IMI_W)
    {
        if(bEn && DoubleBufferStatus)
        {
            DrvSclIrqGetFlag((_DrvSclDmaGetActiveIrqNum(enClientType)+1), &u64ActN);
            if(!u64ActN)
            {
                _Drv_SCLDMA_SC1OnOff(enRWMode, bEn);
                bRet = bEn;
            }
        }
        else
        {
            _Drv_SCLDMA_SC1OnOff(enRWMode, bEn);
            bRet = bEn;
        }
        _DrvSclDmaResetTrigCount(enClientType,bEn);
    }
    else if(enClientType == E_DRV_SCLDMA_2_FRM_W || enClientType == E_DRV_SCLDMA_2_IMI_W || enClientType == E_DRV_SCLDMA_2_FRM2_W)
    {
        if(bEn && DoubleBufferStatus)
        {
            DrvSclIrqGetFlag((_DrvSclDmaGetActiveIrqNum(enClientType)+1), &u64ActN);
            if(!u64ActN)
            {
                _Drv_SCLDMA_SC2OnOff(enRWMode, bEn);
                bRet = bEn;
            }
        }
        else
        {
            _Drv_SCLDMA_SC2OnOff(enRWMode, bEn);
            bRet = bEn;
        }
        _DrvSclDmaResetTrigCount(enClientType,bEn);
    }
    else if(enClientType == E_DRV_SCLDMA_3_FRM_R || enClientType == E_DRV_SCLDMA_3_FRM_W)
    {
        if(bEn && DoubleBufferStatus)
        {
            DrvSclIrqGetFlag((_DrvSclDmaGetActiveIrqNum(enClientType)+1), &u64ActN);
            if(!u64ActN)
            {
                HalSclDmaSetSC3DMAEn(E_DRV_SCLDMA_FRM_R, bEn);
                HalSclDmaSetSC3DMAEn(E_DRV_SCLDMA_FRM_W, bEn);
                bRet = bEn;
            }
        }
        else
        {
            HalSclDmaSetSC3DMAEn(E_DRV_SCLDMA_FRM_R, bEn);
            HalSclDmaSetSC3DMAEn(E_DRV_SCLDMA_FRM_W, bEn);
            bRet = bEn;
        }
        _DrvSclDmaResetTrigCount(enClientType,bEn);
    }
    else
    {
        HalSclDmaSetDisplayDMAEn(enRWMode, bEn);
        _DrvSclDmaResetTrigCount(enClientType,bEn);
    }

    if(enClientType == E_DRV_SCLDMA_3_FRM_R || enClientType == E_DRV_SCLDMA_3_FRM_W)
    {
        SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&Get_DBGMG_SCLIRQclient(enClientType,1), "[DRVSCLDMA]%s Client:%s trig off\n",
        __FUNCTION__,PARSING_SCLDMA_CLIENT(enClientType));
    }
    else
    {
        if(bEn)
        {
            SCL_RTKDBG(0,"[DRVSCLDMA]Client:%s trig on @:%lu\n",PARSING_SCLDMA_CLIENT(enClientType),((u32)DrvSclOsGetSystemTimeStamp()));
            SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&Get_DBGMG_SCLIRQclient(8,1), "[DRVSCLDMA]%s Client:%s trig on\n",
            __FUNCTION__,PARSING_SCLDMA_CLIENT(enClientType));
        }
        else
        {
            SCL_RTKDBG(0,"[DRVSCLDMA]Client:%s trig off @:%lu\n",PARSING_SCLDMA_CLIENT(enClientType),((u32)DrvSclOsGetSystemTimeStamp()));
            SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&Get_DBGMG_SCLIRQclient(8,1), "[DRVSCLDMA]%s Client:%s trig off\n",
            __FUNCTION__,PARSING_SCLDMA_CLIENT(enClientType));
        }
    }
    return bRet;
}
void DrvSclDmaDoubleBufferOnOffById(bool bEn, DrvSclDmaIdType_e enSCLDMA_ID)
{
    if(enSCLDMA_ID <= E_DRV_SCLDMA_ID_2_W)
    {
        HalSclDmaSetHandshakeDoubleBuffer(bEn);
        gVsrcDBnotOpen = !bEn;
    }
    HalSclDmaSetDMAEnableDoubleBuffer(bEn,enSCLDMA_ID);
}
void DrvSclDmaSetIspFrameCount(void)
{
    gu8ISPcount = HalSclDmaGetIspFrameCountReg();
    if(SCL_DELAYFRAME == 0)
    {
        gu8ISPcount = gu8ISPcount;
    }
    else if(SCL_DELAYFRAME == 1)
    {
        gu8ISPcount = (gu8ISPcount==0) ? 0x7F : (gu8ISPcount -1);
    }
    SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()==EN_DBGMG_SCLIRQLEVEL_ELSE, "[DRVSCLDMA]ISP Count:%hhx\n",gu8ISPcount);
}
void DrvSclDmaSetSclFrameDoneTime(DrvSclDmaClientType_e enClientType, u64 u64FRMDoneTime)
{
    u16  u16RealIdx;
    if(_Is_RingMode(enClientType))
    {
        u16RealIdx = _GetIdxType(enClientType,E_DRV_SCLDMA_ACTIVE_BUFFER_SCL);
    }
    else
    {
        u16RealIdx = 0;
    }
    gu64FRMDoneTime[enClientType][u16RealIdx] = (u64)u64FRMDoneTime;
    SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()==EN_DBGMG_SCLIRQLEVEL_ELSE, "[DRVSCLDMA]%d FRMDoneTime:%llu\n",enClientType,u64FRMDoneTime);
}
DrvSclDmaDbStatusType_e DrvSclDmaGetVsrcDoubleBufferStatus(u32 u32Event)
{
    u32 enClientIdx;
    u8 bNeedOff = 0;
    if(_Is_OnlySC1SNPSingleDone(u32Event))//only sc1 snp
    {
        SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&EN_DBGMG_SCLIRQLEVEL_ELSE,"[DRVSCLDMA] ONLY_SC1_SNP_SI_NEED_OFF \n");
        return E_DRV_SCLDMA_DB_STATUS_NEED_OFF;//db off
    }
    else if(_Is_OnlySC1FRMSingleDone(u32Event))//only sc1 frm snp
    {
        SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&EN_DBGMG_SCLIRQLEVEL_ELSE,"[DRVSCLDMA] ONLY_SC1_FRM_SI_NEED_OFF \n");
        return E_DRV_SCLDMA_DB_STATUS_NEED_OFF;//db off
    }
    else if(_Is_OnlySC2FRMSingleDone(u32Event))//only sc2 frm snp
    {
        SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&EN_DBGMG_SCLIRQLEVEL_ELSE,"[DRVSCLDMA] ONLY_SC2_FRM_SI_NEED_OFF \n");
        return E_DRV_SCLDMA_DB_STATUS_NEED_OFF;//db off
    }
    else
    {
        for(enClientIdx= E_DRV_SCLDMA_1_FRM_W; enClientIdx<E_DRV_SCLDMA_3_FRM_R; enClientIdx++)
        {
            if(_Is_DMAClientOn(enClientIdx))
            {
                if(enClientIdx == E_DRV_SCLDMA_1_FRM_W && !(u32Event&E_DRV_SCLIRQ_EVENT_ISTSC1FRM) )
                {
                    bNeedOff ++;
                }
                else if(enClientIdx == E_DRV_SCLDMA_2_FRM_W && !(u32Event&E_DRV_SCLIRQ_EVENT_ISTSC2FRM) )
                {
                    bNeedOff ++;
                }
                else if(enClientIdx == E_DRV_SCLDMA_2_FRM2_W && !(u32Event&E_DRV_SCLIRQ_EVENT_ISTSC2FRM2) )
                {
                    bNeedOff ++;
                }
                else if(enClientIdx == E_DRV_SCLDMA_1_SNP_W && !(u32Event&E_DRV_SCLIRQ_EVENT_ISTSC1SNP) )
                {
                    bNeedOff ++;
                }
                else if(_Is_VsrcDoubleBufferNotOpen())
                {
                    SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&EN_DBGMG_SCLIRQLEVEL_ELSE,"[DRVSCLDMA] DB_NEED_ON \n");
                    return E_DRV_SCLDMA_DB_STATUS_NEED_ON; //db on
                }
                else if(_Is_VsrcDoubleBufferOpen())
                {
                    SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&EN_DBGMG_SCLIRQLEVEL_ELSE,"[DRVSCLDMA] DB_KEEP ON\n");
                    return E_DRV_SCLDMA_DB_STATUS_KEEP; //db keep on
                }
            }
        }
        if(bNeedOff)
        {
            SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&EN_DBGMG_SCLIRQLEVEL_ELSE,"[DRVSCLDMA] reset stauts NEED_OFF \n");
            return E_DRV_SCLDMA_DB_STATUS_NEED_OFF;//db off
        }
        if(_Is_VsrcDoubleBufferNotOpen())
        {
            SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&EN_DBGMG_SCLIRQLEVEL_ELSE,"[DRVSCLDMA] DB_KEEP OFF\n");
            return E_DRV_SCLDMA_DB_STATUS_KEEP;//db keep off
        }
        else
        {
            SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&EN_DBGMG_SCLIRQLEVEL_ELSE,"[DRVSCLDMA] DB_NEED_OFF \n");
            return E_DRV_SCLDMA_DB_STATUS_NEED_OFF;//db off
        }
    }
}
bool _DrvSclDmaSetSWRingModeDmaClientOnOff(DrvSclDmaIdType_e enSCLDMA_ID, DrvSclDmaOnOffConfig_t stCfg )
{
    u32 u32Time          = 0;
    DrvSclDmaClientType_e enClientType;
    u32Time = ((u32)DrvSclOsGetSystemTimeStamp());
    enClientType = _Drv_SCLDMA_TransToClientType(enSCLDMA_ID ,stCfg.enRWMode);
    SCL_DBG(SCL_DBG_LV_DRVSCLDMA()&Get_DBGMG_SCLDMAclient(enClientType,1),
        "[DRVSCLDMA]%s(%d), DmaID:%s(%d), RW:%s(%d), En:%d @:%lu\n",
        __FUNCTION__, __LINE__,
        PARSING_SCLDMA_ID(enSCLDMA_ID), enSCLDMA_ID,
        PARSING_SCLDMA_RWMD(stCfg.enRWMode), stCfg.enRWMode,
        stCfg.bEn,u32Time);
    if(gstScldmaInfo.bDMAOnOff[enClientType] == 1 &&  gstScldmaInfo.bDMAOnOff[enClientType] == stCfg.bEn)
    {
        DRV_SCLDMA_ERR(sclprintf("[DRVSCLDMA] %s:: %s_%s still R/W \n",
            __FUNCTION__, PARSING_SCLDMA_ID(enSCLDMA_ID), PARSING_SCLDMA_RWMD(stCfg.enRWMode)));
        return FALSE;
    }
    if(gbForceClose[enClientType])
    {
        DRV_SCLDMA_ERR(sclprintf("[DRVSCLDMA] %s:: %s_%s BLOCK \n",
            __FUNCTION__, PARSING_SCLDMA_ID(enSCLDMA_ID), PARSING_SCLDMA_RWMD(stCfg.enRWMode)));
        return FALSE;
    }
    DRV_SCLDMA_MUTEX_LOCK_ISR();
    gstScldmaInfo.bDMAOnOff[enClientType] = stCfg.bEn;
    DRV_SCLDMA_MUTEX_UNLOCK_ISR();
    if(stCfg.bEn)
    {
        DRV_SCLDMA_MUTEX_LOCK_ISR();
        gstScldmaInfo.bDmaflag[enClientType]   = E_DRV_SCLDMA_FLAG_BLANKING;
        gstScldmaInfo.bDMAidx[enClientType]    = (E_DRV_SCLDMA_ACTIVE_BUFFER_SCL|(gstScldmaInfo.bMaxid[enClientType]<<4));
        if(enClientType == E_DRV_SCLDMA_3_FRM_R || enClientType == E_DRV_SCLDMA_3_FRM_W)
        {
            gstScldmaInfo.bDMAidx[enClientType]    = (E_DRV_SCLDMA_ACTIVE_BUFFER_SCL|(0x5<<4));
            gstScldmaInfo.bDmaflag[enClientType]   = 0;
        }
        DRV_SCLDMA_MUTEX_UNLOCK_ISR();
    }
    if(stCfg.bEn)
    {
        _Drv_SCLDMA_SetClkOnOff(enSCLDMA_ID,stCfg.stclk,1);
    }
    switch(enSCLDMA_ID)
    {
        case E_DRV_SCLDMA_ID_1_W:
            _Drv_SCLDMA_SC1OnOff(stCfg.enRWMode, stCfg.bEn);
            break;

        case E_DRV_SCLDMA_ID_2_W:
            _Drv_SCLDMA_SC2OnOff(stCfg.enRWMode, stCfg.bEn);
            break;

        case E_DRV_SCLDMA_ID_3_W:
            HalSclDmaSetSC3DMAEn(stCfg.enRWMode, stCfg.bEn);
            if(stCfg.bEn)
            {
                HalSclDmaTrigRegenVSync(E_DRV_SCLDMA_VS_ID_SC3,0);
            }
            break;

        case E_DRV_SCLDMA_ID_3_R:
            HalSclDmaSetSC3DMAEn(stCfg.enRWMode, stCfg.bEn);
            break;

        case E_DRV_SCLDMA_ID_PNL_R:
            HalSclDmaSetDisplayDMAEn(stCfg.enRWMode, stCfg.bEn);

            break;

        default:
            return FALSE;
    }
    return TRUE;
}
bool _DrvSclDmaSetDmaOnOff(DrvSclDmaIdType_e enSCLDMA_ID,DrvSclDmaOnOffConfig_t stCfg)
{
    bool bSingleMode;
    DrvSclDmaClientType_e enClientType;
    enClientType    = _Drv_SCLDMA_TransToClientType(enSCLDMA_ID ,stCfg.enRWMode);
    bSingleMode     = gstScldmaInfo.enBuffMode[enClientType] == E_DRV_SCLDMA_BUF_MD_SINGLE ? TRUE : FALSE;
    switch(enSCLDMA_ID)
    {
        case E_DRV_SCLDMA_ID_1_W:
            if(DoubleBufferStatus)
            {
                _Drv_SCLDMA_SC1OnOff(stCfg.enRWMode, stCfg.bEn);
            }
            else
            {
                _Drv_SCLDMA_SC1OnOffWithoutDoubleBuffer(stCfg.enRWMode, stCfg.bEn,enClientType);
            }
            break;

        case E_DRV_SCLDMA_ID_2_W:
            if(DoubleBufferStatus)
            {
                _Drv_SCLDMA_SC2OnOff(stCfg.enRWMode, stCfg.bEn);
            }
            else
            {
                _Drv_SCLDMA_SC2OnOffWithoutDoubleBuffer(stCfg.enRWMode, stCfg.bEn,enClientType);
            }
            break;

        case E_DRV_SCLDMA_ID_3_W:
            HalSclDmaSetSC3DMAEn(stCfg.enRWMode, stCfg.bEn);
            if(bSingleMode && stCfg.bEn)
            {
                _Drv_SCLDMA_SWRegenVSyncTrigger(E_DRV_SCLDMA_VS_ID_SC3);
                DRV_SCLDMA_MUTEX_LOCK_ISR();
                _SetANDGetFlagType(E_DRV_SCLDMA_3_FRM_W,E_DRV_SCLDMA_FLAG_EVERDMAON,~(E_DRV_SCLDMA_FLAG_DMAOFF));
                DRV_SCLDMA_MUTEX_UNLOCK_ISR();
            }
            else if(!bSingleMode && stCfg.bEn)
            {
                _Drv_SCLDMA_SetVsyncRegenMode(E_DRV_SCLDMA_VS_ID_SC3,E_DRV_SCLDMA_VS_TRIG_MODE_HW_IN_VSYNC);
            }
            break;

        case E_DRV_SCLDMA_ID_3_R:
            HalSclDmaSetSC3DMAEn(stCfg.enRWMode, stCfg.bEn);
            break;

        case E_DRV_SCLDMA_ID_PNL_R:
            HalSclDmaSetDisplayDMAEn(stCfg.enRWMode, stCfg.bEn);
            if(bSingleMode && stCfg.bEn)
            {
                DrvSclDmaDoubleBufferOnOffById(TRUE,E_DRV_SCLDMA_ID_PNL_R);
            }

            break;

        default:
            return FALSE;
    }
    return TRUE;
}
bool DrvSclDmaSetDmaClientOnOff(DrvSclDmaIdType_e enSCLDMA_ID, DrvSclDmaOnOffConfig_t stCfg )
{
    bool bSingleMode;
    u32 u32Time          = 0;
    DrvSclDmaClientType_e enClientType;
    static u16 u16count[E_DRV_SCLDMA_CLIENT_NUM] = {0,0,0,0,0,0,0,0,0};
    u32Time = ((u32)DrvSclOsGetSystemTimeStamp());
    enClientType    = _Drv_SCLDMA_TransToClientType(enSCLDMA_ID ,stCfg.enRWMode);
    if(_Is_SWRingMode(enClientType))
    {
        _DrvSclDmaSetSWRingModeDmaClientOnOff(enSCLDMA_ID,stCfg);
        return 1;
    }
    SCL_DBG(SCL_DBG_LV_DRVSCLDMA()&Get_DBGMG_SCLDMAclient(enClientType,1),
        "[DRVSCLDMA]%s(%d), DmaID:%s(%d), RW:%s(%d), En:%d @:%lu\n",
        __FUNCTION__, __LINE__,
        PARSING_SCLDMA_ID(enSCLDMA_ID), enSCLDMA_ID,
        PARSING_SCLDMA_RWMD(stCfg.enRWMode), stCfg.enRWMode,
        stCfg.bEn,u32Time);
    bSingleMode     = gstScldmaInfo.enBuffMode[enClientType] == E_DRV_SCLDMA_BUF_MD_SINGLE ? TRUE : FALSE;
    if(bSingleMode)
    {
        if(stCfg.bEn && _IsFlagType(enClientType,E_DRV_SCLDMA_FLAG_ACTIVE))
        {
            u16count[enClientType]++;
            DRV_SCLDMA_ERR(sclprintf("[DRVSCLDMA] %s::%d %s_%s still R/W @:%lu\n",
                __FUNCTION__,enClientType, PARSING_SCLDMA_ID(enSCLDMA_ID), PARSING_SCLDMA_RWMD(stCfg.enRWMode),u32Time));
            if(u16count[enClientType]<2)
            {
                return FALSE;
            }
            else
            {
                u16count[enClientType] = 0;
            }
        }
        else
        {
            u16count[enClientType] = 0;
        }
        if(gbForceClose[enClientType])
        {
            DRV_SCLDMA_ERR(sclprintf("[DRVSCLDMA] %s:: %s_%s BLOCK \n",
                __FUNCTION__, PARSING_SCLDMA_ID(enSCLDMA_ID), PARSING_SCLDMA_RWMD(stCfg.enRWMode)));
            return FALSE;
        }
    }
    else
    {
        if(gstScldmaInfo.bDMAOnOff[enClientType] == 1 &&  gstScldmaInfo.bDMAOnOff[enClientType] == stCfg.bEn)
        {
            u16count[enClientType]++;
            DRV_SCLDMA_ERR(sclprintf("[DRVSCLDMA] %s:: %s_%s still R/W %d\n",
                __FUNCTION__, PARSING_SCLDMA_ID(enSCLDMA_ID), PARSING_SCLDMA_RWMD(stCfg.enRWMode),gstScldmaInfo.enBuffMode[enClientType]));
            //DRV_SCLDMA_MUTEX_UNLOCK();
            if(u16count[enClientType]<10)
            {
                return FALSE;
            }
            else
            {
                u16count[enClientType] = 0;
            }
        }
        if(gbForceClose[enClientType])
        {
            DRV_SCLDMA_ERR(sclprintf("[DRVSCLDMA] %s:: %s_%s BLOCK \n",
                __FUNCTION__, PARSING_SCLDMA_ID(enSCLDMA_ID), PARSING_SCLDMA_RWMD(stCfg.enRWMode)));
            return FALSE;
        }
    }
    DRV_SCLDMA_MUTEX_LOCK_ISR();
    gstScldmaInfo.bDMAOnOff[enClientType] = stCfg.bEn;
    if(stCfg.bEn)
    {
        if(!bSingleMode)
        {
            gstScldmaInfo.bDmaflag[enClientType]   = E_DRV_SCLDMA_FLAG_BLANKING;
            gstScldmaInfo.bDMAidx[enClientType]    = (E_DRV_SCLDMA_ACTIVE_BUFFER_SCL|(gstScldmaInfo.bMaxid[enClientType]<<4));
            if(enClientType == E_DRV_SCLDMA_3_FRM_R || enClientType == E_DRV_SCLDMA_3_FRM_W)
            {
                gstScldmaInfo.bDMAidx[enClientType]    = (E_DRV_SCLDMA_ACTIVE_BUFFER_SCL|(0x5<<4));
                gstScldmaInfo.bDmaflag[enClientType]   = 0;
            }
        }
        else
        {
            gstScldmaInfo.bDmaflag[enClientType]   &= ~E_DRV_SCLDMA_FLAG_DMAFORCEOFF;
        }
    }
    else
    {
        gstScldmaInfo.bDmaflag[enClientType]   |= E_DRV_SCLDMA_FLAG_DMAFORCEOFF;
    }
    DRV_SCLDMA_MUTEX_UNLOCK_ISR();
    if(stCfg.bEn)
    {
        _Drv_SCLDMA_SetClkOnOff(enSCLDMA_ID,stCfg.stclk,1);
    }
#if ENABLE_RING_DB
    if(_Is_VsrcDoubleBufferNotOpen() && _Is_VsrcId(enSCLDMA_ID) && DoubleBufferStatus)
    {
        gVsrcDBnotOpen = 0;
        _Drv_SCLDMA_SetDoubleBufferOn(E_DRV_SCLDMA_ID_1_W, SCLIRQ_SC1_SNPI_W_ACTIVE, E_DRV_SCLDMA_1_IMI_W);
    }
#endif
    return _DrvSclDmaSetDmaOnOff(enSCLDMA_ID,stCfg);
}
void _DrvSclDmaGetInQueueCount(DrvSclDmaClientType_e enClientType)
{
    //sclprintf("[GetInQueueCount] %hhd ,%lx,%lx\n",gstScldmaBufferQueue[enClientType].u8InQueueCount,(long)gstScldmaBufferQueue[enClientType].pstWrite,(long)gstScldmaBufferQueue[enClientType].pstRead);
    if(_IsQueueWriteLargeRead(enClientType))
    {
        _GetInQueueCountIfLarge(enClientType);
    }
    else if (_IsQueueWriteSmallRead(enClientType))
    {
        _GetInQueueCountIfSmall(enClientType);
    }
    else if(_Is_SWRingModeBufferFull(enClientType))
    {
        gstScldmaBufferQueue[enClientType].u8InQueueCount = gstScldmaInfo.bMaxid[enClientType];
    }
    else
    {
        gstScldmaBufferQueue[enClientType].u8InQueueCount = 0;
    }
}

void DrvSclDmaPeekBufferQueue(DrvSclDmaBufferQueueConfig_t *pCfg)
{
    DrvSclDmaClientType_e enClientType;
    enClientType = _Drv_SCLDMA_TransToClientType(pCfg->enID ,pCfg->enRWMode);
    if(_Is_SWRingModeBufferReady(enClientType))
    {
        DRV_SCLDMA_MUTEX_LOCK_ISR();
        _DrvSclDmaGetInQueueCount(enClientType);
        pCfg->pstRead = gstScldmaBufferQueue[enClientType].pstRead;
        pCfg->u8InQueueCount = gstScldmaBufferQueue[enClientType].u8InQueueCount;
        pCfg->u8AccessId = gstScldmaBufferQueue[enClientType].u8AccessId;
        DRV_SCLDMA_MUTEX_UNLOCK_ISR();

        //sclprintf("[DRVSCLDMA]%d PEEK W_P:%hhd R_P:%hhd Count:%hhd@:%lu\n",enClientType,
        //pCfg->pstWriteAlready->u8FrameAddrIdx, pCfg->pstRead->u8FrameAddrIdx,pCfg->u8InQueueCount,(u32)DrvSclOsGetSystemTimeStamp());
        SCL_DBG(SCL_DBG_LV_DRVSCLDMA()&Get_DBGMG_SCLDMAclient(enClientType,1),
            "[DRVSCLDMA]%d W_P:%hhd R_P:%hhd Count:%hhd@:%lu\n",enClientType,
        pCfg->pstWrite->u8FrameAddrIdx, pCfg->pstRead->u8FrameAddrIdx,pCfg->u8InQueueCount,(u32)DrvSclOsGetSystemTimeStamp());
    }
    else
    {
        DRV_SCLDMA_ERR(sclprintf("[DRVSCLDMA] enClientType:%d non-used Buffer Queue\n",enClientType ));
    }
}
void _DrvSclDmaBufferClearQueue(DrvSclDmaClientType_e enClientType,u8 u8idx)
{
    DrvSclDmaFrameBufferConfig_t *stCfg;
    u8 u8BufferIdx;
    if(_Is_SWRingModeBufferReady(enClientType))
    {
        DRV_SCLDMA_MUTEX_LOCK_ISR();
        stCfg = gstScldmaBufferQueue[enClientType].pstHead;
        for(u8BufferIdx = 0; u8BufferIdx<=gstScldmaInfo.bMaxid[enClientType]; u8BufferIdx++)
        {
            if(stCfg->u8FrameAddrIdx == u8idx && stCfg->u32FrameAddr != 0)
            {
                DrvSclOsMemset(stCfg,0,SCLDMA_BUFFER_QUEUE_OFFSET);
                break;
            }
            else
            {
                stCfg += 1;
            }
        }
        DRV_SCLDMA_MUTEX_UNLOCK_ISR();
    }
}
DrvSclDmaFrameBufferConfig_t * _DrvSclDmaMoveBufferQueueReadPoint(DrvSclDmaClientType_e enClientType)
{
    DrvSclDmaFrameBufferConfig_t* pstCfg;
    DRV_SCLDMA_MUTEX_LOCK_ISR();
    pstCfg = gstScldmaBufferQueue[enClientType].pstRead;
    gstScldmaBufferQueue[enClientType].pstRead = gstScldmaBufferQueue[enClientType].pstRead + 1;
    if(gstScldmaBufferQueue[enClientType].pstRead >=gstScldmaBufferQueue[enClientType].pstTail)
    {
        gstScldmaBufferQueue[enClientType].pstRead = gstScldmaBufferQueue[enClientType].pstHead;
    }
    if(_Is_SWRingModeBufferFull(enClientType))
    {
        gstScldmaBufferQueue[enClientType].bFull = 0;
    }
    _SetANDGetIdxType(enClientType,(pstCfg->u8FrameAddrIdx<<4),E_DRV_SCLDMA_ACTIVE_BUFFER_SCL);
    DRV_SCLDMA_MUTEX_UNLOCK_ISR();
    return pstCfg;
}
void DrvSclDmaBufferDeQueue(DrvSclDmaBufferQueueConfig_t *pCfg)
{
    DrvSclDmaClientType_e enClientType;
    u8 u8count;
    enClientType = _Drv_SCLDMA_TransToClientType(pCfg->enID ,pCfg->enRWMode);
    if(_Is_SWRingModeBufferReady(enClientType))
    {
        u8count = gstScldmaBufferQueue[enClientType].u8InQueueCount;
        if(u8count)
        {
            pCfg->pstRead = _DrvSclDmaMoveBufferQueueReadPoint(enClientType);
        }
        DRV_SCLDMA_MUTEX_LOCK_ISR();
        _DrvSclDmaGetInQueueCount(enClientType);
        DRV_SCLDMA_MUTEX_UNLOCK_ISR();
        pCfg->pstHead = gstScldmaBufferQueue[enClientType].pstHead;
        pCfg->pstTail = gstScldmaBufferQueue[enClientType].pstTail;
        pCfg->pstWrite = gstScldmaBufferQueue[enClientType].pstWrite;
        pCfg->pstWriteAlready = gstScldmaBufferQueue[enClientType].pstWriteAlready;
        pCfg->u8InQueueCount = gstScldmaBufferQueue[enClientType].u8InQueueCount;
        //sclprintf("[DRVSCLDMA]%d DeQueue :R_P:%lx R_P:%hhd Count:%hhd@:%lu\n",enClientType,
        //    pCfg->pstRead->u32FrameAddr, pCfg->pstRead->u8FrameAddrIdx,gstScldmaBufferQueue[enClientType].u8InQueueCount,(u32)DrvSclOsGetSystemTime());
        SCL_DBG(SCL_DBG_LV_DRVSCLDMA()&Get_DBGMG_SCLDMAclient(enClientType,1),
            "[DRVSCLDMA]%d DeQueue :W_P:%hhd R_P:%hhd Count:%hhd@:%lu\n",enClientType,
        pCfg->pstWriteAlready->u8FrameAddrIdx,gstScldmaBufferQueue[enClientType].pstRead->u8FrameAddrIdx,gstScldmaBufferQueue[enClientType].u8InQueueCount,(u32)DrvSclOsGetSystemTimeStamp());
    }
    else
    {
        DRV_SCLDMA_ERR(sclprintf("[DRVSCLDMA] enClientType:%d non-used Buffer Queue\n",enClientType ));
    }
}
void _DrvSclDmaBufferFillQueue(DrvSclDmaFrameBufferConfig_t *stCfg,const DrvSclDmaFrameBufferConfig_t stTarget)
{
    DrvSclOsMemcpy(stCfg,&stTarget,SCLDMA_BUFFER_QUEUE_OFFSET);
}
void _DrvSclDmaMoveBufferQueueWritePoint(DrvSclDmaClientType_e enClientType)
{
    gstScldmaBufferQueue[enClientType].pstWriteAlready = gstScldmaBufferQueue[enClientType].pstWrite;
    gstScldmaBufferQueue[enClientType].pstWrite += 1;
    if(gstScldmaBufferQueue[enClientType].pstWrite >=gstScldmaBufferQueue[enClientType].pstTail)
    {
        gstScldmaBufferQueue[enClientType].pstWrite = gstScldmaBufferQueue[enClientType].pstHead;
    }

    if(gstScldmaBufferQueue[enClientType].pstWrite == gstScldmaBufferQueue[enClientType].pstRead)
    {
        gstScldmaBufferQueue[enClientType].bFull = 1;
    }
}
u8 DrvSclDmaGetDoneBufferIdx(DrvSclDmaClientType_e enClientType)
{
    u8 u8BufIdx;
    u8BufIdx = gstScldmaBufferQueue[enClientType].pstWriteAlready->u8FrameAddrIdx;
    return u8BufIdx;
}
u8 DrvSclDmaGetNextBufferIdx(DrvSclDmaClientType_e enClientType)
{
    u8 u8BufIdx;
    u8BufIdx = gstScldmaBufferQueue[enClientType].u8NextActiveId;
    if(u8BufIdx == gstScldmaInfo.bMaxid[enClientType])
    {
        u8BufIdx = 0;
    }
    else
    {
        u8BufIdx++;
    }
    return u8BufIdx;
}
u8 DrvSclDmaGetActiveBufferIdx(DrvSclDmaClientType_e enClientType)
{
    u8 u8BufIdx;
    u8BufIdx = gstScldmaBufferQueue[enClientType].u8NextActiveId;
    return u8BufIdx;
}
void _DrvSclDmaDisableBufferAccess(DrvSclDmaClientType_e enClientType,u8 u8FrameAddrIdx)
{
    gstScldmaBufferQueue[enClientType].u8Bufferflag &= ~(0x1<<u8FrameAddrIdx);
}
void _DrvSclDmaEnableBufferAccess(DrvSclDmaClientType_e enClientType,u8 u8FrameAddrIdx)
{
    DRV_SCLDMA_MUTEX_LOCK_ISR();
    gstScldmaBufferQueue[enClientType].u8Bufferflag |= (0x1<<u8FrameAddrIdx);
    gstScldmaBufferQueue[enClientType].u8AccessId = u8FrameAddrIdx;
    DRV_SCLDMA_MUTEX_UNLOCK_ISR();
}
bool _DrvSclDmaMakeSureBufferIsReady(DrvSclDmaClientType_e enClientType,u8 u8FrameAddrIdx)
{
    return (gstScldmaBufferQueue[enClientType].u8Bufferflag & (0x1<<u8FrameAddrIdx));
}
void DrvSclDmaEnableBufferAccess(DrvSclDmaBufferQueueConfig_t *pCfg)
{
    DrvSclDmaClientType_e enClientType;
    enClientType = _Drv_SCLDMA_TransToClientType(pCfg->enID ,pCfg->enRWMode);
    //sclprintf("[DRVSCLDMA]EnableBufferAccess%d :%hhd\n",enClientType,pCfg->u8NextActiveId);
    SCL_DBG(SCL_DBG_LV_DRVSCLDMA()&Get_DBGMG_SCLDMAclient(enClientType,1),
        "[DRVSCLDMA]EnableBufferAccess%d :%hhd\n",enClientType,pCfg->u8AccessId);
    _DrvSclDmaBufferClearQueue(enClientType,pCfg->u8AccessId);
    _DrvSclDmaEnableBufferAccess(enClientType,pCfg->u8AccessId);
}
bool DrvSclDmaMakeSureNextActiveId(DrvSclDmaClientType_e enClientType)
{
    u8 u8BufferIdx;
    u8 u8FrameAddrIdx;
    u8FrameAddrIdx = DrvSclDmaGetNextBufferIdx(enClientType);
    for(u8BufferIdx = 0; u8BufferIdx<gstScldmaInfo.bMaxid[enClientType]; u8BufferIdx++)
    {
        if(_DrvSclDmaMakeSureBufferIsReady(enClientType,u8FrameAddrIdx))
        {
            gstScldmaBufferQueue[enClientType].u8NextActiveId = u8FrameAddrIdx;
            //sclprintf("[DRVSCLDMA]  enClientType:%d NextID:%hhd",enClientType,u8FrameAddrIdx);
            SCL_DBG(SCL_DBG_LV_DRVSCLDMA()&Get_DBGMG_SCLDMAclient(enClientType,1),
                "[DRVSCLDMA]  enClientType:%d NextID:%hhd",enClientType,u8FrameAddrIdx);
            return 1;
        }
        else
        {
            u8FrameAddrIdx = u8FrameAddrIdx + 1;
            if(u8FrameAddrIdx>gstScldmaInfo.bMaxid[enClientType])
            {
                u8FrameAddrIdx = 0;
            }

            if(_Is_IdxRingCircuit(enClientType,u8BufferIdx))
            {
                //DRV_SCLDMA_ERR(sclprintf("[DRVSCLDMA] enClientType:%d ChangeBufferIdx Fail\n",enClientType));
                SCL_DBG(SCL_DBG_LV_DRVSCLDMA()&Get_DBGMG_SCLDMAclient(enClientType,1),
                    "[DRVSCLDMA] enClientType:%d ChangeBufferIdx Fail\n",enClientType);
            }
        }
    }
    return 0;
}
void _DrvSclDmaChangeSc3RBufferOnlyForSc3Mode(DrvSclDmaClientType_e enClientType,u8 u8FrameAddrIdx)
{
    if(enClientType ==E_DRV_SCLDMA_3_FRM_W)
    {
        HalSclDmaSetDMAOutputBufferAddr(E_DRV_SCLDMA_3_FRM_R,
            gstScldmaInfo.u32Base_Y[enClientType][u8FrameAddrIdx],
            gstScldmaInfo.u32Base_C[enClientType][u8FrameAddrIdx],
            gstScldmaInfo.u32Base_V[enClientType][u8FrameAddrIdx]);
    }
}
void DrvSclDmaChangeBufferIdx(DrvSclDmaClientType_e enClientType)
{
    u8 u8FrameAddrIdx;
    u8FrameAddrIdx = gstScldmaBufferQueue[enClientType].u8NextActiveId;
    HalSclDmaSetDMAOutputBufferAddr(enClientType,
        gstScldmaInfo.u32Base_Y[enClientType][u8FrameAddrIdx],
        gstScldmaInfo.u32Base_C[enClientType][u8FrameAddrIdx],
        gstScldmaInfo.u32Base_V[enClientType][u8FrameAddrIdx]);
    _DrvSclDmaChangeSc3RBufferOnlyForSc3Mode(enClientType,u8FrameAddrIdx);

}
u8 _DrvSclDmaGetIspCount(void)
{
    return ((gu8ISPcount==0x7F) ? 0 : (gu8ISPcount +1));
}
void DrvSclDmaSetFrameResolution(DrvSclDmaClientType_e enClientType)
{
    gstScldmaInfo.u16FrameHeight[enClientType] = HalSclDmaGetOutputVsize(enClientType);
    gstScldmaInfo.u16FrameWidth[enClientType] = HalSclDmaGetOutputHsize(enClientType);
}
u8 DrvSclDmaBufferEnQueue(DrvSclDmaClientType_e enClientType,DrvSclDmaFrameBufferConfig_t stTarget)
{
    if(_Is_SWRingModeBufferReady(enClientType) && _Is_SWRingModeBufferNotFull(enClientType))
    {
        stTarget.u8ISPcount = _DrvSclDmaGetIspCount();
        _DrvSclDmaBufferFillQueue(gstScldmaBufferQueue[enClientType].pstWrite,stTarget);
        _DrvSclDmaMoveBufferQueueWritePoint(enClientType);
        _DrvSclDmaDisableBufferAccess(enClientType,stTarget.u8FrameAddrIdx);
        _DrvSclDmaGetInQueueCount(enClientType);

        //sclprintf("[DRVSCLDMA]%d EnQueue :W_P:%hhd R_P:%hhd Count:%hhd@:%lu\n",enClientType,
        //gstScldmaBufferQueue[enClientType].pstWriteAlready->u8FrameAddrIdx,
        //gstScldmaBufferQueue[enClientType].pstRead->u8FrameAddrIdx,
        //gstScldmaBufferQueue[enClientType].u8InQueueCount,(u32)DrvSclOsGetSystemTimeStamp());
        SCL_DBG(SCL_DBG_LV_DRVSCLDMA()&Get_DBGMG_SCLDMAclient(enClientType,1),
            "[DRVSCLDMA]%d EnQueue :W_P:%hhd R_P:%hhd Count:%hhd@:%lu\n",enClientType,
        gstScldmaBufferQueue[enClientType].pstWriteAlready->u8FrameAddrIdx,
        gstScldmaBufferQueue[enClientType].pstRead->u8FrameAddrIdx,
        gstScldmaBufferQueue[enClientType].u8InQueueCount,(u32)DrvSclOsGetSystemTimeStamp());
        return 1;
    }
    else if(_Is_SWRingModeBufferFull(enClientType))
    {
        //sclprintf("[DRVSCLDMA]%d EnQueue Fail (FULL) :W_P:%hhd R_P:%hhd Count:%hhd@:%lu\n",enClientType,
        //gstScldmaBufferQueue[enClientType].pstWrite->u8FrameAddrIdx,
        //gstScldmaBufferQueue[enClientType].pstRead->u8FrameAddrIdx,
        //gstScldmaBufferQueue[enClientType].u8InQueueCount,(u32)DrvSclOsGetSystemTimeStamp());
        SCL_DBG(SCL_DBG_LV_DRVSCLDMA()&Get_DBGMG_SCLDMAclient(enClientType,1),
            "[DRVSCLDMA]%d EnQueue Fail (FULL) :W_P:%hhd R_P:%hhd Count:%hhd@:%lu\n",enClientType,
        gstScldmaBufferQueue[enClientType].pstWrite->u8FrameAddrIdx,
        gstScldmaBufferQueue[enClientType].pstRead->u8FrameAddrIdx,
        gstScldmaBufferQueue[enClientType].u8InQueueCount,(u32)DrvSclOsGetSystemTimeStamp());
        return 0;
    }
    else
    {
        DRV_SCLDMA_ERR(sclprintf("[DRVSCLDMA] enClientType:%d non-used Buffer Queue\n",enClientType ));
        return 0;
    }
}
u32 _DrvSclDmaGetEventById(DrvSclDmaIdType_e enSCLDMA_ID)
{
    u32 u32Events;
    DrvSclIrqTxEventType_e enClearEventById = (enSCLDMA_ID ==E_DRV_SCLDMA_ID_1_W)? E_DRV_SCLIRQ_EVENT_SC1POLL : E_DRV_SCLIRQ_EVENT_SC2POLL;
    u32Events = DrvSclOsGetEvent(DrvSclIrqGetIrqEventID());
    DrvSclOsClearEventIRQ(DrvSclIrqGetIrqEventID(),(u32Events & (enClearEventById)));
    return u32Events;
}
bool _DrvSclDmaGetSc3DoneFlagAndClearSc3Event(u32 u32Events)
{
    bool bDone;
    if((u32Events & E_DRV_SCLIRQ_SC3EVENT_DONE) == E_DRV_SCLIRQ_SC3EVENT_DONE)  //frmend
    {
        bDone = FRM_POLLIN;
    }
    else if((u32Events & (E_DRV_SCLIRQ_SC3EVENT_RESTART)))  //frmend
    {
        bDone = FRM_POLLIN;
    }
    else if(u32Events & E_DRV_SCLIRQ_EVENT_RESUME)
    {
        bDone = FRM_POLLIN;
    }
    else   //timeout
    {
        bDone = FALSE;
    }
    if(bDone)
    {
        DrvSclOsClearEventIRQ(DrvSclIrqGetIrqSC3EventID(),(u32Events & E_DRV_SCLIRQ_SC3EVENT_POLL));
    }
    return bDone;
}
bool _DrvSclDmaGetSc2DoneFlag(u32 u32Events)
{
    bool bSINGLE_SKIP = 0;
    bool bDone;
    bSINGLE_SKIP = (u32Events&E_DRV_SCLIRQ_EVENT_SC2FRM)?
        ((_IsFlagType(E_DRV_SCLDMA_2_FRM_W,E_DRV_SCLDMA_FLAG_DROP)) || !(_IsFlagType(E_DRV_SCLDMA_2_FRM_W,E_DRV_SCLDMA_FLAG_FRMIN))):
                    (u32Events&E_DRV_SCLIRQ_EVENT_SC2FRM2)?
         ((_IsFlagType(E_DRV_SCLDMA_2_FRM2_W,E_DRV_SCLDMA_FLAG_DROP)) || !(_IsFlagType(E_DRV_SCLDMA_2_FRM2_W,E_DRV_SCLDMA_FLAG_FRMIN)))
                                                     : 0;
    if((u32Events & (E_DRV_SCLIRQ_EVENT_SC2)) && bSINGLE_SKIP)  //ISP FIFO FULL
    {
        bDone = SINGLE_SKIP;
        if((u32Events & (E_DRV_SCLIRQ_EVENT_FRM2RESTART|E_DRV_SCLIRQ_EVENT_SC2RESTART))==
            (E_DRV_SCLIRQ_EVENT_FRM2RESTART|E_DRV_SCLIRQ_EVENT_SC2RESTART))  //frmrestart
        {
            bDone = FRM_POLLIN|FRM2_POLLIN;
        }
        else if((u32Events & (E_DRV_SCLIRQ_EVENT_RESUME)))  //frmrestart
        {
            bDone = FRM_POLLIN|FRM2_POLLIN;
        }
        else if((u32Events & (E_DRV_SCLIRQ_EVENT_FRM2RESTART)))  //frmrestart
        {
            bDone = FRM_POLLIN;
        }
        else if((u32Events & (E_DRV_SCLIRQ_EVENT_SC2RESTART)))  //frmrestart
        {
            bDone = FRM2_POLLIN;
        }
    }
    else if((u32Events & (E_DRV_SCLIRQ_EVENT_SC2))==(E_DRV_SCLIRQ_EVENT_SC2))
    {
        bDone = FRM_POLLIN |FRM2_POLLIN;
    }
    else if((u32Events & (E_DRV_SCLIRQ_EVENT_FRM2RESTART|E_DRV_SCLIRQ_EVENT_SC2RESTART))==
        (E_DRV_SCLIRQ_EVENT_FRM2RESTART|E_DRV_SCLIRQ_EVENT_SC2RESTART))  //frmrestart
    {
        bDone = FRM_POLLIN|FRM2_POLLIN;
    }
    else if((u32Events & (E_DRV_SCLIRQ_EVENT_SC2FRM)))  //frmend
    {
        bDone = FRM_POLLIN;
    }
    else if((u32Events & (E_DRV_SCLIRQ_EVENT_SC2FRM2)))  //frmend
    {
        bDone = FRM2_POLLIN;
    }
    else if((u32Events & (E_DRV_SCLIRQ_EVENT_SC2RESTART)))  //frmrestart
    {
        bDone = FRM_POLLIN;
    }
    else if((u32Events & (E_DRV_SCLIRQ_EVENT_FRM2RESTART)))  //frmrestart
    {
        bDone = FRM2_POLLIN;
    }
    else if(u32Events & E_DRV_SCLIRQ_EVENT_RESUME)
    {
        bDone = FRM_POLLIN|FRM2_POLLIN;
    }
    else   //timeout
    {
        bDone = FALSE;
    }
    return bDone;
}
bool _DrvSclDmaGetSc1DoneFlag(u32 u32Events)
{
    bool bSINGLE_SKIP = 0;
    bool bDone;
    bSINGLE_SKIP = (u32Events&E_DRV_SCLIRQ_EVENT_SC1SNP)?
        ((_IsFlagType(E_DRV_SCLDMA_1_SNP_W,E_DRV_SCLDMA_FLAG_DROP)) || !(_IsFlagType(E_DRV_SCLDMA_1_SNP_W,E_DRV_SCLDMA_FLAG_FRMIN))):
                    (u32Events&E_DRV_SCLIRQ_EVENT_SC1FRM)?
        ((_IsFlagType(E_DRV_SCLDMA_1_FRM_W,E_DRV_SCLDMA_FLAG_DROP)) || !(_IsFlagType(E_DRV_SCLDMA_1_FRM_W,E_DRV_SCLDMA_FLAG_FRMIN)))
                        : 0;
    if((u32Events & (E_DRV_SCLIRQ_EVENT_SC1)) && bSINGLE_SKIP)  //ISP FIFO FULL
    {
        bDone = SINGLE_SKIP;
        if((u32Events & (E_DRV_SCLIRQ_EVENT_SC1RESTART|E_DRV_SCLIRQ_EVENT_SNPRESTART))==
            (E_DRV_SCLIRQ_EVENT_SC1RESTART|E_DRV_SCLIRQ_EVENT_SNPRESTART))  //frmrestart
        {
            bDone = FRM_POLLIN|SNP_POLLIN;
        }
        else if((u32Events & (E_DRV_SCLIRQ_EVENT_SC1RESTART)))  //frmrestart
        {
            bDone = FRM_POLLIN;
        }
        else if((u32Events & (E_DRV_SCLIRQ_EVENT_SNPRESTART)))  //frmrestart
        {
            bDone = SNP_POLLIN;
        }
        else if((u32Events & (E_DRV_SCLIRQ_EVENT_RESUME)))  //frmrestart
        {
            bDone = FRM_POLLIN|SNP_POLLIN;
        }
    }
    else if((u32Events & (E_DRV_SCLIRQ_EVENT_SC1))==(E_DRV_SCLIRQ_EVENT_SC1))
    {
        bDone = FRM_POLLIN |SNP_POLLIN;
    }
    else if((u32Events & (E_DRV_SCLIRQ_EVENT_SC1RESTART|E_DRV_SCLIRQ_EVENT_SNPRESTART))==
        (E_DRV_SCLIRQ_EVENT_SC1RESTART|E_DRV_SCLIRQ_EVENT_SNPRESTART))  //frmrestart
    {
        bDone = FRM_POLLIN|SNP_POLLIN;
    }
    else if((u32Events & (E_DRV_SCLIRQ_EVENT_SC1FRM)))  //frmend
    {
        bDone = FRM_POLLIN;
    }
    else if((u32Events & (E_DRV_SCLIRQ_EVENT_SC1SNP)))  //frmend
    {
        bDone = SNP_POLLIN;
    }
    else if((u32Events & (E_DRV_SCLIRQ_EVENT_SC1RESTART)))  //frmrestart
    {
        bDone = FRM_POLLIN;
    }
    else if((u32Events & (E_DRV_SCLIRQ_EVENT_SNPRESTART)))  //frmrestart
    {
        bDone = SNP_POLLIN;
    }
    else if(u32Events & E_DRV_SCLIRQ_EVENT_RESUME)
    {
        bDone = FRM_POLLIN|SNP_POLLIN;
    }
    else   //timeout
    {
        bDone = FALSE;
    }
    return bDone;
}
bool _DrvSclDmaGetReadDoneEvent(DrvSclDmaClientType_e enClientType)
{
    bool bDone;
    if( _Is_SingleMode(enClientType))
    {
        u16 u16IrqNum;
        u64 u64Flag, u64Flag_N;

        u16IrqNum = _DrvSclDmaGetActiveIrqNum(enClientType);

        DrvSclIrqGetFlag(u16IrqNum, &u64Flag);
        DrvSclIrqGetFlag(u16IrqNum+1, &u64Flag_N);
        if(u64Flag && u64Flag_N )
        {
            bDone = TRUE;
        }
        else
        {
            bDone = FALSE;
        }
    }
    else if ( _Is_RingMode(enClientType))
    {
        bDone = TRUE;
    }
    else if(_Is_SWRingMode(enClientType))
    {
        bDone = TRUE;
    }
    else
    {
        bDone = FALSE;
    }
    return bDone;
}
bool DrvSclDmaGetDmaDoneEvent(DrvSclDmaIdType_e enSCLDMA_ID, DrvSclDmaDoneConfig_t *pCfg)
{
    bool bRet = 1;
    DrvSclDmaClientType_e enClientType;
    u32 u32Time   = 0;
    u32 u32Events = 0;
    u32 u32DiffTime   = 0;
    static u32 u32SC3Polltime = 0;
    u8 u8Count = 0;
    u32Time = ((u32)DrvSclOsGetSystemTimeStamp());
    enClientType = _Drv_SCLDMA_TransToClientType(enSCLDMA_ID ,pCfg->enRWMode);
    if(enSCLDMA_ID ==E_DRV_SCLDMA_ID_PNL_R || enSCLDMA_ID ==E_DRV_SCLDMA_ID_3_R )
    {
        _DrvSclDmaGetReadDoneEvent(enClientType);
        SCL_DBG(SCL_DBG_LV_DRVSCLDMA()&Get_DBGMG_SCLDMAclient(enClientType,1),
            "[DRVSCLDMA]%s:: Client:%s bDone=%x @:%lu\n",
            __FUNCTION__, PARSING_SCLDMA_ID(enSCLDMA_ID), pCfg->bDone,u32Time);
    }
    else if(enSCLDMA_ID ==E_DRV_SCLDMA_ID_3_W )
    {
        u32Events = DrvSclOsGetEvent(DrvSclIrqGetIrqSC3EventID());
        pCfg->bDone = _DrvSclDmaGetSc3DoneFlagAndClearSc3Event(u32Events);
        u32DiffTime = (u32Time>=u32SC3Polltime) ? (u32Time-u32SC3Polltime): u32Time;
        if(pCfg->bDone)
        {
            gu32SendTime[E_DRV_SCLDMA_ID_3_R] = u32Time;
            gbSendPoll[E_DRV_SCLDMA_ID_3_R] = pCfg->bDone;
            gu32SendTime[E_DRV_SCLDMA_ID_3_W] = u32Time;
            gbSendPoll[E_DRV_SCLDMA_ID_3_W] = pCfg->bDone;
            if(gstScldmaInfo.bDMAOnOff[E_DRV_SCLDMA_3_FRM_W])
            {
                SCL_DBGERR("%s Delaly\n",__FUNCTION__);
                DrvSclOsDelayTask(20);
            }
        }
        else if(gbSendPoll[E_DRV_SCLDMA_3_FRM_W]&0x10 && (u32DiffTime>OMX_VSPL_POLLTIME))
        {
            //poll time out
            if(gstScldmaInfo.enBuffMode[E_DRV_SCLDMA_3_FRM_W]== E_DRV_SCLDMA_BUF_MD_SINGLE &&
                gstScldmaInfo.bDMAOnOff[E_DRV_SCLDMA_3_FRM_W])
            {
                while(!pCfg->bDone)
                {
                    _Drv_SCLDMA_SWRegenVSyncTrigger(E_DRV_SCLDMA_VS_ID_SC3);
                    DrvSclOsWaitEvent(DrvSclIrqGetIrqSC3EventID(),E_DRV_SCLIRQ_SC3EVENT_DONE, &u32Events, E_DRV_SCLOS_EVENT_MD_OR, 50);
                    pCfg->bDone = _DrvSclDmaGetSc3DoneFlagAndClearSc3Event(u32Events);
                    u8Count++;
                    if(u8Count > 5)
                    {
                        SCL_DBGERR("[SCLDMA]SC3 ERR\n");
                        break;
                    }
                }
                if(pCfg->bDone)
                {
                    gu32SendTime[E_DRV_SCLDMA_3_FRM_R] = u32Time;
                    gbSendPoll[E_DRV_SCLDMA_3_FRM_R] = pCfg->bDone;
                    gu32SendTime[E_DRV_SCLDMA_3_FRM_W] = u32Time;
                    gbSendPoll[E_DRV_SCLDMA_3_FRM_W] = pCfg->bDone;
                }
            }
        }
        else
        {
            if(!(gbSendPoll[E_DRV_SCLDMA_3_FRM_W]&0x10))
            {
                u32SC3Polltime = u32Time;
            }
            gbSendPoll[E_DRV_SCLDMA_ID_3_R] |= 0x10;
            gbSendPoll[E_DRV_SCLDMA_ID_3_W] |= 0x10;
        }
        SCL_DBG(SCL_DBG_LV_DRVSCLDMA()&Get_DBGMG_SCLDMAclient(enClientType,0),
            "[DRVSCLDMA]%s:: Client:%s Event:%lx bDone=%x flag:%hhx@:%lu\n",
            __FUNCTION__, PARSING_SCLDMA_CLIENT(enClientType),u32Events, pCfg->bDone,
            gstScldmaInfo.bDmaflag[enClientType],u32Time);
    }
    else
    {
        if(enSCLDMA_ID ==E_DRV_SCLDMA_ID_1_W)
        {
            u32Events = _DrvSclDmaGetEventById(E_DRV_SCLDMA_ID_1_W);
            pCfg->bDone = _DrvSclDmaGetSc1DoneFlag(u32Events);
            if(pCfg->bDone)
            {
                if((pCfg->bDone&(SNP_POLLIN))||pCfg->bDone==SINGLE_SKIP)
                {
                    gu32SendTime[E_DRV_SCLDMA_1_SNP_W] = u32Time;
                    gbSendPoll[E_DRV_SCLDMA_1_SNP_W] = pCfg->bDone;
                }
                if((pCfg->bDone&(FRM_POLLIN))||pCfg->bDone==SINGLE_SKIP)
                {
                    gu32SendTime[E_DRV_SCLDMA_1_FRM_W] = u32Time;
                    gbSendPoll[E_DRV_SCLDMA_1_FRM_W] = pCfg->bDone;
                }
            }
            else
            {
                gbSendPoll[E_DRV_SCLDMA_1_SNP_W] |= 0x10;
                gbSendPoll[E_DRV_SCLDMA_1_FRM_W] |= 0x10;
            }
        }
        else if(enSCLDMA_ID ==E_DRV_SCLDMA_ID_2_W)
        {
            u32Events = _DrvSclDmaGetEventById(E_DRV_SCLDMA_ID_2_W);
            pCfg->bDone = _DrvSclDmaGetSc2DoneFlag(u32Events);
            if(pCfg->bDone)
            {
                if((pCfg->bDone&(FRM2_POLLIN))||pCfg->bDone==SINGLE_SKIP)
                {
                    gu32SendTime[E_DRV_SCLDMA_2_FRM2_W] = u32Time;
                    gbSendPoll[E_DRV_SCLDMA_2_FRM2_W] = pCfg->bDone;
                }
                if((pCfg->bDone&(FRM_POLLIN))||pCfg->bDone==SINGLE_SKIP)
                {
                    gu32SendTime[E_DRV_SCLDMA_2_FRM_W] = u32Time;
                    gbSendPoll[E_DRV_SCLDMA_2_FRM_W] = pCfg->bDone;
                }
            }
            else
            {
                gbSendPoll[E_DRV_SCLDMA_2_FRM2_W] |= 0x10;
                gbSendPoll[E_DRV_SCLDMA_2_FRM_W] |= 0x10;
            }
        }
        SCL_DBG(SCL_DBG_LV_DRVSCLDMA()&Get_DBGMG_SCLDMAclient(enClientType,1),"[DRVSCLDMA]%s:: Client:%s Event:%lx bDone=%x\n",
             __FUNCTION__, PARSING_SCLDMA_CLIENT(enClientType),u32Events, pCfg->bDone);
    }
    return bRet;
}
bool _DrvSclDmaGetLastTimePoint(DrvSclDmaClientType_e enClientType)
{
    u8 u8LPoint = 0;
    u8LPoint = _GetIdxType(enClientType,E_DRV_SCLDMA_ACTIVE_BUFFER_SCL);
    return u8LPoint;
}
bool _DrvSclDmaGetDmaWritePoint(DrvSclDmaClientType_e enClientType, u8 u8LPoint)
{
    u8 u8WPoint = 0;
    if(_Is_DMAClientOn(enClientType) &&_IsFlagType(enClientType,E_DRV_SCLDMA_FLAG_BLANKING))
    {
        u8WPoint = 0x0F;
    }
    else if( _IsFlagType(enClientType,E_DRV_SCLDMA_FLAG_ACTIVE))
    {
        u8WPoint =(u8LPoint==0) ? gstScldmaInfo.bMaxid[enClientType] : u8LPoint-1;
    }
    else
    {
        u8WPoint = u8LPoint;
    }
    return u8WPoint;
}
bool _DrvSclDmaGetAppReadPoint(DrvSclDmaClientType_e enClientType ,u8 u8AppInfo)
{
    u8 u8RPoint = 0;
    if(_Is_DMAClientOn(enClientType) &&(_IsFlagType(enClientType,E_DRV_SCLDMA_FLAG_BLANKING)))
    {
        u8RPoint = gstScldmaInfo.bMaxid[enClientType];
    }
    else
    {
        u8RPoint = (u8AppInfo & E_DRV_SCLDMA_ACTIVE_BUFFER_OMX_FLAG) ?
            (u8AppInfo &0x0F ) : (gstScldmaInfo.bDMAidx[enClientType]>>4)&E_DRV_SCLDMA_ACTIVE_BUFFER_SCL ;
        if((u8AppInfo & E_DRV_SCLDMA_ACTIVE_BUFFER_OMX_FLAG))
        {
            SCL_DBG(SCL_DBG_LV_DRVSCLDMA()&Get_DBGMG_SCLDMAclient(enClientType,1),
                "[DRVSCLDMA] enClientType:%d Update Rp %hhd \n",enClientType,u8RPoint);
        }
    }
    return u8RPoint;
}
void _DrvSclDmaDmaOffAtActiveTimeWhetherNeedReOpen(DrvSclDmaPointConfig_t *stPointCfg)
{
    if(stPointCfg->bRWequal&&(_IsFlagType(stPointCfg->enClientType,E_DRV_SCLDMA_FLAG_DMAOFF)))
    {
        if(DrvSclDmaSetISRHandlerDmaOff(stPointCfg->enClientType,1))
        {
            DRV_SCLDMA_MUTEX_LOCK_ISR();
            _ReSetFlagType(stPointCfg->enClientType,(E_DRV_SCLDMA_FLAG_EVERDMAON|E_DRV_SCLDMA_FLAG_DMAOFF));
            _ResetTrigCount(stPointCfg->enClientType);
            DrvSclDmaSetSWReTrigCount(stPointCfg->enClientType,0);
            DRV_SCLDMA_MUTEX_UNLOCK_ISR();
            SCL_DBG(SCL_DBG_LV_DRVSCLDMA()&Get_DBGMG_SCLDMAclient(stPointCfg->enClientType,1),
                "[DRVSCLDMA] enClientType:%d ReOpen \n",stPointCfg->enClientType);
        }
        else
        {
            DRV_SCLDMA_ERR(sclprintf(
                "[DRVSCLDMA] enClientType:%d strong collisionR:%hhd\n",stPointCfg->enClientType,stPointCfg->u8RPoint ));
        }
    }
}
void _DrvSclDmaSetDmaOnWhenDMAOff
    (DrvSclDmaIdType_e enSCLDMA_ID, DrvSclDmaClientType_e enClientType, DrvSclDmaActiveBufferConfig_t *pCfg)
{
    DrvSclIrqTxEventType_e enType;
    pCfg->stOnOff.bEn = 1;
    enType = (enClientType==E_DRV_SCLDMA_1_FRM_W) ? E_DRV_SCLIRQ_EVENT_SC1RESTART :
             (enClientType==E_DRV_SCLDMA_1_SNP_W) ? E_DRV_SCLIRQ_EVENT_SNPRESTART :
             (enClientType==E_DRV_SCLDMA_2_FRM_W) ? E_DRV_SCLIRQ_EVENT_SC2RESTART :
             (enClientType==E_DRV_SCLDMA_2_FRM2_W) ? E_DRV_SCLIRQ_EVENT_FRM2RESTART :
                    0;
    DrvSclDmaSetDmaClientOnOff(enSCLDMA_ID ,pCfg->stOnOff);
    DrvSclOsSetEventIrq(DrvSclIrqGetIrqEventID(), enType);
}
u8 _DrvSclDmaGetDoneIdxAndFlagWhenDMAOff(DrvSclDmaPointConfig_t *stPointCfg)
{
    u8 u8ActiveBuffer;
    if((_IsFlagType(stPointCfg->enClientType,E_DRV_SCLDMA_FLAG_EVERDMAON)) && !stPointCfg->bRPointChange)
    {
        u8ActiveBuffer = stPointCfg->u8WPoint | E_DRV_SCLDMA_ACTIVE_BUFFER_OMX_RINGFULL | E_DRV_SCLDMA_ACTIVE_BUFFER_OMX_TRIG;
    }
    else if((_IsFlagType(stPointCfg->enClientType,E_DRV_SCLDMA_FLAG_EVERDMAON)) && stPointCfg->bRPointChange)
    {
        u8ActiveBuffer = stPointCfg->u8WPoint|E_DRV_SCLDMA_ACTIVE_BUFFER_OMX_TRIG;
        DRV_SCLDMA_MUTEX_LOCK_ISR();
        _ReSetFlagType(stPointCfg->enClientType,E_DRV_SCLDMA_FLAG_EVERDMAON);
        DRV_SCLDMA_MUTEX_UNLOCK_ISR();
    }
    else
    {
        u8ActiveBuffer = stPointCfg->u8WPoint | E_DRV_SCLDMA_ACTIVE_BUFFER_OMX_TRIG;
    }
    return u8ActiveBuffer;
}
u8 _DrvSclDmaGetDoneIdxAndFlagWhenDmaOn(DrvSclDmaPointConfig_t *stPointCfg)
{
    u8 u8ActiveBuffer;
    if((_IsFlagType(stPointCfg->enClientType,E_DRV_SCLDMA_FLAG_EVERDMAON)) && !stPointCfg->bRPointChange)
    {
        u8ActiveBuffer = stPointCfg->u8WPoint | E_DRV_SCLDMA_ACTIVE_BUFFER_OMX_RINGFULL;
    }
    else if((_IsFlagType(stPointCfg->enClientType,E_DRV_SCLDMA_FLAG_EVERDMAON)) && stPointCfg->bRPointChange)
    {
        u8ActiveBuffer = stPointCfg->u8WPoint;
        DRV_SCLDMA_MUTEX_LOCK_ISR();
        _ReSetFlagType(stPointCfg->enClientType,E_DRV_SCLDMA_FLAG_EVERDMAON);
        DRV_SCLDMA_MUTEX_UNLOCK_ISR();
    }
    else
    {
        u8ActiveBuffer = stPointCfg->u8WPoint;
    }
    return u8ActiveBuffer;
}
DrvSclDmaPointConfig_t _DrvSclDmaGetPointConfig(DrvSclDmaClientType_e enClientType ,u8 u8AppInfo)
{
    DrvSclDmaPointConfig_t stPointCfg;
    stPointCfg.u8LPoint = _DrvSclDmaGetLastTimePoint(enClientType);
    stPointCfg.u8WPoint = _DrvSclDmaGetDmaWritePoint(enClientType, stPointCfg.u8LPoint);
    stPointCfg.u8RPoint = _DrvSclDmaGetAppReadPoint(enClientType,u8AppInfo);
    stPointCfg.bRWequal = (stPointCfg.u8WPoint == stPointCfg.u8RPoint)? 1 : 0;
    stPointCfg.bRPointChange = (u8AppInfo & E_DRV_SCLDMA_ACTIVE_BUFFER_OMX_FLAG)? 1 : 0;
    stPointCfg.enClientType = enClientType;
    return stPointCfg;
}
void _DrvSclDmaHandlerBufferWhenDmaOff
    (DrvSclDmaIdType_e enSCLDMA_ID, DrvSclDmaPointConfig_t *stPointCfg, DrvSclDmaActiveBufferConfig_t *pCfg)
{
    if(_Is_DMACanReOpen(stPointCfg->bRWequal,stPointCfg->bRPointChange) &&
        !(_IsFlagType(stPointCfg->enClientType,E_DRV_SCLDMA_FLAG_DMAFORCEOFF)))// for issue race condition of OMX update Rp and ioctl.
    {
        _DrvSclDmaSetDmaOnWhenDMAOff(enSCLDMA_ID,stPointCfg->enClientType,pCfg);
        pCfg->u8ActiveBuffer = 0x0F ;
    }
    else
    {
        pCfg->u8ActiveBuffer = _DrvSclDmaGetDoneIdxAndFlagWhenDMAOff(stPointCfg);
    }
}
void _DrvSclDmaHandlerBufferWhenDmaOn
    (DrvSclDmaPointConfig_t *stPointCfg, DrvSclDmaActiveBufferConfig_t *pCfg)
{
    _DrvSclDmaDmaOffAtActiveTimeWhetherNeedReOpen(stPointCfg);
    pCfg->u8ActiveBuffer = _DrvSclDmaGetDoneIdxAndFlagWhenDmaOn(stPointCfg);
}
bool DrvSclDmaGetDmaBufferDoneIdx(DrvSclDmaIdType_e enSCLDMA_ID, DrvSclDmaActiveBufferConfig_t *pCfg)
{
    DrvSclDmaClientType_e enClientType;
    DrvSclDmaPointConfig_t stPointCfg;
    enClientType = _Drv_SCLDMA_TransToClientType(enSCLDMA_ID ,pCfg->enRWMode);
#if ENABLE_RING_DB
    if(_Is_VsrcDoubleBufferNotOpen() && _Is_VsrcId(enSCLDMA_ID) && DoubleBufferStatus)
    {
        gVsrcDBnotOpen = 0;
        _Drv_SCLDMA_SetDoubleBufferOn(E_DRV_SCLDMA_ID_1_W, SCLIRQ_SC1_SNPI_W_ACTIVE, E_DRV_SCLDMA_1_IMI_W);
    }
#endif
    if(_Is_RingMode(enClientType))
    {
        stPointCfg = _DrvSclDmaGetPointConfig(enClientType,pCfg->u8ActiveBuffer);
        DRV_SCLDMA_MUTEX_LOCK_ISR();
        _SetANDGetIdxType(enClientType,(stPointCfg.u8RPoint<<4),E_DRV_SCLDMA_ACTIVE_BUFFER_SCL);
        DRV_SCLDMA_MUTEX_UNLOCK_ISR();
        if(_Is_DMAClientOn(enClientType))
        {
            _DrvSclDmaHandlerBufferWhenDmaOn(&stPointCfg,pCfg);
        }
        else
        {
            _DrvSclDmaHandlerBufferWhenDmaOff(enSCLDMA_ID,&stPointCfg,pCfg);
        }
        if(pCfg->u8ActiveBuffer == 0xF)
        {
            pCfg->u64FRMDoneTime    = gu64FRMDoneTime[enClientType][MAX_BUFFER_COUNT];
        }
        else
        {
            pCfg->u64FRMDoneTime    = gu64FRMDoneTime[enClientType][stPointCfg.u8WPoint];
            DRV_SCLDMA_MUTEX_LOCK_ISR();
            gu64FRMDoneTime[enClientType][MAX_BUFFER_COUNT] = pCfg->u64FRMDoneTime;
            DRV_SCLDMA_MUTEX_UNLOCK_ISR();
        }
    }
    else
    {
        pCfg->u8ActiveBuffer    = 0xFF;
        pCfg->u64FRMDoneTime    = gu64FRMDoneTime[enClientType][0];
    }
    pCfg->u8ISPcount        = gu8ISPcount;
    return 1;
}
void _DrvSclDmaHandlerBufferWhenDmaOnWithoutDoubleBuffer
    (DrvSclDmaPointConfig_t *stPointCfg, DrvSclDmaActiveBufferConfig_t *pCfg)
{
    pCfg->u8ActiveBuffer = _DrvSclDmaGetDoneIdxAndFlagWhenDmaOn(stPointCfg);
}
bool DrvSclDmaGetDmaBufferDoneIdxWithoutDoublebuffer
    (DrvSclDmaIdType_e enSCLDMA_ID, DrvSclDmaActiveBufferConfig_t *pCfg)
{
    DrvSclDmaClientType_e enClientType;
    DrvSclDmaPointConfig_t stPointCfg;
    enClientType = _Drv_SCLDMA_TransToClientType(enSCLDMA_ID ,pCfg->enRWMode);
    if(_Is_RingMode(enClientType))
    {
        stPointCfg = _DrvSclDmaGetPointConfig(enClientType,pCfg->u8ActiveBuffer);
        DRV_SCLDMA_MUTEX_LOCK_ISR();
        _SetANDGetIdxType(enClientType,(stPointCfg.u8RPoint<<4),E_DRV_SCLDMA_ACTIVE_BUFFER_SCL);
        if(stPointCfg.u8WPoint == 0xF)
        {
            pCfg->u64FRMDoneTime    = gu64FRMDoneTime[enClientType][MAX_BUFFER_COUNT];
        }
        else
        {
            pCfg->u64FRMDoneTime    = gu64FRMDoneTime[enClientType][stPointCfg.u8WPoint];
            gu64FRMDoneTime[enClientType][MAX_BUFFER_COUNT] = pCfg->u64FRMDoneTime;
        }
        DRV_SCLDMA_MUTEX_UNLOCK_ISR();
        if(_Is_DMAClientOn(enClientType))
        {
            _DrvSclDmaHandlerBufferWhenDmaOnWithoutDoubleBuffer(&stPointCfg,pCfg);
        }
        else
        {
            _DrvSclDmaHandlerBufferWhenDmaOff(enSCLDMA_ID,&stPointCfg,pCfg);
        }
    }
    else if(_Is_SingleMode(enClientType))
    {
        if(_IsFlagType(enClientType,E_DRV_SCLDMA_FLAG_ACTIVE))
        {
            pCfg->u8ActiveBuffer    = 0xFF;
        }
        else
        {
            pCfg->u8ActiveBuffer    = (0xFF | ~E_DRV_SCLDMA_ACTIVE_BUFFER_OMX_TRIG);
        }
        pCfg->u64FRMDoneTime    = gu64FRMDoneTime[enClientType][0];
    }
    else
    {
        pCfg->u8ActiveBuffer    = 0xFF;
        pCfg->u64FRMDoneTime    = gu64FRMDoneTime[enClientType][0];
    }
    pCfg->u8ISPcount        = gu8ISPcount;
    return 1;
}
void DrvSclDmaSetForceCloseDma(DrvSclDmaIdType_e enID,DrvSclDmaRwModeType_e enRWMode,bool bEn)
{
    DrvSclDmaClientType_e client;
    client    = _Drv_SCLDMA_TransToClientType(enID ,enRWMode);
    gbForceClose[client] = bEn;
}

void DrvSclDmaResetTrigCountByClient(DrvSclDmaIdType_e enSCLDMA_ID,DrvSclDmaRwModeType_e enRWMode)
{
    DrvSclDmaClientType_e enClientType;
    u8 u8ClientIdx;
    enClientType = _Drv_SCLDMA_TransToClientType(enSCLDMA_ID ,enRWMode);
    if(enClientType == E_DRV_SCLDMA_CLIENT_NUM)
    {
        for(u8ClientIdx=0; u8ClientIdx<E_DRV_SCLDMA_CLIENT_NUM; u8ClientIdx++)
        {
            gu32TrigCount[u8ClientIdx] = 0;
        }
    }
    else
    {
        gu32TrigCount[enClientType] = 0;
    }
}
DrvSclDmaAttrType_t DrvSclDmaGetDmaInformationByClient(DrvSclDmaIdType_e enSCLDMA_ID,DrvSclDmaRwModeType_e enRWMode)
{
    DrvSclDmaClientType_e enClientType;
    DrvSclDmaAttrType_t stAttr;
    u8 u8BufferIdx;
    u8 u8LP;
    u8 u8WP;
    enClientType        = _Drv_SCLDMA_TransToClientType(enSCLDMA_ID ,enRWMode);
    stAttr.u16DMAcount  = HalSclDmaGetDMAOutputCount(enClientType);
    stAttr.u16DMAH      = HalSclDmaGetOutputHsize(enClientType);
    stAttr.u16DMAV      = HalSclDmaGetOutputVsize(enClientType);
    stAttr.u32Trigcount = gu32TrigCount[enClientType];
    stAttr.enBuffMode   = gstScldmaInfo.enBuffMode[enClientType];
    stAttr.enColor      = gstScldmaInfo.enColor[enClientType];
    stAttr.u8MaxIdx     = gstScldmaInfo.bMaxid[enClientType];
    for(u8BufferIdx=0;u8BufferIdx<=stAttr.u8MaxIdx;u8BufferIdx++)
    {
        stAttr.u32Base_Y[u8BufferIdx] = gstScldmaInfo.u32Base_Y[enClientType][u8BufferIdx];
        stAttr.u32Base_C[u8BufferIdx] = gstScldmaInfo.u32Base_C[enClientType][u8BufferIdx];
        stAttr.u32Base_V[u8BufferIdx] = gstScldmaInfo.u32Base_V[enClientType][u8BufferIdx];
    }
    stAttr.bDMAEn = gstScldmaInfo.bDMAOnOff[enClientType];
    if(stAttr.enBuffMode == E_DRV_SCLDMA_BUF_MD_RING)
    {
        stAttr.bDMAReadIdx = (gstScldmaInfo.bDMAidx[enClientType]>>4)&E_DRV_SCLDMA_ACTIVE_BUFFER_SCL;
        stAttr.bDMAWriteIdx = gstScldmaInfo.bDMAidx[enClientType]&E_DRV_SCLDMA_ACTIVE_BUFFER_SCL;
        if(stAttr.bDMAReadIdx>stAttr.bDMAWriteIdx)
        {
            stAttr.u8Count = (stAttr.u8MaxIdx+1)-(stAttr.bDMAReadIdx-stAttr.bDMAWriteIdx);
        }
        else
        {
            stAttr.u8Count = (stAttr.u8MaxIdx+1)-(stAttr.bDMAWriteIdx-stAttr.bDMAReadIdx);
        }
    }
    else
    {
        stAttr.bDMAReadIdx = gstScldmaBufferQueue[enClientType].u8AccessId;
        stAttr.bDMAWriteIdx = gstScldmaBufferQueue[enClientType].u8NextActiveId;
        stAttr.u8Count = gstScldmaBufferQueue[enClientType].u8InQueueCount;
    }
    stAttr.bDMAFlag = gstScldmaInfo.bDmaflag[enClientType];
    stAttr.bSendPoll = gbSendPoll[enClientType];
	u8LP = (_GetIdxType(enClientType,E_DRV_SCLDMA_ACTIVE_BUFFER_SCL));
    if( _IsFlagType(enClientType,E_DRV_SCLDMA_FLAG_ACTIVE))
    {
        u8WP =(u8LP==0) ? gstScldmaInfo.bMaxid[enClientType] : u8LP-1;
    }
    else if(_Is_DMAClientOn(enClientType) &&_IsFlagType(enClientType,E_DRV_SCLDMA_FLAG_BLANKING))
    {
        u8WP = MAX_BUFFER_COUNT;
    }
    else
    {
        u8WP = u8LP;
    }
    stAttr.u32FrameDoneTime = (u32)gu64FRMDoneTime[enClientType][u8WP];
    stAttr.u32SendTime = gu32SendTime[enClientType];
    stAttr.u8ResetCount = gu8ResetCount[enClientType];
    stAttr.u8DMAErrCount = gu8DMAErrCount[enClientType];
    return stAttr;
}
void DrvSclDmaClkClose(DrvSclDmaClkConfig_t* stclk)
{
    if(!gbclkforcemode)
    {
        HalSclDmaCLKInit(0,stclk);
        HalSclDmaSC3CLKInit(0,stclk);
        HalSclDmaODCLKInit(0,stclk);
    }
}
u32 DrvSclDmaGetWaitEvent(DrvSclDmaIdType_e enID)
{
    if(enID == E_DRV_SCLDMA_ID_3_W)
    {
        return (E_DRV_SCLIRQ_SC3EVENT_POLL);
    }
    else if(enID == E_DRV_SCLDMA_ID_2_W)
    {
        return (E_DRV_SCLIRQ_EVENT_SC2POLL);
    }
    else if(enID == E_DRV_SCLDMA_ID_1_W)
    {
        return (E_DRV_SCLIRQ_EVENT_SC1POLL);
    }
    else
    {
        return (E_DRV_SCLIRQ_EVENT_ALLPOLL);
    }
}
void * DrvSclDmaGetWaitQueueHead(DrvSclDmaIdType_e enID)
{
    return DrvSclIrqGetWaitQueueHead(enID);
}

#undef DRV_SCLDMA_C
