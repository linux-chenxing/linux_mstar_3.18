#include <linux/types.h>
#include <linux/delay.h>

#include "hal_audio.h"
#include "hal_audio_api.h"
#include "hal_audio_reg.h"
#include "hal_audio_types.h"


static U16 _gnMicPreGain = 0x3; //23dB
static U16 _gnMicInGain = 0x3; //3dB
static U16 _gnLineInGain = 0x2; //0dB

static void MOS_uDelay(U32 u32MicroSeconds)
{
    udelay(u32MicroSeconds);
}

#define USB_SYNTH 0 //SOF mode?
#define NO_HDMI 0 //support HDMI?

static U32 _gnBaseRegAddr = AUD_RIU_BASE_ADDR;
static U32 _gaSynthRateTbl[AUD_RATE_NUM]= {0, 0x5A000000, 0x2D000000, 0x16800000, 0x10539782, 0xF000000, 0x7800000}; //256Fs

static BOOL _gbAnalogIdle;
static BOOL _gbAdcActive;
static BOOL _gbDacActive;
static BOOL _gnAdcSelect;
//static BOOL _gaAtopStatus[AUD_ATOP_NUM];
static AudI2sCfg_t _gaI2sCfg[AUD_I2S_NUM];
static AudDmaClk_t _gaDmaClk[AUD_DMA_NUM];
static BOOL _gbDmaClkBypass[AUD_DMA_NUM];

static AudPdmClkRate_e _geClkRate;

static const U16 _gaMapTable[16][2]= { {AUD_RATE_8K, AUD_PDM_CLK_RATE_400K},
    {AUD_RATE_8K, AUD_PDM_CLK_RATE_800K},
    {AUD_RATE_8K, AUD_PDM_CLK_RATE_1000K},
    {AUD_RATE_8K, AUD_PDM_CLK_RATE_1200K},
    {AUD_RATE_8K, AUD_PDM_CLK_RATE_2000K},
    {AUD_RATE_8K, AUD_PDM_CLK_RATE_2400K},
    {AUD_RATE_8K, AUD_PDM_CLK_RATE_4000K},
    {AUD_RATE_16K, AUD_PDM_CLK_RATE_800K},
    {AUD_RATE_16K, AUD_PDM_CLK_RATE_1600K},
    {AUD_RATE_16K, AUD_PDM_CLK_RATE_2000K},
    {AUD_RATE_16K, AUD_PDM_CLK_RATE_2400K},
    {AUD_RATE_16K, AUD_PDM_CLK_RATE_4000K},
    {AUD_RATE_16K, AUD_PDM_CLK_RATE_4800K},
    {AUD_RATE_16K, AUD_PDM_CLK_RATE_8000K},
    {AUD_RATE_32K, AUD_PDM_CLK_RATE_4000K},
    {AUD_RATE_48K, AUD_PDM_CLK_RATE_4800K}
};

static const U16 _gaMicPreGainTable[] = {
    0b100, //0dB
    0b101, //6dB
    0b000, //13dB
    0b001, //23dB
    0b010, //30dB
    0b011, //36dB
};

#if 0
static const U16 _gaAdcGainTable[][2] = {
    {0, 0b010}, //-6dB
    {0, 0b001}, //-3dB
    {0, 0b000}, //0dB
    {0, 0b011}, //3dB
    {0, 0b100}, //6dB
    {0, 0b101}, //9dB
    {0, 0b110}, //12dB
    {0, 0b111}, //15dB
    {1, 0b110}, //18dB
    {1, 0b111}, //21dB
};
#else
static const U16 _gaLineinGainTable[][2] = {
    {0, 0b001}, //-6dB
    {0, 0b010}, //-3dB
    {0, 0b000}, //0dB
    {0, 0b011}, //3dB
    {0, 0b100}, //6dB
    {0, 0b101}, //9dB
    {0, 0b110}, //12dB
    {0, 0b111}, //15dB
};

static const U16 _gaMicinGainTable[][2] = {
    {0, 0b001}, //-6dB
    {0, 0b010}, //-3dB
    {0, 0b000}, //0dB
    {0, 0b011}, //3dB
    {0, 0b100}, //6dB
    {0, 0b101}, //9dB
    {0, 0b110}, //12dB
    {0, 0b111}, //15dB
    {1, 0b110}, //18dB
    {1, 0b111}, //21dB
};
#endif


void HalAudSysSetBankBaseAddr(U32 nAddr)
{
    _gnBaseRegAddr = nAddr;
}

static void _AudWriteRegByte(U32 nAddr, U8 regMsk, U8 nValue)
{
    U8 nConfigValue;
    nConfigValue = READ_BYTE(_gnBaseRegAddr + ((nAddr) << 1) - ((nAddr) & 1));
    nConfigValue &= ~regMsk;
    nConfigValue |= (nValue);
    WRITE_BYTE(_gnBaseRegAddr + ((nAddr) << 1) - ((nAddr) & 1), nConfigValue);
}

static void _AudWriteReg(AudRegBank_e nBank, U8 nAddr, U16 regMsk, U16 nValue)
{
    U16 nConfigValue;

    switch(nBank)
    {
        case AUD_REG_BANK0:
            nConfigValue = READ_WORD(_gnBaseRegAddr + ((AUD_REG_BANK_0 + (nAddr)) << 1));
            nConfigValue &= ~regMsk;
            nConfigValue |= (nValue & regMsk);
            WRITE_WORD(_gnBaseRegAddr + ((AUD_REG_BANK_0 + (nAddr)) << 1), nConfigValue);
            break;
        case AUD_REG_BANK1:
            nConfigValue = READ_WORD(_gnBaseRegAddr + ((AUD_REG_BANK_1 + (nAddr)) << 1));
            nConfigValue &= ~regMsk;
            nConfigValue |= (nValue & regMsk);
            WRITE_WORD(_gnBaseRegAddr + ((AUD_REG_BANK_1 + (nAddr)) << 1), nConfigValue);
            break;
        case AUD_REG_BANK2:
            nConfigValue = READ_WORD(_gnBaseRegAddr + ((AUD_REG_BANK_2 + (nAddr)) << 1));
            nConfigValue &= ~regMsk;
            nConfigValue |= (nValue & regMsk);
            WRITE_WORD(_gnBaseRegAddr + ((AUD_REG_BANK_2 + (nAddr)) << 1), nConfigValue);
            break;
        case AUD_REG_BANK3:
            nConfigValue = READ_WORD(_gnBaseRegAddr + ((AUD_REG_BANK_3 + (nAddr)) << 1));
            nConfigValue &= ~regMsk;
            nConfigValue |= (nValue & regMsk);
            WRITE_WORD(_gnBaseRegAddr + ((AUD_REG_BANK_3 + (nAddr)) << 1), nConfigValue);
            break;
        case AUD_REG_BANK4:
            nConfigValue = READ_WORD(_gnBaseRegAddr + ((AUD_REG_BANK_4 + (nAddr)) << 1));
            nConfigValue &= ~regMsk;
            nConfigValue |= (nValue & regMsk);
            WRITE_WORD(_gnBaseRegAddr + ((AUD_REG_BANK_4 + (nAddr)) << 1), nConfigValue);
            break;
        case AUD_REG_BANK5:
            nConfigValue = READ_WORD(_gnBaseRegAddr + ((AUD_REG_BANK_5 + (nAddr)) << 1));
            nConfigValue &= ~regMsk;
            nConfigValue |= (nValue & regMsk);
            WRITE_WORD(_gnBaseRegAddr + ((AUD_REG_BANK_5 + (nAddr)) << 1), nConfigValue);
            break;
        default:
            break;
    }
}

U16 _AudReadReg(AudRegBank_e nBank, U8 nAddr)
{
    switch(nBank)
    {
        case AUD_REG_BANK0:
            return READ_WORD(_gnBaseRegAddr + ((AUD_REG_BANK_0 + (nAddr)) << 1));
        case AUD_REG_BANK1:
            return READ_WORD(_gnBaseRegAddr + ((AUD_REG_BANK_1 + (nAddr)) << 1));
        case AUD_REG_BANK2:
            return READ_WORD(_gnBaseRegAddr + ((AUD_REG_BANK_2 + (nAddr)) << 1));
        case AUD_REG_BANK3:
            return READ_WORD(_gnBaseRegAddr + ((AUD_REG_BANK_3 + (nAddr)) << 1));
        case AUD_REG_BANK4:
            return READ_WORD(_gnBaseRegAddr + ((AUD_REG_BANK_4 + (nAddr)) << 1));
        case AUD_REG_BANK5:
            return READ_WORD(_gnBaseRegAddr + ((AUD_REG_BANK_5 + (nAddr)) << 1));
        default:
            return 0;
    }
}


BOOL _HalBtRxNeed256Fs(void)
{
    U16 nConfigValue,nCheckValue;
    nConfigValue = _AudReadReg(AUD_REG_BANK2, REG_SYS_CTRL_05);

    nCheckValue = (0x1<<AUD_MIXTER_BT2DAC_L_POS) |
                  (0x1<<AUD_MIXTER_BT2DAC_R_POS) |
                  (0x1<<AUD_MIXTER_BT2CODEC_L_POS) |
                  (0x1<<AUD_MIXTER_BT2CODEC_R_POS);

    if(nConfigValue & nCheckValue)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }

}

void _HalUpdateDmaWr4Vld(void)
{
    U16 nConfigValue,nCheckValue;
    nConfigValue = _AudReadReg(AUD_REG_BANK2, REG_CLKGEN_CFG_0C);

    nCheckValue = (0x1<<AUD_SEL_CLK_DMA_W4_POS); //0x01 or 0x11

    if(nConfigValue & nCheckValue) //240FS
    {
        //Codec Tx SRC
        nConfigValue = _AudReadReg(AUD_REG_BANK0, REG_AUDIOBAND_CFG_01);
        if(nConfigValue & AUD_DMA_I2S_B_BYPASS)
        {
            _AudWriteReg(AUD_REG_BANK0, REG_VLD_GEN_CFG_3, AUD_VLD_GEN_SRCO_CIC_SEL, AUD_VLD_GEN_SRCO_CIC_SEL);
        }

        //BT Tx SRC
        if(nConfigValue & AUD_DMA_I2S_C_BYPASS)
        {
            _AudWriteReg(AUD_REG_BANK0, REG_VLD_GEN_CFG_4, AUD_VLD_GEN_SRCO_CIC_SEL, AUD_VLD_GEN_SRCO_CIC_SEL);
        }
    }
    else
    {
        //Codec Tx SRC
        _AudWriteReg(AUD_REG_BANK0, REG_VLD_GEN_CFG_3, AUD_VLD_GEN_SRCO_CIC_SEL, 0);

        //BT Tx SRC
        _AudWriteReg(AUD_REG_BANK0, REG_VLD_GEN_CFG_4, AUD_VLD_GEN_SRCO_CIC_SEL, 0);
    }
}

void _HalUpdateDmaRd4Vld(void)
{
    U16 nConfigValue;
    nConfigValue = _AudReadReg(AUD_REG_BANK0, REG_AUDIOBAND_CFG_00);

    if((nConfigValue & AUD_SRC_B_IN_SEL_MSK) == (0x7 << AUD_SRC_B_IN_SEL_POS))
    {
        _AudWriteReg(AUD_REG_BANK2, REG_SYS_CTRL_06, AUD_DMA_R4_VLD_SEL, AUD_DMA_R4_VLD_SEL);
    }
    else
    {
        _AudWriteReg(AUD_REG_BANK2, REG_SYS_CTRL_06, AUD_DMA_R4_VLD_SEL, 0);
    }
}


BOOL HalAudSetMux(AudMux_e eMux, U8 nChoice)
{
    switch(eMux)
    {
        case AUD_MUX2_DMA_W1:
            _AudWriteReg(AUD_REG_BANK2, REG_SYS_CTRL_06, AUD_DMA_W1_IN_SEL, (nChoice ? AUD_DMA_W1_IN_SEL : 0));
            break;
        case AUD_MUX2_DMA_W2: //combine two mux, now we only have SRC mode(0) & 32bit bypass mode(1)
            _AudWriteReg(AUD_REG_BANK0, REG_AUDIOBAND_CFG_01, AUD_SRC_A1_BYPASS, (nChoice ? AUD_SRC_A1_BYPASS : 0));
            _AudWriteReg(AUD_REG_BANK2, REG_SYS_CTRL_06, AUD_SRC_A1_2_BYPASS, (nChoice ? AUD_SRC_A1_2_BYPASS : 0));
            _gbDmaClkBypass[AUD_DMA_WRITER2]=(nChoice ? TRUE:FALSE);
            if(_gbDmaClkBypass[AUD_DMA_WRITER2])
            {
                _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_06, AUD_SEL_CLK_SRC_A1_256FSO_MSK, (0x3<<AUD_SEL_CLK_SRC_A1_256FSO_POS));
            }
            else
            {
                HalAudDmaWr2SetClkRate(_gaDmaClk[AUD_DMA_WRITER2].eSrc, _gaDmaClk[AUD_DMA_WRITER2].eRate);
            }
            break;
        case AUD_MUX2_DMA_W3:
            _AudWriteReg(AUD_REG_BANK0, REG_AUDIOBAND_CFG_01, AUD_SRC_A2_BYPASS, (nChoice ? AUD_SRC_A2_BYPASS : 0));
            _gbDmaClkBypass[AUD_DMA_WRITER3]=(nChoice ? TRUE:FALSE);
            if(_gbDmaClkBypass[AUD_DMA_WRITER3])
            {
                _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_07, AUD_SEL_CLK_SRC_A2_256FSO_MSK, (0x3<<AUD_SEL_CLK_SRC_A2_256FSO_POS));
            }
            else
            {
                HalAudDmaWr3SetClkRate(_gaDmaClk[AUD_DMA_WRITER3].eSrc, _gaDmaClk[AUD_DMA_WRITER3].eRate);
            }
            break;
        case AUD_MUX2_DMA_W4:
            _AudWriteReg(AUD_REG_BANK2, REG_SYS_CTRL_06, AUD_DMA_W4_IN_SEL, (nChoice ? AUD_DMA_W4_IN_SEL : 0));
            break;
        case AUD_MUX4_DMA_W5:
            if(nChoice&0x2)
            {
                _AudWriteReg(AUD_REG_BANK4, REG_VREC_CTRL02, AUD_DMA_W5_SOURCE_SEL, AUD_DMA_W5_SOURCE_SEL);
                _AudWriteReg(AUD_REG_BANK2, REG_SYS_CTRL_06, AUD_DMA_V_IN_SEL, ((nChoice&0x1) ? AUD_DMA_V_IN_SEL : 0));
            }
            else
            {
                _AudWriteReg(AUD_REG_BANK4, REG_VREC_CTRL02, AUD_DMA_W5_SOURCE_SEL, 0);
            }
            break;
        case AUD_MUX2_SRC_B:
            if(nChoice!=0 && nChoice!=7)
                return FALSE;
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_09, AUD_SEL_CLK_SRC_B_256FSI, (nChoice ? AUD_SEL_CLK_SRC_B_256FSI : 0));
            _AudWriteReg(AUD_REG_BANK0, REG_AUDIOBAND_CFG_00, AUD_SRC_B_IN_SEL_MSK, (nChoice<<AUD_SRC_B_IN_SEL_POS)&AUD_SRC_B_IN_SEL_MSK);
            _HalUpdateDmaRd4Vld();
            break;
        case AUD_MUX2_SRC_C:
            if(nChoice!=0 && nChoice!=7)
                return FALSE;
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_0B, AUD_SEL_CLK_SRC_C_256FSI, (nChoice ? AUD_SEL_CLK_SRC_C_256FSI : 0));
            _AudWriteReg(AUD_REG_BANK0, REG_AUDIOBAND_CFG_00, AUD_SRC_C_IN_SEL_MSK, (nChoice<<AUD_SRC_C_IN_SEL_POS)&AUD_SRC_C_IN_SEL_MSK);
            _HalUpdateDmaRd4Vld();
            break;
        case AUD_MUX2_CODEC_I2STX://bypass mode, SRC will change to  DMAW4 automatically
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_09, AUD_SEL_DMA_R2_MSK, (nChoice ? (0x2<<AUD_SEL_DMA_R2_POS) : 0));
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_09, AUD_SEL_CLK_SRC_B_256FSO_MSK, (nChoice ? (0x1<<AUD_SEL_CLK_SRC_B_256FSO_POS) : 0));

            _AudWriteReg(AUD_REG_BANK0, REG_AUDIOBAND_CFG_01, AUD_DMA_I2S_B_BYPASS, (nChoice ? AUD_DMA_I2S_B_BYPASS : 0));
            _AudWriteReg(AUD_REG_BANK0, REG_VLD_GEN_CFG_6, AUD_VLD_GEN_DMA_I2S_B_EN, (nChoice ? AUD_VLD_GEN_DMA_I2S_B_EN : 0));
            _AudWriteReg(AUD_REG_BANK0, REG_VLD_GEN_CFG_6, AUD_VLD_SEL_DMA_R2_SEL, (nChoice ? AUD_VLD_SEL_DMA_R2_SEL : 0));
            _gbDmaClkBypass[AUD_DMA_READER2]=(nChoice ? TRUE:FALSE);
            _HalUpdateDmaWr4Vld();
            break;
        case AUD_MUX2_BT_I2STX://bypass mode, SRC will change to  DMAW4 automatically
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_0B, AUD_SEL_DMA_R3_MSK, (nChoice ? (0x2<<AUD_SEL_DMA_R3_POS) : 0));
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_0B, AUD_SEL_CLK_SRC_C_256FSO_MSK, (nChoice ? (0x1<<AUD_SEL_CLK_SRC_C_256FSO_POS) : 0));

            _AudWriteReg(AUD_REG_BANK0, REG_AUDIOBAND_CFG_01, AUD_DMA_I2S_C_BYPASS, (nChoice ? AUD_DMA_I2S_C_BYPASS : 0));
            _AudWriteReg(AUD_REG_BANK0, REG_VLD_GEN_CFG_6, AUD_VLD_GEN_DMA_I2S_C_EN, (nChoice ? AUD_VLD_GEN_DMA_I2S_C_EN : 0));
            _AudWriteReg(AUD_REG_BANK0, REG_VLD_GEN_CFG_6, AUD_VLD_SEL_DMA_R3_SEL, (nChoice ? AUD_VLD_SEL_DMA_R3_SEL : 0));
            _gbDmaClkBypass[AUD_DMA_READER3]=(nChoice ? TRUE:FALSE);
            _HalUpdateDmaWr4Vld();
            break;
        default:
            return FALSE;
    }
    return TRUE;
}

//0:0, 1:BT, 2:DMA, 3:DMA+BT
BOOL HalAudSetMixer(AudMixer_e eMixer, U8 nChoice)
{
    if(nChoice>3)
    {
        return FALSE;
    }

    switch(eMixer)
    {
        case AUD_MIXER_DAC_L:
            _AudWriteReg(AUD_REG_BANK2, REG_SYS_CTRL_05, AUD_MIXTER_BT2DAC_L_MSK, (nChoice << AUD_MIXTER_BT2DAC_L_POS));
            break;
        case AUD_MIXER_DAC_R:
            _AudWriteReg(AUD_REG_BANK2, REG_SYS_CTRL_05, AUD_MIXTER_BT2DAC_R_MSK, (nChoice << AUD_MIXTER_BT2DAC_R_POS));
            break;
        case AUD_MIXER_CODEC_L:
            _AudWriteReg(AUD_REG_BANK2, REG_SYS_CTRL_05, AUD_MIXTER_BT2CODEC_L_MSK, (nChoice << AUD_MIXTER_BT2CODEC_L_POS));
            break;
        case AUD_MIXER_CODEC_R:
            _AudWriteReg(AUD_REG_BANK2, REG_SYS_CTRL_05, AUD_MIXTER_BT2CODEC_R_MSK, (nChoice << AUD_MIXTER_BT2CODEC_R_POS));
            break;
        default:
            return FALSE;
    }

    //default 240Fs for BT DL path, but it should be 256Fs when mixing with dma reader
    HalAudDmaWr3SetClkRate(_gaDmaClk[AUD_DMA_WRITER3].eSrc, _gaDmaClk[AUD_DMA_WRITER3].eRate);
    return TRUE;
}


