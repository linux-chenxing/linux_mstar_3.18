// $Change: 548282 $
//
// bach_reg.h
//
// definitions for the C3 BACH, Mstar's Audio Codec Spec
//

#ifndef _BACH_REG_H_
#define _BACH_REG_H_

enum
{
    // ----------------------------------------------
    // BANK1 SYS
    // ----------------------------------------------
    BACH_SYS_RST_CTRL0        =  0x00,
    BACH_CKG_CTRL0            =  0x02,
    BACH_CKG_CTRL1            =  0x04,
    BACH_TIMING_CTRL0         =  0x06,
    BACH_AUDIOBAND_CTRL0      =  0x08,
    BACH_AUDIOBAND_STS        =  0x0A,
    BACH_SDM_CTRL0            =  0x0C,
    BACH_SDM_CTRL1            =  0x0E,
    BACH_NF_SYNTH_1_NF_H      =  0x10,
    BACH_NF_SYNTH_1_NF_L      =  0x12,
    BACH_NF_SYNTH_2_NF_H      =  0x14,
    BACH_NF_SYNTH_2_NF_L      =  0x16,
    BACH_FS_SYNTH_CTRL0       =  0x18,
    BACH_FS_SYNTH_STS0        =  0x1A,
    BACH_FS_SYNTH_NF_H        =  0x1C,
    BACH_FS_SYNTH_NF_L        =  0x1E,
    BACH_FS_SYNTH_FRC         =  0x20,
    BACH_IIR_CTRL0            =  0x22,
    BACH_IIR_CTRL1            =  0x24,
    BACH_IIR_CTRL2            =  0x26,
    BACH_MIX_CTRL0            =  0x28,
    BACH_BT_I2S_CTRL0         =  0x30,
    BACH_BT_I2S_CTRL1         =  0x32,
    BACH_BT_I2S_STS           =  0x34,
    BACH_RX_FIFO_CTRL0        =  0x36,
    BACH_RX_FIFO_STS          =  0x38,
    BACH_TIMING_CTRL1         =  0xA0,
    BACH_AUDIOBAND_CTRL1      =  0xA2,
    BACH_NF_SYNTH_4_NF_H      =  0xA4,
    BACH_NF_SYNTH_4_NF_L      =  0xA6,
    BACH_NF_SYNTH_5_NF_H      =  0xA8,
    BACH_NF_SYNTH_5_NF_L      =  0xAA,
    BACH_INT_CTRL             =  0xAC,
    BACH_SINE_GEN_CTRL0       =  0xB2,
    BACH_SINE_GEN_CTRL1       =  0xB4,
    BACH_TEST_CTRL0           =  0xB6,
    BACH_RESERVED_0xB8        =  0xB8,
    BACH_RESERVED_0xBA        =  0xBA,
    BACH_RESERVED_0xBC        =  0xBC,
    BACH_RESERVED_0xBE        =  0xBE,
    BACH_RESERVED_0xF4        =  0xF4,
    BACH_RESERVED_0xF6        =  0xF6,
    BACH_RESERVED_0xF8        =  0xF8,
    BACH_RESERVED_0xFA        =  0xFA,
    BACH_RESERVED_0xFC        =  0xFC,
    BACH_RESERVED_0xFE        =  0xFE
};


enum
{
    // ----------------------------------------------
    // BANK1 DMA
    // ----------------------------------------------
    BACH_DMA1_CTRL_0       =  0x80,
    BACH_DMA1_CTRL_1       =  0x82,
    BACH_DMA1_CTRL_2       =  0x84,
    BACH_DMA1_CTRL_3       =  0x86,
    BACH_DMA1_CTRL_4       =  0x88,
    BACH_DMA1_CTRL_5       =  0x8A,
    BACH_DMA1_CTRL_6       =  0x8C,
    BACH_DMA1_CTRL_7       =  0x8E,
    BACH_DMA1_CTRL_8       =  0x90,
    BACH_DMA1_CTRL_9       =  0x92,
    BACH_DMA1_CTRL_10      =  0x94,
    BACH_DMA1_CTRL_11      =  0x96,
    BACH_DMA1_CTRL_12      =  0x98,
    BACH_DMA1_CTRL_13      =  0x9A,
    BACH_DMA1_CTRL_14      =  0x9C,
    BACH_DMA1_CTRL_15      =  0x9E,
    BACH_DMA2_CTRL_0       =  0xC0,
    BACH_DMA2_CTRL_1       =  0xC2,
    BACH_DMA2_CTRL_2       =  0xC4,
    BACH_DMA2_CTRL_3       =  0xC6,
    BACH_DMA2_CTRL_4       =  0xC8,
    BACH_DMA2_CTRL_5       =  0xCA,
    BACH_DMA2_CTRL_6       =  0xCC,
    BACH_DMA2_CTRL_7       =  0xCE,
    BACH_DMA2_CTRL_8       =  0xD0,
    BACH_DMA2_CTRL_9       =  0xD2,
    BACH_DMA2_CTRL_10      =  0xD4,
    BACH_DMA2_CTRL_11      =  0xD6,
    BACH_DMA2_CTRL_12      =  0xD8,
    BACH_DMA2_CTRL_13      =  0xDA,
    BACH_DMA2_CTRL_14      =  0xDC,
    BACH_DMA2_CTRL_15      =  0xDE,
    BACH_DMA_TEST_CTRL0    =  0xE0,
    BACH_DMA_TEST_CTRL1    =  0xE2,
    BACH_DMA_TEST_CTRL2    =  0xE4,
    BACH_DMA_TEST_CTRL3    =  0xE6,
    BACH_DMA_TEST_CTRL4    =  0xE8,
    BACH_DMA_TEST_CTRL5    =  0xEA,
    BACH_DMA_TEST_CTRL6    =  0xEC,
    BACH_DMA_TEST_CTRL7    =  0xEE,
    BACH_DMA_TOP_CTRL0     =  0xF0,
    BACH_DMA_TOP_CTRL1     =  0xF2
};


enum
{
    // ----------------------------------------------
    // BANK2 SYS
    // ----------------------------------------------
    BACH_AUDIOBAND_CTRL2    =  0x00,
    BACH_AUDIOBAND_CTRL3    =  0x02,
    BACH_NF_SYNTH_3_NF_H    =  0x04,
    BACH_NF_SYNTH_3_NF_L    =  0x06,
    BACH_NF_SYNTH_6_NF_H    =  0x08,
    BACH_NF_SYNTH_6_NF_L    =  0x0A,
    BACH_NF_SYNTH_7_NF_H    =  0x0C,
    BACH_NF_SYNTH_7_NF_L    =  0x0E,
    BACH_SPDIF_TX_CTRL0     =  0x10,
    BACH_SPDIF_TX_CTRL1     =  0x12,
    BACH_SPDIF_TX_CTRL2     =  0x14,
    BACH_SPDIF_TX_STS       =  0x16,
    BACH_SPDIF_RX_CTRL0     =  0x18,
    BACH_SPDIF_RX_CTRL1     =  0x1A,
    BACH_SPDIF_RX_STS0      =  0x1C,
    BACH_SPDIF_RX_STS1      =  0x1E,
    BACH_SPDIF_RX_STS2      =  0x20,
    BACH_SPDIF_RX_STS3      =  0x22,
    BACH_DBB_I2S_CTRL       =  0x24,
    BACH_CODEC_I2S_CTRL     =  0x26,
    BACH_EXT_I2S_CTRL       =  0x28,
    BACH_PAD_CTRL0          =  0x2A,
    BACH_PAD_CTRL1          =  0x2C,
    BACH_AUDIOBAND_STS2     =  0x2E,
    BACH_DIG_MIC_CTRL1      =  0x30,
    BACH_DIG_MIC_CTRL2      =  0x32,
    BACH_DIG_MIC_CTRL3      =  0x34,
    BACH_DMA2_WR_DATA_L     =  0x40,
    BACH_DMA2_WR_DATA_R     =  0x42,
    BACH_DEC3_DOUT          =  0x44,
    BACH_DEC4_DOUT          =  0x46,
    BACH_ASRC2_DEC3_OUT     =  0x48,
    BACH_ASRC2_DEC4_OUT     =  0x4A,
    BACH_SDM_CH3_IN         =  0x4C,
    BACH_SDM_CH4_IN         =  0x4E,
    BACH_PARSER_CTRL0       =  0x60,
    BACH_PARSER_CTRL1       =  0x62,
    BACH_PARSER_CTRL2       =  0x64,
    BACH_PARSER_CTRL3       =  0x66,
    BACH_PARSER_CTRL4       =  0x68,
    BACH_PARSER_CTRL5       =  0x6A,
    BACH_PARSER_STATUS0     =  0x80,
    BACH_PARSER_STATUS1     =  0x82,
    BACH_PARSER_STATUS2     =  0x84,
    BACH_PARSER_STATUS3     =  0x86,
    BACH_PARSER_STATUS4     =  0x88,
    BACH_PARSER_STATUS5     =  0x8A,
    BACH_PARSER_STATUS6     =  0x8C,
    BACH_PARSER_STATUS7     =  0x8E,
    BACH_PARSER_STATUS8     =  0x90,
    BACH_PARSER_STATUS9     =  0x92,
    BACH_PARSER_STATUS10    =  0x94,
    BACH_PARSER_STATUS11    =  0x96,
    BACH_PARSER_STATUS12    =  0x98,
    BACH_PARSER_STATUS13    =  0x9A,
    BACH_PARSER_STATUS14    =  0x9C,
    BACH_PARSER_STATUS15    =  0x9E,
    BACH_DEC1_DOUT_AB       =  0xA0,
    BACH_DEC1_DOUT_IIR      =  0xA2,
    BACH_DEC1_DOUT          =  0xA4,
    BACH_DEC2_DOUT          =  0xA6,
    BACH_INT1_DOUT          =  0xA8,
    BACH_INT2_DOUT          =  0xAA,
    BACH_IIR1_DOUT          =  0xAC,
    BACH_IIR2_DOUT          =  0xAE,
    BACH_FM_RX_L_DIN        =  0xB0,
    BACH_FM_RX_R_DIN        =  0xB2,
    BACH_BT_DL_L_DOUT       =  0xB4,
    BACH_BT_DL_R_DOUT       =  0xB6,
    BACH_BT_UL_MONO_DOUT    =  0xB8,
    BACH_DMARD_L_DOUT       =  0xBA,
    BACH_DMARD_R_DOUT       =  0xBC,
    BACH_DMAWR_L_DOUT       =  0xBE,
    BACH_DMAWR_R_DOUT       =  0xC0,
    BACH_DBB_DL_MONO_DIN    =  0xC2,
    BACH_TEST_PCM_OUT_CH1   =  0xC4,
    BACH_TEST_PCM_OUT_CH2   =  0xC6,
    BACH_VALDI_SIGNALS      =  0xC8,
    BACH_INT_SIGNALS        =  0xCA,
    BACH_CLK_SIGNALS        =  0xCC,
    BACH_RESERVED_READ      =  0xCE,
    BACH_ASRC2_MMP1_IN      =  0xD0,
    BACH_ASRC2_MMP2_IN      =  0xD2,
    BACH_ASRC2_INT1_IN      =  0xD4,
    BACH_ASRC2_INT2_IN      =  0xD6,
    BACH_CH3_CIC_ABS_OUT    =  0xD8,
    BACH_CH4_CIC_ABS_OUT    =  0xDA,
    BACH_ASRC2_DEC1_OUT     =  0xDC,
    BACH_ASRC2_DEC2_OUT     =  0xDE,
    BACH_ASRC2_INT1_OUT     =  0xE0,
    BACH_ASRC2_INT2_OUT     =  0xE2,
    BACH_SDM_CH1_IN         =  0xE4,
    BACH_SDM_CH2_IN         =  0xE6
};

