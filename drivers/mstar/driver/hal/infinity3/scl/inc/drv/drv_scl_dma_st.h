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
// (!¡±MStar Confidential Information!¡L) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////
//==============================================================================
#ifndef __DRV_SCLDMA_ST_H__
#define __DRV_SCLDMA_ST_H__


//-------------------------------------------------------------------------------------------------
//  DEFINE
//-------------------------------------------------------------------------------------------------
#define MAX_BUFFER_COUNT 4
#define SCLTASKHANDLERMAX                     5
#define SCLTASKIDMAX                     4
#define SCLDMA_DBG_Thread                   0
#define DoubleBufferDefaultSet              0  // 0 close 1 open
#define DoubleBufferStatus                  gbDBStatus
#define ENABLE_RING_DB                      1
#define SCLDMA_IRQ_EN                       0
#define ENABLE_ACTIVEID_ISR                 1
#define SCLDMA_BUFFER_QUEUE_OFFSET            sizeof(DrvSclDmaFrameBufferConfig_t)
#define SCLDMA_IRQ_EVENT_ID_MSK                 0xFF000000
#define SCLDMA_IRQ_EVENT_CLIENT_MSK             0x000000FF
#define SCLDMA_IRQ_EVENT_CLEAR_IRQNUM_MSK       0x0000FF00
#define SCLDMA_IRQ_EVENT_CLEAR_MODE_MSK         0x00FF0000

#define GET_SCLDMA_IRQ_EVENT_CLIENT(x)              (x & SCLDMA_IRQ_EVENT_CLIENT_MSK)
#define GET_SCLDMA_IRQ_EVENT_CLEAR_IRQNUM(x)        ((x & SCLDMA_IRQ_EVENT_CLEAR_IRQNUM_MSK) >> 8)
#define GET_SCLDMA_IRQ_EVENT_CLEAR_MODE(x)          ((x & SCLDMA_IRQ_EVENT_CLEAR_MODE_MSK) >> 16)

#define SET_SCLDMA_IRQ_EVENT_PENDING(client)        (E_DRV_SCLDMA_IRQ_EVENT_PENDING |  \
                                                     (client & 0x000000FF))

#define SET_SCLDMA_IRQ_EVENT_CLEAR(client, mode, irqnum)    (E_DRV_SCLDMA_IRQ_EVENT_CLEAR | \
                                                             (client & 0xFF)          | \
                                                             ((irqnum & 0xFF) << 8)   | \
                                                             ((mode & 0xFF)<<16))

//-------------------------------------------------------------------------------------------------
//  ENUM
//-------------------------------------------------------------------------------------------------

typedef enum
{
    E_DRV_SCLDMA_ID_1_W,
    E_DRV_SCLDMA_ID_2_W,
    E_DRV_SCLDMA_ID_3_W,
    E_DRV_SCLDMA_ID_3_R,
    E_DRV_SCLDMA_ID_PNL_R,
    E_DRV_SCLDMA_ID_MAX,    //scldma ID type,I1 has 1_W,2_W,3_R,3_R,PNL_R
}DrvSclDmaIdType_e;

typedef enum
{
    E_DRV_SCLDMA_FRM_W,
    E_DRV_SCLDMA_SNP_W,
    E_DRV_SCLDMA_IMI_W,
    E_DRV_SCLDMA_FRM_R,
    E_DRV_SCLDMA_DBG_R,
    E_DRV_SCLDMA_IMI_R,
    E_DRV_SCLDMA_FRM2_W,
    E_DRV_SCLDMA_RW_NUM,    //scldma rwmode, I1 has FRM,SNP,IMI RW
}DrvSclDmaRwModeType_e;

typedef enum
{
    E_DRV_SCLDMA_DB_STATUS_NEED_OFF,
    E_DRV_SCLDMA_DB_STATUS_NEED_ON,
    E_DRV_SCLDMA_DB_STATUS_KEEP,
    E_DRV_SCLDMA_DB_STATUS_NUM,    //double buffer switcher status, 3 choise
}DrvSclDmaDbStatusType_e;

