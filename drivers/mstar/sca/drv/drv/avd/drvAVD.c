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
/// file    drvAVD.c
/// @brief  AVD Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
// Common Definition
#if defined(MSOS_TYPE_LINUX_KERNEL)
#include <linux/string.h>
#include <linux/kernel.h>
#else
#include <string.h>
#endif

#include "MsCommon.h"
#include "MsVersion.h"
#include "drvAVD.h"
#include "MsIRQ.h"
#include "drvXC_IOPort.h"
#include "apiXC.h"
//#if !defined (MSOS_TYPE_NOS)
    #include "MsOS.h"
//#endif

// Internal Definition
#include "regCHIP.h"
#include "regAVD.h"
#include "halAVD.h"
//#include "drvBDMA.h"
//#include "drvSERFLASH.h"


//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------
#define TEST_VD_DSP                     0               // 1: for TEST VD DSP, 0: for MSTAR VD DSP
#define ADJUST_CHANNEL_CHANGE_HSYNC 1
//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

//avd mutex wait time
#define AVD_MUTEX_WAIT_TIME    1000

#if (TEST_VD_DSP)
#undef  VD_STATUS_RDY
#define VD_STATUS_RDY                   (BIT(8))
#define VD_STANDARD_VALID               BIT(0)
#endif

//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------
//#define MS_DEBUG
#define _AVD_MUTEX_TIME_OUT      5000
#ifdef MS_DEBUG
#define VDDBG(x)          (x)
#define AVD_DRV_DEBUG 1
#if defined( MSOS_TYPE_CE) || defined(MSOS_TYPE_LINUX_KERNEL)		// not implement MsOS_In_Interrupt on wince currently
#define AVD_LOCK()        \
    do{                         \
        if (_u8AVDDbgLevel == AVD_DBGLV_DEBUG) printf("%s lock\n", __FUNCTION__);\
        MsOS_ObtainMutex(_s32AVD_Mutex, _AVD_MUTEX_TIME_OUT);\
        }while(0)
#else
#define AVD_LOCK()      \
    do{                         \
        MS_ASSERT(MsOS_In_Interrupt() == FALSE); \
        if (_u8AVDDbgLevel == AVD_DBGLV_DEBUG) printf("%s lock\n", __FUNCTION__);\
        MsOS_ObtainMutex(_s32AVD_Mutex, _AVD_MUTEX_TIME_OUT);\
        }while(0)
#endif
#define AVD_UNLOCK()      \
    do{                         \
        MsOS_ReleaseMutex(_s32AVD_Mutex);\
        if (_u8AVDDbgLevel == AVD_DBGLV_DEBUG) printf("%s unlock\n", __FUNCTION__); \
        }while(0)
#else
#define VDDBG(x)          //(x)
#define AVD_DRV_DEBUG 0
#if defined( MSOS_TYPE_CE) || defined(MSOS_TYPE_LINUX_KERNEL)
#define AVD_LOCK()      \
    do{                         \
        MsOS_ObtainMutex(_s32AVD_Mutex, _AVD_MUTEX_TIME_OUT);\
        }while(0)
#else
#define AVD_LOCK()      \
    do{                         \
        MS_ASSERT(MsOS_In_Interrupt() == FALSE); \
        MsOS_ObtainMutex(_s32AVD_Mutex, _AVD_MUTEX_TIME_OUT);\
        }while(0)
#endif
#define AVD_UNLOCK()      \
    do{                         \
        MsOS_ReleaseMutex(_s32AVD_Mutex);\
        }while(0)
#endif

//-------------------------------------------------------------------------------------------------
//  Local Structurs
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
static MSIF_Version _drv_avd_version = {
#if defined( MSOS_TYPE_CE) || defined(MSOS_TYPE_LINUX_KERNEL)
	{ AVD_VER, },
#else
    .MW = { AVD_VER, },
#endif
};

static VD_INITDATA g_VD_InitData;
static MS_U16 _u16DataH[3]={0,0,0};
static MS_U8  _u8HtotalDebounce=0;
static MS_U8 _u8AutoTuningIsProgress = FALSE;
static MS_U32 _u32VideoSystemTimer=0;
static MS_U8 _u8Comb10Bit3Flag=1;
static MS_U16 _u16LatchH=1135L;
static AVD_VideoStandardType _eVideoSystem=E_VIDEOSTANDARD_PAL_BGHI;
static MS_U8  _u8AutoDetMode=FSC_AUTO_DET_ENABLE;
static AVD_VideoStandardType _eForceVideoStandard=E_VIDEOSTANDARD_AUTO;
static MS_U16 _u16CurVDStatus=0;
static AVD_VideoStandardType _eLastStandard = E_VIDEOSTANDARD_NOTSTANDARD;
// BY 20090402 static MS_BOOL _bVD_FWStatus = FALSE;    /* TRUE: Nand Flash; FALSE: SPI Flash; */
// BY 20090402 static MS_U32 _u32SysBinAddr=0;
static AVD_InputSourceType _eVDInputSource=E_INPUT_SOURCE_ATV;
static MS_S32 _s32AVD_Mutex;
#if AVD_DRV_DEBUG
static AVD_DbgLv _u8AVDDbgLevel=AVD_DBGLV_NONE;
static MS_U32 u32PreVDPatchFlagStatus=0;
#endif
static MS_U32 u32VDPatchFlagStatus=0;
static MS_U8 _u8Comb57=0x04,_u8Comb58=0x01,_u8Comb5F=0x08;
#ifndef MSOS_TYPE_LINUX
static MS_U16 u16PreVtotal=0;
#endif
static MS_U8 _u8AfecD4Factory=0;
static AVD_Still_Image_Param g_stillImageParam;
static MS_BOOL b2d3dautoflag=1;  //20110602 Brian new flag for api to control driver auto 2D/3D switch

MS_U16 _u16Htt_UserMD = 1135;
MS_U16 _u16HtotalNTSC[]={(1135L*3/2), (1135L), (910L), (1135L)};
MS_U16 _u16HtotalPAL[]={(1135L*3/2), (1135L), (1135L), (1135L)};
MS_U16 _u16HtotalSECAM[]={(1135L*3/2), (1135L), (1097L), (1135L)};
MS_U16 _u16HtotalNTSC_443[]={(1135L*3/2), (1135L), (1127L), (1135L)};
MS_U16 _u16HtotalPAL_M[]={(1135L*3/2), (1135L), (909L), (1135L)};
MS_U16 _u16HtotalPAL_NC[]={(1135L*3/2), (1135L), (917L), (1135L)};
MS_U16 _u16HtotalPAL_60[]={(1135L*3/2), (1135L), (1127L), (1135L)};

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
#if 0
extern void HAL_PWS_Stop_VDMCU(void);
#endif

static void _MDrv_AVD_VDMCU_Init(void)
{
    #if (VD_CHIP_VERSION>=AVD_TITANIA3_A)
    MS_U8 u8Temp;
    #endif

    #if AVD_DRV_DEBUG
    if (_u8AVDDbgLevel == AVD_DBGLV_DEBUG)
    {
        printf("_MDrv_AVD_VDMCU_Init\n");
    }
    #endif

    #if (VD_CHIP_VERSION==AVD_TITANIA3_A)
    HAL_AVD_AFEC_SetClockSource(TRUE);
    #endif

#if 0
    if(!(g_VD_InitData.u32VDPatchFlag & AVD_PATCH_DISABLE_PWS))
    {
        HAL_PWS_Stop_VDMCU();
    }
#endif

    //--------------------------------------------------------
    // Load code
    //--------------------------------------------------------
    #if (VD_CHIP_VERSION==AVD_TITANIA4_A)
    if (1)
    #else
    if (g_VD_InitData.eLoadCodeType == AVD_LOAD_CODE_BYTE_WRITE) //BY 20090402 (_bVD_FWStatus)
    #endif
    {
        // TODO add different loading function for T3
        #if AVD_DRV_DEBUG
        if (_u8AVDDbgLevel == AVD_DBGLV_DEBUG)
        {
            printf("[AVD_LOAD_CODE_BYTE_WRITE]F/W ADDR = 0x%X\n", (unsigned int)(g_VD_InitData.pu8VD_DSP_Code_Address));
        }
        #endif
        HAL_AVD_VDMCU_LoadDSP(g_VD_InitData.pu8VD_DSP_Code_Address, g_VD_InitData.u32VD_DSP_Code_Len);
        #if (VD_CHIP_VERSION==AVD_TITANIA3_A)
        HAL_AVD_VDMCU_SetFreeze(ENABLE);
        HAL_AVD_AFEC_SetClockSource(FALSE);
        HAL_AVD_VDMCU_SetFreeze(DISABLE);
        #endif
    }
    else if (g_VD_InitData.eLoadCodeType == AVD_LOAD_CODE_BDMA_FROM_SPI)
    {
        // TODO implement DMA function
        // copy code from flash to SDRAM
        // BY 20090402 MDrv_DMA_XCopy(MCPY_LOADVDMCUFW, BinInfo.B_FAddr, 0x00000l, BinInfo.B_Len); // TODO add DMA load function
        //MDrv_BDMA_CopyHnd(g_VD_InitData.pu8VD_DSP_Code_Address, 0 , g_VD_InitData.u32VD_DSP_Code_Len, BDMA_CPYTYPE_COL, BDMA_OPCFG_DEF);
        #if AVD_DRV_DEBUG
        if (_u8AVDDbgLevel == AVD_DBGLV_DEBUG)
        {
            printf("[AVD_LOAD_CODE_BDMA_FROM_SPI]F/W ADDR = 0x%X 0x%X\n", (unsigned int)g_VD_InitData.u32VD_DSP_Code_Address, (unsigned int)g_VD_InitData.u32VD_DSP_Code_Len);
        }
        #endif
        //MDrv_BDMA_SetDbgLevel(E_BDMA_DBGLV_ALL);
        HAL_AVD_VDMCU_SetFreeze(ENABLE);
        #if (VD_CHIP_VERSION>=AVD_TITANIA3_A)
        u8Temp = MDrv_AVD_GetRegValue(VD_MCU_SRAM_EN);
        MDrv_AVD_SetRegValue(VD_MCU_SRAM_EN,u8Temp&(~ BIT(0)));
        #endif
		#if !defined( MSOS_TYPE_CE) && !defined(MSOS_TYPE_LINUX_KERNEL)	// not implement MDrv_SERFLASH_CopyHnd on wince currently
        MDrv_SERFLASH_CopyHnd(g_VD_InitData.u32VD_DSP_Code_Address, 0 , g_VD_InitData.u32VD_DSP_Code_Len, E_SPIDMA_DEV_VDMCU, SPIDMA_OPCFG_DEF);
		#endif
        #if (VD_CHIP_VERSION>=AVD_TITANIA3_A)
        MDrv_AVD_SetRegValue(VD_MCU_SRAM_EN,u8Temp|BIT(0));
        #endif
        HAL_AVD_VDMCU_SetFreeze(DISABLE);
        #if (VD_CHIP_VERSION==AVD_TITANIA3_A)
        HAL_AVD_VDMCU_SetFreeze(ENABLE);
        HAL_AVD_AFEC_SetClockSource(FALSE);
        HAL_AVD_VDMCU_SetFreeze(DISABLE);
        #endif
    }
    #if (VD_CHIP_VERSION==AVD_TITANIA3_A)
    else if (g_VD_InitData.eLoadCodeType == AVD_LOAD_CODE_BDMA_FROM_DRAM) // TODO add MIU0/1 check
    {
        // TODO implement DMA function
        // copy code from flash to SDRAM
        // BY 20090402 MDrv_DMA_XCopy(MCPY_LOADVDMCUFW, BinInfo.B_FAddr, 0x00000l, BinInfo.B_Len); // TODO add DMA load function
        //MDrv_BDMA_CopyHnd(g_VD_InitData.pu8VD_DSP_Code_Address, 0 , g_VD_InitData.u32VD_DSP_Code_Len, BDMA_CPYTYPE_COL, BDMA_OPCFG_DEF);
        #if AVD_DRV_DEBUG
        if (_u8AVDDbgLevel == AVD_DBGLV_DEBUG)
        {
            printf("[AVD_LOAD_CODE_BDMA_FROM_DRAM]F/W ADDR = 0x%X\n", (unsigned int)(g_VD_InitData.pu8VD_DSP_Code_Address));
        }
        #endif
        HAL_AVD_VDMCU_SetFreeze(ENABLE);
        MDrv_BDMA_CopyHnd(g_VD_InitData.u32VD_DSP_Code_Address, 0 , g_VD_InitData.u32VD_DSP_Code_Len, E_BDMA_SDRAM2VDMCU, BDMA_OPCFG_DEF);
        HAL_AVD_VDMCU_SetFreeze(DISABLE);
        #if (VD_CHIP_VERSION==AVD_TITANIA3_A)
        HAL_AVD_VDMCU_SetFreeze(ENABLE);
        HAL_AVD_AFEC_SetClockSource(FALSE);
        HAL_AVD_VDMCU_SetFreeze(DISABLE);
        #endif
    }
    #endif
    else
    {
        #if AVD_DRV_DEBUG
        if (_u8AVDDbgLevel >= AVD_DBGLV_ERR)
        {
            printf("_MDrv_AVD_VDMCU_Init eLoadCodeType Invalid\r\n");
        }
        #endif
    }

    HAL_AVD_VDMCU_SetClock(AVD_VDMCU_CLOCK_108Mhz, AVD_VDMCU_CLOCK_NORMAL);

    HAL_AVD_AFEC_SetPatchFlag(g_VD_InitData.u32VDPatchFlag);

    HAL_AVD_RegInitExt(g_VD_InitData.u8VdDecInitializeExt); // TODO use system variable type

    _u8AfecD4Factory=HAL_AVD_GetReg(BK_AFEC_D4);
}

static void _MDrv_AVD_3DCombSpeedup(void)
{
    HAL_AVD_COMB_Set3dCombSpeed(_u8Comb57, _u8Comb58, _u8Comb5F); //need to review, sonic 20091218
    #if ADJUST_CHANNEL_CHANGE_HSYNC
    HAL_AVD_AFEC_SetSwingLimit(0);
    HAL_AVD_AFEC_EnableBottomAverage(DISABLE); // more sensitivity
    #endif
    if (!(_u8AfecD4Factory & BIT(4)))
    {
        HAL_AVD_AFEC_EnableVBIDPLSpeedup(ENABLE);
    }
    HAL_AVD_SetReg(BK_AFEC_A1, 0x6A);
    HAL_AVD_SetReg(BK_AFEC_A0, 0xBC);
    _u8Comb10Bit3Flag=BIT(2)|BIT(1)|BIT(0);
    _u32VideoSystemTimer=MsOS_GetSystemTime();
    #if AVD_DRV_DEBUG
    if (_u8AVDDbgLevel == AVD_DBGLV_DEBUG)
    {
        printf("_MDrv_AVD_3DCombSpeedup Enable %d\n",(int)_u32VideoSystemTimer);
    }
    #endif
}

