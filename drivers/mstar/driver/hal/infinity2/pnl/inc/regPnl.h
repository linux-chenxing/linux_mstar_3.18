
#ifndef _REG_PNL_H_
#define _REG_PNL_H_


//-------------------------------------------------------------------------------------------------
//  Hardware Capability
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------

#define BIT(_bit_)          (1 << (_bit_))
#define BITS(_bits_, _val_) ((BIT(((1)?_bits_)+1)-BIT(((0)?_bits_))) & (_val_<<((0)?_bits_)))
#define BITMASK(_bits_)     (BIT(((1)?_bits_)+1)-BIT(((0)?_bits_)))


#ifndef BIT0
#define BIT0    0x00000001
#endif
#ifndef BIT1
#define BIT1    0x00000002
#endif
#ifndef BIT2
#define BIT2    0x00000004
#endif
#ifndef BIT3
#define BIT3    0x00000008
#endif
#ifndef BIT4
#define BIT4    0x00000010
#endif
#ifndef BIT5
#define BIT5    0x00000020
#endif
#ifndef BIT6
#define BIT6    0x00000040
#endif
#ifndef BIT7
#define BIT7    0x00000080
#endif
#ifndef BIT8
#define BIT8    0x00000100
#endif
#ifndef BIT9
#define BIT9    0x00000200
#endif
#ifndef BIT10
#define BIT10   0x00000400
#endif
#ifndef BIT11
#define BIT11   0x00000800
#endif
#ifndef BIT12
#define BIT12   0x00001000
#endif
#ifndef BIT13
#define BIT13   0x00002000
#endif
#ifndef BIT14
#define BIT14   0x00004000
#endif
#ifndef BIT15
#define BIT15   0x00008000
#endif
#ifndef BIT16
#define BIT16   0x00010000
#endif
#ifndef BIT17
#define BIT17   0x00020000
#endif
#ifndef BIT18
#define BIT18   0x00040000
#endif
#ifndef BIT19
#define BIT19   0x00080000
#endif
#ifndef BIT20
#define BIT20   0x00100000
#endif
#ifndef BIT21
#define BIT21   0x00200000
#endif
#ifndef BIT22
#define BIT22   0x00400000
#endif
#ifndef BIT23
#define BIT23   0x00800000
#endif
#ifndef BIT24
#define BIT24   0x01000000
#endif
#ifndef BIT25
#define BIT25   0x02000000
#endif
#ifndef BIT26
#define BIT26   0x04000000
#endif
#ifndef BIT27
#define BIT27   0x08000000
#endif
#ifndef BIT28
#define BIT28   0x10000000
#endif
#ifndef BIT29
#define BIT29   0x20000000
#endif
#ifndef BIT30
#define BIT30   0x40000000
#endif
#ifndef BIT31
#define BIT31   0x80000000
#endif

//------------------------------------------------------------------------------
// Base Address
//------------------------------------------------------------------------------
#define REG_PNL_BASE 		0x162200  //MV5 bank addr
#define REG_DEC_MISC_BASE 	0x161500  //
#define REG_CHIPTOP_BASE 	0x101E00  //
#define REG_PADTOP1_BASE    0x102600//0x101E00
#define REG_CKG_BASE        0x103300//0x103800

#define BK_REG(reg)               ((reg) * 2)

#define REG_PNL_00 	BK_REG(0x00)
#define REG_PNL_01 	BK_REG(0x01)
#define REG_PNL_02 	BK_REG(0x02)

#define REG_PNL_08 	BK_REG(0x08)
#define REG_PNL_09 	BK_REG(0x09)
#define REG_PNL_0A 	BK_REG(0x0A)

#define REG_PNL_0B 	BK_REG(0x0B)
#define REG_PNL_0C 	BK_REG(0x0C)
#define REG_PNL_0D 	BK_REG(0x0D)

#define REG_PNL_10 	BK_REG(0x10)
#define REG_PNL_11 	BK_REG(0x11)
#define REG_PNL_12 	BK_REG(0x12)
#define REG_PNL_13 	BK_REG(0x13)
#define REG_PNL_14 	BK_REG(0x14)
#define REG_PNL_15 	BK_REG(0x15)
#define REG_PNL_16 	BK_REG(0x16)


