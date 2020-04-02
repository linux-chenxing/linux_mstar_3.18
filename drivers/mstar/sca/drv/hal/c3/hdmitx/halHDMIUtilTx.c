//<MStar Software>
//******************************************************************************
// MStar Software
// Copyright (c) 2010 - 2012 MStar Semiconductor, Inc. All rights reserved.
// All software, firmware and related documentation herein ("MStar Software") are
// intellectual property of MStar Semiconductor, Inc. ("MStar") and protected by
// law, including, but not limited to, copyright law and international treaties.
// Any use, modification, reproduction, retransmission, or republication of all
// or part of MStar Software is expressly prohibited, unless prior written
// permission has been granted by MStar.
//
// By accessing, browsing and/or using MStar Software, you acknowledge that you
// have read, understood, and agree, to be bound by below terms ("Terms") and to
// comply with all applicable laws and regulations:
//
// 1. MStar shall retain any and all right, ownership and interest to MStar
//    Software and any modification/derivatives thereof.
//    No right, ownership, or interest to MStar Software and any
//    modification/derivatives thereof is transferred to you under Terms.
//
// 2. You understand that MStar Software might include, incorporate or be
//    supplied together with third party`s software and the use of MStar
//    Software may require additional licenses from third parties.
//    Therefore, you hereby agree it is your sole responsibility to separately
//    obtain any and all third party right and license necessary for your use of
//    such third party`s software.
//
// 3. MStar Software and any modification/derivatives thereof shall be deemed as
//    MStar`s confidential information and you agree to keep MStar`s
//    confidential information in strictest confidence and not disclose to any
//    third party.
//
// 4. MStar Software is provided on an "AS IS" basis without warranties of any
//    kind. Any warranties are hereby expressly disclaimed by MStar, including
//    without limitation, any warranties of merchantability, non-infringement of
//    intellectual property rights, fitness for a particular purpose, error free
//    and in conformity with any international standard.  You agree to waive any
//    claim against MStar for any loss, damage, cost or expense that you may
//    incur related to your use of MStar Software.
//    In no event shall MStar be liable for any direct, indirect, incidental or
//    consequential damages, including without limitation, lost of profit or
//    revenues, lost or damage of data, and unauthorized system use.
//    You agree that this Section 4 shall still apply without being affected
//    even if MStar Software has been modified by MStar in accordance with your
//    request or instruction for your use, except otherwise agreed by both
//    parties in writing.
//
// 5. If requested, MStar may from time to time provide technical supports or
//    services in relation with MStar Software to you for your use of
//    MStar Software in conjunction with your or your customer`s product
//    ("Services").
//    You understand and agree that, except otherwise agreed by both parties in
//    writing, Services are provided on an "AS IS" basis and the warranty
//    disclaimer set forth in Section 4 above shall apply.
//
// 6. Nothing contained herein shall be construed as by implication, estoppels
//    or otherwise:
//    (a) conferring any license or right to use MStar name, trademark, service
//        mark, symbol or any other identification;
//    (b) obligating MStar or any of its affiliates to furnish any person,
//        including without limitation, you and your customers, any assistance
//        of any kind whatsoever, or any information; or
//    (c) conferring any license or right under any intellectual property right.
//
// 7. These terms shall be governed by and construed in accordance with the laws
//    of Taiwan, R.O.C., excluding its conflict of law rules.
//    Any and all dispute arising out hereof or related hereto shall be finally
//    settled by arbitration referred to the Chinese Arbitration Association,
//    Taipei in accordance with the ROC Arbitration Law and the Arbitration
//    Rules of the Association by three (3) arbitrators appointed in accordance
//    with the said Rules.
//    The place of arbitration shall be in Taipei, Taiwan and the language shall
//    be English.
//    The arbitration award shall be final and binding to both parties.
//
//******************************************************************************
//<MStar Software>
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

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    halHDMIUtilTx.c
/// @author MStar Semiconductor Inc.
/// @brief  HDMITx Utility HAL
///////////////////////////////////////////////////////////////////////////////////////////////////

#define  MHAL_HDMIUTILTX_C

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------

#include "MsCommon.h"
#include "halHDMIUtilTx.h"
#include "regHDMITx.h"

//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

static MS_U32 _gHDMITx_MapBase = 0;
static MS_U32 _gPM_MapBase = 0;

