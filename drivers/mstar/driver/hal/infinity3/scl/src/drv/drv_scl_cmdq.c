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
////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2009 MStar Semiconductor, Inc.
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
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
//
// file    drvCMDQ.c
// @brief  CMDQ Driver
// @author MStar Semiconductor,Inc.
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#define DRV_CMDQ_C

#include "drv_scl_os.h"
#include "hal_scl_util.h"

#include "drv_scl_vip.h"
#include "drv_scl_cmdq.h"
#include "hal_scl_cmdq.h"
#include "drv_scl_irq_st.h"
#include "drv_scl_irq.h"
#include "drv_scl_dbg.h"
#include "hal_scl_reg.h"


static DrvSclCmdqInfo_t gstCMDQInfo[E_DRV_SCLCMDQ_TYPE_MAX] = {{0,0,0,0,0,FALSE,0,0,0,0}};
static u8 gCMDQStatusFlag[E_DRV_SCLCMDQ_TYPE_MAX];
static u8 gbCMDQBuferrFull[E_DRV_SCLCMDQ_TYPE_MAX];
static DrvSclCmdqBufferMiu_t gstMIUSel[E_DRV_SCLCMDQ_TYPE_MAX];
DrvSclCmdqDirectmodeinst_t *gpstCMDQDirinst;
DrvSclCmdqDirectmodeinst_t *gpstCMDQDefaultinst;
//--------------------------------------------------------------------------------------------------
//  Global Variable
//--------------------------------------------------------------------------------------------------
static u32 gu32CMDQMode;
static DrvSclCmdqIpType_e gstCurrworkIP;
static u32 gu32LasttimeFramecnt;//1.last time isp cnt
bool gbISRopen;
u16  gu16ISRflag;
s32 _gCMDQHVSPMUTEX[E_DRV_SCLCMDQ_TYPE_MAX] = {-1,-1,-1};
static s32 _gs32CMDQMutex = -1;
#define CMDQ_MUTEX_CREATE()  _gs32CMDQMutex = DrvSclOsCreateMutex(E_DRV_SCLOS_FIFO, "CMDQMUTEX" , SCLOS_PROCESS_SHARED)
#define CMDQ_MUTEX_LOCK()                       DrvSclOsObtainMutex(_gs32CMDQMutex,SCLOS_WAIT_FOREVER)
#define CMDQ_MUTEX_UNLOCK()                     DrvSclOsReleaseMutex(_gs32CMDQMutex)
#define CMDQ_MUTEX_DELETE()                     DrvSclOsDeleteMutex(_gs32CMDQMutex)
#define _AlignTo(value, align)                  ( ((value) + ((align)-1)) & ~((align)-1) )
#define _CMDQGetDirAddr(u16Point)                (gpstCMDQDirinst->u32VirAddr + ((u16Point)* HAL_SCLCMDQ_CMD_LEN) )
#define _CMDQGetAddr(u16Point)                (gstCMDQInfo[gstCurrworkIP].u32VirAddr + ((u16Point)* HAL_SCLCMDQ_CMD_LEN) )
#define _CMDQGetSramAddr(u16Point)                (gpstCMDQDirinst->stSramVir.pvVirAddr + ((u16Point)* HAL_SCLCMDQ_CMD_LEN) )
#define _CMDQGetCMD(u8type,u32Addr,u16Data,u16Mask) ((u64)(((u64)(u8type)<<(14*4))|((u64)(u32Addr)<<(8*4))|\
                                                ((u64)(u16Data)<<(4*4))|((u64)(u16Mask))))
#define _IsIspCountReset(u8isp_cnt,u8framecnt)  (((gu32LasttimeFramecnt)>(u8isp_cnt))&&\
                                                ((gu32LasttimeFramecnt)>(u8framecnt)))
#define _IsAssignCountReset(u8isp_cnt,u8framecnt)  ((u8framecnt<u8isp_cnt)&&\
                                                    ((gu32LasttimeFramecnt)>=(u8framecnt)))
#define _IsFrameCountError(u8isp_cnt,u8framecnt,u8allowframeerror) (((s16)u8isp_cnt)>(s16)((u8framecnt)+(u8allowframeerror)))\
                                                                &&((u8isp_cnt-u8framecnt)<10)
#define _IsFrameCountWarn(u8isp_cnt,u8framecnt,u8allowframeerror) (((s16)(u8isp_cnt)-(s16)(u8framecnt))>=0&&\
                                                      ((s16)(u8isp_cnt)-(s16)(u8framecnt))<=(u8allowframeerror))
#define _IsCMDQExecuteDone(enIPType)              (HalSclCmdqGetFinalIrq(enIPType, 0x0003)==0x3 && HalSclCmdqGetRawIrq(enIPType, 0x0800))
#define _IsCMDQExecuteDoneISR(enIPType)           (((gu16ISRflag&0x4) == 0x4) && HalSclCmdqGetRawIrq(enIPType, 0x0800))
#define _IsCMDQIPIdle(enIPType)               (HalSclCmdqGetRawIrq(enIPType, 0x0800))
#define _IsCMDQDMADone(enIPType)                   (HalSclCmdqGetRawIrq(enIPType, 0x0002))
#define _IsCMDQFlagType(IpNum,u8type)               (gCMDQStatusFlag[IpNum] & (u8type))
#define _SetCMDQFlagType(IpNum,u8type)              (gCMDQStatusFlag[IpNum] |= (u8type))
#define _ReSetCMDQFlagType(IpNum,u8type)            (gCMDQStatusFlag[IpNum] &= ~(u8type))
#define _IsThisCmdAddrAlreadyExist(u32addr,u64cpcmd) (u32addr==((u32)((u64cpcmd &0xFFFFFFFF00000000)>>32)))
#define _IsIdxLargeThan(u32ShiftIdx,u16Num) (u32ShiftIdx>u16Num)
//--------------------------------------------------------------------------------------------------
//  Debug Function
//--------------------------------------------------------------------------------------------------
#define  CMDQDBG(x)
#define  CMDQERR(x) x
#define  CMDQCMD128 0 //one cmd 64bit but add one null cmd
#define  NotToCheckSameAddr 1
#define  ToCheckSameAddr 0
#define  RETURN_ERROR 2
#define CheckingTimes 35
//#define  INT_IRQ_CMDQ 52
//--------------------------------------------------------------------------------------------------
//  Global Function
//--------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------
///Get MMIO_Base and set struct _REG_CMDQCtrl * _CMDQCtrl
//---------------------------------------------------------------------------
void DrvSclCmdqSetForceSkip(bool bEn)
{
}


//DRV-----------------------------------------------------------------------------------------------
u32 _DrvSclCmdqSetRingTrigPointer(DrvSclCmdqIpType_e enIPType,u32 StartAddr)
{
    HalSclCmdqSetOffsetPointer(enIPType,StartAddr);
    return 1;
}

u32 _DrvSclCmdqSetStartPointer(DrvSclCmdqIpType_e enIPType,u32 StartAddr)
{
    HalSclCmdqSetStartPointer(enIPType,StartAddr);
    return DRV_SCLCMDQ_OK;
}

u32 _DrvSclCmdqSetEndPointer(DrvSclCmdqIpType_e enIPType,u32 EndAddr)
{
    HalSclCmdqSetEndPointer(enIPType,EndAddr);
    return DRV_SCLCMDQ_OK;
}

