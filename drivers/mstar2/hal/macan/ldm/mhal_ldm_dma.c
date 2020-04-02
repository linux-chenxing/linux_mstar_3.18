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
/// file    mhal_ldm_dma.c
/// @brief  local dimming  DMA Driver
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/types.h>
#include <asm/io.h>
#include <mstar/mstar_chip.h>
#include "mst_devid.h"
#include <linux/slab.h>
#include "mdrv_mstypes.h"
#include "mdrv_types.h"
#include "mhal_ldm_dma.h"
#include "reg_ldm_dma.h"

#if defined(CONFIG_ARM64)
extern ptrdiff_t mstar_pm_base;
#define REG_2BYTE(addr)  (*((volatile U16 *)((mstar_pm_base + (addr )))))
#define BASEREG_ADDR(addr)  ((mstar_pm_base + (addr )))

#else

#define REG_ADDR(addr)  (*((volatile U16 *)((REG_RIU_BASE + (addr )))))
#define REG_2BYTE(addr)  (*((volatile U16 *)((REG_RIU_BASE + (addr )))))
#define BASEREG_ADDR(addr)  (REG_RIU_BASE + (addr ))
#endif

/*
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
        do{ REG_ADDR(_hal_ldma.VirtMspBaseAddr + ((_reg_)<<2)) =(_val_) ; }while(0)
*/
#define LDMA_WriteByte(reg,val) \
        do{ REG_BYTE(_hal_ldma.VirtLdmaBaseAddr + ((reg)<<2)) =(val); }while(0)

#define LDMA_ReadByte(reg)    (REG_BYTE(_hal_ldma.VirtLdmaBaseAddr + ((reg)<<2)))

#define LDMA_Write2Byte(reg,val)\
        do{ REG_2BYTE(_hal_ldma.VirtLdmaBaseAddr + ((reg)<<2)) =(val) ; }while(0)

#define LDMA_Read2Byte(reg)  (REG_2BYTE(_hal_ldma.VirtLdmaBaseAddr + ((reg)<<2)))


static struct mutex     hal_ldma_lock;
static MS_BOOL bInited[2]= {false,false};
static LDMA_BaseAddr_st _hal_ldma = {
    .eCurrentCH = E_LDMA1,
    .VirtLdmaBaseAddr = REG_LDMA1_BASE,
};

static void _HAL_LDMA_CheckAndSetBaseAddr(LDMA_CH_e eChannel)
{
    if(eChannel == _hal_ldma.eCurrentCH) {
        return;
    } else if(eChannel ==  E_LDMA1) {
        _hal_ldma.VirtLdmaBaseAddr = REG_LDMA1_BASE;
    } else if(eChannel ==  E_LDMA2) {
        _hal_ldma.VirtLdmaBaseAddr = REG_LDMA2_BASE;
    } else {
        printk("LDMA Channel is out of range!\n");
        return ;
    }
}

MS_U8  Mhal_LDMA_Init(LDMA_CH_e eCH)
{
    mutex_init(&hal_ldma_lock);
    mutex_lock(&hal_ldma_lock);
    if(bInited[eCH]) {
        printk("LDMA Channel %d has been  initiated \n",eCH);
        mutex_unlock(&hal_ldma_lock);
        return 0;
    } else {
        _HAL_LDMA_CheckAndSetBaseAddr(eCH );
        bInited[eCH] = true;
    }
    mutex_unlock(&hal_ldma_lock);
    return 0;
}

