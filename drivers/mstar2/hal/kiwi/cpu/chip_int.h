#ifndef __CHIP_INT_H__
#define __CHIP_INT_H__

#define MSTAR_CHIP_INT_END 128//64

#define REG_INT_BASE                0xbf203200	// Bank: 0x1019

//IRQ registers                                 
#define REG_IRQ_MASK_L              (REG_INT_BASE+ (0x0054<< 2))
#define REG_IRQ_MASK_H              (REG_INT_BASE+ (0x0055<< 2))
#define REG_IRQ_PENDING_L           (REG_INT_BASE+ (0x005c<< 2))
#define REG_IRQ_PENDING_H           (REG_INT_BASE+ (0x005d<< 2))
                                                              
//IRQ EXP registers                                           
#define REG_IRQ_EXP_MASK_L          (REG_INT_BASE+ (0x0056<< 2))
#define REG_IRQ_EXP_MASK_H          (REG_INT_BASE+ (0x0057<< 2))
#define REG_IRQ_EXP_PENDING_L       (REG_INT_BASE+ (0x005e<< 2))
#define REG_IRQ_EXP_PENDING_H       (REG_INT_BASE+ (0x005f<< 2))
                                                              
//FIQ registers                                               
#define REG_FIQ_MASK_L              (REG_INT_BASE+ (0x0044<< 2))
#define REG_FIQ_MASK_H              (REG_INT_BASE+ (0x0045<< 2))
#define REG_FIQ_CLEAR_L             (REG_INT_BASE+ (0x004c<< 2))
#define REG_FIQ_CLEAR_H             (REG_INT_BASE+ (0x004d<< 2))
#define REG_FIQ_PENDING_L           (REG_INT_BASE+ (0x004c<< 2))
#define REG_FIQ_PENDING_H           (REG_INT_BASE+ (0x004d<< 2))
                                                              
//FIQ EXP registers                                           
#define REG_FIQ_EXP_MASK_L          (REG_INT_BASE+ (0x0046<< 2))
#define REG_FIQ_EXP_MASK_H          (REG_INT_BASE+ (0x0047<< 2))
#define REG_FIQ_EXP_CLEAR_L         (REG_INT_BASE+ (0x004e<< 2))
#define REG_FIQ_EXP_CLEAR_H         (REG_INT_BASE+ (0x004f<< 2))
#define REG_FIQ_EXP_PENDING_L       (REG_INT_BASE+ (0x004e<< 2))
#define REG_FIQ_EXP_PENDING_H       (REG_INT_BASE+ (0x004f<< 2))

