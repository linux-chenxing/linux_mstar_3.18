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
#define DRV_SCL_IRQ_C

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "drv_scl_os.h"
#include "drv_scl_dbg.h"

#include "hal_scl_util.h"
#include "hal_scl_reg.h"
#include "drv_scl_irq_st.h"
#include "drv_scl_irq.h"
#include "drv_scl_dbg.h"
#include "drv_scl_dma_st.h" //add later
#include "hal_scl_dma.h" //add later
#include "drv_scl_dma.h" //add later

#include "drv_scl_cmdq.h" //add later
#include "drv_scl_hvsp_st.h"
#include "drv_scl_hvsp.h"
#include "hal_scl_irq.h"
#include "drv_scl_vip_m_st.h"
#include "drv_scl_vip_m.h"

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define DRV_SCLIRQ_DBG(x)
#define DRV_SCLIRQ_DBG_H(x)
#define DRV_SCLIRQ_ERR(x)      x
#define TASKLET_ALREADY 0x8000000000000000
#define IN_TASKLET_ALREADY(u64flag) (((u64)u64flag & TASKLET_ALREADY)!= 0)
#define freerunID               5
#define ENABLE_ISR              0
#define ENABLE_ACTIVEID_ISR     1
#define ENABLE_CMDQ_ISR         1
#define CRITICAL_SECTION_TIME 80
#define CHANGE_CRITICAL_SECTION_TIMING_RANGE 3000 // (ns)
#define CHANGE_CRITICAL_SECTION_TIMING_COUNT 60 // (ns)
#define CRITICAL_SECTION_TIMING_MIN 500 // (ns)
#define _Is_SCLDMA_RingMode(enClientType)            (gstScldmaInfo.enBuffMode[(enClientType)] == E_DRV_SCLDMA_BUF_MD_RING)
#define _Is_SCLDMA_SingleMode(enClientType)         (gstScldmaInfo.enBuffMode[(enClientType)] == E_DRV_SCLDMA_BUF_MD_SINGLE)
#define _Is_SCLDMA_SWRingMode(enClientType)         (gstScldmaInfo.enBuffMode[(enClientType)] == E_DRV_SCLDMA_BUF_MD_SWRING)
#define _IsFrmIN_Vsync()                                  (gbFRMInFlag & (E_DRV_SCLIRQ_FRM_IN_COUNT_NOW ))
#define _IsFrmIN()                                  (gbFRMInFlag & (E_DRV_SCLIRQ_FRM_IN_COUNT_NOW << SCL_DELAYFRAME))
#define _IsFrmIN_Last()                                   (gbFRMInFlag & (E_DRV_SCLIRQ_FRM_IN_COUNT_NOW<< (SCL_DELAYFRAME+1) ))
#define _IsFrmDrop()                                  (gstDropFrameCount)
#define DRV_SCLIRQ_MUTEX_LOCK_ISR()        DrvSclOsObtainMutexIrq(*_pSCLIRQ_SCLDMA_Mutex)
#define DRV_SCLIRQ_MUTEX_UNLOCK_ISR()        DrvSclOsReleaseMutexIrq(*_pSCLIRQ_SCLDMA_Mutex)
#define DRV_SCLIRQ_MUTEX_LOCK()             DrvSclOsObtainMutex(_SCLIRQ_Mutex , SCLOS_WAIT_FOREVER)
#define DRV_SCLIRQ_MUTEX_UNLOCK()             DrvSclOsReleaseMutex(_SCLIRQ_Mutex)
//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------
#if (ENABLE_ISR)
typedef struct
{
    u64  u64IRQ;
    u64  u64Flag;
    u8   u8Count[E_DRV_SCLDMA_CLIENT_NUM];
}ST_SCLIRQ_IFNO_TYPE;
#endif

//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
/////////////////
/// _SCLIRQ_Mutex
/// use in sclirq mutex,not include isr
////////////////
s32 _SCLIRQ_Mutex = -1;

/////////////////
/// _pSCLIRQ_SCLDMA_Mutex
/// scldma and sclirq mutex, include isr, send form scldma
////////////////
s32* _pSCLIRQ_SCLDMA_Mutex = NULL;

/////////////////
/// gsclirqstate
/// record irq state and set event as frame done
////////////////
DrvSclIrqTxEventType_e gsclirqstate;
u32 gu32Time;
u32 gu32FrmEndTime;

/////////////////
/// gscl3irqstate
/// record irq state and set event as frame done
////////////////
DrvSclIrqSc3EventType_e gscl3irqstate;

/////////////////
/// _bSCLIRQ_Suspend
/// record Suspend
////////////////
bool _bSCLIRQ_Suspend = 0;

/////////////////
/// gu32IRQNUM
/// record IRQ number to exit
////////////////
u32 gu32IRQNUM;
u32 gu32CMDQIRQNUM;
/////////////////
/// _s32FRMENDEventId
/// the frame done wait queue ID
////////////////
s32  _s32FRMENDEventId;

/////////////////
/// _s32SYNCEventId
/// the fclk sync wait queue ID
////////////////
s32  _s32SYNCEventId;

/////////////////
/// _s32SC3EventId
/// the Sc3 wait queue ID
////////////////
s32  _s32SC3EventId;
s32  _s32IRQEventId[E_DRV_SCLIRQ_SCIRQ_MAX];

/////////////////
/// gbFRMInFlag
/// use in Drvhvsp and Drvsclirq, and pass to Drvscldma
/// if pre-crop (ISP) is receive full count ,ISR will set this TRUE;
////////////////
bool gbFRMInFlag;

/////////////////
/// gbPtgenMode
/// use in Drvscldma,Drvhvsp and Drvsclirq
/// if True ,is debug ptgen mode. interrupt FRM_IN is inefficacy,so need to ignore.
////////////////
u8 gbPtgenMode;
u8 gbDMADoneEarlyISP;

/////////////////
/// gstThreadCfg
/// IST thread
////////////////
DrvSclDmaThreadConfig_t gstThreadCfg;
DrvSclIrqDazaEventType_e genDAZAEvent;

/////////////////
/// gbMonitorCropMode
/// it use by device attribute to judge checking type
////////////////
u8 gbMonitorCropMode;
/////////////////
/// gbEachDMAEnable
/// record  frame status
////////////////
static bool gbEachDMAEnable;
bool gbBypassDNR;
volatile bool gbInBlanking;
DrvSclIrqScIntsType_t gstSCInts;
u8 gstDropFrameCount;
bool gbsc2frame2reopen = 0;
#if (ENABLE_ISR)
ST_SCLIRQ_IFNO_TYPE gstSCLIrqInfo;
#endif
//-------------------------------------------------------------------------------------------------
//  Functions
//-------------------------------------------------------------------------------------------------
#if (ENABLE_CMDQ_ISR)
void _DrvSclIrqCmdqStateTrig(void)
{
    DrvSclOsSetEvent(_s32SYNCEventId, E_DRV_SCLIRQ_EVENT_CMDQGOING);
}
void _DrvSclIrqCmdqStateDone(void)
{
    u32 u32Events;
    bool bLDC = 0;
    u32Events = DrvSclOsGetEvent(_s32SYNCEventId);
    if(u32Events & E_DRV_SCLIRQ_EVENT_BRESETDNR)
    {
        SCL_DBGERR("[DRVSCLIRQ]%s:!!!E_SCLIRQ_DAZA_BRESETDNR\n",__FUNCTION__);
        HalSclIrqSetReg(REG_SCL0_03_L, 0, BIT12); //LDC path close
        HalSclIrqSetDNRBypass(1);
        gbBypassDNR = 1;
        DrvSclOsSetSclFrameDelay(0);
        DrvSclOsClearEvent(_s32SYNCEventId,(E_DRV_SCLIRQ_EVENT_BRESETDNR));
    }
    if(u32Events & E_DRV_SCLIRQ_EVENT_SETDELAY)
    {
        bLDC = ((HalSclIrqGetRegVal(REG_SCL0_03_L)&BIT12)? 1 : 0);
        SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_ISR,
            "[DRVSCLIRQ]%s:!!!E_SCLIRQ_EVENT_SETDELAY %hhd\n",__FUNCTION__,bLDC);
        DrvSclOsSetSclFrameDelay(bLDC);
        DrvSclOsClearEvent(_s32SYNCEventId,(E_DRV_SCLIRQ_EVENT_SETDELAY));
    }
    DrvSclOsSetEvent(_s32SYNCEventId, E_DRV_SCLIRQ_EVENT_CMDQDONE);
    DrvSclCmdqSetLPoint(E_DRV_SCLCMDQ_TYPE_IP0);
    DrvSclOsQueueWork(1,gstThreadCfg.s32Taskid[E_DRV_SCLIRQ_SCTASKID_DAZA],
        gstThreadCfg.s32HandlerId[E_DRV_SCLIRQ_SCTASK_WORKQUEUECMDQ],0);
}
void  _DrvSclIrqCmdqIsr(void)
{
    u32 u32IrqFlag;
    u32 u32Time;

    u32IrqFlag = (u32)DrvSclCmdqGetFinalIrq(E_DRV_SCLCMDQ_TYPE_IP0);
    DrvSclCmdqClearIrqByFlag(E_DRV_SCLCMDQ_TYPE_IP0,u32IrqFlag);
    u32Time = ((u32)DrvSclOsGetSystemTimeStamp());
    if(u32IrqFlag & CMDQ_IRQ_STATE_TRIG)
    {
        _DrvSclIrqCmdqStateTrig();
    }
    if(u32IrqFlag & CMDQ_IRQ_STATE_DONE)
    {
        _DrvSclIrqCmdqStateDone();
    }
    SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_ISR, "[CMDQ]u32IrqFlag:%lx  %lu\n",u32IrqFlag,u32Time);
}
#endif
void _DrvSclIrqSclFrameEndConnectToIsp(void)
{
    //scl_ve_isr(); // RTK_ToDo wait for drv_isp.c
}
void _DrvSclIrqSclFrameStartConnectToIsp(void)
{
   // scl_vs_isr(); // RTK_ToDo wait for drv_isp.c
}
#if ENABLE_ISR
void  _DrvSclIrqisr(int eIntNum, void* dev_id)
{
    u64 u64Flag = HalSclIrqGetFlag(E_DRV_SCLIRQ_SCTOP_0,0xFFFFFFFFFF);

    HalSclIrqSetClear(E_DRV_SCLIRQ_SCTOP_0,u64Flag, 1);

    HalSclIrqSetClear(E_DRV_SCLIRQ_SCTOP_0,u64Flag, 0);

    //return IRQ_HANDLED;
}

