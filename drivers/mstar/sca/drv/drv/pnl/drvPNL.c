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
/// file    drvPNL.c
/// @brief  Panel Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#define _DRVPNL_C
//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#ifdef MSOS_TYPE_LINUX_KERNEL
#include <linux/string.h>
#include <linux/delay.h>
#include <asm/div64.h>
#include "ms_platform.h"
#else
#include "string.h"
#define do_div(x,y) ((x)/=(y))
#endif


// Common Definition
#include "MsCommon.h"

#include "drvPNL.h"
#include "halPNL.h"

#include "pnl_hwreg_utility2.h"
// Internal Definition
#include "drvMMIO.h"

//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define UNUSED(x)       (x=x)
#define TCON_TAB_MAX_SIZE  2048

#define NONPM_BASE                  (0x100000)


//-------------------------------------------------------------------------------------------------
//  Local Structurs
//-------------------------------------------------------------------------------------------------
static const MS_U8       _cPnlVer[] = PNL_LIB_VERSION;
static const PNL_DrvInfo _cstPnl_DrvInfo = {GAMMA_TYPE, GAMMA_MAPPING};
static PNL_DrvStatus     _stPnl_DrvStatus = {FALSE, FALSE};
static MS_U16 _u16PnlDbgSwitch;

// Internal Data
//extern MS_U32 gu32PnlRiuBaseAddr ;
//extern MS_U32 gu32PMRiuBaseAddr;

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

//PNL_InitData _stPnlInitData;

MS_BOOL g_bDrvPnl_UseTConTable_Mod = FALSE;
//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
// Function in mdrv_xc_menuload.c
typedef enum
{
    E_MLOAD_UNSUPPORTED = 0,
    E_MLOAD_DISABLED    = 1,
    E_MLOAD_ENABLED     = 2,
}MLOAD_TYPE;

typedef enum
{
    E_MLG_UNSUPPORTED = 0,
    E_MLG_DISABLED    = 1,
    E_MLG_ENABLED     = 2,
}MLG_TYPE;

typedef enum
{
    MAIN_WINDOW=0,       ///< main window if with PIP or without PIP
    SUB_WINDOW=1,         ///< sub window if PIP

    SC1_MAIN_WINDOW=2,   ///< main window if with PIP or without PIP
    SC1_SUB_WINDOW,    ///< sub window if PIP
    SC2_MAIN_WINDOW,   ///< main window if with PIP or without PIP
    SC2_SUB_WINDOW,    ///< sub window if PIP

    MAX_WINDOW             /// The max support window
}SCALER_WIN;

extern MS_BOOL MApi_XC_MLoad_WriteCmds_And_Fire(MS_U32 *pu32Addr, MS_U16 *pu16Data, MS_U16 *pu16Mask, MS_U16 u16CmdCnt);
extern MS_BOOL MApi_XC_MLoad_WriteCmd_And_Fire(MS_U32 u32Addr, MS_U16 u16Data, MS_U16 u16Mask);
extern MLOAD_TYPE MApi_XC_MLoad_GetStatus(SCALER_WIN eWindow);
extern MLG_TYPE MApi_XC_MLG_GetStatus(void);
extern void MApi_XC_MLG_Fire(MS_U8 *pR, MS_U8 *pG, MS_U8 *pB, MS_U16 u16Count, MS_U16 *pMaxGammaValue);

//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------


static void _MDrv_PNL_Init_XC_VOP(PNL_InitData *pstPanelInitData)
{

    MS_U16 u16DE_HEND = (pstPanelInitData->u16HStart + pstPanelInitData->u16Width - 1);
    MS_U16 u16DE_HStart = pstPanelInitData->u16HStart;
    MS_U16 u16DE_VEND = (pstPanelInitData->u16VStart + pstPanelInitData->u16Height - 1);
    MS_U16 u16DE_VStart = pstPanelInitData->u16VStart;


    // Htotal/Vtotal
    SC_W2BYTEMSK(REG_SC_BK10_0C_L, (pstPanelInitData->u16HTotal - 1), 0xFFF);         // output htotal
    SC_W2BYTEMSK(REG_SC_BK10_0D_L, (pstPanelInitData->u16VTotal - 1), 0xFFF);         // output vtotal

    // DE H/V start/size
    SC_W2BYTEMSK(REG_SC_BK10_04_L, u16DE_HStart, VOP_DE_HSTART_MASK);                                       // DE H start
    SC_W2BYTEMSK(REG_SC_BK10_05_L, u16DE_HEND  , VOP_DE_HEND_MASK);  // DE H end
    SC_W2BYTEMSK(REG_SC_BK10_06_L, u16DE_VStart, 0xFFF);                                       // DE V start
    SC_W2BYTEMSK(REG_SC_BK10_07_L, u16DE_VEND  , 0xFFF);  // DE V end

    // Display H/V start/size
    SC_W2BYTEMSK(REG_SC_BK10_08_L, u16DE_HStart , VOP_DISPLAY_HSTART_MASK);                                               // Display H start
    SC_W2BYTEMSK(REG_SC_BK10_09_L, u16DE_HEND   , VOP_DISPLAY_HEND_MASK);  // Display H end
    SC_W2BYTEMSK(REG_SC_BK10_0A_L, u16DE_VStart , 0xFFF);                                               // Display V start
    SC_W2BYTEMSK(REG_SC_BK10_0B_L, u16DE_VEND   , 0xFFF);  // Display V end

    // H/V sync start/width
    SC_W2BYTEMSK(REG_SC_BK10_01_L, (pstPanelInitData->u8HSyncWidth-1), LBMASK);                    // hsync width
    SC_W2BYTEMSK(REG_SC_BK10_02_L, (pstPanelInitData->u16VTotal - pstPanelInitData->u16VSyncStart), 0xFFF);     // vsync start = Vtt - (VSyncWidth + BackPorch)
    SC_W2BYTEMSK(REG_SC_BK10_03_L, (pstPanelInitData->u16VTotal - pstPanelInitData->u16VSyncStart + pstPanelInitData->u8VSyncWidth), VOP_VSYNC_END_MASK);    // vsync end   = Vsync start + Vsync Width
    SC_W2BYTEMSK(REG_SC_BK10_10_L, (pstPanelInitData->bManuelVSyncCtrl << 15), BIT(15));                                  // manual vsync control
    SC_W2BYTEMSK(REG_SC_BK10_10_L, BIT(14), BIT(14));       //default set to mode1                                                                       // disable auto_htotal
    SC_W2BYTEMSK(REG_SC_BK10_23_L, BIT(7), BIT(7));

    // output control
    SC_W2BYTE(REG_SC_BK10_21_L, pstPanelInitData->u16OCTRL);

    if (pstPanelInitData->eLPLL_Type == E_PNL_TYPE_LVDS ||
        pstPanelInitData->eLPLL_Type == E_PNL_TYPE_DAC_I ||
        pstPanelInitData->eLPLL_Type == E_PNL_TYPE_DAC_P)
    {
        // LVDS
        SC_W2BYTEMSK(REG_SC_BK10_22_L, 0x11, LBMASK);  //   0x2f44 : Enable clock of internal control - LVDS or TTL Output
    }
    else
    {
        // RSDS
        SC_W2BYTEMSK(REG_SC_BK10_22_L, 0x00, LBMASK);
    }

    SC_W2BYTE(REG_SC_BK10_46_L, pstPanelInitData->u16OSTRL);

    // output driving current
    SC_W2BYTEMSK(REG_SC_BK10_47_L, pstPanelInitData->u16ODRV, LBMASK);

    // dither control
    SC_W2BYTE(REG_SC_BK10_1B_L, pstPanelInitData->u16DITHCTRL);

}

