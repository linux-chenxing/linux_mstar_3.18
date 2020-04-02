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
/// file    Mdrv_sc_ip.c
/// @brief  Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////
#define  _MDRV_SC_IP_C_

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
// Common Definition

#ifdef MSOS_TYPE_LINUX_KERNEL
#include <linux/string.h>
#include <linux/wait.h>
#include <linux/irqreturn.h>
#else
#include <string.h>
#endif

#include "MsCommon.h"
#include "MsIRQ.h"
#include "MsOS.h"
#include "mhal_xc_chip_config.h"

// Internal Definition
#include "drvXC_IOPort.h"
#include "xc_Analog_Reg.h"
#include "xc_hwreg_utility2.h"
#include "apiXC.h"
#include "mvideo_context.h"
#include "drvXC_HDMI_Internal.h"
#include "drv_xc_calibration.h"
#include "mhal_menuload.h"
#include "drv_sc_menuload.h"
#include "drv_sc_isr.h"
#include "drv_sc_ip.h"
#include "apiXC_PCMonitor.h"
#include "drv_sc_scaling.h"

#include "mhal_sc.h"
#include "mhal_ip.h"
#include "mhal_mux.h"

//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define msg_sc_ip(x)                //x
#define FB_DBG(x)                   // x

#define ENABLE_DEBUG_CAPTURE_N_FRAMES           0
#define ENABLE_DEBUG_CLEAR_FB_BEFORE_CAPTURE    0

#define R_CHANNEL_MIN    0x00
#define G_CHANNEL_MIN    0x01
#define B_CHANNEL_MIN    0x02
#define R_CHANNEL_MAX    0x03
#define G_CHANNEL_MAX    0x04
#define B_CHANNEL_MAX    0x05

//-------------------------------------------------------------------------------------------------
//  Local Structurs
//-------------------------------------------------------------------------------------------------
typedef struct SC_MEM_YUV444_FMT_s
{
    MS_U32 Cb:10;
    MS_U32 Y:10;
    MS_U32 Cr:10;
    MS_U32 u8Dummy:2;       // [31:30] no use
} SC_MEM_YUV444_FMT_t, *pSC_MEM_YUV444_FMT_t;

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------
#ifdef MSOS_TYPE_LINUX_KERNEL
//MS_BOOL g_bDoCalibration;
#endif

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// IP MUX
//-----------------------------------------------------------------------------
void MDrv_XC_reset_ip(SCALER_WIN eWindow)
{

    if( eWindow == MAIN_WINDOW || eWindow == SC1_MAIN_WINDOW || eWindow == SC2_MAIN_WINDOW)
    {
        Hal_SC_ip_software_reset(REST_IP_F2, eWindow);
    }
    else
    {
        Hal_SC_ip_software_reset(REST_IP_F1, eWindow);
    }
}


//-------------------------------------------------------------------------------------------------
/// do the software reset for the specific window
/// @param  u8Reset             \b IN: reset IP @ref SOFTWARE_REST_TYPE_t
/// @param  eWindow             \b IN: which window we are going to set
//-------------------------------------------------------------------------------------------------
void MApi_XC_SoftwareReset(MS_U8 u8Reset, SCALER_WIN eWindow)
{
    _XC_ENTRY();
    Hal_SC_ip_software_reset( u8Reset, eWindow );
    _XC_RETURN();
}

void MDrv_XC_SoftwareReset(MS_U8 u8Reset, SCALER_WIN eWindow)
{
    Hal_SC_ip_software_reset( u8Reset, eWindow );
}

/******************************************************************************/
/// -This function will return input video source status
/// @return Video status
/// - BIT0:VSnc polarity bit(0/1 = positive/negative)
/// - BIT1:HSync polarity bit(0/1 = positive/negative)
/// - BIT2:HSync loss bit
/// - BIT3:VSync loss bit
/// - BIT4:Interlace mode
/// - BIT7:User new mode (Not found in mode table)
/******************************************************************************/
void MDrv_XC_ip_get_sync_status(XC_IP_SYNC_STATUS *sXC_Sync_Status, E_MUX_INPUTPORT enInputPort , SCALER_WIN eWindow)
{
    MS_U8 u8DetectStatus;

    /* mode detect status */
    sXC_Sync_Status->u8SyncStatus = 0x00;

    u8DetectStatus = Hal_SC_ip_get_sync_detect_status(eWindow);
    if ( u8DetectStatus & BIT(0) )
    {
        sXC_Sync_Status->u8SyncStatus |= XC_MD_VSYNC_POR_BIT;
    }
    if ( u8DetectStatus & BIT(1) )
    {
        sXC_Sync_Status->u8SyncStatus |= XC_MD_HSYNC_POR_BIT;
    }
    if ( u8DetectStatus & BIT(3) )
    {
        sXC_Sync_Status->u8SyncStatus |= XC_MD_INTERLACE_BIT;
    }

    /* HSync */
    sXC_Sync_Status->u16Hperiod = MDrv_SC_ip_get_horizontalPeriod(eWindow);
    if( (sXC_Sync_Status->u16Hperiod == MST_H_PERIOD_MASK) || (sXC_Sync_Status->u16Hperiod < 10) )
    {
        sXC_Sync_Status->u8SyncStatus |= XC_MD_HSYNC_LOSS_BIT;
    }
    msg_sc_ip(printf("Hperiod=0x%x\n", sXC_Sync_Status->u16Hperiod));

    /* VSync */
    sXC_Sync_Status->u16Vtotal = MDrv_SC_ip_get_verticaltotal(eWindow);
    if ( (sXC_Sync_Status->u16Vtotal == MST_V_TOTAL_MASK) || (sXC_Sync_Status->u16Vtotal < 200) )
    {
        sXC_Sync_Status->u8SyncStatus |= XC_MD_VSYNC_LOSS_BIT;
    }
    msg_sc_ip(printf("Vtotal=0x%x\n", sXC_Sync_Status->u16Vtotal));

    if ( IsDVIPort(enInputPort) ) // DVI
    {
        if ( MDrv_HDMI_dvi_clock_detect(enInputPort) )
            sXC_Sync_Status->u8SyncStatus |= XC_MD_SYNC_LOSS;
    }

    msg_sc_ip(printf("SyncStatus=0x%x\n", sXC_Sync_Status->u8SyncStatus));
}

/********************************************************************************/
/*                   Functions                                                  */
/********************************************************************************/

//-------------------------------------------------------------------------------------------------
/// This function generates specific timing
/// @param  timingtype      \b IN: the specific timing to generate
//-------------------------------------------------------------------------------------------------
void MApi_XC_GenSpecificTiming(XC_Internal_TimingType timingtype)
{
    _XC_ENTRY();
    Hal_SC_IPMux_Gen_SpecificTiming(timingtype);
    _XC_RETURN();
}

//-------------------------------------------------------------------------------------------------
/// This function tells whether it's ip generated timing
//-------------------------------------------------------------------------------------------------
MS_BOOL MDrv_SC_Check_IP_Gen_Timing(void)
{
    return Hal_SC_Check_IP_Gen_Timing();
}

//-------------------------------------------------------------------------------------------------
/// This function initialize IP for internal timing
/// @param  timingtype      \b IN: the specific timing to generate
//-------------------------------------------------------------------------------------------------
void MApi_XC_InitIPForInternalTiming(XC_Internal_TimingType timingtype)
{
    _XC_ENTRY();
    Hal_SC_ip_Init_for_internal_timing(timingtype);
    _XC_RETURN();
}

//-------------------------------------------------------------------------------------------------
/// This function Gets by-pass mode status:
//-------------------------------------------------------------------------------------------------
MS_BOOL MDrv_SC_Get_DE_Bypass_Mode(SCALER_WIN eWindow)
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_2F_L :
                        SUB_WINDOW      ? REG_SC_BK03_2F_L :
						#ifdef MULTI_SCALER_SUPPORTED
                        SC1_MAIN_WINDOW ? REG_SC1_BK01_2F_L :
                        SC2_MAIN_WINDOW ? REG_SC2_BK01_2F_L :
                        SC2_SUB_WINDOW  ? REG_SC2_BK03_2F_L :
						#endif
                                          REG_SC_DUMMY;
    //TRUE: HV mdoe, FALSE:DE mode
    return ((SC_R2BYTEMSK( u32Reg,  BIT(15) ))? TRUE : FALSE);
}
//-------------------------------------------------------------------------------------------------
/// This function get DE window
/// @param  psWin        \b OUT: window info to get
/// @param  eWindow      \b IN: which window(main or sub) to get
//-------------------------------------------------------------------------------------------------
void MApi_XC_GetDEWindow(MS_WINDOW_TYPE *psWin, SCALER_WIN eWindow)
{
    XC_IP_SYNC_STATUS sXC_Sync_Status;

    _XC_ENTRY();

    if(MDrv_SC_Get_DE_Bypass_Mode(eWindow) == TRUE)
    {
        printf("Cannot get DE window under HV mode!! please check your xc lib!\n");
    }

    psWin->x      = Hal_SC_ip_de_hstart_info(eWindow);
    psWin->y      = Hal_SC_ip_de_vstart_info(eWindow);
    psWin->width  = (Hal_SC_ip_de_hend_info(eWindow) - psWin->x)+1;
    psWin->height = (Hal_SC_ip_de_vend_info(eWindow) - psWin->y)+1;

    MDrv_XC_ip_get_sync_status(&sXC_Sync_Status, INPUT_PORT_DVI0, eWindow);

    if(sXC_Sync_Status.u8SyncStatus & XC_MD_INTERLACE_BIT)
    {
        if(psWin->height & 1)
        {
            psWin->height += 1;
        }
    }
    _XC_RETURN();
}

void MDrv_XC_GetDEWidthHeightInDEByPassMode(MS_U16* pu16Width, MS_U16* pu16Height, SCALER_WIN eWindow)
{
    MS_U16 bHSDEnabled = 0x00, bVSDEnabled = 0x00;
    MS_U32 u32Reg05, u32Reg09;

#if (HW_DESIGN_3D_VER >= 3)
    MS_U16 u163DType = 0;

    //save 3D type, which will be disable soon, and which could affect vsd, hsd count
    u163DType = SC_R2BYTE(REG_SC_BK02_63_L);
    SC_W2BYTE(REG_SC_BK02_63_L, 0x0000);
#endif

    u32Reg05 = eWindow == MAIN_WINDOW     ? REG_SC_BK02_05_L :
                          SUB_WINDOW      ? REG_SC_BK04_05_L :
						  #ifdef MULTI_SCALER_SUPPORTED
                          SC1_MAIN_WINDOW ? REG_SC1_BK02_05_L :
                          SC2_MAIN_WINDOW ? REG_SC2_BK02_05_L :
                          SC2_SUB_WINDOW  ? REG_SC2_BK04_05_L :
						  #endif
                                            REG_SC_DUMMY;

    u32Reg09 = eWindow == MAIN_WINDOW     ? REG_SC_BK02_09_L :
                          SUB_WINDOW      ? REG_SC_BK04_09_L :
						  #ifdef MULTI_SCALER_SUPPORTED
                          SC1_MAIN_WINDOW ? REG_SC1_BK02_09_L :
                          SC2_MAIN_WINDOW ? REG_SC2_BK02_09_L :
                          SC2_SUB_WINDOW  ? REG_SC2_BK04_09_L :
						  #endif
                                            REG_SC_DUMMY;
    if(MDrv_SC_Get_DE_Bypass_Mode(eWindow) != TRUE)
    {
        printf("Cannot get HV width and height under DE mode!! please check your xc lib!\n");
    }

    // Backup
    bHSDEnabled = SC_R2BYTEMSK(u32Reg05, BIT(15));
    bVSDEnabled = SC_R2BYTEMSK(u32Reg09, BIT(15));

    // Disable scaling for get correct DE
    MDrv_SC_Enable_PreScaling(FALSE, FALSE, eWindow);

    // To update VSD, V capture have to set full range to make sure get all line count
    MDrv_SC_SetFullRangeCapture(eWindow);

    MDrv_XC_wait_output_vsync(3, 200, MAIN_WINDOW);

    // Get real DE data after VSD / HSD
    *pu16Height = Hal_SC_get_vsd_line_count(eWindow);
    *pu16Width  = Hal_SC_get_hsd_pixel_count(eWindow);
    // Restore
    MDrv_SC_Enable_PreScaling( (MS_BOOL)bHSDEnabled, (MS_BOOL)bVSDEnabled, eWindow);

#if (HW_DESIGN_3D_VER >= 3)
    // Restore 3D Type
    SC_W2BYTE(REG_SC_BK02_63_L, u163DType);
#endif

    if(MApi_XC_PCMonitor_GetSyncStatus(eWindow) & XC_MD_INTERLACE_BIT)
    {
        *pu16Height <<= 1; //For interlace, multiply 2 for height
    }
}

//-------------------------------------------------------------------------------------------------
/// This function get DE window in DE ByPass Mode. (T12 / T13 can not support HDMI HV mode. So DE window from BK1 can not be retrieved if set to HV mode.)
/// We Bypass pre-scaling and get H/V size from BK2 MDrv_SC_get_vsd_line_count & MDrv_SC_get_hsd_pixel_count.
/// Prepare this function for using in the feature.
/// @param  psWin        \b OUT: window info to get
/// @param  eWindow      \b IN: which window(main or sub) to get
//-------------------------------------------------------------------------------------------------
void MApi_XC_GetDEWidthHeightInDEByPassMode(MS_U16* pu16Width,MS_U16* pu16Height ,SCALER_WIN eWindow)
{
    _XC_ENTRY();

    MDrv_XC_GetDEWidthHeightInDEByPassMode(pu16Width, pu16Height, eWindow);
    _XC_RETURN();
}

//-------------------------------------------------------------------------------------------------
/// Get the capture window for specific window
/// @param  capture_win      \b OUT: the window info to get
/// @param  eWindow          \b IN: which window(main or sub) to get
//-------------------------------------------------------------------------------------------------
void MApi_XC_GetCaptureWindow(MS_WINDOW_TYPE* capture_win, SCALER_WIN eWindow)
{
    _XC_ENTRY();
    Hal_SC_ip_get_capture_window(capture_win, eWindow);
    _XC_RETURN();
}

//-------------------------------------------------------------------------------------------------
/// Set the Vertical start of capture window for specific window
/// @param  u16Vstart        \b IN: the window info to get
/// @param  eWindow          \b IN: which window(main or sub) to set
//-------------------------------------------------------------------------------------------------
void MApi_XC_SetCaptureWindowVstart(MS_U16 u16Vstart , SCALER_WIN eWindow)
{
    _XC_ENTRY();
    Hal_SC_ip_set_capture_v_start(u16Vstart, eWindow);
    _XC_RETURN();
}

//-------------------------------------------------------------------------------------------------
/// Set the horizontal start of capture window for specific window
/// @param  u16Hstart        \b IN: the window info to get
/// @param  eWindow          \b IN: which window(main or sub) to set
//-------------------------------------------------------------------------------------------------
void MApi_XC_SetCaptureWindowHstart(MS_U16 u16Hstart , SCALER_WIN eWindow)
{
    _XC_ENTRY();
    Hal_SC_ip_set_capture_h_start(u16Hstart, eWindow);
    _XC_RETURN();
}

//-------------------------------------------------------------------------------------------------
/// Set the Vertical size of capture window for specific window
/// @param  u16Vsize         \b IN: the window info to get
/// @param  eWindow          \b IN: which window(main or sub) to set
//-------------------------------------------------------------------------------------------------
void MApi_XC_SetCaptureWindowVsize(MS_U16 u16Vsize , SCALER_WIN eWindow)
{
    _XC_ENTRY();
    Hal_SC_ip_set_capture_v_size(u16Vsize, eWindow);
    _XC_RETURN();

}

//-------------------------------------------------------------------------------------------------
/// Set the Horizontal size of capture window for specific window
/// @param  u16Hsize         \b IN: the window info to get
/// @param  eWindow          \b IN: which window(main or sub) to set
//-------------------------------------------------------------------------------------------------
void MApi_XC_SetCaptureWindowHsize(MS_U16 u16Hsize , SCALER_WIN eWindow)
{
    _XC_ENTRY();
    Hal_SC_ip_set_capture_h_size(u16Hsize, eWindow);
    _XC_RETURN();
}

//-------------------------------------------------------------------------------------------------
/// enable or disable the Auto Gain for specific window
/// @param  u8Enable         \b IN: enable or disable
/// @param  eWindow          \b IN: which window(main or sub) to set
//-------------------------------------------------------------------------------------------------
void MApi_XC_AutoGainEnable(MS_U8 u8Enable , SCALER_WIN eWindow)
{
    _XC_ENTRY();
    Hal_SC_ip_set_auto_gain_function(u8Enable, eWindow);
    _XC_RETURN();
}

void MApi_XC_SetFIRDownSampleDivider(MS_U8 u8Enable , MS_U16 u16OverSamplingCount , SCALER_WIN eWindow )
{
    _XC_ENTRY();
    Hal_SC_ip_set_fir_down_sample_divider(u8Enable, u16OverSamplingCount ,eWindow);
    _XC_RETURN();
}

//-------------------------------------------------------------------------------------------------
E_APIXC_ReturnValue MApi_XC_EnableIPAutoNoSignal(MS_BOOL bEnable, SCALER_WIN eWindow )
{
    _XC_ENTRY();
    gSrcInfo[eWindow].bAutoNoSignalEnabled = bEnable;
    Hal_SC_ip_set_IPAutoNoSignal(bEnable,eWindow);
    _XC_RETURN();

    return E_APIXC_RET_OK;
}
MS_BOOL MApi_XC_GetIPAutoNoSignal(SCALER_WIN eWindow )
{

    return gSrcInfo[eWindow].bAutoNoSignalEnabled;
}
//-------------------------------------------------------------------------------------------------
/// Check whether Auto gain result is ready or not
/// @param  eWindow                \b IN: which window we are going to query
/// @return @ref MS_BOOL
//-------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_IsAutoGainResultReady(SCALER_WIN eWindow)
{
    MS_BOOL bAuto_gain;
    _XC_ENTRY();
    bAuto_gain = Hal_SC_ip_is_auto_gain_result_ready(eWindow);
    _XC_RETURN();
    return bAuto_gain;// Hal_SC_ip_is_auto_gain_result_ready(eWindow);
}

//-------------------------------------------------------------------------------------------------
/// Get auto gain min value status
/// @param  eWindow                \b IN: which window we are going to query
/// @return @ref MS_BOOL
//-------------------------------------------------------------------------------------------------
MS_U8 MApi_XC_GetAutoGainMinValueStatus(SCALER_WIN eWindow)
{
    return Hal_SC_ip_auto_gain_min_value_status(eWindow);
}