typedef enum
{
    E_DRV_SCLDMA_1_FRM_W = 0,
    E_DRV_SCLDMA_1_SNP_W = 1,
    E_DRV_SCLDMA_2_FRM_W = 2,
    E_DRV_SCLDMA_2_FRM2_W = 3,
    E_DRV_SCLDMA_1_IMI_W = 4,
    E_DRV_SCLDMA_2_IMI_W = 5,
    E_DRV_SCLDMA_3_FRM_R = 6,
    E_DRV_SCLDMA_3_FRM_W = 7,
    E_DRV_SCLDMA_4_FRM_R = 8,
    E_DRV_SCLDMA_3_IMI_R = 9,
    E_DRV_SCLDMA_CLIENT_NUM,    //scldma client ,I1 has dma_1:3 cli dma_2 :2cli dma_3:In/Out debug:pnl &dma_3_IMIR
}DrvSclDmaClientType_e;


typedef enum
{
    E_DRV_SCLDMA_BUF_MD_RING,
    E_DRV_SCLDMA_BUF_MD_SINGLE,
    E_DRV_SCLDMA_BUF_MD_SWRING,
    E_DRV_SCLDMA_BUF_MD_NUM,    //scldma DMA write mode,I1 has 2 type: Ring(HW),Single(HW+SW)
}DrvSclDmaBufferModeType_e;

typedef enum
{
    E_DRV_SCLDMA_COLOR_YUV422, ///< color format: 422Pack
    E_DRV_SCLDMA_COLOR_YUV420, ///< color format: YCSep420
    E_DRV_SCLDMA_COLOR_YCSep422, ///< color format: YC422
    E_DRV_SCLDMA_COLOR_YUVSep422, ///< color format: YUVSep422
    E_DRV_SCLDMA_COLOR_YUVSep420, ///< color format: YUVSep420
    E_DRV_SCLDMA_COLOR_NUM,    //I3 dma has 5 color type 422,420
}DrvSclDmaColorType_e;

typedef enum
{
    E_DRV_SCLDMA_VS_ID_SC    = 0,
    E_DRV_SCLDMA_VS_ID_AFF   = 1,
    E_DRV_SCLDMA_VS_ID_LDC   = 2,
    E_DRV_SCLDMA_VS_ID_SC3   = 3,
    E_DRV_SCLDMA_VS_ID_DISP  = 4,
    E_DRV_SCLDMA_VS_ID_NUM   = 5,   //scl handle vsync type,I1 can handle in AFF,LDC,SC3,DISP, suggest sc1 use HW mode ,sc3 use regen mode
}DrvSclDmaVsIdType_e;



typedef enum
{
    E_DRV_SCLDMA_VS_TRIG_MODE_HW_IN_VSYNC = 0,
    E_DRV_SCLDMA_VS_TRIG_MODE_HW_DELAY    = 1,
    E_DRV_SCLDMA_VS_TRIG_MODE_SWTRIGGER   = 2,
    E_DRV_SCLDMA_VS_TRIG_MODE_DISP_FM_END = 3,
    E_DRV_SCLDMA_VS_TRIG_MODE_NUM         = 4,
}DrvSclDmaVsTrigModeType_e;


typedef enum
{
    E_DRV_SCLDMA_REF_VS_REF_MODE_FALLING = 0,
    E_DRV_SCLDMA_REF_VS_REF_MODE_RASING  = 1,
    E_DRV_SCLDMA_REF_VS_REF_MODE_NUM     = 2,
}DrvSclDmaRegenVsRefModeType_e;


typedef enum
{
    E_DRV_SCLDMA_IRQ_EVENT_TIMER   = 0x01000000,
    E_DRV_SCLDMA_IRQ_EVENT_PENDING = 0x02000000,
    E_DRV_SCLDMA_IRQ_EVENT_CLEAR   = 0x03000000,
    E_DRV_SCLDMA_IRQ_EVENT_ALL     = 0x0FFFFFFF,
}DrvSclDmaIrqEventType_e;

