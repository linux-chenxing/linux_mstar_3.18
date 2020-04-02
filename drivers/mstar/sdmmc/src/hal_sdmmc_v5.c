/***************************************************************************************************************
 *
 * FileName hal_sdmmc_v5.c  (Driver Version)
 *     @author jeremy.wang (2015/07/06)
 * Desc:
 *     HAL SD Driver will support basic SD function but not flow process.
 *     (1) It included Register and Buffer opertion code.
 *     (2) h file will has APIs for Driver, but just SD abstract, not register abstract.
 *
 ***************************************************************************************************************/

#include "../inc/hal_sdmmc_v5.h"
#include "../inc/hal_sd_timer.h"

#if(EN_BIND_CARD_INT)
#include "../inc/hal_card_intr_v5.h"  //inlcue but may not use
#endif


//***********************************************************************************************************
// Config Setting (Internel)
//***********************************************************************************************************

// Enable Setting
//-----------------------------------------------------------------------------------------------------------
#define EN_TRFUNC                   (FALSE)
#define EN_DUMPREG                  (TRUE)

#define EN_BYPASSMODE               (FALSE)    //BYPASS MODE or ADVANCE MODE(SDR/DDR)


// Retry Times
//-----------------------------------------------------------------------------------------------------------
#define RT_CLEAN_SDSTS              3
#define RT_CLEAN_MIEEVENT           3

// Wait Time
//-----------------------------------------------------------------------------------------------------------
#define WT_DAT0HI_END               1000    //(ms)
#define WT_EVENT_CIFD                500    //(ms)
#define WT_RESET                     100    //(ms)

//***********************************************************************************************************
//***********************************************************************************************************

// Reg Static Init Setting
//-----------------------------------------------------------------------------------------------------------
#define V_MIE_PATH_INIT     0
#define V_MMA_PRI_INIT      (R_MIU_R_PRIORITY|R_MIU_W_PRIORITY)
#define V_MIE_INT_EN_INIT   (R_DATA_END_IEN|R_CMD_END_IEN|R_SDIO_INT_IEN|R_BUSY_END_IEN)
#define V_RSP_SIZE_INIT     0
#define V_CMD_SIZE_INIT     (5<<8)
#define V_SD_CTL_INIT       0
#define V_SD_MODE_INIT      (R_CLK_EN)  // Add
#define V_SDIO_MODE_INIT    0
#define V_DDR_MODE_INIT     0

#define V_PLL_SKEW_SUM_INIT             ( (4<<4) & R_TUNE_SHOT_OFFSET_MASK)
#define V_PLL_RX_BPS_EN_INIT            0x3F
#define V_PLL_ATOP_BYP_RX_EN_INIT       (R_PLL_ATOP_BYP_RX_EN)


// Mask Range
//-----------------------------------------------------------------------------------------------------------
#define M_SD_ERRSTS         (R_DAT_RD_CERR|R_DAT_WR_CERR|R_DAT_WR_TOUT|R_CMD_NORSP|R_CMDRSP_CERR|R_DAT_RD_TOUT)
#define M_SD_MIEEVENT       (R_DATA_END|R_CMD_END|R_ERR_STS|R_BUSY_END_INT|R_R2N_RDY_INT)
#define M_RST_STS           (R_RST_MIU_STS|R_RST_MIE_STS|R_RST_MCU_STS)


// Mask Reg Value
//-----------------------------------------------------------------------------------------------------------
#define M_REG_STSERR(IP)        (CARD_REG(A_SD_STS_REG(IP)) & M_SD_ERRSTS)
#define M_REG_SDMIEEvent(IP)    (CARD_REG(A_MIE_EVENT_REG(IP)) & M_SD_MIEEVENT)
#define M_REG_GETDAT0(IP)       (CARD_REG(A_SD_STS_REG(IP)) & R_DAT0)


//-----------------------------------------------------------------------------------------------------------
//IP_FCIE or IP_SDIO Register Basic Address
//-----------------------------------------------------------------------------------------------------------
#define A_SD_REG_POS(IP)                GET_SD_BANK(IP, 0)
#define A_SD_CFIFO_POS(IP)              GET_CARD_REG_ADDR(A_SD_REG_POS(IP), 0x20)  //Always at FCIE5
#define A_SD_CIFD_POS(IP)               GET_SD_BANK(IP, 1)
#define A_SD_PLL_POS(IP)                GET_SD_BANK(IP, 3)


#define A_SD_CIFD_R_POS(IP)             GET_CARD_REG_ADDR(A_SD_CIFD_POS(IP), 0x00)
#define A_SD_CIFD_W_POS(IP)             GET_CARD_REG_ADDR(A_SD_CIFD_POS(IP), 0x20)

#define A_MIE_EVENT_REG(IP)             GET_CARD_REG_ADDR(A_SD_REG_POS(IP), 0x00)
#define A_MMA_PRI_REG_REG(IP)           GET_CARD_REG_ADDR(A_SD_REG_POS(IP), 0x02)
#define A_DMA_ADDR_15_0_REG(IP)         GET_CARD_REG_ADDR(A_SD_REG_POS(IP), 0x03)
#define A_DMA_ADDR_31_16_REG(IP)        GET_CARD_REG_ADDR(A_SD_REG_POS(IP), 0x04)
#define A_DMA_LEN_15_0_REG(IP)          GET_CARD_REG_ADDR(A_SD_REG_POS(IP), 0x05)
#define A_DMA_LEN_31_16_REG(IP)         GET_CARD_REG_ADDR(A_SD_REG_POS(IP), 0x06)
#define A_MIE_FUNC_CTL_REG(IP)          GET_CARD_REG_ADDR(A_SD_REG_POS(IP), 0x07)
#define A_JOB_BLK_CNT_REG(IP)           GET_CARD_REG_ADDR(A_SD_REG_POS(IP), 0x08)
#define A_BLK_SIZE_REG(IP)              GET_CARD_REG_ADDR(A_SD_REG_POS(IP), 0x09)
#define A_CMD_RSP_SIZE_REG(IP)          GET_CARD_REG_ADDR(A_SD_REG_POS(IP), 0x0A)
#define A_SD_MODE_REG(IP)               GET_CARD_REG_ADDR(A_SD_REG_POS(IP), 0x0B)
#define A_SD_CTL_REG(IP)                GET_CARD_REG_ADDR(A_SD_REG_POS(IP), 0x0C)
#define A_SD_STS_REG(IP)                GET_CARD_REG_ADDR(A_SD_REG_POS(IP), 0x0D)
#define A_BOOT_MOD_REG(IP)              GET_CARD_REG_ADDR(A_SD_REG_POS(IP), 0x0E)
#define A_DDR_MOD_REG(IP)               GET_CARD_REG_ADDR(A_SD_REG_POS(IP), 0x0F)
#define A_DDR_TOGGLE_CNT_REG(IP)        GET_CARD_REG_ADDR(A_SD_REG_POS(IP), 0x10)
#define A_SDIO_MODE_REG(IP)             GET_CARD_REG_ADDR(A_SD_REG_POS(IP), 0x11)
#define A_TEST_MODE_REG(IP)             GET_CARD_REG_ADDR(A_SD_REG_POS(IP), 0x15)

#define A_WR_SBIT_TIMER_REG(IP)         GET_CARD_REG_ADDR(A_SD_REG_POS(IP), 0x17)
#define A_RD_SBIT_TIMER_REG(IP)         GET_CARD_REG_ADDR(A_SD_REG_POS(IP), 0x18)

#define A_SDIO_DET_REG(IP)              GET_CARD_REG_ADDR(A_SD_REG_POS(IP), 0x2F)

#define A_CIFD_EVENT_REG(IP)            GET_CARD_REG_ADDR(A_SD_REG_POS(IP), 0x30)
#define A_CIFD_INT_EN_REG(IP)           GET_CARD_REG_ADDR(A_SD_REG_POS(IP), 0x31)

#define A_BOOT_REG(IP)                  GET_CARD_REG_ADDR(A_SD_REG_POS(IP), 0x37)

#define A_DBG_BUS0_REG(IP)              GET_CARD_REG_ADDR(A_SD_REG_POS(IP), 0x38)
#define A_DBG_BUS1_REG(IP)              GET_CARD_REG_ADDR(A_SD_REG_POS(IP), 0x39)
#define A_FCIE_RST_REG(IP)              GET_CARD_REG_ADDR(A_SD_REG_POS(IP), 0x3F)
#define A_NC_FUN_CTL_REG(IP)            GET_CARD_REG_ADDR(A_SD_REG_POS(IP), 0x63)

#define A_CFIFO_OFFSET(IP, OFFSET)      GET_CARD_REG_ADDR(A_SD_CFIFO_POS(IP), OFFSET)
#define A_CIFD_R_OFFSET(IP, OFFSET)     GET_CARD_REG_ADDR(A_SD_CIFD_R_POS(IP), OFFSET)
#define A_CIFD_W_OFFSET(IP, OFFSET)     GET_CARD_REG_ADDR(A_SD_CIFD_W_POS(IP), OFFSET)

//-----------------------------------------------------------------------------------------------------------
//SD_PLL Register Basic Address
//-----------------------------------------------------------------------------------------------------------

#define A_PLL_CLKPH_SKEW(IP)            GET_CARD_REG_ADDR(A_SD_PLL_POS(IP), 0x03)
#define A_PLL_EMMC_TEST(IP)             GET_CARD_REG_ADDR(A_SD_PLL_POS(IP), 0x1A)
#define A_PLL_ECO_EN(IP)                GET_CARD_REG_ADDR(A_SD_PLL_POS(IP), 0x1C)
#define A_PLL_EMMC_EN(IP)               GET_CARD_REG_ADDR(A_SD_PLL_POS(IP), 0x68)
#define A_PLL_SKEW_SUM(IP)              GET_CARD_REG_ADDR(A_SD_PLL_POS(IP), 0x69)
#define A_PLL_IO_BUS_WID(IP)            GET_CARD_REG_ADDR(A_SD_PLL_POS(IP), 0x6A)
#define A_PLL_DQS_PAGE_NO(IP)           GET_CARD_REG_ADDR(A_SD_PLL_POS(IP), 0x6B)
#define A_PLL_DQS_SUM(IP)               GET_CARD_REG_ADDR(A_SD_PLL_POS(IP), 0x6C)
#define A_PLL_DQS_IO_MODE(IP)           GET_CARD_REG_ADDR(A_SD_PLL_POS(IP), 0x6D)
#define A_PLL_RST_SUM(IP)               GET_CARD_REG_ADDR(A_SD_PLL_POS(IP), 0x6F)
#define A_PLL_AFIFO_SUM(IP)             GET_CARD_REG_ADDR(A_SD_PLL_POS(IP), 0x70)
#define A_PLL_TX_BPS_EN(IP)             GET_CARD_REG_ADDR(A_SD_PLL_POS(IP), 0x71)
#define A_PLL_RX_BPS_EN(IP)             GET_CARD_REG_ADDR(A_SD_PLL_POS(IP), 0x73)
#define A_PLL_ATOP_BYP_RX_EN(IP)        GET_CARD_REG_ADDR(A_SD_PLL_POS(IP), 0x74)


