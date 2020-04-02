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
///
/// file    halMSPI.c
/// @brief  Master SPI Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------

#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/types.h>
#include <asm/io.h>
#include <mstar/mstar_chip.h>
#include "mst_devid.h"
#include <linux/slab.h>
#include "mdrv_mstypes.h"
#include "mdrv_types.h"
#include "reg_mspi.h"
#include "mhal_mspi.h"
//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

#if defined(CONFIG_ARM64)
extern ptrdiff_t mstar_pm_base;
#define REG_ADDR(addr)  (*((volatile U16 *)((mstar_pm_base + (addr )))))
#define BASEREG_ADDR(addr)  ((mstar_pm_base + (addr )))
#else
#define REG_ADDR(addr)  (*((volatile U16 *)((REG_RIU_BASE + (addr )))))
#define BASEREG_ADDR(addr)  (REG_RIU_BASE + (addr ))
#endif


// read 2 byte
#define MSPI_READ(_reg_)          (REG_ADDR(_hal_msp.VirtMspBaseAddr + ((_reg_)<<2)))

// write low byte
#define REG_WL(_reg_, _val_)    \
        do{ REG_ADDR(_reg_) = (REG_ADDR(_hal_msp.VirtClkBaseAddr + ((_reg_)<<2)) & 0xFF00) | ((_val_) & 0x00FF); }while(0)

// write high byte
#define REG_WH(_reg_, _val_)    \
        do{ REG_ADDR(_reg_) = (REG_ADDR(_hal_msp.VirtClkBaseAddr + ((_reg_)<<2))  & 0x00FF) | ((_val_) << 8); }while(0)

// write 2 byte
#define MSPI_WRITE(_reg_, _val_)    \
        do{ REG_ADDR(_hal_msp.VirtMspBaseAddr + ((_reg_)<<2)) =(_val_) ; }while(0)

#define CLK_READ(_reg_)                       (REG_ADDR(_hal_msp.VirtClkBaseAddr + ((_reg_)<<2)))
#define CLK_WRITE(_reg_, _val_)    \
        do{ REG_ADDR(_hal_msp.VirtClkBaseAddr + ((_reg_)<<2)) =(_val_) ; }while(0)
//#define _HAL_MSPI_Trigger()                 MSPI_WRITE(MSPI_TRIGGER_OFFSET,MSPI_TRIGGER)
#define _HAL_MSPI_ClearDone()               MSPI_WRITE(MSPI_DONE_CLEAR_OFFSET,MSPI_CLEAR_DONE)
#define MAX_CHECK_CNT                       2000
#define DEBUG_HAL_MSPI(debug_level, x)      do { if (_u8HalMSPIDbgLevel >= (debug_level)) (x); } while(0)

//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
static MSPI_BaseAddr_st _hal_msp = {
    .eCurrentCH = E_MSPI1,
    .VirtMspBaseAddr = REG_MSPI1_BK,
    .VirtClkBaseAddr =BK_CLK0,
};

static struct mutex     hal_mspi_lock;
static MS_U8 _u8MSPIDbgLevel = 1;
MSPI_config stMSPI_config[2];
MSPI_BaseAddr_st *pstBaseAddr = NULL;
//-------------------------------------------------------------------------------------------------
// Local Functions
//-------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Description : Set MSPI read /write buf size in current operation
/// @param Direct \b OUT: read /write operation direction
/// @param Size   \b OUT: size of read /write buffer
//------------------------------------------------------------------------------

static void _HAL_MSPI_CheckandSetBaseAddr(MSPI_CH eChannel)
{
    if(eChannel == _hal_msp.eCurrentCH) {
        return;
    } else if(eChannel ==  E_MSPI1) {
        _hal_msp.VirtMspBaseAddr = REG_MSPI1_BK;
    } else if(eChannel ==  E_MSPI2) {
        _hal_msp.VirtMspBaseAddr = REG_MSPI2_BK;
    } else {
        DEBUG_MSPI(E_MSPI_DBGLV_ERR,printk("MSPI Channel is out of range!\n"));
        return ;
    }
}


static void _HAL_MSPI_RWBUFSize(MS_BOOL Direct, MS_U8 Size)
{
    MS_U16 u16Data = 0;
    u16Data = MSPI_READ(MSPI_RBF_SIZE_OFFSET);

    if(Direct == MSPI_READ_INDEX) {
        u16Data &= MSPI_RWSIZE_MASK;
        u16Data |= Size << MSPI_RSIZE_BIT_OFFSET;
    } else {
        u16Data &= ~MSPI_RWSIZE_MASK;
        u16Data |= Size;
    }
    MSPI_WRITE(MSPI_RBF_SIZE_OFFSET, u16Data);
}