BOOL HalAudSetSynthRate(AudSynth_e eSynth, AudRate_e eRate)
{
    U16 nTrigBit,nEnBit;
    U16 nAddrLo,nAddrHi;
    U16 nValue;

    if(eRate >= AUD_RATE_NUM)
        return FALSE;

    switch(eSynth)
    {
        case AUD_SYNTH_CODEC_RX:
            if(eRate==AUD_RATE_SLAVE)
            {
                _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_06, AUD_SEL_CLK_SRC_A1_256FSI_MSK, 1<<AUD_SEL_CLK_SRC_A1_256FSI_POS);
                return TRUE;
            }
            else
            {
                _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_06, AUD_SEL_CLK_SRC_A1_256FSI_MSK, 0<<AUD_SEL_CLK_SRC_A1_256FSI_POS);
            }

            nAddrHi=REG_CODEC_RX_NF_SYNTH_H;
            nAddrLo=REG_CODEC_RX_NF_SYNTH_L;
            nTrigBit=AUD_CODEC_RX_NF_SYNTH_TRIG;
            nEnBit=AUD_CODEC_RX_EN_TIME_GEN;
            break;
        case AUD_SYNTH_CODEC_TX:
            if(eRate==AUD_RATE_SLAVE)
            {
                _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_09, AUD_SEL_CLK_I2S_B_MSK, 1<<AUD_SEL_CLK_I2S_B_POS);
                return TRUE;
            }
            else
            {
                _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_09, AUD_SEL_CLK_I2S_B_MSK, 0<<AUD_SEL_CLK_I2S_B_POS);
            }

            nAddrHi=REG_CODEC_TX_NF_SYNTH_H;
            nAddrLo=REG_CODEC_TX_NF_SYNTH_L;
            nTrigBit=AUD_CODEC_TX_NF_SYNTH_TRIG;
            nEnBit=AUD_CODEC_TX_EN_TIME_GEN;
            break;
        case AUD_SYNTH_BT_RX:
            if(eRate==AUD_RATE_SLAVE)
            {
                _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_07, AUD_SEL_CLK_SRC_A2_256FSI_MSK, 1<<AUD_SEL_CLK_SRC_A2_256FSI_POS);
                return TRUE;
            }
            else
            {
                _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_07, AUD_SEL_CLK_SRC_A2_256FSI_MSK, 0<<AUD_SEL_CLK_SRC_A2_256FSI_POS);
            }

            nAddrHi=REG_BT_RX_NF_SYNTH_H;
            nAddrLo=REG_BT_RX_NF_SYNTH_L;
            nTrigBit=AUD_BT_RX_NF_SYNTH_TRIG;
            nEnBit=AUD_BT_RX_EN_TIME_GEN;
            break;
        case AUD_SYNTH_BT_TX:
            if(eRate==AUD_RATE_SLAVE)
            {
                _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_0B, AUD_SEL_CLK_I2S_C_MSK, 1<<AUD_SEL_CLK_I2S_C_POS);
                return TRUE;
            }
            else
            {
                _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_0B, AUD_SEL_CLK_I2S_C_MSK, 0<<AUD_SEL_CLK_I2S_C_POS);
            }

            nAddrHi=REG_BT_TX_NF_SYNTH_H;
            nAddrLo=REG_BT_TX_NF_SYNTH_L;
            nTrigBit=AUD_BT_TX_NF_SYNTH_TRIG;
            nEnBit=AUD_BT_TX_EN_TIME_GEN;
            break;
        case AUD_SYNTH_USB:
            if(eRate==AUD_RATE_SLAVE)
            {
                return FALSE;
            }

            nAddrHi=REG_USB_NF_SYNTH_H;
            nAddrLo=REG_USB_NF_SYNTH_L;
            nTrigBit=AUD_USB_NF_SYNTH_TRIG;
            nEnBit=AUD_USB_EN_TIME_GEN;
            break;
        case AUD_SYNTH_HDMI:
            if(eRate==AUD_RATE_SLAVE)
            {
                return FALSE;
            }

            nAddrHi=REG_SYS_CTRL_18;
            nAddrLo=REG_SYS_CTRL_17;
            nTrigBit=AUD_HDMI_NF_SYNTH_TRIG;
            nEnBit=AUD_HDMI_EN_TIME_GEN;


            _AudWriteReg(AUD_REG_BANK2, REG_SYS_CTRL_19, nEnBit, nEnBit);

            nValue = (U16)(_gaSynthRateTbl[eRate]>>AUD_NF_VALUE_HI_OFFSET)&AUD_NF_VALUE_HI_MSK;
            _AudWriteReg(AUD_REG_BANK2, nAddrHi, AUD_NF_VALUE_HI_MSK, nValue);
            nValue = (U16)(_gaSynthRateTbl[eRate] & AUD_NF_VALUE_LO_MSK);
            _AudWriteReg(AUD_REG_BANK2, nAddrLo, AUD_NF_VALUE_LO_MSK, nValue);

            _AudWriteReg(AUD_REG_BANK2, REG_SYS_CTRL_18, nTrigBit, nTrigBit);
            _AudWriteReg(AUD_REG_BANK2, REG_SYS_CTRL_18, nTrigBit, 0);
            return TRUE;
        default:
            return FALSE;
    }

    _AudWriteReg(AUD_REG_BANK2, REG_NF_SYNTH_EN_TRIG, nEnBit, nEnBit);

    nValue = (U16)(_gaSynthRateTbl[eRate]>>AUD_NF_VALUE_HI_OFFSET)&AUD_NF_VALUE_HI_MSK;
    _AudWriteReg(AUD_REG_BANK2, nAddrHi, AUD_NF_VALUE_HI_MSK, nValue);
    nValue = (U16)(_gaSynthRateTbl[eRate] & AUD_NF_VALUE_LO_MSK);
    _AudWriteReg(AUD_REG_BANK2, nAddrLo, AUD_NF_VALUE_LO_MSK, nValue);

    _AudWriteReg(AUD_REG_BANK2, REG_NF_SYNTH_EN_TRIG, nTrigBit, nTrigBit);
    _AudWriteReg(AUD_REG_BANK2, REG_NF_SYNTH_EN_TRIG, nTrigBit, 0);

    return TRUE;
}


BOOL HalAudI2sSetTdmMode(AudI2s_e eI2s, AudI2sMode_e eMode)
{
    U16 nTdmMode;

    switch(eMode)
    {
        case AUD_I2S_MODE_I2S:
            nTdmMode=0;
            break;
        case AUD_I2S_MODE_TDM:
            nTdmMode=1;
            break;
        default:
            return FALSE;
    }

    switch(eI2s)
    {
        case AUD_I2S_MISC_RX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_04, AUD_I2S_RX_TDM_MODE, (nTdmMode?AUD_I2S_RX_TDM_MODE:0));
            break;

        case AUD_I2S_MISC_TX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_05, AUD_I2S_TX_TDM_MODE, (nTdmMode?AUD_I2S_TX_TDM_MODE:0));
            break;

        case AUD_I2S_CODEC_RX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_00, AUD_I2S_RX_TDM_MODE, (nTdmMode?AUD_I2S_RX_TDM_MODE:0));
            break;

        case AUD_I2S_CODEC_TX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_01, AUD_I2S_TX_TDM_MODE, (nTdmMode?AUD_I2S_TX_TDM_MODE:0));
            break;

        case AUD_I2S_BT_RX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_02, AUD_I2S_RX_TDM_MODE, (nTdmMode?AUD_I2S_RX_TDM_MODE:0));
            break;

        case AUD_I2S_BT_TX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_03, AUD_I2S_TX_TDM_MODE, (nTdmMode?AUD_I2S_TX_TDM_MODE:0));
            break;

        default:
            return FALSE;
    }

    _gaI2sCfg[eI2s].eMode = eMode;
    return TRUE;
}


BOOL HalAudI2sSetMsMode(AudI2s_e eI2s, AudI2sMsMode_e eMsMode)
{
    U16 nMsMode;

    switch(eMsMode)
    {
        case AUD_I2S_MSMODE_MASTER:
            nMsMode=1;
            break;
        case AUD_I2S_MSMODE_SLAVE:
            nMsMode=0;
            break;
        default:
            return FALSE;
    }

    switch(eI2s)
    {
        case AUD_I2S_MISC_RX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_04, AUD_I2S_RX_MS_MODE, (nMsMode?AUD_I2S_RX_MS_MODE:0));
            break;

        case AUD_I2S_MISC_TX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_05, AUD_I2S_TX_MS_MODE, (nMsMode?AUD_I2S_TX_MS_MODE:0));
            break;

        case AUD_I2S_CODEC_RX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_00, AUD_I2S_RX_MS_MODE, (nMsMode?AUD_I2S_RX_MS_MODE:0));
            break;

        case AUD_I2S_CODEC_TX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_01, AUD_I2S_TX_MS_MODE, (nMsMode?AUD_I2S_TX_MS_MODE:0));
            break;

        case AUD_I2S_BT_RX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_02, AUD_I2S_RX_MS_MODE, (nMsMode?AUD_I2S_RX_MS_MODE:0));
            break;

        case AUD_I2S_BT_TX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_03, AUD_I2S_TX_MS_MODE, (nMsMode?AUD_I2S_TX_MS_MODE:0));
            break;

        default:
            return FALSE;
    }

    _gaI2sCfg[eI2s].eMsMode = eMsMode;
    return TRUE;
}

BOOL HalAudI2sSetFmt(AudI2s_e eI2s, AudI2sFmt_e eFmt)
{
    U16 nSel;
    switch(eFmt)
    {
        case AUD_I2S_FMT_I2S:
            nSel=0;
            break;
        case AUD_I2S_FMT_LEFT_JUSTIFY:
            nSel=1;
            break;
        default:
            return FALSE;
    }

    switch(eI2s)
    {
        case AUD_I2S_MISC_RX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_04, AUD_I2S_RX_ENC_FMT, (nSel?AUD_I2S_RX_ENC_FMT:0));
            break;

        case AUD_I2S_MISC_TX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_05, AUD_I2S_TX_ENC_FMT, (nSel?AUD_I2S_TX_ENC_FMT:0));
            break;

        case AUD_I2S_CODEC_RX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_00, AUD_I2S_RX_ENC_FMT, (nSel?AUD_I2S_RX_ENC_FMT:0));
            break;

        case AUD_I2S_CODEC_TX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_01, AUD_I2S_TX_ENC_FMT, (nSel?AUD_I2S_TX_ENC_FMT:0));
            break;

        case AUD_I2S_BT_RX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_02, AUD_I2S_RX_ENC_FMT, (nSel?AUD_I2S_RX_ENC_FMT:0));
            break;

        case AUD_I2S_BT_TX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_03, AUD_I2S_TX_ENC_FMT, (nSel?AUD_I2S_TX_ENC_FMT:0));
            break;
        default:
            return FALSE;
    }
    _gaI2sCfg[eI2s].eFormat = eFmt;

    return TRUE;
}

BOOL HalAudI2sSetWidth(AudI2s_e eI2s, AudBitWidth_e eWidth)
{
    U16 nSel;
    switch(eWidth)
    {
        case AUD_BITWIDTH_16:
            nSel=0;
            break;
        case AUD_BITWIDTH_32:
            nSel=1;
            break;
        default:
            return FALSE;
    }

    switch(eI2s)
    {
        case AUD_I2S_MISC_RX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_04, AUD_I2S_RX_BIT_WIDTH, (nSel?AUD_I2S_RX_BIT_WIDTH:0));
            break;

        case AUD_I2S_MISC_TX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_05, AUD_I2S_TX_BIT_WIDTH, (nSel?AUD_I2S_TX_BIT_WIDTH:0));
            break;

        case AUD_I2S_CODEC_RX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_00, AUD_I2S_RX_BIT_WIDTH, (nSel?AUD_I2S_RX_BIT_WIDTH:0));
            break;

        case AUD_I2S_CODEC_TX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_01, AUD_I2S_TX_BIT_WIDTH, (nSel?AUD_I2S_TX_BIT_WIDTH:0));
            break;

        case AUD_I2S_BT_RX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_02, AUD_I2S_RX_BIT_WIDTH, (nSel?AUD_I2S_RX_BIT_WIDTH:0));
            break;

        case AUD_I2S_BT_TX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_03, AUD_I2S_TX_BIT_WIDTH, (nSel?AUD_I2S_TX_BIT_WIDTH:0));
            break;

        default:
            return FALSE;
    }
    _gaI2sCfg[eI2s].eWidth = eWidth;
    return TRUE;
}

BOOL HalAudI2sSetTdmChannel(AudI2s_e eI2s, U16 nChannel)
{
    if(nChannel!=4 && nChannel!=8)
        return FALSE;

    switch(eI2s)
    {
        case AUD_I2S_MISC_RX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_04, AUD_I2S_RX_TDM_CHN_WIDTH, (nChannel==8?AUD_I2S_RX_TDM_CHN_WIDTH:0));
            break;

        case AUD_I2S_MISC_TX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_05, AUD_I2S_TX_TDM_CHN_WIDTH, (nChannel==8?AUD_I2S_TX_TDM_CHN_WIDTH:0));
            break;

        case AUD_I2S_CODEC_RX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_00, AUD_I2S_RX_TDM_CHN_WIDTH, (nChannel==8?AUD_I2S_RX_TDM_CHN_WIDTH:0));
            break;

        case AUD_I2S_CODEC_TX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_01, AUD_I2S_TX_TDM_CHN_WIDTH, (nChannel==8?AUD_I2S_TX_TDM_CHN_WIDTH:0));
            break;

        case AUD_I2S_BT_RX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_02, AUD_I2S_RX_TDM_CHN_WIDTH, (nChannel==8?AUD_I2S_RX_TDM_CHN_WIDTH:0));
            break;

        case AUD_I2S_BT_TX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_03, AUD_I2S_TX_TDM_CHN_WIDTH, (nChannel==8?AUD_I2S_TX_TDM_CHN_WIDTH:0));
            break;

        default:
            return FALSE;
    }

    _gaI2sCfg[eI2s].nTdmChannel = nChannel;
    return TRUE;
}


//for RX slot mask if 0xFF
BOOL HalAudI2sSetTdmSlotConfig(AudI2s_e eI2s, U16 nSlotMsk, AudTdmChnMap_e eMap)
{
    switch(eI2s)
    {
        case AUD_I2S_MISC_TX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_09, AUD_I2S_TX_CHN_SWAP_MSK, (eMap<<AUD_I2S_TX_CHN_SWAP_POS));
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_09, AUD_I2S_TX_ACTIVE_SLOT_MSK, (nSlotMsk<<AUD_I2S_TX_ACTIVE_SLOT_POS));
            break;
        case AUD_I2S_MISC_RX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_0D, AUD_MISC_I2S_RX_CHN_SWAP_MSK, (eMap<<AUD_MISC_I2S_RX_CHN_SWAP_POS));
            break;
        case AUD_I2S_CODEC_TX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_07, AUD_I2S_TX_CHN_SWAP_MSK, (eMap<<AUD_I2S_TX_CHN_SWAP_POS));
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_07, AUD_I2S_TX_ACTIVE_SLOT_MSK, (nSlotMsk<<AUD_I2S_TX_ACTIVE_SLOT_POS));
            break;

        case AUD_I2S_CODEC_RX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_0D, AUD_CODEC_I2S_RX_CHN_SWAP_MSK, (eMap<<AUD_CODEC_I2S_RX_CHN_SWAP_POS));
            break;

        case AUD_I2S_BT_TX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_08, AUD_I2S_TX_CHN_SWAP_MSK, (eMap<<AUD_I2S_TX_CHN_SWAP_POS));
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_08, AUD_I2S_TX_ACTIVE_SLOT_MSK, (nSlotMsk<<AUD_I2S_TX_ACTIVE_SLOT_POS));
            break;

        case AUD_I2S_BT_RX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_0D, AUD_BT_I2S_RX_CHN_SWAP_MSK, (eMap<<AUD_BT_I2S_RX_CHN_SWAP_POS));
            break;
        default:
            return FALSE;
    }
    return TRUE;
}

//0xFFFF for duty 50
BOOL HalAudI2sSetWckWidth(AudI2s_e eI2s, U16 nWidth)
{
    U16 nAddr0,nAddr1;
    U16 nProgramBit;
    U16 nMask,nPos;
    switch(eI2s)
    {
        case AUD_I2S_MISC_TX:
            nAddr0 = REG_I2S_TDM_CFG_05;
            nAddr1 = REG_I2S_TDM_CFG_09;
            nProgramBit = AUD_I2S_TX_WCK_FMT;
            nMask = AUD_I2S_TX_WCK_WIDTH_MSK;
            nPos = AUD_I2S_TX_WCK_WIDTH_POS;
            break;
        case AUD_I2S_MISC_RX:
            nAddr0 = REG_I2S_TDM_CFG_04;
            nAddr1 = REG_I2S_TDM_CFG_06;
            nProgramBit = AUD_I2S_RX_WCK_FMT;
            nMask = AUD_MISC_I2S_RX_WCK_WIDTH_MSK;
            nPos = AUD_MISC_I2S_RX_WCK_WIDTH_POS;
            break;
        case AUD_I2S_CODEC_TX:
            nAddr0 = REG_I2S_TDM_CFG_01;
            nAddr1 = REG_I2S_TDM_CFG_07;
            nProgramBit = AUD_I2S_TX_WCK_FMT;
            nMask = AUD_I2S_TX_WCK_WIDTH_MSK;
            nPos = AUD_I2S_TX_WCK_WIDTH_POS;
            break;

        case AUD_I2S_CODEC_RX:
            nAddr0 = REG_I2S_TDM_CFG_00;
            nAddr1 = REG_I2S_TDM_CFG_06;
            nProgramBit = AUD_I2S_RX_WCK_FMT;
            nMask = AUD_CODEC_I2S_RX_WCK_WIDTH_MSK;
            nPos = AUD_CODEC_I2S_RX_WCK_WIDTH_POS;
            break;

        case AUD_I2S_BT_TX:
            nAddr0 = REG_I2S_TDM_CFG_03;
            nAddr1 = REG_I2S_TDM_CFG_08;
            nProgramBit = AUD_I2S_TX_WCK_FMT;
            nMask = AUD_I2S_TX_WCK_WIDTH_MSK;
            nPos = AUD_I2S_TX_WCK_WIDTH_POS;
            break;

        case AUD_I2S_BT_RX:
            nAddr0 = REG_I2S_TDM_CFG_02;
            nAddr1 = REG_I2S_TDM_CFG_06;
            nProgramBit = AUD_I2S_RX_WCK_FMT;
            nMask = AUD_BT_I2S_RX_WCK_WIDTH_MSK;
            nPos = AUD_BT_I2S_RX_WCK_WIDTH_POS;
            break;
        default:
            return FALSE;
    }

    if(nWidth==0xFFFF)
    {
        _AudWriteReg(AUD_REG_BANK2, nAddr0, nProgramBit, 0);
    }
    else
    {
        _AudWriteReg(AUD_REG_BANK2, nAddr0, nProgramBit, nProgramBit);
        _AudWriteReg(AUD_REG_BANK2, nAddr1, nMask, ((nWidth<<nPos)&nMask));
    }
    return TRUE;
}


BOOL _HalAudI2sSetBck(AudI2s_e eI2s, AudI2sBck_e eBck)
{
    switch(eI2s)
    {
        case AUD_I2S_MISC_RX:
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_03, AUD_SEL_MISC_I2S_RX_BCK_FS_MSK, (eBck<<AUD_SEL_MISC_I2S_RX_BCK_FS_POS));
            break;

        case AUD_I2S_MISC_TX:
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_03, AUD_SEL_MISC_I2S_TX_BCK_FS_MSK, (eBck<<AUD_SEL_MISC_I2S_TX_BCK_FS_POS));
            break;

        case AUD_I2S_CODEC_RX:
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_03, AUD_SEL_CODEC_I2S_RX_BCK_FS_MSK, (eBck<<AUD_SEL_CODEC_I2S_RX_BCK_FS_POS));
            break;

        case AUD_I2S_CODEC_TX:
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_03, AUD_SEL_CODEC_I2S_TX_BCK_FS_MSK, (eBck<<AUD_SEL_CODEC_I2S_TX_BCK_FS_POS));
            break;

        case AUD_I2S_BT_RX:
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_03, AUD_SEL_BT_I2S_RX_BCK_FS_MSK, (eBck<<AUD_SEL_BT_I2S_RX_BCK_FS_POS));
            break;

        case AUD_I2S_BT_TX:
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_03, AUD_SEL_BT_I2S_TX_BCK_FS_MSK, (eBck<<AUD_SEL_BT_I2S_TX_BCK_FS_POS));
            break;
        default:
            return FALSE;
    }
    return TRUE;

}

U16 _HalAudI2sGetBits(AudI2s_e eI2s)
{
    U16 nBits;
    switch(_gaI2sCfg[eI2s].eWidth)
    {
        case AUD_BITWIDTH_16:
            nBits = 16;
            break;
        case AUD_BITWIDTH_24:
        case AUD_BITWIDTH_32:
            nBits = 32;
            break;
        default:
            return 0;
    }

    switch(_gaI2sCfg[eI2s].eMode)
    {
        case AUD_I2S_MODE_I2S:
            nBits *= 2;
            break;
        case AUD_I2S_MODE_TDM:
            nBits *= _gaI2sCfg[eI2s].nTdmChannel;
            break;
        default:
            return 0;
    }

    return nBits;
}

