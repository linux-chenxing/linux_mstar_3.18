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
////////////////////////////////////////////////////////////////////////////////#include <stdio.h>
#define DRV_XC_CALIBRATION_C

// Common Definition
#ifdef MSOS_TYPE_LINUX_KERNEL
#include <linux/string.h>
#include <linux/wait.h>
#else
#include <string.h>
#endif
#include "MsCommon.h"
#include "MsIRQ.h"
#include "MsOS.h"
#include "mhal_xc_chip_config.h"

#include "xc_Analog_Reg.h"

#include "drvXC_IOPort.h"
#include "apiXC.h"
#include "mvideo_context.h"
#include "drv_xc_calibration.h"
#include "drv_sc_ip.h"
#include "drv_sc_scaling.h"
#include "mhal_sc.h"
#include "xc_hwreg_utility2.h"

#define STORE_W_LIMIT_UNIT_SIZE                   8UL
#define STORE_MEM_UNIT_SIZE                          8UL
#define STORE_MEM_ALIGN_SIZE                        8UL // 64 bits
#define CALIBRATION_DUMP_CAP_MEMORY         0
#define CALIBRATION_DUMP_STATISTIC             0
#define ENABLE_CALIB_SCREEN_MUTE                0
#define CALIBRATION_DELAY_TIME                     100
#define CALIBRATION_PRE_CAPTURE_DELAY       30


#define DBG_CAL_CAP(x)         //x
#define MIU_PROTECT             1
#define ENABLE_SC_MIU1       0


//Capture_Memory
#define FB_DBG(x)  //x
#define ADC_MEMAVG_WITHOUT_MAXMIN   0
#define ADC_READ_PATTERN_FROM_EXTERNAL_SIGNAL   0


// Get pixel data
void MApi_XC_Get_PixelData(MS_U16 u16CorX,  MS_U16 u16CorY, MS_PIXEL_32BIT* pixel)
{
    _XC_ENTRY();
    // Enable memory pixel capture function
    //MDrv_WriteByte( BK_SELECT_00, REG_BANK_DLC );
    //MDrv_WriteByteMask(L_BK_DLC(0x68), ENABLE, BIT(0));
    SC_W2BYTEMSK(REG_SC_BK1A_68_L,BIT(0),BIT(0));
    SC_W2BYTE(REG_SC_BK1A_69_L,u16CorX);
    SC_W2BYTE(REG_SC_BK1A_6A_L,u16CorY);
    //MDrv_Write2Byte(L_BK_DLC(0x69), u16CorX);
    //MDrv_Write2Byte(L_BK_DLC(0x6A), u16CorY);

    OS_DELAY_TASK(20);

    /*pixel->G_Y  =   MDrv_Read2Byte(L_BK_DLC(0x6B)) & 0x03FF;
    pixel->B_Cb =   MDrv_Read2Byte(L_BK_DLC(0x6C)) & 0x03FF;
    pixel->R_Cr =   MDrv_Read2Byte(L_BK_DLC(0x6D)) & 0x03FF;*/
    pixel->G_Y  =   SC_R2BYTE(REG_SC_BK1A_6B_L)& 0x03FF;
    pixel->B_Cb =   SC_R2BYTE(REG_SC_BK1A_6C_L) & 0x03FF;
    pixel->R_Cr =   SC_R2BYTE(REG_SC_BK1A_6D_L) & 0x03FF;

    //MDrv_WriteByteMask(L_BK_DLC(0x68), DISABLE, BIT(0));
    SC_W2BYTEMSK(REG_SC_BK1A_68_L,0,BIT(0));
    _XC_RETURN();
}


