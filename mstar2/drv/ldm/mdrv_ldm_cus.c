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
/// file    mdrv_ldm_cus.c
/// @brief  local dimming Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////


#if (defined (CONFIG_HAS_LD_DMA_MODE))

#include <linux/types.h>
#include <linux/fs.h>
#include <asm/io.h>

#include "mdrv_mstypes.h"
#include "mdrv_ldm_io.h"
#include "mdrv_ldm_common.h"
#include "mdrv_ldm_init.h"
#include "mdrv_ldm_interface.h"
#include "mdrv_ldm_algorithm.h"
#include "mdrv_ldm_parse.h"
#include "mhal_ldm.h"
#include "reg_ldm.h"
#include "reg_ldm_dma.h"
#include "mdrv_mbx.h"
#include "mdrv_ldm_cus.h"
#include "mdrv_ldm_dma.h"
#include "mdrv_mspi.h"
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/delay.h>

#include <mstar/mstar_chip.h>

#ifndef LD_MAX_BLOCK_SIZE
#define LD_MAX_BLOCK_SIZE (48*48*2)
#endif

extern MS_U16 *pSPI_blocks;
extern ST_DRV_LD_DMA_INFO  stDrvLdMDAInfo;
extern MSPI_config  stDrvLdMspiInfo;
extern MS_U8  gu8FrameIdx ;

MS_U8 u8LEDData[LD_MAX_BLOCK_SIZE];

static MS_U16 MDrv_LD_CustomerConvert(MS_U16* pu16Addr, MS_U8 *pu8Addr)
{
    return 0;
}

static MS_U16 MDrv_LD_CalculateCheckSum(MS_U8* pu8Addr,MS_U16 u16DataNum)
{
    switch(stDrvLdMDAInfo.eLDMACheckSumMode)
    {
        case E_LDMA_CHECKSUM_ALL:
        case E_LDMA_CHECKSUM_CMD_DATA:
        case E_LDMA_CHECKSUM_DATA_ONLY:
            //hw done the checksum,so do nothing
            break;
        case E_LDMA_NOCHECKSUM:
            //don't do checksum by dma hw ,do it by sw , so do  customer checksum follow the mcu spec
            //do something....
            //add the checksum in the tail of
            break;
        default:
            LD_ERROR("Unknown type %d\n", stDrvLdMDAInfo.eLDMACheckSumMode);
            break;
    }
    return 0;
}


