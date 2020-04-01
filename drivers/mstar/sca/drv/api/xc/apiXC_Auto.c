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
/// file    apiXC_Auto.c
/// @brief  Scaler API layer Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#define _APIXC_AUTO_C_

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#if defined(MSOS_TYPE_LINUX_KERNEL)
#include <linux/kernel.h>
#endif
// Common Definition
#include "MsCommon.h"
#include "MsOS.h"

// Driver
#include "drvXC_IOPort.h"

// Api
#include "apiXC.h"
#include "apiXC_Adc.h"
#include "apiXC_Auto.h"
#include "apiXC_PCMonitor.h"

// Driver
#include "mvideo_context.h"
#include "drvXC_ADC_Internal.h"
#include "drv_sc_ip.h"
#include "drv_sc_scaling.h"

//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------
#ifdef MSOS_TYPE_CE
#define XC_REG
#endif
#define AUTOMSG(x)          //x

#define COMPONENT_AUTO_SW_MODE  FALSE
#define AUTO_PHASE_METHOD       0       // 0 = found maximum
#define GET_RGB_REPORT          DISABLE
#define FAST_AUTO_TUNE          1
#define AUTO_GAIN_SW_MODE       0
#define AUTO_PHASE_STEP         4

#define AUTO_YUV_GAIN_STEP      1
#define AUTO_YUV_MAX_CNT        30

#define ENABLE_VGA_EIA_TIMING   0
//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

#define Y_AUTO_COLOR_75IRE          0   // 0: 100IRE / 1: 75IRE
#define PbPr_AUTO_COLOR_75IRE       0   // 0: 100IRE / 1: 75IRE

#if Y_AUTO_COLOR_75IRE
#define Y_MAX_VALUE                 180 // For 75IRE Y value
#define Y_MIN_VALUE                 16
#else
#define Y_MAX_VALUE                 235 // For 100IRE Y value
#define Y_MIN_VALUE                 16
#endif

#if PbPr_AUTO_COLOR_75IRE
#define PbPr_MAX_VALUE              212 // For 75IRE PbPr value
#define PbPr_MIN_VALUE              44
#else
#define PbPr_MAX_VALUE              240 // For 100IRE PbPr value
#define PbPr_MIN_VALUE              16
#endif

#define Y_AUTO_ACTIVE_RANGE         (Y_MAX_VALUE - Y_MIN_VALUE)
#define PbPr_AUTO_ACTIVE_RANGE      (PbPr_MAX_VALUE - PbPr_MIN_VALUE)
#define Y_AUTO_MIN_VALUE            (Y_MIN_VALUE - 16)
#define PbPr_AUTO_MIN_VALUE         (PbPr_MIN_VALUE)

#define MIN_PC_AUTO_H_START            (StandardInfo->u16HorizontalStart - (StandardInfo->u16HorizontalStart * 8 / 10) )

#if ENABLE_VGA_EIA_TIMING
  #define MAX_PC_AUTO_H_START            (StandardInfo->u16HorizontalStart + (StandardInfo->u16HorizontalStart* 18 / 10) )
#else
  #define MAX_PC_AUTO_H_START            (StandardInfo->u16HorizontalStart + (StandardInfo->u16HorizontalStart* 8 / 10) )
#endif

#define MIN_PC_AUTO_V_START            (1)
#define MAX_PC_AUTO_V_START            (StandardInfo->u16VerticalStart*2 - MIN_PC_AUTO_V_START/*MIN_PC_V_START*/)

/*!
    1. RGB mode, full scale還是維持10bit 1023這個code, 所以gain 還是校正到讀到0.5/0.7*1023.
       Offset也是校正到 0這個code, RGB三個channel 都一樣.
    2. YCbCr mode, 請將Y的full scale改成10bit code 是(235-16)*4, Cb/Cr full scale 改成10bit (240-16)*4.
       所以Y gain值要調到使 digital code 看到 0.5/0.7*(235-16)*4, Cb/Cr則是0.5/0.7*(240-16)*4.
       Offset 則是將 Y 較準到64 (10bit), Cb/Cr 到512 (10bit).
*/

#define   AUTO_GAIN_RANGE_RGB_INTERNAL       731   //0.5/0.7*1023=730.7
#define   AUTO_GAIN_RANGE_Y_INTERNAL         626   //(235-16)*4 *0.5/0.7,
#define   AUTO_GAIN_RANGE_CBCR_INTERNAL      640   //(240-16)*4 *0.5/0.7,

#define   AUTO_GAIN_RANGE_RGB_EXTERNAL       1018  //Find gain setting from 1018. Than fine tune to 1023
#define   AUTO_GAIN_RANGE_Y_EXTERNAL         876   //(235-16)*4,
#define   AUTO_GAIN_RANGE_CBCR_EXTERNAL      896   //(240-16)*4,


#define   AUTO_GAIN_BONDARY           1

#define   AUTO_GAIN_MAX_RGB         (AUTO_GAIN_RANGE_RGB_INTERNAL+AUTO_GAIN_BONDARY )
#define   AUTO_GAIN_MIN_RGB         (AUTO_GAIN_RANGE_RGB_INTERNAL-AUTO_GAIN_BONDARY )

#define   AUTO_GAIN_MAX_Y           (AUTO_GAIN_RANGE_Y_INTERNAL+AUTO_GAIN_BONDARY )
#define   AUTO_GAIN_MAX_CBCR        (AUTO_GAIN_RANGE_CBCR_INTERNAL+AUTO_GAIN_BONDARY )
#define   AUTO_GAIN_MIN_Y           (AUTO_GAIN_RANGE_Y_INTERNAL-AUTO_GAIN_BONDARY )
#define   AUTO_GAIN_MIN_CBCR        (AUTO_GAIN_RANGE_CBCR_INTERNAL-AUTO_GAIN_BONDARY )

#define   AUTO_OFFSET_Y             0x40
#define   AUTO_OFFSET_CBCR          0x200

//-------------------------------------------------------------------------------------------------
//  Local Structurs
//-------------------------------------------------------------------------------------------------

typedef struct
{
      MS_U8   L_BkAtop_01;
      MS_U8   L_BkAtop_0C;
      MS_U8   L_BkAtop_2C;
      MS_U16  L_BkAtop_05;
      MS_U16  L_BkAtop_06;
      MS_U8   L_BkAtop_1C;
      MS_U8   L_BkAtop_1F;

      MS_U16 BkDtop_01;
      MS_U16 BkDtop_02;
      MS_U8   L_BkDtop_06;
      MS_U8   H_BkChipTop_1F;
      MS_U8   L_BkIpMux_1;
      MS_U16 SC_BK1_02;
      MS_U16 SC_BK1_03;
      MS_U16 SC_BK1_04;
      MS_U16 SC_BK1_05;
      MS_U16 SC_BK1_06;
      MS_U16 SC_BK1_07;
      MS_U16 SC_BK1_0E;
      MS_U8   L_SC_BK1_21;
      MS_U8   L_SC_BK10_19;
} MS_AUTOADC_REG_BAK;

typedef enum
{
    E_XC_AUTO_GET_HSTART,
    E_XC_AUTO_GET_HEND,
    E_XC_AUTO_GET_VSTART,
    E_XC_AUTO_GET_VEND,
}XC_Auto_GetAutoPositionType;

typedef enum
{
    E_WAITING_AUTO_POSITION_READY,
    E_WAITING_AUTO_GAIN_READY,
}XC_Auto_WaitingType;

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
static MS_BOOL bHaltAutoGeometry = FALSE;
static MS_U8 _u8ThresholdValue = 0x05;
//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
static void MApi_XC_Auto_SetCalibrartinWindow(MS_BOOL bEnable, SCALER_WIN scwin )
{
    //set auto range window for calibrartin
    MS_U16 Vcut,Hcut;
    MS_WINDOW_TYPE cap_win;

    if (bEnable)
    {
        MApi_XC_GetCaptureWindow(&cap_win,scwin );

        Vcut = cap_win.height/ 10;
        Hcut = cap_win.width/ 10;

        MApi_XC_AutoRangeEnable(ENABLE, scwin);

        MApi_XC_SetAutoRangeWindow( cap_win.y + (Vcut/2),
                                   cap_win.x + (Hcut/2),
                                   cap_win.height- Vcut,
                                   cap_win.width- Hcut,
                                   scwin
                                   );
    }
    else
    {
        MApi_XC_AutoRangeEnable(DISABLE, scwin);
    }

    //printf("capture: %d %d %d %d \n",Vstart, Hstart, Vsize , Hsize );

}

#if 0

static void _MApi_XC_Auto_ResetGainOffset(void)
{
    APIXC_AdcGainOffsetSetting stGainOffset = {0x80, 0x80, 0x80, 0x80, 0x80, 0x80};

    MApi_XC_ADC_AdjustGainOffset(&stGainOffset);
}

/////////////////////////////////////////////////////
// gain setting: 00 is maximum ~~ 0xff is minmum
/////////////////////////////////////////////////////
static MS_BOOL _MApi_XC_Auto_AutoGain( E_XC_AUTO_AutoTuneType enAutoTuneType, APIXC_AdcGainOffsetSetting *pstADCSetting  )
{
    MS_AUTOADC_TYPE pAutoAdc_06;
    MS_AUTOADC_TYPE pAutoAdc_01;
    MS_U16 u16Diff;
    //MS_U16 u16Average0_6,u16Average0_1;
    MS_U8   CheckFlag=0x00;
    MS_U8 GainOffset_R=0x40;
    MS_U8 GainOffset_G=0x40;
    MS_U8 GainOffset_B=0x40;

    MS_U8 Gain_R=0x80;
    MS_U8 Gain_G=0x80;
    MS_U8 Gain_B=0x80;

    if (enAutoTuneType & E_XC_AUTO_TUNE_RGB_GAIN)
    {
        AutoGainMax_YG=AUTO_GAIN_MAX_RGB;
        AutoGainMax_BR=AUTO_GAIN_MAX_RGB;
        AutoGainMin_YG=AUTO_GAIN_MIN_RGB;
        AutoGainMin_BR=AUTO_GAIN_MIN_RGB;
    }
    else
    {
        AutoGainMax_YG=AUTO_GAIN_MAX_Y;
        AutoGainMax_BR=AUTO_GAIN_MAX_CBCR;
        AutoGainMin_YG=AUTO_GAIN_MIN_Y;
        AutoGainMin_BR=AUTO_GAIN_MIN_CBCR;
    }

    AUTOMSG( printf("\nY=%d ~%d\n",AutoGainMin_YG,AutoGainMax_YG));
    AUTOMSG (printf("Cbcr=%d ~%d\n\n",AutoGainMin_BR,AutoGainMax_BR));


    while( GainOffset_R &&  GainOffset_G && GainOffset_B )
    {
        #if ( WATCH_DOG == ENABLE )
        MDrv_Sys_ClearWatchDog();
        #endif
        MApi_XC_ADC_SetInternalVoltage(INTERNAL_0_6V);

        pAutoAdc_06 = MApi_XC_AverageDataInfo();  //get 0.6V average data

        AUTOMSG(printf("%04x %04x %04x \n",pAutoAdc_06.R_CH_AVG,pAutoAdc_06.G_CH_AVG,pAutoAdc_06.B_CH_AVG));
        MApi_XC_ADC_SetInternalVoltage(INTERNAL_0_1V);

        pAutoAdc_01 = MApi_XC_AverageDataInfo();  //get 0.6V average data
        AUTOMSG( printf("%04x %04x %04x\n\n",pAutoAdc_01.R_CH_AVG,pAutoAdc_01.G_CH_AVG,pAutoAdc_01.B_CH_AVG));

        if ( (CheckFlag & BIT(0)) == 0x00 )
        {
              u16Diff = pAutoAdc_06.R_CH_AVG - pAutoAdc_01.R_CH_AVG;
              AUTOMSG(printf("cr_diff=%d\n",u16Diff));
              if ( (u16Diff>AutoGainMin_BR ) && (u16Diff<AutoGainMax_BR ) )
              {
                 CheckFlag|=BIT(0);
                AUTOMSG( printf("R--OK--%02bx\n",Gain_R));
              }
              else
              {
                    if   (u16Diff<= AutoGainMin_BR )
                    {
                        Gain_R-=GainOffset_R;
                    }
                    else
                    {
                        Gain_R+=GainOffset_R;
                    }
                    GainOffset_R=GainOffset_R>>1;
              }
              MApi_XC_ADC_SetGain(ADC_CHR, Gain_R);
         }
        if ( (CheckFlag & BIT(1)) == 0x00 )
        {

            u16Diff = pAutoAdc_06.G_CH_AVG - pAutoAdc_01.G_CH_AVG;
            AUTOMSG(printf("G_diff=%d\n",u16Diff));

            if ( (u16Diff>AutoGainMin_YG ) && (u16Diff<AutoGainMax_YG ) )
            {
                CheckFlag|=BIT(1);
                AUTOMSG(printf("G--OK--%02bx\n",Gain_G));
            }
            else
            {
                if   (u16Diff<= AutoGainMin_YG )
                {
                    Gain_G -=GainOffset_G;
                }
                else   //(u16Diff>AUTO_GAIN_MAX )
                {
                    Gain_G +=GainOffset_G;
                }
                GainOffset_G=GainOffset_G>>1;
            }
            MApi_XC_ADC_SetGain(ADC_CHG, Gain_G);
        }
        if ( (CheckFlag & BIT(2)) == 0x00 )
        {
            u16Diff = pAutoAdc_06.B_CH_AVG - pAutoAdc_01.B_CH_AVG;
            AUTOMSG(printf("cb_diff=%d\n",u16Diff));
            if ( (u16Diff>AutoGainMin_BR ) && (u16Diff<AutoGainMax_BR ) )
            {
                CheckFlag|=BIT(2);
                AUTOMSG(printf("B--OK--%02bx\n",Gain_B));
            }
            else
            {
                if   (u16Diff<= AutoGainMin_BR )
                {
                    Gain_B -=GainOffset_B;
                }
                else   //(u16Diff>AUTO_GAIN_MAX )
                {
                    Gain_B +=GainOffset_B;
                }
                GainOffset_B=GainOffset_B>>1;
            }
            MApi_XC_ADC_SetGain(ADC_CHB, Gain_B);
        }
        if ( (CheckFlag & 0x07 )== 0x07 )
        {
            pstADCSetting->u8RedGain=(~Gain_R);
            pstADCSetting->u8GreenGain=(~Gain_G);
            pstADCSetting->u8BlueGain=(~Gain_B);
            AUTOMSG(printf("gain check ok  %x %x %x\n",Gain_R,Gain_G,Gain_B));
            return(TRUE );
        }
        MsOS_DelayTask(20);
    }
    AUTOMSG (printf("gain adj fail= %02bx\n",CheckFlag) );
    return( FALSE);
}


static MS_BOOL _MApi_XC_Auto_AutoOffset( E_XC_AUTO_AutoTuneType enAutoTuneType, APIXC_AdcGainOffsetSetting *pstADCSetting )
{
    MS_U8 DcOffset_R=0xFF;
    MS_U8 DcOffset_G=0xFF;
    MS_U8 DcOffset_B=0xFF;
    MS_U8 Delta=0x80;
    MS_U8 CheckFlag=0x00;
    MS_AUTOADC_TYPE pAutoAdc_0V;
    MS_U16  OffsetHigh, OffsetLow;
    MS_U16  G_OffsetHigh, G_OffsetLow;

    MApi_XC_ADC_SetInternalVoltage(INTERNAL_0V);

    MApi_XC_ADC_SetOffset(ADC_CHR, DcOffset_R);
    MApi_XC_ADC_SetOffset(ADC_CHG, DcOffset_G);
    MApi_XC_ADC_SetOffset(ADC_CHB, DcOffset_B);

    CheckFlag=0x00;

    if (enAutoTuneType & E_XC_AUTO_TUNE_RGB_OFFSET)
    {
        //if RGB
        OffsetHigh=AUTO_GAIN_BONDARY;
        OffsetLow= 0x00;
        G_OffsetHigh=AUTO_GAIN_BONDARY;
        G_OffsetLow=0x00;
    }
    else
    {
        //if YUV
        OffsetHigh=AUTO_OFFSET_CBCR+(AUTO_GAIN_BONDARY);
        OffsetLow= AUTO_OFFSET_CBCR-(AUTO_GAIN_BONDARY);
        G_OffsetHigh=AUTO_OFFSET_Y+AUTO_GAIN_BONDARY;
        G_OffsetLow= AUTO_OFFSET_Y-AUTO_GAIN_BONDARY;
    }
    MsOS_DelayTask(20);  //** must delay n Vsync

    while( Delta > 0 )   //Get a offset that the ADC is > 0
    {

        pAutoAdc_0V = MApi_XC_AverageDataInfo();
        AUTOMSG(printf("%04x %04x %04x\n\n",pAutoAdc_0V.R_CH_AVG,pAutoAdc_0V.G_CH_AVG,pAutoAdc_0V.B_CH_AVG));
        if ( (CheckFlag & BIT(0))==0x00 )
        {
            if ( ( pAutoAdc_0V.R_CH_AVG > OffsetLow) &&  ( pAutoAdc_0V.R_CH_AVG < OffsetHigh ) )
            {
                CheckFlag |= BIT(0);  //
                AUTOMSG(printf("R-offset -ok---%02bx\n",DcOffset_R));
            }
            else
            {
                if ( pAutoAdc_0V.R_CH_AVG <= OffsetLow )  // == 0 )
                {
                    DcOffset_R=DcOffset_R-Delta;
                }
                else
                {
                    DcOffset_R=DcOffset_R+Delta;
                }
                MApi_XC_ADC_SetOffset(ADC_CHR, DcOffset_R);
            }
       }
       if ( (CheckFlag & BIT(1))==0x00 )
       {
            if ( ( pAutoAdc_0V.G_CH_AVG > G_OffsetLow ) &&  ( pAutoAdc_0V.G_CH_AVG < G_OffsetHigh ) )
            {
                CheckFlag |= BIT(1);  //
                AUTOMSG(printf("G-offset -ok---%02bx\n",DcOffset_G));
            }
            else
            {
                if ( pAutoAdc_0V.G_CH_AVG <= G_OffsetLow )
                {
                    DcOffset_G=DcOffset_G-Delta;
                }
                else
                {
                    DcOffset_G=DcOffset_G+Delta;
                }
                AUTOMSG(printf("DcOffset_G=%04x\n",DcOffset_G ));
                MApi_XC_ADC_SetOffset(ADC_CHG, DcOffset_G);
            }
       }

        if ( (CheckFlag & BIT(2))==0x00 )
        {
            if ( ( pAutoAdc_0V.B_CH_AVG > OffsetLow) &&  ( pAutoAdc_0V.B_CH_AVG < OffsetHigh ) )
            {
                CheckFlag |= BIT(2);  //
                AUTOMSG(printf("B-offset -ok---%02bx\n",DcOffset_B));
            }
            else
            {
                if ( ( pAutoAdc_0V.B_CH_AVG <= OffsetLow )    )
                {
                    DcOffset_B=DcOffset_B-Delta;
                }
                else
                {
                    DcOffset_B=DcOffset_B+Delta;
                }
                MApi_XC_ADC_SetOffset(ADC_CHB, DcOffset_B);
            }
        }
        if (   ( CheckFlag & 0x07 ) == 0x07 )
        {
            pstADCSetting->u8RedOffset=~DcOffset_R;
            pstADCSetting->u8GreenOffset=~DcOffset_G;
            pstADCSetting->u8BlueOffset=~DcOffset_B;
            AUTOMSG(printf("offset check ok  %x %x %x\n",DcOffset_R,DcOffset_G,DcOffset_B));
            return TRUE;
        }
        Delta=Delta>>1;
        MsOS_DelayTask(20);  //** must delay n Vsync
    }
    AUTOMSG(printf("offset check fail\n"));
    return FALSE;
}