u32 _DrvSclCmdqSetTimerRatio(DrvSclCmdqIpType_e enIPType,u32 u32time,u32 u32ratio)
{
    HalSclCmdqSetTimer(enIPType, u32time);
    HalSclCmdqSetRatio(enIPType, u32ratio);
    return DRV_SCLCMDQ_OK;
}
u64 *_DrvSclCmdqGetSramPointerFromPoint(u16 u16Point,u64 *pu32Addr)
{
    u32 DstAddr;
    DstAddr = (u32)_CMDQGetSramAddr(u16Point);
    pu32Addr = (u64 *)(DstAddr);
    return pu32Addr;
}
u64 *_DrvSclCmdqGetPointerFromPoint(u16 u16Point,u64 *pu32Addr)
{
    u32 DstAddr;
    DstAddr = _CMDQGetAddr(u16Point);
    pu32Addr = (u64 *)(DstAddr);
    return pu32Addr;
}
u64 *_DrvSclCmdqGetDirectModePointerFromPoint(u16 u16Point,u64 *pu32Addr)
{
    u32 DstAddr;
    DstAddr = _CMDQGetDirAddr(u16Point);
    pu32Addr = (u64 *)(DstAddr);
    return pu32Addr;
}
void _DrvSclCmdqModifyCmdToPointer(u64 u64Cmd,u64 *pu32Addr)
{
    if((*pu32Addr&0xFFFFFFFF00000000 )==(u64Cmd&0xFFFFFFFF00000000 ))
    {
        *pu32Addr =u64Cmd ;
    }
    else
    {
        *pu32Addr =u64Cmd ;
        SCL_ERR("[CMDQ]%s Maybe Error\n",__FUNCTION__);
    }
}
void _DrvSclCmdqSetCmdToPointer(u64 u64Cmd ,bool bSkipCheckSameAddr,u64 *pu32Addr)
{
    if(!bSkipCheckSameAddr)
    {
        *pu32Addr = ((u64Cmd &0xFFFFFFFFFFFF0000)|0x0000);
    }
    else
    {
        *pu32Addr =u64Cmd ;
    }
}
bool _DrvSclCmdqModifyDirectModeCmd(u64 u64Cmd, u16 u16BufferCnt)
{
    u64 *pu32Addr = NULL;
    pu32Addr = _DrvSclCmdqGetDirectModePointerFromPoint(u16BufferCnt,pu32Addr);
    _DrvSclCmdqModifyCmdToPointer(u64Cmd,pu32Addr);
    SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_NORMAL,"[CMDQ]modify u16WPoint=%hu u16BufferCnt=%hu\n"
        ,gpstCMDQDirinst->u16WPoint,u16BufferCnt);
    return 1;
}
bool _DrvSclCmdqAddDirectModeCmd(u64 u64Cmd)
{
    u64 *pu32Addr = NULL;
    pu32Addr = _DrvSclCmdqGetDirectModePointerFromPoint(gpstCMDQDirinst->u16WPoint,pu32Addr);
    _DrvSclCmdqSetCmdToPointer(u64Cmd,1,pu32Addr);
    gpstCMDQDirinst->u16WPoint++;
    SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_NORMAL,"[CMDQ]add u16WPoint=%d \n"
        ,gpstCMDQDirinst->u16WPoint);
    return 1;
}
bool _DrvSclCmdqAddSramCmd(u64 u64Cmd)
{
    u64 *pu32Addr = NULL;
    pu32Addr = _DrvSclCmdqGetSramPointerFromPoint(gpstCMDQDirinst->stSramVir.u16WPoint,pu32Addr);
    _DrvSclCmdqSetCmdToPointer(u64Cmd,1,pu32Addr);
    gpstCMDQDirinst->stSramVir.u16WPoint++;
    return 1;
}
bool _DrvSclCmdqAddCmd(u64 u64Cmd ,bool bSkipCheckSameAddr)
{
    u64 *pu32Addr = NULL;
    if(gbCMDQBuferrFull[gstCurrworkIP])
    {
        return 0;
    }
    pu32Addr = _DrvSclCmdqGetPointerFromPoint(gstCMDQInfo[gstCurrworkIP].u16WPoint,pu32Addr);
    _DrvSclCmdqSetCmdToPointer(u64Cmd,bSkipCheckSameAddr,pu32Addr);
    gstCMDQInfo[gstCurrworkIP].u16WPoint++;
    SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_NORMAL,"[CMDQ]add u16WPoint=%d \n"
        ,gstCMDQInfo[gstCurrworkIP].u16WPoint);
    if( ((gstCMDQInfo[gstCurrworkIP].u16WPoint)) ==((gstCMDQInfo[gstCurrworkIP].u16DPoint)))
    {
        gbCMDQBuferrFull[gstCurrworkIP] = 1;
        SCL_ERR("[CMDQ]CMDQ BUFFER FULL @:%lu\n",((u32)DrvSclOsGetSystemTimeStamp()));
        SCL_ERR("[CMDQ]add u16DPoint=%d u16WPoint=%d\n"
            ,gstCMDQInfo[gstCurrworkIP].u16DPoint,gstCMDQInfo[gstCurrworkIP].u16WPoint);
    }
    gu32LasttimeFramecnt++;
    return 1;
}

void _DrvSclCmdqResetCmdPointIfBottom(DrvSclCmdqIpType_e enIPType)
{
    if(gstCMDQInfo[enIPType].u16WPoint >= gstCMDQInfo[enIPType].u16MaxCmdCnt)
    {
        gstCMDQInfo[enIPType].u16WPoint = 0;
        //SCL_DBGERR("[CMDQ]CMDQ BUFFER ResetCmdPoint @:%lu\n",((u32)DrvSclOsGetSystemTimeStamp()));
    }
}
void _DrvSclCmdqAddNull(DrvSclCmdqIpType_e enIPType)
{
    u64 nullcmd = HAL_SCLCMDQ_NULL_CMD;
    do
    {
        _DrvSclCmdqAddCmd((u64)nullcmd, ToCheckSameAddr);
        _DrvSclCmdqResetCmdPointIfBottom(enIPType);
    }while (gstCMDQInfo[enIPType].u16WPoint%HAL_SCLCMDQ_CMD_ALIGN != 0);
}
void _DrvSclCmdqAddDirectModeNull(void)
{
    u64 nullcmd = HAL_SCLCMDQ_NULL_CMD;
    do
    {
        _DrvSclCmdqAddDirectModeCmd((u64)nullcmd);
    }while (gpstCMDQDirinst->u16WPoint%HAL_SCLCMDQ_CMD_ALIGN != 0);
}

void _DrvSclCmdqWriteRegDirect(u32 u32Addr,u16 u16Data)
{
    HalSclCmdqWriteRegDirect(u32Addr,u16Data);
}