//------------------------------------------------------------------------------
/// Description : SPI chip select enable and disable
/// @param Enable \b OUT: enable or disable chip select
//------------------------------------------------------------------------------
static void _HAL_MSPI_ChipSelect(MS_BOOL Enable ,MSPI_ChipSelect_e eSelect)
{
    MS_U16 regdata = 0;
    MS_U8 bitmask = 0;
    regdata = MSPI_READ(MSPI_CHIP_SELECT_OFFSET);
    if(Enable) {
        bitmask = ~(1 << eSelect);
        regdata &= bitmask;
    } else {
        bitmask = (1 << eSelect);
        regdata |= bitmask;
    }
    MSPI_WRITE(MSPI_CHIP_SELECT_OFFSET, regdata);
}

//------------------------------------------------------------------------------
/// Description : check MSPI operation complete
/// @return TRUE :  operation complete
/// @return FAIL : failed timeout
//------------------------------------------------------------------------------
static MS_BOOL _HAL_MSPI_CheckDone(void)
{
    MS_U16 uCheckDoneCnt = 0;
    MS_U16 uDoneFlag = 0;
    while(uCheckDoneCnt < MAX_CHECK_CNT) {
        uDoneFlag = MSPI_READ(MSPI_DONE_OFFSET);
        if(uDoneFlag & MSPI_DONE_FLAG) {
            return TRUE;
        }
        uCheckDoneCnt++;
    }
    DEBUG_MSPI(E_MSPI_DBGLV_ERR,printk("ERROR:MSPI Operation Timeout!!!!!\n"));
    return FALSE;
}


//------------------------------------------------------------------------------
/// Description : Trigger MSPI operation
/// @return TRUE  : operation success
/// @return FALSE : operation timeout
//------------------------------------------------------------------------------
MS_BOOL _HAL_MSPI_Trigger(void)
{
    // chip select enable
    //  _HAL_MSPI_ChipSelect(TRUE);
    // trigger operation
    MSPI_WRITE(MSPI_TRIGGER_OFFSET,MSPI_TRIGGER);
    // check operation complete
    if(!_HAL_MSPI_CheckDone()) {
        return FALSE;
    }
    // clear done flag
    _HAL_MSPI_ClearDone();
    // chip select disable
    // _HAL_MSPI_ChipSelect(FALSE);
    // reset read/write buffer size
    MSPI_WRITE(MSPI_RBF_SIZE_OFFSET,0x0);
    return TRUE;
}

static int  _HAL_MSPI_Deplux_RD(U8 *pData, U8 u8Size)
{
    MS_U8  u8Index = 0;
    MS_U16  u16TempBuf = 0;
    MS_U16 i =0, j = 0;

    mutex_lock(&hal_mspi_lock);
    for(i = 0; i < u8Size; i+= MAX_READ_BUF_SIZE) {
        u16TempBuf = u8Size - i;
        if(u16TempBuf > MAX_READ_BUF_SIZE) {
            j = MAX_READ_BUF_SIZE;
        } else {
            j = u16TempBuf;
        }
        for(u8Index = 0; u8Index < j; u8Index++) {

            if(u8Index & 1) {
                u16TempBuf = MSPI_READ((MSPI_FULL_DEPLUX_RD00 + (u8Index >> 1)));
                DEBUG_MSPI(E_MSPI_DBGLV_DEBUG,printk("read Buf data %x index %d\n",u16TempBuf, u8Index));
                pData[u8Index] = u16TempBuf >> 8;
                pData[u8Index-1] = u16TempBuf & 0xFF;
            } else if(u8Index == (j -1)) {
                u16TempBuf = MSPI_READ((MSPI_FULL_DEPLUX_RD00 + (u8Index >> 1)));
                DEBUG_MSPI(E_MSPI_DBGLV_DEBUG,printk("read Buf data %x index %d\n",u16TempBuf, u8Index));
                pData[u8Index] = u16TempBuf & 0xFF;
            }
        }
        pData+= j;
    }

    mutex_unlock(&hal_mspi_lock);

    return u8Size;
}

