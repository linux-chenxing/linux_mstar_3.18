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
/// file    drv_ACE.c
/// @brief  Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////


#define  _MDRV_ACE_C_
#ifdef MSOS_TYPE_LINUX_KERNEL
#include <linux/string.h>
#include <linux/kernel.h>
#else
#include <string.h>
#endif
#include "MsCommon.h"
#include "mhal_ace.h"
#include "drvACE.h"
#include "hwreg_ace.h"
#include "ace_hwreg_utility2.h"
#include "MsAce_LIB.h"

#ifdef MULTI_SCALER_SUPPORTED
#include "hwreg_ace1.h"
#include "hwreg_ace2.h"
#endif

////////////////////////////////////////////////////////////////////////////////
//
//  ACE library function start
//
////////////////////////////////////////////////////////////////////////////////

MS_U8 ACE_VBuf[32] =  {
    'M','S','V','C','0','0',                //0,0 fixed
    'A','4',                                //LIB ID
    '0','0',                                //Interface
    '0','1','0','4',                        //Build number
    '0','0','0','8','9','9','8','3',        // Change List
    'A','E','0','0','0','0','0','0','0',    // AE:Saturn4LE  0: MStar Trunk
    'T'                                     // check sum
};

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define MAIN_WINDOW     0
#define SUB_WINDOW      1

#ifdef MULTI_SCALER_SUPPORTED
#define SC1_MAIN_WINDOW     2
#define SC1_SUB_WINDOW      3
#define SC2_MAIN_WINDOW     4
#define SC2_SUB_WINDOW      5
#endif

typedef struct
{
    MS_U16 u16RedGain;
    MS_U16 u16GreenGain;
    MS_U16 u16BlueGain;
} ACE_PostRGBGainSetting;

static ACE_PostRGBGainSetting stACEPostGainSetting[MAX_WINDOW_NUM];

