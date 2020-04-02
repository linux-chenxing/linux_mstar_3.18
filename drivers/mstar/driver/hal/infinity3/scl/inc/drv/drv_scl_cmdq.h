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
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file   drvCMDQ.h
/// @brief  CMDQ Driver Interface
/// @author MStar Semiconductor,Inc.
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __DRV_SCL_CMDQ_H__
#define __DRV_SCL_CMDQ_H__


//--------------------------------------------------------------------------------------------------
//  Define
//--------------------------------------------------------------------------------------------------
#define CMDQ_DIRECT_MODE                 0x01    // direct mode :nonuse
#define CMDQ_INCREMENT_MODE              0x00    // increment mode :nonuse
#define CMDQ_RING_BUFFER_MODE            0x04    // ring mode
#define CMDQ_DBUF                   0       // whether double buffer
#define CMDQ_IRQ                    0       // isr open
#define CMDQ_NONUSE_FUNCTION        0       // mark no use function
#define CMDQ_POLL_TIMER             0x4D    // poll timer
#define CMDQ_TIMER_RATIO            0x0     // time ratio
#define CMDQ_BASE_AMOUNT            0x1FFFF // wait count amount
#define CMDQ_TIMEOUT_AMOUNT         0x40    // wait timeout amount
#define CMDQ_NUMBER                 1       // number of CMDQ ip
#define CMDQ_ADD_DELAYFRAME_SCLSELF 0       // when=1 is delay 2 frame ,when =0 is delay 2frame but input isp count add 1 itself.
#define CMDQ_ALLOW_ERROR_COUNT      1       // allow overpass count
#define CMDQ_IRQ_STATE_TRIG         (1 << 0)
#define CMDQ_IRQ_STATE_DONE         (1 << 2)
#define CMDQ_IRQ_WAIT_TIMEOUT       (1 << 10)
#define CMDQ_CMDTYPE_WRITE 0x10
#define CMDQ_CMDTYPE_WAIT 0x20
#define CMDQ_CMDTYPE_POLLEQ 0x30
#define CMDQ_CMDTYPE_POLLNEQ 0xb0
#define CMDQ_INST_MAX 32
#define CMDQ_DIRECTBUFFER_CMDQCMD 1500  // 3000
#define CMDQ_DIRECTBUFFER_CMDQBLOCK 20   //45

//--------------------------------------------------------------------------------------------------
//  Driver Capability
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//  Local variable
//-----------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
//  Type and Structure
//--------------------------------------------------------------------------------------------------
typedef enum
{
    E_DRV_SCLCMDQ_TYPE_IP0 ,
    E_DRV_SCLCMDQ_TYPE_IP1 ,
    E_DRV_SCLCMDQ_TYPE_IP2 ,
    E_DRV_SCLCMDQ_TYPE_MAX,
}DrvSclCmdqIpType_e;
typedef enum
{
    E_DRV_SCLCMDQ_INCREMENT_MODE = 0,
    E_DRV_SCLCMDQ_DIRECT_MODE  = 0x1,
    E_DRV_SCLCMDQ_RING_BUFFER_MODE = 0x4 ,
    E_DRV_SCLCMDQ_DIRECT_BUFFER_RINGMODE = 0x5 ,
}DrvSclCmdqIpMode_e;

typedef enum
{
    E_DRV_SCLCMDQ_MIU_0 ,
    E_DRV_SCLCMDQ_MIU_1 ,
}DrvSclCmdqMiuType_e;

