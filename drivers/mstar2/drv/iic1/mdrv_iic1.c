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
/// file    mdrv_iic.c
/// @brief  IIC Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
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
#include <linux/cdev.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <asm/io.h>

#include "mst_devid.h"

#include "mdrv_iic1.h"
#include "mhal_iic1_reg.h"
#include "mhal_iic1.h"
#include "../gpio/mdrv_gpio.h"

//define hwi2c ports
#define HWI2C_PORTM                   4 //maximum support ports
#define HWI2C_PORTS                   HAL_HWI2C_PORTS
#define HWI2C_PORT0                   HAL_HWI2C_PORT0
#define HWI2C_PORT1                   HAL_HWI2C_PORT1
#define HWI2C_PORT2                   HAL_HWI2C_PORT2
#define HWI2C_PORT3                   HAL_HWI2C_PORT3

static BOOL _gbInit = FALSE;
//static HWI2C_DbgLv _geDbgLv = E_HWI2C_DBGLV_ERR_ONLY;
static HWI2C_State _geState = E_HWI2C_IDLE;
static HWI2C_Info _gsI2CInfo;
static U32 g_u32StartDelay = HWI2C_STARTDLY, g_u32StopDelay = HWI2C_STOPDLY; 
static HWI2C_ReadMode g_HWI2CReadMode[HWI2C_PORTS];
static HWI2C_PORT g_HWI2CPort[HWI2C_PORTS];
static U16 g_u16DelayFactor[HWI2C_PORTS];
static BOOL g_bDMAEnable[HWI2C_PORTS];
static U8 g_HWI2CPortIdx = HWI2C_PORT0;

#define _MDrv_HWI2C1_Send_Byte HAL_HWI2C1_Send_Byte
#define _MDrv_HWI2C1_Recv_Byte HAL_HWI2C1_Recv_Byte

BOOL MDrv_HWI2C1_Send_Byte(U16 u16PortOffset, U8 u8Data)
{
	return HAL_HWI2C1_Send_Byte(u16PortOffset, u8Data);
}

BOOL MDrv_HWI2C1_Recv_Byte(U16 u16PortOffset, U8 *pData)
{
	return HAL_HWI2C1_Recv_Byte(u16PortOffset, pData);
}


BOOL MDrv_HWI2C1_NoAck(U16 u16PortOffset)
{
    return HAL_HWI2C1_NoAck(u16PortOffset);
}


BOOL MDrv_HWI2C1_Reset(U16 u16PortOffset, BOOL bReset)
{
    return HAL_HWI2C1_Reset(u16PortOffset, bReset);
}


BOOL _MDrv_HWI2C1_GetPortRegOffset(U8 u8Port, U16 *pu16Offset)
{
    HWI2C_DBG_FUNC();

    if(u8Port>=HWI2C_PORTS)
    {
        HWI2C_DBG_ERR("Port index is >= max supported ports %d !\n",HWI2C_PORTS);
        return FALSE;
    }
    HWI2C_DBG_INFO("g_HWI2CPort[%d] = 0x%02X\n", u8Port, (U8)g_HWI2CPort[u8Port]);
    return HAL_HWI2C1_SetPortRegOffset((HAL_HWI2C_PORT)g_HWI2CPort[u8Port],pu16Offset);
}

