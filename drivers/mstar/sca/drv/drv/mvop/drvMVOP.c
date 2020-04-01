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

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// drvMVOP.c
/// @brief MPEG Video Output Processor (MVOP)
/// @author MStarSemi Inc.
///
/// Display controller(DC) unit (also called Video Output Processor(VOP)).
/// DC read DRAM MVD decode data and send it to back-end processor.
/// The frame rate could be programmed by registers.
/// The sync. generator(SG) generates the SYNC signal like HSYNC, VSYNC, FIELD.
/// It also generate the standard CCIR656 data.
///
/// Features
///  - Output mode : interlace or progressive
///  - Maximum Display resolution 1920x1080
///  - Support MVD interlace or progressive YUV420 image
///  - one read channel to MIU ,  each request always work with 16 words( 8Y8UV)
///  - high MIU read priority when UV buffer data depth under 32 or 16.
///  - CCIR656 support
///  - Horizontal, Vertical duplicate support.
///  - MVD Image size resize mode
///
///////////////////////////////////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
// Common Definition
#ifdef MSOS_TYPE_LINUX_KERNEL
#include <linux/string.h>
#include <linux/kernel.h>
#else
#include <string.h>
#endif

#include "MsCommon.h"
#include "MsVersion.h"
#include "drvMVOP.h"

// Internal Definition
#include "regMVOP.h"

#define MVOP_ENABLE_LINK_WEAK_SYMBOL    1
#if MVOP_ENABLE_LINK_WEAK_SYMBOL
#include "_drvMVOP_weak.h"
#else
#include "halMVOP.h"
#endif

#include "drvMMIO.h"
#include "drvMIU.h"


//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------
//#define _ENABLE_DC1_FRAMESYNC //Notice: Scaler should disable auto-detection when using this.


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define ENABLE_EXTRA_BLANKING     0
// define MAX blanking area reserved for video shift down right for frame bufferless usage
#if ENABLE_EXTRA_BLANKING
    #define EXTRA_H_BLANKING          0
    #define EXTRA_V_BLANKING          64
#else
    #define EXTRA_H_BLANKING          0
    #define EXTRA_V_BLANKING          0
#endif

#if (defined(CHIP_J2) || defined(CHIP_A1) || defined(CHIP_A5) || defined(CHIP_A7) || defined(CHIP_A3) || defined(CHIP_AMETHYST) || defined(CHIP_AGATE) || defined(CHIP_C3))
#define ENABLE_3D_LR_MODE         1 //Enable 3D L/R feature
#else
#define ENABLE_3D_LR_MODE         0 //Disable 3D L/R feature or not support
#endif

#if (defined(CHIP_MARIA10)||defined(CHIP_T12) || defined(CHIP_T13) || defined(CHIP_J2) || defined(CHIP_A1) || defined(CHIP_A5) || defined(CHIP_A6) || defined(CHIP_A7) || defined(CHIP_A3) || defined(CHIP_AMETHYST) || defined(CHIP_AGATE) || defined(CHIP_EAGLE) || defined (CHIP_C3))
#define MVOP_SUPPORT_MIRROR       1
#else
#define MVOP_SUPPORT_MIRROR       0
#endif

#if (defined(CHIP_T12) || defined(CHIP_T13) || defined(CHIP_J2) || defined(CHIP_A1) || defined(CHIP_A5) || defined(CHIP_A6) || defined(CHIP_A7) || defined(CHIP_A3) || defined(CHIP_AMETHYST) || defined(CHIP_AGATE) || defined(CHIP_EAGLE) || defined(CHIP_C3))
#define MVOP_SUPPORT_OPENMAX       1
#else
#define MVOP_SUPPORT_OPENMAX       0
#endif

#if (defined(CHIP_U4) || defined(CHIP_K1) || defined(CHIP_K2))
#define STB_DC 1
#else
#define STB_DC 0
#endif

#define MVOP_VBlank_Min 45
#define MVOP_HBlank_Min 32

#define MVOP_VBlank               45
#define MVOP_HBlank_SD            150
#define MVOP_HBlank_HD            300
#define DC_BaseClock              86400 //86.4*1000

#define MIN_MVOP_FREQ             16000000ul
#define MAX_MVOP_FREQ             86400000ul
#define MVOP_FREQ_144M           144000000ul
#define MVOP_FREQ_160M           160000000ul
#define MIN_MVOP_FREERUN_FREQ     31000000ul

#define MVOP_MIN_FRAMERATE       1000   //1 frame/second

#define MVOP_VBlank_Min_P 20
#define MVOP_VBlank_Min_I 35

#define ALIGN_UPTO_16(x)  ((((x) + 15) >> 4) << 4)
#define ALIGN_DOWNTO_8(x) (((x) >> 3) << 3)

//-------------------------------------------------------------------------------------------------
//  Local Structurs
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
/// Version string
static MSIF_Version _drv_mvop_version = {
#if defined(MSOS_TYPE_CE) || defined(DISABLE_C99) || defined(MSOS_TYPE_LINUX_KERNEL)
    { MVOP_DRV_VERSION },
#else
    .DDI = { MVOP_DRV_VERSION },
#endif
};

typedef MS_BOOL (*HAL_SetU16Para)(MS_U16 u16Para);
typedef void (*HAL_RegSetBase)(MS_U32 u32Base);

#define HAL_MVOP_MODULE_CNT 3
typedef struct
{
    MVOP_DrvStatus _stMVOPDrvStat;
    MVOP_Timing    _mvopTiming;
    MS_U16         _u16FixVtt;
    MS_U16         _u16SrcHSize;        //source width (may be unaligned)
    MS_U16         _u16InsVBlank;       //inserted V-Blank between 3D L & R
    MS_BOOL        _bEnableFreerunMode;
    MS_BOOL        _bEnMinVSize576;     //used by MVOP_AutoGenMPEGTiming
    HAL_SetU16Para fpHalSet3DLRInsVBlank;
} MVOP_DrvCtrlSt;
static MVOP_DrvCtrlSt _stMVOPDrvCtrl[HAL_MVOP_MODULE_CNT];

static MVOP_DrvInfo _stMVOPDrvInfo;
#if STB_DC
static MS_BOOL _bRegSizeFromMVDStatus =TRUE;
static MS_U16 _u16Width = 0;
static MS_U16 _u16Height = 0;
#endif
static MS_U8 _u8DebugMsgLevel = 0;


#define MVOP_GetDrvCtrl(_eDevID) ((_eDevID < HAL_MVOP_MODULE_CNT) ? (&_stMVOPDrvCtrl[(_eDevID)]) : NULL)

#define MVOP_IsInit() (_stMVOPDrvCtrl[E_MVOP_DEV_0]._stMVOPDrvStat.bIsInit == TRUE)
#define MVOP_CheckIsInit()                                                     \
        do {                                                                   \
            if (!MVOP_IsInit())                                                \
            {                                                                  \
                MVOP_DBG(printf("%s: drvMVOP is not init!\n", __FUNCTION__);)  \
                return;                                                        \
            }                                                                  \
        } while(0)

#if (defined(CHIP_T12) || defined(CHIP_J2) || defined(CHIP_A5) || defined(CHIP_A3) || defined(CHIP_AGATE) || defined(CHIP_C3)) //For chips that support two MVOP modules.
#define MVOP_SUPPORT_SUB 1
#else
#define MVOP_SUPPORT_SUB 0
#endif

#if (defined(CHIP_AGATE) || defined(CHIP_C3))
#define MVOP_SUPPORT_3RD 1
#else
#define MVOP_SUPPORT_3RD 0
#endif

#if 1//MVOP_SUPPORT_SUB
#define MVOP_SubIsInit() (_stMVOPDrvCtrl[E_MVOP_DEV_1]._stMVOPDrvStat.bIsInit == TRUE)
#define MVOP_SubCheckIsInit()                                                     \
        do {                                                                      \
            if (!MVOP_SubIsInit())                                                \
            {                                                                     \
                MVOP_DBG(printf("%s: drvMVOPSub is not init!\n", __FUNCTION__);)  \
                return;                                                           \
            }                                                                     \
        } while(0)
#define MVOP_SubCheckIsInit_RetZero()                                                     \
        do {                                                                      \
            if (!MVOP_SubIsInit())                                                \
            {                                                                     \
                MVOP_DBG(printf("%s: drvMVOPSub is not init!\n", __FUNCTION__);)  \
                return 0;                                                         \
            }                                                                     \
        } while(0)
#endif

#if 1//MVOP_SUPPORT_3RD
#define MVOP_EX_IsInit(_eDevID) ((MVOP_GetDrvCtrl(_eDevID)) ? (_stMVOPDrvCtrl[_eDevID]._stMVOPDrvStat.bIsInit == TRUE)  : FALSE)
#define MVOP_EX_CheckIsInit(_eDevID)                                              \
        do {                                                                      \
            if (!MVOP_EX_IsInit(_eDevID))                                         \
            {                                                                     \
                MVOP_DBG(printf("%s: drvMVOP3rd is not init!\n", __FUNCTION__);)  \
                return E_MVOP_NOT_INIT;                                           \
            }                                                                     \
        } while(0)
#endif

#if (defined(CHIP_T12) || defined(CHIP_T13) || defined(CHIP_J2) || defined(CHIP_A1) || defined(CHIP_A5) || defined(CHIP_A7) || defined(CHIP_A3) || defined(CHIP_AMETHYST) || defined(CHIP_AGATE) || defined(CHIP_EAGLE))
#define _MVOP_128BIT_BUS    //128-bit MIU bus
#endif

//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------
#define MVOP_DBG(x)                if (_u8DebugMsgLevel>0)  { x };
#define MVOP_TIMING_DBG(x)         if (_u8DebugMsgLevel>1)  { x };
#define MVOP_INPUT_DBG(x)          if (_u8DebugMsgLevel>2)  { x };
#define MVOP_DBG_VERBAL(x)         if (_u8DebugMsgLevel>3)  { x };
#define MVOP_ERR(x)                { x };

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
static void MVOP_InitVar(MVOP_DevID eID)
{
    MVOP_DrvCtrlSt* pstDrvCtrl = MVOP_GetDrvCtrl(eID);
    MVOP_DrvStatus* pstDrvStat = NULL;
#if defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_LINUX_KERNEL)
	HAL_SetU16Para fpSetU16Para = NULL;
#endif


    if (pstDrvCtrl)
    {
        pstDrvStat = &pstDrvCtrl->_stMVOPDrvStat;
        pstDrvStat->bIsInit    = FALSE;
        pstDrvStat->bIsEnable  = FALSE;
        pstDrvStat->bIsUVShift = FALSE;
        pstDrvStat->bIsBlackBG = FALSE;
        pstDrvStat->bIsMonoMode = FALSE;
        pstDrvStat->bIsSetTiming = FALSE;
        pstDrvCtrl->_u16SrcHSize = 0;
        pstDrvCtrl->_u16InsVBlank= 0;
#if !defined(MSOS_TYPE_CE) && !defined(MSOS_TYPE_LINUX_KERNEL)
		HAL_SetU16Para fpSetU16Para = NULL;
#endif
#if !defined(CHIP_A3) && !defined(CHIP_C3)
        switch (eID)
        {
            case E_MVOP_DEV_0:
                fpSetU16Para = HAL_MVOP_Set3DLRInsVBlank;
                break;
            case E_MVOP_DEV_1:
                fpSetU16Para = HAL_MVOP_SubSet3DLRInsVBlank;
                break;
            case E_MVOP_DEV_2:
                fpSetU16Para = HAL_MVOP_EX_Set3DLRInsVBlank;
                break;
            default:
                fpSetU16Para = HAL_MVOP_Set3DLRInsVBlank;
                break;
        }
#endif
        if (fpSetU16Para)
        {
            pstDrvCtrl->fpHalSet3DLRInsVBlank = fpSetU16Para;
        }
    }
}

static MS_BOOL MVOP_SetRegBaseAdd(HAL_RegSetBase fpRegSetBase)
{
    MS_U32 u32NonPMBankSize, u32NonPMBankAdd;
    if (TRUE == MDrv_MMIO_GetBASE(&u32NonPMBankAdd, &u32NonPMBankSize, MS_MODULE_MVOP))
    {
        MVOP_DBG(printf("%s u32RiuBaseAdd = %lx\n", __FUNCTION__, u32NonPMBankAdd););
        if (fpRegSetBase)
        {
            fpRegSetBase(u32NonPMBankAdd);
            return TRUE;
        }
        else
        {
            MVOP_DBG(printf("%s IOMap failure due to null fptr\n", __FUNCTION__););
        }
    }
    else
    {
        MVOP_DBG(printf("%s IOMap failure due to MDrv_MMIO_GetBASE\n", __FUNCTION__););
    }
    return FALSE;
}

static MS_U16 MVOP_CalculateHSize(MS_U16* pu16SrcHSize, MS_U16 u16OriHSize, MS_U16 u16HOffset)
{
    MS_U16 u16HSize = 0;

    if (pu16SrcHSize == NULL)
    {
        return 0;
    }

#ifdef _MVOP_128BIT_BUS
    u16HSize = ALIGN_UPTO_16(u16OriHSize);
    if (u16HSize != u16OriHSize)
    {
        *pu16SrcHSize = u16OriHSize;
    }
#else
    u16HSize = ALIGN_DOWNTO_8(u16OriHSize);
#endif

#ifdef _SUPPORT_IMG_OFFSET_
    u16HSize += (u16HOffset * 2);
#endif

    return u16HSize;
}

static MS_U16 MVOP_CalculateVSize(MS_U16 u16OriVSize, MS_U16 u16VOffset)
{
    MS_U16 u16VSize = 0;

    u16VSize = u16OriVSize;

#ifdef _SUPPORT_IMG_OFFSET_
    u16VSize += (u16VOffset * 2);
#endif

    return u16VSize;
}

//-------------------------------------------------------------------------------------------------
// Calculate MaxFactor.
// @return MaxFactor
//-------------------------------------------------------------------------------------------------
static MS_U32 _MaxFactor(MS_U32 u32Num1, MS_U32 u32Num2)
{
    int remainder, quotient;

    MVOP_DBG_VERBAL(printf("u32Num1=%lx u32Num2=%lx\n", u32Num1, u32Num2);)
    MS_ASSERT(u32Num1 != 0);
    if (u32Num1 == 0) //no need to check u32Num2 because u32Num2 is always 86400
    {
        MVOP_ERR(printf("%s(%d): invalid parameter!\n", __FILE__, __LINE__);)
        return 0; //to avoid infinite loop
    }

    while (1)
    {
        if (u32Num1 > u32Num2)
        {
            if ((u32Num1%u32Num2) == 0)
            {
                MVOP_DBG_VERBAL(printf("ret0 u32Num1=%lx u32Num2=%lx\n", u32Num1, u32Num2);)
                return u32Num2;
            }
            else
            {
                quotient = u32Num1/u32Num2;
                remainder = u32Num1- (u32Num2*quotient);

                u32Num1= u32Num2;
                u32Num2= remainder;
            }
        }
        else
        {
            if ((u32Num2%u32Num1) == 0)
            {
                MVOP_DBG_VERBAL(printf("ret1 u32Num1=%lx u32Num2=%lx\n", u32Num1, u32Num2);)
                return u32Num1;
            }
            else
            {
                quotient = u32Num2/u32Num1;
                remainder = u32Num2 - (u32Num1*quotient);

                u32Num2 = u32Num1;
                u32Num1 = remainder;
            }
        }
    }
}


