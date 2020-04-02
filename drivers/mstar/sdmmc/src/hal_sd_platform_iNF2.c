/***************************************************************************************************************
 *
 * FileName hal_sd_platform_iNF2.c
 *     @author jeremy.wang (2016/11/29)
 * Desc:
 *     The platform setting of all cards will run here.
 *     Because register setting that doesn't belong to FCIE/SDIO may have different register setting at different projects.
 *     The goal is that we don't need to change "other" HAL_XX.c Level code. (Timing, FCIE/SDIO)
 *
 *     The limitations were listed as below:
 *     (1) Each Project will have XX project name for different hal_card_platform_XX.c files.
 *     (2) IP init, PAD , clock, power and miu setting belong to here.
 *     (4) Timer setting doesn't belong to here, because it will be included by other HAL level.
 *     (5) FCIE/SDIO IP Reg Setting doesn't belong to here.
 *     (6) If we could, we don't need to change any code of hal_card_platform.h
 *
 ***************************************************************************************************************/

#include "../inc/hal_sd_platform.h"
#include "../inc/hal_sd_timer.h"

//***********************************************************************************************************
// Config Setting (Internel)
//***********************************************************************************************************

// Platform Register Basic Address
//------------------------------------------------------------------------------------
#define A_CHIPTOP_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0x80F00)  //Bank: 0x101E
#define A_PADTOP0_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0x81280)  //Bank: 0x1025
#define A_PADTOP1_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0x81300)  //Bank: 0x1026
#define A_CLKGEN2_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0x80500)  //Bank: 0x100A
#define A_PM_GPIO_BANK      GET_CARD_REG_ADDR(A_RIU_BASE, 0x00780)  //Bank: 0xF


//-----------------------------------------------------------------------------------------------------------
//IP_FCIE or IP_SDIO Register Basic Address
//-----------------------------------------------------------------------------------------------------------
#define A_SD_PLL_POS(IP)    GET_SD_BANK(IP, 3)


//-----------------------------------------------------------------------------------------------------------
//SD_PLL Register Basic Address
//-----------------------------------------------------------------------------------------------------------
#define A_PLL_FBDIV_REG(IP)                 GET_CARD_REG_ADDR(A_SD_PLL_POS(IP), 0x04)
#define A_PLL_PDIV_REG(IP)                  GET_CARD_REG_ADDR(A_SD_PLL_POS(IP), 0x05)
#define A_PLL_RESET_PLL_REG(IP)             GET_CARD_REG_ADDR(A_SD_PLL_POS(IP), 0x06)
#define A_PLL_TEST_REG(IP)                  GET_CARD_REG_ADDR(A_SD_PLL_POS(IP), 0x07)
#define A_PLL_DFFSET_15_0_REG(IP)           GET_CARD_REG_ADDR(A_SD_PLL_POS(IP), 0x18)
#define A_PLL_DFFSET_23_16_REG(IP)          GET_CARD_REG_ADDR(A_SD_PLL_POS(IP), 0x19)


// Clock Level Setting (From High Speed to Low Speed)
//-----------------------------------------------------------------------------------------------------------
                        //xtal:13M           //xtal:12M
#define CLK1_F          200000000
#define CLK1_E          100000000
#define CLK1_D          48000000
#define CLK1_C          44000000            //43200000
#define CLK1_B          40000000
#define CLK1_A          36750000            //36000000
#define CLK1_9          32000000
#define CLK1_8          20000000
#define CLK1_7          13000000            //12000000
#define CLK1_6          400000              //alway 400KHz for DTS
#define CLK1_5          0
#define CLK1_4          0
#define CLK1_3          0
#define CLK1_2          0
#define CLK1_1          0
#define CLK1_0          0


#define CLK2_F          200000000
#define CLK2_E          100000000
#define CLK2_D          48000000
#define CLK2_C          44000000             //43200000
#define CLK2_B          40000000
#define CLK2_A          36750000             //36000000
#define CLK2_9          32000000
#define CLK2_8          20000000
#define CLK2_7          13000000             //12000000
#define CLK2_6          400000               //alway 400KHz for DTS
#define CLK2_5          0
#define CLK2_4          0
#define CLK2_3          0
#define CLK2_2          0
#define CLK2_1          0
#define CLK2_0          0


#define CLK3_F          200000000
#define CLK3_E          100000000
#define CLK3_D          48000000
#define CLK3_C          43200000
#define CLK3_B          40000000
#define CLK3_A          36000000
#define CLK3_9          32000000
#define CLK3_8          20000000
#define CLK3_7          12000000
#define CLK3_6          325000
#define CLK3_5          0
#define CLK3_4          0
#define CLK3_3          0
#define CLK3_2          0
#define CLK3_1          0
#define CLK3_0          0


// Reg Dynamic Variable
//-----------------------------------------------------------------------------------------------------------
static volatile BusTimingEmType ge_BusTiming[3] = {0};


static void _PLTSD_PLL_Auto_Turn_ONOFF_LDO(IPEmType eIP)
{

    PortEmType ePort = GET_SD_PORT(eIP);

    if(ePort == EV_PFCIE5_SDIO_PLL)
    {
        //1. Disable OSP : FALSE =>(Enable)
        CARD_REG_CLRBIT(GET_CARD_REG_ADDR(A_SD_PLL_POS(eIP), 0x1D), BIT15_T);

        //2. Set OSP counter[15:8] = 0x30
        CARD_REG_CLRBIT(GET_CARD_REG_ADDR(A_SD_PLL_POS(eIP), 0x37), BIT15_T|BIT14_T|BIT13_T|BIT12_T|BIT11_T|BIT10_T|BIT09_T|BIT08_T);
        CARD_REG_SETBIT(GET_CARD_REG_ADDR(A_SD_PLL_POS(eIP), 0x37), BIT13_T|BIT12_T);

        //3. Turning on LDO  1->0
        CARD_REG_SETBIT(GET_CARD_REG_ADDR(A_SD_PLL_POS(eIP), 0x37), BIT05_T);
        Hal_SD_TIMER_mDelay(10);
        CARD_REG_CLRBIT(GET_CARD_REG_ADDR(A_SD_PLL_POS(eIP), 0x37), BIT05_T);


    }
    else if(ePort == EV_PFCIE5_SDIO_PLL_NU)
    {
        //1. Turning off LDO  0->1
        CARD_REG_CLRBIT(GET_CARD_REG_ADDR(A_SD_PLL_POS(eIP), 0x37), BIT05_T);
        Hal_SD_TIMER_mDelay(10);
        CARD_REG_SETBIT(GET_CARD_REG_ADDR(A_SD_PLL_POS(eIP), 0x37), BIT05_T);
    }

}