BOOL _MDrv_HWI2C1_ReadBytes(U8 u8Port, U16 u16SlaveCfg, U32 uAddrCnt, U8 *pRegAddr, U32 uSize, U8 *pData)
{
    U8 u8SlaveID = LOW_BYTE(u16SlaveCfg);
    U16 u16Offset = 0x00;
    U16 u16Dummy = I2C_ACCESS_DUMMY_TIME; // loop dummy
    BOOL bComplete = FALSE;
    U32 uAddrCntBkp,uSizeBkp;
    U8 *pRegAddrBkp,*pDataBkp;

    HWI2C_DBG_FUNC();

    _geState = E_HWI2C_READ_DATA;
    if (!pRegAddr)
        uAddrCnt = 0;
    if (!pData)
        uSize = 0;

    //check support port index
    if(u8Port>=HWI2C_PORTS)
    {
        HWI2C_DBG_ERR("Port index is >= max supported ports %d !\n",HWI2C_PORTS);
        return FALSE;
    }
    //no meaning operation
    if (!uSize)
    {
        HWI2C_DBG_ERR("Read bytes error!\n");
        return FALSE;
    }

    //configure port register offset ==> important
    if(!_MDrv_HWI2C1_GetPortRegOffset(u8Port,&u16Offset))
    {
        HWI2C_DBG_ERR("Port index error!\n");
        return FALSE;
    }

    //start access routines
    uAddrCntBkp = uAddrCnt;
    pRegAddrBkp = pRegAddr;
    uSizeBkp = uSize;
    pDataBkp = pData;
	
    while (u16Dummy--)
    {
        if((g_HWI2CReadMode[u8Port]!=E_HWI2C_READ_MODE_DIRECT) && (uAddrCnt>0)&& (pRegAddr))
        {	
            HAL_HWI2C1_Start(u16Offset);
            //add extral delay by user configuration
            MsOS_DelayTaskUs(g_u32StartDelay);

            if (!_MDrv_HWI2C1_Send_Byte(u16Offset,HWI2C_SET_RW_BIT(FALSE, u8SlaveID)))
                goto end;

            while(uAddrCnt--)
            {
                if (!_MDrv_HWI2C1_Send_Byte(u16Offset,*pRegAddr))
                    goto end;
                pRegAddr++;
            }

            if(g_HWI2CReadMode[u8Port]==E_HWI2C_READ_MODE_DIRECTION_CHANGE_STOP_START)
            {
                HAL_HWI2C1_Stop(u16Offset);
                //add extral delay by user configuration
                MsOS_DelayTaskUs(g_u32StopDelay);
            }
			else
			{
				HAL_HWI2C1_Reset(u16Offset,TRUE);
				HAL_HWI2C1_Reset(u16Offset,FALSE);
			}
        }

        //Very important to add delay to support all clock speeds
        //Strongly recommend that do not remove this delay routine
        HAL_HWI2C1_ExtraDelay(g_u16DelayFactor[u8Port]);
        HAL_HWI2C1_Start(u16Offset);

        //add extral delay by user configuration
        MsOS_DelayTaskUs(g_u32StartDelay);

        if (!_MDrv_HWI2C1_Send_Byte(u16Offset,HWI2C_SET_RW_BIT(TRUE, u8SlaveID)))
            goto end;

        while(uSize)
        {
            ///////////////////////////////////
            //
            //  must set ACK/NAK before read ready
            //
            uSize--;
            if (uSize==0)
                HAL_HWI2C1_NoAck(u16Offset);
            if (_MDrv_HWI2C1_Recv_Byte(u16Offset,pData)==FALSE)
                goto end;
            pData++;
        }
        bComplete = TRUE;

    end:
        HAL_HWI2C1_Stop(u16Offset);
        //add extral delay by user configuration
        MsOS_DelayTaskUs(g_u32StopDelay);
        if(u16Dummy&&(bComplete==FALSE))
        {
            uAddrCnt = uAddrCntBkp;
            pRegAddr = pRegAddrBkp;
            uSize = uSizeBkp;
            pData = pDataBkp;
            continue;
        }
        break;
    }
    _geState = E_HWI2C_IDLE;
	HAL_HWI2C1_Reset(u16Offset,TRUE);
	HAL_HWI2C1_Reset(u16Offset,FALSE);

    return bComplete;
}

