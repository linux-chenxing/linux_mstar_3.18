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
/// file    mdrv_mspi.c
/// @brief  mspi Driver
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////




#include <linux/autoconf.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/poll.h>
#include <linux/wait.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/types.h>
#include <linux/input.h>
#include <linux/spinlock.h>
#include <linux/semaphore.h>
#include <linux/platform_device.h>
#include <asm/io.h>

#include "mdrv_mspi.h"
#include "mhal_mspi.h"

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Description : Set detailed level of MSPI driver debug message
/// @param u8DbgLevel    \b IN  debug level for Serial Flash driver
/// @return TRUE : succeed
/// @return FALSE : failed to set the debug level
//------------------------------------------------------------------------------
MS_BOOL MDrv_MSPI_SetDbgLevel(MS_U8 u8DbgLevel)
{
    u8DbgLevel = u8DbgLevel;

    return TRUE;
}
EXPORT_SYMBOL(MDrv_MSPI_SetDbgLevel);

//------------------------------------------------------------------------------
/// Description : MSPI initial
/// @return E_MSPI_OK :
/// @return >1 : failed to initial
//------------------------------------------------------------------------------
MSPI_ErrorNo MDrv_MSPI_Init(MSPI_CH eChannel)
{
    MSPI_ErrorNo errorno = E_MSPI_OK;
    HAL_MSPI_Init(eChannel);
    HAL_MSPI_IntEnable(eChannel,TRUE);
    return errorno;
}
EXPORT_SYMBOL(MDrv_MSPI_Init);

//-------------------------------------------------------------------------------------------------
/// Description : read and write  MSPI
/// @param pData \b IN :pointer to receive data from MSPI read buffer
/// @param u16Size \ b OTU : read data size
/// @return the errorno of operation
//-------------------------------------------------------------------------------------------------
MS_U32  MDrv_MSPI_Read_Write(MSPI_CH eChannel,MS_U8 *pReadData,MS_U8 *pWriteData, MS_U16 u8WriteSiz)
{
    MS_U8  u8Index = 0;
    MS_U8  u8TempFrameCnt=0;
    MS_U8  U8TempLastFrameCnt=0;
    MS_BOOL ret = false;
    u8TempFrameCnt = u8WriteSiz/MAX_WRITE_BUF_SIZE; //Cut data to frame by max frame size
    U8TempLastFrameCnt = u8WriteSiz%MAX_WRITE_BUF_SIZE; //Last data less than a MAX_WRITE_BUF_SIZE fame
    for (u8Index = 0; u8Index < u8TempFrameCnt; u8Index++) {
        ret = HAL_MSPI_Read_Write(eChannel,pReadData+u8Index*MAX_WRITE_BUF_SIZE,pWriteData+u8Index*MAX_WRITE_BUF_SIZE,MAX_WRITE_BUF_SIZE);
        if (!ret) {
            return false;
        }
    }
    if(U8TempLastFrameCnt) {
        ret = HAL_MSPI_Read_Write(eChannel,pReadData+u8TempFrameCnt*MAX_WRITE_BUF_SIZE,pWriteData+u8TempFrameCnt*MAX_WRITE_BUF_SIZE,U8TempLastFrameCnt);
    }
    return ret;
}
EXPORT_SYMBOL(MDrv_MSPI_Read_Write);
//-------------------------------------------------------------------------------------------------
/// Description : read data from MSPI
/// @param pData \b IN :pointer to receive data from MSPI read buffer
/// @param u16Size \ b OTU : read data size
/// @return the errorno of operation
//-------------------------------------------------------------------------------------------------
MSPI_ErrorNo MDrv_MSPI_Read(MSPI_CH eChannel, MS_U8 *pData, MS_U16 u16Size)
{
    MS_U8  u8Index = 0;
    MS_U8  u8TempFrameCnt=0;
    MS_U8  U8TempLastFrameCnt=0;
    int  ret = 0;
    if(pData == NULL) {
        return E_MSPI_NULL;
    }
    HAL_MSPI_SetChipSelect( eChannel,  TRUE,  E_MSPI_ChipSelect_0);
    u8TempFrameCnt = u16Size/MAX_WRITE_BUF_SIZE; //Cut data to frame by max frame size
    U8TempLastFrameCnt = u16Size%MAX_WRITE_BUF_SIZE; //Last data less than a MAX_WRITE_BUF_SIZE fame
    for (u8Index = 0; u8Index < u8TempFrameCnt; u8Index++) {
        ret = HAL_MSPI_Read(eChannel,pData+u8Index*MAX_WRITE_BUF_SIZE,MAX_WRITE_BUF_SIZE);
        if (!ret) {
            return E_MSPI_OPERATION_ERROR;
        }
    }
    if(U8TempLastFrameCnt) {
        ret = HAL_MSPI_Read(eChannel,pData+u8TempFrameCnt*MAX_WRITE_BUF_SIZE,U8TempLastFrameCnt);
    }
    HAL_MSPI_SetChipSelect( eChannel,  false,  E_MSPI_ChipSelect_0);
    if (!ret) {
        return E_MSPI_OPERATION_ERROR;
    }
    return E_MSPI_OK;
}
EXPORT_SYMBOL(MDrv_MSPI_Read);

