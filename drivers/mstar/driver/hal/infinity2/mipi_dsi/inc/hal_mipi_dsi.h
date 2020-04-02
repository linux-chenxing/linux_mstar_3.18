////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2008 MStar Semiconductor, Inc.
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


/**
* @file     hal_mipi_dsi.h
* @version
* @Platform I2
* @brief    This file defines the HAL MIPI TX DSI interface
*
*/

#ifndef __HAL_MIPI_DSI_H__
#define __HAL_MIPI_DSI_H__

#define TRUE  1
#define FALSE 0

#define		DSI_DCS_MAXIMUM_RETURN_PACKET_SIZE	0x37
#define		DSI_DCS_SHORT_PACKET_ID_0			0x05
#define		DSI_DCS_SHORT_PACKET_ID_1			0x15
#define		DSI_DCS_LONG_PACKET_ID				0x39
#define		DSI_DCS_READ_PACKET_ID				0x06

#define		DSI_GERNERIC_SHORT_PACKET_ID_1		0x13
#define		DSI_GERNERIC_SHORT_PACKET_ID_2		0x23
#define		DSI_GERNERIC_LONG_PACKET_ID			0x29
#define		DSI_GERNERIC_READ_LONG_PACKET_ID	0x14

// ESD recovery method for video mode LCM
#define		METHOD_NONCONTINUOUS_CLK			(0x1)
#define		METHOD_BUS_TURN_AROUND				(0x2)

#define		DSI_VDO_VSA_VS_STATE				(0x008)
#define		DSI_VDO_VSA_HS_STATE				(0x010)
#define		DSI_VDO_VSA_VE_STATE				(0x020)
#define		DSI_VDO_VBP_STATE					(0x040)
#define		DSI_VDO_VACT_STATE			        (0x080)
#define		DSI_VDO_VFP_STATE					(0x100)



typedef enum _DSI_TOP_CLK_SEL {
       DSI_TOP_CLK_LPLL,
	DSI_TOP_CLK_MPLL_108M,
	DSI_TOP_CLK_MPLL_144M,
	DSI_TOP_CLK_MPLL_160M,
	DSI_TOP_CLK_MPLL_216M,
	DSI_TOP_CLK_MPLL_240M
} E_DSI_TOP_CLK_SEL;


typedef enum
{
	DSI_STATUS_OK = 0,
	DSI_STATUS_PACKET_TYPE_ERROR,

	DSI_STATUS_ERROR,
} DSI_STATUS;


typedef enum
{
	SHORT_PACKET_RW = 0,
	FB_WRITE 		= 1,
	LONG_PACKET_W	= 2,
	FB_READ 		= 3,
} DSI_INS_TYPE;


typedef enum
{
	DISABLE_BTA = 0,
	ENABLE_BTA 	= 1,
} DSI_CMDQ_BTA;

typedef enum
{
	LOW_POWER 	= 0,
	HIGH_SPEED 	= 1,
} DSI_CMDQ_HS;

typedef enum
{
	DISABLE_TE 	= 0,
	ENABLE_TE	= 1,
} DSI_CMDQ_TE;

typedef struct
{
	unsigned		type	: 2;
	unsigned		BTA		: 1;
	unsigned	 	HS		: 1;
	unsigned		CL		: 1;
	unsigned		TE		: 1;
	unsigned 		Rsv		: 1;
	unsigned		RPT		: 1;
} DSI_CMDQ_CONFG, *PDSI_CMDQ_CONFIG;


//Type0 Used for DSI short packet read/write command
typedef struct
{
	u8 CONFG		;
	u8 Data_ID		;
	u8 Data0			;
	u8 Data1			;
} DSI_T0_INS, *PDSI_T0_INS;


//Type2 Used for DSI generic long packet write command
typedef struct
{
	unsigned CONFG	:8;
	unsigned Data_ID		: 8;
	unsigned WC16			: 16;
	unsigned int *pdata;
} DSI_T2_INS, *PDSI_T2_INS;

//Type3 Used for DSI frame buffer read command (short packet)
typedef struct
{
	unsigned CONFG		:8;
	unsigned Data_ID		: 8;
	unsigned mem_start0		: 8;
	unsigned mem_start1		: 8;
} DSI_T3_INS, *PDSI_T3_INS;



typedef struct
{
	unsigned char byte0;
	unsigned char byte1;
	unsigned char byte2;
	unsigned char byte3;
} DSI_CMDQ, *PDSI_CMDQ;