static void _MDrv_AVD_SyncRangeHandler(void)
{
#ifndef MSOS_TYPE_LINUX
    XC_ApiStatus sXC_Status;
    XC_SETWIN_INFO sXC_SetWinInfo;
    MS_WINDOW_TYPE SrcWin;
    static MS_BOOL bXC_BlackScreen_Enabled = TRUE;
    static MS_U16   u16XC_VCap;

    MS_U16 wVtotal = HAL_AVD_AFEC_GetVTotal();
    MS_U16 wVsize_bk, g_cVSizeShift;
    static MS_U8 u8SyncStableCounter = 0;

    if( (!MDrv_AVD_IsSyncLocked()) ||
        MApi_XC_IsFreezeImg(MAIN_WINDOW) ||
        (u16PreVtotal == 0))
    {
        bXC_BlackScreen_Enabled = TRUE;
        return;
    }

/*
    if ((wHPeriod > (u16PreHPeriod + 3)) || (wHPeriod < (u16PreHPeriod - 3)))
	{
		printf("P3 \r\n");
		return;
    	}
*/

    // Get XC capture height from the begining
    if(MApi_XC_IsBlackVideoEnable(MAIN_WINDOW))
    {
        // XC still blocked the video
        bXC_BlackScreen_Enabled = TRUE;
        return;
    }
    else
    {
        if(bXC_BlackScreen_Enabled)
        {
            // update new capture height
            bXC_BlackScreen_Enabled = FALSE;
            MApi_XC_GetCaptureWindow( &SrcWin, MAIN_WINDOW );
            u16XC_VCap = SrcWin.height;

#if 0
            if ((wVtotal < 675 ) && (wVtotal > 570))
                u16PreVtotal = 625;
            else
                u16PreVtotal = 525;
#endif
        }
    }

    //if (((wVtotal < 620) && (wVtotal > 570)) || ((wVtotal > 630) && (wVtotal < 670)))
    if ((wVtotal < 620) && (wVtotal > 570))
    {
        if (u16PreVtotal != wVtotal)
        {
            u16PreVtotal  = wVtotal;
            u8SyncStableCounter = 0;
        }
        else if (u8SyncStableCounter < 5)
            u8SyncStableCounter ++;
    }
    //else if (((wVtotal < 520) && (wVtotal > 470)) || ((wVtotal > 530) && (wVtotal < 570)))
    else if ((wVtotal < 520) && (wVtotal > 470))
    {

        if (u16PreVtotal != wVtotal)
        {
            u16PreVtotal  = wVtotal;
            u8SyncStableCounter = 0;
        }
        else if (u8SyncStableCounter < 5)
            u8SyncStableCounter ++;
    }
    else if(((wVtotal <= 630) && (wVtotal >= 620)) || ((wVtotal <= 530) && (wVtotal >= 520)))
    {
        if((u16PreVtotal > (wVtotal + 5)) || (u16PreVtotal < (wVtotal - 5)))
        {
            u16PreVtotal  = wVtotal;
            u8SyncStableCounter = 0;
        }
        else if (u8SyncStableCounter < 5)
            u8SyncStableCounter ++;
    }

    if(u8SyncStableCounter == 5)
    {
        u8SyncStableCounter = 6;
        wVsize_bk = u16XC_VCap;
        if (((wVtotal < 620) && (wVtotal > 570)) || ((wVtotal > 630) && (wVtotal < 670)))
        {
            if (wVtotal > 625)
            {
                g_cVSizeShift = wVtotal - 625;
               u16XC_VCap = u16XC_VCap + g_cVSizeShift;
            }
            else
            {
                g_cVSizeShift = 625 - wVtotal;
               u16XC_VCap = u16XC_VCap - g_cVSizeShift;
            }
        }
        else if (((wVtotal < 520) && (wVtotal > 470)) || ((wVtotal > 530) && (wVtotal < 570)))
        {
            if (wVtotal > 525)
            {
                g_cVSizeShift = wVtotal - 525;
                u16XC_VCap = u16XC_VCap + g_cVSizeShift;
            }
            else
            {
                g_cVSizeShift = 525 - wVtotal;
                u16XC_VCap = u16XC_VCap - g_cVSizeShift;
            }
        }

        memset(&sXC_Status, 0, sizeof(XC_ApiStatus));
        memset(&sXC_SetWinInfo, 0, sizeof(XC_SETWIN_INFO));
        MApi_XC_GetStatus(&sXC_Status, MAIN_WINDOW);
        memcpy(&sXC_SetWinInfo, &sXC_Status, sizeof(XC_SETWIN_INFO));

        // reset changed part
        sXC_SetWinInfo.bPreHCusScaling = FALSE;
        sXC_SetWinInfo.bPreVCusScaling = FALSE;
        sXC_SetWinInfo.stCapWin.height = u16XC_VCap;

        MApi_XC_SetWindow(&sXC_SetWinInfo, sizeof(XC_SETWIN_INFO), MAIN_WINDOW);

        u16XC_VCap = wVsize_bk;
   }
#endif
}

void MDrv_AVD_SetStillImageParam(AVD_Still_Image_Param param)
{
    g_stillImageParam.bMessageOn   = param.bMessageOn;

    g_stillImageParam.u8Str1_COMB37 = param.u8Str1_COMB37;
    g_stillImageParam.u8Str1_COMB38 = param.u8Str1_COMB38;
    g_stillImageParam.u8Str1_COMB7C = param.u8Str1_COMB7C;
    g_stillImageParam.u8Str1_COMBED = param.u8Str1_COMBED;

    g_stillImageParam.u8Str2_COMB37 = param.u8Str2_COMB37;
    g_stillImageParam.u8Str2_COMB38 = param.u8Str2_COMB38;
    g_stillImageParam.u8Str2_COMB7C = param.u8Str2_COMB7C;
    g_stillImageParam.u8Str2_COMBED = param.u8Str2_COMBED;

    g_stillImageParam.u8Str3_COMB37 = param.u8Str3_COMB37;
    g_stillImageParam.u8Str3_COMB38 = param.u8Str3_COMB38;
    g_stillImageParam.u8Str3_COMB7C = param.u8Str3_COMB7C;
    g_stillImageParam.u8Str3_COMBED = param.u8Str3_COMBED;

    g_stillImageParam.u8Threshold1 = param.u8Threshold1;
    g_stillImageParam.u8Threshold2 = param.u8Threshold2;
    g_stillImageParam.u8Threshold3 = param.u8Threshold3;
    g_stillImageParam.u8Threshold4 = param.u8Threshold4;
}

void _MDrv_AVD_COMB_StillImage(void)
{
    MS_U8  u8Ctl;
    MS_U16 u16Value;
    static MS_U8 check_counter = 0;
    static MS_U16 Total_Nosiemag = 0;
    static MS_U8 status = 2;
    static MS_U16 _u16PreNoiseMag = 0;

    if(_eVDInputSource == E_INPUT_SOURCE_ATV)
        {
            u8Ctl = HAL_AVD_AFEC_GetNoiseMag();// get VD noise magnitude
            if(g_stillImageParam.bMessageOn)
            {
                //printf(" ====================>  Noise mag =%d \n",(int)u8Ctl);
            }

            if (check_counter<10)
            {
                Total_Nosiemag+= u8Ctl;
                check_counter++;
            }
            else
            {
                u16Value =Total_Nosiemag;
                if(g_stillImageParam.bMessageOn)
                {
                    //printf(" ========================>  AVG noise mag =%d \n",(int) u16Value);
                }
                u16Value = ( 8*_u16PreNoiseMag + 8*u16Value )/16;
                /////////////////////////////////////////////////////////////////////////////////////

                if(u16Value <= g_stillImageParam.u8Threshold1)
                {
                    _u16PreNoiseMag = u16Value;
                    check_counter = 0;
                    Total_Nosiemag = 0;
                    u8Ctl = HAL_AVD_GetReg(BK_COMB_38);
                    if(g_stillImageParam.bMessageOn)
                    {
                        //printf("=====>Thread =%d \n\n",u16Value);
                        printf("=====>1Thread1: u16Value =0x%x \n\n",u16Value);
                    }
                    HAL_AVD_SetReg(BK_COMB_37, g_stillImageParam.u8Str1_COMB37);
                    HAL_AVD_SetReg(BK_COMB_38, (u8Ctl & g_stillImageParam.u8Str1_COMB38));
                    HAL_AVD_SetReg(BK_COMB_7C, g_stillImageParam.u8Str1_COMB7C);
                    HAL_AVD_SetReg(BK_COMB_ED, g_stillImageParam.u8Str1_COMBED);
                    //MDrv_AVD_SetRegValue(BK_COMB_C0, 0x6C);
                    status = 1;
                }

                else if((u16Value<g_stillImageParam.u8Threshold2) && (u16Value>g_stillImageParam.u8Threshold1))
                {
                    _u16PreNoiseMag = u16Value;
                    check_counter = 0;
                    Total_Nosiemag = 0;
                    u8Ctl = HAL_AVD_GetReg(BK_COMB_38);
                    if(g_stillImageParam.bMessageOn)
                    {
                        //printf("=====>Thread =%d \n\n",u16Value);
                    }
                    if(status==1)
                    {
                       if(g_stillImageParam.bMessageOn)
                       {
                            printf("=====>4Thread1: u16Value =0x%x \n\n",u16Value);
                       }
                       HAL_AVD_SetReg(BK_COMB_37, g_stillImageParam.u8Str1_COMB37);
                       HAL_AVD_SetReg(BK_COMB_38, (u8Ctl & g_stillImageParam.u8Str1_COMB38));
                       HAL_AVD_SetReg(BK_COMB_7C, g_stillImageParam.u8Str1_COMB7C);
                       HAL_AVD_SetReg(BK_COMB_ED, g_stillImageParam.u8Str1_COMBED);
                       //MDrv_AVD_SetRegValue(BK_COMB_C0, 0x6C);
                       status = 1;
                    }
                    else if(status==2)
                    {
                       if(g_stillImageParam.bMessageOn)
                       {
                            printf("=====>5Thread2: u16Value =0x%x \n\n",u16Value);
                       }
                       HAL_AVD_SetReg(BK_COMB_37, g_stillImageParam.u8Str2_COMB37);
                       HAL_AVD_SetReg(BK_COMB_38, (u8Ctl & g_stillImageParam.u8Str2_COMB38));
                       HAL_AVD_SetReg(BK_COMB_7C, g_stillImageParam.u8Str2_COMB7C);
                       HAL_AVD_SetReg(BK_COMB_ED, g_stillImageParam.u8Str2_COMBED);
                       //MDrv_AVD_SetRegValue(BK_COMB_C0, 0x6C);
                       status = 2;
                   }
                   else if(status==3)
                   {
                       if(g_stillImageParam.bMessageOn)
                       {
                            printf("=====>6Thread3: u16Value =0x%x \n\n",u16Value);
                       }
                       status = 2;
                   }
                }

                else if((u16Value<=g_stillImageParam.u8Threshold3) && (u16Value>=g_stillImageParam.u8Threshold2))
                {
                    _u16PreNoiseMag = u16Value;
                    check_counter = 0;
                    Total_Nosiemag = 0;
                    u8Ctl = HAL_AVD_GetReg(BK_COMB_38);
                    if(g_stillImageParam.bMessageOn)
                    {
                        //printf("=====>Thread =%d \n\n",u16Value);
                        printf("=====>2Thread2: u16Value =0x%x \n\n",u16Value);
                    }
                    HAL_AVD_SetReg(BK_COMB_37, g_stillImageParam.u8Str2_COMB37);
                    HAL_AVD_SetReg(BK_COMB_38, (u8Ctl & g_stillImageParam.u8Str2_COMB38));
                    HAL_AVD_SetReg(BK_COMB_7C, g_stillImageParam.u8Str2_COMB7C);
                    HAL_AVD_SetReg(BK_COMB_ED, g_stillImageParam.u8Str2_COMBED);
                    //MDrv_AVD_SetRegValue(BK_COMB_C0, 0x6C);
                    status = 2;
                }

                else if((u16Value < g_stillImageParam.u8Threshold4) &&( u16Value > g_stillImageParam.u8Threshold3))
                {
                    _u16PreNoiseMag = u16Value;
                    check_counter = 0;
                    Total_Nosiemag = 0;
                    u8Ctl = HAL_AVD_GetReg(BK_COMB_38);
                    if(g_stillImageParam.bMessageOn)
                    {
                        //printf("=====>Thread =%d \n\n",u16Value);
                    }
                    if(status==1)
                    {
                        if(g_stillImageParam.bMessageOn)
                        {
                            printf("=====>7Thread1: u16Value =0x%x \n\n",u16Value);
                        }
                        status = 2;
                    }
                    else if(status==2)
                    {
                       if(g_stillImageParam.bMessageOn)
                       {
                            printf("=====>8Thread2: u16Value =0x%x \n\n",u16Value);
                       }
                       HAL_AVD_SetReg(BK_COMB_37, g_stillImageParam.u8Str2_COMB37);
                       HAL_AVD_SetReg(BK_COMB_38, (u8Ctl & g_stillImageParam.u8Str2_COMB38));
                       HAL_AVD_SetReg(BK_COMB_7C, g_stillImageParam.u8Str2_COMB7C);
                       HAL_AVD_SetReg(BK_COMB_ED, g_stillImageParam.u8Str2_COMBED);
                       //MDrv_AVD_SetRegValue(BK_COMB_C0, 0x6C);
                       status = 2;
                    }
                    else if(status==3)
                    {
                       if(g_stillImageParam.bMessageOn)
                       {
                            printf("=====>9Thread3: u16Value =0x%x \n\n",u16Value);
                       }
                       HAL_AVD_SetReg(BK_COMB_37, g_stillImageParam.u8Str3_COMB37);
                       HAL_AVD_SetReg(BK_COMB_38, (u8Ctl & g_stillImageParam.u8Str3_COMB38));
                       HAL_AVD_SetReg(BK_COMB_7C, g_stillImageParam.u8Str3_COMB7C);
                       HAL_AVD_SetReg(BK_COMB_ED, g_stillImageParam.u8Str3_COMBED);
                       //MDrv_AVD_SetRegValue(BK_COMB_C0, 0x6C);
                       status = 3;
                   }
                }

                else if(u16Value >= g_stillImageParam.u8Threshold4)
                {
                    _u16PreNoiseMag = u16Value;
                    check_counter = 0;
                    Total_Nosiemag = 0;
                    u8Ctl = HAL_AVD_GetReg(BK_COMB_38);
                    if(g_stillImageParam.bMessageOn)
                    {
                        //printf("=====>Thread =%d \n\n",u16Value);
                        printf("=====>3Thread3: u16Value =0x%x \n\n",u16Value);
                    }
                    HAL_AVD_SetReg(BK_COMB_37, g_stillImageParam.u8Str3_COMB37);
                    HAL_AVD_SetReg(BK_COMB_38, (u8Ctl & g_stillImageParam.u8Str3_COMB38));
                    HAL_AVD_SetReg(BK_COMB_7C, g_stillImageParam.u8Str3_COMB7C);
                    HAL_AVD_SetReg(BK_COMB_ED, g_stillImageParam.u8Str3_COMBED);
                    //MDrv_AVD_SetRegValue(BK_COMB_C0/., 0x6C);
                    status = 3;
                }


            /////////////////////////////////////////////////////////////////////////////////////

            }
        }
}

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