#define MSTAR_INT_BASE                          8
/*******************************************************/
/*   THE IRQ AND FIQ ARE NOT COMPLETED.                */
/*   FOR EACH IP OWNER, PLEASE REVIEW IT BY YOURSELF   */
/*******************************************************/
typedef enum
{
	// IRQ
	E_IRQL_START								= 0 + MSTAR_INT_BASE,
	E_IRQ_UART0									= E_IRQL_START + 0,
	E_IRQ_IIC1									= E_IRQL_START + 1,
	E_IRQ_MVD2MIPS								= E_IRQL_START + 2,
	E_IRQ_MVD									= E_IRQL_START + 3,
	E_IRQ_PS									= E_IRQL_START + 4,     
	E_IRQ_IIC2									= E_IRQL_START + 5,     
	E_IRQ_USB									= E_IRQL_START + 6,
	E_IRQ_UHC									= E_IRQL_START + 7,
	E_IRQ_EC_BRIDGE								= E_IRQL_START + 8,
	E_IRQ_AESDMA_S								= E_IRQL_START + 9,
	E_IRQ_DISP									= E_IRQL_START + 10,
	E_IRQ_DIPW									= E_IRQL_START + 11,
	E_IRQ_MSPI									= E_IRQL_START + 12,
	E_IRQ_FIQ_INT									= E_IRQL_START + 13,
	E_IRQ_NFIE									= E_IRQL_START + 14,
	E_IRQ_EMAC									= E_IRQL_START + 15,
	E_IRQL_END									= 15 + MSTAR_INT_BASE,

	E_IRQH_START								= 16 + MSTAR_INT_BASE,
	E_IRQ_TSP2HK								= E_IRQH_START + 0,
	E_IRQ_VE									= E_IRQH_START + 1,
	E_IRQ_USBP1									= E_IRQH_START + 2,
	E_IRQ_DC									= E_IRQH_START + 3,
	E_IRQ_GOP									= E_IRQH_START + 4,
	E_IRQ_UHC1									= E_IRQH_START + 5,
	E_IRQ_IIC0									= E_IRQH_START + 6,
	E_IRQ_RTC									= E_IRQH_START + 7,
	E_IRQ_KEYPAD								= E_IRQH_START + 8,
	E_IRQ_PM									= E_IRQH_START + 9,
	//RESERVE									= E_IRQH_START + 10,
	//RESERVE									= E_IRQH_START + 11,
	//RESERVE									= E_IRQH_START + 12,
	E_IRQ_RTC1									= E_IRQH_START + 13,
	//RESERVE									= E_IRQH_START + 14,
	//RESERVE									= E_IRQH_START + 15,
	E_IRQH_END									= 31 + MSTAR_INT_BASE,

	// FIQ
	E_FIQL_START								= 32 + MSTAR_INT_BASE,
	E_FIQ_EXTIMER0								= E_FIQL_START + 0,
	E_FIQ_EXTIMER1								= E_FIQL_START + 1,
	E_FIQ_WDT									= E_FIQL_START + 2,
	E_FIQ_AU_SPDIF_TX_CS0						= E_FIQL_START + 3,
	E_FIQ_AU_SPDIF_TX_CS1						= E_FIQL_START + 4,
	//RESERVE									= E_FIQL_START + 5,
	E_FIQ_MB_D2M_INT0							= E_FIQL_START + 6,
	E_FIQ_MB_D2M_INT1							= E_FIQL_START + 7,
	//RESERVE									= E_FIQL_START + 8,
	//RESERVE									= E_FIQL_START + 9,
	//RESERVE									= E_FIQL_START + 10,
	E_FIQ_HDMI_NON_PCM							= E_FIQL_START + 11,
	E_FIQ_SPDIF_IN_NON_PCM						= E_FIQL_START + 12,
	E_FIQ_IAN_EASD								= E_FIQL_START + 13,
	E_FIQ_SE_DSP2UP								= E_FIQL_START + 14,
	E_FIQ_TSP2AEON								= E_FIQL_START + 15,
	E_FIQL_END									= 47 + MSTAR_INT_BASE,

	E_FIQH_START								= 48 + MSTAR_INT_BASE,
	E_FIQ_VIVALDI_STR							= E_FIQH_START + 0,
	E_FIQ_VIVALDI_PTS							= E_FIQH_START + 1,
	E_FIQ_DSP_MIU_PROT							= E_FIQH_START + 2,
	E_FIQ_XIU_TIMEOUT							= E_FIQH_START + 3,
	E_FIQ_DMDMCU2HK								= E_FIQH_START + 4,
	E_FIQ_VSYNC_VE4VBI							= E_FIQH_START + 5,
	E_FIQ_FIELD_VE4VBI							= E_FIQH_START + 6,
	//RESERVE									= E_FIQH_START + 7,
	E_FIQ_VE_DONE_TT							= E_FIQH_START + 8,
	//RESERVE									= E_FIQH_START + 9,
	//RESERVE									= E_FIQH_START + 10,
	E_FIQ_IR									= E_FIQH_START + 11,
	//RESERVE									= E_FIQH_START + 12,
	E_FIQ_DEC_DSP2UP							= E_FIQH_START + 13,
	//RESERVE									= E_FIQH_START + 14,
	E_FIQ_DEC_DSP2MIPS							= E_FIQH_START + 15,
	E_FIQH_END									= 63 + MSTAR_INT_BASE,

	//IRQEXP
	E_IRQEXPL_START								= 64 + MSTAR_INT_BASE,
	E_IRQEXPL_HVD								= E_IRQEXPL_START + 0,
	E_IRQEXPL_EVD								= E_IRQEXPL_START + 1,
	//RESERVE									= E_IRQEXPL_START + 2,
	E_IRQEXPL_MIU								= E_IRQEXPL_START + 3,
	E_IRQEXPL_ERROR_RESP									= E_IRQEXPL_START + 4,
	//RESERVE									= E_IRQEXPL_START + 5,
	E_IRQEXPL_IRQ_AEON2HI						= E_IRQEXPL_START + 6,
	E_IRQEXPL_UART1								= E_IRQEXPL_START + 7,
	E_IRQEXPL_UART3								= E_IRQEXPL_START + 8,
	E_IRQEXPL_SAR								= E_IRQEXPL_START + 9,
	//RESERVE									= E_IRQEXPL_START + 10,
	//RESERVE									= E_IRQEXPL_START + 11,
	//RESERVE									= E_IRQEXPL_START + 12,
	E_IRQEXPL_JPD								= E_IRQEXPL_START + 13,
	//RESERVE									= E_IRQEXPL_START + 14,
	//RESERVE									= E_IRQEXPL_START + 15,
	E_IRQEXPL_END								= 79 + MSTAR_INT_BASE,

	E_IRQEXPH_START								= 80 + MSTAR_INT_BASE,
	E_IRQEXPH_BDMA0								= E_IRQEXPH_START + 0,
	E_IRQEXPH_BDMA1								= E_IRQEXPH_START + 1,
	E_IRQEXPH_UART2MCU							= E_IRQEXPH_START + 2,
	E_IRQEXPH_URDMA2MCU							= E_IRQEXPH_START + 3,
	//RESERVE									= E_IRQEXPH_START + 4,
	E_IRQEXPH_CEC								= E_IRQEXPH_START + 5,
	E_IRQEXPH_HDMI_LEVEL						= E_IRQEXPH_START + 6,
	//RESERVE									= E_IRQEXPH_START + 7,
	//RESERVE									= E_IRQEXPH_START + 8,
	E_IRQEXPH_DMAWADR_ERR						= E_IRQEXPH_START + 9,
	E_IRQEXPH_AUDMA_V2							= E_IRQEXPH_START + 10,
	E_IRQEXPH_PAS_PTS_INTRL_COMBINE				= E_IRQEXPH_START + 11,
	E_IRQEXPH_MIU_SECURITY									= E_IRQEXPH_START + 12,
	//RESERVE								= E_IRQEXPH_START + 13,
	E_IRQEXPH_IRQ_OUT_PLUG_DET					= E_IRQEXPH_START + 14,
	E_IRQEXPH_FRM_PM							= E_IRQEXPH_START + 15,
	E_IRQEXPH_END								= 95 + MSTAR_INT_BASE,

	// FIQEXP
	E_FIQEXPL_START								= 96 + MSTAR_INT_BASE,
	E_FIQEXPL_IR_INT_RC							= E_FIQEXPL_START + 0,
	E_FIQEXPL_AU_DMA_BUF_INT					= E_FIQEXPL_START + 1,
	E_FIQEXPL_VE_SW_WR2BUF						= E_FIQEXPL_START + 2,
	//RESERVE									= E_FIQEXPL_START + 3,
	E_FIQEXPL_REG_HST0to3						= E_FIQEXPL_START + 4,
	E_FIQEXPL_REG_HST0to2						= E_FIQEXPL_START + 5,
	E_FIQEXPL_REG_HST0to1						= E_FIQEXPL_START + 6,
	//RESERVE									= E_FIQEXPL_START + 7,
	E_FIQEXPL_REG_HST1to3						= E_FIQEXPL_START + 8,
	E_FIQEXPL_REG_HST1to2						= E_FIQEXPL_START + 9,
	E_FIQEXPL_REG_HST1to0						= E_FIQEXPL_START + 10,
	//RESERVE									= E_FIQEXPL_START + 11,
	E_FIQEXPL_REG_HST2to3						= E_FIQEXPL_START + 12,
	E_FIQEXPL_REG_HST2to1						= E_FIQEXPL_START + 13,
	E_FIQEXPL_REG_HST2to0    					= E_FIQEXPL_START + 14,
	//RESERVE									= E_FIQEXPL_START + 15,
	E_FIQEXPL_END								= 111 + MSTAR_INT_BASE,

	E_FIQEXPH_START								= 112 + MSTAR_INT_BASE,
	E_FIQEXPL_REG_MIPS1toMIPS0					= E_FIQEXPH_START + 0,
	E_FIQEXPL_REG_HST3to1						= E_FIQEXPH_START + 1,
	E_FIQEXPL_REG_HST3to0						= E_FIQEXPH_START + 2,
	//RESERVE									= E_FIQEXPH_START + 3,
	//RESERVE									= E_FIQEXPH_START + 4,
	//RESERVE									= E_FIQEXPH_START + 5,
	E_FIQEXPH_HDMITX_EDGE						= E_FIQEXPH_START + 6,
	E_FIQEXPH_MB_AUR2toMCU0						= E_FIQEXPH_START + 7,
	E_FIQEXPH_MB_AUR2toMCU1						= E_FIQEXPH_START + 8,
	E_FIQEXPH_MB_AUR2toMCU2						= E_FIQEXPH_START + 9,
	//RESERVE									= E_FIQEXPH_START + 10,
	//RESERVE									= E_FIQEXPH_START + 11,
	//RESERVE									= E_FIQEXPH_START + 12,
	//RESERVE									= E_FIQEXPH_START + 13,
	E_FIQEXPH_MB_AUR2toMCU3						= E_FIQEXPH_START + 14,
	E_FIQEXPH_FRM_PM							= E_FIQEXPH_START + 15,
	E_FIQEXPH_END								= 127 + MSTAR_INT_BASE,
 
	E_IRQ_FIQ_ALL								= 0xFF //all IRQs & FIQs
} InterruptNum;

