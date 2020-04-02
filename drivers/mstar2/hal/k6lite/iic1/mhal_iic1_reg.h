////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2007 MStar Semiconductor, Inc.
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

#ifndef _REG_IIC1_H_
#define _REG_IIC1_H_

#define _bit15		0x8000
#define _bit14		0x4000
#define _bit13		0x2000
#define _bit12		0x1000
#define _bit11		0x0800
#define _bit10		0x0400
#define _bit9		0x0200
#define _bit8		0x0100
#define _bit7		0x0080
#define _bit6		0x0040
#define _bit5		0x0020
#define _bit4		0x0010
#define _bit3		0x0008
#define _bit2		0x0004
#define _bit1		0x0002
#define _bit0		0x0001
#define MHal_IIC_DELAY()      { udelay(1000); udelay(1000); udelay(1000); udelay(1000); udelay(1000); }//delay 5ms
#define REG_IIC1_BASE              0xFD226A00 
#define REG_IIC_CTRL               0x00
#define REG_IIC_CLK_SEL            0x01
#define REG_IIC_WDATA              0x02
#define REG_IIC_RDATA              0x03
#define REG_IIC_STATUS             0x04       // reset, clear and status
#define MHal_IIC1_REG(addr)			(*(volatile U32*)(REG_IIC1_BASE + ((addr)<<2)))

#define REG_CHIP_BASE              0xFD203C00
#define REG_IIC_ALLPADIN           0x50
#define REG_IIC_MODE               0x57
#define REG_DDCR_GPIO_SEL          0x70
#define MHal_CHIP_REG(addr)             (*(volatile U32*)(REG_CHIP_BASE + ((addr)<<2)))
//the definitions of GPIO reg set to initialize
#define REG_ARM_BASE              0xFD000000//Use 8 bit addressing
//#define REG_ALL_PAD_IN              ((0x0f50<<1) )   //set all pads (except SPI) as input
#define REG_ALL_PAD_IN              (0x101ea1)   //set all pads (except SPI) as input
/**************************&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&**********************************/
//############################
//
//IP bank address : for pad mux in chiptop 
//
//############################
#define CHIP_REG_BASE                   (0x101E00)
#define PIU_NONPM_REG_BASE              (0x103C00) 
#define CHIP_GPIO1_REG_BASE            (0x101A00)
#define CLKGEN1_REG_BASE                (0x103300)
//for port 0
#define CHIP_REG_HWI2C_MIIC1            (CHIP_REG_BASE+ (0x12)) 
    #define CHIP_MIIC1_PAD_0            0
    #define CHIP_MIIC1_PAD_1            (__BIT2)
    #define CHIP_MIIC1_PAD_2            (__BIT3)
    #define CHIP_MIIC1_PAD_3            (__BIT2|__BIT3)
    #define CHIP_MIIC1_PAD_MSK          (__BIT2|__BIT3)


//pad mux configuration
#define CHIP_REG_ALLPADIN               (CHIP_REG_BASE+0xA0)
    #define CHIP_ALLPAD_IN              (__BIT0)
	
#define REG_HWI2C_MIIC1_VER_SEL          (PIU_NONPM_REG_BASE+ (0x02*2)) //select MI2C mode
    #define REG_HWI2C_MIIC1_VER_V2       (0)
    #define REG_HWI2C_MIIC1_VER_V3       (__BIT1|__BIT0)
    #define REG_HWI2C_MIIC1_VER_MSK      (__BIT1|__BIT0)

#define CHIP_REG_HWI2C_MIIC1_CLK              (CLKGEN1_REG_BASE + (0x30) * 2)
    #define CHIP_REG_HWI2C_MIIC1_CLK_72M      0
    #define CHIP_REG_HWI2C_MIIC1_CLK_XTAL     (__BIT6)
    #define CHIP_REG_HWI2C_MIIC1_CLL_36M      (__BIT7)
    #define CHIP_REG_HWI2C_MIIC1_CLK_54M      (__BIT6 | __BIT7)
    #define CHIP_REG_HWI2C_MIIC1_CLK_MSK      (__BIT4 | __BIT5 | __BIT6 | __BIT7)

//############################
//
//IP bank address : for independent port 
//
//############################
//Standard mode
#define HWI2C1_REG_BASE                  (0x113500) 
#define REG_HWI2C1_MIIC_CFG              (HWI2C1_REG_BASE+0x00*2)
    #define _MIIC1_CFG_RESET             (__BIT0)
    #define _MIIC1_CFG_EN_DMA            (__BIT1)
    #define _MIIC1_CFG_EN_INT            (__BIT2)
    #define _MIIC1_CFG_EN_CLKSTR         (__BIT3)
    #define _MIIC1_CFG_EN_TMTINT         (__BIT4)
    #define _MIIC1_CFG_EN_FILTER         (__BIT5)
    #define _MIIC1_CFG_EN_PUSH1T         (__BIT6)
    #define _MIIC1_CFG_RESERVED          (__BIT7)
