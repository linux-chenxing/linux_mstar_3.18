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
#define DRV_HVSP_C


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "drv_scl_os.h"
#include "drv_scl_dbg.h"
#include "hal_scl_util.h"

#include "drv_scl_hvsp_st.h"
#include "hal_scl_hvsp.h"
#include "drv_scl_hvsp.h"
#include "drv_scl_dma_st.h"
#include "drv_scl_irq_st.h"
#include "drv_scl_irq.h"
#include "hal_scl_reg.h"
#include "drv_scl_cmdq.h"

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define DRV_HVSP_DBG(x)
#define DRV_HVSP_ERR(x)      x
#define DRV_HVSP_MUTEX_LOCK()            DrvSclOsObtainMutex(_HVSP_Mutex,SCLOS_WAIT_FOREVER)
#define DRV_HVSP_MUTEX_UNLOCK()          DrvSclOsReleaseMutex(_HVSP_Mutex)
#define FHD_Width   1920
#define FHD_Height  1080
#define _3M_Width   2048
#define _3M_Height  1536
#define HD_Width    1280
#define HD_Height   720
#define D_Width    704
#define D_Height   576
#define PNL_Width   800
#define PNL_Height  480
#define SRAMFORSCALDOWN 0x10
#define SRAMFORSCALUP 0x21
#define SRAMFORSC2ALDOWN 0x20
#define SRAMFORSC2ALUP 0x21
#define SRAMFORSC3HDOWN 0x00
#define SRAMFORSC3HUP 0x21
#define SRAMFORSC3VDOWN 0x00
#define SRAMFORSC3VUP 0x21
#define HeightRange (gstSrcSize.u16Height ? ((gstSrcSize.u16Height+1)/20) : (FHD_Height/20))
#define Is_StartAndEndInSameByte(u16XStartoffsetbit,u16idx,u16Xoffset) \
    (u16XStartoffsetbit && (u16idx ==0) && (u16Xoffset==1))
#define Is_StartByteOffsetBit(u16XStartoffsetbit,u16idx) \
        (u16XStartoffsetbit && (u16idx ==0))
#define Is_EndByteOffsetBit(u16XEndoffsetbit,u16Xoffset,u16idx) \
    (u16XEndoffsetbit && (u16idx ==(u16Xoffset-1)))
#define Is_DNRBufferReady() (gbMemReadyForDrv)
#define Is_FrameBufferTooSmall(u32ReqMemSize,u32IPMMemSize) (u32ReqMemSize > u32IPMMemSize)
#define Is_PreCropWidthNeedToOpen() (gstSrcSize.u16Width > gstIPMCfg.u16Fetch)
#define Is_PreCropHeightNeedToOpen() (gstSrcSize.u16Height> gstIPMCfg.u16Vsize)
#define Is_InputSrcRotate() (gstSrcSize.u16Height > gstSrcSize.u16Width)
#define Is_IPM_NotSetReady() (gstIPMCfg.u16Fetch == 0 || gstIPMCfg.u16Vsize == 0)
#define Is_INPUTMUX_SetReady() (gstSrcSize.bSet == 1)
#define Is_PreCropNotNeedToOpen() ((gstIPMCfg.u16Fetch == gstSrcSize.u16Width) && \
    (gstIPMCfg.u16Vsize == gstSrcSize.u16Height))
#define HVSP_ID_SRAM_V_OFFSET 2
#define HVSP_ID_SRAM_H_OFFSET 15
#define HVSP_RATIO(input, output)           ((u32)((u64)((u32)(input) * 1048576) / (u32)(output)))
#define HVSP_CROP_RATIO(u16src, u16crop1, u16crop2)  ((u16)(((u32)u16crop2 * (u32)u16crop1) / (u32)u16src ))
#define HVSP_CROP_CHECK(u16croph,u16cropch,u16cropv,u16cropcv)  (((u16croph) != (u16cropch))|| ((u16cropv) != (u16cropcv)))
#define HVSP_DMA_CHECK(u16cropv,u16cropcv)  (((u16cropv) != (u16cropcv)))
#define _CHANGE_SRAM_V_QMAP(enHVSP_ID,up)         (((gstScalinInfo[enHVSP_ID].stSizeAfterCrop.u16Height\
    > gstScalinInfo[enHVSP_ID].stSizeAfterScaling.u16Height)&&(up)) || (((gstScalinInfo[enHVSP_ID].stSizeAfterCrop.u16Height\
    < gstScalinInfo[enHVSP_ID].stSizeAfterScaling.u16Height) && !(up))))
#define _CHANGE_SRAM_H_QMAP(enHVSP_ID,up)         (((gstScalinInfo[enHVSP_ID].stSizeAfterCrop.u16Width\
        > gstScalinInfo[enHVSP_ID].stSizeAfterScaling.u16Width)&&(up)) || (((gstScalinInfo[enHVSP_ID].stSizeAfterCrop.u16Width\
        < gstScalinInfo[enHVSP_ID].stSizeAfterScaling.u16Width) && !(up))))

#define _IsSingleBufferAndZoomSizeBiggerthanLimitation(u8buffernum,cropV,DspV) (((u8buffernum)==1) && \
    ((DspV) > (cropV)))
#define _IsSingleBufferAndNOLDCZoomSizeBiggerthanLimitation(bnoLDC,u8buffernum,cropV,DspV) ((bnoLDC)&&((u8buffernum)==1) && \
    ((DspV) > (cropV)))
#define _Is_Src5MSize() ((gstSrcSize.u16Height*gstSrcSize.u16Width) >= 4500000 &&(gstSrcSize.bSet))
#define _IsChangeFBBufferResolution(u16Width ,u16Height) ((u16Width !=gstIPMCfg.u16Fetch)||(u16Height !=gstIPMCfg.u16Vsize))
#define _IsChangePreCropPosition(u16X ,u16Y ,u16oriX ,u16oriY ) (((u16X !=u16oriX)||(u16Y !=u16oriY)))
#define _IsZoomOut(u16Width ,u16Height) ((u16Width < gstIPMCfg.u16Fetch)||(u16Height < gstIPMCfg.u16Vsize))

//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
DrvSclHvspScalingInfo_t gstScalinInfo[E_DRV_SCLHVSP_ID_MAX];
DrvSclHvspIpmConfig_t   gstIPMCfg = {0};
ST_HVSP_SIZE_CONFIG gstSrcSize;
u8 gbPriMaskPending;
u8 *gpu8PriMaskBuf;
/////////////////
/// gbMemReadyForDrv
/// if True ,DNR buffer was allocated.
////////////////
bool gbMemReadyForDrv;
/////////////////
/// bLDCEn
/// if True ,LDC is open.(To sync VIP and HVSP driver)
////////////////
bool gbLDCEn=0;
u8 gu8FBBufferNum ;
/////////////////
/// gbHvspSuspend
/// To Save suspend status.
////////////////
bool gbHvspSuspend = 0;
/////////////////
/// bclkforcemode
/// use in Drvscldma and Drvhvsp
/// if True ,can't dynamic set Clk
////////////////
u8 gbclkforcemode = 0;//extern
s32 _HVSP_Mutex = -1;
static bool gbVScalingup[E_DRV_SCLHVSP_ID_MAX] ={1,0,0};
static bool gbHScalingup[E_DRV_SCLHVSP_ID_MAX] ={1,0,0};
bool gbSc3FirstHSet = 1;
bool gbSc3FirstVSet = 1;

//-------------------------------------------------------------------------------------------------
//  Private Functions
//-------------------------------------------------------------------------------------------------
void _DrvSclHvspInitSWVarialbe(DrvSclHvspIdType_e HVSP_IP)
{
    DrvSclOsMemset(&gstScalinInfo[HVSP_IP], 0, sizeof(DrvSclHvspScalingInfo_t));
    gbVScalingup[HVSP_IP] = 0;
    gbHScalingup[HVSP_IP] = 0;
    gbSc3FirstHSet = 1;
    gbSc3FirstVSet = 1;
    if(HVSP_IP == E_DRV_SCLHVSP_ID_1)
    {
        DrvSclOsMemset(&gstIPMCfg, 0, sizeof(DrvSclHvspIpmConfig_t));
        gbLDCEn = 0;
        gbPriMaskPending = 0;
        gbVScalingup[HVSP_IP] = SRAMFORSCALUP;
        gbHScalingup[HVSP_IP] = SRAMFORSCALUP;
        gstSrcSize.u16Height    = FHD_Height;
        gstSrcSize.u16Width     = FHD_Width;
        gstSrcSize.bSet         = 0;
        HalSclHvspSetInputSrcSize(gstSrcSize);
    }
    else if(HVSP_IP == E_DRV_SCLHVSP_ID_2)
    {
        gbVScalingup[HVSP_IP] = SRAMFORSC2ALDOWN;
        gbHScalingup[HVSP_IP] = SRAMFORSC2ALDOWN;
    }
    else if(HVSP_IP == E_DRV_SCLHVSP_ID_3)
    {

        gbVScalingup[HVSP_IP] = SRAMFORSC3VDOWN;
        gbHScalingup[HVSP_IP] = SRAMFORSC3HDOWN;
    }

}

HalSclHvspCropInfo_t _Drv_HVSP_FillPreCropInfo(void)
{
    HalSclHvspCropInfo_t stCropInfo;
    if(Is_IPM_NotSetReady() || Is_PreCropNotNeedToOpen())
    {
        if(Is_IPM_NotSetReady())
        {
            DRV_HVSP_ERR(sclprintf("[DRVHVSP]%s %d:: IPM without setting\n", __FUNCTION__, __LINE__));
        }
        stCropInfo.bEn = 0;
        stCropInfo.u16Vst = 0;
        stCropInfo.u16Hst = 0;
        stCropInfo.u16Hsize = 0;
        stCropInfo.u16Vsize = 0;
    }
    else
    {
        stCropInfo.bEn = 1;
        if(Is_InputSrcRotate())//rotate
        {
            stCropInfo.u16Hst = 0;
            stCropInfo.u16Vst = 0;
        }
        else if(Is_PreCropWidthNeedToOpen() || Is_PreCropHeightNeedToOpen())
        {
            if(Is_PreCropWidthNeedToOpen())
            {
                stCropInfo.u16Hst      = (gstSrcSize.u16Width - gstIPMCfg.u16Fetch)/2;
            }
            if(Is_PreCropHeightNeedToOpen())
            {
                stCropInfo.u16Vst      = (gstSrcSize.u16Height - gstIPMCfg.u16Vsize)/2;
            }
        }
        else
        {
            stCropInfo.u16Hst = 0;
            stCropInfo.u16Vst = 0;
        }
        stCropInfo.u16Hsize    = gstIPMCfg.u16Fetch;
        stCropInfo.u16Vsize    = gstIPMCfg.u16Vsize;
    }
    // crop1
    stCropInfo.u16In_hsize = gstSrcSize.u16Width;
    stCropInfo.u16In_vsize = gstSrcSize.u16Height;
    if(stCropInfo.u16In_hsize == 0)
    {
        stCropInfo.u16In_hsize = FHD_Width;
    }
    if(stCropInfo.u16In_vsize == 0)
    {
        stCropInfo.u16In_vsize = FHD_Height;
    }
    return stCropInfo;
}
void _Drv_HVSP_SetCoringThrdOn(DrvSclHvspIdType_e enHVSP_ID)
{
    HalSclHvspSetHspCoringThrdC(enHVSP_ID,0x1);
    HalSclHvspSetHspCoringThrdY(enHVSP_ID,0x1);
    HalSclHvspSetVspCoringThrdC(enHVSP_ID,0x1);
    HalSclHvspSetVspCoringThrdY(enHVSP_ID,0x1);
}


//-------------------------------------------------------------------------------------------------
//  Public Functions
//-------------------------------------------------------------------------------------------------
bool DrvSclHvspSetCmdqTrigType(DrvSclHvspCmdTrigConfig_t stCmdTrigCfg)
{
    HalSclHvspSetCmdqTrigCfg(stCmdTrigCfg);
    HalSclHvspSetCmdqTrigFrameCnt(0);
    return TRUE;
}

bool Drv_HVSP_GetCMDQTrigType(DrvSclHvspCmdTrigConfig_t *pCfg)
{
    HalSclHvspGetCmdqTrigCfg(pCfg);
    return TRUE;
}

bool DrvSclHvspGetCmdqDoneStatus(DrvSclHvspPollIdType_e enPollId)
{
    u32 u32Events = 0;
    static bool sbDone = 0;
    bool bRet = 0;

    if(((sbDone&(0x1<<E_DRV_SCLHVSP_POLL_ID_1)) &&enPollId ==E_DRV_SCLHVSP_POLL_ID_2)||
        ((sbDone&(0x1<<E_DRV_SCLHVSP_POLL_ID_2)) &&enPollId ==E_DRV_SCLHVSP_POLL_ID_1))
    {
        bRet = 1;
    }
    else
    {
        DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(),E_DRV_SCLIRQ_EVENT_CMDQDONE, &u32Events, E_DRV_SCLOS_EVENT_MD_OR, 100);
    }
    if(sbDone&&(DrvSclOsGetEvent(DrvSclIrqGetIrqSYNCEventID())&(E_DRV_SCLIRQ_EVENT_CMDQFIRE|E_DRV_SCLIRQ_EVENT_CMDQ)))
    {
        DRV_HVSP_MUTEX_LOCK();
        sbDone = 0;
        DRV_HVSP_MUTEX_UNLOCK();
    }
    if(u32Events&E_DRV_SCLIRQ_EVENT_CMDQDONE)
    {
        if((enPollId ==E_DRV_SCLHVSP_POLL_ID_1||enPollId ==E_DRV_SCLHVSP_POLL_ID_2))
        {
            DRV_HVSP_MUTEX_LOCK();
            sbDone = (0x1<<enPollId);
            DRV_HVSP_MUTEX_UNLOCK();
        }
        bRet = 1;
    }
    SCL_DBG(SCL_DBG_LV_IOCTL()&(0x1<<enPollId), "[HVSP]u32Events=%lx,bRet=%hhd,@:%lx\n"
        ,u32Events,bRet,DrvSclOsGetEvent(DrvSclIrqGetIrqSYNCEventID()));
    return bRet;
}