//-------------------------------------------------------------------------------------------------
/// Check whether Auto gain max value result
/// @param  eWindow                \b IN: which window we are going to query
/// @return @ref MS_BOOL
//-------------------------------------------------------------------------------------------------
MS_U8 MApi_XC_GetAutoGainMaxValueStatus(SCALER_WIN eWindow)
{
    return Hal_SC_ip_auto_gain_max_value_status(eWindow);
}


//-------------------------------------------------------------------------------------------------
/// Get the average data from hardware report
/// @return @ref MS_AUTOADC_TYPE
//-------------------------------------------------------------------------------------------------
MS_AUTOADC_TYPE MApi_XC_GetAverageDataFromHWReport(void)
{
    MS_U16 u16MaxR,u16MaxG,u16MaxB;
    MS_U16 u16MinR,u16MinG,u16MinB;
    MS_AUTOADC_TYPE ptAdcData;
    MS_U8 u8AvgCnt;
    MS_U8 i;
    MS_U8 u8TimeoutCnt = 0;
    //MS_U8 u8VSyncTime; // VSync time
    //u8VSyncTime = msAPI_Scaler_CalculateVSyncTime(g_wVerticalTotal, g_wHorizontalPeriod); // get VSync time

    ptAdcData.u16CH_AVG[0] = ptAdcData.u16CH_AVG[1] = ptAdcData.u16CH_AVG[2] = 0;
    u8AvgCnt = 4;
    Hal_SC_autogain_enable(ENABLE, MAIN_WINDOW);       //auto gain enable
    msg_sc_ip( printf("check rdy bit\n") );
    //MsOS_DelayTask(u8VSyncTime * 3); // wait stable

    //Auto Gain Result Ready. #0: Result not ready.

    for(i=0; i<u8AvgCnt; i++)
    {
        for(u8TimeoutCnt = 0; u8TimeoutCnt < 10; u8TimeoutCnt++)
        {
            while(Hal_SC_autogain_status(MAIN_WINDOW) == 1);       // skip 1st v blanking
            while(Hal_SC_autogain_status(MAIN_WINDOW) == 0);       // wait until next v blanking
            // ATG ready, read min/max RGB
            u16MinR = Hal_SC_rgb_average_info(R_CHANNEL_MIN, MAIN_WINDOW);
            u16MaxR = Hal_SC_rgb_average_info(R_CHANNEL_MAX, MAIN_WINDOW);
            u16MinG = Hal_SC_rgb_average_info(G_CHANNEL_MIN, MAIN_WINDOW);
            u16MaxG = Hal_SC_rgb_average_info(G_CHANNEL_MAX, MAIN_WINDOW);
            u16MinB = Hal_SC_rgb_average_info(B_CHANNEL_MIN, MAIN_WINDOW);
            u16MaxB = Hal_SC_rgb_average_info(B_CHANNEL_MAX, MAIN_WINDOW);

            if(Hal_SC_autogain_status(MAIN_WINDOW))
            {

                ptAdcData.u16CH_AVG[0]+=(u16MinR+u16MaxR+1)/2;
                ptAdcData.u16CH_AVG[1]+=(u16MinG+u16MaxG+1)/2;
                ptAdcData.u16CH_AVG[2]+=(u16MinB+u16MaxB+1)/2;

                msg_sc_ip( printf("MAX[%04d,%04d,%04d]\n",u16MaxR,u16MaxG,u16MaxB) );
                msg_sc_ip( printf("MIN[%04d,%04d,%04d]\n",u16MinR,u16MinG,u16MinR) );
                break;
            }
            else
            {
                msg_sc_ip( printk("Avg err, again\n") );
            }
        }
    }
    ptAdcData.u16CH_AVG[0] /= u8AvgCnt;
    ptAdcData.u16CH_AVG[1] /= u8AvgCnt;
    ptAdcData.u16CH_AVG[2] /= u8AvgCnt;

    msg_sc_ip( printf("ptAdcData[%04d,%04d,%04d]\n",ptAdcData.u16CH_AVG[0],ptAdcData.u16CH_AVG[1],ptAdcData.u16CH_AVG[2]) );
    return ptAdcData;
}