//for I2S master
BOOL HalAudI2sEnable(AudI2s_e eI2s, BOOL bEnable)
{

    if(bEnable)
    {
        U16 nBits = _HalAudI2sGetBits(eI2s);
        switch(nBits)
        {
            case 32:
                _HalAudI2sSetBck(eI2s, AUD_I2S_BCK_32FS);
                break;
            case 64:
                _HalAudI2sSetBck(eI2s, AUD_I2S_BCK_64FS);
                break;
            case 128:
                _HalAudI2sSetBck(eI2s, AUD_I2S_BCK_128FS);
                break;
            case 256:
                _HalAudI2sSetBck(eI2s, AUD_I2S_BCK_256FS);
                break;
            default:
                return FALSE;
        }
    }

    switch(eI2s)
    {
        case AUD_I2S_MISC_RX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_04, AUD_I2S_RX_BCK_GEN_EN, (bEnable?AUD_I2S_RX_BCK_GEN_EN:0));
            break;

        case AUD_I2S_MISC_TX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_05, AUD_I2S_TX_BCK_GEN_EN, (bEnable?AUD_I2S_TX_BCK_GEN_EN:0));
            break;

        case AUD_I2S_CODEC_RX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_00, AUD_I2S_RX_BCK_GEN_EN, (bEnable?AUD_I2S_RX_BCK_GEN_EN:0));
            break;

        case AUD_I2S_CODEC_TX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_01, AUD_I2S_TX_BCK_GEN_EN, (bEnable?AUD_I2S_TX_BCK_GEN_EN:0));
            break;

        case AUD_I2S_BT_RX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_02, AUD_I2S_RX_BCK_GEN_EN, (bEnable?AUD_I2S_RX_BCK_GEN_EN:0));
            break;

        case AUD_I2S_BT_TX:
            _AudWriteReg(AUD_REG_BANK2, REG_I2S_TDM_CFG_03, AUD_I2S_TX_BCK_GEN_EN, (bEnable?AUD_I2S_TX_BCK_GEN_EN:0));
            break;
        default:
            return FALSE;
    }
    return TRUE;
}

BOOL HalAudDmaReset(AudDmaChn_e eDmaChannel)
{
    AudRegBank_e eBank;
    U8 nAddr0,nAddr1;
    switch(eDmaChannel)
    {
        case AUD_DMA_WRITER1:
            eBank=AUD_REG_BANK3;
            nAddr0=REG_AUDDMA_W1_CFG04;
            nAddr1=REG_AUDDMA_W1_CFG00;
            break;
        case AUD_DMA_WRITER2:
            eBank=AUD_REG_BANK3;
            nAddr0=REG_AUDDMA_W2_CFG04;
            nAddr1=REG_AUDDMA_W2_CFG00;
            break;
        case AUD_DMA_WRITER3:
            eBank=AUD_REG_BANK3;
            nAddr0=REG_AUDDMA_W3_CFG04;
            nAddr1=REG_AUDDMA_W3_CFG00;
            break;
        case AUD_DMA_WRITER4:
            eBank=AUD_REG_BANK3;
            nAddr0=REG_AUDDMA_W4_CFG04;
            nAddr1=REG_AUDDMA_W4_CFG00;
            break;
        case AUD_DMA_WRITER5:
            eBank=AUD_REG_BANK4;
            nAddr0=REG_AUDDMA_W5_CFG04;
            nAddr1=REG_AUDDMA_W5_CFG00;
            break;
        case AUD_DMA_READER1:
            eBank=AUD_REG_BANK3;
            nAddr0=REG_AUDDMA_R1_CFG04;
            nAddr1=REG_AUDDMA_R1_CFG00;
            break;
        case AUD_DMA_READER2:
            eBank=AUD_REG_BANK3;
            nAddr0=REG_AUDDMA_R2_CFG04;
            nAddr1=REG_AUDDMA_R2_CFG00;
            break;
        case AUD_DMA_READER3:
            eBank=AUD_REG_BANK3;
            nAddr0=REG_AUDDMA_R3_CFG04;
            nAddr1=REG_AUDDMA_R3_CFG00;
            break;
        case AUD_DMA_READER4:
            eBank=AUD_REG_BANK3;
            nAddr0=REG_AUDDMA_R4_CFG04;
            nAddr1=REG_AUDDMA_R4_CFG00;
            break;
        case AUD_DMA_READER5:
            eBank=AUD_REG_BANK5;
            nAddr0=REG_AUDDMA_R5_CFG04;
            nAddr1=REG_AUDDMA_R5_CFG00;
            break;
        default:
            return FALSE;

    }

    if(eDmaChannel < AUD_DMA_READER1)//dma writer
    {
        _AudWriteReg(eBank,nAddr0,AUD_DMA_WR_SIZE_MSK,0);

        _AudWriteReg(eBank,nAddr1,AUD_DMA_WR_RESET,AUD_DMA_WR_RESET);
        _AudWriteReg(eBank,nAddr1,AUD_DMA_WR_RESET,0);

        _AudWriteReg(eBank,nAddr1,AUD_DMA_WR_FREE_RUN|AUD_DMA_WR_MIU_HIGH_PRIORITY,AUD_DMA_WR_MIU_HIGH_PRIORITY);
    }
    else//dma reader
    {
        _AudWriteReg(eBank,nAddr0,AUD_DMA_RD_SIZE_MSK,0);

        _AudWriteReg(eBank,nAddr1,AUD_DMA_RD_RESET,AUD_DMA_RD_RESET);
        _AudWriteReg(eBank,nAddr1,AUD_DMA_RD_RESET,0);

        _AudWriteReg(eBank,nAddr1,AUD_DMA_RD_FREE_RUN|AUD_DMA_RD_MIU_HIGH_PRIORITY,AUD_DMA_RD_MIU_HIGH_PRIORITY);
    }
    return TRUE;

}


BOOL HalAudDmaEnable(AudDmaChn_e eDmaChannel, BOOL bEnable)
{
    AudRegBank_e eBank;
    U8 nAddr;
    switch(eDmaChannel)
    {
        case AUD_DMA_WRITER1:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W1_CFG00;
            break;
        case AUD_DMA_WRITER2:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W2_CFG00;
            break;
        case AUD_DMA_WRITER3:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W3_CFG00;
            break;
        case AUD_DMA_WRITER4:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W4_CFG00;
            break;
        case AUD_DMA_WRITER5:
            eBank=AUD_REG_BANK4;
            nAddr=REG_AUDDMA_W5_CFG00;
            break;
        case AUD_DMA_READER1:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_R1_CFG00;
            break;
        case AUD_DMA_READER2:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_R2_CFG00;
            break;
        case AUD_DMA_READER3:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_R3_CFG00;
            break;
        case AUD_DMA_READER4:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_R4_CFG00;
            break;
        case AUD_DMA_READER5:
            eBank=AUD_REG_BANK5;
            nAddr=REG_AUDDMA_R5_CFG00;
            _AudWriteReg(AUD_REG_BANK5,REG_SYS2_CTRL_02,AUD_DMA_WR5_MCH_ENABLE,(bEnable?AUD_DMA_WR5_MCH_ENABLE:0)); //mch enable should be enabled synchronously
            break;
        default:
            return FALSE;

    }

    if(eDmaChannel < AUD_DMA_READER1)//dma writer
    {
        if(bEnable)
        {
            _AudWriteReg(eBank,nAddr,AUD_DMA_WR_MIU_ENABLE,AUD_DMA_WR_MIU_ENABLE);
            _AudWriteReg(eBank,nAddr,AUD_DMA_WR_ENABLE,AUD_DMA_WR_ENABLE);
        }
        else
        {
            _AudWriteReg(eBank,nAddr,AUD_DMA_WR_MIU_ENABLE,0);
            _AudWriteReg(eBank,nAddr,AUD_DMA_WR_ENABLE,0);
        }
    }
    else//dma reader
    {
        if(bEnable)
        {
            _AudWriteReg(eBank,nAddr,AUD_DMA_RD_MIU_ENABLE,AUD_DMA_RD_MIU_ENABLE);
            MOS_uDelay(5);
            _AudWriteReg(eBank,nAddr,AUD_DMA_RD_ENABLE,AUD_DMA_RD_ENABLE);
        }
        else
        {
            _AudWriteReg(eBank,nAddr,AUD_DMA_RD_ENABLE,0);
            _AudWriteReg(eBank,nAddr,AUD_DMA_RD_MIU_ENABLE,0);
        }
    }
    return TRUE;
}

BOOL HalAudDmaPause(AudDmaChn_e eDmaChannel)
{
    AudRegBank_e eBank;
    U8 nAddr;
    switch(eDmaChannel)
    {
        case AUD_DMA_WRITER1:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W1_CFG00;
            break;
        case AUD_DMA_WRITER2:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W2_CFG00;
            break;
        case AUD_DMA_WRITER3:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W3_CFG00;
            break;
        case AUD_DMA_WRITER4:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W4_CFG00;
            break;
        case AUD_DMA_WRITER5:
            eBank=AUD_REG_BANK4;
            nAddr=REG_AUDDMA_W5_CFG00;
            break;
        case AUD_DMA_READER1:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_R1_CFG00;
            break;
        case AUD_DMA_READER2:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_R2_CFG00;
            break;
        case AUD_DMA_READER3:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_R3_CFG00;
            break;
        case AUD_DMA_READER4:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_R4_CFG00;
            break;
        case AUD_DMA_READER5:
            eBank=AUD_REG_BANK5;
            nAddr=REG_AUDDMA_R5_CFG00;
            break;
        default:
            return FALSE;

    }

    if(eDmaChannel < AUD_DMA_READER1)//dma writer
    {
        _AudWriteReg(eBank,nAddr,AUD_DMA_WR_ENABLE,0);
    }
    else//dma reader
    {
        _AudWriteReg(eBank,nAddr,AUD_DMA_RD_ENABLE,0);
    }
    return TRUE;
}

BOOL HalAudDmaResume(AudDmaChn_e eDmaChannel)
{
    AudRegBank_e eBank;
    U8 nAddr;
    switch(eDmaChannel)
    {
        case AUD_DMA_WRITER1:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W1_CFG00;
            break;
        case AUD_DMA_WRITER2:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W2_CFG00;
            break;
        case AUD_DMA_WRITER3:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W3_CFG00;
            break;
        case AUD_DMA_WRITER4:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W4_CFG00;
            break;
        case AUD_DMA_WRITER5:
            eBank=AUD_REG_BANK4;
            nAddr=REG_AUDDMA_W5_CFG00;
            break;
        case AUD_DMA_READER1:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_R1_CFG00;
            break;
        case AUD_DMA_READER2:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_R2_CFG00;
            break;
        case AUD_DMA_READER3:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_R3_CFG00;
            break;
        case AUD_DMA_READER4:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_R4_CFG00;
            break;
        case AUD_DMA_READER5:
            eBank=AUD_REG_BANK5;
            nAddr=REG_AUDDMA_R5_CFG00;
            break;
        default:
            return FALSE;

    }

    if(eDmaChannel < AUD_DMA_READER1)//dma writer
    {
        _AudWriteReg(eBank,nAddr,AUD_DMA_WR_ENABLE,AUD_DMA_WR_ENABLE);
    }
    else//dma reader
    {
        _AudWriteReg(eBank,nAddr,AUD_DMA_RD_ENABLE,AUD_DMA_RD_ENABLE);
    }
    return TRUE;
}

BOOL _HalAudDmaRdIntEnable(AudDmaChn_e eDmaChannel, BOOL bUnderrun, BOOL bEmpty, BOOL bLocalEmpty)
{
    AudRegBank_e eBank;
    U8 nAddr;
    U16 nConfigValue=(AUD_DMA_RD_EMPTY_INT_MASK|AUD_DMA_RD_UNDERRUN_INT_MASK|AUD_DMA_RD_LOCALBUF_EMPTY_INT_MASK);
    switch(eDmaChannel)
    {
        case AUD_DMA_READER1:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_R1_CFG08;
            break;
        case AUD_DMA_READER2:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_R2_CFG08;
            break;
        case AUD_DMA_READER3:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_R3_CFG08;
            break;
        case AUD_DMA_READER4:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_R4_CFG08;
            break;
        case AUD_DMA_READER5:
            eBank=AUD_REG_BANK5;
            nAddr=REG_AUDDMA_R5_CFG08;
            break;
        default:
            return FALSE;

    }

    if(bEmpty)
    {
        nConfigValue &= ~AUD_DMA_RD_EMPTY_INT_MASK;
    }

    if(bUnderrun)
    {
        nConfigValue &= ~AUD_DMA_RD_UNDERRUN_INT_MASK;
    }

    if(bLocalEmpty)
    {
        nConfigValue &= ~AUD_DMA_RD_LOCALBUF_EMPTY_INT_MASK;
    }


    _AudWriteReg(eBank,nAddr,(AUD_DMA_RD_EMPTY_INT_MASK|AUD_DMA_RD_UNDERRUN_INT_MASK|AUD_DMA_RD_LOCALBUF_EMPTY_INT_MASK),nConfigValue);
    return TRUE;
}

BOOL _HalAudDmaWrIntEnable(AudDmaChn_e eDmaChannel, BOOL bOverrun, BOOL bFull, BOOL bLocalFull)
{
    U8 nAddr;
    AudRegBank_e eBank;
    U16 nConfigValue=(AUD_DMA_WR_FULL_INT_MASK|AUD_DMA_WR_OVERRUN_INT_MASK|AUD_DMA_WR_LOCALBUF_FULL_INT_MASK);
    switch(eDmaChannel)
    {
        case AUD_DMA_WRITER1:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W1_CFG08;
            break;
        case AUD_DMA_WRITER2:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W2_CFG08;
            break;
        case AUD_DMA_WRITER3:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W3_CFG08;
            break;
        case AUD_DMA_WRITER4:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W4_CFG08;
            break;
        case AUD_DMA_WRITER5:
            eBank=AUD_REG_BANK4;
            nAddr=REG_AUDDMA_W5_CFG08;
            break;
        default:
            return FALSE;

    }

    if(bFull)
    {
        nConfigValue &= ~AUD_DMA_WR_FULL_INT_MASK;
    }

    if(bOverrun)
    {
        nConfigValue &= ~AUD_DMA_WR_OVERRUN_INT_MASK;
    }

    if(bLocalFull)
    {
        nConfigValue &= ~AUD_DMA_WR_LOCALBUF_FULL_INT_MASK;
    }


    _AudWriteReg(eBank,nAddr,(AUD_DMA_WR_FULL_INT_MASK|AUD_DMA_WR_OVERRUN_INT_MASK|AUD_DMA_WR_LOCALBUF_FULL_INT_MASK),nConfigValue);
    return TRUE;
}

BOOL HalAudDmaIntEnable(AudDmaChn_e eDmaChannel, BOOL bDatatrigger, BOOL bDataboundary, BOOL bLocalData)
{
    BOOL ret;
    if (eDmaChannel <= AUD_DMA_WRITER5)
        ret = _HalAudDmaWrIntEnable(eDmaChannel, bDatatrigger, bDataboundary, bLocalData);
    else
        ret = _HalAudDmaRdIntEnable(eDmaChannel, bDatatrigger, bDataboundary, bLocalData);

    return ret;
}

BOOL HalAudDmaClearInt(AudDmaChn_e eDmaChannel)
{
    AudRegBank_e eBank;
    U8 nAddr;
    switch(eDmaChannel)
    {
        case AUD_DMA_WRITER1:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W1_CFG00;
            break;
        case AUD_DMA_WRITER2:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W2_CFG00;
            break;
        case AUD_DMA_WRITER3:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W3_CFG00;
            break;
        case AUD_DMA_WRITER4:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W4_CFG00;
            break;
        case AUD_DMA_WRITER5:
            eBank=AUD_REG_BANK4;
            nAddr=REG_AUDDMA_W5_CFG00;
            break;
        case AUD_DMA_READER1:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_R1_CFG00;
            break;
        case AUD_DMA_READER2:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_R2_CFG00;
            break;
        case AUD_DMA_READER3:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_R3_CFG00;
            break;
        case AUD_DMA_READER4:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_R4_CFG00;
            break;
        case AUD_DMA_READER5:
            eBank=AUD_REG_BANK5;
            nAddr=REG_AUDDMA_R5_CFG00;
            break;
        default:
            return FALSE;

    }

    if(eDmaChannel < AUD_DMA_READER1)//dma writer
    {
        _AudWriteReg(eBank,nAddr,(AUD_DMA_WR_LOCAL_FULL_FLAG_CLR|AUD_DMA_WR_FULL_FLAG_CLR),(AUD_DMA_WR_LOCAL_FULL_FLAG_CLR|AUD_DMA_WR_FULL_FLAG_CLR));
        _AudWriteReg(eBank,nAddr,(AUD_DMA_WR_LOCAL_FULL_FLAG_CLR|AUD_DMA_WR_FULL_FLAG_CLR),0);
    }
    else//dma reader
    {
        _AudWriteReg(eBank,nAddr,(AUD_DMA_RD_LOCAL_EMPTY_FLAG_CLR|AUD_DMA_RD_EMPTY_FLAG_CLR),(AUD_DMA_RD_LOCAL_EMPTY_FLAG_CLR|AUD_DMA_RD_EMPTY_FLAG_CLR));
        _AudWriteReg(eBank,nAddr,(AUD_DMA_RD_LOCAL_EMPTY_FLAG_CLR|AUD_DMA_RD_EMPTY_FLAG_CLR),0);
    }
    return TRUE;
}

//get dram level count
U32 HalAudDmaGetLevelCnt(AudDmaChn_e eDmaChannel)
{
    AudRegBank_e eBank;
    U8 nAddr0,nAddr1;
    U16 nValue0,nValue1;//,nValue2;
    U16 nCount=0;
    U32 nByteSize;
    switch(eDmaChannel)
    {
        case AUD_DMA_WRITER1:
            eBank=AUD_REG_BANK3;
            nAddr0=REG_AUDDMA_W1_CFG00;
            nAddr1=REG_AUDDMA_W1_STS00;
            break;
        case AUD_DMA_WRITER2:
            eBank=AUD_REG_BANK3;
            nAddr0=REG_AUDDMA_W2_CFG00;
            nAddr1=REG_AUDDMA_W2_STS00;
            break;
        case AUD_DMA_WRITER3:
            eBank=AUD_REG_BANK3;
            nAddr0=REG_AUDDMA_W3_CFG00;
            nAddr1=REG_AUDDMA_W3_STS00;
            break;
        case AUD_DMA_WRITER4:
            eBank=AUD_REG_BANK3;
            nAddr0=REG_AUDDMA_W4_CFG00;
            nAddr1=REG_AUDDMA_W4_STS00;
            break;
        case AUD_DMA_WRITER5:
            eBank=AUD_REG_BANK4;
            nAddr0=REG_AUDDMA_W5_CFG00;
            nAddr1=REG_AUDDMA_W5_STS00;
            break;
        case AUD_DMA_READER1:
            eBank=AUD_REG_BANK3;
            nAddr0=REG_AUDDMA_R1_CFG00;
            nAddr1=REG_AUDDMA_R1_STS00;
            break;
        case AUD_DMA_READER2:
            eBank=AUD_REG_BANK3;
            nAddr0=REG_AUDDMA_R2_CFG00;
            nAddr1=REG_AUDDMA_R2_STS00;
            break;
        case AUD_DMA_READER3:
            eBank=AUD_REG_BANK3;
            nAddr0=REG_AUDDMA_R3_CFG00;
            nAddr1=REG_AUDDMA_R3_STS00;
            break;
        case AUD_DMA_READER4:
            eBank=AUD_REG_BANK3;
            nAddr0=REG_AUDDMA_R4_CFG00;
            nAddr1=REG_AUDDMA_R4_STS00;
            break;
        case AUD_DMA_READER5:
            eBank=AUD_REG_BANK5;
            nAddr0=REG_AUDDMA_R5_CFG00;
            nAddr1=REG_AUDDMA_R5_STS00;
            break;
        default:
            return 0;

    }
#if 0
    if(eDmaChannel < AUD_DMA_READER1)//dma writer
    {
        _AudWriteReg(eBank,nAddr0,AUD_DMA_WR_LEVEL_MASK,AUD_DMA_WR_LEVEL_MASK);

        nConfigValue = _AudReadReg(eBank,nAddr1);

        _AudWriteReg(eBank,nAddr0,AUD_DMA_WR_LEVEL_MASK,0);

        nConfigValue = ((nConfigValue>DMA_LOCALBUF_LINE)? (nConfigValue-DMA_LOCALBUF_LINE):0); //level count contains the local buffer data size
    }
    else//dma reader
    {
        _AudWriteReg(eBank,nAddr0,AUD_DMA_RD_LEVEL_MASK,AUD_DMA_RD_LEVEL_MASK);

        nConfigValue = _AudReadReg(eBank,nAddr1);

        _AudWriteReg(eBank,nAddr0,AUD_DMA_RD_LEVEL_MASK,0);
    }
#else
    if(eDmaChannel < AUD_DMA_READER1)//dma writer
    {
        _AudWriteReg(eBank,nAddr0,AUD_DMA_WR_LEVEL_MASK,AUD_DMA_WR_LEVEL_MASK);

        while(nCount++<10)
        {
            nValue0 = _AudReadReg(eBank,nAddr1);
            nValue1 = _AudReadReg(eBank,nAddr1);
            //nValue2 = _AudReadReg(eBank,nAddr1);
            //if(nValue0 == nValue1 && nValue1==nValue2)
            if(nValue0 == nValue1)
                break;
        }
        if(nCount==10)
        {
            //printk(KERN_ERR"level count!!!!\n");
        }

        _AudWriteReg(eBank,nAddr0,AUD_DMA_WR_LEVEL_MASK,0);

        nValue0 = ((nValue0>DMA_LOCALBUF_LINE)? (nValue0-DMA_LOCALBUF_LINE):0); //level count contains the local buffer data size
    }
    else//dma reader
    {
        _AudWriteReg(eBank,nAddr0,AUD_DMA_RD_LEVEL_MASK,AUD_DMA_RD_LEVEL_MASK);

        while(nCount++<10)
        {
            nValue0 = _AudReadReg(eBank,nAddr1);
            nValue1 = _AudReadReg(eBank,nAddr1);
            if(nValue0 == nValue1)
                break;
        }
        if(nCount==10)
        {
            //printk(KERN_ERR"level count!!!!\n");
        }
        _AudWriteReg(eBank,nAddr0,AUD_DMA_RD_LEVEL_MASK,0);
    }
#endif
    nByteSize = nValue0 * MIU_LINE_SIZE;

    return nByteSize;
}