//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
static void MDrv_BackupAutoAdcReg(MS_AUTOADC_REG_BAK *AutoAdcBackup)
{
    MS_U8 u8Bank;
    u8Bank = MDrv_ReadByte(BK_SELECT_00);
    MDrv_WriteByte(BK_SELECT_00, REG_BANK_IP1F2);

    AutoAdcBackup->BkDtop_01=MDrv_Read2Byte(L_BK_ADC_DTOP(0x01) );
    AutoAdcBackup->BkDtop_02=MDrv_Read2Byte(L_BK_ADC_DTOP(0x02) );
    AutoAdcBackup->H_BkChipTop_1F=MDrv_ReadByte(H_BK_CHIPTOP(0x1f chiptop register depends on chip) );
    AutoAdcBackup->L_BkAtop_01=MDrv_ReadByte(L_BK_ADC_ATOP(0x01) );
    AutoAdcBackup->L_BkAtop_0C=MDrv_ReadByte(L_BK_ADC_ATOP(0x0C) );
    AutoAdcBackup->L_BkAtop_2C=MDrv_ReadByte(L_BK_ADC_ATOP(0x2C) );
    AutoAdcBackup->L_BkAtop_05=MDrv_Read2Byte(L_BK_ADC_ATOP(0x05) );
    AutoAdcBackup->L_BkAtop_06=MDrv_Read2Byte(L_BK_ADC_ATOP(0x06) );
    AutoAdcBackup->L_BkAtop_1C=MDrv_ReadByte(L_BK_ADC_ATOP(0x1C) );
    AutoAdcBackup->L_BkAtop_1F=MDrv_ReadByte(L_BK_ADC_ATOP(0x1F) );

    AutoAdcBackup->L_BkDtop_06=MDrv_ReadByte(L_BK_ADC_DTOP(0x06) );
    AutoAdcBackup->L_BkIpMux_1=MDrv_ReadByte(L_BK_IPMUX(0x01) );
    AutoAdcBackup->L_SC_BK1_21=MDrv_ReadByte(L_BK_IP1F2(0x21) );
    AutoAdcBackup->SC_BK1_02=MDrv_Read2Byte(L_BK_IP1F2(0x02) );
    AutoAdcBackup->SC_BK1_03=MDrv_Read2Byte(L_BK_IP1F2(0x03) );
    AutoAdcBackup->SC_BK1_04=MDrv_Read2Byte(L_BK_IP1F2(0x04) );
    AutoAdcBackup->SC_BK1_05=MDrv_Read2Byte(L_BK_IP1F2(0x05) );
    AutoAdcBackup->SC_BK1_06=MDrv_Read2Byte(L_BK_IP1F2(0x06) );
    AutoAdcBackup->SC_BK1_07=MDrv_Read2Byte(L_BK_IP1F2(0x07) );
    AutoAdcBackup->SC_BK1_0E=MDrv_Read2Byte(L_BK_IP1F2(0x0E) );

    MDrv_WriteByte(BK_SELECT_00, REG_BANK_VOP);
    AutoAdcBackup->L_SC_BK10_19=MDrv_ReadByte(L_BK_VOP(0x19) );

   MDrv_WriteByteMask(L_BK_ADC_ATOP(0x5c), 0x30,0x30);  //ldo

   MDrv_WriteByte(BK_SELECT_00, u8Bank);

}
//----------------------------------------------------------------------
//
//----------------------------------------------------------------------
static void MDrv_RestoreAutoAdcReg(MS_AUTOADC_REG_BAK *AutoAdcBackup)
{
    MS_U8 u8Bank;
    u8Bank = MDrv_ReadByte(BK_SELECT_00);
    MDrv_WriteByte(BK_SELECT_00, REG_BANK_IP1F2);

    MDrv_Write2Byte(L_BK_ADC_DTOP(0x01) ,AutoAdcBackup->BkDtop_01 );
    MDrv_Write2Byte(L_BK_ADC_DTOP(0x02) ,AutoAdcBackup->BkDtop_02);
    MDrv_WriteByte(H_BK_CHIPTOP(0x1f chiptop register depends on chip) ,AutoAdcBackup->H_BkChipTop_1F);
    MDrv_WriteByte(L_BK_ADC_ATOP(0x01),AutoAdcBackup->L_BkAtop_01 );
    MDrv_WriteByte(L_BK_ADC_ATOP(0x0C),AutoAdcBackup->L_BkAtop_0C );
    MDrv_WriteByte(L_BK_ADC_ATOP(0x2C) ,AutoAdcBackup->L_BkAtop_2C);
    MDrv_WriteByte(L_BK_ADC_ATOP(0x1C),AutoAdcBackup->L_BkAtop_1C );
    MDrv_WriteByte(L_BK_ADC_ATOP(0x1F),AutoAdcBackup->L_BkAtop_1F );

    MDrv_Write2Byte(L_BK_ADC_ATOP(0x05) ,AutoAdcBackup->L_BkAtop_05);
    MDrv_Write2Byte(L_BK_ADC_ATOP(0x06) ,AutoAdcBackup->L_BkAtop_06);

    MDrv_WriteByte(L_BK_ADC_DTOP(0x06),AutoAdcBackup->L_BkDtop_06 );
    MDrv_WriteByte(L_BK_IPMUX(0x01) ,AutoAdcBackup->L_BkIpMux_1);
    MDrv_WriteByte(L_BK_IP1F2(0x21) ,AutoAdcBackup->L_SC_BK1_21);
    MDrv_Write2Byte(L_BK_IP1F2(0x02) ,AutoAdcBackup->SC_BK1_02);
    MDrv_Write2Byte(L_BK_IP1F2(0x03) , AutoAdcBackup->SC_BK1_03);
    MDrv_Write2Byte(L_BK_IP1F2(0x04) , AutoAdcBackup->SC_BK1_04);
    MDrv_Write2Byte(L_BK_IP1F2(0x05) , AutoAdcBackup->SC_BK1_05);
    MDrv_Write2Byte(L_BK_IP1F2(0x06) , AutoAdcBackup->SC_BK1_06);
    MDrv_Write2Byte(L_BK_IP1F2(0x07) , AutoAdcBackup->SC_BK1_07);
    MDrv_Write2Byte(L_BK_IP1F2(0x0E) , AutoAdcBackup->SC_BK1_0E);

   MDrv_WriteByteMask(L_BK_ADC_ATOP(0x5c), 0x00,0x70);

   MDrv_WriteByte(BK_SELECT_00, REG_BANK_VOP);
   MDrv_WriteByte(L_BK_VOP(0x19),AutoAdcBackup->L_SC_BK10_19  );


   MDrv_WriteByte(BK_SELECT_00, u8Bank);
}

//*************************************************************************
//
//*************************************************************************
static void _MApi_XC_Auto_SetCalibrartinWindow( SCALER_WIN scwin )
{
    //set auto range window for calibrartin
    MS_U16 Vcut,Hcut;
    MS_WINDOW_TYPE cap_win;

    MApi_XC_GetCaptureWindow(&cap_win,scwin );

    Vcut = cap_win.height/ 10;
    Hcut = cap_win.width/ 10;

    MApi_XC_AutoRangeEnable(ENABLE, scwin);

    MApi_XC_SetAutoRangeWindow( cap_win.y + (Vcut/2),
                               cap_win.x + (Hcut/2),
                               cap_win.height- Vcut,
                               cap_win.width- Hcut,
                               scwin
                               );
    //printf("capture: %d %d %d %d \n",Vstart, Hstart, Vsize , Hsize );

}

//
//
#if  ( GET_RGB_REPORT == ENABLE )
static void GetTestReport(void )
{
    MS_U8 u8Bank,i;
    MS_U16 Testdata[6],Sc_Bk01_0E;
    MS_U16 TimeoutCnt;
    MsOS_DelayTask(1000);
    u8Bank=MDrv_ReadByte(BK_SELECT_00);
    MDrv_WriteByte(BK_SELECT_00, REG_BANK_IP1F2);
    Sc_Bk01_0E=MDrv_Read2Byte(L_BK_IP1F2(0x0e));

    //set calibrartin window
    _MApi_XC_Auto_SetCalibrartinWindow(MAIN_WINDOW);

    MDrv_WriteByteMask(H_BK_IP1F2(0x0e), 0x80,0x80);
    MDrv_WriteByteMask(L_BK_IP1F2(0x0e), 0x11,0x11);
    TimeoutCnt=0;
    while( ( MDrv_ReadByte(L_BK_IP1F2(0x0e) ) & BIT(1))==0x00 )
    {
        MsOS_DelayTask(1);
        TimeoutCnt++;
        if ( TimeoutCnt > 500 )
        {
            //printf("report timeout\n");
            break;
        }
    }

    if (   TimeoutCnt < 500 )
    {
        for  (i=0;i<6;i++)
        {
            MDrv_WriteByte( L_BK_IP1F2(0x0F), i );
            //MsOS_DelayTask(200);
            Testdata[i]=MDrv_Read2Byte(L_BK_IP1F2(0x11));
            //printf("%04x ",Testdata[i]);
        }
        //printf("\n");
    }

    printf("min:  r g b    max:R G B\n");
    for  (i=0;i<6;i++)
    {
        printf("%04x ",Testdata[i]);
    }
    printf("\n");

    MDrv_WriteByteMask(H_BK_IP1F2(0x29), 0x0,0x1);

    MDrv_Write2Byte(L_BK_IP1F2(0x0e),Sc_Bk01_0E);

    MDrv_WriteByte(BK_SELECT_00, u8Bank);

}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
static void CheckAdcData( void )
{
    MS_U8 u8Bank;
    u8Bank=MDrv_ReadByte(BK_SELECT_00);
    MDrv_WriteByte(BK_SELECT_00, REG_BANK_IP1F2);

    if  ( (MDrv_ReadByte(L_BK_IP1F2(0x0e) ) & 0x11 )== 0x11 )
    {
        GetTestReport();
    }

    MDrv_WriteByte(BK_SELECT_00, u8Bank);
}
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
//
//This report is for debug only
// *** the report is pattern dependent!!
static void GetTestReport_Bk1A( void )
{
    MS_U8 u8Bank,i;
    u8Bank=MDrv_ReadByte(BK_SELECT_00);
    MDrv_WriteByte(BK_SELECT_00, REG_BANK_DLC);

    MDrv_WriteByteMask(L_BK_DLC(0x68), 0x01,0x1);
    MDrv_Write2Byte( L_BK_DLC(0x69),0x700 );
    MDrv_Write2Byte( L_BK_DLC(0x6A),0x300 );
    printf(" report  white Bk1a: \n");
    MsOS_DelayTask(20);
    for( i=0;i<3;i++ )
    {
        printf(" %03x  ", MDrv_Read2Byte( L_BK_DLC(0x6b+i ) ) );
    }
    printf("\n");

    MDrv_Write2Byte( L_BK_DLC(0x69),0x50 );
    MDrv_Write2Byte( L_BK_DLC(0x6A),0x100 );

    MsOS_DelayTask(20);
    printf(" report  black Bk1a: \n");

    for( i=0;i<3;i++ )
    {
        printf(" %03x  ", MDrv_Read2Byte( L_BK_DLC(0x6b+i ) ) );
    }
    printf("\n\n");

    MDrv_WriteByteMask(L_BK_DLC(0x68), 0x00,0x1);

    MDrv_WriteByte(BK_SELECT_00, u8Bank);

}
#endif

#endif