BOOL _MDrv_HWI2C1_WriteBytes(U8 u8Port, U16 u16SlaveCfg, U32 uAddrCnt, U8 *pRegAddr, U32 uSize, U8 *pData)
{
    U8 u8SlaveID = LOW_BYTE(u16SlaveCfg);
    U16 u16Offset = 0x00;
    
    U16 u16Dummy = I2C_ACCESS_DUMMY_TIME; // loop dummy
    BOOL bComplete = FALSE;
    U32 uAddrCntBkp,uSizeBkp;
    U8 *pRegAddrBkp,*pDataBkp;

    HWI2C_DBG_FUNC();

    _geState = E_HWI2C_WRITE_DATA;
    if (!pRegAddr)
        uAddrCnt = 0;
    if (!pData)
        uSize = 0;

    //check support port index
    if(u8Port>=HWI2C_PORTS)
    {
        HWI2C_DBG_ERR("Port index is >= max supported ports %d !\n",HWI2C_PORTS);
        return FALSE;
    }
    //no meaning operation
    if (!uSize)
    {
        HWI2C_DBG_ERR("Write bytes error!\n");
        return FALSE;
    }

    //configure port register offset ==> important
    if(!_MDrv_HWI2C1_GetPortRegOffset(u8Port,&u16Offset))
    {
        HWI2C_DBG_ERR("Port index error!\n");
        return FALSE;
    }

    //start access routines
    uAddrCntBkp = uAddrCnt;
    pRegAddrBkp = pRegAddr;
    uSizeBkp = uSize;
    pDataBkp = pData;
    while(u16Dummy--)
    {
        HAL_HWI2C1_Start(u16Offset);
        MsOS_DelayTaskUs(g_u32StartDelay);

        if (!_MDrv_HWI2C1_Send_Byte(u16Offset,HWI2C_SET_RW_BIT(FALSE, u8SlaveID)))
        	{HWI2C_DBG_ERR("HWI2C_SET_RW_BIT error!\n");
            goto end;
        	}
        while(uAddrCnt)
        {
            if (!_MDrv_HWI2C1_Send_Byte(u16Offset, *pRegAddr))
            	{HWI2C_DBG_ERR("pRegAddr error!\n");
                goto end;
            	}
            uAddrCnt--;
            pRegAddr++;
        }

        while(uSize)
        {
            if (!_MDrv_HWI2C1_Send_Byte(u16Offset, *pData))
            	{HWI2C_DBG_ERR("pData error!\n");
                goto end;
            	}
            uSize--;
            pData++;
        }
        bComplete = TRUE;

    end:
        HAL_HWI2C1_Stop(u16Offset);
        //add extral delay by user configuration
        MsOS_DelayTaskUs(g_u32StopDelay);
        if(u16Dummy&&(bComplete==FALSE))
        {
            uAddrCnt = uAddrCntBkp;
            pRegAddr = pRegAddrBkp;
            uSize = uSizeBkp;
            pData = pDataBkp;
            continue;
        }
        break;
    }
    _geState = E_HWI2C_IDLE;
	HAL_HWI2C1_Reset(u16Offset,TRUE);
	HAL_HWI2C1_Reset(u16Offset,FALSE);
    return bComplete;
}

static BOOL _MDrv_HWI2C1_SelectPort(HWI2C_PORT ePort)
{
    U16 u16Offset = 0x00;
    U8 u8Port = 0x00;
    BOOL bRet=TRUE;
    
    HWI2C_DBG_FUNC();

    //(1) Get port index by port number 
    if(HAL_HWI2C1_GetPortIdxByPort((HAL_HWI2C_PORT)ePort,&u8Port))
    {
        g_HWI2CPortIdx = u8Port;
        g_HWI2CPort[u8Port] = ePort;
        //set default port parameters
        g_HWI2CReadMode[u8Port] = E_HWI2C_READ_MODE_DIRECTION_CHANGE;
        g_u16DelayFactor[u8Port] = 0;
        g_bDMAEnable[u8Port] = FALSE;
    }
    else
    {
        g_HWI2CPortIdx = u8Port;
        //do nothing
        return FALSE;
    }
    HWI2C_DBG_INFO("ePort:0x%02X, u8Port:0x%02X\n",(U8)ePort, u8Port);

    //(2) Set pad mux for port number 
    HAL_HWI2C1_SelectPort((HAL_HWI2C_PORT)ePort);

    //(3) configure port register offset ==> important
    bRet &= _MDrv_HWI2C1_GetPortRegOffset(u8Port,&u16Offset);

    //(4) master init
    bRet &= HAL_HWI2C1_Master_Enable(u16Offset);

    return bRet;
}

static BOOL _MDrv_HWI2C1_SetClk(U8 u8Port, HWI2C_CLKSEL eClk)
{
    U16 u16Offset = 0x00;

    HWI2C_DBG_FUNC();
    HWI2C_DBG_INFO("Port%d clk: %u\n", u8Port, eClk);
    //check support port index
    if(u8Port>=HWI2C_PORTS)
    {
        HWI2C_DBG_ERR("Port index is >= max supported ports %d !\n",HWI2C_PORTS);
        return FALSE;
    }
    //check support clock speed
    if (eClk >= E_HWI2C_NOSUP)
    {
        HWI2C_DBG_ERR("Clock [%u] is not supported!\n", eClk);
        return FALSE;
    }

    //configure port register offset ==> important
    if(!_MDrv_HWI2C1_GetPortRegOffset(u8Port,&u16Offset))
    {
        HWI2C_DBG_ERR("Port index error!\n");
        return FALSE;
    }

    g_u16DelayFactor[u8Port] = (U16)(1<<(eClk));
    return HAL_HWI2C1_SetClk(u16Offset,(HAL_HWI2C_CLKSEL)eClk);
}