U32 HalAudDmaTrigLevelCnt(AudDmaChn_e eDmaChannel, U32 nDataSize)
{
    AudRegBank_e eBank;
    U8 nAddr0,nAddr1;
    U32 nConfigValue;

    switch(eDmaChannel)
    {
        case AUD_DMA_WRITER1:
            eBank=AUD_REG_BANK3;
            nAddr0=REG_AUDDMA_W1_CFG04;
            nAddr1=REG_AUDDMA_W1_CFG00;
            break;
        case AUD_DMA_WRITER2:
            eBank=AUD_REG_BANK3;
            nAddr0=REG_AUDDMA_W2_CFG04;
            nAddr1=REG_AUDDMA_W2_CFG00;
            break;
        case AUD_DMA_WRITER3:
            eBank=AUD_REG_BANK3;
            nAddr0=REG_AUDDMA_W3_CFG04;
            nAddr1=REG_AUDDMA_W3_CFG00;
            break;
        case AUD_DMA_WRITER4:
            eBank=AUD_REG_BANK3;
            nAddr0=REG_AUDDMA_W4_CFG04;
            nAddr1=REG_AUDDMA_W4_CFG00;
            break;
        case AUD_DMA_WRITER5:
            eBank=AUD_REG_BANK4;
            nAddr0=REG_AUDDMA_W5_CFG04;
            nAddr1=REG_AUDDMA_W5_CFG00;
            break;
        case AUD_DMA_READER1:
            eBank=AUD_REG_BANK3;
            nAddr0=REG_AUDDMA_R1_CFG04;
            nAddr1=REG_AUDDMA_R1_CFG00;
            break;
        case AUD_DMA_READER2:
            eBank=AUD_REG_BANK3;
            nAddr0=REG_AUDDMA_R2_CFG04;
            nAddr1=REG_AUDDMA_R2_CFG00;
            break;
        case AUD_DMA_READER3:
            eBank=AUD_REG_BANK3;
            nAddr0=REG_AUDDMA_R3_CFG04;
            nAddr1=REG_AUDDMA_R3_CFG00;
            break;
        case AUD_DMA_READER4:
            eBank=AUD_REG_BANK3;
            nAddr0=REG_AUDDMA_R4_CFG04;
            nAddr1=REG_AUDDMA_R4_CFG00;
            break;
        case AUD_DMA_READER5:
            eBank=AUD_REG_BANK5;
            nAddr0=REG_AUDDMA_R5_CFG04;
            nAddr1=REG_AUDDMA_R5_CFG00;
            break;
        default:
            return 0;

    }

    if(eDmaChannel < AUD_DMA_READER1)//dma writer
    {
        nConfigValue = (U16)((nDataSize / MIU_LINE_SIZE) & AUD_DMA_WR_SIZE_MSK);
        if(nConfigValue>0)
        {
            nDataSize = nConfigValue * MIU_LINE_SIZE;
            _AudWriteReg(eBank,nAddr0,AUD_DMA_WR_SIZE_MSK,nConfigValue);

            _AudWriteReg(eBank,nAddr1,AUD_DMA_WR_TRIG,AUD_DMA_WR_TRIG);
            _AudWriteReg(eBank,nAddr1,AUD_DMA_WR_TRIG,0);
  //          udelay(5); //wait level count ready
        }
        return nDataSize;
    }
    else//dma reader
    {
        nConfigValue = (U16)((nDataSize / MIU_LINE_SIZE) & AUD_DMA_RD_SIZE_MSK);
        if(nConfigValue>0)
        {
            nDataSize = nConfigValue * MIU_LINE_SIZE;
            _AudWriteReg(eBank,nAddr0,AUD_DMA_RD_SIZE_MSK,nConfigValue);

            _AudWriteReg(eBank,nAddr1,AUD_DMA_RD_TRIG,AUD_DMA_RD_TRIG);
            _AudWriteReg(eBank,nAddr1,AUD_DMA_RD_TRIG,0);
//            udelay(5); //wait level count ready
        }
        return nDataSize;
    }
    return 0;
}

BOOL HalAudDmaSetPhyAddr(AudDmaChn_e eDmaChannel, U32 nBufAddrOffset, U32 nBufSize)
{
    U16 nMiuAddrLo, nMiuAddrHi, nMiuSize;
    AudRegBank_e eBank;
    U8 nAddr0,nAddr1,nAddr2;

    switch(eDmaChannel)
    {
        case AUD_DMA_WRITER1:
            eBank=AUD_REG_BANK3;
            nAddr0=REG_AUDDMA_W1_CFG01;
            nAddr1=REG_AUDDMA_W1_CFG02;
            nAddr2=REG_AUDDMA_W1_CFG03;
            break;
        case AUD_DMA_WRITER2:
            eBank=AUD_REG_BANK3;
            nAddr0=REG_AUDDMA_W2_CFG01;
            nAddr1=REG_AUDDMA_W2_CFG02;
            nAddr2=REG_AUDDMA_W2_CFG03;
            break;
        case AUD_DMA_WRITER3:
            eBank=AUD_REG_BANK3;
            nAddr0=REG_AUDDMA_W3_CFG01;
            nAddr1=REG_AUDDMA_W3_CFG02;
            nAddr2=REG_AUDDMA_W3_CFG03;
            break;
        case AUD_DMA_WRITER4:
            eBank=AUD_REG_BANK3;
            nAddr0=REG_AUDDMA_W4_CFG01;
            nAddr1=REG_AUDDMA_W4_CFG02;
            nAddr2=REG_AUDDMA_W4_CFG03;
            break;
        case AUD_DMA_WRITER5:
            eBank=AUD_REG_BANK4;
            nAddr0=REG_AUDDMA_W5_CFG01;
            nAddr1=REG_AUDDMA_W5_CFG02;
            nAddr2=REG_AUDDMA_W5_CFG03;
            break;
        case AUD_DMA_READER1:
            eBank=AUD_REG_BANK3;
            nAddr0=REG_AUDDMA_R1_CFG01;
            nAddr1=REG_AUDDMA_R1_CFG02;
            nAddr2=REG_AUDDMA_R1_CFG03;
            break;
        case AUD_DMA_READER2:
            eBank=AUD_REG_BANK3;
            nAddr0=REG_AUDDMA_R2_CFG01;
            nAddr1=REG_AUDDMA_R2_CFG02;
            nAddr2=REG_AUDDMA_R2_CFG03;
            break;
        case AUD_DMA_READER3:
            eBank=AUD_REG_BANK3;
            nAddr0=REG_AUDDMA_R3_CFG01;
            nAddr1=REG_AUDDMA_R3_CFG02;
            nAddr2=REG_AUDDMA_R3_CFG03;
            break;
        case AUD_DMA_READER4:
            eBank=AUD_REG_BANK3;
            nAddr0=REG_AUDDMA_R4_CFG01;
            nAddr1=REG_AUDDMA_R4_CFG02;
            nAddr2=REG_AUDDMA_R4_CFG03;
            break;
        case AUD_DMA_READER5:
            eBank=AUD_REG_BANK5;
            nAddr0=REG_AUDDMA_R5_CFG01;
            nAddr1=REG_AUDDMA_R5_CFG02;
            nAddr2=REG_AUDDMA_R5_CFG03;
            break;
        default:
            //ERRMSG("HalAudDmaSetPhyAddr - ERROR DMA default case!\n");
            return FALSE;

    }

    if(eDmaChannel < AUD_DMA_READER1)//dma writer
    {
        nMiuAddrLo = (U16)((nBufAddrOffset / MIU_LINE_SIZE) & AUD_DMA_WR_BASE_ADDR_LO_MSK);
        nMiuAddrHi = (U16)(((nBufAddrOffset / MIU_LINE_SIZE) >> AUD_DMA_WR_BASE_ADDR_HI_OFFSET) & AUD_DMA_WR_BASE_ADDR_HI_MSK);
        nMiuSize = (U16)((nBufSize / MIU_LINE_SIZE) & AUD_DMA_WR_BUFF_SIZE_MSK);

        _AudWriteReg(eBank, nAddr0, AUD_DMA_WR_BASE_ADDR_LO_MSK, nMiuAddrLo);
        _AudWriteReg(eBank, nAddr1, AUD_DMA_WR_BASE_ADDR_HI_MSK, nMiuAddrHi);
        _AudWriteReg(eBank, nAddr2, AUD_DMA_WR_BUFF_SIZE_MSK, nMiuSize);
    }
    else//dma reader
    {
        nMiuAddrLo = (U16)((nBufAddrOffset / MIU_LINE_SIZE) & AUD_DMA_RD_BASE_ADDR_LO_MSK);
        nMiuAddrHi = (U16)(((nBufAddrOffset / MIU_LINE_SIZE) >> AUD_DMA_RD_BASE_ADDR_HI_OFFSET) & AUD_DMA_RD_BASE_ADDR_HI_MSK);
        nMiuSize = (U16)((nBufSize / MIU_LINE_SIZE) & AUD_DMA_RD_BUFF_SIZE_MSK);

        _AudWriteReg(eBank, nAddr0, AUD_DMA_RD_BASE_ADDR_LO_MSK, nMiuAddrLo);
        _AudWriteReg(eBank, nAddr1, AUD_DMA_RD_BASE_ADDR_HI_MSK, nMiuAddrHi);
        _AudWriteReg(eBank, nAddr2, AUD_DMA_RD_BUFF_SIZE_MSK, nMiuSize);
    }

    return TRUE;
}

//0:16, 1:32
BOOL HalAudDmaSetBitMode(AudDmaChn_e eDmaChannel, U16 nSel)
{
    AudRegBank_e eBank;
    U8 nAddr;
    switch(eDmaChannel)
    {
        case AUD_DMA_WRITER1:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W1_CFG00;
            break;
        case AUD_DMA_WRITER2:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W2_CFG00;
            break;
        case AUD_DMA_WRITER3:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W3_CFG00;
            break;
        case AUD_DMA_WRITER4:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W4_CFG00;
            break;
        case AUD_DMA_WRITER5:
            eBank=AUD_REG_BANK4;
            nAddr=REG_AUDDMA_W5_CFG00;
            break;
        case AUD_DMA_READER1:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_R1_CFG00;
            break;
        case AUD_DMA_READER2:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_R2_CFG00;
            break;
        case AUD_DMA_READER3:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_R3_CFG00;
            break;
        case AUD_DMA_READER4:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_R4_CFG00;
            break;
        case AUD_DMA_READER5:
            eBank=AUD_REG_BANK5;
            nAddr=REG_AUDDMA_R5_CFG00;
            break;
        default:
            return FALSE;

    }

    if(eDmaChannel < AUD_DMA_READER1)//dma writer
    {
        _AudWriteReg(eBank,nAddr,AUD_DMA_WR_BIT_MODE,(nSel?AUD_DMA_WR_BIT_MODE:0));
    }
    else//dma reader
    {
        _AudWriteReg(eBank,nAddr,AUD_DMA_RD_BIT_MODE,(nSel?AUD_DMA_RD_BIT_MODE:0));
    }
    return TRUE;

}

BOOL HalAudDmaWrSetThreshold(AudDmaChn_e eDmaChannel, U32 nOverrunTh)
{
    AudRegBank_e eBank;
    U8 nAddr;
    U16 nMiuOverrunTh;
    switch(eDmaChannel)
    {
        case AUD_DMA_WRITER1:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W1_CFG05;
            break;
        case AUD_DMA_WRITER2:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W2_CFG05;
            break;
        case AUD_DMA_WRITER3:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W3_CFG05;
            break;
        case AUD_DMA_WRITER4:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W4_CFG05;
            break;
        case AUD_DMA_WRITER5:
            eBank=AUD_REG_BANK4;
            nAddr=REG_AUDDMA_W5_CFG05;
            break;
        default:
            //ERRMSG("HalAudDmaWrSetThreshold - ERROR bank default case!\n");
            return FALSE;
    }


    nMiuOverrunTh = (U16)((nOverrunTh / MIU_LINE_SIZE) & AUD_DMA_WR_OVERRUN_TH_MSK);
    _AudWriteReg(eBank, nAddr, AUD_DMA_WR_OVERRUN_TH_MSK, nMiuOverrunTh);
    return TRUE;


}

BOOL HalAudDmaRdSetThreshold(AudDmaChn_e eDmaChannel, U32 nUnderrunTh)
{
    AudRegBank_e eBank;
    U8 nAddr;
    U16 nMiuUnderrunTh;
    switch(eDmaChannel)
    {
        case AUD_DMA_READER1:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_R1_CFG05;
            break;
        case AUD_DMA_READER2:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_R2_CFG05;
            break;
        case AUD_DMA_READER3:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_R3_CFG05;
            break;
        case AUD_DMA_READER4:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_R4_CFG05;
            break;
        case AUD_DMA_READER5:
            eBank=AUD_REG_BANK5;
            nAddr=REG_AUDDMA_R5_CFG05;
            break;
        default:
            return FALSE;
    }

    nMiuUnderrunTh = (U16)((nUnderrunTh / MIU_LINE_SIZE) & AUD_DMA_RD_UNDERRUN_TH_MSK);
    _AudWriteReg(eBank, nAddr, AUD_DMA_WR_OVERRUN_TH_MSK, nMiuUnderrunTh);
    return TRUE;

}

#if 0
BOOL HalAudDmaRdIsEmpty(AudDmaChn_e eDmaChannel)
{
    U8 nAddr;
    U16 nConfigValue;
    switch(eDmaChannel)
    {
        case AUD_DMA_READER1:
            nAddr=REG_AUDDMA_R1_STS01;
            break;
        case AUD_DMA_READER2:
            nAddr=REG_AUDDMA_R2_STS01;
            break;
        case AUD_DMA_READER3:
            nAddr=REG_AUDDMA_R3_STS01;
            break;
        case AUD_DMA_READER4:
            nAddr=REG_AUDDMA_R4_STS01;
            break;
        case AUD_DMA_READER5:
            // To-do
            break;
        default:
            return FALSE;
    }

    nConfigValue = _AudReadReg(AUD_REG_BANK3, nAddr);
    return ((nConfigValue & AUD_DMA_RD_EMPTY_FLAG) ? TRUE : FALSE);

}

BOOL HalAudDmaRdIsLocalEmpty(AudDmaChn_e eDmaChannel)
{
    U8 nAddr;
    U16 nConfigValue;
    switch(eDmaChannel)
    {
        case AUD_DMA_READER1:
            nAddr=REG_AUDDMA_R1_STS01;
            break;
        case AUD_DMA_READER2:
            nAddr=REG_AUDDMA_R2_STS01;
            break;
        case AUD_DMA_READER3:
            nAddr=REG_AUDDMA_R3_STS01;
            break;
        case AUD_DMA_READER4:
            nAddr=REG_AUDDMA_R4_STS01;
            break;
        case AUD_DMA_READER5:
            // To-do
            break;
        default:
            return FALSE;
    }

    nConfigValue = _AudReadReg(AUD_REG_BANK3, nAddr);
    return ((nConfigValue & AUD_DMA_RD_LOCALBUF_EMPTY_FLAG) ? TRUE : FALSE);
}

BOOL HalAudDmaRdIsUnderrun(AudDmaChn_e eDmaChannel)
{
    U8 nAddr;
    U16 nConfigValue;
    switch(eDmaChannel)
    {
        case AUD_DMA_READER1:
            nAddr=REG_AUDDMA_R1_STS01;
            break;
        case AUD_DMA_READER2:
            nAddr=REG_AUDDMA_R2_STS01;
            break;
        case AUD_DMA_READER3:
            nAddr=REG_AUDDMA_R3_STS01;
            break;
        case AUD_DMA_READER4:
            nAddr=REG_AUDDMA_R4_STS01;
            break;
        case AUD_DMA_READER5:
            // To-do
            break;
        default:
            return FALSE;
    }

    nConfigValue = _AudReadReg(AUD_REG_BANK3, nAddr);
    return ((nConfigValue & AUD_DMA_RD_UNDERRUN_FLAG) ? TRUE : FALSE);
}


BOOL HalAudDmaWrIsFull(AudDmaChn_e eDmaChannel)
{
    AudRegBank_e eBank;
    U8 nAddr;
    U16 nConfigValue;
    switch(eDmaChannel)
    {
        case AUD_DMA_WRITER1:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W1_STS01;
            break;
        case AUD_DMA_WRITER2:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W2_STS01;
            break;
        case AUD_DMA_WRITER3:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W3_STS01;
            break;
        case AUD_DMA_WRITER4:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W4_STS01;
            break;
        case AUD_DMA_WRITER5:
            eBank=AUD_REG_BANK4;
            nAddr=REG_AUDMA_V2_W1_STS01;
            break;
        default:
            return FALSE;

    }
    nConfigValue = _AudReadReg(eBank, nAddr);
    return ((nConfigValue & AUD_DMA_WR_FULL_FLAG) ? TRUE : FALSE);
}

BOOL HalAudDmaWrIsLocalFull(AudDmaChn_e eDmaChannel)
{
    AudRegBank_e eBank;
    U8 nAddr;
    U16 nConfigValue;
    switch(eDmaChannel)
    {
        case AUD_DMA_WRITER1:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W1_STS01;
            break;
        case AUD_DMA_WRITER2:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W2_STS01;
            break;
        case AUD_DMA_WRITER3:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W3_STS01;
            break;
        case AUD_DMA_WRITER4:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W4_STS01;
            break;
        case AUD_DMA_WRITER5:
            eBank=AUD_REG_BANK4;
            nAddr=REG_AUDMA_V2_W1_STS01;
            break;
        default:
            return FALSE;

    }
    nConfigValue = _AudReadReg(eBank, nAddr);
    return (nConfigValue & AUD_DMA_WR_LOCALBUF_FULL_FLAG) ? TRUE : FALSE;
}



