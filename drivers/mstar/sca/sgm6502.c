// $Change: 593163 $
//-----------------------------------------------------------------------------
//
// Copyright (c) 2008 MStar Semiconductor, Inc.  All rights reserved.
//
//-----------------------------------------------------------------------------
// FILE
//      sgm6502.c
//
// DESCRIPTION
//      sgm6502 video switch
//	   There are 8 intput & 6 output
// HISTORY
//-----------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Include Files
//------------------------------------------------------------------------------

#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/module.h>
#include "sgm6502.h"

extern void MDrv_HW_IIC_Init(void);
extern BOOL MDrv_HWI2C_WriteBytes(U16 u16SlaveCfg, U32 uAddrCnt, U8 *pRegAddr, U32 uSize, U8 *pData);
extern BOOL MDrv_HWI2C_ReadBytes(U16 u16SlaveCfg, U32 uAddrCnt, U8 *pRegAddr, U32 uSize, U8 *pData);

#define SGM6502_ADD	0X06

#define OUTPUT12_REG	0x00	//bit0~bit3:1 bit4~bit7:2
#define OUTPUT34_REG	0x01	//bit0~bit3:3 bit4~bit7:4
#define OUTPUT56_REG	0x02	//bit0~bit3:5 bit4~bit7:6

#define INPUT_REG	0x03	//bit0~bit7
#define GAIN_REG	0x04	//bit0~bit5


U8 SGM_Open(void)
{
    MDrv_HW_IIC_Init();
	return 0;
}

U8 SGM_Close(void)
{
	return 0;
}

BOOL SGM_IICRead(U8 Command,U8 *readdata,U32 nread)
{
	BOOL bRet = FALSE;
	U8 bData[1] = {0};
	bData[0] = Command;

    bRet=MDrv_HWI2C_ReadBytes(SGM6502_ADD, 1, bData, nread, readdata);
	return bRet;

}

BOOL SGM_IICWrite(U8 reg,U8 data,U32 u32WriteCnt)
{
	BOOL bRet = FALSE;
	U8 bData[1] = {0};
	bData[0] = reg;
	
    bRet=MDrv_HWI2C_WriteBytes((U16)((0x01<<8)|SGM6502_ADD), 1, bData, u32WriteCnt, &data);
	return bRet;

}

BOOL SGM_Switch(U8 u8In, U8 u8Out)
{
	U8 u8Addr = 0;
	U8 u8Data = 0;
	
    if(SGM_Open() != 0)
    {
    	printk("[SGM6502] SGM_Open failed!!!\r\n");
    	return -1;
    }
	
	if(u8In < 0 || u8In > 7)
	{
		printk("[SGM6502] input is error!!!\r\n");	
		return -1;
	}
	if(u8Out < 0 || u8Out > 5)
	{
		printk("[SGM6502] output is error!!!\r\n");	
		return -1;
	}
	
	//初始化SGM6502 关闭所有输入通道
	SGM_IICWrite(OUTPUT12_REG, 0x00, 1);
	SGM_IICWrite(OUTPUT34_REG, 0x00, 1);
	SGM_IICWrite(OUTPUT56_REG, 0x00, 1);
	
	
    	u8Addr=(u8Out-1)/2;
	if(u8Out%2)
	   u8Data = u8In&0x0f;
	else
	   u8Data = (u8In)<<4&0xf0;
	
	SGM_IICWrite(u8Addr, u8Data, 1);
	u8Data=0xFF;
	SGM_IICWrite(INPUT_REG, u8Data, 1);	//set input is clamp
    	u8Data=0x3f;/*0=6db,1=0db */
	SGM_IICWrite(GAIN_REG, u8Data, 1);	//set input is gain
	SGM_Close();
	return  0;
}

