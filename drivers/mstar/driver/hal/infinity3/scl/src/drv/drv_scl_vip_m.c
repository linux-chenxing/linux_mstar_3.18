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
#define _MDRV_VIP_C

#include "drv_scl_os.h"
#include "drv_scl_dbg.h"
#include "hal_scl_util.h"

#include "drv_scl_vip.h"
#include "drv_scl_cmdq.h"
#include "drv_scl_pq_define.h"
#include "drv_scl_pq_declare.h"
#include "drv_scl_pq.h"

#include "drv_scl_vip_m_st.h"
#include "drv_scl_vip_m.h"
#include "drv_scl_irq_st.h"
#include "drv_scl_irq.h"
#include "hal_scl_reg.h"



//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define MDrvSclCmdqMutexLock(enIP,bEn)          (bEn ? DrvSclCmdqGetModuleMutex(enIP,1) : 0)
#define MDrvSclCmdqMutexUnlock(enIP,bEn)        (bEn ? DrvSclCmdqGetModuleMutex(enIP,0) : 0)
#define MDrvSclVipMutexLock()                   DrvSclOsObtainMutex(_MVIP_Mutex,SCLOS_WAIT_FOREVER)
#define MDrvSclVipMutexUnlock()                 DrvSclOsReleaseMutex(_MVIP_Mutex)

#define _IsFrameBufferAllocatedReady()          (gu8FrameBufferReadyNum &0x3)
#define _IsCiirBufferAllocateReady()            (gu8FrameBufferReadyNum &0xC)
#define _IsCmdqNeedToReturnOrigin()             (gbVIPCheckCMDQorPQ == E_MDRV_SCLVIP_CMDQ_CHECK_RETURN_ORI)
#define _IsCmdqAlreadySetting()                 (gbVIPCheckCMDQorPQ == E_MDRV_SCLVIP_CMDQ_CHECK_ALREADY_SETINNG)
#define _IsSetCmdq()                            (gbVIPCheckCMDQorPQ >= E_MDRV_SCLVIP_CMDQ_CHECK_ALREADY_SETINNG)
#define _IsAutoSetting()                        (gbVIPCheckCMDQorPQ == E_MDRV_SCLVIP_CMDQ_CHECK_AUTOSETTING\
                                                 || gbVIPCheckCMDQorPQ == E_MDRV_SCLVIP_CMDQ_CHECK_PQ)
#define _SetSuspendResetFlag(u32flag)           (gstSupCfg.bresetflag |= u32flag)
#define _IsSuspendResetFlag(enType)             ((gstSupCfg.bresetflag &enType)>0)
#define _SetSuspendAipResetFlag(u32flag)        (gstSupCfg.bAIPreflag |= (0x1 << u32flag))
#define _IsSuspendAipResetFlag(enType)          (gstSupCfg.bAIPreflag &(0x1 << enType))
#define _SetAipByPassFlag(u32flag)              (gu32AIPOpenBypass |= (0x1 << u32flag))
#define _IsAipByPassFlag(u32flag)               (gu32AIPOpenBypass &(0x1 << u32flag))
#define _IsCheckPQorCMDQmode()                  (gbVIPCheckCMDQorPQ)
#define _IsNotToCheckPQorCMDQmode()             (!gbVIPCheckCMDQorPQ)
#define _IsOpenVIPBypass()                      (gu32OpenBypass)
#define _IsNotOpenVIPBypass(enType)             (!(gu32OpenBypass & enType))
#define AIPOffset                               PQ_IP_YEE_Main
#define _GetAipOffset(u32Type)                  (u32Type +AIPOffset)
#define _IsVipCmdqSetFlag(enType)               (gbVIPCMDQSet &enType)
#define _IsAipCmdqSetFlag(enType)               (gbAIPCMDQSet &(0x1 << enType))
#define _SetVipCmdqSetFlag(u32flag)             (gbVIPCMDQSet |= u32flag)
#define _SetAipCmdqSetFlag(u32flag)             (gbAIPCMDQSet |= (0x1 << u32flag))
#define _ResetVipCmdqSetFlag(u32flag)           (gbVIPCMDQSet &= ~(u32flag))
#define _ResetAipCmdqSetFlag(u32flag)           (gbAIPCMDQSet &= ~(0x1 << (u32flag)))


//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
/////////////////
/// gstSupCfg
/// save data for suspend/resume and test Qmap.
////////////////
MDrvSclVipSuspendConfig_t gstSupCfg;
/////////////////
/// gbVIPCheckCMDQorPQ
/// if True ,is check PQ mode.
////////////////
bool gbVIPCheckCMDQorPQ = 0;
u32 gu32OpenBypass = 0;
u32 gu32AIPOpenBypass = 0;
u32 gbVIPCMDQSet;
u32 gbAIPCMDQSet;
MDrvSclVipDazaEvent_e genVipDazaEvent;
s32 _MVIP_Mutex = -1;
s32 _MVIP_TASK = -1;

//-------------------------------------------------------------------------------------------------
//  Local Function
//-------------------------------------------------------------------------------------------------
void _MDrvSclVipSetDazaTaskWork(MDrvSclVipDazaEvent_e enEvent)
{
    genVipDazaEvent |= enEvent;
    DrvSclOsSetTaskWork(_MVIP_TASK);
    //SCL_DBGERR("[VIP]%s :task:%hhx\n",__FUNCTION__,bret);
}

u16 _MDrvSclVipCioceStructSizeFromPqType(u8 u8PQIPIdx)
{
    u16 u16DataSize;
    switch(u8PQIPIdx)
    {
        case PQ_IP_LDC_Main:
            u16DataSize = PQ_IP_LDC_Size;
            break;
        case PQ_IP_LDC_422_444_422_Main:
            u16DataSize = PQ_IP_LDC_422_444_422_Size;
            break;
        case PQ_IP_NLM_Main:
            u16DataSize = PQ_IP_NLM_Size;
            break;
        case PQ_IP_422to444_Main:
            u16DataSize = PQ_IP_422to444_Size;
            break;
        case PQ_IP_VIP_Main:
            u16DataSize = PQ_IP_VIP_Size;
            break;
        case PQ_IP_VIP_LineBuffer_Main:
            u16DataSize = PQ_IP_VIP_LineBuffer_Size;
            break;
        case PQ_IP_VIP_HLPF_Main:
            u16DataSize = PQ_IP_VIP_HLPF_Size;
            break;
        case PQ_IP_VIP_HLPF_dither_Main:
            u16DataSize = PQ_IP_VIP_HLPF_dither_Size;
            break;
        case PQ_IP_VIP_VLPF_coef1_Main:
        case PQ_IP_VIP_VLPF_coef2_Main:
            u16DataSize = PQ_IP_VIP_VLPF_coef1_Size;
            break;
        case PQ_IP_VIP_VLPF_dither_Main:
            u16DataSize = PQ_IP_VIP_VLPF_dither_Size;
            break;
        case PQ_IP_VIP_Peaking_Main:
            u16DataSize = PQ_IP_VIP_Peaking_Size;
            break;
        case PQ_IP_VIP_Peaking_band_Main:
            u16DataSize = PQ_IP_VIP_Peaking_band_Size;
            break;
        case PQ_IP_VIP_Peaking_adptive_Main:
            u16DataSize = PQ_IP_VIP_Peaking_adptive_Size;
            break;
        case PQ_IP_VIP_Peaking_Pcoring_Main:
            u16DataSize = PQ_IP_VIP_Peaking_Pcoring_Size;
            break;
        case PQ_IP_VIP_Peaking_Pcoring_ad_Y_Main:
            u16DataSize = PQ_IP_VIP_Peaking_Pcoring_ad_Y_Size;
            break;
        case PQ_IP_VIP_Peaking_gain_Main:
            u16DataSize = PQ_IP_VIP_Peaking_gain_Size;
            break;
        case PQ_IP_VIP_Peaking_gain_ad_Y_Main:
            u16DataSize = PQ_IP_VIP_Peaking_gain_ad_Y_Size;
            break;
        case PQ_IP_VIP_YC_gain_offset_Main:
            u16DataSize = PQ_IP_VIP_YC_gain_offset_Size;
            break;
        case PQ_IP_VIP_LCE_Main:
            u16DataSize = PQ_IP_VIP_LCE_Size;
            break;
        case PQ_IP_VIP_LCE_dither_Main:
            u16DataSize = PQ_IP_VIP_LCE_dither_Size;
            break;
        case PQ_IP_VIP_LCE_setting_Main:
            u16DataSize = PQ_IP_VIP_LCE_setting_Size;
            break;
        case PQ_IP_VIP_LCE_curve_Main:
            u16DataSize = PQ_IP_VIP_LCE_curve_Size;
            break;
        case PQ_IP_VIP_DLC_Main:
            u16DataSize = PQ_IP_VIP_DLC_Size;
            break;
        case PQ_IP_VIP_DLC_dither_Main:
            u16DataSize = PQ_IP_VIP_DLC_dither_Size;
            break;
        case PQ_IP_VIP_DLC_His_range_Main:
            u16DataSize = PQ_IP_VIP_DLC_His_range_Size;
            break;
        case PQ_IP_VIP_DLC_His_rangeH_Main:
            u16DataSize = PQ_IP_VIP_DLC_His_rangeH_Size;
            break;
        case PQ_IP_VIP_DLC_His_rangeV_Main:
            u16DataSize = PQ_IP_VIP_DLC_His_rangeV_Size;
            break;
        case PQ_IP_VIP_DLC_PC_Main:
            u16DataSize = PQ_IP_VIP_DLC_PC_Size;
            break;
        case PQ_IP_VIP_UVC_Main:
            u16DataSize = PQ_IP_VIP_UVC_Size;
            break;
        case PQ_IP_VIP_FCC_full_range_Main:
            u16DataSize = PQ_IP_VIP_FCC_full_range_Size;
            break;
        case PQ_IP_VIP_FCC_T1_Main:
        case PQ_IP_VIP_FCC_T2_Main:
        case PQ_IP_VIP_FCC_T3_Main:
        case PQ_IP_VIP_FCC_T4_Main:
        case PQ_IP_VIP_FCC_T5_Main:
        case PQ_IP_VIP_FCC_T6_Main:
        case PQ_IP_VIP_FCC_T7_Main:
        case PQ_IP_VIP_FCC_T8_Main:
            u16DataSize = PQ_IP_VIP_FCC_T1_Size;
            break;
        case PQ_IP_VIP_FCC_T9_Main:
            u16DataSize = PQ_IP_VIP_FCC_T9_Size;
            break;
        case PQ_IP_VIP_IHC_Main:
            u16DataSize = PQ_IP_VIP_IHC_Size;
            break;
        case PQ_IP_VIP_IHC_Ymode_Main:
            u16DataSize = PQ_IP_VIP_IHC_Ymode_Size;
            break;
        case PQ_IP_VIP_IHC_dither_Main:
            u16DataSize = PQ_IP_VIP_IHC_dither_Size;
            break;
        case PQ_IP_VIP_IHC_SETTING_Main:
            u16DataSize = PQ_IP_VIP_IHC_SETTING_Size;
            break;
        case PQ_IP_VIP_ICC_Main:
            u16DataSize = PQ_IP_VIP_ICC_Size;
            break;
        case PQ_IP_VIP_ICC_Ymode_Main:
            u16DataSize = PQ_IP_VIP_ICC_Ymode_Size;
            break;
        case PQ_IP_VIP_ICC_dither_Main:
            u16DataSize = PQ_IP_VIP_ICC_dither_Size;
            break;
        case PQ_IP_VIP_ICC_SETTING_Main:
            u16DataSize = PQ_IP_VIP_ICC_SETTING_Size;
            break;
        case PQ_IP_VIP_Ymode_Yvalue_ALL_Main:
            u16DataSize = PQ_IP_VIP_Ymode_Yvalue_ALL_Size;
            break;
        case PQ_IP_VIP_Ymode_Yvalue_SETTING_Main:
            u16DataSize = PQ_IP_VIP_Ymode_Yvalue_SETTING_Size;
            break;
        case PQ_IP_VIP_IBC_Main:
            u16DataSize = PQ_IP_VIP_IBC_Size;
            break;
        case PQ_IP_VIP_IBC_dither_Main:
            u16DataSize = PQ_IP_VIP_IBC_dither_Size;
            break;
        case PQ_IP_VIP_IBC_SETTING_Main:
            u16DataSize = PQ_IP_VIP_IBC_SETTING_Size;
            break;
        case PQ_IP_VIP_ACK_Main:
            u16DataSize = PQ_IP_VIP_ACK_Size;
            break;
        case PQ_IP_VIP_YCbCr_Clip_Main:
            u16DataSize = PQ_IP_VIP_YCbCr_Clip_Size;
            break;
        default:
            u16DataSize = 0;

            break;
    }
    return u16DataSize;
}
MDrvSclVipSetPqConfig_t _MDrvSclVipFillPqCfgByType(u8 u8PQIPIdx, u8 *pData, u16 u16DataSize)
{
    MDrvSclVipSetPqConfig_t stSetPQCfg;
    stSetPQCfg.enPQIPType    = u8PQIPIdx;
    stSetPQCfg.pPointToCfg   = pData;
    stSetPQCfg.u32StructSize = u16DataSize;
    return stSetPQCfg;
}
bool MDrvSclVipGetIsBlankingRegion(void)
{
    return DrvSclVipGetIsBlankingRegion();
}

