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

#ifndef _MDRV_HVSP_H
#define _MDRV_HVSP_H



#define MDRV_SCLHVSP_CROP_NUM  2
#define DNR_BUFFER_MODE        1 // depend on DNR Buffer
#define IPMPATH
//-------------------------------------------------------------------------------------------------
//  Defines & Structure
//-------------------------------------------------------------------------------------------------
typedef enum
{
    E_MDRV_SCLHVSP_ID_1,
    E_MDRV_SCLHVSP_ID_2,
    E_MDRV_SCLHVSP_ID_3,
    E_MDRV_SCLHVSP_ID_MAX,     //I1 has 3 HVSP
}MDrvSclHvspIdType_e;
typedef enum
{
    E_MDRV_SCLHVSP_SRC_ISP,       ///< input source: ISP
    E_MDRV_SCLHVSP_SRC_BT656,     ///< input source: BT656
    E_MDRV_SCLHVSP_SRC_DRAM,      ///< input source: DRAM
    E_MDRV_SCLHVSP_SRC_HVSP,      ///< input source: HVSP1
    E_MDRV_SCLHVSP_SRC_PAT_TGEN,  ///< input source: PATGEN
    E_MDRV_SCLHVSP_SRC_NUM,       ///< The max number of input source
}MDrvSclHvspSrcType_e;

typedef enum
{
    E_MDRV_SCLHVSP_COLOR_RGB,     ///< color format:RGB
    E_MDRV_SCLHVSP_COLOR_YUV444,  ///< color format:YUV444
    E_MDRV_SCLHVSP_COLOR_YUV422,  ///< color format:YUV422
    E_MDRV_SCLHVSP_COLOR_YUV420,  ///< color format:YUV420
    E_MDRV_SCLHVSP_COLOR_NUM,     ///< The max number of color format
}MDrvSclHvspColorType_e;

typedef enum
{
    E_MDRV_SCLHVSP_MCNR_YCM_R  = 0x1,    ///< IP only read
    E_MDRV_SCLHVSP_MCNR_YCM_W  = 0x2,    ///< IP only write
    E_MDRV_SCLHVSP_MCNR_YCM_RW = 0x3,    ///< IP R/W
    E_MDRV_SCLHVSP_MCNR_CIIR_R = 0x4,    ///< IP only read
    E_MDRV_SCLHVSP_MCNR_CIIR_W = 0x8,    ///< IP only write
    E_MDRV_SCLHVSP_MCNR_CIIR_RW = 0xC,    ///< IP R/W
    E_MDRV_SCLHVSP_MCNR_NON = 0x10,    ///< IP none open
}MDrvSclHvspMcnrType_e;

typedef enum
{
    E_MDRV_SCLHVSP_IPMPATH_R  = 0x1,    ///< IP only read
    E_MDRV_SCLHVSP_IPMPATH_W  = 0x2,    ///< IP only write
    E_MDRV_SCLHVSP_IPMPATH_RW = 0x3,    ///< IP R/W
    E_MDRV_SCLHVSP_IPMPATH_NON = 0x10,    ///< IP none open
}MDrvSclHvspIpmPathType_e;


typedef enum
{
    E_MDRV_SCLHVSP_PAT_TGEN_TIMING_1920_1080_,   // FHD PT GEN
    E_MDRV_SCLHVSP_PAT_TGEN_TIMING_1024_768_6,    // HD PT gEN
    E_MDRV_SCLHVSP_PAT_TGEN_TIMING_640_480_60,     // SD PT GEN
    E_MDRV_SCLHVSP_PAT_TGEN_TIMING_UNDEFINED,      // random size
    E_MDRV_SCLHVSP_PAT_TGEN_TIMING_MAX,            // 4type
}MDrvSclHvspPatTgenTimingType_e;

typedef enum
{
    E_MDRV_SCLHVSP_MISC_CMD_SET_REG = 0,

    E_MDRV_SCLHVSP_MISC_CMD_MAX,           // MISC set 1 type
}MDrvSclHvspMiscCmdType_e;