u32 _DrvSclCmdqPrintfCashCommand(DrvSclCmdqIpType_e enIPType)
{
    u32 Command_15_0_bit, Command_31_16_bit, Command_55_32_bit, Command_63_56_bit,read_addr;
    Command_15_0_bit = HalSclCmdqErrorCommand(enIPType,HAL_SCLCMDQ_CRASH_15_0_BIT);
    Command_31_16_bit = HalSclCmdqErrorCommand(enIPType,HAL_SCLCMDQ_CRASH_31_16_BIT);
    Command_55_32_bit = HalSclCmdqErrorCommand(enIPType,HAL_SCLCMDQ_CRASH_55_32_BIT);
    Command_63_56_bit = HalSclCmdqErrorCommand(enIPType,HAL_SCLCMDQ_CRASH_63_56_BIT);
    read_addr = HalSclCmdqGetMIUReadAddr(enIPType);
    SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_NORMAL,
        "[CMDQ]\033[35mCommand _bits: %lx, %lx, %lx, %lx read_addr: %lx\033[m\n"
        , Command_63_56_bit, Command_55_32_bit, Command_31_16_bit, Command_15_0_bit,read_addr);
    return DRV_SCLCMDQ_OK;
}
u32 DrvSclCmdqBeTrigger(DrvSclCmdqIpType_e enIPType,bool bStart)
{
    if(bStart)
    {
        HalSclCmdqEnable(enIPType,bStart);
        HalSclCmdqStart(enIPType,bStart);
        _DrvSclCmdqPrintfCashCommand(enIPType);
        DrvSclCmdqSetDPoint(enIPType);
        return DRV_SCLCMDQ_OK;
    }
    else
    {
        CMDQERR(sclprintf("bStart %d, \n",bStart));
        return DRV_SCLCMDQ_FAIL;
    }
}
void _DrvSclCmdqCheckMiuAddr(void)
{
    u64 *pu32Addr = NULL;
    #if CMDQCMD128
        pu32Addr = _DrvSclCmdqGetPointerFromPoint((gstCMDQInfo[gstCurrworkIP].u16WPoint -2),pu32Addr);
        SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_HIGH,"[CMDQ] WP:%hd MIU:%llx"
            ,gstCMDQInfo[gstCurrworkIP].u16WPoint -2,*pu32Addr);
    #else
        pu32Addr = _DrvSclCmdqGetPointerFromPoint((gstCMDQInfo[gstCurrworkIP].u16WPoint -1),pu32Addr);
        SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_HIGH,"[CMDQ] WP:%hd MIU:%llx"
            ,gstCMDQInfo[gstCurrworkIP].u16WPoint -1,*pu32Addr);
    #endif
}
void DrvSclCmdqSetRPoint(DrvSclCmdqIpType_e enIPType)
{
    gstCMDQInfo[enIPType].u16RPoint =
        (gstCMDQInfo[enIPType].u16FPoint>0)? (gstCMDQInfo[enIPType].u16FPoint-1): gstCMDQInfo[enIPType].u16MaxCmdCnt;
}
void DrvSclCmdqSetDPoint(DrvSclCmdqIpType_e enIPType)
{
    gstCMDQInfo[enIPType].u16DPoint =
        (gstCMDQInfo[enIPType].u16FPoint>0)? (gstCMDQInfo[enIPType].u16FPoint-1): gstCMDQInfo[enIPType].u16MaxCmdCnt;
}
void DrvSclCmdqSetLPoint(DrvSclCmdqIpType_e enIPType)
{
    static u16 u16lp=0;
    gstCMDQInfo[enIPType].u16LPoint = u16lp;
    u16lp = gstCMDQInfo[enIPType].u16DPoint;
    _DrvSclCmdqPrintfCashCommand(enIPType);
}
u32 _DrvSclCmdqGetEndCmdAddr(DrvSclCmdqIpType_e enIPType)
{
    u32 u32EndAddr = 0;
    gstCMDQInfo[enIPType].u16FPoint = gstCMDQInfo[enIPType].u16WPoint + 1;
    SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_LOW,"[CMDQ]u16WPoint=%u u16RPoint=%u u16DPoint=%u\n",
        gstCMDQInfo[enIPType].u16WPoint,
        gstCMDQInfo[enIPType].u16RPoint,gstCMDQInfo[enIPType].u16DPoint);

    u32EndAddr = gstCMDQInfo[enIPType].PhyAddr+
        (((gstCMDQInfo[enIPType].u16FPoint)/HAL_SCLCMDQ_CMD_ALIGN)*HAL_SCLCMDQ_MEM_BASE_UNIT);
    return u32EndAddr;
}
void _DrvSclCmdqReSetBufferFull(DrvSclCmdqIpType_e enIPType)
{
    gbCMDQBuferrFull[enIPType] = 0;
}
void _DrvSclCmdqFillInitIpInfo
    (DrvSclCmdqIpType_e enIPType,u32 IP0PhyAddr, u32 u32IP0VirAddr, u32 u32CMDQBufSize)
{
    SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_NORMAL,
        "[CMDQ]PhyAddr:%lx   u32VirAddr:%lx \n",IP0PhyAddr,u32IP0VirAddr);
    gstCMDQInfo[enIPType].bEnable = FALSE;
    gstCMDQInfo[enIPType].u16RPoint = 0;             //last trig
    gstCMDQInfo[enIPType].u16DPoint = 0;             //last done
    gstCMDQInfo[enIPType].u16WPoint = 0;             //current cmd end
    gstCMDQInfo[enIPType].u16FPoint = 0;             //fire point (128 bit/unit)
    gstCMDQInfo[enIPType].u16MaxCmdCnt = (u16)(u32CMDQBufSize / HAL_SCLCMDQ_CMD_LEN);
    gstCMDQInfo[enIPType].PhyAddr = IP0PhyAddr;
    gstCMDQInfo[enIPType].u32VirAddr = u32IP0VirAddr;
    gstCMDQInfo[enIPType].PhyAddrEnd = IP0PhyAddr+
        (gstCMDQInfo[enIPType].u16MaxCmdCnt/2)*HAL_SCLCMDQ_MEM_BASE_UNIT;
    gCMDQStatusFlag[enIPType] = (E_DRV_SCLCMDQ_FLAG_FIRE);
    gbCMDQBuferrFull[enIPType] = 0;
}
void _DrvSclCmdqInit(u32 phyAddr1)
{
    if (0)
    {
        gstMIUSel[0].u8MIUSel_IP = E_DRV_SCLCMDQ_MIU_1;
        SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_LOW,"[CMDQ]%s: Init BF1 at MIU 1\n"
            ,__FUNCTION__);
    }
    else
    {
        gstMIUSel[0].u8MIUSel_IP = E_DRV_SCLCMDQ_MIU_0;
        SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_LOW,"[CMDQ]%s: Init BF1 at MIU 0 \n"
            ,__FUNCTION__);
    }
    gu32CMDQMode = E_DRV_SCLCMDQ_RING_BUFFER_MODE;
    gu32LasttimeFramecnt = 0;
    gstCurrworkIP = E_DRV_SCLCMDQ_TYPE_IP0;
}
void _DrvSclCmdqReset(DrvSclCmdqIpType_e enIPType)
{
    u32 u32CMDQBufSize =
        (gstCMDQInfo[enIPType].u16MaxCmdCnt)*HAL_SCLCMDQ_CMD_LEN;
    _DrvSclCmdqFillInitIpInfo(enIPType, gstCMDQInfo[enIPType].PhyAddr,
        gstCMDQInfo[enIPType].u32VirAddr,u32CMDQBufSize);
    _DrvSclCmdqInit(gstCMDQInfo[enIPType].PhyAddr);
    HalSclCmdqReset(enIPType, 1);
    HalSclCmdqReset(enIPType, 0);
    _DrvSclCmdqAddNull(enIPType);//add 2 null
    gstCMDQInfo[enIPType].u16FPoint = gstCMDQInfo[enIPType].u16WPoint + 1;
}
bool _DrvSclCmdqDirectBufferRingModeFire(DrvSclCmdqIpType_e enIPType,bool bStart)
{
    u32 u32EndAddr = 0;
    u8 ret=0;
    u32 u32Events=0;
    SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_LOW,"[CMDQ]%s count:%ld\n",__FUNCTION__,gu32LasttimeFramecnt);
    if(gbCMDQBuferrFull[enIPType])
    {
        gstCMDQInfo[enIPType].u16WPoint = gstCMDQInfo[enIPType].u16RPoint;
        ret=0;
    }
    else
    {
        gu32LasttimeFramecnt = 0;
        u32EndAddr = _DrvSclCmdqGetEndCmdAddr(enIPType);
        ret =_DrvSclCmdqSetRingTrigPointer(enIPType,u32EndAddr);  //in function,addr already add 1
    }
    if(ret)
    {
        DrvSclOsWaitForCpuWriteToDMem();
        _DrvSclCmdqCheckMiuAddr();
        DrvSclOsClearEventIRQ(DrvSclIrqGetIrqSYNCEventID(),E_DRV_SCLIRQ_EVENT_CMDQ);
        DrvSclCmdqBeTrigger(enIPType,bStart);
        DrvSclCmdqSetRPoint(enIPType);
        SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_NORMAL,"[CMDQ]go CMDQ u32EndAddr:%lx\n",u32EndAddr);
    }
    else
    {
        CMDQERR(sclprintf("[CMDQ]Block Fire for CMDQ FULL!!!\n"));
        if(DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(),
            E_DRV_SCLIRQ_EVENT_CMDQDONE, &u32Events, E_DRV_SCLOS_EVENT_MD_OR, 1000))
        {
            _DrvSclCmdqReSetBufferFull(enIPType);
        }
        else
        {
            _DrvSclCmdqReset(enIPType);
            _DrvSclCmdqReSetBufferFull(enIPType);
            CMDQERR(sclprintf("[CMDQ]Reset CMDQ!!!\n"));
        }
        return FALSE;
    }

    return TRUE;
}
bool _DrvSclCmdqFire(DrvSclCmdqIpType_e enIPType,bool bStart)
{
    u32 u32EndAddr = 0;
    u8 ret=0;
    u32 u32Events=0;
    SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_LOW,"[CMDQ]%s count:%ld\n",__FUNCTION__,gu32LasttimeFramecnt);
    if(gbCMDQBuferrFull[enIPType])
    {
        gstCMDQInfo[enIPType].u16WPoint = gstCMDQInfo[enIPType].u16RPoint;
        ret=0;
    }
    else
    {
        gu32LasttimeFramecnt = 0;
        u32EndAddr = _DrvSclCmdqGetEndCmdAddr(enIPType);
        ret =_DrvSclCmdqSetRingTrigPointer(enIPType,u32EndAddr);  //in function,addr already add 1
    }
    if(ret)
    {
        DrvSclOsWaitForCpuWriteToDMem();
        _DrvSclCmdqCheckMiuAddr();
        DrvSclOsClearEventIRQ(DrvSclIrqGetIrqSYNCEventID(),E_DRV_SCLIRQ_EVENT_CMDQ);
        DrvSclOsSetEventIrq(DrvSclIrqGetIrqSYNCEventID(), E_DRV_SCLIRQ_EVENT_CMDQFIRE);
        //_DrvSclCmdqBeTrigger(enIPType,bStart);
        DrvSclCmdqSetRPoint(enIPType);
        SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_NORMAL,"[CMDQ]go CMDQ u32EndAddr:%lx\n",u32EndAddr);
    }
    else
    {
        CMDQERR(sclprintf("[CMDQ]Block Fire for CMDQ FULL!!!\n"));
        if(DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(),
            E_DRV_SCLIRQ_EVENT_CMDQDONE, &u32Events, E_DRV_SCLOS_EVENT_MD_OR, 1000))
        {
            _DrvSclCmdqReSetBufferFull(enIPType);
        }
        else
        {
            _DrvSclCmdqReset(enIPType);
            _DrvSclCmdqReSetBufferFull(enIPType);
            CMDQERR(sclprintf("[CMDQ]Reset CMDQ!!!\n"));
        }
        return FALSE;
    }

    return TRUE;
}
bool _DrvSclCmdqDirectModeFire(DrvSclCmdqIpType_e enIPType,bool bStart)
{
    SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_LOW,"[CMDQ]%s count:%ld\n",__FUNCTION__,gu32LasttimeFramecnt);
    _DrvSclCmdqSetStartPointer(enIPType,gpstCMDQDirinst->u32MIUAddr);
    _DrvSclCmdqSetEndPointer(enIPType,gpstCMDQDirinst->u32MIUAddrEnd);
    DrvSclOsWaitForCpuWriteToDMem();
    DrvSclOsClearEventIRQ(DrvSclIrqGetIrqSYNCEventID(),E_DRV_SCLIRQ_EVENT_CMDQ);
    DrvSclCmdqBeTrigger(enIPType,bStart);
    SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_NORMAL,"[CMDQ]go CMDQ \n");

    return TRUE;
}
u8 _DrvSclCmdqSetBufferConfig(DrvSclCmdqIpType_e enIPType,u32 StartAddr, u32 EndAddr)
{
    u8 ret,ret2;
    ret = _DrvSclCmdqSetStartPointer(enIPType,StartAddr);
    ret2 = _DrvSclCmdqSetEndPointer(enIPType,EndAddr);
    _DrvSclCmdqSetRingTrigPointer(enIPType,StartAddr);
    if((ret==DRV_SCLCMDQ_OK)&&(ret2==DRV_SCLCMDQ_OK))
    return 1;
    else
    return 0;
}
void _DrvSclCmdqInitScTopIrq(void)
{
    HalSclCmdqWriteRegDirect(0x12183C,0xFFFF);//clear sctop irq
    HalSclCmdqWriteRegDirect(0x121838,0xFFFD);//mask
}
u32 _DrvSclCmdqInitByIP(DrvSclCmdqIpType_e enIPType)
{
    char word[] = {"SclCmdqMutex"};
    char word1[] = {"SclCmdq1Mutex"};
    char word2[] = {"SclCmdq2Mutex"};
    HalSclCmdqSetmiusel(enIPType, gstMIUSel[enIPType].u8MIUSel_IP);
    _DrvSclCmdqAddNull(enIPType);//add 2 null
    gstCMDQInfo[enIPType].u16FPoint = gstCMDQInfo[enIPType].u16WPoint + 1;
    HalSclCmdqSetBaseEn(enIPType, TRUE);
    HalSclCmdqSetTrigAlways(enIPType, TRUE);
    HalSclCmdqReset(enIPType, TRUE);
    HalSclCmdqReset(enIPType, FALSE);

    if(FALSE == HalSclCmdqSetDMAMode(enIPType, gu32CMDQMode))
    {
        CMDQERR(sclprintf("[CMDQ]\033[35mCMDQ Set Mode FAIL!!\033[m\n"));
        return DRV_SCLCMDQ_FAIL;
    }

    /* the polling_timer is for re-checking polling value,
        if the time_interval is too small, cmdq will polling RIU frequently, so that RIU will very busy */
    _DrvSclCmdqSetTimerRatio(enIPType, CMDQ_POLL_TIMER, CMDQ_TIMER_RATIO);
    // set time / ratio, total wait time is (wait_time * (ratio + 1)) => ( FF * (F + 1) / 216  MHz ) = sec
    //4D*1=4D~=77 *(216/M) 1 ~=60(frame/s)
    DrvSclCmdqSetSkipPollWhenWaitTimeOut(enIPType, 0);
    _DrvSclCmdqSetBufferConfig(enIPType, gstCMDQInfo[enIPType].PhyAddr,gstCMDQInfo[enIPType].PhyAddrEnd);
    _DrvSclCmdqInitScTopIrq();
    HalSclCmdqResetSoftInterrupt(enIPType);
    HalSclCmdqSetIsrMsk(enIPType,0xE0F8);
    _gCMDQHVSPMUTEX[enIPType] = DrvSclOsCreateMutex(E_DRV_SCLOS_FIFO,
        (enIPType==E_DRV_SCLCMDQ_TYPE_IP0) ? word : (enIPType==E_DRV_SCLCMDQ_TYPE_IP1) ? word1 : word2, SCLOS_PROCESS_SHARED);
    if (_gCMDQHVSPMUTEX[enIPType] == -1)
    {
        CMDQERR(sclprintf("%s: Init MUTEX fail\n", __FUNCTION__));
        return FALSE;
    }
//---------------------------  irq
    return DRV_SCLCMDQ_OK;
}
//function-----------------------------------------------------------------------------
DrvSclCdmqCheckCmdInfo_t _DrvSclCmdqFillCheckCmdInfo(u64 u64Cmd)
{
    DrvSclCdmqCheckCmdInfo_t stCMDQChkinfo;
    stCMDQChkinfo.u32addr = (u32)((u64Cmd &0xFFFFFFFF00000000)>>32);
    stCMDQChkinfo.u16mask = (u16)(u64Cmd &0xFFFF);
    stCMDQChkinfo.u16data = (u16)((u64Cmd>>16) &0xFFFF);
    stCMDQChkinfo.u16StartPoint = gstCMDQInfo[gstCurrworkIP].u16DPoint;
    if(gstCMDQInfo[gstCurrworkIP].u16WPoint>0)
    {
        stCMDQChkinfo.u16EndPoint = gstCMDQInfo[gstCurrworkIP].u16WPoint-1;//for read last time cmd
    }
    else
    {
        stCMDQChkinfo.u16EndPoint = gstCMDQInfo[gstCurrworkIP].u16MaxCmdCnt-1;
    }

    if (stCMDQChkinfo.u16EndPoint+1==stCMDQChkinfo.u16StartPoint)
    {
        stCMDQChkinfo.Ret = 1;
        return stCMDQChkinfo;
    }

    if(stCMDQChkinfo.u16EndPoint+1>=stCMDQChkinfo.u16StartPoint)
    {
        stCMDQChkinfo.u32CmdDiffCnt = stCMDQChkinfo.u16EndPoint+1-stCMDQChkinfo.u16StartPoint;
    }
    else   //ring
    {
        stCMDQChkinfo.u32CmdDiffCnt =
            stCMDQChkinfo.u16EndPoint+1+(gstCMDQInfo[gstCurrworkIP].u16MaxCmdCnt-stCMDQChkinfo.u16StartPoint);
    }

    return stCMDQChkinfo;
}
u16 _DrvSclCmdqGetCurrentCmdPoint(DrvSclCdmqCheckCmdInfo_t stCMDQChkinfo, u32 u32ShiftIdx)
{
    u16 u16CurrentPoint;
    u16 u16Temp;
    if(((stCMDQChkinfo.u16EndPoint+1)>=stCMDQChkinfo.u16StartPoint))
    {
        u16CurrentPoint = (stCMDQChkinfo.u16EndPoint-u32ShiftIdx);
    }
    else if(((stCMDQChkinfo.u16EndPoint+1)<stCMDQChkinfo.u16StartPoint))
    {
        if((stCMDQChkinfo.u16EndPoint)>=u32ShiftIdx)//top
        {
            u16CurrentPoint = (stCMDQChkinfo.u16EndPoint-u32ShiftIdx);
        }
        else//bottom
        {
            u16Temp = u32ShiftIdx-stCMDQChkinfo.u16EndPoint;
            u16CurrentPoint = (gstCMDQInfo[gstCurrworkIP].u16MaxCmdCnt-u16Temp);
        }
    }
    else
    {
        u16CurrentPoint = (stCMDQChkinfo.u16EndPoint-u32ShiftIdx);
    }
    return u16CurrentPoint;
}
bool _DrvSclCmdqCheckingAlreadyExist(DrvSclCdmqCheckCmdInfo_t stCMDQChkinfo)
{
    u64 u64ExistedCmd;
    u16 u16ExistCmdData;
    u32 u32ShiftIdx;
    u16 u16CurrentPoint;
    for(u32ShiftIdx = 0;u32ShiftIdx<stCMDQChkinfo.u32CmdDiffCnt;u32ShiftIdx++)
    {
        //doing
        u16CurrentPoint = _DrvSclCmdqGetCurrentCmdPoint(stCMDQChkinfo,u32ShiftIdx);
        stCMDQChkinfo.pu32Addr = _DrvSclCmdqGetPointerFromPoint(u16CurrentPoint,stCMDQChkinfo.pu32Addr);
        u64ExistedCmd = *stCMDQChkinfo.pu32Addr;
        if(_IsThisCmdAddrAlreadyExist(stCMDQChkinfo.u32addr,u64ExistedCmd))
        {
            //there is same address
            u16ExistCmdData = (u16)((u64ExistedCmd>>16) &0xFFFF);
            u16ExistCmdData = ((u16ExistCmdData&stCMDQChkinfo.u16mask) | (~stCMDQChkinfo.u16mask&stCMDQChkinfo.u16data));
            u64ExistedCmd = ((u64ExistedCmd &0xFFFFFFFF00000000)|((u32)u16ExistCmdData<<16)|0x0000);
            if( _IsIdxLargeThan(u32ShiftIdx,10)) // TO avoid this cmd too far away next cmd.
            {
                _DrvSclCmdqAddCmd(u64ExistedCmd,0);
#if CMDQCMD128
                _DrvSclCmdqAddNull(gstCurrworkIP);
#endif
                *stCMDQChkinfo.pu32Addr = HAL_SCLCMDQ_NULL_CMD;
            }
            else
            {
                *stCMDQChkinfo.pu32Addr = u64ExistedCmd;
            }
            stCMDQChkinfo.Ret = 0;
            SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_HIGH,"[CMDQ]find cmd same address %lx\n"
                ,stCMDQChkinfo.u32addr);
            break;
        }
#if CMDQCMD128
        else if(_IsIdxLargeThan(u32ShiftIdx,CheckingTimes*2))
        {
            stCMDQChkinfo.Ret = 1;
            SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_HIGH,"[CMDQ]check over 70\n");
            break;
        }