void MDrv_AVD_MCUFreeze(MS_BOOL bEnable) // should be static, no mutex
{
    #if AVD_DRV_DEBUG
    if (_u8AVDDbgLevel == AVD_DBGLV_DEBUG)
    {
        printf("MDrv_AVD_MCUFreeze %d\n",bEnable);
    }
    #endif
    HAL_AVD_VDMCU_SetFreeze(bEnable);
}

void MDrv_AVD_Exit(void)
{
    AVD_LOCK();
    #if AVD_DRV_DEBUG
    if (_u8AVDDbgLevel == AVD_DBGLV_DEBUG)
    {
        printf("MDrv_AVD_Exit\n");
    }
    #endif
    #if (NEW_VD_MCU)
    //HAL_AVD_VDMCU_SoftStop();
    #endif

    HAL_AVD_AFEC_SetClock(DISABLE);
    AVD_UNLOCK();
}

AVD_Result MDrv_AVD_InitCommVars(VD_INITDATA *pVD_InitData, MS_U32 u32InitDataLen)
{
	char pAVD_Mutex_String[10];
	if (NULL == strncpy(pAVD_Mutex_String,"Mutex VD",10))
	 {
	  #if AVD_DRV_DEBUG
		  if (_u8AVDDbgLevel == AVD_DBGLV_DEBUG)
		  {
			  printf("MDrv_AVD_Init strcpy Fail\n");
		   }
	  #endif
		 return E_AVD_FAIL;
	 }
	 _s32AVD_Mutex = MsOS_CreateMutex(E_MSOS_FIFO, pAVD_Mutex_String, MSOS_PROCESS_SHARED);
	 MS_ASSERT(_s32AVD_Mutex >= 0);

 #if AVD_DRV_DEBUG
	 if (_u8AVDDbgLevel == AVD_DBGLV_DEBUG)
	 {
		 printf("MDrv_AVD_Init\n");
	 }
 #endif
	 if ( sizeof(g_VD_InitData) == u32InitDataLen)
	 {
		 memcpy(&g_VD_InitData, pVD_InitData, u32InitDataLen);
	 }
	 else
	 {
		 MS_ASSERT(FALSE);
		 return E_AVD_FAIL;
	 }

	 _u16DataH[0] = _u16DataH[1] = _u16DataH[2] = 1135L;
	 _u8HtotalDebounce = 0;
	 _eVideoSystem = E_VIDEOSTANDARD_NOTSTANDARD;
	 _u8AutoDetMode = FSC_AUTO_DET_ENABLE;
	 _u16CurVDStatus = 0;
 #if AVD_DRV_DEBUG
	 _u8AVDDbgLevel = AVD_DBGLV_NONE;
	 u32VDPatchFlagStatus=u32PreVDPatchFlagStatus=0;
 #endif


	return E_AVD_OK;
}

void MDrv_AVD_COMB_SetMemoryProtect(MS_U32 u32COMB_3D_Addr, MS_U32 u32COMB_3D_Len)
{
	HAL_AVD_COMB_SetMemoryProtect(u32COMB_3D_Addr, u32COMB_3D_Len);
}

AVD_Result MDrv_AVD_Init(VD_INITDATA *pVD_InitData, MS_U32 u32InitDataLen)
{
    //BY 20090410 MS_U16 u16TimeOut;
    //BY 20090410 MS_U32 u32dst;
    //BY 20090410 MS_BOOL bResult;
    // BY 20090402 remove un-necessary code HAL_AVD_VDMCU_ResetAddress();

	AVD_Result eAvdReturn = E_AVD_FAIL;

	if ( E_AVD_OK != ( eAvdReturn = (MDrv_AVD_InitCommVars(pVD_InitData, u32InitDataLen))) )
	{
		return eAvdReturn;
	}

    HAL_AVD_RegInit(); // !! any register access should be after this function
    HAL_AVD_COMB_SetMemoryProtect(g_VD_InitData.u32COMB_3D_ADR, g_VD_InitData.u32COMB_3D_LEN);
    #if 0 // BY 20090331, for FPGA only, take on effects in real chip, by book.weng
    RIU_WriteByte(BK_AFEC_0A, 0x10);
    RIU_WriteByte(BK_AFEC_0F, 0x48);
    #endif


    #if 0 // should call set input directly to set input source
    if(g_VD_InitData.eDemodType == DEMODE_MSTAR_VIF)
    {
        RIU_WriteByte(BK_AFEC_CF, 0x80);
        RIU_WriteByte(BK_AFEC_21, 0x1D); // TODO check setting correct or not?
        RIU_WriteByte(BK_AFEC_40, 0x91);
    }
    #endif

    #if 0 // BY 20090402
    //========MDrv_VD_LoadDSPCode=======
    BinInfo.B_ID = g_VD_InitData.u16VDDSPBinID;
    MDrv_DMA_Get_BinInfo(&BinInfo, &bResult);
    if( bResult != PASS )
    {
        return;
    }
    #endif

    #if( (VD_CHIP_VERSION!=AVD_TITANIA3_A) && (VD_CHIP_VERSION!=AVD_AMBER1_A))
    _MDrv_AVD_VDMCU_Init(); // should keep this for T3 because VIF initial will need mailbox
    #endif

    return E_AVD_OK;
}

MS_U16 MDrv_AVD_GetStatus(void)
{
    return HAL_AVD_AFEC_GetStatus();
}

AVD_Result MDrv_AVD_GetLibVer(const MSIF_Version **ppVersion)
{
    // No mutex check, it can be called before Init
    if (!ppVersion)
    {
        return E_AVD_FAIL;
    }

    *ppVersion = &_drv_avd_version;

    return E_AVD_OK;
}

MS_BOOL MDrv_AVD_SetDbgLevel(AVD_DbgLv u8DbgLevel)
{
    AVD_LOCK();
    #if AVD_DRV_DEBUG
    _u8AVDDbgLevel = u8DbgLevel;
    #endif
    AVD_UNLOCK();
    return TRUE;
}

const AVD_Info* MDrv_AVD_GetInfo(void)
{
    static AVD_Info sAVD_Info;
    sAVD_Info.eVDInputSource=_eVDInputSource;
    sAVD_Info.eVideoSystem=_eVideoSystem;
    sAVD_Info.eLastStandard=_eLastStandard;
    sAVD_Info.u8AutoDetMode=_u8AutoDetMode;
    sAVD_Info.u16CurVDStatus=_u16CurVDStatus;
    sAVD_Info.u8AutoTuningIsProgress=_u8AutoTuningIsProgress;
    return & sAVD_Info;
}

MS_BOOL MDrv_AVD_IsSyncLocked(void)
{
    if( IS_BITS_SET(HAL_AVD_AFEC_GetStatus(), VD_HSYNC_LOCKED) ) // should not check vsync lock during channel scan
    {
        return TRUE;
    }
    return FALSE;
}

#if 0 // use MDrv_AVD_IsHsyncLocked
////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_VD_IsSyncDetected()
/// @brief \b Function \b Description: To get whether horizontal sync is detected or not.
/// @param <IN>        \b None     :
///
///
/// @param <OUT>       \b None     :
/// @param <RET>       \b MS_BOOL  : TRUE : horizontal Sync is detected.
///                                  FALSE: there is no sync.
/// @param <GLOBAL>    \b None     :
////////////////////////////////////////////////////////////////////////////////
MS_BOOL MDrv_AVD_IsSyncDetected(void)
{
    if ( IS_BITS_SET(HAL_AVD_AFEC_GetStatus(), VD_HSYNC_LOCKED) )
    {
        return TRUE;
    }
    return FALSE;
}
#endif

#if 0
MS_U16 MDrv_AVD_Scan_HsyncCheck(MS_U8 u8HtotalTolerance) //TODO should add delay between register read
{
    MS_U16 u16CurrentStatus = VD_RESET_ON;
    MS_U16 u16Dummy, u16Temp, u16SPLHTotal;
    MS_U8 u8Value, u8VdHsyncLockedCount;
    static const MS_U16 u16SPL_NSPL[5] =
    {
        1135,   // PAL
        1097,   // SECAM
        910,    // NTSC, PAL M
        917,    // PAL Nc
        1127,   // NTSC 4.43
    };

    AVD_LOCK();

    u16Dummy = 20; // just protect program dead lock
    u8VdHsyncLockedCount = 0;
    while( u16Dummy-- )
    {
        if( HAL_AVD_AFEC_GetHWHsync() )
        {
            u16CurrentStatus = HAL_AVD_AFEC_GetStatus();
            u8VdHsyncLockedCount++;
        }
    }
    //printf("vdLoop1=%bd\n", u8VdHsyncLockedCount);

    if( u8VdHsyncLockedCount>14 )
    {
        //printf("Second Check\n");
        u8VdHsyncLockedCount = 0;
        u16Dummy = 10; // just protect program dead lock
        u16SPLHTotal = HAL_AVD_AFEC_GetHTotal(); // SPL_NSPL, H total
        //printf("Ht=%d\n", u16SPLHTotal);
        while( u16Dummy-- )
        {
           u16Temp = HAL_AVD_AFEC_GetHTotal(); // SPL_NSPL, H total
            u16Temp = (u16Temp+u16SPLHTotal)>>1;
            u16SPLHTotal = u16Temp;

            //printf("Ht=%d\n", u16SPLHTotal);
            for(u8Value=0; u8Value<=4;u8Value++)
            {
                if( abs(u16SPLHTotal-u16SPL_NSPL[u8Value]) < u8HtotalTolerance )
                {
                    //printf("+");
                    u8VdHsyncLockedCount++;
                }
                else
                {
                    //printf("-");
                }
            }
            //printf("\n");
        }

        //printf("vdLoop2=%bd\n", u8VdHsyncLockedCount);
        if( u8VdHsyncLockedCount < 8 )
        {
            u16CurrentStatus = VD_RESET_ON;
        }
    }
    else
    {
        u16CurrentStatus = VD_RESET_ON;
    }

    AVD_UNLOCK();
    return u16CurrentStatus;
}
#else
MS_U16 MDrv_AVD_Scan_HsyncCheck(MS_U8 u8HtotalTolerance)
{
    MS_U8 u8VdHsyncLockedCount,u8Value,u8ValuePrev;
    MS_U16 wLatchH,u16Dummy,u16Temp;
    static const MS_U16 u16SPL_NSPL[5] =
    {
        1135,   // PAL
        1097,   // SECAM
        910,    // NTSC, PAL M
        917,    // PAL Nc
        1127,   // NTSC 4.43
    };

    AVD_LOCK();
    u16Dummy = VD_CHK_HSYNC_CONT;
    u8VdHsyncLockedCount = 0;

    while (u16Dummy--)
    {
        if( HAL_AVD_AFEC_GetHWHsync() )
        {
            u8VdHsyncLockedCount++;
        }

        if(u8VdHsyncLockedCount>VD_CHK_HSYNC_OK_THR)
            break;
        MsOS_DelayTask(VD_CHK_HSYNC_WAIT);
    }

    #if AVD_DRV_DEBUG
        if (_u8AVDDbgLevel == AVD_DBGLV_DEBUG)
        {
            printf("\r\n VD_HW1st Check Hsync found=%d, Loop=%d \r\n",u8VdHsyncLockedCount,u16Dummy);
        }
    #endif

    if(u8VdHsyncLockedCount>VD_CHK_HSYNC_OK_THR)
    {

        #if AVD_DRV_DEBUG
            if (_u8AVDDbgLevel == AVD_DBGLV_DEBUG)
            {
                printf("Second Check \r\n");
            }
        #endif

        u16Dummy=VD_CHK_DEBOUNCE_CONT;
        while(u16Dummy--)
        {
            MsOS_DelayTask(VD_CHK_DEBOUNCE_WAIT);
            HAL_AVD_SetReg (BK_AFEC_04, 0x00);
            if ((HAL_AVD_GetReg (BK_AFEC_01) & 0xF0) >= 0x60)
            {
                #if AVD_DRV_DEBUG
                    if (_u8AVDDbgLevel == AVD_DBGLV_DEBUG)
                    {
                        printf("Debounce OK, count=%ld \r\n",(MS_U32)(VD_CHK_DEBOUNCE_CONT-u16Dummy));
                    }
                #endif
                u16Dummy = VD_CHK_NSPL_CONT;
                u8VdHsyncLockedCount = 0;
                u8ValuePrev=2;

                wLatchH = HAL_AVD_AFEC_GetHTotal(); // SPL_NSPL, H total

                while (u16Dummy--)
                {
                    MsOS_DelayTask(1);
                    u16Temp = HAL_AVD_AFEC_GetHTotal(); // SPL_NSPL, H total
                    u16Temp = (u16Temp+wLatchH)>>1;
                    wLatchH = u16Temp;
                    #if AVD_DRV_DEBUG
                        if (_u8AVDDbgLevel == AVD_DBGLV_DEBUG)
                        {
                            printf("Htotal = %d\n", wLatchH);
                        }
                    #endif

                    for (u8Value = 0; u8Value <= 4; u8Value++)
                    {
                        if (abs (wLatchH - u16SPL_NSPL[u8Value]) < u8HtotalTolerance)
                        {
                            if (u8ValuePrev==u8Value)
                                u8VdHsyncLockedCount++;
                            else
                                u8VdHsyncLockedCount=0;
                            u8ValuePrev=u8Value;

                            if(u8VdHsyncLockedCount>=VD_CHK_NSPL_OK_THR)
                            {
                                #if AVD_DRV_DEBUG
                                if (_u8AVDDbgLevel == AVD_DBGLV_DEBUG)
                                {
                                    printf("NSPL OK, count=%ld \r\n",(MS_U32)(VD_CHK_NSPL_CONT-u16Dummy));
                                }
                                #endif
                                AVD_UNLOCK();
                                return VD_HSYNC_LOCKED;
                            }
                            break;
                        }
                    }
                    if((u8VdHsyncLockedCount)&&(u8Value==5))
                    {
                        #if AVD_DRV_DEBUG
                            if (_u8AVDDbgLevel == AVD_DBGLV_DEBUG)
                            {
                                printf("Htotal Unstable!\r\n");
                            }
                        #endif
                        u8VdHsyncLockedCount=0;
                    }
                }
                #if AVD_DRV_DEBUG
                    if (_u8AVDDbgLevel == AVD_DBGLV_DEBUG)
                    {
                        printf("VD NSPL Fail\n");
                    }
                #endif
                break;
            }
        }
        #if AVD_DRV_DEBUG
            if (_u8AVDDbgLevel == AVD_DBGLV_DEBUG)
            {
            printf("VD MCU De Bounce Fail\n");
            }
        #endif
    }
    AVD_UNLOCK();
    return VD_RESET_ON;
}