void _MDrvSclVipFillstFcfgBelongGlobal(MDrvSclVipConfigType_e enVIPtype,bool bEn,u8 u8framecnt)
{
    switch(enVIPtype)
    {
        case E_MDRV_SCLVIP_MCNR_CONFIG:
                gstSupCfg.stmcnr.stFCfg.bEn = bEn ;
                gstSupCfg.stmcnr.stFCfg.u8framecnt = u8framecnt;
            break;

        case E_MDRV_SCLVIP_ACK_CONFIG:
                gstSupCfg.stack.stFCfg.bEn = bEn ;
                gstSupCfg.stack.stFCfg.u8framecnt = u8framecnt;
            break;

        case E_MDRV_SCLVIP_IBC_CONFIG:
                gstSupCfg.stibc.stFCfg.bEn = bEn ;
                gstSupCfg.stibc.stFCfg.u8framecnt = u8framecnt;
            break;

        case E_MDRV_SCLVIP_IHCICC_CONFIG:
                gstSupCfg.stihcicc.stFCfg.bEn = bEn ;
                gstSupCfg.stihcicc.stFCfg.u8framecnt = u8framecnt;
            break;

        case E_MDRV_SCLVIP_ICC_CONFIG:
                gstSupCfg.sticc.stFCfg.bEn = bEn ;
                gstSupCfg.sticc.stFCfg.u8framecnt = u8framecnt;
            break;

        case E_MDRV_SCLVIP_IHC_CONFIG:
                gstSupCfg.stihc.stFCfg.bEn = bEn ;
                gstSupCfg.stihc.stFCfg.u8framecnt = u8framecnt;
            break;

        case E_MDRV_SCLVIP_FCC_CONFIG:
                gstSupCfg.stfcc.stFCfg.bEn = bEn ;
                gstSupCfg.stfcc.stFCfg.u8framecnt = u8framecnt;
            break;
        case E_MDRV_SCLVIP_UVC_CONFIG:
                gstSupCfg.stuvc.stFCfg.bEn = bEn ;
                gstSupCfg.stuvc.stFCfg.u8framecnt = u8framecnt;
            break;

        case E_MDRV_SCLVIP_DLC_CONFIG:
                gstSupCfg.stdlc.stFCfg.bEn = bEn ;
                gstSupCfg.stdlc.stFCfg.u8framecnt = u8framecnt;
            break;

        case E_MDRV_SCLVIP_DLC_HISTOGRAM_CONFIG:
                gstSupCfg.sthist.stFCfg.bEn = bEn ;
                gstSupCfg.sthist.stFCfg.u8framecnt = u8framecnt;
            break;

        case E_MDRV_SCLVIP_LCE_CONFIG:
                gstSupCfg.stlce.stFCfg.bEn = bEn ;
                gstSupCfg.stlce.stFCfg.u8framecnt = u8framecnt;
            break;

        case E_MDRV_SCLVIP_PEAKING_CONFIG:
                gstSupCfg.stpk.stFCfg.bEn = bEn ;
                gstSupCfg.stpk.stFCfg.u8framecnt = u8framecnt;
            break;

        case E_MDRV_SCLVIP_LDC_MD_CONFIG:
                gstSupCfg.stldcmd.stFCfg.bEn = bEn ;
                gstSupCfg.stldcmd.stFCfg.u8framecnt = u8framecnt;
            break;

        case E_MDRV_SCLVIP_LDC_DMAP_CONFIG:
                gstSupCfg.stldcdmap.stFCfg.bEn = bEn ;
                gstSupCfg.stldcdmap.stFCfg.u8framecnt = u8framecnt;
            break;

        case E_MDRV_SCLVIP_LDC_SRAM_CONFIG:
                gstSupCfg.stldcsram.stFCfg.bEn = bEn ;
                gstSupCfg.stldcsram.stFCfg.u8framecnt = u8framecnt;
            break;

        case E_MDRV_SCLVIP_NLM_CONFIG:
                gstSupCfg.stnlm.stFCfg.bEn = bEn ;
                gstSupCfg.stnlm.stFCfg.u8framecnt = u8framecnt;
            break;
        case E_MDRV_SCLVIP_LDC_CONFIG:
                gstSupCfg.stldc.stFCfg.bEn = bEn ;
                gstSupCfg.stldc.stFCfg.u8framecnt = u8framecnt;
            break;
        case E_MDRV_SCLVIP_CONFIG:
                gstSupCfg.stack.stFCfg.bEn = bEn ;
                gstSupCfg.stack.stFCfg.u8framecnt = u8framecnt;
            break;
        default:
                gstSupCfg.stvip.stFCfg.bEn = 0 ;
                gstSupCfg.stvip.stFCfg.u8framecnt = 0;
            break;

    }
}
void _MDrvSclVipCopyIpConfigToGlobal(MDrvSclVipConfigType_e enVIPtype,void *pCfg)
{
    MDrvSclVipSetPqConfig_t stSetPQCfg;
    if(_IsNotToCheckPQorCMDQmode())
    {
        stSetPQCfg = MDrvSclVipFillBasicStructSetPqCfg(enVIPtype,pCfg);
        DrvSclOsMemcpy(stSetPQCfg.pGolbalStructAddr, stSetPQCfg.pPointToCfg,stSetPQCfg.u32StructSize);
        _MDrvSclVipFillstFcfgBelongGlobal(enVIPtype,0,0);
        _SetSuspendResetFlag(enVIPtype);
    }
}
void _MDrvSclVipResetAlreadySetting(MDrvSclVipSetPqConfig_t stSetPQCfg)
{
    void * pvPQSetParameter;
    pvPQSetParameter = DrvSclOsMemalloc(stSetPQCfg.u32StructSize, GFP_KERNEL);
    DrvSclOsMemset(pvPQSetParameter, 0, stSetPQCfg.u32StructSize);
    stSetPQCfg.pfForSet(pvPQSetParameter);
    DrvSclOsMemFree(pvPQSetParameter);
}
void _MDrvSclVipModifyAipAlreadySetting(MDrvSclVipAipType_e enAIPtype, MDrvSclVipResetType_e enRe)
{
    MDrvSclVipAipConfig_t staip;
    bool bSet;
    u32 u32viraddr;
    bSet = (enRe == E_MDRV_SCLVIP_RESET_ZERO) ? 0 : 0xFF;
    staip = gstSupCfg.staip[enAIPtype];
    staip.u32Viraddr =(u32)DrvSclOsVirMemalloc(MDrv_PQ_GetIPRegCount(_GetAipOffset(enAIPtype)));
    DrvSclOsMemset((void*)staip.u32Viraddr, bSet, MDrv_PQ_GetIPRegCount(_GetAipOffset(enAIPtype)));
    if(enRe == E_MDRV_SCLVIP_RESET_ZERO)
    {
        MDrvSclVipSetAipConfig(staip);
    }
    else if(enRe == E_MDRV_SCLVIP_RESET_ALREADY)
    {
        if(gstSupCfg.staip[enAIPtype].u32Viraddr)
        {
            MDrvSclVipSetAipConfig(gstSupCfg.staip[enAIPtype]);
        }
    }
    else if (enRe == E_MDRV_SCLVIP_FILL_GOLBAL_FULL)
    {
        u32viraddr = gstSupCfg.staip[enAIPtype].u32Viraddr;
        gstSupCfg.staip[enAIPtype].u32Viraddr = staip.u32Viraddr;
        gstSupCfg.staip[enAIPtype].u16AIPType = enAIPtype;
        staip.u32Viraddr = u32viraddr;
    }
    DrvSclOsVirMemFree((void *)staip.u32Viraddr);
}
void _MDrvSclVipFillAutoSetStruct(MDrvSclVipSetPqConfig_t stSetPQCfg)
{
    void * pvPQSetParameter;
    pvPQSetParameter = DrvSclOsMemalloc(stSetPQCfg.u32StructSize, GFP_KERNEL);
    DrvSclOsMemset(pvPQSetParameter, 0xFF, stSetPQCfg.u32StructSize);
    DrvSclOsMemcpy(stSetPQCfg.pGolbalStructAddr, pvPQSetParameter, stSetPQCfg.u32StructSize);
    DrvSclOsMemFree(pvPQSetParameter);
}
void _MDrvSclVipModifyMcnrAlreadySetting(MDrvSclVipSetPqConfig_t stSetPQCfg, MDrvSclVipResetType_e enRe)
{
    MDrvSclVipMcnrConfig_t stmcnr;
    bool bSet;
    u32 u32viraddr;
    SCL_ERR("[MVIP]%s",__FUNCTION__);
    bSet = (enRe == E_MDRV_SCLVIP_RESET_ZERO) ? 0 : 0xFF;
    stmcnr = gstSupCfg.stmcnr;
    stmcnr.u32Viraddr = (u32)DrvSclOsVirMemalloc(MDrv_PQ_GetIPRegCount(PQ_IP_MCNR_Main));
    stmcnr.bEnMCNR = (enRe == E_MDRV_SCLVIP_RESET_ZERO) ? 0 :
                     (enRe == E_MDRV_SCLVIP_FILL_GOLBAL_FULL) ? 1 :
                        gstSupCfg.stmcnr.bEnMCNR;

    stmcnr.bEnCIIR = (enRe == E_MDRV_SCLVIP_RESET_ZERO) ? 0 :
                     (enRe == E_MDRV_SCLVIP_FILL_GOLBAL_FULL) ? 1 :
                        gstSupCfg.stmcnr.bEnCIIR;
    stmcnr.stFCfg.bEn = 0;
    stmcnr.stFCfg.u8framecnt = 0;
    DrvSclOsMemset((void*)stmcnr.u32Viraddr,  bSet, MDrv_PQ_GetIPRegCount(PQ_IP_MCNR_Main));
    MDrvSclVipMutexLock();
    gu32OpenBypass |= E_MDRV_SCLVIP_MCNR_CONFIG;
    MDrvSclVipMutexUnlock();
    if(enRe == E_MDRV_SCLVIP_RESET_ZERO)
    {
        MDrvSclVipSetMcnrConfig(&stmcnr);
    }
    else if(enRe == E_MDRV_SCLVIP_RESET_ALREADY)
    {
        MDrvSclVipSetMcnrConfig(&gstSupCfg.stmcnr);
    }
    else if (enRe == E_MDRV_SCLVIP_FILL_GOLBAL_FULL)
    {
        u32viraddr = gstSupCfg.stmcnr.u32Viraddr;
        gstSupCfg.stmcnr.u32Viraddr = stmcnr.u32Viraddr;
        stmcnr.u32Viraddr = u32viraddr;
        gstSupCfg.stmcnr.bEnMCNR = stmcnr.bEnMCNR;
        gstSupCfg.stmcnr.bEnCIIR = stmcnr.bEnCIIR;
    }
    DrvSclOsVirMemFree((void *)stmcnr.u32Viraddr);
}
void _MDrvSclVipPrepareCheckSetting(MDrvSclVipConfigType_e enVIPtype)
{
    MDrvSclVipSetPqConfig_t stSetPQCfg;
    bool bEn;
    u8 u8framecount;
    bEn = (_IsSetCmdq()) ? 1 : 0 ;
    u8framecount = 0;
    stSetPQCfg = MDrvSclVipFillBasicStructSetPqCfg(enVIPtype,NULL);
    if(_IsCmdqAlreadySetting())
    {
        if(enVIPtype ==E_MDRV_SCLVIP_MCNR_CONFIG)
        {
            _MDrvSclVipModifyMcnrAlreadySetting(stSetPQCfg,E_MDRV_SCLVIP_RESET_ZERO);
        }
        else
        {
            _MDrvSclVipResetAlreadySetting(stSetPQCfg);
        }
    }
    else if(_IsAutoSetting())
    {
        if(enVIPtype ==E_MDRV_SCLVIP_MCNR_CONFIG)
        {
            _MDrvSclVipModifyMcnrAlreadySetting(stSetPQCfg,E_MDRV_SCLVIP_FILL_GOLBAL_FULL);
        }
        else
        {
            _MDrvSclVipFillAutoSetStruct(stSetPQCfg);
            SCL_DBG(SCL_DBG_LV_VIP()&EN_DBGMG_VIPLEVEL_VIPSUP,
                "[VIP]%x:addr:%lx\n",enVIPtype,(u32)stSetPQCfg.pGolbalStructAddr);
        }
    }
    _MDrvSclVipFillstFcfgBelongGlobal(enVIPtype,bEn,u8framecount);
}
void _MDrvSclVipPrepareAipCheckSetting(MDrvSclVipAipType_e enAIPtype)
{
    bool bEn;
    u8 u8framecount;
    bEn = (_IsSetCmdq()) ? 1 : 0 ;
    u8framecount = 0;
    if(_IsCmdqAlreadySetting())
    {
        _MDrvSclVipModifyAipAlreadySetting(enAIPtype,E_MDRV_SCLVIP_RESET_ZERO);
    }
    else if(_IsAutoSetting())
    {
        _MDrvSclVipModifyAipAlreadySetting(enAIPtype,E_MDRV_SCLVIP_FILL_GOLBAL_FULL);
    }
    gstSupCfg.staip[enAIPtype].stFCfg.bEn = bEn;
    gstSupCfg.staip[enAIPtype].stFCfg.u8framecnt = u8framecount;
}
void _MDrvSclVipForPrepareCheckSetting(void)
{
    MDrvSclVipConfigType_e enVIPtype;
    MDrvSclVipAipType_e enAIPType;
    for(enVIPtype =E_MDRV_SCLVIP_ACK_CONFIG;enVIPtype<=E_MDRV_SCLVIP_MCNR_CONFIG;(enVIPtype*=2))
    {
        if(_IsSuspendResetFlag(enVIPtype))
        {
            _MDrvSclVipPrepareCheckSetting(enVIPtype);
        }
    }
    for(enAIPType =E_MDRV_SCLVIP_AIP_YEE;enAIPType<E_MDRV_SCLVIP_AIP_NUM;(enAIPType++))
    {
        if(_IsSuspendAipResetFlag(enAIPType))
        {
            _MDrvSclVipPrepareAipCheckSetting(enAIPType);
            SCL_DBG(SCL_DBG_LV_VIP()&EN_DBGMG_VIPLEVEL_VIPSUP, "[MDRVVIP]AIP: %d \n",enAIPType);
        }
    }
}
void _MDrvSclVipSetCmdqAfterPollTimeoutSkip(bool bskip)
{
    DrvSclCmdqSetSkipPollWhenWaitTimeOut(E_DRV_SCLCMDQ_TYPE_IP0,bskip);
}
void _MDrvSclVipWaitForCmdqDone(void)
{
    int u32Time;
    u32Time = DrvSclOsGetSystemTime();
    while(1)
    {
        if(DrvSclCmdqCheckIPAlreadyDone(E_DRV_SCLCMDQ_TYPE_IP0))
        {
            _MDrvSclVipSetCmdqAfterPollTimeoutSkip(0);//close no wait
            break;
        }
        else if(DrvSclOsTimerDiffTimeFromNow(u32Time)>1000)
        {
            sclprintf("[VIP]!!!!Timeout\n");
            break;
        }
    }
}
void _MDrvSclVipForSetEachIp(void)
{
    MDrvSclVipConfigType_e enVIPtype;
    MDrvSclVipAipType_e enAIPType;
    MDrvSclVipSetPqConfig_t stSetPQCfg;
    for(enVIPtype =E_MDRV_SCLVIP_ACK_CONFIG;enVIPtype<=E_MDRV_SCLVIP_MCNR_CONFIG;(enVIPtype*=2))
    {
        if(_IsSuspendResetFlag(enVIPtype))
        {
            stSetPQCfg = MDrvSclVipFillBasicStructSetPqCfg(enVIPtype,NULL);
            stSetPQCfg.pfForSet((void *)stSetPQCfg.pGolbalStructAddr);
            SCL_DBG(SCL_DBG_LV_VIP()&EN_DBGMG_VIPLEVEL_VIPSUP, "[MDRVVIP]%s %d \n", __FUNCTION__,enVIPtype);
        }
    }
    for(enAIPType =E_MDRV_SCLVIP_AIP_YEE;enAIPType<E_MDRV_SCLVIP_AIP_NUM;(enAIPType++))
    {
        if(_IsSuspendAipResetFlag(enAIPType))
        {
            MDrvSclVipSetAipConfig(gstSupCfg.staip[enAIPType]);
            SCL_DBG(SCL_DBG_LV_VIP()&EN_DBGMG_VIPLEVEL_VIPSUP, "[MDRVVIP]AIP: %d \n",enAIPType);
        }
    }
}
void MDrvSclVipCheckEachIPByCmdqIst(void)
{
    MDrvSclVipConfigType_e enVIPtype;
    MDrvSclVipAipType_e enAIPType;
    MDrvSclVipSetPqConfig_t stSetPQCfg;
    u32 u32Time = 0,u32Time2 = 0;
    for(enVIPtype =E_MDRV_SCLVIP_ACK_CONFIG;enVIPtype<=E_MDRV_SCLVIP_MCNR_CONFIG;(enVIPtype*=2))
    {
        if(_IsVipCmdqSetFlag(enVIPtype))
        {
            MDrvSclVipMutexLock();
            u32Time = ((u32)DrvSclOsGetSystemTimeStamp());
            MDrvSclVipSetCheckMode(E_MDRV_SCLVIP_CMDQ_CHECK_PQ);
            MDRv_PQ_Check_Type(0,PQ_CHECK_REG);
            stSetPQCfg = MDrvSclVipFillBasicStructSetPqCfg(enVIPtype,NULL);
            stSetPQCfg.pfForSet((void *)stSetPQCfg.pGolbalStructAddr);
            SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_ISR, "[MDRVVIP]%s %d \n", __FUNCTION__,enVIPtype);
            _ResetVipCmdqSetFlag(enVIPtype);
            MDRv_PQ_Check_Type(0,PQ_CHECK_OFF);
            MDrvSclVipSetCheckMode(E_MDRV_SCLVIP_CMDQ_CHECK_RETURN_ORI);
            u32Time2 = ((u32)DrvSclOsGetSystemTimeStamp());
            MDrvSclVipMutexUnlock();
            SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_ISR, "[DRVSCLIRQ]check diff:%lu(ns)\n",(u32Time2-u32Time));
        }
    }
    for(enAIPType =E_MDRV_SCLVIP_AIP_YEE;enAIPType<E_MDRV_SCLVIP_AIP_NUM;(enAIPType++))
    {
        if(_IsAipCmdqSetFlag(enAIPType))
        {
            MDrvSclVipMutexLock();
            u32Time = ((u32)DrvSclOsGetSystemTimeStamp());
            MDrvSclVipSetCheckMode(E_MDRV_SCLVIP_CMDQ_CHECK_PQ);
            MDRv_PQ_Check_Type(0,PQ_CHECK_REG);
            MDrvSclVipSetAipConfig(gstSupCfg.staip[enAIPType]);
            SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_ISR, "[MDRVVIP]AIP: %d \n",enAIPType);
            _ResetAipCmdqSetFlag(enAIPType);
            MDRv_PQ_Check_Type(0,PQ_CHECK_OFF);
            MDrvSclVipSetCheckMode(E_MDRV_SCLVIP_CMDQ_CHECK_RETURN_ORI);
            u32Time2 = ((u32)DrvSclOsGetSystemTimeStamp());
            MDrvSclVipMutexUnlock();
            SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_ISR, "[DRVSCLIRQ]check diff:%lu(ns)\n",(u32Time2-u32Time));
        }
    }
}

void _MDrvSclVipResumeDumpSRAM(void)
{
    if(_IsSuspendResetFlag(E_MDRV_SCLVIP_NLM_CONFIG))
    {
        if(gstSupCfg.stnlm.stSRAM.bEn)
        {
            MDrvSclVipSetNlmSramConfig(gstSupCfg.stnlm.stSRAM);
        }
        else if(!gstSupCfg.stnlm.stSRAM.bEn && gstSupCfg.stnlm.stSRAM.u32viradr)
        {
            MDrvSclVipSetNlmSramConfig(gstSupCfg.stnlm.stSRAM);
        }

    }
    DrvSclVipSramDump();
}
void _MDrvSclVipPrepareBypassFunctionStruct(bool bBypass,MDrvSclVipSetPqConfig_t stSetBypassCfg)
{
    if(bBypass)
    {
        DrvSclOsMemset(stSetBypassCfg.pPointToCfg, 0,stSetBypassCfg.u32StructSize);
    }
    else
    {
        if(stSetBypassCfg.bSetConfigFlag)
        {
            DrvSclOsMemcpy(stSetBypassCfg.pPointToCfg,stSetBypassCfg.pGolbalStructAddr ,stSetBypassCfg.u32StructSize);
        }
    }
}
bool _MDrvSclVipSetCmdqStatus(bool bFire,bool bEn,u8 u8framecnt)
{
    static u8 stu8ISPCnt = 0;
    static bool bEnCMDQ = 0;
    bool bRet = 0;
    // TODO: CMDQ
    if(_IsNotToCheckPQorCMDQmode() && (VIPSETRULE()== E_DRV_SCLOS_VIPSETRUle_CMDQALL || VIPSETRULE()== E_DRV_SCLOS_VIPSETRUle_CMDQALLCheck
        ||(VIPSETRULE() == E_DRV_SCLOS_VIPSETRUle_CMDQALLONLYSRAMCheck)))
    {
        if(DrvSclVipGetEachDmaEn())
        {
            stu8ISPCnt = DrvSclCmdqGetISPHWCnt();
            MDRv_PQ_Set_CmdqCfg(0,1,stu8ISPCnt,bFire);
            if(!bFire)
            {
                bEnCMDQ = 1;
            }
            else
            {
                bEnCMDQ = 0;
                stu8ISPCnt = 0;
            }
            bRet = 1;
            SCL_DBG(SCL_DBG_LV_VIP()&EN_DBGMG_VIPLEVEL_NORMAL, "[MVIP]ALL CMDQ framecnt:%hhd\n",DrvSclCmdqGetISPHWCnt());
        }
        else if(bFire && bEnCMDQ)
        {
            MDRv_PQ_Set_CmdqCfg(0,bEnCMDQ,stu8ISPCnt,bFire);
            bEnCMDQ = 0;
            stu8ISPCnt = 0;
            bRet = 1;
        }
        else
        {
            MDRv_PQ_Set_CmdqCfg(0,bEn,u8framecnt,bFire);
            SCL_DBG(SCL_DBG_LV_VIP()&EN_DBGMG_VIPLEVEL_VIPSUP, "[MVIP]CMDQ:%hhd,framecnt:%hhd\n"
                ,bEn,u8framecnt);
            if(bEn)
            {
                bRet = 1;
            }
        }
    }
    else if(_IsNotToCheckPQorCMDQmode() && VIPSETRULE())
    {
        if((!MDrvSclVipGetIsBlankingRegion())&&(!bEn))
        {
            stu8ISPCnt = DrvSclCmdqGetISPHWCnt();
            MDRv_PQ_Set_CmdqCfg(0,1,stu8ISPCnt,bFire);
            if(!bFire)
            {
                bEnCMDQ = 1;
            }
            else
            {
                bEnCMDQ = 0;
                stu8ISPCnt = 0;
            }
            bRet = 1;
            SCL_DBG(SCL_DBG_LV_VIP()&EN_DBGMG_VIPLEVEL_NORMAL, "[MVIP]framecnt:%hhd\n",DrvSclCmdqGetISPHWCnt());
        }
        else if(bFire && bEnCMDQ)
        {
            MDRv_PQ_Set_CmdqCfg(0,bEnCMDQ,stu8ISPCnt,bFire);
            bEnCMDQ = 0;
            stu8ISPCnt = 0;
            bRet = 1;
        }
        else
        {
            MDRv_PQ_Set_CmdqCfg(0,bEn,u8framecnt,bFire);
            SCL_DBG(SCL_DBG_LV_VIP()&EN_DBGMG_VIPLEVEL_VIPSUP, "[MVIP]CMDQ:%hhd,framecnt:%hhd\n"
                ,bEn,u8framecnt);
            if(bEn)
            {
                bRet = 1;
            }
        }
    }
    else
    {
        MDRv_PQ_Set_CmdqCfg(0,bEn,u8framecnt,bFire);
    }
    return bRet;
}
void _MDrvSclVipSetPqParameter(MDrvSclVipSetPqConfig_t stSetBypassCfg)
{
    MDrv_PQ_LoadSettingByData(0,stSetBypassCfg.enPQIPType,stSetBypassCfg.pPointToCfg,stSetBypassCfg.u32StructSize);
    SCL_DBG(SCL_DBG_LV_VIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "[MVIP]enPQIPType:%ld,u32StructSize:%ld\n"
        ,stSetBypassCfg.enPQIPType,stSetBypassCfg.u32StructSize);
}
void _MDrvSclVipSetPqByType(u8 u8PQIPIdx, u8 *pData)
{
    MDrvSclVipSetPqConfig_t stSetPQCfg;
    u16 u16Structsize;
    if(u8PQIPIdx>=AIPOffset || u8PQIPIdx==PQ_IP_MCNR_Main)
    {
        u16Structsize = MDrv_PQ_GetIPRegCount(u8PQIPIdx);
    }
    else
    {
        u16Structsize = _MDrvSclVipCioceStructSizeFromPqType(u8PQIPIdx);
    }
    stSetPQCfg = _MDrvSclVipFillPqCfgByType(u8PQIPIdx,pData,u16Structsize);
    SCL_DBG(SCL_DBG_LV_VIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "[MVIP]Struct size:%hd,%hd\n"
        ,u16Structsize,MDrv_PQ_GetIPRegCount(u8PQIPIdx));
    _MDrvSclVipSetPqParameter(stSetPQCfg);
}
void _MDrvSclVipSetMcnr(MDrvSclVipMcnrConfig_t *pstCfg)
{
    DrvSclVipSetMcnrIpmRead(pstCfg->bEnMCNR);
    DrvSclVipSetIpmConpress(pstCfg->bEnMCNR);
    if(DrvSclOsGetEvent(DrvSclIrqGetIrqSYNCEventID())&E_DRV_SCLIRQ_EVENT_BCLOSECIIR)
    {
        DrvSclVipSetCiirRead(0);
        DrvSclVipSetCiirWrite(0);
    }
    else if(_IsCiirBufferAllocateReady())
    {
        DrvSclVipSetCiirRead(pstCfg->bEnCIIR);
        DrvSclVipSetCiirWrite(pstCfg->bEnCIIR);
    }
    else
    {
        SCL_DBGERR( "[MDRVVIP]%s,CIIR buffer not alloc \n", __FUNCTION__);
    }
    _MDrvSclVipSetPqByType(PQ_IP_MCNR_Main,(u8 *)pstCfg->u32Viraddr);
}