// Reg Dynamic Variable
//-----------------------------------------------------------------------------------------------------------
static RspStruct gst_RspStruct[3];
static volatile BOOL_T  gb_StopWProc[3] = {0};
static volatile U16_T   gu16_WT_NRC[3] = {0};             //Waiting Time for Nrc (us)

static volatile U16_T   gu16_SD_MODE_DatLine[3] = {0};
static volatile U16_T   gu16_DDR_MODE_REG[3] = {0};
static volatile U16_T   gu16_DDR_MODE_REG_ForR2N[3] = {0};

static volatile BOOL_T  gb_SDIODevice[3] = {0};
static volatile U16_T   gu16_SDIO_MODE_IntMode[3] = {0};
static volatile U16_T   gu16_SDIO_IntEn_RunOnce[3]  = {0};


// For SD_PLL, maybe not use
#if(EN_BIND_PLL)
static volatile U16_T   gu16_PLL_CLKPH_SKEW[3] = {0};
static volatile U16_T   gu16_PLL_EMMC_TEST[3] = {0};
static volatile U16_T   gu16_PLL_ECO_EN[3] = {0};
static volatile U16_T   gu16_PLL_EMMC_EN[3] = {0};
static volatile U16_T   gu16_PLL_SKEW_SUM[3] = {0};
static volatile U16_T   gu16_PLL_DQS_PAGE_NO[3] = {0};
static volatile U16_T   gu16_PLL_DQS_SUM[3] = {0};
static volatile U16_T   gu16_PLL_DDR_IO_MODE[3] = {0};
static volatile U16_T   gu16_PLL_AFIFO_SUM[3] = {0};
static volatile U16_T   gu16_PLL_TX_BPS_EN[3] ={0};
//static volatile U16_T   gu16_PLL_RX_BPS_EN[3] = {0};
//static volatile U16_T   gu16_PLL_ATOP_BYP_RX_EN[3] = {0};

#define V_MAX_PHASE     18
static volatile U8_T    gu8Arr_PASS_PHS[3][V_MAX_PHASE] = {{0},{0}};
static volatile U8_T    gu8_PASS_PH_CNT[3] = {0};

#endif  //#if(EN_BIND_PLL)

static volatile U8_T*   gpu8Buf[3];


// Trace Funcion
//-----------------------------------------------------------------------------------------------------------
#if (EN_TRFUNC)
    #define TR_H_SDMMC(p)       p
#else
    #define TR_H_SDMMC(p)
#endif


// Mem Opertation Define ==> For FCIE5 Special FCIE/SDIO Function Ctrl Setting
//-----------------------------------------------------------------------------------------------------------
/*static void _MEM_CLEAR(volatile U8_T * u8Arr, U16_T u16Size)
{
    U16_T u16Pos;

    for(u16Pos=0; u16Pos<u16Size; u16Pos++)
    {
        u8Arr[u16Pos]=0;
    }
}*/


// IS (or not) Define => For PLL PAD check
//-----------------------------------------------------------------------------------------------------------
#if (EN_BIND_PLL)
static BOOL_T _IS_PLLPAD(IPEmType eIP)
{
    PortEmType ePort = GET_SD_PORT(eIP);

    return (ePort==EV_PFCIE5_SDIO_PLL ? (TRUE) : (FALSE));
}


static void _REG_SetSDRSCanVal(IPEmType eIP, U8_T u8Skew4, U8_T u8Skew4Inv)
{
    gu16_PLL_CLKPH_SKEW[eIP] = (u8Skew4<<12) &  R_PLL_CLKPH_SKEW4_MASK;
    gu16_PLL_SKEW_SUM[eIP] = V_PLL_SKEW_SUM_INIT | ((u8Skew4Inv<<11) & R_CLK_SKEW_INV);
}

static void _REG_SetDDRScanVal(IPEmType eIP, U8_T u8DQS_Mode, U8_T u8DQS_DLY_Sel)
{
    gu16_PLL_DQS_SUM[eIP] = (u8DQS_Mode & R_PLL_DQS_MODE_MASK);
    gu16_PLL_DQS_SUM[eIP] |= ( (u8DQS_DLY_Sel<<4) & R_PLL_DQS_DLY_SEL_MASK );

}

#endif  //End of  #if (EN_BIND_PLL)

// Register Opertation Define ==> For FCIE5 Special FCIE/SDIO Function Ctrl Setting
//-----------------------------------------------------------------------------------------------------------
static U16_T _REG_GetMIEFunCtlSetting(IPEmType eIP)
{
    PortEmType ePort = GET_SD_PORT(eIP);

    return (ePort==EV_PFCIE5_FCIE ? R_SD_EN : R_SDIO_MODE);
}


// Register Operation Define ==> For Clean Reg and Special Case
//-----------------------------------------------------------------------------------------------------------
static RetEmType _REG_ClearSDSTS(IPEmType eIP, U8_T u8Retry)
{
    do
    {
        CARD_REG_SETBIT(A_SD_STS_REG(eIP), M_SD_ERRSTS);

        if ( gb_StopWProc[eIP] )
            break;

        if ( !M_REG_STSERR(eIP) )
            return EV_OK;
        else if(!u8Retry)
            break;

    } while(u8Retry--);

    return EV_FAIL;
}


// Register Operation Define ==> For Clear MIE Event
//-----------------------------------------------------------------------------------------------------------
static RetEmType _REG_ClearMIEEvent(IPEmType eIP, U8_T u8Retry)
{
    /****** Clean global MIEEvent for Interrupt ******/
#if(EN_BIND_CARD_INT)
    Hal_CARD_INT_ClearMIEEvent(eIP);
#endif

    /****** Clean MIEEvent Reg *******/
    do
    {
        CARD_REG(A_MIE_EVENT_REG(eIP)) = M_SD_MIEEVENT;

        if ( gb_StopWProc[eIP] )
            break;

        if ( !M_REG_SDMIEEvent(eIP) )
            return EV_OK;
        else if( !u8Retry )
            break;

    }while( u8Retry-- );

    return EV_FAIL;

}


// Register Operation Define ==> For Wait DAT0 High
//-----------------------------------------------------------------------------------------------------------
static RetEmType _REG_WaitDat0HI(IPEmType eIP, U32_T u32WaitMs)
{
    U32_T u32DiffTime = 0;

#if(EN_BIND_CARD_INT)
    if ( Hal_CARD_INT_MIEIntRunning(eIP, EV_INT_SD) )   // Interrupt Mode
    {
        CARD_REG_SETBIT(A_SD_CTL_REG(eIP), R_BUSY_DET_ON );

        if ( !Hal_CARD_INT_WaitMIEEvent(eIP, R_BUSY_END_INT, u32WaitMs) )
            return EV_FAIL;
        else
            return EV_OK;

    }
    else // Polling Mode
#endif
    {
    do
    {

        if ( gb_StopWProc[eIP] )
            return EV_FAIL;

        if ( M_REG_GETDAT0(eIP) )
            return EV_OK;

        Hal_SD_TIMER_uDelay(1);
        u32DiffTime++;
    }while( u32DiffTime <= (u32WaitMs*1000) );

    }
    return EV_FAIL;

}


// Register Operation Define ==> For Wait MIE Event or CIFD Event
//-----------------------------------------------------------------------------------------------------------
static RetEmType _REG_WaitEvent(IPEmType eIP, IPEventEmType eEvent, U16_T u16ReqEvent, U32_T u32WaitMs)
{
    U32_T u32DiffTime = 0;

#if(EN_BIND_CARD_INT)
    if ( Hal_CARD_INT_MIEIntRunning(eIP, EV_INT_SD) )   // Interrupt Mode
    {
        if(eEvent == EV_MIE)
        {
            if ( !Hal_CARD_INT_WaitMIEEvent(eIP, u16ReqEvent, u32WaitMs) )
                return EV_FAIL;
            else
                return EV_OK;
        }

    }
    else // Polling Mode
#endif
    {
        do
        {

            if ( gb_StopWProc[eIP] )
                return EV_FAIL;

            if(eEvent == EV_MIE)
            {
                if ( (CARD_REG(A_MIE_EVENT_REG(eIP))&u16ReqEvent) == u16ReqEvent )
                    return EV_OK;
            }
            else if(eEvent == EV_CIFD )
            {
                if ( (CARD_REG(A_CIFD_EVENT_REG(eIP))&u16ReqEvent) == u16ReqEvent )
                    return EV_OK;
            }

            Hal_SD_TIMER_mDelay(1);
            u32DiffTime++;
        }while(u32DiffTime <= u32WaitMs);

    }

    return EV_FAIL;

}


// Register Operation Define ==> For Software Reset
//-----------------------------------------------------------------------------------------------------------
static void _REG_ResetIP(IPEmType eIP)
{
    U32_T u32DiffTime = 0;

    CARD_REG_CLRBIT(A_SD_CTL_REG(eIP), R_JOB_START);  //Clear For Safe ?
    CARD_REG_CLRBIT(A_FCIE_RST_REG(eIP), R_FCIE_SOFT_RST);

    do
    {
        if( (CARD_REG(A_FCIE_RST_REG(eIP)) & M_RST_STS) == M_RST_STS )
            break;
        Hal_SD_TIMER_uDelay(1);
        u32DiffTime++;
    }while ( u32DiffTime <= (1000*WT_RESET) );

    if ( u32DiffTime > (1000*WT_RESET) )
        prtstring("[HSD] IP Reset Switch Low Fail !!\r\n");


    u32DiffTime = 0;
    CARD_REG_SETBIT(A_FCIE_RST_REG(eIP), R_FCIE_SOFT_RST);

    do
    {
        if( (CARD_REG(A_FCIE_RST_REG(eIP)) & M_RST_STS) == 0 )
            break;
        Hal_SD_TIMER_uDelay(1);
        u32DiffTime++;

    }while ( u32DiffTime <= (1000* WT_RESET) );

    if ( u32DiffTime > (1000*WT_RESET) )
        prtstring("[HSD] IP Reset Switch High Fail !!\r\n");

    #if (EN_BIND_PLL)     //Reset PLL IP
    if(_IS_PLLPAD(eIP))
    {
        CARD_REG_CLRBIT(A_PLL_RST_SUM(eIP), R_PLL_MACRO_SW_RSTZ|R_PLL_DQS_CNT_RSTN);
        Hal_SD_TIMER_mSleep(1);
        CARD_REG_SETBIT(A_PLL_RST_SUM(eIP), R_PLL_MACRO_SW_RSTZ|R_PLL_DQS_CNT_RSTN);
    }
    #endif  //End of  #if (EN_BIND_PLL)


}