static void _MDrv_PNL_Init_LPLL(PNL_InitData *pstPanelInitData)
{
    MS_U64 ldHz = 1;
    MS_U16 u16DefaultVFreq = 0;

    if(pstPanelInitData->u16DefaultVFreq > 1100)
    {
        u16DefaultVFreq = 1200;
    }
    else if(pstPanelInitData->u16DefaultVFreq > 900)
    {
        u16DefaultVFreq = 1000;
    }
    else if(pstPanelInitData->u16DefaultVFreq > 550)
    {
        u16DefaultVFreq = 600;
    }
    else
    {
        u16DefaultVFreq = 500;
    }
    ldHz = (MS_U64) (((MS_U64)pstPanelInitData->u16HTotal) * pstPanelInitData->u16VTotal * u16DefaultVFreq);

    MHal_PNL_Init_LPLL(pstPanelInitData->eLPLL_Type,pstPanelInitData->eLPLL_Mode,ldHz);

    if (pstPanelInitData->u32PNL_MISC == E_DRVPNL_MISC_MFC_ENABLE)
    {
        MHal_PNL_FRC_lpll_src_sel(0);
    }
    else
    {
        MHal_PNL_FRC_lpll_src_sel(1);
    }

}

static MS_U64 _MDrv_PNL_DClkFactor(PNL_TYPE eLPLL_Type, MS_U32 ldHz)
{
    return (((MS_U64)LVDS_MPLL_CLOCK_MHZ * 524288 * MHal_PNL_Get_LPLL_LoopGain(eLPLL_Type, ldHz)));  //216*524288*8=0x36000000
}

static void _MDrv_PNL_Init_Output_Dclk(PNL_InitData *pstPanelInitData)
{
    MS_U32 u32OutputDclk;
    MS_U16 u16DefaultVFreq = 0;
    MS_U64 ldHz = 1;

    if(pstPanelInitData->u16DefaultVFreq > 1100)
    {
        u16DefaultVFreq = 1200;
    }
    else if(pstPanelInitData->u16DefaultVFreq > 900)
    {
        u16DefaultVFreq = 1000;
    }
    else if(pstPanelInitData->u16DefaultVFreq > 550)
    {
        u16DefaultVFreq = 600;
    }
    else
    {
        u16DefaultVFreq = 500;
    }

    ldHz = (MS_U64) (((MS_U64)pstPanelInitData->u16HTotal) * pstPanelInitData->u16VTotal * u16DefaultVFreq);

    u32OutputDclk = MDrv_PNL_CalculateLPLLSETbyDClk((MS_U32)ldHz);

    //printf("\n_MDrv_PNL_Init_Output_Dclk(): Dclk = %lx\n", u32OutputDclk);
    //printf("_MDrv_PNL_Init_Output_Dclk u32div = %lu, u32LpllSet = %lx\n", (MS_U32)u32div, (MS_U32)ldPllSet);

    MHal_PNL_Switch_LPLL_SubBank(0x00);
    W2BYTEMSK(L_BK_LPLL(0x0C), FALSE, BIT(3));
    W4BYTE(L_BK_LPLL(0x0F), u32OutputDclk);

#if SUPPORT_FRC
    Mhal_PNL_Flock_LPLLSet(ldHz);
#endif
}


/*static void _MDrv_PNL_TCON_GetTabSize(MS_U8 *pTconTab, MS_U16 u16tabtype, MS_U32 *pu32TconTabsize)
{
    MS_U8 u8Colslen = 0;
    MS_U16 u16timeout = 0x3FFF;
    MS_U16 u16Addr;
    if (pTconTab == NULL)
    {
        PNL_ASSERT(0, "[%s] TCONTable error \n.", __FUNCTION__);
        return;
    }
    switch(u16tabtype )
    {
    case TCON_TABTYPE_GENERAL:
    case TCON_TABTYPE_GPIO:
    case TCON_TABTYPE_SCALER:
    case TCON_TABTYPE_MOD:
    case TCON_TABTYPE_GAMMA:
        u8Colslen = 4;
        break;
    case TCON_TABTYPE_POWER_SEQUENCE_ON:
    case TCON_TABTYPE_POWER_SEQUENCE_OFF:
        u8Colslen = 7;
        break;
    default:
        PNL_ASSERT(0, "[%s] GetTable Size :unknown Tab Size \n.", __FUNCTION__);
        return ;
    }
    while (--u16timeout)
    {
        u16Addr = (pTconTab[*pu32TconTabsize]<<8) + pTconTab[(*pu32TconTabsize +1)];
        if (u16Addr == REG_TABLE_END) // check end of table
        {
            *pu32TconTabsize = *pu32TconTabsize + 4;
            break;
        }
        *pu32TconTabsize = *pu32TconTabsize + u8Colslen;
    }
    if (u16timeout==0)
    {
        PNL_ASSERT(0, "[%s] fails: timeout \n.", __FUNCTION__);
    }

    PNL_DBG(PNL_DBGLEVEL_INIT, "<<*pu32TconTabsize= %ld>>\n", *pu32TconTabsize);

    if(*pu32TconTabsize > TCON_TAB_MAX_SIZE)
    {
        PNL_ASSERT(0, "[%s] Tab size too large than TCON_TAB_MAX_SIZE = %d \n.", __FUNCTION__, TCON_TAB_MAX_SIZE);
    }
}*/

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
const MS_U8 * MDrv_PNL_GetLibVer(void)              ///< Get version
{
    return _cPnlVer;
}