#define REG_PNL_18 	BK_REG(0x18)
#define REG_PNL_19 	BK_REG(0x19)
#define REG_PNL_1B 	BK_REG(0x1B)

#define REG_PNL_20 	BK_REG(0x20)
#define REG_PNL_21 	BK_REG(0x21)
#define REG_PNL_22 	BK_REG(0x22)
#define REG_PNL_23 	BK_REG(0x23)
#define REG_PNL_24 	BK_REG(0x24)
#define REG_PNL_25 	BK_REG(0x25)

#define REG_PNL_30 	BK_REG(0x30)
#define REG_PNL_31 	BK_REG(0x31)
#define REG_PNL_32 	BK_REG(0x32)
#define REG_PNL_33 	BK_REG(0x33)

#define REG_PNL_38 	BK_REG(0x38)
#define REG_PNL_39 	BK_REG(0x39)
#define REG_PNL_3A 	BK_REG(0x3A)
#define REG_PNL_3C 	BK_REG(0x3C)
#define REG_PNL_3E 	BK_REG(0x3E)
#define REG_PNL_3F 	BK_REG(0x3F)

#define REG_PNL_40 	BK_REG(0x40)
#define REG_PNL_41 	BK_REG(0x41)
#define REG_PNL_42 	BK_REG(0x42)
#define REG_PNL_43 	BK_REG(0x43)
#define REG_PNL_44 	BK_REG(0x44)
#define REG_PNL_45 	BK_REG(0x45)
#define REG_PNL_46 	BK_REG(0x46)
#define REG_PNL_47 	BK_REG(0x47)
#define REG_PNL_48 	BK_REG(0x48)
#define REG_PNL_49 	BK_REG(0x49)
#define REG_PNL_4A 	BK_REG(0x4A)
#define REG_PNL_4B 	BK_REG(0x4B)
#define REG_PNL_4C 	BK_REG(0x4C)
#define REG_PNL_4D 	BK_REG(0x4D)
#define REG_PNL_4E 	BK_REG(0x4E)

#define REG_PNL_50 	BK_REG(0x50)
#define REG_PNL_51 	BK_REG(0x51)
#define REG_PNL_52 	BK_REG(0x52)
#define REG_PNL_53 	BK_REG(0x53)

#define REG_PNL_58 	BK_REG(0x58)
#define REG_PNL_5A 	BK_REG(0x5A)
#define REG_PNL_5C 	BK_REG(0x5C)
#define REG_PNL_5E 	BK_REG(0x5E)
//------------------------------------------------------------------------------
// Global Setting
//------------------------------------------------------------------------------

#define PNL_TRANS_CTL  						(REG_PNL_BASE +REG_PNL_00) //panel transfer control
	#define PNL_LCD_SW_RST   					(BIT0)
	#define PNL_LCD_TRANS_START   				(BIT1)  //LCD panel transfer start (FW refresh one frame)
	#define PNL_IDX_RDY							(BIT2) // make sureused for serial/parallel panel interface
	#define PNL_IDX_RDY_ST						(BIT3) //used for serial/parallel panel interface
	#define PNL_CMD_RDY						(BIT4) //
	#define PNL_CMD_RDY_ST						(BIT5) //
	#define PNL_FRAME_TX_SETADDR_EN			(BIT6) //
#define PNL_SIGNAL_CTL 						(REG_PNL_BASE +REG_PNL_01) //
	#define PNL_RGB_DATA_DIS   					(BIT10)
	#define PNL_RGB_HSYNC_DIS   				(BIT9)  //
	#define PNL_RGB_VSYNC_DIS					(BIT8) //
	#define PNL_RGB_DEN_DIS					(BIT7) //
	#define PNL_RGB_PIXEL_CLK_DIS				(BIT6) //
	#define PNL_P_LCD_CS2_DIS					(BIT5) //
	#define PNL_P_LCD_CS1_DIS					(BIT4) //
	#define PNL_P_LCD_WR_DIS					(BIT3) //
	#define PNL_P_LCD_RD_DIS					(BIT2) //
	#define PNL_P_LCD_A0_DIS					(BIT1) //
	#define PNL_DSPY_REG_READY				(BIT0) //
