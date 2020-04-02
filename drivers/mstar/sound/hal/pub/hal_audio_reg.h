//
// hal_aud_reg.h
//
// definitions for the Cleveland Haydn, Mstar's Audio Codec Spec
//

#ifndef __HAL_AUD_REG_H__
#define __HAL_AUD_REG_H__


enum
{
    // ----------------------------------------------
    // BANK0 SYS
    // ----------------------------------------------
    REG_AUDIOBAND_CFG_00      = 0x00,
    REG_AUDIOBAND_CFG_01      = 0x02,
    REG_AUDIOBAND_CFG_02      = 0x04,
    REG_AUDIOBAND_CFG_03      = 0x06,
    REG_AUDIOBAND_CFG_04      = 0x08,
    REG_AUDIOBAND_CFG_05      = 0x0A,
    REG_AUDIOBAND_CFG_06      = 0x0C,
    REG_AUDIOBAND_CFG_07      = 0x0E,
    REG_AUDIOBAND_CFG_08      = 0x10,
    REG_AUDIOBAND_CFG_09      = 0x12,
    REG_AUDIOBAND_CFG_0A      = 0x14,
    REG_AUDIOBAND_CFG_0B      = 0x16,
    REG_AUDIOBAND_CFG_0C      = 0x18,
    REG_AUDIOBAND_CFG_0D      = 0x1A,
    REG_AUDIOBAND_CFG_0E      = 0x1C,
    REG_AUDIOBAND_CFG_0F      = 0x1E,
    REG_AUDIOBAND_CFG_10      = 0x20,
    REG_AUDIOBAND_CFG_11      = 0x22,
    REG_AUDIOBAND_CFG_12      = 0x24,
    REG_AUDIOBAND_CFG_13      = 0x26,
    REG_AUDIOBAND_CFG_14      = 0x28,
    REG_VLD_GEN_CFG_0         = 0x80,
    REG_VLD_GEN_CFG_1         = 0x82,
    REG_VLD_GEN_CFG_2         = 0x84,
    REG_VLD_GEN_CFG_3         = 0x86,
    REG_VLD_GEN_CFG_4         = 0x88,
    REG_VLD_GEN_CFG_5         = 0x8A,
    REG_VLD_GEN_CFG_6         = 0x8C,
};

enum
{
    // ----------------------------------------------
    // BANK1 SYS
    // ----------------------------------------------
    REG_AUPLL_CTRL5      = 0x6E,
    REG_AUPLL_CTRL1      = 0x70,
    REG_AUPLL_CTRL2      = 0x72,
    REG_AUPLL_CTRL3      = 0x74,
    REG_AUPLL_CTRL4      = 0x76,
    REG_AUPLL_STATUS     = 0x78,
    REG_AUSDM_CFG16      = 0xD6,
    REG_AUSDM_CFG15      = 0xD8,
	REG_AUSDM_CFG0		 = 0xDA,
    REG_AUSDM_CFG1       = 0xDC,
    REG_AUSDM_CFG2		 = 0xDE,
    REG_AUSDM_CFG3		 = 0xE0,
	REG_AUSDM_CFG4 	     = 0xE2,
	REG_AUSDM_CFG5		 = 0xE4,
	REG_AUSDM_CFG6		 = 0xE6,
	REG_AUSDM_CFG7		 = 0xE8,
	REG_AUSDM_CFG8		 = 0xEA,
	REG_AUSDM_CFG9		 = 0xEC,
	REG_AUSDM_CFG10		 = 0xEE,
    REG_AUSDM_CFG11      = 0xF0,
    REG_AUSDM_CFG12      = 0xF2,
    REG_AUSDM_CFG13      = 0xF4,
    REG_AUSDM_CFG14      = 0xF6,
};

enum
{
    // ----------------------------------------------
    // BANK2 SYS
    // ----------------------------------------------
    REG_SOFTWARE_RESET_00     = 0x00,
    REG_SOFTWARE_RESET_01     = 0x02,
    REG_SOFTWARE_RESET_02     = 0x04,
    REG_CLKGEN_CFG_00         = 0x06,
    REG_CLKGEN_CFG_01         = 0x08,
    REG_CLKGEN_CFG_02         = 0x0A,
    REG_CLKGEN_CFG_03         = 0x0C,
    REG_CLKGEN_CFG_04         = 0x0E,
    REG_CLKGEN_CFG_05         = 0x10,
    REG_CLKGEN_CFG_06         = 0x12,
    REG_CLKGEN_CFG_07         = 0x14,
    REG_CLKGEN_CFG_08         = 0x16,
    REG_CLKGEN_CFG_09         = 0x18,
    REG_CLKGEN_CFG_0A         = 0x1A,
    REG_CLKGEN_CFG_0B         = 0x1C,
    REG_CLKGEN_CFG_0C         = 0x1E,
    REG_2ND_ORDER_SYNTH_0     = 0x20,
    REG_2ND_ORDER_SYNTH_1     = 0x22,
    REG_2ND_ORDER_SYNTH_2     = 0x24,
    REG_2ND_ORDER_SYNTH_3     = 0x26,
    REG_CODEC_RX_NF_SYNTH_L   = 0x28,
    REG_CODEC_RX_NF_SYNTH_H   = 0x2A,
    REG_CODEC_TX_NF_SYNTH_L   = 0x2C,
    REG_CODEC_TX_NF_SYNTH_H   = 0x2E,
    REG_BT_RX_NF_SYNTH_L      = 0x30,
    REG_BT_RX_NF_SYNTH_H      = 0x32,
    REG_BT_TX_NF_SYNTH_L      = 0x34,
    REG_BT_TX_NF_SYNTH_H      = 0x36,
    REG_NF_SYNTH_EN_TRIG      = 0x38,
    REG_FS_SYNTH_BW_FF        = 0x3A,
    REG_USB_SYNTH_CFG_0       = 0x3C,
    REG_USB_SYNTH_CFG_1       = 0x3E,
    REG_USB_SYNTH_CFG_2       = 0x40,
    REG_USB_SYNTH_CFG_3       = 0x42,
    REG_I2S_TDM_CFG_00        = 0x44,
    REG_I2S_TDM_CFG_01        = 0x46,
    REG_I2S_TDM_CFG_02        = 0x48,
    REG_I2S_TDM_CFG_03        = 0x4A,
    REG_I2S_TDM_CFG_04        = 0x4C,
    REG_I2S_TDM_CFG_05        = 0x4E,
    REG_I2S_TDM_CFG_06        = 0x50,
    REG_I2S_TDM_CFG_07        = 0x52,
    REG_I2S_TDM_CFG_08        = 0x54,
    REG_I2S_TDM_CFG_09        = 0x56,
    REG_I2S_TDM_CFG_0A        = 0x58,
    REG_I2S_TDM_CFG_0B        = 0x5A,
    REG_I2S_TDM_CFG_0C        = 0x5C,
    REG_I2S_TDM_CFG_0D        = 0x5E,
    REG_W1_DMA_MCH_CFG1       = 0x60,
    REG_W1_DMA_MCH_CFG2       = 0x62,
    REG_W2_DMA_MCH_CFG1       = 0x64,
    REG_W2_DMA_MCH_CFG2       = 0x66,
    REG_CLKGEN_CFG_0D         = 0x68,
    REG_CLKGEN_CFG_0E         = 0x6A,
    REG_CLKGEN_CFG_0F         = 0x6C,
    REG_CLKGEN_CFG_10         = 0x6E,
    REG_CLKGEN_CFG_11         = 0x70,
    REG_CLKGEN_CFG_12         = 0x72,
    REG_USB_NF_SYNTH_L        = 0x74,
    REG_USB_NF_SYNTH_H        = 0x76,
    REG_RESERVE_0             = 0x78,
    REG_RESERVE_1             = 0x7A,
    REG_RESERVE_2             = 0x7C,
    REG_RESERVE_3             = 0x7E,
    REG_SYS_CTRL_00           = 0x80,
    REG_SYS_CTRL_01           = 0x82,
    REG_SYS_CTRL_02           = 0x84,
    REG_SYS_CTRL_03           = 0x86,
    REG_SYS_CTRL_04           = 0x88,
    REG_SYS_CTRL_05           = 0x8A,
    REG_SYS_CTRL_06           = 0x8C,
    REG_SYS_CTRL_07           = 0x8E,
    REG_SYS_CTRL_08           = 0x90,
    REG_SYS_CTRL_09           = 0x92,
    REG_SYS_CTRL_0A           = 0x94,
    REG_SYS_CTRL_0B           = 0x96,
    REG_SYS_CTRL_0C           = 0x98,
    REG_SYS_CTRL_0D           = 0x9A,
    REG_SYS_CTRL_0E           = 0x9C,
    REG_SYS_CTRL_0F           = 0x9E,
    REG_SYS_STS_00            = 0xA0,
    REG_SYS_STS_01            = 0xA2,
    REG_SYS_STS_02            = 0xA4,
    REG_SYS_STS_03            = 0xA6,
    REG_SYS_STS_04            = 0xA8,
    REG_SYS_STS_05            = 0xAA,
    REG_SYS_STS_06            = 0xAC,
    REG_SYS_STS_07            = 0xAE,
    REG_SYS_STS_08            = 0xB0,
    REG_SYS_STS_09            = 0xB2,
    REG_SYS_STS_0A            = 0xB4,
    REG_SYS_STS_0B            = 0xB6,
    REG_SYS_STS_0C            = 0xB8,
    REG_SYS_STS_0D            = 0xBA,
    REG_SYS_STS_0E            = 0xBC,
    REG_SYS_STS_0F            = 0xBE,
    REG_SYS_STS_10            = 0xC0,
    REG_SYS_STS_11            = 0xC2,
    REG_SYS_STS_12            = 0xC4,
    REG_SYS_STS_13            = 0xC6,
    REG_SYS_STS_14            = 0xC8,
    REG_SYS_STS_15            = 0xCA,
    REG_SYS_STS_16            = 0xCC,
    REG_SYS_STS_17            = 0xCE,
    REG_SYS_STS_18            = 0xD0,
    REG_SYS_STS_19            = 0xD2,
    REG_SYS_STS_1A            = 0xD4,
    REG_SYS_CTRL_10           = 0xD6,
    REG_SYS_CTRL_11           = 0xD8,
    REG_SYS_CTRL_12           = 0xDA,
    REG_SYS_CTRL_13           = 0xDC,
    REG_SYS_CTRL_14           = 0xDE,
    REG_SYS_CTRL_15           = 0xE0,
    REG_SYS_CTRL_16           = 0xE2,
    REG_SYS_CTRL_17           = 0xE4,
    REG_SYS_CTRL_18           = 0xE6,
    REG_SYS_CTRL_19           = 0xE8,
    REG_SYS_CTRL_1A           = 0xEA,
    REG_SYS_CTRL_1B           = 0xEC,
    REG_SYS_CTRL_1C           = 0xEE,
    REG_SYS_CTRL_1D           = 0xF0,
    REG_SYS_CTRL_1E           = 0xF2,
    REG_SYS_CTRL_1F           = 0xF4,
    REG_SYS_CTRL_20           = 0xF6,
    REG_SYS_CTRL_21           = 0xF8,
    REG_SYS_CTRL_22           = 0xFA,
    REG_SYS_CTRL_23           = 0xFC,
    REG_SYS_CTRL_24           = 0xFE,
};