typedef enum
{
    E_MDRV_SCLHVSP_OSD_LOC_AFTER  = 0,    ///< after hvsp
    E_MDRV_SCLHVSP_OSD_LOC_BEFORE = 1,    ///< before hvsp
}MDrvSclHvspOsdLocType_e;

typedef enum
{
    EN_MDRV_SCLHVSP_MONITOR_CROPCHECK  = 1,    ///< crop
    E_MDRV_SCLHVSP_MONITOR_DMA1FRMCHECK = 2,    ///< dma1frm
    E_MDRV_SCLHVSP_MONITOR_DMA1SNPCHECK = 3,    ///< dma1frm
    E_MDRV_SCLHVSP_MONITOR_DMA2FRMCHECK = 4,    ///< dma1frm
    E_MDRV_SCLHVSP_MONITOR_DMA3FRMCHECK = 5,    ///< dma1frm
}MDrvSclHvspMonitorType_e;

typedef enum
{
    E_MDRV_SCLHVSP_CALLPATGEN_STATIC  = 0,    ///< crop
    E_MDRV_SCLHVSP_CALLPATGEN_DYNAMIC = 1,    ///< dma1frm
}MDrvSclHvspCallPatTgenType_e;
typedef enum
{
    E_MDRV_SCLHVSP_FBMG_SET_LDCPATH_ON          = 0x1,
    E_MDRV_SCLHVSP_FBMG_SET_LDCPATH_OFF         = 0x2,
    E_MDRV_SCLHVSP_FBMG_SET_DNR_Read_ON         = 0x4,
    E_MDRV_SCLHVSP_FBMG_SET_DNR_Read_OFF        = 0x8,
    E_MDRV_SCLHVSP_FBMG_SET_DNR_Write_ON        = 0x10,
    E_MDRV_SCLHVSP_FBMG_SET_DNR_Write_OFF       = 0x20,
    E_MDRV_SCLHVSP_FBMG_SET_DNR_BUFFER_1        = 0x40,
    E_MDRV_SCLHVSP_FBMG_SET_DNR_BUFFER_2        = 0x80,
    E_MDRV_SCLHVSP_FBMG_SET_UNLOCK              = 0x100,
    E_MDRV_SCLHVSP_FBMG_SET_DNR_COMDE_ON        = 0x200,
    E_MDRV_SCLHVSP_FBMG_SET_DNR_COMDE_OFF       = 0x400,
    E_MDRV_SCLHVSP_FBMG_SET_DNR_COMDE_265OFF    = 0x800,
    E_MDRV_SCLHVSP_FBMG_SET_PRVCROP_ON          = 0x1000,
    E_MDRV_SCLHVSP_FBMG_SET_PRVCROP_OFF         = 0x2000,
}MDrvSclHvspFbmgSetType_e;
typedef struct
{
    u16 u16X;        ///< crop frame start x point
    u16 u16Y;        ///< crop frame start y point
    u16 u16Width;    ///< crop width size
    u16 u16Height;   ///< crop height size
}MDrvSclHvspWindowConfig_t;
typedef struct
{
    bool  bInterlace;      ///< is interlace or progressive
    u16 u16Htotal;       ///< Htt
    u16 u16Vtotal;       ///< Vtt
    u16 u16Vfrequency;   ///< Vfreq
}MDrvSclHvspTimingConfig_t;

typedef struct
{
    u32 u32Riubase;
    u32 u32IRQNUM;//scl
    u32 u32CMDQIRQNUM;//cmdq
}MDrvSclHvspInitConfig_t;


typedef struct
{
    u32 u32IRQNum;
    u32 u32CMDQIRQNum;//cmdq
}MDrvSclHvspSuspendResumeConfig_t;

typedef struct
{
    MDrvSclHvspMcnrType_e enRW;
    u32 u32YCPhyAddr;
    u32 u32MPhyAddr;
    u32 u32CIIRPhyAddr;
    u16 u16Width;
    u16 u16Height;
    u32  u32MemSize;
}MDrvSclHvspIpmConfig_t;

