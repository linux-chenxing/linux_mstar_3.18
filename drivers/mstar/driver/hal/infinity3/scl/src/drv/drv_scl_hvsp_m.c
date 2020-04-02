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
#define _MDRV_HVSP_C
#include "drv_scl_os.h"
#include "drv_scl_dbg.h"
#include "hal_scl_util.h"
#include "drv_scl_hvsp_io_st.h"
#include "drv_scl_hvsp_st.h"
#include "drv_scl_hvsp.h"
#include "drv_scl_hvsp_m.h"
#include "drv_scl_pnl.h"
#include "drv_scl_multiinst_m.h"
//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define PARSING_PAT_TGEN_TIMING(x)  (x == E_MDRV_SCLHVSP_PAT_TGEN_TIMING_1920_1080_  ?  "E_MDRV_SCLHVSP_PAT_TGEN_TIMING_1920_1080_" : \
                                     x == E_MDRV_SCLHVSP_PAT_TGEN_TIMING_1024_768_6   ?  "E_MDRV_SCLHVSP_PAT_TGEN_TIMING_1024_768_6" : \
                                     x == E_MDRV_SCLHVSP_PAT_TGEN_TIMING_640_480_60    ?  "E_MDRV_SCLHVSP_PAT_TGEN_TIMING_640_480_60" : \
                                     x == E_MDRV_SCLHVSP_PAT_TGEN_TIMING_UNDEFINED     ?  "E_MDRV_SCLHVSP_PAT_TGEN_TIMING_UNDEFINED" : \
                                                                                        "UNKNOWN")
#define IS_WRONG_TYPE(enHVSP_ID,enSrcType)((enHVSP_ID == E_MDRV_SCLHVSP_ID_1 && (enSrcType == E_MDRV_SCLHVSP_SRC_DRAM)) ||\
                        (enHVSP_ID == E_MDRV_SCLHVSP_ID_2 && enSrcType != E_MDRV_SCLHVSP_SRC_HVSP) ||\
                        (enHVSP_ID == E_MDRV_SCLHVSP_ID_3 && enSrcType != E_MDRV_SCLHVSP_SRC_DRAM))
#define Is_PTGEN_FHD(u16Htotal,u16Vtotal,u16Vfrequency) ((u16Htotal) == 2200 && (u16Vtotal) == 1125 && (u16Vfrequency) == 30)
#define Is_PTGEN_HD(u16Htotal,u16Vtotal,u16Vfrequency) ((u16Htotal) == 1344 && (u16Vtotal) == 806 && (u16Vfrequency) == 60)
#define Is_PTGEN_SD(u16Htotal,u16Vtotal,u16Vfrequency) ((u16Htotal) == 800 && (u16Vtotal) == 525 && (u16Vfrequency) == 60)
#define IS_HVSPNotOpen(u16Src_Width,u16Dsp_Width) (u16Src_Width == 0 && u16Dsp_Width == 0)
#define IS_NotScalingAfterCrop(bEn,u16Dsp_Height,u16Height,u16Dsp_Width,u16Width) (bEn && \
                        (u16Dsp_Height == u16Height) && (u16Dsp_Width== u16Width))
#define RATIO_CONFIG 512
#define CAL_HVSP_RATIO(input,output) ((u32)((u64)((u32)input * RATIO_CONFIG )/(u32)output))
#define SCALE(numerator, denominator,value) ((u16)((u32)(value * RATIO_CONFIG *  numerator) / (denominator * RATIO_CONFIG)))

#define PRI_MASK_NUM 10
#define PRI_MASK_PERCENT_BASE 10000
#define MDrvSclHvspMutexLock()            DrvSclOsObtainMutex(_MHVSP_Mutex,SCLOS_WAIT_FOREVER)
#define MDrvSclHvspMutexUnlock()          DrvSclOsReleaseMutex(_MHVSP_Mutex)
//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
MDrvSclHvspScalingConfig_t gstHvspScalingCfg;
MDrvSclHvspPostCropConfig_t gstHvspPostCropCfg;
MDrvSclHvspPriMaskConfig_t gstHvspPriMaskCfg[MDRV_SCLMULTI_INST_PRIVATE_ID_NUM][PRI_MASK_NUM];
u8 gu8PriMaskNum[MDRV_SCLMULTI_INST_PRIVATE_ID_NUM];
u8 gu8PriMaskInst;
/////////////////
/// gbMemReadyForMdrv
/// if True ,DNR buffer was allocated.
////////////////
u8 gbMemReadyForMdrv;
u8 gu8IPMBufferNum;
DrvSclHvspPatTgenConfig_t gstPatTgenCfg[E_MDRV_SCLHVSP_PAT_TGEN_TIMING_MAX] =
{
    {1125,  4,  5, 36, 1080, 2200, 88,  44, 148, 1920}, // 1920_1080_30
    { 806,  3,  6, 29,  768, 1344, 24, 136, 160, 1024}, // 1024_768_60
    { 525, 33,  2, 10,  480,  800, 16,  96,  48,  640}, // 640_480_60
    {   0, 20, 10, 20,    0,    0, 30,  15,  30,    0}, // undefined
};
s32 _MHVSP_Mutex = -1;

//-------------------------------------------------------------------------------------------------
//  Function
//-------------------------------------------------------------------------------------------------
bool MDrvSclHvspGetCmdqDoneStatus(MDrvSclHvspIdType_e enHVSP_ID)
{
    DrvSclHvspPollIdType_e enPollId;
    enPollId = enHVSP_ID == E_MDRV_SCLHVSP_ID_2 ? E_DRV_SCLHVSP_POLL_ID_2 :
           enHVSP_ID == E_MDRV_SCLHVSP_ID_3 ? E_DRV_SCLHVSP_POLL_ID_3 :
                                           E_DRV_SCLHVSP_POLL_ID_1;
    return DrvSclHvspGetCmdqDoneStatus(enPollId);
}

void * MDrvSclHvspGetWaitQueueHead(void)
{
    return DrvSclHvspGetWaitQueueHead();
}
bool MDrvSclHvspSetPollWait(DrvSclOsPollWaitConfig_t stPollWait)
{
    return (bool)DrvSclOsSetPollWait(stPollWait);
}
bool MDrvSclHvspSuspend(MDrvSclHvspIdType_e enHVSP_ID, MDrvSclHvspSuspendResumeConfig_t *pCfg)
{
    DrvSclHvspSuspendResumeConfig_t stSuspendResumeCfg;
    bool bRet = TRUE;;

    stSuspendResumeCfg.u32IRQNUM = pCfg->u32IRQNum;
    stSuspendResumeCfg.u32CMDQIRQNUM = pCfg->u32CMDQIRQNum;
    if(DrvSclHvspSuspend(&stSuspendResumeCfg))
    {
        bRet = TRUE;
    }
    else
    {
        SCL_DBG(SCL_DBG_LV_HVSP()&(Get_DBGMG_HVSP(enHVSP_ID)), "[HVSP]%s Suspend Fail\n", __FUNCTION__);
        bRet = FALSE;
    }

    return bRet;
}

bool MDrvSclHvspResume(MDrvSclHvspIdType_e enHVSP_ID, MDrvSclHvspSuspendResumeConfig_t *pCfg)
{
    DrvSclHvspSuspendResumeConfig_t stSuspendResumeCfg;
    bool bRet = TRUE;;

    stSuspendResumeCfg.u32IRQNUM = pCfg->u32IRQNum;
    stSuspendResumeCfg.u32CMDQIRQNUM = pCfg->u32CMDQIRQNum;
    if(DrvSclHvspResume(&stSuspendResumeCfg))
    {
        bRet = TRUE;
    }
    else
    {
        SCL_DBG(SCL_DBG_LV_HVSP()&(Get_DBGMG_HVSP(enHVSP_ID)), "[HVSP]%s Resume Fail\n", __FUNCTION__);
        bRet = FALSE;
    }

    return bRet;
}
void _MDrvSclHvspResetPriMaskStorage(void)
{
    MDrvSclHvspMutexLock();
    DrvSclOsMemset(&gstHvspPriMaskCfg[gu8PriMaskInst], 0, sizeof(MDrvSclHvspPriMaskConfig_t)*PRI_MASK_NUM);
    gu8PriMaskNum[gu8PriMaskInst] = 0;
    MDrvSclHvspMutexUnlock();
}
void _MDrvSclHvspSwInit(void)
{
    u8 i;
    MDrvSclHvspMutexLock();
    DrvSclOsMemset(&gstHvspScalingCfg, 0, sizeof(MDrvSclHvspScalingConfig_t));
    DrvSclOsMemset(&gstHvspPostCropCfg, 0, sizeof(MDrvSclHvspPostCropConfig_t));
    MDrvSclHvspMutexUnlock();
    for(i = 0 ;i <MDRV_SCLMULTI_INST_PRIVATE_ID_NUM;i++)
    {
        MDrvSclHvspMutexLock();
        gu8PriMaskInst = i;
        MDrvSclHvspMutexUnlock();
        _MDrvSclHvspResetPriMaskStorage();
    }
    MDrvSclHvspMutexLock();
    gu8PriMaskInst = 0;
    MDrvSclHvspMutexUnlock();
}
void MDrvSclHvspSetPriMaskInstId(u8 u8Id)
{
    MDrvSclHvspMutexLock();
    gu8PriMaskInst = u8Id;
    MDrvSclHvspMutexUnlock();
}
void MDrvSclHvspRelease(MDrvSclHvspIdType_e enHVSP_ID)
{
    DrvSclHvspIdType_e enID;
    enID = enHVSP_ID == E_MDRV_SCLHVSP_ID_2 ? E_DRV_SCLHVSP_ID_2 :
           enHVSP_ID == E_MDRV_SCLHVSP_ID_3 ? E_DRV_SCLHVSP_ID_3 :
                                           E_DRV_SCLHVSP_ID_1;
    if(enHVSP_ID == E_MDRV_SCLHVSP_ID_1)
    {
        _MDrvSclHvspSwInit();
    }
    DrvSclHvspRelease(enID);
}
void MDrvSclHvspOpen(MDrvSclHvspIdType_e enHVSP_ID)
{
    DrvSclHvspIdType_e enID;
    enID = enHVSP_ID == E_MDRV_SCLHVSP_ID_2 ? E_DRV_SCLHVSP_ID_2 :
           enHVSP_ID == E_MDRV_SCLHVSP_ID_3 ? E_DRV_SCLHVSP_ID_3 :
                                           E_DRV_SCLHVSP_ID_1;
    DrvSclHvspOpen(enID);
}
bool MDrvSclHvspExit(bool bCloseISR)
{
    if(_MHVSP_Mutex != -1)
    {
         DrvSclOsDeleteMutex(_MHVSP_Mutex);
         _MHVSP_Mutex = -1;
    }
    DrvSclHvspExit(bCloseISR);
    gbMemReadyForMdrv = 0;
    return 1;
}
bool MDrvSclHvspInit(MDrvSclHvspIdType_e enHVSP_ID, MDrvSclHvspInitConfig_t *pCfg)
{
    DrvSclHvspInitConfig_t stInitCfg;
    static bool bInit = FALSE;
    char word[] = {"_MHVSP_Mutex"};

    if(DrvSclOsInit() == FALSE)
    {
        SCL_DBG(SCL_DBG_LV_HVSP()&(Get_DBGMG_HVSP(enHVSP_ID)), "[MDRVHVSP]%s(%d) init SclOS fail\n",
            __FUNCTION__, __LINE__);
        return FALSE;
    }
    if (_MHVSP_Mutex != -1)
    {
        SCL_DBG(SCL_DBG_LV_DRVHVSP(), "[MDRVHVSP]%s(%d) alrady done\n", __FUNCTION__, __LINE__);
        return 1;
    }
    _MHVSP_Mutex = DrvSclOsCreateMutex(E_DRV_SCLOS_FIFO, word, SCLOS_PROCESS_SHARED);

    if (_MHVSP_Mutex == -1)
    {
        SCL_DBG(SCL_DBG_LV_HVSP()&(Get_DBGMG_HVSP(enHVSP_ID)), "[DRVHVSP]%s(%d): create mutex fail\n",
            __FUNCTION__, __LINE__);
        return FALSE;
    }
    stInitCfg.u32RIUBase    = pCfg->u32Riubase;
    stInitCfg.u32IRQNUM     = pCfg->u32IRQNUM;
    stInitCfg.u32CMDQIRQNUM = pCfg->u32CMDQIRQNUM;
    gu8IPMBufferNum = DNR_BUFFER_MODE;
    DrvSclHvspSetBufferNum(gu8IPMBufferNum);
    if(DrvSclHvspInit(&stInitCfg) == FALSE)
    {
        SCL_DBG(SCL_DBG_LV_HVSP()&(Get_DBGMG_HVSP(enHVSP_ID)), "[HVSP]%s Init Fail\n", __FUNCTION__);
        return FALSE;
    }
    else
    {
        if(bInit == FALSE)
        {
            _MDrvSclHvspSwInit();
        }
        bInit = TRUE;

        return TRUE;
    }

}
void MDrvSclHvspSetMemoryAllocateReady(bool bEn)
{
    MDrvSclHvspMutexLock();
    gbMemReadyForMdrv = bEn;
    MDrvSclHvspMutexUnlock();
    DrvSclHvspSetMemoryAllocateReady(gbMemReadyForMdrv);
}
DrvSclHvspIpmConfig_t _MDrvSclHvspFillIpmStruct(MDrvSclHvspIpmConfig_t *pCfg)
{
    DrvSclHvspIpmConfig_t stIPMCfg;
    stIPMCfg.u16Fetch       = pCfg->u16Width;
    stIPMCfg.u16Vsize       = pCfg->u16Height;
    stIPMCfg.bYCMRead       = (bool)((pCfg->enRW)&E_MDRV_SCLHVSP_MCNR_YCM_R);
    stIPMCfg.bYCMWrite      = (bool)(((pCfg->enRW)&E_MDRV_SCLHVSP_MCNR_YCM_W)>>1);
    stIPMCfg.bCIIRRead       = (bool)(((pCfg->enRW)&E_MDRV_SCLHVSP_MCNR_CIIR_R)>>2);
    stIPMCfg.bCIIRWrite      = (bool)(((pCfg->enRW)&E_MDRV_SCLHVSP_MCNR_CIIR_W)>>3);
    stIPMCfg.u32YCBaseAddr    = pCfg->u32YCPhyAddr;
    stIPMCfg.u32MBaseAddr    = pCfg->u32MPhyAddr;
    stIPMCfg.u32CIIRBaseAddr    = pCfg->u32CIIRPhyAddr;
    stIPMCfg.u32MemSize     = pCfg->u32MemSize;
    return stIPMCfg;
}

