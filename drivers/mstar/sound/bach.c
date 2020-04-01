// $Change: 548282 $
//
// bach.cpp
//
// defines for the registers in the C3 BACH chip
//
//
#if defined(__linux__)
#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/dma-mapping.h>

#include <linux/delay.h>
#include <linux/spinlock.h>

//#include <sound/driver.h>   remove it for kernel 2.6.29.1
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <asm/dma.h>
#include <asm/io.h>
//#include <mach/hardware.h>
#include "bach.h"

/*
typedef enum
{
    DEVINFO_BD_MST154A_D01A_S     = 0x0801,
    DEVINFO_BD_MST786_SZDEMO      = 0x0802,
    DEVINFO_BD_MST786_CUSTOM10    = 0x0810,
    DEVINFO_BD_MST786_CUSTOM20    = 0x0820,
    DEVINFO_BD_MST786_CUSTOM30    = 0x0830,
    E_BD_UNKNOWN           = 0xFFFF
}DEVINFO_BOARD_TYPE;
*/

#define DELAY(x) mdelay(x)
#define SLEEP(x) schedule_timeout(msecs_to_jiffies(x * 1000))
#define UDELAY(x) udelay(x)		//CAPDELAY(1000*us)
//extern DEVINFO_BOARD_TYPE ms_devinfo_board_type(void);
#define BOARDNAME() 0x0801 //ms_devinfo_board_type()


#else

#include <windows.h>
#include <waveddsi.h>
#include<wavedbg.h>
#include <ceddk.h>
#include <ddkmacro.h>
#include "bach.h"
#include "columbus.h"
//#include "wavemain.h"

#define DELAY(x) SimpleTimerLoopDelay(x*1000)
#define SLEEP(x) Sleep(x)
#define udelay(x)
#define BOARDNAME() OALReadBoardName()


#endif


typedef enum
{
    BACH_SYNTH_REG_BANK,
    BACH_SYNTH_REG_NF_H,
    BACH_SYNTH_REG_NF_L,
    BACH_SYNTH_REG_NF_TRIG,
    BACH_SYNTH_REG_NUM,
} BachSynthReg_e;

const U16 g_nSynthRegTbl[BACH_SYNTH_NUM][BACH_SYNTH_REG_NUM] =
{
    {0xff, 0xff, 0xff, 0xff},
    {BACH_REG_BANK1, BACH_NF_SYNTH_1_NF_H, BACH_NF_SYNTH_1_NF_L, BACH_NF_SYNTH_1_NF_H},
    {BACH_REG_BANK1, BACH_NF_SYNTH_2_NF_H, BACH_NF_SYNTH_2_NF_L, BACH_NF_SYNTH_2_NF_H},
    {BACH_REG_BANK2, BACH_NF_SYNTH_3_NF_H, BACH_NF_SYNTH_3_NF_L, BACH_NF_SYNTH_3_NF_H},
    {BACH_REG_BANK1, BACH_NF_SYNTH_4_NF_H, BACH_NF_SYNTH_4_NF_L, BACH_NF_SYNTH_4_NF_H},
    {BACH_REG_BANK1, BACH_NF_SYNTH_5_NF_H, BACH_NF_SYNTH_5_NF_L, BACH_NF_SYNTH_5_NF_H},
    {BACH_REG_BANK2, BACH_NF_SYNTH_6_NF_H, BACH_NF_SYNTH_6_NF_L, BACH_NF_SYNTH_6_NF_H},
    {BACH_REG_BANK2, BACH_NF_SYNTH_7_NF_H, BACH_NF_SYNTH_7_NF_L, BACH_NF_SYNTH_7_NF_H}

};

typedef enum
{
    BACH_ASRC_REG_FS_BANK,
    BACH_ASRC_REG_FS_REG,
    BACH_ASRC_REG_FS_SEL,
    BACH_ASRC_REG_NF_BANK,
    BACH_ASRC_REG_NF_REG,
    BACH_ASRC_REG_NF_SEL,
    BACH_ASRC_REG_NUM,
} BachAsrcReg_e;

//BachAsrc_e BachAsrcReg_e
const U16 g_nAsrcRegTbl[BACH_ASRC_NUM][BACH_ASRC_REG_NUM] =
{
    {
        BACH_REG_BANK1, BACH_TIMING_CTRL0, REG_CH1_INT_128FS_SEL,
        BACH_REG_BANK1, BACH_AUDIOBAND_CTRL0, REG_CH1_INT_128FS_NF_SEL
    },

    {
        BACH_REG_BANK1, BACH_TIMING_CTRL0, REG_CH2_INT_128FS_SEL,
        BACH_REG_BANK1, BACH_AUDIOBAND_CTRL0, REG_CH2_INT_128FS_NF_SEL
    },

    {
        0xff, 0xff, 0xff,
        BACH_REG_BANK1, BACH_AUDIOBAND_CTRL0, REG_DEC_125FS_NF_SEL
    },

    {
        0xff, 0xff, 0xff,
        BACH_REG_BANK2, BACH_AUDIOBAND_CTRL3, REG_DEC_125FS_NF_SEL2
    },

    {
        BACH_REG_BANK1, BACH_TIMING_CTRL1, (REG_ASRC2_CH1_INT_128FS_SEL | REG_ASRC2_CH2_INT_128FS_SEL),
        0xff, 0xff, 0xff
    },

    {
        0xff, 0xff, 0xff,
        BACH_REG_BANK2, BACH_AUDIOBAND_CTRL3, REG_ASRC2_DEC_125FS_NF_SEL
    },

    {
        0xff, 0xff, 0xff,
        BACH_REG_BANK2, BACH_AUDIOBAND_CTRL3, REG_ASRC2_DEC_125FS_NF_SEL2
    }
};


const U8 g_nAsrcIntMuxRegValTbl[BACH_ASRC_INT_NUM][BACH_SRC_NUM * 2] =
{
    {3, 0xff, 0xff, 0xff, 2,    0,    0, 0xff, 0xff, 0xff, 0xff, 	  3, 2,    1,     1, 0xff},	//BACH_ASRC_INT1
    {3, 0xff, 0xff, 0xff, 2,    0, 0xff, 0xff,    0, 0xff,    0, 0xff, 2,    1,     1, 0xff},	//BACH_ASRC_INT2
    {0, 0xff,    3, 0xff, 1,    0, 0xff, 0xff,    2, 0xff,    2, 0xff, 1,    1,  0xff, 0xff},	//BACH_ASRC_ASRC2_INT1_2
};

const U8 g_nAsrcSynthAvailMatrix[BACH_ASRC_NUM - 1][BACH_SYNTH_NUM] =
{
    {   1,    0,    1, 0xff, 0xff, 0xff, 0xff, 0xff},		//BACH_ASRC_INT1
    {   1,    0,    1, 0xff, 0xff, 0xff, 0xff, 0xff},
    {0xff, 0xff,    0,    1, 0xff, 0xff, 0xff, 0xff},
    {0xff, 0xff,    0,    1, 0xff, 0xff, 0xff, 0xff},
    {   1, 0xff, 0xff, 0xff,    0, 0xff, 0xff, 0xff},		//BACH_ASRC_ASRC2_INT1_2
    {0xff, 0xff, 0xff, 0xff, 0xff,    1,    0, 0xff},
    {0xff, 0xff, 0xff, 0xff, 0xff,    1,    0, 0xff},
};

const U32 g_nAsrcRateMapTbl1[BACH_ASRC_NUM][BACH_RATE_NUM] =
{
    //fs, 8k, 16k, 32k, 44k, 48k
    {0, 0x02ee0000, 0x01770000, 0x00bb8000, 0x00880def, 0x007d0000, 0},	//BACH_ASRC_INT1
    {0, 0x02ee0000, 0x01770000, 0x00bb8000, 0x00880def, 0x007d0000, 0}, //BACH_ASRC_INT2
    {0, 0x03000000, 0x01800000, 0x00c00000, 0x008b51da, 0x00800000, 0},	//BACH_ASRC_DEC1_2
    {0, 0x03000000, 0x01800000, 0x00c00000, 0x008b51da, 0x00800000, 0}, //BACH_ASRC_DEC3_4
    {0, 0x02ee0000, 0x01770000, 0x00bb8000, 0x00880def, 0x007d0000, 0},	//BACH_ASRC_ASRC2_INT1_2
    {0, 0x03000000, 0x01800000, 0x00c00000, 0x008b51da, 0x00800000, 0},	//BACH_ASRC_ASRC2_DEC1_2
    {0, 0x03000000, 0x01800000, 0x00c00000, 0x008b51da, 0x00800000, 0},	//BACH_ASRC_ASRC2_DEC3_4
    {0, 0x1a5e0000, 0x0d2f0000, 0x06978000, 0x04c87d63, 0x04650000, 0}  //BACH_ASRC_SPIDF_HDMI
};

const U32 g_nAsrcRateMapTbl2[BACH_ASRC_NUM][BACH_RATE_NUM] =
{
    //fs, 8k, 16k, 32k, 44k, 48k
    {0, 0x02ee0000, 0x01770000, 0x00bb8000, 0x00880def, 0x007d0000, 0},	//BACH_ASRC_INT1
    {0, 0x02ee0000, 0x01770000, 0x00bb8000, 0x00880def, 0x007d0000, 0}, //BACH_ASRC_INT2
    {0, 0x03000000, 0x01800000, 0x00c00000, 0x008b51da, 0x00800000, 0},	//BACH_ASRC_DEC1_2
    {0, 0x03000000, 0x01800000, 0x00c00000, 0x008b51da, 0x00800000, 0}, //BACH_ASRC_DEC3_4
    {0, 0x02ee0000, 0x01770000, 0x00bb8000, 0x00880def, 0x007d0000, 0},	//BACH_ASRC_ASRC2_INT1_2
    {0, 0x03000000, 0x01800000, 0x00c00000, 0x008b51da, 0x00800000, 0},	//BACH_ASRC_ASRC2_DEC1_2
    {0, 0x03000000, 0x01800000, 0x00c00000, 0x008b51da, 0x00800000, 0},	//BACH_ASRC_ASRC2_DEC3_4
    {0, 0x1d4c0000, 0x0ea60000, 0x07530000, 0x05508b52, 0x04e20000, 0}  //BACH_ASRC_SPIDF_HDMI
};


const U16 g_nDmaIntReg[BACH_DMA_NUM][BACH_DMA_INT_NUM] =
{
    {REG_WR_UNDERRUN_INT_EN, REG_WR_OVERRUN_INT_EN, 0, REG_WR_FULL_INT_EN},
    {REG_WR_UNDERRUN_INT_EN, REG_WR_OVERRUN_INT_EN, 0, REG_WR_FULL_INT_EN},
    {REG_RD_UNDERRUN_INT_EN, REG_RD_OVERRUN_INT_EN, REG_RD_EMPTY_INT_EN, 0},
    {REG_RD_UNDERRUN_INT_EN, REG_RD_OVERRUN_INT_EN, REG_RD_EMPTY_INT_EN, 0}
};




BachAsrcCfg_e m_tAsrcCfgTbl[] =
{
    {BACH_RATE_48K, BACH_NF_SYNTH_1, BACH_NF_SYNTH_1},
    {BACH_RATE_48K, BACH_NF_SYNTH_1, BACH_NF_SYNTH_1},
    {BACH_RATE_48K, BACH_NF_SYNTH_2, BACH_SYNTH_NULL},
    {BACH_RATE_48K, BACH_NF_SYNTH_2, BACH_NF_SYNTH_3},
    {BACH_RATE_48K, BACH_NF_SYNTH_4, BACH_NF_SYNTH_4},
    {BACH_RATE_48K, BACH_NF_SYNTH_6, BACH_NF_SYNTH_5},
    {BACH_RATE_48K, BACH_NF_SYNTH_6, BACH_NF_SYNTH_6}
};

BachMux4_e g_tAdcMuxTbl[]=
{ BACH_MUX4_DEC1_2, BACH_MUX4_DEC3_4, BACH_MUX4_ASRC2DEC1_2, BACH_MUX4_ASRC2DEC3_4};

//BachSrcCfg_t m_tSrcCfgTbl[BACH_SRC_NUM];
//BachSrcCfg_t m_tSinkCfgTbl[BACH_SRC_NUM];

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------


static DMACHANNEL m_dmachannel[4]; // info about dma channel states

//static BOOL m_bIsMapped; // must call MmUnmapIoSpace when destroyed

static S8  m_nOutputGain[2] = {0, 0};
static S8  m_nRecGain[2] = {0, 0};
//static U16 m_nMicMute = 0;
static U16 m_nMicGain = 0x3;
static U16 m_nMicPreGain = 0x0;
static U16 m_nMicInitGain = 0x3;
static U16 m_nLineOutGain[2] = {0, 0};
static U16 m_nLineOutMute[2] = {0, 0};


static U32  m_pBaseRegAddr;
static U32  m_pAudBank1RegAddr;
static U32  m_pAudBank2RegAddr;
static U32  m_pAudBank3RegAddr;
static U32  m_pAudBank4RegAddr;

//static BOOL m_bBachFuncStatus[BACH_FUNC_NUM];
//static BOOL m_bBachLastFuncStatus[BACH_FUNC_NUM];

static U32 g_nAsrcRateMapTbl[BACH_ASRC_NUM][BACH_RATE_NUM];
static BOOL m_bAnalogIdle;
static BOOL m_bADCActive;
static BOOL m_bDACActive;
static BOOL m_bBachAtopStatus[BACH_ATOP_NUM];
static BOOL m_bBachLastAtopStatus[BACH_ATOP_NUM];

static U16 m_nLineInMux[2]= {0,0};
static U16 m_nLineInGain[2]= {0,0};
static U16 m_nLineInMute[2]= {0,0};

static BachDacSrc_e m_nOutMix[2] = {1, 2};

static BachSinkCfg_t tSinkCfg;
static BachSrcCfg_t  tSrcCfg;
static BachBtChip_e  m_BtChip;

static BOOL m_bWakeUp;
static U16 m_ChipRevision;
static U16 m_BoardRevision;


//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
#define REVISION_U01 (0x0)
#define REVISION_U02 (0x1)
#define REVISION_U03 (0x2)
#define REVISION_U04 (0x3)
#define REVISION_UNKNOWN (0x4)

#ifndef E_BD_MST154A_D01A_S
#define E_BD_MST154A_D01A_S (0x0801)
#endif

static U16 ReadChipRevision(void)
{
    U16 tmp = 0;
    tmp = BachReadReg2Byte(0x001E02);
    return ((tmp >> 8) & 0x00FF);
}

U16 BachGetMaskReg(BachRegBank_e nBank, U8 nAddr)
{
    return BachReadReg(nBank, nAddr);
}

void BachSetBankBaseAddr(U32 addr)
{
    m_pBaseRegAddr = addr;
    m_pAudBank1RegAddr = addr + (U32)(BACH_REG_BANK_1 << 1);
    m_pAudBank2RegAddr = addr + (U32)(BACH_REG_BANK_2 << 1);
    m_pAudBank3RegAddr = addr + (U32)(BACH_REG_BANK_3 << 1);
    m_pAudBank4RegAddr = addr + (U32)(BACH_REG_BANK_4 << 1);
}

void BachWriteReg2Byte(U32 nAddr, U16 nValue)
{
    WRITE_WORD(m_pBaseRegAddr + ((nAddr) << 1), nValue);
}

void BachWriteRegByte(U32 nAddr, U8 nValue)
{
    WRITE_BYTE(m_pBaseRegAddr + ((nAddr) << 1) - ((nAddr) & 1), nValue);
}

U16 BachReadReg2Byte(U32 nAddr)
{
    return READ_WORD(m_pBaseRegAddr + ((nAddr) << 1));
}

U8 BachReadRegByte(U32 nAddr)
{
    return READ_BYTE(m_pBaseRegAddr + ((nAddr) << 1) - ((nAddr) & 1));
}


/*
void BachWriteBank1Reg(U8 nAddr, U16 nValue)
{
#if 0
	WRITE_PORT_USHORT ( TYPE_CAST(U32, PUSHORT, (m_pAudBank1RegAddr + (nAddr) * 2)), nValue );
#else
	WRITE_WORD(m_pAudBank1RegAddr + ((nAddr) << 1), nValue);
#endif
}

void BachWriteBank2Reg(U8 nAddr, U16 nValue)
{
#if 0
	WRITE_PORT_USHORT( TYPE_CAST(U32, PUSHORT, (m_pAudBank2RegAddr + (nAddr) * 2)), nValue );
#else
	WRITE_WORD(m_pAudBank2RegAddr + ((nAddr) << 1), nValue);
#endif
}

void BachWriteBank3Reg(U8 nAddr, U16 nValue)
{
#if 0
	WRITE_PORT_USHORT( TYPE_CAST(U32, PUSHORT, (m_pAudBank3RegAddr + (nAddr) * 2)), nValue );
#else
	WRITE_WORD(m_pAudBank3RegAddr + ((nAddr) << 1), nValue);
#endif
}


void BachWriteBank4Reg(U8 nAddr, U16 nValue)
{
#if 0
	WRITE_PORT_USHORT( TYPE_CAST(U32, PUSHORT, (m_pAudBank4RegAddr + (nAddr) * 2)), nValue );
#else
	WRITE_WORD(m_pAudBank4RegAddr + ((nAddr) << 1), nValue);
#endif
}

U16 BachReadBank1Reg(U8 nAddr)
{
#if 0
	U16 nReg = READ_PORT_USHORT ( TYPE_CAST(U32, PUSHORT, (m_pAudBank1RegAddr + (nAddr) * 2)) );
    return (nReg);
#else
	return READ_WORD(m_pAudBank1RegAddr + ((nAddr) << 1));
#endif
}

U16 BachReadBank2Reg(U8 nAddr)
{
#if 0
	U16 nReg = READ_PORT_USHORT ( TYPE_CAST(U32, PUSHORT, (m_pAudBank2RegAddr + (nAddr) * 2)) );
    return (nReg);
#else
	return READ_WORD(m_pAudBank2RegAddr + ((nAddr) << 1));
#endif
}

U16 BachReadBank3Reg(U8 nAddr)
{
#if 0
	U16 nReg = READ_PORT_USHORT ( TYPE_CAST(U32, PUSHORT, (m_pAudBank3RegAddr + (nAddr) * 2)) );
    return (nReg);
#else
	return READ_WORD(m_pAudBank3RegAddr + ((nAddr) << 1));
#endif
}

U16 BachReadBank4Reg(U8 nAddr)
{
#if 0
	U16 nReg = READ_PORT_USHORT ( TYPE_CAST(U32, PUSHORT, (m_pAudBank4RegAddr + (nAddr) * 2)) );
    return (nReg);
#else
	return READ_WORD(m_pAudBank4RegAddr + ((nAddr) << 1));
#endif
}
*/

void BachWriteReg(BachRegBank_e nBank, U8 nAddr, U16 regMsk, U16 nValue)
{
    U16 nConfigValue;

    switch(nBank)
    {
    case BACH_REG_BANK1:
        //nConfigValue = ReadBank1Reg(nAddr);
        nConfigValue = READ_WORD(m_pAudBank1RegAddr + ((nAddr) << 1));
        nConfigValue &= ~regMsk;
        nConfigValue |= (nValue & regMsk);
        //WriteBank1Reg(nAddr, nConfigValue);
        WRITE_WORD(m_pAudBank1RegAddr + ((nAddr) << 1), nConfigValue);
        break;
    case BACH_REG_BANK2:
        //nConfigValue = ReadBank2Reg(nAddr);
        nConfigValue = READ_WORD(m_pAudBank2RegAddr + ((nAddr) << 1));
        nConfigValue &= ~regMsk;
        nConfigValue |= (nValue & regMsk);
        //WriteBank2Reg(nAddr, nConfigValue);
        WRITE_WORD(m_pAudBank2RegAddr + ((nAddr) << 1), nConfigValue);
        break;
    case BACH_REG_BANK3:
        //nConfigValue = ReadBank3Reg(nAddr);
        nConfigValue = READ_WORD(m_pAudBank3RegAddr + ((nAddr) << 1));
        nConfigValue &= ~regMsk;
        nConfigValue |= (nValue & regMsk);
        //WriteBank3Reg(nAddr, nConfigValue);
        WRITE_WORD(m_pAudBank3RegAddr + ((nAddr) << 1), nConfigValue);
        break;
    case BACH_REG_BANK4:
        //nConfigValue = ReadBank4Reg(nAddr);
        nConfigValue = READ_WORD(m_pAudBank4RegAddr + ((nAddr) << 1));
        nConfigValue &= ~regMsk;
        nConfigValue |= (nValue & regMsk);
        //WriteBank4Reg(nAddr, nConfigValue);
        WRITE_WORD(m_pAudBank4RegAddr + ((nAddr) << 1), nConfigValue);
        break;
    default:
        ERRMSG("WAVEDEV.DLL: BachWriteReg - ERROR bank default case!\n");
        break;
    }
}


U16 BachReadReg(BachRegBank_e nBank, U8 nAddr)
{
    switch(nBank)
    {
    case BACH_REG_BANK1:
        return READ_WORD(m_pAudBank1RegAddr + ((nAddr) << 1));
    case BACH_REG_BANK2:
        return READ_WORD(m_pAudBank2RegAddr + ((nAddr) << 1));
    case BACH_REG_BANK3:
        return READ_WORD(m_pAudBank3RegAddr + ((nAddr) << 1));
    case BACH_REG_BANK4:
        return READ_WORD(m_pAudBank4RegAddr + ((nAddr) << 1));
    default:
        ERRMSG("WAVEDEV.DLL: BachReadReg - ERROR bank default case!\n");
        return 0;
    }
}

void BachSetDmaValidSel(BachDmaChannel_e eDma, U8 u8Choice)
{
    //TRACE2("BachSetDmaValidSel BachDmaChannel_e = %d, u8Choice = %d", eDma, u8Choice);

    switch(eDma)
    {
    case BACH_DMA_WRITER1:
        BachWriteReg(BACH_REG_BANK1, BACH_AUDIOBAND_CTRL0, REG_DMA1_WR_VALID_SEL, (u8Choice << REG_DMA1_WR_VALID_POS));
        break;
    case BACH_DMA_WRITER2:
        BachWriteReg(BACH_REG_BANK2, BACH_PAD_CTRL1, REG_DMA2_WR_VALID_SEL, (u8Choice << REG_DMA2_WR_VALID_POS));
        break;
    case BACH_DMA_READER1:
        BachWriteReg(BACH_REG_BANK1, BACH_AUDIOBAND_CTRL0, REG_DMA1_RD_VALID_SEL_MSK, (u8Choice << REG_DMA1_RD_VALID_SEL_POS));
        break;
    case BACH_DMA_READER2:
        BachWriteReg(BACH_REG_BANK2, BACH_PAD_CTRL1, REG_DMA2_RD_VALID_SEL, (u8Choice << REG_DMA2_RD_VALID_POS));
        break;
    default:
        ERRMSG("BachSetDmaValidSel - ERROR BachDmaChannel_e default case!\n");
    }
}

void BachSetMux4(BachMux4_e eMux, U8 u8Choice)
{
//	TRACE2("BachSetMux4 BachMux4_e = %d, u8Choice = %d", eMux, u8Choice);

    switch(eMux)
    {
    case BACH_MUX4_MMP1_2_CH1:
        BachWriteReg(BACH_REG_BANK1, BACH_MIX_CTRL0, REG_CH1_MUX_SEL_MSK, (u8Choice << REG_CH1_MUX_SEL_POS));
        break;
    case BACH_MUX4_MMP1_2_CH2:
        BachWriteReg(BACH_REG_BANK1, BACH_MIX_CTRL0, REG_CH2_MUX_SEL_MSK, (u8Choice << REG_CH2_MUX_SEL_POS));
        break;
    case BACH_MUX4_DEC1_2:
        if(u8Choice<3 && !m_bADCActive)
            BachAdcMuxGuard(eMux,TRUE);
        else
            BachAdcMuxGuard(eMux,FALSE);
        BachWriteReg(BACH_REG_BANK1, BACH_TIMING_CTRL0, REG_DEC_NUM_MSK, (u8Choice << REG_DEC_NUM_POS));
        break;
    case BACH_MUX4_DEC3_4:
        if(u8Choice<3 && !m_bADCActive)
            BachAdcMuxGuard(eMux,TRUE);
        else
            BachAdcMuxGuard(eMux,FALSE);
        BachWriteReg(BACH_REG_BANK2, BACH_AUDIOBAND_CTRL2, REG_DEC_NUM3_MSK, (u8Choice << REG_DEC_NUM3_POS));
        break;
    case BACH_MUX4_ASRC2MMP1_2:
        BachWriteReg(BACH_REG_BANK1, BACH_AUDIOBAND_CTRL1, REG_ASRC2_CH_MUX_SEL_MSK, (u8Choice << REG_ASRC2_CH_MUX_SEL_POS));
        break;
    case BACH_MUX4_ASRC2DEC1_2:
        if(u8Choice<3 && !m_bADCActive)
            BachAdcMuxGuard(eMux,TRUE);
        else
            BachAdcMuxGuard(eMux,FALSE);
        BachWriteReg(BACH_REG_BANK1, BACH_TIMING_CTRL1, REG_DEC_NUM2_MSK, (u8Choice << REG_DEC_NUM2_POS));
        break;
    case BACH_MUX4_ASRC2DEC3_4:
        if(u8Choice<3 && !m_bADCActive)
            BachAdcMuxGuard(eMux,TRUE);
        else
            BachAdcMuxGuard(eMux,FALSE);
        BachWriteReg(BACH_REG_BANK2, BACH_AUDIOBAND_CTRL2, REG_DEC_NUM4_MSK, (u8Choice << REG_DEC_NUM4_POS));
        break;
    case BACH_MUX4_DMA1_WRITER:
        BachWriteReg(BACH_REG_BANK1, BACH_MIX_CTRL0, REG_DMAWR_MUX_SEL_MSK, (u8Choice << REG_DMAWR_MUX_SEL_POS));
        break;
    case BACH_MUX4_DMA2_WRITER:
        BachWriteReg(BACH_REG_BANK2, BACH_PAD_CTRL1, REG_DMA2_WR_MUX_SEL_MSK, (u8Choice << REG_DMA2_WR_MUX_SEL_POS));;
        break;
    default:
        ERRMSG("BachSetMux4 - ERROR MUX4 default case!\n");
    }
}

U8 BachGetMux4(BachMux4_e eMux)
{
//	TRACE2("BachSetMux4 BachMux4_e = %d, u8Choice = %d", eMux, u8Choice);

    switch(eMux)
    {
    case BACH_MUX4_MMP1_2_CH1:
        return (U8)((BachReadReg(BACH_REG_BANK1, BACH_MIX_CTRL0) & REG_CH1_MUX_SEL_MSK) >> REG_CH1_MUX_SEL_POS);
    case BACH_MUX4_MMP1_2_CH2:
        return (U8)((BachReadReg(BACH_REG_BANK1, BACH_MIX_CTRL0) & REG_CH2_MUX_SEL_MSK) >> REG_CH2_MUX_SEL_POS);
    case BACH_MUX4_DEC1_2:
        return (U8)((BachReadReg(BACH_REG_BANK1, BACH_TIMING_CTRL0) & REG_DEC_NUM_MSK) >> REG_DEC_NUM_POS);
    case BACH_MUX4_DEC3_4:
        return (U8)((BachReadReg(BACH_REG_BANK2, BACH_AUDIOBAND_CTRL2) & REG_DEC_NUM3_MSK) >> REG_DEC_NUM3_POS);
    case BACH_MUX4_ASRC2MMP1_2:
        return (U8)((BachReadReg(BACH_REG_BANK1, BACH_AUDIOBAND_CTRL1) & REG_ASRC2_CH_MUX_SEL_MSK) >> REG_ASRC2_CH_MUX_SEL_POS);
    case BACH_MUX4_ASRC2DEC1_2:
        return (U8)((BachReadReg(BACH_REG_BANK1, BACH_TIMING_CTRL1) & REG_DEC_NUM2_MSK) >> REG_DEC_NUM2_POS);
    case BACH_MUX4_ASRC2DEC3_4:
        return (U8)((BachReadReg(BACH_REG_BANK2, BACH_AUDIOBAND_CTRL2) & REG_DEC_NUM4_MSK) >> REG_DEC_NUM4_POS);
    case BACH_MUX4_DMA1_WRITER:
        return (U8)((BachReadReg(BACH_REG_BANK1, BACH_MIX_CTRL0) & REG_DMAWR_MUX_SEL_MSK) >> REG_DMAWR_MUX_SEL_POS);
    case BACH_MUX4_DMA2_WRITER:
        return (U8)((BachReadReg(BACH_REG_BANK2, BACH_PAD_CTRL1) & REG_DMA2_WR_MUX_SEL_MSK) >> REG_DMA2_WR_MUX_SEL_POS);
    default:
        ERRMSG("BachGetMux4 - ERROR MUX4 default case!\n");
    }

    return 0;
}


void BachSetMux2(BachMux2_e eMux, U8 u8Choice)
{
    U16 nConfigValue = 0;

//	TRACE2("BachSetMux2 BachMux2_e = %d, u8Choice = %d", eMux, u8Choice);

    switch(eMux)
    {
    case BACH_MUX2_MUX:
        if(u8Choice)
            nConfigValue |= REG_CH1_MUX_SEL2;
        BachWriteReg(BACH_REG_BANK2, BACH_PAD_CTRL1, REG_CH1_MUX_SEL2, nConfigValue);
        break;
    case BACH_MUX2_CODEC_I2S_TX:
        if(u8Choice)
            nConfigValue |= REG_CODEC_FS_SEL;
        BachWriteReg(BACH_REG_BANK2, BACH_CODEC_I2S_CTRL, REG_CODEC_FS_SEL, nConfigValue);
        break;
    case BACH_MUX2_DMARD_L:
        if(u8Choice)
            nConfigValue |= REG_DMARD_L_DPGA_DIN_SEL;
        BachWriteReg(BACH_REG_BANK1, BACH_AUDIOBAND_CTRL1, REG_DMARD_L_DPGA_DIN_SEL, nConfigValue);
        break;
    case BACH_MUX2_DMARD_R:
        if(u8Choice)
            nConfigValue |= REG_DMARD_R_DPGA_DIN_SEL;
        BachWriteReg(BACH_REG_BANK1, BACH_AUDIOBAND_CTRL1, REG_DMARD_R_DPGA_DIN_SEL, nConfigValue);
        break;
    case BACH_MUX2_SPDIF_HDMI_TX:
        if(u8Choice)
            nConfigValue |= REG_SPDIF_TX_SOURCE_SEL;
        BachWriteReg(BACH_REG_BANK2, BACH_PAD_CTRL1, REG_SPDIF_TX_SOURCE_SEL, nConfigValue);
        break;
    case BACH_MUX2_SDM_DWA1_CH1:
        if(u8Choice)
            nConfigValue |= REG_PGA1_SWAP;
        BachWriteReg(BACH_REG_BANK1, BACH_SINE_GEN_CTRL0, REG_PGA1_SWAP, nConfigValue);
        break;
    case BACH_MUX2_SDM_DWA1_CH2:
        if(u8Choice)
            nConfigValue |= REG_PGA2_SWAP;
        BachWriteReg(BACH_REG_BANK1, BACH_SINE_GEN_CTRL0, REG_PGA2_SWAP, nConfigValue);
        break;
    case BACH_MUX2_SDM_DWA2_CH1:
        if(u8Choice)
            nConfigValue |= REG_PGA3_SWAP;
        BachWriteReg(BACH_REG_BANK2, BACH_AUDIOBAND_CTRL3, REG_PGA3_SWAP, nConfigValue);
        break;
    case BACH_MUX2_SDM_DWA2_CH2:
        if(u8Choice)
            nConfigValue |= REG_PGA4_SWAP;
        BachWriteReg(BACH_REG_BANK2, BACH_AUDIOBAND_CTRL3, REG_PGA4_SWAP, nConfigValue);
        break;
    case BACH_MUX2_DMAWR1:
        if(u8Choice)
            nConfigValue |= REG_SEL_DEC1_DOUT;
        BachWriteReg(BACH_REG_BANK1, BACH_MIX_CTRL0, REG_SEL_DEC1_DOUT, nConfigValue);
        break;
    case BACH_MUX2_DMAWR2:
        if(u8Choice)
            nConfigValue |= REG_SEL_DEC2_DOUT;
        BachWriteReg(BACH_REG_BANK1, BACH_MIX_CTRL0, REG_SEL_DEC2_DOUT, nConfigValue);
        break;
    case BACH_MUX2_CODEC_TX_CH1:
        if(u8Choice)
            nConfigValue |= REG_CODEC_I2S_TX_L_SWAP;
        BachWriteReg(BACH_REG_BANK1, BACH_RX_FIFO_CTRL0, REG_CODEC_I2S_TX_L_SWAP, nConfigValue);
        break;
    case BACH_MUX2_CODEC_TX_CH2:
        if(u8Choice)
            nConfigValue |= REG_CODEC_I2S_TX_R_SWAP;
        BachWriteReg(BACH_REG_BANK1, BACH_RX_FIFO_CTRL0, REG_CODEC_I2S_TX_R_SWAP, nConfigValue);
        break;
    case BACH_MUX2_SPDIF_TX_CH1:
        if(u8Choice)
            nConfigValue |= REG_SPDIF_TX_L_SWAP;
        BachWriteReg(BACH_REG_BANK1, BACH_RX_FIFO_CTRL0, REG_SPDIF_TX_L_SWAP, nConfigValue);
        break;
    case BACH_MUX2_SPDIF_TX_CH2:
        if(u8Choice)
            nConfigValue |= REG_SPDIF_TX_R_SWAP;
        BachWriteReg(BACH_REG_BANK1, BACH_RX_FIFO_CTRL0, REG_SPDIF_TX_R_SWAP, nConfigValue);
        break;
    case BACH_MUX2_HDMI_TX_CH1:
        if(u8Choice)
            nConfigValue |= REG_HDMI_TX_L_SWAP;
        BachWriteReg(BACH_REG_BANK1, BACH_RX_FIFO_CTRL0, REG_HDMI_TX_L_SWAP, nConfigValue);
        break;
    case BACH_MUX2_HDMI_TX_CH2:
        if(u8Choice)
            nConfigValue |= REG_HDMI_TX_R_SWAP;
        BachWriteReg(BACH_REG_BANK1, BACH_RX_FIFO_CTRL0, REG_HDMI_TX_R_SWAP, nConfigValue);
        break;
    default:
        ERRMSG("WAVEDEV.DLL: BachSetMux2 - ERROR MUX2 default case!\n");
        return;
    }
}