#endif



MS_BOOL MDrv_AVD_IsSignalInterlaced(void)
{
    if ( IS_BITS_SET(HAL_AVD_AFEC_GetStatus(), VD_INTERLACED | VD_HSYNC_LOCKED | VD_STATUS_RDY) )
    {
        return TRUE;
    }
    return FALSE;
}

MS_BOOL MDrv_AVD_IsColorOn(void)
{
    if ( IS_BITS_SET(HAL_AVD_AFEC_GetStatus(), VD_COLOR_LOCKED | VD_HSYNC_LOCKED | VD_STATUS_RDY) )
    {
        return TRUE;
    }
    return FALSE;
}

#if 0 // BY 20090403 unused function
////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_VD_IsStandardValid()
/// @brief \b Function \b Description: To get whether video standard is valid or not.
/// @param <IN>        \b None     :
///
///
/// @param <OUT>       \b None     :
/// @param <RET>       \b MS_BOOL  : TRUE : video standard is valid.
///                                  FALSE: video standard is invalid.
/// @param <GLOBAL>    \b None     :
////////////////////////////////////////////////////////////////////////////////
MS_BOOL MDrv_AVD_IsStandardValid(void)
{
#if (TEST_VD_DSP)
    if( !IS_BITS_SET(HAL_AVD_AFEC_GetStatus(), VD_STANDARD_VALID) )
    {
        return FALSE;
    }
    return TRUE;
#else
    if( _u8AutoDetMode == FSC_AUTO_DET_ENABLE )   // AUTO DET
    {
        if( !IS_BITS_SET(HAL_AVD_AFEC_GetStatus(), VD_STATUS_RDY) )
        {
            return FALSE;
        }
        return TRUE;
    }
    else                                        // FORCE MODE
    {
        MS_U16 u16VDStatus = (HAL_AVD_AFEC_GetStatus() & VD_MODE_STANDARD_MASK);
        static MS_U8 u8StableCnt;

        // The PAL_SWITCH for SECAM is invalid
        if ((u16VDStatus & VD_FSC_TYPE) == VD_FSC_4285)
        {
            u16VDStatus &= ~VD_PAL_SWITCH;
        }

        // debounce?
        if (u16VDStatus == _u16CurVDStatus)
        {
            u8StableCnt = 0;
            return TRUE;
        }
        else
        {
            if (u8StableCnt > IS_STANDARD_VALID_STABLE_CNT)
            {
                VDDBG(printf(" F FAIL, %04X, %04X\n", _u16CurVDStatus, u16VDStatus)); // <-<<<
                return FALSE;
            }
            else
            {
                u8StableCnt++;
                return TRUE;
            }
        }
    }

#endif
}
#endif

AVD_VideoFreq MDrv_AVD_GetVerticalFreq(void)
{
    MS_U16 u16Status;

    u16Status = HAL_AVD_AFEC_GetStatus();
    if( IS_BITS_SET(u16Status, VD_HSYNC_LOCKED | VD_STATUS_RDY) )
    {
        if( VD_VSYNC_50HZ & u16Status )
        {
            return E_VIDEO_FQ_50Hz;
        }
        else
        {
            return E_VIDEO_FQ_60Hz;
        }
    }
    return E_VIDEO_FQ_NOSIGNAL;
}

MS_U16 MDrv_AVD_GetVTotal(void)
{
    return HAL_AVD_AFEC_GetVTotal();
}

MS_U8 MDrv_AVD_GetNoiseMag(void)
{
    return HAL_AVD_AFEC_GetNoiseMag();
}

MS_BOOL MDrv_AVD_SetInput(AVD_InputSourceType eSource, MS_U8 u8ScartFB) //SCART_FB eFB)
{
    //Basic Input Checking
    AVD_LOCK();
    #if AVD_DRV_DEBUG
    if (_u8AVDDbgLevel == AVD_DBGLV_DEBUG)
    {
        printf("MDrv_AVD_SetInput %d\n",eSource);
    }
    #endif
    if (eSource >= E_INPUT_SOURCE_MAX)
        MS_ASSERT(FALSE);

    eSource= (AVD_InputSourceType)(((MS_U8)eSource) & 0x0F);
    _eVDInputSource=eSource;

    HAL_AVD_AFEC_SetInput(eSource,u8ScartFB,g_VD_InitData.eDemodType, g_VD_InitData.u32XTAL_Clock);

	///Brian A1 VIF and ADC different AGC mapping
     #if(VD_CHIP_VERSION==AVD_AMBER1_A)

         	  if(eSource==E_INPUT_SOURCE_ATV)
         	    {
         	       HAL_AVD_SetDSPCodeType(AVD_DSP_CODE_TYPE_VIF);

         	    }
	        else
	          {
	         	HAL_AVD_SetDSPCodeType(AVD_DSP_CODE_TYPE_ADC);
	          }
         	 _MDrv_AVD_VDMCU_Init();
   #endif



    // set gain
    if (eSource == E_INPUT_SOURCE_ATV)
    {
        if (g_VD_InitData.bRFGainSel==VD_USE_FIX_GAIN)
        {
            HAL_AVD_AFEC_AGCSetMode(AVD_AGC_DISABLE);

            if(g_VD_InitData.eDemodType == DEMODE_MSTAR_VIF)
            {
                HAL_AVD_AFEC_AGCSetCoarseGain(VD_AGC_COARSE_GAIN);
                HAL_AVD_AFEC_AGCSetFineGain(g_VD_InitData.u8RFGain);
            }
            else
            {
                HAL_AVD_AFEC_AGCSetCoarseGain(VD_AGC_COARSE_GAIN);
                HAL_AVD_AFEC_AGCSetFineGain(g_VD_InitData.u8RFGain);
            }
        }
        else
        {
            HAL_AVD_AFEC_AGCSetMode(AVD_AGC_ENABLE);
        }
    }
    else
    {
        if (g_VD_InitData.bAVGainSel==VD_USE_FIX_GAIN)
        {
            HAL_AVD_AFEC_AGCSetMode(AVD_AGC_DISABLE);
            HAL_AVD_AFEC_AGCSetCoarseGain(VD_AGC_COARSE_GAIN);
            HAL_AVD_AFEC_AGCSetFineGain(g_VD_InitData.u8AVGain);
        }
        else
        {
            HAL_AVD_AFEC_AGCSetMode(AVD_AGC_ENABLE);
        }
    }

    // VD MCU Reset
    // BY 20090410, move function body here  MDrv_AVD_McuReset();
    #if (VD_CHIP_VERSION==AVD_TITANIA3_A)
    _MDrv_AVD_VDMCU_Init(); // must reload code for T3
    #else
    HAL_AVD_AFEC_McuReset();
    #endif
    HAL_AVD_AFEC_SetPatchFlag(g_VD_InitData.u32VDPatchFlag);
    // Forced to PAL mode
    _eLastStandard = E_VIDEOSTANDARD_NOTSTANDARD;
    _eVideoSystem=E_VIDEOSTANDARD_PAL_BGHI;
    _u16CurVDStatus = 0;

    AVD_UNLOCK();

    return TRUE;
}

void MDrv_AVD_Set_Htt_UserMD(MS_U16 u16Htt)
{
    _u16Htt_UserMD = u16Htt;
}