#elif ENABLE_ACTIVEID_ISR
DrvSclIrqTxEventType_e _DrvSclIrqGetISTEventFlag(DrvSclDmaClientType_e enClient)
{
    switch(enClient)
    {
        case E_DRV_SCLDMA_1_FRM_W:
            return E_DRV_SCLIRQ_EVENT_ISTSC1FRM;
        case E_DRV_SCLDMA_1_SNP_W:
            return E_DRV_SCLIRQ_EVENT_ISTSC1SNP;
        case E_DRV_SCLDMA_2_FRM_W:
            return E_DRV_SCLIRQ_EVENT_ISTSC2FRM;
        case E_DRV_SCLDMA_2_FRM2_W:
            return E_DRV_SCLIRQ_EVENT_ISTSC2FRM2;
        default :
            return 0;
    }
}
void _DrvSclIrqSetIsBlankingRegion(bool bBk)
{
    gbInBlanking = bBk;
}
bool DrvSclIrqGetIsVipBlankingRegion(void)
{
    if(DrvSclOsGetEvent(_s32SYNCEventId) & E_DRV_SCLIRQ_EVENT_CMDQDONE)
    {
        return gbInBlanking;
    }
    else if(gbInBlanking && (DrvSclOsGetEvent(_s32SYNCEventId) & E_DRV_SCLIRQ_EVENT_CMDQGOING))
    {
        return 0;
    }
    else
    {
        return gbInBlanking;
    }
}
bool DrvSclIrqGetIsBlankingRegion(void)
{
    bool bRet = 0;
    u32 u32Time;
    u32Time = ((u32)DrvSclOsGetSystemTimeStamp());
    //critical section
    if(gbInBlanking && gu32FrmEndTime && gstSCInts.u32ISPDoneCount&&
        ((u32Time - gu32FrmEndTime)>((gstSCInts.u32ISPBlankingTime)-CRITICAL_SECTION_TIME)))
    {
        bRet = 0;
        if((u32Time - gu32FrmEndTime)>((gstSCInts.u32ISPBlankingTime)*2))
        {
            // error
            bRet = gbInBlanking;
        }
        else
        {
            SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()==(EN_DBGMG_SCLIRQLEVEL_ELSE),
                "[SCLIRQ]Blanking Critical Section (%ld ,%ld)\n",
                (u32Time - gu32FrmEndTime),((gstSCInts.u32ISPBlankingTime)-CRITICAL_SECTION_TIME));
        }
    }
    else
    {
        bRet = gbInBlanking;
    }
    return bRet;
}
void _DrvSclIrqSetSingleModeDmaInfo(DrvSclDmaClientType_e enclient, u32 u32Events)
{
    if ((u32Events & (SCLIRQ_ISTEVENT_BASE << enclient)) && _Is_SCLDMA_SingleMode(enclient))
    {
        DRV_SCLIRQ_MUTEX_LOCK_ISR();
        gstScldmaInfo.bDMAOnOff[enclient] = 0;
        DrvSclDmaSetFrameResolution(enclient);
        DRV_SCLIRQ_MUTEX_UNLOCK_ISR();
    }
}
void _DrvSclIrqSetSingleModeDmaInfoNoLock(DrvSclDmaClientType_e enclient, u32 u32Events)
{
    if ((u32Events & (SCLIRQ_ISTEVENT_BASE << enclient)) && _Is_SCLDMA_SingleMode(enclient))
    {
        gstScldmaInfo.bDMAOnOff[enclient] = 0;
        DrvSclDmaSetFrameResolution(enclient);
    }
}
static void _DrvSclIrqRingModeActive(DrvSclDmaClientType_e enClient)
{
    u8 u8RPoint = 0;
    u32 u32Time;
    u16  u16RealIdx;
    u32Time = ((u32)DrvSclOsGetSystemTimeStamp());
    _SetANDGetFlagType(enClient,E_DRV_SCLDMA_FLAG_ACTIVE,(~E_DRV_SCLDMA_FLAG_FRMDONE));
    u8RPoint = (_GetIdxType(enClient,E_DRV_SCLDMA_ACTIVE_BUFFER_OMX))>>4;
    if(!_IsFlagType(enClient,E_DRV_SCLDMA_FLAG_DMAOFF))
    {
        u16RealIdx = HalSclDmaGetRWIdx(enClient);
    }
    else
    {
        u16RealIdx = _GetIdxType(enClient,E_DRV_SCLDMA_ACTIVE_BUFFER_SCL);
    }
    DrvSclDmaSetFrameResolution(enClient);
    if(u8RPoint==u16RealIdx && !_IsFlagType(enClient,E_DRV_SCLDMA_FLAG_BLANKING))//5 is debug freerun
    {
        //trig off
        if(u8RPoint != freerunID )
        {
            DrvSclDmaSetISRHandlerDmaOff(enClient,0);
            _SetFlagType(enClient,E_DRV_SCLDMA_FLAG_EVERDMAON);
            _SetANDGetIdxType(enClient,(u16RealIdx),(E_DRV_SCLDMA_ACTIVE_BUFFER_OMX));
            _SetFlagType(enClient,(E_DRV_SCLDMA_FLAG_DMAOFF|E_DRV_SCLDMA_FLAG_ACTIVE));
        }
        else
        {
            _SetANDGetIdxType(enClient,(u16RealIdx),(E_DRV_SCLDMA_ACTIVE_BUFFER_OMX));
            _SetANDGetFlagType(enClient,E_DRV_SCLDMA_FLAG_ACTIVE,(~E_DRV_SCLDMA_FLAG_DMAOFF));
        }
    }
    else
    {
        _SetANDGetIdxType(enClient,u16RealIdx,E_DRV_SCLDMA_ACTIVE_BUFFER_OMX);
        _SetFlagType(enClient,E_DRV_SCLDMA_FLAG_ACTIVE);
    }
    SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&Get_DBGMG_SCLIRQclient(enClient,0),
        "[DRVSCLDMA]%d ISR R_P=%hhx RealIdx=%hx flag:%hx @:%lu\n"
    ,enClient,u8RPoint,u16RealIdx,gstScldmaInfo.bDmaflag[enClient],u32Time);
}
static void _DrvSclIrqRingModeActive_N(DrvSclDmaClientType_e enclient)
{
    _SetANDGetFlagType(enclient,E_DRV_SCLDMA_FLAG_FRMDONE,(~E_DRV_SCLDMA_FLAG_ACTIVE));
    _ReSetFlagType(enclient,E_DRV_SCLDMA_FLAG_ACTIVE);
    if(_IsFlagType(enclient,E_DRV_SCLDMA_FLAG_DMAOFF))
    {
        gstScldmaInfo.bDMAOnOff[enclient] = 0;
    }
    if(_IsFlagType(enclient,E_DRV_SCLDMA_FLAG_BLANKING))
    {
        _ReSetFlagType(enclient,E_DRV_SCLDMA_FLAG_BLANKING);
    }
    SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&Get_DBGMG_SCLIRQclient(enclient,1), "[DRVSCLDMA]%d ISR actN flag:%hx idx:%hhd @:%lu\n"
    ,enclient,gstScldmaInfo.bDmaflag[enclient],gstScldmaInfo.bDMAidx[enclient],((u32)DrvSclOsGetSystemTimeStamp()));
    //sclprintf("ACTIVE_N:%lu\n",u32Time);
}
static void _DrvSclIrqRingModeActiveWithoutDoublebuffer(DrvSclDmaClientType_e enClient)
{
    u8 u8RPoint = 0;
    u32 u32Time;
    u16  u16RealIdx;
    u32Time = ((u32)DrvSclOsGetSystemTimeStamp());
    //u8RPoint = (_GetIdxType(enClient,E_DRV_SCLDMA_ACTIVE_BUFFER_OMX))>>4;
    if(_IsFlagType(enClient,E_DRV_SCLDMA_FLAG_ACTIVE))
    {
        _SetFlagType(enClient,E_DRV_SCLDMA_FLAG_FRMIGNORE);
        SCL_DBGERR("[DRVSCLIRQ]%d!!!!!!!!!!!!double active\n",enClient);
    }
    else
    {
        _SetANDGetFlagType(enClient,E_DRV_SCLDMA_FLAG_ACTIVE,(~E_DRV_SCLDMA_FLAG_FRMDONE));
    }
    u8RPoint = (_GetIdxType(enClient,E_DRV_SCLDMA_ACTIVE_BUFFER_OMX))>>4;
    if(!_IsFlagType(enClient,E_DRV_SCLDMA_FLAG_DMAOFF))
    {
        u16RealIdx = HalSclDmaGetRWIdx(enClient);
        if(u16RealIdx == 0 && _GetIdxType(enClient,E_DRV_SCLDMA_ACTIVE_BUFFER_SCL) != gstScldmaInfo.bMaxid[enClient]&&
            !(_IsFlagType(enClient,E_DRV_SCLDMA_FLAG_BLANKING)))
        {
            _SetFlagType(enClient,E_DRV_SCLDMA_FLAG_FRMIGNORE);
            SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&Get_DBGMG_SCLIRQclient(enClient,0),
                "[DRVSCLDMA]%d wodb ISR ignore\n"
            ,enClient);
        }
    }
    else
    {
        u16RealIdx = _GetIdxType(enClient,E_DRV_SCLDMA_ACTIVE_BUFFER_SCL);
    }
    DrvSclDmaSetFrameResolution(enClient);
    _SetANDGetIdxType(enClient,u16RealIdx,E_DRV_SCLDMA_ACTIVE_BUFFER_OMX);
    SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&Get_DBGMG_SCLIRQclient(enClient,0),
        "[DRVSCLDMA]%d wodb ISR R_P=%hhx RealIdx=%hx flag:%hx @:%lu\n"
    ,enClient,u8RPoint,u16RealIdx,gstScldmaInfo.bDmaflag[enClient],u32Time);
}
static bool _DrvSclIrqRingModeActive_NWithoutDoublebuffer(DrvSclDmaClientType_e enClient)
{
    u8 u8RPoint = 0;
    u16  u16RealIdx;
    u8RPoint = (_GetIdxType(enClient,E_DRV_SCLDMA_ACTIVE_BUFFER_OMX))>>4;
    u16RealIdx = _GetIdxType(enClient,E_DRV_SCLDMA_ACTIVE_BUFFER_SCL);
    if((!_IsFlagType(enClient,E_DRV_SCLDMA_FLAG_FRMIN)))
    {
        _SetFlagType(enClient,E_DRV_SCLDMA_FLAG_FRMIGNORE);
    }
    if(!_IsFlagType(enClient,E_DRV_SCLDMA_FLAG_FRMIGNORE))
    {
        if(u8RPoint==u16RealIdx && !_IsFlagType(enClient,E_DRV_SCLDMA_FLAG_BLANKING))//5 is debug freerun
        {
            //trig off
            if(u8RPoint != freerunID )
            {
                DrvSclDmaSetSWReTrigCount(enClient,1);
                DrvSclDmaSetISRHandlerDmaOff(enClient,0);
                _SetANDGetFlagType
                    (enClient,E_DRV_SCLDMA_FLAG_FRMDONE|E_DRV_SCLDMA_FLAG_EVERDMAON|E_DRV_SCLDMA_FLAG_DMAOFF,(~E_DRV_SCLDMA_FLAG_ACTIVE));
                _SetANDGetIdxType(enClient,(u16RealIdx),(E_DRV_SCLDMA_ACTIVE_BUFFER_OMX));
            }
            else
            {
                _SetANDGetIdxType(enClient,(u16RealIdx),(E_DRV_SCLDMA_ACTIVE_BUFFER_OMX));
                _SetANDGetFlagType(enClient,E_DRV_SCLDMA_FLAG_FRMDONE,(~(E_DRV_SCLDMA_FLAG_DMAOFF|E_DRV_SCLDMA_FLAG_ACTIVE)));
            }
        }
        else
        {
            _SetANDGetFlagType(enClient,E_DRV_SCLDMA_FLAG_FRMDONE,(~E_DRV_SCLDMA_FLAG_ACTIVE));
        }
        if(_IsFlagType(enClient,E_DRV_SCLDMA_FLAG_DMAOFF))
        {
            gstScldmaInfo.bDMAOnOff[enClient] = 0;
        }
        if(_IsFlagType(enClient,E_DRV_SCLDMA_FLAG_BLANKING))
        {
            _ReSetFlagType(enClient,E_DRV_SCLDMA_FLAG_BLANKING);
        }
        SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&Get_DBGMG_SCLIRQclient(enClient,1), "[DRVSCLDMA]%d ISR actN wodb flag:%hx idx:%hhx @:%lu\n"
        ,enClient,gstScldmaInfo.bDmaflag[enClient],gstScldmaInfo.bDMAidx[enClient],((u32)DrvSclOsGetSystemTimeStamp()));
        return 1;
    }
    else
    {
        _ReSetFlagType(enClient,E_DRV_SCLDMA_FLAG_FRMIGNORE);
        if(u8RPoint != freerunID )
        {
            DrvSclDmaSetDMAIgnoreCount(enClient,1);
            DrvSclDmaSetISRHandlerDmaOff(enClient,0);
            SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&Get_DBGMG_SCLIRQclient(enClient,1),
                "[DRVSCLDMA]%d ISR actN wodb ignore flag:%hx\n"
            ,enClient,gstScldmaInfo.bDmaflag[(enClient)]);
            _SetANDGetFlagType
                (enClient,E_DRV_SCLDMA_FLAG_FRMDONE|E_DRV_SCLDMA_FLAG_EVERDMAON|E_DRV_SCLDMA_FLAG_DMAOFF,(~(E_DRV_SCLDMA_FLAG_ACTIVE)));
        }
        if(_IsFlagType(enClient,E_DRV_SCLDMA_FLAG_DMAOFF))
        {
            gstScldmaInfo.bDMAOnOff[enClient] = 0;
        }
        _ReSetFlagType(enClient,E_DRV_SCLDMA_FLAG_FRMIN);
        DrvSclOsSetEvent(_s32FRMENDEventId, (0x1 <<enClient));
        SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&Get_DBGMG_SCLIRQclient(enClient,1),
            "[DRVSCLDMA]%d ISR actN wodb ignore DMA off:%hhx flag:%hx@:%lu\n"
        ,enClient,gstScldmaInfo.bDMAOnOff[enClient],gstScldmaInfo.bDmaflag[(enClient)],((u32)DrvSclOsGetSystemTimeStamp()));
        return 0;
    }
    //sclprintf("ACTIVE_N:%lu\n",u32Time);
}
static void _DrvSclIrqSWRingModeActive(DrvSclDmaClientType_e enclient)
{
    u32 u32Time;
    u16  u16RealIdx;
    u32Time = ((u32)DrvSclOsGetSystemTimeStamp());
    if(_IsFlagType(enclient,E_DRV_SCLDMA_FLAG_BLANKING))
    {
        u16RealIdx = 0;
    }
    else
    {
        u16RealIdx = DrvSclDmaGetActiveBufferIdx(enclient);
    }
    if(_IsFlagType(enclient,E_DRV_SCLDMA_FLAG_ACTIVE))
    {
        _SetFlagType(enclient,E_DRV_SCLDMA_FLAG_FRMIGNORE);
        SCL_DBGERR("[DRVSCLIRQ]!!!!!!!!!!!!double active\n");
    }
    else
    {
        _SetANDGetFlagType(enclient,E_DRV_SCLDMA_FLAG_ACTIVE,(~(E_DRV_SCLDMA_FLAG_FRMDONE|E_DRV_SCLDMA_FLAG_FRMIGNORE)));
    }
    _SetANDGetIdxType(enclient,u16RealIdx,E_DRV_SCLDMA_ACTIVE_BUFFER_OMX);
    SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&Get_DBGMG_SCLIRQclient(enclient,0), "[DRVSCLDMA]%d ISR SWRING flag:%hx ACT:%hhd@:%lu\n"
    ,enclient,gstScldmaInfo.bDmaflag[enclient],u16RealIdx,u32Time);
}
static bool _DrvSclIrqSWRingModeActive_N(DrvSclDmaClientType_e enclient)
{
    DrvSclDmaFrameBufferConfig_t stTarget;
    u64 u64Time;
    bool Ret = 1;
    u64Time = ((u64)DrvSclOsGetSystemTimeStamp());
    if(!_IsFlagType(enclient,E_DRV_SCLDMA_FLAG_FRMIN)|| _IsFlagType(enclient,E_DRV_SCLDMA_FLAG_DROP)
        || _IsFlagType(enclient,E_DRV_SCLDMA_FLAG_FRMIGNORE))
    {
        Ret = 0;
        _ReSetFlagType(enclient,E_DRV_SCLDMA_FLAG_ACTIVE);
        if( _IsFlagType(enclient,E_DRV_SCLDMA_FLAG_FRMIGNORE))
        {
            DrvSclDmaSetDMAIgnoreCount(enclient,1);
            DrvSclDmaSetISRHandlerDmaOff(enclient,0);
            _SetFlagType(enclient,E_DRV_SCLDMA_FLAG_EVERDMAON);
        }
        else
        {
            _SetFlagType(enclient,E_DRV_SCLDMA_FLAG_FRMIGNORE);
        }
        SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&Get_DBGMG_SCLIRQclient(enclient,1),
            "[DRVSCLDMA]%d ISR SWRING ignore flag:%hx idx:%hhd\n"
        ,enclient,gstScldmaInfo.bDmaflag[enclient],gstScldmaInfo.bDMAidx[enclient]);
    }
    else
    {
        _SetANDGetFlagType(enclient,E_DRV_SCLDMA_FLAG_FRMDONE,(~E_DRV_SCLDMA_FLAG_ACTIVE));
        _ReSetFlagType(enclient,E_DRV_SCLDMA_FLAG_ACTIVE);
        stTarget.u8FrameAddrIdx = _GetIdxType(enclient,E_DRV_SCLDMA_ACTIVE_BUFFER_SCL);
        stTarget.u32FrameAddr   = gstScldmaInfo.u32Base_Y[enclient][stTarget.u8FrameAddrIdx];
        stTarget.u64FRMDoneTime = u64Time;
        stTarget.u16FrameWidth   = gstScldmaInfo.u16FrameWidth[enclient];
        stTarget.u16FrameHeight  = gstScldmaInfo.u16FrameHeight[enclient];
        if(DrvSclDmaMakeSureNextActiveId(enclient))
        {
            if(DrvSclDmaBufferEnQueue(enclient,stTarget))
            {
                DrvSclDmaChangeBufferIdx(enclient);
            }
        }
        if(_IsFlagType(enclient,E_DRV_SCLDMA_FLAG_BLANKING))
        {
            _ReSetFlagType(enclient,E_DRV_SCLDMA_FLAG_BLANKING);
        }
        SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&Get_DBGMG_SCLIRQclient(enclient,1), "[DRVSCLDMA]%d ISR SWRING actN flag:%hx idx:%hhd\n"
        ,enclient,gstScldmaInfo.bDmaflag[enclient],gstScldmaInfo.bDMAidx[enclient]);
    }
    return Ret;
}
static void _DrvSclIrqSingleModeActive(DrvSclDmaClientType_e enclient)
{
    u32 u32Time;
    _SetANDGetFlagType(enclient,E_DRV_SCLDMA_FLAG_ACTIVE,(~E_DRV_SCLDMA_FLAG_FRMDONE));
    if(enclient< E_DRV_SCLDMA_3_FRM_R)
    {
        DrvSclDmaSetISRHandlerDmaOff(enclient,0);
        u32Time = ((u32)DrvSclOsGetSystemTimeStamp());
        SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&Get_DBGMG_SCLIRQclient(enclient,1), "[DRVSCLIRQ]single act:%lu\n",u32Time);
    }
}
static void _DrvSclIrqSingleModeActive_N(DrvSclDmaClientType_e enclient)
{
    u32 u32Time;
    _SetANDGetFlagType(enclient,E_DRV_SCLDMA_FLAG_FRMDONE,(~E_DRV_SCLDMA_FLAG_ACTIVE));
    if(enclient< E_DRV_SCLDMA_3_FRM_R)
    {
        u32Time = ((u32)DrvSclOsGetSystemTimeStamp());
        _DrvSclIrqSetSingleModeDmaInfoNoLock(enclient,(SCLIRQ_ISTEVENT_BASE << enclient));
        SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&Get_DBGMG_SCLIRQclient(enclient,1), "[DRVSCLIRQ]single actN:%lu\n",u32Time);
    }
}
static void _DrvSclIrqSingleModeActiveWithoutDoublebuffer(DrvSclDmaClientType_e enclient)
{
    u32 u32Time;
    if(_IsFlagType(enclient,E_DRV_SCLDMA_FLAG_ACTIVE))
    {
        _SetFlagType(enclient,E_DRV_SCLDMA_FLAG_FRMIGNORE);
        SCL_DBGERR("[DRVSCLIRQ]!!!!!!! double active\n");
    }
    else
    {
        _SetANDGetFlagType(enclient,E_DRV_SCLDMA_FLAG_ACTIVE,(~(E_DRV_SCLDMA_FLAG_FRMDONE|E_DRV_SCLDMA_FLAG_FRMIGNORE)));
    }
    u32Time = ((u32)DrvSclOsGetSystemTimeStamp());
    SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&EN_DBGMG_SCLIRQLEVEL_SC1SINGLE, "[DRVSCLIRQ]singlewodb act:%lu\n",u32Time);
}
static bool _DrvSclIrqSingleModeActive_NWithoutDoublebuffer(DrvSclDmaClientType_e enclient)
{
    u32 u32Time;
    bool Ret = 1;
    u32Time = ((u32)DrvSclOsGetSystemTimeStamp());
    _SetANDGetFlagType(enclient,E_DRV_SCLDMA_FLAG_FRMDONE,(~E_DRV_SCLDMA_FLAG_ACTIVE));
    if(enclient< E_DRV_SCLDMA_3_FRM_R)
    {
        if(!_IsFlagType(enclient,E_DRV_SCLDMA_FLAG_FRMIN) || _IsFlagType(enclient,E_DRV_SCLDMA_FLAG_DROP)
            || _IsFlagType(enclient,E_DRV_SCLDMA_FLAG_FRMIGNORE))
        {
            Ret = 0;
            if( _IsFlagType(enclient,E_DRV_SCLDMA_FLAG_FRMIGNORE))
            {
                DrvSclDmaSetDMAIgnoreCount(enclient,1);
                DrvSclDmaSetISRHandlerDmaOff(enclient,0);
                _SetFlagType(enclient,E_DRV_SCLDMA_FLAG_EVERDMAON);
            }
            else
            {
                _SetFlagType(enclient,E_DRV_SCLDMA_FLAG_FRMIGNORE);
            }
            SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&EN_DBGMG_SCLIRQLEVEL_SC1SINGLE, "[DRVSCLIRQ]singlewodb ignore actN:%lu\n",u32Time);
        }
        else
        {
            _SetFlagType(enclient,E_DRV_SCLDMA_FLAG_DMAOFF);
            //Drv_SCLDMA_SetISRHandlerDMAOff(enclient,0);
            _DrvSclIrqSetSingleModeDmaInfoNoLock(enclient,(SCLIRQ_ISTEVENT_BASE << enclient));
            SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&EN_DBGMG_SCLIRQLEVEL_SC1SINGLE, "[DRVSCLIRQ]singlewodb actN:%lu\n",u32Time);
        }
    }
    return Ret;
}
static void _DrvSclIrqSC3SingleMode_Active(void)
{
    u32 u32Time;
    u32Time = ((u32)DrvSclOsGetSystemTimeStamp());
    SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&EN_DBGMG_SCLIRQLEVEL_SC3SINGLE, "[DRVSCLIRQ]SC3 single act @:%lu\n",u32Time);
    _SetANDGetFlagType(E_DRV_SCLDMA_3_FRM_W,E_DRV_SCLDMA_FLAG_ACTIVE,(~E_DRV_SCLDMA_FLAG_FRMDONE));
    _SetANDGetFlagType(E_DRV_SCLDMA_3_FRM_R,E_DRV_SCLDMA_FLAG_ACTIVE,(~E_DRV_SCLDMA_FLAG_FRMDONE));
}
static void _DrvSclIrqSC3SingleMode_Active_N(void)
{
    u32 u32Time;
    u32 u32Events = 0;
    u32Time = ((u32)DrvSclOsGetSystemTimeStamp());
    DrvSclDmaSetISRHandlerDmaOff(E_DRV_SCLDMA_3_FRM_W,0);
    u32Events = DrvSclOsGetEvent(_s32SC3EventId);//get now ir
    _SetANDGetFlagType(E_DRV_SCLDMA_3_FRM_W,(E_DRV_SCLDMA_FLAG_DMAOFF|E_DRV_SCLDMA_FLAG_FRMDONE),
        ~(E_DRV_SCLDMA_FLAG_EVERDMAON|E_DRV_SCLDMA_FLAG_ACTIVE));
    _SetANDGetFlagType(E_DRV_SCLDMA_3_FRM_R,(E_DRV_SCLDMA_FLAG_DMAOFF|E_DRV_SCLDMA_FLAG_FRMDONE),
    ~(E_DRV_SCLDMA_FLAG_EVERDMAON|E_DRV_SCLDMA_FLAG_ACTIVE));
    _DrvSclIrqSetSingleModeDmaInfoNoLock(E_DRV_SCLDMA_3_FRM_W,(SCLIRQ_ISTEVENT_BASE << E_DRV_SCLDMA_3_FRM_W));
    _DrvSclIrqSetSingleModeDmaInfoNoLock(E_DRV_SCLDMA_3_FRM_R,(SCLIRQ_ISTEVENT_BASE << E_DRV_SCLDMA_3_FRM_R));
    SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&EN_DBGMG_SCLIRQLEVEL_SC3SINGLE, "[DRVSCLIRQ]SC3 single actN @:%lu\n",u32Time);
    DrvSclOsClearEvent(_s32SC3EventId,u32Events &(E_DRV_SCLIRQ_SC3EVENT_ISTCLEAR));
    DrvSclOsSetEvent(_s32SC3EventId, (E_DRV_SCLIRQ_SC3EVENT_DONE |E_DRV_SCLIRQ_SC3EVENT_HVSPST));
}
static void _DrvSclIrqSetFrameInFlag(DrvSclDmaClientType_e enclient)
{
    if(_IsFrmDrop())
    {
        _SetFlagType(enclient,E_DRV_SCLDMA_FLAG_DROP);
        SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&EN_DBGMG_SCLIRQLEVEL_ELSE,
            "[DRVSCLIRQ]%dIsFrmDrop @:%lu\n",enclient,((u32)DrvSclOsGetSystemTimeStamp()));
    }
    else
    {
        _ReSetFlagType(enclient,E_DRV_SCLDMA_FLAG_DROP);
    }
    if(_IsFrmIN())
    {
        _SetFlagType(enclient,E_DRV_SCLDMA_FLAG_FRMIN);
    }
    else if(!DrvSclOsGetSclFrameDelay())
    {
        u16 u16Crop;
        u16 u16Crop2;
        u16Crop = HalSclIrqGetRegVal(REG_SCL2_50_L);
        u16Crop &= 0x1;
        u16Crop2 = HalSclIrqGetRegVal(REG_SCL2_58_L);
        u16Crop2 &= 0x1;
        if((u16Crop || u16Crop2)&&_IsFrmIN_Last())
        {
            _SetFlagType(enclient,E_DRV_SCLDMA_FLAG_FRMIN);
        }
        else
        {
            _ReSetFlagType(enclient,E_DRV_SCLDMA_FLAG_FRMIN);
        }
    }
    else
    {
        _ReSetFlagType(enclient,E_DRV_SCLDMA_FLAG_FRMIN);
    }
}
void _DrvSclIrqMonitorSystemTimer(void)
{
    u64 u64Time;
    static u64 u64lasttime = 0;
    u64Time = ((u64)DrvSclOsGetSystemTimeStamp());
    if(u64Time > u64lasttime)
    {
        u64lasttime = u64Time;
    }
    else
    {
        sclprintf("[DRVSCLIRQ]!!!!!!!! system time reset last:%lld this:%lld\n",u64lasttime,u64Time);
        u64lasttime = u64Time;
    }
}
void _DrvSclIrqFrmDoneEventHandler(void)
{
    DrvSclIrqTxEventType_e enEvent = gsclirqstate;
    if(enEvent)
    {
        enEvent |= (E_DRV_SCLIRQ_EVENT_IRQ|E_DRV_SCLIRQ_EVENT_HVSPST);
        DrvSclOsSetEvent(_s32FRMENDEventId, enEvent);
        if(gsclirqstate &E_DRV_SCLIRQ_EVENT_SC1FRM && (gbMonitorCropMode == 2))
        {
            DrvSclOsSetEvent(_s32SYNCEventId, E_DRV_SCLIRQ_EVENT_SYNC);
        }
        gsclirqstate &= (~enEvent);
        gsclirqstate |= E_DRV_SCLIRQ_EVENT_SET;
        SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&(EN_DBGMG_SCLIRQLEVEL_ELSE),
            "%s @:%lu\n",__FUNCTION__,gu32Time);
    }
}
void _DrvSclIrqCmdqBlankingHandler(void)
{
    if(DrvSclOsGetEvent(_s32SYNCEventId)&E_DRV_SCLIRQ_EVENT_CMDQFIRE )
    {
        DrvSclCmdqBeTrigger(E_DRV_SCLCMDQ_TYPE_IP0,1);
        DrvSclOsClearEvent(DrvSclIrqGetIrqSYNCEventID(),(E_DRV_SCLIRQ_EVENT_CMDQFIRE));
        SCL_DBG(SCL_DBG_LV_DRVCMDQ()&(EN_DBGMG_CMDQEVEL_ISR),
            "%s @:%lu\n",__FUNCTION__,gu32Time);
    }
}
void _DrvSclIrqSetFrmInWhenDMADoneEarlyIsp(bool bEn)
{
    if(bEn)
    {
        _SetFlagType(E_DRV_SCLDMA_1_FRM_W,E_DRV_SCLDMA_FLAG_FRMIN);
        _SetFlagType(E_DRV_SCLDMA_1_SNP_W,E_DRV_SCLDMA_FLAG_FRMIN);
        _SetFlagType(E_DRV_SCLDMA_2_FRM_W,E_DRV_SCLDMA_FLAG_FRMIN);
        _SetFlagType(E_DRV_SCLDMA_2_FRM2_W,E_DRV_SCLDMA_FLAG_FRMIN);
    }
    else
    {
        _ReSetFlagType(E_DRV_SCLDMA_1_FRM_W,E_DRV_SCLDMA_FLAG_FRMIN);
        _ReSetFlagType(E_DRV_SCLDMA_1_SNP_W,E_DRV_SCLDMA_FLAG_FRMIN);
        _ReSetFlagType(E_DRV_SCLDMA_2_FRM_W,E_DRV_SCLDMA_FLAG_FRMIN);
        _ReSetFlagType(E_DRV_SCLDMA_2_FRM2_W,E_DRV_SCLDMA_FLAG_FRMIN);
    }
}
void _Drv_SCLIRQ_FrmEndhandler(void)
{
    u8 idx;
    u8 loop = 0;
    if(!gbEachDMAEnable)
    {
        gbEachDMAEnable = 1;
    }
    else if(gsclirqstate)
    {
    }
    else
    {
        for(idx = 0;idx<E_DRV_SCLDMA_CLIENT_NUM;idx++)
        {
            if(gstScldmaInfo.bDMAOnOff[idx])
            {
                loop = 1;
                break;
            }
        }
        if(!loop)
        {
            gbEachDMAEnable = 0;
        }
    }
    DrvSclOsSetEvent(_s32SYNCEventId, E_DRV_SCLIRQ_EVENT_FRMENDSYNC);
    DrvSclOsClearEvent(DrvSclIrqGetIrqSYNCEventID(),(E_DRV_SCLIRQ_EVENT_LDCSYNC));
    //TODO:not open this time
    if((!gbDMADoneEarlyISP)||(SCL_DELAYFRAME) || (gbPtgenMode))
    {
        _DrvSclIrqFrmDoneEventHandler();
    }
}
void _DrvSclIrqSetDmaOnOffWithoutDoubleBuffer(DrvSclDmaClientType_e enclient)
{
    if((_IsFlagType(enclient,E_DRV_SCLDMA_FLAG_DMAOFF))&&_Is_SCLDMA_SingleMode(enclient))
    {
        _ReSetFlagType(enclient,E_DRV_SCLDMA_FLAG_DMAOFF);

        DrvSclDmaSetISRHandlerDmaOff(enclient,0);
        gstScldmaInfo.bDMAOnOff[enclient] = 0;
    }
    else if((_IsFlagType(enclient,E_DRV_SCLDMA_FLAG_EVERDMAON))&&
         (_Is_SCLDMA_SingleMode(enclient)||_Is_SCLDMA_SWRingMode(enclient)))
    {
        _ReSetFlagType(enclient,E_DRV_SCLDMA_FLAG_EVERDMAON);
        DrvSclDmaSetISRHandlerDmaOff(enclient,1);
    }
    if ((_IsFlagType(enclient,E_DRV_SCLDMA_FLAG_NEXT_OFF)))
    {
        _ReSetFlagType(enclient,E_DRV_SCLDMA_FLAG_NEXT_OFF);
        DrvSclDmaSetISRHandlerDmaOff(enclient,0);
        gstScldmaInfo.bDMAOnOff[enclient] = 0;
    }
    if ((_IsFlagType(enclient,E_DRV_SCLDMA_FLAG_NEXT_ON))&&(!_IsFlagType(enclient,E_DRV_SCLDMA_FLAG_DMAFORCEOFF)))
    {
        _ReSetFlagType(enclient,E_DRV_SCLDMA_FLAG_NEXT_ON);
        DrvSclDmaSetISRHandlerDmaOff(enclient,1);
        gstScldmaInfo.bDMAOnOff[enclient] = 1;
    }
}
void _DrvSclIrqSetDmaOnOffWithoutDoubleBufferHandler(void)
{
    _DrvSclIrqSetDmaOnOffWithoutDoubleBuffer(E_DRV_SCLDMA_1_FRM_W);
    _DrvSclIrqSetDmaOnOffWithoutDoubleBuffer(E_DRV_SCLDMA_1_SNP_W);
    _DrvSclIrqSetDmaOnOffWithoutDoubleBuffer(E_DRV_SCLDMA_2_FRM_W);
    //for HW bug
    _DrvSclIrqSetDmaOnOffWithoutDoubleBuffer(E_DRV_SCLDMA_2_FRM2_W);
}
void _DrvSclIrqReduceDropFrameCount(void)
{
    if(gstDropFrameCount)
    {
        gstDropFrameCount--;
    }
}
static void _DrvSclIrqSetRealTimeFrmEndHandler(u32 u32Time)
{
    _DrvSclIrqSetIsBlankingRegion(1);
    if(DrvSclOsGetEvent(_s32SYNCEventId)&E_DRV_SCLIRQ_EVENT_ISPFRMEND)
    {
        DrvSclIrqSetDMADoneEarlyISP(0);
    }
    else
    {
        DrvSclIrqSetDMADoneEarlyISP(1);
    }

    if(_IsFlagType(E_DRV_SCLDMA_2_FRM2_W,E_DRV_SCLDMA_FLAG_ACTIVE))
    {
        gbsc2frame2reopen = 1;
    }
    else
    {
        _DrvSclIrqSetDmaOnOffWithoutDoubleBufferHandler();
        if((!gbDMADoneEarlyISP) || (gbPtgenMode))
        {
            _DrvSclIrqCmdqBlankingHandler();
        }
    }
}
static void _DrvSclIrqSetFrmEndInterruptStatus(u32 u32Time)
{
    //_DrvSclIrqMonitorSystemTimer();
    DrvSclDmaSetIspFrameCount();
    _Drv_SCLIRQ_FrmEndhandler();
    _DrvSclIrqReduceDropFrameCount();
    _DrvSclIrqSclFrameEndConnectToIsp();
}

