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
/// file    Mdrv_sc_display.c
/// @brief  Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#ifdef MSOS_TYPE_LINUX_KERNEL
#include <asm/div64.h>
#else
#define do_div(x,y) ((x)/=(y))
#endif

// Common Definition
#ifdef MSOS_TYPE_LINUX_KERNEL
#include <linux/string.h>
#include <linux/wait.h>
#include <linux/irqreturn.h>
#else
#include <string.h>
#endif
#include "MsCommon.h"
#include "MsIRQ.h"
#include "MsOS.h"
#include "mhal_xc_chip_config.h"

// Internal Definition
#include "drvXC_IOPort.h"
#include "xc_Analog_Reg.h"
#include "xc_hwreg_utility2.h"
#include "apiXC.h"
#include "mvideo_context.h"
#include "drv_sc_ip.h"
#include "drv_sc_scaling.h"
#include "drv_sc_display.h"
#include "drv_sc_isr.h"
#include "mhal_sc.h"
#include "drv_sc_menuload.h"
#include "mdrv_sc_3d.h"

#include "mhal_frc.h"
#include "mdrv_frc.h"

#include "mhal_dip.h"
#ifdef CHIP_C3
#include "mhal_tgen.h"
#endif
//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

#define VERIFY_DISPLAY_FPGA 0
#define PHASE_OFFSET_LIMIT  0x8000UL

//-----------------------------------------------------------------------------
//  below definitions are for FSM FPLL
//-----------------------------------------------------------------------------
typedef enum
{
    FPLL_IDLE_STATE,
    FPLL_INIT_STATE,
    FPLL_ATV_INIT_STATE,
    FPLL_WAIT_PRD_LOCK_STATE,
    FPLL_WAIT_PHASE_STABLE_STATE,
    FPLL_CHECK_FRAME_LOCK_STATE,
    FPLL_LOCKED_STATE,
    FPLL_VD_LOCKED_STATE,
    FPLL_WAIT_LPLL_OFS_STATE,
    FPLL_WAIT_IP_STABLE_STATE,
    FPLL_DEATTACH_ISR_STATE,
} EN_FPLL_FSM;

typedef enum
{
    EN_OUTPUT_TIMING_CHG_DCLK   = 0,
    EN_OUTPUT_TIMING_HTOTAL = 1,
    EN_OUTPUT_TIMING_VTOTAL = 2,
} EN_OUTPUT_TIMING_ADJUST_MODE;

EN_FPLL_FSM _enFpllFsm = FPLL_IDLE_STATE; //PDP_FRAMELOCK

#define FPLL_FSM_STABLE_TIME    (10000)         // if exceed this stable time, the FPLL FSM will re-init
#define FPLL_PRD_MAX            (0xFFFF)        // Max PRD value
#define FPLL_PHASE_VAR_LIMIT    (0x10)          // If phase error variation exceed this, means un-stable
#define FPLL_PRD_THRESHOLD      (0x100)         // The PRD must be smaller than this value to indicate almost finished PRD lock
#define FPLL_PRD_VAR_LIMIT      (0x10)          // If PRD variation exceed this, means un-stable
#define FPLL_PHASE_MAX          (0xFFFF)        // Max PHASE value
#define FPLL_PRD_STABLE_COUNT   0x06

#define GATE_PHASELIMIT         0x3000

// FPLL
#define IVS_OVS_MAX_DIFF    0x1000UL    // before calculate 2nd SET, will check if IVS/OVS diff too big to verify the LPLL SET accuracy
#define IVS_VAR_LIMIT       0x10        // before calculate 2nd SET, the IVS should be very stable, if the variable exceed this, it's un-stable and not cal. 2nd SET
#define VD_HSYNC_LOCKED                 BIT(14)

//Timing
#define VFREQ_DIFF_TOLERANCE         10        //Value to distinguish two Vfreq(x10Hz)
///< number of panel_info_ex supported by current XC
#define PANEL_INFO_EX_NUM            2
//-------------------------------------------------------------------------------------------------
//  Local Structurs
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------
static MS_BOOL _bFSM_FPLL_Working = FALSE;
static MS_BOOL _bFSM_FPLL_Setting = FALSE;


//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
static MS_BOOL  _bFpllCusFlag[E_FPLL_FLAG_MAX];
static MS_U32   _u32FpllCusData[E_FPLL_FLAG_MAX];

static MS_BOOL _bForceFreerun = FALSE;

static E_VFREQ_SEL _ForceVFreq = VFREQ_FROM_PANEL;
static MS_U16 _u16DefaultVSyncWidth, _u16DefaultVSyncBackPorch, _u16DefaultHSyncWidth, _u16DefaultHSyncBackPorch;
static XC_PANEL_INFO_EX _stPanelInfoEx[PANEL_INFO_EX_NUM];
static MS_BOOL _bHSyncChanged = FALSE, _bVSyncChanged = FALSE;
//Variable used for set MCNR in FPLL

static MS_BOOL _bNeedReloadMCNR = FALSE, _bEnableMCNR = FALSE , _bForceDisableMCNR = FALSE;
static MS_U32 _u32MCNRTimer = 0;

static MS_BOOL bFPLL_FSM = FALSE;
static MS_U8 _u8CoastClearDebounce=0;
static MS_BOOL _bDisableCoastDebounce=0;
static MS_BOOL _bUseCustomFRCTable  = FALSE;
static MS_BOOL u32OutputPhaseStable = FALSE;
static MS_BOOL u32OutputPRDLock     = FALSE;

//FRC control
static SC_FRC_SETTING sFRC_Tbl[5];
static SC_FRC_SETTING sFRC_Tbl_Default[5] =
{
    {0, 160, 1, 4, MS_FRC_1_4},
    {160, 245, 2, 5, MS_FRC_2_5},  // 24 to 48 {160, 245, 1, 2, MS_FRC_1_2}
    {245, 260, 1, 2, MS_FRC_1_2},
    {260, 400, 1, 2, MS_FRC_1_2},
    {400, 0xFFFF, 1, 1, MS_FRC_1_1},
};

static MS_U16 _u16OutputFreq[MAX_WINDOW];
static XC_DIP_CONFIG_t g_stDipConfig;
//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
MS_U16 MDrv_SC_FBL_Calc_Vtt(XC_SetTiming_Info *pTimingInfo, SCALER_WIN eWindow);
MS_U16 MDrv_SC_VFreq_Select(MS_U16 u16AutoVFreq);

// Output Dclk and Loop DIV : Ref from PNL driver. It is easy to maintain if XC and PNL use same function.
MS_U32  MDrv_PNL_CalculateLPLLSETbyDClk(MS_U32 ldHz);
MS_U8   MHal_PNL_Get_Loop_DIV( MS_U8 u8LPLL_Mode, MS_U8 eLPLL_Type, MS_U32 ldHz);
MS_U16  MHal_PNL_Get_LPLL_LoopGain(MS_U8 eLPLL_Type, MS_U32 ldHz);

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// screen
//-----------------------------------------------------------------------------
#ifdef MULTI_SCALER_SUPPORTED
void MDrv_SC_Set_BLSK(MS_BOOL bEnable, SCALER_WIN eWindow)
{
    if(MApi_XC_MLoad_GetStatus(eWindow) == E_MLOAD_ENABLED)
        Hal_SC_set_blsk_burst(bEnable, eWindow);
    else
        Hal_SC_set_blsk(bEnable, eWindow);
}

void MDrv_SC_GenerateBlackVideoForBothWin(MS_BOOL bEnable, SCALER_WIN eWindow)
{
    if ( MApi_XC_MLoad_GetStatus(eWindow) == E_MLOAD_ENABLED )
        Hal_SC_set_main_sub_black_screen_burst(bEnable, eWindow);
    else
        Hal_SC_set_main_sub_black_screen(bEnable, eWindow);
}

#else	// #ifdef MULTI_SCALER_SUPPORTED

void MDrv_SC_Set_BLSK(MS_BOOL bEnable)
{
    if(MApi_XC_MLoad_GetStatus(MAIN_WINDOW) == E_MLOAD_ENABLED)
        Hal_SC_set_blsk_burst(bEnable);
    else
        Hal_SC_set_blsk(bEnable);
}

void MDrv_SC_GenerateBlackVideoForBothWin(MS_BOOL bEnable)
{
    if ( MApi_XC_MLoad_GetStatus(MAIN_WINDOW) == E_MLOAD_ENABLED )
        Hal_SC_set_main_sub_black_screen_burst(bEnable);
    else
        Hal_SC_set_main_sub_black_screen(bEnable);
}

#endif

/******************************************************************************/
///enable/diable output black pattern
///@param bEnable \b IN
///-  False:Disable
///-  TRUE :enable
/******************************************************************************/
void MDrv_SC_GenerateBlackVideo(MS_BOOL bEnable, SCALER_WIN eWindow)
{
    //MS_U8 u8Bank;

    //u8Bank = MDrv_ReadByte(BK_SELECT_00);
	#ifdef MULTI_SCALER_SUPPORTED

    if(eWindow == MAIN_WINDOW || eWindow == SC1_MAIN_WINDOW || eWindow == SC2_MAIN_WINDOW)
    {
        if(MApi_XC_MLoad_GetStatus(eWindow) == E_MLOAD_ENABLED)
            Hal_SC_set_main_black_screen_burst(bEnable, eWindow);
        else
            Hal_SC_set_main_black_screen(bEnable, eWindow);
    }
    else
    {
        if(MApi_XC_MLoad_GetStatus(eWindow) == E_MLOAD_ENABLED)
            Hal_SC_set_sub_blue_screen_burst(bEnable, DISABLE, eWindow);
        else
            Hal_SC_set_sub_blue_screen(bEnable, DISABLE, eWindow);
    }

	#else

    if(eWindow == MAIN_WINDOW)
    {
        if(MApi_XC_MLoad_GetStatus(eWindow) == E_MLOAD_ENABLED)
            Hal_SC_set_main_black_screen_burst(bEnable);
        else
            Hal_SC_set_main_black_screen(bEnable);
    }
    else
    {
        if(MApi_XC_MLoad_GetStatus(eWindow) == E_MLOAD_ENABLED)
            Hal_SC_set_sub_blue_screen_burst(bEnable, DISABLE);
        else
            Hal_SC_set_sub_blue_screen(bEnable, DISABLE);
    }
	#endif

    #if VERIFY_DISPLAY_FPGA

    //u8Bank = MDrv_ReadByte(BK_SELECT_00);
    //----- enable IP signal for FPLL ---//
    //MDrv_WriteByte(BK_SELECT_00, REG_BANK_IP1F2);
    //MDrv_WriteRegBit(H_BK_IP1F2(0x03), 1, BIT(7));
    //MDrv_WriteRegBit(H_BK_IP1F2(0x03), 1, BIT(3));
    //MDrv_WriteByte(BK_SELECT_00, u8Bank);

    SC_W2BYTEMSK(REG_SC_BK01_03_L, 0x8000, BIT(15));
    SC_W2BYTEMSK(REG_SC_BK01_03_L, 0x0800, BIT(11));

    //--- set LVDS format ----//
    MDrv_WriteByte(L_BK_MOD(0x40), 0x2C);       // set LVDS format
    MDrv_WriteByte(L_BK_MOD(0x4B), 0x00);       // set LVDS format
    MDrv_WriteByte(L_BK_MOD(0x78), 0x7E);       // set LVDS driving
    #endif
    //--- restore bank select ---//

    //MDrv_WriteByte(BK_SELECT_00, u8Bank);

}

//-------------------------------------------------------------------------------------------------
/// set display window color
/// @param  u8Color                 \b IN: the color to set
/// @param  eWindow                 \b IN: which window we are going to set
//-------------------------------------------------------------------------------------------------
void MApi_XC_SetDispWindowColor(MS_U8 u8Color, SCALER_WIN eWindow)
{
    _XC_ENTRY();
    Hal_SC_set_nosignal_color(u8Color,eWindow);
    _XC_RETURN();
}

//get the greatest common divisor
MS_U16 MDrv_SC_GCD(MS_U16 m, MS_U16 n)
{
    MS_U16 m_cup, n_cup, res;

    m_cup = m;
    n_cup = n;
    res = m_cup % n_cup;
    while (res != 0)
    {
        m_cup = n_cup;
        n_cup = res;
        res = m_cup % n_cup;
    }

    return n_cup;
}

void MDrv_SC_Handle_Special_FRCTbl(MS_U16 u16DefaultVFreq, MS_U16 u16InputVFreq, SCALER_WIN eWindow)
{
    MS_U8 u8Ratio = 0;
    MS_U16 u16FRCType=0;
    MS_BOOL bIsOutput48hz = MDrv_SC_3D_IsOutputVfreq48hz(eWindow, u16InputVFreq);
    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "@@@ bIsOutput48hz = %s\r\n",bIsOutput48hz?"TRUE":"FALSE")
    memcpy(sFRC_Tbl, sFRC_Tbl_Default, sizeof(sFRC_Tbl_Default));

    //when the panel is 120hz default vfreq, the FRC table should be double the ratio.
    //eg. input:30, original x2 out:60, but for 120hz panel, we should x4 out:120.
    //    the multiple is from x2 to x4
    if(u16DefaultVFreq > 900)
    {
        u8Ratio = 2;
        XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "FRC table FRC_OUT x2 for 120 hz panel output\r\n")
    }
    else
    {
        u8Ratio = 1;
    }

    for(u16FRCType = 0; u16FRCType < sizeof(sFRC_Tbl)/sizeof(SC_FRC_SETTING); u16FRCType++)
    {
        MS_U16 u16GCD = 0;
        // 24Hz -> 48Hz
        if (bIsOutput48hz &&(u16DefaultVFreq == 600))
        {
            // check FRC, and modify FRC table.
            if((u16InputVFreq > sFRC_Tbl[u16FRCType].u16LowerBound) &&
            (u16InputVFreq <= sFRC_Tbl[u16FRCType].u16HigherBound))
            {
                sFRC_Tbl[u16FRCType].u8FRC_In = 1;
                sFRC_Tbl[u16FRCType].u8FRC_Out = 2;
                sFRC_Tbl[u16FRCType].eFRC_Type= MS_FRC_1_2;
            }
        }

        sFRC_Tbl[u16FRCType].u8FRC_Out *= u8Ratio;

        u16GCD = MDrv_SC_GCD(sFRC_Tbl[u16FRCType].u8FRC_Out, sFRC_Tbl[u16FRCType].u8FRC_In);
        sFRC_Tbl[u16FRCType].u8FRC_Out /= u16GCD;
        sFRC_Tbl[u16FRCType].u8FRC_In  /= u16GCD;
    }

}
//-----------------------------------------------------------------------------
// frame rate convert
//-----------------------------------------------------------------------------

E_APIXC_ReturnValue MApi_XC_CustomizeFRCTable(SC_FRC_SETTING* stFRCTable)
{
    if (stFRCTable == NULL )
    {
        SC_FRC_SETTING sFRC_Tbl_Temp[5] =
        {
            {0, 160, 1, 4, MS_FRC_1_4},
            {160, 245, 2, 5, MS_FRC_2_5},
            {245, 260, 1, 2, MS_FRC_1_2},
            {260, 400, 1, 2, MS_FRC_1_2},
            {400, 0xFFFF, 1, 1, MS_FRC_1_1},
        };
        _bUseCustomFRCTable = FALSE;
        memcpy( sFRC_Tbl_Default, sFRC_Tbl_Temp , sizeof(sFRC_Tbl_Default) );
    }
    else
    {
        _bUseCustomFRCTable = TRUE;
        memcpy( sFRC_Tbl_Default, stFRCTable , sizeof(sFRC_Tbl_Default) );
    }

    return E_APIXC_RET_OK;
}

MS_U16 MDrv_SC_Cal_FRC_Output_Vfreq(MS_U16 u16InputVFreq,
                             MS_U16 u16DefaultVFreq,
                             MS_BOOL bFBL,
                             MS_U8 *u8FrcIn,
                             MS_U8 *u8FrcOut,
                             MS_BOOL *bSetFrameLock,
                             MS_U8 u8Lpll_bank,
                             SCALER_WIN eWindow)
{
    MS_U16 u16OutputVFreq = u16InputVFreq;
    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING,"u16DefaultVFreq = %d, u16InputVFreq =%d\r\n",
                                        u16DefaultVFreq, u16InputVFreq);

    MDrv_SC_Handle_Special_FRCTbl(u16DefaultVFreq, u16InputVFreq, eWindow);

    if (!bFBL)
    {
        MS_U16 u16FRCType=0;
        for(u16FRCType = 0; u16FRCType < sizeof(sFRC_Tbl)/sizeof(SC_FRC_SETTING); u16FRCType++)
        {
            XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING,
                "InputV = %d, Tbl[%d] = (%d, %d)\r\n", u16InputVFreq, u16FRCType,
                sFRC_Tbl[u16FRCType].u16LowerBound, sFRC_Tbl[u16FRCType].u16HigherBound) ;

            // check FRC
            if((u16InputVFreq > sFRC_Tbl[u16FRCType].u16LowerBound) &&
            (u16InputVFreq <= sFRC_Tbl[u16FRCType].u16HigherBound))
            {
                break;
            }
        }

        MS_ASSERT(u16FRCType <= sizeof(sFRC_Tbl)/sizeof(SC_FRC_SETTING));

        // Set output V
        if(u16FRCType < sizeof(sFRC_Tbl)/sizeof(SC_FRC_SETTING))
        {
            u16OutputVFreq = u16InputVFreq * sFRC_Tbl[u16FRCType].u8FRC_Out
                                          / sFRC_Tbl[u16FRCType].u8FRC_In;

            *u8FrcIn = sFRC_Tbl[u16FRCType].u8FRC_In;
            *u8FrcOut = sFRC_Tbl[u16FRCType].u8FRC_Out;
        }
        else
        {
            u16OutputVFreq = 0;
            *u8FrcIn = 1;
            *u8FrcOut = 1;
        }
    }
    else
    {
        *u8FrcIn = 1;
        *u8FrcOut = 1;
    }

    LPLL_BK_STORE;
    if(u8Lpll_bank)
      LPLL_BK_SWITCH(0x01);
    else
      LPLL_BK_SWITCH(0x00);

    // 60->120 or 50->100
    if(u16DefaultVFreq == 1200)
    {
        if(((u16InputVFreq >= 1180) && (u16InputVFreq <= 1220))||
           ((u16InputVFreq >= 980) && (u16InputVFreq <= 1020)))
        {
            *u8FrcIn = 1;
            *u8FrcOut = 2;
        }
    }

    if((g_XC_Pnl_Misc.FRCInfo.bFRC)&&(gSrcInfo[MAIN_WINDOW].stCapWin.height == DOUBLEHD_720X2P_VSIZE)&&
    (MApi_XC_Get_3D_Output_Mode() == E_XC_3D_OUTPUT_FRAME_ALTERNATIVE))
    {
        *u8FrcIn = 1;
        *u8FrcOut = 2;
    }
#ifdef FA_1920X540_OUTPUT
    else
    if((!g_XC_Pnl_Misc.FRCInfo.bFRC)&&
      ((MApi_XC_Get_3D_Input_Mode(MAIN_WINDOW) == E_XC_3D_INPUT_SIDE_BY_SIDE_HALF)||
       (MApi_XC_Get_3D_Input_Mode(MAIN_WINDOW) == E_XC_3D_INPUT_TOP_BOTTOM)||
       (MApi_XC_Get_3D_Input_Mode(MAIN_WINDOW) == E_XC_3D_INPUT_FIELD_ALTERNATIVE))&&
      (MApi_XC_Get_3D_Output_Mode() == E_XC_3D_OUTPUT_FRAME_ALTERNATIVE))
    {
        *u8FrcIn = 1;
        *u8FrcOut = 2;
    }
#endif
#ifdef TBP_1920X2160_OUTPUT
    else if((!g_XC_Pnl_Misc.FRCInfo.bFRC)&&
            ((gSrcInfo[MAIN_WINDOW].stCapWin.height == DOUBLEHD_1080X2P_VSIZE)||
             (gSrcInfo[MAIN_WINDOW].stCapWin.height == DOUBLEHD_1080X2I_VSIZE))&&
            (MApi_XC_Get_3D_Input_Mode(MAIN_WINDOW) == E_XC_3D_INPUT_FRAME_PACKING)&&
            (MApi_XC_Get_3D_Output_Mode() == E_XC_3D_OUTPUT_TOP_BOTTOM))
    {
        *u8FrcIn = 1;
        *u8FrcOut = 1;
    }
#endif

    MDrv_WriteByte(H_BK_LPLL(0x0C), ((*u8FrcOut-1) << 4) | (*u8FrcIn-1));
    LPLL_BK_RESTORE;

    //for force case
    if (_bForceFreerun)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "force free run\r\n") ;

        u16OutputVFreq = MDrv_SC_VFreq_Select(u16OutputVFreq);
        *bSetFrameLock = FALSE;
    }
    //for normal case
    else if (((u16OutputVFreq >= 470) && (u16OutputVFreq <= 510)) ||
             ((u16OutputVFreq >= 590) && (u16OutputVFreq <= 610)) ||
            ((((u16OutputVFreq >= 980) && (u16OutputVFreq <= 1020))||
            ((u16OutputVFreq >= 1180) && (u16OutputVFreq <= 1220)))&&
             ((u16DefaultVFreq == 1000)||(u16DefaultVFreq == 1200)))||
#if (HW_DESIGN_3D_VER == 4)
            // for FRC FPP-FA 1280x1470@60/SBSH-FA/TBP-FA
            (((u16OutputVFreq >= 1180) && (u16OutputVFreq <= 1220)) && (u16DefaultVFreq == 600) &&
              (MApi_XC_Get_3D_Output_Mode() == E_XC_3D_OUTPUT_FRAME_ALTERNATIVE)) ||

#endif
               _bUseCustomFRCTable
             )
    {
        XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "Frame Lock = True\r\n");
        //u16OutputVFreq = _MDrv_SC_RefineVFreq(MAIN_WINDOW, u16InputVFreq, u16OutputVFreq);
        *bSetFrameLock = TRUE;
    }
    //abnormal case
    else
    {
        XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "Free Run\r\n");
        // For FRC panel and freerun at specific framerate
        if(u16DefaultVFreq > 900)
        {
            u16OutputVFreq = 1200;
        }
        else if ( u16OutputVFreq > 610 || u16OutputVFreq < 470 )
        {
            // Free run at panel default timing.
            u16OutputVFreq = u16DefaultVFreq;
        }
        *bSetFrameLock = FALSE;
    }

    return u16OutputVFreq;
}


