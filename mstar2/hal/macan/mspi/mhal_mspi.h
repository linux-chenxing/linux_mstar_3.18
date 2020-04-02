//<MStar Software>
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2010 - 2012 MStar Semiconductor, Inc.
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
#ifndef _MHAL_MSPI_H_
#define _MHAL_MSPI_H_

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define MSPI_READ_INDEX                0x0
#define MSPI_WRITE_INDEX               0x1
/* check if chip support MSPI*/
//#define HAL_MSPI_HW_Support()          TRUE
#define DEBUG_MSPI(debug_level, x)     do { if (_u8MSPIDbgLevel >= (debug_level)) (x); } while(0)

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------



typedef enum {
    E_MSPI_MODE0, //CPOL = 0,CPHA =0
    E_MSPI_MODE1, //CPOL = 0,CPHA =1
    E_MSPI_MODE2, //CPOL = 1,CPHA =0
    E_MSPI_MODE3, //CPOL = 1,CPHA =1
    E_MSPI_MODE_MAX,
} MSPI_Mode_Config_e;

typedef enum
{
    E_MSPI1,
    E_MSPI2,
    E_MSPI_MAX,
}MSPI_CH;

typedef enum
{
    E_MSPI_ChipSelect_0,
    E_MSPI_ChipSelect_1,
    E_MSPI_ChipSelect_2,
    E_MSPI_ChipSelect_3,
    E_MSPI_ChipSelect_4,
    E_MSPI_ChipSelect_5,
    E_MSPI_ChipSelect_6,
    E_MSPI_ChipSelect_7,
    E_MSPI_ChipSelect_MAX
}MSPI_ChipSelect_e;

typedef enum
{
    E_MSPI_BIT_MSB_FIRST,
    E_MSPI_BIT_LSB_FIRST,
}MSPI_BitSeq_e;

typedef enum _HAL_CLK_Config
{
    E_MSPI_POL,
    E_MSPI_PHA,
    E_MSPI_CLK
}eCLK_config;

typedef enum _HAL_DC_Config
{
    E_MSPI_TRSTART,
    E_MSPI_TREND,
    E_MSPI_TB,
    E_MSPI_TRW
}eDC_config;

typedef struct
{
    MSPI_CH eCurrentCH;;
    MS_U32 VirtMspBaseAddr;
    MS_U32 VirtClkBaseAddr;
} MSPI_BaseAddr_st;

typedef struct
{
    MS_U8 u8TrStart;      //time from trigger to first SPI clock
    MS_U8 u8TrEnd;        //time from last SPI clock to transferred done
    MS_U8 u8TB;           //time between byte to byte transfer
    MS_U8 u8TRW;          //time between last write and first read
} MSPI_DCConfig;

typedef struct
{
    MS_U8 u8WBitConfig[8];      //bits will be transferred in write buffer
    MS_U8 u8RBitConfig[8];      //bits Will be transferred in read buffer
} MSPI_FrameConfig;

typedef struct
{
    MS_U32 u32Clock;
    MS_U8 U8Clock;
    MS_BOOL BClkPolarity;
    MS_BOOL BClkPhase;
    MS_U32 u32MAXClk;
} MSPI_CLKConfig;


typedef struct
{
    MS_U8 u8ClkSpi_P1;
    MS_U8 u8ClkSpi_P2;
    MS_U8 u8ClkSpi_DIV;
    MS_U32 u32ClkSpi;
}ST_DRV_LD_MSPI_CLK;


typedef struct
{
    MS_BOOL bEnable;
    MSPI_CH eChannel;
    MSPI_Mode_Config_e eMSPIMode;
    MSPI_BaseAddr_st stBaseAddr;
    MSPI_CLKConfig tMSPI_ClockConfig;
    MSPI_DCConfig  tMSPI_DCConfig;
    MSPI_FrameConfig  tMSPI_FrameConfig;
    MSPI_ChipSelect_e eChipSel;
    MSPI_BitSeq_e eBLsbFirst;
    MS_U8 u8MspiBuffSizes;              //spi write buffer size
    void (*MSPIIntHandler)(void);       // interrupt handler
    MS_BOOL BIntEnable;                 // interrupt mode enable or polling mode
    //MS_U8 U8BitMapofConfig;
    //MS_U32 u32DevId;
} MSPI_config;