typedef struct
{
	DSI_CMDQ data[32];
} DSI_CMDQ_REGS, *PDSI_CMDQ_REGS;


typedef struct
{
	unsigned TXDIV0			: 2;
	unsigned TXDIV1			: 2;
	unsigned FBK_SEL		: 2;
	unsigned FBK_DIV		: 7;
	unsigned PRE_DIV		: 2;
	unsigned RG_BR			: 2;
	unsigned RG_BC			: 2;
	unsigned RG_BIR			: 4;
	unsigned RG_BIC			: 4;
	unsigned RG_BP			: 4;
}DSI_PLL_CONFIG;


typedef struct
{
	unsigned char LPX;
	unsigned char HS_PRPR;
	unsigned char HS_ZERO;
	unsigned char HS_TRAIL;
} DSI_PHY_TIMCON0_REG, *PDSI_PHY_TIMCON0_REG;


typedef struct
{
	unsigned char TA_GO;
	unsigned char TA_SURE;
	unsigned char TA_GET;
	unsigned char DA_HS_EXIT;
} DSI_PHY_TIMCON1_REG, *PDSI_PHY_TIMCON1_REG;


typedef struct
{
	unsigned char CONT_DET;
	unsigned char RSV8;
	unsigned char CLK_ZERO;
	unsigned char CLK_TRAIL;
} DSI_PHY_TIMCON2_REG, *PDSI_PHY_TIMCON2_REG;


typedef struct
{
	unsigned char CLK_HS_PRPR;
	unsigned char CLK_HS_POST;
	unsigned char CLK_HS_EXIT;
	unsigned 	  rsv24		: 8;
} DSI_PHY_TIMCON3_REG, *PDSI_PHY_TIMCON3_REG;

#if 0
typedef struct
{
	DSI_PHY_TIMCON0_REG	CTRL0;
	DSI_PHY_TIMCON1_REG	CTRL1;
	DSI_PHY_TIMCON2_REG	CTRL2;
	DSI_PHY_TIMCON3_REG	CTRL3;
} DSI_PHY_TIMCON_REG, *PDSI_PHY_TIMCON_REG;
#endif

u32 Hal_MIPI_DSI_RegisterRead(u32 u32Addr);
void Hal_MIPI_DSI_RegisterWrite(u32 u32Addr, u32  u32value);
void Hal_MIPI_DSI_RegisterWriteBit(u32 u32Addr, bool bitValue, u8  u8bitpos);

DSI_STATUS Hal_DSI_TXRX_Control(bool cksm_en,
                                                                 bool ecc_en,
                                                                u8 lane_num,
                                                                u8 vc_num,
                                                                bool null_packet_en,
                                                                bool err_correction_en,
                                                                bool dis_eotp_en,
								        bool hstx_cklp_en,
                                                               u32 max_return_size);
DSI_STATUS  Hal_DSI_WriteShortPacket(u8 count, u8 cmd, u8 *para_list);
DSI_STATUS  Hal_DSI_WriteLongPacket(u8 count, u8 cmd, u8 *para_list);
DSI_STATUS  Hal_DSI_ReadShortPacket(u8 u8ReadBackCount, u8 u8RegAddr);
DSI_STATUS  Hal_DSI_ReadLongPacket(u8 u8ReadBackCount, u8 u8RegAddr);
DSI_STATUS Hal_DSI_PHY_TIMCONFIG(LCM_PARAMS *lcm_params);
DSI_STATUS Hal_DSI_Config_VDO_Timing(LCM_PARAMS *lcm_params);
DSI_STATUS Hal_DSI_DHY_Init(void);
DSI_STATUS Hal_DSI_Switch_Mode(DSI_MODE_CTL mode);
DSI_STATUS Hal_DSI_SetLaneNum(DSI_TOTAL_LANE_NUM LaneNum);
void Hal_DSI_LPLL(LCM_PARAMS *lcm_params);
LCM_PARAMS  Hal_DSI_GetInfo(void);



void  Hal_DSI_PowerOn(void);
void  Hal_DSI_PowerOff(void);
void  Hal_DSI_EnableClk(void);
void  Hal_DSI_DisableClk(void);
void Hal_DSI_PadOutSel(DSI_TOTAL_LANE_NUM LaneNum);
bool Hal_DSI_clk_HS_state(void);
void Hal_DSI_clk_HS_mode(bool enter);

#endif /*__HAL_MIPI_DSI_H__ */