#if defined(MSOS_TYPE_LINUX_KERNEL)
#define REG(bank, addr)              READ_WORD( ((_gPM_MapBase+(bank<<1)) + ((addr)<<2)))
#define PMREG(bank, addr)            READ_WORD( ((_gPM_MapBase+(bank<<1)) + ((addr)<<2)))
#else
#define REG(bank, addr)              (*((volatile MS_U16 *)((_gPM_MapBase+(bank<<1)) + ((addr)<<2))))
#define PMREG(bank, addr)            (*((volatile MS_U16 *)((_gPM_MapBase+(bank<<1)) + ((addr)<<2))))
#endif


//++ start for IIC
#define PIN_HIGH                        1
#define PIN_LOW                         0

#define I2C_ACKNOWLEDGE                 PIN_LOW
#define I2C_NON_ACKNOWLEDGE             PIN_HIGH

#define i2cSetSCL(pin_state)            ( REG(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_01) = (REG(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_01) & (~BIT0)) | (pin_state << 0) )
#define i2cSetSDA(pin_state)            ( REG(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_01) = (REG(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_01) & (~BIT4)) | (pin_state << 4) )
#define i2cSCL_PIN_STATUS               ( (REG(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_01) & BIT0) >> 0 )
#define i2cSDA_PIN_STATUS               ( (REG(HDMITX_MISC_HDCP_REG_BASE, REG_MISC_CONFIG_01) & BIT4) >> 4 )

#define I2C_CHECK_PIN_TIME              1000 // unit: 1 us
#define I2C_CHECK_PIN_CYCLE             8    // cycle of check pin loopp
#define MCU_MICROSECOND_NOP_NUM         1
#define I2C_CHECK_PIN_DUMMY             100//255 /*((I2C_CHECK_PIN_TIME / I2C_CHECK_PIN_CYCLE) * MCU_MICROSECOND_NOP_NUM)*/
#define I2C_ACCESS_DUMMY_TIME           3

#define I2C_DEVICE_ADR_WRITE(slave_adr)   (slave_adr & ~BIT0)
#define I2C_DEVICE_ADR_READ(slave_adr)    (slave_adr | BIT0)
//--- end of IIC

#define HDMITX_RX74_SLAVE_ADDR          0x74
#define HDMITX_EDIDROM_SLAVE_ADDR       0xA0

//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------

typedef enum _I2cIoTransType
{
    I2C_TRANS_READ,
    I2C_TRANS_WRITE
} I2cIoTransType;

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------
extern MS_BOOL g_bDisableRegWrite;

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
#ifdef MS_DEBUG
static MS_BOOL bDebugUtilFlag = TRUE;
#else
static MS_BOOL bDebugUtilFlag = FALSE;
#endif
static MS_U32 u32DDCDelayCount = 700;

//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------

#define DBG_HDMIUTIL(_f)                  do{ if(bDebugUtilFlag & TRUE) (_f); } while(0);


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
extern MS_BOOL MDrv_EEPROM_Read(MS_U32 u32Addr, MS_U8 *pu8Buf, MS_U32 u32Size);

//****************************************
// i2c_Delay()
//****************************************
void i2c_Delay(void)
{
    /*
     * set HDMITx I2C data rate to 50KHz
     */
    volatile MS_U32 i = u32DDCDelayCount;
    while(i-->0)
    {
        #ifdef __mips__
        __asm__ __volatile__ ("nop");
        #endif

        #ifdef __AEONR2__
        __asm__ __volatile__ ("l.nop");
        #endif

        #ifdef __arm__
        __asm__ __volatile__ ("mov r0, r0");
        #endif
    }
}


/////////////////////////////////////////
// Set I2C SCL pin high/low.
//
// Arguments: bSet - high/low bit
/////////////////////////////////////////
void i2cSetSCL_Chk(MS_U8 bSet)
{
    MS_U8 ucDummy; // loop dummy

    i2cSetSCL(bSet); // set SCL pin

    if (bSet == PIN_HIGH) // if set pin high
    {
        ucDummy = I2C_CHECK_PIN_DUMMY; // initialize dummy
        while ((i2cSCL_PIN_STATUS == PIN_LOW) && (ucDummy--)) ; // check SCL pull high
    }
    else
    {
        ucDummy = I2C_CHECK_PIN_DUMMY; // initialize dummy
        while ((i2cSCL_PIN_STATUS == PIN_HIGH) && (ucDummy--)) ; // check SCL pull low
    }
}

