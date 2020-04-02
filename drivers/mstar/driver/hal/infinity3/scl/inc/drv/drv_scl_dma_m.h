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

#ifndef _MDRV_SCLDMA_H
#define _MDRV_SCLDMA_H


//-------------------------------------------------------------------------------------------------
//  Defines & Enum
//-------------------------------------------------------------------------------------------------
#define MDRV_SCLDMA_BLOCK_POLL_EN       0
#define MDRV_SCLDMA_BLOCK_POLL_ISR_EN   1
#define BUFFER_BE_ALLOCATED_MAX         4
#define MDRV_SCLDMA_BUFFER_QUEUE_OFFSET   sizeof(MDrvSclDmaFrameBufferConfig_t)///<Buffer Queue Size For OMX

#define PARSING_SCLDMA_IOID(x)           (x==E_MDRV_SCLDMA_ID_1   ? "SCLDMA_1" : \
                                        x==E_MDRV_SCLDMA_ID_1   ? "SCLDMA_2" : \
                                        x==E_MDRV_SCLDMA_ID_3   ? "SCLDMA_3" : \
                                        x==E_MDRV_SCLDMA_ID_PNL   ? "SCLDMA_PNL" : \
                                                               "UNKNOWN")

#define PARSING_SCLDMA_IOMEM(x)           (x==E_MDRV_SCLDMA_MEM_FRM   ? "FRM" : \
                                        x==E_MDRV_SCLDMA_MEM_SNP   ? "SNP" : \
                                        x==E_MDRV_SCLDMA_MEM_FRM2   ? "FRM2" : \
                                        x==E_MDRV_SCLDMA_MEM_IMI   ? "IMI" : \
                                                               "UNKNOWN")



#define PARSING_SCLDMA_IOBUFMD(x)       (x==E_MDRV_SCLDMA_BUFFER_MD_RING   ? "RING" : \
                                        x==E_MDRV_SCLDMA_BUFFER_MD_SWRING ? "SWRING" : \
                                       x==E_MDRV_SCLDMA_BUFFER_MD_SINGLE ? "SINGLE" : \
                                                                   "UNKNOWN")

#define PARSING_SCLDMA_IOCOLOR(x)       (x==E_MDRV_SCLDMA_COLOR_YUV422? "422PACK" : \
                                       x==E_MDRV_SCLDMA_COLOR_YUV420 ? "YCSep420" : \
                                       x==E_MDRV_SCLDMA_COLOR_YCSep422 ? "YCSep422" : \
                                       x==E_MDRV_SCLDMA_COLOR_YUVSep422 ? "YUVSep422" : \
                                       x==E_MDRV_SCLDMA_COLOR_YUVSep420 ? "YUVSep420" : \
                                                                  "UNKNOWN")


typedef enum
{
    E_MDRV_SCLDMA_ID_1,
    E_MDRV_SCLDMA_ID_2,
    E_MDRV_SCLDMA_ID_3,
    E_MDRV_SCLDMA_ID_PNL,
    E_MDRV_SCLDMA_ID_NUM,
}MDrvSclDmaIdType_e;

typedef enum
{
    E_MDRV_SCLDMA_ACTIVE_BUFFER_OMX         = 0x10,
    E_MDRV_SCLDMA_ACTIVE_BUFFER_OFF         = 0x20,
    E_MDRV_SCLDMA_ACTIVE_BUFFER_RINGFULL    = 0x40,
}MDrvSclDmaActiveBufferType_e;

typedef enum
{
    E_MDRV_SCLDMA_BUFFER_MD_RING,   ///< buffer mode: RING
    E_MDRV_SCLDMA_BUFFER_MD_SINGLE, ///< bufer mode : single
    E_MDRV_SCLDMA_BUFFER_MD_SWRING, ///< bufer mode : sw control ring mode
    E_MDRV_SCLDMA_BUFFER_MD_NUM,    ///< The max number of buffer mode
}MDrvSclDmaBufferModeType_e;

typedef enum
{
    E_MDRV_SCLDMA_COLOR_YUV422, ///< color format: 422Pack
    E_MDRV_SCLDMA_COLOR_YUV420, ///< color format: YCSep420
    E_MDRV_SCLDMA_COLOR_YCSep422, ///< color format: YC422
    E_MDRV_SCLDMA_COLOR_YUVSep422, ///< color format: YUVSep422
    E_MDRV_SCLDMA_COLOR_YUVSep420, ///< color format: YUVSep420
    E_MDRV_SCLDMA_COLOR_NUM,    ///< The max number of color format
}MDrvSclDmaColorType_e;