void _DrvSclIrqHandlerFrmIn(bool bEn)
{
    if(!gbPtgenMode)
    {
        gbFRMInFlag = (bool)((gbFRMInFlag)|bEn);
    }
}
void _DrvSclIrqHandlerFrmInShift(void)
{
    if(!gbPtgenMode)
    {
        gbFRMInFlag = (bool)((gbFRMInFlag << 1));
    }
}
void _DrvSclIrqIsAffFullContinue(bool *bAffFull ,bool *bAffcount)
{
    if((*bAffFull))
    {
        (*bAffcount)++;
    }
}
void _DrvSclIrqIsAffFullContinually(bool *bAffFull ,bool *bAffcount)
{
    static u8 u8framecount = 0;
    DrvSclIrqInterruptEnable(SCLIRQ_AFF_FULL);
    if(!(*bAffFull))
    {
        //HalSclIrqSetMask(E_DRV_SCLIRQ_SCTOP_0,SCLIRQ_MSK_AFF_FULL,0);
        u8framecount = 0;
    }
    else
    {
        //HalSclIrqSetMask(E_DRV_SCLIRQ_SCTOP_0,SCLIRQ_MSK_AFF_FULL,1);
        u8framecount++;
        *bAffFull = 0;
    }
    if((u8framecount)>2)
    {
        *bAffcount = 0;
        DrvSclIrqSetDazaQueueWork(E_DRV_SCLIRQ_DAZA_BRESETFCLK);
        if(gstDropFrameCount <= 1)
        {
            gstDropFrameCount = 1;
        }
        u8framecount = 0;
    }
}
void _DrvSclIrqAffFullHandler(bool *bAffFull ,bool *bAffcount)
{
    SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&(EN_DBGMG_SCLIRQLEVEL_ELSE),
        "SC AFF FULL %lu @:%lu\n",gstSCInts.u32AffCount,gu32Time);
    SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&(EN_DBGMG_SCLIRQLEVEL_ELSE),
        "CROP H:%hd V:%hd \n",HalSclIrqGetRegVal(REG_SCL2_70_L),HalSclIrqGetRegVal(REG_SCL2_71_L));
    SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&(EN_DBGMG_SCLIRQLEVEL_ELSE),
        "CROP2 H:%hd V:%hd \n",HalSclIrqGetRegVal(REG_SCL2_72_L),HalSclIrqGetRegVal(REG_SCL2_73_L));
    *bAffFull = 1;
    DrvSclIrqDisable(SCLIRQ_AFF_FULL);
    _DrvSclIrqIsAffFullContinue(bAffFull,bAffcount);
    if(gstSCInts.u32AffCount == 15)
    {
        DrvSclIrqSetDazaQueueWork(E_DRV_SCLIRQ_DAZA_BCLOSECIIR);
    }
    else if(gstSCInts.u32AffCount == 30)
    {
        DrvSclOsClearEvent(_s32SYNCEventId, E_DRV_SCLIRQ_EVENT_BCLOSECIIR);
    }
    else if(gstSCInts.u32AffCount == 40)
    {
        DrvSclIrqSetDazaQueueWork(E_DRV_SCLIRQ_DAZA_BCLOSECIIR);
    }
}
/*
void _DrvSclIrqFrameInEndRealTimeHandler(void)
{
    if(DrvSclOsGetEvent(_s32SYNCEventId)&E_DRV_SCLIRQ_EVENT_FRMENDSYNC)
    {
        DrvSclIrqSetDMADoneEarlyISP(1);
    }
    else
    {
        DrvSclIrqSetDMADoneEarlyISP(0);
    }
    _DrvSclIrqHandlerFrmIn(1);
    if(gbBypassDNR)
    {
        HalSclIrqSetDNRBypass(0);
        gbBypassDNR = 0;
    }
    if(gbDMADoneEarlyISP)
    {
        if(!SCL_DELAYFRAME)
        {
            _DrvSclIrqSetFrmInWhenDMADoneEarlyIsp(1);
            //TODO:not open this time
            _DrvSclIrqFrmDoneEventHandler();
        }
        _DrvSclIrqCmdqBlankingHandler();
    }
    DrvSclOsSetEvent(_s32SYNCEventId, E_DRV_SCLIRQ_EVENT_ISPFRMEND);
}
*/
void _DrvSclIrqFrameInEndHandler(bool *bNonFRMEndCount,bool *bISPFramePerfect,bool *bAffFull)
{
    if(DrvSclOsGetEvent(_s32SYNCEventId)&E_DRV_SCLIRQ_EVENT_FRMENDSYNC)
    {
        DrvSclIrqSetDMADoneEarlyISP(1);
    }
    else
    {
        DrvSclIrqSetDMADoneEarlyISP(0);
    }
    DrvSclDmaSetIspFrameCount();
    *bISPFramePerfect = 1;
    _DrvSclIrqHandlerFrmIn(1);
    if(gbBypassDNR)
    {
        HalSclIrqSetDNRBypass(0);
        gbBypassDNR = 0;
    }
    if(gbDMADoneEarlyISP)
    {
        if(!SCL_DELAYFRAME)
        {
            _DrvSclIrqSetFrmInWhenDMADoneEarlyIsp(1);
            //TODO:not open this time
            _DrvSclIrqFrmDoneEventHandler();
        }
        _DrvSclIrqCmdqBlankingHandler();
    }
    DrvSclOsSetEvent(_s32SYNCEventId, E_DRV_SCLIRQ_EVENT_ISPFRMEND);
}
void _DrvSclIrqLDCVsyncInRealTimeHandler(void)
{
    u8 idx = 0;
    u8 loop = 0;
    if(gbEachDMAEnable)
    {
        if(gbInBlanking == 0)
        {
            for(idx = 0;idx<E_DRV_SCLDMA_CLIENT_NUM;idx++)
            {
                if(gstScldmaInfo.bDMAOnOff[idx])
                {
                    loop = 1;
                    break;
                }
            }
            if(!loop)
            {
                gbEachDMAEnable = 0;
                _DrvSclIrqSetIsBlankingRegion(1);
            }
        }
        else
        {
            _DrvSclIrqSetIsBlankingRegion(0);
        }
    }
    else
    {
        // for issue reset rstz_ctrl_clk_miu_sc_dnr cause to overwrite 0x0
        HalSclIrqSetReg(REG_SCL_DNR1_24_L, 0, BIT6);
        HalSclIrqSetReg(REG_SCL_DNR1_60_L, 0, BIT0);
    }
    //disable dnr for ISP not perfect
    if(!_IsFrmIN_Vsync())
    {
        HalSclIrqSetDNRBypass(1);
        gbBypassDNR = 1;
    }
}
void _DrvSclIrqLDCVsyncInHandler(bool *bISPFramePerfect ,bool *bAffFull)
{
    _DrvSclIrqHandlerFrmInShift();
    DrvSclOsSetEvent(DrvSclIrqGetIrqSYNCEventID(), E_DRV_SCLIRQ_EVENT_LDCSYNC);
    DrvSclOsClearEvent(DrvSclIrqGetIrqSYNCEventID(),(E_DRV_SCLIRQ_EVENT_FRMENDSYNC|E_DRV_SCLIRQ_EVENT_ISPFRMEND));
    if(gsclirqstate &E_DRV_SCLIRQ_EVENT_SET)
    {
        gsclirqstate = E_DRV_SCLIRQ_EVENT_CLEAR;
    }
    else if(gsclirqstate)
    {
        gstSCInts.u32ErrorCount++;
        if(gbDMADoneEarlyISP)
        {
            if(!SCL_DELAYFRAME)
            {
                SCL_DBGERR("[LDC]:ISP_IN lost:%lu\n",gstSCInts.u32ErrorCount);
                _DrvSclIrqSetFrmInWhenDMADoneEarlyIsp(0);
                //TODO:not open this time
                _DrvSclIrqFrmDoneEventHandler();
                if(gsclirqstate &E_DRV_SCLIRQ_EVENT_SET)
                {
                    gsclirqstate = E_DRV_SCLIRQ_EVENT_CLEAR;
                }
            }
        }
    }
    if(gbMonitorCropMode == 1)
    {
        DrvSclOsSetEvent(_s32SYNCEventId, E_DRV_SCLIRQ_EVENT_SYNC);
    }
    _DrvSclIrqSclFrameStartConnectToIsp();
}