enum
{
    // ----------------------------------------------
    // BANK3 ATOP
    // ----------------------------------------------
    BACH_ANALOG_CTRL01     =  0x02,
    BACH_ANALOG_CTRL02     =  0x04,
    BACH_ANALOG_CTRL03     =  0x06,
    BACH_ANALOG_CTRL04     =  0x08,
    BACH_ANALOG_CTRL05     =  0x0A,
    BACH_ANALOG_CTRL06     =  0x0C,
    BACH_ANALOG_CTRL07     =  0x0E,
    BACH_ANALOG_CTRL08     =  0x10,
    BACH_ANALOG_CTRL09     =  0x12,
    BACH_ANALOG_CTRL10     =  0x14,
    BACH_ANALOG_CTRL11     =  0x16,
    BACH_ANALOG_CTRL12     =  0x18,
    BACH_ANALOG_CTRL13     =  0x1A,
    BACH_ANALOG_CTRL14     =  0x1C,
    BACH_ANALOG_CTRL15     =  0x1E,
    BACH_ANALOG_CTRL16     =  0x20,
    BACH_ANALOG_CTRL17     =  0x22,
    BACH_ANALOG_CTRL18     =  0x24,
    BACH_ANALOG_CTRL19     =  0x26
};

enum
{
    // ----------------------------------------------
    // BANK4 DPGA
    // ----------------------------------------------
    BACH_MMP1_DPGA_CFG1            =  0x40,
    BACH_MMP1_DPGA_CFG2            =  0x42,
    BACH_MMP2_DPGA_CFG1            =  0x44,
    BACH_MMP2_DPGA_CFG2            =  0x46,
    BACH_DEC1_1_DPGA_CFG1          =  0x48,
    BACH_DEC1_1_DPGA_CFG2          =  0x4A,
    BACH_DEC1_2_DPGA_CFG1          =  0x4C,
    BACH_DEC1_2_DPGA_CFG2          =  0x4E,
    BACH_DEC1_3_DPGA_CFG1          =  0x50,
    BACH_DEC1_3_DPGA_CFG2          =  0x52,
    BACH_DBB_1_DPGA_CFG1           =  0x54,
    BACH_DBB_1_DPGA_CFG2           =  0x56,
    BACH_DBB_2_DPGA_CFG1           =  0x58,
    BACH_DBB_2_DPGA_CFG2           =  0x5A,
    BACH_DMARD_L_DPGA_CFG1         =  0x5C,
    BACH_DMARD_L_DPGA_CFG2         =  0x5E,
    BACH_DMARD_R_DPGA_CFG1         =  0x60,
    BACH_DMARD_R_DPGA_CFG2         =  0x62,
    BACH_ASRC2_MMP1_DPGA_CFG1      =  0x64,
    BACH_ASRC2_MMP1_DPGA_CFG2      =  0x66,
    BACH_ASRC2_MMP2_DPGA_CFG1      =  0x68,
    BACH_ASRC2_MMP2_DPGA_CFG2      =  0x6A,
    BACH_CODEC1_DPGA_CFG1          =  0x6C,
    BACH_CODEC1_DPGA_CFG2          =  0x6E,
    BACH_CODEC2_DPGA_CFG1          =  0x70,
    BACH_CODEC2_DPGA_CFG2          =  0x72,
    BACH_SPDIF1_DPGA_CFG1          =  0x74,
    BACH_SPDIF1_DPGA_CFG2          =  0x76,
    BACH_SPDIF2_DPGA_CFG1          =  0x78,
    BACH_SPDIF2_DPGA_CFG2          =  0x7A,
    BACH_DMAWR1_DPGA_CFG1          =  0x7C,
    BACH_DMAWR1_DPGA_CFG2          =  0x7E,
    BACH_DMAWR2_DPGA_CFG1          =  0x80,
    BACH_DMAWR2_DPGA_CFG2          =  0x82,
    BACH_DMAWR3_DPGA_CFG1          =  0x84,
    BACH_DMAWR3_DPGA_CFG2          =  0x86,
    BACH_DMAWR4_DPGA_CFG1          =  0x88,
    BACH_DMAWR4_DPGA_CFG2          =  0x8A,
    BACH_PGA1_DPGA_CFG1            =  0x8C,
    BACH_PGA2_DPGA_CFG1            =  0x8E,
    BACH_DPGA_MUTE_CFG             =  0x90,
    BACH_DPGA_MUTE_CFG2            =  0x92,
    BACH_DPGA_MUTE_STS             =  0x94,
    BACH_DPGA_MUTE_STS2            =  0x96,
    BACH_DPGA_MUTE_STS3            =  0x98
};



/****************************************************************************/
/*        AUDIO ChipTop                                     */
/****************************************************************************/
/**
 * @brief Register 1A00h,
 */
#define REG_BTI2S_MODE_POS          2
#define REG_BTI2S_MODE_MSK          (0x3 << REG_BTI2S_MODE_POS)
#define REG_EXTI2S_MODE_POS         11
#define REG_EXTI2S_MODE_MSK         (0x3 << REG_EXTI2S_MODE_POS)

/**
 * @brief Register 1A04h,
 */
#define REG_SPDIFOUT_MODE           (1<<14)

/**
 * @brief Register 1A10h,
 */
#define REG_DBBI2S_MODE_POS          2
#define REG_DBBI2S_MODE_MSK          (0x3 << REG_DBBI2S_MODE_POS)

/**
 * @brief Register 0B02h,
 */
#define REG_CLK_432_AU_POS          4
#define REG_CLK_432_AU_MSK          (0x3 << REG_CLK_432_AU_POS )


/****************************************************************************/
/*        AUDIO DIGITAL registers bank1                                     */
/****************************************************************************/
/**
 * @brief Register 00h,
 */
#define REG_RESET_AU_SYNC       (1<<15)
#define REG_RESET_DPGA_SYNC     (1<<14)
#define REG_RESET_SDM_SYNC      (1<<13)

/**
 * @brief Register 02h
 */
#define REG_AU_SYS_48M_GATE           (1<<15)
#define REG_AU_SYS_48M_INV_GATE       (1<<14)
#define REG_AU_DAC_GATE               (1<<13)
#define REG_AU_ADC_GATE               (1<<12)
#define REG_AU_SRC1_MAC_48M_GATE      (1<<11)
#define REG_AU_SRC2_MAC_48M_GATE      (1<<10)
#define REG_AU_SYS_216M_GATE          (1<<9)
#define REG_AU_SYS_432M_GATE          (1<<8)
#define REG_AU_CLK_PARSER_GATE        (1<<7)
#define REG_AU_SPDIF_TX_128FS_GATE    (1<<6)
#define REG_AU_SPDIF_RX_128FS_GATE    (1<<5)

/**
 * @brief Register 04h,
 */