static BOOL _MDrv_HWI2C1_SetReadMode(U8 u8Port, HWI2C_ReadMode eReadMode)
{
    HWI2C_DBG_FUNC();
    HWI2C_DBG_INFO("Port%d Readmode: %u\n", u8Port, eReadMode);
    //check support port index
    if(u8Port>=HWI2C_PORTS)
    {
        HWI2C_DBG_ERR("Port index is >= max supported ports %d !\n",HWI2C_PORTS);
        return FALSE;
    }
    if(eReadMode>=E_HWI2C_READ_MODE_MAX)
        return FALSE;
    g_HWI2CReadMode[u8Port] = eReadMode;
    return TRUE;
}

static BOOL _MDrv_HWI2C1_InitPort(HWI2C_UnitCfg *psCfg)
{
    U8 u8PortIdx = 0, u8Port = 0;
    U16 u16Offset = 0x00;
    BOOL bRet = TRUE;
    HWI2C_PortCfg stPortCfg;
        
    HWI2C_DBG_FUNC();

    //(1) set default value for port variables
    for(u8PortIdx=0; u8PortIdx < HWI2C_PORTS; u8PortIdx++)
    {
        stPortCfg = psCfg->sCfgPort[u8PortIdx];
        if(stPortCfg.bEnable)
        {
            if(_MDrv_HWI2C1_SelectPort(stPortCfg.ePort))
            {
                //set clock speed
                bRet &= _MDrv_HWI2C1_SetClk(g_HWI2CPortIdx, stPortCfg.eSpeed);
                //set read mode
                bRet &= _MDrv_HWI2C1_SetReadMode(g_HWI2CPortIdx, stPortCfg.eReadMode);
                //get port index
                bRet &= HAL_HWI2C1_GetPortIdxByPort((HAL_HWI2C_PORT)stPortCfg.ePort,&u8Port);
                //configure port register offset ==> important
                bRet &= _MDrv_HWI2C1_GetPortRegOffset(u8Port,&u16Offset);
                //master init
                bRet &= HAL_HWI2C1_Master_Enable(u16Offset);
                g_bDMAEnable[u8Port] = stPortCfg.bDmaEnable;
                //dump port information
                HWI2C_DBG_INFO("Port:%u Index=%u\n",stPortCfg.ePort,u8Port);
                HWI2C_DBG_INFO("Enable=%u\n",stPortCfg.bEnable);
                HWI2C_DBG_INFO("DmaReadMode:%u\n",stPortCfg.eReadMode);
                HWI2C_DBG_INFO("Speed:%u\n",stPortCfg.eSpeed);
                HWI2C_DBG_INFO("DmaEnable:%u\n",stPortCfg.bDmaEnable);
                HWI2C_DBG_INFO("DmaAddrMode:%u\n",stPortCfg.eDmaAddrMode);
                HWI2C_DBG_INFO("DmaMiuCh:%u\n",stPortCfg.eDmaMiuCh);
                HWI2C_DBG_INFO("DmaMiuPri:%u\n",stPortCfg.eDmaMiuPri);
                HWI2C_DBG_INFO("DmaPhyAddr:%lx\n",stPortCfg.u32DmaPhyAddr);
            }
        }
    }

    //(2) check initialized port : override above port configuration
    if(_MDrv_HWI2C1_SelectPort(psCfg->ePort))
    {
        //set clock speed
        bRet &=_MDrv_HWI2C1_SetClk(g_HWI2CPortIdx,psCfg->eSpeed);
        //set read mode
        bRet &=_MDrv_HWI2C1_SetReadMode(g_HWI2CPortIdx,psCfg->eReadMode);
        //get port index
        bRet &= HAL_HWI2C1_GetPortIdxByPort((HAL_HWI2C_PORT)psCfg->ePort,&u8Port);
        //configure port register offset ==> important
        bRet &= _MDrv_HWI2C1_GetPortRegOffset(u8Port,&u16Offset);
        //master init
        bRet &= HAL_HWI2C1_Master_Enable(u16Offset);
    }

    //(3) dump allocated port information 
    for(u8PortIdx=0; u8PortIdx < HWI2C_PORTS; u8PortIdx++)
    {
        HWI2C_DBG_INFO("HWI2C Allocated Port[%d] = 0x%02X\n",u8PortIdx,g_HWI2CPort[u8PortIdx]);
    }
  
    return bRet;
}