typedef enum
{
     E_MDRV_SCLDMA_MEM_FRM  = 0,  ///< memory type: FRM
     E_MDRV_SCLDMA_MEM_SNP  = 1,  ///< memory type: SNP
     E_MDRV_SCLDMA_MEM_FRM2 = 2,  ///< memory type: FRM2
     E_MDRV_SCLDMA_MEM_IMI  = 3,  ///< memory type: IMI
     E_MDRV_SCLDMA_MEM_NUM  = 4,  ///< The max number of memory type
}MDrvSclDmaMemType_e;
typedef struct
{
    u8  btsBase_0   : 1;  ///< base0 flag
    u8  btsBase_1   : 1;  ///< base1 flag
    u8  btsBase_2   : 1;  ///< base2 flag
    u8  btsBase_3   : 1;  ///< Base3 flag
    u8  btsReserved : 4;  ///< reserved
}MDrvSclDmaBufferFlagBit_t;
typedef enum
{
    EN_MDRV_SCLDMA_BUFFER_QUEUE_TYPE_PEEKQUEUE,    ///< buffer queue type: user only to get information
    EN_MDRV_SCLDMA_BUFFER_QUEUE_TYPE_DEQUEUE,      ///< buffer queue type: user can set Read information
    EN_MDRV_SCLDMA_BUFFER_QUEUE_TYPE_ENQUEUE,      ///< TODO : buffer queue type: user can set Write information(not use)
    EN_MDRV_SCLDMA_BUFFER_QUEUE_TYPE_NUM,          ///< buffer type: totally
}MDrvSclDMaUsedBufferQueueType_e;

//-------------------------------------------------------------------------------------------------
//  Structure
//-------------------------------------------------------------------------------------------------
typedef struct
{
    u32 u32Riubase;
    u32 u32IRQNUM;//scl
    u32 u32CMDQIRQNUM;//cmdq
}MDrvSclDmaInitConfig_t;
typedef struct
{
    void* idclk;
    void* fclk1;
    void* fclk2;
    void* odclk;
}MDrvSclDmaClkConfig_t;

typedef struct
{
    bool       bEn;
    MDrvSclDmaMemType_e   enMemType;
    MDrvSclDmaClkConfig_t *stclk;
}MDrvSclDmaTriggerConfig_t;
typedef struct
{
    union
    {
        u8  u8Flag;
        MDrvSclDmaBufferFlagBit_t bvFlag;   ///< buffer flag
    };

    MDrvSclDmaMemType_e   enMemType;         ///< memory type
    MDrvSclDmaColorType_e enColorType;       ///< color type
    MDrvSclDmaBufferModeType_e enBufMDType; ///< buffer mode

    u16      u16BufNum;    ///< number of buffer
    u32      u32Base_Y[BUFFER_BE_ALLOCATED_MAX]; ///< base address of Y
    u32      u32Base_C[BUFFER_BE_ALLOCATED_MAX]; ///< base address of CbCr
    u32      u32Base_V[BUFFER_BE_ALLOCATED_MAX]; ///< base address of Cr
    u16      u16Width;     ///< width of buffer
    u16      u16Height;    ///< height of buffer
} MDrvSclDmaBufferConfig_t;

typedef struct
{
    bool bDone;
}MDrvSclDmaBufferDoneConfig_t;
typedef struct
{
    MDrvSclDmaMemType_e   enMemType;
    u8 u8ActiveBuffer;
    MDrvSclDmaTriggerConfig_t stOnOff;
    u8   u8ISPcount;
    u64   u64FRMDoneTime;
}MDrvSclDmaActiveBufferConfig_t;
typedef struct
{
    u16 u16DMAcount;
    u32 u32Trigcount;
    u16 u16DMAH;
    u16 u16DMAV;
    MDrvSclDmaColorType_e enColorType;       ///< color type
    MDrvSclDmaBufferModeType_e enBufMDType; ///< buffer mode
    u16 u16BufNum;    ///< number of buffer
    u32 u32Base_Y[BUFFER_BE_ALLOCATED_MAX]; ///< base address of Y
    u32 u32Base_C[BUFFER_BE_ALLOCATED_MAX]; ///< base address of CbCr
    u32 u32Base_V[BUFFER_BE_ALLOCATED_MAX]; ///< base address of Cr
    bool bDMAEn;
    bool bDMAReadIdx;
    bool bDMAWriteIdx;
    bool bDMAFlag;
    u8  bSendPoll;
    u32 u32FrameDoneTime;
    u32 u32SendTime;
    u8  u8Count;
    u8  u8ResetCount;
    u8  u8DMAErrCount;
}MDrvSclDmaAttrType_t;

typedef struct
{
    u8    u8FrameAddrIdx;       ///< ID of Frame address
    u32   u32FrameAddr;         ///< Frame Address
    u8    u8ISPcount;           ///< ISP counter
    u16   u16FrameWidth;         ///< Frame Width
    u16   u16FrameHeight;        ///< Frame Height
    u64   u64FRMDoneTime;  ///< Time of FRMDone
}__attribute__ ((__packed__))MDrvSclDmaFrameBufferConfig_t;