typedef enum
{
    E_SCLDMA_IRQ_MODE_PENDING  = 0x00,
    E_DRV_SCLDMA_IRQ_MODE_ACTIVE   = 0x01,
    E_DRV_SCLDMA_IRQ_MODE_ACTIVE_N = 0x02,
    E_DRV_SCLDMA_IRQ_MODE_SIDONE   = 0x03,
    E_DRV_SCLDMA_IRQ_MODE_END      = 0x04,
    E_DRV_SCLDMA_IRQ_MODE_DONE     = 0x06,
    E_DRV_SCLDMA_IRQ_MODE_ALL      = 0x07,
}DrvSclDmaIrqModeType_e;

typedef enum
{
    E_DRV_SCLDMA_ACTIVE_BUFFER_OMX          = 0xF0, // OMX buffer status ,driver reserve.
    E_DRV_SCLDMA_ACTIVE_BUFFER_SCL          = 0x03, // SCL buffer status, save active/done buffer idx
    E_DRV_SCLDMA_ACTIVE_BUFFER_OFF          = 0x04, // already not use
    E_DRV_SCLDMA_ACTIVE_BUFFER_ACT          = 0x08, // already not use
    E_DRV_SCLDMA_ACTIVE_BUFFER_SCLANDFLAG   = 0x0F, // like E_DRV_SCLDMA_ACTIVE_BUFFER_SCL
    E_DRV_SCLDMA_ACTIVE_BUFFER_OMX_FLAG     = 0x10, // it's represent OMX is update empty buffer to buffer
    E_DRV_SCLDMA_ACTIVE_BUFFER_OMX_TRIG     = 0x20, // it's represent dma turn off and driver update to OMX
    E_DRV_SCLDMA_ACTIVE_BUFFER_OMX_RINGFULL = 0x40, // it's represent dma already done one buffer, but OMX isn't to receive.
}DrvSclDmaActiveBufferType_e;

//bit0 next time off , bit 1 blanking ,bit 2 DMAonoff, bit3 no DMA on ,bit4 ever DMA on ,but already off or open again
typedef enum
{
    E_DRV_SCLDMA_FLAG_NEXT_ON   = 0x01, // for non double buffer mode, if active wait for blanking
    E_DRV_SCLDMA_FLAG_BLANKING  = 0x02, // if reset dma,active id is temporarily state, need to handle until first frame done.
    E_DRV_SCLDMA_FLAG_DMAOFF    = 0x04, // ref E_DRV_SCLDMA_ACTIVE_BUFFER_OFF,dma off
    E_DRV_SCLDMA_FLAG_ACTIVE    = 0x08, // ref E_DRV_SCLDMA_ACTIVE_BUFFER_ACT,dma act
    E_DRV_SCLDMA_FLAG_EVERDMAON = 0x10, // like E_DRV_SCLDMA_ACTIVE_BUFFER_OMX_RINGFULL
    E_DRV_SCLDMA_FLAG_FRMIN     = 0x20, // this frame is integrate.
    E_DRV_SCLDMA_FLAG_FRMDONE   = 0x40, // dma done.(idle)
    E_DRV_SCLDMA_FLAG_FRMIGNORE = 0x80, // dma done but ignore.
    E_DRV_SCLDMA_FLAG_DROP      = 0x100, // set drop this frame
    E_DRV_SCLDMA_FLAG_NEXT_OFF  = 0x200, // for non double buffer mode, if active wait for blanking
    E_DRV_SCLDMA_FLAG_DMAFORCEOFF = 0x400, // ref E_DRV_SCLDMA_ACTIVE_BUFFER_OFF,dma off
}DrvSclDmaFlagType_e;

