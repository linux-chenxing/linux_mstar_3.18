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
/// file    Drvtvencoder.c
/// @brief  TV encoder.
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _DRVTVENCODER_C
#define _DRVTVENCODER_C

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
// Common Definition
#ifdef MSOS_TYPE_LINUX_KERNEL
#include <linux/string.h>
#include <linux/delay.h>
#include <asm/div64.h>
#else
#include <string.h>
#define do_div(x,y) ((x)/=(y))
#endif

#ifdef MSOS_TYPE_LINUX
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h> // O_RDWR
#endif
#include "MsCommon.h"
#include "MsVersion.h"
#include "MsIRQ.h"
#include "MsOS.h"
#include "drvMMIO.h"
#include "halCHIP.h"

// Internal Definition
#include "ve_Analog_Reg.h"
#include "drvTVEncoder.h"
#include "mhal_tvencoder.h"
#include "mdrv_gflip_ve_io.h"

//#include "MsXC_Common_Internal.h"

//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

// Need to Refine

#define ENABLE_VE_FIELD_INV       1

#define VE_FRC_TOLERANT           10

//-------------------------------------------------------------------------------------------------
//  Local Structurs
//-------------------------------------------------------------------------------------------------
typedef struct
{
    //ve out timing
    MS_U16 u16VttOut;   //VttOut
    MS_U16 u16HttOut;   //HttOut
    MS_U16 u16HdeOut;    //HdeOut
    MS_U16 u16VdeOut;    //Vdeout

    MS_BOOL bInterlace;

}MS_VE_PLL_OutCfg;

typedef struct
{
    MS_VE_PLL_InCfg InCfg;
    MS_VE_PLL_OutCfg OutCfg;

    MS_U32 u32IDclk;
    MS_U32 u32ODclk;
    MS_U32 u32InitPll;
}MS_VE_PLL_INFO;

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------
#ifdef MSOS_TYPE_CE

#pragma data_seg(".VETex")
MS_BOOL g_bVeDisableRegWrite = FALSE;
#pragma data_seg()
#pragma comment(linker,"/SECTION:.VETex,RWS")

#else

MS_BOOL g_bVeDisableRegWrite = FALSE;
#endif

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
static MSIF_Version _drv_ve_version = {
#if (defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_LINUX_KERNEL))
	{VE_DRV_VERSION },
#else
    .DDI = { VE_DRV_VERSION },
#endif
};

static MS_U16 _gu16DbgLevel = FALSE;
static VE_DrvInfo _stVE_DrvInfo;
static MS_VE_PLL_INFO _stVE_PllInfo;

//VE
typedef struct
{
    MS_VE_OUTPUT_DEST_TYPE OutputDestType[2];   // output device
    MS_VE_INPUT_SRC_TYPE   InputSrcType;        // input source
    MS_VE_INPUT_SRC_TYPE   InputSrcOfMixedSrc;  // The real input source for mixed "InputSrcType" of VE,
                                                // eg. Real InputSrc of "MS_VE_SRC_MAIN", "MS_VE_SRC_SUB", "MS_VE_SRC_SCALER" etc.
    MS_VE_VIDEOSYS         VideoSystem;         // video std of output signal
    MS_U8   u8DACType;
    MS_U16  u16inputVfreq;
    MS_VE_Backup_Reg BackupReg;
    MS_U16  u16H_CapStart;
    MS_U16  u16H_CapSize;
    MS_U16  u16V_CapStart;
    MS_U16  u16V_CapSize;
    MS_U16  u16H_SC_CapStart;
    MS_U16  u16H_SC_CapSize;
    MS_U16  u16V_SC_CapStart;
    MS_U16  u16V_SC_CapSize;
    MS_U8   u8VE_DisplayStatus;
    MS_BOOL bHDuplicate;
    MS_BOOL bSrcInterlace;
    MS_BOOL bForceCaptureMode;

    MS_U16  u16OutputWidth;  //Downscaling output H size
    MS_U16  u16Outputheight; //Downscaling output V size
    MS_U32  u32MiuBaseAddr;
    MS_U32  u32MemSize;
    MS_BOOL bVECapture; ///< TRUE/FALSE, enable/disable VE as a capture device
    MS_VE_CusScalingInfo stVECusScalingInfo;
    MS_S32 s32FdGFlip;
}MS_VE_Info, *PMS_VE_Info;

#ifdef MSOS_TYPE_CE		// chanting.tsai, not initialize s32FdGflip
static MS_VE_Info g_VEInfo;
#else
static MS_VE_Info g_VEInfo={.s32FdGFlip=-1,}; //Init the file descriptor to avoid unused duplicate open of "/dev/gflip"
#endif

MS_VE_Cap g_VECap;  // ve capability structure

//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------
#define MSG_DRV_VE(x)      //do{if(_gu16DbgLevel ) x;} while(0);


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
static MS_U32 _GCD(MS_U32 u32A,MS_U32 u32B)
{
    MS_U32 u32R;

    u32R = u32A % u32B;
    do{
        u32A = u32B;
        u32B = u32R;
        u32R = u32A % u32B;
    }while(u32R != 0);

    return u32B;
}

static MS_BOOL _ReductionFrac (MS_U32 *pu32A,MS_U32 *pu32B)
{
    MS_U32 u32GCD;
    while(1)
    {
        if( (*pu32A<3000) && (*pu32B<3000))
        {
            return TRUE;
        }

        u32GCD = _GCD(*pu32A,*pu32B);
        if(u32GCD == 1)
            return TRUE;

        do_div(*pu32A, u32GCD);
        do_div(*pu32B, u32GCD);
    }
    return TRUE;
}

static MS_BOOL _MultiplFrac(MS_U32 *pu32A,MS_U32 *pu32B)
{
    while(1)
    {
        if( ( (*pu32A<<1)<3000) && ( (*pu32B<<1) <3000))
        {
             *pu32A<<=1;
             *pu32B<<=1;
        }
        else
        {
            return TRUE;
        }
    }
    return TRUE;
}

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
/// @brief \b Function \b Name: MDrv_VE_GetLibVer
/// @brief \b Function \b Description: Show the VE driver version
/// @param ppVersion \b Out: Library version string
/// @return             @ref  VE_Result
//-------------------------------------------------------------------------------------------------
VE_Result MDrv_VE_GetLibVer(const MSIF_Version **ppVersion)
{

    if (!ppVersion)
    {
        return E_VE_FAIL;
    }

    *ppVersion = &_drv_ve_version;
    return E_VE_OK;
}

//-------------------------------------------------------------------------------------------------
/// Get VE Information
/// @return @ref VE_DrvInfo return the VE information in this member
//-------------------------------------------------------------------------------------------------
const VE_DrvInfo * MDrv_VE_GetInfo(void)
{
    MDrv_VE_GetCaps(&_stVE_DrvInfo.stCaps);
    return &_stVE_DrvInfo;
}

//-------------------------------------------------------------------------------------------------
/// Get VE Status
/// @param  pDrvStatus                  \b OUT: store the status
/// @return @ref VE_Result
//-------------------------------------------------------------------------------------------------
VE_Result MDrv_VE_GetStatus(VE_DrvStatus* pDrvStatus)
{
    pDrvStatus->u16H_CapStart = g_VEInfo.u16H_CapStart;
    pDrvStatus->u16H_CapSize  = g_VEInfo.u16H_CapSize;
    pDrvStatus->u16V_CapStart = g_VEInfo.u16V_CapStart;
    pDrvStatus->u16V_CapSize  = g_VEInfo.u16V_CapSize;
    pDrvStatus->VideoSystem   = g_VEInfo.VideoSystem;
    return E_VE_OK;
}

void MDrv_VE_SetDbgLevel(MS_U16 level)
{
    _gu16DbgLevel = level;
}

//Clear SW variable for VE
static void MDrv_VE_VariableInit(void)
{
    MS_S32 s32FdGFlip = g_VEInfo.s32FdGFlip;

    memset(&g_VEInfo, 0, sizeof(MS_VE_Info));
    memset(&g_VECap, 0, sizeof(MS_VE_Cap));
    memset(&_stVE_PllInfo, 0, sizeof(MS_VE_PLL_INFO));
    memset(&_stVE_DrvInfo, 0, sizeof(VE_DrvInfo));

    //Restore the file descriptor to avoid unused duplicate open of "/dev/gflip"
    g_VEInfo.s32FdGFlip = s32FdGFlip;
    g_VEInfo.InputSrcType = MS_VE_SRC_NONE;
    g_VEInfo.InputSrcOfMixedSrc = MS_VE_SRC_NONE;
}

MS_BOOL MDrv_VE_SetIOMapBase(void)
{
    MS_U32 _VERIUBaseAddress=0, u32NonPMBankSize = 0;
    if(MDrv_MMIO_GetBASE( &_VERIUBaseAddress, &u32NonPMBankSize, MS_MODULE_PM ) != TRUE)
    {
        MSG_DRV_VE(printf("MDrv_VE_Init GetBASE failure\n"));
        MS_ASSERT(0);
        return FALSE;
    }

    Hal_VE_init_riu_base( _VERIUBaseAddress );

    return TRUE;
}

//------------------------------------------------------------------------------
/// Set VE FB memory base address
/// @param[in]  u32MIUAddress  \b IN: The FB address for VE
/// @return none
//------------------------------------------------------------------------------
void MDrv_VE_SetMemoryBaseAddr(MS_PHYADDR u32MIUAddress, MS_U32 u32MIUSize)
{
    MSG_DRV_VE(printf("Change VE MemoryBase: Address=0x%lx Size=0x%lx \n",u32MIUAddress, u32MIUSize));
    //u32MIUAddress = 0x0000100000+HAL_MIU1_BASE;
    if( (u32MIUAddress >= HAL_MIU1_BASE) && (HAL_MIU1_BASE > 0))
    {
        u32MIUAddress = u32MIUAddress - HAL_MIU1_BASE;
        Hal_VE_SelMIU(TRUE);
    }
    else
    {
        Hal_VE_SelMIU(FALSE);
    }
    //g_VEInfo.u32MemSize = u32MIUSize/(BYTE_PER_WORD>>1); //Now only used in VEC, Change like this later to avoid not enable 3 frame mode
    g_VEInfo.u32MemSize = u32MIUSize;
    g_VEInfo.u32MiuBaseAddr = u32MIUAddress;
    MSG_DRV_VE(printf("Final VE MemoryBase: Address=0x%lx, size=0x%lx \n",g_VEInfo.u32MiuBaseAddr, g_VEInfo.u32MemSize));
    Hal_VE_SetMemAddr(&u32MIUAddress, &u32MIUAddress);
}

//-------------------------------------------------------------------------------------------------
/// GFlip Init for VE capture, get handler for VEC
/// @return TRUE: success/fail
/// @attention
/// <b>[Mxlib] <em></em></b>
//-------------------------------------------------------------------------------------------------
MS_BOOL MDrv_VEC_GFLIPInit(void)
{
    #ifdef MSOS_TYPE_LINUX
    if(0> g_VEInfo.s32FdGFlip)
    {
        g_VEInfo.s32FdGFlip = open("/dev/gflip", O_RDWR);
        if(0> g_VEInfo.s32FdGFlip)
        {
            printf("[%s][%d] open fail /dev/gflip\n", __FUNCTION__, __LINE__);
            return FALSE;
        }
    }
    #endif
    return TRUE;
}

//------------------------------------------------------------------------------
/// Initiate VE
/// @param[in]  u32MIUAddress  \b IN: The address reserved for VE
/// @return none
//------------------------------------------------------------------------------
// initiate video encoder
void MDrv_VE_Init(MS_PHYADDR u32MIUAddress)
{
    // This should be the first function of VE.
    MDrv_VE_VariableInit();
    if (MDrv_VE_SetIOMapBase() != TRUE)
    {
        MSG_DRV_VE(printf("VE: MDrv_VE_Init MDrv_VE_SetIOMapBase() failure\n");)
        return;
    }
    //u32MIUAddress = 0x0000100000+HAL_MIU1_BASE;
    MSG_DRV_VE(printf("VE input  u32MIUAddress=0x%lx \n",u32MIUAddress));

    MDrv_VE_SetMemoryBaseAddr(u32MIUAddress, 0);

    //get chip capability
    Hal_VE_GetCaps(&g_VECap);

    Hal_VE_init();
}

MS_BOOL MDrv_VE_Exit(void)
{
    MS_VE_Output_Ctrl OutputCtrl;
    // Turn off VE
    OutputCtrl.bEnable = FALSE;
    OutputCtrl.OutputType = MS_VE_OUT_TVENCODER;
    MDrv_VE_SetOutputCtrl(&OutputCtrl);
    return TRUE;
}