/******************************************************************************/
/*                   Function Prototypes                                      */
/******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_ACE_GetVersion()
/// @brief \b Function \b Description:  Get version of ACE library
/// @param <IN>        \b None
/// @param <OUT>       \b None
/// @param <RET>       \b Version
/// @param <GLOBAL>    \b None
////////////////////////////////////////////////////////////////////////////////
MS_U16 MDrv_ACE_GetVersion(void)
{
    return msACE_GetVersion();
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: void MDrv_ACE_Init(  MS_BOOL bScalerWin, MS_S16* psColorCorrectionTable, MS_S16* psPCsRGBTable )
/// @brief \b Function \b Description:  Initialize ACE varible
/// @param <IN>        \b None
/// @param <OUT>       \b None
/// @param <RET>       \b None
/// @param <GLOBAL>    \b None
////////////////////////////////////////////////////////////////////////////////

void MDrv_ACE_Init(  MS_BOOL bScalerWin, MS_S16* psColorCorrectionTable, MS_S16* psPCsRGBTable )
{
    memset(stACEPostGainSetting, 0, sizeof(stACEPostGainSetting));

    InitACEVar();
    msACEPCForceYUVtoRGB(bScalerWin, TRUE);
    msACESetColorCorrectionTable(bScalerWin, psColorCorrectionTable);
    msACESetPCsRGBTable(bScalerWin, psPCsRGBTable);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_ACE_PCForceYUVtoRGB()
/// @brief \b Function \b Description:  Force transfer YUV to RGB
/// @param <IN>        \b bScalerWin
/// @param <IN>        \b bEnable
/// @param <OUT>       \b None
/// @param <RET>       \b None
/// @param <GLOBAL>    \b None
////////////////////////////////////////////////////////////////////////////////
void MDrv_ACE_PCForceYUVtoRGB( MS_BOOL bScalerWin, MS_BOOL bEnable )
{
    msACEPCForceYUVtoRGB(bScalerWin, bEnable);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_ACE_PicSetContrast()
/// @brief \b Function \b Description:  Adjust Contrast
/// @param <IN>        \b bScalerWin
/// @param <IN>        \b u8Contrast
/// @param <OUT>       \b None
/// @param <RET>       \b None
/// @param <GLOBAL>    \b None
////////////////////////////////////////////////////////////////////////////////
void MDrv_ACE_PicSetContrast( MS_BOOL bScalerWin, MS_BOOL bUseYUVSpace, MS_U8 u8Contrast )
{
    if(bUseYUVSpace)
    {
        msAdjustVideoContrast(bScalerWin, u8Contrast);
    }
    else
    {
        msAdjustPCContrast(bScalerWin, u8Contrast);
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_ACE_PicSetContrast()
/// @brief \b Function \b Description:  Adjust Contrast only on G channel
/// @param <IN>        \b bScalerWin
/// @param <IN>        \b u8Contrast
/// @param <OUT>       \b None
/// @param <RET>       \b None
/// @param <GLOBAL>    \b None
////////////////////////////////////////////////////////////////////////////////
void MDrv_ACE_PicSetContrast_G( MS_BOOL bScalerWin, MS_BOOL bUseYUVSpace, MS_U8 u8Contrast )
{
    if(bUseYUVSpace)
    {
        msAdjustVideoContrast_G(bScalerWin, u8Contrast);
    }
    else
    {
        msAdjustPCContrast_G(bScalerWin, u8Contrast);
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_ACE_PicSetRGB()
/// @brief \b Function \b Description:  Adjust RGB
/// @param <IN>        \b bScalerWin
/// @param <IN>        \b u8Red
/// @param <IN>        \b u8Green
/// @param <IN>        \b u8Blue
/// @param <OUT>       \b None
/// @param <RET>       \b None
/// @param <GLOBAL>    \b None
////////////////////////////////////////////////////////////////////////////////
void MDrv_ACE_PicSetRGB( MS_BOOL bScalerWin, MS_BOOL bUseYUVSpace, MS_U8 u8Red, MS_U8 u8Green, MS_U8 u8Blue )
{
    if(bUseYUVSpace)
    {
        msAdjustVideoRGB(bScalerWin, u8Red, u8Green, u8Blue);
    }
    else
    {
        msAdjustPCRGB(bScalerWin, u8Red, u8Green, u8Blue);
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_ACE_PicSetPostRGBGain()
/// @brief \b Function \b Description:  Adjust Post RGB Gain
/// @param <IN>        \b bScalerWin
/// @param <IN>        \b u8RedGain
/// @param <IN>        \b u8GreenGain
/// @param <IN>        \b u8BlueGain
/// @param <OUT>       \b None
/// @param <RET>       \b None
/// @param <GLOBAL>    \b None
////////////////////////////////////////////////////////////////////////////////
void MDrv_ACE_PicSetPostRGBGain(MS_BOOL bScalerWin, MS_U16 u8RedGain, MS_U16 u8GreenGain, MS_U16 u8BlueGain)
{
    MS_U8 u8Idx;

    u8Idx = (bScalerWin == TRUE) ? 1 : 0;

    stACEPostGainSetting[u8Idx].u16RedGain = u8RedGain ;
    stACEPostGainSetting[u8Idx].u16GreenGain = u8GreenGain ;
    stACEPostGainSetting[u8Idx].u16BlueGain = u8BlueGain ;

    Hal_ACE_SetPostRGBGain(bScalerWin,
        stACEPostGainSetting[u8Idx].u16RedGain,
        stACEPostGainSetting[u8Idx].u16GreenGain,
        stACEPostGainSetting[u8Idx].u16BlueGain);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_ACE_PicGetPostRGBGain()
/// @brief \b Function \b Description:  Get ACE Post RGB Gain Info
/// @param <IN>        \b bScalerWin
/// @param <IN>        \b ucInfoType
/// @param <OUT>       \b None
/// @param <RET>       \b wReturnValue
/// @param <GLOBAL>    \b None
////////////////////////////////////////////////////////////////////////////////
MS_U16 MDrv_ACE_PicGetPostRGBGain(MS_BOOL bWindow, MS_U16 u16ACEinfo)
{
    EnuACEInfoType eACEInfoType   = (EnuACEInfoType) u16ACEinfo;
    MS_U16         u16ReturnValue = 0;
    MS_U8           u8Idx;

    u8Idx = (bWindow == TRUE) ? 1 : 0;

    switch( eACEInfoType )
    {
        case ACE_INFO_TYPE_R_GAIN:
            u16ReturnValue = stACEPostGainSetting[u8Idx].u16RedGain;
            break;

        case ACE_INFO_TYPE_G_GAIN:
	        u16ReturnValue = stACEPostGainSetting[u8Idx].u16GreenGain;
	        break;

        case ACE_INFO_TYPE_B_GAIN:
            u16ReturnValue = stACEPostGainSetting[u8Idx].u16BlueGain;
            break;

        default:
        {
            //MS_DEBUG_MSG(printf("Get Post RGB Gain fails\n"));
            u16ReturnValue = 0;
            break;
        }
    }
    return u16ReturnValue;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_ACE_PicSetPostRGBOffset()
/// @brief \b Function \b Description:  Adjust Post RGB Offset
/// @param <IN>        \b bScalerWin
/// @param <IN>        \b u16RedOffset
/// @param <IN>        \b u16GreenOffset
/// @param <IN>        \b u16BlueOffset
/// @param <IN>        \b u8Step
/// @param <OUT>       \b None
/// @param <RET>       \b None
/// @param <GLOBAL>    \b None
////////////////////////////////////////////////////////////////////////////////
void MDrv_ACE_PicSetPostRGBOffset(MS_BOOL bScalerWin, MS_U16 u16RedOffset, MS_U16 u16GreenOffset, MS_U16 u16BlueOffset, MS_U8 u8Step)
{
    Hal_ACE_SetPostRGBOffset(bScalerWin, u16RedOffset << u8Step, u16GreenOffset << u8Step, u16BlueOffset << u8Step);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_ACE_PicGetPostRGBGain()
/// @brief \b Function \b Description:  Get ACE Post RGB Gain Info
/// @param <IN>        \b bScalerWin
/// @param <IN>        \b ucInfoType
/// @param <OUT>       \b None
/// @param <RET>       \b wReturnValue
/// @param <GLOBAL>    \b None
////////////////////////////////////////////////////////////////////////////////
MS_U16 MDrv_ACE_PicGetPostRGBOffset(MS_BOOL bWindow, MS_U16 u16ACEinfo)
{
    EnuACEInfoType eACEInfoType   = (EnuACEInfoType) u16ACEinfo;
    MS_U16         u16ReturnValue = 0;

    switch( eACEInfoType )
    {
        case ACE_INFO_TYPE_COLOR_CORRECTION_OFFSET_R:
	 	{
            if(MAIN_WINDOW == bWindow)
			{
				u16ReturnValue = SC_R2BYTEMSK(REG_SC_BK25_21_L, 0x07FF);
			}
			else if(SUB_WINDOW == bWindow)
			{
				u16ReturnValue = SC_R2BYTEMSK(REG_SC_BK25_27_L, 0x07FF);
			}
        #ifdef MULTI_SCALER_SUPPORTED
			else if(SC1_MAIN_WINDOW == bWindow)
            {
				u16ReturnValue = SC_R2BYTEMSK(REG_SC1_BK25_21_L, 0x07FF);
			}
			else if(SC1_SUB_WINDOW == bWindow)
			{
				u16ReturnValue = SC_R2BYTEMSK(REG_SC1_BK25_27_L, 0x07FF);
			}
			else if(SC2_MAIN_WINDOW == bWindow)
            {
				u16ReturnValue = SC_R2BYTEMSK(REG_SC2_BK25_21_L, 0x07FF);
			}
			else // SC2_SUB_WINDOW
			{
				u16ReturnValue = SC_R2BYTEMSK(REG_SC2_BK25_27_L, 0x07FF);
			}
        #endif
			break;
		}
        case ACE_INFO_TYPE_COLOR_CORRECTION_OFFSET_G:
		{
			if(MAIN_WINDOW == bWindow)
			{
				u16ReturnValue = SC_R2BYTEMSK(REG_SC_BK25_22_L, 0x07FF);
			}
			else if(SUB_WINDOW == bWindow)
			{
				u16ReturnValue = SC_R2BYTEMSK(REG_SC_BK25_28_L, 0x07FF);
			}
        #ifdef MULTI_SCALER_SUPPORTED
			else if(SC1_MAIN_WINDOW == bWindow)
			{
				u16ReturnValue = SC_R2BYTEMSK(REG_SC1_BK25_22_L, 0x07FF);
			}
			else if(SC1_SUB_WINDOW == bWindow)
			{
				u16ReturnValue = SC_R2BYTEMSK(REG_SC2_BK25_28_L, 0x07FF);
			}
			else if(SC2_MAIN_WINDOW == bWindow)
			{
				u16ReturnValue = SC_R2BYTEMSK(REG_SC2_BK25_22_L, 0x07FF);
			}
			else // SC2_SUB_WINDOW
			{
				u16ReturnValue = SC_R2BYTEMSK(REG_SC2_BK25_28_L, 0x07FF);
			}
        #endif
			break;
        }
        case ACE_INFO_TYPE_COLOR_CORRECTION_OFFSET_B:
		{
			if(MAIN_WINDOW == bWindow)
			{
				u16ReturnValue = SC_R2BYTEMSK(REG_SC_BK25_23_L, 0x07FF);
			}
			else if(SUB_WINDOW == bWindow)
			{
				u16ReturnValue = SC_R2BYTEMSK(REG_SC_BK25_29_L, 0x07FF);
			}
        #ifdef MULTI_SCALER_SUPPORTED
			else if(SC1_MAIN_WINDOW == bWindow)
            {
				u16ReturnValue = SC_R2BYTEMSK(REG_SC1_BK25_23_L, 0x07FF);
			}
			else if(SC1_SUB_WINDOW == bWindow)
			{
				u16ReturnValue = SC_R2BYTEMSK(REG_SC1_BK25_29_L, 0x07FF);
			}
			else if(SC2_MAIN_WINDOW == bWindow)
			{
				u16ReturnValue = SC_R2BYTEMSK(REG_SC2_BK25_23_L, 0x07FF);
			}
			else // SC2_SUB_WINDOW
			{
				u16ReturnValue = SC_R2BYTEMSK(REG_SC2_BK25_29_L, 0x07FF);
			}
        #endif
			break;
        }
        default:
        {
            //MS_DEBUG_MSG(printf("Get Post RGB Offset fails\n"));
            u16ReturnValue = 0;
            break;
        }
    }
    return u16ReturnValue;
}

MS_U8 MDrv_ACE_GetPostRGBGain(MS_BOOL bScalerWin, MS_U16 u16ACEinfo)
{
    EnuACEInfoType eACEInfoType;
    MS_U8 ret = 0;

    eACEInfoType = (EnuACEInfoType) u16ACEinfo;

    if(MAIN_WINDOW == bScalerWin)
    {
        switch(eACEInfoType)
        {
            case ACE_INFO_TYPE_R_GAIN:
                ret = (MS_U8)(SC_R2BYTEMSK(POST_R_CHANNEL_GAIN_MAIN, POST_R_CHANNEL_GAIN_MASK)>> 3);
                break;
            case ACE_INFO_TYPE_G_GAIN:
                ret = (MS_U8)(SC_R2BYTEMSK(POST_G_CHANNEL_GAIN_MAIN, POST_G_CHANNEL_GAIN_MASK)>> 3);
                break;
            case ACE_INFO_TYPE_B_GAIN:
                ret = (MS_U8)(SC_R2BYTEMSK(POST_B_CHANNEL_GAIN_MAIN, POST_B_CHANNEL_GAIN_MASK)>> 3);
                break;
            default:
                ret = 0;
                break;
        }
    }
#if (MAX_WINDOW_NUM > 1)
    else if(SUB_WINDOW == bScalerWin)
    {
        switch(eACEInfoType)
        {
            case ACE_INFO_TYPE_R_GAIN:
                ret = (MS_U8)(SC_R2BYTEMSK(POST_R_CHANNEL_GAIN_SUB, POST_R_CHANNEL_GAIN_MASK)>>3);
                break;
            case ACE_INFO_TYPE_G_GAIN:
                ret = (MS_U8)(SC_R2BYTEMSK(POST_G_CHANNEL_GAIN_SUB, POST_G_CHANNEL_GAIN_MASK)>>3);
                break;
            case ACE_INFO_TYPE_B_GAIN:
                ret = (MS_U8)(SC_R2BYTEMSK(POST_B_CHANNEL_GAIN_SUB, POST_B_CHANNEL_GAIN_MASK)>>3);
                break;
            default:
                ret = 0;
                break;
        }
    }
#ifdef MULTI_SCALER_SUPPORTED
    else if(SC1_MAIN_WINDOW == bScalerWin)
    {
        switch(eACEInfoType)
        {
            case ACE_INFO_TYPE_R_GAIN:
                ret = (MS_U8)(SC_R2BYTEMSK(POST_R_CHANNEL_GAIN_SC1_MAIN, POST_R_CHANNEL_GAIN_MASK)>> 3);
                break;
            case ACE_INFO_TYPE_G_GAIN:
                ret = (MS_U8)(SC_R2BYTEMSK(POST_G_CHANNEL_GAIN_SC1_MAIN, POST_G_CHANNEL_GAIN_MASK)>> 3);
                break;
            case ACE_INFO_TYPE_B_GAIN:
                ret = (MS_U8)(SC_R2BYTEMSK(POST_B_CHANNEL_GAIN_SC1_MAIN, POST_B_CHANNEL_GAIN_MASK)>> 3);
                break;
            default:
                ret = 0;
                break;
        }
    }
    else if(SC1_SUB_WINDOW == bScalerWin)
    {
        switch(eACEInfoType)
        {
            case ACE_INFO_TYPE_R_GAIN:
                ret = (MS_U8)(SC_R2BYTEMSK(POST_R_CHANNEL_GAIN_SC1_SUB, POST_R_CHANNEL_GAIN_MASK)>>3);
                break;
            case ACE_INFO_TYPE_G_GAIN:
                ret = (MS_U8)(SC_R2BYTEMSK(POST_G_CHANNEL_GAIN_SC1_SUB, POST_G_CHANNEL_GAIN_MASK)>>3);
                break;
            case ACE_INFO_TYPE_B_GAIN:
                ret = (MS_U8)(SC_R2BYTEMSK(POST_B_CHANNEL_GAIN_SC1_SUB, POST_B_CHANNEL_GAIN_MASK)>>3);
                break;
            default:
                ret = 0;
                break;
        }
    }
    else if(SC2_MAIN_WINDOW == bScalerWin)
    {
        switch(eACEInfoType)
        {
            case ACE_INFO_TYPE_R_GAIN:
                ret = (MS_U8)(SC_R2BYTEMSK(POST_R_CHANNEL_GAIN_SC2_MAIN, POST_R_CHANNEL_GAIN_MASK)>> 3);
                break;
            case ACE_INFO_TYPE_G_GAIN:
                ret = (MS_U8)(SC_R2BYTEMSK(POST_G_CHANNEL_GAIN_SC2_MAIN, POST_G_CHANNEL_GAIN_MASK)>> 3);
                break;
            case ACE_INFO_TYPE_B_GAIN:
                ret = (MS_U8)(SC_R2BYTEMSK(POST_B_CHANNEL_GAIN_SC2_MAIN, POST_B_CHANNEL_GAIN_MASK)>> 3);
                break;
            default:
                ret = 0;
                break;
        }
    }
    else // SC2_SUB_WINDOW
    {
        switch(eACEInfoType)
        {
            case ACE_INFO_TYPE_R_GAIN:
                ret = (MS_U8)(SC_R2BYTEMSK(POST_R_CHANNEL_GAIN_SC2_SUB, POST_R_CHANNEL_GAIN_MASK)>>3);
                break;
            case ACE_INFO_TYPE_G_GAIN:
                ret = (MS_U8)(SC_R2BYTEMSK(POST_G_CHANNEL_GAIN_SC2_SUB, POST_G_CHANNEL_GAIN_MASK)>>3);
                break;
            case ACE_INFO_TYPE_B_GAIN:
                ret = (MS_U8)(SC_R2BYTEMSK(POST_B_CHANNEL_GAIN_SC2_SUB, POST_B_CHANNEL_GAIN_MASK)>>3);
                break;
            default:
                ret = 0;
                break;
        }
    }
#endif // MULTI_SCALER_SUPPORTED

#else
    else
    {
        ret = 0;
    }
#endif
    //MS_DEBUG_MSG(printf("[MDrv_ACE_GetPostRGBGain] ret = %d\n", ret));
    return ret;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_ACE_SelectYUVtoRGBMatrix()
/// @brief \b Function \b Description:  Select YUV to RGB Matrix
/// @param <IN>        \b bScalerWin
/// @param <IN>        \b ucMatrix
/// @param <IN>        \b psUserYUVtoRGBMatrix
/// @param <OUT>       \b None
/// @param <RET>       \b None
/// @param <GLOBAL>    \b None
////////////////////////////////////////////////////////////////////////////////
void MDrv_ACE_SelectYUVtoRGBMatrix( MS_BOOL bScalerWin, MS_U8 ucMatrix, MS_S16* psUserYUVtoRGBMatrix )
{
    msACESelectYUVtoRGBMatrix(bScalerWin, ucMatrix, psUserYUVtoRGBMatrix);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_ACE_SetColorCorrectionTable()
/// @brief \b Function \b Description:  Set Color Correction Table
/// @param <IN>        \b bScalerWin
/// @param <IN>        \b psColorCorrectionTable
/// @param <OUT>       \b None
/// @param <RET>       \b None
/// @param <GLOBAL>    \b None
////////////////////////////////////////////////////////////////////////////////
void MDrv_ACE_SetColorCorrectionTable( MS_BOOL bScalerWin)
{
    msSetVideoColorMatrix( bScalerWin );
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_ACE_SetPCsRGBTable()
/// @brief \b Function \b Description:  Set PCs RGB Table
/// @param <IN>        \b bScalerWin
/// @param <IN>        \b psPCsRGBTable
/// @param <OUT>       \b None
/// @param <RET>       \b None
/// @param <GLOBAL>    \b None
////////////////////////////////////////////////////////////////////////////////
void MDrv_ACE_SetPCsRGBTable( MS_BOOL bScalerWin)
{
    msSetPCColorMatrix( bScalerWin);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_ACE_SetSaturation()
/// @brief \b Function \b Description:  Adjust Video Saturation
/// @param <IN>        \b bScalerWin
/// @param <IN>        \b u8Saturation
/// @param <OUT>       \b None
/// @param <RET>       \b None
/// @param <GLOBAL>    \b None
////////////////////////////////////////////////////////////////////////////////
void MDrv_ACE_SetSaturation( MS_BOOL bScalerWin, MS_U8 u8Saturation )
{
    msAdjustVideoSaturation(bScalerWin, u8Saturation);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_ACE_SetHue()
/// @brief \b Function \b Description:  Adjust Video Hue
/// @param <IN>        \b bScalerWin
/// @param <IN>        \b u8Hue
/// @param <OUT>       \b None
/// @param <RET>       \b None
/// @param <GLOBAL>    \b None
////////////////////////////////////////////////////////////////////////////////
void MDrv_ACE_SetHue( MS_BOOL bScalerWin, MS_U8 u8Hue )
{
    msAdjustVideoHue(bScalerWin, u8Hue);
}

MS_BOOL MDrv_ACE_IsPostRGBGain(MS_BOOL bWindow)
{
    MS_BOOL bRet;

    if (MAIN_WINDOW == bWindow)
    {
        bRet = SC_R2BYTEMSK(REG_SC_BK25_01_L, BIT(5)) ? TRUE: FALSE;
    }
    else if(SUB_WINDOW == bWindow)
    {
        bRet = SC_R2BYTEMSK(REG_SC_BK25_11_L, BIT(5)) ? TRUE: FALSE;
    }
#ifdef MULTI_SCALER_SUPPORTED
    else if(SC1_MAIN_WINDOW == bWindow)
    {
        bRet = SC_R2BYTEMSK(REG_SC1_BK25_01_L, BIT(5)) ? TRUE: FALSE;
    }
    else if(SC1_SUB_WINDOW == bWindow)
    {
        bRet = SC_R2BYTEMSK(REG_SC1_BK25_11_L, BIT(5)) ? TRUE: FALSE;
    }
    else if(SC2_MAIN_WINDOW == bWindow)
    {
        bRet = SC_R2BYTEMSK(REG_SC2_BK25_01_L, BIT(5)) ? TRUE: FALSE;
    }
    else // SC2_SUB_WINDOW
    {
        bRet = SC_R2BYTEMSK(REG_SC2_BK25_11_L, BIT(5)) ? TRUE: FALSE;
    }
#endif
    return bRet;
}

MS_BOOL MDrv_ACE_IsPostRGBOffset(MS_BOOL bWindow)
{
    MS_BOOL bRet;

    if (MAIN_WINDOW == bWindow)
    {
        bRet = SC_R2BYTEMSK(REG_SC_BK25_01_L, BIT(4)) ? TRUE: FALSE;
    }
    else if(SUB_WINDOW == bWindow)
    {
        bRet = SC_R2BYTEMSK(REG_SC_BK25_11_L, BIT(4)) ? TRUE: FALSE;
    }
#ifdef MULTI_SCALER_SUPPORTED
    else if(SC1_MAIN_WINDOW == bWindow)
    {
        bRet = SC_R2BYTEMSK(REG_SC1_BK25_01_L, BIT(4)) ? TRUE: FALSE;
    }
    else if(SC1_SUB_WINDOW == bWindow)
    {
        bRet = SC_R2BYTEMSK(REG_SC1_BK25_11_L, BIT(4)) ? TRUE: FALSE;
    }
    else if(SC2_MAIN_WINDOW == bWindow)
    {
        bRet = SC_R2BYTEMSK(REG_SC2_BK25_01_L, BIT(4)) ? TRUE: FALSE;
    }
    else // SC2_SUB_WINDOW
    {
        bRet = SC_R2BYTEMSK(REG_SC2_BK25_11_L, BIT(4)) ? TRUE: FALSE;
    }
#endif
    return bRet;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_ACE_GetInfo()
/// @brief \b Function \b Description:  Get ACE Info
/// @param <IN>        \b bScalerWin
/// @param <IN>        \b ucInfoType
/// @param <OUT>       \b None
/// @param <RET>       \b wReturnValue
/// @param <GLOBAL>    \b None
////////////////////////////////////////////////////////////////////////////////
MS_U16 MDrv_ACE_GetACEInfo( MS_BOOL bWindow, MS_U16 u16ACEinfo )
{
    EnuACEInfoType eACEInfoType;
    eACEInfoType = (EnuACEInfoType) u16ACEinfo;

    if(SC_R2BYTEMSK(REG_SC_BK25_01_L, BIT(5)) && ((eACEInfoType == ACE_INFO_TYPE_R_GAIN) || (eACEInfoType == ACE_INFO_TYPE_G_GAIN) || (eACEInfoType == ACE_INFO_TYPE_B_GAIN)))
    {
        return MDrv_ACE_PicGetPostRGBGain(bWindow, u16ACEinfo);
    }
    //else if( SC_R2BYTEMSK(REG_SC_BK25_01_L, BIT(4)) && ((eACEInfoType == ACE_INFO_TYPE_COLOR_CORRECTION_OFFSET_R) || (eACEInfoType == ACE_INFO_TYPE_COLOR_CORRECTION_OFFSET_G) || (eACEInfoType == ACE_INFO_TYPE_COLOR_CORRECTION_OFFSET_B)))
    //{
    //    return MDrv_ACE_PicGetPostRGBOffset( bWindow, u16ACEinfo);
    //}
    else return msACE_GetInfo(bWindow, eACEInfoType);
}

////////////////////////////////////////////////////////////////////////////////
//
//  ACE library function end
//
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//
//  DrvACE.c start
//
////////////////////////////////////////////////////////////////////////////////
void MDrv_ACE_DMS( MS_BOOL bScalerWin, MS_BOOL bisATV )
{
    Hal_ACE_DMS(bScalerWin, bisATV);
}

void MDrv_ACE_GetColorMatrix( MS_BOOL bScalerWin, MS_U16* pu16Matrix )
{
    Hal_ACE_GetColorMatrix(bScalerWin, pu16Matrix);
}

void MDrv_ACE_SetColorMatrix( MS_BOOL bScalerWin, MS_U16* pu16Matrix )
{
    Hal_ACE_SetColorMatrix(bScalerWin, pu16Matrix);
}

void MDrv_ACE_PatchDTGColorChecker( MS_U8 u8Mode)
{
    Hal_ACE_PatchDTGColorChecker(u8Mode);
}


void MDrv_ACE_SetFleshTone( MS_BOOL bScalerWin, MS_BOOL bEn, MS_U8 u8FleshTone)
{
    Hal_ACE_SetFCC_En(bScalerWin, 0, bEn);
    Hal_ACE_SetFCC_Cb(0, u8FleshTone);
}

void MDrv_ACE_SetBlackAdjust( MS_BOOL bScalerWin, MS_U8 u8BlackAdjust)
{
    Hal_ACE_SetSlopValue(bScalerWin, u8BlackAdjust);
}

void MDrv_ACE_init_riu_base( MS_U32 u32riu_base )
{
    Hal_ACE_init_riu_base(u32riu_base);
}

void MDrv_ACE_SetSharpness( MS_BOOL bScalerWin, MS_U8 u8Sharpness )
{
    Hal_ACE_SetSharpness(bScalerWin, u8Sharpness);
}

/******************************************************************************/
///Set brightness
///@param u8Brightness \b IN
///- brightness value from 0 -0xff
/******************************************************************************/
void MDrv_ACE_SetBrightness ( MS_BOOL bScalerWin, MS_U8 u8RedBrightness, MS_U8 u8GreenBrightness, MS_U8 u8BlueBrightness )
{
    Hal_ACE_SetBrightness(bScalerWin, u8RedBrightness, u8GreenBrightness, u8BlueBrightness);
}