void * DrvSclHvspGetWaitQueueHead(void)
{
    return DrvSclIrqGetSyncQueue();
}
void DrvSclHvspRelease(DrvSclHvspIdType_e HVSP_IP)
{
    if(HVSP_IP == E_DRV_SCLHVSP_ID_1)
    {
        DrvSclHvspSetPriMaskTrigger(E_DRV_SCLHVSP_PRIMASK_DISABLE);
        HalSclHvspSetReset();
        if(!DrvSclOsReleaseMutexAll())
        {
            SCL_DBG(SCL_DBG_LV_MDRV_IO(), "[DRVHVSP]!!!!!!!!!!!!!!!!!!! HVSP Release Mutex fail\n");
        }
    }
    _DrvSclHvspInitSWVarialbe(HVSP_IP);
    if(HVSP_IP == E_DRV_SCLHVSP_ID_1)
    {
        DrvSclCmdqrelease(E_DRV_SCLCMDQ_TYPE_IP0);
    }
    SCL_DBG(SCL_DBG_LV_DRVHVSP()&(Get_DBGMG_HVSP(HVSP_IP)), "[DRVHVSP]%s(%d) HVSP %d Release\n", __FUNCTION__, __LINE__,HVSP_IP);
}
void DrvSclHvspOpen(DrvSclHvspIdType_e HVSP_IP)
{
    if(HVSP_IP==E_DRV_SCLHVSP_ID_1)
    {
        DrvSclCmdqEnable(1,0);
    }
}
bool DrvSclHvspSuspend(DrvSclHvspSuspendResumeConfig_t *pCfg)
{
    DrvSclIrqSuspendResumeConfig_t stSclirq;
    bool bRet = TRUE;

    DrvSclOsObtainMutex(_HVSP_Mutex, SCLOS_WAIT_FOREVER);
    HalSclHvspSetReset();
    if(gbHvspSuspend == 0)
    {
        stSclirq.u32IRQNUM =  pCfg->u32IRQNUM;
        stSclirq.u32CMDQIRQNUM =  pCfg->u32CMDQIRQNUM;
        if(DrvSclIrqSuspend(&stSclirq))
        {
            bRet = TRUE;
            gbHvspSuspend = 1;
        }
        else
        {
            bRet = FALSE;
            DRV_HVSP_ERR(sclprintf("[DRVHVSP]%s(%d) Suspend IRQ Fail\n", __FUNCTION__, __LINE__));
        }
    }
    else
    {
        SCL_DBG(SCL_DBG_LV_DRVHVSP()&(Get_DBGMG_HVSP(3)), "[DRVHVSP]%s(%d) alrady suspned\n", __FUNCTION__, __LINE__);
        bRet = TRUE;
    }

    DrvSclOsReleaseMutex(_HVSP_Mutex);

    return bRet;
}
void _Drv_HVSP_PriMaskInit(void)
{
    HalSclHvspPriMaskColorY(0x0);
    HalSclHvspPriMaskColorU(0x200);
    HalSclHvspPriMaskColorV(0x200);
}
void _Drv_HVSP_SetHWinit(void)
{
    _Drv_HVSP_PriMaskInit();
    HalSclHvspSetTestPatCfg();
    HalSclHvspSetVpsSRAMEn(1);
    HalSclHvspSetIpmBufferNumber(gu8FBBufferNum);
    HalSclHvspSetOsdBypass(E_DRV_SCLHVSP_ID_1, 1);
    HalSclHvspSetOsdBypassWTM(E_DRV_SCLHVSP_ID_1, 1);
    HalSclHvspSramDump(E_HAL_SCLHVSP_SRAM_DUMP_HVSP_V,SRAMFORSCALUP);
    HalSclHvspSramDump(E_HAL_SCLHVSP_SRAM_DUMP_HVSP_V_1,SRAMFORSCALDOWN);
    HalSclHvspSramDump(E_HAL_SCLHVSP_SRAM_DUMP_HVSP_V_2,SRAMFORSC3VDOWN);
    HalSclHvspSramDump(E_HAL_SCLHVSP_SRAM_DUMP_HVSP_H,SRAMFORSCALUP);
    HalSclHvspSramDump(E_HAL_SCLHVSP_SRAM_DUMP_HVSP_H_1,SRAMFORSCALDOWN);
    HalSclHvspSramDump(E_HAL_SCLHVSP_SRAM_DUMP_HVSP_H_2,SRAMFORSC3HDOWN);
    gbVScalingup[E_DRV_SCLHVSP_ID_3] = SRAMFORSC3VDOWN;
    gbHScalingup[E_DRV_SCLHVSP_ID_3] = SRAMFORSC3HDOWN;
}

bool DrvSclHvspResume(DrvSclHvspSuspendResumeConfig_t *pCfg)
{
    DrvSclIrqSuspendResumeConfig_t stSclirq;
    bool bRet = TRUE;

    DrvSclOsObtainMutex(_HVSP_Mutex, SCLOS_WAIT_FOREVER);

    //sclprintf("%s,(%d) %d\n", __FUNCTION__, __LINE__, gbHvspSuspend);
    if(gbHvspSuspend == 1)
    {
        stSclirq.u32IRQNUM =  pCfg->u32IRQNUM;
        stSclirq.u32CMDQIRQNUM =  pCfg->u32CMDQIRQNUM;
        if(DrvSclIrqResume(&stSclirq))
        {
            DrvSclIrqInterruptEnable(SCLIRQ_VSYNC_FCLK_LDC);
            HalSclHvspSetTestPatCfg();
            gbHvspSuspend = 0;
            bRet = TRUE;
        }
        else
        {

            DRV_HVSP_ERR(sclprintf("[DRVHVSP]%s(%d) Resume IRQ Fail\n", __FUNCTION__, __LINE__));
            bRet = FALSE;
        }
        _Drv_HVSP_SetHWinit();
    }
    else
    {
        SCL_DBG(SCL_DBG_LV_DRVHVSP()&(Get_DBGMG_HVSP(3)), "[DRVHVSP]%s(%d) alrady resume\n", __FUNCTION__, __LINE__);
        bRet = TRUE;
    }

    DrvSclOsReleaseMutex(_HVSP_Mutex);

    return bRet;
}
void DrvSclHvspSetBufferNum(u8 u8Num)
{
    gu8FBBufferNum = u8Num;
}
bool DrvSclHvspGetBufferNum(void)
{
    return gu8FBBufferNum;
}
void DrvSclHvspPriMaskBufferReset(void)
{
    DRV_HVSP_MUTEX_LOCK();
    DrvSclOsMemset(gpu8PriMaskBuf,0,HVSP_PRIMASK_SIZE);
    DRV_HVSP_MUTEX_UNLOCK();
}
void DrvSclHvspExit(u8 bCloseISR)
{
    if(_HVSP_Mutex != -1)
    {
        DrvSclOsDeleteMutex(_HVSP_Mutex);
        _HVSP_Mutex = -1;
    }
    if(bCloseISR)
    {
        DrvSclIrqExit();
    }
    if(gpu8PriMaskBuf)
    {
        DrvSclOsVirMemFree(gpu8PriMaskBuf);  //probe ,free
        gpu8PriMaskBuf = NULL;
    }
    HalSclHvspExit();
    gbMemReadyForDrv = 0;
    gu8FBBufferNum = 0;
    gbclkforcemode = 0;
}

bool DrvSclHvspInit(DrvSclHvspInitConfig_t *pInitCfg)
{
    char word[] = {"_HVSP_Mutex"};
    DrvSclIrqInitConfig_t stIRQInitCfg;
    u8 u8IDidx;

    if(_HVSP_Mutex != -1)
    {
        SCL_DBG(SCL_DBG_LV_DRVHVSP()&(Get_DBGMG_HVSP(3)), "[DRVHVSP]%s(%d) alrady done\n", __FUNCTION__, __LINE__);
        return TRUE;
    }

    if(DrvSclOsInit() == FALSE)
    {
        DRV_HVSP_ERR(sclprintf("[DRVHVSP]%s(%d) DrvSclOsInit Fail\n", __FUNCTION__, __LINE__));
        return FALSE;
    }

    _HVSP_Mutex = DrvSclOsCreateMutex(E_DRV_SCLOS_FIFO, word, SCLOS_PROCESS_SHARED);

    if (_HVSP_Mutex == -1)
    {
        DRV_HVSP_ERR(sclprintf("[DRVHVSP]%s(%d): create mutex fail\n", __FUNCTION__, __LINE__));
        return FALSE;
    }

    stIRQInitCfg.u32RiuBase = pInitCfg->u32RIUBase;
    stIRQInitCfg.u32IRQNUM  = pInitCfg->u32IRQNUM;
    stIRQInitCfg.u32CMDQIRQNUM  = pInitCfg->u32CMDQIRQNUM;
    if(DrvSclIrqInit(&stIRQInitCfg) == FALSE)
    {
        DRV_HVSP_ERR(sclprintf("[DRVHVSP]%s(%d) Init IRQ Fail\n", __FUNCTION__, __LINE__));
        return FALSE;
    }

    HalSclHvspSetRiuBase(pInitCfg->u32RIUBase);
    DRV_HVSP_MUTEX_LOCK();
    HalSclHvspSetReset();
    for(u8IDidx = E_DRV_SCLHVSP_ID_1; u8IDidx<E_DRV_SCLHVSP_ID_MAX; u8IDidx++)
    {
        _DrvSclHvspInitSWVarialbe(u8IDidx);
        _Drv_HVSP_SetCoringThrdOn(u8IDidx);
    }
    DrvSclHvspSetCmdqTrigTypeByRIU();
    DRV_HVSP_MUTEX_UNLOCK();
    gpu8PriMaskBuf = DrvSclOsVirMemalloc(HVSP_PRIMASK_SIZE);  //probe ,not free yet
    DrvSclHvspPriMaskBufferReset();
    DrvSclIrqInterruptEnable(SCLIRQ_VSYNC_FCLK_LDC);
    HalSclHvspSetFrameBufferManageLock(0);
    DrvSclIrqSetDnrLock(0);
    _Drv_HVSP_SetHWinit();
    return TRUE;
}
DrvSclHvspCmdTrigConfig_t DrvSclHvspSetCmdqTrigTypeByRIU(void)
{
    DrvSclHvspCmdTrigConfig_t stCmdTrigCfg,stori;
    Drv_HVSP_GetCMDQTrigType(&stori);
    stCmdTrigCfg.enType     = E_DRV_SCLHVSP_CMD_TRIG_NONE;
    stCmdTrigCfg.u8Fmcnt    = 0;
    DrvSclHvspSetCmdqTrigType(stCmdTrigCfg);
    return stori;
}
void _DrvSclHvspSetPreCropWhenInputMuxReady(void)
{
    HalSclHvspCropInfo_t stCropInfo_1;
    if(Is_INPUTMUX_SetReady())
    {
        stCropInfo_1 = _Drv_HVSP_FillPreCropInfo();
        HalSclHvspSetCropConfig(E_DRV_SCLHVSP_CROP_ID_1, stCropInfo_1);
    }
}
bool DrvSclHvspSetIPMConfig(DrvSclHvspIpmConfig_t stCfg)
{
    DrvSclHvspCmdTrigConfig_t stCmdTrigCfg;
    SCL_DBG(SCL_DBG_LV_DRVHVSP()&(Get_DBGMG_HVSP(0)), "[DRVHVSP]%s(%d): Base=%lx, Fetch=%d, Vsize=%d\n"
        , __FUNCTION__, __LINE__, stCfg.u32YCBaseAddr, stCfg.u16Fetch, stCfg.u16Vsize);
    DrvSclOsMemcpy(&gstIPMCfg, &stCfg, sizeof(DrvSclHvspIpmConfig_t));
    DRV_HVSP_MUTEX_LOCK();
    if(gstIPMCfg.u16Fetch%2)
    {
        gstIPMCfg.u16Fetch--;
        stCfg.u16Fetch--;
        SCL_ERR("[DRVHVSP]IPM Width not align 2\n");
    }
    stCmdTrigCfg = DrvSclHvspSetCmdqTrigTypeByRIU();
    _DrvSclHvspSetPreCropWhenInputMuxReady();
    HalSclHvspSetBt656SrcConfig(1,stCfg.u16Fetch);
    HalSclHvspSetIpmYCBase(stCfg.u32YCBaseAddr);
    HalSclHvspSetIpmMotionBase(stCfg.u32MBaseAddr);
    HalSclHvspSetIpmCiirBase(stCfg.u32CIIRBaseAddr);
    HalSclHvspSetIpmFetchNum(stCfg.u16Fetch);
    HalSclHvspSetIpmLineOffset(stCfg.u16Fetch);
    HalSclHvspSetIpmvSize(stCfg.u16Vsize);
    HalSclHvspSetIpmYCMReadEn(stCfg.bYCMRead);
    HalSclHvspSetIpmYCMWriteEn(stCfg.bYCMWrite);
    HalSclHvspSetIpmCiirReadEn(stCfg.bCIIRRead);
    HalSclHvspSetIpmCiirWriteEn(stCfg.bCIIRWrite);
    DrvSclHvspSetCmdqTrigType(stCmdTrigCfg);
    DRV_HVSP_MUTEX_UNLOCK();
    return TRUE;
}
void DrvSclHvspSetFbManageConfig(DrvSclHvspSetFbManageConfig_t stCfg)
{
    (sclprintf("[DRVHVSP]%s(%d):%x \n",
        __FUNCTION__, __LINE__, stCfg.enSet));
    HalSclHvspSetFrameBufferManageLock(0);
    if(stCfg.enSet & E_DRV_SCLHVSP_FBMG_SET_LDCPATH_ON)
    {
        HalSclHvspSetLdcPathSel(1);
        sclprintf("LDC ON\n");
    }
    else if(stCfg.enSet & E_DRV_SCLHVSP_FBMG_SET_LDCPATH_OFF)
    {
        HalSclHvspSetLdcPathSel(0);
        sclprintf("LDC OFF\n");
    }
    if(stCfg.enSet & E_DRV_SCLHVSP_FBMG_SET_PRVCROP_ON)
    {
        HalSclHvspSetPrv2CropOnOff(1);
        sclprintf("PRVCROP ON\n");
    }
    else if(stCfg.enSet & E_DRV_SCLHVSP_FBMG_SET_PRVCROP_OFF)
    {
        HalSclHvspSetPrv2CropOnOff(0);
        sclprintf("PRVCROP OFF\n");
    }
    if(stCfg.enSet & E_DRV_SCLHVSP_FBMG_SET_DNR_Read_ON)
    {
        HalSclHvspSetIpmYCMReadEn(1);
        sclprintf("DNRR ON\n");
    }
    else if(stCfg.enSet & E_DRV_SCLHVSP_FBMG_SET_DNR_Read_OFF)
    {
        HalSclHvspSetIpmYCMReadEn(0);
        sclprintf("DNRR OFF\n");
    }
    if(stCfg.enSet & E_DRV_SCLHVSP_FBMG_SET_CIIR_ON)
    {
        HalSclHvspSetIpmCiirReadEn(1);
        HalSclHvspSetIpmCiirWriteEn(1);
        sclprintf("DNRR ON\n");
    }
    else if(stCfg.enSet & E_DRV_SCLHVSP_FBMG_SET_CIIR_OFF)
    {
        HalSclHvspSetIpmCiirReadEn(0);
        HalSclHvspSetIpmCiirWriteEn(0);
        sclprintf("DNRR OFF\n");
    }
    if(stCfg.enSet & E_DRV_SCLHVSP_FBMG_SET_DNR_Write_ON)
    {
        HalSclHvspSetIpmYCMWriteEn(1);
        sclprintf("DNRW ON\n");
    }
    else if(stCfg.enSet & E_DRV_SCLHVSP_FBMG_SET_DNR_Write_OFF)
    {
        HalSclHvspSetIpmYCMWriteEn(0);
        sclprintf("DNRW OFF\n");
    }
    if(stCfg.enSet & E_DRV_SCLHVSP_FBMG_SET_DNR_BUFFER_1)
    {
        HalSclHvspSetIpmBufferNumber(1);
        sclprintf("DNRB 1\n");
    }
    else if(stCfg.enSet & E_DRV_SCLHVSP_FBMG_SET_DNR_BUFFER_2)
    {
        HalSclHvspSetIpmBufferNumber(2);
        sclprintf("DNRB 2\n");
    }
    HalSclHvspSetFrameBufferManageLock(1);
    if(stCfg.enSet & E_DRV_SCLHVSP_FBMG_SET_UNLOCK)
    {
        HalSclHvspSetFrameBufferManageLock(0);
        DrvSclIrqSetDnrLock(0);
        sclprintf("UNLOCK\n");
    }
    if(stCfg.enSet & E_DRV_SCLHVSP_FBMG_SET_LOCK)
    {
        HalSclHvspSetFrameBufferManageLock(1);
        DrvSclIrqSetDnrLock(1);
        sclprintf("LOCK\n");
    }
}