void BachSetMixer(BachMixer_e eMixer, U8 nCh1, U8 nCh2)
{
//	TRACE2("BachSetMixer BachMixer_e = %d, nCh1 = %d", eMixer, nCh1);

    switch(eMixer)
    {
    case BACH_MIXER_DEC1_2:
        BachWriteReg(BACH_REG_BANK1, BACH_SINE_GEN_CTRL0, REG_INT1_MIX_CTRL_MSK, (nCh1 << REG_INT1_MIX_CTRL_POS));
        BachWriteReg(BACH_REG_BANK1, BACH_SINE_GEN_CTRL0, REG_INT2_MIX_CTRL_MSK, (nCh2 << REG_INT2_MIX_CTRL_POS));
        break;
    case BACH_MIXER_DEC3_4:
        BachWriteReg(BACH_REG_BANK2, BACH_AUDIOBAND_CTRL2, REG_INT3_MIX_CTRL_MSK, (nCh1 << REG_INT3_MIX_CTRL_POS));
        BachWriteReg(BACH_REG_BANK2, BACH_AUDIOBAND_CTRL2, REG_INT4_MIX_CTRL_MSK, (nCh2 << REG_INT4_MIX_CTRL_POS));
        break;
    case BACH_MIXER_ASRC2_DEC1_2:
        BachWriteReg(BACH_REG_BANK1, BACH_SINE_GEN_CTRL0, REG_ASRC2_INT1_MIX_CTRL_MSK, (nCh1 << REG_ASRC2_INT1_MIX_CTRL_POS));
        BachWriteReg(BACH_REG_BANK1, BACH_SINE_GEN_CTRL0, REG_ASRC2_INT2_MIX_CTRL_MSK, (nCh2 << REG_ASRC2_INT2_MIX_CTRL_POS));
        break;
    case BACH_MIXER_ASRC2_DEC3_4:
        BachWriteReg(BACH_REG_BANK2, BACH_AUDIOBAND_CTRL2, REG_ASRC2_INT3_MIX_CTRL_MSK, (nCh1 << REG_ASRC2_INT3_MIX_CTRL_POS));
        BachWriteReg(BACH_REG_BANK2, BACH_AUDIOBAND_CTRL2, REG_ASRC2_INT4_MIX_CTRL_MSK, (nCh2 << REG_ASRC2_INT4_MIX_CTRL_POS));
        break;
    case BACH_MIXER_SDM_DWA1:
        BachWriteReg(BACH_REG_BANK1, BACH_AUDIOBAND_CTRL1, REG_SDM_DWA_DATAIN_L_SEL_MSK, (nCh1 << REG_SDM_DWA_DATAIN_L_SEL_POS));
        BachWriteReg(BACH_REG_BANK1, BACH_AUDIOBAND_CTRL1, REG_SDM_DWA_DATAIN_R_SEL_MSK, (nCh2 << REG_SDM_DWA_DATAIN_R_SEL_POS));
        break;
    case BACH_MIXER_SDM_DWA2:
        BachWriteReg(BACH_REG_BANK2, BACH_AUDIOBAND_CTRL3, REG_SDM_DWA_DATAIN_L_SEL2_MSK, (nCh1 << REG_SDM_DWA_DATAIN_L_SEL2_POS));
        BachWriteReg(BACH_REG_BANK2, BACH_AUDIOBAND_CTRL3, REG_SDM_DWA_DATAIN_R_SEL2_MSK, (nCh2 << REG_SDM_DWA_DATAIN_R_SEL2_POS));
        break;
    case BACH_MIXER_AMR_MIX:
        BachWriteReg(BACH_REG_BANK1, BACH_MIX_CTRL0, REG_AMR_MIX_MSK, (nCh1 << REG_AMR_MIX_POS));
        break;
    case BACH_MIXER_DBB_UL_MIX:
        BachWriteReg(BACH_REG_BANK1, BACH_MIX_CTRL0, REG_DBB_UL_MIX_MSK, (nCh1 << REG_DBB_UL_MIX_POS));
        break;
    case BACH_MIXER_ST_MIX:
        BachWriteReg(BACH_REG_BANK1, BACH_MIX_CTRL0, REG_ST_MIX_MSK, (nCh1 << REG_ST_MIX_POS));
        break;
    case BACH_MIXER_AV_MIX:
        BachWriteReg(BACH_REG_BANK1, BACH_MIX_CTRL0, REG_AV_MIX_MSK, (nCh1 << REG_AV_MIX_POS));
        break;
    default:
        ERRMSG("WAVEDEV.DLL: BachSetMixer - ERROR default BachMixer_e case!\n");
        return;
    }
}


void BachSetSynthRate(BachSynth_e eSynth, U32 nRate)
{
    BachRegBank_e eBank;
    U8  nAddr;
    U16	nValue;

    eBank  = (BachRegBank_e)(g_nSynthRegTbl[eSynth][BACH_SYNTH_REG_BANK]);
    nAddr  = (U8) (g_nSynthRegTbl[eSynth][BACH_SYNTH_REG_NF_H]);
    nValue = (U16)(nRate >> 16);

    BachWriteReg(eBank, nAddr, REG_NF_SYNTH_VALUE_HI_MSK, nValue);	//write nf hi rate register

    nAddr  = (U8) (g_nSynthRegTbl[eSynth][BACH_SYNTH_REG_NF_L]);
    nValue = (U16)(nRate & 0xffff);

    BachWriteReg(eBank, nAddr, REG_NF_SYNTH_VALUE_LO_MSK, nValue);	//write nf lo rate register

    nAddr = (U8) (g_nSynthRegTbl[eSynth][BACH_SYNTH_REG_NF_TRIG]);

    BachWriteReg(eBank, nAddr, REG_NF_SYNTH_TRIG, REG_NF_SYNTH_TRIG);	//trigger nf rate register
    BachWriteReg(eBank, nAddr, REG_NF_SYNTH_TRIG, 0);
}

BOOL BachSetAsrcSynth(BachAsrc_e eAsrc, BachSynth_e eSynth)
{
    BachRegBank_e eBank;
    U8  nAddr;
    U16	nMask, nValue;
//	TRACE2("BachSetAsrcSynth BachAsrc_e %d, BachSynth_e %d",eAsrc,eSynth);
    if (g_nAsrcSynthAvailMatrix[eAsrc][eSynth] == 0xff)
        return FALSE;

    eBank = (BachRegBank_e)(g_nAsrcRegTbl[eAsrc][BACH_ASRC_REG_FS_BANK]);
    nAddr = (U8)(g_nAsrcRegTbl[eAsrc][BACH_ASRC_REG_FS_REG]);
    nMask = g_nAsrcRegTbl[eAsrc][BACH_ASRC_REG_FS_SEL];
    nValue = 0;
    if (BACH_FS_SYNTH == eSynth)
    {
        if(g_nAsrcRegTbl[eAsrc][BACH_ASRC_REG_FS_BANK] == 0xff)
            return FALSE;
        nValue = nMask;
        BachWriteReg(eBank, nAddr, nMask, nValue);		//write fs select register
        return TRUE;
    }

    if(g_nAsrcRegTbl[eAsrc][BACH_ASRC_REG_FS_BANK] != 0xff)
    {
        BachWriteReg(eBank, nAddr, nMask, nValue);		//write fs select register
        if(g_nAsrcRegTbl[eAsrc][BACH_ASRC_REG_NF_BANK] == 0xff)
            return TRUE;
    }

    if(g_nAsrcRegTbl[eAsrc][BACH_ASRC_REG_NF_BANK] == 0xff)
        return FALSE;

    eBank = (BachRegBank_e)(g_nAsrcRegTbl[eAsrc][BACH_ASRC_REG_NF_BANK]);
    nAddr = (U8)(g_nAsrcRegTbl[eAsrc][BACH_ASRC_REG_NF_REG]);
    nMask = g_nAsrcRegTbl[eAsrc][BACH_ASRC_REG_NF_SEL];
    nValue = 0;
    if (g_nAsrcSynthAvailMatrix[eAsrc][eSynth])
        nValue = g_nAsrcRegTbl[eAsrc][BACH_ASRC_REG_NF_SEL];

    BachWriteReg(eBank, nAddr, nMask, nValue);		//write nf select register

    return TRUE;
}

BOOL BachSetI2sRate(BachI2s_e eI2S, BachRate_e eRate)
{
    BachRegBank_e eBank;
    U8	nAddr = 0;
    U16 nConfigValue = 0;
    U16 nRegBckPos = 0;
    U16 nRegBckMsk = 0;
    U16 nRegMsMode = 0;

    //U16 nAddr2 = 0;
    //U16 nRegWckOen = 0;
    //U16 nRegBckOen = 0;
    //U16 nRegSdiOen = 0;
    //U16 nRegSdoOen = 0;

//	TRACE2("BachSetI2sRate BachI2s_e = %d, BachRate_e = %d", eI2S, eRate);

    switch(eI2S)
    {
    case BACH_I2S_EXT_RX:
        eBank = BACH_REG_BANK2;
        nAddr = BACH_EXT_I2S_CTRL;
        nRegBckPos = REG_EXT_I2S_BCK_GEN_SEL_POS;
        nRegBckMsk = REG_EXT_I2S_BCK_GEN_SEL_MSK;
        nRegMsMode =  REG_EXT_MS_MODE;

        break;
    case BACH_I2S_BT_TRX:
        eBank = BACH_REG_BANK1;
        nAddr = BACH_BT_I2S_CTRL0;
        nRegBckPos = REG_BT_I2S_BCK_GEN_SEL_POS;
        nRegBckMsk = REG_BT_I2S_BCK_GEN_SEL_MSK;
        nRegMsMode =  REG_BT_MS_MODE;
        break;
    case BACH_I2S_CODEC_TX:
        eBank = BACH_REG_BANK2;
        nAddr = BACH_CODEC_I2S_CTRL;
        nRegBckPos = REG_CODEC_I2S_BCK_GEN_SEL_POS;
        nRegBckMsk = REG_CODEC_I2S_BCK_GEN_SEL_MSK;
        nRegMsMode =  REG_CODEC_MS_MODE;
        break;
    case BACH_I2S_DBB_TRX:
        eBank = BACH_REG_BANK2;
        nAddr = BACH_DBB_I2S_CTRL;
        nRegBckPos = REG_DBB_I2S_BCK_GEN_SEL_POS;
        nRegBckMsk = REG_DBB_I2S_BCK_GEN_SEL_MSK;
        nRegMsMode =  REG_DBB_MS_MODE;
        break;
    default:
        ERRMSG("BachI2SSetRate - ERROR BachI2s_e case!\n");
        return FALSE;
    }

    switch(eRate)
    {
    case BACH_RATE_SLAVE:
        BachWriteReg(eBank, nAddr, nRegMsMode, 0);	//slave mode
        return TRUE;
    case BACH_RATE_8K:
        nConfigValue |= (0x0 << nRegBckPos);
        break;
    case BACH_RATE_16K:
        nConfigValue |= (0x1 << nRegBckPos);
        break;
    case BACH_RATE_32K:
        nConfigValue |= (0x2 << nRegBckPos);
        break;
    case BACH_RATE_48K:
        nConfigValue |= (0x3 << nRegBckPos);
        break;
    default:
        ERRMSG("BachI2SSetRate - ERROR BachRate_e case!\n");
        return FALSE;
    }
    BachWriteReg(eBank, nAddr, nRegMsMode, nRegMsMode);	//master mode
    BachWriteReg(eBank, nAddr, nRegBckMsk, nConfigValue); //set rate

    return TRUE;

}

BOOL BachSetI2sMode(BachI2s_e eI2S, BachI2sPcmMode_e eMode)
{
    BachRegBank_e eBank;
    U8	nAddr = 0;
    //U16 nConfigValue = 0;
    U16 nRegMode = 0;

//	TRACE2("BachSetI2sMode BachI2s_e = %d, BachI2sPcmMode_e = %d", eI2S, eMode);

    switch(eI2S)
    {
    case BACH_I2S_EXT_RX:
        return TRUE;
        break;
    case BACH_I2S_BT_TRX:
        eBank = BACH_REG_BANK1;
        nAddr = BACH_BT_I2S_CTRL0;
        nRegMode =  REG_BT_PCM_MODE;
        break;
    case BACH_I2S_CODEC_TX:
        eBank = BACH_REG_BANK2;
        nAddr = BACH_CODEC_I2S_CTRL;
        nRegMode =  REG_CODEC_PCM_MODE;
        break;
    case BACH_I2S_DBB_TRX:
        eBank = BACH_REG_BANK2;
        nAddr = BACH_DBB_I2S_CTRL;
        nRegMode =  REG_DBB_PCM_MODE;
        break;
    default:
        ERRMSG("BachI2SSetFmt - ERROR BachI2s_e case!\n");
        return FALSE;
    }

    switch(eMode)
    {
    case BACH_PCM_MODE_I2S:
        BachWriteReg(eBank, nAddr, nRegMode, 0);
        break;
    case BACH_PCM_MODE_PCM:
        BachWriteReg(eBank, nAddr, nRegMode, nRegMode);
        break;
    default:
        ERRMSG("BachSetI2sMode - ERROR BachI2sPcmMode_e case!\n");
        return FALSE;
    }

    return TRUE;
}


BOOL BachSetI2sFmt(BachI2s_e eI2S, BachI2sFmt_e eFmt)
{
    BachRegBank_e eBank;
    U8	nAddr = 0;
    //U16 nConfigValue = 0;
    U16 nRegFmt = 0;

    //TRACE2("BachSetI2sFmt BachI2s_e = %d, BachI2sFmt_e = %d", eI2S, eFmt);

    switch(eI2S)
    {
    case BACH_I2S_EXT_RX:
        eBank = BACH_REG_BANK2;
        nAddr = BACH_EXT_I2S_CTRL;
        nRegFmt =  REG_EXT_I2S_FMT;
        break;
    case BACH_I2S_BT_TRX:
        eBank = BACH_REG_BANK1;
        nAddr = BACH_BT_I2S_CTRL0;
        nRegFmt =  REG_BT_I2S_FMT;
        break;
    case BACH_I2S_CODEC_TX:
        eBank = BACH_REG_BANK2;
        nAddr = BACH_CODEC_I2S_CTRL;
        nRegFmt =  REG_CODEC_I2S_FMT;
        break;
    case BACH_I2S_DBB_TRX:
        eBank = BACH_REG_BANK2;
        nAddr = BACH_DBB_I2S_CTRL;
        nRegFmt =  REG_DBB_I2S_FMT;
        break;
    default:
        ERRMSG("WAVEDEV.DLL: BachI2SSetFmt - ERROR BachI2s_e case!\n");
        return FALSE;
    }

    switch(eFmt)
    {
    case BACH_I2S_FMT_I2S:
        BachWriteReg(eBank, nAddr, nRegFmt, 0);
        break;
    case BACH_I2S_FMT_LEFT_JUSTIFY:
        BachWriteReg(eBank, nAddr, nRegFmt, nRegFmt);
        break;
    default:
        ERRMSG("WAVEDEV.DLL: BachSetI2sFmt - ERROR BachI2sFmt_e case!\n");
        return FALSE;
    }

    return TRUE;
}


BOOL BachSetI2sWidth(BachI2s_e eI2S, BachI2sWidth_e eWidth)
{
    BachRegBank_e eBank;
    U8	nAddr = 0;
    U16 nConfigValue = 0;
    U16 nRegWdthPos = 0;
    U16 nRegWdthMsk = 0;

    //TRACE2("BachSetI2sWidth BachI2s_e = %d, BachI2sWidth_e = %d", eI2S, eWidth);

    switch(eI2S)
    {
    case BACH_I2S_EXT_RX:
        eBank = BACH_REG_BANK2;
        nAddr = BACH_EXT_I2S_CTRL;
        nRegWdthPos = REG_EXT_I2S_WDTH_POS;
        nRegWdthMsk = REG_EXT_I2S_WDTH_MSK;
        break;
    case BACH_I2S_BT_TRX:
        eBank = BACH_REG_BANK1;
        nAddr = BACH_BT_I2S_CTRL0;
        nRegWdthPos = REG_BT_I2S_WDTH_POS;
        nRegWdthMsk = REG_BT_I2S_WDTH_MSK;
        break;
    case BACH_I2S_CODEC_TX:
        eBank = BACH_REG_BANK2;
        nAddr = BACH_CODEC_I2S_CTRL;
        nRegWdthPos = REG_CODEC_I2S_WDTH_POS;
        nRegWdthMsk = REG_CODEC_I2S_WDTH_MSK;
        break;
    case BACH_I2S_DBB_TRX:
        eBank = BACH_REG_BANK2;
        nAddr = BACH_DBB_I2S_CTRL;
        nRegWdthPos = REG_DBB_I2S_WDTH_POS;
        nRegWdthMsk = REG_DBB_I2S_WDTH_MSK;
        break;
    default:
        ERRMSG("BachSetI2sWidth - ERROR BachI2s_e case!\n");
        return FALSE;
    }

    switch(eWidth)
    {
    case BACH_I2S_WIDTH_16:
        nConfigValue |= (0x0 << nRegWdthPos);
        break;
    case BACH_I2S_WIDTH_24:
        nConfigValue |= (0x1 << nRegWdthPos);
        break;
    case BACH_I2S_WIDTH_32:
        nConfigValue |= (0x2 << nRegWdthPos);
        break;
    default:
        ERRMSG("BachSetI2sWidth - ERROR BachI2sWidth_e case!\n");
        return FALSE;
    }

    BachWriteReg(eBank, nAddr, nRegWdthMsk, nConfigValue);
    return TRUE;

}

BOOL BachSetI2sBck(BachI2s_e eI2S, BOOL bEn)
{
    BachRegBank_e eBank;
    U8	nAddr = 0;
    //U16 nConfigValue = 0;
    U16 nReg = 0;

    //TRACE2("BachSetI2sFmt BachI2s_e = %d, BachI2sFmt_e = %d", eI2S, eFmt);

    switch(eI2S)
    {
    case BACH_I2S_EXT_RX:
        eBank = BACH_REG_BANK2;
        nAddr = BACH_EXT_I2S_CTRL;
        nReg =  REG_EXT_I2S_BCK_GEN_UPDATE;
        break;
    case BACH_I2S_BT_TRX:
        eBank = BACH_REG_BANK1;
        nAddr = BACH_BT_I2S_CTRL0;
        nReg =  REG_BT_I2S_BCK_GEN_UPDATE;
        break;
    case BACH_I2S_CODEC_TX:
        eBank = BACH_REG_BANK2;
        nAddr = BACH_CODEC_I2S_CTRL;
        nReg =  REG_CODEC_I2S_BCK_GEN_UPDATE;
        break;
    case BACH_I2S_DBB_TRX:
        eBank = BACH_REG_BANK2;
        nAddr = BACH_DBB_I2S_CTRL;
        nReg =  REG_DBB_I2S_BCK_GEN_UPDATE;
        break;
    default:
        ERRMSG("WAVEDEV.DLL: BachSetI2sBck - ERROR BachI2s_e case!\n");
        return FALSE;
    }

    if(bEn)
        BachWriteReg(eBank, nAddr, nReg, 0);
    else
        BachWriteReg(eBank, nAddr, nReg, nReg);

    return TRUE;
}

void BachSelectDAC1Input(BachChannelMode_e eChannel)
{
//	TRACE1("BachSelectDAC1Input BachDacType_e = %d", eType);

    //set MUX2
    switch(eChannel)
    {
    case BACH_CHMODE_BOTH:
        BachPgaEnable(BACH_PGA1, FALSE);
        BachPgaEnable(BACH_PGA2, FALSE);
        BachSetMux2(BACH_MUX2_SDM_DWA1_CH1, 0);
        BachSetMux2(BACH_MUX2_SDM_DWA1_CH2, 0);
        break;
    case BACH_CHMODE_LEFT:
        //Fade out before switch left channel DAC in order to reduce pop noise
        BachPgaMute(BACH_PGA2);

        BachSetMux2(BACH_MUX2_SDM_DWA1_CH1, 0);
        BachSetMux2(BACH_MUX2_SDM_DWA1_CH2, 1);
        break;
    case BACH_CHMODE_RIGHT:
        //Fade out before switch right channel DAC in order to reduce pop noise
        BachPgaMute(BACH_PGA1);

        BachSetMux2(BACH_MUX2_SDM_DWA1_CH1, 1);
        BachSetMux2(BACH_MUX2_SDM_DWA1_CH2, 0);
        break;
    default:
        ERRMSG("BachSelectDAC1Input - ERROR default case!\n");
        return;
    }
    //set S2M,1AA2[13:10]
    //BachWriteReg(BACH_REG_BANK1,BACH_AUDIOBAND_CTRL1,(REG_SDM_DWA_DATAIN_L_SEL_MSK|REG_SDM_DWA_DATAIN_R_SEL_MSK),(0xA<<REG_SDM_DWA_DATAIN_R_SEL_POS));
    //BachSetMixer(BACH_MIXER_SDM_DWA1, 2, 2);

}

void BachSelectDAC2Input(BachChannelMode_e eChannel)
{
//	TRACE1("BachSelectDAC2Input BachDacType_e = %d", eType);

    //set MUX2
    switch(eChannel)
    {
    case BACH_CHMODE_BOTH:
        BachPgaEnable(BACH_PGA3, FALSE);
        BachPgaEnable(BACH_PGA4, FALSE);
        BachSetMux2(BACH_MUX2_SDM_DWA2_CH1, 0);
        BachSetMux2(BACH_MUX2_SDM_DWA2_CH2, 0);
        break;
    case BACH_CHMODE_LEFT:
        //Fade out before switch left channel DAC in order to reduce pop noise
        //BachPgaSetGain(BACH_PGA4,-45);
        //BachPgaCtrl(BACH_PGA4,TRUE,TRUE);
        BachPgaMute(BACH_PGA4);
        BachSetMux2(BACH_MUX2_SDM_DWA2_CH1, 0);
        BachSetMux2(BACH_MUX2_SDM_DWA2_CH2, 1);
        break;
    case BACH_CHMODE_RIGHT:
        //Fade out before switch right channel DAC in order to reduce pop noise
        //BachPgaSetGain(BACH_PGA3,-45);
        //BachPgaCtrl(BACH_PGA3,TRUE,TRUE);
        BachPgaMute(BACH_PGA3);
        BachSetMux2(BACH_MUX2_SDM_DWA2_CH1, 1);
        BachSetMux2(BACH_MUX2_SDM_DWA2_CH2, 0);
        break;
    default:
        ERRMSG("BachSelectDAC2Input - ERROR default case!\n");
        return;
    }
    //set S2M,1B02[7:4]
    //BachWriteReg(BACH_REG_BANK2,BACH_AUDIOBAND_CTRL3,(REG_SDM_DWA_DATAIN_L_SEL2_MSK|REG_SDM_DWA_DATAIN_R_SEL2_MSK),(0xA<<REG_SDM_DWA_DATAIN_R_SEL2_POS));
    //BachSetMixer(BACH_MIXER_SDM_DWA2, 2, 2);
}

void BachSelectSinkTxInput(BachSinkTx_e eSink, BachChannelMode_e eChannel)
{
    BachMux2_e eMuxL, eMuxR;
    switch(eSink)
    {
    case BACH_SINKTX_CODEC_I2S:
        eMuxL = BACH_MUX2_CODEC_TX_CH1;
        eMuxR = BACH_MUX2_CODEC_TX_CH2;
        break;
    case BACH_SINKTX_SPDIF:
        eMuxL = BACH_MUX2_SPDIF_TX_CH1;
        eMuxR = BACH_MUX2_SPDIF_TX_CH2;
        break;
    case BACH_SINKTX_HDMI:
        eMuxL = BACH_MUX2_HDMI_TX_CH1;
        eMuxR = BACH_MUX2_HDMI_TX_CH2;
        break;
    default:
        ERRMSG("BachSelectSinkTxInput - Not Available Sink Tx\n");
        return;
    }


    //set MUX2
    switch(eChannel)
    {
    case BACH_CHMODE_BOTH:
        BachSetMux2(eMuxL, 0);
        BachSetMux2(eMuxR, 0);
        break;
    case BACH_CHMODE_LEFT:
        BachSetMux2(eMuxL, 0);
        BachSetMux2(eMuxR, 1);
        break;
    case BACH_CHMODE_RIGHT:
        BachSetMux2(eMuxL, 1);
        BachSetMux2(eMuxR, 0);
        break;
    default:
        ERRMSG("BachSelectSinkTxInput - ERROR default channel mode!\n");
        return;
    }
}
void BachPgaInit(BachPga_e ePga)
{
    BachPgaSetGain(ePga, 0);
    BachPgaCtrl(ePga, TRUE, TRUE);
}


void BachPgaMute(BachPga_e ePga)
{
    //PgaSetGain(ePga,BACH_PGA_GAIN_MIN_DB);
    BachPgaCtrl(ePga, TRUE, TRUE);
}

void BachPgaEnable(BachPga_e ePga, BOOL bEn)
{
    //PgaSetGain(ePga,BACH_PGA_GAIN_MIN_DB);
    BachPgaCtrl(ePga, bEn, TRUE);
}


void BachPgaCtrl(BachPga_e ePga, BOOL bEnable, BOOL bMute)
{
    U16 nConfigValue = 0;

    switch(ePga)
    {
    case BACH_PGA1:
        if(bEnable)
            nConfigValue |= REG_PGA1_EN;
        if(bMute)
            nConfigValue |= REG_PGA1_MUTE;
        BachWriteReg(BACH_REG_BANK4, BACH_PGA1_DPGA_CFG1, (REG_PGA1_EN | REG_PGA1_MUTE), nConfigValue);
        break;
    case BACH_PGA2:
        if(bEnable)
            nConfigValue |= REG_PGA2_EN;
        if(bMute)
            nConfigValue |= REG_PGA2_MUTE;
        BachWriteReg(BACH_REG_BANK4, BACH_PGA1_DPGA_CFG1, (REG_PGA2_EN | REG_PGA2_MUTE), nConfigValue);
        break;
    case BACH_PGA3:
        if(bEnable)
            nConfigValue |= REG_PGA3_EN;
        if(bMute)
            nConfigValue |= REG_PGA3_MUTE;
        BachWriteReg(BACH_REG_BANK4, BACH_PGA2_DPGA_CFG1, (REG_PGA3_EN | REG_PGA3_MUTE), nConfigValue);
        break;
    case BACH_PGA4:
        if(bEnable)
            nConfigValue |= REG_PGA4_EN;
        if(bMute)
            nConfigValue |= REG_PGA4_MUTE;
        BachWriteReg(BACH_REG_BANK4, BACH_PGA2_DPGA_CFG1, (REG_PGA4_EN | REG_PGA4_MUTE), nConfigValue);
        break;
    default:
        ERRMSG("BachPgaCtrl - ERROR default case!\n");
        return;
    }
}


void BachPgaCalGain(S8 s8Gain, U8 *pu8GainIdx) //we can get just 3*n dB
{
    if(s8Gain > BACH_PGA_GAIN_MAX_DB)
        s8Gain = BACH_PGA_GAIN_MAX_DB;
    else if(s8Gain < BACH_PGA_GAIN_MIN_DB)
        s8Gain = BACH_PGA_GAIN_MIN_DB;

    *pu8GainIdx = (U8)(-s8Gain / 3); //index = - (gain/3) ,because step = -3dB
}

S8	BachPgaGetGain(BachPga_e ePga)
{
    U16 nConfigValue;
    U8 nGainIdx;
    S8 nGain;
    switch(ePga)
    {
    case BACH_PGA1:
        nConfigValue = BachReadReg(BACH_REG_BANK4, BACH_PGA1_DPGA_CFG1);
        nGainIdx = (U8)((nConfigValue & REG_PGA1_GAIN_MSK) >> REG_PGA1_GAIN_POS);
        break;
    case BACH_PGA2:
        nConfigValue = BachReadReg(BACH_REG_BANK4, BACH_PGA1_DPGA_CFG1);
        nGainIdx = (U8)(nConfigValue & REG_PGA2_GAIN_MSK) >> REG_PGA2_GAIN_POS;
        break;
    case BACH_PGA3:
        nConfigValue = BachReadReg(BACH_REG_BANK4, BACH_PGA2_DPGA_CFG1);
        nGainIdx = (U8)((nConfigValue & REG_PGA3_GAIN_MSK) >> REG_PGA3_GAIN_POS);
        break;
    case BACH_PGA4:
        nConfigValue = BachReadReg(BACH_REG_BANK4, BACH_PGA2_DPGA_CFG1);
        nGainIdx = (U8)((nConfigValue & REG_PGA4_GAIN_MSK) >> REG_PGA4_GAIN_POS);
        break;
    default:
        ERRMSG("BachPgaGetGain - ERROR default case!\n");
        return 0;
    }
    nGain = -(S8)(nGainIdx * 3);
    return nGain;
}

void BachPgaSetGain(BachPga_e ePga, S8 s8Gain)
{
    U8 nGainIdx;

    BachPgaCalGain(s8Gain, &nGainIdx);

    switch(ePga)
    {
    case BACH_PGA1:
        //set gain
        BachWriteReg(BACH_REG_BANK4, BACH_PGA1_DPGA_CFG1, REG_PGA1_GAIN_MSK, (nGainIdx << REG_PGA1_GAIN_POS));

        //trigger
        BachDpgaTrigger(BACH_PGA1_DPGA_CFG1, REG_PGA1_GAIN_TRIG);
        break;
    case BACH_PGA2:
        //set gain
        BachWriteReg(BACH_REG_BANK4, BACH_PGA1_DPGA_CFG1, REG_PGA2_GAIN_MSK, (nGainIdx << REG_PGA2_GAIN_POS));

        //trigger
        BachDpgaTrigger(BACH_PGA1_DPGA_CFG1, REG_PGA2_GAIN_TRIG);
        break;
    case BACH_PGA3:
        //set gain
        BachWriteReg(BACH_REG_BANK4, BACH_PGA2_DPGA_CFG1, REG_PGA3_GAIN_MSK, (nGainIdx << REG_PGA3_GAIN_POS));

        //trigger
        BachDpgaTrigger(BACH_PGA2_DPGA_CFG1, REG_PGA3_GAIN_TRIG);
        break;
    case BACH_PGA4:
        //set gain
        BachWriteReg(BACH_REG_BANK4, BACH_PGA2_DPGA_CFG1, REG_PGA4_GAIN_MSK, (nGainIdx << REG_PGA4_GAIN_POS));

        //trigger
        BachDpgaTrigger(BACH_PGA2_DPGA_CFG1, REG_PGA4_GAIN_TRIG);
        break;
    default:
        ERRMSG("BachPgaSetGain - ERROR default case!\n");
        return;
    }
}

void BachDpgaGainInit()
{
    int i;
    for(i = 0; i < BACH_DPGA_NUM; i++)
    {
        BachDpgaSetGain((BachDpga_e)i, 0);
        //RETAILMSG(1,(TEXT("!!!!!!!!!!!!!!!! Gain%d : %d\r\n"),i,BachDpgaGetGain((BachDpga_e)i)));
    }
    //RETAILMSG(1,(TEXT("!!!!!!!!!!!!!!!!!!!void BachDpgaGainInit()\r\n")));
}

void BachDpgaInit(BachDpga_e eDpga, S8 nGain)
{
    BachDpgaSetGain(eDpga, nGain);
    BachDpgaCtrl(eDpga, TRUE, TRUE, TRUE);
}


void BachDpgaTrigger(U8 nAddr, U16 trigBit)
{
    U16 nConfigValue;
    //nConfigValue = ReadBank4Reg(nAddr);
    nConfigValue = READ_WORD(m_pAudBank4RegAddr + ((nAddr) << 1));
    nConfigValue |= trigBit;
    WRITE_WORD(m_pAudBank4RegAddr + ((nAddr) << 1), nConfigValue);

    nConfigValue &= ~trigBit;
    WRITE_WORD(m_pAudBank4RegAddr + ((nAddr) << 1), nConfigValue);
}

void BachDpgaMute(BachDpga_e eDpga)
{
    BachDpgaSetGain(eDpga, BACH_DPGA_GAIN_MIN_DB);
    BachDpgaCtrl(eDpga, TRUE, TRUE, TRUE);
}

void BachDpgaEnable(BachDpga_e eDpga, BOOL bEn)
{
    //BachDpgaSetGain(eDpga, BACH_DPGA_GAIN_MIN_DB);
    BachDpgaCtrl(eDpga, bEn, TRUE, TRUE);
}