const PNL_DrvInfo * MDrv_PNL_GetInfo(void)          ///< Get info from driver
{
    return &_cstPnl_DrvInfo;
}

MS_BOOL MDrv_PNL_GetStatus(PNL_DrvStatus *pDrvStatus)    ///< Get panel current status
{
    pDrvStatus->bPanel_Initialized = _stPnl_DrvStatus.bPanel_Initialized;
    pDrvStatus->bPanel_Enabled = _stPnl_DrvStatus.bPanel_Enabled;
    return TRUE;
}

MS_BOOL MDrv_PNL_SetDbgLevel(MS_U16 u16DbgSwitch)       ///< Set debug level
{
    _u16PnlDbgSwitch = u16DbgSwitch;
    UNUSED(_u16PnlDbgSwitch);

    MHal_PNL_SetDbgLevel(u16DbgSwitch);

    return TRUE;
}

MS_BOOL MDrv_PNL_Control_Out_Swing(MS_U16 u16Swing_Level)
{
    return (MS_BOOL)MHal_PNL_MOD_Control_Out_Swing(u16Swing_Level);
}

PNL_Result MDrv_PNL_PreInit(PNL_OUTPUT_MODE eParam)
{
    if ( eParam == E_PNL_OUTPUT_MAX )
        return E_PNL_FAIL;
    MHal_PNL_PreInit(eParam);

    return E_PNL_OK;
}

// Output Dclk
MS_U32 MDrv_PNL_CalculateLPLLSETbyDClk(MS_U32 ldHz)
{

    MS_U64 ldPllSet = 0;
    MS_U32 u32div = 0;
    switch(_stPnlInitData.eLPLL_Type)
    {

        case E_PNL_TYPE_MINILVDS:
        case E_PNL_TYPE_ANALOG_MINILVDS:
        case E_PNL_TYPE_DIGITAL_MINILVDS:
        case E_PNL_LPLL_MINILVDS_5P_2L:
        case E_PNL_LPLL_MINILVDS_4P_2L:
        case E_PNL_LPLL_MINILVDS_3P_2L:
        case E_PNL_LPLL_MINILVDS_6P_1L:
        case E_PNL_LPLL_MINILVDS_5P_1L:
        case E_PNL_LPLL_MINILVDS_4P_1L:
        case E_PNL_LPLL_MINILVDS_3P_1L:

        case E_PNL_LPLL_MINILVDS_1CH_3P_6BIT:
        case E_PNL_LPLL_MINILVDS_1CH_4P_6BIT:
        case E_PNL_LPLL_MINILVDS_1CH_5P_6BIT:
        case E_PNL_LPLL_MINILVDS_1CH_6P_6BIT:

        case E_PNL_LPLL_MINILVDS_2CH_3P_6BIT:
        case E_PNL_LPLL_MINILVDS_2CH_4P_6BIT:
        case E_PNL_LPLL_MINILVDS_2CH_5P_6BIT:
        case E_PNL_LPLL_MINILVDS_2CH_6P_6BIT:

        case E_PNL_LPLL_MINILVDS_1CH_3P_8BIT:
        case E_PNL_LPLL_MINILVDS_1CH_4P_8BIT:
        case E_PNL_LPLL_MINILVDS_1CH_5P_8BIT:
        case E_PNL_LPLL_MINILVDS_1CH_6P_8BIT:

        case E_PNL_LPLL_MINILVDS_2CH_3P_8BIT:
        case E_PNL_LPLL_MINILVDS_2CH_4P_8BIT:
        case E_PNL_LPLL_MINILVDS_2CH_5P_8BIT:
        case E_PNL_LPLL_MINILVDS_2CH_6P_8BIT:

        case E_PNL_TYPE_TTL_TCON:
        case E_PNL_TYPE_TTL:
        case E_PNL_TYPE_LVDS:
        case E_PNL_TYPE_RSDS:
        case E_PNL_TYPE_MFC:
        case E_PNL_TYPE_DAC_I:
        case E_PNL_TYPE_DAC_P:
        case E_PNL_TYPE_PDPLVDS:

        default:
            //the first " *2 " is from  the dual mode
            u32div=(MS_U32)(MHal_PNL_Get_Loop_DIV(_stPnlInitData.eLPLL_Mode, _stPnlInitData.eLPLL_Type, ldHz));
            ldPllSet = (_MDrv_PNL_DClkFactor(_stPnlInitData.eLPLL_Type, ldHz) * 10000000 *2) + ((ldHz * u32div) >> 1);
            do_div(ldPllSet, ldHz);
            do_div(ldPllSet, u32div);

            break;
    }

    //printf("CalculateLPLLSETbyDClk u32KHz = %lu, u32LpllSet = %lx\n", (MS_U32)ldHz, (MS_U32)ldPllSet);
    return (MS_U32)ldPllSet;

}

PNL_Result MDrv_PNL_Init(void)
{
    MS_U32 u32NonPMBankSize, u32PMBankSize;

    // get MMIO base
    if(MDrv_MMIO_GetBASE( &gu32PnlRiuBaseAddr, &u32NonPMBankSize, MS_MODULE_PNL ) != TRUE)
    {
        PNL_ASSERT(0, "%s %lx %lx\n", "Get base address failed", gu32PnlRiuBaseAddr, u32NonPMBankSize);
        return E_PNL_GET_BASEADDR_FAIL;
    }
    else
    {
        printf("MDrv_PNL_Init u32PnlRiuBaseAddr = %lx \n", gu32PnlRiuBaseAddr);
    }

    if(MDrv_MMIO_GetBASE( &gu32PMRiuBaseAddr, &u32PMBankSize, MS_MODULE_PM ) != TRUE)
    {
        PNL_ASSERT(0, "%s %lx %lx\n", "Get PM base address failed", gu32PMRiuBaseAddr, u32PMBankSize);
        return E_PNL_GET_BASEADDR_FAIL;
    }
    else
    {
        printf("MDrv_PNL_Init u32PMRiuBaseAddr = %lx \n", gu32PMRiuBaseAddr);
    }

    _stPnl_DrvStatus.bPanel_Initialized = TRUE;

    MHal_PQ_Clock_Gen_For_Gamma();

    return E_PNL_OK;
}

