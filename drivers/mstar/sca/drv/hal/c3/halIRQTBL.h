////////////////////////////////////////////////////////////////////////////////

//

// Copyright (c) 2008-2009 MStar Semiconductor, Inc.

// All rights reserved.

//

// Unless otherwise stipulated in writing, any and all information contained

// herein regardless in any format shall remain the sole proprietary of

// MStar Semiconductor Inc. and be kept in strict confidence

// ("MStar Confidential Information") by the recipient.

// Any unauthorized act including without limitation unauthorized disclosure,

// copying, use, reproduction, sale, distribution, modification, disassembling,

// reverse engineering and compiling of the contents of MStar Confidential

// Information is unlawful and strictly prohibited. MStar hereby reserves the

// rights to any and all damages, losses, costs and expenses resulting therefrom.

//

////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////

///

/// file   MsIRQ.h

/// @brief  MStar IRQ

/// @author MStar Semiconductor Inc.

///////////////////////////////////////////////////////////////////////////////////////////////////





#ifndef _HAL_IRQTBL_H_

#define _HAL_IRQTBL_H_







#ifdef __cplusplus

extern "C"

{

#endif





//#if defined (MSOS_TYPE_LINUX)

typedef enum

{

    // IRQ

    E_IRQL_START                = 64,

    E_IRQ_UART0                 = E_IRQL_START  + 0,

    E_IRQ_PMSLEEP               = E_IRQL_START  + 1,  // T12

    E_IRQ_ONIF                  = E_IRQL_START  + 2,  // T8

    E_IRQ_MVD                   = E_IRQL_START  + 3,

    E_IRQ_PS                    = E_IRQL_START  + 4,

    E_IRQ_NFIE                  = E_IRQL_START  + 5,

    E_IRQ_USB                   = E_IRQL_START  + 6,

    E_IRQ_UHC                   = E_IRQL_START  + 7,

    E_IRQ_EC_BRIDGE             = E_IRQL_START  + 8,

    E_IRQ_EMAC                  = E_IRQL_START  + 9,

    E_IRQ_DISP                  = E_IRQL_START  + 10,

    E_IRQ_MIIC_DMA_INT3         = E_IRQL_START  + 12, // A5

    E_IRQ_MIIC_INT3             = E_IRQL_START  + 13, // A5

    E_IRQ_COMB                  = E_IRQL_START  + 14,

    E_IRQL_END                  = 79,



    E_IRQH_START                = 80,

    E_IRQ_TSP2HK                = E_IRQH_START  + 0,

    E_IRQ_VE                    = E_IRQH_START  + 1,

    E_IRQ_CIMAX2MCU             = E_IRQH_START  + 2,

    E_IRQ_DC                    = E_IRQH_START  + 3,

    E_IRQ_GOP                   = E_IRQH_START  + 4,

    E_IRQ_PCM                   = E_IRQH_START  + 5,

    E_IRQ_SMART                 = E_IRQH_START  + 7,  // A5

    E_IRQ_VP6                   = E_IRQH_START  + 9,  // A3

    E_IRQ_DDC2BI                = E_IRQH_START  + 10,

    E_IRQ_SCM                   = E_IRQH_START  + 11,

    E_IRQ_VBI                   = E_IRQH_START  + 12,

    E_IRQ_MVD2MIPS              = E_IRQH_START  + 13, // A5

    E_IRQ_GPD                   = E_IRQH_START  + 14, // A5

    E_IRQ_ADCDVI2RIU            = E_IRQH_START  + 15,

    E_IRQH_END                  = 95,



    // FIQ

    E_FIQL_START                = 0,

    E_FIQ_EXTIMER0              = E_FIQL_START +  0,

    E_FIQ_EXTIMER1              = E_FIQL_START +  1,

    E_FIQ_WDT                   = E_FIQL_START +  2,

    E_FIQ_STRETCH               = E_FIQL_START +  3,  // T12

    E_FIQ_AEON_MB2_MCU0         = E_FIQL_START +  4,  // T8

    E_FIQ_AEON_MB2_MCU1         = E_FIQL_START +  5,  // T8

    E_FIQ_DSP2_MB2_MCU0         = E_FIQL_START +  6,  // T8

    E_FIQ_DSP2_MB2_MCU1         = E_FIQL_START +  7,  // T8

    E_FIQ_USB                   = E_FIQL_START +  8,  // T8

    E_FIQ_UHC                   = E_FIQL_START +  9,  // T8

    E_FIQ_VP6                   = E_FIQL_START + 10,  // A3

    E_FIQ_HDMI_NON_PCM          = E_FIQL_START + 11,

    E_FIQ_SPDIF_IN_NON_PCM      = E_FIQL_START + 12,

    E_FIQ_EMAC                  = E_FIQL_START + 13,

    E_FIQ_SE_DSP2UP             = E_FIQL_START + 14,

    E_FIQ_TSP2AEON              = E_FIQL_START + 15,

    E_FIQL_END                  = 15,



    E_FIQH_START                = 16,

    E_FIQ_VIVALDI_STR           = E_FIQH_START +  0,

    E_FIQ_VIVALDI_PTS           = E_FIQH_START +  1,

    E_FIQ_DSP_MIU_PROT          = E_FIQH_START +  2,

    E_FIQ_XIU_TIMEOUT           = E_FIQH_START +  3,

    E_FIQ_DMDMCU2HK             = E_FIQH_START +  4,  // T8

    E_FIQ_VSYNC_VE4VBI          = E_FIQH_START +  5,

    E_FIQ_FIELD_VE4VBI          = E_FIQH_START +  6,

    E_FIQ_VDMCU2HK              = E_FIQH_START +  7,

    E_FIQ_VE_DONE_TT            = E_FIQH_START +  8,

    E_FIQ_IR                    = E_FIQH_START + 11,

    E_FIQ_AFEC_VSYNC            = E_FIQH_START + 12,

    E_FIQ_DEC_DSP2UP            = E_FIQH_START + 13,

    E_FIQ_DSP2MIPS              = E_FIQH_START + 15,

    E_FIQH_END                  = 31,



    //IRQEXP

    E_IRQEXPL_START             = 96,

    E_IRQEXPL_HVD               = E_IRQEXPL_START+0,

    E_IRQEXPL_USB1              = E_IRQEXPL_START+1,

    E_IRQEXPL_UHC1              = E_IRQEXPL_START+2,

    E_IRQEXPL_MIU               = E_IRQEXPL_START+3,

    E_IRQEXPL_USB2              = E_IRQEXPL_START+4,

    E_IRQEXPL_UHC2              = E_IRQEXPL_START+5,

    E_IRQEXPL_AEON2HI           = E_IRQEXPL_START+6,

    E_IRQEXPL_UART1             = E_IRQEXPL_START+7,

    E_IRQEXPL_UART2             = E_IRQEXPL_START+8,

    E_IRQEXPL_MPIF              = E_IRQEXPL_START+10,

    E_IRQEXPL_MIIC_DMA_INT2     = E_IRQEXPL_START+11, // A5

    E_IRQEXPL_MIIC_INT2         = E_IRQEXPL_START+12, // A5

    E_IRQEXPL_JPD               = E_IRQEXPL_START+13,

    E_IRQEXPL_DISPI             = E_IRQEXPL_START+14,

    E_IRQEXPL_MFE               = E_IRQEXPL_START+15, // T8

    E_IRQEXPL_END               = 111,



    E_IRQEXPH_START             = 112,

    E_IRQEXPH_BDMA0             = E_IRQEXPH_START+0,

    E_IRQEXPH_BDMA1             = E_IRQEXPH_START+1,

    E_IRQEXPH_UART2MCU          = E_IRQEXPH_START+2,

    E_IRQEXPH_URDMA2MCU         = E_IRQEXPH_START+3,

    E_IRQEXPH_DVI_HDMI_HDCP     = E_IRQEXPH_START+4,

    E_IRQEXPH_G3D2MCU           = E_IRQEXPH_START+5,

    E_IRQEXPH_CEC               = E_IRQEXPH_START+6, // T8

    E_IRQEXPH_HDCP_IIC          = E_IRQEXPH_START+7, // T8

    E_IRQEXPH_HDCP_X74          = E_IRQEXPH_START+8, // T8

    E_IRQEXPH_WADR_ERR          = E_IRQEXPH_START+9, // T8

    E_IRQEXPH_DCSUB             = E_IRQEXPH_START+10, // T8

    E_IRQEXPH_GE                = E_IRQEXPH_START+11, // T8

    E_IRQEXPH_MIIC_DMA_INT1     = E_IRQEXPH_START+12, // A5

    E_IRQEXPH_MIIC_INT1         = E_IRQEXPH_START+13, // A5

    E_IRQEXPH_MIIC_DMA_INT0     = E_IRQEXPH_START+14, // A5

    E_IRQEXPH_MIIC_INT0         = E_IRQEXPH_START+15, // A5

    E_IRQEXPH_END               = 127,



    // FIQEXP

    E_FIQEXPL_START                     = 32,

    E_FIQEXPL_IR_INT_RC                 = E_FIQEXPL_START+0,

    E_FIQEXPL_AU_DMA_BUF_INT            = E_FIQEXPL_START+1,

    E_FIQEXPL_IR_IN                     = E_FIQEXPL_START+2,

    E_FIQEXPL_8051_TO_MIPS_VPE0         = E_FIQEXPL_START+6, // MBX (8051->MIPS)

    E_FIQEXPL_GPIO0                     = E_FIQEXPL_START+7, // T12

    E_FIQEXPL_MIPS_VPE0_TO_8051         = E_FIQEXPL_START+10,// MBX (MIPS-->8051)

    E_FIQEXPL_GPIO1                     = E_FIQEXPL_START+11,

    E_FIQEXPL_GPIO2                     = E_FIQEXPL_START+15, // T12

    E_FIQEXPL_END                       = 47,



    E_FIQEXPH_START                     = 48,

    E_FIQEXPH_USB1                      = E_FIQEXPH_START +  3,

    E_FIQEXPH_UHC1                      = E_FIQEXPH_START +  4,

    E_FIQEXPH_USB2                      = E_FIQEXPH_START +  5,

    E_FIQEXPH_UHC2                      = E_FIQEXPH_START +  6,

    E_FIQEXPH_GPIO3                     = E_FIQEXPH_START +  7, // T12

    E_FIQEXPH_GPIO4                     = E_FIQEXPH_START +  8, // T12

    E_FIQEXPH_GPIO5                     = E_FIQEXPH_START +  9, // T12

    E_FIQEXPH_GPIO6                     = E_FIQEXPH_START + 10, // T12

    E_FIQEXPH_PWM_RP_L                  = E_FIQEXPH_START + 11, // T12

    E_FIQEXPH_PWM_FP_L                  = E_FIQEXPH_START + 12, // T12

    E_FIQEXPH_PWM_RP_R                  = E_FIQEXPH_START + 13, // T12

    E_FIQEXPH_PWM_FP_R                  = E_FIQEXPH_START + 14, // T12

    E_FIQEXPH_GPIO7                     = E_FIQEXPH_START + 15, // T12

    E_FIQEXPH_END                       = 63,



    //For Mailbox

    E_FIQ_8051_TO_BEON                  = E_FIQEXPL_8051_TO_MIPS_VPE0,

    E_FIQ_BEON_TO_8051                  = E_FIQEXPL_MIPS_VPE0_TO_8051,

    E_FIQ_AEON_TO_BEON                  = E_FIQEXPL_MIPS_VPE0_TO_8051,



    E_IRQ_FIQ_NONE              = 0xFE,

    E_IRQ_FIQ_ALL               = 0xFF //all IRQs & FIQs



} IRQFIQNum;





static MS_U8 IntEnum2HWIdx[256] =

{

    /* 0x00 */

    /* E_INT_IRQ_UART0 */                   E_IRQ_UART0,

    /* E_INT_IRQ_BDMA_CH0 */                E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_BDMA_CH1 */                E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_MVD */                     E_IRQ_MVD,

    /* E_INT_IRQ_PS */                      E_IRQ_PS,

    /* E_INT_IRQ_NFIE */                    E_IRQ_NFIE,

    /* E_INT_IRQ_USB */                     E_IRQ_USB,

    /* E_INT_IRQ_UHC */                     E_IRQ_UHC,

    /* E_INT_IRQ_EC_BRIDGE */               E_IRQ_EC_BRIDGE,

    /* E_INT_IRQ_EMAC */                    E_IRQ_EMAC,

    /* E_INT_IRQ_DISP */                    E_IRQ_DISP,

    /* E_INT_IRQ_DHC */                     E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_PMSLEEP */                 E_IRQ_PMSLEEP,

    /* E_INT_IRQ_SBM */                     E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_COMB */                    E_IRQ_COMB,

    /* E_INT_IRQ_ECC_DERR */                E_IRQ_FIQ_NONE,



    /* 0x10 */

    /* E_INT_IRQ_TSP2HK */                  E_IRQ_TSP2HK,

    /* E_INT_IRQ_VE */                      E_IRQ_VE,

    /* E_INT_IRQ_CIMAX2MCU */               E_IRQ_CIMAX2MCU,

    /* E_INT_IRQ_DC */                      E_IRQ_DC,

    /* E_INT_IRQ_GOP */                     E_IRQ_GOP,

    /* E_INT_IRQ_PCM */                     E_IRQ_PCM,

    /* E_INT_IRQ_IIC0 */                    E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_RTC */                     E_IRQ_SMART,

    /* E_INT_IRQ_KEYPAD */                  E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_PM */                      E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_DDC2BI */                  E_IRQ_DDC2BI,

    /* E_INT_IRQ_SCM */                     E_IRQ_SCM,

    /* E_INT_IRQ_VBI */                     E_IRQ_VBI,

    /* E_INT_IRQ_M4VD */                    E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_FCIE2RIU */                E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_ADCDVI2RIU */              E_IRQ_ADCDVI2RIU,



    /* 0x20 */

    /* E_INT_FIQ_EXTIMER0 */                E_FIQ_EXTIMER0,

    /* E_INT_FIQ_EXTIMER1 */                E_FIQ_EXTIMER1,

    /* E_INT_FIQ_WDT */                     E_FIQ_WDT,

    /* E_INT_FIQ_AEON_TO_8051 */            E_IRQ_FIQ_NONE, //### For MAILBOX : No Use

    /* E_INT_FIQ_8051_TO_AEON */            E_IRQ_FIQ_NONE, //### For MAILBOX : No Use

    /* E_INT_FIQ_8051_TO_BEON */            E_FIQ_8051_TO_BEON, //### For MAILBOX

    /* E_INT_FIQ_BEON_TO_8051 */            E_FIQ_BEON_TO_8051, //### For MAILBOX

    /* E_INT_FIQ_BEON_TO_AEON */            E_IRQ_FIQ_NONE, //### For MAILBOX : No Use

    /* E_INT_FIQ_AEON_TO_BEON */            E_FIQ_AEON_TO_BEON, //### For MAILBOX

    /* E_INT_FIQ_JPD */                     E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_MENULOAD */                E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_HDMI_NON_PCM */            E_FIQ_HDMI_NON_PCM,

    /* E_INT_FIQ_SPDIF_IN_NON_PCM */        E_FIQ_SPDIF_IN_NON_PCM,

    /* E_INT_FIQ_EMAC */                    E_FIQ_EMAC,

    /* E_INT_FIQ_SE_DSP2UP */               E_FIQ_SE_DSP2UP,

    /* E_INT_FIQ_TSP2AEON */                E_FIQ_TSP2AEON,



    /* 0x30 */

    /* E_INT_FIQ_VIVALDI_STR */             E_FIQ_VIVALDI_STR,

    /* E_INT_FIQ_VIVALDI_PTS */             E_FIQ_VIVALDI_PTS,

    /* E_INT_FIQ_DSP_MIU_PROT */            E_FIQ_DSP_MIU_PROT,

    /* E_INT_FIQ_XIU_TIMEOUT */             E_FIQ_XIU_TIMEOUT,

    /* E_INT_FIQ_DMA_DONE */                E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_VSYNC_VE4VBI */            E_FIQ_VSYNC_VE4VBI,

    /* E_INT_FIQ_FIELD_VE4VBI */            E_FIQ_FIELD_VE4VBI,

    /* E_INT_FIQ_VDMCU2HK */                E_FIQ_VDMCU2HK,

    /* E_INT_FIQ_VE_DONE_TT */              E_FIQ_VE_DONE_TT,

    /* E_INT_FIQ_INT_CCFL */                E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_INT */                     E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_IR */                      E_FIQ_IR,

    /* E_INT_FIQ_AFEC_VSYNC */              E_FIQ_AFEC_VSYNC,

    /* E_INT_FIQ_DEC_DSP2UP */              E_FIQ_DEC_DSP2UP,

    /* E_INT_FIQ_MIPS_WDT */                E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_DEC_DSP2MIPS */            E_FIQ_DSP2MIPS,





    /* 0x40 */

    /* E_INT_IRQ_SVD_HVD */                 E_IRQEXPL_HVD,

    /* E_INT_IRQ_USB2 */                    E_IRQEXPL_USB2,

    /* E_INT_IRQ_UHC2 */                    E_IRQEXPL_UHC2,

    /* E_INT_IRQ_MIU */                     E_IRQEXPL_MIU,

    /* E_INT_IRQ_GDMA */                    E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_UART2 */                   E_IRQEXPL_UART2,

    /* E_INT_IRQ_UART1 */                   E_IRQEXPL_UART1,

    /* E_INT_IRQ_DEMOD */                   E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_MPIF */                    E_IRQEXPL_MPIF,

    /* E_INT_IRQ_JPD */                     E_IRQEXPL_JPD,

    /* E_INT_IRQ_AEON2HI */                 E_IRQEXPL_AEON2HI,

    /* E_INT_IRQ_BDMA0 */                   E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_BDMA1 */                   E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_OTG */                     E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_MVD_CHECKSUM_FAIL */       E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_TSP_CHECKSUM_FAIL */       E_IRQ_FIQ_NONE,





    /* 0x50 */

    /* E_INT_IRQ_CA_I3 */                   E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_HDMI_LEVEL */              E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_MIPS_WADR_ERR */           E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_RASP */                    E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_CA_SVP */                  E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_UART2MCU */                E_IRQEXPH_UART2MCU,

    /* E_INT_IRQ_URDMA2MCU */               E_IRQEXPH_URDMA2MCU,

    /* E_INT_IRQ_IIC1 */                    E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_HDCP */                    E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_DMA_WADR_ERR */            E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_UP_IRQ_UART_CA */          E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_UP_IRQ_EMM_ECM */          E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_ONIF */                    E_IRQ_ONIF,

    /* E_INT_IRQ_USB1 */                    E_IRQEXPL_USB1,

    /* E_INT_IRQ_UHC1 */                    E_IRQEXPL_UHC1,

    /* E_INT_IRQ_MFE */                     E_IRQEXPL_MFE,



    /* 0x60 */

    /* E_INT_FIQ_IR_INT_RC */               E_FIQEXPL_IR_INT_RC,

    /* E_INT_FIQ_HDMITX_IRQ_EDGE */         E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_UP_IRQ_UART_CA */          E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_UP_IRQ_EMM_ECM */          E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_PVR2MI_INT0 */             E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_PVR2MI_INT1 */             E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_8051_TO_AEON */            E_IRQ_FIQ_NONE,

    /* 0x67 Not Used */                     E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_AEON_TO_MIPS_VPE0 */       E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_AEON_TO_MIPS_VPE1 */       E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_AEON_TO_8051 */            E_IRQ_FIQ_NONE,

    /* 0x6B Not Used */                     E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_MIPS_VPE1_TO_MIPS_VPE0 */  E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_MIPS_VPE1_TO_AEON */       E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_MIPS_VPE1_TO_8051 */       E_IRQ_FIQ_NONE,

    /* 0x6F Not Used */                     E_IRQ_FIQ_NONE,



    /* 0x70 */

    /* E_INT_FIQ_MIPS_VPE0_TO_MIPS_VPE1 */  E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_MIPS_VPE0_TO_AEON */       E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_MIPS_VPE0_TO_8051 */       E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_IR_IN */                   E_FIQEXPL_IR_IN,

    /* E_INT_FIQ_DMDMCU2HK */               E_FIQ_DMDMCU2HK,

    /* E_INT_FIQ_R2TOMCU_INT0 */            E_FIQ_AEON_MB2_MCU0,

    /* E_INT_FIQ_R2TOMCU_INT1 */            E_FIQ_AEON_MB2_MCU1,

    /* E_INT_FIQ_DSPTOMCU_INT0 */           E_FIQ_DSP2_MB2_MCU0,

    /* E_INT_FIQ_DSPTOMCU_INT1 */           E_FIQ_DSP2_MB2_MCU1,

    /* E_INT_FIQ_USB */                     E_FIQ_USB,

    /* E_INT_FIQ_UHC */                     E_FIQ_UHC,

    /* E_INT_FIQ_USB1 */                    E_FIQEXPH_USB1,

    /* E_INT_FIQ_UHC1 */                    E_FIQEXPH_UHC1,

    /* E_INT_FIQ_USB2 */                    E_FIQEXPH_USB2,

    /* E_INT_FIQ_UHC2 */                    E_FIQEXPH_UHC2,

    /* 0x7F Not Used */                     E_IRQ_FIQ_NONE,



    /* 0x80 */

    /* E_INT_IRQ_MLINK */                   E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_AFEC */                    E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_DPTX */                    E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_TMDDRLINK */               E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_DISPI */                   E_IRQEXPL_DISPI,

    /* E_INT_IRQ_EXP_MLINK */               E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_M4VE */                    E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_DVI_HDMI_HDCP */           E_IRQEXPH_DVI_HDMI_HDCP,

    /* E_INT_IRQ_G3D2MCU */                 E_IRQEXPH_G3D2MCU,

    /* E_INT_IRQ_VP6 */                     E_IRQ_VP6,

    /* 0x8A Not Used */                     E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_CEC */                     E_IRQEXPH_CEC,

    /* E_INT_IRQ_HDCP_IIC */                E_IRQEXPH_HDCP_IIC,

    /* E_INT_IRQ_HDCP_X74 */                E_IRQEXPH_HDCP_X74,

    /* E_INT_IRQ_WADR_ERR */                E_IRQEXPH_WADR_ERR,

    /* E_INT_IRQ_DCSUB */                   E_IRQEXPH_DCSUB,



    /* 0x90 */

    /* E_INT_IRQ_GE */                      E_IRQEXPH_GE,

    /* 0x91 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0x92 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0x93 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0x94 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0x95 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0x96 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0x97 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0x98 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0x99 Not Used */                     E_IRQ_SMART,

    /* 0x9A Not Used */                     E_IRQ_MVD2MIPS,

    /* 0x9B Not Used */                     E_IRQ_GPD,

    /* 0x9C Not Used */                     E_IRQ_FIQ_NONE,

    /* 0x9D Not Used */                     E_IRQ_FIQ_NONE,

    /* 0x9E Not Used */                     E_IRQ_MIIC_DMA_INT3,

    /* 0x9F Not Used */                     E_IRQ_MIIC_INT3,





    /* 0xA0 */

    /* 0xA0 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xA1 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xA2 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xA3 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xA4 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xA5 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xA6 Not Used */                     E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_EXT_GPIO1 */               E_FIQEXPL_GPIO1,

    /* 0xA8 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xA9 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xAA Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xAB Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xAC Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xAD Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xAE Not Used */                     E_IRQEXPL_MIIC_DMA_INT2,

    /* 0xAF Not Used */                     E_IRQEXPL_MIIC_INT2,



    /* 0xB0 */

    /* 0xB0 Not Used */                     E_IRQEXPH_MIIC_DMA_INT1,

    /* 0xB1 Not Used */                     E_IRQEXPH_MIIC_INT1,

    /* 0xB2 Not Used */                     E_IRQEXPH_MIIC_DMA_INT0,

    /* 0xB3 Not Used */                     E_IRQEXPH_MIIC_INT0,

    /* 0xB4 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xB5 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xB6 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xB7 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xB8 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xB9 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xBA Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xBB Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xBC Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xBD Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xBE Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xBF Not Used */                     E_IRQ_FIQ_NONE,



    /* 0xC0 */

    /* E_INT_FIQ_DMARD */                   E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_AU_DMA_BUF_INT */          E_FIQEXPL_AU_DMA_BUF_INT,

    /* E_INT_FIQ_8051_TO_MIPS_VPE1 */       E_IRQ_FIQ_NONE,

    /* 0xC3 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xC4 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xC5 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xC6 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xC7 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xC8 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xC9 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xCA Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xCB Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xCC Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xCD Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xCE Not Used */                     E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_VP6 */                     E_FIQ_VP6,



    /* 0xD0 */

    /* 0xD0 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xD1 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xD2 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xD3 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xD4 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xD5 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xD6 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xD7 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xD8 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xD9 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xDA Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xDB Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xDC Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xDD Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xDE Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xDF Not Used */                     E_IRQ_FIQ_NONE,



    /* 0xE0 */

    /* 0xE0 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xE1 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xE2 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xE3 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xE4 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xE5 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xE6 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xE7 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xE8 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xE9 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xEA Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xEB Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xEC Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xED Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xEE Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xEF Not Used */                     E_IRQ_FIQ_NONE,



    /* 0xF0 */

    /* 0xF0 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xF1 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xF2 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xF3 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xF4 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xF5 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xF6 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xF7 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xF8 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xF9 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xFA Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xFB Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xFC Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xFD Not Used */                     E_IRQ_FIQ_NONE,

    /* E_IRQ_FIQ_NONE */                    E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_FIQ_ALL */                 E_IRQ_FIQ_ALL



};



static MS_U8 HWIdx2IntEnum[128] =

{

    /* 0x00 */

    E_INT_FIQ_EXTIMER0,                     //E_FIQ_EXTIMER0,

    E_INT_FIQ_EXTIMER1,                     //E_FIQ_EXTIMER1,

    E_INT_FIQ_WDT,                          //E_FIQ_WDT,

    E_INT_FIQ_STRETCH,                     //E_IRQ_FIQ_NONE,

    E_INT_FIQ_R2TOMCU_INT0,                 //E_FIQ_AEON_MB2_MCU0

    E_INT_FIQ_R2TOMCU_INT1,                 //E_FIQ_AEON_MB2_MCU1

    E_INT_FIQ_DSPTOMCU_INT0,                //E_FIQ_DSP2_MB2_MCU0

    E_INT_FIQ_DSPTOMCU_INT1,                //E_FIQ_DSP2_MB2_MCU1

    E_INT_FIQ_USB,                          //E_FIQ_USB

    E_INT_FIQ_UHC,                          //E_FIQ_UHC

    E_INT_FIQ_VP6,                          //E_FIQ_VP6,    // A3

    E_INT_FIQ_HDMI_NON_PCM,                 //E_FIQ_HDMI_NON_PCM,

    E_INT_FIQ_SPDIF_IN_NON_PCM,             //E_FIQ_SPDIF_IN_NON_PCM,

    E_INT_FIQ_EMAC,                         //E_FIQ_EMAC

    E_INT_FIQ_SE_DSP2UP,                    //E_FIQ_SE_DSP2UP,

    E_INT_FIQ_TSP2AEON,                     //E_FIQ_TSP2AEON,



    /* 0x10 */

    E_INT_FIQ_VIVALDI_STR,                  //E_FIQ_VIVALDI_STR,

    E_INT_FIQ_VIVALDI_PTS,                  //E_FIQ_VIVALDI_PTS,

    E_INT_FIQ_DSP_MIU_PROT,                 //E_FIQ_DSP_MIU_PROT,

    E_INT_FIQ_XIU_TIMEOUT,                  //E_FIQ_XIU_TIMEOUT,

    E_INT_FIQ_DMDMCU2HK,                    //E_FIQ_DMDMCU2HK,

    E_INT_FIQ_VSYNC_VE4VBI,                 //E_FIQ_VSYNC_VE4VBI,

    E_INT_FIQ_FIELD_VE4VBI,                 //E_FIQ_FIELD_VE4VBI,

    E_INT_FIQ_VDMCU2HK,                     //E_FIQ_VDMCU2HK,

    E_INT_FIQ_VE_DONE_TT,                   //E_FIQ_VE_DONE_TT,

    E_INT_IRQ_FIQ_NONE,                     //E_FIQ_INT_CCFL,

    E_INT_IRQ_FIQ_NONE,                     //E_FIQ_INT,

    E_INT_FIQ_IR,                           //E_FIQ_IR,

    E_INT_FIQ_AFEC_VSYNC,                   //E_FIQ_AFEC_VSYNC,

    E_INT_FIQ_DEC_DSP2UP,                   //E_FIQ_DEC_DSP2UP,

    E_INT_FIQ_AEON_TO_BEON,                 //E_FIQ_AEON_TO_BEON, //### For MAILBOX

    E_INT_FIQ_DEC_DSP2MIPS,                 //E_FIQ_DEC_DSP2MIPS,



	/* 0x20 */

    E_INT_FIQ_IR_INT_RC,                    //E_FIQEXPL_IR_INT_RC,

    E_INT_FIQ_AU_DMA_BUF_INT,               //E_FIQEXPL_AU_DMA_BUF_INT,

    E_INT_FIQ_IR_IN,                        //E_FIQEXPL_IR_IN

    E_INT_IRQ_FIQ_NONE,                     //E_IRQ_FIQ_NONE,

    E_INT_IRQ_FIQ_NONE,                     //E_FIQEXPL_8051_TO_MIPS_VPE0,

    E_INT_IRQ_FIQ_NONE,                     //E_FIQEXPL_8051_TO_MIPS_VPE1,

    E_INT_FIQ_8051_TO_BEON,                 //E_FIQ_8051_TO_BEON, //### For MAILBOX

    E_INT_FIQ_GPIO0,                        //E_IRQ_FIQ_NONE,

    E_INT_IRQ_FIQ_NONE,                     //E_FIQEXPL_AEON_TO_MIPS_VPE0,

    E_INT_IRQ_FIQ_NONE,                     //E_FIQEXPL_AEON_TO_MIPS_VPE1,

    E_INT_FIQ_BEON_TO_8051,                 //E_FIQ_BEON_TO_8051, //### For MAILBOX

    E_INT_FIQ_GPIO1,                        //E_IRQ_FIQ_NONE,

    E_INT_IRQ_FIQ_NONE,                     //E_FIQEXPL_MIPS_VPE1_TO_MIPS_VPE0,

    E_INT_IRQ_FIQ_NONE,                     //E_FIQEXPL_MIPS_VPE1_TO_AEON,

    E_INT_IRQ_FIQ_NONE,                     //E_FIQEXPL_MIPS_VPE1_TO_8051,

    E_INT_FIQ_GPIO2,                        //E_IRQ_FIQ_NONE,



    /* 0x30 */

    E_INT_IRQ_FIQ_NONE,                     //E_FIQEXPH_MIPS_VPE0_TO_MIPS_VPE1,

    E_INT_IRQ_FIQ_NONE,                     //E_FIQEXPH_MIPS_VPE0_TO_AEON,

    E_INT_IRQ_FIQ_NONE,                     //E_FIQEXPH_MIPS_VPE0_TO_8051,

    E_INT_FIQ_USB1,                         //E_FIQEXPH_USB1

    E_INT_FIQ_UHC1,                         //E_FIQEXPH_UHC1

    E_INT_FIQ_USB2,                         //E_FIQEXPH_USB2

    E_INT_FIQ_UHC2,                         //E_FIQEXPH_UHC2

    E_INT_FIQ_GPIO3,

    E_INT_FIQ_GPIO4,

    E_INT_FIQ_GPIO5,

    E_INT_FIQ_GPIO6,

    E_INT_FIQ_PWM_RP_RP_L,

    E_INT_FIQ_PWM_RP_FP_L,

    E_INT_FIQ_PWM_RP_RP_R,

    E_INT_FIQ_PWM_RP_FP_R,

    E_INT_FIQ_GPIO7,

    

    /* 0x40 */

    E_INT_IRQ_UART0,                        //E_IRQ_UART0,

    E_INT_IRQ_PMSLEEP,                      //E_IRQ_FIQ_NONE,

    E_INT_IRQ_ONIF,                         //E_IRQ_ONIF,

    E_INT_IRQ_MVD,                          //E_IRQ_MVD,

    E_INT_IRQ_PS,                           //E_IRQ_PS,

    E_INT_IRQ_NFIE,                         //E_IRQ_NFIE,

    E_INT_IRQ_USB,                          //E_IRQ_USB,

    E_INT_IRQ_UHC,                          //E_IRQ_UHC,

    E_INT_IRQ_EC_BRIDGE,                    //E_IRQ_EC_BRIDGE,

    E_INT_IRQ_EMAC,                         //E_IRQ_EMAC,

    E_INT_IRQ_DISP,                         //E_IRQ_DISP,

    E_INT_IRQ_FIQ_NONE,                     //E_IRQ_FRC_SC,  // A5

    E_INT_IRQ_MIIC_DMA_INT3,                //E_IRQ_MIIC_DMA_INT3, // A5

    E_INT_IRQ_MIIC_INT3,                    //E_IRQ_MIIC_INT3, // A5

    E_INT_IRQ_COMB,                         //E_IRQ_COMB,

    E_INT_IRQ_FIQ_NONE,                     //E_IRQ_FRC_INT_FIQ2HST0, // A5



    /* 0x50 */

    E_INT_IRQ_TSP2HK,                       //E_IRQ_TSP2HK,

    E_INT_IRQ_VE,                           //E_IRQ_VE,

    E_INT_IRQ_CIMAX2MCU,                    //E_IRQ_CIMAX2MCU,

    E_INT_IRQ_DC,                           //E_IRQ_DC,

    E_INT_IRQ_GOP,                          //E_IRQ_GOP,

    E_INT_IRQ_PCM,                          //E_IRQ_PCM,

    E_INT_IRQ_FIQ_NONE,                     //E_IRQ_IIC0,

    E_INT_IRQ_SMART,                        //E_IRQ_SMART,  // A5

    E_INT_IRQ_VP6,                          //E_IRQ_VP6,    // A3

    E_INT_IRQ_FIQ_NONE,                     //E_IRQ_PM,

    E_INT_IRQ_DDC2BI,                       //E_IRQ_DDC2BI,

    E_INT_IRQ_SCM,                          //E_IRQ_SCM,

    E_INT_IRQ_VBI,                          //E_IRQ_VBI,

    E_INT_IRQ_MVD2MIPS,                     //E_IRQ_MVD2MIPS, // A5

    E_INT_IRQ_GPD,                          //E_IRQ_GPD, // A5

    E_INT_IRQ_ADCDVI2RIU,                   //E_IRQ_ADCDVI2RIU,



    /* 0x60 */

    E_INT_IRQ_SVD_HVD,                      //E_IRQEXPL_HVD,

    E_INT_IRQ_USB1,                         //E_IRQEXPL_USB1,

    E_INT_IRQ_UHC1,                         //E_IRQEXPL_UHC1,

    E_INT_IRQ_MIU,                          //E_IRQEXPL_MIU,

    E_INT_IRQ_USB2,                         //E_IRQEXPL_USB2,

    E_INT_IRQ_UHC2,                         //E_IRQEXPL_UHC2,

    E_INT_IRQ_AEON2HI,                      //E_IRQEXPL_AEON2HI,

    E_INT_IRQ_UART1,                        //E_IRQEXPL_UART1,

    E_INT_IRQ_UART2,                        //E_IRQEXPL_UART2,

    E_INT_IRQ_FIQ_NONE,                     //E_IRQEXPL_FRC_INT_IRQ2HST0, // A5

    E_INT_IRQ_MPIF,                         //E_IRQEXPL_MPIF,

    E_INT_IRQ_MIIC_DMA_INT2,                //E_IRQEXPL_MIIC_DMA_INT2,    // A5

    E_INT_IRQ_MIIC_INT2,                    //E_IRQEXPL_MIIC_INT2,        // A5

    E_INT_IRQ_JPD,                          //E_IRQEXPL_JPD,

    E_INT_IRQ_DISPI,                        //E_IRQEXPL_DISPI,

    E_INT_IRQ_MFE,                          //E_IRQEXPL_MFE,



    /* 0x70 */

    E_INT_IRQ_BDMA0,                        //E_IRQEXPH_BDMA0,

    E_INT_IRQ_BDMA1,                        //E_IRQEXPH_BDMA1,

    E_INT_IRQ_UART2MCU,                     //E_IRQEXPH_UART2MCU,

    E_INT_IRQ_URDMA2MCU,                    //E_IRQEXPH_URDMA2MCU,

    E_INT_IRQ_DVI_HDMI_HDCP,                //E_IRQEXPH_DVI_HDMI_HDCP,

    E_INT_IRQ_G3D2MCU,                      //E_IRQEXPH_G3D2MCU,

    E_INT_IRQ_CEC,                          //E_IRQEXPH_CEC

    E_INT_IRQ_HDCP_IIC,                     //E_IRQEXPH_HDCP_IIC

    E_INT_IRQ_HDCP_X74,                     //E_IRQEXPH_HDCP_X74

    E_INT_IRQ_WADR_ERR,                     //E_IRQEXPH_WADR_ERR

    E_INT_IRQ_DCSUB,                        //E_IRQEXPH_DCSUB

    E_INT_IRQ_GE,                           //E_IRQEXPH_GE

    E_INT_IRQ_MIIC_DMA_INT1,                //E_IRQ_FIQ_NONE, A5

    E_INT_IRQ_MIIC_INT1,                    //E_IRQ_FIQ_NONE, A5

    E_INT_IRQ_MIIC_DMA_INT0,                //E_IRQ_FIQ_NONE, A5

    E_INT_IRQ_MIIC_INT0,                    //E_IRQ_FIQ_NONE, A5



};

#elif defined (MSOS_TYPE_NOS)



typedef enum

{

    // IRQ

    E_IRQL_START                = 0x00,

    E_IRQ_UART0                 = E_IRQL_START  + 0,

    E_IRQ_PMSLEEP               = E_IRQL_START  + 1,  // T12

    E_IRQ_ONIF                  = E_IRQL_START  + 2,  // T8

    E_IRQ_MVD                   = E_IRQL_START  + 3,

    E_IRQ_PS                    = E_IRQL_START  + 4,

    E_IRQ_NFIE                  = E_IRQL_START  + 5,

    E_IRQ_USB                   = E_IRQL_START  + 6,

    E_IRQ_UHC                   = E_IRQL_START  + 7,

    E_IRQ_EC_BRIDGE             = E_IRQL_START  + 8,

    E_IRQ_EMAC                  = E_IRQL_START  + 9,

    E_IRQ_DISP                  = E_IRQL_START  + 10,

    E_IRQ_MIIC_DMA_INT3         = E_IRQL_START  + 12, // A5

    E_IRQ_MIIC_INT3             = E_IRQL_START  + 13, // A5

    E_IRQ_COMB                  = E_IRQL_START  + 14,

    E_IRQL_END                  = 15,



    E_IRQH_START                = 16,

    E_IRQ_TSP2HK                = E_IRQH_START  + 0,

    E_IRQ_VE                    = E_IRQH_START  + 1,

    E_IRQ_CIMAX2MCU             = E_IRQH_START  + 2,

    E_IRQ_DC                    = E_IRQH_START  + 3,

    E_IRQ_GOP                   = E_IRQH_START  + 4,

    E_IRQ_PCM                   = E_IRQH_START  + 5,

    E_IRQ_SMART                 = E_IRQH_START  + 7,  // A5

    E_IRQ_VP6                   = E_IRQH_START  + 9,  // A3

    E_IRQ_DDC2BI                = E_IRQH_START  + 10,

    E_IRQ_SCM                   = E_IRQH_START  + 11,

    E_IRQ_VBI                   = E_IRQH_START  + 12,

    E_IRQ_MVD2MIPS              = E_IRQH_START  + 13, // A5

    E_IRQ_GPD                   = E_IRQH_START  + 14, // A5

    E_IRQ_ADCDVI2RIU            = E_IRQH_START  + 15,

    E_IRQH_END                  = 31,



    // FIQ

    E_FIQL_START                = 32,

    E_FIQ_EXTIMER0              = E_FIQL_START +  0,

    E_FIQ_EXTIMER1              = E_FIQL_START +  1,

    E_FIQ_WDT                   = E_FIQL_START +  2,

    E_FIQ_STRETCH               = E_FIQL_START +  3,  // T12

    E_FIQ_AEON_MB2_MCU0         = E_FIQL_START +  4,  // T8

    E_FIQ_AEON_MB2_MCU1         = E_FIQL_START +  5,  // T8

    E_FIQ_DSP2_MB2_MCU0         = E_FIQL_START +  6,  // T8

    E_FIQ_DSP2_MB2_MCU1         = E_FIQL_START +  7,  // T8

    E_FIQ_USB                   = E_FIQL_START +  8,  // T8

    E_FIQ_UHC                   = E_FIQL_START +  9,  // T8

    E_FIQ_VP6                   = E_FIQL_START + 10,  // A3

    E_FIQ_HDMI_NON_PCM          = E_FIQL_START + 11,

    E_FIQ_SPDIF_IN_NON_PCM      = E_FIQL_START + 12,

    E_FIQ_EMAC                  = E_FIQL_START + 13,

    E_FIQ_SE_DSP2UP             = E_FIQL_START + 14,

    E_FIQ_TSP2AEON              = E_FIQL_START + 15,

    E_FIQL_END                  = 47,



    E_FIQH_START                = 48,

    E_FIQ_VIVALDI_STR           = E_FIQH_START +  0,

    E_FIQ_VIVALDI_PTS           = E_FIQH_START +  1,

    E_FIQ_DSP_MIU_PROT          = E_FIQH_START +  2,

    E_FIQ_XIU_TIMEOUT           = E_FIQH_START +  3,

    E_FIQ_DMDMCU2HK             = E_FIQH_START +  4,  // T8

    E_FIQ_VSYNC_VE4VBI          = E_FIQH_START +  5,

    E_FIQ_FIELD_VE4VBI          = E_FIQH_START +  6,

    E_FIQ_VDMCU2HK              = E_FIQH_START +  7,

    E_FIQ_VE_DONE_TT            = E_FIQH_START +  8,

    E_FIQ_IR                    = E_FIQH_START + 11,

    E_FIQ_AFEC_VSYNC            = E_FIQH_START + 12,

    E_FIQ_DEC_DSP2UP            = E_FIQH_START + 13,

    E_FIQ_DSP2MIPS              = E_FIQH_START + 15,

    E_FIQH_END                  = 63,



    //IRQEXP

    E_IRQEXPL_START             = 64,

    E_IRQEXPL_HVD               = E_IRQEXPL_START+0,

    E_IRQEXPL_USB1              = E_IRQEXPL_START+1,

    E_IRQEXPL_UHC1              = E_IRQEXPL_START+2,

    E_IRQEXPL_MIU               = E_IRQEXPL_START+3,

    E_IRQEXPL_USB2              = E_IRQEXPL_START+4,

    E_IRQEXPL_UHC2              = E_IRQEXPL_START+5,

    E_IRQEXPL_AEON2HI           = E_IRQEXPL_START+6,

    E_IRQEXPL_UART1             = E_IRQEXPL_START+7,

    E_IRQEXPL_UART2             = E_IRQEXPL_START+8,

    E_IRQEXPL_MPIF              = E_IRQEXPL_START+10,

    E_IRQEXPL_MIIC_DMA_INT2     = E_IRQEXPL_START+11, // A5

    E_IRQEXPL_MIIC_INT2         = E_IRQEXPL_START+12, // A5

    E_IRQEXPL_JPD               = E_IRQEXPL_START+13,

    E_IRQEXPL_DISPI             = E_IRQEXPL_START+14,

    E_IRQEXPL_MFE               = E_IRQEXPL_START+15, // T8

    E_IRQEXPL_END               = 79,



    E_IRQEXPH_START             = 80,

    E_IRQEXPH_BDMA0             = E_IRQEXPH_START+0,

    E_IRQEXPH_BDMA1             = E_IRQEXPH_START+1,

    E_IRQEXPH_UART2MCU          = E_IRQEXPH_START+2,

    E_IRQEXPH_URDMA2MCU         = E_IRQEXPH_START+3,

    E_IRQEXPH_DVI_HDMI_HDCP     = E_IRQEXPH_START+4,

    E_IRQEXPH_G3D2MCU           = E_IRQEXPH_START+5,

    E_IRQEXPH_CEC               = E_IRQEXPH_START+6, // T8

    E_IRQEXPH_HDCP_IIC          = E_IRQEXPH_START+7, // T8

    E_IRQEXPH_HDCP_X74          = E_IRQEXPH_START+8, // T8

    E_IRQEXPH_WADR_ERR          = E_IRQEXPH_START+9, // T8

    E_IRQEXPH_DCSUB             = E_IRQEXPH_START+10, // T8

    E_IRQEXPH_GE                = E_IRQEXPH_START+11, // T8

    E_IRQEXPH_MIIC_DMA_INT1     = E_IRQEXPH_START+12, // A5

    E_IRQEXPH_MIIC_INT1         = E_IRQEXPH_START+13, // A5

    E_IRQEXPH_MIIC_DMA_INT0     = E_IRQEXPH_START+14, // A5

    E_IRQEXPH_MIIC_INT0         = E_IRQEXPH_START+15, // A5

    E_IRQEXPH_END               = 95,



    // FIQEXP

    E_FIQEXPL_START                     = 96,

    E_FIQEXPL_IR_INT_RC                 = E_FIQEXPL_START+0,

    E_FIQEXPL_AU_DMA_BUF_INT            = E_FIQEXPL_START+1,

    E_FIQEXPL_IR_IN                     = E_FIQEXPL_START+2,

    E_FIQEXPL_8051_TO_MIPS_VPE0         = E_FIQEXPL_START+6, // MBX (8051->MIPS)

    E_FIQEXPL_GPIO0                     = E_FIQEXPL_START+7, // T12

    E_FIQEXPL_MIPS_VPE0_TO_8051         = E_FIQEXPL_START+10,// MBX (MIPS-->8051)

    E_FIQEXPL_GPIO1                     = E_FIQEXPL_START+11,

    E_FIQEXPL_GPIO2                     = E_FIQEXPL_START+15, // T12

    E_FIQEXPL_END                       = 111,



    E_FIQEXPH_START                     = 112,

    E_FIQEXPH_USB1                      = E_FIQEXPH_START +  3,

    E_FIQEXPH_UHC1                      = E_FIQEXPH_START +  4,

    E_FIQEXPH_USB2                      = E_FIQEXPH_START +  5,

    E_FIQEXPH_UHC2                      = E_FIQEXPH_START +  6,

    E_FIQEXPH_GPIO3                     = E_FIQEXPH_START +  7, // T12

    E_FIQEXPH_GPIO4                     = E_FIQEXPH_START +  8, // T12

    E_FIQEXPH_GPIO5                     = E_FIQEXPH_START +  9, // T12

    E_FIQEXPH_GPIO6                     = E_FIQEXPH_START + 10, // T12

    E_FIQEXPH_PWM_RP_L                  = E_FIQEXPH_START + 11, // T12

    E_FIQEXPH_PWM_FP_L                  = E_FIQEXPH_START + 12, // T12

    E_FIQEXPH_PWM_RP_R                  = E_FIQEXPH_START + 13, // T12

    E_FIQEXPH_PWM_FP_R                  = E_FIQEXPH_START + 14, // T12

    E_FIQEXPH_GPIO7                     = E_FIQEXPH_START + 15, // T12

    E_FIQEXPH_END                       = 127,



    //For Mailbox

    E_FIQ_8051_TO_BEON                  = E_FIQEXPL_8051_TO_MIPS_VPE0,

    E_FIQ_BEON_TO_8051                  = E_FIQEXPL_MIPS_VPE0_TO_8051,

    E_FIQ_AEON_TO_BEON                  = E_FIQEXPL_MIPS_VPE0_TO_8051,



    E_IRQ_FIQ_NONE              = 0xFE,

    E_IRQ_FIQ_ALL               = 0xFF //all IRQs & FIQs



} IRQFIQNum;





static MS_U8 IntEnum2HWIdx[256] =

{

    /* 0x00 */

    /* E_INT_IRQ_UART0 */                   E_IRQ_UART0,

    /* E_INT_IRQ_BDMA_CH0 */                E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_BDMA_CH1 */                E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_MVD */                     E_IRQ_MVD,

    /* E_INT_IRQ_PS */                      E_IRQ_PS,

    /* E_INT_IRQ_NFIE */                    E_IRQ_NFIE,

    /* E_INT_IRQ_USB */                     E_IRQ_USB,

    /* E_INT_IRQ_UHC */                     E_IRQ_UHC,

    /* E_INT_IRQ_EC_BRIDGE */               E_IRQ_EC_BRIDGE,

    /* E_INT_IRQ_EMAC */                    E_IRQ_EMAC,

    /* E_INT_IRQ_DISP */                    E_IRQ_DISP,

    /* E_INT_IRQ_DHC */                     E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_PMSLEEP */                 E_IRQ_PMSLEEP,

    /* E_INT_IRQ_SBM */                     E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_COMB */                    E_IRQ_COMB,

    /* E_INT_IRQ_ECC_DERR */                E_IRQ_FIQ_NONE,



    /* 0x10 */

    /* E_INT_IRQ_TSP2HK */                  E_IRQ_TSP2HK,

    /* E_INT_IRQ_VE */                      E_IRQ_VE,

    /* E_INT_IRQ_CIMAX2MCU */               E_IRQ_CIMAX2MCU,

    /* E_INT_IRQ_DC */                      E_IRQ_DC,

    /* E_INT_IRQ_GOP */                     E_IRQ_GOP,

    /* E_INT_IRQ_PCM */                     E_IRQ_PCM,

    /* E_INT_IRQ_IIC0 */                    E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_RTC */                     E_IRQ_SMART,

    /* E_INT_IRQ_KEYPAD */                  E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_PM */                      E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_DDC2BI */                  E_IRQ_DDC2BI,

    /* E_INT_IRQ_SCM */                     E_IRQ_SCM,

    /* E_INT_IRQ_VBI */                     E_IRQ_VBI,

    /* E_INT_IRQ_M4VD */                    E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_FCIE2RIU */                E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_ADCDVI2RIU */              E_IRQ_ADCDVI2RIU,



    /* 0x20 */

    /* E_INT_FIQ_EXTIMER0 */                E_FIQ_EXTIMER0,

    /* E_INT_FIQ_EXTIMER1 */                E_FIQ_EXTIMER1,

    /* E_INT_FIQ_WDT */                     E_FIQ_WDT,

    /* E_INT_FIQ_AEON_TO_8051 */            E_IRQ_FIQ_NONE, //### For MAILBOX : No Use

    /* E_INT_FIQ_8051_TO_AEON */            E_IRQ_FIQ_NONE, //### For MAILBOX : No Use

    /* E_INT_FIQ_8051_TO_BEON */            E_FIQ_8051_TO_BEON, //### For MAILBOX

    /* E_INT_FIQ_BEON_TO_8051 */            E_FIQ_BEON_TO_8051, //### For MAILBOX

    /* E_INT_FIQ_BEON_TO_AEON */            E_IRQ_FIQ_NONE, //### For MAILBOX : No Use

    /* E_INT_FIQ_AEON_TO_BEON */            E_FIQ_AEON_TO_BEON, //### For MAILBOX

    /* E_INT_FIQ_JPD */                     E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_MENULOAD */                E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_HDMI_NON_PCM */            E_FIQ_HDMI_NON_PCM,

    /* E_INT_FIQ_SPDIF_IN_NON_PCM */        E_FIQ_SPDIF_IN_NON_PCM,

    /* E_INT_FIQ_EMAC */                    E_FIQ_EMAC,

    /* E_INT_FIQ_SE_DSP2UP */               E_FIQ_SE_DSP2UP,

    /* E_INT_FIQ_TSP2AEON */                E_FIQ_TSP2AEON,



    /* 0x30 */

    /* E_INT_FIQ_VIVALDI_STR */             E_FIQ_VIVALDI_STR,

    /* E_INT_FIQ_VIVALDI_PTS */             E_FIQ_VIVALDI_PTS,

    /* E_INT_FIQ_DSP_MIU_PROT */            E_FIQ_DSP_MIU_PROT,

    /* E_INT_FIQ_XIU_TIMEOUT */             E_FIQ_XIU_TIMEOUT,

    /* E_INT_FIQ_DMA_DONE */                E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_VSYNC_VE4VBI */            E_FIQ_VSYNC_VE4VBI,

    /* E_INT_FIQ_FIELD_VE4VBI */            E_FIQ_FIELD_VE4VBI,

    /* E_INT_FIQ_VDMCU2HK */                E_FIQ_VDMCU2HK,

    /* E_INT_FIQ_VE_DONE_TT */              E_FIQ_VE_DONE_TT,

    /* E_INT_FIQ_INT_CCFL */                E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_INT */                     E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_IR */                      E_FIQ_IR,

    /* E_INT_FIQ_AFEC_VSYNC */              E_FIQ_AFEC_VSYNC,

    /* E_INT_FIQ_DEC_DSP2UP */              E_FIQ_DEC_DSP2UP,

    /* E_INT_FIQ_MIPS_WDT */                E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_DEC_DSP2MIPS */            E_FIQ_DSP2MIPS,





    /* 0x40 */

    /* E_INT_IRQ_SVD_HVD */                 E_IRQEXPL_HVD,

    /* E_INT_IRQ_USB2 */                    E_IRQEXPL_USB2,

    /* E_INT_IRQ_UHC2 */                    E_IRQEXPL_UHC2,

    /* E_INT_IRQ_MIU */                     E_IRQEXPL_MIU,

    /* E_INT_IRQ_GDMA */                    E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_UART2 */                   E_IRQEXPL_UART2,

    /* E_INT_IRQ_UART1 */                   E_IRQEXPL_UART1,

    /* E_INT_IRQ_DEMOD */                   E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_MPIF */                    E_IRQEXPL_MPIF,

    /* E_INT_IRQ_JPD */                     E_IRQEXPL_JPD,

    /* E_INT_IRQ_AEON2HI */                 E_IRQEXPL_AEON2HI,

    /* E_INT_IRQ_BDMA0 */                   E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_BDMA1 */                   E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_OTG */                     E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_MVD_CHECKSUM_FAIL */       E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_TSP_CHECKSUM_FAIL */       E_IRQ_FIQ_NONE,





    /* 0x50 */

    /* E_INT_IRQ_CA_I3 */                   E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_HDMI_LEVEL */              E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_MIPS_WADR_ERR */           E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_RASP */                    E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_CA_SVP */                  E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_UART2MCU */                E_IRQEXPH_UART2MCU,

    /* E_INT_IRQ_URDMA2MCU */               E_IRQEXPH_URDMA2MCU,

    /* E_INT_IRQ_IIC1 */                    E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_HDCP */                    E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_DMA_WADR_ERR */            E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_UP_IRQ_UART_CA */          E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_UP_IRQ_EMM_ECM */          E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_ONIF */                    E_IRQ_ONIF,

    /* E_INT_IRQ_USB1 */                    E_IRQEXPL_USB1,

    /* E_INT_IRQ_UHC1 */                    E_IRQEXPL_UHC1,

    /* E_INT_IRQ_MFE */                     E_IRQEXPL_MFE,



    /* 0x60 */

    /* E_INT_FIQ_IR_INT_RC */               E_FIQEXPL_IR_INT_RC,

    /* E_INT_FIQ_HDMITX_IRQ_EDGE */         E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_UP_IRQ_UART_CA */          E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_UP_IRQ_EMM_ECM */          E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_PVR2MI_INT0 */             E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_PVR2MI_INT1 */             E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_8051_TO_AEON */            E_IRQ_FIQ_NONE,

    /* 0x67 Not Used */                     E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_AEON_TO_MIPS_VPE0 */       E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_AEON_TO_MIPS_VPE1 */       E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_AEON_TO_8051 */            E_IRQ_FIQ_NONE,

    /* 0x6B Not Used */                     E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_MIPS_VPE1_TO_MIPS_VPE0 */  E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_MIPS_VPE1_TO_AEON */       E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_MIPS_VPE1_TO_8051 */       E_IRQ_FIQ_NONE,

    /* 0x6F Not Used */                     E_IRQ_FIQ_NONE,



    /* 0x70 */

    /* E_INT_FIQ_MIPS_VPE0_TO_MIPS_VPE1 */  E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_MIPS_VPE0_TO_AEON */       E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_MIPS_VPE0_TO_8051 */       E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_IR_IN */                   E_FIQEXPL_IR_IN,

    /* E_INT_FIQ_DMDMCU2HK */               E_FIQ_DMDMCU2HK,

    /* E_INT_FIQ_R2TOMCU_INT0 */            E_FIQ_AEON_MB2_MCU0,

    /* E_INT_FIQ_R2TOMCU_INT1 */            E_FIQ_AEON_MB2_MCU1,

    /* E_INT_FIQ_DSPTOMCU_INT0 */           E_FIQ_DSP2_MB2_MCU0,

    /* E_INT_FIQ_DSPTOMCU_INT1 */           E_FIQ_DSP2_MB2_MCU1,

    /* E_INT_FIQ_USB */                     E_FIQ_USB,

    /* E_INT_FIQ_UHC */                     E_FIQ_UHC,

    /* E_INT_FIQ_USB1 */                    E_FIQEXPH_USB1,

    /* E_INT_FIQ_UHC1 */                    E_FIQEXPH_UHC1,

    /* E_INT_FIQ_USB2 */                    E_FIQEXPH_USB2,

    /* E_INT_FIQ_UHC2 */                    E_FIQEXPH_UHC2,

    /* 0x7F Not Used */                     E_IRQ_FIQ_NONE,



    /* 0x80 */

    /* E_INT_IRQ_MLINK */                   E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_AFEC */                    E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_DPTX */                    E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_TMDDRLINK */               E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_DISPI */                   E_IRQEXPL_DISPI,

    /* E_INT_IRQ_EXP_MLINK */               E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_M4VE */                    E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_DVI_HDMI_HDCP */           E_IRQEXPH_DVI_HDMI_HDCP,

    /* E_INT_IRQ_G3D2MCU */                 E_IRQEXPH_G3D2MCU,

    /* E_INT_IRQ_VP6 */                     E_IRQ_VP6,

    /* 0x8A Not Used */                     E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_CEC */                     E_IRQEXPH_CEC,

    /* E_INT_IRQ_HDCP_IIC */                E_IRQEXPH_HDCP_IIC,

    /* E_INT_IRQ_HDCP_X74 */                E_IRQEXPH_HDCP_X74,

    /* E_INT_IRQ_WADR_ERR */                E_IRQEXPH_WADR_ERR,

    /* E_INT_IRQ_DCSUB */                   E_IRQEXPH_DCSUB,



    /* 0x90 */

    /* E_INT_IRQ_GE */                      E_IRQEXPH_GE,

    /* 0x91 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0x92 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0x93 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0x94 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0x95 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0x96 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0x97 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0x98 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0x99 Not Used */                     E_IRQ_SMART,

    /* 0x9A Not Used */                     E_IRQ_MVD2MIPS,

    /* 0x9B Not Used */                     E_IRQ_GPD,

    /* 0x9C Not Used */                     E_IRQ_FIQ_NONE,

    /* 0x9D Not Used */                     E_IRQ_FIQ_NONE,

    /* 0x9E Not Used */                     E_IRQ_MIIC_DMA_INT3,

    /* 0x9F Not Used */                     E_IRQ_MIIC_INT3,





    /* 0xA0 */

    /* 0xA0 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xA1 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xA2 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xA3 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xA4 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xA5 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xA6 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xA7 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xA8 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xA9 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xAA Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xAB Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xAC Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xAD Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xAE Not Used */                     E_IRQEXPL_MIIC_DMA_INT2,

    /* 0xAF Not Used */                     E_IRQEXPL_MIIC_INT2,



    /* 0xB0 */

    /* 0xB0 Not Used */                     E_IRQEXPH_MIIC_DMA_INT1,

    /* 0xB1 Not Used */                     E_IRQEXPH_MIIC_INT1,

    /* 0xB2 Not Used */                     E_IRQEXPH_MIIC_DMA_INT0,

    /* 0xB3 Not Used */                     E_IRQEXPH_MIIC_INT0,

    /* 0xB4 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xB5 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xB6 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xB7 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xB8 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xB9 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xBA Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xBB Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xBC Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xBD Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xBE Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xBF Not Used */                     E_IRQ_FIQ_NONE,



    /* 0xC0 */

    /* E_INT_FIQ_DMARD */                   E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_AU_DMA_BUF_INT */          E_FIQEXPL_AU_DMA_BUF_INT,

    /* E_INT_FIQ_8051_TO_MIPS_VPE1 */       E_IRQ_FIQ_NONE,

    /* 0xC3 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xC4 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xC5 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xC6 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xC7 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xC8 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xC9 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xCA Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xCB Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xCC Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xCD Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xCE Not Used */                     E_IRQ_FIQ_NONE,

    /* E_INT_FIQ_VP6 */                     E_FIQ_VP6,



    /* 0xD0 */

    /* 0xD0 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xD1 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xD2 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xD3 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xD4 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xD5 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xD6 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xD7 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xD8 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xD9 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xDA Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xDB Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xDC Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xDD Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xDE Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xDF Not Used */                     E_IRQ_FIQ_NONE,



    /* 0xE0 */

    /* 0xE0 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xE1 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xE2 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xE3 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xE4 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xE5 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xE6 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xE7 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xE8 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xE9 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xEA Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xEB Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xEC Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xED Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xEE Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xEF Not Used */                     E_IRQ_FIQ_NONE,



    /* 0xF0 */

    /* 0xF0 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xF1 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xF2 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xF3 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xF4 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xF5 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xF6 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xF7 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xF8 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xF9 Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xFA Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xFB Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xFC Not Used */                     E_IRQ_FIQ_NONE,

    /* 0xFD Not Used */                     E_IRQ_FIQ_NONE,

    /* E_IRQ_FIQ_NONE */                    E_IRQ_FIQ_NONE,

    /* E_INT_IRQ_FIQ_ALL */                 E_IRQ_FIQ_ALL



};



static MS_U8 HWIdx2IntEnum[128] =

{

    /* 0x00 */

    E_INT_IRQ_UART0,                        //E_IRQ_UART0,

    E_INT_IRQ_PMSLEEP,                      //E_IRQ_FIQ_NONE,

    E_INT_IRQ_ONIF,                         //E_IRQ_ONIF,

    E_INT_IRQ_MVD,                          //E_IRQ_MVD,

    E_INT_IRQ_PS,                           //E_IRQ_PS,

    E_INT_IRQ_NFIE,                         //E_IRQ_NFIE,

    E_INT_IRQ_USB,                          //E_IRQ_USB,

    E_INT_IRQ_UHC,                          //E_IRQ_UHC,

    E_INT_IRQ_EC_BRIDGE,                    //E_IRQ_EC_BRIDGE,

    E_INT_IRQ_EMAC,                         //E_IRQ_EMAC,

    E_INT_IRQ_DISP,                         //E_IRQ_DISP,

    E_INT_IRQ_FIQ_NONE,                     //E_IRQ_FRC_SC,  // A5

    E_INT_IRQ_MIIC_DMA_INT3,                //E_IRQ_MIIC_DMA_INT3, // A5

    E_INT_IRQ_MIIC_INT3,                    //E_IRQ_MIIC_INT3, // A5

    E_INT_IRQ_COMB,                         //E_IRQ_COMB,

    E_INT_IRQ_FIQ_NONE,                     //E_IRQ_FRC_INT_FIQ2HST0, // A5



    /* 0x10 */

    E_INT_IRQ_TSP2HK,                       //E_IRQ_TSP2HK,

    E_INT_IRQ_VE,                           //E_IRQ_VE,

    E_INT_IRQ_CIMAX2MCU,                    //E_IRQ_CIMAX2MCU,

    E_INT_IRQ_DC,                           //E_IRQ_DC,

    E_INT_IRQ_GOP,                          //E_IRQ_GOP,

    E_INT_IRQ_PCM,                          //E_IRQ_PCM,

    E_INT_IRQ_FIQ_NONE,                     //E_IRQ_IIC0,

    E_INT_IRQ_SMART,                        //E_IRQ_SMART,  // A5

    E_INT_IRQ_VP6,                          //E_IRQ_VP6,    // A3

    E_INT_IRQ_FIQ_NONE,                     //E_IRQ_PM,

    E_INT_IRQ_DDC2BI,                       //E_IRQ_DDC2BI,

    E_INT_IRQ_SCM,                          //E_IRQ_SCM,

    E_INT_IRQ_VBI,                          //E_IRQ_VBI,

    E_INT_IRQ_MVD2MIPS,                     //E_IRQ_MVD2MIPS, // A5

    E_INT_IRQ_GPD,                          //E_IRQ_GPD, // A5

    E_INT_IRQ_ADCDVI2RIU,                   //E_IRQ_ADCDVI2RIU,



    /* 0x20 */

    E_INT_FIQ_EXTIMER0,                     //E_FIQ_EXTIMER0,

    E_INT_FIQ_EXTIMER1,                     //E_FIQ_EXTIMER1,

    E_INT_FIQ_WDT,                          //E_FIQ_WDT,

    E_INT_FIQ_STRETCH,                     //E_IRQ_FIQ_NONE,

    E_INT_FIQ_R2TOMCU_INT0,                 //E_FIQ_AEON_MB2_MCU0

    E_INT_FIQ_R2TOMCU_INT1,                 //E_FIQ_AEON_MB2_MCU1

    E_INT_FIQ_DSPTOMCU_INT0,                //E_FIQ_DSP2_MB2_MCU0

    E_INT_FIQ_DSPTOMCU_INT1,                //E_FIQ_DSP2_MB2_MCU1

    E_INT_FIQ_USB,                          //E_FIQ_USB

    E_INT_FIQ_UHC,                          //E_FIQ_UHC

    E_INT_FIQ_VP6,                          //E_FIQ_VP6,    // A3

    E_INT_FIQ_HDMI_NON_PCM,                 //E_FIQ_HDMI_NON_PCM,

    E_INT_FIQ_SPDIF_IN_NON_PCM,             //E_FIQ_SPDIF_IN_NON_PCM,

    E_INT_FIQ_EMAC,                         //E_FIQ_EMAC

    E_INT_FIQ_SE_DSP2UP,                    //E_FIQ_SE_DSP2UP,

    E_INT_FIQ_TSP2AEON,                     //E_FIQ_TSP2AEON,



    /* 0x30 */

    E_INT_FIQ_VIVALDI_STR,                  //E_FIQ_VIVALDI_STR,

    E_INT_FIQ_VIVALDI_PTS,                  //E_FIQ_VIVALDI_PTS,

    E_INT_FIQ_DSP_MIU_PROT,                 //E_FIQ_DSP_MIU_PROT,

    E_INT_FIQ_XIU_TIMEOUT,                  //E_FIQ_XIU_TIMEOUT,

    E_INT_FIQ_DMDMCU2HK,                    //E_FIQ_DMDMCU2HK,

    E_INT_FIQ_VSYNC_VE4VBI,                 //E_FIQ_VSYNC_VE4VBI,

    E_INT_FIQ_FIELD_VE4VBI,                 //E_FIQ_FIELD_VE4VBI,

    E_INT_FIQ_VDMCU2HK,                     //E_FIQ_VDMCU2HK,

    E_INT_FIQ_VE_DONE_TT,                   //E_FIQ_VE_DONE_TT,

    E_INT_IRQ_FIQ_NONE,                     //E_FIQ_INT_CCFL,

    E_INT_IRQ_FIQ_NONE,                     //E_FIQ_INT,

    E_INT_FIQ_IR,                           //E_FIQ_IR,

    E_INT_FIQ_AFEC_VSYNC,                   //E_FIQ_AFEC_VSYNC,

    E_INT_FIQ_DEC_DSP2UP,                   //E_FIQ_DEC_DSP2UP,

    E_INT_FIQ_AEON_TO_BEON,                 //E_FIQ_AEON_TO_BEON, //### For MAILBOX

    E_INT_FIQ_DEC_DSP2MIPS,                 //E_FIQ_DEC_DSP2MIPS,



    /* 0x40 */

    E_INT_IRQ_SVD_HVD,                      //E_IRQEXPL_HVD,

    E_INT_IRQ_USB1,                         //E_IRQEXPL_USB1,

    E_INT_IRQ_UHC1,                         //E_IRQEXPL_UHC1,

    E_INT_IRQ_MIU,                          //E_IRQEXPL_MIU,

    E_INT_IRQ_USB2,                         //E_IRQEXPL_USB2,

    E_INT_IRQ_UHC2,                         //E_IRQEXPL_UHC2,

    E_INT_IRQ_AEON2HI,                      //E_IRQEXPL_AEON2HI,

    E_INT_IRQ_UART1,                        //E_IRQEXPL_UART1,

    E_INT_IRQ_UART2,                        //E_IRQEXPL_UART2,

    E_INT_IRQ_FIQ_NONE,                     //E_IRQEXPL_FRC_INT_IRQ2HST0, // A5

    E_INT_IRQ_MPIF,                         //E_IRQEXPL_MPIF,

    E_INT_IRQ_MIIC_DMA_INT2,                //E_IRQEXPL_MIIC_DMA_INT2,    // A5

    E_INT_IRQ_MIIC_INT2,                    //E_IRQEXPL_MIIC_INT2,        // A5

    E_INT_IRQ_JPD,                          //E_IRQEXPL_JPD,

    E_INT_IRQ_DISPI,                        //E_IRQEXPL_DISPI,

    E_INT_IRQ_MFE,                          //E_IRQEXPL_MFE,



    /* 0x50 */

    E_INT_IRQ_BDMA0,                        //E_IRQEXPH_BDMA0,

    E_INT_IRQ_BDMA1,                        //E_IRQEXPH_BDMA1,

    E_INT_IRQ_UART2MCU,                     //E_IRQEXPH_UART2MCU,

    E_INT_IRQ_URDMA2MCU,                    //E_IRQEXPH_URDMA2MCU,

    E_INT_IRQ_DVI_HDMI_HDCP,                //E_IRQEXPH_DVI_HDMI_HDCP,

    E_INT_IRQ_G3D2MCU,                      //E_IRQEXPH_G3D2MCU,

    E_INT_IRQ_CEC,                          //E_IRQEXPH_CEC

    E_INT_IRQ_HDCP_IIC,                     //E_IRQEXPH_HDCP_IIC

    E_INT_IRQ_HDCP_X74,                     //E_IRQEXPH_HDCP_X74

    E_INT_IRQ_WADR_ERR,                     //E_IRQEXPH_WADR_ERR

    E_INT_IRQ_DCSUB,                        //E_IRQEXPH_DCSUB

    E_INT_IRQ_GE,                           //E_IRQEXPH_GE

    E_INT_IRQ_MIIC_DMA_INT1,                //E_IRQ_FIQ_NONE, A5

    E_INT_IRQ_MIIC_INT1,                    //E_IRQ_FIQ_NONE, A5

    E_INT_IRQ_MIIC_DMA_INT0,                //E_IRQ_FIQ_NONE, A5

    E_INT_IRQ_MIIC_INT0,                    //E_IRQ_FIQ_NONE, A5



    /* 0x60 */

    E_INT_FIQ_IR_INT_RC,                    //E_FIQEXPL_IR_INT_RC,

    E_INT_FIQ_AU_DMA_BUF_INT,               //E_FIQEXPL_AU_DMA_BUF_INT,

    E_INT_FIQ_IR_IN,                        //E_FIQEXPL_IR_IN

    E_INT_IRQ_FIQ_NONE,                     //E_IRQ_FIQ_NONE,

    E_INT_IRQ_FIQ_NONE,                     //E_FIQEXPL_8051_TO_MIPS_VPE0,

    E_INT_IRQ_FIQ_NONE,                     //E_FIQEXPL_8051_TO_MIPS_VPE1,

    E_INT_FIQ_8051_TO_BEON,                 //E_FIQ_8051_TO_BEON, //### For MAILBOX

    E_INT_FIQ_GPIO0,                        //E_IRQ_FIQ_NONE,

    E_INT_IRQ_FIQ_NONE,                     //E_FIQEXPL_AEON_TO_MIPS_VPE0,

    E_INT_IRQ_FIQ_NONE,                     //E_FIQEXPL_AEON_TO_MIPS_VPE1,

    E_INT_FIQ_BEON_TO_8051,                 //E_FIQ_BEON_TO_8051, //### For MAILBOX

    E_INT_FIQ_GPIO1,                        //E_IRQ_FIQ_NONE,

    E_INT_IRQ_FIQ_NONE,                     //E_FIQEXPL_MIPS_VPE1_TO_MIPS_VPE0,

    E_INT_IRQ_FIQ_NONE,                     //E_FIQEXPL_MIPS_VPE1_TO_AEON,

    E_INT_IRQ_FIQ_NONE,                     //E_FIQEXPL_MIPS_VPE1_TO_8051,

    E_INT_FIQ_GPIO2,                        //E_IRQ_FIQ_NONE,



    /* 0x70 */

    E_INT_IRQ_FIQ_NONE,                     //E_FIQEXPH_MIPS_VPE0_TO_MIPS_VPE1,

    E_INT_IRQ_FIQ_NONE,                     //E_FIQEXPH_MIPS_VPE0_TO_AEON,

    E_INT_IRQ_FIQ_NONE,                     //E_FIQEXPH_MIPS_VPE0_TO_8051,

    E_INT_FIQ_USB1,                         //E_FIQEXPH_USB1

    E_INT_FIQ_UHC1,                         //E_FIQEXPH_UHC1

    E_INT_FIQ_USB2,                         //E_FIQEXPH_USB2

    E_INT_FIQ_UHC2,                         //E_FIQEXPH_UHC2

    E_INT_FIQ_GPIO3,

    E_INT_FIQ_GPIO4,

    E_INT_FIQ_GPIO5,

    E_INT_FIQ_GPIO6,

    E_INT_FIQ_PWM_RP_RP_L,

    E_INT_FIQ_PWM_RP_FP_L,

    E_INT_FIQ_PWM_RP_RP_R,

    E_INT_FIQ_PWM_RP_FP_R,

    E_INT_FIQ_GPIO7,

};

//#else


//#ifdef MSOS_TYPE_CE
//#else
//#error
//#endif



//#endif

#ifdef __cplusplus

}

#endif



#endif // _HAL_IRQTBL_H_