static void MDrv_XC_Access_BufferData(E_XC_OUTPUTDATA_TYPE eBufType, MS_WINDOW_TYPE *pMemRect, void *pDataBuf, MS_WINDOW_TYPE *pSrcRect, SCALER_WIN eWindow, MS_BOOL bIsReadMode)
{
    void *pPoint;
    volatile MS_U8 *pMEM;
    volatile MS_U8 *pMEM_temp;

    MS_U16 u16LBOffset = 0;
    MS_U16 u16PointSize = 0;
    MS_U8 u8BytePerTwoPixel = 0;
    MS_U16 x, y;
    MS_U16 x0, x1, y0, y1;
    MS_U8 u8MemFmt = SC_R2BYTE(REG_SC_BK12_02_L) & 0x0F;
    MS_U8 u8FieldMode = SC_R2BYTE(REG_SC_BK12_19_L) & 0x07;
    MS_U8 u8FieldNum;

    MS_BOOL bHmirror;
    MS_BOOL bVmirror;

    MS_U32 u32Y_Offset, u32X_Offset;
    MS_U8 u8Discardpixels = 0;
    MS_U8 u8Offsetpixels = 0;
    MS_U8 u8pixels_per_unit = 0;

    MS_U16 xRegion, accessStart;
    MS_S8 yNext, xNext, accessNext;
    MS_U8 u8packinginterlacedshift = 0;
    MS_U8 u8YCSeparateOffset = BYTE_PER_WORD;
    MS_U8 u8FrameNum;
    MS_U32 u32JumpOffset = 0;
    MS_U32 u32Accesspixel;
	MS_U16 yCount, xCount, accessCount;
	MS_PIXEL_32BIT *pPoint32 = NULL;
    MS_PIXEL_24BIT *pPoint24 = NULL;
	int index;
    MS_U16 u16WriteModeSrcPoint;

    SC_FRAMEBUF_INFO_t FBInfo;

	#if defined(MSOS_TYPE_NOS)
    UNUSED(y1);
    UNUSED(u8FieldNum);
	#endif

	if (pDataBuf == NULL)
    {
        MS_CRITICAL_MSG(printf("Data buffer is NULL! \n"));
        return;
    }

    if ((pMemRect == NULL) || ((!bIsReadMode) && (pSrcRect == NULL)))
    {
        MS_CRITICAL_MSG(printf("Data rectangle or memory rectangle is NULL! \n"));
        return;
    }

    Hal_SC_get_framebuf_Info(&FBInfo);

    if((pMemRect->x >= gSrcInfo[eWindow].u16H_SizeAfterPreScaling) ||
       (pMemRect->width>= gSrcInfo[eWindow].u16H_SizeAfterPreScaling) ||
      ((pMemRect->x + pMemRect->width) >= gSrcInfo[eWindow].u16H_SizeAfterPreScaling) ||
       (pMemRect->y >= gSrcInfo[eWindow].u16V_SizeAfterPreScaling ) ||
       (pMemRect->height>= gSrcInfo[eWindow].u16V_SizeAfterPreScaling) ||
      ((pMemRect->y + pMemRect->height) >= gSrcInfo[eWindow].u16V_SizeAfterPreScaling))
    {
        MS_CRITICAL_MSG(printf("invalid parameter!\n"));
        return;
    }

    MsOS_ReadMemory();

    if(eWindow == MAIN_WINDOW)
    {
        bHmirror = SC_R2BYTEMSK(REG_SC_BK12_03_L, BIT(12)) >> 12;
        bVmirror = SC_R2BYTEMSK(REG_SC_BK12_03_L, BIT(13)) >> 13;
    }
    else
    {
        bHmirror = SC_R2BYTEMSK(REG_SC_BK12_43_L, BIT(12)) >> 12;
        bVmirror = SC_R2BYTEMSK(REG_SC_BK12_43_L, BIT(13)) >> 13;
    }

    u8FrameNum = FBInfo.u8FBNum;

    //F2 select 8-field(0), 10-field(1), 12-field(2), 14-field(3), 16-field(4)  mem config
    u8FieldNum = 8 + u8FieldMode*2;

#if (_FIELD_PACKING_MODE_SUPPORTED)
    if (FBInfo.bMemFormat422 == TRUE)
    {
        FB_DBG(printf("source is YUV 422 mode\n"));
        u8YCSeparateOffset = u8FrameNum * BYTE_PER_WORD;

        if( 0 == u8MemFmt)
        {
            u8BytePerTwoPixel = 4;
            u8pixels_per_unit = OFFSET_PIXEL_ALIGNMENT;

            if ( FBInfo.bLinearAddrMode )
                u32JumpOffset = 0;
            else
                u32JumpOffset = BYTE_PER_WORD * (u8FrameNum + u8FrameNum -1);

        }
        else if((4 == u8MemFmt) || (5 == u8MemFmt) || (6 == u8MemFmt))
        {
            u8BytePerTwoPixel = 5;
            u8pixels_per_unit = OFFSET_PIXEL_ALIGNMENT;

            if ( FBInfo.bLinearAddrMode )
                u32JumpOffset = 0;
            else
                u32JumpOffset = BYTE_PER_WORD * (u8FrameNum * 2 + u8FrameNum -1);
        }
        else if((8 == u8MemFmt) || (9 == u8MemFmt) || (10 == u8MemFmt) || (11 == u8MemFmt))
        {
            u8BytePerTwoPixel = 6;
            u8pixels_per_unit = OFFSET_PIXEL_ALIGNMENT;

            if ( FBInfo.bLinearAddrMode )
                u32JumpOffset = 0;
            else
                u32JumpOffset = BYTE_PER_WORD * (u8FrameNum * 3 + u8FrameNum -1);
        }
    }
    else
    {
        FB_DBG(printf("source is YUV 444 mode, bit: %u\n", FBInfo.u8BitPerPixel));
        if( 1 == u8MemFmt)
            u8BytePerTwoPixel = 6;
        else
            u8BytePerTwoPixel = FBInfo.u8BitPerPixel * 2 / 8;

        u8YCSeparateOffset = u8FrameNum * BYTE_PER_WORD;

        u8pixels_per_unit = OFFSET_PIXEL_ALIGNMENT/2;

    }
#else
    if (FBInfo.bMemFormat422 == TRUE)
    {
        FB_DBG(printf("source is YUV 422 mode\n"));
        //we will enable BK12_02[9:8] to keep same memory layout, hence, BytePerPixel is 3
        // F0     F0     F2     F2     F0   F0   F2   F2   F0     F0     F2     F2
        // Y8/C8  Y8/C8  Y8/C8  Y8/C8  LSB  YM4  LSB  YM4  Y8/C8  Y8/C8  Y8/C8  Y8/C8=>24bit Y10C10M4
        // Y8/C8  Y8/C8  Y8/C8  Y8/C8  LSB  XXX  LSB  XXX  Y8/C8  Y8/C8  Y8/C8  Y8/C8=>20bit Y10C10
        // Y8/C8  Y8/C8  Y8/C8  Y8/C8  XXX  YM4  XXX  YM4  Y8/C8  Y8/C8  Y8/C8  Y8/C8=>20bit Y8YC8M4
        // Y8/C8  Y8/C8  Y8/C8  Y8/C8  XXX  XXX  XXX  XXX  Y8/C8  Y8/C8  Y8/C8  Y8/C8=>16bit Y8C8

        if( 0 == u8MemFmt)
        {
            u8BytePerTwoPixel = 4;
            u8pixels_per_unit = OFFSET_PIXEL_ALIGNMENT /2;
            if ( FBInfo.bLinearAddrMode )
                u32JumpOffset = 0;
            else
                u32JumpOffset = BYTE_PER_WORD * 2;
        }
        else if((4 == u8MemFmt) || (5 == u8MemFmt) || (6 == u8MemFmt) || (7 == u8MemFmt))
        {
            u8BytePerTwoPixel = 5;
            u8pixels_per_unit = OFFSET_PIXEL_ALIGNMENT;
            if ( FBInfo.bLinearAddrMode )
                u32JumpOffset = BYTE_PER_WORD;
            else
                u32JumpOffset = BYTE_PER_WORD * 4;
        }
        else if((8 == u8MemFmt) || (9 == u8MemFmt) || (10 == u8MemFmt) || (11 == u8MemFmt))
        {
            u8BytePerTwoPixel = 6;
            u8pixels_per_unit = OFFSET_PIXEL_ALIGNMENT;
            if ( FBInfo.bLinearAddrMode )
                u32JumpOffset = BYTE_PER_WORD * 2;
            else
                u32JumpOffset = BYTE_PER_WORD * 6;
        }
        else
        {
            u8YCSeparateOffset = (u8FieldNum/2) * BYTE_PER_WORD;
            u8FrameNum = u8FieldNum/2;
            u8BytePerTwoPixel = 6;
            u8pixels_per_unit = OFFSET_PIXEL_ALIGNMENT;
            if ( FBInfo.bLinearAddrMode )
                u32JumpOffset = BYTE_PER_WORD * 2;
            else
                u32JumpOffset = BYTE_PER_WORD * 2 * (u8FieldNum -1); // (u8FieldNum/2 -1)*2 MIU + u8FieldNum/2 MIU (for LSB) + u8FieldNum/2 MIU (for Motion)
        }
    }
    else
    {
        FB_DBG(printf("source is YUV 444 mode, bit: %u\n", FBInfo.u8BitPerPixel));
        u8BytePerTwoPixel = FBInfo.u8BitPerPixel * 2 / 8;

        if ( FBInfo.bLinearAddrMode )
            u32JumpOffset = 0;
        else
            u32JumpOffset = BYTE_PER_WORD * 2;

        if(32 == FBInfo.u8BitPerPixel)
            u8pixels_per_unit = OFFSET_PIXEL_ALIGNMENT/2;
        else
            u8pixels_per_unit = OFFSET_PIXEL_ALIGNMENT;
    }
#endif

    if(eBufType == E_XC_OUTPUTDATA_RGB10BITS)
    {
        u16PointSize = sizeof(MS_PIXEL_32BIT);
    }
    else if(eBufType == E_XC_OUTPUTDATA_RGB8BITS)
    {
        u16PointSize = sizeof(MS_PIXEL_24BIT);
    }

    x0 = pMemRect->x;
    x1 = pMemRect->x + pMemRect->width -1;
    y0 = pMemRect->y;
    y1 = pMemRect->y + pMemRect->height -1;

    // calculate the MIU counts need to access for H line
    xRegion = x1/u8pixels_per_unit - x0/u8pixels_per_unit + 1;

    accessStart = 0;
    yNext = 1;
    xNext = u8pixels_per_unit;
    accessNext = 1;

    // for mirror mode calculation
    //
    // (x0,y0)                                   (x1, y1)
    //   -----------    mirror mode     -----------
    //   |                |       ------->     |                |
    //   -----------                           -----------
    //                   (x1, y1)                                (x0, y0)
    //                                             access from x0 to x1, from y0 to y1, decrease to next point
    if (bHmirror)
    {
        x0 = (gSrcInfo[eWindow].u16H_SizeAfterPreScaling - pMemRect->x) -1;
        x1 = gSrcInfo[eWindow].u16H_SizeAfterPreScaling - (pMemRect->x + pMemRect->width);

        xRegion = x0/u8pixels_per_unit - x1/u8pixels_per_unit + 1;
        accessStart = u8pixels_per_unit -1;
        xNext = -1 * u8pixels_per_unit;
        accessNext = -1;
    }
    if (bVmirror)
    {
        y0 = gSrcInfo[eWindow].u16V_SizeAfterPreScaling  - pMemRect->y -1;
        y1 = gSrcInfo[eWindow].u16V_SizeAfterPreScaling  - (pMemRect->y + pMemRect->height);

        yNext = -1;
    }

    FB_DBG(printf("real region: (%u, %u)-(%u, %d) \n", x0, y0, x1, y1));

    u16LBOffset = x0 % u8pixels_per_unit;

    pPoint = pDataBuf;

    //MS_U16 yCount, xCount, accessCount;
    for (y = y0, yCount = 0; yCount < pMemRect->height ; y = y + yNext, yCount+=1)
    {
        if(bIsReadMode)
            pPoint = (MS_U8*)pDataBuf + (yCount % pMemRect->height) * pMemRect->width * u16PointSize;
        else
            pPoint = (MS_U8*)pDataBuf + (yCount % pSrcRect->height) * pSrcRect->width * u16PointSize;

        pPoint32 = (MS_PIXEL_32BIT*)pPoint;
        pPoint24 = (MS_PIXEL_24BIT*)pPoint;

        // Y offset
        if (gSrcInfo[eWindow].bInterlace)
        {
            u32Y_Offset = (MS_U32)y/2 * FBInfo.u16IPMOffset * (MS_U32)u8BytePerTwoPixel/2 * u8FrameNum;

#if (_FIELD_PACKING_MODE_SUPPORTED)
            if(eWindow == MAIN_WINDOW)
                u32Y_Offset += g_XC_InitData.u32Main_FB_Start_Addr;
            else
                u32Y_Offset += g_XC_InitData.u32Sub_FB_Start_Addr;

            if (y & 0x1)
                u8packinginterlacedshift = 0;
            else
                u8packinginterlacedshift = BYTE_PER_WORD;
#else
            if(eWindow == MAIN_WINDOW)
            {
                if (y & 0x1)
                    u32Y_Offset += g_XC_InitData.u32Main_FB_Start_Addr;
                else
                    u32Y_Offset += g_XC_InitData.u32Main_FB_Start_Addr + (FBInfo.u32IPMBase1 - FBInfo.u32IPMBase0);
            }
            else
            {
                if (y & 0x1)
                    u32Y_Offset += g_XC_InitData.u32Sub_FB_Start_Addr;
                else
                    u32Y_Offset += g_XC_InitData.u32Sub_FB_Start_Addr + (FBInfo.u32IPMBase1 - FBInfo.u32IPMBase0);
            }
#endif
        }
        else
        {
            if ( FBInfo.bLinearAddrMode )
                u32Y_Offset = (MS_U32)y * FBInfo.u16IPMOffset * (MS_U32)u8BytePerTwoPixel/2;
            else
                u32Y_Offset = (MS_U32)y * FBInfo.u16IPMOffset * (MS_U32)u8BytePerTwoPixel/2 * u8FrameNum;

            if(eWindow == MAIN_WINDOW)
                u32Y_Offset += g_XC_InitData.u32Main_FB_Start_Addr;
            else
                u32Y_Offset += g_XC_InitData.u32Sub_FB_Start_Addr;
        }

        FB_DBG(printf("[1]u32Y_Offset:0x%x, u16IPMOffset:%u, y:%u, u8BytePerTwoPixel:%u, u8FBNum:%u\n",
            (int)u32Y_Offset, FBInfo.u16IPMOffset, y, u8BytePerTwoPixel, FBInfo.u8FBNum));

        for (x = x0 - u16LBOffset, xCount = 0; xCount < xRegion; x = x + xNext, xCount+=1)
        {
            // X offset
            if(FBInfo.bLinearAddrMode)
                u32X_Offset = x * (MS_U32)u8BytePerTwoPixel/2;
            else
                u32X_Offset = x * (MS_U32)u8BytePerTwoPixel/2 * u8FrameNum;

            FB_DBG(printf("[2]u32X_Offset=0x%x, align(x)=%u\n", (int)u32X_Offset, x));

            pMEM = (volatile MS_U8 *)(0 + u32Y_Offset + u32X_Offset);

            FB_DBG(printf("[3]pMEM=0x%x, IPMBase0:0x%x, x:%u, y:%u \n", (int)pMEM, (int)FBInfo.u32IPMBase0, x, y));

            pMEM_temp = (MS_U8 *)MS_PA2KSEG1((MS_U32)pMEM);

            if (FBInfo.bMemFormat422 == TRUE)
            {
                if(!bHmirror)
                {
                    //  |<----- MIU block--- ---->|<----- MIU block--- ---->|
                    // 1st  x                          x0              x1
                    //  |<--u8Offsetpixels-->|<-->|<->|<--u8Discardpixels-->|
                    //                                          2nd x
                    //  x is at start point of MIU block includes x0, then increase to next start point of MIU block

                    if(x<x0)
                        u8Offsetpixels = (MS_U8)u16LBOffset;
                    else
                        u8Offsetpixels = 0;

                    if(x+u8pixels_per_unit > x1)
                        u8Discardpixels = x+u8pixels_per_unit - x1;
                    else
                        u8Discardpixels = 0;
                }
                else
                {
                    // Mirror mode:
                    //  |<----- MIU block--- ---->|<----- MIU block--- ---->|
                    //                                  x1     1st  x     x0
                    //  |<--u8Offsetpixels-->|<-->|<--->|<-u8Discardpixels->|
                    // 2nd x
                    //  x is at start point of MIU block includes x0, then decrease to previous start point of MIU block

                    if(x<x1)
                        u8Offsetpixels = x1 - x;
                    else
                        u8Offsetpixels = 0;

                    if(x+u8pixels_per_unit > x0)
                        u8Discardpixels = x+u8pixels_per_unit - x0;
                    else
                        u8Discardpixels = 0;
                }

                index = accessStart;
                u16WriteModeSrcPoint = 0;
                for(accessCount = 0; accessCount < u8pixels_per_unit; index = index + accessNext, accessCount+=1)
                {
                    if((index < u8Offsetpixels) || (index > (u8pixels_per_unit - u8Discardpixels)))
                        continue;

                    if(!bIsReadMode)
                    {
                        if(!bHmirror)
                        {
                            u16WriteModeSrcPoint = (xCount * u8pixels_per_unit + (index - u16LBOffset)) % pSrcRect->width;
                        }
                        else
                        {
                            u16WriteModeSrcPoint = (xCount * u8pixels_per_unit + (u16LBOffset - index)) % pSrcRect->width;
                        }
                    }
#if (_FIELD_PACKING_MODE_SUPPORTED)
                    if(true)
#else
                    if(FBInfo.bYCSeparate)
#endif
                    {
                        if(index < BYTE_PER_WORD)
                        {
                            u32Accesspixel = index;
                        }
                        else
                        {
#if (_FIELD_PACKING_MODE_SUPPORTED)
                            u32Accesspixel = index + u32JumpOffset;
#else
                            u32Accesspixel = index + BYTE_PER_WORD + u32JumpOffset;
#endif
                        }
                        u32Accesspixel += u8packinginterlacedshift;

                        if(bIsReadMode)
                        {
                            // for get buffer data mode
                            if(eBufType == E_XC_OUTPUTDATA_RGB10BITS)
                            {
                                // source 422 format, output R10G10B10 mode
                                pPoint32->G_Y = pMEM_temp[u32Accesspixel] << 2;
                                if (index&0x1)
                                {
                                    pPoint32->B_Cb = pMEM_temp[u32Accesspixel-1+u8YCSeparateOffset] << 2;
                                    pPoint32->R_Cr = pMEM_temp[u32Accesspixel+u8YCSeparateOffset] << 2;
                                }
                                else {
                                    pPoint32->B_Cb = pMEM_temp[u32Accesspixel+u8YCSeparateOffset] << 2;
                                    pPoint32->R_Cr = pMEM_temp[u32Accesspixel+1+u8YCSeparateOffset] << 2;
                                }
                            }
                            else if(eBufType == E_XC_OUTPUTDATA_RGB8BITS)
                            {
                                // source 422 format, output R8G8B8 mode
                                pPoint24->G_Y = pMEM_temp[u32Accesspixel];
                                if (index&0x1)
                                {
                                    pPoint24->B_Cb = pMEM_temp[u32Accesspixel-1+u8YCSeparateOffset];
                                    pPoint24->R_Cr = pMEM_temp[u32Accesspixel+u8YCSeparateOffset];
                                }
                                else {
                                    pPoint24->B_Cb = pMEM_temp[u32Accesspixel+u8YCSeparateOffset];
                                    pPoint24->R_Cr = pMEM_temp[u32Accesspixel+1+u8YCSeparateOffset];
                                }
                            }

                        }
                        else
                        {
                            // for set buffer data mode
                            if(eBufType == E_XC_OUTPUTDATA_RGB10BITS)
                            {
                                // target 422 format, input 10G10B10 mode
                                pMEM_temp[u32Accesspixel] = (MS_U8)(pPoint32[u16WriteModeSrcPoint].G_Y >> 2);
                                if (index&0x1)
                                {
                                    pMEM_temp[u32Accesspixel-1+u8YCSeparateOffset] = (MS_U8)(pPoint32[u16WriteModeSrcPoint].B_Cb >> 2);
                                    pMEM_temp[u32Accesspixel+u8YCSeparateOffset] = (MS_U8)(pPoint32[u16WriteModeSrcPoint].R_Cr >> 2);
                                }
                                else {
                                    pMEM_temp[u32Accesspixel+u8YCSeparateOffset] = (MS_U8)(pPoint32[u16WriteModeSrcPoint].B_Cb >> 2);
                                    pMEM_temp[u32Accesspixel+1+u8YCSeparateOffset] = (MS_U8)(pPoint32[u16WriteModeSrcPoint].R_Cr >> 2);
                                }
                            }
                            else if(eBufType == E_XC_OUTPUTDATA_RGB8BITS)
                            {
                                // target 422 format, input R8G8B8 mode
                                pMEM_temp[u32Accesspixel] = pPoint24[u16WriteModeSrcPoint].G_Y;
                                if (index&0x1)
                                {
                                    pMEM_temp[u32Accesspixel-1+u8YCSeparateOffset] = pPoint24[u16WriteModeSrcPoint].B_Cb;
                                    pMEM_temp[u32Accesspixel+u8YCSeparateOffset] = pPoint24[u16WriteModeSrcPoint].R_Cr;
                                }
                                else {
                                    pMEM_temp[u32Accesspixel+u8YCSeparateOffset] = pPoint24[u16WriteModeSrcPoint].B_Cb;
                                    pMEM_temp[u32Accesspixel+1+u8YCSeparateOffset] = pPoint24[u16WriteModeSrcPoint].R_Cr;
                                }
                            }
                        }
                    }
                    else
                    {
                        if(index < BYTE_PER_WORD)
                        {
                            u32Accesspixel = index*2;
                        }
                        else
                        {
                            u32Accesspixel = index*2 + u32JumpOffset;
                        }

                        if(bIsReadMode)
                        {
                            // for get buffer data mode
                            if(eBufType == E_XC_OUTPUTDATA_RGB10BITS)
                            {
                                // source 422 format, output R10G10B10 mode
                                pPoint32->G_Y = pMEM_temp[u32Accesspixel] << 2;
                                if (index&0x1) {
                                    pPoint32->B_Cb = pMEM_temp[u32Accesspixel-1] << 2;
                                    pPoint32->R_Cr = pMEM_temp[u32Accesspixel+1] << 2;
                                }
                                else {
                                    pPoint32->B_Cb = pMEM_temp[u32Accesspixel+1] << 2;
                                    pPoint32->R_Cr= pMEM_temp[u32Accesspixel+3] << 2;
                                }
                            }
                            else if(eBufType == E_XC_OUTPUTDATA_RGB8BITS)
                            {
                                // source 422 format, output R8G8B8 mode
                                pPoint24->G_Y = pMEM_temp[u32Accesspixel];
                                if (index&0x1) {
                                    pPoint24->B_Cb = pMEM_temp[u32Accesspixel-1];
                                    pPoint24->R_Cr = pMEM_temp[u32Accesspixel+1];
                                }
                                else {
                                    pPoint24->B_Cb = pMEM_temp[u32Accesspixel+1];
                                    pPoint24->R_Cr= pMEM_temp[u32Accesspixel+3];
                                }
                            }
                        }
                        else
                        {
                            // for set buffer data mode,
                            if(eBufType == E_XC_OUTPUTDATA_RGB10BITS)
                            {
                                // target 422 format, input R10G10B10 mode
                                pMEM_temp[u32Accesspixel] = (MS_U8)(pPoint32[u16WriteModeSrcPoint].G_Y >> 2);
                                if (index&0x1) {
                                    pMEM_temp[u32Accesspixel-1] = (MS_U8)(pPoint32[u16WriteModeSrcPoint].B_Cb >> 2);
                                    pMEM_temp[u32Accesspixel+1] = (MS_U8)(pPoint32[u16WriteModeSrcPoint].R_Cr >> 2);
                                }
                                else {
                                    pMEM_temp[u32Accesspixel+1] = (MS_U8)(pPoint32[u16WriteModeSrcPoint].B_Cb >> 2);
                                    pMEM_temp[u32Accesspixel+3] = (MS_U8)(pPoint32[u16WriteModeSrcPoint].R_Cr>> 2);
                                }
                            }
                            else if(eBufType == E_XC_OUTPUTDATA_RGB8BITS)
                            {
                                // target 422 format, inout R8G8B8 mode
                                pMEM_temp[u32Accesspixel] = pPoint24[u16WriteModeSrcPoint].G_Y;
                                if (index&0x1) {
                                    pMEM_temp[u32Accesspixel-1] = pPoint24[u16WriteModeSrcPoint].B_Cb;
                                    pMEM_temp[u32Accesspixel+1] = pPoint24[u16WriteModeSrcPoint].R_Cr;
                                }
                                else {
                                    pMEM_temp[u32Accesspixel+1] = pPoint24[u16WriteModeSrcPoint].B_Cb;
                                    pMEM_temp[u32Accesspixel+3] = pPoint24[u16WriteModeSrcPoint].R_Cr;
                                }
                            }
                        }
                    }

                    if(bIsReadMode)
                    {
                        if(eBufType == E_XC_OUTPUTDATA_RGB10BITS)
                        {
                            FB_DBG(printf("==> index: %d, R: %d, G:%d, B: %d \n", index, pPoint32->R_Cr, pPoint32->G_Y, pPoint32->B_Cb));
                            pPoint32++;
                        }
                        else if(eBufType == E_XC_OUTPUTDATA_RGB8BITS)
                        {
                            FB_DBG(printf("==> index: %d, R: %d, G:%d, B: %d \n", index, pPoint24->R_Cr, pPoint24->G_Y, pPoint24->B_Cb));
                            pPoint24++;
                        }
                    }

                }

            }
            else //444 case
            {
                if(!bHmirror)
                {
                    //  |<----- MIU block--- ---->|<----- MIU block--- ---->|
                    // 1st  x                          x0              x1
                    //  |<--u8Offsetpixels-->|<-->|<->|<--u8Discardpixels-->|
                    //                                          2nd x
                    //  x is at start point of MIU block includes x0, then increase to next start point of MIU block

                    if(x<x0)
                        u8Offsetpixels = (MS_U8)u16LBOffset;
                    else
                        u8Offsetpixels = 0;

                    if(x+u8pixels_per_unit > x1)
                        u8Discardpixels = x+u8pixels_per_unit - x1;
                    else
                        u8Discardpixels = 0;
                }
                else
                {
                    // Mirror mode:
                    //  |<----- MIU block--- ---->|<----- MIU block--- ---->|
                    //                                  x1     1st  x     x0
                    //  |<--u8Offsetpixels-->|<-->|<--->|<-u8Discardpixels->|
                    // 2nd x
                    //  x is at start point of MIU block includes x0, then decrease to previous start point of MIU block

                    if(x<x1)
                        u8Offsetpixels = x1 - x;
                    else
                        u8Offsetpixels = 0;

                    if(x+u8pixels_per_unit > x0)
                        u8Discardpixels = x+u8pixels_per_unit - x0;
                    else
                        u8Discardpixels = 0;
                }

                index = accessStart;
                u16WriteModeSrcPoint = 0;
                for(accessCount = 0; accessCount < u8pixels_per_unit; index = index + accessNext, accessCount+=1)
                {
                    if((index < u8Offsetpixels) || (index > (u8pixels_per_unit - u8Discardpixels)))
                        continue;

                    if(!bIsReadMode)
                    {
                        if(!bHmirror)
                        {
                            u16WriteModeSrcPoint = (xCount * u8pixels_per_unit + (index - u16LBOffset)) % pSrcRect->width;
                        }
                        else
                        {
                            u16WriteModeSrcPoint = (xCount * u8pixels_per_unit + (u16LBOffset - index)) % pSrcRect->width;
                        }
                    }
#if (_FIELD_PACKING_MODE_SUPPORTED)
                    u32Accesspixel = index;
#else
                    u32Accesspixel = index*u8BytePerTwoPixel/2;
#endif

#if (_FIELD_PACKING_MODE_SUPPORTED)
                    if(false)
#else
                    if(32 == FBInfo.u8BitPerPixel) //10bits
#endif
                    {
                        if(u32Accesspixel>= 2*BYTE_PER_WORD)
                            u32Accesspixel += u32JumpOffset;

                        if(bIsReadMode)
                        {
                            // for get buffer data mode
                            if(eBufType == E_XC_OUTPUTDATA_RGB10BITS)
                            {
                                // source 444 10bits format, output R10G10B10 mode
                                pPoint32->B_Cb = ( pMEM_temp[u32Accesspixel] ) | ( (pMEM_temp[u32Accesspixel + 1] & 0x03 ) << 8 );
                                pPoint32->G_Y = ( pMEM_temp[u32Accesspixel+ 1] >> 2 ) | ( (pMEM_temp[u32Accesspixel+ 2] & 0x0F ) << 6 );
                                pPoint32->R_Cr = ( pMEM_temp[u32Accesspixel+ 2] >> 4 ) | ( (pMEM_temp[u32Accesspixel+ 3] & 0x3F ) << 4 );
                            }
                            else if(eBufType == E_XC_OUTPUTDATA_RGB8BITS)
                            {
                                // source 444 10bits format, output R8G8B8 mode
                                pPoint24->B_Cb = (( pMEM_temp[u32Accesspixel] ) | ( (pMEM_temp[u32Accesspixel + 1] & 0x03 ) << 8 )) >> 2;
                                pPoint24->G_Y = (( pMEM_temp[u32Accesspixel+ 1] >> 2 ) | ( (pMEM_temp[u32Accesspixel+ 2] & 0x0F ) << 6 )) >> 2;
                                pPoint24->R_Cr = (( pMEM_temp[u32Accesspixel+ 2] >> 4 ) | ( (pMEM_temp[u32Accesspixel+ 3] & 0x3F ) << 4 )) >> 2;
                            }
                        }
                        else
                        {
                            // for set buffer data mode
                            if(eBufType == E_XC_OUTPUTDATA_RGB10BITS)
                            {
                                // target 444 10bits format, input R10G10B10 mode
                                pMEM_temp[u32Accesspixel] = (MS_U8)(pPoint32[u16WriteModeSrcPoint].B_Cb & 0xFF);
                                pMEM_temp[u32Accesspixel+1] = (MS_U8)((pPoint32[u16WriteModeSrcPoint].G_Y & 0x3F) << 2) |  (MS_U8)(pPoint32[u16WriteModeSrcPoint].B_Cb >> 8);
                                pMEM_temp[u32Accesspixel+2] = (MS_U8)((pPoint32[u16WriteModeSrcPoint].R_Cr & 0x0F) << 4) | (MS_U8)(pPoint32[u16WriteModeSrcPoint].G_Y >> 6);
                                pMEM_temp[u32Accesspixel+3] = (MS_U8)(pPoint32[u16WriteModeSrcPoint].R_Cr>> 4);

                            }
                            else if(eBufType == E_XC_OUTPUTDATA_RGB8BITS)
                            {
                                // target 444 10bits format, input R8G8B8 mode
                                pMEM_temp[u32Accesspixel] = (pPoint24[u16WriteModeSrcPoint].B_Cb & 0x3F) << 2;
                                pMEM_temp[u32Accesspixel+1] = ((pPoint24[u16WriteModeSrcPoint].G_Y & 0x0F) << 4) | ((pPoint24[u16WriteModeSrcPoint].B_Cb >>6) & 0x03);
                                pMEM_temp[u32Accesspixel+2] = ((pPoint24[u16WriteModeSrcPoint].R_Cr & 0x03) << 6) | (pPoint24[u16WriteModeSrcPoint].G_Y >> 4);
                                pMEM_temp[u32Accesspixel+3] = (pPoint24[u16WriteModeSrcPoint] .R_Cr >>2);
                            }
                        }

                    }
                    else // 8bits
                    {
#if (_FIELD_PACKING_MODE_SUPPORTED)
                        if(bIsReadMode)
                        {
                            // for get buffer data mode
                            if(eBufType == E_XC_OUTPUTDATA_RGB10BITS)
                            {
                                // source 444 8bits format, output R10G10B10 mode
                                pPoint32->B_Cb = pMEM_temp[u32Accesspixel] << 2;
                                pPoint32->G_Y = pMEM_temp[u32Accesspixel+u8YCSeparateOffset] << 2;
                                pPoint32->R_Cr = pMEM_temp[u32Accesspixel+u8YCSeparateOffset+u8YCSeparateOffset] << 2;
                            }
                            else if(eBufType == E_XC_OUTPUTDATA_RGB8BITS)
                            {
                                // source 444 8bits format, output R8G8B8 mode
                                pPoint24->B_Cb = pMEM_temp[u32Accesspixel];
                                pPoint24->G_Y = pMEM_temp[u32Accesspixel+u8YCSeparateOffset];
                                pPoint24->R_Cr = pMEM_temp[u32Accesspixel+u8YCSeparateOffset+u8YCSeparateOffset];
                            }
                        }
                        else
                        {
                            // for set buffer data mode
                            if(eBufType == E_XC_OUTPUTDATA_RGB10BITS)
                            {
                                // target 444 8bits format, input R10G10B10 mode
                                pMEM_temp[u32Accesspixel] = (MS_U8)(pPoint32[u16WriteModeSrcPoint].B_Cb>> 2);
                                pMEM_temp[u32Accesspixel+u8YCSeparateOffset]= (MS_U8)(pPoint32[u16WriteModeSrcPoint].G_Y>> 2);
                                pMEM_temp[u32Accesspixel+u8YCSeparateOffset+u8YCSeparateOffset]= (MS_U8)(pPoint32[u16WriteModeSrcPoint].R_Cr>> 2);
                            }
                            else if(eBufType == E_XC_OUTPUTDATA_RGB8BITS)
                            {
                                // target 444 8bits format, input R8G8B8 mode
                                pMEM_temp[u32Accesspixel] = pPoint24[u16WriteModeSrcPoint].B_Cb;
                                pMEM_temp[u32Accesspixel+u8YCSeparateOffset] = pPoint24[u16WriteModeSrcPoint].G_Y;
                                pMEM_temp[u32Accesspixel+u8YCSeparateOffset+u8YCSeparateOffset] = pPoint24[u16WriteModeSrcPoint].R_Cr;
                            }
                            else
                                printf("-->  not support type \n");
                        }
#else
                        MS_U8 u8JumpNum = u32Accesspixel/ (2*BYTE_PER_WORD);
                        MS_U8 u8BoundMax = (2*BYTE_PER_WORD) + 2* u8JumpNum * (2*BYTE_PER_WORD);

                        u32Accesspixel += u32JumpOffset*u8JumpNum;

                        if(bIsReadMode)
                        {
                            // for get buffer data mode
                            if(eBufType == E_XC_OUTPUTDATA_RGB10BITS)
                            {
                                // source 444 8bits format, output R10G10B10 mode
                                pPoint32->B_Cb = pMEM_temp[u32Accesspixel] << 2;

                                if(u32Accesspixel+1 >= u8BoundMax)
                                    pPoint32->G_Y = pMEM_temp[u32Accesspixel+1+u32JumpOffset] << 2;
                                else
                                    pPoint32->G_Y = pMEM_temp[u32Accesspixel+1] << 2;

                                if(u32Accesspixel+2 >= u8BoundMax)
                                    pPoint32->R_Cr = pMEM_temp[u32Accesspixel+2+u32JumpOffset] << 2;
                                else
                                    pPoint32->R_Cr = pMEM_temp[u32Accesspixel+2] << 2;
                            }
                            else if(eBufType == E_XC_OUTPUTDATA_RGB8BITS)
                            {
                                // source 444 8bits format, output R8G8B8 mode
                                pPoint24->B_Cb = pMEM_temp[u32Accesspixel];

                                if(u32Accesspixel+1 >= u8BoundMax)
                                    pPoint24->G_Y = pMEM_temp[u32Accesspixel+1+u32JumpOffset];
                                else
                                    pPoint24->G_Y = pMEM_temp[u32Accesspixel+1];

                                if(u32Accesspixel+2 >= u8BoundMax)
                                    pPoint24->R_Cr = pMEM_temp[u32Accesspixel+2+u32JumpOffset];
                                else
                                    pPoint24->R_Cr = pMEM_temp[u32Accesspixel+2];
                            }
                        }
                        else
                        {
                            // for set buffer data mode
                            if(eBufType == E_XC_OUTPUTDATA_RGB10BITS)
                            {
                                // target 444 8bits format, input R10G10B10 mode
                                pMEM_temp[u32Accesspixel] = pPoint32[u16WriteModeSrcPoint].B_Cb >> 2;

                                if(u32Accesspixel+1 >= u8BoundMax)
                                    pMEM_temp[u32Accesspixel+1+u32JumpOffset] = pPoint32[u16WriteModeSrcPoint].G_Y >> 2;
                                else
                                    pMEM_temp[u32Accesspixel+1]= pPoint32[u16WriteModeSrcPoint].G_Y>> 2;

                                if(u32Accesspixel+2 >= u8BoundMax)
                                    pMEM_temp[u32Accesspixel+2+u32JumpOffset] = pPoint32[u16WriteModeSrcPoint].R_Cr>> 2;
                                else
                                    pMEM_temp[u32Accesspixel+2]= pPoint32[u16WriteModeSrcPoint].R_Cr >> 2;
                            }
                            else if(eBufType == E_XC_OUTPUTDATA_RGB8BITS)
                            {
                                // target 444 8bits format, input R8G8B8 mode
                                pMEM_temp[u32Accesspixel] = pPoint24[u16WriteModeSrcPoint].B_Cb;

                                if(u32Accesspixel+1 >= u8BoundMax)
                                    pMEM_temp[u32Accesspixel+1+u32JumpOffset] = pPoint24[u16WriteModeSrcPoint].G_Y;
                                else
                                    pMEM_temp[u32Accesspixel+1] = pPoint24[u16WriteModeSrcPoint].G_Y;

                                if(u32Accesspixel+2 >= u8BoundMax)
                                    pMEM_temp[u32Accesspixel+2+u32JumpOffset] = pPoint24[u16WriteModeSrcPoint].R_Cr;
                                else
                                    pMEM_temp[u32Accesspixel+2] = pPoint24[u16WriteModeSrcPoint].R_Cr;
                            }
                            else
                                printf("-->  not support type \n");
                        }
#endif
                    }

                    if(bIsReadMode)
                    {
                        if(eBufType == E_XC_OUTPUTDATA_RGB10BITS)
                        {
                            FB_DBG(printf("==> index: %d, R: %d, G:%d, B: %d \n", index, pPoint32->R_Cr, pPoint32->G_Y, pPoint32->B_Cb));
                            pPoint32++;
                        }
                        else if(eBufType == E_XC_OUTPUTDATA_RGB8BITS)
                        {
                            FB_DBG(printf("==> index: %d, R: %d, G:%d, B: %d \n", index, pPoint24->R_Cr, pPoint24->G_Y, pPoint24->B_Cb));
                            pPoint24++;
                        }
                    }
                }
            }

            FB_DBG(printf("pMEM_temp=0x%x\n", (int)pMEM_temp));
            FB_DBG(printf("pMEM_temp[%u]=", (u16LBOffset)));
            FB_DBG(printf( "0x%x\n",pMEM_temp[u16LBOffset]));

        }
    }
    FB_DBG(printf("=>Get Memory finish\n"));

}