BOOL HalAudDmaWrIsOverrun(AudDmaChn_e eDmaChannel)
{
    AudRegBank_e eBank;
    U8 nAddr;
    U16 nConfigValue;
    switch(eDmaChannel)
    {
        case AUD_DMA_WRITER1:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W1_STS01;
            break;
        case AUD_DMA_WRITER2:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W2_STS01;
            break;
        case AUD_DMA_WRITER3:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W3_STS01;
            break;
        case AUD_DMA_WRITER4:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W4_STS01;
            break;
        case AUD_DMA_WRITER5:
            eBank=AUD_REG_BANK4;
            nAddr=REG_AUDMA_V2_W1_STS01;
            break;
        default:
            return FALSE;

    }
    nConfigValue = _AudReadReg(eBank, nAddr);
    return (nConfigValue & AUD_DMA_WR_OVERRUN_FLAG) ? TRUE : FALSE;
}
#endif
BOOL _HalAudDmaRdGetFlags(AudDmaChn_e eDmaChannel, BOOL *pbUnderrun, BOOL *pbEmtpy, BOOL *pbLocalEmpty)
{
    U8 nAddr;
    U16 nConfigValue;
    AudRegBank_e eBank;
    switch(eDmaChannel)
    {
        case AUD_DMA_READER1:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_R1_STS01;
            break;
        case AUD_DMA_READER2:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_R2_STS01;
            break;
        case AUD_DMA_READER3:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_R3_STS01;
            break;
        case AUD_DMA_READER4:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_R4_STS01;
            break;
        case AUD_DMA_READER5:
            eBank=AUD_REG_BANK5;
            nAddr=REG_AUDDMA_R5_STS01;
            break;
        default:
            return FALSE;
    }
    nConfigValue = _AudReadReg(eBank, nAddr);
    *pbUnderrun = (nConfigValue & AUD_DMA_RD_UNDERRUN_FLAG) ? TRUE : FALSE;
    *pbEmtpy = (nConfigValue & AUD_DMA_RD_EMPTY_FLAG) ? TRUE : FALSE;
    *pbLocalEmpty = (nConfigValue & AUD_DMA_RD_LOCALBUF_EMPTY_FLAG) ? TRUE : FALSE;

    return TRUE;

}

BOOL _HalAudDmaWrGetFlags(AudDmaChn_e eDmaChannel, BOOL *pbOverrun, BOOL *pbFull, BOOL *pbLocalFull)
{
    U8 nAddr;
    U16 nConfigValue;
    AudRegBank_e eBank;

    switch(eDmaChannel)
    {
        case AUD_DMA_WRITER1:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W1_STS01;
            break;
        case AUD_DMA_WRITER2:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W2_STS01;
            break;
        case AUD_DMA_WRITER3:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W3_STS01;
            break;
        case AUD_DMA_WRITER4:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W4_STS01;
            break;
        case AUD_DMA_WRITER5:
            eBank=AUD_REG_BANK4;
            nAddr=REG_AUDDMA_W5_STS01;
            break;
        default:
            return FALSE;

    }
    nConfigValue = _AudReadReg(eBank, nAddr);
    *pbOverrun = (nConfigValue & AUD_DMA_WR_OVERRUN_FLAG) ? TRUE : FALSE;
    *pbFull = (nConfigValue & AUD_DMA_WR_FULL_FLAG) ? TRUE : FALSE;
    *pbLocalFull = (nConfigValue & AUD_DMA_WR_LOCALBUF_FULL_FLAG) ? TRUE : FALSE;
    return TRUE;
}

BOOL HalAudDmaGetFlags(AudDmaChn_e eDmaChannel, BOOL *pbDatatrigger, BOOL *pbDataboundary, BOOL *pbLocalData)
{
    BOOL ret;

    if (eDmaChannel <= AUD_DMA_WRITER5)
        ret = _HalAudDmaWrGetFlags(eDmaChannel, pbDatatrigger, pbDataboundary, pbLocalData);
    else
        ret = _HalAudDmaRdGetFlags(eDmaChannel, pbDatatrigger, pbDataboundary, pbLocalData);

    return ret;
}

BOOL _HalAudDmaRdGetStatus(AudDmaChn_e eDmaChannel, BOOL *pbEmtpy)
{
    U8 nAddr;
    U16 nConfigValue;
    AudRegBank_e eBank;
    switch(eDmaChannel)
    {
        case AUD_DMA_READER1:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_R1_STS01;
            break;
        case AUD_DMA_READER2:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_R2_STS01;
            break;
        case AUD_DMA_READER3:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_R3_STS01;
            break;
        case AUD_DMA_READER4:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_R4_STS01;
            break;
        case AUD_DMA_READER5:
            eBank=AUD_REG_BANK5;
            nAddr=REG_AUDDMA_R5_STS01;
            break;
        default:
            return FALSE;
    }
    nConfigValue = _AudReadReg(eBank, nAddr);

    *pbEmtpy = (nConfigValue & AUD_DMA_RD_EMPTY_STATUS)? TRUE : FALSE;

    return TRUE;

}

BOOL _HalAudDmaWrGetStatus(AudDmaChn_e eDmaChannel, BOOL *pbFull)
{
    U8 nAddr;
    U16 nConfigValue;
    AudRegBank_e eBank;

    switch(eDmaChannel)
    {
        case AUD_DMA_WRITER1:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W1_STS01;
            break;
        case AUD_DMA_WRITER2:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W2_STS01;
            break;
        case AUD_DMA_WRITER3:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W3_STS01;
            break;
        case AUD_DMA_WRITER4:
            eBank=AUD_REG_BANK3;
            nAddr=REG_AUDDMA_W4_STS01;
            break;
        case AUD_DMA_WRITER5:
            eBank=AUD_REG_BANK4;
            nAddr=REG_AUDDMA_W5_STS01;
            break;
        default:
            return FALSE;

    }
    nConfigValue = _AudReadReg(eBank, nAddr);
    *pbFull = (nConfigValue & AUD_DMA_WR_FULL_STATUS) ? TRUE : FALSE;

    return TRUE;
}

BOOL HalAudDmaGetStatus(AudDmaChn_e eDmaChannel, BOOL *pbDataboundary)
{
    BOOL ret;

    if (eDmaChannel <= AUD_DMA_WRITER5)
        ret = _HalAudDmaWrGetStatus(eDmaChannel, pbDataboundary);
    else
        ret = _HalAudDmaRdGetStatus(eDmaChannel, pbDataboundary);

    return ret;
}

BOOL HalAudDmaRd5ConfigMch(U16 nCh)
{
    U16 aChnSup[5]={1,2,4,6,8};
    U16 i;
    for(i=0;i<5;i++)
    {
        if(nCh==aChnSup[i])
            break;
    }

    if(i==5)
        return FALSE;


    _AudWriteReg(AUD_REG_BANK5, REG_SYS2_CTRL_02, AUD_DMA_WR5_MCH_MSK, (nCh<<AUD_DMA_WR5_MCH_POS));
    return TRUE;
}

BOOL HalAudDmaWrConfigMch(AudDmaChn_e eDmaChannel, U16 nCh)
{
    U8 nAddr;
    U16 nConfigValue;
    AudRegBank_e eBank;
    switch(eDmaChannel)
    {
        case AUD_DMA_WRITER3:
        case AUD_DMA_WRITER4:
            if(nCh==2)
            {
                return TRUE;
            }
            else
            {
                return FALSE;
            }

        case AUD_DMA_WRITER1:
            eBank = AUD_REG_BANK2;
            nAddr = REG_W1_DMA_MCH_CFG1;
            break;
        case AUD_DMA_WRITER2:
            eBank = AUD_REG_BANK2;
            nAddr = REG_W2_DMA_MCH_CFG1;
            break;
        case AUD_DMA_WRITER5:
            eBank = AUD_REG_BANK4;
            nAddr = REG_DMA_MCH_IF_CFG1;
            break;
        default:
            return FALSE;
    }

    if(nCh==2)
    {
        nConfigValue = 0;
    }
    else if(nCh==4)
    {
        nConfigValue = 1;
    }
    else if(nCh==6)
    {
        nConfigValue = 2;
    }
    else if(nCh==8)
    {
        nConfigValue = 3;
    }
    else
    {
        return FALSE;
    }

    _AudWriteReg(eBank, nAddr, AUD_DMA_WR_CHN_MODE_MSK, (nConfigValue<<AUD_DMA_WR_CHN_MODE_POS));
    return TRUE;
}

BOOL HalAudDmaWr1SetClkRate(AudDmaClkSrc_e eSrc, AudRate_e eRate)
{
    U16 nConfigValue;
    U16 nConfigValue1;
    switch(eSrc)
    {
        case AUD_DMA_CLK_GPA:
            switch(eRate)
            {
                case AUD_RATE_8K:
                    nConfigValue = 0x4;
                    break;
                case AUD_RATE_16K:
                    nConfigValue = 0x3;
                    break;
                case AUD_RATE_32K:
                    nConfigValue = 0x2;
                    break;
                case AUD_RATE_48K:
                    nConfigValue = 0x1;
                    break;
                case AUD_RATE_96K:
                    nConfigValue = 0x0;
                    break;
                default:
                    return FALSE;
            }
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_04, AUD_SEL_CLK_ADC_256FS_MSK, (nConfigValue<<AUD_SEL_CLK_ADC_256FS_POS));
            break;
        case AUD_DMA_CLK_ADC:
            switch(eRate)
            {
                case AUD_RATE_8K:
                    nConfigValue = 0x0;
                    nConfigValue1 = 0x0;
                    break;
                case AUD_RATE_16K:
                    nConfigValue = 0x1;
                    nConfigValue1 = 0x3;
                    break;
                case AUD_RATE_32K:
                    nConfigValue = 0x2;
                    nConfigValue1 = 0x6;
                    break;
                case AUD_RATE_48K:
                    nConfigValue = 0x3;
                    nConfigValue1 = 0x8;
                    break;
                case AUD_RATE_96K:
                    nConfigValue = 0x4;
                    nConfigValue1 = 0xb;
                    break;
                default:
                    return FALSE;
            }

            _AudWriteReg(AUD_REG_BANK0, REG_AUDIOBAND_CFG_02, AUD_CODEC_ADC_DEC_MSK, (nConfigValue1<<AUD_CODEC_ADC_DEC_POS));
            _AudWriteReg(AUD_REG_BANK0, REG_VLD_GEN_CFG_0, AUD_VLD_GEN_ADC_FS_SEL_MSK, (nConfigValue<<AUD_VLD_GEN_ADC_FS_SEL_POS));
            break;
        default:
            return FALSE;
    }
    _gaDmaClk[AUD_DMA_WRITER1].eRate = eRate;
    _gaDmaClk[AUD_DMA_WRITER1].eSrc = eSrc;
    return TRUE;
}

//GPA 256FS not support in SW case
BOOL HalAudDmaWr2SetClkRate(AudDmaClkSrc_e eSrc, AudRate_e eRate)
{
    U16 nConfigValue;
    switch(eSrc)
    {
        case AUD_DMA_CLK_GPA:
            switch(eRate)
            {
                case AUD_RATE_8K:
                    nConfigValue = 0x4;
                    break;
                case AUD_RATE_16K:
                    nConfigValue = 0x3;
                    break;
                case AUD_RATE_32K:
                    nConfigValue = 0x2;
                    break;
                case AUD_RATE_48K:
                    nConfigValue = 0x1;
                    break;
                case AUD_RATE_96K:
                    nConfigValue = 0x0;
                    break;
                default:
                    return FALSE;
            }
            if(!_gbDmaClkBypass[AUD_DMA_WRITER2])
            {
                _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_06, AUD_SEL_CLK_SRC_A1_256FSO_MSK, (0x1<<AUD_SEL_CLK_SRC_A1_256FSO_POS)); //240FS
            }
            _AudWriteReg(AUD_REG_BANK0, REG_VLD_GEN_CFG_1, AUD_VLD_GEN_SRCO_CIC_SEL, AUD_VLD_GEN_SRCO_CIC_SEL);
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_06, AUD_SEL_A1_FS_RATE_MSK, (nConfigValue<<AUD_SEL_A1_FS_RATE_POS));
            break;
        case AUD_DMA_CLK_USB240FS:
            if(!_gbDmaClkBypass[AUD_DMA_WRITER2])
            {
                _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_06, AUD_SEL_CLK_SRC_A1_256FSO_MSK, (0x2<<AUD_SEL_CLK_SRC_A1_256FSO_POS));
            }
            _AudWriteReg(AUD_REG_BANK0, REG_VLD_GEN_CFG_1, AUD_VLD_GEN_SRCO_CIC_SEL, AUD_VLD_GEN_SRCO_CIC_SEL);
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_06, AUD_SEL_CLK_SRC_A1_256FSO_USB, AUD_SEL_CLK_SRC_A1_256FSO_USB);
            //usb sof synthesizer
            break;
#if USB_SYNTH==0
        case AUD_DMA_CLK_USB256FS:
            if(!_gbDmaClkBypass[AUD_DMA_WRITER2])
            {
                _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_06, AUD_SEL_CLK_SRC_A1_256FSO_MSK, (0x2<<AUD_SEL_CLK_SRC_A1_256FSO_POS));
            }
            _AudWriteReg(AUD_REG_BANK0, REG_VLD_GEN_CFG_1, AUD_VLD_GEN_SRCO_CIC_SEL, 0);
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_06, AUD_SEL_CLK_SRC_A1_256FSO_USB, 0);
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_12, AUD_SEL_USB_SYNTH_MS, 0);
            //usb sof synthesizer
            break;
#else
        case AUD_DMA_CLK_SYNTH:
            if(!_gbDmaClkBypass[AUD_DMA_WRITER2])
            {
                _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_06, AUD_SEL_CLK_SRC_A1_256FSO_MSK, (0x2<<AUD_SEL_CLK_SRC_A1_256FSO_POS));
            }
            _AudWriteReg(AUD_REG_BANK0, REG_VLD_GEN_CFG_1, AUD_VLD_GEN_SRCO_CIC_SEL, 0);
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_06, AUD_SEL_CLK_SRC_A1_256FSO_USB, 0);
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_12, AUD_SEL_USB_SYNTH_MS, AUD_SEL_USB_SYNTH_MS);
            if(HalAudSetSynthRate(AUD_SYNTH_USB, eRate)==FALSE)
                return FALSE;
            break;
#endif
        default:
            return FALSE;
    }

    _gaDmaClk[AUD_DMA_WRITER2].eRate = eRate;
    _gaDmaClk[AUD_DMA_WRITER2].eSrc = eSrc;
    return TRUE;
}

//USB 240FS cannot support in this SW version
BOOL HalAudDmaWr3SetClkRate(AudDmaClkSrc_e eSrc, AudRate_e eRate)
{
    U16 nConfigValue;
    switch(eSrc)
    {
        case AUD_DMA_CLK_GPA:
            switch(eRate)
            {
                case AUD_RATE_8K:
                    nConfigValue = 0x4;
                    break;
                case AUD_RATE_16K:
                    nConfigValue = 0x3;
                    break;
                case AUD_RATE_32K:
                    nConfigValue = 0x2;
                    break;
                case AUD_RATE_48K:
                    nConfigValue = 0x1;
                    break;
                case AUD_RATE_96K:
                    nConfigValue = 0x0;
                    break;
                default:
                    return FALSE;
            }
            if(!_gbDmaClkBypass[AUD_DMA_WRITER3])
            {
                if(_HalBtRxNeed256Fs())
                {
                    _AudWriteReg(AUD_REG_BANK0, REG_VLD_GEN_CFG_2, AUD_VLD_GEN_SRCO_CIC_SEL, 0);
                    _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_07, AUD_SEL_CLK_SRC_A2_256FSO_MSK, 0);
                }
                else
                {
                    _AudWriteReg(AUD_REG_BANK0, REG_VLD_GEN_CFG_2, AUD_VLD_GEN_SRCO_CIC_SEL, AUD_VLD_GEN_SRCO_CIC_SEL);
                    _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_07, AUD_SEL_CLK_SRC_A2_256FSO_MSK, (0x1<<AUD_SEL_CLK_SRC_A2_256FSO_POS)); //240FS
                }
            }
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_07, AUD_SEL_A2_FS_RATE_MSK, (nConfigValue<<AUD_SEL_A2_FS_RATE_POS));
            break;
            /* case AUD_DMA_CLK_USB240FS:
                 if(_HalBtRxNeed256Fs())
                 {
                     return FALSE;
                 }

                 if(!_gbDmaClkBypass[AUD_DMA_WRITER3])
                 {
                     _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_07, AUD_SEL_CLK_SRC_A2_256FSO_MSK, (0x2<<AUD_SEL_CLK_SRC_A2_256FSO_POS));
                 }

                 _AudWriteReg(AUD_REG_BANK0, REG_VLD_GEN_CFG_2, AUD_VLD_GEN_SRCO_CIC_SEL, AUD_VLD_GEN_SRCO_CIC_SEL);
                 _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_07, AUD_SEL_CLK_SRC_A2_256FSO_USB, AUD_SEL_CLK_SRC_A2_256FSO_USB);

                 break;*/
#if USB_SYNTH==0
        case AUD_DMA_CLK_USB256FS:
            if(!_gbDmaClkBypass[AUD_DMA_WRITER3])
            {
                _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_07, AUD_SEL_CLK_SRC_A2_256FSO_MSK, (0x2<<AUD_SEL_CLK_SRC_A2_256FSO_POS));
            }
            _AudWriteReg(AUD_REG_BANK0, REG_VLD_GEN_CFG_2, AUD_VLD_GEN_SRCO_CIC_SEL, 0);
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_07, AUD_SEL_CLK_SRC_A2_256FSO_USB, 0);
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_12, AUD_SEL_USB_SYNTH_MS, 0);
            //usb sof synthesizer
            break;
#else
        case AUD_DMA_CLK_SYNTH:
            if(!_gbDmaClkBypass[AUD_DMA_WRITER3])
            {
                _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_07, AUD_SEL_CLK_SRC_A2_256FSO_MSK, (0x2<<AUD_SEL_CLK_SRC_A2_256FSO_POS));
            }
            _AudWriteReg(AUD_REG_BANK0, REG_VLD_GEN_CFG_2, AUD_VLD_GEN_SRCO_CIC_SEL, 0);
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_07, AUD_SEL_CLK_SRC_A2_256FSO_USB, 0);
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_12, AUD_SEL_USB_SYNTH_MS, AUD_SEL_USB_SYNTH_MS);
            if(HalAudSetSynthRate(AUD_SYNTH_USB, eRate)==FALSE)
                return FALSE;
            break;
#endif
        default:
            return FALSE;
    }

    _gaDmaClk[AUD_DMA_WRITER3].eRate = eRate;
    _gaDmaClk[AUD_DMA_WRITER3].eSrc = eSrc;
    return TRUE;
}


BOOL HalAudDmaWr4SetClkRate(AudDmaClkSrc_e eSrc, AudRate_e eRate)
{
    U16 nConfigValue;
    switch(eSrc)
    {
        case AUD_DMA_CLK_GPA:
            switch(eRate)
            {
                case AUD_RATE_8K:
                    nConfigValue = 0x4;
                    break;
                case AUD_RATE_16K:
                    nConfigValue = 0x3;
                    break;
                case AUD_RATE_32K:
                    nConfigValue = 0x2;
                    break;
                case AUD_RATE_48K:
                    nConfigValue = 0x1;
                    break;
                case AUD_RATE_96K:
                    nConfigValue = 0x0;
                    break;
                default:
                    return FALSE;
            }

            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_0C, AUD_SEL_CLK_DMA_W4_MSK, (0x1<<AUD_SEL_CLK_DMA_W4_POS)); //240FS
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_0C, AUD_SEL_DMA_W4_GPA_RATE_MSK, (nConfigValue<<AUD_SEL_DMA_W4_GPA_RATE_POS));
            break;
        case AUD_DMA_CLK_USB240FS:
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_0C, AUD_SEL_CLK_DMA_W4_MSK, (0x3<<AUD_SEL_CLK_DMA_W4_POS));
            //usb sof synthesizer

            break;
#if USB_SYNTH==0
        case AUD_DMA_CLK_USB256FS:
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_0C, AUD_SEL_CLK_DMA_W4_MSK, (0x2<<AUD_SEL_CLK_DMA_W4_POS));
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_12, AUD_SEL_USB_SYNTH_MS, 0);
            //usb sof synthesizer

            break;
#else
        case AUD_DMA_CLK_SYNTH:
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_0C, AUD_SEL_CLK_DMA_W4_MSK, (0x2<<AUD_SEL_CLK_DMA_W4_POS));
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_12, AUD_SEL_USB_SYNTH_MS, AUD_SEL_USB_SYNTH_MS);
            if(HalAudSetSynthRate(AUD_SYNTH_USB, eRate)==FALSE)
                return FALSE;
            break;
#endif
        default:
            return FALSE;
    }
    _HalUpdateDmaWr4Vld();
    _gaDmaClk[AUD_DMA_WRITER4].eRate = eRate;
    _gaDmaClk[AUD_DMA_WRITER4].eSrc = eSrc;
    return TRUE;
}