PNL_Result MDrv_PNL_SetPanelType(PNL_InitData *pstPanelInitData)
{
    MHal_PNL_Init_XC_Clk(&_stPnlInitData);
    _MDrv_PNL_Init_XC_VOP(&_stPnlInitData);
    MHal_PNL_Init_MOD(&_stPnlInitData);
    _MDrv_PNL_Init_LPLL(&_stPnlInitData);
    _MDrv_PNL_Init_Output_Dclk(&_stPnlInitData);

    return E_PNL_OK;
}

PNL_Result MDrv_PNL_Close(void)
{
    return E_PNL_OK;
}

static void _MDrv_VOP_SetGammaMappingMode(MS_U8 u8Mapping)
{
    MHal_VOP_SetGammaMappingMode(u8Mapping);
}

/// set gamma table
PNL_Result MDrv_PNL_SetGammaTbl( DRVPNL_GAMMA_TYPE eGammaType, MS_U8* pu8GammaTab[3],
                                 DRVPNL_GAMMA_MAPPEING_MODE GammaMapMode )
{
    MS_U8 u8TgtChannel = 0;
    MS_U8* pcTab1;
    MS_U8 u8BackupMainSubEnableFlag = 0;

    UNUSED(eGammaType);

    // delay 2ms to wait LPLL stable (if not stable, it will make gamma value incorrect)
#ifdef MSOS_TYPE_LINUX_KERNEL
    mdelay(2);
#else
    MsOS_DelayTask(2);
#endif


    // If gamma is on, turn if off
    if(SC_R2BYTEMSK(REG_SC_BK10_50_L, BIT(0)))
    {
        if(MApi_XC_MLoad_GetStatus(MAIN_WINDOW) == E_MLOAD_ENABLED)
        {
            MS_U32 u32Addr[2];
            MS_U16 u16Data[2], u16Mask[2];
            // Disable Gamma of MainWindow
            u32Addr[0] = 0x10A0;
            u16Data[0] = 0x0000;
            u16Mask[0] = 0x0001;
            // Disable Gamma of SubWindow
            u32Addr[1] = 0x0F30;
            u16Data[1] = 0x0000;
            u16Mask[1] = 0x0001;
            MApi_XC_MLoad_WriteCmds_And_Fire(u32Addr ,u16Data ,u16Mask , 2);
        }
        else
        {
            SC_W2BYTEMSK(REG_SC_BK10_50_L, FALSE, BIT(0));
            SC_W2BYTEMSK(REG_SC_BK0F_18_L, FALSE, BIT(0));
        }

        u8BackupMainSubEnableFlag |= BIT(0);
    }

    _MDrv_VOP_SetGammaMappingMode( (GammaMapMode == E_DRVPNL_GAMMA_8BIT_MAPPING) ? GAMMA_8BIT_MAPPING : GAMMA_10BIT_MAPPING);

    if  (MApi_XC_MLG_GetStatus() == E_MLG_ENABLED)
    {
        MS_U8 u8Channel;
        MS_U16 pMaxGammaValue[3] ;
        MS_U16 u16NumOfLevel = ((GammaMapMode == E_DRVPNL_GAMMA_8BIT_MAPPING) ? 256 : 1024);
        pMaxGammaValue[0] = pMaxGammaValue[1] = pMaxGammaValue[2] = 0;
        MApi_XC_MLG_Fire(pu8GammaTab[0], pu8GammaTab[1], pu8GammaTab[2], u16NumOfLevel, pMaxGammaValue);
        // write max. value of gamma
        for(u8Channel = 0; u8Channel < 3; u8Channel ++)
        {
            hal_PNL_SetMaxGammaValue(u8Channel, pMaxGammaValue[u8Channel]);
        }
    }
    else
    {

        while( u8TgtChannel < 3 )   // 0 for R, 1 for G, 2 for B
        {
            pcTab1 = pu8GammaTab[u8TgtChannel];
            Hal_PNL_Set12BitGammaPerChannel(u8TgtChannel, pcTab1, GammaMapMode);
            u8TgtChannel++;
        }
    }

    // Recovery gamma table
    if( u8BackupMainSubEnableFlag & BIT(0) )
    {

        if(MApi_XC_MLoad_GetStatus(MAIN_WINDOW) == E_MLOAD_ENABLED)
        {
            MS_U32 u32Addr[2];
            MS_U16 u16Data[2], u16Mask[2];

            // Enable Gamma of MainWindow
            u32Addr[0] = 0x10A0;
            u16Data[0] = 0x0001;
            u16Mask[0] = 0x0001;
            // Enable Gamma of SubWindow
            u32Addr[1] = 0x0F30;
            u16Data[1] = 0x0001;
            u16Mask[1] = 0x0001;
            MApi_XC_MLoad_WriteCmds_And_Fire(u32Addr ,u16Data ,u16Mask , 2);
        }
        else
        {

            SC_W2BYTEMSK(REG_SC_BK10_50_L, BIT(0), BIT(0));
            SC_W2BYTEMSK(REG_SC_BK0F_18_L, BIT(0), BIT(0));
        }
    }

    return E_PNL_OK;
}