enum
{
    // ----------------------------------------------
    // BANK3 DMA
    // ----------------------------------------------
    REG_AUDDMA_R1_CFG00       =  0x00,
    REG_AUDDMA_R1_CFG01       =  0x02,
    REG_AUDDMA_R1_CFG02       =  0x04,
    REG_AUDDMA_R1_CFG03       =  0x06,
    REG_AUDDMA_R1_CFG04       =  0x08,
    REG_AUDDMA_R1_CFG05       =  0x0A,
    REG_AUDDMA_R1_CFG06       =  0x0C,
    REG_AUDDMA_R1_CFG07       =  0x0E,
    REG_AUDDMA_R1_CFG08       =  0x10,
    REG_AUDDMA_R1_CFG09       =  0x12,
    REG_AUDDMA_R1_STS00       =  0x14,
    REG_AUDDMA_R1_STS01       =  0x16,
    REG_AUDDMA_R1_STS02       =  0x18,
    REG_AUDDMA_R1_TEST00      =  0x1A,
    REG_AUDDMA_R1_TEST01      =  0x1C,
    REG_AUDDMA_R1_TEST02      =  0x1E,
    REG_AUDDMA_W1_CFG00       =  0x20,
    REG_AUDDMA_W1_CFG01       =  0x22,
    REG_AUDDMA_W1_CFG02       =  0x24,
    REG_AUDDMA_W1_CFG03       =  0x26,
    REG_AUDDMA_W1_CFG04       =  0x28,
    REG_AUDDMA_W1_CFG05       =  0x2A,
    REG_AUDDMA_W1_CFG06       =  0x2C,
    REG_AUDDMA_W1_CFG07       =  0x2E,
    REG_AUDDMA_W1_CFG08       =  0x30,
    REG_AUDDMA_W1_CFG09       =  0x32,
    REG_AUDDMA_W1_STS00       =  0x34,
    REG_AUDDMA_W1_STS01       =  0x36,
    REG_AUDDMA_W1_STS02       =  0x38,
    REG_AUDDMA_W1_TEST00      =  0x3A,
    REG_AUDDMA_W1_TEST01      =  0x3C,
    REG_AUDDMA_W1_TEST02      =  0x3E,
    REG_AUDDMA_R2_CFG00       =  0x40,
    REG_AUDDMA_R2_CFG01       =  0x42,
    REG_AUDDMA_R2_CFG02       =  0x44,
    REG_AUDDMA_R2_CFG03       =  0x46,
    REG_AUDDMA_R2_CFG04       =  0x48,
    REG_AUDDMA_R2_CFG05       =  0x4A,
    REG_AUDDMA_R2_CFG06       =  0x4C,
    REG_AUDDMA_R2_CFG07       =  0x4E,
    REG_AUDDMA_R2_CFG08       =  0x50,
    REG_AUDDMA_R2_CFG09       =  0x52,
    REG_AUDDMA_R2_STS00       =  0x54,
    REG_AUDDMA_R2_STS01       =  0x56,
    REG_AUDDMA_R2_STS02       =  0x58,
    REG_AUDDMA_R2_TEST00      =  0x5A,
    REG_AUDDMA_R2_TEST01      =  0x5C,
    REG_AUDDMA_R2_TEST02      =  0x5E,
    REG_AUDDMA_W2_CFG00       =  0x60,
    REG_AUDDMA_W2_CFG01       =  0x62,
    REG_AUDDMA_W2_CFG02       =  0x64,
    REG_AUDDMA_W2_CFG03       =  0x66,
    REG_AUDDMA_W2_CFG04       =  0x68,
    REG_AUDDMA_W2_CFG05       =  0x6A,
    REG_AUDDMA_W2_CFG06       =  0x6C,
    REG_AUDDMA_W2_CFG07       =  0x6E,
    REG_AUDDMA_W2_CFG08       =  0x70,
    REG_AUDDMA_W2_CFG09       =  0x72,
    REG_AUDDMA_W2_STS00       =  0x74,
    REG_AUDDMA_W2_STS01       =  0x76,
    REG_AUDDMA_W2_STS02       =  0x78,
    REG_AUDDMA_W2_TEST00      =  0x7A,
    REG_AUDDMA_W2_TEST01      =  0x7C,
    REG_AUDDMA_W2_TEST02      =  0x7E,
    REG_AUDDMA_R3_CFG00       =  0x80,
    REG_AUDDMA_R3_CFG01       =  0x82,
    REG_AUDDMA_R3_CFG02       =  0x84,
    REG_AUDDMA_R3_CFG03       =  0x86,
    REG_AUDDMA_R3_CFG04       =  0x88,
    REG_AUDDMA_R3_CFG05       =  0x8A,
    REG_AUDDMA_R3_CFG06       =  0x8C,
    REG_AUDDMA_R3_CFG07       =  0x8E,
    REG_AUDDMA_R3_CFG08       =  0x90,
    REG_AUDDMA_R3_CFG09       =  0x92,
    REG_AUDDMA_R3_STS00       =  0x94,
    REG_AUDDMA_R3_STS01       =  0x96,
    REG_AUDDMA_R3_STS02       =  0x98,
    REG_AUDDMA_R3_TEST00      =  0x9A,
    REG_AUDDMA_R3_TEST01      =  0x9C,
    REG_AUDDMA_R3_TEST02      =  0x9E,
    REG_AUDDMA_W3_CFG00       =  0xA0,
    REG_AUDDMA_W3_CFG01       =  0xA2,
    REG_AUDDMA_W3_CFG02       =  0xA4,
    REG_AUDDMA_W3_CFG03       =  0xA6,
    REG_AUDDMA_W3_CFG04       =  0xA8,
    REG_AUDDMA_W3_CFG05       =  0xAA,
    REG_AUDDMA_W3_CFG06       =  0xAC,
    REG_AUDDMA_W3_CFG07       =  0xAE,
    REG_AUDDMA_W3_CFG08       =  0xB0,
    REG_AUDDMA_W3_CFG09       =  0xB2,
    REG_AUDDMA_W3_STS00       =  0xB4,
    REG_AUDDMA_W3_STS01       =  0xB6,
    REG_AUDDMA_W3_STS02       =  0xB8,
    REG_AUDDMA_W3_TEST00      =  0xBA,
    REG_AUDDMA_W3_TEST01      =  0xBC,
    REG_AUDDMA_W3_TEST02      =  0xBE,
    REG_AUDDMA_R4_CFG00       =  0xC0,
    REG_AUDDMA_R4_CFG01       =  0xC2,
    REG_AUDDMA_R4_CFG02       =  0xC4,
    REG_AUDDMA_R4_CFG03       =  0xC6,
    REG_AUDDMA_R4_CFG04       =  0xC8,
    REG_AUDDMA_R4_CFG05       =  0xCA,
    REG_AUDDMA_R4_CFG06       =  0xCC,
    REG_AUDDMA_R4_CFG07       =  0xCE,
    REG_AUDDMA_R4_CFG08       =  0xD0,
    REG_AUDDMA_R4_CFG09       =  0xD2,
    REG_AUDDMA_R4_STS00       =  0xD4,
    REG_AUDDMA_R4_STS01       =  0xD6,
    REG_AUDDMA_R4_STS02       =  0xD8,
    REG_AUDDMA_R4_TEST00      =  0xDA,
    REG_AUDDMA_R4_TEST01      =  0xDC,
    REG_AUDDMA_R4_TEST02      =  0xDE,
    REG_AUDDMA_W4_CFG00       =  0xE0,
    REG_AUDDMA_W4_CFG01       =  0xE2,
    REG_AUDDMA_W4_CFG02       =  0xE4,
    REG_AUDDMA_W4_CFG03       =  0xE6,
    REG_AUDDMA_W4_CFG04       =  0xE8,
    REG_AUDDMA_W4_CFG05       =  0xEA,
    REG_AUDDMA_W4_CFG06       =  0xEC,
    REG_AUDDMA_W4_CFG07       =  0xEE,
    REG_AUDDMA_W4_CFG08       =  0xF0,
    REG_AUDDMA_W4_CFG09       =  0xF2,
    REG_AUDDMA_W4_STS00       =  0xF4,
    REG_AUDDMA_W4_STS01       =  0xF6,
    REG_AUDDMA_W4_STS02       =  0xF8,
    REG_AUDDMA_W4_TEST00      =  0xFA,
    REG_AUDDMA_W4_TEST01      =  0xFC,
    REG_AUDDMA_W4_TEST02      =  0xFE,
};

