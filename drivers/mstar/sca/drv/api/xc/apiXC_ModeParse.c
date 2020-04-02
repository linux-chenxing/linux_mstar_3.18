////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (¡§MStar Confidential Information¡¨) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    apiXC_ModeParse.c
/// @brief  Scaler API layer Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _API_XC_MODEPARSE_C_
#define _API_XC_MODEPARSE_C_

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
// Common Definition

// Common
#ifdef MSOS_TYPE_LINUX_KERNEL
#include <linux/string.h>
#include <linux/kernel.h>
#else
#include "string.h"
#endif

// XC related include files
#include "MsCommon.h"

// Driver
#include "drvXC_IOPort.h"
#include "apiXC.h"
#include "drvXC_HDMI_if.h"
#include "mvideo_context.h"
#include "drv_sc_ip.h"

#include "apiXC_ModeParse.h"

// Internal Definition

//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------
#define XC_ModeParse_MUTEX

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

#define MODE_DBG(x)         //x
#define EURO_HDTV_DBG(x)    //x

#define MODE_NAME(str)  str,

// search mode torlance
#define MD_HFREQ_TORLANCE           15      // Unit: 0.1kHz
#define MD_VFREQ_TORLANCE           15      // Unit: 0.1Hz
#define MD_HFREQ_TORLANCE_MIN       5
#define MD_VFREQ_TORLANCE_MIN       5
#define MD_FREQ_DELTA               100     // search the user mode



// input resolution
typedef struct
{
    MS_U16 u16DisplayWidth;  // width
    MS_U16 u16DisplayHeight; // height

}MS_MODE_RESOLUTION;

#define _XC_ModeParse_MUTEX_TIME_OUT                 MSOS_WAIT_FOREVER

// Mutex function
#ifdef XC_ModeParse_MUTEX
#define _XC_ModeParse_ENTRY()                                                         \
            if(!MsOS_ObtainMutex(_XC_ModeParse_MUTEX, _XC_ModeParse_MUTEX_TIME_OUT))  \
            {                                                                         \
                MODE_DBG(printf("[apiXC_ModeParse][%06d] Mutex taking timeout\n",__LINE__));    \
            }
#define _XC_ModeParse_RETURN()    MsOS_ReleaseMutex(_XC_ModeParse_MUTEX);
                                //return _ret;
#else // #if not def XC_MUTEX
#define _XC_ModeParse_ENTRY()         while(0)
#define _XC_ModeParse_RETURN()        while(0)
#endif // #ifdef _XC_MUTEX
//-------------------------------------------------------------------------------------------------
//  Local Structurs
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
#ifdef XC_ModeParse_MUTEX
static MS_S32            _XC_ModeParse_MUTEX= -1;
#endif // #ifdef XC_ModeParse_MUTEX

static const MS_MODE_RESOLUTION astStandardModeResolution[RES_MAXIMUM] =
{
    { 640,  350}, // 00: RES_640X350
    { 640,  400}, // 01: RES_640X400
    { 720,  400}, // 02: RES_720X400
    { 640,  480}, // 03: RES_640X480
    { 800,  600}, // 04: RES_800X600
    { 832,  624}, // 05: RES_832X624
    {1024,  768}, // 06: RES_1024X768
    {1280, 1024}, // 07: RES_1280X1024
    {1600, 1200}, // 08: RES_1600X1200
    {1152,  864}, // 09: RES_1152X864
    {1152,  870}, // 10: RES_1152X870
    {1280,  768}, // 11: RES_1280x768
    {1280,  960}, // 12: RES_1280X960
    { 720,  480}, // 13: RES_720X480
    {1920, 1080}, // 14: RES_1920X1080
    {1280,  720}, // 15: RES_1280X720
    { 720,  576}, // 16: RES_720X576
    {1920, 1200}, // 17: RES_1920X1200
    {1400, 1050}, // 18: RES_1400X1050
    {1440,  900}, // 19: RES_1440X900
    {1680, 1050}, // 20: RES_1680X1050
    {1280,  800}, // 21: RES_1280X800
    {1600, 1024}, // 22: RES_1600X1024
    {1600,  900}, // 23: RES_1600X900
    {1360,  768}, // 24: RES_1360X768
    { 848,  480}, // 25: RES_848X480
    {1920, 1080}, // 26: RES_1920X1080P
    {1366,  768}, // 27: RES_1366X768,
    { 864,  648}, // 28: RES_864X648,
    {1280, 1470}, // 29: RES_1280X1470,
    {1920, 2205}, // 30: RES_1920X2205,
    { 720,  240}, // 31: RES_720x240,
    { 720,  288}, // 32: RES_720x288,
    {1152,  900}, // 33: RES_1152X900,
    { 856,  480}, // 34: RES_856x480,
};