void BachDpgaCtrl(BachDpga_e eDpga, BOOL bEnable, BOOL bMute, BOOL bFade)
{
    U8 nAddr;
    U16 nConfigValue;
    switch(eDpga)
    {
    case BACH_DPGA_MMP1:
        nAddr = BACH_MMP1_DPGA_CFG1;
        break;
    case BACH_DPGA_MMP2:
        nAddr = BACH_MMP2_DPGA_CFG1;
        break;
    case BACH_DPGA_CODEC1:
        nAddr = BACH_CODEC1_DPGA_CFG1;
        break;
    case BACH_DPGA_CODEC2:
        nAddr = BACH_CODEC2_DPGA_CFG1;
        break;
    case BACH_DPGA_DBB_1:
        nAddr = BACH_DBB_1_DPGA_CFG1;
        break;
    case BACH_DPGA_DEC1_1:
        nAddr = BACH_DEC1_1_DPGA_CFG1;
        break;
    case BACH_DPGA_DMARD_L:
        nAddr = BACH_DMARD_L_DPGA_CFG1;
        break;
    case BACH_DPGA_DEC1_2:
        nAddr = BACH_DEC1_2_DPGA_CFG1;
        break;
    case BACH_DPGA_DMARD_R:
        nAddr = BACH_DMARD_R_DPGA_CFG1;
        break;
    case BACH_DPGA_ASRC2MMP1:
        nAddr = BACH_ASRC2_MMP1_DPGA_CFG1;
        break;
    case BACH_DPGA_ASRC2MMP2:
        nAddr = BACH_ASRC2_MMP2_DPGA_CFG1;
        break;
    case BACH_DPGA_SPDIF1:
        nAddr = BACH_SPDIF1_DPGA_CFG1;
        break;
    case BACH_DPGA_SPDIF2:
        nAddr = BACH_SPDIF2_DPGA_CFG1;
        break;
    case BACH_DPGA_DMAWR3:
        nAddr = BACH_DMAWR3_DPGA_CFG1;
        break;
    case BACH_DPGA_DMAWR4:
        nAddr = BACH_DMAWR4_DPGA_CFG1;
        break;
    case BACH_DPGA_DEC1_3:
        nAddr = BACH_DEC1_3_DPGA_CFG1;
        break;
    case BACH_DPGA_DBB_2:
        nAddr = BACH_DBB_2_DPGA_CFG1;
        break;
    case BACH_DPGA_DMAWR1:
        nAddr = BACH_DMAWR1_DPGA_CFG1;
        break;
    case BACH_DPGA_DMAWR2:
        nAddr = BACH_DMAWR2_DPGA_CFG1;
        break;
    default:
        ERRMSG("BachDpgaCtrl - ERROR default case!\n");
        return;
    }

    nConfigValue = 0;
    if(bEnable)
        nConfigValue |= DPGA_EN;
    if(bMute)
        nConfigValue |= MUTE_2_ZERO;
    if(bFade)
        nConfigValue |= FADING_EN;

    BachWriteReg(BACH_REG_BANK4, nAddr, (DPGA_EN | MUTE_2_ZERO | FADING_EN), nConfigValue);
}

void BachDpgaFadingSize(BachDpga_e eDpga, U8 nStepIdx)
{
    U8 nAddr;
    switch(eDpga)
    {
    case BACH_DPGA_MMP1:
        nAddr = BACH_MMP1_DPGA_CFG1;
        break;
    case BACH_DPGA_MMP2:
        nAddr = BACH_MMP2_DPGA_CFG1;
        break;
    case BACH_DPGA_CODEC1:
        nAddr = BACH_CODEC1_DPGA_CFG1;
        break;
    case BACH_DPGA_CODEC2:
        nAddr = BACH_CODEC2_DPGA_CFG1;
        break;
    case BACH_DPGA_DBB_1:
        nAddr = BACH_DBB_1_DPGA_CFG1;
        break;
    case BACH_DPGA_DEC1_1:
        nAddr = BACH_DEC1_1_DPGA_CFG1;
        break;
    case BACH_DPGA_DMARD_L:
        nAddr = BACH_DMARD_L_DPGA_CFG1;
        break;
    case BACH_DPGA_DEC1_2:
        nAddr = BACH_DEC1_2_DPGA_CFG1;
        break;
    case BACH_DPGA_DMARD_R:
        nAddr = BACH_DMARD_R_DPGA_CFG1;
        break;
    case BACH_DPGA_ASRC2MMP1:
        nAddr = BACH_ASRC2_MMP1_DPGA_CFG1;
        break;
    case BACH_DPGA_ASRC2MMP2:
        nAddr = BACH_ASRC2_MMP2_DPGA_CFG1;
        break;
    case BACH_DPGA_SPDIF1:
        nAddr = BACH_SPDIF1_DPGA_CFG1;
        break;
    case BACH_DPGA_SPDIF2:
        nAddr = BACH_SPDIF2_DPGA_CFG1;
        break;
    case BACH_DPGA_DMAWR3:
        nAddr = BACH_DMAWR3_DPGA_CFG1;
        break;
    case BACH_DPGA_DMAWR4:
        nAddr = BACH_DMAWR4_DPGA_CFG1;
        break;
    case BACH_DPGA_DEC1_3:
        nAddr = BACH_DEC1_3_DPGA_CFG1;
        break;
    case BACH_DPGA_DBB_2:
        nAddr = BACH_DBB_2_DPGA_CFG1;
        break;
    case BACH_DPGA_DMAWR1:
        nAddr = BACH_DMAWR1_DPGA_CFG1;
        break;
    case BACH_DPGA_DMAWR2:
        nAddr = BACH_DMAWR2_DPGA_CFG1;
        break;
    default:
        ERRMSG("WAVEDEV.DLL: BachDpgaFadingSize - ERROR default case!");
        return;
    }

    if(nStepIdx > 6 && nStepIdx < 0)
        ERRMSG("WAVEDEV.DLL: BachDpgaFadingSize - ERROR Step Index!");

    BachWriteReg(BACH_REG_BANK4, nAddr, STEP_MSK, nStepIdx << STEP_POS);
}


void BachDpgaCalGain(S8 s8Gain, U8 *pu8GainIdx) //ori step:0.5dB,new step 1dB
{
    if(s8Gain > BACH_DPGA_GAIN_MAX_DB)
        s8Gain = BACH_DPGA_GAIN_MAX_DB;
    else if(s8Gain < BACH_DPGA_GAIN_MIN_DB)
        s8Gain = BACH_DPGA_GAIN_MIN_DB;

    if(s8Gain == BACH_DPGA_GAIN_MIN_DB)
        *pu8GainIdx = BACH_DPGA_GAIN_MIN_IDX;
    else
        *pu8GainIdx = (U8)(-2 * s8Gain); //index = -2 * (gain) ,because step = -0.5dB
}

S8 BachDpgaGetGain(BachDpga_e eDpga)
{
    U16 nConfigValue;
    U8	nGainIdx, nAddr;
    S8	nGain;

    switch(eDpga)
    {
    case BACH_DPGA_MMP1:
        nAddr = BACH_MMP1_DPGA_CFG2;
        break;
    case BACH_DPGA_MMP2:
        nAddr = BACH_MMP2_DPGA_CFG2;
        break;
    case BACH_DPGA_CODEC1:
        nAddr = BACH_CODEC1_DPGA_CFG2;
        break;
    case BACH_DPGA_CODEC2:
        nAddr = BACH_CODEC2_DPGA_CFG2;
        break;
    case BACH_DPGA_DBB_1:
        nAddr = BACH_DBB_1_DPGA_CFG2;
        break;
    case BACH_DPGA_DEC1_1:
        nAddr = BACH_DEC1_1_DPGA_CFG2;
        break;
    case BACH_DPGA_DMARD_L:
        nAddr = BACH_DMARD_L_DPGA_CFG2;
        break;
    case BACH_DPGA_DEC1_2:
        nAddr = BACH_DEC1_2_DPGA_CFG2;
        break;
    case BACH_DPGA_DMARD_R:
        nAddr = BACH_DMARD_R_DPGA_CFG2;
        break;
    case BACH_DPGA_ASRC2MMP1:
        nAddr = BACH_ASRC2_MMP1_DPGA_CFG2;
        break;
    case BACH_DPGA_ASRC2MMP2:
        nAddr = BACH_ASRC2_MMP2_DPGA_CFG2;
        break;
    case BACH_DPGA_SPDIF1:
        nAddr = BACH_SPDIF1_DPGA_CFG2;
        break;
    case BACH_DPGA_SPDIF2:
        nAddr = BACH_SPDIF2_DPGA_CFG2;
        break;
    case BACH_DPGA_DMAWR3:
        nAddr = BACH_DMAWR3_DPGA_CFG2;
        break;
    case BACH_DPGA_DMAWR4:
        nAddr = BACH_DMAWR4_DPGA_CFG2;
        break;
    case BACH_DPGA_DEC1_3:
        nAddr = BACH_DEC1_3_DPGA_CFG2;
        break;
    case BACH_DPGA_DBB_2:
        nAddr = BACH_DBB_2_DPGA_CFG2;
        break;
    case BACH_DPGA_DMAWR1:
        nAddr = BACH_DMAWR1_DPGA_CFG2;
        break;
    case BACH_DPGA_DMAWR2:
        nAddr = BACH_DMAWR2_DPGA_CFG2;
        break;
    default:
        ERRMSG("BachDpgaGetGain - ERROR default case!\n");
        return 0;
    }

    nConfigValue = BachReadReg(BACH_REG_BANK4, nAddr);
    nGainIdx = (U8)(nConfigValue & REG_GAIN_MSK);
    if(nGainIdx == BACH_DPGA_GAIN_MIN_IDX)
        nGain = BACH_DPGA_GAIN_MIN_DB;
    else
        nGain = -((S8)nGainIdx) / 2;

    return nGain;
}



void BachDpgaSetGain(BachDpga_e eDpga, S8 s8Gain)
{
    U8 nAddr;
    U8 nGainIdx;

    BachDpgaCalGain(s8Gain, &nGainIdx);
    //ERRMSG(1,(TEXT("GainIdx %d\r\n"),nGainIdx));
    switch(eDpga)
    {
    case BACH_DPGA_MMP1:
        nAddr = BACH_MMP1_DPGA_CFG2;
        break;
    case BACH_DPGA_MMP2:
        nAddr = BACH_MMP2_DPGA_CFG2;
        break;
    case BACH_DPGA_CODEC1:
        nAddr = BACH_CODEC1_DPGA_CFG2;
        break;
    case BACH_DPGA_CODEC2:
        nAddr = BACH_CODEC2_DPGA_CFG2;
        break;
    case BACH_DPGA_DBB_1:
        nAddr = BACH_DBB_1_DPGA_CFG2;
        break;
    case BACH_DPGA_DEC1_1:
        nAddr = BACH_DEC1_1_DPGA_CFG2;
        break;
    case BACH_DPGA_DMARD_L:
        nAddr = BACH_DMARD_L_DPGA_CFG2;
        break;
    case BACH_DPGA_DEC1_2:
        nAddr = BACH_DEC1_2_DPGA_CFG2;
        break;
    case BACH_DPGA_DMARD_R:
        nAddr = BACH_DMARD_R_DPGA_CFG2;
        break;
    case BACH_DPGA_ASRC2MMP1:
        nAddr = BACH_ASRC2_MMP1_DPGA_CFG2;
        break;
    case BACH_DPGA_ASRC2MMP2:
        nAddr = BACH_ASRC2_MMP2_DPGA_CFG2;
        break;
    case BACH_DPGA_SPDIF1:
        nAddr = BACH_SPDIF1_DPGA_CFG2;
        break;
    case BACH_DPGA_SPDIF2:
        nAddr = BACH_SPDIF2_DPGA_CFG2;
        break;
    case BACH_DPGA_DMAWR3:
        nAddr = BACH_DMAWR3_DPGA_CFG2;
        break;
    case BACH_DPGA_DMAWR4:
        nAddr = BACH_DMAWR4_DPGA_CFG2;
        break;
    case BACH_DPGA_DEC1_3:
        nAddr = BACH_DEC1_3_DPGA_CFG2;
        break;
    case BACH_DPGA_DBB_2:
        nAddr = BACH_DBB_2_DPGA_CFG2;
        break;
    case BACH_DPGA_DMAWR1:
        nAddr = BACH_DMAWR1_DPGA_CFG2;
        break;
    case BACH_DPGA_DMAWR2:
        nAddr = BACH_DMAWR2_DPGA_CFG2;
        break;
    default:
        ERRMSG("BachDpgaSetGain - ERROR default case!\n");
        return;
    }

    //set gain
    BachWriteReg(BACH_REG_BANK4, nAddr, REG_GAIN_MSK, nGainIdx);

    //trigger
    BachDpgaTrigger(nAddr, REG_GAIN_TRIG);
}

/*
BOOL HardwareContext::MapRegisters()
{

    // IIS registers.
    //
    g_pIISregs = (volatile S3C2410X_IISBUS_REG*)VirtualAlloc(0, sizeof(S3C2410X_IISBUS_REG), MEM_RESERVE, PAGE_NOACCESS);
    if (!g_pIISregs)
    {
        DEBUGMSG(1, (TEXT("IISreg: VirtualAlloc failed!\r\n")));
        return(FALSE);
    }
    if (!VirtualCopy((PVOID)g_pIISregs, (PVOID)(S3C2410X_BASE_REG_PA_IISBUS >> 8), sizeof(S3C2410X_IISBUS_REG), PAGE_PHYSICAL | PAGE_READWRITE | PAGE_NOCACHE))
    {
        DEBUGMSG(1, (TEXT("IISreg: VirtualCopy failed!\r\n")));
        return(FALSE);
    }

    // SPI registers.
    //
    g_pSPIregs = (volatile S3C2410X_SPI_REG*)VirtualAlloc(0, sizeof(S3C2410X_SPI_REG), MEM_RESERVE, PAGE_NOACCESS);
    if (!g_pSPIregs)
    {
        DEBUGMSG(1, (TEXT("SPIreg: VirtualAlloc failed!\r\n")));
        return(FALSE);
    }
    if (!VirtualCopy((PVOID)g_pSPIregs, (PVOID)(S3C2410X_BASE_REG_PA_SPI >> 8), sizeof(S3C2410X_SPI_REG), PAGE_PHYSICAL | PAGE_READWRITE | PAGE_NOCACHE))
    {
        DEBUGMSG(1, (TEXT("SPIreg: VirtualCopy failed!\r\n")));
        return(FALSE);
    }

    g_pIOPregs = (volatile S3C2410X_IOPORT_REG*)VirtualAlloc(0, sizeof(S3C2410X_IOPORT_REG), MEM_RESERVE, PAGE_NOACCESS);
    if (!g_pIOPregs)
    {
        DEBUGMSG(1, (TEXT("IOPreg: VirtualAlloc failed!\r\n")));
        return(FALSE);
    }
    if (!VirtualCopy((PVOID)g_pIOPregs, (PVOID)(S3C2410X_BASE_REG_PA_IOPORT >> 8), sizeof(S3C2410X_IOPORT_REG), PAGE_PHYSICAL | PAGE_READWRITE | PAGE_NOCACHE))
    {
        DEBUGMSG(1, (TEXT("IOPreg: VirtualCopy failed!\r\n")));
        return(FALSE);
    }

    g_pDMAregs = (volatile S3C2410X_DMA_REG*)VirtualAlloc(0, sizeof(S3C2410X_DMA_REG), MEM_RESERVE, PAGE_NOACCESS);
    if (!g_pDMAregs)
    {
        DEBUGMSG(1, (TEXT("DMAreg: VirtualAlloc failed!\r\n")));
        return(FALSE);
    }
    if (!VirtualCopy((PVOID)g_pDMAregs, (PVOID)(S3C2410X_BASE_REG_PA_DMA >> 8), sizeof(S3C2410X_DMA_REG), PAGE_PHYSICAL | PAGE_READWRITE | PAGE_NOCACHE))
    {
        DEBUGMSG(1, (TEXT("DMAreg: VirtualCopy failed!\r\n")));
        return(FALSE);
    }

    s2410INT = (volatile S3C2410X_INTR_REG*)VirtualAlloc(0, sizeof(S3C2410X_INTR_REG), MEM_RESERVE, PAGE_NOACCESS);
    if (!g_pDMAregs)
    {
        DEBUGMSG(1, (TEXT("INTreg: VirtualAlloc failed!\r\n")));
        return(FALSE);
    }
    if (!VirtualCopy((PVOID)s2410INT, (PVOID)(S3C2410X_BASE_REG_PA_INTR >> 8), sizeof(S3C2410X_INTR_REG), PAGE_PHYSICAL | PAGE_READWRITE | PAGE_NOCACHE))
    {
        DEBUGMSG(1, (TEXT("INTreg: VirtualCopy failed!\r\n")));
        return(FALSE);
    }

    g_pCLKPWRreg = (volatile S3C2410X_CLKPWR_REG*)VirtualAlloc(0, sizeof(S3C2410X_CLKPWR_REG), MEM_RESERVE, PAGE_NOACCESS);
    if (!g_pCLKPWRreg)
    {
        DEBUGMSG(1, (TEXT("DMAreg: VirtualAlloc failed!\r\n")));
        return(FALSE);
    }
    if (!VirtualCopy((PVOID)g_pCLKPWRreg, (PVOID)(S3C2410X_BASE_REG_PA_CLOCK_POWER >> 8), sizeof(S3C2410X_CLKPWR_REG), PAGE_PHYSICAL | PAGE_READWRITE | PAGE_NOCACHE))
    {
        DEBUGMSG(1, (TEXT("DMAreg: VirtualCopy failed!\r\n")));
        return(FALSE);
    }

    return(TRUE);
}
*/

void BachSetWakeUpMode(BOOL bWakeUp)
{
    m_bWakeUp = bWakeUp;
}

void BachSysInit(void)
{
    U16 nValue;
    //PAD MUX
    //BachWriteReg2Byte(0x00101a00, 0x0800);
    //BachWriteReg2Byte(0x00101a20, 0x0008);
    //BachWriteReg2Byte(0x00101a04, 0x4000);
    //Bach_CASE1_MM_GPS_RECORD
    m_ChipRevision = ReadChipRevision();
    m_BoardRevision = BOARDNAME();

#if defined(__linux__)
    TRACE("BachSysInit - ChipRevision = %d\n", m_ChipRevision);
    TRACE("BachSysInit - BoardRevision = %d\n", m_BoardRevision);
#else
    RETAILMSG(1,(TEXT("[Audio] BachSysInit - ChipRevision = %d \r\n"),m_ChipRevision));
    RETAILMSG(1,(TEXT("[Audio] BachSysInit - BoardRevision = %d \r\n"),m_BoardRevision));
    //ERRMSG1("[Audio] BachSysInit - ChipRevision = %d\n", m_ChipRevision);
#endif



    BachAtopInit();

    if(m_ChipRevision == REVISION_U01)
    {
        BachAtopIdle();
    }
    else
    {
        BachAtopEnDigClk();
    }

    //MUTE INPUT SOURCE
    BachWriteReg2Byte(0x00112d40, 0x0005);
    BachWriteReg2Byte(0x00112d44, 0x0005);
    BachWriteReg2Byte(0x00112d64, 0x0005);
    BachWriteReg2Byte(0x00112d66, 0x0005);

    //AU, DPGA, SDM RST
    BachWriteReg2Byte(0x00112a00, 0xe000);
    BachWriteReg2Byte(0x00112a00, 0x0000);

    //INIT AudioBand
    //BachWriteReg2Byte(0x00112a08, 0x0003);
    BachWriteReg2Byte(0x00112a08, 0x0002);	//sel NF_SYNTH_1 as Ch1/Ch2 INT NF synthesizer source
    //sel NF_SYNTH_2 as DEC NF synthesizer source

    BachWriteReg2Byte(0x00112a22, 0x000f);	//enable ch1,2,3,4 IIR
    BachWriteReg2Byte(0x00112a18, 0x0103);	//enable FS_SYNTH as 128fs
    BachWriteReg2Byte(0x00112a02, 0x0003);
    BachWriteReg2Byte(0x00112a06, 0x033f);
    BachWriteReg2Byte(0x00112aa0, 0x0f3e);
    BachWriteReg2Byte(0x00112b00, 0xfaaf);
    BachWriteReg2Byte(0x00112b02, 0x05a0);
    BachWriteReg2Byte(0x00112a0c, 0x0015);
    BachWriteReg2Byte(0x00112a10, 0x007d);
    BachWriteReg2Byte(0x00112a14, 0x0080);
    BachWriteReg2Byte(0x00112b04, 0x0080);
    BachWriteReg2Byte(0x00112aa4, 0x007d);
    BachWriteReg2Byte(0x00112aa8, 0x0080);
    BachWriteReg2Byte(0x00112b08, 0x0080);
    if(m_ChipRevision == REVISION_U01)
        BachWriteReg2Byte(0x00112b0c, 0x0465);
    else
        BachWriteReg2Byte(0x00112b0c, 0x04E2);
    BachWriteReg2Byte(0x00112a10, 0x807d);
    BachWriteReg2Byte(0x00112a14, 0x8080);
    BachWriteReg2Byte(0x00112b04, 0x8080);
    BachWriteReg2Byte(0x00112aa4, 0x807d);
    BachWriteReg2Byte(0x00112aa8, 0x8080);
    BachWriteReg2Byte(0x00112b08, 0x8080);
    if(m_ChipRevision == REVISION_U01)
        BachWriteReg2Byte(0x00112b0c, 0x8465);
    else
        BachWriteReg2Byte(0x00112b0c, 0x84E2);
    BachWriteReg2Byte(0x00112d40, 0x0000);
    BachWriteReg2Byte(0x00112d44, 0x0000);
    BachWriteReg2Byte(0x00112d64, 0x0000);
    BachWriteReg2Byte(0x00112d66, 0x0000);
/////////////////////////////////////////////
#if 0
    BachWriteReg2Byte(0x00112c00, 0x8040);
    BachWriteReg2Byte(0x00112c06, 0x4004);
    BachWriteReg2Byte(0x00112c08, 0x2328);
    BachWriteReg2Byte(0x00112c0a, 0x280c);
    BachWriteReg2Byte(0x00112c0c, 0x8407);
    BachWriteReg2Byte(0x00112c0e, 0x3802);
    BachWriteReg2Byte(0x00112c10, 0x00f8);
    BachWriteReg2Byte(0x00112c12, 0x0000);
    BachWriteReg2Byte(0x00112c14, 0xc0c0);
    BachWriteReg2Byte(0x00112c16, 0x9080);
    BachWriteReg2Byte(0x00112c18, 0x0000);
    BachWriteReg2Byte(0x00112c1a, 0x6000);
#endif
////////////////////////////////////////////
    BachWriteReg2Byte(0x00112a06, 0x023f);
    BachWriteReg2Byte(0x00112a28, 0x253c);
    BachWriteReg2Byte(0x00112a30, 0x0096);
    //BachWriteReg2Byte(0x00112a32, 0x2020);
    BachWriteReg2Byte(0x00112a32, 0x2000);
    BachWriteReg2Byte(0x00112aa2, 0xea82);
    //BachWriteReg2Byte(0x00112aa2, 0xfe82); //dma2
    BachWriteReg2Byte(0x00112aea, 0);//0xe006);
    BachWriteReg2Byte(0x00112aec, 0);//0xe004);
    BachWriteReg2Byte(0x00112b02, 0x05a0);
    BachWriteReg2Byte(0x00112b24, 0x3180);
    BachWriteReg2Byte(0x00112b26, 0x3184);
    BachWriteReg2Byte(0x00112b28, 0x2180);
    BachWriteReg2Byte(0x00112b2c, 0x0e01);

    BachWriteReg(BACH_REG_BANK2,BACH_SPDIF_TX_CTRL2,REG_SPDIF_EN_ABS,REG_SPDIF_EN_ABS); // spdif Tx
    //=================================================
    //for bring up board
    //=================================================
    BachWriteReg2Byte(0x00112b2a, 0x0040);
//	BachWriteReg2Byte(0x00101a00, 0x0804);
//	BachWriteReg2Byte(0x00112c16, 0x9000);

    //test Pop noise
    /*BachWriteReg2Byte(0x00112a06, 0x0227);
    nValue = BachReadReg2Byte(0x102B30);
    nValue |= (0x1<<8);
    nValue &= ~(0x1<<9);
    BachWriteReg2Byte(0x102B30, nValue);*/

    //=================================================
    //Pad MUX
    //=================================================
    nValue = BachReadReg2Byte(0x101a00);
    nValue &= ~(REG_BTI2S_MODE_MSK | REG_EXTI2S_MODE_MSK);
    nValue |= (((0x1 << REG_BTI2S_MODE_POS) & REG_BTI2S_MODE_MSK) |
               ((0x1 << REG_EXTI2S_MODE_POS) & REG_EXTI2S_MODE_MSK));
    BachWriteReg2Byte(0x101a00, nValue);

    nValue = BachReadReg2Byte(0x101a04);
    nValue |= REG_SPDIFOUT_MODE;
    BachWriteReg2Byte(0x101a04, nValue);

    //temporarily mask for GPS
    if (m_BoardRevision != E_BD_MST154A_D01A_S)
    {
        nValue = BachReadReg2Byte(0x101a20);
        nValue &= ~REG_DBBI2S_MODE_MSK;
        nValue |= ((0x2 << REG_DBBI2S_MODE_POS) & REG_DBBI2S_MODE_MSK);
        BachWriteReg2Byte(0x101a20, nValue);

        BachI2sPadMux(BACH_I2S_PAD_CODEC,BACH_I2S_OUTPAD_CODEC,TRUE);
        BachI2sPadMux(BACH_I2S_PAD_BT,BACH_I2S_OUTPAD_BT,TRUE);
    }


    /*    for(i=0;i<BACH_FUNC_NUM;i++)
            m_bBachFuncStatus[i]=FALSE;*/

    //=================================================
    //Clock
    //=================================================
    if(m_ChipRevision > REVISION_U01)
    {
        nValue = BachReadReg2Byte(0x100b02);
        nValue &= ~REG_CLK_432_AU_MSK;
        nValue |= (0x3 << REG_CLK_432_AU_POS) & REG_CLK_432_AU_MSK;
        BachWriteReg2Byte(0x100b02, nValue);
    }

    if(m_ChipRevision == REVISION_U01)
        memcpy(g_nAsrcRateMapTbl, g_nAsrcRateMapTbl1, sizeof(g_nAsrcRateMapTbl1));
    else
        memcpy(g_nAsrcRateMapTbl, g_nAsrcRateMapTbl2, sizeof(g_nAsrcRateMapTbl2));


}

void BachAtopEnDigClk(void)
{
    BachWriteReg2Byte(0x112C00, 0x8040);
    BachWriteReg2Byte(0x112C04, 0x0780);
    BachWriteReg2Byte(0x112C06, 0x4CA4);
    BachWriteReg2Byte(0x112C08, 0x2000);
    BachWriteReg2Byte(0x112C0A, 0x0000);
    BachWriteReg2Byte(0x112C0C, 0x0004);
    BachWriteReg2Byte(0x112C0E, 0x0000);
    BachWriteReg2Byte(0x112C10, 0x0000);
    BachWriteReg2Byte(0x112C12, 0x0000);
    BachWriteReg2Byte(0x112C14, 0x0000);
    BachWriteReg2Byte(0x112C16, 0x0080);
    BachWriteReg2Byte(0x112C18, 0x0000);
    BachWriteReg2Byte(0x112C1A, 0x6000);
    m_bAnalogIdle = TRUE;
}


void BachAtopInit(void)
{
    int i;
    //1. Enable micbias and do triming
    //2. Enable Reference and OPLP for Earphone depop
    BachWriteReg2Byte(0x112C00, 0x8040);
    BachWriteReg2Byte(0x112C04, 0x0780);
    BachWriteReg2Byte(0x112C06, 0x0C24);
    BachWriteReg2Byte(0x112C08, 0x2000);
    BachWriteReg2Byte(0x112C0A, 0x0000);
    BachWriteReg2Byte(0x112C0C, 0x0000);
    BachWriteReg2Byte(0x112C0E, 0x0000);
    //BachWriteReg2Byte(0x112C10, 0x0010);
    BachWriteReg2Byte(0x112C10, 0x0000);
    BachWriteReg2Byte(0x112C12, 0x0000);
    BachWriteReg2Byte(0x112C14, 0x0000);
    BachWriteReg2Byte(0x112C16, 0x0000);
    BachWriteReg2Byte(0x112C18, 0x0000);
    BachWriteReg2Byte(0x112C1A, 0x4000);
    SLEEP(1);
    BachWriteReg2Byte(0x112C1A, 0x6000);

    //Sleep(500);
    //SimpleTimerLoopDelay(500000);
    while(!BachAtopIsTrimOk());

    //status init
    m_bAnalogIdle = FALSE;
    m_bADCActive = FALSE;
    m_bDACActive = FALSE;

    for(i = 0; i < BACH_ATOP_NUM; i++)
        m_bBachAtopStatus[i] = FALSE;

}


void BachAtopIdle(void)
{
    //1. Enable OPLP for Earphone depop
    //2. MICBIAS polling, Ref polling
    //3. Enable EAR_DET and LP detect
    //4. others are OFF for save power

    //BachWriteReg2Byte(0x112C06, 0x0CA4);
    BachWriteReg2Byte(0x112C06, 0x0000);
    BachWriteReg2Byte(0x112C08, 0x1000);
    BachWriteReg2Byte(0x112C0A, 0x0000);
    BachWriteReg2Byte(0x112C0C, 0x0000);
    BachWriteReg2Byte(0x112C0E, 0x0000);
    //BachWriteReg2Byte(0x112C10, 0x0010);
    BachWriteReg2Byte(0x112C10, 0x0000);
    BachWriteReg2Byte(0x112C12, 0x0000);
    BachWriteReg2Byte(0x112C14, 0x0000);
    BachWriteReg2Byte(0x112C16, 0x0000);
    BachWriteReg2Byte(0x112C18, 0x0000);
    m_bAnalogIdle = TRUE;

}

void BachAtopEnableRef(BOOL bEnable)
{
    //1. Enable OPLP for Earphone depop
    //2. MICBIAS polling, Ref polling
    //3. Enable EAR_DET and LP detect
    //4. others are OFF for save power

    U16 nValue;

    if (bEnable)
    {
        nValue = BachReadReg2Byte(0x112C06);
        nValue |= (RCV_EN);	//Enable Handset(RCV) micbias output
        nValue |= (EN_MICBIAS);	//Enable micbias
        BachWriteReg2Byte(0x112C06, nValue);

        udelay(20);

        nValue = BachReadReg2Byte(0x112C06);
        nValue |= (EN_REF_NO_BG);	//Enable REF_NO_BG block
        BachWriteReg2Byte(0x112C06, nValue);

        udelay(20);

        nValue = BachReadReg2Byte(0x112C06);
        nValue |= (EN_POLLING_DRV);	//Enable driver polling, 0:disable,1:enable
        BachWriteReg2Byte(0x112C06, nValue);

        m_bAnalogIdle = FALSE;

    }
    else
    {
        nValue = BachReadReg2Byte(0x112C06);
        nValue &= ~(EN_POLLING_DRV);	//Enable driver polling, 0:disable,1:enable
        BachWriteReg2Byte(0x112C06, nValue);

        udelay(20);

        nValue = BachReadReg2Byte(0x112C06);
        nValue &= ~(RCV_EN);
        nValue &= ~(EN_MICBIAS);
        nValue &= ~(EN_REF_NO_BG);
        BachWriteReg2Byte(0x112C06, nValue);

        m_bAnalogIdle = TRUE;
    }

}

void BachAtopADCOn(BOOL bEnable)
{
    U16 nValue;
	U16 nIndex;

    if (bEnable)
    {
        nValue = BachReadReg2Byte(0x112C06);
        nValue |= (EN_AUDIO_IBIAS | EN_POLLING_DRV | EN_REF_NO_BG |
                   EN_MICDET | RCV_EN | EN_MICBIAS);//0x4CA4;
        BachWriteReg2Byte(0x112C06, nValue);

        nValue = BachReadReg2Byte(0x112C08);
        nValue &= ~PGA0_MICA2_GAIN_MSK;
        //nValue &= ~EN_MICTRIM;
        nValue |= (EN_MICDET_LP | ((0x0 << PGA0_MICA2_GAIN_POS) &PGA0_MICA2_GAIN_MSK));//0x1000;
        BachWriteReg2Byte(0x112C08, nValue);

        nValue = BachReadReg2Byte(0x112C0A);
        nValue &= ~PGA0_MICA4_GAIN_MSK;
        nValue |= ((0x0 << PGA0_MICA4_GAIN_POS) &PGA0_MICA4_GAIN_MSK);
        BachWriteReg2Byte(0x112C0A, nValue);

        nValue = BachReadReg2Byte(0x112C0C);
        nValue &= ~(PGA1_MICA2_GAIN_MSK | PGA1_MICA4_GAIN_MSK);
        nValue |= (EN_DAC0_LDO11 | ((0x0 << PGA1_MICA2_GAIN_POS) &PGA1_MICA2_GAIN_MSK) |
                   ((0x0 << PGA1_MICA4_GAIN_POS) &PGA1_MICA4_GAIN_MSK));//0x0004;
        BachWriteReg2Byte(0x112C0C, nValue);

        nValue = BachReadReg2Byte(0x112C0E);
        nValue |= EN_ADC0;//0x0002;
        BachWriteReg2Byte(0x112C0E, nValue);

        nValue = BachReadReg2Byte(0x112C10);
        nValue |= (EN_ADC1 | EN_OPLP);//0x0090;
        BachWriteReg2Byte(0x112C10, nValue);

        nValue = BachReadReg2Byte(0x112C16);
        nValue |= (EN_LDO25 | EN_CLK);//0x9080;
        BachWriteReg2Byte(0x112C16, nValue);

        nValue = BachReadReg2Byte(0x112C10);
        nValue |= (RESET_ADC1_R | RESET_ADC1_L);//0x0090;
        BachWriteReg2Byte(0x112C10, nValue);

        nValue = BachReadReg2Byte(0x112C0E);
        nValue |= (RESET_ADC0_R | RESET_ADC0_L);//0x0090;
        BachWriteReg2Byte(0x112C0E, nValue);

        udelay(20);

        nValue = BachReadReg2Byte(0x112C10);
        nValue &= ~(RESET_ADC1_R | RESET_ADC1_L);//0x0090;
        BachWriteReg2Byte(0x112C10, nValue);

        nValue = BachReadReg2Byte(0x112C0E);
        nValue &= ~(RESET_ADC0_R | RESET_ADC0_L);//0x0090;
        BachWriteReg2Byte(0x112C0E, nValue);

        m_bADCActive = TRUE;

    }
    else
    {
        nValue = BachReadReg2Byte(0x112C0E);
        nValue &= ~EN_ADC0;
        BachWriteReg2Byte(0x112C0E, nValue);

        nValue = BachReadReg2Byte(0x112C10);
        nValue &= ~EN_ADC1;
        BachWriteReg2Byte(0x112C10, nValue);
        m_bADCActive = FALSE;
    }

    for(nIndex=0;nIndex<4; nIndex++)
    {
        if(BachGetMux4(g_tAdcMuxTbl[nIndex])<3)
        {
            if(m_bADCActive)
                BachAdcMuxGuard(g_tAdcMuxTbl[nIndex],FALSE);
            else
                BachAdcMuxGuard(g_tAdcMuxTbl[nIndex],TRUE);
        }
    }
}