MS_BOOL _MApi_XC_Auto_TuneOffset_Internal( XC_Auto_TuneType enAutoTuneType, APIXC_AdcGainOffsetSetting  *pstADCSetting ,SCALER_WIN eWindow )
{
    MS_WINDOW_TYPE mem_win;
    MS_WINDOW_TYPE tempCaptureWindow;

    //MS_U8 DcOffset_R=0xFF;
    //MS_U8 DcOffset_G=0xFF;
    //MS_U8 DcOffset_B=0xFF;
    MS_U16 DcOffset_R=0x80;
    MS_U16 DcOffset_G=0x80;
    MS_U16 DcOffset_B=0x80;
    //MS_U8 LIMIT_R,LIMIT_G,LIMIT_B;
    //MS_U8 Delta=0x80;
    MS_U8 Delta=0x40;
    MS_U8 CheckFlag=0x00;
    MS_AUTOADC_TYPE pAutoAdc_0V;
    MS_U16  OffsetHigh, OffsetLow;
    MS_U16  G_OffsetHigh, G_OffsetLow;
    //MS_U16 i = 0, j = 0, x = 0;
    MS_U16 y = 0;
    MS_U16 u16UpperDiff_R = 0xFFFF, u16LowerDiff_R = 0xFFFF;
    MS_U16 u16UpperDiff_G = 0xFFFF, u16LowerDiff_G = 0xFFFF;
    MS_U16 u16UpperDiff_B = 0xFFFF, u16LowerDiff_B = 0xFFFF;
    MS_U16 u16UpperGain_R = 0, u16LowerGain_R = 0;
    MS_U16 u16UpperGain_G = 0, u16LowerGain_G = 0;
    MS_U16 u16UpperGain_B = 0, u16LowerGain_B = 0;
    MS_U8 u8Direction_R = 0, u8Direction_G = 0, u8Direction_B = 0;
    MS_U16 TempDcOffset_R=0x80;
    MS_U16 TempDcOffset_G=0x80;
    MS_U16 TempDcOffset_B=0x80;

    //printf("offset\n");

    CheckFlag=0x00;

    if (enAutoTuneType & E_XC_AUTO_TUNE_RGB_OFFSET)
    {
        //if RGB
        OffsetHigh=AUTO_GAIN_BONDARY;
        OffsetLow= 0x00;
        G_OffsetHigh=AUTO_GAIN_BONDARY;
        G_OffsetLow=0x00;
    }
    else
    {
        //if YUV
        OffsetHigh=AUTO_OFFSET_CBCR+(AUTO_GAIN_BONDARY);
        OffsetLow= AUTO_OFFSET_CBCR-(AUTO_GAIN_BONDARY);
        G_OffsetHigh=AUTO_OFFSET_Y+AUTO_GAIN_BONDARY;
        G_OffsetLow= AUTO_OFFSET_Y-AUTO_GAIN_BONDARY;
    }

    AUTOMSG( printf("\nY Offset=%d ~%d\n",G_OffsetLow,G_OffsetHigh));
    AUTOMSG (printf("CbcrOffset=%d ~%d\n\n",OffsetLow,OffsetHigh));
    AUTOMSG(printf("\nInit Offset_R=0x%x Offset_G=0x%x Offset_B=0x%x\n\n", DcOffset_R, DcOffset_G, DcOffset_B));

    //printf("\nY Offset=%d ~%d\n",G_OffsetLow,G_OffsetHigh);
    //printf("CbcrOffset=%d ~%d\n\n",OffsetLow,OffsetHigh);


    //MS_AUTOADC_TYPE pAutoAdc_0V_B;
    MApi_XC_ADC_SetInternalVoltage(E_ADC_Internal_0V);
    //set init value
    MApi_XC_ADC_SetOffset(ADC_CHR, DcOffset_R);
    MApi_XC_ADC_SetOffset(ADC_CHG, DcOffset_G);
    MApi_XC_ADC_SetOffset(ADC_CHB, DcOffset_B);


    //MsOS_DelayTask(20);  //** must delay n Vsync

    MApi_XC_GetCaptureWindow(&tempCaptureWindow, eWindow);

    // mem window is logical window, so x/y starts from 0.
    mem_win.height = 32;
    mem_win.width = 32;
    mem_win.x = ( tempCaptureWindow.width / 2 ) - (mem_win.width/2);
    mem_win.y = ( tempCaptureWindow.height / 2 ) - (mem_win.height/2);


    //while( Delta > 0 )   //Get a offset that the ADC is > 0
    for (y = 0; y < 9; y++)
    {
        // wait input 2 frames
        MApi_XC_WaitInputVSync(3, 100, eWindow);

        // freeze memory
        //MDrv_WriteByte(BK_SELECT_00, REG_BANK_SCMI);
        //MDrv_WriteRegBit(H_BK_SCMI(0x01), TRUE, BIT(3));
        MApi_XC_FreezeImg(ENABLE,eWindow);
        //printf("Offset Loop Count %d\n", ++x);

        //MDrv_WriteByte(BK_SELECT_00, REG_BANK_IP2F2);
        //MDrv_WriteRegBit(H_BK_SCMI(0x09), TRUE, BIT(7));

        //mdrv_sc_averagedata_info(&pAutoAdc_0V);
        pAutoAdc_0V = MApi_XC_GetAverageDataFromMemory(&mem_win);
        //AUTOMSG(printf("%04x %04x %04x\n\n",pAutoAdc_0V.R_CH_AVG,pAutoAdc_0V.G_CH_AVG,pAutoAdc_0V.B_CH_AVG));

        // un-freeze memory
        MApi_XC_FreezeImg(DISABLE,eWindow);


        //MDrv_WriteByte(BK_SELECT_00, REG_BANK_SCMI);
        //MDrv_WriteRegBit(H_BK_SCMI(0x01), FALSE, BIT(3));

        //MDrv_WriteByte(BK_SELECT_00, REG_BANK_IP2F2);
        //MDrv_WriteRegBit(H_BK_SCMI(0x09), FALSE, BIT(7));

        if ( (CheckFlag & BIT(0))==0x00 )
        {
            if ( y == 8 )
            {
                AUTOMSG(printf("============No. %02d=============>\n", (y+1)));

                AUTOMSG(printf("DcOffset_R 0x%02x\n",TempDcOffset_R));
                AUTOMSG(printf("Cr Offset %02d\n",pAutoAdc_0V.u16CH_AVG[0]));

                AUTOMSG(printf("<===============================\n\n\n"));

                if (pAutoAdc_0V.u16CH_AVG[0] >= AUTO_OFFSET_CBCR)
                {
                    if (u16UpperDiff_R > (pAutoAdc_0V.u16CH_AVG[0] - AUTO_OFFSET_CBCR))
                    {
                        u16UpperDiff_R = pAutoAdc_0V.u16CH_AVG[0] - AUTO_OFFSET_CBCR;
                        u16UpperGain_R = TempDcOffset_R;
                    }
                }
                if (pAutoAdc_0V.u16CH_AVG[0] <= AUTO_OFFSET_CBCR)
                {
                    if (u16LowerDiff_R > (AUTO_OFFSET_CBCR - pAutoAdc_0V.u16CH_AVG[0]))
                    {
                        u16LowerDiff_R = AUTO_OFFSET_CBCR - pAutoAdc_0V.u16CH_AVG[0];
                        u16LowerGain_R = TempDcOffset_R;
                    }
                }
            }
            else
            {
                AUTOMSG(printf("============No. %02d=============>\n", (y+1)));

                AUTOMSG(printf("DcOffset_R 0x%02x\n",DcOffset_R));
                AUTOMSG(printf("Cr Offset %02d\n",pAutoAdc_0V.u16CH_AVG[0]));

                AUTOMSG(printf("<===============================\n\n\n"));

                if ( ( pAutoAdc_0V.u16CH_AVG[0] >= OffsetLow) &&  ( pAutoAdc_0V.u16CH_AVG[0] <= OffsetHigh ) )
                {
                    CheckFlag |= BIT(0);  //
                    AUTOMSG(printf("R-offset -ok---0x%02x\n",DcOffset_R));
                    u16UpperDiff_R = u16LowerDiff_R = 0;
                    u16UpperGain_R = u16LowerGain_R = DcOffset_R;
                }
                else
                {
                    if (pAutoAdc_0V.u16CH_AVG[0] >= AUTO_OFFSET_CBCR)
                    {
                        if (u16UpperDiff_R > (pAutoAdc_0V.u16CH_AVG[0] - AUTO_OFFSET_CBCR))
                        {
                            u16UpperDiff_R = pAutoAdc_0V.u16CH_AVG[0] - AUTO_OFFSET_CBCR;
                            u16UpperGain_R = DcOffset_R;
                        }
                    }
                    if (pAutoAdc_0V.u16CH_AVG[0] <= AUTO_OFFSET_CBCR)
                    {
                        if (u16LowerDiff_R > (AUTO_OFFSET_CBCR - pAutoAdc_0V.u16CH_AVG[0]))
                        {
                            u16LowerDiff_R = AUTO_OFFSET_CBCR - pAutoAdc_0V.u16CH_AVG[0];
                            u16LowerGain_R = DcOffset_R;
                        }
                    }

                    if ( pAutoAdc_0V.u16CH_AVG[0] <= OffsetLow )  // == 0 )
                    {
                        DcOffset_R=DcOffset_R-Delta;
                        if ( y == 6 )
                        {
                            u8Direction_R = 1; //DownToUp
                        }
                    }
                    else
                    {
                        DcOffset_R=DcOffset_R+Delta;
                        if ( y == 6 )
                        {
                            u8Direction_R = 2; //UpToDown
                        }
                    }
                }

                if ( y == 7 )
                {
                    if (u8Direction_R == 1)
                    {
                        TempDcOffset_R = DcOffset_R-1;

                    }
                    else if (u8Direction_R == 2)
                    {
                        TempDcOffset_R = DcOffset_R+1;
                    }
                    MApi_XC_ADC_SetOffset(ADC_CHR, TempDcOffset_R);
                }
                else
                {
                    AUTOMSG( printf("DcOffset_R 0x%02x\n",DcOffset_R));
                    MApi_XC_ADC_SetOffset(ADC_CHR, DcOffset_R);
                }
            }
        }


        if ( (CheckFlag & BIT(1))==0x00 )
        {
            if ( y == 8 )
            {
                AUTOMSG(printf("============No. %02d=============>\n", (y+1)));

                AUTOMSG(printf("DcOffset_G 0x%02x\n",TempDcOffset_G ));
                AUTOMSG(printf("Y Offset %02d\n",pAutoAdc_0V.u16CH_AVG[1]));

                AUTOMSG(printf("<===============================\n\n\n"));

                if (pAutoAdc_0V.u16CH_AVG[1] >= AUTO_OFFSET_Y)
                {
                    if (u16UpperDiff_G > (pAutoAdc_0V.u16CH_AVG[1] - AUTO_OFFSET_Y))
                    {
                        u16UpperDiff_G = pAutoAdc_0V.u16CH_AVG[1] - AUTO_OFFSET_Y;
                        u16UpperGain_G = TempDcOffset_G;
                    }
                }
                if (pAutoAdc_0V.u16CH_AVG[1] <= AUTO_OFFSET_Y)
                {
                    if (u16LowerDiff_G > (AUTO_OFFSET_Y - pAutoAdc_0V.u16CH_AVG[1]))
                    {
                        u16LowerDiff_G = AUTO_OFFSET_Y - pAutoAdc_0V.u16CH_AVG[1];
                        u16LowerGain_G = TempDcOffset_G;
                    }
                }
            }
            else
            {
                AUTOMSG(printf("============No. %02d=============>\n", (y+1)));

                AUTOMSG(printf("DcOffset_G 0x%02x\n",DcOffset_G ));
                AUTOMSG(printf("Y Offset %02d\n",pAutoAdc_0V.u16CH_AVG[1]));

                AUTOMSG(printf("<===============================\n\n\n"));

                if ( ( pAutoAdc_0V.u16CH_AVG[1] >= G_OffsetLow ) &&  ( pAutoAdc_0V.u16CH_AVG[1] <= G_OffsetHigh ) )
                {
                    CheckFlag |= BIT(1);  //
                    AUTOMSG(printf("G-offset -ok---0x%02x\n",DcOffset_G));
                    u16UpperDiff_G = u16LowerDiff_G = 0;
                    u16UpperGain_G = u16LowerGain_G = DcOffset_G;
                }
                else
                {
                    if (pAutoAdc_0V.u16CH_AVG[1] >= AUTO_OFFSET_Y)
                    {
                        if (u16UpperDiff_G > (pAutoAdc_0V.u16CH_AVG[1] - AUTO_OFFSET_Y))
                        {
                            u16UpperDiff_G = pAutoAdc_0V.u16CH_AVG[1] - AUTO_OFFSET_Y;
                            u16UpperGain_G = DcOffset_G;
                        }
                    }
                    if (pAutoAdc_0V.u16CH_AVG[1] <= AUTO_OFFSET_Y)
                    {
                        if (u16LowerDiff_G > (AUTO_OFFSET_Y - pAutoAdc_0V.u16CH_AVG[1]))
                        {
                            u16LowerDiff_G = AUTO_OFFSET_Y - pAutoAdc_0V.u16CH_AVG[1];
                            u16LowerGain_G = DcOffset_G;
                        }
                    }

                    if ( pAutoAdc_0V.u16CH_AVG[1] <= G_OffsetLow )
                    {
                        DcOffset_G=DcOffset_G-Delta;
                        if ( y == 6 )
                        {
                            u8Direction_G = 1; //DownToUp
                        }
                    }
                    else
                    {
                        DcOffset_G=DcOffset_G+Delta;
                        if ( y == 6 )
                        {
                            u8Direction_G = 2; //DownToUp
                        }
                    }
                }

                if ( y == 7 )
                {
                    if (u8Direction_G == 1)
                    {
                        TempDcOffset_G = DcOffset_G-1;

                    }
                    else if (u8Direction_G == 2)
                    {
                        TempDcOffset_G = DcOffset_G+1;
                    }
                    MApi_XC_ADC_SetOffset(ADC_CHG, TempDcOffset_G);
                }
                else
                {
                    //AUTOMSG(printf("DcOffset_G 0x%02bx\n",DcOffset_G ));
                    MApi_XC_ADC_SetOffset(ADC_CHG, DcOffset_G);
                }
            }
        }


        if ( (CheckFlag & BIT(2))==0x00 )
        {
            if ( y == 8 )
            {
                AUTOMSG(printf("============No. %02d=============>\n", (y+1)));

                AUTOMSG(printf("DcOffset_B 0x%02x\n",TempDcOffset_B));
                AUTOMSG(printf("Cb Offset %02d\n",pAutoAdc_0V.u16CH_AVG[2]));

                AUTOMSG(printf("<===============================\n\n\n"));

                if (pAutoAdc_0V.u16CH_AVG[2] >= AUTO_OFFSET_CBCR)
                {
                    if (u16UpperDiff_B > (pAutoAdc_0V.u16CH_AVG[2] - AUTO_OFFSET_CBCR))
                    {
                        u16UpperDiff_B = pAutoAdc_0V.u16CH_AVG[2] - AUTO_OFFSET_CBCR;
                        u16UpperGain_B = TempDcOffset_B;
                    }
                }
                if (pAutoAdc_0V.u16CH_AVG[2] <= AUTO_OFFSET_CBCR)
                {
                    if (u16LowerDiff_B > (AUTO_OFFSET_CBCR - pAutoAdc_0V.u16CH_AVG[2]))
                    {
                        u16LowerDiff_B = AUTO_OFFSET_CBCR - pAutoAdc_0V.u16CH_AVG[2];
                        u16LowerGain_B = TempDcOffset_B;
                    }
                }
            }
            else
            {
                AUTOMSG(printf("============No. %02d=============>\n", (y+1)));

                AUTOMSG(printf("DcOffset_B 0x%02x\n",DcOffset_B));
                AUTOMSG(printf("Cb Offset %02d\n",pAutoAdc_0V.u16CH_AVG[2]));

                AUTOMSG(printf("<===============================\n\n\n"));

                if ( ( pAutoAdc_0V.u16CH_AVG[2] >= OffsetLow) &&  ( pAutoAdc_0V.u16CH_AVG[2] <= OffsetHigh ) )
                {
                    CheckFlag |= BIT(2);  //
                    AUTOMSG(printf("B-offset -ok---0x%02x\n",DcOffset_B));
                    u16UpperDiff_B = u16LowerDiff_B = 0;
                    u16UpperGain_B = u16LowerGain_B = DcOffset_B;
                }
                else
                {
                    if (pAutoAdc_0V.u16CH_AVG[2] >= AUTO_OFFSET_CBCR)
                    {
                        if (u16UpperDiff_B > (pAutoAdc_0V.u16CH_AVG[2] - AUTO_OFFSET_CBCR))
                        {
                            u16UpperDiff_B = pAutoAdc_0V.u16CH_AVG[2] - AUTO_OFFSET_CBCR;
                            u16UpperGain_B = DcOffset_B;
                        }
                    }
                    if (pAutoAdc_0V.u16CH_AVG[2] <= AUTO_OFFSET_CBCR)
                    {
                        if (u16LowerDiff_B > (AUTO_OFFSET_CBCR - pAutoAdc_0V.u16CH_AVG[2]))
                        {
                            u16LowerDiff_B = AUTO_OFFSET_CBCR - pAutoAdc_0V.u16CH_AVG[2];
                            u16LowerGain_B = DcOffset_B;
                        }
                    }

                    if ( pAutoAdc_0V.u16CH_AVG[2] <= OffsetLow )  //== 0 )
                    {
                        DcOffset_B=DcOffset_B-Delta;
                        if ( y == 6 )
                        {
                            u8Direction_B = 1; //DownToUp
                        }
                    }
                    else
                    {
                        DcOffset_B=DcOffset_B+Delta;
                         if ( y == 6 )
                        {
                            u8Direction_B = 2; //UpToDown
                        }
                   }
                }

                if ( y == 7 )
                {
                    if (u8Direction_B == 1)
                    {
                        TempDcOffset_B = DcOffset_B-1;

                    }
                    else if (u8Direction_B == 2)
                    {
                        TempDcOffset_B = DcOffset_B+1;
                    }
                    MApi_XC_ADC_SetOffset(ADC_CHB, TempDcOffset_B);
                }
                else
                {
                    //AUTOMSG( printf("DcOffset_B 0x%02bx\n",DcOffset_B));
                    MApi_XC_ADC_SetOffset(ADC_CHB, DcOffset_B);
                }
            }
        }

        if ( ( CheckFlag & 0x07 ) == 0x07 )
        {
            MApi_XC_ADC_SetOffset(ADC_CHR, DcOffset_R);
            MApi_XC_ADC_SetOffset(ADC_CHG, DcOffset_G);
            MApi_XC_ADC_SetOffset(ADC_CHB, DcOffset_B);

            pstADCSetting->u16RedOffset=DcOffset_R;
            pstADCSetting->u16GreenOffset=DcOffset_G;
            pstADCSetting->u16BlueOffset=DcOffset_B;
            AUTOMSG(printf("offset check ok  0x%02x 0x%02x 0x%02x\n",DcOffset_R,DcOffset_G,DcOffset_B));
            //MDrv_WriteByte(BK_SELECT_00, u8Bank);

            return TRUE;
        }

        Delta=Delta>>1;
        //printf("Delta %d\n", Delta);
        //MsOS_DelayTask(20);  //** must delay n Vsync
    }


    if (u16UpperDiff_R <= u16LowerDiff_R)
    {
        DcOffset_R = u16UpperGain_R;
    }
    else
    {
        DcOffset_R = u16LowerGain_R;
    }

    if (u16UpperDiff_G <= u16LowerDiff_G)
    {
        DcOffset_G = u16UpperGain_G;
    }
    else
    {
        DcOffset_G = u16LowerGain_G;
    }

    if (u16UpperDiff_B <= u16LowerDiff_B)
    {
        DcOffset_B = u16UpperGain_B;
    }
    else
    {
        DcOffset_B = u16LowerGain_B;
    }

    MApi_XC_ADC_SetOffset(ADC_CHR, DcOffset_R);
    MApi_XC_ADC_SetOffset(ADC_CHG, DcOffset_G);
    MApi_XC_ADC_SetOffset(ADC_CHB, DcOffset_B);

    pstADCSetting->u16RedOffset=DcOffset_R;
    pstADCSetting->u16GreenOffset=DcOffset_G;
    pstADCSetting->u16BlueOffset=DcOffset_B;
    AUTOMSG(printf("offset check ok  0x%02x 0x%02x 0x%02x\n",DcOffset_R,DcOffset_G,DcOffset_B));
    //MDrv_WriteByte(BK_SELECT_00, u8Bank);

    return TRUE;

}



//----------------------------------------------------------------------
//
// gain setting: 00 is maximum ~~ 0xff is minmum
//----------------------------------------------------------------------
enum {
    DOWN_TOP= 1,
    TOP_DOWN= 2,
};

MS_BOOL MApi_XC_Auto_SetValidData(MS_U8 Value)
{
    _u8ThresholdValue = Value;
    return TRUE;
}

static void _GetGainRange(XC_Auto_CalibrationType type,XC_Auto_TuneType tune_type ,MS_AUTOADC_TYPE* Diff)
{
    MS_AUTOADC_TYPE pAutoAdc_105;
    MS_AUTOADC_TYPE pAutoAdc_055;
    MS_WINDOW_TYPE mem_win;
    int ch ;

    // External YUV need gather Max/Min by differnt win
    if ( tune_type != E_XC_AUTO_TUNE_YUV_COLOR || type != E_XC_EXTERNAL_CALIBRATION )
    {
        //////////////////////////////
        // Get Max value
        if (type == E_XC_INTERNAL_CALIBRATION)
        {
            // internal have to generate internal voltage.
            mem_win.x = 5;
            mem_win.y = 5;
            mem_win.width = 10;
            mem_win.height= 10;
            MApi_XC_ADC_SetInternalVoltage(E_ADC_Internal_0_6V);
        }
        else
        {
            mem_win.x = 640;
            mem_win.y = 360;
            mem_win.width = 10;
            mem_win.height= 10;
        }
        MApi_XC_WaitInputVSync(5, 200,MAIN_WINDOW);

        pAutoAdc_105 = MApi_XC_GetAverageDataFromMemory(&mem_win);

        //printf("[internal 1.05V]  %04d %04d %04d \n",pAutoAdc_105.u16CH_AVG[0],pAutoAdc_105.u16CH_AVG[1],pAutoAdc_105.u16CH_AVG[2]);

        //////////////////////////////
        // Get Min value
        if (type == E_XC_INTERNAL_CALIBRATION)
        {
            // internal have to generate internal voltage.
            mem_win.x = 5;
            mem_win.y = 5;
            mem_win.width = 10;
            mem_win.height= 10;
            MApi_XC_ADC_SetInternalVoltage(E_ADC_Internal_0_1V);  //MDrv_WriteByteMask( L_BK_ADC_DTOP(0x13) , 0x20, 0x30 ); //switch internal to 0.1V
        }
        else
        {
            mem_win.x = 5;
            mem_win.y = 5;
            mem_win.width = 10;
            mem_win.height= 10;
        }

        MApi_XC_WaitInputVSync(5, 200,MAIN_WINDOW);

        pAutoAdc_055 = MApi_XC_GetAverageDataFromMemory(&mem_win);

        //printf("[internal 0.55V]  %04d %04d %04d \n",pAutoAdc_055.u16CH_AVG[0],pAutoAdc_055.u16CH_AVG[1],pAutoAdc_055.u16CH_AVG[2]);
        for (ch =0; ch  < 3;ch ++)
          Diff->u16CH_AVG[ch] = pAutoAdc_105.u16CH_AVG[ch] - pAutoAdc_055.u16CH_AVG[ch];
    }
    else
    {
        //////////////////////////////
        // Get Y Max
        mem_win.x = 0X200;
        mem_win.y = 0X10;
        mem_win.width = 10;
        mem_win.height= 10;

        MApi_XC_WaitInputVSync(5, 200,MAIN_WINDOW);

        pAutoAdc_105 = MApi_XC_GetAverageDataFromMemory(&mem_win);

        //////////////////////////////
        // Get Y Min
        mem_win.x = 0X200;
        mem_win.y = 0X280;
        mem_win.width = 10;
        mem_win.height= 10;

        MApi_XC_WaitInputVSync(5, 200,MAIN_WINDOW);

        pAutoAdc_055 = MApi_XC_GetAverageDataFromMemory(&mem_win);
        Diff->u16CH_AVG[1] = pAutoAdc_105.u16CH_AVG[1] - pAutoAdc_055.u16CH_AVG[1];

        //////////////////////////////
        // Get Pb Max
        mem_win.x = 0X200;
        mem_win.y = 0X220;
        mem_win.width = 10;
        mem_win.height= 10;

        MApi_XC_WaitInputVSync(5, 200,MAIN_WINDOW);

        pAutoAdc_105 = MApi_XC_GetAverageDataFromMemory(&mem_win);

        //////////////////////////////
        // Get Pb Min
        mem_win.x = 0X200;
        mem_win.y = 0X60;
        mem_win.width = 10;
        mem_win.height= 10;

        MApi_XC_WaitInputVSync(5, 200,MAIN_WINDOW);

        pAutoAdc_055 = MApi_XC_GetAverageDataFromMemory(&mem_win);
        Diff->u16CH_AVG[2] = pAutoAdc_105.u16CH_AVG[2] - pAutoAdc_055.u16CH_AVG[2];

        //////////////////////////////
        // Get Pr Max
        mem_win.x = 0X200;
        mem_win.y = 0X1C0;
        mem_win.width = 10;
        mem_win.height= 10;

        MApi_XC_WaitInputVSync(5, 200,MAIN_WINDOW);

        pAutoAdc_105 = MApi_XC_GetAverageDataFromMemory(&mem_win);

        //////////////////////////////
        // Get Pr Min
        mem_win.x = 0X200;
        mem_win.y = 0XB0;
        mem_win.width = 10;
        mem_win.height= 10;

        MApi_XC_WaitInputVSync(5, 200,MAIN_WINDOW);

        pAutoAdc_055 = MApi_XC_GetAverageDataFromMemory(&mem_win);
        Diff->u16CH_AVG[0] = pAutoAdc_105.u16CH_AVG[0] - pAutoAdc_055.u16CH_AVG[0];

    }
}

//*************************************************************************
//Function name: _MApi_XC_Auto_TuneGain
//Passing parameter:
//  MS_U8 u8VSyncTime: VSync time
//  APIXC_AdcGainOffsetSetting * pstADCSetting: ADC setting of Current PC mode
//Return parameter:
//  MS_BOOL: Success status. (If faile, return FALSE.)
//Description: Auto-tune R/G/B gain of ADC.
//*************************************************************************