typedef struct
{
    bool  bEn;
    u16 u16X;
    u16 u16Y;
    u16 u16Width;
    u16 u16Height;
}MDrvSclHvspCropWindowConfig_t;
typedef struct
{
    void* idclk;
    void* fclk1;
    void* fclk2;
    void* odclk;
}MDrvSclHvspClkConfig_t;

typedef struct
{
    u16 u16Src_Width;
    u16 u16Src_Height;
    u16 u16Dsp_Width;
    u16 u16Dsp_Height;
    MDrvSclHvspCropWindowConfig_t stCropWin;
    MDrvSclHvspClkConfig_t       *stclk;      ///< clk framework
}MDrvSclHvspScalingConfig_t;

typedef struct
{
    bool bOSDEn;    ///< OSD en
    bool bOSDBypass;    ///< OSD en
    bool bWTMBypass;    ///< OSD en
}MDrvSclHvspOsdOnOffConfig_t;

typedef struct
{
    MDrvSclHvspOsdLocType_e enOSD_loc;    ///< OSD locate
    MDrvSclHvspOsdOnOffConfig_t stOsdOnOff;
}MDrvSclHvspOsdConfig_t;
typedef struct
{
    bool bMask; ///<Mask enable
    bool u8idx; ///<mask id
    MDrvSclHvspWindowConfig_t stMaskWin;      ///< Mask info
}MDrvSclHvspPriMaskConfig_t;

typedef struct
{
    MDrvSclHvspSrcType_e        enSrcType;
    MDrvSclHvspColorType_e      enColor;
    MDrvSclHvspWindowConfig_t   stCaptureWin;
    MDrvSclHvspTimingConfig_t    stTimingCfg;
    MDrvSclHvspClkConfig_t       *stclk;
}MDrvSclHvspInputConfig_t;
typedef struct
{
    bool  bCropEn;     ///< post crop En
    u16 u16X;        ///< crop frame start x point
    u16 u16Y;        ///< crop frame start y point
    u16 u16Width;    ///< crop frame width
    u16 u16Height;   ///< crop frame height
    bool  bFmCntEn;    ///< Is use CMDQ to set
    bool  u8FmCnt;     ///< when frame count
    MDrvSclHvspClkConfig_t       *stclk;  ///<clk framework
}MDrvSclHvspPostCropConfig_t;

typedef struct
{
    u16 u16X;            ///< x vs input src
    u16 u16Y;            ///< y vs input src
    u16 u16Width;        ///< display width
    u16 u16Height;       ///< display height
    u16 u16crop2inWidth; ///< framebuffer width
    u16 u16crop2inHeight;///< framebuffer height
    u16 u16crop2OutWidth; ///< after crop width
    u16 u16crop2OutHeight;///< after crop height
}MDrvSclHvspScInformConfig_t;
typedef struct
{
    bool   u8Cmd;      ///< register value
    u32   u32Size;    ///< number
    u32   u32Addr;    ///< bank&addr
}MDrvSclHvspMiscConfig_t;