#define PNL_RGB_SHARE_P_LCD_BUS			(REG_PNL_BASE +REG_PNL_02) //
	#define PNL_P_LCD_ONLY                  			(0x00)
       #define PNL_RGB_18BIT_SHARE_WITH_P_LCD   (BIT1)
       #define PNL_RGB_24BIT_SHARE_WITH_P_LCD   (BIT2)
       #define PNL_RGB_LCD_ONLY               			(BIT1|BIT0)


//------------------------------------------------------------------------------
// Interrupt related
//------------------------------------------------------------------------------
#define PNL_LCD_IRQ_MSK			(REG_PNL_BASE +REG_PNL_08) //
#define PNL_LCD_IRQ_FORCE			(REG_PNL_BASE +REG_PNL_09) //
#define PNL_LCD_IRQ_RAW_ST		(REG_PNL_BASE +REG_PNL_0A) //
	/*
	Bit0 : LCD frame end interrupt status of primary display
	Bit1 : LCD index ready
	Bit2 : LCD command ready
	Bit3 : LCD RGB VSYNC
	Bit4 : LCD RGB frame count hit
       Bit5 : LCD busy status
       Bit6 : LCD RGB HSYNC
       Bit7 : RGB I/F line match INT1 status
       Bit8: : RGB I/F line match INT2 status
       Bit9 : LCD FLM INT status
       Bit12 : LCD RGB underflow
	*/
#define PNL_LCD_IRQ_FIN_ST			(REG_PNL_BASE +REG_PNL_0B) //
#define PNL_LCD_IRQ_SEL				(REG_PNL_BASE +REG_PNL_0C) //
#define PNL_LCD_IRQ_CLR				(REG_PNL_BASE +REG_PNL_0D) //write 1 clear

//------------------------------------------------------------------------------
// RGB panel setting
//------------------------------------------------------------------------------
#define PNL_DSPY_USED				(REG_PNL_BASE +REG_PNL_10)    //TBD
    #define PNL_DSPY1_SEL                      (BIT6)
    #define PNL_DSPY1_RGB_SEL              BITS(2:0,0x2)
    #define PNL_DSPY1_I80_SEL               BITS(2:0,0x0)
     #define PNL_DSPY1_MASK                 BITMASK(2:0)
#define PNL_RGB_PRT_PHS			(REG_PNL_BASE +REG_PNL_11)    //Represent the number of DOTCLKs from PRT transition to HSYNC falling edge
														    // the unit is DOTCLK(pixel). It value is 0~255, represent 0~255
	#define PNL_RGB_DOT_CLK_RATIO  BITMASK(15:8)

#define PNL_RGB_V_BPORCH			(REG_PNL_BASE +REG_PNL_12)    //
	#define PNL_PORCH_MASK  	BITMASK(9:0)
#define PNL_RGB_V_BLANK			(REG_PNL_BASE +REG_PNL_13)    //
#define PNL_RGB_H_BPORCH			(REG_PNL_BASE +REG_PNL_14)    //
#define PNL_RGB_H_BLANK			(REG_PNL_BASE +REG_PNL_15)    //

#define PNL_RGB_SYNC_WIDTH		(REG_PNL_BASE +REG_PNL_16)    //
       #define PNL_RGB_VSYNC_W_SHIFT  	        BIT8
       #define PNL_RGB_HSYNC_W_SHIFT  	        0
	#define PNL_RGB_VSYNC_W_MASK  	BITMASK(15:8)
	#define PNL_RGB_HSYNC_W_MASK 		BITMASK(7:0)

