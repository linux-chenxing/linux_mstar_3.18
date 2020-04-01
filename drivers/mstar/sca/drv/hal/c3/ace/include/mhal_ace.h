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
#ifndef MHAL_ACE_H
#define MHAL_ACE_H

void Hal_ACE_DMS( MS_BOOL bScalerWin, MS_BOOL bisATV );
void Hal_ACE_GetColorMatrix( MS_BOOL bScalerWin, MS_U16* pu16Matrix);
void Hal_ACE_SetColorMatrix( MS_BOOL bScalerWin, MS_U16* pu16Matrix);
void Hal_ACE_PatchDTGColorChecker(MS_U8 u8Mode);
void Hal_ACE_SetSlopValue( MS_BOOL bScalerWin, MS_U8 u8SlopValue);
void Hal_ACE_SetFCC_En( MS_BOOL bScalerWin, MS_U8 u8Reg, MS_BOOL bEn);
void Hal_ACE_SetFCC_Cb(MS_U8 u8Reg, MS_U8 u8value);
void Hal_ACE_init_riu_base(MS_U32 u32riu_base);
void Hal_ACE_SetSharpness(MS_BOOL bScalerWin, MS_U8 u8Sharpness);
void Hal_ACE_SetSkipWaitVsync( MS_BOOL bIsMainSkipWaitVsyn,MS_BOOL bIsSubSkipWaitVsyn);
MS_BOOL Hal_ACE_GetSkipWaitVsync(  MS_BOOL bScalerWin);

void Hal_ACE_SetBrightness( MS_BOOL bScalerWin, MS_U8 u8RedBrightness, MS_U8 u8GreenBrightness, MS_U8 u8BlueBrightness );
void Hal_ACE_SetBrightnessPrecise(MS_BOOL bScalerWin, MS_U16 u16RedBrightness, MS_U16 u16GreenBrightness, MS_U16 u16BlueBrightness);
void Hal_ACE_SetPostRGBGain(MS_BOOL bScalerWin, MS_U16 u8RedGain, MS_U16 u8GreenGain, MS_U16 u8BlueGain);
void Hal_ACE_SetPostRGBOffset(MS_BOOL bScalerWin, MS_U16 u16RedOffset, MS_U16 u16GreenOffset, MS_U16 u16BlueOffset);
void Hal_ACE_Set_IHC_SRAM(MS_U8 *pBuf, MS_U8 u8SRAM_Idx, MS_U16 u16Cnt);
void Hal_ACE_Set_ICC_SRAM(MS_U16 *pBuf, MS_U16 u16Cnt);
void Hal_ACE_Write_Color_Matrix_Burst( MS_BOOL bWindow, MS_U16* psMatrix );
MS_BOOL Hal_ACE_Is_Support_MLoad( MS_BOOL bWindow );

void Hal_ACE_MWESetWin(MS_U16 u16hstart, MS_U16 u16hend, MS_U16 u16vstart, MS_U16 u16vend);
void Hal_ACE_MWEEnable(MS_BOOL ben, MS_BOOL bLoadFromTable);
void Hal_ACE_MWESetBorder( MS_BOOL bScalerWin, MS_U16 u16color, MS_U16 u16_l, MS_U16 u16_r, MS_U16 u16_t, MS_U16 u16_d);
void Hal_ACE_MWECloneVisualEffect(void);
void Hal_ACE_3DClonePQMap(MS_BOOL bHWeaveOut, MS_BOOL bVWeaveOut);

MS_U8 Hal_ACE_DNR_GetMotion(void);
MS_U8 Hal_ACE_DNR_GetCoringThreshold(MS_BOOL bScalerWin);
MS_U8 Hal_ACE_DNR_GetSharpnessAdjust(MS_BOOL bScalerWin);
MS_U8 Hal_ACE_DNR_GetGuassin_SNR_Threshold(MS_BOOL bScalerWin);