////////////////////////////////////////////////////////////////////////////////
// Global Functions
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: MDrv_HWI2C1_Init
/// @brief \b Function  \b Description: Init HWI2C driver
/// @param psCfg        \b IN: hw I2C config
/// @return             \b TRUE: Success FALSE: Fail
////////////////////////////////////////////////////////////////////////////////
BOOL MDrv_HWI2C1_Init(HWI2C_UnitCfg *psCfg)
{
    BOOL bRet = TRUE;
    U32 u32BaseAddr = REG_ARM_BASE;//, u32BaseSize;
    U8 u8Port=0;

    HWI2C_DBG_FUNC();

	#if 0
    //(1) get & set io base
    if ((!psCfg) || (!MDrv_MMIO_GetBASE(&u32BaseAddr, &u32BaseSize, MS_MODULE_HWI2C)))
        return FALSE;
	#endif
	
    HAL_HWI2C1_SetIOMapBase(u32BaseAddr);

    //(2) Initialize pad mux and basic settings
    HWI2C_DBG_INFO("Pinreg:%lx bit:%u enable:%u speed:%u\n",psCfg->sI2CPin.u32Reg, psCfg->sI2CPin.u8BitPos, psCfg->sI2CPin.bEnable,psCfg->eSpeed);
    bRet &= HAL_HWI2C1_Init_Chip();
    //(3) Initialize all port
    bRet &= _MDrv_HWI2C1_InitPort(psCfg);
    //(4) Check final result
    if (!bRet)
    {
        HWI2C_DBG_ERR("I2C init fail!\n");
    }
    
    //(5) Extra procedure to do after initialization
    HAL_HWI2C1_Init_ExtraProc();

    _gsI2CInfo.u32IOMap = u32BaseAddr;
    g_u32StartDelay = HWI2C_STARTDLY; 
    g_u32StopDelay = HWI2C_STOPDLY; 
    HWI2C_DBG_INFO("START default delay %d(us)\n",(int)g_u32StartDelay);
    HWI2C_DBG_INFO("STOP default delay %d(us)\n",(int)g_u32StopDelay);
    memcpy(&_gsI2CInfo.sUnitCfg, psCfg, sizeof(HWI2C_UnitCfg));
    _gbInit = TRUE;

    HWI2C_DBG_INFO("HWI2C_MUTEX_CREATE!\n");
    for(u8Port=0;u8Port<(U8)HWI2C_PORTS;u8Port++)
    {
        HWI2C_MUTEX_CREATE(u8Port);
    }
    HWI2C_DBG_INFO("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&MDrv_HWI2C1_Init&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");
    return bRet;
}

void MDrv_HW_IIC1_Init(void)
{
	HWI2C_UnitCfg pHwbuscfg[1];
	U8 j;
	
	memset(pHwbuscfg, 0, sizeof(HWI2C_UnitCfg));

    for(j = 0 ; j < 1 ; j++)
    {
    	
        pHwbuscfg[0].sCfgPort[j].bEnable = TRUE;
        pHwbuscfg[0].sCfgPort[j].ePort = E_HAL_HWI2C_PORT0_0;
        pHwbuscfg[0].sCfgPort[j].eSpeed= E_HAL_HWI2C_CLKSEL_NORMAL; //E_HAL_HWI2C1_CLKSEL_VSLOW;//pIIC_Param->u8ClockIIC;//
        pHwbuscfg[0].sCfgPort[j].eReadMode = E_HWI2C_READ_MODE_DIRECTION_CHANGE;//pIIC_Param->u8IICReadMode;//
        pHwbuscfg[0].sCfgPort[j].bDmaEnable = FALSE;  //Use default setting
        pHwbuscfg[0].sCfgPort[j].eDmaAddrMode = E_HWI2C_DMA_ADDR_NORMAL;  //Use default setting
        pHwbuscfg[0].sCfgPort[j].eDmaMiuPri = E_HWI2C_DMA_PRI_LOW;  //Use default setting
        pHwbuscfg[0].sCfgPort[j].eDmaMiuCh = E_HWI2C_DMA_MIU_CH0;  //Use default setting
        pHwbuscfg[0].sCfgPort[j].u32DmaPhyAddr = 0x00000000;  //Use default setting
        j++;
    }

	pHwbuscfg[0].sI2CPin.bEnable = FALSE;
	pHwbuscfg[0].sI2CPin.u8BitPos = 0;
	pHwbuscfg[0].sI2CPin.u32Reg = 0;
	pHwbuscfg[0].eSpeed = E_HAL_HWI2C_CLKSEL_NORMAL; //E_HAL_HWI2C1_CLKSEL_VSLOW;//pIIC_Param->u8ClockIIC;//
    pHwbuscfg[0].ePort = E_HAL_HWI2C_PORT0_0;          /// port
    pHwbuscfg[0].eReadMode = E_HWI2C_READ_MODE_DIRECTION_CHANGE;//pIIC_Param->u8IICReadMode;//

    MDrv_HWI2C1_Init(&pHwbuscfg[0]);
}