#define PNL_RGB_CTL					(REG_PNL_BASE +REG_PNL_18)    //
	 #define PNL_DDRCLK_POLAR   			(BIT7) //0:NORMAL MODE, 1:invert
        #define PNL_DOTCLK_MODE      		(BIT6) //0:NORMAL MODE, 1:DDR clock
        #define PNL_HSYNC_POLAR	       	(BIT5)//0:Active low, 1: Active High
        #define PNL_VSYNC_POLAR       		(BIT4)//0:Active low, 1: Active High
        #define PNL_DOT_POLAR         			(BIT3)//0:Active low, 1: Active High
	 #define PNL_DEN_DATA_MODE		(BIT2)// function selection of "data enable".
        #define PNL_PARTIAL_MODE_EN		(BIT1)//partial mode enable
        #define PNL_RGB_IF_EN		        		(BIT0) //RGB interface enable

#define PNL_RGB_DTYPE					(REG_PNL_BASE +REG_PNL_19)
	#define YUV422_D8BIT_Y1VY0U          	BITS(5:0,0x27) //(BIT5|BIT2|BIT1|BIT0) //0x27
        #define YUV422_D8BIT_Y1UY0V         	BITS(5:0,0x26) //(BIT5|BIT2|BIT1)		 //0x26
        #define YUV422_D8BIT_Y0VY1U         	BITS(5:0,0x25) //(BIT5|BIT2|BIT0) 		//0x25
        #define YUV422_D8BIT_Y0UY1V         	BITS(5:0,0x24) //(BIT5|BIT2) 			//0x24
        #define YUV422_D8BIT_VY1UY0         	BITS(5:0,0x23) //(BIT5|BIT1|BIT0) 		//0x23
        #define YUV422_D8BIT_UY1VY0         	BITS(5:0,0x22) //(BIT5|BIT1) 			//0x22
        #define YUV422_D8BIT_VY0UY1          	BITS(5:0,0x21) //(BIT5|BIT0) 			//0x21
        #define YUV422_D8BIT_UY0VY1          	BITS(5:0,0x20) //(BIT5) 				//0x20

        #define RGB_D24BIT_BGR332			                    BITS(5:0,0xd)    			//0x0d
        #define RGB_D24BIT_BGR666_LSB_PACKED		      BITS(5:0,0xc)   			//0x0c
        #define RGB_D24BIT_BGR666_MSB_PACKED	      BITS(5:0,0xb)   			//0x0b
        #define RGB_D24BIT_BGR565			                    BITS(5:0,0xa) 			//0x0a
        #define RGB_D24BIT_BGR666			                    BITS(5:0,0x9) 			//0x09
        #define RGB_D24BIT_BGR888			                    BITS(5:0,0x8) 			//0x08
        #define RGB_D24BIT_RGB332			                    BITS(5:0,0x5) 			//0x05
        #define RGB_D24BIT_RGB666_LSB_PACKED		      BITS(5:0,0x4) 			//0x04
        #define RGB_D24BIT_RGB666_MSB_PACKED            BITS(5:0,0x3) 			//0x03
        #define RGB_D24BIT_RGB565			                   BITS(5:0,0x2)  			//0x02
        #define RGB_D24BIT_RGB666			                   BITS(5:0,0x1) 			//0x01
        #define RGB_D24BIT_RGB888			                   BITS(5:0,0x0) 			//0x00
        #define RGB_DTYPE_MASK			                    BITMASK(5:0)
        #define RGB_DTYPE_SWAP			(BIT6)