void _DrvSclIrqDmaActiveHandler(DrvSclDmaClientType_e enclient)
{
    if(_Is_SCLDMA_RingMode(enclient) && DoubleBufferStatus)
    {
        _DrvSclIrqRingModeActive(enclient);
    }
    else if(_Is_SCLDMA_RingMode(enclient))
    {
        _DrvSclIrqRingModeActiveWithoutDoublebuffer(enclient);
    }
    else if(_Is_SCLDMA_SWRingMode(enclient))
    {
        _DrvSclIrqSWRingModeActive(enclient);
    }
    else if(enclient ==E_DRV_SCLDMA_3_FRM_W &&_Is_SCLDMA_SingleMode(E_DRV_SCLDMA_3_FRM_W))
    {
        _DrvSclIrqSC3SingleMode_Active();
    }
    else if(_Is_SCLDMA_SingleMode(enclient) && DoubleBufferStatus)
    {
        _DrvSclIrqSingleModeActive(enclient);
    }
    else if(_Is_SCLDMA_SingleMode(enclient))
    {
        _DrvSclIrqSingleModeActiveWithoutDoublebuffer(enclient);
    }
    DrvSclDmaSetSclFrameDoneTime(enclient ,DrvSclOsGetSystemTimeStamp());
}
bool _DrvSclIrqDmaActiveNHandler(DrvSclDmaClientType_e enclient)
{
    bool Ret=1;
    if(enclient < E_DRV_SCLDMA_3_FRM_R)
    {
        _DrvSclIrqSetFrameInFlag(enclient);
    }
    if(_Is_SCLDMA_RingMode(enclient) && DoubleBufferStatus)
    {
        _DrvSclIrqRingModeActive_N(enclient);
    }
    else if(_Is_SCLDMA_RingMode(enclient))
    {
        Ret = _DrvSclIrqRingModeActive_NWithoutDoublebuffer(enclient);
    }
    else if(_Is_SCLDMA_SWRingMode(enclient))
    {
        _DrvSclIrqSWRingModeActive_N(enclient);
    }
    else if(enclient ==E_DRV_SCLDMA_3_FRM_W && _Is_SCLDMA_SingleMode(E_DRV_SCLDMA_3_FRM_W))
    {
        _DrvSclIrqSC3SingleMode_Active_N();
    }
    else if(_Is_SCLDMA_SingleMode(enclient) && DoubleBufferStatus)
    {
        _DrvSclIrqSingleModeActive_N(enclient);
    }
    else if(_Is_SCLDMA_SingleMode(enclient))
    {
        Ret = _DrvSclIrqSingleModeActive_NWithoutDoublebuffer(enclient);
    }
    return Ret;
}
/*
static void _DrvSclIrqRingModeActiveWithoutDoublebufferRealTime(DrvSclDmaClientType_e enClient)
{
    u8 u8RPoint = 0;
    u32 u32Time;
    u32Time = ((u32)DrvSclOsGetSystemTimeStamp());
    u8RPoint = (_GetIdxType(enClient,E_DRV_SCLDMA_ACTIVE_BUFFER_OMX))>>4;
    gu16RealIdx[enClient] = HalSclDmaGetRWIdx(enClient);
    if(gu16RealIdx[enClient] == 0 && _GetIdxType(enClient,E_DRV_SCLDMA_ACTIVE_BUFFER_SCL) != gstScldmaInfo.bMaxid[enClient]&&
        !(_IsFlagType(enClient,E_DRV_SCLDMA_FLAG_BLANKING)))
    {
        _SetFlagType(enClient,E_DRV_SCLDMA_FLAG_FRMIGNORE);
        SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&Get_DBGMG_SCLIRQclient(enClient,0),
            "[DRVSCLDMA]%d wodb ISR ignore\n"
        ,enClient);
    }
    else
    {
        _ReSetFlagType(enClient,E_DRV_SCLDMA_FLAG_FRMIGNORE);
    }
    if(_IsFlagType(enClient,E_DRV_SCLDMA_FLAG_ACTIVE))
    {
         _SetFlagType(enClient,E_DRV_SCLDMA_FLAG_FRMIGNORE);
         SCL_DBGERR("[DRVSCLIRQ]%d!!!!!!!!!!!!double active\n",enClient);
    }
    else
    {
         _SetANDGetFlagType(enClient,E_DRV_SCLDMA_FLAG_ACTIVE,(~E_DRV_SCLDMA_FLAG_FRMDONE));
    }
    SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&Get_DBGMG_SCLIRQclient(enClient,0),
        "[DRVSCLDMA]%d realwodb ISR R_P=%hhx RealIdx=%hx flag:%hx @:%lu\n"
    ,enClient,u8RPoint,gu16RealIdx[enClient],gstScldmaInfo.bDmaflag[enClient],u32Time);
}
static bool _DrvSclIrqRingModeActive_NWithoutDoublebufferRealTime(DrvSclDmaClientType_e enClient)
{
    u8 u8RPoint = 0;
    gbdmaoff[enClient] = 0;
    if((!_IsFlagType(enClient,E_DRV_SCLDMA_FLAG_FRMIN)))
    {
        _SetFlagType(enClient,E_DRV_SCLDMA_FLAG_FRMIGNORE);
    }
    u8RPoint = (_GetIdxType(enClient,E_DRV_SCLDMA_ACTIVE_BUFFER_OMX))>>4;
    if(!_IsFlagType(enClient,E_DRV_SCLDMA_FLAG_FRMIGNORE))
    {
        _SetANDGetFlagType(enClient,E_DRV_SCLDMA_FLAG_FRMDONE,(~E_DRV_SCLDMA_FLAG_ACTIVE));
        if(u8RPoint==gu16RealIdx[enClient] && !_IsFlagType(enClient,E_DRV_SCLDMA_FLAG_BLANKING))//5 is debug freerun
        {
            //trig off
            if(u8RPoint != freerunID )
            {
                DrvSclDmaSetISRHandlerDmaOff(enClient,0);
                DrvSclDmaSetSWReTrigCount(enClient,1);
                gbdmaoff[enClient] = 1;
            }
        }
        SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&Get_DBGMG_SCLIRQclient(enClient,1), "[DRVSCLDMA]%d ISR actN realwodb flag:%hx idx:%hhx @:%lu\n"
        ,enClient,gstScldmaInfo.bDmaflag[enClient],gstScldmaInfo.bDMAidx[enClient],((u32)DrvSclOsGetSystemTimeStamp()));
        return 1;
    }
    else
    {
        if(u8RPoint != freerunID )
        {
            DrvSclDmaSetISRHandlerDmaOff(enClient,0);
            DrvSclDmaSetDMAIgnoreCount(enClient,1);
            _SetANDGetFlagType(enClient,E_DRV_SCLDMA_FLAG_FRMDONE,(~E_DRV_SCLDMA_FLAG_ACTIVE));
            gbdmaoff[enClient] = 1;
            SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&Get_DBGMG_SCLIRQclient(enClient,1),
                "[DRVSCLDMA]%d ISR actN realwodb ignore flag:%hx\n"
            ,enClient,gstScldmaInfo.bDmaflag[(enClient)]);
        }
        else
        {
            _SetANDGetFlagType(enClient,E_DRV_SCLDMA_FLAG_FRMDONE,(~E_DRV_SCLDMA_FLAG_ACTIVE));
            SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&Get_DBGMG_SCLIRQclient(enClient,1),
                "[DRVSCLDMA]%d ISR actN realwodb ignore flag:%hx\n"
            ,enClient,gstScldmaInfo.bDmaflag[(enClient)]);
        }
        return 0;
    }
    //sclprintf("ACTIVE_N:%lu\n",u32Time);
}
static void _DrvSclIrqSWRingModeActiveRealTime(DrvSclDmaClientType_e enclient)
{
    u32 u32Time;
    u32Time = ((u32)DrvSclOsGetSystemTimeStamp());
    if(_IsFlagType(enclient,E_DRV_SCLDMA_FLAG_ACTIVE))
    {
         _SetFlagType(enclient,E_DRV_SCLDMA_FLAG_FRMIGNORE);
         SCL_DBGERR("[DRVSCLIRQ]!!!!!!!!!!!!double active\n");
    }
    else
    {
         _SetANDGetFlagType(enclient,E_DRV_SCLDMA_FLAG_ACTIVE,(~E_DRV_SCLDMA_FLAG_FRMDONE));
    }
    if(_IsFlagType(enclient,E_DRV_SCLDMA_FLAG_BLANKING))
    {
        gu16RealIdx[enclient] = 0;
    }
    else
    {
        gu16RealIdx[enclient] = DrvSclDmaGetActiveBufferIdx(enclient);
    }
    SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&Get_DBGMG_SCLIRQclient(enclient,0), "[DRVSCLDMA]%d ISR SWRING flag:%hx ACT:%hhd@:%lu\n"
    ,enclient,gstScldmaInfo.bDmaflag[enclient],gu16RealIdx[enclient],u32Time);
}
static bool _DrvSclIrqSWRingModeActive_NRealTime(DrvSclDmaClientType_e enclient)
{
    DrvSclDmaFrameBufferConfig_t stTarget;
    u64 u64Time;
    bool Ret = 1;
    u64Time = ((u64)DrvSclOsGetSystemTimeStamp());
    if(!_IsFlagType(enclient,E_DRV_SCLDMA_FLAG_FRMIN) || _IsFlagType(enclient,E_DRV_SCLDMA_FLAG_DROP)
        || _IsFlagType(enclient,E_DRV_SCLDMA_FLAG_FRMIGNORE))
    {
        Ret = 0;
        if( _IsFlagType(enclient,E_DRV_SCLDMA_FLAG_FRMIGNORE))
        {
            DrvSclDmaSetISRHandlerDmaOff(enclient,0);
            DrvSclDmaSetDMAIgnoreCount(enclient,1);
            _SetFlagType(enclient,E_DRV_SCLDMA_FLAG_EVERDMAON);
        }
        else
        {
            _SetFlagType(enclient,E_DRV_SCLDMA_FLAG_FRMIGNORE);
        }
    }
    else
    {
        _SetANDGetFlagType(enclient,E_DRV_SCLDMA_FLAG_FRMDONE,(~E_DRV_SCLDMA_FLAG_ACTIVE));
        _ReSetFlagType(enclient,E_DRV_SCLDMA_FLAG_FRMIGNORE);
        stTarget.u8FrameAddrIdx = _GetIdxType(enclient,E_DRV_SCLDMA_ACTIVE_BUFFER_SCL);
        stTarget.u32FrameAddr   = gstScldmaInfo.u32Base_Y[enclient][stTarget.u8FrameAddrIdx];
        stTarget.u64FRMDoneTime = u64Time;
        stTarget.u16FrameWidth   = gstScldmaInfo.u16FrameWidth[enclient];
        stTarget.u16FrameHeight  = gstScldmaInfo.u16FrameHeight[enclient];
        if(DrvSclDmaMakeSureNextActiveId(enclient))
        {
            if(DrvSclDmaBufferEnQueue(enclient,stTarget))
            {
                DrvSclDmaChangeBufferIdx(enclient);
            }
        }
        SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&Get_DBGMG_SCLIRQclient(enclient,1), "[DRVSCLDMA]%d ISR SWRING actN flag:%hx idx:%hhd\n"
        ,enclient,gstScldmaInfo.bDmaflag[enclient],gstScldmaInfo.bDMAidx[enclient]);
    }
    return Ret;
}
static void _DrvSclIrqSingleModeActiveWithoutDoublebufferRealTime(DrvSclDmaClientType_e enclient)
{
    u32 u32Time = ((u32)DrvSclOsGetSystemTimeStamp());
    if(_IsFlagType(enclient,E_DRV_SCLDMA_FLAG_ACTIVE))
    {
         _SetFlagType(enclient,E_DRV_SCLDMA_FLAG_FRMIGNORE);
         SCL_DBGERR("[DRVSCLIRQ]!!!!!!!!!!!!double active\n");
    }
    else
    {
         _SetANDGetFlagType(enclient,E_DRV_SCLDMA_FLAG_ACTIVE,(~E_DRV_SCLDMA_FLAG_FRMDONE));
    }
    SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&EN_DBGMG_SCLIRQLEVEL_SC1SINGLE, "[DRVSCLIRQ]singlewodb act:%lu\n",u32Time);
}
static bool _DrvSclIrqSingleModeActive_NWithoutDoublebufferRealTime(DrvSclDmaClientType_e enclient)
{
    u32 u32Time;
    bool Ret = 1;
    u32Time = ((u32)DrvSclOsGetSystemTimeStamp());
    _SetANDGetFlagType(enclient,E_DRV_SCLDMA_FLAG_FRMDONE,(~E_DRV_SCLDMA_FLAG_ACTIVE));
    if(enclient< E_DRV_SCLDMA_3_FRM_R)
    {
        if(!_IsFlagType(enclient,E_DRV_SCLDMA_FLAG_FRMIN) || _IsFlagType(enclient,E_DRV_SCLDMA_FLAG_DROP)
            || _IsFlagType(enclient,E_DRV_SCLDMA_FLAG_FRMIGNORE))
        {
            Ret = 0;
            if( _IsFlagType(enclient,E_DRV_SCLDMA_FLAG_FRMIGNORE))
            {
                DrvSclDmaSetISRHandlerDmaOff(enclient,0);
                DrvSclDmaSetDMAIgnoreCount(enclient,1);
                _SetFlagType(enclient,E_DRV_SCLDMA_FLAG_EVERDMAON);
            }
            else
            {
                _SetFlagType(enclient,E_DRV_SCLDMA_FLAG_FRMIGNORE);
            }
            SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&EN_DBGMG_SCLIRQLEVEL_SC1SINGLE, "[DRVSCLIRQ]singlewodb ignore actN:%lu\n",u32Time);
        }
        else
        {
            _ReSetFlagType(enclient,E_DRV_SCLDMA_FLAG_FRMIGNORE);
            _SetFlagType(enclient,E_DRV_SCLDMA_FLAG_DMAOFF);
            SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&EN_DBGMG_SCLIRQLEVEL_SC1SINGLE, "[DRVSCLIRQ]singlewodb actN:%lu\n",u32Time);
        }
    }
    return Ret;
}
void _DrvSclIrqDmaActiveRealTimeHandler(DrvSclDmaClientType_e enclient)
{
    if(_Is_SCLDMA_RingMode(enclient))
    {
        //_DrvSclIrqRingModeActiveWithoutDoublebufferRealTime(enclient);
    }
    else if(_Is_SCLDMA_SWRingMode(enclient))
    {
        _DrvSclIrqSWRingModeActiveRealTime(enclient);
    }
    else if(enclient ==E_DRV_SCLDMA_3_FRM_W &&_Is_SCLDMA_SingleMode(E_DRV_SCLDMA_3_FRM_W))
    {
    }
    else if(_Is_SCLDMA_SingleMode(enclient))
    {
        _DrvSclIrqSingleModeActiveWithoutDoublebufferRealTime(enclient);
    }
    DrvSclDmaSetSclFrameDoneTime(enclient,(DrvSclOsGetSystemTimeStamp()));
}
void _DrvSclIrqDmaActiveNRealTimeHandler(DrvSclDmaClientType_e enclient)
{
    if(enclient < E_DRV_SCLDMA_3_FRM_R)
    {
        _DrvSclIrqSetFrameInFlag(enclient);
    }
    if(_Is_SCLDMA_RingMode(enclient))
    {
        _DrvSclIrqRingModeActive_NWithoutDoublebufferRealTime(enclient);
    }
    else if(_Is_SCLDMA_SWRingMode(enclient))
    {
        _DrvSclIrqSWRingModeActive_NRealTime(enclient);
    }
    else if(enclient ==E_DRV_SCLDMA_3_FRM_W && _Is_SCLDMA_SingleMode(E_DRV_SCLDMA_3_FRM_W))
    {
    }
    else if(_Is_SCLDMA_SingleMode(enclient))
    {
        _DrvSclIrqSingleModeActive_NWithoutDoublebufferRealTime(enclient);
    }
}
*/
static int SCLIRQ_DazaIST(void)
{
    u32 u32Events;
    DrvSclIrqDazaEventType_e enDAZAEvent;
    SCL_DBGERR("[DRVSCLIRQ]%s:going event:%x @:%ld\n",__FUNCTION__,genDAZAEvent,((u32)DrvSclOsGetSystemTimeStamp()));
    if((DrvSclOsGetEvent(DrvSclIrqGetIrqSYNCEventID())&(E_DRV_SCLIRQ_EVENT_ISPFRMEND|E_DRV_SCLIRQ_EVENT_FRMENDSYNC))
        !=(E_DRV_SCLIRQ_EVENT_ISPFRMEND|E_DRV_SCLIRQ_EVENT_FRMENDSYNC))
    {
        DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(), E_DRV_SCLIRQ_EVENT_ISPFRMEND, &u32Events, E_DRV_SCLOS_EVENT_MD_OR, 200); // get status: FRM END
        DrvSclOsDelayTaskUs(100);
    }
    DRV_SCLIRQ_MUTEX_LOCK_ISR();
    enDAZAEvent = genDAZAEvent;
    genDAZAEvent &= (~enDAZAEvent);
    DRV_SCLIRQ_MUTEX_UNLOCK_ISR();
    if(enDAZAEvent &E_DRV_SCLIRQ_DAZA_BRESETFCLK)
    {
        HalSclIrqSetReg(REG_SCL0_01_L, BIT3, BIT3);
        HalSclIrqSetReg(REG_SCL0_01_L, 0, BIT3);
    }
    if(enDAZAEvent &E_DRV_SCLIRQ_DAZA_BCLOSECIIR)
    {
        DrvSclOsSetEventIrq(_s32SYNCEventId, E_DRV_SCLIRQ_EVENT_BCLOSECIIR);
    }
    if(enDAZAEvent &E_DRV_SCLIRQ_DAZA_BCLOSELDCP)
    {
        SCL_DBGERR("[DRVSCLIRQ]%s:!!!E_SCLIRQ_DAZA_BCLOSELDCP@:%lu\n",__FUNCTION__,((u32)DrvSclOsGetSystemTimeStamp()));
        HalSclDmaSetSC1DMAEn(E_DRV_SCLDMA_IMI_W,0);
        HalSclDmaSetSC1DMAEn(E_DRV_SCLDMA_IMI_W,1);
        HalSclIrqSetReg(REG_SCL0_01_L, BIT3, BIT3);
        HalSclIrqSetReg(REG_SCL0_01_L, 0, BIT3);
        HalSclIrqSetReg(REG_SCL0_03_L, 0, BIT12);
        DRV_SCLIRQ_MUTEX_LOCK_ISR();
        if(gstDropFrameCount <= 1)
        {
            gstDropFrameCount = 1;
        }
        DrvSclOsSetSclFrameDelay(0);
        DRV_SCLIRQ_MUTEX_UNLOCK_ISR();
    }
    if(enDAZAEvent &E_DRV_SCLIRQ_DAZA_BRESETLDCP)
    {
        SCL_DBGERR("[DRVSCLIRQ]%s:!!!E_SCLIRQ_DAZA_BRESETLDCP@:%lu\n",__FUNCTION__,((u32)DrvSclOsGetSystemTimeStamp()));
        HalSclIrqSetReg(REG_SCL0_03_L, BIT12, BIT12);
        DRV_SCLIRQ_MUTEX_LOCK_ISR();
        DrvSclOsSetSclFrameDelay(1);
        DRV_SCLIRQ_MUTEX_UNLOCK_ISR();
    }
    return 0;
}
void _DrvSclIrqSetHvspHSram(DrvSclHvspIdType_e enHVSP_ID,u32 u32EventFlag)
{
    u32 u32Events;
    if(DrvSclIrqGetIsBlankingRegion())
    {
        DrvSclHvspSetSCIQHSRAM(enHVSP_ID);
        DrvSclOsClearEventIRQ(_s32SYNCEventId,(u32EventFlag));
    }
    else
    {
        SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_ISR, "[DRVSCLIRQ]%s:SRAMSETH wating\n",__FUNCTION__);
        DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(), E_DRV_SCLIRQ_EVENT_FRMEND, &u32Events, E_DRV_SCLOS_EVENT_MD_OR, 200); // get status: FRM END
        DrvSclHvspSetSCIQHSRAM(enHVSP_ID);
        DrvSclOsClearEventIRQ(_s32SYNCEventId,(u32EventFlag));
    }
}
void _DrvSclIrqSetHvspVSram(DrvSclHvspIdType_e enHVSP_ID,u32 u32EventFlag)
{
    u32 u32Events;
    if(DrvSclIrqGetIsBlankingRegion())
    {
        DrvSclHvspSetSCIQVSRAM(enHVSP_ID);
        DrvSclOsClearEventIRQ(_s32SYNCEventId,(u32EventFlag));
    }
    else
    {
        SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_ISR, "[DRVSCLIRQ]%s:SRAMSETV wating\n",__FUNCTION__);
        DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(), E_DRV_SCLIRQ_EVENT_FRMEND, &u32Events, E_DRV_SCLOS_EVENT_MD_OR, 200); // get status: FRM END
        DrvSclHvspSetSCIQVSRAM(enHVSP_ID);
        DrvSclOsClearEventIRQ(_s32SYNCEventId,(u32EventFlag));
    }
}
static int SCLIRQ_CMDQIST(void)
{
    u32 u32Events;
    u32Events = DrvSclOsGetEvent(_s32SYNCEventId);
    SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_ISR, "[DRVSCLIRQ]%s:going\n",__FUNCTION__);
    if(u32Events & E_DRV_SCLIRQ_EVENT_BRESETFCLK)
    {
        DrvSclIrqSetDazaQueueWork(E_DRV_SCLIRQ_DAZA_BRESETFCLK);
        DrvSclOsClearEventIRQ(_s32SYNCEventId,(E_DRV_SCLIRQ_EVENT_BRESETFCLK));
    }
    if(u32Events&E_DRV_SCLIRQ_EVENT_VIP)
    {
        if((VIPSETRULE() == E_DRV_SCLOS_VIPSETRUle_CMDQCHECK)||(VIPSETRULE() == E_DRV_SCLOS_VIPSETRUle_CMDQALLCheck))
        {
            MDrvSclVipCheckEachIPByCmdqIst();
        }
        DrvSclOsClearEventIRQ(_s32SYNCEventId,E_DRV_SCLIRQ_EVENT_VIP);
    }
    if(u32Events&E_DRV_SCLIRQ_EVENT_GM10to12_R)
    {
        {
            DrvSclCmdqCheckVIPSRAM(E_DRV_SCLIRQ_EVENT_GM10to12_R);
        }
        DrvSclOsClearEventIRQ(_s32SYNCEventId,E_DRV_SCLIRQ_EVENT_GM10to12_R);
    }
    if(u32Events&E_DRV_SCLIRQ_EVENT_GM10to12_G)
    {
        {
            DrvSclCmdqCheckVIPSRAM(E_DRV_SCLIRQ_EVENT_GM10to12_G);
        }
        DrvSclOsClearEventIRQ(_s32SYNCEventId,E_DRV_SCLIRQ_EVENT_GM10to12_G);
    }
    if(u32Events&E_DRV_SCLIRQ_EVENT_GM10to12_B)
    {
        {
            DrvSclCmdqCheckVIPSRAM(E_DRV_SCLIRQ_EVENT_GM10to12_B);
        }
        DrvSclOsClearEventIRQ(_s32SYNCEventId,E_DRV_SCLIRQ_EVENT_GM10to12_B);
    }
    if(u32Events&E_DRV_SCLIRQ_EVENT_GM12to10_R)
    {
        {
            DrvSclCmdqCheckVIPSRAM(E_DRV_SCLIRQ_EVENT_GM12to10_R);
        }
        DrvSclOsClearEventIRQ(_s32SYNCEventId,E_DRV_SCLIRQ_EVENT_GM12to10_R);
    }
    if(u32Events&E_DRV_SCLIRQ_EVENT_GM12to10_G)
    {
        {
            DrvSclCmdqCheckVIPSRAM(E_DRV_SCLIRQ_EVENT_GM12to10_G);
        }
        DrvSclOsClearEventIRQ(_s32SYNCEventId,E_DRV_SCLIRQ_EVENT_GM12to10_G);
    }
    if(u32Events&E_DRV_SCLIRQ_EVENT_GM12to10_B)
    {
        {
            DrvSclCmdqCheckVIPSRAM(E_DRV_SCLIRQ_EVENT_GM12to10_B);
        }
        DrvSclOsClearEventIRQ(_s32SYNCEventId,E_DRV_SCLIRQ_EVENT_GM12to10_B);
    }
    if(u32Events&E_DRV_SCLIRQ_EVENT_GAMMA_Y)
    {
        {
            DrvSclCmdqCheckVIPSRAM(E_DRV_SCLIRQ_EVENT_GAMMA_Y);
        }
        DrvSclOsClearEventIRQ(_s32SYNCEventId,E_DRV_SCLIRQ_EVENT_GAMMA_Y);
    }
    if(u32Events&E_DRV_SCLIRQ_EVENT_GAMMA_U)
    {
        {
            DrvSclCmdqCheckVIPSRAM(E_DRV_SCLIRQ_EVENT_GAMMA_U);
        }
        DrvSclOsClearEventIRQ(_s32SYNCEventId,E_DRV_SCLIRQ_EVENT_GAMMA_U);
    }
    if(u32Events&E_DRV_SCLIRQ_EVENT_GAMMA_V)
    {
        {
            DrvSclCmdqCheckVIPSRAM(E_DRV_SCLIRQ_EVENT_GAMMA_V);
        }
        DrvSclOsClearEventIRQ(_s32SYNCEventId,E_DRV_SCLIRQ_EVENT_GAMMA_V);
    }
    if(u32Events & E_DRV_SCLIRQ_EVENT_SC1HSRAMSET)
    {
        _DrvSclIrqSetHvspHSram(E_DRV_SCLHVSP_ID_1,E_DRV_SCLIRQ_EVENT_SC1HSRAMSET);
    }
    if(u32Events & E_DRV_SCLIRQ_EVENT_SC1VSRAMSET)
    {
        _DrvSclIrqSetHvspVSram(E_DRV_SCLHVSP_ID_1,E_DRV_SCLIRQ_EVENT_SC1VSRAMSET);
    }
    if(u32Events & E_DRV_SCLIRQ_EVENT_SC2HSRAMSET)
    {
        _DrvSclIrqSetHvspHSram(E_DRV_SCLHVSP_ID_2,E_DRV_SCLIRQ_EVENT_SC2HSRAMSET);
    }
    if(u32Events & E_DRV_SCLIRQ_EVENT_SC2VSRAMSET)
    {
        _DrvSclIrqSetHvspVSram(E_DRV_SCLHVSP_ID_2,E_DRV_SCLIRQ_EVENT_SC2VSRAMSET);
    }
    if(u32Events & E_DRV_SCLIRQ_EVENT_SC3HSRAMSET)
    {
        _DrvSclIrqSetHvspHSram(E_DRV_SCLHVSP_ID_3,E_DRV_SCLIRQ_EVENT_SC3HSRAMSET);
    }
    if(u32Events & E_DRV_SCLIRQ_EVENT_SC3VSRAMSET)
    {
        _DrvSclIrqSetHvspVSram(E_DRV_SCLHVSP_ID_3,E_DRV_SCLIRQ_EVENT_SC3VSRAMSET);
    }

   return 0;

}
void _DrvSclIrqIsDMAHangUp(u64 u64Flag)
{
    static bool bVsyncWithVend = 0;
    static bool bLDCEvenOpen = 0;
    static bool bLDCEvenOpenCount = 0;
    if(u64Flag &SCLIRQ_MSK_VSYNC_FCLK_LDC)
    {
        bVsyncWithVend ++;
    }
    else
    {
        bVsyncWithVend = 0;
        bLDCEvenOpenCount++;
    }
    if(bLDCEvenOpen && bLDCEvenOpenCount>100)
    {
        DrvSclIrqSetDazaQueueWork(E_DRV_SCLIRQ_DAZA_BRESETLDCP);
        bLDCEvenOpen = 0;
        bLDCEvenOpenCount = 0;
    }
    if(bVsyncWithVend>3 && (HalSclIrqGetRegVal(REG_SCL0_03_L)&BIT12))
    {
        DrvSclIrqSetDazaQueueWork(E_DRV_SCLIRQ_DAZA_BCLOSELDCP);
        bLDCEvenOpen = 1;
        bVsyncWithVend = 0;
        bLDCEvenOpenCount = 0;
    }
}
void _DrvSclIrqSetISPBlankingTime(u32 u32DiffTime)
{
    static bool bChangeTimingCount = 0;
    if(gstSCInts.u32ISPBlankingTime > u32DiffTime && (u32DiffTime > CRITICAL_SECTION_TIMING_MIN))
    {
        //for get most critical
        gstSCInts.u32ISPBlankingTime = u32DiffTime;
    }
    else if(gstSCInts.u32ISPBlankingTime == 0)
    {
        //for get inital
        gstSCInts.u32ISPBlankingTime = u32DiffTime;
    }
    else if (gstSCInts.u32ISPBlankingTime < u32DiffTime)
    {
        if(u32DiffTime - gstSCInts.u32ISPBlankingTime > CHANGE_CRITICAL_SECTION_TIMING_RANGE)
        {
            // for timing change
            bChangeTimingCount ++;
        }
        else
        {
            bChangeTimingCount = 0;
        }
        if(bChangeTimingCount > CHANGE_CRITICAL_SECTION_TIMING_COUNT)
        {
            // for ensure timing change
            gstSCInts.u32ISPBlankingTime = u32DiffTime;
            bChangeTimingCount = 0;
            SCL_DBGERR("[SCLIRQ]chang time :%lu\n",gstSCInts.u32ISPBlankingTime);
        }
    }
}
u64 _DrvSclIrqFlagRefine(u64 u64Flag)
{
    static bool bDMAHangUp = 0;
    // for ensure blanking is real if interrupt not stable.

    if((u64Flag &SCLIRQ_MSK_VSYNC_FCLK_LDC)&&(u64Flag &SCLIRQ_MSK_SC1_ENG_FRM_END)&&(u64Flag &SCLIRQ_MSK_SC_IN_FRM_END))
    {
        // for no handle case
        bDMAHangUp = 0;
        return u64Flag;
    }
    if((u64Flag &SCLIRQ_MSK_SC_IN_FRM_END)&&(bDMAHangUp))
    {
        // shift frame end to real blanking when interrupt not stable.
        u64Flag |=(SCLIRQ_MSK_SC1_ENG_FRM_END);
        bDMAHangUp = 0;
    }
    if((u64Flag &SCLIRQ_MSK_VSYNC_FCLK_LDC)&&(u64Flag &SCLIRQ_MSK_SC1_ENG_FRM_END))
    {
        // for make sure frame end is blanking
        u64Flag &=(~SCLIRQ_MSK_SC1_ENG_FRM_END);
        bDMAHangUp = 1;
    }
    else if((u64Flag &SCLIRQ_MSK_VSYNC_FCLK_LDC) && bDMAHangUp &&!(u64Flag &SCLIRQ_MSK_SC1_ENG_FRM_END))
    {
        // for already recover
        bDMAHangUp = 0;
    }
    else if((u64Flag &SCLIRQ_MSK_SC1_ENG_FRM_END) && bDMAHangUp &&!(u64Flag &SCLIRQ_MSK_VSYNC_FCLK_LDC))
    {
        // for already recover
        bDMAHangUp = 0;
    }
    return u64Flag;
}
void SCLIRQ_DazaThread(void)
{
    while(1)
    {
        if(DrvSclOsGetProbeInformation(E_DRV_SCLOS_INIT_HVSP) && DrvSclOsGetProbeInformation(E_DRV_SCLOS_INIT_DMA))
        {
            break;
        }
        else
        {
            SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&EN_DBGMG_SCLIRQLEVEL_ELSE, "[DRVSCLIRQ]%s:going \n",__FUNCTION__);
            DrvSclOsTaskWait((u32)(0x1 <<(gstThreadCfg.s32Taskid[E_DRV_SCLIRQ_SCTASKID_DAZA] &0xFFFF)));
        }

    }
    while(1)
    {
        SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&EN_DBGMG_SCLIRQLEVEL_ELSE, "[DRVSCLIRQ]%s:going \n",__FUNCTION__);
        if(DrvSclOsGetQueueExist((s32)gstThreadCfg.s32HandlerId[E_DRV_SCLIRQ_SCTASK_WORKQUEUEDAZA]) == TRUE)
        {
            if(DrvSclOsQueueWait(gstThreadCfg.s32HandlerId[E_DRV_SCLIRQ_SCTASK_WORKQUEUEDAZA]))
            {
                SCLIRQ_DazaIST();
            }
        }
        if(DrvSclOsGetQueueExist((s32)gstThreadCfg.s32HandlerId[E_DRV_SCLIRQ_SCTASK_WORKQUEUECMDQ]) == TRUE)
        {
            if(DrvSclOsQueueWait(gstThreadCfg.s32HandlerId[E_DRV_SCLIRQ_SCTASK_WORKQUEUECMDQ]))
            {
                SCLIRQ_CMDQIST();
            }
        }
        DrvSclOsTaskWait((u32)(0x1 <<(gstThreadCfg.s32Taskid[E_DRV_SCLIRQ_SCTASKID_DAZA] &0xFFFF)));
    }
}
void SCLIRQ_IST(void)
{
#if 0
    static bool bAffFull = 0,bAffcount=0;
    static bool bISPFramePerfect = 0;
    static bool bNonFRMEndCount = 0;
    u32 u32flag =0;
    bool bframeenduse = 0;
    SCL_RTKDBG(0,"[DRVSCLIRQ]%s:going \n",__FUNCTION__);
    while(1)
    {
        if(DrvSclOsGetProbeInformation(E_DRV_SCLOS_INIT_HVSP) && DrvSclOsGetProbeInformation(E_DRV_SCLOS_INIT_DMA))
        {
            break;
        }
        else
        {
            DrvSclOsTaskWait((u32)(0x1 <<E_DRV_SCLIRQ_SCTASKID_IRQ));
        }

    }
    while(1)
    {
        if(DrvSclOsGetQueueExist((s32)gstThreadCfg.s32HandlerId[E_DRV_SCLIRQ_SCTASK_WORKQUEUEIRQ]) == TRUE)
        {
            DrvSclOsQueueWait(gstThreadCfg.s32HandlerId[E_DRV_SCLIRQ_SCTASK_WORKQUEUEIRQ]);
            //SCL_ERR("[DRVSCLIRQ]%s:going \n",__FUNCTION__);
            //SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&EN_DBGMG_SCLIRQLEVEL_ELSE, "[DRVSCLIRQ]%s:going \n",__FUNCTION__);
            if(DrvSclOsGetandClearEventRing(E_DRV_SCLIRQ_SCIRQ_AFFFULL))
            {
                u32flag |= (0x1<<E_DRV_SCLIRQ_SCIRQ_AFFFULL);
            }
            if(DrvSclOsGetandClearEventRing(E_DRV_SCLIRQ_SCIRQ_INFRMEND))
            {
                u32flag |= (0x1<<E_DRV_SCLIRQ_SCIRQ_INFRMEND);
            }
            if(DrvSclOsGetandClearEventRing(E_DRV_SCLIRQ_SCIRQ_FCLKLDC))
            {
                u32flag |= (0x1<<E_DRV_SCLIRQ_SCIRQ_FCLKLDC);
            }
            // SC1 FRM
            if(DrvSclOsGetandClearEventRing(E_DRV_SCLIRQ_SCIRQ_SC1FRMWACTIVE))
            {
                u32flag |= (0x1<<E_DRV_SCLIRQ_SCIRQ_SC1FRMWACTIVE);
            }

            // SC2 FRM
            if(DrvSclOsGetandClearEventRing(E_DRV_SCLIRQ_SCIRQ_SC2FRMWACTIVE))
            {
                u32flag |= (0x1<<E_DRV_SCLIRQ_SCIRQ_SC2FRMWACTIVE);
            }
            // SC2 FRM2
            if(DrvSclOsGetandClearEventRing(E_DRV_SCLIRQ_SCIRQ_SC2FRM2WACTIVE))
            {
                u32flag |= (0x1<<E_DRV_SCLIRQ_SCIRQ_SC2FRM2WACTIVE);
            }

            // SC1 SNP
            if(DrvSclOsGetandClearEventRing(E_DRV_SCLIRQ_SCIRQ_SC1SNPWACTIVE))
            {
                u32flag |= (0x1<<E_DRV_SCLIRQ_SCIRQ_SC1SNPWACTIVE);
            }
            // SC1 FRM
            if(DrvSclOsGetandClearEventRing(E_DRV_SCLIRQ_SCIRQ_SC1FRMWACTIVE_N))
            {
                u32flag |= (0x1<<E_DRV_SCLIRQ_SCIRQ_SC1FRMWACTIVE_N);
            }
            // SC1 SNP
            if(DrvSclOsGetandClearEventRing(E_DRV_SCLIRQ_SCIRQ_SC1SNPWACTIVE_N))
            {
                u32flag |= (0x1<<E_DRV_SCLIRQ_SCIRQ_SC1SNPWACTIVE_N);
            }
            // SC2 FRM
            if(DrvSclOsGetandClearEventRing(E_DRV_SCLIRQ_SCIRQ_SC2FRMWACTIVE_N))
            {
                u32flag |= (0x1<<E_DRV_SCLIRQ_SCIRQ_SC2FRMWACTIVE_N);
            }
            // SC2 FRM2
            if(DrvSclOsGetandClearEventRing(E_DRV_SCLIRQ_SCIRQ_SC2FRM2WACTIVE_N))
            {
                u32flag |= (0x1<<E_DRV_SCLIRQ_SCIRQ_SC2FRM2WACTIVE_N);
            }
            // SC SRC FRMEND
            if(DrvSclOsGetandClearEventRing(E_DRV_SCLIRQ_SCIRQ_SCTOPFRMEND))
            {
                u32flag |= (0x1<<E_DRV_SCLIRQ_SCIRQ_SCTOPFRMEND);
            }
            // SC3 FRM
            if(DrvSclOsGetandClearEventRing(E_DRV_SCLIRQ_SCIRQ_SC3FRMWACTIVE))
            {
                u32flag |= (0x1<<E_DRV_SCLIRQ_SCIRQ_SC3FRMWACTIVE);
            }

            if(DrvSclOsGetandClearEventRing(E_DRV_SCLIRQ_SCIRQ_SC3FRMWACTIVE_N))
            {
                u32flag |= (0x1<<E_DRV_SCLIRQ_SCIRQ_SC3FRMWACTIVE_N);
            }
            if(DrvSclOsGetandClearEventRing(E_DRV_SCLIRQ_SCIRQ_CMDQ0TRIG))
            {
                u32flag |= (0x1<<E_DRV_SCLIRQ_SCIRQ_CMDQ0TRIG);
            }

            if(DrvSclOsGetandClearEventRing(E_DRV_SCLIRQ_SCIRQ_CMDQ0DONE))
            {
                u32flag |= (0x1<<E_DRV_SCLIRQ_SCIRQ_CMDQ0DONE);
            }
            DRV_SCLIRQ_MUTEX_LOCK_ISR();
            if(u32flag & (0x1<<E_DRV_SCLIRQ_SCIRQ_AFFFULL))
            {
                _DrvSclIrqAffFullHandler(&bAffFull, &bAffcount);
            }
            if(u32flag &(0x1<<E_DRV_SCLIRQ_SCIRQ_INFRMEND))
            {
                _DrvSclIrqFrameInEndHandler(&bNonFRMEndCount,&bISPFramePerfect,&bAffFull);
            }
            if((u32flag &(0x1<<(E_DRV_SCLIRQ_SCIRQ_SCTOPFRMEND)))&&(
                (u32flag &(0x1<<(E_DRV_SCLIRQ_SCIRQ_SC1FRMWACTIVE_N)))||
                (u32flag &(0x1<<(E_DRV_SCLIRQ_SCIRQ_SC1SNPWACTIVE_N)))||
                (u32flag &(0x1<<(E_DRV_SCLIRQ_SCIRQ_SC2FRMWACTIVE_N)))||
                (u32flag &(0x1<<(E_DRV_SCLIRQ_SCIRQ_SC2FRM2WACTIVE_N)))))
            {
                bframeenduse = 1;
            }
            else if (u32flag &(0x1<<(E_DRV_SCLIRQ_SCIRQ_SCTOPFRMEND)))
            {
                SCL_RTKDBG(0,"[DRVSCLIRQ]FRMEND :%x @:%lu\n",gsclirqstate,((u32)DrvSclOsGetSystemTimeStamp()));
                _DrvSclIrqSetFrmEndInterruptStatus(gu32Time);
                bNonFRMEndCount = 0;
                _DrvSclIrqIsAffFullContinually(&bAffFull, &bAffcount);
                bframeenduse = 0;
            }
            if(u32flag &(0x1<<(E_DRV_SCLIRQ_SCIRQ_FCLKLDC)))
            {
                _DrvSclIrqLDCVsyncInHandler(&bISPFramePerfect,&bAffFull);
                bAffFull = 0;
            }
            // SC1 FRM
            if(u32flag &(0x1<<(E_DRV_SCLIRQ_SCIRQ_SC1FRMWACTIVE)))
            {
                _DrvSclIrqDmaActiveHandler(E_DRV_SCLDMA_1_FRM_W);
            }

            // SC2 FRM
            if(u32flag &(0x1<<(E_DRV_SCLIRQ_SCIRQ_SC2FRMWACTIVE)))
            {
                _DrvSclIrqDmaActiveHandler(E_DRV_SCLDMA_2_FRM_W);
            }
            // SC2 FRM2
            if(u32flag &(0x1<<(E_DRV_SCLIRQ_SCIRQ_SC2FRM2WACTIVE)))
            {
                _DrvSclIrqDmaActiveHandler(E_DRV_SCLDMA_2_FRM2_W);
            }

            // SC1 SNP
            if(u32flag &(0x1<<(E_DRV_SCLIRQ_SCIRQ_SC1SNPWACTIVE)))
            {
                _DrvSclIrqDmaActiveHandler(E_DRV_SCLDMA_1_SNP_W);
            }
            // SC1 FRM
            if(u32flag &(0x1<<(E_DRV_SCLIRQ_SCIRQ_SC1FRMWACTIVE_N)))
            {
                if(_DrvSclIrqDmaActiveNHandler(E_DRV_SCLDMA_1_FRM_W))
                {
                    gsclirqstate |= (E_DRV_SCLIRQ_EVENT_SC1FRM|E_DRV_SCLIRQ_EVENT_ISTSC1FRM);
                }
            }
            // SC1 SNP
            if(u32flag &(0x1<<(E_DRV_SCLIRQ_SCIRQ_SC1SNPWACTIVE_N)))
            {
                if(_DrvSclIrqDmaActiveNHandler(E_DRV_SCLDMA_1_SNP_W))
                {
                    gsclirqstate |= (E_DRV_SCLIRQ_EVENT_SC1SNP|E_DRV_SCLIRQ_EVENT_ISTSC1SNP);
                }
            }
            // SC2 FRM
            if(u32flag &(0x1<<(E_DRV_SCLIRQ_SCIRQ_SC2FRMWACTIVE_N)))
            {
                if(_DrvSclIrqDmaActiveNHandler(E_DRV_SCLDMA_2_FRM_W))
                {
                    gsclirqstate |= (E_DRV_SCLIRQ_EVENT_SC2FRM|E_DRV_SCLIRQ_EVENT_ISTSC2FRM);
                }
            }
            // SC2 FRM2
            if(u32flag &(0x1<<(E_DRV_SCLIRQ_SCIRQ_SC2FRM2WACTIVE_N)))
            {
                if(_DrvSclIrqDmaActiveNHandler(E_DRV_SCLDMA_2_FRM2_W))
                {
                    DrvSclOsSetEventIrq(_s32FRMENDEventId, (E_DRV_SCLIRQ_EVENT_SC2FRM2|E_DRV_SCLIRQ_EVENT_ISTSC2FRM2));
                }
            }
            // SC SRC FRMEND
            if((u32flag &(0x1<<(E_DRV_SCLIRQ_SCIRQ_SCTOPFRMEND))) && bframeenduse)
            {
                SCL_RTKDBG(0,"[DRVSCLIRQ]FRMEND :%x @:%lu\n",gsclirqstate,((u32)DrvSclOsGetSystemTimeStamp()));
                _DrvSclIrqSetFrmEndInterruptStatus(gu32Time);
                bNonFRMEndCount = 0;
                _DrvSclIrqIsAffFullContinually(&bAffFull, &bAffcount);
                bframeenduse = 0;
            }
            // SC3 FRM
            if(u32flag &(0x1<<(E_DRV_SCLIRQ_SCIRQ_SC3FRMWACTIVE)))
            {
                gscl3irqstate = E_DRV_SCLIRQ_SC3EVENT_CLEAR;
                gscl3irqstate |= (E_DRV_SCLIRQ_SC3EVENT_ACTIVE | E_DRV_SCLIRQ_SC3EVENT_ISTACTIVE);
                _DrvSclIrqDmaActiveHandler(E_DRV_SCLDMA_3_FRM_W);
            }

            if(u32flag &(0x1<<(E_DRV_SCLIRQ_SCIRQ_SC3FRMWACTIVE_N)))
            {
                gscl3irqstate |= (E_DRV_SCLIRQ_SC3EVENT_ACTIVEN | E_DRV_SCLIRQ_SC3EVENT_ISTACTIVEN | E_DRV_SCLIRQ_SC3EVENT_IRQ);
                _DrvSclIrqDmaActiveNHandler(E_DRV_SCLDMA_3_FRM_W);
            }
            if(u32flag &(0x1<<(E_DRV_SCLIRQ_SCIRQ_CMDQ0TRIG)))
            {
                _DrvSclIrqCmdqStateTrig();
            }

            if(u32flag &(0x1<<(E_DRV_SCLIRQ_SCIRQ_CMDQ0DONE)))
            {
                _DrvSclIrqCmdqStateDone();
            }
            if(u32flag)
            {
                SCL_RTKDBG(0,"[DRVSCLIRQ]IST :%lx @:%lu\n",u32flag,((u32)DrvSclOsGetSystemTimeStamp()));
                u32flag = 0;
            }
            DRV_SCLIRQ_MUTEX_UNLOCK_ISR();
        }
        else
        {
            SCL_ERR("[DRVSCLIRQ]%s:WorkQuue Not Create \n",__FUNCTION__);
            DrvSclOsDelayTask(1000);
        }
    }
	#endif
}
bool _Delete_SCLIRQ_IST(void)
{
    gstThreadCfg.flag = 0;
    DrvSclOsFlushWorkQueue(1,gstThreadCfg.s32Taskid[E_DRV_SCLIRQ_SCTASKID_DAZA]);
    DrvSclOsestroyWorkQueueTask(gstThreadCfg.s32Taskid[E_DRV_SCLIRQ_SCTASKID_DAZA]);
    return 0;
}
bool _Create_SCLIRQ_IST(void)
{
    char pName2[] = {"SCLDAZA_THREAD"};
    u8 bRet = 0;
    gstThreadCfg.s32Taskid[E_DRV_SCLIRQ_SCTASKID_DAZA] = DrvSclOsCreateWorkQueueTask(pName2);
    gstThreadCfg.s32HandlerId[E_DRV_SCLIRQ_SCTASK_WORKQUEUECMDQ] = DrvSclOsCreateWorkQueueEvent((void*)SCLIRQ_CMDQIST);
    gstThreadCfg.s32HandlerId[E_DRV_SCLIRQ_SCTASK_WORKQUEUEDAZA] = DrvSclOsCreateWorkQueueEvent((void*)SCLIRQ_DazaIST);
    return bRet;
}
void  _DrvSclIrqisr(void)
{
    static u32 u32ActTime[7];
    static bool bAffFull = 0,bAffcount=0;
    static bool bISPFramePerfect = 0;
    static bool bNonFRMEndCount = 0;
    u64 u64Flag,u64Mask=0x0;
    u32 u32DiffTime = 0;
    u64Mask = gstScldmaInfo.u64mask;
    u64Flag = HalSclIrqGetFlag(E_DRV_SCLIRQ_SCTOP_0,0xFFFFFFFFFFFF);
    if(_bSCLIRQ_Suspend)
    {
        HalSclIrqSetClear(E_DRV_SCLIRQ_SCTOP_0,0xFFFFFFFFFFFF, 1);
    }

    HalSclIrqSetClear(E_DRV_SCLIRQ_SCTOP_0,(~u64Mask)&u64Flag, 1);
    gu32Time = ((u32)DrvSclOsGetSystemTimeStamp());
    if(u64Flag &SCLIRQ_MSK_AFF_FULL)
    {
        if(u64Flag &SCLIRQ_MSK_VSYNC_FCLK_LDC)
        {
            // HW bug : aff fifo full error report
        }
        else
        {
            if((DrvSclOsGetEvent(_s32SYNCEventId)& E_DRV_SCLIRQ_EVENT_BRESETFCLK) ==0)
            {
                gstSCInts.u32AffCount++;
            }
            _DrvSclIrqAffFullHandler(&bAffFull, &bAffcount);
        }
    }
    // next frame IN . because of receive and send in same time.(ISP in frame end)
    // 1.frame coming(vsync)
    // 2.frame receive done.(irq get sc_frm_in_end<before next vsync,maybe>)keep in DNR
    // 3.active from dnr(LDC_trig<next vsync>)
    if(u64Flag &SCLIRQ_MSK_SC_IN_FRM_END)
    {
        _DrvSclIrqFrameInEndHandler(&bNonFRMEndCount,&bISPFramePerfect,&bAffFull);
        gstSCInts.u32ISPDoneCount++;
        u32DiffTime = gu32Time - u32ActTime[5];
        gstSCInts.u32ISPTime += u32DiffTime;
        if(gstSCInts.u32ISPDoneCount > 256)
        {
            gstSCInts.u32ISPTime = gstSCInts.u32ISPTime/gstSCInts.u32ISPDoneCount;
            gstSCInts.u32ISPInCount = (gstSCInts.u32ISPInCount-gstSCInts.u32ISPDoneCount)+1;
            gstSCInts.u32ISPDoneCount = 1;
        }
        //_DrvSclIrqFrameInEndRealTimeHandler();
        //SCL_ERR("[FRMIN]@%lu\n",((u32)DrvSclOsGetSystemTimeStamp()));
        SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&(EN_DBGMG_SCLIRQLEVEL_ELSE),"[FRMIN]@%lu\n",((u32)DrvSclOsGetSystemTimeStamp()));
    }
    if(u64Flag &SCLIRQ_MSK_VSYNC_SC1_HVSP_FINISH)
    {
        //sclprintf("HVSP:%lu,%ld\n",gu32Time,count);
    }
    if(u64Flag &SCLIRQ_MSK_VSYNC_FCLK_LDC)
    {
        u32ActTime[5] = gu32Time;
        gstSCInts.u32ISPInCount++;
        if(DrvSclOsGetEvent(DrvSclIrqGetIrqSYNCEventID())&(E_DRV_SCLIRQ_EVENT_FRMENDSYNC))
        {
            u32DiffTime = gu32Time - u32ActTime[6];
            if(!(u64Flag &SCLIRQ_MSK_SC1_ENG_FRM_END))
            {
                _DrvSclIrqSetISPBlankingTime(u32DiffTime);
            }
            gstSCInts.u32ISPBlanking += u32DiffTime;
        }
        _DrvSclIrqLDCVsyncInRealTimeHandler();
        _DrvSclIrqLDCVsyncInHandler(&bISPFramePerfect,&bAffFull);
        SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&(EN_DBGMG_SCLIRQLEVEL_ELSE),"[FRMLDC]Flag:%llx @%lu\n",u64Flag,((u32)DrvSclOsGetSystemTimeStamp()));
    }
    // SC1 FRM
    if(u64Flag & SCLIRQ_MSK_SC1_FRM_W_ACTIVE)
    {
        u32ActTime[0] = gu32Time;
		//_DrvSclIrqDmaActiveRealTimeHandler(E_DRV_SCLDMA_1_FRM_W);
        _DrvSclIrqDmaActiveHandler(E_DRV_SCLDMA_1_FRM_W);
    }

    if(u64Flag & SCLIRQ_MSK_SC1_FRM_W_ACTIVE_N)
    {
	    //_DrvSclIrqDmaActiveNRealTimeHandler(E_DRV_SCLDMA_1_FRM_W);
        if(_DrvSclIrqDmaActiveNHandler(E_DRV_SCLDMA_1_FRM_W))
        {
            gsclirqstate |= (E_DRV_SCLIRQ_EVENT_SC1FRM|E_DRV_SCLIRQ_EVENT_ISTSC1FRM);
        }
        gstSCInts.u32SC1FrmDoneCount++;
        u32DiffTime = gu32Time - u32ActTime[0];
        gstSCInts.u32SC1FrmActiveTime += u32DiffTime;
        if(gstSCInts.u32SC1FrmDoneCount > 256)
        {
            gstSCInts.u32SC1FrmActiveTime = gstSCInts.u32SC1FrmActiveTime/gstSCInts.u32SC1FrmDoneCount;
            gstSCInts.u32SC1FrmDoneCount = 1;
        }
    }


    // SC2 FRM
    if(u64Flag & SCLIRQ_MSK_SC2_FRM_W_ACTIVE)
    {
        u32ActTime[1] = gu32Time;
		//_DrvSclIrqDmaActiveRealTimeHandler(E_DRV_SCLDMA_2_FRM_W);
        _DrvSclIrqDmaActiveHandler(E_DRV_SCLDMA_2_FRM_W);
    }

    if(u64Flag & SCLIRQ_MSK_SC2_FRM_W_ACTIVE_N)
    {
	    //_DrvSclIrqDmaActiveNRealTimeHandler(E_DRV_SCLDMA_2_FRM_W);
        if(_DrvSclIrqDmaActiveNHandler(E_DRV_SCLDMA_2_FRM_W))
        {
            gsclirqstate |= (E_DRV_SCLIRQ_EVENT_SC2FRM|E_DRV_SCLIRQ_EVENT_ISTSC2FRM);
        }
        gstSCInts.u32SC2FrmDoneCount++;
        u32DiffTime = gu32Time -u32ActTime[1];
        gstSCInts.u32SC2FrmActiveTime += u32DiffTime;
        if(gstSCInts.u32SC2FrmDoneCount > 256)
        {
            gstSCInts.u32SC2FrmActiveTime = gstSCInts.u32SC2FrmActiveTime/gstSCInts.u32SC2FrmDoneCount;
            gstSCInts.u32SC2FrmDoneCount = 1;
        }
    }
    // SC2 FRM2
    if(u64Flag & SCLIRQ_MSK_SC2_FRM2_W_ACTIVE)
    {
        u32ActTime[2] = gu32Time;
		//_DrvSclIrqDmaActiveRealTimeHandler(E_DRV_SCLDMA_2_FRM2_W);
        _DrvSclIrqDmaActiveHandler(E_DRV_SCLDMA_2_FRM2_W);
    }
    if(u64Flag & SCLIRQ_MSK_SC2_FRM2_W_ACTIVE_N)
    {
	    //_DrvSclIrqDmaActiveNRealTimeHandler(E_DRV_SCLDMA_2_FRM2_W);
        if(_DrvSclIrqDmaActiveNHandler(E_DRV_SCLDMA_2_FRM2_W))
        {
            DrvSclOsSetEvent(_s32FRMENDEventId, (E_DRV_SCLIRQ_EVENT_SC2FRM2|E_DRV_SCLIRQ_EVENT_ISTSC2FRM2));
        }
        else
        {
            if(gbsc2frame2reopen && (u64Flag & SCLIRQ_MSK_SC2_FRM2_W_ACTIVE))
            {
                gbsc2frame2reopen = 0;
            }
        }
        // for HW bug
        //gsclirqstate |= (E_DRV_SCLIRQ_EVENT_SC2FRM2|E_DRV_SCLIRQ_EVENT_ISTSC2FRM2);
        if(gbsc2frame2reopen)
        {
            gbsc2frame2reopen = 0;
            SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&(EN_DBGMG_SCLIRQLEVEL_ELSE),
                "[FRMEND]SC2 FRM2 reopen\n");
            _DrvSclIrqSetDmaOnOffWithoutDoubleBufferHandler();
            if((!gbDMADoneEarlyISP) || (gbPtgenMode))
            {
                _DrvSclIrqCmdqBlankingHandler();
            }
        }
        gstSCInts.u32SC2Frm2DoneCount++;
        u32DiffTime = gu32Time -u32ActTime[2];
        gstSCInts.u32SC2Frm2ActiveTime += u32DiffTime;
        if(gstSCInts.u32SC2Frm2DoneCount > 256)
        {
            gstSCInts.u32SC2Frm2ActiveTime = gstSCInts.u32SC2Frm2ActiveTime/gstSCInts.u32SC2Frm2DoneCount;
            gstSCInts.u32SC2Frm2DoneCount = 1;
        }
    }

    // SC1 SNP
    if(u64Flag & SCLIRQ_MSK_SC1_SNP_W_ACTIVE)
    {
        u32ActTime[3] = gu32Time;
		//_DrvSclIrqDmaActiveRealTimeHandler(E_DRV_SCLDMA_1_SNP_W);
        _DrvSclIrqDmaActiveHandler(E_DRV_SCLDMA_1_SNP_W);
    }

    if(u64Flag & SCLIRQ_MSK_SC1_SNP_W_ACTIVE_N)
    {
	    //_DrvSclIrqDmaActiveNRealTimeHandler(E_DRV_SCLDMA_1_SNP_W);
        if(_DrvSclIrqDmaActiveNHandler(E_DRV_SCLDMA_1_SNP_W))
        {
            gsclirqstate |= (E_DRV_SCLIRQ_EVENT_SC1SNP|E_DRV_SCLIRQ_EVENT_ISTSC1SNP);
        }
        gstSCInts.u32SC1SnpDoneCount++;
        u32DiffTime = gu32Time -u32ActTime[3];
        gstSCInts.u32SC1SnpActiveTime += u32DiffTime;
        if(gstSCInts.u32SC1SnpDoneCount > 256)
        {
            gstSCInts.u32SC1SnpActiveTime = gstSCInts.u32SC1SnpActiveTime/gstSCInts.u32SC1SnpDoneCount;
            gstSCInts.u32SC1SnpDoneCount = 1;
        }
    }
    // SC SRC FRMEND
    if(u64Flag &SCLIRQ_MSK_SC1_ENG_FRM_END)
    {
        _DrvSclIrqSetRealTimeFrmEndHandler(gu32Time);
        _DrvSclIrqSetFrmEndInterruptStatus(gu32Time);
        _DrvSclIrqIsAffFullContinually(&bAffFull, &bAffcount);
        _DrvSclIrqIsDMAHangUp(u64Flag);
        bNonFRMEndCount = 0;
        u32ActTime[6] = gu32Time;
        gu32FrmEndTime = gu32Time;
        u32DiffTime = gu32Time -u32ActTime[5];
        gstSCInts.u32SCLMainActiveTime += u32DiffTime;
        if(gstSCInts.u32ISPBlanking)
        {
            gstSCInts.u32SCLMainDoneCount++;
        }
        if(gstSCInts.u32SCLMainDoneCount > 256)
        {
            gstSCInts.u32ISPBlanking = gstSCInts.u32ISPBlankingTime;
            gstSCInts.u32SCLMainActiveTime = gstSCInts.u32SCLMainActiveTime/gstSCInts.u32SCLMainDoneCount;
            gstSCInts.u32SCLMainDoneCount = 1;
            gstSCInts.u8CountReset ++;
        }
        if(gbPtgenMode)
        {
            gstSCInts.u32ISPDoneCount++;
            if(gstSCInts.u32ISPDoneCount > 256)
            {
                gstSCInts.u32ISPTime = gstSCInts.u32ISPTime/gstSCInts.u32ISPDoneCount;
                gstSCInts.u32ISPInCount = (gstSCInts.u32ISPInCount-gstSCInts.u32ISPDoneCount)+1;
                gstSCInts.u32ISPDoneCount = 1;
            }
        }
        SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&(EN_DBGMG_SCLIRQLEVEL_ELSE),
            "[FRMEND]%x flag:%llx,%lu\n",gsclirqstate,u64Flag,((u32)DrvSclOsGetSystemTimeStamp()));
    }

    // SC3 FRM_W
    if(u64Flag & SCLIRQ_MSK_SC3_DMA_W_ACTIVE)
    {
        gscl3irqstate = E_DRV_SCLIRQ_SC3EVENT_CLEAR;
        gscl3irqstate |= (E_DRV_SCLIRQ_SC3EVENT_ACTIVE | E_DRV_SCLIRQ_SC3EVENT_ISTACTIVE);
        _DrvSclIrqDmaActiveHandler(E_DRV_SCLDMA_3_FRM_W);
        u32ActTime[4] = gu32Time;
    }

    if(u64Flag & SCLIRQ_MSK_SC3_DMA_W_ACTIVE_N)
    {
        gscl3irqstate |= (E_DRV_SCLIRQ_SC3EVENT_ACTIVEN | E_DRV_SCLIRQ_SC3EVENT_ISTACTIVEN | E_DRV_SCLIRQ_SC3EVENT_IRQ);
        _DrvSclIrqDmaActiveNHandler(E_DRV_SCLDMA_3_FRM_W);
        gstSCInts.u32SC3DoneCount++;
        u32DiffTime = gu32Time -u32ActTime[4];
        gstSCInts.u32SC3ActiveTime += u32DiffTime;
        if(gstSCInts.u32SC3DoneCount > 256)
        {
            gstSCInts.u32SC3ActiveTime = gstSCInts.u32SC3ActiveTime/gstSCInts.u32SC3DoneCount;
            gstSCInts.u32SC3DoneCount = 1;
        }
    }
    HalSclIrqSetClear(E_DRV_SCLIRQ_SCTOP_0,(~u64Mask)&u64Flag, 0);
    u32DiffTime = ((u32)DrvSclOsGetSystemTimeStamp())-gu32Time;
    SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&(EN_DBGMG_SCLIRQLEVEL_NORMAL),
        "[ISR]Diff:%lu(ns)\n",u32DiffTime);
}
#endif //ENABLE_ACTIVEID_ISR