MS_BOOL MDrv_AVD_SetVideoStandard(AVD_VideoStandardType eStandard, MS_BOOL bIsInAutoTuning)
{
    AVD_LOCK();

    #if AVD_DRV_DEBUG
    if (_u8AVDDbgLevel == AVD_DBGLV_DEBUG)
    {
        printf("MDrv_AVD_SetVideoStandard %d\n",eStandard);
    }
    #endif

    _eVideoSystem = eStandard;

   //Check input
   if (eStandard>=E_VIDEOSTANDARD_MAX)
        MS_ASSERT(FALSE);

    switch (eStandard)
    {
    case E_VIDEOSTANDARD_NTSC_44:
        if((g_VD_InitData.u32VDPatchFlag & AVD_PATCH_HTOTAL_MASK) == AVD_PATCH_HTOTAL_USER)
        {
            HAL_AVD_AFEC_SetHTotal(_u16Htt_UserMD);
        }
        else
        {
            HAL_AVD_AFEC_SetHTotal(_u16HtotalNTSC_443[(g_VD_InitData.u32VDPatchFlag & AVD_PATCH_HTOTAL_MASK)>>8]* (VD_USE_FB + 1));
        }

        HAL_AVD_AFEC_SetBT656Width(0xB4);
        _u16LatchH = 1127L;
        _u16CurVDStatus= VD_FSC_4433 | VD_STATUS_RDY;
        HAL_AVD_AFEC_SetFSCMode(FSC_MODE_NTSC_443);
        break;
    case E_VIDEOSTANDARD_PAL_60:    // 1127
        if((g_VD_InitData.u32VDPatchFlag & AVD_PATCH_HTOTAL_MASK) == AVD_PATCH_HTOTAL_USER)
        {
            HAL_AVD_AFEC_SetHTotal(_u16Htt_UserMD);
        }
        else
        {
            HAL_AVD_AFEC_SetHTotal(_u16HtotalPAL_60[(g_VD_InitData.u32VDPatchFlag & AVD_PATCH_HTOTAL_MASK)>>8] * (VD_USE_FB + 1));
        }
        HAL_AVD_AFEC_SetBT656Width(0xB4);
        _u16LatchH = 1127L;
        _u16CurVDStatus= VD_PAL_SWITCH | VD_FSC_4433 | VD_STATUS_RDY;
        HAL_AVD_AFEC_SetFSCMode(FSC_MODE_PAL_60);
        break;
    case E_VIDEOSTANDARD_SECAM:
        if ( bIsInAutoTuning )
        {
            if((g_VD_InitData.u32VDPatchFlag & AVD_PATCH_HTOTAL_MASK) == AVD_PATCH_HTOTAL_USER)
            {
                HAL_AVD_AFEC_SetHTotal(_u16Htt_UserMD);
            }
            else
            {
                HAL_AVD_AFEC_SetHTotal(_u16HtotalPAL[(g_VD_InitData.u32VDPatchFlag & AVD_PATCH_HTOTAL_MASK)>>8] * (VD_USE_FB + 1));
            }
        }
        else
        {
            if((g_VD_InitData.u32VDPatchFlag & AVD_PATCH_HTOTAL_MASK) == AVD_PATCH_HTOTAL_USER)
            {
                HAL_AVD_AFEC_SetHTotal(_u16Htt_UserMD);
            }
            else
            {
                HAL_AVD_AFEC_SetHTotal(_u16HtotalSECAM[(g_VD_InitData.u32VDPatchFlag & AVD_PATCH_HTOTAL_MASK)>>8] * (VD_USE_FB + 1));
            }
        }

        HAL_AVD_AFEC_SetBT656Width(0xB2);
        _u16LatchH = 1097L;
        _u16CurVDStatus = (VD_VSYNC_50HZ | VD_FSC_4285 | VD_STATUS_RDY);
        HAL_AVD_AFEC_SetFSCMode(FSC_MODE_SECAM);
        break;

    case E_VIDEOSTANDARD_PAL_M: // 909
        if((g_VD_InitData.u32VDPatchFlag & AVD_PATCH_HTOTAL_MASK) == AVD_PATCH_HTOTAL_USER)
        {
            HAL_AVD_AFEC_SetHTotal(_u16Htt_UserMD);
        }
        else
        {
            HAL_AVD_AFEC_SetHTotal(_u16HtotalPAL_M[(g_VD_InitData.u32VDPatchFlag & AVD_PATCH_HTOTAL_MASK)>>8] * (VD_USE_FB + 1));
        }
        HAL_AVD_AFEC_SetBT656Width(0x8E);
        _u16LatchH = 909L;
        _u16CurVDStatus = (VD_PAL_SWITCH | VD_FSC_3575 | VD_STATUS_RDY);
        HAL_AVD_AFEC_SetFSCMode(FSC_MODE_PAL_M);
        break;

    case E_VIDEOSTANDARD_PAL_N: // 917
        if((g_VD_InitData.u32VDPatchFlag & AVD_PATCH_HTOTAL_MASK) == AVD_PATCH_HTOTAL_USER)
        {
            HAL_AVD_AFEC_SetHTotal(_u16Htt_UserMD);
        }
        else
        {
            HAL_AVD_AFEC_SetHTotal(_u16HtotalPAL_NC[(g_VD_InitData.u32VDPatchFlag & AVD_PATCH_HTOTAL_MASK)>>8] * (VD_USE_FB + 1));
        }
        HAL_AVD_AFEC_SetBT656Width(0x93);
        _u16LatchH = 917L;
        _u16CurVDStatus = (VD_VSYNC_50HZ | VD_PAL_SWITCH | VD_FSC_3582 | VD_STATUS_RDY);
        HAL_AVD_AFEC_SetFSCMode(FSC_MODE_PAL_N);
        break;

    case E_VIDEOSTANDARD_NTSC_M: // 910
        if((g_VD_InitData.u32VDPatchFlag & AVD_PATCH_HTOTAL_MASK) == AVD_PATCH_HTOTAL_USER)
        {
            HAL_AVD_AFEC_SetHTotal(_u16Htt_UserMD);
        }
        else
        {
            HAL_AVD_AFEC_SetHTotal(_u16HtotalNTSC[(g_VD_InitData.u32VDPatchFlag & AVD_PATCH_HTOTAL_MASK)>>8] * (VD_USE_FB + 1));
        }
        HAL_AVD_AFEC_SetBT656Width(0x6D);
        _u16LatchH = 910L;
        _u16CurVDStatus = (VD_FSC_3579 | VD_STATUS_RDY);
        HAL_AVD_AFEC_SetFSCMode(FSC_MODE_NTSC);
        break;
    default: // case E_VIDEOSTANDARD_PAL_BGHI:  // 1135
        if((g_VD_InitData.u32VDPatchFlag & AVD_PATCH_HTOTAL_MASK) == AVD_PATCH_HTOTAL_USER)
        {
            HAL_AVD_AFEC_SetHTotal(_u16Htt_UserMD);
        }
        else
        {
            HAL_AVD_AFEC_SetHTotal(_u16HtotalPAL[(g_VD_InitData.u32VDPatchFlag & AVD_PATCH_HTOTAL_MASK)>>8] * (VD_USE_FB + 1));
        }
        HAL_AVD_AFEC_SetBT656Width(0xB6);
        _u16LatchH = 1135L;
        _u16CurVDStatus = (VD_VSYNC_50HZ | VD_PAL_SWITCH | VD_FSC_4433 | VD_STATUS_RDY);
        HAL_AVD_AFEC_SetFSCMode(FSC_MODE_PAL);
        break;
    }

    if ((_eVDInputSource == E_INPUT_SOURCE_SVIDEO1) || (_eVDInputSource == E_INPUT_SOURCE_SVIDEO2))
    {
        if (E_VIDEOSTANDARD_NTSC_M == eStandard)
        {
            HAL_AVD_COMB_SetYCPipe(0x20);
        }
        else // SIG_NTSC_443, SIG_PAL, SIG_PAL_M, SIG_PAL_NC, SIG_SECAM
        {
            HAL_AVD_COMB_SetYCPipe(0x30);
        }

        switch (eStandard)
        {
        case E_VIDEOSTANDARD_PAL_M:
        case E_VIDEOSTANDARD_PAL_N:
            HAL_AVD_COMB_SetCbCrInverse(0x04);
            break;

        case E_VIDEOSTANDARD_NTSC_44:
            HAL_AVD_COMB_SetCbCrInverse(0x0C);
            break;

        case E_VIDEOSTANDARD_PAL_60:
            HAL_AVD_COMB_SetCbCrInverse(0x08);
            break;

        case E_VIDEOSTANDARD_NTSC_M:
        case E_VIDEOSTANDARD_SECAM:
            HAL_AVD_COMB_SetCbCrInverse(0x00);
            break;

        case E_VIDEOSTANDARD_PAL_BGHI:
        default:
            HAL_AVD_COMB_SetCbCrInverse(0x08);
            break;
        }
    }
    else
    {
        HAL_AVD_COMB_SetYCPipe(0x20);
        HAL_AVD_COMB_SetCbCrInverse(0x00);
    }

    if (g_VD_InitData.u32VDPatchFlag & AVD_PATCH_NTSC_50)
    {
        if ((HAL_AVD_AFEC_GetStatus() & (VD_FSC_TYPE|VD_VSYNC_50HZ|VD_COLOR_LOCKED))
              == (VD_FSC_3579|VD_VSYNC_50HZ|VD_COLOR_LOCKED)) // NTSC-50
        {
            HAL_AVD_COMB_SetVerticalTimingDetectMode(0x02);
        }
        else
        {
            HAL_AVD_COMB_SetVerticalTimingDetectMode(0x00);
        }
    }

    if (eStandard == E_VIDEOSTANDARD_NTSC_44)
    {
        HAL_AVD_COMB_SetLineBufferMode(0x06);
        HAL_AVD_COMB_SetHtotal(0x467);
    }
    else if ((g_VD_InitData.u32VDPatchFlag & AVD_PATCH_NTSC_50) && ((HAL_AVD_AFEC_GetStatus() & (VD_FSC_TYPE|VD_VSYNC_50HZ|VD_COLOR_LOCKED))
               == (VD_FSC_3579|VD_VSYNC_50HZ|VD_COLOR_LOCKED))) // NTSC-50
    {
        HAL_AVD_COMB_SetLineBufferMode(0x06);
        HAL_AVD_COMB_SetHtotal(0x396);
    }
    else
    {
        HAL_AVD_COMB_SetLineBufferMode(0x07);
        HAL_AVD_COMB_SetHtotal(0x38D);
    }

    if (eStandard == E_VIDEOSTANDARD_SECAM)
    {
        HAL_AVD_VBI_SetVPSPhaseAcc(0x9A6D);
    }
    else
    {
        HAL_AVD_VBI_SetVPSPhaseAcc(0x018C);
    }

    {
        if (HAL_AVD_AFEC_GetBurstOn())
        {
            _MDrv_AVD_3DCombSpeedup();
        }
    }

#if 0
    if (g_VD_InitData.u32VDPatchFlag & AVD_PATCH_FINE_TUNE_COMB_F2) //bug's life, for Eris ATT, James.Lu, 20080327
    {

        if(((_eVDInputSource == E_INPUT_SOURCE_CVBS1) ||
            (_eVDInputSource == E_INPUT_SOURCE_CVBS2) ||
            ( _eVDInputSource == E_INPUT_SOURCE_CVBS3)) && (eStandard == E_VIDEOSTANDARD_NTSC_M))
        {
            HAL_AVD_COMB_SetF2(0x57);
        }
        else
        {
            HAL_AVD_COMB_SetF2(0x47);
        }
    }


    if (g_VD_InitData.u32VDPatchFlag & AVD_PATCH_FINE_TUNE_3D_COMB)
    {
        HAL_AVD_COMB_Set3dFineTune((eStandard==E_VIDEOSTANDARD_PAL_BGHI));
    }


    {
        switch (eStandard)
        {
            case E_VIDEOSTANDARD_PAL_BGHI:
                HAL_AVD_COMB_Set3dDetectionTolerance(0x06);
                break;
            case E_VIDEOSTANDARD_PAL_M:
                HAL_AVD_COMB_Set3dDetectionTolerance(0x04);
                break;
            default:
                HAL_AVD_COMB_Set3dDetectionTolerance(0x05);
                break;
        }
    }
#endif

    if (g_VD_InitData.u32VDPatchFlag & AVD_PATCH_NON_STANDARD_VTOTAL) //bug's life, for Eris ATT, James.Lu, 20080327
    {
#ifndef MSOS_TYPE_LINUX
        if(
            ((_eVDInputSource == E_INPUT_SOURCE_CVBS1) ||
            (_eVDInputSource == E_INPUT_SOURCE_CVBS2) ||
            ( _eVDInputSource == E_INPUT_SOURCE_CVBS3) ||
            ( _eVDInputSource == E_INPUT_SOURCE_ATV)) &&
            (eStandard!=E_VIDEOSTANDARD_NOTSTANDARD)
            )
        {
            MS_U16 u16Vtotal=HAL_AVD_AFEC_GetVTotal();
            if ((u16Vtotal < 675 ) && (u16Vtotal > 570))
                u16PreVtotal = 625;
            else
                u16PreVtotal = 525;
        }
        else
        {
            u16PreVtotal = 0;
        }
#endif
    }
    AVD_UNLOCK();

    return TRUE;
}

void MDrv_AVD_StartAutoStandardDetection(void)
{
    AVD_LOCK();
    _u8AutoDetMode=FSC_AUTO_DET_ENABLE;
    HAL_AVD_AFEC_EnableForceMode(DISABLE);
    AVD_UNLOCK();
}


#if 0 // BY 20090403 unused function
////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_VD_StopAutoStandardDetection()
/// @brief \b Function \b Description: To stop auto detection mode.
/// @param <IN>        \b None     :
///
///
/// @param <OUT>       \b None     :
/// @param <RET>       \b None     :
/// @param <GLOBAL>    \b None     :
////////////////////////////////////////////////////////////////////////////////
void MDrv_VD_StopAutoStandardDetection(void)
{
    _u8AutoDetMode=FSC_AUTO_DET_DISABLE;
    HAL_AVD_AFEC_EnableForceMode(FALSE); // BY 20090403 always turn off force mode, TODO disable this at api layer
}
#endif

MS_BOOL MDrv_AVD_ForceVideoStandard(AVD_VideoStandardType eVideoStandardType)
{
   //Check input
   AVD_LOCK();

    #if AVD_DRV_DEBUG
    if (_u8AVDDbgLevel == AVD_DBGLV_DEBUG)
    {
        printf("MDrv_AVD_ForceVideoStandard %d\n",eVideoStandardType);
    }
    #endif

    if (eVideoStandardType >= E_VIDEOSTANDARD_MAX)
        MS_ASSERT(FALSE);

    _u8AutoDetMode= FSC_AUTO_DET_DISABLE;
    _eForceVideoStandard=eVideoStandardType;
    HAL_AVD_AFEC_EnableForceMode(ENABLE);
    switch(eVideoStandardType)
    {
        case E_VIDEOSTANDARD_NTSC_M:
            HAL_AVD_AFEC_SetFSCMode(FSC_MODE_NTSC);
            break;
        case E_VIDEOSTANDARD_SECAM:
            HAL_AVD_AFEC_SetFSCMode(FSC_MODE_SECAM);
            break;
        case E_VIDEOSTANDARD_NTSC_44:
            HAL_AVD_AFEC_SetFSCMode(FSC_MODE_NTSC_443);
            break;
        case E_VIDEOSTANDARD_PAL_M:
            HAL_AVD_AFEC_SetFSCMode(FSC_MODE_PAL_M);
            break;
        case E_VIDEOSTANDARD_PAL_N:
            HAL_AVD_AFEC_SetFSCMode(FSC_MODE_PAL_N);
            break;
        case E_VIDEOSTANDARD_PAL_60:
            HAL_AVD_AFEC_SetFSCMode(FSC_MODE_PAL_60);
            break;
        default:    //E_VIDEOSTANDARD_PAL_BGHI
            HAL_AVD_AFEC_SetFSCMode(FSC_MODE_PAL);
            break;
    }

    AVD_UNLOCK();

    return TRUE;
}

#if 0 // BY 20090403 use MDrv_VD_StartAutoStandardDetection, not supported
////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_VD_DisableForceMode()
/// @brief \b Function \b Description: To disable VD forced mode.
/// @param <IN>        \b None     :
///
///
/// @param <OUT>       \b None     :
/// @param <RET>       \b None     :
/// @param <GLOBAL>    \b None     :
////////////////////////////////////////////////////////////////////////////////
void MDrv_VD_DisableForceMode(void)
{
    RIU_WriteByteMask( BK_AFEC_CE, 0, BIT0 );
    _u8AutoDetMode= FSC_AUTO_DET_ENABLE;
}
#endif

#if 0 // BY 20090403 use MDrv_VD_GetStandardDetection, not supported
////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_VD_GetResultOfAutoStandardDetection()
/// @brief \b Function \b Description: To get result of auto detection.
/// @param <IN>        \b None     :
///
///
/// @param <OUT>       \b None     :
/// @param <RET>       \b AVD_VideoStandardType: the detected video standard.
/// @param <GLOBAL>    \b None     :
////////////////////////////////////////////////////////////////////////////////
AVD_VIDEOSTANDARD_TYPE MDrv_VD_GetResultOfAutoStandardDetection(void)
{
    AVD_VIDEOSTANDARD_TYPE eVideoStandard;

    eVideoStandard = MDrv_VD_GetStandardDetection();

    return eVideoStandard;
}
#endif

AVD_VideoStandardType MDrv_AVD_GetLastDetectedStandard(void)
{
    return _eLastStandard;
}

AVD_VideoStandardType MDrv_AVD_GetStandardDetection(MS_U16 *u16LatchStatus)
{
    MS_U16 u16Status;
    AVD_VideoStandardType eVideoStandard;

    AVD_LOCK();
    *u16LatchStatus = u16Status = HAL_AVD_AFEC_GetStatus();


    if ( !IS_BITS_SET(u16Status, VD_HSYNC_LOCKED|VD_SYNC_LOCKED|VD_STATUS_RDY) )
    {
        _eLastStandard=E_VIDEOSTANDARD_NOTSTANDARD;
        AVD_UNLOCK();
        return E_VIDEOSTANDARD_NOTSTANDARD;
    }


    if (_u8AutoDetMode==FSC_AUTO_DET_DISABLE)
    {
        eVideoStandard = _eForceVideoStandard;
    }
    else
    {
        if (u16Status & VD_BURST_ON)
        {
            switch (u16Status & VD_FSC_TYPE)
            {
                case VD_FSC_4433: // (FSC_MODE_PAL << 5):
                    HAL_AVD_ADC_SetGMC(0x06);
                    #if (!TEST_VD_DSP)
                    if (u16Status & VD_VSYNC_50HZ) // 50 Hz
                    #endif
                    {
                        eVideoStandard = E_VIDEOSTANDARD_PAL_BGHI;
                    }
                    #if (!TEST_VD_DSP)
                    else // 60MHz
                    {
                        if (u16Status & VD_PAL_SWITCH)
                        {
                            eVideoStandard = E_VIDEOSTANDARD_PAL_60; // or vsdNTSC_44
                        }
                        else
                        {
                            eVideoStandard = E_VIDEOSTANDARD_NTSC_44;
                        }
                    }
                    #endif
                    break;

                case VD_FSC_3579:
                    if (g_VD_InitData.u32VDPatchFlag & AVD_PATCH_NTSC_50)
                    {
                         if ( u16Status & VD_VSYNC_50HZ ) // 50Hz
                         {
                             // 3.579545MHz, NTSC-50
                             // glock RIU_WriteByteMask(L_BK_ADC_ATOP(0x37), 0x07, 0x07);
                             eVideoStandard = E_VIDEOSTANDARD_PAL_BGHI;
                         }
                         else
                        {
                             // 3.579545MHz, NTSC
                             HAL_AVD_ADC_SetGMC(0x07);
                             eVideoStandard = E_VIDEOSTANDARD_NTSC_M;
                        }
                    }
                    else
                    {
                        HAL_AVD_ADC_SetGMC(0x07);
                        eVideoStandard = E_VIDEOSTANDARD_NTSC_M;
                    }
                    break;

                case VD_FSC_3575: // (FSC_MODE_PAL_M << 5):
                    HAL_AVD_ADC_SetGMC(0x07);
                    eVideoStandard = E_VIDEOSTANDARD_PAL_M;
                    break;

                case VD_FSC_3582: // (FSC_MODE_PAL_N << 5):
                    HAL_AVD_ADC_SetGMC(0x07);
                    eVideoStandard = E_VIDEOSTANDARD_PAL_N;
                    break;

                #if (TEST_VD_DSP)
                case (FSC_MODE_NTSC_443 << 5):
                    eVideoStandard = E_VIDEOSTANDARD_NTSC_44;
                    break;

                case (FSC_MODE_PAL_60 << 5):
                    eVideoStandard = E_VIDEOSTANDARD_PAL_60;
                    break;
                #endif

                default:
                    eVideoStandard = E_VIDEOSTANDARD_NOTSTANDARD;
                    break;
            }
        }
        else
        {
            if (u16Status & VD_VSYNC_50HZ)
            {
                if ((u16Status & VD_FSC_TYPE) ==  VD_FSC_4285)
                {
                    HAL_AVD_ADC_SetGMC(0x06);
                    // if (u16Status & VD_VSYNC_50HZ) // SECAM must 50 Hz
                    {
                        eVideoStandard = E_VIDEOSTANDARD_SECAM;
                    }
                }
                else
                {
                    HAL_AVD_ADC_SetGMC(0x06);
                    eVideoStandard = E_VIDEOSTANDARD_PAL_BGHI;
                }
            }
            else
            {
                HAL_AVD_ADC_SetGMC(0x07);
                eVideoStandard = E_VIDEOSTANDARD_NTSC_M;
            }
        }
    }

    _eLastStandard=eVideoStandard;

    AVD_UNLOCK();
    return eVideoStandard;
}