#define PNL_RGB_DELTA_MODE					(REG_PNL_BASE +REG_PNL_1B)
	   //BIT4~2
	 #define RGB_ODD_LINE_MSK        	BITMASK(4:2) //(BIT4|BIT3|BIT2)
        #define RGB_ODD_LINE_RGB        	BITS(4:2,0)
        #define RGB_ODD_LINE_RBG        	BITS(4:2,1)   //0x04
        #define RGB_ODD_LINE_GRB        	BITS(4:2,2)   //0x08
        #define RGB_ODD_LINE_GBR        	BITS(4:2,3)   //0x0C
        #define RGB_ODD_LINE_BRG        	BITS(4:2,4)  //0x10
        #define RGB_ODD_LINE_BGR        	BITS(4:2,5) // //0x14
         //BIT5~7
	 #define RGB_EVEN_LINE_MSK        	BITMASK(7:5) //(BIT7|BIT6|BIT5)
        #define RGB_EVEN_LINE_RGB       	BITS(7:5,0) // 0x00
        #define RGB_EVEN_LINE_RBG       	BITS(7:5,1) //0x20
        #define RGB_EVEN_LINE_GRB       	BITS(7:5,2)  //0x40
        #define RGB_EVEN_LINE_GBR       	BITS(7:5,3)  //0x60
        #define RGB_EVEN_LINE_BRG       	BITS(7:5,4)  //0x80
        #define RGB_EVEN_LINE_BGR       	BITS(7:5,5) //0xA0
	 #define RGB_DELTA_MODE_ENABLE   	BIT(0)
        #define RGB_DUMMY_MODE_ENABLE   	BIT(1)

#define PNL_RGB_PART_ST_Y			(REG_PNL_BASE +REG_PNL_20)   	//BIT0~12
#define PNL_RGB_PART_ED_Y			(REG_PNL_BASE +REG_PNL_21)   	//BIT0~12
#define PNL_RGB_PART_ST_X			(REG_PNL_BASE +REG_PNL_22)   	//BIT0~12
#define PNL_RGB_PART_ED_X			(REG_PNL_BASE +REG_PNL_23)   	//BIT0~12

#define PNL_LCD_WIDTH			(REG_PNL_BASE +REG_PNL_24)   	//BIT0~12
#define PNL_LCD_HEIGHT			(REG_PNL_BASE +REG_PNL_25)  	//BIT0~12

#define PNL_RGB_INTERLACE_EN		(REG_PNL_BASE +REG_PNL_30)  	//
	 #define RGB_INTERLACE_DIS      		0x00
        #define RGB_INTERLACE_EN       		BIT(0)
        #define RGB_TOTAL_ODD               		0x00
        #define RGB_TOTAL_EVEN              		BIT(1)

#define PNL_RGB_CPU_INT_FRM_CNT	(REG_PNL_BASE +REG_PNL_31)  	//
	 #define RGB_CPU_INT_FRM_ST      		(BIT8|BIT7|BIT6|BIT5|BIT4|BIT3|BIT2|BIT1|BIT0)
#define PNL_RGB_CPU1_INT_LINE_CNT		(REG_PNL_BASE +REG_PNL_32)  	//
#define PNL_RGB_CPU2_INT_LINE_CNT		(REG_PNL_BASE +REG_PNL_33)  	//

#define PNL_RGB_PIXL_CNT_L		(REG_PNL_BASE +REG_PNL_38)  	//
#define PNL_RGB_PIXL_CNT_H		(REG_PNL_BASE +REG_PNL_39)  	//

#define PNL_RGB_CTL_4		(REG_PNL_BASE +REG_PNL_3A)  	//for parallel panel only
	 #define LCD_OUT_RGB                 	0x0000
        #define LCD_OUT_BGR                 	 BIT(0)
        #define LCD_OUT_SEL_LCD1            BIT(1)
        #define LCD_OUT_SEL_LCD2            BIT(2)

#define PNL_LCD_FLM_CTL		(REG_PNL_BASE +REG_PNL_3C)  	//
	#define FLM_CHK_FLM_EN				 BIT(0)
    	#define FLM_SIG_POL					 BIT(1)
	#define FLM_INT_MODE_CNT_DONE	 BIT(2)
	#define FLM_CHK_VSYNC_EN			 BIT(8)
	#define FLM_VSYNC_SIG_POL			 BIT(9)

#define PNL_FLM_VSYNC_CNT_L		(REG_PNL_BASE +REG_PNL_3E)  	//
#define PNL_FLM_VSYNC_CNT_H		(REG_PNL_BASE +REG_PNL_3F)  	//

//------------------------------------------------------------------------------
// Parallel panel setting
//------------------------------------------------------------------------------
#define PNL_PL_LCD_TX_1_L			(REG_PNL_BASE +REG_PNL_40)  	//
#define PNL_PL_LCD_TX_1_H			(REG_PNL_BASE +REG_PNL_41)  	//