// FIQL
#define FIQL_MASK                               0xFFFF
	#define FIQ_EXTIMER0			(0x1 << (E_FIQ_EXTIMER0			- E_FIQL_START) )
	#define FIQ_EXTIMER1			(0x1 << (E_FIQ_EXTIMER1			- E_FIQL_START) )
	#define FIQ_WDT					(0x1 << (E_FIQ_WDT				- E_FIQL_START) )
	#define FIQ_AU_SPDIF_TX_CS0		(0x1 << (E_FIQ_AU_SPDIF_TX_CS0	- E_FIQL_START) )
	#define FIQ_AU_SPDIF_TX_CS1		(0x1 << (E_FIQ_AU_SPDIF_TX_CS1	- E_FIQL_START) )
	#define FIQ_MB_D2M_INT0			(0x1 << (E_FIQ_MB_D2M_INT0		- E_FIQL_START) )
	#define FIQ_MB_D2M_INT1			(0x1 << (E_FIQ_MB_D2M_INT1		- E_FIQL_START) )
	#define FIQ_HDMI_NON_PCM		(0x1 << (E_FIQ_HDMI_NON_PCM		- E_FIQL_START) )
	#define FIQ_SPDIF_IN_NON_PCM	(0x1 << (E_FIQ_SPDIF_IN_NON_PCM	- E_FIQL_START) )
	#define FIQ_IAN_EASD			(0x1 << (E_FIQ_IAN_EASD			- E_FIQL_START) )
	#define FIQ_SE_DSP2UP			(0x1 << (E_FIQ_SE_DSP2UP		- E_FIQL_START) )
	#define FIQ_TSP2AEON			(0x1 << (E_FIQ_TSP2AEON			- E_FIQL_START) )