enum
{
    // ----------------------------------------------
    // BANK4
    // ----------------------------------------------
    REG_VREC_CTRL01           =  0x02,
    REG_VREC_CTRL02           =  0x04,
    REG_VREC_CTRL04           =  0x08,
    REG_VREC_CTRL05           =  0x0A,
    REG_VREC_CTRL06 		  =  0x0C,
    REG_DMIC_CTRL0            =  0x20,
    REG_DMIC_CTRL1            =  0x22,
	REG_DMIC_CTRL2			  =  0x24,
	REG_DMIC_CTRL3			  =  0x26,
	REG_DMIC_CTRL4			  =  0x28,
	REG_DMIC_CTRL5			  =  0x2a,
	REG_DMIC_CTRL6			  =  0x2c,
	REG_DMIC_CTRL7			  =  0x2e,
	REG_DMIC_CTRL8			  =  0x30,

	REG_CIC_CTRL0 			  =  0x40,
	REG_CIC_CTRL1			  =  0x42,
	REG_CIC_CTRL2			  =  0x44,
	REG_CIC_CTRL3			  =  0x46,
	REG_CIC_CTRL4			  =  0x48,
    REG_AUDDMA_W5_CFG00     =  0x60,
    REG_AUDDMA_W5_CFG01     =  0x62,
    REG_AUDDMA_W5_CFG02     =  0x64,
    REG_AUDDMA_W5_CFG03     =  0x66,
    REG_AUDDMA_W5_CFG04     =  0x68,
    REG_AUDDMA_W5_CFG05     =  0x6A,
    REG_AUDDMA_W5_CFG06     =  0x6C,
    REG_AUDDMA_W5_CFG07     =  0x6E,
    REG_AUDDMA_W5_CFG08     =  0x70,
    REG_AUDDMA_W5_CFG09     =  0x72,
    REG_AUDDMA_W5_STS00     =  0x74,
    REG_AUDDMA_W5_STS01     =  0x76,
    REG_AUDDMA_W5_STS02     =  0x78,
    REG_AUDDMA_W5_TEST00    =  0x7A,
    REG_AUDDMA_W5_TEST01    =  0x7C,
    REG_AUDDMA_W5_TEST02    =  0x7E,
    REG_DMA_MCH_IF_CFG1       =  0x80,
    REG_DMA_MCH_IF_STS        =  0x82,
    REG_DMA_MCH_IF_CFG2       =  0x84,
};


enum
{
    // ----------------------------------------------
    // BANK5
    // ----------------------------------------------
    REG_AUDDMA_R5_CFG00     =  0x00,
    REG_AUDDMA_R5_CFG01     =  0x02,
    REG_AUDDMA_R5_CFG02     =  0x04,
    REG_AUDDMA_R5_CFG03     =  0x06,
    REG_AUDDMA_R5_CFG04     =  0x08,
    REG_AUDDMA_R5_CFG05     =  0x0A,
    REG_AUDDMA_R5_CFG06     =  0x0C,
    REG_AUDDMA_R5_CFG07     =  0x0E,
    REG_AUDDMA_R5_CFG08     =  0x10,
    REG_AUDDMA_R5_CFG09     =  0x12,
    REG_AUDDMA_R5_STS00     =  0x14,
    REG_AUDDMA_R5_STS01     =  0x16,
    REG_AUDDMA_R5_STS02     =  0x18,
    REG_AUDDMA_R5_TEST00    =  0x1A,
    REG_AUDDMA_R5_TEST01    =  0x1C,
    REG_AUDDMA_R5_TEST02    =  0x1E,
    REG_SYS2_CTRL_02        =  0xA4,
};


/****************************************************************************/
/*        AUDIO DIGITAL registers bank0                                     */
/****************************************************************************/
/**
 * @brief Register 00h
 */
#define AUD_SRC_B_IN_SEL_POS                           4
#define AUD_SRC_B_IN_SEL_MSK                           (0x7<<AUD_SRC_B_IN_SEL_POS)
#define AUD_SRC_C_IN_SEL_POS                           0
#define AUD_SRC_C_IN_SEL_MSK                           (0x7<<AUD_SRC_C_IN_SEL_POS)


/**
 * @brief Register 02h
 */
#define AUD_DMA_I2S_B_BYPASS                           (1<<5)
#define AUD_DMA_I2S_C_BYPASS                           (1<<4)
#define AUD_SRC_A1_BYPASS                              (1<<3)
#define AUD_SRC_A2_BYPASS                              (1<<2)