PNL_Result MDrv_PNL_SetGammaValue(MS_U8 u8Channel, MS_U16 u16Offset, MS_U16 u16GammaValue)
{
    MS_U16 u16MaxGammaValue;
    MS_U8 u8BackupMainSubEnableFlag = 0;
    MS_U16 u16MaxOffset = 0;
    MS_BOOL bUsingBurstWrite = FALSE;

    // delay 2ms to wait LPLL stable (if not stable, it will make gamma value incorrect)
#ifdef MSOS_TYPE_LINUX_KERNEL
    mdelay(2);
#else
    MsOS_DelayTask(2);
#endif

    if (u8Channel > 2)
    {
        return E_PNL_FAIL;
    }

    if (Hal_VOP_Is_GammaMappingMode_enable())
    {
        u16MaxOffset = 1024;
        // Check write mode
        bUsingBurstWrite = !Hal_VOP_Is_GammaSupportSignalWrite(E_DRVPNL_GAMMA_10BIT_MAPPING);
    }
    else
    {
        u16MaxOffset = 256;
        bUsingBurstWrite = !Hal_VOP_Is_GammaSupportSignalWrite(E_DRVPNL_GAMMA_8BIT_MAPPING);
    }

    if (u16Offset >= u16MaxOffset)
    {
        return E_PNL_FAIL;
    }

    // If gamma is on, turn if off
    if(SC_R2BYTEMSK(REG_SC_BK10_50_L, BIT(0)))
    {
        SC_W2BYTEMSK(REG_SC_BK10_50_L, FALSE, BIT(0));
        u8BackupMainSubEnableFlag |= BIT(0);
    }

    hal_PNL_WriteGamma12Bit(u8Channel,bUsingBurstWrite, u16Offset, u16GammaValue);

    u16MaxGammaValue = SC_R2BYTEMSK((REG_SC_BK10_7A_L + 2 * u8Channel), 0xFFF);
    if (u16GammaValue > u16MaxGammaValue)
    {
        u16MaxGammaValue = u16GammaValue;
    }

    SC_W2BYTEMSK((REG_SC_BK10_7A_L + 4 * u8Channel), u16MaxGammaValue, 0xFFF);           // max. base 0
    SC_W2BYTEMSK((REG_SC_BK10_7B_L + 4 * u8Channel), u16MaxGammaValue, 0xFFF);           // max. base 1

    // Recovery gamma table
    if( u8BackupMainSubEnableFlag & BIT(0) )
    {
        SC_W2BYTEMSK(REG_SC_BK10_50_L, BIT(0), BIT(0));
    }


    return E_PNL_OK;
}

PNL_Result MDrv_PNL_En(MS_BOOL bPanelOn, MS_BOOL bCalEn)
{
    _stPnl_DrvStatus.bPanel_Enabled = bPanelOn;
    return MHal_PNL_En(bPanelOn, bCalEn);
}

//-----------------------------------------------------------------------------
// SSC
//-----------------------------------------------------------------------------
// SPAN value, recommend value is 20KHz ~ 40KHz
// STEP percent value, recommend is under 3%

// recommend value.
// u16Periodx100Hz == 350, u16Percentx100 == 200
PNL_Result MDrv_PNL_SetSSC(MS_U16 u16Fmodulation, MS_U16 u16Rdeviation, MS_BOOL bEnable)
{
    MS_U16 u16Span;
    MS_U16 u16Step;
    MS_U32 u32PLL_SET;/// = MDrv_Read3Byte(L_BK_LPLL(0x0F));

    MHal_PNL_Switch_LPLL_SubBank(0x00);
    u32PLL_SET = R4BYTE(L_BK_LPLL(0x0F));
    // Set SPAN
    if(u16Fmodulation < 200 || u16Fmodulation > 400)
        u16Fmodulation = 300;
    u16Span =(MS_U16)( ( (((MS_U32)LVDS_MPLL_CLOCK_MHZ*LVDS_SPAN_FACTOR ) / (u16Fmodulation) ) * 10000) / ((MS_U32)u32PLL_SET) ) ;

    // Set STEP
    if(u16Rdeviation > 300)
        u16Rdeviation = 300;
    u16Step = (MS_U16)(((MS_U32)u32PLL_SET*u16Rdeviation) / ((MS_U32)u16Span*10000));

    W2BYTE(L_BK_LPLL(0x17), u16Step & 0x03FF);// LPLL_STEP
    W2BYTE(L_BK_LPLL(0x18), u16Span & 0x3FFF);// LPLL_SPAN
    W2BYTEMSK((L_BK_LPLL(0x0D)), (bEnable << 11), BIT(11)); // Enable ssc


    return E_PNL_OK;
}

void MDrv_PNL_SetOutputPattern(MS_BOOL bEnable, MS_U16 u16Red , MS_U16 u16Green, MS_U16 u16Blue)
{

    //printf("MDrv_PNL_SetOutputPattern: bEnable = %u, R/G/B=0x%x, 0x%x, 0x%x\n", bEnable, u16Red , u16Green, u16Blue);
    MHal_PNL_SetOutputPattern(bEnable, u16Red , u16Green, u16Blue);
}

void MDrv_PNL_SetOutputType(PNL_OUTPUT_MODE eOutputMode, PNL_TYPE eLPLL_Type)
{
    MHal_PNL_SetOutputType(MHal_PNL_Get_Output_MODE(), eLPLL_Type);
}

#define REG_TABLE_END   0xFFFF
void MDrv_PNL_TCON_DumpSCRegTab(MS_U8* pu8TconTab)
{
    MS_U32 u32tabIdx = 0;
    MS_U16 u16timeout = 0x3FFF;
    MS_U32 u32Addr;
    MS_U16 u16Mask;
    MS_U16 u16Value;
    MS_BOOL bHiByte;

    //printf("MDrv_PNL_TCON_DumpSCRegTab\n");

    PNL_DBG(PNL_DBGLEVEL_INIT, "tab: sc %s\n", __FUNCTION__);

    if (pu8TconTab == NULL)
{
        PNL_ASSERT(0, "[%s] TCONTable error \n.", __FUNCTION__);
        return;
    }

    while (--u16timeout)
    {
        u32Addr = ((pu8TconTab[u32tabIdx]<<8) + pu8TconTab[(u32tabIdx +1)]) & 0xFFFF;
        u16Mask  = pu8TconTab[(u32tabIdx +2)] & 0xFF;
        u16Value = pu8TconTab[(u32tabIdx +3)] & 0xFF;

        if (u32Addr == REG_TABLE_END) // check end of table
        break;


        bHiByte = (u32Addr & 0x8000) ? TRUE : FALSE;
        u32Addr = ((u32Addr & 0x7F00) | ((u32Addr & 0xFF) << 1));

        //PNL_DBG(PNL_DBGLEVEL_INIT, "[addr=%04lx, msk=%02x, val=%02x] \n", u32Addr, u16Mask, u16Value);
        //PNL_DBG(PNL_DBGLEVEL_INIT, "[addr=%04lx, msk=%02x, val=%02x] \n", BK_SC((u32Addr & 0x00FF))|bHiByte, u16Mask, u16Value);

        if (bHiByte)
        {
            SC_W2BYTEMSK(u32Addr, (u16Value << 8), (u16Mask << 8));
        }
        else
        {
            SC_W2BYTEMSK(u32Addr, u16Value, u16Mask);
        }
        u32tabIdx = u32tabIdx + 4;
    }
    if (u16timeout==0)
    {
        PNL_ASSERT(0, "[%s] fails: timeout \n.", __FUNCTION__);
    }
    //printf("MDrv_PNL_TCON_DumpSCRegTab end\n");
}