/////////////////////////////////////////
// Set I2C SDA pin high/low
//
// Arguments: bSet - high/low bit
/////////////////////////////////////////
void i2cSetSDA_Chk(MS_U8 bSet)
{
    MS_U8 ucDummy; // loop dummy

    i2cSetSDA(bSet); // set SDA pin

    if (bSet == PIN_HIGH) // if set pin high
    {
        ucDummy = I2C_CHECK_PIN_DUMMY; // initialize dummy
        while ((i2cSDA_PIN_STATUS == PIN_LOW) && (ucDummy--)) ; // check SDA pull high
    }
}

/////////////////////////////////////////
// Set I2C SDA pin as input
//
// Arguments:
/////////////////////////////////////////
void i2cSetSDA_Input(void)
{
    volatile MS_U8 ucDummy = 40; // loop dummy
    MS_BOOL bflag;

    i2cSetSDA(PIN_HIGH); // set SDA pin

    while (ucDummy-- > 0)
    {
        bflag = (i2cSDA_PIN_STATUS == PIN_HIGH) ? TRUE : FALSE;
    }
}

//////////////////////////////////////////////////////
// I2C start signal.
// <comment>
//  SCL ________
//              \_________
//  SDA _____
//           \____________
//
// Return value: None
//////////////////////////////////////////////////////
MS_BOOL i2c_Start(void)
{
    MS_BOOL bStatus = TRUE; // success status
    MS_U32 u32OldIntr;

    //disable all interrupt
    u32OldIntr = MsOS_DisableAllInterrupts();

    i2cSetSDA_Chk(PIN_HIGH);
    i2c_Delay();
    i2cSetSCL_Chk(PIN_HIGH);
    i2c_Delay();

    // check pin error
    if ((i2cSCL_PIN_STATUS == PIN_LOW) || (i2cSDA_PIN_STATUS == PIN_LOW))
    {
        bStatus = FALSE;
        DBG_HDMIUTIL(printf("i2c_Start()::SCL or SDA could not pull low, SCL = %d, SDA= %d\n", i2cSCL_PIN_STATUS, i2cSDA_PIN_STATUS));
    }
    else // success
    {
        i2cSetSDA(PIN_LOW);
        i2c_Delay();
        i2cSetSCL(PIN_LOW);
        i2c_Delay(); //AWU addded
    }

    //restore interrupt
    MsOS_RestoreAllInterrupts(u32OldIntr);

    return bStatus;
}

/////////////////////////////////////////
// I2C stop signal.
// <comment>
//              ____________
//  SCL _______/
//                 _________
//  SDA __________/
/////////////////////////////////////////
void i2c_Stop(void)
{
    MS_U32 u32OldIntr;

    //disable all interrupt
    u32OldIntr = MsOS_DisableAllInterrupts();

    i2cSetSCL(PIN_LOW);
    i2c_Delay();
    i2cSetSDA(PIN_LOW);
    i2c_Delay();
    i2cSetSCL_Chk(PIN_HIGH);
    i2c_Delay();
    i2cSetSDA_Chk(PIN_HIGH);
    i2c_Delay();

    //restore interrupt
    MsOS_RestoreAllInterrupts(u32OldIntr);
}

//////////////////////////////////////////////////////////////////////////
// I2C receive byte from device.
//
// Return value: receive byte
//////////////////////////////////////////////////////////////////////////
MS_U8 i2c_ReceiveByte(MS_U16 bAck)
{
    MS_U8 ucReceive = 0;
    MS_U8 ucMask = 0x80;

    MS_U32 u32OldIntr;

    //disable all interrupt
    u32OldIntr = MsOS_DisableAllInterrupts();
    //i2c_Delay();//AWU added

    while (ucMask)
    {
        //i2cSetSDA(PIN_HIGH);
        //i2cSetSDA_Chk(PIN_HIGH);    //AWU
        i2cSetSDA_Input();
        i2cSetSCL_Chk(PIN_HIGH);
        i2c_Delay();
        if ( i2cSDA_PIN_STATUS == PIN_HIGH )
            ucReceive |= ucMask;
        i2cSetSCL_Chk(PIN_LOW);
        //i2c_Delay();
        ucMask >>= 1; // next
    } // while

    if (bAck) // acknowledge
        i2cSetSDA_Chk(I2C_ACKNOWLEDGE);
    else // non-acknowledge
        i2cSetSDA_Chk(I2C_NON_ACKNOWLEDGE);

    i2c_Delay();
    i2cSetSCL_Chk(PIN_HIGH);
    i2c_Delay();
    i2cSetSCL(PIN_LOW);
    i2c_Delay();

    //restore interrupt
    MsOS_RestoreAllInterrupts(u32OldIntr);

    return ucReceive;
}