//------------------------------------------------------------------------------
/// set the output destination of video encoder
///
///              None SCART CVBS SVIDEO YPbPr
///       None    O     O     O     O     O
///       SCART O      -     X     X     X
///        CVBS  O     X      -     O     O
///     S_VIDE  O     O     X      O     -     X
///       YPbPr   O     X     O      X     -
///
/// @param  -pSwitchInfo \b IN/OUT: the information of switching output destination of TV encodeer
/// @return None
//------------------------------------------------------------------------------
void MDrv_VE_SwitchOuputDest(PMS_Switch_VE_Dest_Info pSwitchInfo)
{
    if((pSwitchInfo->OutputDstType != MS_VE_DEST_SCART) &&
       (pSwitchInfo->OutputDstType != MS_VE_DEST_YPBPR) &&
       (pSwitchInfo->OutputDstType != MS_VE_DEST_CVBS)  &&
       (pSwitchInfo->OutputDstType != MS_VE_DEST_SVIDEO))
    {
        pSwitchInfo->Status = MS_VE_SWITCH_DST_INVALID_PARAM;
        return;
    }

    pSwitchInfo->Status = MS_VE_SWITCH_DST_SUCCESS;

    if(pSwitchInfo->OutputDstType == MS_VE_DEST_SCART)
        g_VEInfo.u8DACType = VE_OUT_CVBS_RGB;
    else if(pSwitchInfo->OutputDstType == MS_VE_DEST_YPBPR)
        g_VEInfo.u8DACType = VE_OUT_CVBS_YCbCr;
    else
        g_VEInfo.u8DACType = VE_OUT_CVBS_YCC;

    Hal_VE_set_out_sel(g_VEInfo.u8DACType);
    Hal_VE_set_color_convert(g_VEInfo.VideoSystem, g_VEInfo.u8DACType);
}

//------------------------------------------------------------------------------
/// Get u8VE_DisplayStatus for set the input source of video encoder
///
/// @param  -pSwitchInfo \b IN/OUT: the information of switching input destination of TV encodeer
/// @return None
//------------------------------------------------------------------------------
static void MDrv_VE_SetSrcDispStatus(PMS_VE_InputSrc_Info pInputSrcInfo)
{
    MS_VE_INPUT_SRC_TYPE eInputSrc;
    g_VEInfo.u8VE_DisplayStatus &= ~(EN_VE_CCIR656_IN | EN_VE_RGB_IN | EN_VE_DEMODE);
    if((pInputSrcInfo->eInputSrcType == MS_VE_SRC_SUB) ||
       (pInputSrcInfo->eInputSrcType == MS_VE_SRC_MAIN))
    {
        eInputSrc = pInputSrcInfo->eInputSrcOfMixedSrc;
    }
    else
    {
        eInputSrc = pInputSrcInfo->eInputSrcType;
    }

    switch(eInputSrc)
    {
    case MS_VE_SRC_DTV:
        if(pInputSrcInfo->eInputSrcType == MS_VE_SRC_SUB)
        {
            //By now, DTV is always goes with capture mode in AV output
            //For SRC_SUB, we used in vecapture function, so use DE mode
            g_VEInfo.u8VE_DisplayStatus |= EN_VE_DEMODE;
        }
        break;

    case MS_VE_SRC_DSUB:
        g_VEInfo.u8VE_DisplayStatus |= EN_VE_RGB_IN;
        g_VEInfo.u8VE_DisplayStatus |= EN_VE_DEMODE;
        break;

    case MS_VE_SRC_COMP:
        g_VEInfo.u8VE_DisplayStatus |= EN_VE_DEMODE;
        break;

    case MS_VE_SRC_ATV:
    case MS_VE_SRC_CVBS0:
    case MS_VE_SRC_CVBS1:
    case MS_VE_SRC_CVBS2:
    case MS_VE_SRC_CVBS3:
    case MS_VE_SRC_SVIDEO:
        break;

    case MS_VE_SRC_HDMI_A:
    case MS_VE_SRC_HDMI_B:
    case MS_VE_SRC_HDMI_C:
        //g_VEInfo.u8VE_DisplayStatus |= EN_VE_RGB_IN; //For DVI/HDMI, control this in AP layer
        g_VEInfo.u8VE_DisplayStatus |= EN_VE_DEMODE;
        break;

    case MS_VE_SRC_SCALER:
        // set input color at HAL layer
        //g_VEInfo.u8VE_DisplayStatus |= EN_VE_RGB_IN;
        if(!g_VEInfo.bForceCaptureMode)
            g_VEInfo.u8VE_DisplayStatus |= EN_VE_DEMODE;
        else
            g_VEInfo.u8VE_DisplayStatus &= ~EN_VE_DEMODE;
        break;

    case MS_VE_SRC_DI:
        if(!g_VEInfo.bForceCaptureMode)
            g_VEInfo.u8VE_DisplayStatus |= EN_VE_DEMODE;
        else
            g_VEInfo.u8VE_DisplayStatus &= ~EN_VE_DEMODE;
        break;

    case MS_VE_SRC_DNR:
        if(!g_VEInfo.bForceCaptureMode)
            g_VEInfo.u8VE_DisplayStatus |= EN_VE_DEMODE;
        else
            g_VEInfo.u8VE_DisplayStatus &= ~EN_VE_DEMODE;
        break;

    case MS_VE_SRC_DTV_FROM_MVOP:
        g_VEInfo.u8VE_DisplayStatus |= EN_VE_DEMODE;
        break;

    case MS_VE_SRC_BT656:
        if(!g_VEInfo.bForceCaptureMode)
            g_VEInfo.u8VE_DisplayStatus |= EN_VE_DEMODE;
        else
            g_VEInfo.u8VE_DisplayStatus &= ~EN_VE_DEMODE;
        g_VEInfo.u8VE_DisplayStatus |= EN_VE_CCIR656_IN;
        break;

    default:
        break;
    }
    MSG_DRV_VE(printf("VE: %s:%d status=%x eInputSrc=%u\n", __FUNCTION__, __LINE__, g_VEInfo.u8VE_DisplayStatus, eInputSrc));
}


//------------------------------------------------------------------------------
/// Extended API for set the input source of video encoder(To Expand&Replace MDrv_VE_SwitchInputSource)
///
/// @param  -pSwitchInfo \b IN/OUT: the information of switching input destination of TV encodeer
/// @return @ref MS_SWITCH_VE_SRC_STATUS
//------------------------------------------------------------------------------
MS_SWITCH_VE_SRC_STATUS MDrv_VE_SetInputSource(PMS_VE_InputSrc_Info pInputSrcInfo)
{
    MSG_DRV_VE(printf("VE: %s, Src = %d, %d\n", __FUNCTION__, pInputSrcInfo->eInputSrcType, pInputSrcInfo->eInputSrcOfMixedSrc));

    g_VEInfo.InputSrcType = pInputSrcInfo->eInputSrcType;
    g_VEInfo.InputSrcOfMixedSrc = pInputSrcInfo->eInputSrcOfMixedSrc;
    MDrv_VE_SetSrcDispStatus(pInputSrcInfo);
    switch(pInputSrcInfo->eInputSrcType)
    {
    case MS_VE_SRC_COMP:
        Hal_VE_set_source_sync_inv(0x12, 0xFF);// set the sync polarity
        Hal_VE_set_sog(ENABLE); // enable sog
        break;

    case MS_VE_SRC_DTV_FROM_MVOP:
    case MS_VE_SRC_SUB:
    case MS_VE_SRC_DTV:
        Hal_VE_set_source_sync_inv(0x12, 0xFF);// set the sync polarity
        break;
    default:
        break;
    }
    if(g_VEInfo.u8VE_DisplayStatus & EN_VE_RGB_IN)
        Hal_VE_set_rgb_in(ENABLE); // enable RGB in
    else
        Hal_VE_set_rgb_in(DISABLE);// disable RGB in

    if(g_VEInfo.u8VE_DisplayStatus & EN_VE_CCIR656_IN)
        Hal_VE_set_ccir656_in(ENABLE);
    else
        Hal_VE_set_ccir656_in(DISABLE);

    Hal_VE_set_inputsource(pInputSrcInfo);
    return MS_VE_SWITCH_SRC_SUCCESS;
}

//------------------------------------------------------------------------------
/// set the input source of video encoder
///
/// @param  -pSwitchInfo \b IN/OUT: the information of switching input destination of TV encodeer
/// @return None
//------------------------------------------------------------------------------
void MDrv_VE_SwitchInputSource(PMS_Switch_VE_Src_Info pSwitchInfo)
{
    if((pSwitchInfo->InputSrcType == MS_VE_SRC_SUB) || (pSwitchInfo->InputSrcType == MS_VE_SRC_MAIN))
    {
        printf("!!!ALERT, FATAL ERROR: Non supported argument[%d] for [%s], try [MDrv_VE_SetInputSource]\n", pSwitchInfo->InputSrcType, __FUNCTION__);
    }
    else
    {
        MS_VE_InputSrc_Info InputSrcInfo;
        memset(&InputSrcInfo, 0, sizeof(MS_VE_InputSrc_Info));
        InputSrcInfo.u16Version = 0; //It is old version interface
        InputSrcInfo.eInputSrcType = pSwitchInfo->InputSrcType;
        InputSrcInfo.eInputSrcOfMixedSrc = MS_VE_SRC_NONE;
        MDrv_VE_SetInputSource(&InputSrcInfo);
    }
}


//------------------------------------------------------------------------------
/// control the output of video encoder
///
/// @param  -pOutputCtrl \b IN: the control information of VE
/// @return none
//------------------------------------------------------------------------------
void MDrv_VE_SetOutputCtrl(PMS_VE_Output_Ctrl pOutputCtrl)
{
    MS_BOOL bOutCCIR656 = FALSE, bOutTVE=FALSE, bFieldInv;
    MS_U16 u16EnVal;

    //(printf("-------- this is VE out 2009 /04/29-------\n") );

    if(pOutputCtrl->bEnable)
    {
        if(pOutputCtrl->OutputType == MS_VE_OUT_TVENCODER)
        {
            bOutTVE = TRUE;
        }
        else if(pOutputCtrl->OutputType == MS_VE_OUT_CCIR656)
        {
            //When CCIR is on, TV encoder is also needed to be enabled
            bOutTVE = TRUE;
            bOutCCIR656 = TRUE;
        }

        if((g_VEInfo.InputSrcType == MS_VE_SRC_SCALER) ||
           (g_VEInfo.InputSrcType == MS_VE_SRC_DI)     ||
           (g_VEInfo.InputSrcType == MS_VE_SRC_BT656)  ||
           (g_VEInfo.InputSrcType == MS_VE_SRC_DNR)    ||
           ((g_VEInfo.InputSrcType >= MS_VE_SRC_ATV) && (g_VEInfo.InputSrcType <= MS_VE_SRC_SVIDEO)) ||
           ((g_VEInfo.InputSrcOfMixedSrc >= MS_VE_SRC_ATV) && (g_VEInfo.InputSrcOfMixedSrc <= MS_VE_SRC_SVIDEO))
           )
        {
            if(!g_VEInfo.bForceCaptureMode)
                u16EnVal = BIT(7)|(bOutTVE << 2)|(bOutCCIR656 << 1)|(bOutCCIR656 << 6); // Enable DE mode when input source is Scaler OP/DI or VD
            else
                u16EnVal = (bOutTVE << 2)|(bOutCCIR656 << 1)|(bOutCCIR656 << 6); // Disable DE mode when input source is Scaler OP
        }
        else
        {
            u16EnVal = ((g_VEInfo.u8VE_DisplayStatus & EN_VE_DEMODE)<< 7) |
                       (bOutTVE << 2) |
                       (bOutCCIR656 << 1)|
                       (bOutCCIR656 << 6);
        }
        MSG_DRV_VE(printf("VE Power on: bOutCCIR656=0x%x, bOutTVE=0x%x, u16EnVal=0x%x \n", bOutCCIR656, bOutTVE, u16EnVal));
        if(bOutCCIR656)
        {
            Hal_VE_set_source_sync_inv(0, BIT(4));
            if(g_VEInfo.VideoSystem <= MS_VE_PAL_M)
            {   // NTSC
                Hal_VE_set_ccir656_out_pal(0);
            }
            else
            {   // PAL
                Hal_VE_set_ccir656_out_pal(1);
            }
        }

        // Field Invert
        if(g_VEInfo.InputSrcType == MS_VE_SRC_DTV_FROM_MVOP)
        {
            Hal_VE_set_field_inverse(0);
        }
        else if(g_VEInfo.InputSrcType == MS_VE_SRC_COMP)
        {
            bFieldInv = (g_VEInfo.u16V_SC_CapStart & 0x01) ?
                            ENABLE_VE_FIELD_INV : ~ENABLE_VE_FIELD_INV;
            bFieldInv = g_VEInfo.bSrcInterlace ? bFieldInv : 0;
            Hal_VE_set_field_inverse((bFieldInv & 0x01));
        }
        else
        {
            if(g_VEInfo.u8VE_DisplayStatus & EN_VE_DEMODE)
            {
                bFieldInv = (g_VEInfo.bSrcInterlace) ? ENABLE_VE_FIELD_INV : 0;
                Hal_VE_set_field_inverse((bFieldInv & 0x01));
            }
            else
            {
                bFieldInv = (g_VEInfo.u16V_SC_CapStart & 0x01) ?
                            ~ENABLE_VE_FIELD_INV : ENABLE_VE_FIELD_INV;
                bFieldInv = g_VEInfo.bSrcInterlace ? bFieldInv : 0;
                Hal_VE_set_field_inverse((bFieldInv & 0x01));
            }
        }

        Hal_VE_set_clk_on_off(ENABLE);
        Hal_VE_set_ctrl(u16EnVal);
        if(!g_VEInfo.bVECapture)
        {
            Hal_VE_set_vbi(ENABLE);
        }

        //OS_DELAY_TASK(300);
        Hal_VE_sofeware_reset(ENABLE); // software reset
        Hal_VE_set_reg_load(ENABLE);// load register,but not affect bit3(VBI output)

        //MsOS_DelayTask(5) ; // delay 5 ms
        OS_DELAY_TASK(30);

        // TELETEXT

        Hal_VE_set_reg_load(DISABLE);// load register,but not affect bit3(VBI output)
        OS_DELAY_TASK(5);
        Hal_VE_sofeware_reset(DISABLE); // software reset
        if(pOutputCtrl->OutputType != MS_VE_OUT_CAPTURE)
        {
            //For VEC, turn on it in kernel
            OS_DELAY_TASK(5);
            Hal_VE_set_ve_on_off(ENABLE);
        }
    }
    else
    {
        //Hal_VE_set_cvbs_buffer_out(0x0F, 0, 0, 0);
        MSG_DRV_VE(printf("VE Power off\n"));
        Hal_VE_set_ctrl(0);
        Hal_VE_set_vbi(DISABLE);
        Hal_VE_set_ve_on_off(DISABLE);
        Hal_VE_set_clk_on_off(DISABLE);
    }

    //workaround to pass parasoft test:
    //[2]A pointer parameter in a function prototype should be declared as pointer to const if the pointer is not used to modify the addressed object (JSF-118-4)
    pOutputCtrl->bEnable += 0;
}