void DrvSclIrqSetDnrLock(bool bEn)
{
    HalSclIrqSetDNRLock(bEn);
}
void DrvSclIrqSetDropFrameFromCmdqDone(u8 u8Count)
{
    u32 u32Events;
    DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(), E_DRV_SCLIRQ_EVENT_CMDQDONE, &u32Events, E_DRV_SCLOS_EVENT_MD_OR, 100); // get status: CMDQ Done
    DRV_SCLIRQ_MUTEX_LOCK_ISR();
    if(gstDropFrameCount<=u8Count)
    {
        gstDropFrameCount = u8Count;
    }
    DRV_SCLIRQ_MUTEX_UNLOCK_ISR();
}
void DrvSclIrqSetDropFrameFromIsp(u8 u8Count)
{
    SCL_ERR("[SCLIRQ]ISP drop Frame Count:%hhd \n",u8Count);
    DRV_SCLIRQ_MUTEX_LOCK_ISR();
    if(gstDropFrameCount<=u8Count)
    {
        gstDropFrameCount = u8Count;
    }
    DRV_SCLIRQ_MUTEX_UNLOCK_ISR();
}
void DrvSclIrqSetDazaQueueWorkISR(DrvSclIrqDazaEventType_e enEvent)
{
    if(!(genDAZAEvent&enEvent))
    {
        DRV_SCLIRQ_MUTEX_LOCK_ISR();
        genDAZAEvent |= enEvent;
        DRV_SCLIRQ_MUTEX_UNLOCK_ISR();
        DrvSclOsQueueWork(1,gstThreadCfg.s32Taskid[E_DRV_SCLIRQ_SCTASKID_DAZA],
            gstThreadCfg.s32HandlerId[E_DRV_SCLIRQ_SCTASK_WORKQUEUEDAZA],0);
    }
}
void DrvSclIrqSetDazaQueueWork(DrvSclIrqDazaEventType_e enEvent)
{
    if(!(genDAZAEvent&enEvent))
    {
        genDAZAEvent |= enEvent;
        DrvSclOsQueueWork(1,gstThreadCfg.s32Taskid[E_DRV_SCLIRQ_SCTASKID_DAZA],
            gstThreadCfg.s32HandlerId[E_DRV_SCLIRQ_SCTASK_WORKQUEUEDAZA],0);
    }
}
bool DrvSclIrqInitVariable(void)
{
    u64 u64Flag;
#if (ENABLE_ISR)
    DrvSclOsMemset(&gstSCLIrqInfo, 0, sizeof(ST_SCLIRQ_IFNO_TYPE));
#endif
    gsclirqstate        = 0;
    gscl3irqstate       = 0;
    _bSCLIRQ_Suspend    = 0;
    gbFRMInFlag         = 0;
    gbPtgenMode         = 0;
    gbDMADoneEarlyISP   = 0;
    genDAZAEvent        = 0;
    gbMonitorCropMode   = 0;
    gbEachDMAEnable     = 0;
    gbBypassDNR = 0;
    gstDropFrameCount = 0;
    gbsc2frame2reopen = 0;
    gu32FrmEndTime = 0;
    //DrvSclOsMemset(&gstThreadCfg,0x0,sizeof(DrvSclDmaThreadConfig_t));
    DrvSclOsMemset(&gstSCInts,0x0,sizeof(DrvSclIrqScIntsType_t));
    _DrvSclIrqSetIsBlankingRegion(1);

    DrvSclOsClearEventIRQ(_s32SYNCEventId,0xFFFFFFFF);
    DrvSclOsClearEventIRQ(_s32FRMENDEventId,0xFFFFFFFF);
    DrvSclOsClearEventIRQ(_s32SC3EventId,0xFFFFFFFF);
    u64Flag = HalSclIrqGetFlag(E_DRV_SCLIRQ_SCTOP_0,0xFFFFFFFFFF);
    HalSclIrqSetClear(E_DRV_SCLIRQ_SCTOP_0,u64Flag, 1);
    HalSclIrqSetClear(E_DRV_SCLIRQ_SCTOP_0,u64Flag, 0);
    return TRUE;
}
void DrvSclIrqSetPTGenStatus(bool bPTGen)
{
    DRV_SCLIRQ_MUTEX_LOCK();
    gbPtgenMode  = bPTGen;
    if(bPTGen)
    {
        gbFRMInFlag = 0xF;
    }
    DRV_SCLIRQ_MUTEX_UNLOCK();
}
void DrvSclIrqSetDMADoneEarlyISP(bool bDMADoneEarlyISP)
{
    // LDC patch open,crop open,
    gbDMADoneEarlyISP  = bDMADoneEarlyISP;
}
void DrvSclIrqSetCheckcropflag(bool bcheck)
{
    DRV_SCLIRQ_MUTEX_LOCK();
    gbMonitorCropMode  = bcheck;
    DRV_SCLIRQ_MUTEX_UNLOCK();
}
u8 DrvSclIrqGetCheckcropflag(void)
{
    return gbMonitorCropMode;
}
u8 DrvSclIrqGetEachDMAEn(void)
{
    return gbEachDMAEnable;
}
u8 DrvSclIrqGetIspIn(void)
{
    return ((DrvSclOsGetEvent(_s32SYNCEventId)&E_DRV_SCLIRQ_EVENT_LDCSYNC)? 1 : 0);
}
DrvSclIrqScIntsType_t DrvSclIrqGetSclInts(void)
{
    return gstSCInts;
}