typedef enum
{
    E_DRV_SCLDMA_ISR_LOG_ISPOFF     = 0x1,
    E_DRV_SCLDMA_ISR_LOG_SNPONLY    = 0x2,
    E_DRV_SCLDMA_ISR_LOG_SNPISR     = 0x3,
    E_DRV_SCLDMA_ISR_LOG_ISPON      = 0x4,
    E_DRV_SCLDMA_ISR_LOG_SC1ON      = 0x5,
    E_DRV_SCLDMA_ISR_LOG_SC1OFF     = 0x6,
    E_DRV_SCLDMA_ISR_LOG_SC2ON      = 0x7,
    E_DRV_SCLDMA_ISR_LOG_SC2OFF     = 0x8,
    E_DRV_SCLDMA_ISR_LOG_SC1A       = 0x9,
    E_DRV_SCLDMA_ISR_LOG_SC1N       = 0xA,
    E_DRV_SCLDMA_ISR_LOG_SC2A       = 0xB,
    E_DRV_SCLDMA_ISR_LOG_SC2N       = 0xC,
    E_DRV_SCLDMA_ISR_LOG_FRMEND     = 0xD,//isr log status
}DrvSclDmaIsrLogType_e;


//-------------------------------------------------------------------------------------------------
//  Structure
//-------------------------------------------------------------------------------------------------
typedef struct
{
    u8 u8RPoint;//R-OMX
    u8 u8WPoint;//W-SCL
    u8 u8LPoint;// L-last time scl
    bool bRWequal;
    bool bRPointChange;
    DrvSclDmaClientType_e enClientType;
}DrvSclDmaPointConfig_t;
typedef struct
{
    bool flag;
    s32 s32Taskid[SCLTASKIDMAX];
    s32 s32HandlerId[SCLTASKHANDLERMAX];
    DrvSclOsTaskStruct_t sttask;
}DrvSclDmaThreadConfig_t;

typedef struct
{
    u32 u32RIUBase;
    u32 u32IRQNUM;//scl
    u32 u32CMDQIRQNUM;//cmdq
}DrvSclDmaInitConfig_t;

typedef struct
{
    u32 u32IRQNum;
}DrvSclDmaSuspendResumeConfig_t;

typedef struct
{
    u8 btsBase_0   : 1;
    u8 btsBase_1   : 1;
    u8 btsBase_2   : 1;
    u8 btsBase_3   : 1;
    u8 btsReserved : 4;
}DrvSclDmaRwFlagType_t;

typedef struct
{
    union
    {
        u8 u8Flag;
        DrvSclDmaRwFlagType_t bvFlag;
    };

    DrvSclDmaRwModeType_e enRWMode;
    DrvSclDmaColorType_e enColor;
    DrvSclDmaBufferModeType_e enBuffMode;
    u32 u32Base_Y[MAX_BUFFER_COUNT];
    u32 u32Base_C[MAX_BUFFER_COUNT];
    u32 u32Base_V[MAX_BUFFER_COUNT];
    u8  u8MaxIdx;
    u16 u16Width;
    u16 u16Height;
}DrvSclDmaRwConfig_t;

typedef struct
{
    DrvSclOsClkStruct_t* idclk;
    DrvSclOsClkStruct_t* fclk1;
    DrvSclOsClkStruct_t* fclk2;
    DrvSclOsClkStruct_t* odclk;
}DrvSclDmaClkConfig_t;

typedef struct
{
    bool bEn;
    DrvSclDmaRwModeType_e enRWMode;
    DrvSclDmaClkConfig_t *stclk;
}DrvSclDmaOnOffConfig_t;

typedef struct
{
    DrvSclDmaVsTrigModeType_e enTrigMd;
    DrvSclDmaRegenVsRefModeType_e enVsRefMd;
    u16 u16Vs_Width;
    u16 u16Vs_St;
}DrvSclDmaVsConfig_t;


typedef struct
{
    bool bDone;
    DrvSclDmaRwModeType_e enRWMode;
}DrvSclDmaDoneConfig_t;