//-----------------------------------------------------------------------------
// LPLL
//-----------------------------------------------------------------------------
void MDrv_SC_set_fpll(MS_BOOL bEnable, MS_BOOL bEnablePhaseLock, MS_U8 u8Lpll_bank)
{
    MS_U32 u32OrigLPLL = 0;

    // store bank
    LPLL_BK_STORE;
    // switch bank
    if(u8Lpll_bank)
        LPLL_BK_SWITCH(0x01);
    else
        LPLL_BK_SWITCH(0x00);

    u32OrigLPLL = MDrv_Read4Byte(L_BK_LPLL(0x0F));

    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "MDrv_SC_set_fpll(%u,%u)\r\n", bEnable, bEnablePhaseLock) ;

    //XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "MDrv_SC_set_fpll is called the VOP vend is 0x%x, GOP vend is 0x%x\r\n",u16VideoVEnd,MDrv_GOP_GWIN_GetMaxVEnd(FALSE))
    //if (u16VideoVEnd < MDrv_GOP_GWIN_GetMaxVEnd(FALSE))     //If Video size < Graphic size need to set L_BK_VOP(0x25) bit 7 to avoid noisy when display timing change
    //    MDrv_WriteRegBit(L_BK_VOP(0x25), ENABLE, BIT(7));
    //else
    //    MDrv_WriteRegBit(L_BK_VOP(0x25), DISABLE, BIT7);
    MDrv_WriteRegBit(L_BK_LPLL(0x0C), bEnable, BIT(3));
    MDrv_WriteRegBit(L_BK_LPLL(0x0C), bEnablePhaseLock, BIT(6));

    // !!Initial set have to re-update again to force LPLL free run at initial state or
    // lock from initial set!!
    MDrv_Write3Byte(L_BK_LPLL(0x0F), u32OrigLPLL);

    // restore bank
    LPLL_BK_RESTORE;


}

void MDrv_SC_clear_lpll(void)
{
    SC_W2BYTEMSK(REG_SC_BK01_0F_L, 0x40, BIT(6));
}

void MDrv_SC_monitor_lpll(void)
{
    MS_U16 u16Tmp;
    //MS_U8 u8Bank;

    u16Tmp = SC_R2BYTE(REG_SC_BK01_0F_L);

    if (u16Tmp & BIT(6))
    {
        //MDrv_WriteByte(L_BK_IP1F2(0x0F), u8Tmp & ~(BIT(6)));
        SC_W2BYTE(REG_SC_BK01_0F_L, u16Tmp & ~(BIT(6)));
    }

}

static void MDrv_SC_adjust_output_htt_vtt(MS_U16 *u16Htt, MS_U16 *u16Vtt,
                                          MS_U16 u16VFreq,
                                          MS_U16 u16DefaultHtt,
                                          MS_U16 u16DefaultVtt,
                                          MS_U16 u16DefaultVFreq,
                                          MS_BOOL bChangeVtt)
{
    MS_U32 u32DCLK;
    u32DCLK = ((MS_U32)u16DefaultHtt) * u16DefaultVtt * u16DefaultVFreq/10;

    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING,
        "default Htt=%u, Vtt=%u, VFreq=%u, dclk=%u\r\n",
        u16DefaultHtt,
        u16DefaultVtt,
        u16DefaultVFreq, (unsigned int)u32DCLK);

    if(u16VFreq == 0)
    {
        return;
    }
    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "1.Htt=%u, Vtt=%u, VFreq=%u, dclk=%u\r\n",
        *u16Htt, *u16Vtt, u16VFreq, (unsigned int)(((MS_U32)(*u16Htt)) * (*u16Vtt) * u16VFreq/10));

    if (u16VFreq != u16DefaultVFreq)
    {
        if (bChangeVtt)
        {
            *u16Vtt = (MS_U16)((u32DCLK*10 + (u16VFreq * (*u16Htt))/2) / (u16VFreq * (*u16Htt)));
        }
        else
        {
            *u16Htt = (MS_U16)((u32DCLK*10 + (u16VFreq * (*u16Vtt))/2) / (u16VFreq * (*u16Vtt)));
        }
    }

    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "2.Htt=%u, Vtt=%u, VFreq=%u, dclk=%u\r\n",
        *u16Htt, *u16Vtt, u16VFreq, (unsigned int)(((MS_U32)(*u16Htt)) * (*u16Vtt) * u16VFreq/10));

}

void MDrv_SC_set_fastlpll(MS_BOOL bEn)
{
    MDrv_WriteRegBit(H_BK_LPLL(0x1E), bEn, 0x20);  //Enable/Disable Fast LPLL
}

MS_BOOL MDrv_SC_get_fastlpll(void)
{
    return MDrv_ReadRegBit(H_BK_LPLL(0x1E), 0x20);  //Enable/Disable FBL
}
E_APIXC_ReturnValue MApi_XC_SetOutputAdjustSetting(XC_OUTPUT_TIMING_ADJUST_SETTING *stAdjSetting)
{
    XC_PANEL_INFO_EX stPanelInfoEx;
    if (stAdjSetting==NULL)
    {
        return E_APIXC_RET_FAIL;
    }

    memset(&stPanelInfoEx, 0, sizeof(XC_PANEL_INFO_EX));
    stPanelInfoEx.bVttDelayValid = TRUE;
    stPanelInfoEx.u16HttDelay = stAdjSetting->u16VTotal_Delay;
    stPanelInfoEx.bVttStepsValid = TRUE;
    stPanelInfoEx.u16HttSteps = stAdjSetting->u16VTotal_Step;
    stPanelInfoEx.bHttDelayValid = TRUE;
    stPanelInfoEx.u16VttDelay = stAdjSetting->u16HTotal_Delay;
    stPanelInfoEx.bHttStepsValid = TRUE;
    stPanelInfoEx.u16VttSteps= stAdjSetting->u16HTotal_Step;
    stPanelInfoEx.bDclkDelayValid = TRUE;
    stPanelInfoEx.u16DclkDelay = stAdjSetting->u16Dclk_Delay;
    stPanelInfoEx.bDclkStepsValid = TRUE;
    stPanelInfoEx.u16DclkSteps= stAdjSetting->u16Dclk_Step;
    stPanelInfoEx.u32PanelInfoEx_Version = PANEL_INFO_EX_VERSION;
    stPanelInfoEx.u16PanelInfoEX_Length = sizeof(XC_PANEL_INFO_EX);
    //Step setting for change to 50hz
    stPanelInfoEx.u16VFreq = 500;
    if(MApi_XC_SetExPanelInfo(TRUE, &stPanelInfoEx) == FALSE)
    {
        return E_APIXC_RET_FAIL;
    }
    //Step setting for change to 60hz
    stPanelInfoEx.u16VFreq = 600;
    if(MApi_XC_SetExPanelInfo(TRUE, &stPanelInfoEx) == FALSE)
    {
        return E_APIXC_RET_FAIL;
    }
    return E_APIXC_RET_OK;
}

MS_BOOL MApi_XC_Set_CustomerSyncInfo(MS_U16 u16FrontPorch, MS_U16 u16SyncWidth, MS_U16 u16OutputVfreq)
{
    XC_PANEL_INFO_EX stPanelInfoEx;
    MS_U16 u16Vtt;
    if(g_XC_InitData.stPanelInfo.enPnl_Out_Timing_Mode != E_XC_PNL_CHG_VTOTAL)
    {
        return FALSE;
    }
    if(u16OutputVfreq == g_XC_InitData.stPanelInfo.u16DefaultVFreq)
    {
        u16Vtt = g_XC_InitData.stPanelInfo.u16VTotal;
    }
    else
    {
        u16Vtt = g_XC_InitData.stPanelInfo.u16VTotal*g_XC_InitData.stPanelInfo.u16DefaultVFreq/u16OutputVfreq;
    }
    memset(&stPanelInfoEx, 0, sizeof(XC_PANEL_INFO_EX));
    stPanelInfoEx.bVSyncBackPorchValid = TRUE;
    stPanelInfoEx.bVSyncWidthValid     = TRUE;
    stPanelInfoEx.u16VSyncBackPorch = u16Vtt - g_XC_InitData.stPanelInfo.u16Height - u16FrontPorch - u16SyncWidth;
    stPanelInfoEx.u16VSyncWidth = u16SyncWidth;
    stPanelInfoEx.u16VFreq = u16OutputVfreq;
    stPanelInfoEx.u32PanelInfoEx_Version = PANEL_INFO_EX_VERSION;
    stPanelInfoEx.u16PanelInfoEX_Length = sizeof(XC_PANEL_INFO_EX);
    return MApi_XC_SetExPanelInfo(TRUE, &stPanelInfoEx);
}

/*
    Only for adjusting.
    E_XC_PNL_CHG_DCLK,
    E_XC_PNL_CHG_HTOTAL,
    E_XC_PNL_CHG_VTOTAL
    Adjust different type of output timing paramete base on Step/Delay and Ratio to u16Target_Value
*/
#define MIN_STEP_VALUE                  10
#define MIN_DIFF_VALUE_FOR_STEPCHANGE   50
static E_APIXC_ReturnValue _MDrv_XC_SetHttVtt_Steply(EN_OUTPUT_TIMING_ADJUST_MODE AdjustMode,MS_U16 u16Target_Value,
                                                     MS_U16 u16UserStep, MS_U16 u16UserDelay)
{

    MS_U16 u16Step, u16Temp, u16Old_Value = 0;
    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "SetHttVtt Steply: Mode=%u, Value=%u, Step=%u, Delay=%u\r\n", AdjustMode, u16Target_Value, u16UserStep, u16UserDelay)
    if (u16UserStep > 1 && AdjustMode != EN_OUTPUT_TIMING_CHG_DCLK)
    {
        if (AdjustMode==EN_OUTPUT_TIMING_VTOTAL)
        {
            u16Old_Value = SC_R2BYTE(REG_SC_BK10_0D_L)+1;
        }
        else if (AdjustMode==EN_OUTPUT_TIMING_HTOTAL)
        {
            u16Old_Value = SC_R2BYTE(REG_SC_BK10_0C_L)+1;
        }
        else
        {
            u16Old_Value = 0;
        }
        u16Temp = abs(u16Target_Value - u16Old_Value);
        u16Step = (u16Temp + u16UserStep - 1)/u16UserStep;
        if(u16Temp < MIN_DIFF_VALUE_FOR_STEPCHANGE)
        {
            //If the difference or stepvalue is too small, take it as one step to save time
            //Add this to prevent 60HZ FBL to 60HZ FB step change
            u16Step = u16Temp;
            u16UserStep = 1;
        }
        XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "Step start from %u\r\n", u16Old_Value)

        if ( u16Old_Value >= u16Target_Value )
        {
            while(u16Old_Value > u16Target_Value + u16Step)
            {
                u16Old_Value -= u16Step;
                if (AdjustMode==EN_OUTPUT_TIMING_VTOTAL)
                {
                    if(MApi_XC_MLoad_GetStatus(MAIN_WINDOW) == E_MLOAD_ENABLED)
                    {
                        _MLOAD_ENTRY();
                        MApi_XC_MLoad_WriteCmd(REG_SC_BK10_0D_L,(u16Old_Value - 1)&0xFFF, 0xFFF  );
                        _MLOAD_RETURN();
                    }
                    else
                    {
                        SC_W2BYTE(REG_SC_BK10_0D_L,(u16Old_Value - 1)&0xFFF);
                    }
                }
                else if (AdjustMode==EN_OUTPUT_TIMING_HTOTAL)
                {
                    if(MApi_XC_MLoad_GetStatus(MAIN_WINDOW) == E_MLOAD_ENABLED)
                    {
                        _MLOAD_ENTRY();
                        MApi_XC_MLoad_WriteCmd(REG_SC_BK10_0C_L,(u16Old_Value - 1), 0xFFF  );
                        _MLOAD_RETURN();
                    }
                    else
                    {
                        SC_W2BYTE(REG_SC_BK10_0C_L,(u16Old_Value - 1));
                    }
                }
                XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, " -> %u", (u16Old_Value - 1))
                MsOS_DelayTask(u16UserDelay);
            }
        }
        else
        {
            while(u16Old_Value < u16Target_Value - u16Step)
            {
                u16Old_Value += u16Step;
                if (AdjustMode==EN_OUTPUT_TIMING_VTOTAL)
                {
                    if(MApi_XC_MLoad_GetStatus(MAIN_WINDOW) == E_MLOAD_ENABLED)
                    {
                        _MLOAD_ENTRY();
                        MApi_XC_MLoad_WriteCmd(REG_SC_BK10_0D_L,(u16Old_Value - 1)&0xFFF, 0xFFF  );
                        _MLOAD_RETURN();
                    }
                    else
                    {
                        SC_W2BYTE(REG_SC_BK10_0D_L,(u16Old_Value - 1)&0xFFF);
                    }
                }
                else if (AdjustMode==EN_OUTPUT_TIMING_HTOTAL)
                {
                    if(MApi_XC_MLoad_GetStatus(MAIN_WINDOW) == E_MLOAD_ENABLED)
                    {
                        _MLOAD_ENTRY();
                        MApi_XC_MLoad_WriteCmd(REG_SC_BK10_0C_L,(u16Old_Value - 1), 0xFFF  );
                        _MLOAD_RETURN();
                    }
                    else
                    {
                        SC_W2BYTE(REG_SC_BK10_0C_L,(u16Old_Value - 1));
                    }
                }
                XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, " -> %u", (u16Old_Value - 1))
                MsOS_DelayTask(u16UserDelay);
            }
        }
    }

    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, " -> %u\r\n", (u16Target_Value-1))
    if (AdjustMode==EN_OUTPUT_TIMING_VTOTAL)
    {
        if(MApi_XC_MLoad_GetStatus(MAIN_WINDOW) == E_MLOAD_ENABLED)
        {
            MApi_XC_MLoad_WriteCmd(REG_SC_BK10_0D_L,(u16Target_Value-1)&0xFFF, 0xFFF  );
        }
        else
        {
            SC_W2BYTE(REG_SC_BK10_0D_L,(u16Target_Value-1)&0xFFF);
        }
    }
    else if (AdjustMode==EN_OUTPUT_TIMING_HTOTAL)
    {
        if(MApi_XC_MLoad_GetStatus(MAIN_WINDOW) == E_MLOAD_ENABLED)
        {
            _MLOAD_ENTRY();
            MApi_XC_MLoad_WriteCmd(REG_SC_BK10_0C_L,u16Target_Value-1, 0xFFF  );
            _MLOAD_RETURN();
        }
        else
        {
            SC_W2BYTE(REG_SC_BK10_0C_L,u16Target_Value-1);
        }
    }
    else
    {
        //Todo: Adjust Dclk
        return E_APIXC_RET_FAIL;
    }

    return E_APIXC_RET_OK;
}

//-------------------------------------------------------------------------------------------------
/// Set the FPLL mode:
/// @param  bTrue      \b IN: True: FSM mode, False: Non FSM mode
//-------------------------------------------------------------------------------------------------
void MDrv_XC_FPLLCusReset(void)
{
    int i;
    for (i = 0; i < E_FPLL_FLAG_MAX; i++)
    {
        _bFpllCusFlag[i]    = FALSE;
        _u32FpllCusData[i]  = 0;
    }
}

//-------------------------------------------------------------------------------------------------
/// Set the custmer FPLL parameters(You can just call this function once when system init for testing)
/// @param  eMode:Enable/disable customer setting      \b IN: @ref EN_FPLL_MODE
/// @param  eFlag:The specified FPLL parameter type which to be modified      \b IN: @ref EN_FPLL_FLAG
/// @param  eFlag:The value to set      \b IN: @ref MS_U32
/// Note: the setting will take effect after running of function MApi_XC_SetPanelTiming
//-------------------------------------------------------------------------------------------------
void MApi_XC_FPLLCustomerMode(EN_FPLL_MODE eMode, EN_FPLL_FLAG eFlag, MS_U32 u32Data)
{
    if (eMode >= E_FPLL_MODE_MAX)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING,"Wrong Fpll Mode selected!\r\n");
        return;
    }

    if (eFlag >= E_FPLL_FLAG_MAX)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING,"Wrong Fpll Flag!\r\n");
        return;
    }

    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING,"Fpll Customer Mode: eMode=0x%x, eFlag=0x%x, u32Data=0x%lx\r\n", eMode, eFlag, u32Data);
    if (eMode == E_FPLL_MODE_DISABLE_ALL)
    {
        MDrv_XC_FPLLCusReset();
    }
    else if (eMode == E_FPLL_MODE_DISABLE)
    {
        _bFpllCusFlag[eFlag]      = FALSE;
    }
    else
    {
        _bFpllCusFlag[eFlag]      = TRUE;
        _u32FpllCusData[eFlag]    = u32Data;
    }
}

//For debug purpose only
void MApi_XC_FPLLDbgMode(EN_FPLL_DBG_MODE DbgMode, EN_FPLL_DBG_FLAG DbgFlag, MS_U32 u32Data)
{
    MApi_XC_FPLLCustomerMode((EN_FPLL_MODE)DbgMode, (EN_FPLL_FLAG)DbgFlag, u32Data);
}

//-------------------------------------------------------------------------------------------------
// This function will force freerun in MDrv_Scaler_SetPanelTiming(),
// otherwise MDrv_Scaler_SetPanelTiming() will decide to do framelock or not by itself.
//
// Note: this function is difference with MDrv_SC_set_fpll() which is used to
//       unlock framelock after MDrv_Scaler_SetPanelTiming().
//       But this function is used to affect MDrv_Scaler_SetPanelTiming()'s behavior.
//-------------------------------------------------------------------------------------------------
void MApi_SC_ForceFreerun(MS_BOOL bEnable)
{
    _bForceFreerun = bEnable;
}

//-------------------------------------------------------------------------------------------------
// This function will return the force freerun state
//-------------------------------------------------------------------------------------------------
MS_BOOL MApi_SC_IsForceFreerun(void)
{
    return _bForceFreerun;
}

void MApi_SC_SetFreerunVFreq(E_VFREQ_SEL VFreq)
{
    _ForceVFreq     = VFreq;
}

MS_U16 MDrv_SC_VFreq_Select(MS_U16 u16AutoVFreq)
{
    if(_ForceVFreq == VFREQ_50HZ)
    {
        return 500;
    }
    else if(_ForceVFreq == VFREQ_60HZ)
    {
        return 600;
    }
    else if(_ForceVFreq == VFREQ_FROM_SRC)
    {
        return u16AutoVFreq;
    }
    else if (_ForceVFreq == VFREQ_FROM_PANEL)
    {
        return g_XC_InitData.stPanelInfo.u16DefaultVFreq;
    }
    else
    {
        XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "MDrv_SC_VFreq_Select(): wrong Vfreq_SEL input, use default logic!!");
        return u16AutoVFreq;
    }
}
//-------------------------------------------------------------------------------------------------
// This function will Get the exact ext_panel info structure for input vfreq
// IN:
//    pPanelInfoEx: ext-panel info for return,
//                  if u16VFreq=0, pPanelInfoEx= next address to save new panel info
//                  else return the panel info pointer of "VFreq"
//    u16VFreq: the request vfreq for getting ext-panel info
// Return:
//    TRUE: Success, FALSE: FAIL
//-------------------------------------------------------------------------------------------------
static MS_BOOL _MDrv_XC_GetExPanelInfo(XC_PANEL_INFO_EX **ppPanelInfoEx, MS_U16 u16VFreq)
{
    MS_BOOL bRet = FALSE;
    MS_U8 i;
    *ppPanelInfoEx = NULL;
    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "u16VFreq=%u\r\n", u16VFreq)

    for(i=0; i<PANEL_INFO_EX_NUM; i++)
    {
        if(u16VFreq == 0)
        {
            //Get next available place for store
            if(_stPanelInfoEx[i].u32PanelInfoEx_Version == 0)
            {
                bRet = TRUE;
                *ppPanelInfoEx = &_stPanelInfoEx[i];
                break;
            }
        }
        else
        {
            //Get ex panel info of specified "u16VFreq"
            if(_stPanelInfoEx[i].u32PanelInfoEx_Version != 0 &&
               abs(_stPanelInfoEx[i].u16VFreq - u16VFreq) <= VFREQ_DIFF_TOLERANCE )
            {
                bRet = TRUE;
                *ppPanelInfoEx = &_stPanelInfoEx[i];
                break;
            }
        }
    }
    //printf("Get ExPanelInfo u16VFreq=%u, pPanelInfoEx=0x%lx, bRet=%u\r\n", u16VFreq, (MS_U32)*ppPanelInfoEx, bRet);
    return bRet;
}

//-------------------------------------------------------------------------------------------------
/// Set external panel info for output
/// Currently, only support setting of two different vfreqs at same time
/// @param  bEnable             \b IN: TRUE: Enable this XC_PANEL_INFO_EX, FALSE: Disable this XC_PANEL_INFO_EX
/// @param  pPanelInfoEx        \b IN: The extern panel information to set @ref XC_PANEL_INFO_EX
/// @return @ref MS_BOOL               TRUE: Success, FALSE: Failed
/// Note: the setting will take effect after running of function MApi_XC_SetPanelTiming
//-------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_SetExPanelInfo(MS_BOOL bEnable, XC_PANEL_INFO_EX *pPanelInfoEx)
{
    MS_BOOL bRet = TRUE;
    XC_PANEL_INFO_EX *pPnlInfoEx = NULL;
    if(NULL == pPanelInfoEx || pPanelInfoEx->u32PanelInfoEx_Version < 2)
    {
        //We cosider compatible operation form version2 , so reject the info init when version invalid
        XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "MApi_XC_EnablePanelInfo: Version or pPanelInfoEx is Invalid!!")
        bRet = FALSE;
    }
    if(bRet)
    {
        MS_U16 u16VFreq = pPanelInfoEx->u16VFreq;
        MS_U16 u16Length = (pPanelInfoEx->u16PanelInfoEX_Length > sizeof(XC_PANEL_INFO_EX)) ? \
               sizeof(XC_PANEL_INFO_EX) : pPanelInfoEx->u16PanelInfoEX_Length;
        XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "MApi_XC_SetExPanelInfo: bEnable=%u, u16VFreq=%u\r\n", bEnable, u16VFreq)
        if(bEnable)
        {
            bRet = _MDrv_XC_GetExPanelInfo(&pPnlInfoEx, 0);
            if(bRet && pPnlInfoEx != NULL)
            {
                //There are still space to store this info, so copy it
                memcpy(pPnlInfoEx, pPanelInfoEx, u16Length);
            }
        }
        else
        {
            if(u16VFreq == PANEL_INFO_EX_INVALID_ALL)
            {
                //Clear all info in ex panel info structure
                memset(_stPanelInfoEx, 0, (sizeof(XC_PANEL_INFO_EX))*PANEL_INFO_EX_NUM);
            }
            else
            {
                //Clear ex panel info structure of specified "VFreq"
                bRet = _MDrv_XC_GetExPanelInfo(&pPnlInfoEx, u16VFreq);
                if(bRet && pPnlInfoEx != NULL)
                {
                    memset(pPnlInfoEx, 0, sizeof(XC_PANEL_INFO_EX));
                }
            }
        }
    }
    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "MApi_XC_SetExPanelInfo Exit: bRet=%u, pPnlInfoEx=%lx\r\n", bRet, (MS_U32)pPnlInfoEx)
    return bRet;
}