bool DrvSclIrqInit(DrvSclIrqInitConfig_t *pCfg)
{
    char mutx_word[] = {"_SCLIRQ_Mutex"};

    u8 i;

    if(_SCLIRQ_Mutex != -1)
    {
        SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&EN_DBGMG_SCLIRQLEVEL_NORMAL, "[DRVSCLIRQ]%s(%d)::Already Done\n", __FUNCTION__, __LINE__);
        return TRUE;
    }

    if(DrvSclOsInit() == FALSE)
    {
        DRV_SCLIRQ_ERR(sclprintf("[DRVSCLIRQ]%s(%d)::DrvSclOsInit Fail\n", __FUNCTION__, __LINE__));
        return FALSE;
    }

    _SCLIRQ_Mutex = DrvSclOsCreateMutex(E_DRV_SCLOS_FIFO, mutx_word, SCLOS_PROCESS_SHARED);
    _pSCLIRQ_SCLDMA_Mutex = DrvSclDmaGetDmaandIRQCommonMutex();
    if (_SCLIRQ_Mutex == -1)
    {
        DRV_SCLIRQ_ERR(sclprintf("[DRVSCLIRQ]%s(%d)::Create Mutex Flag Fail\n", __FUNCTION__, __LINE__));
        return FALSE;
    }
    HalSclIrqSetUtility();
    //HalSclIrqSetRiuBase(pCfg->u32RiuBase);

    // disable all interrupt first
    for(i=0; i<SCLIRQ_NUM; i++)
    {
        HalSclIrqSetMask(E_DRV_SCLIRQ_SCTOP_0, ((u64)1)<<i, 1);
        HalSclIrqSetClear(E_DRV_SCLIRQ_SCTOP_0, ((u64)1)<<i, 1);
        HalSclIrqSetMask(E_DRV_SCLIRQ_SCTOP_1, ((u64)1)<<i, 1);
        HalSclIrqSetClear(E_DRV_SCLIRQ_SCTOP_1, ((u64)1)<<i, 1);
        HalSclIrqSetMask(E_DRV_SCLIRQ_SCTOP_2, ((u64)1)<<i, 1);
        HalSclIrqSetClear(E_DRV_SCLIRQ_SCTOP_2, ((u64)1)<<i, 1);
    }


    DrvSclIrqInitVariable();
    DrvSclOsMemset(&gstThreadCfg,0x0,sizeof(DrvSclDmaThreadConfig_t));