#if 0 // BY 20090408 unused function
////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_VD_DetectStandardAutomatically()
/// @brief \b Function \b Description: To set VD detection mode with auto, and
///                                    get video standard of input signal within 200ms.
/// @param <IN>        \b None     :
///
///
/// @param <OUT>       \b None     :
/// @param <RET>       \b AVD_VIDEOSTANDARD_TYPE: the detected video standard.
/// @param <GLOBAL>    \b None     :
////////////////////////////////////////////////////////////////////////////////
AVD_VIDEOSTANDARD_TYPE MDrv_VD_DetectStandardAutomatically(void)
{
    MDrv_VD_StartAutoStandardDetection();
    return MDrv_VD_GetStandardDetection();
}
#endif
#if 0 // BY 20090403 unused function
////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_VD_EnableAutoGainControl()
/// @brief \b Function \b Description: To enable/disable auto gain control.
/// @param <IN>        \b bEnable  : TRUE:enable, FALSE:disable
///
///
/// @param <OUT>       \b None     :
/// @param <RET>       \b None     :
/// @param <GLOBAL>    \b None     :
////////////////////////////////////////////////////////////////////////////////
void MDrv_VD_EnableAutoGainControl(MS_BOOL bEnable) // TODO implement real function
{
    bEnable = bEnable;
    VDDBG(printf("MDrv_VD_EnableAutoGainControl(%s)\n", (bEnable) ? "ENABLE" : "DISABLE")); // <-<<<
}
#endif

void MDrv_AVD_SetFreerunPLL(AVD_VideoFreq eVideoFreq)
{
    AVD_LOCK();
    if( eVideoFreq == E_VIDEO_FQ_60Hz )
    {
        HAL_AVD_COMB_SetHtotal((MS_U16)0x38E);
    }
    else if( eVideoFreq == E_VIDEO_FQ_50Hz )
    {
        HAL_AVD_COMB_SetHtotal((MS_U16)0x46F);
    }
    else if( eVideoFreq == E_VIDEO_FQ_NOSIGNAL )
    {
        //Do nothing
    }else
    {
        MS_ASSERT(FALSE);
     }
    AVD_UNLOCK();
}

void MDrv_AVD_SetFreerunFreq(AVD_FreeRunFreq eFreerunfreq)
{
    AVD_LOCK();
    if (eFreerunfreq >= E_FREERUN_FQ_MAX)
        MS_ASSERT(FALSE);

    switch(eFreerunfreq)
    {
        case E_FREERUN_FQ_AUTO:
            HAL_AVD_AFEC_SetVtotal(0x00);
            break;

        case E_FREERUN_FQ_50Hz:
            HAL_AVD_AFEC_SetVtotal(0x01);
            break;

        case E_FREERUN_FQ_60Hz:
            HAL_AVD_AFEC_SetVtotal(0x03);
            break;

        default:
            break;
    }
    AVD_UNLOCK();
}

void MDrv_AVD_SetRegFromDSP(void)
{
    MS_U8  u8Ctl, u8Value;
    MS_U16 u16Htotal, u16CurrentHStart;
    MS_U8  u8update;
    AVD_LOCK();

    HAL_AVD_AFEC_SetRegFromDSP();
    #if(AVD_PATCH_CHANGECLK_442 == 1)
    if(((_eVDInputSource >= E_INPUT_SOURCE_CVBS1) && (_eVDInputSource <= E_INPUT_SOURCE_SVIDEO2))
        || ((_eVDInputSource == E_INPUT_SOURCE_ATV) && (g_VD_InitData.eDemodType != DEMODE_MSTAR_VIF)))
    {
        MDrv_AVD_ShifClk_Monitor();
    }
    #endif

    {
        u16Htotal = HAL_AVD_AFEC_GetHTotal(); // SPL_NSPL, H total

        switch( _eVideoSystem ) // 2006.06.17 Michael, need to check SRC1 if we use MST6xxx
        {
            case E_VIDEOSTANDARD_NTSC_M:     // SIG_NTSC: 910
                 u16CurrentHStart = 910;
                 break;

            case E_VIDEOSTANDARD_PAL_BGHI:   // SIG_PAL: 1135
                 u16CurrentHStart = 1135;
                 break;

            case E_VIDEOSTANDARD_NTSC_44:    // SIG_NTSC_443: 1127
            case E_VIDEOSTANDARD_PAL_60:
                 u16CurrentHStart = 1127;
                 break;

            case E_VIDEOSTANDARD_PAL_M:      // SIG_PAL_M: 909
                 u16CurrentHStart = 909;
                 break;

            case E_VIDEOSTANDARD_PAL_N:      // SIG_PAL_NC: 917
                 u16CurrentHStart = 917;
                 break;

            case E_VIDEOSTANDARD_SECAM:      // SIG_SECAM: 1097
                 u16CurrentHStart = 1097;
                 break;

            default:
                 // ASSERT
                 u16CurrentHStart = 1135;
                 break;
        }

        _u16DataH[2] = _u16DataH[1];
        _u16DataH[1] = _u16DataH[0];
        _u16DataH[0] = u16Htotal;

        if( (_u16DataH[2] == _u16DataH[1]) && (_u16DataH[1] == _u16DataH[0]) )
        {
            if( _u8HtotalDebounce > 3 )
            {
                _u16LatchH = _u16DataH[0];
            }
            else
            {
                _u8HtotalDebounce++;
            }
        }
        else
        {
            _u8HtotalDebounce = 0;
        }

        u16Htotal = _u16LatchH;

        switch( _eVideoSystem )
        {
            case E_VIDEOSTANDARD_PAL_BGHI:  // SIG_PAL
            case E_VIDEOSTANDARD_NTSC_44:   // SIG_NTSC_443
                    u8Value = 3;
                    break;

            case E_VIDEOSTANDARD_PAL_M:     // SIG_PAL_M
            case E_VIDEOSTANDARD_PAL_N:     // SIG_PAL_NC
                    u8Value = 1;
                    break;

            default:                        // NTSC
                    u8Value = 2;
                    break;
        }

        if (g_VD_InitData.u32VDPatchFlag & AVD_PATCH_NTSC_50)
        {
            if ( ((HAL_AVD_AFEC_GetStatus() & (VD_FSC_TYPE|VD_VSYNC_50HZ|VD_COLOR_LOCKED))
                == (VD_FSC_3579|VD_VSYNC_50HZ|VD_COLOR_LOCKED)) ) // not NTSC-50
                    u8Value = 2;
        }

        u8Ctl = ( u16Htotal - u8Value ) % 4;
        u8update =  1;
        if( u8Ctl == 3 )
        {
            u16Htotal = u16Htotal + 1;
        }
        else if( u8Ctl == 2 )
        {
            if (g_VD_InitData.u32VDPatchFlag & AVD_PATCH_NTSC_50)
            {
                if ( ((HAL_AVD_AFEC_GetStatus() & (VD_FSC_TYPE|VD_VSYNC_50HZ|VD_COLOR_LOCKED))
                    == (VD_FSC_3579|VD_VSYNC_50HZ|VD_COLOR_LOCKED)) ) // not NTSC-50
                    u16Htotal = u16Htotal + 2;
            }
            else
            {
                u8update = 0;
            }
        }
        else
        {
            u16Htotal = u16Htotal - u8Ctl;
        }

        if (g_VD_InitData.u32VDPatchFlag & AVD_PATCH_NTSC_50)
        {
            if ( ((HAL_AVD_AFEC_GetStatus() & (VD_FSC_TYPE|VD_VSYNC_50HZ|VD_COLOR_LOCKED))
                == (VD_FSC_3579|VD_VSYNC_50HZ|VD_COLOR_LOCKED)) ) // not NTSC-50
                u16CurrentHStart = 918;
        }

        if( u8update )
        {
            // HAL_AVD_COMB_SetHtotal( u16Htotal );
            HAL_AVD_COMB_SetHtotal(u16CurrentHStart);
        }

        if (g_VD_InitData.u32VDPatchFlag & AVD_PATCH_FINE_TUNE_FH_DOT)
        {
            #define FINE_TUNE_FH_DOT_MAX 20
            static MS_U8 u8FhDotDebouncer=0;
            u8Ctl = HAL_AVD_AFEC_GetNoiseMag(); // get VD noise magnitude

            if((((_eVDInputSource == E_INPUT_SOURCE_ATV) ||
                 (_eVDInputSource == E_INPUT_SOURCE_CVBS1) ||
                 (_eVDInputSource == E_INPUT_SOURCE_CVBS2) ||
                 ( _eVDInputSource == E_INPUT_SOURCE_CVBS3)) ||
                ((_eVDInputSource == E_INPUT_SOURCE_SCART1) ||
                 ( _eVDInputSource == E_INPUT_SOURCE_SCART2))) &&
               ((E_VIDEOSTANDARD_NTSC_M ==_eVideoSystem)||(E_VIDEOSTANDARD_PAL_BGHI==_eVideoSystem)))
            {
                if((abs(_u16LatchH-u16CurrentHStart)>=2)&&(u8Ctl<=((_eVDInputSource == E_INPUT_SOURCE_ATV) ? 2: 1)))
                {
                    if (u8FhDotDebouncer<FINE_TUNE_FH_DOT_MAX)
                    {
                        u8FhDotDebouncer++;
                    }
                }
                else if (u8FhDotDebouncer)
                {
                    u8FhDotDebouncer--;
                }
            }
            else
            {
                u8FhDotDebouncer=0;
            }

            if (u8FhDotDebouncer>=FINE_TUNE_FH_DOT_MAX)
            {
                u32VDPatchFlagStatus |= AVD_PATCH_FINE_TUNE_FH_DOT;
                HAL_AVD_COMB_SetNonStandardHtotal(TRUE);
            }
            else if (!u8FhDotDebouncer)
            {
                u32VDPatchFlagStatus &= ~AVD_PATCH_FINE_TUNE_FH_DOT;
                HAL_AVD_COMB_SetNonStandardHtotal(FALSE);
            }
            #if AVD_DRV_DEBUG
            if (_u8AVDDbgLevel == AVD_DBGLV_DEBUG)
            {
                if (u32VDPatchFlagStatus & AVD_PATCH_FINE_TUNE_FH_DOT)
                    printf("_u16LatchH,u16CurrentHStart,u8FhDotDebouncer= %d %d % d\n",_u16LatchH,u16CurrentHStart,u8FhDotDebouncer);
            }
            #endif
        }
    }


    // for Color Hysteresis
    if (_eVDInputSource == E_INPUT_SOURCE_ATV)
    {
        if ( (HAL_AVD_AFEC_GetStatus() & (VD_HSYNC_LOCKED | VD_STATUS_RDY | VD_COLOR_LOCKED))
            == (VD_HSYNC_LOCKED | VD_STATUS_RDY) )
        {
            HAL_AVD_AFEC_SetColorKillLevel(g_VD_InitData.u8ColorKillHighBound);
        }
        else
        {
            HAL_AVD_AFEC_SetColorKillLevel(g_VD_InitData.u8ColorKillLowBound);
        }
    }
    else
    {
        HAL_AVD_AFEC_SetColorKillLevel(g_VD_InitData.u8ColorKillLowBound);
    }

    // co-channel
    {
        if (HAL_AVD_AFEC_GetCoChannelOn())
        {
            HAL_AVD_VBI_SetTTSigDetSel(TRUE);
        }
        else
        {
            HAL_AVD_VBI_SetTTSigDetSel(FALSE);
        }
    }

    // For maintain the Hsync reliability and seneitivity
    {
        static MS_U8 u8DebounceCnt=0;

        if (!(HAL_AVD_AFEC_GetStatus() & (VD_HSYNC_LOCKED|VD_SYNC_LOCKED)))
        {
            if (u8DebounceCnt < 15)
            {
                u8DebounceCnt++;
            }
        }
        else
        {
            if (u8DebounceCnt > 0)
            {
                u8DebounceCnt--;
            }
        }

        if ((u8DebounceCnt >= 10) || _u8AutoTuningIsProgress)
        {
            HAL_AVD_AFEC_EnableBottomAverage(FALSE); // more sensitivity
        }
        else if (u8DebounceCnt <= 5)
        {
            HAL_AVD_AFEC_EnableBottomAverage(TRUE); // more reliability
        }
    }

    #if 0 // BY 20090403 Metis,Martina,T3,U3 fixed by hardware, removed
    {
        MS_U16 u16Status;
        u16Status=HAL_AVD_AFEC_GetStatus();
        if ( IS_BITS_SET(u16Status, VD_HSYNC_LOCKED|VD_SYNC_LOCKED|VD_STATUS_RDY) )
        {
            if ((u16Status&VD_FSC_TYPE)==VD_FSC_4285) //SECAM
            {
                RIU_WriteByteMask( BK_COMB_4A, 0x00, 0x01 ); // COMB_4A=0x56, James.Lu, 20080730
            }
        }
    }
    #endif

    {
        MS_U16 u16StandardVtotal;

        if (HAL_AVD_AFEC_GetStatus() & VD_VSYNC_50HZ)
        {
            u16StandardVtotal = 625;
        }
        else
        {
            u16StandardVtotal = 525;
        }

        // Patch for non-standard V freq issue (3D COMB) should use 2D comb
        if (abs(u16StandardVtotal - HAL_AVD_AFEC_GetVTotal()) >= 2)
        {
            HAL_AVD_COMB_Set3dComb(FALSE);
        }
        else
        {
            if(b2d3dautoflag==1)
            {
               HAL_AVD_COMB_Set3dComb(TRUE);
            }
            else
            {
               HAL_AVD_COMB_Set3dComb(FALSE);
            }

            #ifdef AVD_COMB_3D_MID // BY 20090717 enable MID mode after enable 3D comb, if the sequence is wrong, there will be garbage
            if ((_eVDInputSource != E_INPUT_SOURCE_ATV) && (_eVDInputSource != E_INPUT_SOURCE_SVIDEO1) && (_eVDInputSource != E_INPUT_SOURCE_SVIDEO2))
            {
                if (( _eVideoSystem == E_VIDEOSTANDARD_PAL_BGHI ) || ( _eVideoSystem == E_VIDEOSTANDARD_NTSC_M ))
                {
                    if (!(u32VDPatchFlagStatus & AVD_PATCH_FINE_TUNE_FH_DOT))
                    {
                        HAL_AVD_COMB_Set3dCombMid(ENABLE);
                    }
                }
            }
            #endif
        }

        // patch for color stripe function abnormal at non-standard v, Vestel TG45 issue
        if (abs(u16StandardVtotal - HAL_AVD_AFEC_GetVTotal()) >= 9)
        {
            HAL_AVD_AFEC_SetColorStripe(0x31);
        }
        else
        {
            HAL_AVD_AFEC_SetColorStripe(0x40);
        }
    }

    // Fix Comb bug
    {
        if (HAL_AVD_AFEC_GetBurstOn())
        {
            HAL_AVD_COMB_SetHsyncTolerance(0x00); // Burst On
        }
        else
        {
            HAL_AVD_COMB_SetHsyncTolerance(0x20); // No Burst (for hsync tolerance)
        }
    }

#if 0
    if (g_VD_InitData.u32VDPatchFlag & AVD_PATCH_FINE_TUNE_FSC_SHIFT)
    {
        #define FINE_TUNE_FSC_SHIFT_CNT_STEP 3
        #define FINE_TUNE_FSC_SHIFT_CNT_MAX FINE_TUNE_FSC_SHIFT_CNT_STEP*7
        #define FINE_TUNE_FSC_SHIFT_CNT_THR FINE_TUNE_FSC_SHIFT_CNT_STEP*3
        static MS_U8 u8FscShiftDebounceCnt=0;

        if ((E_VIDEOSTANDARD_NTSC_M==_eVideoSystem)||(E_VIDEOSTANDARD_PAL_BGHI==_eVideoSystem) ||
            (E_VIDEOSTANDARD_PAL_M==_eVideoSystem) || (E_VIDEOSTANDARD_PAL_N==_eVideoSystem))
        {
            if ( !HAL_AVD_COMB_Get3dCombTimingCheck()) // got comb 3D unlocked
            {
                if (u8FscShiftDebounceCnt<FINE_TUNE_FSC_SHIFT_CNT_MAX)
                {
                    u8FscShiftDebounceCnt+=FINE_TUNE_FSC_SHIFT_CNT_STEP;
                }
            }
            else
            {
                if (u8FscShiftDebounceCnt)
                {
                    u8FscShiftDebounceCnt--;
                }
            }
        }
        else
        {
            u8FscShiftDebounceCnt = 0;
        }
        if (u8FscShiftDebounceCnt>=FINE_TUNE_FSC_SHIFT_CNT_THR)
        {
            u32VDPatchFlagStatus |= AVD_PATCH_FINE_TUNE_FSC_SHIFT;
            HAL_AVD_COMB_SetNonStandardFSC(TRUE,TRUE);
        }
        else if (!u8FscShiftDebounceCnt)
        {
            u32VDPatchFlagStatus &= ~AVD_PATCH_FINE_TUNE_FSC_SHIFT;
            HAL_AVD_COMB_SetNonStandardFSC((E_VIDEOSTANDARD_PAL_BGHI==_eVideoSystem), FALSE);
        }
    }
#endif

    {  // burst
        MS_BOOL bBurstOn;
        static MS_BOOL bPrevBurstOn=FALSE;

        bBurstOn = HAL_AVD_AFEC_GetBurstOn();
        if ( bBurstOn  )
        {
            if ( !bPrevBurstOn )
            {
                _MDrv_AVD_3DCombSpeedup();
            }
        }
        #ifdef LOCK3DSPEEDUP_PERFORMANCE_1
        else
        {
            if ( !_u8Comb10Bit3Flag )
            {
                HAL_AVD_COMB_Set3dCombSpeed(0x14, 0x10, 0x88);
            }
        }
        #endif
        bPrevBurstOn=bBurstOn;

        if ( _u8Comb10Bit3Flag )
        {
            if ((_u8Comb10Bit3Flag&(BIT(0))) && (MsOS_GetSystemTime() -_u32VideoSystemTimer >3000))
            {
                //MDrv_WriteByteMask( BK_COMB_10, 0, BIT3 );
                HAL_AVD_COMB_Set3dCombSpeed(0x50, 0x20, 0x88);
                #if ADJUST_CHANNEL_CHANGE_HSYNC
                HAL_AVD_AFEC_SetSwingLimit(g_VD_InitData.u8SwingThreshold);
                HAL_AVD_AFEC_EnableBottomAverage(ENABLE); // more reliability
                #endif
                _u8Comb10Bit3Flag&=(~(BIT(0)));
                #if AVD_DRV_DEBUG
                if (_u8AVDDbgLevel == AVD_DBGLV_DEBUG)
                {
                    printf("MDrv_AVD_3DCombSpeedup Disable\n");
                }
                #endif
            }
            if ((_u8Comb10Bit3Flag&(BIT(1))) && (MsOS_GetSystemTime() -_u32VideoSystemTimer >2000))
            {
                if (!(_u8AfecD4Factory & BIT(4)))
                {
                    HAL_AVD_AFEC_EnableVBIDPLSpeedup(DISABLE);
                }
                _u8Comb10Bit3Flag&=(~(BIT(1)));
            }
            if ((_u8Comb10Bit3Flag&(BIT(2))) && (MsOS_GetSystemTime() -_u32VideoSystemTimer >400))
            {
                HAL_AVD_SetReg(BK_AFEC_A0, HAL_AVD_GetReg(BK_AFEC_A0)&(~BIT(7)));
                _u8Comb10Bit3Flag&=(~(BIT(2)));
            }
        }
    }

    #if AVD_DRV_DEBUG
    if (_u8AVDDbgLevel == AVD_DBGLV_DEBUG)
    {
        if (u32PreVDPatchFlagStatus != u32VDPatchFlagStatus)
        {
            printf("u32VDPatchFlagStatus %x\n",(unsigned int)u32VDPatchFlagStatus);
            u32PreVDPatchFlagStatus=u32VDPatchFlagStatus;
        }
    }
    #endif

    if (g_VD_InitData.u32VDPatchFlag & AVD_PATCH_NON_STANDARD_VTOTAL)
    {
        _MDrv_AVD_SyncRangeHandler();
    }

    if (g_VD_InitData.u32VDPatchFlag & AVD_PATCH_FINE_TUNE_STILL_IMAGE)
    {
        _MDrv_AVD_COMB_StillImage();
    }
    AVD_UNLOCK();
}