#define REG_AU_SYS_48M_INVT           (1<<15)
#define REG_AU_SYS_48M_INV_INVT       (1<<14)
#define REG_AU_DAC_INVT               (1<<13)
#define REG_AU_ADC_INVT               (1<<12)
#define REG_AU_SRC1_MAC_48M_INVT      (1<<11)
#define REG_AU_SRC2_MAC_48M_INVT      (1<<10)
#define REG_AU_SYS_216M_INVT          (1<<9)
#define REG_AU_SYS_432M_INVT          (1<<8)
#define REG_AU_CLK_PARSER_INVT        (1<<7)
#define REG_AU_SPDIF_TX_128FS_INVT    (1<<6)
#define REG_AU_SPDIF_RX_128FS_INVT    (1<<5)

#define REG_AU_MCLK_SEL_POS           2
#define REG_AU_MCLK_SEL_MSK           (0x7<<REG_AU_MCLK_SEL_POS)

#define REG_AU_MCLK_DIV2              (1<<1)
#define REG_CKG_AU_SYS_216_SEL        (1<<0)

/**
 * @brief Register 06h,
 */
#define REG_SYNC_CLK1_SEL_POS   14
#define REG_SYNC_CLK1_SEL_MSK   (0x3<<REG_SYNC_CLK1_SEL_POS)

#define REG_SYNC_CLK2_SEL_POS   12
#define REG_SYNC_CLK2_SEL_MSK   (0x3<<REG_SYNC_CLK2_SEL_POS)

#define REG_DEC_NUM_POS         8
#define REG_DEC_NUM_MSK         (0x3<<REG_DEC_NUM_POS)

#define REG_CH1_INT_128FS_SEL   (1 << 7)
#define REG_CH2_INT_128FS_SEL   (1 << 6)
#define REG_EN_ASRC1            (1 << 5)
#define REG_EN_INT_2            (1 << 4)
#define REG_EN_INT_1            (1 << 3)
#define REG_EN_DEC_2            (1 << 2)
#define REG_EN_DEC_1            (1 << 1)
#define REG_EN_TIME_GEN         (1 << 0)

/**
 * @brief Register 08h,
 */
#define REG_NF_SYNTH_3_RATE_SEL_POS   14
#define REG_NF_SYNTH_3_RATE_SEL_MSK   (0x3<<REG_NF_SYNTH_3_RATE_SEL_POS)

#define REG_CH1_INT_128FS_NF_SEL      (1<<13)
#define REG_CH2_INT_128FS_NF_SEL      (1<<12)
#define REG_DEC_125FS_NF_SEL          (1<<11)

#define REG_DMA1_RD_VALID_SEL_POS     9
#define REG_DMA1_RD_VALID_SEL_MSK     (0x3<<REG_DMA1_RD_VALID_SEL_POS)

#define REG_DMA1_WR_VALID_POS         8
#define REG_DMA1_WR_VALID_SEL         (1<<8)
#define REG_CH12_SYNC_ENABLE          (1<<1)
#define REG_INI_SRAM                  (1<<0)

/**
 * @brief Register 0Ah,
 */
#define REG_IIR_CH3_MACOVER     (1<<15)
#define REG_IIR_CH2_MACOVER     (1<<14)
#define REG_IIR_CH1_MACOVER     (1<<13)
#define REG_IIR_CH4_MACOVER     (1<<12)

#define QUANT_OUT_L_POS         6
#define QUANT_OUT_L_MSK         (0x3F<<QUANT_OUT_L_POS)

#define QUANT_OUT_R_POS         0
#define QUANT_OUT_R_MSK         (0x3F<<QUANT_OUT_L_POS)


/**
 * @brief Register 0Ch,
 */
#define REG_EN_SDM             (1<<0)
#define REG_FS_SYNTH_SEL       (1<<1)

#define REG_DAC_DIN_L_SEL_POS  2
#define REG_DAC_DIN_L_SEL_MSK  (0x3<<REG_DAC_DIN_L_SEL_POS)

#define REG_DAC_DIN_R_SEL_POS  4
#define REG_DAC_DIN_R_SEL_MSK  (0x3<<REG_DAC_DIN_L_SEL_POS)

#define REG_FIX_MSB_EN         (1<<6)

#define REG_FIX_MSB_SEL_POS    7
#define REG_FIX_MSB_SEL_MSK    (0x1F<<REG_FIX_MSB_SEL_POS)

#define REG_DITHER_EN          (1<<12)

#define REG_DITHER_SEL_POS     13
#define REG_DITHER_SEL_MSK     (0x7<<REG_DITHER_SEL_POS)

/**
 * @brief Register 0Eh,
 */
#define REG_SDM_OFFSET_POS     0
#define REG_SDM_OFFSET_MSK     (0xFFFF<<REG_SDM_OFFSET_POS)

/**
 * @brief Register 10h,14h,A4h,A8h; 04h,08h,0Ch(bank2)
 */
#define REG_NF_SYNTH_TRIG             (1<<15)
#define REG_NF_SYNTH_VALUE_HI_OFFSET  16
#define REG_NF_SYNTH_VALUE_HI_POS     0
#define REG_NF_SYNTH_VALUE_HI_MSK     (0x3FFF<<REG_NF_SYNTH_VALUE_HI_POS)

/**
 * @brief Register 12h,16h,A6h,AAh; 06h,0Ah,0Eh(bank2)
 */
#define REG_NF_SYNTH_VALUE_LO_OFFSET 0
#define REG_NF_SYNTH_VALUE_LO_POS    0
#define REG_NF_SYNTH_VALUE_LO_MSK    (0xFFFF<<REG_NF_SYNTH_VALUE_LO_POS)

/**
 * @brief Register 18h
 */
#define REG_FS_SYNTH_EN         (1<<0)
#define REG_FS_SYNTH_SOURCE_SEL (1<<1)

#define REG_FS_SYNTH_BW_POS     4
#define REG_FS_SYNTH_BW_MSK     (0x7<<REG_FS_SYNTH_BW_POS)

#define REG_FS_SYNTH_FLOCK      (1<<7)
#define REG_FS_SYNTH_RATE_SEL   (1<<8)

#define REG_FS_SYNTH_SOURCE_SEL   (1<<1)

/**
 * @brief Register 1Ah
 */
#define REG_FS_SYNTH_NO_SIGNAL        (1<<0)

/**
 * @brief Register 1Ch,1Eh
 */
#define REG_FS_SYNTH_NF_POS     0
#define REG_FS_SYNTH_NF_MSK     (0xFFFF<<REG_FS_SYNTH_NF_POS)

/**
 * @brief Register 20h
 */
#define REG_FS_SYNTH_FRC_POS    0
#define REG_FS_SYNTH_FRC_MSK    (0xFFFF<<REG_FS_SYNTH_FRC_POS)

/**
 * @brief Register 22h
 */
#define REG_IIR_CH1_EN        (1<<0)
#define REG_IIR_CH2_EN        (1<<1)
#define REG_IIR_CH3_EN        (1<<2)
#define REG_IIR_CH4_EN        (1<<3)
#define REG_IIR_BUF_INIT      (1<<4)
#define REG_IIR_COEFF_WR      (1<<5)
#define REG_IIR_CLR_MACOVER   (1<<15)

/**
 * @brief Register 24h
 */
#define REG_IIR_COEFF_WR_ADDR_POS     0
#define REG_IIR_COEFF_WR_ADDR_MSK     (0x3F<<REG_IIR_COEFF_WR_ADDR_POS)

#define REG_IIR_COEFF_DATA_HI_OFFSET  16
#define REG_IIR_COEFF_DATA_HI_POS     8
#define REG_IIR_COEFF_DATA_HI_MSK     (0xFF<<REG_IIR_COEFF_DATA_HI_POS)

/**
 * @brief Register 26h
 */
#define REG_IIR_COEFF_DATA_LO_OFFSET  0
#define REG_IIR_COEFF_DATA_LO_POS     0
#define REG_IIR_COEFF_DATA_LO_MSK     (0xFFFF<<REG_IIR_COEFF_DATA_LO_POS)

/**
 * @brief Register 28h
 */
#define REG_SEL_DEC1_DOUT       (1<<15)
#define REG_SEL_DEC2_DOUT       (1<<14)

#define REG_ST_MIX_POS          12
#define REG_ST_MIX_MSK          (0x3<<REG_ST_MIX_POS)

#define REG_AV_MIX_POS          10
#define REG_AV_MIX_MSK          (0x3<<REG_AV_MIX_POS)

#define REG_DBB_UL_MIX_POS      8
#define REG_DBB_UL_MIX_MSK      (0x3<<REG_DBB_UL_MIX_POS)

#define REG_AMR_MIX_POS         6
#define REG_AMR_MIX_MSK         (0x3<<REG_AMR_MIX_POS)

#define REG_CH1_MUX_SEL_POS     4
#define REG_CH1_MUX_SEL_MSK     (0x3<<REG_CH1_MUX_SEL_POS)

#define REG_CH2_MUX_SEL_POS     2
#define REG_CH2_MUX_SEL_MSK     (0x3<<REG_CH2_MUX_SEL_POS)

#define REG_DMAWR_MUX_SEL_POS   0
#define REG_DMAWR_MUX_SEL_MSK   (0x3<<REG_DMAWR_MUX_SEL_POS)

/**
 * @brief Register 30h
 */
#define REG_BT_PCM_MODE             (1<<15)
#define REG_BT_PCM_SYNC_SEL         (1<<14)
#define REG_BT_I2S_TRX_BCK_INV      (1<<13)
#define REG_PAD_I2S_TRX_BCK_INV     (1<<12)
#define REG_BT_I2S_TX_DS2           (1<<9)
#define REG_BT_I2S_TX_DS            (1<<8)
#define REG_BT_MS_MODE              (1<<7)