#if !(EN_FPGA_TEST)
static BOOL_T _PLTSD_PLL_Switch_AVDD(IPEmType eIP, PADVddEmType ePADVdd)
{

    U16_T u16Reg = 1;
    U8_T u8Timeout = 0;
    PortEmType ePort = GET_SD_PORT(eIP);

    if(ePort != EV_PFCIE5_SDIO_PLL)
        return 0;

    if(ePADVdd == EV_NORVOL)
        CARD_REG_CLRBIT(GET_CARD_REG_ADDR(A_SD_PLL_POS(eIP), 0x37), BIT04_T|BIT02_T);  //Switch to 3.3V
    else if(ePADVdd == EV_LOWVOL)
        CARD_REG_SETBIT(GET_CARD_REG_ADDR(A_SD_PLL_POS(eIP), 0x37), BIT04_T|BIT02_T);  //Switch to 1.8V

    do
    {

        if(ePADVdd == EV_NORVOL)
            u16Reg = CARD_REG(GET_CARD_REG_ADDR(A_SD_PLL_POS(eIP), 0x10)) & BIT12_T;  // Check 3.3v Ready
        else if(ePADVdd == EV_LOWVOL)
            u16Reg = CARD_REG(GET_CARD_REG_ADDR(A_SD_PLL_POS(eIP), 0x10)) & BIT13_T;  // Check 1.8v Ready

        if(u16Reg)
            break;

        u8Timeout++;
        Hal_SD_TIMER_mSleep(1);

    } while(u8Timeout<20);


    if(u8Timeout>=20)
    {
        if(ePADVdd==EV_NORVOL)
            prtstring(">>>>> Over Time during 3.3v setting\r\n");
        else if(ePADVdd==EV_LOWVOL)
            prtstring(">>>>> Over Time during 1.8v setting\r\n");

        return 1;
    }

    //prtstring(">>>>> Time count = ("); prtU8(u8Timeout); prtstring(")\n");
    return 0;

}
#else //(EN_FPGA_TEST)
static BOOL_T _PLTSD_PLL_Switch_AVDD(IPEmType eIP, PADVddEmType ePADVdd)
{

    CARD_REG_CLRBIT(GET_CARD_REG_ADDR(A_SD_PLL_POS(eIP), 0x43), BIT06_T);           //PAD_LSOE : output mode
    CARD_REG_SETBIT(GET_CARD_REG_ADDR(A_SD_PLL_POS(eIP), 0x41), BIT06_T);           //PAD_LSOE : output 1 (using)

    CARD_REG_CLRBIT(GET_CARD_REG_ADDR(A_SD_PLL_POS(eIP), 0x43), BIT07_T);           //PAD_VSEL : output mode

    if(ePADVdd==EV_NORVOL)
        CARD_REG_CLRBIT(GET_CARD_REG_ADDR(A_SD_PLL_POS(eIP), 0x41), BIT07_T);       //PAD_VSEL : output 0
    else if(ePADVdd==EV_LOWVOL)
        CARD_REG_SETBIT(GET_CARD_REG_ADDR(A_SD_PLL_POS(eIP), 0x41), BIT07_T);       //PAD_VSEL : output 1


    return 0;
}
#endif  // #if !(EN_FPGA_TEST)


static void _PLTSD_PLL_1XClock_Setting(IPEmType eIP, U32_T u32ClkFromIPSet)
{
    U8_T u8PDIV_REG = BIT00_T;

    //(1) reset emmc pll
    CARD_REG_SETBIT(A_PLL_RESET_PLL_REG(eIP), BIT00_T);
    CARD_REG_CLRBIT(A_PLL_RESET_PLL_REG(eIP), BIT00_T);

    //(2) synth clock
    switch(u32ClkFromIPSet)
    {
        case CLK1_F:      //200000KHz

             //CARD_REG(A_PLL_DFFSET_23_16_REG(eIP)) = 0x22;
             //CARD_REG(A_PLL_DFFSET_15_0_REG(eIP)) = 0x8F5C;

            //195MHz
             CARD_REG(A_PLL_DFFSET_23_16_REG(eIP)) = 0x24;
             CARD_REG(A_PLL_DFFSET_15_0_REG(eIP)) = 0x03D8;
             u8PDIV_REG = BIT00_T;  //1
             break;

        case CLK1_E:      //100000KHz
             CARD_REG(A_PLL_DFFSET_23_16_REG(eIP)) = 0x45;
             CARD_REG(A_PLL_DFFSET_15_0_REG(eIP)) = 0x1EB8;
             u8PDIV_REG = BIT00_T;  //1
             break;

        case CLK1_D:   //48000MHz //DDR

             //The same with 195MHz with 8-bit macro
             CARD_REG(A_PLL_DFFSET_23_16_REG(eIP)) = 0x24;
             CARD_REG(A_PLL_DFFSET_15_0_REG(eIP)) = 0xD944;
             u8PDIV_REG = BIT00_T;  //1

             break;

    }

    //(3) VCO clock
    CARD_REG(A_PLL_FBDIV_REG(eIP)) = 0x06;

    //(4) 1X clock
    CARD_REG_CLRBIT(A_PLL_PDIV_REG(eIP), BIT02_T|BIT01_T|BIT00_T);
    CARD_REG_SETBIT(A_PLL_PDIV_REG(eIP), u8PDIV_REG);

    CARD_REG_CLRBIT(A_PLL_TEST_REG(eIP), BIT10_T); //0

    Hal_SD_TIMER_uDelay(100);


}