void MApi_XC_Set_BufferData(E_XC_INPUTDATA_TYPE eBufType, MS_WINDOW_TYPE *pDstRect, MS_PIXEL_32BIT *pSrcBuf, MS_WINDOW_TYPE *pSrcRect, SCALER_WIN eWindow)
{
    E_XC_OUTPUTDATA_TYPE eDataBufType;

    if (eBufType == E_XC_INPUTDATA_RGB8BITS)
    {
        eDataBufType = E_XC_OUTPUTDATA_RGB8BITS;
    }
    else if (eBufType == E_XC_INPUTDATA_RGB10BITS)
    {
        eDataBufType = E_XC_OUTPUTDATA_RGB10BITS;
    }
    else
    {
        printf("Unsupported Buf Type! \n");
        return;
    }

    MDrv_XC_Access_BufferData(eDataBufType, pDstRect, pSrcBuf, pSrcRect, eWindow, FALSE);
}

void MApi_XC_Get_BufferData(E_XC_OUTPUTDATA_TYPE eBufType, MS_WINDOW_TYPE *pRect, void *pRectBuf, SCALER_WIN eWindow)
{
    MDrv_XC_Access_BufferData(eBufType, pRect, pRectBuf, NULL, eWindow, TRUE);
}

#if 0
//-------------------------------------------------------------------------------------------------
/// Get frame data
/// @param  stSrcInfo           \b IN: @ref XC_ApiStatus
/// @param  eRect_Info          \b IN: @ref MS_RECT_INFO
//-------------------------------------------------------------------------------------------------
void MApi_XC_GetFrameData(XC_ApiStatus *stSrcInfo, MS_RECT_INFO *eRect_Info)
{
    MS_U32 u32RectSize;

    if(eRect_Info->s16X_Start <0 || eRect_Info->s16X_Start >= (stSrcInfo->u16H_SizeAfterPreScaling -1) ||
       eRect_Info->s16X_Size <0 || eRect_Info->s16X_Size >= (stSrcInfo->u16H_SizeAfterPreScaling -1) ||
       eRect_Info->s16X_Start > eRect_Info->s16X_Size || eRect_Info->s16Y_Start > eRect_Info->s16Y_Size||
       eRect_Info->s16Y_Start <0 || eRect_Info->s16Y_Start >= (stSrcInfo->u16V_SizeAfterPreScaling -1) ||
       eRect_Info->s16Y_Size <0 || eRect_Info->s16Y_Size >= (stSrcInfo->u16V_SizeAfterPreScaling -1))
    {
        MS_CRITICAL_MSG(printf("invalid parameter!\n"));
        return;
    }

    u32RectSize = eRect_Info->u32RectPitch * (eRect_Info->s16Y_Size - eRect_Info->s16Y_Start + 1) * eRect_Info->u16PointSize;

    MDrv_SC_GetFrameDataCore(eRect_Info->s16X_Start, eRect_Info->s16Y_Start, eRect_Info->s16X_Size, eRect_Info->s16Y_Size,
                              eRect_Info->pRect,
                              eRect_Info->u32RectPitch,
                              u32RectSize,
                              eRect_Info->u16PointSize);

}
#endif
//void MApi_XC_SetFrameData(XC_ApiStatus stSrcInfo, MS_RECT_INFO eRect_Info);


static void _MDrv_SC_WritePixel(volatile MS_U8* pMEM, MS_BOOL bMemFmt422,
                         void *pPoint, MS_U16 u16PointSize,
                         MS_BOOL bR_Cr)
{
    MS_PIXEL_24BIT *pPoint24 = (MS_PIXEL_24BIT*)pPoint;
    MS_PIXEL_32BIT *pPoint32 = (MS_PIXEL_32BIT*)pPoint;

    if (bMemFmt422)
    {
        if (u16PointSize == 4)
        {
            pMEM[0] = (MS_U8)(pPoint32->G_Y>>2);
            pMEM[1] = (MS_U8)(((bR_Cr) ? pPoint32->R_Cr : pPoint32->B_Cb)>>2);
        }
        else
        {
            pMEM[0] = pPoint24->G_Y;
            pMEM[1] = (bR_Cr) ? pPoint24->R_Cr : pPoint24->B_Cb;
        }
    }
    else
    {
        if (u16PointSize == 4)
        {
            pMEM[0] = (MS_U8)(pPoint32->B_Cb);
            pMEM[1] = (MS_U8)((pPoint32->G_Y << 2) | (pPoint32->B_Cb>> 8));
            pMEM[2] = (MS_U8)((pPoint32->R_Cr<< 4) | (pPoint32->G_Y >> 6));
            pMEM[3] = (MS_U8)(pPoint32->R_Cr >> 4);
        }
        else
        {
            pMEM[0] = (pPoint24->B_Cb<< 2);
            pMEM[1] = (pPoint24->G_Y << 4) | (pPoint24->B_Cb>> 6);
            pMEM[2] = (pPoint24->R_Cr<< 6) | (pPoint24->G_Y >> 4);
            pMEM[3] = (pPoint24->R_Cr>> 2);
        }
    }
}


void MDrv_SC_SetFrameDataCore(MS_S16 x0, MS_S16 y0, MS_S16 x1, MS_S16 y1,
                              MS_U8 *pRect,
                              MS_U32 u32RectPitch,
                              MS_U32 u32RectSize,
                              MS_U16 u16PointSize)
{
    MS_U8 *pPoint;
    volatile MS_U8 *pMEM;
    volatile MS_U8 *pMEM_temp;
    MS_U8 u8BytePerPixel;
    MS_U8 u8LSB_YM4_Size;
    MS_S16 x, y;
    MS_U16 u16OffsetPixelAlign;
    MS_U32 u32Y_Offset, u32X_Offset;
    SC_FRAMEBUF_INFO_t FBInfo;

    Hal_SC_get_framebuf_Info(&FBInfo);

    // not implement pre-scaling down
    if (FBInfo.bMemFormat422 == TRUE)
    {
        if (FBInfo.u8BitPerPixel == 24)
        {
            FB_DBG(printf("422_10BIT\n"));
            u8BytePerPixel = 3;
            u8LSB_YM4_Size = BYTE_PER_WORD*2;
            u16OffsetPixelAlign = BYTE_PER_WORD/2*2; // *2 (F0 F0)
        }
        else //if (FBInfo.u8BitPerPixel == 16)
        {
            FB_DBG(printf("422_8BIT\n"));
            u8BytePerPixel = 2;
            u8LSB_YM4_Size = 0;
            u16OffsetPixelAlign = BYTE_PER_WORD/2*2; // *2 (F0 F0)
        }
    }
    else
    {   // using 444 10 bit
        FB_DBG(printf("444_10BIT\n"));
        u8BytePerPixel = 4;
        u8LSB_YM4_Size = 0;
        u16OffsetPixelAlign = BYTE_PER_WORD/4*2; // *2 (F0 F0)
    }

    for (y = y0; y <= y1; y++)
    {
        if (FBInfo.bInterlace)
        {
            u32Y_Offset = (MS_U32)y/2 * FBInfo.u16IPMOffset * u8BytePerPixel * FBInfo.u8FBNum;
            if (y & 0x1)
                u32Y_Offset += FBInfo.u32IPMBase1;
            else
                u32Y_Offset += FBInfo.u32IPMBase0;
        }
        else
        {
            u32Y_Offset = (MS_U32)y * FBInfo.u16IPMOffset * u8BytePerPixel * FBInfo.u8FBNum;
            u32Y_Offset += FBInfo.u32IPMBase0;
        }
        FB_DBG(printf("[1]u32Y_Offset:0x%x, u16IPMOffset:%u, y:%u, u8BytePerPixel:%u, u8FBNum:%u\n",
            (int)u32Y_Offset, FBInfo.u16IPMOffset, y, u8BytePerPixel, FBInfo.u8FBNum));

        for (x = x0; x <= x1; x++)
        {
            u32X_Offset = (MS_U32)x / u16OffsetPixelAlign * u16OffsetPixelAlign * u8BytePerPixel * FBInfo.u8FBNum;

            FB_DBG(printf("[2]u32X_Offset=0x%x, u16OffsetPixelAlign=%u, x=%u, align(x)=%u\n",
                (int)u32X_Offset, u16OffsetPixelAlign, x, x / u16OffsetPixelAlign * u16OffsetPixelAlign));

            pPoint = pRect+ (u32RectPitch * (y - y0) + (x - x0)) * u16PointSize;
            if ((x / u16OffsetPixelAlign) & 0x1)
            {
                pMEM = (volatile MS_U8 *)(0 + u32Y_Offset + u32X_Offset + u8LSB_YM4_Size);
            }
            else
            {
                pMEM = (volatile MS_U8 *)(0 + u32Y_Offset + u32X_Offset);
            }
            pMEM_temp = (MS_U8 *)MS_PA2KSEG1((MS_U32)pMEM);
            FB_DBG(printf("[3]pMEM=0x%x, x:%u, y:%u, odd:%u\n",
                (int)pMEM, x, y, ((x / u16OffsetPixelAlign) & 0x1)));

            if (FBInfo.bMemFormat422 == TRUE)
            {
                _MDrv_SC_WritePixel(&pMEM_temp[(x%u16OffsetPixelAlign)*2],
                                    FBInfo.bMemFormat422,
                                    pPoint,
                                    u16PointSize,
                                    (x&0x1));
            }
            else
            {
                _MDrv_SC_WritePixel(&pMEM_temp[(x%u16OffsetPixelAlign)*4],
                                    FBInfo.bMemFormat422,
                                    pPoint,
                                    u16PointSize,
                                    (x&0x1));
            }
        }
    }
}

#if 1
#define H_CAP_ADD_FOR_GARBAGE 2
#define V_CAP_ADD_FOR_GARBAGE 2