#define REG_BT_I2S_WDTH_POS         5
#define REG_BT_I2S_WDTH_MSK         (0x3<<REG_BT_I2S_WDTH_POS)

#define REG_BT_I2S_FMT              (1<<4)
#define REG_BT_I2S_BCK_GEN_DIV2     (1<<3)

#define REG_BT_I2S_BCK_GEN_SEL_POS  1
#define REG_BT_I2S_BCK_GEN_SEL_MSK  (0x3<<REG_BT_I2S_BCK_GEN_SEL_POS)

#define REG_BT_I2S_BCK_GEN_UPDATE   (1<<0)

/**
 * @brief Register 32h
 */
#define REG_BT_I2S_TRX_WCK_OEN    (1<<15)
#define REG_BT_I2S_TRX_BCK_OEN    (1<<14)
#define REG_BT_I2S_TRX_SDI_OEN    (1<<13)
#define REG_BT_I2S_TRX_SDO_OEN    (1<<12)
#define REG_BT_I2S_SOURCE_SEL     (1<<5)

/**
 * @brief Register 34h
 */
#define REG_BT_I2S_OUT_WDTH_POS   2
#define REG_BT_I2S_OUT_WDTH_MSK   (0x3<<REG_BT_I2S_OUT_WDTH_POS)

#define REG_BT_I2S_IN_WDTH_POS    0
#define REG_BT_I2S_IN_WDTH_MSK    (0x3<<REG_BT_I2S_IN_WDTH_POS)

/**
 * @brief Register 36h
 */

#define REG_CODEC_I2S_TX_R_SWAP          (1<<13)
#define REG_CODEC_I2S_TX_L_SWAP          (1<<12)
#define REG_HDMI_TX_R_SWAP               (1<<11)
#define REG_HDMI_TX_L_SWAP               (1<<10)
#define REG_SPDIF_TX_R_SWAP              (1<<9)
#define REG_SPDIF_TX_L_SWAP              (1<<8)
#define REG_BT_RX_FIFO_EXTENSION         (1<<5)
#define REG_BT_RX_FIFO_STATUS_CLR_p      (1<<4)

/**
 * @brief Register 38h
 */
#define REG_BT_RX_FIFO_STATUS     (1<<0)

/**
 * @brief Register A0h
 */
#define REG_CH1_INT_CNT_SYNC_SEL         (1<<15)
#define REG_CH2_INT_CNT_SYNC_SEL         (1<<14)

#define REG_DEC_NUM2_POS                 8
#define REG_DEC_NUM2_MSK                 (0x3<<REG_DEC_NUM2_POS)

#define REG_ASRC2_CH1_INT_128FS_SEL      (1<<7)
#define REG_ASRC2_CH2_INT_128FS_SEL      (1<<6)
#define REG_EN_ASRC2_p                   (1<<5)
#define REG_EN_ASRC2_INT_1_p             (1<<4)
#define REG_EN_ASRC2_INT_2_p             (1<<3)
#define REG_EN_ASRC2_DEC_1_p             (1<<2)
#define REG_EN_ASRC2_DEC_2_p             (1<<1)

/**
 * @brief Register A2h
 */
#define REG_ASRC2_CH_MUX_SEL_POS      14
#define REG_ASRC2_CH_MUX_SEL_MSK      (0x3<<REG_ASRC2_CH_MUX_SEL_POS)

#define REG_SDM_DWA_DATAIN_L_SEL_POS  12
#define REG_SDM_DWA_DATAIN_L_SEL_MSK  (0x3<<REG_SDM_DWA_DATAIN_L_SEL_POS)

#define REG_SDM_DWA_DATAIN_R_SEL_POS  10
#define REG_SDM_DWA_DATAIN_R_SEL_MSK  (0x3<<REG_SDM_DWA_DATAIN_R_SEL_POS)

#define REG_S2M_ASRC2_CH1_SEL_POS     8
#define REG_S2M_ASRC2_CH1_SEL_MSK     (0x3<<REG_S2M_ASRC2_CH1_SEL_POS)

#define REG_S2M_ASRC2_CH2_SEL_POS     6
#define REG_S2M_ASRC2_CH2_SEL_MSK     (0x3<<REG_S2M_ASRC2_CH2_SEL_POS)

#define REG_DMARD_L_DPGA_DIN_SEL      (1<<5)
#define REG_DMARD_R_DPGA_DIN_SEL      (1<<4)
#define REG_ASRC2_FS_SYNTH_SEL_p      (1<<2)
#define REG_ASRC2_CH12_SYNC_ENABLE    (1<<1)

/**
 * @brief Register ACh
 */
#define Interrupt_test_Mode           (1<<15)
#define Level_interrupt_test_value    (1<<14)
#define Edge_interrupt_test_value     (1<<13)
#define pasA_mis_shot                 (1<<4)
#define AU_PARSER_INT_EN              (1<<2)
#define AU_DMA_INT_EN                 (1<<1)
#define AU_DPGA_INT_EN                (1<<0)

/**
 * @brief Register B2h
 */
#define REG_INT1_MIX_CTRL_POS         14
#define REG_INT1_MIX_CTRL_MSK         (0x3<<REG_INT1_MIX_CTRL_POS)

#define REG_INT2_MIX_CTRL_POS         12
#define REG_INT2_MIX_CTRL_MSK         (0x3<<REG_INT2_MIX_CTRL_POS)

#define REG_ASRC2_INT1_MIX_CTRL_POS   10
#define REG_ASRC2_INT1_MIX_CTRL_MSK   (0x3<<REG_ASRC2_INT1_MIX_CTRL_POS)

#define REG_ASRC2_INT2_MIX_CTRL_POS   8
#define REG_ASRC2_INT2_MIX_CTRL_MSK   (0x3<<REG_ASRC2_INT2_MIX_CTRL_POS)

#define REG_PGA1_SWAP                 (1<<7)
#define REG_PGA2_SWAP                 (1<<6)
#define REG_EN_DIT_IIR                (1<<4)
#define REG_EN_DIT_SRC                (1<<3)
#define REG_EN_DIT_SINE               (1<<2)
#define REG_CH2_SIN_GEN_EN            (1<<1)
#define REG_CH1_SIN_GEN_EN            (1<<0)

/**
 * @brief Register B4h
 */
#define REG_SINE_GEN_CH2_GAIN_POS   12
#define REG_SINE_GEN_CH2_GAIN_MSK   (0xF<<REG_SINE_GEN_CH2_GAIN_POS)

#define REG_SINE_GEN_CH2_FREQ_POS   8
#define REG_SINE_GEN_CH2_FREQ_MSK   (0xF<<REG_SINE_GEN_CH2_FREQ_POS)

#define REG_SINE_GEN_CH1_GAIN_POS   4
#define REG_SINE_GEN_CH1_GAIN_MSK   (0xF<<REG_SINE_GEN_CH1_GAIN_POS)

#define REG_SINE_GEN_CH1_FREQ_POS   0
#define REG_SINE_GEN_CH1_FREQ_MSK   (0xF<<REG_SINE_GEN_CH1_FREQ_POS)

/**
 * @brief Register B6h
 */
#define REG_DAC_TEST_EN         (1<<0)
#define REG_SDM_TEST_EN         (1<<1)
#define REG_SDM_SFT_DIS         (1<<2)
#define REG_SRAM_CG_EN          (1<<3)
#define REG_TESTCLK_SEL_POS     4
#define REG_TESTCLK_SEL_MSK     (0x1F<<REG_TESTCLK_SEL_POS)
#define REG_TESTBUS_SEL_POS     12
#define REG_TESTBUS_SEL_MSK     (0xF<<REG_TESTBUS_SEL_POS)


/**
 * @brief Register 80h, C0h
 */
#define REG_WR_UNDERRUN_INT_EN       (1 << 15)
#define REG_WR_OVERRUN_INT_EN        (1 << 14)
#define REG_RD_UNDERRUN_INT_EN       (1 << 13)
#define REG_RD_OVERRUN_INT_EN        (1 << 12)
#define REG_WR_FULL_INT_EN           (1 << 11)
#define REG_RD_EMPTY_INT_EN          (1 << 10)
#define REG_WR_FULL_FLAG_CLR         (1 << 9)
#define REG_RD_EMPTY_FLAG_CLR        (1 << 8)
#define REG_SEL_TES_BUS              (1 << 6)
#define REG_RD_LR_SWAP_EN            (1 << 5)
#define REG_PRIORITY_KEEP_HIGH       (1 << 4)
#define REG_RD_BYTE_SWAP_EN          (1 << 3)
#define REG_RD_LEVEL_CNT_LIVE_MASK   (1 << 2)
#define REG_ENABLE                   (1 << 1)
#define REG_SW_RST_DMA               (1 << 0)

/**
 * @brief Register 82h, C2h
 */
#define REG_RD_ENABLE                (1 << 15)
#define REG_RD_INIT                  (1 << 14)
#define REG_RD_TRIG                  (1 << 13)
#define REG_RD_LEVEL_CNT_MASK        (1 << 12)

#define REG_RD_BASE_ADDR_LO_POS      0
#define REG_RD_BASE_ADDR_LO_MSK      (0xFFF << REG_RD_BASE_ADDR_LO_POS)

/**
 * @brief Register 84h, C4h
 */
