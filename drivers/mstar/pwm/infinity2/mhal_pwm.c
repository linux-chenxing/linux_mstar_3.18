// $Change: 626442 $
//-----------------------------------------------------------------------------
//
// Copyright (c) 2008 MStar Semiconductor, Inc.  All rights reserved.
//
//-----------------------------------------------------------------------------
// FILE
//      pwm.c
//
// DESCRIPTION
//      PWM HW control codes.
//
// HISTORY
//      2008.05.15      Bryan Fan      Initial Version
//      2008.6.12       Bryan Fan      Revised for Code Review
//      2008.6.26       Bryan Fan      Revised for new Code Review
//
//-----------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Include Files
//------------------------------------------------------------------------------

#include "mhal_pwm.h"
#include "gpio.h"
#define XTAL_12000K        12000000

//------------------------------------------------------------------------------
//  Variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Local Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  External Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Global Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
//  Function:   MhalPWMSetDuty
//
//  Description
//      Set Duty value
//
//  Parameters
//      u8Id:    [in] PWM ID
//      u16Val:  [in] Duty value
//
//  Return Value
//      None
//
void MhalPWMSetDuty(struct mstar_pwm_chip *ms_chip, U8 u8Id, U32 u32Val)
{
    U32 u32Period;
    U32 u32Duty;

	u32Period = INREG16(ms_chip->base + (u8Id*0x80) + u16REG_PWM_PERIOD_L) + ((INREG16(ms_chip->base + (u8Id*0x80) + u16REG_PWM_PERIOD_H)&0x3)<< 16);

	if(u32Val == 0) //allows output low, to avoid 1T pulse
	{
		OUTREGMSK16(ms_chip->base + u16REG_SW_RESET, BIT0<<u8Id, BIT0<<u8Id);
	}
	else
	{
		 u32Duty = ((u32Period * u32Val) / 100);

	    pr_err("reg=0x%08X clk=%d, u32Duty=0x%x\n", (U32)(ms_chip->base + (u8Id*0x80) + u16REG_PWM_DUTY_L), (U32)(12000000), u32Duty);
		OUTREGMSK16(ms_chip->base + u16REG_SW_RESET, ~(BIT0<<u8Id), BIT0<<u8Id);
		OUTREG16(ms_chip->base + (u8Id*0x80) + u16REG_PWM_DUTY_L, (u32Duty&0xFFFF));
	    OUTREG16(ms_chip->base + (u8Id*0x80) + u16REG_PWM_DUTY_H, ((u32Duty>>16)&0x3));
	}
}

//------------------------------------------------------------------------------
//
//  Function:   MhalPWMSetPeriod
//
//  Description
//      Set Period value
//
//  Parameters
//      u8Id:    [in] PWM ID
//      u16Val:  [in] Period value
//
//  Return Value
//      None
//
void MhalPWMSetPeriod(struct mstar_pwm_chip *ms_chip, U8 u8Id, U32 u32Val)
{
    U32 u32Period;

    u32Period = (U32)(XTAL_12000K)/u32Val;

	//[APN] range 2<=Period<=262144
	if(u32Period < 2)
		u32Period = 2;
	if(u32Period > 262144)
		u32Period = 262144;

	//[APN] PWM _PERIOD= (REG_PERIOD+1)
	u32Period = u32Period - 1;

    pr_err("reg=0x%08X period=0x%x\n", (U32)(ms_chip->base + (u8Id*0x80) + u16REG_PWM_PERIOD_L),  u32Period);

    OUTREG16(ms_chip->base + (u8Id*0x80) + u16REG_PWM_PERIOD_L, (u32Period&0xFFFF));
    OUTREG16(ms_chip->base + (u8Id*0x80) + u16REG_PWM_PERIOD_H, ((u32Period>>16)&0x3));
}

//------------------------------------------------------------------------------
//
//  Function:   MhalPWMSetPolarity
//
//  Description
//      Set Polarity value
//
//  Parameters
//      u8Id:   [in] PWM ID
//      u8Val:  [in] Polarity value
//
//  Return Value
//      None
//
void MhalPWMSetPolarity(struct mstar_pwm_chip *ms_chip, U8 u8Id, U8 u8Val)
{
    OUTREGMSK16(ms_chip->base + (u8Id*0x80) + u16REG_PWM_CTRL, (u8Val<<POLARITY_BIT), (0x1<<POLARITY_BIT));
}