//***********************************************************************************************************
// IP Setting for SD Platform
//***********************************************************************************************************

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDPLT_InitIPOnce
 *     @author jeremy.wang (2017/12/5)
 * Desc: IP once setting , it's about platform setting (IP Clock Source, IP Power Enable, Special Setting)
 *
 * @param eIP : FCIE1/FCIE2/...
 ----------------------------------------------------------------------------------------------------------*/
void Hal_SDPLT_InitIPOnce(IPEmType eIP)
{

    _PLTSD_PLL_Auto_Turn_ONOFF_LDO(eIP);


    if(eIP == EV_IP_FCIE1)
    {
    }
    else if(eIP == EV_IP_FCIE2)
    {
    }


}


/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDPLT_IPBeginSettnig
 *     @author jeremy.wang (2017/12/5)
 * Desc: IP begin setting before every operation, it's about platform setting.
 *
 * @param eIP : FCIE1/FCIE2/...
 ----------------------------------------------------------------------------------------------------------*/
void Hal_SDPLT_IPBeginSettnig(IPEmType eIP)
{

    if(eIP == EV_IP_FCIE1)
    {
    }
    else if(eIP == EV_IP_FCIE2)
    {
    }
    else if(eIP == EV_IP_FCIE3)
    {
    }


}


/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDPLT_IPEndSettnig
 *     @author jeremy.wang (2017/12/5)
 * Desc: IP end setting after every operation, it's about platform setting.
 *
 * @param eIP : FCIE1/FCIE2/...
 ----------------------------------------------------------------------------------------------------------*/
void Hal_SDPLT_IPEndSettnig(IPEmType eIP)
{

    if(eIP == EV_IP_FCIE1)
    {
    }
    else if(eIP == EV_IP_FCIE2)
    {
    }
    else if(eIP == EV_IP_FCIE3)
    {
    }
}

//***********************************************************************************************************
// PAD Setting for SD Platform
//***********************************************************************************************************

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDPLT_SetPADPortPath
 *     @author jeremy.wang (2015/12/4)
 * Desc: Set PAD to connect IP Port
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param ePort : Port (But FCIE5 use it to decide FCIE or SDIO IP)
 * @param ePAD : PAD
 ----------------------------------------------------------------------------------------------------------*/
void Hal_SDPLT_SetPADPortPath(IPEmType eIP, PortEmType ePort, PADEmType ePAD)
{
    SET_SD_PORT(eIP, ePort);

    CARD_REG_CLRBIT(GET_CARD_REG_ADDR(A_PADTOP1_BANK, 0x00), BIT00_T);              //reg_all_pad_in => Close

    if(eIP == EV_IP_FCIE1)
    {
        if(ePAD == EV_PAD1)  //PAD_SDIO30
        {
            CARD_REG_SETBIT(GET_CARD_REG_ADDR(A_PADTOP1_BANK, 0x0A), BIT07_T);          //reg_sd30_mode = 1
        }

    }
    else if(eIP == EV_IP_FCIE2)
    {

        if(ePAD == EV_PAD2) //PAD_SD20
        {
            CARD_REG_CLRBIT(GET_CARD_REG_ADDR(A_PADTOP1_BANK, 0x0A), BIT09_T|BIT08_T);  //reg_sdio_mode = 0
            CARD_REG_SETBIT(GET_CARD_REG_ADDR(A_PADTOP1_BANK, 0x0A), BIT08_T);          //reg_sdio_mode = 1

            CARD_REG_CLRBIT(GET_CARD_REG_ADDR(A_CHIPTOP_BANK, 0x12), BIT05_T|BIT04_T|BIT01_T|BIT00_T); //reg_test_in/out_mode = 0

        }

    }

}


/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDPLT_InitPADPin
 *     @author jeremy.wang (2017/12/4)
 * Desc: Init PAD ( pull enable, pull up/down, driving strength)
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param ePAD : PAD
 ----------------------------------------------------------------------------------------------------------*/
void Hal_SDPLT_InitPADPin(IPEmType eIP, PADEmType ePAD)
{

    if(eIP == EV_IP_FCIE1)
    {
        if(ePAD == EV_PAD1) //PAD_SDI30
        {

            CARD_REG_SETBIT(GET_CARD_REG_ADDR(A_SD_PLL_POS(eIP), 0x47), BIT05_T|BIT04_T|BIT03_T|BIT02_T|BIT01_T|BIT00_T);   //CLK, CMD, D3, D2, D1, D0=> pull en

            CARD_REG_SETBIT(GET_CARD_REG_ADDR(A_SD_PLL_POS(eIP), 0x49), BIT04_T|BIT03_T|BIT02_T|BIT01_T|BIT00_T);   //CMD, D3, D2, D1, D0=> ps up
            CARD_REG_CLRBIT(GET_CARD_REG_ADDR(A_SD_PLL_POS(eIP), 0x49), BIT05_T);   //CLK => ps down

        }

    }
    else if(eIP == EV_IP_FCIE2)
    {
        if (ePAD == EV_PAD2) //PAD_SDIO20
        {
            //No Need: Default
        }
    }

}


/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDPLT_PullPADPin
 *     @author jeremy.wang (2017/12/4)
 * Desc: Pull PAD Pin for Special Purpose (Avoid Power loss.., Save Power)
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param ePAD : PAD
 * @param ePinPull : Pull up/Pull down
 ----------------------------------------------------------------------------------------------------------*/