void MDrv_Scaler_SetIGainPGain(MS_U8 u8FRC_Out, MS_U16 u16FRC_VFreq)
{
    //    MS_U8   i,u8Vco,u8LGain2 = 1,u8LGain1=1;
    MS_U8   u8Vco, u8LGain1=1;
    MS_U32  u32Factor, u32dclk=0, u32Temp=0;
    MS_U8   u8GainI, u8GainP, u8StepI = DEFAULT_STEP_I, u8StepP = DEFAULT_STEP_P;

    //check parameter valid, avoiding the diving zero failure
    if(0 == u8FRC_Out)
    {
        u8FRC_Out = 1;
    }
    //=================================================
    //  FPLL  I/P gain setting
    //
    //                        MM * 524288*loop_gain
    //  i_gain = ---------------------------
    //                   8 * Htt * Vtt * (Ovs + 1 ) * Vco
    //
    //  MM = MPLL / Xtal = 216 / 12(MHz) = 18
    //  loop_gain = 2^(reg_loop_gain_1st) * loop_gain_2nd
    //  loop_gain_2nd = loop_gain_2nd == 0 ? 1 : reg_loop_gain_2nd;
    //  Vco = LVDS == Dual mode ? 7 : 3.5;
    //  p_gain = i_gain *2;
    //  i_gain = 2^(reg_i_gain-5)
    //  p_gain = 2^(reg_p_gain-5)
    //  reg_loop_gain_1st = BK_0x3101[1:0]
    //  loop_gain_2nd       = BK_0x3101[15:8]
    //  Dual mode              = BK_0x3103[8]
    //  Ovs                         = BK_0x310C[15:12]
    //  reg_i_gain              = BK_0x310B[11:8]
    //  reg_p_gain              = BK_0x310B[15:12]
    //--------------------------------------------------------
    u32Temp = (MS_U32)(g_XC_InitData.stPanelInfo.u16VTotal*g_XC_InitData.stPanelInfo.u16HTotal*u8FRC_Out);

    u32dclk = (MS_U32)(g_XC_InitData.stPanelInfo.u16VTotal*g_XC_InitData.stPanelInfo.u16HTotal*u16FRC_VFreq);


    u8LGain1 = (MS_U8)MHal_PNL_Get_LPLL_LoopGain(g_XC_InitData.stPanelInfo.eLPLL_Type, u32dclk);

    u8Vco = MHal_PNL_Get_Loop_DIV(g_XC_InitData.stPanelInfo.u8LPLL_Mode, g_XC_InitData.stPanelInfo.eLPLL_Type, u32dclk);

    // MM = MPLL / Xtal = (LVDS_MPLL_CLOCK_MHZ*1000000UL) /g_XC_InitData.u32XTAL_Clock
    u32Factor = ((LVDS_MPLL_CLOCK_MHZ*1000000UL/g_XC_InitData.u32XTAL_Clock)*524288*u8LGain1*8)/((MS_U32)(u32Temp*u8Vco));

    if (u8FRC_Out >= 2)
    {
        u8StepI = STEP_I;
        u8StepP = STEP_P;
    }

    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "MM=%ld,u8LGain1=%d,Htt=%d,Vtt=%d,Ovs=%d,Vco=%d,i_gain=%ld,REGIg=%d,StepI=%d, StepP=%d\r\n",
        (LVDS_MPLL_CLOCK_MHZ*1000000UL/g_XC_InitData.u32XTAL_Clock),u8LGain1,g_XC_InitData.stPanelInfo.u16HTotal,
        g_XC_InitData.stPanelInfo.u16VTotal,u8FRC_Out,u8Vco/2,u32Factor,MHal_SC_cal_usedgain_to_reggain(u32Factor),u8StepI,u8StepP);

    u8GainI = MHal_SC_cal_usedgain_to_reggain(u32Factor*8/u8StepI);
    u8GainP = MHal_SC_cal_usedgain_to_reggain(u32Factor*8/u8StepP);

    if((u8FRC_Out >= 2) && (u8GainP == u8GainI))
    {
        u8GainP++;
    }
    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "==>LG1=%d,iGain=%ld,I=%d,P=%d\r\n",u8LGain1,u32Factor,u8GainI,u8GainP);
    //=================================================
    if( ((u8GainP>6) || (u8GainI>5)))// && (_bFSM_FPLL_Working == TRUE))
    {
        u8GainP = 6;
        u8GainI = 5;
    }

    if (_bFpllCusFlag[E_FPLL_FLAG_IGAIN])
    {
        u8GainI = (MS_U8)_u32FpllCusData[E_FPLL_FLAG_IGAIN];
        XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "user forces REG_IGain= 0x%X (%u)\r\n", u8GainI, u8GainI);
    }
    if (_bFpllCusFlag[E_FPLL_FLAG_PGAIN])
    {
        u8GainP = (MS_U8)_u32FpllCusData[E_FPLL_FLAG_PGAIN];
        XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "user forces REG_PGain= 0x%X (%u)\r\n", u8GainP, u8GainP);
    }


    LPLL_BK_STORE;
    LPLL_BK_SWITCH(0x00);
    MDrv_WriteByte(H_BK_LPLL(0x0B), (u8GainP<<4)|u8GainI);  //Gain
    LPLL_BK_RESTORE;

    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "i_Gain= 0x%X (%u)\r\n", (unsigned int)u32Factor, (unsigned int)u32Factor)
    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "REG_IGain= 0x%X (%u)\r\n", u8GainI, u8GainI)
    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "REG_PGain= 0x%X (%u)\r\n", u8GainP, u8GainP)
}
//-------------------------------------------------------------------------------------------------
// This function will Set the H or V sync information: SyncStart/end/width
// IN:
//    H/V Sync infomation and H/V output total that corresponding to this sync info.
// Return:
//    NONE
//-------------------------------------------------------------------------------------------------
static void _MDrv_XC_SetHVSync(MS_BOOL bHSync, MS_U16 u16SyncWidth, MS_U16 u16SyncBackPorch, MS_U16 u16HVTotal)
{
    //Change to big one: Change Vtt first to avoid changed vsync end > old vtt
    MS_U16 u16Temp;
    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "bHSync=%u, SyncWidth=%u, SyncBackPorch=%u, Total=%u\r\n",
                 bHSync, u16SyncWidth, u16SyncBackPorch, u16HVTotal)
    if(bHSync)
    {
        if(MApi_XC_MLoad_GetStatus(MAIN_WINDOW) == E_MLOAD_ENABLED)
        {
            _MLOAD_ENTRY();
            MApi_XC_MLoad_WriteCmd(REG_SC_BK10_04_L, u16SyncWidth + u16SyncBackPorch, VOP_DE_HSTART_MASK);  // output HDE start
            MApi_XC_MLoad_WriteCmd(REG_SC_BK10_01_L, u16SyncWidth, 0xFF);    // output Hsync Width
            _MLOAD_RETURN();
        }
        else
        {
            u16Temp = SC_R2BYTEMSK(REG_SC_BK10_01_L, 0xFF);//Old Hsync Width
            if(u16Temp > u16SyncWidth)
            {
                //Change to small one
                SC_W2BYTEMSK(REG_SC_BK10_01_L, u16SyncWidth, 0xFF);  // output HDE start
                SC_W2BYTEMSK(REG_SC_BK10_04_L, u16SyncWidth + u16SyncBackPorch, VOP_DE_HSTART_MASK);
            }
            else
            {
                //Change to big one
                SC_W2BYTEMSK(REG_SC_BK10_04_L, u16SyncWidth + u16SyncBackPorch, VOP_DE_HSTART_MASK);
                SC_W2BYTEMSK(REG_SC_BK10_01_L, u16SyncWidth, 0xFF);
            }
        }
    }
    else
    {

        MS_U16 u16NewVsyncEnd = u16HVTotal-u16SyncBackPorch;
        if(MApi_XC_MLoad_GetStatus(MAIN_WINDOW) == E_MLOAD_ENABLED)
        {
            _MLOAD_ENTRY();
            MApi_XC_MLoad_WriteCmd(REG_SC_BK10_02_L, u16NewVsyncEnd-u16SyncWidth, 0xFFFF);  // output Vsync start
            MApi_XC_MLoad_WriteCmd(REG_SC_BK10_03_L, u16NewVsyncEnd, VOP_VSYNC_END_MASK);    // output Vsync end
            _MLOAD_RETURN();
        }
        else
        {
            u16Temp = SC_R2BYTEMSK(REG_SC_BK10_03_L, VOP_VSYNC_END_MASK);//Old Vsync End
            if(u16Temp > u16NewVsyncEnd)
            {
                //Change to small one
                SC_W2BYTE(REG_SC_BK10_02_L, u16NewVsyncEnd-u16SyncWidth);  // output Vsync start
                SC_W2BYTE(REG_SC_BK10_03_L, u16NewVsyncEnd);
            }
            else
            {
                //Change to big one
                SC_W2BYTE(REG_SC_BK10_03_L, u16NewVsyncEnd);
                SC_W2BYTE(REG_SC_BK10_02_L, u16NewVsyncEnd-u16SyncWidth);  // output Vsync start
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------
// This function will Enable/Disable manual Vsync Mode
// IN:
//    bManualMode: TRUE---Manual Mode, False---Auto Mode
// Return:
//    NONE
//-------------------------------------------------------------------------------------------------
static void _MDrv_XC_SetOutputVSyncMode(MS_BOOL bManualMode)
{
    //Change to big one: Change Vtt first to avoid changed vsync end > old vtt
    MS_U16 u16OldVSyncMode = SC_R2BYTE(REG_SC_BK10_10_L) & BIT(15);
    if((((MS_U16)bManualMode)<<15) !=  u16OldVSyncMode)
    {
        u16OldVSyncMode = (((MS_U16)bManualMode)<<15);
        if(MApi_XC_MLoad_GetStatus(MAIN_WINDOW) == E_MLOAD_ENABLED)
        {
            _MLOAD_ENTRY();
            MApi_XC_MLoad_WriteCmd(REG_SC_BK10_10_L, u16OldVSyncMode, BIT(15));  // output Vsync start
            _MLOAD_RETURN();
        }
        else
        {
            SC_W2BYTEMSK(REG_SC_BK10_10_L, u16OldVSyncMode, BIT(15));    //Manual output Vsync
        }
    }
}

void MDrv_SC_set_Htt_Vtt(XC_SetTiming_Info *pTimingInfo, MS_U16 *pu16VFreq, XC_DISPLAYOUT_INFO *pDisplyout_Info, SCALER_WIN eWindow)
{
    MS_U16 u16OldHtt, u16OldVtt;
    MS_U16 u16HSyncWidth, u16HBackPorch, u16VSyncWidth, u16VBackPorch;
    MS_U16 u16ChangeHttSteps=1,u16ChangeHttDelay=0,u16ChangeVttSteps=1,u16ChangeVttDelay=0;
    XC_PANEL_INFO_EX *pPanelInfoEx = NULL;

    pDisplyout_Info->u16Htt_out = g_XC_InitData.stPanelInfo.u16DefaultHTotal;
    pDisplyout_Info->u16Vtt_out = g_XC_InitData.stPanelInfo.u16DefaultVTotal;

    u16HSyncWidth = _u16DefaultHSyncWidth;
    u16HBackPorch = _u16DefaultHSyncBackPorch;
    u16VSyncWidth = _u16DefaultVSyncWidth;
    u16VBackPorch = _u16DefaultVSyncBackPorch;

    u16OldVtt = SC_R2BYTEMSK(REG_SC_BK10_0D_L, 0xFFF);
    u16OldHtt = SC_R2BYTEMSK(REG_SC_BK10_0C_L, 0xFFF);
    if(_MDrv_XC_GetExPanelInfo(&pPanelInfoEx, *pu16VFreq) /*&& pPanelInfoEx != NULL*/)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "PanelInfoEx assigned\r\n")
        if(pPanelInfoEx->bHttDelayValid)
        {
            u16ChangeHttDelay = pPanelInfoEx->u16HttDelay;
        }
        if(pPanelInfoEx->bHttStepsValid)
        {
            u16ChangeHttSteps = pPanelInfoEx->u16HttSteps;
        }
        if(pPanelInfoEx->bVttDelayValid)
        {
            u16ChangeVttDelay = pPanelInfoEx->u16VttDelay;
        }
        if(pPanelInfoEx->bVttStepsValid)
        {
            u16ChangeVttSteps = pPanelInfoEx->u16VttSteps;
        }
    }
    //printf("[DEBUG] Adjust Htt Vtt \r\n");
    if(!(gSrcInfo[eWindow].bFBL || gSrcInfo[eWindow].bR_FBL) ||
       (_bForceFreerun && (_ForceVFreq != VFREQ_FROM_SRC)))
    {
        XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "PanelInfoEx: %s, org htt=%u, vtt=%u, vfreq=%u, Mode=%u\r\n", (pPanelInfoEx==NULL) ? "NO": "YES",
                     pDisplyout_Info->u16Htt_out, pDisplyout_Info->u16Vtt_out, *pu16VFreq, g_XC_InitData.stPanelInfo.enPnl_Out_Timing_Mode)

        if (g_XC_InitData.stPanelInfo.enPnl_Out_Timing_Mode != E_XC_PNL_CHG_DCLK)
        {
            if((pPanelInfoEx == NULL)
            ||(g_XC_InitData.stPanelInfo.enPnl_Out_Timing_Mode == E_XC_PNL_CHG_VTOTAL
            && pPanelInfoEx->bVTotalValid == FALSE)
            ||(g_XC_InitData.stPanelInfo.enPnl_Out_Timing_Mode == E_XC_PNL_CHG_HTOTAL
            && pPanelInfoEx->bHTotalValid == FALSE))
            {
                MDrv_SC_adjust_output_htt_vtt(&(pDisplyout_Info->u16Htt_out), &(pDisplyout_Info->u16Vtt_out), *pu16VFreq,
                            g_XC_InitData.stPanelInfo.u16DefaultHTotal,
                            g_XC_InitData.stPanelInfo.u16DefaultVTotal,
                            g_XC_InitData.stPanelInfo.u16DefaultVFreq,
                            (g_XC_InitData.stPanelInfo.enPnl_Out_Timing_Mode == E_XC_PNL_CHG_VTOTAL) ? TRUE:FALSE);
            }
        }
    }
    else
    {
        //calculate Vtt_out for FBL, Vtt_out = Vtt_in * Vde_out / Vde_in
        pDisplyout_Info->u16Vtt_out = MDrv_SC_FBL_Calc_Vtt(pTimingInfo, eWindow);
    }

    if(pPanelInfoEx != NULL)
    {
        MS_BOOL bExceeded = FALSE;
        if(!_bForceFreerun)
        {
            XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "@@@ Set Htt/Vtt By Ex panel\r\n\r\n");
            if(pPanelInfoEx->bVTotalValid)
            {
                pDisplyout_Info->u16Vtt_out = pPanelInfoEx->u16VTotal;
                XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "11: u16Vtt_out=%u\r\n",pDisplyout_Info->u16Vtt_out);
            }

            if(pPanelInfoEx->bHTotalValid)
            {
                pDisplyout_Info->u16Htt_out = pPanelInfoEx->u16HTotal;
                XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "22: u16Htt_out=%u\r\n",pDisplyout_Info->u16Htt_out);
            }
            XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "33: u16MaxVTotal = %u, u16MinVTotal = %u, u16MaxHTotal = %u, u16MinHTotal = %u\r\n",
            pPanelInfoEx->u16MaxVTotal, pPanelInfoEx->u16MinVTotal,
            pPanelInfoEx->u16MaxHTotal, pPanelInfoEx->u16MinHTotal);

            XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "44: bMaxVTotalValid = %u, bMinVTotalValid = %u, bMaxHTotalValid = %u, bMinHTotalValid = %u\r\n",
            pPanelInfoEx->bMaxVTotalValid, pPanelInfoEx->bMinVTotalValid,
            pPanelInfoEx->bMaxHTotalValid, pPanelInfoEx->bMinHTotalValid);


            if(pPanelInfoEx->bMaxVTotalValid && (pDisplyout_Info->u16Vtt_out > pPanelInfoEx->u16MaxVTotal))
            {
                pDisplyout_Info->u16Vtt_out = pPanelInfoEx->u16MaxVTotal;
                bExceeded = TRUE;
            }

            if(pPanelInfoEx->bMinVTotalValid && (pDisplyout_Info->u16Vtt_out < pPanelInfoEx->u16MinVTotal))
            {
                pDisplyout_Info->u16Vtt_out = pPanelInfoEx->u16MinVTotal;
                bExceeded = TRUE;
            }

            if(pPanelInfoEx->bMaxHTotalValid && (pDisplyout_Info->u16Htt_out > pPanelInfoEx->u16MaxHTotal))
            {
                pDisplyout_Info->u16Htt_out = pPanelInfoEx->u16MaxHTotal;
                bExceeded = TRUE;
            }

            if(pPanelInfoEx->bMinHTotalValid && (pDisplyout_Info->u16Htt_out < pPanelInfoEx->u16MinHTotal))
            {
                pDisplyout_Info->u16Htt_out = pPanelInfoEx->u16MinHTotal;
                bExceeded = TRUE;
            }

            if(bExceeded)
            {
                printf("ALERT: HTotal or VTotal is out of range, Set to default timing and freerun !!!!!!\r\n");
                pDisplyout_Info->u16Htt_out = g_XC_InitData.stPanelInfo.u16DefaultHTotal;
                pDisplyout_Info->u16Vtt_out = g_XC_InitData.stPanelInfo.u16DefaultVTotal;
                *pu16VFreq = g_XC_InitData.stPanelInfo.u16DefaultVFreq;
                MDrv_XC_SetFreeRunTiming();
            }
        }
    }
    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "new htt=%u, vtt=%u, vfreq=%u\r\n", pDisplyout_Info->u16Htt_out, pDisplyout_Info->u16Vtt_out, *pu16VFreq);
    g_XC_InitData.stPanelInfo.u16HTotal = pDisplyout_Info->u16Htt_out;
    g_XC_InitData.stPanelInfo.u16VTotal = pDisplyout_Info->u16Vtt_out;

    if(pPanelInfoEx != NULL)
    {
        if(pPanelInfoEx->bHSyncBackPorchValid ||
           pPanelInfoEx->bHSyncWidthValid)
        {
            _bHSyncChanged = TRUE;
            if(pPanelInfoEx->bHSyncBackPorchValid)
            {
                u16HBackPorch = pPanelInfoEx->u16HSyncBackPorch;
            }
            if(pPanelInfoEx->bHSyncWidthValid)
            {
                u16HSyncWidth = pPanelInfoEx->u16HSyncWidth;
            }
        }
        if(pPanelInfoEx->bVSyncBackPorchValid ||
           pPanelInfoEx->bVSyncWidthValid)
        {
            _bVSyncChanged = TRUE;
            if(pPanelInfoEx->bVSyncBackPorchValid)
            {
                u16VBackPorch = pPanelInfoEx->u16VSyncBackPorch;
            }
            if(pPanelInfoEx->bVSyncWidthValid)
            {
                u16VSyncWidth = pPanelInfoEx->u16VSyncWidth;
            }
        }
    }
    if(pDisplyout_Info->u16Vtt_out > u16OldVtt)
    {
        //Change to big one: Change Vtt first to avoid changed Vsync end > old Vtt
        _MDrv_XC_SetHttVtt_Steply(EN_OUTPUT_TIMING_VTOTAL, pDisplyout_Info->u16Vtt_out, u16ChangeVttSteps, u16ChangeVttDelay);
        if(_bVSyncChanged)
        {
            _MDrv_XC_SetHVSync(FALSE, u16VSyncWidth, u16VBackPorch, pDisplyout_Info->u16Vtt_out);
        }
    }
    else
    {
        if(_bVSyncChanged)
        {
            _MDrv_XC_SetHVSync(FALSE, u16VSyncWidth, u16VBackPorch, pDisplyout_Info->u16Vtt_out);
        }
        _MDrv_XC_SetHttVtt_Steply(EN_OUTPUT_TIMING_VTOTAL, pDisplyout_Info->u16Vtt_out, u16ChangeVttSteps, u16ChangeVttDelay);
    }

    if(pDisplyout_Info->u16Htt_out > u16OldHtt)
    {
        //Change to big one: Change Htt first to avoid changed Hsync end > old Htt
        _MDrv_XC_SetHttVtt_Steply(EN_OUTPUT_TIMING_HTOTAL, pDisplyout_Info->u16Htt_out, u16ChangeHttSteps, u16ChangeHttDelay);
        if(_bHSyncChanged)
        {
            _MDrv_XC_SetHVSync(TRUE, u16HSyncWidth, u16HBackPorch, pDisplyout_Info->u16Htt_out);
        }
    }
    else
    {
        if(_bHSyncChanged)
        {
            _MDrv_XC_SetHVSync(TRUE, u16HSyncWidth, u16HBackPorch, pDisplyout_Info->u16Htt_out);
        }
        _MDrv_XC_SetHttVtt_Steply(EN_OUTPUT_TIMING_HTOTAL, pDisplyout_Info->u16Htt_out, u16ChangeHttSteps, u16ChangeHttDelay);
    }
    if(_bHSyncChanged || _bVSyncChanged)
    {
        //Notes: Here We do not consider the dynamic switch of Manual mode and Auto mode
        //       Because there is no such panel, so no such request.
        _MDrv_XC_SetOutputVSyncMode(TRUE);
    }
    if(MApi_XC_MLoad_GetStatus(eWindow) == E_MLOAD_ENABLED)
    {
        _MLOAD_ENTRY();
        if (!MApi_XC_MLoad_Fire(TRUE))
        {
            printf("Set Htt/vtt ML Fire Error!!!!\r\n ");
        }
        _MLOAD_RETURN();
    }
    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "Htt_out, Vtt_out= %u , %u\r\n", pDisplyout_Info->u16Htt_out, pDisplyout_Info->u16Vtt_out)
}

void MApi_XC_SetOutTimingMode(E_XC_PNL_OUT_TIMING_MODE enPnl_Out_Timing_Mode)
{
    g_XC_InitData.stPanelInfo.enPnl_Out_Timing_Mode = enPnl_Out_Timing_Mode;
}