/**
 * @brief Register 04h
 */
#define AUD_CODEC_ADC_DEC_POS                          8
#define AUD_CODEC_ADC_DEC_MSK                          (0xF<<AUD_CODEC_ADC_DEC_POS)


/**
 * @brief Register 10h
 */
#define AUD_CODEC_DAC_AUTO_SYNC                        (1<<14)
#define AUD_CODEC_DAC_SDM_EN                           (1<<8)


/**
 * @brief Register 20h,22h,24h,26h
 */
#define AUD_AUTO_SYNC                                  (1<<1)
#define AUD_CIC_EN                                     (1<<0)


/**
 * @brief Register 80h
 */
#define AUD_VLD_GEN_DAC_EN                             (1<<15)
#define AUD_VLD_GEN_ADC_EN                             (1<<7)
#define AUD_VLD_GEN_ADC_FS_SEL_POS                     0
#define AUD_VLD_GEN_ADC_FS_SEL_MSK                     (0x7<<AUD_VLD_GEN_ADC_FS_SEL_POS)


/**
 * @brief Register 82h,84h,86h,88h
 */
#define AUD_VLD_GEN_SRCI                               (1<<15)
#define AUD_VLD_GEN_SRCO                               (1<<7)
#define AUD_VLD_GEN_SRCO_CIC_SEL                       (1<<0)


/**
 * @brief Register 8Ch
 */
#define AUD_VLD_GEN_DMA_I2S_B_EN                       (1<<15)
#define AUD_VLD_SEL_DMA_R2_SEL                         (1<<8)
#define AUD_VLD_GEN_DMA_I2S_C_EN                       (1<<7)
#define AUD_VLD_SEL_DMA_R3_SEL                         (1<<0)


/****************************************************************************/
/*        AUDIO  registers bank1                                     */
/****************************************************************************/
/**
 * @brief Register 6Eh
 */
#define AUD_EN_AUPLL_LPFSW_POS                         4
#define AUD_EN_AUPLL_LPFSW_MSK                         (0x3<<AUD_EN_AUPLL_LPFSW_POS)


/**
 * @brief Register 76h
 */
#define AUD_AUPLL_LOOPDIV_SECOND_POS                   8
#define AUD_AUPLL_LOOPDIV_SECOND_MSK                   (0xFF<<AUD_AUPLL_LOOPDIV_SECOND_POS)

/**
 * @brief Register E2h
 */
#define AUD_SEL_CH_INMUX0_L_POS                        4
#define AUD_SEL_CH_INMUX0_L_MSK                        (0xF<<AUD_SEL_CH_INMUX0_L_POS)
#define AUD_SEL_CH_INMUX0_R_POS                        0
#define AUD_SEL_CH_INMUX0_R_MSK                        (0xF<<AUD_SEL_CH_INMUX0_R_POS)


/**
 * @brief Register E4h
 */
#define AUD_SEL_GAIN_INMUX0_L_POS                      13
#define AUD_SEL_GAIN_INMUX0_L_MSK                      (0x7<<AUD_SEL_GAIN_INMUX0_L_POS)
#define AUD_SEL_GAIN_INMUX0_R_POS                      10
#define AUD_SEL_GAIN_INMUX0_R_MSK                      (0x7<<AUD_SEL_GAIN_INMUX0_R_POS)
#define AUD_SEL_MICGAIN_INMUX_R                        (1<<9)
#define AUD_SEL_MICGAIN_INMUX_L                        (1<<8)

/**
 * @brief Register F6h
 */
#define SEL_GAIN_R_MICAMP0_POS                         10
#define SEL_GAIN_R_MICAMP0_MSK                         (0x7<<SEL_GAIN_R_MICAMP0_POS)
#define SEL_GAIN_L_MICAMP0_POS                         4
#define SEL_GAIN_L_MICAMP0_MSK                         (0x7<<SEL_GAIN_L_MICAMP0_POS)

/****************************************************************************/
/*        AUDIO DIGITAL registers bank2                                     */
/****************************************************************************/
/**
 * @brief Register 06h
 */
#define AUD_CLK_DMA_W4_EN                              (1<<15)
#define AUD_CLK_DMA_R4_EN                              (1<<14)
#define AUD_CLK_SRC_A2_256FSO_EN                       (1<<13)
#define AUD_CLK_SRC_A2_256FSI_EN                       (1<<12)
#define AUD_CLK_SRC_A1_256FSO_EN                       (1<<11)
#define AUD_CLK_SRC_A1_256FSI_EN                       (1<<10)
#define AUD_CLK_AUPLL_REF_EN                           (1<<9)
#define AUD_CLK_2ND_ORDER_SYNTH_REF_EN                 (1<<8)
#define AUD_CLK_USB_SYNTH_REF_EN                       (1<<7)
#define AUD_CLK_MAC_FREE_EN                            (1<<6)
#define AUD_CLK_NF_SYNTH_REF_EN                        (1<<5)
#define AUD_CLK_288MHZ_SH_EN                           (1<<4)
#define AUD_CLK_DAC_512FS_EN                           (1<<3)
#define AUD_CLK_DAC_256FS_EN                           (1<<2)
#define AUD_CLK_ADC_512FS_EN                           (1<<1)
#define AUD_CLK_ADC_256FS_EN                           (1<<0)


/**
 * @brief Register 08h
 */
#define AUD_CLK_24MHZ_EN                               (1<<15)
#define AUD_CLK_DE_GLITCH_REF_EN                       (1<<14)
#define AUD_CLK_MISC_I2S_TX_BCK_EN                     (1<<13)
#define AUD_CLK_MISC_I2S_RX_BCK_EN                     (1<<12)
#define AUD_CLK_BT_I2S_TX_BCK_EN                       (1<<11)
#define AUD_CLK_BT_I2S_RX_BCK_EN                       (1<<10)
#define AUD_CLK_CODEC_I2S_TX_BCK_EN                    (1<<9)
#define AUD_CLK_CODEC_I2S_RX_BCK_EN                    (1<<8)
#define AUD_CLK_SRC_C_256FSO_EN                        (1<<7)
#define AUD_CLK_I2S_C_256FS_EN                         (1<<6)
#define AUD_CLK_SRC_C_256FSI_EN                        (1<<5)
#define AUD_CLK_DMA_R3_EN                              (1<<4)
#define AUD_CLK_SRC_B_256FSO_EN                        (1<<3)
#define AUD_CLK_I2S_B_256FS_EN                         (1<<2)
#define AUD_CLK_SRC_B_256FSI_EN                        (1<<1)
#define AUD_CLK_DMA_R2_EN                              (1<<0)


/**
 * @brief Register 0Ah
 */
#define AUD_CLK_MCK_SYNTH_REF_EN                       (1<<7)
#define AUD_CLK_FS_SYNTH_REF_EN                        (1<<6)
#define AUD_CLK_MISC_I2S_MCK_EN                        (1<<5)
#define AUD_CLK_CODEC_I2S_MCK_EN                       (1<<4)
#define AUD_CLK_CLK_INT_3MHZ_EN                        (1<<3)
#define AUD_CLK_MIU_EN                                 (1<<2)
#define AUD_CLK_SPDIF_TX_128FS_EN                      (1<<1)
#define AUD_CLK_SPDIF_TX_256FS_EN                      (1<<0)


/**
 * @brief Register 0Ch
 */
#define AUD_SEL_MISC_I2S_TX_BCK_FS_POS                 10
#define AUD_SEL_MISC_I2S_TX_BCK_FS_MSK                 (0x3<<AUD_SEL_MISC_I2S_TX_BCK_FS_POS)
#define AUD_SEL_MISC_I2S_RX_BCK_FS_POS                 8
#define AUD_SEL_MISC_I2S_RX_BCK_FS_MSK                 (0x3<<AUD_SEL_MISC_I2S_RX_BCK_FS_POS)
#define AUD_SEL_BT_I2S_TX_BCK_FS_POS                   6
#define AUD_SEL_BT_I2S_TX_BCK_FS_MSK                   (0x3<<AUD_SEL_BT_I2S_TX_BCK_FS_POS)
#define AUD_SEL_BT_I2S_RX_BCK_FS_POS                   4
#define AUD_SEL_BT_I2S_RX_BCK_FS_MSK                   (0x3<<AUD_SEL_BT_I2S_RX_BCK_FS_POS)
#define AUD_SEL_CODEC_I2S_TX_BCK_FS_POS                2
#define AUD_SEL_CODEC_I2S_TX_BCK_FS_MSK                (0x3<<AUD_SEL_CODEC_I2S_TX_BCK_FS_POS)
#define AUD_SEL_CODEC_I2S_RX_BCK_FS_POS                0
#define AUD_SEL_CODEC_I2S_RX_BCK_FS_MSK                (0x3<<AUD_SEL_CODEC_I2S_RX_BCK_FS_POS)