void VIP_DazaIST(void *arg)
{
    u32 u32Events;
    MDrvSclVipDazaEvent_e enDAZAEvent;
    u16 u16AIPsheet;

    while(1)
    {
        if(DrvSclOsGetProbeInformation(E_DRV_SCLOS_INIT_VIP))
        {
            break;
        }
        else
        {
            SCL_RTKDBG(0,"[MVIP]%s:sleep \n",__FUNCTION__);

    #if defined (SCLOS_TYPE_LINUX_KERNEL)
            DrvSclOsSleepTaskWork(_MVIP_TASK);
    #else
            DrvSclOsTaskWait((u32)(0x1 << (_MVIP_TASK&0xFFFF)));
    #endif
        }

    }

    while(1)
    {
        SCL_DBG(SCL_DBG_LV_VIP()&EN_DBGMG_VIPLEVEL_NORMAL, "[DRVVIP]%s:going event:%x @:%lu\n"
            ,__FUNCTION__,genVipDazaEvent,((u32)DrvSclOsGetSystemTimeStamp()));
        SCL_RTKDBG(0,"[MVIP]%s:going \n",__FUNCTION__);
        if(genVipDazaEvent)
        {
            if((DrvSclOsGetEvent(DrvSclIrqGetIrqSYNCEventID())&(E_DRV_SCLIRQ_EVENT_ISPFRMEND|E_DRV_SCLIRQ_EVENT_FRMENDSYNC))
                !=(E_DRV_SCLIRQ_EVENT_ISPFRMEND|E_DRV_SCLIRQ_EVENT_FRMENDSYNC))
            {
                DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(), E_DRV_SCLIRQ_EVENT_ISPFRMEND, &u32Events, E_DRV_SCLOS_EVENT_MD_OR, 200); // get status: FRM END
                DrvSclOsDelayTaskUs(100);
            }
            MDrvSclVipMutexLock();
            enDAZAEvent = genVipDazaEvent;
            MDrvSclVipMutexUnlock();
            if(enDAZAEvent &E_MDRV_SCLVIP_DAZA_BMCNR)
            {
                MDrvSclVipMutexLock();
                if(DrvSclIrqGetIsBlankingRegion())
                {
                    SCL_DBG(SCL_DBG_LV_VIP()&EN_DBGMG_VIPLEVEL_NORMAL, "[DRVVIP]E_MDRV_SCLVIP_DAZA_BMCNR\n");
                    MDRv_PQ_Set_CmdqCfg(0,0,0,1);
                    _MDrvSclVipSetMcnr(&gstSupCfg.stmcnr);
                    genVipDazaEvent &= (~E_MDRV_SCLVIP_DAZA_BMCNR);
                }
                MDrvSclVipMutexUnlock();
            }
            // avoid one blanking region can't set all cmd,double switch.
            else if(enDAZAEvent &E_MDRV_SCLVIP_DAZA_BGMA2C)
            {
                u16AIPsheet = _GetAipOffset(E_MDRV_SCLVIP_AIP_GM10TO12);
                MDrvSclVipMutexLock();
                if(DrvSclIrqGetIsBlankingRegion())
                {
                    SCL_DBG(SCL_DBG_LV_VIP()&EN_DBGMG_VIPLEVEL_NORMAL, "[DRVVIP]E_MDRV_SCLVIP_DAZA_BGMA2C\n");
                    MDRv_PQ_Set_CmdqCfg(0,0,0,1);
                    DrvSclCmdqWriteRegWithMaskDirect(REG_SCL0_01_L, BIT3, BIT3);
                    _MDrvSclVipSetPqByType(u16AIPsheet,(u8 *)gstSupCfg.staip[E_MDRV_SCLVIP_AIP_GM10TO12].u32Viraddr);
                    DrvSclCmdqWriteRegWithMaskDirect(REG_SCL0_01_L, 0, BIT3);
                    genVipDazaEvent &= (~E_MDRV_SCLVIP_DAZA_BGMA2C);
                }
                MDrvSclVipMutexUnlock();
            }
            if(enDAZAEvent &E_MDRV_SCLVIP_DAZA_BNLM)
            {
                MDrvSclVipMutexLock();
                if(DrvSclIrqGetIsBlankingRegion())
                {
                    SCL_DBG(SCL_DBG_LV_VIP()&EN_DBGMG_VIPLEVEL_NORMAL, "[DRVVIP]E_MDRV_SCLVIP_DAZA_BNLM\n");
                    MDRv_PQ_Set_CmdqCfg(0,0,0,1);
                    _MDrvSclVipSetPqByType(PQ_IP_NLM_Main,(u8 *)&gstSupCfg.stnlm.stNLM);
                    genVipDazaEvent &= (~E_MDRV_SCLVIP_DAZA_BNLM);
                }
                MDrvSclVipMutexUnlock();
            }
            else if(enDAZAEvent &E_MDRV_SCLVIP_DAZA_BGMC2A)
            {
                u16AIPsheet = _GetAipOffset(E_MDRV_SCLVIP_AIP_GM12TO10);
                MDrvSclVipMutexLock();
                if(DrvSclIrqGetIsBlankingRegion())
                {
                    SCL_DBG(SCL_DBG_LV_VIP()&EN_DBGMG_VIPLEVEL_NORMAL, "[DRVVIP]E_MDRV_SCLVIP_DAZA_BGMC2A\n");
                    MDRv_PQ_Set_CmdqCfg(0,0,0,1);
                    DrvSclCmdqWriteRegWithMaskDirect(REG_SCL0_01_L, BIT3, BIT3);
                    _MDrvSclVipSetPqByType(u16AIPsheet,(u8 *)gstSupCfg.staip[E_MDRV_SCLVIP_AIP_GM12TO10].u32Viraddr);
                    DrvSclCmdqWriteRegWithMaskDirect(REG_SCL0_01_L, 0, BIT3);
                    genVipDazaEvent &= (~E_MDRV_SCLVIP_DAZA_BGMC2A);
                }
                MDrvSclVipMutexUnlock();
            }
            if(enDAZAEvent &E_MDRV_SCLVIP_DAZA_BXNR)
            {
                MDrvSclVipMutexLock();
                if(DrvSclIrqGetIsBlankingRegion())
                {
                    SCL_DBG(SCL_DBG_LV_VIP()&EN_DBGMG_VIPLEVEL_NORMAL, "[DRVVIP]E_MDRV_SCLVIP_DAZA_BXNR\n");
                    u16AIPsheet = _GetAipOffset(E_MDRV_SCLVIP_AIP_XNR);
                    MDRv_PQ_Set_CmdqCfg(0,0,0,1);
                    _MDrvSclVipSetPqByType(u16AIPsheet,(u8 *)gstSupCfg.staip[E_MDRV_SCLVIP_AIP_XNR].u32Viraddr);
                    genVipDazaEvent &= (~E_MDRV_SCLVIP_DAZA_BXNR);
                }
                MDrvSclVipMutexUnlock();
            }
        }
        else
        {

#if defined (SCLOS_TYPE_LINUX_KERNEL)
            DrvSclOsSleepTaskWork(_MVIP_TASK);
#else
            DrvSclOsTaskWait((u32)(0x1 <<(_MVIP_TASK&0xFFFF)));
#endif

        }
    }
}

bool _MDrvSclVipForSetEachPqTypeByIp
    (u8 u8FirstType,u8 u8LastType,MDrvSclVipFcconfig_t stFCfg,u8 ** pPointToData)
{
    u8 u8PQType;
    bool bRet = 0;
    for(u8PQType = u8FirstType;u8PQType<=u8LastType;u8PQType++)
    {
        if(u8PQType == u8FirstType)
        {
            bRet = _MDrvSclVipSetCmdqStatus(0,stFCfg.bEn,stFCfg.u8framecnt);
            MDrvSclCmdqMutexLock(E_DRV_SCLCMDQ_TYPE_IP0,1);
        }
        else if(u8PQType == u8LastType)
        {
            bRet = _MDrvSclVipSetCmdqStatus(1,stFCfg.bEn,stFCfg.u8framecnt);
        }
        _MDrvSclVipSetPqByType(u8PQType,pPointToData[u8PQType-u8FirstType]);
        if(u8PQType == u8LastType)
        {
            MDrvSclCmdqMutexUnlock(E_DRV_SCLCMDQ_TYPE_IP0,1);
        }
    }
    return bRet;
}
bool _MDrvSclVipSetBypassIp(u32 bBypass,MDrvSclVipConfigType_e enVIPtype)
{
    MDrvSclVipSetPqConfig_t stSetBypassCfg;
    stSetBypassCfg.bSetConfigFlag = _IsSuspendResetFlag(enVIPtype);
    switch(enVIPtype)
    {
        case E_MDRV_SCLVIP_MCNR_CONFIG:
                stSetBypassCfg = MDrvSclVipFillBasicStructSetPqCfg(enVIPtype,NULL);
                if(bBypass)
                {
                    _MDrvSclVipModifyMcnrAlreadySetting(stSetBypassCfg,E_MDRV_SCLVIP_RESET_ZERO);
                }
                else
                {
                    stSetBypassCfg.pfForSet((void *)stSetBypassCfg.pGolbalStructAddr);
                }
            break;
        case E_MDRV_SCLVIP_NLM_CONFIG:
                stSetBypassCfg.u32StructSize = sizeof(MDrvSclVipNlmMainConfig_t);
                stSetBypassCfg.pGolbalStructAddr = (u8 *)&gstSupCfg.stnlm.stNLM;
                stSetBypassCfg.enPQIPType = PQ_IP_NLM_Main;
                stSetBypassCfg.pPointToCfg = DrvSclOsMemalloc(stSetBypassCfg.u32StructSize, GFP_KERNEL);
                _MDrvSclVipPrepareBypassFunctionStruct(bBypass,stSetBypassCfg);
                if(stSetBypassCfg.bSetConfigFlag)
                {
                    _MDrvSclVipSetPqParameter(stSetBypassCfg);
                }
                DrvSclOsMemFree(stSetBypassCfg.pPointToCfg);
            break;
        case E_MDRV_SCLVIP_LDC_CONFIG:
                DrvSclVipSetLdcOnConfig(!bBypass);
            break;
        case E_MDRV_SCLVIP_CONFIG:
                MDRv_PQ_Set_CmdqCfg(0,0,0,0);
                stSetBypassCfg.u32StructSize = sizeof(MDrvSclVipByPassConfig_t);
                stSetBypassCfg.enPQIPType = PQ_IP_VIP_Main;
                stSetBypassCfg.pPointToCfg = DrvSclOsMemalloc(stSetBypassCfg.u32StructSize, GFP_KERNEL);
                DrvSclOsMemset(stSetBypassCfg.pPointToCfg, (bBypass) ? 0x1 :0,stSetBypassCfg.u32StructSize);
                _MDrvSclVipSetPqParameter(stSetBypassCfg);
                DrvSclOsMemFree(stSetBypassCfg.pPointToCfg);
            break;
        default:
                stSetBypassCfg.u32StructSize = 0;
                stSetBypassCfg.pGolbalStructAddr = NULL;
                stSetBypassCfg.bSetConfigFlag = 0;
                stSetBypassCfg.pPointToCfg = NULL;
                stSetBypassCfg.enPQIPType = PQ_IP_SC_End_Main;
            return 0;

    }
    return 1;
}
u32 _MDrvSclVipFillSettingBuffer(u16 u16PQIPIdx, u32 u32pstViraddr, u32 *u32gpViraddr)
{
    if(_IsNotToCheckPQorCMDQmode())
    {
        u16 u16StructSize;
        u32 u32ViraddrOri;
        u32 u32Viraddr;
        u16StructSize = MDrv_PQ_GetIPRegCount(u16PQIPIdx);
        u32ViraddrOri = u32pstViraddr;
        if(*u32gpViraddr)
        {
            u32Viraddr = *u32gpViraddr;
        }
        else
        {
            *u32gpViraddr = (u32)DrvSclOsVirMemalloc(u16StructSize);
            DrvSclOsMemset((void *)*u32gpViraddr,0,u16StructSize);
            u32Viraddr = *u32gpViraddr;
        }
        if(u32Viraddr != u32ViraddrOri)
        {
            if(DrvSclOsCopyFromUser((void *)u32Viraddr, (void *)u32ViraddrOri, u16StructSize))
            {
                if((u32ViraddrOri&MIU0Vir_BASE)==MIU0Vir_BASE)
                {
                    DrvSclOsMemcpy((void *)u32Viraddr, (void *)u32ViraddrOri, u16StructSize);
                }
                else
                {
                    SCL_ERR( "[VIP] copy buffer error Ori:%lx ,%lx\n",u32ViraddrOri,u32Viraddr);
                    return 0;
                }
            }
        }
        return u32Viraddr;
    }
    else
    {
        return u32pstViraddr;
    }
}
//-------------------------------------------------------------------------------------------------
//  Function
//-------------------------------------------------------------------------------------------------
MDrvSclVipSetPqConfig_t MDrvSclVipFillBasicStructSetPqCfg(MDrvSclVipConfigType_e enVIPtype,void *pPointToCfg)
{
    MDrvSclVipSetPqConfig_t stSetPQCfg;
    stSetPQCfg.bSetConfigFlag = _IsSuspendResetFlag(enVIPtype);
    stSetPQCfg.pPointToCfg = pPointToCfg;
    switch(enVIPtype)
    {
        case E_MDRV_SCLVIP_MCNR_CONFIG:
                stSetPQCfg.u32StructSize = sizeof(MDrvSclVipMcnrConfig_t);
                stSetPQCfg.pGolbalStructAddr = (u8 *)&gstSupCfg.stmcnr;
                stSetPQCfg.pfForSet = MDrvSclVipSetMcnrConfig;
            break;

        case E_MDRV_SCLVIP_ACK_CONFIG:
                stSetPQCfg.u32StructSize = sizeof(MDrvSclVipAckConfig_t);
                stSetPQCfg.pGolbalStructAddr = (u8 *)&gstSupCfg.stack;
                stSetPQCfg.pfForSet = MDrvSclVipSetAckConfig;
            break;

        case E_MDRV_SCLVIP_IBC_CONFIG:
                stSetPQCfg.u32StructSize = sizeof(MDrvSclVipIbcConfig_t);
                stSetPQCfg.pGolbalStructAddr = (u8 *)&gstSupCfg.stibc;
                stSetPQCfg.pfForSet = MDrvSclVipSetIbcConfig;
            break;

        case E_MDRV_SCLVIP_IHCICC_CONFIG:
                stSetPQCfg.u32StructSize = sizeof(MDrvSclVipIhcIccConfig_t);
                stSetPQCfg.pGolbalStructAddr = (u8 *)&gstSupCfg.stihcicc;
                stSetPQCfg.pfForSet = MDrvSclVipSetIhcICCADPYConfig;
            break;

        case E_MDRV_SCLVIP_ICC_CONFIG:
                stSetPQCfg.u32StructSize = sizeof(MDrvSclVipIccConfig_t);
                stSetPQCfg.pGolbalStructAddr = (u8 *)&gstSupCfg.sticc;
                stSetPQCfg.pfForSet = MDrvSclVipSetICEConfig;
            break;

        case E_MDRV_SCLVIP_IHC_CONFIG:
                stSetPQCfg.u32StructSize = sizeof(MDrvSclVipIhcConfig_t);
                stSetPQCfg.pGolbalStructAddr = (u8 *)&gstSupCfg.stihc;
                stSetPQCfg.pfForSet = MDrvSclVipSetIhcConfig;
            break;

        case E_MDRV_SCLVIP_FCC_CONFIG:
                stSetPQCfg.u32StructSize = sizeof(MDrvSclVipFccConfig_t);
                stSetPQCfg.pGolbalStructAddr = (u8 *)&gstSupCfg.stfcc;
                stSetPQCfg.pfForSet = MDrvSclVipSetFccConfig;
            break;
        case E_MDRV_SCLVIP_UVC_CONFIG:
                stSetPQCfg.u32StructSize = sizeof(MDrvSclVipUvcConfig_t);
                stSetPQCfg.pGolbalStructAddr = (u8 *)&gstSupCfg.stuvc;
                stSetPQCfg.pfForSet = MDrvSclVipSetUvcConfig;
            break;

        case E_MDRV_SCLVIP_DLC_CONFIG:
                stSetPQCfg.u32StructSize = sizeof(MDrvSclVipDlcConfig_t);
                stSetPQCfg.pGolbalStructAddr = (u8 *)&gstSupCfg.stdlc;
                stSetPQCfg.pfForSet = MDrvSclVipSetDlcConfig;
            break;

        case E_MDRV_SCLVIP_DLC_HISTOGRAM_CONFIG:
                stSetPQCfg.u32StructSize = sizeof(MDrvSclVipDlcHistogramConfig_t);
                stSetPQCfg.pGolbalStructAddr = (u8 *)&gstSupCfg.sthist;
                stSetPQCfg.pfForSet = MDrvSclVipSetHistogramConfig;
            break;

        case E_MDRV_SCLVIP_LCE_CONFIG:
                stSetPQCfg.u32StructSize = sizeof(MDrvSclVipLceConfig_t);
                stSetPQCfg.pGolbalStructAddr = (u8 *)&gstSupCfg.stlce;
                stSetPQCfg.pfForSet = MDrvSclVipSetLceConfig;
            break;

        case E_MDRV_SCLVIP_PEAKING_CONFIG:
                stSetPQCfg.u32StructSize = sizeof(MDrvSclVipPeakingConfig_t);
                stSetPQCfg.pGolbalStructAddr = (u8 *)&gstSupCfg.stpk;
                stSetPQCfg.pfForSet = MDrvSclVipSetPeakingConfig;
            break;

        case E_MDRV_SCLVIP_LDC_MD_CONFIG:
                stSetPQCfg.u32StructSize = sizeof(MDrvSclVipLdcMdConfig_t);
                stSetPQCfg.pGolbalStructAddr = (u8 *)&gstSupCfg.stldcmd;
                stSetPQCfg.pfForSet = MDrvSclVipSetLdcmdConfig;
            break;

        case E_MDRV_SCLVIP_LDC_DMAP_CONFIG:
                stSetPQCfg.u32StructSize = sizeof(MDrvSclVipLdcDmaPConfig_t);
                stSetPQCfg.pGolbalStructAddr = (u8 *)&gstSupCfg.stldcdmap;
                stSetPQCfg.pfForSet = MDrvSclVipSetLdcDmapConfig;
            break;

        case E_MDRV_SCLVIP_LDC_SRAM_CONFIG:
                stSetPQCfg.u32StructSize = sizeof(MDrvSclVipLdcSramConfig_t);
                stSetPQCfg.pGolbalStructAddr = (u8 *)&gstSupCfg.stldcsram;
                stSetPQCfg.pfForSet = MDrvSclVipSetLdcSramConfig;
            break;

        case E_MDRV_SCLVIP_NLM_CONFIG:
                stSetPQCfg.u32StructSize = sizeof(MDrvSclVipNlmConfig_t);
                stSetPQCfg.pGolbalStructAddr = (u8 *)&gstSupCfg.stnlm;
                stSetPQCfg.pfForSet = MDrvSclVipSetNlmConfig;
            break;
        case E_MDRV_SCLVIP_LDC_CONFIG:
                stSetPQCfg.u32StructSize = sizeof(MDrvSclVipLdcConfig_t);
                stSetPQCfg.pGolbalStructAddr = (u8 *)&gstSupCfg.stldc;
                stSetPQCfg.pfForSet = MDrvSclVipSetLdcConfig;
            break;
        case E_MDRV_SCLVIP_CONFIG:
                stSetPQCfg.u32StructSize = sizeof(MDrvSclVipConfig_t);
                stSetPQCfg.pGolbalStructAddr = (u8 *)&gstSupCfg.stvip;
                stSetPQCfg.pfForSet = MDrvSclVipSetVipOtherConfig;
            break;
        default:
                stSetPQCfg.u32StructSize = 0;
                stSetPQCfg.pGolbalStructAddr = NULL;
                stSetPQCfg.bSetConfigFlag = 0;
                stSetPQCfg.pfForSet = NULL;
            break;

    }
    return stSetPQCfg;
}
void _MDrvSclVipInitSupCfg(void)
{
    DrvSclOsMemset(&gstSupCfg, 0, sizeof(MDrvSclVipSuspendConfig_t));
}
void MDrvSclVipFreeMemory(void)
{
    MDrvSclVipAipType_e enVIPtype;
    if(gstSupCfg.stmcnr.u32Viraddr)
    {
        DrvSclOsVirMemFree((void *)gstSupCfg.stmcnr.u32Viraddr);
        gstSupCfg.stmcnr.u32Viraddr = 0;
    }
    for(enVIPtype = 0;enVIPtype<E_MDRV_SCLVIP_AIP_NUM;enVIPtype++)
    {
        if(gstSupCfg.staip[enVIPtype].u32Viraddr)
        {
            DrvSclOsVirMemFree((void *)gstSupCfg.staip[enVIPtype].u32Viraddr);
            gstSupCfg.staip[enVIPtype].u32Viraddr = 0;
        }
    }
}