MS_U8   Mhal_LDMA_SetSPITriggerMode(LDMA_CH_e eCH, LDMA_SPITriggerMode_e eTriggerMode)
{
    mutex_lock(&hal_ldma_lock);

    _HAL_LDMA_CheckAndSetBaseAddr(eCH );
    switch(eTriggerMode) {
    case E_LDMA_SPI_TRIGGER_STOP:
        // Menuload trigger src = one shot

        LDMA_Write2Byte(REG_MENULOAD_CTRL_MODE, LDMA_Read2Byte(REG_MENULOAD_CTRL_MODE)&(~REG_MENULOAD_CTRL_MODE_MSK));
        // menuload abort one shot
        LDMA_Write2Byte(REG_MENULOAD_ABORT, LDMA_Read2Byte(REG_MENULOAD_ABORT)|BIT0);
        // menuload disable one shot
        LDMA_Write2Byte(REG_MENULOAD_ENABLE, LDMA_Read2Byte(REG_MENULOAD_ENABLE)&(~BIT0));
        break;

    case E_LDMA_SPI_TRIGGER_ONE_SHOT:
        // Menuload trigger src = one shot
        LDMA_Write2Byte(REG_MENULOAD_CTRL_MODE, LDMA_Read2Byte(REG_MENULOAD_CTRL_MODE)&(~REG_MENULOAD_CTRL_MODE_MSK));
        // menuload enable one shot
        LDMA_Write2Byte(REG_MENULOAD_ENABLE, LDMA_Read2Byte(REG_MENULOAD_ENABLE)|(BIT0));
        // menuload realtime trigger one shot
        LDMA_Write2Byte(REG_MENULOAD_REALTIME_TRIG, LDMA_Read2Byte(REG_MENULOAD_REALTIME_TRIG)|(BIT0));

        break;

    case E_LDMA_SPI_TRIGGER_EVERY_VSYNC:
        // Menuload trigger src = V sync
        LDMA_Write2Byte(REG_MENULOAD_CTRL_MODE, LDMA_Read2Byte(REG_MENULOAD_CTRL_MODE)|BIT0);
        LDMA_Write2Byte(REG_USE_HW_LOOP_MD, LDMA_Read2Byte(REG_USE_HW_LOOP_MD)|BIT7);

        break;
    }

    mutex_unlock(&hal_ldma_lock);
    return 0;
}

MS_U8 Mhal_LDMA_SetMenuloadNumber(LDMA_CH_e eCH, MS_U32 u32MenuldNum)
{

    mutex_lock(&hal_ldma_lock);
    _HAL_LDMA_CheckAndSetBaseAddr(eCH );
    LDMA_Write2Byte(REG_MENULOAD_NUMBER, u32MenuldNum);
    mutex_unlock(&hal_ldma_lock);

    return 0;
}

MS_U8 Mhal_LDMA_SetSPICommandFormat(LDMA_CH_e eCH,MS_U8 u8CmdLen, MS_U16* pu16CmdBuf )
{

    if( u8CmdLen > LDMA_MAX_SPI_CMD_NUM ) {
        printk("[LDM-DMA]Spi command len is too long=%u!\n", u8CmdLen);
        u8CmdLen = LDMA_MAX_SPI_CMD_NUM;
    }

    if(!pu16CmdBuf) {
        printk("NULL Pointer,please check pu16CmdBuf!\n ");
    }

    mutex_lock(&hal_ldma_lock);
    _HAL_LDMA_CheckAndSetBaseAddr(eCH );
    switch (u8CmdLen) {
    case 10: {
        LDMA_Write2Byte(REG_COMMAND8, pu16CmdBuf[9]);
        LDMA_Write2Byte(REG_CMD_LENGTH, LDMA_Read2Byte(REG_CMD_LENGTH)|BIT9);
    }
    case 9: {
        LDMA_Write2Byte(REG_COMMAND7, pu16CmdBuf[8]);
        LDMA_Write2Byte(REG_CMD_LENGTH, LDMA_Read2Byte(REG_CMD_LENGTH)|BIT8);
    }
    case 8: {
        LDMA_Write2Byte(REG_COMMAND6, pu16CmdBuf[7]);
        LDMA_Write2Byte(REG_CMD_LENGTH, LDMA_Read2Byte(REG_CMD_LENGTH)|BIT7);
    }
    case 7: {
        LDMA_Write2Byte(REG_COMMAND5, pu16CmdBuf[6]);
        LDMA_Write2Byte(REG_CMD_LENGTH, LDMA_Read2Byte(REG_CMD_LENGTH)|BIT6);
    }
    case 6: {
        LDMA_Write2Byte(REG_COMMAND4, pu16CmdBuf[5]);
        LDMA_Write2Byte(REG_CMD_LENGTH, LDMA_Read2Byte(REG_CMD_LENGTH)|BIT5);
    }
    case 5: {
        LDMA_Write2Byte(REG_COMMAND3, pu16CmdBuf[4]);
        LDMA_Write2Byte(REG_CMD_LENGTH, LDMA_Read2Byte(REG_CMD_LENGTH)|BIT4);
    }
    case 4:  {
        LDMA_Write2Byte(REG_COMMAND2, pu16CmdBuf[3]);
        LDMA_Write2Byte(REG_CMD_LENGTH, LDMA_Read2Byte(REG_CMD_LENGTH)|BIT3);
    }
    case 3: {
        LDMA_Write2Byte(REG_COMMAND1, pu16CmdBuf[2]);
        LDMA_Write2Byte(REG_CMD_LENGTH, LDMA_Read2Byte(REG_CMD_LENGTH)|BIT2);
    }
    case 2: {
        LDMA_Write2Byte(REG_COMMAND, pu16CmdBuf[1]);
        LDMA_Write2Byte(REG_CMD_LENGTH, LDMA_Read2Byte(REG_CMD_LENGTH)|BIT1);
    }
    case 1: {
        LDMA_Write2Byte(REG_START_CODE, pu16CmdBuf[0]);
        LDMA_Write2Byte(REG_CMD_LENGTH, LDMA_Read2Byte(REG_CMD_LENGTH)|BIT0);
        break;
    }
    default: {
        printk("Oops, u8CmdLen should  be bigger than 0\n");
    }

    }
    mutex_unlock(&hal_ldma_lock);

    return 0;
}