static MS_BOOL _MApi_XC_Auto_TuneGain(XC_Auto_CalibrationType calibration_type,XC_Auto_TuneType enAutoTuneType, APIXC_AdcGainOffsetSetting *pstADCSetting, SCALER_WIN eWindow )
{
    //pstADCSetting=pstADCSetting;

    #define TOLERATE_ERROR 65 // 10%
    #define MAX_CALIBRATION_ITERATION_EXTERNAL   12

    MS_AUTOADC_TYPE Diff;
    MS_BOOL bDone[3] = {FALSE, FALSE, FALSE};
    MS_U16 GainTargetLBound[3];
    MS_U16 GainTargetUBound[3];

    // T2 have to change to 0x80
    // Default gain
    MS_U16 Gain[3]      ={0x1000, 0x1000, 0x1000};
    MS_U16 TempGain[3]  ={0x1000, 0x1000, 0x1000};
    MS_U16 delta[3]={0x800, 0x800, 0x800};
    MS_U16 u16UpperDiff[3] = {0xFFFF, 0xFFFF, 0xFFFF};
    MS_U16 u16LowerDiff[3] = {0xFFFF, 0xFFFF, 0xFFFF};
    MS_U16 u16UpperGain[3] = {0, 0, 0};
    MS_U16 u16LowerGain[3] = {0, 0, 0};
    MS_U16 GainTarget[3];
    MS_U8 u8Direction[3] = {0, 0, 0};
    MS_U16 ch;
    MS_U16 y = 0;
    MS_U16 u16Diff;
    APIXC_AdcGainOffsetSetting tempADCSetting;

    //////////////////////////////
    // Initialize
    if (calibration_type == E_XC_INTERNAL_CALIBRATION)
    {

        if (enAutoTuneType & E_XC_AUTO_TUNE_RGB_GAIN)
        {
            //printf("RGB\n");
            GainTarget[0] = AUTO_GAIN_RANGE_RGB_INTERNAL;
            GainTarget[1] = AUTO_GAIN_RANGE_RGB_INTERNAL;
            GainTarget[2] = AUTO_GAIN_RANGE_RGB_INTERNAL;
        }
        else
        {
            //printf("YUV\n");
            GainTarget[0] = AUTO_GAIN_RANGE_CBCR_INTERNAL;
            GainTarget[1] = AUTO_GAIN_RANGE_Y_INTERNAL;
            GainTarget[2] = AUTO_GAIN_RANGE_CBCR_INTERNAL;
        }

    }
    else
    {
        if (enAutoTuneType & E_XC_AUTO_TUNE_RGB_GAIN)
        {
            //printf("ext RGB\n");
            GainTarget[0] = AUTO_GAIN_RANGE_RGB_EXTERNAL;
            GainTarget[1] = AUTO_GAIN_RANGE_RGB_EXTERNAL;
            GainTarget[2] = AUTO_GAIN_RANGE_RGB_EXTERNAL;
            // Initial
            MApi_XC_ADC_InitExternalCalibration(INPUT_SOURCE_VGA, &tempADCSetting);

        }
        else
        {
            //printf("ext YUV\n");
            GainTarget[0] = AUTO_GAIN_RANGE_CBCR_EXTERNAL;
            GainTarget[1] = AUTO_GAIN_RANGE_Y_EXTERNAL;
            GainTarget[2] = AUTO_GAIN_RANGE_CBCR_EXTERNAL;
            // Initial
            MApi_XC_ADC_InitExternalCalibration(INPUT_SOURCE_YPBPR, &tempADCSetting);

        }

        tempADCSetting.u16RedGain = Gain[0];
        tempADCSetting.u16GreenGain = Gain[1];
        tempADCSetting.u16BlueGain = Gain[2];

    }

    XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"GainTarget R: %x  G: %x  B: %x\n",GainTarget[0],GainTarget[1],GainTarget[2]);


    // Set initial gain value
    for (ch=0; ch<3; ch++)
    {
        GainTargetUBound[ch] = GainTarget[ch] + AUTO_GAIN_BONDARY;
        GainTargetLBound[ch] = GainTarget[ch] - AUTO_GAIN_BONDARY;
        MApi_XC_ADC_SetGain((MS_U8)ch, Gain[ch]);
    }

    XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"GainTarget Upper Bound R: %x  G: %x  B: %x \n", GainTargetUBound[0],GainTargetUBound[1],GainTargetUBound[2]);
    XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"GainTarget Lower Bound R: %x  G: %x  B: %x \n", GainTargetLBound[0],GainTargetLBound[1],GainTargetLBound[2]);

    // Start Gain calibration
    for ( y = 0; y < MAX_CALIBRATION_ITERATION_EXTERNAL; y++)
    {
        _GetGainRange(calibration_type,enAutoTuneType,&Diff);

        //////////////////////////////
        // Adjust for RGB / YUV
        for (ch=0; ch<3; ch++)
        {
            if (ch==0) {
                XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"============No. %02d=============>\n", (y+1));
            }
            if (!bDone[ch])
            {
                u16Diff = Diff.u16CH_AVG[ch];
                if ( y == (MAX_CALIBRATION_ITERATION_EXTERNAL-1) )
                {
                    XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"Gain[%u]=0x%x, diff=%d\n", ch, Gain[ch], u16Diff);

                    if (u16Diff >= GainTarget[ch])
                    {
                        if (u16UpperDiff[ch] > (u16Diff - GainTarget[ch]))
                        {
                            u16UpperDiff[ch] = u16Diff - GainTarget[ch];
                            u16UpperGain[ch] = TempGain[ch];
                            //AUTOMSG(printk("Cr Upper Diff %04d, Gain %04d\n", u16UpperDiff_R, u16UpperGain_R));
                        }
                    }

                    if (u16Diff <= GainTarget[ch])
                    {
                        if (u16LowerDiff[ch] > (GainTarget[ch] - u16Diff))
                        {
                            u16LowerDiff[ch] = GainTarget[ch] - u16Diff;
                            u16LowerGain[ch] = TempGain[ch];
                            //AUTOMSG(printk("Cr Lower Diff %04d, Gain %04d\n", u16LowerDiff_R, u16LowerGain_R));
                        }
                    }
                }
                else
                {
                    XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"Gain[%u]=0x%x, diff=%d\n", ch, Gain[ch], u16Diff);

                    // Found a target gain.
                    if ( (u16Diff>=GainTargetLBound[ch] ) && (u16Diff<=GainTargetUBound[ch] ) )
                    {
                        //MS_U16 target,u16Result[5],Step;
                        //MS_AUTOADC_TYPE modifiy_diff;
                        //MS_U8 index,bRetry = TRUE ;

                        bDone[ch] = TRUE;
                        u16UpperDiff[ch] = u16LowerDiff[ch] = 0;

                        u8Direction[ch] = 0;

                        ////////////Find a best solution////////////////////////
                        //target = ( GainTargetLBound[ch] + GainTargetUBound[ch] )  / 2 ;
                        //XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY," target : %d  current : %d\n",target,u16Diff);

                        #if 0 // This section takes long time. Remove first.
                        // Retrieve result array, and make sure it is linear.
                        while (bRetry)
                        {
                            for (index = 0, Step = Gain[ch] - 0x08; Step <= Gain[ch] + 0x08 ; Step+= 0x04,index++)
                            {
                                MApi_XC_ADC_SetGain(ch, Step);
                                _GetGainRange(calibration_type,enAutoTuneType,&modifiy_diff);
                                u16Result[index] = modifiy_diff.u16CH_AVG[ch];
                                XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"Result index: %d Gain: 0x%4x   %d \n",index,Step,u16Result[index] );
                            }

                            // check result is linear
                            for ( index = 0 ; index < 5 ; index ++)
                            {
                                if ( index >= 4 )
                                {
                                    bRetry = FALSE;
                                    break;
                                }

                                if ( u16Result[index] > u16Result[index+1] )
                                {
                                    XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY," fail \n");
                                    break;
                                }
                            }
                            if ( bRetry)
                                continue;

                            // Find Best solution
                            MS_U8 Index_Start = 0 ,Index_End = 4;
                            MS_BOOL bFound1 = FALSE, bFound2 = FALSE;

                            while ( Index_End > Index_Start && ( !bFound1 || !bFound2) )
                            {
                                if ( u16Result[Index_Start] != target )
                                    Index_Start++;
                                else
                                    bFound1 = TRUE;

                                if ( u16Result[Index_End] != target)
                                    Index_End--;
                                else
                                    bFound2 = TRUE;
                            }

                            // Founded
                            if ( Index_End >= Index_Start)
                            {
                                Step = Gain[ch] - 0x08;
                                Gain[ch] = Step + ( ( (Index_Start + Index_End ) * 10 ) / 2 ) * 0x04 / 10;
                            }
                            else
                            {
                                XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY," not found \n");
                            }

                        }

                        #endif
                        /////////////////////////////////////////////////

                        u16UpperGain[ch] = u16LowerGain[ch] = Gain[ch];

                        XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"=====ch:%u--OK--0x%x=======\n", ch, Gain[ch]);
                    }
                    // continoue
                    else
                    {
                        if (u16Diff >= GainTarget[ch])
                        {
                            if (u16UpperDiff[ch] > (u16Diff - GainTarget[ch]))
                            {
                                u16UpperDiff[ch] = u16Diff - GainTarget[ch];
                                u16UpperGain[ch] = Gain[ch];
                                //AUTOMSG(printk("Upper Diff %04d, Gain %04d\n", u16UpperDiff[ch], u16UpperGain[ch]));
                            }
                        }

                        if (u16Diff <= GainTarget[ch])
                        {
                            if (u16LowerDiff[ch] > (GainTarget[ch] - u16Diff))
                            {
                                u16LowerDiff[ch] = GainTarget[ch] - u16Diff;
                                u16LowerGain[ch] = Gain[ch];
                                //AUTOMSG(printk("Lower Diff %04d, Gain %04d\n", u16LowerDiff[ch], u16LowerGain[ch]));
                            }
                        }

                        if (u16Diff < GainTargetLBound[ch] )
                        {

                            Gain[ch]+=delta[ch];
                            u8Direction[ch] = DOWN_TOP; //DownToUp
                        }
                        else   //(u16Diff > GainTargetUBound[ch] )
                        {

                            Gain[ch]-=delta[ch];
                            u8Direction[ch] = TOP_DOWN; //UpToDown
                        }
                        delta[ch]=delta[ch]>>1;
                    }

                    if ( y == (MAX_CALIBRATION_ITERATION_EXTERNAL-2) )
                    {
                        if (u8Direction[ch] == DOWN_TOP)
                        {
                            // T2 is 1
                            TempGain[ch] = Gain[ch]-0x02;

                        }
                        else if (u8Direction[ch] == TOP_DOWN)
                        {
                            // T2 is 1
                            TempGain[ch] = Gain[ch]+0x02;
                        }
                        MApi_XC_ADC_SetGain((MS_U8)ch, TempGain[ch]);
                    }
                    else
                    {
                        //AUTOMSG( printk("Gain[%u] 0x%x\n", ch, Gain[ch]));
                        //AUTOMSG( printk("delta[%u] 0x%x\n", ch, delta[ch]));
                        MApi_XC_ADC_SetGain((MS_U8)ch, Gain[ch]);
                    }
                }
            }
        }

        if ( bDone[0] && bDone[1] && bDone[2] )
        {
            ////////////////////////////////
            // External RGB fine tune
            if ((calibration_type == E_XC_EXTERNAL_CALIBRATION) && (enAutoTuneType & E_XC_AUTO_TUNE_RGB_GAIN))
            {
                XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY," fine tune gain\n");
                bDone[0] = bDone[1] = bDone[2] = false;
#if 1
                for(ch=0; ch<3; ch++)
                {
                    while(1)
                    {
                        Gain[ch] += 2;
                        MApi_XC_ADC_SetGain((MS_U8)ch, Gain[ch]);
                        _GetGainRange(calibration_type,enAutoTuneType,&Diff);
                        u16Diff = Diff.u16CH_AVG[ch];
                        if ( u16Diff >= 1023 )
                        {
                            bDone[ch] = true;
                            break;
                        }
                    }
                }
#else
                while ( 1 )
                {
                    for (ch=0; ch<3; ch++)
                    {
                        if ( !bDone[ch] )
                        {
                            Gain[ch] += 2 ;
                            MApi_XC_ADC_SetGain(ch, Gain[ch]);
                            _GetGainRange(calibration_type,enAutoTuneType,&Diff);
                            u16Diff = Diff.u16CH_AVG[ch];
                            // Fine tune gain until
                            if ( u16Diff == 1023 )
                            {
                                bDone[ch] = true;
                            }
                        }
                    }

                    if(bDone[0] || bDone[1] || bDone[2])
                    {
                        break;
                    }
                }
#endif
            }
            ///////////////////////////////////////////

            pstADCSetting->u16RedGain =  Gain[0];
            pstADCSetting->u16GreenGain = Gain[1];
            pstADCSetting->u16BlueGain = Gain[2];
            XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"done - \n");
            XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"gain check ok \33[0;31m 0x%x 0x%x 0x%x \33[m \n",Gain[0],Gain[1],Gain[2]);
            return TRUE;
        }
    }

    // If code flow reach here, that's meaning we can not find a good gain value.
    // Choose best gain value in TOLERATE_ERROR.
    if ( !(bDone[0] && bDone[1] && bDone[2]) )
    {
        bDone[0] = bDone[1] = bDone[2] = 0;

        for (ch=0; ch<3; ch++)
        {
            if (u16UpperDiff[ch] <= u16LowerDiff[ch])
            {
                if (u16UpperDiff[ch] <= TOLERATE_ERROR )
                {
                    Gain[ch] = u16UpperGain[ch];
                    bDone[ch] = true;
                }
            }
            else
            {
                if (u16LowerGain[ch] <= TOLERATE_ERROR )
                {
                    Gain[ch] = u16LowerGain[ch];
                    bDone[ch] = true;
                }
            }

        }

    }

    if ( !(bDone[0] && bDone[1] && bDone[2]) )
    {
        return FALSE;
    }

    for (ch=0; ch<3; ch++)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"Chose one best gain value - ");
        pstADCSetting->u16RedGain =  Gain[0];
        pstADCSetting->u16GreenGain = Gain[1];
        pstADCSetting->u16BlueGain = Gain[2];
        MApi_XC_ADC_SetGain((MS_U8)ch, Gain[ch]);
    }

    XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"gain check ok  0x%x 0x%x 0x%x\n",Gain[0],Gain[1],Gain[2]);
    return TRUE;


}

static MS_BOOL _MApi_XC_Auto_Internal_AutoCablication( XC_Auto_TuneType enAutoTuneType, APIXC_AdcGainOffsetSetting *pstADCSetting , SCALER_WIN eWindow)
{
    // Do not use 720P and 1080P timing in Internal calibration.
    #define ADC_720P    0
    #define ADC_1080P   0

    APIXC_AdcGainOffsetSetting tempADCSetting;
    MS_BOOL result;
    MS_BOOL bMirrorBackup = g_XC_InitData.bMirror[eWindow];
    MS_U16 u16BackupHsize, u16BackupVsize;

    XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"======== Start : Calibration Type: %x ======== \n",enAutoTuneType);

    XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"Backup Register setting");

    //MS_AUTOADC_REG_BAK  AutoAdcRegBak;
    MApi_XC_ADC_BackupInternalAutoReg();
    u16BackupHsize = gSrcInfo[eWindow].u16H_SizeAfterPreScaling;
    u16BackupVsize = gSrcInfo[eWindow].u16V_SizeAfterPreScaling;

    MApi_XC_GenerateBlackVideo( ENABLE, eWindow );  //balck screen

    XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"Initial internal calibration setting");


    if(enAutoTuneType & E_XC_AUTO_TUNE_RGB_GAIN)
    {
        if(gSrcInfo[0].enInputSourceType == INPUT_SOURCE_SCART)
        {
            MApi_XC_ADC_InitInternalCalibration(INPUT_SOURCE_SCART, &tempADCSetting);
            MDrv_XC_ADC_Source_Calibrate(_MApi_XC_ADC_ConvertSrcToADCSrc(INPUT_SOURCE_SCART));
        }
        else
        {
            MApi_XC_ADC_InitInternalCalibration(INPUT_SOURCE_VGA, &tempADCSetting);
            MDrv_XC_ADC_Source_Calibrate(_MApi_XC_ADC_ConvertSrcToADCSrc(INPUT_SOURCE_VGA));
        }
    }
    else if(enAutoTuneType & E_XC_AUTO_TUNE_YUV_COLOR)
    {
        MApi_XC_ADC_InitInternalCalibration(INPUT_SOURCE_YPBPR, &tempADCSetting);
        MDrv_XC_ADC_Source_Calibrate(_MApi_XC_ADC_ConvertSrcToADCSrc(INPUT_SOURCE_YPBPR));
    }
    else
    {
        MApi_XC_ADC_InitInternalCalibration(INPUT_SOURCE_SCART, &tempADCSetting);
        MDrv_XC_ADC_Source_Calibrate(_MApi_XC_ADC_ConvertSrcToADCSrc(INPUT_SOURCE_SCART));
    }

#if ADC_720P
    XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"Auto Gen timing: 720p\n");
    MApi_XC_ADC_GenClock(E_ADC_Gen_720P_Clk);
    MApi_XC_GenSpecificTiming(E_XC_720P);
    MApi_XC_InitIPForInternalTiming(E_XC_720P);
    gSrcInfo[eWindow].u16H_SizeAfterPreScaling = 1280;
    gSrcInfo[eWindow].u16V_SizeAfterPreScaling = 720;
#elif ADC_1080P
    XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"Auto Gen timing: 1080p\n");

    MApi_XC_ADC_GenClock(E_ADC_Gen_1080P_Clk);
    MApi_XC_GenSpecificTiming(E_XC_1080P);
    MApi_XC_InitIPForInternalTiming(E_XC_1080P);
    gSrcInfo[eWindow].u16H_SizeAfterPreScaling = 1920;
    gSrcInfo[eWindow].u16V_SizeAfterPreScaling = 1080;
#else
    XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"Auto Gen timing: 480p\n");

    MApi_XC_ADC_GenClock(E_ADC_Gen_480P_Clk);
    MApi_XC_GenSpecificTiming(E_XC_480P);
    MApi_XC_InitIPForInternalTiming(E_XC_480P);
    gSrcInfo[eWindow].u16H_SizeAfterPreScaling = 720;
    gSrcInfo[eWindow].u16V_SizeAfterPreScaling = 480;
#endif
    //MyMDrv_SetCalibrartinWindow(1, 16);
    //MDrv_ADC_gainoffset_reset();
    MApi_XC_ADC_AutoSetting(ENABLE,TRUE,COMPONENT_AUTO_SW_MODE);

    XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"Start Auto Gain Procedure\n");

    result=_MApi_XC_Auto_TuneGain(E_XC_INTERNAL_CALIBRATION,enAutoTuneType, &tempADCSetting , eWindow );

    pstADCSetting->u16RedOffset = tempADCSetting.u16RedOffset;
    pstADCSetting->u16GreenOffset = tempADCSetting.u16GreenOffset;
    pstADCSetting->u16BlueOffset = tempADCSetting.u16BlueOffset;

    pstADCSetting->u16RedGain = tempADCSetting.u16RedGain;
    pstADCSetting->u16GreenGain = tempADCSetting.u16GreenGain;
    pstADCSetting->u16BlueGain = tempADCSetting.u16BlueGain;

    #if 0 //FIXME:  This should be enabled in T2
    if ( result == TRUE )
    {
        MApi_XC_ADC_SetVClampLevel(E_ADC_VClamp_0_9V);
        // Bk25_2D=MDrv_ReadByte(H_BK_ADC_ATOP(0x2d ) );
        // MDrv_WriteByte( H_BK_ADC_ATOP(0x2d ), (Bk25_2D |0x03 ) );
        result=_MApi_XC_Auto_TuneOffset_Internal(enAutoTuneType, pstADCSetting, eWindow);
        // MDrv_WriteByte( H_BK_ADC_ATOP(0x2d ), (Bk25_2D ) );
         if ( result != TRUE )
            printf("offset_fail...\n");

    }
    #endif

//    AUTOMSG(printf("Total Time %04ld ms\n", msAPI_Timer_GetTime0() - u32ADCTime));

    AUTOMSG(printf("gain check ok  0x%02x 0x%02x 0x%02x\n",
        pstADCSetting->u16RedGain,pstADCSetting->u16GreenGain,pstADCSetting->u16BlueGain));

    AUTOMSG(printf("offset check ok  0x%02x 0x%02x 0x%02x\n",
        pstADCSetting->u16RedOffset,pstADCSetting->u16GreenOffset,pstADCSetting->u16BlueOffset));

    MApi_XC_ADC_SetInternalVoltage( E_ADC_Internal_None );

    MsOS_DelayTask(200);
    MApi_XC_AutoGainEnable(DISABLE,eWindow);
    //MDrv_WriteByte(L_BK_IP1F2(0x0E), 0x00); // disable auto gain function
    //MDrv_WriteByte( BK_SELECT_00, u8Bank );
    //test----start----
#if ( GET_RGB_REPORT == ENABLE )
    if ( result == TRUE )
    {
       GetTestReport();

       GetTestReport_Bk1A();
     }
     //test----start----
#endif
    MApi_XC_GenSpecificTiming(E_XC_OFF);
    if(bMirrorBackup == TRUE)
    {
        g_XC_InitData.bMirror[eWindow] = TRUE;
    }
    MApi_XC_ADC_RestoreInternalAdcReg();
    gSrcInfo[eWindow].u16H_SizeAfterPreScaling = u16BackupHsize;
    gSrcInfo[eWindow].u16V_SizeAfterPreScaling = u16BackupVsize;

    return  result;

}


//-----------------------------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------------------------




static void _MApi_XC_Auto_WaitStatusReady(XC_Auto_WaitingType type, SCALER_WIN eWindow  )
{
    MS_U16 u16Dummy = 1000;
    if ( type == E_WAITING_AUTO_POSITION_READY )
        while(!( MApi_XC_IsAutoPositionResultReady(eWindow) ) && (u16Dummy--)) ;
    else
        while(!( MApi_XC_IsAutoGainResultReady(eWindow) ) && (u16Dummy--)) ;
}


//*************************************************************************
//Function name: MDrv_Auto_CheckSyncLoss
//Passing parameter: NO
//Return parameter:
//  MS_BOOL: If PC mode change, return TRUE.
//Description: Check PC mode change when auto-tune executive.
//*************************************************************************
static MS_BOOL _MApi_XC_Auto_CheckSyncLoss(SCALER_WIN eWindow)
{
    MS_BOOL bResult = FALSE;

    bResult = MApi_XC_PCMonitor_InvalidTimingDetect(TRUE, eWindow);

    if(bResult)
    {
        MApi_XC_PCMonitor_Restart(eWindow);
    }

    return bResult;
}