// IP Buffer Operation => Get Byte Value form Register
//-----------------------------------------------------------------------------------------------------------
static U8_T _BUF_GetByteFromRegAddr(volatile void *pBuf, U16_T u16Pos)
{
    if(u16Pos & 0x1)
        return CARD_REG_H8(GET_CARD_REG_ADDR(pBuf, u16Pos>>1));
    else
        return CARD_REG_L8(GET_CARD_REG_ADDR(pBuf, u16Pos>>1));

}


// IP Buffer Operation => CIFD FIFO Buffer Operation Define
//-----------------------------------------------------------------------------------------------------------
void _BUF_CIFD_DATA_IO(IPEmType eIP, CmdEmType eCmdType, volatile U16_T *pu16Buf, U8_T u8WordCnt)
{
    U8_T u8Pos = 0;

    for ( u8Pos = 0; u8Pos < u8WordCnt; u8Pos++ )
    {
        if ( eCmdType==EV_CMDREAD )
            pu16Buf[u8Pos] = CARD_REG(A_CIFD_R_OFFSET(eIP, u8Pos));
        else
            CARD_REG(A_CIFD_W_OFFSET(eIP, u8Pos)) = pu16Buf[u8Pos];
    }

}


// IP Buffer Operation => CIFD FIFO Buffer Operation for waiting FCIE5 special Event
//-----------------------------------------------------------------------------------------------------------
static RetEmType _BUF_CIFD_WaitEvent(IPEmType eIP,  CmdEmType eCmdType, volatile U8_T *pu8R2NBuf)
{
    U8_T u8RegionNo = 0, u8RegionMax = 0, u8RemainByte = 0;
    U32_T u32_TranLen  =  CARD_REG(A_DMA_LEN_15_0_REG(eIP)) + ( CARD_REG(A_DMA_LEN_31_16_REG(eIP)) << 16 );

    u8RemainByte = u32_TranLen & (64-1);  // u32_TranLen % 64
    u8RegionMax = (u32_TranLen>>6) + (u8RemainByte ? 1: 0);

    for(u8RegionNo=0; u8RegionNo<u8RegionMax; u8RegionNo++)
    {

        if ( eCmdType==EV_CMDREAD )
        {
            if ( _REG_WaitEvent(eIP, EV_CIFD, R_WBUF_FULL, WT_EVENT_CIFD) )
               return EV_FAIL;

           if ( (u8RegionNo == (u8RegionMax-1)) && (u8RemainByte>0) )
                _BUF_CIFD_DATA_IO(eIP, eCmdType, (volatile U16_T *)(pu8R2NBuf + (u8RegionNo << 6)), u8RemainByte/2);
           else
               _BUF_CIFD_DATA_IO(eIP, eCmdType, (volatile U16_T *)(pu8R2NBuf + (u8RegionNo << 6)), 32);

           CARD_REG(A_CIFD_EVENT_REG(eIP)) = R_WBUF_FULL;
           CARD_REG(A_CIFD_EVENT_REG(eIP)) = R_WBUF_EMPTY_TRIG;
        }
        else // Write
        {
            if ( (u8RegionNo == (u8RegionMax-1)) && (u8RemainByte>0) )
                 _BUF_CIFD_DATA_IO(eIP, eCmdType, (volatile U16_T *)(pu8R2NBuf + (u8RegionNo << 6)), u8RemainByte/2);
            else
                _BUF_CIFD_DATA_IO(eIP, eCmdType, (volatile U16_T *)(pu8R2NBuf + (u8RegionNo << 6)), 32);

             CARD_REG(A_CIFD_EVENT_REG(eIP)) = R_RBUF_FULL_TRIG;

            if ( _REG_WaitEvent(eIP, EV_CIFD, R_RBUF_EMPTY, WT_EVENT_CIFD) )
               return EV_FAIL;

             CARD_REG(A_CIFD_EVENT_REG(eIP)) = R_RBUF_EMPTY;

        }

    }

    return EV_OK;

}


// SDMMC Internel Logic Function
//-----------------------------------------------------------------------------------------------------------
static void _SDMMC_REG_Dump(IPEmType eIP)
{

#if (EN_DUMPREG)

    U8_T u8Pos, u8DGMode;

    prtstring("\n----------------------------------------------------\r\n");
    prtstring("  CMD_");
    prtUInt(gst_RspStruct[eIP].u8Cmd);
    prtstring(" (Arg: ");
    prtU32Hex(gst_RspStruct[eIP].u32Arg);
    prtstring(") [Line: ");
    prtUInt(gst_RspStruct[eIP].u32ErrLine);
    prtstring("]\r\n");
    prtstring("----------------------------------------------------\r\n");

    for(u8Pos = 0; u8Pos < gst_RspStruct[eIP].u8RspSize; u8Pos++)
    {
        if( (u8Pos == 0) || (u8Pos == 8) )
            prtstring("[");

        prtU8Hex(_BUF_GetByteFromRegAddr((volatile void *)A_SD_CFIFO_POS(eIP), u8Pos));
        prtstring(",");

        if( (u8Pos == 7) || (u8Pos == (gst_RspStruct[eIP].u8RspSize-1)) )
            prtstring("]\n");
    }

    prtstring("---------------DumpReg------------------------------\r\n");
    prtstring("[0x07][MIE_FUNC_CTL_REG]=    ");
    prtU16Hex(CARD_REG(A_MIE_FUNC_CTL_REG(eIP)));
    prtstring("\r\n");

    prtstring("[0x0B][SD_MODE_REG]=         ");
    prtU16Hex(CARD_REG(A_SD_MODE_REG(eIP)));
    prtstring("\r\n");

    prtstring("[0x0C][SD_CTL_REG]=          ");
    prtU16Hex(CARD_REG(A_SD_CTL_REG(eIP)));
    prtstring("\r\n");

    prtstring("[0x0F][DDR_MOD_REG]=         ");
    prtU16Hex(CARD_REG(A_DDR_MOD_REG(eIP)));
    prtstring("\r\n");

    prtstring("[0x0D][SD_STS_REG]=          ");
    prtU16Hex(CARD_REG(A_SD_STS_REG(eIP)));
    prtstring("\r\n");

#if(EN_BIND_CARD_INT)
    if ( Hal_CARD_INT_MIEIntRunning(eIP, EV_INT_SD) )   // Interrupt Mode
    {
        prtstring("[gu16_MIEEvent_ForInt]=      ");
        prtU16Hex(Hal_CARD_INT_GetMIEEvent(eIP));
        prtstring("\r\n");
    }
#endif

    prtstring("[0x00][MIE_EVENT_REG]=       ");
    prtU16Hex(CARD_REG(A_MIE_EVENT_REG(eIP)));
    prtstring("\r\n");

    prtstring("[0x03][MMA_ADDR_15_0_REG]=   ");
    prtU16Hex(CARD_REG(A_DMA_ADDR_15_0_REG(eIP)));
    prtstring("\r\n");

    prtstring("[0x04][MMA_ADDR_31_16_REG]=  ");
    prtU16Hex(CARD_REG(A_DMA_ADDR_31_16_REG(eIP)));
    prtstring("\r\n");

    prtstring("[0x05][MMA_LEN_15_0_REG]=    ");
    prtU16Hex(CARD_REG(A_DMA_LEN_15_0_REG(eIP)));
    prtstring("\r\n");

    prtstring("[0x06][MMA_LEN_31_16_REG]=   ");
    prtU16Hex(CARD_REG(A_DMA_LEN_31_16_REG(eIP)));
    prtstring("\r\n");

    prtstring("[0x08][JOB_BLK_CNT]=         ");
    prtU16Hex(CARD_REG(A_JOB_BLK_CNT_REG(eIP)));
    prtstring("\r\n");

    prtstring("[0x09][BLK_SIZE]=            ");
    prtU16Hex(CARD_REG(A_BLK_SIZE_REG(eIP)));
    prtstring("\r\n");

    prtstring("[0x37][BOOT_REG]=            ");
    prtU16Hex(CARD_REG(A_BOOT_REG(eIP)));
    prtstring("\r\n");

    #if (EN_BIND_PLL)
    if(_IS_PLLPAD(eIP))
    {
        prtstring("[0x03][PLL_CLKPH_SKEW]=      ");
        prtU16Hex(CARD_REG(A_PLL_CLKPH_SKEW(eIP)));
        prtstring("\r\n");

        prtstring("[0x1A][PLL_EMMC_TEST]=       ");
        prtU16Hex(CARD_REG(A_PLL_EMMC_TEST(eIP)));
        prtstring("\r\n");

        prtstring("[0x1C][PLL_ECO_EN]=          ");
        prtU16Hex(CARD_REG(A_PLL_ECO_EN(eIP)));
        prtstring("\r\n");

        prtstring("[0x68][PLL_EMMC_EN]=         ");
        prtU16Hex(CARD_REG(A_PLL_EMMC_EN(eIP)));
        prtstring("\r\n");

        prtstring("[0x69][PLL_SKEW_SUM]=        ");
        prtU16Hex(CARD_REG(A_PLL_SKEW_SUM(eIP)));
        prtstring("\r\n");

        prtstring("[0x69][PLL_SKEW_SUM]=        ");
        prtU16Hex(CARD_REG(A_PLL_SKEW_SUM(eIP)));
        prtstring("\r\n");

        prtstring("[0x6A][PLL_IO_BUS_WID]=      ");
        prtU16Hex(CARD_REG(A_PLL_IO_BUS_WID(eIP)));
        prtstring("\r\n");

        prtstring("[0x6B][PLL_DQS_PAGE_NO]=     ");
        prtU16Hex(CARD_REG(A_PLL_DQS_PAGE_NO(eIP)));
        prtstring("\r\n");

        prtstring("[0x6C][PLL_DQS_SUM]=         ");
        prtU16Hex(CARD_REG(A_PLL_DQS_SUM(eIP)));
        prtstring("\r\n");

        prtstring("[0x6D][PLL_DQS_IO_MODE]=     ");
        prtU16Hex(CARD_REG(A_PLL_DQS_IO_MODE(eIP)));
        prtstring("\r\n");

        prtstring("[0x6F][PLL_RST_SUM]=         ");
        prtU16Hex(CARD_REG(A_PLL_RST_SUM(eIP)));
        prtstring("\r\n");

        prtstring("[0x70][PLL_AFIFO_SUM]=       ");
        prtU16Hex(CARD_REG(A_PLL_AFIFO_SUM(eIP)));
        prtstring("\r\n");

        prtstring("[0x71][PLL_TX_BPS_EN]=       ");
        prtU16Hex(CARD_REG(A_PLL_TX_BPS_EN(eIP)));
        prtstring("\r\n");

        prtstring("[0x73][PLL_RX_BPS_EN]=       ");
        prtU16Hex(CARD_REG(A_PLL_RX_BPS_EN(eIP)));
        prtstring("\r\n");

        prtstring("[0x74][PLL_ATOP_BYP_RX_EN]=  ");
        prtU16Hex(CARD_REG(A_PLL_ATOP_BYP_RX_EN(eIP)));
        prtstring("\r\n");
    }
    #endif  //End of  #if (EN_BIND_PLL)

    prtstring("------[DEBUG_MODE]= CARD (1)(2)(3)(4)\r\n");
    CARD_REG_CLRBIT(A_DBG_BUS1_REG(eIP), R_DEBUG_MOD3 | R_DEBUG_MOD2 | R_DEBUG_MOD1 | R_DEBUG_MOD0);
    CARD_REG_SETBIT(A_DBG_BUS1_REG(eIP), R_DEBUG_MOD2 | R_DEBUG_MOD0); //Mode 5


    prtstring("[0x38][DEBUG_BUS0]=          ");
    for(u8DGMode = 1; u8DGMode <=4; u8DGMode++)
    {
        CARD_REG_CLRBIT(A_TEST_MODE_REG(eIP), R_SD_DEBUG_MOD2 | R_SD_DEBUG_MOD0 | R_SD_DEBUG_MOD1);
        CARD_REG_SETBIT(A_TEST_MODE_REG(eIP), (u8DGMode<<1));
        prtU16Hex(CARD_REG(A_DBG_BUS0_REG(eIP)));
        prtstring(", ");
    }
    prtstring("\r\n");
    prtstring("[0x39][DEBUG_BUS1]=          ");

    for(u8DGMode = 1; u8DGMode <=4; u8DGMode++)
    {
        CARD_REG_CLRBIT(A_TEST_MODE_REG(eIP), R_SD_DEBUG_MOD2 | R_SD_DEBUG_MOD0 | R_SD_DEBUG_MOD1);
        CARD_REG_SETBIT(A_TEST_MODE_REG(eIP), (u8DGMode<<1));
        prtU16Hex(CARD_REG(A_DBG_BUS1_REG(eIP)));
        prtstring(", ");

    }
    prtstring("\r\n");


    prtstring("------[DEBUG_MODE]= MMA\r\n");
    CARD_REG_CLRBIT(A_DBG_BUS1_REG(eIP), R_DEBUG_MOD3 | R_DEBUG_MOD2 | R_DEBUG_MOD1 | R_DEBUG_MOD0);
    CARD_REG_SETBIT(A_DBG_BUS1_REG(eIP), R_DEBUG_MOD1 | R_DEBUG_MOD0); //Mode 3

    prtstring("[0x38][DEBUG_BUS0]=          ");
    prtU16Hex(CARD_REG(A_DBG_BUS0_REG(eIP)));
    prtstring("\r\n");
    prtstring("[0x39][DEBUG_BUS1]=          ");
    prtU16Hex(CARD_REG(A_DBG_BUS1_REG(eIP)));
    prtstring("\r\n");

    prtstring("------[DEBUG_MODE]= REG_QIFX_CNT\r\n");
    CARD_REG_CLRBIT(A_DBG_BUS1_REG(eIP), R_DEBUG_MOD3 | R_DEBUG_MOD2 | R_DEBUG_MOD1 | R_DEBUG_MOD0);
    CARD_REG_SETBIT(A_DBG_BUS1_REG(eIP), R_DEBUG_MOD3); //Mode 8

    prtstring("[0x38][DEBUG_BUS0]=          ");
    prtU16Hex(CARD_REG(A_DBG_BUS0_REG(eIP)));
    prtstring("\r\n");
    prtstring("[0x39][DEBUG_BUS1]=          ");
    prtU16Hex(CARD_REG(A_DBG_BUS1_REG(eIP)));
    prtstring("\r\n");

    prtstring("------[DEBUG_MODE]= REG_QIFY_CNT\r\n");
    CARD_REG_CLRBIT(A_DBG_BUS1_REG(eIP), R_DEBUG_MOD3 | R_DEBUG_MOD2 | R_DEBUG_MOD1 | R_DEBUG_MOD0);
    CARD_REG_SETBIT(A_DBG_BUS1_REG(eIP), R_DEBUG_MOD3 | R_DEBUG_MOD0); //Mode 9

    prtstring("[0x38][DEBUG_BUS0]=          ");
    prtU16Hex(CARD_REG(A_DBG_BUS0_REG(eIP)));
    prtstring("\r\n");
    prtstring("[0x39][DEBUG_BUS1]=          ");
    prtU16Hex(CARD_REG(A_DBG_BUS1_REG(eIP)));
    prtstring("\r\n");

    prtstring("------[DEBUG_MODE]= REG_DMA_REQ_CNT\r\n");
    CARD_REG_CLRBIT(A_DBG_BUS1_REG(eIP), R_DEBUG_MOD3 | R_DEBUG_MOD2 | R_DEBUG_MOD1 | R_DEBUG_MOD0);
    CARD_REG_SETBIT(A_DBG_BUS1_REG(eIP), R_DEBUG_MOD3 | R_DEBUG_MOD1); //Mode A

    prtstring("[0x38][DEBUG_BUS0]=          ");
    prtU16Hex(CARD_REG(A_DBG_BUS0_REG(eIP)));
    prtstring("\r\n");
    prtstring("[0x39][DEBUG_BUS1]=          ");
    prtU16Hex(CARD_REG(A_DBG_BUS1_REG(eIP)));
    prtstring("\r\n");

    prtstring("------[DEBUG_MODE]= REG_DMA_RSP_CNT\r\n");
    CARD_REG_CLRBIT(A_DBG_BUS1_REG(eIP), R_DEBUG_MOD3 | R_DEBUG_MOD2 | R_DEBUG_MOD1 | R_DEBUG_MOD0);
    CARD_REG_SETBIT(A_DBG_BUS1_REG(eIP), R_DEBUG_MOD3 | R_DEBUG_MOD1 | R_DEBUG_MOD0); //Mode B

    prtstring("[0x38][DEBUG_BUS0]=          ");
    prtU16Hex(CARD_REG(A_DBG_BUS0_REG(eIP)));
    prtstring("\r\n");
    prtstring("[0x39][DEBUG_BUS1]=          ");
    prtU16Hex(CARD_REG(A_DBG_BUS1_REG(eIP)));
    prtstring("\r\n");


    prtstring("----------------------------------------------------\r\n");

#endif  //End #if(EN_DUMPREG)


}