static void MVOP_AutoGenTiming(MS_BOOL bIsMain,
                                MVOP_Timing *pVOPTiming,
                                MVOP_VidStat *pstVideoStatus)
{
#ifdef _ENABLE_DC1_FRAMESYNC
    MS_U32 u32R, u32Q;
    MS_U32 u32MaxFactor;
    MS_U32 MVOP_Clock;
#endif
    MS_U16 u16HSize;
    MS_U16 u16VSize;
    MS_U16 bInterlace;
    MS_U16 VB0, VB1;
    MS_U16 u8VSyncOffset;
    MS_S16 iVBlank = 0;
#if defined( MSOS_TYPE_CE) || defined(MSOS_TYPE_LINUX_KERNEL)
	MS_U16 *pu16SrcHSize = NULL;
#endif

    u8VSyncOffset = 10;
#if !defined( MSOS_TYPE_CE) && !defined(MSOS_TYPE_LINUX_KERNEL)
	MS_U16 *pu16SrcHSize = NULL;
#endif

    if (bIsMain)
    {
        pu16SrcHSize = &_stMVOPDrvCtrl[E_MVOP_DEV_0]._u16SrcHSize;
    }
#if MVOP_SUPPORT_SUB
    else
    {
        pu16SrcHSize = & _stMVOPDrvCtrl[E_MVOP_DEV_1]._u16SrcHSize;
    }
#endif
    u16HSize = MVOP_CalculateHSize(pu16SrcHSize, pstVideoStatus->u16HorSize, pstVideoStatus->u16HorOffset);
    u16VSize = MVOP_CalculateVSize(pstVideoStatus->u16VerSize, pstVideoStatus->u16VerOffset);
    pVOPTiming->u16Width  = u16HSize;
    pVOPTiming->u16Height = u16VSize;

    bInterlace = pstVideoStatus->u8Interlace;

    MS_ASSERT(pstVideoStatus->u16FrameRate!=0);
    if(pstVideoStatus->u16FrameRate == 0)// for parasoft check divide by zero.
    {
        return;
    }

    {   //to avoid (Vtotal-Vde)>(Vde/10) for scaler FBL mode.
        MS_U16* pu16FixVtt = & _stMVOPDrvCtrl[E_MVOP_DEV_0]._u16FixVtt;
        if(*pu16FixVtt > 0)
        {
            pVOPTiming->u16V_TotalCount = *pu16FixVtt;
            *pu16FixVtt = 0;
        }
        else
        {
            pVOPTiming->u16V_TotalCount = u16VSize + MVOP_VBlank;
        }
        if (bIsMain)
        {
            pVOPTiming->u16H_TotalCount = (MS_U16)((MS_U32)(HAL_MVOP_GetMaxFreerunClk()) / pstVideoStatus->u16FrameRate
                                           *1000 / pVOPTiming->u16V_TotalCount);
        }
        else
        {
#if MVOP_SUPPORT_SUB
            pVOPTiming->u16H_TotalCount = (MS_U16)((MS_U32)(HAL_MVOP_SubGetMaxFreerunClk()) / pstVideoStatus->u16FrameRate
                                           *1000 / pVOPTiming->u16V_TotalCount);
#else
            MVOP_ERR(printf("Err: Not support SubMVOP!\n");)
#endif
        }
        pVOPTiming->u16H_TotalCount = (pVOPTiming->u16H_TotalCount + 0x1) & ~0x1;
        pVOPTiming->u16HActive_Start = pVOPTiming->u16H_TotalCount - u16HSize;
        //printf("=== Htt=%d Has=%d Hsize=%d\n", pVOPTiming->u16H_TotalCount, pVOPTiming->u16HActive_Start, u16HSize);
        //Aiken's protection for FBL ZOOM, Horizontal Blanking can not less than MVOP_HBlank_Min
        if((int)pVOPTiming->u16HActive_Start < MVOP_HBlank_Min) // avoid unsigned rounding, may be negative
        {
            MVOP_ERR(printf(" Error !! Horizontal Blanking can not less than %d \n", MVOP_HBlank_Min);)
            pVOPTiming->u16HActive_Start = MVOP_HBlank_Min;
            pVOPTiming->u16H_TotalCount =  pVOPTiming->u16HActive_Start + u16HSize;
            if (bIsMain)
            {
                pVOPTiming->u16V_TotalCount = (MS_U16)((MS_U32)(HAL_MVOP_GetMaxFreerunClk()) / pstVideoStatus->u16FrameRate * 1000 / pVOPTiming->u16H_TotalCount);
            }
            else
            {
#if MVOP_SUPPORT_SUB
                pVOPTiming->u16V_TotalCount = (MS_U16)((MS_U32)(HAL_MVOP_SubGetMaxFreerunClk()) / pstVideoStatus->u16FrameRate * 1000 / pVOPTiming->u16H_TotalCount);
#else
                MVOP_ERR(printf("Err: Not support SubMVOP!\n");)
#endif
            }
            iVBlank = pVOPTiming->u16V_TotalCount - u16VSize;
            // interlace video need ensure thant VB = VB0 + VB1
            // here, also need ensure vop's freqency should less than VOP_FREQ_144M/VOP_FREQ_160M
            if(bInterlace && ((iVBlank & 0x1) == 0))
            {
                pVOPTiming->u16V_TotalCount--;
                iVBlank--;
            }
            if(iVBlank < MVOP_VBlank_Min)
            {
                MVOP_ERR(printf(" Error !! Vertical Blanking can not less than %d \n", MVOP_VBlank_Min);)
                u16VSize = u16VSize - (MVOP_VBlank_Min - iVBlank);
                pVOPTiming->u16Height = u16VSize;
            }
        }
#ifdef _SUPPORT_IMG_OFFSET_
        pVOPTiming->u16HImg_Start = pVOPTiming->u16HActive_Start + pstVideoStatus->u16HorOffset;
        //printf("=== His=%d\n", pVOPTiming->u16HImg_Start);
#endif
        MS_ASSERT(pVOPTiming->u16H_TotalCount < 4095);
    }

    pVOPTiming->bInterlace = (MS_BOOL)(pstVideoStatus->u8Interlace);
    if (bInterlace)
    {
        VB0 = (pVOPTiming->u16V_TotalCount - u16VSize) / 2 + 1; // VOP_VBlank/2+1 + EXTRA_V_BLANKING/2;
        VB1 = VB0 - 1;

#if ENABLE_3D_LR_MODE
        if (ENABLE == HAL_MVOP_Get3DLRMode())
        {
            VB1 = VB0;
            pVOPTiming->u16V_TotalCount++;
            MVOP_DBG(printf("%s 3DLR interlace: force VB0=VB1=0x%x, Vtt+1=0x%x\n", __FUNCTION__, VB1, pVOPTiming->u16V_TotalCount);)
        }
#endif

        pVOPTiming->u16VBlank0_Start = 1;
        pVOPTiming->u16VBlank0_End   = pVOPTiming->u16VBlank0_Start + VB0;
        pVOPTiming->u16VBlank1_Start = pVOPTiming->u16VBlank0_End + (u16VSize/2);
        pVOPTiming->u16VBlank1_End   = pVOPTiming->u16VBlank1_Start + VB1;
    }
    else
    {
        VB1 = VB0 = (pVOPTiming->u16V_TotalCount - u16VSize);   //VB1 = VB0 for progressive
        pVOPTiming->u16VBlank0_Start = 1;
        pVOPTiming->u16VBlank0_End   = pVOPTiming->u16VBlank0_Start + VB0;
        pVOPTiming->u16VBlank1_Start = 1;
        pVOPTiming->u16VBlank1_End   = pVOPTiming->u16VBlank1_Start + VB1;
    }
#ifdef _SUPPORT_IMG_OFFSET_
    if (bInterlace)
    {
        pVOPTiming->u16VImg_Start0 = pVOPTiming->u16VBlank0_End + pstVideoStatus->u16VerOffset/2;
        pVOPTiming->u16VImg_Start1 = pVOPTiming->u16VBlank1_End + pstVideoStatus->u16VerOffset/2;
    }
    else
    {
        pVOPTiming->u16VImg_Start0 = pVOPTiming->u16VBlank0_End + pstVideoStatus->u16VerOffset;
        pVOPTiming->u16VImg_Start1 = pVOPTiming->u16VBlank1_End + pstVideoStatus->u16VerOffset;
    }
#endif
    pVOPTiming->u16TopField_VS = pVOPTiming->u16VBlank0_Start + u8VSyncOffset;
    pVOPTiming->u16BottomField_VS = pVOPTiming->u16VBlank1_Start + u8VSyncOffset;
    if (pVOPTiming->bInterlace)
    {
#if ENABLE_3D_LR_MODE
        //Do not adjust BottomField_VStart for 3D L/R interlace source
        if (ENABLE != HAL_MVOP_Get3DLRMode())
#endif
        {
            pVOPTiming->u16BottomField_VS -=1;
        }
    }

    pVOPTiming->u16TopField_Start = pVOPTiming->u16TopField_VS;
    pVOPTiming->u16BottomField_Start = pVOPTiming->u16BottomField_VS;

    // u8Framerate & u16H_Freq will be assigned to g_SrcInfo.*
    // in other place and cannot be removed
    pVOPTiming->u8Framerate = pstVideoStatus->u16FrameRate/1000;
    pVOPTiming->u16ExpFrameRate = pstVideoStatus->u16FrameRate;

    // u16H_Freq (unit: 100Hz)
    pVOPTiming->u16H_Freq = (MS_U16)
        ((MS_U32)pVOPTiming->u16V_TotalCount *
         (MS_U32)pstVideoStatus->u16FrameRate/100000);

#ifndef _ENABLE_DC1_FRAMESYNC
    // for dc1 ...
    pVOPTiming->u16Num = 1;
    pVOPTiming->u16Den = 1;
#else   //calculate clock for dc1 synthesizer
    //calculate clock
    u32R = (MS_U32)pVOPTiming->u16H_TotalCount*(MS_U32)pVOPTiming->u16V_TotalCount%1000;
    u32Q = (MS_U32)pVOPTiming->u16H_TotalCount*(MS_U32)pVOPTiming->u16V_TotalCount/1000;
    u32R *= pstVideoStatus->u16FrameRate;
    u32Q *= pstVideoStatus->u16FrameRate;
    u32Q += u32R/1000;
    u32Q /= 1000; // u32Q is the result
    MVOP_Clock = u32Q;
    u32MaxFactor = _MaxFactor(MVOP_Clock, DC_BaseClock);

    if (u32MaxFactor == 1)
    {
        pVOPTiming->u16Num = (MS_U16)(MVOP_Clock/2);
        pVOPTiming->u16Den = (MS_U16)(DC_BaseClock/2);
    }
    else if (u32MaxFactor != 0)
    {
        pVOPTiming->u16Num = (MS_U16)(MVOP_Clock/u32MaxFactor);
        pVOPTiming->u16Den = (MS_U16)(DC_BaseClock/u32MaxFactor);
    }
    else
    {
        pVOPTiming->u16Num = 0;
        pVOPTiming->u16Den = 0;
    }
#endif

    MVOP_TIMING_DBG(printf("\n>>>>> MVOP output 50/60\n");)
    MVOP_TIMING_DBG(printf(">>>>> w=%d, h=%d, i=%d, fr=%d\n", pVOPTiming->u16Width,
                     pVOPTiming->u16Height, pVOPTiming->bInterlace, pVOPTiming->u16ExpFrameRate);)
    MVOP_TIMING_DBG(printf(">>>>> hTotal=%d, vTotal=%d\n",
                     pVOPTiming->u16H_TotalCount, pVOPTiming->u16V_TotalCount);)
    MVOP_TIMING_DBG(printf(">>>>> hActive=%d VB0=%d VB1=%d\n",
                     pVOPTiming->u16HActive_Start, VB0, VB1);)
    MVOP_TIMING_DBG(printf(">>>>> ImgHSt=%d ImgVSt0=%d ImgVSt1=%d\n", pVOPTiming->u16HImg_Start, pVOPTiming->u16VImg_Start0, pVOPTiming->u16VImg_Start1);)
    MVOP_TIMING_DBG(printf(">>>>> hOffset=%d vOffset=%d\n", pstVideoStatus->u16HorOffset, pstVideoStatus->u16VerOffset);)
}