typedef struct
{
    MS_U8 u8NRenable;
    MS_U8 u8Memory_FMT;
    MS_U8 u8IPM_Memory_FMT;
    MS_U8 u8OPM_Memory_FMT;

    SC_FRAMEBUF_INFO_t pFBInfo;

    MS_U8 u8IPMux;
    MS_U8 u8IPClk;
    MS_U8 u8OPMux;

    MS_U8 u8CapSource;
    MS_U8 u8CapMethod;
    MS_U8 u8User_Interlace;
    MS_WINDOW_TYPE capture_win;
    MS_BOOL bIsRGB_Format;
    MS_BOOL bIsHPreDown;
    MS_BOOL bIsVPreDown;
    MS_BOOL bIs444to422_filter;
    MS_U8 u8Bit_Format;
    MS_U32 u32IRQ_Mask;
}XC_IPM_Setting_Status;
//-------------------------------------------------------------------------------------------------
/// Store the current Setting before reroute the path from VOP to IP
/// @param  pXC_PreSetting      \b IN: The current setting before reroute
/// @return @ref XC_IPM_Setting_Status
//-------------------------------------------------------------------------------------------------
void MApi_XC_Get_Current_Setting(XC_IPM_Setting_Status *pXC_PreSetting)
{
    //Step3, Store original setting
    pXC_PreSetting->u8NRenable= HAL_SC_get_dnr_setting(MAIN_WINDOW);
    //Step4, Get input/output display data format
    pXC_PreSetting->u8Memory_FMT = HAL_SC_get_memory_fmt();
    pXC_PreSetting->u8OPM_Memory_FMT = HAL_SC_get_OPM_memory_fmt();
    pXC_PreSetting->u8IPM_Memory_FMT = HAL_SC_get_IPM_memory_fmt();
    //Step5, Store Memory Address for Capture
    Hal_SC_get_framebuf_Info(&(pXC_PreSetting->pFBInfo ));
    //Step6, Store INT from Main Capture port
    pXC_PreSetting->u32IRQ_Mask = HAL_SC_get_IRQ_mask_0_31();
    //Step7/8, Store IP MUX/Store IP Clock
    pXC_PreSetting->u8IPMux = HAL_SC_ip_get_ipmux();
    pXC_PreSetting->u8IPClk = HAL_SC_ip_get_ipclk();
    //Step9, Store OP MUX
    pXC_PreSetting->u8OPMux = HAL_SC_get_VOP_mux();
    //Step10, Store Capture Source
    pXC_PreSetting->u8CapSource = HAL_SC_ip_get_capturesource();
    //Step11,
    pXC_PreSetting->u8CapMethod = HAL_SC_ip_get_capture_method();
    //Step12,
    pXC_PreSetting->u8User_Interlace= Hal_SC_ip_get_user_def_interlace_status();
    //Step13, Store capture Infor
    Hal_SC_ip_get_capture_window( &(pXC_PreSetting->capture_win), MAIN_WINDOW);
    //Setp14, Store capture format
    pXC_PreSetting->bIsRGB_Format = HAL_SC_ip_get_capture_format();
    //Setp15, Store Pre_memory scaling down status
        //H Scaling Down enable
    pXC_PreSetting->bIsHPreDown = HAL_SC_ip_get_h_predown_info();
        //V Scaling Down enable
    pXC_PreSetting->bIsHPreDown = HAL_SC_ip_get_v_predown_info();
    //Step 16, Store the 444to422 setting
    pXC_PreSetting->bIs444to422_filter = HAL_SC_ip_get_444to422_filter_mod();
    pXC_PreSetting->u8Bit_Format = HAL_SC_get_memory_bit_fmt(MAIN_WINDOW);

}
//-------------------------------------------------------------------------------------------------
/// Setting for reroute the path from VOP to IP
/// @param  eXC_PreSetting      \b IN: The new setting for reroute
/// @param  bRestore               \b IN: Enable Restore mode or not
//-------------------------------------------------------------------------------------------------
void MApi_XC_Set_New_Setting(XC_IPM_Setting_Status eXC_PreSetting, MS_BOOL bRestore)
{
    //Step 3, Store original setting
    HAL_SC_enable_dnr(eXC_PreSetting.u8NRenable, MAIN_WINDOW);
    //Step4, Set OP memory format
    HAL_SC_set_OPM_memory_fmt(eXC_PreSetting.u8OPM_Memory_FMT);

    //Step 5, Set Memory Address for Capture
    //Set IPM Offset and Fetch
    Hal_SC_set_framebuf_Info(eXC_PreSetting.pFBInfo);

    //Step 6, Set IRQ for Main Capture port
    HAL_SC_set_IRQ_mask_0_31(eXC_PreSetting.u32IRQ_Mask);
    //Step 7/8, Set IP MUX and IP Clock
    HAL_SC_ip_set_ipmux(eXC_PreSetting.u8IPMux);
    HAL_SC_ip_set_ipclk(eXC_PreSetting.u8IPClk);
    //Step 9~12
    if(bRestore)
    {
        HAL_SC_set_capture_image2ip(DISABLE);
        HAL_SC_set_VOP_mux(eXC_PreSetting.u8OPMux);

        HAL_SC_ip_set_capturesource(eXC_PreSetting.u8CapSource);
        //Set Capture Method
        Hal_SC_ip_set_image_wrap((MS_BOOL)eXC_PreSetting.u8CapMethod&BIT(0),
                                                       (MS_BOOL)eXC_PreSetting.u8CapMethod&BIT(1), MAIN_WINDOW);
        Hal_SC_ip_set_input_sync_reference_edge((MS_BOOL)eXC_PreSetting.u8CapMethod&BIT(3),
                                                       (MS_BOOL)eXC_PreSetting.u8CapMethod&BIT(4), MAIN_WINDOW);
        Hal_SC_ip_set_input_vsync_delay((MS_BOOL)eXC_PreSetting.u8CapMethod&BIT(5), MAIN_WINDOW);
        Hal_SC_ip_set_de_only_mode((MS_BOOL)eXC_PreSetting.u8CapMethod&BIT(6), MAIN_WINDOW);
        //Disable User def Interlace mode
        Hal_SC_ip_set_user_def_interlace_status((MS_BOOL)eXC_PreSetting.u8User_Interlace&BIT(0),
                                                                              (MS_BOOL)eXC_PreSetting.u8User_Interlace&BIT(1));
        HAL_SC_ip_set_444to422_filter_mod(eXC_PreSetting.bIs444to422_filter);
    }
    else
    {
        HAL_SC_set_capture_image2ip(ENABLE);
        /*BK0F_46_L[7:4]
        0 : VOP2_dp input data
        1 : BRI output
        2 : HBC output
        3 : CON_BRI output
        4 : FWC output
        5 : gamma output
        6 : noise dither output
        */
        HAL_SC_set_VOP_mux(0x50);
        //Set Input Source
        /* #000: Analog 1.
            #001: Analog 2.
            #010: Analog 3.
            #011: DVI.
            #100: Video.
            #101: HDTV
            #111: HDMI. */
        HAL_SC_ip_set_capturesource(0x03);
        //Set Capture Method
        Hal_SC_ip_set_image_wrap( DISABLE, DISABLE, MAIN_WINDOW);
        Hal_SC_ip_set_input_sync_reference_edge( DISABLE, DISABLE, MAIN_WINDOW);
        Hal_SC_ip_set_input_vsync_delay( DISABLE, MAIN_WINDOW);
        Hal_SC_ip_set_de_only_mode( DISABLE, MAIN_WINDOW);
        //Disable User def Interlace mode
        Hal_SC_ip_set_user_def_interlace_status(DISABLE, DISABLE);

    }

    //Step 13, Set New Capture Window
    Hal_SC_ip_set_capture_v_start(eXC_PreSetting.capture_win.y , MAIN_WINDOW);
    Hal_SC_ip_set_capture_h_start(eXC_PreSetting.capture_win.x , MAIN_WINDOW);
    Hal_SC_ip_set_capture_v_size(eXC_PreSetting.capture_win.height, MAIN_WINDOW);
    Hal_SC_ip_set_capture_h_size(eXC_PreSetting.capture_win.width, MAIN_WINDOW);
    //Step 14, Set capture format, Default Display out is RGB
    HAL_SC_ip_set_capture_format(eXC_PreSetting.bIsRGB_Format);
    //Step 15, setting H/V pre down mode
    HAL_SC_ip_set_h_predown_info(eXC_PreSetting.bIsHPreDown);
    HAL_SC_ip_set_v_predown_info(eXC_PreSetting.bIsVPreDown);
    //Step 16, Set Capture memory store format
    HAL_SC_set_IPM_memory_fmt(eXC_PreSetting.u8IPM_Memory_FMT);

}
void MApi_XC_Rerout_for_Capture_VOP_Data(void)
{
    XC_IPM_Setting_Status stXC_PreSetting;
    XC_IPM_Setting_Status stXC_NewSetting;
    MS_U16 u16NewIPMFetch, u16NewIPMOffset;
    SC_FRAMEBUF_INFO_t FBInfo;

    memset(&stXC_PreSetting,0,sizeof(XC_IPM_Setting_Status));
    memset(&stXC_NewSetting,0,sizeof(XC_IPM_Setting_Status));
    //Step 1, Freeze image
    Hal_SC_set_freezeimg(ENABLE, MAIN_WINDOW);
    //Step 2, Wait 1/20 second
    MsOS_DelayTaskUs(50); //wait at 1/20 second

    //Store Current setting
    MApi_XC_Get_Current_Setting(&stXC_PreSetting);
	//Init the value using pre setting value
    memcpy(&stXC_NewSetting, &stXC_PreSetting, sizeof(XC_IPM_Setting_Status));
    //Set the new setting
    stXC_NewSetting.u8NRenable=0x00;
    switch(stXC_PreSetting.u8Memory_FMT)
    {
        case 0:
            stXC_NewSetting.u8OPM_Memory_FMT=0x00;
            break;
        case 1:
            stXC_NewSetting.u8OPM_Memory_FMT=0x05;
            break;
        case 4:
            stXC_NewSetting.u8OPM_Memory_FMT=0x06;
            break;
        case 5:
            stXC_NewSetting.u8OPM_Memory_FMT=0x0B;
            break;
        case 6:
            stXC_NewSetting.u8OPM_Memory_FMT=0x05;
            break;
        case 7:
        default:
            stXC_NewSetting.u8OPM_Memory_FMT=0x00;
            break;
    }


    u16NewIPMFetch = (stXC_PreSetting.pFBInfo.u16IPMFetch + H_CAP_ADD_FOR_GARBAGE)&~1;

    if(u16NewIPMFetch > MST_LINE_BFF_MAX)
        u16NewIPMFetch = MST_LINE_BFF_MAX;
    u16NewIPMOffset = u16NewIPMFetch;
    if(u16NewIPMOffset & 0x0F)
         u16NewIPMOffset = (u16NewIPMOffset & 0xFFF0) + 16;

    u16NewIPMFetch = (u16NewIPMFetch + 1) & ~1;
    stXC_NewSetting.pFBInfo.u16IPMFetch = u16NewIPMFetch;
    stXC_NewSetting.pFBInfo.u16IPMOffset = u16NewIPMOffset;
    // TODO: Implement V Length
    //stXC_NewSetting.pFBInfo->u16VLength =;
    stXC_NewSetting.pFBInfo.bLinearAddrMode = TRUE;
    stXC_NewSetting.u32IRQ_Mask = stXC_NewSetting.u32IRQ_Mask|0x033C03C0;
    stXC_NewSetting.u8IPMux = 0x70;
    stXC_NewSetting.u8IPClk = 0x1C;
    //Step 14, Set capture format YCbCr/RGB by Display out format
    //Default is RGB
    stXC_NewSetting.bIsRGB_Format=FALSE;

    stXC_NewSetting.bIsHPreDown=DISABLE;
    stXC_NewSetting.bIsVPreDown=DISABLE;
    Hal_SC_get_framebuf_Info(&FBInfo);
    if (FBInfo.bMemFormat422 == TRUE)
    {
        if (FBInfo.u8BitPerPixel == 20)//24
        {
            FB_DBG(printf("YC 422_10BIT\n"));
            stXC_NewSetting.u8IPM_Memory_FMT=0x05;
            stXC_NewSetting.u8Bit_Format=0x00;
        }
        else //if (FBInfo.u8BitPerPixel == 16)
        {
            FB_DBG(printf("YC 422_8BIT\n"));
            stXC_NewSetting.u8IPM_Memory_FMT=0x00;
            stXC_NewSetting.u8Bit_Format=0x00;
        }
    }
    else
    {
        if (FBInfo.u8BitPerPixel == 24)
        {
            FB_DBG(printf("RGB/YCbCr 444_8BIT\n"));
            stXC_NewSetting.u8IPM_Memory_FMT=0x01;
            stXC_NewSetting.u8Bit_Format=0x01;
        }
    }
    MApi_XC_Set_New_Setting(stXC_NewSetting, DISABLE);
    //Step 17, Wait 1/20 second
    MsOS_DelayTaskUs(50); //wait at 1/20 second
    //Step 18, Set Capture Enable/Start
    HAL_SC_set_IPM_capture_start(ENABLE);
    //Step 19, Wait 1/20 second
    MsOS_DelayTaskUs(50); //wait at 1/20 second
    //Step 20, Set Capture Disable/Stop
    HAL_SC_set_IPM_capture_start(DISABLE);
    //Step 21, Wait 1/20 second
    MsOS_DelayTaskUs(50); //wait at 1/20 second

    //Restore the previous setting
    MApi_XC_Set_New_Setting(stXC_PreSetting, ENABLE);
    //Step 24, Wait 1/20 second
    MsOS_DelayTaskUs(50); //wait at 1/20 second

    //Step 26, Freeze image
    Hal_SC_set_freezeimg(DISABLE, MAIN_WINDOW);

}

#endif