// FIQH
#define FIQH_MASK                               0xFFFF
	#define FIQ_VIVALDI_STR			(0x1 << (E_FIQ_VIVALDI_STR		- E_FIQH_START) )
	#define FIQ_VIVALDI_PTS			(0x1 << (E_FIQ_VIVALDI_PTS		- E_FIQH_START) )
	#define FIQ_DSP_MIU_PROT		(0x1 << (E_FIQ_DSP_MIU_PROT		- E_FIQH_START) )
	#define FIQ_XIU_TIMEOUT			(0x1 << (E_FIQ_XIU_TIMEOUT		- E_FIQH_START) )
	#define FIQ_DMDMCU2HK			(0x1 << (E_FIQ_DMDMCU2HK		- E_FIQH_START) )
	#define FIQ_VSYNC_VE4VBI		(0x1 << (E_FIQ_VSYNC_VE4VBI		- E_FIQH_START) )
	#define FIQ_FIELD_VE4VBI		(0x1 << (E_FIQ_FIELD_VE4VBI		- E_FIQH_START) )
	#define FIQ_VE_DONE_TT			(0x1 << (E_FIQ_VE_DONE_TT		- E_FIQH_START) )
	#define FIQ_IR					(0x1 << (E_FIQ_IR				- E_FIQH_START) )
	#define FIQ_DEC_DSP2UP			(0x1 << (E_FIQ_DEC_DSP2UP		- E_FIQH_DEC_START) )
	#define FIQ_DEC_DSP2MIPS		(0x1 << (E_FIQ_DEC_DSP2MIPS		- E_FIQH_DEC_START) )