static MS_BOOL MVOP_AutoGenMPEGTiming(MS_BOOL bIsMain,
                                MVOP_Timing *pVOPTiming,
                                MVOP_VidStat *pstVideoStatus,
                                MS_BOOL bEnHDup)
{
    MS_U32 u32R, u32Q;
    MS_U32 u32MaxFactor;
    MS_U32 MVOP_Clock;
    MS_U16 u16HSize;
    MS_U16 u16VSize;
    MS_U16 bInterlace;
    MS_U16 VB0, VB1;
    MS_U16 V_Blanking = MVOP_VBlank+EXTRA_V_BLANKING;
    MS_U16 u8VSyncOffset;
    MS_U16 u16MinVBlank = 0;
#ifdef _SUPPORT_IMG_OFFSET_
    MS_U16 u16HMaxOffset = 0;
    MS_U16 u16VMaxOffset = 0;
#endif
    MS_U16* pu16FixVtt = NULL;
    MS_BOOL* pbEnMinVSize576 = NULL;
    MS_BOOL bFreerunMode = FALSE;
#if defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_LINUX_KERNEL)
    MS_U16* pu16SrcHSize = NULL;
#endif

    if (bIsMain)
    {
        pu16FixVtt = &_stMVOPDrvCtrl[E_MVOP_DEV_0]._u16FixVtt;
        pbEnMinVSize576 = &_stMVOPDrvCtrl[E_MVOP_DEV_0]._bEnMinVSize576;
    }
    else
    {
#if MVOP_SUPPORT_SUB
        pu16FixVtt = &_stMVOPDrvCtrl[E_MVOP_DEV_1]._u16FixVtt;
        pbEnMinVSize576 = & _stMVOPDrvCtrl[E_MVOP_DEV_1]._bEnMinVSize576;
#else
        MVOP_ERR(printf("Err no sub-mvop!\n");)
        return FALSE;
#endif
    }

    MVOP_TIMING_DBG(printf(">>>>> MVOP in\n");)
    MVOP_TIMING_DBG(printf(">>> HOffset=%d VOffset=%d fixVtt=%d EnMinVSize576=%x\n",
                     pstVideoStatus->u16HorOffset, pstVideoStatus->u16VerOffset,
                     *pu16FixVtt, *pbEnMinVSize576);)
    MVOP_TIMING_DBG(printf(">>>>> w=%d, h=%d, i=%d, fr=%d\n",
                     pstVideoStatus->u16HorSize, pstVideoStatus->u16VerSize,
                     pstVideoStatus->u8Interlace, pstVideoStatus->u16FrameRate);)

    MS_ASSERT(pstVideoStatus->u16FrameRate != 0);
    if (pstVideoStatus->u16FrameRate == 0)
    {
        MVOP_TIMING_DBG(printf(">>>>> MVOP fr==0!\n");)
        return FALSE;
    }

#if ENABLE_3D_LR_MODE
    if (ENABLE == HAL_MVOP_Get3DLRMode())
    {
        pstVideoStatus->u16VerSize *= 2;
        MVOP_TIMING_DBG(printf(">>>>> 3D L/R enable, multiple VSize=%d\n", pstVideoStatus->u16VerSize);)
    }
#endif

#ifdef _SUPPORT_IMG_OFFSET_
    if ((pstVideoStatus->u16HorOffset%2) != 0)
    {
        //HorOffset needs to be even, or video color will be very strange.
        pstVideoStatus->u16HorOffset += 1;
        MVOP_TIMING_DBG(printf(">>>> Change HOffset as %d \n\n", pstVideoStatus->u16HorOffset);)
    }
    u16HMaxOffset = MDrv_MVOP_GetMaxHOffset(pstVideoStatus->u16HorSize,pstVideoStatus->u16VerSize,pstVideoStatus->u16FrameRate);
    if (bIsMain)
    {
        u16VMaxOffset = MDrv_MVOP_GetMaxVOffset(pstVideoStatus->u16HorSize,pstVideoStatus->u16VerSize,pstVideoStatus->u16FrameRate);
    }
    else
    {
        u16VMaxOffset = MDrv_MVOP_SubGetMaxVOffset(pstVideoStatus->u16HorSize,pstVideoStatus->u16VerSize,pstVideoStatus->u16FrameRate);
    }
    if (pstVideoStatus->u16HorOffset > u16HMaxOffset)
    {
        MVOP_TIMING_DBG(printf("Change HorOffset %d --> %d\n", pstVideoStatus->u16HorOffset, u16HMaxOffset);)
        pstVideoStatus->u16HorOffset = u16HMaxOffset;
    }
    if (pstVideoStatus->u16VerOffset > u16VMaxOffset)
    {
        MVOP_TIMING_DBG(printf("Change u16VerOffset %d --> %d\n", pstVideoStatus->u16VerOffset, u16VMaxOffset);)
        pstVideoStatus->u16VerOffset = u16VMaxOffset;
    }
#endif

#if !defined(MSOS_TYPE_CE) && !defined(MSOS_TYPE_LINUX_KERNEL)
    MS_U16* pu16SrcHSize = NULL;
#endif
    if (bIsMain)
    {
        pu16SrcHSize = &_stMVOPDrvCtrl[E_MVOP_DEV_0]._u16SrcHSize;
    }
#if MVOP_SUPPORT_SUB
    else
    {
        pu16SrcHSize = &_stMVOPDrvCtrl[E_MVOP_DEV_1]._u16SrcHSize;
    }
#endif
    u16HSize = MVOP_CalculateHSize(pu16SrcHSize, pstVideoStatus->u16HorSize, pstVideoStatus->u16HorOffset);
    u16VSize = MVOP_CalculateVSize(pstVideoStatus->u16VerSize, pstVideoStatus->u16VerOffset);

    if ((TRUE == *pbEnMinVSize576) && (u16VSize < 576) && (576 - u16VSize > V_Blanking))
    {
        V_Blanking = 576 - u16VSize;
    }

    if (bIsMain)
    {
        _stMVOPDrvCtrl[E_MVOP_DEV_0]._stMVOPDrvStat.bIsSetTiming = TRUE;
    }
#if MVOP_SUPPORT_SUB
    else
    {
        _stMVOPDrvCtrl[E_MVOP_DEV_1]._stMVOPDrvStat.bIsSetTiming = TRUE;
    }
#endif

    if(pstVideoStatus->u8Interlace)
    {
        u16MinVBlank = MVOP_VBlank_Min_I;
    }
    else
    {
        u16MinVBlank = MVOP_VBlank_Min_P;
    }

    if((u16VSize >= *pu16FixVtt) || ((*pu16FixVtt - u16VSize) < u16MinVBlank))
    {
        // Restore to original flow
        *pu16FixVtt = 0;
        MVOP_DBG(printf("fix vtt feature is not enabled\n");)
    }

    if(bEnHDup && (u16HSize < 704))
    {
        u16HSize *= 2;
        pVOPTiming->bHDuplicate = 1;
    }
    else
    {
        pVOPTiming->bHDuplicate = 0;
    }

    pVOPTiming->u16Width  = u16HSize;
    pVOPTiming->u16Height = u16VSize;

    bInterlace = pstVideoStatus->u8Interlace;

    if (u16HSize > 720) // HD
    {
        pVOPTiming->u16H_TotalCount = u16HSize + MVOP_HBlank_HD+EXTRA_H_BLANKING*2;
        pVOPTiming->u16HActive_Start = MVOP_HBlank_HD+EXTRA_H_BLANKING*2;
    }
    else    //SD
    {
        pVOPTiming->u16H_TotalCount = u16HSize + MVOP_HBlank_SD+EXTRA_H_BLANKING*2;
        pVOPTiming->u16HActive_Start = MVOP_HBlank_SD+EXTRA_H_BLANKING*2;
    }
    if(*pu16FixVtt > 0)
    {
        pVOPTiming->u16V_TotalCount = *pu16FixVtt;
    }
    else
    {
        pVOPTiming->u16V_TotalCount = u16VSize + V_Blanking;

#if ENABLE_3D_LR_MODE
        if (ENABLE == HAL_MVOP_Get3DLRMode())
        {
            MS_U16 u16InsVBlank = _stMVOPDrvCtrl[E_MVOP_DEV_0]._u16InsVBlank;
            if  (u16InsVBlank != 0)
            {
                pVOPTiming->u16V_TotalCount += u16InsVBlank;
                if (_stMVOPDrvCtrl[E_MVOP_DEV_0].fpHalSet3DLRInsVBlank)
                {
                    _stMVOPDrvCtrl[E_MVOP_DEV_0].fpHalSet3DLRInsVBlank(u16InsVBlank);
                }
                MVOP_DBG(printf(">>>>> 3D L/R enable, Vtt => %d, InsVB=%d\n", pVOPTiming->u16V_TotalCount, u16InsVBlank);)
            }
        }
#endif

    }

    u32Q = (MS_U32)pVOPTiming->u16H_TotalCount * (MS_U32)pVOPTiming->u16V_TotalCount;
    u32R = u32Q % 1000;
    u32Q = u32Q / 1000;
    MVOP_Clock = u32Q * (MS_U32)pstVideoStatus->u16FrameRate +
                u32R * (MS_U32)pstVideoStatus->u16FrameRate / 1000;

    MVOP_TIMING_DBG(printf(">>>>> MVOP_Clock=%ld, Htt=%d, Vtt=%d\n", MVOP_Clock,
                    pVOPTiming->u16H_TotalCount, pVOPTiming->u16V_TotalCount);)

    if (MVOP_Clock > MAX_MVOP_FREQ)
    {
        MVOP_DBG(printf("drvMVOP: Enable CLK2!\n");)
        if (bIsMain)
        {
            HAL_MVOP_SetEnable60P(TRUE);
            MDrv_MVOP_EnableFreerunMode(FALSE);
        }
#if MVOP_SUPPORT_SUB
        else
        {
            HAL_MVOP_SubSetEnable60P(TRUE);
            MDrv_MVOP_SubEnableFreerunMode(FALSE);
        }
#endif
        MVOP_AutoGenTiming(bIsMain, pVOPTiming, pstVideoStatus);
        return TRUE;
    }
    else
    {
        if (bIsMain)
        {
            HAL_MVOP_SetEnable60P(FALSE);
        }
#if MVOP_SUPPORT_SUB
        else
        {
            HAL_MVOP_SubSetEnable60P(FALSE);
        }
#endif
        if(*pu16FixVtt > 0)
        {
            *pu16FixVtt = 0;
        }
    }

    bFreerunMode = (bIsMain && _stMVOPDrvCtrl[E_MVOP_DEV_0]._bEnableFreerunMode) ||
                                  ((!bIsMain) && _stMVOPDrvCtrl[E_MVOP_DEV_1]._bEnableFreerunMode);
    if (((!bFreerunMode) && (MVOP_Clock < MIN_MVOP_FREQ))
        || (bFreerunMode && (MVOP_Clock < MIN_MVOP_FREERUN_FREQ))
        )
    {
        MS_U32 u32MinMvopFreq = 0;
        u32MaxFactor = (MS_U32)(pstVideoStatus->u16FrameRate / 1000)*
                         (MS_U32)pVOPTiming->u16V_TotalCount;

        if (u32MaxFactor == 0)
        {
            MVOP_ERR(printf("%s(%d) u32MaxFactor==0!\n", __FUNCTION__, __LINE__);)
            return FALSE;
        }

        if(bFreerunMode)
        {
            u32MinMvopFreq = MIN_MVOP_FREERUN_FREQ;
        }
        else
        {
            u32MinMvopFreq = MIN_MVOP_FREQ;
        }

        u32Q = u32MinMvopFreq / u32MaxFactor;
        u32R = u32MinMvopFreq % u32MaxFactor;
        pVOPTiming->u16H_TotalCount = (MS_U16)(u32Q);
        if (u32R != 0)
            pVOPTiming->u16H_TotalCount++;

        pVOPTiming->u16H_TotalCount = (pVOPTiming->u16H_TotalCount + 0x1) & ~0x1;

        if(pVOPTiming->u16H_TotalCount > 4094)
        {
            pVOPTiming->u16H_TotalCount = 4094;
            u32MaxFactor = (MS_U32)(pstVideoStatus->u16FrameRate / 1000)*
                             (MS_U32)pVOPTiming->u16H_TotalCount;
            u32Q = u32MinMvopFreq / u32MaxFactor;
            u32R = u32MinMvopFreq % u32MaxFactor;
            pVOPTiming->u16V_TotalCount = (MS_U16)(u32Q);
            if (u32R != 0)
                pVOPTiming->u16V_TotalCount++;

        }

        pVOPTiming->u16HActive_Start = pVOPTiming->u16H_TotalCount - u16HSize;
    }
    MS_ASSERT(!bFreerunMode && (MVOP_Clock < MAX_MVOP_FREQ));

#ifdef _SUPPORT_IMG_OFFSET_
    pVOPTiming->u16HImg_Start = pVOPTiming->u16HActive_Start + pstVideoStatus->u16HorOffset;
#endif

    if (bInterlace)
    {
        // interlace video need ensure thant VB = VB0 + VB1
        if(((pVOPTiming->u16V_TotalCount - u16VSize) & 0x1) == 0)
        {
            pVOPTiming->u16V_TotalCount++;
        }
        VB0 = (pVOPTiming->u16V_TotalCount - u16VSize) / 2 + 1;
        VB1 = VB0 - 1;
#if ENABLE_3D_LR_MODE
        if (ENABLE == HAL_MVOP_Get3DLRMode())
        {
            VB1 = VB0;
            pVOPTiming->u16V_TotalCount++;
            MVOP_DBG(printf("%s 3DLR interlace: force VB0=VB1=0x%x, Vtt+1=0x%x\n", __FUNCTION__, VB1, pVOPTiming->u16V_TotalCount);)
        }
#endif
        pVOPTiming->u16VBlank0_Start = 1;
        pVOPTiming->u16VBlank0_End   = pVOPTiming->u16VBlank0_Start + VB0;
        pVOPTiming->u16VBlank1_Start = pVOPTiming->u16VBlank0_End + (u16VSize/2);
        pVOPTiming->u16VBlank1_End   = pVOPTiming->u16VBlank1_Start + VB1;
        pVOPTiming->bInterlace = TRUE;
    }
    else
    {
        VB1 = VB0 = (pVOPTiming->u16V_TotalCount - u16VSize);
        pVOPTiming->u16VBlank0_Start = 1;
        pVOPTiming->u16VBlank0_End   = pVOPTiming->u16VBlank0_Start + VB0;
        pVOPTiming->u16VBlank1_Start = 1;
        pVOPTiming->u16VBlank1_End   = pVOPTiming->u16VBlank1_Start + VB1;
        pVOPTiming->bInterlace = FALSE;
    }

#ifdef _SUPPORT_IMG_OFFSET_
    if(bInterlace)
    {
        pVOPTiming->u16VImg_Start0 = pVOPTiming->u16VBlank0_End + pstVideoStatus->u16VerOffset / 2;
        pVOPTiming->u16VImg_Start1 = pVOPTiming->u16VBlank1_End + pstVideoStatus->u16VerOffset / 2;
    }
    else
    {
        pVOPTiming->u16VImg_Start0 = pVOPTiming->u16VBlank0_End + pstVideoStatus->u16VerOffset;
        pVOPTiming->u16VImg_Start1 = pVOPTiming->u16VBlank1_End + pstVideoStatus->u16VerOffset;
    }
#endif

    u8VSyncOffset = VB0/2;
    pVOPTiming->u16TopField_VS = pVOPTiming->u16VBlank0_Start + u8VSyncOffset;
    pVOPTiming->u16BottomField_VS = pVOPTiming->u16VBlank1_Start + u8VSyncOffset;
    if (pVOPTiming->bInterlace)
    {
#if ENABLE_3D_LR_MODE
        if (ENABLE != HAL_MVOP_Get3DLRMode())
#endif
        {
            pVOPTiming->u16BottomField_VS -=1;
        }
    }

    pVOPTiming->u16TopField_Start = pVOPTiming->u16TopField_VS;
    pVOPTiming->u16BottomField_Start = pVOPTiming->u16BottomField_VS;

    // u8Framerate & u16H_Freq will be assigned to g_SrcInfo.*
    // in other place and cannot be removed
    pVOPTiming->u8Framerate = pstVideoStatus->u16FrameRate/1000;
    pVOPTiming->u16ExpFrameRate = pstVideoStatus->u16FrameRate;

    // u16H_Freq (unit: 100Hz)
    pVOPTiming->u16H_Freq = (MS_U16)
        ((MS_U32)pVOPTiming->u16V_TotalCount *
         (MS_U32)pstVideoStatus->u16FrameRate/100000);

    //calculate clock
    u32R = (MS_U32)pVOPTiming->u16H_TotalCount*(MS_U32)pVOPTiming->u16V_TotalCount%1000;
    u32Q = (MS_U32)pVOPTiming->u16H_TotalCount*(MS_U32)pVOPTiming->u16V_TotalCount/1000;
    u32R *= pstVideoStatus->u16FrameRate; // frameratex1000
    u32Q *= pstVideoStatus->u16FrameRate; // frameratex1000
    u32Q += u32R/1000;
    u32Q /= 1000; // u32Q is the result
    MVOP_Clock = u32Q;
    u32MaxFactor = _MaxFactor(MVOP_Clock, DC_BaseClock);

    if (u32MaxFactor == 1)
    {
        pVOPTiming->u16Num = (MS_U16)(MVOP_Clock/2);
        pVOPTiming->u16Den = (MS_U16)(DC_BaseClock/2);
    }
    else if (u32MaxFactor != 0)
    {
        pVOPTiming->u16Num = (MS_U16)(MVOP_Clock/u32MaxFactor);
        pVOPTiming->u16Den = (MS_U16)(DC_BaseClock/u32MaxFactor);
    }
    else
    {
        pVOPTiming->u16Num = 0;
        pVOPTiming->u16Den = 0;
    }

    MVOP_TIMING_DBG(printf("\n>>>>> MVOP output\n");)
    MVOP_TIMING_DBG(printf(">>>>> w=%d, h=%d, i=%d, fr=%d\n", pVOPTiming->u16Width, pVOPTiming->u16Height,
        pVOPTiming->bInterlace, pVOPTiming->u16ExpFrameRate);)
    MVOP_TIMING_DBG(printf(">>>>> hTotal=%d, vTotal=%d\n", pVOPTiming->u16H_TotalCount, pVOPTiming->u16V_TotalCount);)
    MVOP_TIMING_DBG(printf(">>> ImgHSt=%d ImgVSt0=%d ImgVSt1=%d\n", pVOPTiming->u16HImg_Start, pVOPTiming->u16VImg_Start0, pVOPTiming->u16VImg_Start1);)
    return TRUE;
}

//Notice: This function is not supported for T2.
static MVOP_Result MVOP_MiuSwitch(MS_BOOL bIsMain, MS_U8 u8Miu)
{
    eMIUClientID eCid = MVOP_MIU_CLIENT_MAIN;
    eMIU_SelType eMiuSel = MIU_SELTYPE_MIU1;

    if (!bIsMain)
    {
        eCid = MVOP_MIU_CLIENT_SUB;
    }

    if (u8Miu == 0)
    {
        eMiuSel = MIU_SELTYPE_MIU0;
    }
    else if (u8Miu == 1)
    {
        if (!MDrv_MIU_IsSupportMIU1())
        {
            return E_MVOP_INVALID_PARAM;
        }
        eMiuSel = MIU_SELTYPE_MIU1;
    }
    else
    {
        return E_MVOP_INVALID_PARAM;
    }

    MVOP_DBG(printf("MVOP_MiuSwitch eCid=0x%x, eMiuSel=0x%x\n", eCid, eMiuSel);)
    MDrv_MIU_SelMIU(eCid, eMiuSel);

    return E_MVOP_OK;
}

static MS_BOOL MVOP_GetStatus(MVOP_DevID eID, MVOP_DrvStatus *pMVOPStat)
{
    MVOP_DrvStatus *pDrvStat = &_stMVOPDrvCtrl[eID]._stMVOPDrvStat;
    if ((pMVOPStat ==  NULL) || (pDrvStat ==  NULL))
    {
        return FALSE;
    }

    pMVOPStat->bIsInit = pDrvStat->bIsInit;
    pMVOPStat->bIsEnable = pDrvStat->bIsEnable;
    pMVOPStat->bIsUVShift = pDrvStat->bIsUVShift;
    pMVOPStat->bIsBlackBG = pDrvStat->bIsBlackBG;
    pMVOPStat->bIsMonoMode = pDrvStat->bIsMonoMode;
    return TRUE;
}

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
/// This function shows the version of MVOP driver.
///
/// @param *ppVersion : pointer to the driver version
////////////////////////////////////////////////////////////////////////////////
MVOP_Result MDrv_MVOP_GetLibVer(const MSIF_Version **ppVersion)
{
    if (!ppVersion)
        return E_MVOP_FAIL;

    *ppVersion = &_drv_mvop_version;
    return E_MVOP_OK;
}


/********************************************************************************/
/// Set detailed level of MVOP driver debug message (level)
/// This function is used for MVOP driver debug message setting
/// @param level  \b IN  level from 0 to 1
/********************************************************************************/
void MDrv_MVOP_SetDbgLevel(MS_U8 level)
{
    _u8DebugMsgLevel = level;
    return;
}

//-----------------------------------------------------------------------------
/// Get information of MVOP driver.
/// @return driver information
//-----------------------------------------------------------------------------
const MVOP_DrvInfo* MDrv_MVOP_GetInfo(void)
{
    _stMVOPDrvInfo.u32MaxSynClk = MAX_MVOP_FREQ;
    _stMVOPDrvInfo.u32MinSynClk = MIN_MVOP_FREQ;
    _stMVOPDrvInfo.u32MaxFreerunClk = (MS_U32)(HAL_MVOP_GetMaxFreerunClk());

    return (&_stMVOPDrvInfo);
}

//-----------------------------------------------------------------------------
/// Get status of MVOP driver
/// @param -pstatus: driver status
/// @return - TRUE / FALSE
//-----------------------------------------------------------------------------
MS_BOOL MDrv_MVOP_GetStatus(MVOP_DrvStatus *pMVOPStat)
{
    return MVOP_GetStatus(E_MVOP_DEV_0, pMVOPStat);
}

/********************************************************************************/
/// Get distance from HSync to DE for Scaler, unit: mvop clock cycle
/// @return HStart
/********************************************************************************/
MS_U16 MDrv_MVOP_GetHStart(void)
{
    return (_stMVOPDrvCtrl[E_MVOP_DEV_0]._mvopTiming.u16HActive_Start)/2 + 2;
}

/********************************************************************************/
/// Get distance from HSync to DE for Scaler, unit: mvop clock cycle
/// @return VStart
/********************************************************************************/
MS_U16 MDrv_MVOP_GetVStart(void)
{
    MS_U16 u16Vst;
    u16Vst = _stMVOPDrvCtrl[E_MVOP_DEV_0]._mvopTiming.u16VBlank0_End - _stMVOPDrvCtrl[E_MVOP_DEV_0]._mvopTiming.u16TopField_VS;
    if (_stMVOPDrvCtrl[E_MVOP_DEV_0]._mvopTiming.bInterlace==1)
    {
        u16Vst*=2;
        u16Vst-=18;
    }
    else
    {
        u16Vst-=5;
    }
    return u16Vst;
}

/******************************************************************************/
// Set MVOP clock enable
// @param bEnable \b IN
//   - # TRUE  Enable clock
//   - # FALSE Close clock
/******************************************************************************/
static void MDrv_MVOP_SetClk ( MS_BOOL bEnable )
{
    HAL_MVOP_SetDCClk(0, bEnable);  //Clk DC0
    HAL_MVOP_SetDCClk(1, bEnable);  //Clk DC1
}

//-----------------------------------------------------------------------------
/// Set MVOP test pattern.
//-----------------------------------------------------------------------------
void MDrv_MVOP_SetPattern(MVOP_Pattern enMVOPPattern)
{
    static MVOP_Pattern s_enMVOPPattern = MVOP_PATTERN_NORMAL;
    MVOP_CheckIsInit();
    if(enMVOPPattern == MVOP_PATTERN_DEFAULT)
    {
        enMVOPPattern = (MVOP_Pattern)(((int)(s_enMVOPPattern) + 1) % (int)MVOP_PATTERN_DEFAULT);
    }
    HAL_MVOP_SetPattern(enMVOPPattern);
    s_enMVOPPattern = enMVOPPattern;
}

