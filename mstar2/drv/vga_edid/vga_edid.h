// $Change: 455385 $
//------------------------------------------------------------------------------
//
// Copyright (c) 2009 MStar Semiconductor, Inc.  All rights reserved.
//
//------------------------------------------------------------------------------
// FILE
//      fmt.c
//
// DESCRIPTION
//      Stream interface driver for FM-TX driver
//
// HISTORY
//      2009.07.29      Hata Tang       Initial Version
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Include Files
//------------------------------------------------------------------------------
#ifndef _MDRV_FMT_H_
#define _MDRV_FMT_H_
#include <asm/types.h>
#include "mdrv_types.h"


#define	LT8612_DEV_NAME	"lt8612"


//------------------------------------------------------------------------------
//  Macros
//------------------------------------------------------------------------------

#define FREQ2CHREG(freq)   ((freq-7600)/5)
#define CHREG2FREQ(ch)     (ch*5+7600)


#define _QNCOMMON_H_
 
#define QND_REG_NUM_MAX   19  // for qn8027
// crystal setting definition is not ready yet, please read datasheet to do setting accordingly
#define QND_CRYSTAL_REG             0x03 //?
#define QND_CRYSTAL_BIT_MASK        0x3f //?
 
#define QND_CRYSTAL_24MHZ           0x2b //?
#define QND_CRYSTAL_DEFAULT         QND_CRYSTAL_24MHZ
 
 
 
#define QND_MODE_SLEEP      0     
#define QND_MODE_WAKEUP     1
#define QND_MODE_DEFAULT    2
// RX / TX value is using upper 8 bit
 
#define QND_MODE_RX         0x8000
#define QND_MODE_TX         0x4000
// AM / FM value is using lower 8 bit 
// need to check datasheet to get right bit 
#define QND_MODE_FM         0x0000
 
#define BAND_FM        0

/**********************************************************************************************
// limitation configuration 
***********************************************************************************************/
 
#define QND_READ_RSSI_DELAY    10
#define QND_DELAY_BEFORE_UNMUTE  200
// auto scan
#define QND_MP_THRESHOLD       0x28   
#define PILOT_READ_OUT_DELAY_TIME 70
#define PILOT_SNR_READ_OUT_DELAY_TIME  (150-PILOT_READ_OUT_DELAY_TIME)
#define CH_SETUP_DELAY_TIME    300           
 
 
#define assert(str)
#define QND_LOG(a)
#define QND_LOGA(a,b)
#define QND_LOGB(a,b)
#define QND_LOGHEX(a,b)
#define _QNREG_H_
#define SYSTEM1       0x00
#define SYSTEM2       0x00
#define CH            0x01
#define CH_STEP       0x00
#define RDSD0         0x08
#define PAG_CAL       0x1f
#define CID2          0x06
#define RDSEN         0x80
#define TXREQ         0x20
#define CH_CH         0x03
#define RDSTXRDY      0x04
#define TX_FDEV       0x11  // FDEV on datasheet
#define _QNSYS_H_
 
typedef struct
{
    unsigned char  value;
}FMT_INIT_CONFIG;

typedef struct
{
    int mode;
}FMT_MODE_CONFIG;

typedef struct
{
    unsigned int freq;
}FMT_FREQ_CONFIG;

typedef struct
{
    unsigned char gain;
}FMT_TXPOWER_CONFIG;

#define FMT_IOC_MAGIC               0xd2
#define IOCTL_FMT_INIT         _IOWR(FMT_IOC_MAGIC, 0,FMT_INIT_CONFIG)
#define IOCTL_FMT_SET_MODE     _IOWR(FMT_IOC_MAGIC, 1,FMT_MODE_CONFIG)
#define IOCTL_FMT_SET_FREQ     _IOWR(FMT_IOC_MAGIC, 2,FMT_FREQ_CONFIG)
#define IOCTL_FMT_SET_TX_POWER     _IOWR(FMT_IOC_MAGIC, 3,FMT_TXPOWER_CONFIG)


#define FMT_IOC_MAXNR               4

//------------------------------------------------------------------------------
//  Variables
//------------------------------------------------------------------------------
//volatile PREG_FMT_st g_pFMTRegs = NULL;
//static HANDLE g_hDeviceFile = NULL;
//volatile PREG_CHIPTOP_st g_pFMTCHTPReg;

//------------------------------------------------------------------------------
//  Functions
//------------------------------------------------------------------------------

#endif // _MDRV_FMT_H_