// IRQL
#define IRQL_MASK                               0xFFFF
	#define IRQ_UART0		(0x1 << (E_IRQ_UART0		- E_IRQL_START) )
	#define IRQ_IIC1		(0x1 << (E_IRQ_IIC1		- E_IRQL_START) )
	#define IRQ_MVD2MIPS	(0x1 << (E_IRQ_MVD2MIPS	- E_IRQL_START) )
	#define IRQ_MVD			(0x1 << (E_IRQ_MVD		- E_IRQL_START) )
	#define IRQ_PS			(0x1 << (E_IRQ_PS		- E_IRQL_START) )
	#define IRQ_IIC2		(0x1 << (E_IRQ_IIC2		- E_IRQL_START) )
	#define IRQ_USB			(0x1 << (E_IRQ_USB		- E_IRQL_START) )
	#define IRQ_UHC			(0x1 << (E_IRQ_UHC		- E_IRQL_START) )
	#define IRQ_EC_BRIDGE	(0x1 << (E_IRQ_EC_BRIDGE	- E_IRQL_START) )
	#define IRQ_AESDMA_S	(0x1 << (E_IRQ_AESDMA_S	- E_IRQL_START) )
	#define IRQ_DISP		(0x1 << (E_IRQ_DISP		- E_IRQL_START) )
	#define IRQ_DIPW		(0x1 << (E_IRQ_DIPW		- E_IRQL_START) )
	#define IRQ_MSPI		(0x1 << (E_IRQ_MSPI		- E_IRQL_START) )
	#define IRQ_FIG			(0x1 << (E_IRQ_FIG		- E_IRQL_START) )
	#define IRQ_NFIE		(0x1 << (E_IRQ_NFIE		- E_IRQL_START) )
	#define IRQ_EMAC		(0x1 << (E_IRQ_EMAC		- E_IRQL_START) )                                         