/**
 * @brief Register 0Eh
 */
#define AUD_SEL_CLK_DAC_512FS_POS                      12
#define AUD_SEL_CLK_DAC_512FS_MSK                      (0x3<<AUD_SEL_CLK_DAC_512FS_POS)
#define AUD_SEL_CLK_DAC_256FS_POS                      8
#define AUD_SEL_CLK_DAC_256FS_MSK                      (0x7<<AUD_SEL_CLK_DAC_256FS_POS)
#define AUD_SEL_CLK_ADC_512FS_POS                      4
#define AUD_SEL_CLK_ADC_512FS_MSK                      (0x3<<AUD_SEL_CLK_ADC_512FS_POS)
#define AUD_SEL_CLK_ADC_256FS_POS                      0
#define AUD_SEL_CLK_ADC_256FS_MSK                      (0x7<<AUD_SEL_CLK_ADC_256FS_POS)


/**
 * @brief Register 10h
 */
#define AUD_SEL_CLK_FS_SYNTH_REF_POS                   12
#define AUD_SEL_CLK_FS_SYNTH_REF_MSK                   (0x3<<AUD_SEL_CLK_FS_SYNTH_REF_POS)
#define AUD_SEL_CLK_USB_SYNTH_REF_POS                  10
#define AUD_SEL_CLK_USB_SYNTH_REF_MSK                  (0x3<<AUD_SEL_CLK_USB_SYNTH_REF_POS)
#define AUD_SEL_CLK_NF_SYNTH_REF_POS                   8
#define AUD_SEL_CLK_NF_SYNTH_REF_MSK                   (0x3<<AUD_SEL_CLK_NF_SYNTH_REF_POS)
#define AUD_SEL_CLK_AUPLL_REF_POS                      2
#define AUD_SEL_CLK_AUPLL_REF_MSK                      (0x3<<AUD_SEL_CLK_AUPLL_REF_POS)


/**
 * @brief Register 12h
 */
#define AUD_SEL_CLK_SRC_A1_256FSI_POS                  12
#define AUD_SEL_CLK_SRC_A1_256FSI_MSK                  (0x7<<AUD_SEL_CLK_SRC_A1_256FSI_POS)
#define AUD_SEL_CLK_SRC_A1_256FSO_USB                  (1<<8)
#define AUD_SEL_CLK_SRC_A1_256FSO_POS                  4
#define AUD_SEL_CLK_SRC_A1_256FSO_MSK                  (0x3<<AUD_SEL_CLK_SRC_A1_256FSO_POS)
#define AUD_SEL_A1_FS_RATE_POS                         0
#define AUD_SEL_A1_FS_RATE_MSK                         (0x7<<AUD_SEL_A1_FS_RATE_POS)


/**
 * @brief Register 14h
 */
#define AUD_SEL_CLK_SRC_A2_256FSI_POS                  12
#define AUD_SEL_CLK_SRC_A2_256FSI_MSK                  (0x7<<AUD_SEL_CLK_SRC_A2_256FSI_POS)
#define AUD_SEL_CLK_SRC_A2_256FSO_USB                  (1<<8)
#define AUD_SEL_CLK_SRC_A2_256FSO_POS                  4
#define AUD_SEL_CLK_SRC_A2_256FSO_MSK                  (0x3<<AUD_SEL_CLK_SRC_A2_256FSO_POS)
#define AUD_SEL_A2_FS_RATE_POS                         0
#define AUD_SEL_A2_FS_RATE_MSK                         (0x7<<AUD_SEL_A2_FS_RATE_POS)


/**
 * @brief Register 16h
 */
#define AUD_SEL_DMA_R1_POS                             12
#define AUD_SEL_DMA_R1_MSK                             (0x7<<AUD_SEL_DMA_R1_POS)
#define AUD_SEL_R2_GPA_FS_RATE_POS                     0
#define AUD_SEL_R2_GPA_FS_RATE_MSK                     (0x7<<AUD_SEL_R2_GPA_FS_RATE_POS)


/**
 * @brief Register 18h
 */
#define AUD_SEL_CLK_I2S_B_POS                          12
#define AUD_SEL_CLK_I2S_B_MSK                          (0x7<<AUD_SEL_CLK_I2S_B_POS)
#define AUD_SEL_CLK_SRC_B_256FSO_POS                   8
#define AUD_SEL_CLK_SRC_B_256FSO_MSK                   (0x3<<AUD_SEL_CLK_SRC_B_256FSO_POS)
#define AUD_SEL_CLK_SRC_B_256FSI                       (1<<4)
#define AUD_SEL_DMA_R2_POS                             0
#define AUD_SEL_DMA_R2_MSK                             (0x7<<AUD_SEL_DMA_R2_POS)


/**
 * @brief Register 1Ah
 */
#define AUD_SEL_R3_GPA_FS_RATE_POS                     0
#define AUD_SEL_R3_GPA_FS_RATE_MSK                     (0x7<<AUD_SEL_R3_GPA_FS_RATE_POS)


/**
 * @brief Register 1Ch
 */
#define AUD_SEL_CLK_I2S_C_POS                          12
#define AUD_SEL_CLK_I2S_C_MSK                          (0x7<<AUD_SEL_CLK_I2S_C_POS)
#define AUD_SEL_CLK_SRC_C_256FSO_POS                   8
#define AUD_SEL_CLK_SRC_C_256FSO_MSK                   (0x3<<AUD_SEL_CLK_SRC_C_256FSO_POS)
#define AUD_SEL_CLK_SRC_C_256FSI                       (1<<4)
#define AUD_SEL_DMA_R3_POS                             0
#define AUD_SEL_DMA_R3_MSK                             (0x7<<AUD_SEL_DMA_R3_POS)


/**
 * @brief Register 1Eh
 */
#define AUD_SEL_CLK_DMA_W4_POS                         12
#define AUD_SEL_CLK_DMA_W4_MSK                         (0x7<<AUD_SEL_CLK_DMA_W4_POS)
#define AUD_SEL_DMA_W4_GPA_RATE_POS                    8
#define AUD_SEL_DMA_W4_GPA_RATE_MSK                    (0x7<<AUD_SEL_DMA_W4_GPA_RATE_POS)
#define AUD_SEL_CLK_DMA_R4_POS                         4
#define AUD_SEL_CLK_DMA_R4_MSK                         (0x3<<AUD_SEL_CLK_DMA_R4_POS)
#define AUD_SEL_DMA_R4_GPA_RATE_POS                    0
#define AUD_SEL_DMA_R4_GPA_RATE_MSK                    (0x7<<AUD_SEL_DMA_R3_POS)


/**
 * @brief Register 20h
 */
#define AUD_2ND_ORDER_SYNTH_NF_LOAD                    (1<<14)
#define AUD_SEL_CLK_2ND_ORDER_SYNTH_REF_POS            8
#define AUD_SEL_CLK_2ND_ORDER_SYNTH_REF_MSK            (0x3<<AUD_SEL_CLK_2ND_ORDER_SYNTH_REF_POS)
#define AUD_2ND_ORDER_SYNTH_NF_VALUE_LO_POS            0
#define AUD_2ND_ORDER_SYNTH_NF_VALUE_LO_MSK            (0xFF<<AUD_2ND_ORDER_SYNTH_NF_VALUE_LO_POS)


/**
 * @brief Register 22h
 */