typedef enum
{
    E_MSPI_DBGLV_NONE,    //disable all the debug message
    E_MSPI_DBGLV_INFO,    //information
    E_MSPI_DBGLV_NOTICE,  //normal but significant condition
    E_MSPI_DBGLV_WARNING, //warning conditions
    E_MSPI_DBGLV_ERR,     //error conditions
    E_MSPI_DBGLV_CRIT,    //critical conditions
    E_MSPI_DBGLV_ALERT,   //action must be taken immediately
    E_MSPI_DBGLV_EMERG,   //system is unusable
    E_MSPI_DBGLV_DEBUG,   //debug-level messages
} MSPI_DbgLv;


typedef enum _MSPI_ERRORNOn {
     E_MSPI_OK = 0
    ,E_MSPI_INIT_FLOW_ERROR =1
    ,E_MSPI_DCCONFIG_ERROR =2
    ,E_MSPI_CLKCONFIG_ERROR =4
    ,E_MSPI_FRAMECONFIG_ERROR =8
    ,E_MSPI_OPERATION_ERROR = 0x10
    ,E_MSPI_PARAM_OVERFLOW = 0x20
    ,E_MSPI_MMIO_ERROR = 0x40
    ,E_MSPI_HW_NOT_SUPPORT = 0x80
    ,E_MSPI_NULL
} MSPI_ErrorNo;

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Description : MSPI initial
/// @return void:
//------------------------------------------------------------------------------
void HAL_MSPI_Init(MSPI_CH eChannel);
void HAL_MSPI_IntEnable(MSPI_CH eChannel,MS_BOOL bEnable);
void HAL_MSPI_SetChipSelect(MSPI_CH eChannel, MS_BOOL Enable, MSPI_ChipSelect_e eCS);
MS_BOOL HAL_MSPI_Read(MSPI_CH eChannel, MS_U8 *pData, MS_U16 u16Size);
MS_BOOL HAL_MSPI_Write(MSPI_CH eChannel, MS_U8 *pData, MS_U16 u16Size);
MS_BOOL HAL_MSPI_SetReadBufferSize(MSPI_CH eChannel,  MS_U8 u8Size);
MS_BOOL HAL_MSPI_SetWriteBufferSize(MSPI_CH eChannel,  MS_U8 u8Size);
MS_U8 HAL_MSPI_Read_Write(MSPI_CH eChannel,  MS_U8 *pReadData,U8 *pWriteData, MS_U8 u8WriteSize);
MS_BOOL HAL_MSPI_Reset_DCConfig(MSPI_CH eChannel);
MS_BOOL HAL_MSPI_Reset_FrameConfig(MSPI_CH eChannel);
MS_BOOL HAL_MSPI_Reset_CLKConfig(MSPI_CH eChannel);
MS_BOOL HAL_MSPI_Trigger(void);
void HAL_MSPI_SlaveEnable(MSPI_CH eChannel, MS_BOOL Enable);
void HAL_MSPI_SetDcTiming (MSPI_CH eChannel, eDC_config eDCField, MS_U8 u8DCtiming);
void HAL_MSPI_SetCLKTiming(MSPI_CH eChannel, eCLK_config eCLKField, MS_U8 u8CLKVal);
MS_BOOL HAL_MSPI_LD_CLK_Config(MS_U8 u8Chanel,MS_U32 u32MspiClk);

void HAL_MSPI_SetPerFrameSize(MSPI_CH eChannel, MS_BOOL bDirect, MS_U8 u8BufOffset, MS_U8 u8PerFrameSize);
MS_BOOL HAL_MSPI_CLOCK_Config(MSPI_CH eChannel, MS_U32 u32MaxClock);
#endif