//*************************************************************************
//Function name: _MApi_XC_Auto_GetPosition
//Passing parameter:
//  MS_U8 u8RegIndex: Register index
//  MS_U8 u8VSyncTime: VSync time
//Return parameter:
//  MS_U16: Position value.
//Description: Get position by register select
//*************************************************************************
static MS_U16 _MApi_XC_Auto_GetPosition(XC_Auto_GetAutoPositionType type , MS_U8 u8VSyncTime, SCALER_WIN eWindow)
{

    MS_U16 u16ComparePos=0,u16AutoPos; // position buffer,  Add the initial value
    MS_U8 u8Dummy = 20; // loop dummy
    MS_U8 u8Count = 0; // counter of compare alike
    MS_U16 (*GetPositionFunction)( SCALER_WIN eWindow ) = MApi_XC_SetAutoPositionHstart;//simply assign a trivial initial value


    MApi_XC_AutoPositionEnable(ENABLE, eWindow);

    switch ( type )
    {
        case E_XC_AUTO_GET_HSTART:
            GetPositionFunction = MApi_XC_SetAutoPositionHstart;
            break;
        case E_XC_AUTO_GET_HEND:
            GetPositionFunction = MApi_XC_SetAutoPositionHend;
            break;
        case E_XC_AUTO_GET_VSTART:
            GetPositionFunction = MApi_XC_SetAutoPositionVstart;
            break;
        case E_XC_AUTO_GET_VEND:
            GetPositionFunction = MApi_XC_SetAutoPositionVend;
            break;
        default:
            break;
    }

	//!!! Warning fixed: give u16AutoPos default value !!!
	u16AutoPos = GetPositionFunction(eWindow);
    while(u8Dummy--)
    {
        _MApi_XC_Auto_WaitStatusReady(E_WAITING_AUTO_POSITION_READY, eWindow); // auto position result ready

        u16AutoPos = GetPositionFunction(eWindow); //MDrv_Read2Byte(u32RegIndex) & 0xFFF; // get auto position
        if(u16AutoPos == u16ComparePos) // match
            u8Count++;
        else // different
        {
            u8Count = 0; // reset counter
            u16ComparePos = u16AutoPos; // reset position
        }

        if(u8Count == 3) // match counter ok
            break;

        if(_MApi_XC_Auto_CheckSyncLoss(eWindow)) // check no signal
        {

            return ((MS_U16)-1); // return fail
        }

        MsOS_DelayTask(u8VSyncTime); // wait next frame
    } // while
    return u16AutoPos;
}

#if (!FAST_AUTO_TUNE)
//*************************************************************************
//Function name: MDrv_Auto_SetValidData
//Passing parameter:
//  MS_U8 u8VSyncTime : VSync time for delay
//Return parameter:
//  MS_BOOL: If PC mode change, return FALSE.
//Description: Auto set valid data value.
//*************************************************************************
static MS_BOOL _MApi_XC_Auto_SetValidData(MS_U8 u8VSyncTime, SCALER_WIN eWindow)
{
    MS_U8 u8ValidData; // valide dataa value
    MS_U8 u8PhaseIndex; // phase index
    MS_U16 u16ComapreHPos; // compare horizontal position

    for(u8ValidData = 0x04; u8ValidData != 0x10; u8ValidData++)
    {
        MDrv_XC_SetValidDataThreshold(u8ValidData, eWindow);

        MDrv_XC_ADC_SetPhaseEx(0x00); // set phase
        MsOS_DelayTask(u8VSyncTime);

        u16ComapreHPos = _MApi_XC_Auto_GetPosition( E_XC_AUTO_GET_HSTART ,u8VSyncTime , eWindow); // horizontal position
        for(u8PhaseIndex = 0x01; u8PhaseIndex != 0x10; u8PhaseIndex++)
        {
            MDrv_XC_ADC_SetPhaseEx(u8PhaseIndex * 4); // set phase
            MsOS_DelayTask(u8VSyncTime);

            if(abs(u16ComapreHPos - _MApi_XC_Auto_GetPosition( E_XC_AUTO_GET_HSTART ,u8VSyncTime , eWindow)) > 3) // check lose data
                break;

            if(_MApi_XC_Auto_CheckSyncLoss(eWindow)) // check Sync change
            {
                return FALSE;
            }
        } // for

        if(u8PhaseIndex == 0x10)
            break;
    } // for

    AUTOMSG(printf("Valid=0x%x\n", u8ValidData << 4));
    return TRUE;
}
#endif



//*************************************************************************
//Function name: _MApi_XC_Auto_TunePosition
//Passing parameter:
//  MS_U8 u8VSyncTime : VSync time
//  MS_PCADC_MODESETTING_TYPE *pstModesetting: Current PC mode setting
//Return parameter:
//  MS_BOOL: Success status. (If faile, return FALSE.)
//Description: Auto-tune vertical and horizontal position for PC mode
//*************************************************************************
static MS_BOOL _MApi_XC_Auto_TunePosition(MS_U8 u8VSyncTime, XC_Auto_Signal_Info_Ex *Active , XC_Auto_Signal_Info_Ex *StandardInfo , SCALER_WIN eWindow)
{
    MS_U16 u16PosBff; // position buffer
    MS_BOOL bResult=TRUE;

    // horizotal position
    // auto horizontal start position detected result
    u16PosBff = _MApi_XC_Auto_GetPosition( E_XC_AUTO_GET_HSTART ,u8VSyncTime , eWindow);

#if ENABLE_VGA_EIA_TIMING
    if (MApi_XC_PCMonitor_GetSyncStatus(eWindow) &XC_MD_INTERLACE_BIT)
    {
        u16PosBff++;
    }
#endif

    if(u16PosBff > MAX_PC_AUTO_H_START || u16PosBff < MIN_PC_AUTO_H_START)
    {
        //printf("H start limit: 0x%x (%x->%x)\n", u16PosBff, (MS_U16) MIN_PC_AUTO_H_START, (MS_U16) MAX_PC_AUTO_H_START);
        u16PosBff = StandardInfo->u16HorizontalStart;
        bResult = FALSE;
    }

    Active->u16HorizontalStart = u16PosBff;

    MApi_XC_SetCaptureWindowHstart(u16PosBff , eWindow);

    // vertical positoin
    // auto vertical start position detected result

    u16PosBff =_MApi_XC_Auto_GetPosition( E_XC_AUTO_GET_VSTART ,u8VSyncTime , eWindow);

    if(u16PosBff > MAX_PC_AUTO_V_START || u16PosBff < MIN_PC_AUTO_V_START)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"V start limit: 0x%x (%x->%x)\n", u16PosBff, (MS_U16) MIN_PC_AUTO_V_START, (MS_U16) MAX_PC_AUTO_V_START);
        u16PosBff = StandardInfo->u16VerticalStart;
        bResult = FALSE;
    }

    Active->u16VerticalStart = u16PosBff;

    MApi_XC_SetCaptureWindowVstart(u16PosBff, eWindow);

    XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"AutoPosition VStart=0x%x\n", Active->u16VerticalStart);

    return bResult;
}

//*************************************************************************
//Function name: _MApi_XC_Auto_GetTransPos
//Passing parameter:
//  MS_U8 u8VSyncTime : VSync time
//Return parameter:
//  MS_U8: If faile, return -1. else return phase setting
//Description: Get trans-position.
//*************************************************************************
static MS_U8 _MApi_XC_Auto_GetTransPos(MS_U8 u8VSyncTime, SCALER_WIN eWindow)
{
    MS_U16 u16ComparePos; // compare start position
    MS_U8 u8AdjustPhase = 0x20,u8PhaseDelta = 0x20; // phase data buffer

    MDrv_XC_ADC_SetPhaseEx(0x00); // intialize
    u16ComparePos = _MApi_XC_Auto_GetPosition( E_XC_AUTO_GET_HSTART ,u8VSyncTime , eWindow);

    while(1)
    {
        MDrv_XC_ADC_SetPhaseEx(u8AdjustPhase); // set phase

        u8PhaseDelta /= 2; // next step
        if(u8PhaseDelta == 0x00) // check end
            break;

        if( _MApi_XC_Auto_GetPosition( E_XC_AUTO_GET_HSTART ,u8VSyncTime , eWindow) == u16ComparePos) // find critical phase
            u8AdjustPhase += u8PhaseDelta; // right shift
        else
            u8AdjustPhase -= u8PhaseDelta; // left shift

        if(_MApi_XC_Auto_CheckSyncLoss(eWindow)) // check no signal
        {
            return ((MS_U8)-1);
        }
    } // while

    return (u8AdjustPhase);
}

//*************************************************************************
//Function name: _MApi_XC_Auto_GetActualWidth
//Passing parameter:
//  MS_U8 u8VSyncTime : VSync time
//Return parameter:
//  MS_U16: return actual image width
//Description: Get actual image width.
//*************************************************************************
static MS_U16 _MApi_XC_Auto_GetActualWidth(MS_U8 u8VSyncTime, SCALER_WIN eWindow)
{
    MS_U16 u16HStart; // actual horizontal start
    MS_U16 u16Width;

    MDrv_XC_ADC_SetPhaseEx(0x00); // initialize phase value

    u16HStart =  _MApi_XC_Auto_GetPosition( E_XC_AUTO_GET_HSTART ,u8VSyncTime , eWindow); // horizontal start position
    XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"Get %s Window's VSync Time =%d and H Start=%d\n", eWindow?("Sub"):("Main"), u8VSyncTime, u16HStart);
    _MApi_XC_Auto_GetTransPos(u8VSyncTime, eWindow); // seek critical phase

    u16Width = (( _MApi_XC_Auto_GetPosition( E_XC_AUTO_GET_HEND ,u8VSyncTime , eWindow) - u16HStart) + 1); // actual image width

    return u16Width;
}

//*************************************************************************
//Function name: _MApi_XC_Auto_TuneHTotal
//Passing parameter:
//  MS_U8 u8VSyncTime: VSync time
//  MS_PCADC_MODESETTING_TYPE *pstModesetting: Current PC mode setting
//Return parameter:
//  MS_BOOL: Success status. (If faile, return FALSE.)
//Description: auto-tune horizontal total.
//*************************************************************************
static MS_BOOL _MApi_XC_Auto_TuneHTotal(MS_U8 u8VSyncTime, XC_Auto_Signal_Info_Ex *Active , XC_Auto_Signal_Info_Ex *StandardInfo , SCALER_WIN eWindow)
{
    MS_U16 u16ActualWidth; // actual width
    MS_U16 u16StdWidth; // standard width
    MS_U16 u16HTotalBff; // horizontal total buffer
    MS_BOOL bResult = FALSE;

    MDrv_XC_ADC_SetPhaseEx(0x00); // initialize phase value

    // Get Active width from Scaler.
    u16ActualWidth = _MApi_XC_Auto_GetActualWidth(u8VSyncTime, eWindow);

    // Get standard width from PCmonitor (mode table)
    u16StdWidth = StandardInfo->u16HResolution;

    XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"Step1 u16StdWidth=%u, u16ActualWidth=%u\n",u16StdWidth, u16ActualWidth);

    if(abs(u16ActualWidth - u16StdWidth) > (u16StdWidth / 6)) // check actual width over standard
    {
        XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"AWoverSW(%u,%u)\n", u16ActualWidth, u16StdWidth);
        return FALSE;
    }
    u16HTotalBff = Active->u16HorizontalTotal; // intialize horizontal total buffer
    XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"Step2 Htt=%u, Std Htt=%u\n", u16HTotalBff, StandardInfo->u16HorizontalTotal);

    if(abs(u16ActualWidth - u16StdWidth) > 1) // check width difference
    {
        // calculate horizontal total
        u16HTotalBff = (MS_U16)(((MS_U32) Active->u16HorizontalTotal * u16StdWidth) / u16ActualWidth);

        // check over range of adjusting
        if(abs(u16HTotalBff - StandardInfo->u16HorizontalTotal) > (Active->u16HorizontalTotal/4) )
        {
            //AUTOMSG(printf("OverADJ %u\n", ADJUST_CLOCK_RANGE));
            XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"OverADJ\n");
            return FALSE;
        }
        MApi_XC_ADC_SetPcClock(u16HTotalBff); // set clock
    }
    // check width
    u16ActualWidth = _MApi_XC_Auto_GetActualWidth(u8VSyncTime, eWindow);
    XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"Step3 HTT=%u, Actual Width=%u\n", u16HTotalBff, u16ActualWidth);

    if(u16ActualWidth != u16StdWidth) // match width
    {
        // adjust horizontal total
        u16HTotalBff = u16HTotalBff + (u16StdWidth - u16ActualWidth);

        MApi_XC_ADC_SetPcClock(u16HTotalBff);

        u16ActualWidth = _MApi_XC_Auto_GetActualWidth(u8VSyncTime, eWindow);

        // adjust horizontal total again
        u16HTotalBff = u16HTotalBff + (u16StdWidth - u16ActualWidth);
    }
    XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"Step4 HTT=%u, Actual Width=%u\n", u16HTotalBff, u16ActualWidth);
    if(u16HTotalBff & 0x01) // match width and check odd
    {
        MApi_XC_ADC_SetPcClock(u16HTotalBff - 1); // find decrement

        u16ActualWidth = _MApi_XC_Auto_GetActualWidth(u8VSyncTime, eWindow);
        if(u16ActualWidth == u16StdWidth) // match width
            u16HTotalBff--;
        else
        {
            MApi_XC_ADC_SetPcClock(u16HTotalBff + 1); // find increment
            u16ActualWidth = _MApi_XC_Auto_GetActualWidth(u8VSyncTime, eWindow);
            if(u16ActualWidth == u16StdWidth) // match width
                u16HTotalBff++;
        }
    }
    XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"Step5 HTT=%u, Actual Width=%u\n", u16HTotalBff, u16ActualWidth);
    // check horizontal total range
    if(abs(u16HTotalBff - (StandardInfo->u16HorizontalTotal)) < (StandardInfo->u16HorizontalTotal/4) )
    {
        Active->u16HorizontalTotal = u16HTotalBff;
        Active->u16HResolution = u16ActualWidth;
        bResult = TRUE;
    }
    else
    {
        Active->u16HResolution = 0xFFFF;
    }

    MApi_XC_ADC_SetPcClock(Active->u16HorizontalTotal); // setting ADC clock
    MDrv_XC_ADC_SetPhaseEx(Active->u16Phase); // setting ADC phase

    XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"AutoHtt bResult=%u, Htt = %u, Actual Width=%u\n", bResult, Active->u16HorizontalTotal, u16ActualWidth);
    return bResult;
}


//*************************************************************************
//Function name: _MApi_XC_Auto_TunePhase
//Passing parameter:
//  MS_U8 u8VSyncTime: VSync time
//  MS_PCADC_MODESETTING_TYPE *pstModesetting: Current PC mode setting
//Return parameter:
//  MS_BOOL: Success status. (If faile, return FALSE.)
//Description: auto-tune phase.
//*************************************************************************

MS_BOOL _MApi_XC_Auto_TunePhase(MS_U8 u8VSyncTime, XC_Auto_Signal_Info_Ex *Active, SCALER_WIN eWindow)
{

#if AUTO_PHASE_METHOD
    MS_U8 u8Index; // loop index
    MS_U16 u16PhaseRange = MApi_XC_ADC_GetPhaseRange() - 1;
    MS_U32 u32AutoPhaseVal; // auto phase value result
    MS_U32 u32MiniPhaseVal = -1; // minimum phase value
    MS_U8 u8WorstPhase1,u8WorstPhase2;

    u8WorstPhase1 = 0x00; // initizlize
    for(u8Index = u8WorstPhase1; u8Index <= u16PhaseRange; u8Index += AUTO_PHASE_STEP)
    {
        MDrv_XC_ADC_SetPhaseEx(u8Index);
        MsOS_DelayTask(u8VSyncTime); // delay 1 frame
        u32AutoPhaseVal = MApi_XC_GetPhaseValue(eWindow);

        if(u32AutoPhaseVal < u32MiniPhaseVal) // check minimum
        {
            u8WorstPhase1 = u8Index; // refresh best phase
            u32MiniPhaseVal = u32AutoPhaseVal; // refresh minimum value
        }

        if(_MApi_XC_Auto_CheckSyncLoss(eWindow)) // check no signal
            return FALSE;
    } // for

    // initizlize
    u8WorstPhase2 = (u8WorstPhase1 - AUTO_PHASE_STEP + 1) & u16PhaseRange;
    u8WorstPhase1 = (u8WorstPhase1 + AUTO_PHASE_STEP) & u16PhaseRange;
    u32MiniPhaseVal = -1;
    for(u8Index = u8WorstPhase2; u8Index != u8WorstPhase1; u8Index = ((u8Index + 1) & u16PhaseRange))
    {
        MDrv_XC_ADC_SetPhaseEx(u8Index);
        MsOS_DelayTask(u8VSyncTime); // delay 1 frame
        u32AutoPhaseVal = MApi_XC_GetPhaseValue(eWindow);

        if(u32AutoPhaseVal < u32MiniPhaseVal) // check minimum
        {
            u8WorstPhase2 = u8Index; // refresh best phase
            u32MiniPhaseVal = u32AutoPhaseVal; // refresh minimum value
        }

        if(_MApi_XC_Auto_CheckSyncLoss(eWindow)) // check no signal
            return FALSE;
    } // for

    Active->u16Phase = (u8WorstPhase2 + (u16PhaseRange / 2)) & u16PhaseRange;
    MDrv_XC_ADC_SetPhaseEx(Active->u16Phase);

#else

    MS_U16 u16PhaseRange = MApi_XC_ADC_GetPhaseRange();
    const MS_U16 u16PhaseStep = (u16PhaseRange + 63) / 64;

    MS_U32 u32AutoPhaseVal, u32MaxPhaseVal = 0; // maximum phase value
    MS_U16 u16CurPhase, u16BestPhase = 0;

    for (u16CurPhase = 0; u16CurPhase < u16PhaseRange; u16CurPhase += u16PhaseStep)
    {
        MDrv_XC_ADC_SetPhaseEx(u16CurPhase);
        MsOS_DelayTask(u8VSyncTime);

        u32AutoPhaseVal = MApi_XC_GetPhaseValue(eWindow);

        if (u32AutoPhaseVal > u32MaxPhaseVal) // check maximum
        {
            u16BestPhase = u16CurPhase;
            u32MaxPhaseVal = u32AutoPhaseVal;
        }

        if (_MApi_XC_Auto_CheckSyncLoss(eWindow))
            return FALSE;
    }

    // 2nd stage for searching best phase around prvious "BestPhase"
    // initizlize (Set range)
    u16CurPhase   = u16BestPhase == 0 ? 0 : u16BestPhase - u16PhaseStep + 1;
    u16PhaseRange = u16BestPhase + u16PhaseStep;
    u32MaxPhaseVal = 0;

    for (; u16CurPhase < u16PhaseRange; u16CurPhase += 1)
    {
        MDrv_XC_ADC_SetPhaseEx(u16CurPhase);
        MsOS_DelayTask(u8VSyncTime);

        u32AutoPhaseVal = MApi_XC_GetPhaseValue(eWindow);

        if (u32AutoPhaseVal > u32MaxPhaseVal) // check maximum
        {
            u16BestPhase = u16CurPhase;
            u32MaxPhaseVal = u32AutoPhaseVal;
        }

        if (_MApi_XC_Auto_CheckSyncLoss(eWindow))
            return FALSE;
    }

    MDrv_XC_ADC_SetPhaseEx(u16BestPhase);
    Active->u16Phase = u16BestPhase;
#endif

    //printf("AutoPhase %x\n", Active->u16Phase);
    return TRUE;
}

//*************************************************************************
//Function name: _MApi_XC_Auto_TuneOffset
//Passing parameter:
//  MS_U8 u8VSyncTime: VSync time
//  APIXC_AdcGainOffsetSetting * pstADCSetting: ADC setting of Current PC mode
//Return parameter:
//  MS_BOOL: Success status. (If faile, return FALSE.)
//Description: Auto-tune R/G/B Offset of ADC.
//*************************************************************************