#else
        else if(_IsIdxLargeThan(u32ShiftIdx,CheckingTimes))
        {
            stCMDQChkinfo.Ret = 1;
            SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_HIGH,"[CMDQ]check over 35\n");
            break;
        }
#endif
        else
        {
            stCMDQChkinfo.Ret = 1;
        }

    }
    return stCMDQChkinfo.Ret;
}
bool _DrvSclCmdqCheckAddedCmdSameRegPos(u64 u64Cmd,bool bSkipCheckSameAddr)
{
    DrvSclCdmqCheckCmdInfo_t stCMDQChkinfo;
    if (bSkipCheckSameAddr)
    {
        stCMDQChkinfo.Ret = 1;
        SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_HIGH,"[CMDQ]bSkipCheckSameAddr check cmd repeat:%hhx\n"
            ,bSkipCheckSameAddr);
        return stCMDQChkinfo.Ret;
    }
    stCMDQChkinfo = _DrvSclCmdqFillCheckCmdInfo(u64Cmd);
    if(stCMDQChkinfo.Ret)
    {
        return stCMDQChkinfo.Ret;
    }
    SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_HIGH,"[CMDQ]u16EndPoint:%hd u16StartPoint:%hd u32CmdDiffCnt:%ld\n"
        ,stCMDQChkinfo.u16EndPoint,stCMDQChkinfo.u16StartPoint,stCMDQChkinfo.u32CmdDiffCnt);
    //checking
    stCMDQChkinfo.Ret = _DrvSclCmdqCheckingAlreadyExist(stCMDQChkinfo);
    return stCMDQChkinfo.Ret;
}