void MDrvSclVipResume(MDrvSclVipInitConfig_t *pCfg)
{
    DrvSclCmdqOpen_t stCMDQIniCfg;
    DrvSclOsMemset(&stCMDQIniCfg,0,sizeof(DrvSclCmdqOpen_t));
    stCMDQIniCfg.u32RiuBase     = pCfg->u32RiuBase;
    stCMDQIniCfg.u32CMDQ_Phy[0]    = pCfg->CMDQCfg.u32CMDQ_Phy;
    stCMDQIniCfg.u32CMDQ_Size[0]   = pCfg->CMDQCfg.u32CMDQ_Size;
    stCMDQIniCfg.u32CMDQ_Vir[0]    = pCfg->CMDQCfg.u32CMDQ_Vir;
    DrvSclVipHWInit();
    MDrvSclVipVtrackInit();
    DrvSclCmdqInit(stCMDQIniCfg);
    MDrvSclVipMutexLock();
    gu32OpenBypass = 0;
    gu32AIPOpenBypass = 0;
    gbVIPCMDQSet = 0;
    gbAIPCMDQSet = 0;
    MDrvSclVipMutexUnlock();
    DrvSclVipSramDump();
}

bool MDrvSclVipInit(MDrvSclVipInitConfig_t *pCfg)
{
    DrvSclVipInitConfig_t stIniCfg;
    DrvSclCmdqOpen_t stCMDQIniCfg;
    MS_PQ_Init_Info    stPQInitInfo;
    bool      ret = FALSE;
    char word[] = {"_MVIP_Mutex"};
    _MVIP_Mutex = DrvSclOsCreateMutex(E_DRV_SCLOS_FIFO, word, SCLOS_PROCESS_SHARED);
    if (_MVIP_Mutex == -1)
    {
        SCL_ERR("[MDRVVIP]%s create mutex fail\n", __FUNCTION__);
        return FALSE;
    }
    _MVIP_TASK  = DrvSclOsCreateTask((TaskEntry)VIP_DazaIST,(u32)NULL,TRUE,(char*)"VIPDazaTask");
    if (_MVIP_TASK == -1)
    {
        SCL_ERR("[MDRVVIP]%s create task fail\n", __FUNCTION__);
        return FALSE;
    }
    stIniCfg.u32RiuBase         = pCfg->u32RiuBase;
    DrvSclOsMemset(&stCMDQIniCfg,0,sizeof(DrvSclCmdqOpen_t));
    stCMDQIniCfg.u32RiuBase     = pCfg->u32RiuBase;
    stCMDQIniCfg.u32CMDQ_Phy[0]    = pCfg->CMDQCfg.u32CMDQ_Phy;
    stCMDQIniCfg.u32CMDQ_Size[0]   = pCfg->CMDQCfg.u32CMDQ_Size;
    stCMDQIniCfg.u32CMDQ_Vir[0]    = pCfg->CMDQCfg.u32CMDQ_Vir;
    DrvSclCmdqInit(stCMDQIniCfg);
    if(DrvSclVipInit(&stIniCfg) == 0)
    {
        SCL_ERR( "[MDRVVIP]%s, Fail\n", __FUNCTION__);
        return FALSE;
    }
    DrvSclVipHWInit();
    MDrvSclVipVtrackInit();
    MDrv_PQ_init_RIU(pCfg->u32RiuBase);
    DrvSclOsMemset(&stPQInitInfo, 0, sizeof(MS_PQ_Init_Info));
    MDrvSclVipMutexLock();
    gu32OpenBypass = 0;
    gu32AIPOpenBypass = 0;
    gbVIPCMDQSet = 0;
    gbAIPCMDQSet = 0;
    MDrvSclVipMutexUnlock();
    // Init PQ
    stPQInitInfo.u16PnlWidth    = 1920;
    stPQInitInfo.u8PQBinCnt     = 0;
    stPQInitInfo.u8PQTextBinCnt = 0;
    if(MDrv_PQ_Init(&stPQInitInfo))
    {
        MDrv_PQ_DesideSrcType(PQ_MAIN_WINDOW, PQ_INPUT_SOURCE_ISP);
        MDrv_PQ_LoadSettings(PQ_MAIN_WINDOW);
        ret = TRUE;
    }
    else
    {
        ret = FALSE;
    }
    _MDrvSclVipInitSupCfg();
    return ret;
}
void MDrvSclVipDelete(bool bCloseISR)
{
    DrvSclOsDeleteTask(_MVIP_TASK);
    _MVIP_TASK = -1;
    if (_MVIP_Mutex != -1)
    {
        DrvSclOsDeleteMutex(_MVIP_Mutex);
        _MVIP_Mutex = -1;
    }
    if(bCloseISR)
    {
        DrvSclIrqExit();
    }
    DrvSclVipExit();
    DrvSclCmdqDelete(E_DRV_SCLCMDQ_TYPE_IP0);
}
void MDrvSclVipSupend(void)
{
    DrvSclCmdqDelete(E_DRV_SCLCMDQ_TYPE_IP0);
}
void MDrvSclVipRelease(void)
{
    u32 u32Events;
    DrvSclCmdqrelease(E_DRV_SCLCMDQ_TYPE_IP0);
    MDrvSclVipMutexLock();
    gbVIPCMDQSet = 0;
    gbAIPCMDQSet = 0;
    MDrvSclVipMutexUnlock();
    if(!MDrvSclVipGetIsBlankingRegion())
    {
        DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(), E_DRV_SCLIRQ_EVENT_FRMENDSYNC, &u32Events, E_DRV_SCLOS_EVENT_MD_OR, 200); // get status: FRM END
    }
    DrvSclVipSetIpmConpress(0);
    MDrv_PQ_LoadSettings(PQ_MAIN_WINDOW);
}
bool MDrvSclVipGetCmdqHwDone(void)
{
    return DrvSclVipGetCmdqHwDone();
}

void * MDrvSclVipGetWaitQueueHead(void)
{
    return DrvSclVipGetWaitQueueHead();
}

bool MDrvSclVipSetPollWait(DrvSclOsPollWaitConfig_t stPollWait)
{
    return DrvSclOsSetPollWait(stPollWait);
}

void MDrvSclVipSuspendResetFlagInit(void)
{
    gstSupCfg.bresetflag = 0;
}
void MDrvSclVipSetAllVipOneShot(MDrvSclVipSuspendConfig_t *stvipCfg)
{
    MDrvSclVipConfigType_e enVIPtype;
    MDrvSclVipAipType_e enAIPtype;
    MDrvSclVipSetPqConfig_t stSetPQCfg;
    u32 u32StructSize = sizeof(u32);
    gstSupCfg.bresetflag = stvipCfg->bresetflag;
    gstSupCfg.bresetflag &= 0x2FFFF;
    gstSupCfg.bAIPreflag = stvipCfg->bAIPreflag;
    for(enVIPtype =E_MDRV_SCLVIP_ACK_CONFIG;enVIPtype<=E_MDRV_SCLVIP_MCNR_CONFIG;(enVIPtype*=2))
    {
        stSetPQCfg = MDrvSclVipFillBasicStructSetPqCfg(enVIPtype,(void *)((u32)stvipCfg+u32StructSize));
        if(_IsSuspendResetFlag(enVIPtype))
        {
            SCL_DBG(SCL_DBG_LV_VIP()&EN_DBGMG_VIPLEVEL_VIPSUP,
                "enVIPtype :%d pGolbalStructAddr: %lx,pPointToCfg: %lx,size:%lx\n",enVIPtype,
                (u32)stSetPQCfg.pGolbalStructAddr,(u32)stSetPQCfg.pPointToCfg,stSetPQCfg.u32StructSize);
            DrvSclOsMemcpy(stSetPQCfg.pGolbalStructAddr,stSetPQCfg.pPointToCfg,stSetPQCfg.u32StructSize);
        }
        u32StructSize += stSetPQCfg.u32StructSize;
    }
    for(enAIPtype =E_MDRV_SCLVIP_AIP_YEE;enAIPtype<E_MDRV_SCLVIP_AIP_NUM;(enAIPtype++))
    {
        if(_IsSuspendAipResetFlag(enAIPtype))
        {
            _MDrvSclVipFillSettingBuffer
                (_GetAipOffset(enAIPtype),stvipCfg->staip[enAIPtype].u32Viraddr,&gstSupCfg.staip[enAIPtype].u32Viraddr);
            gstSupCfg.staip[enAIPtype].stFCfg.bEn = stvipCfg->staip[enAIPtype].stFCfg.bEn;
            gstSupCfg.staip[enAIPtype].stFCfg.u8framecnt = stvipCfg->staip[enAIPtype].stFCfg.u8framecnt;
            gstSupCfg.staip[enAIPtype].u16AIPType = stvipCfg->staip[enAIPtype].u16AIPType;
        }
    }
    _MDrvSclVipForSetEachIp();
}
void MDrvSclVipResetEachIP(void)
{
    _MDrvSclVipForSetEachIp();
    _MDrvSclVipResumeDumpSRAM();
    if(gbVIPCheckCMDQorPQ >= E_MDRV_SCLVIP_CMDQ_CHECK_ALREADY_SETINNG)
    {
        DrvSclCmdqBeTrigger(E_DRV_SCLCMDQ_TYPE_IP0,1);
    }
}

bool MDrvSclVipSysInit(MDrvSclVipInitConfig_t *pCfg)
{
    SCL_DBG(SCL_DBG_LV_VIP(), "[MDRVVIP]%s %d\n", __FUNCTION__,__LINE__);
    DrvSclCmdqEnable(1,0);
    DrvSclVipOpen();
    return TRUE;
}
void MDrvSclVipSetMcnrConpressForDebug(bool bEn)
{
    if(gstSupCfg.stmcnr.bEnMCNR)
    {
        DrvSclVipSetIpmConpress(bEn);
    }
}
void _MDrvSclVipCopyAipConfigToGlobal(MDrvSclVipAipType_e enVIPtype, void *pCfg, u32 u32StructSize)
{
    if(_IsNotToCheckPQorCMDQmode())
    {
        DrvSclOsMemcpy(&gstSupCfg.staip[enVIPtype], pCfg,u32StructSize);
        gstSupCfg.staip[enVIPtype].stFCfg.bEn = 0 ;
        gstSupCfg.staip[enVIPtype].stFCfg.u8framecnt = 0;
        gstSupCfg.staip[enVIPtype].u16AIPType= enVIPtype;
        _SetSuspendAipResetFlag(enVIPtype);
    }
}
bool MDrvSclVipSetMcnrConfig(void *pCfg)
{
    MDrvSclVipMcnrConfig_t *pstCfg = pCfg;
    static bool bbypass = 0;
    if(_IsFrameBufferAllocatedReady())
    {
        if(_IsNotToCheckPQorCMDQmode())
        {
            MDrvSclVipMutexLock();
        }

        if(_IsNotOpenVIPBypass(E_MDRV_SCLVIP_MCNR_CONFIG))
        {
            bbypass = 0;
            pstCfg->u32Viraddr =
            _MDrvSclVipFillSettingBuffer(PQ_IP_MCNR_Main,pstCfg->u32Viraddr,&gstSupCfg.stmcnr.u32Viraddr);
            MDrvSclCmdqMutexLock(E_DRV_SCLCMDQ_TYPE_IP0,pstCfg->stFCfg.bEn);
            MDRv_PQ_Set_CmdqCfg(0,pstCfg->stFCfg.bEn,pstCfg->stFCfg.u8framecnt,1);
            _MDrvSclVipSetMcnr(pstCfg);
            MDrvSclCmdqMutexUnlock(E_DRV_SCLCMDQ_TYPE_IP0,pstCfg->stFCfg.bEn);
            _MDrvSclVipCopyIpConfigToGlobal(E_MDRV_SCLVIP_MCNR_CONFIG,(void *)pstCfg);
        }
        else if(bbypass==0)
        {
            MDrvSclCmdqMutexLock(E_DRV_SCLCMDQ_TYPE_IP0,pstCfg->stFCfg.bEn);
            MDRv_PQ_Set_CmdqCfg(0,pstCfg->stFCfg.bEn,pstCfg->stFCfg.u8framecnt,1);
            _MDrvSclVipSetMcnr(pstCfg);
            MDrvSclCmdqMutexUnlock(E_DRV_SCLCMDQ_TYPE_IP0,pstCfg->stFCfg.bEn);
            bbypass = 1;
        }
        else
        {
            _MDrvSclVipCopyIpConfigToGlobal(E_MDRV_SCLVIP_MCNR_CONFIG,(void *)pstCfg);
        }
        if(_IsNotToCheckPQorCMDQmode())
        {
            MDrvSclVipMutexUnlock();
        }
        SCL_DBG(SCL_DBG_LV_VIP(),
            "[MDRVVIP]%s,addr:%lx flag:%lx \n", __FUNCTION__,(u32)&(gstSupCfg.stmcnr),gstSupCfg.bresetflag);
        return TRUE;
    }
    else
    {
        SCL_ERR( "[MDRVVIP]%s,MCNR buffer not alloc \n", __FUNCTION__);
        return FALSE;
    }
    return 1;
}


