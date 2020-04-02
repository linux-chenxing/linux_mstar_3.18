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
//==============================================================================
// [mvideo_context.h]
// Date: 20081203
// Descriptions: structure pool
//==============================================================================
#ifndef MVIDEO_CONTEXT_H
#define MVIDEO_CONTEXT_H

#include "MsCommon.h"

//==============================================================================
//==============================================================================
#ifdef MVIDEO_CONTEXT_C
#define INTERFACE
#else
#define INTERFACE extern
#endif

#define XC_CONTEXT_ENABLE   1
#define TEST_NEW_DYNAMIC_NR 0
#define FPGA                0

//#define MS_DEBUG 1

#ifdef MS_DEBUG
#ifdef MSOS_TYPE_CE
#define XC_LOG_TRACE(_dbgSwitch_,  _fmt, ...)                       \
    {                                                               \
        if( (_dbgSwitch_ & _u16XCDbgSwitch_InternalUseOnly) != 0 )  \
        {                                                           \
            MDrv_XC_ShowTrace_Header(_dbgSwitch_);                  \
            printf(_fmt, __VA_ARGS__);                              \
        }                                                           \
    }
#else
#define XC_LOG_TRACE(_dbgSwitch_,  _fmt, _args...)                  \
    {                                                               \
        if( (_dbgSwitch_ & _u16XCDbgSwitch_InternalUseOnly) != 0 )  \
        {                                                           \
            MDrv_XC_ShowTrace_Header(_dbgSwitch_);                  \
            printf("[%s,%5d] ",__FUNCTION__,__LINE__);                  \
            printf(_fmt, ##_args);                                  \
        }                                                           \
    }
#endif

#else
#ifdef MSOS_TYPE_CE
#define XC_LOG_TRACE(_dbgSwitch_,  _fmt, ...)
#else
#define XC_LOG_TRACE(_dbgSwitch_,  _fmt, _args...)
#endif
#endif

/// frame/field number stored in frame buffer
typedef enum
{
    IMAGE_STORE_UNDEFINE = 0,
    IMAGE_STORE_2_FIELDS = 1,
    IMAGE_STORE_4_FIELDS,
    IMAGE_STORE_6_FIELDS,
    IMAGE_STORE_8_FIELDS,
    IMAGE_STORE_10_FIELDS,
    IMAGE_STORE_12_FIELDS,
    IMAGE_STORE_14_FIELDS,
    IMAGE_STORE_16_FIELDS,
    IMAGE_STORE_18_FIELDS,
    IMAGE_STORE_20_FIELDS,

    IMAGE_STORE_2_FRAMES = 100,
    IMAGE_STORE_3_FRAMES,
    IMAGE_STORE_4_FRAMES,
    IMAGE_STORE_5_FRAMES,
    IMAGE_STORE_6_FRAMES,
    IMAGE_STORE_7_FRAMES,
    IMAGE_STORE_8_FRAMES,
    IMAGE_STORE_10_FRAMES,
    IMAGE_STORE_12_FRAMES
} XC_FRAME_STORE_NUMBER;


////===========================>>SW DB Buffer
typedef struct
{
    MS_U16 u16H_CapStart;
    MS_U16 u16H_CapSize;
    MS_U16 u16V_CapStart;
    MS_U16 u16V_CapSize;

    MS_U16 u16H_DisStart;
    MS_U16 u16H_DisEnd;
    MS_U16 u16V_DisStart;
    MS_U16 u16V_DisEnd;

    MS_U32 u32H_PreScalingRatio;
    MS_U32 u32V_PreScalingRatio;
    MS_U32 u32H_PostScalingRatio;
    MS_U32 u32V_PostScalingRatio;

    MS_U16 u16VLen;
    MS_U16 u16VWritelimit;

    MS_U16 u16DNROffset;
    MS_U16 u16DNRFetch;
    MS_U16 u16OPMOffset;
    MS_U16 u16OPMFetch;
    MS_U16 u16LBOffset;
    MS_U16 u16DispOffset;
    MS_U16 u16AlignWidth;

    MS_U32 u32WritelimitBase;
    MS_U32 u32DNRBase0;
    MS_U32 u32DNRBase1;
    MS_U32 u32DNRBase2;

    MS_U32 u32OPMBase0;
    MS_U32 u32OPMBase1;
    MS_U32 u32OPMBase2;
}SC_SWDB_INFO, *P_SC_SWDB_INFO;

typedef struct
{
    // XC need below information do to frame lock
    MS_U16 u16HStart;           ///<DE H start
    MS_U16 u16VStart;           ///<DE V start
    MS_U16 u16Width;            ///<DE H width
    MS_U16 u16Height;           ///< DE V height
    MS_U16 u16HTotal;           ///<H total
    MS_U16 u16VTotal;           ///<V total
    MS_U16 u16VFreq;            ///<Panel output Vfreq
}XC_PANEL_TIMING, *PXC_PANEL_TIMING;

typedef struct
{
    //-------------------------
    // customized pre scaling
    //-------------------------
    MS_BOOL bPreHCusScaling;            ///<assign pre H customized scaling instead of using XC scaling
    MS_U16  u16PreHCusScalingSrc;       ///<pre H customized scaling src width
    MS_U16  u16PreHCusScalingDst;       ///<pre H customized scaling dst width
    MS_BOOL bPreVCusScaling;            ///<assign pre V manuel scaling instead of using XC scaling
    MS_U16  u16PreVCusScalingSrc;       ///<pre V customized scaling src height
    MS_U16  u16PreVCusScalingDst;       ///<pre V customized scaling dst height

    MS_BOOL bAutoBestPreScaling;

    //-------------------------
    // memory format
    //-------------------------
    XC_FRAME_STORE_NUMBER eFrameStoreNumber;    ///< the number of frame/field stored in frame bufffer

    MS_U16 u16LBOffset;                 ///<line buffer offset
    MS_U16 u16OutputVFreqAfterFRC;
    E_XC_SOURCE_TO_VE eSourceToVE;
    E_XC_OUTPUT_COLOR_TYPE enOutputColorType;
    XC_PANEL_TIMING stXCPanelDefaultTiming;
} XC_InternalStatus2, *P_XC_InternalStatus2;

/// XC internal status
typedef struct
{
    //----------------------
    // Customer setting
    //----------------------
    INPUT_SOURCE_TYPE_t enInputSourceType;      ///< Input source type

    //----------------------
    // Window
    //----------------------
    MS_WINDOW_TYPE stCapWin;       ///<Capture window
    MS_WINDOW_TYPE stDispWin;      ///<Display window
    MS_WINDOW_TYPE stCropWin;      ///<Crop window

    //----------------------
    // Timing
    //----------------------
    MS_BOOL bInterlace;             ///<Interlaced or Progressive
    MS_BOOL bHDuplicate;            ///<flag for vop horizontal duplicate, for MVD, YPbPr, indicate input double sampled or not
    MS_U16  u16InputVFreq;          ///<Input V Frequency, VFreqx10, for calculate output panel timing
    MS_U16  u16InputVTotal;         ///<Input Vertical total, for calculate output panel timing
    MS_U16  u16DefaultHtotal;       ///<Default Htotal for VGA/YPbPr input
    MS_U8   u8DefaultPhase;         ///<Default Phase for VGA/YPbPr input

    //----------------------
    // customized post scaling
    //----------------------
    MS_BOOL bHCusScaling;           ///<assign H customized scaling instead of using XC scaling
    MS_U16  u16HCusScalingSrc;      ///<H customized scaling src width
    MS_U16  u16HCusScalingDst;      ///<H customized scaling dst width
    MS_BOOL bVCusScaling;           ///<assign V manuel scaling instead of using XC scaling
    MS_U16  u16VCusScalingSrc;      ///<V customized scaling src height
    MS_U16  u16VCusScalingDst;      ///<V customized scaling dst height

    //--------------
    // 9 lattice
    //--------------
    MS_BOOL bDisplayNineLattice;    ///<used to indicate where to display in panel and where to put in frame buffer

    //----------------------
    // XC internal setting
    //----------------------

    /* scaling ratio */
    MS_U16 u16H_SizeAfterPreScaling;         ///<Horizontal size after prescaling
    MS_U16 u16V_SizeAfterPreScaling;         ///<Vertical size after prescaling
    MS_BOOL bPreV_ScalingDown;      ///<define whether it's pre-Vertical scaling down
    MS_BOOL bPreScalingRatioChanged;        ///<True: PreScaling Ratio is different from with previous setWindow. False: PreScaling Ratio is same as previous one.

    /* real crop win in memory */
    MS_WINDOW_TYPE ScaledCropWin;

    /* others */
    MS_U32 u32Op2DclkSet;           ///<OP to Dot clock set

    /* Video screen status */
    MS_BOOL bBlackscreenEnabled;      ///<Black screen status
    MS_BOOL bBluescreenEnabled;       ///<Blue screen status
    MS_BOOL bAutoNoSignalEnabled;     ///< Auto No signal enabled
    MS_U16 u16VideoDark;            ///<Video dark

    MS_U16 u16V_Length;             ///<for MDrv_Scaler_SetFetchNumberLimit
    MS_U16 u16BytePerWord;          ///<BytePerWord in Scaler
    MS_U16 u16OffsetPixelAlignment; ///<Pixel alignment of Offset (including IPM/OPM)
    MS_U8 u8BitPerPixel;            ///<Bits number Per Pixel
    MS_DEINTERLACE_MODE eDeInterlaceMode;       ///<De-interlace mode
    MS_U8 u8DelayLines;             ///<delay lines
    MS_BOOL bMemFmt422;             ///<whether memory format is 422, for easy usage
    MS_IMAGE_STORE_FMT eMemory_FMT; ///<memory format
    MS_BOOL bForceNRoff;            ///<Force NR off
    MS_BOOL bEn3DNR;                ///<whether it's 3DNR enabled
    MS_BOOL bUseYUVSpace;           ///< color format before 3x3 matrix
    MS_BOOL bMemYUVFmt;             ///< memroy color format
    MS_BOOL bForceRGBin;            ///<Force set format in memory as RGB (only for RGB input source)
    MS_BOOL bLinearMode;            ///<Is current memory format LinearMode?
    MS_BOOL bRWBankAuto;            ///<accounding to framebuffer ,Auto adjust read/write bank mapping mode

    // frame lock related
    // only main can select FB/FBL because panel output timing depends on main window, sub will always use FB
    MS_BOOL bFBL;                   ///<whether it's framebufferless case
    MS_BOOL bR_FBL;                   ///<whether it's request framebufferless case
    MS_BOOL bFastFrameLock;         ///<whether framelock is enabled or not
    MS_BOOL bDoneFPLL;              ///<whether the FPLL is done
    MS_BOOL bEnableFPLL;            ///<enable FPLL or not
    MS_BOOL bFPLL_LOCK;             ///<whether FPLL is locked (in specific threshold.)

    // Capture_Memory
    MS_U32 u32IPMBase0;             ///<IPM base 0
    MS_U32 u32IPMBase1;             ///<IPM base 1
    MS_U32 u32IPMBase2;             ///<IPM base 2
    MS_U16 u16IPMOffset;            ///<IPM offset
    MS_U16 u16IPMFetch;             ///<IPM fetch

    //-------------------------------------------------------
    // To keep interface compatibility, please don't add any
    // new members to XC_InternalStatus.
    // Please add to XC_InternalStatus2.
    //-------------------------------------------------------
    XC_InternalStatus2 Status2;
}XC_InternalStatus, *P_XC_InternalStatus;

typedef struct
{
    MS_S32 s32MutexCnt;
    MS_S32 s32CurThreadId;
    MS_S32 s32ComMutex;
}XC_Mutex;

typedef struct
{
    XC_InternalStatus StatusInfo[MAX_WINDOW];
}XC_Context_Shared;

typedef struct
{
    XC_Mutex Mutex;
    XC_Context_Shared *pCommInfo;

    // FPLL related
    MS_BOOL bEnableFPLLManualDebug;     ///< if enabled, no set to phase limit, limitD5D6D7RK
}XC_Context;

INTERFACE MS_BOOL g_bIsIMMESWITCH;
INTERFACE MS_BOOL g_bIMMESWITCH_DVI_POWERSAVING;
INTERFACE MS_BOOL g_bDVI_AUTO_EQ;


// XC Init
#ifdef MSOS_TYPE_CE

#ifdef ASIC_VERIFY

	INTERFACE XC_INITDATA g_XC_InitData;
    INTERFACE MS_U16 _u16XCDbgSwitch_InternalUseOnly;


	#if (XC_CONTEXT_ENABLE)

	INTERFACE XC_Context *g_pSContext;
	#define gSrcInfo  (g_pSContext->pCommInfo->StatusInfo)

	#else

	INTERFACE XC_InternalStatus  gSrcInfo[MAX_WINDOW];

	#endif

#else

	#ifdef MVIDEO_CONTEXT_C

		#if (XC_CONTEXT_ENABLE)

		#pragma data_seg(".XCTex")
		INTERFACE   XC_INITDATA g_XC_InitData = {0};
		INTERFACE 	XC_Context 	*g_pSContext  = NULL;
		INTERFACE MS_U16 _u16XCDbgSwitch_InternalUseOnly = 0;
		#pragma data_seg()
		#pragma comment(linker,"/SECTION:.XCTex,RWS")

		#define gSrcInfo  (g_pSContext->pCommInfo->StatusInfo)

		#else

		#pragma data_seg(".XCTex")
		INTERFACE   XC_INITDATA g_XC_InitData      = {0};
		INTERFACE 	XC_InternalStatus  gSrcInfo[MAX_WINDOW] = {0};
		INTERFACE MS_U16 _u16XCDbgSwitch_InternalUseOnly = 0;
		#pragma data_seg()
		#pragma comment(linker,"/SECTION:.XCTex,RWS")

		#endif


	#elif (1==WITHIN_MDRV_XC_DLL)

		INTERFACE	XC_INITDATA  g_XC_InitData;
        INTERFACE MS_U16 _u16XCDbgSwitch_InternalUseOnly;

		#if (XC_CONTEXT_ENABLE)
		INTERFACE 	XC_Context 	*g_pSContext;
		#else
		INTERFACE 	XC_InternalStatus  gSrcInfo[MAX_WINDOW];
		#endif

		#define gSrcInfo  (g_pSContext->pCommInfo->StatusInfo)

	#else

		__declspec(dllimport)	XC_INITDATA  g_XC_InitData;
        __declspec(dllimport)   MS_U16 _u16XCDbgSwitch_InternalUseOnly;

		#if (XC_CONTEXT_ENABLE)
		__declspec(dllimport)	XC_Context 	*g_pSContext;
		#else
		__declspec(dllimport)	XC_InternalStatus  gSrcInfo[MAX_WINDOW];
		#endif

		#define gSrcInfo  (g_pSContext->pCommInfo->StatusInfo)

	#endif
#endif

#else

	INTERFACE XC_INITDATA g_XC_InitData;
    INTERFACE MS_U16 _u16XCDbgSwitch_InternalUseOnly;

	#if (XC_CONTEXT_ENABLE)

	INTERFACE XC_Context *g_pSContext;
	#define gSrcInfo  (g_pSContext->pCommInfo->StatusInfo)

	#else

	INTERFACE XC_InternalStatus  gSrcInfo[MAX_WINDOW];

	#endif

#endif

// XC Init MISC
INTERFACE XC_INITMISC g_XC_Init_Misc;

//SC

INTERFACE SC_SWDB_INFO stDBreg;         ///< SW double buffer

INTERFACE MS_WINDOW_TYPE g_RepWindow;
INTERFACE MS_BOOL g_bEnableRepWindowForFrameColor;

//FRC
INTERFACE XC_PREINIT_INFO_t g_XC_Pnl_Misc;

//EURO HDTV support flag
INTERFACE MS_BOOL _bEnableEuro;
INTERFACE void MDrv_XC_ShowTrace_Header(MS_U16 u16LogSwitch);

INTERFACE void mvideo_sc_variable_init(MS_BOOL bFirstInstance, XC_INITDATA *pXC_InitData);
INTERFACE void mvideo_sc_getcropwin( MS_WINDOW_TYPE *pCropWin, SCALER_WIN eWindow );
INTERFACE MS_U8  mvideo_pnl_is_dualport(void);
INTERFACE void mvideo_pnl_set_dclkmode(E_XC_PNL_OUT_TIMING_MODE enPnl_Out_Timing_Mode);

INTERFACE MS_BOOL mvideo_sc_is_enable_3dnr(SCALER_WIN eWindow);
INTERFACE void mvideo_sc_set_3dnr(MS_BOOL bEnable, SCALER_WIN eWindow);

INTERFACE void MDrv_XC_GetInfo(XC_ApiInfo *pXC_Info);
INTERFACE void MDrv_XC_SetHdmiSyncMode(E_HDMI_SYNC_TYPE eSynctype);
INTERFACE void MDrv_XC_Switch_DE_HV_Mode_By_Timing(SCALER_WIN eWindow);

//DLC
INTERFACE void MDrv_DLC_Handler(MS_BOOL bWindow);
INTERFACE MS_BOOL MDrv_DLC_GetHistogram(MS_BOOL bWindow);
INTERFACE MS_U8 MDrv_DLC_GetAverageValue(void);
INTERFACE MS_U16 MDrv_XC_PCMonitor_Get_Vtotal(SCALER_WIN eWindow);

INTERFACE MS_BOOL MDrv_XC_Get_SCMI_Type(void);

#undef INTERFACE
#endif /* MVIDEO_CONTEXT_H */