bool _DrvSclCmdqBufferEmpty(DrvSclCmdqIpType_e eCMDQBuf)
{
    if((gstCMDQInfo[eCMDQBuf].u16WPoint == gstCMDQInfo[eCMDQBuf].u16RPoint) &&
       (gstCMDQInfo[eCMDQBuf].u16WPoint == gstCMDQInfo[eCMDQBuf].u16FPoint))
        return TRUE;
    else
        return FALSE;
}
bool _DrvSclCmdqWriteDirectModeAlreadyCmd(u64 u64Cmd,u16 u16BufferCnt)
{
    bool bRet = TRUE;
    SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_HIGH,
    "[CMDQ]%s u64Cmd:%llx\n",__FUNCTION__,u64Cmd);
    _DrvSclCmdqModifyDirectModeCmd(u64Cmd,u16BufferCnt);
    return bRet;
}
bool _DrvSclCmdqWriteDirectModeNewCmd(u64 u64Cmd)
{
    bool bRet = TRUE;
    SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_HIGH,
    "[CMDQ]%s u64Cmd:%llx\n",__FUNCTION__,u64Cmd);
    _DrvSclCmdqAddDirectModeCmd(u64Cmd);
    return bRet;
}
bool _DrvSclCmdqCpyQueryBufToRingBuf(DrvSclCmdqIpType_e enIPType, u32 u32VirAddr)
{
    u64 *pu32Addr = NULL;
    pu32Addr = _DrvSclCmdqGetPointerFromPoint(gstCMDQInfo[gstCurrworkIP].u16WPoint,pu32Addr);
    DrvSclOsMemcpy((void *)pu32Addr,(void *)u32VirAddr,(CMDQ_DIRECTBUFFER_CMDQCMD*HAL_SCLCMDQ_CMD_LEN));
    gstCMDQInfo[gstCurrworkIP].u16WPoint+=CMDQ_DIRECTBUFFER_CMDQCMD;
    if(gstCMDQInfo[gstCurrworkIP].u16WPoint>=(CMDQ_DIRECTBUFFER_CMDQCMD*CMDQ_DIRECTBUFFER_CMDQBLOCK))
    {
        gstCMDQInfo[gstCurrworkIP].u16WPoint = 0;
    }
    return 1;
}

bool _DrvSclCmdqWriteCmd(u64 u64Cmd,bool bSkipCheckSameAddr)
{
    bool bRet = TRUE;
    u16 u16DummyCmdIdx;
    DrvSclCmdqIpType_e enCMDQIP = gstCurrworkIP;
    CMDQDBG(sclprintf("[CMDQ] u64Cmd:%llx\n",u64Cmd));
    if( _DrvSclCmdqBufferEmpty(enCMDQIP) )
    {
        CMDQERR(sclprintf("[CMDQ] buffer empty init\n"));
        gstCMDQInfo[enCMDQIP].u16RPoint = 0;
        gstCMDQInfo[enCMDQIP].u16WPoint = 0;
        gstCMDQInfo[enCMDQIP].u16FPoint = 0;
        _DrvSclCmdqAddNull(gstCurrworkIP);
        _DrvSclCmdqAddCmd(u64Cmd,bSkipCheckSameAddr);
        bRet = TRUE;
    }
    else
    {

        u16DummyCmdIdx=(gstCMDQInfo[enCMDQIP].u16WPoint/2+1)*2;
        if(u16DummyCmdIdx < gstCMDQInfo[enCMDQIP].u16MaxCmdCnt)//med
        {
            if(_DrvSclCmdqCheckAddedCmdSameRegPos(u64Cmd,bSkipCheckSameAddr))
            {
                _DrvSclCmdqAddCmd(u64Cmd,bSkipCheckSameAddr);
            #if CMDQCMD128
                _DrvSclCmdqAddNull(gstCurrworkIP);
            #endif
            }
            bRet = TRUE;
        }
        else if(u16DummyCmdIdx == gstCMDQInfo[enCMDQIP].u16MaxCmdCnt)//last
        {
            if(gstCMDQInfo[enCMDQIP].u16WPoint+1 < gstCMDQInfo[enCMDQIP].u16MaxCmdCnt)//last+2
            {
                if(_DrvSclCmdqCheckAddedCmdSameRegPos(u64Cmd,bSkipCheckSameAddr))
                {
                    _DrvSclCmdqAddCmd(u64Cmd,bSkipCheckSameAddr);
                #if CMDQCMD128
                    _DrvSclCmdqAddNull(gstCurrworkIP);
                #endif
                    SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_NORMAL,
                    "[CMDQ]FC last+2 u16MaxCmdCnt=%d u16WPoint=%d\n"
                        ,gstCMDQInfo[enCMDQIP].u16MaxCmdCnt,gstCMDQInfo[enCMDQIP].u16WPoint);
                }
                bRet = TRUE;
            }
            else if(gstCMDQInfo[enCMDQIP].u16WPoint+1 == gstCMDQInfo[enCMDQIP].u16MaxCmdCnt)//last+1
            {
                if(_DrvSclCmdqCheckAddedCmdSameRegPos(u64Cmd,bSkipCheckSameAddr))
                {
                    _DrvSclCmdqAddCmd(u64Cmd,bSkipCheckSameAddr);
                    SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_NORMAL,
                        "[CMDQ]FC last+1 u16MaxCmdCnt=%d u16WPoint=%d\n"
                        ,gstCMDQInfo[enCMDQIP].u16MaxCmdCnt,gstCMDQInfo[enCMDQIP].u16WPoint);
                }
                _DrvSclCmdqResetCmdPointIfBottom(enCMDQIP);
                bRet = TRUE;
            }

        }
        else if(u16DummyCmdIdx > gstCMDQInfo[enCMDQIP].u16MaxCmdCnt)//last
        {
            _DrvSclCmdqResetCmdPointIfBottom(enCMDQIP);
            if(_DrvSclCmdqCheckAddedCmdSameRegPos(u64Cmd,bSkipCheckSameAddr))
            {
                SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_NORMAL,"[CMDQ] frame cnt buffer full -->reset\n");
                _DrvSclCmdqAddCmd(u64Cmd,bSkipCheckSameAddr);
            #if CMDQCMD128
                _DrvSclCmdqAddNull(gstCurrworkIP);
            #endif
                SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_NORMAL,"[CMDQ]FC last u16MaxCmdCnt=%d u16WPoint=%d\n"
                    ,gstCMDQInfo[enCMDQIP].u16MaxCmdCnt,gstCMDQInfo[enCMDQIP].u16WPoint);
            }
            bRet = TRUE;
        }
        else
        {
            bRet = FALSE;
        }


    }
    return bRet;
}
u64 _DrvSclCmdqGetCmd(u8 u8type,u32 u32Addr, u16 u16Data, u16 u16Mask)
{
    u16 u16Bank;
    u8 u8addr;
    DrvSclCmdqCmd_t data;
    u16Bank = (u16)((u32Addr >> 8) & 0xFFFF);
    u8addr = (u8)((u32Addr & 0xFF) );
    if(u8addr%2)  //Hbyte
    {
        data.u16Mask=~(u16Mask<<8);
        data.u16Data = (HalSclCmdqRead2ByteReg(u32Addr-1) & ~(u16Mask<<8)) | ((u16Data & u16Mask)<<8);
    }
    else
    {
        data.u16Mask=~(u16Mask);
        if( u16Mask == 0xFFFF )
        {
            data.u16Data = u16Data;
        }
        else
        {
            data.u16Data = (HalSclCmdqRead2ByteReg(u32Addr) & ~u16Mask) | (u16Data & u16Mask);
        }
    }

    data.u8type = u8type;
    data.u32Addr =((((u32)u16Bank<<4)*8)|((u32)u8addr/2));
    data.u64Cmd = _CMDQGetCMD(data.u8type,data.u32Addr,data.u16Data,data.u16Mask);
    return data.u64Cmd;
}
void _DrvSclCmdqWriteSramCmd(DrvSclCmdqIpType_e enIPType, DrvSclCmdqCmdReg_t stCfg)
{
    u64 u64CMD;
    u64CMD = _DrvSclCmdqGetCmd(CMDQ_CMDTYPE_WRITE,stCfg.u32Addr,stCfg.u16Data,stCfg.u16Mask);
    _DrvSclCmdqAddSramCmd(u64CMD);
}
void _DrvSclCmdqPutCmd(u8 u8type,DrvSclCmdqCmdReg_t stCfg,bool bSkipCheckSameAddr)
{
    u64 u64CMD;
    u16 u16BufferCnt;
    bool bNewBufferCnt;
    u64CMD = _DrvSclCmdqGetCmd(u8type,stCfg.u32Addr,stCfg.u16Data,stCfg.u16Mask);
    if(gu32CMDQMode & E_DRV_SCLCMDQ_DIRECT_MODE)
    {
         bNewBufferCnt = HalSclCmdqGetBufferCmdCnt(&u16BufferCnt,stCfg.u32Addr);
         if(bNewBufferCnt)
         {
            if(u8type == CMDQ_CMDTYPE_WRITE)
            {
                HalSclCmdqSetBufferCmdCnt(stCfg.u32Addr,gpstCMDQDirinst->u16WPoint);
            }
            _DrvSclCmdqWriteDirectModeNewCmd(u64CMD);//cmd:real cmd
         }
         else
         {
             _DrvSclCmdqWriteDirectModeAlreadyCmd(u64CMD,u16BufferCnt);//cmd:real cmd
         }
    }
    else
    {
        _DrvSclCmdqWriteCmd(u64CMD,bSkipCheckSameAddr);//cmd:real cmd
    }
}
void _DrvSclCmdqAddWaitLdcTrigCmd(void)
{
    #if 0
    _DrvSclCmdqAddNull(E_DRV_SCLCMDQ_TYPE_IP0);
    _DrvSclCmdqPutCmd(CMDQ_CMDTYPE_WRITE,REG_SCL0_1E_L,0xFFFD,0xFFFF,1);
    DrvSclCmdqWaitCmd(SC2CMDQ_LV_TRIG);//wait
    _DrvSclCmdqAddNull(E_DRV_SCLCMDQ_TYPE_IP0);
    _DrvSclCmdqPutCmd(CMDQ_CMDTYPE_WRITE,REG_SCL0_1E_L,0xFFFF,0xFFFF,1);
    #endif
}
//--------------------------------------------------------------------------------------------------
// In ring-buffer mode, this function will trig for update reg_sw_wr_mi_wadr
// The CMDQ will keep on executing cmd until reg_rd_mi_radr reach reg_sw_wr_mi_wadr
//--------------------------------------------------------------------------------------------------

