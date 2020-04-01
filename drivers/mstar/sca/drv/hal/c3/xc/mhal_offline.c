////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2009 MStar Semiconductor, Inc.
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
#define  MHAL_OFFLINE_C


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#if defined(MSOS_TYPE_LINUX_KERNEL)
#include <linux/wait.h>
#endif

// Common Definition
#include "MsCommon.h"
#include "mhal_xc_chip_config.h"

// Registers
#include "drvXC_IOPort.h"
#include "xc_hwreg_utility2.h"
#include "hwreg_sc.h"
#include "hwreg_sc1.h"
#include "hwreg_sc2.h"

#include "xc_Analog_Reg.h"
#include "mhal_mux.h"
#include "mhal_offline.h"
#include "mhal_mux.h"
#include "drvXC_IOPort.h"


//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define    AIS_HalDebug(x)    //x

//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------
void Hal_XC_WaitForHVCleared_AV(MS_U8 Channel)
{
    MS_U16 u16Hcnt, u16Vcnt;
    MS_U16 u16Count=0;

    u16Hcnt = Hal_XC_GetOffLineOfH();
    u16Vcnt = Hal_XC_GetOffLineOfV();
    while((u16Hcnt != 0x3fff)&&(u16Vcnt !=0x7ff)
            &&(u16Hcnt != 0)&&(u16Vcnt !=0))
    {
        AIS_HalDebug(printf("u16Count=%u, Channel =%u, MApi_XC_GetOffLineDetection: vcnt=%x hcnt=%x\n", u16Count,Channel, u16Vcnt,u16Hcnt );)
        u16Count++;
        MsOS_DelayTask(10);
        u16Hcnt = Hal_XC_GetOffLineOfH();
        u16Vcnt = Hal_XC_GetOffLineOfV();
        AIS_HalDebug(printf("u16Count=%u, Channel =%u, MApi_XC_GetOffLineDetection: vcnt=%x hcnt=%x\n", u16Count++,Channel, u16Vcnt,u16Hcnt );)
        if(u16Count>=0x100)
        {
            AIS_HalDebug(printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");)
            AIS_HalDebug(printf("!!!!!!!!!!!!!!!!!!!!!!Hal_XC_WaitForHVCleared(): reset fail!!!!!!!!!!!!!!!!!\n");)
            AIS_HalDebug(printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");)
            return;
        }
    }
}

void Hal_XC_WaitForHVCleared(MS_U8 Channel)
{
    MS_U16 u16Hcnt, u16Vcnt;
    MS_U16 u16Count=0;

    u16Hcnt = Hal_XC_GetOffLineOfH();
    u16Vcnt = Hal_XC_GetOffLineOfV();
    while((u16Hcnt != 0x3fff)&&(u16Vcnt !=0x7ff)
           &&(u16Hcnt != 0)&&(u16Vcnt !=0))
    {
        AIS_HalDebug(printf("u16Count=%u, Channel =%u, MApi_XC_GetOffLineDetection: vcnt=%x hcnt=%x\n", u16Count,Channel, u16Vcnt,u16Hcnt );)
        u16Count++;
        //reset offline ip
        SC_W2BYTEMSK( REG_SC_BK00_02_L, BIT(5), 0xFF );
        MsOS_DelayTask(1);
        SC_W2BYTEMSK( REG_SC_BK00_02_L, 0x00, 0xFF );

        MsOS_DelayTask(10);
        u16Hcnt = Hal_XC_GetOffLineOfH();
        u16Vcnt = Hal_XC_GetOffLineOfV();
        AIS_HalDebug(printf("u16Count=%u, Channel =%u, MApi_XC_GetOffLineDetection: vcnt=%x hcnt=%x\n", u16Count++,Channel, u16Vcnt,u16Hcnt );)
        if(u16Count>=0x100)
        {
            AIS_HalDebug(printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");)
            AIS_HalDebug(printf("!!!!!!!!!!!!!!!!!!!!!!Hal_XC_WaitForHVCleared(): reset fail!!!!!!!!!!!!!!!!!\n");)
            AIS_HalDebug(printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");)
            return;
        }
    }
}

void Hal_XC_SetOffLineToSog_AV( MS_U8 Channel )
{
    W2BYTEMSK(L_BK_ADC_ATOP(0x3C), 0, BIT(13) );
    OS_DELAY_TASK(1);
    W2BYTEMSK(L_BK_ADC_ATOP(0x3C), BIT(13), BIT(13) );
    SC_W2BYTE(REG_SC_BK13_02_L, 0x0071);
    Hal_XC_TurnOnDetectClkPath(SC_SUBWIN_IPMUX_MLINK);

    MDrv_Write2Byte(L_BK_ADC_ATOP(0x04), 0);
    MDrv_Write2Byte(L_BK_ADC_ATOP(0x05), 0);
    MDrv_Write2Byte(L_BK_ADC_ATOP(0x06), 0);
    MDrv_Write2ByteMask(L_BK_ADC_ATOP(0x5e), 0, 0x3fc);
    MDrv_WriteByte(L_BK_ADC_ATOP(0x3d), (MDrv_ReadByte(L_BK_ADC_ATOP(0x3d)) & 0xf0) | (Channel + 3));
}
//------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------
void Hal_XC_SetOffLineToSog_YUV( MS_U8 Channel )
{
    W2BYTEMSK(L_BK_ADC_ATOP(0x3C), 0, BIT(13) );
    OS_DELAY_TASK(1);
    W2BYTEMSK(L_BK_ADC_ATOP(0x3C), BIT(13), BIT(13) );
    SC_W2BYTE(REG_SC_BK13_02_L, 0x0071);
    Hal_XC_TurnOnDetectClkPath(SC_SUBWIN_IPMUX_MLINK);

    MDrv_Write2Byte(L_BK_ADC_ATOP(0x04), 0);
    MDrv_Write2Byte(L_BK_ADC_ATOP(0x05), 0);
    MDrv_Write2Byte(L_BK_ADC_ATOP(0x06), 0);
    MDrv_Write2ByteMask(L_BK_ADC_ATOP(0x5e), 0, 0x3fc);
    MDrv_WriteByte(L_BK_ADC_ATOP(0x3d), (MDrv_ReadByte(L_BK_ADC_ATOP(0x3d)) & 0xf0) | Channel);
}
//------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------
void Hal_XC_SetOffLineToHv( MS_U8 Channel )
{
    W2BYTEMSK(L_BK_ADC_ATOP(0x3C), 0, BIT(13) );
    OS_DELAY_TASK(1);
    W2BYTEMSK(L_BK_ADC_ATOP(0x3C), BIT(13), BIT(13) );
    SC_W2BYTE(REG_SC_BK13_02_L,0x0001);
    Hal_XC_TurnOnDetectClkPath(SC_SUBWIN_IPMUX_MLINK);
    MDrv_Write2Byte(L_BK_ADC_ATOP(0x04), 0);
    MDrv_Write2Byte(L_BK_ADC_ATOP(0x05), 0);
    MDrv_Write2Byte(L_BK_ADC_ATOP(0x06), 0);
    MDrv_Write2ByteMask(L_BK_ADC_ATOP(0x5e), 0, 0x3fc);
    MDrv_WriteByte(L_BK_ADC_ATOP(0x01), (MDrv_ReadByte(L_BK_ADC_ATOP(0x01)) & 0xf3) | (Channel << 2));
}

//------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------
void Hal_XC_SetOffLineToHDMI( MS_U8 Channel )
{
#if 0
     //Reg0x25c1/0x25c0 =0x0000
     MDrv_WriteByte(L_BK_ADC_ATOP(0x60), 0x00);
     MDrv_WriteByte(H_BK_ADC_ATOP(0x60), 0x00);
     //Reg0x25d3/0x25d2 =0x0000
     MDrv_WriteByte(L_BK_ADC_ATOP(0x69), 0x00);
     MDrv_WriteByte(H_BK_ADC_ATOP(0x69), 0x00);
     //Reg0x250d_bit7=0;
     MDrv_WriteRegBit(H_BK_ADC_ATOP(0x06), DISABLE, BIT(7));
#endif
}

//------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------
void Hal_XC_SetOffLineToUSB( MS_U8 Channel )
{
    //Need to do nothing
}

//------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------
MS_U16 Hal_XC_GetOffLineOfH(void)
{
    MS_U16 u16Hcnt;
    /*MS_U8 u8Bank;

    u8Bank = MDrv_ReadByte(BK_SELECT_00);
    MDrv_WriteByte(BK_SELECT_00, REG_BANK_OFFLINE);
    u16Hcnt = MDrv_Read2Byte(L_BK_OFFLINE(0x20)) & 0x3fff;
    MDrv_WriteByte(BK_SELECT_00, u8Bank);*/
    u16Hcnt = SC_R2BYTE(REG_SC_BK13_20_L) & 0x3fff;
    return u16Hcnt;
}

//------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------
MS_U16 Hal_XC_GetOffLineOfV(void)
{
    MS_U16 u16Vcnt;
    /*MS_U8 u8Bank;

    u8Bank = MDrv_ReadByte(BK_SELECT_00);
    MDrv_WriteByte(BK_SELECT_00, REG_BANK_OFFLINE);
    u16Vcnt = MDrv_Read2Byte(L_BK_OFFLINE(0x1f)) & 0x07ff;
    MDrv_WriteByte(BK_SELECT_00, u8Bank);*/
    u16Vcnt = SC_R2BYTE(REG_SC_BK13_1F_L) & 0x07ff;
    return u16Vcnt;
}

//------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------
MS_U16 Hal_XC_GetOffLineOfDVI01(void)
{
#if 0
    return (MDrv_ReadByte(H_BK_ADC_DTOP(0x57)) & 0x08);
#else
    return 1;
#endif
}

//------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------
MS_U16 Hal_XC_GetOffLineOfDVI23(void)
{
#if 0
    return (MDrv_ReadByte(H_BK_ADC_DTOP(0x58)) & 0x08);
#else
    return 1;
#endif
}

//------------------------------------------------------------------------------------------------
// Return HW connections status of UHCx
//------------------------------------------------------------------------------------------------
MS_U16 Hal_XC_GetOffLineOfStorage(void)
{
    return ((MDrv_ReadByte(REG_UHC0_BASE + 0x30) & 0x01) | (MDrv_ReadByte(REG_UHC1_BASE + 0x30) & 0x01));
}

//------------------------------------------------------------------------------------------------
// Function: Switch Clk path
//----
// Input Source Select.
//   0000: ADC A.
//   0001: DVI
//   0010: VD
//   0011: Capture
//   0100: Reserved
//   0101: Ext VD
//   0110: ADC B
//
//------------------------------------------------------------------------------------------------
void   Hal_XC_TurnOnDetectClkPath(MS_U8 u8DetectClkPath)
{
    MDrv_WriteByte(H_BK_IPMUX(0x01), (MDrv_ReadByte(H_BK_IPMUX(0x01)) & 0xf0) | u8DetectClkPath);
}

void Hal_XC_SetOfflineDetectClk(void)
{
    MDrv_WriteByteMask(REG_CKG_IDCLK0, CKG_IDCLK0_XTAL, CKG_IDCLK0_MASK); // initial to XTAL first, will be changed when switch input source
}

MS_BOOL Hal_XC_SetOffLineSogThreshold(MS_U8 u8Threshold)
{
    MDrv_WriteByte(L_BK_ADC_ATOP(0x3e), u8Threshold); //8 bit
    return TRUE;
}

MS_BOOL Hal_XC_SetOffLineSogBW(MS_U8 u8BW)
{
    MS_BOOL bRet = TRUE;
    if(u8BW < 32) // only 5 bit and always >= 0
    {
        bRet = TRUE;
    }
    else
    {
        printf("[Offline Detect]invalid sog filter bandwidth value, use default value:0x0!\n");
        u8BW = 0;
        bRet = FALSE;
    }
    MDrv_WriteByte(L_BK_ADC_ATOP(0x3c), (MDrv_ReadByte(L_BK_ADC_ATOP(0x3c)) & 0xe0) | u8BW);
    return bRet;
}

void Hal_XC_OffLineInit(void)
{
    MDrv_WriteByte(L_BK_ADC_ATOP(0x3f), MDrv_ReadByte(L_BK_ADC_ATOP(0x3f)) & 0xfd);
}

void Hal_XC_OffLineExit(void)
{
    MDrv_WriteByteMask(L_BK_ADC_ATOP(0x3f), BIT(1), BIT(1));
}

MS_U8 Hal_XC_GetOffLineDetection(MS_U8 u8CheckSrc)
{
    if(((u8CheckSrc >= (MS_U8)INPUT_PORT_YMUX_CVBS0) &&
        (u8CheckSrc <= (MS_U8)INPUT_PORT_YMUX_CVBS7))
         ||
        ((u8CheckSrc >= (MS_U8)INPUT_PORT_ANALOG0_SYNC)  &&
        (u8CheckSrc <= (MS_U8)INPUT_PORT_ANALOG4_SYNC))
         ||
        ((u8CheckSrc >= (MS_U8)INPUT_PORT_ANALOG0)  &&
        (u8CheckSrc <= (MS_U8)INPUT_PORT_ANALOG4)))
    {
        MS_U16 u16Hcnt, u16Vcnt;

        u16Hcnt = Hal_XC_GetOffLineOfH();
        u16Vcnt = Hal_XC_GetOffLineOfV();
        AIS_HalDebug(printf("MApi_XC_GetOffLineDetection: vcnt=%x hcnt=%x\n", u16Vcnt,u16Hcnt );)

        if((u16Hcnt == 0x3fff)
            ||(u16Vcnt ==0x7ff)
            ||(u16Hcnt == 0)
            ||(u16Vcnt ==0)
            )
        {
            return 0;        // Nosignal
        }
        else
        {
            return 1;        //Signal in
        }
    }
    else if(((u8CheckSrc >= INPUT_PORT_DVI0) && (u8CheckSrc <= INPUT_PORT_DVI1)) || (u8CheckSrc == INPUT_PORT_DVI3))
    {
        //limitation: we cannot distinguish the difference between HDMI1 and HDMI2
        //Thus, HDMI1 and HDMI2 will be on or off at the same time
        return (Hal_XC_GetOffLineOfDVI01())?0:1;
    }
    else if(u8CheckSrc == INPUT_PORT_DVI2)
    {
        return (Hal_XC_GetOffLineOfDVI23())?0:1;
    }
    else if(u8CheckSrc == INPUT_PORT_MVOP)
    {
        //UHC0_REG_BASE and UHC1_REG_BASE
        return (Hal_XC_GetOffLineOfStorage())?1:0;
    }
    else
    {
        return 0;
    }
}

#undef MHAL_OFFLINE_C