void BachAtopHandsetMic(BOOL bEnable)
{
    U16 nValue;

    if (bEnable)
    {
        nValue = BachReadReg2Byte(0x112C06);
        nValue |= (EN_AUDIO_IBIAS | EN_POLLING_DRV | EN_REF_NO_BG |
                   EN_MICDET | RCV_EN | EN_MICBIAS);//0x4CA4;
        BachWriteReg2Byte(0x112C06, nValue);

        ////////////////////////////////////
        nValue = BachReadReg2Byte(0x112C08);	//Enable PGA0 micamp1 and Enable PGA0 micamp2
        nValue &= ~(PGA0_MICA2_GAIN_MSK | PGA0_MICA1_GAIN);
        nValue |= (EN_MICDET_LP | EN_VCMBUF0 | EN_IBIAS_PGA0 |
                   EN_PGA0_MICA1 | EN_PGA0_MICA2 | ((m_nMicGain << PGA0_MICA2_GAIN_POS) &PGA0_MICA2_GAIN_MSK)) | (m_nMicPreGain ? PGA0_MICA1_GAIN : 0);//0x132B;
        BachWriteReg2Byte(0x112C08, nValue);
        ///////////////////////////////////
        nValue = BachReadReg2Byte(0x112C0A);
        nValue &= ~(PGA0_MICA2_SEL | PGA0_MICA4_SEL);//0x0000;
        BachWriteReg2Byte(0x112C0A, nValue);

        nValue = BachReadReg2Byte(0x112C0C);
        nValue |= EN_DAC0_LDO11;//0x0004;
        BachWriteReg2Byte(0x112C0C, nValue);

        nValue = BachReadReg2Byte(0x112C0E);
        nValue |= EN_ADC0;//0x0002;
        BachWriteReg2Byte(0x112C0E, nValue);

        nValue = BachReadReg2Byte(0x112C10);
        nValue |= EN_OPLP;//0x0010;
        BachWriteReg2Byte(0x112C10, nValue);

        nValue = BachReadReg2Byte(0x112C16);
        nValue |= (EN_LDO25 | EN_CLK);//0x9080;
        BachWriteReg2Byte(0x112C16, nValue);
        m_bBachAtopStatus[BACH_ATOP_HANDSET] = TRUE;
    }
    else
    {

        ////////////////////////////////////
        nValue = BachReadReg2Byte(0x112C08);
        nValue &= ~(EN_PGA0_MICA1 | EN_PGA0_MICA2);

        if(!m_bBachAtopStatus[BACH_ATOP_AUXMIC])
            nValue &= ~(EN_VCMBUF0 | EN_IBIAS_PGA0);


        BachWriteReg2Byte(0x112C08, nValue);
        ///////////////////////////////////
        m_bBachAtopStatus[BACH_ATOP_HANDSET] = FALSE;
    }
}

void BachAtopAuxMic(BOOL bEnable)
{
    U16 nValue;

    if (bEnable)
    {
        nValue = BachReadReg2Byte(0x112C06);
        nValue |= (EN_AUDIO_IBIAS | EN_POLLING_DRV | EN_REF_NO_BG |
                   EN_MICDET | RCV_EN | EN_MICBIAS);//0x4CA4;
        BachWriteReg2Byte(0x112C06, nValue);

        /////////////////////////////////////
        nValue = BachReadReg2Byte(0x112C08);
        nValue |= (EN_MICDET_LP | EN_VCMBUF0 | EN_IBIAS_PGA0 );//0x1300;
        BachWriteReg2Byte(0x112C08, nValue);

        nValue = BachReadReg2Byte(0x112C0A);	//Enable PGA0 micamp3 and Enable PGA0 micamp4
        nValue &= ~(PGA0_MICA4_GAIN_MSK | PGA0_MICA3_GAIN);
        nValue |= (EN_PGA0_MICA3 | EN_PGA0_MICA4 | ((m_nMicGain << PGA0_MICA4_GAIN_POS) & PGA0_MICA4_GAIN_MSK)) | (m_nMicPreGain ? PGA0_MICA3_GAIN : 0);//0x2B00;
        nValue &= ~(PGA0_MICA2_SEL | PGA0_MICA4_SEL);
        BachWriteReg2Byte(0x112C0A, nValue);
        /////////////////////////////////////

        nValue = BachReadReg2Byte(0x112C0C);
        nValue |= EN_DAC0_LDO11;//0x0004;
        BachWriteReg2Byte(0x112C0C, nValue);

        nValue = BachReadReg2Byte(0x112C0E);
        nValue |= EN_ADC0;//0x0002;
        BachWriteReg2Byte(0x112C0E, nValue);

        nValue = BachReadReg2Byte(0x112C10);
        nValue |= EN_OPLP;//0x0010;
        BachWriteReg2Byte(0x112C10, nValue);

        nValue = BachReadReg2Byte(0x112C16);
        nValue |= (EN_LDO25 | EN_CLK);//0x9080;
        BachWriteReg2Byte(0x112C16, nValue);
        m_bBachAtopStatus[BACH_ATOP_AUXMIC] = TRUE;
    }
    else
    {
        //////////////////////////////////
        if(!m_bBachAtopStatus[BACH_ATOP_HANDSET])
        {
            nValue = BachReadReg2Byte(0x112C08);
            nValue &= ~(EN_VCMBUF0 | EN_IBIAS_PGA0);
            BachWriteReg2Byte(0x112C08, nValue);
        }

        nValue = BachReadReg2Byte(0x112C0A);
        nValue &= ~(EN_PGA0_MICA3 | EN_PGA0_MICA4);
        BachWriteReg2Byte(0x112C0A, nValue);
        /////////////////////////////////
        m_bBachAtopStatus[BACH_ATOP_AUXMIC] = FALSE;
    }

}

void BachAtopLineIn0(BOOL bEnable)
{
    U16 nValue;

    if (bEnable)
    {
        nValue = BachReadReg2Byte(0x112C06);
        nValue |= (EN_AUDIO_IBIAS | EN_POLLING_DRV | EN_REF_NO_BG |
                   EN_MICDET | RCV_EN | EN_MICBIAS);//0x4CA4;
        BachWriteReg2Byte(0x112C06, nValue);

        /////////////////////////////////////
        nValue = BachReadReg2Byte(0x112C08);
        nValue |= (EN_MICDET_LP | EN_VCMBUF0 |
                   EN_IBIAS_PGA0 | EN_PGA0_MICA2);//0x1308;
        nValue &= ~(PGA0_MICA2_GAIN_MSK);
        nValue |= m_nLineInGain[0]<<PGA0_MICA2_GAIN_POS;
        BachWriteReg2Byte(0x112C08, nValue);

        nValue = BachReadReg2Byte(0x112C0A);
        nValue |= (PGA0_MICA2_SEL | EN_PGA0_MICA4 | PGA0_MICA4_SEL);//0x8880;
        nValue &= ~(PGA0_LINE_SEL | PGA0_MICA4_GAIN_MSK);
        nValue |= m_nLineInGain[0]<<PGA0_MICA4_GAIN_POS;
        nValue |= (m_nLineInMux[0] ? PGA0_LINE_SEL : 0);
        nValue &= ~(PGA0_MUTE_L | PGA0_MUTE_R);
        nValue |= (m_nLineInMute[0] ? (PGA0_MUTE_L|PGA0_MUTE_R) : 0);
        BachWriteReg2Byte(0x112C0A, nValue);
        /////////////////////////////////////

        nValue = BachReadReg2Byte(0x112C0C);
        nValue |= EN_DAC0_LDO11;//0x0004;
        BachWriteReg2Byte(0x112C0C, nValue);

        nValue = BachReadReg2Byte(0x112C0E);
        nValue |= EN_ADC0;//0x0002;
        BachWriteReg2Byte(0x112C0E, nValue);

        nValue = BachReadReg2Byte(0x112C10);
        nValue |= EN_OPLP;//0x0010;
        BachWriteReg2Byte(0x112C10, nValue);

        nValue = BachReadReg2Byte(0x112C16);
        nValue |= (EN_LDO25 | EN_CLK);//0x9080;
        BachWriteReg2Byte(0x112C16, nValue);
        m_bBachAtopStatus[BACH_ATOP_LINEIN0] = TRUE;
    }
    else
    {
        nValue = BachReadReg2Byte(0x112C08);
        nValue &= ~(EN_VCMBUF0 | EN_IBIAS_PGA0 | EN_PGA0_MICA2);
        BachWriteReg2Byte(0x112C08, nValue);

        nValue = BachReadReg2Byte(0x112C0A);
        nValue &= ~(EN_PGA0_MICA4);
        nValue &= ~(PGA0_MUTE_L | PGA0_MUTE_R);
        BachWriteReg2Byte(0x112C0A, nValue);
        m_bBachAtopStatus[BACH_ATOP_LINEIN0] = FALSE;
    }
}


void BachAtopLineIn1(BOOL bEnable)
{
    U16 nValue;

    if (bEnable)
    {
        nValue = BachReadReg2Byte(0x112C06);
        nValue |= (EN_AUDIO_IBIAS | EN_POLLING_DRV | EN_REF_NO_BG |
                   EN_MICDET | RCV_EN | EN_MICBIAS);//0x4CA4;
        BachWriteReg2Byte(0x112C06, nValue);

        nValue = BachReadReg2Byte(0x112C08);
        nValue |= EN_MICDET_LP;//0x1000;
        BachWriteReg2Byte(0x112C08, nValue);

        /////////////////////////////////////////
        nValue = BachReadReg2Byte(0x112C0A);
        nValue |= (EN_VCMBUF1 | EN_IBIAS_PGA1);//0x000C;
        BachWriteReg2Byte(0x112C0A, nValue);

        nValue = BachReadReg2Byte(0x112C0C);
        nValue |= (EN_PGA1_MICA2 | EN_PGA1_MICA4 | EN_DAC0_LDO11);//0x8404;
        nValue &= ~(PGA1_LINE_SEL | PGA1_MICA2_GAIN_MSK | PGA1_MICA4_GAIN_MSK);
        nValue |= (m_nLineInMux[1] ? PGA1_LINE_SEL : 0);
        nValue |= (m_nLineInGain[1]<<PGA1_MICA2_GAIN_POS | m_nLineInGain[1]<<PGA1_MICA4_GAIN_POS);

        nValue &= ~(PGA1_MUTE_L | PGA1_MUTE_R);
        nValue |= (m_nLineInMute[1] ? (PGA1_MUTE_L|PGA1_MUTE_R) : 0);
        BachWriteReg2Byte(0x112C0C, nValue);

        nValue = BachReadReg2Byte(0x112C10);
        nValue |= (EN_ADC1 | EN_OPLP);//0x0090;		//enable ADC1
        BachWriteReg2Byte(0x112C10, nValue);
        //////////////////////////////////////////

        nValue = BachReadReg2Byte(0x112C16);
        nValue |= (EN_LDO25 | EN_CLK);//0x9080;
        BachWriteReg2Byte(0x112C16, nValue);

        m_bBachAtopStatus[BACH_ATOP_LINEIN1] = TRUE;

    }
    else
    {

        nValue = BachReadReg2Byte(0x112C0A);
        nValue &= ~(EN_VCMBUF1 | EN_IBIAS_PGA1);
        BachWriteReg2Byte(0x112C0A, nValue);

        nValue = BachReadReg2Byte(0x112C0C);
        nValue &= ~(EN_PGA1_MICA2 | EN_PGA1_MICA4);
        nValue &= ~(PGA1_MUTE_L | PGA1_MUTE_R);
        BachWriteReg2Byte(0x112C0C, nValue);


        m_bBachAtopStatus[BACH_ATOP_LINEIN1] = FALSE;
    }
}

void BachAtopDACOn(BOOL bEnable)
{
    U16 nValue;

    if (bEnable)
    {
        nValue = BachReadReg2Byte(0x112C06);
        nValue |= (EN_AUDIO_IBIAS | EN_POLLING_DRV | EN_REF_NO_BG |
                   EN_MICDET | RCV_EN | EN_MICBIAS);//0x4CA4;
        BachWriteReg2Byte(0x112C06, nValue);

        nValue = BachReadReg2Byte(0x112C08);
        nValue |= EN_MICDET_LP;//0x1000;
        BachWriteReg2Byte(0x112C08, nValue);

        nValue = BachReadReg2Byte(0x112C0C);
        nValue |= (EN_DAC0_LDO11 | EN_DAC0_L | EN_DAC0_R);//0x0007;			//enable L/R DAC0
        BachWriteReg2Byte(0x112C0C, nValue);

        nValue = BachReadReg2Byte(0x112C0E);
        nValue |= (EN_DAC1_LDO11 | EN_DAC1_L | EN_DAC1_R);//0x3800;			//enable L/R DAC1
        BachWriteReg2Byte(0x112C0E, nValue);

        nValue = BachReadReg2Byte(0x112C12);
        nValue &= ~TCSEL_MSK;
        nValue |= ((0x1 << TCSEL_POS)& TCSEL_MSK);
        BachWriteReg2Byte(0x112C12, nValue);

        nValue = BachReadReg2Byte(0x112C16);
        nValue |= (EN_LDO25 | EN_CLK);//0x9080;
        BachWriteReg2Byte(0x112C16, nValue);

        /*nValue = BachReadReg2Byte(0x102B30);
        U16 bit8 = nValue & (0x1<<8);
        nValue &= ~(0x1<<8);
        nValue |= (bit8^(0x1<<8));
        BachWriteReg2Byte(0x102B30, nValue);

        Sleep(1000);
        nValue = BachReadReg2Byte(0x102B30);
        bit8 = nValue & (0x1<<8);
        nValue &= ~(0x1<<8);
        nValue |= (bit8^(0x1<<8));
        BachWriteReg2Byte(0x102B30, nValue);*/

        nValue = BachReadReg2Byte(0x112C10);
        nValue |= EN_OPLP;//0x0010;
        BachWriteReg2Byte(0x112C10, nValue);

        m_bDACActive = TRUE;
    }
    else
    {
        nValue = BachReadReg2Byte(0x112C0C);
        nValue &= ~(EN_DAC0_LDO11 | EN_DAC0_L | EN_DAC0_R);
        BachWriteReg2Byte(0x112C0C, nValue);

        nValue = BachReadReg2Byte(0x112C0E);
        nValue &= ~(EN_DAC1_LDO11 | EN_DAC1_L | EN_DAC1_R);
        BachWriteReg2Byte(0x112C0E, nValue);
        m_bDACActive = FALSE;

    }
}

void BachAtopEarphone(BOOL bEnable)
{
    U16 nValue;

    if (bEnable)
    {

        ///////////////////////////////////////////////
        nValue = BachReadReg2Byte(0x112C10);
        nValue &= ~GAIN_EAR_MSK;
        nValue |= (EN_OPLP);
        nValue |= ((0x2 << GAIN_EAR_POS)&GAIN_EAR_MSK); // for earphone voice too small.
        BachWriteReg2Byte(0x112C10, nValue);


        //delay 1s
        //udelay(1000000);

        SLEEP(1000);

        /*nValue = BachReadReg2Byte(0x102B30);
        U16 bit8 = nValue & (0x1<<8);
        nValue &= ~(0x1<<8);
        nValue |= (bit8^(0x1<<8));
        BachWriteReg2Byte(0x102B30, nValue);*/
//		RETAILMSG(1,(TEXT("Sleep %d %d %d"),start,end,end-start));

        nValue = BachReadReg2Byte(0x112C10);
        nValue |= (EN_EARL);
        nValue |= (EN_EARR);
        BachWriteReg2Byte(0x112C10, nValue);

        //delay 100ms
        //udelay(100000);
        //Sleep(100);

        SLEEP(100);


        /*nValue = BachReadReg2Byte(0x102B30);
         bit8 = nValue & (0x1<<8);
        nValue &= ~(0x1<<8);
        nValue |= (bit8^(0x1<<8));
        BachWriteReg2Byte(0x102B30, nValue);*/

        nValue = BachReadReg2Byte(0x112C10);
        nValue |= (EN_STG2AB);
        BachWriteReg2Byte(0x112C10, nValue);
        ///////////////////////////////////////////////
        /*nValue = BachReadReg2Byte(0x112C12);
        nValue &= ~(EAR_MUTE);
        BachWriteReg2Byte(0x112C12, nValue);*/

        m_bBachAtopStatus[BACH_ATOP_EAR] = TRUE;
    }
    else
    {
        ///////////////////////////////////////////////
        nValue = BachReadReg2Byte(0x112C10);
        nValue &= ~(EN_EARL);
        nValue &= ~(EN_EARR);
        nValue &= ~(EN_STG2AB);
        BachWriteReg2Byte(0x112C10, nValue);
        ///////////////////////////////////////////////
        m_bBachAtopStatus[BACH_ATOP_EAR] = FALSE;
    }
}


void BachAtopLineOut0(BOOL bEnable)
{
    U16 nValue;

    if (bEnable)
    {
        nValue = BachReadReg2Byte(0x112C06);
        nValue |= (EN_AUDIO_IBIAS | EN_POLLING_DRV | EN_REF_NO_BG |
                   EN_MICDET | RCV_EN | EN_MICBIAS);//0x4CA4;
        BachWriteReg2Byte(0x112C06, nValue);

        nValue = BachReadReg2Byte(0x112C08);
        nValue |= EN_MICDET_LP;//0x1000;
        BachWriteReg2Byte(0x112C08, nValue);

        nValue = BachReadReg2Byte(0x112C0C);
        nValue |= (EN_DAC0_LDO11 | EN_DAC0_L | EN_DAC0_R);//0x0007;			//enable L/R DAC0
        BachWriteReg2Byte(0x112C0C, nValue);

        nValue = BachReadReg2Byte(0x112C0E);
        nValue |= (EN_DAC1_LDO11 | EN_DAC1_L | EN_DAC1_R);//0x3800;			////enable L/R DAC1
        BachWriteReg2Byte(0x112C0E, nValue);

        nValue = BachReadReg2Byte(0x112C10);
        nValue |= EN_OPLP;//0x0010;
        BachWriteReg2Byte(0x112C10, nValue);

        ///////////////////////////////////////////
        nValue = BachReadReg2Byte(0x112C14);
        nValue &= ~MUTE_LINE0;
        nValue |= (m_nLineOutMute[0] << MUTE_LINE0_POS);
        nValue |= (EN_LINE0_L | EN_LINE0_R);//;0xC000;	//Enable L/R-channel LINE-out PGA0
        nValue &= ~GAIN_LINE0_MSK;
        nValue |= (m_nLineOutGain[0]<<GAIN_LINE0_POS) & GAIN_LINE0_MSK;
        BachWriteReg2Byte(0x112C14, nValue);
        //////////////////////////////////////////

        nValue = BachReadReg2Byte(0x112C16);
        nValue |= (EN_LDO25 | EN_CLK);//0x9080;
        BachWriteReg2Byte(0x112C16, nValue);

        m_bBachAtopStatus[BACH_ATOP_LINEOUT0] = TRUE;

    }
    else
    {
        ///////////////////////////////////////////
        nValue = BachReadReg2Byte(0x112C14);
        nValue &= ~(EN_LINE0_L | EN_LINE0_R);//;0xC000;		//diable L/R-channel LINE-out PGA0
        nValue &= ~GAIN_LINE0_MSK;
        BachWriteReg2Byte(0x112C14, nValue);
        //////////////////////////////////////////
        m_bBachAtopStatus[BACH_ATOP_LINEOUT0] = FALSE;
    }
}

void BachAtopLineOut1(BOOL bEnable)
{
    U16 nValue;


    if (bEnable)
    {
        nValue = BachReadReg2Byte(0x112C06);
        nValue |= (EN_AUDIO_IBIAS | EN_POLLING_DRV | EN_REF_NO_BG |
                   EN_MICDET | RCV_EN | EN_MICBIAS);//0x4CA4;
        BachWriteReg2Byte(0x112C06, nValue);

        nValue = BachReadReg2Byte(0x112C08);
        nValue |= EN_MICDET_LP;//0x1000;
        BachWriteReg2Byte(0x112C08, nValue);

        nValue = BachReadReg2Byte(0x112C0C);
        nValue |= (EN_DAC0_LDO11 | EN_DAC0_L | EN_DAC0_R);//0x0007;			//enable L/R DAC0
        BachWriteReg2Byte(0x112C0C, nValue);

        nValue = BachReadReg2Byte(0x112C0E);
        nValue |= (EN_DAC1_LDO11 | EN_DAC1_L | EN_DAC1_R);//0x3800;			////enable L/R DAC1
        BachWriteReg2Byte(0x112C0E, nValue);

        nValue = BachReadReg2Byte(0x112C10);
        nValue |= EN_OPLP;//0x0010;
        BachWriteReg2Byte(0x112C10, nValue);

        ///////////////////////////////////////////
        nValue = BachReadReg2Byte(0x112C14);
        nValue &= ~MUTE_LINE1;
        nValue |= (m_nLineOutMute[1] << MUTE_LINE1_POS);
        nValue |= (EN_LINE1_L | EN_LINE1_R);//0x00C0;	//Enable L/R-channel LINE-out PGA1
        nValue &= ~GAIN_LINE1_MSK;
        nValue |= (m_nLineOutGain[1]<<GAIN_LINE1_POS) & GAIN_LINE1_MSK;
        BachWriteReg2Byte(0x112C14, nValue);
        //////////////////////////////////////////

        nValue = BachReadReg2Byte(0x112C16);
        nValue |= (EN_LDO25 | EN_CLK);//0x9080;
        BachWriteReg2Byte(0x112C16, nValue);

        m_bBachAtopStatus[BACH_ATOP_LINEOUT1] = TRUE;
    }
    else
    {
        ///////////////////////////////////////////
        nValue = BachReadReg2Byte(0x112C14);
        nValue &= ~(EN_LINE1_L | EN_LINE1_R);//0x00C0;	//diable L/R-channel LINE-out PGA1
        BachWriteReg2Byte(0x112C14, nValue);
        //////////////////////////////////////////
        m_bBachAtopStatus[BACH_ATOP_LINEOUT1] = FALSE;
    }
}



BOOL BachOpenAtop(BachAtopPath_e path)
{
    if(path < 0 || path > 6)
        return FALSE;
    else
    {
        if(m_bAnalogIdle)
        {
            BachAtopEnableRef(TRUE);
            //Sleep(100);
        }

        if(path < 4)
        {
            if(!m_bADCActive)
                BachAtopADCOn(TRUE);

            switch(path)
            {
            case BACH_ATOP_LINEIN0:
                if(m_bBachAtopStatus[BACH_ATOP_AUXMIC] || m_bBachAtopStatus[BACH_ATOP_HANDSET])
                    return FALSE;
                else if(!m_bBachAtopStatus[BACH_ATOP_LINEIN0])
                    BachAtopLineIn0(TRUE);
                return TRUE;

            case BACH_ATOP_LINEIN1:
                if(!m_bBachAtopStatus[BACH_ATOP_LINEIN1])
                    BachAtopLineIn1(TRUE);
                return TRUE;
            case BACH_ATOP_HANDSET:
                if(m_bBachAtopStatus[BACH_ATOP_LINEIN0])
                    return FALSE;
                else if(!m_bBachAtopStatus[BACH_ATOP_HANDSET])
                    BachAtopHandsetMic(TRUE);
                return TRUE;
            case BACH_ATOP_AUXMIC:
                if(m_bBachAtopStatus[BACH_ATOP_LINEIN0])
                    return FALSE;
                else if(!m_bBachAtopStatus[BACH_ATOP_AUXMIC])
                    BachAtopAuxMic(TRUE);
                return TRUE;
            default:
                ERRMSG("BachOpenAtop - ERROR BachAtopPath_e case!\n");
                return FALSE;
            }
            return TRUE;
        }
        else
        {
            if(!m_bDACActive)
                BachAtopDACOn(TRUE);

            switch(path)
            {
            case BACH_ATOP_LINEOUT0:
                if(!m_bBachAtopStatus[BACH_ATOP_LINEOUT0])
                    BachAtopLineOut0(TRUE);
                break;
            case BACH_ATOP_LINEOUT1:
                if(!m_bBachAtopStatus[BACH_ATOP_LINEOUT1])
                    BachAtopLineOut1(TRUE);
                break;
            case BACH_ATOP_EAR:
                if(!m_bBachAtopStatus[BACH_ATOP_EAR])
                    BachAtopEarphone(TRUE);
                break;
            default:
                ERRMSG("BachOpenAtop - ERROR BachAtopPath_e case!\n");
                return FALSE;
            }
            return TRUE;
        }
    }
}

BOOL BachCloseAtop(BachAtopPath_e path)
{
    if(path < 0 || path > 6)
        return FALSE;
    else
    {
        switch(path)
        {
        case BACH_ATOP_LINEIN0:
            if(m_bBachAtopStatus[BACH_ATOP_LINEIN0])
                BachAtopLineIn0(FALSE);
            break;
        case BACH_ATOP_LINEIN1:
            if(m_bBachAtopStatus[BACH_ATOP_LINEIN1])
                BachAtopLineIn1(FALSE);
            break;
        case BACH_ATOP_HANDSET:
            if(m_bBachAtopStatus[BACH_ATOP_HANDSET])
                BachAtopHandsetMic(FALSE);
            break;
        case BACH_ATOP_AUXMIC:
            if(m_bBachAtopStatus[BACH_ATOP_AUXMIC])
                BachAtopAuxMic(FALSE);
            break;
        case BACH_ATOP_LINEOUT0:
            if(m_bBachAtopStatus[BACH_ATOP_LINEOUT0])
                BachAtopLineOut0(FALSE);
            break;
        case BACH_ATOP_LINEOUT1:
            if(m_bBachAtopStatus[BACH_ATOP_LINEOUT1])
                BachAtopLineOut1(FALSE);
            break;
        case BACH_ATOP_EAR:
            if(m_bBachAtopStatus[BACH_ATOP_EAR])
                BachAtopEarphone(FALSE);
            break;
        default:
            ERRMSG("BachCloseAtop - ERROR BachAtopPath_e case!\n");
            return FALSE;
        }

        if(m_bADCActive && !(m_bBachAtopStatus[BACH_ATOP_LINEIN0] || m_bBachAtopStatus[BACH_ATOP_LINEIN1] || m_bBachAtopStatus[BACH_ATOP_AUXMIC] || m_bBachAtopStatus[BACH_ATOP_HANDSET]))
            BachAtopADCOn(FALSE);

        if(m_bDACActive && !(m_bBachAtopStatus[BACH_ATOP_LINEOUT0] || m_bBachAtopStatus[BACH_ATOP_LINEOUT1] || m_bBachAtopStatus[BACH_ATOP_EAR]))
            BachAtopDACOn(FALSE);

        if(!m_bAnalogIdle && !(m_bADCActive || m_bDACActive))
        {
            BachAtopEnableRef(FALSE);
            if(m_ChipRevision == REVISION_U01)
            {
                BachAtopIdle();
            }
            else
            {
                BachAtopEnDigClk();
            }
        }

        return TRUE;
    }
}


BOOL BachAtopIsTrimOk(void)
{
    if(BachReadReg(BACH_REG_BANK3, BACH_ANALOG_CTRL16) & TRIM_AU_OK)
        return (BachReadReg(BACH_REG_BANK3, BACH_ANALOG_CTRL16) & TRIM_AU_OK) ? TRUE : FALSE;
    else
        return FALSE;
}

void BachDigMic(BOOL bEnable)
{
    U16 nValue;
    if(bEnable)
    {
        BachWriteReg2Byte(0x112B30, 0x8200);
        BachWriteReg2Byte(0x112B32, 0xA929);
        BachWriteReg2Byte(0x112B34, 0x2929);
    }
    else
    {
        nValue = BachReadReg2Byte(0x112B30);
        nValue &= ~REG_DIG_MIC_EN ;
        BachWriteReg2Byte(0x112B30, nValue);
    }
}

void BachSetOutputPathOnOff(BachOutputPath_e eOutput, BOOL bOn)
{
    BachDpga_e dpgaL, dpgaR;
    S8  nGain = 0;

    dpgaL = dpgaR = BACH_DPGA_NULL;

    switch(eOutput)
    {
    case BACH_OUTPUT_PATH_DMA1_WRITER:
        dpgaL = BACH_DPGA_DMAWR3;
        dpgaR = BACH_DPGA_DMAWR4;
        nGain = m_nRecGain[0];
        break;
    case BACH_OUTPUT_PATH_DMA2_WRITER:
        dpgaL = BACH_DPGA_DMAWR1;
        dpgaR = BACH_DPGA_DMAWR2;
        nGain = m_nRecGain[1];
        break;
    case BACH_OUTPUT_PATH_CODEC_I2S:
        dpgaL = BACH_DPGA_CODEC1;
        dpgaR = BACH_DPGA_CODEC2;
        break;
    case BACH_OUTPUT_PATH_DBB_I2S:
        dpgaR = BACH_DPGA_DEC1_2;
        //dpgaR = BACH_DPGA_DMARD_R;
        break;
    case BACH_OUTPUT_PATH_DBB_AEC_I2S:
        //dpgaL = BACH_DPGA_DEC1_2;
        dpgaR = BACH_DPGA_DMARD_R;
        break;
    case BACH_OUTPUT_PATH_BT_I2S:
        //dpgaL = BACH_DPGA_MMP1;
        dpgaR = BACH_DPGA_MMP2;
        break;
    case BACH_OUTPUT_PATH_SPDIF_HDMI:
        dpgaL = BACH_DPGA_SPDIF1;
        dpgaR = BACH_DPGA_SPDIF2;
        break;
    default:
        ERRMSG("BachSetOutputPathOnOff - default case!\n");
        break;

    }
    if((dpgaL != BACH_DPGA_NULL) && (dpgaR != BACH_DPGA_NULL))
    {
        if(bOn)
        {
            BachDpgaInit(dpgaL, nGain);
            BachDpgaInit(dpgaR, nGain);
        }
        else
        {
            BachDpgaMute(dpgaL);
            BachDpgaMute(dpgaR);
        }
        return;
    }
    else if(dpgaR != BACH_DPGA_NULL)
    {
        if(bOn)
            BachDpgaEnable(dpgaR, FALSE);
        else
            BachDpgaMute(dpgaR);
        return;
    }

    /*switch(eOutput)
    {
        case BACH_OUTPUT_PATH_DAC1:
    		if(bOn)
    		{
    			BachOpenAtop(BACH_ATOP_LINEOUT0);
    			BachPgaEnable(BACH_PGA1,FALSE);
    			BachPgaEnable(BACH_PGA2,FALSE);
    		}
    		else
    		{
    			BachCloseAtop(BACH_ATOP_LINEOUT0);
    			BachPgaMute(BACH_PGA1);
    			BachPgaMute(BACH_PGA2);
    		}
    		break;
      	case BACH_OUTPUT_PATH_DAC2:
    		if(bOn)
    		{
    			BachOpenAtop(BACH_ATOP_LINEOUT1);
    			BachPgaEnable(BACH_PGA3,FALSE);
    			BachPgaEnable(BACH_PGA4,FALSE);
    		}
    		else
    		{
    			BachCloseAtop(BACH_ATOP_LINEOUT1);
    			BachPgaMute(BACH_PGA3);
    			BachPgaMute(BACH_PGA4);
    		}
    		break;
    	default:
    		ERRMSG("BachSetOutputPathOnOff error output path!!");
    	return;

    }*/
}

void BachSetInputPathOnOff(BachInputPath_e eInput, BOOL bOn)
{
    BachDpga_e dpgaL, dpgaR;

    dpgaL = dpgaR = BACH_DPGA_NUM;

    switch(eInput)
    {
    case BACH_INPUT_PATH_ASRC1:
        dpgaL = BACH_DPGA_MMP1;
        dpgaR = BACH_DPGA_MMP2;
        break;
    case BACH_INPUT_PATH_ASRC2:
        dpgaL = BACH_DPGA_ASRC2MMP1;
        dpgaR = BACH_DPGA_ASRC2MMP2;
        break;
    default:
        ERRMSG("BachSetInputPathOnOff error input path!!\n");
        return;
    }

    if(bOn)
    {

        BachDpgaInit(dpgaL, m_nOutputGain[eInput]);
        BachDpgaInit(dpgaR, m_nOutputGain[eInput]);

        //BachDpgaEnable(dpgaL,FALSE);
        //BachDpgaEnable(dpgaR,FALSE);

    }
    else
    {
        // m_nOutputGain[eInput] = BachDpgaGetGain(dpgaL);
        BachDpgaMute(dpgaL);
        BachDpgaMute(dpgaR);
    }
}