#define PNL_PL_LCD_TX_2_L			(REG_PNL_BASE +REG_PNL_42)  	//
#define PNL_PL_LCD_TX_2_H			(REG_PNL_BASE +REG_PNL_43)  	//

#define PNL_PL_LCD_TX_3_L			(REG_PNL_BASE +REG_PNL_44)  	//
#define PNL_PL_LCD_TX_3_H			(REG_PNL_BASE +REG_PNL_45)  	//

#define PNL_PL_LCD_TX_4_L			(REG_PNL_BASE +REG_PNL_46)  	//
#define PNL_PL_LCD_TX_4_H			(REG_PNL_BASE +REG_PNL_47)  	//

#define PNL_PL_LCD_TX_5_L			(REG_PNL_BASE +REG_PNL_48)  	//
#define PNL_PL_LCD_TX_5_H			(REG_PNL_BASE +REG_PNL_49)  	//

#define PNL_PL_LCD_TX_6_L			(REG_PNL_BASE +REG_PNL_4A)  	//
#define PNL_PL_LCD_TX_6_H			(REG_PNL_BASE +REG_PNL_4B)  	//

#define PNL_PL_LCD_TX_7_L			(REG_PNL_BASE +REG_PNL_4C)  	//
#define PNL_PL_LCD_TX_7_H			(REG_PNL_BASE +REG_PNL_4D)  	//

#define PNL_PL_LCD_AUTO_CFG			(REG_PNL_BASE +REG_PNL_4E)  	//
	 #define AUTO_TX_TYPE_IDX            	0x0000
        #define AUTO_TX_TYPE_CMD            	 BIT(0)
        #define LCD_TX_TYPE_IDX(_n)         	(AUTO_TX_TYPE_IDX << _n)
        #define LCD_TX_TYPE_CMD(_n)         	(AUTO_TX_TYPE_CMD << _n)

#define PNL_PL_LCD_CTL			(REG_PNL_BASE +REG_PNL_50)  	//
	 #define PLCD_BUS_8BPP               		BITS(1:0,0)
        #define PLCD_BUS_16BPP              		BITS(1:0,1) //(BIT0)
        #define PLCD_BUS_18BPP              		BITS(1:0,2) //(BIT1)
        #define PLCD_BUS_12BPP              		BITS(1:0,3) //(BIT1|BIT0)
        #define PLCD_BUS_MASK               	 	BITS(1:0,3) //(BIT1|BIT0)
        #define PLCD_68SYS_RD_POL       		(BIT2)  //read signal polarity for 68-system
        #define PLCD_CMD_BURST_MODE    	(BIT3)  //0:non-burst , 1:burst
        #define PLCD_PHA                		 	(BIT4) //phase of the LCD parallel data enable
        #define PLCD_POR                  	 		(BIT5) //polarity of the LCD parallel data enable
        #define PLCD_TYPE_80_68_SEL       	(BIT6) //0:68-system, 1:80-system
        #define PLCD_RD_DATA       		 	(BIT7) //
#define PNL_PL_LCD_FMT			(REG_PNL_BASE +REG_PNL_51)  	//
	 #define LCD_SPI1_PL2					0x00 //[Reserved:ShouldNotToRemoved]
        #define LCD_PL1_SPI2                			0x01 //[Reserved:ShouldNotToRemoved]
        #define LCD_PL1_PL2                 			0x02 //[Reserved:ShouldNotToRemoved]
        #define LCD_SPI1_SPI2               			0x03 //[Reserved:ShouldNotToRemoved]
        #define PLCD_RS_LEAD_CS_EN      	(BIT2)
        #define A0_SIG_FOR_IDX_CMD			(BIT3) //0:A0 low for index, High for CMD 1: A0 low for CMD, High for index
        #define RGB565_TX_FIRST				(BIT4)//0:RG first for 1st cycle, GB for 2nd cycle,  1: GB first for 1st cycle, RG for 2nd cycle