void Hal_SDPLT_PullPADPin(IPEmType eIP, PADEmType ePAD, PinPullEmType ePinPull)
{

    if( (eIP == EV_IP_FCIE1) && (ePAD == EV_PAD1) )  //PAD_SDIO30
        {
            if(ePinPull ==EV_PULLDOWN)
            {
                CARD_REG_CLRBIT(GET_CARD_REG_ADDR(A_SD_PLL_POS(eIP), 0x49), BIT04_T|BIT03_T|BIT02_T|BIT01_T|BIT00_T);   //CMD, D3, D2, D1, D0=> ps up
            }
            else if(ePinPull == EV_PULLUP)
            {
                CARD_REG_SETBIT(GET_CARD_REG_ADDR(A_SD_PLL_POS(eIP), 0x49), BIT04_T|BIT03_T|BIT02_T|BIT01_T|BIT00_T);   //CMD, D3, D2, D1, D0=> ps up
            }
        }
    else if( (eIP == EV_IP_FCIE2) && (ePAD == EV_PAD2) ) //PAD//SDIO20
    {
        if(ePinPull ==EV_PULLDOWN)
        {
            CARD_REG_CLRBIT(GET_CARD_REG_ADDR(A_PADTOP1_BANK, 0x0A), BIT09_T|BIT08_T);  //reg_sdio_mode = 0
            CARD_REG_CLRBIT(GET_CARD_REG_ADDR(A_CHIPTOP_BANK, 0x12), BIT05_T|BIT04_T|BIT01_T|BIT00_T); //reg_test_in/out_mode = 0

            //D3, D2, D1, D0, CMD, CLK => output mode
            CARD_REG_CLRBIT(GET_CARD_REG_ADDR(A_PADTOP1_BANK, 0x6F), BIT05_T|BIT04_T|BIT03_T|BIT02_T|BIT01_T|BIT00_T);

            //D3, D2, D1, D0, CMD, CLK => output: 0
            CARD_REG_CLRBIT(GET_CARD_REG_ADDR(A_PADTOP1_BANK, 0x5A), BIT07_T|BIT06_T|BIT05_T|BIT04_T|BIT03_T|BIT02_T);

    }
        else if(ePinPull == EV_PULLUP)
    {
            CARD_REG_CLRBIT(GET_CARD_REG_ADDR(A_PADTOP1_BANK, 0x0A), BIT09_T|BIT08_T);  //reg_sdio_mode = 0
            CARD_REG_SETBIT(GET_CARD_REG_ADDR(A_PADTOP1_BANK, 0x0A), BIT08_T);          //reg_sdio_mode = 1

            CARD_REG_CLRBIT(GET_CARD_REG_ADDR(A_CHIPTOP_BANK, 0x12), BIT05_T|BIT04_T|BIT01_T|BIT00_T); //reg_test_in/out_mode = 0

            //D3, D2, D1, D0, CMD, CLK => input mode
            CARD_REG_SETBIT(GET_CARD_REG_ADDR(A_PADTOP1_BANK, 0x6F), BIT05_T|BIT04_T|BIT03_T|BIT02_T|BIT01_T|BIT00_T);

}
    }
}


//***********************************************************************************************************
// Clock Setting for SD Platform
//***********************************************************************************************************

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDPLT_SetClock
 *     @author jeremy.wang (2015/7/23)
 * Desc: Set Clock Level by Real Clock from IP
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param u32ClkFromIPSet : Clock Value From IP Source Set
 ----------------------------------------------------------------------------------------------------------*/