void BachSetInputPathGain(BachInputPath_e eInput, S8 s8Gain, BachChannelMode_e eChannel)
{
    BachDpga_e dpgaL, dpgaR;

    dpgaL = dpgaR = BACH_DPGA_NULL;

    switch(eInput)
    {
    case BACH_INPUT_PATH_ASRC1:
        dpgaL = BACH_DPGA_MMP1;
        dpgaR = BACH_DPGA_MMP2;
        break;
    case BACH_INPUT_PATH_ASRC2:
        dpgaL = BACH_DPGA_ASRC2MMP1;
        dpgaR = BACH_DPGA_ASRC2MMP2;
        break;
    default:
        ERRMSG("BachSetInputPathGain error input path!!\n");
        return;
    }
    if(eChannel & BACH_CHMODE_LEFT)
    {
        BachDpgaSetGain(dpgaL, s8Gain);
        m_nOutputGain[eInput] = BachDpgaGetGain(dpgaL);
    }
    if(eChannel & BACH_CHMODE_RIGHT)
    {
        BachDpgaSetGain(dpgaR, s8Gain);
        m_nOutputGain[eInput] = BachDpgaGetGain(dpgaR);
    }
}


S8 BachGetInputPathGain(BachInputPath_e eInput)
{
    return m_nOutputGain[eInput];
}

void BachSetRecPathGain(BachRecPath_e eRec, S8 s8Gain)
{
    BachDpga_e dpgaL, dpgaR;

    dpgaL = dpgaR = BACH_DPGA_NULL;

    switch(eRec)
    {
    case BACH_REC_PATH_DMA1:
        dpgaL = BACH_DPGA_DMAWR3;
        dpgaR = BACH_DPGA_DMAWR4;
        break;
    case BACH_REC_PATH_DMA2:
        dpgaL = BACH_DPGA_DMAWR1;
        dpgaR = BACH_DPGA_DMAWR2;
        break;
    default:
        ERRMSG("BachSetRecPathGain error rec path!!\n");
        return;
    }

    BachDpgaSetGain(dpgaL, s8Gain);
    BachDpgaSetGain(dpgaR, s8Gain);
    m_nRecGain[eRec] = BachDpgaGetGain(dpgaR);

}

S8 BachGetRecPathGain(BachRecPath_e eRec)
{
    return m_nRecGain[eRec];
}

//------------------------------------------------------------------------------
//
//  Function:   BachC3::MapHardware
//
//  Description
//      Maps port address, assigns SysIntr.
//
//  Parameters
//      u32RegAddr:     [in] Physical address of audio hardware register.
//      u32RegLength:   [in] Length of the address space of audio hardware register.
//
//  Return Value
//      Returns TRUE if device was mapped properly
//      Return FALSE if device could not be mapped
//------------------------------------------------------------------------------
void BachDmaInitChannel( U32 nChannelIndex,
                         U32 nPhysDMAAddr,
                         U32 nBufferSize,
                         U32 nChannels,
                         U32 nSampleSize,
                         U32 nSampleRate,
                         U32 nOverrunTh,
                         U32 nUnderrunTh
                       )
{
    //U16 nConfigValue;

    // save off the info for power managment
    m_dmachannel[nChannelIndex].nPhysDMAAddr = nPhysDMAAddr;
    m_dmachannel[nChannelIndex].nBufferSize  = nBufferSize;
    m_dmachannel[nChannelIndex].nChannels   = nChannels;
    m_dmachannel[nChannelIndex].nSampleSize = nSampleSize;
    m_dmachannel[nChannelIndex].nSampleRate = nSampleRate;


    // Set up the physical DMA buffer address
    BachDmaSetPhyAddr((BachDmaChannel_e)nChannelIndex, nPhysDMAAddr, nBufferSize);


    // Set up the underrun and overrun
    //DmaSetThreshold((BachDmaChannel_e)nChannelIndex, nBufferSize+MIU_WORD_BYTE_SIZE, (nBufferSize/4)*3);
    BachDmaSetThreshold((BachDmaChannel_e)nChannelIndex, nOverrunTh, nUnderrunTh);

    return ;
}

void BachDmaReset(void)
{
    //rest DMA1 interal register
    /* PR98079 - Reset read and write size. Otherwise Bach update level count if REG_WR_TRIG is 1. */
    /*WriteBank1Reg(BACH_DMA1_CTRL_4, 0);
      WriteBank1Reg(BACH_DMA1_CTRL_12, 0);

      nConfigValue = ReadBank1Reg(BACH_DMA1_CTRL_0);
      nConfigValue |= REG_SW_RST_DMA;
      WriteBank1Reg(BACH_DMA1_CTRL_0, nConfigValue);

      nConfigValue = ReadBank1Reg(BACH_DMA1_CTRL_0);
      nConfigValue &= ~REG_SW_RST_DMA;
      WriteBank1Reg(BACH_DMA1_CTRL_0, nConfigValue);*/

    BachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_4, 0xFFFF, 0);		//reset DMA 1 read size
    BachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_12, 0xFFFF, 0);		//reset DMA 1 write size

    BachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_0, REG_SW_RST_DMA, REG_SW_RST_DMA);		//DMA 1 software reset
    BachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_0, REG_SW_RST_DMA, 0);


    //rest DMA2 interal register
    /*WriteBank1Reg(BACH_DMA2_CTRL_4, 0);
    WriteBank1Reg(BACH_DMA2_CTRL_12, 0);

    nConfigValue = ReadBank1Reg(BACH_DMA2_CTRL_0);
    nConfigValue |= REG_SW_RST_DMA;
    WriteBank1Reg(BACH_DMA2_CTRL_0, nConfigValue);

    nConfigValue = ReadBank1Reg(BACH_DMA2_CTRL_0);
    nConfigValue &= ~REG_SW_RST_DMA;
    WriteBank1Reg(BACH_DMA2_CTRL_0, nConfigValue);*/
    BachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_4, 0xFFFF, 0);		//reset DMA 2 read size
    BachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_12, 0xFFFF, 0);		//reset DMA 2 write size

    BachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_0, REG_SW_RST_DMA, REG_SW_RST_DMA);		//DMA 2 software reset
    BachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_0, REG_SW_RST_DMA, 0);


    BachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_0, (REG_PRIORITY_KEEP_HIGH | REG_RD_LEVEL_CNT_LIVE_MASK | REG_ENABLE),
                 (REG_PRIORITY_KEEP_HIGH | REG_RD_LEVEL_CNT_LIVE_MASK | REG_ENABLE));

    BachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_0, (REG_PRIORITY_KEEP_HIGH | REG_RD_LEVEL_CNT_LIVE_MASK | REG_ENABLE),
                 (REG_PRIORITY_KEEP_HIGH | REG_RD_LEVEL_CNT_LIVE_MASK | REG_ENABLE));

    //enable DMA interrupt
    BachWriteReg(BACH_REG_BANK1, BACH_INT_CTRL, AU_DMA_INT_EN, AU_DMA_INT_EN);

}

void BachDmaReInit(BachDmaChannel_e eDmaChannel)
{
    // U16 nConfigValue;
    switch ( eDmaChannel )
    {
    case BACH_DMA_READER1:

        /*nConfigValue = ReadBank1Reg(BACH_DMA1_CTRL_0);
          //nConfigValue |= (REG_RD_UNDERRUN_INT_EN | REG_RD_OVERRUN_INT_EN | REG_RD_EMPTY_INT_EN | REG_RD_LEVEL_CNT_LIVE_MASK | REG_ENABLE);
          nConfigValue |= (REG_RD_LEVEL_CNT_LIVE_MASK | REG_ENABLE);
          WriteBank1Reg(BACH_DMA1_CTRL_0, nConfigValue);

        nConfigValue = ReadBank1Reg(BACH_DMA1_CTRL_1);
          nConfigValue |= REG_RD_INIT;
          WriteBank1Reg(BACH_DMA1_CTRL_1, nConfigValue);
          nConfigValue &= ~REG_RD_INIT;
          WriteBank1Reg(BACH_DMA1_CTRL_1, nConfigValue);*/

        //remove to reset()
        //BachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_0, (
        //	REG_RD_LEVEL_CNT_LIVE_MASK | REG_ENABLE), (REG_RD_LEVEL_CNT_LIVE_MASK | REG_ENABLE));

        BachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_1, REG_RD_INIT, REG_RD_INIT);
        BachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_1, REG_RD_INIT, 0);
        break;

    case BACH_DMA_WRITER1:
        //BachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_0,
        //	(REG_RD_LEVEL_CNT_LIVE_MASK | REG_ENABLE), (REG_RD_LEVEL_CNT_LIVE_MASK | REG_ENABLE));

        BachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_9, REG_WR_INIT, REG_WR_INIT);
        BachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_9, REG_WR_INIT, 0);
        break;

    case BACH_DMA_READER2:

        /* nConfigValue = ReadBank1Reg(BACH_DMA2_CTRL_0);
         //nConfigValue |= (REG_RD_UNDERRUN_INT_EN | REG_RD_OVERRUN_INT_EN | REG_RD_EMPTY_INT_EN | REG_RD_LEVEL_CNT_LIVE_MASK | REG_ENABLE);
         nConfigValue |= (REG_RD_LEVEL_CNT_LIVE_MASK | REG_ENABLE);
         WriteBank1Reg(BACH_DMA2_CTRL_0, nConfigValue);

        nConfigValue = ReadBank1Reg(BACH_DMA2_CTRL_1);
         nConfigValue |= REG_RD_INIT;
         WriteBank1Reg(BACH_DMA2_CTRL_1, nConfigValue);
         nConfigValue &= ~REG_RD_INIT;
         WriteBank1Reg(BACH_DMA2_CTRL_1, nConfigValue);*/
        //BachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_0,
        //	(REG_RD_LEVEL_CNT_LIVE_MASK | REG_ENABLE),(REG_RD_LEVEL_CNT_LIVE_MASK | REG_ENABLE));

        BachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_1, REG_RD_INIT, REG_RD_INIT);
        BachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_1, REG_RD_INIT, 0);
        break;

    case BACH_DMA_WRITER2:

        //BachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_0,
        //	(REG_RD_LEVEL_CNT_LIVE_MASK | REG_ENABLE),(REG_RD_LEVEL_CNT_LIVE_MASK | REG_ENABLE));

        BachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_9, REG_WR_INIT, REG_WR_INIT);
        BachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_9, REG_WR_INIT, 0);
        break;

    default:
        ERRMSG("BachDmaReInit - ERROR bank default case!\n");
        break;
    }
}

void BachDmaEnable(BachDmaChannel_e eDmaChannel, BOOL bEnable)
{
    // U16 nConfigValue;

    switch ( eDmaChannel )
    {
    case BACH_DMA_READER1:

        /*nConfigValue = ReadBank1Reg(BACH_DMA1_CTRL_1);
        if(bEnable)
        {
            nConfigValue |= REG_RD_ENABLE;
        }
        else
        {
            nConfigValue &= ~REG_RD_ENABLE;
        }
        WriteBank1Reg(BACH_DMA1_CTRL_1, nConfigValue);*/

        BachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_0,
                     (REG_RD_EMPTY_INT_EN | REG_RD_UNDERRUN_INT_EN),
                     (bEnable ? (REG_RD_EMPTY_INT_EN | REG_RD_UNDERRUN_INT_EN) : 0));

        BachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_1, REG_RD_ENABLE, (bEnable ? REG_RD_ENABLE : 0));

        break;

    case BACH_DMA_READER2:

        BachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_0,
                     (REG_RD_EMPTY_INT_EN | REG_RD_UNDERRUN_INT_EN),
                     (bEnable ? (REG_RD_EMPTY_INT_EN | REG_RD_UNDERRUN_INT_EN) : 0));

        BachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_1, REG_RD_ENABLE, (bEnable ? REG_RD_ENABLE : 0));

        break;


    case BACH_DMA_WRITER1:

        /*nConfigValue = ReadBank1Reg(BACH_DMA1_CTRL_9);
          if(bEnable)
          {
              nConfigValue |= REG_WR_ENABLE;
          }
          else
          {
              nConfigValue &= ~REG_WR_ENABLE;
          }

        	WriteBank1Reg(BACH_DMA1_CTRL_9, nConfigValue);*/

        BachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_0,
                     (REG_WR_FULL_INT_EN | REG_WR_OVERRUN_INT_EN),
                     (bEnable ? (REG_WR_FULL_INT_EN | REG_WR_OVERRUN_INT_EN) : 0));

        BachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_9, REG_WR_ENABLE, (bEnable ? REG_WR_ENABLE : 0));

        break;


    case BACH_DMA_WRITER2:

        BachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_0,
                     (REG_WR_FULL_INT_EN | REG_WR_OVERRUN_INT_EN),
                     (bEnable ? (REG_WR_FULL_INT_EN | REG_WR_OVERRUN_INT_EN) : 0));

        BachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_9, REG_WR_ENABLE, (bEnable ? REG_WR_ENABLE : 0));

        break;

    default:
        ERRMSG("BachDmaEnable - ERROR bank default case!\n");
        break;

    }

}

void BachDmaStartChannel(BachDmaChannel_e eDmaChannel)
{
//TRACE1("BachDmaStartChannel %d",eDmaChannel);
    //DmaReInit(eDmaChannel);
    BachDmaClearInt(eDmaChannel);
    BachDmaEnable(eDmaChannel, TRUE);
    m_dmachannel[eDmaChannel].nDMAChannelState = DMA_RUNNING;  // save the state
    return;
}

void BachDmaStopChannel(BachDmaChannel_e eDmaChannel)
{
    BachDmaEnable(eDmaChannel, FALSE);
    BachDmaReInit(eDmaChannel);
    //Sleep(100);
    // save the state
    m_dmachannel[eDmaChannel].nDMAChannelState = DMA_STOPPED;
    return;
}


/**
 * \brief clear DMA2 interrupt
 */
void BachDmaClearInt(BachDmaChannel_e eDmaChannel)
{
//    U16 nConfigValue;

    switch(eDmaChannel)
    {

    case BACH_DMA_WRITER1:
        //BACH_DMA1_INT_CLR_WR_FULL:
        /*nConfigValue = ReadBank1Reg(BACH_DMA1_CTRL_0);
        nConfigValue |= REG_WR_FULL_FLAG_CLR;
        WriteBank1Reg(BACH_DMA1_CTRL_0, nConfigValue);
        nConfigValue &= ~REG_WR_FULL_FLAG_CLR;
        WriteBank1Reg(BACH_DMA1_CTRL_0, nConfigValue);*/

        //DMA writer full flag clear / DMA writer local buffer full flag clear
        BachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_0, REG_WR_FULL_FLAG_CLR, REG_WR_FULL_FLAG_CLR);
        BachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_0, REG_WR_FULL_FLAG_CLR, 0);
        break;

    case BACH_DMA_WRITER2:
        //BACH_DMA2_INT_CLR_WR_FULL:
        BachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_0, REG_WR_FULL_FLAG_CLR, REG_WR_FULL_FLAG_CLR);
        BachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_0, REG_WR_FULL_FLAG_CLR, 0);
        break;

    case BACH_DMA_READER1:
        //BACH_DMA1_INT_CLR_RD_EMPTY:
        /*nConfigValue = ReadBank1Reg(BACH_DMA1_CTRL_0);
        nConfigValue |= REG_RD_EMPTY_FLAG_CLR;
        WriteBank1Reg(BACH_DMA1_CTRL_0, nConfigValue);
        nConfigValue &= ~REG_RD_EMPTY_FLAG_CLR;
        WriteBank1Reg(BACH_DMA1_CTRL_0, nConfigValue);*/

        //DMA reader empty flag clear / DMA reader local buffer empty flag clear
        BachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_0, REG_RD_EMPTY_FLAG_CLR, REG_RD_EMPTY_FLAG_CLR);
        BachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_0, REG_RD_EMPTY_FLAG_CLR, 0);

        break;

    case BACH_DMA_READER2:
        //BACH_DMA2_INT_CLR_RD_EMPTY:
        BachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_0, REG_RD_EMPTY_FLAG_CLR, REG_RD_EMPTY_FLAG_CLR);
        BachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_0, REG_RD_EMPTY_FLAG_CLR, 0);
        break;

    default:
        ERRMSG("BachDmaClearInt - ERROR bank default case!\n");
        break;
    }

    return;
}

U32 BachDmaGetLevelCnt(BachDmaChannel_e eDmaChannel)
{
    U16 nConfigValue = 0;
    U32 nByteSize = 0;

    switch(eDmaChannel)
    {
    case BACH_DMA_WRITER1:
    {
        /*nConfigValue = ReadBank1Reg(BACH_DMA1_CTRL_9);
        nConfigValue |= REG_WR_LEVEL_CNT_MASK;
        WriteBank1Reg(BACH_DMA1_CTRL_9, nConfigValue);

        nConfigValue = ReadBank1Reg(BACH_DMA1_CTRL_15);
        nByteSize = nConfigValue * MIU_WORD_BYTE_SIZE;

        nConfigValue = ReadBank1Reg(BACH_DMA1_CTRL_9);
        nConfigValue &= ~REG_WR_LEVEL_CNT_MASK;
        WriteBank1Reg(BACH_DMA1_CTRL_9, nConfigValue);*/

        BachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_9, REG_WR_LEVEL_CNT_MASK, REG_WR_LEVEL_CNT_MASK);
        nConfigValue = BachReadReg(BACH_REG_BANK1, BACH_DMA1_CTRL_15);
        BachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_9, REG_WR_LEVEL_CNT_MASK, 0);
        nConfigValue = ((nConfigValue>8)? (nConfigValue-8):0); //level count contains the local buffer data size
        break;
    }
    case BACH_DMA_WRITER2:
    {
        BachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_9, REG_WR_LEVEL_CNT_MASK, REG_WR_LEVEL_CNT_MASK);
        nConfigValue = BachReadReg(BACH_REG_BANK1, BACH_DMA2_CTRL_15);
        BachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_9, REG_WR_LEVEL_CNT_MASK, 0);
        nConfigValue = ((nConfigValue>8)? (nConfigValue-8):0); //level count contains the local buffer data size
        break;
    }
    case BACH_DMA_READER1:
    {
        /*nConfigValue = ReadBank1Reg(BACH_DMA1_CTRL_1);
        nConfigValue |= REG_RD_LEVEL_CNT_MASK;
        WriteBank1Reg(BACH_DMA1_CTRL_1, nConfigValue);

        nConfigValue = ReadBank1Reg(BACH_DMA1_CTRL_7);
        nByteSize = nConfigValue * MIU_WORD_BYTE_SIZE;

        nConfigValue = ReadBank1Reg(BACH_DMA1_CTRL_1);
        nConfigValue &= ~REG_RD_LEVEL_CNT_MASK;
        WriteBank1Reg(BACH_DMA1_CTRL_1, nConfigValue);*/
        BachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_1, REG_RD_LEVEL_CNT_MASK, REG_RD_LEVEL_CNT_MASK);
        nConfigValue = BachReadReg(BACH_REG_BANK1, BACH_DMA1_CTRL_7);
        BachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_1, REG_RD_LEVEL_CNT_MASK, 0);


        break;
    }
    case BACH_DMA_READER2:
    {
        BachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_1, REG_RD_LEVEL_CNT_MASK, REG_RD_LEVEL_CNT_MASK);
        nConfigValue = BachReadReg(BACH_REG_BANK1, BACH_DMA2_CTRL_7);
        BachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_1, REG_RD_LEVEL_CNT_MASK, 0);
        break;
    }

    default:
        ERRMSG("BachDmaGetLevelCnt - ERROR bank default case!\n");
        return 0;

    }

    nByteSize = nConfigValue * MIU_WORD_BYTE_SIZE;

    return nByteSize;
}

void BachDmaSetThreshold(BachDmaChannel_e eDmaChannel, U32 nOverrunTh, U32 nUnderrunTh)
{
    U16 nMiuOverrunTh, nMiuUnderrunTh;

    switch(eDmaChannel)
    {
    case BACH_DMA_WRITER1:
        nMiuOverrunTh = (U16)((nOverrunTh / MIU_WORD_BYTE_SIZE) & REG_WR_OVERRUN_TH_MSK);
        nMiuUnderrunTh = (U16)((nUnderrunTh / MIU_WORD_BYTE_SIZE) & REG_WR_UNDERRUN_TH_MSK);
        /*WriteBank1Reg(BACH_DMA1_CTRL_13, nMiuOverrunTh);
        WriteBank1Reg(BACH_DMA1_CTRL_14, nMiuUnderrunTh);*/
        BachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_13, 0xFFFF, nMiuOverrunTh);
        BachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_14, 0xFFFF, nMiuUnderrunTh);
        break;

    case BACH_DMA_WRITER2:
        nMiuOverrunTh = (U16)((nOverrunTh / MIU_WORD_BYTE_SIZE) & REG_WR_OVERRUN_TH_MSK);
        nMiuUnderrunTh = (U16)((nUnderrunTh / MIU_WORD_BYTE_SIZE) & REG_WR_UNDERRUN_TH_MSK);
        BachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_13, 0xFFFF, nMiuOverrunTh);
        BachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_14, 0xFFFF, nMiuUnderrunTh);
        break;

    case BACH_DMA_READER1:

        nMiuOverrunTh = (U16)((nOverrunTh / MIU_WORD_BYTE_SIZE) & REG_RD_OVERRUN_TH_MSK);
        nMiuUnderrunTh = (U16)((nUnderrunTh / MIU_WORD_BYTE_SIZE) & REG_RD_UNDERRUN_TH_MSK);
        /*WriteBank1Reg(BACH_DMA1_CTRL_5, nMiuOverrunTh);
        WriteBank1Reg(BACH_DMA1_CTRL_6, nMiuUnderrunTh);*/
        BachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_5, 0xFFFF, nMiuOverrunTh);
        BachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_6, 0xFFFF, nMiuUnderrunTh);
        break;

    case BACH_DMA_READER2:

        nMiuOverrunTh = (U16)((nOverrunTh / MIU_WORD_BYTE_SIZE) & REG_RD_OVERRUN_TH_MSK);
        nMiuUnderrunTh = (U16)((nUnderrunTh / MIU_WORD_BYTE_SIZE) & REG_RD_UNDERRUN_TH_MSK);
        BachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_5, 0xFFFF, nMiuOverrunTh);
        BachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_6, 0xFFFF, nMiuUnderrunTh);
        break;

    default:
        ERRMSG("BachDmaSetThreshold - ERROR bank default case!\n");
        break;
    }

}

/**
 * \brief DMA set MIU address
 */
void BachDmaSetPhyAddr(BachDmaChannel_e eDmaChannel, U32 nBufAddrOffset, U32 nBufSize)
{
//    U16 nConfigValue;
    U16 nMiuAddrLo, nMiuAddrHi, nMiuSize;
    //U32 nOffset = nBufAddr & ~0xf0000000;		//transfer to miu bus address

    ///nOffset = HalUtilPHY2MIUAddr(nBufAddr);
    //nOffset = getSysPhyAddr(nBufAddr);
    //nOffset = nBufAddr - 0x40000000;

    //printf("HalBachDma2SetWrMiuAddr nBufAddr:%x, nBufSize:%x, PHYaddress:%x\n",nBufAddr,nBufSize,nOffset);
    ///nMiuAddrLo = (nBufAddr) & REG_WR_BASE_ADDR_LO_MSK;
    ///nMiuAddrHi = ((nBufAddr) >> REG_WR_BASE_ADDR_HI_OFFSET) & REG_WR_BASE_ADDR_HI_MSK;
    ///nMiuSize = (nBufSize / MIU_WORD_BYTE_SIZE) & REG_WR_BUFF_SIZE_MSK;

    //TRACE2("BachDmaSetPhyAddr() MIU addr = 0x%x, size = 0x%x", nOffset, nBufSize);

    switch(eDmaChannel)
    {
    case BACH_DMA_WRITER1:
        nMiuAddrLo = (U16)((nBufAddrOffset / MIU_WORD_BYTE_SIZE) & REG_WR_BASE_ADDR_LO_MSK);
        nMiuAddrHi = (U16)(((nBufAddrOffset / MIU_WORD_BYTE_SIZE) >> REG_WR_BASE_ADDR_HI_OFFSET) & REG_WR_BASE_ADDR_HI_MSK);
        nMiuSize = (U16)((nBufSize / MIU_WORD_BYTE_SIZE) & REG_WR_BUFF_SIZE_MSK);

        /*nConfigValue = ReadBank1Reg(BACH_DMA1_CTRL_9);
        nConfigValue &= ~REG_WR_BASE_ADDR_LO_MSK;
        nConfigValue |= nMiuAddrLo;
        WriteBank1Reg(BACH_DMA1_CTRL_9, nConfigValue);

        nConfigValue = nMiuAddrHi;
        WriteBank1Reg(BACH_DMA1_CTRL_10, nConfigValue);

        WriteBank1Reg(BACH_DMA1_CTRL_11, nMiuSize);*/

        BachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_9, REG_WR_BASE_ADDR_LO_MSK, nMiuAddrLo);
        BachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_10, REG_WR_BASE_ADDR_HI_MSK, nMiuAddrHi);
        BachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_11, 0xFFFF, nMiuSize);

        break;

    case BACH_DMA_WRITER2:
        nMiuAddrLo = (U16)((nBufAddrOffset / MIU_WORD_BYTE_SIZE) & REG_WR_BASE_ADDR_LO_MSK);
        nMiuAddrHi = (U16)(((nBufAddrOffset / MIU_WORD_BYTE_SIZE) >> REG_WR_BASE_ADDR_HI_OFFSET) & REG_WR_BASE_ADDR_HI_MSK);
        nMiuSize = (U16)((nBufSize / MIU_WORD_BYTE_SIZE) & REG_WR_BUFF_SIZE_MSK);

        /*   nConfigValue = ReadBank1Reg(BACH_DMA2_CTRL_9);
           nConfigValue &= ~REG_WR_BASE_ADDR_LO_MSK;
           nConfigValue |= nMiuAddrLo;
           WriteBank1Reg(BACH_DMA2_CTRL_9, nConfigValue);

           nConfigValue = nMiuAddrHi;
           WriteBank1Reg(BACH_DMA2_CTRL_10, nConfigValue);

           WriteBank1Reg(BACH_DMA2_CTRL_11, nMiuSize);*/

        BachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_9, REG_WR_BASE_ADDR_LO_MSK, nMiuAddrLo);
        BachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_10, REG_WR_BASE_ADDR_HI_MSK, nMiuAddrHi);
        BachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_11, 0xFFFF, nMiuSize);

        break;

    case BACH_DMA_READER1:

        nMiuAddrLo = (U16)((nBufAddrOffset / MIU_WORD_BYTE_SIZE) & REG_RD_BASE_ADDR_LO_MSK);
        nMiuAddrHi = (U16)(((nBufAddrOffset / MIU_WORD_BYTE_SIZE) >> REG_RD_BASE_ADDR_HI_OFFSET) & REG_RD_BASE_ADDR_HI_MSK);
        nMiuSize = (U16)((nBufSize / MIU_WORD_BYTE_SIZE) & REG_RD_BUFF_SIZE_MSK);

        /*nConfigValue = ReadBank1Reg(BACH_DMA1_CTRL_1);
        nConfigValue &= ~REG_RD_BASE_ADDR_LO_MSK;
        nConfigValue |= nMiuAddrLo;
        WriteBank1Reg(BACH_DMA1_CTRL_1, nConfigValue);

        nConfigValue = nMiuAddrHi;
        WriteBank1Reg(BACH_DMA1_CTRL_2, nConfigValue);

        WriteBank1Reg(BACH_DMA1_CTRL_3, nMiuSize);*/

        BachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_1, REG_RD_BASE_ADDR_LO_MSK, nMiuAddrLo);
        BachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_2, REG_RD_BASE_ADDR_HI_MSK, nMiuAddrHi);
        BachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_3, 0xFFFF, nMiuSize);

        break;

    case BACH_DMA_READER2:

        nMiuAddrLo = (U16)((nBufAddrOffset / MIU_WORD_BYTE_SIZE) & REG_RD_BASE_ADDR_LO_MSK);
        nMiuAddrHi = (U16)(((nBufAddrOffset / MIU_WORD_BYTE_SIZE) >> REG_RD_BASE_ADDR_HI_OFFSET) & REG_RD_BASE_ADDR_HI_MSK);
        nMiuSize = (U16)((nBufSize / MIU_WORD_BYTE_SIZE) & REG_RD_BUFF_SIZE_MSK);

        /*nConfigValue = ReadBank1Reg(BACH_DMA2_CTRL_1);
        nConfigValue &= ~REG_RD_BASE_ADDR_LO_MSK;
        nConfigValue |= nMiuAddrLo;
        WriteBank1Reg(BACH_DMA2_CTRL_1, nConfigValue);

        nConfigValue = nMiuAddrHi;
        WriteBank1Reg(BACH_DMA2_CTRL_2, nConfigValue);

        WriteBank1Reg(BACH_DMA2_CTRL_3, nMiuSize);*/

        BachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_1, REG_RD_BASE_ADDR_LO_MSK, nMiuAddrLo);
        BachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_2, REG_RD_BASE_ADDR_HI_MSK, nMiuAddrHi);
        BachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_3, 0xFFFF, nMiuSize);

        break;

    default:
        ERRMSG("BachDmaSetPhyAddr - ERROR bank default case!\n");
        break;

    }

}

BOOL BachDmaMaskInt(BachDmaChannel_e eDmaChan, BachDmaInterrupt_e eDmaInt, BOOL bMask)
{
    //U16 nConfigValue;

    switch(eDmaChan)
    {
    case BACH_DMA_READER1:
        BachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_0,
                     g_nDmaIntReg[BACH_DMA_READER1][eDmaInt], (bMask ? 0 : g_nDmaIntReg[BACH_DMA_READER1][eDmaInt]));
        break;

    case BACH_DMA_READER2:
        BachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_0,
                     g_nDmaIntReg[BACH_DMA_READER2][eDmaInt], (bMask ? 0 : g_nDmaIntReg[BACH_DMA_READER2][eDmaInt]));
        break;
    case BACH_DMA_WRITER1:
        BachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_0,
                     g_nDmaIntReg[BACH_DMA_WRITER1][eDmaInt], (bMask ? 0 : g_nDmaIntReg[BACH_DMA_WRITER1][eDmaInt]));
        break;

    case BACH_DMA_WRITER2:
        BachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_0,
                     g_nDmaIntReg[BACH_DMA_WRITER2][eDmaInt], (bMask ? 0 : g_nDmaIntReg[BACH_DMA_WRITER2][eDmaInt]));
        break;

    default:
        return FALSE;
    }

    return TRUE;
}

BOOL BachDmaIsFull(BachDmaChannel_e eDmaChannel)
{
    U16 nConfigValue;

    switch(eDmaChannel)
    {
    case BACH_DMA_WRITER1:
        nConfigValue = BachReadReg(BACH_REG_BANK1, BACH_DMA1_CTRL_8);
        return (nConfigValue & REG_WR_FULL_FLAG) ? TRUE : FALSE;

    case BACH_DMA_WRITER2:
        nConfigValue = BachReadReg(BACH_REG_BANK1, BACH_DMA2_CTRL_8);
        return (nConfigValue & REG_WR_FULL_FLAG) ? TRUE : FALSE;

    default:
        return FALSE;
    }
}

BOOL BachDmaIsEmpty(BachDmaChannel_e eDmaChannel)
{
    U16 nConfigValue;

    switch(eDmaChannel)
    {
    case BACH_DMA_READER1:
        nConfigValue = BachReadReg(BACH_REG_BANK1, BACH_DMA1_CTRL_8);
        return (nConfigValue & REG_RD_EMPTY_FLAG) ? TRUE : FALSE;

    case BACH_DMA_READER2:
        nConfigValue = BachReadReg(BACH_REG_BANK1, BACH_DMA2_CTRL_8);
        return (nConfigValue & REG_RD_EMPTY_FLAG) ? TRUE : FALSE;

    default:
        return FALSE;
    }
}

BOOL BachDmaIsLocalEmpty(BachDmaChannel_e eDmaChannel)
{
    U16 nConfigValue;

    switch(eDmaChannel)
    {
    case BACH_DMA_READER1:
        nConfigValue = BachReadReg(BACH_REG_BANK1, BACH_DMA1_CTRL_8);
        return (nConfigValue & REG_RD_LOCALBUF_EMPTY) ? TRUE : FALSE;

    case BACH_DMA_READER2:
        nConfigValue = BachReadReg(BACH_REG_BANK1, BACH_DMA2_CTRL_8);
        return (nConfigValue & REG_RD_LOCALBUF_EMPTY) ? TRUE : FALSE;

    default:
        return FALSE;
    }
}


/*
BOOL BachDmaIntIsActive(BachDmaChannel_e eDmaChannel, BachDmaInterrupt_e eDmaInt)
{

  }
  */