//-------------------------------------------------------------------------------------------------
/// Get the average data from memory
/// @param  mem_win      \b IN: The window position and size
/// @return @ref MS_AUTOADC_TYPE
//-------------------------------------------------------------------------------------------------
MS_AUTOADC_TYPE MApi_XC_GetAverageDataFromMemory(MS_WINDOW_TYPE* mem_win)
{

    MS_IMAGE_MEM_INFO  pMemInfo;
    _XC_ENTRY();
    pMemInfo.eCal_Win_Size.u16X = mem_win->x ;
    pMemInfo.eCal_Win_Size.u16Y = mem_win->y ;
    pMemInfo.eCal_Win_Size.u16Xsize = mem_win->width;
    pMemInfo.eCal_Win_Size.u16Ysize = mem_win->height ;
    MApi_XC_ReadRgbData(&pMemInfo);
    _XC_RETURN();
    return pMemInfo.pixelAverage;

#if 0

    #define GET_MEMAVG_WITHOUT_MAXMIN 0
    #define ADC_READ_PATTERN_FROM_EXTERNAL_SIGNAL 0

    #define RGB_CAL_MEM_BLOCKSIZE   (16)
    #define SCALER_SLOT_SIZE        (16)    // 128 bits

    #define RGB_CAL_BYTES_PER_PIXEL (4)
    #define RGB_CAL_PIXELS_PER_SLOT (SCALER_SLOT_SIZE/RGB_CAL_BYTES_PER_PIXEL)

    MS_AUTOADC_TYPE ptAdcData;
    MS_U8 *pu8VirMemBaseAdr, *pu8VirPixelAdr;
    MS_U32 u32PixelAddrOffset;
    MS_U32 u32Rtt, u32Gtt, u32Btt;
    MS_U32 i, x, y, tmp;
    MS_U16 u16LineOffset;
    MS_U8 u8Bank;
    MS_U16 u16MemVStart, u16MemHStart, u16CaptureHStart, u16CaptureVStart;
    MS_U16 u16MemVEnd, u16MemHEnd, u16CaptureHSize, u16CaptureVSize;

#if GET_MEMAVG_WITHOUT_MAXMIN
    MS_U16 u16MaxR,u16MaxG,u16MaxB;
    MS_U16 u16MinR,u16MinG,u16MinB;
    MS_U16 u16NumMaxR,u16NumMaxG,u16NumMaxB;
    MS_U16 u16NumMinR,u16NumMinG,u16NumMinB;
    MS_U16 u16NoMinMaxAvgCb, u16NoMinMaxAvgY, u16NoMinMaxAvgCr;
#endif

    SC_MEM_YUV444_FMT_t sMemBuf[mem_win->height * mem_win->width];


    ptAdcData.u16CH_AVG[0] = ptAdcData.u16CH_AVG[1] = ptAdcData.u16CH_AVG[2] = 0;


    //SC_BK_STORE;
    u8Bank = MDrv_ReadByte(BK_SELECT_00);

    // get line offset
    MDrv_WriteByte(BK_SELECT_00, REG_BANK_SCMI);
    u16LineOffset = MDrv_Read2Byte(L_BK_SCMI(0x0E));
    msg_sc_ip(printf("F2 IP frame buffer line offset (pixel unit) %d\n", u16LineOffset));


    pu8VirMemBaseAdr = (MS_U8*)g_XC_InitData.u32Main_FB_Start_Addr;
    msg_sc_ip(printf("DNR Buf Adr 0x%lx\n", (MS_U32)pu8VirMemBaseAdr));


    i = 0;
    u32Rtt = 0;
    u32Gtt = 0;
    u32Btt = 0;

#if GET_MEMAVG_WITHOUT_MAXMIN
    u16MaxR = 0;
    u16MaxG = 0;
    u16MaxB = 0;
    u16MinR = 0;
    u16MinG = 0;
    u16MinB = 0;

    u16NumMaxR = 0;
    u16NumMaxG = 0;
    u16NumMaxB = 0;
    u16NumMinR = 0;
    u16NumMinG = 0;
    u16NumMinB = 0;

    u16NoMinMaxAvgCb = 0;
    u16NoMinMaxAvgY = 0;
    u16NoMinMaxAvgCr = 0;
#endif

    MDrv_WriteByte(BK_SELECT_00, REG_BANK_IP1F2);
    u16CaptureVStart = MDrv_Read2Byte(L_BK_IP1F2(0x04));
    u16CaptureHStart = MDrv_Read2Byte(L_BK_IP1F2(0x05));
    u16CaptureVSize = MDrv_Read2Byte(L_BK_IP1F2(0x06));
    u16CaptureHSize = MDrv_Read2Byte(L_BK_IP1F2(0x07));
    u16MemVStart = 0;
    u16MemHStart = 0;
    u16MemVEnd = u16CaptureVSize;
    u16MemHEnd = u16CaptureHSize;


    msg_sc_ip(printf("Capture H Start %d V Start %d\n", u16CaptureHStart, u16CaptureVStart));
    msg_sc_ip(printf("Capture H End   %d V Eend  %d\n",
        (u16CaptureHStart + u16CaptureHSize),
        (u16CaptureVStart + u16CaptureVSize)));

    u16MemVStart = mem_win->y;
    u16MemHStart = mem_win->x;

    /*
    switch (u8Position)
    {
        case 0: //Frame
            u16MemVStart = 0;
            u16MemHStart = 0;
            break;
        case 1: //Up-Left Coner
            u16MemVStart = 0;
            u16MemHStart = 0;
            break;
        case 2: //Up-Right Coner
            u16MemVStart = 0;
            u16MemHStart = u16MemHEnd - mem_win->width;
            break;
        case 3: //Bottom-Left Coner
            u16MemVStart = u16MemVEnd - mem_win->height;
            u16MemHStart = 0;
            break;
        case 4: //Bottom-Right Coner
            u16MemVStart = u16MemVEnd - mem_win->height;
            u16MemHStart = u16MemHEnd - mem_win->width;
            break;
        case 5: //Center
            u16MemVStart = ((u16MemVEnd - u16MemVStart) / 2) - (mem_win->height / 2);
            u16MemHStart = ((u16MemHEnd - u16MemHStart) / 2) - (mem_win->width / 2);
            break;
#if ADC_READ_PATTERN_FROM_EXTERNAL_SIGNAL
        case 6: //White - Panel 1366x768, MSPG-925FS Model:217(1280x720p), Pattern: 15 (8 color bar)
            u16MemVStart = 0x170;
            u16MemHStart = 0x20;
            break;
        case 7: //Black - Panel 1366x768, MSPG-925FS Model:217(1280x720p), Pattern: 15 (8 color bar)
            u16MemVStart = 0x170;
            u16MemHStart = 0x520;
            break;
#endif
        default:
            break;
    }
    */

#if ADC_READ_PATTERN_FROM_EXTERNAL_SIGNAL
    msg_sc_ip(printf("Block Positon (%d, %d) size %dx%d\n",
        u16MemHStart,
        u16MemVStart,
        u16HBlockSize,
        mem_win.height));
    // Enable Debug Cross
    MDrv_WriteByte(BK_SELECT_00, REG_BANK_DLC);
    MDrv_WriteRegBit(L_BK_DLC(0x68), ENABLE, BIT(0));
    for(y=u16MemVStart; y<u16MemVStart+mem_win->height; y++)
    {
        for(x=u16MemHStart; x<u16MemHStart+mem_win->width; x++)
        {
            MS_U16 u16Y, u16Cb, u16Cr;

            MDrv_WriteByte(BK_SELECT_00, REG_BANK_DLC);

            MDrv_Write2Byte(L_BK_DLC(0x69), x);
            MDrv_Write2Byte(L_BK_DLC(0x6A), y);

            MsOS_DelayTask(10);

            u16Y = (MDrv_Read2Byte(L_BK_DLC(0x6B)) & 0x03FF);
            u16Cb = (MDrv_Read2Byte(L_BK_DLC(0x6C)) & 0x03FF);
            u16Cr = (MDrv_Read2Byte(L_BK_DLC(0x6D)) & 0x03FF);

            //printf("Y = 0x%04x, Cb = 0x%04x, Cr = 0x%04x\n", u16Y, u16Cb, u16Cr);
            u32Rtt += u16Cr;
            u32Gtt += u16Y;
            u32Btt += u16Cb;
        }
    }
    // Disable Debug Cross
    MDrv_WriteByte(BK_SELECT_00, REG_BANK_DLC);
    MDrv_WriteRegBit(L_BK_DLC(0x68), DISABLE, BIT(0));

    ptAdcData.u16CH_AVG[0] = u32Rtt / (mem_win.height * mem_win->width);
    ptAdcData.u16CH_AVG[1] = u32Gtt / (mem_win.height * mem_win->width);
    ptAdcData.u16CH_AVG[2] = u32Btt / (mem_win.height * mem_win->width);

    //SC_BK_RESTORE;
    MDrv_WriteByte(BK_SELECT_00, u8Bank);

    msg_sc_ip(printf("AvgCr AvgY AvgCb\n"));
    msg_sc_ip(printf("%04d  %04d %04d\n\n", ptAdcData.R_CH_AVG, ptAdcData.G_CH_AVG, ptAdcData.B_CH_AVG));


#else


    msg_sc_ip(printf("Block Positon (%d, %d) Mem Start (%d, %d) size %dx%d\n",
        (u16MemHStart + u16CaptureHStart),
        (u16MemVStart + u16CaptureVStart),
        u16MemHStart,
        u16MemVStart,
        mem_win->width,
        mem_win->height));


    for(y=u16MemVStart; y<u16MemVStart+mem_win->height; y++)
    {
        for(x=u16MemHStart; x<u16MemHStart+mem_win->width; x++)
        {
            u32PixelAddrOffset = y * u16LineOffset * RGB_CAL_BYTES_PER_PIXEL * 2;
            u32PixelAddrOffset += ((MS_U32)(x / RGB_CAL_PIXELS_PER_SLOT)) * SCALER_SLOT_SIZE * 2;
            pu8VirPixelAdr = pu8VirMemBaseAdr + u32PixelAddrOffset +
                                    (x % RGB_CAL_PIXELS_PER_SLOT) * RGB_CAL_BYTES_PER_PIXEL;

            memcpy(&(sMemBuf[i]), pu8VirPixelAdr, RGB_CAL_BYTES_PER_PIXEL);
            memcpy(&tmp, pu8VirPixelAdr, RGB_CAL_BYTES_PER_PIXEL);


#if GET_MEMAVG_WITHOUT_MAXMIN
            if (i == 0)
            {
                //printf("pu8VirPixelAdr 0x%lx\n", (U32)pu8VirPixelAdr);
                u16MaxR = sMemBuf[i].Cr;
                u16MinR = sMemBuf[i].Cr;
                u16MaxG = sMemBuf[i].Y;
                u16MinG = sMemBuf[i].Y;
                u16MaxB = sMemBuf[i].Cb;
                u16MinB = sMemBuf[i].Cb;
                u16NumMaxR = 1;
                u16NumMaxG = 1;
                u16NumMaxB = 1;
                u16NumMinR = 1;
                u16NumMinG = 1;
                u16NumMinB = 1;
                //while (1);
            }
            else
            {
                if (sMemBuf[i].Cr > u16MaxR)
                {
                    u16MaxR = sMemBuf[i].Cr;
                    u16NumMaxR = 1;
                }
                else if (sMemBuf[i].Cr == u16MaxR)
                {
                    u16NumMaxR++;
                }

                if (sMemBuf[i].Cr < u16MinR)
                {
                    u16MinR = sMemBuf[i].Cr;
                    u16NumMinR = 1;
                }
                else if (sMemBuf[i].Cr == u16MinR)
                {
                    u16NumMinR++;
                }


                if (sMemBuf[i].Y > u16MaxG)
                {
                    u16MaxG = sMemBuf[i].Y;
                    u16NumMaxG = 1;
                }
                else if (sMemBuf[i].Y == u16MaxG)
                {
                    u16NumMaxG++;
                }

                if (sMemBuf[i].Y < u16MinG)
                {
                    u16MinG = sMemBuf[i].Y;
                    u16NumMinG = 1;
                }
                else if (sMemBuf[i].Y == u16MinG)
                {
                    u16NumMinG++;
                }

                if (sMemBuf[i].Cb > u16MaxB)
                {
                    u16MaxB = sMemBuf[i].Cb;
                    u16NumMaxB = 1;
                }
                else if (sMemBuf[i].Cb == u16MaxB)
                {
                    u16NumMaxB++;
                }

                if (sMemBuf[i].Cb < u16MinB)
                {
                    u16MinB = sMemBuf[i].Cb;
                    u16NumMinB = 1;
                }
                else if (sMemBuf[i].Cb == u16MinB)
                {
                    u16NumMinB++;
                }
            }
#endif
            u32Rtt += sMemBuf[i].Cr;
            u32Gtt += sMemBuf[i].Y;
            u32Btt += sMemBuf[i].Cb;
            i++;
        }
    }



#if 0
    printf("\n");

    printf("CrBlock %dx%d", u16HBlockSize, u16VBlockSize);
    for (i = 0; i < (u16VBlockSize * u16HBlockSize); i++)
    {
        if ((i % u16HBlockSize) == 0)
        {
            printf("\n");
        }
        printf("%04d ", sMemBuf[i].Cr);
    }
    printf("\n");

    printf("YBlock %dx%d", u16HBlockSize, u16VBlockSize);
    for (i = 0; i < (u16VBlockSize * u16HBlockSize); i++)
    {
        if ((i % u16HBlockSize) == 0)
        {
            printf("\n");
        }
        printf("%04d ", sMemBuf[i].Y);
    }
    printf("\n");

    printf("CbBlock %dx%d", u16HBlockSize, u16VBlockSize);
    for (i = 0; i < (u16VBlockSize * u16HBlockSize); i++)
    {
        if ((i % u16HBlockSize) == 0)
        {
            printf("\n");
        }
        printf("%04d ", sMemBuf[i].Cb);
    }
    printf("\n");
#endif

#if GET_MEMAVG_WITHOUT_MAXMIN
    tmp = u32Rtt - ((MS_U32)u16MaxR * (MS_U32)u16NumMaxR) - ((MS_U32)u16MinR * (MS_U32)u16NumMinR);
    if ( tmp != 0 )
    {
        u16NoMinMaxAvgCr = tmp / ((mem_win->height * mem_win->width) - (u16NumMaxR + u16NumMinR));
    }

    tmp = u32Gtt - ((MS_U32)u16MaxG * (MS_U32)u16NumMaxG) - ((MS_U32)u16MinG * (MS_U32)u16NumMinG);
    if ( tmp != 0 )
    {
        u16NoMinMaxAvgY = tmp / ((mem_win->height * mem_win->width) - (u16NumMaxG + u16NumMinG));
    }

    tmp = u32Btt - ((MS_U32)u16MaxB * (MS_U32)u16NumMaxB) - ((MS_U32)u16MinB * (MS_U32)u16NumMinB);
    if ( tmp != 0 )
    {
        u16NoMinMaxAvgCb = tmp / ((mem_win->height * mem_win->width) - (u16NumMaxB + u16NumMinB));
    }
#endif

    ptAdcData.u16CH_AVG[0] = u32Rtt / (mem_win->height * mem_win->width);
    ptAdcData.u16CH_AVG[1] = u32Gtt / (mem_win->height * mem_win->width);
    ptAdcData.u16CH_AVG[2] = u32Btt / (mem_win->height * mem_win->width);


    //SC_BK_RESTORE;
    MDrv_WriteByte(BK_SELECT_00, u8Bank);

    msg_sc_ip( printf("ptAdcData[%04d,%04d,%04d]\n",ptAdcData.R_CH_AVG,ptAdcData.G_CH_AVG,ptAdcData.B_CH_AVG) );

#if GET_MEMAVG_WITHOUT_MAXMIN
    printf("MaxCr    MinCr    MaxY    MinY    MaxCb    MinCb    AvgCr AvgY AvgCb\n");
    printf("%04d     %04d     %04d    %04d    %04d     %04d     %04d  %04d %04d\n",
        u16MaxR, u16MinR, u16MaxG, u16MinG, u16MaxB, u16MinB, ptAdcData.R_CH_AVG, ptAdcData.G_CH_AVG, ptAdcData.B_CH_AVG);

    printf("NumMaxCr NumMinCr NumMaxY NumMinY NumMaxCb NumMinCb AvgCr AvgY AvgCr\n");
    printf("%04d     %04d     %04d    %04d    %04d     %04d     %04d  %04d %04d\n\n",
        u16NumMaxR, u16NumMinR, u16NumMaxG, u16NumMinG, u16NumMaxB, u16NumMinB, u16NoMinMaxAvgCr, u16NoMinMaxAvgY, u16NoMinMaxAvgCb);
#endif

#endif

    return ptAdcData;
#endif

}



MS_U8 MDrv_XC_wait_input_vsync(MS_U8 u8NumVSyncs, MS_U16 u16Timeout, SCALER_WIN eWindow)
{
    MS_U32 u32Time;
    MS_U8 u8VsyncPolarity;
    MS_U8 bVSync;
    MS_U8 u8VSyncVal;
    u32Time = MsOS_GetSystemTime();

	#ifdef MULTI_SCALER_SUPPORTED
	u8VsyncPolarity = Hal_SC_ip_get_input_vsync_polarity(eWindow);
	#else
	u8VsyncPolarity = Hal_SC_ip_get_input_vsync_polarity();
	#endif

	bVSync = !u8VsyncPolarity;

    while (1)
    {
        u8VSyncVal = Hal_SC_ip_get_input_vsync_value(eWindow);

	//for Special case,(like U4) if no subwindow, don't wait vsync. break immediately
        if(u8VSyncVal == 0xFF)
            break;

        if (u8VSyncVal == bVSync)
        {
            if ((bVSync == u8VsyncPolarity) && --u8NumVSyncs == 0)
                break;
            bVSync = !bVSync;
        }

        if ( ( MsOS_GetSystemTime() - u32Time) >= u16Timeout )
        {
            break;
        }
    }

    return u8NumVSyncs;
}

MS_U8 MDrv_XC_wait_output_vsync(MS_U8 u8NumVSyncs, MS_U16 u16Timeout, SCALER_WIN eWindow)
{
    MS_U32 u32Time;
    MS_U8 bVSync;
    MS_U8 u8VSyncVal;

    bVSync = 1;
    u32Time = MsOS_GetSystemTime();

    while (1)
    {
        u8VSyncVal = Hal_SC_ip_get_output_vsync_value(eWindow);

        //for Special case,(like U4) if no subwindow don't wait vsync. break immediately
        if(u8VSyncVal == 0xFF)
            break;

        if (u8VSyncVal == bVSync)
        {
            u8NumVSyncs -= 1;
            if ((!bVSync) && (u8NumVSyncs == 0))
                break;
            bVSync = !bVSync;
        }

        if ( (MsOS_GetSystemTime() - u32Time) >= u16Timeout )
        {
            break;
        }
    }
    return u8NumVSyncs;
}

//-------------------------------------------------------------------------------------------------
// This function will delay both 'u32DelayTime' ms and vsync signal
// @param  u32DelayTime      \b IN: delay time in ms unit
// @return MS_U32: real time cost of this timedelay
//-------------------------------------------------------------------------------------------------
MS_U32 MDrv_XC_DelayAndWaitOutVsync(MS_U32 u32DelayTime, SCALER_WIN eWindow)
{
    #define TIME_OUPUTVSYNC_TIMEOUT 30

    MS_U32 u32TimeStart, u32TimeSpend=0;
    MS_U8 bVSync;
    MS_U8 u8VSyncVal;

    bVSync = 1;
    u32TimeStart = MsOS_GetSystemTime();

    while (1)
    {
        u8VSyncVal = Hal_SC_ip_get_output_vsync_value(eWindow);
        //for Special case,(like U4) if no subwindow don't wait vsync. break immediately
        if(u8VSyncVal == 0xFF)
            break;
        if (u8VSyncVal == bVSync)
        {
            if ((u32TimeSpend >= u32DelayTime) && !bVSync)
            {
                //delay time is enough and now is also vsync ,so break
                break;
            }
            bVSync = !bVSync;
        }
        if (u32TimeSpend  >= (u32DelayTime + TIME_OUPUTVSYNC_TIMEOUT))
        {
            //wait vysnc time out
            break;
        }
        u32TimeSpend =  MsOS_GetSystemTime() - u32TimeStart;
    }
    return u32TimeSpend;
}
//-------------------------------------------------------------------------------------------------
/// This function will calculate and return H Frequency x 10
/// @param  wHPeriod      \b IN: Horizontal period
/// @return MS_U16: H Frequency x 10
//-------------------------------------------------------------------------------------------------
MS_U16 MApi_XC_CalculateHFreqx10(MS_U16 wHPeriod)
{
    if(wHPeriod)
        return (MS_U16)( (((MS_U32)g_XC_InitData.u32XTAL_Clock + (wHPeriod/2)) / wHPeriod ) / 100 ); //KHz
    else
        return 1;   // avoid devide by 0 error
}

//-------------------------------------------------------------------------------------------------
/// This function will calculate and return V Frequency x 10
/// @param  wHFreq      \b IN: Horizontal frequency
/// @param  wVTotal     \b IN: Vertical Total, usually the scan lines counts of a frame
/// @return MS_U16: V Frequency x 10
//-------------------------------------------------------------------------------------------------
MS_U16 MApi_XC_CalculateVFreqx10(MS_U16 wHFreq, MS_U16 wVTotal)
{
    if(wHFreq && wVTotal)
        return (MS_U16)( (((MS_U32)wHFreq * 1000 ) + (wVTotal/2)) / wVTotal ); // Hz
    else
        return 0;
}

//-------------------------------------------------------------------------------------------------
/// This function will calculate and return Vsync time
/// @param  u16VTotal       \b IN: Vercital total
/// @param  u16HPeriod      \b IN: Horizontal period
/// @return MS_U8:  VSync time
//-------------------------------------------------------------------------------------------------
MS_U8 MApi_XC_CalculateVsyncTime(MS_U16 u16VTotal, MS_U16 u16HPeriod)
{
    MS_U16 wHFreqX10; // horizontal frequency
    wHFreqX10 = MApi_XC_CalculateHFreqx10(u16HPeriod); // calculate HSync frequency
    return (MS_U8)(((u16VTotal*10)+(wHFreqX10/2))/wHFreqX10);
}
/******************************************************************************/
///Set line buffer clock
///@param u16HTotal \b IN
///- Horizontal total
///@return
///- Output Pixel colok
/******************************************************************************/
MS_U16 MDrv_XC_cal_pixclk(MS_U16 u16HTotal, SCALER_WIN eWindow)
{
    return (MS_U16)
    (((MS_U32)MApi_XC_CalculateHFreqx10(Hal_SC_ip_get_horizontalPeriod(eWindow))*u16HTotal+5000)/10000);
}


#if ENABLE_DEBUG_CAPTURE_N_FRAMES
static MS_BOOL bFinishedInputVSyncISR = FALSE;
static void _MApi_XC_InputVSyncISR(SC_INT_SRC eIntNum, void * pParam)
{
    static volatile MS_U8 u8Cnt = 0;
    volatile MS_BOOL *bFinishedISR = (volatile MS_BOOL *) pParam;

    u8Cnt++;

#if 1
    if(u8Cnt == 4)
    {
        Hal_SC_set_freezeimg(ENABLE, MAIN_WINDOW);
        u8Cnt = 0;
        *bFinishedISR = TRUE;
    }
#else
    printf("(%3d) E %4x W %d, R %d, pha %4x, prd %4x\n",
        u8Cnt,
        SC_R2BYTEMSK(REG_SC_BK01_1E_L, 0x0400),
        SC_R2BYTEMSK(REG_SC_BK12_3A_L, 0x07),           // current write bank
        SC_R2BYTEMSK(REG_SC_BK12_3A_L, 0x70) >> 4,      // current read bank
        MDrv_Read2Byte(L_BK_LPLL(0x11)),                // phase error
        MDrv_Read2Byte(L_BK_LPLL(0x13)));               // prd error
#endif
}
#endif

MS_BOOL MApi_XC_InterruptAttach(SC_INT_SRC eIntNum, SC_InterruptCb pIntCb, void * pParam)
{
    return MDrv_XC_InterruptAttach(eIntNum, pIntCb,pParam);
}

MS_BOOL MApi_XC_InterruptDeAttach(SC_INT_SRC eIntNum, SC_InterruptCb pIntCb, void * pParam)
{
    return MDrv_XC_InterruptDeAttach(eIntNum, pIntCb,pParam);
}