#define REG_RD_BASE_ADDR_HI_OFFSET   12
#define REG_RD_BASE_ADDR_HI_POS      0
#define REG_RD_BASE_ADDR_HI_MSK      (0x7FFF << REG_RD_BASE_ADDR_HI_POS)

/**
 * @brief Register 86h, C6h
 */
#define REG_RD_BUFF_SIZE_POS         0
#define REG_RD_BUFF_SIZE_MSK         (0xFFFF << REG_RD_BUFF_SIZE_POS)

/**
 * @brief Register 88h, C8h
 */
#define REG_RD_SIZE_POS              0
#define REG_RD_SIZE_MSK              (0xFFFF << REG_RD_SIZE_POS)

/**
 * @brief Register 8Ah, CAh
 */
#define REG_RD_OVERRUN_TH_POS        0
#define REG_RD_OVERRUN_TH_MSK        (0xFFFF << REG_RD_OVERRUN_TH_POS)

/**
 * @brief Register 8Ch, CCh
 */
#define REG_RD_UNDERRUN_TH_POS       0
#define REG_RD_UNDERRUN_TH_MSK       (0xFFFF << REG_RD_UNDERRUN_TH_POS)

/**
 * @brief Register 8Eh, CEh
 */
#define REG_RD_LEVEL_CNT_POS         0
#define REG_RD_LEVEL_CNT_MSK         (0xFFFF << REG_RD_LEVEL_CNT_POS)

/**
 * @brief Register 90h, D0h
 */
#define REG_RD_LOCALBUF_EMPTY        (1 << 7)
#define REG_WR_LOCALBUF_FULL         (1 << 6)
#define REG_WR_FULL_FLAG             (1 << 5)
#define REG_RD_EMPTY_FLAG            (1 << 4)
#define REG_RD_OVERRUN_FLAG          (1 << 3)
#define REG_RD_UNDERRUN_FLAG         (1 << 2)
#define REG_WR_OVERRUN_FLAG          (1 << 1)
#define REG_WR_UNDERRUN_FLAG         (1 << 0)

/**
 * @brief Register 92h, D2h
 */
#define REG_WR_ENABLE                (1 << 15)
#define REG_WR_INIT                  (1 << 14)
#define REG_WR_TRIG                  (1 << 13)
#define REG_WR_LEVEL_CNT_MASK        (1 << 12)

#define REG_WR_BASE_ADDR_LO_POS      0
#define REG_WR_BASE_ADDR_LO_MSK      (0xFFF << REG_WR_BASE_ADDR_LO_POS)

/**
 * @brief Register 94h, D4h
 */
#define REG_WR_BASE_ADDR_HI_OFFSET   12
#define REG_WR_BASE_ADDR_HI_POS      0
#define REG_WR_BASE_ADDR_HI_MSK      (0x7FFF << REG_WR_BASE_ADDR_HI_POS)

/**
 * @brief Register 96h, D6h
 */
#define REG_WR_BUFF_SIZE_POS         0
#define REG_WR_BUFF_SIZE_MSK         (0xFFFF << REG_WR_BUFF_SIZE_POS)

/**
 * @brief Register 98h, D8h
 */
#define REG_WR_SIZE_POS              0
#define REG_WR_SIZE_MSK              (0xFFFF << REG_WR_SIZE_POS)

/**
 * @brief Register 9Ah, DAh
 */
#define REG_WR_OVERRUN_TH_POS        0
#define REG_WR_OVERRUN_TH_MSK        (0xFFFF << REG_WR_OVERRUN_TH_POS)

/**
 * @brief Register 9Ch, DCh
 */
#define REG_WR_UNDERRUN_TH_POS       0
#define REG_WR_UNDERRUN_TH_MSK       (0xFFFF << REG_WR_UNDERRUN_TH_POS)

/**
 * @brief Register 9Eh, DEh
 */
#define REG_WR_LEVEL_CNT_POS         0
#define REG_WR_LEVEL_CNT_MSK         (0xFFFF << REG_RD_LEVEL_CNT_POS)

/**
 * @brief Register E0h
 */
#define DMA_TEST_RD_SEL_POS          6
#define DMA_TEST_RD_SEL_MSK          (3 << DMA_TEST_RD_SEL_POS)

#define DMA_TEST_RD                  (1 << 5)
#define DMA_TEST_RD_EN               (1 << 4)

#define DMA_TEST_WR_SEL_POS          2
#define DMA_TEST_WR_SEL_MSK          (3 << DMA_TEST_RD_SEL_POS)

#define DMA_TEST_WR                  (1 << 1)
#define DMA_TEST_WR_EN               (1 << 0)

/**
 * @brief Register E2h
 */
#define DMA_WR_DATA_L_MUX_POS        0
#define DMA_WR_DATA_L_MUX_MSK        (0xFFFF << DMA_WR_DATA_L_MUX_POS)

/**
 * @brief Register E4h
 */
#define DMA_WR_DATA_R_MUX_POS        0
#define DMA_WR_DATA_R_MUX_MSK        (0xFFFF << DMA_WR_DATA_R_MUX_POS)

/**
 * @brief Register E6h
 */
#define DMA_RD_DATA_L_POS        0
#define DMA_RD_DATA_L_MSK        (0xFFFF << DMA_RD_DATA_L_POS)

/**
 * @brief Register E8h
 */
#define DMA_RD_DATA_R_POS        0
#define DMA_RD_DATA_R_MSK        (0xFFFF << DMA_RD_DATA_R_POS)

/**
 * @brief Register EAh, ECh
 */
#define REG_SINE_GEN_EN              (1 << 15)
#define REG_SINE_GEN_L               (1 << 14)
#define REG_SINE_GEN_R               (1 << 13)
#define REG_SINE_GEN_RD_WR           (1 << 12)
#define REG_DMA_TEST_SEL_H           (1 << 11)

#define REG_DMA_TEST_SEL_POS         8
#define REG_DMA_TEST_SEL_MSK         (7 << REG_DMA_TEST_SEL_POS)

#define REG_SINE_GEN_GAIN_POS        4
#define REG_SINE_GEN_GAIN_MSK        (0xF << REG_SINE_GEN_GAIN_POS)

#define REG_SINE_GEN_FREQ_POS        0
#define REG_SINE_GEN_FREQ_MSK        (0xF << REG_SINE_GEN_FREQ_POS)


/**
 * @brief Register EEh
 */
#define REG_FORCE_MREQ_ON                 (1 << 15)
#define DMA2MI_2_SEL                      (1 << 5)
#define DMA2MI_1_SEL                      (1 << 4)
#define REG_DMA2_WR_MODE                  (1 << 3)
#define REG_DMA1_WR_MODE                  (1 << 2)
#define REG_DMA2_WR_FIFO_REWRITE_CLR      (1 << 1)
#define REG_DMA1_WR_FIFO_REWRITE_CLR      (1 << 0)

/**
 * @brief Register F0h,F2h
 */
#define DMA2MI_MOBF_IDX_POS   0
#define DMA2MI_MOBF_IDX_MSK   (0x3F << DMA2MI_MOBF_IDX_POS)


/****************************************************************************/
/*        AUDIO DIGITAL registers bank2                                     */
/****************************************************************************/
/**
 * @brief Register 00h
 */
#define REG_DEC_NUM3_POS                14
#define REG_DEC_NUM3_MSK                (0x3<<REG_DEC_NUM3_POS)

#define REG_DEC_NUM4_POS                12
#define REG_DEC_NUM4_MSK                (0x3<<REG_DEC_NUM4_POS)

#define REG_INT3_MIX_CTRL_POS           10
#define REG_INT3_MIX_CTRL_MSK           (0x3<<REG_INT3_MIX_CTRL_POS)

#define REG_INT4_MIX_CTRL_POS           8
#define REG_INT4_MIX_CTRL_MSK           (0x3<<REG_INT4_MIX_CTRL_POS)

#define REG_ASRC2_INT3_MIX_CTRL_POS     6
#define REG_ASRC2_INT3_MIX_CTRL_MSK     (0x3<<REG_ASRC2_INT3_MIX_CTRL_POS)

#define REG_ASRC2_INT4_MIX_CTRL_POS     4
#define REG_ASRC2_INT4_MIX_CTRL_MSK     (0x3<<REG_ASRC2_INT4_MIX_CTRL_POS)

#define REG_EN_DEC_3_p                  (1 << 3)
#define REG_EN_DEC_4_p                  (1 << 2)
#define REG_EN_ASRC2_DEC_3_p            (1 << 1)
#define REG_EN_ASRC2_DEC_4_p            (1 << 0)

/**
 * @brief Register 02h
 */
#define REG_PGA3_SWAP                      (1 << 15)
#define REG_PGA4_SWAP                      (1 << 14)
#define REG_SDM_SEL3                       (1 << 13)
#define REG_SDM_SEL4                       (1 << 12)

#define REG_DAC_DIN_L_SEL2_POS             10
#define REG_DAC_DIN_L_SEL2_MSK             (0x3<<REG_DAC_DIN_L_SEL2_POS)

#define REG_DAC_DIN_R_SEL2_POS             8
#define REG_DAC_DIN_R_SEL2_MSK             (0x3<<REG_DAC_DIN_R_SEL2_POS)

#define REG_SDM_DWA_DATAIN_L_SEL2_POS      6
#define REG_SDM_DWA_DATAIN_L_SEL2_MSK      (0x3<<REG_SDM_DWA_DATAIN_L_SEL2_POS)