MS_U32 MDrv_AVD_GetFlag(void)
{
    return g_VD_InitData.u32VDPatchFlag;
}

void MDrv_AVD_SetFlag(MS_U32  u32VDPatchFlag)
{
    AVD_LOCK();
    g_VD_InitData.u32VDPatchFlag=u32VDPatchFlag;
    HAL_AVD_AFEC_SetPatchFlag(g_VD_InitData.u32VDPatchFlag);
    AVD_UNLOCK();
}

void MDrv_AVD_SetHsyncDetectionForTuning(MS_BOOL bEnable)
{
    AVD_LOCK();
    #if AVD_DRV_DEBUG
    if (_u8AVDDbgLevel == AVD_DBGLV_DEBUG)
    {
        printf("MDrv_AVD_SetHsyncDetectionForTuning %d\n",bEnable);
    }
    #endif

    if (bEnable)    // for Auto Scan Mode
    {
        HAL_AVD_AFEC_SetHsyncSensitivity(g_VD_InitData.eVDHsyncSensitivityTuning);
        HAL_AVD_AFEC_SetSwingLimit(0);
        HAL_AVD_AFEC_EnableBottomAverage(DISABLE); // more sensitivity
        _u8AutoTuningIsProgress = TRUE;
    }
    else            // for Normal Mode
    {
        HAL_AVD_AFEC_SetHsyncSensitivity(g_VD_InitData.eVDHsyncSensitivityNormal);
        HAL_AVD_AFEC_SetSwingLimit(g_VD_InitData.u8SwingThreshold);
        HAL_AVD_AFEC_EnableBottomAverage(ENABLE); // more reliability
        _u8AutoTuningIsProgress = FALSE;
    }
    AVD_UNLOCK();
}

void MDrv_AVD_Set3dComb(MS_BOOL bEnable)
{
    AVD_LOCK();
    // it's ok to enable/disable 3D comb for svideo input
    if(bEnable)
    {
        b2d3dautoflag=1;
    }
    else
    {
        b2d3dautoflag=0;
    }
    AVD_UNLOCK();
}

void MDrv_AVD_Set3dCombSpeed(MS_U8 u8COMB57, MS_U8 u8COMB58)
{
    AVD_LOCK();
    _u8Comb57 = u8COMB57;
    _u8Comb58 = u8COMB58;
    AVD_UNLOCK();
}

void MDrv_AVD_3DCombSpeedup(void)
{
    AVD_LOCK();
    _MDrv_AVD_3DCombSpeedup();
    AVD_UNLOCK();
}

void MDrv_AVD_SetChannelChange(void)
{
    AVD_LOCK();
    #if AVD_DRV_DEBUG
    if (_u8AVDDbgLevel == AVD_DBGLV_DEBUG)
    {
        printf("MDrv_AVD_SetChannelChange\n");
    }
    #endif
    HAL_AVD_AFEC_SetChannelChange();
    //MDrv_AVD_3DCombSpeedup();
    AVD_UNLOCK();
}

MS_U8 MDrv_AVD_GetRegValue(MS_U16 u16Addr)
{
    return HAL_AVD_GetReg( u16Addr );
}


void MDrv_AVD_SetRegValue(MS_U16 u16Addr,MS_U8 u8Value)
{
    // avoid writing other block
    if (((u16Addr & 0xFF00 ) == AFEC_REG_BASE) || \
        ((u16Addr & 0xFF00 ) == COMB_REG_BASE) || \
        ((u16Addr & 0xFF00 ) == VBI_REG_BASE) || \
        ((u16Addr & 0xFF00 ) == MIIC_REG_BASE) ||\
        ((u16Addr & 0xFF00 ) == SCM_REG_BASE))
    {
        HAL_AVD_SetReg( u16Addr ,u8Value);
    }

}

MS_BOOL MDrv_AVD_McuReset(void)
{
    AVD_LOCK();
    #if AVD_DRV_DEBUG
    if (_u8AVDDbgLevel == AVD_DBGLV_DEBUG)
    {
        printf("MDrv_AVD_McuReset\n");
    }
    #endif
    HAL_AVD_AFEC_McuReset(); // assume clock source is ready, it's required to switch source before reset after T4
    HAL_AVD_AFEC_SetPatchFlag(g_VD_InitData.u32VDPatchFlag);

    // Forced to PAL mode
    _u16CurVDStatus = 0;

    AVD_UNLOCK();

    return TRUE;
}

void MDrv_AVD_LoadDSP(MS_U8 *pu8VD_DSP, MS_U32 len)
{
    AVD_LOCK();
    HAL_AVD_VDMCU_LoadDSP(pu8VD_DSP,len);
    AVD_UNLOCK();
}

void MDrv_AVD_SetPQFineTune(void)
{
    AVD_LOCK();
    HAL_AVD_SetPQFineTune();
    AVD_UNLOCK();
}

void MDrv_AVD_GetCaptureWindow(void *stCapWin, AVD_VideoStandardType eVideoStandardType)
{
    AVD_LOCK();
    #if AVD_DRV_DEBUG
    if (_u8AVDDbgLevel == AVD_DBGLV_DEBUG)
    {
        printf("MDrv_AVD_GetCaptureWindow %ld %d\n",(g_VD_InitData.u32VDPatchFlag & AVD_PATCH_HTOTAL_MASK)>>8, _u16HtotalPAL[(g_VD_InitData.u32VDPatchFlag & AVD_PATCH_HTOTAL_MASK)>>8]);
    }
    #endif
    switch (eVideoStandardType)
    {
        case E_VIDEOSTANDARD_PAL_BGHI:
                ((MS_WINDOW_TYPE *)stCapWin)->width=(((MS_U32)720*(_u16HtotalPAL[(g_VD_InitData.u32VDPatchFlag & AVD_PATCH_HTOTAL_MASK)>>8])+432)/864);
            break;
        case E_VIDEOSTANDARD_NTSC_M:
                ((MS_WINDOW_TYPE *)stCapWin)->width=(((MS_U32)720*(_u16HtotalNTSC[(g_VD_InitData.u32VDPatchFlag & AVD_PATCH_HTOTAL_MASK)>>8])+429)/858);
            break;
        case E_VIDEOSTANDARD_SECAM:
                ((MS_WINDOW_TYPE *)stCapWin)->width=(((MS_U32)720*(_u16HtotalSECAM[(g_VD_InitData.u32VDPatchFlag & AVD_PATCH_HTOTAL_MASK)>>8])+432)/864);
            break;
        case E_VIDEOSTANDARD_NTSC_44:
                ((MS_WINDOW_TYPE *)stCapWin)->width=(((MS_U32)720*(_u16HtotalNTSC_443[(g_VD_InitData.u32VDPatchFlag & AVD_PATCH_HTOTAL_MASK)>>8])+432)/864);
            break;
        case E_VIDEOSTANDARD_PAL_M:
                ((MS_WINDOW_TYPE *)stCapWin)->width=(((MS_U32)720*(_u16HtotalPAL_M[(g_VD_InitData.u32VDPatchFlag & AVD_PATCH_HTOTAL_MASK)>>8])+429)/858);
            break;
        case E_VIDEOSTANDARD_PAL_N:
                ((MS_WINDOW_TYPE *)stCapWin)->width=(((MS_U32)720*(_u16HtotalPAL_NC[(g_VD_InitData.u32VDPatchFlag & AVD_PATCH_HTOTAL_MASK)>>8])+429)/858);
            break;
        case E_VIDEOSTANDARD_PAL_60:
                ((MS_WINDOW_TYPE *)stCapWin)->width=(((MS_U32)720*(_u16HtotalPAL_60[(g_VD_InitData.u32VDPatchFlag & AVD_PATCH_HTOTAL_MASK)>>8])+432)/864);
            break;
        default :
                ((MS_WINDOW_TYPE *)stCapWin)->width=(((MS_U32)720*(_u16HtotalPAL[(g_VD_InitData.u32VDPatchFlag & AVD_PATCH_HTOTAL_MASK)>>8])+432)/864);
            break;
    }
    AVD_UNLOCK();
}