DrvSclHvspLdcFrameBufferConfig_t _MDrvSclHvspFillLdcStruct(MDrvSclHvspIpmConfig_t *pCfg, bool bWrite)
{
    DrvSclHvspLdcFrameBufferConfig_t stLDCCfg;
    stLDCCfg.bEnSWMode      = 0x00;
    stLDCCfg.enLDCType      = E_DRV_SCLHVSP_LDCLCBANKMODE_128;
    stLDCCfg.u16Height      = pCfg->u16Height;
    if(pCfg->u16Width == 2048)
    {
        stLDCCfg.u16Width       = pCfg->u16Width -1;
    }
    else
    {
        stLDCCfg.u16Width       = pCfg->u16Width;
    }
    stLDCCfg.u32FBaddr      = pCfg->u32YCPhyAddr;
    stLDCCfg.u8FBidx        = 0x00;
    stLDCCfg.u8FBrwdiff     = SCL_DELAY2FRAMEINDOUBLEBUFFERMode ? 0x0 : 0x1;
                                    // delay 1 frame, set 1 is true, because Frame_buffer_idx = hw_w_idx - reg_ldc_fb_hw_rw_diff
                                    // So ,if want delay 2 frame, need to set '0' for 2 DNR buffer.
                                    //if only 1 DNR buffer , set '0' also delay 1 frame.
    stLDCCfg.bEnDNR         = bWrite;
    return stLDCCfg;
}
bool MDrvSclHvspSetInitIpmConfig(MDrvSclHvspIdType_e enHVSP_ID, MDrvSclHvspIpmConfig_t *pCfg)
{
    DrvSclHvspIpmConfig_t stIPMCfg;
    DrvSclHvspLdcFrameBufferConfig_t stLDCCfg;
    if(enHVSP_ID != E_MDRV_SCLHVSP_ID_1)
    {
        SCL_ERR( "[HVSP]%s ID not correct: %d\n", __FUNCTION__, enHVSP_ID);
        return FALSE;
    }
    SCL_DBG(SCL_DBG_LV_HVSP()&(Get_DBGMG_HVSP(enHVSP_ID)), "%s:%d:PhyAddr=%lx, width=%x, height=%x \n",  __FUNCTION__,enHVSP_ID, pCfg->u32YCPhyAddr, pCfg->u16Width, pCfg->u16Height);
    stIPMCfg = _MDrvSclHvspFillIpmStruct(pCfg);
    stLDCCfg = _MDrvSclHvspFillLdcStruct(pCfg, 0);
    DrvSclHvspSetLDCFrameBuffer_Config(stLDCCfg);
    // ToDo
    // DrvSclHvspSetPrv2CropOnOff(stIPMCfg.bYCMRead);
    if(DrvSclHvspSetIPMConfig(stIPMCfg) == FALSE)
    {
        SCL_DBG(SCL_DBG_LV_HVSP()&(Get_DBGMG_HVSP(enHVSP_ID)), "[HVSP] Set IPM Config Fail\n");
        return FALSE;
    }
    else
    {
        return TRUE;;
    }
}


MDrvSclHvspPatTgenTimingType_e _MDrvSclHvspGetPatTgenTiming(MDrvSclHvspTimingConfig_t *pTiming)
{
    MDrvSclHvspPatTgenTimingType_e enTiming;

    if(Is_PTGEN_FHD(pTiming->u16Htotal,pTiming->u16Vtotal,pTiming->u16Vfrequency))
    {
        enTiming = E_MDRV_SCLHVSP_PAT_TGEN_TIMING_1920_1080_;
    }
    else if(Is_PTGEN_HD(pTiming->u16Htotal,pTiming->u16Vtotal,pTiming->u16Vfrequency))
    {
        enTiming = E_MDRV_SCLHVSP_PAT_TGEN_TIMING_1024_768_6;
    }
    else if(Is_PTGEN_SD(pTiming->u16Htotal,pTiming->u16Vtotal,pTiming->u16Vfrequency))
    {
        enTiming = E_MDRV_SCLHVSP_PAT_TGEN_TIMING_640_480_60;
    }
    else
    {
        enTiming = E_MDRV_SCLHVSP_PAT_TGEN_TIMING_UNDEFINED;
    }

    SCL_DBG(SCL_DBG_LV_HVSP()&(Get_DBGMG_HVSP(3)), "[HVSP]%s(%d) Timing:%s(%d)", __FUNCTION__, __LINE__, PARSING_PAT_TGEN_TIMING(enTiming), enTiming);
    return enTiming;
}

//---------------------------------------------------------------------------------------------------------
// IOCTL function
//---------------------------------------------------------------------------------------------------------
bool _MDrvSclHvspIsInputSrcPatternGen(DrvSclHvspIpMuxType_e enIPMux, MDrvSclHvspInputConfig_t *pCfg)
{
    if(enIPMux == E_DRV_SCLHVSP_IP_MUX_PAT_TGEN)
    {
        MDrvSclHvspPatTgenTimingType_e enPatTgenTiming;
        DrvSclHvspPatTgenConfig_t stPatTgenCfg;

        enPatTgenTiming = _MDrvSclHvspGetPatTgenTiming(&pCfg->stTimingCfg);
        SCL_DBG(SCL_DBG_LV_HVSP()&(Get_DBGMG_HVSP(E_MDRV_SCLHVSP_ID_1)),
            "%s:%d type:%d\n", __FUNCTION__,E_MDRV_SCLHVSP_ID_1,enPatTgenTiming);
        DrvSclOsMemcpy(&stPatTgenCfg, &gstPatTgenCfg[enPatTgenTiming],sizeof(DrvSclHvspPatTgenConfig_t));
        if(enPatTgenTiming == E_MDRV_SCLHVSP_PAT_TGEN_TIMING_UNDEFINED)
        {
            stPatTgenCfg.u16HActive = pCfg->stCaptureWin.u16Width;
            stPatTgenCfg.u16VActive = pCfg->stCaptureWin.u16Height;
            stPatTgenCfg.u16Htt = stPatTgenCfg.u16HActive +
                                  stPatTgenCfg.u16HBackPorch +
                                  stPatTgenCfg.u16HFrontPorch +
                                  stPatTgenCfg.u16HSyncWidth;

            stPatTgenCfg.u16Vtt = stPatTgenCfg.u16VActive +
                                  stPatTgenCfg.u16VBackPorch +
                                  stPatTgenCfg.u16VFrontPorch +
                                  stPatTgenCfg.u16VSyncWidth;

        }
        return (bool)DrvSclHvspSetPatTgen(TRUE, &stPatTgenCfg);
    }
    else
    {
        return 0;
    }
}
DrvSclHvspPriMaskConfig_t _MDrvSclHvspFillPriMaskConfig(MDrvSclHvspPriMaskConfig_t stCfg)
{
    DrvSclHvspPriMaskConfig_t stPriMaskCfg;
    stPriMaskCfg.bMask = stCfg.bMask;
    stPriMaskCfg.u16Height = stCfg.stMaskWin.u16Height;
    stPriMaskCfg.u16Width = stCfg.stMaskWin.u16Width;
    stPriMaskCfg.u16X = stCfg.stMaskWin.u16X;
    stPriMaskCfg.u16Y = stCfg.stMaskWin.u16Y;
    return stPriMaskCfg;
}
u16 _MDrvSclHvspGetScaleParameter(u16 u16Src, u16 u16Dsp, u16 u16Val)
{
    //((u16)((u32)(value * RATIO_CONFIG *  numerator) / (denominator * RATIO_CONFIG)))
    u32 u32input;
    u16 u16output;
    u32input = u16Src *u16Val;
    u16output = (u16)(u32input/u16Dsp);
    if((u32input%u16Dsp))
    {
        u16output ++;
    }
    return u16output;
}
MDrvSclHvspPriMaskConfig_t _MDrvSclHvspTransPercentToPriCfg
    (MDrvSclHvspPriMaskConfig_t stPerCfg ,MDrvSclHvspScalingConfig_t sthvspCfg)
{
    MDrvSclHvspPriMaskConfig_t stCfg;
    stCfg.stMaskWin.u16X = (stPerCfg.stMaskWin.u16X * sthvspCfg.u16Dsp_Width) % PRI_MASK_PERCENT_BASE;
    if(stCfg.stMaskWin.u16X > 0)
    {
        stCfg.stMaskWin.u16X = (stPerCfg.stMaskWin.u16X * sthvspCfg.u16Dsp_Width) / PRI_MASK_PERCENT_BASE;
        stCfg.stMaskWin.u16X++;
    }
    else
    {
        stCfg.stMaskWin.u16X = (stPerCfg.stMaskWin.u16X * sthvspCfg.u16Dsp_Width) / PRI_MASK_PERCENT_BASE;
    }
    stCfg.stMaskWin.u16Y = (stPerCfg.stMaskWin.u16Y * sthvspCfg.u16Dsp_Height) % PRI_MASK_PERCENT_BASE;
    if(stCfg.stMaskWin.u16Y > 0)
    {
        stCfg.stMaskWin.u16Y = (stPerCfg.stMaskWin.u16Y * sthvspCfg.u16Dsp_Height) / PRI_MASK_PERCENT_BASE;
        stCfg.stMaskWin.u16Y++;
    }
    else
    {
        stCfg.stMaskWin.u16Y = (stPerCfg.stMaskWin.u16Y * sthvspCfg.u16Dsp_Height) / PRI_MASK_PERCENT_BASE;
    }
    stCfg.stMaskWin.u16Width = (stPerCfg.stMaskWin.u16Width * sthvspCfg.u16Dsp_Width) % PRI_MASK_PERCENT_BASE;
    if(stCfg.stMaskWin.u16Width > 0)
    {
        stCfg.stMaskWin.u16Width= (stPerCfg.stMaskWin.u16Width * sthvspCfg.u16Dsp_Width) / PRI_MASK_PERCENT_BASE;
        stCfg.stMaskWin.u16Width++;
    }
    else
    {
        stCfg.stMaskWin.u16Width= (stPerCfg.stMaskWin.u16Width * sthvspCfg.u16Dsp_Width) / PRI_MASK_PERCENT_BASE;
    }
    stCfg.stMaskWin.u16Height = (stPerCfg.stMaskWin.u16Height * sthvspCfg.u16Dsp_Height) % PRI_MASK_PERCENT_BASE;
    if(stCfg.stMaskWin.u16Height > 0)
    {
        stCfg.stMaskWin.u16Height= (stPerCfg.stMaskWin.u16Height * sthvspCfg.u16Dsp_Height) / PRI_MASK_PERCENT_BASE;
        stCfg.stMaskWin.u16Height++;
    }
    else
    {
        stCfg.stMaskWin.u16Height= (stPerCfg.stMaskWin.u16Height * sthvspCfg.u16Dsp_Height) / PRI_MASK_PERCENT_BASE;
    }
    stCfg.bMask = stPerCfg.bMask;
    stCfg.u8idx = stPerCfg.u8idx;
    SCL_DBG(SCL_DBG_LV_HVSP()&(Get_DBGMG_HVSP(E_MDRV_SCLHVSP_ID_1)),
       "%s:MASK OnOff:%hhd ,Position:(%hd,%hd),w=%hd h=%hd  \n",
       __FUNCTION__,stCfg.bMask,stCfg.stMaskWin.u16X,stCfg.stMaskWin.u16Y,stCfg.stMaskWin.u16Width,
       stCfg.stMaskWin.u16Height);
    return stCfg;
}
MDrvSclHvspPriMaskConfig_t _MDrvSclHvspTransPriCfgToPercent
    (MDrvSclHvspPriMaskConfig_t stCfg ,MDrvSclHvspScalingConfig_t sthvspCfg)
{
    MDrvSclHvspPriMaskConfig_t stPerCfg;
    stPerCfg.stMaskWin.u16X = (stCfg.stMaskWin.u16X * PRI_MASK_PERCENT_BASE) / sthvspCfg.u16Dsp_Width;
    stPerCfg.stMaskWin.u16Y = (stCfg.stMaskWin.u16Y * PRI_MASK_PERCENT_BASE) / sthvspCfg.u16Dsp_Height;
    stPerCfg.stMaskWin.u16Width = (stCfg.stMaskWin.u16Width * PRI_MASK_PERCENT_BASE) / sthvspCfg.u16Dsp_Width;
    stPerCfg.stMaskWin.u16Height = (stCfg.stMaskWin.u16Height * PRI_MASK_PERCENT_BASE) / sthvspCfg.u16Dsp_Height;
    stPerCfg.bMask = stCfg.bMask;
    stPerCfg.u8idx = stCfg.u8idx;
    SCL_DBG(SCL_DBG_LV_HVSP()&(Get_DBGMG_HVSP(E_MDRV_SCLHVSP_ID_1)),
       "%s:MASK OnOff:%hhd ,Position:(%hd,%hd),w=%hd h=%hd  (/10000)\n",
       __FUNCTION__,stPerCfg.bMask,stPerCfg.stMaskWin.u16X,stPerCfg.stMaskWin.u16Y,
       stPerCfg.stMaskWin.u16Height,stPerCfg.stMaskWin.u16Width);
    return stPerCfg;
}