void Hal_ACE_DNR_SetCoringThreshold(MS_BOOL bScalerWin, MS_U16 u16val);
void Hal_ACE_DNR_SetSharpnessAdjust(MS_BOOL bScalerWin, MS_U16 u16val);
void Hal_ACE_DNR_SetNM_V(MS_BOOL bScalerWin, MS_U16 u16val);
void Hal_ACE_DNR_SetGNR_0(MS_BOOL bScalerWin, MS_U16 u16val);
void Hal_ACE_DNR_SetGNR_1(MS_BOOL bScalerWin, MS_U16 u16val);
void Hal_ACE_DNR_SetCP(MS_BOOL bScalerWin, MS_U16 u16val);
void Hal_ACE_DNR_SetDP(MS_BOOL bScalerWin, MS_U16 u16val);
void Hal_ACE_DNR_SetNM_H_0(MS_BOOL bScalerWin, MS_U16 u16val);
void Hal_ACE_DNR_SetNM_H_1(MS_BOOL bScalerWin, MS_U16 u16val);
void Hal_ACE_DNR_SetGray_Ground_Gain(MS_U16 u16val);
void Hal_ACE_DNR_SetGray_Ground_En(MS_BOOL bScalerWin, MS_U16 u16val);
void Hal_ACE_DNR_SetSC_Coring(MS_BOOL bScalerWin, MS_U16 u16val);
void Hal_ACE_DNR_SetSpikeNR_0(MS_U16 u16val);
void Hal_ACE_DNR_SetSpikeNR_1(MS_U16 u16val);
void Hal_ACE_DNR_SetSNR_NM(MS_U16 u16val);
void Hal_ACE_DNR_SetBank_Coring(MS_BOOL bScalerWin, MS_U16 u16val);
void Hal_ACE_DNR_SetGuassin_SNR_Threshold(MS_BOOL bScalerWin, MS_U16 u16val);
void Hal_ACE_DNR_SetNRTbl_Y(MS_U8 u8Idx, MS_U16 u16val);
void Hal_ACE_DNR_SetNRTbl_C(MS_U8 u8Idx, MS_U16 u16val);

// Function in mdrv_xc_menuload.c
typedef enum
{
    E_MLOAD_UNSUPPORTED = 0,
    E_MLOAD_DISABLED    = 1,
    E_MLOAD_ENABLED     = 2,
}MLOAD_TYPE;

typedef enum
{
    E_MLG_UNSUPPORTED = 0,
    E_MLG_DISABLED    = 1,
    E_MLG_ENABLED     = 2,
}MLG_TYPE;

typedef enum
{
    MAIN_WINDOW=0,       ///< main window if with PIP or without PIP
    SUB_WINDOW=1,         ///< sub window if PIP

    SC1_MAIN_WINDOW=2,   ///< main window if with PIP or without PIP
    SC1_SUB_WINDOW,    ///< sub window if PIP
    SC2_MAIN_WINDOW,   ///< main window if with PIP or without PIP
    SC2_SUB_WINDOW,    ///< sub window if PIP

    MAX_WINDOW             /// The max support window
}SCALER_WIN;

extern MS_BOOL MApi_XC_MLoad_Fire(MS_BOOL bImmediate);
extern MS_BOOL MApi_XC_MLoad_WriteCmd(MS_U32 u32Addr, MS_U16 u16Data, MS_U16 u16Mask);
extern MS_BOOL MApi_XC_MLoad_WriteCmds_And_Fire(MS_U32 *pu32Addr, MS_U16 *pu16Data, MS_U16 *pu16Mask, MS_U16 u16CmdCnt);
extern MS_BOOL MApi_XC_MLoad_WriteCmd_And_Fire(MS_U32 u32Addr, MS_U16 u16Data, MS_U16 u16Mask);
extern MLOAD_TYPE MApi_XC_MLoad_GetStatus(SCALER_WIN eWindow);

#ifdef MSOS_TYPE_CE
 #ifdef ASIC_VERIFY
    extern MS_S32 _MLOAD_MUTEX;
 #else
    __declspec(dllimport) MS_S32 _MLOAD_MUTEX;
 #endif

#else

extern MS_S32 _MLOAD_MUTEX;

#endif

#define MLOAD_MUTEX
#define MLOAD_MUTEX_DBG 0

#ifdef MLOAD_MUTEX
    #if(MLOAD_MUTEX_DBG)
        #define _MLOAD_ENTRY()                                                         \
                printf("1,==========================\n");                              \
                printf("[%s][%s][%06d]\n",__FILE__,__FUNCTION__,__LINE__);    \
                if(!MsOS_ObtainMutex(_MLOAD_MUTEX, MSOS_WAIT_FOREVER))                 \
                {                                                                       \
                    printf("==========================\n");                             \
                    printf("[%s][%s][%06d] Mutex taking timeout\n",__FILE__,__FUNCTION__,__LINE__);    \
                }
        #define _MLOAD_RETURN()                                                       \
                printf("0,==========================\n");                              \
                printf("[%s][%s][%06d] \n",__FILE__,__FUNCTION__,__LINE__);    \
                MsOS_ReleaseMutex(_MLOAD_MUTEX);
                                    //return _ret;
    #else
        #define _MLOAD_ENTRY()                                               \
                if(!MsOS_ObtainMutex(_MLOAD_MUTEX, MSOS_WAIT_FOREVER))      \
                {                                                            \
                }
        #define _MLOAD_RETURN()    MsOS_ReleaseMutex(_MLOAD_MUTEX);
                                    //return _ret;
    #endif
#else // #if not def MLOAD_MUTEX
    #define _MLOAD_ENTRY()         while(0)
    #define _MLOAD_RETURN()    while(0)
#endif // #ifdef _MLOAD_MUTEX

#endif