BOOL HalAudDmaWr5SetClkRate(AudDmaClkSrc_e eSrc, AudRate_e eRate)
{
    U16 nConfigValue;
    U16 nConfigValue1;
    switch(eSrc)
    {
        case AUD_DMA_CLK_PDM:
#if 1
//			msb_aud_dmic_setclkrate(AUD_PDM_CLK_RATE_2000K);
//            if(!msb_aud_dmic_setmode(msb_aud_dmic_findmode(eRate)))
//                return FALSE;
#endif
            break;
        case AUD_DMA_CLK_GPA:
            switch(eRate)
            {
                case AUD_RATE_8K:
                    nConfigValue = 0x4;
                    break;
                case AUD_RATE_16K:
                    nConfigValue = 0x3;
                    break;
                case AUD_RATE_32K:
                    nConfigValue = 0x2;
                    break;
                case AUD_RATE_48K:
                    nConfigValue = 0x1;
                    break;
                case AUD_RATE_96K:
                    nConfigValue = 0x0;
                    break;
                default:
                    return FALSE;
            }
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_04, AUD_SEL_CLK_ADC_256FS_MSK, (nConfigValue<<AUD_SEL_CLK_ADC_256FS_POS));
            break;
        case AUD_DMA_CLK_ADC:
            switch(eRate)
            {
                case AUD_RATE_8K:
                    nConfigValue = 0x0;
                    nConfigValue1 = 0x0;
                    break;
                case AUD_RATE_16K:
                    nConfigValue = 0x1;
                    nConfigValue1 = 0x3;
                    break;
                case AUD_RATE_32K:
                    nConfigValue = 0x2;
                    nConfigValue1 = 0x6;
                    break;
                case AUD_RATE_48K:
                    nConfigValue = 0x3;
                    nConfigValue1 = 0x8;
                    break;
                case AUD_RATE_96K:
                    nConfigValue = 0x4;
                    nConfigValue1 = 0xb;
                    break;
                default:
                    return FALSE;
            }

            _AudWriteReg(AUD_REG_BANK0, REG_VLD_GEN_CFG_0, AUD_VLD_GEN_ADC_FS_SEL_MSK, (nConfigValue<<AUD_VLD_GEN_ADC_FS_SEL_POS));
            _AudWriteReg(AUD_REG_BANK0, REG_AUDIOBAND_CFG_02, AUD_CODEC_ADC_DEC_MSK, (nConfigValue1<<AUD_CODEC_ADC_DEC_POS));
            break;
        default:
            return FALSE;
    }
    _gaDmaClk[AUD_DMA_WRITER5].eRate = eRate;
    _gaDmaClk[AUD_DMA_WRITER5].eSrc = eSrc;
    return TRUE;
}

BOOL HalAudDmaRd1SetClkRate(AudDmaClkSrc_e eSrc, AudRate_e eRate)
{
    U16 nConfigValue;
    switch(eSrc)
    {
        case AUD_DMA_CLK_GPA:
            switch(eRate)
            {
                case AUD_RATE_8K:
                    nConfigValue = 0x4;
                    break;
                case AUD_RATE_16K:
                    nConfigValue = 0x3;
                    break;
                case AUD_RATE_32K:
                    nConfigValue = 0x2;
                    break;
                case AUD_RATE_48K:
                    nConfigValue = 0x1;
                    break;
                case AUD_RATE_96K:
                    nConfigValue = 0x0;
                    break;
                default:
                    return FALSE;
            }

            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_08, AUD_SEL_DMA_R1_MSK, (0<<AUD_SEL_DMA_R1_POS));
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_04, AUD_SEL_CLK_DAC_256FS_MSK, (nConfigValue<<AUD_SEL_CLK_DAC_256FS_POS));
            break;
#if USB_SYNTH==0
        case AUD_DMA_CLK_USB256FS:
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_08, AUD_SEL_DMA_R1_MSK, (0x4<<AUD_SEL_DMA_R1_POS));
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_12, AUD_SEL_USB_SYNTH_MS, 0);
            //usb sof synthesizer

            break;

#if NO_HDMI==1
        case AUD_DMA_CLK_SYNTH:
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_08, AUD_SEL_DMA_R1_MSK, (0x1<<AUD_SEL_DMA_R1_POS));
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_12, AUD_SEL_USB_SYNTH_MS, AUD_SEL_USB_SYNTH_MS);
            if(HalAudSetSynthRate(AUD_SYNTH_USB, eRate)==FALSE)
                return FALSE;
            break;
#endif
#else
        case AUD_DMA_CLK_SYNTH:
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_08, AUD_SEL_DMA_R1_MSK, (0x4<<AUD_SEL_DMA_R1_POS));
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_12, AUD_SEL_USB_SYNTH_MS, AUD_SEL_USB_SYNTH_MS);
            if(HalAudSetSynthRate(AUD_SYNTH_USB, eRate)==FALSE)
                return FALSE;
            break;
#endif
        default:
            return FALSE;
    }

    _gaDmaClk[AUD_DMA_READER1].eRate = eRate;
    _gaDmaClk[AUD_DMA_READER1].eSrc = eSrc;
    return TRUE;
}

BOOL HalAudDmaRd2SetClkRate(AudDmaClkSrc_e eSrc, AudRate_e eRate)
{
    U16 nConfigValue;
    switch(eSrc)
    {
        case AUD_DMA_CLK_GPA:
            switch(eRate)
            {
                case AUD_RATE_8K:
                    nConfigValue = 0x4;
                    break;
                case AUD_RATE_16K:
                    nConfigValue = 0x3;
                    break;
                case AUD_RATE_32K:
                    nConfigValue = 0x2;
                    break;
                case AUD_RATE_48K:
                    nConfigValue = 0x1;
                    break;
                case AUD_RATE_96K:
                    nConfigValue = 0x0;
                    break;
                default:
                    return FALSE;
            }
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_08, AUD_SEL_R2_GPA_FS_RATE_MSK, (nConfigValue<<AUD_SEL_R2_GPA_FS_RATE_POS));
            break;
        default:
            return FALSE;
    }

    _gaDmaClk[AUD_DMA_READER2].eRate = eRate;
    _gaDmaClk[AUD_DMA_READER2].eSrc = eSrc;
    return TRUE;
}


BOOL HalAudDmaRd3SetClkRate(AudDmaClkSrc_e eSrc, AudRate_e eRate)
{
    U16 nConfigValue;
    switch(eSrc)
    {
        case AUD_DMA_CLK_GPA:
            switch(eRate)
            {
                case AUD_RATE_8K:
                    nConfigValue = 0x4;
                    break;
                case AUD_RATE_16K:
                    nConfigValue = 0x3;
                    break;
                case AUD_RATE_32K:
                    nConfigValue = 0x2;
                    break;
                case AUD_RATE_48K:
                    nConfigValue = 0x1;
                    break;
                case AUD_RATE_96K:
                    nConfigValue = 0x0;
                    break;
                default:
                    return FALSE;
            }
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_0A, AUD_SEL_R3_GPA_FS_RATE_MSK, (nConfigValue<<AUD_SEL_R3_GPA_FS_RATE_POS));
            break;
        default:
            return FALSE;
    }

    _gaDmaClk[AUD_DMA_READER3].eRate = eRate;
    _gaDmaClk[AUD_DMA_READER3].eSrc = eSrc;
    return TRUE;
}


BOOL HalAudDmaRd4SetClkRate(AudDmaClkSrc_e eSrc, AudRate_e eRate)
{
    U16 nConfigValue;
    switch(eSrc)
    {
        case AUD_DMA_CLK_GPA:
            switch(eRate)
            {
                case AUD_RATE_8K:
                    nConfigValue = 0x4;
                    break;
                case AUD_RATE_16K:
                    nConfigValue = 0x3;
                    break;
                case AUD_RATE_32K:
                    nConfigValue = 0x2;
                    break;
                case AUD_RATE_48K:
                    nConfigValue = 0x1;
                    break;
                case AUD_RATE_96K:
                    nConfigValue = 0x0;
                    break;
                default:
                    return FALSE;
            }

            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_0C, AUD_SEL_CLK_DMA_R4_MSK, 0);
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_0C, AUD_SEL_DMA_R4_GPA_RATE_MSK, (nConfigValue<<AUD_SEL_DMA_R4_GPA_RATE_POS));
            break;
#if USB_SYNTH==0
        case AUD_DMA_CLK_USB256FS:
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_0C, AUD_SEL_CLK_DMA_R4_MSK, (0x1<<AUD_SEL_CLK_DMA_R4_POS));
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_12, AUD_SEL_USB_SYNTH_MS, 0);
            //usb sof synthesizer

            break;
#else
        case AUD_DMA_CLK_SYNTH:
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_0C, AUD_SEL_CLK_DMA_R4_MSK, (0x1<<AUD_SEL_CLK_DMA_R4_POS));
            _AudWriteReg(AUD_REG_BANK2, REG_CLKGEN_CFG_12, AUD_SEL_USB_SYNTH_MS, AUD_SEL_USB_SYNTH_MS);
            if(HalAudSetSynthRate(AUD_SYNTH_USB, eRate)==FALSE)
                return FALSE;
            break;
#endif
        default:
            return FALSE;
    }

    _gaDmaClk[AUD_DMA_READER4].eRate = eRate;
    _gaDmaClk[AUD_DMA_READER4].eSrc = eSrc;
    return TRUE;
}

BOOL HalAudDmaRd5SetClkRate(AudDmaClkSrc_e eSrc, AudRate_e eRate)
{
    switch(eSrc)
    {
#if NO_HDMI==0
        case AUD_DMA_CLK_SYNTH:
            if(HalAudSetSynthRate(AUD_SYNTH_HDMI, eRate)==FALSE)
                return FALSE;
            break;
#endif
        default:
            return FALSE;
    }

    _gaDmaClk[AUD_DMA_READER5].eRate = eRate;
    _gaDmaClk[AUD_DMA_READER5].eSrc = eSrc;
    return TRUE;
}

//ToDo : atop independent on/off
void HalAudAtopInit(void)
{
//----------------- AUSDM ---------------------------- //Item xx
    //AUSDM Setting
    _AudWriteRegByte(0x3111de, 0xff, 0x00);
    _AudWriteRegByte(0x3111df, 0xff, 0x78);
    _AudWriteRegByte(0x3111e0, 0xff, 0x00);
    _AudWriteRegByte(0x3111e1, 0xff, 0x00);
    _AudWriteRegByte(0x3111e2, 0xff, 0x00);
    _AudWriteRegByte(0x3111e3, 0xff, 0x10);
    _AudWriteRegByte(0x3111d8, 0xff, 0x00);
    _AudWriteRegByte(0x3111d9, 0xff, 0x00);
    _AudWriteRegByte(0x3111f6, 0xff, 0x40);
    _AudWriteRegByte(0x3111f7, 0xff, 0x92);
    _AudWriteRegByte(0x3111e6, 0xff, 0x00);
    _AudWriteRegByte(0x3111e7, 0xff, 0x00);
    _AudWriteRegByte(0x3111e8, 0xff, 0x00);
    _AudWriteRegByte(0x3111e9, 0xff, 0x00);
    _AudWriteRegByte(0x3111ea, 0xff, 0x00);
    _AudWriteRegByte(0x3111eb, 0xff, 0x00);
    _AudWriteRegByte(0x3111ec, 0xff, 0x00);
    _AudWriteRegByte(0x3111ed, 0xff, 0x18);
    _AudWriteRegByte(0x3111ee, 0xff, 0x03);
    _AudWriteRegByte(0x3111ef, 0xff, 0x03);
    _AudWriteRegByte(0x3111f2, 0xff, 0x01);
    _AudWriteRegByte(0x3111f3, 0xff, 0x0f);
    _AudWriteRegByte(0x3111dc, 0xff, 0x00);
    _AudWriteRegByte(0x3111dd, 0xff, 0x00);
    _AudWriteRegByte(0x3111da, 0xff, 0x28);
    _AudWriteRegByte(0x3111db, 0xff, 0x03);
    _AudWriteRegByte(0x3111f0, 0xff, 0x00);
    //AUSDM Verification setting
    _AudWriteRegByte(0x3111f2, 0xff, 0x21);   // [    5] EN_ADC_DITHER
    _AudWriteRegByte(0x3111f3, 0xff, 0x0f);
    _AudWriteRegByte(0x3111f0, 0xff, 0x00);   // [11:10] DAC CHOP
    _AudWriteRegByte(0x3111f1, 0xff, 0x0c);
    _AudWriteRegByte(0x3111f0, 0xff, 0x00);   // [ 9: 8] SEL_CHOP_FREQ_DAC
    _AudWriteRegByte(0x3111f1, 0xff, 0x0f);

//----------------- Auto De-POP ----------------------------- //Item 37
    //TBD!!! add later in phase 2


    //status init
    _gbAnalogIdle = FALSE;
    _gnAdcSelect = 0;
    _gbAdcActive = _gbDacActive = FALSE;
}



void _HalAudAtopLineIn(BOOL bEnable)
{
    if (bEnable)
    {
        _AudWriteReg(AUD_REG_BANK1, REG_AUSDM_CFG5, (AUD_SEL_MICGAIN_INMUX_L|AUD_SEL_MICGAIN_INMUX_R), (_gaLineinGainTable[_gnLineInGain][0]?(AUD_SEL_MICGAIN_INMUX_L|AUD_SEL_MICGAIN_INMUX_R):0));
        _AudWriteReg(AUD_REG_BANK1, REG_AUSDM_CFG5, (AUD_SEL_GAIN_INMUX0_L_MSK|AUD_SEL_GAIN_INMUX0_R_MSK), (_gaLineinGainTable[_gnLineInGain][1]<<AUD_SEL_GAIN_INMUX0_L_POS)|(_gaLineinGainTable[_gnLineInGain][1]<<AUD_SEL_GAIN_INMUX0_R_POS));
        _gbAdcActive = TRUE;

    }
    else
    {
        _gbAdcActive = FALSE;
    }
}

void _HalAudAtopMicIn(BOOL bEnable)
{
    if (bEnable)
    {
        _AudWriteReg(AUD_REG_BANK1, REG_AUSDM_CFG5, (AUD_SEL_MICGAIN_INMUX_L|AUD_SEL_MICGAIN_INMUX_R), (_gaMicinGainTable[_gnMicInGain][0]?(AUD_SEL_MICGAIN_INMUX_L|AUD_SEL_MICGAIN_INMUX_R):0));
        _AudWriteReg(AUD_REG_BANK1, REG_AUSDM_CFG5, (AUD_SEL_GAIN_INMUX0_L_MSK|AUD_SEL_GAIN_INMUX0_R_MSK), (_gaMicinGainTable[_gnMicInGain][1]<<AUD_SEL_GAIN_INMUX0_L_POS)|(_gaMicinGainTable[_gnMicInGain][1]<<AUD_SEL_GAIN_INMUX0_R_POS));
        _gbAdcActive = TRUE;

    }
    else
    {
        _gbAdcActive = FALSE;
    }
}

/* To-Do : GPIO  for different board*/
void _HalAudAtopAmp(BOOL bEnable)
{
    U16 nConfigValue;
    nConfigValue = READ_WORD(_gnBaseRegAddr + ((0x1026ce) << 1));
    nConfigValue &= ~(1<<10);
    WRITE_WORD(_gnBaseRegAddr + ((0x1026ce) << 1), nConfigValue);
    nConfigValue = READ_WORD(_gnBaseRegAddr + ((0x1026a2) << 1));
    if(bEnable)
        nConfigValue |= (1<<10);
    else
        nConfigValue &= ~(1<<10);
    WRITE_WORD(_gnBaseRegAddr + ((0x1026a2) << 1), nConfigValue);
}

void _HalAudAtopLineOut(BOOL bEnable)
{
    _HalAudAtopAmp(bEnable);
    if (bEnable)
    {
        _gbDacActive = TRUE;
    }
    else
    {
        _gbDacActive = FALSE;
    }
}


void _HalAudAtopEnableRef(BOOL bEnable)
{

    if (bEnable)
    {
        _gbAnalogIdle = FALSE;
    }
    else
    {
        _gbAnalogIdle = TRUE;
    }

}

BOOL HalAudAtopOpen(AudAtopPath_e ePath)
{
    if(ePath!= AUD_ATOP_ADC && ePath!= AUD_ATOP_DAC)
    {
        return FALSE;
    }
    else
    {
        if(_gbAnalogIdle)
        {
            _HalAudAtopEnableRef(TRUE);
        }


        switch(ePath)
        {
            case AUD_ATOP_ADC:
                if(!_gbAdcActive)
                {
                    if(_gnAdcSelect==0)
                        _HalAudAtopLineIn(TRUE);
                    else
                        _HalAudAtopMicIn(TRUE);
                }
                break;

            case AUD_ATOP_DAC:
                if(!_gbDacActive)
                {
                    _HalAudAtopLineOut(TRUE);
                }
                break;

            default:
                //ERRMSG("HalAudAtopOpen - ERROR AudAtopPath_e case!");
                return FALSE;

        }
        return TRUE;


    }

}

BOOL HalAudAtopClose(AudAtopPath_e ePath)
{
    if(ePath!= AUD_ATOP_ADC && ePath!= AUD_ATOP_DAC)
    {
        return FALSE;
    }
    else
    {
        switch(ePath)
        {
            case AUD_ATOP_ADC:
                if(_gbAdcActive)
                {
                    if(_gnAdcSelect==0)
                        _HalAudAtopLineIn(FALSE);
                    else
                        _HalAudAtopMicIn(FALSE);
                }
                break;

            case AUD_ATOP_DAC:
                if(_gbDacActive)
                {
                    _HalAudAtopLineOut(FALSE);
                }
                break;
            default:
                //ERRMSG("[Audio Hal] HalAudAtopClose - ERROR AudAtopPath_e case!");
                return FALSE;
        }


        if(!_gbAnalogIdle && !(_gbAdcActive || _gbDacActive ))
        {
            _HalAudAtopEnableRef(FALSE);
        }

        return TRUE;
    }
}

BOOL HalAudAtopMicAmpGain(U16 nSel)
{
    if(nSel>=sizeof(_gaMicPreGainTable)/sizeof(_gaMicPreGainTable[0]))
    {
        //ERRMSG("HalAudAtopMicAmpGain - ERROR!! not Support.\n");
        return FALSE;
    }

    _gnMicPreGain = nSel;

    _AudWriteReg(AUD_REG_BANK1, REG_AUSDM_CFG14, (SEL_GAIN_L_MICAMP0_MSK|SEL_GAIN_R_MICAMP0_MSK), (_gaMicPreGainTable[_gnMicPreGain]<<SEL_GAIN_L_MICAMP0_POS)|(_gaMicPreGainTable[_gnMicPreGain]<<SEL_GAIN_R_MICAMP0_POS));

    return TRUE;
}

BOOL HalAudAtopAdcGain(AudAdcPath_e eAdc, U16 nSel)
{


	if(eAdc == AUD_ADC_LINEIN)
	{
        if(nSel>=sizeof(_gaLineinGainTable)/sizeof(_gaLineinGainTable[0]))
        {
            //ERRMSG("HalAudAtopAdcGain - ERROR!! not Support .\n");
            return FALSE;
        }
		_gnLineInGain = nSel;

        if(eAdc==_gnAdcSelect)
        {
            _AudWriteReg(AUD_REG_BANK1, REG_AUSDM_CFG5, (AUD_SEL_MICGAIN_INMUX_L|AUD_SEL_MICGAIN_INMUX_R), (_gaLineinGainTable[nSel][0]?(AUD_SEL_MICGAIN_INMUX_L|AUD_SEL_MICGAIN_INMUX_R):0));
            _AudWriteReg(AUD_REG_BANK1, REG_AUSDM_CFG5, (AUD_SEL_GAIN_INMUX0_L_MSK|AUD_SEL_GAIN_INMUX0_R_MSK), (_gaLineinGainTable[nSel][1]<<AUD_SEL_GAIN_INMUX0_L_POS)|(_gaLineinGainTable[nSel][1]<<AUD_SEL_GAIN_INMUX0_R_POS));
        }
	}
	else if(eAdc == AUD_ADC_MICIN)
	{
        if(nSel>=sizeof(_gaMicinGainTable)/sizeof(_gaMicinGainTable[0]))
        {
            //ERRMSG("HalAudAtopAdcGain - ERROR!! not Support .\n");
            return FALSE;
        }
		_gnMicInGain = nSel;

        if(eAdc==_gnAdcSelect)
        {
            _AudWriteReg(AUD_REG_BANK1, REG_AUSDM_CFG5, (AUD_SEL_MICGAIN_INMUX_L|AUD_SEL_MICGAIN_INMUX_R), (_gaMicinGainTable[nSel][0]?(AUD_SEL_MICGAIN_INMUX_L|AUD_SEL_MICGAIN_INMUX_R):0));
            _AudWriteReg(AUD_REG_BANK1, REG_AUSDM_CFG5, (AUD_SEL_GAIN_INMUX0_L_MSK|AUD_SEL_GAIN_INMUX0_R_MSK), (_gaMicinGainTable[nSel][1]<<AUD_SEL_GAIN_INMUX0_L_POS)|(_gaMicinGainTable[nSel][1]<<AUD_SEL_GAIN_INMUX0_R_POS));
        }
	}
	else
	{
        //ERRMSG("HalAudAtopAdcGain - ERROR!! ADC source error.\n");
        return FALSE;
    }


    return TRUE;
}