#define REG_SDM_DWA_DATAIN_R_SEL2_POS      4
#define REG_SDM_DWA_DATAIN_R_SEL2_MSK      (0x3<<REG_SDM_DWA_DATAIN_R_SEL2_POS)

#define REG_DEC_125FS_NF_SEL2              (1 << 3)
#define REG_ASRC2_DEC_125FS_NF_SEL         (1 << 2)
#define REG_ASRC2_DEC_125FS_NF_SEL2        (1 << 1)

/**
 * @brief Register 10h
 */
#define REG_SPDIF_OUT_CS1_POS     8
#define REG_SPDIF_OUT_CS1_MSK     (0xFF<<REG_SPDIF_OUT_CS1_POS)

#define REG_SPDIF_OUT_CS0_POS     0
#define REG_SPDIF_OUT_CS0_MSK     (0xFF<<REG_SPDIF_OUT_CS0_POS)

/**
 * @brief Register 12h
 */
#define REG_SPDIF_OUT_CS3_POS     8
#define REG_SPDIF_OUT_CS3_MSK     (0xFF<<REG_SPDIF_OUT_CS3_POS)

#define REG_SPDIF_OUT_CS2_POS     0
#define REG_SPDIF_OUT_CS2_MSK     (0xFF<<REG_SPDIF_OUT_CS2_POS)

/**
 * @brief Register 14h
 */
#define REG_TX_FIFO_EX            (1 << 15)
#define REG_TX_FIFO_CLR           (1 << 14)
#define REG_CNT_RST               (1 << 13)
#define REG_SPDIF_FIX             (1 << 12)
#define REG_SPDIF_FIX_VAL         (1 << 11)
#define REG_SPDIF_CFG             (1 << 10)
#define REG_SPDIF_VALIDITY        (1 << 9)
#define REG_SPDIF_EN_ABS          (1 << 8)

#define REG_SPDIF_OUT_CS4_POS     0
#define REG_SPDIF_OUT_CS4_MSK     (0xFF<<REG_SPDIF_OUT_CS4_POS)

/**
 * @brief Register 16h
 */
#define SPDIF_DATA_L_HI_OFFSET    12
#define SPDIF_DATA_L_HI_POS     4
#define SPDIF_DATA_L_HI_MSK     (0xFFF<<SPDIF_DATA_L_HI_POS)

#define SPDIF_DATA_V          (1 << 3)
#define SPDIN_TX_VALID        (1 << 2)
#define SPDIF_OUT             (1 << 1)
#define TX_FIFO_STATUS        (1 << 0)

/**
 * @brief Register 18h
 */
#define COARSE_K_POS        12
#define COARSE_K_MSK        (0xF<<COARSE_K_POS)

#define PHASE_OFFSET_POS    8
#define PHASE_OFFSET_MSK    (0xF<<PHASE_OFFSET_POS)

#define FDC_KD_REG_POS      4
#define FDC_KD_REG_MSK      (0xF<<FDC_KD_REG_POS)

#define PD_RESOLUTION_POS   1
#define PD_RESOLUTION_MSK   (0x7<<PD_RESOLUTION_POS)

#define ENABLE_CDR          (1<<0)

/**
 * @brief Register 1Ah
 */
#define REG_RX_FIFO_EX                   (1 << 3)
#define REG_RX_FIFO_CLR                  (1 << 2)
#define REG_SPDIF_IN_BLOCK_NO_CHECK      (1 << 1)
#define PHASE_TRACKER                    (1 << 0)

/**
 * @brief Register 1Ch
 */
#define NO_SIGNAL_IN            (1<<15)
#define SPDIF_N_F_STATUS_POS    0
#define SPDIF_N_F_STATUS_MSK    (0x7FFF<<SPDIF_N_F_STATUS_POS)

/**
 * @brief Register 1Eh
 */
#define SPDIN_CS_LO_POS 0
#define SPDIN_CS_LO_MSK (0xFFFF<<SPDIN_CS_LO_POS)

/**
 * @brief Register 20h
 */
#define SPDIN_CS_HI_OFFSET  16
#define SPDIN_CS_HI_POS     0
#define SPDIN_CS_HI_MSK     (0xFFFF<<SPDIN_CS_HI_POS)

/**
 * @brief Register 22h
 */
#define SPDIN_CS_HIHI_OFFSET    32
#define SPDIN_CS_HIHI_POS       8
#define SPDIN_CS_HIHI_MSK       (0xFF<<SPDIN_CS_HI_POS)

#define SPDIN_LDATA23         (1 << 7)
#define RX_FIFO_STATUS        (1 << 6)
#define SPDIN_WE              (1 << 5)
#define COARSE_NORMAL_STATUS  (1 << 4)
#define CRT_SPDIF_RX_256FS    (1 << 3)
#define CRT_SPDIF_RX_128FS    (1 << 2)
#define RECOVERY_DATA         (1 << 1)
#define SPDIN_ERROR           (1 << 0)

/**
 * @brief Register 24h
 */
#define REG_DBB_PCM_MODE                 (1 << 15)
#define REG_DBB_PCM_SYNC_SEL             (1 << 14)
#define REG_DBB_I2S_FMT                  (1 << 13)
#define REG_DBB_MS_MODE                  (1 << 12)

#define REG_DBB_I2S_WDTH_POS             10
#define REG_DBB_I2S_WDTH_MSK             (0x3<<REG_DBB_I2S_WDTH_POS)

#define REG_DBB_I2S_BCK_GEN_DIV2         (1 << 9)

#define REG_DBB_I2S_BCK_GEN_SEL_POS      7
#define REG_DBB_I2S_BCK_GEN_SEL_MSK      (0x3<<REG_DBB_I2S_BCK_GEN_SEL_POS)

#define REG_DBB_I2S_BCK_GEN_UPDATE       (1 << 6)
#define REG_DBB_I2S_TRX_BCK_INV          (1 << 5)
#define REG_DBB_RX_FIFO_STATUS_CLR       (1 << 4)
#define REG_DBB_RX_FIFO_EXTENSION        (1 << 3)
#define REG_DBB_TX_FIFO_STATUS_CLR       (1 << 2)
#define REG_DBB_TX_FIFO_EXTENSION        (1 << 1)
#define REG_DBB_I2S_LOOPBACK_EN          (1 << 0)

/**
 * @brief Register 26h
 */
#define REG_CODEC_PCM_MODE                 (1 << 15)
#define REG_CODEC_PCM_SYNC_SEL             (1 << 14)
#define REG_CODEC_I2S_FMT                  (1 << 13)
#define REG_CODEC_MS_MODE                  (1 << 12)

#define REG_CODEC_I2S_WDTH_POS             10
#define REG_CODEC_I2S_WDTH_MSK             (0x3<<REG_CODEC_I2S_WDTH_POS)

#define REG_CODEC_I2S_BCK_GEN_DIV2         (1 << 9)

#define REG_CODEC_I2S_BCK_GEN_SEL_POS      7
#define REG_CODEC_I2S_BCK_GEN_SEL_MSK      (0x3<<REG_CODEC_I2S_BCK_GEN_SEL_POS)

#define REG_CODEC_I2S_BCK_GEN_UPDATE       (1 << 6)
#define REG_CODEC_I2S_TRX_BCK_INV          (1 << 5)
#define REG_CODEC_TX_FIFO_STATUS_CLR       (1 << 4)
#define REG_CODEC_TX_FIFO_EXTENSION        (1 << 3)
#define REG_CODEC_FS_SEL                   (1 << 2)

/**
 * @brief Register 28h
 */
#define REG_EXT_I2S_FMT                  (1 << 13)
#define REG_EXT_MS_MODE                  (1 << 12)

#define REG_EXT_I2S_WDTH_POS             10
#define REG_EXT_I2S_WDTH_MSK             (0x3<<REG_EXT_I2S_WDTH_POS)

#define REG_EXT_I2S_BCK_GEN_DIV2         (1 << 9)

#define REG_EXT_I2S_BCK_GEN_SEL_POS      7
#define REG_EXT_I2S_BCK_GEN_SEL_MSK      (0x3<<REG_EXT_I2S_BCK_GEN_SEL_POS)

#define REG_EXT_I2S_BCK_GEN_UPDATE       (1 << 6)
#define REG_EXT_I2S_TRX_BCK_INV          (1 << 5)
#define REG_EXT_RX_FIFO_STATUS_CLR       (1 << 4)
#define REG_EXT_RX_FIFO_EXTENSION        (1 << 3)
#define REG_EXT_FS_SEL                   (1 << 2)
#define REG_EXT_I2S_LOOPBACK_EN          (1 << 1)

/**
 * @brief Register 2Ah
 */
#define REG_PAD_EXT_I2S_RX_SEL_POS        14
#define REG_PAD_EXT_I2S_RX_SEL_MSK        (0x3<<REG_PAD_EXT_I2S_RX_SEL_POS)

#define REG_PAD_CODEC_I2S_TX_S_SEL_POS    12
#define REG_PAD_CODEC_I2S_TX_S_SEL_MSK    (0x3<<REG_PAD_CODEC_I2S_TX_S_SEL_POS)

#define REG_PAD_CODEC_I2S_TX_M_SEL_POS    10
#define REG_PAD_CODEC_I2S_TX_M_SEL_MSK    (0x3<<REG_PAD_CODEC_I2S_TX_M_SEL_POS)

#define REG_PAD_BT_I2S_TRX_S_SEL_POS      8
#define REG_PAD_BT_I2S_TRX_S_SEL_MSK      (0x3<<REG_PAD_BT_I2S_TRX_S_SEL_POS)