static MS_U32 _HAL_MSPI_GetCLK(MS_U16 u16CLK)
{
    MS_U32 u32MspiClk = 0;
    switch (u16CLK) {
    case 0:
        u32MspiClk = 27000000/2;
        return u32MspiClk;
    case 1:
        u32MspiClk = 48000000/2;
        return u32MspiClk;
    case 2:
        u32MspiClk = 62000000/2;
        return u32MspiClk;
    case 3:
        u32MspiClk = 72000000/2;
        return u32MspiClk;
    case 4:
        u32MspiClk = 86000000/2;
        return u32MspiClk;
    case 5:
        u32MspiClk = 108000000/2;
        return u32MspiClk;
    case 6:
        u32MspiClk = 123000000/2;
        return u32MspiClk;
    case 7:
        u32MspiClk = 160000000/2;
        return u32MspiClk;
    default:
        printk("ERROR CLOCK SETTING NOT SUPPORT \r\n");
        return u32MspiClk;
    }
}

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Description : MSPI initial
/// @return void :
//------------------------------------------------------------------------------
void HAL_MSPI_Init(MSPI_CH eChannel)
{
    MS_U16 TempData;
    //init  MSPI
    mutex_init(&hal_mspi_lock);
    if(eChannel == E_MSPI1) {
        stMSPI_config[0].eChannel = E_MSPI1;
        stMSPI_config[0].stBaseAddr.VirtClkBaseAddr =   REG_MSPI1_BK;
        stMSPI_config[0].bEnable = 1;
    } else if(eChannel == E_MSPI2) {
        stMSPI_config[1].eChannel = E_MSPI2;
        stMSPI_config[1].stBaseAddr.VirtClkBaseAddr =   REG_MSPI2_BK;
        stMSPI_config[1].bEnable = 1;
    } else {
        DEBUG_MSPI(E_MSPI_DBGLV_ERR,printk("MSPI Channel is out of range!\n"));
        return ;
    }
    mutex_lock(&hal_mspi_lock);
    _HAL_MSPI_CheckandSetBaseAddr(eChannel);
    MSPI_WRITE(MSPI_CTRL_OFFSET,MSPI_READ(MSPI_CTRL_OFFSET)|(MSPI_RESET|MSPI_ENABLE));
    // CLK SETTING
    TempData = CLK_READ(MSPI_CLK_CFG);
    TempData &= !(MSPI_CLK_MASK);
    TempData |= MSPI_CLK_DEFAULT;
    CLK_WRITE(MSPI_CLK_CFG, TempData);

    //chiptop
    REG_ADDR(0x101E00*2 + ((0xb4)<<1)) =(0x4F) ;
    REG_ADDR(0x101E00*2 + ((0x06)<<1)) =(0x44) ;
    mutex_unlock(&hal_mspi_lock);

}

//------------------------------------------------------------------------------
/// Description : MSPI  interrupt enable
/// @param bEnable \b OUT: enable or disable mspi interrupt
/// @return void:
//------------------------------------------------------------------------------
void HAL_MSPI_IntEnable(MSPI_CH eChannel,MS_BOOL bEnable)
{
    mutex_lock(&hal_mspi_lock);
    _HAL_MSPI_CheckandSetBaseAddr(eChannel);
    if(bEnable) {
        MSPI_WRITE(MSPI_CTRL_OFFSET,MSPI_READ(MSPI_CTRL_OFFSET)|MSPI_INT_ENABLE);
        stMSPI_config[eChannel].BIntEnable=TRUE;
    } else {
        MSPI_WRITE(MSPI_CTRL_OFFSET,MSPI_READ(MSPI_CTRL_OFFSET)&(~MSPI_INT_ENABLE));
        stMSPI_config[eChannel].BIntEnable=FALSE;
    }
    mutex_unlock(&hal_mspi_lock);
}


//------------------------------------------------------------------------------
/// Description : Set MSPI chip select
/// @param u8CS \u8 OUT: MSPI chip select
/// @return void:
//------------------------------------------------------------------------------
void HAL_MSPI_SetChipSelect(MSPI_CH eChannel, MS_BOOL Enable, MSPI_ChipSelect_e eCS)
{
    mutex_lock(&hal_mspi_lock);
    _HAL_MSPI_CheckandSetBaseAddr( eChannel);
    stMSPI_config[eChannel].eChipSel=eCS;
    _HAL_MSPI_ChipSelect( Enable, eCS);

    mutex_unlock(&hal_mspi_lock);
}