void MDrv_PNL_TCON_DumpMODRegTab(MS_U8* pu8TconTab)
{
    MS_U32 u32tabIdx = 0;
    MS_U16 u16timeout = 0x3FFF;
    MS_U32 u32Addr;
    MS_U16 u16Mask;
    MS_U16 u16Value;
    MS_BOOL bHiByte;

    //printf("MDrv_PNL_TCON_DumpMODRegTab\n");

    PNL_DBG(PNL_DBGLEVEL_INIT, "tab: mod  %s\n", __FUNCTION__);

    if (pu8TconTab == NULL)
    {
        PNL_ASSERT(0, "[%s] TCONTable error \n.", __FUNCTION__);
        return;
    }

    g_bDrvPnl_UseTConTable_Mod = TRUE;

    while (--u16timeout)
    {
        u32Addr = ((pu8TconTab[u32tabIdx]<<8) + pu8TconTab[(u32tabIdx +1)]) & 0xFFFF;
        u16Mask  = pu8TconTab[(u32tabIdx +2)] & 0xFF;
        u16Value = pu8TconTab[(u32tabIdx +3)] & 0xFF;

        if (u32Addr == REG_TABLE_END) // check end of table
        break;

        //printf("[addr=%04lx, msk=%02x, val=%02x] \n", u32Addr, u16Mask, u16Value);

        bHiByte = (u32Addr & 0x8000) ? TRUE : FALSE;
        u32Addr = ((u32Addr & 0x7F00) | ((u32Addr & 0xFF) << 1));

        PNL_DBG(PNL_DBGLEVEL_INIT, "[addr=%04lx, msk=%02x, val=%02x] \n", u32Addr, u16Mask, u16Value);
        //PNL_DBG(PNL_DBGLEVEL_INIT, "[addr=%04lx, msk=%02x, val=%02x] \n", BK_MOD((u32Addr & 0x00FF))|bHiByte, u16Mask, u16Value);
        //printf("->[addr=%04lx, msk=%02x, val=%02x] \n", u32Addr, u16Mask, u16Value);

        if (bHiByte)
        {
            MOD_W2BYTEMSK(u32Addr, (u16Value << 8), (u16Mask << 8));
        }
        else
        {
            MOD_W2BYTEMSK(u32Addr, u16Value, u16Mask);
        }
        u32tabIdx = u32tabIdx + 4;
    }

    if (u16timeout==0)
    {
        PNL_ASSERT(0, "[%s] fails: timeout \n.", __FUNCTION__);
    }

    //printf("MDrv_PNL_TCON_DumpMODRegTab END: REG_MOD_BK00_6E_L=%X\n", MOD_R2BYTE(REG_MOD_BK00_6E_L));
}

void MDrv_PNL_TCON_DumpGENRegTab(MS_U8* pu8TconTab)
{
    MS_U32 u32tabIdx = 0;
    MS_U16 u16timeout = 0x3FFF;
    MS_U32 u32Addr;
    MS_U16 u16Mask;
    MS_U16 u16Value;
    MS_U8 u8TconSubBank;

    printf("MDrv_PNL_TCON_DumpGENRegTab\n");

    PNL_DBG(PNL_DBGLEVEL_INIT, "tab: gen  %s\n", __FUNCTION__);

    if (pu8TconTab == NULL)
    {
        PNL_ASSERT(0, "[%s] TCONTable error \n.", __FUNCTION__);
        return;
}

    u8TconSubBank = R2BYTE(0x103000)&0xFF;
    //printf("u8TconSubBank=%x\n", u8TconSubBank);

    while (--u16timeout)
{
        u32Addr = ((pu8TconTab[u32tabIdx]<<8) + pu8TconTab[(u32tabIdx +1)]) & 0xFFFF;
        u16Mask  = pu8TconTab[(u32tabIdx +2)] & 0xFF;
        u16Value = pu8TconTab[(u32tabIdx +3)] & 0xFF;

        if (u32Addr == REG_TABLE_END) // check end of table
            break;

        u32Addr = (u32Addr | 0x100000);

        PNL_DBG(PNL_DBGLEVEL_INIT, "[addr=%04lx, msk=%02x, val=%02x] \n", u32Addr, u16Mask, u16Value);

        if (u32Addr & 0x1)
    {
            u32Addr --;
            W2BYTEMSK(u32Addr, (u16Value << 8), (u16Mask << 8));
    }
    else
    {
            W2BYTEMSK(u32Addr, u16Value, u16Mask);
        }
        u32tabIdx = u32tabIdx + 4;
    }

    W2BYTEMSK(0x103000, u8TconSubBank, 0xFF);

    W2BYTE(0x1030C2, 0x3FC0);

    if (u16timeout==0)
    {
        PNL_ASSERT(0, "[%s] fails: timeout \n.", __FUNCTION__);
    }
    //printf("MDrv_PNL_TCON_DumpGENRegTab end\n");
}
#if 0 // mode to APP
static void _MDrv_PNL_TCON_DumpPSRegTab(MS_U8 u8TconTab[TCON_TAB_MAX_SIZE], MS_U8 u8Tcontype, MS_U8 *pu8delay)
{
    MS_U32 u32tabIdx = 0;
    MS_U16 u16timeout = 0x3FFF;
    MS_U32 u32Addr;
    MS_U16 u16Mask;
    MS_U16 u16Value;
    MS_U8  u8signal_type = 0;

    PNL_DBG(PNL_DBGLEVEL_INIT, "tab: power sequence \n");

    if (u8TconTab == NULL)
    {
        PNL_ASSERT(0, "[%s] TCONTable error \n.", __FUNCTION__);
        return;
    }

    while (--u16timeout)
    {
        u32Addr = ((u8TconTab[u32tabIdx]<<8) + u8TconTab[(u32tabIdx +1)]) & 0xFFFF;
        u16Mask  = u8TconTab[(u32tabIdx +2)] & 0xFF;
        u16Value = u8TconTab[(u32tabIdx +3)] & 0xFF;
        u8signal_type = u8TconTab[(u32tabIdx +6)];

        if (u32Addr == REG_TABLE_END) // check end of table
            break;

        u32Addr = (u32Addr | 0x100000);

        if(u8Tcontype == u8signal_type)
        {
            PNL_DBG(PNL_DBGLEVEL_INIT, "[addr=%04lx, msk=%02x, val=%02x] \n", u32Addr, u16Mask, u16Value);

            if (u32Addr & 0x1)
    {
                u32Addr --;
                W2BYTEMSK(u32Addr, (u16Value << 8), (u16Mask << 8));
    }
    else
    {
                W2BYTEMSK(u32Addr, u16Value, u16Mask);
    }
            *pu8delay = u8TconTab[(u32tabIdx +5)];
}
        u32tabIdx = u32tabIdx + 7;
    }

    if (u16timeout==0)
    {
        PNL_ASSERT(0, "[%s] fails: timeout \n.", __FUNCTION__);
    }
}
#endif