static RspErrEmType _SDMMC_EndProcess(IPEmType eIP, RspErrEmType eRspErr, BOOL_T bCloseClk, int Line)
{
    U16_T u16RspErr = (U16_T)eRspErr;
    U16_T u16IPErr = EV_STS_RIU_ERR | EV_STS_MIE_TOUT | EV_STS_DAT0_BUSY;

    /****** (1) Record Information *******/
    gst_RspStruct[eIP].u32ErrLine = (U32_T)Line;
    gst_RspStruct[eIP].u8RspSize = (U8_T)CARD_REG(A_CMD_RSP_SIZE_REG(eIP));
    gst_RspStruct[eIP].eErrCode = eRspErr;

    /****** (2) Dump and the Reg Info + Reset IP *******/
    if ( u16RspErr && gb_StopWProc[eIP] )
    {
        eRspErr = EV_SWPROC_ERR;
        _REG_ResetIP(eIP);
    }
    else if( u16RspErr & u16IPErr ) //SD IP Error
    {
        _SDMMC_REG_Dump(eIP);
        _REG_ResetIP(eIP);
    }
    else if( u16RspErr & M_SD_ERRSTS ) //SD_STS Reg Error
    {
        //_SDMMC_REG_Dump(eIP);
        //Do Nothing
    }

    /****** (3) Close clock and DMA Stop function ******/
    if(bCloseClk && !gb_SDIODevice[eIP])
        CARD_REG_CLRBIT(A_SD_MODE_REG(eIP), (EN_N_CLOSE_CLK ? 0 : R_CLK_EN) | R_DMA_RD_CLK_STOP);

    return eRspErr;


}


//***********************************************************************************************************
// SDMMC Operation Function
//***********************************************************************************************************
/*----------------------------------------------------------------------------------------------------------
*
* Function: Hal_SDMMC_SetDataWidth
*     @author jeremy.wang (2015/7/9)
* Desc: According as Data Bus Width to Set IP DataWidth
*
* @param eIP : FCIE1/FCIE2/...
* @param eBusWidth : 1BIT/4BITs/8BITs
----------------------------------------------------------------------------------------------------------*/
void Hal_SDMMC_SetDataWidth(IPEmType eIP, SDMMCBusWidthEmType eBusWidth)
{
    gu16_SD_MODE_DatLine[eIP] = (U16_T)eBusWidth;
}