typedef enum
{
    E_DRV_SCLCMDQ_FLAG_FIRE              = 0x1,  // whether already fire or need to fire
}DrvSclCmdqFlagType_e;
typedef struct
{
    u16  u16WPoint;           // current cmd end
    u16  u16RPoint;           // last trig
    u16  u16FPoint;           // fire point (128 bit/unit)
    u32  PhyAddr;         // by Chip_Phys_to_MIU
    u16  u16MaxCmdCnt;        // assframe buf count
    bool bEnable;            // CMDQ enable ((nonuse
    u32  u32VirAddr;          // kernel virtul after allocate
    u32  PhyAddrEnd;      // by Chip_Phys_to_MIU+cnt
    u16  u16LPoint;           // To get trig Count
    u16  u16DPoint;           //  last done
}DrvSclCmdqInfo_t;
typedef struct
{
    u16  u16WPoint;           // current cmd end
    void  *pvVirAddr;          // kernel virtul after allocate
}DrvSclCmdqSram_t;
typedef struct
{
    u16  u16WPoint;           // current cmd end
    u32  u32MIUAddr;         // by Chip_Phys_to_MIU
    u32  u32VirAddr;          // kernel virtul after allocate
    u16  u16MaxCmdCnt;        // assframe buf count
    u32  u32MIUAddrEnd;      // by Chip_Phys_to_MIU+cnt
    DrvSclCmdqSram_t  stSramVir;          // kernel virtul after allocate
}DrvSclCmdqDirectmodeinst_t;
typedef struct
{
    u16 u16InstIdx;            // multiinst id
    u32  u32MIUAddr;          // kernel virtul after allocate
    u32  u32MIUAddrEnd;          // kernel virtul after allocate
    u32  u32VirAddr;          // kernel virtul after allocate
    DrvSclCmdqSram_t  stSramVir;          // kernel virtul after allocate
}DrvSclCmdqMultiinst_t;

typedef struct
{
    u8 u8MIUSel_IP;
}DrvSclCmdqBufferMiu_t;
typedef struct
{
    u16  u16StartPoint;
    u16  u16EndPoint;
    u32  u32CmdDiffCnt;
    u32  u32ActualCmdDiffCnt;
    bool Ret;
    u32  u32addr;
    u16  u16mask;
    u16  u16data;
    u64  *pu32Addr;
}DrvSclCdmqCheckCmdInfo_t;

typedef struct
{
   u8  u8type;    // CMD type(write,wait,poll)
   u16 u16Data;  // 16bit data
   u32 u32Addr;  // 16bit Bank addr + 8bit 16bit-regaddr
   u16 u16Mask;  // inverse normal case
   u64 u64Cmd;   // 64bit to consist CMDQ CMD
   u8  u8AssignFramecnt;
   u8  bCntWarn;
   u8  bAddPollFunc;
}DrvSclCmdqCmd_t;

typedef struct
{
   u16 u16Data;  // 16bit data
   u32 u32Addr;  // 16bit Bank addr + 8bit 16bit-regaddr
   u16 u16Mask;  // inverse normal case
}DrvSclCmdqCmdReg_t;

typedef enum
{
    E_DRV_SCLCMDQ_EVENT_RUN = 0x00000001,
    E_DRV_SCLCMDQ_EVENT_IRQ = 0x00000002,
} DrvSclCmdqEventType_e;

#define DRV_SCLCMDQ_OK                   0x00000000
#define DRV_SCLCMDQ_FAIL                 0x00000001

typedef struct
{
    u32 u32RiuBase;
    u32 u32CMDQ_Phy[E_DRV_SCLCMDQ_TYPE_MAX];
    u32 u32CMDQ_Size[E_DRV_SCLCMDQ_TYPE_MAX];
    u32 u32CMDQ_Vir[E_DRV_SCLCMDQ_TYPE_MAX];
}DrvSclCmdqOpen_t;
typedef struct
{
    void *pCmdqVirAddr;
    void *pSramVirAddr;
}DrvSclCmdqCtxConfig_t;

//--------------------------------------------------------------------------------------------------
//  Function Prototype
//--------------------------------------------------------------------------------------------------
//==============================Enable===============================================
void DrvSclCmdqEnable(bool bEnable,DrvSclCmdqIpType_e enIPType);
void DrvSclCmdqSetForceSkip(bool bEn);
void DrvSclCmdqAllocDirectInst(void);
void DrvSclCmdqSetInst(DrvSclCmdqCtxConfig_t stInst);
void DrvSclCmdqFreeDirectInst(void);