BOOL HalAudAtopAdcInmux(AudAdcPath_e eAdcPath)
{
    BOOL bWork = FALSE;
    if(_gnAdcSelect != eAdcPath && _gbAdcActive)
    {
        HalAudAtopClose(AUD_ATOP_ADC);
        bWork = TRUE;
    }
    switch(eAdcPath)
    {
        case AUD_ADC_LINEIN:
            _AudWriteReg(AUD_REG_BANK1, REG_AUSDM_CFG4, (AUD_SEL_CH_INMUX0_L_MSK|AUD_SEL_CH_INMUX0_R_MSK), 0);
            _gnAdcSelect = 0;
            break;
        case AUD_ADC_MICIN:
            _AudWriteReg(AUD_REG_BANK1, REG_AUSDM_CFG4, (AUD_SEL_CH_INMUX0_L_MSK|AUD_SEL_CH_INMUX0_R_MSK), (0x7<<AUD_SEL_CH_INMUX0_L_POS)|(0x7<<AUD_SEL_CH_INMUX0_R_POS));
            _gnAdcSelect = 1;
            break;
        default:
            return FALSE;
    }
    if(bWork)
    {
        HalAudAtopOpen(AUD_ATOP_ADC);
    }
    return TRUE;
}

void HalAudConfigInit(void)
{
    U16 i;
    for(i=0; i<AUD_DMA_NUM; i++)
    {
        _gbDmaClkBypass[i]=FALSE;
    }

    _gaDmaClk[AUD_DMA_READER1].eRate = AUD_RATE_48K;
    _gaDmaClk[AUD_DMA_READER1].eSrc = AUD_DMA_CLK_GPA;

    _gaDmaClk[AUD_DMA_READER2].eRate = AUD_RATE_48K;
    _gaDmaClk[AUD_DMA_READER2].eSrc = AUD_DMA_CLK_GPA;

    _gaDmaClk[AUD_DMA_READER3].eRate = AUD_RATE_48K;
    _gaDmaClk[AUD_DMA_READER3].eSrc = AUD_DMA_CLK_GPA;

    _gaDmaClk[AUD_DMA_READER4].eRate = AUD_RATE_48K;
    _gaDmaClk[AUD_DMA_READER4].eSrc = AUD_DMA_CLK_GPA;

    _gaDmaClk[AUD_DMA_READER5].eRate = AUD_RATE_48K;
    _gaDmaClk[AUD_DMA_READER5].eSrc = AUD_DMA_CLK_SYNTH;

    _gaDmaClk[AUD_DMA_WRITER1].eRate = AUD_RATE_48K;
    _gaDmaClk[AUD_DMA_WRITER1].eSrc = AUD_DMA_CLK_ADC;

    _gaDmaClk[AUD_DMA_WRITER2].eRate = AUD_RATE_48K;
    _gaDmaClk[AUD_DMA_WRITER2].eSrc = AUD_DMA_CLK_GPA;

    _gaDmaClk[AUD_DMA_WRITER3].eRate = AUD_RATE_48K;
    _gaDmaClk[AUD_DMA_WRITER3].eSrc = AUD_DMA_CLK_GPA;

    _gaDmaClk[AUD_DMA_WRITER4].eRate = AUD_RATE_48K;
    _gaDmaClk[AUD_DMA_WRITER4].eSrc = AUD_DMA_CLK_GPA;

    _gaDmaClk[AUD_DMA_WRITER5].eRate = AUD_RATE_16K;
    _gaDmaClk[AUD_DMA_WRITER5].eSrc = AUD_DMA_CLK_PDM;

}

void HalAudSysInit(void)
{

    //---------------- AUPLL source clk --------------------- //Item xx
    //2nd order synthesizer
    _AudWriteRegByte(0x311207, 0x01, 0x01);   //[8] Enable reference clock
    _AudWriteRegByte(0x311222, 0xff, 0x40);   //[15:0] N.F[23:8]
    _AudWriteRegByte(0x311223, 0xff, 0x1f);
    _AudWriteRegByte(0x311220, 0xff, 0x00);   //[ 7:0] N.F[ 7:0]
    _AudWriteRegByte(0x311221, 0xff, 0xc0);   //[15] enable [14] load 0->1 [9:8] ref clk upll 480MHz
    _AudWriteRegByte(0x311221, 0xff, 0x80);   //[14] load 1->0

//---------------- AUPLL control ------------------------- //Item xx
    _AudWriteRegByte(0x311207, 0x02, 0x02);   //[9] Enable reference clock
    _AudWriteRegByte(0x311170, 0xff, 0xd0);
    _AudWriteRegByte(0x311171, 0xff, 0x12);
    _AudWriteRegByte(0x311177, 0xff, 0x03);   // [15: 8] LoopDivSecond
    _AudWriteRegByte(0x31116e, 0x30, 0x20);   // [ 5: 4] EN LPFSW
    _AudWriteRegByte(0x311176, 0xff, 0x00);   // Power Down

//---------------- Enable CLK ---------------------------- //Item xx
    _AudWriteRegByte(0x311206, 0xff, 0xff);
    _AudWriteRegByte(0x311207, 0xff, 0xff);
    _AudWriteRegByte(0x311208, 0xff, 0xff);
    _AudWriteRegByte(0x311209, 0xff, 0xff);
    _AudWriteRegByte(0x31120a, 0xff, 0xff);

//---------------- CLK selection ------------------------- //Item xx
    _AudWriteRegByte(0x311210, 0xff, 0x80);   // All ref_clk use default
    _AudWriteRegByte(0x311211, 0xff, 0x00);   // All ref_clk use default
    _AudWriteRegByte(0x31120e, 0xff, 0x01);   // ADC <= GPA 256x48K
    _AudWriteRegByte(0x31120f, 0xff, 0x01);   // DAC <= GPA 256x48K
    _AudWriteRegByte(0x31120c, 0xff, 0x00);   // BCK = 32Fs for 16bit nonTDM
    _AudWriteRegByte(0x31120d, 0x0f, 0x00);   // BCK = 32Fs for 16bit nonTDM
    _AudWriteRegByte(0x311212, 0xff, 0x11);   // CODEC Rx: A1o      <= GPA 240x48
    _AudWriteRegByte(0x311213, 0xff, 0x00);   // CODEC Rx: A1i      <= CODEC_RX_256FS_M
    _AudWriteRegByte(0x311214, 0xff, 0x11);   // BT    Rx: A2o      <= GPA 240x48
    _AudWriteRegByte(0x311215, 0xff, 0x00);   // BT    Rx: A2i      <= BT_RX_256FS_M
    _AudWriteRegByte(0x311216, 0xff, 0x01);   // CODEC Tx: DMA_R2   <= GPA 256x48
    _AudWriteRegByte(0x311218, 0xff, 0x00);   // CODEC Tx: DMA_R2   <= GPA 256x48
    // CODEC Tx: SRC_Bi   <= DMA_R2
    _AudWriteRegByte(0x311219, 0xff, 0x00);   // CODEC Tx: CODEC_TX <= CODEC_TX_256FS_M
    // CODEC Tx: SRC_Bo   <= CODEC_TX
    _AudWriteRegByte(0x31121a, 0xff, 0x01);   // BT    Tx: DMA_R3   <= GPA 256x48
    _AudWriteRegByte(0x31121c, 0xff, 0x00);   // BT    Tx: DMA_R3   <= GPA 256x48
    // BT    Tx: SRC_Ci   <= DMA_R3
    _AudWriteRegByte(0x31121d, 0xff, 0x10);   // BT    Tx: BT_TX    <= BT_TX_256FS_S, Note: A2i (4 wire mode) fail!!!
    // BT    Tx: SRC_Co   <= BT_TX

//----------------- NF Synthesizer -------------------------- //Item xx
    _AudWriteRegByte(0x3112d8, 0xff, 0x00);   // MISC  MCK N.F [ 7: 0]
    _AudWriteRegByte(0x3112d9, 0xff, 0x00);   // MISC  MCK N.F [15: 8]
    _AudWriteRegByte(0x3112da, 0xff, 0x1e);   // MISC  MCK N.F [23:16]
    _AudWriteRegByte(0x3112db, 0xff, 0x01);   // MISC  MCK EXP [ 7: 0]
    _AudWriteRegByte(0x3112dc, 0xff, 0x00);   // CODEC MCK N.F [ 7: 0]
    _AudWriteRegByte(0x3112dd, 0xff, 0x00);   // CODEC MCK N.F [15: 8]
    _AudWriteRegByte(0x3112de, 0xff, 0x1e);   // CODEC MCK N.F [23:16]
    _AudWriteRegByte(0x3112df, 0xff, 0x01);   // CODEC MCK EXP [ 7: 0]
    _AudWriteRegByte(0x311228, 0xff, 0x00);   // CODEC_RX N.F [ 7: 0]
    _AudWriteRegByte(0x311229, 0xff, 0x00);   // CODEC_RX N.F [15: 0]
    _AudWriteRegByte(0x31122a, 0xff, 0x00);   // CODEC_RX N.F [23:16]
    _AudWriteRegByte(0x31122b, 0xff, 0x0F);   // CODEC_RX N.F [30:24]
    _AudWriteRegByte(0x31122c, 0xff, 0x00);   // CODEC_TX N.F [ 7: 0]
    _AudWriteRegByte(0x31122d, 0xff, 0x00);   // CODEC_TX N.F [15: 0]
    _AudWriteRegByte(0x31122e, 0xff, 0x00);   // CODEC_TX N.F [23:16]
    _AudWriteRegByte(0x31122f, 0xff, 0x0F);   // CODEC_TX N.F [30:24]
    _AudWriteRegByte(0x311230, 0xff, 0x00);   // BT_RX N.F    [ 7: 0]
    _AudWriteRegByte(0x311231, 0xff, 0x00);   // BT_RX N.F    [15: 0]
    _AudWriteRegByte(0x311232, 0xff, 0x00);   // BT_RX N.F    [23:16]
    _AudWriteRegByte(0x311233, 0xff, 0x0F);   // BT_RX N.F    [30:24]
    _AudWriteRegByte(0x311234, 0xff, 0x00);   // BT_TX N.F    [ 7: 0]
    _AudWriteRegByte(0x311235, 0xff, 0x00);   // BT_TX N.F    [15: 0]
    _AudWriteRegByte(0x311236, 0xff, 0x00);   // BT_TX N.F    [23:16]
    _AudWriteRegByte(0x311237, 0xff, 0x0F);   // BT_TX N.F    [30:24]
    _AudWriteRegByte(0x311238, 0xff, 0xff);   // en/ trig 0->1
    _AudWriteRegByte(0x311239, 0xff, 0xff);   // en/ trig 0->1
    _AudWriteRegByte(0x311238, 0xff, 0xaa);   // en/ trig 1->0
    _AudWriteRegByte(0x311239, 0xff, 0xaa);   // en/ trig 1->0

//----------------- USB SOF Synthesizer -------------------------- //Item xx

//----------------- AudioBand ---------------------------- //Item xx
    //DAC
    _AudWriteRegByte(0x311018, 0xff, 0xff);   // [ 7: 0] DWA fix value
    _AudWriteRegByte(0x311019, 0xff, 0x01);   // [15: 8] DWA fix value
    _AudWriteRegByte(0x311016, 0xff, 0x98);   // [ 1: 0] DWA fix value[17:16]
    // [    7] DAC dither enable
    // [ 6: 4] DAC dither sel 9level
    // [ 3: 2] DAC dither sel 17level
    _AudWriteRegByte(0x311010, 0xff, 0x90);   // [    7] DWA fix enable
    // [    6] SDMIN_FIX
    // [    5] SDM_TEST_EN
    // [    4] FIX_MSB_EN
    // [ 3: 0] FIX_MSB_SEL
    _AudWriteRegByte(0x311011, 0xff, 0x70);   // [   15] FIFO_SWRST
    // [   14] FIFO_AUTO_SYNC
    // [   13] DC_OFFSET_EN
    // [   12] FIFO_INIT_SRAM   0 -> 1 -> 0
    // [   10] DWA_SHIFT_DIS
    // [    9] FIRST_ORDER_EN
    // [    8] SDM_EN           0 ------> 1
    _AudWriteRegByte(0x311012, 0xff, 0x80);   // DC offset value 17L
    _AudWriteRegByte(0x311013, 0xff, 0x00);   //
    _AudWriteRegByte(0x311014, 0xff, 0x80);   // DC offset value 9L
    _AudWriteRegByte(0x311015, 0xff, 0x00);   //
    _AudWriteRegByte(0x31100c, 0x40, 0x40);   // [    6] FIFO NewMD
    //ADC
    _AudWriteRegByte(0x311004, 0xff, 0x70);   // [ 7: 0] CIC CTRL
    _AudWriteRegByte(0x311005, 0xff, 0x08);   // [11: 8] DEC NUM
    _AudWriteRegByte(0x311006, 0xff, 0x02);   // [ 7: 0] dither lvl
    _AudWriteRegByte(0x311007, 0xff, 0x10);   // [   12] FIFO AUTO SYNC
    //SRC A1
    _AudWriteRegByte(0x311020, 0xff, 0x23);   // [    5] FIFO NewMD
    // [    1] FIFO AUTO SYNC
    // [    0] CIC EN
    _AudWriteRegByte(0x311021, 0x01, 0x01);   // [    8] init SRAM
    //SRC A2
    _AudWriteRegByte(0x311022, 0xff, 0x23);   // [    5] FIFO NewMD
    // [    1] FIFO AUTO SYNC
    // [    0] CIC EN
    _AudWriteRegByte(0x311023, 0x01, 0x01);  // [    8] init SRAM
    //SRC B
    _AudWriteRegByte(0x311024, 0xff, 0x23);   // [    5] FIFO NewMD
    // [    1] FIFO AUTO SYNC
    // [    0] CIC EN
    _AudWriteRegByte(0x311025, 0x01, 0x01);   // [    8] init SRAM
    //SRC C
    _AudWriteRegByte(0x311026, 0xff, 0x23);   // [    5] FIFO NewMD
    // [    1] FIFO AUTO SYNC
    // [    0] CIC EN
    _AudWriteRegByte(0x311027, 0x01, 0x01);   // [    8] init SRAM
    //MAC
    _AudWriteRegByte(0x311028, 0x01, 0x01);   // [    0] init SRAM

//----------------- AudioBand 2 ---------------------------- //Item xx
    // Release init SRAM
    _AudWriteRegByte(0x311021, 0x01, 0x00);   // [    8] init SRAM
    _AudWriteRegByte(0x311023, 0x01, 0x00);   // [    8] init SRAM
    _AudWriteRegByte(0x311025, 0x01, 0x00);   // [    8] init SRAM
    _AudWriteRegByte(0x311027, 0x01, 0x00);   // [    8] init SRAM
    _AudWriteRegByte(0x311028, 0x01, 0x00);   // [    8] init SRAM
    _AudWriteRegByte(0x311010, 0x80, 0x00);   // [    7] DWA fix enable
    _AudWriteRegByte(0x311011, 0x11, 0x01);   // [   12] FIFO_INIT_SRAM   0 -> 1 -> 0
    // [    8] SDM_EN           0 ------> 1
    //VLD
    _AudWriteRegByte(0x311080, 0xf0, 0x80);   // [    7] ADC VLD EN
    _AudWriteRegByte(0x311081, 0xf0, 0x80);   // [   15] DAC VLD EN
    _AudWriteRegByte(0x311082, 0xff, 0x81);   // [    7] A1o VLD EN
    // [    0] 240FS
    _AudWriteRegByte(0x311083, 0xff, 0x80);   // [   15] A1i VLD EN
    _AudWriteRegByte(0x311084, 0xff, 0x81);   // [    7] A2o VLD EN
    // [    0] 240FS
    _AudWriteRegByte(0x311085, 0xff, 0x80);   // [   15] A2i VLD EN
    _AudWriteRegByte(0x311086, 0xff, 0x80);   // [    7] Bo VLD EN
    // [    0] 240FS
    _AudWriteRegByte(0x311087, 0xff, 0x80);   // [   15] Ci VLD EN
    _AudWriteRegByte(0x311088, 0xff, 0x80);   // [    7] Co VLD EN
    // [    0] 240FS
    _AudWriteRegByte(0x311089, 0xff, 0x80);   // [   15] Ci VLD EN

    //atop init
    HalAudAtopInit();

//---------------- Mixer --------------------------- //Item xx
    _AudWriteRegByte(0x31128a, 0xff, 0x80);  // [ 9: 6] BT2CODEC L/R
    _AudWriteRegByte(0x31128b, 0xff, 0x2a);  // [13:10] BT2DAC_L/R
//---------------- MIU Arbiter --------------------------- //Item xx
    _AudWriteRegByte(0x311588, 0xff, 0xff);
    _AudWriteRegByte(0x311589, 0xff, 0xff);
    _AudWriteRegByte(0x311586, 0xff, 0x00);
    _AudWriteRegByte(0x311587, 0xff, 0x00);
    _AudWriteRegByte(0x311584, 0xff, 0x00);
    _AudWriteRegByte(0x311585, 0xff, 0x04);
    _AudWriteRegByte(0x311582, 0xff, 0x08);
    _AudWriteRegByte(0x311583, 0xff, 0x20);
    _AudWriteRegByte(0x311580, 0xff, 0x15);
    _AudWriteRegByte(0x311581, 0xff, 0x80);

//---------------- I2S Tx/Rx --------------------------- //Item xx
#if 0
   //MCK = SYNTH CLK
    _AudWriteRegByte(0x3112d6, 0xff, 0xa0);   // [ 7: 6] REG_SEL_CODEC_I2S_MCK_O
    // [ 5: 4] REG_SEL_MISC_I2S_MCK_O
    //CODEC Rx
    _AudWriteRegByte(0x311244, 0xff, 0x40);   // [    6] 0:i2s  1:left-justified
    _AudWriteRegByte(0x311245, 0xff, 0xc0);   // [   15] BCK enable
    // [   14] 0: slave  1: master
    // [   13] tdm mode
    // [    9] 32 bit
    //CODEC Tx
    _AudWriteRegByte(0x311253, 0xff, 0x0f);   // [12: 8] wck width=15 (0~31)
    _AudWriteRegByte(0x311246, 0xff, 0xc0);   // [    7] 0:duty 50  1:programmable
    // [    6] 0:i2s  1:left-justified (Tx LJF fail)
    _AudWriteRegByte(0x311247, 0xff, 0xc0);   // [   15] BCK enable
    // [   14] 0: slave  1: master
    // [   13] tdm mode
    // [    9] 32 bit
    //BT Rx (fake 4-wire mode)
    _AudWriteRegByte(0x311248, 0xff, 0x40);   // [    6] 0:i2s  1:left-justified
    _AudWriteRegByte(0x311249, 0xff, 0xc0);   // [   15] BCK enable
    // [   14] 0: slave  1: master
    // [   13] tdm mode
    // [    9] 32 bit
    //BT Tx (fake 4-wire mode)
    _AudWriteRegByte(0x311255, 0xff, 0x0f);   // [12: 8] wck width=15 (0~31)
    _AudWriteRegByte(0x31124a, 0xff, 0xc0);   // [    7] 0:duty 50  1:programmable
    // [    6] 0:i2s  1:left-justified (Tx LJF fail)
    _AudWriteRegByte(0x31124b, 0xff, 0x80);   // [   15] BCK enable
    // [   14] 0: slave  1: master
    // [   13] tdm mode
    // [    9] 32 bit
    //MISC Rx
    _AudWriteRegByte(0x31124c, 0xff, 0x40);   // [    6] 0:i2s  1:left-justified
    _AudWriteRegByte(0x31124d, 0xff, 0xc0);   // [   15] BCK enable
    // [   14] 0: slave  1: master
    // [   13] tdm mode
    // [    9] 32 bit
    //MISC Tx
    _AudWriteRegByte(0x311257, 0xff, 0x0f);   // [12: 8] wck width=15 (0~31)
    _AudWriteRegByte(0x31124e, 0xff, 0xc0);   // [    7] 0:duty 50  1:programmable
    // [    6] 0:i2s  1:left-justified (Tx LJF fail)
    _AudWriteRegByte(0x31124f, 0xff, 0xc0);   // [   15] BCK enable
    // [   14] 0: slave  1: master
    // [   13] tdm mode
    // [    9] 32 bit
#else
//MCK = SYNTH CLK
    _AudWriteRegByte(0x3112d6, 0xff, 0xa0);   // [ 7: 6] REG_SEL_CODEC_I2S_MCK_O
    // [ 5: 4] REG_SEL_MISC_I2S_MCK_O
    //CODEC Rx
    _AudWriteRegByte(0x311244, 0xff, 0x40);   // [    6] 0:i2s  1:left-justified
    _AudWriteRegByte(0x311245, 0xff, 0x40);   // [   15] BCK enable
    // [   14] 0: slave  1: master
    // [   13] tdm mode
    // [    9] 32 bit
    //CODEC Tx
    _AudWriteRegByte(0x311253, 0xff, 0x0f);   // [12: 8] wck width=15 (0~31)
    _AudWriteRegByte(0x311246, 0xff, 0xc0);   // [    7] 0:duty 50  1:programmable
    // [    6] 0:i2s  1:left-justified (Tx LJF fail)
    _AudWriteRegByte(0x311247, 0xff, 0x40);   // [   15] BCK enable
    // [   14] 0: slave  1: master
    // [   13] tdm mode
    // [    9] 32 bit
    //BT Rx (fake 4-wire mode)
    _AudWriteRegByte(0x311248, 0xff, 0x40);   // [    6] 0:i2s  1:left-justified
    _AudWriteRegByte(0x311249, 0xff, 0x40);   // [   15] BCK enable
    // [   14] 0: slave  1: master
    // [   13] tdm mode
    // [    9] 32 bit
    //BT Tx (fake 4-wire mode)
    _AudWriteRegByte(0x311255, 0xff, 0x0f);   // [12: 8] wck width=15 (0~31)
    _AudWriteRegByte(0x31124a, 0xff, 0xc0);   // [    7] 0:duty 50  1:programmable
    // [    6] 0:i2s  1:left-justified (Tx LJF fail)
    _AudWriteRegByte(0x31124b, 0xff, 0x00);   // [   15] BCK enable
    // [   14] 0: slave  1: master
    // [   13] tdm mode
    // [    9] 32 bit
    //MISC Rx
    _AudWriteRegByte(0x31124c, 0xff, 0x40);   // [    6] 0:i2s  1:left-justified
    _AudWriteRegByte(0x31124d, 0xff, 0x40);   // [   15] BCK enable
    // [   14] 0: slave  1: master
    // [   13] tdm mode
    // [    9] 32 bit
    //MISC Tx
    _AudWriteRegByte(0x311257, 0xff, 0x0f);   // [12: 8] wck width=15 (0~31)
    _AudWriteRegByte(0x31124e, 0xff, 0xc0);   // [    7] 0:duty 50  1:programmable
    // [    6] 0:i2s  1:left-justified (Tx LJF fail)
    _AudWriteRegByte(0x31124f, 0xff, 0x40);   // [   15] BCK enable
    // [   14] 0: slave  1: master
    // [   13] tdm mode
    // [    9] 32 bit
#endif
//---------------- HDMI Tx --------------------------- //Item xx
    _AudWriteRegByte(0x3112e8, 0xff, 0x8f);   // ENABLE
    _AudWriteRegByte(0x3112e9, 0xff, 0x01);   // ENABLE
    _AudWriteRegByte(0x3112e4, 0xff, 0x00);   // NF_SYNTH_VALUE[15:0]
    _AudWriteRegByte(0x3112e5, 0xff, 0x00);   // NF_SYNTH_VALUE[15:0]
    _AudWriteRegByte(0x3112e6, 0xff, 0x00);   // NF_SYNTH_VALUE[30:16]
    _AudWriteRegByte(0x3112e7, 0xff, 0x0f);   // NF_SYNTH_VALUE[30:16]
    _AudWriteRegByte(0x3112e7, 0xff, 0x8f);   // NF_SYNTH_TRIG 0->1
    _AudWriteRegByte(0x3112e7, 0xff, 0x0f);   // NF_SYNTH_TRIG 1->0

//---------------- Digital Microphone --------------------------- //Item xx
    _AudWriteRegByte(0x311402, 0xff, 0x31);   //sel enable
    _AudWriteRegByte(0x311403, 0xff, 0x00);
    _AudWriteRegByte(0x311404, 0xff, 0x00);   //[0]=1 for sel imi
    _AudWriteRegByte(0x311405, 0xff, 0x00);
    _AudWriteRegByte(0x311408, 0xff, 0x09);   //1600MHz & 16k
    _AudWriteRegByte(0x311409, 0xff, 0x00);
    _AudWriteRegByte(0x31140c, 0xff, 0x00);   //audioband sram init
    _AudWriteRegByte(0x31140d, 0xff, 0x80);
    _AudWriteRegByte(0x31140c, 0xff, 0x00);
    _AudWriteRegByte(0x31140d, 0xff, 0x00);
    _AudWriteRegByte(0x311420, 0xff, 0x00);   //sel pd
    _AudWriteRegByte(0x311421, 0xff, 0x00);
    _AudWriteRegByte(0x311422, 0xff, 0x01);   //sel
    _AudWriteRegByte(0x311423, 0xff, 0x00);
    _AudWriteRegByte(0x311424, 0xff, 0x00);   //sel
    _AudWriteRegByte(0x311425, 0xff, 0x80);
    _AudWriteRegByte(0x311426, 0xff, 0x01);   //sel
    _AudWriteRegByte(0x311427, 0xff, 0x00);
    _AudWriteRegByte(0x311428, 0xff, 0x00);   //sel
    _AudWriteRegByte(0x311429, 0xff, 0x80);
    _AudWriteRegByte(0x31142a, 0xff, 0x01);   //sel
    _AudWriteRegByte(0x31142b, 0xff, 0x00);
    _AudWriteRegByte(0x31142c, 0xff, 0x00);   //sel
    _AudWriteRegByte(0x31142d, 0xff, 0x80);
    _AudWriteRegByte(0x31142e, 0xff, 0x01);   //sel
    _AudWriteRegByte(0x31142f, 0xff, 0x00);
    _AudWriteRegByte(0x311430, 0xff, 0x00);   //sel
    _AudWriteRegByte(0x311431, 0xff, 0x80);
    _AudWriteRegByte(0x311440, 0xff, 0x01);   //cic
    _AudWriteRegByte(0x311441, 0xff, 0x00);
    _AudWriteRegByte(0x31140c, 0xff, 0x1f);   //audioband/timeGen enable
    _AudWriteRegByte(0x31140d, 0xff, 0x01);

//---------------- Audio Reset --------------------------- //Item 24
    _AudWriteRegByte(0x311200, 0xff, 0xda);
    _AudWriteRegByte(0x311201, 0xff, 0x3f);   // RSTZ except Synthesizers
    _AudWriteRegByte(0x311202, 0x80, 0x80);   // RSTZ All AudioBand
    _AudWriteRegByte(0x311200, 0xff, 0x00);
    _AudWriteRegByte(0x311201, 0xff, 0x00);   // RSTZ except Synthesizers
    _AudWriteRegByte(0x311202, 0x80, 0x00);   // RSTZ All AudioBand


    //DMA L/R
    _AudWriteRegByte(0x311300, 0x08, 0x08);
    _AudWriteRegByte(0x311320, 0x08, 0x08);
    _AudWriteRegByte(0x311340, 0x08, 0x08);
    _AudWriteRegByte(0x311360, 0x08, 0x08);
    _AudWriteRegByte(0x311380, 0x08, 0x08);
    _AudWriteRegByte(0x3113a0, 0x08, 0x08);
    _AudWriteRegByte(0x3113c0, 0x08, 0x08);
    _AudWriteRegByte(0x3113e0, 0x08, 0x08);
    _AudWriteRegByte(0x311460, 0x08, 0x08);
    _AudWriteRegByte(0x311500, 0x08, 0x08);

    _AudWriteRegByte(0x311301, 0x80, 0x80);
    _AudWriteRegByte(0x311321, 0x80, 0x80);
    _AudWriteRegByte(0x311341, 0x80, 0x80);
    _AudWriteRegByte(0x311361, 0x80, 0x80);
    _AudWriteRegByte(0x311381, 0x80, 0x80);
    _AudWriteRegByte(0x3113a1, 0x80, 0x80);
    _AudWriteRegByte(0x3113c1, 0x80, 0x80);
    _AudWriteRegByte(0x3113e1, 0x80, 0x80);
    _AudWriteRegByte(0x311461, 0x80, 0x80);
    _AudWriteRegByte(0x311501, 0x80, 0x80);

    _AudWriteRegByte(0x311301, 0x80, 0x00);
    _AudWriteRegByte(0x311321, 0x80, 0x00);
    _AudWriteRegByte(0x311341, 0x80, 0x00);
    _AudWriteRegByte(0x311361, 0x80, 0x00);
    _AudWriteRegByte(0x311381, 0x80, 0x00);
    _AudWriteRegByte(0x3113a1, 0x80, 0x00);
    _AudWriteRegByte(0x3113c1, 0x80, 0x00);
    _AudWriteRegByte(0x3113e1, 0x80, 0x00);
    _AudWriteRegByte(0x311461, 0x80, 0x00);
    _AudWriteRegByte(0x311501, 0x80, 0x00);

    _AudWriteRegByte(0x102600, 0x01, 0x00);   //[0] reg_allpad_in
    _AudWriteRegByte(0x102604, 0x07, 0x06);   //[0]  reg_bt_i2s_rx_0_mode
                               //[1]  reg_bt_i2s_trx_0_mode
                               //[2]  reg_bt_i2s_tx_0_mode
    _AudWriteRegByte(0x102607, 0xfe, 0xfe);   //[9]  reg_codec_i2s_rx_0_mode
                               //[10] reg_codec_i2s_rx_1_mode
                               //[11] reg_codec_i2s_rx_2_mode
                               //[12] reg_codec_i2s_rx_3_mode
                               //[13] reg_codec_i2s_rx_mck_mode
                               //[14] reg_codec_i2s_tx_0_mode
                               //[15] reg_codec_i2s_tx_mute_mode
    _AudWriteRegByte(0x102608, 0xff, 0x55);   //[7:6]reg_dmic_3_mode[1:0] //CH3
                               //[5:4]reg_dmic_2_mode[1:0] //CH2
                               //[3:2]reg_dmic_1_mode[1:0] //CH1
                               //[1:0]reg_dmic_0_mode[1:0] //BCK & CH0
    _AudWriteRegByte(0x10260e, 0xc0, 0xc0);   //[6]  reg_misc_i2s_rx_0_mode
                               //[7]  reg_misc_i2s_rx_mck_mode
    _AudWriteRegByte(0x10260f, 0x07, 0x07);   //[8]  reg_misc_i2s_tx_0_mode
                               //[9]  reg_misc_i2s_tx_1_mode
                               //[10] reg_misc_i2s_tx_2_mode

    //_AudWriteRegByte(0x311406, 0xff, 0x21);
    //_AudWriteRegByte(0x311407, 0xff, 0x20);

}