//-------------------------------------------------------------------------------------------------
/// Get Average Data
/// @param  sMemBuf          \b IN: the buffer of memory
/// @param  pCalWin          \b IN: the window position and size of the memory
/// @return @ref MS_AUTOADC_TYPE
//-------------------------------------------------------------------------------------------------
MS_AUTOADC_TYPE MApi_XC_GetAverageData(MS_PIXEL_32BIT *sMemBuf, MS_AUTO_CAL_WINDOW_t pCalWin)
{
    MS_AUTOADC_TYPE ptAdcData;
    MS_U32 u32Rtt, u32Gtt, u32Btt;
    MS_U16 i, x, y;
#if ADC_MEMAVG_WITHOUT_MAXMIN
    MS_U32 tmp;
    MS_U16 u16MaxR,u16MaxG,u16MaxB;
    MS_U16 u16MinR,u16MinG,u16MinB;
    MS_U16 u16NumMaxR,u16NumMaxG,u16NumMaxB;
    MS_U16 u16NumMinR,u16NumMinG,u16NumMinB;
    MS_U16 u16NoMinMaxAvgCb, u16NoMinMaxAvgY, u16NoMinMaxAvgCr;
#endif
	MS_WINDOW_TYPE stTmpWin;

    ptAdcData.u16CH_AVG[0] = ptAdcData.u16CH_AVG[1] = ptAdcData.u16CH_AVG[2] = 0;

    i = 0;
    u32Rtt = 0;
    u32Gtt = 0;
    u32Btt = 0;

#if ADC_MEMAVG_WITHOUT_MAXMIN
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

#if ADC_READ_PATTERN_FROM_EXTERNAL_SIGNAL

    for(y=pCalWin.u16Y; y<pCalWin.u16Y+pCalWin.u16Ysize; y++)
    {
        for(x=pCalWin.u16X; x<pCalWin.u16X+pCalWin.u16Xsize; x++)
        {

            MS_PIXEL_32BIT pix;
            MDrv_XC_Get_Pixel(x,y,&pix);

            //printf("Y = 0x%04x, Cb = 0x%04x, Cr = 0x%04x\n", u16Y, u16Cb, u16Cr);
            u32Rtt += pix.R_Cr;
            u32Gtt += pix.G_Y;
            u32Btt += pix.B_Cb;
        }
    }
    // Disable Debug Cross
    //MDrv_WriteByte( BK_SELECT_00, REG_BANK_DLC );
    //MDrv_WriteByteMask(L_BK_DLC(0x68), DISABLE, BIT(0));
    SC_W2BYTEMSK(REG_SC_BK1A_68_L,0,BIT(0));
    ptAdcData.u16CH_AVG[0] = u32Rtt / (pCalWin.u16Ysize * pCalWin.u16Xsize);
    ptAdcData.u16CH_AVG[1] = u32Gtt / (pCalWin.u16Ysize * pCalWin.u16Xsize);
    ptAdcData.u16CH_AVG[2] = u32Btt / (pCalWin.u16Ysize * pCalWin.u16Xsize);

    FB_DBG(printf("AvgCr AvgY AvgCb\n"));
    FB_DBG(printf("%04d  %04d %04d\n\n", ptAdcData.u16CH_AVG[0], ptAdcData.u16CH_AVG[1], ptAdcData.u16CH_AVG[2]));


#else
    FB_DBG(printf("Mem Start (%d, %d) size %dx%d\n", pCalWin.u16X, pCalWin.u16Y, pCalWin.u16Xsize, pCalWin.u16Ysize));
    FB_DBG(printf("u32RectSize=%u, u16PointSize=%u\n", pCalWin.u16Ysize * pCalWin.u16Xsize * sizeof(MS_PIXEL_32BIT), sizeof(MS_PIXEL_32BIT)));

    //MS_WINDOW_TYPE stTmpWin;
    stTmpWin.x = pCalWin.u16X;
    stTmpWin.y = pCalWin.u16Y;
    stTmpWin.width = pCalWin.u16Xsize;
    stTmpWin.height = pCalWin.u16Ysize;

    MApi_XC_Get_BufferData(E_XC_OUTPUTDATA_RGB10BITS, &stTmpWin, sMemBuf, MAIN_WINDOW);

    for(y=pCalWin.u16Y; y<(pCalWin.u16Y+pCalWin.u16Ysize); y++)
    {
        for(x=pCalWin.u16X; x<(pCalWin.u16X+pCalWin.u16Xsize); x++)
        {
#if ADC_MEMAVG_WITHOUT_MAXMIN
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
            u32Rtt += sMemBuf[i].R_Cr;
            u32Gtt += sMemBuf[i].G_Y;
            u32Btt += sMemBuf[i].B_Cb;

            //printf("(%3ld,%3ld) Rdata=%u, Gdata=%u, Bdata=%u\n",x,y,sMemBuf[i].R_Cr,sMemBuf[i].G_Y, sMemBuf[i].B_Cb);
            FB_DBG(printf("Rtt=%lu, Gtt=%lu, Btt=%lu\n",u32Rtt,u32Gtt,u32Btt));
            i++;
        }
    }


#if ADC_MEMAVG_WITHOUT_MAXMIN
    tmp = u32Rtt - ((U32)u16MaxR * (U32)u16NumMaxR) - ((U32)u16MinR * (U32)u16NumMinR);
    if ( tmp != 0 )
    {
        u16NoMinMaxAvgCr = tmp / ((pCalWin.u16Ysize * pCalWin.u16Xsize) - (u16NumMaxR + u16NumMinR));
    }

    tmp = u32Gtt - ((U32)u16MaxG * (U32)u16NumMaxG) - ((U32)u16MinG * (U32)u16NumMinG);
    if ( tmp != 0 )
    {
        u16NoMinMaxAvgY = tmp / ((pCalWin.u16Ysize * pCalWin.u16Xsize) - (u16NumMaxG + u16NumMinG));
    }

    tmp = u32Btt - ((U32)u16MaxB * (U32)u16NumMaxB) - ((U32)u16MinB * (U32)u16NumMinB);
    if ( tmp != 0 )
    {
        u16NoMinMaxAvgCb = tmp / ((pCalWin.u16Ysize * pCalWin.u16Xsize) - (u16NumMaxB + u16NumMinB));
    }
#endif

    ptAdcData.u16CH_AVG[0] = (MS_U16)(u32Rtt / (pCalWin.u16Ysize * pCalWin.u16Xsize));
    ptAdcData.u16CH_AVG[1] = (MS_U16)(u32Gtt / (pCalWin.u16Ysize * pCalWin.u16Xsize));
    ptAdcData.u16CH_AVG[2] = (MS_U16)(u32Btt / (pCalWin.u16Ysize * pCalWin.u16Xsize));

    //printf("ptAdcData[RGB][%04d,%04d,%04d]\n",ptAdcData.u16CH_AVG[0],ptAdcData.u16CH_AVG[1],ptAdcData.u16CH_AVG[2]) ;

#if ADC_MEMAVG_WITHOUT_MAXMIN
    printf("MaxCr    MinCr    MaxY    MinY    MaxCb    MinCb    AvgCr AvgY AvgCb\n");
    printf("%04d     %04d     %04d    %04d    %04d     %04d     %04d  %04d %04d\n",
        u16MaxR, u16MinR, u16MaxG, u16MinG, u16MaxB, u16MinB, ptAdcData.u16CH_AVG[0], ptAdcData.u16CH_AVG[1], ptAdcData.u16CH_AVG[2]);

    printf("NumMaxCr NumMinCr NumMaxY NumMinY NumMaxCb NumMinCb AvgCr AvgY AvgCr\n");
    printf("%04d     %04d     %04d    %04d    %04d     %04d     %04d  %04d %04d\n\n",
        u16NumMaxR, u16NumMinR, u16NumMaxG, u16NumMinG, u16NumMaxB, u16NumMinB, u16NoMinMaxAvgCr, u16NoMinMaxAvgY, u16NoMinMaxAvgCb);
#endif

#endif

    return ptAdcData;
}

