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

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    mdrv_gpio.c
/// @brief  GPIO Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////
#define MDRV_SCA_MST701_C

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
//#include "MsCommon.h"
//#include <linux/autoconf.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/serio.h>
#include <linux/clk.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/semaphore.h>
#include <linux/kthread.h>
//#include <stdio.h>
//#include <mach/Irqs.h>
#include <mach/ms_types.h>
#include "mdrv_sca_mst701.h"


#define SIGNAL_GOOD     1
#define SIGNAL_OFF      2
#define SIGNAL_UNSTABLE 3
#define M9_RESET        4
#define MST701_BIN_PATH "/data/MST701.BIN"


extern void MDrv_HW_IIC2_Init(void);
extern BOOL MDrv_HWI2C2_M701_WriteBytes(U16 u16SlaveCfg, U32 uAddrCnt, U8 *pRegAddr, U32 uSize, U8 *pData);
extern BOOL MDrv_HWI2C2_WriteBurst(U8 *pu8WriteValue, U32 u32WriteCnt);
extern BOOL MDrv_HWI2C2_CheckM9Status(U8   u8SlaveId, U8  *pu8RegAddr,U32  u32RegAddrCnt,U8  *pu8Data,U32  u32DataCnt);


void Maria_I2C_WriteByte(U16 i2c_addr, U8 i2c_data)
{
    U16 SlaveID ;
    U8 SlaveReg ;
    U8 aI2CData[1];
    U8 bData[1] = {0};

    aI2CData[0] = i2c_data;
    SlaveID= (i2c_addr>>8)&0xff;
    SlaveReg = (U8)((i2c_addr)&0xff);
    bData[0] = SlaveReg;
    MDrv_HWI2C2_M701_WriteBytes(SlaveID, 1, bData, 1, aI2CData);
}

BOOL Maria_I2C_WriteBytes(U16 i2c_addr, U32 i2c_num, U8 *i2c_data)
{
    U16 SlaveID ;
    U8 SlaveReg ;
    U32 SlaveLen;
	U8 bData[1] = {0};

    SlaveLen = (U32)(i2c_num);
    SlaveID= (U16)((i2c_addr>>8)&0xff);
    SlaveReg = (U8)((i2c_addr)&0xff);
	bData[0] = SlaveReg;

    return (MDrv_HWI2C2_M701_WriteBytes(SlaveID, 1, bData, SlaveLen, i2c_data));
}

void Maria_I2C_WriteBurst( U8 len, U8* i2cdata)
{
    U32 SlaveLen;

    SlaveLen =  (U32)(len);
    MDrv_HWI2C2_WriteBurst( i2cdata, SlaveLen);
}


BOOL load8051(U32 dataBytes, U8 *pBinData)
{
   /* FILE* pFile;
   // DWORD	dwfileLen, decTime;
    unsigned int dwfileLen;

    U16 dwNumRead;
    //U8 pBinData[4*1024+1] = {0};

    //decTime = GetTickCount();

    if ((pFile = fopen(lpFileName,_T("rb"))) != NULL) {
        if(fseek( pFile, 0, SEEK_END)){
            RETAILMSG(1, (_T("[LOAD BIN] fseek file[%s] fail!\r\n"),lpFileName));
            return FALSE;
        }
        else{
            dwfileLen = ftell(pFile);
            fseek( pFile, 0, SEEK_SET);
            dwNumRead = fread( pBinData, sizeof( char ), dwfileLen, pFile );
            fclose(pFile);

            if(dwNumRead > 4096)
            {
                RETAILMSG(DBG_WAV_CTL, (_T("[LOAD BIN] bin size error(0x%x)!\r\n"),dwNumRead));
                return FALSE;
            }
            else
                RETAILMSG(DBG_WAV_CTL, (_T("[LOAD BIN] file size[0x%x] !\r\n"),dwNumRead));

        }
    }
    else
    {
        RETAILMSG(DBG_WAV_CTL, (_T("[LOAD BIN] open file[%s] fail\r\n"),lpFileName));
        return FALSE;
    }

    //TODO: dump
*/
    // load 4k code
    return (Maria_I2C_WriteBytes(0xF000, dataBytes, pBinData));
    //RETAILMSG(1, (_T("[LOAD BIN]upload mst701 finished!\r\n\r\n")));
    //return TRUE;
}