#if !(EN_FPGA_TEST)
void Hal_SDPLT_SetClock(IPEmType eIP, U32_T u32ClkFromIPSet, BOOL_T b1stRunFlag)
{
    PortEmType ePort = GET_SD_PORT(eIP);

    /** First Run Process **/
    if(b1stRunFlag)
    {
        if(ePort == EV_PFCIE5_SDIO_PLL)
    {

            CARD_REG_CLRBIT(GET_CARD_REG_ADDR(A_CLKGEN2_BANK,0x3C), BIT07_T|BIT06_T|BIT05_T|BIT04_T); //[5:2]: Clk_Sel [1]: Clk_i [0]: Clk_g
            CARD_REG_SETBIT(GET_CARD_REG_ADDR(A_CLKGEN2_BANK,0x3C), BIT06_T);

            // 432MHz,
        }
        }


    if(eIP == EV_IP_FCIE1)
    {

        CARD_REG_CLRBIT(GET_CARD_REG_ADDR(A_CLKGEN2_BANK,0x3E), BIT13_T|BIT12_T|BIT11_T|BIT10_T|BIT09_T|BIT08_T); //[5:2]: Clk_Sel [1]: Clk_i [0]: Clk_g
        CARD_REG_SETBIT(GET_CARD_REG_ADDR(A_CLKGEN2_BANK,0x3E), BIT14_T); // select clk_sdio_p1


        if( (ge_BusTiming[eIP]==EV_BUS_SDR50) || (ge_BusTiming[eIP]==EV_BUS_SDR104_HS200) || (ge_BusTiming[eIP]==EV_BUS_DDR50) )
        {

            CARD_REG_SETBIT(GET_CARD_REG_ADDR(A_CLKGEN2_BANK,0x3E), BIT13_T|BIT11_T|BIT10_T); // select clk_sd30_1x_p

            _PLTSD_PLL_1XClock_Setting(eIP, u32ClkFromIPSet);

            return ;
        }


        switch(u32ClkFromIPSet)
        {
            case CLK1_D:      //48000KHz         (CLK_upll_clk_192m_div4)
                CARD_REG_SETBIT(GET_CARD_REG_ADDR(A_CLKGEN2_BANK,0x3E), BIT13_T|BIT12_T|BIT11_T|BIT10_T); //15
                break;
            case CLK1_C:      //44000KHz, 43200KHz
                CARD_REG_SETBIT(GET_CARD_REG_ADDR(A_CLKGEN2_BANK,0x3E), BIT12_T|BIT10_T); //5
                break;
            case CLK1_B:      //40000KHz         (CLK_upll_clk_160m_div4)
                CARD_REG_SETBIT(GET_CARD_REG_ADDR(A_CLKGEN2_BANK,0x3E), BIT12_T);  //4
                break;
            case CLK1_A:      //36750KHz, 36000KHz
                CARD_REG_SETBIT(GET_CARD_REG_ADDR(A_CLKGEN2_BANK,0x3E), BIT11_T|BIT10_T); //3
                break;
            case CLK1_9:      //32000KHz          (CLK_upll_clk_160m_div5)
                CARD_REG_SETBIT(GET_CARD_REG_ADDR(A_CLKGEN2_BANK,0x3E), BIT11_T); // 2
                break;
            case CLK1_8:      //20000KHz          (CLK_upll_clk_160m_div8)
                CARD_REG_SETBIT(GET_CARD_REG_ADDR(A_CLKGEN2_BANK,0x3E), BIT10_T); //1
                break;
            case CLK1_7:      //13000KHz, 12000KHz (xtal:13:12 M)
                break;
            case CLK1_6:      //325KHz, 300KHz  (xtal:13:12 M)
                CARD_REG_SETBIT(GET_CARD_REG_ADDR(A_CLKGEN2_BANK,0x3E), BIT13_T|BIT12_T|BIT10_T); //13
                break;
            /*
            case CLK1_7:
                break;
            case CLK1_6:
                break;
            case CLK1_5:
                break;
            case CLK1_4:
                break;
            case CLK1_3:
                break;
            case CLK1_2:
                break;
            case CLK1_1:
                break;
            case CLK1_0:
                break;*/

        }

    }
    else if(eIP == EV_IP_FCIE2)
    {

        CARD_REG_CLRBIT(GET_CARD_REG_ADDR(A_CLKGEN2_BANK,0x3E), BIT05_T|BIT04_T|BIT03_T|BIT02_T|BIT01_T|BIT00_T); //[5:2]: Clk_Sel [1]: Clk_i [0]: Clk_g
        CARD_REG_SETBIT(GET_CARD_REG_ADDR(A_CLKGEN2_BANK,0x3E), BIT06_T); // select clk_sdio_p1


        if( (ge_BusTiming[eIP]==EV_BUS_SDR50) || (ge_BusTiming[eIP]==EV_BUS_SDR104_HS200) ||  (ge_BusTiming[eIP]==EV_BUS_DDR50) )
        {
            //CARD_REG_SETBIT(GET_CARD_REG_ADDR(A_CLKGEN_BANK,0x43), BIT13_T|BIT11_T|BIT10_T); //clk_sd30_1x_p
            //_PLTSD_PLL_1XClock_Setting(eIP, u32ClkFromIPSet);

        return;
        }


        switch(u32ClkFromIPSet)
        {
            case CLK2_D:      //48000KHz
                CARD_REG_SETBIT(GET_CARD_REG_ADDR(A_CLKGEN2_BANK,0x3E), BIT05_T|BIT04_T|BIT03_T|BIT02_T); //15
                break;
            case CLK2_C:      //44000KHz, 43200KHz
                CARD_REG_SETBIT(GET_CARD_REG_ADDR(A_CLKGEN2_BANK,0x3E), BIT04_T|BIT02_T); //5
                break;
            case CLK2_B:      //40000KHz         (CLK_upll_clk_160m_div4)
                CARD_REG_SETBIT(GET_CARD_REG_ADDR(A_CLKGEN2_BANK,0x3E), BIT04_T);  //4
                break;
            case CLK2_A:      //36750KHz, 36000KHz
                CARD_REG_SETBIT(GET_CARD_REG_ADDR(A_CLKGEN2_BANK,0x3E), BIT03_T|BIT02_T);  //3
                break;
            case CLK2_9:      //32000KHz          (CLK_upll_clk_160m_div5)
                CARD_REG_SETBIT(GET_CARD_REG_ADDR(A_CLKGEN2_BANK,0x3E), BIT03_T); // 2
                break;
            case CLK2_8:      //20000KHz          (CLK_upll_clk_160m_div8)
                CARD_REG_SETBIT(GET_CARD_REG_ADDR(A_CLKGEN2_BANK,0x3E), BIT02_T); //1
                break;
            case CLK2_7:      //13000KHz, 12000KHz (xtal:13:12 M)
                break;
            case CLK2_6:      //325KHz, 300KHz  (xtal:13:12 M)
                CARD_REG_SETBIT(GET_CARD_REG_ADDR(A_CLKGEN2_BANK,0x3E), BIT05_T|BIT04_T|BIT02_T); //13
                break;
            /*
            case CLK1_7:
                break;
            case CLK1_6:
                break;
            case CLK1_5:
                break;
            case CLK1_4:
                break;
            case CLK1_3:
                break;
            case CLK1_2:
                break;
            case CLK1_1:
                break;
            case CLK1_0:
                break;*/
        }

    }
    else if(eIP == EV_IP_FCIE3)
    {
        switch(u32ClkFromIPSet)
        {
            /*
            case CLK3_F:      //48000KHz
                break;
            case CLK3_E:      //43200KHz
                break;
            case CLK3_D:      //40000KHz
                break;
            case CLK3_C:      //36000KHz
                break;
            case CLK3_B:      //32000KHz
                break;
            case CLK3_A:      //24000KHz
                break;
            case CLK3_9:      //12000KHz
                break;
            case CLK3_8:      //300KHz
                break;
            case CLK2_4:
                break;
            case CLK_3:
                break;
            case CLK2_2:
                break;
            case CLK2_1:
                break;
            case CLK2_0:
                break;*/

        }

    }

}
#else //(EN_FPGA_TEST)
void Hal_SDPLT_SetClock(IPEmType eIP, U32_T u32ClkFromIPSet, BOOL_T b1stRunFlag)
{

    if(u32ClkFromIPSet==CLK1_8) //Low Speed
    {
        CARD_REG_SETBIT(GET_CARD_REG_ADDR(A_SD_PLL_POS(eIP), 0x1D), BIT00_T);         //300KHz
    }
    else
    {
        CARD_REG_CLRBIT(GET_CARD_REG_ADDR(A_SD_PLL_POS(eIP), 0x1D), BIT00_T);         //1.5MHz
    }

}
#endif //!(EN_FPGA_TEST)