MS_U16 MDrv_LD_ConvertLumaData2SPIData(MS_U16* pu16Addr, MS_U16 u16LEDNum)
{
    MS_U16 u16DataByteNum = 2*u16LEDNum;
    MS_U8* pu8Addr = (MS_U8 *) pu16Addr;
    MS_U8* pu8SPIAddr = (MS_U8 *)pSPI_blocks;
    MS_U16 u16PerLineLedNum = gu16LedWidth * 2;
    MS_U16 i =0,j = 0;
    MS_U8 u8PackLength = 32;
    MS_U16 u16dataoffset = 0;

    memset(u8LEDData,0,LD_MAX_BLOCK_SIZE);

    if(LD_MAX_BLOCK_SIZE < u16DataByteNum)
    {
        LD_ERROR("!! error buffer is not enough, modify it, pls!! led data:%d, buff:%d \n", u16DataByteNum, 2*LD_MAX_BLOCK_SIZE);
    }

    switch(stDrvLdMiscInfo.u8SPIBits)
    {
        case E_LD_SPI_DATA_16_BIT_LH: //setting ldm.ini by u8SPIBits = 0x0 ,u8DataPackMode =0x1 , u8MspiBuffSizes = 0x2,u8DataInvert=0x0
        case E_LD_SPI_DATA_8_BIT:     //setting ldm.ini by u8SPIBits = 0x1 , u8DataPackMode =0x1 , u8MspiBuffSizes = 0x1,u8DataInvert=0x1
        case E_LD_SPI_DATA_16_BIT_HL: //setting ldm.ini by  u8SPIBits = 0x5 , u8DataPackMode =0x1 ,  u8MspiBuffSizes = 0x2,u8DataInvert=0x1
            memcpy(u8LEDData, pu8Addr, u16DataByteNum);
            break;
        case E_LD_SPI_DATA_12_BIT_HL: //setting ldm.ini by  u8SPIBits = 0x3 , u8DataPackMode =0x1 , u8MspiBuffSizes = 0x2,u8DataInvert=0x1
        case E_LD_SPI_DATA_12_BIT_LH: //setting ldm.ini by  u8SPIBits = 0x4 , u8DataPackMode =0x1 , u8MspiBuffSizes = 0x2,u8DataInvert=0x0
            for(i=0;i<u16DataByteNum;)
            {
                u8LEDData[i] = ((pu8Addr[i+1]&0x0f)<<4) |((pu8Addr[i]&0xf0)>>4);
                u8LEDData[i+1] = (pu8Addr[i+1]>> 4);
                i = i + 2;
            }
            break;
        case E_LD_SPI_DATA_12_BIT:
            for(i=0,j=0;j<u16DataByteNum;)
            {
                u8LEDData[i] = pu8Addr[j+1];
                u8LEDData[i+1] = (pu8Addr[j]&0xf0)|((pu8Addr[j+3]&0xf0)>>4);
                u8LEDData[i+2] = ((pu8Addr[j+3]&0x0f)<<4)|((pu8Addr[j+2]&0xf0)>>4);
                i = i + 3;
                j = j + 4;
            }
            u16DataByteNum = i; //the data length
            u16PerLineLedNum = (gu16LedWidth*3)/2 ;
            break;
        case E_LD_SPI_DATA_CUS_BIT:
            MDrv_LD_CustomerConvert(pu16Addr,u8LEDData);
            break;
        default:
            LD_ERROR("Unknown type %d\n", stDrvLdMiscInfo.u8SPIBits);
            memcpy(u8LEDData, pu8Addr, u16DataByteNum);
            break;
    }

    if(stDrvLdMiscInfo.u8MirrorPanel == 1) //for HVmirror panel just test for Public version 0-normal panel 1-mirror panel
    {
        MS_U8 u8SPItemp = 0;
        for (i = 0; i < u16DataByteNum/2; i++)
        {
            u8SPItemp  = u8LEDData[i];
            u8LEDData[i] = u8LEDData[u16DataByteNum-i-1];
            u8LEDData[u16DataByteNum-i-1] = u8SPItemp;
        }
    }
    //change spi data to be DMA format
    u8PackLength = (((u16PerLineLedNum - 1) / stDrvLdMDAInfo.u16DMAPackLength) + 1) * stDrvLdMDAInfo.u16DMAPackLength;
    for (j = 0; j < gu16LedHeight; j++)
    {
        u16dataoffset = j*u8PackLength;
        for (i = 0; i < u16PerLineLedNum; i++)
        {
         pu8SPIAddr[i + u16dataoffset] = u8LEDData[i+j*u16PerLineLedNum];
        }
    }
    u16DataByteNum = u8PackLength * gu16LedHeight;

    MDrv_LD_CalculateCheckSum(u8LEDData,u16DataByteNum);
    if(enDbgLevel&E_LD_DEBUG_LEVEL_INFO)
    {
        static MS_U32 k;
        if(0 == ++k%4000)
        {
            LD_INFO(" output spi data bits : %d, the length of 16-bit:%d, the length of 12-bit:%d \n",
                    stDrvLdMiscInfo.u8SPIBits, u16LEDNum*2, u16DataByteNum);
            LD_INFO("\033[41;33m %s[%d]pSPI_blocks = 0x%p u8FrameIdx =%d\033[0m\n", __FUNCTION__, __LINE__,pSPI_blocks,gu8FrameIdx);
        }
    }

    return u16DataByteNum;
}

#endif