//-----------------------------------------------------------------------------
/// Configure the tile format of the MVOP input.
/// @return TRUE or FALSE
//-----------------------------------------------------------------------------
MS_BOOL MDrv_MVOP_SetTileFormat(MVOP_TileFormat eTileFmt)
{
    if (!MVOP_IsInit())
    {
        return FALSE;
    }
    if ((eTileFmt != E_MVOP_TILE_8x32) && (eTileFmt != E_MVOP_TILE_16x32))
    {
        return FALSE;
    }
    return HAL_MVOP_SetTileFormat(eTileFmt);
}

//-----------------------------------------------------------------------------
/// Set MMIO Base for MVOP.
/// @return TRUE or FALSE
//-----------------------------------------------------------------------------
MS_BOOL MDrv_MVOP_SetMMIOMapBase(void)
{
    return MVOP_SetRegBaseAdd(HAL_MVOP_RegSetBase);
}

/******************************************************************************/
/// Initialize MVOP hardware and set it to hardwire mode
/******************************************************************************/
void MDrv_MVOP_Init(void)
{
    MVOP_InitVar(E_MVOP_DEV_0);

    if(MDrv_MVOP_SetMMIOMapBase() == FALSE)
    {
        return;
    }

    HAL_MVOP_Init();
    HAL_MVOP_SetInputMode( VOPINPUT_HARDWIRE, NULL );

    // Set fld inv & ofld_inv
    HAL_MVOP_SetFieldInverse(ENABLE, ENABLE);

    // Set Croma weighting off
    HAL_MVOP_SetChromaWeighting(ENABLE);
#if ENABLE_3D_LR_MODE
    HAL_MVOP_Enable3DLR(DISABLE);
#endif
    MDrv_MVOP_SetClk(TRUE);
    MDrv_MVOP_EnableFreerunMode(FALSE);
    HAL_MVOP_LoadReg();

     _stMVOPDrvCtrl[E_MVOP_DEV_0]._stMVOPDrvStat.bIsInit = TRUE;
}

/******************************************************************************/
/// Exit MVOP, turn off clock
/******************************************************************************/
void MDrv_MVOP_Exit()
{
    MVOP_CheckIsInit();
    MDrv_MVOP_SetClk(FALSE);
}

/******************************************************************************/
/// Enable and Disable MVOP
/// @param bEnable \b IN
///   - # TRUE  Enable
///   - # FALSE Disable and reset
/******************************************************************************/
void MDrv_MVOP_Enable ( MS_BOOL bEnable )
{
    MVOP_CheckIsInit();

    HAL_MVOP_SetMIUReqMask(TRUE);
    HAL_MVOP_Enable(bEnable);
    HAL_MVOP_SetMIUReqMask(FALSE);

     _stMVOPDrvCtrl[E_MVOP_DEV_0]._stMVOPDrvStat.bIsEnable = bEnable;
}


/******************************************************************************/
/// Get if MVOP is enable or not.
/// @param pbEnable \b OUT
///   - # TRUE  Enable
///   - # FALSE Disable
/******************************************************************************/
MVOP_Result MDrv_MVOP_GetIsEnable (MS_BOOL* pbEnable)
{
    if ((NULL == pbEnable) || (TRUE != MVOP_IsInit()))
    {
        return E_MVOP_FAIL;
    }

    *pbEnable = HAL_MVOP_GetEnableState();
    return E_MVOP_OK;
}

/******************************************************************************/
/// Set enable UVShift
/******************************************************************************/
void MDrv_MVOP_EnableUVShift(MS_BOOL bEnable)
{
    MVOP_CheckIsInit();
    HAL_MVOP_EnableUVShift(bEnable);
     _stMVOPDrvCtrl[E_MVOP_DEV_0]._stMVOPDrvStat.bIsUVShift = bEnable;
}

/******************************************************************************/
/// Set enable black background
/******************************************************************************/
void MDrv_MVOP_EnableBlackBG ( void )
{
    MVOP_CheckIsInit();
    HAL_MVOP_SetBlackBG();
     _stMVOPDrvCtrl[E_MVOP_DEV_0]._stMVOPDrvStat.bIsBlackBG = TRUE;
}

/******************************************************************************/
/// Set MVOP output timing
/// @param ptiming \b IN VOP timing setting
/******************************************************************************/
static void MVOP_DumpOutputTiming ( MVOP_Timing *ptiming )
{
    if (!ptiming)
    {
        return;
    }

    MVOP_DBG(printf("\nMVOP SetOutputTiming\n");)
    MVOP_DBG(printf(" VTot=%u,\t",ptiming->u16V_TotalCount);)
    MVOP_DBG(printf(" HTot=%u,\t",ptiming->u16H_TotalCount);)
    MVOP_DBG(printf(" I/P=%u\n",ptiming->bInterlace);)
    MVOP_DBG(printf(" W=%u,\t",ptiming->u16Width);)
    MVOP_DBG(printf(" H=%u,\t",ptiming->u16Height);)
    MVOP_DBG(printf(" FRate=%u,\t",ptiming->u8Framerate);)
    MVOP_DBG(printf(" HFreq=%u\n",ptiming->u16H_Freq);)
    MVOP_DBG(printf(" Num=0x%x,\t",ptiming->u16Num);)
    MVOP_DBG(printf(" Den=0x%x,\t",ptiming->u16Den);)
    MVOP_DBG(printf(" u16HImg_Start=%u\n",ptiming->u16HImg_Start);)
    MVOP_DBG(printf(" u16VImg_Start0=%u\n",ptiming->u16VImg_Start0);)
    MVOP_DBG(printf(" u16VImg_Start1=%u\n",ptiming->u16VImg_Start1);)
    MVOP_DBG(printf(" u16ExpFRate=%u #\n\n", ptiming->u16ExpFrameRate);)
}


/******************************************************************************/
/// MVOP mono mode (fix chroma value)
/******************************************************************************/
void MDrv_MVOP_SetMonoMode(MS_BOOL bEnable)
{
    MVOP_CheckIsInit();
    HAL_MVOP_SetMonoMode(bEnable);
     _stMVOPDrvCtrl[E_MVOP_DEV_0]._stMVOPDrvStat.bIsMonoMode = bEnable;
}

//-----------------------------------------------------------------------------
/// Configure MVOP input.
/// @return MVOP_Result
//-----------------------------------------------------------------------------
MVOP_Result MDrv_MVOP_SetInputCfg (MVOP_InputSel in, MVOP_InputCfg* pCfg)
{
    MVOP_Result ret = E_MVOP_OK;

    if (!MVOP_IsInit())
    {
        MVOP_DBG(printf("%s:MVOP driver is not initialized!\n", __FUNCTION__);)
        return E_MVOP_FAIL;
    }

    HAL_MVOP_SetMonoMode(FALSE);

    #if STB_DC
    return HAL_MVOP_SetInputCfg( in,  pCfg);
    #endif

    switch (in)
    {
        case MVOP_INPUT_DRAM:
            if (!pCfg)
            {
                return E_MVOP_INVALID_PARAM;
            }
            HAL_MVOP_SetInputMode(VOPINPUT_MCUCTRL, pCfg);
            break;

#if ENABLE_3D_LR_MODE
        case MVOP_INPUT_HVD_3DLR:
            HAL_MVOP_Enable3DLR(ENABLE);
            //no break here to continue setting HVD hardwire mode
#endif
        case MVOP_INPUT_H264:
            HAL_MVOP_SetInputMode(VOPINPUT_HARDWIRE, NULL);
            HAL_MVOP_SetH264HardwireMode();
            break;

#if ENABLE_3D_LR_MODE
        case MVOP_INPUT_MVD_3DLR:
            HAL_MVOP_Enable3DLR(ENABLE);
            //no break here to continue setting MVD hardwire mode
#endif
        case MVOP_INPUT_MVD:
            HAL_MVOP_SetInputMode(VOPINPUT_HARDWIRE, NULL);
            break;
        case MVOP_INPUT_RVD:
            HAL_MVOP_SetInputMode(VOPINPUT_HARDWIRE, NULL);
            HAL_MVOP_SetRMHardwireMode();
            break;
        case MVOP_INPUT_CLIP:
            if (!pCfg)
            {
                return E_MVOP_INVALID_PARAM;
            }
            HAL_MVOP_SetCropWindow(pCfg);
            break;
        case MVOP_INPUT_JPD:
            HAL_MVOP_SetInputMode(VOPINPUT_HARDWIRE, NULL);
            HAL_MVOP_SetJpegHardwireMode();
            break;
        default:
            ret = E_MVOP_INVALID_PARAM;
            break;
    }

    return ret;
}