/******************************************************************************/
///Set brightness
///@param u16Brightness \b IN
///- brightness value from 0-0x7ff
/******************************************************************************/
void MDrv_ACE_SetBrightnessPrecise(MS_BOOL bScalerWin, MS_U16 u16RedBrightness, MS_U16 u16GreenBrightness, MS_U16 u16BlueBrightness)
{
    Hal_ACE_SetBrightnessPrecise(bScalerWin, u16RedBrightness, u16GreenBrightness, u16BlueBrightness);
}


////////////////////////////////////////////////////////////////////////////////
//
//  MWE start
//
////////////////////////////////////////////////////////////////////////////////

static PanelSizeType MWEPanel;

MS_U8 MWE_Border_Color[] =
{
    0x00, // E_XC_FREE_RUN_COLOR_BLACK
    0xFF, // E_XC_FREE_RUN_COLOR_WHITE
    0x03, // E_XC_FREE_RUN_COLOR_BLUE
    0xE0, // E_XC_FREE_RUN_COLOR_RED
    0x1C, // E_XC_FREE_RUN_COLOR_GREEN
};

void MDrv_XC_ACE_MWEinit(MS_U16 u16MWEHstart, MS_U16 u16MWEVstart, MS_U16 u16MWEWidth, MS_U16 u16MWEHeight)
{
    MWEPanel.u16Pnl_Height = u16MWEHeight;
    MWEPanel.u16Pnl_Width  = u16MWEWidth;
    MWEPanel.u16Pnl_Hstart = u16MWEHstart;
    MWEPanel.u16Pnl_Vstart = u16MWEVstart;
}