//==============================Delete===============================================
void DrvSclCmdqDelete(DrvSclCmdqIpType_e enIPType);
void DrvSclCmdqSetRPoint(DrvSclCmdqIpType_e enIPType);
void DrvSclCmdqSetDPoint(DrvSclCmdqIpType_e enIPType);
void DrvSclCmdqSetLPoint(DrvSclCmdqIpType_e enIPType);
u32  DrvSclCmdqBeTrigger(DrvSclCmdqIpType_e enIPType,bool bStart);
u64  DrvSclCmdqGetCMDFromPoint(DrvSclCmdqIpType_e enIPType ,u16 u16Point);
u32  DrvSclCmdqGetCMDBankFromCMD(u64 u64Cmd);
void DrvSclCmdqWriteRegWithMaskDirect(u32 u32Addr,u16 u16Data,u16 u16Mask);
//===============================init==============================================
//-------------------------------------------------------------------------------------------------
/// DrvSclCmdqInit
/// is CMDQ IP init function, to set timer,address and size
/// @param  PhyAddr1        \b IN: from msys_request_dmem,and transform to MIU address
/// @param  u32VirAddr1  \b IN:like phyaddr,map to kernel virtual
/// @param  u32BufByteLen \b IN:already allocate memory size
/// @param  u32RIUBase     \b IN:RIU's base shift
//-------------------------------------------------------------------------------------------------
void DrvSclCmdqInit(DrvSclCmdqOpen_t stCMDQIniCfg);
void DrvSclCmdqInitRIUBase(u32 u32RIUBase);

//-------------------------------------------------------------------------------------------------
/// Drv_CMDQ_Set_timer_ratio
/// set poll times in 1sec
/// set time / ratio, total wait time is (wait_time * (ratio + 1)) =>ex. ( FF * (F + 1) / 216  MHz ) = sec
///                                                                                      4D*1=4D~=77 *(216/M) 1 ~=60(frame/s)
/// the polling_timer is for re-checking polling value, if the time_interval is too small, cmdq will polling RIU frequently, so that RIU will very busy
/// @param  time  \b IN: poll wait time :#CMDQ_POLL_TIMER
/// @param  ratio  \b IN: time ratio       :# CMDQ_TIMER_RATIO
/// retval : OK
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
/// DrvSclCmdqSetSkipPollWhenWaitTimeOut
/// set wait timeout count and whether jump timeout
/// @param   u16bjump \b IN:if true, timeout will jump wait, and  carry out  command
//-------------------------------------------------------------------------------------------------
void DrvSclCmdqSetSkipPollWhenWaitTimeOut(DrvSclCmdqIpType_e enIPType,u16 bEn);
//--------------------------------------------------------------------------------------------------
// Drv_CMDQ_Set_Buffer
// set addr st and end pointer ,and set move range
// In ring-buffer mode, this function will trig for update reg_sw_wr_mi_wadr
// The CMDQ will keep on executing cmd until reg_rd_mi_radr reach reg_sw_wr_mi_wadr
/// @param   StartAddr \b IN:MIU addr(byte base) --> IP need 16byte addr,so function inside will /16
/// @param   EndAddr \b IN:MIU addr
//--------------------------------------------------------------------------------------------------
u16  DrvSclCmdqGetFinalIrq(DrvSclCmdqIpType_e enIPType);
void DrvSclCmdqClearIrqByFlag(DrvSclCmdqIpType_e enIPType,u16 u16Irq);
void DrvSclCmdqSetISRStatus(bool bEn);
bool DrvSclCmdqFireDirectBufferRingMode(DrvSclCmdqIpType_e enIPType,bool bStart);
void DrvSclCmdqFireFirstTime(void);
bool DrvSclCmdqFireSram(DrvSclCmdqIpType_e enIPType,bool bStart);
void DrvSclCmdqGetModuleMutex(DrvSclCmdqIpType_e enIPType,bool bEn);
DrvSclCmdqInfo_t DrvSclCmdqGetCMDQInformation(DrvSclCmdqIpType_e enIPType);