//-------------------------------------------------------------------------------------------------
/// Description : read data from MSPI
/// @param pData \b IN :pointer to receive data from MSPI read buffer
/// @param u16Size \ b OTU : read data size
/// @return TRUE  : read data success
/// @return FALSE : read data fail
//-------------------------------------------------------------------------------------------------
MS_BOOL HAL_MSPI_Read(MSPI_CH eChannel, MS_U8 *pData, MS_U16 u16Size)
{
    MS_U8  u8Index = 0;
    MS_U16  u16TempBuf = 0;
    MS_U16 i =0, j = 0;

    mutex_lock(&hal_mspi_lock);
    _HAL_MSPI_CheckandSetBaseAddr( eChannel);
    for(i = 0; i < u16Size; i+= MAX_READ_BUF_SIZE) {
        u16TempBuf = u16Size - i;
        if(u16TempBuf > MAX_READ_BUF_SIZE) {
            j = MAX_READ_BUF_SIZE;
        } else {
            j = u16TempBuf;
        }
        _HAL_MSPI_RWBUFSize(MSPI_READ_INDEX, j);

        _HAL_MSPI_Trigger();
        for(u8Index = 0; u8Index < j; u8Index++) {

            if(u8Index & 1) {
                u16TempBuf = MSPI_READ((MSPI_READ_BUF_OFFSET + (u8Index >> 1)));
                DEBUG_MSPI(E_MSPI_DBGLV_DEBUG,printk("read Buf data %x index %d\n",u16TempBuf, u8Index));
                pData[u8Index] = u16TempBuf >> 8;
                pData[u8Index-1] = u16TempBuf & 0xFF;
            } else if(u8Index == (j -1)) {
                u16TempBuf = MSPI_READ((MSPI_READ_BUF_OFFSET + (u8Index >> 1)));
                DEBUG_MSPI(E_MSPI_DBGLV_DEBUG,printk("read Buf data %x index %d\n",u16TempBuf, u8Index));
                pData[u8Index] = u16TempBuf & 0xFF;
            }
        }
        pData+= j;
    }

    mutex_unlock(&hal_mspi_lock);
    return TRUE;
}

//------------------------------------------------------------------------------
/// Description : read data from MSPI
/// @param pData \b OUT :pointer to write  data to MSPI write buffer
/// @param u16Size \ b OTU : write data size
/// @return TRUE  : write data success
/// @return FALSE : wirte data fail
//------------------------------------------------------------------------------
MS_BOOL HAL_MSPI_Write(MSPI_CH eChannel, MS_U8 *pData, MS_U16 u16Size)
{
    MS_U8  u8Index = 0;
    MS_U16 u16TempBuf = 0;
    mutex_lock(&hal_mspi_lock);
    _HAL_MSPI_CheckandSetBaseAddr( eChannel);

    for(u8Index = 0; u8Index < u16Size; u8Index++) {
        if(u8Index & 1) {
            u16TempBuf = (pData[u8Index] << 8) | pData[u8Index-1];
            DEBUG_MSPI(E_MSPI_DBGLV_DEBUG,printk("write Buf data %x index %d\n",u16TempBuf, u8Index));
            MSPI_WRITE((MSPI_WRITE_BUF_OFFSET + (u8Index >> 1)),u16TempBuf);
        } else if(u8Index == (u16Size -1)) {
            DEBUG_MSPI(E_MSPI_DBGLV_DEBUG,printk("write Buf data %x index %d\n",pData[u8Index], u8Index));
            MSPI_WRITE((MSPI_WRITE_BUF_OFFSET + (u8Index >> 1)),pData[u8Index]);
        }
    }

    _HAL_MSPI_RWBUFSize(MSPI_WRITE_INDEX, u16Size);
    _HAL_MSPI_Trigger();
    // set write data size
    mutex_unlock(&hal_mspi_lock);
    return TRUE;
}

//return read data size
MS_U8 HAL_MSPI_Read_Write(MSPI_CH eChannel,  MS_U8 *pReadData,U8 *pWriteData, MS_U8 u8WriteSize)
{
    MS_U8 ret =0;
       MS_U8  u8Index = 0;
    MS_U16 u16TempBuf = 0;
        mutex_lock(&hal_mspi_lock);
    _HAL_MSPI_CheckandSetBaseAddr( eChannel);
    if (u8WriteSize>MAX_WRITE_BUF_SIZE || u8WriteSize <= 0)
        return 0;

        for(u8Index = 0; u8Index < u8WriteSize; u8Index++) {
        if(u8Index & 1) {
            u16TempBuf = (pWriteData[u8Index] << 8) | pWriteData[u8Index-1];
            DEBUG_MSPI(E_MSPI_DBGLV_DEBUG,printk("write Buf data %x index %d\n",u16TempBuf, u8Index));
            MSPI_WRITE((MSPI_WRITE_BUF_OFFSET + (u8Index >> 1)),u16TempBuf);
        } else if(u8Index == (u8WriteSize -1)) {
            DEBUG_MSPI(E_MSPI_DBGLV_DEBUG,printk("write Buf data %x index %d\n",pWriteData[u8Index], u8Index));
            MSPI_WRITE((MSPI_WRITE_BUF_OFFSET + (u8Index >> 1)),pWriteData[u8Index]);
        }
    }
    _HAL_MSPI_RWBUFSize(MSPI_WRITE_INDEX, u8WriteSize);
   ret =  _HAL_MSPI_Trigger();
    if (ret)
    {
        ret = _HAL_MSPI_Deplux_RD(pReadData,u8WriteSize);
            mutex_unlock(&hal_mspi_lock);
        return ret;
    }
    else
    {
        mutex_unlock(&hal_mspi_lock);
        return 0;
    }
}