//===============================================================================

void MDrv_XC_ACE_MWESetWin(MWE_RECT *pRect)
{
    MS_U16 u16hstart, u16hend, u16vstart, u16vend;

    u16hstart = (MS_U16)(pRect->Xpos + MWEPanel.u16Pnl_Hstart);
    u16hend   = (MS_U16)(u16hstart + pRect->Xsize - 1);
    u16vstart = (MS_U16)(pRect->Ypos + MWEPanel.u16Pnl_Vstart);
    u16vend   = (MS_U16)(u16vstart + pRect->Ysize - 1);

    if(u16hend > MWEPanel.u16Pnl_Hstart + MWEPanel.u16Pnl_Width - 1)
        u16hend = MWEPanel.u16Pnl_Hstart + MWEPanel.u16Pnl_Width - 1;

    if(u16vend > MWEPanel.u16Pnl_Vstart + MWEPanel.u16Pnl_Height - 1)
        u16vend = MWEPanel.u16Pnl_Vstart + MWEPanel.u16Pnl_Height - 1;
    //MS_DEBUG_MSG(printf("[MWE] Hstart %4x  Hend %4x  Vstart %4x Vend %4x \n",
    //        u16hstart, u16hend, u16vstart, u16vend));
    Hal_ACE_MWESetWin(u16hstart, u16hend, u16vstart, u16vend);
}