// TODO: Implemant Later
#if 0
void MDrv_Calibration_SetMirror( MS_BOOL bOnOff )
{
    MS_U8  u8Bank;
    MS_BOOL bInputEnable;


    MDrv_WriteByte(BK_SELECT_00, REG_BANK_IP1F2);
    bInputEnable = (MDrv_ReadRegBit( BK_SC_IP1F2_02_L, _BIT7)) ? TRUE : FALSE;

#if(ENABLE_CALIB_SCREEN_MUTE)
    if( bInputEnable )
        MApi_XC_DisableInputSource(DISABLE);
#endif

    if(bOnOff)
    {
        g_SrcInfo.u8SCStatus |= (SCSTATUS_H_MIRROR | SCSTATUS_V_MIRROR);
    }
    else
    {
        g_SrcInfo.u8SCStatus &= ~(SCSTATUS_H_MIRROR | SCSTATUS_V_MIRROR);
    }

    MDrv_Scaler_SetScalingRatio(gtSysVar.mbMpSrc, &g_DisplayWindowSetting);

    MDrv_Scaler_SW_DB();

#if(ENABLE_CALIB_SCREEN_MUTE)
    if( bInputEnable )
        MApi_XC_DisableInputSource(ENABLE);
#endif
}
#endif
#if 0 // TODO: Implement Later
static void MDrv_Calibration_SetSCMiu(MS_U8 u8miuNum)
{
    // enable freeze
    Hal_SC_set_freezeimg(ENABLE, MAIN_WINDOW);
    g_bDoCalibration = ((u8miuNum == 0) ? TRUE : FALSE);

    MDrv_MIU_ChangeScalerMIUNumber(u8miuNum);

    MDrv_SC_set_fetch_number_limit( &gSrcInfo[MAIN_WINDOW], MAIN_WINDOW );
    MDrv_SC_sw_db( &stDBreg, MAIN_WINDOW );

    // disable freeze
    Hal_SC_set_freezeimg(DISABLE, MAIN_WINDOW);
    MsOS_DelayTask(CALIBRATION_DELAY_TIME); //change MIU delay to avoid garbage image
}
#endif