/*----------------------------------------------------------------------------------------------------------
*
* Function: Hal_SDMMC_SetBusTiming
*     @author jeremy.wang (2015/7/29)
* Desc:
*
* @param eIP : FCIE1/FCIE2/...
* @param eBusTiming : LOW/DEF/HS/SDR12/DDR...
----------------------------------------------------------------------------------------------------------*/
void Hal_SDMMC_SetBusTiming(IPEmType eIP, BusTimingEmType eBusTiming)
{

    switch ( eBusTiming )
    {

#if (EN_BYPASSMODE)

        case EV_BUS_LOW:
        case EV_BUS_DEF_SDR12:

            gu16_DDR_MODE_REG[eIP] = 0;
            gu16_DDR_MODE_REG_ForR2N[eIP] = gu16_DDR_MODE_REG[eIP] | R_PAD_IN_BYPASS;

            #if(EN_BIND_PLL)
            gu16_PLL_EMMC_TEST[eIP] = 0;
            gu16_PLL_ECO_EN[eIP] = 0;
            #endif  // #if(EN_BIND_PLL)

            break;

        case EV_BUS_HS_SDR25:

            gu16_DDR_MODE_REG[eIP] = 0;
            gu16_DDR_MODE_REG_ForR2N[eIP] = gu16_DDR_MODE_REG[eIP] | 0;

            #if(EN_BIND_PLL)
            gu16_PLL_EMMC_TEST[eIP] = 0;
            gu16_PLL_ECO_EN[eIP] = 0;
            #endif  // #if(EN_BIND_PLL)
            break;
#else
        case EV_BUS_LOW:
        case EV_BUS_DEF_SDR12:
        case EV_BUS_HS_SDR25:


            gu16_DDR_MODE_REG[eIP] = (R_PAD_CLK_SEL|R_PAD_IN_SEL|R_FALL_LATCH);
            gu16_DDR_MODE_REG_ForR2N[eIP] = gu16_DDR_MODE_REG[eIP] | R_PAD_IN_RDY_SEL | R_PRE_FULL_SEL0 | R_PRE_FULL_SEL1;


            #if(EN_BIND_PLL)
            gu16_PLL_CLKPH_SKEW[eIP] = 0;
            gu16_PLL_EMMC_TEST[eIP] = R_PLL_C2_EN;
            gu16_PLL_ECO_EN[eIP] = 0;
            gu16_PLL_EMMC_EN[eIP] = 0;
            gu16_PLL_SKEW_SUM[eIP] = V_PLL_SKEW_SUM_INIT;
            gu16_PLL_DQS_PAGE_NO[eIP] = 0;
            gu16_PLL_DQS_SUM[eIP] = 0;
            gu16_PLL_DDR_IO_MODE[eIP] = 0;
            //gu16_PLL_RST_SUM[eIP] = (R_PLL_MACRO_SW_RSTZ|R_PLL_DQS_CNT_RSTN|R_PLL_OSP_SW_RSTZ);;
            gu16_PLL_AFIFO_SUM[eIP] = 0;
            gu16_PLL_TX_BPS_EN[eIP] = 0x3F;
            //gu16_PLL_RX_BPS_EN[eIP] = 0x3F;
            //gu16_PLL_ATOP_BYP_RX_EN[eIP] = 1;
            #endif  // #if(EN_BIND_PLL)

            break;

#endif
        //Other can't run bypass mode
        case EV_BUS_SDR50:
        case EV_BUS_SDR104_HS200:

            gu16_DDR_MODE_REG[eIP] = (R_DDR_MACRO32_EN);
            gu16_DDR_MODE_REG_ForR2N[eIP] = 0;

            #if(EN_BIND_PLL)
            gu16_PLL_CLKPH_SKEW[eIP] = 0;
            gu16_PLL_EMMC_TEST[eIP] = (R_PLL_PAD_DRV|R_PLL_G_RX_W_OEN_DOUT|R_PLL_G_RX_W_OEN_COUT);
            gu16_PLL_ECO_EN[eIP] = R_PLL_32BIF_RX_ECO_EN;
            gu16_PLL_EMMC_EN[eIP] = R_PLL_EMMC_EN;
            gu16_PLL_SKEW_SUM[eIP] = V_PLL_SKEW_SUM_INIT;
            gu16_PLL_DQS_PAGE_NO[eIP] = 0x413;
            gu16_PLL_DQS_SUM[eIP] = 0;
            gu16_PLL_DDR_IO_MODE[eIP] = 0;
            gu16_PLL_AFIFO_SUM[eIP] = (R_PLL_SEL_FLASH_32BIF|R_PLL_RX_AFIFO_EN|R_PLL_RSP_AFIFO_EN);
            gu16_PLL_TX_BPS_EN[eIP] = 0;
            //gu16_PLL_RX_BPS_EN[eIP] = 0x3F;
            //gu16_PLL_ATOP_BYP_RX_EN[eIP] = 1;
            #endif  // #if(EN_BIND_PLL)

            break;

        case EV_BUS_DDR50:

            gu16_DDR_MODE_REG[eIP] = (R_DDR_MACRO_EN|R_DDR_EN);
            gu16_DDR_MODE_REG_ForR2N[eIP] = 0;

            #if(EN_BIND_PLL)
            gu16_PLL_CLKPH_SKEW[eIP] = 0;
            gu16_PLL_EMMC_TEST[eIP] = 0;
            gu16_PLL_ECO_EN[eIP] = R_PLL_1X_SYN_ECO_EN;
            gu16_PLL_EMMC_EN[eIP] = R_PLL_EMMC_EN;
            gu16_PLL_SKEW_SUM[eIP] = V_PLL_SKEW_SUM_INIT;
            gu16_PLL_DQS_PAGE_NO[eIP] = 0x213;
            gu16_PLL_DQS_SUM[eIP] = 0;
            gu16_PLL_DDR_IO_MODE[eIP] = R_PLL_DDR_IO_MODE;
            gu16_PLL_AFIFO_SUM[eIP] = 0;
            gu16_PLL_TX_BPS_EN[eIP] = 0x3F;
            //gu16_PLL_RX_BPS_EN[eIP] = 0x3F;
            //gu16_PLL_ATOP_BYP_RX_EN[eIP] = 1;
            #endif  // #if(EN_BIND_PLL)

            break;



    }


}


/*----------------------------------------------------------------------------------------------------------
*
* Function: Hal_SDMMC_SetNrcDelay
*     @author jeremy.wang (2015/7/9)
* Desc: According as Current Clock to Set Nrc Delay
*
* @param eIP : FCIE1/FCIE2/...
* @param u32RealClk : Real Clock
----------------------------------------------------------------------------------------------------------*/
void Hal_SDMMC_SetNrcDelay(IPEmType eIP, U32_T u32RealClk)
{

    if( u32RealClk >= 8000000 )         //>=8MHz
        gu16_WT_NRC[eIP] = 1;
    else if( u32RealClk >= 4000000 )    //>=4MHz
        gu16_WT_NRC[eIP] = 2;
    else if( u32RealClk >= 2000000 )    //>=2MHz
        gu16_WT_NRC[eIP] = 4;
    else if( u32RealClk >= 1000000 )    //>=1MHz
        gu16_WT_NRC[eIP] = 8;
    else if( u32RealClk >= 400000 )     //>=400KHz
        gu16_WT_NRC[eIP] = 20;
    else if( u32RealClk >= 300000 )     //>=300KHz
        gu16_WT_NRC[eIP] = 27;
    else if( u32RealClk >= 100000 )     //>=100KHz
        gu16_WT_NRC[eIP] = 81;
    else if(u32RealClk==0)
        gu16_WT_NRC[eIP] = 100;          //Default

}


/*----------------------------------------------------------------------------------------------------------
*
* Function: Hal_SDMMC_SetCmdToken
*     @author jeremy.wang (2015/7/8)
* Desc: Set Cmd Token
*
* @param eIP : FCIE1/FCIE2/...
* @param u8Cmd : SD Command
* @param u32Arg : SD Argument
----------------------------------------------------------------------------------------------------------*/
void Hal_SDMMC_SetCmdToken(IPEmType eIP, U8_T u8Cmd, U32_T u32Arg)
{
    gst_RspStruct[eIP].u8Cmd        = u8Cmd;
    gst_RspStruct[eIP].u32Arg       = u32Arg;

    CARD_REG(A_CFIFO_OFFSET(eIP, 0)) = (((U8_T)(u32Arg>>24))<<8) | (0x40 + u8Cmd);
    CARD_REG(A_CFIFO_OFFSET(eIP, 1)) = (((U8_T)(u32Arg>>8))<<8) | ((U8_T)(u32Arg>>16));
    CARD_REG(A_CFIFO_OFFSET(eIP, 2)) = (U8_T)u32Arg;

    TR_H_SDMMC(prtstring("[I_")); TR_H_SDMMC(prtU8(eIP));
    TR_H_SDMMC(prtstring("] CMD_")); TR_H_SDMMC(prtU8(u8Cmd));

    TR_H_SDMMC(prtstring(" ("));
    TR_H_SDMMC(prtU32Hex(u32Arg));
    TR_H_SDMMC(prtstring(")"));


}


/*----------------------------------------------------------------------------------------------------------
*
* Function: Hal_SDMMC_GetRspToken
*     @author jeremy.wang (2015/7/9)
* Desc: Get Command Response Info
*
* @param eIP : FCIE1/FCIE2/...
*
* @return RspStruct*  : Response Struct
----------------------------------------------------------------------------------------------------------*/
RspStruct* Hal_SDMMC_GetRspToken(IPEmType eIP)
{
    U8_T u8Pos;

    TR_H_SDMMC(prtstring(" =>RSP: ("));
    TR_H_SDMMC(prtU16Hex((U16_T)gst_RspStruct[eIP].eErrCode));
    TR_H_SDMMC(prtstring(")\r\n"));

    for(u8Pos=0; u8Pos<0x10; u8Pos++ )
        gst_RspStruct[eIP].u8ArrRspToken[u8Pos] = 0;

    TR_H_SDMMC(prtstring("["));

    for(u8Pos=0; u8Pos< gst_RspStruct[eIP].u8RspSize; u8Pos++)
    {
        gst_RspStruct[eIP].u8ArrRspToken[u8Pos] = _BUF_GetByteFromRegAddr((volatile void *)A_SD_CFIFO_POS(eIP), u8Pos);
        TR_H_SDMMC(prtU8Hex(gst_RspStruct[eIP].u8ArrRspToken[u8Pos]));
        TR_H_SDMMC(prtstring(", "));

    }

    TR_H_SDMMC(prtstring("]\r\n\r\n"));

    return &gst_RspStruct[eIP];

}


/*----------------------------------------------------------------------------------------------------------
*
* Function: Hal_SDMMC_TransCmdSetting
*     @author jeremy.wang (2015/7/15)
* Desc: For Data Transfer Setting
*
* @param eIP : FCIE1/FCIE2/...
* @param eTransType : CIFD/DMA/ADMA/NONE
* @param u16BCnt : Block Cnt
* @param u16BlkSize : Block Size
* @param u32BufAddr : Memory Address or DMA Table Address (32bits)
* @param pu8Buf : If run CIFD, it neet the buf pointer to do io between CIFD and Buf
----------------------------------------------------------------------------------------------------------*/
void Hal_SDMMC_TransCmdSetting(IPEmType eIP, TransEmType eTransType, U16_T u16BlkCnt, U16_T u16BlkSize, volatile U32_T u32BufAddr, volatile U8_T *pu8Buf)
{
    U32_T u32_TranLen = u16BlkCnt * u16BlkSize;

    CARD_REG(A_BLK_SIZE_REG(eIP)) = u16BlkSize;

    if (eTransType==EV_ADMA)
    {
        CARD_REG(A_JOB_BLK_CNT_REG(eIP)) = 1;        //ADMA BLK_CNT = 1
        CARD_REG(A_DMA_LEN_15_0_REG(eIP))  = 0x10;   //ADMA Fixed Value = 0x10
        CARD_REG(A_DMA_LEN_31_16_REG(eIP)) = 0;
    }
    else //R2N or DMA
    {
        CARD_REG(A_JOB_BLK_CNT_REG(eIP)) = u16BlkCnt;
        CARD_REG(A_DMA_LEN_15_0_REG(eIP))  = (U16_T)(u32_TranLen & 0xFFFF);
        CARD_REG(A_DMA_LEN_31_16_REG(eIP)) = (U16_T)(u32_TranLen >> 16);
    }

    if( (eTransType== EV_DMA) || (eTransType==EV_ADMA) )
    {
        CARD_REG(A_DMA_ADDR_15_0_REG(eIP))  = (U16_T)(u32BufAddr & 0xFFFF);
        CARD_REG(A_DMA_ADDR_31_16_REG(eIP)) = (U16_T)(u32BufAddr >> 16);
    }
    else //CIFD (R2N)
    {
        gpu8Buf[eIP] = pu8Buf;
    }

}