//API--------------------------------------------------------------------------------------------------
void DrvSclCmdqWriteRegWithMaskDirect(u32 u32Addr,u16 u16Data,u16 u16Mask)
{
    HalSclCmdqWriteRegMaskDirect(u32Addr,u16Data,u16Mask);
}

u64 DrvSclCmdqGetCMDFromPoint(DrvSclCmdqIpType_e enIPType ,u16 u16Point)
{
    u64 *pu32Addr = NULL;
    CMDQ_MUTEX_LOCK();
    gstCurrworkIP = enIPType;
    pu32Addr = _DrvSclCmdqGetPointerFromPoint((u16Point),pu32Addr);
    CMDQ_MUTEX_UNLOCK();
    return (u64)(*pu32Addr);
}
u32 DrvSclCmdqGetCMDBankFromCMD(u64 u64Cmd)
{
    u32 u32BankAddr;
    u16 u16Addr;
    u32 u32Bank;
    u32BankAddr = (u32)((u64Cmd &0x00FFFFFF00000000 )>>32) ;
    if((u32BankAddr&0x0000FF)>=0x80) // odd bank
    {
        u16Addr = (u16)((u32BankAddr&0x0000FF)-0x80);
    }
    else // even bank
    {
        u16Addr = (u16)(u32BankAddr&0x0000FF);
    }
    u32Bank = (u32)(((u32BankAddr - u16Addr))*2) ;
    u32BankAddr = (u32)(u32Bank + u16Addr);

    return u32BankAddr;
}

u8 DrvSclCmdqGetISPHWCnt(void)
{
    u16 isp_cnt;
    isp_cnt = HalSclCmdqGetIspCnt();
    isp_cnt = ((isp_cnt>>8)&0x7F);
    return (u8)isp_cnt;

}

void DrvSclCmdqSetSkipPollWhenWaitTimeOut(DrvSclCmdqIpType_e enIPType ,u16 bEn)
{
    HalSclCmdqSetSkipPollWhenWaitTimeout(enIPType,bEn);
    HalSclCmdqSetTimeoutAmount(enIPType);
}

