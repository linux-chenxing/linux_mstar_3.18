#define _HAL_BDMA_C

////////////////////////////////////////////////////////////////////////////////
// Header Files
////////////////////////////////////////////////////////////////////////////////
//#include <stdbool.h>
#include <linux/string.h>
#include <linux/kernel.h>

#include "mdrv_types.h"

#include "mdrv_bdma.h"
#include "mhal_bdma.h"
#include "mhal_rng_bdma.h"

////////////////////////////////////////////////////////////////////////////////
// Define & data type
///////////////////////////////////////////////////////////////////////////////
#define HAL_BDMA_REG_CH02CH1(ch0Reg)    (ch0Reg+BDMA_CH_REG_OFFSET)
#define HAL_BDMA_REG_CH12CH0(ch1Reg)    (ch1Reg-BDMA_CH_REG_OFFSET)

#define BDMA_HAL_ERR(x, args...)        //{printf(x, ##args);}
#define BDMA_HAL_NOTSUPPORT()           //{printk("%s not support!\n", __FUNCTION__);}

////////////////////////////////////////////////////////////////////////////////
// Global variable
////////////////////////////////////////////////////////////////////////////////
static long _gMIO_MapBase = 0;

////////////////////////////////////////////////////////////////////////////////
// Extern Function
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Function Declaration
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Local Function
////////////////////////////////////////////////////////////////////////////////
static long _HAL_BDMA_Get_RegAddr(long phy64RegAddr, U8 u8Ch);
static bool  _HAL_BDMA_Is_Status_On(U8 u8Status, U8 u8Ch);

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: _HAL_BDMA_Get_RegAddr
/// @brief \b Function  \b Description: Get byte DMA register address for channel 0/1
/// @param <IN>         \b u32RegAddr: Source address
/// @param <IN>         \b u8Ch: Channel 0/1
/// @param <OUT>        \b None :
/// @param <RET>        \b register address
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
static long _HAL_BDMA_Get_RegAddr(long phy64RegAddr, U8 u8Ch)
{
    if (E_BDMA_CH1 == u8Ch)
        return HAL_BDMA_REG_CH02CH1(phy64RegAddr);
    return phy64RegAddr;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: _HAL_BDMA_Is_Status_On
/// @brief \b Function  \b Description: Get byte DMA status for channel 0/1
/// @param <IN>         \b u8Status : queried status
/// @param <IN>         \b u8Ch: Channel 0/1
/// @param <OUT>        \b None
/// @param <RET>        \b TRUE: Ok FALSE: Fail
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
static bool _HAL_BDMA_Is_Status_On(U8 u8Status, U8 u8Ch)
{
    long phy64Reg = _HAL_BDMA_Get_RegAddr(BDMA_REG_CH0_STATUS, u8Ch);
    BDMA_HAL_ERR("%s reg:%lx val:%u status:%u\n",  __FUNCTION__, u32Reg, HAL_BDMA_ReadByte(phy64Reg), u8Status);
    return (HAL_BDMA_ReadByte(phy64Reg) & u8Status) ? TRUE : FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// Global Function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_BDMA_ReadByte
/// @brief \b Function  \b Description: read 1 Byte data
/// @param <IN>         \b u32RegAddr: register address
/// @param <OUT>        \b None :
/// @param <RET>        \b U8
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
U8 HAL_BDMA_ReadByte(long phy64RegAddr)
{
    return *(volatile U8*)phy64RegAddr;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_BDMA_Read4Byte
/// @brief \b Function  \b Description: read 2 Byte data
/// @param <IN>         \b u32RegAddr: register address
/// @param <OUT>        \b None :
/// @param <RET>        \b U16
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
U16 HAL_BDMA_Read2Byte(long phy64RegAddr)
{
    return *(volatile U16*)phy64RegAddr ;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_BDMA_Read4Byte
/// @brief \b Function  \b Description: read 4 Byte data
/// @param <IN>         \b u32RegAddr: register address
/// @param <OUT>        \b None :
/// @param <RET>        \b U32
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
U32 HAL_BDMA_Read4Byte(long phy64RegAddr)
{
    return (HAL_BDMA_Read2Byte(phy64RegAddr) | HAL_BDMA_Read2Byte(phy64RegAddr+2) << 16);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_BDMA_WriteByte
/// @brief \b Function  \b Description: write 1 Byte data
/// @param <IN>         \b u32RegAddr: register address
/// @param <IN>         \b u8Val : 1 byte data
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE: Ok FALSE: Fail
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
bool HAL_BDMA_WriteByte(long phy64RegAddr, U8 u8Val)
{
    if (!phy64RegAddr)
    {
        BDMA_HAL_ERR("%s reg error!\n", __FUNCTION__);
        return FALSE;
    }

    *(volatile U8*)phy64RegAddr = u8Val;
	//*(volatile U8*) ((phy64RegAddr << 1) - (phy64RegAddr & 1)) = u8Val;
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_BDMA_Write2Byte
/// @brief \b Function  \b Description: write 2 Byte data
/// @param <IN>         \b u32RegAddr: register address
/// @param <IN>         \b u16Val : 2 byte data
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE: Ok FALSE: Fail
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
bool HAL_BDMA_Write2Byte(long phy64RegAddr, U16 u16Val)
{
    if (!phy64RegAddr)
    {
        BDMA_HAL_ERR("%s reg error!\n", __FUNCTION__);
        return FALSE;
    }

    *(volatile U16*)phy64RegAddr = u16Val;
	//*(volatile U16*) ((phy64RegAddr << 1) - (phy64RegAddr & 1)) = u16Val ;
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_BDMA_Write4Byte
/// @brief \b Function  \b Description: write 4 Byte data
/// @param <IN>         \b phy64RegAddr: register address
/// @param <IN>         \b u32Val : 4 byte data
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE: Ok FALSE: Fail
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
bool HAL_BDMA_Write4Byte(long phy64RegAddr, U32 u32Val)
{
    if (!phy64RegAddr)
    {
        BDMA_HAL_ERR("%s reg error!\n", __FUNCTION__);
        return FALSE;
    }

    HAL_BDMA_Write2Byte(phy64RegAddr, u32Val & 0x0000FFFF);
    HAL_BDMA_Write2Byte(phy64RegAddr+4, u32Val >> 16);
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_BDMA_WriteByte
/// @brief \b Function  \b Description: write 1 Byte data
/// @param <IN>         \b phy64RegAddr: register address
/// @param <IN>         \b u8Val : 1 byte data
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE: Ok FALSE: Fail
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
bool HAL_BDMA_WriteRegBit(long phy64RegAddr, U8 u8Mask, bool bEnable)
{
    U8 u8Val = HAL_BDMA_ReadByte(phy64RegAddr);
    if (!phy64RegAddr)
    {
        BDMA_HAL_ERR("%s reg error!\n", __FUNCTION__);
        return FALSE;
    }

    u8Val = HAL_BDMA_ReadByte(phy64RegAddr);
    u8Val = (bEnable) ? (u8Val | u8Mask) : (u8Val & ~u8Mask);
    HAL_BDMA_WriteByte(phy64RegAddr, u8Val);
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_BDMA_SetSrcAddr
/// @brief \b Function  \b Description: Set source address
/// @param <IN>         \b phy64RegAddr: Source address
/// @param <IN>         \b u8Ch: Channel 0/1
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE: Ok FALSE: Fail
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
bool HAL_BDMA_SetSrcAddr(BDMA_Act eAct, long phy64RegAddr, U8 u8Ch)
{
    long phy64Reg = _HAL_BDMA_Get_RegAddr(BDMA_REG_CH0_SRC_ADDR_L, u8Ch);
	
    return HAL_BDMA_Write4Byte(phy64Reg, phy64RegAddr);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_BDMA_GetSrcAddr
/// @brief \b Function  \b Description: Get destination address
/// @param <IN>         \b u8Ch: Channel 0/1
/// @param <RET>        \b destinaiton address
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
U32 HAL_BDMA_GetSrcAddr(BDMA_Act eAct, U8 u8Ch)
{
    long phy64Reg = _HAL_BDMA_Get_RegAddr(BDMA_REG_CH0_SRC_ADDR_L, u8Ch);
    return HAL_BDMA_Read4Byte(phy64Reg);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_BDMA_SetDstAddr
/// @brief \b Function  \b Description: Set destination address
/// @param <IN>         \b u32RegAddr: destination address
/// @param <IN>         \b u8Ch: Channel 0/1
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE: Ok FALSE: Fail
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
bool HAL_BDMA_SetDstAddr(BDMA_Act eAct, long phy64RegAddr, U8 u8Ch)
{
    long phy64Reg = _HAL_BDMA_Get_RegAddr(BDMA_REG_CH0_DST_ADDR_L, u8Ch);
	
    return HAL_BDMA_Write4Byte(phy64Reg, phy64RegAddr);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_BDMA_GetDstAddr
/// @brief \b Function  \b Description: Get destination address
/// @param <IN>         \b u8Ch: Channel 0/1
/// @param <RET>        \b destinaiton address
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
U32 HAL_BDMA_GetDstAddr(BDMA_Act eAct, U8 u8Ch)
{
    long phy64Reg = _HAL_BDMA_Get_RegAddr(BDMA_REG_CH0_DST_ADDR_L, u8Ch);
    return HAL_BDMA_Read4Byte(phy64Reg);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_BDMA_GetDevDw
/// @brief \b Function  \b Description: Get device data width
/// @param <IN>         \b eDev: Device
/// @param <IN>         \b u8Ch: Channel 0/1
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE: Ok FALSE: Fail
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
U8 HAL_BDMA_GetDevDw(BDMA_Dev eDev)
{
    switch (eDev)
    {
    case E_BDMA_DEV_MIU0:
    case E_BDMA_DEV_MIU1:
        return E_BDMA_DW_16BYTE;

    case E_BDMA_DEV_FLASH:
        return E_BDMA_DW_8BYTE;

    case E_BDMA_DEV_MEM_FILL:
        return E_BDMA_DW_4BYTE;
    default:
        return E_BDMA_DW_1BYTE;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_BDMA_GetDevId
/// @brief \b Function  \b Description: Get device id
/// @param <IN>         \b eDev: Device
/// @param <RET>        \b The device ID
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
U8 HAL_BDMA_GetDevId(BDMA_Dev eDev)
{
    switch (eDev)
    {
        case E_BDMA_DEV_MIU0:
            return E_BDMA_HALDEV_MIU0;
        case E_BDMA_DEV_MIU1:
            return E_BDMA_HALDEV_MIU1;
        case E_BDMA_DEV_SEARCH:
            return E_BDMA_HALDEV_SEARCH;
        case E_BDMA_DEV_CRC32:
            return E_BDMA_HALDEV_CRC32;
        case E_BDMA_DEV_MEM_FILL:
            return E_BDMA_HALDEV_MEM_FILL;
        case E_BDMA_DEV_FLASH:
            return E_BDMA_HALDEV_FLASH;
        case E_BDMA_DEV_1KSRAM_HK51:
            return E_BDMA_HALDEV_1KSRAM_HK51;
        case E_BDMA_DEV_DMDMCU:
            return E_BDMA_HALDEV_DMDMCU;
        case E_BDMA_DEV_DSP:
            return E_BDMA_HALDEV_DSP;
        case E_BDMA_DEV_TSP:
            return E_BDMA_HALDEV_TSP;
        default:
            return E_BDMA_HALDEV_NOT_SUPPORT;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_BDMA_SetSrcDev
/// @brief \b Function  \b Description: Set source device
/// @param <IN>         \b u8DevCfg: Device config
/// @param <IN>         \b u8Ch: Channel 0/1
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE: Ok FALSE: Fail
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
bool HAL_BDMA_SetSrcDev(U8 u8DevCfg, U8 u8Ch)
{
    long phy64Reg = _HAL_BDMA_Get_RegAddr(BDMA_REG_CH0_SRC_SEL, u8Ch);
    return HAL_BDMA_WriteByte(phy64Reg, u8DevCfg);
}

///////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_BDMA_SetSrcDev
/// @brief \b Function  \b Description: Set source device
/// @param <IN>         \b u8DevCfg: Device config
/// @param <IN>         \b u8Ch: Channel 0/1
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE: Ok FALSE: Fail
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
bool HAL_BDMA_SetDstDev(U8 u8DevCfg, U8 u8Ch)
{
    long phy64Reg = _HAL_BDMA_Get_RegAddr(BDMA_REG_CH0_DST_SEL, u8Ch);
    return HAL_BDMA_WriteByte(phy64Reg, u8DevCfg);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_BDMA_SetCmd0
/// @brief \b Function  \b Description: Set command 0
/// @param <IN>         \b u32Cmd: Command 0 value
/// @param <IN>         \b u8Ch: Channel 0/1
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE: Ok FALSE: Fail
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
bool HAL_BDMA_SetCmd0(U32 u32Cmd, U8 u8Ch)
{
    long phy64Reg = _HAL_BDMA_Get_RegAddr(BDMA_REG_CH0_CMD0_L, u8Ch);
    return HAL_BDMA_Write4Byte(phy64Reg, u32Cmd);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_BDMA_GetCmd0
/// @brief \b Function  \b Description: Get command 0
/// @param <IN>         \b u8Ch: Channel 0/1
/// @param <OUT>        \b *pu32Cmd : Command 0 value
/// @param <RET>        \b TRUE: Ok FALSE: Fail
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
U32 HAL_BDMA_GetCmd0(U8 u8Ch)
{
    long phy64Reg = _HAL_BDMA_Get_RegAddr(BDMA_REG_CH0_CMD0_L, u8Ch);
    return HAL_BDMA_Read4Byte(phy64Reg);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_BDMA_SetCmd1
/// @brief \b Function  \b Description: Set command 1
/// @param <IN>         \b u32Cmd: Command 1 value
/// @param <IN>         \b u8Ch: Channel 0/1
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE: Ok FALSE: Fail
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
bool HAL_BDMA_SetCmd1(U32 u32Cmd, U8 u8Ch)
{
    long phy64Reg = _HAL_BDMA_Get_RegAddr(BDMA_REG_CH0_CMD1_L, u8Ch);
    return HAL_BDMA_Write4Byte(phy64Reg, u32Cmd);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_BDMA_GetCmd1
/// @brief \b Function  \b Description: Get command 1
/// @param <IN>         \b u8Ch: Channel 0/1
/// @param <OUT>        \b *pu32Cmd : Command 1 value
/// @param <RET>        \b TRUE: Ok FALSE: Fail
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
U32 HAL_BDMA_GetCmd1(U8 u8Ch)
{
    long phy64Reg = _HAL_BDMA_Get_RegAddr(BDMA_REG_CH0_CMD1_L, u8Ch);
    return HAL_BDMA_Read4Byte(phy64Reg);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_BDMA_SetCmd2
/// @brief \b Function  \b Description: Set command 2
/// @param <IN>         \b u32Cmd: Command 2 value
/// @param <IN>         \b u8Ch: Channel 0/1
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE: Ok FALSE: Fail
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
bool HAL_BDMA_SetCmd2(U32 u32Cmd, U8 u8Ch)
{
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_BDMA_GetCmd2
/// @brief \b Function  \b Description: Get command 2
/// @param <IN>         \b u8Ch: Channel 0/1
/// @param <OUT>        \b *pu32Cmd : Command 2 value
/// @param <RET>        \b TRUE: Ok FALSE: Fail
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
U32 HAL_BDMA_GetCmd2(U8 u8Ch)
{
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_BDMA_GetCRC32
/// @brief \b Function  \b Description: Get crc32 result
/// @param <IN>         \b u8Ch: Channel 0/1
/// @param <OUT>        \b None:
/// @param <RET>        \b Matched address
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
U32 HAL_BDMA_GetCRC32(U8 u8Ch)
{
    return HAL_BDMA_GetCmd1(u8Ch);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_BDMA_GetMatched
/// @brief \b Function  \b Description: Get matched address
/// @param <IN>         \b u8Ch: Channel 0/1
/// @param <OUT>        \b None:
/// @param <RET>        \b Matched address
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
U32 HAL_BDMA_GetMatched(long phy64SrcAddr, U8 u8Ch)
{
    BDMA_HAL_ERR("Matched address:%x\n", HAL_BDMA_GetSrcAddr(E_BDMA_ACT_SEARCH, u8Ch));
    return ((U32) phy64SrcAddr + HAL_BDMA_GetSrcAddr(E_BDMA_ACT_SEARCH, u8Ch) - 1);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_BDMA_SetLen
/// @brief \b Function  \b Description: Set data size
/// @param <IN>         \b u32Len: Data size
/// @param <IN>         \b u8Ch: Channel 0/1
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE: Ok FALSE: Fail
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
bool HAL_BDMA_SetLen(BDMA_Act eAct, U32 u32Len, U8 u8Ch)
{
    long phy64Reg = _HAL_BDMA_Get_RegAddr(BDMA_REG_CH0_SIZE_L, u8Ch);
    return HAL_BDMA_Write4Byte(phy64Reg, u32Len);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_BDMA_GetLen
/// @brief \b Function  \b Description: Get command 0
/// @param <IN>         \b u8Ch: Channel 0/1
/// @param <OUT>        \b *pu32Len : data length
/// @param <RET>        \b TRUE: Ok FALSE: Fail
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
U32 HAL_BDMA_GetLen(BDMA_Act eAct, U8 u8Ch)
{
    long phy64Reg = _HAL_BDMA_Get_RegAddr(BDMA_REG_CH0_SIZE_L, u8Ch);
    return HAL_BDMA_Read4Byte(phy64Reg);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_BDMA_Set_Addr_Dec
/// @brief \b Function  \b Description: Set address increasing/decreasing direction
/// @param <IN>         \b bDec: TRUE: Decreasing FALSE: Increasing
/// @param <IN>         \b u8Ch: Channel 0/1
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE: Ok FALSE: Fail
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
bool HAL_BDMA_Set_Addr_Dec(bool bDec, U8 u8Ch)
{
    long phy64Reg = _HAL_BDMA_Get_RegAddr(BDMA_REG_CH0_MISC, u8Ch);

    BDMA_HAL_ERR("%s reg:%lx\n", __FUNCTION__,phy64Reg);
    return HAL_BDMA_WriteRegBit(phy64Reg, BDMA_CH_ADDR_DECDIR, bDec);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_BDMA_Set_CRC_Reflect
/// @brief \b Function  \b Description: Set CRC value reflection
/// @param <IN>         \b bReflect: TRUE: Reflection FALSE: Not Reflection
/// @param <IN>         \b u8Ch: Channel 0/1
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE: Ok FALSE: Fail
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
bool HAL_BDMA_Set_CRC_Reflect(bool bReflect, U8 u8Ch)
{
    long phy64Reg = _HAL_BDMA_Get_RegAddr(BDMA_REG_CH0_MISC, u8Ch);

    BDMA_HAL_ERR("%s reg:%lx\n", __FUNCTION__,phy64Reg);
    return HAL_BDMA_WriteRegBit(phy64Reg, BDMA_CH_CRC_REFLECTION, bReflect);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_BDMA_SetDmyWrCnt
/// @brief \b Function  \b Description: Set Dummy write count
/// @param <IN>         \b u8Cnt: Dummy count
/// @param <IN>         \b u8Ch: Channel 0/1
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE: Ok FALSE: Fail
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
bool HAL_BDMA_SetDmyWrCnt(U8 u8Cnt, U8 u8Ch)
{
    long phy64Reg = _HAL_BDMA_Get_RegAddr(BDMA_REG_CH0_DWUM_CNT, u8Ch);
    return HAL_BDMA_WriteByte(phy64Reg, u8Cnt);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: MDrv_BDMA_TrigOn
/// @brief \b Function  \b Description: Trigger on BDMA action for channel 0/1
/// @param <IN>         \b eAct: BDMA action
/// @param <IN>         \b u8Ch: Channel 0/1
/// @param <IN>         \b u8Para: trigger parameter
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE: Ok FALSE: Fail
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
bool HAL_BDMA_TrigOn(U8 u8Src, U8 u8Dst, U8 u8Ch)
{
    long phy64Reg = _HAL_BDMA_Get_RegAddr(BDMA_REG_CH0_CTRL, u8Ch);
    return HAL_BDMA_WriteRegBit(phy64Reg, BDMA_CH_TRIGGER, ENABLE);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_BDMA_Stop
/// @brief \b Function  \b Description: Stop BDMA operations
/// @param <IN>         \b u8Ch: Channel 0/1
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE: Ok FALSE: Fail
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
bool HAL_BDMA_Stop(U8 u8Ch)
{
    bool bRet = TRUE;
    long phy64Reg = _HAL_BDMA_Get_RegAddr(BDMA_REG_CH0_CTRL, u8Ch);

    //Set stop
    bRet &= HAL_BDMA_WriteRegBit(phy64Reg, BDMA_CH_STOP, ENABLE);
    //Clear stop
    bRet &= HAL_BDMA_WriteRegBit(phy64Reg, BDMA_CH_STOP, DISABLE);
    return bRet;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_BDMA_Enable_Done_INT
/// @brief \b Function  \b Description: Enable interrupt when action done
/// @param <IN>         \b bEnable: TRUE: Enable FALSE: Disable
/// @param <IN>         \b u8Ch: Channel 0/1
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE: Ok FALSE: Fail
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
bool HAL_BDMA_Enable_INT(bool bEnable, U8 u8Ch)
{
    long phy64Reg = _HAL_BDMA_Get_RegAddr(BDMA_REG_CH0_MISC, u8Ch);
    return HAL_BDMA_WriteRegBit(phy64Reg, BDMA_CH_DONE_INT_EN, bEnable);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_BDMA_Is_Queued
/// @brief \b Function  \b Description: Check if any action is queued
/// @param <IN>         \b u8Ch: Channel 0/1
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE: queued FALSE: empty
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
bool HAL_BDMA_Is_Queued(U8 u8Ch)
{
    return _HAL_BDMA_Is_Status_On(BDMA_CH_QUEUED, u8Ch);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_DMA_Is_FlashBusy
/// @brief \b Function  \b Description: check if DMA is ready for flash copy
/// @param <IN>         \b u8Ch: Channel 0/1
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE: Busy FALSE: Not busy
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
bool HAL_DMA_Is_FlashReady()
{
    BDMA_HAL_NOTSUPPORT();
    return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: MDrv_BDMA_Is_Busy
/// @brief \b Function  \b Description: Check if Byte DMA is busy
/// @param <IN>         \b u8Ch: Channel 0/1
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE: Busy FALSE: Not busy
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
bool HAL_BDMA_Is_Busy(U8 u8Ch)
{
    return _HAL_BDMA_Is_Status_On(BDMA_CH_BUSY, u8Ch);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_BDMA_Is_Int
/// @brief \b Function  \b Description: Check if interrupted when Byte DMA is done
/// @param <IN>         \b u8Ch: Channel 0/1
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE: Interrupted FALSE: No interrupt
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
bool HAL_BDMA_Is_Int(U8 u8Ch)
{
    return _HAL_BDMA_Is_Status_On(BDMA_CH_INT, u8Ch);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_BDMA_Is_Done
/// @brief \b Function  \b Description: Check if Byte DMA action is done
/// @param <IN>         \b eAct: BDMA action
/// @param <IN>         \b u8Ch: Channel 0/1
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE: Done FALSE: Not Done
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
bool HAL_BDMA_Is_Done(BDMA_Act eAct, U8 u8Ch)
{
    if (E_BDMA_ACT_SEARCH == eAct && HAL_BDMA_Is_Found(u8Ch))
        return TRUE;
    return _HAL_BDMA_Is_Status_On(BDMA_CH_DONE, u8Ch);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_BDMA_Is_Found
/// @brief \b Function  \b Description: Check if Byte DMA find matched pattern
/// @param <IN>         \b u8Ch: Channel 0/1
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE: found FALSE: Not found
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
bool HAL_BDMA_Is_Found(U8 u8Ch)
{
    return _HAL_BDMA_Is_Status_On(BDMA_CH_RESULT, u8Ch);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_BDMA_Clear_Status
/// @brief \b Function  \b Description: Clear BDMA action status
/// @param <IN>         \b eAct: BDMA action
/// @param <IN>         \b u8Ch: Channel 0/1
/// @param <OUT>        \b None :
/// @param <RET>        \b TRUE: Success FALSE: Fail
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
bool HAL_BDMA_Clear_Status(BDMA_Act eAct, U8 u8Ch)
{
    long phy64Reg = _HAL_BDMA_Get_RegAddr(BDMA_REG_CH0_STATUS, u8Ch);
    return HAL_BDMA_WriteByte(phy64Reg, BDMA_CH_CLEAR_STATUS);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_BDMA_Dump_AllReg
/// @brief \b Function  \b Description: Dump bdma all register
/// @param <IN>         \b None :
/// @param <OUT>        \b None :
/// @param <RET>        \b None :
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
void HAL_BDMA_Dump_AllReg(void)
{
    long phy64Reg = BDMA_REG_BASE;
    long phy64EndAddr = _HAL_BDMA_Get_RegAddr(BDMA_REG_CH0_CMD1_H, E_BDMA_CH1);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_BDMA_SetIOMapBase
/// @brief \b Function  \b Description: Set IO Map base
/// @param <IN>         \b None :
/// @param <OUT>        \b None :
/// @param <RET>        \b None :
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
void HAL_BDMA_SetIOMapBase(long u32Base)
{
    _gMIO_MapBase = u32Base;
    BDMA_HAL_ERR("BDMA IOMap base:%8x Reg offset:%4x\n", u32Base, BDMA_REG_BASE);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: HAL_BDMA_Get_IOMap_Base
/// @brief \b Function  \b Description: Get IO Map base
/// @param <IN>         \b None :
/// @param <OUT>        \b None :
/// @param <RET>        \b None :
/// @param <GLOBAL>     \b None :
////////////////////////////////////////////////////////////////////////////////
long HAL_BDMA_GetIOMapBase()
{
    return _gMIO_MapBase;
}