//------------------------------------------------------------------------------
/// set the output video standard of video encoder
///
/// @param  -VideoSystem \b IN: the video standard
/// @return TRUE: supported and success,  FALSE: unsupported or unsuccess
//------------------------------------------------------------------------------
MS_BOOL MDrv_VE_SetOutputVideoStd(MS_VE_VIDEOSYS VideoSystem)
{
    MS_BOOL bRet;
    MSG_DRV_VE(printf("VE Standard: VideoSystem=%d\n", VideoSystem);)
    g_VEInfo.VideoSystem = VideoSystem;

    bRet = Hal_VE_set_output_video_std(VideoSystem);

    return bRet;
}

void MDrv_VE_SetWSSData(MS_BOOL bEn, MS_U16 u16WSSData) // 071204
{
//    MSG_DRV_VE(printf("WSSData=%d, data=%u\n", bEn, u16WSSData));
    Hal_VE_set_wss_data(bEn, u16WSSData);

}

MS_U16 MDrv_VE_GetWSSData(void)
{
    return Hal_VE_get_wss_data();
}

//Small adjust Hratio for HW design rule:
//tmp = ratio*input pixel, then tmp[31:11] = Number of output pixel, must adjust ratio to make this value even
//Ex : ratio = 0x155, input pixel = 1920, tmp = 0x9FD80, here tmp[11]=1,then set ratio=ratio+1
#define MAX_TRY_HRATIOAROUND 100
void MDrv_VE_HRatioAround(MS_U16 *pu16In_Hsize, MS_U32 *pu32H_Ratio)
{
    MS_U32 u32OutputPixel;
    MS_U8 i = 0xFF;
    do
    {
        i++;
        u32OutputPixel = *pu16In_Hsize * (*pu32H_Ratio+i);
    }while((u32OutputPixel & BIT(11)) && (i<MAX_TRY_HRATIOAROUND));

    if(i < MAX_TRY_HRATIOAROUND)
    {
        if(i != 0)
        {
            *pu32H_Ratio +=i;
            MSG_DRV_VE(printf("VE Scaling: HRatio Around=%d\n", (int)*pu32H_Ratio);)
        }
    }
    else
    {
        printf("!!!Alert: VE HRatio Around fail!!!\n");
    }

    //workaround to pass parasoft test:
    //[2]A pointer parameter in a function prototype should be declared as pointer to const if the pointer is not used to modify the addressed object (JSF-118-4)
    *pu16In_Hsize += 0;
}

VE_Result  MDrv_VE_Set_Customer_Scaling(MS_VE_CusScalingInfo *pstVECusScalingInfo)
{
    memcpy(&g_VEInfo.stVECusScalingInfo, pstVECusScalingInfo, sizeof(MS_VE_CusScalingInfo));
    return E_VE_OK;
}

void MDrv_VE_set_scaling_ratio(MS_U16 u16Out_Hsize, MS_U16 u16Out_Vsize)
{
    MS_U32 u32H_Ratio = 0, u32V_Ratio = 0;
    MS_U16 u16In_Vsize, u16In_Hsize;

    if(g_VEInfo.stVECusScalingInfo.bHCusScalingEnable==ENABLE)
    {
        u16In_Hsize = g_VEInfo.stVECusScalingInfo.u16HScalingsrc;
        u16Out_Hsize= g_VEInfo.stVECusScalingInfo.u16HScalingdst;
    }
    else
    {
        u16In_Hsize = g_VEInfo.u16H_CapSize;
    }
    if(g_VEInfo.stVECusScalingInfo.bVCusScalingEnable==ENABLE)
    {
        u16In_Vsize = g_VEInfo.stVECusScalingInfo.u16VScalingsrc;
        u16Out_Vsize= g_VEInfo.stVECusScalingInfo.u16VScalingdst;
    }
    else
    {
        u16In_Vsize = g_VEInfo.u16V_CapSize;
    }
    MSG_DRV_VE(printf("VE Scaling: bHCusScaling=%u, bVCusScaling=%u, In_Hsize =%d, In_Vsize = %d, Out_Hsize =%d, Out_Vsize = %d,\n",
                          g_VEInfo.stVECusScalingInfo.bHCusScalingEnable, g_VEInfo.stVECusScalingInfo.bVCusScalingEnable, u16In_Hsize, u16In_Vsize, u16Out_Hsize, u16Out_Vsize);)
    if(u16Out_Hsize != u16In_Hsize)
    {
        if(u16In_Hsize > u16Out_Hsize)
        {
            u32H_Ratio = (MS_U16)VE_H_SCALE_DOWN_RATIO(u16In_Hsize, u16Out_Hsize);
            MDrv_VE_HRatioAround(&u16In_Hsize, &u32H_Ratio);
            Hal_VE_set_h_scaling(ENABLE, (MS_U16)u32H_Ratio);

            if (g_VECap.bSupport_UpScale)
            {
                Hal_VE_set_h_upscaling(ENABLE, 0x10000);    // set to no-scale
            }
        }
        else
        {
            if (!g_VECap.bSupport_UpScale)
            {
                //VE not support upscale,just bypass it
                if(g_VEInfo.u16H_CapSize ==544)//DVB DTV 544x576 needs fine tune by hand
                {
                    Hal_VE_set_h_scaling(ENABLE, 0x7FF);
                }
                else
                {
                    Hal_VE_set_h_scaling(DISABLE, 0x0000);
                }
            }
            else
            {
                //do h upscale
                u32H_Ratio = VE_H_SCALE_UP_RATIO(u16In_Hsize, u16Out_Hsize);
                Hal_VE_set_h_upscaling(ENABLE, u32H_Ratio);

                Hal_VE_set_h_scaling(DISABLE, 0x0000);
                MSG_DRV_VE(printf("VE H upscale ratio: u32H_Ratio =%d\n", (int)u32H_Ratio);)
            }
        }
    }
    else
    {
        Hal_VE_set_h_scaling(DISABLE, 0x0000);
        if (g_VECap.bSupport_UpScale)
        {
            Hal_VE_set_h_upscaling(ENABLE, 0x10000);    // set to no-scale
        }
    }

    if(u16Out_Vsize != u16In_Vsize)
    {
        if(u16In_Vsize > u16Out_Vsize)
        {
            u32V_Ratio = (MS_U16)VE_V_SCALE_DOWN_RATIO(u16In_Vsize, u16Out_Vsize);
            if(g_VEInfo.stVECusScalingInfo.bVCusScalingEnable==ENABLE)
            {
                Hal_VE_set_v_scaling(ENABLE, (MS_U16)u32V_Ratio);//For customer scaling, just set it as requested
            }
            else
            {
                Hal_VE_set_v_scaling_Traditional(ENABLE, (MS_U16)u32V_Ratio);//For traditional V scaling, need considering the recevier's overscan
            }

            if (g_VECap.bSupport_UpScale)
            {
                Hal_VE_set_v_upscaling(ENABLE, 0x10000);
            }
        }
        else
        {
            if (!g_VECap.bSupport_UpScale)
            {
                //VE not support upscale,just bypass it
                Hal_VE_set_v_scaling(DISABLE, 0x0000);
            }
            else
            {
                //do v upscale
                u32V_Ratio = VE_V_SCALE_UP_RATIO(u16In_Vsize, u16Out_Vsize);
                Hal_VE_set_v_upscaling(ENABLE, u32V_Ratio);

                Hal_VE_set_v_scaling(DISABLE, 0x0000);
                MSG_DRV_VE(printf("VE V upscale ratio: u32V_Ratio =%d\n", (int)u32V_Ratio);)
            }
        }
    }
    else
    {
        if(g_VEInfo.u16H_CapSize ==544)//DVB DTV 544x576 needs fine tune by hand
        {
            Hal_VE_set_v_scaling(ENABLE, 0x0750);
        }
        else
        {
            Hal_VE_set_v_scaling(DISABLE, 0x0000);
            if (g_VECap.bSupport_UpScale)
            {
                Hal_VE_set_v_upscaling(ENABLE, 0x10000);
            }
        }
    }
    MSG_DRV_VE(printf("VE Scaling: u32H_Ratio =%d, u32V_Ratio = %d \n", (int)u32H_Ratio, (int)u32V_Ratio);)
}

//------------------------------------------------------------------------------
/// VE Set display windows
/// @return none
//------------------------------------------------------------------------------
void MDrv_VE_set_display_window(MS_VE_WINDOW_TYPE stDispWin)
{
    MS_U16 u16Out_Hsize= (((g_VEInfo.bHDuplicate) && (g_VEInfo.InputSrcType == MS_VE_SRC_DTV)) ? 704 : 720);
    MS_U16 u16Out_Vsize = ((g_VEInfo.VideoSystem <= MS_VE_PAL_M) ? 480 : 576);
    MS_U32 u32MIUOffsetAddr;

    //Check display window location and size.
    if(stDispWin.x > u16Out_Hsize) {stDispWin.x = 0 ; MSG_DRV_VE(printf("x >> width,please check x range \n"));}
    if(stDispWin.y > u16Out_Vsize) {stDispWin.y = 0 ; MSG_DRV_VE(printf("y >> height,please check y range \n"));}
    if(stDispWin.x + stDispWin.width > u16Out_Hsize) {stDispWin.width = u16Out_Hsize ; MSG_DRV_VE(printf("width >> Hsize,please check width size \n"));}
    if(stDispWin.y + stDispWin.height > u16Out_Vsize) {stDispWin.height = u16Out_Vsize ; MSG_DRV_VE(printf("height >> Vsize,please check height size \n"));}
    MSG_DRV_VE(printf("VE Set Display Window: x/y/w/h = %u, %u, %u, %u\n", stDispWin.x, stDispWin.y, stDispWin.width, stDispWin.height);)

    if(MApi_VE_R2BYTE_MSK(MS_VE_REG_BANK_3B, 0x35, BIT(3)) == 0)
    {
        stDispWin.y /=2;
    }
    u32MIUOffsetAddr = 2 * (stDispWin.x + stDispWin.y * u16Out_Hsize);
    MDrv_VE_set_scaling_ratio(stDispWin.width,stDispWin.height);
    u32MIUOffsetAddr += g_VEInfo.u32MiuBaseAddr;
    Hal_VE_Set_winodw_offset(u32MIUOffsetAddr);
}