MS_BOOL HAL_MSPI_SetReadBufferSize(MSPI_CH eChannel,  MS_U8 u8Size)
{
        mutex_lock(&hal_mspi_lock);
    _HAL_MSPI_CheckandSetBaseAddr( eChannel);
        _HAL_MSPI_RWBUFSize(MSPI_READ_INDEX, u8Size);
            mutex_unlock(&hal_mspi_lock);
            return TRUE;
}

MS_BOOL HAL_MSPI_SetWriteBufferSize(MSPI_CH eChannel,  MS_U8 u8Size)
{
    mutex_lock(&hal_mspi_lock);
    _HAL_MSPI_CheckandSetBaseAddr( eChannel);
    _HAL_MSPI_RWBUFSize(MSPI_WRITE_INDEX, u8Size);
    mutex_unlock(&hal_mspi_lock);
    return TRUE;
}
//------------------------------------------------------------------------------
/// Description : Reset  DC register setting of MSPI
/// @param NONE
/// @return TRUE  : reset complete
//------------------------------------------------------------------------------
MS_BOOL HAL_MSPI_Reset_DCConfig(MSPI_CH eChannel)
{
    mutex_lock(&hal_mspi_lock);
    _HAL_MSPI_CheckandSetBaseAddr( eChannel);
    //DC reset
    MSPI_WRITE(MSPI_DC_TR_START_OFFSET, 0x00);
    MSPI_WRITE(MSPI_DC_TB_OFFSET, 0x00);
    stMSPI_config[eChannel].tMSPI_DCConfig.u8TrStart = 0x0;
    stMSPI_config[eChannel].tMSPI_DCConfig.u8TB = 0x0;
    stMSPI_config[eChannel].tMSPI_DCConfig.u8TrEnd = 0x0;
    stMSPI_config[eChannel].tMSPI_DCConfig.u8TRW = 0x0;
    mutex_unlock(&hal_mspi_lock);
    return TRUE;
}

//------------------------------------------------------------------------------
/// Description : Reset  Frame register setting of MSPI
/// @param NONE
/// @return TRUE  : reset complete
//------------------------------------------------------------------------------
MS_BOOL HAL_MSPI_Reset_FrameConfig(MSPI_CH eChannel)
{
    mutex_lock(&hal_mspi_lock);
    _HAL_MSPI_CheckandSetBaseAddr( eChannel);
    // Frame reset
    MSPI_WRITE(MSPI_FRAME_WBIT_OFFSET, 0xFFF);
    MSPI_WRITE(MSPI_FRAME_WBIT_OFFSET+2, 0xFFF);
    MSPI_WRITE(MSPI_FRAME_RBIT_OFFSET, 0xFFF);
    MSPI_WRITE(MSPI_FRAME_RBIT_OFFSET+2, 0xFFF);
    mutex_unlock(&hal_mspi_lock);
    //  stMSPI_config[eChannel].tMSPI_FrameConfig.u8RBitConfig = {0xFF,};
    // stMSPI_config[eChannel].tMSPI_FrameConfig.u8WBitConfig = {0xFF,};
    return TRUE;
}

//------------------------------------------------------------------------------
/// Description : Reset  CLK register setting of MSPI
/// @param NONE
/// @return TRUE  : reset complete
//------------------------------------------------------------------------------
MS_BOOL HAL_MSPI_Reset_CLKConfig(MSPI_CH eChannel)
{
    MS_U16 Tempbuf;
    mutex_lock(&hal_mspi_lock);
    _HAL_MSPI_CheckandSetBaseAddr( eChannel);
    //reset clock
    Tempbuf = MSPI_READ(MSPI_CTRL_OFFSET);
    Tempbuf &= 0x3F;
    MSPI_WRITE(MSPI_CTRL_OFFSET, Tempbuf);
    stMSPI_config[eChannel].tMSPI_ClockConfig.U8Clock = Tempbuf;
    mutex_unlock(&hal_mspi_lock);
    return TRUE;
}