//------------------------------------------------------------------------------
//
//  Function:   MHalPWMSetDben
//
//  Description
//      Enable/Disable Dben function
//
//  Parameters
//      u8Id:   [in] PWM ID
//      u8Val:  [in] On/Off value
//
//  Return Value
//      None
//

void MHalPWMSetDben(struct mstar_pwm_chip *ms_chip, U8 u8Id, U8 u8Val)
{
    OUTREGMSK16(ms_chip->base + (u8Id*0x80) + u16REG_PWM_CTRL, (u8Val<<DBEN_BIT), (0x1<<DBEN_BIT));
}

void MhalPWMEnable(struct mstar_pwm_chip *ms_chip, U8 u8Id, U8 u8Val)
{
    if(u8Val)
        CLRREG16(ms_chip->base + u16REG_SW_RESET, 1<<u8Id);
    else
        SETREG16(ms_chip->base + u16REG_SW_RESET, 1<<u8Id);
}

void MhalPWMPadSet(U8 u8Id, U8 u8Val)
{
    //reg_pwm0_mode [1:0]
    //reg_pwm1_mode [3:2]
    //reg_pwm2_mode [5:4]
    //reg_pwm3_mode [7:6]
    //reg_pwm4_mode [9:8]
    //reg_pwm5_mode [11:10]
    //reg_pwm6_mode [13:12]
    //reg_pwm7_mode [15:14]
    if( 0 == u8Id )
    {
        if(u8Val==PAD_PWM0){
            OUTREGMSK16(BASE_REG_PADTOP1_PA + REG_ID_09, BIT0, BIT1|BIT0);
        }else if(u8Val==PAD_MIPI_TX_IO0){
            OUTREGMSK16(BASE_REG_PADTOP1_PA + REG_ID_09, BIT1, BIT1|BIT0);
        }else if(u8Val==PAD_SNR3_D0){
            OUTREGMSK16(BASE_REG_PADTOP1_PA + REG_ID_09, BIT1|BIT0, BIT1|BIT0);
        }else{
            OUTREGMSK16(BASE_REG_PADTOP1_PA + REG_ID_09, 0x0, BIT1|BIT0);
            if(u8Val!=PAD_UNKNOWN)
                printk("PWM pad set failed\n");
        }
    }
    else if( 1 == u8Id )
    {
        if(u8Val==PAD_PWM1){
            OUTREGMSK16(BASE_REG_PADTOP1_PA + REG_ID_09, BIT2, BIT2|BIT3);
        }else if(u8Val==PAD_MIPI_TX_IO1){
            OUTREGMSK16(BASE_REG_PADTOP1_PA + REG_ID_09, BIT3, BIT2|BIT3);
        }else if(u8Val==PAD_SNR3_D1){
            OUTREGMSK16(BASE_REG_PADTOP1_PA + REG_ID_09, BIT2|BIT3, BIT2|BIT3);
        }else{
            OUTREGMSK16(BASE_REG_PADTOP1_PA + REG_ID_09, 0x0, BIT2|BIT3);
            if(u8Val!=PAD_UNKNOWN)
                printk("PWM pad set failed\n");
        }

    }
    else if( 2 == u8Id )
    {
        if(u8Val==PAD_GPIO8){
            OUTREGMSK16(BASE_REG_PADTOP1_PA + REG_ID_09, BIT4, BIT5|BIT4);
        }else if(u8Val==PAD_MIPI_TX_IO2){
            OUTREGMSK16(BASE_REG_PADTOP1_PA + REG_ID_09, BIT5, BIT5|BIT4);
        }else if(u8Val==PAD_SNR3_D2){
            OUTREGMSK16(BASE_REG_PADTOP1_PA + REG_ID_09, BIT5|BIT4, BIT5|BIT4);
		}else{
            OUTREGMSK16(BASE_REG_PADTOP1_PA + REG_ID_09, 0x0, BIT5|BIT4);
            if(u8Val!=PAD_UNKNOWN)
                printk("PWM pad set failed\n");

        }
    }
    else if( 3 == u8Id )
    {
        if(u8Val==PAD_GPIO9){
            OUTREGMSK16(BASE_REG_PADTOP1_PA + REG_ID_09, BIT6, BIT7|BIT6);
        }else if(u8Val==PAD_MIPI_TX_IO3){
            OUTREGMSK16(BASE_REG_PADTOP1_PA + REG_ID_09, BIT7, BIT7|BIT6);
        }else if(u8Val==PAD_SNR3_D3){
            OUTREGMSK16(BASE_REG_PADTOP1_PA + REG_ID_09, BIT7|BIT6, BIT7|BIT6);
		}else{
            OUTREGMSK16(BASE_REG_PADTOP1_PA + REG_ID_09, 0x0, BIT7|BIT6);
            if(u8Val!=PAD_UNKNOWN)
                printk("PWM pad set failed\n");
        }
    }
    else if( 4 == u8Id )
    {
        if(u8Val==PAD_GPIO10){
            OUTREGMSK16(BASE_REG_PADTOP1_PA + REG_ID_09, BIT8, BIT9|BIT8);
        }else if(u8Val==PAD_MIPI_TX_IO4){
            OUTREGMSK16(BASE_REG_PADTOP1_PA + REG_ID_09, BIT9, BIT9|BIT8);
		}else if(u8Val==PAD_SNR3_D8){
            OUTREGMSK16(BASE_REG_PADTOP1_PA + REG_ID_09, BIT9|BIT8, BIT9|BIT8);
        }else{
            OUTREGMSK16(BASE_REG_PADTOP1_PA + REG_ID_09, 0x0, BIT9|BIT8);
            if(u8Val!=PAD_UNKNOWN)
                printk("PWM pad set failed\n");
        }
    }
    else if( 5 == u8Id )
    {
        if(u8Val==PAD_GPIO11){
            OUTREGMSK16(BASE_REG_PADTOP1_PA + REG_ID_09, BIT10, BIT11|BIT10);
        }else if(u8Val==PAD_MIPI_TX_IO5){
            OUTREGMSK16(BASE_REG_PADTOP1_PA + REG_ID_09, BIT11, BIT11|BIT10);
        }else if(u8Val==PAD_SNR3_D9){
            OUTREGMSK16(BASE_REG_PADTOP1_PA + REG_ID_09, BIT11|BIT10, BIT11|BIT10);
		}else{
            OUTREGMSK16(BASE_REG_PADTOP1_PA + REG_ID_09, 0x0, BIT11|BIT10);
            if(u8Val!=PAD_UNKNOWN)
                printk("PWM pad set failed\n");

        }
    }
    else if( 6 == u8Id )
    {
        if(u8Val==PAD_GPIO12){
            OUTREGMSK16(BASE_REG_PADTOP1_PA + REG_ID_09, BIT12, BIT13|BIT12);
        }else if(u8Val==PAD_MIPI_TX_IO6){
            OUTREGMSK16(BASE_REG_PADTOP1_PA + REG_ID_09, BIT13, BIT13|BIT12);
        }else if(u8Val==PAD_PM_LED0){
            OUTREGMSK16(BASE_REG_PADTOP1_PA + REG_ID_09, BIT13|BIT12, BIT13|BIT12);
		}else{
            OUTREGMSK16(BASE_REG_PADTOP1_PA + REG_ID_09, 0x0, BIT13|BIT12);
            if(u8Val!=PAD_UNKNOWN)
                printk("PWM pad set failed\n");

        }
    }
    else if( 7 == u8Id )
    {
        if(u8Val==PAD_GPIO13){
            OUTREGMSK16(BASE_REG_PADTOP1_PA + REG_ID_09, BIT14, BIT15|BIT14);
        }else if(u8Val==PAD_MIPI_TX_IO7){
            OUTREGMSK16(BASE_REG_PADTOP1_PA + REG_ID_09, BIT15, BIT15|BIT14);
        }else if(u8Val==PAD_PM_LED1){
            OUTREGMSK16(BASE_REG_PADTOP1_PA + REG_ID_09, BIT15|BIT14, BIT15|BIT14);
		}else{
            OUTREGMSK16(BASE_REG_PADTOP1_PA + REG_ID_09, 0x0, BIT15|BIT14);
            if(u8Val!=PAD_UNKNOWN)
                printk("PWM pad set failed\n");

        }
    }
    else
    {
        printk(KERN_ERR "void DrvPWMEnable error!!!! (%x, %x)\r\n", u8Id, u8Val);
    }

}