bool MDrvSclVipSetPeakingConfig(void *pvCfg)
{
    MDrvSclVipPeakingConfig_t *pCfg = pvCfg;
    u8 *p8PQType[(PQ_IP_VIP_Peaking_gain_ad_Y_Main-PQ_IP_VIP_HLPF_Main+1)];
    p8PQType[0]     = (u8*)&(pCfg->stHLPF);
    p8PQType[1]     = (u8*)&(pCfg->stHLPFDith);
    p8PQType[2]     = (u8*)&(pCfg->stVLPFcoef1);
    p8PQType[3]     = (u8*)&(pCfg->stVLPFcoef2);
    p8PQType[4]     = (u8*)&(pCfg->stVLPFDith);
    p8PQType[5]     = (u8*)&(pCfg->stOnOff);
    p8PQType[6]     = (u8*)&(pCfg->stBand);
    p8PQType[7]     = (u8*)&(pCfg->stAdp);
    p8PQType[8]     = (u8*)&(pCfg->stPcor);
    p8PQType[9]     = (u8*)&(pCfg->stAdpY);
    p8PQType[10]    = (u8*)&(pCfg->stGain);
    p8PQType[11]    = (u8*)&(pCfg->stGainAdpY);
    if(_IsNotToCheckPQorCMDQmode())
    {
        MDrvSclVipMutexLock();
    }
    if(_MDrvSclVipForSetEachPqTypeByIp(PQ_IP_VIP_HLPF_Main,PQ_IP_VIP_Peaking_gain_ad_Y_Main,pCfg->stFCfg,p8PQType))
    {
        _SetVipCmdqSetFlag(E_MDRV_SCLVIP_PEAKING_CONFIG);
    }

    _MDrvSclVipCopyIpConfigToGlobal(E_MDRV_SCLVIP_PEAKING_CONFIG,(void *)pCfg);
    if(_IsNotToCheckPQorCMDQmode())
    {
        MDrvSclVipMutexUnlock();
    }
    SCL_DBG(SCL_DBG_LV_VIP(),
        "[MDRVVIP]%s,addr:%lx flag:%lx \n", __FUNCTION__,(u32)&gstSupCfg.stpk,gstSupCfg.bresetflag);
    return TRUE;
}

bool MDrvSclVipSetHistogramConfig(void *pvCfg)
{
    ST_VIP_DLC_HISTOGRAM_CONFIG stDLCCfg;
    u16 i;
    MDrvSclVipDlcHistogramConfig_t *pCfg = pvCfg;
    stDLCCfg.bVariable_Section  = pCfg->bVariable_Section;
    stDLCCfg.bstat_MIU          = pCfg->bstat_MIU;
    stDLCCfg.bcurve_fit_en      = pCfg->bcurve_fit_en;
    stDLCCfg.bcurve_fit_rgb_en  = pCfg->bcurve_fit_rgb_en;
    stDLCCfg.bDLCdither_en      = pCfg->bDLCdither_en;
    stDLCCfg.bhis_y_rgb_mode_en = pCfg->bhis_y_rgb_mode_en;
    stDLCCfg.bstatic            = pCfg->bstatic;
    stDLCCfg.bRange             = pCfg->bRange;
    stDLCCfg.u16Vst             = pCfg->u16Vst;
    stDLCCfg.u16Hst             = pCfg->u16Hst;
    stDLCCfg.u16Vnd             = pCfg->u16Vnd;
    stDLCCfg.u16Hnd             = pCfg->u16Hnd;
    stDLCCfg.u8HistSft          = pCfg->u8HistSft;
    stDLCCfg.u8trig_ref_mode    = pCfg->u8trig_ref_mode;
    stDLCCfg.u32StatBase[0]     = _Phys2Miu(pCfg->u32StatBase[0]);
    stDLCCfg.u32StatBase[1]     = _Phys2Miu(pCfg->u32StatBase[1]);
    stDLCCfg.stFCfg.bEn         = pCfg->stFCfg.bEn;
    stDLCCfg.stFCfg.u8framecnt  = pCfg->stFCfg.u8framecnt;

    for(i=0;i<VIP_DLC_HISTOGRAM_SECTION_NUM;i++)
    {
        stDLCCfg.u8Histogram_Range[i] = pCfg->u8Histogram_Range[i];
    }
    if(_IsNotToCheckPQorCMDQmode())
    {
        MDrvSclVipMutexLock();
    }
    MDrvSclCmdqMutexLock(E_DRV_SCLCMDQ_TYPE_IP0,pCfg->stFCfg.bEn);

    if(DrvSclVipSetDlcHistogramConfig(stDLCCfg) == 0)
    {
        SCL_DBG(SCL_DBG_LV_VIP(), "[MDRVVIP]%s, Fail\n", __FUNCTION__);
        MDrvSclCmdqMutexUnlock(E_DRV_SCLCMDQ_TYPE_IP0,pCfg->stFCfg.bEn);
        MDrvSclVipMutexUnlock();
        return FALSE;
    }
    MDrvSclCmdqMutexUnlock(E_DRV_SCLCMDQ_TYPE_IP0,pCfg->stFCfg.bEn);
    _MDrvSclVipCopyIpConfigToGlobal(E_MDRV_SCLVIP_DLC_HISTOGRAM_CONFIG,(void *)pCfg);
    if(_IsNotToCheckPQorCMDQmode())
    {
        MDrvSclVipMutexUnlock();
    }
    SCL_DBG(SCL_DBG_LV_VIP(),
        "[MDRVVIP]%s,addr:%lx flag:%lx \n", __FUNCTION__,(u32)&gstSupCfg.sthist,gstSupCfg.bresetflag);
    return TRUE;
}

bool MDrvSclVipGetDlcHistogramReport(void *pvCfg)
{
    DrvSclVipDlcHistogramReport_t stDLCCfg;
    u16 i;
    MDrvSclVipDlcHistogramReport_t *pCfg = pvCfg;
    stDLCCfg             = DrvSclVipGetDlcHistogramConfig();
    pCfg->u32PixelWeight = stDLCCfg.u32PixelWeight;
    pCfg->u32PixelCount  = stDLCCfg.u32PixelCount;
    pCfg->u8MaxPixel     = stDLCCfg.u8MaxPixel;
    pCfg->u8MinPixel     = stDLCCfg.u8MinPixel;
    pCfg->u8Baseidx      = stDLCCfg.u8Baseidx;
    for(i=0;i<VIP_DLC_HISTOGRAM_REPORT_NUM;i++)
    {
        stDLCCfg.u32Histogram[i]    = DrvSclVipGetDlcHistogramReport(i);
        pCfg->u32Histogram[i]       = stDLCCfg.u32Histogram[i];
    }

    return TRUE;
}

bool MDrvSclVipSetDlcConfig(void *pvCfg)
{
    MDrvSclVipDlcConfig_t *pCfg = pvCfg;
    u8 *p8PQType[(PQ_IP_VIP_YC_gain_offset_Main-PQ_IP_VIP_DLC_His_range_Main+1)];//add PQ_IP_VIP_YC_gain_offset_Main
    p8PQType[0] = (u8*)&(pCfg->sthist);
    p8PQType[1] = (u8*)&(pCfg->stEn);
    p8PQType[2] = (u8*)&(pCfg->stDither);
    p8PQType[3] = (u8*)&(pCfg->stHistH);
    p8PQType[4] = (u8*)&(pCfg->stHistV);
    p8PQType[5] = (u8*)&(pCfg->stPC);
    p8PQType[6] = (u8*)&(pCfg->stGainOffset);
    if(_IsNotToCheckPQorCMDQmode())
    {
        MDrvSclVipMutexLock();
    }

    if(_MDrvSclVipForSetEachPqTypeByIp(PQ_IP_VIP_DLC_His_range_Main,PQ_IP_VIP_YC_gain_offset_Main,pCfg->stFCfg,p8PQType))
    {
        _SetVipCmdqSetFlag(E_MDRV_SCLVIP_DLC_CONFIG);
    }

    _MDrvSclVipCopyIpConfigToGlobal(E_MDRV_SCLVIP_DLC_CONFIG,(void *)pCfg);
    if(_IsNotToCheckPQorCMDQmode())
    {
        MDrvSclVipMutexUnlock();
    }
    SCL_DBG(SCL_DBG_LV_VIP(),
        "[MDRVVIP]%s,addr:%lx flag:%lx \n", __FUNCTION__,(u32)&gstSupCfg.stdlc,gstSupCfg.bresetflag);
    return TRUE;
}

bool MDrvSclVipSetLceConfig(void *pvCfg)
{
    MDrvSclVipLceConfig_t *pCfg = pvCfg;
    u8 *p8PQType[(PQ_IP_VIP_LCE_curve_Main-PQ_IP_VIP_LCE_Main+1)];
    p8PQType[0] = (u8*)&(pCfg->stOnOff);
    p8PQType[1] = (u8*)&(pCfg->stDITHER);
    p8PQType[2] = (u8*)&(pCfg->stSet);
    p8PQType[3] = (u8*)&(pCfg->stCurve);
    if(_IsNotToCheckPQorCMDQmode())
    {
        MDrvSclVipMutexLock();
    }
    if(_MDrvSclVipForSetEachPqTypeByIp(PQ_IP_VIP_LCE_Main,PQ_IP_VIP_LCE_curve_Main,pCfg->stFCfg,p8PQType))
    {
        _SetVipCmdqSetFlag(E_MDRV_SCLVIP_LCE_CONFIG);
    }

    _MDrvSclVipCopyIpConfigToGlobal(E_MDRV_SCLVIP_LCE_CONFIG,(void *)pCfg);
    if(_IsNotToCheckPQorCMDQmode())
    {
        MDrvSclVipMutexUnlock();
    }
    SCL_DBG(SCL_DBG_LV_VIP(),
        "[MDRVVIP]%s,addr:%lx flag:%lx \n", __FUNCTION__,(u32)&gstSupCfg.stlce,gstSupCfg.bresetflag);
    return TRUE;
}

bool MDrvSclVipSetUvcConfig(void *pvCfg)
{
    u8 *pUVC = NULL;
    u8 bRet = 0;
    MDrvSclVipUvcConfig_t *pCfg = pvCfg;
    pUVC = (u8*)&(pCfg->stUVC);
    if(_IsNotToCheckPQorCMDQmode())
    {
        MDrvSclVipMutexLock();
    }
    if(_MDrvSclVipSetCmdqStatus(1,pCfg->stFCfg.bEn,pCfg->stFCfg.u8framecnt))
    {
        MDrvSclCmdqMutexLock(E_DRV_SCLCMDQ_TYPE_IP0,1);
        _SetVipCmdqSetFlag(E_MDRV_SCLVIP_UVC_CONFIG);
        bRet = 1;
    }
    _MDrvSclVipSetPqByType(PQ_IP_VIP_UVC_Main,pUVC);
    MDrvSclCmdqMutexUnlock(E_DRV_SCLCMDQ_TYPE_IP0,bRet);
    _MDrvSclVipCopyIpConfigToGlobal(E_MDRV_SCLVIP_UVC_CONFIG,(void *)pCfg);
    if(_IsNotToCheckPQorCMDQmode())
    {
        MDrvSclVipMutexUnlock();
    }
    SCL_DBG(SCL_DBG_LV_VIP(),
        "[MDRVVIP]%s,addr:%lx flag:%lx \n", __FUNCTION__,(u32)&gstSupCfg.stuvc,gstSupCfg.bresetflag);
    return TRUE;
}


bool MDrvSclVipSetIhcConfig(void *pvCfg)
{
    MDrvSclVipIhcConfig_t *pCfg = pvCfg;
    u8 *p8PQType[(PQ_IP_VIP_IHC_dither_Main-PQ_IP_VIP_IHC_Main+2)];
    p8PQType[0] = (u8*)&(pCfg->stOnOff);
    p8PQType[1] = (u8*)&(pCfg->stYmd);
    p8PQType[2] = (u8*)&(pCfg->stDither);
    p8PQType[3] = (u8*)&(pCfg->stset);
    if(_IsNotToCheckPQorCMDQmode())
    {
        MDrvSclVipMutexLock();
    }
    _MDrvSclVipSetCmdqStatus(0,pCfg->stFCfg.bEn,pCfg->stFCfg.u8framecnt);
    MDrvSclCmdqMutexLock(E_DRV_SCLCMDQ_TYPE_IP0,1);
    _MDrvSclVipSetPqByType(PQ_IP_VIP_IHC_SETTING_Main,p8PQType[3]);
    MDrvSclCmdqMutexUnlock(E_DRV_SCLCMDQ_TYPE_IP0,1);
    if(_MDrvSclVipForSetEachPqTypeByIp(PQ_IP_VIP_IHC_Main,PQ_IP_VIP_IHC_dither_Main,pCfg->stFCfg,p8PQType))
    {
        _SetVipCmdqSetFlag(E_MDRV_SCLVIP_IHC_CONFIG);
    }

    _MDrvSclVipCopyIpConfigToGlobal(E_MDRV_SCLVIP_IHC_CONFIG,(void *)pCfg);
    if(_IsNotToCheckPQorCMDQmode())
    {
        MDrvSclVipMutexUnlock();
    }
    SCL_DBG(SCL_DBG_LV_VIP(),
        "[MDRVVIP]%s,addr:%lx flag:%lx \n", __FUNCTION__,(u32)&gstSupCfg.stihc,gstSupCfg.bresetflag);
    return TRUE;
}

bool MDrvSclVipSetICEConfig(void *pvCfg)
{
    MDrvSclVipIccConfig_t *pCfg = pvCfg;
    u8 *p8PQType[(PQ_IP_VIP_ICC_SETTING_Main-PQ_IP_VIP_ICC_dither_Main+2)];
    p8PQType[0] = (u8*)&(pCfg->stEn);
    p8PQType[1] = (u8*)&(pCfg->stYmd);
    p8PQType[2] = (u8*)&(pCfg->stDither);
    p8PQType[3] = (u8*)&(pCfg->stSet);
    if(_IsNotToCheckPQorCMDQmode())
    {
        MDrvSclVipMutexLock();
    }
    _MDrvSclVipSetCmdqStatus(0,pCfg->stFCfg.bEn,pCfg->stFCfg.u8framecnt);
    MDrvSclCmdqMutexLock(E_DRV_SCLCMDQ_TYPE_IP0,1);
    _MDrvSclVipSetPqByType(PQ_IP_VIP_ICC_SETTING_Main,p8PQType[3]);
    MDrvSclCmdqMutexUnlock(E_DRV_SCLCMDQ_TYPE_IP0,1);
    if(_MDrvSclVipForSetEachPqTypeByIp(PQ_IP_VIP_ICC_Main,PQ_IP_VIP_ICC_dither_Main,pCfg->stFCfg,p8PQType))
    {
        _SetVipCmdqSetFlag(E_MDRV_SCLVIP_ICC_CONFIG);
    }

    _MDrvSclVipCopyIpConfigToGlobal(E_MDRV_SCLVIP_ICC_CONFIG,(void *)pCfg);
    if(_IsNotToCheckPQorCMDQmode())
    {
        MDrvSclVipMutexUnlock();
    }
    SCL_DBG(SCL_DBG_LV_VIP(),
        "[MDRVVIP]%s,addr:%lx flag:%lx \n", __FUNCTION__,(u32)&gstSupCfg.sticc,gstSupCfg.bresetflag);
    return TRUE;
}


bool MDrvSclVipSetIhcICCADPYConfig(void *pvCfg)
{
    MDrvSclVipIhcIccConfig_t *pCfg = pvCfg;
    u8 *p8PQType[(PQ_IP_VIP_Ymode_Yvalue_SETTING_Main-PQ_IP_VIP_Ymode_Yvalue_ALL_Main+1)];
    p8PQType[0] = (u8*)&(pCfg->stYmdall);
    p8PQType[1] = (u8*)&(pCfg->stYmdset);
    if(_IsNotToCheckPQorCMDQmode())
    {
        MDrvSclVipMutexLock();
    }

    if(_MDrvSclVipForSetEachPqTypeByIp(PQ_IP_VIP_Ymode_Yvalue_ALL_Main,PQ_IP_VIP_Ymode_Yvalue_SETTING_Main,pCfg->stFCfg,p8PQType))
    {
        _SetVipCmdqSetFlag(E_MDRV_SCLVIP_IHCICC_CONFIG);
    }

    _MDrvSclVipCopyIpConfigToGlobal(E_MDRV_SCLVIP_IHCICC_CONFIG,(void *)pCfg);
    if(_IsNotToCheckPQorCMDQmode())
    {
        MDrvSclVipMutexUnlock();
    }
    SCL_DBG(SCL_DBG_LV_VIP(),
        "[MDRVVIP]%s,addr:%lx flag:%lx \n", __FUNCTION__,(u32)&gstSupCfg.stihcicc,gstSupCfg.bresetflag);
    return TRUE;
}

bool MDrvSclVipSetIbcConfig(void *pvCfg)
{
    MDrvSclVipIbcConfig_t *pCfg = pvCfg;
    u8 *p8PQType[(PQ_IP_VIP_IBC_SETTING_Main-PQ_IP_VIP_IBC_Main+1)];
    p8PQType[0] = (u8*)&(pCfg->stEn);
    p8PQType[1] = (u8*)&(pCfg->stDither);
    p8PQType[2] = (u8*)&(pCfg->stSet);
    if(_IsNotToCheckPQorCMDQmode())
    {
        MDrvSclVipMutexLock();
    }
    if(_MDrvSclVipForSetEachPqTypeByIp(PQ_IP_VIP_IBC_Main,PQ_IP_VIP_IBC_SETTING_Main,pCfg->stFCfg,p8PQType))
    {
        _SetVipCmdqSetFlag(E_MDRV_SCLVIP_IBC_CONFIG);
    }

    _MDrvSclVipCopyIpConfigToGlobal(E_MDRV_SCLVIP_IBC_CONFIG,(void *)pCfg);
    if(_IsNotToCheckPQorCMDQmode())
    {
        MDrvSclVipMutexUnlock();
    }
    SCL_DBG(SCL_DBG_LV_VIP(),
        "[MDRVVIP]%s,addr:%lx flag:%lx \n", __FUNCTION__,(u32)&gstSupCfg.stibc,gstSupCfg.bresetflag);
    return TRUE;
}

bool MDrvSclVipSetFccConfig(void *pvCfg)
{
    MDrvSclVipFccConfig_t *pCfg = pvCfg;
    u8 *p8PQType[(PQ_IP_VIP_FCC_T9_Main-PQ_IP_VIP_FCC_T1_Main+2)];
    p8PQType[9] = (u8*)&(pCfg->stfr);
    p8PQType[0] = (u8*)&(pCfg->stT[0]);
    p8PQType[1] = (u8*)&(pCfg->stT[1]);
    p8PQType[2] = (u8*)&(pCfg->stT[2]);
    p8PQType[3] = (u8*)&(pCfg->stT[3]);
    p8PQType[4] = (u8*)&(pCfg->stT[4]);
    p8PQType[5] = (u8*)&(pCfg->stT[5]);
    p8PQType[6] = (u8*)&(pCfg->stT[6]);
    p8PQType[7] = (u8*)&(pCfg->stT[7]);
    p8PQType[8] = (u8*)&(pCfg->stT9);
    if(_IsNotToCheckPQorCMDQmode())
    {
        MDrvSclVipMutexLock();
    }
    _MDrvSclVipSetCmdqStatus(0,pCfg->stFCfg.bEn,pCfg->stFCfg.u8framecnt);
    MDrvSclCmdqMutexLock(E_DRV_SCLCMDQ_TYPE_IP0,1);
    _MDrvSclVipSetPqByType(PQ_IP_VIP_FCC_full_range_Main,p8PQType[9]);
    MDrvSclCmdqMutexUnlock(E_DRV_SCLCMDQ_TYPE_IP0,1);
    if(_MDrvSclVipForSetEachPqTypeByIp(PQ_IP_VIP_FCC_T1_Main,PQ_IP_VIP_FCC_T9_Main,pCfg->stFCfg,p8PQType))
    {
        _SetVipCmdqSetFlag(E_MDRV_SCLVIP_FCC_CONFIG);
    }

    _MDrvSclVipCopyIpConfigToGlobal(E_MDRV_SCLVIP_FCC_CONFIG,(void *)pCfg);
    if(_IsNotToCheckPQorCMDQmode())
    {
        MDrvSclVipMutexUnlock();
    }
    SCL_DBG(SCL_DBG_LV_VIP(),
        "[MDRVVIP]%s,addr:%lx flag:%lx \n", __FUNCTION__,(u32)&gstSupCfg.stfcc,gstSupCfg.bresetflag);
    return TRUE;
}