static MS_BOOL MVOP_CheckOutputCfg(MVOP_VidStat *pstVideoStatus)
{
    if (pstVideoStatus == NULL)
    {
        MVOP_ERR(printf("%s invalid para NULL Ptr!\n", __FUNCTION__);)
        return FALSE;
    }

    //Designer suggests to set the minimum HSize as 16.
    //If supporting HSize<16, miu_request_timeout must be enabled.
    if ((pstVideoStatus->u16HorSize < 16) ||
        (pstVideoStatus->u16VerSize == 0) ||
        (pstVideoStatus->u16FrameRate < MVOP_MIN_FRAMERATE))
    {
        MVOP_ERR(printf("%s invalid para H=%d V=%d FR=%d\n", __FUNCTION__,
        pstVideoStatus->u16HorSize, pstVideoStatus->u16VerSize, pstVideoStatus->u16FrameRate);)
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

//-----------------------------------------------------------------------------
/// Configure MVOP output.
/// @return MVOP_Result
//-----------------------------------------------------------------------------
MVOP_Result MDrv_MVOP_SetOutputCfg(MVOP_VidStat *pstVideoStatus, MS_BOOL bEnHDup)
{
    MVOP_Timing* pstTiming = &_stMVOPDrvCtrl[E_MVOP_DEV_0]._mvopTiming;
    if (!MVOP_IsInit())
    {
        MVOP_DBG(printf("%s: driver is not initialized!\n", __FUNCTION__);)
        return E_MVOP_FAIL;
    }

    if (MVOP_CheckOutputCfg(pstVideoStatus) != TRUE)
    {
        MVOP_DBG(printf("%s: invalid parameter!\n", __FUNCTION__);)
        return E_MVOP_INVALID_PARAM;
    }

    if (MVOP_AutoGenMPEGTiming(TRUE, pstTiming, pstVideoStatus, bEnHDup) != TRUE)
    {
        MVOP_DBG(printf("%s: fail to calculate timing!\n", __FUNCTION__);)
        return E_MVOP_FAIL;
    }

    HAL_MVOP_SetOutputInterlace(pstTiming->bInterlace);
    HAL_MVOP_SetOutputTiming(pstTiming);
    HAL_MVOP_SetSynClk(pstTiming);

    if ( _stMVOPDrvCtrl[E_MVOP_DEV_0]._u16SrcHSize != 0)
    {
        //report the source width for scaler, intead of the 16-aligned width
        //MS_U16 u16Discard_Width = 0;
        //u16Discard_Width = _mvopTiming.u16Width - _u16SrcHSize;
        MVOP_DBG(printf("%s: _mvopTiming.u16Width:  %d ==> %d\n", __FUNCTION__,
                        pstTiming->u16Width, _stMVOPDrvCtrl[E_MVOP_DEV_0]._u16SrcHSize);)
        pstTiming->u16Width = _stMVOPDrvCtrl[E_MVOP_DEV_0]._u16SrcHSize;
#if 0   //for H-mirror ON
        if (pstTiming->u16HActive_Start > u16Discard_Width)
        {
            pstTiming->u16HActive_Start -= u16Discard_Width;
        }
        else
        {
            printf("Warning: u16HActive_Start(%d) <= u16Discard_Width(%d)\n",
                pstTiming->u16HActive_Start, u16Discard_Width);
        }
        if (pstTiming->u16HImg_Start > u16Discard_Width)
        {
            pstTiming->u16HImg_Start -= u16Discard_Width;
        }
        else
        {
            printf("Warning: u16HImg_Start(%d) <= u16Discard_Width(%d)\n",
                pstTiming->u16HImg_Start, u16Discard_Width);
        }
#endif
    }

    MVOP_DumpOutputTiming(pstTiming);
    return E_MVOP_OK;
}


//-----------------------------------------------------------------------------
/// Get MVOP output timing information.
/// @return MVOP_Result
//-----------------------------------------------------------------------------
MVOP_Result MDrv_MVOP_GetOutputTiming(MVOP_Timing* pMVOPTiming)
{
#if defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_LINUX_KERNEL)
	MVOP_Timing* pstTiming = NULL;
#endif

    if (!MVOP_IsInit())
    {
        MVOP_DBG(printf("%s: driver is not initialized!\n", __FUNCTION__);)
        return E_MVOP_FAIL;
    }

    if (!pMVOPTiming)
    {
        MVOP_DBG(printf("%s: invalid parameter!\n", __FUNCTION__);)
        return E_MVOP_INVALID_PARAM;
    }
#if defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_LINUX_KERNEL)
	pstTiming = &_stMVOPDrvCtrl[E_MVOP_DEV_0]._mvopTiming;
#else
    MVOP_Timing* pstTiming = &_stMVOPDrvCtrl[E_MVOP_DEV_0]._mvopTiming;
#endif
    memcpy(pMVOPTiming, pstTiming, sizeof(MVOP_Timing));

    return E_MVOP_OK;
}


//-----------------------------------------------------------------------------
/// Get Horizontal Size.
/// @return Horizontal Size
//-----------------------------------------------------------------------------
MS_U16 MDrv_MVOP_GetHSize(void)
{
    return (_stMVOPDrvCtrl[E_MVOP_DEV_0]._mvopTiming.u16Width);
}


//-----------------------------------------------------------------------------
/// Get Vertical Size.
/// @return Vertical Size
//-----------------------------------------------------------------------------
MS_U16 MDrv_MVOP_GetVSize(void)
{
    return (_stMVOPDrvCtrl[E_MVOP_DEV_0]._mvopTiming.u16Height);
}


//-----------------------------------------------------------------------------
/// Get MVOP timing is interlace or progressive.
/// @return TRUE for interlace; FALSE for progressive
//-----------------------------------------------------------------------------
MS_BOOL MDrv_MVOP_GetIsInterlace(void)
{
    return (_stMVOPDrvCtrl[E_MVOP_DEV_0]._mvopTiming.bInterlace);
}


//-----------------------------------------------------------------------------
/// Get MVOP timing is horizontal duplicated or not.
/// @return TRUE for yes; FALSE for not.
//-----------------------------------------------------------------------------
MS_BOOL MDrv_MVOP_GetIsHDuplicate(void)
{
    return (_stMVOPDrvCtrl[E_MVOP_DEV_0]._mvopTiming.bHDuplicate);
}

/******************************************************************************************/
/// This function is used for dectection of MVOP's Capability
/// @param u16HSize    \b IN
/// @param u16VSize    \b IN
/// @param u16Fps      \b IN
/// @return TRUE if MVOP's frequency can support this video source, otherwise return FASLE
/******************************************************************************************/
MS_BOOL MDrv_MVOP_CheckCapability(MS_U16 u16HSize, MS_U16 u16VSize, MS_U16 u16Fps)
{
    return ((MS_U32)((u16HSize + MVOP_HBlank_Min) * (u16VSize + MVOP_VBlank_Min)* u16Fps) <= (MS_U32)(HAL_MVOP_GetMaxFreerunClk()));
}

MS_U16 MDrv_MVOP_GetMaxHOffset(MS_U16 u16HSize, MS_U16 u16VSize, MS_U16 u16Fps)
{
    MS_U16 u16HttMax, u16OffsetMax = 0;

    //printf("\nHSize=%d, VSize=%d, Fps=%d, ", u16HSize, u16VSize, u16Fps);
    u16HttMax = HAL_MVOP_GetMaxFreerunClk() / u16Fps*1000 / (u16VSize + MVOP_VBlank);
    //printf("u16HttMax=%d\n", u16HttMax);
    u16OffsetMax = (u16HttMax - MVOP_HBlank_HD - u16HSize)/2;

    //printf("H_OffsetMax=%d\n", u16OffsetMax);
    return u16OffsetMax;
}

MS_U16 MDrv_MVOP_GetMaxVOffset(MS_U16 u16HSize, MS_U16 u16VSize, MS_U16 u16Fps)
{
    MS_U16 u16VttMax, u16OffsetMax = 0;
    MS_U16* pu16FixVtt =  &_stMVOPDrvCtrl[E_MVOP_DEV_0]._u16FixVtt;
    if(*pu16FixVtt > 0)
    {
        u16VttMax = *pu16FixVtt;
    }
    else
    {
        u16VttMax = HAL_MVOP_GetMaxFreerunClk() / u16Fps*1000 / (u16HSize + MVOP_HBlank_HD);
    }
    u16OffsetMax = (u16VttMax - MVOP_VBlank - u16VSize)/2;

    //printf("V_OffsetMax=%d\n", u16OffsetMax);
    return u16OffsetMax;
}

//------------------------------------------------------------------------------
/// MVOP Get Destination Information for GOP mixer
/// @return TRUE/FALSE
//------------------------------------------------------------------------------
MS_BOOL MDrv_MVOP_GetDstInfo(MVOP_DST_DispInfo *pDstInfo, MS_U32 u32SizeofDstInfo)
{
#if defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_LINUX_KERNEL)
    MVOP_TimingInfo_FromRegisters mvopTimingInfo;
#endif

    if(NULL == pDstInfo)
    {
        MVOP_ERR(printf("MApi_MVOP_GetDstInfo():pDstInfo is NULL\n");)
        return FALSE;
    }

    if(u32SizeofDstInfo != sizeof(MVOP_DST_DispInfo))
    {
        MVOP_ERR(printf("MApi_MVOP_GetDstInfo():u16SizeofDstInfo is different from the MVOP_DST_DispInfo defined, check header file!\n");)
        return FALSE;
    }
#if !defined(MSOS_TYPE_CE) && !defined(MSOS_TYPE_LINUX_KERNEL)
    MVOP_TimingInfo_FromRegisters mvopTimingInfo;
#endif
    memset(&mvopTimingInfo, 0, sizeof(MVOP_TimingInfo_FromRegisters));
    if(HAL_MVOP_GetTimingInfoFromRegisters(&mvopTimingInfo) == FALSE)
    {
        MVOP_ERR(printf("encounter errors in HAL_MVOP_GetTimingInfoFromRegisters()!\n");)
        memset(pDstInfo, 0, u32SizeofDstInfo);
        return FALSE;
    }
    pDstInfo->VDTOT = mvopTimingInfo.u16V_TotalCount;
    pDstInfo->HDTOT = mvopTimingInfo.u16H_TotalCount;
    pDstInfo->bInterlaceMode = mvopTimingInfo.bInterlace;
    pDstInfo->DEHST = mvopTimingInfo.u16HActive_Start / 2 + 2;
    if(pDstInfo->bInterlaceMode)
    {
        pDstInfo->DEVST = (mvopTimingInfo.u16VBlank0_End - mvopTimingInfo.u16TopField_VS) * 2 - 18;
    }
    else
    {
        pDstInfo->DEVST = (mvopTimingInfo.u16VBlank0_End - mvopTimingInfo.u16TopField_VS) - 5;
    }
    //HDeEnd=HDeStart+HDeWidth
    pDstInfo->DEHEND = pDstInfo->DEHST + pDstInfo->HDTOT - mvopTimingInfo.u16HActive_Start;
    //VDeEnd=VDeStart+VDeWidth
    if(pDstInfo->bInterlaceMode)
    {
        pDstInfo->DEVEND = pDstInfo->DEVST + pDstInfo->VDTOT - (2 * mvopTimingInfo.u16VBlank0_End - 3);
    }
    else
    {
        pDstInfo->DEVEND = pDstInfo->DEVST + pDstInfo->VDTOT - (mvopTimingInfo.u16VBlank0_End - 1);
    }
    MVOP_TIMING_DBG(printf("bInterlace=%u,HTotal=%lu,VTotal=%lu\n", pDstInfo->bInterlaceMode, pDstInfo->HDTOT, pDstInfo->VDTOT);)
    MVOP_TIMING_DBG(printf("HDeStart=%lu,HDeEnd=%lu,VDeStart=%lu,VDeEnd=%lu\n", pDstInfo->DEHST, pDstInfo->DEHEND, pDstInfo->DEVST, pDstInfo->DEVEND);)
    return TRUE;
}

/********************************************************************************/
/// Used for Output Fix Vtotal of MVOP
/// @param u16FixVtt      \b IN
/********************************************************************************/
MS_BOOL MDrv_MVOP_SetFixVtt(MS_U16 u16FixVtt)
{
     _stMVOPDrvCtrl[E_MVOP_DEV_0]._u16FixVtt = u16FixVtt;
    return TRUE;
}

MS_BOOL MDrv_MVOP_EnableFreerunMode(MS_BOOL bEnable)
{
    HAL_MVOP_EnableFreerunMode(bEnable);
    _stMVOPDrvCtrl[E_MVOP_DEV_0]._bEnableFreerunMode = bEnable;
    return TRUE;
}

MVOP_Result MDrv_MVOP_MiuSwitch(MS_U8 u8Miu)
{
    return MVOP_MiuSwitch(TRUE, u8Miu);
}

MS_BOOL MDrv_MVOP_SetBaseAdd(MS_U32 u32YOffset, MS_U32 u32UVOffset, MS_BOOL bProgressive, MS_BOOL b422pack)
{
#if defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_LINUX_KERNEL)
	HAL_MVOP_SetYUVBaseAdd(u32YOffset, u32UVOffset, bProgressive, b422pack);
    HAL_MVOP_LoadReg();
    return TRUE;
#else
    if (HAL_MVOP_SetYUVBaseAdd)
    {
        HAL_MVOP_SetYUVBaseAdd(u32YOffset, u32UVOffset, bProgressive, b422pack);
        HAL_MVOP_LoadReg();
        return TRUE;
    }
    else
    {
        MVOP_ERR(printf("%s not support!\n", __FUNCTION__);)
        return FALSE;
    }
#endif
}

void MDrv_MVOP_SEL_OP_FIELD(MS_BOOL bEnable)
{
#if STB_DC
    MVOP_CheckIsInit();
    HAL_MVOP_MaskDBRegCtrl(bEnable ,E_MVOP_SEL_OP_FIELD);
#endif
    MVOP_DBG(printf("\nMVOP MDrv_MVOP_SEL_OP_FIELD (0x%x) \n",bEnable);)
}


void MDrv_MVOP_INV_OP_VS(MS_BOOL bEnable)
{
#if STB_DC
    MVOP_CheckIsInit();
    HAL_MVOP_MaskDBRegCtrl(bEnable ,E_MVOP_INV_OP_VS);
#endif
    MVOP_DBG(printf("\nMVOP MDrv_MVOP_INV_OP_VS (0x%x) \n",bEnable);)
}

void MDrv_MVOP_FORCE_TOP(MS_BOOL bEnable)
{
#if STB_DC
    MVOP_CheckIsInit();
    HAL_MVOP_ModeCtrl(bEnable ,E_MVOP_FORCE_TOP);
#endif
    MVOP_DBG(printf("\nMVOP MDrv_MVOP_FORCE_TOP (0x%x) \n",bEnable);)
}

MS_BOOL MDrv_MVOP_SetImageWidthHight(MS_U16 u16Width ,MS_U16 u16Height)
{
#if STB_DC
    if(u16Width == 0 ||u16Height ==0)
    {
        MVOP_ERR(printf("\nMVOP the Input value could not be zero(0x%x ,0x%x) \n",u16Width ,u16Height);)
        return FALSE;
    }
    HAL_MVOP_SetPicWidthMinus(u16Width-1);
    HAL_MVOP_SetPicHeightMinus(u16Height-1);
    _u16Width = u16Width;
    _u16Height =u16Height;

#endif
    MVOP_DBG(printf("\nMVOP MDrv_MVOP_SetImageWidthHight (0x%x 0x%x) \n",u16Width ,u16Height);)
    return TRUE;
}

void MDrv_MVOP_SetStartPos(MS_U16 u16Xpos ,MS_U16 u16Ypos)
{
#if STB_DC
    MVOP_CheckIsInit();
    HAL_MVOP_SetStartX(u16Xpos);
    HAL_MVOP_SetStartY(u16Ypos);
#endif
    MVOP_DBG(printf("\nMVOP MDrv_MVOP_SetStartPos (0x%x 0x%x) \n",u16Xpos ,u16Ypos);)
}



void MDrv_MVOP_SetRegSizeFromMVD(MS_BOOL bEnable)
{
#if STB_DC
    MVOP_CheckIsInit();
    _bRegSizeFromMVDStatus = bEnable;
    HAL_MVOP_SetDeb2MVDFrameModeCtrl(_bRegSizeFromMVDStatus,E_MVOP_SIZE_FROM_MVD);
#endif
    MVOP_DBG(printf("\nMVOP MDrv_MVOP_SetRegSizeFromMVD (0x%x) \n",bEnable);)
}


void MDrv_MVOP_SetFrequency(MVOP_FREQUENCY eFreq)
{
#if STB_DC
    MVOP_CheckIsInit();
    HAL_MVOP_SetFrequency((HALMVOPFREQUENCY)eFreq);
#endif
    MVOP_DBG(printf("\nMVOP HAL_MVOP_SetFrequency (0x%x) \n",eFreq);)
}

//------------------------------------------------------------------------------
/// MVOP Enable/Disable mirror mode (chips after M10 support this feature)
/// @return TRUE/FALSE
//------------------------------------------------------------------------------
void MDrv_MVOP_SetVOPMirrorMode(MS_BOOL bEnable,MVOP_DrvMirror eMirrorMode)
{
#if MVOP_SUPPORT_MIRROR
    if( eMirrorMode == E_VOPMIRROR_VERTICAL)
    {
        HAL_MVOP_SetVerticalMirrorMode(bEnable);
    }
    else if(eMirrorMode == E_VOPMIRROR_HORIZONTALL)
    {
        HAL_MVOP_SetHorizontallMirrorMode(bEnable);
    }
#endif
    MVOP_DBG(printf("\nMVOP MDrv_MVOP_SetVOPMirrorMode (eMirrorMode :0x%x , 0x%x) \n",eMirrorMode,bEnable);)
}


void MDrv_MVOP_GetBaseAdd(MS_U32* u32YOffset, MS_U32* u32UVOffset)
{
#if MVOP_SUPPORT_OPENMAX
    *u32YOffset =  HAL_MVOP_GetYBaseAdd();
    *u32UVOffset = HAL_MVOP_GetUVBaseAdd();
#endif
}

//=============================================================================
// SUB MVOP
// Chips that support two MVOP modules: T12/T8, J2, A5.

//-----------------------------------------------------------------------------
/// Get status of MVOP driver
/// @param -pstatus: driver status
/// @return - TRUE / FALSE
//-----------------------------------------------------------------------------
MS_BOOL MDrv_MVOP_SubGetStatus(MVOP_DrvStatus *pMVOPStat)
{
    if (!MVOP_SubIsInit())
    {
        return FALSE;
    }
    return MVOP_GetStatus(E_MVOP_DEV_1, pMVOPStat);;
}


//-----------------------------------------------------------------------------
/// Get distance from HSync to DE for Scaler, unit: mvop clock cycle
/// @return HStart
//-----------------------------------------------------------------------------
MS_U16 MDrv_MVOP_SubGetHStart(void)
{
    MVOP_SubCheckIsInit_RetZero();
    return (_stMVOPDrvCtrl[E_MVOP_DEV_1]._mvopTiming.u16HActive_Start)/2 + 2;
}

//-----------------------------------------------------------------------------
/// Get distance from HSync to DE for Scaler, unit: mvop clock cycle
/// @return VStart
//-----------------------------------------------------------------------------
MS_U16 MDrv_MVOP_SubGetVStart(void)
{
    MS_U16 u16Vst;

    MVOP_SubCheckIsInit_RetZero();

    u16Vst = _stMVOPDrvCtrl[E_MVOP_DEV_1]._mvopTiming.u16VBlank0_End - _stMVOPDrvCtrl[E_MVOP_DEV_1]._mvopTiming.u16TopField_VS;
    if (_stMVOPDrvCtrl[E_MVOP_DEV_1]._mvopTiming.bInterlace==1)
    {
        u16Vst*=2;
        u16Vst-=18;
    }
    else
    {
        u16Vst-=5;
    }
    return u16Vst;
}

//-----------------------------------------------------------------------------
// Set MVOP clock enable
// @param bEnable \b IN
//   - # TRUE  Enable clock
//   - # FALSE Close clock
//-----------------------------------------------------------------------------
static void MDrv_MVOP_SubSetClk ( MS_BOOL bEnable )
{
    HAL_MVOP_SubSetDCClk(0, bEnable);  //Clk DC0
    HAL_MVOP_SubSetDCClk(1, bEnable);  //Clk DC1
}

//-----------------------------------------------------------------------------
/// Set MVOP test pattern.
//-----------------------------------------------------------------------------
void MDrv_MVOP_SubSetPattern(MVOP_Pattern enMVOPPattern)
{
    static MVOP_Pattern s_enMVOPPattern = MVOP_PATTERN_NORMAL;
    MVOP_SubCheckIsInit();
    if(enMVOPPattern == MVOP_PATTERN_DEFAULT)
    {
        enMVOPPattern = (MVOP_Pattern)(((int)(s_enMVOPPattern) + 1) % (int)MVOP_PATTERN_DEFAULT);
    }
    HAL_MVOP_SubSetPattern(enMVOPPattern);
    s_enMVOPPattern = enMVOPPattern;
}

//-----------------------------------------------------------------------------
/// Configure the tile format of the MVOP input.
/// @return TRUE or FALSE
//-----------------------------------------------------------------------------
MS_BOOL MDrv_MVOP_SubSetTileFormat(MVOP_TileFormat eTileFmt)
{
    if (!MVOP_SubIsInit())
    {
        return FALSE;
    }
    if ((eTileFmt != E_MVOP_TILE_8x32) && (eTileFmt != E_MVOP_TILE_16x32))
    {
        return FALSE;
    }
    return HAL_MVOP_SubSetTileFormat(eTileFmt);
}

//-----------------------------------------------------------------------------
/// Set MMIO Base for MVOP.
/// @return TRUE or FALSE
//-----------------------------------------------------------------------------
MS_BOOL MDrv_MVOP_SubSetMMIOMapBase(void)
{
    return MVOP_SetRegBaseAdd(HAL_MVOP_SubRegSetBase);
}

//-----------------------------------------------------------------------------
/// Initialize MVOP hardware and set it to hardwire mode
//-----------------------------------------------------------------------------
void MDrv_MVOP_SubInit()
{
    MVOP_InitVar(E_MVOP_DEV_1);

    if(MDrv_MVOP_SubSetMMIOMapBase() == FALSE)
    {
        return;
    }

#if defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_LINUX_KERNEL)
	HAL_MVOP_SubInit();
#else
    if (HAL_MVOP_SubInit)
    {
        HAL_MVOP_SubInit();
    }
    else
    {
        MVOP_ERR(printf("***%s: SUB MVOP UNSUPPORTED!\n", __FUNCTION__);)
        return;
    }
#endif

    HAL_MVOP_SubSetInputMode( VOPINPUT_HARDWIRE, NULL );

    // Set fld inv & ofld_inv
    HAL_MVOP_SubSetFieldInverse(ENABLE, ENABLE);

    // Set Croma weighting off
    HAL_MVOP_SubSetChromaWeighting(ENABLE);
#if ENABLE_3D_LR_MODE
    HAL_MVOP_SubEnable3DLR(DISABLE);
#endif
    MDrv_MVOP_SubSetClk(TRUE);
    MDrv_MVOP_SubEnableFreerunMode(FALSE);
    HAL_MVOP_SubEnableMVDInterface(FALSE);
    HAL_MVOP_SubLoadReg();

    _stMVOPDrvCtrl[E_MVOP_DEV_1]._stMVOPDrvStat.bIsInit = TRUE;
}

//-----------------------------------------------------------------------------
/// Exit MVOP, turn off clock
//-----------------------------------------------------------------------------
void MDrv_MVOP_SubExit()
{
    MVOP_SubCheckIsInit();
    MDrv_MVOP_SubSetClk(FALSE);
}


//-----------------------------------------------------------------------------
/// Enable and Disable MVOP
/// @param bEnable \b IN
///   - # TRUE  Enable
///   - # FALSE Disable and reset
//-----------------------------------------------------------------------------
void MDrv_MVOP_SubEnable ( MS_BOOL bEnable )
{
    MVOP_SubCheckIsInit();

    HAL_MVOP_SubSetMIUReqMask(TRUE);
    HAL_MVOP_SubEnable(bEnable);
    HAL_MVOP_SubSetMIUReqMask(FALSE);

    _stMVOPDrvCtrl[E_MVOP_DEV_1]._stMVOPDrvStat.bIsEnable = bEnable;
}


//-----------------------------------------------------------------------------
/// Get if MVOP is enable or not.
/// @param pbEnable \b OUT
///   - # TRUE  Enable
///   - # FALSE Disable
//-----------------------------------------------------------------------------
MVOP_Result MDrv_MVOP_SubGetIsEnable (MS_BOOL* pbEnable)
{
    if ((NULL == pbEnable) || (TRUE != MVOP_SubIsInit()))
    {
        return E_MVOP_FAIL;
    }

    *pbEnable = HAL_MVOP_SubGetEnableState();
    return E_MVOP_OK;
}

//-----------------------------------------------------------------------------
/// Set enable UVShift
//-----------------------------------------------------------------------------
void MDrv_MVOP_SubEnableUVShift(MS_BOOL bEnable)
{
    MVOP_SubCheckIsInit();
    HAL_MVOP_SubEnableUVShift(bEnable);
    _stMVOPDrvCtrl[E_MVOP_DEV_1]._stMVOPDrvStat.bIsUVShift = bEnable;
}

//-----------------------------------------------------------------------------
/// Set enable black background
//-----------------------------------------------------------------------------
void MDrv_MVOP_SubEnableBlackBG ( void )
{
    MVOP_SubCheckIsInit();
    HAL_MVOP_SubSetBlackBG();
    _stMVOPDrvCtrl[E_MVOP_DEV_1]._stMVOPDrvStat.bIsBlackBG = TRUE;
}


//-----------------------------------------------------------------------------
/// MVOP mono mode (fix chroma value)
//-----------------------------------------------------------------------------
void MDrv_MVOP_SubSetMonoMode(MS_BOOL bEnable)
{
    MVOP_SubCheckIsInit();
    HAL_MVOP_SubSetMonoMode(bEnable);
    _stMVOPDrvCtrl[E_MVOP_DEV_1]._stMVOPDrvStat.bIsMonoMode = bEnable;
}


//-----------------------------------------------------------------------------
/// Configure MVOP input.
/// @return MVOP_Result
//-----------------------------------------------------------------------------
MVOP_Result MDrv_MVOP_SubSetInputCfg (MVOP_InputSel in, MVOP_InputCfg* pCfg)
{
    MVOP_Result ret = E_MVOP_OK;

    if (!MVOP_SubIsInit())
    {
        MVOP_DBG(printf("%s:MVOP driver is not initialized!\n", __FUNCTION__);)
        return E_MVOP_FAIL;
    }

    switch (in)
    {
        case MVOP_INPUT_DRAM:
            if (!pCfg)
            {
                return E_MVOP_INVALID_PARAM;
            }
            HAL_MVOP_SubSetInputMode(VOPINPUT_MCUCTRL, pCfg);
            break;

#if ENABLE_3D_LR_MODE
        case MVOP_INPUT_HVD_3DLR:
            HAL_MVOP_SubEnable3DLR(ENABLE);
            //no break here to continue setting HVD hardwire mode
#endif
        case MVOP_INPUT_H264:
            HAL_MVOP_SubSetInputMode(VOPINPUT_HARDWIRE, NULL);
            HAL_MVOP_SubSetH264HardwireMode();
            break;

#if ENABLE_3D_LR_MODE
        case MVOP_INPUT_MVD_3DLR:
            HAL_MVOP_SubEnable3DLR(ENABLE);
            //no break here to continue setting MVD hardwire mode
#endif
        case MVOP_INPUT_MVD:
            HAL_MVOP_SubEnableMVDInterface(TRUE);
            HAL_MVOP_SubSetInputMode(VOPINPUT_HARDWIRE, NULL);
            break;
        case MVOP_INPUT_RVD:
            HAL_MVOP_SubSetInputMode(VOPINPUT_HARDWIRE, NULL);
            HAL_MVOP_SubSetRMHardwireMode();
            break;
        case MVOP_INPUT_CLIP:
            if (!pCfg)
            {
                return E_MVOP_INVALID_PARAM;
            }
            HAL_MVOP_SubSetCropWindow(pCfg);
            break;
        case MVOP_INPUT_JPD:
            HAL_MVOP_SubSetInputMode(VOPINPUT_HARDWIRE, NULL);
            HAL_MVOP_SubSetJpegHardwireMode();
            break;
        default:
            ret = E_MVOP_INVALID_PARAM;
            break;
    }

    return ret;
}


//-----------------------------------------------------------------------------
/// Configure MVOP output.
/// @return MVOP_Result
//-----------------------------------------------------------------------------
MVOP_Result MDrv_MVOP_SubSetOutputCfg(MVOP_VidStat *pstVideoStatus, MS_BOOL bEnHDup)
{
#if defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_LINUX_KERNEL)
	MVOP_Timing* pSubTiming = NULL;
#endif

    if (!MVOP_SubIsInit())
    {
        MVOP_DBG(printf("%s: driver is not initialized!\n", __FUNCTION__);)
        return E_MVOP_FAIL;
    }

    if (MVOP_CheckOutputCfg(pstVideoStatus) != TRUE)
    {
        MVOP_DBG(printf("%s: invalid parameter!\n", __FUNCTION__);)
        return E_MVOP_INVALID_PARAM;
    }
#if defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_LINUX_KERNEL)
	pSubTiming = &_stMVOPDrvCtrl[E_MVOP_DEV_1]._mvopTiming;
#else
	MVOP_Timing* pSubTiming = &_stMVOPDrvCtrl[E_MVOP_DEV_1]._mvopTiming;
#endif
    if (MVOP_AutoGenMPEGTiming(FALSE, pSubTiming, pstVideoStatus, bEnHDup) != TRUE)
    {
        MVOP_DBG(printf("%s: fail to calculate timing!\n", __FUNCTION__);)
        return E_MVOP_FAIL;
    }

    HAL_MVOP_SubSetOutputInterlace(pSubTiming->bInterlace);
    HAL_MVOP_SubSetOutputTiming(pSubTiming);
    HAL_MVOP_SubSetSynClk(pSubTiming);

    if (_stMVOPDrvCtrl[E_MVOP_DEV_1]._u16SrcHSize != 0)
    {
        //report the source width for scaler, intead of the 16-aligned width
        //MS_U16 u16Discard_Width = 0;
        //u16Discard_Width = _u16SubSrcHSize.u16Width - _u16SubSrcHSize;
        MVOP_DBG(printf("%s: _mvopSubTiming.u16Width: %d ==> %d\n", __FUNCTION__,
                        pSubTiming->u16Width, _stMVOPDrvCtrl[E_MVOP_DEV_1]._u16SrcHSize);)
        pSubTiming->u16Width = _stMVOPDrvCtrl[E_MVOP_DEV_1]._u16SrcHSize;
#if 0   //for H-mirror ON
        if (_mvopTiming.u16HActive_Start > u16Discard_Width)
        {
            _mvopTiming.u16HActive_Start -= u16Discard_Width;
        }
        else
        {
            printf("Warning: u16HActive_Start(%d) <= u16Discard_Width(%d)\n",
                _mvopTiming.u16HActive_Start, u16Discard_Width);
        }
        if (_mvopTiming.u16HImg_Start > u16Discard_Width)
        {
            _mvopTiming.u16HImg_Start -= u16Discard_Width;
        }
        else
        {
            printf("Warning: u16HImg_Start(%d) <= u16Discard_Width(%d)\n",
                _mvopTiming.u16HImg_Start, u16Discard_Width);
        }
#endif
    }
    MVOP_DumpOutputTiming(pSubTiming);
    return E_MVOP_OK;
}


//-----------------------------------------------------------------------------
/// Get MVOP output timing information.
/// @return MVOP_Result
//-----------------------------------------------------------------------------
MVOP_Result MDrv_MVOP_SubGetOutputTiming(MVOP_Timing* pMVOPTiming)
{
#if defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_LINUX_KERNEL)
	MVOP_Timing* pSubTiming = NULL;
#endif

    if (!MVOP_SubIsInit())
    {
        MVOP_DBG(printf("%s: driver is not initialized!\n", __FUNCTION__);)
        return E_MVOP_FAIL;
    }

    if (!pMVOPTiming)
    {
        MVOP_DBG(printf("%s: invalid parameter!\n", __FUNCTION__);)
        return E_MVOP_INVALID_PARAM;
    }

#if defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_LINUX_KERNEL)
    pSubTiming = &_stMVOPDrvCtrl[E_MVOP_DEV_1]._mvopTiming;
#else
    MVOP_Timing* pSubTiming = &_stMVOPDrvCtrl[E_MVOP_DEV_1]._mvopTiming;
#endif

    memcpy(pMVOPTiming, pSubTiming, sizeof(MVOP_Timing));
#if 0
    pMVOPTiming->u16V_TotalCount      =  _mvopSubTiming.u16V_TotalCount;
    pMVOPTiming->u16H_TotalCount      =  _mvopSubTiming.u16H_TotalCount;
    pMVOPTiming->u16VBlank0_Start     =  _mvopSubTiming.u16VBlank0_Start;
    pMVOPTiming->u16VBlank0_End       =  _mvopSubTiming.u16VBlank0_End;
    pMVOPTiming->u16VBlank1_Start     =  _mvopSubTiming.u16VBlank1_Start;
    pMVOPTiming->u16VBlank1_End       =  _mvopSubTiming.u16VBlank1_End;
    pMVOPTiming->u16TopField_Start    =  _mvopSubTiming.u16TopField_Start;
    pMVOPTiming->u16BottomField_Start =  _mvopSubTiming.u16BottomField_Start;
    pMVOPTiming->u16TopField_VS       =  _mvopSubTiming.u16TopField_VS;
    pMVOPTiming->u16BottomField_VS    =  _mvopSubTiming.u16BottomField_VS;
    pMVOPTiming->u16HActive_Start     =  _mvopSubTiming.u16HActive_Start;

    pMVOPTiming->bInterlace           =  _mvopSubTiming.bInterlace;
    pMVOPTiming->u8Framerate          =  _mvopSubTiming.u8Framerate;
    pMVOPTiming->u16H_Freq            =  _mvopSubTiming.u16H_Freq;
    pMVOPTiming->u16Num               =  _mvopSubTiming.u16Num;
    pMVOPTiming->u16Den               =  _mvopSubTiming.u16Den;
    pMVOPTiming->u8MvdFRCType         =  _mvopSubTiming.u8MvdFRCType;
    pMVOPTiming->u16ExpFrameRate      =  _mvopSubTiming.u16ExpFrameRate;
    pMVOPTiming->u16Width             =  _mvopSubTiming.u16Width;
    pMVOPTiming->u16Height            =  _mvopSubTiming.u16Height;
    pMVOPTiming->bHDuplicate          =  _mvopSubTiming.bHDuplicate;
#endif
    return E_MVOP_OK;
}


//-----------------------------------------------------------------------------
/// Get Horizontal Size.
/// @return Horizontal Size
//-----------------------------------------------------------------------------
MS_U16 MDrv_MVOP_SubGetHSize(void)
{
    MVOP_SubCheckIsInit_RetZero();
    return (_stMVOPDrvCtrl[E_MVOP_DEV_1]._mvopTiming.u16Width);
}


//-----------------------------------------------------------------------------
/// Get Vertical Size.
/// @return Vertical Size
//-----------------------------------------------------------------------------
MS_U16 MDrv_MVOP_SubGetVSize(void)
{
    MVOP_SubCheckIsInit_RetZero();
    return (_stMVOPDrvCtrl[E_MVOP_DEV_1]._mvopTiming.u16Height);
}


//-----------------------------------------------------------------------------
/// Get MVOP timing is interlace or progressive.
/// @return TRUE for interlace; FALSE for progressive
//-----------------------------------------------------------------------------
MS_BOOL MDrv_MVOP_SubGetIsInterlace(void)
{
    MVOP_SubCheckIsInit_RetZero();
    return (_stMVOPDrvCtrl[E_MVOP_DEV_1]._mvopTiming.bInterlace);
}


//-----------------------------------------------------------------------------
/// Get MVOP timing is horizontal duplicated or not.
/// @return TRUE for yes; FALSE for not.
//-----------------------------------------------------------------------------
MS_BOOL MDrv_MVOP_SubGetIsHDuplicate(void)
{
    MVOP_SubCheckIsInit_RetZero();
    return (_stMVOPDrvCtrl[E_MVOP_DEV_1]._mvopTiming.bHDuplicate);
}

/******************************************************************************************/
/// This function is used for dectection of MVOP's Capability
/// @param u16HSize    \b IN
/// @param u16VSize    \b IN
/// @param u16Fps      \b IN
/// @return TRUE if MVOP's frequency can support this video source, otherwise return FASLE
/******************************************************************************************/
MS_BOOL MDrv_MVOP_SubCheckCapability(MS_U16 u16HSize, MS_U16 u16VSize, MS_U16 u16Fps)
{
    return ((MS_U32)((u16HSize + MVOP_HBlank_Min) * (u16VSize + MVOP_VBlank_Min)* u16Fps) <= (MS_U32)(HAL_MVOP_SubGetMaxFreerunClk()));
}

MS_U16 MDrv_MVOP_SubGetMaxHOffset(MS_U16 u16HSize, MS_U16 u16VSize, MS_U16 u16Fps)
{
    MS_U16 u16HttMax, u16OffsetMax = 0;

#if defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_LINUX_KERNEL)
		//printf("\nHSize=%d, VSize=%d, Fps=%d, ", u16HSize, u16VSize, u16Fps);
        u16HttMax = HAL_MVOP_SubGetMaxFreerunClk() / u16Fps*1000 / (u16VSize + MVOP_VBlank);
        //printf("u16HttMax=%d\n", u16HttMax);
        u16OffsetMax = (u16HttMax - MVOP_HBlank_HD - u16HSize)/2;

        //printf("H_OffsetMax=%d\n", u16OffsetMax);
#else
    if (HAL_MVOP_SubGetMaxFreerunClk)
    {
        //printf("\nHSize=%d, VSize=%d, Fps=%d, ", u16HSize, u16VSize, u16Fps);
        u16HttMax = HAL_MVOP_SubGetMaxFreerunClk() / u16Fps*1000 / (u16VSize + MVOP_VBlank);
        //printf("u16HttMax=%d\n", u16HttMax);
        u16OffsetMax = (u16HttMax - MVOP_HBlank_HD - u16HSize)/2;

        //printf("H_OffsetMax=%d\n", u16OffsetMax);
    }
#endif
    return u16OffsetMax;
}

MS_U16 MDrv_MVOP_SubGetMaxVOffset(MS_U16 u16HSize, MS_U16 u16VSize, MS_U16 u16Fps)
{
    MS_U16 u16VttMax, u16OffsetMax = 0;

#if defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_LINUX_KERNEL)
	if (_stMVOPDrvCtrl[E_MVOP_DEV_1]._u16FixVtt > 0)
	{
		u16VttMax = _stMVOPDrvCtrl[E_MVOP_DEV_1]._u16FixVtt;
	}
    else
    {
        u16VttMax = HAL_MVOP_SubGetMaxFreerunClk() / u16Fps*1000 / (u16HSize + MVOP_HBlank_HD);
    }
    u16OffsetMax = (u16VttMax - MVOP_VBlank - u16VSize)/2;
#else
    if (HAL_MVOP_SubGetMaxFreerunClk)
    {
        if (_stMVOPDrvCtrl[E_MVOP_DEV_1]._u16FixVtt > 0)
        {
            u16VttMax = _stMVOPDrvCtrl[E_MVOP_DEV_1]._u16FixVtt;
        }
        else
        {
            u16VttMax = HAL_MVOP_SubGetMaxFreerunClk() / u16Fps*1000 / (u16HSize + MVOP_HBlank_HD);
        }
        u16OffsetMax = (u16VttMax - MVOP_VBlank - u16VSize)/2;
    }
#endif

    //printf("V_OffsetMax=%d\n", u16OffsetMax);
    return u16OffsetMax;
}

//------------------------------------------------------------------------------
/// MVOP Get Destination Information for GOP mixer
/// @return TRUE/FALSE
//------------------------------------------------------------------------------
MS_BOOL MDrv_MVOP_SubGetDstInfo(MVOP_DST_DispInfo *pDstInfo, MS_U32 u32SizeofDstInfo)
{
#if defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_LINUX_KERNEL)
    MVOP_TimingInfo_FromRegisters mvopTimingInfo;
#endif

    MVOP_SubCheckIsInit_RetZero();

    if(NULL == pDstInfo)
    {
        MVOP_ERR(printf("MApi_MVOP_GetDstInfo():pDstInfo is NULL\n");)
        return FALSE;
    }

    if(u32SizeofDstInfo != sizeof(MVOP_DST_DispInfo))
    {
        MVOP_ERR(printf("MApi_MVOP_GetDstInfo():u16SizeofDstInfo is different from the MVOP_DST_DispInfo defined, check header file!\n");)
        return FALSE;
    }

#if !defined(MSOS_TYPE_CE) && !defined(MSOS_TYPE_LINUX_KERNEL)
    MVOP_TimingInfo_FromRegisters mvopTimingInfo;
#endif

    memset(&mvopTimingInfo, 0, sizeof(MVOP_TimingInfo_FromRegisters));
    if(HAL_MVOP_SubGetTimingInfoFromRegisters(&mvopTimingInfo) == FALSE)
    {
        MVOP_ERR(printf("encounter errors in HAL_MVOP_SubGetTimingInfoFromRegisters()!\n");)
        memset(pDstInfo, 0, u32SizeofDstInfo);
        return FALSE;
    }
    pDstInfo->VDTOT = mvopTimingInfo.u16V_TotalCount;
    pDstInfo->HDTOT = mvopTimingInfo.u16H_TotalCount;
    pDstInfo->bInterlaceMode = mvopTimingInfo.bInterlace;
    pDstInfo->DEHST = mvopTimingInfo.u16HActive_Start / 2 + 2;
    if(pDstInfo->bInterlaceMode)
    {
        pDstInfo->DEVST = (mvopTimingInfo.u16VBlank0_End - mvopTimingInfo.u16TopField_VS) * 2 - 18;
    }
    else
    {
        pDstInfo->DEVST = (mvopTimingInfo.u16VBlank0_End - mvopTimingInfo.u16TopField_VS) - 5;
    }
    //HDeEnd=HDeStart+HDeWidth
    pDstInfo->DEHEND = pDstInfo->DEHST + pDstInfo->HDTOT - mvopTimingInfo.u16HActive_Start;
    //VDeEnd=VDeStart+VDeWidth
    if(pDstInfo->bInterlaceMode)
    {
        pDstInfo->DEVEND = pDstInfo->DEVST + pDstInfo->VDTOT - (2 * mvopTimingInfo.u16VBlank0_End - 3);
    }
    else
    {
        pDstInfo->DEVEND = pDstInfo->DEVST + pDstInfo->VDTOT - (mvopTimingInfo.u16VBlank0_End - 1);
    }
    MVOP_TIMING_DBG(printf("bInterlace=%u,HTotal=%lu,VTotal=%lu\n", pDstInfo->bInterlaceMode, pDstInfo->HDTOT, pDstInfo->VDTOT);)
    MVOP_TIMING_DBG(printf("HDeStart=%lu,HDeEnd=%lu,VDeStart=%lu,VDeEnd=%lu\n", pDstInfo->DEHST, pDstInfo->DEHEND, pDstInfo->DEVST, pDstInfo->DEVEND);)
    return TRUE;
}

MS_BOOL MDrv_MVOP_SubSetFixVtt(MS_U16 u16FixVtt)
{
    _stMVOPDrvCtrl[E_MVOP_DEV_1]._u16FixVtt = u16FixVtt;
    return TRUE;
}

MS_BOOL MDrv_MVOP_SubEnableFreerunMode(MS_BOOL bEnable)
{
    HAL_MVOP_SubEnableFreerunMode(bEnable);
    _stMVOPDrvCtrl[E_MVOP_DEV_1]._bEnableFreerunMode = bEnable;
    return TRUE;
}

MVOP_Result MDrv_MVOP_SubMiuSwitch(MS_U8 u8Miu)
{
    if (!MVOP_SubIsInit())
    {
        MVOP_DBG(printf("%s: driver is not initialized!\n", __FUNCTION__);)
        return E_MVOP_FAIL;
    }

    return MVOP_MiuSwitch(FALSE, u8Miu);
}

MS_BOOL MDrv_MVOP_SubSetBaseAdd(MS_U32 u32YOffset, MS_U32 u32UVOffset, MS_BOOL bProgressive, MS_BOOL b422pack)
{
#if !defined(CHIP_A3) && !defined(CHIP_C3)
    if (HAL_MVOP_SubSetYUVBaseAdd)
    {
        HAL_MVOP_SubSetYUVBaseAdd(u32YOffset, u32UVOffset, bProgressive, b422pack);
        HAL_MVOP_SubLoadReg();
        return TRUE;
    }
    else
    {
        MVOP_ERR(printf("%s not support!\n", __FUNCTION__);)
        return FALSE;
    }
#else
	MVOP_ERR(printf("%s not support!\n", __FUNCTION__);)
    return FALSE;
#endif
}

//------------------------------------------------------------------------------
/// MVOP Enable/Disable mirror mode (chips after M10 support this feature)
/// @return TRUE/FALSE
//------------------------------------------------------------------------------
void MDrv_MVOP_SubSetVOPMirrorMode(MS_BOOL bEnable, MVOP_DrvMirror eMirrorMode)
{
#if MVOP_SUPPORT_MIRROR
    MVOP_SubCheckIsInit();

    if( eMirrorMode == E_VOPMIRROR_VERTICAL)
    {
        HAL_MVOP_SubSetVerticalMirrorMode(bEnable);
    }
    else if(eMirrorMode == E_VOPMIRROR_HORIZONTALL)
    {
        HAL_MVOP_SubSetHorizontallMirrorMode(bEnable);
    }
#endif
    MVOP_DBG(printf("\nMVOP MDrv_MVOP_SubSetVOPMirrorMode (eMirrorMode :0x%x , 0x%x) \n",eMirrorMode,bEnable);)
}


MVOP_Result MDrv_MVOP_SendBlueScreen(MS_U16 u16Width, MS_U16 u16Height)
{
#if STB_DC
    MS_U8 u8InitFieldCtl = (MVOP_EN_MASK_INIT_FIELD | MVOP_MASK_INIT_FIELD_CNT0 | MVOP_MASK_INIT_FIELD_CNT1
                    | MVOP_MASK_INIT_FIELD_CNT2 | MVOP_RST_MASK_INIT_FIELD | MVOP_FORCE_DEB_START);

    if(u16Width == 0 || u16Height ==0)
    {
        MVOP_ERR(printf("\nMVOP the Input value could not be zero(0x%x ,0x%x) \n",u16Width ,u16Height);)
        return E_MVOP_FAIL;
    }

    MDrv_MVOP_SEL_OP_FIELD(TRUE);
    //MDrv_MVOP_SetImageWidthHight(u16Width, u16Height);
    HAL_MVOP_SetPicWidthMinus(u16Width-1);
    HAL_MVOP_SetPicHeightMinus(u16Height-1);
    HAL_MVOP_SetDeb2MVDFrameModeCtrl(FALSE, E_MVOP_SIZE_FROM_MVD);

    //Force clear STB_DC fb_mask
    HAL_MVOP_BaseFromIPCtrl(TRUE, E_MVOP_FD_MASK_CLR);
    HAL_MVOP_SetInitFieldCtl(u8InitFieldCtl, u8InitFieldCtl);

    MsOS_DelayTask(90);

    HAL_MVOP_SetInitFieldCtl(MVOP_FORCE_DEB_START, u8InitFieldCtl);
    MDrv_MVOP_SEL_OP_FIELD(FALSE);
    HAL_MVOP_SetDeb2MVDFrameModeCtrl(TRUE, E_MVOP_SIZE_FROM_MVD);
    HAL_MVOP_BaseFromIPCtrl(FALSE, E_MVOP_FD_MASK_CLR);

    if(!_bRegSizeFromMVDStatus)
    {
        HAL_MVOP_SetDeb2MVDFrameModeCtrl(FALSE, E_MVOP_SIZE_FROM_MVD);
        HAL_MVOP_SetPicWidthMinus(_u16Width-1);
        HAL_MVOP_SetPicHeightMinus(_u16Height-1);
    }
#endif

    return E_MVOP_OK;
}

//-----------------------------------------------------------------------------
/// MVOP set command interface.
//-----------------------------------------------------------------------------
MVOP_Result MDrv_MVOP_SetCommand(MVOP_Handle* stHd, MVOP_Command eCmd, void* pPara)
{
    MVOP_Result eRet = E_MVOP_OK;
    MVOP_DevID  eDevID = E_MVOP_DEV_0; //default is to control main mvop

    if ((eCmd & E_MVOP_CMD_SET_TYPE) != E_MVOP_CMD_SET_TYPE)
    {
        MVOP_ERR(printf("%s: invalid set cmd 0x%x\n", __FUNCTION__, eCmd);)
        return E_MVOP_INVALID_PARAM;
    }
    if (!pPara)
    {
        MVOP_ERR(printf("%s: null pPara\n", __FUNCTION__);)
        return E_MVOP_INVALID_PARAM;
    }
    if (stHd != NULL)
    {
        eDevID = (MVOP_DevID)(stHd->eModuleNum);
        MVOP_DBG(printf("%s eDevID=%x, eCmd=%x\n", __FUNCTION__, eDevID, eCmd);)
    }

    switch (eCmd)
    {
        case E_MVOP_CMD_SET_VSIZE_MIN:
            _stMVOPDrvCtrl[eDevID]._bEnMinVSize576 = *(MS_BOOL*)pPara;
            MVOP_DBG(printf("Set _bEnMinVSize576 as %x\n", _stMVOPDrvCtrl[eDevID]._bEnMinVSize576);)
            break;

        case E_MVOP_CMD_SET_STB_FD_MASK_CLR:
        #if STB_DC
        {
            MS_BOOL bEnable;
            bEnable =*(MS_BOOL*)pPara;
            HAL_MVOP_BaseFromIPCtrl(bEnable, E_MVOP_FD_MASK_CLR);
            MVOP_DBG(printf("\nMVOP MDrv_MVOP_ClearFDMask (0x%x) \n",bEnable);)

        }
        #else
        {
            MVOP_DBG(printf("\n not support this function!! \n");)
            eRet = E_MVOP_FAIL;
        }
        #endif
            break;

#if (defined(CHIP_AGATE))
        case E_MVOP_CMD_SET_3DLR_INST_VBLANK:
            _stMVOPDrvCtrl[eDevID]._u16InsVBlank= *(MS_U16*)pPara;
            printf("Set _u16InsVBlank as %x\n", _stMVOPDrvCtrl[eDevID]._u16InsVBlank);
            break;
#endif

        case E_MVOP_CMD_SET_3DLR_ALT_OUT:
        {
            MS_BOOL bEnable;
            bEnable = *(MS_BOOL*)pPara;
            if ((E_MVOP_DEV_0 == eDevID) && HAL_MVOP_Set3DLRAltOutput)
            {
                HAL_MVOP_Set3DLRAltOutput(bEnable);
                HAL_MVOP_LoadReg();
            }
            else
            {
                MVOP_DBG(printf("\n %s not support this cmd 0x%x!! \n", __FUNCTION__, eCmd);)
                eRet = E_MVOP_FAIL;
            }
         }
            break;

        default:
            break;
    }

    return eRet;
}



#if !defined (CHIP_A3) && !defined (CHIP_C3)
//=============================================================================
// The 3rd MVOP
// Chips that support three MVOP modules: Agate.

//-----------------------------------------------------------------------------
/// Get status of MVOP driver
/// @param -pstatus: driver status
/// @return - TRUE / FALSE
//-----------------------------------------------------------------------------
MS_BOOL MDrv_MVOP_EX_GetStatus(MVOP_DevID eID, MVOP_DrvStatus *pMVOPStat)
{
    return MVOP_GetStatus(eID, pMVOPStat);
}

//-----------------------------------------------------------------------------
/// Get distance from HSync to DE for Scaler, unit: mvop clock cycle
/// @return HStart
//-----------------------------------------------------------------------------
MS_U16 MDrv_MVOP_EX_GetHStart(MVOP_DevID eID)
{
    MVOP_DrvCtrlSt* pstDrvCtrl = MVOP_GetDrvCtrl(eID);
    MS_U16 u16HStart = (pstDrvCtrl) ? ((pstDrvCtrl->_mvopTiming.u16HActive_Start)/2 + 2) : 0;
    return u16HStart;
}

//-----------------------------------------------------------------------------
/// Get distance from HSync to DE for Scaler, unit: mvop clock cycle
/// @return VStart
//-----------------------------------------------------------------------------
MS_U16 MDrv_MVOP_EX_GetVStart(MVOP_DevID eID)
{
    MVOP_DrvCtrlSt* pstDrvCtrl = MVOP_GetDrvCtrl(eID);
    MVOP_Timing* pstTiming = NULL;
    MS_U16 u16Vst = 0;
    if (pstDrvCtrl)
    {
        pstTiming = &pstDrvCtrl->_mvopTiming;
        u16Vst = pstTiming->u16VBlank0_End - pstTiming->u16TopField_VS;
        if (pstTiming->bInterlace==1)
        {
            u16Vst*=2;
            u16Vst-=18;
        }
        else
        {
            u16Vst-=5;
        }
    }
    return u16Vst;
}

//-----------------------------------------------------------------------------
// Set MVOP clock enable
// @param bEnable \b IN
//   - # TRUE  Enable clock
//   - # FALSE Close clock
//-----------------------------------------------------------------------------
static void MDrv_MVOP_EX_SetClk(MVOP_DevID eID,  MS_BOOL bEnable)
{
    HAL_MVOP_EX_SetDCClk(eID, 0, bEnable);  //Clk DC0
    HAL_MVOP_EX_SetDCClk(eID, 1, bEnable);  //Clk DC1
}

//-----------------------------------------------------------------------------
/// Set MVOP test pattern.
//-----------------------------------------------------------------------------
MVOP_Result MDrv_MVOP_EX_SetPattern(MVOP_DevID eID, MVOP_Pattern enMVOPPattern)
{
    static MVOP_Pattern s_enMVOPPattern = MVOP_PATTERN_NORMAL;
    MVOP_EX_CheckIsInit(eID);
    if(enMVOPPattern == MVOP_PATTERN_DEFAULT)
    {
        enMVOPPattern = (MVOP_Pattern)(((int)(s_enMVOPPattern) + 1) % (int)MVOP_PATTERN_DEFAULT);
    }
    HAL_MVOP_EX_SetPattern(eID, enMVOPPattern);
    s_enMVOPPattern = enMVOPPattern;
    return E_MVOP_OK;
}

//-----------------------------------------------------------------------------
/// Configure the tile format of the MVOP input.
/// @return TRUE or FALSE
//-----------------------------------------------------------------------------
MVOP_Result MDrv_MVOP_EX_SetTileFormat(MVOP_DevID eID, MVOP_TileFormat eTileFmt)
{
    MVOP_Result eRet = E_MVOP_FAIL;
    MVOP_EX_CheckIsInit(eID);
    if ((eTileFmt != E_MVOP_TILE_8x32) && (eTileFmt != E_MVOP_TILE_16x32))
    {
        return E_MVOP_INVALID_PARAM;
    }
    if (TRUE == HAL_MVOP_EX_SetTileFormat(eID, eTileFmt))
    {
        eRet = E_MVOP_OK;
    }
    return eRet;
}

//-----------------------------------------------------------------------------
/// Set MMIO Base for MVOP.
/// @return TRUE or FALSE
//-----------------------------------------------------------------------------
MS_BOOL MDrv_MVOP_EX_SetMMIOMapBase(MVOP_DevID eID)
{
    return MVOP_SetRegBaseAdd(HAL_MVOP_EX_RegSetBase);
}

//-----------------------------------------------------------------------------
/// Initialize MVOP hardware and set it to hardwire mode
//-----------------------------------------------------------------------------
MVOP_Result MDrv_MVOP_EX_Init(MVOP_DevID eID, MS_U32 u32InitParam)
{
    MVOP_InitVar(eID);

    if (MDrv_MVOP_EX_SetMMIOMapBase(eID) == FALSE)
    {
        return E_MVOP_FAIL;
    }

    if (HAL_MVOP_EX_Init)
    {
        if (HAL_MVOP_EX_Init(eID) == FALSE)
        {
            return E_MVOP_FAIL;
        }
    }
    else
    {
        return E_MVOP_FAIL;
    }

    HAL_MVOP_EX_SetInputMode(eID, VOPINPUT_HARDWIRE, NULL);

    // Set fld inv & ofld_inv
    HAL_MVOP_EX_SetFieldInverse(eID, ENABLE, ENABLE);

    // Set Croma weighting off
    HAL_MVOP_EX_SetChromaWeighting(eID, ENABLE);
#if ENABLE_3D_LR_MODE
    HAL_MVOP_EX_Enable3DLR(eID, DISABLE);
#endif
    MDrv_MVOP_EX_SetClk(eID, TRUE);
    MDrv_MVOP_EX_EnableFreerunMode(eID, FALSE);
    HAL_MVOP_EX_EnableMVDInterface(eID, FALSE);
    HAL_MVOP_EX_LoadReg(eID);

    _stMVOPDrvCtrl[eID]._stMVOPDrvStat.bIsInit = TRUE;
    return E_MVOP_OK;
}

//-----------------------------------------------------------------------------
/// Exit MVOP, turn off clock
//-----------------------------------------------------------------------------
MVOP_Result MDrv_MVOP_EX_Exit(MVOP_DevID eID, MS_U32 u32ExitParam)
{
    MVOP_Result eRet = E_MVOP_FAIL;
    if (MVOP_EX_IsInit(eID))
    {
        MDrv_MVOP_EX_SetClk(eID, FALSE);
        eRet = E_MVOP_OK;
    }

    return eRet;
}

//-----------------------------------------------------------------------------
/// Enable and Disable MVOP
/// @param bEnable \b IN
///   - # TRUE  Enable
///   - # FALSE Disable and reset
//-----------------------------------------------------------------------------
MVOP_Result MDrv_MVOP_EX_Enable(MVOP_DevID eID, MS_BOOL bEnable)
{
    MVOP_EX_CheckIsInit(eID);

    HAL_MVOP_EX_SetMIUReqMask(eID, TRUE);
    HAL_MVOP_EX_Enable(eID, bEnable);
    HAL_MVOP_EX_SetMIUReqMask(eID, FALSE);

    _stMVOPDrvCtrl[eID]._stMVOPDrvStat.bIsEnable = bEnable;
    return E_MVOP_OK;
}


//-----------------------------------------------------------------------------
/// Get if MVOP is enable or not.
/// @param pbEnable \b OUT
///   - # TRUE  Enable
///   - # FALSE Disable
//-----------------------------------------------------------------------------
MVOP_Result MDrv_MVOP_EX_GetIsEnable (MVOP_DevID eID, MS_BOOL* pbEnable)
{
    if ((NULL == pbEnable) || (TRUE != MVOP_EX_IsInit(eID)))
    {
        return E_MVOP_FAIL;
    }

    *pbEnable = HAL_MVOP_EX_GetEnableState(eID);
    return E_MVOP_OK;
}

//-----------------------------------------------------------------------------
/// Set enable UVShift
//-----------------------------------------------------------------------------
MVOP_Result MDrv_MVOP_EX_EnableUVShift(MVOP_DevID eID, MS_BOOL bEnable)
{
    MVOP_EX_CheckIsInit(eID);
    HAL_MVOP_EX_EnableUVShift(eID, bEnable);
    _stMVOPDrvCtrl[eID]._stMVOPDrvStat.bIsUVShift = bEnable;
    return E_MVOP_OK;
}

//-----------------------------------------------------------------------------
/// Set enable black background
//-----------------------------------------------------------------------------
MVOP_Result MDrv_MVOP_EX_EnableBlackBG (MVOP_DevID eID)
{
    MVOP_EX_CheckIsInit(eID);
    HAL_MVOP_EX_SetBlackBG(eID);
    _stMVOPDrvCtrl[eID]._stMVOPDrvStat.bIsBlackBG = TRUE;
    return E_MVOP_OK;
}

//-----------------------------------------------------------------------------
/// MVOP mono mode (fix chroma value)
//-----------------------------------------------------------------------------
MVOP_Result MDrv_MVOP_EX_SetMonoMode(MVOP_DevID eID, MS_BOOL bEnable)
{
    MVOP_EX_CheckIsInit(eID);
    HAL_MVOP_EX_SetMonoMode(eID, bEnable);
    _stMVOPDrvCtrl[eID]._stMVOPDrvStat.bIsMonoMode = bEnable;
    return E_MVOP_OK;
}

//-----------------------------------------------------------------------------
/// Configure MVOP input.
/// @return MVOP_Result
//-----------------------------------------------------------------------------
MVOP_Result MDrv_MVOP_EX_SetInputCfg (MVOP_DevID eID, MVOP_InputSel in, MVOP_InputCfg* pCfg)
{
    MVOP_Result ret = E_MVOP_OK;

    if (!MVOP_EX_IsInit(eID))
    {
        MVOP_DBG(printf("%s:MVOP driver is not initialized!\n", __FUNCTION__);)
        return E_MVOP_FAIL;
    }

    switch (in)
    {
        case MVOP_INPUT_DRAM:
            if (!pCfg)
            {
                return E_MVOP_INVALID_PARAM;
            }
            HAL_MVOP_EX_SetInputMode(eID, VOPINPUT_MCUCTRL, pCfg);
            break;

#if ENABLE_3D_LR_MODE
        case MVOP_INPUT_HVD_3DLR:
            HAL_MVOP_EX_Enable3DLR(eID, ENABLE);
            //no break here to continue setting HVD hardwire mode
#endif
        case MVOP_INPUT_H264:
            HAL_MVOP_EX_SetInputMode(eID, VOPINPUT_HARDWIRE, NULL);
            HAL_MVOP_EX_SetH264HardwireMode(eID);
            break;

#if ENABLE_3D_LR_MODE
        case MVOP_INPUT_MVD_3DLR:
            HAL_MVOP_EX_Enable3DLR(eID, ENABLE);
            //no break here to continue setting MVD hardwire mode
#endif
        case MVOP_INPUT_MVD:
            HAL_MVOP_EX_EnableMVDInterface(eID, TRUE);
            HAL_MVOP_EX_SetInputMode(eID, VOPINPUT_HARDWIRE, NULL);
            break;
        case MVOP_INPUT_RVD:
            HAL_MVOP_EX_SetInputMode(eID, VOPINPUT_HARDWIRE, NULL);
            HAL_MVOP_EX_SetRMHardwireMode(eID);
            break;
        case MVOP_INPUT_CLIP:
            if (!pCfg)
            {
                return E_MVOP_INVALID_PARAM;
            }
            HAL_MVOP_EX_SetCropWindow(eID, pCfg);
            break;
        case MVOP_INPUT_JPD:
            HAL_MVOP_EX_SetInputMode(eID, VOPINPUT_HARDWIRE, NULL);
            HAL_MVOP_EX_SetJpegHardwireMode(eID);
            break;
        default:
            ret = E_MVOP_INVALID_PARAM;
            break;
    }

    return ret;
}

//-----------------------------------------------------------------------------
/// Configure MVOP output.
/// @return MVOP_Result
//-----------------------------------------------------------------------------
MVOP_Result MDrv_MVOP_EX_SetOutputCfg(MVOP_DevID eID, MVOP_VidStat *pstVideoStatus, MS_BOOL bEnHDup)
{
#if defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_LINUX_KERNEL)
    MVOP_Timing* pstTiming = NULL;
	MS_U16 u16SrcHSize = 0;
#endif

    if (!MVOP_EX_IsInit(eID))
    {
        MVOP_DBG(printf("%s: driver is not initialized!\n", __FUNCTION__);)
        return E_MVOP_FAIL;
    }

    if (MVOP_CheckOutputCfg(pstVideoStatus) != TRUE)
    {
        MVOP_DBG(printf("%s: invalid parameter!\n", __FUNCTION__);)
        return E_MVOP_INVALID_PARAM;
    }

#if defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_LINUX_KERNEL)
    pstTiming = &_stMVOPDrvCtrl[eID]._mvopTiming;
#else
    MVOP_Timing* pstTiming = &_stMVOPDrvCtrl[eID]._mvopTiming;
#endif

    if (MVOP_AutoGenMPEGTiming(FALSE, pstTiming, pstVideoStatus, bEnHDup) != TRUE)
    {
        MVOP_DBG(printf("%s: fail to calculate timing!\n", __FUNCTION__);)
        return E_MVOP_FAIL;
    }

    HAL_MVOP_EX_SetOutputInterlace(eID, pstTiming->bInterlace);
    HAL_MVOP_EX_SetOutputTiming(eID, pstTiming);
    HAL_MVOP_EX_SetSynClk(eID, pstTiming);

#if defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_LINUX_KERNEL)
	u16SrcHSize = _stMVOPDrvCtrl[eID]._u16SrcHSize;
#else
    MS_U16 u16SrcHSize = _stMVOPDrvCtrl[eID]._u16SrcHSize;
#endif

    if (u16SrcHSize != 0)
    {
        //report the source width for scaler, intead of the 16-aligned width
        //MS_U16 u16Discard_Width = 0;
        //u16Discard_Width = _u16SubSrcHSize.u16Width - _u16SubSrcHSize;
        MVOP_DBG(printf("%s: _mvopTiming.u16Width: %d ==> %d\n", __FUNCTION__,
                        pstTiming->u16Width, u16SrcHSize);)
        pstTiming->u16Width = u16SrcHSize;
#if 0   //for H-mirror ON
        if (_mvopTiming.u16HActive_Start > u16Discard_Width)
        {
            _mvopTiming.u16HActive_Start -= u16Discard_Width;
        }
        else
        {
            printf("Warning: u16HActive_Start(%d) <= u16Discard_Width(%d)\n",
                _mvopTiming.u16HActive_Start, u16Discard_Width);
        }
        if (_mvopTiming.u16HImg_Start > u16Discard_Width)
        {
            _mvopTiming.u16HImg_Start -= u16Discard_Width;
        }
        else
        {
            printf("Warning: u16HImg_Start(%d) <= u16Discard_Width(%d)\n",
                _mvopTiming.u16HImg_Start, u16Discard_Width);
        }
#endif
    }
    MVOP_DumpOutputTiming(pstTiming);
    return E_MVOP_OK;
}


//-----------------------------------------------------------------------------
/// Get MVOP output timing information.
/// @return MVOP_Result
//-----------------------------------------------------------------------------
MVOP_Result MDrv_MVOP_EX_GetOutputTiming(MVOP_DevID eID, MVOP_Timing* pMVOPTiming)
{
#if defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_LINUX_KERNEL)
	MVOP_Timing* pstTiming = NULL;
#endif

    if (!MVOP_EX_IsInit(eID))
    {
        MVOP_DBG(printf("%s: driver is not initialized!\n", __FUNCTION__);)
        return E_MVOP_FAIL;
    }

    if (!pMVOPTiming)
    {
        MVOP_DBG(printf("%s: invalid parameter!\n", __FUNCTION__);)
        return E_MVOP_INVALID_PARAM;
    }

#if defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_LINUX_KERNEL)
    pstTiming = &_stMVOPDrvCtrl[eID]._mvopTiming;
#else
    MVOP_Timing* pstTiming = &_stMVOPDrvCtrl[eID]._mvopTiming;
#endif
    memcpy(pMVOPTiming, pstTiming, sizeof(MVOP_Timing));
    return E_MVOP_OK;
}

//-----------------------------------------------------------------------------
/// Get Horizontal Size.
/// @return Horizontal Size
//-----------------------------------------------------------------------------
MS_U16 MDrv_MVOP_EX_GetHSize(MVOP_DevID eID)
{
    MS_U16 u16HSize = 0;
    MVOP_DrvCtrlSt* pstDrvCtrl = NULL;

    if (MVOP_EX_IsInit(eID))
    {
        pstDrvCtrl = MVOP_GetDrvCtrl(eID);
        u16HSize = (pstDrvCtrl) ? (pstDrvCtrl->_mvopTiming.u16Width) : 0;
    }
    return u16HSize;
}

//-----------------------------------------------------------------------------
/// Get Vertical Size.
/// @return Vertical Size
//-----------------------------------------------------------------------------
MS_U16 MDrv_MVOP_EX_GetVSize(MVOP_DevID eID)
{
    MS_U16 u16VSize = 0;
    MVOP_DrvCtrlSt* pstDrvCtrl = NULL;

    if (MVOP_EX_IsInit(eID))
    {
        pstDrvCtrl = MVOP_GetDrvCtrl(eID);
        u16VSize = (pstDrvCtrl) ? (pstDrvCtrl->_mvopTiming.u16Height) : 0;
    }
    return u16VSize;
}

//-----------------------------------------------------------------------------
/// Get MVOP timing is interlace or progressive.
/// @return TRUE for interlace; FALSE for progressive
//-----------------------------------------------------------------------------
MS_BOOL MDrv_MVOP_EX_GetIsInterlace(MVOP_DevID eID)
{
    MS_BOOL bInterlaced = FALSE;
    MVOP_DrvCtrlSt* pstDrvCtrl = NULL;

    if (MVOP_EX_IsInit(eID))
    {
        pstDrvCtrl = MVOP_GetDrvCtrl(eID);
        bInterlaced = (pstDrvCtrl) ? (pstDrvCtrl->_mvopTiming.bInterlace) : FALSE;
    }
    return bInterlaced;
}

//-----------------------------------------------------------------------------
/// Get MVOP timing is horizontal duplicated or not.
/// @return TRUE for yes; FALSE for not.
//-----------------------------------------------------------------------------
MS_BOOL MDrv_MVOP_EX_GetIsHDuplicate(MVOP_DevID eID)
{
    MS_BOOL bHDuplicated = FALSE;
    MVOP_DrvCtrlSt* pstDrvCtrl = NULL;

    if (MVOP_EX_IsInit(eID))
    {
        pstDrvCtrl = MVOP_GetDrvCtrl(eID);
        bHDuplicated = (pstDrvCtrl) ? (pstDrvCtrl->_mvopTiming.bHDuplicate) : FALSE;
    }
    return bHDuplicated;
}

/******************************************************************************************/
/// This function is used for dectection of MVOP's Capability
/// @param u16HSize    \b IN
/// @param u16VSize    \b IN
/// @param u16Fps      \b IN
/// @return TRUE if MVOP's frequency can support this video source, otherwise return FALSE
/******************************************************************************************/
MS_BOOL MDrv_MVOP_EX_CheckCapability(MVOP_DevID eID, MS_U32 u32InParam)
{
    MS_U16 u16HSize = 0;
    MS_U16 u16VSize = 0;
    MS_U16 u16Fps = 0;
    MS_BOOL bRet = FALSE;
    if ((u32InParam != 0) && (((MVOP_CapInput*)u32InParam)->u8StrVer == 0))
   {
        MVOP_CapInput* pstInput = (MVOP_CapInput*)u32InParam;
        u16HSize = pstInput->u16HSize;
        u16VSize = pstInput->u16VSize;
        u16Fps = pstInput->u16Fps;
        bRet = ((MS_U32)((u16HSize + MVOP_HBlank_Min) * (u16VSize + MVOP_VBlank_Min)* u16Fps) <= (MS_U32)(HAL_MVOP_EX_GetMaxFreerunClk(eID)));
    }
    return bRet;
}

MS_U16 MDrv_MVOP_EX_GetMaxVOffset(MVOP_DevID eID, MS_U16 u16HSize, MS_U16 u16VSize, MS_U16 u16Fps)
{
    MS_U16 u16VttMax, u16OffsetMax = 0;

    if(_stMVOPDrvCtrl[eID]._u16FixVtt > 0)
    {
        u16VttMax = _stMVOPDrvCtrl[eID]._u16FixVtt;
    }
    else
    {
        u16VttMax = HAL_MVOP_EX_GetMaxFreerunClk(eID) / u16Fps*1000 / (u16HSize + MVOP_HBlank_HD);
    }
    u16OffsetMax = (u16VttMax - MVOP_VBlank - u16VSize)/2;

    //printf("V_OffsetMax=%d\n", u16OffsetMax);
    return u16OffsetMax;
}

//------------------------------------------------------------------------------
/// MVOP Get Destination Information for GOP mixer
/// @return TRUE/FALSE
//------------------------------------------------------------------------------
MS_BOOL MDrv_MVOP_EX_GetDstInfo(MVOP_DevID eID, MVOP_DST_DispInfo *pDstInfo, MS_U32 u32SizeofDstInfo)
{
#if defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_LINUX_KERNEL)
    MVOP_TimingInfo_FromRegisters mvopTimingInfo;
#endif

    if(NULL == pDstInfo)
    {
        MVOP_ERR(printf("MApi_MVOP_GetDstInfo():pDstInfo is NULL\n");)
        return FALSE;
    }

    if(u32SizeofDstInfo != sizeof(MVOP_DST_DispInfo))
    {
        MVOP_ERR(printf("MApi_MVOP_GetDstInfo():u16SizeofDstInfo is different from the MVOP_DST_DispInfo defined, check header file!\n");)
        return FALSE;
    }

#if !defined(MSOS_TYPE_CE) && !defined(MSOS_TYPE_LINUX_KERNEL)
    MVOP_TimingInfo_FromRegisters mvopTimingInfo;
#endif
    memset(&mvopTimingInfo, 0, sizeof(MVOP_TimingInfo_FromRegisters));
    if(HAL_MVOP_EX_GetTimingInfoFromRegisters(eID, &mvopTimingInfo) == FALSE)
    {
        MVOP_ERR(printf("encounter errors in HAL_MVOP_EX_GetTimingInfoFromRegisters()!\n");)
        memset(pDstInfo, 0, u32SizeofDstInfo);
        return FALSE;
    }
    pDstInfo->VDTOT = mvopTimingInfo.u16V_TotalCount;
    pDstInfo->HDTOT = mvopTimingInfo.u16H_TotalCount;
    pDstInfo->bInterlaceMode = mvopTimingInfo.bInterlace;
    pDstInfo->DEHST = mvopTimingInfo.u16HActive_Start / 2 + 2;
    if(pDstInfo->bInterlaceMode)
    {
        pDstInfo->DEVST = (mvopTimingInfo.u16VBlank0_End - mvopTimingInfo.u16TopField_VS) * 2 - 18;
    }
    else
    {
        pDstInfo->DEVST = (mvopTimingInfo.u16VBlank0_End - mvopTimingInfo.u16TopField_VS) - 5;
    }
    //HDeEnd=HDeStart+HDeWidth
    pDstInfo->DEHEND = pDstInfo->DEHST + pDstInfo->HDTOT - mvopTimingInfo.u16HActive_Start;
    //VDeEnd=VDeStart+VDeWidth
    if(pDstInfo->bInterlaceMode)
    {
        pDstInfo->DEVEND = pDstInfo->DEVST + pDstInfo->VDTOT - (2 * mvopTimingInfo.u16VBlank0_End - 3);
    }
    else
    {
        pDstInfo->DEVEND = pDstInfo->DEVST + pDstInfo->VDTOT - (mvopTimingInfo.u16VBlank0_End - 1);
    }
    MVOP_TIMING_DBG(printf("bInterlace=%u,HTotal=%lu,VTotal=%lu\n", pDstInfo->bInterlaceMode, pDstInfo->HDTOT, pDstInfo->VDTOT);)
    MVOP_TIMING_DBG(printf("HDeStart=%lu,HDeEnd=%lu,VDeStart=%lu,VDeEnd=%lu\n", pDstInfo->DEHST, pDstInfo->DEHEND, pDstInfo->DEVST, pDstInfo->DEVEND);)
    return TRUE;
}

MVOP_Result MDrv_MVOP_EX_SetFixVtt(MVOP_DevID eID, MS_U16 u16FixVtt)
{
    _stMVOPDrvCtrl[eID]._u16FixVtt = u16FixVtt;
    return E_MVOP_OK;
}

MVOP_Result MDrv_MVOP_EX_EnableFreerunMode(MVOP_DevID eID, MS_BOOL bEnable)
{
    HAL_MVOP_EX_EnableFreerunMode(eID, bEnable);
    _stMVOPDrvCtrl[eID]._bEnableFreerunMode = bEnable;
    return E_MVOP_OK;
}

MVOP_Result MDrv_MVOP_EX_MiuSwitch(MVOP_DevID eID, MS_U8 u8Miu)
{
    return MVOP_MiuSwitch(FALSE, u8Miu);
}

//------------------------------------------------------------------------------
/// MVOP Enable/Disable mirror mode (chips after M10 support this feature)
/// @return TRUE/FALSE
//------------------------------------------------------------------------------
MVOP_Result MDrv_MVOP_EX_SetVOPMirrorMode(MVOP_DevID eID, MS_BOOL bEnable, MVOP_DrvMirror eMirrorMode)
{
    MVOP_EX_CheckIsInit(eID);
#if MVOP_SUPPORT_MIRROR
    if( eMirrorMode == E_VOPMIRROR_VERTICAL)
    {
        HAL_MVOP_EX_SetVerticalMirrorMode(eID, bEnable);
    }
    else if(eMirrorMode == E_VOPMIRROR_HORIZONTALL)
    {
        HAL_MVOP_EX_SetHorizontallMirrorMode(eID, bEnable);
    }
#endif
    MVOP_DBG(printf("\nMVOP MDrv_MVOP_EX_SetVOPMirrorMode (eMirrorMode :0x%x , 0x%x) \n",eMirrorMode,bEnable);)
    return E_MVOP_OK;
}

#endif