#if 1
static MS_BOOL _MApi_XC_Auto_TuneOffset(MS_U8 u8VSyncTime, APIXC_AdcGainOffsetSetting *pstADCSetting)
{
    APIXC_AdcGainOffsetSetting tempADCSetting = {0,0,0,0,0,0};//trivial initial value is assigned
    MS_U16 u16OffsetDelta = MApi_XC_ADC_GetCenterGain(); // adjust step
    MS_U8 u8ATGStatus = 0; // auto gain status//ERROR FIX Prevent Tool 070522
    MS_U8 u8FlowFlag = 0x00; // underflow or overflow flag

    MApi_XC_ADC_InitExternalCalibration(INPUT_SOURCE_VGA, &tempADCSetting);

    while(1)
    {
        u16OffsetDelta /= 2; // next

        if(u16OffsetDelta == 0x00) // check end
            break;

        MApi_XC_ADC_AdjustGainOffset(&tempADCSetting);
        MsOS_DelayTask(u8VSyncTime * 3); // wait stable
        _MApi_XC_Auto_WaitStatusReady(E_WAITING_AUTO_GAIN_READY, MAIN_WINDOW);

        {
            MS_U16 u16ResultR,u16ResultG,u16ResultB;

            u16ResultR = MApi_XC_GetAutoGainResult(AUTO_MIN_R, u8VSyncTime, MAIN_WINDOW);
            u16ResultG = MApi_XC_GetAutoGainResult(AUTO_MIN_G, u8VSyncTime, MAIN_WINDOW);
            u16ResultB = MApi_XC_GetAutoGainResult(AUTO_MIN_B, u8VSyncTime, MAIN_WINDOW);
            AUTOMSG(printf(" Result R: %d  G: %d  B: %d \n", u16ResultR,u16ResultG,u16ResultB));
            if (u16ResultR == 0x00)
                u8ATGStatus |= BIT(2);
            else
                u8ATGStatus &= ~BIT(2);

            if (u16ResultG == 0x00)
                u8ATGStatus |= BIT(1);
            else
                u8ATGStatus &= ~BIT(1);


            if (u16ResultB == 0x00)
                u8ATGStatus |= BIT(0);
            else
                u8ATGStatus &= ~BIT(0);
        }

        // red
        if(u8ATGStatus & BIT(2) )
        {
            tempADCSetting.u16RedOffset += u16OffsetDelta;
            u8FlowFlag |= BIT(0);
        }
        else
        {
            tempADCSetting.u16RedOffset -= u16OffsetDelta;
            u8FlowFlag |= BIT(1);
        }

        // green
        if( u8ATGStatus & BIT(1) )
        {
            tempADCSetting.u16GreenOffset += u16OffsetDelta;
            u8FlowFlag |= BIT(2);
        }
        else
        {
            tempADCSetting.u16GreenOffset -= u16OffsetDelta;
            u8FlowFlag |= BIT(3);
        }

        // blue
        if(u8ATGStatus & BIT(0))
        {
            tempADCSetting.u16BlueOffset += u16OffsetDelta;
            u8FlowFlag |= BIT(4);
        }
        else
        {
            tempADCSetting.u16BlueOffset -= u16OffsetDelta;
            u8FlowFlag |= BIT(5);
        }
        AUTOMSG(printf("OffATG=0x%x,FF=0x%x\n", u8ATGStatus, u8FlowFlag));
    } // while

    // adjust offset after auto-tune
    if(u8FlowFlag == 0x3f)
    {
        pstADCSetting->u16RedOffset = tempADCSetting.u16RedOffset;
        pstADCSetting->u16GreenOffset = tempADCSetting.u16GreenOffset;
        pstADCSetting->u16BlueOffset = tempADCSetting.u16BlueOffset;
    }
    AUTOMSG(printf(" offset R: %d	G: %d  B: %d \n",pstADCSetting->u16RedOffset,
    pstADCSetting->u16GreenOffset,
    pstADCSetting->u16BlueOffset));

    // if calibration successed, will write new value, else, write original value
    MApi_XC_ADC_AdjustGainOffset(pstADCSetting);

    return (u8FlowFlag == 0x3F ? TRUE : FALSE);
}
#endif

MS_BOOL _MApi_XC_Auto_TuneRGBGain(MS_U8 u8VSyncTime, APIXC_AdcGainOffsetSetting *pstADCSetting,SCALER_WIN eWindow)
{
    APIXC_AdcGainOffsetSetting tempADCSetting={0,0,0,0,0,0};//trivial initial value is assigned

    MS_U16 u8GainDelta = MApi_XC_ADC_GetCenterGain();
    MS_U8 u8ATGStatus = 0 ;
    MS_U8 u8FlowFlag = 0x00;

    APIXC_AdcGainOffsetSetting* p_tempADCSetting = &tempADCSetting;

    MApi_XC_ADC_InitExternalCalibration(INPUT_SOURCE_VGA, &tempADCSetting);

    // MApi_XC_ADC_InitExternalCalibration will overwrite offset setting updating by software auto tune.
    // Roll back them here before tuning gain
    if (!MDrv_XC_ADC_use_hardware_auto_offset(INPUT_SOURCE_VGA))
    {
        tempADCSetting.u16RedOffset = pstADCSetting->u16RedOffset ;
        tempADCSetting.u16GreenOffset =pstADCSetting->u16GreenOffset ;
        tempADCSetting.u16BlueOffset =pstADCSetting->u16BlueOffset ;
        MDrv_XC_ADC_offset_setting((XC_AdcGainOffsetSetting*)p_tempADCSetting );
    }

    while (1)
    {
        u8GainDelta /= 2; // next

        if (u8GainDelta == 0x00) // check end
            break;

        MDrv_XC_ADC_gain_setting((XC_AdcGainOffsetSetting*)p_tempADCSetting);
        MsOS_DelayTask(u8VSyncTime * 3); // wait stable
        _MApi_XC_Auto_WaitStatusReady(E_WAITING_AUTO_GAIN_READY, eWindow);
#if 0
        {
            MS_U16 u16ResultR,u16ResultG,u16ResultB;

            u16ResultR = MApi_XC_GetAutoGainResult(AUTO_MAX_R, u8VSyncTime,eWindow);
            u16ResultG = MApi_XC_GetAutoGainResult(AUTO_MAX_G, u8VSyncTime,eWindow);
            u16ResultB = MApi_XC_GetAutoGainResult(AUTO_MAX_B, u8VSyncTime,eWindow);
            printf("gain value %d \n",tempADCSetting.u16RedGain);
            printf("Rresult :%d  Gresult :%d  Bresult :%d \n", u16ResultR, u16ResultG,u16ResultB);

            if (u16ResultR == 0x3FF)
                u8ATGStatus |= BIT(2);
            else
                u8ATGStatus &= ~BIT(2);

            if (u16ResultG == 0x3FF)
                u8ATGStatus |= BIT(1);
            else
                u8ATGStatus &= ~BIT(1);


            if (u16ResultB == 0x3FF)
                u8ATGStatus |= BIT(0);
            else
                u8ATGStatus &= ~BIT(0);
        }
#else
        u8ATGStatus = MApi_XC_GetAutoGainMaxValueStatus(MAIN_WINDOW);
#endif
        // red
        if (u8ATGStatus & BIT(2))
        {
            tempADCSetting.u16RedGain -= u8GainDelta;
            u8FlowFlag |= BIT(0);
        }
        else
        {
            tempADCSetting.u16RedGain += u8GainDelta;
            u8FlowFlag |= BIT(1);
        }

        // green
        if (u8ATGStatus & BIT(1) )
        {
            tempADCSetting.u16GreenGain -= u8GainDelta;
            u8FlowFlag |= BIT(2);
        }
        else
        {
            tempADCSetting.u16GreenGain += u8GainDelta;
            u8FlowFlag |= BIT(3);
        }

        // blue
        if (u8ATGStatus & BIT(0) )
        {
            tempADCSetting.u16BlueGain -= u8GainDelta;
            u8FlowFlag |= BIT(4);
        }
        else
        {
            tempADCSetting.u16BlueGain += u8GainDelta;
            u8FlowFlag |= BIT(5);
        }
        AUTOMSG(printf("GainATG=0x%x,FF=0x%x\n", u8ATGStatus, u8FlowFlag));
    } // while

    // adjust gain after auto-tune
    if (u8FlowFlag == 0x3f)
    {
        pstADCSetting->u16RedGain = tempADCSetting.u16RedGain;
        pstADCSetting->u16GreenGain = tempADCSetting.u16GreenGain;
        pstADCSetting->u16BlueGain = tempADCSetting.u16BlueGain;
    }

    //printf(" Gain R: %d	G: %d  B: %d \n",pstADCSetting->u16RedGain,
    //pstADCSetting->u16GreenGain,
    //pstADCSetting->u16BlueGain);

    // if calibration successed, will write new value, else, write original value
    MApi_XC_ADC_AdjustGainOffset(pstADCSetting);

    return (u8FlowFlag == 0x3F ? TRUE : FALSE);
}

MS_BOOL _MApi_XC_Auto_TuneScartRGBGain(MS_U8 u8VSyncTime, APIXC_AdcGainOffsetSetting *pstADCSetting,SCALER_WIN eWindow)
{
    APIXC_AdcGainOffsetSetting tempADCSetting = {0,0,0,0,0,0};//trivial initial value is assigned
	APIXC_AdcGainOffsetSetting* p_tempADCSetting = NULL;

    MS_U8 u8GainDelta = 0x80; // adjust step
    MS_U8 u8ATGStatus = 0 ; // auto gain status
    MS_U8 u8FlowFlag = 0x00; // underflow or overflow flag
    MS_BOOL bDone[] = {FALSE,FALSE,FALSE};
	MS_U16 u16ResultR,u16ResultG,u16ResultB;
    //MS_U8 u8TimeoutCnt = 0;

    //APIXC_AdcGainOffsetSetting* p_tempADCSetting = &tempADCSetting;

    //tempADCSetting.u16RedGain = tempADCSetting.u16GreenGain= tempADCSetting.u16RedGain = 0x1000;

    MApi_XC_ADC_InitExternalCalibration(INPUT_SOURCE_SCART, &tempADCSetting);
    MApi_XC_ADC_AdjustGainOffset(&tempADCSetting);

    while (1)
    {
        u8GainDelta /= 2; // next

        if (u8GainDelta == 0x00) // check end
            break;

        p_tempADCSetting = &tempADCSetting;
        MDrv_XC_ADC_gain_setting((XC_AdcGainOffsetSetting*) p_tempADCSetting);
        //printf(" Gain R: %d  G: %d  B: %d \n",tempADCSetting.u16RedGain,
        //tempADCSetting.u16GreenGain,
        //tempADCSetting.u16BlueGain);

        //MsOS_DelayTask(u8VSyncTime * 3); // wait stable
        //_MApi_XC_Auto_WaitStatusReady(E_WAITING_AUTO_GAIN_READY, eWindow);

        {
            //MS_U16 u16ResultR,u16ResultG,u16ResultB;

            u16ResultR = MApi_XC_GetAutoGainResult(AUTO_MAX_R, u8VSyncTime, MAIN_WINDOW);
            u16ResultG = MApi_XC_GetAutoGainResult(AUTO_MAX_G, u8VSyncTime, MAIN_WINDOW);
            u16ResultB = MApi_XC_GetAutoGainResult(AUTO_MAX_B, u8VSyncTime, MAIN_WINDOW);
            //printf(" Result R: %d  G: %d  B: %d \n", u16ResultR,u16ResultG,u16ResultB);

            // Target is 235 ( 10 bit mode : 235 * 4 = 940 )
            if (u16ResultR >= 0x3A0)
                u8ATGStatus |= BIT(2);
            else
                u8ATGStatus &= ~BIT(2);

            if (u16ResultG >= 0x3A0)
                u8ATGStatus |= BIT(1);
            else
                u8ATGStatus &= ~BIT(1);


            if (u16ResultB >= 0x3A0)
                u8ATGStatus |= BIT(0);
            else
                u8ATGStatus &= ~BIT(0);
        }

        //u8ATGStatus = MApi_XC_GetAutoGainMaxValueStatus(MAIN_WINDOW);

        // red
        if (u8ATGStatus & BIT(2))
        {
            tempADCSetting.u16RedGain -= u8GainDelta;
            u8FlowFlag |= BIT(0);
        }
        else
        {
            tempADCSetting.u16RedGain += u8GainDelta;
            u8FlowFlag |= BIT(1);
        }

        // green
        if (u8ATGStatus & BIT(1) )
        {
            tempADCSetting.u16GreenGain -= u8GainDelta;
            u8FlowFlag |= BIT(2);
        }
        else
        {
            tempADCSetting.u16GreenGain += u8GainDelta;
            u8FlowFlag |= BIT(3);
        }

        // blue
        if (u8ATGStatus & BIT(0) )
        {
            tempADCSetting.u16BlueGain -= u8GainDelta;
            u8FlowFlag |= BIT(4);
        }
        else
        {
            tempADCSetting.u16BlueGain += u8GainDelta;
            u8FlowFlag |= BIT(5);
        }
        AUTOMSG(printf("GainATG=0x%x,FF=0x%x\n", u8ATGStatus, u8FlowFlag));
    } // while
    // adjust gain after auto-tune
    #if 0
    if (u8FlowFlag == 0x3f)
    {

        printf(" success !! \n");
        pstADCSetting->u16RedGain = tempADCSetting.u16RedGain;
        pstADCSetting->u16GreenGain = tempADCSetting.u16GreenGain;
        pstADCSetting->u16BlueGain = tempADCSetting.u16BlueGain;
    }
    #endif

    while (1)
    {
        //MS_U16 u16ResultR,u16ResultG,u16ResultB;

        u16ResultR = MApi_XC_GetAutoGainResult(AUTO_MAX_R, u8VSyncTime, MAIN_WINDOW);
        u16ResultG = MApi_XC_GetAutoGainResult(AUTO_MAX_G, u8VSyncTime, MAIN_WINDOW);
        u16ResultB = MApi_XC_GetAutoGainResult(AUTO_MAX_B, u8VSyncTime, MAIN_WINDOW);
        //printf("2nd Result R: %d  G: %d  B: %d \n", u16ResultR,u16ResultG,u16ResultB);
#if 1
        // Target is 235 ( 10 bit mode : 235 * 4 = 940 )
        if (u16ResultR >= 0x3AC)
            bDone[0] = TRUE;
        else
            tempADCSetting.u16RedGain+=2;

        if (u16ResultG >= 0x3AC)
            bDone[1] = TRUE;
        else
            tempADCSetting.u16GreenGain+=2;

        if (u16ResultB >= 0x3AC)
            bDone[2] = TRUE;
        else
            tempADCSetting.u16BlueGain+=2;
#else
        // Target is 235 ( 10 bit mode : 235 * 4 = 940 )
        if (u16ResultR >= 0x3AC)
            bDone[0] = TRUE;
        else
            tempADCSetting.u16RedGain++;

        if (u16ResultG >= 0x3AC)
            bDone[1] = TRUE;
        else
            tempADCSetting.u16GreenGain++;

        if (u16ResultB >= 0x3AC)
            bDone[2] = TRUE;
        else
            tempADCSetting.u16BlueGain++;

#endif

        p_tempADCSetting = &tempADCSetting;
        MDrv_XC_ADC_gain_setting((XC_AdcGainOffsetSetting*)p_tempADCSetting);

        if ( bDone[0] && bDone[1] && bDone[2] )
            break;
    }

    // if calibration successed, will write new value, else, write original value
    if (bDone[0] && bDone[1] && bDone[2])
    {
        AUTOMSG(printf("Calibration success !! \n"));
        // Because there are variaty between IP1 HW and memory.
        // Add Gain value on all channel for this variaty (All channel have reach to 0x3AC by reading from memory)
        // Add Gain value on all channel 26. (We do not know 0~255. The only part we know is 0~235.)
        pstADCSetting->u16RedGain = tempADCSetting.u16RedGain + 0x220;
        pstADCSetting->u16GreenGain = tempADCSetting.u16GreenGain + 0x220;
        pstADCSetting->u16BlueGain = tempADCSetting.u16BlueGain + 0x220;
        pstADCSetting->u16RedOffset = tempADCSetting.u16RedOffset;
        pstADCSetting->u16GreenOffset = tempADCSetting.u16GreenOffset;
        pstADCSetting->u16BlueOffset = tempADCSetting.u16BlueOffset;
        MApi_XC_ADC_AdjustGainOffset(pstADCSetting);
        MApi_XC_ADC_ExitExternalCalibration(INPUT_SOURCE_SCART, &tempADCSetting);
        return TRUE;
    }

    // calibration failed, restore original value
    MApi_XC_ADC_AdjustGainOffset(pstADCSetting);
    MApi_XC_ADC_ExitExternalCalibration(INPUT_SOURCE_SCART, &tempADCSetting);
    return FALSE;

    //return (u8FlowFlag == 0x3F ? TRUE : FALSE);
}


//*************************************************************************
//Function name: _MApi_XC_Auto_ColorYUV
//Passing parameter:
//  MS_U8 u8VSyncTime: VSync time
//  APIXC_AdcGainOffsetSetting * pstADCSetting: ADC setting of Current YCbCr mode
//Return parameter:
//  MS_BOOL: Success status. (If faile, return FALSE.)
//Description: Auto-tune Y gain of ADC.
//*************************************************************************
#if COMPONENT_AUTO_SW_MODE