MS_U8 Mhal_LDMA_SetCheckSumMode(LDMA_CH_e eCH,LDMA_CheckSumMode_e eSumMode)
{
    mutex_lock(&hal_ldma_lock);
    _HAL_LDMA_CheckAndSetBaseAddr(eCH );
    LDMA_Write2Byte(REG_CMD_LENGTH, LDMA_Read2Byte(REG_CMD_LENGTH)|BIT15);
    switch (eSumMode) {
    case E_LDMA_NOCHECKSUM: {
        LDMA_Write2Byte(REG_CMD_LENGTH, LDMA_Read2Byte(REG_CMD_LENGTH)&(~BIT15));
        break;
    }
    case E_LDMA_CHECKSUM_ALL: {
        LDMA_Write2Byte(REG_CHKSUM_MD, LDMA_Read2Byte(REG_CHKSUM_MD)&(~REG_CHKSUM_MD_MSK));
        break;
    }
    case E_LDMA_CHECKSUM_CMD_DATA: {
        LDMA_Write2Byte(REG_CHKSUM_MD, BIT0);
        break;
    }
    case E_LDMA_CHECKSUM_DATA_ONLY: {
        LDMA_Write2Byte(REG_CHKSUM_MD,BIT1);
        break;
    }
    default: {
        printk("Oops, No Such CheckSum Mode. \n");
    }
    }

    mutex_unlock(&hal_ldma_lock);

    return 0;

}


MS_U8 Mhal_LDMA_SetTrigDelay(LDMA_CH_e eCH, LDMA_TrigDelay_st  *pstTrigDelay)
{
    mutex_lock(&hal_ldma_lock);
    _HAL_LDMA_CheckAndSetBaseAddr(eCH );
    LDMA_Write2Byte(REG_TRIG_DELAY0_CNT0,pstTrigDelay->u16Delay0Cnt0);
    LDMA_Write2Byte(REG_TRIG_DELAY0_CNT1,pstTrigDelay->u16Delay0Cnt1);
    LDMA_Write2Byte(REG_TRIG_DELAY1_CNT0,pstTrigDelay->u16Delay1Cnt0);
    LDMA_Write2Byte(REG_TRIG_DELAY1_CNT1,pstTrigDelay->u16Delay1Cnt1);
    mutex_unlock(&hal_ldma_lock);
    return 0;
}

MS_U8 Mhal_LDMA_EnableCS(LDMA_CH_e eCH, MS_BOOL bEnable)
{
    mutex_lock(&hal_ldma_lock);
    _HAL_LDMA_CheckAndSetBaseAddr(eCH );
    LDMA_Write2Byte(REG_TRIG_DELAY_CTRL, bEnable&BIT0);
    mutex_unlock(&hal_ldma_lock);
    return 0;
}

MS_U8 Mhal_LDMA_GetStatus(LDMA_CH_e eCH,LDMA_GetStatus_e index)
{
    mutex_lock(&hal_ldma_lock);
    _HAL_LDMA_CheckAndSetBaseAddr(eCH );
    mutex_unlock(&hal_ldma_lock);

    return 0;
}

MS_U8 Mhal_LDMA_SetSPI16BitMode(LDMA_CH_e eCH, MS_BOOL bEnable)
{
    mutex_lock(&hal_ldma_lock);
    _HAL_LDMA_CheckAndSetBaseAddr(eCH );
    LDMA_Write2Byte(REG_SPI_16BIT_MD, LDMA_Read2Byte(REG_SPI_16BIT_MD)&(~REG_SPI_16BIT_MD_MSK));
    mutex_unlock(&hal_ldma_lock);
    return 0;
}