/*----------------------------------------------------------------------------------------------------------
*
* Function: Hal_SDMMC_SendCmdAndWaitProcess
*     @author jeremy.wang (2015/7/14)
* Desc: Send SD Command and Waiting for Process
*
* @param eIP :  FCIE1/FCIE2/...
* @param eTransType : CIFD/DMA/ADMA/NONE
* @param eCmdType : CMDRSP/READ/WRITIE
* @param eRspType : R1/R2/R3/...
* @param bCloseClk : Close Clock or not
*
* @return RspErrEmType  : Response Error Code
----------------------------------------------------------------------------------------------------------*/
RspErrEmType Hal_SDMMC_SendCmdAndWaitProcess(IPEmType eIP, TransEmType eTransType, CmdEmType eCmdType, SDMMCRspEmType eRspType, BOOL_T bCloseClk)
{
    U32_T u32WaitMS = WT_EVENT_RSP;
    U16_T u16WaitMIEEvent = R_CMD_END;

    CARD_REG(A_CMD_RSP_SIZE_REG(eIP)) = V_CMD_SIZE_INIT | ((U8_T)eRspType);
    CARD_REG(A_MIE_FUNC_CTL_REG(eIP)) = V_MIE_PATH_INIT | _REG_GetMIEFunCtlSetting(eIP);
    CARD_REG(A_NC_FUN_CTL_REG(eIP)) = 0;

    CARD_REG(A_SD_MODE_REG(eIP)) = V_SD_MODE_INIT | (eTransType>>8) | gu16_SD_MODE_DatLine[eIP] | ((U8_T)(eTransType & R_DMA_RD_CLK_STOP));
    CARD_REG(A_SD_CTL_REG(eIP))  = V_SD_CTL_INIT | (eRspType>>12) | (eCmdType>>4) | ((U8_T)(eTransType & R_ADMA_EN));
    CARD_REG(A_SDIO_MODE_REG(eIP)) = V_SDIO_MODE_INIT | gu16_SDIO_MODE_IntMode[eIP];

    CARD_REG_CLRBIT(A_BOOT_MOD_REG(eIP), R_BOOT_MODE);
    CARD_REG_CLRBIT(A_BOOT_REG(eIP), (R_NAND_BOOT_EN | R_BOOTSRAM_ACCESS_SEL | R_IMI_SEL));

    CARD_REG(A_MMA_PRI_REG_REG(eIP)) = V_MMA_PRI_INIT;
    CARD_REG(A_DDR_MOD_REG(eIP)) = V_DDR_MODE_INIT | (eTransType==EV_CIF ? gu16_DDR_MODE_REG_ForR2N[eIP] : gu16_DDR_MODE_REG[eIP]);

    #if (EN_BIND_PLL)
    if(_IS_PLLPAD(eIP))
    {
        CARD_REG(A_PLL_CLKPH_SKEW(eIP)) = gu16_PLL_CLKPH_SKEW[eIP];
        CARD_REG(A_PLL_EMMC_TEST(eIP)) = gu16_PLL_EMMC_TEST[eIP];
        CARD_REG(A_PLL_ECO_EN(eIP)) = gu16_PLL_ECO_EN[eIP];
        CARD_REG(A_PLL_EMMC_EN(eIP)) = gu16_PLL_EMMC_EN[eIP];
        CARD_REG(A_PLL_SKEW_SUM(eIP)) = gu16_PLL_SKEW_SUM[eIP];
        CARD_REG(A_PLL_IO_BUS_WID(eIP)) = (gu16_SD_MODE_DatLine[eIP] >> 1);
        CARD_REG(A_PLL_DQS_PAGE_NO(eIP)) = gu16_PLL_DQS_PAGE_NO[eIP];
        CARD_REG(A_PLL_DQS_SUM(eIP)) = gu16_PLL_DQS_SUM[eIP];
        CARD_REG(A_PLL_DQS_IO_MODE(eIP)) = gu16_PLL_DDR_IO_MODE[eIP];
        CARD_REG(A_PLL_AFIFO_SUM(eIP)) = gu16_PLL_AFIFO_SUM[eIP];
        CARD_REG(A_PLL_TX_BPS_EN(eIP)) = gu16_PLL_TX_BPS_EN[eIP];
        CARD_REG(A_PLL_RX_BPS_EN(eIP)) = V_PLL_RX_BPS_EN_INIT/* | gu16_PLL_RX_BPS_EN[eIP]*/;
        CARD_REG(A_PLL_ATOP_BYP_RX_EN(eIP)) = V_PLL_ATOP_BYP_RX_EN_INIT/* | gu16_PLL_ATOP_BYP_RX_EN[eIP]*/;
    }
    #endif  //End of  #if (EN_BIND_PLL)

    Hal_SD_TIMER_uDelay(gu16_WT_NRC[eIP]);


    if( _REG_ClearSDSTS(eIP, RT_CLEAN_SDSTS) || _REG_ClearMIEEvent(eIP, RT_CLEAN_MIEEVENT) )
        return _SDMMC_EndProcess(eIP, EV_STS_RIU_ERR, bCloseClk, __LINE__);

#if(EN_BIND_CARD_INT)
    Hal_CARD_INT_SetMIEIntEn(eIP, EV_INT_SD, V_MIE_INT_EN_INIT);
#endif

    if(eCmdType==EV_CMDREAD)
    {
        if (eTransType!=EV_CIF)
        {
            u16WaitMIEEvent |= R_DATA_END;
            u32WaitMS |= WT_EVENT_READ;
        }
        CARD_REG_SETBIT(A_SD_CTL_REG(eIP), (R_CMD_EN | R_DTRX_EN | R_JOB_START) );
    }
    else
        CARD_REG_SETBIT(A_SD_CTL_REG(eIP), (R_CMD_EN | R_JOB_START) );

    if(_REG_WaitEvent(eIP, EV_MIE, u16WaitMIEEvent, u32WaitMS))
        return _SDMMC_EndProcess(eIP, EV_STS_MIE_TOUT, bCloseClk, __LINE__);

    //====== Special Case for R2N CIFD Read Transfer ======
    if( (eCmdType==EV_CMDREAD) && (eTransType==EV_CIF) )
    {
        if(_BUF_CIFD_WaitEvent(eIP, eCmdType, gpu8Buf[eIP]))
           return _SDMMC_EndProcess(eIP, EV_STS_MIE_TOUT, bCloseClk, __LINE__);

        if(_REG_WaitEvent(eIP, EV_MIE, R_DATA_END, WT_EVENT_READ))
            return _SDMMC_EndProcess(eIP, EV_STS_MIE_TOUT, bCloseClk, __LINE__);
    }

    if( (eRspType == EV_R1B))
    {
        if( _REG_ClearMIEEvent(eIP, RT_CLEAN_MIEEVENT) )
            return _SDMMC_EndProcess(eIP, EV_STS_RIU_ERR, bCloseClk, __LINE__);
        if( _REG_WaitDat0HI(eIP, WT_DAT0HI_END) )
            return _SDMMC_EndProcess(eIP, EV_STS_DAT0_BUSY, bCloseClk, __LINE__);
    }
    else if( (eRspType == EV_R3) || (eRspType == EV_R4) )  // For IP CRC bug
        CARD_REG(A_SD_STS_REG(eIP)) = R_CMDRSP_CERR; //Clear CMD CRC Error

    if( (eCmdType == EV_CMDWRITE) && (!M_REG_STSERR(eIP)) )
    {
        if( _REG_ClearSDSTS(eIP, RT_CLEAN_SDSTS) || _REG_ClearMIEEvent(eIP, RT_CLEAN_MIEEVENT) )
            return _SDMMC_EndProcess(eIP, EV_STS_RIU_ERR, bCloseClk, __LINE__);

        CARD_REG(A_SD_CTL_REG(eIP)) = V_SD_CTL_INIT |(eCmdType>>4) | ((U8_T)(eTransType & R_ADMA_EN));
        CARD_REG_SETBIT(A_SD_CTL_REG(eIP), (R_DTRX_EN | R_JOB_START) );

        //====== Special Case for R2N CIFD Write Transfer ======
        if ( (eTransType==EV_CIF) && _BUF_CIFD_WaitEvent(eIP, eCmdType, gpu8Buf[eIP]) )
            return _SDMMC_EndProcess(eIP, EV_STS_MIE_TOUT, bCloseClk, __LINE__);

        if(_REG_WaitEvent(eIP, EV_MIE, R_DATA_END, WT_EVENT_WRITE))
            return _SDMMC_EndProcess(eIP, EV_STS_MIE_TOUT, bCloseClk, __LINE__);

    }

    return _SDMMC_EndProcess(eIP, (RspErrEmType)M_REG_STSERR(eIP), bCloseClk, __LINE__);

}


/*----------------------------------------------------------------------------------------------------------
*
* Function: Hal_SDMMC_RunBrokenDmaAndWaitProcess
*     @author jeremy.wang (2015/7/29)
* Desc: For Broken DMA Data Transfer
*
* @param eIP : FCIE1/FCIE2/...
* @param eCmdType : READ/WRITE
*
* @return RspErrEmType  : Response Error Code
----------------------------------------------------------------------------------------------------------*/
RspErrEmType Hal_SDMMC_RunBrokenDmaAndWaitProcess(IPEmType eIP, CmdEmType eCmdType)
{
    U32_T u32WaitMS = 0;

    if(eCmdType==EV_CMDREAD)
        u32WaitMS = WT_EVENT_READ;
    else if(eCmdType==EV_CMDWRITE)
        u32WaitMS = WT_EVENT_WRITE;

    if ( _REG_ClearMIEEvent(eIP, RT_CLEAN_MIEEVENT) )
        return _SDMMC_EndProcess(eIP, EV_STS_RIU_ERR, FALSE, __LINE__);

    CARD_REG_CLRBIT(A_SD_CTL_REG(eIP), R_CMD_EN );
    CARD_REG_SETBIT(A_SD_CTL_REG(eIP), (R_DTRX_EN | R_JOB_START) );

    if ( _REG_WaitEvent(eIP, EV_MIE, R_DATA_END, u32WaitMS) )
        return _SDMMC_EndProcess(eIP, EV_STS_MIE_TOUT, FALSE, __LINE__);

    return _SDMMC_EndProcess(eIP, (RspErrEmType)M_REG_STSERR(eIP), FALSE, __LINE__);

}