//////////////////////////////////////////////////////////////////////////
// I2C send byte to device.
//
// Arguments: uc_val - send byte
// Return value: I2C acknowledge bit
//               I2C_ACKNOWLEDGE/I2C_NON_ACKNOWLEDGE
//////////////////////////////////////////////////////////////////////////
MS_BOOL i2c_SendByte(MS_U8 uc_val)
{
    MS_U8 ucMask = 0x80;
    MS_U8 bAck; // acknowledge bit

    MS_U32 u32OldIntr;

    //disable all interrupt
    u32OldIntr = MsOS_DisableAllInterrupts();

    while (ucMask)
    {
        if (uc_val & ucMask)
            i2cSetSDA_Chk(PIN_HIGH);
        else
            i2cSetSDA_Chk(PIN_LOW);
        i2c_Delay();
        i2cSetSCL_Chk(PIN_HIGH); // clock
        i2c_Delay();
        i2cSetSCL_Chk(PIN_LOW);
        //i2c_Delay();

        ucMask >>= 1; // next
    } // while

    // recieve acknowledge
    i2cSetSDA(PIN_HIGH);
    i2c_Delay();
    i2cSetSCL_Chk(PIN_HIGH);
    i2c_Delay();
    bAck = i2cSDA_PIN_STATUS; // recieve acknowlege
    i2cSetSCL(PIN_LOW);
    i2c_Delay();

    //restore interrupt
    MsOS_RestoreAllInterrupts(u32OldIntr);

    return (bAck);
}

//////////////////////////////////////////////////////////////////////////
// I2C access start.
//
// Arguments: ucSlaveAdr - slave address
//            trans_t - I2C_TRANS_WRITE/I2C_TRANS_READ
//////////////////////////////////////////////////////////////////////////
MS_BOOL i2c_AccessStart(MS_U8 ucSlaveAdr, I2cIoTransType trans_t)
{
    MS_U8 ucDummy; // loop dummy

    if (trans_t == I2C_TRANS_READ) // check i2c read or write
        ucSlaveAdr = I2C_DEVICE_ADR_READ(ucSlaveAdr); // read
    else
        ucSlaveAdr = I2C_DEVICE_ADR_WRITE(ucSlaveAdr); // write

    ucDummy = I2C_ACCESS_DUMMY_TIME;
    while (ucDummy--)
    {
        if (i2c_Start() == FALSE)
            continue;

        if (i2c_SendByte(ucSlaveAdr) == I2C_ACKNOWLEDGE) // check acknowledge
            return TRUE;
        else
        {
            DBG_HDMIUTIL(printf("i2c_AccessStart()::No ACK\n"));
        }

        i2c_Stop();

        //MsOS_DelayTask(1);//delay 1ms
        MsOS_DelayTaskUs(100); // delay 100us
    } // while

    return FALSE;
}