BOOL BachDmaIsUnderrun(BachDmaChannel_e eDmaChannel)
{
    U16 nConfigValue;

    switch(eDmaChannel)
    {
    case BACH_DMA_WRITER1:
        //nConfigValue = ReadBank1Reg(BACH_DMA1_CTRL_8);
        nConfigValue = BachReadReg(BACH_REG_BANK1, BACH_DMA1_CTRL_8);
        return (nConfigValue & REG_WR_UNDERRUN_FLAG) ? TRUE : FALSE;

    case BACH_DMA_WRITER2:
        //nConfigValue = ReadBank1Reg(BACH_DMA2_CTRL_8);
        nConfigValue = BachReadReg(BACH_REG_BANK1, BACH_DMA2_CTRL_8);
        return (nConfigValue & REG_WR_UNDERRUN_FLAG) ? TRUE : FALSE;

    case BACH_DMA_READER1:
        //nConfigValue = ReadBank1Reg(BACH_DMA1_CTRL_8);
        nConfigValue = BachReadReg(BACH_REG_BANK1, BACH_DMA1_CTRL_8);
        return (nConfigValue & REG_RD_UNDERRUN_FLAG) ? TRUE : FALSE;

    case BACH_DMA_READER2:
        //nConfigValue = ReadBank1Reg(BACH_DMA2_CTRL_8);
        nConfigValue = BachReadReg(BACH_REG_BANK1, BACH_DMA2_CTRL_8);
        return (nConfigValue & REG_RD_UNDERRUN_FLAG) ? TRUE : FALSE;

    default:
        ERRMSG("BachDmaIsUnderrun - ERROR default case!\n");
        return FALSE;
    }

    return FALSE;
}

BOOL BachDmaIsOverrun(BachDmaChannel_e eDmaChannel)
{
    U16 nConfigValue;

    switch(eDmaChannel)
    {
    case BACH_DMA_WRITER1:
        //nConfigValue = ReadBank1Reg(BACH_DMA1_CTRL_8);
        nConfigValue = BachReadReg(BACH_REG_BANK1, BACH_DMA1_CTRL_8);
        return (nConfigValue & REG_WR_OVERRUN_FLAG) ? TRUE : FALSE;

    case BACH_DMA_WRITER2:
        //nConfigValue = ReadBank1Reg(BACH_DMA2_CTRL_8);
        nConfigValue = BachReadReg(BACH_REG_BANK1, BACH_DMA2_CTRL_8);
        return (nConfigValue & REG_WR_OVERRUN_FLAG) ? TRUE : FALSE;

    case BACH_DMA_READER1:
        //nConfigValue = ReadBank1Reg(BACH_DMA1_CTRL_8);
        nConfigValue = BachReadReg(BACH_REG_BANK1, BACH_DMA1_CTRL_8);
        return (nConfigValue & REG_RD_OVERRUN_FLAG) ? TRUE : FALSE;

    case BACH_DMA_READER2:
        //nConfigValue = ReadBank1Reg(BACH_DMA2_CTRL_8);
        nConfigValue = BachReadReg(BACH_REG_BANK1, BACH_DMA2_CTRL_8);
        return (nConfigValue & REG_RD_OVERRUN_FLAG) ? TRUE : FALSE;

    default:
        return FALSE;
    }

    return FALSE;
}

U32 BachDmaTrigLevelCnt(BachDmaChannel_e eDmaChannel, U32 nDataSize)
{
    U16 nConfigValue = 0;

    nConfigValue = (U16)((nDataSize / MIU_WORD_BYTE_SIZE) & REG_WR_SIZE_MSK);
    nDataSize = nConfigValue * MIU_WORD_BYTE_SIZE;

    if (nConfigValue > 0)
    {
        switch(eDmaChannel)
        {
        case BACH_DMA_WRITER1:

            /*WriteBank1Reg(BACH_DMA1_CTRL_12, nConfigValue);

            nConfigValue = ReadBank1Reg(BACH_DMA1_CTRL_9);
            if(nConfigValue & REG_WR_TRIG)
            {
                nConfigValue &= ~REG_WR_TRIG;
            }
            else
            {
                nConfigValue |= REG_WR_TRIG;
            }
            WriteBank1Reg(BACH_DMA1_CTRL_9, nConfigValue);*/
            BachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_12, 0xFFFF, nConfigValue);
            nConfigValue = BachReadReg(BACH_REG_BANK1, BACH_DMA1_CTRL_9);
            if(nConfigValue & REG_WR_TRIG)
            {
                BachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_9, REG_WR_TRIG, 0);
            }
            else
            {
                BachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_9, REG_WR_TRIG, REG_WR_TRIG);
            }

            break;

        case BACH_DMA_WRITER2:

            BachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_12, 0xFFFF, nConfigValue);
            nConfigValue = BachReadReg(BACH_REG_BANK1, BACH_DMA2_CTRL_9);
            if(nConfigValue & REG_WR_TRIG)
            {
                BachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_9, REG_WR_TRIG, 0);
            }
            else
            {
                BachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_9, REG_WR_TRIG, REG_WR_TRIG);
            }
            break;

        case BACH_DMA_READER1:

            /*WriteBank1Reg(BACH_DMA1_CTRL_4, nConfigValue);

            nConfigValue = ReadBank1Reg(BACH_DMA1_CTRL_1);
            if(nConfigValue & REG_RD_TRIG)
            {
                nConfigValue &= ~REG_RD_TRIG;
            }
            else
            {
                nConfigValue |= REG_RD_TRIG;
            }
            WriteBank1Reg(BACH_DMA1_CTRL_1, nConfigValue);*/
            BachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_4, 0xFFFF, nConfigValue);
            nConfigValue = BachReadReg(BACH_REG_BANK1, BACH_DMA1_CTRL_1);
            if(nConfigValue & REG_RD_TRIG)
            {
                BachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_1, REG_RD_TRIG, 0);
            }
            else
            {
                BachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_1, REG_RD_TRIG, REG_RD_TRIG);
            }
            break;

        case BACH_DMA_READER2:

            BachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_4, 0xFFFF, nConfigValue);
            nConfigValue = BachReadReg(BACH_REG_BANK1, BACH_DMA2_CTRL_1);
            if(nConfigValue & REG_RD_TRIG)
            {
                BachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_1, REG_RD_TRIG, 0);
            }
            else
            {
                BachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_1, REG_RD_TRIG, REG_RD_TRIG);
            }
            break;

        default:
            ERRMSG("BachDmaTrigLevelCnt - ERROR bank default case!\n");
            return 0;
        }

        return nDataSize;
    }

    return 0;

}

U32 BachRateToU32(BachRate_e eRate)
{
    switch(eRate)
    {
    case BACH_RATE_8K:
        return 8000;
    case BACH_RATE_16K:
        return 16000;
    case BACH_RATE_32K:
        return 32000;
    case BACH_RATE_44K:
        return 44100;
    case BACH_RATE_48K:
        return 48000;
    case BACH_RATE_96K:
        return 96000;
    default:
        return 0;
    }
}

BachRate_e BachRateFromU32(U32 nRate)
{
    switch(nRate)
    {
    case 8000:
        return BACH_RATE_8K;
    case 16000:
        return BACH_RATE_16K;
    case 32000:
        return BACH_RATE_32K;
    case 44100:
        return BACH_RATE_44K;
    case 48000:
        return BACH_RATE_48K;
    case 96000:
        return BACH_RATE_96K;
    default:
        return BACH_RATE_NULL;
    }
}

BachRate_e BachDmaGetRate(BachDmaChannel_e eDmaChannel)
{
    switch(eDmaChannel)
    {
    case BACH_DMA_WRITER1:
        return tSinkCfg.tDma1WriterCfg.eRate;
    case BACH_DMA_WRITER2:
        return tSinkCfg.tDma2WriterCfg.eRate;
    case BACH_DMA_READER1:
        return tSrcCfg.tDma1ReaderCfg.eRate;
    case BACH_DMA_READER2:
        return tSrcCfg.tDma2ReaderCfg.eRate;
    default:
        return BACH_RATE_NULL;
    }
}

BOOL BachDmaSetRate(BachDmaChannel_e eDmaChannel, BachRate_e eRate)
{
    switch(eRate)
    {
    case BACH_RATE_8K:
    case BACH_RATE_16K:
    case BACH_RATE_32K:
    case BACH_RATE_44K:
    case BACH_RATE_48K:
    case BACH_RATE_96K:
        break;
    default:
        return FALSE;
    }

    switch(eDmaChannel)
    {
    case BACH_DMA_WRITER1:
        tSinkCfg.tDma1WriterCfg.eRate = eRate;
        break;
    case BACH_DMA_WRITER2:
        tSinkCfg.tDma2WriterCfg.eRate = eRate;
        break;
    case BACH_DMA_READER1:
        tSrcCfg.tDma1ReaderCfg.eRate = eRate;
        break;
    case BACH_DMA_READER2:
        tSrcCfg.tDma2ReaderCfg.eRate = eRate;
        break;
    default:
        return FALSE;
    }

    return TRUE;
}

void BachDmaWriterSetMode(BachDmaChannel_e eDma, BOOL bMono)
{
    U16 nValue;
    if(bMono)
        nValue = REG_DMA1_WR_MODE | REG_DMA2_WR_MODE;
    else
        nValue = 0;
    switch(eDma)
    {
    case BACH_DMA_WRITER1:
        BachWriteReg(BACH_REG_BANK1, BACH_DMA_TEST_CTRL7, REG_DMA1_WR_MODE, nValue & REG_DMA1_WR_MODE);
        return;
    case BACH_DMA_WRITER2:
        BachWriteReg(BACH_REG_BANK1, BACH_DMA_TEST_CTRL7, REG_DMA2_WR_MODE, nValue & REG_DMA2_WR_MODE);
        return;
    default:
        ERRMSG("BachDmaWriterSetMode - Only have 2 DMA writer\n");
        return;
    }
}

BOOL BachAsrcIntSetMux(BachAsrcInt_e eInt, BachSource_e eSrc)
{
    BOOL bRet = FALSE;

    BachMux4_e mux4;
    BachMux2_e mux2 = BACH_MUX2_MUX;

    U8 val4 = g_nAsrcIntMuxRegValTbl[eInt][eSrc * 2];
    U8 val2 = g_nAsrcIntMuxRegValTbl[eInt][eSrc * 2 + 1];

    //TRACE2("BachAsrcIntSetMux BachAsrcInt_e = %d, BachSource_e = %d", eInt, eSrc);

    switch (eInt)
    {
    case BACH_ASRC_INT_ASRC1_1:
        mux4 = BACH_MUX4_MMP1_2_CH1;
        break;
    case BACH_ASRC_INT_ASRC1_2:
        mux4 = BACH_MUX4_MMP1_2_CH2;
        break;
    case BACH_ASRC_INT_ASRC2_1_2:
        mux4 = BACH_MUX4_ASRC2MMP1_2;
        break;
    default:
        ERRMSG("BachAsrcIntSetMux - ERROR BachAsrcInt_e default case!\n");
        return FALSE;
    }

    if (val4 != 0xff)
    {
        //TRACE2("BachAsrcIntSetMux mux4 = %d, val4 = %d", mux4, val4);
        BachSetMux4(mux4, val4);

        bRet = TRUE;
    }
    if (val2 != 0xff)
    {
        //TRACE2("BachAsrcIntSetMux mux2 = %d, val2 = %d", mux2, val2);
        BachSetMux2(mux2, val2);
        bRet = TRUE;
    }

    return bRet;
}

BachSynth_e BachAsrcGetUsedSynth(BachAsrc_e eAsrc)
{
    return m_tAsrcCfgTbl[eAsrc].eUsedSynth;
}

U32 BachAsrcGetSynthRate(BachAsrc_e eAsrc)
{
    /*switch(m_tAsrcCfgTbl[eAsrc].eNowRate)
    {
    case BACH_RATE_8K:
        return 8000;
    case BACH_RATE_16K:
        return 16000;
    case BACH_RATE_32K:
        return 32000;
    case BACH_RATE_48K:
        return 48000;
    default:
        return 0;
    }*/
    return  g_nAsrcRateMapTbl[eAsrc][m_tAsrcCfgTbl[eAsrc].eNowRate];
}

BOOL BachAsrcConfigSynth(BachAsrc_e eAsrc, BachRate_e eRate)
{
    BachSynth_e nFinalSynth = BACH_SYNTH_NULL, nFreferSynth = BACH_SYNTH_NULL;
    U8 nSynth = 0;
    U8 nAsrc = 0;
    BachAsrcCfg_e tmpTbl[sizeof(BachAsrcCfg_e)];
    U8 tmpAsrc;
    BOOL result = TRUE;
    //TRACE2("BachAsrcConfigSynth BachAsrc_e = %d, BachRate_e = %d", eAsrc, eRate);

    if (BACH_RATE_NULL == eRate)
    {
        m_tAsrcCfgTbl[eAsrc].eNowRate   = BACH_RATE_NULL;
        m_tAsrcCfgTbl[eAsrc].eUsedSynth = BACH_SYNTH_NULL;
        //m_tAsrcCfgTbl[eAsrc].eSyncAsrc  = BACH_SYNTH_NULL;
        return TRUE;
    }
    else if (BACH_RATE_SLAVE == eRate)
    {
        if (g_nAsrcSynthAvailMatrix[eAsrc][BACH_FS_SYNTH] != 0xff)
        {
          /*
            result &= BachSetAsrcSynth(eAsrc, BACH_FS_SYNTH);
            m_tAsrcCfgTbl[eAsrc].eNowRate   = BACH_RATE_SLAVE;
            m_tAsrcCfgTbl[eAsrc].eUsedSynth = BACH_FS_SYNTH;
          */
            BachSetAsrcConfig(eAsrc, BACH_FS_SYNTH, BACH_RATE_SLAVE);
            return TRUE;
        }
        else
            return FALSE;
    }
    else
    {
        nFreferSynth = m_tAsrcCfgTbl[eAsrc].ePreferSynth;

        if (BACH_SYNTH_NULL != nFreferSynth && g_nAsrcSynthAvailMatrix[eAsrc][nFreferSynth] != 0xff)
        {
                for (nAsrc = 0; nAsrc < BACH_ASRC_NUM - 1; nAsrc++)
                {
                    if ((nAsrc != eAsrc) && (nFreferSynth == m_tAsrcCfgTbl[nAsrc].eUsedSynth))
                      break;
                }

                if ((BACH_ASRC_NUM - 1) == nAsrc)
                {
                    BachSetAsrcConfig(eAsrc, nFreferSynth, eRate);
                    return TRUE;
                }
        }

        for (nSynth = 1; nSynth < BACH_SYNTH_NUM - 1; nSynth++)
        {
            if (g_nAsrcSynthAvailMatrix[eAsrc][nSynth] != 0xff)
            {
                for (nAsrc = 0; nAsrc < BACH_ASRC_NUM - 1; nAsrc++)
                {
                    if ((nAsrc != eAsrc) && (nSynth == m_tAsrcCfgTbl[nAsrc].eUsedSynth))
                    {
                        if (g_nAsrcRateMapTbl[nAsrc][m_tAsrcCfgTbl[nAsrc].eNowRate] == g_nAsrcRateMapTbl[eAsrc][eRate])
                        {
                            m_tAsrcCfgTbl[eAsrc].eNowRate = eRate;
                            m_tAsrcCfgTbl[eAsrc].eUsedSynth = (BachSynth_e)nSynth;
                            //m_tAsrcCfgTbl[eAsrc].eSyncAsrc= nAsrc;
                            //TRACE2("BachAsrcConfigSynth Asrc = %d, Synth = %d", eAsrc, nSynth);

                            result &= BachSetAsrcSynth(eAsrc, (BachSynth_e)nSynth);

                            //TRACE2("BachAsrcConfigSynth Sync enable to Asrc = %d, Rate = %d", nAsrc, eRate);

                            return result;
                        }
                        else
                            break;
                    }
                }

                if ((BACH_ASRC_NUM - 1) == nAsrc)
                    nFinalSynth = (BachSynth_e)nSynth;

            }
        }

        if (nFinalSynth != BACH_SYNTH_NULL)
        {
            BachSetAsrcConfig(eAsrc, nFinalSynth, eRate);
            return TRUE;
        }
        else
        {
            #if 1
            //int nAsrc;
            ERRMSG("BachAsrcConfigSynth BachAsrc_e = %d, BachRate_e = %d", eAsrc, eRate);
            for (nAsrc = 0; nAsrc < BACH_ASRC_NUM - 1; nAsrc++)
                ERRMSG("m_tAsrcCfgTbl[%d] Rate = %d, Synth = %d\n", nAsrc, m_tAsrcCfgTbl[nAsrc].eNowRate, m_tAsrcCfgTbl[nAsrc].eUsedSynth);
            #endif

            //TRACE("BachAsrcConfigSynth  FORCE SET ASRC");
            memcpy(tmpTbl, m_tAsrcCfgTbl, sizeof(m_tAsrcCfgTbl));
            BachSetAsrcConfig(eAsrc, m_tAsrcCfgTbl[eAsrc].eUsedSynth, eRate);

            //check contradiction
            for (nAsrc = 0; nAsrc < BACH_ASRC_NUM - 1; nAsrc++)
            {
                //TRACE1("Check Contradiction %d",nAsrc);
                if ((nAsrc != eAsrc))
                {
                    if((m_tAsrcCfgTbl[nAsrc].eUsedSynth == m_tAsrcCfgTbl[eAsrc].eUsedSynth) && (g_nAsrcRateMapTbl[nAsrc][m_tAsrcCfgTbl[nAsrc].eNowRate] != g_nAsrcRateMapTbl[eAsrc][m_tAsrcCfgTbl[eAsrc].eNowRate]))
                    {
                        if(!BachChangeAsrcSynth((BachAsrc_e)nAsrc))
                        {
                            BachResetAsrcByTable(tmpTbl);
                            return FALSE;
                        }
                    }
                    else
                    {
                        for(tmpAsrc = 0; tmpAsrc < nAsrc; tmpAsrc++)
                        {
                            if((m_tAsrcCfgTbl[tmpAsrc].eUsedSynth == m_tAsrcCfgTbl[nAsrc].eUsedSynth) && (g_nAsrcRateMapTbl[tmpAsrc][m_tAsrcCfgTbl[tmpAsrc].eNowRate] != g_nAsrcRateMapTbl[nAsrc][m_tAsrcCfgTbl[nAsrc].eNowRate]))
                            {
                                if(! BachChangeAsrcSynth((BachAsrc_e)nAsrc))
                                {
                                    BachResetAsrcByTable(tmpTbl);
                                    return FALSE;
                                }
                            }
                        }
                    }
                }
            }
            //TRACE("BachAsrcConfigSynth  FORCE SET ASRC DONE");
            return TRUE;
        }


    }
}

void BachResetAsrcByTable(BachAsrcCfg_e eTable[])
{
    //TRACE("BachResetAsrcByTable");
    U8 nAsrc;
    for(nAsrc = 0 ; nAsrc < BACH_ASRC_NUM - 1 ; nAsrc++)
        BachSetAsrcConfig((BachAsrc_e)nAsrc, eTable[nAsrc].eUsedSynth, eTable[nAsrc].eNowRate);
}

void BachSetAsrcConfig(BachAsrc_e eAsrc, BachSynth_e eSynth, BachRate_e eRate)
{
    m_tAsrcCfgTbl[eAsrc].eNowRate = eRate;
    m_tAsrcCfgTbl[eAsrc].eUsedSynth = eSynth;
    //m_tAsrcCfgTbl[eAsrc].eSyncAsrc= 0xff;

    //TRACE2("BachSetAsrcConfigForce Asrc = %d, Final Synth = %d", eAsrc, eSynth);
    if(eSynth == BACH_SYNTH_NULL)
        return;

    if(!BachSetAsrcSynth(eAsrc, eSynth))
        ERRMSG("BachSetAsrcConfig - SetAsrcSynth Fail!!\n");


    if(eSynth == BACH_FS_SYNTH || eRate == BACH_RATE_SLAVE || eRate == BACH_RATE_NULL)
        return;
    //TRACE2("BachSetAsrcConfigForce Setup Synth Rate = %d(0x%x)", eRate, g_nAsrcRateMapTbl[eAsrc][eRate]);
    BachSetSynthRate(eSynth, g_nAsrcRateMapTbl[eAsrc][eRate]);
}

BOOL BachChangeAsrcSynth(BachAsrc_e eAsrc)
{

    U8 nAsrc;
    U8 nSynth;
    BachSynth_e nFinalSynth = BACH_SYNTH_NULL;
    BOOL result;
    for (nSynth = 1; nSynth < BACH_SYNTH_NUM - 1; nSynth++)
    {
        if ((g_nAsrcSynthAvailMatrix[eAsrc][nSynth] != 0xff) && (nSynth != m_tAsrcCfgTbl[eAsrc].eUsedSynth))
        {
            for (nAsrc = 0; nAsrc < eAsrc; nAsrc++)
            {
                if ((nSynth == m_tAsrcCfgTbl[nAsrc].eUsedSynth))
                {
                    if (g_nAsrcRateMapTbl[nAsrc][m_tAsrcCfgTbl[nAsrc].eNowRate] == g_nAsrcRateMapTbl[eAsrc][m_tAsrcCfgTbl[eAsrc].eNowRate])
                    {
                        //m_tAsrcCfgTbl[eAsrc].eNowRate= m_tAsrcCfgTbl[nAsrc].eNowRate;
                        m_tAsrcCfgTbl[eAsrc].eUsedSynth = (BachSynth_e)nSynth;
                        //m_tAsrcCfgTbl[eAsrc].eSyncAsrc= nAsrc;

                        //TRACE2("BachAsrcConfigSynth Asrc = %d, Synth = %d", eAsrc, nSynth);

                        result = BachSetAsrcSynth(eAsrc, (BachSynth_e)nSynth);
                        //		TRACE2("BachAsrcConfigSynth Sync enable to Asrc = %d, Rate = %d", nAsrc, eRate);

                        return result;
                    }
                    else
                        break;
                }
            }

            if (eAsrc == nAsrc)
                nFinalSynth = (BachSynth_e)nSynth;
        }
    }

    if (nFinalSynth != BACH_SYNTH_NULL)
    {
        BachSetAsrcConfig(eAsrc, nFinalSynth, m_tAsrcCfgTbl[eAsrc].eNowRate);
        return TRUE;
    }
    else
        return FALSE;
}

BOOL BachChannelSelectSrc(BachChannel_e eChan, BachSource_e eSrc)
{
    BachSource_e eLSrc, eRSrc;
    BachRate_e  eLRate, eRRate, eRate;
    BachDmaChannel_e eDma = BACH_DMA_NULL;
    U8 nValidSel = 0;
    BOOL result = TRUE;
    BOOL sync = FALSE;
    eRate = eLRate = eRRate = BACH_RATE_NULL;
    //TRACE2("BachChannelSelectSrc eChan = %d, eSrc = %d", eChan, eSrc);


    switch (eSrc)
    {
    case BACH_SRC_DMA1_READER:
        nValidSel = 0;
        sync = TRUE;
        eDma = BACH_DMA_READER1;
        eRSrc = eLSrc = eSrc;
        eLRate = tSrcCfg.tDma1ReaderCfg.eRate;
        eRate = eRRate = eLRate;
        break;
    case BACH_SRC_DMA2_READER:
        eRSrc = eLSrc = eSrc;
        eRate = tSrcCfg.tDma2ReaderCfg.eRate;
        break;
    case BACH_SRC_EXT_I2S:
        sync = TRUE;
        eRSrc = eLSrc = eSrc;
        eLRate = tSrcCfg.tExtI2sCfg.eRate;
        eRate = eRRate = eLRate;
        break;
    case BACH_SRC_SPDIF:
        sync = TRUE;
        eRSrc = eLSrc = eSrc;
        eLRate = tSrcCfg.tSpidfCfg.eRate;
        eRate = eRRate = eLRate;
        break;
    case BACH_SRC_BT_I2S:
        nValidSel = 1;
        eDma = BACH_DMA_READER1;

        eLSrc = eSrc;
        eRSrc = BACH_SRC_DMA1_READER;
        eLRate = tSrcCfg.tBtI2sCfg.eRate;
        eRRate = tSrcCfg.tDma1ReaderCfg.eRate;
        BachSetMixer(BACH_MIXER_DEC1_2, 2, 2);
        break;
    case BACH_SRC_DBB_I2S:
        eLSrc = BACH_SRC_NULL;
        eRSrc = eSrc;
        //	eLRate = BACH_RATE_NULL;
        eRRate = tSrcCfg.tDbbI2sCfg.eRate;
        BachSetMixer(BACH_MIXER_ST_MIX, 2, 0);
        BachSetMixer(BACH_MIXER_AV_MIX, 1, 0);
        break;
    case BACH_SRC_DBB_AEC_I2S:
        nValidSel = 0;
        eDma = BACH_DMA_READER1;

        eLSrc = BACH_SRC_DMA1_READER;
        eRSrc = eSrc;
        eLRate = tSrcCfg.tDma1ReaderCfg.eRate;
        eRRate = tSrcCfg.tDbbI2sCfg.eRate;
        BachSetMixer(BACH_MIXER_ST_MIX, 2, 0);
        BachSetMixer(BACH_MIXER_AV_MIX, 1, 0);
        break;
    case BACH_SRC_AUX:
        sync = TRUE;
        eRSrc = eLSrc = eSrc;
        if (BachGetMux4(BACH_MUX4_ASRC2DEC1_2) < 3)
        {
            //ERRMSG("BachChannelSelectSrc - Aux Input is Line-in!\n");
            eLRate = tSrcCfg.tAuxCfg.eRate;
        }
        else
        {
            //ERRMSG("BachChannelSelectSrc - Aux Input is Mixer!\n");
            eLRate = tSinkCfg.tDma1WriterCfg.eRate;
        }
        eRRate = eLRate;
        break;
    default:
        ERRMSG("BachChannelSelectSrc - ERROR BachChannel_e default case!\n");
        return FALSE;

    }

    switch (eChan)
    {
    case BACH_CHAN_ASRC1:
        if((eLRate == BACH_RATE_NULL) && (eRRate == BACH_RATE_NULL))
            break;
        result &= BachAsrcIntSetMux(BACH_ASRC_INT_ASRC1_1, eLSrc);
        result &= BachAsrcIntSetMux(BACH_ASRC_INT_ASRC1_2, eRSrc);

        result &= BachAsrcConfigSynth(BACH_ASRC_INT1, BACH_RATE_NULL);
        result &= BachAsrcConfigSynth(BACH_ASRC_INT2, BACH_RATE_NULL);
        result &= BachAsrcConfigSynth(BACH_ASRC_INT1, eLRate);
        result &= BachAsrcConfigSynth(BACH_ASRC_INT2, eRRate);

        if (eSrc == BACH_SRC_BT_I2S && eLRate == BACH_RATE_SLAVE)
            BachWriteReg(BACH_REG_BANK1, BACH_FS_SYNTH_CTRL0, REG_FS_SYNTH_SOURCE_SEL, 0);
        else
            BachWriteReg(BACH_REG_BANK1, BACH_FS_SYNTH_CTRL0, REG_FS_SYNTH_SOURCE_SEL, REG_FS_SYNTH_SOURCE_SEL);

        if (eDma != BACH_DMA_NULL)
            BachSetDmaValidSel(eDma, nValidSel);

        if(sync)
        {
            BachWriteReg(BACH_REG_BANK1, BACH_AUDIOBAND_CTRL0, REG_CH12_SYNC_ENABLE, REG_CH12_SYNC_ENABLE);
            BachWriteReg(BACH_REG_BANK1, BACH_TIMING_CTRL1, REG_CH2_INT_CNT_SYNC_SEL, REG_CH2_INT_CNT_SYNC_SEL);
        }
        else
        {
            BachWriteReg(BACH_REG_BANK1, BACH_AUDIOBAND_CTRL0, REG_CH12_SYNC_ENABLE, 0);
            BachWriteReg(BACH_REG_BANK1, BACH_TIMING_CTRL1, REG_CH2_INT_CNT_SYNC_SEL, 0);
        }

#if 0
        int nAsrc;
        for (nAsrc = 0; nAsrc < BACH_ASRC_NUM - 1; nAsrc++)
            ERRMSG("m_tAsrcCfgTbl[%d] Rate = %d, Synth = %d\n", nAsrc, m_tAsrcCfgTbl[nAsrc].eNowRate, m_tAsrcCfgTbl[nAsrc].eUsedSynth);
#endif

        return result;

    case BACH_CHAN_ASRC2:
        if(eRate == BACH_RATE_NULL)
            break;
        result &= BachAsrcIntSetMux(BACH_ASRC_INT_ASRC2_1_2, eSrc);
        result &= BachAsrcConfigSynth(BACH_ASRC_ASRC2_INT1_2, eRate);

        if (eDma != BACH_DMA_NULL)
            BachSetDmaValidSel(eDma, 2);
        return result;

    case BACH_CHAN_MICIN:
    case BACH_CHAN_LINEIN:
    default:
        ERRMSG("BachChannelSelectSrc - ERROR BachSource_e default case!\n");
        break;
    }

    return FALSE;
}


BOOL BachSinkSelectChannel(BachSink_e eSink, BachChannel_e eChan)
{

    BachAsrc_e  eAsrc;
    BachRate_e  eRate;

    BachMux4_e eMux4;
    BachMixer_e eMixer;

    //TRACE2("BachSinkConfigChannel eSink = %d, eChan = %d", eSink, eChan);

    switch (eSink)
    {
    case BACH_SINK_DMA1_WRITER:
        eAsrc = BACH_ASRC_ASRC2_DEC1_2;
        eMux4 = BACH_MUX4_ASRC2DEC1_2;
        if (eChan == BACH_CHAN_MICIN)
            eRate = tSrcCfg.tAdcCfg.eRate;
        else if (eChan == BACH_CHAN_LINEIN)
            eRate = tSrcCfg.tAuxCfg.eRate;
        else
            eRate = tSinkCfg.tDma1WriterCfg.eRate;
        eMixer = BACH_MIXER_ASRC2_DEC1_2;
        BachSetMux4(BACH_MUX4_DMA1_WRITER, 3);
        BachSetDmaValidSel(BACH_DMA_WRITER1, 1);
        break;
    case BACH_SINK_DMA2_WRITER:
        eAsrc = BACH_ASRC_DEC1_2;
        eMux4 = BACH_MUX4_DEC1_2;
        if (eChan == BACH_CHAN_MICIN)
            eRate = tSrcCfg.tAdcCfg.eRate;
        else if (eChan == BACH_CHAN_LINEIN)
            eRate = tSrcCfg.tAuxCfg.eRate;
        else
            eRate = tSinkCfg.tDma2WriterCfg.eRate;
        eMixer = BACH_MIXER_DEC1_2;
        BachSetMux4(BACH_MUX4_DMA2_WRITER, 0);
        BachSetDmaValidSel(BACH_DMA_WRITER2, 0);
        break;
    case BACH_SINK_CODEC_I2S:
        eAsrc = BACH_ASRC_DEC3_4;
        eMux4 = BACH_MUX4_DEC3_4;
        eRate = tSinkCfg.tCodecI2sCfg.eRate;
        eMixer = BACH_MIXER_DEC3_4;
        BachSetMux2(BACH_MUX2_CODEC_I2S_TX, 1);
        break;
    case BACH_SINK_SPDIF_HDMI:
        eAsrc = BACH_ASRC_ASRC2_DEC3_4;
        eMux4 = BACH_MUX4_ASRC2DEC3_4;
        eRate = tSinkCfg.tSpidfHdmiCfg.eRate;
        eMixer = BACH_MIXER_ASRC2_DEC3_4;
        BachSetMux2(BACH_MUX2_SPDIF_HDMI_TX, 0);
        BachSetSynthRate(BACH_NF_SYNTH_7, g_nAsrcRateMapTbl[BACH_ASRC_SPIDF_HDMI][eRate]);
        break;
    default:
        ERRMSG("BachSinkSelectChannel - ERROR BachSink_e default case!\n");
        return FALSE;

    }

    switch (eChan)
    {
    case BACH_CHAN_ASRC1:
        BachSetMux4(eMux4, 3);
        BachAsrcConfigSynth(eAsrc, eRate);

        if (eMixer == BACH_MIXER_DEC1_2 || eMixer == BACH_MIXER_DEC3_4)
            BachSetMixer(eMixer, 2, 2);
        else
            BachSetMixer(eMixer, 3, 3);
        break;
    case BACH_CHAN_ASRC2:
        BachSetMux4(eMux4, 3);
        BachAsrcConfigSynth(eAsrc, eRate);

        if (eMixer == BACH_MIXER_DEC1_2 || eMixer == BACH_MIXER_DEC3_4)
            BachSetMixer(eMixer, 3, 3);
        else
            BachSetMixer(eMixer, 2, 2);
        break;
    case BACH_CHAN_MIX:
        BachSetMux4(eMux4, 3);
        BachAsrcConfigSynth(eAsrc, eRate);
        BachSetMixer(eMixer, 1, 1);

        break;
    case BACH_CHAN_MICIN:
    case BACH_CHAN_LINEIN:
        if (BACH_RATE_8K == eRate)
            BachSetMux4(eMux4, 0);
        else if (BACH_RATE_16K == eRate)
            BachSetMux4(eMux4, 1);
        else if (BACH_RATE_32K == eRate)
            BachSetMux4(eMux4, 2);
        else
        {
            ERRMSG("BachSinkSelectChannel - mic/line-in only support 32k, 16k and 8k!\n");
            BachSetMux4(eMux4, 0);	//$$$ may need to re-assign
        }
        BachAsrcConfigSynth(eAsrc, BACH_RATE_NULL);
        break;
    default:
        ERRMSG("BachSinkSelectChannel - ERROR BachChannel_e default case!\n");
        return FALSE;

    }

#if 0
    int nAsrc;
    for (nAsrc = 0; nAsrc < BACH_ASRC_NUM - 1; nAsrc++)
        ERRMSG("m_tAsrcCfgTbl[%d] Rate = %d, Synth = %d\n", nAsrc, m_tAsrcCfgTbl[nAsrc].eNowRate, m_tAsrcCfgTbl[nAsrc].eUsedSynth);
#endif

    return TRUE;
}