#if 1
void _MDrv_AVD_MBX_WaitReady(void)
{
    MS_U32 u32StartTime=OS_SYSTEM_TIME();
    while (OS_SYSTEM_TIME()-u32StartTime < 10)
    {
        if (!HAL_AVD_GetReg(BK_AFEC_C0)) break;
    }
}

MS_U8 MDrv_AVD_MBX_ReadByteByVDMbox(MS_U8 u8Addr)
{
    MS_U8 u8Value;
    AVD_LOCK();
    _MDrv_AVD_MBX_WaitReady();

    HAL_AVD_SetReg(BK_AFEC_C1, u8Addr);
    HAL_AVD_SetReg(BK_AFEC_C0, 0x02);

    _MDrv_AVD_MBX_WaitReady();
    u8Value=MDrv_AVD_GetRegValue(BK_AFEC_C2);
    AVD_UNLOCK();
    return u8Value;
}

MS_U8 MDrv_AVD_GetHsyncEdge(void)
{
    return HAL_AVD_GetHsyncEdge();
}
#endif
void MDrv_AVD_SetAFECD4Factory(MS_U8 u8Value)
{
   _u8AfecD4Factory=u8Value;
}
#if 0 // BY 20090402
////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_VD_SetFWStaus()
/// @brief \b Function \b Description: To set VD firmware status and its start address in DRAM.
/// @param <IN>        \b bFWStatus: TRUE: doesn't need to load code from flash to DRAM, already resident in DRAM
///                                  FALSE: to load VD code from flash to DRAM
/// @param <IN>        \b u32SysBinAddr: the start address of firmware code in DRAM.
///
///
/// @param <OUT>       \b None     :
/// @param <RET>       \b None     :
/// @param <GLOBAL>    \b None     :
////////////////////////////////////////////////////////////////////////////////
void MDrv_VD_SetFWStaus(MS_BOOL bFWStatus, MS_U32 u32SysBinAddr)
{

    //Basic Input Checking
    if (u32SysBinAddr==0)
        MS_ASSERT(FALSE);

    _bVD_FWStatus = bFWStatus;
    _u32SysBinAddr = u32SysBinAddr;
}
#endif
void MDrv_AVD_Set2D3DPatchOnOff(MS_BOOL bEnable)
{
    HAL_AVD_Set2D3DPatchOnOff(bEnable);
    printf("Patch %s \r\n",(bEnable)?"On" : "Off" );
}
void MDrv_AVD_BackPorchWindowPositon(MS_BOOL bEnable , MS_U8 u8Value)
{
    HAL_AVD_AFEC_BackPorchWindowPosition(bEnable , u8Value);

}

void MDrv_AVD_SetFactoryPara(AVD_Factory_Para FactoryPara,MS_U8 u8Value)
{
    MS_U8 u8tmp;

    switch(FactoryPara)
    {
       case E_FACTORY_PARA_AFEC_D4:
           HAL_AVD_SetReg(BK_AFEC_D4,u8Value);
           break;
       case E_FACTORY_PARA_AFEC_D8:
           HAL_AVD_SetReg(BK_AFEC_D8,u8Value);
           break;
       case E_FACTORY_PARA_AFEC_D5_BIT2:
           u8tmp = HAL_AVD_GetReg(BK_AFEC_D5);
           u8tmp &= (~BIT(2));
           HAL_AVD_SetReg(BK_AFEC_D5,u8tmp |(u8Value & BIT(2)));
           break;
       case E_FACTORY_PARA_AFEC_D9_BIT0:
           u8tmp = HAL_AVD_GetReg(BK_AFEC_D9);
           u8tmp &= (~BIT(0));
           HAL_AVD_SetReg(BK_AFEC_D9,u8tmp |(u8Value & BIT(0)));
           break;
       case E_FACTORY_PARA_AFEC_A0:
           HAL_AVD_SetReg(BK_AFEC_A0,u8Value);
           break;
       case E_FACTORY_PARA_AFEC_A1:
           HAL_AVD_SetReg(BK_AFEC_A1,u8Value);
           break;
       case E_FACTORY_PARA_AFEC_66_BIT67:
           u8tmp = HAL_AVD_GetReg(BK_AFEC_66);
           u8tmp &= (~(BIT(6)|BIT(7)));
           HAL_AVD_SetReg(BK_AFEC_66,u8tmp |(u8Value & (BIT(6)|BIT(7))));
           break;
       case E_FACTORY_PARA_AFEC_6E_BIT7654:
           u8tmp = HAL_AVD_GetReg(BK_AFEC_6E);
           u8tmp &= 0x0F;
           HAL_AVD_SetReg(BK_AFEC_6E,u8tmp |(u8Value & 0xF0));
           break;
       case E_FACTORY_PARA_AFEC_6E_BIT3210:
           u8tmp = HAL_AVD_GetReg(BK_AFEC_6E);
           u8tmp &= 0XF0;
           HAL_AVD_SetReg(BK_AFEC_6E,u8tmp |(u8Value & 0x0F));
           break;
       case E_FACTORY_PARA_AFEC_43:
           u8tmp = HAL_AVD_GetReg(BK_AFEC_43);
           u8tmp &= (~(BIT(5)|BIT(6)));
           HAL_AVD_SetReg(BK_AFEC_43,u8tmp |(u8Value & (BIT(5)|BIT(6))));
           u8tmp = HAL_AVD_GetReg(BK_AFEC_55);
           HAL_AVD_SetReg(BK_AFEC_55,u8tmp|BIT(2));
           MsOS_DelayTask(2);
           HAL_AVD_SetReg(BK_AFEC_55,u8tmp &(~BIT(2)));
           break;
       case E_FACTORY_PARA_AFEC_44:
           HAL_AVD_SetReg(BK_AFEC_44,u8Value);
           HAL_AVD_AFEC_AGCSetMode(AVD_AGC_DISABLE);
           break;
       case E_FACTORY_PARA_AFEC_CB:
           u8tmp = HAL_AVD_GetReg(BK_AFEC_CB);
           u8tmp &= BIT(7);
           HAL_AVD_SetReg(BK_AFEC_CB,u8tmp|(u8Value & 0x7F)); //except BIT7
           break;
       case E_FACTORY_PARA_AFEC_CF_BIT2:
           u8tmp = HAL_AVD_GetReg(BK_AFEC_CF);
           u8tmp &= (~BIT(2));
           HAL_AVD_SetReg(BK_AFEC_CF,u8tmp |(u8Value & BIT(2)));
           break;
       default:
           printf("ERROR!!! Invalid Factory Parameter!\n");
           break;
    }
}

static MS_U8 MDrv_AVD_DSPReadByte(MS_U8 u8Cmd, MS_U16 u16Addr)
{
    MS_U8 u8Dummy = 200;
    MS_U8 u8Data;

    HAL_AVD_SetReg(BK_AFEC_C1, (MS_U8)u16Addr);
    HAL_AVD_SetReg(BK_AFEC_C3, u16Addr>>8);
    HAL_AVD_SetReg(BK_AFEC_C0, u8Cmd);

    while(u8Dummy--)
    {
        if (HAL_AVD_GetReg(BK_AFEC_C0) == 0x00)
            break;
        MsOS_DelayTask(1);
    }
    u8Data =  HAL_AVD_GetReg(BK_AFEC_C2);

    return u8Data;
}

#if 0 // not used yet
static void MDrv_AVD_DSPWriteByte(MS_U8 u8Cmd, MS_U16 u16Addr, MS_U8 u8Data)
{

    MS_U8 u8Dummy = 200;

    HAL_AVD_SetReg(BK_AFEC_C1, u16Addr);
    HAL_AVD_SetReg(BK_AFEC_C3, u16Addr>>8);
    HAL_AVD_SetReg(BK_AFEC_C2, u8Data);
    HAL_AVD_SetReg(BK_AFEC_C0, u8Cmd);

    while(u8Dummy--)
    {
        if (HAL_AVD_GetReg(BK_AFEC_C0) == 0x00)
            break;
        MsOS_DelayTask(1);
    }
}
#endif

// From Sharp T2, Sharp's feature
MS_U8 MDrv_AVD_SetAutoFineGainToFixed(void)
{
#if 1
    // Keep the function but do nothing, for compatibility.
    return 0;
#else
    MS_U8 u8CoaseGain,u8LPFGain, u8temp;
    MS_U16 u16FineGain;

    AVD_LOCK();

    HAL_AVD_AFEC_AGCSetMode(AVD_AGC_ENABLE);//auto gain
    MsOS_DelayTask(400);

    //printf("0x353F:%x\r\n",HAL_AVD_GetReg(0x353F));
    //printf("0x3544:%x\r\n",HAL_AVD_GetReg(0x3544));
    //printf("0x355F:%x\r\n",HAL_AVD_GetReg(0x355F));
    //printf("0x3540:%x\r\n",HAL_AVD_GetReg(0x3540));

    u8LPFGain = ((MDrv_AVD_DSPReadByte(2, 0x81)&BIT(4))>>4); //DSP_81[4], LPF gain
    u8CoaseGain = MDrv_AVD_DSPReadByte(2, 0x80)&0x0F;        //DSP_80[3:0], coarse gain
    //Read AGC fine-gain data DSP_80[11:4]
    u16FineGain = ((MDrv_AVD_DSPReadByte(2, 0x80)&0xF0)>>4)+((MDrv_AVD_DSPReadByte(2, 0x81)&0x0F)<<4);

    //printf("u8LPFGain:%x\r\n",u8LPFGain);
    //printf("u8CoaseGain:%x\r\n",u8CoaseGain);
    //printf("u16FineGain:%x\r\n",u16FineGain);

    u8temp = HAL_AVD_GetReg(BK_AFEC_3F)& 0xF0;
    HAL_AVD_SetReg(BK_AFEC_3F, u8CoaseGain | u8temp);  //AFEC_3F[3:0], coarse gain

    HAL_AVD_SetReg(BK_AFEC_44, (u16FineGain)& 0xFF);//AFEC_44[7:0], fine gain

    u8temp = HAL_AVD_GetReg(BK_AFEC_40);
    u8temp &= (~ BIT(4));
    HAL_AVD_SetReg(BK_AFEC_40,u8temp |(u8LPFGain & BIT(2))); //AFEC_40[4]LPF gain

    //printf("0x353F:%x\r\n",HAL_AVD_GetReg(0x353F));
    //printf("0x3544:%x\r\n",HAL_AVD_GetReg(0x3544));
    //printf("0x355F:%x\r\n",HAL_AVD_GetReg(0x355F));
    //printf("0x3540:%x\r\n",HAL_AVD_GetReg(0x3540));

    MsOS_DelayTask(10);

    HAL_AVD_AFEC_AGCSetMode(AVD_AGC_DISABLE);//fixed gain

    AVD_UNLOCK();

    return (u16FineGain>>6);
#endif
}

MS_U8 MDrv_AVD_GetDSPFineGain(void)
{
    MS_U8 u8CoaseGain,u8LPFGain, u8temp;
    MS_U16 u16FineGain;

    AVD_LOCK();

    u8LPFGain = ((MDrv_AVD_DSPReadByte(2, 0x81)&BIT(4))>>4); //DSP_81[4], LPF gain
    u8CoaseGain = MDrv_AVD_DSPReadByte(2, 0x80)&0x0F;        //DSP_80[3:0], coarse gain
    //Read AGC fine-gain data DSP_80[11:4]
    u16FineGain = ((MDrv_AVD_DSPReadByte(2, 0x80)&0xF0)>>4)+((MDrv_AVD_DSPReadByte(2, 0x81)&0x0F)<<4);

    u8temp = HAL_AVD_GetReg(BK_AFEC_3F)& 0xF0;
    HAL_AVD_SetReg(BK_AFEC_3F, u8CoaseGain | u8temp);  //AFEC_3F[3:0], coarse gain

    HAL_AVD_SetReg(BK_AFEC_44, (u16FineGain)& 0xFF);//AFEC_44[7:0], fine gain

    u8temp = HAL_AVD_GetReg(BK_AFEC_40);
    u8temp &= (~ BIT(4));
    HAL_AVD_SetReg(BK_AFEC_40,u8temp |(u8LPFGain & BIT(2))); //AFEC_40[4]LPF gain

    AVD_UNLOCK();

    return (u16FineGain>>6);
}

MS_U16 MDrv_AVD_GetDSPVersion(void)
{
    return (HAL_AVD_GetReg(BK_AFEC_D0)+DSP_VER_OFFSET);
}

void MDrv_AVD_ShifClk_Monitor(void)
{
       MS_U16 u16Status;
       AVD_VideoStandardType eVideoStandard = E_VIDEOSTANDARD_PAL_BGHI;
       u16Status = HAL_AVD_AFEC_GetStatus();
       switch (u16Status & VD_FSC_TYPE)
            {
                case VD_FSC_4433:
                    eVideoStandard = E_VIDEOSTANDARD_PAL_BGHI;
                    break;

                case VD_FSC_3579:
                    eVideoStandard = E_VIDEOSTANDARD_NTSC_M;
                    break;

                case VD_FSC_3575:
                    eVideoStandard = E_VIDEOSTANDARD_PAL_M;
                    break;

                case VD_FSC_3582:
                    eVideoStandard = E_VIDEOSTANDARD_PAL_N;
                    break;
                 case VD_FSC_4285:
                    eVideoStandard = E_VIDEOSTANDARD_SECAM;
                    break;

                 default :
                    break;
            }
       HAL_AVD_ShiftClk(eVideoStandard);
}