/////////////////////////////////////////////////////////////////
// I2C read bytes from device.
//
// Arguments: ucSlaveAdr - slave address
//            ucSubAdr - sub address
//            pBuf - pointer of buffer
//            ucBufLen - length of buffer
/////////////////////////////////////////////////////////////////
MS_BOOL i2cBurstReadBytes(MS_U8 ucSlaveAdr, MS_U8 ucSubAdr, MS_U8 *pBuf, MS_U16 ucBufLen)
{
    MS_U8 ucDummy; // loop dummy
    MS_BOOL result = FALSE;

    ucDummy = I2C_ACCESS_DUMMY_TIME;
    while (ucDummy--)
    {
        if (i2c_AccessStart(ucSlaveAdr, I2C_TRANS_WRITE) == FALSE)
            continue;

        if (i2c_SendByte(ucSubAdr) == I2C_NON_ACKNOWLEDGE) // check non-acknowledge
        {
            DBG_HDMIUTIL(printf("i2cBurstReadBytes()::No ACK\n"));
            continue;
        }

        //i2c_Stop();//AWU added

        if (i2c_AccessStart(ucSlaveAdr, I2C_TRANS_READ) == FALSE)
            continue;

        while (ucBufLen--) // loop to burst read
        {
            *pBuf = i2c_ReceiveByte(ucBufLen); // receive byte

            //if(*pBuf) DBG_HDMITX(printf("i2cBurstReadBytes()::BINGO\n"));

            pBuf++; // next byte pointer
        } // while
        result = TRUE;
        break;
    } // while

    i2c_Stop();

    return result;
}

/////////////////////////////////////////////////////////////////
// I2C write bytes to device.
//
// Arguments: ucSlaveAdr - slave address
//            ucSubAdr - sub address
//            pBuf - pointer of buffer
//            ucBufLen - length of buffer
/////////////////////////////////////////////////////////////////
MS_BOOL i2cBurstWriteBytes(MS_U8 ucSlaveAdr, MS_U8 ucSubAdr, MS_U8 *pBuf, MS_U8 ucBufLen)
{
    MS_U8 ucDummy; // loop dummy
    MS_BOOL result = FALSE;

    ucDummy = I2C_ACCESS_DUMMY_TIME;
    while (ucDummy--)
    {
        if (i2c_AccessStart(ucSlaveAdr, I2C_TRANS_WRITE) == FALSE)
            continue;

        if (i2c_SendByte(ucSubAdr) == I2C_NON_ACKNOWLEDGE) // check non-acknowledge
        {
            DBG_HDMIUTIL(printf("i2cBurstReadBytes()::No ACK\n"));
            continue;
        }

        while (ucBufLen--) // loop of writting data
        {
            i2c_SendByte(*pBuf); // send byte

            pBuf++; // next byte pointer
        } // while
        result = TRUE;
        break;
    } // while

    i2c_Stop();

    return result;
}

/////////////////////////////////////////////////////////////////
// I2C read a byte from device.
//
// Arguments: ucSlaveAdr - slave address
//            ucSubAdr - sub address
//            pBuf - return buffer point
// Return value: read byte
/////////////////////////////////////////////////////////////////
MS_BOOL i2cReadByte(MS_U8 ucSlaveAdr, MS_U8 ucSubAdr, MS_U8 *pBuf)
{
    return (i2cBurstReadBytes(ucSlaveAdr, ucSubAdr, pBuf, 1));
}

/////////////////////////////////////////////////////////////////
// I2C write a byte from device.
//
// Arguments: ucSlaveAdr - slave address
//            ucSubAdr - sub address
//            uc_val - write byte
/////////////////////////////////////////////////////////////////
MS_BOOL i2cWriteByte(MS_U8 ucSlaveAdr, MS_U8 ucSubAdr, MS_U8 uc_val)
{
    return (i2cBurstWriteBytes(ucSlaveAdr, ucSubAdr, &uc_val, 1));
}


//------------------------------------------------------------------------------
/// @brief Set HDMITx register base address
/// @param[in] u32Base
/// @return None
//------------------------------------------------------------------------------
void MHal_HDMITx_SetIOMapBase(MS_U32 u32Base, MS_U32 u32PMBase)
{
    _gHDMITx_MapBase = u32Base;
    _gPM_MapBase = u32PMBase;
    //DBG_HDMIUTIL(printf("HDMITx IOMap base:%4x Reg offset:%4x\n", u32Base, HDMITX_REG_BASE));
}

MS_BOOL MHal_HDMITx_Rx74WriteByte(MS_U8 addr, MS_U8 value)
{
    return (i2cWriteByte(HDMITX_RX74_SLAVE_ADDR, addr, value));
}

MS_BOOL MHal_HDMITx_Rx74ReadByte(MS_U8  addr, MS_U8 *pBuf)
{
    return (i2cReadByte(HDMITX_RX74_SLAVE_ADDR, addr, pBuf));
}