void _DrvSclHvspSetCmdqTrigFire(DrvSclHvspCmdTrigConfig_t stCmdTrigCfg,DrvSclHvspIdType_e HVSP_IP)
{
    u32 u32Time;
    u32 u32Events = 0;
    u32Time = ((u32)DrvSclOsGetSystemTime());
    if(stCmdTrigCfg.enType != E_DRV_SCLHVSP_CMD_TRIG_NONE)
    {
        if(stCmdTrigCfg.enType == E_DRV_SCLHVSP_CMD_TRIG_POLL_LDC_SYNC)
        {
            while(1)
            {
                if(HVSP_IP == E_DRV_SCLHVSP_ID_3)
                {
                    DrvSclOsWaitEvent(DrvSclIrqGetIrqSC3EventID(),
                        E_DRV_SCLIRQ_SC3EVENT_HVSPST, &u32Events, E_DRV_SCLOS_EVENT_MD_OR, SCLOS_WAIT_FOREVER); // get status: FRM END
                    DrvSclOsClearEventIRQ(DrvSclIrqGetIrqSC3EventID(),E_DRV_SCLIRQ_SC3EVENT_HVSPST);
                    break;
                }
                else
                {
                    DrvSclOsWaitEvent(DrvSclIrqGetIrqEventID(),
                        E_DRV_SCLIRQ_EVENT_HVSPST, &u32Events, E_DRV_SCLOS_EVENT_MD_OR, SCLOS_WAIT_FOREVER); // get status: FRM END
                    DrvSclOsClearEventIRQ(DrvSclIrqGetIrqEventID(),E_DRV_SCLIRQ_EVENT_HVSPST);
                    break;
                }
                if((u32)DrvSclOsGetSystemTime() - u32Time > 50)
                {
                    break;
                }
            }
        }
        HalSclHvspSetCmdqTrigFire();
        SCL_DBG(SCL_DBG_LV_DRVHVSP()&(Get_DBGMG_HVSP(HVSP_IP)),
            "[DRVHVSP]%s id:%d @:%lu\n", __FUNCTION__,HVSP_IP,(u32)DrvSclOsGetSystemTime());
    }
}
void DrvSclHvspSetMemoryAllocateReady(bool bEn)
{
    gbMemReadyForDrv = bEn;
}
u8 _DrvSclHvspGetScalingHRatioConfig(DrvSclHvspIdType_e enHVSP_ID,u8 bUp)
{
    u8 bret = 0;
    if(bUp)
    {
        if(enHVSP_ID ==E_DRV_SCLHVSP_ID_1)
        {
            bret = SRAMFORSCALUP;
        }
        else if(enHVSP_ID ==E_DRV_SCLHVSP_ID_2)
        {
            bret = SRAMFORSC2ALUP;
        }
        else if(enHVSP_ID ==E_DRV_SCLHVSP_ID_3)
        {
            bret = SRAMFORSC3HUP;
        }
    }
    else
    {
        if(enHVSP_ID ==E_DRV_SCLHVSP_ID_1)
        {
            bret = SRAMFORSCALDOWN;
        }
        else if(enHVSP_ID ==E_DRV_SCLHVSP_ID_2)
        {
            if(gstScalinInfo[enHVSP_ID].stSizeAfterScaling.u16Width >=HD_Width)
            {
                bret = SRAMFORSCALDOWN;
            }
            else
            {
                bret = SRAMFORSC2ALDOWN;
            }
        }
        else if(enHVSP_ID ==E_DRV_SCLHVSP_ID_3)
        {
            // if sc 2 output > 704x576 use talbe 1.
            if(gstScalinInfo[E_DRV_SCLHVSP_ID_2].stSizeAfterScaling.u16Width >=D_Width)
            {
                bret = SRAMFORSC2ALDOWN;
            }
            else
            {
                bret = SRAMFORSC3HDOWN;
            }
        }
    }
    return bret;
}
u8 _DrvSclHvspGetScalingVRatioConfig(DrvSclHvspIdType_e enHVSP_ID,u8 bUp)
{
    u8 bret = 0;
    if(bUp)
    {
        if(enHVSP_ID ==E_DRV_SCLHVSP_ID_1)
        {
            bret = SRAMFORSCALUP;
        }
        else if(enHVSP_ID ==E_DRV_SCLHVSP_ID_2)
        {
            bret = SRAMFORSC2ALUP;
        }
        else if(enHVSP_ID ==E_DRV_SCLHVSP_ID_3)
        {
            bret = SRAMFORSC3VUP;
        }
    }
    else
    {
        if(enHVSP_ID ==E_DRV_SCLHVSP_ID_1)
        {
            bret = SRAMFORSCALDOWN;
        }
        else if(enHVSP_ID ==E_DRV_SCLHVSP_ID_2)
        {
            if(gstScalinInfo[enHVSP_ID].stSizeAfterScaling.u16Height >=HD_Height)
            {
                bret = SRAMFORSCALDOWN;
            }
            else
            {
                bret = SRAMFORSC2ALDOWN;
            }
        }
        else if(enHVSP_ID ==E_DRV_SCLHVSP_ID_3)
        {
            // if sc 2 output > 704x576 use talbe 1.
            if(gstScalinInfo[E_DRV_SCLHVSP_ID_2].stSizeAfterScaling.u16Height >=D_Height)
            {
                bret = SRAMFORSCALDOWN;
            }
            else
            {
                bret = SRAMFORSC3VDOWN;
            }
        }
    }
    return bret;
}
void _DrvSclHvspSetHorizotnalScalingConfig(DrvSclHvspIdType_e enHVSP_ID,bool bEn)
{

    HalSclHvspSetScalingHoEn(enHVSP_ID, bEn);
    HalSclHvspSetScalingHoFacotr(enHVSP_ID, bEn ? gstScalinInfo[enHVSP_ID].u32ScalingRatio_H: 0);
    HalSclHvspSetModeYHo(enHVSP_ID,bEn ? E_HAL_SCLHVSP_FILTER_MODE_SRAM_0: E_HAL_SCLHVSP_FILTER_MODE_BYPASS);
    HalSclHvspSetModeCHo(enHVSP_ID,bEn ? E_HAL_SCLHVSP_FILTER_MODE_BILINEAR: E_HAL_SCLHVSP_FILTER_MODE_BYPASS,E_HAL_SCLHVSP_SRAM_SEL_0);
    HalSclHvspSetHspDithEn(enHVSP_ID,bEn);
    HalSclHvspSetHspCoringEnC(enHVSP_ID,bEn);
    HalSclHvspSetHspCoringEnY(enHVSP_ID,bEn);
}
void _DrvSclHvspSetHTbl(DrvSclHvspIdType_e enHVSP_ID)
{
    if(gbHScalingup[enHVSP_ID] &0x1)
    {
        if(enHVSP_ID ==E_DRV_SCLHVSP_ID_1)
        {
            gbHScalingup[enHVSP_ID] = SRAMFORSCALUP;
        }
        else if(enHVSP_ID ==E_DRV_SCLHVSP_ID_2)
        {
            gbHScalingup[enHVSP_ID] = SRAMFORSC2ALUP;
        }
        else if(enHVSP_ID ==E_DRV_SCLHVSP_ID_3)
        {
            gbHScalingup[enHVSP_ID] = SRAMFORSC3HUP;
        }
    }
    else
    {
        if(enHVSP_ID ==E_DRV_SCLHVSP_ID_1)
        {
            gbHScalingup[enHVSP_ID] = SRAMFORSCALDOWN;
        }
        else if(enHVSP_ID ==E_DRV_SCLHVSP_ID_2)
        {
            if(gstScalinInfo[enHVSP_ID].stSizeAfterScaling.u16Width >=HD_Width)
            {
                gbHScalingup[enHVSP_ID] = SRAMFORSCALDOWN;
            }
            else
            {
                gbHScalingup[enHVSP_ID] = SRAMFORSC2ALDOWN;
            }
        }
        else if(enHVSP_ID ==E_DRV_SCLHVSP_ID_3)
        {
            // if sc 2 output > 704x576 use talbe 1.
            if(gstScalinInfo[E_DRV_SCLHVSP_ID_2].stSizeAfterScaling.u16Width >=D_Width)
            {
                gbHScalingup[enHVSP_ID] = SRAMFORSC2ALDOWN;
            }
            else
            {
                gbHScalingup[enHVSP_ID] = SRAMFORSC3HDOWN;
            }
        }
    }
}
void _DrvSclHvspSetVTbl(DrvSclHvspIdType_e enHVSP_ID)
{
    if(gbVScalingup[enHVSP_ID] &0x1)
    {
        if(enHVSP_ID ==E_DRV_SCLHVSP_ID_1)
        {
            gbVScalingup[enHVSP_ID] = SRAMFORSCALUP;
        }
        else if(enHVSP_ID ==E_DRV_SCLHVSP_ID_2)
        {
            gbVScalingup[enHVSP_ID] = SRAMFORSC2ALUP;
        }
        else if(enHVSP_ID ==E_DRV_SCLHVSP_ID_3)
        {
            gbVScalingup[enHVSP_ID] = SRAMFORSC3VUP;
        }
    }
    else
    {
        if(enHVSP_ID ==E_DRV_SCLHVSP_ID_1)
        {
            gbVScalingup[enHVSP_ID] = SRAMFORSCALDOWN;
        }
        else if(enHVSP_ID ==E_DRV_SCLHVSP_ID_2)
        {
            if(gstScalinInfo[enHVSP_ID].stSizeAfterScaling.u16Height>=HD_Height)
            {
                gbVScalingup[enHVSP_ID] = SRAMFORSCALDOWN;
            }
            else
            {
                gbVScalingup[enHVSP_ID] = SRAMFORSC2ALDOWN;
            }
        }
        else if(enHVSP_ID ==E_DRV_SCLHVSP_ID_3)
        {
            if(gstScalinInfo[E_DRV_SCLHVSP_ID_2].stSizeAfterScaling.u16Height >=D_Height)
            {
                gbVScalingup[enHVSP_ID] = SRAMFORSCALDOWN;
            }
            else
            {
                gbVScalingup[enHVSP_ID] = SRAMFORSC3VDOWN;
            }
        }
    }
}
void _DrvSclHvspSetHorizotnalSramTbl(DrvSclHvspIdType_e enHVSP_ID)
{
    u32 u32flag;
    if((_CHANGE_SRAM_H_QMAP(enHVSP_ID,(gbHScalingup[enHVSP_ID]&0x1))))
    {
        gbHScalingup[enHVSP_ID] &= 0x1;
        gbHScalingup[enHVSP_ID] = (gbHScalingup[enHVSP_ID]^1);//XOR 1 :reverse
        //gbHScalingup[enHVSP_ID] |= ((_DrvSclHvspGetScalingHRatioConfig(enHVSP_ID,gbHScalingup[enHVSP_ID]&0x1))<<4);
        _DrvSclHvspSetHTbl(enHVSP_ID);
        u32flag = (enHVSP_ID==E_DRV_SCLHVSP_ID_1) ? E_DRV_SCLIRQ_EVENT_SC1HSRAMSET :
                  (enHVSP_ID==E_DRV_SCLHVSP_ID_2) ? E_DRV_SCLIRQ_EVENT_SC2HSRAMSET :
                                             E_DRV_SCLIRQ_EVENT_SC3HSRAMSET;
        if(DrvSclIrqGetIsBlankingRegion()|| enHVSP_ID ==E_DRV_SCLHVSP_ID_3 || VIPSETRULE())
        {
            HalSclHvspSramDump(enHVSP_ID+HVSP_ID_SRAM_H_OFFSET,gbHScalingup[enHVSP_ID]); //level 1 :up 0:down
            SCL_DBG(SCL_DBG_LV_DRVHVSP()&(Get_DBGMG_HVSP(enHVSP_ID)),
                "[DRVHVSP]Change and do H Qmap SRAM id:%d scaling UP:%hhx\n",enHVSP_ID,gbHScalingup[enHVSP_ID]);
        }
        else
        {
            DrvSclOsSetEventIrq(DrvSclIrqGetIrqSYNCEventID(), u32flag);
            SCL_DBG(SCL_DBG_LV_DRVHVSP()&(Get_DBGMG_HVSP(enHVSP_ID)),
                "[DRVHVSP]Change H Qmap SRAM id:%d scaling UP:%hhx\n",enHVSP_ID,gbHScalingup[enHVSP_ID]);
        }
    }
    else if(gbSc3FirstHSet && enHVSP_ID ==E_DRV_SCLHVSP_ID_3)
    {
        gbSc3FirstHSet = 0;
        _DrvSclHvspSetHTbl(enHVSP_ID);
        u32flag = E_DRV_SCLIRQ_EVENT_SC3HSRAMSET;
         if(enHVSP_ID ==E_DRV_SCLHVSP_ID_3)
         {
             HalSclHvspSramDump(enHVSP_ID+HVSP_ID_SRAM_H_OFFSET,gbHScalingup[enHVSP_ID]); //level 1 :up 0:down
             SCL_DBG(SCL_DBG_LV_DRVHVSP()&(Get_DBGMG_HVSP(enHVSP_ID)),
                 "[DRVHVSP]Change and do H Qmap SRAM id:%d scaling static UP:%hhx\n",enHVSP_ID,gbHScalingup[enHVSP_ID]);
         }
    }
    else if(enHVSP_ID !=E_DRV_SCLHVSP_ID_3 && ((_DrvSclHvspGetScalingHRatioConfig(enHVSP_ID,gbHScalingup[enHVSP_ID]&0x1))!=
        ((gbHScalingup[enHVSP_ID]&0xF0)>>4)))
    {
        //gbHScalingup[enHVSP_ID] &= 0x1;
        //gbHScalingup[enHVSP_ID] |= ((_DrvSclHvspGetScalingHRatioConfig(enHVSP_ID,gbHScalingup[enHVSP_ID]&0x1))<<4);
        _DrvSclHvspSetHTbl(enHVSP_ID);
        u32flag = (enHVSP_ID==E_DRV_SCLHVSP_ID_1) ? E_DRV_SCLIRQ_EVENT_SC1HSRAMSET :
                  (enHVSP_ID==E_DRV_SCLHVSP_ID_2) ? E_DRV_SCLIRQ_EVENT_SC2HSRAMSET :
                                             E_DRV_SCLIRQ_EVENT_SC3HSRAMSET;
        if(DrvSclIrqGetIsBlankingRegion()|| enHVSP_ID ==E_DRV_SCLHVSP_ID_3)
        {
            HalSclHvspSramDump(enHVSP_ID+HVSP_ID_SRAM_H_OFFSET,gbHScalingup[enHVSP_ID]); //level 1 :up 0:down
            SCL_DBG(SCL_DBG_LV_DRVHVSP()&(Get_DBGMG_HVSP(enHVSP_ID)),
                "[DRVHVSP]Change and do H Qmap SRAM id:%d scaling RUP:%hhx\n",enHVSP_ID,gbHScalingup[enHVSP_ID]);
        }
        else
        {
            DrvSclOsSetEventIrq(DrvSclIrqGetIrqSYNCEventID(), u32flag);
            SCL_DBG(SCL_DBG_LV_DRVHVSP()&(Get_DBGMG_HVSP(enHVSP_ID)),
                "[DRVHVSP]Change H Qmap SRAM id:%d scaling RUP:%hhx\n",enHVSP_ID,gbHScalingup[enHVSP_ID]);
        }
    }
}
void _DrvSclHvspSetVerticalScalingConfig(DrvSclHvspIdType_e enHVSP_ID,bool bEn)
{
    HalSclHvspSetScalingVeEn(enHVSP_ID, bEn);
    HalSclHvspSetScalingVeFactor(enHVSP_ID,  bEn ? gstScalinInfo[enHVSP_ID].u32ScalingRatio_V: 0);
    HalSclHvspSetModeYVe(enHVSP_ID,bEn ? E_HAL_SCLHVSP_FILTER_MODE_SRAM_0: E_HAL_SCLHVSP_FILTER_MODE_BYPASS);
    HalSclHvspSetModeCVe(enHVSP_ID,bEn ? E_HAL_SCLHVSP_FILTER_MODE_BILINEAR: E_HAL_SCLHVSP_FILTER_MODE_BYPASS,E_HAL_SCLHVSP_SRAM_SEL_0);
    HalSclHvspSetVspDithEn(enHVSP_ID,bEn);
    HalSclHvspSetVspCoringEnC(enHVSP_ID,bEn);
    HalSclHvspSetVspCoringEnY(enHVSP_ID,bEn);
}
void _DrvSclHvspSetVerticalSramTbl(DrvSclHvspIdType_e enHVSP_ID)
{
    u32 u32flag;
    if(_CHANGE_SRAM_V_QMAP(enHVSP_ID,(gbVScalingup[enHVSP_ID]&0x1)))
    {
        gbVScalingup[enHVSP_ID] &= 0x1;
        gbVScalingup[enHVSP_ID] = (gbVScalingup[enHVSP_ID]^1);//XOR 1 :reverse
        //gbVScalingup[enHVSP_ID] |= ((_DrvSclHvspGetScalingVRatioConfig(enHVSP_ID,gbVScalingup[enHVSP_ID]&0x1))<<4);
        _DrvSclHvspSetVTbl(enHVSP_ID);
        u32flag = (enHVSP_ID==E_DRV_SCLHVSP_ID_1) ? E_DRV_SCLIRQ_EVENT_SC1VSRAMSET :
                  (enHVSP_ID==E_DRV_SCLHVSP_ID_2) ? E_DRV_SCLIRQ_EVENT_SC2VSRAMSET :
                                             E_DRV_SCLIRQ_EVENT_SC3VSRAMSET;

        if(DrvSclIrqGetIsBlankingRegion()|| enHVSP_ID ==E_DRV_SCLHVSP_ID_3 || VIPSETRULE())
        {
            HalSclHvspSramDump(enHVSP_ID+HVSP_ID_SRAM_V_OFFSET,gbVScalingup[enHVSP_ID]); //level 1 :up 0:down
            SCL_DBG(SCL_DBG_LV_DRVHVSP()&(Get_DBGMG_HVSP(enHVSP_ID)),
                "[DRVHVSP]Change and do V Qmap SRAM id:%d scaling UP:%hhx\n",enHVSP_ID,gbVScalingup[enHVSP_ID]);
        }
        else
        {
            DrvSclOsSetEventIrq(DrvSclIrqGetIrqSYNCEventID(), u32flag);
            SCL_DBG(SCL_DBG_LV_DRVHVSP()&(Get_DBGMG_HVSP(enHVSP_ID)),
                "[DRVHVSP]Change V Qmap SRAM id:%d scaling UP:%hhx\n",enHVSP_ID,gbVScalingup[enHVSP_ID]);
        }
    }
    else if(gbSc3FirstVSet && enHVSP_ID ==E_DRV_SCLHVSP_ID_3)
    {
        gbSc3FirstVSet = 0;
        _DrvSclHvspSetVTbl(enHVSP_ID);
        u32flag = E_DRV_SCLIRQ_EVENT_SC3VSRAMSET;
         if(enHVSP_ID ==E_DRV_SCLHVSP_ID_3)
         {
             HalSclHvspSramDump(enHVSP_ID+HVSP_ID_SRAM_V_OFFSET,gbVScalingup[enHVSP_ID]); //level 1 :up 0:down
             SCL_DBG(SCL_DBG_LV_DRVHVSP()&(Get_DBGMG_HVSP(enHVSP_ID)),
                 "[DRVHVSP]Change and do V Qmap SRAM id:%d scaling static UP:%hhx\n",enHVSP_ID,gbVScalingup[enHVSP_ID]);
         }
    }
    else if(enHVSP_ID !=E_DRV_SCLHVSP_ID_3 && ((_DrvSclHvspGetScalingVRatioConfig(enHVSP_ID,gbVScalingup[enHVSP_ID]&0x1))!=
        ((gbVScalingup[enHVSP_ID]&0xF0)>>4)))
    {
        //gbVScalingup[enHVSP_ID] &= 0x1;
        //gbVScalingup[enHVSP_ID] |= ((_DrvSclHvspGetScalingVRatioConfig(enHVSP_ID,gbVScalingup[enHVSP_ID]&0x1))<<4);
        _DrvSclHvspSetVTbl(enHVSP_ID);
        u32flag = (enHVSP_ID==E_DRV_SCLHVSP_ID_1) ? E_DRV_SCLIRQ_EVENT_SC1VSRAMSET :
                  (enHVSP_ID==E_DRV_SCLHVSP_ID_2) ? E_DRV_SCLIRQ_EVENT_SC2VSRAMSET :
                                             E_DRV_SCLIRQ_EVENT_SC3VSRAMSET;

        if(DrvSclIrqGetIsBlankingRegion()|| enHVSP_ID ==E_DRV_SCLHVSP_ID_3)
        {
            HalSclHvspSramDump(enHVSP_ID+HVSP_ID_SRAM_V_OFFSET,gbVScalingup[enHVSP_ID]); //level 1 :up 0:down
            SCL_DBG(SCL_DBG_LV_DRVHVSP()&(Get_DBGMG_HVSP(enHVSP_ID)),
                "[DRVHVSP]Change and do V Qmap SRAM id:%d scaling UP:%hhx\n",enHVSP_ID,gbVScalingup[enHVSP_ID]);
        }
        else
        {
            DrvSclOsSetEventIrq(DrvSclIrqGetIrqSYNCEventID(), u32flag);
            SCL_DBG(SCL_DBG_LV_DRVHVSP()&(Get_DBGMG_HVSP(enHVSP_ID)),
                "[DRVHVSP]Change V Qmap SRAM id:%d scaling UP:%hhx\n",enHVSP_ID,gbVScalingup[enHVSP_ID]);
        }
    }
}
HalSclHvspCropInfo_t _DrvSclHvspSetCrop(bool u8CropID,DrvSclHvspScalingConfig_t stCfg)
{
    HalSclHvspCropInfo_t stCropInfo;
    u16 u16In_hsize;
    u16 u16In_vsize;
    u16In_hsize = (u8CropID ==DRV_HVSP_CROP_1) ? gstSrcSize.u16Width : gstIPMCfg.u16Fetch;
    u16In_vsize = (u8CropID ==DRV_HVSP_CROP_1) ? gstSrcSize.u16Height : gstIPMCfg.u16Vsize;
    if(stCfg.bCropEn[u8CropID])
    {
        stCropInfo.bEn         = stCfg.bCropEn[u8CropID];
        if(u16In_hsize && u16In_vsize)
        {
            stCropInfo.u16In_hsize = u16In_hsize;
            stCropInfo.u16In_vsize = u16In_vsize;
        }
        else
        {
            stCropInfo.u16In_hsize = stCfg.u16Src_Width;
            stCropInfo.u16In_vsize = stCfg.u16Src_Height;
        }
        stCropInfo.u16Hsize    = stCfg.u16Crop_Width[u8CropID];
        stCropInfo.u16Vsize    = stCfg.u16Crop_Height[u8CropID];
        if(stCfg.u16Crop_Width[u8CropID]%2)
        {
            stCropInfo.u16Hsize      = stCfg.u16Crop_Width[u8CropID]-1;
        }
        if(u8CropID == DRV_HVSP_CROP_1)
        {
            gstIPMCfg.u16Fetch = stCropInfo.u16Hsize;
            gstIPMCfg.u16Vsize = stCropInfo.u16Vsize;
        }
        if(stCfg.u16Crop_X[u8CropID]%2)
        {
            stCropInfo.u16Hst      = stCfg.u16Crop_X[u8CropID]-1;
        }
        else
        {
            stCropInfo.u16Hst      = stCfg.u16Crop_X[u8CropID];
        }
        stCropInfo.u16Vst      = stCfg.u16Crop_Y[u8CropID];
    }
    else if((u8CropID == DRV_HVSP_CROP_1) &&(Is_PreCropWidthNeedToOpen() ||Is_PreCropHeightNeedToOpen()))
    {
        stCropInfo = _Drv_HVSP_FillPreCropInfo();
    }
    else
    {
        stCropInfo.bEn         = 0;
        if(u16In_hsize && u16In_vsize)
        {
            stCropInfo.u16In_hsize = u16In_hsize;
            stCropInfo.u16In_vsize = u16In_vsize;
            stCropInfo.u16Hsize    = u16In_hsize;//stCfg.u16Src_Width;
            stCropInfo.u16Vsize    = u16In_vsize;//stCfg.u16Src_Height;
        }
        else
        {
            stCropInfo.u16In_hsize = stCfg.u16Src_Width;
            stCropInfo.u16In_vsize = stCfg.u16Src_Height;
            stCropInfo.u16Hsize    = stCfg.u16Src_Width;//stCfg.u16Src_Width;
            stCropInfo.u16Vsize    = stCfg.u16Src_Height;//stCfg.u16Src_Height;
        }
        stCropInfo.u16Hst      = 0;
        stCropInfo.u16Vst      = 0;
    }
    return stCropInfo;
}
DrvSclHvspScalingConfig_t _DrvSclHvspSetScalingRetVal(HalSclHvspCropInfo_t stCropInfo_1, HalSclHvspCropInfo_t stCropInfo_2)
{
    DrvSclHvspScalingConfig_t stRet;
    stRet.bCropEn[DRV_HVSP_CROP_1] = stCropInfo_1.bEn;
    stRet.u16Crop_Height[DRV_HVSP_CROP_1] = stCropInfo_1.u16Vsize;
    stRet.u16Crop_Width[DRV_HVSP_CROP_1] = stCropInfo_1.u16Hsize;
    stRet.u16Crop_X[DRV_HVSP_CROP_1] = stCropInfo_1.u16Hst;
    stRet.u16Crop_Y[DRV_HVSP_CROP_1] = stCropInfo_1.u16Vst;
    stRet.bCropEn[DRV_HVSP_CROP_2] = stCropInfo_2.bEn;
    stRet.u16Crop_Height[DRV_HVSP_CROP_2] = stCropInfo_2.u16Vsize;
    stRet.u16Crop_Width[DRV_HVSP_CROP_2] = stCropInfo_2.u16Hsize;
    stRet.u16Crop_X[DRV_HVSP_CROP_2] = stCropInfo_2.u16Hst;
    stRet.u16Crop_Y[DRV_HVSP_CROP_2] = stCropInfo_2.u16Vst;
    return stRet;
}
DrvSclHvspScalingConfig_t DrvSclHvspSetScaling(DrvSclHvspIdType_e enHVSP_ID, DrvSclHvspScalingConfig_t stCfg, DrvSclHvspClkConfig_t* stclk)
{
    u8 u8FBbuffer = DrvSclHvspGetBufferNum();
    HalSclHvspIspClkType_e enISPClkType = HalSclHvspGetIspClkType();
    DrvSclHvspScalingConfig_t stRet;
    HalSclHvspCropInfo_t stCropInfo_2;
    HalSclHvspCropInfo_t stCropInfo_1;
    u16  u16Hst = HalSclHvspGetCropX();
    u16  u16Vst = HalSclHvspGetCropY();
    bool bChangePreCropPosition = 0;
    u16 u16FBWidth = gstIPMCfg.u16Fetch;
    u16 u16FBHeight = gstIPMCfg.u16Vsize;
    u32 u32bLDCEvent = 0;
    stCropInfo_1.bEn = 0;
    stCropInfo_2.bEn = 0;
    if(enHVSP_ID == E_DRV_SCLHVSP_ID_1)
    {
        u32 u32FrameBufferMemSize = gstIPMCfg.u32MemSize;
        u32 u32ReqMemSize = stCfg.u16Src_Width * stCfg.u16Src_Height * 2 * u8FBbuffer;


        SCL_DBG(SCL_DBG_LV_DRVHVSP()&(Get_DBGMG_HVSP(enHVSP_ID)), "[DRVHVSP]%s id:%d @:%lu\n",
            __FUNCTION__,enHVSP_ID,(u32)DrvSclOsGetSystemTime());

        if(Is_DNRBufferReady())
        {
            if(Is_FrameBufferTooSmall(u32ReqMemSize, u32FrameBufferMemSize))
            {
                DRV_HVSP_ERR(sclprintf("[DRVHVSP]%s %d:: Out of memory\n", __FUNCTION__, __LINE__));
                stRet.bRet = 0;
                return stRet;
            }
        }
        else
        {
            DRV_HVSP_ERR(sclprintf("[DRVHVSP]%s %d:: Can't Use DNR (FB Not Ready)\n", __FUNCTION__, __LINE__));
            if(Is_FrameBufferTooSmall(u32ReqMemSize, u32FrameBufferMemSize))
            {
                DRV_HVSP_ERR(sclprintf("[DRVHVSP]%s %d:: Out of memory\n", __FUNCTION__, __LINE__));
            }
        }
        if(stCfg.u16Src_Width != gstIPMCfg.u16Fetch)
        {
            SCL_DBGERR("[DRVHVSP]%s %d:: H Src Size Mismatch(%hd,%hd)\n",
                __FUNCTION__, __LINE__,stCfg.u16Src_Width,gstIPMCfg.u16Fetch);
        }
        if(stCfg.u16Src_Height != gstIPMCfg.u16Vsize)
        {
            SCL_DBGERR("[DRVHVSP]%s %d:: V Src Size Mismatch(%hd,%hd)\n",
                __FUNCTION__, __LINE__,stCfg.u16Src_Height,gstIPMCfg.u16Vsize);
        }

        // setup cmd trig config
        DRV_HVSP_MUTEX_LOCK();
        DrvSclHvspSetCmdqTrigType(stCfg.stCmdTrigCfg);
        stCropInfo_1 = _DrvSclHvspSetCrop(DRV_HVSP_CROP_1,stCfg);
        stCropInfo_2 = _DrvSclHvspSetCrop(DRV_HVSP_CROP_2,stCfg);
        if(_IsSingleBufferAndZoomSizeBiggerthanLimitation
            (u8FBbuffer,stCropInfo_2.u16Vsize, HalSclHvspGetLimitationByIspClk
            (enISPClkType, stCropInfo_1.u16Hsize ,stCfg.u16Dsp_Height)))
        {
            if(stCfg.bCropEn[DRV_HVSP_CROP_1])
            {
                stCfg.u16Crop_Height[DRV_HVSP_CROP_1] =
                    HalSclHvspGetLimitationByIspClk(enISPClkType, stCropInfo_1.u16Hsize ,stCfg.u16Dsp_Height) ;
                stCropInfo_1.u16Vsize    = stCfg.u16Crop_Height[DRV_HVSP_CROP_1];
                stCropInfo_1.u16Vst = ((stCropInfo_1.u16Vst+stCropInfo_1.u16Vsize)>stCropInfo_1.u16In_vsize) ?
                    (stCropInfo_1.u16In_vsize - stCropInfo_1.u16Vsize) : stCropInfo_1.u16Vst;
                gstIPMCfg.u16Vsize = stCropInfo_1.u16Vsize;
                stCropInfo_2.u16In_vsize = gstIPMCfg.u16Vsize;
                stCropInfo_2.u16Vsize    = gstIPMCfg.u16Vsize;//stCfg.u16Src_Height;
                SCL_ERR("[DRVHVSP]%s %d:: Buffer num : %hhd ,Zoom size bigger than Limitation and force to change(%hd,%hd,%hd,%hd)\n"
                    , __FUNCTION__, __LINE__,u8FBbuffer,stCropInfo_1.u16Hst,stCropInfo_1.u16Vst,
                    stCfg.u16Crop_Width[DRV_HVSP_CROP_1],stCfg.u16Crop_Height[DRV_HVSP_CROP_1]);
            }
            if(stCfg.bCropEn[DRV_HVSP_CROP_2])
            {
                stCfg.u16Crop_Height[DRV_HVSP_CROP_2] =
                    HalSclHvspGetLimitationByIspClk(enISPClkType, stCropInfo_2.u16Hsize ,stCfg.u16Dsp_Height);
                stCropInfo_2.u16Vsize    = stCfg.u16Crop_Height[DRV_HVSP_CROP_2];
                stCropInfo_2.u16Vst = ((stCropInfo_2.u16Vst+stCropInfo_2.u16Vsize)>stCropInfo_2.u16In_vsize) ?
                    (stCropInfo_2.u16In_vsize - stCropInfo_2.u16Vsize) : stCropInfo_2.u16Vst;
                SCL_ERR("[DRVHVSP]%s %d:: Buffer num : %hhd ,Zoom size bigger than Limitation and force to change(%hd,%hd,%hd,%hd)\n"
                    , __FUNCTION__, __LINE__,u8FBbuffer,stCropInfo_2.u16Hst,stCropInfo_2.u16Vst,
                    stCfg.u16Crop_Width[DRV_HVSP_CROP_2],stCfg.u16Crop_Height[DRV_HVSP_CROP_2]);
            }
            stRet = _DrvSclHvspSetScalingRetVal(stCropInfo_1,stCropInfo_2);
            stRet.bRet = 0;
        }

        gstScalinInfo[enHVSP_ID].stSizeAfterCrop.u16Width  = stCropInfo_2.u16Hsize;//stCfg.u16Src_Width;
        gstScalinInfo[enHVSP_ID].stSizeAfterCrop.u16Height = stCropInfo_2.u16Vsize;//stCfg.u16Src_Height;
        gstScalinInfo[enHVSP_ID].stSizeAfterScaling.u16Width  = stCfg.u16Dsp_Width;
        gstScalinInfo[enHVSP_ID].stSizeAfterScaling.u16Height = stCfg.u16Dsp_Height;

        // Crop1
        HalSclHvspSetCropConfig(E_DRV_SCLHVSP_CROP_ID_1, stCropInfo_1);
        // Crop2
        HalSclHvspSetCropConfig(E_DRV_SCLHVSP_CROP_ID_2, stCropInfo_2);
        HalSclHvspSetIpmvSize(stCropInfo_2.u16In_vsize);
        HalSclHvspSetIpmLineOffset(stCropInfo_2.u16In_hsize);
        HalSclHvspSetIpmFetchNum(stCropInfo_2.u16In_hsize);
        HalSclHvspSetLdcWidth(stCropInfo_2.u16In_hsize);
        HalSclHvspSetLdcHeight(stCropInfo_2.u16In_vsize);
        // NLM size
        HalSclHvspSetVipSize(gstScalinInfo[enHVSP_ID].stSizeAfterCrop.u16Width,
                gstScalinInfo[enHVSP_ID].stSizeAfterCrop.u16Height);
        HalSclHvspSetNlmLineBufferSize(stCropInfo_2.u16In_hsize,stCropInfo_2.u16In_vsize);
        HalSclHvspSetNlmEn(1);
        //AIP size
        HalSclHvspSetXnrSize(stCropInfo_2.u16In_hsize,stCropInfo_2.u16In_vsize);
        HalSclHvspSetWdrGlobalSize(gstScalinInfo[enHVSP_ID].stSizeAfterCrop.u16Width,
                gstScalinInfo[enHVSP_ID].stSizeAfterCrop.u16Height);
        HalSclHvspSetWdrLocalSize(gstScalinInfo[enHVSP_ID].stSizeAfterCrop.u16Width,
                gstScalinInfo[enHVSP_ID].stSizeAfterCrop.u16Height);
        HalSclHvspSetMXnrSize(gstScalinInfo[enHVSP_ID].stSizeAfterCrop.u16Width,
                gstScalinInfo[enHVSP_ID].stSizeAfterCrop.u16Height);
        HalSclHvspSetUVadjSize(gstScalinInfo[enHVSP_ID].stSizeAfterCrop.u16Width,
                gstScalinInfo[enHVSP_ID].stSizeAfterCrop.u16Height);
        HalSclHvspPriMaskWidth(gstScalinInfo[enHVSP_ID].stSizeAfterCrop.u16Width);
        HalSclHvspPriMaskHeight(gstScalinInfo[enHVSP_ID].stSizeAfterCrop.u16Height);
        if(gbPriMaskPending)
        {
            gbPriMaskPending = 0;
            HalSclHvspPriMaskbEn(E_HAL_SCLHVSP_TRIGGER_CMDQ);
        }
        bChangePreCropPosition = _IsChangePreCropPosition(stCropInfo_1.u16Hst,stCropInfo_1.u16Vst,u16Hst,u16Vst);
    }
    else
    {
        if(enHVSP_ID == E_DRV_SCLHVSP_ID_3 &&_IsFlagType(E_DRV_SCLDMA_3_FRM_W,E_DRV_SCLDMA_FLAG_ACTIVE))
        {
            stRet.bRet = 0;
            return stRet;
        }
        DRV_HVSP_MUTEX_LOCK();
        DrvSclHvspSetCmdqTrigType(stCfg.stCmdTrigCfg);
        gstScalinInfo[enHVSP_ID].stSizeAfterCrop.u16Width      = stCfg.u16Src_Width;
        gstScalinInfo[enHVSP_ID].stSizeAfterCrop.u16Height     = stCfg.u16Src_Height;
        gstScalinInfo[enHVSP_ID].stSizeAfterScaling.u16Width   = stCfg.u16Dsp_Width;
        gstScalinInfo[enHVSP_ID].stSizeAfterScaling.u16Height  = stCfg.u16Dsp_Height;
    }

    gstScalinInfo[enHVSP_ID].u32ScalingRatio_H = (u32)HVSP_RATIO(gstScalinInfo[enHVSP_ID].stSizeAfterCrop.u16Width,
                                                            gstScalinInfo[enHVSP_ID].stSizeAfterScaling.u16Width);

    gstScalinInfo[enHVSP_ID].u32ScalingRatio_V = (u32)HVSP_RATIO(gstScalinInfo[enHVSP_ID].stSizeAfterCrop.u16Height,
                                                            gstScalinInfo[enHVSP_ID].stSizeAfterScaling.u16Height);

    SCL_DBG(SCL_DBG_LV_DRVHVSP()&(Get_DBGMG_HVSP(enHVSP_ID)),
        "[DRVHVSP]%s(%d):: HVSP_%d, AfterCrop(%d, %d)\n", __FUNCTION__, __LINE__,
        enHVSP_ID, gstScalinInfo[enHVSP_ID].stSizeAfterCrop.u16Width, gstScalinInfo[enHVSP_ID].stSizeAfterCrop.u16Height);
    SCL_DBG(SCL_DBG_LV_DRVHVSP()&(Get_DBGMG_HVSP(enHVSP_ID)),
        "[DRVHVSP]%s(%d):: HVSP_%d, AfterScaling(%d, %d)\n", __FUNCTION__, __LINE__,
        enHVSP_ID, gstScalinInfo[enHVSP_ID].stSizeAfterScaling.u16Width, gstScalinInfo[enHVSP_ID].stSizeAfterScaling.u16Height);
    SCL_DBG(SCL_DBG_LV_DRVHVSP()&(Get_DBGMG_HVSP(enHVSP_ID)),
        "[DRVHVSP]%s(%d):: HVSP_%d, Ratio(%lx, %lx)\n", __FUNCTION__, __LINE__,
        enHVSP_ID, gstScalinInfo[enHVSP_ID].u32ScalingRatio_H, gstScalinInfo[enHVSP_ID].u32ScalingRatio_V);

    // horizotnal HVSP Scaling
    if(gstScalinInfo[enHVSP_ID].stSizeAfterCrop.u16Width == gstScalinInfo[enHVSP_ID].stSizeAfterScaling.u16Width)
    {
        _DrvSclHvspSetHorizotnalScalingConfig(enHVSP_ID, FALSE);
    }
    else
    {
        _DrvSclHvspSetHorizotnalScalingConfig(enHVSP_ID, TRUE);
        _DrvSclHvspSetHorizotnalSramTbl(enHVSP_ID);
    }

    // vertical HVSP Scaling
    if(gstScalinInfo[enHVSP_ID].stSizeAfterCrop.u16Height == gstScalinInfo[enHVSP_ID].stSizeAfterScaling.u16Height)
    {
        _DrvSclHvspSetVerticalScalingConfig(enHVSP_ID, FALSE);
    }
    else
    {
        _DrvSclHvspSetVerticalScalingConfig(enHVSP_ID, TRUE);
        _DrvSclHvspSetVerticalSramTbl(enHVSP_ID);
    }

    // HVSP In size
    HalSclHvspSetHVSPInputSize(enHVSP_ID,
                                 gstScalinInfo[enHVSP_ID].stSizeAfterCrop.u16Width,
                                 gstScalinInfo[enHVSP_ID].stSizeAfterCrop.u16Height);

    // HVSP Out size
    HalSclHvspSetHVSPOutputSize(enHVSP_ID,
                                  gstScalinInfo[enHVSP_ID].stSizeAfterScaling.u16Width,
                                  gstScalinInfo[enHVSP_ID].stSizeAfterScaling.u16Height);
    if(enHVSP_ID == E_DRV_SCLHVSP_ID_3)
    {
        if(!gbclkforcemode)
        {
            HalSclHvspFclk2(stclk);
        }
    }
    else
    {
        if(!gbclkforcemode)
        {
            HalSclHvspFclk1(stclk);
        }
    }
    if(ISZOOMDROPFRAME)
    {
        _DrvSclHvspSetCmdqTrigFire(stCfg.stCmdTrigCfg,enHVSP_ID);
        if(_IsChangeFBBufferResolution(u16FBWidth,u16FBHeight) || bChangePreCropPosition)
        {
            //for 3DNR
            DrvSclIrqSetDropFrameFromCmdqDone(1);
        }
    }
    else
    {
        if(_IsChangeFBBufferResolution(u16FBWidth,u16FBHeight) || bChangePreCropPosition)
        {
            if(u32bLDCEvent)
            {
                SCL_DBG(SCL_DBG_LV_DRVHVSP()&(Get_DBGMG_HVSP(enHVSP_ID)),"[DRVHVSP]Without Drop frame LDC close\n");
                HalSclHvspSetResetDNR(1);
                _DrvSclHvspSetCmdqTrigFire(stCfg.stCmdTrigCfg,enHVSP_ID);
                DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(), E_DRV_SCLIRQ_EVENT_CMDQDONE, &u32bLDCEvent, E_DRV_SCLOS_EVENT_MD_OR, 100); // get status: CMDQ Done
                HalSclHvspSetLdcPathSel(1);
            }
            else
            {
                SCL_DBG(SCL_DBG_LV_DRVHVSP()&(Get_DBGMG_HVSP(enHVSP_ID)),"[DRVHVSP]Without Drop frame \n");
                HalSclHvspSetResetDNR(1);
            }
        }
        _DrvSclHvspSetCmdqTrigFire(stCfg.stCmdTrigCfg,enHVSP_ID);
    }
    DRV_HVSP_MUTEX_UNLOCK();
    stRet.bRet = 1;
    return stRet;
}
void DrvSclHvspSetInputSrcSize(u16 u16Height,u16 u16Width)
{
    if(u16Height > 0)
    {
        gstSrcSize.u16Height    = u16Height;
    }
    if(u16Width > 0)
    {
        gstSrcSize.u16Width     = u16Width;
    }
    gstSrcSize.bSet = 1;
    HalSclHvspSetInputSrcSize(gstSrcSize);
}
void DrvSclHvspSetCropWindowSize(void)
{
    HalSclHvspCropInfo_t stCropInfo_1;
    DrvSclHvspCmdTrigConfig_t stCmdTrigCfg;
    stCropInfo_1 = _Drv_HVSP_FillPreCropInfo();
    DRV_HVSP_MUTEX_LOCK();
    stCmdTrigCfg = DrvSclHvspSetCmdqTrigTypeByRIU();
    HalSclHvspSetCropConfig(E_DRV_SCLHVSP_CROP_ID_1, stCropInfo_1);
    DrvSclHvspSetCmdqTrigType(stCmdTrigCfg);
    DRV_HVSP_MUTEX_UNLOCK();
    SCL_DBG(SCL_DBG_LV_DRVHVSP()&(Get_DBGMG_HVSP(0)), "[DRVHVSP]%s Input:(%hd,%hd) Crop:(%hd,%hd)\n", __FUNCTION__,
        stCropInfo_1.u16In_hsize,stCropInfo_1.u16In_vsize,stCropInfo_1.u16Hsize,stCropInfo_1.u16Vsize);
}