#define AUD_2ND_ORDER_SYNTH_NF_VALUE_HI_OFFSET          8
#define AUD_2ND_ORDER_SYNTH_NF_VALUE_HI_POS             0
#define AUD_2ND_ORDER_SYNTH_NF_VALUE_HI_MSK             (0xFFFF<<AUD_2ND_ORDER_SYNTH_NF_VALUE_HI_POS)

/**
 * @brief Register 28h,2Ch,30h,34h
 */
#define AUD_NF_VALUE_LO_POS                             0
#define AUD_NF_VALUE_LO_MSK                             (0xFFFF<<AUD_NF_VALUE_LO_POS)


/**
 * @brief Register 2Ah,2Eh,32h,36h
 */
#define AUD_NF_VALUE_HI_OFFSET                          16
#define AUD_NF_VALUE_HI_POS                             0
#define AUD_NF_VALUE_HI_MSK                             (0x7FFF<<AUD_NF_VALUE_HI_POS)


/**
 * @brief Register 38h
 */
#define AUD_CODEC_MCK_EN_TIME_GEN                      (1<<13)
#define AUD_CODEC_MCK_NF_SYNTH_TRIG                    (1<<12)
#define AUD_MISC_MCK_EN_TIME_GEN                       (1<<11)
#define AUD_MISC_MCK_NF_SYNTH_TRIG                     (1<<10)
#define AUD_USB_EN_TIME_GEN                            (1<<9)
#define AUD_USB_NF_SYNTH_TRIG                          (1<<8)
#define AUD_CODEC_RX_EN_TIME_GEN                       (1<<7)
#define AUD_CODEC_RX_NF_SYNTH_TRIG                     (1<<6)
#define AUD_CODEC_TX_EN_TIME_GEN                       (1<<5)
#define AUD_CODEC_TX_NF_SYNTH_TRIG                     (1<<4)
#define AUD_BT_RX_EN_TIME_GEN                          (1<<3)
#define AUD_BT_RX_NF_SYNTH_TRIG                        (1<<2)
#define AUD_BT_TX_EN_TIME_GEN                          (1<<1)
#define AUD_BT_TX_NF_SYNTH_TRIG                        (1<<0)


/**
 * @brief Register 44h,48h,4Ch
 */
#define AUD_I2S_RX_BCK_GEN_EN                          (1<<15)
#define AUD_I2S_RX_MS_MODE                             (1<<14)
#define AUD_I2S_RX_TDM_MODE                            (1<<13)
#define AUD_I2S_RX_BIT_WIDTH                           (1<<9)
#define AUD_I2S_RX_TDM_CHN_WIDTH                       (1<<8)
#define AUD_I2S_RX_WCK_FMT                             (1<<7)
#define AUD_I2S_RX_ENC_FMT                             (1<<6)
#define AUD_I2S_RX_WCK_INV                             (1<<5)
#define AUD_I2S_RX_BCK_INV                             (1<<4)
#define AUD_I2S_RX_BCK_DEGLITCH                        (1<<3)
#define AUD_I2S_RX_BCK_DEGLITCH_NUMBER_POS             0
#define AUD_I2S_RX_BCK_DEGLITCH_NUMBER_MSK             (0x7<<AUD_I2S_RX_BCK_DEGLITCH_NUMBER_POS)


/**
 * @brief Register 46h,4Ah,4Eh
 */
#define AUD_I2S_TX_BCK_GEN_EN                          (1<<15)
#define AUD_I2S_TX_MS_MODE                             (1<<14)
#define AUD_I2S_TX_TDM_MODE                            (1<<13)
#define AUD_I2S_TX_4WIRE_MODE                          (1<<12)
#define AUD_I2S_TX_FIFO_STATUS                         (1<<11)
#define AUD_I2S_TX_BIT_WIDTH                           (1<<9)
#define AUD_I2S_TX_TDM_CHN_WIDTH                       (1<<8)
#define AUD_I2S_TX_WCK_FMT                             (1<<7)
#define AUD_I2S_TX_ENC_FMT                             (1<<6)
#define AUD_I2S_TX_WCK_INV                             (1<<5)
#define AUD_I2S_TX_BCK_INV                             (1<<4)
#define AUD_I2S_TX_BCK_DEGLITCH                        (1<<3)
#define AUD_I2S_TX_BCK_DEGLITCH_NUMBER_POS             0
#define AUD_I2S_TX_BCK_DEGLITCH_NUMBER_MSK             (0x7<<AUD_I2S_TX_BCK_DEGLITCH_NUMBER_POS)


/**
 * @brief Register 50h
 */
#define AUD_MISC_I2S_RX_WCK_WIDTH_POS                  10
#define AUD_MISC_I2S_RX_WCK_WIDTH_MSK                  (0x1F<<AUD_MISC_I2S_RX_WCK_WIDTH_POS)
#define AUD_CODEC_I2S_RX_WCK_WIDTH_POS                 5
#define AUD_CODEC_I2S_RX_WCK_WIDTH_MSK                 (0x1F<<AUD_CODEC_I2S_RX_WCK_WIDTH_POS)
#define AUD_BT_I2S_RX_WCK_WIDTH_POS                    0
#define AUD_BT_I2S_RX_WCK_WIDTH_MSK                    (0x1F<<AUD_BT_I2S_RX_WCK_WIDTH_POS)


/**
 * @brief Register 52h,54h,56h
 */
#define AUD_I2S_TX_CHN_SWAP_POS                        13
#define AUD_I2S_TX_CHN_SWAP_MSK                        (0x3<<AUD_I2S_TX_CHN_SWAP_POS)
#define AUD_I2S_TX_WCK_WIDTH_POS                       8
#define AUD_I2S_TX_WCK_WIDTH_MSK                       (0x1F<<AUD_I2S_TX_WCK_WIDTH_POS)
#define AUD_I2S_TX_ACTIVE_SLOT_POS                     0
#define AUD_I2S_TX_ACTIVE_SLOT_MSK                     (0xFF<<AUD_I2S_TX_ACTIVE_SLOT_POS)


/**
 * @brief Register 5Eh
 */
#define AUD_MISC_I2S_RX_CHN_SWAP_POS                   4
#define AUD_MISC_I2S_RX_CHN_SWAP_MSK                   (0x3<<AUD_MISC_I2S_RX_CHN_SWAP_POS)
#define AUD_BT_I2S_RX_CHN_SWAP_POS                     2
#define AUD_BT_I2S_RX_CHN_SWAP_MSK                     (0x3<<AUD_BT_I2S_RX_CHN_SWAP_POS)
#define AUD_CODEC_I2S_RX_CHN_SWAP_POS                  0
#define AUD_CODEC_I2S_RX_CHN_SWAP_MSK                  (0x3<<AUD_CODEC_I2S_RX_CHN_SWAP_POS)


/**
 * @brief Register 60h,64h, (311480h)
 */
#define AUD_DMA_WR_CHN_MODE_POS                        0
#define AUD_DMA_WR_CHN_MODE_MSK                        (0x3<<AUD_DMA_WR_CHN_MODE_POS)


/**
 * @brief Register 68h
 */
#define AUD_MISC_MCK_SYNTH_EXP_EN                      (1<<15)
#define AUD_CODEC_MCK_SYNTH_EXP_EN                     (1<<14)
#define AUD_SEL_CLK_MISC_I2S_MCK_POS                   4
#define AUD_SEL_CLK_MISC_I2S_MCK_MSK                   (0xF<<AUD_SEL_CLK_MISC_I2S_MCK_POS)
#define AUD_SEL_CLK_CODEC_I2S_MCK_POS                  0
#define AUD_SEL_CLK_CODEC_I2S_MCK_MSK                  (0xF<<AUD_SEL_CLK_CODEC_I2S_MCK_POS)

/**
 * @brief Register 72h
 */
#define AUD_SEL_USB_SYNTH_MS                           (1<<0)

/**
 * @brief Register 8Ah
 */