MS_BOOL HAL_MSPI_Trigger(void)
{
    MS_BOOL ret= 0;
    mutex_lock(&hal_mspi_lock);
    ret =  _HAL_MSPI_Trigger();
    mutex_unlock(&hal_mspi_lock);
    return ret;
}

//------------------------------------------------------------------------------
/// Description : config spi transfer timing
/// @param ptDCConfig    \b OUT  struct pointer of bits of buffer tranferred to slave config
/// @return NONE
//------------------------------------------------------------------------------
void HAL_MSPI_SetDcTiming (MSPI_CH eChannel, eDC_config eDCField, MS_U8 u8DCtiming)
{
    MS_U16 u16TempBuf = 0;
    mutex_lock(&hal_mspi_lock);
    _HAL_MSPI_CheckandSetBaseAddr( eChannel);
    switch(eDCField) {
    case E_MSPI_TRSTART:
        u16TempBuf = MSPI_READ(MSPI_DC_TR_START_OFFSET);
        u16TempBuf &= (~MSPI_DC_MASK);
        u16TempBuf |= u8DCtiming;
        MSPI_WRITE(MSPI_DC_TR_START_OFFSET, u16TempBuf);
        stMSPI_config[eChannel].tMSPI_DCConfig.u8TrStart = u16TempBuf;
        break;
    case E_MSPI_TREND:
        u16TempBuf = MSPI_READ(MSPI_DC_TR_END_OFFSET);
        u16TempBuf &= MSPI_DC_MASK;
        u16TempBuf |= u8DCtiming << MSPI_DC_BIT_OFFSET;
        MSPI_WRITE(MSPI_DC_TR_END_OFFSET, u16TempBuf);
        stMSPI_config[eChannel].tMSPI_DCConfig.u8TrEnd = u16TempBuf;
        break;
    case E_MSPI_TB:
        u16TempBuf = MSPI_READ(MSPI_DC_TB_OFFSET);
        u16TempBuf &= (~MSPI_DC_MASK);
        u16TempBuf |= u8DCtiming;
        MSPI_WRITE(MSPI_DC_TB_OFFSET, u16TempBuf);
        stMSPI_config[eChannel].tMSPI_DCConfig.u8TB = u16TempBuf;
        break;
    case E_MSPI_TRW:
        u16TempBuf = MSPI_READ(MSPI_DC_TRW_OFFSET);
        u16TempBuf &= MSPI_DC_MASK;
        u16TempBuf |= u8DCtiming << MSPI_DC_BIT_OFFSET;
        MSPI_WRITE(MSPI_DC_TRW_OFFSET, u16TempBuf);
        stMSPI_config[eChannel].tMSPI_DCConfig.u8TRW = u16TempBuf;
        break;
    }

    mutex_unlock(&hal_mspi_lock);
}

void HAL_MSPI_SetCLKTiming(MSPI_CH eChannel, eCLK_config eCLKField, MS_U8 u8CLKVal)
{
    MS_U16 u16TempBuf = 0;
    mutex_lock(&hal_mspi_lock);
    _HAL_MSPI_CheckandSetBaseAddr( eChannel);
    switch(eCLKField) {
    case E_MSPI_POL:
        u16TempBuf = MSPI_READ(MSPI_CLK_CLOCK_OFFSET);
        u16TempBuf &= ~(MSPI_CLK_POLARITY_MASK);
        u16TempBuf |= u8CLKVal << MSPI_CLK_POLARITY_BIT_OFFSET;
        break;
    case E_MSPI_PHA:
        u16TempBuf = MSPI_READ(MSPI_CLK_CLOCK_OFFSET);
        u16TempBuf &= ~(MSPI_CLK_PHASE_MASK);
        u16TempBuf |= u8CLKVal << MSPI_CLK_PHASE_BIT_OFFSET;
        break;
    case E_MSPI_CLK:
        u16TempBuf = MSPI_READ(MSPI_CLK_CLOCK_OFFSET);
        u16TempBuf &= MSPI_CLK_CLOCK_MASK;
        u16TempBuf |= u8CLKVal << MSPI_CLK_CLOCK_BIT_OFFSET;
        break;
    }
    MSPI_WRITE(MSPI_CLK_CLOCK_OFFSET, u16TempBuf);
    stMSPI_config[eChannel].tMSPI_ClockConfig.BClkPhase = u16TempBuf&MSPI_CLK_PHASE_MASK;
    stMSPI_config[eChannel].tMSPI_ClockConfig.BClkPolarity= u16TempBuf&MSPI_CLK_POLARITY_MASK;
    stMSPI_config[eChannel].tMSPI_ClockConfig.U8Clock=  u16TempBuf&MSPI_CLK_CLOCK_MASK;

    mutex_unlock(&hal_mspi_lock);
}