#define PNL_PL_LCD_BUS			(REG_PNL_BASE +REG_PNL_52)  	//
	#define PLCD_RGB565_BUS16           	BITS(3:0,0) //0x0000
        #define PLCD_RGB444_BUS16           BITS(3:0,1) //0x0001
        #define PLCD_RGB666_BUS16           BITS(3:0,2) //0x0002
        #define PLCD_RGB888_BUS8            BITS(3:0,3) //0x0003
        #define PLCD_RGB332_BUS8            BITS(3:0,4) //0x0004
        #define PLCD_RGB444_BUS8            BITS(3:0,5) //0x0005
        #define PLCD_RGB666_BUS8            BITS(3:0,6) //0x0006
        #define PLCD_RGB565_BUS8            BITS(3:0,7) //0x0007
        #define PLCD_RGB666_BUS18           BITS(3:0,8) //0x0008
        #define PLCD_RGB666_BUS18_9_9     	BITS(3:0,9) //0x0009
        #define PLCD_RGB666_BUS18_2_16      BITS(3:0,10) //0x000A
        #define PLCD_RGB666_BUS18_16_2      BITS(3:0,11) //0x000B
        #define PLCD_RGB24_BUS18_16_8     	BITS(3:0,12) //0x000C
        #define PLCD_RGB24_BUS18_8_16  	BITS(3:0,13) //	0x000D
        #define PLCD_RGB18_BUS8_2_8_8		BITS(3:0,14) //0x000E
        #define PLCD_RGB666_BUS8_2_7       	BIT(4)
        #define PLCD_RGB444_B12_EXT_B16   	BIT(5)
        #define PLCD_RGB444_B15_4     		BIT(6)
        #define PLCD_RGBB9_9_17             	BIT(7)

#define PNL_PL_LCD_BUS1			(REG_PNL_BASE +REG_PNL_53)  	//
	#define PLCD_RGB666_B9              			BIT(0) //0x01
        #define LCD_RGB666_B2_16            		BIT(1)//0x02
        #define LCD_RGB666_B16_2            		0x03
        #define LCD_B8_RGB666_12_17         		0x04
        #define LCD_B16_1_8_10_17           		0x08
        #define LCD_B8_10_17                			0x10
        #define LCD_B8_1_8                  			0x20
        #define LCD_B8_RGB565_G_LSB_FIRST   	0x40
#define PNL_P_LCD_RS_LEAD_CS_CYC			(REG_PNL_BASE +REG_PNL_58)  	//
#define PNL_P_LCD_CS_LEAD_RW_CYC			(REG_PNL_BASE +REG_PNL_5A)  	//
#define PNL_P_LCD_RW_CYC					(REG_PNL_BASE +REG_PNL_5C)  	//
#define PNL_P_LCD_IDX_CMD_NUM				(REG_PNL_BASE +REG_PNL_5E)  	//

//------------------------------------------------------------------------------
// Dec Misc Setting
//------------------------------------------------------------------------------
#define REG_DEC_MISC_02     BK_REG(0x02)
#define REG_DEC_MISC_42     BK_REG(0x42)
#define REG_DEC_MISC_47     BK_REG(0x47)
#define REG_DEC_MISC_4A     BK_REG(0x4A)
#define REG_DEC_MISC_4B     BK_REG(0x4B)
#define REG_DEC_MISC_50     BK_REG(0x50)
#define REG_DEC_MISC_51     BK_REG(0x51)
#define REG_DEC_MISC_52     BK_REG(0x52)
#define REG_DEC_MISC_53     BK_REG(0x53)
#define REG_DEC_MISC_54     BK_REG(0x54)
#define REG_DEC_MISC_55     BK_REG(0x55)
#define REG_DEC_MISC_56     BK_REG(0x56)
#define REG_DEC_MISC_57     BK_REG(0x57)
#define REG_DEC_MISC_58     BK_REG(0x58)

#define DEC_MISC_MVOP_TRIG_OFF          (REG_DEC_MISC_BASE +REG_DEC_MISC_4A)  	//
#define DEC_MISC_MVOP_VSYNC_OFF         (REG_DEC_MISC_BASE +REG_DEC_MISC_4B)  	//