EXPORT_SYMBOL(MDrv_HW_IIC1_Init);
////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: MDrv_HWI2C1_Start
/// @brief \b Function  \b Description: send start bit
/// @return             \b TRUE: Success FALSE: Fail
////////////////////////////////////////////////////////////////////////////////
BOOL MDrv_HWI2C1_Start(U16 u16PortOffset)
{
    HWI2C_DBG_FUNC();
    return HAL_HWI2C1_Start(u16PortOffset);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: MDrv_HWI2C1_Stop
/// @brief \b Function  \b Description: send stop bit
/// @return             \b TRUE: Success FALSE: Fail
////////////////////////////////////////////////////////////////////////////////
BOOL MDrv_HWI2C1_Stop(U16 u16PortOffset)
{
    HWI2C_DBG_FUNC();
    return HAL_HWI2C1_Stop(u16PortOffset);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: MDrv_HWI2C1_GetPortIndex
/// @brief \b Function  \b Description: Get port index from port number
/// @param ePort        \b IN: port number
/// @param ePort        \b OUT: pointer to port index
/// @return             \b U8: Port Index
////////////////////////////////////////////////////////////////////////////////
BOOL MDrv_HWI2C1_GetPortIndex(HWI2C_PORT ePort, U8* pu8Port)
{
    BOOL bRet=TRUE;
    
    HWI2C_DBG_FUNC();

    //(1) Get port index by port number 
    bRet &= HAL_HWI2C1_GetPortIdxByPort((HAL_HWI2C_PORT)ePort, pu8Port);
    HWI2C_DBG_INFO("ePort:0x%02X, u8Port:0x%02X\n",(U8)ePort,(U8)*pu8Port);

    return bRet;
}

//###################
//
//  Multi-Port Support: Port 0
//
//###################
////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: MDrv_HWI2C1_SelectPort
/// @brief \b Function  \b Description: Decide port index and pad mux for port number
/// @param ePort        \b IN: port number
/// @return             \b TRUE: Success FALSE: Fail
////////////////////////////////////////////////////////////////////////////////
BOOL MDrv_HWI2C1_SelectPort(HWI2C_PORT ePort)
{
    HWI2C_DBG_FUNC();
    if(ePort >= E_HWI2C_PORT_NOSUP)
        return FALSE;
    return _MDrv_HWI2C1_SelectPort(ePort);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: MDrv_HWI2C1_SetClk
/// @brief \b Function  \b Description: Set HW I2C clock
/// @param eClk         \b IN: clock rate
/// @return             \b TRUE: Success FALSE: Fail
////////////////////////////////////////////////////////////////////////////////
BOOL MDrv_HWI2C1_SetClk(HWI2C_CLKSEL eClk)
{
    HWI2C_DBG_FUNC();
    return _MDrv_HWI2C1_SetClk(g_HWI2CPortIdx, eClk);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: MDrv_HWI2C1_SetReadMode
/// @brief \b Function  \b Description: Set HW I2C Read Mode
/// @param eClk         \b IN: ReadMode
/// @return             \b TRUE: Success FALSE: Fail
////////////////////////////////////////////////////////////////////////////////
BOOL MDrv_HWI2C1_SetReadMode(HWI2C_ReadMode eReadMode)
{
    return _MDrv_HWI2C1_SetReadMode(g_HWI2CPortIdx, eReadMode);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: MDrv_HWI2C1_ReadByte
/// @brief \b Function  \b Description: read 1 byte data
/// @param u16SlaveCfg  \b IN: [15:8]: Channel number [7:0]:Slave ID
/// @param u8RegAddr    \b IN: target register address
/// @param pData        \b Out: read 1 byte data
/// @return             \b TRUE: Success FALSE: Fail
////////////////////////////////////////////////////////////////////////////////
BOOL MDrv_HWI2C1_ReadByte(U16 u16SlaveCfg, U8 u8RegAddr, U8 *pData)
{
    HWI2C_DBG_FUNC();
    return MDrv_HWI2C1_ReadBytes(u16SlaveCfg, 1, &u8RegAddr, 1, pData);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: MDrv_HWI2C1_ReadByte
/// @brief \b Function  \b Description: write 1 byte data
/// @param u16SlaveCfg  \b IN: [15:8]: Channel number [7:0]:Slave ID
/// @param u8RegAddr    \b IN: target register address
/// @param u8Data       \b IN: 1 byte data
/// @return             \b TRUE: Success FALSE: Fail
////////////////////////////////////////////////////////////////////////////////
BOOL MDrv_HWI2C1_WriteByte(U16 u16SlaveCfg, U8 u8RegAddr, U8 u8Data)
{
    HWI2C_DBG_FUNC();
    return MDrv_HWI2C1_WriteBytes(u16SlaveCfg, 1, &u8RegAddr, 1, &u8Data);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: MDrv_HWI2C1_WriteBytes
/// @brief \b Function  \b Description: Init HWI2C driver
/// @param u16SlaveCfg  \b IN: [15:8]: Channel number [7:0]:Slave ID
/// @param uAddrCnt     \b IN: register address count
/// @param pRegAddr     \b IN: pointer to targert register address
/// @param uSize        \b IN: data length
/// @param pData        \b IN: data array
/// @return             \b TRUE: Success FALSE: Fail
////////////////////////////////////////////////////////////////////////////////
BOOL MDrv_HWI2C1_WriteBytes(U16 u16SlaveCfg, U32 uAddrCnt, U8 *pRegAddr, U32 uSize, U8 *pData)
{
    BOOL bRet;
    U8 u8Port;

    u8Port = HIGH_BYTE(u16SlaveCfg);
	u8Port = g_HWI2CPortIdx;
    if(u8Port>=HWI2C_PORTS)
    {
        HWI2C_DBG_ERR("Port index is >= max supported ports %d !\n",HWI2C_PORTS);
        return FALSE;
    }
    HWI2C_MUTEX_LOCK(u8Port);
    bRet = _MDrv_HWI2C1_WriteBytes(u8Port,u16SlaveCfg,uAddrCnt,pRegAddr,uSize,pData);
    HWI2C_MUTEX_UNLOCK(u8Port);
    return bRet;
}
EXPORT_SYMBOL(MDrv_HWI2C1_WriteBytes);

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: MDrv_HWI2C1_ReadBytes
/// @brief \b Function  \b Description: Init HWI2C driver
/// @param u16SlaveCfg  \b IN: [15:8]: Channel number [7:0]:Slave ID
/// @param uAddrCnt     \b IN: register address count
/// @param pRegAddr     \b IN: pointer to targert register address
/// @param uSize        \b IN: data length
/// @param pData        \b Out: read data aray
/// @return             \b TRUE: Success FALSE: Fail
////////////////////////////////////////////////////////////////////////////////
BOOL MDrv_HWI2C1_ReadBytes(U16 u16SlaveCfg, U32 uAddrCnt, U8 *pRegAddr, U32 uSize, U8 *pData)
{
    BOOL bRet;
    U8 u8Port;

    u8Port = HIGH_BYTE(u16SlaveCfg);
	u8Port = g_HWI2CPortIdx;
    if(u8Port>=HWI2C_PORTS)
    {
        HWI2C_DBG_ERR("Port index is >= max supported ports %d !\n",HWI2C_PORTS);
        return FALSE;
    }
    HWI2C_MUTEX_LOCK(u8Port);
    bRet = _MDrv_HWI2C1_ReadBytes(u8Port,u16SlaveCfg,uAddrCnt,pRegAddr,uSize,pData);
    HWI2C_MUTEX_UNLOCK(u8Port);
    return bRet;
}
EXPORT_SYMBOL(MDrv_HWI2C1_ReadBytes);