bool MDrvSclVipSetAckConfig(void *pvCfg)
{
    MDrvSclVipAckConfig_t *pCfg = pvCfg;
    u8 *p8PQType[(PQ_IP_VIP_YCbCr_Clip_Main-PQ_IP_VIP_ACK_Main+1)];
    p8PQType[0] = (u8*)&(pCfg->stACK);
    p8PQType[1] = (u8*)&(pCfg->stclip);
    if(_IsNotToCheckPQorCMDQmode())
    {
        MDrvSclVipMutexLock();
    }
    if(_MDrvSclVipForSetEachPqTypeByIp(PQ_IP_VIP_ACK_Main,PQ_IP_VIP_YCbCr_Clip_Main,pCfg->stFCfg,p8PQType))
    {
        _SetVipCmdqSetFlag(E_MDRV_SCLVIP_ACK_CONFIG);
    }

    _MDrvSclVipCopyIpConfigToGlobal(E_MDRV_SCLVIP_ACK_CONFIG,(void *)pCfg);
    if(_IsNotToCheckPQorCMDQmode())
    {
        MDrvSclVipMutexUnlock();
    }
    SCL_DBG(SCL_DBG_LV_VIP(),
        "[MDRVVIP]%s,addr:%lx flag:%lx \n", __FUNCTION__,(u32)&gstSupCfg.stack,gstSupCfg.bresetflag);
    return TRUE;
}

bool MDrvSclVipSetNlmConfig(void *pvCfg)
{
    u8 *stNLM = NULL;
    MDrvSclVipNlmConfig_t *pCfg = pvCfg;
    stNLM  = (u8*)&(pCfg->stNLM);
    if(_IsNotToCheckPQorCMDQmode())
    {
        MDrvSclVipMutexLock();
    }

    if(_IsNotOpenVIPBypass(E_MDRV_SCLVIP_NLM_CONFIG))
    {
        MDRv_PQ_Set_CmdqCfg(0,pCfg->stFCfg.bEn,pCfg->stFCfg.u8framecnt,1);
        MDrvSclCmdqMutexLock(E_DRV_SCLCMDQ_TYPE_IP0,pCfg->stFCfg.bEn);
        _MDrvSclVipSetPqByType(PQ_IP_NLM_Main,stNLM);
        MDrvSclCmdqMutexUnlock(E_DRV_SCLCMDQ_TYPE_IP0,pCfg->stFCfg.bEn);
        _MDrvSclVipCopyIpConfigToGlobal(E_MDRV_SCLVIP_NLM_CONFIG,(void *)pCfg);
    }
    else
    {
        _MDrvSclVipCopyIpConfigToGlobal(E_MDRV_SCLVIP_NLM_CONFIG,(void *)pCfg);
    }
    if(_IsNotToCheckPQorCMDQmode())
    {
        MDrvSclVipMutexUnlock();
    }
    SCL_DBG(SCL_DBG_LV_VIP(),
        "[MDRVVIP]%s,addr:%lx flag:%lx \n", __FUNCTION__,(u32)&gstSupCfg.stnlm,gstSupCfg.bresetflag);
    return TRUE;
}

bool MDrvSclVipSetNlmSramConfig(MDrvSclVipNlmSramConfig_t stSRAM)
{
    DrvSclVipNlmSramConfig_t stCfg;
    stCfg.u32baseadr    = _Phys2Miu(stSRAM.u32Baseadr);
    stCfg.bCLientEn     = stSRAM.bEn;
    stCfg.u32viradr     = stSRAM.u32viradr;
    stCfg.btrigContinue = 0;                    //single
    stCfg.u16depth      = VIP_NLM_ENTRY_NUM;    // entry
    stCfg.u16reqlen     = VIP_NLM_ENTRY_NUM;
    stCfg.u16iniaddr    = 0;
    SCL_DBG(SCL_DBG_LV_VIP(),
        "[MDRVVIP]%s, flag:%hhx ,addr:%lx,addr:%lx \n", __FUNCTION__,stSRAM.bEn,stSRAM.u32viradr,stSRAM.u32Baseadr);
    DrvSclVipSetNlmSramConfig(stCfg);
    return TRUE;
}

bool MDrvSclVipSetVipOtherConfig(void *pvCfg)
{
    MDrvSclVipConfig_t *pCfg = pvCfg;
    u8 *p8PQType[(PQ_IP_VIP_Main-PQ_IP_422to444_Main+2)];
    p8PQType[0] = (u8*)&(pCfg->st422_444);
    p8PQType[1] = (u8*)&(pCfg->stBypass);
    p8PQType[2] = (u8*)&(pCfg->stLB);
    if(_IsNotToCheckPQorCMDQmode())
    {
        MDrvSclVipMutexLock();
    }
    _MDrvSclVipSetCmdqStatus(0,pCfg->stFCfg.bEn,pCfg->stFCfg.u8framecnt);
    MDrvSclCmdqMutexLock(E_DRV_SCLCMDQ_TYPE_IP0,1);
    _MDrvSclVipSetPqByType(PQ_IP_VIP_LineBuffer_Main,p8PQType[2]);
    MDrvSclCmdqMutexUnlock(E_DRV_SCLCMDQ_TYPE_IP0,1);
    if(_MDrvSclVipForSetEachPqTypeByIp(PQ_IP_422to444_Main,PQ_IP_VIP_Main,pCfg->stFCfg,p8PQType))
    {
        _SetVipCmdqSetFlag(E_MDRV_SCLVIP_CONFIG);
    }

    _MDrvSclVipCopyIpConfigToGlobal(E_MDRV_SCLVIP_CONFIG,(void *)pCfg);
    if(_IsNotToCheckPQorCMDQmode())
    {
        MDrvSclVipMutexUnlock();
    }
    return TRUE;
}
void MDrvSclVipCheckRegister(void)
{
    SCL_ERR( "[MDRVVIP]%s  \n", __FUNCTION__);
    MDRv_PQ_Check_Type(0,PQ_CHECK_REG);
    MDrvSclVipResetEachIP();
    MDRv_PQ_Check_Type(0,PQ_CHECK_OFF);
}

void MDrvSclVipCheckConsist(void)
{
    SCL_ERR( "[MDRVVIP]%s  \n", __FUNCTION__);
    MDRv_PQ_Check_Type(0,PQ_CHECK_SIZE);
    MDrvSclVipResetEachIP();
    MDRv_PQ_Check_Type(0,PQ_CHECK_OFF);
}
void MDrvSclVipPrepareStructToCheckRegister(MDrvSclVipCmdqCheckType_e enCheckType)
{
    MDrvSclVipAipType_e enIdx = 0;
    SCL_ERR( "[MDRVVIP]%s CHECK_TYPE:%hhd \n", __FUNCTION__,enCheckType);
    MDrvSclVipSetCheckMode(enCheckType);
    if(_IsAutoSetting())
    {
        _SetSuspendResetFlag(0x2FFFF);
        for(enIdx = E_MDRV_SCLVIP_AIP_YEE;enIdx<E_MDRV_SCLVIP_AIP_NUM;enIdx++)
        {
            _SetSuspendAipResetFlag(enIdx);
        }
    }
    _MDrvSclVipForPrepareCheckSetting();


    if(_IsCmdqNeedToReturnOrigin())
    {
        _MDrvSclVipWaitForCmdqDone();
    }
    else
    {
        _MDrvSclVipSetCmdqAfterPollTimeoutSkip(1);//not wait
    }
}
void MDrvSclVipSetCheckMode(MDrvSclVipCmdqCheckType_e enCheckType)
{
    gbVIPCheckCMDQorPQ = enCheckType;
}
bool MDrvSclVipSetAipBypassConfig(MDrvSclVipAipType_e enAIPtype)
{
    MDrvSclVipAipType_e entype;
    if(enAIPtype != E_MDRV_SCLVIP_AIP_NUM)
    {
        _MDrvSclVipModifyAipAlreadySetting(enAIPtype,E_MDRV_SCLVIP_RESET_ZERO);
        _SetAipByPassFlag(enAIPtype);
    }
    else
    {
        for(entype=E_MDRV_SCLVIP_AIP_YEE;entype<E_MDRV_SCLVIP_AIP_NUM;entype++)
        {
            if(_IsAipByPassFlag(entype))
            {
                _MDrvSclVipModifyAipAlreadySetting(entype,E_MDRV_SCLVIP_RESET_ALREADY);
            }
        }
        MDrvSclVipMutexLock();
        gu32AIPOpenBypass = 0;
        MDrvSclVipMutexUnlock();
    }
    return TRUE;
}
bool MDrvSclVipSetVipBypassConfig(MDrvSclVipConfigType_e enVIPtype)
{
    if(enVIPtype)
    {
        if(enVIPtype&E_MDRV_SCLVIP_CONFIG)
        {
            _MDrvSclVipSetBypassIp(enVIPtype&E_MDRV_SCLVIP_CONFIG, E_MDRV_SCLVIP_CONFIG);
        }
        if(enVIPtype&E_MDRV_SCLVIP_MCNR_CONFIG)
        {
            _MDrvSclVipSetBypassIp(enVIPtype&E_MDRV_SCLVIP_MCNR_CONFIG, E_MDRV_SCLVIP_MCNR_CONFIG);
        }
        if(enVIPtype&E_MDRV_SCLVIP_NLM_CONFIG)
        {
            _MDrvSclVipSetBypassIp(enVIPtype&E_MDRV_SCLVIP_NLM_CONFIG, E_MDRV_SCLVIP_NLM_CONFIG);
        }
        if(enVIPtype&E_MDRV_SCLVIP_LDC_CONFIG)
        {
            _MDrvSclVipSetBypassIp(enVIPtype&E_MDRV_SCLVIP_LDC_CONFIG, E_MDRV_SCLVIP_LDC_CONFIG);
        }
        MDrvSclVipMutexLock();
        gu32OpenBypass |= enVIPtype;
        MDrvSclVipMutexUnlock();
    }
    else
    {
        if(gu32OpenBypass&E_MDRV_SCLVIP_CONFIG)
        {
            _MDrvSclVipSetBypassIp(0, E_MDRV_SCLVIP_CONFIG);
        }
        if(gu32OpenBypass&E_MDRV_SCLVIP_MCNR_CONFIG)
        {
            _MDrvSclVipSetBypassIp(0, E_MDRV_SCLVIP_MCNR_CONFIG);
        }
        if(gu32OpenBypass&E_MDRV_SCLVIP_NLM_CONFIG)
        {
            _MDrvSclVipSetBypassIp(0, E_MDRV_SCLVIP_NLM_CONFIG);
        }
        if(gu32OpenBypass&E_MDRV_SCLVIP_LDC_CONFIG)
        {
            _MDrvSclVipSetBypassIp(0, E_MDRV_SCLVIP_LDC_CONFIG);
        }
        MDrvSclVipMutexLock();
        gu32OpenBypass = enVIPtype;
        MDrvSclVipMutexUnlock();
    }
    return TRUE;
}

bool MDrvSclVipCmdqWriteConfig(MDrvSclVipCmdqConfig_t *pCfg)
{
    u32 u32Addr;
    u16 u16Data,u16Mask;
    //u8 u8framecnt;
    u8 bfire;
    //u8 bUseFrameCntCMD;
    DrvSclCmdqCmdReg_t stCfg;
    bfire           = pCfg->bfire;
    //bUseFrameCntCMD = pCfg->bCnt;
    u32Addr         = pCfg->u32Addr;
    u16Data         = pCfg->u16Data;
    u16Mask         = pCfg->u16Mask;
    //u8framecnt      = pCfg->u8framecnt;
    if(bfire)
    {
        DrvSclCmdqFire(E_DRV_SCLCMDQ_TYPE_IP0,bfire);
        SCL_DBG(SCL_DBG_LV_VIP(), "[MDRVVIP]%s, fire\n", __FUNCTION__);
    }
    else
    {
        DrvSclCmdqFillCmd(&stCfg, u32Addr, u16Data, u16Mask);
        if(DrvSclCmdqWriteCmd(E_DRV_SCLCMDQ_TYPE_IP0,stCfg,0) == 0)
        {
            SCL_DBG(SCL_DBG_LV_VIP(), "[MDRVVIP]%s,No frame cmd Write Fail\n", __FUNCTION__);
            return FALSE;
        }
    }
    return TRUE;

}
bool MDrvSclVipSetLdcConfig(void *pvCfg)
{
    MDrvSclVipLdcConfig_t *pCfg = pvCfg;
    u8 bRet = 0;
    if(_IsFrameBufferAllocatedReady())
    {
        //u8 *stEn = NULL;
        u8 *stmd = NULL;
        //stEn = (u8*)&(pCfg->stEn);
        stmd = (u8*)&(pCfg->stmd);
        if(_IsNotToCheckPQorCMDQmode())
        {
            MDrvSclVipMutexLock();
        }
        if(DrvSclVipCheckIpmResolution())
        {

            if(_MDrvSclVipSetCmdqStatus(1,pCfg->stFCfg.bEn,pCfg->stFCfg.u8framecnt))
            {
                _SetVipCmdqSetFlag(E_MDRV_SCLVIP_LDC_CONFIG);
                MDrvSclCmdqMutexLock(E_DRV_SCLCMDQ_TYPE_IP0,1);
                bRet = 1;
            }
            _MDrvSclVipSetPqByType(PQ_IP_LDC_422_444_422_Main,stmd);
            DrvSclVipSetLdcBankMode(pCfg->enLDCType);
            DrvSclVipSetLdcOnConfig(pCfg->stEn.bEn_ldc);
            MDrvSclCmdqMutexUnlock(E_DRV_SCLCMDQ_TYPE_IP0,bRet);

        }
        else
        {
            SCL_ERR("[MDRVVIP]LDC Resolution Over Spec \n");
        }
        _MDrvSclVipCopyIpConfigToGlobal(E_MDRV_SCLVIP_LDC_CONFIG,(void *)pCfg);
        if(_IsNotToCheckPQorCMDQmode())
        {
            MDrvSclVipMutexUnlock();
        }
        SCL_DBG(SCL_DBG_LV_VIP(),
            "[MDRVVIP]%s,addr:%lx flag:%lx \n", __FUNCTION__,(u32)&gstSupCfg.stldc,gstSupCfg.bresetflag);
        return TRUE;
    }
    else
    {
        SCL_ERR( "[MDRVVIP]%s,MCNR buffer not alloc \n", __FUNCTION__);
        return FALSE;
    }
}

bool MDrvSclVipSetLdcmdConfig(void *pvCfg)
{
    MDrvSclVipLdcMdConfig_t *pCfg = pvCfg;
    if(_IsFrameBufferAllocatedReady())
    {
        DrvSclVipLdcMdConfig_t stLDCCfg;
        stLDCCfg.stFCfg.bEn         = pCfg->stFCfg.bEn;
        stLDCCfg.stFCfg.u8framecnt  = pCfg->stFCfg.u8framecnt;
        stLDCCfg.bEnSWMode          = pCfg->bEnSWMode;
        stLDCCfg.u8FBidx            = pCfg->u8FBidx;
        stLDCCfg.u8FBrwdiff         = pCfg->u8FBrwdiff;
        stLDCCfg.enbypass           = pCfg->enbypass;
        stLDCCfg.enLDCType          = pCfg->enLDCType;
        if(_IsNotToCheckPQorCMDQmode())
        {
            MDrvSclVipMutexLock();
        }

        if(DrvSclVipSetLdcMdConfig(stLDCCfg) == 0)
        {
            SCL_DBG(SCL_DBG_LV_VIP(), "[MDRVVIP]%s, Fail\n", __FUNCTION__);
            return FALSE;
        }

        _MDrvSclVipCopyIpConfigToGlobal(E_MDRV_SCLVIP_LDC_MD_CONFIG,(void *)pCfg);
        if(_IsNotToCheckPQorCMDQmode())
        {
            MDrvSclVipMutexUnlock();
        }
        SCL_DBG(SCL_DBG_LV_VIP(),
            "[MDRVVIP]%s,addr:%lx flag:%lx \n", __FUNCTION__,(u32)&gstSupCfg.stldcmd,gstSupCfg.bresetflag);
        return TRUE;
    }
    else
    {
        SCL_ERR( "[MDRVVIP]%s,MCNR buffer not alloc \n", __FUNCTION__);
        return FALSE;
    }
}

bool MDrvSclVipSetLdcDmapConfig(void *pvCfg)
{
    MDrvSclVipLdcDmaPConfig_t *pCfg = pvCfg;
    if(_IsFrameBufferAllocatedReady())
    {
        DrvSclVipLdcDmapConfig_t stLDCCfg;
        stLDCCfg.bEnPowerSave       = pCfg->bEnPowerSave;
        stLDCCfg.u32DMAPaddr        = _Phys2Miu(pCfg->u32DMAPaddr);
        stLDCCfg.u16DMAPWidth       = pCfg->u16DMAPWidth;
        stLDCCfg.u8DMAPoffset       = pCfg->u8DMAPoffset;
        stLDCCfg.stFCfg.bEn         = pCfg->stFCfg.bEn;
        stLDCCfg.stFCfg.u8framecnt  = pCfg->stFCfg.u8framecnt;
        if(_IsNotToCheckPQorCMDQmode())
        {
            MDrvSclVipMutexLock();
        }

        if(DrvSclVipSetLdcDmapConfig(stLDCCfg) == 0)
        {
            SCL_DBG(SCL_DBG_LV_VIP(), "[MDRVVIP]%s, Fail\n", __FUNCTION__);
            return FALSE;
        }

        _MDrvSclVipCopyIpConfigToGlobal(E_MDRV_SCLVIP_LDC_DMAP_CONFIG,(void *)pCfg);
        if(_IsNotToCheckPQorCMDQmode())
        {
            MDrvSclVipMutexUnlock();
        }
        SCL_DBG(SCL_DBG_LV_VIP(),
            "[MDRVVIP]%s,addr:%lx flag:%lx \n", __FUNCTION__,(u32)&gstSupCfg.stldcdmap,gstSupCfg.bresetflag);
        return TRUE;
    }
    else
    {
        SCL_ERR( "[MDRVVIP]%s,MCNR buffer not alloc \n", __FUNCTION__);
        return FALSE;
    }
}