//------------------------------------------------------------------------------
/// Description : read data from MSPI
/// @param pData \b OUT :pointer to write  data to MSPI write buffer
/// @param u16Size \ b OTU : write data size
/// @return the errorno of operation
//------------------------------------------------------------------------------
MSPI_ErrorNo MDrv_MSPI_Write(MSPI_CH eChannel, MS_U8 *pData, MS_U16 u16Size)
{
    MS_U8  u8Index = 0;
    MS_U8  u8TempFrameCnt=0;
    MS_U8  U8TempLastFrameCnt=0;
    MS_BOOL  ret = false;
    u8TempFrameCnt = u16Size/MAX_WRITE_BUF_SIZE; //Cut data to frame by max frame size
    U8TempLastFrameCnt = u16Size%MAX_WRITE_BUF_SIZE; //Last data less than a MAX_WRITE_BUF_SIZE fame
    for (u8Index = 0; u8Index < u8TempFrameCnt; u8Index++) {
        ret = HAL_MSPI_Write(eChannel,pData+u8Index*MAX_WRITE_BUF_SIZE,MAX_WRITE_BUF_SIZE);
        if (!ret) {
            return E_MSPI_OPERATION_ERROR;
        }
    }

    if(U8TempLastFrameCnt) {
        ret = HAL_MSPI_Write(eChannel,pData+u8TempFrameCnt*MAX_WRITE_BUF_SIZE,U8TempLastFrameCnt);
    }

    if (!ret) {
        return E_MSPI_OPERATION_ERROR;
    }
    return E_MSPI_OK;
}
EXPORT_SYMBOL(MDrv_MSPI_Write);

MSPI_ErrorNo MDrv_MSPI_SetReadBufferSize(MSPI_CH eChannel,  MS_U8 u8Size)
{
    HAL_MSPI_SetReadBufferSize( eChannel,  u8Size);
    return E_MSPI_OK;

}
EXPORT_SYMBOL(MDrv_MSPI_SetReadBufferSize);


MSPI_ErrorNo MDrv_MSPI_SetWriteBufferSize(MSPI_CH eChannel,  MS_U8 u8Size)
{
        HAL_MSPI_SetWriteBufferSize( eChannel,  u8Size);
        return E_MSPI_OK;
}
EXPORT_SYMBOL(MDrv_MSPI_SetWriteBufferSize);
//------------------------------------------------------------------------------
/// Description : config spi transfer timing
/// @param ptDCConfig    \b OUT  struct pointer of transfer timing config
/// @return E_MSPI_OK : succeed
/// @return E_MSPI_DCCONFIG_ERROR : failed to config transfer timing
//------------------------------------------------------------------------------
MSPI_ErrorNo MDrv_MSPI_DCConfig(MSPI_CH eChannel, MSPI_DCConfig *ptDCConfig)
{
    MSPI_ErrorNo errnum = E_MSPI_OK;

    if(ptDCConfig == NULL) {
        HAL_MSPI_Reset_DCConfig(eChannel);
        return E_MSPI_OK;
    }
    HAL_MSPI_SetDcTiming(eChannel, E_MSPI_TRSTART ,ptDCConfig->u8TrStart);
    HAL_MSPI_SetDcTiming(eChannel, E_MSPI_TREND ,ptDCConfig->u8TrEnd);
    HAL_MSPI_SetDcTiming(eChannel, E_MSPI_TB ,ptDCConfig->u8TB);
    HAL_MSPI_SetDcTiming(eChannel, E_MSPI_TRW ,ptDCConfig->u8TRW);
    return errnum;
}
EXPORT_SYMBOL(MDrv_MSPI_DCConfig);

