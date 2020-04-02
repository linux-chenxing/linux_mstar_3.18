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

///////////////////////////////////////////////////////////////////////////////////////////////////
/// file    mhal_ldm_dma.h
/// @brief DMA drv file
/// @author MStar Semiconductor Inc.
//////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef _MHAL_DMA_H_
#define _MHAL_DMA_H_

typedef enum
{
    E_LDMA_SPI_TRIGGER_STOP,
    E_LDMA_SPI_TRIGGER_ONE_SHOT,
    E_LDMA_SPI_TRIGGER_EVERY_VSYNC,
}LDMA_SPITriggerMode_e;

typedef enum
{
    E_LDMA1,
    E_LDMA2,
    E_LDMA_MAX,
}LDMA_CH_e;

typedef enum
{
    E_LDMA_NOCHECKSUM,
    E_LDMA_CHECKSUM_ALL,
    E_LDMA_CHECKSUM_CMD_DATA,
    E_LDMA_CHECKSUM_DATA_ONLY,
}LDMA_CheckSumMode_e;

typedef enum
{
    E_LDMA_GETMENULOAD_BUSY,
    E_LDMA_GETMENULOAD_DONE,
    E_LDMA_GETMENULOAD_ABORRED_ONCE,
}LDMA_GetStatus_e;

typedef struct
{
    LDMA_CH_e eCurrentCH;
    MS_U32 VirtLdmaBaseAddr;
} LDMA_BaseAddr_st;

typedef struct
{
    U16 u16Delay0Cnt0;
    U16 u16Delay0Cnt1;
    U16 u16Delay1Cnt0;
    U16 u16Delay1Cnt1;
} LDMA_TrigDelay_st;

typedef struct
{
    LDMA_CH_e eLDMAchanel;
    LDMA_SPITriggerMode_e eLDMATrimode;
    LDMA_CheckSumMode_e eLDMACheckSumMode;
    MS_U16 u16MspiHead[8];                    //led mcu  start Indicator1 command
    MS_U16 u16DMADelay[4];                   //The delay of dma
    MS_U8 u8cmdlength;                      //the number of spi head
    MS_U8 u8BLWidth;                       //led Width
    MS_U8 u8BLHeight;                      //led Height
    MS_U16 u16LedNum;                      // led total num
    MS_U8 u8DataPackMode;                  //spi data pack mode.1:16bits,0:8bits
    MS_U16 u16DMAPackLength;               //setting by u8DataPackMode
    MS_U8 u8DataInvert;                    //spi data invert mode.0 : [15:0], 1: {[7:0], [15:8]}, 2: {[0:15]}
    MS_U32 u32DMABaseOffset;
}ST_DRV_LD_DMA_INFO;



MS_U8 Mhal_LDMA_Init(LDMA_CH_e eCH);
MS_U8 Mhal_LDMA_SetSPITriggerMode(LDMA_CH_e eCH, LDMA_SPITriggerMode_e eTriggerMode);
MS_U8 Mhal_LDMA_SetMenuloadNumber(LDMA_CH_e eCH, MS_U32 u32MenuldNum);
MS_U8 Mhal_LDMA_SetSPICommandFormat(LDMA_CH_e eCH,MS_U8 u8CmdLen, MS_U16* pu16CmdBuf);
MS_U8 Mhal_LDMA_SetCheckSumMode(LDMA_CH_e eCH,LDMA_CheckSumMode_e eSumMode);
MS_U8 Mhal_LDMA_GetStatus(LDMA_CH_e eCH,LDMA_GetStatus_e index);
MS_U8 Mhal_LDMA_SetTrigDelay(LDMA_CH_e eCH,LDMA_TrigDelay_st* pstTrigDelay);
MS_U8 Mhal_LDMA_EnableCS(LDMA_CH_e eCH, MS_BOOL bEnable);
MS_U8 Mhal_LDMA_SetSPI16BitMode(LDMA_CH_e eCH, MS_BOOL bEnable);

#endif