bool MDrvSclVipSetLdcSramConfig(void *pvCfg)
{
    MDrvSclVipLdcSramConfig_t *pCfg = pvCfg;
    if(_IsFrameBufferAllocatedReady())
    {
        DrvSclVipLdcSramConfig_t stLDCCfg;
        stLDCCfg.u16SRAMhorstr      = pCfg->u16SRAMhorstr;
        stLDCCfg.u16SRAMverstr      = pCfg->u16SRAMverstr;
        stLDCCfg.u32loadhoraddr     = _Phys2Miu(pCfg->u32loadhoraddr);
        stLDCCfg.u32loadveraddr     = _Phys2Miu(pCfg->u32loadveraddr);
        stLDCCfg.u16SRAMhoramount   = pCfg->u16SRAMhoramount;
        stLDCCfg.u16SRAMveramount   = pCfg->u16SRAMveramount;
        stLDCCfg.stFCfg.bEn         = pCfg->stFCfg.bEn;
        stLDCCfg.stFCfg.u8framecnt  = pCfg->stFCfg.u8framecnt;
        if(_IsNotToCheckPQorCMDQmode())
        {
            MDrvSclVipMutexLock();
        }

        if(DrvSclVipSetLdcSramConfig(stLDCCfg) == 0)
        {
            SCL_DBG(SCL_DBG_LV_VIP(), "[MDRVVIP]%s, Fail\n", __FUNCTION__);
            return FALSE;
        }

        _MDrvSclVipCopyIpConfigToGlobal(E_MDRV_SCLVIP_LDC_SRAM_CONFIG,(void *)pCfg);
        if(_IsNotToCheckPQorCMDQmode())
        {
            MDrvSclVipMutexUnlock();
        }
        SCL_DBG(SCL_DBG_LV_VIP(),
            "[MDRVVIP]%s,addr:%lx flag:%lx \n", __FUNCTION__,(u32)&gstSupCfg.stldcsram,gstSupCfg.bresetflag);
        return TRUE;
    }
    else
    {
        SCL_ERR( "[MDRVVIP]%s,MCNR buffer not alloc \n", __FUNCTION__);
        return FALSE;
    }
}

bool MDrvSclVipVtrackEnable( u8 u8FrameRate, MDrvSclVipVtrackEnableType_e bEnable)
{
    DrvSclVipVtrackEnable(u8FrameRate, bEnable);
    return 1;
}
bool MDrvSclVipVtrackSetPayloadData(u16 u16Timecode, u8 u8OperatorID)
{
    DrvSclVipVtrackSetPayloadData(u16Timecode,u8OperatorID);
    return 1;
}
bool MDrvSclVipVtrackSetKey(bool bUserDefinded, u8 *pu8Setting)
{
    DrvSclVipVtrackSetKey(bUserDefinded,pu8Setting);
    return 1;
}

bool MDrvSclVipVtrackSetUserDefindedSetting(bool bUserDefinded, u8 *pu8Setting)
{
    DrvSclVipVtrackSetUserDefindedSetting(bUserDefinded,pu8Setting);
    return 1;
}
bool MDrvSclVipVtrackInit(void)
{
    DrvSclVipVtrackSetUserDefindedSetting(0,NULL);
    DrvSclVipVtrackSetPayloadData(0,0);
    DrvSclVipVtrackSetKey(0,NULL);
    return 1;
}
void _MDrvSclVipAipSettingDebugMessage
    (MDrvSclVipAipConfig_t stCfg,void * pvPQSetParameter,u16 u16StructSize)
{
    u16 u16AIPsheet;
    u8 word1,word2;
    u16 u16idx;
    u16AIPsheet = _GetAipOffset(stCfg.u16AIPType);
    SCL_DBG(SCL_DBG_LV_VIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "[MDRVVIP]Sheet:%hd,size:%hd\n",u16AIPsheet,u16StructSize);
    SCL_DBG(SCL_DBG_LV_VIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "[MDRVVIP]copy addr:%lx,vir addr:%lx\n"
        ,(u32)pvPQSetParameter,(u32)stCfg.u32Viraddr);
    if(SCL_DBG_LV_VIP()&EN_DBGMG_VIPLEVEL_VIPLOG)
    {
        for(u16idx =0 ;u16idx<u16StructSize;u16idx++)
        {
            word1 = *((u8 *)pvPQSetParameter+u16idx);
            word2 = *((u8 *)stCfg.u32Viraddr+u16idx);
            SCL_DBG(SCL_DBG_LV_VIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "[MDRVVIP]idx :%hd, copy value:%hhx,vir value:%hhx\n"
                ,u16idx,(u8)(word1),(u8)(word2));
        }
    }
}
void _MDrvSclVipAipSramSettingDebugMessage
    (MDrvSclVipAipSramConfig_t stCfg,void * pvPQSetParameter,u16 u16StructSize)
{
    u8 word1,word2;
    u16 u16idx;
    SCL_DBG(SCL_DBG_LV_VIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "[MDRVVIP]%s\n",__FUNCTION__);
    SCL_DBG(SCL_DBG_LV_VIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "[MDRVVIP]enType:%hd,size:%hd\n",stCfg.enAIPType,u16StructSize);
    SCL_DBG(SCL_DBG_LV_VIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "[MDRVVIP]copy addr:%lx,vir addr:%lx\n"
        ,(u32)pvPQSetParameter,(u32)stCfg.u32Viraddr);
    if(SCL_DBG_LV_VIP()&EN_DBGMG_VIPLEVEL_VIPLOG)
    {
        for(u16idx =0 ;u16idx<u16StructSize;u16idx++)
        {
            word1 = *((u8 *)pvPQSetParameter+u16idx);
            word2 = *((u8 *)stCfg.u32Viraddr+u16idx);
            SCL_DBG(SCL_DBG_LV_VIP()&EN_DBGMG_VIPLEVEL_VIPLOG, "[MDRVVIP]idx :%hd, copy value:%hhx,vir value:%hhx\n"
                ,u16idx,(u8)(word1),(u8)(word2));
        }
    }
}
bool MDrvSclVipSetAipConfig(MDrvSclVipAipConfig_t stCfg)
{
    void * pvPQSetParameter;
    u16 u16StructSize;
    u16 u16AIPsheet;
    u8 bRet = 0;

    if (((stCfg.u16AIPType==E_MDRV_SCLVIP_AIP_GM10TO12)||(stCfg.u16AIPType==E_MDRV_SCLVIP_AIP_GM12TO10)))
    {
        // for test
        return 1;
    }
    if(!_IsAipByPassFlag(stCfg.u16AIPType))
    {
        u16AIPsheet = _GetAipOffset(stCfg.u16AIPType);
        u16StructSize = MDrv_PQ_GetIPRegCount(u16AIPsheet);
        if(_IsNotToCheckPQorCMDQmode())
        {
            MDrvSclVipMutexLock();
        }
        pvPQSetParameter = (void *)
        _MDrvSclVipFillSettingBuffer(u16AIPsheet,stCfg.u32Viraddr,&gstSupCfg.staip[stCfg.u16AIPType].u32Viraddr);
        _MDrvSclVipAipSettingDebugMessage(stCfg,pvPQSetParameter,u16StructSize);
        if(_MDrvSclVipSetCmdqStatus(1,stCfg.stFCfg.bEn,stCfg.stFCfg.u8framecnt))
        {
            _SetAipCmdqSetFlag(stCfg.u16AIPType);
            MDrvSclCmdqMutexLock(E_DRV_SCLCMDQ_TYPE_IP0,1);
            bRet = 1;
        }
        _MDrvSclVipSetPqByType(u16AIPsheet,(u8 *)pvPQSetParameter);
        MDrvSclCmdqMutexUnlock(E_DRV_SCLCMDQ_TYPE_IP0,bRet);
        if(gstSupCfg.staip[stCfg.u16AIPType].u32Viraddr != stCfg.u32Viraddr)
        {
            stCfg.u32Viraddr = (u32)pvPQSetParameter;
            _MDrvSclVipCopyAipConfigToGlobal(stCfg.u16AIPType,(void *)&stCfg,sizeof(MDrvSclVipAipConfig_t));
        }
        if(_IsNotToCheckPQorCMDQmode())
        {
            MDrvSclVipMutexUnlock();
        }
    }
    else
    {
         SCL_DBGERR("[AIP]Bypass AIP:%d\n",stCfg.u16AIPType);
    }
    //DrvSclOsVirMemFree(pvPQSetParameter);
    return 1;
}

u16 _MDrvSclVipGetSramBufferSize(MDrvSclVipAipSramType_e enAIPType)
{
    u16 u16StructSize;
    switch(enAIPType)
    {
        case E_MDRV_SCLVIP_AIP_SRAM_GAMMA_Y:
            u16StructSize = PQ_IP_YUV_Gamma_tblY_SRAM_SIZE_Main;
            break;
        case E_MDRV_SCLVIP_AIP_SRAM_GAMMA_U:
            u16StructSize = PQ_IP_YUV_Gamma_tblU_SRAM_SIZE_Main;
            break;
        case E_MDRV_SCLVIP_AIP_SRAM_GAMMA_V:
            u16StructSize = PQ_IP_YUV_Gamma_tblV_SRAM_SIZE_Main;
            break;
        case E_MDRV_SCLVIP_AIP_SRAM_GM10to12_R:
            u16StructSize = PQ_IP_ColorEng_GM10to12_Tbl_R_SRAM_SIZE_Main;
            break;
        case E_MDRV_SCLVIP_AIP_SRAM_GM10to12_G:
            u16StructSize = PQ_IP_ColorEng_GM10to12_Tbl_G_SRAM_SIZE_Main;
            break;
        case E_MDRV_SCLVIP_AIP_SRAM_GM10to12_B:
            u16StructSize = PQ_IP_ColorEng_GM10to12_Tbl_B_SRAM_SIZE_Main;
            break;
        case E_MDRV_SCLVIP_AIP_SRAM_GM12to10_R:
            u16StructSize = PQ_IP_ColorEng_GM12to10_CrcTbl_R_SRAM_SIZE_Main;
            break;
        case E_MDRV_SCLVIP_AIP_SRAM_GM12to10_G:
            u16StructSize = PQ_IP_ColorEng_GM12to10_CrcTbl_G_SRAM_SIZE_Main;
            break;
        case E_MDRV_SCLVIP_AIP_SRAM_GM12to10_B:
            u16StructSize = PQ_IP_ColorEng_GM12to10_CrcTbl_B_SRAM_SIZE_Main;
            break;
        case E_MDRV_SCLVIP_AIP_SRAM_WDR:
            u16StructSize = (81*2 *8);
            break;
        default:
            u16StructSize = 0;
            break;
    }
    return u16StructSize;
}

bool MDrvSclVipSetAipSramConfig(MDrvSclVipAipSramConfig_t stCfg)
{
    void * pvPQSetParameter;
    void * pvPQSetPara;
    u16 u16StructSize;
    DrvSclVipSramType_e enAIPType;
    if(_IsNotToCheckPQorCMDQmode())
    {
        MDrvSclVipMutexLock();
    }
    u16StructSize = _MDrvSclVipGetSramBufferSize(stCfg.enAIPType);
    pvPQSetParameter = DrvSclOsVirMemalloc(u16StructSize);
    DrvSclOsMemset(pvPQSetParameter,0,u16StructSize);

    if(DrvSclOsCopyFromUser(pvPQSetParameter, (void  *)stCfg.u32Viraddr, u16StructSize))
    {
        if((stCfg.u32Viraddr&MIU0Vir_BASE)==MIU0Vir_BASE)
        {
            DrvSclOsMemcpy(pvPQSetParameter, (void *)stCfg.u32Viraddr, u16StructSize);
        }
        else
        {
            SCL_ERR( "[VIP] copy buffer error\n");
            DrvSclOsVirMemFree(pvPQSetParameter);
            return 0;
        }
    }
    enAIPType = stCfg.enAIPType;
    _MDrvSclVipAipSramSettingDebugMessage(stCfg,pvPQSetParameter,u16StructSize);
    // NOt need to lock CMDQ ,because hal will do it.
    pvPQSetPara = DrvSclVipSetAipSramConfig(pvPQSetParameter,enAIPType);
    if(pvPQSetPara!=NULL)
    {
        DrvSclOsVirMemFree(pvPQSetPara);
    }
    if(_IsNotToCheckPQorCMDQmode())
    {
        MDrvSclVipMutexUnlock();
    }
    return DrvSclVipGetSramCheckPass();
}

u32 MDrvSclVipGetSCLFrameEndCount(void)
{
    DrvSclIrqScIntsType_t stints;
    stints = DrvSclIrqGetSclInts();
    if(stints.u8CountReset)
    {
        return 100;
    }
    else
    {
        return stints.u32SCLMainDoneCount;
    }
}

#if defined (SCLOS_TYPE_LINUX_KERNEL)
ssize_t MDrvSclVipProcShow(char *buf)
{
    char *str = buf;
    char *end = buf + PAGE_SIZE;
    str += DrvSclOsScnprintf(str, end - str, "========================SCL VIP STATUS======================\n");
    str += DrvSclOsScnprintf(str, end - str, "Command       IP      Status\n");
    str += DrvSclOsScnprintf(str, end - str, "----------------------------------------------\n");
    str += DrvSclOsScnprintf(str, end - str, "   0      Normal ALL     %s\n",(gu32OpenBypass) ? "No" :"Yes");
    str += DrvSclOsScnprintf(str, end - str, "   1      Bypass ALL     %s\n",(gu32OpenBypass==0x7FFFF) ? "ON" :"OFF");
    str += DrvSclOsScnprintf(str, end - str, "   2      VIP Bypass     %s\n", DrvSclVipGetBypassStatus(E_MDRV_SCLVIP_CONFIG) ? "ON" :"OFF");
    str += DrvSclOsScnprintf(str, end - str, "   3          MCNR       %s\n", DrvSclVipGetBypassStatus(E_MDRV_SCLVIP_MCNR_CONFIG) ? "Bypass" :"ON");
    str += DrvSclOsScnprintf(str, end - str, "   5          NLM        %s\n", DrvSclVipGetBypassStatus(E_MDRV_SCLVIP_NLM_CONFIG) ? "Bypass" :"ON");
    str += DrvSclOsScnprintf(str, end - str, "   6          LDC        %s\n", DrvSclVipGetBypassStatus(E_MDRV_SCLVIP_LDC_CONFIG) ? "Bypass" :"ON");
    str += DrvSclOsScnprintf(str, end - str, "-------------------AIP------------------------\n");
    if(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_YEE].u32Viraddr)
    {
        str += DrvSclOsScnprintf(str, end - str, "   4          EE         %s\n",
            (*((u8 *)(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_YEE].u32Viraddr)+71)) ? "ON" :"OFF");
    }
    if(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_WDR_GLOB].u32Viraddr)
    {
        str += DrvSclOsScnprintf(str, end - str, "   7      WDR GLOB       %s\n",
            (*((u8 *)(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_WDR_GLOB].u32Viraddr)+0)) ? "ON" :"OFF");
    }
    if(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_WDR_LOC].u32Viraddr)
    {
        str += DrvSclOsScnprintf(str, end - str, "   8      WDR LOCAL      %s\n",
            (*((u8 *)(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_WDR_LOC].u32Viraddr)+0)) ? "ON" :"OFF");
    }
    if(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_MXNR].u32Viraddr)
    {
        str += DrvSclOsScnprintf(str, end - str, "   9          MXNR       %s\n",
            (*((u8 *)(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_MXNR].u32Viraddr)+0)) ? "ON" :"OFF");
    }
    if(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_UVADJ].u32Viraddr)
    {
        str += DrvSclOsScnprintf(str, end - str, "   A          YUVADJ     %s\n",
            (*((u8 *)(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_UVADJ].u32Viraddr)+1)) ? "ON" :"OFF");
    }
    if(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_XNR].u32Viraddr)
    {
        str += DrvSclOsScnprintf(str, end - str, "   B          XNR        %s\n",
            (*((u8 *)(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_XNR].u32Viraddr)+0)) ? "ON" :"OFF");
    }
    if(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_YCUVM].u32Viraddr)
    {
        str += DrvSclOsScnprintf(str, end - str, "   C          YCUVM      %s\n",
            (*((u8 *)(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_YCUVM].u32Viraddr)+0)) ? "Bypass" :"ON");
    }
    if(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_COLORTRAN].u32Viraddr)
    {
        str += DrvSclOsScnprintf(str, end - str, "   D      COLOR TRAN     %s\n",
            (*((u8 *)(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_COLORTRAN].u32Viraddr)+0)) ? "ON" :"OFF");
    }
    if(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_GAMMA].u32Viraddr)
    {
        str += DrvSclOsScnprintf(str, end - str, "   E      YUV GAMMA      %s\n",
            (*((u8 *)(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_GAMMA].u32Viraddr)+0)) ? "ON" :"OFF");
    }
    str += DrvSclOsScnprintf(str, end - str, "---------------COLOR ENGINE--------------------\n");
    if(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_YUVTORGB].u32Viraddr)
    {
        str += DrvSclOsScnprintf(str, end - str, "   F           Y2R       %s\n",
            (*((u8 *)(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_YUVTORGB].u32Viraddr)+0)) ? "ON" :"OFF");
    }
    if(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_GM10TO12].u32Viraddr)
    {
        str += DrvSclOsScnprintf(str, end - str, "   G      GM10to12       %s\n",
            (*((u8 *)(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_GM10TO12].u32Viraddr)+0)) ? "ON" :"OFF");
    }
    if(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_CCM].u32Viraddr)
    {
        str += DrvSclOsScnprintf(str, end - str, "   H           CCM       %s\n",
            (*((u8 *)(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_CCM].u32Viraddr)+0)) ? "ON" :"OFF");
    }
    if(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_HSV].u32Viraddr)
    {
        str += DrvSclOsScnprintf(str, end - str, "   I           HSV       %s\n",
            (*((u8 *)(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_HSV].u32Viraddr)+0)) ? "ON" :"OFF");
    }
    if(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_GM12TO10].u32Viraddr)
    {
        str += DrvSclOsScnprintf(str, end - str, "   J      GM12to10       %s\n",
            (*((u8 *)(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_GM12TO10].u32Viraddr)+0)) ? "ON" :"OFF");
    }
    if(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_RGBTOYUV].u32Viraddr)
    {
        str += DrvSclOsScnprintf(str, end - str, "   K           R2Y       %s\n",
            (*((u8 *)(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_RGBTOYUV].u32Viraddr)+0)) ? "ON" :"OFF");
    }
    str += DrvSclOsScnprintf(str, end - str, "----------------------------------------------\n");
    str += DrvSclOsScnprintf(str, end - str, "========================SCL VIP STATUS======================\n");
    return (str - buf);
}
ssize_t MDrvSclVipVipSetRuleShow(char *buf)
{
    char *str = buf;
    char *end = buf + PAGE_SIZE;
    str += DrvSclOsScnprintf(str, end - str, "========================SCL VIPSetRule======================\n");
    str += DrvSclOsScnprintf(str, end - str, "NOW Rule:%d\n",DrvSclOsGetVipSetRule());
    str += DrvSclOsScnprintf(str, end - str, "echo 0 > VIPSetRule :Default (RIU+assCMDQ)\n");
    str += DrvSclOsScnprintf(str, end - str, "echo 1 > VIPSetRule :CMDQ in active (RIU blanking+CMDQ active)\n");
    str += DrvSclOsScnprintf(str, end - str, "echo 2 > VIPSetRule :CMDQ & checking(like 1,but add IST to check)\n");
    str += DrvSclOsScnprintf(str, end - str, "echo 3 > VIPSetRule :ALLCMDQ (Rule 4)\n");
    str += DrvSclOsScnprintf(str, end - str, "echo 4 > VIPSetRule :ALLCMDQ & checking(like 4,but add IST to checkSRAM)\n");
    str += DrvSclOsScnprintf(str, end - str, "echo 5 > VIPSetRule :ALLCMDQ & checking(like 4,but add IST to check)\n");
    str += DrvSclOsScnprintf(str, end - str, "========================SCL VIPSetRule======================\n");
    return (str - buf);
}