#define REG_HWI2C1_CMD_START             (HWI2C1_REG_BASE+0x01*2)
    #define _CMD_START                  (__BIT0)
#define REG_HWI2C1_CMD_STOP              (HWI2C1_REG_BASE+0x01*2+1)
    #define _CMD_STOP                   (__BIT0)
#define REG_HWI2C1_WDATA                 (HWI2C1_REG_BASE+0x02*2)
#define REG_HWI2C1_WDATA_GET             (HWI2C1_REG_BASE+0x02*2+1)
    #define _WDATA_GET_ACKBIT           (__BIT0)
#define REG_HWI2C1_RDATA                 (HWI2C1_REG_BASE+0x03*2)
#define REG_HWI2C1_RDATA_CFG             (HWI2C1_REG_BASE+0x03*2+1)
    #define _RDATA_CFG_TRIG             (__BIT0)
    #define _RDATA_CFG_ACKBIT           (__BIT1)
#define REG_HWI2C1_INT_CTL               (HWI2C1_REG_BASE+0x04*2)
    #define _INT_CTL                    (__BIT0) //write this register to clear int
#define REG_HWI2C1_CUR_STATE             (HWI2C1_REG_BASE+0x05*2) //For Debug
    #define _CUR_STATE_MSK              (__BIT4|__BIT3|__BIT2|__BIT1|__BIT0)
#define REG_HWI2C1_INT_STATUS            (HWI2C1_REG_BASE+0x05*2+1) //For Debug
    #define _INT_STARTDET               (__BIT0)
    #define _INT_STOPDET                (__BIT1)
    #define _INT_RXDONE                 (__BIT2)
    #define _INT_TXDONE                 (__BIT3)
    #define _INT_CLKSTR                 (__BIT4)
    #define _INT_SCLERR                 (__BIT5)
#define REG_HWI2C1_STP_CNT               (HWI2C1_REG_BASE+0x08*2)
#define REG_HWI2C1_CKH_CNT               (HWI2C1_REG_BASE+0x09*2)
#define REG_HWI2C1_CKL_CNT               (HWI2C1_REG_BASE+0x0A*2)
#define REG_HWI2C1_SDA_CNT               (HWI2C1_REG_BASE+0x0B*2)
#define REG_HWI2C1_STT_CNT               (HWI2C1_REG_BASE+0x0C*2)
#define REG_HWI2C1_LTH_CNT               (HWI2C1_REG_BASE+0x0D*2)
#define REG_HWI2C1_TMT_CNT               (HWI2C1_REG_BASE+0x0E*2)
#define REG_HWI2C1_SCLI_DELAY            (HWI2C1_REG_BASE+0x0F*2)
    #define _SCLI_DELAY                 (__BIT2|__BIT1|__BIT0)

//DMA mode
#define REG_HWI2C1_DMA_CFG               (HWI2C1_REG_BASE+0x20*2)
#define REG_HWI2C1_DMA_MIU_ADR           (HWI2C1_REG_BASE+0x21*2) // 4 bytes
#define REG_HWI2C1_DMA_CTL               (HWI2C1_REG_BASE+0x23*2)
#define REG_HWI2C1_DMA_TXR               (HWI2C1_REG_BASE+0x24*2)
#define REG_HWI2C1_DMA_CMDDAT0           (HWI2C1_REG_BASE+0x25*2) // 8 bytes
#define REG_HWI2C1_DMA_CMDDAT1           (HWI2C1_REG_BASE+0x25*2+1)
#define REG_HWI2C1_DMA_CMDDAT2           (HWI2C1_REG_BASE+0x26*2)
#define REG_HWI2C1_DMA_CMDDAT3           (HWI2C1_REG_BASE+0x26*2+1)
#define REG_HWI2C1_DMA_CMDDAT4           (HWI2C1_REG_BASE+0x27*2)
#define REG_HWI2C1_DMA_CMDDAT5           (HWI2C1_REG_BASE+0x27*2+1)
#define REG_HWI2C1_DMA_CMDDAT6           (HWI2C1_REG_BASE+0x28*2)
#define REG_HWI2C1_DMA_CMDDAT7           (HWI2C1_REG_BASE+0x28*2+1)
#define REG_HWI2C1_DMA_CMDLEN            (HWI2C1_REG_BASE+0x29*2)
#define REG_HWI2C1_DMA_DATLEN            (HWI2C1_REG_BASE+0x2A*2) // 4 bytes
#define REG_HWI2C1_DMA_TXFRCNT           (HWI2C1_REG_BASE+0x2C*2) // 4 bytes
#define REG_HWI2C1_DMA_SLVADR            (HWI2C1_REG_BASE+0x2E*2)
#define REG_HWI2C1_DMA_SLVCFG            (HWI2C1_REG_BASE+0x2E*2+1)
#define REG_HWI2C1_DMA_CTL_TRIG          (HWI2C1_REG_BASE+0x2F*2)
#define REG_HWI2C1_DMA_CTL_RETRIG        (HWI2C1_REG_BASE+0x2F*2+1)


#endif // _REG_IIC1_H_