void MDrv_XC_ACE_MWECloneVisualEffect()
{
    Hal_ACE_MWECloneVisualEffect();
}

void MDrv_ACE_3DClonePQMap(MS_BOOL bHWeaveOut, MS_BOOL bVWeaveOut)
{
    Hal_ACE_3DClonePQMap(bHWeaveOut, bVWeaveOut);
}

void MDrv_XC_ACE_MWESetRegRow(MS_U32 u32Addr, MS_U8 u8Value, MS_U8 u8Mask)
{
    if(u32Addr & 0x1)
    {
        SC_W2BYTEMSK((MS_U32)(u32Addr & 0xFFFFFE), (MS_U16)u8Value << 8, (MS_U16)u8Mask << 8);
    }
    else
    {
        SC_W2BYTEMSK((MS_U32)u32Addr, (MS_U16)u8Value, (MS_U16)u8Mask);
    }
}

//===============================================================================

void MDrv_XC_ACE_MWESetBorder( MS_BOOL bScalerWin, MS_BOOL ben, MS_U8 u8color, MS_U16 u16_l, MS_U16 u16_r, MS_U16 u16_t, MS_U16 u16_d)
{
    MS_U16 u16color =  (u8color >= MWE_BORDER_COLOR_MAX)
                    ? MWE_Border_Color[MWE_BORDER_COLOR_BLACK]
                    : MWE_Border_Color[u8color];

    if(ben)
    {
        Hal_ACE_MWESetBorder(bScalerWin, u16color, u16_l, u16_r, u16_t, u16_d);
    }

}