DrvSclHvspPriMaskConfig_t _MDrvSclHvspGetPriMaskConfig
    (MDrvSclHvspPriMaskConfig_t stCfg, MDrvSclHvspScalingConfig_t sthvspCfg)
{
    DrvSclHvspPriMaskConfig_t stPriMaskCfg;
    //u32 u32WidthRatio;
    //u32 u32HeightRatio;
    if(IS_HVSPNotOpen(sthvspCfg.u16Src_Width,sthvspCfg.u16Dsp_Width))
    {
        SCL_DBG(SCL_DBG_LV_HVSP()&(Get_DBGMG_HVSP(E_MDRV_SCLHVSP_ID_1)),
           "%s:, Still Not Setting HVSP ,MASK OnOff:%hhd ,Area =%ld ,Position:(%hd,%hd)\n",
           __FUNCTION__,stCfg.bMask,
           (u32)((u32)stCfg.stMaskWin.u16Height*(u32)stCfg.stMaskWin.u16Width),
           stCfg.stMaskWin.u16X,stCfg.stMaskWin.u16Y);
    }
    else
    {
        if(IS_NotScalingAfterCrop(sthvspCfg.stCropWin.bEn,
            sthvspCfg.u16Dsp_Height,sthvspCfg.stCropWin.u16Height,
            sthvspCfg.u16Dsp_Width,sthvspCfg.stCropWin.u16Width))
        {
            SCL_DBG(SCL_DBG_LV_HVSP()&(Get_DBGMG_HVSP(E_MDRV_SCLHVSP_ID_1)),
               "%s:, NotScalingAfterCrop, Not need to handler,MASK OnOff:%hhd ,Area =%ld ,Position:(%hd,%hd)\n",
               __FUNCTION__,stCfg.bMask,
               (u32)((u32)stCfg.stMaskWin.u16Height*(u32)stCfg.stMaskWin.u16Width),
               stCfg.stMaskWin.u16X,stCfg.stMaskWin.u16Y);
        }
        else
        {
            if(sthvspCfg.stCropWin.bEn)
            {
                //u32WidthRatio = CAL_HVSP_RATIO(sthvspCfg.stCropWin.u16Width,sthvspCfg.u16Dsp_Width);
                //u32HeightRatio = CAL_HVSP_RATIO(sthvspCfg.stCropWin.u16Height,sthvspCfg.u16Dsp_Height);
                stCfg.stMaskWin.u16X = (_MDrvSclHvspGetScaleParameter
                 (sthvspCfg.stCropWin.u16Width,sthvspCfg.u16Dsp_Width,stCfg.stMaskWin.u16X));
                stCfg.stMaskWin.u16Y = (_MDrvSclHvspGetScaleParameter
                 (sthvspCfg.stCropWin.u16Height,sthvspCfg.u16Dsp_Height,stCfg.stMaskWin.u16Y));
                stCfg.stMaskWin.u16Height = (_MDrvSclHvspGetScaleParameter
                 (sthvspCfg.stCropWin.u16Height,sthvspCfg.u16Dsp_Height,stCfg.stMaskWin.u16Height));
                stCfg.stMaskWin.u16Width = (_MDrvSclHvspGetScaleParameter
                    (sthvspCfg.stCropWin.u16Width,sthvspCfg.u16Dsp_Width,stCfg.stMaskWin.u16Width));
            }
            else
            {
                //u32WidthRatio = CAL_HVSP_RATIO(sthvspCfg.u16Src_Width,sthvspCfg.u16Dsp_Width);
                //u32HeightRatio = CAL_HVSP_RATIO(sthvspCfg.u16Src_Height,sthvspCfg.u16Dsp_Height);
                stCfg.stMaskWin.u16X = (_MDrvSclHvspGetScaleParameter
                    (sthvspCfg.u16Src_Width,sthvspCfg.u16Dsp_Width,stCfg.stMaskWin.u16X));
                stCfg.stMaskWin.u16Y = (_MDrvSclHvspGetScaleParameter
                    (sthvspCfg.u16Src_Height,sthvspCfg.u16Dsp_Height,stCfg.stMaskWin.u16Y));
                stCfg.stMaskWin.u16Height = (_MDrvSclHvspGetScaleParameter
                    (sthvspCfg.u16Src_Height,sthvspCfg.u16Dsp_Height,stCfg.stMaskWin.u16Height));
                stCfg.stMaskWin.u16Width = (_MDrvSclHvspGetScaleParameter
                    (sthvspCfg.u16Src_Width,sthvspCfg.u16Dsp_Width,stCfg.stMaskWin.u16Width));
            }
            //stCfg.stMaskWin.u16X = (stCfg.stMaskWin.u16X * (u32WidthRatio) / RATIO_CONFIG);
            //stCfg.stMaskWin.u16Y = (stCfg.stMaskWin.u16Y * (u32HeightRatio) / RATIO_CONFIG);
            //stCfg.stMaskWin.u16Height = (stCfg.stMaskWin.u16Height * (u32HeightRatio) / RATIO_CONFIG);
            //stCfg.stMaskWin.u16Width = (stCfg.stMaskWin.u16Width * (u32WidthRatio) / RATIO_CONFIG);
            SCL_DBG(SCL_DBG_LV_HVSP()&(Get_DBGMG_HVSP(E_MDRV_SCLHVSP_ID_1)),
               "%s:, Mask move,MASK OnOff:%hhd ,H =%ld,W=%ld ,Position:(%hd,%hd)\n",
               __FUNCTION__,stCfg.bMask,
               (u32)stCfg.stMaskWin.u16Height,(u32)stCfg.stMaskWin.u16Width,
               stCfg.stMaskWin.u16X,stCfg.stMaskWin.u16Y);
        }
    }
    stPriMaskCfg = _MDrvSclHvspFillPriMaskConfig(stCfg);
    return stPriMaskCfg;
}
bool _MDrvSclHvspFindUnusePriMask(void)
{
    u8 u8Num;
    u8 u8lastTimeNum = 0;
    for(u8Num=0;u8Num<PRI_MASK_NUM;u8Num++)
    {
        if(gstHvspPriMaskCfg[gu8PriMaskInst][u8Num].bMask == 0)
        {
            u8lastTimeNum = u8Num;
            break;
        }
        if(u8Num == (PRI_MASK_NUM-1))
        {
            u8lastTimeNum = PRI_MASK_NUM;
            break;
        }

    }
    return u8lastTimeNum;
}
void _MDrvSclHvspSetPriMaskStorageDynamic(MDrvSclHvspPriMaskConfig_t stPriMaskCfg)
{
    u8 u8Num;
    bool bStage = 1;
    for(u8Num=0;u8Num<PRI_MASK_NUM;u8Num++)
    {
        if(gstHvspPriMaskCfg[gu8PriMaskInst][u8Num].stMaskWin.u16Height == stPriMaskCfg.stMaskWin.u16Height &&
        gstHvspPriMaskCfg[gu8PriMaskInst][u8Num].stMaskWin.u16Width == stPriMaskCfg.stMaskWin.u16Width &&
        gstHvspPriMaskCfg[gu8PriMaskInst][u8Num].stMaskWin.u16X == stPriMaskCfg.stMaskWin.u16X &&
        gstHvspPriMaskCfg[gu8PriMaskInst][u8Num].stMaskWin.u16Y == stPriMaskCfg.stMaskWin.u16Y)
        {
            bStage = 0;
            break;
        }

    }
    if(bStage)
    {
        if(gu8PriMaskNum[gu8PriMaskInst] <PRI_MASK_NUM)
        {
            MDrvSclHvspMutexLock();
            DrvSclOsMemcpy(&gstHvspPriMaskCfg[gu8PriMaskInst][gu8PriMaskNum[gu8PriMaskInst]],&stPriMaskCfg,sizeof(DrvSclHvspPriMaskConfig_t));
            gu8PriMaskNum[gu8PriMaskInst] = _MDrvSclHvspFindUnusePriMask();
            MDrvSclHvspMutexUnlock();
        }
        else
        {
            SCL_DBG(SCL_DBG_LV_HVSP()&(Get_DBGMG_HVSP(E_MDRV_SCLHVSP_ID_1)),
            "%s::ID:%d, PriMask Storage Full\n",__FUNCTION__,E_MDRV_SCLHVSP_ID_1);
        }
    }
    else
    {
        if(gstHvspPriMaskCfg[gu8PriMaskInst][u8Num].bMask != stPriMaskCfg.bMask)
        {
            MDrvSclHvspMutexLock();
            gstHvspPriMaskCfg[gu8PriMaskInst][u8Num].bMask = stPriMaskCfg.bMask;
            MDrvSclHvspMutexUnlock();
        }
    }
}
void _MDrvSclHvspSetPriMaskStorage(MDrvSclHvspPriMaskConfig_t stPriMaskCfg)
{
    MDrvSclHvspMutexLock();
    gu8PriMaskNum[gu8PriMaskInst] = PRI_MASK_NUM;
    DrvSclOsMemcpy(&gstHvspPriMaskCfg[gu8PriMaskInst][stPriMaskCfg.u8idx],&stPriMaskCfg,sizeof(DrvSclHvspPriMaskConfig_t));
    MDrvSclHvspMutexUnlock();
}
void MDrvSclHvspSetPatTgenStatus(bool bEn)
{
    DrvSclHvspSetPatTgen(bEn, NULL);
}
bool MDrvSclHvspSetInputConfig(MDrvSclHvspIdType_e enHVSP_ID, MDrvSclHvspInputConfig_t *pCfg)
{
    bool Ret = TRUE;
    SCL_DBG(SCL_DBG_LV_HVSP()&(Get_DBGMG_HVSP(enHVSP_ID)), "%s:%d\n", __FUNCTION__,enHVSP_ID);

    //SCL_ERR("clk1:%d, clk2:%d\n",(int)pCfg->stclk->idclk,(int)pCfg->stclk->fclk1);

    if(IS_WRONG_TYPE(enHVSP_ID,pCfg->enSrcType))
    {
        SCL_ERR( "[HVSP] Wrong Input Type: %d, %d\n", enHVSP_ID, pCfg->enSrcType);
        return FALSE;
    }
    if(enHVSP_ID == E_MDRV_SCLHVSP_ID_1)
    {
        DrvSclHvspIpMuxType_e enIPMux;
        enIPMux = pCfg->enSrcType == E_MDRV_SCLHVSP_SRC_ISP      ? E_DRV_SCLHVSP_IP_MUX_ISP :
                  pCfg->enSrcType == E_MDRV_SCLHVSP_SRC_BT656    ? E_DRV_SCLHVSP_IP_MUX_BT656 :
                  pCfg->enSrcType == E_MDRV_SCLHVSP_SRC_HVSP     ? E_DRV_SCLHVSP_IP_MUX_HVSP :
                  pCfg->enSrcType == E_MDRV_SCLHVSP_SRC_PAT_TGEN ? E_DRV_SCLHVSP_IP_MUX_PAT_TGEN :
                                                                 E_DRV_SCLHVSP_IP_MUX_MAX;
        Ret &= (bool)DrvSclHvspSetInputMux(enIPMux,(DrvSclHvspClkConfig_t *)pCfg->stclk);
        DrvSclHvspSetInputSrcSize(pCfg->stCaptureWin.u16Height,pCfg->stCaptureWin.u16Width);
        DrvSclHvspSetCropWindowSize();
        if(_MDrvSclHvspIsInputSrcPatternGen(enIPMux, pCfg))
        {
            Ret &= TRUE;
        }
        else
        {
            Ret &= (bool)DrvSclHvspSetPatTgen(FALSE, NULL);
        }

    }
    return Ret;
}