void MDrv_PNL_TCON_Count_Reset( MS_BOOL bEnable )
{
    W2BYTEMSK(L_BK_TCON(0x03), (bEnable << 14), BIT(14));
}

void MDrv_PNL_TCON_Init(void)
{
    MHal_PNL_TCON_Init();
}


MS_BOOL MDrv_PNL_GetDataFromRegister(PNL_InitData *PNL_Data)
{

    if(NULL == PNL_Data)
    {
        return FALSE;
    }

    // Htotal/Vtotal
    PNL_Data->u16HTotal  = SC_R2BYTEMSK(REG_SC_BK10_0C_L, 0xFFF);         // output htotal
    PNL_Data->u16VTotal  = SC_R2BYTEMSK(REG_SC_BK10_0D_L, 0xFFF);         // output vtotal

    // DE H/V start/size
    PNL_Data->u16HStart  = SC_R2BYTEMSK(REG_SC_BK10_04_L, VOP_DE_HSTART_MASK);                                       // DE H start
    PNL_Data->u16Width   = SC_R2BYTEMSK(REG_SC_BK10_05_L, VOP_DE_HEND_MASK) - PNL_Data->u16HStart;        // DE H end
    PNL_Data->u16VStart  = SC_R2BYTEMSK(REG_SC_BK10_06_L, 0xFFF);                                       // DE V start
    PNL_Data->u16Height  = SC_R2BYTEMSK(REG_SC_BK10_07_L, 0xFFF) - PNL_Data->u16VStart;       // DE V end
    PNL_Data->eLPLL_Type = MHal_PNL_GetDACOut()? E_PNL_TYPE_DAC_I:E_PNL_TYPE_LVDS;

    //printf("----MDrv_PNL_GetDataFromRegister().DAC flag------------%x,%x,%x\n", PM_R1BYTE(L_BK_DAC(REG_DAC_HD_CTRL),  0:0),
    //                                        PM_R1BYTE(L_BK_DAC(REG_DAC_SD_CTRL),  2:0),
    //                                        PM_R1BYTE(H_BK_HDMITX(REG_VE_CONFIG_01),  0:0));

    return TRUE;

}

void MDrv_PNL_SetSSC_En(MS_BOOL bEnable)
{
    //printf("bEnable = %d\n", bEnable);
    MHal_PNL_Switch_LPLL_SubBank(0x00);
    W2BYTEMSK((L_BK_LPLL(0x0D)), (bEnable << 11), BIT(11)); // Enable ssc
}

void MDrv_PNL_SetSSC_Fmodulation(MS_U16 u16Fmodulation)
{

#if 1
     MS_U16 u16Span;
    //MS_U16 u16Step;
    MS_U32 u32PLL_SET;/// = MDrv_Read3Byte(L_BK_LPLL(0x0F));

    MHal_PNL_Switch_LPLL_SubBank(0x00);
    u32PLL_SET = R4BYTE(L_BK_LPLL(0x0F));
    //printf("u16Fmodulation = %d\n", u16Fmodulation );
    u16Span =(MS_U16)(( ( (((MS_U32)LVDS_MPLL_CLOCK_MHZ*LVDS_SPAN_FACTOR ) / (u16Fmodulation) ) * 10000) + ((MS_U32)u32PLL_SET/2)) / ((MS_U32)u32PLL_SET))  ;
    //printf("u16Span = 0x%x\n", u16Span);
    W2BYTE(L_BK_LPLL(0x18), u16Span & 0x3FFF);// LPLL_SPAN

#endif

}



void MDrv_PNL_SetSSC_Rdeviation(MS_U16 u16Rdeviation)
{
#if 1
    MS_U16 u16Span;
    MS_U16 u16Step;
    MS_U32 u32PLL_SET;

    MHal_PNL_Switch_LPLL_SubBank(0x00);
    u32PLL_SET = R4BYTE(L_BK_LPLL(0x0F));
    u16Span =R2BYTE(L_BK_LPLL(0x18)) ;
    //printf("u16Rdeviation = %d\n", u16Rdeviation);
    u16Step = (MS_U16)((((MS_U32)u32PLL_SET*u16Rdeviation) + ((MS_U32)u16Span*5000)) / ((MS_U32)u16Span*10000));
    //printf("u16Step = 0x%x\n", u16Step);
    W2BYTE(L_BK_LPLL(0x17), u16Step & 0x03FF);// LPLL_STEP
#endif

}
void MDrv_Mod_Calibration_Init(MS_U8 U8MOD_CALI_TARGET, MS_S8 S8MOD_CALI_OFFSET)
{
    MHal_MOD_Calibration_Init( U8MOD_CALI_TARGET, S8MOD_CALI_OFFSET);
}

void MDrv_BD_LVDS_Output_Type(MS_U16 Type)
{
    MHal_BD_LVDS_Output_Type(Type);
}

MS_BOOL MDrv_PNL_SkipTimingChange_GetCaps(void)
{
    return Hal_PNL_SkipTimingChange_GetCaps();
}

void MDrv_PNL_HWLVDSReservedtoLRFlag(PNL_DrvHW_LVDSResInfo lvdsresinfo)
{
    MHal_PNL_HWLVDSReservedtoLRFlag(lvdsresinfo);
}