//------------------------------------------------------------------------------
/// @brief VE be set display window (backend), after MIU
///
/// @argument:
//          - MS_VE_WINDOW_TYPE - display window
/// @return VE_Result
//------------------------------------------------------------------------------
VE_Result MDrv_VE_set_be_display_window(MS_VE_WINDOW_TYPE stDispWin)
{
    MS_U16 u16Out_Hsize= (((g_VEInfo.bHDuplicate) && (g_VEInfo.InputSrcType == MS_VE_SRC_DTV)) ? 704 : 720);
    MS_U16 u16Out_Vsize = ((g_VEInfo.VideoSystem <= MS_VE_PAL_M) ? 480 : 576);
    VE_Result ret = E_VE_OK;

    if (!g_VECap.bSupport_CropMode) {
        MSG_DRV_VE(printf("[%s] is not supported in this chip\n",
                    __FUNCTION__));
        ret = E_VE_NOT_SUPPORT;
        goto fail;
    }

    //Check display window location and size.
    if(stDispWin.x > u16Out_Hsize) {
        MSG_DRV_VE(printf("x >> width,please check x range \n"));
        ret = E_VE_FAIL;
        goto fail;
    }
    if(stDispWin.y > u16Out_Vsize) {
        MSG_DRV_VE(printf("y >> height,please check y range \n"));
        ret = E_VE_FAIL;
        goto fail;
    }
    if(stDispWin.x + stDispWin.width > u16Out_Hsize) {
        MSG_DRV_VE(printf("width >> Hsize,please check width size \n"));
        ret = E_VE_FAIL;
        goto fail;
    }
    if(stDispWin.y + stDispWin.height > u16Out_Vsize) {
        MSG_DRV_VE(printf("height >> Vsize,please check height size \n"));
        ret = E_VE_FAIL;
        goto fail;
    }
    MSG_DRV_VE(printf("VE Set Display Window: x/y/w/h = %u, %u, %u, %u\n", stDispWin.x, stDispWin.y, stDispWin.width, stDispWin.height);)

    MDrv_VE_set_scaling_ratio(stDispWin.width, stDispWin.height);
    Hal_VE_Set_HScale_Output_Line_Number(stDispWin.width);
    Hal_VE_Set_VScale_Output_Line_Number((stDispWin.height) / 2);   // field size
    Hal_VE_Set_Crop_OSD_Offset(stDispWin.x, (stDispWin.y / 2)); // field size

    return E_VE_OK;
fail:
    printf("Error: [%s] failed, errno: %d\n", __FUNCTION__, ret);
    return ret;
}

void MDrv_VE_set_frc(MS_U16 u16VInputFreq)
{
    MS_U32 u32FullNum, u32EmptyNum;
    MS_U32 u32FrameSize;
    MS_U16 u16Out_FR, u16In_FR;
    MS_U16 b3FrameMode = FALSE;

    if(g_VEInfo.VideoSystem <= MS_VE_PAL_M)
    {
        u32FrameSize = VE_NTSC_FRAME_SIZE;
        u16Out_FR = 2997;
    }
    else
    {
        u32FrameSize = VE_PAL_FRAME_SIZE;
        u16Out_FR = 2500;
    }
    if(g_VEInfo.bVECapture)
    {
        //FrameSize = 720*Height*2/MIU Length
        u32FrameSize = (720*g_VEInfo.u16Outputheight/(BYTE_PER_WORD>>1));//The pitch is always 720pixel for VE
        Hal_VE_set_field_size((MS_U16)(u32FrameSize/2));
    }
    u16In_FR =  (u16VInputFreq/2*10) ;
    MSG_DRV_VE(printf("---VE: InputVFreq=%u, In_FR=%u, Out_FR=%u\n", u16VInputFreq, u16In_FR, u16Out_FR);)
    if(g_VEInfo.bVECapture)
    {
        if(g_VEInfo.u32MemSize/3 >= u32FrameSize)
        {
            b3FrameMode = TRUE;
        }
        Hal_VE_set_frc(DISABLE, 0xFFFF, 0, g_VEInfo.bSrcInterlace, (MS_BOOL)b3FrameMode);
    }
#if 0 // for tearing issue, VE need to turn on FRC function
    else if(u16In_FR > (u16Out_FR - VE_FRC_TOLERANT) && u16In_FR<(u16Out_FR + VE_FRC_TOLERANT) )
    {
        // No need to do FRC
        MSG_DRV_VE(printf("---VE: Disable FRC\n"););
        Hal_VE_set_frc(DISABLE, 0, 0, DISABLE);
    }
#else
    else if( ( u16In_FR > (u16Out_FR - VE_FRC_TOLERANT) ) && ( u16In_FR<(u16Out_FR + VE_FRC_TOLERANT) ) )
    {
        // Default always to enable FRC (avoid tearing)
        u32FullNum = u32FrameSize * 3/2;
        u32EmptyNum = u32FrameSize / 2;
        MSG_DRV_VE(printf("---VE: default ENABLE FRC,u32FullNum=%lu, u32EmptyNum=%lu\n", u32FullNum, u32EmptyNum););
        Hal_VE_set_frc(ENABLE, u32FullNum, u32EmptyNum, g_VEInfo.bSrcInterlace, (MS_BOOL)b3FrameMode);
    }
#endif
    else if(u16In_FR < u16Out_FR)
    {
        u32FullNum  = u32FrameSize * 19 / 10 ;
        u32EmptyNum = (MS_U32)(u16Out_FR - u16In_FR) * u32FrameSize / (MS_U32)u16Out_FR ;
        MSG_DRV_VE(printf("---VE: Enable FRC, u32FullNum=%lu, u32EmptyNum=%lu\n", u32FullNum, u32EmptyNum););
        Hal_VE_set_frc(ENABLE, u32FullNum, u32EmptyNum, g_VEInfo.bSrcInterlace, (MS_BOOL)b3FrameMode);
    }
    else
    {
        u32FullNum = u32FrameSize +
                     ((MS_U32)u16Out_FR  * u32FrameSize / u16In_FR);
        u32EmptyNum = u32FrameSize / 10 ;
        MSG_DRV_VE(printf("---VE: Enable FRC, u32FullNum=%lu, u32EmptyNum=%lu\n", u32FullNum, u32EmptyNum););
        Hal_VE_set_frc(ENABLE, u32FullNum, u32EmptyNum, g_VEInfo.bSrcInterlace, (MS_BOOL)b3FrameMode);
    }
    MSG_DRV_VE(printf("---VE: bVECapture=%u, u32FrameSize=%lu, b3FrameMode=%u\n", g_VEInfo.bVECapture, u32FrameSize, b3FrameMode);)
}