MS_BOOL MHal_HDMITx_Rx74WriteBytes(MS_U8  addr, MS_U8  len, MS_U8  *buf)
{
    return (i2cBurstWriteBytes(HDMITX_RX74_SLAVE_ADDR, addr, buf, len));
}

MS_BOOL MHal_HDMITx_Rx74ReadBytes(MS_U8  addr, MS_U16  len, MS_U8  *buf)
{
    return (i2cBurstReadBytes(HDMITX_RX74_SLAVE_ADDR, addr, buf, len));
}

// This routine read edid
MS_BOOL _MHal_HDMITx_EdidReadBytes(MS_U8  addr, MS_U8  len, MS_U8  *buf)
{
    return (i2cBurstReadBytes(HDMITX_EDIDROM_SLAVE_ADDR, addr, buf, len));
}

//      TxEdidSetSegment
MS_BOOL _MHal_HDMITx_EdidSetSegment(MS_U8  value)
{
    if(i2c_AccessStart(0x60, I2C_TRANS_WRITE) == FALSE)
        return FALSE;
    if(i2c_SendByte(value) == I2C_NON_ACKNOWLEDGE) // send byte
        return FALSE;
    return TRUE;
}

//  This routine read the block in EDID
MS_BOOL MHal_HDMITx_EdidReadBlock(MS_U8  num, MS_U8 *buf)
{
#if 1
    MS_U8 start_addr;

    start_addr = (num & 0x01) ? 0x80 : 0x00;

    if (num > 1)
    {
        if(_MHal_HDMITx_EdidSetSegment(num / 2) == FALSE)
            return FALSE;
    }
    if(_MHal_HDMITx_EdidReadBytes(start_addr, 128, buf) == FALSE)
        return FALSE;
    return TRUE;
#else
    MS_U8 i;

    for(i=0;i<10;i++)
    {
        i2cSetSDA_Chk(PIN_HIGH);
        i2cSetSCL_Chk(PIN_HIGH);
        i2c_Delay();
        i2cSetSDA_Chk(PIN_LOW);
        i2cSetSCL_Chk(PIN_LOW);
        i2c_Delay();
    }
    i2cSetSDA_Chk(PIN_HIGH);
    i2cSetSCL_Chk(PIN_HIGH);
    return TRUE;
#endif
}

#if 0
void TestI2C(void)
{
    MS_U8 data[16], i;
//   0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,

    while(1)
    {
        for(i=0;i<12;i++)
        {
            i2c_Start();
            i2c_SendByte(HDMITX_EDIDROM_SLAVE_ADDR);    //write
            i2c_SendByte(i);
            i2c_Stop();

            i2c_Start();
            i2c_SendByte(HDMITX_EDIDROM_SLAVE_ADDR+1);  //read
            data[i] = i2c_ReceiveByte(0);
            i2c_Stop();
        }

        MsOS_DelayTask(5);


        DBG_HDMITX(printf("TestI2C()-->  "));

        for(i=0;i<12;i++)
            DBG_HDMITX(printf("  %x", data[i]));

        DBG_HDMITX(printf("\n"));
    }

}
#endif


//------------------------------------------------------------------------------
/// @brief This routine reads HDMI Register
/// @param[in] bank register bank
/// @param[in] address register address
/// @return register value
//------------------------------------------------------------------------------
MS_U16 MHal_HDMITx_Read(MS_U32 bank, MS_U16 address)
{
    return(REG(bank, address));
}

//------------------------------------------------------------------------------
/// @brief This routine writes HDMI Register
/// @param[in] bank register bank
/// @param[in] address register address
/// @param[in] reg_data register data
/// @return None
//------------------------------------------------------------------------------
void MHal_HDMITx_Write(MS_U32 bank, MS_U16 address, MS_U16 reg_data)
{
    if(g_bDisableRegWrite == TRUE)
    {
        return;
    }
    REG(bank, address) = reg_data;
}

//------------------------------------------------------------------------------
/// @brief This routine writes HDMI Register with mask
/// @param[in] bank register bank
/// @param[in] address register address
/// @param[in] reg_mask mask value
/// @param[in] reg_data register data
/// @return None
//------------------------------------------------------------------------------
void MHal_HDMITx_Mask_Write(MS_U32 bank, MS_U16 address, MS_U16 reg_mask, MS_U16 reg_data)
{
    MS_U16 reg_value;

    if(g_bDisableRegWrite == TRUE)
    {
        return;
    }
    reg_value = (REG(bank, address) & (~reg_mask)) | (reg_data & reg_mask);
    REG(bank, address) = reg_value;
}