/*----------------------------------------------------------------------------------------------------------
*
* Function: Hal_SDMMC_ADMASetting
*     @author jeremy.wang (2015/7/13)
* Desc: For ADMA Data Transfer Settings
*
* @param pDMATable : DMA Table memory address pointer
* @param u8Item : DMA Table Item 0 ~
* @param u32SubLen :  DMA Table DMA Len
* @param u32SubBCnt : DMA Table Job Blk Cnt
* @param u32SubAddr : DMA Table DMA Addr
* @param u8MIUSel : MIU Select
* @param bEnd : End Flag
----------------------------------------------------------------------------------------------------------*/
void Hal_SDMMC_ADMASetting(volatile void *pDMATable, U8_T u8Item, U32_T u32SubLen, U16_T u16SubBCnt, U32_T u32SubAddr, U8_T u8MIUSel, BOOL_T bEnd)
{
    //U16_T u16Pos;

    AdmaDescStruct* pst_AdmaDescStruct = (AdmaDescStruct*) pDMATable;

    pst_AdmaDescStruct = (pst_AdmaDescStruct+u8Item);
    //_MEM_CLEAR((volatile U8_T *)pst_AdmaDescStruct, sizeof(AdmaDescStruct));
    memset(pst_AdmaDescStruct, 0, sizeof(AdmaDescStruct));

    pst_AdmaDescStruct->u32_DmaLen = u32SubLen;
    pst_AdmaDescStruct->u32_Address = u32SubAddr;
    pst_AdmaDescStruct->u32_JobCnt = u16SubBCnt;
    pst_AdmaDescStruct->u32_MiuSel = u8MIUSel;
    pst_AdmaDescStruct->u32_End    = bEnd;

    /*prtstring("\r\n");
    prtstring("gpst_AdmaDescStruct Pos=(");
    prtU32Hex((U32_T)pst_AdmaDescStruct);
    prtstring(")\r\n");

    for(u16Pos=0; u16Pos<192 ; u16Pos++)
    {
        if( (u16Pos%12)==0)
           (prtstring("\r\n"));

        (prtstring("["));
        (prtU8Hex( *(pu8Buf+u16Pos)));
        (prtstring("]"));

    }*/


}


//***********************************************************************************************************
//  SDMMC Special Operation Function
//***********************************************************************************************************

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDMMC_ClkCtrl
*     @author jeremy.wang (2017/12/27)
 * Desc: Enable IP clock output
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param bEnable : Enable (TRUE) or Disable (FALSE)
 * @param u16DelayMs : Delay ms to Specail Purpose
 ----------------------------------------------------------------------------------------------------------*/
void Hal_SDMMC_ClkCtrl(IPEmType eIP, BOOL_T bEnable, U16_T u16DelayMs)
{
    CARD_REG(A_MIE_FUNC_CTL_REG(eIP)) = V_MIE_PATH_INIT | _REG_GetMIEFunCtlSetting(eIP);

    if( bEnable )
        CARD_REG_SETBIT(A_SD_MODE_REG(eIP), R_CLK_EN);
    else
        CARD_REG_CLRBIT(A_SD_MODE_REG(eIP), R_CLK_EN);

    Hal_SD_TIMER_mSleep(u16DelayMs);
}


/*----------------------------------------------------------------------------------------------------------
*
* Function: Hal_SDMMC_Reset
*     @author jeremy.wang (2015/7/17)
* Desc:  Reset IP to avoid IP dead
*
* @param eIP : FCIE1/FCIE2/...
----------------------------------------------------------------------------------------------------------*/
void Hal_SDMMC_Reset(IPEmType eIP)
{
    _REG_ResetIP(eIP);
}


/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDMMC_StopProcessCtrl
 *     @author jeremy.wang (2017/9/15)
 * Desc: Stop process to avoid long time waiting
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param bEnable : Enable (TRUE) or Disable (FALSE)
 ----------------------------------------------------------------------------------------------------------*/
void Hal_SDMMC_StopProcessCtrl(IPEmType eIP, BOOL_T bEnable)
{
    gb_StopWProc[eIP] = bEnable;

#if(EN_BIND_CARD_INT)
    if ( gb_StopWProc[eIP] )
        Hal_CARD_INT_StopWaitMIEEventCtrl(eIP, TRUE);
    else
        Hal_CARD_INT_StopWaitMIEEventCtrl(eIP, FALSE);
#endif

}


/*----------------------------------------------------------------------------------------------------------
*
* Function: Hal_SDMMC_OtherPreUse
*     @author jeremy.wang (2015/7/17)
* Desc: Use this for avoid SD/EMMC to use FCIE at the same time
*
* @param eIP : FCIE1/FCIE2/...
*
* @return BOOL_T  :  (TRUE: Other driver usnig)
----------------------------------------------------------------------------------------------------------*/
BOOL_T Hal_SDMMC_OtherPreUse(IPEmType eIP)
{

    if ( CARD_REG(A_MIE_FUNC_CTL_REG(eIP)) & R_EMMC_EN )
        return (TRUE);

    if ( !(CARD_REG(A_MIE_FUNC_CTL_REG(eIP)) & (R_SD_EN | R_SDIO_MODE)) ) //Not SD Path
        return (TRUE);

    return (FALSE);
}


/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDMMC_ErrGroup
 *     @author jeremy.wang (2015/10/28)
 * Desc: transfer response error type to group error type
 *
 * @param eRspErrType : Response Error Type
 *
 * @return ErrGrpEmType  : Group Error Type
 ----------------------------------------------------------------------------------------------------------*/
ErrGrpEmType Hal_SDMMC_ErrGroup(RspErrEmType eErrType)
{

    switch((U16_T)eErrType)
    {
        case EV_STS_OK:
            return EV_EGRP_OK;

        case EV_STS_WR_TOUT:
        case EV_STS_NORSP:
        case EV_STS_RD_TOUT:
        case EV_STS_RIU_ERR:
        case EV_STS_DAT0_BUSY:
        case EV_STS_MIE_TOUT:
            return EV_EGRP_TOUT;

        case EV_STS_RD_CERR:
        case EV_STS_WD_CERR:
        case EV_STS_RSP_CERR:
        case (EV_STS_RSP_CERR|EV_STS_RD_CERR):
        case (EV_STS_NORSP|EV_STS_RD_CERR):
            return EV_EGRP_COMM;

        default:
            return EV_EGRP_OTHER;
    }

}


//***********************************************************************************************************
// SDMMC Information
//***********************************************************************************************************

/*----------------------------------------------------------------------------------------------------------
*
* Function: Hal_SDMMC_DumpMemTool
*     @author jeremy.wang (2015/7/29)
* Desc: Help us to dump memory
*
* @param u8ListNum : Each List Num include 16 bytes
* @param pu8Buf : Buffer Pointer
----------------------------------------------------------------------------------------------------------*/
void Hal_SDMMC_DumpMemTool(U8_T u8ListNum, volatile U8_T *pu8Buf)
{
    U16_T u16Pos=0;
    U8_T u8ListPos;
    U32_T u32BufAddr = (U32_T)pu8Buf;
    prtstring("\r\n $[Prt MEM_DATA: ");
    prtU32Hex(u32BufAddr);
    prtstring(" ]\r\n");

    for(u8ListPos=0 ; u8ListPos <u8ListNum; u8ListPos++ )
    {
        u16Pos= u8ListPos*16;
        for(; u16Pos< (u8ListPos+1)*16; u16Pos++)
        {
            prtstring("[");
            prtU8Hex(pu8Buf[u16Pos]);
            prtstring("]");
        }
        prtstring("\r\n");
    }

    prtstring("\r\n\r\n");

}


/*----------------------------------------------------------------------------------------------------------
*
* Function: Hal_SDMMC_GetDATBusLevel
*     @author jeremy.wang (2015/7/17)
* Desc: Monitor bus level for debug
*
* @param eIP : FCIE1/FCIE2/...
*
* @return U8_T  : Return DAT Bus Level (4Bits)
----------------------------------------------------------------------------------------------------------*/
U8_T Hal_SDMMC_GetDATBusLevel(IPEmType eIP)
{
    U16_T u16Temp = 0;

    u16Temp = CARD_REG(A_SD_STS_REG(eIP)) & 0x0F00;

    return (U8_T)(u16Temp>>8);

}


/*----------------------------------------------------------------------------------------------------------
*
* Function: Hal_SDMMC_GetMIEEvent
*     @author jeremy.wang (2015/7/31)
* Desc: Monitor MIE Event for debug
*
* @param eIP : FCIE1/FCIE2/...
*
* @return U16_T  : Return MIE Event
----------------------------------------------------------------------------------------------------------*/
U16_T Hal_SDMMC_GetMIEEvent(IPEmType eIP)
{
    return CARD_REG(A_MIE_EVENT_REG(eIP));
}


//***********************************************************************************************************
// SDMMC SDIO Setting
//***********************************************************************************************************

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDMMC_SDIODeviceCtrl
 *     @author jeremy.wang (2015/11/3)
 * Desc: Enable SDIO device for do something for SDIO case
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param bEnable : Enable (TRUE) or Disable (FALSE)
 ----------------------------------------------------------------------------------------------------------*/
void Hal_SDMMC_SDIODeviceCtrl(IPEmType eIP, BOOL_T bEnable)
{
    if(bEnable)
        gb_SDIODevice[eIP] = TRUE;
    else
        gb_SDIODevice[eIP] = FALSE;

}


/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDMMC_SDIOIntDetCtrl
 *     @author jeremy.wang (2015/11/3)
 * Desc: Enable SDIO interrupt detection
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param bEnable : Enable (TRUE) or Disable (FALSE)
 ----------------------------------------------------------------------------------------------------------*/
void Hal_SDMMC_SDIOIntDetCtrl(IPEmType eIP, BOOL_T bEnable)
{

    if(gb_SDIODevice[eIP])
    {
        if(bEnable)
        {
            //always open R_SDIO_INT_IEN
            if(!gu16_SDIO_IntEn_RunOnce[eIP])
            {
                gu16_SDIO_IntEn_RunOnce[eIP] = TRUE;  //If we confirm this device is SDIO, we enable the int_en once

                #if(EN_BIND_CARD_INT)
                //Hal_CARD_INT_SetMIEIntEn_ForSDIO(eIP, EV_INT_SD, bEnable);
                #endif // End of EN_BIND_CARD_INT

            }

            CARD_REG_SETBIT(A_SDIO_DET_REG(eIP), R_SDIO_DET_ON);
        }
        else
        {
            //Auto Clear at FCIE5 (No Need)
            //CARD_REG_CLRBIT(A_SDIO_DET_REG(eIP), R_SDIO_DET_ON);
        }


    }

}


/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDMMC_SetSDIOIntBeginSetting
 *     @author jeremy.wang (2015/11/3)
 * Desc: Set SDIO Int begin setting
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param u8Cmd : SDIO Command
 * @param u32Arg : SDIO Argument
 * @param eCmdType : CMDRSP/READ/WRITE
 * @param u16BlkCnt : Block Cnt
 ----------------------------------------------------------------------------------------------------------*/