void MDrv_VE_set_capture_window(void)
{
	MS_U16 u16Out_Vsize, u16Out_Hsize,u16V_CapSize=0;
    MS_U16 u16Tmp, u16Tmp2;

    if(g_VEInfo.bVECapture)
    {
        u16Out_Hsize = g_VEInfo.u16OutputWidth;
        u16Out_Vsize = g_VEInfo.u16Outputheight;
    }
    else if(!g_VEInfo.bForceCaptureMode)
    {
        u16Out_Hsize = ((g_VEInfo.InputSrcType == MS_VE_SRC_DTV) && (g_VEInfo.bHDuplicate)) ? 704 : 720;
        u16Out_Vsize = (g_VEInfo.VideoSystem <= MS_VE_PAL_M) ? 480 : 576;
    }
    else
    {
        u16Out_Hsize = g_VEInfo.u16H_CapSize;
        u16Out_Vsize = g_VEInfo.u16V_CapSize;
    }

    if(g_VEInfo.InputSrcType == MS_VE_SRC_DI)
    {
        MSG_DRV_VE(printf("VE InputSrcType = MS_VE_SRC_DI \n"));
        u16Out_Hsize = g_VEInfo.u16H_CapSize;
        u16Out_Vsize = g_VEInfo.u16V_CapSize;
    }

    g_VEInfo.u8VE_DisplayStatus &= ~(EN_VE_INVERSE_HSYNC);


    MSG_DRV_VE(printf("Out: Hsize=%d, Vsize=%d \n", u16Out_Hsize, u16Out_Vsize));
    MSG_DRV_VE(printf("VE Cap: Hstart=%d, Hsize =%d \n", g_VEInfo.u16H_CapStart, g_VEInfo.u16H_CapSize));
    //MSG_DRV_VE(printf("SC Cap: Hstart=%d, Hsize =%d \n", g_SrcInfo.u16H_CapStart, g_SrcInfo.u16H_CapSize));
    MSG_DRV_VE(printf("VE Cap: Vstart=%d, Vsize =%d \n", g_VEInfo.u16V_CapStart, g_VEInfo.u16V_CapSize));
    //MSG_DRV_VE(printf("SC Cap: Vstart=%d, Vsize =%d \n", g_SrcInfo.u16V_CapStart, g_SrcInfo.u16V_CapSize));
    MSG_DRV_VE(printf("[%s][%d]_gbIsCaptureMode = %d\n", __FUNCTION__, __LINE__, g_VEInfo.bForceCaptureMode);)

    // Horizontal
#if (ENABLE_VE_SUBTITLE)

    if((g_VEInfo.VideoSystem <= MS_VE_PAL_M) &&
        (g_VEInfo.u16V_CapSize <= 480))
    {
        u16Tmp2 = 0x17; //fine tune for ntsc 480i case, which is too left
    }
    else
    {
        u16Tmp2 = 0;
    }

    if(g_VEInfo.u16H_CapSize < u16Out_Hsize)
    {
        g_VEInfo.u8VE_DisplayStatus &= ~ EN_VE_DEMODE;
        g_VEInfo.u8VE_DisplayStatus |= EN_VE_INVERSE_HSYNC;

        u16Tmp = u16Out_Hsize - g_VEInfo.u16H_CapSize;
        g_VEInfo.u16H_CapStart =  g_VEInfo.u16H_CapStart - u16Tmp/2 + u16Tmp2 + 9 ;
        g_VEInfo.u16H_CapSize = u16Out_Hsize;
        MSG_DRV_VE(printf("--1 temp %d, u16Tmp2 %d, H_start %d\n",u16Tmp,u16Tmp2,g_VEInfo.u16H_CapStart));
    }
    else if( ( g_VEInfo.u16H_CapSize == u16Out_Hsize ) &&
            ( g_VEInfo.u16H_CapSize > g_VEInfo.u16H_SC_CapSize )) //Same condition with Vsize
    {
        g_VEInfo.u8VE_DisplayStatus &= ~ EN_VE_DEMODE;
        g_VEInfo.u8VE_DisplayStatus |= EN_VE_INVERSE_HSYNC;

        u16Tmp = g_VEInfo.u16H_CapSize - g_VEInfo.u16H_SC_CapSize;
        g_VEInfo.u16H_CapStart = g_VEInfo.u16H_SC_CapStart - u16Tmp/2 + u16Tmp2 + 9;
        MSG_DRV_VE(printf("--2 temp %d, u16Tmp2 %d, H_start %d\n",u16Tmp, u16Tmp2, g_VEInfo.u16H_CapStart));
    }
    else
    {
        //g_VEInfo.u8VE_DisplayStatus &= ~ EN_VE_DEMODE;
        g_VEInfo.u8VE_DisplayStatus |= EN_VE_INVERSE_HSYNC;

        u16Tmp = g_VEInfo.u16H_CapSize - g_VEInfo.u16H_SC_CapSize;
        g_VEInfo.u16H_CapStart = g_VEInfo.u16H_SC_CapStart - u16Tmp/2 + u16Tmp2 + 9;
		MSG_DRV_VE(printf("--3 temp %d, u16Tmp2 %d , H_start %d\n",u16Tmp, u16Tmp2,g_VEInfo.u16H_CapStart));
    }

#else
    if(g_VEInfo.u16H_CapSize < u16Out_Hsize)
    {
        g_VEInfo.u8VE_DisplayStatus &= ~ EN_VE_DEMODE;
        g_VEInfo.u8VE_DisplayStatus |= EN_VE_INVERSE_HSYNC;

        u16Tmp = u16Out_Hsize - g_VEInfo.u16H_CapSize;
        g_VEInfo.u16H_CapStart -= (u16Tmp/2);
        g_VEInfo.u16H_CapSize += u16Tmp;
    }
#endif

    // Vertical
#if (ENABLE_VE_SUBTITLE)
        if((g_VEInfo.VideoSystem <= MS_VE_PAL_M) &&
            (g_VEInfo.u16V_CapSize <= 480))
        {
            if(g_VEInfo.bSrcInterlace)
            {
                u16Tmp2 = 0x11; //fine tune for ntsc 480i case, which will leave a black band on the bottom
            }
            else
            {
                u16Tmp2 = 0x5; //fine tune for ntsc 480p case, which will leave a black band on the bottom
            }
        }
        else if((g_VEInfo.VideoSystem <= MS_VE_PAL_M) &&
            (g_VEInfo.u16V_CapSize <= 720))
        {
                u16Tmp2 = 0x9; //fine tune for ntsc 720p case, which will leave a black band on the bottom
        }
        else if((g_VEInfo.VideoSystem <= MS_VE_PAL_M) &&
            (g_VEInfo.u16V_CapSize <= 1080))
        {
                u16Tmp2 = 0x9; //fine tune for ntsc 1080i case, which will leave a black band on the bottom
        }
        else
        {
            u16Tmp2 = 0;
        }

        if(g_VEInfo.u16V_CapSize < u16Out_Vsize)
        {
            g_VEInfo.u8VE_DisplayStatus &= ~ EN_VE_DEMODE;
            g_VEInfo.u8VE_DisplayStatus |= EN_VE_INVERSE_HSYNC;

            u16Tmp = u16Out_Vsize - g_VEInfo.u16V_CapSize;
            if(g_VEInfo.u16V_CapStart > u16Tmp/2 + u16Tmp2)
            {
                g_VEInfo.u16V_CapStart =  g_VEInfo.u16V_CapStart - u16Tmp/2 - u16Tmp2;
            }
            else
            {
                g_VEInfo.u16V_CapStart = 0;
            }
            g_VEInfo.u16V_CapSize = u16Out_Vsize;
			MSG_DRV_VE(printf("--1 temp %d, u16Tmp2 %d, V_start %d\n",u16Tmp,u16Tmp2,g_VEInfo.u16V_CapStart));
        }
        else if( ( g_VEInfo.u16V_CapSize == u16Out_Vsize ) &&
                ( g_VEInfo.u16V_CapSize > g_VEInfo.u16V_SC_CapSize ))
        {
            g_VEInfo.u8VE_DisplayStatus &= ~ EN_VE_DEMODE;
            g_VEInfo.u8VE_DisplayStatus |= EN_VE_INVERSE_HSYNC;

            u16Tmp = g_VEInfo.u16V_CapSize - g_VEInfo.u16V_SC_CapSize;
            if(g_VEInfo.u16V_SC_CapStart > u16Tmp/2 + u16Tmp2)
            {
                g_VEInfo.u16V_CapStart = g_VEInfo.u16V_SC_CapStart - u16Tmp/2 - u16Tmp2;
            }
            else
            {
                g_VEInfo.u16V_CapStart = 0;
            }
            MSG_DRV_VE(printf("--2 temp %d, u16Tmp2 %d, V_start %d\n",u16Tmp,u16Tmp2,g_VEInfo.u16V_CapStart));
        }
        else
        {
            //g_VEInfo.u8VE_DisplayStatus &= ~ EN_VE_DEMODE;
            g_VEInfo.u8VE_DisplayStatus |= EN_VE_INVERSE_HSYNC;
            u16Tmp = g_VEInfo.u16V_CapSize - g_VEInfo.u16V_SC_CapSize;

            MSG_DRV_VE(printf("--g_VEInfo.u16V_CapStart %x\n",g_VEInfo.u16V_CapStart));
            MSG_DRV_VE(printf("--u16Tmp %d, u16Tmp2 %d\n",u16Tmp, u16Tmp2));
            if(g_VEInfo.u16V_SC_CapStart > (u16Tmp/2 + u16Tmp2))
            {
                g_VEInfo.u16V_CapStart = g_VEInfo.u16V_SC_CapStart - u16Tmp/2 - u16Tmp2;
            }
            else
            {
                g_VEInfo.u16V_CapStart = 0;
            }
			MSG_DRV_VE(printf("--3 temp %d , u16Tmp2 %d, V_start %d\n",u16Tmp, u16Tmp2 ,g_VEInfo.u16V_CapStart));
        }

        MSG_DRV_VE(printf("-- %d\n",g_VEInfo.u16V_CapStart + g_VEInfo.u16V_CapSize - 1));

        if(g_VEInfo.u16V_CapStart> (g_VEInfo.u16V_CapStart + g_VEInfo.u16V_CapSize - 1))
        {
           Hal_VE_set_cvbs_buffer_out(0x0F,0x00, 0x00, 0x00);//disable
        }
#else
        if(g_VEInfo.u16V_CapSize < u16Out_Vsize)
        {
            g_VEInfo.u8VE_DisplayStatus &= ~ EN_VE_DEMODE;
            g_VEInfo.u8VE_DisplayStatus |= EN_VE_INVERSE_HSYNC;

            u16Tmp = u16Out_Vsize - g_VEInfo.u16V_CapSize;
            g_VEInfo.u16V_CapStart -= (u16Tmp/2);
            g_VEInfo.u16V_CapSize += u16Tmp;
        }
        if(g_VEInfo.u16V_CapStart > (g_VEInfo.u16V_CapStart + g_VEInfo.u16V_CapSize - 1))
            Hal_VE_set_cvbs_buffer_out(0x0F,0x00, 0x00, 0x00);//disable
#endif

    MSG_DRV_VE(printf("VE status=%x \n", g_VEInfo.u8VE_DisplayStatus));
    MSG_DRV_VE(printf("VE Cap: Hstart=%d, Hsize =%d \n", g_VEInfo.u16H_CapStart, g_VEInfo.u16H_CapSize));
    MSG_DRV_VE(printf("VE Cap: Vstart=%d, Vsize =%d \n", g_VEInfo.u16V_CapStart, g_VEInfo.u16V_CapSize));

    /*
    For input source is component and HDMI all use DE mode.
    */
    if((g_VEInfo.InputSrcType >= MS_VE_SRC_COMP) && (g_VEInfo.InputSrcType <= MS_VE_SRC_HDMI_C))
    {
        g_VEInfo.u8VE_DisplayStatus |=  EN_VE_DEMODE;
    }

#if 1
    if(g_VEInfo.bSrcInterlace)
    {
        u16V_CapSize = g_VEInfo.u16V_CapSize / 2;
        MSG_DRV_VE(printf("VE Cap: Interlace mode, Change Vsize =%d \n", u16V_CapSize));
    }
    else
    {
        u16V_CapSize = g_VEInfo.u16V_CapSize;
    }

    if(g_VEInfo.u8VE_DisplayStatus & EN_VE_CCIR656_IN)
        Hal_VE_set_capture_window(0x000, 0x7FF, 0x10, 0xFF);
    else if((g_VEInfo.u8VE_DisplayStatus & EN_VE_DEMODE) && (!g_VEInfo.bForceCaptureMode)) // Force DE mode for source coming Scaler OP
        Hal_VE_set_capture_window(0x000, 0x7FF, 0x000, 0x7FF);
    else
    {
        Hal_VE_set_capture_window(g_VEInfo.u16H_CapStart,
        g_VEInfo.u16H_CapStart + g_VEInfo.u16H_CapSize - 1,
        g_VEInfo.u16V_CapStart,
        g_VEInfo.u16V_CapStart + u16V_CapSize - 1);
    }
#else
    if(g_VEInfo.u8VE_DisplayStatus & EN_VE_CCIR656_IN)
        Hal_VE_set_capture_window(0x000, 0x7FF, 0x10, 0xFF);
    else if((g_VEInfo.u8VE_DisplayStatus & EN_VE_DEMODE || g_VEInfo.InputSrcType == MS_VE_SRC_SCALER) && (!_gbIsCaptureMode)) // Force DE mode for source coming Scaler OP if capture mode is disable
        Hal_VE_set_capture_window(0x000, 0x7FF, 0x000, 0x7FF);
    else
    {
        Hal_VE_set_capture_window(g_VEInfo.u16H_CapStart,
                            g_VEInfo.u16H_CapStart + g_VEInfo.u16H_CapSize - 1,
                            g_VEInfo.u16V_CapStart,
                            g_VEInfo.u16V_CapStart + g_VEInfo.u16V_CapSize - 1);
    }
#endif

    if(g_VEInfo.u8VE_DisplayStatus & EN_VE_INVERSE_HSYNC)
        Hal_VE_set_hsync_inverse(1); //enable inverse Hsync
    else
        Hal_VE_set_hsync_inverse(0); //disable inverse Hsync


    if(g_VEInfo.bSrcInterlace)
        Hal_VE_set_source_interlace(ENABLE);
    else
        Hal_VE_set_source_interlace(DISABLE);
}

//------------------------------------------------------------------------------
/// @brief MDrv_VE_Get_Ext_Caps: get VE extented capability
///
/// @argument:
/// @return none
//------------------------------------------------------------------------------
VE_Result MDrv_VE_Get_Ext_Caps(MS_VE_Ext_Cap *cap)
{
    cap->bSupport_UpScale = g_VECap.bSupport_UpScale;
    cap->bSupport_CropMode = g_VECap.bSupport_CropMode;

    return E_VE_OK;
}

//------------------------------------------------------------------------------
/// @brief MDrv_VE_set_crop_window, set up crop window (crop video frame in MIU)
///
/// @argument:
//          - MS_VE_WINDOW_TYPE - crop window
/// @return VE_Result
//------------------------------------------------------------------------------
VE_Result MDrv_VE_set_crop_window(MS_VE_WINDOW_TYPE stCropWin)
{
    MS_U16 u16Out_Hsize= (((g_VEInfo.bHDuplicate) && (g_VEInfo.InputSrcType == MS_VE_SRC_DTV)) ? 704 : 720);
    MS_U16 u16Out_Vsize = ((g_VEInfo.VideoSystem <= MS_VE_PAL_M) ? 480 : 576);
    MS_U32 u32MIUOffsetAddr = 0;
    MS_U8 u8PackPerLine = 0;
    MS_U16 u16PixelPerUnit = 0;
    MS_U16 u16FieldLineNum = 0;
    VE_Result ret = E_VE_OK;

    if (!g_VECap.bSupport_CropMode) {
        MSG_DRV_VE(printf("[%s] is not supported in this chip\n",
                    __FUNCTION__));
        ret = E_VE_NOT_SUPPORT;
        goto fail;
    }

    //Check crop window location and size.
    if(stCropWin.x > u16Out_Hsize) {
        MSG_DRV_VE(printf("[%s]x >> width,please check x range\n",
                    __FUNCTION__));
        ret = E_VE_FAIL;
        goto fail;
    }
    if(stCropWin.y > u16Out_Vsize) {
        MSG_DRV_VE(printf("[%s]y >> height,please check y range\n",
                    __FUNCTION__));
        ret = E_VE_FAIL;
        goto fail;
    }

    if (Hal_VE_Get_DRAM_Format() == MS_VE_DRAM_FORMAT_Y8C8) {
        // Y8C8
        u16PixelPerUnit = 4;    // 64 / 16 = 4
    } else {
        // Y8C8M4
        u16PixelPerUnit = 3;    // 64 / 20 = 3
        // NOTE: In Y8C8M4 format, there are at most 3 pixel per access unit
        // (64bit). There are 4 unused bits every 64 bits.
    }
    // step 0: crop left part(stCropWin.x)
    // step 1: top part(stCropWin.y * u16Out_HSize)
    // NOTE: there are PAL: 288 / NTSC:240 lines in one field.
    u32MIUOffsetAddr = (stCropWin.x + (stCropWin.y / 2) * u16Out_Hsize) / u16PixelPerUnit * 8;
    u32MIUOffsetAddr += g_VEInfo.u32MiuBaseAddr;
    Hal_VE_SetMemAddr(&u32MIUOffsetAddr, NULL);

    // seep 2: crop right part (stCropWin.width)
    // unit: MIU bus width: 64 bits
    u8PackPerLine = (stCropWin.width / u16PixelPerUnit) - 1;
    Hal_VE_Set_Pack_Num_Per_Line(u8PackPerLine);

    MSG_DRV_VE(printf("step 2: pack number per line: %u\n",
                (unsigned int)u8PackPerLine));

    // seep 3: crop bottom part (stCropWin.height)
    // line per field: PAL: 288, NTSC: 240
    u16FieldLineNum = ((stCropWin.height) / 2) - 1;
    Hal_VE_Set_Field_Line_Number(u16FieldLineNum);
    MSG_DRV_VE(printf("step 3: field line number: %u\n",
                (unsigned int)u16FieldLineNum));

    // update Cap size for MDrv_VE_set_scaling_ratio
    g_VEInfo.u16H_CapSize = stCropWin.width;
    g_VEInfo.u16V_CapSize = stCropWin.height;
    return E_VE_OK;
fail:
    printf("Error: [%s] failed, errno: %d\n", __FUNCTION__, ret);
    return ret;
}