MS_U16 MDrv_SC_FBL_Calc_Vtt(XC_SetTiming_Info *pTimingInfo, SCALER_WIN eWindow)
{
    MS_U16  u16Vde_in, u16Vde_out, u16Vtt_in, u16Vtt_out;

    u16Vde_in = SC_R2BYTE(REG_SC_BK01_06_L) & 0x1FFF;

    if(u16Vde_in == 0) // avoid divide by zero.
    {
        return 0;
    }
    // Remove storage Port later ( Driver level does not know what storagePort is )
    // Merge MVD port and Storage port
    if( pTimingInfo->bMVOPSrc)
    {
        u16Vtt_in = pTimingInfo->u16InputVTotal;//MDrv_Read2Byte(VOP_FRAME_VCOUNT) & 0x7FF;
    }
    else if(IsSrcTypeAnalog(gSrcInfo[eWindow].enInputSourceType) || IsSrcTypeHDMI(gSrcInfo[eWindow].enInputSourceType))
    {
        u16Vtt_in = MDrv_XC_PCMonitor_Get_Vtotal(eWindow);
    }
    else
    {
        u16Vtt_in = Hal_SC_ip_get_verticaltotal(MAIN_WINDOW);//MDrv_Read2Byte(L_BK_IP1F2(0x1f)) & MST_V_TOTAL_MASK;
    }

    //MDrv_WriteByte(BK_SELECT_00, REG_BANK_VOP);
    //u16Vde_out = (MDrv_Read2Byte(L_BK_VOP(0x0B))&0x7FF) - (MDrv_Read2Byte(L_BK_VOP(0x0A))&0x7FF) + 1;
    u16Vde_out = (SC_R2BYTE(REG_SC_BK10_0B_L)&0x7FF) - (SC_R2BYTE(REG_SC_BK10_0A_L)&0x7FF) + 1;
    MS_ASSERT(u16Vde_in);

    u16Vtt_out = (MS_U16)(((MS_U32)u16Vtt_in * u16Vde_out + u16Vde_in/2) / u16Vde_in);

    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "Vde_in= 0x%X (%u)\r\n", u16Vde_in, u16Vde_in)
    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "Vde_out= 0x%X (%u)\r\n", u16Vde_out, u16Vde_out)
    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "Vtt_in= 0x%X (%u)\r\n", u16Vtt_in, u16Vtt_in)

    return u16Vtt_out;
}

void MDrv_SC_Set_LockFreeze_Point(XC_SetTiming_Info *pTimingInfo, MS_U16 u16Vtt_out, SCALER_WIN eWindow)
{
    MS_U16  u16LockPoint = 0, u16FreezePoint = 0;
    MS_BOOL bForceReadBank = FALSE;

    if( !gSrcInfo[eWindow].bFBL)
    {
        if(gSrcInfo[eWindow].bR_FBL) //for M10 Reguest FBL mode, it doesn't the same with FBL
        {
            u16LockPoint = u16Vtt_out;
            u16FreezePoint = u16LockPoint - 2;

            // Setip Y lock
            SC_W2BYTEMSK(REG_SC_BK01_09_L, 0x2, BITMASK(3:0));

            // Setup delay line
            if(pTimingInfo->bInterlace)
            {
                gSrcInfo[eWindow].u8DelayLines = 6;
                bForceReadBank = FALSE;
            }
            else
            {
                gSrcInfo[eWindow].u8DelayLines = 6;
                bForceReadBank = TRUE;
            }
        }
        else
        {
            MS_U16 u16DEWidth = 0, u16DEHeight = 0;

            /* 2010-07-05
            * according to the discussion from designer, we have the following formula:
            * freeze point = vde_end (reg_value) + 1;
            * lock = vde_end(reg_value) + 3;
            */
            u16FreezePoint = SC_R2BYTE(REG_SC_BK10_07_L) + 1;
            u16LockPoint = u16FreezePoint + 2;

            // T2: YLock = 4 for FB case
            SC_W2BYTEMSK(REG_SC_BK01_09_L, 0x4, BITMASK(3:0));
            if((gSrcInfo[eWindow].eDeInterlaceMode == MS_DEINT_2DDI_AVG) ||
                (gSrcInfo[eWindow].eDeInterlaceMode == MS_DEINT_2DDI_BOB))
            {
                gSrcInfo[eWindow].u8DelayLines = 1;    // 5
            }
            else
            {
                if(_bEnableMCNR &&(Hal_SC_GetFrameStoreMode(eWindow) == IMAGE_STORE_4_FIELDS))  //For 25_4R_MC mode only
                {
                    gSrcInfo[eWindow].u8DelayLines = 10;
                }
                else
                {
                    gSrcInfo[eWindow].u8DelayLines = 2;
                }
            }

            //if HV mode with capture size 0x1fff, we need to adjust delay line to 6.
            if ((MApi_XC_GetHdmiSyncMode() == HDMI_SYNC_HV) &&
                (IsSrcTypeHDMI(gSrcInfo[eWindow].enInputSourceType)) &&
                (!pTimingInfo->bInterlace) &&
                (_bEnableMCNR &&(Hal_SC_GetFrameStoreMode(eWindow) == IMAGE_STORE_3_FRAMES)))//For 25_4R_MC mode only
            {
                MDrv_XC_GetDEWidthHeightInDEByPassMode(&u16DEWidth, &u16DEHeight, MAIN_WINDOW);

                if( (u16DEWidth == 720) && ((u16DEHeight == 480) || (u16DEHeight == 576)))
                {
                    gSrcInfo[eWindow].u8DelayLines = 6;
                }
            }

            bForceReadBank = FALSE;
        }
        // Setup delay line
        Hal_SC_set_delayline( gSrcInfo[eWindow].u8DelayLines, eWindow );
    }
    else
    {
        //New method for lock point in FBL
        SC_W2BYTEMSK(REG_SC_BK01_09_L, 0x2, BITMASK(3:0));

        if(pTimingInfo->bInterlace)
        {
            gSrcInfo[eWindow].u8DelayLines = 7;
            bForceReadBank = FALSE;
        }
        else
        {
            if((stDBreg.u32V_PostScalingRatio & 0x3FFFFFL) > 0x100000)
            {
                //printf("---scaling down----\r\n");
                gSrcInfo[eWindow].u8DelayLines = DELAY_LINE_SC_DOWN;//8;
            }
            else
            {
                //printf("----scaling up----\r\n");
                gSrcInfo[eWindow].u8DelayLines = DELAY_LINE_SC_UP;// 7;
            }
            bForceReadBank = TRUE;
        }
        Hal_SC_set_delayline( gSrcInfo[eWindow].u8DelayLines, eWindow );

        //SC0F_55 = SC10_0D, SC0F_54 = SC0F_55 -2
        u16LockPoint = u16Vtt_out;
        u16FreezePoint = u16LockPoint - 2;
    }

    // Only turn on when FBL/RFBL and the signal is progressive mode.
    SC_W2BYTEMSK(REG_SC_BK12_07_L, (bForceReadBank? ((MS_U16)0x8)<<8 : 0), BITMASK(11:8));

    SC_W2BYTEMSK(REG_SC_BK0F_56_L, BIT(1), BIT(1));  //Using new ovs_ref
    SC_W2BYTE(REG_SC_BK0F_55_L, (u16LockPoint-1)&0x7FF);
    SC_W2BYTE(REG_SC_BK0F_54_L, (u16FreezePoint-1)&0x7FF);    //FreezePoint = LockPoint-2

    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "LockPoint= 0x%X (%u)\r\n", u16LockPoint, u16LockPoint)
}

void MDrv_Scaler_SetPhaseLimit(MS_U8 u8Lpll_bank)
{
    MS_U16 u16PhaseLimit = PHASE_OFFSET_LIMIT;

    //when the dclk max-min value is too small, the panel has hw limitation. we should use
    //much safer parameter to do the FPLL
    //for now, we only found only AU46_T460hw03v1_HD has this problem.
    //So the value:0x0A is coming from AU46_T460hw03v1_HD
    if((g_XC_InitData.stPanelInfo.u32MaxSET - g_XC_InitData.stPanelInfo.u32MinSET)
        < GATE_PHASELIMIT)
    {
        u16PhaseLimit = 0x1000;
    }

    if (_bFpllCusFlag[E_FPLL_FLAG_PHASELIMIT])
    {
        u16PhaseLimit = (MS_U16)_u32FpllCusData[E_FPLL_FLAG_PHASELIMIT];
        XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "user forces Phase Limit = 0x%X\r\n", u16PhaseLimit);
    }
    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "Phase Limit = 0x%X\r\n", u16PhaseLimit);

    LPLL_BK_STORE;
    if(u8Lpll_bank)
        LPLL_BK_SWITCH(0x01);
    else
        LPLL_BK_SWITCH(0x00);

    MDrv_Write2Byte( L_BK_LPLL(0x0A), u16PhaseLimit);
    LPLL_BK_RESTORE;
}

void MDrv_Scaler_FastFrameLock(MS_U16 u16InputVFreq,MS_U16 u16OutputVFreq)
{
    MS_U16 u16temp;
    MS_BOOL bCnt;

    //when calling MApi_XC_SetPanelTiming, input source is disabled
    //MApi_XC_DisableInputSource(TRUE, MAIN_WINDOW);

    SC_W2BYTEMSK(REG_SC_BK0F_56_L, 0, BIT(0));
    SC_W2BYTEMSK(REG_SC_BK0F_56_L, 1, BIT(0));
    MDrv_SC_set_fpll(ENABLE, DISABLE, 0);//enable FPLL
    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "Enable Fast FPLL\r\n")

    u16temp = 0;
    bCnt = 0;

    while(1)
    {
        u16temp++;
        MsOS_DelayTask(1);
        //if(MDrv_ReadByte(H_BK_S_VOP(0x56)) & BIT(7)) // get vcnt freeze status
        if(SC_R2BYTE(REG_SC_BK0F_56_H) & BIT(7)) // get vcnt freeze status
        {
           //need wait vcnt down
           bCnt = 1 ;
           break;
        }
        else if(u16temp >((10000/u16OutputVFreq)+2)) // wait at lease one output vsync
        {
            SC_W2BYTEMSK(REG_SC_BK0F_56_L, 0x08, BIT(3));  //SW Reset vcnt freeze
            SC_W2BYTEMSK(REG_SC_BK0F_56_L, 0x00, BIT(3));
            break;
        }
    }

    u16temp = 0;
    if(bCnt)
    {
        //while(MDrv_ReadByte(H_BK_S_VOP(0x56)) & BIT(7))
        while(SC_R2BYTE(REG_SC_BK0F_56_H) & BIT(7))
        {
            u16temp++;
            MsOS_DelayTask(1);
            if(u16temp > ((10000/u16InputVFreq)+2))
            {
                //MDrv_WriteRegBit(L_BK_S_VOP(0x56), 1, BIT(3));  //SW Reset vcnt freeze
                //MDrv_WriteRegBit(L_BK_S_VOP(0x56), 0, BIT(3));
                SC_W2BYTEMSK(REG_SC_BK0F_56_L, 0x08, BIT(3));  //SW Reset vcnt freeze
                SC_W2BYTEMSK(REG_SC_BK0F_56_L, 0x00, BIT(3));
                break;
            }
        }
    }

    //MDrv_WriteRegBit(L_BK_S_VOP(0x56), 1, BIT(3));  //SW Reset vcnt freeze
    //MDrv_WriteRegBit(L_BK_S_VOP(0x56), 0, BIT(3));

    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "temp= %u\r\n", u16temp)
}

void MDrv_SC_Set_Output_Dclk_Slowly(MS_U32 u32PllSet, MS_U32 u32Steps, MS_U32 u32DelayMs, SCALER_WIN eWindow)
{
    //slowly move to target
    MS_U32 u32PLLStep, u32OrigLPLL = 0;
    MS_U8  i;

    LPLL_BK_STORE;
    LPLL_BK_SWITCH(0x00);
    u32OrigLPLL = MDrv_Read4Byte(L_BK_LPLL(0x0F));

    u32PllSet = u32PllSet & 0xFFFFFF;

    if (_bFpllCusFlag[E_FPLL_FLAG_INITSETSTEP] && _u32FpllCusData[E_FPLL_FLAG_INITSETSTEP])
    {
        u32Steps = _u32FpllCusData[E_FPLL_FLAG_INITSETSTEP];
    }
    if (_bFpllCusFlag[E_FPLL_FLAG_INITSETDELAY] && _u32FpllCusData[E_FPLL_FLAG_INITSETDELAY])
    {
        u32DelayMs = _u32FpllCusData[E_FPLL_FLAG_INITSETDELAY];
    }

    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "SetDCLK: Steps=%lu, DelayMs=%lu\r\n", u32Steps, u32DelayMs);

    u32PLLStep = abs(u32PllSet - u32OrigLPLL) / u32Steps;

    if(u32PLLStep > (0x50000/u32Steps))
    {
        for(i=1; i<(u32Steps+1); i++)
        {
            if(u32DelayMs != 0)
            {
                MDrv_XC_DelayAndWaitOutVsync(u32DelayMs, eWindow); // change output dclk when blanking
            }
            else
            {
                MDrv_XC_wait_output_vsync(2, 50, eWindow);        // change output dclk when blanking
            }
            if(u32PllSet > u32OrigLPLL)
            {
                //add
                MDrv_Write3Byte(L_BK_LPLL(0x0F), u32OrigLPLL + (u32PLLStep * i));
                XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "Slow steps add 0x%lx\r\n", u32OrigLPLL + (u32PLLStep * i));
            }
            else
            {
                //minus
                MDrv_Write3Byte(L_BK_LPLL(0x0F), u32OrigLPLL - (u32PLLStep * i));
                XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "Slow steps minus 0x%lx\r\n", u32OrigLPLL - (u32PLLStep * i));
            }
        }
    }
    else
    {
        // direct set
        MDrv_XC_wait_output_vsync(1, 50, eWindow);       // change output dclk when blanking
        MDrv_Write3Byte(L_BK_LPLL(0x0F), u32PllSet);
        XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "Direct set to 0x%lx\r\n", u32PllSet);
    }
    LPLL_BK_RESTORE;
}

//-------------------------------------------------------------------------------------------------
/// Get the time for scaler output one frame(Vertical Period)
/// @return @ref MS_U16 return output vertical Period(unit: ms)
//-------------------------------------------------------------------------------------------------
static MS_U16 _MDrv_SC_GetOutputVPeriod(void)
{
    MS_U16 u16VFreq = MDrv_SC_GetOutputVFreqX100();
    return ((100000+u16VFreq/2)/u16VFreq); //Period = 1000/(vfreq/100)
}

//-----------------------------------------------------------------------------
//  below functions are for FSM FPLL
//-----------------------------------------------------------------------------
static MS_U16 _MDrv_Scaler_GetAVDStatus(void)
{
    return Hal_SC_GetAVDStatus();
}

void MDrv_Scaler_GetLPLLPhaseOfs(MS_U32* pu32UpOfs, MS_BOOL *bDir)
{
    LPLL_BK_STORE;
    LPLL_BK_SWITCH(0x00);

    *pu32UpOfs   = (MS_U32)MDrv_Read2Byte(L_BK_LPLL(0x11));
    *bDir = MDrv_ReadRegBit(L_BK_LPLL(0x12), BIT(0));
    LPLL_BK_RESTORE;
}

void MDrv_Scaler_GetLPLLPrdOfs(MS_U32* pu32UpOfs, MS_BOOL *bDir)
{
    LPLL_BK_STORE;
    LPLL_BK_SWITCH(0x00);

    *pu32UpOfs   = (MS_U32)MDrv_Read2Byte(L_BK_LPLL(0x13));
    *bDir = MDrv_ReadRegBit(L_BK_LPLL(0x14), BIT(0));
    LPLL_BK_RESTORE;
}

//judge whether the fpll is done or not.
MS_BOOL MDrv_Scaler_GetFPLLDoneStatus(void)
{
    MS_U32 u32PhaseOfs,u32PrdOfs;
    MS_BOOL bPhaseDir,bPrdDir;

    MDrv_Scaler_GetLPLLPhaseOfs(&u32PhaseOfs,&bPhaseDir);
    MDrv_Scaler_GetLPLLPrdOfs(&u32PrdOfs,&bPrdDir);

    //printf("Framelock: (%lu,%u,%lu,%u)\r\n",u32PhaseOfs,bPhaseDir,u32PrdOfs,bPrdDir);
    if(MDrv_ReadRegBit(L_BK_LPLL(0x0C), BIT(3)) &&
       (u32PhaseOfs <= FPLL_PHASE_VAR_LIMIT) &&
       (u32PrdOfs <= FPLL_PRD_VAR_LIMIT))
    {
        //printf("OK.\r\n");
        return TRUE;
    }
    else
    {
        //printf("No.\r\n");
        return FALSE;
    }
}

MS_BOOL MDrv_Scaler_IsPRDLock(MS_U16 *u16OrigPrd)
{
    MS_U16 u16NewPrd = 0;
    MS_U16 u16NewPhase = 0;

    LPLL_BK_STORE;
    LPLL_BK_SWITCH(0x00);

    u16NewPrd = MDrv_Read2Byte(L_BK_LPLL(0x13));
    u16NewPhase = MDrv_Read2Byte(L_BK_LPLL(0x11));
    LPLL_BK_RESTORE;

    if( (u16NewPhase < (_bFpllCusFlag[E_FPLL_FLAG_PHASELIMIT] ? (_u32FpllCusData[E_FPLL_FLAG_PHASELIMIT]) : (PHASE_OFFSET_LIMIT)))
       && (u16NewPrd < FPLL_PRD_THRESHOLD) && (abs(*u16OrigPrd - u16NewPrd) < FPLL_PRD_VAR_LIMIT))
    {
        *u16OrigPrd = u16NewPrd;
        return TRUE;
    }
    else
    {
        *u16OrigPrd = u16NewPrd;
        return FALSE;
    }
}

MS_BOOL MDrv_Scaler_IsPhaseStable(MS_U16 *u16OrigPhase)
{
    MS_U16 u16NewPhase = 0;

    LPLL_BK_STORE;
    LPLL_BK_SWITCH(0x00);

    u16NewPhase = MDrv_Read2Byte(L_BK_LPLL(0x11));
    LPLL_BK_RESTORE;


    if( (abs(*u16OrigPhase - u16NewPhase) < FPLL_PHASE_VAR_LIMIT) &&
        (u16NewPhase < (_bFpllCusFlag[E_FPLL_FLAG_PHASELIMIT] ? (_u32FpllCusData[E_FPLL_FLAG_PHASELIMIT]) : (PHASE_OFFSET_LIMIT))) )
    {
       *u16OrigPhase = u16NewPhase;
        return TRUE;
    }
    else
    {
       *u16OrigPhase = u16NewPhase;
        return FALSE;
    }
}

void MDrv_Scaler_SetPhaseLock_Thresh(MS_U16 u16Thresh, MS_U8 u8Lpll_bank)
{
    LPLL_BK_STORE;
    if(u8Lpll_bank)
        LPLL_BK_SWITCH(0x01);
    else
        LPLL_BK_SWITCH(0x00);
     XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "u16Thresh= %u\r\n", u16Thresh);
    MDrv_Write2ByteMask( L_BK_LPLL(0x05), u16Thresh<<8, 0xF00);
    LPLL_BK_RESTORE;
}

static MS_U16 MDrv_SC_FPLL_GetMCNRDelayTime(void)
{
    MS_U16 u16DelayCount;
    if(!_bEnableMCNR)
    {
        return 0;
    }
    else
    {
        switch(gSrcInfo[MAIN_WINDOW].Status2.eFrameStoreNumber)//Get delay Vsync number first
        {
        case IMAGE_STORE_2_FIELDS:
        case IMAGE_STORE_2_FRAMES:
            u16DelayCount = 2; //For 2 frame/field mode
            break;
        case IMAGE_STORE_4_FIELDS:
            u16DelayCount = 4;
            break;
        case IMAGE_STORE_6_FIELDS:
            u16DelayCount = 6;
            break;
        case IMAGE_STORE_8_FIELDS:
            u16DelayCount = 8;
            break;
        case IMAGE_STORE_10_FIELDS:
            u16DelayCount = 10;
            break;
        case IMAGE_STORE_12_FIELDS:
            u16DelayCount = 12;
            break;
        case IMAGE_STORE_14_FIELDS:
            u16DelayCount = 14;
            break;
        case IMAGE_STORE_16_FIELDS:
            u16DelayCount = 16;
            break;
        case IMAGE_STORE_18_FIELDS:
            u16DelayCount = 18;
            break;
        case IMAGE_STORE_20_FIELDS:
            u16DelayCount = 20;
            break;
        case IMAGE_STORE_3_FRAMES:
            u16DelayCount = 3;
            break;
        case IMAGE_STORE_4_FRAMES:
            u16DelayCount = 4;
            break;
        case IMAGE_STORE_5_FRAMES:
            u16DelayCount = 5;
            break;
        case IMAGE_STORE_6_FRAMES:
            u16DelayCount = 6;
            break;
        case IMAGE_STORE_7_FRAMES:
            u16DelayCount = 7;
            break;
        case IMAGE_STORE_8_FRAMES:
            u16DelayCount = 8;
            break;
        case IMAGE_STORE_10_FRAMES:
            u16DelayCount = 10;
            break;
        case IMAGE_STORE_12_FRAMES:
            u16DelayCount = 12;
            break;
        default:
            u16DelayCount = 0;
            break;
        }
        u16DelayCount *= _MDrv_SC_GetOutputVPeriod(); //get delay time
        XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "MCNR Enable Delay=%u\r\n", u16DelayCount)
        return u16DelayCount;
    }
}