typedef struct
{
    u8 u8ActiveBuffer;
    DrvSclDmaRwModeType_e enRWMode;
    DrvSclDmaOnOffConfig_t stOnOff;
    u8       u8ISPcount;
    u64      u64FRMDoneTime;
}DrvSclDmaActiveBufferConfig_t;
typedef struct
{
    u8   u8FrameAddrIdx;       ///< ID of Frame address
    u32   u32FrameAddr;         ///< Frame Address
    u8   u8ISPcount;           ///< ISP counter
    u16  u16FrameWidth;         ///< Frame Width
    u16  u16FrameHeight;        ///< Frame Height
    u64   u64FRMDoneTime;       ///< Time of FRMDone
}__attribute__ ((__packed__))DrvSclDmaFrameBufferConfig_t;

typedef struct
{
    DrvSclDmaRwModeType_e enRWMode;
    DrvSclDmaIdType_e enID;
    bool bUsed;
    bool bFull;
    u8 u8Bufferflag;
    DrvSclDmaFrameBufferConfig_t* pstHead;
    DrvSclDmaFrameBufferConfig_t* pstTail;
    DrvSclDmaFrameBufferConfig_t* pstWrite;
    DrvSclDmaFrameBufferConfig_t* pstWriteAlready;
    DrvSclDmaFrameBufferConfig_t* pstRead;
    u8  u8InQueueCount;
    u8  u8NextActiveId;
    u8  u8AccessId;
}DrvSclDmaBufferQueueConfig_t;

typedef struct
{
    DrvSclDmaVsConfig_t stVsCfg[E_DRV_SCLDMA_VS_ID_NUM];
    DrvSclDmaBufferModeType_e enBuffMode[E_DRV_SCLDMA_CLIENT_NUM];
    bool bDMAOnOff[E_DRV_SCLDMA_CLIENT_NUM];
    bool bDMAidx[E_DRV_SCLDMA_CLIENT_NUM];
    bool bMaxid[E_DRV_SCLDMA_CLIENT_NUM];
    u16 bDmaflag[E_DRV_SCLDMA_CLIENT_NUM];//bit0 next time off , bit 1 blanking ,bit 2 DMAonoff, bit3 no DMA on
    u64 u64mask;
    DrvSclDmaColorType_e enColor[E_DRV_SCLDMA_CLIENT_NUM];
    u32 u32Base_Y[E_DRV_SCLDMA_CLIENT_NUM][MAX_BUFFER_COUNT];
    u32 u32Base_C[E_DRV_SCLDMA_CLIENT_NUM][MAX_BUFFER_COUNT];
    u32 u32Base_V[E_DRV_SCLDMA_CLIENT_NUM][MAX_BUFFER_COUNT];
    u16 u16FrameWidth[E_DRV_SCLDMA_CLIENT_NUM];
    u16 u16FrameHeight[E_DRV_SCLDMA_CLIENT_NUM];
#if SCLDMA_IRQ_EN
    s32 s32IrqTaskid;
    s32 s32IrqEventId;
    s32 s32IrqTimerId;
#endif
}DrvSclDmaInfoType_t;
typedef struct
{
    u16 u16DMAcount;
    u16 u16DMAH;
    u16 u16DMAV;
    u32 u32Trigcount;
    DrvSclDmaColorType_e enColor;
    DrvSclDmaBufferModeType_e enBuffMode;
    u32 u32Base_Y[MAX_BUFFER_COUNT];
    u32 u32Base_C[MAX_BUFFER_COUNT];
    u32 u32Base_V[MAX_BUFFER_COUNT];
    u8  u8MaxIdx;
    u8  bDMAEn;
    u8  bDMAReadIdx;
    u8  bDMAWriteIdx;
    u8  bDMAFlag;
    u8  bSendPoll;
    u32 u32FrameDoneTime;
    u32 u32SendTime;
    u8  u8Count;
    u8  u8ResetCount;
    u8  u8DMAErrCount;
}DrvSclDmaAttrType_t;
//-------------------------------------------------------------------------------------------------
//  Prototype
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  extern Variable
//-------------------------------------------------------------------------------------------------
/////////////////
/// ScldmaInfo
/// use in Drvscldma and Drvsclirq
/// record all DMA client status
////////////////
extern DrvSclDmaInfoType_t  gstScldmaInfo;
extern bool gbDBStatus;
#endif