#if (ENABLE_ISR|ENABLE_ACTIVEID_ISR)
    _s32FRMENDEventId   = DrvSclOsCreateEventGroup("FRMEND_Event");
    _s32SYNCEventId     = DrvSclOsCreateEventGroup("SYNC_Event");
    _s32SC3EventId      = DrvSclOsCreateEventGroup("SC3_Event");
    gu32IRQNUM      = pCfg->u32IRQNUM;
    if(DrvSclOsAttachInterrupt(pCfg->u32IRQNUM, (InterruptCb)_DrvSclIrqisr ,IRQF_DISABLED, "SCLINTR"))
    {
        DRV_SCLIRQ_ERR(sclprintf("[DRVSCLIRQ]%s(%d):: Request IRQ Fail\n", __FUNCTION__, __LINE__));
        return FALSE;
    }
    DrvSclOsDisableInterrupt(pCfg->u32IRQNUM);
    DrvSclOsEnableInterrupt(pCfg->u32IRQNUM);
    _Create_SCLIRQ_IST();
#endif
#if (ENABLE_CMDQ_ISR)
    DrvSclCmdqInitRIUBase(pCfg->u32RiuBase);
    if(DrvSclOsAttachInterrupt(pCfg->u32CMDQIRQNUM, (InterruptCb)_DrvSclIrqCmdqIsr , 0, "CMDQINTR"))
    {
        DRV_SCLIRQ_ERR(sclprintf("[DRVSCLIRQ]%s(%d):: Request IRQ Fail\n", __FUNCTION__, __LINE__));
        return FALSE;
    }
    gu32CMDQIRQNUM      = pCfg->u32CMDQIRQNUM;
    DrvSclOsDisableInterrupt(pCfg->u32CMDQIRQNUM);
    DrvSclOsEnableInterrupt(pCfg->u32CMDQIRQNUM);
    DrvSclCmdqSetISRStatus(1);