//===============================================================================


void MDrv_XC_ACE_MWEEnable(MS_BOOL bEnable, MS_BOOL bLoadFromTable)
{
     Hal_ACE_MWEEnable(bEnable, bLoadFromTable);
}

////////////////////////////////////////////////////////////////////////////////
//
//  MWE end
//
////////////////////////////////////////////////////////////////////////////////

MS_BOOL MDrv_XC_ACE_Set_ICC_SRAM(MS_U16 *pBuf, MS_U16 u16ByteSize)
{
    if(u16ByteSize > 256*2)
        return FALSE;

    Hal_ACE_Set_ICC_SRAM(pBuf, u16ByteSize/2);
    return TRUE;
}

MS_BOOL MDrv_XC_ACE_Set_IHC_SRAM(MS_U8 *pBuf, MS_U16 u16ByteSize)
{
#if 0
    #define H_SIZE  17
    #define V_SIZE  17
    #define MAX_SRAM_SIZE 0x100
    #define BUF_BYTE_SIZE 17*17

    MS_U8 v_idx, h_idx;
    MS_U8 data, row, col;
    MS_U16 cont1, cont2, cont3, cont4;
    MS_U8 SRAM1_IHC[MAX_SRAM_SIZE];
    MS_U8 SRAM2_IHC[MAX_SRAM_SIZE];
    MS_U8 SRAM3_IHC[MAX_SRAM_SIZE];
    MS_U8 SRAM4_IHC[MAX_SRAM_SIZE];

    if(u16ByteSize > BUF_BYTE_SIZE)
        return FALSE;

    cont1 = cont2 = cont3 = cont4 = 0;

    for (v_idx=1; v_idx <= V_SIZE; v_idx++)
        for (h_idx=1; h_idx <= H_SIZE-1; h_idx++)
        {
            data = pBuf[(v_idx-1)*H_SIZE + h_idx-1]; //pBuf[v_idx-1][h_idx-1]

            row = (int)(v_idx-(v_idx/2)*2);
            col = (int)(h_idx-(h_idx/2)*2);

            if ( (row==1) & (col==1) )
            {
                SRAM1_IHC[cont1]=data;
                cont1 = cont1 < MAX_SRAM_SIZE-1 ? cont1+1 :  MAX_SRAM_SIZE-1;
            }

            if ( (row==1) & (col==0) )
            {
                SRAM2_IHC[cont2]=data;
                cont2 = cont2 < MAX_SRAM_SIZE-1 ? cont2+1 :  MAX_SRAM_SIZE-1;
            }

            if ( (row==0) & (col==1) )
            {
                SRAM3_IHC[cont3]=data;
                cont3 = cont3 < MAX_SRAM_SIZE-1 ? cont3+1 :  MAX_SRAM_SIZE-1;
            }

            if ( (row==0) & (col==0) )
            {
                SRAM4_IHC[cont4]=data;
                cont4 = cont4 < MAX_SRAM_SIZE-1 ? cont4+1 :  MAX_SRAM_SIZE-1;
            }

        }

    for (v_idx=1; v_idx <= V_SIZE; v_idx++)
    {
        row = (v_idx-(v_idx/2)*2);

        data = pBuf[(v_idx-1)*H_SIZE+16];

        if (row==1)
        {
            SRAM1_IHC[cont1]=data;
            cont1 = cont1 < MAX_SRAM_SIZE-1 ? cont1+1 :  MAX_SRAM_SIZE-1;
        }
        else
        {
            SRAM3_IHC[cont3]=data;
            cont3 = cont3 < MAX_SRAM_SIZE-1 ? cont3+1 :  MAX_SRAM_SIZE-1;
        }
    }

    Hal_ACE_Set_IHC_SRAM(&SRAM1_IHC[0], 0, cont1);
    Hal_ACE_Set_IHC_SRAM(&SRAM2_IHC[0], 1, cont2);
    Hal_ACE_Set_IHC_SRAM(&SRAM3_IHC[0], 2, cont3);
    Hal_ACE_Set_IHC_SRAM(&SRAM4_IHC[0], 3, cont4);
#endif
    return TRUE;
}