//-------------------------------------------------------------------------------------------------
//
// Attention! This function should only be used in the xc interrupt!!!
//
/// This function will tell the current read bank
/// @param  bDisable       \b IN: TRUE : Disable; FALSE: Enable
//-------------------------------------------------------------------------------------------------
MS_U16 MApi_XC_GetCurrentReadBank(SCALER_WIN eWindow)
{
    MS_U16 u16CurrentReadBank = 0;
#if (!ENABLE_REGISTER_SPREAD)
    MS_U8 u8Bank = 0;
    u8Bank = MDrv_ReadByte( BK_SELECT_00 );
#endif
    if(MAIN_WINDOW == eWindow)
    {
        u16CurrentReadBank = SC_R2BYTEMSK(REG_SC_BK12_3A_L, 0x70) >> 4 ;
    }
    else
    {
        u16CurrentReadBank = SC_R2BYTEMSK(REG_SC_BK12_7A_L, 0x70) >> 4 ;
    }
#if (!ENABLE_REGISTER_SPREAD)
    MDrv_WriteByte(BK_SELECT_00, u8Bank);
#endif
    return u16CurrentReadBank;
}

//-------------------------------------------------------------------------------------------------
/// This function will disable input source
/// @param  bDisable       \b IN: TRUE : Disable; FALSE: Enable
//-------------------------------------------------------------------------------------------------
void MApi_XC_DisableInputSource(MS_BOOL bDisable, SCALER_WIN eWindow)
{
    _XC_ENTRY();
    gSrcInfo[eWindow].bBluescreenEnabled = bDisable;

    if ((MApi_XC_MLoad_GetStatus(eWindow) == E_MLOAD_ENABLED) && (!MDrv_SC_GetSkipWaitVsync(eWindow)))
    {
        Hal_SC_disable_inputsource_burst(bDisable, eWindow);
    }
    else
    {
        if(eWindow==SUB_WINDOW)
            MDrv_XC_wait_output_vsync(2, 200, MAIN_WINDOW);
        Hal_SC_disable_inputsource(bDisable, eWindow);
    }
    _XC_RETURN();


#if ENABLE_DEBUG_CAPTURE_N_FRAMES
    printf("MApi_XC_DisableInputSource %d %d\n", bDisable, eWindow);

    if(bDisable)
    {
        // start ISR monitor
        MDrv_XC_InterruptDeAttach(SC_INT_F2_IPVS_SB, _MApi_XC_InputVSyncISR, (void *) &bFinishedInputVSyncISR);
    }
    else
    {
        bFinishedInputVSyncISR = FALSE;

        if(!MDrv_XC_InterruptIsAttached(SC_INT_F2_IPVS_SB, _MApi_XC_InputVSyncISR, (void *) &bFinishedInputVSyncISR))
        {
            MDrv_XC_InterruptAttach(SC_INT_F2_IPVS_SB, _MApi_XC_InputVSyncISR, (void *) &bFinishedInputVSyncISR);

            while(*((volatile MS_BOOL *) &bFinishedInputVSyncISR) == FALSE)
            {
            }

            MDrv_XC_InterruptDeAttach(SC_INT_F2_IPVS_SB, _MApi_XC_InputVSyncISR, (void *) &bFinishedInputVSyncISR);
        }
    }
#endif


/*    if(bDisable)
    {
        if(eWindow==0)
            printf("Main win Blue Screen ON\n");
        else
            printf("Sub win Blue Screen ON\n");
    }
    else
    {
        if(eWindow==0)
            printf("Main win Blue Screen OFF\n");
        else
            printf("Sub win Blue Screen OFF\n");
    }
*/
}

// Used in driver
void MDrv_XC_DisableInputSource(MS_BOOL bDisable, SCALER_WIN eWindow)
{
    gSrcInfo[eWindow].bBluescreenEnabled = bDisable;

    Hal_SC_disable_inputsource(bDisable, eWindow);

}

MS_BOOL MDrv_XC_IsInputSourceDisabled(SCALER_WIN eWindow)
{
    return Hal_SC_Is_InputSource_Disable(eWindow) ? TRUE : FALSE;
}
MS_BOOL MApi_XC_IsInputSourceDisabled(SCALER_WIN eWindow)
{
    MS_BOOL bIsInputSourceDisabled;
    _XC_ENTRY();
    bIsInputSourceDisabled = MDrv_XC_IsInputSourceDisabled(eWindow);
    _XC_RETURN();
    return bIsInputSourceDisabled;
}

//-------------------------------------------------------------------------------------------------
/// Get the auto phase value for the specific window
/// @param  eWindow                \b IN: which window we are going to get
/// @return MS_U32: Return full image sun of difference value between two pixles
//-------------------------------------------------------------------------------------------------
MS_U32 MApi_XC_GetPhaseValue(SCALER_WIN eWindow)
{
    MS_U32 u32PhaseValue; // double word buffer
    MS_U16 u8TimeOut = 0;
    MS_U16 u16Dummy = 1000;
    MS_U8 u8FirstReady =0 ,u8SecondReady = 0;

    //u8Bank = MDrv_ReadByte( BK_SELECT_00 );
    //MDrv_WriteByte( BK_SELECT_00, REG_BANK_IP1F2 );
    _XC_ENTRY();
    while ( Hal_SC_ip_is_auto_phase_result_ready(eWindow) && u8TimeOut < 500 )
    {
        u8TimeOut++;
    }
    //printf(" %u timeout \n",u8TimeOut );
    if ( u8TimeOut >= 500 )
    {
        _XC_RETURN();
        return 0; // Timeout
    }
    u8TimeOut = 0 ;

    while ( u8TimeOut < 100 )
    {
        // Waiting for ready

        while( !( Hal_SC_ip_is_auto_phase_result_ready(eWindow) ) && (u16Dummy--) ) ;

        u8FirstReady = Hal_SC_ip_is_auto_phase_result_ready(eWindow);

        u32PhaseValue = Hal_SC_ip_get_auto_phase_value(eWindow);

        u8SecondReady = Hal_SC_ip_is_auto_phase_result_ready(eWindow);

        if ( u8FirstReady == TRUE && u8SecondReady == TRUE )
        {
            _XC_RETURN();
            return u32PhaseValue;
        }
        u8TimeOut++;
    }
    _XC_RETURN();
    return 0;

}

//-------------------------------------------------------------------------------------------------
/// Auto-tune R/G/B Offset of ADC.
/// @param  u8Type              \b IN: report AUTO_MIN_[RGB] or AUTO_MAX_[RGB]
/// @param  u8DelayMS           \b IN: delay how long to get the report, usually, the delay will be 1 * VSyncTime
/// @return @ref MS_BOOL the min/max gain report
//-------------------------------------------------------------------------------------------------
MS_U16 MApi_XC_GetAutoGainResult(AUTO_GAIN_TYPE u8Type, MS_U8 u8DelayMS, SCALER_WIN eWindow)
{
    MS_U8 u8Ready0, u8Ready1, u8GainRptTimeout = 255;
    MS_U16 u16Value = 0;
    //MS_U8 u8Bank;

    //u8Bank = MDrv_ReadByte( BK_SELECT_00 );

    if(eWindow == MAIN_WINDOW)
    {
        //MDrv_WriteByte( BK_SELECT_00, REG_BANK_IP1F2 );
        //MDrv_WriteByteMask(L_BK_IP1F2(0x0F), u8Type, BIT(2) | BIT(1) | BIT(0));
        SC_W2BYTEMSK(REG_SC_BK01_0F_L, u8Type, BIT(2) | BIT(1) | BIT(0));
    }
    else
    {
        //MDrv_WriteByte( BK_SELECT_00, REG_BANK_IP1F1 );
        //MDrv_WriteByteMask(L_BK_IP1F1(0x0F), u8Type, BIT(2) | BIT(1) | BIT(0));
        SC_W2BYTEMSK(REG_SC_BK03_0F_L, u8Type, BIT(2) | BIT(1) | BIT(0));
    }

    MsOS_DelayTask(u8DelayMS);

    // Sometimes the gain report will give a very un-reasonable value,
    // So, we've to wait the IP1F2_0E[1] to be ready (=1) to get the gain report
    // For safety reason, you may use this flow:
    //    (1) Set what to get AUTO_MIN_[RGB] or AUTO_MAX_[RGB]
    //    (2) wait for ready bit to be off
    //                           (because sometimes when you change the AUTO_MIN_[RGB] or AUTO_MAX_[RGB],
    //                                you'll get a ready immediately but the report is the old value)
    //    (3) wait for ready bit to be on
    //    (4) get the report when ready bit is on (note: see source code below)
    //
    // For quick response, you may get the report when ready bit is on, the report is rarely wrong if ready bit is on.

    /*
    // wait for ready bit to be off
    while(MDrv_ReadByte(L_BK_IP1F2(0x0E)) & (0x1 << 1))
    {
        u8Ready = MDrv_ReadByte(L_BK_IP1F2(0x0E));
        u8Value = MDrv_ReadByte(L_BK_IP1F2(0x11));
        printf("DA:C=0x%bx,R=0x%bx,V=0x%bx\n", MDrv_ReadByte(L_BK_IP1F2(0x0F)), u8Ready, u8Value);
    }

    // wait for ready bit to be on
    while(!(MDrv_ReadByte(L_BK_IP1F2(0x0E)) & (0x1 << 1)))
    {
        u8Ready = MDrv_ReadByte(L_BK_IP1F2(0x0E));
        u8Value = MDrv_ReadByte(L_BK_IP1F2(0x11));
        printf("DA:C=0x%bx,R=0x%bx,V=0x%bx\n", MDrv_ReadByte(L_BK_IP1F2(0x0F)), u8Ready, u8Value);
    }
    */

    while(u8GainRptTimeout)
    {
        // It's important to read the 2 values by this order
        // (1) ready bit first
        // (2) report next
        // and no waste time between these 2 commands

        // Waiting until ready bit raise
		MS_U16 u16Dummy = 1000;
		while(!( MApi_XC_IsAutoGainResultReady(eWindow) ) && (u16Dummy--)) ;

        if(eWindow == MAIN_WINDOW)
        {
            u8Ready0 = MApi_XC_IsAutoGainResultReady(eWindow);
            //u16Value = (MDrv_Read2Byte(L_BK_IP1F2(0x11)) & 0x3FF);  // the 10 bit report is 0.0 ~ 255.75, [9:2] = 0 ~ 255, [1:0] = 0, 0.25, 0.5, 0.75
            u16Value = (SC_R2BYTE(REG_SC_BK01_11_L) & 0x3FF);  // the 10 bit report is 0.0 ~ 255.75, [9:2] = 0 ~ 255, [1:0] = 0, 0.25, 0.5, 0.75
            u8Ready1 = MApi_XC_IsAutoGainResultReady(eWindow);
        }
        else
        {
            u8Ready0 = MApi_XC_IsAutoGainResultReady(eWindow);
            //u16Value = (MDrv_Read2Byte(L_BK_IP1F1(0x11)) & 0x3FF);  // the 10 bit report is 0.0 ~ 255.75, [9:2] = 0 ~ 255, [1:0] = 0, 0.25, 0.5, 0.75
            u16Value = (SC_R2BYTE(REG_SC_BK03_11_L) & 0x3FF);  // the 10 bit report is 0.0 ~ 255.75, [9:2] = 0 ~ 255, [1:0] = 0, 0.25, 0.5, 0.75
            u8Ready1 = MApi_XC_IsAutoGainResultReady(eWindow);
        }

        // Check if the report is reasonable here
        if( u8Ready0 && u8Ready1)
        {
            break;
        }
        u8GainRptTimeout--;
    }
    MS_ASSERT(u8GainRptTimeout > 0);

    //MDrv_WriteByte( BK_SELECT_00, u8Bank );

    return u16Value;

}

//-------------------------------------------------------------------------------------------------
/// Check whether Auto phase result is ready or not
/// @param  eWindow                \b IN: which window we are going to query
/// @return @ref MS_BOOL
//-------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_IsAutoPhaseResultReady(SCALER_WIN eWindow)
{
    return Hal_SC_ip_is_auto_phase_result_ready(eWindow);
}

//-------------------------------------------------------------------------------------------------
/// Get auto phase result value
/// @param  eWindow                \b IN: which window we are going to query
/// @return @ref MS_BOOL
//-------------------------------------------------------------------------------------------------
MS_U32 MApi_XC_GetAutoPhaseResult(SCALER_WIN eWindow)
{
    return Hal_SC_ip_get_auto_phase_value(eWindow);
}

//-------------------------------------------------------------------------------------------------
/// check whether the auto position is ready or not for the specific window
/// @param  eWindow             \b IN: which window we are going to set
/// @return @ref MS_BOOL
//-------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_IsAutoPositionResultReady(SCALER_WIN eWindow)
{
    MS_BOOL bAuto_position;
    _XC_ENTRY();
    bAuto_position = Hal_SC_ip_is_auto_position_result_ready(eWindow);
    _XC_RETURN();
    return bAuto_position ; //Hal_SC_ip_is_auto_position_result_ready(eWindow);

}

//-------------------------------------------------------------------------------------------------
/// enable or disable the auto position for the specific window
/// @param  u8Enable            \b IN: enable or disable the auto position
/// @param  eWindow             \b IN: which window we are going to set
//-------------------------------------------------------------------------------------------------
void MApi_XC_AutoPositionEnable(MS_U8 u8Enable , SCALER_WIN eWindow)
{
    _XC_ENTRY();
    Hal_SC_ip_set_auto_position_function(u8Enable, eWindow);
    _XC_RETURN();
}

//-------------------------------------------------------------------------------------------------
/// set valid data threshold for the specific window
/// @param  u8Threshold         \b IN: the threshold to set
/// @param  eWindow             \b IN: which window we are going to set
//-------------------------------------------------------------------------------------------------
void MDrv_XC_SetValidDataThreshold(MS_U8 u8Threshold , SCALER_WIN eWindow)
{
    _XC_ENTRY();
    Hal_SC_ip_set_valid_data_threshold(u8Threshold, eWindow);
    _XC_RETURN();
}

//-------------------------------------------------------------------------------------------------
/// get auto position vstart for the specific window
/// @param  eWindow             \b IN: which window we are going to get
/// @return @ref MS_U16 the auto position vstart
//-------------------------------------------------------------------------------------------------
MS_U16 MApi_XC_SetAutoPositionVstart(SCALER_WIN eWindow)
{
    MS_U16 u16V_start;
    _XC_ENTRY();
    u16V_start = Hal_SC_ip_get_auto_position_v_start(eWindow);
    _XC_RETURN();
    return u16V_start ;//Hal_SC_ip_get_auto_position_v_start(eWindow);
}

//-------------------------------------------------------------------------------------------------
/// get auto position hstart for the specific window
/// @param  eWindow             \b IN: which window we are going to get
/// @return @ref MS_U16 the auto position hstart
//-------------------------------------------------------------------------------------------------
MS_U16 MApi_XC_SetAutoPositionHstart(SCALER_WIN eWindow)
{
    MS_U16 u16H_start;
    _XC_ENTRY();
    u16H_start = Hal_SC_ip_get_auto_position_h_start(eWindow);
    _XC_RETURN();
    return u16H_start;//Hal_SC_ip_get_auto_position_h_start(eWindow);
}

//-------------------------------------------------------------------------------------------------
/// get auto position vendt for the specific window
/// @param  eWindow             \b IN: which window we are going to get
/// @return @ref MS_U16 the auto position vend
//-------------------------------------------------------------------------------------------------
MS_U16 MApi_XC_SetAutoPositionVend(SCALER_WIN eWindow)
{
    MS_U16 u16V_end;
    _XC_ENTRY();
    u16V_end = Hal_SC_ip_get_auto_position_v_end(eWindow);
    _XC_RETURN();
    return u16V_end;//Hal_SC_ip_get_auto_position_v_end(eWindow);
}

//-------------------------------------------------------------------------------------------------
/// get auto position Horizontal end for the specific window
/// @param  eWindow             \b IN: which window we are going to get
/// @return @ref MS_U16 the auto position horizontal end
//-------------------------------------------------------------------------------------------------
MS_U16 MApi_XC_SetAutoPositionHend(SCALER_WIN eWindow)
{
    MS_U16 u16H_end;
    _XC_ENTRY();
    u16H_end = Hal_SC_ip_get_auto_position_h_end(eWindow);
    _XC_RETURN();
    return u16H_end;//Hal_SC_ip_get_auto_position_h_end(eWindow);
}

//-------------------------------------------------------------------------------------------------
/// set auto range window for the specific window
/// @param  u16Vstart             \b IN: vertical start
/// @param  u16Hstart             \b IN: horizontal start
/// @param  u16Vsize              \b IN: vertical size
/// @param  u16Hsize              \b IN: horizontal size
/// @param  eWindow             \b IN: which window we are going to get
//-------------------------------------------------------------------------------------------------
void MApi_XC_SetAutoRangeWindow(MS_U16 u16Vstart, MS_U16 u16Hstart, MS_U16 u16Vsize, MS_U16 u16Hsize, SCALER_WIN eWindow)
{
    Hal_SC_ip_set_auto_range_window(u16Vstart, u16Hstart, u16Vsize, u16Hsize, eWindow);
}

//-------------------------------------------------------------------------------------------------
/// enable or disable the auto range for the specific window
/// @param  u8Enable            \b IN: enable or disable the auto range
/// @param  eWindow             \b IN: which window we are going to set
//-------------------------------------------------------------------------------------------------
void MApi_XC_AutoRangeEnable(MS_U8 u8Enable , SCALER_WIN eWindow)
{
    Hal_SC_ip_set_auto_range_function(u8Enable, eWindow);
}

//-------------------------------------------------------------------------------------------------
/// enable or disable the Vsync polarity detect mode for the specific window
/// @param  u8Enable            \b IN: enable or disable
/// @param  eWindow             \b IN: which window we are going to set
//-------------------------------------------------------------------------------------------------
void MApi_XC_VsyncPolarityDetectMode(MS_U8 u8Enable , SCALER_WIN eWindow)
{
    _XC_ENTRY();
    Hal_SC_ip_set_reg_usr_vspolmd(u8Enable, eWindow);
    _XC_RETURN();
}

//-------------------------------------------------------------------------------------------------
/// init register for timing change
/// @param  eWindow             \b IN: which window we are going to set
//-------------------------------------------------------------------------------------------------
void MApi_XC_TimingChangeInit(SCALER_WIN eWindow)
{
    _XC_ENTRY();
    Hal_SC_ip_init_reg_in_timingchange(eWindow);
    _XC_RETURN();
}


void MDrv_XC_init_riu_base(MS_U32 u32riu_base, MS_U32 u32PMriu_base)
{
    Hal_SC_init_riu_base(u32riu_base, u32PMriu_base);
}