void DrvSclCmdqEnable(bool bEnable,DrvSclCmdqIpType_e enIPType)
{
    CMDQ_MUTEX_LOCK();
    if(bEnable)
    {
        gstCMDQInfo[enIPType].bEnable++;
    }
    else
    {
         if(gstCMDQInfo[enIPType].bEnable)
         {
             gstCMDQInfo[enIPType].bEnable--;
         }
    }
    CMDQ_MUTEX_UNLOCK();
    SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_LOW, "[CMDQ]%s enable:%hhd\n"
        ,__FUNCTION__,gstCMDQInfo[enIPType].bEnable);
}
void DrvSclCmdqSetInst(DrvSclCmdqCtxConfig_t stInst)
{
    CMDQ_MUTEX_LOCK();
    if(gpstCMDQDirinst)
    {
        gpstCMDQDirinst->u16WPoint = 0;
        gpstCMDQDirinst->u32VirAddr = (u32)stInst.pCmdqVirAddr;
        gpstCMDQDirinst->stSramVir.pvVirAddr = stInst.pSramVirAddr;
        gpstCMDQDirinst->stSramVir.u16WPoint= 0;
        gpstCMDQDirinst->u16MaxCmdCnt = CMDQ_DIRECTBUFFER_CMDQCMD;
    }
    CMDQ_MUTEX_UNLOCK();

}
void DrvSclCmdqAllocDirectInst(void)
{
    CMDQ_MUTEX_LOCK();
    if(!gpstCMDQDirinst)
    {
        gpstCMDQDirinst =
            (DrvSclCmdqDirectmodeinst_t *)DrvSclOsVirMemalloc(sizeof(DrvSclCmdqDirectmodeinst_t));
    }
    CMDQ_MUTEX_UNLOCK();
}
void DrvSclCmdqAllocDefaultInst(void)
{
    CMDQ_MUTEX_LOCK();
    if(!gpstCMDQDefaultinst)
    {
        gpstCMDQDefaultinst =
            (DrvSclCmdqDirectmodeinst_t *)DrvSclOsVirMemalloc(sizeof(DrvSclCmdqDirectmodeinst_t));
    }
    gpstCMDQDefaultinst->u16WPoint = 0;
    gpstCMDQDefaultinst->u32VirAddr = (u32)DrvSclOsVirMemalloc(CMDQ_DIRECTBUFFER_CMDQCMD *HAL_SCLCMDQ_CMD_LEN);
    gpstCMDQDefaultinst->stSramVir.pvVirAddr = DrvSclOsVirMemalloc(CMDQ_DIRECTBUFFER_CMDQCMD *HAL_SCLCMDQ_CMD_LEN);
    gpstCMDQDefaultinst->stSramVir.u16WPoint = 0;
    CMDQ_MUTEX_UNLOCK();
}
void DrvSclCmdqFreeDirectInst(void)
{
    CMDQ_MUTEX_LOCK();
    if(gpstCMDQDirinst)
    {
        DrvSclOsVirMemFree((void *)gpstCMDQDirinst);
        gpstCMDQDirinst = NULL;
    }
    if(gpstCMDQDefaultinst)
    {
        DrvSclOsVirMemFree((void *)gpstCMDQDefaultinst->stSramVir.pvVirAddr);
        DrvSclOsVirMemFree((void *)gpstCMDQDefaultinst->u32VirAddr);
        DrvSclOsVirMemFree((void *)gpstCMDQDirinst);
        gpstCMDQDirinst = NULL;
    }
    CMDQ_MUTEX_UNLOCK();
}
void DrvSclCmdqInit(DrvSclCmdqOpen_t stCMDQIniCfg)
{
    u32 IP0PhyAddr;
    u32 u32CMDQBufSize;
    u32 u32IP0VirAddr;
    u8 i;
    DrvSclCmdqCtxConfig_t stCfg;
    HalSclCmdqInitRiuBase(stCMDQIniCfg.u32RiuBase);
    CMDQ_MUTEX_CREATE();
    gpstCMDQDirinst = NULL;
    gpstCMDQDefaultinst = NULL;
    DrvSclCmdqAllocDirectInst();
    DrvSclCmdqAllocDefaultInst();
    stCfg.pCmdqVirAddr = (void*)gpstCMDQDefaultinst->u32VirAddr;
    stCfg.pSramVirAddr = gpstCMDQDefaultinst->stSramVir.pvVirAddr;
    DrvSclCmdqSetInst(stCfg);
    for(i=E_DRV_SCLCMDQ_TYPE_IP0 ;i<E_DRV_SCLCMDQ_TYPE_MAX;i++)
    {
        if(stCMDQIniCfg.u32CMDQ_Phy[i] && stCMDQIniCfg.u32CMDQ_Vir[i]&& stCMDQIniCfg.u32CMDQ_Size[i])
        {
            u32CMDQBufSize = stCMDQIniCfg.u32CMDQ_Size[i] / CMDQ_NUMBER;//  0x4000/2
            IP0PhyAddr = stCMDQIniCfg.u32CMDQ_Phy[i];
            u32IP0VirAddr = stCMDQIniCfg.u32CMDQ_Vir[i];
            DrvSclOsMemset((void *)stCMDQIniCfg.u32CMDQ_Vir[i], 0x00, stCMDQIniCfg.u32CMDQ_Size[i]);
            CMDQ_MUTEX_LOCK();
            _DrvSclCmdqInit(IP0PhyAddr);
            _DrvSclCmdqFillInitIpInfo(i, IP0PhyAddr,
                u32IP0VirAddr, u32CMDQBufSize);
            _DrvSclCmdqInitByIP(i);
            CMDQ_MUTEX_UNLOCK();
            SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_LOW, "[CMDQ]init %s: 0x%lx,0x%lx, 0x%lx\n"
                , __FUNCTION__, stCMDQIniCfg.u32CMDQ_Phy[i],stCMDQIniCfg.u32CMDQ_Vir[i], stCMDQIniCfg.u32CMDQ_Size[i]);
        }
    }

}
void DrvSclCmdqDelete(DrvSclCmdqIpType_e enIPType)
{
    HalSclCmdqClearIrqByFlag(enIPType,0x0800);   //clear idle
    while(1)
    {
        if(_IsCMDQIPIdle(enIPType))                // wait idle
        {
            break;
        }
    }
    DrvSclCmdqFreeDirectInst();
    HalSclCmdqSetIsrMsk(enIPType,0xFFFF);
    HalSclCmdqResetSoftInterrupt(enIPType);
    CMDQ_MUTEX_DELETE();
    if (_gCMDQHVSPMUTEX[enIPType] != -1)
    {
        DrvSclOsDeleteMutex(_gCMDQHVSPMUTEX[enIPType]);
    }
}
void DrvSclCmdqrelease(DrvSclCmdqIpType_e enIPType)
{
    int u32Time;
    if(gstCMDQInfo[enIPType].bEnable == 1 || gstCMDQInfo[enIPType].bEnable == 0)
    {
        u32Time = DrvSclOsGetSystemTime();
        DrvSclCmdqBeTrigger(enIPType,1);
        DrvSclCmdqSetSkipPollWhenWaitTimeOut(enIPType,1);
        while(1)
        {
            if(DrvSclCmdqCheckIPAlreadyDone(enIPType))
            {
                DrvSclCmdqSetSkipPollWhenWaitTimeOut(enIPType,0);
                break;
            }
            else if(DrvSclOsTimerDiffTimeFromNow(u32Time)>1000)
            {
                DrvSclCmdqSetSkipPollWhenWaitTimeOut(enIPType,0);
                break;
            }
        }
        _DrvSclCmdqReset(enIPType);
        SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_LOW, "[CMDQ]%s enable:%hhd\n"
            ,__FUNCTION__,gstCMDQInfo[0].bEnable);
    }
    else
    {
        DrvSclCmdqEnable(0,0);
    }
}
bool DrvSclCmdqWrite(DrvSclCmdqIpType_e enIPType,u32 u32Addr, u16 u16Data, u16 u16Mask)
{
    DrvSclCmdqCmdReg_t stCfg;
    DrvSclCmdqFillCmd(&stCfg, u32Addr, u16Data, u16Mask);
    return DrvSclCmdqWriteCmd(E_DRV_SCLCMDQ_TYPE_IP0, stCfg, 0);
}
void DrvSclCmdqFillCmd(DrvSclCmdqCmdReg_t *stCfg,u32 u32Addr, u16 u16Data, u16 u16Mask)
{
    stCfg->u16Data = u16Data;
    stCfg->u32Addr = u32Addr;
    stCfg->u16Mask = u16Mask;
}
void DrvSclCmdqSramReset(void)
{
    CMDQ_MUTEX_LOCK();
    DrvSclOsMemset(gpstCMDQDirinst->stSramVir.pvVirAddr,0,(CMDQ_DIRECTBUFFER_CMDQCMD*HAL_SCLCMDQ_CMD_LEN));
    gpstCMDQDirinst->stSramVir.u16WPoint = 0;
    CMDQ_MUTEX_UNLOCK();
    SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_LOW,"[CMDQ]%s\n",__FUNCTION__);
}
bool DrvSclCmdqWriteSramCmd(DrvSclCmdqIpType_e enIPType, DrvSclCmdqCmdReg_t stCfg, bool bSkipCheckSameAddr)
{
    bool bRet=1;
    if(_gs32CMDQMutex == -1 || gstCMDQInfo[enIPType].bEnable==0)
    {
        return 0;
    }
    SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_NORMAL,"[CMDQ]%s\n",__FUNCTION__);
    CMDQ_MUTEX_LOCK();
    gstCurrworkIP = enIPType;
    SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_NORMAL,"[CMDQ]Write: %04lx %04x %04x flag: %hhx\n"
        ,stCfg.u32Addr, stCfg.u16Data, stCfg.u16Mask,gCMDQStatusFlag[enIPType]);
    if(gu32CMDQMode ==E_DRV_SCLCMDQ_DIRECT_BUFFER_RINGMODE)
    {
        _DrvSclCmdqWriteSramCmd(enIPType,stCfg);
        CMDQ_MUTEX_UNLOCK();
    }
    else
    {
        CMDQ_MUTEX_UNLOCK();
        DrvSclCmdqWriteCmd(enIPType, stCfg, 1);
    }
    return bRet;
}