void MDrv_VE_SetMode(MS_VE_Set_Mode_Type *pVESetMode)
{
    g_VEInfo.u16H_CapSize  = pVESetMode->u16H_CapSize;
    g_VEInfo.u16V_CapSize  = pVESetMode->u16V_CapSize;
    g_VEInfo.u16H_CapStart = pVESetMode->u16H_CapStart;
    g_VEInfo.u16V_CapStart = pVESetMode->u16V_CapStart;

    g_VEInfo.u16H_SC_CapSize  = pVESetMode->u16H_SC_CapSize;
    g_VEInfo.u16V_SC_CapSize  = pVESetMode->u16V_SC_CapSize;
    g_VEInfo.u16H_SC_CapStart = pVESetMode->u16H_SC_CapStart;
    g_VEInfo.u16V_SC_CapStart = pVESetMode->u16V_SC_CapStart;
    g_VEInfo.bHDuplicate = pVESetMode->bHDuplicate;
    g_VEInfo.bSrcInterlace = pVESetMode->bSrcInterlace;

    MSG_DRV_VE(printf("VE SetMode, VFreq=%d, Hdupliate=%d, Interlace=%d\n",
        pVESetMode->u16InputVFreq, g_VEInfo.bHDuplicate, g_VEInfo.bSrcInterlace));

    MSG_DRV_VE(printf("VE->H:(%d, %d), V:(%d, %d)\n",
        g_VEInfo.u16H_CapStart, g_VEInfo.u16H_CapSize,
        g_VEInfo.u16V_CapStart, g_VEInfo.u16V_CapSize));

    MSG_DRV_VE(printf("SC->H:(%d, %d), V:(%d, %d)\n",
        g_VEInfo.u16H_SC_CapStart, g_VEInfo.u16H_SC_CapSize,
        g_VEInfo.u16V_SC_CapStart, g_VEInfo.u16V_SC_CapSize));
    if(g_VEInfo.bVECapture == FALSE)
    {
        g_VEInfo.u16OutputWidth = (( ( g_VEInfo.bHDuplicate ) && ( g_VEInfo.InputSrcType == MS_VE_SRC_DTV ) ) ? 704 : 720);
        g_VEInfo.u16Outputheight= ((g_VEInfo.VideoSystem <= MS_VE_PAL_M) ? 480 : 576);
    }
    MDrv_VE_set_capture_window(); // set capture window
    MDrv_VE_set_scaling_ratio(g_VEInfo.u16OutputWidth, g_VEInfo.u16Outputheight);    // set scaling ratio
    MDrv_VE_set_frc(pVESetMode->u16InputVFreq); // set frc
}

void MDrv_VE_SetBlackScreen(MS_BOOL bEn)
{
    MSG_DRV_VE(printf("VE Setblackscreen %x\n", bEn));
    Hal_VE_set_blackscreen(bEn);
}

MS_BOOL MDrv_VE_IsBlackScreenEnabled(void)
{
    return Hal_VE_is_blackscreen_enabled();
}

void MDrv_VE_GetCaps(VE_Caps* pCaps)
{
    pCaps->bNTSC = TRUE;
    pCaps->bPAL  = TRUE;
	pCaps->bVESupported = VE_SUPPORTED;
}

//------------------------------------------------------------------------------
/// VE restart TVE to read data from TT buffer
/// @return none
//------------------------------------------------------------------------------
void MDrv_VE_EnableTtx(MS_BOOL bEnable)
{
    Hal_VE_EnableTtx(bEnable);
}

#define FILED_PER_FIRE 5
//------------------------------------------------------------------------------
/// VE set TT buffer address
/// @return none
//------------------------------------------------------------------------------
void MDrv_VE_SetTtxBuffer(MS_U32 u32StartAddr, MS_U32 u32Size)
{

    //Hal_VE_Set_VbiTtxActiveLines(linePerField);

    // MIU size and base adress
    Hal_VE_Set_ttx_Buffer(u32StartAddr, u32Size);

    // interrupt force & interrupt clear
    Hal_VE_Clear_ttxReadDoneStatus();

    // start next vbi ttx read
    Hal_VE_EnableTtx(ENABLE);
}

//------------------------------------------------------------------------------
/// VE clear TT buffer read done status
/// @return none
//------------------------------------------------------------------------------
void MDrv_VE_ClearTtxReadDoneStatus(void)
{
    Hal_VE_Clear_ttxReadDoneStatus();
}

//------------------------------------------------------------------------------
/// VE TT buffer read done status
/// @return TRUE/FALSE
//------------------------------------------------------------------------------
MS_BOOL MDrv_VE_GetTtxReadDoneStatus(void)
{
    return Hal_VE_Get_ttxReadDoneStatus();
}

//------------------------------------------------------------------------------
/// VE Set VBI TT active Lines
/// @return none
//------------------------------------------------------------------------------
void MDrv_VE_SetVbiTtxActiveLines(MS_U8 u8LinePerField)
{
    Hal_VE_Set_VbiTtxActiveLines(u8LinePerField);
}

//------------------------------------------------------------------------------
/// VE Set VBI TT active Lines by the given bitmap
/// @param <IN>\b u32Bitmap: a bitmap that defines whick physical lines the teletext lines are to be inserted.
/// @return none
//------------------------------------------------------------------------------
void MDrv_VE_SetVbiTtxActiveLinesBitmap(MS_U32 u32Bitmap)
{
    Hal_VE_Set_VbiTtxActiveLinesBitmap(u32Bitmap);
}

//------------------------------------------------------------------------------
/// ve set vbi ttx active line
/// @param <in>\b odd_start: odd page start line
/// @param <in>\b odd_end: odd page end line
/// @param <in>\b even_start: even page start line
/// @param <in>\b even_end: even page end line
/// @return none
//------------------------------------------------------------------------------
void MDrv_VE_SetVbiTtxRange(MS_U16 odd_start, MS_U16 odd_end,
                            MS_U16 even_start, MS_U16 enen_end)

{
    Hal_VE_SetVbiTtxRange(odd_start, odd_end, even_start, enen_end);
}



//------------------------------------------------------------------------------
/// VE Set VBI CC
/// @return none
//------------------------------------------------------------------------------
void MDrv_VE_EnableCcSw(MS_BOOL bEnable)
{
    Hal_VE_EnableCcSw( bEnable);
}

//------------------------------------------------------------------------------
/// VE Set VBI CC active line
/// @return none
//------------------------------------------------------------------------------
void MDrv_VE_SetCcRange(MS_U16 odd_start, MS_U16 odd_end, MS_U16 even_start, MS_U16 enen_end)
{
    Hal_VE_SetCcRange( odd_start,  odd_end,  even_start,  enen_end);
}

//------------------------------------------------------------------------------
/// VE Set VBI CC data
/// @return none
//------------------------------------------------------------------------------
void MDrv_VE_SendCcData(MS_BOOL bIsOdd, MS_U16 data)    // true: odd, false: even
{
    Hal_VE_SendCcData( bIsOdd,  data);
}

//------------------------------------------------------------------------------
/// VE Set output with OSD
/// @return none
//------------------------------------------------------------------------------
void MDrv_VE_SetOSD(MS_BOOL bEnable)
{
    Hal_VE_Set_OSD(bEnable);
}

//------------------------------------------------------------------------------
/// VE Set RGB In
/// @return none
//------------------------------------------------------------------------------
void MDrv_VE_SetRGBIn(MS_BOOL bEnable)
{
    Hal_VE_set_rgb_in(bEnable);
}

//------------------------------------------------------------------------------
/// VE Get output video standard
/// @param <RET>       \b VideoSystem : the video standard: PAL or NTSC
//------------------------------------------------------------------------------
MS_VE_VIDEOSYS MDrv_VE_Get_Output_Video_Std(void)
{
    return Hal_VE_Get_Output_Video_Std();
}

//------------------------------------------------------------------------------
/// Force VE Enable/Disable Capture Mode, used to avoid enter DE mode in some case
/// @param <RET>       \b bEnable : TRUE: Force use capture mode, FALSE: use defaut
//------------------------------------------------------------------------------
void MDrv_VE_SetCaptureMode(MS_BOOL bEnable)
{
    g_VEInfo.bForceCaptureMode = bEnable;
}

//------------------------------------------------------------------------------
/// VE Get Destination Information for GOP mixer
/// @return TRUE/FALSE
//------------------------------------------------------------------------------
MS_BOOL MApi_VE_GetDstInfo(MS_VE_DST_DispInfo *pDstInfo, MS_U32 u32SizeofDstInfo)
{
    if(NULL == pDstInfo)
    {
        MSG_DRV_VE(printf("MApi_VE_GetDstInfo():pDstInfo is NULL\n");)
        return FALSE;
    }

    if(u32SizeofDstInfo != sizeof(MS_VE_DST_DispInfo))
    {
        MSG_DRV_VE(printf("MApi_VE_GetDstInfo():u16SizeofDstInfo is different from the MS_VE_DST_DispInfo defined, check header file!\n");)
        return FALSE;
    }

    if(MDrv_VE_Get_Output_Video_Std() == MS_VE_PAL)
    {
        //PAL
        MSG_DRV_VE(printf("MApi_VE_GetDstInfo(): It's PAL case.\n");)
        pDstInfo->bInterlaceMode = TRUE;
        pDstInfo->HDTOT = 864;
        pDstInfo->VDTOT = 624;
        pDstInfo->DEHST = 132;
        pDstInfo->DEHEND= pDstInfo->DEHST + 720;
        pDstInfo->DEVST = 44;
        pDstInfo->DEVEND= pDstInfo->DEVST + 576;
        return TRUE;
    }
    else if(MDrv_VE_Get_Output_Video_Std() == MS_VE_NTSC)
    {
        //NTSC
        MSG_DRV_VE(printf("MApi_VE_GetDstInfo(): It's NTSC case.\n");)
        pDstInfo->bInterlaceMode = TRUE;
        pDstInfo->HDTOT = 858;
        pDstInfo->VDTOT = 524;
        pDstInfo->DEHST = 119;
        pDstInfo->DEHEND= pDstInfo->DEHST +720;
        pDstInfo->DEVST = 36;
        pDstInfo->DEVEND= pDstInfo->DEVST + 480;
        return TRUE;
    }
    else
    {
        MSG_DRV_VE(printf("MApi_VE_GetDstInfo(): Unkown standard.\n");)
        pDstInfo->bInterlaceMode = FALSE;
        pDstInfo->HDTOT = 0;
        pDstInfo->VDTOT = 0;
        pDstInfo->DEHST = 0;
        pDstInfo->DEHEND= 0;
        pDstInfo->DEVST = 0;
        pDstInfo->DEVEND= 0;
        return FALSE;
    }

}
void MDrv_VE_Set_TestPattern(MS_BOOL bEn)
{
    MSG_DRV_VE(printf("VE Set TestPattern bEn = %x\n", bEn));
    Hal_VE_Set_TestPattern(bEn);
}

void MApi_VE_W2BYTE_MSK(MS_VE_REG_BANK VE_BK, MS_U32 u32Reg, MS_U16 u16Val, MS_U16 u16Mask)
{
    if(VE_BK == MS_VE_REG_BANK_3B)
    {
    #if !defined(CHIP_C3)
        Hal_VE_W2BYTE_MSK(L_BK_VE_SRC(u32Reg), u16Val, u16Mask);
    #endif
    }
    else if(VE_BK == MS_VE_REG_BANK_3E)
    {
        Hal_VE_W2BYTE_MSK(L_BK_VE_ENC(u32Reg), u16Val, u16Mask);
    }
    else if(VE_BK == MS_VE_REG_BANK_3F)
    {
        Hal_VE_W2BYTE_MSK(L_BK_VE_ENC_EX(u32Reg), u16Val, u16Mask);
    }
}