/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDPLT_FindClockSetting
 *     @author jeremy.wang (2015/7/20)
 * Desc: Find Real Clock Level Setting by Reference Clock
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param u32ReffClk : Reference Clock Value
 * @param u8PassLevel : Pass Level to Clock Speed
 * @param u8DownLevel : Down Level to Decrease Clock Speed
 *
 * @return U32_T  : Real Clock
 ----------------------------------------------------------------------------------------------------------*/
U32_T Hal_SDPLT_FindClockSetting(IPEmType eIP, U32_T u32ReffClk, U8_T u8PassLevel, U8_T u8DownLevel)
{
    U8_T  u8LV = u8PassLevel;
    U32_T u32RealClk = 0;
    U32_T u32ClkArr[3][16] = { \
        {CLK1_F, CLK1_E, CLK1_D, CLK1_C, CLK1_B, CLK1_A, CLK1_9, CLK1_8, CLK1_7, CLK1_6, CLK1_5, CLK1_4, CLK1_3, CLK1_2, CLK1_1, CLK1_0} \
       ,{CLK2_F, CLK2_E, CLK2_D, CLK2_C, CLK2_B, CLK2_A, CLK2_9, CLK2_8, CLK2_7, CLK2_6, CLK2_5, CLK2_4, CLK2_3, CLK2_2, CLK2_1, CLK2_0} \
       ,{CLK3_F, CLK3_E, CLK3_D, CLK3_C, CLK3_B, CLK3_A, CLK3_9, CLK3_8, CLK3_7, CLK3_6, CLK3_5, CLK3_4, CLK3_3, CLK3_2, CLK3_1, CLK3_0} };

    for(; u8LV<16; u8LV++)
    {
        if( (u32ReffClk >= u32ClkArr[eIP][u8LV]) || (u8LV==15) || (u32ClkArr[eIP][u8LV+1]==0) )
        {
            u32RealClk = u32ClkArr[eIP][u8LV];
            break;
        }
    }

    /****** For decrease clock speed******/
    if( (u8DownLevel) && (u32RealClk) && ((u8LV+u8DownLevel)<=15) )
    {
        if(u32ClkArr[eIP][u8LV+u8DownLevel]>0) //Have Level for setting
            u32RealClk = u32ClkArr[eIP][u8LV+u8DownLevel];
    }

    return u32RealClk;
}


/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDPLT_SetBusTiming
 *     @author jeremy.wang (2015/7/20)
 * Desc: Platform Setting for different Bus Timing
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param eBusTiming : LOW/DEF/HS/SDR12/DDR...
 ----------------------------------------------------------------------------------------------------------*/
void Hal_SDPLT_SetBusTiming(IPEmType eIP, BusTimingEmType eBusTiming)
{
    ge_BusTiming[eIP] = eBusTiming;
}


//***********************************************************************************************************
// Power and Voltage Setting for Card Platform
//***********************************************************************************************************

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDPLT_PowerOn
 *     @author jeremy.wang (2017/5/24)
 * Desc: Power ON Card Power
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param ePAD : PAD
 * @param u16DelayMs : Delay ms for stable power
----------------------------------------------------------------------------------------------------------*/
#if !(EN_FPGA_TEST)
void Hal_SDPLT_PowerOn(IPEmType eIP, PADEmType ePAD, U16_T u16DelayMs)
{
    if(ePAD==EV_PAD1) //PAD_SDIO30
    {
        CARD_REG_CLRBIT(GET_CARD_REG_ADDR(A_PM_GPIO_BANK, 0x06), BIT00_T);           // output mode
        CARD_REG_CLRBIT(GET_CARD_REG_ADDR(A_PM_GPIO_BANK, 0x06), BIT01_T);           // output:0

    }
    else if(ePAD==EV_PAD2) //PAD_SD20
    {
        CARD_REG_CLRBIT(GET_CARD_REG_ADDR(A_PM_GPIO_BANK, 0x0D), BIT00_T);          // output mode
        CARD_REG_CLRBIT(GET_CARD_REG_ADDR(A_PM_GPIO_BANK, 0x0D), BIT01_T);          // output:0

    }

    Hal_SD_TIMER_mSleep(u16DelayMs);

    }
#else //(EN_FPGA_TEST)
void Hal_SDPLT_PowerOn(IPEmType eIP, PADEmType ePAD, U16_T u16DelayMs)
{

    CARD_REG_CLRBIT(GET_CARD_REG_ADDR(A_SD_PLL_POS(eIP), 0x43), BIT08_T);         //PAD_PWRCTL : output mode
    CARD_REG_CLRBIT(GET_CARD_REG_ADDR(A_SD_PLL_POS(eIP), 0x41), BIT08_T);         //PAD_PWRCTL : output 0

    Hal_SD_TIMER_mSleep(u16DelayMs);
}
#endif //!(EN_FPGA_TEST)


/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDPLT_PowerOff
 *     @author jeremy.wang (2017/5/24)
 * Desc: Power Off Card Power
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param ePAD : PAD
 * @param u16DelayMs :  Delay ms to confirm no any power