BOOL HalAudDmicCicCtrl(BOOL bEnable)
{
    _AudWriteReg(AUD_REG_BANK4, REG_CIC_CTRL0, AUD_DMIC_CIC_RESET_MSK, (0<<AUD_DMIC_CIC_RESET_POS));

    _AudWriteReg(AUD_REG_BANK4, REG_CIC_CTRL0, AUD_DMIC_CIC_EN_MSK, (bEnable<<AUD_DMIC_CIC_EN_POS));
    return TRUE;
}

BOOL HalAudDmicSetGain(U16 nGain)
{
    if(nGain>7)
        return FALSE;

    _AudWriteReg(AUD_REG_BANK4, REG_CIC_CTRL1, AUD_DMIC_GAIN1_MSK, (nGain<<AUD_DMIC_GAIN1_POS));
    _AudWriteReg(AUD_REG_BANK4, REG_CIC_CTRL1, AUD_DMIC_GAIN2_MSK, (nGain<<AUD_DMIC_GAIN2_POS));

    _AudWriteReg(AUD_REG_BANK4, REG_CIC_CTRL2, AUD_DMIC_GAIN3_MSK, (nGain<<AUD_DMIC_GAIN3_POS));
    _AudWriteReg(AUD_REG_BANK4, REG_CIC_CTRL2, AUD_DMIC_GAIN4_MSK, (nGain<<AUD_DMIC_GAIN4_POS));

    _AudWriteReg(AUD_REG_BANK4, REG_CIC_CTRL3, AUD_DMIC_GAIN5_MSK, (nGain<<AUD_DMIC_GAIN5_POS));
    _AudWriteReg(AUD_REG_BANK4, REG_CIC_CTRL3, AUD_DMIC_GAIN6_MSK, (nGain<<AUD_DMIC_GAIN6_POS));

    _AudWriteReg(AUD_REG_BANK4, REG_CIC_CTRL4, AUD_DMIC_GAIN7_MSK, (nGain<<AUD_DMIC_GAIN7_POS));
    _AudWriteReg(AUD_REG_BANK4, REG_CIC_CTRL4, AUD_DMIC_GAIN8_MSK, (nGain<<AUD_DMIC_GAIN8_POS));

    return TRUE;
}


BOOL HalAudDmicSetChannel(S16 nChannels)
{
    U16 nConfigValue;
    switch(nChannels)
    {
        case 1:
            nConfigValue=0;
        case 2:
            nConfigValue=1;
        case 4:
            nConfigValue=2;
        case 8:
            nConfigValue=3;
        default:
            return FALSE;
    }
    _AudWriteReg(AUD_REG_BANK4, REG_VREC_CTRL01, AUD_DMIC_CHANNEL_MSK, (nConfigValue<<AUD_DMIC_CHANNEL_POS));
    return TRUE;
}

#if 0
BOOL msb_aud_dmic_enable(BOOL bEnable)
{
    _AudWriteReg(AUD_REG_BANK4, REG_VREC_CTRL01, AUD_DMIC_DECIMATION_EN_MSK, (bEnable<<AUD_DMIC_DECIMATION_EN_POS));

    _AudWriteReg(AUD_REG_BANK4, REG_VREC_CTRL06, AUD_DMIC_RESET_CHANNEL_MSK, 0<<AUD_DMIC_RESET_CHANNEL_POS);

    _AudWriteReg(AUD_REG_BANK4, REG_DMIC_CTRL1, AUD_DMIC_PHASE_ALIGNED_MSK, (0<<AUD_DMIC_PHASE_ALIGNED_POS));
    _AudWriteReg(AUD_REG_BANK4, REG_DMIC_CTRL1, AUD_DMIC_PHASE_EN_MSK, (1<<AUD_DMIC_PHASE_EN_POS));

    _AudWriteReg(AUD_REG_BANK4, REG_DMIC_CTRL2, AUD_DMIC_PHASE_SELECT_AUTO_MSK, (1<<AUD_DMIC_PHASE_SELECT_AUTO_POS));
    _AudWriteReg(AUD_REG_BANK4, REG_DMIC_CTRL2, AUD_DMIC_PHASE_SELECT_CH2_MSK, (0<<AUD_DMIC_PHASE_SELECT_CH2_POS));
    _AudWriteReg(AUD_REG_BANK4, REG_DMIC_CTRL2, AUD_DMIC_PHASE_SELECT_CH1_MSK, (0<<AUD_DMIC_PHASE_SELECT_CH1_POS));

    _AudWriteReg(AUD_REG_BANK4, REG_DMIC_CTRL3, AUD_DMIC_PHASE_ALIGNED_MSK, (0<<AUD_DMIC_PHASE_ALIGNED_POS));
    _AudWriteReg(AUD_REG_BANK4, REG_DMIC_CTRL3, AUD_DMIC_PHASE_EN_MSK, (1<<AUD_DMIC_PHASE_EN_POS));

    _AudWriteReg(AUD_REG_BANK4, REG_DMIC_CTRL4, AUD_DMIC_PHASE_SELECT_AUTO_MSK, (1<<AUD_DMIC_PHASE_SELECT_AUTO_POS));
    _AudWriteReg(AUD_REG_BANK4, REG_DMIC_CTRL4, AUD_DMIC_PHASE_SELECT_CH2_MSK, (0<<AUD_DMIC_PHASE_SELECT_CH2_POS));
    _AudWriteReg(AUD_REG_BANK4, REG_DMIC_CTRL4, AUD_DMIC_PHASE_SELECT_CH1_MSK, (0<<AUD_DMIC_PHASE_SELECT_CH1_POS));

    _AudWriteReg(AUD_REG_BANK4, REG_DMIC_CTRL5, AUD_DMIC_PHASE_ALIGNED_MSK, (0<<AUD_DMIC_PHASE_ALIGNED_POS));
    _AudWriteReg(AUD_REG_BANK4, REG_DMIC_CTRL5, AUD_DMIC_PHASE_EN_MSK, (1<<AUD_DMIC_PHASE_EN_POS));

    _AudWriteReg(AUD_REG_BANK4, REG_DMIC_CTRL6, AUD_DMIC_PHASE_SELECT_AUTO_MSK, (1<<AUD_DMIC_PHASE_SELECT_AUTO_POS));
    _AudWriteReg(AUD_REG_BANK4, REG_DMIC_CTRL6, AUD_DMIC_PHASE_SELECT_CH2_MSK, (0<<AUD_DMIC_PHASE_SELECT_CH2_POS));
    _AudWriteReg(AUD_REG_BANK4, REG_DMIC_CTRL6, AUD_DMIC_PHASE_SELECT_CH1_MSK, (0<<AUD_DMIC_PHASE_SELECT_CH1_POS));

    _AudWriteReg(AUD_REG_BANK4, REG_DMIC_CTRL7, AUD_DMIC_PHASE_ALIGNED_MSK, (0<<AUD_DMIC_PHASE_ALIGNED_POS));
    _AudWriteReg(AUD_REG_BANK4, REG_DMIC_CTRL7, AUD_DMIC_PHASE_EN_MSK, (1<<AUD_DMIC_PHASE_EN_POS));

    _AudWriteReg(AUD_REG_BANK4, REG_DMIC_CTRL8, AUD_DMIC_PHASE_SELECT_AUTO_MSK, (1<<AUD_DMIC_PHASE_SELECT_AUTO_POS));
    _AudWriteReg(AUD_REG_BANK4, REG_DMIC_CTRL8, AUD_DMIC_PHASE_SELECT_CH2_MSK, (0<<AUD_DMIC_PHASE_SELECT_CH2_POS));
    _AudWriteReg(AUD_REG_BANK4, REG_DMIC_CTRL8, AUD_DMIC_PHASE_SELECT_CH1_MSK, (0<<AUD_DMIC_PHASE_SELECT_CH1_POS));

    _AudWriteReg(AUD_REG_BANK4, REG_DMIC_CTRL0, AUD_DMIC_RESET_CHANNEL_MSK, (0<<AUD_DMIC_RESET_CHANNEL_POS));

    _AudWriteReg(AUD_REG_BANK4, REG_VREC_CTRL06, AUD_DMIC_ENABLE_DECIMATION_MSK, AUD_DMIC_ENABLE_DECIMATION_MSK);

    _AudWriteReg(AUD_REG_BANK4, REG_VREC_CTRL06, AUD_DMIC_ENABLE_TIMEGEN_MSK, AUD_DMIC_ENABLE_TIMEGEN_MSK);

    pr_warn("%s\n", __func__);
    return TRUE;
}
#endif

BOOL HalAudDmicSetMode(U16 nMode)
{
    U16 nMax = sizeof(_gaMapTable)/sizeof(_gaMapTable[0]);

    if(nMode <= nMax && nMode > 0)
    {
        _AudWriteReg(AUD_REG_BANK4, REG_VREC_CTRL04, AUD_DMIC_AUTO_SEL_MODE_MSK, (nMode<<AUD_DMIC_AUTO_SEL_MODE_POS));
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL HalAudDmicSetClkRate(AudPdmClkRate_e eClkRate)
{
    if(eClkRate < AUD_PDM_CLK_RATE_NUM)
    {
        _geClkRate = eClkRate;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

U16 HalAudDmicFindMode(AudRate_e eRate)
{
    U16 i;
    U16 nMax = sizeof(_gaMapTable)/sizeof(_gaMapTable[0]);
    for(i=0; i<nMax; i++)
    {
        if(_gaMapTable[i][0]==eRate && _gaMapTable[i][1]==_geClkRate)
            break;
    }

    if(i<nMax)
        return i+1;
    else
        return 0;
}

BOOL HalAudIntEnable(AudInt_e eInt, U16 nEnMsk)
{
    BOOL ret = TRUE;
    switch(eInt)
    {
        case AUD_AU_INT:
            _AudWriteReg(AUD_REG_BANK2, REG_SYS_CTRL_15, AUD_INT_ALL_MSK, nEnMsk);
            break;
        case AUD_AU_INT_GEN:
            _AudWriteReg(AUD_REG_BANK2, REG_SYS_CTRL_16, AUD_INT_ALL_MSK, nEnMsk);
            break;
        default:
            ret = FALSE;
    }

    return ret;
}