u32 DrvSclHvspGetInputSrcMux(void)
{
    return HalSclHvspGetInputSrcMux();
}

bool DrvSclHvspSetInputMux(DrvSclHvspIpMuxType_e enIP,DrvSclHvspClkConfig_t* stclk)
{
    DRV_HVSP_DBG(sclprintf("[DRVHVSP]%s(%d): IP=%x\n", __FUNCTION__,  __LINE__,enIP));
    SCL_DBG(SCL_DBG_LV_DRVHVSP()&(Get_DBGMG_HVSP(0)), "[DRVHVSP]%s(%d): IP=%x\n", __FUNCTION__,  __LINE__,enIP);
    HalSclHvspSetInputMuxType(enIP);
    if(!gbclkforcemode && stclk != NULL)
    {
        HalSclHvspSetIdClkOnOff(1,stclk);
    }
    if(enIP >= E_DRV_SCLHVSP_IP_MUX_MAX)
    {
        DRV_HVSP_ERR(sclprintf("[DRVHVSP]%s(%d):: Wrong IP Type\n", __FUNCTION__, __LINE__));
        return FALSE;
    }

    HalSclHvspSetHwInputMux(enIP);
    return TRUE;
}

bool DrvSclHvspSetRegisterForce(u32 u32Reg, u8 u8Val, u8 u8Msk)
{
    HalSclHvspSetReg(u32Reg, u8Val, u8Msk);
    return TRUE;
}
void DrvSclHvspSetSCIQVSRAM(DrvSclHvspIdType_e enHVSP_ID)
{
    SCL_DBG(SCL_DBG_LV_DRVHVSP()&(Get_DBGMG_HVSP(enHVSP_ID)),
        "[DRVHVSP]Doing V Qmap SRAM id:%d scaling UP:%hhx\n",enHVSP_ID,gbVScalingup[enHVSP_ID]);
    HalSclHvspSramDump(enHVSP_ID+HVSP_ID_SRAM_V_OFFSET,gbVScalingup[enHVSP_ID]); //level 1 :up 0:down
}
void DrvSclHvspSetSCIQHSRAM(DrvSclHvspIdType_e enHVSP_ID)
{
    SCL_DBG(SCL_DBG_LV_DRVHVSP()&(Get_DBGMG_HVSP(enHVSP_ID)),
        "[DRVHVSP]Doing H Qmap SRAM id:%d scaling UP:%hhx\n",enHVSP_ID,gbHScalingup[enHVSP_ID]);
    HalSclHvspSramDump(enHVSP_ID+HVSP_ID_SRAM_H_OFFSET,gbHScalingup[enHVSP_ID]); //level 1 :up 0:down
}
void DrvSclHvspSetOsdConfig(DrvSclHvspIdType_e enID, DrvSclHvspOsdConfig_t stOSdCfg)
{
    HalSclHvspSetOsdLocate(enID,stOSdCfg.enOSD_loc);
    HalSclHvspSetOsdOnOff(enID,stOSdCfg.stOsdOnOff.bOSDEn);
    HalSclHvspSetOsdBypass(enID,stOSdCfg.stOsdOnOff.bOSDBypass);
    HalSclHvspSetOsdBypassWTM(enID,stOSdCfg.stOsdOnOff.bWTMBypass);
}
void _DrvSclHvspSetPriMaskBufferByByte(u8 * pu8Addr,u8 u8MaskVal)
{
   *pu8Addr =  u8MaskVal;
}
void _Drv_HVSP_SetPriMaskBufferByBit(u8 * pu8Addr,u8 u8MaskVal,u8 mask)
{
    u8 u8val;
    u8val = *pu8Addr;
   *pu8Addr =  ((u8val&mask)|u8MaskVal);
}
u8 _DrvSclHvspGetPriMaskBufferByByte(u8 * pu8Addr)
{
   u8 u8val;
   u8val = *pu8Addr;
   return u8val;
}
u8 * _DrvSclHvspGetPriMaskStartPoint(u16 u16X,u16 u16Y)
{
    u8 *pu8StartPoint;
    u16 u16newX;
    u16 u16newY;
    u16newX = u16X/HVSP_PRIMASK_BLOCK_SIZE; // bit/shift,ex.300 ->300/32 = 9.xx ->bit9 start(9th end is 288 10th start is 289)
    u16newY = u16Y/HVSP_PRIMASK_BLOCK_SIZE; // bit/shift (line)
    pu8StartPoint = gpu8PriMaskBuf + (HVSP_PRIMASK_Width_SIZE*(u16newY))+(u16newX/BYTESIZE);
    SCL_DBG(SCL_DBG_LV_DRVHVSP()&(Get_DBGMG_HVSP(0)), "[DRVHVSP]%s u16newX = %hd,u16newY = %hd,pu8StartPoint = %lx\n"
        ,__FUNCTION__, u16newX,u16newY,(u32)pu8StartPoint);
    return pu8StartPoint;
}
u16 _DrvSclHvspGetPriMaskOffsetBit(u16 u16P)
{
    u16 u16Offsetbit;
    u16 u16Xbyte;
    u16Offsetbit = u16P/HVSP_PRIMASK_BLOCK_SIZE; // bit/shift,ex.300 ->300/32 = 9.xx ->bit9 start(9th end is 288 10th start is 289)
    u16Xbyte = u16Offsetbit/BYTESIZE;
    u16Offsetbit = u16Offsetbit - (u16Xbyte*BYTESIZE);
    SCL_DBG(SCL_DBG_LV_DRVHVSP()&(Get_DBGMG_HVSP(0)), "[DRVHVSP]%s offset = %hd\n",__FUNCTION__, u16Offsetbit);
    return u16Offsetbit;
}
u16 _DrvSclHvspGetPriMaskXOffset
    (u16 u16P,u16 u16Leng)
{
    u16 u16Offeset;
    u16 u16Offsetnow;
    u16 u16lastOffeset;
    u16 u16XOffset=0;
    u16Offeset = u16Leng;//256
    u16Offsetnow = (u16P/HVSP_PRIMASK_BLOCK_SIZE);// 4
    u16Offsetnow = u16Offsetnow/BYTESIZE;// 0
    u16lastOffeset = ((u16Offeset)/HVSP_PRIMASK_BLOCK_SIZE);// 8
    u16lastOffeset = u16lastOffeset/BYTESIZE;// 1
    if(u16Offeset%(BYTESIZE*HVSP_PRIMASK_BLOCK_SIZE))
    {
        u16XOffset = u16lastOffeset-u16Offsetnow+1;// 2
    }
    else
    {
        u16XOffset = u16lastOffeset-u16Offsetnow;// 1 full byte
    }
    SCL_DBG(SCL_DBG_LV_DRVHVSP()&(Get_DBGMG_HVSP(0)), "[DRVHVSP]%s offset = %hd\n",__FUNCTION__, u16XOffset);
    return u16XOffset;
}
u16 _DrvSclHvspGetPriMaskOffset(u16 u16Leng)
{
    u16 u16newOffeset;
    u16newOffeset = u16Leng/HVSP_PRIMASK_BLOCK_SIZE;
    if(((u16Leng)%HVSP_PRIMASK_BLOCK_SIZE)>(HVSP_PRIMASK_BLOCK_SIZE/2))
    {
        u16newOffeset+=1;
    }
    SCL_DBG(SCL_DBG_LV_DRVHVSP()&(Get_DBGMG_HVSP(0)), "[DRVHVSP]%s offset = %hd\n",__FUNCTION__, u16newOffeset);
    return u16newOffeset;
}
void DrvSclHvspSetPriMaskConfig(DrvSclHvspPriMaskConfig_t stCfg)
{
    u8 *pu8StartPoint;
    u16 u16X;//x
    u16 u16Y;//x
    u16 u16Xend;//x+width
    u16 u16XStartoffsetbit;//first byte
    u16 u16XEndoffsetbit;//last byte
    u16 u16Xoffset;
    u16 u16Yoffset;
    u16 u16idx;
    u16 u16idy;
    u8 u8mask;
    u8 u8orimask;
    u16X = ((stCfg.u16X/HVSP_PRIMASK_BLOCK_SIZE)*HVSP_PRIMASK_BLOCK_SIZE);
    if((stCfg.u16X%HVSP_PRIMASK_BLOCK_SIZE)>(HVSP_PRIMASK_BLOCK_SIZE/2))
    {
        u16X+=HVSP_PRIMASK_BLOCK_SIZE;
    }
    u16Y = ((stCfg.u16Y/HVSP_PRIMASK_BLOCK_SIZE)*HVSP_PRIMASK_BLOCK_SIZE);
    if((stCfg.u16Y%HVSP_PRIMASK_BLOCK_SIZE)>(HVSP_PRIMASK_BLOCK_SIZE/2))
    {
        u16Y+=HVSP_PRIMASK_BLOCK_SIZE;
    }
    u16Xend = (((stCfg.u16X+stCfg.u16Width)/HVSP_PRIMASK_BLOCK_SIZE)*HVSP_PRIMASK_BLOCK_SIZE);
    if(((stCfg.u16X+stCfg.u16Width)%HVSP_PRIMASK_BLOCK_SIZE)>(HVSP_PRIMASK_BLOCK_SIZE/2))
    {
        u16Xend+=HVSP_PRIMASK_BLOCK_SIZE;
    }
    pu8StartPoint = _DrvSclHvspGetPriMaskStartPoint(u16X,u16Y);//byte/unit
    u16XStartoffsetbit = _DrvSclHvspGetPriMaskOffsetBit(u16X);// bit/unit
    u16XEndoffsetbit = _DrvSclHvspGetPriMaskOffsetBit(u16Xend);// bit/unit
    if(u16XEndoffsetbit)
    {
        u16XEndoffsetbit = BYTESIZE-u16XEndoffsetbit;
    }
    u16Xoffset = _DrvSclHvspGetPriMaskXOffset(u16X,u16Xend);//byte/unit
    u16Yoffset = _DrvSclHvspGetPriMaskOffset(stCfg.u16Height);// bit/unit
    for(u16idy = 0;u16idy<u16Yoffset;u16idy++)
    {
        for(u16idx = 0;u16idx<(u16Xoffset);u16idx++)
        {
            u8mask = (stCfg.bMask) ? 0xFF :0;
            if(Is_StartAndEndInSameByte(u16XStartoffsetbit,u16idx,u16Xoffset))
            {
                u8orimask = _DrvSclHvspGetPriMaskBufferByByte(pu8StartPoint+(u16idy*HVSP_PRIMASK_Width_SIZE));
                if(stCfg.bMask)
                {
                    u8mask = u8orimask | ((0xFF>>u16XEndoffsetbit)&(0xFF<<u16XStartoffsetbit));
                }
                else
                {
                    u8mask = (u8orimask&(~((0xFF>>u16XEndoffsetbit)&(0xFF<<u16XStartoffsetbit))));
                }
            }
            else if(Is_StartByteOffsetBit(u16XStartoffsetbit,u16idx))
            {
                u8orimask = _DrvSclHvspGetPriMaskBufferByByte(pu8StartPoint+(u16idy*HVSP_PRIMASK_Width_SIZE));
                if(stCfg.bMask)
                {
                    u8mask = u8orimask | (u8mask&(0xFF<<u16XStartoffsetbit));
                }
                else
                {
                    u8mask = (u8orimask&(~(0xFF<<u16XStartoffsetbit)));
                }
            }
            else if(Is_EndByteOffsetBit(u16XEndoffsetbit,u16Xoffset,u16idx))
            {
                u8orimask = _DrvSclHvspGetPriMaskBufferByByte(pu8StartPoint+u16idx+(u16idy*HVSP_PRIMASK_Width_SIZE));
                if(stCfg.bMask)
                {
                    u8mask = u8orimask | (u8mask&(0xFF>>(u16XEndoffsetbit)));
                }
                else
                {
                    u8mask = u8orimask &(~(0xFF>>(u16XEndoffsetbit)));
                }
            }
            _DrvSclHvspSetPriMaskBufferByByte(pu8StartPoint+u16idx+(u16idy*HVSP_PRIMASK_Width_SIZE),u8mask);
        }
    }
}
u16 DrvSclHvspPriMaskGetSram(u8 u8idx, u8 u8idy)
{
    u16 u16val = 0;
    u8* p8valpoint;
    p8valpoint = (u8*)gpu8PriMaskBuf;
    u16val = 0;
    if(u8idx==(HVSP_PRIMASK_Width_SIZE/2))
    {
        u16val = (*(p8valpoint+(u8idx*2)+(u8idy*HVSP_PRIMASK_Width_SIZE)));
    }
    else
    {
        u16val = (*(p8valpoint+(u8idx*2)+(u8idy*HVSP_PRIMASK_Width_SIZE))|
            (*(p8valpoint+(u8idx*2)+1+(u8idy*HVSP_PRIMASK_Width_SIZE)))<<8);
    }
    return u16val;
}
void _DrvSclHvspPriMaskFillSram(void)
{
    u8 u8idx = 0;
    u8 u8idy = 0;
    u16 u16val = 0;
    u8* p8valpoint;
    p8valpoint = (u8*)gpu8PriMaskBuf;
    DrvSclOsWaitForCpuWriteToDMem();
    _Drv_HVSP_PriMaskInit();
    DRV_HVSP_MUTEX_LOCK();
    DrvSclCmdqGetModuleMutex(E_DRV_SCLCMDQ_TYPE_IP0,1);
    for(u8idy = 0;u8idy<HVSP_PRIMASK_Height_SIZE;u8idy++)
    {
        //HalSclHvspPriMaskSramWriteIdx(u8idy);
        SCL_DBG(SCL_DBG_LV_DRVHVSP()&(Get_DBGMG_HVSP(0)), "[DRVHVSP]%s",__FUNCTION__);
        for(u8idx =0;u8idx<=(HVSP_PRIMASK_Width_SIZE/2);u8idx++)
        {
            u16val = 0;
            if(u8idx==(HVSP_PRIMASK_Width_SIZE/2))
            {
                u16val = (*(p8valpoint+(u8idx*2)+(u8idy*HVSP_PRIMASK_Width_SIZE)));
            }
            else
            {
                u16val = (*(p8valpoint+(u8idx*2)+(u8idy*HVSP_PRIMASK_Width_SIZE))|
                    (*(p8valpoint+(u8idx*2)+1+(u8idy*HVSP_PRIMASK_Width_SIZE)))<<8);
            }
            HalSclHvspPriMaskSetSram(u8idx,u16val);
            SCL_DBG(SCL_DBG_LV_DRVHVSP()&(Get_DBGMG_HVSP(0)), " val:%hx,",u16val);
        }
        SCL_DBG(SCL_DBG_LV_DRVHVSP()&(Get_DBGMG_HVSP(0)), "idy:%hhd\n",u8idy);
        HalSclHvspPriMaskSramEn(1,u8idy);
    }
    HalSclHvspPriMaskFire();
    DrvSclCmdqGetModuleMutex(E_DRV_SCLCMDQ_TYPE_IP0,0);
    DRV_HVSP_MUTEX_UNLOCK();
}
void DrvSclHvspSetPriMaskTrigger(DrvSclHvspPriMaskType_e enType)
{
    //Fill SRAM
    //clk open
    u16 u16clkreg;
    u16clkreg = HalSclHvspGet2ByteReg(REG_SCL_CLK_64_L);
    HalSclHvspSet2ByteReg(REG_SCL_CLK_64_L,0x0,0xFFFF);
    if(enType == E_DRV_SCLHVSP_PRIMASK_DISABLE)
    {
        DrvSclHvspPriMaskBufferReset();
    }
    _DrvSclHvspPriMaskFillSram();
    //TRIGGER
    if(enType == E_DRV_SCLHVSP_PRIMASK_DISABLE || enType == E_DRV_SCLHVSP_PRIMASK_ONLYHWOFF)
    {
        HalSclHvspPriMaskbEn(E_HAL_SCLHVSP_TRIGGER_OFF);
    }
    else if(enType == E_DRV_SCLHVSP_PRIMASK_ENABLE)
    {
        HalSclHvspPriMaskbEn(E_HAL_SCLHVSP_TRIGGER_ON);
    }
    else if (enType == E_DRV_SCLHVSP_PRIMASK_PENDING)
    {
        gbPriMaskPending = 1;
    }
    //clk close
    HalSclHvspSet2ByteReg(REG_SCL_CLK_64_L,u16clkreg,0xFFFF);
}
void DrvSclHvspSetPrv2CropOnOff(u8 bEn)
{
    HalSclHvspSetPrv2CropOnOff(bEn);
}
DrvSclHvspScIntsType_t DrvSclHvspGetSclInts(void)
{
    DrvSclIrqScIntsType_t stints;
    DrvSclHvspScIntsType_t sthvspints;
    stints = DrvSclIrqGetSclInts();
    DrvSclOsMemcpy(&sthvspints,&stints,sizeof(DrvSclHvspScIntsType_t));
    return sthvspints;
}
void DrvSclHvspSclIq(DrvSclHvspIdType_e enID,DrvSclHvspIqType_e enIQ)
{
    DrvSclHvspCmdTrigConfig_t stCmdTrigCfg;
    switch(enIQ)
    {
        case E_DRV_SCLHVSP_IQ_H_Tbl0:
            DRV_HVSP_MUTEX_LOCK();
            stCmdTrigCfg = DrvSclHvspSetCmdqTrigTypeByRIU();
            HalSclHvspSetModeYHo(enID,E_HAL_SCLHVSP_FILTER_MODE_SRAM_0);
            DrvSclHvspSetCmdqTrigType(stCmdTrigCfg);
            DRV_HVSP_MUTEX_UNLOCK();
            HalSclHvspSramDump(enID+HVSP_ID_SRAM_H_OFFSET,gbHScalingup[enID]&0x1); //level 1 :up 0:down
            gbHScalingup[enID] = gbHScalingup[enID]&0x1;
        break;
        case E_DRV_SCLHVSP_IQ_H_Tbl1:
            DRV_HVSP_MUTEX_LOCK();
            stCmdTrigCfg = DrvSclHvspSetCmdqTrigTypeByRIU();
            HalSclHvspSetModeYHo(enID,E_HAL_SCLHVSP_FILTER_MODE_SRAM_0);
            DrvSclHvspSetCmdqTrigType(stCmdTrigCfg);
            DRV_HVSP_MUTEX_UNLOCK();
            HalSclHvspSramDump(enID+HVSP_ID_SRAM_H_OFFSET,(0x10 |(gbHScalingup[enID]&0x1))); //level 1 :up 0:down
            gbHScalingup[enID] = (0x10 |(gbHScalingup[enID]&0x1));
        break;
        case E_DRV_SCLHVSP_IQ_H_Tbl2:
            DRV_HVSP_MUTEX_LOCK();
            stCmdTrigCfg = DrvSclHvspSetCmdqTrigTypeByRIU();
            HalSclHvspSetModeYHo(enID,E_HAL_SCLHVSP_FILTER_MODE_SRAM_0);
            DrvSclHvspSetCmdqTrigType(stCmdTrigCfg);
            DRV_HVSP_MUTEX_UNLOCK();
            HalSclHvspSramDump(enID+HVSP_ID_SRAM_H_OFFSET,(0x20 |(gbHScalingup[enID]&0x1))); //level 1 :up 0:down
            gbHScalingup[enID] = (0x20 |(gbHScalingup[enID]&0x1));
        break;
        case E_DRV_SCLHVSP_IQ_H_Tbl3:
            DRV_HVSP_MUTEX_LOCK();
            stCmdTrigCfg = DrvSclHvspSetCmdqTrigTypeByRIU();
            HalSclHvspSetModeYHo(enID,E_HAL_SCLHVSP_FILTER_MODE_SRAM_0);
            DrvSclHvspSetCmdqTrigType(stCmdTrigCfg);
            DRV_HVSP_MUTEX_UNLOCK();
            HalSclHvspSramDump(enID+HVSP_ID_SRAM_H_OFFSET,(0x30 |(gbHScalingup[enID]&0x1))); //level 1 :up 0:down
            gbHScalingup[enID] = (0x30 |(gbHScalingup[enID]&0x1));
        break;
        case E_DRV_SCLHVSP_IQ_H_BYPASS:
            DRV_HVSP_MUTEX_LOCK();
            stCmdTrigCfg = DrvSclHvspSetCmdqTrigTypeByRIU();
            HalSclHvspSetModeYHo(enID,E_HAL_SCLHVSP_FILTER_MODE_BYPASS);
            DrvSclHvspSetCmdqTrigType(stCmdTrigCfg);
            DRV_HVSP_MUTEX_UNLOCK();
        break;
        case E_DRV_SCLHVSP_IQ_H_BILINEAR:
            DRV_HVSP_MUTEX_LOCK();
            stCmdTrigCfg = DrvSclHvspSetCmdqTrigTypeByRIU();
            HalSclHvspSetModeYHo(enID,E_HAL_SCLHVSP_FILTER_MODE_BILINEAR);
            DrvSclHvspSetCmdqTrigType(stCmdTrigCfg);
            DRV_HVSP_MUTEX_UNLOCK();
        break;
        case E_DRV_SCLHVSP_IQ_V_Tbl0:
            DRV_HVSP_MUTEX_LOCK();
            stCmdTrigCfg = DrvSclHvspSetCmdqTrigTypeByRIU();
            HalSclHvspSetModeYVe(enID,E_HAL_SCLHVSP_FILTER_MODE_SRAM_0);
            DrvSclHvspSetCmdqTrigType(stCmdTrigCfg);
            DRV_HVSP_MUTEX_UNLOCK();
            HalSclHvspSramDump(enID+HVSP_ID_SRAM_V_OFFSET,gbVScalingup[enID]&0x1); //level 1 :up 0:down
            gbVScalingup[enID] = gbVScalingup[enID]&0x1;
        break;
        case E_DRV_SCLHVSP_IQ_V_Tbl1:
            DRV_HVSP_MUTEX_LOCK();
            stCmdTrigCfg = DrvSclHvspSetCmdqTrigTypeByRIU();
            HalSclHvspSetModeYVe(enID,E_HAL_SCLHVSP_FILTER_MODE_SRAM_0);
            DrvSclHvspSetCmdqTrigType(stCmdTrigCfg);
            DRV_HVSP_MUTEX_UNLOCK();
            HalSclHvspSramDump(enID+HVSP_ID_SRAM_V_OFFSET,(0x10 |(gbVScalingup[enID]&0x1))); //level 1 :up 0:down
            gbVScalingup[enID] = (0x10 |(gbVScalingup[enID]&0x1));
        break;
        case E_DRV_SCLHVSP_IQ_V_Tbl2:
            DRV_HVSP_MUTEX_LOCK();
            stCmdTrigCfg = DrvSclHvspSetCmdqTrigTypeByRIU();
            HalSclHvspSetModeYVe(enID,E_HAL_SCLHVSP_FILTER_MODE_SRAM_0);
            DrvSclHvspSetCmdqTrigType(stCmdTrigCfg);
            DRV_HVSP_MUTEX_UNLOCK();
            HalSclHvspSramDump(enID+HVSP_ID_SRAM_V_OFFSET,(0x20 |(gbVScalingup[enID]&0x1))); //level 1 :up 0:down
            gbVScalingup[enID] = (0x20 |(gbVScalingup[enID]&0x1));
        break;
        case E_DRV_SCLHVSP_IQ_V_Tbl3:
            DRV_HVSP_MUTEX_LOCK();
            stCmdTrigCfg = DrvSclHvspSetCmdqTrigTypeByRIU();
            HalSclHvspSetModeYVe(enID,E_HAL_SCLHVSP_FILTER_MODE_SRAM_0);
            DrvSclHvspSetCmdqTrigType(stCmdTrigCfg);
            DRV_HVSP_MUTEX_UNLOCK();
            HalSclHvspSramDump(enID+HVSP_ID_SRAM_V_OFFSET,(0x30 |(gbVScalingup[enID]&0x1))); //level 1 :up 0:down
            gbVScalingup[enID] = (0x30 |(gbVScalingup[enID]&0x1));
        break;
        case E_DRV_SCLHVSP_IQ_V_BYPASS:
            DRV_HVSP_MUTEX_LOCK();
            stCmdTrigCfg = DrvSclHvspSetCmdqTrigTypeByRIU();
            HalSclHvspSetModeYVe(enID,E_HAL_SCLHVSP_FILTER_MODE_BYPASS);
            DrvSclHvspSetCmdqTrigType(stCmdTrigCfg);
            DRV_HVSP_MUTEX_UNLOCK();
        break;
        case E_DRV_SCLHVSP_IQ_V_BILINEAR:
            DRV_HVSP_MUTEX_LOCK();
            stCmdTrigCfg = DrvSclHvspSetCmdqTrigTypeByRIU();
            HalSclHvspSetModeYVe(enID,E_HAL_SCLHVSP_FILTER_MODE_BILINEAR);
            DrvSclHvspSetCmdqTrigType(stCmdTrigCfg);
            DRV_HVSP_MUTEX_UNLOCK();
        break;
        default:
        break;
    }
}
bool DrvSclHvspGetSCLInform(DrvSclHvspIdType_e enID,DrvSclHvspScInformConfig_t *stInformCfg)
{
    stInformCfg->u16X               = HalSclHvspGetCrop2Xinfo();
    stInformCfg->u16Y               = HalSclHvspGetCrop2Yinfo();
    stInformCfg->u16Width           = HalSclHvspGetHvspOutputWidth(enID);
    stInformCfg->u16Height          = HalSclHvspGetHvspOutputHeight(enID);
    stInformCfg->u16crop2inWidth    = HalSclHvspGetCrop2InputWidth();
    stInformCfg->u16crop2inHeight   = HalSclHvspGetCrop2InputHeight();
    stInformCfg->u16crop2OutWidth   = HalSclHvspGetCrop2OutputWidth();
    stInformCfg->u16crop2OutHeight  = HalSclHvspGetCrop2OutputHeight();
    stInformCfg->bEn                = HalSclHvspGetCrop2En();
    return TRUE;
}
bool DrvSclHvspGetHvspAttribute(DrvSclHvspIdType_e enID,DrvSclHvspInformConfig_t *stInformCfg)
{
    stInformCfg->u16Width           = HalSclHvspGetHvspOutputWidth(enID);
    stInformCfg->u16Height          = HalSclHvspGetHvspOutputHeight(enID);
    stInformCfg->u16inWidth         = HalSclHvspGetHvspInputWidth(enID);
    stInformCfg->u16inHeight        = HalSclHvspGetHvspInputHeight(enID);
    stInformCfg->bEn                = HalSclHvspGetScalingFunctionStatus(enID);
    stInformCfg->bEn |= (gbVScalingup[enID]&0xF0);
    stInformCfg->bEn |= ((gbHScalingup[enID]&0xF0)<<2);
    return TRUE;
}
void DrvSclHvspGetOsdAttribute(DrvSclHvspIdType_e enID,DrvSclHvspOsdConfig_t *stOsdCfg)
{
    stOsdCfg->enOSD_loc = HalSclHvspGetOsdLocate(enID);
    stOsdCfg->stOsdOnOff.bOSDEn = HalSclHvspGetOsdOnOff(enID);
    stOsdCfg->stOsdOnOff.bOSDBypass = HalSclHvspGetOsdBypass(enID);
    stOsdCfg->stOsdOnOff.bWTMBypass = HalSclHvspGetOsdBypassWTM(enID);
}
bool DrvSclHvspGetFrameBufferAttribute(DrvSclHvspIdType_e enID,DrvSclHvspIpmConfig_t *stInformCfg)
{
    bool bLDCorPrvCrop;
    stInformCfg->bYCMWrite = gstIPMCfg.bYCMWrite;
    stInformCfg->u16Fetch = gstIPMCfg.u16Fetch;
    stInformCfg->u16Vsize = gstIPMCfg.u16Vsize;
    stInformCfg->u32YCBaseAddr = gstIPMCfg.u32YCBaseAddr+0x20000000;
    stInformCfg->u32MemSize = gstIPMCfg.u32MemSize;
    stInformCfg->bYCMRead = gstIPMCfg.bYCMRead;
    bLDCorPrvCrop = (HalSclHvspGetLdcPathSel()) ? 1 :
                    (HalSclHvspGetPrv2CropOnOff())? 2 : 0;
    return bLDCorPrvCrop;
}
bool DrvSclHvspSetLDCFrameBuffer_Config(DrvSclHvspLdcFrameBufferConfig_t stLDCCfg)
{
    DrvSclHvspCmdTrigConfig_t stCmdTrigCfg;
    SCL_DBG(SCL_DBG_LV_DRVHVSP()&(Get_DBGMG_HVSP(0)), "[DRVHVSP]%s(%d): Width=%hx,Height:%x\n", __FUNCTION__,
        __LINE__, stLDCCfg.u16Width,stLDCCfg.u16Height);
    DRV_HVSP_MUTEX_LOCK();
    stCmdTrigCfg = DrvSclHvspSetCmdqTrigTypeByRIU();
    HalSclHvspSetLdcWidth(stLDCCfg.u16Width);
    HalSclHvspSetLdcHeight(stLDCCfg.u16Height);
    HalSclHvspSetLdcBase(stLDCCfg.u32FBaddr);
    HalSclHvspSetLdcHWrwDiff(((u16)stLDCCfg.u8FBrwdiff));
    HalSclHvspSetLdcSwIdx(stLDCCfg.u8FBidx);
    HalSclHvspSetLdcSwMode(stLDCCfg.bEnSWMode);
    HalSclHvspSetLdcBankMode(stLDCCfg.enLDCType);
    HalSclHvspSetLdcPathSel(stLDCCfg.bEnDNR);
    if(!gbLDCEn)
    {
        HalSclHvspSetLdcBypass(1);
    }
    DrvSclHvspSetCmdqTrigType(stCmdTrigCfg);
    DRV_HVSP_MUTEX_UNLOCK();
    return TRUE;
}
void DrvSclHvspSetLdcONOFF(bool bEn)
{
    u32 u32Events;
    static bool bEnPrv2Crop = 0;
    //ToDo
    if(HalSclHvspGetLdcPathSel()!= bEn)
    {
        if(!DrvSclIrqGetIsBlankingRegion())
        {
            DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(), E_DRV_SCLIRQ_EVENT_FRMENDSYNC, &u32Events, E_DRV_SCLOS_EVENT_MD_OR, 100); // get status: FRM END
        }
        if(HalSclHvspGetPrv2CropOnOff() == bEn)
        {
            if(bEnPrv2Crop &&(!bEn))
            {
                HalSclHvspSetPrv2CropOnOff(1);
            }
            else
            {
                HalSclHvspSetPrv2CropOnOff(0);
            }
            bEnPrv2Crop = bEn;
        }
        HalSclHvspSetLdcPathSel(bEn);
    }
    gbLDCEn = bEn;
}
bool DrvSclHvspSetPatTgen(bool bEn, DrvSclHvspPatTgenConfig_t *pCfg)
{
    u16 u16VSync_St, u16HSync_St;
    bool bRet = TRUE;
    DrvSclIrqSetPTGenStatus(bEn);
    if(bEn)
    {
        u16VSync_St = 1;
        u16HSync_St = 0;
        if(pCfg)
        {
            if((u16VSync_St + pCfg->u16VSyncWidth + pCfg->u16VBackPorch + pCfg->u16VActive - 1 )<1125)
            {
                HalSclHvspSetPatTgVtt(3000); //scaling up need bigger Vtt, , using vtt of 1920x1080 for all timing
            }
            else
            {
                HalSclHvspSetPatTgVtt(3000); //rotate
            }
            HalSclHvspSetPatTgVsyncSt(u16VSync_St);
            HalSclHvspSetPatTgVsyncEnd(u16VSync_St + pCfg->u16VSyncWidth - 1);
            HalSclHvspSetPatTgVdeSt(u16VSync_St + pCfg->u16VSyncWidth + pCfg->u16VBackPorch);
            HalSclHvspSetPatTgVdeEnd(u16VSync_St + pCfg->u16VSyncWidth + pCfg->u16VBackPorch + pCfg->u16VActive - 1);
            HalSclHvspSetPatTgVfdeSt(u16VSync_St + pCfg->u16VSyncWidth + pCfg->u16VBackPorch);
            HalSclHvspSetPatTgVfdeEnd(u16VSync_St + pCfg->u16VSyncWidth + pCfg->u16VBackPorch + pCfg->u16VActive - 1);

            HalSclHvspSetPatTgHtt(3200); // scaling up need bigger Vtt, , using vtt of 1920x1080 for all timing
            HalSclHvspSetPatTgHsyncSt(u16HSync_St);
            HalSclHvspSetPatTgHsyncEnd(u16HSync_St + pCfg->u16HSyncWidth - 1);
            HalSclHvspSetPatTgHdeSt(u16HSync_St + pCfg->u16HSyncWidth + pCfg->u16HBackPorch);
            HalSclHvspSetPatTgHdeEnd(u16HSync_St + pCfg->u16HSyncWidth + pCfg->u16HBackPorch + pCfg->u16HActive - 1);
            HalSclHvspSetPatTgHfdeSt(u16HSync_St + pCfg->u16HSyncWidth + pCfg->u16HBackPorch);
            HalSclHvspSetPatTgHfdeEnd(u16HSync_St + pCfg->u16HSyncWidth + pCfg->u16HBackPorch + pCfg->u16HActive - 1);

            HalSclHvspSetPatTgEn(TRUE);
            bRet = TRUE;
        }
        else
        {
            bRet = FALSE;
        }
    }
    else
    {
        HalSclHvspSetPatTgEn(FALSE);
        bRet = TRUE;
    }
    return bRet;
}
void DrvSclHvspIdclkRelease(DrvSclHvspClkConfig_t* stclk)
{
    if(!gbclkforcemode)
    HalSclHvspSetIdClkOnOff(0,stclk);
}
u32 DrvSclHvspCmdqStatusReport(void)
{
    u32 u32Reg;
    static u32 u32savereg;
    u32Reg = HalSclHvspGetCmdqStatus();
    if(u32savereg == u32Reg)
    {
        return 0;
    }
    else
    {
        u32savereg = u32Reg;
        return u32Reg ;
    }
}
void DrvSclHvspGetCrop12Inform(DrvSclHvspInputInformConfig_t *stInformCfg)
{
    stInformCfg->bEn = HalSclHvspGetCrop1En();
    stInformCfg->u16inWidth         = HalSclHvspGetCrop1Width();
    stInformCfg->u16inHeight        = HalSclHvspGetCrop1Height();
    stInformCfg->u16inCropWidth         = HalSclHvspGetCrop2InputWidth();
    stInformCfg->u16inCropHeight        = HalSclHvspGetCrop2InputHeight();
    stInformCfg->u16inCropX         = HalSclHvspGetCropX();
    stInformCfg->u16inCropY        = HalSclHvspGetCropY();
    stInformCfg->u16inWidthcount    = HalSclHvspGetCrop1WidthCount();
    if(stInformCfg->u16inWidthcount)
    {
        stInformCfg->u16inWidthcount++;
    }
    stInformCfg->u16inHeightcount   = HalSclHvspGetCrop1HeightCount();
    stInformCfg->enMux              = HalSclHvspGetInputSrcMux();

}
void DrvSclHvspSetClkForcemode(u8 bEn)
{
    gbclkforcemode = bEn;
}
bool DrvSclHvspGetClkForcemode(void)
{
    return gbclkforcemode;
}
void DrvSclHvspSetClkRate(u8 u8Idx)
{
    if(gbclkforcemode)
    {
        u8Idx |= E_HALSCLHVSP_CLKATTR_FORCEMODE;
    }
    HalSclHvspSetClkRate(u8Idx);
}
u8 DrvSclHvspCheckInputVsync(void)
{
        u32 u32Events = 0;
        DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(), E_DRV_SCLIRQ_EVENT_SYNC, &u32Events, E_DRV_SCLOS_EVENT_MD_OR, SCLOS_WAIT_FOREVER); // get status: FRM END
        if(u32Events)
        {
            DrvSclOsClearEventIRQ(DrvSclIrqGetIrqSYNCEventID(),(E_DRV_SCLIRQ_EVENT_SYNC));
            return 1;
        }
        return 0;
}
#undef DRV_HVSP_C