#define AUD_MIXTER_BT2DAC_L_POS                        12
#define AUD_MIXTER_BT2DAC_L_MSK                        (0x3<<AUD_MIXTER_BT2DAC_L_POS)
#define AUD_MIXTER_BT2DAC_R_POS                        10
#define AUD_MIXTER_BT2DAC_R_MSK                        (0x3<<AUD_MIXTER_BT2DAC_R_POS)
#define AUD_MIXTER_BT2CODEC_L_POS                      8
#define AUD_MIXTER_BT2CODEC_L_MSK                      (0x3<<AUD_MIXTER_BT2CODEC_L_POS)
#define AUD_MIXTER_BT2CODEC_R_POS                      6
#define AUD_MIXTER_BT2CODEC_R_MSK                      (0x3<<AUD_MIXTER_BT2CODEC_R_POS)


/**
 * @brief Register 8Ch
 */
#define AUD_DMA_W4_IN_SEL                              (1<<15)
#define AUD_DMA_W1_IN_SEL                              (1<<14)
#define AUD_DMA_V_IN_SEL                               (1<<13)
#define AUD_DMA_R4_VLD_SEL                             (1<<12)
#define AUD_SRC_A1_2_BYPASS                            (1<<11)

/**
 * @brief Register A0h,A6h,ACh,B2h
 */
#define AUD_FS_SYNTH_NF_VALUE_LO_POS                   0
#define AUD_FS_SYNTH_NF_VALUE_LO_MSK                   (0xFFFF<<AUD_FS_SYNTH_NF_VALUE_LO_POS)


/**
 * @brief Register A2h,A8h,AEh,B4h
 */
#define AUD_FS_SYNTH_NF_VALUE_HI_OFFSET                16
#define AUD_FS_SYNTH_NF_VALUE_HI_POS                   0
#define AUD_FS_SYNTH_NF_VALUE_HI_MSK                   (0xFFFF<<AUD_FS_SYNTH_NF_VALUE_HI_POS)


/**
 * @brief Register E0h,E2h
 */
#define AUD_INT_SYNTH                     (1<<0)
#define AUD_INT_DMA_R1                    (1<<1)
#define AUD_INT_DMA_W1                    (1<<2)
#define AUD_INT_DMA_R2                    (1<<3)
#define AUD_INT_DMA_W2                    (1<<4)
#define AUD_INT_DMA_R3                    (1<<5)
#define AUD_INT_DMA_W3                    (1<<6)
#define AUD_INT_DMA_R4                    (1<<7)
#define AUD_INT_DMA_W4                    (1<<8)
#define AUD_INT_DMA_W1_MCH                (1<<9)
#define AUD_INT_DMA_W2_MCH                (1<<10)
#define AUD_INT_DMA_R5_HDMI               (1<<11)
#define AUD_INT_DMA_W5_VREC               (1<<14)
#define AUD_INT_USB                       (1<<15)
#define AUD_INT_ALL_MSK                   0xFFFF


/**
 * @brief Register E6h
 */
#define AUD_HDMI_NF_SYNTH_TRIG            (1<<15)

/**
 * @brief Register E8h
 */
#define AUD_HDMI_EN_TIME_GEN              (1<<2)

/****************************************************************************/
/*        AUDIO DIGITAL registers bank3 bank4  bank5                                    */
/****************************************************************************/
/**
 * @brief Register 311300h,311340h,311380h,3113C0h,311500h
 */
#define AUD_DMA_RD_RESET                               (1<<15)
#define AUD_DMA_RD_BIT_MODE                            (1<<9)
#define AUD_DMA_RD_LOCAL_EMPTY_FLAG_CLR                (1<<8)
#define AUD_DMA_RD_EMPTY_FLAG_CLR                      (1<<7)
#define AUD_DMA_RD_MIU_HIGH_PRIORITY                   (1<<6)
#define AUD_DMA_RD_LEVEL_MASK                          (1<<5)
#define AUD_DMA_RD_TRIG                                (1<<4)
#define AUD_DMA_RD_LR_SWAP                             (1<<3)
#define AUD_DMA_RD_FREE_RUN                            (1<<2)
#define AUD_DMA_RD_ENABLE                              (1<<1)
#define AUD_DMA_RD_MIU_ENABLE                          (1<<0)


/**
 * @brief Register 311302h,311342h,311382h,3113C2h,311502h
 */
#define AUD_DMA_RD_BASE_ADDR_LO_POS                    0
#define AUD_DMA_RD_BASE_ADDR_LO_MSK                    (0xFFFF << AUD_DMA_RD_BASE_ADDR_LO_POS)


/**
 * @brief Register 311304h,311344h,311384h,3113C4h,311504h
 */
#define AUD_DMA_RD_BASE_ADDR_HI_OFFSET                 16
#define AUD_DMA_RD_BASE_ADDR_HI_POS                    0
#define AUD_DMA_RD_BASE_ADDR_HI_MSK                    (0xFFF << AUD_DMA_RD_BASE_ADDR_HI_POS)

/**
 * @brief Register 311306h,311346h,311386h,3113C6h,311506h
 */
#define AUD_DMA_RD_BUFF_SIZE_POS                       0
#define AUD_DMA_RD_BUFF_SIZE_MSK                       (0xFFFF << AUD_DMA_RD_BUFF_SIZE_POS)


/**
 * @brief Register 311308h,311348h,311388h,3113C8h,311508h
 */
#define AUD_DMA_RD_SIZE_POS                            0
#define AUD_DMA_RD_SIZE_MSK                            (0xFFFF << AUD_DMA_RD_SIZE_POS)


/**
 * @brief Register 31130Ah,31134Ah,31138Ah,3113CAh,31150Ah
 */
#define AUD_DMA_RD_UNDERRUN_TH_POS                     0
#define AUD_DMA_RD_UNDERRUN_TH_MSK                     (0xFFFF << AUD_DMA_RD_UNDERRUN_TH_POS)


/**
 * @brief Register 311310h,311350h,311390h,3113D0h,311510h
 */
#define AUD_DMA_RD_LOCALBUF_EMPTY_INT_MASK             (1 << 2)
#define AUD_DMA_RD_UNDERRUN_INT_MASK                   (1 << 1)
#define AUD_DMA_RD_EMPTY_INT_MASK                      (1 << 0)

/**
 * @brief Register 311314h,311354h,311394h,3113D4h,311514h
 */
#define AUD_DMA_RD_LEVEL_CNT_POS                       0
#define AUD_DMA_RD_LEVEL_CNT_MSK                       (0xFFFF << AUD_DMA_RD_LEVEL_CNT_POS)


/**
 * @brief Register 311316h,311356h,311396h,3113D6h,311516h
 */
#define AUD_DMA_RD_LOCALBUF_EMPTY_STATUS               (1 << 10)
#define AUD_DMA_RD_LOCALBUF_FULL_STATUS                (1 << 9)
#define AUD_DMA_RD_EMPTY_STATUS                        (1 << 8)
#define AUD_DMA_RD_LOCALBUF_EMPTY_FLAG                 (1 << 2)
#define AUD_DMA_RD_UNDERRUN_FLAG                       (1 << 1)
#define AUD_DMA_RD_EMPTY_FLAG                          (1 << 0)


/**
 * @brief Register 311320h,311360h,3113A0h,3113E0h,311460h
 */
#define AUD_DMA_WR_RESET                               (1<<15)
#define AUD_DMA_WR_BIT_MODE                            (1<<9)
#define AUD_DMA_WR_LOCAL_FULL_FLAG_CLR                 (1<<8)
#define AUD_DMA_WR_FULL_FLAG_CLR                       (1<<7)
#define AUD_DMA_WR_MIU_HIGH_PRIORITY                   (1<<6)
#define AUD_DMA_WR_LEVEL_MASK                          (1<<5)
#define AUD_DMA_WR_TRIG                                (1<<4)
#define AUD_DMA_WR_LR_SWAP                             (1<<3)
#define AUD_DMA_WR_FREE_RUN                            (1<<2)
#define AUD_DMA_WR_ENABLE                              (1<<1)
#define AUD_DMA_WR_MIU_ENABLE                          (1<<0)


/**
 * @brief Register 311322h,311362h,3113A2h,3113E2h,311462h
 */
#define AUD_DMA_WR_BASE_ADDR_LO_POS                    0
#define AUD_DMA_WR_BASE_ADDR_LO_MSK                    (0xFFFF << AUD_DMA_WR_BASE_ADDR_LO_POS)


