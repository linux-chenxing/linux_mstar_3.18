////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2007 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (¡§MStar Confidential Information¡¨) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
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
#include <linux/cdev.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <asm/io.h>

#ifndef _HAL_IIC0_C_
#define _HAL_IIC0_C_

#include "mhal_iic0.h"
#include "mhal_iic0_reg.h"

#define LOWBYTE(w)                ((w) & 0x00ff)
#define HIBYTE(w)                (((w) >> 8) & 0x00ff)

////////////////////////////////////////////////////////////////////////////////
// Define & data type
///////////////////////////////////////////////////////////////////////////////
#define HWI2C_HAL_RETRY_TIMES     (3)
#define HWI2C_HAL_WAIT_TIMEOUT    30000//(1500)
#define HWI2C_HAL_FUNC()              //{printk("=============%s\n",  __FUNCTION__);}
#define HWI2C_HAL_INFO(x, args...)    //{printk(x, ##args);}
#define HWI2C_HAL_ERR(x, args...)     {printk(x, ##args);}
#ifndef UNUSED
#define UNUSED(x) ((x)=(x))
#endif

#define HWI2C_DMA_CMD_DATA_LEN      7
#define HWI2C_DMA_WAIT_TIMEOUT      (30000)
#define HWI2C_DMA_WRITE             0
#define HWI2C_DMA_READ              0

#define _PA2VA(x) (U32)MsOS0_PA2KSEG1((x))
#define _VA2PA(x) (U32)MsOS0_VA2PA((x))

////////////////////////////////////////////////////////////////////////////////
// Local variable
///////////////////////////////////////////////////////////////////////////////
static U32 _gMIO_MapBase = 0;
static BOOL g_bLastByte[HAL_HWI2C_PORTS];
static U32 g_u32DmaPhyAddr[HAL_HWI2C_PORTS];
static HAL_HWI2C_PortCfg g_stPortCfg[HAL_HWI2C_PORTS];
static U16 g_u16DmaDelayFactor[HAL_HWI2C_PORTS];

U32 MsOS0_PA2KSEG1(U32 addr)
{
	return ((U32)(((U32)addr) | 0xa0000000));
}
U32 MsOS0_VA2PA(U32 addr)
{
	return ((U32)(((U32)addr) & 0x1fffffff));
}