//------------------------------------------------------------------------------
/// Description : config spi clock setting
/// @param ptCLKConfig    \b OUT  struct pointer of clock config
/// @return E_MSPI_OK : succeed
/// @return E_MSPI_CLKCONFIG_ERROR : failed to config spi clock
//------------------------------------------------------------------------------
MSPI_ErrorNo  MDrv_MSPI_SetMode(MSPI_CH eChannel, MSPI_Mode_Config_e eMode)
{
    if (eMode >= E_MSPI_MODE_MAX) {
        return E_MSPI_PARAM_OVERFLOW;
    }

    switch (eMode) {
    case E_MSPI_MODE0:
        HAL_MSPI_SetCLKTiming(eChannel, E_MSPI_POL ,false);
        HAL_MSPI_SetCLKTiming(eChannel, E_MSPI_PHA ,false);

        break;
    case E_MSPI_MODE1:
        HAL_MSPI_SetCLKTiming(eChannel, E_MSPI_POL ,false);
        HAL_MSPI_SetCLKTiming(eChannel, E_MSPI_PHA ,true);
        break;
    case E_MSPI_MODE2:
        HAL_MSPI_SetCLKTiming(eChannel, E_MSPI_POL ,true);
        HAL_MSPI_SetCLKTiming(eChannel, E_MSPI_PHA ,false);
        break;
    case E_MSPI_MODE3:
        HAL_MSPI_SetCLKTiming(eChannel, E_MSPI_POL ,true);
        HAL_MSPI_SetCLKTiming(eChannel, E_MSPI_PHA ,true);
        break;
    default:
        return E_MSPI_OPERATION_ERROR;
    }

    return E_MSPI_OK;
}
EXPORT_SYMBOL(MDrv_MSPI_SetMode);

MSPI_ErrorNo MDrv_MSPI_SetCLK(MSPI_CH eChannel, MS_U8 U8ClockDiv)
{
    MSPI_ErrorNo errnum = E_MSPI_OK;
    HAL_MSPI_SetCLKTiming(eChannel, E_MSPI_CLK ,U8ClockDiv);
    return errnum;

}
EXPORT_SYMBOL(MDrv_MSPI_SetCLK);

// add to sync code from utopia for localdimming to set clk by ini
MSPI_ErrorNo MDrv_MSPI_SetCLKByINI(MSPI_CH eChannel, MS_U32 u32MspiClk)
{
    HAL_MSPI_LD_CLK_Config(eChannel,u32MspiClk);
    return E_MSPI_OK;
}
EXPORT_SYMBOL(MDrv_MSPI_SetCLKByINI);


MS_BOOL MDrv_MSPI_CLOCK_Config(MSPI_CH eChannel, MS_U32 u32MaxClock)
{
    return  HAL_MSPI_CLOCK_Config( eChannel,  u32MaxClock);
}
EXPORT_SYMBOL(MDrv_MSPI_CLOCK_Config);

//------------------------------------------------------------------------------
/// Description : config spi transfer timing
/// @param ptDCConfig    \b OUT  struct pointer of bits of buffer tranferred to slave config
/// @return E_MSPI_OK : succeed
/// @return E_MSPI_FRAMECONFIG_ERROR : failed to config transfered bit per buffer
//------------------------------------------------------------------------------
MSPI_ErrorNo MDrv_MSPI_FRAMEConfig(MSPI_CH eChannel, MSPI_FrameConfig  *ptFrameConfig)
{
    MSPI_ErrorNo errnum = E_MSPI_OK;
    MS_U8 u8Index = 0;

    if(ptFrameConfig == NULL) {
        HAL_MSPI_Reset_FrameConfig(eChannel);
        return E_MSPI_OK;
    }
    // read buffer bit config
    for(u8Index = 0; u8Index < MAX_READ_BUF_SIZE; u8Index++) {
        if(ptFrameConfig->u8RBitConfig[u8Index] > MSPI_FRAME_BIT_MAX) {
            errnum = E_MSPI_PARAM_OVERFLOW;
        } else {
            HAL_MSPI_SetPerFrameSize(eChannel, MSPI_READ_INDEX,  u8Index, ptFrameConfig->u8RBitConfig[u8Index]);
        }
    }
    //write buffer bit config
    for(u8Index = 0; u8Index < MAX_WRITE_BUF_SIZE; u8Index++) {
        if(ptFrameConfig->u8WBitConfig[u8Index] > MSPI_FRAME_BIT_MAX) {
            errnum = E_MSPI_PARAM_OVERFLOW;
        } else {
            HAL_MSPI_SetPerFrameSize(eChannel, MSPI_WRITE_INDEX,  u8Index, ptFrameConfig->u8WBitConfig[u8Index]);
        }
    }
    return errnum;
}
EXPORT_SYMBOL(MDrv_MSPI_FRAMEConfig);

//-------------------------------------------------------
// Description : MSPI Power state
//-------------------------------------------------------
MS_U32 MDrv_MSPI_SetPowerState(void)
{
    return 0;
}
EXPORT_SYMBOL(MDrv_MSPI_SetPowerState);