static MS_BOOL _MApi_XC_Auto_ColorYUV(MS_U8 u8VSyncTime, APIXC_AdcGainOffsetSetting *pstADCSetting)
{
    APIXC_AdcGainOffsetSetting tempADCSetting;
    MS_U8 u8ATGStatus =0, u8Tmp, u8TuneStep = 4;
    MS_U16 u8ResultR,u8ResultG,u8ResultB;

    // Using SMPTE 100% or 75% depends on the definition of devAuto.h
    tempADCSetting.u16RedGain = 0x1000;
    tempADCSetting.u16GreenGain = 0x1000;
    tempADCSetting.u16BlueGain = 0x1000;
    tempADCSetting.u16RedOffset = 0x800;
    tempADCSetting.u16GreenOffset =0x100;
    tempADCSetting.u16BlueOffset = 0x800;

    //MApi_XC_ADC_AutoSetting(DISABLE, TRUE, COMPONENT_AUTO_SW_MODE);

    // Tune Gain
    for(u8Tmp=0; u8Tmp<0xFF; u8Tmp++)
    {
        MApi_XC_ADC_AdjustGainOffset(&tempADCSetting);

        MsOS_DelayTask(u8VSyncTime *3 ); // wait stable

        // Get Pr active value (Max. value - Min. value)
        u8ResultR = MApi_XC_GetAutoGainResult(AUTO_MIN_R, u8VSyncTime, MAIN_WINDOW);
        AUTOMSG(printf("Min R: 0x%x ; ", u8ResultR));
        u8ResultR = MApi_XC_GetAutoGainResult(AUTO_MAX_R, u8VSyncTime, MAIN_WINDOW) - u8ResultR;
        // Get Y active value (Max. value - Min. value)
        u8ResultG = MApi_XC_GetAutoGainResult(AUTO_MIN_G, u8VSyncTime, MAIN_WINDOW);
        AUTOMSG(printf("Min G: 0x%x ; ", u8ResultG));
        u8ResultG = MApi_XC_GetAutoGainResult(AUTO_MAX_G, u8VSyncTime, MAIN_WINDOW) - u8ResultG;
        // Get Pb active value (Max. value - Min. value)
        u8ResultB = MApi_XC_GetAutoGainResult(AUTO_MIN_B, u8VSyncTime, MAIN_WINDOW);
        AUTOMSG(printf("Min B: 0x%x; \n", u8ResultB));
        u8ResultB = MApi_XC_GetAutoGainResult(AUTO_MAX_B, u8VSyncTime, MAIN_WINDOW) - u8ResultB;
        AUTOMSG(printf("Diff R=0x%x, Diff G=0x%x,Diff B=0x%x\n", u8ResultR, u8ResultG, u8ResultB));

        if((u8ResultR >= (PbPr_AUTO_ACTIVE_RANGE<<2)) && (u8ResultR <= ((PbPr_AUTO_ACTIVE_RANGE<<2)+1)))  // Range: 0x10~0xF0
        //if(u8ResultR == (PbPr_AUTO_ACTIVE_RANGE<<2))
        {
            u8ATGStatus |= BIT(4);
        }
        else
        {
            tempADCSetting.u16RedGain += (signed)((PbPr_AUTO_ACTIVE_RANGE<<2)*u8TuneStep - u8ResultR*u8TuneStep);
            u8ATGStatus &= ~BIT(4);
        }

        if( (u8ResultG >= (Y_AUTO_ACTIVE_RANGE<<2)) && (u8ResultG <= ((Y_AUTO_ACTIVE_RANGE<<2)+1)) )  // Range: 0x00~0xDB
        //if(u8ResultG == (Y_AUTO_ACTIVE_RANGE<<2))
        {
            u8ATGStatus |= BIT(3);
        }
        else
        {
            tempADCSetting.u16GreenGain += (signed)((Y_AUTO_ACTIVE_RANGE<<2)*u8TuneStep - u8ResultG*u8TuneStep);
            u8ATGStatus &= ~BIT(3);
        }

        if( (u8ResultB >= (PbPr_AUTO_ACTIVE_RANGE<<2)) && (u8ResultB <= ((PbPr_AUTO_ACTIVE_RANGE<<2)+1)))  // Range: 0x10~0xF0
        //if(u8ResultB == (PbPr_AUTO_ACTIVE_RANGE<<2))
        {
            u8ATGStatus |= BIT(2);
        }
        else
        {
            tempADCSetting.u16BlueGain += (signed)((PbPr_AUTO_ACTIVE_RANGE<<2)*u8TuneStep - u8ResultB*u8TuneStep);
            u8ATGStatus &= ~BIT(2);
        }

        AUTOMSG(printf("u8ATGStatus=0x%x\n", u8ATGStatus));
        if(u8ATGStatus == 0x1C)
            break;
    }

    AUTOMSG(printf("R gain: 0x%x, G gain: 0x%x, B gain: %x\n",tempADCSetting.u16RedGain, tempADCSetting.u16GreenGain, tempADCSetting.u16BlueGain));

    // Tune Offset
    for(u8Tmp=0; u8Tmp<0xFF; u8Tmp++)
    {
        MApi_XC_ADC_AdjustGainOffset(&tempADCSetting);

        MsOS_DelayTask(u8VSyncTime * 3); // wait stable

        u8ResultR = MApi_XC_GetAutoGainResult(AUTO_MIN_R, u8VSyncTime, MAIN_WINDOW);
        u8ResultG = MApi_XC_GetAutoGainResult(AUTO_MIN_G, u8VSyncTime, MAIN_WINDOW);
        u8ResultB = MApi_XC_GetAutoGainResult(AUTO_MIN_B, u8VSyncTime, MAIN_WINDOW);
        AUTOMSG(printf("Min R=0x%x,G=0x%x,B=0x%x\n", u8ResultR, u8ResultG, u8ResultB));
        AUTOMSG(printf("Max R=0x%x,G=0x%x,B=0x%x\n", (MApi_XC_GetAutoGainResult(AUTO_MAX_R, u8VSyncTime, MAIN_WINDOW)>>2),
            (MApi_XC_GetAutoGainResult(AUTO_MAX_G, u8VSyncTime, MAIN_WINDOW)>>2), (MApi_XC_GetAutoGainResult(AUTO_MAX_B, u8VSyncTime, MAIN_WINDOW)>>2)));

        // Red - Pr
        if ( (u8ResultR >= (PbPr_AUTO_MIN_VALUE<<2)) && (u8ResultR <= ((PbPr_AUTO_MIN_VALUE<<2)+1)) )    // Range (0x10~0xF0)
        //if ( u8ResultR == (PbPr_AUTO_MIN_VALUE<<2) )
        {
            u8ATGStatus |= BIT(7);
        }
        else
        {
            AUTOMSG(printf("old R offset: 0x%x\n", tempADCSetting.u16RedOffset));
            tempADCSetting.u16RedOffset += (signed)((PbPr_AUTO_MIN_VALUE<<2) - (u8ResultR));
            AUTOMSG(printf("new R offset: 0x%x\n", tempADCSetting.u16RedOffset));
            u8ATGStatus &= ~BIT(7);
        }

        // Green - Y
        if ( (u8ResultG >= (Y_MIN_VALUE<<2)) && (u8ResultG <= ((Y_MIN_VALUE<<2)+1)) ) // Range (0x10~0xEB)
        //if ( u8ResultG == (Y_MIN_VALUE<<2) )
        {
            u8ATGStatus |= BIT(6);
        }
        else
        {
            AUTOMSG(printf("old G offset: 0x%x\n", tempADCSetting.u16GreenOffset));
            tempADCSetting.u16GreenOffset += (signed)((Y_MIN_VALUE<<2) - (u8ResultG));
            AUTOMSG(printf("new G offset: 0x%x\n", tempADCSetting.u16GreenOffset));
            u8ATGStatus &= ~BIT(6);
        }

        // Blue - Pb
        if ( (u8ResultB >= (PbPr_AUTO_MIN_VALUE<<2)) && (u8ResultB <= ((PbPr_AUTO_MIN_VALUE<<2)+1)) )    // Range (0x10~0xF0)
        //if ( u8ResultB == (PbPr_AUTO_MIN_VALUE<<2) )
        {
            u8ATGStatus |= BIT(5);
        }
        else
        {
            AUTOMSG(printf("old B offset: 0x%x\n",tempADCSetting.u16BlueOffset));
            tempADCSetting.u16BlueOffset += (signed)((PbPr_AUTO_MIN_VALUE<<2) - (u8ResultB));
            AUTOMSG(printf("new B offset: 0x%x\n",tempADCSetting.u16BlueOffset));
            u8ATGStatus &= ~BIT(5);
        }

        AUTOMSG(printf("u8ATGStatus =0x%x\n", u8ATGStatus));

        if(u8ATGStatus == 0xFC)
            break;
    }

    if(u8ATGStatus == 0xFC)
    {
        // succssed, copy setting to user
        pstADCSetting->u16RedGain = tempADCSetting.u16RedGain;
        pstADCSetting->u16GreenGain = tempADCSetting.u16GreenGain;
        pstADCSetting->u16BlueGain = tempADCSetting.u16BlueGain;
        pstADCSetting->u16RedOffset = tempADCSetting.u16RedOffset;
        pstADCSetting->u16GreenOffset = tempADCSetting.u16GreenOffset;
        pstADCSetting->u16BlueOffset = tempADCSetting.u16BlueOffset;
    }
    else
    {
        MApi_XC_ADC_AdjustGainOffset(pstADCSetting);
        MApi_XC_ADC_AdjustGainOffset(pstADCSetting);
    }

    return (u8ATGStatus == 0xFC ? TRUE : FALSE);

}

#if 0
static MS_BOOL _MApi_XC_Auto_ColorYUV(MS_U8 u8VSyncTime, APIXC_AdcGainOffsetSetting *pstADCSetting)
{
    APIXC_AdcGainOffsetSetting tempADCSetting;
    MS_U8 u8ATGStatus, u8Tmp, u8FlowFlag;
    MS_U8 u8ResultR,u8ResultG,u8ResultB;

    // Using SMPTE 100% or 75% depends on the definition of devAuto.h
    tempADCSetting.u16RedGain = 0x30;
    tempADCSetting.u16GreenGain = 0x30;
    tempADCSetting.u16BlueGain = 0x30;
    tempADCSetting.u16RedOffset = 0x80;
    tempADCSetting.u16GreenOffset =0x80;
    tempADCSetting.u16BlueOffset = 0x80;

    MApi_XC_ADC_AutoSetting(DISABLE, TRUE, COMPONENT_AUTO_SW_MODE);

    u8ATGStatus = 0;
    u8FlowFlag = 0;

    // Tune offset
    for(u8Tmp=0; u8Tmp<0xFF; u8Tmp++)
    {
        //printf("u8Tmp=0x%bx\n", u8Tmp);
        MApi_XC_ADC_AdjustGainOffset(&tempADCSetting);

        MsOS_DelayTask(u8VSyncTime * 3); // wait stable

        u8ResultR = MApi_XC_GetAutoGainResult(AUTO_MIN_R, u8VSyncTime, MAIN_WINDOW);
        u8ResultG = MApi_XC_GetAutoGainResult(AUTO_MIN_G, u8VSyncTime, MAIN_WINDOW);
        u8ResultB = MApi_XC_GetAutoGainResult(AUTO_MIN_B, u8VSyncTime, MAIN_WINDOW);

        if ((u8ResultR >= PbPr_AUTO_MIN_VALUE) && (u8ResultR <= (PbPr_AUTO_MIN_VALUE+1)))    // Range (0x10~0xF0)
            u8ATGStatus |= BIT(7);
        else
            u8ATGStatus &= ~BIT(7);

        if ((u8ResultG >= (Y_AUTO_MIN_VALUE+1)) && (u8ResultG <= (Y_AUTO_MIN_VALUE+2))) // Range (0x00~0xDB)
            u8ATGStatus |= BIT(6);
        else
            u8ATGStatus &= ~BIT(6);

        if ((u8ResultB >= PbPr_AUTO_MIN_VALUE) && (u8ResultB <= (PbPr_AUTO_MIN_VALUE+1)))    // Range (0x10~0xF0)
            u8ATGStatus |= BIT(5);
        else
            u8ATGStatus &= ~BIT(5);

        // Red - Pr
        if ( !(u8FlowFlag&BIT(0)) )
        {
            if (_bit7_(u8ATGStatus))
            {
                // do nothing to RedOffset
                u8FlowFlag |= BIT(0);
            }
            else
            {
                tempADCSetting.u16RedOffset += (signed)(PbPr_AUTO_MIN_VALUE - u8ResultR);
            }
        }

        // Green - Y
        if ( !(u8FlowFlag&BIT(2)) )
        {
            if (_bit6_(u8ATGStatus))
            {
                // do nothing to GreenOffset
                u8FlowFlag |= BIT(2);
            }
            else
            {
                tempADCSetting.u16GreenOffset += (signed)(Y_AUTO_MIN_VALUE+1 - u8ResultG);
            }
        }

        // Blue - Pb
        if ( !(u8FlowFlag&BIT(4)) )
        {
            if (_bit5_(u8ATGStatus))
            {
                // do nothing to BlueOffset
                u8FlowFlag |= BIT(4);
            }
            else
            {
                tempADCSetting.u16BlueOffset += (signed)(PbPr_AUTO_MIN_VALUE - u8ResultB);
            }
        }

        //printf("ATG=0x%bx, FF=0x%bx, Min R=0x%bx,G=0x%bx,B=0x%bx\n", u8ATGStatus, u8FlowFlag, u8ResultR, u8ResultG, u8ResultG);

        // Get Pr active value (Max. value - Min. value)
        u8ResultR = MApi_XC_GetAutoGainResult(AUTO_MAX_R, u8VSyncTime, MAIN_WINDOW)-u8ResultR;
        // Get Y active value (Max. value - Min. value)
        u8ResultG = MApi_XC_GetAutoGainResult(AUTO_MAX_G, u8VSyncTime, MAIN_WINDOW)-u8ResultG;
        // Get Pb active value (Max. value - Min. value)
        u8ResultB = MApi_XC_GetAutoGainResult(AUTO_MAX_B, u8VSyncTime, MAIN_WINDOW)-u8ResultB;

        if((u8ResultR >= PbPr_AUTO_ACTIVE_RANGE) && (u8ResultR <= (PbPr_AUTO_ACTIVE_RANGE+1)))  // Range: 0x10~0xF0
        {
            u8ATGStatus |= BIT(4);
        }
        else
        {
            u8FlowFlag &= ~(BIT(1)+BIT(0));
            u8ATGStatus &= ~BIT(4);
        }

        if((u8ResultG >= Y_AUTO_ACTIVE_RANGE) && (u8ResultG <= (Y_AUTO_ACTIVE_RANGE+1)))  // Range: 0x00~0xDB
        {
            u8ATGStatus |= BIT(3);
        }
        else
        {
            u8FlowFlag &= ~(BIT(3)+BIT(2));
            u8ATGStatus &= ~BIT(3);
        }

        if((u8ResultB >= PbPr_AUTO_ACTIVE_RANGE) && (u8ResultB <= (PbPr_AUTO_ACTIVE_RANGE+1)))  // Range: 0x10~0xF0
        {
            u8ATGStatus |= BIT(2);
        }
        else
        {
            u8FlowFlag &= ~(BIT(5)+BIT(4));
            u8ATGStatus &= ~BIT(2);
        }

        // Red - Pr
        if ( !(u8FlowFlag&BIT(1)) )
        {
            if (_bit4_(u8ATGStatus))
            {
                u8FlowFlag |= BIT(1);
            }
            else
            {
                tempADCSetting.u16RedGain += (signed)(PbPr_AUTO_ACTIVE_RANGE - u8ResultR);
                u8FlowFlag &= ~BIT(1);
            }
        }

        // Green - Y
        if ( !(u8FlowFlag&BIT(3)) )
        {
            if (_bit3_(u8ATGStatus))
            {
                u8FlowFlag |= BIT(3);
            }
            else
            {
                tempADCSetting.u16GreenGain += (signed)(Y_AUTO_ACTIVE_RANGE - u8ResultG);
                u8FlowFlag &= ~BIT(3);
            }
        }

        // Blue - Pb
        if ( !(u8FlowFlag&BIT(5)) )
        {
            if (_bit2_(u8ATGStatus))
            {
                u8FlowFlag |= BIT(5);
            }
            else
            {
                tempADCSetting.u16BlueGain += (signed)(PbPr_AUTO_ACTIVE_RANGE - u8ResultB);
                u8FlowFlag &= ~BIT(5);
            }
        }

        //printf("ATG=0x%bx, FF=0x%bx, Max R=0x%bx,G=0x%bx,B=0x%bx\n", u8ATGStatus, u8FlowFlag, u8ResultR, u8ResultG, u8ResultG);

        if(u8FlowFlag == 0x3F)
            break;
    }

    MsOS_DelayTask(u8VSyncTime * 2);



    //printf("FF=0x%bx, Gain R=0x%bx,G=0x%bx,B=0x%bx\n", u8FlowFlag,
    //    ~tempADCSetting.u16RedGain, ~tempADCSetting.u16GreenGain, ~tempADCSetting.u16BlueGain);
    //printf("FF=0x%bx, Offset R=0x%bx,G=0x%bx,B=0x%bx\n", u8FlowFlag,
    //    ~tempADCSetting.u16RedOffset, ~tempADCSetting.u16GreenOffset, ~tempADCSetting.u16BlueOffset);


    if(u8FlowFlag == 0x3F)
    {
        // succssed, copy setting to user
        pstADCSetting->u16RedGain = tempADCSetting.u16RedGain;
        pstADCSetting->u16GreenGain = tempADCSetting.u16GreenGain;
        pstADCSetting->u16BlueGain = tempADCSetting.u16BlueGain;
        pstADCSetting->u16RedOffset = tempADCSetting.u16RedOffset;
        pstADCSetting->u16GreenOffset = tempADCSetting.u16GreenOffset;
        pstADCSetting->u16BlueOffset = tempADCSetting.u16BlueOffset;
    }
    else
    {
        MApi_XC_ADC_AdjustGainOffset(pstADCSetting);
        MApi_XC_ADC_AdjustGainOffset(pstADCSetting);
    }

    return (u8FlowFlag == 0x3F ? TRUE : FALSE);
}
#endif


#else

static MS_BOOL _MApi_XC_Auto_ColorYUV(MS_U8 u8VSyncTime, APIXC_AdcGainOffsetSetting *pstADCSetting)
{
    APIXC_AdcGainOffsetSetting tempADCSetting = {0,0,0,0,0,0};//trivial initial value is assigned;
    MS_U8 u8ATGStatus;
    MS_U16 u16GainDelta, i;
    MS_U8 u8FlowFlag = 0; // Add the initial value

    // Initial
    MApi_XC_ADC_InitExternalCalibration(INPUT_SOURCE_YPBPR, &tempADCSetting);

    // Initial Delta
    u16GainDelta = MApi_XC_ADC_GetCenterGain();

    MApi_XC_ADC_AutoSetting(ENABLE, TRUE, COMPONENT_AUTO_SW_MODE);

    while(1)
    {
        u16GainDelta >>= 1;
        if(u16GainDelta == 0x00)
            break;

        // Update gain.
        MApi_XC_ADC_AdjustGainOffset(&tempADCSetting);

        ///MApi_XC_ADC_AutoSetting(ENABLE, TRUE, COMPONENT_AUTO_SW_MODE);
        AUTOMSG(printf("RGain %x \r\n", tempADCSetting.u16RedGain ));
        AUTOMSG(printf("GGain %x \r\n", tempADCSetting.u16GreenGain ));
        AUTOMSG(printf("BGain %x \r\n", tempADCSetting.u16BlueGain ));
        MsOS_DelayTask(u8VSyncTime * 3); // wait stable
        _MApi_XC_Auto_WaitStatusReady(E_WAITING_AUTO_GAIN_READY, MAIN_WINDOW);
        u8ATGStatus = MApi_XC_GetAutoGainMaxValueStatus(MAIN_WINDOW); // get auto gain status

        // Red - Pr
        if ( u8ATGStatus & BIT(2) )
        {
            tempADCSetting.u16RedGain -= u16GainDelta;
            u8FlowFlag |= BIT(0);
        }
        else
        {
            tempADCSetting.u16RedGain += u16GainDelta;
            u8FlowFlag |= BIT(1);
        }

        // Green - Y
        if ( u8ATGStatus & BIT(1) )
        {
            tempADCSetting.u16GreenGain -= u16GainDelta;
            u8FlowFlag |= BIT(2);
        }
        else
        {
            tempADCSetting.u16GreenGain += u16GainDelta;
            u8FlowFlag |= BIT(3);
        }

        // Blue - Pb
        if (u8ATGStatus & BIT(0) )
        {
            tempADCSetting.u16BlueGain -= u16GainDelta;
            u8FlowFlag |= BIT(4);
        }
        else
        {
            tempADCSetting.u16BlueGain += u16GainDelta;
            u8FlowFlag |= BIT(5);
        }
        AUTOMSG(printf("u8FlowFlag %x \r\n",u8FlowFlag));
    }

    u8FlowFlag = 0;

    //Decrease the gain setting and fine tune it
    if(tempADCSetting.u16RedGain > 10)
    {
        tempADCSetting.u16RedGain -= 10;
    }
    else
    {
        tempADCSetting.u16RedGain = 0;
    }

    if(tempADCSetting.u16GreenGain > 10)
    {
        tempADCSetting.u16GreenGain -= 10;
    }
    else
    {
        tempADCSetting.u16GreenGain = 0;
    }

    if(tempADCSetting.u16BlueGain > 10)
    {
        tempADCSetting.u16BlueGain -= 10;
    }
    else
    {
        tempADCSetting.u16BlueGain = 0;
    }

    u16GainDelta =  MApi_XC_ADC_GetMaximalGainValue();
    AUTOMSG(printf(" fine tune \r\n"));
    for(i=0; i<u16GainDelta; i++)
    {
        // Update gain.
        MApi_XC_ADC_AdjustGainOffset(&tempADCSetting);

        //MApi_XC_ADC_AutoSetting(ENABLE, TRUE, COMPONENT_AUTO_SW_MODE);
        AUTOMSG(printf("RGain %x \r\n", tempADCSetting.u16RedGain ));
        AUTOMSG(printf("GGain %x \r\n", tempADCSetting.u16GreenGain ));
        AUTOMSG(printf("BGain %x \r\n", tempADCSetting.u16BlueGain ));
        MsOS_DelayTask(u8VSyncTime * 3); // wait stable
        _MApi_XC_Auto_WaitStatusReady(E_WAITING_AUTO_GAIN_READY, MAIN_WINDOW);

        u8ATGStatus = MApi_XC_GetAutoGainMaxValueStatus(MAIN_WINDOW); // get auto gain status

        // Red - Pr
        if ((u8FlowFlag&(BIT(0)|BIT(1))) != (BIT(0)|BIT(1)))
        {
            if(u8ATGStatus & BIT(2))
            {
                u8FlowFlag |= BIT(0);
                if ((u8FlowFlag&BIT(1)) != BIT(1))
                {
                    tempADCSetting.u16RedGain -= AUTO_YUV_GAIN_STEP;
                    AUTOMSG(printf("R- \r\n"));
                }
            }
            else
            {
                tempADCSetting.u16RedGain += AUTO_YUV_GAIN_STEP;
                u8FlowFlag |= BIT(1);
                AUTOMSG(printf("R+ \r\n"));
            }
        }

        // Green - Y
        if((u8FlowFlag&(BIT(2)|BIT(3))) != (BIT(2)|BIT(3)))
        {
            if(u8ATGStatus & BIT(1))
            {
                u8FlowFlag |= BIT(2);
                if ((u8FlowFlag&BIT(3)) != BIT(3))
                {
                    tempADCSetting.u16GreenGain -= AUTO_YUV_GAIN_STEP;
                    AUTOMSG(printf("G+ \r\n"));
                }
            }
            else
            {
                tempADCSetting.u16GreenGain += AUTO_YUV_GAIN_STEP;
                u8FlowFlag |= BIT(3);
                AUTOMSG(printf("G- \r\n"));
            }
        }

        // Blue - Pb
        if((u8FlowFlag&(BIT(4)|BIT(5))) != (BIT(4)|BIT(5)))
        {
            if(u8ATGStatus & BIT(0) )
            {
                u8FlowFlag |= BIT(4);
                if ((u8FlowFlag&BIT(5)) != BIT(5))
                {
                    tempADCSetting.u16BlueGain -= AUTO_YUV_GAIN_STEP;
                    AUTOMSG(printf("B+ \r\n"));
                }
            }
            else
            {
                tempADCSetting.u16BlueGain += AUTO_YUV_GAIN_STEP;
                u8FlowFlag |= BIT(5);
                AUTOMSG(printf("B- \r\n"));
            }
        }

        if (u8FlowFlag == 0x3F || i > AUTO_YUV_MAX_CNT)
        {
            break;
        }
    }

    // adjust gain after auto-tune
    if (u8FlowFlag == 0x3f)
    {
        // successed
        AUTOMSG(printf("success \r\n"));
        pstADCSetting->u16RedGain = tempADCSetting.u16RedGain;
        pstADCSetting->u16GreenGain = tempADCSetting.u16GreenGain;
        pstADCSetting->u16BlueGain = tempADCSetting.u16BlueGain;
        pstADCSetting->u16RedOffset = tempADCSetting.u16RedOffset;
        pstADCSetting->u16GreenOffset = tempADCSetting.u16GreenOffset;
        pstADCSetting->u16BlueOffset = tempADCSetting.u16BlueOffset;
        return TRUE;
    }
    else
    {
		AUTOMSG(printf("failed... flag: 0x%X, delta: %d, step: %d \r\n", u8FlowFlag, u16GainDelta, i));

        // Restore original setting.
        MApi_XC_ADC_AdjustGainOffset(pstADCSetting);
        return FALSE;
    }
}


