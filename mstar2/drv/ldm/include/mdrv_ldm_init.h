//<MStar Software>
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2010 - 2012 Mstar Semiconductor, Inc.
// This program is free software.
// You can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation;
// either version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with this program;
// if not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
/// file    mdrv_ldm_init.h
/// @brief  local dimming Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MDRV_LD_INIT_H_
#define _MDRV_LD_INIT_H_


/// Define PNL local dimming return type
typedef enum
{
    E_LD_STATUS_SUCCESS = 0,
    E_LD_STATUS_FAIL = 1,
    E_LD_STATUS_NOT_SUPPORTED,
    E_LD_STATUS_PARAMETER_ERROR,
    E_LD_OBTAIN_MUTEX_FAIL,            ///< obtain mutex timeout when calling this function
    E_LD_OBTAIN_MMIO_FAIL,            ///< obtain mutex timeout when calling this function
} EN_LD_RETURN;

typedef enum
{
    E_LD_SPI_DATA_16_BIT_LH = 0x00,//don't convert
    E_LD_SPI_DATA_8_BIT = 0x01,   //16-bit --> 8-bit: AB CD --> CD
    E_LD_SPI_DATA_12_BIT = 0x02,  //16-bit --> 12-bit  etc 4 bytes: aa bc de fg --> aa bd ef
    E_LD_SPI_DATA_12_BIT_HL = 0x03,
    E_LD_SPI_DATA_12_BIT_LH = 0x04,
    E_LD_SPI_DATA_16_BIT_HL = 0x05,  //16-bit --> 16-bit  AB CD --> CD AB
    E_LD_SPI_DATA_CUS_BIT = 0x06, //customer format
    E_LD_SPI_DATA_MAX
}EN_LD_SPI_DATA_BITS;


typedef struct
{
    MS_U16 u16PanelWidth;
    MS_U16 u16PanelHeight;
    MS_U16 u16PackLength;
    MS_PHY u32BaseAddr;
}ST_DRV_LD_INFO;


typedef struct
{
    EN_LD_TYPE enLEDType; // 0/1/2/3

    // PANEL
    MS_U8 u8LDFWidth, u8LDFHeight; // Statistic width & height  MAX=32*32
    MS_U8 u8LEDWidth, u8LEDHeight; // LED width & height  MAX=W*H=64 if Edge2d enable, MAX=128*72 otherwise
    MS_U8 u8LSFWidth, u8LSFHeight; // LSF width & height  MAX=128*72
    MS_BOOL bCompensationEn; // Compensation enable
    const MS_U8* pu8CompTable; // Compensation table  SIZE=256x2
    const MS_U8* pu8CompTable1; // Compensation table  SIZE=256x2
    const MS_U8* pu8CompTable2; // Compensation table  SIZE=256x2
    MS_BOOL bEdge2DEn; // Edge2D enable
    const MS_U8* pu8Edge2DTable; // Edge2D table  SIZE=(u8LEDWidth*u8LEDHeight)*(u8LDFWidth*u8LDFHeight)

    MS_BOOL bLSFEncodingEn;
    MS_BOOL bCompLowGainEn;
    MS_BOOL bSaturationEn;
    MS_U8 u8SaturationA;
    MS_U16 u16SaturationC;
    MS_U8 u8SaturationCShift;
    MS_BOOL bSFLinearEn;

    // PQ HW
    MS_U32 u32LsfInitialPhase_H, u32LsfInitialPhase_V;
    MS_U8 u8DCMaxAlpha; // statistics : higher -> prefer max, lower -> prefer average
    MS_U8 u8CompenGain; // pixel compensation tuner : higher -> prefer compensated pixel, lower -> prefer original pixel

    // PQ SW
    MS_U8 u8GDStrength; // global dimming strength
    MS_U8 u8LDStrength; // local dimming strength
    MS_U8 u8TFStrengthUp; // temporal filter strength up
    MS_U8 u8TFStrengthDn; // temporal filter strength down
    MS_U8 u8TFLowThreshold; // temporal filter low threshold
    MS_U8 u8TFHightThreshold; // temporal filter high threshold
    MS_U8 u8SFStrength; // spatial filter strength
    MS_U8 u8SFStrength2; // spatial filter strength 2
    MS_U8 u8SFStrength3; // spatial filter strength 3
    MS_U8 u8SFStrength4; // spatial filter strength 4
    MS_U8 u8SFStrength5; // spatial filter strength 5
    MS_U8 u8InGamma; // gamma at SW begin
    MS_U8 u8OutGamma; // gamma at SW end
    MS_U8 u8BLMinValue; // min clamp value
    MS_U8 u8NRStrength; // de-noise strength
}ST_DRV_LD_PQ_INFO;

typedef struct
{
    MS_U32 u32MarqueeDelay;  //Marqueue demo Delay,unit ms
    MS_BOOL bDbgEn;          // switcher for u8DebugLevel/bLDEn
    MS_U8 u8DebugLevel;     // log level
    MS_BOOL bLDEn;          // local dimming enable
    MS_U8 u8SPIBits;          //spi data 16-bit, 12-bit , 8-bit
    MS_U8 u8ClkHz;           //panel frequency  120hz;60hz
    MS_U8 u8MirrorPanel;      //for HVmirror panel just test for Public version 0-normal panel 1-mirror panel
}ST_DRV_LD_MISC_INFO;


#define LOCAL_DIMMING_PWM_CH E_PWM_CH2


#define LOCAL_DIMMING_PWM_VSYNC_Period      (0x4FFF)
#define LOCAL_DIMMING_PWM_VSYNC_DutyCycle   (0x22)
#define LOCAL_DIMMING_PWM_VSYNC_Div 0x0b


#endif