#define DEC_MISC_RGB565_EC0             (REG_DEC_MISC_BASE +REG_DEC_MISC_47)  	//
    #define DEC_MISC_RGB565_EC0_SEL         				BIT(0) //0x02
    #define DEC_MISC_MIPI_DSI_EC0_SEL         				BIT(1) //0x02
#define DEC_MISC_OUT_MODE        (REG_DEC_MISC_BASE +REG_DEC_MISC_42)  	//
    #define DEC_MISC_LCD         BITS(1:0,0)
    #define DEC_MISC_MIPI_DSI    BITS(1:0,1)
    #define DEC_MISC_MIPI_CSI    BITS(1:0,2)
    #define DEC_MISC_LB_OUT    BITMASK(1:0)
#define DEC_MISC_BLK_BG_IN_WIDTH        (REG_DEC_MISC_BASE +REG_DEC_MISC_50)  	//
#define DEC_MISC_BLK_BG_IN_WIDTH        (REG_DEC_MISC_BASE +REG_DEC_MISC_50)  	//
#define DEC_MISC_BLK_BG_IN_HEIGHT       (REG_DEC_MISC_BASE +REG_DEC_MISC_51)  	//
#define DEC_MISC_BLK_BG_OUT_WIDTH       (REG_DEC_MISC_BASE +REG_DEC_MISC_52)  	//

#define DEC_MISC_BG_OUT_X_ST            (REG_DEC_MISC_BASE +REG_DEC_MISC_53)  	//
#define DEC_MISC_BG_OUT_Y_ST		    (REG_DEC_MISC_BASE +REG_DEC_MISC_54)  	//
#define DEC_MISC_BG_OUT_COL_R           (REG_DEC_MISC_BASE +REG_DEC_MISC_55)  	//
#define DEC_MISC_BG_OUT_COL_G		    (REG_DEC_MISC_BASE +REG_DEC_MISC_56)  	//
#define DEC_MISC_BG_OUT_COL_B	        (REG_DEC_MISC_BASE +REG_DEC_MISC_57)  	//
#define DEC_MISC_BG_FUNC			    (REG_DEC_MISC_BASE +REG_DEC_MISC_58)  	//
	#define BG_FUNC_BYPASS              			BIT(1)  //0x01
	#define BG_FORCE_COL            				BIT(0) //0x02

//------------------------------------------------------------------------------
// Chiptop Setting
//------------------------------------------------------------------------------
#define REG_CHIPTOP_4A     BK_REG(0x4A)
#define REG_CHIPTOP_4B     BK_REG(0x4B)
#define REG_CHIPTOP_4C     BK_REG(0x4C)
#define REG_CHIPTOP_4D     BK_REG(0x4D)

#define REG_CHIPTOP_14      BK_REG(0x14)
#define CHIPTOP_LCDPAD_MUX     (REG_CHIPTOP_BASE+REG_CHIPTOP_14)
    #define RGB8_BITMODE        BITS(9:8,0)
    #define RGB16_BITMODE       BITS(5:4,1)
    #define RGB18_BITMODE       BIT(6)
    #define RGB24_BITMODE       BIT(7)

    #define PL8_BITMODE         BIT(2)
    #define PL16_BITMODE        BIT(0)
    #define PL18_BITMODE        BIT(1)

#define CLKG_ODCLK_CTRL0     (REG_CKG_BASE+BK_REG(0x54))
    #define CKG_ODCLK_DIS_CLK             BIT(0)
    #define CKG_ODCLK_INV_CLK             BIT(1)
    #define CKG_ODCLK_MPLL_240M           BITS(4:2,5)
    #define CKG_ODCLK_MPLL_216M           BITS(4:2,4)
    #define CKG_ODCLK_MPLL_160M           BITS(4:2,1)
    #define CKG_ODCLK_MPLL_144M           BITS(4:2,2)
    #define CKG_ODCLK_MPLL_108M           BITS(4:2,3)
    #define CKG_ODCLK_LPLL_CLK            BITS(4:2,0)

#endif