static MS_U8 clk_spi_m_p1[8] = {27, 48, 62, 72, 86, 108, 123, 160};
static MS_U16 clk_spi_div[8] = {2, 4, 8, 16, 32, 64, 128, 256};
static ST_DRV_LD_MSPI_CLK clk_buffer[256];


MS_BOOL HAL_MSPI_LD_CLK_Config(MS_U8 u8Chanel,MS_U32 u32MspiClk)
{
    MS_U16 i = 0;
    MS_U16 j = 0;
    MS_U16 k= 0;
    MS_U16 TempData = 0;
    MS_U32 clk =0;
    ST_DRV_LD_MSPI_CLK temp;
    if(u8Chanel >=2)
        return FALSE;
    memset(&temp,0,sizeof(ST_DRV_LD_MSPI_CLK));
    memset(&clk_buffer,0,sizeof(ST_DRV_LD_MSPI_CLK)*256);
    for(i = 0;i < 8;i++)//clk_spi_m_p1
    {
        for(j =0 ;j <16;j++)//clk_spi_m_p2
        {
            for(k = 0;k<8;k++)//spi div
            {
                clk = clk_spi_m_p1[i]*1000000;
                clk_buffer[k+8*j+16*i].u8ClkSpi_P1 = i;
                clk_buffer[k+8*j+16*i].u8ClkSpi_P2 = j;
                clk_buffer[k+8*j+16*i].u8ClkSpi_DIV = k ;
                clk_buffer[k+8*j+16*i].u32ClkSpi = clk/((j+1)*(clk_spi_div[k]));
            }
        }
    }
    for(i = 0;i<256;i++)
    {
        for(j = i;j<256;j++)
        {
            if(clk_buffer[i].u32ClkSpi > clk_buffer[j].u32ClkSpi)
            {
                memcpy(&temp,&clk_buffer[i],sizeof(ST_DRV_LD_MSPI_CLK));

                memcpy(&clk_buffer[i],&clk_buffer[j],sizeof(ST_DRV_LD_MSPI_CLK));

                memcpy(&clk_buffer[j],&temp,sizeof(ST_DRV_LD_MSPI_CLK));
            }
        }
    }
    for(i = 0;i<256;i++)
    {
        if(u32MspiClk <= clk_buffer[i].u32ClkSpi)
        {
            break;
        }
    }
    //match Closer clk
    if((u32MspiClk - clk_buffer[i-1].u32ClkSpi)<(clk_buffer[i].u32ClkSpi - u32MspiClk))
    {
        i -= 1;
    }
    printk("[Lwc Debug] u8ClkSpi_P1 =%d\n",clk_buffer[i].u8ClkSpi_P1);
    printk("[Lwc Debug] u8ClkSpi_P2 =%d\n",clk_buffer[i].u8ClkSpi_P2);
    printk("[Lwc Debug] u8ClkSpi_DIV =%d\n",clk_buffer[i].u8ClkSpi_DIV);
    printk("[Lwc Debug] u32ClkSpi = %ld\n",clk_buffer[i].u32ClkSpi);

    if(u8Chanel == 0)//mspi0
    {
        TempData = CLK_READ(MSPI_CLK_CFG);
        TempData &= ~(MSPI_CLK_MASK);
        TempData |= clk_buffer[i].u8ClkSpi_P1 << 2;
        CLK_WRITE(MSPI_CLK_CFG, TempData);

        TempData = CLK_READ(MSPI_CLK_DIV);
        TempData &= ~(MSPI_CLK_DIV_MASK);
        TempData |= clk_buffer[i].u8ClkSpi_P2;
        CLK_WRITE(MSPI_CLK_DIV, TempData);
    }
    else//mspi1
    {
        TempData = CLK_READ(MSPI1_CLK_CFG);
        TempData &= ~(MSPI1_CLK_MASK);
        TempData |= clk_buffer[i].u8ClkSpi_P1 << 10;
        CLK_WRITE(MSPI1_CLK_CFG, TempData);

        TempData = CLK_READ(MSPI1_CLK_DIV);
        TempData &= ~(MSPI1_CLK_DIV_MASK);
        TempData |= clk_buffer[i].u8ClkSpi_P2;
        CLK_WRITE(MSPI1_CLK_DIV, TempData);
    }
    TempData = MSPI_READ(MSPI_CLK_CLOCK_OFFSET);
    TempData &= MSPI_CLK_CLOCK_MASK;
    TempData |= clk_buffer[i].u8ClkSpi_DIV << MSPI_CLK_CLOCK_BIT_OFFSET;
    MSPI_WRITE(MSPI_CLK_CLOCK_OFFSET, TempData);

    return E_MSPI_OK;
}