#define REG_PAD_BT_I2S_TRX_M_SEL_POS      6
#define REG_PAD_BT_I2S_TRX_M_SEL_MSK      (0x3<<REG_PAD_BT_I2S_TRX_M_SEL_POS)

#define REG_PAD_DBB_I2S_TRX_S_SEL_POS     4
#define REG_PAD_DBB_I2S_TRX_S_SEL_MSK     (0x3<<REG_PAD_DBB_I2S_TRX_S_SEL_POS)

#define REG_PAD_SPDIF_IN_SEL_POS          1
#define REG_PAD_SPDIF_IN_SEL_MSK          (0x7<<REG_PAD_SPDIF_IN_SEL_POS)

/**
 * @brief Register 2Ch
 */
#define REG_CODEC_I2S_TRX_WCK_OEN      (1 << 15)
#define REG_CODEC_I2S_TRX_BCK_OEN      (1 << 14)
#define REG_CODEC_I2S_TRX_SDI_OEN      (1 << 13)
#define REG_CODEC_I2S_TRX_SDO_OEN      (1 << 12)
#define REG_EXT_I2S_RX_WCK_OEN         (1 << 11)
#define REG_EXT_I2S_RX_BCK_OEN         (1 << 10)
#define REG_EXT_I2S_RX_SDI_OEN         (1 << 9)
#define REG_DMA2_RD_VALID_SEL          (1 << 8)
#define REG_DMA2_WR_VALID_SEL          (1 << 7)
#define REG_DMA2_RD_VALID_POS          8
#define REG_DMA2_WR_VALID_POS          7

#define REG_DMA2_WR_MUX_SEL_POS        5
#define REG_DMA2_WR_MUX_SEL_MSK        (0x3 << REG_DMA2_WR_MUX_SEL_POS)

#define REG_CH1_MUX_SEL2               (1 << 4)
#define REG_SPDIF_TX_SOURCE_SEL        (1 << 3)
#define REG_SPDIF_TX_SDO_OEN           (1 << 2)
#define REG_SPDIF_RX_FS_SEL            (1 << 1)

/**
 * @brief Register 2Eh
 */
#define REG_DBB_I2S_IN_WDTH_POS     14
#define REG_DBB_I2S_IN_WDTH_MSK     (0x3<<REG_DBB_I2S_IN_WDTH_POS)

#define REG_DBB_I2S_OUT_WDTH_POS    12
#define REG_DBB_I2S_OUT_WDTH_MSK    (0x3<<REG_DBB_I2S_OUT_WDTH_POS)

#define REG_DBB_RX_FIFO_STATUS      (1 << 11)
#define REG_DBB_TX_FIFO_STATUS      (1 << 10)

#define REG_CODEC_I2S_IN_WDTH_POS   8
#define REG_CODEC_I2S_IN_WDTH_MSK   (0x3<<REG_CODEC_I2S_IN_WDTH_POS)

#define REG_CODEC_I2S_OUT_WDTH_POS  6
#define REG_CODEC_I2S_OUT_WDTH_MSK  (0x3<<REG_CODEC_I2S_OUT_WDTH_POS)

#define REG_CODEC_TX_FIFO_STATUS    (1 << 5)
#define REG_EXT_RX_FIFO_STATUS      (1 << 4)

/**
 * @brief Register 30h
 */
#define REG_DIG_MIC_EN                      (1 << 15)
#define REG_DIG_MIC_BCK_INV                 (1 << 14)
#define REG_DIG_MIC_BCK_SEL                 (1 << 13)
#define REG_PAD_CODEC_I2S_TX_SDI_M_SEL      (1 << 12)
#define REG_PAD_BT_I2S_TRX_SDI_M_SEL        (1 << 11)
#define REG_PAD_DIG_MIC_BCK_OEN             (1 << 10)

#define REG_PAD_DIG_MIC_SDI_OEN_POS         8
#define REG_PAD_DIG_MIC_SDI_OEN_MSK         (0x3 << REG_PAD_DIG_MIC_SDI_OEN_POS)

#define REG_PNT_MMC_DEC_ERR_CLR             (1 << 5)
#define REG_PNT_ASRC2_MMC_DEC_ERR_CLR       (1 << 4)
#define REG_PNT_MMC1_DEC_SYNC_ENZ           (1 << 3)
#define REG_PNT_MMC3_DEC_SYNC_ENZ           (1 << 2)
#define REG_PNT_MMC2_DEC_SYNC_ENZ           (1 << 1)
#define REG_PNT_MMC4_DEC_SYNC_ENZ           (1 << 0)

/**
 * @brief Register 32h
 */
#define REG_DIG_MIC_SEL1                     (1 << 15)//DEC1_2
#define REG_DIG_MIC_SEL2                     (1 << 7) //ASRC2_DEC1_2


/**
 * @brief Register 34h
 */
#define REG_DIG_MIC_SEL3                     (1 << 15)//DEC3_4
#define REG_DIG_MIC_SEL4                     (1 << 7) //ASRC2_DEC3_4



/****************************************************************************/
/*        AUDIO ANALOG registers bank3                                     */
/****************************************************************************/
/**
 * @brief Register 06h
 */
#define EN_CLK_TST                  (1 << 15)
#define EN_AUDIO_IBIAS              (1 << 14)

#define IBSEL_AUDIO_POS             12
#define IBSEL_AUDIO_MSK             (0x3 << IBSEL_AUDIO_POS)

#define EN_POLLING_DRV              (1 << 11)
#define EN_REF_NO_BG                (1 << 10)
#define REG_SEL_CONTROL             (1 << 9)
#define REG_CONTROL                 (1 << 8)
#define EN_MICDET                   (1 << 7)
#define HST_EN                      (1 << 6)
#define RCV_EN                      (1 << 5)

#define MICV_SEL_POS                3
#define MICV_SEL_MSK                (0x3 << MICV_SEL_POS)

#define EN_MICBIAS                  (1 << 2)

#define SEL_IREFDET_POS             0
#define SEL_IREFDET_MSK             (0x3 << SEL_IREFDET_POS)

/**
 * @brief Register 08h
 */
#define SEL_IREFGND_POS             14
#define SEL_IREFGND_MSK             (0x3 << SEL_IREFGND_POS)

#define EN_MICTRIM                  (1 << 13)
#define EN_MICDET_LP                (1 << 12)
#define PULLUP_HSIP                 (1 << 11)
#define SEL_VCMREF0                 (1 << 10)
#define EN_VCMBUF0                  (1 << 9)
#define EN_IBIAS_PGA0               (1 << 8)

#define IBIAS_PGA0_POS              6
#define IBIAS_PGA0_MSK              0x3 << IBIAS_PGA0_POS)

#define EN_PGA0_MICA1               (1 << 5)
#define PGA0_MICA1_GAIN             (1 << 4)
#define EN_PGA0_MICA2               (1 << 3)

#define PGA0_MICA2_GAIN_POS         0
#define PGA0_MICA2_GAIN_MSK         (0x7 << PGA0_MICA2_GAIN_POS)

/**
 * @brief Register 0Ah
 */
#define PGA0_MICA2_SEL              (1 << 15)
#define PGA0_MUTE_L                 (1 << 14)
#define EN_PGA0_MICA3               (1 << 13)
#define PGA0_MICA3_GAIN             (1 << 12)
#define EN_PGA0_MICA4               (1 << 11)

#define PGA0_MICA4_GAIN_POS         8
#define PGA0_MICA4_GAIN_MSK         (0x7 << PGA0_MICA4_GAIN_POS)

#define PGA0_MICA4_SEL              (1 << 7)
#define PGA0_MUTE_R                 (1 << 6)
#define PGA0_LINE_SEL               (1 << 5)
#define SEL_VCMREF1                 (1 << 4)
#define EN_VCMBUF1                  (1 << 3)
#define EN_IBIAS_PGA1               (1 << 2)

#define IBIAS_PGA1_POS              0
#define IBIAS_PGA1_MSK              (0x3 << IBIAS_PGA1_POS)

/**
 * @brief Register 0Ch
 */
#define EN_PGA1_MICA2               (1 << 15)

#define PGA1_MICA2_GAIN_POS         12
#define PGA1_MICA2_GAIN_MSK         (0x7 << PGA1_MICA2_GAIN_POS)

#define PGA1_MUTE_L                 (1 << 11)
#define EN_PGA1_MICA4               (1 << 10)

#define PGA1_MICA4_GAIN_POS         7
#define PGA1_MICA4_GAIN_MSK         (0x7 << PGA1_MICA4_GAIN_POS)
#define PGA1_MUTE_R                 (1 << 6)
#define PGA1_LINE_SEL               (1 << 5)
#define POS_RL0                     (1 << 4)
#define LDO11_VC0                   (1 << 3)
#define EN_DAC0_LDO11               (1 << 2)
#define EN_DAC0_L                   (1 << 1)
#define EN_DAC0_R                   (1 << 0)

/**
 * @brief Register 0Eh
 */
#define POS_RL1                     (1 << 15)
#define LDO11_VC1                   (1 << 14)
#define EN_DAC1_LDO11               (1 << 13)
#define EN_DAC1_L                   (1 << 12)
#define EN_DAC1_R                   (1 << 11)
#define EN_ADC0_DIT                 (1 << 10)