//-------------------------------------------------------------------------------------------------
//  Prototype
//-------------------------------------------------------------------------------------------------
#ifndef _MDRV_HVSP_C
#define INTERFACE extern
#else
#define INTERFACE
#endif
INTERFACE bool MDrvSclHvspInit(MDrvSclHvspIdType_e enHVSP_ID, MDrvSclHvspInitConfig_t *pCfg);
INTERFACE bool MDrvSclHvspExit(bool bCloseISR);
INTERFACE void MDrvSclHvspRelease(MDrvSclHvspIdType_e enHVSP_ID);
INTERFACE void MDrvSclHvspSetPriMaskInstId(bool u8Id);
INTERFACE void MDrvSclHvspOpen(MDrvSclHvspIdType_e enHVSP_ID);
INTERFACE void MDrvSclVipSetMcnrConpressForDebug(bool bEn);
INTERFACE bool MDrvSclHvspSetOsdConfig(MDrvSclHvspIdType_e enHVSP_ID, MDrvSclHvspOsdConfig_t*pCfg);
INTERFACE bool MDrvSclHvspSetInputConfig(MDrvSclHvspIdType_e enHVSP_ID, MDrvSclHvspInputConfig_t *pCfg);
INTERFACE void MDrvSclHvspSetPatTgenStatus(bool bEn);
INTERFACE bool MDrvSclHvspSetScalingConfig(MDrvSclHvspIdType_e enHVSP_ID, MDrvSclHvspScalingConfig_t *pCfg );
INTERFACE bool MDrvSclHvspSetInitIpmConfig(MDrvSclHvspIdType_e enHVSP_ID, MDrvSclHvspIpmConfig_t *pCfg);
INTERFACE bool MDrvSclHvspSetMiscConfig(MDrvSclHvspMiscConfig_t *pCfg);
INTERFACE bool MDrvSclHvspSetPostCropConfig(MDrvSclHvspIdType_e enHVSP_ID, MDrvSclHvspPostCropConfig_t *pCfg);
INTERFACE void MDrvSclHvspSetMemoryAllocateReady(bool bEn);
INTERFACE bool MDrvSclHvspGetSCLInform(MDrvSclHvspIdType_e enHVSP_ID, MDrvSclHvspScInformConfig_t *pstCfg);
INTERFACE void MDrvSclHvspIdclkRelease(MDrvSclHvspClkConfig_t* stclk);
INTERFACE bool MDrvSclHvspSetMiscConfigForKernel(MDrvSclHvspMiscConfig_t *pCfg);
INTERFACE bool MDrvSclHvspInputVSyncMonitor(void);
INTERFACE bool MDrvSclHvspSuspend(MDrvSclHvspIdType_e enHVSP_ID, MDrvSclHvspSuspendResumeConfig_t *pCfg);
INTERFACE bool MDrvSclHvspResume(MDrvSclHvspIdType_e enHVSP_ID, MDrvSclHvspSuspendResumeConfig_t *pCfg);
INTERFACE ssize_t MDrvSclHvspProcShow(char *buf);

//#if defined (SCLOS_TYPE_LINUX_KERNEL)
INTERFACE ssize_t MDrvSclHvspClkFrameworkShow(char *buf,MDrvSclHvspClkConfig_t* stclk);
INTERFACE ssize_t MDrvSclHvspOsdShow(char *buf);
INTERFACE ssize_t MDrvSclHvspOdShow(char *buf);
INTERFACE ssize_t MDrvSclHvspFBMGShow(char *buf);
INTERFACE void MDrvSclHvspOsdStore(const char *buf,MDrvSclHvspIdType_e enHVSP_ID);
INTERFACE ssize_t MDrvSclHvspIntsShow(char *buf);
INTERFACE ssize_t MDrvSclHvspLockShow(char *buf);
INTERFACE ssize_t MDrvSclHvspScIqShow(char *buf, MDrvSclHvspIdType_e enHVSP_ID);
INTERFACE void MDrvSclHvspScIqStore(const char *buf,MDrvSclHvspIdType_e enHVSP_ID);
INTERFACE ssize_t MDrvSclHvspDbgmgFlagShow(char *buf);
INTERFACE ssize_t MDrvSclHvspmonitorHWShow(char *buf,int VsyncCount ,int MonitorErrCount);
INTERFACE void MDrvSclHvspSetClkForcemode(bool bEn);
INTERFACE void MDrvSclHvspSetClkOnOff(void* adjclk,bool bEn);
INTERFACE void MDrvSclHvspSetClkRate(void* adjclk,u8 u8Idx);
//#endif
INTERFACE bool MDrvSclHvspSetPollWait(DrvSclOsPollWaitConfig_t stPollWait);


INTERFACE void * MDrvSclHvspGetWaitQueueHead(void);
INTERFACE bool MDrvSclHvspGetCmdqDoneStatus(MDrvSclHvspIdType_e enHVSP_ID);
INTERFACE bool MDrvSclHvspSetFbManageConfig(MDrvSclHvspFbmgSetType_e enSet);
INTERFACE u8   MdrvSclHvspGetFrameBufferCountInformation(void);
INTERFACE bool MDrvSclHvspSetPriMaskConfig(MDrvSclHvspPriMaskConfig_t stCfg);
INTERFACE bool MDrvSclHvspSetPriMaskTrigger(bool bEn);

#undef INTERFACE
#endif