#endif


    return TRUE;
}
void * DrvSclIrqGetWaitQueueHead(u32 enID)
{
    if(enID == E_DRV_SCLDMA_ID_3_W)
    {
        return DrvSclOsGetEventQueue(&_s32SC3EventId);
    }
    else
    {
        return DrvSclOsGetEventQueue(&_s32FRMENDEventId);
    }
    return 0;
}

void * DrvSclIrqGetSyncQueue(void)
{
    return DrvSclOsGetEventQueue(&_s32SYNCEventId);
}

s32 DrvSclIrqGetIrqEventID(void)
{
    return _s32FRMENDEventId;
}
s32 DrvSclIrqGetIrqSYNCEventID(void)
{
    return _s32SYNCEventId;
}
s32 DrvSclIrqGetIrqSC3EventID(void)
{
    return _s32SC3EventId;
}

bool DrvSclIrqSuspend(DrvSclIrqSuspendResumeConfig_t *pCfg)
{
    SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&EN_DBGMG_SCLIRQLEVEL_NORMAL, "%s(%d), bSuspend=%d\n", __FUNCTION__, __LINE__, _bSCLIRQ_Suspend);
    _bSCLIRQ_Suspend = 1;
    _DrvSclIrqSetIsBlankingRegion(1);
    return TRUE;
}

bool DrvSclIrqResume(DrvSclIrqSuspendResumeConfig_t *pCfg)
{
    u16 i;
    SCL_DBG(SCL_DBG_LV_DRVSCLIRQ()&EN_DBGMG_SCLIRQLEVEL_NORMAL, "%s(%d), bResume=%d\n", __FUNCTION__, __LINE__, _bSCLIRQ_Suspend);

    if(_bSCLIRQ_Suspend == 1)
    {
        // disable all interrupt first
        for(i=0; i<SCLIRQ_NUM; i++)
        {
            HalSclIrqSetMask(E_DRV_SCLIRQ_SCTOP_0,((u64)1)<<i, 1);
            HalSclIrqSetClear(E_DRV_SCLIRQ_SCTOP_0,((u64)1)<<i, 1);
            HalSclIrqSetMask(E_DRV_SCLIRQ_SCTOP_1,((u64)1)<<i, 1);
            HalSclIrqSetClear(E_DRV_SCLIRQ_SCTOP_1,((u64)1)<<i, 1);
            HalSclIrqSetMask(E_DRV_SCLIRQ_SCTOP_2,((u64)1)<<i, 1);
            HalSclIrqSetClear(E_DRV_SCLIRQ_SCTOP_2,((u64)1)<<i, 1);
        }
        _bSCLIRQ_Suspend = 0;
    }

    return TRUE;
}

bool DrvSclIrqExit(void)
{
    bool bRet = TRUE;
    u64 u64IRQ;
    if(_SCLIRQ_Mutex != -1)
    {
        DrvSclOsDeleteMutex(_SCLIRQ_Mutex);
        _SCLIRQ_Mutex = -1;
    }
    else
    {
        return FALSE;
    }
    DrvSclIrqSetDnrLock(0);

#if (ENABLE_ISR)

    DRV_SCLIRQ_MUTEX_LOCK();
    u64IRQ = gstSCLIrqInfo.u64IRQ;
    DRV_SCLIRQ_MUTEX_UNLOCK();

    if( u64IRQ == 0)
    {
        DrvSclOsDisableInterrupt(gu32IRQNUM);
        DrvSclOsDetachInterrupt(gu32IRQNUM);
        bRet = TRUE;
    }
    else
    {
        DRV_SCLIRQ_ERR(sclprintf("[DRVSCLIRQ]: IRQ no clear %llx\n", gstSCLIrqInfo.u64IRQ));
        bRet = FALSE;
    }
#elif ENABLE_ACTIVEID_ISR
    u64IRQ = HalSclIrqGetFlag(E_DRV_SCLIRQ_SCTOP_0,0xFFFFFFFFFF);
    HalSclIrqSetClear(E_DRV_SCLIRQ_SCTOP_0,u64IRQ, 1);
    HalSclIrqSetClear(E_DRV_SCLIRQ_SCTOP_0,u64IRQ, 0);
    _Delete_SCLIRQ_IST();
    DrvSclOsDisableInterrupt(gu32IRQNUM);
    DrvSclOsDetachInterrupt(gu32IRQNUM);
#endif
#if (ENABLE_CMDQ_ISR)
    DrvSclOsDisableInterrupt(gu32CMDQIRQNUM);
    DrvSclOsDetachInterrupt(gu32CMDQIRQNUM);
    DrvSclCmdqSetISRStatus(0);
#endif
    DrvSclOsExit();
    return bRet;
}

bool DrvSclIrqInterruptEnable(u16 u16IRQ)
{
    bool bRet = TRUE;
    if(u16IRQ < 48)
    {
#if (ENABLE_ISR)
        DrvSclOsObtainMutex(_SCLIRQ_Mutex , SCLOS_WAIT_FOREVER);
        gstSCLIrqInfo.u64IRQ |= ((u64)1<<u16IRQ);
        DRV_SCLIRQ_MUTEX_UNLOCK();
#endif
        HalSclIrqSetMask(E_DRV_SCLIRQ_SCTOP_0,((u64)1<<u16IRQ), 0);
        HalSclIrqSetClear(E_DRV_SCLIRQ_SCTOP_0,((u64)1<<u16IRQ), 0);
        bRet = TRUE;
    }
    else
    {
        bRet = FALSE;
    }

    return bRet;
}

bool DrvSclIrqDisable(u16 u16IRQ)
{
    bool bRet = TRUE;

    if(u16IRQ < 48)
    {
#if (ENABLE_ISR)

        DRV_SCLIRQ_MUTEX_LOCK();
        gstSCLIrqInfo.u64IRQ &= ~((u64)1<<u16IRQ);
        DRV_SCLIRQ_MUTEX_UNLOCK();
#endif
        HalSclIrqSetMask(E_DRV_SCLIRQ_SCTOP_0,((u64)1<<u16IRQ), 1);
        HalSclIrqSetClear(E_DRV_SCLIRQ_SCTOP_0,((u64)1<<u16IRQ), 1);
        bRet = TRUE;
    }
    else
    {
        bRet = FALSE;
    }
    return bRet;
}



bool DrvSclIrqGetFlag(u16 u16IRQ, u64 *pFlag)
{
    bool bRet = TRUE;
    if(u16IRQ < 48)
    {
#if (ENABLE_ISR)

        DRV_SCLIRQ_MUTEX_LOCK();
        *pFlag = gstSCLIrqInfo.u64Flag & (((u64)1)<<u16IRQ) ? 1 : 0;
        DRV_SCLIRQ_MUTEX_UNLOCK();
#else
        *pFlag = HalSclIrqGetFlag(E_DRV_SCLIRQ_SCTOP_0,((u64)1<<u16IRQ));
#endif
        bRet = TRUE;
    }
    else
    {
        bRet = FALSE;
    }
    return bRet;
}

bool DrvSclIrqSetClear(u16 u16IRQ)
{
    bool bRet = TRUE;
    if(u16IRQ < 48)
    {
#if (ENABLE_ISR)
        DRV_SCLIRQ_MUTEX_LOCK();
        gstSCLIrqInfo.u64Flag &= ~(((u64)1)<<u16IRQ);
        DRV_SCLIRQ_MUTEX_UNLOCK();
#else
        //DRV_SCLIRQ_ERR(sclprintf("[DRVSCLIRQ]: IRQ clear %hx\n", u16IRQ));
        HalSclIrqSetClear(E_DRV_SCLIRQ_SCTOP_0,(u64)1<<u16IRQ, 1);
        HalSclIrqSetClear(E_DRV_SCLIRQ_SCTOP_0,(u64)1<<u16IRQ, 0);
#endif
    }
    else
    {
        bRet = FALSE;
    }
    return bRet;
}

bool DrvSclIrqSetMask(u64 u64IRQ)
{
    bool bRet = TRUE;
    HalSclIrqSetMask(E_DRV_SCLIRQ_SCTOP_0,u64IRQ,1);
    return bRet;
}

bool DrvSclIrqGetFlags_Msk(u64 u64IrqMsk, u64 *pFlags)
{
#if (ENABLE_ISR)

    DRV_SCLIRQ_MUTEX_LOCK();
    *pFlags = gstSCLIrqInfo.u64Flag & u64IrqMsk;
    DRV_SCLIRQ_MUTEX_UNLOCK();
#else
    *pFlags = HalSclIrqGetFlag(E_DRV_SCLIRQ_SCTOP_0,u64IrqMsk);
#endif
    return TRUE;
}
bool DrvSclIrqSetClear_Msk(u64 u64IrqMsk)
{
#if ENABLE_ISR
    DRV_SCLIRQ_MUTEX_LOCK();
    gstSCLIrqInfo.u64Flag &= ~(((u64)1)<<u64IrqMsk);
    DRV_SCLIRQ_MUTEX_UNLOCK();
#else
    //DRV_SCLIRQ_ERR(sclprintf("[DRVSCLIRQ]: IRQ clear %hx\n", u16IRQ));
    HalSclIrqSetClear(E_DRV_SCLIRQ_SCTOP_0,u64IrqMsk, 1);
    HalSclIrqSetClear(E_DRV_SCLIRQ_SCTOP_0,u64IrqMsk, 0);
#endif
    return TRUE;
}
#undef DRV_SCL_IRQ_C