void HAL_HWI2C0_ExtraDelay(U32 u32Us)
{
    // volatile is necessary to avoid optimization
    U32 volatile u32Dummy = 0;
    //U32 u32Loop;
    U32 volatile u32Loop;

    u32Loop = (U32)(50 * u32Us);
    while (u32Loop--)
    {
        u32Dummy++;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_HWI2C0_SetIOMapBase
/// @brief \b Function  \b Description: Dump bdma all register
/// @param <IN>         \b None :
/// @param <OUT>        \b None :
/// @param <RET>        \b None :
////////////////////////////////////////////////////////////////////////////////
void HAL_HWI2C0_SetIOMapBase(U32 u32Base)
{
    HWI2C_HAL_FUNC();

    _gMIO_MapBase = u32Base;
    HWI2C_HAL_INFO("HWI2C IOMap base:%16lx Reg offset:%4x\n", u32Base, (U16)HWI2C0_REG_BASE);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_HWI2C0_ReadByte
/// @brief \b Function  \b Description: read 1 Byte data
/// @param <IN>         \b u32RegAddr: register address
/// @param <OUT>        \b None :
/// @param <RET>        \b U8
////////////////////////////////////////////////////////////////////////////////
U8 HAL_HWI2C0_ReadByte(U32 u32RegAddr)
{
	U16 u16value;

	HWI2C_HAL_FUNC();
	HWI2C_HAL_INFO("HWI2C IOMap base:%16lx u32RegAddr:%8lx u32RegAddr:%16lx\n", _gMIO_MapBase, u32RegAddr,(_gMIO_MapBase+((u32RegAddr & 0xFFFFFF00ul) << 1) + (((u32RegAddr & 0xFF)/ 2) << 2)));
    //return ((volatile U8*)(_gMIO_MapBase))[(u32RegAddr << 1) - (u32RegAddr & 1)];

    u16value = (*(volatile U32*)(_gMIO_MapBase+((u32RegAddr & 0xFFFFFF00ul) << 1) + (((u32RegAddr & 0xFF)/ 2) << 2)));
	HWI2C_HAL_INFO("u16value===:%4x\n", u16value);
	return ((u32RegAddr & 0xFF) % 2)? HIBYTE(u16value) : LOWBYTE(u16value);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_HWI2C0_Read4Byte
/// @brief \b Function  \b Description: read 2 Byte data
/// @param <IN>         \b u32RegAddr: register address
/// @param <OUT>        \b None :
/// @param <RET>        \b U16
////////////////////////////////////////////////////////////////////////////////
U16 HAL_HWI2C0_Read2Byte(U32 u32RegAddr)
{
	HWI2C_HAL_FUNC();
	HWI2C_HAL_INFO("HWI2C IOMap base:%16lx u32RegAddr:%8lx\n", _gMIO_MapBase, u32RegAddr);
    //return ((volatile U16*)(_gMIO_MapBase))[u32RegAddr];
    return (*(volatile U32*)(_gMIO_MapBase+((u32RegAddr & 0xFFFFFF00ul) << 1) + (((u32RegAddr & 0xFF)/ 2) << 2)));
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_HWI2C0_Read4Byte
/// @brief \b Function  \b Description: read 4 Byte data
/// @param <IN>         \b u32RegAddr: register address
/// @param <OUT>        \b None :
/// @param <RET>        \b U32
////////////////////////////////////////////////////////////////////////////////
U32 HAL_HWI2C0_Read4Byte(U32 u32RegAddr)
{
	HWI2C_HAL_FUNC();
	HWI2C_HAL_INFO("HWI2C IOMap base:%16lx u32RegAddr:%8lx\n", _gMIO_MapBase, u32RegAddr);

    return (HAL_HWI2C0_Read2Byte(u32RegAddr) | HAL_HWI2C0_Read2Byte(u32RegAddr+2) << 16);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_HWI2C0_WriteByte
/// @brief \b Function  \b Description: write 1 Byte data
/// @param <IN>         \b u32RegAddr: register address
/// @param <IN>         \b u8Val : 1 byte data
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE : ok, FALSE : fail
////////////////////////////////////////////////////////////////////////////////
BOOL HAL_HWI2C0_WriteByte(U32 u32RegAddr, U8 u8Val)
{
	U16 u16value;

    if (!u32RegAddr)
    {
        HWI2C_HAL_ERR("%s reg error!\n", __FUNCTION__);
        return FALSE;
    }
	HWI2C_HAL_FUNC();
	HWI2C_HAL_INFO("HWI2C IOMap base:%16lx u32RegAddr:%8lx\n", _gMIO_MapBase, u32RegAddr);

    //((volatile U8*)(_gMIO_MapBase))[(u32RegAddr << 1) - (u32RegAddr & 1)] = u8Val;
	if((u32RegAddr & 0xFF) % 2)
	{
		u16value = (((U16)u8Val) << 8)|((*(volatile U32*)(_gMIO_MapBase+((u32RegAddr & 0xFFFFFF00ul) << 1) + (((u32RegAddr & 0xFF)/ 2) << 2))) & 0xFF);
	}
	else
	{
		u16value = ((U16)u8Val)|((*(volatile U32*)(_gMIO_MapBase+((u32RegAddr & 0xFFFFFF00ul) << 1) + (((u32RegAddr & 0xFF)/ 2) << 2))) & 0xFF00);
	}

	(*(volatile U32*)(_gMIO_MapBase+((u32RegAddr & 0xFFFFFF00ul) << 1) + (((u32RegAddr & 0xFF)/ 2) << 2))) = u16value;
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_HWI2C0_Write2Byte
/// @brief \b Function  \b Description: write 2 Byte data
/// @param <IN>         \b u32RegAddr: register address
/// @param <IN>         \b u16Val : 2 byte data
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE : ok, FALSE : fail
////////////////////////////////////////////////////////////////////////////////
BOOL HAL_HWI2C0_Write2Byte(U32 u32RegAddr, U16 u16Val)
{
    if (!u32RegAddr)
    {
        HWI2C_HAL_ERR("%s reg error!\n", __FUNCTION__);
        return FALSE;
    }
	HWI2C_HAL_FUNC();
	HWI2C_HAL_INFO("HWI2C IOMap base:%16lx u32RegAddr:%4x\n", _gMIO_MapBase, u16Val);
	HWI2C_HAL_INFO("HWI2C IOMap base:%16lx u32RegAddr:%8lx u32RegAddr:%16lx\n", _gMIO_MapBase, u32RegAddr,(_gMIO_MapBase+((u32RegAddr & 0xFFFFFF00ul) << 1) + (((u32RegAddr & 0xFF)/ 2) << 2)));

    //((volatile U16*)(_gMIO_MapBase))[u32RegAddr] = u16Val;
    //HAL_HWI2C0_WriteByte(u32RegAddr,LOWBYTE(u16Val));
    //HAL_HWI2C0_WriteByte(u32RegAddr+1,HIBYTE(u16Val));
    (*(volatile U32*)(_gMIO_MapBase+((u32RegAddr & 0xFFFFFF00ul) << 1) + (((u32RegAddr & 0xFF)/ 2) << 2))) = u16Val;
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_HWI2C0_Write4Byte
/// @brief \b Function  \b Description: write 4 Byte data
/// @param <IN>         \b u32RegAddr: register address
/// @param <IN>         \b u32Val : 4 byte data
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE : ok, FALSE : fail
////////////////////////////////////////////////////////////////////////////////
BOOL HAL_HWI2C0_Write4Byte(U32 u32RegAddr, U32 u32Val)
{
    if (!u32RegAddr)
    {
        HWI2C_HAL_ERR("%s reg error!\n", __FUNCTION__);
        return FALSE;
    }

	HWI2C_HAL_FUNC();
	HWI2C_HAL_INFO("HWI2C IOMap base:%16lx u32RegAddr:%8lx\n", _gMIO_MapBase, u32RegAddr);

    HAL_HWI2C0_Write2Byte(u32RegAddr, u32Val & 0x0000FFFF);
    HAL_HWI2C0_Write2Byte(u32RegAddr+2, u32Val >> 16);
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_HWI2C0_WriteRegBit
/// @brief \b Function  \b Description: write 1 Byte data
/// @param <IN>         \b u32RegAddr: register address
/// @param <IN>         \b u8Val : 1 byte data
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE : ok, FALSE : fail
////////////////////////////////////////////////////////////////////////////////
BOOL HAL_HWI2C0_WriteRegBit(U32 u32RegAddr, U8 u8Mask, BOOL bEnable)
{
    U8 u8Val = 0;

    if (!u32RegAddr)
    {
        HWI2C_HAL_ERR("%s reg error!\n", __FUNCTION__);
        return FALSE;
    }
	HWI2C_HAL_FUNC();
	HWI2C_HAL_INFO("HWI2C IOMap base:%16lx u32RegAddr:%8lx u8Mask: %x bEnable: %x\n", _gMIO_MapBase, u32RegAddr, u8Mask, bEnable);

    u8Val = HAL_HWI2C0_ReadByte(u32RegAddr);
    u8Val = (bEnable) ? (u8Val | u8Mask) : (u8Val & ~u8Mask);
    HAL_HWI2C0_WriteByte(u32RegAddr, u8Val);
	HWI2C_HAL_INFO("read back u32RegAddr:%x\n", HAL_HWI2C0_ReadByte(u32RegAddr));
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_HWI2C0_WriteByteMask
/// @brief \b Function  \b Description: write data with mask bits
/// @param <IN>         \b u32RegAddr: register address
/// @param <IN>         \b u8Val : 1 byte data
/// @param <IN>         \b u8Mask : mask bits
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE : ok, FALSE : fail
////////////////////////////////////////////////////////////////////////////////
BOOL HAL_HWI2C0_WriteByteMask(U32 u32RegAddr, U8 u8Val, U8 u8Mask)
{
    if (!u32RegAddr)
    {
        HWI2C_HAL_ERR("%s reg error!\n", __FUNCTION__);
        return FALSE;
    }
	HWI2C_HAL_FUNC();
	HWI2C_HAL_INFO("HWI2C IOMap base:%16lx u32RegAddr:%8lx\n", _gMIO_MapBase, u32RegAddr);

    u8Val = (HAL_HWI2C0_ReadByte(u32RegAddr) & ~u8Mask) | (u8Val & u8Mask);
    HAL_HWI2C0_WriteByte(u32RegAddr, u8Val);
    return TRUE;
}

//#####################
//
//  MIIC STD Related Functions
//  Static or Internal use
//
//#####################
////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_HWI2C0_EnINT
/// @brief \b Function  \b Description: Enable Interrupt
/// @param <IN>         \b bEnable : TRUE: Enable, FALSE: Disable
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE: Ok, FALSE: Fail
////////////////////////////////////////////////////////////////////////////////
static BOOL HAL_HWI2C0_EnINT(U16 u16PortOffset, BOOL bEnable)
{
    HWI2C_HAL_FUNC();
    return HAL_HWI2C0_WriteRegBit(REG_HWI2C0_MIIC_CFG+u16PortOffset, _MIIC_CFG_EN_INT, bEnable);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_HWI2C0_EnDMA
/// @brief \b Function  \b Description: Enable DMA
/// @param <IN>         \b bEnable : TRUE: Enable, FALSE: Disable
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE: Ok, FALSE: Fail
////////////////////////////////////////////////////////////////////////////////
static BOOL HAL_HWI2C0_EnDMA(U16 u16PortOffset, BOOL bEnable)
{
    HWI2C_HAL_FUNC();
    return HAL_HWI2C0_WriteRegBit(REG_HWI2C0_MIIC_CFG+u16PortOffset, _MIIC_CFG_EN_DMA, bEnable);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_HWI2C0_EnClkStretch
/// @brief \b Function  \b Description: Enable Clock Stretch
/// @param <IN>         \b bEnable : TRUE: Enable, FALSE: Disable
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE: Ok, FALSE: Fail
////////////////////////////////////////////////////////////////////////////////
static BOOL HAL_HWI2C0_EnClkStretch(U16 u16PortOffset, BOOL bEnable)
{
    HWI2C_HAL_FUNC();
    return HAL_HWI2C0_WriteRegBit(REG_HWI2C0_MIIC_CFG+u16PortOffset, _MIIC_CFG_EN_CLKSTR, bEnable);
}


////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_HWI2C0_EnFilter
/// @brief \b Function  \b Description: Enable Filter
/// @param <IN>         \b bEnable : TRUE: Enable, FALSE: Disable
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE: Ok, FALSE: Fail
////////////////////////////////////////////////////////////////////////////////
static BOOL HAL_HWI2C0_EnFilter(U16 u16PortOffset, BOOL bEnable)
{
    HWI2C_HAL_FUNC();
    return HAL_HWI2C0_WriteRegBit(REG_HWI2C0_MIIC_CFG+u16PortOffset, _MIIC_CFG_EN_FILTER, bEnable);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_HWI2C0_EnPushSda
/// @brief \b Function  \b Description: Enable push current for SDA
/// @param <IN>         \b bEnable : TRUE: Enable, FALSE: Disable
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE: Ok, FALSE: Fail
////////////////////////////////////////////////////////////////////////////////
static BOOL HAL_HWI2C0_EnPushSda(U16 u16PortOffset, BOOL bEnable)
{
    HWI2C_HAL_FUNC();
    return HAL_HWI2C0_WriteRegBit(REG_HWI2C0_MIIC_CFG+u16PortOffset, _MIIC_CFG_EN_PUSH1T, bEnable);
}

//#####################
//
//  MIIC STD Related Functions
//  External
//
//#####################
////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_HWI2C0_Init_Chip
/// @brief \b Function  \b Description: Init HWI2C chip
/// @param <IN>         \b None :
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE : ok, FALSE : fail
////////////////////////////////////////////////////////////////////////////////
BOOL HAL_HWI2C0_Init_Chip(void)
{
    BOOL bRet = TRUE;

    HWI2C_HAL_FUNC();
    //not set all pads (except SPI) as input
    bRet &= HAL_HWI2C0_WriteRegBit(CHIP_REG_ALLPADIN, CHIP_ALLPAD_IN, FALSE);
	bRet &= HAL_HWI2C0_WriteByteMask(REG_HWI2C_MIIC0_VER_SEL, REG_HWI2C_MIIC0_VER_V3, REG_HWI2C_MIIC0_VER_MSK);
    return bRet;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_HWI2C0_IsMaster
/// @brief \b Function  \b Description: Check if Master I2C
/// @param <IN>         \b None :
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE: Master, FALSE: Slave
////////////////////////////////////////////////////////////////////////////////
BOOL HAL_HWI2C0_IsMaster(void)
{
    HWI2C_HAL_FUNC();
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_HWI2C0_Master_Enable
/// @brief \b Function  \b Description: Master I2C enable
/// @param <IN>         \b None :
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE : ok, FALSE : fail
////////////////////////////////////////////////////////////////////////////////
BOOL HAL_HWI2C0_Master_Enable(U16 u16PortOffset)
{
    U8 u8Port;
    BOOL bRet = TRUE;

    HWI2C_HAL_FUNC();

    if(HAL_HWI2C0_GetPortIdxByOffset(u16PortOffset,&u8Port)==FALSE)
        return FALSE;
    g_bLastByte[u8Port] = FALSE;

    //(1) clear interrupt
    HAL_HWI2C0_Clear_INT(u16PortOffset);
    //(2) reset standard master iic
    HAL_HWI2C0_Reset(u16PortOffset,TRUE);
    HAL_HWI2C0_Reset(u16PortOffset,FALSE);
    //(3) configuration
    HAL_HWI2C0_EnINT(u16PortOffset,TRUE);
    HAL_HWI2C0_EnClkStretch(u16PortOffset,TRUE);
    HAL_HWI2C0_EnFilter(u16PortOffset,TRUE);
    HAL_HWI2C0_EnPushSda(u16PortOffset,TRUE);

    return bRet;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_HWI2C0_SetPortRegOffset
/// @brief \b Function  \b Description: Set HWI2C port register offset
/// @param <IN>         \b ePort : HWI2C port number
/// @param <OUT>         \b pu16Offset : port register offset
/// @param <RET>        \b TRUE : ok, FALSE : fail
////////////////////////////////////////////////////////////////////////////////
BOOL HAL_HWI2C0_SetPortRegOffset(HAL_HWI2C_PORT ePort, U16* pu16Offset)
{
    HWI2C_HAL_FUNC();

    if((ePort>=E_HAL_HWI2C_PORT0_0)&&(ePort<=E_HAL_HWI2C_PORT0_1))
    {//port 0 : bank register address 0x111800
        *pu16Offset = (U16)0x00;
    }
    else if((ePort>=E_HAL_HWI2C_PORT1_0)&&(ePort<=E_HAL_HWI2C_PORT1_1))
    {//port 1 : bank register address 0x111900
        *pu16Offset = (U16)0x100;
    }
    else if((ePort>=E_HAL_HWI2C_PORT2_0)&&(ePort<=E_HAL_HWI2C_PORT2_1))
    {//port 2 : bank register address 0x111A00
        *pu16Offset = (U16)0x200;
    }
    else if((ePort>=E_HAL_HWI2C_PORT3_0)&&(ePort<=E_HAL_HWI2C_PORT3_1))
    {//port 2 : bank register address 0x111B00
        *pu16Offset = (U16)0x300;
    }
    else
    {
        *pu16Offset = (U16)0x00;
        return FALSE;
    }
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_HWI2C0_GetPortIdxByRegOffset
/// @brief \b Function  \b Description: Get HWI2C port index by register offset
/// @param <IN>         \b u16Offset : port register offset
/// @param <OUT>         \b pu8Port :  port index
/// @param <RET>        \b TRUE : ok, FALSE : fail
////////////////////////////////////////////////////////////////////////////////
BOOL HAL_HWI2C0_GetPortIdxByOffset(U16 u16Offset, U8* pu8Port)
{
    HWI2C_HAL_FUNC();

    if(u16Offset==(U16)0x00)
    {//port 0 : bank register address 0x11800
        *pu8Port = HAL_HWI2C_PORT0;
    }
    else if(u16Offset==(U16)0x100)
    {//port 1 : bank register address 0x11900
        *pu8Port = HAL_HWI2C_PORT1;
    }
    else if(u16Offset==(U16)0x200)
    {//port 2 : bank register address 0x11A00
        *pu8Port = HAL_HWI2C_PORT2;
    }
    else if(u16Offset==(U16)0x300)
    {//port 3 : bank register address 0x11B00
        *pu8Port = HAL_HWI2C_PORT3;
    }
    else
    {
        *pu8Port = HAL_HWI2C_PORT0;
        return FALSE;
    }
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_HWI2C0_GetPortIdxByPort
/// @brief \b Function  \b Description: Get HWI2C port index by port number
/// @param <IN>         \b ePort : port number
/// @param <OUT>         \b pu8Port :  port index
/// @param <RET>        \b TRUE : ok, FALSE : fail
////////////////////////////////////////////////////////////////////////////////
BOOL HAL_HWI2C0_GetPortIdxByPort(HAL_HWI2C_PORT ePort, U8* pu8Port)
{
    HWI2C_HAL_FUNC();

    if((ePort>=E_HAL_HWI2C_PORT0_0)&&(ePort<=E_HAL_HWI2C_PORT0_1))
    {
        *pu8Port = HAL_HWI2C_PORT0;
    }
    else if((ePort>=E_HAL_HWI2C_PORT1_0)&&(ePort<=E_HAL_HWI2C_PORT1_1))
    {
        *pu8Port = HAL_HWI2C_PORT1;
    }
    else if((ePort>=E_HAL_HWI2C_PORT2_0)&&(ePort<=E_HAL_HWI2C_PORT2_1))
    {
        *pu8Port = HAL_HWI2C_PORT2;
    }
    else if((ePort>=E_HAL_HWI2C_PORT3_0)&&(ePort<=E_HAL_HWI2C_PORT3_1))
    {
        *pu8Port = HAL_HWI2C_PORT3;
    }
    else
    {
        *pu8Port = HAL_HWI2C_PORT0;
        return FALSE;
    }
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_HWI2C0_SelectPort
/// @brief \b Function  \b Description: Select HWI2C port
/// @param <IN>         \b None : HWI2C port
/// @param param        \b None :
/// @param <RET>        \b TRUE : ok, FALSE : fail
////////////////////////////////////////////////////////////////////////////////
BOOL HAL_HWI2C0_SelectPort(HAL_HWI2C_PORT ePort)
{
    U8 u8Value1=0;

    HWI2C_HAL_FUNC();

    //decide port mask
    if((ePort>=E_HAL_HWI2C_PORT0_0)&&(ePort<=E_HAL_HWI2C_PORT0_7))//port 0
    {
        switch(ePort)
        {
            case E_HAL_HWI2C_PORT0_0:
                u8Value1 = CHIP_MIIC0_PAD_1;
                break;
            case E_HAL_HWI2C_PORT0_1:
                u8Value1 = CHIP_MIIC0_PAD_2;
                break;
            default:
                return FALSE;
        }
		HAL_HWI2C0_WriteByteMask(CHIP_REG_HWI2C_MIIC0_CLK, CHIP_REG_HWI2C_MIIC0_CLK_XTAL, CHIP_REG_HWI2C_MIIC0_CLK_MSK);
        HAL_HWI2C0_WriteByteMask(CHIP_REG_HWI2C_MIIC0, u8Value1, CHIP_MIIC0_PAD_MSK);
    }
    else if((ePort>=E_HAL_HWI2C_PORT1_0)&&(ePort<=E_HAL_HWI2C_PORT1_7))//port 1
    {
        switch(ePort)
        {
            case E_HAL_HWI2C_PORT1_0: //miic1 disable
                u8Value1 = CHIP_MIIC1_PAD_0;
                break;
            case E_HAL_HWI2C_PORT1_1: //miic1 using PAD_TGPIO2/PAD_TGPIO3
                u8Value1 = CHIP_MIIC1_PAD_1;
                break;
            default:
                return FALSE;
        }
        HAL_HWI2C0_WriteByteMask(CHIP_REG_HWI2C_MIIC1, u8Value1, CHIP_MIIC1_PAD_MSK);
    }
    else if((ePort>=E_HAL_HWI2C_PORT2_0)&&(ePort<=E_HAL_HWI2C_PORT2_7))//port 2
    {
        switch(ePort)
        {
            case E_HAL_HWI2C_PORT2_0: //miic2 disable
                u8Value1 = CHIP_MIIC2_PAD_0;
                break;
            case E_HAL_HWI2C_PORT2_1: //miic2 using PAD_I2S_IN_BCK/PAD_I2S_IN_SD
                u8Value1 = CHIP_MIIC2_PAD_1;
                break;
            default:
                return FALSE;
        }
        HAL_HWI2C0_WriteByteMask(CHIP_REG_HWI2C_MIIC2, u8Value1, CHIP_MIIC2_PAD_MSK);
    }
    else if((ePort>=E_HAL_HWI2C_PORT3_0)&&(ePort<=E_HAL_HWI2C_PORT3_1))//port 3
    {
        switch(ePort)
        {
            case E_HAL_HWI2C_PORT3_0: //ddcr_miic disable
                u8Value1 = CHIP_DDCR_PAD_0;
                break;
            case E_HAL_HWI2C_PORT3_1: //ddcr_miic using PAD_DDCR_CK/PAD_DDCR_DA
                u8Value1 = CHIP_DDCR_PAD_1;
                break;
            default:
                return FALSE;
        }
        HAL_HWI2C0_WriteByteMask(CHIP_REG_HWI2C_DDCR, u8Value1, CHIP_DDCR_PAD_MSK);
    }
    else
    {
        return FALSE;
    }
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_HWI2C0_SetClk
/// @brief \b Function  \b Description: Set I2C clock
/// @param <IN>         \b u8Clk: clock rate
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE : ok, FALSE : fail
////////////////////////////////////////////////////////////////////////////////
BOOL HAL_HWI2C0_SetClk(U16 u16PortOffset, HAL_HWI2C_CLKSEL eClkSel)
{
    U16 u16ClkHCnt=0,u16ClkLCnt=0;
    U16 u16StpCnt=0,u16SdaCnt=0,u16SttCnt=0,u16LchCnt=0;

    HWI2C_HAL_FUNC();

    if(eClkSel>=E_HAL_HWI2C_CLKSEL_NOSUP)
        return FALSE;

    switch(eClkSel)//use Xtal = 12M Hz
    {
        case E_HAL_HWI2C_CLKSEL_HIGH: // 400 KHz
            u16ClkHCnt =   9; u16ClkLCnt =   13; break;
        case E_HAL_HWI2C_CLKSEL_NORMAL: //300 KHz
            u16ClkHCnt =  15; u16ClkLCnt =   17; break;
        case E_HAL_HWI2C_CLKSEL_SLOW: //200 KHz
            u16ClkHCnt =  25; u16ClkLCnt =   27; break;
        case E_HAL_HWI2C_CLKSEL_VSLOW: //100 KHz
            u16ClkHCnt =  57; u16ClkLCnt =   57; break;
        case E_HAL_HWI2C_CLKSEL_USLOW: //50 KHz
            u16ClkHCnt =  115; u16ClkLCnt = 117; break;
        case E_HAL_HWI2C_CLKSEL_UVSLOW: //25 KHz
            u16ClkHCnt =  235; u16ClkLCnt = 237; break;
        default:
            u16ClkHCnt =  15; u16ClkLCnt =  17; break;
    }
    u16SttCnt=38; u16StpCnt=38; u16SdaCnt=5; u16LchCnt=5;

    HAL_HWI2C0_Write2Byte(REG_HWI2C0_CKH_CNT+u16PortOffset, u16ClkHCnt);
    HAL_HWI2C0_Write2Byte(REG_HWI2C0_CKL_CNT+u16PortOffset, u16ClkLCnt);
    HAL_HWI2C0_Write2Byte(REG_HWI2C0_STP_CNT+u16PortOffset, u16StpCnt);
    HAL_HWI2C0_Write2Byte(REG_HWI2C0_SDA_CNT+u16PortOffset, u16SdaCnt);
    HAL_HWI2C0_Write2Byte(REG_HWI2C0_STT_CNT+u16PortOffset, u16SttCnt);
    HAL_HWI2C0_Write2Byte(REG_HWI2C0_LTH_CNT+u16PortOffset, u16LchCnt);
    //HAL_HWI2C0_Write2Byte(REG_HWI2C0_TMT_CNT+u16PortOffset, 0x0000);
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_HWI2C0_Start
/// @brief \b Function  \b Description: Send start condition
/// @param <IN>         \b None :
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE : ok, FALSE : fail
////////////////////////////////////////////////////////////////////////////////
BOOL HAL_HWI2C0_Start(U16 u16PortOffset)
{
    U16 u16Count = HWI2C_HAL_WAIT_TIMEOUT;

    HWI2C_HAL_FUNC();
    //reset I2C
    HAL_HWI2C0_WriteRegBit(REG_HWI2C0_CMD_START+u16PortOffset, _CMD_START, TRUE);
    while((!HAL_HWI2C0_Is_INT(u16PortOffset))&&(u16Count > 0))
        u16Count--;
	udelay(5);
    HAL_HWI2C0_Clear_INT(u16PortOffset);
    return (u16Count)? TRUE:FALSE;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_HWI2C0_Stop
/// @brief \b Function  \b Description: Send Stop condition
/// @param <IN>         \b None :
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE : ok, FALSE : fail
////////////////////////////////////////////////////////////////////////////////
BOOL HAL_HWI2C0_Stop(U16 u16PortOffset)
{
    U16 u16Count = HWI2C_HAL_WAIT_TIMEOUT;

    HWI2C_HAL_FUNC();
	udelay(5);
    HAL_HWI2C0_WriteRegBit(REG_HWI2C0_CMD_STOP+u16PortOffset, _CMD_STOP, TRUE);
    while((!HAL_HWI2C0_Is_Idle(u16PortOffset))&&(!HAL_HWI2C0_Is_INT(u16PortOffset))&&(u16Count > 0))
        u16Count--;
    HAL_HWI2C0_Clear_INT(u16PortOffset);
    return (u16Count)? TRUE:FALSE;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_HWI2C0_ReadRdy
/// @brief \b Function  \b Description: Start byte reading
/// @param <IN>         \b None :
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE : ok, FALSE : fail
////////////////////////////////////////////////////////////////////////////////
BOOL HAL_HWI2C0_ReadRdy(U16 u16PortOffset)
{
    U8 u8Value=0;
    U8 u8Port;

    HWI2C_HAL_FUNC();

    if(HAL_HWI2C0_GetPortIdxByOffset(u16PortOffset,&u8Port)==FALSE)
        return FALSE;
    u8Value = (g_bLastByte[u8Port])? (_RDATA_CFG_TRIG|_RDATA_CFG_ACKBIT) : (_RDATA_CFG_TRIG);
    g_bLastByte[u8Port] = FALSE;
    return HAL_HWI2C0_WriteByte(REG_HWI2C0_RDATA_CFG+u16PortOffset, u8Value);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_HWI2C0_SendData
/// @brief \b Function  \b Description: Send 1 byte data to SDA
/// @param <IN>         \b None :
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE : ok, FALSE : fail
////////////////////////////////////////////////////////////////////////////////
BOOL HAL_HWI2C0_SendData(U16 u16PortOffset, U8 u8Data)
{
    HWI2C_HAL_FUNC();

    return HAL_HWI2C0_WriteByte(REG_HWI2C0_WDATA+u16PortOffset, u8Data);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_HWI2C0_RecvData
/// @brief \b Function  \b Description: Receive 1 byte data from SDA
/// @param <IN>         \b None :
/// @param <OUT>        \b None :
/// @param <RET>        \b U8 :
////////////////////////////////////////////////////////////////////////////////
U8 HAL_HWI2C0_RecvData(U16 u16PortOffset)
{
    HWI2C_HAL_FUNC();

    return HAL_HWI2C0_ReadByte(REG_HWI2C0_RDATA+u16PortOffset);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_HWI2C0_Get_SendAck
/// @brief \b Function  \b Description: Get ack after sending data
/// @param <IN>         \b None :
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE: Valid ack, FALSE: No ack
////////////////////////////////////////////////////////////////////////////////
BOOL HAL_HWI2C0_Get_SendAck(U16 u16PortOffset)
{
    HWI2C_HAL_FUNC();
    return (HAL_HWI2C0_ReadByte(REG_HWI2C0_WDATA_GET+u16PortOffset) & _WDATA_GET_ACKBIT) ? FALSE : TRUE;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_HWI2C0_NoAck
/// @brief \b Function  \b Description: generate no ack pulse
/// @param <IN>         \b None :
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE : ok, FALSE : fail
////////////////////////////////////////////////////////////////////////////////
BOOL HAL_HWI2C0_NoAck(U16 u16PortOffset)
{
    U8 u8Port;

    HWI2C_HAL_FUNC();

    if(HAL_HWI2C0_GetPortIdxByOffset(u16PortOffset,&u8Port)==FALSE)
        return FALSE;
    g_bLastByte[u8Port] = TRUE;
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_HWI2C0_Ack
/// @brief \b Function  \b Description: generate ack pulse
/// @param <IN>         \b None :
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE : ok, FALSE : fail
////////////////////////////////////////////////////////////////////////////////
BOOL HAL_HWI2C0_Ack(U16 u16PortOffset)
{
    U8 u8Port;

    HWI2C_HAL_FUNC();

    if(HAL_HWI2C0_GetPortIdxByOffset(u16PortOffset,&u8Port)==FALSE)
        return FALSE;
    g_bLastByte[u8Port] = FALSE;
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_HWI2C0_GetStae
/// @brief \b Function  \b Description: Get i2c Current State
/// @param <IN>         \b u16PortOffset: HWI2C Port Offset
/// @param <OUT>        \b None
/// @param <RET>        \b HWI2C current status
////////////////////////////////////////////////////////////////////////////////
U8 HAL_HWI2C0_GetState(U16 u16PortOffset)
{

	U8 cur_state =  HAL_HWI2C0_ReadByte(REG_HWI2C0_CUR_STATE+u16PortOffset) & _CUR_STATE_MSK;
	HWI2C_HAL_FUNC();

	if (cur_state <= 0) // 0: idle
		return E_HAL_HWI2C_STATE_IDEL;
	else if (cur_state <= 2) // 1~2:start
		return E_HAL_HWI2C_STATE_START;
	else if (cur_state <= 6) // 3~6:write
		return E_HAL_HWI2C_STATE_WRITE;
	else if (cur_state <= 10) // 7~10:read
		return E_HAL_HWI2C_STATE_READ;
	else if (cur_state <= 11) // 11:interrupt
		return E_HAL_HWI2C_STATE_INT;
	else if (cur_state <= 12) // 12:wait
		return E_HAL_HWI2C_STATE_WAIT;
	else  // 13~15:stop
		return E_HAL_HWI2C_STATE_STOP;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_HWI2C0_Is_Idle
/// @brief \b Function  \b Description: Check if i2c is idle
/// @param <IN>         \b u16PortOffset: HWI2C Port Offset
/// @param <OUT>        \b None
/// @param <RET>        \b TRUE : idle, FALSE : not idle
////////////////////////////////////////////////////////////////////////////////
BOOL HAL_HWI2C0_Is_Idle(U16 u16PortOffset)
{
    HWI2C_HAL_FUNC();

    return ((HAL_HWI2C0_GetState(u16PortOffset)==E_HAL_HWI2C_STATE_IDEL) ? TRUE : FALSE);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_HWI2C0_Is_INT
/// @brief \b Function  \b Description: Check if i2c is interrupted
/// @param <IN>         \b u8Status : queried status
/// @param <IN>         \b u8Ch: Channel 0/1
/// @param <OUT>        \b None
/// @param <RET>        \b TRUE : ok, FALSE : fail
////////////////////////////////////////////////////////////////////////////////
BOOL HAL_HWI2C0_Is_INT(U16 u16PortOffset)
{
    HWI2C_HAL_FUNC();
    return (HAL_HWI2C0_ReadByte(REG_HWI2C0_INT_CTL+u16PortOffset) & _INT_CTL) ? TRUE : FALSE;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_HWI2C0_Clear_INT
/// @brief \b Function  \b Description: Enable interrupt for HWI2C
/// @param <IN>         \b None :
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE : ok, FALSE : fail
////////////////////////////////////////////////////////////////////////////////
BOOL HAL_HWI2C0_Clear_INT(U16 u16PortOffset)
{
    HWI2C_HAL_FUNC();

    return HAL_HWI2C0_WriteRegBit(REG_HWI2C0_INT_CTL+u16PortOffset, _INT_CTL, TRUE);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_HWI2C0_Reset
/// @brief \b Function  \b Description: Reset HWI2C state machine
/// @param <IN>         \b bReset : TRUE: Reset FALSE: Not reset
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE: Ok, FALSE: Fail
////////////////////////////////////////////////////////////////////////////////
BOOL HAL_HWI2C0_Reset(U16 u16PortOffset, BOOL bReset)
{
    HWI2C_HAL_FUNC();

    return HAL_HWI2C0_WriteRegBit(REG_HWI2C0_MIIC_CFG+u16PortOffset, _MIIC_CFG_RESET, bReset);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_HWI2C0_Send_Byte
/// @brief \b Function  \b Description: Send one byte
/// @param u8Data       \b IN: 1 byte data
/// @return             \b TRUE: Success FALSE: Fail
////////////////////////////////////////////////////////////////////////////////
BOOL HAL_HWI2C0_Send_Byte(U16 u16PortOffset, U8 u8Data)
{
    U8 u8Retry = HWI2C_HAL_RETRY_TIMES;
    U16 u16Count = HWI2C_HAL_WAIT_TIMEOUT;

    HWI2C_HAL_FUNC();
    HWI2C_HAL_ERR("Send byte 0x%X\n", u8Data);

    while(u8Retry--)
    {
        HAL_HWI2C0_Clear_INT(u16PortOffset);
        if (HAL_HWI2C0_SendData(u16PortOffset,u8Data))
        {
            u16Count = HWI2C_HAL_WAIT_TIMEOUT;
            while(u16Count--)
            {
                //HAL_HWI2C0_ExtraDelay(10000);
                if (HAL_HWI2C0_Is_INT(u16PortOffset))
                {
                    HAL_HWI2C0_Clear_INT(u16PortOffset);
                    if (HAL_HWI2C0_Get_SendAck(u16PortOffset))
                    {
                        #if 1
                        //HAL_HWI2C0_ExtraDelay(1);
                        udelay(1);
                        #else
                        MsOS_DelayTaskUs(1);
                        #endif
                        return TRUE;
                    }
					//break;
                }
            }
        }

		if(HAL_HWI2C0_ReadByte(REG_HWI2C0_CUR_STATE+u16PortOffset)==0)
		{
			HAL_HWI2C0_Start(u16PortOffset);
			udelay(2);
		}
		else
		{
			HAL_HWI2C0_Stop(u16PortOffset);
			udelay(2);
			HAL_HWI2C0_Reset(u16PortOffset,TRUE);
			HAL_HWI2C0_Reset(u16PortOffset,FALSE);
			udelay(2);
			HAL_HWI2C0_Start(u16PortOffset);
			udelay(2);
		}
    }
    HWI2C_HAL_ERR("Send byte 0x%X fail!\n", u8Data);
    return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_HWI2C0_Recv_Byte
/// @brief \b Function  \b Description: Init HWI2C driver and auto generate ACK
/// @param *pData       \b Out: received data
/// @return             \b TRUE: Success FALSE: Fail
////////////////////////////////////////////////////////////////////////////////
BOOL HAL_HWI2C0_Recv_Byte(U16 u16PortOffset, U8 *pData)
{
    U16 u16Count = HWI2C_HAL_WAIT_TIMEOUT;

    HWI2C_HAL_FUNC();

    if (!pData)
        return FALSE;

    HAL_HWI2C0_ReadRdy(u16PortOffset);
    while((!HAL_HWI2C0_Is_INT(u16PortOffset))&&(u16Count > 0))
        u16Count--;
    HAL_HWI2C0_Clear_INT(u16PortOffset);
    if (u16Count)
    {
        //get data before clear int and stop
        *pData = HAL_HWI2C0_RecvData(u16PortOffset);
        //clear interrupt
        HAL_HWI2C0_Clear_INT(u16PortOffset);
        #if 1
        HAL_HWI2C0_ExtraDelay(1);
        #else
        MsOS_DelayTaskUs(1);
        #endif
        return TRUE;
    }
    HWI2C_HAL_ERR("Recv byte fail!\n");
    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_HWI2C0_Init_ExtraProc
/// @brief \b Function  \b Description: Do extral procedure after initialization
/// @param <IN>         \b None :
/// @param param        \b None :
/// @param <RET>        \b None :
////////////////////////////////////////////////////////////////////////////////
void HAL_HWI2C0_Init_ExtraProc(void)
{
    HWI2C_HAL_FUNC();
    //Extra procedure TODO
}
#endif