// IRQH
#define IRQH_MASK                               0xFFFF
	#define IRQ_TSP2HK		(0x1 << (E_IRQ_TSP2HK	- E_IRQH_START) )
	#define IRQ_VE			(0x1 << (E_IRQ_VE		- E_IRQH_START) )
	#define IRQ_USBP1		(0x1 << (E_IRQ_USBP1	- E_IRQH_START) )
	#define IRQ_DC			(0x1 << (E_IRQ_DC		- E_IRQH_START) )
	#define IRQ_GOP			(0x1 << (E_IRQ_GOP		- E_IRQH_START) )
	#define IRQ_UHC1		(0x1 << (E_IRQ_UHC1		- E_IRQH_START) )
	#define IRQ_IIC0		(0x1 << (E_IRQ_IIC0		- E_IRQH_START) )
	#define IRQ_RTC			(0x1 << (E_IRQ_RTC		- E_IRQH_START) )
	#define IRQ_KEYPAD		(0x1 << (E_IRQ_KEYPAD	- E_IRQH_START) )
	#define IRQ_PM			(0x1 << (E_IRQ_PM		- E_IRQH_START) )
	#define IRQ_RTC1		(0x1 << (E_IRQ_RTC1		- E_IRQH_START) ) 

// IRQEXPL
#define IRQEXPL_MASK                            0xFFFF
	#define IRQEXPL_HVD			(0x1 << (E_IRQEXPL_HVD			- E_IRQEXPL_START) )
	#define IRQEXPL_EVD			(0x1 << (E_IRQEXPL_EVD			- E_IRQEXPL_START) )
	#define IRQEXPL_MIU			(0x1 << (E_IRQEXPL_MIU			- E_IRQEXPL_START) )
	#define IRQEXPL_IRQ_AEON2HI	(0x1 << (E_IRQEXPL_IRQ_AEON2HI	- E_IRQEXPL_START) )
	#define IRQEXPL_UART1		(0x1 << (E_IRQEXPL_UART1		- E_IRQEXPL_START) )
	#define IRQEXPL_UART3		(0x1 << (E_IRQEXPL_UART3		- E_IRQEXPL_START) )
	#define IRQEXPL_SAR			(0x1 << (E_IRQEXPL_SAR			- E_IRQEXPL_START) )
	#define IRQEXPL_JPD			(0x1 << (E_IRQEXPL_JPD			- E_IRQEXPL_START) )
    
// IRQEXPH
#define IRQEXPH_MASK                            0xFFFF
	#define IRQEXPH_BDMA0					(0x1 << (E_IRQEXPH_BDMA0				- E_IRQEXPL_START) )
	#define IRQEXPH_BDMA1					(0x1 << (E_IRQEXPH_BDMA1				- E_IRQEXPH_START) )
	#define IRQEXPH_UART2MCU				(0x1 << (E_IRQEXPH_UART2MCU				- E_IRQEXPH_START) )
	#define IRQEXPH_URDMA2MCU				(0x1 << (E_IRQEXPH_URDMA2MCU			- E_IRQEXPH_START) )
	#define IRQEXPH_CEC						(0x1 << (E_IRQEXPH_CEC					- E_IRQEXPH_START) )
	#define IRQEXPH_HDMI_LEVEL				(0x1 << (E_IRQEXPH_HDMI_LEVEL			- E_IRQEXPH_START) )
	#define IRQEXPH_DMAWADR_ERR				(0x1 << (E_IRQEXPH_DMAWADR_ERR			- E_IRQEXPH_START) )
	#define IRQEXPH_AUDMA_V2				(0x1 << (E_IRQEXPH_AUDMA_V2				- E_IRQEXPH_START) )
	#define IRQEXPH_PAS_PTS_INTRL_COMBINE	(0x1 << (E_IRQEXPH_PAS_PTS_INTRL_COMBINE- E_IRQEXPH_START) )
	#define IRQEXPH_RASP					(0x1 << (E_IRQEXPH_RASP					- E_IRQEXPH_START) )
	#define IRQEXPH_IRQ_OUT_PLUG_DET		(0x1 << (E_IRQEXPH_IRQ_OUT_PLUG_DET		- E_IRQEXPH_START) )
	#define IRQEXPH_FRM_PM					(0x1 << (E_IRQEXPH_FRM_PM				- E_IRQEXPH_START) )
                                                