MS_BOOL  MApi_XC_FrameLockCheck(void)
{
        //check  PRD Lock        // check  Phase Stable  ->check Frame lock
    if(u32OutputPhaseStable  &&  u32OutputPRDLock)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*
   Frame lock mechanism should be detected here.
   msAPI_Scaler_FPLL_FSM wait to lock frame only if the
   phase error is small enough.
*/
static MS_U32 s_u32DelayT = 0;
static MS_U32 s_u32InputVsyncStart = 0;

static void _MApi_XC_FPLL_FSM_ISR(SC_INT_SRC eIntNum, void * pParam)
{
    static MS_U16 s_u16OrigPrd = FPLL_PRD_MAX;         // max PRD error
    static MS_U16 s_u16OrigPhase = 0xFFFF;             // max phase error
    static MS_U32 s_u32FSMTimer = 0;
    MS_U32 u32Ofs;
    MS_BOOL Dir ;
    MS_U16 u16PhaseLimit;
    MS_U8 u8Bank;
    UNUSED(eIntNum); UNUSED(pParam);

    //printf("FSM looping _enFpllFsm = %x\r\n", _enFpllFsm);
    switch (_enFpllFsm)
    {
    case FPLL_IDLE_STATE:
        // no signal, do nothing
        break;

    case FPLL_INIT_STATE://dtv special
        //wait 2 input vsync for framelock phase error stable
        //according our test of 24P video, after enble mvop, need at least 2 input vsync, framelock
        //phase error can be stable.
        u32OutputPhaseStable = FALSE;
        u32OutputPRDLock     = FALSE;

        if((MsOS_GetSystemTime() - s_u32InputVsyncStart) < s_u32DelayT)
            break;

        XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING_ISR, "[FPLL INIT->PRD]\r\n")
        s_u16OrigPrd = FPLL_PRD_MAX;
        _enFpllFsm = FPLL_WAIT_PRD_LOCK_STATE;
        s_u32FSMTimer = MsOS_GetSystemTime();
        _u32MCNRTimer = s_u32FSMTimer;//For MCNR enable counter
        //InISR, we can not wait XC entry, because interrupt is higher than xc mutex
        u8Bank = MDrv_ReadByte(BK_SELECT_00);
        Hal_SC_SetOPWriteOff(ENABLE);
        MDrv_WriteByte(BK_SELECT_00, u8Bank);
        _bNeedReloadMCNR = TRUE;
        break;
    case FPLL_ATV_INIT_STATE://atv special
        u32OutputPhaseStable = FALSE;
        u32OutputPRDLock     = FALSE;
        s_u32FSMTimer = MsOS_GetSystemTime();
        _enFpllFsm = FPLL_WAIT_IP_STABLE_STATE;
        break;

    case FPLL_WAIT_IP_STABLE_STATE://atv special
        if (_MDrv_Scaler_GetAVDStatus() & VD_HSYNC_LOCKED)
        {
            _enFpllFsm = FPLL_WAIT_LPLL_OFS_STATE;
            u8Bank = MDrv_ReadByte(BK_SELECT_00);
            Hal_SC_SetOPWriteOff(ENABLE);
            MDrv_WriteByte(BK_SELECT_00, u8Bank);
            _u32MCNRTimer = MsOS_GetSystemTime();
            _bNeedReloadMCNR = TRUE;
            XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING_ISR, "[WAIT IP STABLE] -> [WAIT LPLL OFS]\r\n")
        }
        else if(FPLL_FSM_STABLE_TIME < (MsOS_GetSystemTime() - s_u32FSMTimer))
        {
            _enFpllFsm = FPLL_DEATTACH_ISR_STATE;      // force re-start
        }

        break;

    case FPLL_WAIT_LPLL_OFS_STATE://atv special
        // We set PHASE_OFFSET_LIMIT to "limit for lpll phase offset", the "phase dif value" will take
        // PHASE_OFFSET_LIMIT+1 as its max value. then we set the gate to PHASE_OFFSET_LIMIT,
        // every case the "phase dif value" is smaller or equal to the PHASE_OFFSET_LIMIT,
        // we consider the phase is changed to a level, then we can do the adjustment.
        MDrv_Scaler_GetLPLLPhaseOfs(&u32Ofs,&Dir);

        u16PhaseLimit = _bFpllCusFlag[E_FPLL_FLAG_PHASELIMIT] ? (MS_U16)_u32FpllCusData[E_FPLL_FLAG_PHASELIMIT] : (MS_U16)PHASE_OFFSET_LIMIT;
        if (u32Ofs <= u16PhaseLimit)
        {
            MDrv_SC_set_fpll(ENABLE, DISABLE, 0);
            _enFpllFsm = FPLL_VD_LOCKED_STATE;
            XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING_ISR, "Phase offs go into limit~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\r\n")
            XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING_ISR, "[WAIT LPLL OFS] -> [VD LOCKED]\r\n")
        }
        else if(FPLL_FSM_STABLE_TIME < (MsOS_GetSystemTime() - s_u32FSMTimer))
        {
            _enFpllFsm = FPLL_DEATTACH_ISR_STATE;      // force re-start
        }

        XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING_ISR, "===> Phase Diff=0x%lx, Limit=0x%x\r\n", u32Ofs, u16PhaseLimit)
        break;

    case FPLL_VD_LOCKED_STATE://atv special
        {
            u32OutputPhaseStable  = TRUE;
            u32OutputPRDLock      = TRUE;
            if(_bNeedReloadMCNR && _bEnableMCNR && !_bForceDisableMCNR)
            {
                //Wait some vsync, then turn on MCNR
                if(MDrv_SC_FPLL_GetMCNRDelayTime() < (MsOS_GetSystemTime() - _u32MCNRTimer))
                {
                    u8Bank = MDrv_ReadByte(BK_SELECT_00);
                    Hal_SC_SetOPWriteOff(!_bEnableMCNR);
                    MDrv_WriteByte(BK_SELECT_00, u8Bank);
                    _bNeedReloadMCNR = FALSE;
                    _enFpllFsm = FPLL_DEATTACH_ISR_STATE;
                }
            }
            else
            {
                _enFpllFsm = FPLL_DEATTACH_ISR_STATE;
            }
        }
        break;

    case FPLL_WAIT_PRD_LOCK_STATE://Dtv special
        if(MDrv_Scaler_IsPRDLock(&s_u16OrigPrd))
        {
            XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING_ISR, "[FPLL PRD locked->Phase]\r\n")
            s_u16OrigPhase = FPLL_PHASE_MAX;
            s_u32FSMTimer = MsOS_GetSystemTime();
            _enFpllFsm = FPLL_WAIT_PHASE_STABLE_STATE;
        }
        else
        {
            if(FPLL_FSM_STABLE_TIME < (MsOS_GetSystemTime() - s_u32FSMTimer))
            {
                _enFpllFsm = FPLL_DEATTACH_ISR_STATE;      // force re-start
            }
        }
        XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING_ISR, "PRD: phase %04x, prd %04x\r\n", MDrv_Read2Byte(L_BK_LPLL(0x11)), MDrv_Read2Byte(L_BK_LPLL(0x13)))
        break;

    case FPLL_WAIT_PHASE_STABLE_STATE://Dtv special
        if(MDrv_Scaler_IsPhaseStable(&s_u16OrigPhase))
        {

            if(MDrv_Scaler_IsPRDLock(&s_u16OrigPrd)) // read current PRD again
            {
                XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING_ISR, "[FPLL phase->check Frame lock]\r\n")
            }

            MDrv_SC_set_fpll(ENABLE, DISABLE, 0);

            XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING_ISR, "[FPLL phase->check Frame lock]\r\n")

            s_u32FSMTimer = MsOS_GetSystemTime();
            _enFpllFsm = FPLL_CHECK_FRAME_LOCK_STATE;
        }
        else
        {
            if(FPLL_FSM_STABLE_TIME < (MsOS_GetSystemTime() - s_u32FSMTimer))
            {
                _enFpllFsm = FPLL_DEATTACH_ISR_STATE;      // force re-start
            }
        }
        XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING_ISR, "PHS: phase %04x, prd %04x\r\n", MDrv_Read2Byte(L_BK_LPLL(0x11)), MDrv_Read2Byte(L_BK_LPLL(0x13)))
        break;

    case FPLL_CHECK_FRAME_LOCK_STATE://Dtv special
        {

            //MDrv_Scaler_IsPRDLock(&s_u16OrigPrd);       // read current PRD again
            //MDrv_Scaler_IsPhaseStable(&s_u16OrigPhase); // read current phase again
            if(MDrv_Scaler_IsPRDLock(&s_u16OrigPrd))
            {
                u32OutputPRDLock = TRUE;
                XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING_ISR, "[FPLL PRD Lock->check Frame lock]\r\n")
            }
            if(MDrv_Scaler_IsPhaseStable(&s_u16OrigPhase))
            {
                u32OutputPhaseStable = TRUE;
                XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING_ISR, "[FPLL phase Lock->check Frame lock]\r\n")
            }
            LPLL_BK_STORE;
            LPLL_BK_SWITCH(0x00);
            if((MDrv_Read2Byte(L_BK_LPLL(0x11)) > FPLL_PHASE_VAR_LIMIT) || (MDrv_Read2Byte(L_BK_LPLL(0x13)) > FPLL_PRD_VAR_LIMIT))
            {
                XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING_ISR, "CHK: phase %04x, prd %04x\r\n", MDrv_Read2Byte(L_BK_LPLL(0x11)), MDrv_Read2Byte(L_BK_LPLL(0x13)))
                if(FPLL_FSM_STABLE_TIME < (MsOS_GetSystemTime() - s_u32FSMTimer))
                {
                    _enFpllFsm = FPLL_DEATTACH_ISR_STATE;      // force re-start
                    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING_ISR, "CHK Stable count is outof range, force re-start.\r\n")
                }
            }
            else
            {
                _enFpllFsm = FPLL_LOCKED_STATE;
                XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING_ISR, "Locked, the using SET is %lx\r\n", MDrv_Read4Byte(L_BK_LPLL(0x28)))
            }
            LPLL_BK_RESTORE;
        }
        break;

    case FPLL_LOCKED_STATE://Dtv special
        {
            if(_bNeedReloadMCNR && _bEnableMCNR && !_bForceDisableMCNR)
            {
                //Wait some vsync, then turn on MCNR
                if(MDrv_SC_FPLL_GetMCNRDelayTime() < (MsOS_GetSystemTime() - _u32MCNRTimer))
                {
                    u8Bank = MDrv_ReadByte(BK_SELECT_00);
                    Hal_SC_SetOPWriteOff(!_bEnableMCNR);
                    MDrv_WriteByte(BK_SELECT_00, u8Bank);
                    _bNeedReloadMCNR = FALSE;
                    _enFpllFsm = FPLL_DEATTACH_ISR_STATE;
                }
            }
            else
            {
                _enFpllFsm = FPLL_DEATTACH_ISR_STATE;
            }
         }
         break;

    case FPLL_DEATTACH_ISR_STATE:
        {
            //switch bank back
            //MS_BOOL bret;
            //u8Bank = MDrv_ReadByte(BK_SELECT_00);
            //bret = MDrv_XC_InterruptDeAttachWithoutMutex(SC_INT_VSINT, _MApi_XC_FPLL_FSM_ISR, (void *) &bFPLL_FSM);
            //MDrv_WriteByte(BK_SELECT_00, u8Bank);
            // If we cannot lock, the bank31_0c will be 0x48, we clear it to 0x08,
            // which is the same with the Non-FSM case.

           LPLL_BK_STORE;
            LPLL_BK_SWITCH(0x00);
            MDrv_WriteRegBit(L_BK_LPLL(0x0C), FALSE, BIT(6));
            LPLL_BK_RESTORE;
            _enFpllFsm = FPLL_IDLE_STATE;
            XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING_ISR, "FSM ISR: de-attached ISR result=%u, and reset FSM to Idle\r\n",
                MDrv_XC_InterruptDeAttachWithoutMutex(SC_INT_VSINT, _MApi_XC_FPLL_FSM_ISR, (void *) &bFPLL_FSM));
        }
        break;

    default:
        break;
    }
}

/*
   Frame lock mechanism should be detected here.
   msAPI_Scaler_FPLL_FSM wait to lock frame only if the
   phase error is small enough.
*/
void msAPI_Scaler_FPLL_FSM(SCALER_WIN eWindow)
{
    // use _MApi_XC_FPLL_FSM_ISR() now
    UNUSED(eWindow);

    return;
}

void MApi_XC_SetPanelTiming_FSM(XC_SetTiming_Info *pTimingInfo, SCALER_WIN eWindow)
{
    //MS_U8   u8Bank;
    MS_U8   u8FRC_In, u8FRC_Out;

    XC_DISPLAYOUT_INFO eDisplyout_Info;
    eDisplyout_Info.u16Htt_out = g_XC_InitData.stPanelInfo.u16HTotal;
    eDisplyout_Info.u16Vtt_out = g_XC_InitData.stPanelInfo.u16VTotal;


    //calculate the time of 2 input vsync
   if(pTimingInfo->u16InputVFreq > 0)
        s_u32DelayT = 20000 / pTimingInfo->u16InputVFreq + 20;
   else
        s_u32DelayT = 1;

    MDrv_SC_set_fpll(DISABLE, ENABLE, 0);
    Hal_SC_SetOPWriteOff(ENABLE);//Disable MCNR first, Enable MCNR when FPLL done
    if(!g_pSContext->bEnableFPLLManualDebug)
    {
        MDrv_Scaler_SetPhaseLimit(0);
    }

    //MDrv_WriteByte(BK_SELECT_00, REG_BANK_S_VOP);
    if (!pTimingInfo->bMVOPSrc)
    {
        // according to customer side test experience, ATV disable framelock point,
        // could get better result.
        //MDrv_WriteRegBit(L_BK_S_VOP(0x56), 0, BIT(1));
        SC_W2BYTEMSK(REG_SC_BK0F_56_L, 0x00 ,BIT(1));
    }
    else
    {
        //MDrv_WriteRegBit(L_BK_S_VOP(0x56), 1, BIT(1));  //Using new ovs_ref
        SC_W2BYTEMSK(REG_SC_BK0F_56_L, 0x02 ,BIT(1));
    }

    gSrcInfo[eWindow].Status2.u16OutputVFreqAfterFRC = MDrv_SC_Cal_FRC_Output_Vfreq(pTimingInfo->u16InputVFreq,
                                              g_XC_InitData.stPanelInfo.u16DefaultVFreq,
                                              gSrcInfo[eWindow].bFBL,
                                              &u8FRC_In,
                                              &u8FRC_Out,
                                              &gSrcInfo[eWindow].bEnableFPLL,
                                              0,
                                              eWindow);
    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "FSM: _u16OutputVFreqAfterFRC = %u\r\n", gSrcInfo[eWindow].Status2.u16OutputVFreqAfterFRC );

    if (gSrcInfo[eWindow].bFBL)
    {
        //we don't use FSM flow to do FBL case
        MDrv_XC_SetFreeRunTiming();
        XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "---Now it's FBL case, FSM FPLL cannot do it. Just let it freerun.---\r\n")
    }

    MDrv_SC_set_Htt_Vtt(pTimingInfo, &gSrcInfo[eWindow].Status2.u16OutputVFreqAfterFRC, &eDisplyout_Info, eWindow);
    MDrv_SC_Set_LockFreeze_Point(pTimingInfo, eDisplyout_Info.u16Vtt_out, eWindow);

    MDrv_SC_set_output_dclk(eDisplyout_Info.u16Htt_out,
                            eDisplyout_Info.u16Vtt_out,
                            gSrcInfo[eWindow].Status2.u16OutputVFreqAfterFRC, eWindow);

    MDrv_Scaler_SetIGainPGain(u8FRC_Out, gSrcInfo[eWindow].Status2.u16OutputVFreqAfterFRC);

#ifdef FA_1920X540_OUTPUT
    if((!g_XC_Pnl_Misc.FRCInfo.bFRC)&&
      ((MApi_XC_Get_3D_Input_Mode(MAIN_WINDOW) == E_XC_3D_INPUT_SIDE_BY_SIDE_HALF)||
       (MApi_XC_Get_3D_Input_Mode(MAIN_WINDOW) == E_XC_3D_INPUT_TOP_BOTTOM)||
       (MApi_XC_Get_3D_Input_Mode(MAIN_WINDOW) == E_XC_3D_INPUT_FIELD_ALTERNATIVE))&&
      (MApi_XC_Get_3D_Output_Mode() == E_XC_3D_OUTPUT_FRAME_ALTERNATIVE))
    {
        SC_W2BYTEMSK(REG_SC_BK10_0D_L, (g_XC_InitData.stPanelInfo.u16DefaultVTotal/2), 0x0FFF);
        XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "###g_XC_InitData.stPanelInfo.u16DefaultVTotal=%x\r\n",g_XC_InitData.stPanelInfo.u16DefaultVTotal/2);
    }
#endif
#ifdef TBP_1920X2160_OUTPUT
    if((!g_XC_Pnl_Misc.FRCInfo.bFRC)&&
        ((gSrcInfo[MAIN_WINDOW].stCapWin.height == DOUBLEHD_1080X2P_VSIZE)||
        (gSrcInfo[MAIN_WINDOW].stCapWin.height == DOUBLEHD_1080X2I_VSIZE))&&
        (MApi_XC_Get_3D_Input_Mode(MAIN_WINDOW) == E_XC_3D_INPUT_FRAME_PACKING)&&
        (MApi_XC_Get_3D_Output_Mode() == E_XC_3D_OUTPUT_TOP_BOTTOM))
    {
        SC_W2BYTEMSK(REG_SC_BK10_0D_L, (g_XC_InitData.stPanelInfo.u16DefaultVTotal*2), 0x0FFF);
        XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "###g_XC_InitData.stPanelInfo.u16DefaultVTotal=%x\r\n",g_XC_InitData.stPanelInfo.u16DefaultVTotal*2);
    }
#endif


    /* enable output free-run */
    SC_W2BYTEMSK(REG_SC_BK10_1C_L, BIT(8), BIT(8));  //output free run

    /* Enable/Disable FrameLock */
    SC_W2BYTEMSK(REG_SC_BK01_03_L, (gSrcInfo[eWindow].bEnableFPLL)<<15, BIT(15));

    MDrv_SC_set_fpll(gSrcInfo[eWindow].bEnableFPLL, gSrcInfo[eWindow].bEnableFPLL, 0);
    s_u32InputVsyncStart = MsOS_GetSystemTime();

    if(gSrcInfo[eWindow].bEnableFPLL)
    {
        if(IsSrcTypeDigitalVD(gSrcInfo[eWindow].enInputSourceType))
        {
            _enFpllFsm = FPLL_ATV_INIT_STATE;
        }
        else
        {
            _enFpllFsm = FPLL_INIT_STATE;
        }
    }
    else
    {
        _bNeedReloadMCNR = FALSE;
        _enFpllFsm = FPLL_IDLE_STATE;
        XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "FPLL disabled, reset FSM to Idle\r\n")
    }
}

//////////////////////////////////////////////////////----------------

void MApi_XC_SetPanelTiming_NonFSM(XC_SetTiming_Info *pTimingInfo, SCALER_WIN eWindow)
{
    //MS_U8   u8Bank;
    MS_U8   u8FRC_In, u8FRC_Out;
    XC_DISPLAYOUT_INFO eDisplyout_Info;
    eDisplyout_Info.u16Htt_out = g_XC_InitData.stPanelInfo.u16HTotal;
    eDisplyout_Info.u16Vtt_out = g_XC_InitData.stPanelInfo.u16VTotal;

    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "bFBL=%u, bMVOPSrc=%u, InputVFreq=%u, InputVtt=%u, bInterlace=%u bFastFL=%u\r\n",
        gSrcInfo[eWindow].bFBL,
        pTimingInfo->bMVOPSrc,
        pTimingInfo->u16InputVFreq,
        pTimingInfo->u16InputVTotal,
        pTimingInfo->bInterlace,
        pTimingInfo->bFastFrameLock)
    Hal_SC_SetOPWriteOff(ENABLE);//Disable MCNR first, Enable MCNR when FPLL done
    gSrcInfo[eWindow].bFastFrameLock = pTimingInfo->bFastFrameLock;

    //u8Bank = MDrv_ReadByte(BK_SELECT_00);           // backup first
    MDrv_SC_set_fpll(DISABLE, ENABLE, 0);    // disable FPLL

    MDrv_Scaler_SetPhaseLimit(0);

    //MDrv_WriteByte(H_BK_LPLL(0x05), 0xF0);
    //MDrv_WriteByte(L_BK_LPLL(0x05), 0x22);

#if VERIFY_DISPLAY_FPGA
    MDrv_Write3Byte(L_BK_LPLL(0x06), 0x30000);
    MDrv_Write3Byte(L_BK_LPLL(0x08), 0x30000);
    MDrv_Write2Byte(L_BK_LPLL(0x0A), 0xFFF0);  // limit for lpll phase limit offset; set it to speed up frame lock

    u8FRC_In = 2;
    u8FRC_Out = 5;
    gSrcInfo[eWindow].Status2.u16OutputVFreqAfterFRC = pTimingInfo->u16InputVFreq * u8FRC_Out/u8FRC_In;

    gSrcInfo[eWindow].bEnableFPLL = TRUE;

    if(!gSrcInfo[eWindow].bFBL )
    {
        MDrv_WriteByte(H_BK_LPLL(0x0C), ((u8FRC_Out-1) << 4) | (u8FRC_In-1));
    }
    else
    {
        // fps_in:fps_out = 1:1  for FBL, only main can use FBL
        MDrv_WriteByte(H_BK_LPLL(0x0C), 0x00);
    }
#else
    gSrcInfo[eWindow].Status2.u16OutputVFreqAfterFRC = MDrv_SC_Cal_FRC_Output_Vfreq(pTimingInfo->u16InputVFreq,
                                              g_XC_InitData.stPanelInfo.u16DefaultVFreq,
                                              gSrcInfo[eWindow].bFBL,
                                              &u8FRC_In,
                                              &u8FRC_Out,
                                              &gSrcInfo[eWindow].bEnableFPLL,
                                              0,
                                              eWindow);