//-----------------------------------------------------------------------------
/// Enable/Disable Euro/Australia Hdtv Detection support in driver
/// @param  bEnable:
/// - ENABLE : enable support
/// - DISABLE : disable disable
/// @return None.
//-----------------------------------------------------------------------------
void MApi_XC_EnableEuroHdtvSupport(MS_BOOL bEnable)
{
    _bEnableEuro = bEnable;
}

MS_BOOL MApi_XC_Get_EuroHdtvSupport(void)
{
    return _bEnableEuro;
}

//-----------------------------------------------------------------------------
/// Enable Euro/Australia Hdtv Detection
/// @param  bEnable:
/// - ENABLE : enable detection
/// - DISABLE : disable detection
/// @return None.
//-----------------------------------------------------------------------------
void MApi_XC_EnableEuroHdtvDetection(MS_BOOL bEnable, SCALER_WIN eWindow)
{
    //MS_U8 u8Bank;
    MS_U32 u32Reg_1F_H, u32Reg_29_L;

    _XC_ENTRY();
    // switch to BK1F2
    //u8Bank = MDrv_ReadByte(BK_SELECT_00);

    if(eWindow == MAIN_WINDOW)
    {
        u32Reg_1F_H = REG_SC_BK01_1F_H;
        u32Reg_29_L = REG_SC_BK01_29_L;
    }
    else
    {
        u32Reg_1F_H = REG_SC_BK03_1F_H;
        u32Reg_29_L = REG_SC_BK03_29_L;
    }
    if(bEnable)
    {
        //(printf("Enable detecting EURO_HDTV\n");
        // set read HSync pulse width
        // old setting: we don't switch hsync/vsync-read in new chip because hsync & vsync are reported by different register.
        SC_W2BYTEMSK(u32Reg_1F_H, 0, BIT(5));// set read HSync pulse width instead of VSync pulse width
        SC_W2BYTEMSK(u32Reg_29_L, (BIT(7)|BIT(1)), 0x00FF);// All case, auto field
    }
    else
    {
        //printf("Disable detecting EURO_HDTV\n");
        Hal_SC_set_cs_det_cnt(0, eWindow);
        SC_W2BYTEMSK(u32Reg_29_L, 0x00, 0x00FF);// All case, auto field
    }
    _XC_RETURN();

    //MDrv_WriteByte(BK_SELECT_00, u8Bank);
}

//-----------------------------------------------------------------------------
/// Get Euro/Australia Hdtv Detection Result
/// @return MS_DETECT_EURO_HDTV_STATUS_TYPE:
/// - EURO_HDTV_DETECTED
/// - AUSTRALIA_HDTV_DETECTED
/// - NORMAL : No Euro/Australia HDTV detected.
//-----------------------------------------------------------------------------
MS_DETECT_EURO_HDTV_STATUS_TYPE MDrv_XC_ip_GetEuroHdtvStatus(SCALER_WIN eWindow)
{
    //MS_U8 u8Status;
    //MS_U8 u8Bank;
    MS_U16 u16Status;
    MS_DETECT_EURO_HDTV_STATUS_TYPE Euro_status;

    // switch to BK1F2
    //u8Bank = MDrv_ReadByte(BK_SELECT_00);

    if(eWindow == MAIN_WINDOW)
    {
        /*MDrv_WriteByte(BK_SELECT_00, REG_BANK_IP1F2);
        u8Status = MDrv_ReadByte(L_BK_IP1F2(0x29));*/
        u16Status = SC_R2BYTE(REG_SC_BK01_29_L);
    }
    else
    {
        /*MDrv_WriteByte(BK_SELECT_00, REG_BANK_IP1F1);
        u8Status = MDrv_ReadByte(L_BK_IP1F1(0x29));*/
        u16Status = SC_R2BYTE(REG_SC_BK03_29_L);
    }

    //if( (u8Status & 0x0C) == 0x0C )
    if( (u16Status & 0x000C) == 0x000C )
    {
        msg_sc_ip(printf("EURO_HDTV_DETECTED (0x%x)\n", u16Status));
        Euro_status = EURO_HDTV_DETECTED;
    }
    //else if( (u8Status & 0x0C) == 0x08 )
    else if( (u16Status & 0x000C) == 0x0008 )
    {
        msg_sc_ip(printf("AUSTRALIA_HDTV_DETECTED (0x%x)\n", u16Status));
        Euro_status = AUSTRALIA_HDTV_DETECTED;
    }
    else
    {
        msg_sc_ip(printf("NORMAL_HDTV_DETECTED (0x%x)\n", eWindow ? SC_R2BYTE(REG_SC_BK03_29_L) : SC_R2BYTE(REG_SC_BK01_29_L)));
        Euro_status = EURO_AUS_HDTV_NORMAL;
    }

    //MDrv_WriteByte(BK_SELECT_00, u8Bank);

    return Euro_status;
}

//-----------------------------------------------------------------------------
/// Euro/Australia Hdtv Detection Handler, this handler was executed if no input sync detected
/// and will try to detect if Euro/Australia/Normal input.
/// @param  eWindow             \b IN: which window we are going to set
/// @return None.
//-----------------------------------------------------------------------------
void MApi_XC_EuroHdtvHandler(SCALER_WIN eWindow)
{
    MS_U8 u8Bank;
    static MS_U8 u8Count = 1;
    //40ms delay in this function will cause ZUI MApp_ZUI_ACT_TransitionEffectDoing too slow.
    //in order to speed up ZUI MApp_ZUI_ACT_TransitionEffectDoing, we disable 40ms delay in this function
    //and let g_ucModeDetectPollingCounter in MApp_PCMode_TimingMonitor to handle the delay
    //MApp_PCMode_TimingMonitor will be called every 30ms while MDrv_Mode_DetectEuroHdtvHandler will be called every 60ms
    if(u8Count)
    {
        u8Count--;
        return;
    }
    _XC_ENTRY();
    u8Count = 1;
    // switch to BK1F2
    u8Bank = MDrv_ReadByte(BK_SELECT_00);

    // Toggle Composite Sync Separate Decision Count
    if( Hal_SC_get_cs_det_cnt(eWindow) == 0x00)
    {
        Hal_SC_set_cs_det_cnt(Hal_SC_get_plus_width(eWindow), eWindow);
    }
    else
    {
        _XC_RETURN();
        MDrv_XC_HWAuto_SyncSeparate(eWindow);
        _XC_ENTRY();
    }
    //printf("++++++++++++++++++++++++XC_MD_SYNC_LOSS<>:%x, %x\n",Hal_SC_get_cs_det_cnt(eWindow),Hal_SC_get_plus_width(eWindow));

    //MsOS_DelayTask(40);     // Delay till stable

    //printf("SepCnt=0x%bx\n", MDrv_ReadByte(H_BK_IP1F2(0x0C)));

    MDrv_WriteByte(BK_SELECT_00, u8Bank);

    _XC_RETURN();
    return;
}

//-----------------------------------------------------------------------------
// set composite sync separate decision count to 0:HW Auto Decide
// @return None.
//-----------------------------------------------------------------------------
void MDrv_XC_HWAuto_SyncSeparate(SCALER_WIN eWindow)
{
    MS_U8 u8Bank;
    _XC_ENTRY();

    // switch to BK1F2
    u8Bank = MDrv_ReadByte(BK_SELECT_00);

    // Toggle Composite Sync Separate Decision Count
    if( Hal_SC_get_cs_det_cnt(eWindow) != 0x00)
    {
        Hal_SC_set_cs_det_cnt(0x00, eWindow);

        if(eWindow == MAIN_WINDOW)
            Hal_SC_ip_software_reset(REST_IP_F2,eWindow);
        else
            Hal_SC_ip_software_reset(REST_IP_F1,eWindow);
    }

    MDrv_WriteByte(BK_SELECT_00, u8Bank);
    _XC_RETURN();

    return;
}

//-----------------------------------------------------------------------------
// set FilmMode_P
// @return None.
//-----------------------------------------------------------------------------
void MApi_XC_FilmMode_P(void)
{
    _XC_ENTRY();
    HAL_SC_FilmMode_Patch1();
    _XC_RETURN();
}

/// @brief Don't update wbank when MVOP pause, affetcs only in chips below:
/// (1)after T3 U04, (2)after Euclid U02, (3)T4, (4)T7, (5)Janus
/// @param[in] bEnable
void MApi_XC_set_FD_Mask(MS_BOOL bEnable, SCALER_WIN eWindow)
{
    MS_U32 u32Reg;

    u32Reg = eWindow == MAIN_WINDOW     ? REG_SC_BK01_26_L :
             eWindow == SUB_WINDOW      ? REG_SC_BK03_26_L :
             eWindow == SC1_MAIN_WINDOW ? REG_SC1_BK01_26_L :
             eWindow == SC2_MAIN_WINDOW ? REG_SC2_BK01_26_L :
             eWindow == SC2_SUB_WINDOW  ? REG_SC2_BK03_26_L :
                                          REG_SC_DUMMY;

    _XC_ENTRY();
 // if ( MDrv_ReadByte( 0x001ecf ) >= 0x03 )   //t3 u04 rev_id=0x03
    SC_W2BYTEMSK(u32Reg, (bEnable?BIT(8):0), BIT(8));
    _XC_RETURN();
}
MS_BOOL MApi_XC_set_GOP_Enable(MS_U8 MaxGOP ,MS_U8 UseNum, MS_U8 u8MuxNum, MS_BOOL bEnable)
{
    MS_BOOL bRet = FALSE;
    _XC_ENTRY();
    bRet = HAL_SC_set_GOP_Enable(MaxGOP , UseNum, u8MuxNum, bEnable);
    _XC_RETURN();
    return bRet;
}
void MApi_XC_ip_sel_for_gop(MS_U8 u8MuxNum ,MS_XC_IPSEL_GOP ipSelGop)
{
    _XC_ENTRY();
    HAL_SC_ip_sel_for_gop( u8MuxNum , ipSelGop);
    _XC_RETURN();
}


/******************************************************************************/
/// Set Scaler VOP New blending level
/******************************************************************************/
void MApi_XC_SetVOPNBL(void)
{
    _XC_ENTRY();
    HAL_SC_SetVOPNBL();
    _XC_RETURN();
}

MS_BOOL MApi_XC_GetDstInfo_IP1(MS_XC_DST_DispInfo *pDstInfo, MS_U32 u32SizeofDstInfo, SCALER_WIN eWindow)
{
    MS_BOOL bResult = FALSE;
    MS_WINDOW_TYPE capWin;

    if(NULL == pDstInfo)
    {
        msg_sc_ip(printf("MApi_XC_GetDstInfo_IP1():pDstInfo is NULL\n");)
        return FALSE;
    }

    if(u32SizeofDstInfo != sizeof(MS_XC_DST_DispInfo))
    {
        msg_sc_ip(printf("MApi_XC_GetDstInfo_IP1():u16SizeofDstInfo is different from the MS_XC_DST_DispInfo defined, check header file!\n");)
        return FALSE;
    }

    Hal_SC_ip_get_capture_window(&capWin, eWindow);
    pDstInfo->VDTOT = Hal_SC_ip_get_verticaltotal(eWindow);
    pDstInfo->HDTOT = 0;//cannot get this info
    pDstInfo->DEHST = capWin.x;
    pDstInfo->DEHEND= capWin.x + capWin.width - 1;
    pDstInfo->DEVST= capWin.y;
    pDstInfo->DEVEND= capWin.y + capWin.height - 1;
    pDstInfo->bInterlaceMode = Hal_SC_ip_get_interlace_status(eWindow);

    pDstInfo->bYUVInput = Hal_SC_get_csc(eWindow);
    bResult = TRUE;

    msg_sc_ip(printf("pDstInfo:bInterlaceMode=%x,HDTOT=%lx,VDTOT=%lx,DEHST=%lx,DEHEND=%lx,DEVST=%lx,DEVEND=%lx\n",
        pDstInfo->bInterlaceMode, pDstInfo->HDTOT, pDstInfo->VDTOT, pDstInfo->DEHST, pDstInfo->DEHEND,
        pDstInfo->DEVST, pDstInfo->DEVEND);)
    return bResult;

}

//------------------------------------------------------------------------------
/// XC IP0 Get Destination Information for GOP mixer
/// @return TRUE/FALSE
//------------------------------------------------------------------------------
MS_BOOL MApi_XC_GetDstInfo(MS_XC_DST_DispInfo *pDstInfo, MS_U32 u32SizeofDstInfo, EN_GOP_XCDST_TYPE XCDstType)
{
    MS_BOOL bResult = FALSE;
    _XC_ENTRY();

    if(NULL == pDstInfo)
    {
        msg_sc_ip(printf("MApi_XC_GetDstInfo_IP():pDstInfo is NULL\n");)
        bResult = FALSE;
    }else if(u32SizeofDstInfo != sizeof(MS_XC_DST_DispInfo))
    {
        msg_sc_ip(printf("MApi_XC_GetDstInfo_IP():u16SizeofDstInfo is different from the MS_XC_DST_DispInfo defined, check header file!\n");)
        bResult = FALSE;
    }else if(XCDstType >= MAX_GOP_XCDST_SUPPORT)
    {
        msg_sc_ip(printf("MApi_XC_GetDstInfo_IP():XCDstType(%x) is out of range!\n", XCDstType);)
        bResult = FALSE;
    }
    else
    {
        switch(XCDstType)
        {
            default:
            case E_GOP_XCDST_IP2:
                msg_sc_ip(printf("MApi_XC_GetDstInfo_IP():XCDstType(%x) is not supported temporarily!\n", XCDstType);)
                pDstInfo->bInterlaceMode = FALSE;
                pDstInfo->bYUVInput = FALSE;
                pDstInfo->HDTOT = 0;
                pDstInfo->VDTOT = 0;
                pDstInfo->DEHST = 0;
                pDstInfo->DEHEND= 0;
                pDstInfo->DEVST = 0;
                pDstInfo->DEVEND= 0;
                bResult = FALSE;
                break;

            case E_GOP_XCDST_IP1_MAIN:
                bResult = MApi_XC_GetDstInfo_IP1(pDstInfo, u32SizeofDstInfo, MAIN_WINDOW);
                break;

            case E_GOP_XCDST_IP1_SUB:
                bResult = MApi_XC_GetDstInfo_IP1(pDstInfo, u32SizeofDstInfo, SUB_WINDOW);
                break;
        }
    }
    _XC_RETURN();

    return bResult;
}

//-------------------------------------------------------------------------------------------------
/// Set IP1 Test Pattern
/// @param  eWindow          \b IN: IP1 Test Patter Enable
/// @param  eWindow          \b IN: IP1 Test Pattern Type
/// @param  eWindow          \b IN: which window(main or sub) to get
//-------------------------------------------------------------------------------------------------
void MApi_XC_SetIP1TestPattern(MS_U8 u8Enable, MS_U16 u6Pattern_type, SCALER_WIN eWindow)
{
    _XC_ENTRY();
    Hal_SC_ip_Set_TestPattern(u8Enable, u6Pattern_type, eWindow);
    _XC_RETURN();
}


void MApi_XC_InitRegInTimingChange_Post( MS_U8 u8CurrentSyncStatus )
{
    _XC_ENTRY();

    //if( MP_IS_COMP() ) // just for component
    {
        // For Master 925FS timing 221 (1080i)

        if ( u8CurrentSyncStatus & (BIT(2)|BIT(3)|BIT(4)))// sync loss | MD_INTERLACE_BIT) )
            HAL_SC_ip_detect_mode(ENABLE);
        else
            HAL_SC_ip_detect_mode(DISABLE);
    }

    _XC_RETURN();
}

MS_BOOL MApi_XC_Is_H_Sync_Active(SCALER_WIN eWindow)
{
    MS_BOOL bRet = FALSE;
    _XC_ENTRY();
    if(Hal_SC_ip_get_h_v_sync_active(eWindow) & 0x40)
    {
        bRet = TRUE;
    }
    else
    {
        bRet = FALSE;
    }
    _XC_RETURN();
    return bRet;
}

MS_BOOL MApi_XC_Is_V_Sync_Active(SCALER_WIN eWindow)
{
    MS_BOOL bRet = FALSE;
    _XC_ENTRY();
    if(Hal_SC_ip_get_h_v_sync_active(eWindow) & 0x80)
    {
        bRet = TRUE;
    }
    else
    {
        bRet = FALSE;
    }
    _XC_RETURN();
    return bRet;
}

MS_U8 MApi_XC_GetVSyncWidth(SCALER_WIN eWindow)
{
    MS_U8 u8VsyncWidth = 0;
    _XC_ENTRY();
    u8VsyncWidth = Hal_SC_getVSyncWidth(eWindow);
    _XC_RETURN();
    return u8VsyncWidth;
}

E_APIXC_ReturnValue MApi_XC_OutputFrameCtrl(MS_BOOL bEnable,XC_OUTPUTFRAME_Info *pstOutFrameInfo,SCALER_WIN eWindow)
{
    _XC_ENTRY();
    if(!bEnable)
    {
        gSrcInfo[eWindow].bRWBankAuto = TRUE;
        Hal_SC_set_skp_fd(FALSE);
        // if support _FIELD_PACKING_MODE_SUPPORTED, it will control by QMAP(J2/A5/A6/A3)
        #if(!_FIELD_PACKING_MODE_SUPPORTED)
        Hal_SC_set_wr_bank_mapping(0x00, eWindow);
        #endif
    }
    else
    {
        //When bRWBankAuto = FALSE, wr_bank_mapping must be controled by QMap
        gSrcInfo[eWindow].bRWBankAuto = FALSE;

        if(gSrcInfo[eWindow].bFBL)
        {
            Hal_SC_set_skp_fd(FALSE);
        }
        else
        {
            if(!pstOutFrameInfo->bInterlace) //progress
            {
                Hal_SC_set_skp_fd(FALSE);
            }
            else    //interlace
            {
                 if(pstOutFrameInfo->u16InVFreq > pstOutFrameInfo->u16OutVFreq)    //Drop frame case
                {
                    Hal_SC_set_skp_fd(TRUE);
                }
                else
                {
                    Hal_SC_set_skp_fd(FALSE);
                }
            }
        }
    }
    _XC_RETURN();

    return E_APIXC_RET_OK;
}

void MApi_XC_SetIPMux(MS_U8 u8Val)
{
    _XC_ENTRY();
    HAL_SC_ip_set_ipmux(u8Val);
    _XC_RETURN();
}

#undef  _MDRV_SC_IP_C_