MS_U16 MApi_VE_R2BYTE_MSK(MS_VE_REG_BANK VE_BK, MS_U32 u32Reg, MS_U16 u16Mask)
{
    if(VE_BK == MS_VE_REG_BANK_3B)
    {
    #if defined(CHIP_C3)
        return 0;
    #else
        return Hal_VE_R2BYTE_MSK(L_BK_VE_SRC(u32Reg), u16Mask);
    #endif
    }
    else if(VE_BK == MS_VE_REG_BANK_3E)
    {
        return Hal_VE_R2BYTE_MSK(L_BK_VE_ENC(u32Reg), u16Mask);
    }
    else if(VE_BK == MS_VE_REG_BANK_3F)
    {
        return Hal_VE_R2BYTE_MSK(L_BK_VE_ENC_EX(u32Reg), u16Mask);
    }
    else
    {
        return 0;
    }
}

//------------------------------------------------------------------------------
/// VE Dump Table Interface
/// @return TRUE/FALSE
//------------------------------------------------------------------------------
void MDrv_VE_DumpTable(MS_U8 *pVETable, MS_U8 u8TableType)
{
    Hal_VE_DumpTable(pVETable, u8TableType);
}


#if 1
//------------------------------------------------------------------------------
/// VE Set MV
/// @return TRUE/FALSE
//------------------------------------------------------------------------------
VE_Result MDrv_VE_SetMV(MS_BOOL bEnble, MS_VE_MV_TYPE eMvType)
{
    return Hal_VE_SetMV(bEnble, eMvType);
}
#endif //#if 1

//------------------------------------------------------------------------------
/// @brief This routine set flag to mask register write for special case \n
///			e.g. MBoot to APP with logo display
/// @argument:
///              - bFlag: TRUE: Mask register write, FALSE: not Mask
/// @return None
//------------------------------------------------------------------------------
VE_Result MDrv_VE_DisableRegWrite(MS_BOOL bFlag)
{
    if(Hal_VE_DisableRegWrite_GetCaps() == TRUE)
    {
	    g_bVeDisableRegWrite = bFlag;
        return E_VE_OK;
    }
    else
    {
        g_bVeDisableRegWrite = FALSE;
        return E_VE_OK;
    }
}


//------------------------------------------------------------------------------
/// VE show internal color bar (test pattern)
/// @param  - bEnable IN: TRUE to enable color; false to disable color bar
/// @return None
//------------------------------------------------------------------------------
void MDrv_VE_ShowColorBar(MS_BOOL bEnable)
{
    Hal_VE_ShowColorBar(bEnable);
}

//------------------------------------------------------------------------------
/// @brief This routine set read/write addjustment to centralize VE display window.\n
///        Set the adjustment and it works after MDrv_VE_set_display_window() API is invoked. \n
///
/// @argument:
///              - s32WAddrAdjustment: the pixel units to adjust on write address
///              - s32RAddrAdjustment: the pixel units to adjust on read address
/// @return VE_Result
//------------------------------------------------------------------------------
VE_Result MDrv_VE_AdjustPositionBase(MS_S32 s32WAddrAdjustment, MS_S32 s32RAddrAdjustment)
{
    return Hal_VE_AdjustPositionBase(s32WAddrAdjustment, s32RAddrAdjustment);
}

//------------------------------------------------------------------------------
/// @brief In STB Chip, VE needs to set vepll to lock frame rate between HD and SD Path.\n
///
/// @argument:
///              - MS_VE_PLL_InCfg: Information of HD path
///              - MS_VE_PLL_OutCfg: Information of SD path
/// @return VE_Result
//------------------------------------------------------------------------------
VE_Result MDrv_VE_SetFrameLock(MS_VE_PLL_InCfg *pInCfg, MS_BOOL bEnable)
{
    MS_VE_PLL_OutCfg stOutCfg;
	MS_U32 u32FreqIn,u32FreqOut;

    if(g_VEInfo.VideoSystem <= MS_VE_PAL_M)
    {
        //NTSC, PAL-M System
        stOutCfg.u16VttOut = 525;
        stOutCfg.u16HttOut = 858;
        stOutCfg.u16VdeOut = 480;
        stOutCfg.u16HdeOut = 720;
    }
    else
    {
        //PAL System
        stOutCfg.u16VttOut = 625;
        stOutCfg.u16HttOut = 864;
        stOutCfg.u16VdeOut = 576;
        stOutCfg.u16HdeOut = 720;
    }

    stOutCfg.bInterlace = TRUE;

    if(pInCfg != NULL)
    {
        memcpy(&_stVE_PllInfo.InCfg, pInCfg, sizeof(MS_VE_PLL_InCfg));
        memcpy(&_stVE_PllInfo.OutCfg, &stOutCfg, sizeof(MS_VE_PLL_OutCfg));

        //idclk_div & odclk_div

        if(_stVE_PllInfo.InCfg.u16HdeIn > 720 ) //HD timing
        {
            _stVE_PllInfo.u32InitPll = 0x19999999;
        }
        else
        {
            _stVE_PllInfo.u32InitPll = 0x15555555;
        }

        u32FreqIn   = _stVE_PllInfo.InCfg.u16VttIn  *  _stVE_PllInfo.InCfg.u16HttIn;
        u32FreqOut = ((MS_U32)(_stVE_PllInfo.OutCfg.u16VttOut)* (MS_U32)(_stVE_PllInfo.OutCfg.u16HttOut))*4;
        if(!_stVE_PllInfo.OutCfg.bInterlace)
            u32FreqOut *= 2;

        MSG_DRV_VE(printf("[VE_PLL] MDrv_VE_SetFrameLock\n");)
        MSG_DRV_VE(printf("[VE_PLL] 32FreqIn=%lx \t u32FreqOut=%lx \n",u32FreqIn,u32FreqOut);)
        MSG_DRV_VE(printf("[VE_PLL] u32InitPll=%lx \n",_stVE_PllInfo.u32InitPll);)

        _ReductionFrac(&u32FreqIn, &u32FreqOut);
        _MultiplFrac(&u32FreqIn,&u32FreqOut);

        _stVE_PllInfo.u32IDclk  = u32FreqIn;
        _stVE_PllInfo.u32ODclk =  u32FreqOut;

        if( ( _stVE_PllInfo.u32IDclk>0xFFFFFF ) || ( _stVE_PllInfo.u32ODclk>0xFFFFFF ) )
        {
                MSG_DRV_VE(printf("[VE_PLL] ASSERT !!! idclk/odclk overflow\n");)
        }

        MSG_DRV_VE(printf("[VE_PLL] idclk=%lx \t odclk=%lx\n",_stVE_PllInfo.u32IDclk,_stVE_PllInfo.u32ODclk););
    }
    else
    {
        MSG_DRV_VE(printf("[VE_PLL] input config is NULL, set fixed clk as ve clk!\n"));
        bEnable = FALSE;
    }

    return HAL_VE_SetFrameLock((_stVE_PllInfo.u32IDclk&0xFFFFFF),(_stVE_PllInfo.u32ODclk&0xFFFFFF), _stVE_PllInfo.u32InitPll, bEnable);
}

//------------------------------------------------------------------------------
/// @brief MDrv_VE_SetWindow, set crop window and display window in one API.
///
/// @argument:
///     - a pointer to MS_VE_WINDOW_TYPE - source window, depends on input size
///     - a pointer to MS_VE_WINDOW_TYPE - crop window, depends on source window size
///     - a pointer to MS_VE_WINDOW_TYPE - display window, depends on output size
/// @return VE_Result
//------------------------------------------------------------------------------
VE_Result MDrv_VE_SetWindow(MS_VE_WINDOW_TYPE *stSrcWin, MS_VE_WINDOW_TYPE *stCropWin,
        MS_VE_WINDOW_TYPE *stDispWin)
{
    MS_VE_WINDOW_TYPE scaled_srcWin, scaled_cropWin;
    const MS_U32 base_width = 720;  // maximun down-scaled width
    const MS_U32 base_height = 576; // maximun down-scaled height
    MS_U16 bak_H_CapSize = g_VEInfo.u16H_CapSize;
    MS_U16 bak_V_CapSize = g_VEInfo.u16V_CapSize;
    VE_Result ret = E_VE_OK;

    // check chip capibility
    if ((!g_VECap.bSupport_CropMode) || (!g_VECap.bSupport_UpScale)) {
        MSG_DRV_VE(printf("[%s] is not supported in this chip\n",
                    __FUNCTION__));
        ret = E_VE_NOT_SUPPORT;
        goto fail;
    }
    // check input parameters
    if (stSrcWin == NULL) {
        MSG_DRV_VE(printf("[%s][%d] failed\n", __FUNCTION__, __LINE__));
        ret = E_VE_FAIL;
        goto fail;
    }
    if (stCropWin == NULL) {
        MSG_DRV_VE(printf("[%s][%d] failed\n", __FUNCTION__, __LINE__));
        ret = E_VE_FAIL;
        goto fail;
    }
    if (stDispWin == NULL) {
        MSG_DRV_VE(printf("[%s][%d] failed\n", __FUNCTION__, __LINE__));
        ret = E_VE_FAIL;
        goto fail;
    }
    memcpy(&scaled_srcWin, stSrcWin, sizeof(MS_VE_WINDOW_TYPE));
    memcpy(&scaled_cropWin, stCropWin, sizeof(MS_VE_WINDOW_TYPE));

    // stage0 - setup down scaling factor
    if ((stSrcWin->width > base_width) || (stSrcWin->height > base_height)) {
        if ((stSrcWin->width >= ((base_width * stSrcWin->height) / base_height))) {
            // stSrcWin->width:stSrcwin->height >= base_width:base_height
            // base_width-based down scale
            MSG_DRV_VE(printf("[%s]stage 0: do width-based down-scale\n", __FUNCTION__));
            // setup scaled_srcWin
            scaled_srcWin.x = (MS_U16)base_width * stSrcWin->x / stSrcWin->width;
            scaled_srcWin.y = (MS_U16)base_width * stSrcWin->y / stSrcWin->width;
            scaled_srcWin.width = (MS_U16)base_width;
            scaled_srcWin.height = (MS_U16)base_width * stSrcWin->height / stSrcWin->width;
            // setup scaled_cropWin
            scaled_cropWin.x = (MS_U16)base_width * stCropWin->x / stSrcWin->width;
            scaled_cropWin.y = (MS_U16)base_width * stCropWin->y / stSrcWin->width;
            scaled_cropWin.width = (MS_U16)base_width * stCropWin->width / stSrcWin->width;
            scaled_cropWin.height = (MS_U16)base_width * stCropWin->height / stSrcWin->width;
        } else {
            // stSrcWin->width:stSrcwin->height < base_width:base_height
            // base_height-based down scale
            MSG_DRV_VE(printf("[%s]stage 0: do height-based down-scale\n", __FUNCTION__));
            // setup scaled_srcWin
            scaled_srcWin.x = (MS_U16)base_height * stSrcWin->x / stSrcWin->height;
            scaled_srcWin.y = (MS_U16)base_height * stSrcWin->y / stSrcWin->height;
            scaled_srcWin.width = (MS_U16)base_height * stSrcWin->width / stSrcWin->height;
            scaled_srcWin.height = (MS_U16)base_height;
            // setup scaled_cropWin
            scaled_cropWin.x = (MS_U16)base_height * stCropWin->x / stSrcWin->height;
            scaled_cropWin.y = (MS_U16)base_height * stCropWin->y / stSrcWin->height;
            scaled_cropWin.width = (MS_U16)base_height * stCropWin->width / stSrcWin->height;
            scaled_cropWin.height = (MS_U16)base_height * stCropWin->height / stSrcWin->height;
        }
    } else {
        // no down-scale
        MSG_DRV_VE(printf("[%s]stage 0: no down-scale\n", __FUNCTION__));
        // setup scaled_srcWin
        scaled_srcWin.x = stSrcWin->x;
        scaled_srcWin.y = stSrcWin->y;
        scaled_srcWin.width = stSrcWin->width;
        scaled_srcWin.height = stSrcWin->height;
        // setup scaled_cropWin
        scaled_cropWin.x = stCropWin->x;
        scaled_cropWin.y = stCropWin->y;
        scaled_cropWin.width = stCropWin->width;
        scaled_cropWin.height = stCropWin->height;
    }

    // stage1 - Down scale source window width to base_width without lossing information.
    // (before MIU)
    MSG_DRV_VE(printf("[%s]stage 1: stSrcWin (%d, %d, %d, %d) -> "
                "scaled_srcWin (%d, %d, %d, %d)\n",
                __FUNCTION__,
                stSrcWin->x, stSrcWin->y, stSrcWin->width, stSrcWin->height,
                scaled_srcWin.x, scaled_srcWin.x, scaled_srcWin.width, scaled_srcWin.height));

    g_VEInfo.u16H_CapSize = stSrcWin->width;
    g_VEInfo.u16V_CapSize = stSrcWin->height;
    MDrv_VE_set_scaling_ratio(scaled_srcWin.width, scaled_srcWin.height);

    // stage2 - Crop video from the down-scaled video. (after MIU)
    MSG_DRV_VE(printf("[%s]stage 2: stCropWin (%d, %d, %d, %d) -> "
                "scaled_cropWin (%d, %d, %d, %d)\n",
                __FUNCTION__,
                stCropWin->x, stCropWin->y, stCropWin->width, stCropWin->height,
                scaled_cropWin.x, scaled_cropWin.x, scaled_cropWin.width,
                scaled_cropWin.height));

    if (E_VE_OK != MDrv_VE_set_crop_window(scaled_cropWin)) {
        MSG_DRV_VE(printf("[%s][%d] failed\n", __FUNCTION__, __LINE__));
        ret = E_VE_FAIL;
        goto fail;
    }

    // stage3 - Scale cropped video to output video size. (after MIU)
    g_VEInfo.u16H_CapSize = scaled_cropWin.width;
    g_VEInfo.u16V_CapSize = scaled_cropWin.height;
    if (E_VE_OK != MDrv_VE_set_be_display_window(*stDispWin)) {
        MSG_DRV_VE(printf("[%s][%d] failed\n", __FUNCTION__, __LINE__));
        ret = E_VE_FAIL;
        goto fail;
    }
    MSG_DRV_VE(printf("[%s]stage 3: scaled_cropWin(%d, %d, %d, %d) -> "
                "stDispWin (%d, %d, %d, %d)\n",
                __FUNCTION__,
                scaled_cropWin.x, scaled_cropWin.x, scaled_cropWin.width,
                scaled_cropWin.height,
                stDispWin->x, stDispWin->y, stDispWin->width, stDispWin->height));

    return E_VE_OK;

fail:
    // revert changes
    g_VEInfo.u16H_CapSize = bak_H_CapSize;
    g_VEInfo.u16V_CapSize = bak_V_CapSize;
    printf("Error: [%s] failed, errno: %d\n", __FUNCTION__, ret);
    return ret;
}