typedef struct
{
    MDrvSclDmaMemType_e   enMemType; ///< memory type
    MDrvSclDMaUsedBufferQueueType_e  enUsedType;
    MDrvSclDmaFrameBufferConfig_t stRead;
    u8  u8InQueueCount;
    u8  u8EnqueueIdx;
}__attribute__ ((__packed__))MDrvSclDmaBUfferQueueConfig_t;


//-------------------------------------------------------------------------------------------------
//  Prototype
//-------------------------------------------------------------------------------------------------
#ifndef _MDRV_SCLDMA_C
#define INTERFACE extern
#else
#define INTERFACE
#endif
INTERFACE bool MDrvSclDmaInit(MDrvSclDmaIdType_e enSCLDMA_ID, MDrvSclDmaInitConfig_t *pCfg);
INTERFACE bool MDrvSclDmaExit(bool bCloseISR);
INTERFACE bool MDrvSclDmaSetDmaReadClientConfig(MDrvSclDmaIdType_e enSCLDMA_ID, MDrvSclDmaBufferConfig_t *pCfg);
INTERFACE bool MDrvSclDmaSetDmaReadClientTrigger(MDrvSclDmaIdType_e enSCLDMA_ID, MDrvSclDmaTriggerConfig_t *pCfg);
INTERFACE bool MDrvSclDmaSetDmaWriteClientConfig(MDrvSclDmaIdType_e enSCLDMA_ID, MDrvSclDmaBufferConfig_t *pCfg);
INTERFACE bool MDrvSclDmaSetDmaWriteClientTrigger(MDrvSclDmaIdType_e enSCLDMA_ID, MDrvSclDmaTriggerConfig_t *pCfg);
INTERFACE bool MDrvSclDmaGetInBufferDoneEvent(MDrvSclDmaIdType_e enSCLDMA_ID, MDrvSclDmaMemType_e enMemType, MDrvSclDmaBufferDoneConfig_t *pCfg);
INTERFACE bool MDrvSclDmaGetOutBufferDoneEvent(MDrvSclDmaIdType_e enSCLDMA_ID, MDrvSclDmaMemType_e enMemType, MDrvSclDmaBufferDoneConfig_t *pCfg);
INTERFACE bool MDrvSclDmaGetDmaReadBufferActiveIdx(MDrvSclDmaIdType_e enSCLDMA_ID, MDrvSclDmaActiveBufferConfig_t *pstCfg);
INTERFACE bool MDrvSclDmaGetDmaWriteBufferAcitveIdx(MDrvSclDmaIdType_e enSCLDMA_ID, MDrvSclDmaActiveBufferConfig_t *pstCfg);
INTERFACE bool MDrvSclDmaBufferQueueHandle(MDrvSclDmaIdType_e enSCLDMA_ID, MDrvSclDmaBUfferQueueConfig_t *pstCfg);
INTERFACE MDrvSclDmaAttrType_t MDrvSclDmaGetDmaInformationByClient(MDrvSclDmaIdType_e enSCLDMA_ID,MDrvSclDmaMemType_e enMemType,bool bYC);

#if defined (SCLOS_TYPE_LINUX_KERNEL)
INTERFACE ssize_t MDrvSclDmaProcShow(char *buf, MDrvSclDmaIdType_e enID, MDrvSclDmaMemType_e enMem, bool bRread);
#endif

INTERFACE void MDrvSclDmaResetTrigCountByClient(MDrvSclDmaIdType_e enSCLDMA_ID,MDrvSclDmaMemType_e enMemType,bool bYC);
INTERFACE void MDrvSclDmaResetTrigCountAllClient(void);
INTERFACE void MDrvSclDmaSysInit(bool bEn);
INTERFACE void MDrvSclDmaRelease(MDrvSclDmaIdType_e enSCLDMA_ID,MDrvSclDmaClkConfig_t *stclkcfg);
INTERFACE void MDrvSclDmaClkClose(MDrvSclDmaClkConfig_t* stclk);
INTERFACE bool MDrvSclDmaSuspend(MDrvSclDmaIdType_e enSCLDMA_ID);

INTERFACE bool MDrvSclDmaSetPollWait(DrvSclOsPollWaitConfig_t stPollWait);

INTERFACE bool MDrvSclDmaResume(MDrvSclDmaIdType_e enSCLDMA_ID);
INTERFACE void * MDrvSclDmaGetWaitQueueHead(MDrvSclDmaIdType_e enSCLDMA_ID);
INTERFACE u32 MDrvSclDmaGetWaitEvent(MDrvSclDmaIdType_e enSCLDMA_ID);
INTERFACE void MDrvSclDmaSetForceCloseDmaClient
    (MDrvSclDmaIdType_e enSCLDMA_ID,MDrvSclDmaMemType_e enMemType,bool bYC,bool bEn);
INTERFACE bool MDrvSclDmaGetDoubleBufferStatus(void);
INTERFACE void MDrvSclDmaSetDoubleBufferConfig(bool bEn);

#undef INTERFACE
#endif