ssize_t MDrvSclVipCmdqShow(char *buf)
{
    char *str = buf;
    char *end = buf + PAGE_SIZE;
    u32 long u64DramMsg;
    u16 u16Count;
    u16 idx,reserve = 0;
    bool bRotate;
    DrvSclCmdqInfo_t stCMDQinfo = DrvSclCmdqGetCMDQInformation(E_DRV_SCLCMDQ_TYPE_IP0);
    if(stCMDQinfo.u16WPoint >= stCMDQinfo.u16LPoint)
    {
        u16Count = stCMDQinfo.u16WPoint - stCMDQinfo.u16LPoint;
    }
    else
    {
        u16Count = stCMDQinfo.u16WPoint + stCMDQinfo.u16MaxCmdCnt-stCMDQinfo.u16LPoint;
        bRotate = 1;
    }
    str += DrvSclOsScnprintf(str, end - str, "========================CMDQ STATUS======================\n");
    str += DrvSclOsScnprintf(str, end - str, "---------------------- SW INFORMATION -------------------\n");
    str += DrvSclOsScnprintf(str, end - str, "CMDQ Current CMD End Pointer      :%hd\n",stCMDQinfo.u16WPoint);
    str += DrvSclOsScnprintf(str, end - str, "CMDQ Last Time Trigger Pointer    :%hd\n",stCMDQinfo.u16RPoint);
    str += DrvSclOsScnprintf(str, end - str, "CMDQ Last Time Trigger COUNT      :%hd\n",u16Count);
    str += DrvSclOsScnprintf(str, end - str, "CMDQ Fire Pointer                 :%hd\n",stCMDQinfo.u16FPoint);
    str += DrvSclOsScnprintf(str, end - str, "---------------------- HW INFORMATION -------------------\n");
    str += DrvSclOsScnprintf(str, end - str, "CMDQ Start                        :%lx\n",stCMDQinfo.PhyAddr);
    str += DrvSclOsScnprintf(str, end - str, "CMDQ End                          :%lx\n",stCMDQinfo.PhyAddrEnd);
    str += DrvSclOsScnprintf(str, end - str, "CMDQ Max CMD One shot             :%hd\n",stCMDQinfo.u16MaxCmdCnt);
    str += DrvSclOsScnprintf(str, end - str, "---------------------- DRAM INFORMATION -----------------\n");
    for(idx=0;idx<u16Count;idx++)
    {
        if(bRotate)
        {
            if((stCMDQinfo.u16LPoint+idx+1)<=stCMDQinfo.u16MaxCmdCnt)
            {
                u64DramMsg = DrvSclCmdqGetCMDFromPoint(E_DRV_SCLCMDQ_TYPE_IP0,stCMDQinfo.u16LPoint+idx);
            }
            else
            {
                u64DramMsg = DrvSclCmdqGetCMDFromPoint(E_DRV_SCLCMDQ_TYPE_IP0,reserve);
                reserve++;
            }
            SCL_DBGERR("%hd:%llx => %lx\n",idx,u64DramMsg,DrvSclCmdqGetCMDBankFromCMD(u64DramMsg));
            //str += DrvSclOsScnprintf(str, end - str, "%hd:%llx => %lx\n",idx,u64DramMsg,DrvSclCmdqGetCMDBankFromCMD(u64DramMsg));
        }
        else
        {
            u64DramMsg = DrvSclCmdqGetCMDFromPoint(E_DRV_SCLCMDQ_TYPE_IP0,stCMDQinfo.u16LPoint+idx);

            SCL_DBGERR("%hd:%llx => %lx\n",idx,u64DramMsg,DrvSclCmdqGetCMDBankFromCMD(u64DramMsg));
            //str += DrvSclOsScnprintf(str, end - str, "%hd:%llx => %lx\n",idx,u64DramMsg,DrvSclCmdqGetCMDBankFromCMD(u64DramMsg));
        }
    }
    str += DrvSclOsScnprintf(str, end - str, "========================CMDQ STATUS======================\n");
    return (str - buf);
}
ssize_t MDrvSclVipVipShow(char *buf)
{
    char *str = buf;
    char *end = buf + PAGE_SIZE;
    str += DrvSclOsScnprintf(str, end - str, "========================SCL VIP STATUS======================\n");
    str += DrvSclOsScnprintf(str, end - str, "     IP      Status\n");
    str += DrvSclOsScnprintf(str, end - str, "---------------VIP---------------------\n");
    str += DrvSclOsScnprintf(str, end - str, "  Normal ALL     %s\n",(gu32OpenBypass) ? "No" :"Yes");
    str += DrvSclOsScnprintf(str, end - str, "  Bypass ALL     %s\n",(gu32OpenBypass==0x7FFFF) ? "ON" :"OFF");
    str += DrvSclOsScnprintf(str, end - str, "  VIP Bypass     %s\n", DrvSclVipGetBypassStatus(E_MDRV_SCLVIP_CONFIG) ? "OFF" :"ON");
    str += DrvSclOsScnprintf(str, end - str, "     MCNR         %s\n", DrvSclVipGetBypassStatus(E_MDRV_SCLVIP_MCNR_CONFIG) ? "Bypass" :"ON");
    str += DrvSclOsScnprintf(str, end - str, "     NLM         %s\n", DrvSclVipGetBypassStatus(E_MDRV_SCLVIP_NLM_CONFIG) ? "Bypass" :"ON");
    str += DrvSclOsScnprintf(str, end - str, "     LDC         %s\n", DrvSclVipGetBypassStatus(E_MDRV_SCLVIP_LDC_CONFIG) ? "Bypass" :"ON");
    str += DrvSclOsScnprintf(str, end - str, "     ACK         %s\n", gstSupCfg.stack.stACK.backen ? "ON" :"OFF");
    str += DrvSclOsScnprintf(str, end - str, "     IBC         %s\n", gstSupCfg.stibc.stEn.bIBC_en ? "ON" :"OFF");
    str += DrvSclOsScnprintf(str, end - str, "     ICC         %s\n", gstSupCfg.sticc.stEn.bICC_en ? "ON" :"OFF");
    str += DrvSclOsScnprintf(str, end - str, "     IHC         %s\n", gstSupCfg.stihc.stOnOff.bIHC_en? "ON" :"OFF");
    str += DrvSclOsScnprintf(str, end - str, "     FCC1        %s\n", gstSupCfg.stfcc.stT[0].bEn ? "ON" :"OFF");
    str += DrvSclOsScnprintf(str, end - str, "     FCC2        %s\n", gstSupCfg.stfcc.stT[1].bEn ? "ON" :"OFF");
    str += DrvSclOsScnprintf(str, end - str, "     FCC3        %s\n", gstSupCfg.stfcc.stT[2].bEn ? "ON" :"OFF");
    str += DrvSclOsScnprintf(str, end - str, "     FCC4        %s\n", gstSupCfg.stfcc.stT[3].bEn ? "ON" :"OFF");
    str += DrvSclOsScnprintf(str, end - str, "     FCC5        %s\n", gstSupCfg.stfcc.stT[4].bEn ? "ON" :"OFF");
    str += DrvSclOsScnprintf(str, end - str, "     FCC6        %s\n", gstSupCfg.stfcc.stT[5].bEn ? "ON" :"OFF");
    str += DrvSclOsScnprintf(str, end - str, "     FCC7        %s\n", gstSupCfg.stfcc.stT[6].bEn ? "ON" :"OFF");
    str += DrvSclOsScnprintf(str, end - str, "     FCC8        %s\n", gstSupCfg.stfcc.stT[7].bEn ? "ON" :"OFF");
    str += DrvSclOsScnprintf(str, end - str, "     FCC9        %s\n", gstSupCfg.stfcc.stT9.bEn ? "ON" :"OFF");
    str += DrvSclOsScnprintf(str, end - str, "     UVC         %s\n", gstSupCfg.stuvc.stUVC.buvc_en? "ON" :"OFF");
    str += DrvSclOsScnprintf(str, end - str, " DLC CURVEFITPW  %s\n", gstSupCfg.stdlc.stEn.bcurve_fit_var_pw_en? "ON" :"OFF");
    str += DrvSclOsScnprintf(str, end - str, " DLC  CURVEFIT   %s\n", gstSupCfg.stdlc.stEn.bcurve_fit_en? "ON" :"OFF");
    str += DrvSclOsScnprintf(str, end - str, " DLC  STATISTIC  %s\n", gstSupCfg.stdlc.stEn.bstatistic_en? "ON" :"OFF");
    str += DrvSclOsScnprintf(str, end - str, "     LCE         %s\n", gstSupCfg.stlce.stOnOff.bLCE_En ? "ON" :"OFF");
    str += DrvSclOsScnprintf(str, end - str, "     PK          %s\n", gstSupCfg.stpk.stOnOff.bpost_peaking_en ? "ON" :"OFF");
    str += DrvSclOsScnprintf(str, end - str, "---------------AIP--------------------\n");

    if(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_YEE].u32Viraddr)
    {
        str += DrvSclOsScnprintf(str, end - str, "     EE          %s\n",
            (*((u8 *)(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_YEE].u32Viraddr)+71)) ? "ON" :"OFF");
        str += DrvSclOsScnprintf(str, end - str, "     EYEE        %s\n",
            (*((u8 *)(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_YEE].u32Viraddr)+70)) ? "ON" :"OFF");
        str += DrvSclOsScnprintf(str, end - str, "     YEE Merge   %s\n",
            (*((u8 *)(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_YEE].u32Viraddr)+204)) ? "ON" :"OFF");
    }

    if(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_YEE_AC_LUT].u32Viraddr)
    {
        str += DrvSclOsScnprintf(str, end - str, "     YC SEC From %s\n",
            (*((u8 *)(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_YEE_AC_LUT].u32Viraddr)+64)==0) ? "YEE" :
            (*((u8 *)(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_YEE_AC_LUT].u32Viraddr)+64)==0x1) ? "2DPK" :
            (*((u8 *)(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_YEE_AC_LUT].u32Viraddr)+64)==0x2) ? "MIX" :"Debug mode" );
    }
    if(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_WDR_GLOB].u32Viraddr)
    {
        str += DrvSclOsScnprintf(str, end - str, "     WDR GLOB    %s\n",
            (*((u8 *)(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_WDR_GLOB].u32Viraddr)+0)) ? "ON" :"OFF");
    }
    if(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_WDR_LOC].u32Viraddr)
    {
        str += DrvSclOsScnprintf(str, end - str, "     WDR LOCAL   %s\n",
            (*((u8 *)(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_WDR_LOC].u32Viraddr)+0)) ? "ON" :"OFF");
    }
    if(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_MXNR].u32Viraddr)
    {
        str += DrvSclOsScnprintf(str, end - str, "     MXNR        %s\n",
            (*((u8 *)(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_MXNR].u32Viraddr)+0)) ? "ON" :"OFF");
    }
    if(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_UVADJ].u32Viraddr)
    {
        str += DrvSclOsScnprintf(str, end - str, "     YUVADJ      %s\n",
            (*((u8 *)(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_UVADJ].u32Viraddr)+1)) ? "ON" :"OFF");
        str += DrvSclOsScnprintf(str, end - str, "     UVADJbyY    %s\n",
            (*((u8 *)(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_UVADJ].u32Viraddr)+0)) ? "ON" :"OFF");
        str += DrvSclOsScnprintf(str, end - str, "     UVADJbyS    %s\n",
            (*((u8 *)(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_UVADJ].u32Viraddr)+2)) ? "ON" :"OFF");
    }
    if(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_XNR].u32Viraddr)
    {
        str += DrvSclOsScnprintf(str, end - str, "     XNR         %s\n",
            (*((u8 *)(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_XNR].u32Viraddr)+0)) ? "ON" :"OFF");
    }
    if(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_YCUVM].u32Viraddr)
    {
        str += DrvSclOsScnprintf(str, end - str, "     YCUVM       %s\n",
            (*((u8 *)(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_YCUVM].u32Viraddr)+0)) ? "Bypass" :"ON");
    }
    if(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_COLORTRAN].u32Viraddr)
    {
        str += DrvSclOsScnprintf(str, end - str, "     COLOR TRAN  %s\n",
            (*((u8 *)(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_COLORTRAN].u32Viraddr)+0)) ? "ON" :"OFF");
    }
    if(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_GAMMA].u32Viraddr)
    {
        str += DrvSclOsScnprintf(str, end - str, "     YUV GAMMA   %s\n",
            (*((u8 *)(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_GAMMA].u32Viraddr)+0)) ? "ON" :"OFF");
    }
    str += DrvSclOsScnprintf(str, end - str, "---------------COLOR ENGINE--------------------\n");
    if(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_YUVTORGB].u32Viraddr)
    {
        str += DrvSclOsScnprintf(str, end - str, "     Y2R         %s\n",
            (*((u8 *)(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_YUVTORGB].u32Viraddr)+0)) ? "ON" :"OFF");
    }
    if(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_GM10TO12].u32Viraddr)
    {
        str += DrvSclOsScnprintf(str, end - str, "     GM10to12    %s\n",
            (*((u8 *)(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_GM10TO12].u32Viraddr)+0)) ? "ON" :"OFF");
    }
    if(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_CCM].u32Viraddr)
    {
        str += DrvSclOsScnprintf(str, end - str, "     CCM         %s\n",
            (*((u8 *)(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_CCM].u32Viraddr)+0)) ? "ON" :"OFF");
    }
    if(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_HSV].u32Viraddr)
    {
        str += DrvSclOsScnprintf(str, end - str, "     HSV         %s\n",
            (*((u8 *)(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_HSV].u32Viraddr)+0)) ? "ON" :"OFF");
    }
    if(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_GM12TO10].u32Viraddr)
    {
        str += DrvSclOsScnprintf(str, end - str, "     GM12to10    %s\n",
            (*((u8 *)(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_GM12TO10].u32Viraddr)+0)) ? "ON" :"OFF");
    }
    if(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_RGBTOYUV].u32Viraddr)
    {
        str += DrvSclOsScnprintf(str, end - str, "     R2Y         %s\n",
            (*((u8 *)(gstSupCfg.staip[E_MDRV_SCLVIP_AIP_RGBTOYUV].u32Viraddr)+0)) ? "ON" :"OFF");
    }
    str += DrvSclOsScnprintf(str, end - str, "---------------SRAM--------------------\n");
    str += DrvSclOsScnprintf(str, end - str, "-1-----------Y GAMMA--------------------\n");
    str += DrvSclOsScnprintf(str, end - str, "-2-----------U GAMMA--------------------\n");
    str += DrvSclOsScnprintf(str, end - str, "-3-----------V GAMMA--------------------\n");
    str += DrvSclOsScnprintf(str, end - str, "-4-----------R GAMMAA2C--------------------\n");
    str += DrvSclOsScnprintf(str, end - str, "-5-----------G GAMMAA2C--------------------\n");
    str += DrvSclOsScnprintf(str, end - str, "-6-----------B GAMMAA2C--------------------\n");
    str += DrvSclOsScnprintf(str, end - str, "-7-----------R GAMMAC2A--------------------\n");
    str += DrvSclOsScnprintf(str, end - str, "-8-----------G GAMMAC2A--------------------\n");
    str += DrvSclOsScnprintf(str, end - str, "-9-----------B GAMMAC2A--------------------\n");
    str += DrvSclOsScnprintf(str, end - str, "-A-----------WDR--------------------\n");
    str += DrvSclOsScnprintf(str, end - str, "========================SCL VIP STATUS======================\n");
    return (str - buf);
}
void MDrvSclVipVipStore(const char *buf)
{
    const char *str = buf;
    if(NULL != buf)
    {
        if((int)*str == 49)    //input 1  echo 1 >
        {
            DrvSclCmdqCheckVIPSRAM(E_DRV_SCLIRQ_EVENT_GAMMA_Y);
        }
        else if((int)*str == 50)  //input 2  echo 2 >
        {
            DrvSclCmdqCheckVIPSRAM(E_DRV_SCLIRQ_EVENT_GAMMA_U);
        }
        else if((int)*str == 51)  //input 3  echo 3 >
        {
            DrvSclCmdqCheckVIPSRAM(E_DRV_SCLIRQ_EVENT_GAMMA_V);
        }
        else if((int)*str == 52)  //input 4  echo 4 >
        {
            DrvSclCmdqCheckVIPSRAM(E_DRV_SCLIRQ_EVENT_GM10to12_R);
        }
        else if((int)*str == 53)  //input 5  echo 5 >
        {
            DrvSclCmdqCheckVIPSRAM(E_DRV_SCLIRQ_EVENT_GM10to12_G);
        }
        else if((int)*str == 54)  //input 6  echo 6 >
        {
            DrvSclCmdqCheckVIPSRAM(E_DRV_SCLIRQ_EVENT_GM10to12_B);
        }
        else if((int)*str == 55)  //input 7  echo 7 >
        {
            DrvSclCmdqCheckVIPSRAM(E_DRV_SCLIRQ_EVENT_GM12to10_R);
        }
        else if((int)*str == 56)  //input 8  echo 8 >
        {
            DrvSclCmdqCheckVIPSRAM(E_DRV_SCLIRQ_EVENT_GM12to10_G);
        }
        else if((int)*str == 57)  //input 9  echo 9 >
        {
            DrvSclCmdqCheckVIPSRAM(E_DRV_SCLIRQ_EVENT_GM12to10_B);
        }
        else if((int)*str == 65)  //input A  echo A >
        {
        }

    }
}
#endif