#define MS_VEC_MAX_FRAMENUM_P      3 //These 2 setting should obey with driver setting in "Hal_VE_set_frc"
#define MS_VEC_MAX_FRAMENUM_I      2
/******************************************************************************/
/// Init and config the VE capture
/// @param pVECapture \b IN : @ref PMS_VE_Output_CAPTURE
/// @return TRUE: success
/// @return FALSE: fail
/******************************************************************************/
void MDrv_VE_InitVECapture(PMS_VE_Output_CAPTURE pVECapture)
{
#ifdef MSOS_TYPE_LINUX
    g_VEInfo.bVECapture = pVECapture->bVECapture;
    if(g_VEInfo.bVECapture)
    {
        MS_GFLIP_VEC_CONFIG stGflipVECConfig;
        MDrv_VEC_GFLIPInit();
        g_VEInfo.u16OutputWidth = pVECapture->u16Width;
        g_VEInfo.u16Outputheight= pVECapture->u16height;
        g_VEInfo.u32MemSize     = pVECapture->u32MemSize;
        g_VEInfo.u32MiuBaseAddr = pVECapture->u32MiuBaseAddr;
        MDrv_VE_SetMemoryBaseAddr(g_VEInfo.u32MiuBaseAddr, g_VEInfo.u32MemSize);
        stGflipVECConfig.u16Version  = VE_VEC_CONFIG_VERSION;//For compatibility with kernel
        stGflipVECConfig.u16Length   = sizeof(MS_GFLIP_VEC_CONFIG);
        stGflipVECConfig.eConfigType = MS_VEC_CONFIG_INIT;
        stGflipVECConfig.bInterlace  = g_VEInfo.bSrcInterlace;
        stGflipVECConfig.eIsrType    = MS_VEC_ISR_SC_OP;
        stGflipVECConfig.u8MaxFrameNumber_P = MS_VEC_MAX_FRAMENUM_P;
        stGflipVECConfig.u8MaxFrameNumber_I = MS_VEC_MAX_FRAMENUM_I;
        stGflipVECConfig.u8Result = FALSE;
        if (ioctl(g_VEInfo.s32FdGFlip, MDRV_GFLIP_VEC_IOC_CONFIG, &stGflipVECConfig))
        {
            MSG_DRV_VE(printf("%s %d: IO(INIT) fail!!!!\n", __FUNCTION__, __LINE__));
        }
    }
    else
    {
        g_VEInfo.stVECusScalingInfo.bHCusScalingEnable = FALSE;
        g_VEInfo.stVECusScalingInfo.bVCusScalingEnable = FALSE;
    }

    //workaround to pass parasoft test:
    //[2]A pointer parameter in a function prototype should be declared as pointer to const if the pointer is not used to modify the addressed object (JSF-118-4)
    pVECapture->u16Width += 0;
    MSG_DRV_VE(printf("VE InitVECapture bEnable=%u, W/H=%u, %u, MemBase=0x%lx, Size=%lu\n",
                       g_VEInfo.bVECapture, pVECapture->u16Width, pVECapture->u16height, pVECapture->u32MiuBaseAddr, g_VEInfo.u32MemSize));
#endif //def MSOS_TYPE_LINUX
}

/******************************************************************************/
/// Enable VE capture
/// @param pstVECapState \b IN : @ref PMS_VE_VECAPTURESTATE
/// @return TRUE: success
/// @return FALSE: fail
/******************************************************************************/
MS_BOOL MDrv_VE_EnaVECapture(PMS_VE_VECAPTURESTATE pstVECapState)
{
#ifdef MSOS_TYPE_LINUX
    MS_GFLIP_VEC_CONFIG stGflipVECConfig;
    MS_GFLIP_VEC_STATE  stGflipVECstate;
    stGflipVECstate.bEnable      = pstVECapState->bEnable;//Enable or Disable
    stGflipVECstate.u8Result    = FALSE; //default set to false


    if(0 > g_VEInfo.s32FdGFlip)
    {
        MSG_DRV_VE(printf("%s %d: GLIP device not opened!!!!\n", __FUNCTION__, __LINE__));
        return FALSE;
    }

    //Below is Progressive/Interlace config for VEC
    memset(&stGflipVECConfig, 0, sizeof(stGflipVECConfig));
    stGflipVECConfig.u16Version  = VE_VEC_CONFIG_VERSION;//For compatibility with kernel
    stGflipVECConfig.u16Length   = sizeof(MS_GFLIP_VEC_CONFIG);
    stGflipVECConfig.eConfigType = MS_VEC_CONFIG_ENABLE;
    stGflipVECConfig.bInterlace  = g_VEInfo.bSrcInterlace;
    stGflipVECConfig.u8Result    = FALSE;
    if (ioctl(g_VEInfo.s32FdGFlip, MDRV_GFLIP_VEC_IOC_CONFIG, &stGflipVECConfig))
    {
        MSG_DRV_VE(printf("%s %d: IO(INIT) fail!!!!\n", __FUNCTION__, __LINE__));
        return FALSE;
    }
    if(TRUE == stGflipVECConfig.u8Result)
    {
        if (ioctl(g_VEInfo.s32FdGFlip, MDRV_GFLIP_VEC_IOC_ENABLEVECAPTURE, &stGflipVECstate))
        {
            MSG_DRV_VE(printf("%s %d: IO(ENABLE) fail!!!!\n", __FUNCTION__, __LINE__));
            return FALSE;
        }
        pstVECapState->u8Result = stGflipVECstate.u8Result;
        pstVECapState->bEnable  = stGflipVECstate.bEnable;
        pstVECapState->u8FrameCount = stGflipVECstate.u8FrameCount;
    }
    return stGflipVECstate.u8Result;
#endif
    return FALSE; //Unsupport now
}

/******************************************************************************/
/// Get VE capture state
/// @param pstVECapState \b OUT : @ref PMS_VE_VECAPTURESTATE
/// @return TRUE: success
/// @return FALSE: fail
/******************************************************************************/
MS_BOOL MDrv_VE_GetVECaptureState(PMS_VE_VECAPTURESTATE pstVECapState)
{
#ifdef MSOS_TYPE_LINUX
    MS_GFLIP_VEC_STATE stGflipVECstate;
    stGflipVECstate.u8Result     = FALSE;
    if(0 > g_VEInfo.s32FdGFlip)
    {
        MSG_DRV_VE(printf("%s %d: GLIP device not opened!!!!\n", __FUNCTION__, __LINE__));
        return FALSE;
    }

    if (ioctl(g_VEInfo.s32FdGFlip, MDRV_GFLIP_VEC_IOC_GETVECAPTURESTATE, &stGflipVECstate))
    {
        MSG_DRV_VE(printf("%s %d: IO(GET STATE) fail!!!!\n", __FUNCTION__, __LINE__));
        return FALSE;
    }
    //printf("GFLP=%u\n", pstVECapState->u8FrameCount);
    pstVECapState->u8Result = stGflipVECstate.u8Result;
    if(TRUE == pstVECapState->u8Result)
    {
        pstVECapState->bEnable = stGflipVECstate.bEnable;
        pstVECapState->u8FrameCount = stGflipVECstate.u8FrameCount;
    }
    return pstVECapState->u8Result;
#endif
    return FALSE; //Unsupport now
}

//-------------------------------------------------------------------------------------------------
/// Wait on the finish of specified frame: Check if the frame is captured,
/// if Yes, return TRUE, otherwise sleep until the next Vsync ISR
/// @param  pstVECapState          \b IN : @ref PMS_VE_VECAPTURESTATE
/// @return TRUE: success
/// @return FALSE: fail
//-------------------------------------------------------------------------------------------------
MS_BOOL MDrv_VE_VECaptureWaitOnFrame(PMS_VE_VECAPTURESTATE pstVECapState)
{
#ifdef MSOS_TYPE_LINUX
    MS_GFLIP_VEC_STATE stGflipVECstate;
    stGflipVECstate.bEnable      = pstVECapState->bEnable;
    stGflipVECstate.u8FrameCount = pstVECapState->u8FrameCount;
    stGflipVECstate.u8Result     = FALSE;
    if(0 > g_VEInfo.s32FdGFlip)
    {
        MSG_DRV_VE(printf("%s %d: GLIP device not opened!!!!\n", __FUNCTION__, __LINE__));
        return FALSE;
    }

    if (ioctl(g_VEInfo.s32FdGFlip, MDRV_GFLIP_VEC_IOC_VECAPTUREWAITONFRAME, &stGflipVECstate))
    {
        MSG_DRV_VE(printf("%s %d: IO(WAIT) fail!!!!\n", __FUNCTION__, __LINE__));
        return FALSE;
    }
    //printf("GFLP=%u\n", pstVECapState->u8FrameCount);
    pstVECapState->bEnable  = stGflipVECstate.bEnable;
    pstVECapState->u8Result = stGflipVECstate.u8Result;
    return pstVECapState->u8Result;
#endif
    return FALSE; //Unsupport now
}

////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function \b Name: MDrv_VE_Adjust_FrameStart
/// @brief \b Function \b Description : the frame start is used to adjust output video
///                                     (in pixel)
///
/// @param <IN>        \b pixel_offset
/// @return VE_Result
////////////////////////////////////////////////////////////////////////////////
VE_Result MDrv_VE_Adjust_FrameStart(MS_VE_WINDOW_TYPE *stSrcWin,MS_S16 pixel_offset)
{
    //central alignment
    //(720 only show 704. Align formula : Waddr mapping Raddr+8pixel)
    //case 1: source is not PAL mode - non-alignment
    //case 2: source is 704 & PAL mode and DI mode - non-alignment //DI
    //others : alignment //include OP:704 scale to 720 case

    Hal_VE_SetMemAddr(&g_VEInfo.u32MiuBaseAddr,&g_VEInfo.u32MiuBaseAddr);

    if((stSrcWin) && (g_VEInfo.VideoSystem >= MS_VE_PAL_N )&&(g_VEInfo.VideoSystem <= MS_VE_PAL ) )   //PAL mode
    {
        if(!(((stSrcWin->width==704) && (stSrcWin->height==576)) &&
            (g_VEInfo.InputSrcType == MS_VE_SRC_DI)) )
        {
            MS_U32 u32AlignAddr = g_VEInfo.u32MiuBaseAddr + 16;
            Hal_VE_SetMemAddr(&(u32AlignAddr),&g_VEInfo.u32MiuBaseAddr);
        }
    }

    return Hal_VE_Adjust_FrameStart(pixel_offset);
}

#endif