//------------------------------------------------------------------------------
// @brief This routine writes bulk HDMI Register
// @param[in] pTable the table of register bank, address, mask and value
// @param[in] num register number
// @return None
//------------------------------------------------------------------------------
void MHal_HDMITx_RegsTbl_Write(MSTHDMITX_REG_TYPE *pTable, MS_U8 num)
{
    MS_U8 i;

    for (i = 0; i < num; i++)
    {
        if (pTable->mask != 0xFFFF)
        {
            MHal_HDMITx_Mask_Write(pTable->bank, pTable->address, pTable->mask, pTable->value);
        }
        else
           MHal_HDMITx_Write(pTable->bank, pTable->address, pTable->value);

        pTable++;
    }
}

//------------------------------------------------------------------------------
/// @brief This routine reads PM Register
/// @param[in] bank register bank
/// @param[in] address register address
/// @return register value
//------------------------------------------------------------------------------
MS_U16 MHal_HDMITxPM_Read(MS_U32 bank, MS_U16 address)
{
    return(PMREG(bank, address));
}

//------------------------------------------------------------------------------
/// @brief This routine writes PM Register
/// @param[in] bank register bank
/// @param[in] address register address
/// @param[in] reg_data register data
/// @return None
//------------------------------------------------------------------------------
void MHal_HDMITxPM_Write(MS_U32 bank, MS_U16 address, MS_U16 reg_data)
{
    PMREG(bank, address) = reg_data;
}

//------------------------------------------------------------------------------
/// @brief This routine writes PM Register with mask
/// @param[in] bank register bank
/// @param[in] address register address
/// @param[in] reg_mask mask value
/// @param[in] reg_data register data
/// @return None
//------------------------------------------------------------------------------
void MHal_HDMITxPM_Mask_Write(MS_U32 bank, MS_U16 address, MS_U16 reg_mask, MS_U16 reg_data)
{
    MS_U16 reg_value;

    reg_value = (PMREG(bank, address) & (~reg_mask)) | (reg_data & reg_mask);
    PMREG(bank, address) = reg_value;
}


// Read HDCP key from external EEPROM if not used internal HDCP key
MS_BOOL MHal_HDMITx_HDCPKeyReadByte(MS_U32 u32Addr, MS_U8 *pu8Buf, MS_U32 u32Size)
{
    return (MDrv_EEPROM_Read(u32Addr, pu8Buf, u32Size));
}

void MHal_HDMITx_UtilDebugEnable(MS_BOOL benable)
{
    bDebugUtilFlag = benable;
}

MS_U32 MHal_HDMITx_GetDDCDelayCount(void)
{
    return u32DDCDelayCount;
}

void MHal_HDMITx_SetDDCDelayCount(MS_U32 u32Delay)
{
    u32DDCDelayCount = u32Delay;
}