----------------------------------------------------------------------------------------------------------*/
#if !(EN_FPGA_TEST)
void Hal_SDPLT_PowerOff(IPEmType eIP, PADEmType ePAD, U16_T u16DelayMs)
{

    if( (ePAD==EV_PAD1)) //PAD_SD0
    {
        CARD_REG_CLRBIT(GET_CARD_REG_ADDR(A_PM_GPIO_BANK, 0x06), BIT00_T);           // output mode
        CARD_REG_SETBIT(GET_CARD_REG_ADDR(A_PM_GPIO_BANK, 0x06), BIT01_T);           // output:1

    }
    else if(ePAD==EV_PAD2) //PAD_SD1
    {
        CARD_REG_CLRBIT(GET_CARD_REG_ADDR(A_PM_GPIO_BANK, 0x0D), BIT00_T);          // output mode
        CARD_REG_SETBIT(GET_CARD_REG_ADDR(A_PM_GPIO_BANK, 0x0D), BIT01_T);          // output:1
    }

    Hal_SD_TIMER_mSleep(u16DelayMs);
}
#else //(EN_FPGA_TEST)
void Hal_SDPLT_PowerOff(IPEmType eIP, PADEmType ePAD, U16_T u16DelayMs)
{
        CARD_REG_CLRBIT(GET_CARD_REG_ADDR(A_SD_PLL_POS(eIP), 0x43), BIT08_T);         //PAD_PWRCTL : output mode
        CARD_REG_SETBIT(GET_CARD_REG_ADDR(A_SD_PLL_POS(eIP), 0x41), BIT08_T);         //PAD_PWRCTL : output 1

    Hal_SD_TIMER_mSleep(u16DelayMs);
}
#endif //!(EN_FPGA_TEST)


/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDPLT_SetPADVdd
 *     @author jeremy.wang (2017/6/12)
 * Desc:
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param ePAD : PAD
 * @param ePADVdd :
 * @param u16DelayMs : Delay ms after set PAD power
----------------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDPLT_SetPADVdd
 *     @author jeremy.wang (2018/1/29)
 * Desc:
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param ePAD : PAD
 * @param ePADVdd :
 * @param u16DelayMs : Delay ms after set PAD power
 *
 * @return BOOL_T  :
 ----------------------------------------------------------------------------------------------------------*/
BOOL_T Hal_SDPLT_SetPADVdd(IPEmType eIP, PADEmType ePAD, PADVddEmType ePADVdd, U16_T u16DelayMs)
    {
    BOOL_T bRet = FALSE;

    bRet = _PLTSD_PLL_Switch_AVDD(eIP, ePADVdd);


    if(ePAD == EV_PAD1) //Pad SD0
        {
    }
    else if(ePAD == EV_PAD2) //PAD_SD1
    {
    }

    Hal_SD_TIMER_mSleep(u16DelayMs);

    return bRet;
}


//***********************************************************************************************************
// Card Detect and GPIO Setting for SD Platform
//***********************************************************************************************************

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDPLT_CDZInitGPIO
 *     @author jeremy.wang (2017/9/15)
 * Desc: Init GPIO Setting for CDZ or other GPIO (Pull high/low and driving, base SD/GPIO mode setting)
 *
 * @param eGPIO : GPIO1/GPIO2/...
 * @param ePAD : PAD1/PAD2/..
 * @param bEnable : Enable GPIO or disable GPIO to avoid loss power
 ----------------------------------------------------------------------------------------------------------*/
void Hal_SDPLT_CDZInitGPIO(GPIOEmType eGPIO, PADEmType ePAD, BOOL_T bEnable)
{

    if( (eGPIO==EV_GPIO1) && (ePAD == EV_PAD1) ) //EV_GPIO1 for Slot 0
    {
        if(bEnable)
        {
            CARD_REG_SETBIT(GET_CARD_REG_ADDR(A_PM_GPIO_BANK, 0x04E), BIT00_T);         //input mode
        }
        /*
        { else
        {
        }*/
        }
    else if( (eGPIO == EV_GPIO2) && (ePAD == EV_PAD2) ) //EV_GPIO2 for Slot 1
    {
        if(bEnable)
        {
            CARD_REG_SETBIT(GET_CARD_REG_ADDR(A_PM_GPIO_BANK, 0x04F), BIT00_T);         //input mode

        }
        /*
        else
        {
        }
        */
    }

}


/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDPLT_CDZGetGPIOState
 *     @author jeremy.wang (2017/9/15)
 * Desc: Get GPIO input mode value (Include input mode setting)
 *
 * @param eGPIO : GPIO1/GPIO2/...
 * @param ePAD : PAD1/PAD2/..
 *
 * @return BOOL_T  : TRUE or FALSE
 ----------------------------------------------------------------------------------------------------------*/
BOOL_T Hal_SDPLT_CDZGetGPIOState(GPIOEmType eGPIO, PADEmType ePAD)
{
    U16_T u16Reg = 0;

    if( eGPIO==EV_GPIO1 ) //EV_GPIO1 for Slot 0
    {
        u16Reg = CARD_REG(GET_CARD_REG_ADDR(A_PM_GPIO_BANK, 0x4E)) & BIT02_T;


        if(!u16Reg) //Low Active
            return (TRUE);
        else
            return (FALSE);

    }
    if( eGPIO==EV_GPIO2 ) //EV_GPIO2 for Slot 1
    {

        u16Reg = CARD_REG(GET_CARD_REG_ADDR(A_PM_GPIO_BANK, 0x4F)) & BIT02_T;


        if(!u16Reg) //Low Active
            return (TRUE);
        else
            return (FALSE);

    }

    return (FALSE);
}


/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDPLT_CDZSetGPIOState
 *     @author jeremy.wang (2017/12/18)
 * Desc: Set GPIO output mode value (Include output mode setting), it's for SDIO WIFI control using
 *
 * @param eGPIO : GPIO1/GPIO2/...
 * @param ePAD : PAD1/PAD2/..
 * @param bOutputState : TRUE or FALSE
 ----------------------------------------------------------------------------------------------------------*/