//u32Addr :8bit addr
bool DrvSclCmdqWriteCmd(DrvSclCmdqIpType_e enIPType, DrvSclCmdqCmdReg_t stCfg, bool bSkipCheckSameAddr)
{
    bool bRet=1;
    if(_gs32CMDQMutex == -1 || gstCMDQInfo[enIPType].bEnable==0)
    {
        return 0;
    }
    SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_NORMAL,"[CMDQ]%s\n",__FUNCTION__);
    CMDQ_MUTEX_LOCK();
    gstCurrworkIP = enIPType;
    SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_NORMAL,"[CMDQ]Write: %04lx %04x %04x flag: %hhx\n"
        ,stCfg.u32Addr, stCfg.u16Data, stCfg.u16Mask,gCMDQStatusFlag[enIPType]);
    #if USE_Utility
	HalSclCmdqWriteRegMaskBuffer(stCfg.u32Addr,stCfg.u16Data, stCfg.u16Mask);
    _DrvSclCmdqPutCmd(CMDQ_CMDTYPE_WRITE,stCfg,NotToCheckSameAddr);
    #else
    _DrvSclCmdqPutCmd(CMDQ_CMDTYPE_WRITE,stCfg,bSkipCheckSameAddr);
    #endif
    CMDQ_MUTEX_UNLOCK();
    return bRet;
}
DrvSclCmdqInfo_t DrvSclCmdqGetCMDQInformation(DrvSclCmdqIpType_e enIPType)
{
    return gstCMDQInfo[enIPType];
}
void DrvSclCmdqGetModuleMutex(DrvSclCmdqIpType_e enIPType,bool bEn)
{
    static bool block = 0;
    if(bEn)
    {
        DrvSclOsObtainMutex(_gCMDQHVSPMUTEX[enIPType],SCLOS_WAIT_FOREVER);
        block = 1;
        SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_NORMAL,"[CMDQ]IN Mutex\n");
    }
    else if(block && !bEn)
    {
        block = 0;
        DrvSclOsReleaseMutex(_gCMDQHVSPMUTEX[enIPType]);
        SCL_DBG(SCL_DBG_LV_DRVCMDQ()&EN_DBGMG_CMDQEVEL_NORMAL,"[CMDQ]Out Mutex\n");
    }
    else//(!block && !bEn)
    {
    }
}
void DrvSclCmdqInitRIUBase(u32 u32RIUBase)
{
    HalSclCmdqInitRiuBase(u32RIUBase);
}
u16 DrvSclCmdqGetFinalIrq(DrvSclCmdqIpType_e enIPType)
{
    gu16ISRflag = HalSclCmdqGetFinalIrq(enIPType, 0xFFFF);
    return gu16ISRflag;
}
void DrvSclCmdqClearIrqByFlag(DrvSclCmdqIpType_e enIPType,u16 u16Irq)
{
    HalSclCmdqClearIrqByFlag(enIPType,u16Irq);
}
void DrvSclCmdqSetISRStatus(bool bEn)
{
    gbISRopen = bEn;
}
//-------------------------------------------------------------------------------------------------
/// Fire the commands
/// @return  TRUE if succeed, FALSE if failed
//-------------------------------------------------------------------------------------------------
bool DrvSclCmdqFireSram(DrvSclCmdqIpType_e enIPType,bool bStart)
{
    //1. prepare buffer
    //2. memcpy to Ring buffer
    //3. fire ring
    if(_gs32CMDQMutex == -1  || gstCMDQInfo[enIPType].bEnable==0)
    {
        SCL_DBGERR("[CMDQ]%s can't use\n",__FUNCTION__);
        return 0;
    }
    if(gu32CMDQMode == E_DRV_SCLCMDQ_RING_BUFFER_MODE)
    {
        DrvSclCmdqFire(enIPType,1);
    }
    else
    {
        CMDQ_MUTEX_LOCK();
        gstCurrworkIP = enIPType;
        _DrvSclCmdqCpyQueryBufToRingBuf(enIPType,(u32)gpstCMDQDirinst->stSramVir.pvVirAddr);
        //_DrvSclCmdqDirectBufferRingModeFire(enIPType,1);

        CMDQ_MUTEX_UNLOCK();
    }
    return TRUE;
}

void DrvSclCmdqFireFirstTime(void)
{
    u64 nullcmd = HAL_SCLCMDQ_NULL_CMD;
    u64 *pu32Addr = NULL;
    bool i ;
    for(i = 0 ;i<10; i++)
    {
        pu32Addr = _DrvSclCmdqGetDirectModePointerFromPoint(i,pu32Addr);
        if(*pu32Addr&HAL_SCLCMDQ_WAIT_CMD)
        {
            *pu32Addr = nullcmd;
            break;
        }
    }
}
bool DrvSclCmdqFireDirectBufferRingMode(DrvSclCmdqIpType_e enIPType,bool bStart)
{
    //1. prepare buffer
    //2. memcpy to Ring buffer
    //3. fire ring
    if(_gs32CMDQMutex == -1  || gstCMDQInfo[enIPType].bEnable==0)
    {
        SCL_DBGERR("[CMDQ]%s can't use\n",__FUNCTION__);
        return 0;
    }
    CMDQ_MUTEX_LOCK();
    gstCurrworkIP = enIPType;
    if(gu32CMDQMode == E_DRV_SCLCMDQ_DIRECT_BUFFER_RINGMODE)
    {
        if(gpstCMDQDirinst->u16WPoint%2)
        {
            _DrvSclCmdqAddDirectModeNull();
        }
        _DrvSclCmdqCpyQueryBufToRingBuf(enIPType,gpstCMDQDirinst->u32VirAddr);
        _SetCMDQFlagType(enIPType,E_DRV_SCLCMDQ_FLAG_FIRE);
        DrvSclCmdqFireSram(enIPType,bStart);
        _DrvSclCmdqDirectBufferRingModeFire(enIPType,1);

    }
    CMDQ_MUTEX_UNLOCK();
    return TRUE;
}
bool DrvSclCmdqFire(DrvSclCmdqIpType_e enIPType,bool bStart)
{
    if(_gs32CMDQMutex == -1  || gstCMDQInfo[enIPType].bEnable==0)
    {
        SCL_DBGERR("[CMDQ]%s can't use\n",__FUNCTION__);
        return 0;
    }
    CMDQ_MUTEX_LOCK();
    gstCurrworkIP = enIPType;
#if (CMDQCMD128==0)
    if(gu32CMDQMode == E_DRV_SCLCMDQ_RING_BUFFER_MODE && gstCMDQInfo[enIPType].u16WPoint%2)
    {
        _DrvSclCmdqAddNull(enIPType);
    }
    else if(gu32CMDQMode == E_DRV_SCLCMDQ_DIRECT_MODE && gpstCMDQDirinst->u16WPoint%2)
    {
        _DrvSclCmdqAddDirectModeNull();
    }
    else if(gu32CMDQMode == E_DRV_SCLCMDQ_DIRECT_BUFFER_RINGMODE && gpstCMDQDirinst->u16WPoint%2)
    {
    }
#endif
    _SetCMDQFlagType(enIPType,E_DRV_SCLCMDQ_FLAG_FIRE);
    if(gu32CMDQMode == E_DRV_SCLCMDQ_RING_BUFFER_MODE)
    {
        _DrvSclCmdqFire(enIPType,1);
    }
    else if(gu32CMDQMode == E_DRV_SCLCMDQ_DIRECT_MODE)
    {
        _DrvSclCmdqDirectModeFire(enIPType,1);
    }
    else if(gu32CMDQMode == E_DRV_SCLCMDQ_DIRECT_BUFFER_RINGMODE)
    {
    }
    CMDQ_MUTEX_UNLOCK();

    return TRUE;

}
void DrvSclCmdqSetEventForFire(void)
{
    DrvSclOsSetEventIrq(DrvSclIrqGetIrqSYNCEventID(),  E_DRV_SCLIRQ_EVENT_VIP);
}
void DrvSclCmdqWaitCmd(DrvSclCmdqIpType_e enIPType,u16 u16bus)
{
    u64 u64Cmd;
    CMDQ_MUTEX_LOCK();
    gstCurrworkIP = enIPType;
    u64Cmd = HAL_SCLCMDQ_WAIT_CMD + u16bus;
    if(gu32CMDQMode & E_DRV_SCLCMDQ_DIRECT_MODE)
    {
         _DrvSclCmdqWriteDirectModeNewCmd(u64Cmd);//cmd:real cmd
    }
    else
    {
        _DrvSclCmdqWriteCmd(u64Cmd,NotToCheckSameAddr);//cmd:real cmd
    }
    CMDQ_MUTEX_UNLOCK();
}

void DrvSclCmdqPollingEqCmd(DrvSclCmdqIpType_e enIPType,DrvSclCmdqCmdReg_t stCfg)
{
    CMDQ_MUTEX_LOCK();
    gstCurrworkIP = enIPType;
    _DrvSclCmdqPutCmd(CMDQ_CMDTYPE_POLLEQ,stCfg,NotToCheckSameAddr);
    CMDQ_MUTEX_UNLOCK();
}

void DrvSclCmdqPollingNeqCmd(DrvSclCmdqIpType_e enIPType,DrvSclCmdqCmdReg_t stCfg)
{
    CMDQ_MUTEX_LOCK();
    gstCurrworkIP = enIPType;
    _DrvSclCmdqPutCmd(CMDQ_CMDTYPE_POLLNEQ,stCfg,NotToCheckSameAddr);
    CMDQ_MUTEX_UNLOCK();
}
void DrvSclCmdqCheckVIPSRAM(u32 u32Type)
{
    DrvSclVipCheckVipSram(u32Type);
}
bool DrvSclCmdqCheckIPAlreadyDone(DrvSclCmdqIpType_e enIPType)
{
    bool bEn;
    if(_IsCMDQDMADone(enIPType)) //cmdq dma done
    {
        bEn = TRUE;
        HalSclCmdqClearIrqByFlag(enIPType,0x0003);
    }
    else if(gbISRopen)
    {
        if(_IsCMDQExecuteDoneISR(enIPType))
        {
            bEn = TRUE;
        }
        else if(DrvSclOsGetEvent(DrvSclIrqGetIrqSYNCEventID())&E_DRV_SCLIRQ_EVENT_CMDQDONE)
        {
            bEn = TRUE;
        }
        else
        {
            bEn = FALSE;
        }
    }
    else if(_IsCMDQExecuteDone(enIPType))
    {
        bEn = TRUE;
        HalSclCmdqClearIrqByFlag(enIPType,0x0003);
    }
    else
    {
        bEn = FALSE;
    }
    return bEn;
}