MS_BOOL MHal_HDMITx_AdjustDDCFreq(MS_U32 u32Speed_K)
{
    #define C3_CPU_SPEED_900MHZ  900
    #define DELAY_CNT(SpeedKHz)  ((u32FactorDelay/(SpeedKHz))-((u32Parameter1+u32AdjParam)-((SpeedKHz)/u32AdjParam))+((1<<((u32Parameter2-SpeedKHz)/40))))

    MS_U32 u32FactorDelay = 50400UL;
    MS_U32 u32FactorAdjust = 11040UL;
    MS_U32 u32ParamBase1 = 130UL;
    MS_U32 u32Parameter1 = 130UL;
    MS_U32 u32Parameter2 = 440UL;
    MS_U32 u32AdjParam = 0;
    MS_U32 u32CpuSpeedMHz = 0;

#if 0
    //(1) assign primary parameters
    if(MDrv_COPRO_GetBase()==FALSE)
    {
        DBG_HDMIUTIL(printf("[%s][%d] MDrv_COPRO_Init Fail! \n", __FUNCTION__, __LINE__));
        return FALSE;
    }
#endif

    u32CpuSpeedMHz = (MS_U32)(C3_CPU_SPEED_900MHZ);//(MDrv_CPU_QueryClock()/1000000UL);
    u32FactorDelay = u32CpuSpeedMHz * 100;
    u32FactorAdjust = (u32CpuSpeedMHz>=312)? 10000UL :13000UL;

    if (u32CpuSpeedMHz > 0)
    {
        u32AdjParam = u32FactorAdjust/u32CpuSpeedMHz;
        DBG_HDMIUTIL(printf("[%s][%d] u32AdjParam = %lu, u32CpuSpeedMHz = %lu \n", __FUNCTION__, __LINE__, u32AdjParam, u32CpuSpeedMHz));
    }
    else
    {
        DBG_HDMIUTIL(printf("%s, Error parameter u32CpuSpeedMHz=%ld",__FUNCTION__, u32CpuSpeedMHz));
        return FALSE;
    }

    if (u32AdjParam == 0)
    {
        u32DDCDelayCount = 250;
        return FALSE;
    }

    u32Parameter2 = 440UL;
    //(2) assign base for parameter

    if(u32CpuSpeedMHz>=1000) u32ParamBase1 = 150UL;
    else if(u32CpuSpeedMHz>=900) u32ParamBase1 = 140UL;
    else if(u32CpuSpeedMHz>=780) u32ParamBase1 = 135UL;
    else if(u32CpuSpeedMHz>=720) u32ParamBase1 = 130UL;
    else if(u32CpuSpeedMHz>=650) u32ParamBase1 = 125UL;
    else if(u32CpuSpeedMHz>=600) u32ParamBase1 = 110UL;
    else if(u32CpuSpeedMHz>=560) u32ParamBase1 = 100UL;
    else if(u32CpuSpeedMHz>=530) u32ParamBase1 = 95UL;
    else if(u32CpuSpeedMHz>=500) u32ParamBase1 = 90UL;
    else if(u32CpuSpeedMHz>=480) u32ParamBase1 = 85UL;
    else if(u32CpuSpeedMHz>=430) u32ParamBase1 = 80UL;
    else if(u32CpuSpeedMHz>=400) u32ParamBase1 = 75UL;
    else if(u32CpuSpeedMHz>=384) u32ParamBase1 = 70UL;
    else if(u32CpuSpeedMHz>=360) u32ParamBase1 = 65UL;
    else if(u32CpuSpeedMHz>=336) u32ParamBase1 = 60UL;
    else if(u32CpuSpeedMHz>=312) u32ParamBase1 = 40UL;
    else if(u32CpuSpeedMHz>=240) u32ParamBase1 = 10UL;
    else if(u32CpuSpeedMHz>=216) u32ParamBase1 = 0UL;
    else u32ParamBase1 = 0UL;
    //(3) compute parameter 1 by base
    if(u32Speed_K>=100)
    {
        u32Parameter1 = u32ParamBase1 + 250; //100K level
    }
    else if (u32Speed_K>=75)
    {
        u32Parameter1 = u32ParamBase1 + 340; //75K level
    }
    else if (u32Speed_K>=50)
    {
        u32Parameter1 = u32ParamBase1 + 560; //50K level
    }
    else
    {
        u32Parameter1 = u32ParamBase1 + 860; //30K level
    }

    //u32DDCDelayCount = 250;
    DBG_HDMIUTIL(printf("[%s][%d] u32Speed_K = %lu , u32AdjParam = %lu \n", __FUNCTION__, __LINE__, u32Speed_K, u32AdjParam));

    //(4) compute delay counts
    if ((u32Speed_K>0) && (u32AdjParam>0))
    {
        u32DDCDelayCount = DELAY_CNT(u32Speed_K);
        DBG_HDMIUTIL(printf("[%s][%d] u32DDCDelayCount = %lu \n", __FUNCTION__, __LINE__, u32DDCDelayCount));

        //u32DDCDelayCount = 250;
        DBG_HDMIUTIL(printf("[%s][%d] u32DDCDelayCount = %lu \n", __FUNCTION__, __LINE__, u32DDCDelayCount));
    }
    else
    {
        DBG_HDMIUTIL(printf("[%s][%d] Error parameter u32Speed_K=%ld , u32AdjParam=%ld",__FUNCTION__,  __LINE__, u32Speed_K, u32AdjParam));
        return FALSE;
    }

    return TRUE;
}