//#define MODE_TABLE_MAXIMUM_NUM    (sizeof(astStandardModeTable) / sizeof(MS_PCADC_MODETABLE_TYPE))

//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------


//*************************************************************************
//Function name:    _MApi_XC_ModeParse_GetStdModeStatusFlag
//Passing parameter:    MS_U8 u8ModeIndex: current mode index
//Return parameter: MS_U8: ADC phase
//Description:          get standard ADC phase
//*************************************************************************

static MS_BOOL _MApi_XC_ModeParse_CompareModePolarity(MS_PCADC_MODETABLE_TYPE* ModeDB,MS_U8 u8InputStatusFlag, MS_U8 u8StdModeIndex)
{
    MS_U8 u8ModeFlag;

    u8ModeFlag = ModeDB[u8StdModeIndex].u8StatusFlag;

    if (u8ModeFlag & (MD_FLAG_POR_HPVP | MD_FLAG_POR_HPVN | MD_FLAG_POR_HNVP | MD_FLAG_POR_HNVN)) // check polarity
    {
        if( ( (u8InputStatusFlag&(XC_MD_HSYNC_POR_BIT|XC_MD_VSYNC_POR_BIT) ) == 3) && !(u8ModeFlag&MD_FLAG_POR_HPVP) )
        {
            return FALSE;
        }
        if( ( (u8InputStatusFlag&(XC_MD_HSYNC_POR_BIT|XC_MD_VSYNC_POR_BIT) )==2) && !(u8ModeFlag&MD_FLAG_POR_HPVN) )
        {
            return FALSE;
        }
        if( ( (u8InputStatusFlag&(XC_MD_HSYNC_POR_BIT|XC_MD_VSYNC_POR_BIT) ) == 1) && !(u8ModeFlag&MD_FLAG_POR_HNVP) )
        {
            return FALSE;
        }
        if( ( (u8InputStatusFlag&(XC_MD_HSYNC_POR_BIT|XC_MD_VSYNC_POR_BIT) ) == 0) && !(u8ModeFlag&MD_FLAG_POR_HNVN) )
        {
            return FALSE;
        }

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

//*************************************************************************
//Function name:    _MApi_XC_ModeParse_FindMatchMode
//Passing parameter:    MS_PCADC_MODETABLE_TYPE * pstInputType: current mode parameters
//                  MS_U8 u8InputSourceType:           current input source type
//Return parameter: MS_U8: Match table index
//Description:  This function will search matched standard mode index
//*************************************************************************
static MS_U8 _MApi_XC_ModeParse_FindMatchMode(MS_PCADC_MODETABLE_TYPE* ModeDB, MS_U8 u8NumberOfItems , MS_PCADC_MODETABLE_TYPE *pstInputType, INPUT_SOURCE_TYPE_t enInputSourceType )
{
    MS_PCADC_MODETABLE_TYPE *pStdModeTbl; // table pointer
    MS_U8 ucIndex; // loop index
    MS_U8 ucFlagBff; // flag buffer
    MS_U16 u16InputVTotal;

    MS_U8 ucMatchIndex; // match index
    MS_U16 u16FreqMatchDelta; // difference
    MS_U16 u16FreqDelta;

    MS_U16 u16VTotalMatchDelta;
    MS_U16 u16VTotalDelta;
#if (MS_VGA_SOG_EN)
    MS_U8  u8Bank;
    MS_U16  u16VGASogStatus;
#endif
    MS_U8 u8HFtolerance, u8VFtolerance;

//#if (SUPPORT_EURO_HDTV)
    MS_BOOL bIsEURO_HDTV = FALSE;
#ifdef MSOS_TYPE_CE
    if (MApi_XC_Get_EuroHdtvSupport())
#else
    if (_bEnableEuro)
#endif
    {
        bIsEURO_HDTV = MDrv_XC_ip_GetEuroHdtvStatus(MAIN_WINDOW) == EURO_AUS_HDTV_NORMAL ? FALSE : TRUE;
    }
//#endif

#if 0 //test AP for detect the similar resolution index
{
	MS_U8 i,j;
	MS_PCADC_MODETABLE_TYPE *modeTab1;
	MS_PCADC_MODETABLE_TYPE *modeTab2;

	u8HFtolerance = MD_HFREQ_TORLANCE;
	u8VFtolerance = MD_VFREQ_TORLANCE;

	for(i=0; i<u8NumberOfItems-1; i++)
	{
		modeTab1 = ModeDB + i;

		if(modeTab1->u8StatusFlag & MD_FLAG_YPBPR_BIT)
		{
			continue;
		}

		for(j=i+1; j<u8NumberOfItems; j++)
		{
			modeTab2 = ModeDB+j;

			if ( (abs(modeTab1->u16HorizontalFrequency - modeTab2->u16HorizontalFrequency) < u8HFtolerance) &&
					(abs(modeTab1->u16VerticalFrequency - modeTab2->u16VerticalFrequency) < u8VFtolerance))
			{
				printf("mode(%d,%d) Res idx %d@%d is the same with %d@%d\n", i, j,
					modeTab1->enResolutionIndex, modeTab1->u16VerticalFrequency,
					modeTab2->enResolutionIndex, modeTab2->u16VerticalFrequency);
			}
		}

	}
}
#endif

    // initilaize
    ucMatchIndex = u8NumberOfItems;
    u16FreqMatchDelta = MD_HFREQ_TORLANCE + MD_VFREQ_TORLANCE;
    u16VTotalMatchDelta = 0xFFFF;

    u16InputVTotal = pstInputType->u16VerticalTotal;

    pStdModeTbl = ModeDB; // initailize

    for(ucIndex=0; ucIndex<u8NumberOfItems; ucIndex++, pStdModeTbl++)
    {
        ucFlagBff = pStdModeTbl->u8StatusFlag; // get sync flag

        MODE_DBG(printf("1:Md=%u,StatusFlag=0x%x,Tbl=0x%x\n",
                        ucIndex, pstInputType->u8StatusFlag, ucFlagBff);)

        if( IsSrcTypeYPbPr(enInputSourceType) || IsSrcTypeHDMI(enInputSourceType)   || IsSrcTypeBT656(enInputSourceType)) // Mode detect for YPbPr
        {
            if( !(ucFlagBff & MD_FLAG_YPBPR_BIT) ) // check YPbPr mode only
                continue;

//#if (SUPPORT_EURO_HDTV)
        #ifdef MSOS_TYPE_CE
            if (MApi_XC_Get_EuroHdtvSupport())
        #else
            if (_bEnableEuro)
        #endif
            {
                if(bIsEURO_HDTV)
                {
                    if( !(ucFlagBff & MD_FLAG_EURO_HDTV_BIT) )
                    {
                        continue;
                    }
                }
                else
                {
                    if( (ucFlagBff & MD_FLAG_EURO_HDTV_BIT) )
                    {
                        continue;
                    }
                }
            }
//#endif

        }
        else // Mode detect for PC
        {
            if( (ucFlagBff&MD_FLAG_YPBPR_BIT) /*&& (ucMatchIndex != _END_OF_TBL_)*/ )
                continue;
        }

#if (MS_VGA_SOG_EN)
        //u8Bank = MDrv_ReadByte(BK_SELECT_00);
        //MDrv_WriteByte(BK_SELECT_00, REG_BANK_IP1F2);

        //u8VGASogStatus = MDrv_ReadByte(L_BK_IP1F2(0x02));
        u16VGASogStatus = SC_R2BYTE(REG_SC_BK01_02_L);
        //MDrv_WriteByte(BK_SELECT_00, u8Bank);

        if((u16VGASogStatus & 0x70)==0x70)
        {
            ;
        }
        else
#endif // #if (MS_VGA_SOG_EN)

        if(_MApi_XC_ModeParse_CompareModePolarity(ModeDB,pstInputType->u8StatusFlag, ucIndex)) // check polarity
        {
            //printf("MDrv_Mode_CompareModePolarity TRUE  ucIndex %u\n", (MS_U8)ucIndex);
            ;
        }
        else
        {
            if(!IsSrcTypeYPbPr(enInputSourceType))
            {
                //printf("MDrv_Mode_CompareModePolarity fail  ucIndex %u\n", (MS_U8)ucIndex);
                continue;
            }
        }

        // match interlace mode
        if ((ucFlagBff & MD_FLAG_INTERLACE) != (pstInputType->u8StatusFlag & XC_MD_INTERLACE_BIT))
            continue;

        //if (wVTotal < tStandardModeResolution[pStdModeTbl->ResolutionIndex].DisplayHeight)
        //    continue;

        u8HFtolerance = MD_HFREQ_TORLANCE;
        u8VFtolerance = MD_VFREQ_TORLANCE;
        if (pStdModeTbl->enResolutionIndex == RES_1920X1080P)
        {
            if ((abs(pStdModeTbl->u16VerticalFrequency - 250) < MD_VFREQ_TORLANCE_MIN)||
                (abs(pStdModeTbl->u16VerticalFrequency - 240) < MD_VFREQ_TORLANCE_MIN))
            {
                u8HFtolerance = MD_HFREQ_TORLANCE_MIN;
                u8VFtolerance = MD_VFREQ_TORLANCE_MIN;
            }
        }

        MODE_DBG(printf("(HFreq, VFreq, Vtt) in (%u, %u, %u), mode (%u, %u, %u)\n",
            pstInputType->u16HorizontalFrequency, pstInputType->u16VerticalFrequency, u16InputVTotal,
            pStdModeTbl->u16HorizontalFrequency, pStdModeTbl->u16VerticalFrequency, pStdModeTbl->u16VerticalTotal);)
        MODE_DBG(printf("Torlance (%u, %u, %u)\n", (MS_U8) u8HFtolerance, (MS_U8) u8VFtolerance, pStdModeTbl->u8VTotalTolerance);)

        // match H/V frequency
        if ( (abs(pstInputType->u16HorizontalFrequency - pStdModeTbl->u16HorizontalFrequency) < u8HFtolerance) &&
                (abs(pstInputType->u16VerticalFrequency - pStdModeTbl->u16VerticalFrequency) < u8VFtolerance) &&
                (abs(u16InputVTotal - pStdModeTbl->u16VerticalTotal) < pStdModeTbl->u8VTotalTolerance) )
        {
            // calculate frequency diffenece
            u16FreqDelta = abs(pstInputType->u16HorizontalFrequency - pStdModeTbl->u16HorizontalFrequency) +
                        abs(pstInputType->u16VerticalFrequency - pStdModeTbl->u16VerticalFrequency);

            u16VTotalDelta = abs(u16InputVTotal - pStdModeTbl->u16VerticalTotal);

            MODE_DBG(printf("New FreqDelta, VTotalDelta (%u, %u), Old (%u, %u)\n",
                u16FreqDelta, u16VTotalDelta, u16FreqMatchDelta, u16VTotalMatchDelta);)
            // check more near
            if ( ((u16FreqMatchDelta > u16FreqDelta) && (u16VTotalMatchDelta > u16VTotalDelta))  ||
                 ((u16FreqDelta < 10) && (u16VTotalDelta < u16VTotalMatchDelta))                 ||
                 ((u16FreqMatchDelta == u16FreqDelta) && (u16VTotalMatchDelta > u16VTotalDelta)) ||
                 ((u16FreqMatchDelta > u16FreqDelta) && (u16VTotalMatchDelta == u16VTotalDelta))
               )
            {
                MODE_DBG(printf("MatchIdx=%u\n", ucIndex);)
                u16FreqMatchDelta = u16FreqDelta;
                u16VTotalMatchDelta = u16VTotalDelta;
                ucMatchIndex = ucIndex; // match standard mode table
            }
        }
    } // for

    if(ucMatchIndex != u8NumberOfItems)
    {
        MODE_DBG(printf("1:Idx=%u,W=%u,H=%u,interlaced=%u\n",
                        ucMatchIndex,
                        astStandardModeResolution[ModeDB[ucMatchIndex].enResolutionIndex].u16DisplayWidth,
                        astStandardModeResolution[ModeDB[ucMatchIndex].enResolutionIndex].u16DisplayHeight,
                        pstInputType->u8StatusFlag & XC_MD_INTERLACE_BIT));
        return ucMatchIndex;
    }
    else
    {
        MODE_DBG(printf("search user mode\n"));

        // initilaize
        ucMatchIndex = u8NumberOfItems;
        u16FreqMatchDelta = MD_FREQ_DELTA;
        u16VTotalMatchDelta = 0xFFFF;

        pStdModeTbl = ModeDB; // initailize

        for(ucIndex=0; ucIndex<u8NumberOfItems; ucIndex++, pStdModeTbl++)
        {
            ucFlagBff = pStdModeTbl->u8StatusFlag; // get sync flag

            MODE_DBG(printf("2:Md=%u,StatusFlag=0x%x,Tbl=0x%x\n",
                        ucIndex, pstInputType->u8StatusFlag, ucFlagBff);)

            // check YPbPr mode detect(only suppot YPbPr mode)
            if( IsSrcTypeYPbPr(enInputSourceType) || IsSrcTypeHDMI(enInputSourceType)  || IsSrcTypeBT656(enInputSourceType)) // Mode detect for YPbPr
            {
                if( !(ucFlagBff & MD_FLAG_YPBPR_BIT) ) // check YPbPr mode only
                    continue;
    //#if (SUPPORT_EURO_HDTV)
            #ifdef MSOS_TYPE_CE
                if (MApi_XC_Get_EuroHdtvSupport())
            #else
                if (_bEnableEuro)
            #endif
                {
                    if(bIsEURO_HDTV)
                    {
                        if( !(ucFlagBff & MD_FLAG_EURO_HDTV_BIT) )
                        {
                            continue;
                        }
                    }
                    else
                    {
                        if( (ucFlagBff & MD_FLAG_EURO_HDTV_BIT) )
                        {
                            continue;
                        }
                    }
                }
    //#endif
            }
            else // Mode detect for PC
            {
            //  if( (ucFlagBff&MD_FLAG_YPBPR_BIT) /*&& (ucMatchIndex != _END_OF_TBL_)*/ )
            //      continue;
            }

            if ((ucFlagBff & MD_FLAG_INTERLACE) != (pstInputType->u8StatusFlag & XC_MD_INTERLACE_BIT))
                continue;

            MODE_DBG(printf("        InputVtt=%u,Mode=%u\n", u16InputVTotal, astStandardModeResolution[pStdModeTbl->enResolutionIndex].u16DisplayHeight);)

            if ( u16InputVTotal > astStandardModeResolution[pStdModeTbl->enResolutionIndex].u16DisplayHeight)
            {
                MODE_DBG(printf("        HFreq(%u,%u),VFreq(%u,%u)\n",
                    pstInputType->u16HorizontalFrequency,
                    pStdModeTbl->u16HorizontalFrequency,
                    pstInputType->u16VerticalFrequency,
                    pStdModeTbl->u16VerticalFrequency);)

                // calculate frequency delta
                u16FreqDelta = abs(pstInputType->u16HorizontalFrequency - pStdModeTbl->u16HorizontalFrequency) +
                            abs(pstInputType->u16VerticalFrequency - pStdModeTbl->u16VerticalFrequency);

                u16VTotalDelta = abs(u16InputVTotal - pStdModeTbl->u16VerticalTotal);

                // compare delta minimum & match interlace status
                if ( ((u16FreqDelta < u16FreqMatchDelta) && (u16VTotalDelta < u16VTotalMatchDelta)) ||
                    ((u16FreqDelta <= 10) && (u16VTotalDelta < u16VTotalMatchDelta)) )
                {
                    u16FreqMatchDelta = u16FreqDelta;
                    ucMatchIndex = ucIndex; // match user mode index
                    u16VTotalMatchDelta = u16VTotalDelta;

                    MODE_DBG(printf("        --Delta Freq=%u,Vtt=%u\n", u16FreqMatchDelta, u16VTotalMatchDelta);)
                }
            }
        } // for

        if(ucMatchIndex != u8NumberOfItems)
        {
            //pstInputType->u8StatusFlag |= _BIT7; //user new mode
            pstInputType->u8StatusFlag |= XC_MD_USER_MODE_BIT; //user new mode
//          printf("This mode is USER MODE \n");
        }

        MODE_DBG(printf("2:Idx=%u,W=%u,H=%u,interlaced=%u\n",
                        ucMatchIndex,
                        astStandardModeResolution[ModeDB[ucMatchIndex].enResolutionIndex].u16DisplayWidth,
                        astStandardModeResolution[ModeDB[ucMatchIndex].enResolutionIndex].u16DisplayHeight,
                        pstInputType->u8StatusFlag & XC_MD_INTERLACE_BIT);)
        return ucMatchIndex; // result
    }
}

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------


/********************************************************************************/
/*                   Local Function Prototypes                  */
/********************************************************************************/
/********************************************************************************/
/*                   Functions                      */
/********************************************************************************/

MS_BOOL MApi_XC_ModeParse_Init(void)
{
    if (-1!=_XC_ModeParse_MUTEX)
    {
        MODE_DBG(printf("[_XC_ModeParse_MUTEX][%06d] Initialization more than once\n", __LINE__));
        //return FALSE;
    }

    _XC_ModeParse_MUTEX = MsOS_CreateMutex(E_MSOS_FIFO, "_XC_ModeParse_MUTEX", MSOS_PROCESS_SHARED);

    if (-1 == _XC_ModeParse_MUTEX)
    {
        MODE_DBG(printf("[_XC_ModeParse_MUTEX][%06d] create mutex fail\n", __LINE__));
    }
    return TRUE;
}
XC_MODEPARSE_RESULT MApi_XC_ModeParse_MatchMode(MS_PCADC_MODETABLE_TYPE* ModeDB, MS_U8 u8NumberOfItems , XC_MODEPARSE_INPUT_INFO *psInputInfo)
{
    MS_PCADC_MODETABLE_TYPE stTempInputBuff;

    _XC_ModeParse_ENTRY();

    if(IsSrcTypeAnalog(psInputInfo->eSrcType) || IsSrcTypeBT656(psInputInfo->eSrcType))
    {
        // VGA/YPbPr
        // search standard mode, fill related parameter
        stTempInputBuff.u16HorizontalFrequency = psInputInfo->u16HFreqx10;
        stTempInputBuff.u16VerticalFrequency = psInputInfo->u16VFreqx10;
        stTempInputBuff.u16VerticalTotal = psInputInfo->u16Vtotal;
        stTempInputBuff.u8StatusFlag = psInputInfo->u8SyncStatus;

        psInputInfo->u8ModeIndex = _MApi_XC_ModeParse_FindMatchMode(ModeDB, u8NumberOfItems, &stTempInputBuff, psInputInfo->eSrcType);
		MODE_DBG(printf("1.psInputInfo->eSrcType=%u, psInputInfo->u8ModeIndex=%u\n",psInputInfo->eSrcType, psInputInfo->u8ModeIndex));
        if ( psInputInfo->u8ModeIndex == u8NumberOfItems ) // not match
        {
            MODE_DBG(printf("XC_MODEPARSE_UNSUPPORT_MODE\n"));
            _XC_ModeParse_RETURN();
            return XC_MODEPARSE_UNSUPPORT_MODE;
        }
        else
        {
            psInputInfo->u8SyncStatus |= (stTempInputBuff.u8StatusFlag & XC_MD_USER_MODE_BIT);
            MODE_DBG(printf("LMd=%u,N=%u\n", psInputInfo->u8ModeIndex, (MS_BOOL)((stTempInputBuff.u8StatusFlag & XC_MD_USER_MODE_BIT)>0)));
            psInputInfo->u16ResWidth  = astStandardModeResolution[ModeDB[psInputInfo->u8ModeIndex].enResolutionIndex].u16DisplayWidth;
            //_MApi_XC_ModeParse_GetStdMdResH(psInputInfo->u8ModeIndex);
            psInputInfo->u16ResHeight = astStandardModeResolution[ModeDB[psInputInfo->u8ModeIndex].enResolutionIndex].u16DisplayHeight;
            //_MApi_XC_ModeParse_GetStdMdResV(psInputInfo->u8ModeIndex);
            MODE_DBG(printf("XC_MODEPARSE_SUPPORT_MODE\n"));
            _XC_ModeParse_RETURN();
            return XC_MODEPARSE_SUPPORT_MODE;
        }
    }
    else
    {
        // DVI/HDMI
        psInputInfo->u8ModeIndex = 0;
        _XC_ModeParse_RETURN();
        return XC_MODEPARSE_SUPPORT_MODE;
    }
	MODE_DBG(printf("2.psInputInfo->eSrcType=%u, psInputInfo->u8ModeIndex=%u\n",psInputInfo->eSrcType, psInputInfo->u8ModeIndex));
}

#undef _API_XC_MODEPARSE_C_
#endif  // _API_XC_MODEPARSE_C_