void Hal_SDMMC_SetSDIOIntBeginSetting(IPEmType eIP, U8_T u8Cmd, U32_T u32Arg, CmdEmType eCmdType, U16_T u16BlkCnt)
{
    BOOL_T bSDIOAbortMode = (u8Cmd == 52) && ( (u32Arg & 0x83FFFE00) == 0x80000C00);

    gu16_SDIO_MODE_IntMode[eIP] = 0;

    if(gb_SDIODevice[eIP])
    {
        if(eCmdType != EV_CMDRSP)
        {
            if(u16BlkCnt == 1)
                gu16_SDIO_MODE_IntMode[eIP] = R_SDIO_INT_MOD1;
            else
                gu16_SDIO_MODE_IntMode[eIP] = R_SDIO_INT_MOD0 | R_SDIO_INT_MOD1;
        }
        else if ( (u8Cmd == 12) || bSDIOAbortMode)
        {
            gu16_SDIO_MODE_IntMode[eIP] = R_SDIO_INT_MOD0;
        }
    }

}


/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDMMC_SetSDIOIntEndSetting
 *     @author jeremy.wang (2015/10/29)
 * Desc:
 *
 * @param eIP :
 * @param eRspErr :
 * @param u16BlkCnt :
 ----------------------------------------------------------------------------------------------------------*/
void Hal_SDMMC_SetSDIOIntEndSetting(IPEmType eIP, RspErrEmType eRspErr, U16_T u16BlkCnt)
{
    U16_T u16RspErr = (U16_T)eRspErr;

    if(gb_SDIODevice[eIP])
    {
        if(u16BlkCnt>1)
        {

        }
        if(u16RspErr>0)
        {
        }
    }
}


//***********************************************************************************************************
// SDMMC SD3.0 Setting
//***********************************************************************************************************
#if(EN_BIND_PLL)
/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SD_SetPhase
 *     @author jeremy.wang (2018/1/12)
 * Desc: Set Phase
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param eSD30Bus :  SD30_DDR/SD30_SDR
 * @param u8Phase : Phase NO
 ----------------------------------------------------------------------------------------------------------*/
void Hal_SD_SetPhase(IPEmType eIP, SD30BusEmType eSD30Bus, U8_T u8Phase)
{

    if (eSD30Bus == EV_SD30_SDR)
    {
         if ( u8Phase < 9 )
             _REG_SetSDRSCanVal(eIP, u8Phase, 0);
         else
             _REG_SetSDRSCanVal(eIP, u8Phase-9, 1);
    }
    else if ( eSD30Bus == EV_SD30_DDR)
    {
        _REG_SetDDRScanVal(eIP, u8Phase+1, 0);
    }

}


/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SD_SavePassPhase
 *     @author jeremy.wang (2018/1/12)
 * Desc: Save pass phase
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param u8Phase : Phase NO
 * @param bCleanFlag : Clean all
 *
 * @return U8_T  :
 ----------------------------------------------------------------------------------------------------------*/
U8_T Hal_SD_SavePassPhase(IPEmType eIP, U8_T u8Phase, BOOL_T bCleanFlag)
{
    U8_T u8Pos = 0;

    if ( u8Phase >= V_MAX_PHASE)
        return 1; // Over V_MAX_PHASE

    //Clean All Phase Table
    if (bCleanFlag)
    {
        gu8_PASS_PH_CNT[eIP] = 0;
        for (u8Pos = 0 ; u8Pos < V_MAX_PHASE; u8Pos++)
            gu8Arr_PASS_PHS[eIP][u8Pos] = 0;
    }
    else
    {
        if ( gu8_PASS_PH_CNT[eIP] < V_MAX_PHASE)
        {
            gu8Arr_PASS_PHS[eIP][gu8_PASS_PH_CNT[eIP]] = u8Phase;
            gu8_PASS_PH_CNT[eIP] += 1;
        }
        else
            return 1; //Save too many phases (over V_MAX_PHASE)
    }

    return 0;

}


/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SD_FindFitPhaseSetting
 *     @author jeremy.wang (2018/1/12)
 * Desc: Find fit phase setting from pass phases
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param u8ScanMaxPhase : Max phase number in this scan case
 *
 * @return S8_T  : -1 (Error) or correct phase
 ----------------------------------------------------------------------------------------------------------*/
S8_T Hal_SD_FindFitPhaseSetting(IPEmType eIP, U8_T u8ScanMaxPhase)
{

    U8_T u8ArrPHSegs[V_MAX_PHASE][V_MAX_PHASE] = { {0}, {0} };
    U8_T u8ArrPHCntsPerSeg[V_MAX_PHASE] = {0};
    U8_T u8SegNum = 0, u8SegPos = 0, u8SelSegPos = 0, u8PH0RawSegPos = 0, u8PHSMaxRawSegPos = 0;
    U8_T u8PassPHPos = 0, u8PHPos = 0, u8PHPos_Inx = 0;
    U8_T u8PHCntsAtPH0RawSeg = 0, u8PHCntsAtPHSMaxRawSeg = 0, u8MaxPHCnts = 0;
    BOOL_T bPH_0_Found = FALSE, bPH_SMax_Found = FALSE;
    S8_T s8RetPhase = 0;

    if ( !gu8_PASS_PH_CNT[eIP] || (gu8_PASS_PH_CNT[eIP] > V_MAX_PHASE) )
        return -1; // No ref or over max phases design

    //Save Pass Phases to different Segements (Eash has continuous phase numbers)
    for (u8PassPHPos = 0; u8PassPHPos < gu8_PASS_PH_CNT[eIP]; u8PassPHPos++)
    {
        u8ArrPHSegs[u8SegNum][u8PHPos] =  gu8Arr_PASS_PHS[eIP][u8PassPHPos];
        u8ArrPHCntsPerSeg[u8SegNum] += 1;
        u8PHPos++;

        if ( (u8PassPHPos + 1) ==  gu8_PASS_PH_CNT[eIP] )
            break; //Last PassPHPos
        else if ( (gu8Arr_PASS_PHS[eIP][u8PassPHPos] + 1) != (gu8Arr_PASS_PHS[eIP][u8PassPHPos + 1]) )
        {
            //Change to Next Segement
            u8SegNum++;
            u8PHPos = 0;
        }
    }

    //Find Phase_0 at first Segement (Window)
    if (!u8ArrPHSegs[0][0]) {
        bPH_0_Found = TRUE;
        u8PH0RawSegPos = 0;

        //Find Phase_ScanMax at last Segement (Window)
        for (u8SegPos = 1; u8SegPos <= u8SegNum; u8SegPos++)
        {
            if (u8ArrPHCntsPerSeg[u8SegPos])
            {
                for (u8PHPos = 0; u8PHPos < u8ArrPHCntsPerSeg[u8SegPos]; u8PHPos++)
                {
                    if (u8ArrPHSegs[u8SegPos][u8PHPos] == u8ScanMaxPhase) {
                        bPH_SMax_Found = TRUE;
                        u8PHSMaxRawSegPos = u8SegPos;
                        break;
                    }
                }
            }
        }
    }

    //Merge first Segment and last Segement if they are cycle
    if (bPH_0_Found && bPH_SMax_Found) {

        u8PHCntsAtPH0RawSeg = u8ArrPHCntsPerSeg[u8PH0RawSegPos];
        u8PHCntsAtPHSMaxRawSeg = u8ArrPHCntsPerSeg[u8PHSMaxRawSegPos];

        if (u8PHCntsAtPH0RawSeg + u8PHCntsAtPHSMaxRawSeg >= V_MAX_PHASE)
            return -1;

        //Merge two Segments
        u8PHPos_Inx = u8PHCntsAtPHSMaxRawSeg;
        for (u8PHPos = 0; u8PHPos < u8PHCntsAtPH0RawSeg; u8PHPos++) {
            u8ArrPHSegs[u8PHSMaxRawSegPos][u8PHPos_Inx] =
                u8ArrPHSegs[u8PH0RawSegPos][u8PHPos];
            if (++u8PHPos_Inx >= V_MAX_PHASE)
                break;
        }

        u8ArrPHCntsPerSeg[u8PH0RawSegPos] = 0;
        u8ArrPHCntsPerSeg[u8PHSMaxRawSegPos] = u8PHCntsAtPH0RawSeg + u8PHCntsAtPHSMaxRawSeg;
    }


    #if 0
    prtstring("\r\n---------------------------[ Dump Segs Table  ] -----------------------------\r\n");

    for(u8SegPos=0; u8SegPos <= u8SegNum ; u8SegPos++)
    {

        if (u8ArrPHCntsPerSeg[u8SegPos])
        {
            for (u8PHPos = 0; u8PHPos < u8ArrPHCntsPerSeg[u8SegPos]; u8PHPos++)
            {
                prtU8(u8ArrPHSegs[u8SegPos][u8PHPos]);
                prtstring(", ");

            }
            prtstring("\r\n");
        }
    }
    prtstring(")\r\n");

    prtstring("\r\n-----------------------------------------------------------------------------\r\n");

    #endif

    //Find the segment that has max phase counts
    for (u8SegPos = 0; u8SegPos <= u8SegNum; u8SegPos++)
    {
        if (u8ArrPHCntsPerSeg[u8SegPos] > u8MaxPHCnts)
        {
            u8MaxPHCnts = u8ArrPHCntsPerSeg[u8SegPos];
            u8SelSegPos = u8SegPos;
        }
    }

    u8PHPos = (u8MaxPHCnts * 11) / 20;
    if (u8PHPos) u8PHPos--;

    s8RetPhase = (S8_T)u8ArrPHSegs[u8SelSegPos][u8PHPos];

    if ( s8RetPhase >= V_MAX_PHASE )
         return -1;

    return s8RetPhase;

}


/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SD_GoodPhases_Dump
 *     @author jeremy.wang (2018/2/7)
 * Desc: Dump Good Phases
 *
 * @param eIP : FCIE1/FCIE2/...
 ----------------------------------------------------------------------------------------------------------*/
void Hal_SD_GoodPhases_Dump(IPEmType eIP)
{
    U8_T u8PassPHPos = 0;

    prtstring("$$ PassPHs["); prtU8(gu8_PASS_PH_CNT[eIP]);
    prtstring("] = (");

    for(u8PassPHPos=0; u8PassPHPos< gu8_PASS_PH_CNT[eIP] ; u8PassPHPos++)
    {
        prtU8(gu8Arr_PASS_PHS[eIP][u8PassPHPos]);
        prtstring(", ");
    }

    prtstring(")\r\n");

}

#endif  //End of EN_BIND_PLL
//***********************************************************************************************************
// SDMMC Interrupt Setting
//***********************************************************************************************************

#if(EN_BIND_CARD_INT)

/*----------------------------------------------------------------------------------------------------------
 *
 * Function: Hal_SDMMC_MIEIntCtrl
 *     @author jeremy.wang (2015/11/2)
 * Desc: Enable MIE event interrupt control
 *
 * @param eIP : FCIE1/FCIE2/...
 * @param bEnable : Enable (TRUE) or Disable (FALSE)
 ----------------------------------------------------------------------------------------------------------*/
void Hal_SDMMC_MIEIntCtrl(IPEmType eIP, BOOL_T bEnable)
{
    Hal_CARD_INT_MIEIntCtrl(eIP, EV_INT_SD, bEnable);
}


#endif  // End of EN_BIND_CARD_INT