#endif
    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "NON-FSM: _u16OutputVFreqAfterFRC = %u\r\n", gSrcInfo[eWindow].Status2.u16OutputVFreqAfterFRC)
    gSrcInfo[eWindow].bDoneFPLL = !gSrcInfo[eWindow].bEnableFPLL;       // if enable FPLL, then not done fpll yet

    MDrv_SC_set_Htt_Vtt(pTimingInfo, &gSrcInfo[eWindow].Status2.u16OutputVFreqAfterFRC, &eDisplyout_Info, eWindow);
    MDrv_SC_Set_LockFreeze_Point(pTimingInfo, g_XC_InitData.stPanelInfo.u16VTotal, eWindow);

    MDrv_SC_set_output_dclk(g_XC_InitData.stPanelInfo.u16HTotal,
                            g_XC_InitData.stPanelInfo.u16VTotal,
                            gSrcInfo[eWindow].Status2.u16OutputVFreqAfterFRC, eWindow);

    MDrv_Scaler_SetIGainPGain(u8FRC_Out, gSrcInfo[eWindow].Status2.u16OutputVFreqAfterFRC);
    //MDrv_WriteByte(BK_SELECT_00, REG_BANK_VOP);
    //MDrv_WriteRegBit(H_BK_VOP(0x1C), 1, BIT(0));  //output free run
    SC_W2BYTEMSK(REG_SC_BK10_1C_L, BIT(8), BIT(8));  //output free run

    if (gSrcInfo[eWindow].bEnableFPLL)
    {
        //MDrv_WriteByte(BK_SELECT_00, REG_BANK_IP1F2);
        //MDrv_WriteRegBit(H_BK_IP1F2(0x03), 1, BIT(7));  //enable frame lock
        SC_W2BYTEMSK(REG_SC_BK01_03_L, BIT(15), BIT(15));  //enable frame lock

        if (gSrcInfo[eWindow].bFastFrameLock) //MDrv_ReadRegBit(H_BK_LPLL(0x1E), 0x10)
        {
            MDrv_Scaler_FastFrameLock(pTimingInfo->u16InputVFreq, gSrcInfo[eWindow].Status2.u16OutputVFreqAfterFRC);
            //MDrv_XC_DisableInputSource(FALSE, MAIN_WINDOW);  //enable Input
        }
        else
        {
            //MDrv_XC_DisableInputSource(FALSE, MAIN_WINDOW);  //enable Input
            HAL_SC_EnableFPLL();
        }
        _u32MCNRTimer = MsOS_GetSystemTime();
        _bNeedReloadMCNR = TRUE;
    }
    else
    {
        _bNeedReloadMCNR = FALSE;
    }

    #if 0
    u16temp = 0;
    while( !(MDrv_ReadByte(H_BK_LPLL(0x2A)) & BIT0) &&  u16temp<3000 )
    {
        u16temp++;
        MsOS_DelayTask(1);
    }
    printf("[FPLL Speed]Time= %d\r\n", msAPI_Timer_DiffTimeFromNow(u32TestTimer));
    printf("[FPLL Speed]temp= %d\r\n", u16temp);
    #endif

    #if 0
    for(u16temp=0; u16temp<20; u16temp++)
    {
        printf("\r\n%d\r\n", u16temp);
        printf("[Freeze]Phase dif= 0x%X (%u)\r\n", MDrv_Read2Byte(L_BK_LPLL(0x11)), MDrv_Read2Byte(L_BK_LPLL(0x11)));
        printf("[Freeze]Phase up= 0x%X (%u)\r\n", MDrv_ReadByte(L_BK_LPLL(0x12)), MDrv_ReadByte(L_BK_LPLL(0x12)));
        printf("[Freeze]SET using= 0x%X (%u)\r\n", MDrv_Read2Byte(L_BK_LPLL(0x28))|(MDrv_ReadByte(L_BK_LPLL(0x29))<<16),
                                                 MDrv_Read2Byte(L_BK_LPLL(0x28))|(MDrv_ReadByte(L_BK_LPLL(0x29))<<16));
        MsOS_DelayTask((10000/pTimingInfo->u16InputVFreq));
    }
    #endif
}

#if FRC_INSIDE
void MApi_XC_SetFLock(XC_SetTiming_Info *pTimingInfo, E_XC_FLOCK_TYPE eFlock_type, SCALER_WIN eWindow)
{
    MS_U8  u8FRC_In, u8FRC_Out;
    MS_U16 u16DefaultVFreq = g_XC_InitData.stPanelInfo.u16DefaultVFreq/2;
    MS_U16 u16InputVFreq = pTimingInfo->u16InputVFreq;
    MS_U16 u16OutputVfreqAfterFRC = 0;

    MS_U32 u32DClk;
    MS_U32 u32LpllLimitLow=0x10000;
    MS_U32 u32LpllLimitHigh=0x10000;
    MS_BOOL bEnableFPLL;
    MS_U16 u16HTotal = g_XC_InitData.stPanelInfo.u16HTotal;
    MS_U16 u16VTotal = g_XC_InitData.stPanelInfo.u16VTotal;

    if(MApi_XC_Get_3D_Output_Mode() == E_XC_3D_OUTPUT_FRAME_ALTERNATIVE)
    {
        if(g_XC_Pnl_Misc.FRCInfo.u83D_FI_out == E_XC_3D_OUTPUT_FI_960x1080)
        {
            u16HTotal /= 2;
            u16InputVFreq *=2 ;
        }
        else if(g_XC_Pnl_Misc.FRCInfo.u83D_FI_out == E_XC_3D_OUTPUT_FI_1920x540)
        {
            u16VTotal = u16VTotal/2;
            u16InputVFreq *=2 ;
        }
        else if(g_XC_Pnl_Misc.FRCInfo.u83D_FI_out == E_XC_3D_OUTPUT_FI_1280x720)
        {
            u16InputVFreq *=2 ;
        }
    }
    MDrv_SC_set_fpll(DISABLE, ENABLE, 1);
    MDrv_Scaler_SetPhaseLimit(1);

    u16OutputVfreqAfterFRC = MDrv_SC_Cal_FRC_Output_Vfreq(u16InputVFreq,
                                              u16DefaultVFreq,
                                              gSrcInfo[eWindow].bFBL,
                                              &u8FRC_In,
                                              &u8FRC_Out,
                                              &bEnableFPLL,
                                              1,
                                              eWindow);
    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "@@========\r\n")
    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "SetFlock: u16InputVFreq = %u\r\n,u16OutputVfreqAfterFRC = %u\r\n",
                                                   u16InputVFreq, u16OutputVfreqAfterFRC)
    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "@@========\r\n")



    if(MApi_XC_MLoad_GetStatus(eWindow) == E_MLOAD_ENABLED)
    {
        MApi_XC_MLoad_WriteCmd_And_Fire(REG_SC_BK10_0D_L,(u16VTotal - 1)&0xFFF, 0xFFF  );
    }
    else
    {
        SC_W2BYTE(REG_SC_BK10_0D_L,(u16VTotal - 1)&0xFFF);
    }
    if(MApi_XC_MLoad_GetStatus(eWindow) == E_MLOAD_ENABLED)
    {
        MApi_XC_MLoad_WriteCmd_And_Fire(REG_SC_BK10_0C_L,(u16HTotal - 1), 0xFFF  );
    }
    else
    {
        SC_W2BYTE(REG_SC_BK10_0C_L,(u16HTotal - 1));
    }

    u32DClk = u16HTotal * u16VTotal * u16OutputVfreqAfterFRC;
    if((u16InputVFreq >= 235 )&&(u16InputVFreq <= 245 ))
        MDrv_Scaler_SetPhaseLock_Thresh(0x0F, 1);
    else
        MDrv_Scaler_SetPhaseLock_Thresh(0x02, 1);

#if 0
    //Change Htt/Vtt for SBS/TBP in
    SC_W2BYTEMSK(REG_SC_BK10_0C_L, u16HTotal, 0xFFF);
    SC_W2BYTEMSK(REG_SC_BK10_0D_L, u16VTotal, 0xFFF);
#endif
    _MHal_SC_Flock_Caculate_LPLLSet(u32DClk);
    _MHal_SC_Flock_Set_IGainPGain( u8FRC_Out, u16OutputVfreqAfterFRC, u16HTotal, u16VTotal);
    //_MHal_SC_Flock_Set_LPLL_Enable(eFlock_type);
    _MHal_SC_Set_LPLL_Limit(u32LpllLimitHigh, u32LpllLimitLow, 1);
    MDrv_SC_set_fpll(bEnableFPLL, !bEnableFPLL, 1);

     XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "Input Freq = %u, After FRC = %u\r\n\r\n", u16InputVFreq, u16OutputVfreqAfterFRC);

    if(g_XC_Pnl_Misc.FRCInfo.u83D_FI_out != E_XC_3D_OUTPUT_FI_MODE_NONE)
    {
        Hal_XC_FRC_R2_Set_Timing(u16OutputVfreqAfterFRC/10, gSrcInfo[MAIN_WINDOW].Status2.u16OutputVFreqAfterFRC/10);
    }
    else
    {
        Hal_XC_FRC_R2_Set_Timing(u16OutputVfreqAfterFRC/10, gSrcInfo[MAIN_WINDOW].Status2.u16OutputVFreqAfterFRC/10);
    }
// set R2 timing should be before the set 3D mode, wait R2 refactoring
#if 0
{
//    E_XC_3D_INPUT_MODE e3dInputMode = MApi_XC_Get_3D_Input_Mode(eWindow);
    E_XC_3D_OUTPUT_MODE e3dOutputMode = MApi_XC_Get_3D_Output_Mode();
    MS_U16 u16R2_Hsize = 1920;
    MS_U16 u16R2_Vsize = 1080;

    switch(e3dOutputMode)
    {
        case E_XC_3D_OUTPUT_MODE_NONE:
        {
            Hal_XC_FRC_R2_Set_InputFrameSize(u16R2_Hsize, u16R2_Vsize);
            Hal_XC_FRC_R2_Set_Input3DFormat(E_FRC_INPUT_2D, E_FRC_3D_CFG_NORMAL, DISABLE, 0x80);
        }
        break;
        case E_XC_3D_OUTPUT_FRAME_ALTERNATIVE:
        {
            switch(g_XC_Pnl_Misc.FRCInfo.u83D_FI_out)
            {
                default:
                case E_XC_3D_OUTPUT_FI_1920x1080:
                    u16R2_Hsize = 1920;
                    u16R2_Vsize = 1080;
                break;
                case E_XC_3D_OUTPUT_FI_960x1080:

                    u16R2_Hsize = 960;
                    u16R2_Vsize = 1080;
                break;
                case E_XC_3D_OUTPUT_FI_1920x540:
                    u16R2_Hsize = 1920;
                    u16R2_Vsize = 540;
                break;
            }
            if(g_XC_Pnl_Misc.FRCInfo.bFRC)
            {
                Hal_XC_FRC_R2_Set_InputFrameSize(u16R2_Hsize, u16R2_Vsize);
                Hal_XC_FRC_R2_Set_Input3DFormat(E_FRC_INPUT_3D_FI_1080p, E_FRC_3D_CFG_NORMAL, DISABLE, 0x80);
            }
        }
        break;

        default:
        case E_XC_3D_OUTPUT_TOP_BOTTOM:
        {
            Hal_XC_FRC_R2_Set_InputFrameSize(u16R2_Hsize, u16R2_Vsize);
            Hal_XC_FRC_R2_Set_Input3DFormat(E_FRC_INPUT_3D_TD, E_FRC_3D_CFG_NORMAL, DISABLE, 0x80);
        }
        break;
    }
}
#endif
}

#endif

//-------------------------------------------------------------------------------------------------
/// Set the FPLL mode:
/// @param  bTrue      \b IN: True: FSM mode, False: Non FSM mode
//-------------------------------------------------------------------------------------------------

void MApi_SC_Enable_FPLL_FSM(MS_BOOL bTrue)
{
    _bFSM_FPLL_Setting = bTrue;
}

//-------------------------------------------------------------------------------------------------
/// Setup panel timing (output DCLK)/FRC/FramePLL, and enable input source
/// @param  pTimingInfo      \b IN: the information of the timing to set @ref XC_SetTiming_Info
//-------------------------------------------------------------------------------------------------
void MApi_XC_SetPanelTiming(XC_SetTiming_Info *pTimingInfo, SCALER_WIN eWindow)
{
    _XC_ENTRY();

    if((_bFSM_FPLL_Setting) && (!gSrcInfo[eWindow].bFBL) && (!gSrcInfo[eWindow].bR_FBL))
    {
        _enFpllFsm = FPLL_IDLE_STATE; //FSM will restart, so clear FSM status to idle
        _bFSM_FPLL_Working = TRUE;
        MApi_XC_SetPanelTiming_FSM(pTimingInfo, eWindow);
        if(gSrcInfo[eWindow].bEnableFPLL)
        {
            //FSM enable fpll, then enable ISR
            if(FALSE == MDrv_XC_InterruptIsAttached(SC_INT_VSINT, _MApi_XC_FPLL_FSM_ISR, (void *) &bFPLL_FSM))
            {
                MDrv_XC_InterruptAttachWithoutMutex(SC_INT_VSINT, _MApi_XC_FPLL_FSM_ISR, (void *) &bFPLL_FSM);
            }
        }
        else
        {
            //FSM disable FPLL, then disable ISR
            if(TRUE == MDrv_XC_InterruptIsAttached(SC_INT_VSINT, _MApi_XC_FPLL_FSM_ISR, (void *) &bFPLL_FSM))
            {
                MDrv_XC_InterruptDeAttachWithoutMutex(SC_INT_VSINT, _MApi_XC_FPLL_FSM_ISR, (void *) &bFPLL_FSM);
            }
        }
    }
    else
    {
        if(_bFSM_FPLL_Working &&
           (TRUE == MDrv_XC_InterruptIsAttached(SC_INT_VSINT, _MApi_XC_FPLL_FSM_ISR, (void *) &bFPLL_FSM)))
        {
            //State change from FSM to NON-FSM, disable ISR
            MDrv_XC_InterruptDeAttachWithoutMutex(SC_INT_VSINT, _MApi_XC_FPLL_FSM_ISR, (void *) &bFPLL_FSM);
            _enFpllFsm = FPLL_IDLE_STATE;
        }
        _bFSM_FPLL_Working= FALSE;
        MApi_XC_SetPanelTiming_NonFSM(pTimingInfo, eWindow);
    }
#if FRC_INSIDE
    if(g_XC_Pnl_Misc.FRCInfo.bFRC)
    {
        MApi_XC_SetFLock(pTimingInfo, E_XC_FLOCK_FPLL_ON, eWindow);

    }


#endif
    _XC_RETURN();
}

//-------------------------------------------------------------------------------------------------
/// set free run(Disable input chanel) color
/// @param  u32aRGB
/// -  0:Black
/// -  1:White
/// -  2:Blue
//-------------------------------------------------------------------------------------------------
void MApi_XC_SetFrameColor ( MS_U32 u32aRGB, SCALER_WIN eWindow )
{
    //MS_U8 u8Bank;
    _XC_ENTRY();
    //u8Bank = MDrv_ReadByte(BK_SELECT_00);
    //MDrv_WriteByte(BK_SELECT_00, REG_BANK_VOP);
    //MDrv_WriteByte(H_BK_VOP(0x19), (MS_U8) (u32aRGB >> 16));                ///< R
    //MDrv_WriteByte(L_BK_VOP(0x1A), (MS_U8) (u32aRGB >> 8));                 ///< G
    //MDrv_WriteByte(H_BK_VOP(0x1A), (MS_U8) u32aRGB);                        ///< B
    //SC_W2BYTEMSK(REG_SC_BK10_19_L,  (u32aRGB >> 8),0xFF00);            ///< R
    //SC_W2BYTEMSK(REG_SC_BK10_1A_L,  (u32aRGB >> 8),0x00FF);            ///< G
    //SC_W2BYTEMSK(REG_SC_BK10_1A_L,  (u32aRGB << 8),0xFF00);            ///< B
    Hal_XC_SetFrameColor(u32aRGB, eWindow);
    //MDrv_WriteByte(BK_SELECT_00, u8Bank);
    _XC_RETURN();
}

//-------------------------------------------------------------------------------------------------
/// set frame color on/off
/// @param  bEn
/// -  0:disable
/// -  1:enable
//-------------------------------------------------------------------------------------------------
void MApi_XC_SetFrameColor_En(MS_BOOL bEn, SCALER_WIN eWindow)
{
    _XC_ENTRY();
    Hal_XC_SetFrameColor_En(bEn, eWindow);
    _XC_RETURN();
}
/******************************************************************************/
///set free run display window size
/******************************************************************************/
void MDrv_SC_set_freerun_window ( void )
{
    //MS_U8 u8Bank;

    //u8Bank = MDrv_ReadByte(BK_SELECT_00);
    //MDrv_WriteByte(BK_SELECT_00, REG_BANK_VOP);

    /*MDrv_Write2Byte(L_BK_VOP(0x08), g_XC_InitData.stPanelInfo.u16HStart); // Image H start
    MDrv_Write2Byte(L_BK_VOP(0x09), g_XC_InitData.stPanelInfo.u16HStart + g_XC_InitData.stPanelInfo.u16Width - 1); // Image H end
    MDrv_Write2Byte(L_BK_VOP(0x0A), g_XC_InitData.stPanelInfo.u16VStart + g_XC_InitData.stPanelInfo.u16Height - 1); // Image V end
    MDrv_Write2Byte(L_BK_VOP(0x0B), g_XC_InitData.stPanelInfo.u16VStart + g_XC_InitData.stPanelInfo.u16Height - 1); // DE V end*/
    SC_W2BYTE(REG_SC_BK10_08_L, g_XC_InitData.stPanelInfo.u16HStart); // Image H start
    SC_W2BYTE(REG_SC_BK10_09_L, g_XC_InitData.stPanelInfo.u16HStart + g_XC_InitData.stPanelInfo.u16Width - 1); // Image H end
    SC_W2BYTE(REG_SC_BK10_0A_L, g_XC_InitData.stPanelInfo.u16VStart + g_XC_InitData.stPanelInfo.u16Height - 1); // Image V end
    SC_W2BYTE(REG_SC_BK10_0B_L, g_XC_InitData.stPanelInfo.u16VStart + g_XC_InitData.stPanelInfo.u16Height - 1); // DE V end*/

    //MDrv_WriteByte(BK_SELECT_00, u8Bank);
}

//-----------------------------------------------------------------------------
// frame lock
//-----------------------------------------------------------------------------
static void MDrv_Scaler_SetFrameLock(MS_BOOL bSignal)
{
    //MS_U8 u8Bank;
    MS_U16 u16OutputVtotal = g_XC_InitData.stPanelInfo.u16DefaultVTotal;

    MS_BOOL bFramelockEnable = DISABLE;
    MS_BOOL bEnFreerunOutput = DISABLE;
    MS_BOOL bUseMainFB = !gSrcInfo[0].bFBL;

    MS_BOOL bSetFPLL = DISABLE;

    //u8Bank = MDrv_ReadByte(BK_SELECT_00);

    if (bSignal == FALSE) // No Signal (Pure Freerun Mode)
    {
        u16OutputVtotal -= 1;
        bEnFreerunOutput = ENABLE;
    }
    else // Have Signal
    {
        if (bUseMainFB)
        {
            bSetFPLL = ENABLE;
            u16OutputVtotal -= 1;
            bEnFreerunOutput = ENABLE;

            bFramelockEnable = ENABLE;
        }
        else
        {
            bSetFPLL = DISABLE;
            u16OutputVtotal -= 1;
            bEnFreerunOutput = ENABLE;

            bFramelockEnable = DISABLE;
        }
    }

    /* set FPLL */
    MDrv_SC_set_fpll(bSetFPLL, DISABLE, 0);

    /* enable output free-run */
    //MDrv_WriteByte(BK_SELECT_00, REG_BANK_VOP);
    //MDrv_WriteByte(H_BK_VOP(0x1C), bEnFreerunOutput);
    SC_W2BYTEMSK(REG_SC_BK10_1C_L, bEnFreerunOutput ? BIT(8): 0,BIT(8));
    /* Enable/Disable FrameLock */
    //MDrv_WriteByte(BK_SELECT_00, REG_BANK_IP1F2);
    //MDrv_WriteRegBit(H_BK_IP1F2(0x03), bFramelockEnable, BIT(7));
    SC_W2BYTEMSK(REG_SC_BK01_03_L, bFramelockEnable ? BIT(15): 0,BIT(15));

    //MDrv_WriteByte(BK_SELECT_00, u8Bank);
}

//-------------------------------------------------------------------------------------------------
/// set free run display window timing
//-------------------------------------------------------------------------------------------------
void MDrv_XC_SetFreeRunTiming (void)
{
    if(_bFSM_FPLL_Working &&
       (TRUE == MDrv_XC_InterruptIsAttached(SC_INT_VSINT, _MApi_XC_FPLL_FSM_ISR, (void *) &bFPLL_FSM)))
    {
        //FSM, disable FPLL, so disable ISR
        MDrv_XC_InterruptDeAttachWithoutMutex(SC_INT_VSINT, _MApi_XC_FPLL_FSM_ISR, (void *) &bFPLL_FSM);
        _enFpllFsm = FPLL_IDLE_STATE;
    }
#if VERIFY_DISPLAY_FPGA  //value setting
    MDrv_SC_set_output_dclk(g_XC_InitData.stPanelInfo.u16HTotal,
                            g_XC_InitData.stPanelInfo.u16VTotal,
                            120, eWindow);
#endif
    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "SetFreeRunTiming: Disable FPLL and Clear FSM to Idle state\r\n")
    gSrcInfo[MAIN_WINDOW].bEnableFPLL = FALSE;
    gSrcInfo[SUB_WINDOW].bEnableFPLL = FALSE;
    MDrv_Scaler_SetFrameLock(FALSE);

   u32OutputPhaseStable = FALSE;
   u32OutputPRDLock     = FALSE;

    switch (g_XC_InitData.stPanelInfo.eLPLL_Type)
    {
        case E_XC_PNL_LPLL_DAC_I:
        case E_XC_PNL_LPLL_DAC_P:
            //MDrv_WriteByteMask(H_BK_VOP(0x10), BIT(7)|BIT(6), 0xFF); // set output vsync manually
            SC_W2BYTEMSK(REG_SC_BK10_10_L, BIT(15)|BIT(14), 0xFF00); // set output vsync manually
            break;
        default:
            //MDrv_WriteByteMask(H_BK_VOP(0x10), BIT(6), 0x7F); // set output sync ctl for Freerun
            SC_W2BYTEMSK(REG_SC_BK10_10_L, BIT(14), 0x7F00); // set output sync ctl for Freerun
    }

    //MDrv_WriteByte(BK_SELECT_00, u8Bank);
}

//-------------------------------------------------------------------------------------------------
/// set free run display window timing
//-------------------------------------------------------------------------------------------------
void MApi_XC_SetFreeRunTiming (void)
{
    _XC_ENTRY();
    MDrv_XC_SetFreeRunTiming();
    _XC_RETURN();
}

//-------------------------------------------------------------------------------------------------
/// Setup Gamma function ON/OFF
/// @param  bEnable      \b IN: Enable or Disable
//-------------------------------------------------------------------------------------------------
void MApi_XC_SetGammaOnOff ( MS_BOOL bEnable )
{
    //MS_U8 u8Bank;
    _XC_ENTRY();
    /*u8Bank = MDrv_ReadByte(BK_SELECT_00);
    MDrv_WriteByte(BK_SELECT_00, REG_BANK_VOP);
    MDrv_WriteRegBit(L_BK_VOP(0x50), bEnable, BIT(0));
    MDrv_WriteByte(BK_SELECT_00, u8Bank);*/
    SC_W2BYTEMSK(REG_SC_BK10_50_L, bEnable, BIT(0));
    _XC_RETURN();
}