MS_U8 MDrv_XC_ACE_DNR_GetMotion(void)
{
    MS_U8 u8Ret;

    DECLARA_BANK_VARIABLE
    BACKUP_SC_BANK

    u8Ret = Hal_ACE_DNR_GetMotion();

    RECOVER_SC_BANK

    return u8Ret;
}

MS_U8 MDrv_ACE_DNR_GetCoringThreshold(MS_BOOL bScalerWin)
{
    return Hal_ACE_DNR_GetCoringThreshold(bScalerWin);
}

MS_U8 MDrv_ACE_DNR_GetSharpnessAdjust(MS_BOOL bScalerWin)
{
    return Hal_ACE_DNR_GetSharpnessAdjust(bScalerWin);
}

MS_U8 MDrv_ACE_DNR_GetGuassin_SNR_Threshold(MS_BOOL bScalerWin)
{
    return Hal_ACE_DNR_GetGuassin_SNR_Threshold(bScalerWin);
}

void MDrv_ACE_DNR_SetCoringThreshold(MS_BOOL bScalerWin, MS_U16 u16val)
{
    DECLARA_BANK_VARIABLE
    BACKUP_SC_BANK

    Hal_ACE_DNR_SetCoringThreshold(bScalerWin, u16val);

    RECOVER_SC_BANK
}

void MDrv_ACE_DNR_SetSharpnessAdjust(MS_BOOL bScalerWin, MS_U16 u16val)
{
    DECLARA_BANK_VARIABLE
    BACKUP_SC_BANK

    Hal_ACE_DNR_SetSharpnessAdjust(bScalerWin, u16val);

    RECOVER_SC_BANK

}

void MDrv_ACE_DNR_SetNM_V(MS_BOOL bScalerWin, MS_U16 u16val)
{
    DECLARA_BANK_VARIABLE
    BACKUP_SC_BANK

    Hal_ACE_DNR_SetNM_V(bScalerWin, u16val);

    RECOVER_SC_BANK
}

void MDrv_ACE_DNR_SetGNR_0(MS_BOOL bScalerWin, MS_U16 u16val)
{
    DECLARA_BANK_VARIABLE
    BACKUP_SC_BANK

    Hal_ACE_DNR_SetGNR_0(bScalerWin, u16val);

    RECOVER_SC_BANK
}

void MDrv_ACE_DNR_SetGNR_1(MS_BOOL bScalerWin, MS_U16 u16val)
{
    DECLARA_BANK_VARIABLE
    BACKUP_SC_BANK

    Hal_ACE_DNR_SetGNR_1(bScalerWin, u16val);

    RECOVER_SC_BANK
}

void MDrv_ACE_DNR_SetCP(MS_BOOL bScalerWin, MS_U16 u16val)
{
    DECLARA_BANK_VARIABLE
    BACKUP_SC_BANK

    Hal_ACE_DNR_SetCP(bScalerWin, u16val);

    RECOVER_SC_BANK
}