bool MDrvSclHvspSetPostCropConfig(MDrvSclHvspIdType_e enHVSP_ID, MDrvSclHvspPostCropConfig_t *pCfg)
{
    DrvSclHvspIdType_e enID;
    DrvSclHvspScalingConfig_t stScalingCfg;
    bool ret;
    DrvSclHvspScalingConfig_t stRet;

    SCL_DBG(SCL_DBG_LV_HVSP()&(Get_DBGMG_HVSP(enHVSP_ID)), "%s\n", __FUNCTION__);

    enID = enHVSP_ID == E_MDRV_SCLHVSP_ID_2 ? E_DRV_SCLHVSP_ID_2 :
           enHVSP_ID == E_MDRV_SCLHVSP_ID_3 ? E_DRV_SCLHVSP_ID_3 :
                                           E_DRV_SCLHVSP_ID_1;

    MDrvSclHvspMutexLock();
    DrvSclOsMemcpy(&gstHvspPostCropCfg, pCfg, sizeof(MDrvSclHvspPostCropConfig_t));
    MDrvSclHvspMutexUnlock();
    stScalingCfg.bCropEn[DRV_HVSP_CROP_1]        = 0;
    stScalingCfg.bCropEn[DRV_HVSP_CROP_2]        = pCfg->bCropEn;
    stScalingCfg.u16Crop_X[DRV_HVSP_CROP_2]      = pCfg->u16X;
    stScalingCfg.u16Crop_Y[DRV_HVSP_CROP_2]      = pCfg->u16Y;
    stScalingCfg.u16Crop_Width[DRV_HVSP_CROP_2]  = pCfg->u16Width;
    stScalingCfg.u16Crop_Height[DRV_HVSP_CROP_2] = pCfg->u16Height;

    stScalingCfg.u16Src_Width  = gstHvspScalingCfg.u16Src_Width;
    stScalingCfg.u16Src_Height = gstHvspScalingCfg.u16Src_Height;
    stScalingCfg.u16Dsp_Width  = gstHvspScalingCfg.u16Dsp_Width;
    stScalingCfg.u16Dsp_Height = gstHvspScalingCfg.u16Dsp_Height;

stScalingCfg.stCmdTrigCfg.enType = E_DRV_SCLHVSP_CMD_TRIG_NONE; //ToDo

    stRet = DrvSclHvspSetScaling(enID, stScalingCfg,(DrvSclHvspClkConfig_t *)pCfg->stclk);
    ret = stRet.bRet;
    return  ret;


}
bool _MDrvSclHvspChangePri(MDrvSclHvspScalingConfig_t *pCfg )
{
    unsigned char bChangePri;
    if( pCfg->stCropWin.bEn == gstHvspScalingCfg.stCropWin.bEn)
    {
        bChangePri = 0;
    }
    else
    {
        return 1;
    }
    if(pCfg->stCropWin.u16X == gstHvspScalingCfg.stCropWin.u16X)
    {
        bChangePri = 0;
    }
    else
    {
        return 1;
    }
    if(pCfg->stCropWin.u16Y == gstHvspScalingCfg.stCropWin.u16Y)
    {
        bChangePri = 0;
    }
    else
    {
        return 1;
    }
    if(pCfg->stCropWin.u16Width == gstHvspScalingCfg.stCropWin.u16Width)
    {
        bChangePri = 0;
    }
    else
    {
        return 1;
    }
    if(pCfg->stCropWin.u16Height == gstHvspScalingCfg.stCropWin.u16Height)
    {
        bChangePri = 0;
    }
    else
    {
        return 1;
    }

    return bChangePri;
}
MDrvSclHvspScalingConfig_t _MDrvSclHvspChangebyZoomSizeLimitation
    (MDrvSclHvspScalingConfig_t stGCfg,DrvSclHvspScalingConfig_t stCfg )
{
    MDrvSclHvspScalingConfig_t stRet;
    unsigned char u16cropType;
    if( SetPostCrop)
    {
        u16cropType = 1;
    }
    else
    {
        u16cropType = 0;
    }
    stRet.stCropWin.bEn = stCfg.bCropEn[u16cropType];
    stRet.stCropWin.u16Height = stCfg.u16Crop_Height[u16cropType];
    stRet.stCropWin.u16Width = stCfg.u16Crop_Width[u16cropType] ;
    stRet.stCropWin.u16X = stCfg.u16Crop_X[u16cropType];
    stRet.stCropWin.u16Y = stCfg.u16Crop_Y[u16cropType];
    stRet.u16Dsp_Height = stGCfg.u16Dsp_Height;
    stRet.u16Dsp_Width = stGCfg.u16Dsp_Width;
    stRet.u16Src_Height = stGCfg.u16Src_Height;
    stRet.u16Src_Width = stGCfg.u16Src_Width;
    return stRet;
}
bool MDrvSclHvspSetScalingConfig(MDrvSclHvspIdType_e enHVSP_ID, MDrvSclHvspScalingConfig_t *pCfg )
{
    DrvSclHvspIdType_e enID;
    DrvSclHvspScalingConfig_t stScalingCfg;
    DrvSclHvspScalingConfig_t stRet;
    bool ret;
    u8 idx;
    bool bChangePri = 0;
    DrvSclHvspPriMaskConfig_t stPriMaskCfg;
    MDrvSclHvspPriMaskConfig_t stPriMaskStore;
    MDrvSclHvspScalingConfig_t stChange;
    SCL_DBG(SCL_DBG_LV_HVSP()&(Get_DBGMG_HVSP(enHVSP_ID)), "%s:%d\n", __FUNCTION__,enHVSP_ID);
    enID = enHVSP_ID == E_MDRV_SCLHVSP_ID_2 ? E_DRV_SCLHVSP_ID_2 :
           enHVSP_ID == E_MDRV_SCLHVSP_ID_3 ? E_DRV_SCLHVSP_ID_3 :
                                           E_DRV_SCLHVSP_ID_1;

    if(enID == E_DRV_SCLHVSP_ID_1)
    {
       bChangePri = _MDrvSclHvspChangePri(pCfg);
       MDrvSclHvspMutexLock();
       DrvSclOsMemcpy(&gstHvspScalingCfg, pCfg, sizeof(MDrvSclHvspScalingConfig_t));
       MDrvSclHvspMutexUnlock();
    }

    if( SetPostCrop || (DrvSclHvspGetInputSrcMux()==E_DRV_SCLHVSP_IP_MUX_PAT_TGEN))
    {
        stScalingCfg.bCropEn[DRV_HVSP_CROP_1]        = 0;
        stScalingCfg.u16Crop_X[DRV_HVSP_CROP_1]      = 0;
        stScalingCfg.u16Crop_Y[DRV_HVSP_CROP_1]      = 0;
        stScalingCfg.u16Crop_Width[DRV_HVSP_CROP_1]  = pCfg->u16Src_Width;
        stScalingCfg.u16Crop_Height[DRV_HVSP_CROP_1] = pCfg->u16Src_Height;
        stScalingCfg.bCropEn[DRV_HVSP_CROP_2]        = pCfg->stCropWin.bEn;
        stScalingCfg.u16Crop_X[DRV_HVSP_CROP_2]      = pCfg->stCropWin.u16X;
        stScalingCfg.u16Crop_Y[DRV_HVSP_CROP_2]      = pCfg->stCropWin.u16Y;
        stScalingCfg.u16Crop_Width[DRV_HVSP_CROP_2]  = pCfg->stCropWin.u16Width;
        stScalingCfg.u16Crop_Height[DRV_HVSP_CROP_2] = pCfg->stCropWin.u16Height;
    }
    else
    {
        stScalingCfg.bCropEn[DRV_HVSP_CROP_1]        = pCfg->stCropWin.bEn;
        stScalingCfg.u16Crop_X[DRV_HVSP_CROP_1]      = pCfg->stCropWin.u16X;
        stScalingCfg.u16Crop_Y[DRV_HVSP_CROP_1]      = pCfg->stCropWin.u16Y;
        stScalingCfg.u16Crop_Width[DRV_HVSP_CROP_1]  = pCfg->stCropWin.u16Width;
        stScalingCfg.u16Crop_Height[DRV_HVSP_CROP_1] = pCfg->stCropWin.u16Height;
        stScalingCfg.bCropEn[DRV_HVSP_CROP_2]        = 0;
        stScalingCfg.u16Crop_X[DRV_HVSP_CROP_2]      = 0;
        stScalingCfg.u16Crop_Y[DRV_HVSP_CROP_2]      = 0;
        stScalingCfg.u16Crop_Width[DRV_HVSP_CROP_2]  = pCfg->stCropWin.u16Width;
        stScalingCfg.u16Crop_Height[DRV_HVSP_CROP_2] = pCfg->stCropWin.u16Height;
    }
    stScalingCfg.u16Src_Width  = pCfg->u16Src_Width;
    stScalingCfg.u16Src_Height = pCfg->u16Src_Height;
    stScalingCfg.u16Dsp_Width  = pCfg->u16Dsp_Width;
    stScalingCfg.u16Dsp_Height = pCfg->u16Dsp_Height;
    if(enHVSP_ID==E_MDRV_SCLHVSP_ID_1 || enHVSP_ID==E_MDRV_SCLHVSP_ID_2)
    {
        stScalingCfg.stCmdTrigCfg.enType = E_DRV_SCLHVSP_CMD_TRIG_CMDQ_LDC_SYNC; //ToDo
    }
    else if(enHVSP_ID==E_MDRV_SCLHVSP_ID_3)
    {
        stScalingCfg.stCmdTrigCfg.enType = E_DRV_SCLHVSP_CMD_TRIG_NONE; //ToDo
    }
    else
    {
        stScalingCfg.stCmdTrigCfg.enType = E_DRV_SCLHVSP_CMD_TRIG_POLL_LDC_SYNC; //ToDo
    }
    stScalingCfg.stCmdTrigCfg.u8Fmcnt = 0;
    stRet = DrvSclHvspSetScaling(enID, stScalingCfg,(DrvSclHvspClkConfig_t *)pCfg->stclk);
    ret = stRet.bRet;

    if(enID == E_DRV_SCLHVSP_ID_1)
    {
        if(!ret)
        {
            stChange = _MDrvSclHvspChangebyZoomSizeLimitation(gstHvspScalingCfg,stRet);
        }
        else
        {
            DrvSclOsMemcpy(&stChange, &gstHvspScalingCfg, sizeof(MDrvSclHvspScalingConfig_t));
        }
        if(gu8PriMaskNum[gu8PriMaskInst] && bChangePri)
        {
            //disable trigger
            DrvSclHvspSetPriMaskTrigger(E_DRV_SCLHVSP_PRIMASK_DISABLE);
            for(idx=0;idx<gu8PriMaskNum[gu8PriMaskInst];idx++)
            {
                if(gstHvspPriMaskCfg[gu8PriMaskInst][idx].u8idx == idx && gstHvspPriMaskCfg[gu8PriMaskInst][idx].bMask)
                {
                    stPriMaskStore =
                        _MDrvSclHvspTransPercentToPriCfg(gstHvspPriMaskCfg[gu8PriMaskInst][idx],stChange);
                    stPriMaskCfg = _MDrvSclHvspGetPriMaskConfig(stPriMaskStore,stChange);
                    DrvSclHvspSetPriMaskConfig(stPriMaskCfg);
                }
            }
            DrvSclHvspSetPriMaskTrigger(E_DRV_SCLHVSP_PRIMASK_ENABLE);
        }
    }
    return  ret;
}



static void  _MDrvSclHvspSetRegisterForce(u32 u32Size, u8 *pBuf)
{
    u32 i;
    u32 u32Reg;
    u16 u16Bank;
    u8  u8Addr, u8Val, u8Msk;

    // bank,  addrr,  val,  msk
    for(i=0; i<u32Size; i+=5)
    {
        u16Bank = (u16)pBuf[i+0] | ((u16)pBuf[i+1])<<8;
        u8Addr  = pBuf[i+2];
        u8Val   = pBuf[i+3];
        u8Msk   = pBuf[i+4];
        u32Reg  = (((u32)u16Bank) << 8) | (u32)u8Addr;

        SCL_DBG(SCL_DBG_LV_HVSP()&(Get_DBGMG_HVSP(3)), "%08lx, %02x, %02x\n", u32Reg, u8Val, u8Msk);
        DrvSclHvspSetRegisterForce(u32Reg, u8Val, u8Msk);
    }
}


bool MDrvSclHvspSetMiscConfig(MDrvSclHvspMiscConfig_t *pCfg)
{
    u8 *pBuf = NULL;

    SCL_DBG(SCL_DBG_LV_HVSP()&(Get_DBGMG_HVSP(3)), "%s\n", __FUNCTION__);

    pBuf = DrvSclOsMemalloc(pCfg->u32Size, GFP_KERNEL);

    if(pBuf == NULL)
    {
        SCL_ERR( "[HVSP1] allocate buffer fail\n");
        return 0;
    }


    if(DrvSclOsMemcpy(pBuf, (void *)pCfg->u32Addr, pCfg->u32Size))
    {
        SCL_ERR( "[HVSP1] copy msic buffer error\n");
        DrvSclOsMemFree(pBuf);
        return 0;
    }

    switch(pCfg->u8Cmd)
    {
        case E_MDRV_SCLHVSP_MISC_CMD_SET_REG:
            _MDrvSclHvspSetRegisterForce(pCfg->u32Size, pBuf);
            break;

        default:
            break;
    }

    DrvSclOsMemFree(pBuf);

    return 1;
}

bool MDrvSclHvspSetMiscConfigForKernel(MDrvSclHvspMiscConfig_t *pCfg)
{
     SCL_DBG(SCL_DBG_LV_HVSP()&(Get_DBGMG_HVSP(3)), "%s\n", __FUNCTION__);
     switch(pCfg->u8Cmd)
     {
         case E_MDRV_SCLHVSP_MISC_CMD_SET_REG:
             _MDrvSclHvspSetRegisterForce(pCfg->u32Size, (u8 *)pCfg->u32Addr);
             break;

         default:
             break;
     }
     return 1;
}

bool MDrvSclHvspGetSCLInform(MDrvSclHvspIdType_e enHVSP_ID, MDrvSclHvspScInformConfig_t *pstCfg)
{
     DrvSclHvspScInformConfig_t stInformCfg;
     DrvSclHvspIdType_e enID;
     bool bRet = 1;
     SCL_DBG(SCL_DBG_LV_HVSP()&(Get_DBGMG_HVSP(enHVSP_ID)), "[HVSP1]%s\n", __FUNCTION__);
     enID = enHVSP_ID == E_MDRV_SCLHVSP_ID_2 ? E_DRV_SCLHVSP_ID_2 :
            enHVSP_ID == E_MDRV_SCLHVSP_ID_3 ? E_DRV_SCLHVSP_ID_3 :
                                            E_DRV_SCLHVSP_ID_1;

     DrvSclHvspGetSCLInform(enID, &stInformCfg);
     DrvSclOsMemcpy(pstCfg, &stInformCfg, sizeof(MDrvSclHvspScInformConfig_t));
     SCL_DBG(SCL_DBG_LV_HVSP()&(Get_DBGMG_HVSP(enHVSP_ID)),
        "[HVSP1]u16Height:%hd u16Width:%hd\n",stInformCfg.u16Height, stInformCfg.u16Width);
     return bRet;
}