void Hal_SDPLT_CDZSetGPIOState(GPIOEmType eGPIO, PADEmType ePAD, BOOL_T bOutputState)
{

    /*if( eGPIO==EV_GPIO1 ) //EV_GPIO1 for Slot 0
    {
        CARD_REG_CLRBIT(GET_CARD_REG_ADDR(A_PMGPIO_BANK, 0x05), BIT00_T);           //PMU_GPIO_OUT_EN

        if(bOutputState)
        {
            CARD_REG_SETBIT(GET_CARD_REG_ADDR(A_PMGPIO_BANK, 0x07), BIT00_T);        //PMU_GPIO_OUT=1
        }
        else
        {
            CARD_REG_CLRBIT(GET_CARD_REG_ADDR(A_PMGPIO_BANK, 0x07), BIT00_T);       //PMU_GPIO_OUT=0
        }
    }
    if( eGPIO==EV_GPIO2 ) //EV_GPIO2 for Slot 1
    {
    }
    if( eGPIO==EV_GPIO3 ) //EV_GPIO2 for Slot 1
    {
    }

    // Add a 500ms Delay after card removing to avoid the next card inserting issue
    if(bOutputState==1)
    {
        Hal_SD_TIMER_mSleep(500);
    }
    else // For 0->1, 1->0 stable
    {
        Hal_SD_TIMER_mSleep(1);
    }*/

}

//***********************************************************************************************************
#if (D_OS == D_OS__LINUX)
//***********************************************************************************************************

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDPLT_CDZGetGPIONum
 *     @author jeremy.wang (2017/12/20)
 * Desc: Get GPIO number for special platform (like Linux) to use it get irq number
 *
 * @param eGPIO : GPIO1/GPIO2/...
 * @param ePAD : PAD1/PAD2/...
 *
 * @return U32_T  : GPIO number
 ----------------------------------------------------------------------------------------------------------*/
U32_T Hal_SDPLT_CDZGetGPIONum(GPIOEmType eGPIO, PADEmType ePAD)
{
    S32_T s32GPIO = -1;

    /*if( eGPIO==EV_GPIO1 ) //EV_GPIO1 for Slot 0
    {
        //s32GPIO = DrvPadmuxGetGpio(IO_CHIP_INDEX_SD_CDZ);
    }
    else if( eGPIO==EV_GPIO2)
    {
    }*/

    if(s32GPIO>0)
        return (U32_T)s32GPIO;
    else
        return 0;
}


#include <linux/irq.h>
#include "../../../mstar/include/infinity/irqs.h"
extern struct irq_chip gic_arch_extn;

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDPLT_CDZSetGPIOIntAttr
 *     @author jeremy.wang (2018/1/8)
 * Desc: Set GPIO Interrupt Attribute (Option 1..5), it could design for different requirement
 *
 * @param eGPIO : GPIO1/GPIO2/...
 * @param u32CDZIntNum : Request IRQ NO
 * @param eGPIOOPT : Option1/Option2/...
 ----------------------------------------------------------------------------------------------------------*/
void Hal_SDPLT_CDZSetGPIOIntAttr(GPIOEmType eGPIO, U32_T u32CDZIntNum, GPIOOptEmType eGPIOOPT)
{

    //struct irq_data sd_irqdata;
    //struct irq_chip *chip;
    if( eGPIO==EV_GPIO1 ) //EV_GPIO1 for Slot 0
    {
            //sd_irqdata.irq = u32CDZIntNum;
            //sd_irqdata.hwirq = u32CDZIntNum;
    }
    else if( eGPIO==EV_GPIO2)
    {

    }

#ifdef CONFIG_ARM_GIC
    //chip= &gic_arch_extn;
#else
    //chip = irq_get_chip(sd_irqdata.irq);
#endif

    if(eGPIOOPT==EV_GPIO_OPT1) //clear interrupt
    {
        //chip->irq_ack(&sd_irqdata);
    }
    else if((eGPIOOPT==EV_GPIO_OPT2))
    {
    }
    else if((eGPIOOPT==EV_GPIO_OPT3))  //sd polarity _HI Trig for remove
    {
        //chip->irq_set_type(&sd_irqdata,(IRQ_TYPE_EDGE_FALLING|IRQ_TYPE_LEVEL_HIGH));
    }
    else if((eGPIOOPT==EV_GPIO_OPT4)) //sd polarity _LO Trig for insert
    {
        //chip->irq_set_type(&sd_irqdata,(IRQ_TYPE_EDGE_RISING|IRQ_TYPE_LEVEL_LOW));
    }


}


/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDPLT_CDZGPIOIntFilter
 *     @author jeremy.wang (2017/9/15)
 * Desc: GPIO Interrupt Filter, it could design to filter GPIO Interrupt (Many sources belong to the same one)
 *
 * @param eGPIO : GPIO1/GPIO2/...
 * @param ePAD : PAD1/PAD2/...
 *
 * @return BOOL_T  : TRUE or FALSE
 ----------------------------------------------------------------------------------------------------------*/
BOOL_T Hal_SDPLT_CDZGPIOIntFilter(GPIOEmType eGPIO, PADEmType ePAD)
{

    if( eGPIO==EV_GPIO1 ) //EV_GPIO1 for Slot 0
    {
        return (TRUE);
    }
    else if( eGPIO==EV_GPIO2 )
    {
         return (TRUE);
    }
    else if( eGPIO==EV_GPIO3 )
    {
         return (TRUE);
    }

    return (FALSE);
}

//***********************************************************************************************************
#endif //#if (D_OS == D_OS__LINUX)
//***********************************************************************************************************


//***********************************************************************************************************
// MIU Setting for Card Platform
//***********************************************************************************************************

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDPLT_TransMIUAddr
 *     @author jeremy.wang (2015/7/31)
 * Desc: Transfer original address to HW special dma address (MIU0/MIU1)
 *
 * @param u32DMAAddr : Original address
 *
 * @return U32_T  : DMA address
 ----------------------------------------------------------------------------------------------------------*/
volatile U32_T Hal_SDPLT_TransMIUAddr(volatile U32_T u32DMAAddr)
{
    return (volatile U32_T)(u32DMAAddr-0x20000000);
}