#define SEL_DIT_LVL_ADC0_POS        8
#define SEL_DIT_LVL_ADC0_MSK        (0x3 << SEL_DIT_LVL_ADC0_POS)

#define SEL_IBIAS_ADC0_POS          6
#define SEL_IBIAS_ADC0_MSK          (0x3 << SEL_IBIAS_ADC0_POS)
#define RESET_ADC0_R                (1 << 5)
#define RESET_ADC0_L                (1 << 4)
#define SHRT_ADC0_R                 (1 << 3)
#define SHRT_ADC0_L                 (1 << 2)
#define EN_ADC0                     (1 << 1)
#define EN_ADC1_DIT                 (1 << 0)

/**
 * @brief Register 10h
 */
#define SEL_DIT_LVL_ADC1_POS        14
#define SEL_DIT_LVL_ADC1_MSK        (0x3 << SEL_DIT_LVL_ADC1_POS)

#define SEL_IBIAS_ADC1_POS          12
#define SEL_IBIAS_ADC1_MSK          (0x3 << SEL_IBIAS_ADC1_POS)

#define RESET_ADC1_R                (1 << 11)
#define RESET_ADC1_L                (1 << 10)
#define SHRT_ADC1_R                 (1 << 9)
#define SHRT_ADC1_L                 (1 << 8)
#define EN_ADC1                     (1 << 7)
#define EN_EARR                     (1 << 6)
#define EN_EARL                     (1 << 5)
#define EN_OPLP                     (1 << 4)
#define EN_STG2AB                   (1 << 3)

#define GAIN_EAR_POS                1
#define GAIN_EAR_MSK                (0x3 << GAIN_EAR_POS)

/**
 * @brief Register 12h
 */
#define TCSEL_POS                   14
#define TCSEL_MSK                   (0x3 << TCSEL_POS)

#define EAR_POPRES_POS              12
#define EAR_POPRES_MSK              (0x3 << EAR_POPRES_POS)

#define EAR_MUTE                    (1 << 11)

#define TST_DRV_POS                 7
#define TST_DRV_MSK                 (0xF << TST_DRV_POS)

#define ISEL_DRV_POS                5
#define ISEL_DRV_MSK                (0x3 << ISEL_DRV_POS)

#define ISEL_OCP_POS                3
#define ISEL_OCP_MSK                (0x3 << ISEL_OCP_POS)

#define MX_EAR_POS                  0
#define MX_EAR_MSK                  (0x7 << MX_EAR_POS)

/**
 * @brief Register 14h
 */
#define EN_LINE0_L                  (1 << 15)
#define EN_LINE0_R                  (1 << 14)

#define GAIN_LINE0_POS              11
#define GAIN_LINE0_MSK              (0x7 << GAIN_LINE0_POS)

#define MX_LINE0_POS                9
#define MX_LINE0_MSK                (0x3 << MX_LINE0_POS)

#define MUTE_LINE0_POS              8
#define MUTE_LINE0                  (1 << 8)
#define EN_LINE1_L                  (1 << 7)
#define EN_LINE1_R                  (1 << 6)

#define GAIN_LINE1_POS              3
#define GAIN_LINE1_MSK              (0x7 << GAIN_LINE1_POS)

#define MX_LINE1_POS                1
#define MX_LINE1_MSK                (0x3 << MX_LINE1_POS)

#define MUTE_LINE1_POS              0
#define MUTE_LINE1                  (1 << 0)

/**
 * @brief Register 16h
 */
#define EN_LDO25                    (1 << 15)

#define V_SET_LDO25_POS             13
#define V_SET_LDO25_MSK             (0x3 << V_SET_LDO25_POS)

#define EN_CLK                  (1 << 12)
#define DAC_CLK_FREQ                (1 << 11)
#define ADC_CLK_FREQ                (1 << 10)
#define SEL_PHS_ADCCLK              (1 << 9)

#define SEL_CK_AUDIO_POS            7
#define SEL_CK_AUDIO_MSK            (0x3 << SEL_CK_AUDIO_POS)

#define TST_AUDIO_POS               3
#define TST_AUDIO_MSK               (0xF << TST_AUDIO_POS)

/**
 * @brief Register 18h
 */
#define AUDIO_REG_POS               0
#define AUDIO_REG_MSK               (0xFF << AUDIO_REG_POS)

/**
 * @brief Register 20h
 */
#define TRIM_AU_OK                  (1<<7)


/****************************************************************************/
/*        AUDIO DIGITAL registers bank4                                     */
/****************************************************************************/
/**
 * @brief Register 40h,44h,48h,4Ch,50h,54h,58h,5Ch,60h,62h,64h,68h,6Ch,70h,74h,78h,7Ch,80h,82h,84h,88h
 */
#define REG_OFFSET_POS    6
#define REG_OFFSET_MSK    (0x1F<<REG_OFFSET_POS)

#define STEP_POS      3
#define STEP_MSK      (0x7<<STEP_POS)

#define MUTE_2_ZERO   (1 << 2)
#define FADING_EN     (1 << 1)
#define DPGA_EN       (1 << 0)

/**
 * @brief Register 42h,46h,4Ah,4Eh,52h,56h,5Ah,5Eh,62h,66h,6Ah,6Eh,72h,76h,7Ah,7Eh,82h,86h,8Ah
 */
#define REG_GAIN_TRIG   (1<<8)
#define REG_GAIN_POS    0
#define REG_GAIN_MSK    (0xFF<<REG_GAIN_POS)

/**
 * @brief Register 8Ch
 */
#define REG_PGA1_EN          (1 << 15)
#define REG_PGA1_MUTE        (1 << 14)
#define REG_PGA1_GAIN_TRIG   (1 << 13)
#define REG_PGA1_GAIN_POS    8
#define REG_PGA1_GAIN_MSK    (0x1F<<REG_PGA1_GAIN_POS)
#define REG_PGA2_EN          (1 << 7)
#define REG_PGA2_MUTE        (1 << 6)
#define REG_PGA2_GAIN_TRIG   (1 << 5)
#define REG_PGA2_GAIN_POS    0
#define REG_PGA2_GAIN_MSK    (0x1F<<REG_PGA2_GAIN_POS)

/**
 * @brief Register 8Eh
 */
#define REG_PGA3_EN          (1 << 15)
#define REG_PGA3_MUTE        (1 << 14)
#define REG_PGA3_GAIN_TRIG   (1 << 13)
#define REG_PGA3_GAIN_POS    8
#define REG_PGA3_GAIN_MSK    (0x1F<<REG_PGA3_GAIN_POS)
#define REG_PGA4_EN          (1 << 7)
#define REG_PGA4_MUTE        (1 << 6)
#define REG_PGA4_GAIN_TRIG   (1 << 5)
#define REG_PGA4_GAIN_POS    0
#define REG_PGA4_GAIN_MSK    (0x1F<<REG_PGA4_GAIN_POS)

/**
 * @brief Register 90h
 */
#define DPGA_MUTE_DONE_INTERRUPT_MASK   (1 << 15)
#define DPGA_STATUS_CLEAR               (1 << 14)
#define PGA_STATUS_CLEAR                (1 << 13)
#define MMP1_MUTE_DONE_BYPASS              		(1 << 9)
#define MMP2_MUTE_DONE_BYPASS              		(1 << 8)
#define DEC1_1_MUTE_DONE_BYPASS            		(1 << 7)
#define DEC1_2_MUTE_DONE_BYPASS            		(1 << 6)
#define DEC1_3_MUTE_DONE_BYPASS            		(1 << 5)
#define DBB_1_MUTE_DONE_BYPASS             		(1 << 4)
#define DBB_2_MUTE_DONE_BYPASS             		(1 << 3)
#define DMARD_L_MUTE_DONE_BYPASS           		(1 << 2)
#define DMARD_R_MUTE_DONE_BYPASS           		(1 << 1)
#define SPARE_MUTE_DONE_BYPASS             		(1 << 0)

/**
 * @brief Register 94h
 */
#define ASRC2_MMP1_MUTE_DONE      (1 << 10)
#define ASRC2_MMP2_MUTE_DONE      (1 << 9)
#define DMARD_R_MUTE_DONE         (1 << 8)
#define DMARD_L_MUTE_DONE         (1 << 7)
#define DBB_2_MUTE_DONE           (1 << 6)
#define DBB_1_MUTE_DONE           (1 << 5)
#define DEC1_3_MUTE_DONE          (1 << 4)
#define DEC1_2_MUTE_DONE          (1 << 3)
#define DEC1_1_MUTE_DONE          (1 << 2)
#define MMP2_MUTE_DONE            (1 << 1)
#define MMP1_MUTE_DONE            (1 << 0)

/**
 * @brief Register 96h
 */
#define DMAWR4_MUTE_DONE    (1 << 7)
#define DMAWR3_MUTE_DONE    (1 << 6)
#define DMAWR2_MUTE_DONE    (1 << 5)
#define DMAWR1_MUTE_DONE    (1 << 4)
#define SPIDF2_MUTE_DONE    (1 << 3)
#define SPIDF1_MUTE_DONE    (1 << 2)
#define CODEC2_MUTE_DONE    (1 << 1)
#define CODEC1_MUTE_DONE    (1 << 0)

/**
 * @brief Register 98h
 */
#define PGA_STATUS_POS			14
#define PGA_STATUS_MSK			(0x3<<PGA_STATUS_POS)
#define DPGA_STATUS_POS			0
#define DPGA_STATUS_MSK			(0x3FFF<<DPGA_STATUS_POS)

#endif