BOOL BachDacSetSrc(BachDac_e eDac, BachDacSrc_e eSrc)
{
    if((eDac < 0 || eDac > 1) || (eSrc < 0 || eSrc > 2))
        return FALSE;

    if (BACH_DAC1 == eDac)
    {
        switch(eSrc)
        {
        case BACH_DAC_MIX:
            BachSetMixer(BACH_MIXER_SDM_DWA1, 1, 1);
            break;
        case BACH_DAC_ASRC1:
            BachSetMixer(BACH_MIXER_SDM_DWA1, 2, 2);
            break;
        case BACH_DAC_ASRC2:
            BachSetMixer(BACH_MIXER_SDM_DWA1, 3, 3);
            break;
        default:
            ERRMSG("BachDacSetSrc - ERROR BachDacSrc_e default case!\n");
            return FALSE;
        }
    }
    else if (BACH_DAC2 == eDac)
    {
        switch(eSrc)
        {
        case BACH_DAC_MIX:
            BachSetMixer(BACH_MIXER_SDM_DWA2, 1, 1);
            break;
        case BACH_DAC_ASRC1:
            BachSetMixer(BACH_MIXER_SDM_DWA2, 3, 3);
            break;
        case BACH_DAC_ASRC2:
            BachSetMixer(BACH_MIXER_SDM_DWA2, 2, 2);
            break;
        default:
            ERRMSG("BachDacSetSrc - ERROR BachDacSrc_e default case!\n");
            return FALSE;
        }
    }
    m_nOutMix[eDac] = eSrc;
    return TRUE;
}

U16 BachDacGetSrc(BachDac_e eDac)
{
    return m_nOutMix[eDac];
}


BOOL BachDacSelectChannel(BachDac_e eDac,  BachChannelMode_e eChannel)
{
    if (BACH_DAC1 == eDac)
    {
        BachSelectDAC1Input(eChannel);
        switch(m_nOutMix[0])
        {
        case BACH_DAC_MIX:
            BachSetMixer(BACH_MIXER_SDM_DWA1, 1, 1);
            break;
        case BACH_DAC_ASRC1:
            BachSetMixer(BACH_MIXER_SDM_DWA1, 2, 2);
            break;
        case BACH_DAC_ASRC2:
            BachSetMixer(BACH_MIXER_SDM_DWA1, 3, 3);
            break;
        default:
            ERRMSG("BachDacSelectChannel - ERROR BachChannel_e default case!\n");
            return FALSE;
        }
    }
    else if (BACH_DAC2 == eDac)
    {
        BachSelectDAC2Input(eChannel);
        switch(m_nOutMix[1])
        {
        case BACH_DAC_MIX:
            BachSetMixer(BACH_MIXER_SDM_DWA2, 1, 1);
            break;
        case BACH_DAC_ASRC1:
            BachSetMixer(BACH_MIXER_SDM_DWA2, 3, 3);
            break;
        case BACH_DAC_ASRC2:
            BachSetMixer(BACH_MIXER_SDM_DWA2, 2, 2);
            break;
        default:
            ERRMSG("BachDacSelectChannel - ERROR BachChannel_e default case!\n");
            return FALSE;
        }
    }
    else
    {
        ERRMSG("BachDacSelectChannel - ERROR BachDac_e default case!\n");
        return FALSE;
    }

    return TRUE;

}
/*void BachFuncInit(BachFunction_e eFunc)
{
	BachSinkCfg_t tSinkCfg;
	BachSinkCfg_t tSrcCfg;

	switch(eFunc)
	{
		case BACH_FUNC_MM://input 48K,output 48K & line-out
			ChannelSelectSrc(BACH_CHAN_ASRC1, BACH_SRC_DMA1_READER);

			SinkSelectChannel(BACH_SINK_CODEC_I2S, BACH_CHAN_ASRC1);
			SinkSelectChannel(BACH_SINK_HDMI, BACH_CHAN_ASRC1);
			SinkSelectChannel(BACH_SINK_SPIDF, BACH_CHAN_ASRC1);
			DacSelectChannel(BACH_DAC1, BACH_CHAN_ASRC1, BACH_DAC_TYPE_BOTH);	//speaker1
			break;
		case BACH_FUNC_GPS://input 48K,output line-out
			ChannelSelectSrc(BACH_CHAN_ASRC2, BACH_SRC_DMA2_READER);

			DacSelectChannel(BACH_DAC2, BACH_CHAN_ASRC2, BACH_DAC_TYPE_BOTH);	//speaker2
			break;
		case BACH_FUNC_BT_I2S://input eInRate,output 48K & line-out
			ChannelSelectSrc(BACH_CHAN_ASRC1, BACH_SRC_BT_I2S);

			SinkSelectChannel(BACH_SINK_DMA1_WRITER, BACH_CHAN_ASRC1);
			SinkSelectChannel(BACH_SINK_DMA2_WRITER, BACH_CHAN_MICIN);

			SinkSelectChannel(BACH_SINK_CODEC_I2S, BACH_CHAN_ASRC1);
			SinkSelectChannel(BACH_SINK_HDMI, BACH_CHAN_ASRC1);
			SinkSelectChannel(BACH_SINK_SPIDF, BACH_CHAN_ASRC1);

			DacSelectChannel(BACH_DAC1, BACH_CHAN_ASRC1, BACH_DAC_TYPE_LEFT);

			break;
		case BACH_FUNC_DBB:
			ChannelSelectSrc(BACH_CHAN_ASRC1, BACH_SRC_DBB_I2S);

			//SinkSelectChannel(BACH_SINK_DMA1_WRITER, BACH_CHAN_ASRC1);
			//SinkSelectChannel(BACH_SINK_DMA2_WRITER, BACH_CHAN_MICIN);
			SinkSelectChannel(BACH_SINK_CODEC_I2S, BACH_CHAN_ASRC1);
			SinkSelectChannel(BACH_SINK_HDMI, BACH_CHAN_ASRC1);
			SinkSelectChannel(BACH_SINK_SPIDF, BACH_CHAN_ASRC1);

			DacSelectChannel(BACH_DAC1, BACH_CHAN_ASRC1, BACH_DAC_TYPE_RIGHT);

			break;
		case BACH_FUNC_FM:
			ChannelSelectSrc(BACH_CHAN_ASRC1, BACH_SRC_AUX);

			tSinkCfg.tDmaCfg.eRate = BACH_RATE_32K;
			SinkSetCfg(BACH_SINK_DMA1_WRITER, tSinkCfg);
			SinkSelectChannel(BACH_SINK_DMA1_WRITER, BACH_CHAN_LINEIN);

			//user define
			SinkSelectChannel(BACH_SINK_CODEC_I2S, BACH_CHAN_ASRC1);
			SinkSelectChannel(BACH_SINK_HDMI, BACH_CHAN_ASRC1);
			SinkSelectChannel(BACH_SINK_SPIDF, BACH_CHAN_ASRC1);

			if (tSrcCfg.tAuxCfg.bIsStereo)
				DacSelectChannel(BACH_DAC1, BACH_CHAN_ASRC1, BACH_DAC_TYPE_BOTH);
			else
				DacSelectChannel(BACH_DAC1, BACH_CHAN_ASRC1, BACH_DAC_TYPE_LEFT);

			break;
		case BACH_FUNC_RECORDER:
			SinkSelectChannel(BACH_SINK_DMA2_WRITER, BACH_CHAN_MICIN);
			break;

	}
}*/

BOOL BachAdcSetRate(BachRate_e eRate)
{

    switch(eRate)
    {
    case BACH_RATE_8K:
    case BACH_RATE_16K:
    case BACH_RATE_32K:
        break;
    default:
        return FALSE;
    }

    tSrcCfg.tAdcCfg.eRate = eRate;

    return TRUE;
}

BachRate_e BachAdcGetRate()
{
    return tSrcCfg.tAdcCfg.eRate;
}

BOOL BachAuxSetCfg(BachRate_e eRate, BOOL bStereo)
{
    switch(eRate)
    {
    case BACH_RATE_8K:
    case BACH_RATE_16K:
    case BACH_RATE_32K:
        break;
    default:
        return FALSE;
    }

    tSrcCfg.tAuxCfg.eRate = eRate;
    tSrcCfg.tAuxCfg.bIsStereo = bStereo;

    return TRUE;
}

BachRate_e BachAuxGetRate()
{
    return tSrcCfg.tAuxCfg.eRate;
}

BOOL BachBtSetCfg(BachI2sFmt_e eFormat, BachI2sWidth_e eWidth, BachRate_e eRate)
{
    switch(eRate)
    {
    case BACH_RATE_SLAVE:
    case BACH_RATE_8K:
    case BACH_RATE_16K:
    case BACH_RATE_32K:
        break;
    default:
        return FALSE;
    }

    tSrcCfg.tBtI2sCfg.eFormat = eFormat;
    tSrcCfg.tBtI2sCfg.eWidth = eWidth;
    tSrcCfg.tBtI2sCfg.eRate = eRate;
    return BachI2sConfig(BACH_I2S_BT_TRX, tSrcCfg.tBtI2sCfg);
}

BOOL BachBtSetRate(BachRate_e eRate)
{
    switch(eRate)
    {
    case BACH_RATE_SLAVE:
    case BACH_RATE_8K:
    case BACH_RATE_16K:
    case BACH_RATE_32K:
    case BACH_RATE_48K:
        break;
    default:
        return FALSE;
    }

    tSrcCfg.tBtI2sCfg.eRate = eRate;
    return BachI2sConfig(BACH_I2S_BT_TRX, tSrcCfg.tBtI2sCfg);
}

BachRate_e BachBtGetRate()
{
    return tSrcCfg.tBtI2sCfg.eRate;
}

void BachBtSetWidth(BachI2sWidth_e eWidth)
{
    tSrcCfg.tBtI2sCfg.eWidth = eWidth;
    BachI2sConfig(BACH_I2S_BT_TRX, tSrcCfg.tBtI2sCfg);
}

void BachBtSetChip(BachBtChip_e eChip)
{
    m_BtChip = eChip;
}

void BachBtPadEnable(BOOL bEn)
{
//    ERRMSG("######BachBtPadEnable = %d, rate = %d, chip = %d\n", bEn, tSrcCfg.tBtI2sCfg.eRate, m_BtChip);

    if (bEn)
    {
        switch(m_BtChip)
        {
        case BACH_BT_MXK:
            BachI2sPadMux(BACH_I2S_PAD_BT, BACH_I2S_OUTPAD_BT, (tSrcCfg.tBtI2sCfg.eRate? TRUE: FALSE));

            BachWriteReg(BACH_REG_BANK1, BACH_BT_I2S_CTRL0, REG_PAD_I2S_TRX_BCK_INV, REG_PAD_I2S_TRX_BCK_INV); //match rising edge
            BachWriteReg(BACH_REG_BANK1, BACH_BT_I2S_CTRL1, REG_BT_I2S_SOURCE_SEL, REG_BT_I2S_SOURCE_SEL);
            break;
        case BACH_BT_EXT:
            BachI2sPadMux(BACH_I2S_PAD_BT, BACH_I2S_OUTPAD_BT, (tSrcCfg.tBtI2sCfg.eRate? TRUE: FALSE));

            BachWriteReg(BACH_REG_BANK1, BACH_BT_I2S_CTRL0, REG_PAD_I2S_TRX_BCK_INV, 0);
            BachWriteReg(BACH_REG_BANK1, BACH_BT_I2S_CTRL1, REG_BT_I2S_SOURCE_SEL, REG_BT_I2S_SOURCE_SEL);
            break;
        case BACH_BT_MSTAR:
            BachWriteReg(BACH_REG_BANK1, BACH_BT_I2S_CTRL0, REG_PAD_I2S_TRX_BCK_INV, 0);
            BachWriteReg(BACH_REG_BANK1, BACH_BT_I2S_CTRL1, REG_BT_I2S_SOURCE_SEL, 0);
            break;
        default:
            break;
        }
    }
    else
    {
        if(m_BoardRevision == E_BD_MST154A_D01A_S)
            BachI2sPadMux(BACH_I2S_PAD_CODEC, BACH_I2S_OUTPAD_BT, TRUE);
    }
}

void BachCfgInit(void)
{
    BOOL result = TRUE;
    m_BtChip = BACH_BT_MXK;
    tSrcCfg.tBtI2sCfg.eRate = BACH_RATE_8K;
    tSrcCfg.tBtI2sCfg.eWidth = BACH_I2S_WIDTH_16;
    tSrcCfg.tBtI2sCfg.eMode = BACH_PCM_MODE_I2S;
    tSrcCfg.tBtI2sCfg.eFormat = BACH_I2S_FMT_LEFT_JUSTIFY;
    result &= BachI2sConfig(BACH_I2S_BT_TRX, tSrcCfg.tBtI2sCfg);

    tSrcCfg.tDbbI2sCfg.eRate = BACH_RATE_8K;
    tSrcCfg.tDbbI2sCfg.eWidth = BACH_I2S_WIDTH_16;
    tSrcCfg.tDbbI2sCfg.eMode = BACH_PCM_MODE_I2S;
    tSrcCfg.tDbbI2sCfg.eFormat = BACH_I2S_FMT_LEFT_JUSTIFY;
    result &= BachI2sConfig(BACH_I2S_DBB_TRX, tSrcCfg.tDbbI2sCfg);

    tSrcCfg.tAdcCfg.eRate = BACH_RATE_8K;

    tSrcCfg.tAuxCfg.eRate = BACH_RATE_32K;
    tSrcCfg.tAuxCfg.bIsStereo = TRUE;

    tSrcCfg.tDma1ReaderCfg.eRate = BACH_RATE_48K;
    tSrcCfg.tDma2ReaderCfg.eRate = BACH_RATE_48K;

    tSinkCfg.tCodecI2sCfg.eRate = BACH_RATE_48K;
    tSinkCfg.tCodecI2sCfg.eWidth = BACH_I2S_WIDTH_16;
    tSinkCfg.tCodecI2sCfg.eMode = BACH_PCM_MODE_I2S;
    tSinkCfg.tCodecI2sCfg.eFormat = BACH_I2S_FMT_LEFT_JUSTIFY;
    result &= BachI2sConfig(BACH_I2S_CODEC_TX, tSinkCfg.tCodecI2sCfg);

    tSinkCfg.tSpidfHdmiCfg.eRate = BACH_RATE_48K;

    tSinkCfg.tDma1WriterCfg.eRate = BACH_RATE_8K;
    tSinkCfg.tDma2WriterCfg.eRate = BACH_RATE_8K;

    m_tAsrcCfgTbl[0].eNowRate = BACH_RATE_48K;  //sync to m_tAsrcCfgTbl
    m_tAsrcCfgTbl[1].eNowRate = BACH_RATE_48K;
    m_tAsrcCfgTbl[2].eNowRate = BACH_RATE_48K;
    m_tAsrcCfgTbl[3].eNowRate = BACH_RATE_48K;
    m_tAsrcCfgTbl[4].eNowRate = BACH_RATE_48K;
    m_tAsrcCfgTbl[5].eNowRate = BACH_RATE_48K;
    m_tAsrcCfgTbl[6].eNowRate = BACH_RATE_48K;

    m_tAsrcCfgTbl[0].eUsedSynth = BACH_NF_SYNTH_1;
    m_tAsrcCfgTbl[1].eUsedSynth = BACH_NF_SYNTH_1;
    m_tAsrcCfgTbl[2].eUsedSynth = BACH_NF_SYNTH_2;
    m_tAsrcCfgTbl[3].eUsedSynth = BACH_NF_SYNTH_2;
    m_tAsrcCfgTbl[4].eUsedSynth = BACH_NF_SYNTH_4;
    m_tAsrcCfgTbl[5].eUsedSynth = BACH_NF_SYNTH_6;
    m_tAsrcCfgTbl[6].eUsedSynth = BACH_NF_SYNTH_6;

    if(!result)
        ERRMSG("Config Init Fail!!!\n");

}


/*void BachFuncCfgReInit(BachFunction_e eFunc)
{
	BachSinkCfg_t tSinkCfg;
	BachSinkCfg_t tSrcCfg;

	switch(eFunc)
	{
		case BACH_FUNC_MM://input 48K,output 48K & line-out
			tSrcCfg.tDmaCfg.eRate = BACH_RATE_48K;
			SourceSaveCfg(BACH_SRC_DMA1_READER, tSrcCfg);

			tSinkCfg.tI2sCfg.eRate = BACH_RATE_48K;
			tSinkCfg.tI2sCfg.eWidth= BACH_I2S_WIDTH_16;
			tSinkCfg.tI2sCfg.eMode = BACH_PCM_MODE_I2S;
			tSinkCfg.tI2sCfg.eFormat = BACH_I2S_FMT_LEFT_JUSTIFY;
			SinkSaveCfg(BACH_SINK_CODEC_I2S, tSinkCfg);

			tSinkCfg.tSpidfHdmiCfg.eRate = BACH_RATE_48K;
			SinkSaveCfg(BACH_SINK_SPIDF_HDMI, tSinkCfg);

			DacType = BACH_DAC_TYPE_BOTH;
			break;
		case BACH_FUNC_GPS://input 48K,output line-out
			tSrcCfg.tDmaCfg.eRate = BACH_RATE_48K;
			SourceSaveCfg(BACH_SRC_DMA2_READER, tSrcCfg);
			DacType = BACH_DAC_TYPE_BOTH;
			break;
		case BACH_FUNC_BT_I2S://input 8K,output 48K & line-out

			tSrcCfg.tI2sCfg.eRate=BACH_RATE_8K;
			tSrcCfg.tI2sCfg.eWidth=BACH_I2S_WIDTH_16;
			tSrcCfg.tI2sCfg.eMode = BACH_PCM_MODE_I2S;
			tSrcCfg.tI2sCfg.eFormat = BACH_I2S_FMT_LEFT_JUSTIFY;
			SourceSaveCfg(BACH_SRC_BT_I2S, tSrcCfg);

			tSrcCfg.tDmaCfg.eRate = BACH_RATE_8K;
			SourceSaveCfg(BACH_SRC_DMA1_READER, tSrcCfg);	//uplink

			tSinkCfg.tDmaCfg.eRate = BACH_RATE_8K;
			SinkSaveCfg(BACH_SINK_DMA1_WRITER, tSinkCfg);	//downlink for aec
			SinkSaveCfg(BACH_SINK_DMA2_WRITER, tSinkCfg);	//mic-in for aec

			//user define
			tSinkCfg.tI2sCfg.eRate = BACH_RATE_48K;
			tSinkCfg.tI2sCfg.eWidth= BACH_I2S_WIDTH_16;
			tSinkCfg.tI2sCfg.eMode = BACH_PCM_MODE_I2S;
			tSinkCfg.tI2sCfg.eFormat = BACH_I2S_FMT_LEFT_JUSTIFY;
			SinkSaveCfg(BACH_SINK_CODEC_I2S, tSinkCfg);

			tSinkCfg.tSpidfHdmiCfg.eRate = BACH_RATE_48K;
			SinkSaveCfg(BACH_SINK_SPIDF_HDMI, tSinkCfg);

			DacType = BACH_DAC_TYPE_LEFT;
			break;
		case BACH_FUNC_DBB:
			tSrcCfg.tI2sCfg.eRate=BACH_RATE_8K;
			tSrcCfg.tI2sCfg.eWidth=BACH_I2S_WIDTH_16;
			tSrcCfg.tI2sCfg.eMode = BACH_PCM_MODE_I2S;
			tSrcCfg.tI2sCfg.eFormat = BACH_I2S_FMT_LEFT_JUSTIFY;
			SourceSaveCfg(BACH_SRC_DBB_I2S, tSrcCfg);

			//user define
			tSinkCfg.tI2sCfg.eRate = BACH_RATE_48K;
			tSinkCfg.tI2sCfg.eWidth= BACH_I2S_WIDTH_16;
			tSinkCfg.tI2sCfg.eMode = BACH_PCM_MODE_I2S;
			tSinkCfg.tI2sCfg.eFormat = BACH_I2S_FMT_LEFT_JUSTIFY;
			SinkSaveCfg(BACH_SINK_CODEC_I2S, tSinkCfg);

			tSinkCfg.tSpidfHdmiCfg.eRate = BACH_RATE_48K;
			SinkSaveCfg(BACH_SINK_SPIDF_HDMI, tSinkCfg);

			DacType = BACH_DAC_TYPE_RIGHT;
			break;
		case BACH_FUNC_DBB_AEC:
			tSrcCfg.tI2sCfg.eRate=BACH_RATE_8K;
			tSrcCfg.tI2sCfg.eWidth=BACH_I2S_WIDTH_16;
			tSrcCfg.tI2sCfg.eMode = BACH_PCM_MODE_I2S;
			tSrcCfg.tI2sCfg.eFormat = BACH_I2S_FMT_LEFT_JUSTIFY;
			SourceSaveCfg(BACH_SRC_DBB_AEC_I2S, tSrcCfg);

			tSrcCfg.tDmaCfg.eRate = BACH_RATE_8K;
			SourceSaveCfg(BACH_SRC_DMA1_READER, tSrcCfg);	//uplink

			tSinkCfg.tDmaCfg.eRate = BACH_RATE_8K;
			SinkSaveCfg(BACH_SINK_DMA1_WRITER, tSinkCfg);	//downlink for aec
			SinkSaveCfg(BACH_SINK_DMA2_WRITER, tSinkCfg);	//mic-in for aec

			//user define
			tSinkCfg.tI2sCfg.eRate = BACH_RATE_48K;
			tSinkCfg.tI2sCfg.eWidth= BACH_I2S_WIDTH_16;
			tSinkCfg.tI2sCfg.eMode = BACH_PCM_MODE_I2S;
			tSinkCfg.tI2sCfg.eFormat = BACH_I2S_FMT_LEFT_JUSTIFY;
			SinkSaveCfg(BACH_SINK_CODEC_I2S, tSinkCfg);

			tSinkCfg.tSpidfHdmiCfg.eRate = BACH_RATE_48K;
			SinkSaveCfg(BACH_SINK_SPIDF_HDMI, tSinkCfg);

			DacType = BACH_DAC_TYPE_RIGHT;
			break;
		case BACH_FUNC_FM:
			tSrcCfg.tAuxCfg.eRate = BACH_RATE_32K;
			tSrcCfg.tAuxCfg.bIsStereo = FALSE;
			SourceSaveCfg(BACH_SRC_AUX, tSrcCfg);	//uplink
			if (tSrcCfg.tAuxCfg.bIsStereo)
				DacType = BACH_DAC_TYPE_BOTH;
			else
				DacType = BACH_DAC_TYPE_LEFT;

			//user define
			tSinkCfg.tI2sCfg.eRate = BACH_RATE_48K;
			tSinkCfg.tI2sCfg.eWidth= BACH_I2S_WIDTH_16;
			tSinkCfg.tI2sCfg.eMode = BACH_PCM_MODE_I2S;
			tSinkCfg.tI2sCfg.eFormat = BACH_I2S_FMT_LEFT_JUSTIFY;
			SinkSaveCfg(BACH_SINK_CODEC_I2S, tSinkCfg);

			tSinkCfg.tSpidfHdmiCfg.eRate = BACH_RATE_48K;
			SinkSaveCfg(BACH_SINK_SPIDF_HDMI, tSinkCfg);
			break;
		case BACH_FUNC_RECORDER:
			tSinkCfg.tDmaCfg.eRate = BACH_RATE_8K;
			SinkSaveCfg(BACH_SINK_DMA2_WRITER, tSinkCfg);	//recording
			break;
		case BACH_FUNC_FM_RECORDER:
			tSinkCfg.tDmaCfg.eRate = BACH_RATE_32K;
			SinkSaveCfg(BACH_SINK_DMA1_WRITER, tSinkCfg);
			break;

	}
}
*/

void BachFuncEnable(BachFunction_e eFunc, BOOL bEnable)
{
    if(bEnable)
    {
        switch(eFunc)
        {
        case BACH_FUNC_DMA://input 48K,output 48K & line-out
        case BACH_FUNC_BT_A2DP:
            if(m_BoardRevision == E_BD_MST154A_D01A_S)
            {
                if(tSinkCfg.tCodecI2sCfg.eRate == BACH_RATE_SLAVE)
                {
                    ERRMSG("BachFuncEnable - codec I2s only support master mode!\n");
                    tSinkCfg.tCodecI2sCfg.eRate = BACH_RATE_48K;
                    BachI2sConfig(BACH_I2S_CODEC_TX, tSinkCfg.tCodecI2sCfg);
                }
                BachI2sPadMux(BACH_I2S_PAD_CODEC,BACH_I2S_OUTPAD_BT,TRUE);
            }
            tSrcCfg.tDma1ReaderCfg.eRate = BachRateMapping(m_dmachannel[BACH_DMA_READER1].nSampleRate);//BACH_RATE_48K;
            BachChannelSelectSrc(BACH_CHAN_ASRC1, BACH_SRC_DMA1_READER);

            BachSinkSelectChannel(BACH_SINK_CODEC_I2S, BACH_CHAN_ASRC1);
            BachSelectSinkTxInput(BACH_SINKTX_CODEC_I2S, BACH_CHMODE_BOTH);
            BachSinkSelectChannel(BACH_SINK_SPDIF_HDMI, BACH_CHAN_ASRC1);
            BachSelectSinkTxInput(BACH_SINKTX_SPDIF, BACH_CHMODE_BOTH);
            BachSelectSinkTxInput(BACH_SINKTX_HDMI, BACH_CHMODE_BOTH);

            BachDacSelectChannel(BACH_DAC1, BACH_CHMODE_BOTH);	//speaker1
            break;
        case BACH_FUNC_GPS://input 48K,output line-out
            tSrcCfg.tDma2ReaderCfg.eRate = BachRateMapping(m_dmachannel[BACH_DMA_READER2].nSampleRate);//BACH_RATE_48K;
            BachChannelSelectSrc(BACH_CHAN_ASRC2, BACH_SRC_DMA2_READER);

            BachDacSelectChannel(BACH_DAC2, BACH_CHMODE_BOTH);	//speaker2
            break;
        case BACH_FUNC_DMA_SUBCH://input 48K,output line-out
            BachChannelSelectSrc(BACH_CHAN_ASRC2, BACH_SRC_DMA1_READER);

            BachDacSelectChannel(BACH_DAC2, BACH_CHMODE_BOTH);	//speaker2
            break;
        case BACH_FUNC_BT_I2S://input eInRate,output 48K & line-out
            tSinkCfg.tDma2WriterCfg.eRate = tSrcCfg.tAdcCfg.eRate;
            tSrcCfg.tDma1ReaderCfg.eRate = tSinkCfg.tDma2WriterCfg.eRate;
            BachChannelSelectSrc(BACH_CHAN_ASRC1, BACH_SRC_BT_I2S);

            BachWriteReg(BACH_REG_BANK1, BACH_BT_I2S_CTRL0, REG_PAD_I2S_TRX_BCK_INV, 0);
            BachWriteReg(BACH_REG_BANK1, BACH_BT_I2S_CTRL1, REG_BT_I2S_SOURCE_SEL, 0);

            tSinkCfg.tDma1WriterCfg.eRate = tSinkCfg.tDma2WriterCfg.eRate;
            BachSinkSelectChannel(BACH_SINK_DMA1_WRITER, BACH_CHAN_ASRC1);
            BachSinkSelectChannel(BACH_SINK_DMA2_WRITER, BACH_CHAN_MICIN);

            BachSinkSelectChannel(BACH_SINK_CODEC_I2S, BACH_CHAN_ASRC1);
            BachSelectSinkTxInput(BACH_SINKTX_CODEC_I2S, BACH_CHMODE_LEFT);
            BachSinkSelectChannel(BACH_SINK_SPDIF_HDMI, BACH_CHAN_ASRC1);
            BachSelectSinkTxInput(BACH_SINKTX_SPDIF, BACH_CHMODE_LEFT);
            BachSelectSinkTxInput(BACH_SINKTX_HDMI, BACH_CHMODE_LEFT);

            BachDacSelectChannel(BACH_DAC1, BACH_CHMODE_LEFT);

            break;
        case BACH_FUNC_DBB:
            BachChannelSelectSrc(BACH_CHAN_ASRC1, BACH_SRC_DBB_I2S);

            BachSelectDbbUplinkSource(BACH_DBB_UPLINK_DEC1);
            BachSinkSelectChannel(BACH_SINK_CODEC_I2S, BACH_CHAN_ASRC1);
            BachSelectSinkTxInput(BACH_SINKTX_CODEC_I2S, BACH_CHMODE_RIGHT);
            BachSinkSelectChannel(BACH_SINK_SPDIF_HDMI, BACH_CHAN_ASRC1);
            BachSelectSinkTxInput(BACH_SINKTX_SPDIF, BACH_CHMODE_RIGHT);
            BachSelectSinkTxInput(BACH_SINKTX_HDMI, BACH_CHMODE_RIGHT);

            BachDacSelectChannel(BACH_DAC1, BACH_CHMODE_RIGHT);

            break;
        case BACH_FUNC_DBB_AEC:

            tSinkCfg.tDma2WriterCfg.eRate = tSrcCfg.tAdcCfg.eRate;

            tSrcCfg.tDma1ReaderCfg.eRate = tSinkCfg.tDma2WriterCfg.eRate;
            BachChannelSelectSrc(BACH_CHAN_ASRC1, BACH_SRC_DBB_AEC_I2S);

            tSinkCfg.tDma1WriterCfg.eRate = tSinkCfg.tDma2WriterCfg.eRate;
            BachSinkSelectChannel(BACH_SINK_DMA1_WRITER, BACH_CHAN_ASRC1);
            BachSinkSelectChannel(BACH_SINK_DMA2_WRITER, BACH_CHAN_MICIN);

            BachSelectDbbUplinkSource(BACH_DBB_UPLINK_DMA1_RD_RIGHT);

            BachSinkSelectChannel(BACH_SINK_CODEC_I2S, BACH_CHAN_ASRC1);
            BachSelectSinkTxInput(BACH_SINKTX_CODEC_I2S, BACH_CHMODE_RIGHT);
            BachSinkSelectChannel(BACH_SINK_SPDIF_HDMI, BACH_CHAN_ASRC1);
            BachSelectSinkTxInput(BACH_SINKTX_SPDIF, BACH_CHMODE_RIGHT);
            BachSelectSinkTxInput(BACH_SINKTX_HDMI, BACH_CHMODE_RIGHT);

            BachDacSelectChannel(BACH_DAC1, BACH_CHMODE_RIGHT);

            /*for test*/
            /*BachWriteReg(BACH_REG_BANK1,BACH_AUDIOBAND_CTRL1,REG_S2M_ASRC2_CH2_SEL_MSK|REG_DMARD_R_DPGA_DIN_SEL,(0x2<<REG_S2M_ASRC2_CH2_SEL_POS)|REG_DMARD_R_DPGA_DIN_SEL);
            BachWriteReg(BACH_REG_BANK1,BACH_SINE_GEN_CTRL0,REG_ASRC2_INT1_MIX_CTRL_MSK,(0x2<<REG_ASRC2_INT1_MIX_CTRL_POS));
            BachWriteReg(BACH_REG_BANK1,BACH_SINE_GEN_CTRL0,REG_ASRC2_INT2_MIX_CTRL_MSK,(0x2<<REG_ASRC2_INT2_MIX_CTRL_POS));
            BachWriteReg(BACH_REG_BANK1,BACH_MIX_CTRL0,REG_AMR_MIX_MSK,REG_AMR_MIX_MSK);
            BachWriteReg(BACH_REG_BANK2,BACH_PAD_CTRL1,REG_DMA2_WR_MUX_SEL_MSK,0x2<<REG_DMA2_WR_MUX_SEL_POS);
            BachWriteReg(BACH_REG_BANK2,BACH_DBB_I2S_CTRL,REG_DBB_I2S_LOOPBACK_EN,REG_DBB_I2S_LOOPBACK_EN);*/

            break;
        case BACH_FUNC_FM:
            BachChannelSelectSrc(BACH_CHAN_ASRC1, BACH_SRC_AUX);

            BachSinkSelectChannel(BACH_SINK_CODEC_I2S, BACH_CHAN_ASRC1);
            BachSinkSelectChannel(BACH_SINK_SPDIF_HDMI, BACH_CHAN_ASRC1);
            if (tSrcCfg.tAuxCfg.bIsStereo)
            {
                BachDacSelectChannel(BACH_DAC1, BACH_CHMODE_BOTH);
                BachSelectSinkTxInput(BACH_SINKTX_CODEC_I2S, BACH_CHMODE_BOTH);
                BachSelectSinkTxInput(BACH_SINKTX_SPDIF, BACH_CHMODE_BOTH);
                BachSelectSinkTxInput(BACH_SINKTX_HDMI, BACH_CHMODE_BOTH);
            }
            else
            {
                BachDacSelectChannel(BACH_DAC1, BACH_CHMODE_LEFT);
                BachSelectSinkTxInput(BACH_SINKTX_CODEC_I2S, BACH_CHMODE_LEFT);
                BachSelectSinkTxInput(BACH_SINKTX_SPDIF, BACH_CHMODE_LEFT);
                BachSelectSinkTxInput(BACH_SINKTX_HDMI, BACH_CHMODE_LEFT);
            }
            break;
        case BACH_FUNC_RECORDER:
            tSinkCfg.tDma2WriterCfg.eRate = tSrcCfg.tAdcCfg.eRate;
            BachSinkSelectChannel(BACH_SINK_DMA2_WRITER, BACH_CHAN_MICIN);
            break;
        case BACH_FUNC_FM_RECORDER:
            tSinkCfg.tDma1WriterCfg.eRate = tSrcCfg.tAuxCfg.eRate;
            BachSinkSelectChannel(BACH_SINK_DMA1_WRITER, BACH_CHAN_LINEIN);
            break;
        case BACH_FUNC_DMA_RECORDER:
            tSinkCfg.tDma1WriterCfg.eRate = tSrcCfg.tDma1ReaderCfg.eRate;
            BachSinkSelectChannel(BACH_SINK_DMA1_WRITER, BACH_CHAN_ASRC1);
            break;
        case BACH_FUNC_EXTBT_PCMMTK_M:
            BachI2sPadMux(BACH_I2S_PAD_BT,BACH_I2S_OUTPAD_BT,TRUE);
            if(tSrcCfg.tBtI2sCfg.eRate == BACH_RATE_SLAVE)
                ERRMSG("BachFuncEnable - master mode rate should be set!");

            //config setting
            tSrcCfg.tBtI2sCfg.eRate = BACH_RATE_8K;
            tSrcCfg.tBtI2sCfg.eWidth = BACH_I2S_WIDTH_16;
            tSrcCfg.tBtI2sCfg.eMode = BACH_PCM_MODE_I2S;
            tSrcCfg.tBtI2sCfg.eFormat = BACH_I2S_FMT_LEFT_JUSTIFY;
            BachI2sConfig(BACH_I2S_BT_TRX, tSrcCfg.tBtI2sCfg);

            tSinkCfg.tDma2WriterCfg.eRate = tSrcCfg.tAdcCfg.eRate;
            tSrcCfg.tDma1ReaderCfg.eRate = tSinkCfg.tDma2WriterCfg.eRate;
            BachChannelSelectSrc(BACH_CHAN_ASRC1, BACH_SRC_BT_I2S);

            BachWriteReg(BACH_REG_BANK1, BACH_BT_I2S_CTRL0, REG_PAD_I2S_TRX_BCK_INV, REG_PAD_I2S_TRX_BCK_INV); //match rising edge
            BachWriteReg(BACH_REG_BANK1, BACH_BT_I2S_CTRL1, REG_BT_I2S_SOURCE_SEL, REG_BT_I2S_SOURCE_SEL);

            tSinkCfg.tDma1WriterCfg.eRate = tSinkCfg.tDma2WriterCfg.eRate;
            BachSinkSelectChannel(BACH_SINK_DMA1_WRITER, BACH_CHAN_ASRC1);
            BachSinkSelectChannel(BACH_SINK_DMA2_WRITER, BACH_CHAN_MICIN);

            BachSinkSelectChannel(BACH_SINK_CODEC_I2S, BACH_CHAN_ASRC1);
            BachSelectSinkTxInput(BACH_SINKTX_CODEC_I2S, BACH_CHMODE_LEFT);
            BachSinkSelectChannel(BACH_SINK_SPDIF_HDMI, BACH_CHAN_ASRC1);
            BachSelectSinkTxInput(BACH_SINKTX_SPDIF, BACH_CHMODE_LEFT);
            BachSelectSinkTxInput(BACH_SINKTX_HDMI, BACH_CHMODE_LEFT);

            BachDacSelectChannel(BACH_DAC1, BACH_CHMODE_LEFT);

            break;
        case BACH_FUNC_EXTBT_I2S_M:
            BachI2sPadMux(BACH_I2S_PAD_BT,BACH_I2S_OUTPAD_BT, TRUE);
            if(tSrcCfg.tBtI2sCfg.eRate == BACH_RATE_SLAVE)
            {
                ERRMSG("BachFuncEnable - master mode rate should be set!");
                BachBtSetRate(BACH_RATE_8K);
            }

            tSinkCfg.tDma2WriterCfg.eRate = tSrcCfg.tAdcCfg.eRate;
            tSrcCfg.tDma1ReaderCfg.eRate = tSinkCfg.tDma2WriterCfg.eRate;

            BachChannelSelectSrc(BACH_CHAN_ASRC1, BACH_SRC_BT_I2S);

            BachWriteReg(BACH_REG_BANK1, BACH_BT_I2S_CTRL0, REG_PAD_I2S_TRX_BCK_INV, 0);
            BachWriteReg(BACH_REG_BANK1, BACH_BT_I2S_CTRL1, REG_BT_I2S_SOURCE_SEL, REG_BT_I2S_SOURCE_SEL);

            tSinkCfg.tDma1WriterCfg.eRate = tSinkCfg.tDma2WriterCfg.eRate;
            BachSinkSelectChannel(BACH_SINK_DMA1_WRITER, BACH_CHAN_ASRC1);
            BachSinkSelectChannel(BACH_SINK_DMA2_WRITER, BACH_CHAN_MICIN);

            BachSinkSelectChannel(BACH_SINK_CODEC_I2S, BACH_CHAN_ASRC1);
            BachSelectSinkTxInput(BACH_SINKTX_CODEC_I2S, BACH_CHMODE_LEFT);
            BachSinkSelectChannel(BACH_SINK_SPDIF_HDMI, BACH_CHAN_ASRC1);
            BachSelectSinkTxInput(BACH_SINKTX_SPDIF, BACH_CHMODE_LEFT);
            BachSelectSinkTxInput(BACH_SINKTX_HDMI, BACH_CHMODE_LEFT);

            BachDacSelectChannel(BACH_DAC1, BACH_CHMODE_LEFT);
            break;
        default:
            ERRMSG("BachFuncEnable - NO USE FUNCTION!!!\n");
            return;

        }
    }
}