void MDrv_PNL_OverDriver_Init(MS_U32 u32OD_MSB_Addr, MS_U32 u32OD_LSB_Addr, MS_U8 u8ODTbl[1056])
{
    MS_U32 u32OD_MSB_limit, u32OD_LSB_limit;
    u32OD_MSB_Addr = (u32OD_MSB_Addr / BYTE_PER_WORD) & 0xFFFFFFFF;
    u32OD_MSB_limit = u32OD_MSB_Addr + (((1920 *1080 *15)/(8 * BYTE_PER_WORD)) + (4096/BYTE_PER_WORD) + 20 + 16024);
    u32OD_LSB_Addr = (u32OD_LSB_Addr / BYTE_PER_WORD) & 0xFFFFFFFF;
    u32OD_LSB_limit = u32OD_LSB_Addr + (((1920 *1080 *6)/(8 * BYTE_PER_WORD)) + 20 + 80);
    if (SUPPORT_OVERDRIVE)
    {
        MHal_PNL_OverDriver_Init(u32OD_MSB_Addr, u32OD_MSB_limit, u32OD_LSB_Addr, u32OD_LSB_limit);
        MHal_PNL_OverDriver_TBL(u8ODTbl);
    }
}

void MDrv_PNL_OverDriver_Enable(MS_BOOL bEnable)
{
    if (SUPPORT_OVERDRIVE)
    {
        MHal_PNL_OverDriver_Enable(bEnable);
    }
}

MS_BOOL MDrv_PNL_Is_SupportFRC(void)
{
    return SUPPORT_FRC;
}

void MDrv_PNL_Dump(void)
{ // for compile warning MOD_R2BYTE & MOD_R2BYTEMSK;
    MS_U16 K;
    K =   MOD_R2BYTE(REG_MOD_BK00_40_L);
    K +=  MOD_R2BYTEMSK(REG_MOD_BK00_40_L, LBMASK);
}

void MDrv_PNL_Reset_Param(PNL_InitData *pstPanelInitData)
{
    MS_U16 u16DE_HEND = (pstPanelInitData->u16HStart + pstPanelInitData->u16Width - 1);
    MS_U16 u16DE_HStart = pstPanelInitData->u16HStart;
    MS_U16 u16DE_VEND = (pstPanelInitData->u16VStart + pstPanelInitData->u16Height - 1);
    MS_U16 u16DE_VStart = pstPanelInitData->u16VStart;
    MS_U32 u32OutputDclk;
    MS_U64 ldHz = 1;

    _stPnlInitData.u16HStart = pstPanelInitData->u16HStart;
    _stPnlInitData.u16VStart = pstPanelInitData->u16VStart;
    _stPnlInitData.u16Width  = pstPanelInitData->u16Width;
    _stPnlInitData.u16Height = pstPanelInitData->u16Height;
    _stPnlInitData.u16HTotal = pstPanelInitData->u16HTotal;
    _stPnlInitData.u16VTotal = pstPanelInitData->u16VTotal;

    _stPnlInitData.u16DefaultVFreq = pstPanelInitData->u16DefaultVFreq;

    // sync
    _stPnlInitData.u8HSyncWidth     = pstPanelInitData->u8HSyncWidth;
    _stPnlInitData.u16VSyncStart    = pstPanelInitData->u16VSyncStart;
    _stPnlInitData.u8VSyncWidth     = pstPanelInitData->u8VSyncWidth;
    _stPnlInitData.bManuelVSyncCtrl = pstPanelInitData->bManuelVSyncCtrl;

    // Htotal/Vtotal
    SC_W2BYTEMSK(REG_SC_BK10_0C_L, (pstPanelInitData->u16HTotal - 1), 0xFFF);         // output htotal
    SC_W2BYTEMSK(REG_SC_BK10_0D_L, (pstPanelInitData->u16VTotal - 1), 0xFFF);         // output vtotal

    // DE H/V start/size
    SC_W2BYTEMSK(REG_SC_BK10_04_L, u16DE_HStart, VOP_DE_HSTART_MASK);                                       // DE H start
    SC_W2BYTEMSK(REG_SC_BK10_05_L, u16DE_HEND  , VOP_DE_HEND_MASK);  // DE H end
    SC_W2BYTEMSK(REG_SC_BK10_06_L, u16DE_VStart, 0xFFF);                                       // DE V start
    SC_W2BYTEMSK(REG_SC_BK10_07_L, u16DE_VEND  , 0xFFF);  // DE V end

    // Display H/V start/size
    SC_W2BYTEMSK(REG_SC_BK10_08_L, u16DE_HStart , VOP_DISPLAY_HSTART_MASK);                                               // Display H start
    SC_W2BYTEMSK(REG_SC_BK10_09_L, u16DE_HEND   , VOP_DISPLAY_HEND_MASK);  // Display H end
    SC_W2BYTEMSK(REG_SC_BK10_0A_L, u16DE_VStart , 0xFFF);                                               // Display V start
    SC_W2BYTEMSK(REG_SC_BK10_0B_L, u16DE_VEND   , 0xFFF);  // Display V end

    // H/V sync start/width
    SC_W2BYTEMSK(REG_SC_BK10_01_L, (pstPanelInitData->u8HSyncWidth-1), LBMASK);                    // hsync width
    SC_W2BYTEMSK(REG_SC_BK10_02_L, (pstPanelInitData->u16VTotal - pstPanelInitData->u16VSyncStart), 0xFFF);     // vsync start = Vtt - (VSyncWidth + BackPorch)
    SC_W2BYTEMSK(REG_SC_BK10_03_L, (pstPanelInitData->u16VTotal - pstPanelInitData->u16VSyncStart + pstPanelInitData->u8VSyncWidth), VOP_VSYNC_END_MASK);    // vsync end   = Vsync start + Vsync Width
    SC_W2BYTEMSK(REG_SC_BK10_10_L, (pstPanelInitData->bManuelVSyncCtrl << 15), BIT(15));                                  // manual vsync control


    ldHz = (MS_U64) (((MS_U64)pstPanelInitData->u16HTotal) * pstPanelInitData->u16VTotal * pstPanelInitData->u16DefaultVFreq);

    u32OutputDclk = MDrv_PNL_CalculateLPLLSETbyDClk((MS_U32)ldHz);

    //printf("\n_MDrv_PNL_Init_Output_Dclk(): Dclk = %lx\n", u32OutputDclk);
    //printf("_MDrv_PNL_Init_Output_Dclk u32div = %lu, u32LpllSet = %lx\n", (MS_U32)u32div, (MS_U32)ldPllSet);

    MHal_PNL_Switch_LPLL_SubBank(0x00);
    W2BYTEMSK(L_BK_LPLL(0x0C), FALSE, BIT(3));
    W4BYTE(L_BK_LPLL(0x0F), u32OutputDclk);

    _MDrv_PNL_Init_Output_Dclk(&_stPnlInitData);
}