void MDrv_ACE_DNR_SetDP(MS_BOOL bScalerWin, MS_U16 u16val)
{
    DECLARA_BANK_VARIABLE
    BACKUP_SC_BANK

    Hal_ACE_DNR_SetDP(bScalerWin, u16val);

    RECOVER_SC_BANK
}

void MDrv_ACE_DNR_SetNM_H_0(MS_BOOL bScalerWin, MS_U16 u16val)
{
    DECLARA_BANK_VARIABLE
    BACKUP_SC_BANK

    Hal_ACE_DNR_SetNM_H_0(bScalerWin, u16val);
}

void MDrv_ACE_DNR_SetNM_H_1(MS_BOOL bScalerWin, MS_U16 u16val)
{
    DECLARA_BANK_VARIABLE
    BACKUP_SC_BANK

    Hal_ACE_DNR_SetNM_H_1(bScalerWin, u16val);

    RECOVER_SC_BANK
}

void MDrv_ACE_DNR_SetGray_Ground_Gain(MS_U16 u16val)
{
    DECLARA_BANK_VARIABLE
    BACKUP_SC_BANK

    Hal_ACE_DNR_SetGray_Ground_Gain(u16val);

    RECOVER_SC_BANK
}

void MDrv_ACE_DNR_SetGray_Ground_En(MS_BOOL bScalerWin, MS_U16 u16val)
{
    DECLARA_BANK_VARIABLE
    BACKUP_SC_BANK

    Hal_ACE_DNR_SetGray_Ground_En(bScalerWin, u16val);

    RECOVER_SC_BANK
}


void MDrv_ACE_DNR_SetSC_Coring(MS_BOOL bScalerWin, MS_U16 u16val)
{
    DECLARA_BANK_VARIABLE
    BACKUP_SC_BANK

    Hal_ACE_DNR_SetSC_Coring(bScalerWin, u16val);

    RECOVER_SC_BANK
}


void MDrv_ACE_DNR_SetSpikeNR_0(MS_U16 u16val)
{
    DECLARA_BANK_VARIABLE
    BACKUP_SC_BANK

    Hal_ACE_DNR_SetSpikeNR_0(u16val);
}

void MDrv_ACE_DNR_SetSpikeNR_1(MS_U16 u16val)
{
    DECLARA_BANK_VARIABLE
    BACKUP_SC_BANK

    Hal_ACE_DNR_SetSpikeNR_1(u16val);

    RECOVER_SC_BANK
}

void MDrv_ACE_DNR_SetSNR_NM(MS_U16 u16val)
{
    DECLARA_BANK_VARIABLE
    BACKUP_SC_BANK

    Hal_ACE_DNR_SetSNR_NM(u16val);

    RECOVER_SC_BANK
}


void MDrv_ACE_DNR_SetBank_Coring(MS_BOOL bScalerWin, MS_U16 u16val)
{
    DECLARA_BANK_VARIABLE
    BACKUP_SC_BANK

    Hal_ACE_DNR_SetBank_Coring(bScalerWin, u16val);

    RECOVER_SC_BANK
}

void MDrv_ACE_DNR_SetGuassin_SNR_Threshold(MS_BOOL bScalerWin, MS_U16 u16val)
{
    DECLARA_BANK_VARIABLE
    BACKUP_SC_BANK

    Hal_ACE_DNR_SetGuassin_SNR_Threshold(bScalerWin, u16val);

    RECOVER_SC_BANK

}

void MDrv_ACE_DNR_SetNRTable(MS_U8 *pTbl)
{
    MS_U8 i;
    MS_U16 u16val;

    DECLARA_BANK_VARIABLE

    for(i=0; i<8; i=i+2)
    {
        u16val = ((MS_U16)pTbl[i+1]<<8) | (MS_U16) pTbl[i];

        BACKUP_SC_BANK

        Hal_ACE_DNR_SetNRTbl_Y(i, u16val);
        Hal_ACE_DNR_SetNRTbl_C(i, u16val);

        RECOVER_SC_BANK

    }
}

void MDrv_ACE_PCsRGBTable( MS_BOOL bScalerWin, MS_S16 *psPCsRGBTable)
{
    msACESetPCsRGBTable(bScalerWin, psPCsRGBTable);
}

void MDrv_ACE_ColorCorrectionTable( MS_BOOL bScalerWin, MS_S16 *psColorCorrectionTable )
{
    msACESetColorCorrectionTable(bScalerWin, psColorCorrectionTable);
}

void MDrv_ACE_SetColorMatrixControl( MS_BOOL bScalerWin, MS_BOOL bEnable )
{
    msACE_SetColorMatrixControl(bScalerWin, bEnable);
}

void MDrv_ACE_SetRBChannelRange(MS_BOOL bScalerWin, MS_BOOL bRange)
{
    msACE_SetRBChannelRange(bScalerWin, bRange);
}

void MDrv_ACE_WriteColorMatrixBurst( MS_BOOL bWindow, MS_U16* psMatrix )
{
    Hal_ACE_Write_Color_Matrix_Burst(bWindow,psMatrix);
}

MS_BOOL MDrv_ACE_IsSupportMLoad(MS_BOOL bWindow )
{
    return Hal_ACE_Is_Support_MLoad(bWindow);
}

//-------------------------------------------------------------------------------------------------
/// Skip Wait Vsync
/// @param eWindow               \b IN: Enable
/// @param Skip wait Vsync      \b IN: Disable wait Vsync
//-------------------------------------------------------------------------------------------------
void MDrv_ACE_SetSkipWaitVsync( MS_BOOL eWindow,MS_BOOL bIsSkipWaitVsyn)
{
   Hal_ACE_SetSkipWaitVsync( eWindow, bIsSkipWaitVsyn);
}

MS_BOOL MDrv_ACE_GetSkipWaitVsync( MS_BOOL bScalerWin)
{
   return Hal_ACE_GetSkipWaitVsync( bScalerWin);
}


#undef  _MDRV_ACE_C_