bool MDrvSclHvspSetOsdConfig(MDrvSclHvspIdType_e enHVSP_ID, MDrvSclHvspOsdConfig_t* pstCfg)
{
     bool Ret = TRUE;
     DrvSclHvspOsdConfig_t stOSdCfg;
     DrvSclHvspIdType_e enID;
     enID = enHVSP_ID == E_MDRV_SCLHVSP_ID_2 ? E_DRV_SCLHVSP_ID_2 :
            enHVSP_ID == E_MDRV_SCLHVSP_ID_3 ? E_DRV_SCLHVSP_ID_3 :
                                            E_DRV_SCLHVSP_ID_1;
     DrvSclOsMemcpy(pstCfg, &stOSdCfg, sizeof(MDrvSclHvspOsdConfig_t));
     DrvSclHvspSetOsdConfig(enID, stOSdCfg);
     SCL_DBG(SCL_DBG_LV_HVSP()&(Get_DBGMG_HVSP(enID)),
        "%s::ID:%d, OnOff:%hhd ,Bypass:%hhd\n",
        __FUNCTION__,enID,stOSdCfg.stOsdOnOff.bOSDEn,stOSdCfg.stOsdOnOff.bOSDBypass);
     return Ret;
}
bool MDrvSclHvspSetPriMaskConfig(MDrvSclHvspPriMaskConfig_t stCfg)
{
    bool Ret = TRUE;
    u8 u8idx = 0;
    DrvSclHvspPriMaskConfig_t stPriMaskCfg;
    MDrvSclHvspPriMaskConfig_t stPriMaskStore;
    SCL_DBG(SCL_DBG_LV_HVSP()&(Get_DBGMG_HVSP(E_MDRV_SCLHVSP_ID_1)),
    "%s::ID:%d, OnOff:%hhd id:%hhd\n",__FUNCTION__,E_MDRV_SCLHVSP_ID_1,stCfg.bMask,stCfg.u8idx);
    //need to turn off mask
    //1.turn off same idx
    //2.turn on others idx
    if(stCfg.u8idx == gstHvspPriMaskCfg[gu8PriMaskInst][stCfg.u8idx].u8idx &&
        gstHvspPriMaskCfg[gu8PriMaskInst][stCfg.u8idx].bMask)
    {
        DrvSclHvspPriMaskBufferReset();
        //ReOpen to avoid disable repeat region.
        for(u8idx = 0 ; u8idx<PRI_MASK_NUM; u8idx++)
        {
            if(u8idx == stCfg.u8idx)
            {
                continue;
            }
            else if(u8idx == gstHvspPriMaskCfg[gu8PriMaskInst][u8idx].u8idx
                && gstHvspPriMaskCfg[gu8PriMaskInst][u8idx].bMask)
            {
                stPriMaskStore =
                    _MDrvSclHvspTransPercentToPriCfg(gstHvspPriMaskCfg[gu8PriMaskInst][u8idx],gstHvspScalingCfg);
                stPriMaskCfg = _MDrvSclHvspGetPriMaskConfig(stPriMaskStore,gstHvspScalingCfg);
                DrvSclHvspSetPriMaskConfig(stPriMaskCfg);
            }
        }
    }
    stPriMaskStore = _MDrvSclHvspTransPriCfgToPercent(stCfg,gstHvspScalingCfg);
    _MDrvSclHvspSetPriMaskStorage(stPriMaskStore);
    stPriMaskCfg = _MDrvSclHvspGetPriMaskConfig(stCfg,gstHvspScalingCfg);
    if(stPriMaskCfg.bMask)
    {
        DrvSclHvspSetPriMaskConfig(stPriMaskCfg);
    }
     return Ret;
}
bool MDrvSclHvspSetPriMaskTrigger(bool bEn)
{
     bool Ret = TRUE;
     SCL_DBG(SCL_DBG_LV_HVSP()&(Get_DBGMG_HVSP(E_MDRV_SCLHVSP_ID_1)),
        "%s, OnOff:%hhd\n",__FUNCTION__,bEn);
     DrvSclHvspSetPriMaskTrigger(bEn ? E_DRV_SCLHVSP_PRIMASK_ENABLE : E_DRV_SCLHVSP_PRIMASK_DISABLE);
     if(bEn == E_DRV_SCLHVSP_PRIMASK_DISABLE)
     {
         _MDrvSclHvspResetPriMaskStorage();
     }
     return Ret;
}

u8 MdrvSclHvspGetFrameBufferCountInformation(void)
{
    return gu8IPMBufferNum;
}

bool MDrvSclHvspSetFbManageConfig(MDrvSclHvspFbmgSetType_e enSet)
{
    bool Ret = TRUE;
    DrvSclHvspSetFbManageConfig_t stCfg;
    SCL_DBG(SCL_DBG_LV_HVSP()&(Get_DBGMG_HVSP(3)), "%s\n", __FUNCTION__);
    stCfg.enSet = enSet;
    if (stCfg.enSet & E_DRV_SCLHVSP_FBMG_SET_DNR_COMDE_ON)
    {
        SCL_ERR( "COMDE_ON\n");
        MDrvSclVipSetMcnrConpressForDebug(1);
    }
    else if (stCfg.enSet & E_DRV_SCLHVSP_FBMG_SET_DNR_COMDE_OFF)
    {
        SCL_ERR( "COMDE_OFF\n");
        MDrvSclVipSetMcnrConpressForDebug(0);
    }
    else if (stCfg.enSet & E_DRV_SCLHVSP_FBMG_SET_DNR_COMDE_265OFF)
    {
        SCL_ERR( "COMDE_265OFF\n");
        MDrvSclVipSetMcnrConpressForDebug(0);
    }
    if(stCfg.enSet &E_DRV_SCLHVSP_FBMG_SET_DNR_BUFFER_1)
    {
        gu8IPMBufferNum = 1;
        DrvSclHvspSetBufferNum(gu8IPMBufferNum);
    }
    else if (stCfg.enSet &E_DRV_SCLHVSP_FBMG_SET_DNR_BUFFER_2)
    {
        gu8IPMBufferNum = 2;
        DrvSclHvspSetBufferNum(gu8IPMBufferNum);
    }
    DrvSclHvspSetFbManageConfig(stCfg);
    return Ret;
}
void MDrvSclHvspIdclkRelease(MDrvSclHvspClkConfig_t* stclk)
{
    SCL_DBG(SCL_DBG_LV_HVSP()&(Get_DBGMG_HVSP(3)), "%s\n", __FUNCTION__);
    DrvSclHvspIdclkRelease((DrvSclHvspClkConfig_t *)stclk);
}