#define FIQEXPL_MASK                            0xFFFF
	#define FIQEXPL_IR_INT_RC			(0x1 << (E_FIQEXPL_IR_INT_RC		- E_FIQEXPL_START) )
	#define FIQEXPL_AU_DMA_BUF_INT		(0x1 << (E_FIQEXPL_AU_DMA_BUF_INT	- E_FIQEXPL_START) )
	#define FIQEXPL_VE_SW_WR2BUF		(0x1 << (E_FIQEXPL_VE_SW_WR2BUF		- E_FIQEXPL_START) )
	#define FIQEXPL_REG_HST0to3			(0x1 << (E_FIQEXPL_REG_HST0to3		- E_FIQEXPL_START) )
	#define FIQEXPL_REG_HST0to2			(0x1 << (E_FIQEXPL_REG_HST0to2		- E_FIQEXPL_START) )
	#define FIQEXPL_REG_HST0to1			(0x1 << (E_FIQEXPL_REG_HST0to1		- E_FIQEXPL_START) )
	#define FIQEXPL_REG_HST1to3			(0x1 << (E_FIQEXPL_REG_HST1to3		- E_FIQEXPL_START) )
	#define FIQEXPL_REG_HST1to2			(0x1 << (E_FIQEXPL_REG_HST1to2		- E_FIQEXPL_START) )
	#define FIQEXPL_REG_HST1to0			(0x1 << (E_FIQEXPL_REG_HST1to0		- E_FIQEXPL_START) )
	#define FIQEXPL_REG_HST2to3			(0x1 << (E_FIQEXPL_REG_HST2to3		- E_FIQEXPL_START) )
	#define FIQEXPL_REG_HST2to1			(0x1 << (E_FIQEXPL_REG_HST2to1		- E_FIQEXPL_START) )
	#define FIQEXPL_REG_MIPS0toMIPS1	(0x1 << (E_FIQEXPL_REG_MIPS0toMIPS1	- E_FIQEXPL_START) )
                                                
#define FIQEXPH_MASK                            0xFFFF
	#define FIQEXPL_REG_MIPS1toMIPS0	(0x1 << (E_FIQEXPL_REG_MIPS1toMIPS0	- E_FIQEXPH_START) )
	#define FIQEXPL_REG_HST3to1			(0x1 << (E_FIQEXPL_REG_HST3to1		- E_FIQEXPH_START) )
	#define FIQEXPL_REG_HST3to0			(0x1 << (E_FIQEXPL_REG_HST3to0		- E_FIQEXPH_START) )
	#define FIQEXPH_HDMITX_EDGE			(0x1 << (E_FIQEXPH_HDMITX_EDGE		- E_FIQEXPH_START) )
	#define FIQEXPH_MB_AUR2toMCU0		(0x1 << (E_FIQEXPH_MB_AUR2toMCU0	- E_FIQEXPH_START) )
	#define FIQEXPH_MB_AUR2toMCU1		(0x1 << (E_FIQEXPH_MB_AUR2toMCU1	- E_FIQEXPH_START) )
	#define FIQEXPH_MB_AUR2toMCU2		(0x1 << (E_FIQEXPH_MB_AUR2toMCU2	- E_FIQEXPH_START) )
	#define FIQEXPH_MB_AUR2toMCU3		(0x1 << (E_FIQEXPH_MB_AUR2toMCU3	- E_FIQEXPH_START) )
	#define FIQEXPH_FRM_PM				(0x1 << (E_FIQEXPH_FRM_PM			- E_FIQEXPH_START) )

#define IRQL_EXP_ALL                            0xFFFF
#define IRQH_EXP_ALL                            0xFFFF
#define FIQL_EXP_ALL                            0xFFFF
#define FIQH_EXP_ALL                            0xFFFF

#endif // #ifndef __CHIP_INT_H__