/**
 * @brief Register 311324h,311364h,3113A4h,3113E4h,311464h
 */
#define AUD_DMA_WR_BASE_ADDR_HI_OFFSET                 16
#define AUD_DMA_WR_BASE_ADDR_HI_POS                    0
#define AUD_DMA_WR_BASE_ADDR_HI_MSK                    (0xFFF << AUD_DMA_WR_BASE_ADDR_HI_POS)

/**
 * @brief Register 311326h,311366h,3113A6h,3113E6h,311466h
 */
#define AUD_DMA_WR_BUFF_SIZE_POS                       0
#define AUD_DMA_WR_BUFF_SIZE_MSK                       (0xFFFF << AUD_DMA_WR_BUFF_SIZE_POS)


/**
 * @brief Register 311328h,311368h,3113A8h,3113E8h,311468h
 */
#define AUD_DMA_WR_SIZE_POS                            0
#define AUD_DMA_WR_SIZE_MSK                            (0xFFFF << AUD_DMA_WR_SIZE_POS)


/**
 * @brief Register 31132Ah,31136Ah,3113AAh,3113EAh,31146Ah
 */
#define AUD_DMA_WR_OVERRUN_TH_POS                     0
#define AUD_DMA_WR_OVERRUN_TH_MSK                      (0xFFFF << AUD_DMA_WR_OVERRUN_TH_POS)


/**
 * @brief Register 311330h,311370h,3113B0h,3113F0h,311470h
 */
#define AUD_DMA_WR_LOCALBUF_FULL_INT_MASK              (1 << 2)
#define AUD_DMA_WR_OVERRUN_INT_MASK                    (1 << 1)
#define AUD_DMA_WR_FULL_INT_MASK                       (1 << 0)

/**
 * @brief Register 311334h,311374h,3113B4h,3113F4h,311474h
 */
#define AUD_DMA_WR_LEVEL_CNT_POS                       0
#define AUD_DMA_WR_LEVEL_CNT_MSK                       (0xFFFF << AUD_DMA_WR_LEVEL_CNT_POS)


/**
 * @brief Register 311336h,311376h,3113B6h,3113F6h,311476h
 */
#define AUD_DMA_WR_LOCALBUF_EMPTY_STATUS               (1 << 10)
#define AUD_DMA_WR_LOCALBUF_FULL_STATUS                (1 << 9)
#define AUD_DMA_WR_FULL_STATUS                         (1 << 8)
#define AUD_DMA_WR_LOCALBUF_FULL_FLAG                  (1 << 2)
#define AUD_DMA_WR_OVERRUN_FLAG                        (1 << 1)
#define AUD_DMA_WR_FULL_FLAG                           (1 << 0)

/****************************************************************************/
/*        AUDIO DIGITAL registers bank4                                      */
/****************************************************************************/
/**
 * @brief Register 02h
 */
#define AUD_DMIC_DECIMATION_EN_POS                     0
#define AUD_DMIC_DECIMATION_EN_MSK                     (0x1 << AUD_DMIC_DECIMATION_EN_POS)


#define AUD_DMIC_CHANNEL_POS                           4
#define AUD_DMIC_CHANNEL_MSK                           (0x3 << AUD_DMIC_CHANNEL_POS)

/**
 * @brief Register 04h
 */
#define AUD_DMA_W5_SOURCE_SEL                          (1 << 2)
#define AUD_DMA_W5_DEST_SEL                            (1 << 0)

/**
 * @brief Register 08h
 */
#define AUD_DMIC_AUTO_SEL_MODE_POS                     0
#define AUD_DMIC_AUTO_SEL_MODE_MSK                     (0x1F << AUD_DMIC_AUTO_SEL_MODE_POS)
#if 0
#define AUD_DMIC_AUTO_SEL_POS                          0
#define AUD_DMIC_AUTO_SEL_MSK                          (0x1 << AUD_DMIC_AUTO_SEL_POS)
#endif

/**
 * @brief Register 40h
 */
#define AUD_DMIC_CIC_RESET_POS                         4
#define AUD_DMIC_CIC_RESET_MSK                         (0xFF << AUD_DMIC_CIC_RESET_POS)

#define AUD_DMIC_CIC_EN_POS                            0
#define AUD_DMIC_CIC_EN_MSK                            (0x1 << AUD_DMIC_CIC_EN_POS)


/**
 * @brief Register 42h
 */
#define AUD_DMIC_GAIN2_POS                             12
#define AUD_DMIC_GAIN2_MSK                             (0x7 << AUD_DMIC_GAIN2_POS)

#define AUD_DMIC_GAIN1_POS                             8
#define AUD_DMIC_GAIN1_MSK                             (0x7 << AUD_DMIC_GAIN1_POS)

/**
 * @brief Register 44h
 */
#define AUD_DMIC_GAIN4_POS                             12
#define AUD_DMIC_GAIN4_MSK                             (0x7 << AUD_DMIC_GAIN4_POS)

#define AUD_DMIC_GAIN3_POS                             8
#define AUD_DMIC_GAIN3_MSK                             (0x7 << AUD_DMIC_GAIN3_POS)

/**
 * @brief Register 46h
 */
#define AUD_DMIC_GAIN6_POS                             12
#define AUD_DMIC_GAIN6_MSK                             (0x7 << AUD_DMIC_GAIN6_POS)

#define AUD_DMIC_GAIN5_POS                             8
#define AUD_DMIC_GAIN5_MSK                             (0x7 << AUD_DMIC_GAIN5_POS)

/**
 * @brief Register 48h
 */
#define AUD_DMIC_GAIN8_POS                             12
#define AUD_DMIC_GAIN8_MSK                             (0x7 << AUD_DMIC_GAIN8_POS)

#define AUD_DMIC_GAIN7_POS                             8
#define AUD_DMIC_GAIN7_MSK                             (0x7 << AUD_DMIC_GAIN7_POS)

#if 0


#define AUD_DMIC_PHASE_ALIGNED_POS                     8
#define AUD_DMIC_PHASE_ALIGNED_MSK                     (0x3f << AUD_DMIC_PHASE_ALIGNED_POS)

#define AUD_DMIC_PHASE_EN_POS                          0
#define AUD_DMIC_PHASE_EN_MSK                          (0x1 << AUD_DMIC_PHASE_EN_POS)

#define AUD_DMIC_PHASE_SELECT_AUTO_POS                 15
#define AUD_DMIC_PHASE_SELECT_AUTO_MSK                 (0x1 << AUD_DMIC_PHASE_SELECT_AUTO_POS)

#define AUD_DMIC_PHASE_SELECT_CH2_POS                  8
#define AUD_DMIC_PHASE_SELECT_CH2_MSK                  (0x3f << AUD_DMIC_PHASE_SELECT_CH2_POS)

#define AUD_DMIC_PHASE_SELECT_CH1_POS                  0
#define AUD_DMIC_PHASE_SELECT_CH1_MSK                  (0x3f << AUD_DMIC_PHASE_SELECT_CH1_POS)



#define AUD_DMIC_ENABLE_DECIMATION_POS                 0
#define AUD_DMIC_ENABLE_DECIMATION_MSK                 (0x1f << AUD_DMIC_ENABLE_DECIMATION_POS)

#define AUD_DMIC_ENABLE_TIMEGEN_POS                    8
#define AUD_DMIC_ENABLE_TIMEGEN_MSK                    (0x1 << AUD_DMIC_ENABLE_TIMEGEN_POS)

#define AUD_DMIC_RESET_SRAM_POS                        15
#define AUD_DMIC_RESET_SRAM_MSK                        (0x1 << AUD_DMIC_RESET_SRAM_POS)

#endif

/****************************************************************************/
/*        AUDIO DIGITAL registers bank5                                      */
/****************************************************************************/
/**
 * @brief Register A4h
 */
#define AUD_DMA_WR5_MCH_POS                        0
#define AUD_DMA_WR5_MCH_MSK                        (0xF << AUD_DMA_WR5_MCH_POS)
#define AUD_DMA_WR5_MCH_ENABLE                     (1 << 7)

#endif //__HAL_AUD_REG_H__