//===============================write==============================================
//-------------------------------------------------------------------------------------------------
/// DrvSclCmdqWriteCmd
/// write cmd API for any case ,to add cmd to MIU
/// @param  u32Addr   \b IN: 8bit-addr
/// @param  u16Data  \b IN:  16bit data
/// @param  u16Mask \b IN:  16bit ~mask,
/// @param  bSkipCheckSameAddr     \b IN:if true,don't need to check .if false don't use mask(for RIU 32bit)
//-------------------------------------------------------------------------------------------------
bool DrvSclCmdqWriteCmd(DrvSclCmdqIpType_e enIPType,DrvSclCmdqCmdReg_t stCfg,bool bSkipCheckSameAddr);
void DrvSclCmdqFillCmd(DrvSclCmdqCmdReg_t *stCfg,u32 u32Addr, u16 u16Data, u16 u16Mask);
void DrvSclCmdqSramReset(void);
bool DrvSclCmdqWriteSramCmd(DrvSclCmdqIpType_e enIPType, DrvSclCmdqCmdReg_t stCfg, bool bSkipCheckSameAddr);
bool DrvSclCmdqWrite(DrvSclCmdqIpType_e enIPType,u32 u32Addr, u16 u16Data, u16 u16Mask);
void DrvSclCmdqrelease(DrvSclCmdqIpType_e enIPType);
u8   DrvSclCmdqGetISPHWCnt(void);

//-------------------------------------------------------------------------------------------------
/// Drv_CMDQ_AddCmd
/// To add CMD to MIU, _Buf is use to nonframe count
/// @param  u64Cmd         \b IN: the CMD want to write to MIU
/// @param  bSkipCheckSameAddr         \b IN:  if true,don't need to check .if false don't use mask(for RIU 32bit)
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
/// Drv_CMDQ_MoveBufCmd
/// To move nonframe count CMD form nonframe count region to assframe count region
/// @param  enIPType         \b IN: Buf number,nonuse
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
/// Drv_CMDQ_AddCmd_Check
/// To check the CMD whether already exist. if yes ,to handle it.
/// @param  u64Cmd  \b IN: the CMD want to write to MIU
/// @param  bframecount      \b IN: if true, is frame count case.
/// @param  bSkipCheckSameAddr     \b IN:if true,don't need to check .if false don't use mask(for RIU 32bit)
//-------------------------------------------------------------------------------------------------

 //===============================wait==============================================
 //-------------------------------------------------------------------------------------------------
 /// DrvSclCmdqWaitCmd
 /// To add wait CMD
 /// @param  u16bus         \b IN: wait trigger bus
 //-------------------------------------------------------------------------------------------------
 void DrvSclCmdqWaitCmd(DrvSclCmdqIpType_e enIPType, u16 u16bus);

 //===============================poll==============================================
 //-------------------------------------------------------------------------------------------------
 /// DrvSclCmdqPollingEqCmd
 /// add Polling CMD ,or neq CMD
 /// @param  u32Addr   \b IN: 8bit-addr
 /// @param  u16Data  \b IN:  16bit data
 /// @param  u16Mask \b IN:  16bit ~mask,
 //-------------------------------------------------------------------------------------------------
 void DrvSclCmdqPollingEqCmd(DrvSclCmdqIpType_e enIPType,DrvSclCmdqCmdReg_t stCfg);
 void DrvSclCmdqPollingNeqCmd(DrvSclCmdqIpType_e enIPType,DrvSclCmdqCmdReg_t stCfg);
 void DrvSclCmdqCheckVIPSRAM(u32 u32Type);

 //===============================fire==============================================
 //-------------------------------------------------------------------------------------------------
 /// DrvSclCmdqFire
 /// To trig CMDQ
 /// @param  bStart   \b IN: trig
 //-------------------------------------------------------------------------------------------------
bool DrvSclCmdqFire(DrvSclCmdqIpType_e enIPType,bool bStart);
void DrvSclCmdqSetEventForFire(void);

//===============================check==============================================
//-------------------------------------------------------------------------------------------------
/// DrvSclCmdqCheckIPAlreadyDone
/// To check CMDQ status
//-------------------------------------------------------------------------------------------------
bool DrvSclCmdqCheckIPAlreadyDone(DrvSclCmdqIpType_e enIPType);

//===============================write Register==============================================
#endif // __DRV_SCL_CMDQ_H__