bool MDrvSclHvspInputVSyncMonitor(void)
{
    if(DrvSclHvspCheckInputVsync())
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
ssize_t MDrvSclHvspProcShow(char *buf)
{
    char *str = buf;
    char *end = buf + PAGE_SIZE;
    DrvSclHvspInformConfig_t sthvspformCfg;
    DrvSclHvspInformConfig_t sthvsp2formCfg;
    DrvSclHvspInformConfig_t sthvsp3formCfg;
    DrvSclHvspScInformConfig_t stzoomformCfg;
    DrvSclHvspIpmConfig_t stIpmformCfg;
    DrvSclHvspInputInformConfig_t stInformCfg;
    bool bLDCorPrvCrop;
    DrvSclHvspOsdConfig_t stOsdCfg;
    DrvSclHvspGetCrop12Inform(&stInformCfg);
    DrvSclHvspGetSCLInform(E_DRV_SCLHVSP_ID_1,&stzoomformCfg);
    DrvSclHvspGetHvspAttribute(E_DRV_SCLHVSP_ID_1,&sthvspformCfg);
    DrvSclHvspGetHvspAttribute(E_DRV_SCLHVSP_ID_2,&sthvsp2formCfg);
    DrvSclHvspGetHvspAttribute(E_DRV_SCLHVSP_ID_3,&sthvsp3formCfg);
    DrvSclHvspGetOsdAttribute(E_DRV_SCLHVSP_ID_1,&stOsdCfg);
    bLDCorPrvCrop = DrvSclHvspGetFrameBufferAttribute(E_DRV_SCLHVSP_ID_1,&stIpmformCfg);
    str += DrvSclOsScnprintf(str, end - str, "========================SCL PROC FRAMEWORK======================\n");
    str += DrvSclOsScnprintf(str, end - str, "\n");
    str += DrvSclOsScnprintf(str, end - str, "------------------------SCL INPUT MUX----------------------\n");
    if(stInformCfg.enMux==0)
    {
        str += DrvSclOsScnprintf(str, end - str, "Input SRC :BT656\n");
    }
    else if(stInformCfg.enMux==1)
    {
        str += DrvSclOsScnprintf(str, end - str, "Input SRC :ISP\n");
    }
    else if(stInformCfg.enMux==3)
    {
        str += DrvSclOsScnprintf(str, end - str, "Input SRC :PTGEN\n");
    }
    else
    {
        str += DrvSclOsScnprintf(str, end - str, "Input SRC :OTHER\n");
    }
    str += DrvSclOsScnprintf(str, end - str, "Input H   :%hd\n",stInformCfg.u16inWidth);
    str += DrvSclOsScnprintf(str, end - str, "Input V   :%hd\n",stInformCfg.u16inHeight);
    str += DrvSclOsScnprintf(str, end - str, "Receive H :%hd\n",stInformCfg.u16inWidthcount);
    str += DrvSclOsScnprintf(str, end - str, "Receive V :%hd\n",stInformCfg.u16inHeightcount);
    str += DrvSclOsScnprintf(str, end - str, "------------------------SCL FB-----------------------------\n");
    str += DrvSclOsScnprintf(str, end - str, "FB H          :%hd\n",stIpmformCfg.u16Fetch);
    str += DrvSclOsScnprintf(str, end - str, "FB V          :%hd\n",stIpmformCfg.u16Vsize);
    str += DrvSclOsScnprintf(str, end - str, "FB Addr       :%lx\n",stIpmformCfg.u32YCBaseAddr);
    str += DrvSclOsScnprintf(str, end - str, "FB memsize    :%ld\n",stIpmformCfg.u32MemSize);
    str += DrvSclOsScnprintf(str, end - str, "FB Buffer     :%hhd\n",gu8IPMBufferNum);
    str += DrvSclOsScnprintf(str, end - str, "FB Write      :%hhd\n",stIpmformCfg.bYCMWrite);
    if(bLDCorPrvCrop &0x1)
    {
        str += DrvSclOsScnprintf(str, end - str, "READ PATH     :LDC\n");
    }
    else if(bLDCorPrvCrop &0x2)
    {
        str += DrvSclOsScnprintf(str, end - str, "READ PATH     :PrvCrop\n");
    }
    else if(stIpmformCfg.bYCMRead)
    {
        str += DrvSclOsScnprintf(str, end - str, "READ PATH     :MCNR\n");
    }
    else
    {
        str += DrvSclOsScnprintf(str, end - str, "READ PATH     :NONE\n");
    }
    str += DrvSclOsScnprintf(str, end - str, "FRAME DELAY     :%hhd\n",DrvSclOsGetSclFrameDelay());
    str += DrvSclOsScnprintf(str, end - str, "------------------------SCL Crop----------------------------\n");
    str += DrvSclOsScnprintf(str, end - str, "Crop      :%hhd\n",stInformCfg.bEn);
    str += DrvSclOsScnprintf(str, end - str, "CropX     :%hd\n",stInformCfg.u16inCropX);
    str += DrvSclOsScnprintf(str, end - str, "CropY     :%hd\n",stInformCfg.u16inCropY);
    str += DrvSclOsScnprintf(str, end - str, "CropOutW  :%hd\n",stInformCfg.u16inCropWidth);
    str += DrvSclOsScnprintf(str, end - str, "CropOutH  :%hd\n",stInformCfg.u16inCropHeight);
    str += DrvSclOsScnprintf(str, end - str, "SrcW      :%hd\n",stInformCfg.u16inWidth);
    str += DrvSclOsScnprintf(str, end - str, "SrcH      :%hd\n",stInformCfg.u16inHeight);
    str += DrvSclOsScnprintf(str, end - str, "------------------------SCL Zoom----------------------------\n");
    str += DrvSclOsScnprintf(str, end - str, "Zoom      :%hhd\n",stzoomformCfg.bEn);
    str += DrvSclOsScnprintf(str, end - str, "ZoomX     :%hd\n",stzoomformCfg.u16X);
    str += DrvSclOsScnprintf(str, end - str, "ZoomY     :%hd\n",stzoomformCfg.u16Y);
    str += DrvSclOsScnprintf(str, end - str, "ZoomOutW  :%hd\n",stzoomformCfg.u16crop2OutWidth);
    str += DrvSclOsScnprintf(str, end - str, "ZoomOutH  :%hd\n",stzoomformCfg.u16crop2OutHeight);
    str += DrvSclOsScnprintf(str, end - str, "SrcW      :%hd\n",stzoomformCfg.u16crop2inWidth);
    str += DrvSclOsScnprintf(str, end - str, "SrcH      :%hd\n",stzoomformCfg.u16crop2inHeight);
    str += DrvSclOsScnprintf(str, end - str, "------------------------SCL OSD----------------------------\n");
    str += DrvSclOsScnprintf(str, end - str, "ONOFF     :%hhd\n",stOsdCfg.stOsdOnOff.bOSDEn);
    if(stOsdCfg.enOSD_loc)
    {
        str += DrvSclOsScnprintf(str, end - str, "Locate: Before\n");
    }
    else
    {
        str += DrvSclOsScnprintf(str, end - str, "Locate: After\n");
    }
    str += DrvSclOsScnprintf(str, end - str, "------------------------SCL HVSP1----------------------------\n");
    str += DrvSclOsScnprintf(str, end - str, "InputH    :%hd\n",sthvspformCfg.u16inWidth);
    str += DrvSclOsScnprintf(str, end - str, "InputV    :%hd\n",sthvspformCfg.u16inHeight);
    str += DrvSclOsScnprintf(str, end - str, "OutputH   :%hd\n",sthvspformCfg.u16Width);
    str += DrvSclOsScnprintf(str, end - str, "OutputV   :%hd\n",sthvspformCfg.u16Height);
    str += DrvSclOsScnprintf(str, end - str, "H en  :%hhx\n",sthvspformCfg.bEn&0x1);
    str += DrvSclOsScnprintf(str, end - str, "H function  :%hhx\n",(sthvspformCfg.bEn&0xC0)>>6);
    str += DrvSclOsScnprintf(str, end - str, "V en  :%hhx\n",(sthvspformCfg.bEn&0x2)>>1);
    str += DrvSclOsScnprintf(str, end - str, "V function  :%hhx\n",(sthvspformCfg.bEn&0x30)>>4);
    str += DrvSclOsScnprintf(str, end - str, "------------------------SCL HVSP2----------------------------\n");
    str += DrvSclOsScnprintf(str, end - str, "InputH    :%hd\n",sthvsp2formCfg.u16inWidth);
    str += DrvSclOsScnprintf(str, end - str, "InputV    :%hd\n",sthvsp2formCfg.u16inHeight);
    str += DrvSclOsScnprintf(str, end - str, "OutputH   :%hd\n",sthvsp2formCfg.u16Width);
    str += DrvSclOsScnprintf(str, end - str, "OutputV   :%hd\n",sthvsp2formCfg.u16Height);
    str += DrvSclOsScnprintf(str, end - str, "H en  :%hhx\n",sthvsp2formCfg.bEn&0x1);
    str += DrvSclOsScnprintf(str, end - str, "H function  :%hhx\n",(sthvsp2formCfg.bEn&0xC0)>>6);
    str += DrvSclOsScnprintf(str, end - str, "V en  :%hhx\n",(sthvsp2formCfg.bEn&0x2)>>1);
    str += DrvSclOsScnprintf(str, end - str, "V function  :%hhx\n",(sthvsp2formCfg.bEn&0x30)>>4);
    str += DrvSclOsScnprintf(str, end - str, "------------------------SCL HVSP3----------------------------\n");
    str += DrvSclOsScnprintf(str, end - str, "InputH    :%hd\n",sthvsp3formCfg.u16inWidth);
    str += DrvSclOsScnprintf(str, end - str, "InputV    :%hd\n",sthvsp3formCfg.u16inHeight);
    str += DrvSclOsScnprintf(str, end - str, "OutputH   :%hd\n",sthvsp3formCfg.u16Width);
    str += DrvSclOsScnprintf(str, end - str, "OutputV   :%hd\n",sthvsp3formCfg.u16Height);
    str += DrvSclOsScnprintf(str, end - str, "H en  :%hhx\n",sthvsp3formCfg.bEn&0x1);
    str += DrvSclOsScnprintf(str, end - str, "H function  :%hhx\n",(sthvsp3formCfg.bEn&0xC0)>>6);
    str += DrvSclOsScnprintf(str, end - str, "V en  :%hhx\n",(sthvsp3formCfg.bEn&0x2)>>1);
    str += DrvSclOsScnprintf(str, end - str, "V function  :%hhx\n",(sthvsp3formCfg.bEn&0x30)>>4);
    str += DrvSclOsScnprintf(str, end - str, "\n");
    str += DrvSclOsScnprintf(str, end - str, "========================SCL PROC FRAMEWORK======================\n");
    end = end;
    return (str - buf);
}

//#if defined (SCLOS_TYPE_LINUX_KERNEL)
void MDrvSclHvspSetClkForcemode(bool bEn)
{
    DrvSclHvspSetClkForcemode(bEn);
}
void MDrvSclHvspSetClkRate(void* adjclk,u8 u8Idx)
{
    DrvSclOsClkStruct_t* pstclock = NULL;
    DrvSclHvspSetClkRate(u8Idx);
    if (DrvSclOsClkGetEnableCount((DrvSclOsClkStruct_t*)adjclk)==0)
    {
    }
    else
    {
        if (NULL != (pstclock = DrvSclOsClkGetParentByIndex((DrvSclOsClkStruct_t*)adjclk, (u8Idx &0x0F))))
        {
            DrvSclOsClkSetParent((DrvSclOsClkStruct_t*)adjclk, pstclock);
        }
    }
}
void MDrvSclHvspSetClkOnOff(void* adjclk,bool bEn)
{
    DrvSclOsClkStruct_t* pstclock = NULL;
    if (DrvSclOsClkGetEnableCount((DrvSclOsClkStruct_t*)adjclk)==0 &&bEn)
    {
        if (NULL != (pstclock = DrvSclOsClkGetParentByIndex((DrvSclOsClkStruct_t*)adjclk, 0)))
        {
            DrvSclOsClkSetParent((DrvSclOsClkStruct_t*)adjclk, pstclock);
            DrvSclOsClkPrepareEnable((DrvSclOsClkStruct_t*)adjclk);
        }
    }
    else if(DrvSclOsClkGetEnableCount((DrvSclOsClkStruct_t*)adjclk)!=0 && !bEn)
    {
        if (NULL != (pstclock = DrvSclOsClkGetParentByIndex((DrvSclOsClkStruct_t*)adjclk, 0)))
        {
            DrvSclOsClkSetParent((DrvSclOsClkStruct_t*)adjclk, pstclock);
            DrvSclOsClkDisableUnprepare((DrvSclOsClkStruct_t*)adjclk);
        }
    }
}
ssize_t MDrvSclHvspmonitorHWShow(char *buf,int VsyncCount ,int MonitorErrCount)
{
    char *str = buf;
    char *end = buf + PAGE_SIZE;
    str += DrvSclOsScnprintf(str, end - str, "========================SCL monitor HW BUTTON======================\n");
    str += DrvSclOsScnprintf(str, end - str, "CROP Monitor    :1\n");
    str += DrvSclOsScnprintf(str, end - str, "DMA1FRM Monitor :2\n");
    str += DrvSclOsScnprintf(str, end - str, "DMA1SNP Monitor :3\n");
    str += DrvSclOsScnprintf(str, end - str, "DMA2FRM Monitor :4\n");
    str += DrvSclOsScnprintf(str, end - str, "DMA3FRM Monitor :5\n");
    str += DrvSclOsScnprintf(str, end - str, "========================SCL monitor HW ======================\n");
    str += DrvSclOsScnprintf(str, end - str, "vysnc count:%d",VsyncCount);
    str += DrvSclOsScnprintf(str, end - str, "Monitor Err count:%d\n",MonitorErrCount);
    end = end;
    return (str - buf);
}
ssize_t MDrvSclHvspDbgmgFlagShow(char *buf)
{
    char *str = buf;
    char *end = buf + PAGE_SIZE;
    str += DrvSclOsScnprintf(str, end - str, "========================SCL Debug Message BUTTON======================\n");
    str += DrvSclOsScnprintf(str, end - str, "CONFIG            ECHO        STATUS\n");
    str += DrvSclOsScnprintf(str, end - str, "MDRV_CONFIG       (1)         0x%x\n",gbdbgmessage[EN_DBGMG_MDRV_CONFIG]);
    str += DrvSclOsScnprintf(str, end - str, "IOCTL_CONFIG      (2)         0x%x\n",gbdbgmessage[EN_DBGMG_IOCTL_CONFIG]);
    str += DrvSclOsScnprintf(str, end - str, "HVSP_CONFIG       (3)         0x%x\n",gbdbgmessage[EN_DBGMG_HVSP_CONFIG]);
    str += DrvSclOsScnprintf(str, end - str, "SCLDMA_CONFIG     (4)         0x%x\n",gbdbgmessage[EN_DBGMG_SCLDMA_CONFIG]);
    str += DrvSclOsScnprintf(str, end - str, "PNL_CONFIG        (5)         0x%x\n",gbdbgmessage[EN_DBGMG_PNL_CONFIG]);
    str += DrvSclOsScnprintf(str, end - str, "VIP_CONFIG        (6)         0x%x\n",gbdbgmessage[EN_DBGMG_VIP_CONFIG]);
    str += DrvSclOsScnprintf(str, end - str, "DRVPQ_CONFIG      (7)         0x%x\n",gbdbgmessage[EN_DBGMG_DRVPQ_CONFIG]);
    str += DrvSclOsScnprintf(str, end - str, "INST_ENTRY_CONFIG (8)         0x%x\n",gbdbgmessage[EN_DBGMG_INST_ENTRY_CONFIG]);
    str += DrvSclOsScnprintf(str, end - str, "INST_LOCK_CONFIG  (9)         0x%x\n",gbdbgmessage[EN_DBGMG_INST_LOCK_CONFIG]);
    str += DrvSclOsScnprintf(str, end - str, "INST_FUNC_CONFIG  (A)         0x%x\n",gbdbgmessage[EN_DBGMG_INST_FUNC_CONFIG]);
    str += DrvSclOsScnprintf(str, end - str, "DRVHVSP_CONFIG    (B)         0x%x\n",gbdbgmessage[EN_DBGMG_DRVHVSP_CONFIG]);
    str += DrvSclOsScnprintf(str, end - str, "DRVSCLDMA_CONFIG  (C)         0x%x\n",gbdbgmessage[EN_DBGMG_DRVSCLDMA_CONFIG]);
    str += DrvSclOsScnprintf(str, end - str, "DRVSCLIRQ_CONFIG  (D)         0x%x\n",gbdbgmessage[EN_DBGMG_DRVSCLIRQ_CONFIG]);
    str += DrvSclOsScnprintf(str, end - str, "DRVCMDQ_CONFIG    (E)         0x%x\n",gbdbgmessage[EN_DBGMG_DRVCMDQ_CONFIG]);
    str += DrvSclOsScnprintf(str, end - str, "DRVVIP_CONFIG     (F)         0x%x\n",gbdbgmessage[EN_DBGMG_DRVVIP_CONFIG]);
    str += DrvSclOsScnprintf(str, end - str, "PRIORITY_CONFIG   (G)         0x%x\n",gbdbgmessage[EN_DBGMG_PRIORITY_CONFIG]);
    str += DrvSclOsScnprintf(str, end - str, "PRIORITY_CONFIG   (H)         0x%x\n",gbdbgmessage[EN_DBGMG_UTILITY_CONFIG]);
    str += DrvSclOsScnprintf(str, end - str, "ALL Reset         (0)\n");
    str += DrvSclOsScnprintf(str, end - str, "========================SCL Debug Message BUTTON======================\n");
    str += DrvSclOsScnprintf(str, end - str, "========================SCL Debug Message LEVEL======================\n");
    str += DrvSclOsScnprintf(str, end - str, "default is level 1\n");
    str += DrvSclOsScnprintf(str, end - str, "-------------------------------IOCTL LEVEL---------------------------\n");
    str += DrvSclOsScnprintf(str, end - str, "0x 1 : SC1\n");
    str += DrvSclOsScnprintf(str, end - str, "0x 2 : SC2\n");
    str += DrvSclOsScnprintf(str, end - str, "0x 4 : SC3\n");
    str += DrvSclOsScnprintf(str, end - str, "0x 8 : VIP\n");
    str += DrvSclOsScnprintf(str, end - str, "0x10 : SC1HLEVEL\n");
    str += DrvSclOsScnprintf(str, end - str, "0x20 : SC2HLEVEL\n");
    str += DrvSclOsScnprintf(str, end - str, "0x40 : LCD\n");
    str += DrvSclOsScnprintf(str, end - str, "0x80 : ELSE\n");
    str += DrvSclOsScnprintf(str, end - str, "-------------------------------HVSP LEVEL---------------------------\n");
    str += DrvSclOsScnprintf(str, end - str, "0x 1 : HVSP1\n");
    str += DrvSclOsScnprintf(str, end - str, "0x 2 : HVSP2\n");
    str += DrvSclOsScnprintf(str, end - str, "0x 4 : HVSP3\n");
    str += DrvSclOsScnprintf(str, end - str, "0x 8 : ELSE\n");
    str += DrvSclOsScnprintf(str, end - str, "-------------------------------SCLDMA LEVEL-------------------------\n");
    str += DrvSclOsScnprintf(str, end - str, "0x 1 : SC1 FRM\n");
    str += DrvSclOsScnprintf(str, end - str, "0x 2 : SC1 SNP \n");
    str += DrvSclOsScnprintf(str, end - str, "0x 4 : SC2 FRM\n");
    str += DrvSclOsScnprintf(str, end - str, "0x 8 : SC3 FRM\n");
    str += DrvSclOsScnprintf(str, end - str, "0x10 : SC1 FRM HL\n");
    str += DrvSclOsScnprintf(str, end - str, "0x20 : SC1 SNP HL\n");
    str += DrvSclOsScnprintf(str, end - str, "0x40 : SC2 FRM HL\n");
    str += DrvSclOsScnprintf(str, end - str, "0x80 : ELSE\n");
    str += DrvSclOsScnprintf(str, end - str, "-------------------------------VIP LEVEL(IOlevel)-------------------\n");
    str += DrvSclOsScnprintf(str, end - str, "0x 1 : NORMAL\n");
    str += DrvSclOsScnprintf(str, end - str, "0x 2 : VIP LOG \n");
    str += DrvSclOsScnprintf(str, end - str, "0x 4 : VIP SUSPEND\n");
    str += DrvSclOsScnprintf(str, end - str, "0x 8 : ELSE\n");
    str += DrvSclOsScnprintf(str, end - str, "------------------------------MULTI LEVEL---------------------------\n");
    str += DrvSclOsScnprintf(str, end - str, "0x 1 : SC1\n");
    str += DrvSclOsScnprintf(str, end - str, "0x 2 : SC3\n");
    str += DrvSclOsScnprintf(str, end - str, "0x 4 : DISP\n");
    str += DrvSclOsScnprintf(str, end - str, "0x 8 : ELSE\n");
    str += DrvSclOsScnprintf(str, end - str, "-------------------------------PQ LEVEL---------------------------\n");
    str += DrvSclOsScnprintf(str, end - str, "0x 1 : befor crop\n");
    str += DrvSclOsScnprintf(str, end - str, "0x 2 : color eng\n");
    str += DrvSclOsScnprintf(str, end - str, "0x 4 : VIP Y\n");
    str += DrvSclOsScnprintf(str, end - str, "0x 8 : VIP C\n");
    str += DrvSclOsScnprintf(str, end - str, "0x10 : AIP\n");
    str += DrvSclOsScnprintf(str, end - str, "0x20 : AIP post\n");
    str += DrvSclOsScnprintf(str, end - str, "0x40 : HVSP\n");
    str += DrvSclOsScnprintf(str, end - str, "0x80 : ELSE\n");
    str += DrvSclOsScnprintf(str, end - str, "-------------------------------SCLIRQ LEVEL(drvlevel)---------------\n");
    str += DrvSclOsScnprintf(str, end - str, "0x 1 : NORMAL\n");
    str += DrvSclOsScnprintf(str, end - str, "0x 2 : SC1RINGA \n");
    str += DrvSclOsScnprintf(str, end - str, "0x 4 : SC1RINGN\n");
    str += DrvSclOsScnprintf(str, end - str, "0x 8 : SC1SINGLE\n");
    str += DrvSclOsScnprintf(str, end - str, "0x10 : SC2RINGA\n");
    str += DrvSclOsScnprintf(str, end - str, "0x20 : SC2RINGN \n");
    str += DrvSclOsScnprintf(str, end - str, "0x40 : SC3SINGLE\n");
    str += DrvSclOsScnprintf(str, end - str, "0x80 : ELSE\n");
    str += DrvSclOsScnprintf(str, end - str, "-------------------------------CMDQ LEVEL(drvlevel)-----------------\n");
    str += DrvSclOsScnprintf(str, end - str, "0x 1 : LOW\n");
    str += DrvSclOsScnprintf(str, end - str, "0x 2 : NORMAL \n");
    str += DrvSclOsScnprintf(str, end - str, "0x 4 : HIGH\n");
    str += DrvSclOsScnprintf(str, end - str, "0x 8 : ISR\n");
    str += DrvSclOsScnprintf(str, end - str, "0x10 : ISR check\n");
    str += DrvSclOsScnprintf(str, end - str, "0x10 : SRAM check\n");
    str += DrvSclOsScnprintf(str, end - str, "\n");
    str += DrvSclOsScnprintf(str, end - str, "========================SCL Debug Message LEVEL======================\n");
    end = end;
    return (str - buf);
}
ssize_t MDrvSclHvspIntsShow(char *buf)
{
    char *str = buf;
    char *end = buf + PAGE_SIZE;
    DrvSclHvspScIntsType_t stInts;
    u32 u32val;
    stInts = DrvSclHvspGetSclInts();
    str += DrvSclOsScnprintf(str, end - str, "========================SCL INTS======================\n");
    str += DrvSclOsScnprintf(str, end - str, "AFF          Count: %lu\n",stInts.u32AffCount);
    str += DrvSclOsScnprintf(str, end - str, "DMAERROR     Count: %lu\n",stInts.u32ErrorCount);
    str += DrvSclOsScnprintf(str, end - str, "ISPIn        Count: %lu\n",stInts.u32ISPInCount);
    str += DrvSclOsScnprintf(str, end - str, "ISPDone      Count: %lu\n",stInts.u32ISPDoneCount);
    str += DrvSclOsScnprintf(str, end - str, "DIFF         Count: %lu\n",(stInts.u32ISPInCount-stInts.u32ISPDoneCount));
    str += DrvSclOsScnprintf(str, end - str, "ResetCount   Count: %hhu\n",stInts.u8CountReset);
    str += DrvSclOsScnprintf(str, end - str, "SC1FrmDone   Count: %lu\n",stInts.u32SC1FrmDoneCount);
    str += DrvSclOsScnprintf(str, end - str, "SC1SnpDone   Count: %lu\n",stInts.u32SC1SnpDoneCount);
    str += DrvSclOsScnprintf(str, end - str, "SC2FrmDone   Count: %lu\n",stInts.u32SC2FrmDoneCount);
    str += DrvSclOsScnprintf(str, end - str, "SC2Frm2Done  Count: %lu\n",stInts.u32SC2Frm2DoneCount);
    str += DrvSclOsScnprintf(str, end - str, "SC3Done      Count: %lu\n",stInts.u32SC3DoneCount);
    str += DrvSclOsScnprintf(str, end - str, "SCLMainDone  Count: %lu\n",stInts.u32SCLMainDoneCount);
    if(stInts.u32SC1FrmDoneCount)
    {
        u32val = (u32)(stInts.u32SC1FrmActiveTime/stInts.u32SC1FrmDoneCount);
    }
    else
    {
        u32val = stInts.u32SC1FrmActiveTime;
    }
    str += DrvSclOsScnprintf(str, end - str, "SC1Frm       ActiveTime: %lu (us)\n",u32val);
    if(stInts.u32SC1SnpDoneCount)
    {
        u32val = (u32)(stInts.u32SC1SnpActiveTime/stInts.u32SC1SnpDoneCount);
    }
    else
    {
        u32val = stInts.u32SC1SnpActiveTime;
    }
    str += DrvSclOsScnprintf(str, end - str, "SC1Snp       ActiveTime: %lu (us)\n",u32val);
    if(stInts.u32SC2FrmDoneCount)
    {
        u32val = (u32)(stInts.u32SC2FrmActiveTime/stInts.u32SC2FrmDoneCount);
    }
    else
    {
        u32val = stInts.u32SC2FrmDoneCount;
    }
    str += DrvSclOsScnprintf(str, end - str, "SC2Frm       ActiveTime: %lu (us)\n",u32val);
    if(stInts.u32SC2Frm2DoneCount)
    {
        u32val = (u32)(stInts.u32SC2Frm2ActiveTime/stInts.u32SC2Frm2DoneCount);
    }
    else
    {
        u32val = stInts.u32SC2Frm2DoneCount;
    }
    str += DrvSclOsScnprintf(str, end - str, "SC2Frm2      ActiveTime: %lu (us)\n",u32val);
    if(stInts.u32SC3DoneCount)
    {
        u32val = (u32)(stInts.u32SC3ActiveTime/stInts.u32SC3DoneCount);
    }
    else
    {
        u32val = stInts.u32SC3DoneCount;
    }
    str += DrvSclOsScnprintf(str, end - str, "SC3          ActiveTime: %lu (us)\n",u32val);
    if(stInts.u32SCLMainDoneCount)
    {
        u32val = (u32)(stInts.u32SCLMainActiveTime/stInts.u32SCLMainDoneCount);
    }
    else
    {
        u32val = stInts.u32SCLMainActiveTime;
    }
    str += DrvSclOsScnprintf(str, end - str, "SCLMain       ActiveTime: %lu (us)\n",u32val);
    if(stInts.u32ISPDoneCount)
    {
        u32val = (u32)(stInts.u32ISPTime/stInts.u32ISPDoneCount);
    }
    else
    {
        u32val = stInts.u32ISPTime;
    }
    str += DrvSclOsScnprintf(str, end - str, "ISP       ActiveTime: %lu (us)\n",u32val);
    str += DrvSclOsScnprintf(str, end - str, "ISP       BlankingTime: %lu (us)\n",stInts.u32ISPBlankingTime);
    str += DrvSclOsScnprintf(str, end - str, "========================SCL INTS======================\n");
    end = end;
    return (str - buf);
}
void MDrvSclHvspScIqStore(const char *buf,MDrvSclHvspIdType_e enHVSP_ID)
{
    const char *str = buf;
    DrvSclHvspIdType_e enID;
    enID = enHVSP_ID == E_MDRV_SCLHVSP_ID_2 ? E_DRV_SCLHVSP_ID_2 :
           enHVSP_ID == E_MDRV_SCLHVSP_ID_3 ? E_DRV_SCLHVSP_ID_3 :
                                           E_DRV_SCLHVSP_ID_1;
    if(NULL!=buf)
    {
        //if(!)
        if((int)*str == 49)    //input 1
        {
            DrvSclHvspSclIq(enID,E_DRV_SCLHVSP_IQ_H_Tbl0);
            SCL_ERR( "[SCLIQ]Set %d\n",(int)*str);
        }
        else if((int)*str == 50)  //input 2
        {
            DrvSclHvspSclIq(enID,E_DRV_SCLHVSP_IQ_H_Tbl1);
            SCL_ERR( "[SCLIQ]Set %d\n",(int)*str);
        }
        else if((int)*str == 51)  //input 3
        {
            DrvSclHvspSclIq(enID,E_DRV_SCLHVSP_IQ_H_Tbl2);
            SCL_ERR( "[SCLIQ]Set %d\n",(int)*str);
        }
        else if((int)*str == 52)  //input 4
        {
            DrvSclHvspSclIq(enID,E_DRV_SCLHVSP_IQ_H_Tbl3);
            SCL_ERR( "[SCLIQ]Set %d\n",(int)*str);
        }
        else if((int)*str == 53)  //input 5
        {
            DrvSclHvspSclIq(enID,E_DRV_SCLHVSP_IQ_H_BYPASS);
            SCL_ERR( "[SCLIQ]Set %d\n",(int)*str);
        }
        else if((int)*str == 54)  //input 6
        {
            DrvSclHvspSclIq(enID,E_DRV_SCLHVSP_IQ_H_BILINEAR);
            SCL_ERR( "[SCLIQ]Set %d\n",(int)*str);
        }
        else if((int)*str == 55)  //input 7
        {
            DrvSclHvspSclIq(enID,E_DRV_SCLHVSP_IQ_V_Tbl0);
            SCL_ERR( "[SCLIQ]Set %d\n",(int)*str);
        }
        else if((int)*str == 56)  //input 8
        {
            DrvSclHvspSclIq(enID,E_DRV_SCLHVSP_IQ_V_Tbl1);
            SCL_ERR( "[SCLIQ]Set %d\n",(int)*str);
        }
        else if((int)*str == 57)  //input 9
        {
            DrvSclHvspSclIq(enID,E_DRV_SCLHVSP_IQ_V_Tbl2);
            SCL_ERR( "[SCLIQ]Set %d\n",(int)*str);
        }
        else if((int)*str == 65)  //input A
        {
            DrvSclHvspSclIq(enID,E_DRV_SCLHVSP_IQ_V_Tbl3);
            SCL_ERR( "[SCLIQ]Set %d\n",(int)*str);
        }
        else if((int)*str == 66)  //input B
        {
            DrvSclHvspSclIq(enID,E_DRV_SCLHVSP_IQ_V_BYPASS);
            SCL_ERR( "[SCLIQ]Set %d\n",(int)*str);
        }
        else if((int)*str == 67)  //input C
        {
            DrvSclHvspSclIq(enID,E_DRV_SCLHVSP_IQ_V_BILINEAR);
            SCL_ERR( "[SCLIQ]Set %d\n",(int)*str);
        }
    }
}
ssize_t MDrvSclHvspScIqShow(char *buf, MDrvSclHvspIdType_e enHVSP_ID)
{
    char *str = buf;
    char *end = buf + PAGE_SIZE;
    DrvSclHvspInformConfig_t sthvspformCfg;
    DrvSclHvspGetHvspAttribute(enHVSP_ID,&sthvspformCfg);
    str += DrvSclOsScnprintf(str, end - str, "------------------------SCL IQ----------------------\n");
    str += DrvSclOsScnprintf(str, end - str, "H en  :%hhx\n",sthvspformCfg.bEn&0x1);
    str += DrvSclOsScnprintf(str, end - str, "H function  :%hhx\n",(sthvspformCfg.bEn&0xC0)>>6);
    str += DrvSclOsScnprintf(str, end - str, "V en  :%hhx\n",(sthvspformCfg.bEn&0x2)>>1);
    str += DrvSclOsScnprintf(str, end - str, "V function  :%hhx\n",(sthvspformCfg.bEn&0x30)>>4);
    str += DrvSclOsScnprintf(str, end - str, "SC H   :1~6\n");
    str += DrvSclOsScnprintf(str, end - str, "SC table 0   :1\n");
    str += DrvSclOsScnprintf(str, end - str, "SC table 1   :2\n");
    str += DrvSclOsScnprintf(str, end - str, "SC table 2   :3\n");
    str += DrvSclOsScnprintf(str, end - str, "SC table 3   :4\n");
    str += DrvSclOsScnprintf(str, end - str, "SC bypass    :5\n");
    str += DrvSclOsScnprintf(str, end - str, "SC bilinear  :6\n");
    str += DrvSclOsScnprintf(str, end - str, "SC V :7~C\n");
    str += DrvSclOsScnprintf(str, end - str, "SC table 0   :7\n");
    str += DrvSclOsScnprintf(str, end - str, "SC table 1   :8\n");
    str += DrvSclOsScnprintf(str, end - str, "SC table 2   :9\n");
    str += DrvSclOsScnprintf(str, end - str, "SC table 3   :A\n");
    str += DrvSclOsScnprintf(str, end - str, "SC bypass    :B\n");
    str += DrvSclOsScnprintf(str, end - str, "SC bilinear  :C\n");
    str += DrvSclOsScnprintf(str, end - str, "------------------------SCL IQ----------------------\n");
    end = end;
    return (str - buf);
}
ssize_t MDrvSclHvspClkFrameworkShow(char *buf,MDrvSclHvspClkConfig_t* stclk)
{
    char *str = buf;
    char *end = buf + PAGE_SIZE;
    str += DrvSclOsScnprintf(str, end - str, "========================SCL CLK FRAMEWORK======================\n");
    str += DrvSclOsScnprintf(str, end - str, "echo 1 > clk :open force mode\n");
    str += DrvSclOsScnprintf(str, end - str, "echo 0 > clk :close force mode\n");
    str += DrvSclOsScnprintf(str, end - str, "echo 2 > clk :fclk1 172\n");
    str += DrvSclOsScnprintf(str, end - str, "echo 3 > clk :fclk1 86\n");
    str += DrvSclOsScnprintf(str, end - str, "echo E > clk :fclk1 216\n");
    str += DrvSclOsScnprintf(str, end - str, "echo 4 > clk :fclk1 open\n");
    str += DrvSclOsScnprintf(str, end - str, "echo 5 > clk :fclk1 close\n");
    str += DrvSclOsScnprintf(str, end - str, "echo 6 > clk :fclk2 172\n");
    str += DrvSclOsScnprintf(str, end - str, "echo 7 > clk :fclk2 86\n");
    str += DrvSclOsScnprintf(str, end - str, "echo F > clk :fclk2 216\n");
    str += DrvSclOsScnprintf(str, end - str, "echo 8 > clk :fclk2 open\n");
    str += DrvSclOsScnprintf(str, end - str, "echo 9 > clk :fclk2 close\n");
    str += DrvSclOsScnprintf(str, end - str, "echo : > clk :idclk ISP\n");
    str += DrvSclOsScnprintf(str, end - str, "echo D > clk :idclk BT656\n");
    str += DrvSclOsScnprintf(str, end - str, "echo B > clk :idclk open\n");
    str += DrvSclOsScnprintf(str, end - str, "echo = > clk :idclk close\n");
    str += DrvSclOsScnprintf(str, end - str, "echo C > clk :odclk MAX\n");
    str += DrvSclOsScnprintf(str, end - str, "echo ? > clk :odclk LPLL\n");
    str += DrvSclOsScnprintf(str, end - str, "echo @ > clk :odclk open\n");
    str += DrvSclOsScnprintf(str, end - str, "echo A > clk :odclk close\n");
    str += DrvSclOsScnprintf(str, end - str, "========================SCL CLK STATUS======================\n");
    str += DrvSclOsScnprintf(str, end - str, "force mode :%hhd\n",DrvSclHvspGetClkForcemode());
    if(DrvSclOsClkGetEnableCount((DrvSclOsClkStruct_t*)stclk->fclk1))
    {
        str += DrvSclOsScnprintf(str, end - str, "fclk1 open :%ld ,%ld\n",
            DrvSclOsClkGetEnableCount((DrvSclOsClkStruct_t*)stclk->fclk1),DrvSclOsClkGetRate((DrvSclOsClkStruct_t*)stclk->fclk1));
    }
    else
    {
        str += DrvSclOsScnprintf(str, end - str, "fclk1 close\n");
    }
    if(DrvSclOsClkGetEnableCount((DrvSclOsClkStruct_t*)stclk->fclk2))
    {
        str += DrvSclOsScnprintf(str, end - str, "fclk2 open :%ld,%ld\n",
            DrvSclOsClkGetEnableCount((DrvSclOsClkStruct_t*)stclk->fclk2),DrvSclOsClkGetRate((DrvSclOsClkStruct_t*)stclk->fclk2));
    }
    else
    {
        str += DrvSclOsScnprintf(str, end - str, "fclk2 close\n");
    }
    if(DrvSclOsClkGetEnableCount((DrvSclOsClkStruct_t*)stclk->idclk))
    {
        if(DrvSclOsClkGetRate((DrvSclOsClkStruct_t*)stclk->idclk) > 10)
        {
            str += DrvSclOsScnprintf(str, end - str, "idclk open :ISP\n");
        }
        else if(DrvSclOsClkGetRate((DrvSclOsClkStruct_t*)stclk->idclk) == 1)
        {
            str += DrvSclOsScnprintf(str, end - str, "idclk open :BT656\n");
        }
    }
    else
    {
        str += DrvSclOsScnprintf(str, end - str, "idclk close\n");
    }
    if(DrvSclOsClkGetEnableCount((DrvSclOsClkStruct_t*)stclk->odclk))
    {
        if(DrvSclOsClkGetRate((DrvSclOsClkStruct_t*)stclk->odclk) == 432000000)
        {
            str += DrvSclOsScnprintf(str, end - str, "odclk open LPLL:%ld\n",(DrvSclPnlGetLPLLDclk()/10000)*10000);
        }
        else
        {
            str += DrvSclOsScnprintf(str, end - str, "odclk open :%ld\n",DrvSclOsClkGetRate(stclk->odclk));
        }
    }
    else if(DrvSclPnlGetPnlOpen())
    {
        str += DrvSclOsScnprintf(str, end - str, "odclk manual open LPLL:%ld\n",(DrvSclPnlGetLPLLDclk()/10000)*10000);
    }
    else
    {
        str += DrvSclOsScnprintf(str, end - str, "odclk close\n");
    }
    end = end;
    return (str - buf);
}
ssize_t MDrvSclHvspFBMGShow(char *buf)
{
    char *str = buf;
    char *end = buf + PAGE_SIZE;
    str += DrvSclOsScnprintf(str, end - str, "------------------------SCL FBMG----------------------------\n");
    str += DrvSclOsScnprintf(str, end - str, "LDCPATH_ON    :1\n");
    str += DrvSclOsScnprintf(str, end - str, "LDCPATH_OFF   :2\n");
    str += DrvSclOsScnprintf(str, end - str, "DNRRead_ON    :3\n");
    str += DrvSclOsScnprintf(str, end - str, "DNRRead_OFF   :4\n");
    str += DrvSclOsScnprintf(str, end - str, "DNRWrite_ON   :5\n");
    str += DrvSclOsScnprintf(str, end - str, "DNRWrite_OFF  :6\n");
    str += DrvSclOsScnprintf(str, end - str, "DNRBuf1       :7\n");
    str += DrvSclOsScnprintf(str, end - str, "DNRBuf2       :8\n");
    str += DrvSclOsScnprintf(str, end - str, "UNLOCK        :9\n");
    str += DrvSclOsScnprintf(str, end - str, "PrvCrop_ON    :A\n");
    str += DrvSclOsScnprintf(str, end - str, "PrvCrop_OFF   :B\n");
    str += DrvSclOsScnprintf(str, end - str, "CIIR_ON       :C\n");
    str += DrvSclOsScnprintf(str, end - str, "CIIR_OFF      :D\n");
    str += DrvSclOsScnprintf(str, end - str, "LOCK          :E\n");
    str += DrvSclOsScnprintf(str, end - str, "------------------------SCL FBMG----------------------------\n");
    end = end;
    return (str - buf);
}
ssize_t MDrvSclHvspOdShow(char *buf)
{
    char *str = buf;
    char *end = buf + PAGE_SIZE;
    u8 u8idx = 0;
    u8 u8idy = 0;
    u8 u8idz = 0;
    u16 u16val = 0;
    str += DrvSclOsScnprintf(str, end - str, "------------------------SCL OD----------------------------\n");
    str += DrvSclOsScnprintf(str, end - str,
        "------0---------320-------640-------960-------1280------1600------1920------2240------2560\n");
    for(u8idy = 0;u8idy<=(gstHvspScalingCfg.stCropWin.u16Height/32)+1;u8idy++)
    {
        if(u8idy==10)
        {
            str += DrvSclOsScnprintf(str, end - str, "|320  ");
        }
        else if(u8idy==20)
        {
            str += DrvSclOsScnprintf(str, end - str, "|640  ");
        }
        else if(u8idy==30)
        {
            str += DrvSclOsScnprintf(str, end - str, "|960  ");
        }
        else if(u8idy==40)
        {
            str += DrvSclOsScnprintf(str, end - str, "|1280 ");
        }
        else if(u8idy==50)
        {
            str += DrvSclOsScnprintf(str, end - str, "|1600 ");
        }
        else
        {
            str += DrvSclOsScnprintf(str, end - str, "|     ");
        }
        for(u8idx =0;u8idx<=(gstHvspScalingCfg.stCropWin.u16Width/(16*32));u8idx++)
        {
            u16val = 0;
            u16val = DrvSclHvspPriMaskGetSram(u8idx, u8idy);
            for(u8idz = 0;u8idz<16;u8idz++)
            {
                if(u16val &(0x1 <<u8idz))
                {
                    str += DrvSclOsScnprintf(str, end - str, "1");
                }
                else
                {
                    str += DrvSclOsScnprintf(str, end - str, "0");
                }
            }
        }
        str += DrvSclOsScnprintf(str, end - str, "\n");
    }
    str += DrvSclOsScnprintf(str, end - str, "------------------------SCL OD----------------------------\n");
    end = end;
    return (str - buf);
}
ssize_t MDrvSclHvspOsdShow(char *buf)
{
    char *str = buf;
    char *end = buf + PAGE_SIZE;
    u8 idx;
    DrvSclHvspOsdConfig_t stOsdCfg;
    DrvSclHvspGetOsdAttribute(E_DRV_SCLHVSP_ID_1,&stOsdCfg);
    str += DrvSclOsScnprintf(str, end - str, "------------------------SCL OSD----------------------------\n");
    str += DrvSclOsScnprintf(str, end - str, "ONOFF     :%hhd\n",stOsdCfg.stOsdOnOff.bOSDEn);
    if(stOsdCfg.enOSD_loc)
    {
        str += DrvSclOsScnprintf(str, end - str, "Locate: Before\n");
    }
    else
    {
        str += DrvSclOsScnprintf(str, end - str, "Locate: After\n");
    }
    str += DrvSclOsScnprintf(str, end - str, "Bypass    :%hhd\n",stOsdCfg.stOsdOnOff.bOSDBypass);
    str += DrvSclOsScnprintf(str, end - str, "WTM Bypass:%hhd\n",stOsdCfg.stOsdOnOff.bWTMBypass);
    str += DrvSclOsScnprintf(str, end - str, "------------------------SCL OD----------------------------\n");
    if(gu8PriMaskNum[gu8PriMaskInst])
    {
        str += DrvSclOsScnprintf(str, end - str, "ONOFF     :ON @:%hhd\n",gu8PriMaskInst);
        str += DrvSclOsScnprintf(str, end - str, "W RATIO   :%ld\n",
            (u32)CAL_HVSP_RATIO(gstHvspScalingCfg.stCropWin.u16Width,gstHvspScalingCfg.u16Dsp_Width));
        str += DrvSclOsScnprintf(str, end - str, "H RATIO   :%ld\n",
            (u32)CAL_HVSP_RATIO(gstHvspScalingCfg.stCropWin.u16Height,gstHvspScalingCfg.u16Dsp_Height));
        for(idx=0;idx<gu8PriMaskNum[gu8PriMaskInst];idx++)
        {
            if(gstHvspPriMaskCfg[gu8PriMaskInst][idx].u8idx == idx)
            {
                str += DrvSclOsScnprintf(str, end - str, "IDX     :%hhd, ON:%hhd ,(%hd,%hd,%hd,%hd)\n",
                    idx,gstHvspPriMaskCfg[gu8PriMaskInst][idx].bMask,gstHvspPriMaskCfg[gu8PriMaskInst][idx].stMaskWin.u16X
                    ,gstHvspPriMaskCfg[gu8PriMaskInst][idx].stMaskWin.u16Y,gstHvspPriMaskCfg[gu8PriMaskInst][idx].stMaskWin.u16Width
                    ,gstHvspPriMaskCfg[gu8PriMaskInst][idx].stMaskWin.u16Height);
            }
        }
    }
    else
    {
        str += DrvSclOsScnprintf(str, end - str, "ONOFF     :OFF\n");
    }
    str += DrvSclOsScnprintf(str, end - str, "------------------------SCL OSD----------------------------\n");
    str += DrvSclOsScnprintf(str, end - str, "echo 1 > OSD :open OSD\n");
    str += DrvSclOsScnprintf(str, end - str, "echo 0 > OSD :close OSD\n");
    str += DrvSclOsScnprintf(str, end - str, "echo 2 > OSD :Set OSD before\n");
    str += DrvSclOsScnprintf(str, end - str, "echo 3 > OSD :Set OSD After\n");
    str += DrvSclOsScnprintf(str, end - str, "echo 4 > OSD :Set OSD Bypass\n");
    str += DrvSclOsScnprintf(str, end - str, "echo 5 > OSD :Set OSD Bypass Off\n");
    str += DrvSclOsScnprintf(str, end - str, "echo 6 > OSD :Set OSD WTM Bypass\n");
    str += DrvSclOsScnprintf(str, end - str, "echo 7 > OSD :Set OSD WTM Bypass Off\n");
    str += DrvSclOsScnprintf(str, end - str, "echo 8 > OSD :Set Privacy Mask On\n");
    str += DrvSclOsScnprintf(str, end - str, "echo 9 > OSD :Set Privacy Mask Off\n");
    str += DrvSclOsScnprintf(str, end - str, "------------------------SCL OSD----------------------------\n");
    end = end;
    return (str - buf);
}
void MDrvSclHvspOsdStore(const char *buf,MDrvSclHvspIdType_e enHVSP_ID)
{
    const char *str = buf;
    DrvSclHvspOsdConfig_t stOSdCfg;
    DrvSclHvspIdType_e enID;
    enID = enHVSP_ID == E_MDRV_SCLHVSP_ID_2 ? E_DRV_SCLHVSP_ID_2 :
           enHVSP_ID == E_MDRV_SCLHVSP_ID_3 ? E_DRV_SCLHVSP_ID_3 :
                                           E_DRV_SCLHVSP_ID_1;

    DrvSclHvspGetOsdAttribute(enID,&stOSdCfg);
    if((int)*str == 49)    //input 1
    {
        SCL_ERR( "[OSD]open OSD %d\n",(int)*str);
        stOSdCfg.stOsdOnOff.bOSDEn = 1;
        DrvSclHvspSetOsdConfig(enID, stOSdCfg);
    }
    else if((int)*str == 48)  //input 0
    {
        SCL_ERR( "[OSD]close OSD %d\n",(int)*str);
        stOSdCfg.stOsdOnOff.bOSDEn = 0;
        DrvSclHvspSetOsdConfig(enID, stOSdCfg);
    }
    else if((int)*str == 50)  //input 2
    {
        SCL_ERR( "[OSD]Set OSD before %d\n",(int)*str);
        stOSdCfg.enOSD_loc = E_DRV_SCLHVSP_OSD_LOC_BEFORE;
        DrvSclHvspSetOsdConfig(enID, stOSdCfg);
    }
    else if((int)*str == 51)  //input 3
    {
        SCL_ERR( "[OSD]Set OSD After %d\n",(int)*str);
        stOSdCfg.enOSD_loc = E_DRV_SCLHVSP_OSD_LOC_AFTER;
        DrvSclHvspSetOsdConfig(enID, stOSdCfg);
    }
    else if((int)*str == 52)  //input 4
    {
        SCL_ERR( "[OSD]Set OSD Bypass %d\n",(int)*str);
        stOSdCfg.stOsdOnOff.bOSDBypass = 1;
        DrvSclHvspSetOsdConfig(enID, stOSdCfg);
    }
    else if((int)*str == 53)  //input 5
    {
        SCL_ERR( "[OSD]Set OSD Bypass Off %d\n",(int)*str);
        stOSdCfg.stOsdOnOff.bOSDBypass = 0;
        DrvSclHvspSetOsdConfig(enID, stOSdCfg);
    }
    else if((int)*str == 54)  //input 6
    {
        SCL_ERR( "[OSD]Set OSD WTM Bypass %d\n",(int)*str);
        stOSdCfg.stOsdOnOff.bWTMBypass = 1;
        DrvSclHvspSetOsdConfig(enID, stOSdCfg);
    }
    else if((int)*str == 55)  //input 7
    {
        SCL_ERR( "[OSD]Set OSD WTM Bypass Off %d\n",(int)*str);
        stOSdCfg.stOsdOnOff.bWTMBypass = 0;
        DrvSclHvspSetOsdConfig(enID, stOSdCfg);
    }
    else if((int)*str == 56)  //input 8
    {
        SCL_ERR( "[OSD]Set OD ON %d\n",(int)*str);
        DrvSclHvspSetPriMaskTrigger(E_DRV_SCLHVSP_PRIMASK_ENABLE);
    }
    else if((int)*str == 57)  //input 9
    {
        SCL_ERR( "[OSD]Set OD Off %d\n",(int)*str);
        DrvSclHvspSetPriMaskTrigger(E_DRV_SCLHVSP_PRIMASK_ONLYHWOFF);
    }
}
ssize_t MDrvSclHvspLockShow(char *buf)
{
    char *str = buf;
    char *end = buf + PAGE_SIZE;
    str += DrvSclOsScnprintf(str, end - str, "------------------------SCL Lock----------------------------\n");
    str = DrvSclOsCheckMutex(str,end);
    str += DrvSclOsScnprintf(str, end - str, "------------------------SCL Lock----------------------------\n");
    end = end;
    return (str - buf);
}
//#endif