void HAL_MSPI_SetPerFrameSize(MSPI_CH eChannel, MS_BOOL bDirect, MS_U8 u8BufOffset, MS_U8 u8PerFrameSize)
{
    MS_U8 u8Index = 0;
    MS_U16 u16TempBuf = 0;
    MS_U8 u8BitOffset = 0;
    MS_U16 u16regIndex = 0;

    mutex_lock(&hal_mspi_lock);
    _HAL_MSPI_CheckandSetBaseAddr( eChannel);
    if(bDirect == MSPI_READ_INDEX) {
        u16regIndex = MSPI_FRAME_RBIT_OFFSET;
    } else {
        u16regIndex = MSPI_FRAME_WBIT_OFFSET;
    }
    if(u8BufOffset >=4) {
        u8Index++;
        u8BufOffset -= 4;
    }
    u8BitOffset = u8BufOffset * MSPI_FRAME_BIT_FIELD;
    u16TempBuf = MSPI_READ(u16regIndex+ u8Index);
    u16TempBuf &= ~(MSPI_FRAME_BIT_MASK << u8BitOffset);
    u16TempBuf |= u8PerFrameSize << u8BitOffset;
    MSPI_WRITE((u16regIndex + u8Index), u16TempBuf);
    mutex_unlock(&hal_mspi_lock);
}

MS_BOOL HAL_MSPI_CLOCK_Config(MSPI_CH eChannel, MS_U32 u32MaxClock)
{
    MS_U16 u16ClkDiv = 0;
    MS_U16 u16ClkLevel = 0;
    MS_U16 u16TempDiv = 0;
    MS_U32 u32Clock = 0;
    MS_U32 u32TempClock = 0;
    MS_U32 u32ClockMatch = 0;
    MS_U8  u8ClkIdx = 0;
    MS_U8  u8DivIdx = 0;
    u32MaxClock = u32MaxClock* 1000000;
    mutex_lock(&hal_mspi_lock);
    // get current clock
    _HAL_MSPI_CheckandSetBaseAddr( eChannel);
    for(u8ClkIdx = 0; u8ClkIdx < MSPI_MAXCLKLEVEL; u8ClkIdx++) {
        u32Clock = _HAL_MSPI_GetCLK(u8ClkIdx);
        if(u32Clock < u32MaxClock && u32Clock > u32ClockMatch) {
            u32ClockMatch = u32Clock;
            u16ClkLevel = u8ClkIdx;
        } else if(u32Clock >= u32MaxClock ) {
            if(u32Clock == u32MaxClock) {
                break;
            } else {
                for(u8DivIdx = 1; u8DivIdx < MSPI_CLK_CLOCK_MAX; u8DivIdx++) {
                    u32TempClock = u32Clock / (1 << (u8DivIdx + 1));
                    if(u32TempClock <= u32MaxClock) {
                        if(u32TempClock > u32ClockMatch) {
                            u32ClockMatch = u32TempClock;
                            u16ClkDiv = u8DivIdx;
                            u16ClkLevel = u8ClkIdx;
                        }
                        break;
                    }
                }
            }
        }
    }
    //set clock div of mspi
    u16TempDiv = MSPI_READ(MSPI_CLK_CLOCK_OFFSET);
    u16TempDiv &= MSPI_CLK_CLOCK_MASK;
    u16TempDiv |= u16ClkDiv << MSPI_CLK_CLOCK_BIT_OFFSET;
    MSPI_WRITE(MSPI_CLK_CLOCK_OFFSET, u16TempDiv);
    u16ClkLevel = u16ClkLevel << 2;
    CLK_WRITE(MSPI_CLK_CFG, u16ClkLevel);
    mutex_unlock(&hal_mspi_lock);
    return TRUE;
}