void MDrv_SC_set_output_dclk(MS_U16 u16Htt, MS_U16 u16Vtt, MS_U16 u16VFreq, SCALER_WIN eWindow)
{
    MS_U32 u32UpBound, u32LowBound, u32Steps = 10, u32DelayMs = 0; //delaytime=0 means wait 2vsync as the old logic
    XC_PANEL_INFO_EX *pPanelInfoEx;
    MS_U32 u32PllSet;

    MS_U32 u32DClk = u16Htt * u16Vtt * u16VFreq;

    pPanelInfoEx = NULL;
    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "**** SetOutputDclk u16VFreq=%u ***\r\n", u16VFreq)
    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "DefaultPanelInfo: MinSET = 0x%lx, MaxSET = 0x%lx\r\n", g_XC_InitData.stPanelInfo.u32MinSET, g_XC_InitData.stPanelInfo.u32MaxSET);

#if 1
{
    MS_U32 u32Factor = MHal_SC_Get_LpllSet_Factor(g_XC_InitData.stPanelInfo.u8LPLL_Mode);
    u32UpBound  = g_XC_InitData.stPanelInfo.u32MaxSET * u32Factor;
    u32LowBound = g_XC_InitData.stPanelInfo.u32MinSET * u32Factor;
}
#else
#if (_FIX_LOOP_DIV_SUPPORTED)
    //The low/up bound is calculated with old method, so here need cover the change between the two methods
    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "g_XC_InitData.stPanelInfo.u8LPLL_Mode = %d mode\r\n", g_XC_InitData.stPanelInfo.u8LPLL_Mode);
    // the following is coding for M10/J2
    //============================
    //Because the LPLL_LOOPGAIN = 8,
    //UP_M10_Dual   = UP_A6_Dual   * 1/2
    //UP_M10_Single = UP_A6_Single * 1
    //============================

    u32UpBound  = g_XC_InitData.stPanelInfo.u32MaxSET * (LVDS_MPLL_CLOCK_MHZ * LPLL_LOOPGAIN/8/216);
    u32LowBound = g_XC_InitData.stPanelInfo.u32MinSET * (LVDS_MPLL_CLOCK_MHZ * LPLL_LOOPGAIN/8/216);
    if(g_XC_InitData.stPanelInfo.u8LPLL_Mode != 0)
    {
        //In old method, single channel set is half of dual. But In M10, all set is calculated like single mode
        u32LowBound /= 2;
        u32UpBound  /= 2;
    }
#else
    #if FRC_INSIDE// for A5
    u32UpBound  = g_XC_InitData.stPanelInfo.u32MaxSET * (LPLL_LOOPGAIN/8);
    u32LowBound = g_XC_InitData.stPanelInfo.u32MinSET * (LPLL_LOOPGAIN/8);

    u32LowBound /= 2;
    u32UpBound  /= 2;
    #else // for A1/A7/T12/T13

    u32UpBound = g_XC_InitData.stPanelInfo.u32MaxSET  * ( LPLL_LOOPGAIN / 8 ) ;
    u32LowBound = g_XC_InitData.stPanelInfo.u32MinSET * ( LPLL_LOOPGAIN / 8 ) ;
    if ((LVDS_MPLL_CLOCK_MHZ == 432) && (LPLL_LOOPGAIN == 8))
    {
        u32UpBound  = g_XC_InitData.stPanelInfo.u32MaxSET * (LVDS_MPLL_CLOCK_MHZ * LPLL_LOOPGAIN/8/216);
        u32LowBound = g_XC_InitData.stPanelInfo.u32MinSET * (LVDS_MPLL_CLOCK_MHZ * LPLL_LOOPGAIN/8/216);
    }
    #endif
#endif
#endif

    if(_MDrv_XC_GetExPanelInfo(&pPanelInfoEx, u16VFreq) && pPanelInfoEx != NULL)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "PanelInfoEx assigned\r\n")
        if(pPanelInfoEx->bDClkValid)
        {
            u32DClk = pPanelInfoEx->u32DClk*10000;
        }
        //LowBound for MaxCLK, UpBound for MinCLK
        if(pPanelInfoEx->bMaxDClkValid)
        {
            u32LowBound = MDrv_PNL_CalculateLPLLSETbyDClk(pPanelInfoEx->u32MaxDClk*10000);
        }
        if(pPanelInfoEx->bMinDClkValid)
        {
            u32UpBound = MDrv_PNL_CalculateLPLLSETbyDClk(pPanelInfoEx->u32MinDClk*10000);
        }
        if(pPanelInfoEx->bDclkDelayValid)
        {
            u32DelayMs = (MS_U32)pPanelInfoEx->u16DclkDelay;
        }
        if(pPanelInfoEx->bDclkStepsValid)
        {
            u32Steps = (MS_U32)pPanelInfoEx->u16DclkSteps;
        }
    }
    //u32PllSet = MDrv_Scaler_CalculateLPLLSETbyDClk(u32DClk);
    u32PllSet = MDrv_PNL_CalculateLPLLSETbyDClk(u32DClk);

    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "u32PllSet = 0x%lx, u32UpBound = 0x%lx, u32LowBound = 0x%lx\r\n", u32PllSet, u32UpBound, u32LowBound)
    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "User forces Enable=%u, LPLL limit=0x%lx\r\n", _bFpllCusFlag[E_FPLL_FLAG_D5D6D7], _u32FpllCusData[E_FPLL_FLAG_D5D6D7])

    HAL_SC_Set_FPLL_Limit(&u32PllSet, u32LowBound, u32UpBound, _bFpllCusFlag[E_FPLL_FLAG_D5D6D7], _u32FpllCusData[E_FPLL_FLAG_D5D6D7]);

    // just first patch for TCON case
    if((g_XC_InitData.stPanelInfo.eLPLL_Type != E_XC_PNL_LPLL_MINILVDS) &&
       (g_XC_InitData.stPanelInfo.eLPLL_Type != E_XC_PNL_LPLL_ANALOG_MINILVDS) &&
       (g_XC_InitData.stPanelInfo.eLPLL_Type != E_XC_PNL_LPLL_DIGITAL_MINILVDS))
    {
        MDrv_SC_Set_Output_Dclk_Slowly(u32PllSet, u32Steps, u32DelayMs, eWindow);
    }

    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "**** SetOutputDclk ***\r\n")
    XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "SET= 0x%X (%u)\r\n", (unsigned int)u32PllSet, (unsigned int)u32PllSet)
}

MS_U32 MApi_XC_Get_Current_OutputVFreqX100(void)
{
#if 0
    MS_U32 u32OutFrameRatex100 = 0;  //OutVFreq*100

    MS_U32 u32PllSet= 0; // 3BYTE
    MS_U16 u16Htt = SC_R2BYTEMSK(REG_SC_BK10_0C_L, 0xFFF);
    MS_U16 u16Vtt = SC_R2BYTEMSK(REG_SC_BK10_0D_L, 0xFFF);
    MS_U64 u64tmp = 0;

    LPLL_BK_STORE;
    LPLL_BK_SWITCH(0x00);
    u32PllSet= MDrv_Read4Byte(L_BK_LPLL(0x0F)) & 0xFFFFFF; // 3BYTE
    LPLL_BK_RESTORE;
    if((g_XC_InitData.stPanelInfo.eLPLL_Type == E_XC_PNL_LPLL_MINILVDS) || (g_XC_InitData.stPanelInfo.eLPLL_Type == E_XC_PNL_LPLL_ANALOG_MINILVDS) || (g_XC_InitData.stPanelInfo.eLPLL_Type == E_XC_PNL_LPLL_DIGITAL_MINILVDS))
    {
//        u32OutFrameRatex100 = (MS_U32)((((MS_U64)LVDS_MPLL_CLOCK_MHZ * 524288 * 2) *100000000)/((MS_U64)(u16Htt * u16Vtt) * u32PllSet));
//**NOTE! In 2.6.28.9 Linux Kernel does not support 64bit division**
//        u32OutFrameRatex100 = (MS_U32)((((MS_U64)(LVDS_MPLL_CLOCK_MHZ * 524288) * 2 * 100000000)/(u16Htt * u16Vtt))/ u32PllSet);
        u64tmp = ((MS_U64)LVDS_MPLL_CLOCK_MHZ * 524288 * 2) * 100000000;
        do_div(u64tmp,(MS_U64)(u16Htt * u16Vtt));
        do_div(u64tmp,(u32PllSet));
        u32OutFrameRatex100 = u64tmp;
    }
    else
    {
        if( g_XC_InitData.stPanelInfo.u8LPLL_Mode )
        {
            //u32OutFrameRatex100 = (MS_U32)((((MS_U64)(LVDS_MPLL_CLOCK_MHZ * 524288 * LPLL_LOOPGAIN) * 2 * 100000000)/(u16Htt * u16Vtt * 7))/ u32PllSet);
            u64tmp = ((MS_U64)LVDS_MPLL_CLOCK_MHZ * 524288 * LPLL_LOOPGAIN * 2) * 100000000;
            do_div(u64tmp,(u16Htt * u16Vtt * 7));
            do_div(u64tmp,(u32PllSet));
            u32OutFrameRatex100 = u64tmp;

        }
        else
        {
//            u32OutFrameRatex100 = (MS_U32)((((MS_U64)(LVDS_MPLL_CLOCK_MHZ * 524288 * LPLL_LOOPGAIN) * 100000000)/(u16Htt * u16Vtt * 7))/ u32PllSet);
            u64tmp = ((MS_U64)LVDS_MPLL_CLOCK_MHZ * 524288 * LPLL_LOOPGAIN) * 100000000;
            do_div(u64tmp,(u16Htt * u16Vtt * 7));
            do_div(u64tmp,(u32PllSet));
            u32OutFrameRatex100 = u64tmp;
        }
    }
    return u32OutFrameRatex100;
#else
    return MDrv_SC_GetOutputVFreqX100();
#endif
}


//-------------------------------------------------------------------------------------------------
/// Enable or disable IP auto coast
/// @param  bEnable                \b IN: Enable or Disable
//-------------------------------------------------------------------------------------------------
void MApi_XC_EnableIPAutoCoast(MS_BOOL bEnable)
{
    /*MS_U8 u8Bank;
    u8Bank = MDrv_ReadByte(BK_SELECT_00);
    MDrv_WriteByte(BK_SELECT_00, REG_BANK_IP1F2);
    MDrv_WriteRegBit(L_BK_IP1F2(0x0F), bEnable, BIT(7));
    MDrv_WriteByte(BK_SELECT_00, u8Bank);*/
    _XC_ENTRY();
    SC_W2BYTEMSK(REG_SC_BK01_0F_L, bEnable? BIT(7):0, BIT(7));
    _XC_RETURN();
}

//-------------------------------------------------------------------------------------------------
/// Enable IP auto coast Debounce
//-------------------------------------------------------------------------------------------------
void MApi_XC_EnableIPCoastDebounce(void)
{
    _bDisableCoastDebounce = 0;
}

//-------------------------------------------------------------------------------------------------
/// Clear IP auto coast status
//-------------------------------------------------------------------------------------------------
void MApi_XC_ClearIPCoastStatus(void)
{
    MS_U16 u16Tmp;
    //MS_U8 u8Bank;
    _XC_ENTRY();

    //u8Bank = MDrv_ReadByte(BK_SELECT_00);

    //MDrv_WriteByte(BK_SELECT_00, REG_BANK_IP1F2);

    //u8Tmp = MDrv_ReadByte(L_BK_IP1F2(0x0F));
    u16Tmp = SC_R2BYTE(REG_SC_BK01_0F_L);
    if (u16Tmp & BIT(6))
    {
        _u8CoastClearDebounce++;
        //MDrv_WriteByte(L_BK_IP1F2(0x0F), u8Tmp & ~(BIT(6)));
        SC_W2BYTE(REG_SC_BK01_0F_L, u16Tmp & ~(BIT(6)));
        if(_u8CoastClearDebounce>3)
        {
            if (_bDisableCoastDebounce)
            {
                LPLL_BK_STORE;
                LPLL_BK_SWITCH(0x00);
                MDrv_WriteRegBit(L_BK_LPLL(0x0C), DISABLE, BIT(3));
                LPLL_BK_RESTORE;
            }
            else
            {
                MDrv_XC_wait_output_vsync(1, 50, MAIN_WINDOW);
                Hal_SC_ip_software_reset(REST_IP_F2, MAIN_WINDOW);
                //MDrv_WriteByte(L_BK_IP1F2(0x0F), u8Tmp & ~(BIT(6)));
                SC_W2BYTE(REG_SC_BK01_0F_L, u16Tmp & ~(BIT(6)));
                _u8CoastClearDebounce=0;
                _bDisableCoastDebounce = 1;
            }
        }
    }
    else
    {
        MDrv_WriteRegBit(L_BK_LPLL(0x0C), ENABLE, BIT(3));
        _u8CoastClearDebounce=0;
    }
    _XC_RETURN();

    //MDrv_WriteByte(BK_SELECT_00, u8Bank);
}

//-------------------------------------------------------------------------------------------------
/// Query the capability of scaler about source to Video encoder(VE)
/// @param  OutputCapability              \b OUT: Which part of scaler can output data to Video Encoder (VE).
///Check E_XC_SOURCE_TO_VE for capability bit. Take an example, if bit0 of OutputCapability equal to 1,
///Scaler can output data to ve from its E_XC_IP(IP)
///@return @ref MS_BOOL True if query success.
//-------------------------------------------------------------------------------------------------
E_APIXC_ReturnValue MApi_XC_SupportSourceToVE(MS_U16* pOutputCapability)
{
    return Hal_SC_support_source_to_ve(pOutputCapability);
}

E_APIXC_ReturnValue MApi_XC_SetOutputCapture(MS_BOOL bEnable,E_XC_SOURCE_TO_VE eSourceToVE)
{
    E_APIXC_ReturnValue ret;
    _XC_ENTRY();
    ret = Hal_SC_set_output_capture_enable(bEnable,eSourceToVE);
    if(bEnable)
    {
        gSrcInfo[MAIN_WINDOW].Status2.eSourceToVE = eSourceToVE;
    }
    else
    {
        gSrcInfo[MAIN_WINDOW].Status2.eSourceToVE = E_XC_NONE;
    }
    _XC_RETURN();
    return ret;
}

void MApi_XC_Set_OutputColor(SCALER_WIN eWindow, E_XC_OUTPUT_COLOR_TYPE enColorType)
{
    _XC_ENTRY();
    gSrcInfo[eWindow].Status2.enOutputColorType = enColorType;
    _XC_RETURN();
}


E_XC_OUTPUT_COLOR_TYPE MApi_XC_Get_OutputColor(SCALER_WIN eWindow)
{
    E_XC_OUTPUT_COLOR_TYPE  enColor;
    _XC_ENTRY();
    enColor = gSrcInfo[eWindow].Status2.enOutputColorType;
    _XC_RETURN();
    return enColor;
}



void MDrv_SC_set_csc( MS_BOOL bEnable, SCALER_WIN eWindow )
{
    Hal_SC_set_csc( bEnable, eWindow );
}

#ifdef MULTI_SCALER_SUPPORTED

void MDrv_SC_rep_window(MS_BOOL bEnable,MS_WINDOW_TYPE Win,MS_U8 u8Color, SCALER_WIN eWindow)
{
    Hal_SC_set_rep_window(bEnable,Win.x,Win.y,Win.width,Win.height,u8Color, eWindow);
}

#else

void MDrv_SC_rep_window(MS_BOOL bEnable,MS_WINDOW_TYPE Win,MS_U8 u8Color)
{
    Hal_SC_set_rep_window(bEnable,Win.x,Win.y,Win.width,Win.height,u8Color);
}

#endif

//-------------------------------------------------------------------------------------------------
/// update display window registers with input window
/// @param  eWindow                          \b IN: Main or sub window
/// @param  pdspwin                          \b IN: window info that will be setted to registers
//-------------------------------------------------------------------------------------------------
void MDrv_XC_set_dispwin_to_reg(SCALER_WIN eWindow, MS_WINDOW_TYPE *pdspwin)
{
    Hal_SC_set_disp_window(eWindow, pdspwin);
}

//-------------------------------------------------------------------------------------------------
/// get current display window registers setting
/// @param  eWindow                          \b IN : Main or sub window
/// @param  pdspwin                          \b OUT: Pointer for ouput disp window register
//-------------------------------------------------------------------------------------------------
void MDrv_XC_get_dispwin_from_reg(SCALER_WIN eWindow, MS_WINDOW_TYPE *pdspwin)
{
    Hal_SC_get_disp_window(eWindow, pdspwin);
}

//-------------------------------------------------------------------------------------------------
/// Set VOP Gain for Main Window(after 3x3 before Gamma)
/// @param  eVop_Channel                \b IN: Select VOP R/G/B
/// @param  u16Val                           \b IN: Set value
//-------------------------------------------------------------------------------------------------
void MApi_XC_SetPreGammaGain(SCALER_WIN eWindow, MS_XC_VOP_CHANNEL_t eVop_Channel, MS_U16 u16Val)
{
    _XC_ENTRY();
    HAL_SC_VOP_Set_Contrast_Value(eVop_Channel, u16Val, eWindow);
    _XC_RETURN();
}

//-------------------------------------------------------------------------------------------------
/// Set VOP Offset for Main Window(after 3x3 before Gamma)
/// @param  eVop_Channel                \b IN: Select VOP R/G/B
/// @param  u16Val                           \b IN: Set value
//-------------------------------------------------------------------------------------------------
void MApi_XC_SetPreGammaOffset(SCALER_WIN eWindow, MS_XC_VOP_CHANNEL_t eVop_Channel, MS_U16 u16Val)
{
    _XC_ENTRY();
    HAL_SC_VOP_Set_Brightness_Value(eVop_Channel, u16Val, eWindow);
    _XC_RETURN();
}