#endif

static MS_BOOL _MApi_XC_Auto_External_AutoCablication( XC_Auto_TuneType enAutoTuneType, APIXC_AdcGainOffsetSetting *pstADCSetting,SCALER_WIN eWindow)
{
    MS_BOOL bAutoResult = TRUE;
    MS_U8 u8VSyncTime; // VSync time

    MApi_XC_Auto_SetCalibrartinWindow(ENABLE,eWindow);
    MApi_XC_AutoGainEnable(ENABLE,eWindow);

    u8VSyncTime = MApi_XC_PCMonitor_GetVSyncTime(eWindow); // get VSync time

    // auto RGB offset

    if(enAutoTuneType & E_XC_AUTO_TUNE_RGB_OFFSET)
    {
        if (!MDrv_XC_ADC_use_hardware_auto_offset(INPUT_SOURCE_VGA))
        {
            // Run software tune offset
            if(_MApi_XC_Auto_TuneOffset(u8VSyncTime, pstADCSetting) == FALSE)
            {
                AUTOMSG(printf("AutoOffset failed\n"));
                //enAutoTuneType = E_XC_AUTO_TUNE_NULL;
                bAutoResult = FALSE;
            }
        }
        else
        {
            // Using HW offset, so use default offset value.
            pstADCSetting->u16BlueOffset = pstADCSetting->u16RedOffset = pstADCSetting->u16GreenOffset = 0;
        }
    } // auto-tune ADC offset


    // auto RGB gain
    if(enAutoTuneType & E_XC_AUTO_TUNE_RGB_GAIN)
    {
        if(_MApi_XC_Auto_TuneRGBGain(u8VSyncTime,pstADCSetting,eWindow) == FALSE)
        {
            AUTOMSG(printf("AutoGain failed\n"));
            //enAutoTuneType = E_XC_AUTO_TUNE_NULL;
            bAutoResult = FALSE;
        }
    } // auto-tune ADC gain


    // auto Scart RGB gain
    if(enAutoTuneType & E_XC_AUTO_TUNE_SCART_RGB_GAIN)
    {
        if(_MApi_XC_Auto_TuneScartRGBGain(u8VSyncTime,pstADCSetting,eWindow) == FALSE)
        {
            AUTOMSG(printf("AutoGain failed\n"));
            //enAutoTuneType = E_XC_AUTO_TUNE_NULL;
            bAutoResult = FALSE;
        }
    } // auto-tune ADC gain

    if(enAutoTuneType & E_XC_AUTO_TUNE_YUV_COLOR)
    {
        //if(_MApi_XC_Auto_TuneGain(E_XC_EXTERNAL_CALIBRATION, enAutoTuneType,pstADCSetting,eWindow) == FALSE)
        if (_MApi_XC_Auto_ColorYUV(u8VSyncTime, pstADCSetting) == FALSE)
        {
            //enAutoTuneType = E_XC_AUTO_TUNE_NULL;
            bAutoResult = FALSE;
            AUTOMSG(printf("AutoColorYUV failed\n"));
        }
    } // auto-tune ADC gain

    // disable auto gain function
    MApi_XC_AutoGainEnable(DISABLE,eWindow);
    MApi_XC_Auto_SetCalibrartinWindow(DISABLE,eWindow);

    AUTOMSG(printf("Offset R=0x%x,G=0x%x,B=0x%x\n",
                   pstADCSetting->u16RedOffset,
                   pstADCSetting->u16GreenOffset,
                   pstADCSetting->u16BlueOffset));
    AUTOMSG(printf("Gain R=0x%x,G=0x%x,B=0x%x\n",
                   pstADCSetting->u16RedGain,
                   pstADCSetting->u16GreenGain,
                   pstADCSetting->u16BlueGain));
    return bAutoResult;
}

//
// The reason to wait stable here is some pattern gen such as MSPG-1025D,
// when change timing of RGB, the pattern gen will send out H/V sync first,
// then RGB signal later. (This is not measured by the scope. If pause the program,
// change pattern gen timing, the video come out after around 5 seconds.
// If used MSPG-925FS, change pattern gen timing, the video come out
// within 1 second.) So, we've to wait auto-position H_Start, H_End avaliable and then proceed
// RGB auto function, otherwise, the auto function will be failed because of using the wrong auto-position
// H_Start, H_End.
//
// When waiting the H_Start, H_End to be stable, there are some cases:
// (1) H_Start is 0x7ff, H_Size is 0x00: -> un-stable
// (2) H_Start != 0x7ff and H_Size != 0x00, but H_Start == H_Size: -> un-stable
// (3) H_Start != 0x7ff and H_Size != 0x00, and H_Start != H_Size, but H_Start is only about H_Size -1 or -2: -> un-stable
static MS_BOOL _MApi_XC_Auto_waitstable(MS_U8 u8VSyncTime, MS_U16 u16WaitCnt, SCALER_WIN eWindow)
{
    MS_U16 i;
    MS_U16 u16HStart, u16HEnd;
    MS_WINDOW_TYPE sWin;

    // use a short delay for stable

    for(i=0; i<u16WaitCnt; i++)
    {
        MsOS_DelayTask(u8VSyncTime);

        MApi_XC_GetDEWindow(&sWin, eWindow);

        u16HStart = sWin.x;
        u16HEnd = sWin.x + sWin.width;

        if(((u16HStart != 0x7FF) && (u16HEnd != 0x000)) ||
            (u16HStart < u16HEnd))
        {
            if(abs(u16HStart - u16HEnd) > 10)
            {
                break;
            }
        }
    }

    return (i == u16WaitCnt) ? FALSE : TRUE;
}

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

MS_BOOL MApi_XC_Auto_GainOffset(XC_Auto_CalibrationType type, XC_Auto_TuneType enAutoTuneType,  APIXC_AdcGainOffsetSetting *pstADCSetting, SCALER_WIN eWindow)
{
    if(type == E_XC_INTERNAL_CALIBRATION)
    {
        return( _MApi_XC_Auto_Internal_AutoCablication( enAutoTuneType ,pstADCSetting,eWindow)) ;
    }
    else
    {
        return( _MApi_XC_Auto_External_AutoCablication( enAutoTuneType ,pstADCSetting,eWindow)) ;
    }
}

//*************************************************************************
//Function name: MApi_XC_Auto_Geometry
//Passing parameter:
//  enAutoTuneType: Auto function select
//  ActiveInfo: Current PC mode setting
//  StandardInfo: Standard mode. (This is singal information of stardard)
//Return parameter:
//  MS_BOOL: Success status. (If faile, return FALSE.)
//Description: Auto-tune for PC mode
//*************************************************************************
MS_BOOL MApi_XC_Auto_Geometry(XC_Auto_TuneType enAutoTuneType, XC_Auto_Signal_Info *ActiveInfo, XC_Auto_Signal_Info *StandardInfo , SCALER_WIN eWindow)
{
    XC_Auto_Signal_Info_Ex ActiveInfoEx, StandardInfoEx;
    MS_BOOL bResult;

    ActiveInfoEx.u16HorizontalStart = ActiveInfo->u16HorizontalStart;
    ActiveInfoEx.u16VerticalStart = ActiveInfo->u16VerticalStart;
    ActiveInfoEx.u16HorizontalTotal = ActiveInfo->u16HorizontalTotal;
    ActiveInfoEx.u16HResolution = ActiveInfo->u16HResolution;
    ActiveInfoEx.u16Phase = (MS_U16)ActiveInfo->u8Phase;
    ActiveInfoEx.u16Version = 0;

    StandardInfoEx.u16HorizontalStart = StandardInfo->u16HorizontalStart;
    StandardInfoEx.u16VerticalStart = StandardInfo->u16VerticalStart;
    StandardInfoEx.u16HorizontalTotal = StandardInfo->u16HorizontalTotal;
    StandardInfoEx.u16HResolution = StandardInfo->u16HResolution;
    StandardInfoEx.u16Phase = (MS_U16)StandardInfo->u8Phase;
    StandardInfoEx.u16Version= 0;

    bResult = MApi_XC_Auto_Geometry_Ex(enAutoTuneType, &ActiveInfoEx, &StandardInfoEx , eWindow);

    ActiveInfo->u16HorizontalStart = ActiveInfoEx.u16HorizontalStart;
    ActiveInfo->u16VerticalStart = ActiveInfoEx.u16VerticalStart;
    ActiveInfo->u16HorizontalTotal = ActiveInfoEx.u16HorizontalTotal;
    ActiveInfo->u16HResolution = ActiveInfoEx.u16HResolution;
    ActiveInfo->u8Phase = (MS_U8)ActiveInfoEx.u16Phase;

    return bResult;
}

//*************************************************************************
//Function name: MApi_XC_Auto_Geometry_Ex
//Passing parameter:
//  enAutoTuneType: Auto function select
//  ActiveInfo: Current PC mode setting
//  StandardInfo: Standard mode. (This is singal information of stardard)
//Return parameter:
//  MS_BOOL: Success status. (If faile, return FALSE.)
//Description: Auto-tune for PC mode
//*************************************************************************
MS_BOOL MApi_XC_Auto_Geometry_Ex(XC_Auto_TuneType enAutoTuneType, XC_Auto_Signal_Info_Ex *ActiveInfo, XC_Auto_Signal_Info_Ex *StandardInfo , SCALER_WIN eWindow)
{
    MS_U8 u8VSyncTime; // VSync time
    MS_BOOL bAutoResult = TRUE;
    MS_U16 u16OldClk = MApi_XC_ADC_GetPcClock();

    // Reset flag. bHaltAutoGeometry should be update during auto geometry

    bHaltAutoGeometry = FALSE;

    XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"\n======== Auto Tune Type : 0x%x, Htt %u->%u ======== \n",enAutoTuneType, u16OldClk, ActiveInfo->u16HorizontalTotal);
    if( u16OldClk != ActiveInfo->u16HorizontalTotal )
    {
        MS_U16  u16OrgDefaultHtotal, u16OrgDefaultPhase;
        MS_BOOL bOrgHDuplicate;
        //Save original timing info first
        bOrgHDuplicate = gSrcInfo[eWindow].bHDuplicate;
        u16OrgDefaultHtotal = gSrcInfo[eWindow].u16DefaultHtotal;
        u16OrgDefaultPhase  = (MS_U16)gSrcInfo[eWindow].u8DefaultPhase;

        //Use new timing info to tune
        gSrcInfo[eWindow].bHDuplicate = FALSE;
        gSrcInfo[eWindow].u8DefaultPhase = (MS_U8)ActiveInfo->u16Phase;
        gSrcInfo[eWindow].u16DefaultHtotal = ActiveInfo->u16HorizontalTotal;
        MDrv_XC_ADC_SetMode(gSrcInfo[eWindow].enInputSourceType, &gSrcInfo[eWindow], eWindow);

        //Restore original timing info
        gSrcInfo[eWindow].bHDuplicate = bOrgHDuplicate;
        gSrcInfo[eWindow].u16DefaultHtotal = u16OrgDefaultHtotal ;
        gSrcInfo[eWindow].u8DefaultPhase = (MS_U8)u16OrgDefaultPhase;
    }
    MApi_XC_AutoGainEnable(ENABLE, eWindow);

    u8VSyncTime = MApi_XC_PCMonitor_GetVSyncTime(eWindow); // get VSync time

    // Should wait stable to proceed auto
    _MApi_XC_Auto_waitstable(u8VSyncTime, ActiveInfo->u16HorizontalTotal*4/10, eWindow); // use u16HorizontalTotal as 2nd para is just to estimate

    #if (!FAST_AUTO_TUNE)
    // auto position valid data theshold
    if(enAutoTuneType & E_XC_AUTO_TUNE_VALID_DATA)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"Start to tune Valid data.\n");

        if(_MApi_XC_Auto_SetValidData(u8VSyncTime, eWindow) == FALSE)
        {
            XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"AutoValidData failed\n");

            bAutoResult = FALSE;
        }
        else // fixed valid data
        {
            MDrv_XC_SetValidDataThreshold(0x04, eWindow);
        }
    }
    #else

    XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"Force set valid data.\n");

    MDrv_XC_SetValidDataThreshold(_u8ThresholdValue, eWindow);

    #endif

    ////////////////Stop Geometry checker/////////////////////////////
    if (bHaltAutoGeometry)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"Halt auto geometry\n");
        bHaltAutoGeometry = FALSE;
        return FALSE;
    }

    // Auto Tune H total
    if(enAutoTuneType & E_XC_AUTO_TUNE_FREQ)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"Start to tune HTotal\n");

        if(_MApi_XC_Auto_TuneHTotal(u8VSyncTime, ActiveInfo, StandardInfo, eWindow) == FALSE)
        {
            XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"Tune HTotal fail!!\n");

            bAutoResult = FALSE;
        }
        else
        {
            XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"Tune HTotal success!!\n");
        }
    }

    ////////////////Stop Geometry checker/////////////////////////////
    if (bHaltAutoGeometry)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"Halt auto geometry\n");
        bHaltAutoGeometry = FALSE;
        return FALSE;
    }


    // auto phase
    if(enAutoTuneType & E_XC_AUTO_TUNE_PHASE)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"Start to tune phase\n");

        if(_MApi_XC_Auto_TunePhase(u8VSyncTime, ActiveInfo, eWindow) == FALSE)
        {
            XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"Tune phase fail!!\n");

            bAutoResult = FALSE;
        }
        else
        {
            XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"Tune phase success!!\n");
        }
    }

    ////////////////Stop Geometry checker/////////////////////////////
    if (bHaltAutoGeometry)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"Halt auto geometry\n");
        bHaltAutoGeometry = FALSE;
        return FALSE;
    }

    // auto position
    if(enAutoTuneType & E_XC_AUTO_TUNE_POSITION)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"Start to tune position\n");

        if(_MApi_XC_Auto_TunePosition(u8VSyncTime, ActiveInfo, StandardInfo, eWindow) == FALSE)
        {
            XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"Tune position fail!!\n");
            //enAutoTuneType = E_XC_AUTO_TUNE_NULL;
            bAutoResult = FALSE;
        }
        else
        {
            XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"Tune position success!!\n");
        }
    }

    XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"Result: %d \n",bAutoResult);

    return    bAutoResult;
}

// Set flag as true to halt auto geometry
MS_BOOL MApi_XC_Auto_StopAutoGeometry(void)
{
    XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"Stop auto geometry.\n");
    bHaltAutoGeometry = TRUE;
    return TRUE;
}


/******************************************************************************/
/// -This function will recalibration ADC offset, must be called when mode change.
/// @param bEnable \b IN:
/// - 0: Disable offset recalibration
/// - 1: Enable offset recalibration
/// @param bIsYPbPrFlag \b IN: to tell if YPbPr in
/// - 0: Vga mode
/// - 1: YPbPr mode
/******************************************************************************/
void MApi_XC_Auto_AutoOffset(MS_BOOL bEnable, MS_BOOL bIsYPbPrFlag)
{
    MApi_XC_ADC_AutoSetting(bEnable, bIsYPbPrFlag, COMPONENT_AUTO_SW_MODE);
}

MS_BOOL MApi_XC_Auto_DetectWidth(XC_Auto_Signal_Info_Ex *pSignalInfo , SCALER_WIN eWindow)
{
    MS_U8 u8VSyncTime ; // VSync time
    MS_BOOL bAutoResult = TRUE;
    MS_U16 u16ActualWidth = 0xFFFF;
    MS_U16 u16OldClk = MApi_XC_ADC_GetPcClock();

    XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"\n======== Auto DetectWidth : eWindow=%u, Htt %u->%u ======== \n",eWindow, u16OldClk, pSignalInfo->u16HorizontalTotal);
    if( u16OldClk != pSignalInfo->u16HorizontalTotal )
    {
        MS_U16  u16OrgDefaultHtotal, u16OrgDefaultPhase;
        MS_BOOL bOrgHDuplicate;
        //Save original timing info first
        bOrgHDuplicate = gSrcInfo[eWindow].bHDuplicate;
        u16OrgDefaultHtotal = gSrcInfo[eWindow].u16DefaultHtotal;
        u16OrgDefaultPhase  = (MS_U16)gSrcInfo[eWindow].u8DefaultPhase;

        //Use new timing info to tune
        gSrcInfo[eWindow].bHDuplicate = FALSE;
        gSrcInfo[eWindow].u8DefaultPhase = (MS_U8)pSignalInfo->u16Phase;
        gSrcInfo[eWindow].u16DefaultHtotal = pSignalInfo->u16HorizontalTotal;
        MDrv_XC_ADC_SetMode(gSrcInfo[eWindow].enInputSourceType, &gSrcInfo[eWindow], eWindow);

        //Restore original timing info
        gSrcInfo[eWindow].bHDuplicate = bOrgHDuplicate;
        gSrcInfo[eWindow].u16DefaultHtotal = u16OrgDefaultHtotal ;
        gSrcInfo[eWindow].u8DefaultPhase = (MS_U8)u16OrgDefaultPhase;
    }
    MApi_XC_AutoGainEnable(ENABLE, eWindow);
    u8VSyncTime = MApi_XC_PCMonitor_GetVSyncTime(eWindow); // get VSync time
    // Should wait stable to proceed auto
    _MApi_XC_Auto_waitstable(u8VSyncTime, pSignalInfo->u16HorizontalTotal*4/10, eWindow); // use u16HorizontalTotal as 2nd para is just to estimate
#if (!FAST_AUTO_TUNE)
    // auto position valid data theshold
    XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"Auto DetectWidth: Start to tune Valid data.\n");
    if(_MApi_XC_Auto_SetValidData(u8VSyncTime, eWindow) == FALSE)
    {
        // auto position valid data theshold
        XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"Auto DetectWidth: Start to tune Valid data.\n");
        bAutoResult = FALSE;
    }
    else // fixed valid data
    {
        MDrv_XC_SetValidDataThreshold(0x04, eWindow);
        u16ActualWidth = _MApi_XC_Auto_GetActualWidth(u8VSyncTime, eWindow);
    }
#else
    XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"Auto DetectWidth: Force set valid data.\n");
    MDrv_XC_SetValidDataThreshold(_u8ThresholdValue, eWindow);
    u16ActualWidth = _MApi_XC_Auto_GetActualWidth(u8VSyncTime, eWindow);
#endif
    pSignalInfo->u16HResolution = u16ActualWidth; //Set to invalid resolution
    XC_LOG_TRACE(XC_DBGLEVEL_AUTOGEOMETRY,"Auto DetectWidth: bResult=%u, DetectWidth=%u.\n", bAutoResult, u16ActualWidth);
    return bAutoResult;
}

#undef _APIXC_AUTO_C_