#if ( defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_NOS) )
#define	MAX_u16Xsize	512
#define MAX_u16Ysize	512
#endif
void MApi_XC_ReadRgbData( MS_IMAGE_MEM_INFO * pMemInfo)
{
#if ( defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_NOS) )
	MS_PIXEL_32BIT sMemBuf[MAX_u16Xsize*MAX_u16Ysize];
#else
    MS_PIXEL_32BIT sMemBuf[(pMemInfo->eCal_Win_Size.u16Xsize)*(pMemInfo->eCal_Win_Size.u16Ysize)];
#endif
    memset(&sMemBuf, 0 , sizeof(sMemBuf) );
    FB_DBG(printf("x=%u, y=%u, xsize=%u, ysize=%u\n",pMemInfo->eCal_Win_Size.u16X,pMemInfo->eCal_Win_Size.u16Y,
        pMemInfo->eCal_Win_Size.u16Xsize,pMemInfo->eCal_Win_Size.u16Ysize));
    Hal_SC_set_freezeimg(ENABLE, MAIN_WINDOW);

    pMemInfo->pixelAverage = MApi_XC_GetAverageData(sMemBuf, pMemInfo->eCal_Win_Size);
    Hal_SC_set_freezeimg(DISABLE, MAIN_WINDOW);
}

void MApi_XC_ReadVOPData( MS_IMAGE_MEM_INFO * pMemInfo)
{
#if ( defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_NOS) )
	MS_PIXEL_32BIT sMemBuf[MAX_u16Xsize*MAX_u16Ysize];
#else
    MS_PIXEL_32BIT sMemBuf[(pMemInfo->eCal_Win_Size.u16Xsize)*(pMemInfo->eCal_Win_Size.u16Ysize)];
#endif
    memset(&sMemBuf, 0 , sizeof(sMemBuf) );
    FB_DBG(printf("x=%u, y=%u, xsize=%u, ysize=%u\n",pMemInfo->eCal_Win_Size.u16X,pMemInfo->eCal_Win_Size.u16Y,
        pMemInfo->eCal_Win_Size.u16Xsize,pMemInfo->eCal_Win_Size.u16Ysize));
    Hal_SC_set_freezeimg(ENABLE, MAIN_WINDOW);
    MApi_XC_Rerout_for_Capture_VOP_Data();
    pMemInfo->pixelAverage = MApi_XC_GetAverageData(sMemBuf, pMemInfo->eCal_Win_Size);
    Hal_SC_set_freezeimg(DISABLE, MAIN_WINDOW);

}