//-------------------------------------------------------------------------------------------------
/// Wait FPLL finished
/// @return @ref MS_BOOL indicate FPLL finished or not (frame lock finished or not)
//-------------------------------------------------------------------------------------------------
static MS_BOOL _MDrv_SC_WaitFPLLDone(void)
{
    if(HAL_SC_WaitFPLLDone())
    {
        if(_bNeedReloadMCNR && _bEnableMCNR && !_bForceDisableMCNR)
        {
            MS_U32 u32timer = MsOS_GetSystemTime() - _u32MCNRTimer;
            MS_U32 u32DelayTime = (MS_U32)MDrv_SC_FPLL_GetMCNRDelayTime();
            if(u32DelayTime > u32timer)
            {
                MsOS_DelayTask(u32DelayTime-u32timer); //Turn on before 8 vsync
            }
            _XC_ENTRY();
            Hal_SC_SetOPWriteOff(!_bEnableMCNR);
            _XC_RETURN();
            _bNeedReloadMCNR = FALSE;
        }
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

//-------------------------------------------------------------------------------------------------
/// Wait FPLL finished
/// @return @ref MS_BOOL indicate FPLL finished or not (frame lock finished or not)
//-------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_WaitFPLLDone(void)
{
    // we didn't add mutex here because
    // in T3, MDrv_SC_WaitFPLLDone() will use ISR
    // in U3, MDrv_SC_WaitFPLLDone() won't use ISR and will use mutex in HAL layer
    if(gSrcInfo[MAIN_WINDOW].bEnableFPLL)
    {
        if(_bFSM_FPLL_Working)
        {
            gSrcInfo[MAIN_WINDOW].bDoneFPLL = TRUE; //FSM framelock open screen directly, no need to wait.
        }
        else
        {
            gSrcInfo[MAIN_WINDOW].bDoneFPLL = _MDrv_SC_WaitFPLLDone();
        }
    }
    else
    {
        gSrcInfo[MAIN_WINDOW].bDoneFPLL = FALSE; //Freerun, NO fpll
    }
    return gSrcInfo[MAIN_WINDOW].bDoneFPLL;
}

//-------------------------------------------------------------------------------------------------
/// Get scaler output vertical frequency
/// @return @ref MS_U16 return output vertical frequency x 100
//-------------------------------------------------------------------------------------------------
MS_U16 MDrv_SC_GetOutputVFreqX100(void)
{
    return (gSrcInfo[MAIN_WINDOW].Status2.u16OutputVFreqAfterFRC*10);
}

//-------------------------------------------------------------------------------------------------
/// Set scaler display line buffer mode
/// @param  bEnable                \b IN: Select "Enable" to ring buffer mode or "Disable" for line buffer mode
//-------------------------------------------------------------------------------------------------
#ifdef MULTI_SCALER_SUPPORTED
void MDrv_SC_SetDisplay_LineBuffer_Mode(MS_BOOL bEnable, SCALER_WIN eWindow)
{
    HAL_SC_SetDisplay_LineBuffer_Mode(bEnable, eWindow);
}
#else  // #ifdef MULTI_SCALER_SUPPORTED
void MDrv_SC_SetDisplay_LineBuffer_Mode(MS_BOOL bEnable)
{
    HAL_SC_SetDisplay_LineBuffer_Mode(bEnable);
}
#endif

void MDrv_XC_SetOSD2VEmode(EN_VOP_SEL_OSD_XC2VE_MUX  eVOPSelOSD_MUX)
{
    Hal_SC_Set_OSD2VE(eVOPSelOSD_MUX);
}

MS_BOOL MDrv_XC_Check_HNonLinearScaling(void)
{
    return Hal_SC_Check_HNonLinearScaling();
}

MS_BOOL MApi_XC_Check_HNonLinearScaling(void)
{
    MS_BOOL bRet;
    _XC_ENTRY();
    bRet = MDrv_XC_Check_HNonLinearScaling();
    _XC_RETURN();
    return bRet;
}

#ifdef MULTI_SCALER_SUPPORTED
void MApi_XC_Set_Extra_fetch_adv_line(MS_U8 u8val, SCALER_WIN eWindow)
{
    _XC_ENTRY();
    Hal_SC_Set_extra_fetch_line( u8val & 0x0F, eWindow);
    Hal_SC_Set_extra_adv_line( u8val>>4, eWindow);
    _XC_RETURN();
}
#else	// #ifdef MULTI_SCALER_SUPPORTED
void MApi_XC_Set_Extra_fetch_adv_line(MS_U8 u8val)
{
    _XC_ENTRY();
    Hal_SC_Set_extra_fetch_line( u8val & 0x0F);
    Hal_SC_Set_extra_adv_line( u8val>>4);
    _XC_RETURN();
}
#endif
//-------------------------------------------------------------------------------------------------
/// Enable FPLL related settings not by program but by user's setting from TV tool
/// Including PhaseLimit, limitD5D6D7_RK
/// @param bEnable @ref MS_BOOL          \b IN: enable or not
//-------------------------------------------------------------------------------------------------
void MApi_XC_EnableFpllManualSetting(MS_BOOL bEnable)
{
    g_pSContext->bEnableFPLLManualDebug = bEnable;
}

//--------------------------------------------------------------------------------------------------------------------
/// Test if panel output is ok by changing output DClk, this is to test max range that FPLL can do
/// @param u32FpllSetOffset @ref MS_U32     \b IN: the offset to be add/minus the typical DClk output, unit is SET
/// @param u16TestCnt       @ref MS_U16     \b IN: the times to do boundary test
//--------------------------------------------------------------------------------------------------------------------
void MApi_XC_FpllBoundaryTest(MS_U32 u32FpllSetOffset, MS_U16 u16TestCnt)
{
    MS_U16 i;
    MS_U32 u32OrigFpllSet;

    // set to free run
    MApi_XC_SetFreeRunTiming();

    // set to default output DClk
    printf("MApi_XC_EnableFpllBoundaryTest(0x%lx, %d)\r\n", u32FpllSetOffset, u16TestCnt);
    printf("MApi_XC_EnableFpllBoundaryTest() -> Default DClk = %d x %d x %d = %ld\r\n",
        g_XC_InitData.stPanelInfo.u16DefaultHTotal,
        g_XC_InitData.stPanelInfo.u16DefaultVTotal,
        g_XC_InitData.stPanelInfo.u16DefaultVFreq,
        MDrv_PNL_CalculateLPLLSETbyDClk(
                                    g_XC_InitData.stPanelInfo.u16DefaultHTotal*
                                    g_XC_InitData.stPanelInfo.u16DefaultVTotal*
                                    g_XC_InitData.stPanelInfo.u16DefaultVFreq)
    );

    MDrv_SC_set_output_dclk(g_XC_InitData.stPanelInfo.u16DefaultHTotal,
                            g_XC_InitData.stPanelInfo.u16DefaultVTotal,
                            g_XC_InitData.stPanelInfo.u16DefaultVFreq, MAIN_WINDOW);
    LPLL_BK_STORE;
    LPLL_BK_SWITCH(0x00);
    u32OrigFpllSet = MDrv_Read4Byte(L_BK_LPLL(0x0F));

    printf("MApi_XC_EnableFpllBoundaryTest(): std set = 0x%lx\r\n", u32OrigFpllSet);

    // do FpllBoundary testst
    for(i=0; i<u16TestCnt; i++)
    {
        MDrv_Write3Byte(L_BK_LPLL(0x0F), u32OrigFpllSet + u32FpllSetOffset);
        MsOS_DelayTask(2);
        MDrv_Write3Byte(L_BK_LPLL(0x0F), u32OrigFpllSet - u32FpllSetOffset);
        MsOS_DelayTask(2);
    }
    LPLL_BK_RESTORE;
}


void MApi_XC_Set_OPWriteOffEnable(MS_BOOL bEnable, SCALER_WIN eWindow)
{
    _bEnableMCNR = !bEnable;
    UNUSED(eWindow);
}

void MApi_XC_ReLoad_OPWriteOff(void)
{
     _bNeedReloadMCNR = TRUE;
}


// If this function is called, driver will set OPW immediatly.
void MApi_XC_ForceSet_OPWriteOffEnable(MS_BOOL bEnable ,SCALER_WIN eWindow)
{
    if (eWindow == MAIN_WINDOW )
    {
        if (bEnable)
        {
            Hal_SC_SetOPWriteOff(TRUE);
            _bForceDisableMCNR = TRUE;
        }
        else
        {
            _bForceDisableMCNR = FALSE;
        }

    }
}


MS_BOOL MDrv_XC_Get_OPWriteOffEnable(SCALER_WIN eWindow)
{
    if (eWindow == MAIN_WINDOW)
        return !_bEnableMCNR;
    else
        return TRUE;
}
//-------------------------------------------------------------------------------------------------
// This function will store the H or V sync information(SyncStart/end/width) of panel default for XC to use
// Because their is no other place to get them in XC Lib source code
// IN:
//    NONE
// Return:
//    NONE
//-------------------------------------------------------------------------------------------------
void MDrv_XC_GetDefaultHVSyncInfo(void)
{
    _u16DefaultHSyncWidth = SC_R2BYTEMSK(REG_SC_BK10_01_L, 0xFF);
    _u16DefaultHSyncBackPorch = SC_R2BYTEMSK(REG_SC_BK10_04_L, VOP_DE_HSTART_MASK) - _u16DefaultHSyncWidth;
    _u16DefaultVSyncWidth = SC_R2BYTEMSK(REG_SC_BK10_03_L, VOP_VSYNC_END_MASK) - SC_R2BYTEMSK(REG_SC_BK10_02_L, 0x7FF);
    _u16DefaultVSyncBackPorch = SC_R2BYTEMSK(REG_SC_BK10_0D_L, 0xFFF) - SC_R2BYTEMSK(REG_SC_BK10_03_L, VOP_VSYNC_END_MASK);
}

///For K1 OSD Layer Switch
E_APIXC_ReturnValue MDrv_XC_SetOSDLayer(E_VOP_OSD_LAYER_SEL eVOPOSDLayer, SCALER_WIN eWindow)
{
    return Hal_SC_Set_OSDLayer(eVOPOSDLayer, eWindow);
}

E_VOP_OSD_LAYER_SEL MDrv_XC_GetOSDLayer(SCALER_WIN eWindow)
{
    return Hal_SC_Get_OSDLayer(eWindow);
}

E_APIXC_ReturnValue MDrv_XC_SetVideoAlpha(MS_U8 u8Val, SCALER_WIN eWindow)
{
    return Hal_SC_Set_VideoAlpha(u8Val, eWindow);
}

E_APIXC_ReturnValue MDrv_XC_GetVideoAlpha(MS_U8 *pu8Val, SCALER_WIN eWindow)
{
    return Hal_SC_Get_VideoAlpha(pu8Val, eWindow);
}

MS_BOOL MDrv_XC_Get_TGen_SCTbl_HV_Start(SCALER_WIN eWindow, MS_U16 *pu16Hstart, MS_U16 *pu16Vstart)
{
   return Hal_SC_Get_TGen_SC_Tbl_HV_Start(eWindow, pu16Hstart, pu16Vstart);
}

MS_BOOL MDrv_XC_Get_TGen_SCTbl_H_Bacporch(SCALER_WIN eWindow, MS_U16 *pu16H_Backporch)
{
    return Hal_SC_Get_TGen_SC_Tbl_H_Bacporch(eWindow, pu16H_Backporch);
}

void MDrv_XC_Set_TGen(SCALER_WIN eWindow, XC_TGEN_INFO_t stTGenInfo)
{
#ifdef CHIP_C3
    if(stTGenInfo.enBits == E_XC_HDMITX_10_BITS)
        Hal_SC_TGen_SetTiming(eWindow, stTGenInfo.enTimeType, TRUE);
    else
        Hal_SC_TGen_SetTiming(eWindow, stTGenInfo.enTimeType, FALSE);


    switch(stTGenInfo.enTimeType)
    {
    default:
    case E_XC_TGEN_VE_480_I_60:
    case E_XC_TGEN_HDMI_480_I_60:
    case E_XC_TGEN_HDMI_480_P_60:
    case E_XC_TGEN_HDMI_720_P_60:
    case E_XC_TGEN_HDMI_1080_I_60:
    case E_XC_TGEN_HDMI_1080_P_60:
        _u16OutputFreq[eWindow] = 600;
        break;

    case E_XC_TGEN_VE_576_I_50:
    case E_XC_TGEN_HDMI_576_I_50:
    case E_XC_TGEN_HDMI_576_P_50:
    case E_XC_TGEN_HDMI_720_P_50:
    case E_XC_TGEN_HDMI_1080_I_50:
    case E_XC_TGEN_HDMI_1080_P_50:
        _u16OutputFreq[eWindow] = 500;
        break;

    case E_XC_TGEN_HDMI_1080_P_30:
        _u16OutputFreq[eWindow] = 300;
        break;

    case E_XC_TGEN_HDMI_1080_P_25:
        _u16OutputFreq[eWindow] = 250;
        break;

    case E_XC_TGEN_HDMI_1080_P_24:
        _u16OutputFreq[eWindow] = 240;
        break;

    }
#endif
}


void MApi_XC_DIP_Set_Dwin_Wt_Once(MS_BOOL bEn)
{
    if(bEn)
    {
        Hal_DIP_set_dwin_wt_once(TRUE);
        //Hal_DIP_set_dyn_clk_gate(FALSE);
        Hal_DIP_set_pwd_off(FALSE);
    }
    else
    {
        //Hal_DIP_set_dyn_clk_gate(TRUE);
        Hal_DIP_set_pwd_off(TRUE);
        Hal_DIP_set_dwin_wt_once(FALSE);
    }
}

void MApi_XC_DIP_Set_Dwin_Wt_Once_Trig(MS_BOOL bEn)
{
    if(bEn)
    {
        //Hal_DIP_set_dyn_clk_gate(FALSE);
        Hal_DIP_set_dwin_wt_once_trig(TRUE);
        //Hal_DIP_set_dyn_clk_gate(TRUE);
    }
}

E_APIXC_ReturnValue MApi_XC_Set_Dip_Trig(MS_BOOL bEn)
{

    if(bEn)
    {
        //Hal_DIP_set_dyn_clk_gate(FALSE);
        Hal_DIP_set_pwd_off(FALSE);
    }
    else
    {
        //Hal_DIP_set_dyn_clk_gate(TRUE);
        Hal_DIP_set_pwd_off(TRUE);
    }

    return E_APIXC_RET_OK;
}



E_APIXC_ReturnValue MApi_XC_Set_Dip_Config(XC_DIP_CONFIG_t stConfig)
{

    MS_U16 u16width;
    MS_U16 u16height;
    MS_U32 u32Totol_FB_Size = 0;
    MS_U32 u32Totol_C_FB_Size = 0;
    MS_U32 u32Extra_w_limit = 0;
    MS_U32 u32Extra_c_w_limit = 0;
    MS_U32 u32MIU_BUS = DIP_MIU_BUS;

    if(stConfig.bClipEn)
    {
        if(stConfig.stClipWin.width > stConfig.u16Width ||
           stConfig.stClipWin.height > stConfig.u16Height)
        {
            printf("SetDip: Clip window size is too big \n");
            return E_APIXC_RET_FAIL;
        }

        u16width  = stConfig.stClipWin.width < stConfig.u16Width   ? stConfig.stClipWin.width  : stConfig.u16Width;
        u16height = stConfig.stClipWin.height < stConfig.u16Height ? stConfig.stClipWin.height : stConfig.u16Height;
    }
    else
    {
        u16width  = stConfig.u16Width;
        u16height = stConfig.u16Height;
    }

    switch(stConfig.eDstFMT)
    {
    case DIP_DST_FMT_YC422:
        u32Totol_FB_Size = u16height * u16width * stConfig.u8FrameNum * 2;
        u32Extra_w_limit = 1 * DIP_MIU_BUS_BYTE;
        u32MIU_BUS = DIP_MIU_BUS;
        break;

    case DIP_DST_FMT_RGB565:
        u32Totol_FB_Size = u16height * u16width * stConfig.u8FrameNum * 2;
        u32Extra_w_limit = 1 * DIP_MIU_BUS_BYTE;
        u32MIU_BUS = DIP_MIU_BUS;
        break;

    case DIP_DST_FMT_ARGB8888:
        u32Totol_FB_Size = u16height * u16width * stConfig.u8FrameNum * 4;
        u32Extra_w_limit = 1 * DIP_MIU_BUS_BYTE;
        u32MIU_BUS = DIP_MIU_BUS;
        break;

    case DIP_DST_FMT_YC420_MVOP:
        u32Totol_FB_Size =  u16height * u16width * stConfig.u8FrameNum * 1;
        u32Totol_C_FB_Size = MVOP_420_H_Align(u16width) * MVOP_420_V_Align(u16height/2) * stConfig.u8FrameNum * 1;
        u32Extra_w_limit = 32 * DIP_MIU_BUS_BYTE;
        u32Extra_c_w_limit = 8 * DIP_MIU_BUS_BYTE;
        u32MIU_BUS = DIP_MIU_BUS;
        break;

    case DIP_DST_FMT_YC420_MFE:
        u32Totol_FB_Size = u16height * u16width * stConfig.u8FrameNum * 1;
        u32Totol_C_FB_Size = MFE_420_H_Align(u16width) * MFE_420_V_Align(u16height/2) * stConfig.u8FrameNum * 1;
        u32Extra_w_limit = 32 * DIP_MIU_BUS_I64_BYTE;
        u32Extra_c_w_limit = 8 * DIP_MIU_BUS_I64_BYTE;
        u32MIU_BUS = DIP_MIU_BUS_I64;
        break;

    default:
        printf("SetDip: unkwon FMT \n");
        return E_APIXC_RET_FAIL;

    }

    if(stConfig.u32BaseSize < u32Totol_FB_Size )
    {
        printf("SetDip: FB size is too small: %08x < %08x !!!\n", (int)stConfig.u32BaseSize, (int)u32Totol_FB_Size);
        return E_APIXC_RET_FAIL;
    }

    if(u32Totol_C_FB_Size && stConfig.u32C_BaseSize  < u32Totol_C_FB_Size )
    {
        printf("SetDip: C_FB size is too small: %08x < %08x  !!!\n", (int)stConfig.u32C_BaseSize, (int)u32Totol_C_FB_Size);
        return E_APIXC_RET_FAIL;
    }

    /*
    printf("bClip:%d, Clip:(%d, %d, %d, %d), FBNum:%d, w:%d, h:%d, picth:%d, base:%08x, size:%08x, cbase:%08x, csize:%08x,  \n",
        stConfig.bClipEn, stConfig.stClipWin.x, stConfig.stClipWin.y, stConfig.stClipWin.width, stConfig.stClipWin.height,
        stConfig.u8FrameNum, stConfig.u16Width, stConfig.u16Height, stConfig.u16Pitch,
        (int)stConfig.u32BaseAddr, (int)stConfig.u32BaseSize,
        (int)stConfig.u32C_BaseAddr, (int)stConfig.u32C_BaseSize
        );
    */

    // source select
    if(stConfig.eSrcFMT == DIP_SRC_FMT_IP_MAIN)
    {
        Hal_DIP_set_src_ip_main(TRUE);
    }
    else if(stConfig.eSrcFMT == DIP_SRC_FMT_IP_SUB)
    {
        Hal_DIP_set_src_ip_sub(TRUE);
    }
    else
    {
        Hal_DIP_set_src_op(TRUE);
    }

    // 444 to 422
    Hal_DIP_set_444_to_422(stConfig.b444To422);

    // source 422
    Hal_DIP_set_src_422(stConfig.bSrc422);

    // y2r enable
    if(stConfig.bSrcYUV == TRUE &&
       (stConfig.eDstFMT ==  DIP_DST_FMT_RGB565 || stConfig.eDstFMT == DIP_DST_FMT_ARGB8888))
    {
        Hal_DIP_set_y2r_en(TRUE);
    }
    else
    {
        Hal_DIP_set_y2r_en(FALSE);
    }

    // clip window
    Hal_DIP_set_clip_en(stConfig.bClipEn);
    Hal_DIP_set_clip_window(stConfig.stClipWin);


    // frame number
    if(stConfig.bWOnce)
    {
        Hal_DIP_set_dip_frame_num(1);
    }
    else
    {
        Hal_DIP_set_dip_frame_num(stConfig.u8FrameNum);
    }


    //base addr
    Hal_DIP_set_base_addr(stConfig.u32BaseAddr, u32MIU_BUS);

    // base offset
    Hal_DIP_set_base_offset(stConfig.u16Width * stConfig.u16Height);

    // wlimit base

    if(stConfig.eDstFMT == DIP_DST_FMT_YC420_MFE )
    {
        Hal_DIP_set_wlimit_base_addr(stConfig.u32BaseAddr + stConfig.u32BaseSize * 2 + u32Extra_w_limit, u32MIU_BUS);
    }
    else
    {
        Hal_DIP_set_wlimit_base_addr(stConfig.u32BaseAddr + stConfig.u32BaseSize + u32Extra_w_limit, u32MIU_BUS);
    }


    if(stConfig.eDstFMT == DIP_DST_FMT_YC420_MFE || stConfig.eDstFMT == DIP_DST_FMT_YC420_MVOP)
    {
        //C base addr
        Hal_DIP_set_c_base_addr(stConfig.u32C_BaseAddr, u32MIU_BUS);

        //C base offset
        if(stConfig.eDstFMT == DIP_DST_FMT_YC420_MFE)
        {
            Hal_DIP_set_c_base_offset(stConfig.u16Width * MFE_420_V_Align(stConfig.u16Height/2));
            // wlimit C base
            Hal_DIP_set_wlimit_c_base_addr(stConfig.u32C_BaseAddr + stConfig.u32C_BaseSize * 2 + u32Extra_c_w_limit, u32MIU_BUS);

        }
        else
        {
            Hal_DIP_set_c_base_offset(stConfig.u16Width *  MVOP_420_V_Align(stConfig.u16Height/2));
            // wlimit C base
            Hal_DIP_set_wlimit_c_base_addr(stConfig.u32C_BaseAddr + stConfig.u32C_BaseSize + u32Extra_c_w_limit, u32MIU_BUS);
        }


        //444to422 ratio
        Hal_DIP_set_422to420_ratio(0x08);

        //tile req num
        Hal_DIP_set_tile_req_num(0x10);
    }

     // wlimit Enable
     if(stConfig.bWOnce)
     {
        Hal_DIP_set_wlimit_en(FALSE);
        printf("SetDip: disable wlimit  (FIXME-FIXME-FIXME) !!!\n");
     }
    else{
        Hal_DIP_set_wlimit_en(TRUE);
     }

    // source height & width
    Hal_DIP_set_src_size(u16width, u16height);

    // pitch
    Hal_DIP_set_base_pitch(stConfig.u16Pitch);


    //thrd & max
    Hal_DIP_set_wreq_thrd_max(0x08, 0x10);

    // outpu format
    Hal_DIP_set_output_fmt(stConfig.eDstFMT);

    memcpy(&g_stDipConfig, &stConfig, sizeof(XC_DIP_CONFIG_t));
    return E_APIXC_RET_OK;
}


E_APIXC_ReturnValue MApi_XC_DIP_Set_Base(MS_U32 u32BaseAddr, MS_U32 u32C_BaseAddr)
{
    MS_U16 u16width;
    MS_U16 u16height;
    MS_U32 u32FB_Size = 0;
    MS_U32 u32FB_C_Size = 0;
    MS_U32 u32MIU_BUS;

    u16width  = g_stDipConfig.bClipEn ? g_stDipConfig.stClipWin.width : g_stDipConfig.u16Width;
    u16height = g_stDipConfig.bClipEn ? g_stDipConfig.stClipWin.height : g_stDipConfig.u16Height;

    switch(g_stDipConfig.eDstFMT)
    {
    case DIP_DST_FMT_YC422:
        u32FB_Size = u16height * u16width * 2;
        u32MIU_BUS = DIP_MIU_BUS;
        break;

    case DIP_DST_FMT_RGB565:
        u32FB_Size = u16height * u16width * 2;
        u32MIU_BUS = DIP_MIU_BUS;
        break;

    case DIP_DST_FMT_ARGB8888:
        u32FB_Size = u16height * u16width * 4;
        u32MIU_BUS = DIP_MIU_BUS;
        break;

    case DIP_DST_FMT_YC420_MVOP:
        u32FB_Size =  u16height * u16width * 1;
        u32FB_C_Size = MVOP_420_H_Align(u16width) * MVOP_420_V_Align(u16height/2) * 1;
        u32MIU_BUS = DIP_MIU_BUS;
        break;

    case DIP_DST_FMT_YC420_MFE:
        u32FB_Size = u16height * u16width * 1;
        u32FB_C_Size = MFE_420_H_Align(u16width) * MFE_420_V_Align(u16height/2) * 1;
        u32MIU_BUS = DIP_MIU_BUS_I64;
        break;

    default:
        printf("%s: unkwon FMT \n", __FUNCTION__);
        return E_APIXC_RET_FAIL;

    }

    /** FIXME  **/
    #if 0
    if((u32BaseAddr + u32FB_Size) > (g_stDipConfig.u32BaseAddr + g_stDipConfig.u32BaseSize))
    {
        printf("%s: out of size in Y Buffer \n",__FUNCTION__);
        return E_APIXC_RET_FAIL;
    }

    if(u32FB_C_Size &&
      ((u32C_BaseAddr + u32FB_C_Size) > (g_stDipConfig.u32C_BaseAddr + g_stDipConfig.u32C_BaseSize))  )
    {
        printf("%s: out of size in UV Buffer \n",__FUNCTION__);
        return E_APIXC_RET_FAIL;
    }
    #endif

    Hal_DIP_set_base_addr(u32BaseAddr, u32MIU_BUS);

    if(g_stDipConfig.eDstFMT == DIP_DST_FMT_YC420_MFE || g_stDipConfig.eDstFMT == DIP_DST_FMT_YC420_MVOP)
    {
        //C base addr
        Hal_DIP_set_c_base_addr(u32C_BaseAddr, u32MIU_BUS);
    }

    return E_APIXC_RET_OK;
}

E_APIXC_ReturnValue MApi_XC_Get_Htt_Vtt(MS_U16 *pu16Htt, MS_U16 *pu16Vtt, SCALER_WIN eWindow)
{
    Hal_SC_get_Htt_Vtt(pu16Htt, pu16Vtt, eWindow);
    return E_APIXC_RET_OK;
}

E_APIXC_ReturnValue MApi_XC_Get_OuputCfg(XC_OUPUT_CFG *pOutputCfg, SCALER_WIN eWindow)
{
    MS_U16 u16Htt, u16Vtt;
    MS_WINDOW_TYPE stDEWin;
    MS_U16 u16VSyncEnd, u16HSyncEnd;

    u16VSyncEnd = Hal_SC_Get_Ouput_VSync_End(eWindow);
    u16HSyncEnd = Hal_SC_Get_Ouput_HSync_End(eWindow);

    Hal_SC_get_Htt_Vtt(&u16Htt, &u16Vtt, eWindow);
    Hal_SC_get_disp_de_window(&stDEWin, eWindow);

    pOutputCfg->bInterlace = Hal_SC_Get_Ouput_Interlace(eWindow);
    pOutputCfg->u16Htt = u16Htt;
    pOutputCfg->u16Vtt = u16Vtt;
    pOutputCfg->u16Width = stDEWin.width;
    pOutputCfg->u16Height = stDEWin.height;
    pOutputCfg->u16Hstart = u16HSyncEnd;
    pOutputCfg->u16Vstart = u16Vtt - u16VSyncEnd - 4;
    pOutputCfg->u16OutputFreq = _u16OutputFreq[eWindow];

    return E_APIXC_RET_OK;
}