static void EnterMariaDebugMode(void)
{
	BYTE  enter_debug[]={0x53,0x45,0x52,0x44,0x42};
	// enter debug mode
	Maria_I2C_WriteBurst(5,(U8 *)&enter_debug);
	//Maria_I2C_WriteBurst(5,enter_debug);

	// enable single step
	Maria_I2C_WriteByte(0xC0C1,0x53);
}

static void ExitMariaDebugMode(void)
{
	BYTE  exit_debug[]={0x45};
	// disable single step
	Maria_I2C_WriteByte(0xC0C1,0x00);
	// exit debug mode
	Maria_I2C_WriteBurst(1,exit_debug);
}

U8 CheckM701(void)
{
    BYTE  buff[]={0x00,0xFF,0xFF,0xFF};
    BYTE  VerBuff[3]={0x01,0x01,0x02};
    BYTE  i;
    BOOL bRet = FALSE;

    for(i=0;i<2;i++)
    {
		bRet = MDrv_HWI2C2_CheckM9Status(0x6E, (U8 *)&buff, 4, VerBuff, 3);

        printk(KERN_INFO " VER : %x,%x,%x\r\n", VerBuff[0], VerBuff[1], VerBuff[2]);
        if(bRet && VerBuff[1]== 0x55)
        {
            printk("i = %d SIGNAL_GOOD\n" , i);
            return SIGNAL_GOOD;
        }
        else if(bRet && VerBuff[1]== 0x00)
        {
            printk("i = %d SIGNAL_OFF\n" , i);
            return SIGNAL_OFF;
        }
        else if(bRet && VerBuff[1]== 0x88)
        {
            printk("i = %d M9_RESET\n" , i);
            return M9_RESET;
        }
        else
        {
            printk("i = %d SIGNAL_UNSTABLE\n" , i);
            return SIGNAL_UNSTABLE;
        }
    }

    return FALSE;
   /* U16 dwNumRead;
    U8 pBinData[4] = {0x0C, 0x01, 0x5A, 0x9A};
    Maria_I2C_WriteBytes(0xF000,4,pBinData);*/
}

BOOL Load4KCode(U32 dataBytes, U8 *pBinData)
{
    BOOL ret = FALSE;
    U8 u8Loop=0;
    //char fname[100] = ;

ReLoadCode:
    // enter debug mode
    EnterMariaDebugMode();

    // switch to 4k data
    Maria_I2C_WriteByte(0xC039,0x00);

    // load 4k code
    ret = load8051(dataBytes, pBinData);
    //if(!ret)
       // ret = load8051(TEXT("\\windows\\mst701.bin"));

    //if fail ,redo.
    if(!ret && (u8Loop++ < 3))
    {
		//exit debug mode
        ExitMariaDebugMode();
        msleep(10);
        printk (KERN_INFO"[drv]load8051 fail ,count(%d)\r\n",u8Loop );
        goto ReLoadCode;
    }

    if(ret)
    {
        // restart maria
        Maria_I2C_WriteByte(0xc0c5,0x52);
        // switch to 4k code
        Maria_I2C_WriteByte(0xC039,0x30);
        Maria_I2C_WriteByte(0xC09E,0x0F);
    }

    //exit debug mode
    ExitMariaDebugMode();

    return ret;
}

MS_BOOL MApi_Init_MST701(MS_U32 dataBytes, MS_U8 *pBinData)
{
    MDrv_HW_IIC2_Init();

    if(!Load4KCode(dataBytes, pBinData))
        return FALSE;

    msleep(700);

    if(!CheckM701())
        return FALSE;

    return TRUE;
}