BOOL BachI2sConfig(BachI2s_e eI2s, BachI2sCfg_t tCfg)
{
    BOOL result = TRUE;
    result &= BachSetI2sRate(eI2s, tCfg.eRate);
    result &= BachSetI2sMode(eI2s, tCfg.eMode);
    result &= BachSetI2sFmt(eI2s, tCfg.eFormat);
    result &= BachSetI2sWidth(eI2s, tCfg.eWidth);

    return result;
}

BOOL BachI2sEnable(BachI2s_e eI2s, BOOL bEn)
{
    BOOL result = TRUE;

    result &= BachSetI2sBck(eI2s, bEn);

    if (eI2s == BACH_I2S_CODEC_TX)
    {
      if(bEn)
      {
        BachWriteReg(BACH_REG_BANK1, BACH_CKG_CTRL1, REG_AU_MCLK_SEL_MSK, 0);
        BachWriteReg(BACH_REG_BANK2, BACH_PAD_CTRL1, 1, 0);   //switch gpio28 pad to mclk mode
      }
      else
      {
        BachWriteReg(BACH_REG_BANK1, BACH_CKG_CTRL1, REG_AU_MCLK_SEL_MSK, (5 << REG_AU_MCLK_SEL_POS));
        //BachWriteReg(BACH_REG_BANK2, BACH_PAD_CTRL1, 1, 1); //switch gpio28 pad to gpio mode
      }
    }

    return result;
}

/*void BachSourceSaveCfg(BachSource_e eSrc, BachSrcCfg_t tSrcCfg)
{
	m_tSrcCfgTbl[eSrc] = tSrcCfg;
}

void BachSinkSaveCfg(BachSink_e eSink, BachSinkCfg_t tSinkCfg)
{
	m_tSinkCfgTbl[eSink] = tSinkCfg;
}*/

/*BOOL BachSourceSetCfg(BachSource_e eSrc)
{
	BOOL result = TRUE;

	switch(eSrc)
	{
	  case BACH_SRC_DMA1_READER:
	  case BACH_SRC_DMA2_READER:
	  		//DmaConfig(BACH_I2S_EXT_RX, tSrcCfg.tI2sCfg);
			break;
	  case BACH_SRC_EXT_I2S:
	  		result = I2sConfig(BACH_I2S_EXT_RX, tSrcCfg.tExtI2sCfg);
			break;
	  case BACH_SRC_BT_I2S:
	  		result = I2sConfig(BACH_I2S_BT_TRX, tSrcCfg.tBtI2sCfg);
			break;
	  case BACH_SRC_DBB_I2S:
	  case BACH_SRC_DBB_AEC_I2S:
	  		result = I2sConfig(BACH_I2S_DBB_TRX, tSrcCfg.tDbbI2sCfg);
			break;
	  case BACH_SRC_SPDIF:
	  		//SpidfConfig(BACH_I2S_DBB_TRX, tSrcCfg.tI2sCfg);
	  		break;
	  case BACH_SRC_AUX:
  			break;
	  default:
	  	break;
	}

	return result;
}


BOOL BachSinkSetCfg(BachSink_e eSink)
{
	BOOL result = TRUE;

	switch(eSink)
	{
	  case BACH_SINK_DMA1_WRITER:
	  case BACH_SINK_DMA2_WRITER:
		break;
	  case BACH_SINK_CODEC_I2S:
  		result = I2sConfig(BACH_I2S_CODEC_TX, tSinkCfg.tCodecI2sCfg);
		break;
	  case BACH_SINK_SPIDF_HDMI:
	  default:
	  	break;
	}

	return result;
}
*/
/*

BOOL BachAudioSetInputSourceInfo(BachAsrcChannel_e eChan, BachSource_e eSrc, BachSrcCfg_t tSrcCfg)
{
	m_InputSrcCfgTbl[eChan][eSrc] = tSrcCfg;

	if (eSrc == m_nActiveInputSrc)
	{
		AudioSetMute(eChan);



		switch (tSrcCfg.eRate)
		{
			case


		}
	}


}

void BachAsrcChanSetMute(BachAsrcChannel_e eChan, BOOL bMute)
{

	switch(eChan)
    {
		case BACH_ASRC_CHAN1:

			if(bMute)
			{
				m_nDpgaGain[BACH_DPGA_MMP1] = DpgaGetGain(BACH_DPGA_MMP1);
				m_nDpgaGain[BACH_DPGA_MMP2] = DpgaGetGain(BACH_DPGA_MMP2);

				BachDpgaMute(BACH_DPGA_MMP1);
				BachDpgaMute(BACH_DPGA_MMP2);
			}
			else
			{
				BachDpgaSetGain(BACH_DPGA_MMP1, m_nDpgaGain[BACH_DPGA_MMP1]);
				BachDpgaSetGain(BACH_DPGA_MMP2, m_nDpgaGain[BACH_DPGA_MMP2]);
			}
			break;

		case BACH_ASRC_CHAN2:

			if(bMute)
			{
				m_nDpgaGain[BACH_DPGA_ASRC2MMP1] = DpgaGetGain(BACH_DPGA_ASRC2MMP1);
				m_nDpgaGain[BACH_DPGA_ASRC2MMP2] = DpgaGetGain(BACH_DPGA_ASRC2MMP2);

				BachDpgaMute(BACH_DPGA_ASRC2MMP1);
				BachDpgaMute(BACH_DPGA_ASRC2MMP2);
			}
			else
			{
				BachDpgaSetGain(BACH_DPGA_ASRC2MMP1, m_nDpgaGain[BACH_DPGA_ASRC2MMP1]);
				BachDpgaSetGain(BACH_DPGA_ASRC2MMP2, m_nDpgaGain[BACH_DPGA_ASRC2MMP2]);
			}
			break;

			break;

		default:
			break;

	}
}

void BachAsrcChanSetGain(BachAsrcChannel_e eChan, U16 bMute)
{

}

void BachI2SSetConfig(Bool bIsMaster, )
{
	U16 nConfigValue;

    switch(eDmaChannel)
    {
    case BACH_I2S_EXT_RX:
		nConfigValue = BachReadReg(BACH_REG_BANK1,BACH_DMA1_CTRL_8);
		return (nConfigValue & REG_WR_FULL_FLAG) ? TRUE : FALSE;

    case BACH_DMA_WRITER2:
		nConfigValue = BachReadReg(BACH_REG_BANK1,BACH_DMA2_CTRL_8);
		return (nConfigValue & REG_WR_FULL_FLAG) ? TRUE : FALSE;

    default:
        return FALSE;
    }

}
*/
//for 1A06[9:8]
U32 BachGetRecorderRate(void)
{
    U16 nConfigValue;
    nConfigValue = BachReadReg(BACH_REG_BANK1, BACH_TIMING_CTRL0);
    nConfigValue &= REG_DEC_NUM_MSK;
    nConfigValue = nConfigValue >> REG_DEC_NUM_POS;

    switch(nConfigValue)
    {
    case 0:
        return 8000;
    case 1:
        return 16000;
    case 2:
        return 32000;
    case 3:
        return m_dmachannel[BACH_DMA_READER1].nSampleRate;
    default:
        ERRMSG("BachGetRecorderRate() - ERROR default case!\n");
    }
    return 0;
}

//for 1AA0[9:8]
U32 BachGetPlayRecorderRate(void)
{
    U16 nConfigValue;
    nConfigValue = BachReadReg(BACH_REG_BANK1, BACH_TIMING_CTRL1);
    nConfigValue &= REG_DEC_NUM_MSK;
    nConfigValue = nConfigValue >> REG_DEC_NUM_POS;

    switch(nConfigValue)
    {
    case 0:
        return 8000;
    case 1:
        return 16000;
    case 2:
        return 32000;
    case 3:
        return m_dmachannel[BACH_DMA_READER1].nSampleRate;
    default:
        ERRMSG("BachGetPlayRecorderRate() - ERROR default case!\n");
    }
    return 0;
}



void BachSelectDbbUplinkSource(BachDbbUplink_e eSource)
{
    switch(eSource)
    {
    case BACH_DBB_UPLINK_DEC1:
        BachSetMixer(BACH_MIXER_DBB_UL_MIX, 1, 0);
        break;
    case BACH_DBB_UPLINK_DMA1_RD_RIGHT:
        BachSetMux2(BACH_MUX2_DMARD_R, 0);
        BachSetMixer(BACH_MIXER_DBB_UL_MIX, 2, 0);
        break;
    default:
        ERRMSG("BachSelectDbbUplinkSource - ERROR default case!\n");
    }
}

void BachI2sPadMux(BachI2sPad_e eI2s, BachI2sOutPad_e eOutI2s, BOOL bMaster)
{
    switch(eI2s)
    {
    case BACH_I2S_PAD_BT:
        switch(eOutI2s)
        {
        case BACH_I2S_OUTPAD_BT:
            BachWriteReg(BACH_REG_BANK2, BACH_PAD_CTRL0, REG_PAD_BT_I2S_TRX_S_SEL_MSK, 0);
            BachWriteReg(BACH_REG_BANK2, BACH_PAD_CTRL0, REG_PAD_BT_I2S_TRX_M_SEL_MSK, 0);
            break;
        case BACH_I2S_OUTPAD_CODEC:
            BachWriteReg(BACH_REG_BANK2, BACH_PAD_CTRL0, REG_PAD_BT_I2S_TRX_S_SEL_MSK, (1<<REG_PAD_BT_I2S_TRX_S_SEL_POS));
            BachWriteReg(BACH_REG_BANK2, BACH_PAD_CTRL0, REG_PAD_CODEC_I2S_TX_M_SEL_MSK, (2<<REG_PAD_CODEC_I2S_TX_M_SEL_POS));
            break;
        default:
            ERRMSG("BachI2sPadMux - ERROR default OutPad case!\n");
        }
        break;
    case BACH_I2S_PAD_CODEC:
        switch(eOutI2s)
        {
        case BACH_I2S_OUTPAD_BT:
            BachWriteReg(BACH_REG_BANK2, BACH_PAD_CTRL0, REG_PAD_BT_I2S_TRX_M_SEL_MSK, (1<<REG_PAD_BT_I2S_TRX_M_SEL_POS));
            break;
        case BACH_I2S_OUTPAD_CODEC:
            BachWriteReg(BACH_REG_BANK2, BACH_PAD_CTRL0, REG_PAD_CODEC_I2S_TX_M_SEL_MSK, 0);
            break;
        default:
            ERRMSG("BachI2sPadMux - ERROR default OutPad case!\n");
        }
        break;

    default:
        ERRMSG("BachI2sPadMux - ERROR default case!\n");
    }

    switch(eOutI2s)
    {
    case BACH_I2S_OUTPAD_BT:
        if(bMaster)
        {
            BachWriteReg(BACH_REG_BANK1, BACH_BT_I2S_CTRL1, REG_BT_I2S_TRX_WCK_OEN, 0);
            BachWriteReg(BACH_REG_BANK1, BACH_BT_I2S_CTRL1, REG_BT_I2S_TRX_BCK_OEN, 0);
            BachWriteReg(BACH_REG_BANK1, BACH_BT_I2S_CTRL1, REG_BT_I2S_TRX_SDI_OEN, REG_BT_I2S_TRX_SDI_OEN);
            BachWriteReg(BACH_REG_BANK1, BACH_BT_I2S_CTRL1, REG_BT_I2S_TRX_SDO_OEN, 0);
        }
        else
        {
            BachWriteReg(BACH_REG_BANK1, BACH_BT_I2S_CTRL1, REG_BT_I2S_TRX_WCK_OEN, REG_BT_I2S_TRX_WCK_OEN);
            BachWriteReg(BACH_REG_BANK1, BACH_BT_I2S_CTRL1, REG_BT_I2S_TRX_BCK_OEN, REG_BT_I2S_TRX_BCK_OEN);
            BachWriteReg(BACH_REG_BANK1, BACH_BT_I2S_CTRL1, REG_BT_I2S_TRX_SDI_OEN, REG_BT_I2S_TRX_SDI_OEN);
            BachWriteReg(BACH_REG_BANK1, BACH_BT_I2S_CTRL1, REG_BT_I2S_TRX_SDO_OEN, 0);
        }
        break;
    case BACH_I2S_OUTPAD_CODEC:
        if(bMaster)
        {
            BachWriteReg(BACH_REG_BANK2, BACH_PAD_CTRL1, REG_CODEC_I2S_TRX_WCK_OEN, 0);
            BachWriteReg(BACH_REG_BANK2, BACH_PAD_CTRL1, REG_CODEC_I2S_TRX_BCK_OEN, 0);
            BachWriteReg(BACH_REG_BANK2, BACH_PAD_CTRL1, REG_CODEC_I2S_TRX_SDI_OEN, REG_CODEC_I2S_TRX_SDI_OEN);
            BachWriteReg(BACH_REG_BANK2, BACH_PAD_CTRL1, REG_CODEC_I2S_TRX_SDO_OEN, 0);
        }
        else
        {
            BachWriteReg(BACH_REG_BANK2, BACH_PAD_CTRL1, REG_CODEC_I2S_TRX_WCK_OEN, REG_CODEC_I2S_TRX_WCK_OEN);
            BachWriteReg(BACH_REG_BANK2, BACH_PAD_CTRL1, REG_CODEC_I2S_TRX_BCK_OEN, REG_CODEC_I2S_TRX_BCK_OEN);
            BachWriteReg(BACH_REG_BANK2, BACH_PAD_CTRL1, REG_CODEC_I2S_TRX_SDI_OEN, REG_CODEC_I2S_TRX_SDI_OEN);
            BachWriteReg(BACH_REG_BANK2, BACH_PAD_CTRL1, REG_CODEC_I2S_TRX_SDO_OEN, 0);
        }
        break;
    case BACH_I2S_OUTPAD_EXT:
        if(bMaster)
        {
            BachWriteReg(BACH_REG_BANK2, BACH_PAD_CTRL1, REG_EXT_I2S_RX_WCK_OEN, 0);
            BachWriteReg(BACH_REG_BANK2, BACH_PAD_CTRL1, REG_EXT_I2S_RX_BCK_OEN, 0);
            BachWriteReg(BACH_REG_BANK2, BACH_PAD_CTRL1, REG_EXT_I2S_RX_SDI_OEN, REG_EXT_I2S_RX_SDI_OEN);
        }
        else
        {
            BachWriteReg(BACH_REG_BANK2, BACH_PAD_CTRL1, REG_EXT_I2S_RX_WCK_OEN, REG_EXT_I2S_RX_WCK_OEN);
            BachWriteReg(BACH_REG_BANK2, BACH_PAD_CTRL1, REG_EXT_I2S_RX_BCK_OEN, REG_EXT_I2S_RX_BCK_OEN);
            BachWriteReg(BACH_REG_BANK2, BACH_PAD_CTRL1, REG_EXT_I2S_RX_SDI_OEN, REG_EXT_I2S_RX_SDI_OEN);
        }
        break;
    default:
        ERRMSG("BachI2sPadMux - ERROR default OutPad case!\n");

    }


}


/*void BachRestoreFunc()
{
    int i;
    for(i=0;i<BACH_FUNC_NUM;i++)
    {
        if(m_bBachLastFuncStatus[i])
            BachFuncEnable((BachFunction_e)i,TRUE);
    }
}

void BachHaltFunc()
{
    int i;
    for(i=0;i<BACH_FUNC_NUM;i++)
    {
        m_bBachLastFuncStatus[i]=FALSE;
        if(m_bBachFuncStatus[i])
        {
            m_bBachLastFuncStatus[i]=TRUE;
        }
    }
}*/

/*void BachRestoreAtop()
{
    int i;
    for(i=0;i<BACH_ATOP_NUM;i++)
    {
        if(m_bBachLastAtopStatus[i])
            BachOpenAtop((BachAtopPath_e)i);
    }
}*/

void BachHaltAtop(void)
{
    int i;
    for(i = 0; i < BACH_ATOP_NUM; i++)
    {
        m_bBachLastAtopStatus[i] = FALSE;
        if(m_bBachAtopStatus[i])
        {
            m_bBachLastAtopStatus[i] = TRUE;
            BachCloseAtop((BachAtopPath_e)i);
        }
    }
}

void BachResetAsrcCfgTable(void)
{
    BachAsrcCfg_e m_tAsrcCfgInitTbl[] =
    {
        {BACH_RATE_48K, BACH_NF_SYNTH_1, TRUE},
        {BACH_RATE_48K, BACH_NF_SYNTH_1, TRUE},
        {BACH_RATE_48K, BACH_NF_SYNTH_2, TRUE},
        {BACH_RATE_48K, BACH_NF_SYNTH_2, TRUE},
        {BACH_RATE_48K, BACH_NF_SYNTH_4, FALSE},
        {BACH_RATE_48K, BACH_NF_SYNTH_6, TRUE},
        {BACH_RATE_48K, BACH_NF_SYNTH_6, TRUE}
    };

    memcpy(m_tAsrcCfgTbl, m_tAsrcCfgInitTbl, sizeof(m_tAsrcCfgInitTbl));
}

void BachSelectDmaWR12Input(BachChannelMode_e eChannel)
{
    //set MUX2
    switch(eChannel)
    {
    case BACH_CHMODE_BOTH:
        BachSetMux2(BACH_MUX2_DMAWR1, 0);
        BachSetMux2(BACH_MUX2_DMAWR2, 0);
        break;
    case BACH_CHMODE_LEFT:
        BachSetMux2(BACH_MUX2_DMAWR1, 0);
        BachSetMux2(BACH_MUX2_DMAWR2, 1);
        break;
    case BACH_CHMODE_RIGHT:
        BachSetMux2(BACH_MUX2_DMAWR1, 1);
        BachSetMux2(BACH_MUX2_DMAWR2, 0);
        break;
    case BACH_CHMODE_BOTH_INV:
        BachSetMux2(BACH_MUX2_DMAWR1, 1);
        BachSetMux2(BACH_MUX2_DMAWR2, 1);
        break;
    default:
        ERRMSG("BachSelectDmaWR12Input - ERROR default channel mode!\n");
        return;
    }
}


BOOL BachDualMicSetting(void)
{
    if(m_bBachAtopStatus[BACH_ATOP_HANDSET] || m_bBachAtopStatus[BACH_ATOP_AUXMIC])
    {
        ERRMSG("BachDualMicSetting - ERROR!! this function should be called before MIC turns on.\n");
        return FALSE;
    }

    m_nRecGain[1] = 10;
    m_nMicGain = 0x0;
    m_nMicInitGain = 0x0;
    return TRUE;
}

/*
BOOL BachLineOutGainSetting(U16 nChoice)
{
    switch(nChoice)
    {
    case 0:
        m_nLineOutGain = 0x0; //0dB
        break;
    case 1:
        m_nLineOutGain = 0x1; //3dB
        break;
    default:
        return FALSE;
    }

    return TRUE;
}
*/

BachRate_e BachRateMapping(U32 nRate)
{
    switch(nRate)
    {
    case 8000:
        return BACH_RATE_8K;
    case 16000:
        return BACH_RATE_16K;
    case 32000:
        return BACH_RATE_32K;
    case 44100:
        return BACH_RATE_44K;
    case 48000:
        return BACH_RATE_48K;
    case 96000:
        return BACH_RATE_96K;
    default:
        return BACH_RATE_NULL;
    }
}


BOOL BachAtopLineOutGain(BachAtopPath_e eLineOut, U16 nLevel)
{
    U8 nLO = 0;

    if (eLineOut == BACH_ATOP_LINEOUT0)
      nLO = 0;
    else if (eLineOut == BACH_ATOP_LINEOUT1)
      nLO = 1;
    else
      return FALSE;

    switch(nLevel)
    {
    case 0:
        m_nLineOutMute[nLO] = 1;
        break;
    case 1:
        m_nLineOutMute[nLO] = 0;
        m_nLineOutGain[nLO] = 0x0; //0dB
        break;
    case 2:
        m_nLineOutMute[nLO] = 0;
        m_nLineOutGain[nLO] = 0x1; //3dB
        break;
    default:
        return FALSE;
    }

    return TRUE;
}

BOOL BachAtopLineInGain(BachAtopPath_e eLineIn, U16 nLevel)
{
    U8 nLIN = 0;

    if (eLineIn == BACH_ATOP_LINEIN0)
      nLIN = 0;
    else if (eLineIn == BACH_ATOP_LINEIN1)
      nLIN = 1;
    else
    {
      ERRMSG("BachAtopLineInGain - LineIn enum failed\n");
      return FALSE;
    }

    if (nLevel == 0)
    {
        m_nLineInMute[nLIN] = 1;
    }
    else if (nLevel <= 6)
    {
        m_nLineInMute[nLIN] = 0;
        m_nLineInGain[nLIN] = nLevel-1; //0dB
    }
    else
    {
        ERRMSG("BachAtopLineInGain - Level too large\n");
        return FALSE;
    }

    return TRUE;
}

BOOL BachAtopMicGain(U16 nLevel)
{
    if(nLevel>5)
    {
        ERRMSG("BachAtopMicGain - ERROR!! Level too large .\n");
        return FALSE;
    }

    m_nMicGain = m_nMicInitGain + nLevel;
    if(m_nMicGain>5)
    {
        m_nMicPreGain = 1;
        m_nMicGain -= 3;
    }
    else
        m_nMicPreGain = 0;


    return TRUE;
}


void BachAtopLineInMux(BachAtopPath_e eAtop,U16 nLineIn)
{
    U16 nValue;
    switch(eAtop)
    {
    case BACH_ATOP_LINEIN0:
        m_nLineInMux[0] = nLineIn;
        if(m_bBachAtopStatus[BACH_ATOP_LINEIN0])
        {
            nValue = BachReadReg2Byte(0x112C0A);
            nValue &= ~(PGA0_LINE_SEL);
            nValue |= (m_nLineInMux[0] ? PGA0_LINE_SEL : 0);
            BachWriteReg2Byte(0x112C0A, nValue);
        }
        break;
    case BACH_ATOP_LINEIN1:
        m_nLineInMux[1] = !nLineIn;
        if(m_bBachAtopStatus[BACH_ATOP_LINEIN1])
        {
            nValue = BachReadReg2Byte(0x112C0C);
            nValue &= ~(PGA1_LINE_SEL);
            nValue |= (m_nLineInMux[1] ? PGA1_LINE_SEL : 0);
            BachWriteReg2Byte(0x112C0C, nValue);
        }
        break;
    default:
        ERRMSG("BachAtopLineInMux - ERROR!! UNSUPPORT DEVICE");
    }
}

BOOL BachCodecSetCfg(BachI2sFmt_e eFormat, BachI2sWidth_e eWidth, BachRate_e eRate)
{
    if(eRate == BACH_RATE_SLAVE)
        return FALSE;
    tSinkCfg.tCodecI2sCfg.eFormat = eFormat;
    tSinkCfg.tCodecI2sCfg.eWidth = eWidth;
    tSinkCfg.tCodecI2sCfg.eRate = eRate;
    return BachI2sConfig(BACH_I2S_CODEC_TX, tSinkCfg.tCodecI2sCfg);
}

BOOL BachAtopLineInGainSet(U16 nLineIn,U16 nLevel)
{
    U16 nValue;
    if(nLineIn>1)
    {
        ERRMSG("BachAtopLineInGain - ERROR!! No this device  .");
        return FALSE;
    }
    if(nLevel>5)
    {
        ERRMSG("BachAtopLineInGain - ERROR!! Level not support .");
        return FALSE;
    }

    m_nLineInGain[nLineIn]=nLevel;

    if(nLineIn==0 && m_bBachAtopStatus[BACH_ATOP_LINEIN0])
    {
        nValue = BachReadReg2Byte(0x112C08);
        nValue &= ~(PGA0_MICA2_GAIN_MSK);
        nValue |= m_nLineInGain[0]<<PGA0_MICA2_GAIN_POS;
        BachWriteReg2Byte(0x112C08, nValue);

        nValue = BachReadReg2Byte(0x112C0A);
        nValue &= ~(PGA0_MICA4_GAIN_MSK);
        nValue |= m_nLineInGain[0]<<PGA0_MICA4_GAIN_POS;
        BachWriteReg2Byte(0x112C0A, nValue);
    }
    else if(nLineIn==1 && m_bBachAtopStatus[BACH_ATOP_LINEIN1])
    {
        nValue = BachReadReg2Byte(0x112C0C);
        nValue &= ~(PGA1_MICA2_GAIN_MSK | PGA1_MICA4_GAIN_MSK);
        nValue |= (m_nLineInGain[1]<<PGA1_MICA2_GAIN_POS | m_nLineInGain[1]<<PGA1_MICA4_GAIN_POS);
        BachWriteReg2Byte(0x112C0C, nValue);
    }

    return TRUE;
}


U16 BachAtopLineInGainGet(U16 nLineIn)
{
    return m_nLineInGain[nLineIn];
}

void BachInitSram(void)
{
    BachWriteReg(BACH_REG_BANK1,BACH_AUDIOBAND_CTRL0,REG_INI_SRAM,REG_INI_SRAM);
    BachWriteReg(BACH_REG_BANK1,BACH_AUDIOBAND_CTRL0,REG_INI_SRAM,0);
}


void BachInitTimeGen(void)
{
    BachRegBank_e eBank;
    U8  nAddr;
    U16 nSynth;

    BachWriteReg(BACH_REG_BANK1,BACH_TIMING_CTRL0,REG_EN_TIME_GEN,0);
    BachWriteReg(BACH_REG_BANK1,BACH_TIMING_CTRL0,REG_EN_TIME_GEN,REG_EN_TIME_GEN);


    for(nSynth = 1; nSynth < BACH_SYNTH_NUM; nSynth++)
    {
        eBank  = (BachRegBank_e)(g_nSynthRegTbl[nSynth][BACH_SYNTH_REG_BANK]);
        nAddr = (U8) (g_nSynthRegTbl[nSynth][BACH_SYNTH_REG_NF_TRIG]);

        BachWriteReg(eBank, nAddr, REG_NF_SYNTH_TRIG, REG_NF_SYNTH_TRIG);	//trigger nf rate register
        BachWriteReg(eBank, nAddr, REG_NF_SYNTH_TRIG, 0);
    }
}


void BachAdcMuxGuard(BachMux4_e eMux4,BOOL bOn)
{
    U16 nAddr;
    U16 nValue;
    switch(eMux4)
    {
    case BACH_MUX4_DEC1_2:
        nAddr = BACH_DIG_MIC_CTRL2;
        nValue = REG_DIG_MIC_SEL1;
        break;
    case BACH_MUX4_DEC3_4:
        nAddr = BACH_DIG_MIC_CTRL3;
        nValue = REG_DIG_MIC_SEL3;
        break;
    case BACH_MUX4_ASRC2DEC1_2:
        nAddr = BACH_DIG_MIC_CTRL2;
        nValue = REG_DIG_MIC_SEL2;
        break;
    case BACH_MUX4_ASRC2DEC3_4:
        nAddr = BACH_DIG_MIC_CTRL3;
        nValue = REG_DIG_MIC_SEL4;
        break;
    default:
        return;
    }
    BachWriteReg(BACH_REG_BANK2, nAddr, nValue, bOn? nValue : 0 );
}

U16 BachAtopMicGainGet(void)
{
    return m_nMicPreGain*3+m_nMicGain - m_nMicInitGain;
}


BOOL BachEarphoneIsInserted(void)
{
    U16 nValue;
    nValue = BachReadReg2Byte(0x0f10);   //pm_gpio 1

    return (nValue & 0x2) ? TRUE: FALSE;
}

void BachEarDetMode(U16 nTrigMode)
{
    U16 polarity_l,polarity_h;
    if(nTrigMode==0) //falling-edge trigger
    {
        polarity_l = BachReadReg2Byte(0x0EF4);
        polarity_h = BachReadReg2Byte(0x0EF6);
        polarity_l |= 0x2;  //PM_GPIO01
        BachWriteReg2Byte(0x0EF4, polarity_l);
        BachWriteReg2Byte(0x0EF6, polarity_h);

    }
    else //rising-edge trigger
    {
        polarity_l = BachReadReg2Byte(0x0EF4);
        polarity_h = BachReadReg2Byte(0x0EF6);
        polarity_l &= ~0x2;  //PM_GPIO01
        BachWriteReg2Byte(0x0EF4, polarity_l);
        BachWriteReg2Byte(0x0EF6, polarity_h);
    }
}



