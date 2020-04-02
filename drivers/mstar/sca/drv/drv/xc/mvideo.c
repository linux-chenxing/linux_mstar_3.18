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
/// file    Mvideo.c
/// @brief  Main API in XC library
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////
#define  MVIDEO_C

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
// Common Definition
#ifdef MSOS_TYPE_LINUX_KERNEL
#include <asm/div64.h>
#elif defined(MSOS_TYPE_CE)
#include <windows.h>
#include <ceddk.h>
#else
#define do_div(x,y) ((x)/=(y))
#endif

#ifdef MSOS_TYPE_LINUX_KERNEL
#include <linux/string.h>
#include <linux/wait.h>
#include <linux/irqreturn.h>
#else
#include <string.h>
#endif
#include "MsCommon.h"
#include "MsVersion.h"
#include "MsOS.h"
#include "mhal_xc_chip_config.h"

// Internal Definition
#include "drvXC_IOPort.h"
#include "apiXC.h"
#include "apiXC_Adc.h"

#include "drvXC_HDMI_Internal.h"
#include "mvideo_context.h"
#include "drvXC_ADC_Internal.h"

#include "xc_Analog_Reg.h"

#include "drv_sc_isr.h"
#include "drv_sc_ip.h"
#include "drv_sc_display.h"
#include "drv_sc_scaling.h"
#include "drv_sc_mux.h"

#include "drvscaler_nr.h"
#include "drvMMIO.h"
#include "drv_sc_ip.h"

//Add for A5
#include "mdrv_frc.h"
#include "mhal_frc.h"

// Tobe refined
#include "xc_hwreg_utility2.h"
#include "drv_sc_menuload.h"
#include "mhal_sc.h"
#include "mhal_mux.h"
#include "mhal_hdmi.h"
#include "mdrv_sc_3d.h"
#include "apiXC_PCMonitor.h"

/*
    XC_INITDATA_VERSION                      current version : 1    //1: Add an field eScartIDPort_Sel for port selection
*/


PQ_Function_Info    s_PQ_Function_Info = {0, 0, 0, 0, 0, 0, 0, 0, 0};
extern MS_BOOL _bSupportHVMode;

#ifdef MULTI_SCALER_SUPPORTED
	extern MS_BOOL MApi_XC_Is_SubWindowEanble(SCALER_WIN eWindow);
#else
	extern MS_BOOL MApi_XC_Is_SubWindowEanble(void);
#endif

MS_XC_MEM_FMT s_eMemFmt = E_MS_XC_MEM_FMT_AUTO;
MS_BOOL       s_bKeepPixelPointerAppear = FALSE;

MS_U8 MApi_Pnl_Get_LPLL_Type(void);
//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define msg_video(x) //x
#define VERIFY_MVIDEO_FPGA  0
typedef enum
{
    SCAN_AUTO = 0,
    SCAN_PROGRESSIVE = SCAN_AUTO,
    SCAN_INTERLACE = 1
}EN_VIDEO_SCAN_TYPE;

//-------------------------------------------------------------------------------------------------
//  Local Structurs
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------
#if(XC_CONTEXT_ENABLE)
//XC_Context *g_pSContext = NULL;
static XC_Context _SContext;
static XC_Context_Shared _SCShared;
#endif

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
static MSIF_Version _api_xc_version = {
#if ( defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_NOS) || defined(MSOS_TYPE_LINUX_KERNEL))
	{ XC_API_VERSION },
#else
    .DDI = { XC_API_VERSION },
#endif
};

static XC_ApiInfo       _stXC_ApiInfo;
static MS_BOOL _bSkipSWReset = FALSE;   ///< Flag for: If the system needs to show Logo from MBoot layer to APP layer without flashing. \n We bypass SW reset in MApi_XC_Init.
static MS_BOOL _bDFBInit = FALSE;
static PQ_ADC_SAMPLING_INFO stPQADCSamplingInfo = {0};
static INPUT_SOURCE_TYPE_t enOldInputSrc[MAX_WINDOW] = {INPUT_SOURCE_NONE, INPUT_SOURCE_NONE,
                                                     #ifdef MULTI_SCALER_SUPPORTED
                                                        INPUT_SOURCE_NONE, INPUT_SOURCE_NONE,
                                                        INPUT_SOURCE_NONE, INPUT_SOURCE_NONE,
                                                     #endif
                                                       };

//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
static MS_BOOL Is_YPbPr_CVBSO_Path_Existed(void);
static void _MApi_XC_Patch_SCx_Coase_Signal(INPUT_SOURCE_TYPE_t enInputSourceType, SCALER_WIN eWindow);


//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
#if(XC_CONTEXT_ENABLE)
MS_U8 _XC_Client[] = {"Linux XC driver"};

static MS_BOOL _MApi_XC_Init_Context(MS_BOOL *pbFirstDrvInstant)
{
    XC_Context_Shared *pShared = NULL;
    MS_BOOL bNeedInitShared = FALSE, bRet = TRUE;

    if (g_pSContext)
    {
        // The context instance has been created already
        // before somewhere sometime in the same process.
        *pbFirstDrvInstant = bNeedInitShared;
        return FALSE;
    }

#ifdef MSOS_TYPE_LINUX
    MS_U32 u32ShmId;
    MS_U32 u32Addr;
    MS_U32 u32BufSize;

    MsOS_SHM_Init();
    if (FALSE == MsOS_SHM_GetId(_XC_Client, sizeof(XC_Context_Shared), &u32ShmId, &u32Addr, &u32BufSize, MSOS_SHM_QUERY))
    {
        if (FALSE == MsOS_SHM_GetId(_XC_Client, sizeof(XC_Context_Shared), &u32ShmId, &u32Addr, &u32BufSize, MSOS_SHM_CREATE))
        {
            msg_video(printf("XC: SHM allocation failed!\n"));
            bRet = FALSE;
        }
        if(bRet)
        {
            msg_video(printf("XC: [%s][%d] This is first initial 0x%08lx\n", __FUNCTION__, __LINE__, u32Addr));
            memset( (MS_U8*)u32Addr, 0, sizeof(XC_Context_Shared));
            bNeedInitShared = TRUE;
        }
    }
    if(bRet)
    {
        pShared = (XC_Context_Shared *)u32Addr;
    }
#endif
    if(pShared == NULL)
    {
        pShared = &_SCShared;
        bNeedInitShared = TRUE;
    }

    *pbFirstDrvInstant = bNeedInitShared;
    memset(&_SContext, 0, sizeof(_SContext));
    if (bNeedInitShared)
    {
        memset(pShared, 0, sizeof(XC_Context_Shared));
    }
    _SContext.pCommInfo = pShared;

    g_pSContext = &_SContext;

    return bRet;
}
#endif

//-------------------------------------------------------------------------------------------------
/// Exit the XC
/// @return @ref MS_BOOL
//-------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_Exit(void)
{

    if (_XC_Mutex == -1)
    {
        return TRUE;
    }

    // Protect memory
    MApi_XC_FreezeImg(ENABLE, MAIN_WINDOW);

    _XC_ENTRY();
    if(MApi_XC_GetInfo()->u8MaxWindowNum > 1)
    {
        // DISABLE FIClk1
        MDrv_WriteRegBit(REG_CKG_FICLK_F1, ENABLE, CKG_FICLK_F1_GATED);                // Enable clock

    #ifdef MULTI_SCALER_SUPPORTED
        MDrv_WriteRegBit(REG_CKG_SC2_FICLK_F1, ENABLE, CKG_SC2_FICLK_F1_GATED);        // Enable clock
    #endif

        // DISABLE IDClk1
        MDrv_WriteRegBit(REG_CKG_IDCLK1, ENABLE, CKG_IDCLK1_GATED);
    }

    // DISABLE FIClk2
    // SC0
    MDrv_WriteRegBit(REG_CKG_FICLK_F2, ENABLE, CKG_FICLK_F2_GATED);             // Enable clock

    #ifdef  MULTI_SCALER_SUPPORTED
    // SC1  & SC2
    MDrv_WriteRegBit(REG_CKG_SC1_FICLK_F2, ENABLE, CKG_SC1_FICLK_F2_GATED);     // Enable clock
    MDrv_WriteRegBit(REG_CKG_SC2_FICLK_F2, ENABLE, CKG_SC2_FICLK_F2_GATED);     // Enable clock
    #endif

    // DISABLE FClk
    MDrv_WriteRegBit(REG_CKG_FCLK, ENABLE, CKG_FCLK_GATED);                            // Enable clock

#ifdef CHIP_C3
    // DISABLE EDCLK
    MDrv_WriteRegBit(REG_CKG_EDCLK, ENABLE, CKG_EDCLK_GATED);                          // Enable clock
#endif

    // DISABLE IDClk2
    MDrv_WriteRegBit(REG_CKG_IDCLK2, ENABLE, CKG_IDCLK2_GATED);                        // Enable clock

    #ifdef  MULTI_SCALER_SUPPORTED
    // SC1 & SC2
    MDrv_WriteRegBit(REG_CKG_SC1_IDCLK2, ENABLE, CKG_SC1_IDCLK2_GATED);         // Enable clock
    MDrv_WriteRegBit(REG_CKG_SC2_IDCLK2, ENABLE, CKG_SC2_IDCLK2_GATED);         // Enable clock

    #endif
    _XC_RETURN();

    // Release ISR
    MsOS_DetachInterrupt(E_INT_IRQ_DISP);

    // Clear internal variable
    mvideo_sc_variable_init(TRUE, &g_XC_InitData);

    _XC_ENTRY();
    memset(&g_XC_InitData, 0 , sizeof(XC_INITDATA) );
    _XC_RETURN();

    // For XC re-init
    MsOS_DeleteMutex(_XC_Mutex);
    _XC_Mutex = -1;

    MsOS_DeleteMutex(_XC_ISR_Mutex);
    _XC_ISR_Mutex = -1;

#if(XC_CONTEXT_ENABLE)
    g_pSContext  = NULL;
#endif

    return TRUE;

}

MS_BOOL MDrv_XC_SetIOMapBase_i(void)
{
    MS_U32 _XCRIUBaseAddress = 0, _PMRIUBaseAddress = 0, u32NonPMBankSize = 0, u32PMBankSize = 0;


    if(MDrv_MMIO_GetBASE( &_XCRIUBaseAddress, &u32NonPMBankSize, MS_MODULE_XC ) != TRUE)
    {
        msg_video(printf("MApi_XC_Init GetBASE failure\n"));
        return FALSE;
    }
    else
    {
        //printf("MApi_XC_Init GetBase success %lx\n", _XCRIUBaseAddress);
    }

    if(MDrv_MMIO_GetBASE( &_PMRIUBaseAddress, &u32PMBankSize, MS_MODULE_PM ) != TRUE)
    {
        msg_video(printf("MApi_XC_Init Get PM BASE failure\n"));
        return FALSE;
    }
    else
    {
        //printf("MApi_XC_Init GetPMBase success %lx\n", _PMRIUBaseAddress);
    }
    //printf("XC RIU base=%lx\n", _XCRIUBaseAddress);

    MDrv_XC_init_riu_base(_XCRIUBaseAddress, _PMRIUBaseAddress);

    return TRUE;
}

//This API only designed for DFB calling.
//Supernova involved xc lib only by initializing this function.
//So we need to create mutext in this API.
//it's useless for new dfb-xc design
MS_BOOL MDrv_XC_SetIOMapBase(void)
{
    MS_BOOL bReturn = FALSE;
    _bDFBInit = TRUE; //This flag is to compatible old dfb version.
                      //new dfb will not call this func,
                      //instead of set u32InitDataLen=0 of MApi_XC_Init()
    printf("MDrv_XC_SetIOMapBase(), do nothing, it's assumed DFB init case!\n");

    return bReturn;
}

void MApi_XC_PreInit_FRC(XC_PREINIT_INFO_t *pPanelInfo)
{
    memcpy(&g_XC_Pnl_Misc, pPanelInfo, sizeof(XC_PREINIT_INFO_t));
}

E_APIXC_ReturnValue MApi_XC_PreInit( E_XC_PREINIT_t eType ,void* para, MS_U32 u32Length)
{
    switch(eType)
    {
        case E_XC_PREINIT_FRC:
            if(u32Length != sizeof(XC_PREINIT_INFO_t))
            {
                printf("Error size of parameters\n");
                return E_APIXC_RET_FAIL;
            }
            MApi_XC_PreInit_FRC((XC_PREINIT_INFO_t *)para);
        break;
        default:
        case E_XC_PREINIT_NULL:

        break;
    }
    return E_APIXC_RET_OK;
}


#if FRC_INSIDE
static void MApi_XC_FRC_PNLInfo_Transform(XC_PANEL_INFO *XC_PNLInfo, XC_PREINIT_INFO_t *XC_PNLInfo_Adv,
                                                 MST_PANEL_INFO_t *PanelInfo)
{
    PanelInfo->u8HSyncStart        = (MS_U8)(XC_PNLInfo->u16HTotal -
                                     (MS_U16)(XC_PNLInfo_Adv->u8PanelHSyncBackPorch - XC_PNLInfo_Adv->u8PanelHSyncWidth));
    PanelInfo->u8HSyncEnd          = (MS_U8)(XC_PNLInfo->u16HTotal -
                                     (MS_U16)(XC_PNLInfo_Adv->u8PanelHSyncBackPorch));
    PanelInfo->u8VSyncStart        = (MS_U8)(XC_PNLInfo->u16VTotal -
                                     (MS_U16)(XC_PNLInfo_Adv->u8PanelVSyncBackPorch - XC_PNLInfo_Adv->u8PanelVSyncWidth));
    PanelInfo->u8VSyncEnd          = (MS_U8)(XC_PNLInfo->u16VTotal -
                                     (MS_U16)(XC_PNLInfo_Adv->u8PanelVSyncBackPorch));
    PanelInfo->u16VTrigX           = XC_PNLInfo_Adv->u16VTrigX;
    PanelInfo->u16VTrigY           = XC_PNLInfo_Adv->u16VTrigY;

    PanelInfo->u16HTotal           = XC_PNLInfo->u16HTotal;
    PanelInfo->u16VTotal           = XC_PNLInfo->u16VTotal;
    // DE
    PanelInfo->u16HStart           = XC_PNLInfo->u16HStart;
    PanelInfo->u16DE_VStart        = XC_PNLInfo->u16VStart;
    PanelInfo->u16Width            = XC_PNLInfo->u16Width;
    PanelInfo->u16Height           = XC_PNLInfo->u16Height;
    PanelInfo->u16DefaultVFreq     = XC_PNLInfo->u16DefaultVFreq;

}
#endif
//-------------------------------------------------------------------------------------------------
/// Initialize the XC
/// If the length is 0, it won't do anything except the mutex and share memory. Usually it is the action of DFB
/// @param  pXC_InitData                  \b IN: the Initialized Data
/// @param  u32InitDataLen                \b IN: the length of the initialized data
/// @return @ref MS_BOOL
//-------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_Init(XC_INITDATA *pXC_InitData, MS_U32 u32InitDataLen)
{
    char wordISR[] = {"_XC_ISR_Mutex"};


#if(XC_CONTEXT_ENABLE)
    MS_BOOL bFirstInstance = TRUE;
#endif

    MS_U8 u8VerCharIndex = 0 ;
    printf("[XC,Version]\33[0;36m ");
    for (; u8VerCharIndex < 8 ; u8VerCharIndex++)
        printf("%c",_api_xc_version.MS_DDI.change[u8VerCharIndex]);
    printf("\n \33[m");

    //add condition of "u32InitDataLen != 0" is for avoiding DFB init case
    if((u32InitDataLen != sizeof(XC_INITDATA)) && (u32InitDataLen != 0))
    {
        return FALSE;
    }


#if 0 //def MSOS_TYPE_LINUX_KERNEL ToDo Ryan
    init_waitqueue_head(&_XC_EventQueue);
#endif

#if(XC_CONTEXT_ENABLE)
    _MApi_XC_Init_Context(&bFirstInstance);
    if (!bFirstInstance)
    {
        //The XC instance already exists
        //return TRUE;
    }

    if (g_pSContext == NULL || g_pSContext->pCommInfo == NULL)
    {
        msg_video(printf("[MAPI XC][%06d] create context fail\n", __LINE__));
        return FALSE;
    }
#endif
    _XC_Mutex = MsOS_CreateMutex(E_MSOS_FIFO, "_XC_Mutex", MSOS_PROCESS_SHARED);

    if (_XC_Mutex == -1)
    {
        msg_video(printf("[MAPI XC][%06d] create mutex fail\n", __LINE__));
        return FALSE;
    }

    _XC_ISR_Mutex = MsOS_CreateMutex(E_MSOS_FIFO, wordISR, MSOS_PROCESS_SHARED);
    if (_XC_ISR_Mutex == -1)
    {
        msg_video(printf("[MAPI XC][%06d] create ISR mutex fail\n", __LINE__));
        return FALSE;
    }

    // This should be the first function. get MMIO base
    if (MDrv_XC_SetIOMapBase_i() != TRUE)
    {
        msg_video(printf("MApi_XC_Init MDrv_XC_SetIOMapBase() failure\n");)
    }

    // If the length is 0, it won't do anything except the mutex and share memory. Usually it is the action of DFB
    // if the _bDFBInit is TRUE, it also indicates DFB init case
    if((0 == u32InitDataLen) || (TRUE == _bDFBInit))
    {
        _bDFBInit = FALSE;
        msg_video(printf("MApi_XC_Init(): the u32InitDataLen is zero or DFB init case.\n");)
        return TRUE;
    }

    MDrv_XC_FPLLCusReset();

    _bEnableEuro = TRUE;//Euro HDTV support flag


    if (pXC_InitData->stPanelInfo.eLPLL_Type == E_XC_PNL_LPLL_EXT)
    {
    #ifdef ASIC_VERIFY
        pXC_InitData->stPanelInfo.eLPLL_Type = E_XC_PNL_LPLL_TTL;
    #else
        pXC_InitData->stPanelInfo.eLPLL_Type = (E_XC_PNL_LPLL_TYPE)MApi_Pnl_Get_LPLL_Type();
    #endif
    }

    memcpy(&g_XC_InitData, pXC_InitData, u32InitDataLen);
    msg_video(
        printf("Main_FB(start, size)=(%lx, %lu)\n", g_XC_InitData.u32Main_FB_Start_Addr, g_XC_InitData.u32Main_FB_Size);
        printf("Sub_FB (start, size)=(%lx, %lu)\n", g_XC_InitData.u32Sub_FB_Start_Addr, g_XC_InitData.u32Sub_FB_Size);
    );

    // Scart ID port selection is available after version 1.
    if (g_XC_InitData.u32XC_version < 1)
    {
        g_XC_InitData.eScartIDPort_Sel = E_XC_SCARTID_NONE;
    }

    mvideo_sc_variable_init(bFirstInstance, pXC_InitData);


    for(u8VerCharIndex=0; u8VerCharIndex<MAX_WINDOW; u8VerCharIndex++)
    {
        enOldInputSrc[u8VerCharIndex] = INPUT_SOURCE_NONE;
    }

    memset(&stPQADCSamplingInfo, 0, sizeof(PQ_ADC_SAMPLING_INFO));

    if(_bSkipSWReset == FALSE)
    {
        _XC_ENTRY();
        MDrv_SC_SetSkipWaitVsync( MAIN_WINDOW, TRUE);
        MDrv_SC_SetSkipWaitVsync( SUB_WINDOW , TRUE);
        MDrv_XC_SoftwareReset(REST_IP_ALL, MAIN_WINDOW);

        // adc init
        MDrv_ADC_init((MS_U16)(g_XC_InitData.u32XTAL_Clock/1000UL), g_XC_InitData.bIsShareGround,g_XC_InitData.eScartIDPort_Sel);

		#ifdef MULTI_SCALER_SUPPORTED
        MDrv_XC_DisableSubWindow(SUB_WINDOW);
        MDrv_XC_DisableSubWindow(SC1_SUB_WINDOW);
        MDrv_XC_DisableSubWindow(SC2_SUB_WINDOW);
		#else
        MDrv_XC_DisableSubWindow();
		#endif
        MDrv_XC_DisableInputSource(ENABLE, MAIN_WINDOW);
        MDrv_XC_DisableInputSource(ENABLE, SUB_WINDOW);
		#ifdef MULTI_SCALER_SUPPORTED
        MDrv_XC_DisableInputSource(ENABLE, SC1_MAIN_WINDOW);
        MDrv_XC_DisableInputSource(ENABLE, SC1_SUB_WINDOW);
        MDrv_XC_DisableInputSource(ENABLE, SC2_MAIN_WINDOW);
        MDrv_XC_DisableInputSource(ENABLE, SC2_SUB_WINDOW);
		#endif
        // default frame buffer address init
        MDrv_XC_SetFrameBufferAddress(g_XC_InitData.u32Main_FB_Start_Addr, g_XC_InitData.u32Main_FB_Size, MAIN_WINDOW);
        MDrv_XC_SetFrameBufferAddress(g_XC_InitData.u32Sub_FB_Start_Addr, g_XC_InitData.u32Sub_FB_Size, SUB_WINDOW);
		#ifdef MULTI_SCALER_SUPPORTED
        MDrv_XC_SetFrameBufferAddress(g_XC_InitData.u32Main_FB_Start_Addr, g_XC_InitData.u32Main_FB_Size, SC1_MAIN_WINDOW);
        MDrv_XC_SetFrameBufferAddress(g_XC_InitData.u32Sub_FB_Start_Addr, g_XC_InitData.u32Sub_FB_Size, SC2_MAIN_WINDOW);
        MDrv_XC_SetFrameBufferAddress(g_XC_InitData.u32Sub_FB_Start_Addr, g_XC_InitData.u32Sub_FB_Size, SC2_SUB_WINDOW);
		#endif
        MDrv_XC_init_fbn_win(MAIN_WINDOW);
        MDrv_XC_init_fbn_win(SUB_WINDOW);
		#ifdef MULTI_SCALER_SUPPORTED
        MDrv_XC_init_fbn_win(SC1_MAIN_WINDOW);
        MDrv_XC_init_fbn_win(SC2_MAIN_WINDOW);
        MDrv_XC_init_fbn_win(SC2_SUB_WINDOW);
		#endif

        _XC_RETURN();

        if(g_XC_InitData.bEnableIPAutoCoast)
        {
            // enable IP auto coast
            MApi_XC_EnableIPAutoCoast(ENABLE);
        }
        _XC_ENTRY();

        /**
         * !!! Attention !!! The ISR interface is different from Chakra2 and Utopia.
         * In Chakra2, there are two para needed, MHAL_SavedRegisters and vector.
         * In Utopia, there is no parameter.
         */
        //MsOS_AttachInterrupt(E_INT_IRQ_DISP, MDrv_SC_isr); ToDo..Ryan


#if 0 //ndef MSOS_TYPE_LINUX_KERNEL ToDo Ryan
        MsOS_EnableInterrupt(E_INT_IRQ_DISP);
#endif

        // Enable XC interrupts
        //MDrv_WriteByte(BK_SELECT_00, REG_BANK_VOP);
        //MDrv_WriteRegBit(L_BK_VOP(0x25), ENABLE, BIT(7));
        SC_W2BYTEMSK(REG_SC_BK10_25_L, 0x80, BIT(7));

        // Daten FixMe, move from panel init to here, need to check why
        //MDrv_WriteByte(BK_SELECT_00, REG_BANK_S_VOP);
        //MDrv_WriteRegBit(L_BK_S_VOP(0x56), 1, BIT(1));  //Using new ovs_ref
        SC_W2BYTEMSK(REG_SC_BK0F_56_L, 0x02, BIT(1));

        //MDrv_WriteByte(BK_SELECT_00, REG_BANK_IP1F2);
        //MDrv_WriteByteMask(L_BK_IP1F2(0x09), 0x02, BIT(3) | BIT(2) | BIT(1) | BIT(0));  //YLock
        SC_W2BYTEMSK(REG_SC_BK01_09_L, 0x02, BIT(3) | BIT(2) | BIT(1) | BIT(0));
        //Extend the pixel of bottom A session at the right side [7:0]
        //Extend the pixel of bottom B session at the left side[15:8] for PIP issue
        SC_W2BYTE(REG_SC_BK20_14_L, 0x2020);

        //----------------------------
        // Enable clock
        //----------------------------

        // Enable sub window clock
#if 0
        if(MApi_XC_GetInfo()->u8MaxWindowNum > 1)
        {
            // Enable FIClk1
            MDrv_WriteByteMask(REG_CKG_FICLK_F1, CKG_FICLK_F1_FLK, CKG_FICLK_F1_MASK);      // select FClk first, if pre-scaling down -> change to CKG_FICLK_F1_IDCLK1
            MDrv_WriteRegBit(REG_CKG_FICLK_F1, DISABLE, CKG_FICLK_F1_INVERT);               // Not Invert
            MDrv_WriteRegBit(REG_CKG_FICLK_F1, DISABLE, CKG_FICLK_F1_GATED);                // Enable clock

            // Enable IDClk1
            MDrv_WriteByteMask(REG_CKG_IDCLK1, CKG_IDCLK1_XTAL, CKG_IDCLK1_MASK);           // initial to XTAL first, will be changed when switch input source
            MDrv_WriteRegBit(REG_CKG_IDCLK1, DISABLE, CKG_IDCLK1_INVERT);                   // Not Invert
            MDrv_WriteRegBit(REG_CKG_IDCLK1, DISABLE, CKG_IDCLK1_GATED);                    // Enable clock
        }
#endif
        // Enable FIClk2
        // SC0
        MDrv_WriteByteMask(REG_CKG_FICLK_F2, CKG_FICLK_F2_FLK, CKG_FICLK_F2_MASK);          // select FClk first, if pre-scaling down -> change to CKG_FICLK_F1_IDCLK2
        MDrv_WriteRegBit(REG_CKG_FICLK_F2, DISABLE, CKG_FICLK_F2_INVERT);                   // Not Invert
        MDrv_WriteRegBit(REG_CKG_FICLK_F2, DISABLE, CKG_FICLK_F2_GATED);                    // Enable clock

    #ifdef MULTI_SCALER_SUPPORTED
        // SC1 & SC2
        MDrv_WriteByteMask(REG_CKG_SC1_FICLK_F2, CKG_SC1_FICLK_F2_FLK, CKG_SC1_FICLK_F2_MASK);      // select FClk first, if pre-scaling down -> change to CKG_FICLK_F1_IDCLK2
        MDrv_WriteRegBit(REG_CKG_SC1_FICLK_F2, DISABLE, CKG_SC1_FICLK_F2_INVERT);                   // Not Invert
        MDrv_WriteRegBit(REG_CKG_SC1_FICLK_F2, DISABLE, CKG_SC1_FICLK_F2_GATED);                    // Enable clock

        MDrv_WriteByteMask(REG_CKG_SC2_FICLK_F2, CKG_SC2_FICLK_F2_FLK, CKG_SC2_FICLK_F2_MASK);      // select FClk first, if pre-scaling down -> change to CKG_FICLK_F1_IDCLK2
        MDrv_WriteRegBit(REG_CKG_SC2_FICLK_F2, DISABLE, CKG_SC2_FICLK_F2_INVERT);                   // Not Invert
        MDrv_WriteRegBit(REG_CKG_SC2_FICLK_F2, DISABLE, CKG_SC2_FICLK_F2_GATED);                    // Enable clock
    #endif

        // Enable FClk
        // SC0/SC1/SC2 common CLK
        MDrv_WriteByteMask(REG_CKG_FCLK, CKG_FCLK_170MHZ, CKG_FCLK_MASK);                   // select 170MHz, if not enough (for example, scaling bandwidth not enough), increase this
        MDrv_WriteRegBit(REG_CKG_FCLK, DISABLE, CKG_FCLK_INVERT);                           // Not Invert
        MDrv_WriteRegBit(REG_CKG_FCLK, DISABLE, CKG_FCLK_GATED);                            // Enable clock

    #ifdef CHIP_C3
        //SC0/SC1/SC2 common EDCLK
        MDrv_WriteByteMask(REG_CKG_FCLK, CKG_EDCLK_172_BUF, CKG_EDCLK_MASK);                //
        MDrv_WriteRegBit(REG_CKG_EDCLK, DISABLE, CKG_EDCLK_INVERT);                         // Not Invert
        MDrv_WriteRegBit(REG_CKG_EDCLK, DISABLE, CKG_EDCLK_GATED);                          // Enable clock
    #endif

        // Enable IDClk2
        // SC0
		if (!g_XC_InitData.pCbfPreTestInputSourceLocked || !g_XC_InitData.pCbfPreTestInputSourceLocked())
		{
        	MDrv_WriteByteMask(REG_CKG_IDCLK2, CKG_IDCLK2_XTAL, CKG_IDCLK2_MASK);               // initial to XTAL first, will be changed when switch input source
		}
    	MDrv_WriteRegBit(REG_CKG_IDCLK2, DISABLE, CKG_IDCLK2_INVERT);                       // Not Invert
    	MDrv_WriteRegBit(REG_CKG_IDCLK2, DISABLE, CKG_IDCLK2_GATED);                        // Enable clock
    	MDrv_Write2ByteMask(REG_CLKGEN0_51_L, 0x4000, 0xF000);                             // enable ficlk2_f2



    #ifdef MULTI_SCALER_SUPPORTED
        // SC1 & SC2
        MDrv_WriteByteMask(REG_CKG_SC1_IDCLK2, CKG_SC1_IDCLK2_XTAL, CKG_SC1_IDCLK2_MASK);  // initial to XTAL first, will be changed when switch input source
        MDrv_WriteRegBit(REG_CKG_SC1_IDCLK2, DISABLE, CKG_SC1_IDCLK2_INVERT);              // Not Invert
        MDrv_WriteRegBit(REG_CKG_SC1_IDCLK2, DISABLE, CKG_SC1_IDCLK2_GATED);               // Enable clock
        MDrv_Write2ByteMask(REG_CLKGEN1_24_L, 0x4000, 0xF000);                             // enable ficlk2_f2

        MDrv_WriteByteMask(REG_CKG_SC2_IDCLK2, CKG_SC2_IDCLK2_XTAL, CKG_SC2_IDCLK2_MASK);  // initial to XTAL first, will be changed when switch input source
        MDrv_WriteRegBit(REG_CKG_SC2_IDCLK2, DISABLE, CKG_SC2_IDCLK2_INVERT);              // Not Invert
        MDrv_WriteRegBit(REG_CKG_SC2_IDCLK2, DISABLE, CKG_SC2_IDCLK2_GATED);               // Enable clock
    #endif

	#ifdef MULTI_SCALER_SUPPORTED
        MDrv_SC_SetDisplay_LineBuffer_Mode(DISABLE, MAIN_WINDOW); // fil line buffer mode
        MDrv_SC_SetDisplay_LineBuffer_Mode(DISABLE, SC1_MAIN_WINDOW); // fil line buffer mode
        MDrv_SC_SetDisplay_LineBuffer_Mode(ENABLE, SC2_MAIN_WINDOW); // fil line buffer mode
	#else
        MDrv_SC_SetDisplay_LineBuffer_Mode(DISABLE); // fil line buffer mode
	#endif
        // Set PAFRC mixed with noise dither disable---Move the control to pnl mod init
        //SC_W2BYTEMSK(REG_SC_BK24_3F_L, 0x0, 0x8);

        Hal_SC_set_Mclk(TRUE, MAIN_WINDOW);
    #ifdef MULTI_SCALER_SUPPORTED
        Hal_SC_set_Mclk(TRUE, SC1_MAIN_WINDOW);
        Hal_SC_set_Mclk(TRUE, SC2_MAIN_WINDOW);

        Hal_SC1_set_ODCLK(CKG_SC1_ODCLK_HDMI_PLL);
        Hal_SC1_set_ODCLK_src(FALSE, HDMI_OUT_NONE);

        Hal_SC2_set_ODCLK((MS_U32)(((MS_U64)(216000000) *  (MS_U64)(1048576)) / (MS_U64)(858 * 525 * 60)), CKG_SC2_ODCLK_SYN_216MHZ);
    #endif

        if(pXC_InitData->stPanelInfo.u16Width == 1920)
        {
		#ifdef MULTI_SCALER_SUPPORTED
            Hal_SC_Set_extra_fetch_line(0x05, MAIN_WINDOW);
            Hal_SC_Set_extra_adv_line(0x03, MAIN_WINDOW);
		#else
            Hal_SC_Set_extra_fetch_line(0x05);
            Hal_SC_Set_extra_adv_line(0x03);
		#endif
        }
        else
        {
		#ifdef MULTI_SCALER_SUPPORTED
            Hal_SC_Set_extra_fetch_line(0x07, MAIN_WINDOW);
            Hal_SC_Set_extra_adv_line(0x05, MAIN_WINDOW);
		#else
            Hal_SC_Set_extra_fetch_line(0x07);
            Hal_SC_Set_extra_adv_line(0x05);
		#endif
        }

    #ifdef MULTI_SCALER_SUPPORTED
        Hal_SC_Set_extra_fetch_line(0x07, SC1_MAIN_WINDOW);
        Hal_SC_Set_extra_adv_line(0x05, SC1_MAIN_WINDOW);
        Hal_SC_Set_extra_fetch_line(0x07, SC2_MAIN_WINDOW);
        Hal_SC_Set_extra_adv_line(0x05, SC2_MAIN_WINDOW);
	#endif

        MDrv_SC_SetSkipWaitVsync( MAIN_WINDOW, FALSE);
        MDrv_SC_SetSkipWaitVsync( SUB_WINDOW , FALSE);

        _XC_RETURN();
    }
    else
    {
        // default frame buffer address init
        MDrv_XC_SetFrameBufferAddressSilently(g_XC_InitData.u32Main_FB_Start_Addr, g_XC_InitData.u32Main_FB_Size, MAIN_WINDOW);
        MDrv_XC_SetFrameBufferAddressSilently(g_XC_InitData.u32Sub_FB_Start_Addr, g_XC_InitData.u32Sub_FB_Size, SUB_WINDOW);
	#ifdef MULTI_SCALER_SUPPORTED
        MDrv_XC_SetFrameBufferAddressSilently(g_XC_InitData.u32Main_FB_Start_Addr, g_XC_InitData.u32Main_FB_Size, SC1_MAIN_WINDOW);
        MDrv_XC_SetFrameBufferAddressSilently(g_XC_InitData.u32Main_FB_Start_Addr, g_XC_InitData.u32Main_FB_Size, SC2_MAIN_WINDOW);
        MDrv_XC_SetFrameBufferAddressSilently(g_XC_InitData.u32Sub_FB_Start_Addr, g_XC_InitData.u32Sub_FB_Size, SC2_SUB_WINDOW);
	#endif
        //when from mboot to AP, there is an OS switch, so we need to do interrupt handle again.
        /**
         * !!! Attention !!! The ISR interface is different from Chakra2 and Utopia.
         * In Chakra2, there are two para needed, MHAL_SavedRegisters and vector.
         * In Utopia, there is no parameter.
         */
        //MsOS_AttachInterrupt(E_INT_IRQ_DISP, MDrv_SC_isr); //ToDo .. Ryan

#ifndef MSOS_TYPE_LINUX_KERNEL
        MsOS_EnableInterrupt(E_INT_IRQ_DISP);
#endif

    }

#if 1//FRC_INSIDE
// In A5 new feature,pre-arbiter can support (IPM_W and IPS_R)/(IPM_R and IPS_W)/(OP_R and OPW)  use the same MIU request client
//Others, total 6 MIU request client

// Current setting is : Turn off the max request function of pre-arbiter(2 to 1)
    SC_W2BYTE(REG_SC_BK12_63_L,0x0A00); // 0xFF00 // miu merge arbiter IP0
    SC_W2BYTE(REG_SC_BK12_68_L,0x0A00); // 0xFF00 // miu merge arbiter IP1
    SC_W2BYTE(REG_SC_BK12_6A_L,0x1000); // 0xFF00 // miu merge arbiter OP
#endif

    //Mantis issue of 0266525 . Need to toggle IP write mask field count clear before input V sync .
    //Main window
    SC_W2BYTEMSK(REG_SC_BK12_07_L, BIT(4), BIT(4) |BIT(5) |BIT(6));
    SC_W2BYTEMSK(REG_SC_BK12_07_L, 0, BIT(7));
    SC_W2BYTEMSK(REG_SC_BK12_07_L, BIT(7), BIT(7));
    SC_W2BYTEMSK(REG_SC_BK12_07_L, 0, BIT(7));
    //Sub window
    SC_W2BYTEMSK(REG_SC_BK12_47_L, BIT(4), BIT(4) |BIT(5) |BIT(6));
    SC_W2BYTEMSK(REG_SC_BK12_47_L, 0, BIT(7));
    SC_W2BYTEMSK(REG_SC_BK12_47_L, BIT(7), BIT(7));
    SC_W2BYTEMSK(REG_SC_BK12_47_L, 0, BIT(7));

#ifdef MULTI_SCALER_SUPPORTED
    SC_W2BYTEMSK(REG_SC1_BK12_07_L, BIT(4), BIT(4) |BIT(5) |BIT(6));
    SC_W2BYTEMSK(REG_SC1_BK12_07_L, 0, BIT(7));
    SC_W2BYTEMSK(REG_SC1_BK12_07_L, BIT(7), BIT(7));
    SC_W2BYTEMSK(REG_SC1_BK12_07_L, 0, BIT(7));

    SC_W2BYTEMSK(REG_SC2_BK12_07_L, BIT(4), BIT(4) |BIT(5) |BIT(6));
    SC_W2BYTEMSK(REG_SC2_BK12_07_L, 0, BIT(7));
    SC_W2BYTEMSK(REG_SC2_BK12_07_L, BIT(7), BIT(7));
    SC_W2BYTEMSK(REG_SC2_BK12_07_L, 0, BIT(7));
    //Sub window
    SC_W2BYTEMSK(REG_SC2_BK12_47_L, BIT(4), BIT(4) |BIT(5) |BIT(6));
    SC_W2BYTEMSK(REG_SC2_BK12_47_L, 0, BIT(7));
    SC_W2BYTEMSK(REG_SC2_BK12_47_L, BIT(7), BIT(7));
    SC_W2BYTEMSK(REG_SC2_BK12_47_L, 0, BIT(7));
#endif

    SC_W2BYTE(REG_SC_BK36_08_L, 0x00FF); // disable DIPW interrupt

    return TRUE;
}

E_APIXC_ReturnValue MApi_XC_Init_MISC(XC_INITMISC *pXC_Init_Misc, MS_U32 u32InitMiscDataLen)
{

    MST_PANEL_INFO_t stFRCPanelInfo;

    if(u32InitMiscDataLen != sizeof(XC_INITMISC))
    {
        return E_APIXC_RET_FAIL;
    }

    if ( (!SUPPORT_IMMESWITCH) && (pXC_Init_Misc->u32MISC_A & E_XC_INIT_MISC_A_IMMESWITCH ))
    {
        return E_APIXC_RET_FAIL;
    }

    if ( (!SUPPORT_DVI_AUTO_EQ) && (pXC_Init_Misc->u32MISC_A & E_XC_INIT_MISC_A_DVI_AUTO_EQ) )
    {
        return E_APIXC_RET_FAIL;
    }

    if ( (!FRC_INSIDE) && (pXC_Init_Misc->u32MISC_A & E_XC_INIT_MISC_A_FRC_INSIDE) )
    {
        return E_APIXC_RET_FAIL;
    }

    // Start only when all setting ready.
    memset(&stFRCPanelInfo, 0, sizeof(MST_PANEL_INFO_t));

    memcpy(&g_XC_Init_Misc, pXC_Init_Misc, u32InitMiscDataLen);

    #if (FRC_INSIDE)
    if(g_XC_Init_Misc.u32MISC_A & E_XC_INIT_MISC_A_FRC_INSIDE)
    {

        MHal_CLKGEN_FRC_Init(g_XC_InitData.stPanelInfo.u8LPLL_Mode);
        MApi_XC_FRC_PNLInfo_Transform(&g_XC_InitData.stPanelInfo,&g_XC_Pnl_Misc, &stFRCPanelInfo);

        if(g_XC_Pnl_Misc.FRCInfo.bFRC) // init ready?! if yes, continue
        {
            MDrv_FRC_Init(&stFRCPanelInfo, &(g_XC_Pnl_Misc.FRCInfo));
        }

        //printf("\n## FRC %s mode\n",g_XC_Pnl_Misc.FRCInfo.bFRC?"NORMAL":"BYPASS");
        //printf("## FRC LPLL mode =%d\n\n",g_XC_InitData.stPanelInfo.u8LPLL_Mode);
        MDrv_FRC_ByPass_Enable(FALSE);
    }
    else
    {
        MDrv_FRC_ByPass_Enable(TRUE);
    }

    #endif

    return E_APIXC_RET_OK;
}

E_APIXC_ReturnValue MApi_XC_GetMISCStatus(XC_INITMISC *pXC_Init_Misc)
{

    memcpy(pXC_Init_Misc,&g_XC_Init_Misc,sizeof(XC_INITMISC));

    return E_APIXC_RET_OK;
}

MS_U32 MApi_XC_GetCapability(MS_U32 u32Id)
{
	MS_U32 u32RetCapability = 0;

    switch (u32Id)
    {
        case E_XC_SUPPORT_IMMESWITCH:
            u32RetCapability = SUPPORT_IMMESWITCH;
            break;

        case E_XC_SUPPORT_DVI_AUTO_EQ:
            u32RetCapability = SUPPORT_DVI_AUTO_EQ;
            break;

        case  E_XC_SUPPORT_FRC_INSIDE:
            u32RetCapability = FRC_INSIDE;
            break;

        default:
            break;
    }

	return u32RetCapability;
}

//-------------------------------------------------------------------------------------------------
/// Enable sub window clock.
//-------------------------------------------------------------------------------------------------
#ifdef MULTI_SCALER_SUPPORTED
void MApi_XC_EnableCLK_for_SUB(MS_BOOL bEnable, SCALER_WIN eWindow)
{
    MDrv_XC_EnableCLK_for_SUB(bEnable, eWindow);
}

#else
void MApi_XC_EnableCLK_for_SUB(MS_BOOL bEnable)
{
    MDrv_XC_EnableCLK_for_SUB(bEnable);
}
#endif

//-------------------------------------------------------------------------------------------------
/// This function will change panel type dynamically
/// @param  pstPanelInfo      \b IN: the new panel type
//-------------------------------------------------------------------------------------------------
void MApi_XC_ChangePanelType(XC_PANEL_INFO *pstPanelInfo)
{
    XC_PANEL_INFO_EX stPanelInfoEx;
    //Reset information used by ex panel info
    memset(&stPanelInfoEx, 0, sizeof(XC_PANEL_INFO_EX));
    stPanelInfoEx.u16VFreq = PANEL_INFO_EX_INVALID_ALL;
    stPanelInfoEx.u32PanelInfoEx_Version = PANEL_INFO_EX_VERSION;
    stPanelInfoEx.u16PanelInfoEX_Length = sizeof(XC_PANEL_INFO_EX);
    MApi_XC_SetExPanelInfo(FALSE, &stPanelInfoEx);

    _XC_ENTRY();
    MDrv_XC_GetDefaultHVSyncInfo();
    _XC_RETURN();

    memcpy(&g_XC_InitData.stPanelInfo, pstPanelInfo, sizeof(XC_PANEL_INFO));
}

//-------------------------------------------------------------------------------------------------
/// This function will let scaler driver know if to decide best pre-scaling down ratio based on FB size of not.
/// Please set this before call MApi_XC_SetWindow()
/// @param  bEnable      \b IN: enable this feature or not
//-------------------------------------------------------------------------------------------------
void MApi_XC_SetAutoPreScaling(MS_BOOL bEnable, SCALER_WIN eWindow)
{
    gSrcInfo[eWindow].Status2.bAutoBestPreScaling = bEnable;
    printf("MApi_XC_SetAutoPreScaling(): obsolete interface\n");
}

//-------------------------------------------------------------------------------------------------
/// This function will enable/diable output black pattern
/// @param  bEnable      \b IN: TRUE: enable, FALSE: otherwise
//-------------------------------------------------------------------------------------------------
#ifdef MULTI_SCALER_SUPPORTED

void MApi_XC_Set_BLSK(MS_BOOL bEnable, SCALER_WIN eWindow)
{
    _XC_ENTRY();
    MDrv_SC_Set_BLSK(bEnable, eWindow);
    _XC_RETURN();
}

void MApi_XC_GenerateBlackVideoForBothWin( MS_BOOL bEnable, SCALER_WIN eWindow)
{
    SCALER_WIN eMainWin;
    SCALER_WIN eSubWin;

    _XC_ENTRY();

    if(eWindow == MAIN_WINDOW || eWindow == SUB_WINDOW)
    {
        eMainWin = MAIN_WINDOW;
        eSubWin = SUB_WINDOW;
    }
    else if(eWindow == SC1_MAIN_WINDOW || eWindow == SC1_SUB_WINDOW)
    {
        eMainWin = SC1_MAIN_WINDOW;
        eSubWin = SC1_SUB_WINDOW;
    }
    else
    {
        eMainWin = SC2_MAIN_WINDOW;
        eSubWin = SC2_SUB_WINDOW;
    }


    if(E_MLOAD_ENABLED == MApi_XC_MLoad_GetStatus(eWindow))
    {
        MDrv_SC_GenerateBlackVideoForBothWin(bEnable, eWindow);
    }
    else
    {
        MDrv_XC_WaitOutputVSync(1, 50, eMainWin);
        MDrv_SC_GenerateBlackVideo( bEnable, eMainWin);
        MDrv_XC_WaitOutputVSync(1, 50, eSubWin);
        MDrv_SC_GenerateBlackVideo( bEnable, eSubWin);
    }
    gSrcInfo[eMainWin].bBlackscreenEnabled = bEnable;
    gSrcInfo[eSubWin].bBlackscreenEnabled = bEnable;
    _XC_RETURN();
}

#else // #ifdef MULTI_SCALER_SUPPORTED

void MApi_XC_Set_BLSK(MS_BOOL bEnable)
{
    _XC_ENTRY();
    MDrv_SC_Set_BLSK(bEnable);
    _XC_RETURN();
}

void MApi_XC_GenerateBlackVideoForBothWin( MS_BOOL bEnable)
{
    _XC_ENTRY();
    if(E_MLOAD_ENABLED == MApi_XC_MLoad_GetStatus(MAIN_WINDOW))
    {
        MDrv_SC_GenerateBlackVideoForBothWin(bEnable);
    }
    else
    {
        MDrv_XC_WaitOutputVSync(1, 50, MAIN_WINDOW);
        MDrv_SC_GenerateBlackVideo( bEnable, MAIN_WINDOW);
        MDrv_XC_WaitOutputVSync(1, 50, SUB_WINDOW);
        MDrv_SC_GenerateBlackVideo( bEnable, SUB_WINDOW);
    }
    gSrcInfo[MAIN_WINDOW].bBlackscreenEnabled = bEnable;
    gSrcInfo[SUB_WINDOW].bBlackscreenEnabled = bEnable;
    _XC_RETURN();
}
#endif

//-------------------------------------------------------------------------------------------------
/// This function will enable/diable output black pattern
/// @param  bEnable      \b IN: TRUE: enable, FALSE: otherwise
//-------------------------------------------------------------------------------------------------
void MApi_XC_GenerateBlackVideo ( MS_BOOL bEnable, SCALER_WIN eWindow )
{
    _XC_ENTRY();
    //wait VSync and do setting in Vblanking to avoid broken picture

    if(MApi_XC_MLoad_GetStatus(eWindow) != E_MLOAD_ENABLED)
    {
        MDrv_XC_WaitOutputVSync(1, 50, eWindow);
    }

    if ( eWindow < MAX_WINDOW )
    {
        MDrv_SC_GenerateBlackVideo( bEnable, eWindow );
        gSrcInfo[eWindow].bBlackscreenEnabled = bEnable;
    }
    else
    {
		#ifdef MULTI_SCALER_SUPPORTED
        MDrv_SC_GenerateBlackVideoForBothWin(bEnable, MAIN_WINDOW );
        MDrv_SC_GenerateBlackVideoForBothWin(bEnable, SC1_MAIN_WINDOW );
        MDrv_SC_GenerateBlackVideoForBothWin(bEnable, SC2_MAIN_WINDOW );
        gSrcInfo[MAIN_WINDOW].bBlackscreenEnabled = gSrcInfo[SUB_WINDOW].bBlackscreenEnabled = bEnable;
        gSrcInfo[SC1_MAIN_WINDOW].bBlackscreenEnabled = gSrcInfo[SC1_SUB_WINDOW].bBlackscreenEnabled = bEnable;
        gSrcInfo[SC2_MAIN_WINDOW].bBlackscreenEnabled = gSrcInfo[SC2_SUB_WINDOW].bBlackscreenEnabled = bEnable;
		#else

        MDrv_SC_GenerateBlackVideoForBothWin(bEnable);
        gSrcInfo[MAIN_WINDOW].bBlackscreenEnabled = gSrcInfo[SUB_WINDOW].bBlackscreenEnabled = bEnable;
		#endif
    }

    #if TEST_NEW_DYNAMIC_NR   //Why turn on/of DNR here, but not in SetBlueScreen?
    // DNR should be turned on only if video is avaliable.m
    if (mvideo_sc_is_enable_3dnr(MAIN_WINDOW))
    {
        //printk("Enable 3DNR %bu (flag is 0x%x\n", !bEnable, g_SrcInfo.u16ColorFinetuneFlag & FINETUNE_3DNR);
        MDrv_Scaler_Enable3DNR(!bEnable, MAIN_WINDOW);
    }
    #endif

#if 0
    // Enable FPD function when source is MVOP ouput & "Not" in FBL mode.
    {
        if( (IsSrcTypeDTV(gSrcInfo[eWindow].enInputSourceType) || IsSrcTypeStorage(gSrcInfo[eWindow].enInputSourceType) ) &&
            (!MApi_XC_IsCurrentFrameBufferLessMode()))
        {
            MApi_XC_set_FD_Mask( !bEnable );
        }
    }
#endif
    _XC_RETURN();
}

//-------------------------------------------------------------------------------------------------
/// Query whether current XC is black video enabled or not
/// @return @ref MS_BOOL
//-------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_IsBlackVideoEnable( SCALER_WIN eWindow )
{
    return gSrcInfo[eWindow].bBlackscreenEnabled;
}

//-------------------------------------------------------------------------------------------------
/// wait for input sync
/// @param  u8NumVSyncs        \b IN: the number of VSync we are going to wait
/// @param  u16Timeout         \b IN: time out
/// @return @ref MS_U8 retun the number of left Vsync if timeout
//-------------------------------------------------------------------------------------------------
MS_U8 MApi_XC_WaitInputVSync(MS_U8 u8NumVSyncs, MS_U16 u16Timeout, SCALER_WIN eWindow)
{
    MS_U8 u8InputVsyncTime;
    _XC_ENTRY();
    u8InputVsyncTime = MDrv_XC_wait_input_vsync(u8NumVSyncs, u16Timeout, eWindow);
    //return MDrv_XC_wait_input_vsync(u8NumVSyncs, u16Timeout, eWindow);
    _XC_RETURN();
    return u8InputVsyncTime;
}

//-------------------------------------------------------------------------------------------------
/// wait for output sync
/// @param  u8NumVSyncs        \b IN: the number of VSync we are going to wait
/// @param  u16Timeout         \b IN: time out
/// @return @ref MS_U8 retun the number of left Vsync if timeout
//-------------------------------------------------------------------------------------------------
MS_U8 MApi_XC_WaitOutputVSync(MS_U8 u8NumVSyncs, MS_U16 u16Timeout, SCALER_WIN eWindow)
{
    MS_U8 u8OutputVsyncTime;
    _XC_ENTRY();
    u8OutputVsyncTime = MDrv_XC_wait_output_vsync(u8NumVSyncs, u16Timeout, eWindow);
    //return MDrv_XC_wait_output_vsync(u8NumVSyncs, u16Timeout, eWindow);
    _XC_RETURN();
    return u8OutputVsyncTime;
}

//-------------------------------------------------------------------------------------------------
/// wait for output sync
/// @param  u8NumVSyncs        \b IN: the number of VSync we are going to wait
/// @param  u16Timeout         \b IN: time out
/// @return @ref MS_U8 retun the number of left Vsync if timeout
//-------------------------------------------------------------------------------------------------
MS_U8 MDrv_XC_WaitOutputVSync(MS_U8 u8NumVSyncs, MS_U16 u16Timeout, SCALER_WIN eWindow)
{
    MS_U8 u8OutputVsyncTime;
    u8OutputVsyncTime = MDrv_XC_wait_output_vsync(u8NumVSyncs, u16Timeout, eWindow);
    return u8OutputVsyncTime;
}

void msAPI_Scaler_Field_Detect(INPUT_SOURCE_TYPE_t enInputSrcType, EN_VIDEO_SCAN_TYPE enScanType, SCALER_WIN eWindow)
{

    MS_U8 reg_IP1F2_1D, reg_IP1F2_23;
    MS_U16 reg_IP1F2_21;
    MS_U16 u16Vtt = 0;

    // Note:
    // IP1F2_1D[13]: Enable auto no signal filter mode.
    // This functionality is to improve timing detection stability.

    if(IsSrcTypeDigitalVD(enInputSrcType))
    {
        msg_video(printf("VD \n"));
        reg_IP1F2_1D = 0xA1;
        reg_IP1F2_21 = 0x0403;
        reg_IP1F2_23 = 0x30;
    }
    else if(IsSrcTypeDTV(enInputSrcType) || IsSrcTypeStorage(enInputSrcType) || IsSrcTypeCamera(enInputSrcType) || IsSrcTypeBT656(enInputSrcType) || IsSrcTypeSC_VOP(enInputSrcType))
    {
        msg_video(printf("DTV "));
        if (enScanType == SCAN_INTERLACE)
        {
            reg_IP1F2_1D = 0xA1;
            reg_IP1F2_21 = 0x0403;
            reg_IP1F2_23 = 0x30;
            msg_video(printf("-> Interlace\n"));
        }
        else
        {
            reg_IP1F2_1D = 0x21;
            reg_IP1F2_21 = 0x0400;
            reg_IP1F2_23 = 0x00;
            msg_video(printf("-> Progressive\n"));
        }
    }
    else if (IsSrcTypeHDMI(enInputSrcType))
    {

        msg_video(printf("HDMI "));
        if (enScanType == SCAN_INTERLACE)
        {
            reg_IP1F2_1D = 0x21;
            reg_IP1F2_21 = 0x0003;      // enable DE -> no field invert, disable DE -> use field invert
            reg_IP1F2_23 = 0x20;
            msg_video(printf("-> Interlace\n"));

            u16Vtt = MDrv_SC_ip_get_verticaltotal(eWindow);
            if((u16Vtt/2)%2 )
            {
                reg_IP1F2_21 |= BIT(8);
            }
            else
            {
                reg_IP1F2_21 &= ~BIT(8);
            }
        }
        else
        {
            reg_IP1F2_1D = 0x21;
            reg_IP1F2_21 = 0x0000;
            reg_IP1F2_23 = 0x00;
            msg_video(printf("-> Progressive\n"));
        }

        reg_IP1F2_21 |= BIT(12);

        if( (MApi_XC_GetHdmiSyncMode() == HDMI_SYNC_HV)
          &&(enScanType == SCAN_INTERLACE) )
        {
            MS_U16 u16Cur_IP1F2_21Val = SC_R2BYTE(REG_SC_BK01_21_L);
            if( u16Cur_IP1F2_21Val&0xC000 )
            {
                if( reg_IP1F2_21&BIT(8) )
                {
                    reg_IP1F2_21 &= ~BIT(8);
                }
                else
                {
                    reg_IP1F2_21 |= BIT(8);
                }
                //printf("daniel_test patch REG_SC_BK01_21_L: FieldInvert\n");
            }
            else
            {
            }
        }

    }
    else if(IsSrcTypeVga(enInputSrcType))
    {
        msg_video(printf("DSUB "));
        if (enScanType == SCAN_INTERLACE)
        {
            reg_IP1F2_1D = 0x21;
            reg_IP1F2_21 = 0x0103;
            reg_IP1F2_23 = 0x10;
            msg_video(printf("-> Interlace\n"));
        }
        else
        {
            reg_IP1F2_1D = 0x21;
            reg_IP1F2_21 = 0x0000;
            reg_IP1F2_23 = 0x00;
            msg_video(printf("-> Progressive\n"));
        }
    }
    else // YPbPr
    {
        msg_video(printf("YPbPr "));

        if(MDrv_XC_ip_GetEuroHdtvStatus(MAIN_WINDOW) == EURO_AUS_HDTV_NORMAL)
        {
            reg_IP1F2_21 = 0x0100;
        }
        else
        {
            reg_IP1F2_21 = 0x0000;
        }
        reg_IP1F2_1D = 0xA1;
        reg_IP1F2_23 = 0x08; //0x20  // Vtt will use "IP1F2_23 * 16" as lower bound.
    }


    MDrv_SC_setfield(reg_IP1F2_1D, reg_IP1F2_21, reg_IP1F2_23, eWindow);

}

//-------------------------------------------------------------------------------------------------
/// Get the specific window's sync status
/// @param  eCurrentSrc             \b IN: the current input source
/// @param  sXC_Sync_Status         \b OUT:store the sync status
/// @param  eWindow                 \b IN: which window we are going to get
//-------------------------------------------------------------------------------------------------
void MApi_XC_GetSyncStatus(INPUT_SOURCE_TYPE_t eCurrentSrc, XC_IP_SYNC_STATUS *sXC_Sync_Status, SCALER_WIN eWindow)
{
	E_MUX_INPUTPORT _enPort;
    _XC_ENTRY();
    _enPort = MApi_XC_Mux_GetHDMIPort(eCurrentSrc);

    MDrv_XC_ip_get_sync_status(sXC_Sync_Status, _enPort , eWindow);
    _XC_RETURN();
}

void MDrv_XC_GetSyncStatus(INPUT_SOURCE_TYPE_t eCurrentSrc, XC_IP_SYNC_STATUS *sXC_Sync_Status, SCALER_WIN eWindow)
{
    E_MUX_INPUTPORT _enPort = MApi_XC_Mux_GetHDMIPort(eCurrentSrc);

    MDrv_XC_ip_get_sync_status(sXC_Sync_Status, _enPort , eWindow);
}

//-------------------------------------------------------------------------------------------------
/// Set input source type
/// @param  enInputSourceType      \b IN: which input source typewe are going to set
/// @param  eWindow                \b IN: which window we are going to set
/// @return void
//-------------------------------------------------------------------------------------------------
void MApi_XC_SetInputSource( INPUT_SOURCE_TYPE_t enInputSourceType,SCALER_WIN eWindow )
{
	MS_U8 u8InputSrcSel, u8SyncSel, u8VideoSel=0, u8isYPbPr, u8HankShakingMD;

    if(enOldInputSrc[eWindow] == enInputSourceType)
    {
        //printf("MApi_XC_SetInputSource[Win %u] return because old src %d -> %d\n", eWindow, enOldInputSrc[eWindow], enInputSourceType);
        //return;
    }
    else
    {
        enOldInputSrc[eWindow] = enInputSourceType;
    }

    //MS_U8 u8InputSrcSel, u8SyncSel, u8VideoSel=0, u8isYPbPr, u8HankShakingMD;
    // Should disable "pixel repetition" for other source
    if(  (!(IsSrcTypeHDMI(enInputSourceType) || IsSrcTypeDVI(enInputSourceType)))
      && (MApi_XC_GetHdmiSyncMode() == HDMI_SYNC_HV) )
    {
        MApi_XC_SetFIRDownSampleDivider(DISABLE,0x00,eWindow);
    }

    _XC_ENTRY();
    if(IsSrcTypeVga(enInputSourceType))
    {
        u8InputSrcSel = IP_ANALOG1;
        u8SyncSel = (AUTO_DETECT << 1) | CSYNC;
        u8isYPbPr = FALSE;
        u8HankShakingMD = FALSE;
    }
    else if(IsSrcTypeYPbPr(enInputSourceType))
    {
    	u8InputSrcSel = IP_ANALOG1;
		u8SyncSel = (SYNC_ON_GREEN << 1) | SOG;
        u8isYPbPr = TRUE;
        u8HankShakingMD = FALSE;
        MApi_XC_ADC_SwitchSource(1); // Force ADC to reset after source switch
    }
    else if(IsSrcTypeHDMI(enInputSourceType))
    {
#if SUPPORT_IP_HDMI_FOR_HV_MODE
        // set to DE mode to sync with IP_HDMI below, to make system status sync.
        // to fix hdmi 720p-->atv-->720p, then MDrv_XC_Switch_DE_HV_Mode_By_Timing()
        // won't set IP_VIDEO.
        //first time bootup and do 3d conversion, sub window will be set to hdmi also.
        //and sub window will override the HV mode of main. wrong
        //HV mode will be taken as a whole system feature, not main/sub window's
        //beside, we have only one HDMI engine.
        if(!((eWindow == SUB_WINDOW) && (IsSrcTypeHDMI(enOldInputSrc[MAIN_WINDOW]))))
        {
            MDrv_XC_SetHdmiSyncMode(HDMI_SYNC_DE);
        }
        u8InputSrcSel = IP_HDMI;
        u8VideoSel = IP_HDMI;
        u8SyncSel = (AUTO_DETECT << 1) | CSYNC;
#else
        if( MApi_XC_GetHdmiSyncMode() == HDMI_SYNC_DE)
        {
            u8InputSrcSel = IP_HDMI;
            u8VideoSel = IP_HDMI;
            u8SyncSel = (AUTO_DETECT << 1) | CSYNC;
        }
        else
        {
            u8InputSrcSel = IP_VIDEO;
            u8VideoSel = IP_MST_VD_A;
            u8SyncSel = (AUTO_DETECT << 1) | CSYNC;
        }
#endif
        u8isYPbPr = TRUE;
        u8HankShakingMD = FALSE;

    }
    else if(IsSrcTypeDigitalVD(enInputSourceType))
    {
        u8InputSrcSel = IP_VIDEO;
        u8VideoSel = IP_MST_VD_A;
        u8SyncSel = (AUTO_DETECT << 1) | CSYNC;
        u8isYPbPr = TRUE;
        u8HankShakingMD = FALSE;
    }
    #ifdef __EXTVD
    else if(IsUseExtVDPort(enInputPortType))
    {
        u8InputSrcSel = IP_VIDEO;
        u8VideoSel = IP_CCIR656_A;
        u8SyncSel = (AUTO_DETECT << 1) | CSYNC;
        u8isYPbPr = TRUE;
        u8HankShakingMD = FALSE;
    }
    #endif
    else if(IsSrcTypeDTV(enInputSourceType) || (IsSrcTypeStorage(enInputSourceType)) || IsSrcTypeSC_VOP(enInputSourceType))
    {
        u8InputSrcSel = IP_VIDEO;
        u8VideoSel = IP_MST_VD_A;
        u8SyncSel = (AUTO_DETECT << 1) | CSYNC;
        u8isYPbPr = TRUE;
        u8HankShakingMD = FALSE;
    }
    else if(IsSrcTypeCamera(enInputSourceType))
    {
        u8InputSrcSel = IP_ANALOG1;
        u8VideoSel = IP_MST_VD_A;
        u8SyncSel = (AUTO_DETECT << 1) | CSYNC;
        u8isYPbPr = TRUE;
        u8HankShakingMD = TRUE;
    }
    else if(IsSrcTypeBT656(enInputSourceType))
    {
        u8InputSrcSel = IP_VIDEO;
        u8VideoSel = IP_CCIR656_A;
        u8SyncSel = (AUTO_DETECT << 1) | CSYNC;
        u8isYPbPr = TRUE;
        u8HankShakingMD = FALSE;

    }
    else if(IsSrcTypeSC_VOP(enInputSourceType))
    {
        u8InputSrcSel = IP_VIDEO;
        u8VideoSel = IP_MST_VD_A;
        u8SyncSel = (AUTO_DETECT << 1) | CSYNC;
        u8isYPbPr = TRUE;
        u8HankShakingMD = FALSE;
    }
    else
    {
        //printf("Unknow Input Type \n");
        _XC_RETURN();
        return;
    }

    MDrv_SC_ip_set_input_source( u8InputSrcSel, u8SyncSel, u8VideoSel, u8isYPbPr, eWindow);

    _MApi_XC_Patch_SCx_Coase_Signal (enInputSourceType, eWindow);

    if(IsSrcTypeVga(enInputSourceType))
    {
        MDrv_SC_ip_set_image_wrap( DISABLE, DISABLE, eWindow );
    }
    else
    {
        MDrv_SC_ip_set_image_wrap( ENABLE, ENABLE, eWindow );
    }
    MDrv_SC_ip_set_handshaking_md(u8HankShakingMD, eWindow);
    MDrv_SC_ip_set_de_bypass_mode(DISABLE, eWindow); // fix HDMI <-> component switch problems

    if(IsSrcTypeVga(enInputSourceType))
    {
        MDrv_SC_ip_set_input_sync_reference_edge( LEADING_EDGE, TAILING_EDGE, eWindow );
        MDrv_SC_ip_set_input_vsync_delay( DELAY_QUARTER_HSYNC, eWindow );
        MDrv_SC_ip_set_de_only_mode( DISABLE, eWindow );
        MDrv_ip_set_input_10bit( ENABLE, eWindow );

        MDrv_SC_ip_set_coast_input( PC_SOURCE, eWindow );
        MDrv_SC_ip_set_ms_filter(ENABLE,0,eWindow);
        MDrv_SC_ip_set_coast_window( 0x0C, 0x0C, eWindow );
        MDrv_SC_ip_set_input_sync_sample_mode( NORMAL_MODE, eWindow );
        MDrv_SC_ip_set_post_glitch_removal( DISABLE, 0x00, eWindow );
    }
    else if(IsSrcTypeYPbPr(enInputSourceType))
    {
        MDrv_SC_ip_set_input_sync_reference_edge( LEADING_EDGE, TAILING_EDGE, eWindow );
        MDrv_SC_ip_set_input_vsync_delay( NO_DELAY, eWindow );
        MDrv_SC_ip_set_ms_filter(ENABLE,0,eWindow);
        MDrv_SC_ip_set_de_only_mode( DISABLE, eWindow );
        MDrv_ip_set_input_10bit( ENABLE, eWindow );

        MDrv_SC_ip_set_coast_input( COMPONENT_VIDEO, eWindow );
        MDrv_SC_ip_set_coast_window( 0x0C, 0x0C, eWindow );
        MDrv_SC_ip_set_input_sync_sample_mode( GLITCH_REMOVAL, eWindow );
        MDrv_SC_ip_set_post_glitch_removal( ENABLE, 0x01, eWindow );
    }
    else if(IsSrcTypeHDMI(enInputSourceType))
    {
        MDrv_SC_ip_set_input_sync_reference_edge( LEADING_EDGE, TAILING_EDGE, eWindow );
        MDrv_SC_ip_set_input_vsync_delay( DELAY_QUARTER_HSYNC, eWindow );

        if( MApi_XC_GetHdmiSyncMode() == HDMI_SYNC_DE)
        {
            MDrv_SC_ip_set_input_vsync_delay( DELAY_QUARTER_HSYNC, eWindow );
            MDrv_SC_ip_set_de_only_mode( ENABLE, eWindow );
            MDrv_SC_ip_set_de_bypass_mode(DISABLE, eWindow);
            MDrv_SC_ip_set_input_sync_sample_mode( NORMAL_MODE, eWindow );
            MDrv_SC_ip_set_post_glitch_removal( ENABLE, 0x07, eWindow );
            Hal_HDMI_Set_YUV422to444_Bypass(TRUE);
        }
        else
        {
            MDrv_SC_ip_set_input_vsync_delay( NO_DELAY, eWindow );
            MDrv_SC_ip_set_de_only_mode( DISABLE, eWindow );
            // Use DE as capture window
            MDrv_SC_ip_set_de_bypass_mode(ENABLE, eWindow);
            // Disable scaling for get correct DE.
            //MDrv_SC_Enable_PreScaling(FALSE, FALSE, eWindow);
            // Set full range capture size for de-bypass mode
            //MDrv_SC_SetFullRangeCapture(eWindow);

            MDrv_SC_ip_set_input_sync_sample_mode( GLITCH_REMOVAL, eWindow );
            MDrv_SC_ip_set_post_glitch_removal( ENABLE, 0x01, eWindow );
            Hal_HDMI_Set_YUV422to444_Bypass(FALSE);
        }

        MDrv_ip_set_input_10bit( ENABLE, eWindow );

        MDrv_SC_ip_set_ms_filter(DISABLE,0,eWindow);

        //MDrv_SC_ip_set_coast_input( PC_SOURCE, eWindow );
        MDrv_SC_ip_set_ms_filter(DISABLE,0,eWindow);
        //MDrv_SC_ip_set_coast_window( 0x0C, 0x0C, eWindow );
        MDrv_SC_ip_set_input_sync_sample_mode( NORMAL_MODE, eWindow );
        MDrv_SC_ip_set_post_glitch_removal( ENABLE, 0x07, eWindow );
    }
    else if(IsSrcTypeBT656(enInputSourceType))
	{
        MDrv_SC_ip_set_image_wrap( DISABLE, DISABLE, eWindow );
        MDrv_SC_ip_set_input_sync_reference_edge( LEADING_EDGE, TAILING_EDGE, eWindow );
        MDrv_SC_ip_set_input_vsync_delay( DELAY_QUARTER_HSYNC, eWindow );
        MDrv_ip_set_input_10bit( DISABLE, eWindow );
        MDrv_SC_ip_set_de_only_mode( DISABLE, eWindow );

	}
    else if(IsSrcTypeDigitalVD(enInputSourceType)
    #ifdef __EXTVD
    | IsUseExtVDPort(enInputPortType)
    #endif
    )
    {
        MDrv_SC_ip_set_input_sync_reference_edge( TAILING_EDGE, TAILING_EDGE, eWindow );
        MDrv_SC_ip_set_input_vsync_delay( DELAY_QUARTER_HSYNC, eWindow );
        MDrv_SC_ip_set_de_only_mode( DISABLE, eWindow );

        #ifdef __EXTVD
        if(IsUseExtVDPort(enInputPortType))
        {
            MS_U16 u16tmp;
            MDrv_ip_set_input_10bit( DISABLE, eWindow );

            //MDrv_WriteByte(BK_SELECT_00, REG_BANK_IP1F2);
            //MDrv_WriteByte(L_BK_IP1F2(0x08), (MDrv_ReadByte(L_BK_IP1F2(0x08)) & 0x7F));
            //u16tmp = SC_R2BYTE(REG_SC_BK01_08_L);
            //SC_W2BYTEMSK(REG_SC_BK01_08_L, u16tmp & 0x007F, 0x00FF);
        }
        else
        #endif

        {
            MDrv_ip_set_input_10bit( ENABLE, eWindow );
        }
        //MDrv_SC_ip_set_coast_input( PC_SOURCE, eWindow );
        MDrv_SC_ip_set_ms_filter(DISABLE,0,eWindow);
        //MDrv_SC_ip_set_coast_window( 0x0C, 0x0C, eWindow );
        MDrv_SC_ip_set_input_sync_sample_mode( NORMAL_MODE, eWindow );
        MDrv_SC_ip_set_post_glitch_removal( DISABLE, 0x00, eWindow );
    }
    else //DTV
    {
        //set HSYNC reference to leading edge, the capature start will be 1/2 Hsync when input source is from DC0/DC1
        MDrv_SC_ip_set_input_sync_reference_edge( LEADING_EDGE, TAILING_EDGE, eWindow );
        MDrv_SC_ip_set_input_vsync_delay( DELAY_QUARTER_HSYNC, eWindow );
        MDrv_SC_ip_set_de_only_mode( DISABLE, eWindow );
        MDrv_ip_set_input_10bit( ENABLE, eWindow );

        //MDrv_SC_ip_set_coast_input( PC_SOURCE, eWindow );
        MDrv_SC_ip_set_ms_filter(DISABLE,0,eWindow);
        //MDrv_SC_ip_set_coast_window( 0x0C, 0x0C, eWindow );
        MDrv_SC_ip_set_input_sync_sample_mode( NORMAL_MODE, eWindow );
        MDrv_SC_ip_set_post_glitch_removal( DISABLE, 0x00, eWindow );
    }

    // set  h0027 [2] : true to enable DE only mode Glitch Protect for position
    // to fix HDMI 720P YUV422 HStart error & HDE lack of 1
    if (IsSrcTypeHDMI(enInputSourceType))
        MDrv_SC_ip_set_DE_Mode_Glitch(0x24 , eWindow ); // VSync glitch removal with line less than 2(DE only)
    else
        MDrv_SC_ip_set_DE_Mode_Glitch(0x04 , eWindow ); // CHANNEL_ATTRIBUTE function control

    msAPI_Scaler_Field_Detect( enInputSourceType, SCAN_AUTO, eWindow );

    MDrv_XC_reset_ip( eWindow );

    gSrcInfo[eWindow].enInputSourceType = enInputSourceType;

#if 0
    if(g_bIMMESWITCH_DVI_POWERSAVING)
    {
        if((enInputSourceType < INPUT_SOURCE_HDMI || enInputSourceType > INPUT_SOURCE_HDMI4))
            MDrv_DVI_ForceAllPortsEnterPS();
        else
            u8PSDVIStableCount[enInputSourceType - INPUT_SOURCE_HDMI] = 0;
    }
#endif
    _XC_RETURN();


}



static void msAPI_Scaler_SetMode(INPUT_SOURCE_TYPE_t enInputSourceType, XC_InternalStatus *pSrcInfo, SCALER_WIN eWindow )
{
    MS_BOOL bFBL = pSrcInfo->bFBL;
    MS_BOOL bInterlace = pSrcInfo->bInterlace;
    MS_BOOL bUseYUVSpace;
    MS_U8 interlace_type;
    PQ_WIN ePQWin = eWindow == MAIN_WINDOW     ? PQ_MAIN_WINDOW :
                    eWindow == SUB_WINDOW      ? PQ_SUB_WINDOW :
					#ifdef MULTI_SCALER_SUPPORTED
                    eWindow == SC1_MAIN_WINDOW ? PQ_SC1_MAIN_WINDOW :
                    eWindow == SC2_MAIN_WINDOW ? PQ_SC2_MAIN_WINDOW :
                    eWindow == SC2_SUB_WINDOW  ? PQ_SC2_SUB_WINDOW :
					#endif
                                                 PQ_MAIN_WINDOW;

    //----------------------------------------------------
    // Setup Memory format
    //----------------------------------------------------
    if(s_eMemFmt == E_MS_XC_MEM_FMT_AUTO)
    {
        if(!bInterlace &&
          ((IsSrcTypeHDMI(enInputSourceType) && !g_HdmiPollingStatus.bIsHDMIMode) || // DVI plugged into HDMI connector
          IsSrcTypeVga(enInputSourceType)  ) )
        {   // always keep memory foramt to 444 for corresponding to Y/C post-scaling filter
            pSrcInfo->bMemFmt422 = FALSE;
        }
        else
        {   // if input source is 422, HW will convert to 444 automatically
            pSrcInfo->bMemFmt422 = TRUE;
        }
    }
    else
    {
        if(s_eMemFmt == E_MS_XC_MEM_FMT_422)
            pSrcInfo->bMemFmt422 = TRUE;
        else
            pSrcInfo->bMemFmt422 = FALSE;
    }

    msg_video(printf("MemFmt422=%u\n", (MS_U16)pSrcInfo->bMemFmt422));

    if(s_PQ_Function_Info.pq_set_csc == NULL)
        bUseYUVSpace = 0;
    else
        bUseYUVSpace = s_PQ_Function_Info.pq_set_csc(ePQWin,
                                      gSrcInfo[eWindow].bForceRGBin == ENABLE ? PQ_FOURCE_COLOR_RGB : PQ_FOURCE_COLOR_DEFAULT);

    if (bUseYUVSpace)
    {
        pSrcInfo->bUseYUVSpace = TRUE;
     }
    else
    {
        pSrcInfo->bUseYUVSpace = FALSE;
    }

    if(s_PQ_Function_Info.pq_get_memyuvfmt == NULL)
        pSrcInfo->bMemYUVFmt =  TRUE;
    else
        pSrcInfo->bMemYUVFmt = s_PQ_Function_Info.pq_get_memyuvfmt(ePQWin,
                                  gSrcInfo[eWindow].bForceRGBin == ENABLE ? PQ_FOURCE_COLOR_RGB : PQ_FOURCE_COLOR_DEFAULT);

    //-------------------------------------------
    // Set De-interlaced mode and Memory format
    //-------------------------------------------
    //printf("==> Set PQ MEMORY Mode: %d\n", eWindow);

    if(s_PQ_Function_Info.pq_set_memformat == NULL)
        interlace_type = 0;
    else
        interlace_type = (MS_U8) s_PQ_Function_Info.pq_set_memformat(
            ePQWin,
            pSrcInfo->bMemFmt422,
            bFBL,
            &(pSrcInfo->u8BitPerPixel));
    pSrcInfo->eDeInterlaceMode = (MS_DEINTERLACE_MODE)interlace_type;
    //printf("DI mode =%x, bitPerPixel=%d\n",pSrcInfo->eDeInterlaceMode, pSrcInfo->u8BitPerPixel);

    if(bFBL && (gSrcInfo[eWindow].stDispWin.height != g_XC_InitData.stPanelInfo.u16Height))
    {
        if(SUPPORT_SPECIAL_FRAMELOCK == FALSE)
        {
            MDrv_SC_set_std_display_window(eWindow);  //only support full screen
            msg_video(printf("[FBL]Set to std_display_window(panel size, full screen) \n"));
        }
    }

    _XC_ENTRY();

    // set field shift line  // set shift line between even/odd field
    MDrv_SC_set_shift_line(pSrcInfo->bFBL, pSrcInfo->eDeInterlaceMode, eWindow);

    //------------------------------------
    // Setup Delay line
    //------------------------------------
    /* move to MDrv_SC_Set_LockFreeze_Point()
    if(bFBL)
    {
        pSrcInfo->u8DelayLines = 1;
        //printf("[FBL]Set delay line\n");
    }
    else if( (pSrcInfo->eDeInterlaceMode == MS_DEINT_2DDI_AVG) ||
             (pSrcInfo->eDeInterlaceMode == MS_DEINT_2DDI_BOB))
    {
        pSrcInfo->u8DelayLines = 1;    // 5
    }
    else
    {
        pSrcInfo->u8DelayLines = 2;
    }

    // Setup delay line
    Hal_SC_set_delayline( pSrcInfo->u8DelayLines, eWindow );           // set delay line for trigger point, for 2DDI
    */
    msAPI_Scaler_Field_Detect( enInputSourceType, (pSrcInfo->bInterlace?SCAN_INTERLACE:SCAN_PROGRESSIVE), eWindow );

    //--------------
    // PIP related
    //--------------
    if(PIP_SUPPORTED)
    {
		#ifdef MULTI_SCALER_SUPPORTED
        if(eWindow == MAIN_WINDOW || eWindow == SC2_MAIN_WINDOW)
        {
            MS_U32 u32Reg = eWindow == MAIN_WINDOW ? REG_SC_BK20_11_L : REG_SC2_BK20_11_L;
            if(pSrcInfo->bInterlace == TRUE)
            {
                SC_W2BYTEMSK(u32Reg, BIT(13), BIT(13));
            }
            else
            {
                SC_W2BYTEMSK(u32Reg, 0, BIT(13));
            }
        }
		#else
        if(eWindow == MAIN_WINDOW)
        {
            if(pSrcInfo->bInterlace == TRUE)
            {
                SC_W2BYTEMSK(REG_SC_BK20_11_L, BIT(13), BIT(13));
            }
            else
            {
                SC_W2BYTEMSK(REG_SC_BK20_11_L, 0, BIT(13));
            }
        }
		#endif
    }

    _XC_RETURN();
}

void MApi_XC_Set_PQ_SourceData(
    SCALER_WIN eWindow,
    INPUT_SOURCE_TYPE_t enInputSourceType,
    XC_InternalStatus *pstXC_WinTime_Info)
{
    PQ_WIN ePQWin;
    PQ_INPUT_SOURCE_TYPE enPQSourceType=PQ_INPUT_SOURCE_NONE;
    MS_PQ_Mode_Info stPQModeInfo;

    switch (eWindow)
    {
    default:
    case MAIN_WINDOW:
        ePQWin = PQ_MAIN_WINDOW;
        break;

    case SUB_WINDOW:
        ePQWin = PQ_SUB_WINDOW;
        break;

	#ifdef MULTI_SCALER_SUPPORTED
    case SC1_MAIN_WINDOW:
        ePQWin = PQ_SC1_MAIN_WINDOW;
        break;

    case SC2_MAIN_WINDOW:
        ePQWin = PQ_SC2_MAIN_WINDOW;
        break;

    case SC2_SUB_WINDOW:
        ePQWin = PQ_SC2_SUB_WINDOW;
        break;
	#endif
    }

    if(IsSrcTypeVga(enInputSourceType))
    {
        enPQSourceType = PQ_INPUT_SOURCE_VGA; // VGA
    }
    else if(IsSrcTypeATV(enInputSourceType))
    {
        enPQSourceType = PQ_INPUT_SOURCE_TV; // TV
    }
    else if(IsSrcTypeDTV(enInputSourceType))
    {
        enPQSourceType = PQ_INPUT_SOURCE_DTV; //DTV
    }
    else if(IsSrcTypeAV(enInputSourceType))
    {
        enPQSourceType = PQ_INPUT_SOURCE_CVBS; // AV
    }
    else if(IsSrcTypeScart(enInputSourceType))
    {
        enPQSourceType = PQ_INPUT_SOURCE_SCART; // SCART
    }
    else if(IsSrcTypeSV(enInputSourceType))
    {
        enPQSourceType = PQ_INPUT_SOURCE_SVIDEO; // SV
    }
    else if(IsSrcTypeHDMI(enInputSourceType))
    {
        enPQSourceType = PQ_INPUT_SOURCE_HDMI; // HDMI
    }
    else if(IsSrcTypeYPbPr(enInputSourceType))
    {
        enPQSourceType = PQ_INPUT_SOURCE_YPBPR; // COMP
    }
    else if(IsSrcTypeJpeg(enInputSourceType))
    {
        enPQSourceType = PQ_INPUT_SOURCE_JPEG; // JPEG
    }
    else if(IsSrcTypeStorage(enInputSourceType))
    {
        enPQSourceType = PQ_INPUT_SOURCE_STORAGE; // Storage
    }
    else if(enInputSourceType == INPUT_SOURCE_KTV)
    {
        enPQSourceType = PQ_INPUT_SOURCE_KTV; // KTV
    }
    else if(IsSrcTypeCamera(enInputSourceType))
    {
        enPQSourceType = PQ_INPUT_SOURCE_DTV; // camera
    }
    else if(IsSrcTypeBT656(enInputSourceType))
    {
        enPQSourceType = PQ_INPUT_SOURCE_BT656; //BT656
    }
    else if(IsSrcTypeSC_VOP(enInputSourceType))
    {
        enPQSourceType = PQ_INPUT_SOURCE_DTV; //SC_VOP
    }
    else
    {
        MS_ASSERT(0);
    }

    msg_video(printf("PQSetSource:Input src=%u, win=%u,\n", enInputSourceType, eWindow));
    msg_video(printf("PQSetSource:   PQ src=%u, win=%u\n", enPQSourceType, eWindow));

    stPQModeInfo.bFBL               = pstXC_WinTime_Info->bFBL;
    stPQModeInfo.bInterlace         = pstXC_WinTime_Info->bInterlace;
    stPQModeInfo.u16input_hsize     = pstXC_WinTime_Info->stCapWin.width;
    stPQModeInfo.u16input_vsize     = pstXC_WinTime_Info->stCapWin.height;
    stPQModeInfo.u16input_vfreq     = pstXC_WinTime_Info->u16InputVFreq;
    stPQModeInfo.u16input_vtotal    = pstXC_WinTime_Info->u16InputVTotal;
    stPQModeInfo.u16ouput_vfreq     = pstXC_WinTime_Info->u16InputVFreq;
    stPQModeInfo.u16display_hsize   = pstXC_WinTime_Info->stDispWin.width;
    stPQModeInfo.u16display_vsize   = pstXC_WinTime_Info->stDispWin.height;

    if (s_PQ_Function_Info.pq_set_modeInfo != NULL)
    {
        s_PQ_Function_Info.pq_set_modeInfo(ePQWin, enPQSourceType, &stPQModeInfo);
    }

    if (s_PQ_Function_Info.pq_deside_srctype != NULL)
    {
        s_PQ_Function_Info.pq_deside_srctype(ePQWin, enPQSourceType);
    }

}

///////////////////////////////////////////////////
/// ADC API
///////////////////////////////////////////////////
void MDrv_XC_ADC_SetMode(INPUT_SOURCE_TYPE_t enSourceType, XC_InternalStatus *pSrcInfo, SCALER_WIN eWindow)
{
    //MS_U16 u16Width;//, u16RealWidth;
    MS_U16 u16PixClk = 0, u16RealPixClk = 0; // Add the initial value
    MS_U16 u16HorizontalTotal;
    XC_IP_SYNC_STATUS sXC_Sync_Status;

	// Mantis 0296003: check input source and comapre with previous one //
    if ( !IsSrcTypeVga(enSourceType)   &&
         !IsSrcTypeYPbPr(enSourceType) &&
         !IsSrcTypeScart(enSourceType) )
	{
    	// clear ADC sampling setting
		//printf("[%s:%04d] ***NOT*** allow for non-ADC input !!! \r\n", TEXT(__FUNCTION__), __LINE__ );
		return;
    }


	if ( stPQADCSamplingInfo.enPQSourceType  == (IsSrcTypeVga(enSourceType) ? PQ_INPUT_SOURCE_VGA : PQ_INPUT_SOURCE_YPBPR) &&
		 stPQADCSamplingInfo.stPQModeInfo.bInterlace     == pSrcInfo->bInterlace  &&
		 stPQADCSamplingInfo.stPQModeInfo.u16input_hsize == pSrcInfo->stCapWin.width  &&
		 stPQADCSamplingInfo.stPQModeInfo.u16input_vsize == pSrcInfo->stCapWin.height &&
		 stPQADCSamplingInfo.stPQModeInfo.u16input_vfreq == pSrcInfo->u16InputVFreq
	   )
	{
		MDrv_XC_ADC_Set_Freerun(FALSE);
		//printf("[%s:%04d] same source, disable adc free run !!! \r\n", TEXT(__FUNCTION__), __LINE__ );
		return;
	}


    if ( IsSrcTypeVga(enSourceType) || IsSrcTypeYPbPr(enSourceType) )
    {

        // get sync status
        _XC_ENTRY();
        MDrv_XC_GetSyncStatus(enSourceType, &sXC_Sync_Status, eWindow);
        _XC_RETURN();

        // get real width
        //u16Width = pSrcInfo->stCapWin.width;
        if(pSrcInfo->bHDuplicate)
        {
            if(s_PQ_Function_Info.pq_ioctl)
            {
                stPQADCSamplingInfo.enPQSourceType = IsSrcTypeVga(enSourceType) ? PQ_INPUT_SOURCE_VGA : PQ_INPUT_SOURCE_YPBPR;
                stPQADCSamplingInfo.stPQModeInfo.bFBL               = pSrcInfo->bFBL;
                stPQADCSamplingInfo.stPQModeInfo.bInterlace         = pSrcInfo->bInterlace;
                stPQADCSamplingInfo.stPQModeInfo.u16input_hsize     = pSrcInfo->stCapWin.width;
                stPQADCSamplingInfo.stPQModeInfo.u16input_vsize     = pSrcInfo->stCapWin.height;
                stPQADCSamplingInfo.stPQModeInfo.u16input_vfreq     = pSrcInfo->u16InputVFreq;
                stPQADCSamplingInfo.stPQModeInfo.u16input_vtotal    = pSrcInfo->u16InputVTotal;
                stPQADCSamplingInfo.stPQModeInfo.u16ouput_vfreq     = pSrcInfo->u16InputVFreq;
                stPQADCSamplingInfo.stPQModeInfo.u16display_hsize   = pSrcInfo->stDispWin.width;
                stPQADCSamplingInfo.stPQModeInfo.u16display_vsize   = pSrcInfo->stDispWin.height;

                if(s_PQ_Function_Info.pq_ioctl((PQ_WIN)eWindow, E_PQ_IOCTL_ADC_SAMPLING, (void *)&stPQADCSamplingInfo, sizeof(PQ_ADC_SAMPLING_INFO)) == FALSE)
                {
                    stPQADCSamplingInfo.u16ratio = 2;
                }
            }
            else
            {
                stPQADCSamplingInfo.u16ratio = 2;
            }

            //u16RealWidth = u16Width / stPQADCSamplingInfo.u16ratio;
        }
        else
        {
            //u16RealWidth = u16Width;
            stPQADCSamplingInfo.u16ratio = 1;
        }

        // Set Hsync polarity
        MDrv_XC_ADC_hpolarity_setting(sXC_Sync_Status.u8SyncStatus & XC_MD_HSYNC_POR_BIT);

        // Set user default phase
        MDrv_XC_ADC_SetPhase(pSrcInfo->u8DefaultPhase); // setting ADC phase

        //Sog Filter behavior is before the ADC double sampling, so for Sog Filter, we don't need to use
        //double horizontal total for the bandwidth calculation.
        _XC_ENTRY();
        u16RealPixClk = MDrv_XC_cal_pixclk(pSrcInfo->u16DefaultHtotal, eWindow);
        _XC_RETURN();

        MDrv_XC_ADC_Set_Mode(enSourceType, u16RealPixClk, pSrcInfo->u16DefaultHtotal, stPQADCSamplingInfo.u16ratio);

        // Get Htt
        if(pSrcInfo->bHDuplicate)
        {
            u16HorizontalTotal = pSrcInfo->u16DefaultHtotal * stPQADCSamplingInfo.u16ratio; // for better quality
        }
        else
        {
            u16HorizontalTotal = pSrcInfo->u16DefaultHtotal;
        }

        // Calculate pixel clock, the unit is MHz
        _XC_ENTRY();
        u16PixClk = MDrv_XC_cal_pixclk(u16HorizontalTotal, eWindow);
        _XC_RETURN();

        if (IsSrcTypeVga(enSourceType))
        {
            // ADC reset, Patch PLL unlock problem.
            MDrv_XC_ADC_reset(REST_ADC);
        }
        else
        {
            MDrv_XC_ADC_clamp_placement_setting(u16PixClk);
        }

        MDrv_XC_ADC_vco_ctrl(IsSrcTypeHDMI(enSourceType), u16PixClk);
    }
    else
    {
        MDrv_XC_ADC_Set_Mode(enSourceType, u16RealPixClk, pSrcInfo->u16DefaultHtotal, stPQADCSamplingInfo.u16ratio);
    }
}

void MApi_XC_ADC_SwitchSource(MS_BOOL bUpdateStatus)
{
    MDrv_ADC_SourceSwitch(bUpdateStatus);
}

//-------------------------------------------------------------------------------------------------
/// Set Dynamic Scaling
/// @param  pstDSInfo              \b IN: the information of Dynamic Scaling
/// @param  u32DSInforLen          \b IN: the length of the pstDSInfo
/// @param  eWindow                \b IN: which window we are going to set
/// @return @ref MS_BOOL
//-------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_SetDynamicScaling(XC_DynamicScaling_Info *pstDSInfo, MS_U32 u32DSInfoLen, SCALER_WIN eWindow)
{
    MS_BOOL bStatus;
    if(u32DSInfoLen != sizeof(XC_DynamicScaling_Info))
    {
        return FALSE;
    }
    _XC_ENTRY();
    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"It's dynamic scaling case(%u,%u,%u)\n",
            pstDSInfo->bOP_DS_On,pstDSInfo->bIPS_DS_On,pstDSInfo->bIPM_DS_On);
    bStatus = MDrv_XC_Set_DynamicScaling(
        pstDSInfo->u32DS_Info_BaseAddr,
        pstDSInfo->u8MIU_Select,
        pstDSInfo->u8DS_Index_Depth,
        pstDSInfo->bOP_DS_On,
        pstDSInfo->bIPS_DS_On,
        pstDSInfo->bIPM_DS_On);
    _XC_RETURN();
    return bStatus;
}

//-------------------------------------------------------------------------------------------------
/// Set the specific window
/// @param  pstXC_SetWin_Info      \b IN: the information of the window setting
/// @param  u32InitDataLen         \b IN: the length of the pstXC_SetWin_Info
/// @param  eWindow                \b IN: which window we are going to set
/// @return @ref MS_BOOL
//-------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_SetWindow(XC_SETWIN_INFO *pstXC_SetWin_Info, MS_U32 u32InitDataLen, SCALER_WIN eWindow)
{
#if VERIFY_MVIDEO_FPGA
    MS_BOOL binterlace =FALSE;
#endif
//    MS_BOOL bEnableHighSpeedFclk = FALSE;
    MS_U32 u32CopyLen;

    MDrv_SC_SetPQHSDFlag(FALSE, eWindow); //Reset variable
    // we've to support interface changed without re-compile user application
    // please specify new added interface default value here
    if(u32InitDataLen != sizeof(XC_SETWIN_INFO))
    {
        // customer application different version with our driver
        // start from customized pre H/V scaling
        gSrcInfo[eWindow].Status2.bPreHCusScaling = FALSE;
        gSrcInfo[eWindow].Status2.bPreVCusScaling = FALSE;
    }
    else
    {
        gSrcInfo[eWindow].Status2.bPreHCusScaling = pstXC_SetWin_Info->bPreHCusScaling;
        gSrcInfo[eWindow].Status2.u16PreHCusScalingSrc = pstXC_SetWin_Info->u16PreHCusScalingSrc;
        gSrcInfo[eWindow].Status2.u16PreHCusScalingDst = pstXC_SetWin_Info->u16PreHCusScalingDst;

        if(gSrcInfo[eWindow].Status2.u16PreHCusScalingDst > gSrcInfo[eWindow].Status2.u16PreHCusScalingSrc)
        {
            XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"H can't do pre scaling up, change to auto prescaling\r\n");
            gSrcInfo[eWindow].Status2.bPreHCusScaling = FALSE;
        }

        if (gSrcInfo[eWindow].Status2.bPreHCusScaling)
        {
            if (gSrcInfo[eWindow].Status2.u16PreHCusScalingSrc == 0 || gSrcInfo[eWindow].Status2.u16PreHCusScalingDst == 0)
            {
                XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"PreHCus Src or Dst can not be 0 \n");
                return FALSE;
            }
        }

        gSrcInfo[eWindow].Status2.bPreVCusScaling = pstXC_SetWin_Info->bPreVCusScaling;
        gSrcInfo[eWindow].Status2.u16PreVCusScalingSrc = pstXC_SetWin_Info->u16PreVCusScalingSrc;
        gSrcInfo[eWindow].Status2.u16PreVCusScalingDst = pstXC_SetWin_Info->u16PreVCusScalingDst;

        if(gSrcInfo[eWindow].Status2.u16PreVCusScalingDst > gSrcInfo[eWindow].Status2.u16PreVCusScalingSrc)
        {
            XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"V can't do pre scaling up, change to auto prescaling\n");
            gSrcInfo[eWindow].Status2.bPreVCusScaling = FALSE;
        }

        if (gSrcInfo[eWindow].Status2.bPreVCusScaling)
        {
            if (gSrcInfo[eWindow].Status2.u16PreVCusScalingSrc == 0 || gSrcInfo[eWindow].Status2.u16PreVCusScalingDst == 0  )
            {
                XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"PreVCus Src or Dst can not be 0 \n");
                return FALSE;
            }
        }
    }


    //The component 1080i can't frame lock when capture start is odd.
    if((! IsSrcTypeDTV( gSrcInfo[eWindow].enInputSourceType )) && gSrcInfo[eWindow].bInterlace)
    {
        // the Capture start should be even
        if(pstXC_SetWin_Info->stCapWin.y%2)
          pstXC_SetWin_Info->stCapWin.y += 1;
    }

    // copy user data to internal data
    // here is a patch first, will remove after seperate Internal XC status with set window information
    u32CopyLen = sizeof(XC_SETWIN_INFO) - (sizeof(MS_BOOL)*2) - (sizeof(MS_U16)*4);
    memcpy(&gSrcInfo[eWindow], pstXC_SetWin_Info, u32CopyLen);

    // assert check
    MS_ASSERT((gSrcInfo[eWindow].bHCusScaling==TRUE)?((gSrcInfo[eWindow].u16HCusScalingSrc!=0)&&(gSrcInfo[eWindow].u16HCusScalingDst!=0)):1);
    MS_ASSERT((gSrcInfo[eWindow].bVCusScaling==TRUE)?((gSrcInfo[eWindow].u16VCusScalingSrc!=0)&&(gSrcInfo[eWindow].u16VCusScalingDst!=0)):1);
    MS_ASSERT((gSrcInfo[eWindow].Status2.bPreHCusScaling==TRUE)?((gSrcInfo[eWindow].Status2.u16PreHCusScalingSrc!=0)&&(gSrcInfo[eWindow].Status2.u16PreHCusScalingDst!=0)):1);
    MS_ASSERT((gSrcInfo[eWindow].Status2.bPreVCusScaling==TRUE)?((gSrcInfo[eWindow].Status2.u16PreVCusScalingSrc!=0)&&(gSrcInfo[eWindow].Status2.u16PreVCusScalingDst!=0)):1);

    // if mirror, need to re-cal display window
    if( g_XC_InitData.bMirror[eWindow] )
    {
        if(!MDrv_SC_3D_Is3DProcessByPIP(eWindow))
        {
            if( g_XC_InitData.stPanelInfo.u16Width > ( gSrcInfo[eWindow].stDispWin.x + gSrcInfo[eWindow].stDispWin.width ) )
            {
                gSrcInfo[eWindow].stDispWin.x =
                    g_XC_InitData.stPanelInfo.u16Width - ( gSrcInfo[eWindow].stDispWin.x + gSrcInfo[eWindow].stDispWin.width );
            }
            else
            {
                gSrcInfo[eWindow].stDispWin.x = 0;
            }

            if( g_XC_InitData.stPanelInfo.u16Height > ( gSrcInfo[eWindow].stDispWin.y + gSrcInfo[eWindow].stDispWin.height ) )
            {
                gSrcInfo[eWindow].stDispWin.y =
                    g_XC_InitData.stPanelInfo.u16Height - ( gSrcInfo[eWindow].stDispWin.y + gSrcInfo[eWindow].stDispWin.height );
            }
            else
            {
                gSrcInfo[eWindow].stDispWin.y = 0;
            }
        }
    }

    MDrv_SC_3D_FormatValidation(&gSrcInfo[eWindow], eWindow);
    MDrv_SC_3D_AdjustHShift(&gSrcInfo[eWindow], eWindow);

    if(eWindow == MAIN_WINDOW || eWindow == SUB_WINDOW)
    {
        gSrcInfo[eWindow].stDispWin.x += g_XC_InitData.stPanelInfo.u16HStart;
        gSrcInfo[eWindow].stDispWin.y += g_XC_InitData.stPanelInfo.u16VStart;
    }
    else if(eWindow == SC1_MAIN_WINDOW || eWindow == SC2_MAIN_WINDOW)
    {
        MS_U16 u16Hstart, u16Vstart;
        MDrv_XC_Get_TGen_SCTbl_HV_Start(eWindow, &u16Hstart, &u16Vstart);
        gSrcInfo[eWindow].stDispWin.x += u16Hstart; // from TGen table
        gSrcInfo[eWindow].stDispWin.y += u16Vstart;
    }
    else
    {
        printf("ToDo!!! SC2 display window x/y start !!!!! \n");
    }

    // dump debug msg
    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"============= SetWindow Start (Window : %u, src: %u) =============\n",eWindow, gSrcInfo[eWindow].enInputSourceType);
    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"CapWin  x: %4u  y: %4u  w: %4u  h: %4u \n",gSrcInfo[eWindow].stCapWin.x, gSrcInfo[eWindow].stCapWin.y, gSrcInfo[eWindow].stCapWin.width, gSrcInfo[eWindow].stCapWin.height);
    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"CropWin x: %4u  y: %4u  w: %4u  h: %4u \n",gSrcInfo[eWindow].stCropWin.x, gSrcInfo[eWindow].stCropWin.y, gSrcInfo[eWindow].stCropWin.width, gSrcInfo[eWindow].stCropWin.height);
    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"DispWin x: %4u  y: %4u  w: %4u  h: %4u \n",gSrcInfo[eWindow].stDispWin.x,gSrcInfo[eWindow].stDispWin.y, gSrcInfo[eWindow].stDispWin.width, gSrcInfo[eWindow].stDispWin.height);
    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"Panel   x: %4u  y: %4u  w: %4u  h: %4u \n",g_XC_InitData.stPanelInfo.u16HStart, g_XC_InitData.stPanelInfo.u16VStart, g_XC_InitData.stPanelInfo.u16Width, g_XC_InitData.stPanelInfo.u16Height);
    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"Mirror/Interlace/Hdup = %u/%u/%u\r\n", g_XC_InitData.bMirror[eWindow], pstXC_SetWin_Info->bInterlace, pstXC_SetWin_Info->bHDuplicate);
    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"H/V total = (%u, %u), VFreq = %u\r\n", pstXC_SetWin_Info->u16DefaultHtotal, pstXC_SetWin_Info->u16InputVTotal, pstXC_SetWin_Info->u16InputVFreq);
    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"Pre  H cus scaling %u (%u -> %u)\n", gSrcInfo[eWindow].Status2.bPreHCusScaling, gSrcInfo[eWindow].Status2.u16PreHCusScalingSrc, gSrcInfo[eWindow].Status2.u16PreHCusScalingDst);
    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"Pre  V cus scaling %u (%u -> %u)\n", gSrcInfo[eWindow].Status2.bPreVCusScaling, gSrcInfo[eWindow].Status2.u16PreVCusScalingSrc, gSrcInfo[eWindow].Status2.u16PreVCusScalingDst);
    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"Post H cus scaling %u (%u -> %u)\n", gSrcInfo[eWindow].bHCusScaling, gSrcInfo[eWindow].u16HCusScalingSrc, gSrcInfo[eWindow].u16HCusScalingDst);
    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"Post V cus scaling %u (%u -> %u)\n", gSrcInfo[eWindow].bVCusScaling, gSrcInfo[eWindow].u16VCusScalingSrc, gSrcInfo[eWindow].u16VCusScalingDst);
#if VERIFY_MVIDEO_FPGA  //non-de-interlace function
    if(gSrcInfo[eWindow].bInterlace)
    {
        gSrcInfo[eWindow].bInterlace = 0;
        binterlace =TRUE;//gSrcInfo[eWindow].u16V_DisSize /= 2;
    }
#endif

    // Scaler must update source before width / height checking.
    MApi_XC_Set_PQ_SourceData(eWindow, gSrcInfo[eWindow].enInputSourceType, &gSrcInfo[eWindow]);

    // Capture width & height can not be 0 !!
    if (gSrcInfo[eWindow].stCapWin.width == 0 || gSrcInfo[eWindow].stCapWin.height == 0 ||
        gSrcInfo[eWindow].stCropWin.width == 0 || gSrcInfo[eWindow].stCropWin.height == 0 ||
        gSrcInfo[eWindow].stDispWin.width == 0 || gSrcInfo[eWindow].stDispWin.height == 0 )
    {
        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"Cap/Crop/Disp width or height can not be 0 \n");
        return FALSE;
    }

#if ENABLE_REQUEST_FBL

    if(s_PQ_Function_Info.pq_ioctl)
    {
        s_PQ_Function_Info.pq_ioctl((PQ_WIN)eWindow, E_PQ_IOCTL_RFBL_CTRL, NULL, NULL);
    }

	#ifdef MULTI_SCALER_SUPPORTED
    if ( (MApi_XC_IsRequestFrameBufferLessMode() && MApi_XC_IsCurrentRequest_FrameBufferLessMode(eWindow))
         && (!MApi_XC_IsCurrentFrameBufferLessMode()) )
	#else
    if ( (MApi_XC_IsRequestFrameBufferLessMode() && MApi_XC_IsCurrentRequest_FrameBufferLessMode())
         && (!MApi_XC_IsCurrentFrameBufferLessMode()) )
	#endif
    {
        printf("====No Crop==\n");

        gSrcInfo[eWindow].stCropWin.x = 0;
        gSrcInfo[eWindow].stCropWin.y = 0;
        gSrcInfo[eWindow].stCropWin.width = gSrcInfo[eWindow].stCapWin.width;
        gSrcInfo[eWindow].stCropWin.height= gSrcInfo[eWindow].stCapWin.height;
    }

#endif

    //if (IsSrcTypeVga(gSrcInfo[eWindow].enInputSourceType)   ||
    //    IsSrcTypeYPbPr(gSrcInfo[eWindow].enInputSourceType) ||
    //    IsSrcTypeScart(gSrcInfo[eWindow].enInputSourceType) )
    {
        if(!((E_XC_3D_INPUT_MODE_NONE != MApi_XC_Get_3D_Input_Mode(eWindow))
              && (SUB_WINDOW == eWindow)))// don't set adc mode when set sub window at 3d
        {
            XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"ADC Set Mode \r\n");
            MDrv_XC_ADC_SetMode(gSrcInfo[eWindow].enInputSourceType, &gSrcInfo[eWindow], eWindow);
        }
    }

    msAPI_Scaler_SetMode(gSrcInfo[eWindow].enInputSourceType, &gSrcInfo[eWindow], eWindow);

    MDrv_SC_set_prescaling_ratio( gSrcInfo[eWindow].enInputSourceType, &gSrcInfo[eWindow], eWindow );

    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"CapWin  x: %4u  y: %4u  w: %4u  h: %4u \n",gSrcInfo[eWindow].stCapWin.x, gSrcInfo[eWindow].stCapWin.y, gSrcInfo[eWindow].stCapWin.width, gSrcInfo[eWindow].stCapWin.height);
    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"CropWin x: %4u  y: %4u  w: %4u  h: %4u \n",gSrcInfo[eWindow].stCropWin.x, gSrcInfo[eWindow].stCropWin.y, gSrcInfo[eWindow].stCropWin.width, gSrcInfo[eWindow].stCropWin.height);

    MDrv_SC_set_crop_window( &gSrcInfo[eWindow], eWindow );

    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"CapWin  x: %4u  y: %4u  w: %4u  h: %4u \n",gSrcInfo[eWindow].stCapWin.x, gSrcInfo[eWindow].stCapWin.y, gSrcInfo[eWindow].stCapWin.width, gSrcInfo[eWindow].stCapWin.height);
    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"CropWin x: %4u  y: %4u  w: %4u  h: %4u \n",gSrcInfo[eWindow].stCropWin.x, gSrcInfo[eWindow].stCropWin.y, gSrcInfo[eWindow].stCropWin.width, gSrcInfo[eWindow].stCropWin.height);
    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"DispWin x: %4u  y: %4u  w: %4u  h: %4u \n",gSrcInfo[eWindow].stDispWin.x,gSrcInfo[eWindow].stDispWin.y, gSrcInfo[eWindow].stDispWin.width, gSrcInfo[eWindow].stDispWin.height);

    MDrv_SC_set_postscaling_ratio( &gSrcInfo[eWindow], eWindow );

    if (s_PQ_Function_Info.pq_set_420upsampling != NULL)
    {
        PQ_WIN ePQWin;

        ePQWin = (eWindow == MAIN_WINDOW)     ? PQ_MAIN_WINDOW :
                 (eWindow == SUB_WINDOW)      ? PQ_SUB_WINDOW :
				 #ifdef MULTI_SCALER_SUPPORTED
                 (eWindow == SC1_MAIN_WINDOW) ? PQ_SC1_MAIN_WINDOW :
                 (eWindow == SC2_MAIN_WINDOW) ? PQ_SC2_MAIN_WINDOW :
                 (eWindow == SC2_SUB_WINDOW)  ? PQ_SC2_SUB_WINDOW :
				 #endif
                                                PQ_MAIN_WINDOW;
        s_PQ_Function_Info.pq_set_420upsampling(
                        ePQWin,
                        gSrcInfo[eWindow].bFBL,
                        gSrcInfo[eWindow].bPreV_ScalingDown,
                        gSrcInfo[eWindow].ScaledCropWin.y);
    }
    // FIXME: direct use variable instead of stDBreg?
    _XC_ENTRY();

    MDrv_SC_set_fetch_number_limit( &gSrcInfo[eWindow], eWindow );

    MDrv_sc_set_capture_window( eWindow );

#if VERIFY_MVIDEO_FPGA //non-de-interlace function=> celear second frame half garbage
    if(binterlace)
    {
        gSrcInfo[eWindow].stDispWin.height/= 2;
        gSrcInfo[eWindow].u16InputVTotal /= 2;
    }
#endif
    // FIXME: direct use variable instead of stDBreg?
    MDrv_SC_set_display_window( eWindow );

    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"Physical DispWin HStart: %4u  HEnd: %4u  Vstart: %4u  VEnd: %4u \n",stDBreg.u16H_DisStart,stDBreg.u16H_DisEnd, stDBreg.u16V_DisStart, stDBreg.u16V_DisEnd);

//**need to set in SW_DB! move to T3 HAL_SC_SW_DB
#if 0
    if(gSrcInfo[SUB_WINDOW].stDispWin.y <= gSrcInfo[MAIN_WINDOW].stDispWin.y )
    {
        MDrv_XC_FilLineBuffer(ENABLE);
        MDrv_SC_SetDisplay_LineBuffer_Mode(DISABLE);
    }
    else
    {
        MDrv_XC_FilLineBuffer(DISABLE);
        MDrv_SC_SetDisplay_LineBuffer_Mode(ENABLE);
    }
    MDrv_XC_wait_output_vsync(2, 200, eWindow);
#endif
    // Adjust FCLK by different case
    if (gSrcInfo[eWindow].bFBL)
    {
        MDrv_SC_set_Fclk(EN_FCLK_FBL);
    }
    else
    {
        MDrv_SC_set_Fclk(EN_FCLK_NORMAL);
    }

    MDrv_SC_sw_db( &stDBreg, eWindow );


    _XC_RETURN();
    MDrv_XC_3D_LoadReg(MApi_XC_Get_3D_Input_Mode(eWindow), MApi_XC_Get_3D_Output_Mode(), &gSrcInfo[eWindow], eWindow);
    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"============= SetWindow Done (Window : %u, src: %u) =============\n",eWindow, gSrcInfo[eWindow].enInputSourceType);
    return TRUE;
}

#if FRC_INSIDE
void Mapi_XC_FRC_Set_MemoryFMT(E_XC_3D_INPUT_MODE e3dInputMode,
                                E_XC_3D_OUTPUT_MODE e3dOutputMode,
                                E_XC_3D_PANEL_TYPE e3dPanelType)
{

    // RGB and DVI
    if(0) //(IsSrcTypeVga(enInputSourceType) ||
       //(IsSrcTypeHDMI(enInputSourceType)&(!g_HdmiPollingStatus.bIsHDMIMode)))
    {
        XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW, "### is RGB Space\n");
        g_XC_Pnl_Misc.FRCInfo.u8IpMode = FRC_IP_MEM_IP_RGB_10_SPECIAL;
    }
    else
    {
        if(g_XC_Pnl_Misc.FRCInfo.bFRC)
        {
            if(e3dOutputMode == E_XC_3D_OUTPUT_FRAME_ALTERNATIVE)
            {
                XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"### E_XC_3D_OUTPUT_FRAME_ALTERNATIVE\n");
                if((g_XC_Pnl_Misc.FRCInfo.u83D_FI_out == E_XC_3D_OUTPUT_FI_960x1080)||
                   (g_XC_Pnl_Misc.FRCInfo.u83D_FI_out == E_XC_3D_OUTPUT_FI_1280x720))
                {
                    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"### is YUV Space, 960x1080\n");
                    g_XC_Pnl_Misc.FRCInfo.u8IpMode = FRC_IP_MEM_IP_YUV_10_SPECIAL;
                }
                //else if((g_XC_Pnl_Misc.FRCInfo.u83D_FI_out == E_XC_3D_OUTPUT_FI_1920x1080)||
                //   (g_XC_Pnl_Misc.FRCInfo.u83D_FI_out == E_XC_3D_OUTPUT_FI_1920x540))
                else
                {
                    XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"### is YC Space, 1920x1080/1920x540\n");
                    g_XC_Pnl_Misc.FRCInfo.u8IpMode = FRC_IP_MEM_IP_YC_10_SPECIAL;
                }
            }
            else if(e3dOutputMode == E_XC_3D_OUTPUT_SIDE_BY_SIDE_HALF)
            {
                XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"### E_XC_3D_OUTPUT_SIDE_BY_SIDE_HALF\n");
                XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"### is YUV Space, SBS 960x1080\n");
                g_XC_Pnl_Misc.FRCInfo.u8IpMode = FRC_IP_MEM_IP_YUV_10_SPECIAL;
            }
            else
            {
                XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"### is YC Space, others\n");
                g_XC_Pnl_Misc.FRCInfo.u8IpMode = FRC_IP_MEM_IP_YC_10_SPECIAL;
            }
        }
        else
        {
            g_XC_Pnl_Misc.FRCInfo.u8IpMode = FRC_IP_MEM_IP_YC_10_SPECIAL;
            XC_LOG_TRACE(XC_DBGLEVEL_SETWINDOW,"### is YC Space, bypass\n");
        }
    }
#if 0 // for test, test code
    if(MApi_XC_Get_3D_Output_Mode() == E_XC_3D_OUTPUT_SIDE_BY_SIDE_HALF)
    {


    }
    else
    {
        MDrv_FRC_SetMemFormat(&g_XC_Pnl_Misc.FRCInfo);
        if(g_XC_Pnl_Misc.FRCInfo.bFRC)
        {
            if(g_XC_Pnl_Misc.FRCInfo.u83D_FI_out == E_XC_3D_OUTPUT_FI_960x1080)
            {
                g_XC_Pnl_Misc.FRCInfo.u16FB_YcountLinePitch /= 2;
            }

            MDrv_FRC_OPM_SetBaseOfset(&g_XC_Pnl_Misc.FRCInfo, MApi_XC_Get_3D_Output_Mode());
        }
    }
#else
    MDrv_FRC_SetMemFormat(&g_XC_Pnl_Misc.FRCInfo);

    if(g_XC_Pnl_Misc.FRCInfo.bFRC)
    {
        if((g_XC_Pnl_Misc.FRCInfo.u83D_FI_out == E_XC_3D_OUTPUT_FI_960x1080)&&
           (e3dOutputMode == E_XC_3D_OUTPUT_FRAME_ALTERNATIVE))
        {
            g_XC_Pnl_Misc.FRCInfo.u16FB_YcountLinePitch /= 2;
        }
        MDrv_FRC_OPM_SetBaseOfset(&g_XC_Pnl_Misc.FRCInfo, e3dOutputMode);
    }

#endif
}

//-------------------------------------------------------------------------------------------------
/// set 3D convert format type
/// @param  e3dInputMode                \b IN: the input format
/// @param  e3dOutputMode               \b IN: the format we want to convert to
/// @param  e3dPanelType                \b IN: the panel type.
//-------------------------------------------------------------------------------------------------
void MApi_XC_FRC_Set_3D_Mode(E_XC_3D_INPUT_MODE e3dInputMode,
                                       E_XC_3D_OUTPUT_MODE e3dOutputMode,
                                       E_XC_3D_PANEL_TYPE e3dPanelType)
{


    MS_U8 u8FRC_Pnl_type = E_FRC_PNL_TYPE_PASSIVE;
    MS_U8 u83D_FI_out = E_XC_3D_OUTPUT_FI_MODE_NONE;


    if (e3dPanelType == E_XC_3D_PANEL_SHUTTER)
    {
        u8FRC_Pnl_type = E_FRC_PNL_TYPE_ACTIVE;
    }

    u83D_FI_out = g_XC_Pnl_Misc.FRCInfo.u83D_FI_out;

    switch(e3dInputMode)
    {
        case E_XC_3D_INPUT_MODE_NONE:
        case E_XC_3D_INPUT_NORMAL_2D_HW:
            MDrv_FRC_Set_3D_QMap(u8FRC_Pnl_type, e3dInputMode, e3dOutputMode, u83D_FI_out);
        break;

        default:
        case E_XC_3D_INPUT_FRAME_PACKING:
            if(e3dOutputMode == E_XC_3D_OUTPUT_TOP_BOTTOM)
            {
                MDrv_FRC_Set_3D_QMap(u8FRC_Pnl_type, E_XC_3D_INPUT_FRAME_PACKING, E_XC_3D_OUTPUT_TOP_BOTTOM, u83D_FI_out);
            }
            else if(e3dOutputMode == E_XC_3D_OUTPUT_FRAME_ALTERNATIVE)
            {
                MDrv_FRC_Set_3D_QMap(u8FRC_Pnl_type, E_XC_3D_INPUT_FRAME_PACKING, E_XC_3D_OUTPUT_FRAME_ALTERNATIVE, u83D_FI_out);
            }
            else if(e3dOutputMode == E_XC_3D_OUTPUT_SIDE_BY_SIDE_HALF)
            {
                MDrv_FRC_Set_3D_QMap(u8FRC_Pnl_type, E_XC_3D_INPUT_FRAME_PACKING, E_XC_3D_OUTPUT_SIDE_BY_SIDE_HALF, u83D_FI_out);
            }
            else
            {
                printf("\n##[%s]1. Please check the 3D input and out mode\n\n", __FUNCTION__);
            }
        break;
        case E_XC_3D_INPUT_SIDE_BY_SIDE_HALF:
            if(e3dOutputMode == E_XC_3D_OUTPUT_TOP_BOTTOM)
            {
                MDrv_FRC_Set_3D_QMap(u8FRC_Pnl_type, E_XC_3D_INPUT_SIDE_BY_SIDE_HALF, E_XC_3D_OUTPUT_TOP_BOTTOM, u83D_FI_out);

            }
            else if(e3dOutputMode == E_XC_3D_OUTPUT_FRAME_ALTERNATIVE)
            {
                MDrv_FRC_Set_3D_QMap(u8FRC_Pnl_type, E_XC_3D_INPUT_SIDE_BY_SIDE_HALF, E_XC_3D_OUTPUT_FRAME_ALTERNATIVE, u83D_FI_out);

            }
            else if(e3dOutputMode == E_XC_3D_OUTPUT_SIDE_BY_SIDE_HALF)
            {
                MDrv_FRC_Set_3D_QMap(u8FRC_Pnl_type, E_XC_3D_INPUT_SIDE_BY_SIDE_HALF, E_XC_3D_OUTPUT_SIDE_BY_SIDE_HALF, u83D_FI_out);
            }
            else
            {
                printf("\n##[%s]2. Please check the 3D input and out mode\n\n", __FUNCTION__);
            }

        break;
        case E_XC_3D_INPUT_SIDE_BY_SIDE_FULL:
            if(e3dOutputMode == E_XC_3D_OUTPUT_TOP_BOTTOM)
            {
                MDrv_FRC_Set_3D_QMap(u8FRC_Pnl_type, E_XC_3D_INPUT_SIDE_BY_SIDE_FULL, E_XC_3D_OUTPUT_TOP_BOTTOM, u83D_FI_out);

            }
            else if(e3dOutputMode == E_XC_3D_OUTPUT_FRAME_ALTERNATIVE)
            {
                MDrv_FRC_Set_3D_QMap(u8FRC_Pnl_type, E_XC_3D_INPUT_SIDE_BY_SIDE_FULL, E_XC_3D_OUTPUT_FRAME_ALTERNATIVE, u83D_FI_out);
            }
            else
            {
                printf("\n##[%s]2. Please check the 3D input and out mode\n\n", __FUNCTION__);
            }
        break;
        case E_XC_3D_INPUT_TOP_BOTTOM:
            if(e3dOutputMode == E_XC_3D_OUTPUT_TOP_BOTTOM)
            {
                MDrv_FRC_Set_3D_QMap(u8FRC_Pnl_type, E_XC_3D_INPUT_TOP_BOTTOM, E_XC_3D_OUTPUT_TOP_BOTTOM, u83D_FI_out);
            }
            else if(e3dOutputMode == E_XC_3D_OUTPUT_FRAME_ALTERNATIVE)
            {
                MDrv_FRC_Set_3D_QMap(u8FRC_Pnl_type, E_XC_3D_INPUT_TOP_BOTTOM, E_XC_3D_OUTPUT_FRAME_ALTERNATIVE, u83D_FI_out);

            }
            else if(e3dOutputMode == E_XC_3D_OUTPUT_SIDE_BY_SIDE_HALF)
            {
                MDrv_FRC_Set_3D_QMap(u8FRC_Pnl_type, E_XC_3D_OUTPUT_TOP_BOTTOM, E_XC_3D_OUTPUT_SIDE_BY_SIDE_HALF, u83D_FI_out);

            }
            else
            {
                printf("\n##[%s]3. Please check the 3D input and out mode\n\n", __FUNCTION__);
            }
        break;
        case E_XC_3D_INPUT_FRAME_ALTERNATIVE:
            if(e3dOutputMode == E_XC_3D_OUTPUT_TOP_BOTTOM)
            {
                MDrv_FRC_Set_3D_QMap(u8FRC_Pnl_type, E_XC_3D_INPUT_FRAME_ALTERNATIVE, E_XC_3D_OUTPUT_TOP_BOTTOM, u83D_FI_out);
            }
            else if(e3dOutputMode == E_XC_3D_OUTPUT_FRAME_ALTERNATIVE)
            {
                MDrv_FRC_Set_3D_QMap(u8FRC_Pnl_type, E_XC_3D_INPUT_FRAME_ALTERNATIVE, E_XC_3D_OUTPUT_FRAME_ALTERNATIVE, u83D_FI_out);
            }
            else
            {
                printf("\n##[%s]1. Please check the 3D input and out mode\n\n", __FUNCTION__);
            }
        break;


    }

    switch(e3dOutputMode)
    {
        //for 2D to 3D use

        // ParaCnt=4, P0 = 2D to 3D ConversionMode(0=Disable, 1=Enable)
        //                  P1 = 3D Depth Gain(0~31)
        //                  P2 = 3D Depth Offset(0~127)
        //                  P3 = Artificial Gain(0~15)

        case E_XC_3D_OUTPUT_CHECKBOARD_HW:
        case E_XC_3D_OUTPUT_LINE_ALTERNATIVE_HW:
        case E_XC_3D_OUTPUT_PIXEL_ALTERNATIVE_HW:
        case E_XC_3D_OUTPUT_FRAME_L_HW:
        case E_XC_3D_OUTPUT_FRAME_R_HW:
            if(!Hal_XC_FRC_R2_Set_2DTo3DMode(1, 0x13, 0x69, 0x0E))
            {
                printf("Enable 2DTo3D Failed\n");
            }
        break;

        default:
            if(!Hal_XC_FRC_R2_Set_2DTo3DMode(0, 0x13, 0x69, 0x8))
            {
                printf("Disable 2DTo3D Failed\n");
            }
        break;
    }
}
#endif

void MApi_XC_FRC_SetWindow(E_XC_3D_INPUT_MODE e3dInputMode,
                                       E_XC_3D_OUTPUT_MODE e3dOutputMode,
                                       E_XC_3D_PANEL_TYPE e3dPanelType)
{
#if FRC_INSIDE
    MApi_XC_FRC_Set_3D_Mode(e3dInputMode, e3dOutputMode, e3dPanelType);
    Mapi_XC_FRC_Set_MemoryFMT(e3dInputMode, e3dOutputMode, e3dPanelType);
#endif
}

//-------------------------------------------------------------------------------------------------
/// Set report window
/// @param  bEnable        \b IN: enable or disable report window
/// @param  Window         \b IN: the window position and size
/// @param  u8Color        \b IN: report window's color
//-------------------------------------------------------------------------------------------------
#ifdef MULTI_SCALER_SUPPORTED
void MApi_XC_SetRepWindow(MS_BOOL bEnable,MS_WINDOW_TYPE Window,MS_U8 u8Color, SCALER_WIN eWindow)
{
    _XC_ENTRY();
    MDrv_SC_rep_window(bEnable,Window,u8Color, eWindow);
    _XC_RETURN();
}
#else
void MApi_XC_SetRepWindow(MS_BOOL bEnable,MS_WINDOW_TYPE Window,MS_U8 u8Color)
{
    _XC_ENTRY();
    MDrv_SC_rep_window(bEnable,Window,u8Color);
    _XC_RETURN();
}
#endif
//-------------------------------------------------------------------------------------------------
/// update display window registers with input window
/// @param  eWindow                          \b IN: Main or sub window
/// @param  pdspwin                          \b IN: window info that will be setted to registers
//-------------------------------------------------------------------------------------------------
void    MApi_XC_SetDispWinToReg(MS_WINDOW_TYPE *pstDspwin, SCALER_WIN eWindow)
{
    _XC_ENTRY();
    MDrv_XC_set_dispwin_to_reg(eWindow, pstDspwin);
    _XC_RETURN();
}

//-------------------------------------------------------------------------------------------------
/// get current display window registers setting
/// @param  eWindow                          \b IN : Main or sub window
/// @param  pdspwin                          \b OUT: Pointer for ouput disp window register
//-------------------------------------------------------------------------------------------------
void    MApi_XC_GetDispWinFromReg(MS_WINDOW_TYPE *pstDspwin, SCALER_WIN eWindow)
{
    _XC_ENTRY();
    MDrv_XC_get_dispwin_from_reg(eWindow, pstDspwin);
    _XC_RETURN();
}

/// MUX API
///////////////////////////////////////////////////

/**
 * InputSource Mux is a controller which handles all of the video path.
 * Before using video source, user have to create a data path between source and sink.
 * For example, DTV -> MainWindow or DTV -> SubWindow.
 */

/////////////////////////////////////////
// Configuration
/////////////////////////////////////////

// Path
typedef struct s_Data_Path
{

    //////////////////////////////
    // Path Type
    //////////////////////////////
    E_PATH_TYPE e_PathType;

    //////////////////////////////
    // Path Status
    //////////////////////////////

    /*
        Make the signal go through the path. If this flag is not enabled,
        synchronous_event_handler and source_monitor are useless.
     */
    #define DATA_PATH_ENABLE BIT(0)

    /*
        Used to identify a path is created or not. ( 1: path created )
     */
    #define DATA_PATH_USING BIT(1)

    /*
        Used to identify the priodic handler enabled or disabled
     */
    #define ENABLE_PERIODIC_HANDLER BIT(2)

    MS_U8 u8PathStatus;

    //////////////////////////////
    // A path has a format like Src -> Dest
    //////////////////////////////
    INPUT_SOURCE_TYPE_t src;
    E_DEST_TYPE dest;

    //////////////////////////////
    // thread for source.
    //////////////////////////////
    void (*path_thread)(INPUT_SOURCE_TYPE_t src, MS_BOOL bRealTimeMonitorOnly);

    ///////////////////////////////////////
    // Customize function.
    // This will be executed automatically when synchronous event triggered.
    ///////////////////////////////////////
    void (* synchronous_event_handler )(INPUT_SOURCE_TYPE_t src, void* para);

    ///////////////////////////////////////
    // Customize function.
    // This will be executed automatically when destination on/off event triggered.
    ///////////////////////////////////////
    void (* dest_on_off_event_handler )(INPUT_SOURCE_TYPE_t src, void* para);

    ///////////////////////////////////////
    // Customize function.
    // This will be executed periodically after enabled destinatio period handler
    ///////////////////////////////////////
    void (* dest_periodic_handler )(INPUT_SOURCE_TYPE_t src, MS_BOOL bRealTimeMonitorOnly);
}MUX_DATA_PATH;

// Path Collection
typedef struct s_Mux_InputSource_Attr
{
    // The number of pathes which is used right now.
    MS_U8  u8PathCount;

    void (*input_source_to_input_port)(INPUT_SOURCE_TYPE_t src_ids , E_MUX_INPUTPORT* port_ids , MS_U8* u8port_count );
    // Pathes.
    // Path array in MAPP_MUX is constructed [Sync path, Sync path, ..., Async path, Async path]
    MUX_DATA_PATH Mux_DataPaths[MAX_DATA_PATH_SUPPORTED];

    // indicate how many times the src created
    MS_U16 u16SrcCreateTime[INPUT_SOURCE_NUM];
}InputSource_Mux_Attr;


/********************************************************************************/
/*                 Static Variable and Functions (Private)                      */
/********************************************************************************/
static InputSource_Mux_Attr s_InputSource_Mux_attr;

/********************************************************************************/
/*                 Functions                                                    */
/********************************************************************************/

//////////////////////////////////////
//    Data Query Functions
//////////////////////////////////////

//-------------------------------------------------------------------------------------------------
/// Get input port from input source type
/// @param  src                 \b IN: the query based on the input source type
/// @param  port_ids            \b OUT: the port ID we get
/// @param  u8port_count        \b OUT: how many ports we get
//-------------------------------------------------------------------------------------------------
void MApi_XC_Mux_GetPort(INPUT_SOURCE_TYPE_t src, E_MUX_INPUTPORT* port_ids , MS_U8* u8port_count )
{
    if ( s_InputSource_Mux_attr.input_source_to_input_port != NULL )
    {
        s_InputSource_Mux_attr.input_source_to_input_port(src,port_ids,u8port_count);
    }
    else
    {
        *u8port_count = 0;
    }
}

//-------------------------------------------------------------------------------------------------
/// Get input port from input HDMI source type
/// @param  src                 \b IN: the query based on the input HDMI type
/// @return @ref E_MUX_INPUTPORT
//-------------------------------------------------------------------------------------------------
E_MUX_INPUTPORT MApi_XC_Mux_GetHDMIPort( INPUT_SOURCE_TYPE_t src )
{
    // Get Inputport
    E_MUX_INPUTPORT enPorts[4] = {INPUT_PORT_NONE_PORT, INPUT_PORT_NONE_PORT, INPUT_PORT_NONE_PORT, INPUT_PORT_NONE_PORT};
    MS_U8 u8Port_count = 0;
    if ( !IsSrcTypeHDMI(src) )
        return INPUT_PORT_NONE_PORT;

    // Need refine.
    MApi_XC_Mux_GetPort(src , enPorts , &u8Port_count );
    return (enPorts[0]);
}

//////////////////////////////////////
//    Set path & power
//////////////////////////////////////


static void _MApi_XC_Mux_SetPath(INPUT_SOURCE_TYPE_t enInputSourceType, E_DEST_TYPE enOutputType)
{
    MS_U8 u8Port_count = 0;
    E_MUX_INPUTPORT enPorts[3] = {INPUT_PORT_NONE_PORT, INPUT_PORT_NONE_PORT, INPUT_PORT_NONE_PORT};
    MS_U8 MS_U8index = 0 ;
    MS_U8 dest = (MS_U8)enOutputType;
    // Get source ports.

    MApi_XC_Mux_GetPort(enInputSourceType , enPorts , &u8Port_count );

    // Initial : Turn off Y/C mux
    // This is only for one VD
    if ( IsSrcTypeDigitalVD(enInputSourceType) && enOutputType != OUTPUT_CVBS1 && enOutputType != OUTPUT_CVBS2 )
    {
        MDrv_XC_mux_turnoff_ymux();
        MDrv_XC_mux_turnoff_cmux();
    }

	if( IsSrcTypeBT656(enInputSourceType) /*&& enOutputType !=OUTPUT_CVBS1 && enOutputType !=OUTPUT_CVBS2  */)
	{
		MDrv_BT656test_bus();
	}
    // For backward compatibility ( Remove if app layer ready )
    // Because we can not change the API, we add a check here for old behavior.
    if ( IsSrcTypeVga(enInputSourceType) && u8Port_count == 1)
    {
        MDrv_XC_mux_set_sync_port_by_dataport(enPorts[0]);
    }

    for (  ;  MS_U8index < u8Port_count ; MS_U8index++ )
    {
        //printf(" Port index: %d  Port type: %d \n", MS_U8index ,enPorts[MS_U8index]);
        MDrv_XC_mux_dispatch( enPorts[MS_U8index] ,(E_MUX_OUTPUTPORT)dest);
    }

    //printf(" Output port: %d \n", enOutputType);

}

//////////////////////////////////////
//    Path Managerment Functions
//////////////////////////////////////

/*!
 *  Check path is valid or not
 *  @param src The input source of a path
 *  @param dest The destination of a path
 *  @return BOOL true if path is valid.
 */
MS_BOOL _MApi_XC_Mux_CheckValidPath(INPUT_SOURCE_TYPE_t src , E_DEST_TYPE dest)
{
    // After T3, there is no spec about SV -> CVBS out. (HW design)
    if ( IsSrcTypeSV(src) && ( dest == OUTPUT_CVBS1 || dest == OUTPUT_CVBS2 ) )
    {
        return Hal_XC_SVOutput_GetCaps();
    }
    return TRUE;
}

/*!
 *  Search a patch and return the path index if the desire path founded
 *  @param src The input source of a path
 *  @param dest The destination of a path
 *  @return Path Id stored in Mux Controller
 */
static MS_S16 _MApi_XC_Mux_SearchPath(INPUT_SOURCE_TYPE_t src, E_DEST_TYPE dest)
{
    MS_S16 i;

    for ( i = 0 ; i < MAX_DATA_PATH_SUPPORTED; i++)
    {
        if ( s_InputSource_Mux_attr.Mux_DataPaths[i].src == src &&
             s_InputSource_Mux_attr.Mux_DataPaths[i].dest == dest &&
             (s_InputSource_Mux_attr.Mux_DataPaths[i].u8PathStatus & DATA_PATH_USING)
           )
        {
            return i;
        }
    }
    return -1; // No maching path
}

static MS_BOOL _MApi_XC_Mux_IsOtherPathUsingSameSource(INPUT_SOURCE_TYPE_t src, MS_S16 s16MyPathId)
{
	MS_BOOL bRet = FALSE;
	MS_S16  i;

    for ( i = 0 ; i < MAX_DATA_PATH_SUPPORTED; i++)
    {
        if ( (s_InputSource_Mux_attr.Mux_DataPaths[i].src == src) &&
             (s_InputSource_Mux_attr.Mux_DataPaths[i].u8PathStatus & DATA_PATH_USING)
           )
        {
        	//!! its own path, skip and continue compare !!//
			if ( s16MyPathId == i)
				continue;

            bRet = TRUE;
			break;
        }
    }

	return bRet;
}

static MS_BOOL _MApi_XC_Mux_IsDestInUse(E_DEST_TYPE dest)
{
    MS_S16 i;

    for ( i = 0 ; i < MAX_DATA_PATH_SUPPORTED; i++)
    {
        if ( s_InputSource_Mux_attr.Mux_DataPaths[i].dest == dest &&
             (s_InputSource_Mux_attr.Mux_DataPaths[i].u8PathStatus & DATA_PATH_USING)
           )
        {
            return TRUE;
        }
    }
    return FALSE; // No maching path
}

static MS_S16 _MApi_XC_Mux_GetAvailablePath(E_PATH_TYPE e_type)
{
    MS_S16 index = 0, boundary = 0;

    if ( e_type == PATH_TYPE_ASYNCHRONOUS )
    {
        index = MAX_SYNC_DATA_PATH_SUPPORTED ;
        boundary = MAX_DATA_PATH_SUPPORTED ;
    }
    else
    {
        boundary = MAX_SYNC_DATA_PATH_SUPPORTED ;
    }

    for (; index < boundary; index++)
    {
        if (s_InputSource_Mux_attr.Mux_DataPaths[index].u8PathStatus & DATA_PATH_USING)
        {
            continue;
        }
        else
        {
            return index;
        }
    }

    return -1; // No maching path

}

// Return sources connected with Main window or Sub window
static MS_U8 _MApi_XC_Mux_GetUsingSources(INPUT_SOURCE_TYPE_t* src_array )
{
    MS_U8 i = 0, j = 0 , available_source_count = 0;

    for ( ; i < MAX_SYNC_DATA_PATH_SUPPORTED ; i++)
    {
        // Retrieve sources which is using right now.
        if (  ( s_InputSource_Mux_attr.Mux_DataPaths[i].u8PathStatus & ( DATA_PATH_USING ) ) != ( DATA_PATH_USING ) )
            continue;

        // Check if source on path i already in src_array
        for ( j = 0 ; j < available_source_count ; j++ )
        {
            if ( src_array[j] == s_InputSource_Mux_attr.Mux_DataPaths[i].src )
                break;
        }

        if ( j >= available_source_count )
        {
            // no same source in src_array
            // only return source which connected with main or sub window
            if ( s_InputSource_Mux_attr.Mux_DataPaths[i].dest != OUTPUT_CVBS1 &&
                 s_InputSource_Mux_attr.Mux_DataPaths[i].dest != OUTPUT_CVBS2 )
            {
                src_array[available_source_count] = s_InputSource_Mux_attr.Mux_DataPaths[i].src ;
                available_source_count++;
            }
        }
    }
    return available_source_count;
}

static MS_BOOL Is_YPbPr_Path_Existed(E_DEST_TYPE enDestType)
{
	MS_BOOL bRet = FALSE;

	// Mantis 0296003: if CVBSO with input YPbPr path existed //
	if ( -1 != _MApi_XC_Mux_SearchPath(INPUT_SOURCE_YPBPR , enDestType) )
	{
		bRet = TRUE;
	}

	return bRet;
}

static void _MApi_XC_Patch_SCx_Coase_Signal(INPUT_SOURCE_TYPE_t enInputSourceType, SCALER_WIN eWindow)
{
    MS_BOOL bPatch = FALSE;

    if(eWindow == MAIN_WINDOW || eWindow == SUB_WINDOW)
    {
        E_DEST_TYPE enDestType = eWindow == MAIN_WINDOW  ? OUTPUT_SCALER_MAIN_WINDOW : OUTPUT_SCALER_SUB_WINDOW;
    	if ( Is_YPbPr_Path_Existed(enDestType) )
    	{
            bPatch = FALSE;
    	}
    	else
    	{
        	if ( Is_YPbPr_Path_Existed(OUTPUT_SCALER1_MAIN_WINDOW) )
        	{
                bPatch = TRUE;
        	}
        	else if ( Is_YPbPr_Path_Existed(OUTPUT_SCALER2_MAIN_WINDOW) )
        	{
                bPatch = TRUE;
        	}
        	else if ( Is_YPbPr_Path_Existed(OUTPUT_SCALER2_SUB_WINDOW) )
        	{
                bPatch = TRUE;
        	}
        	else
        	{
                bPatch = FALSE;
        	}
    	}
    }
    else
    {
        if(IsSrcTypeYPbPr(enInputSourceType))
        {
        	if ( Is_YPbPr_Path_Existed(OUTPUT_SCALER_MAIN_WINDOW) )
        	{
                bPatch = FALSE;
        	}
        	else if ( Is_YPbPr_Path_Existed(OUTPUT_SCALER_SUB_WINDOW) )
        	{
                bPatch = FALSE;
        	}
        	else
        	{
                bPatch = TRUE;
        	}
        }
        else
        {
            bPatch = FALSE;
        }
    }

    if(bPatch)
    {
        if( _MApi_XC_Mux_IsDestInUse(OUTPUT_SCALER_SUB_WINDOW) == FALSE)
        {
            msg_video(printf("Patch SCx Coase Singal in SC0_Sub \n"));
            MDrv_XC_EnableCLK_for_SUB(TRUE, SUB_WINDOW);
            SC_W2BYTEMSK(REG_SC_BK03_02_L, 0x470, 0x0FFF);
            SC_W2BYTEMSK(REG_SC_BK03_24_L, 0xC21, 0x0FFF);
        }
        else if( _MApi_XC_Mux_IsDestInUse(OUTPUT_SCALER_MAIN_WINDOW) == FALSE)
        {
            msg_video(printf("Patch SCx Coase Singal in SC0_Main \n"));
            SC_W2BYTEMSK(REG_SC_BK01_02_L, 0x470, 0x0FFF);
            SC_W2BYTEMSK(REG_SC_BK01_24_L, 0xC21, 0x0FFF);
        }
        else
        {
            msg_video(printf("Patch SCx Coase Singal Fail !!!!! \n"));
        }
    }
}

static MS_BOOL Is_YPbPr_CVBSO_Path_Existed(void)
{
	MS_BOOL bRet = FALSE;

	// Mantis 0296003: if CVBSO with input YPbPr path existed //
	if ( -1 != _MApi_XC_Mux_SearchPath(INPUT_SOURCE_YPBPR , OUTPUT_SCALER1_MAIN_WINDOW) )
	{
		bRet = TRUE;
	}

	return bRet;
}

static SCALER_WIN convert_to_window(E_DEST_TYPE eDesttype)
{
	SCALER_WIN eWin = MAX_WINDOW;

	switch (eDesttype)
	{
		case OUTPUT_SCALER_MAIN_WINDOW:
			eWin = MAIN_WINDOW;
			break;

		case OUTPUT_SCALER_SUB_WINDOW:
			eWin = SUB_WINDOW;
			break;

		case OUTPUT_SCALER1_MAIN_WINDOW:
			eWin = SC1_MAIN_WINDOW;
			break;

		case OUTPUT_SCALER2_MAIN_WINDOW:
			eWin = SC2_MAIN_WINDOW;
			break;

		case OUTPUT_SCALER2_SUB_WINDOW:
			eWin = SC2_SUB_WINDOW;
			break;
        default:
            eWin = MAX_WINDOW;
            break;
	}

	return eWin;
}
// Public Functions

//-------------------------------------------------------------------------------------------------
/// Initialize Mux before using Mux controller
/// @param  input_source_to_input_port      \b IN: a function to map from input source to input port
//-------------------------------------------------------------------------------------------------
void MApi_XC_Mux_Init(void (*input_source_to_input_port)(INPUT_SOURCE_TYPE_t src_ids , E_MUX_INPUTPORT* port_ids , MS_U8* u8port_count ) )
{
    MS_U16 i;
    s_InputSource_Mux_attr.u8PathCount = 0;
    s_InputSource_Mux_attr.input_source_to_input_port = input_source_to_input_port;
    for ( i = 0 ; i < MAX_DATA_PATH_SUPPORTED; i++)
    {
        s_InputSource_Mux_attr.Mux_DataPaths[i].u8PathStatus = 0;
    }

    for(i=0; i<INPUT_SOURCE_NUM; i++)
    {
        s_InputSource_Mux_attr.u16SrcCreateTime[i] = 0;
    }
}

//-------------------------------------------------------------------------------------------------
/// Monitor the source of Mux
/// @param  bRealTimeMonitorOnly      \b IN: define whether do the monitor routine only for real time
//-------------------------------------------------------------------------------------------------
void MApi_XC_Mux_SourceMonitor(MS_BOOL bRealTimeMonitorOnly)
{
    MS_S16 i;

    for ( i = 0 ; i < MAX_SYNC_DATA_PATH_SUPPORTED ; i++)
    {
        if (  ( s_InputSource_Mux_attr.Mux_DataPaths[i].u8PathStatus & ( DATA_PATH_ENABLE | DATA_PATH_USING ) )
              == ( DATA_PATH_ENABLE | DATA_PATH_USING ) )
        {
            // execute path_thread()
            if (s_InputSource_Mux_attr.Mux_DataPaths[i].path_thread != NULL)
            {
                s_InputSource_Mux_attr.Mux_DataPaths[i].path_thread(s_InputSource_Mux_attr.Mux_DataPaths[i].src, bRealTimeMonitorOnly);
            }

            // execute destination periodic handler
            if(((s_InputSource_Mux_attr.Mux_DataPaths[i].u8PathStatus & ENABLE_PERIODIC_HANDLER) == ENABLE_PERIODIC_HANDLER) &&
               (s_InputSource_Mux_attr.Mux_DataPaths[i].dest_periodic_handler != NULL))
            {
                s_InputSource_Mux_attr.Mux_DataPaths[i].dest_periodic_handler(s_InputSource_Mux_attr.Mux_DataPaths[i].src, bRealTimeMonitorOnly );
            }
        }
    }

    // This is patch for imme-switch. It can be fix in the feature.
    Hal_DVI_IMMESWITCH_PS_SW_Path();
}

//-------------------------------------------------------------------------------------------------
/// A path is disabled after creating it (\ref MApi_XC_Mux_CreatePath)
/// If a path is not enabled,   \ref MApi_XC_Mux_SourceMonitor will bypass it.
/// @param PathId               \b IN: PathId The Path you want to enable
/// @return 1 if enable successfuly. Return -1 if enable fail
//-------------------------------------------------------------------------------------------------
MS_S16 MApi_XC_Mux_EnablePath(MS_U16 PathId)
{
    if ( PathId >= MAX_DATA_PATH_SUPPORTED )
    {
        return -1;
    }
    else
    {
        s_InputSource_Mux_attr.Mux_DataPaths[PathId].u8PathStatus |= DATA_PATH_ENABLE;
        return 1;
    }
}

#if (ENABLE_NONSTD_INPUT_MCNR==1)

static MS_BOOL bIsUnStdSignal = FALSE;
static MS_U8 u8StdSignalStbCnt = 0;
static MS_U8 u8UnStdOutput[MAX_WINDOW] = {0, 1};

extern MS_U8 MDrv_AVD_GetRegValue(MS_U16 u16Addr);
#define BK_AFEC_CC    (0x35CC)

static void _MApi_XC_Sys_Detect_UnStd_Input_Status_Init(void)
{
    bIsUnStdSignal = FALSE;
    u8StdSignalStbCnt = 0;
}

static void _MApi_XC_CVBSUnStdISR(SC_INT_SRC eIntNum, void * pParam)
{
    UNUSED(eIntNum); UNUSED(pParam);

    if((MDrv_AVD_GetRegValue(BK_AFEC_CC) & 0x18) == 0x18)        // non-standard input
    {
        u8StdSignalStbCnt = 0;

        if (bIsUnStdSignal == FALSE)
        {
            SC_W2BYTEMSK(REG_SC_BK12_27_L, 0x01, 0x01);
            bIsUnStdSignal = TRUE;
            msg_video(printf("_MApi_XC_CVBSUnStdISR: unStdSignal\n"));
        }
    }
    else
    {
        if(bIsUnStdSignal == TRUE)
        {
            u8StdSignalStbCnt++;

            if(u8StdSignalStbCnt > 10)
            {
                SC_W2BYTEMSK(REG_SC_BK12_27_L, 0x00, 0x01);
                u8StdSignalStbCnt = 0;
                bIsUnStdSignal = FALSE;
                msg_video(printf("_MApi_XC_CVBSUnStdISR: StdSignal\n"));
            }
        }
    }
}

#endif

//-------------------------------------------------------------------------------------------------
/// delete a path from Mux Controller.
/// @param src              \b IN: Type of input source of a path you want to delete
/// @param dest             \b IN: Type of destination of a path you want to delete
/// @return 1 if delete successfuly. Return -1 if delete fail
//-------------------------------------------------------------------------------------------------
MS_S16 MApi_XC_Mux_DeletePath(INPUT_SOURCE_TYPE_t src, E_DEST_TYPE dest)
{
    // Maximum number of sources is the number of pathes
    INPUT_SOURCE_TYPE_t _InputSource[MAX_SYNC_DATA_PATH_SUPPORTED] ;
    MS_U8 _source_count = 0;
	MS_S16 PathId;
	MS_U8 u8Index;
    SCALER_WIN eWindow;

    _XC_ENTRY();
    PathId = _MApi_XC_Mux_SearchPath(src,dest);

    XC_LOG_TRACE(XC_DBGLEVEL_MUX,"=========== MApi_XC_Mux_DeletePath(ID 0x%x) is called =========== \n", PathId);
    XC_LOG_TRACE(XC_DBGLEVEL_MUX,"Source : %d   Destination : %d \n",src,dest);

    if ( PathId != -1 )
    {
        u8Index = 0;

        // Initial _InputSource
        for (; u8Index < MAX_SYNC_DATA_PATH_SUPPORTED; u8Index++)
        {
            _InputSource[u8Index] = INPUT_SOURCE_NONE;
        }

        // Clear status.
        s_InputSource_Mux_attr.Mux_DataPaths[PathId].u8PathStatus = 0;

        if (s_InputSource_Mux_attr.u8PathCount > 0)
            s_InputSource_Mux_attr.u8PathCount--;

        // Update ADC setting for multi-source.
        if ( s_InputSource_Mux_attr.u8PathCount > 0 )
        {
        	#if ENABLE_ADC_INPUTSOURCE_COUNTER_MODE
			*_InputSource = src;
			_source_count = 1;
			#else
            _source_count = _MApi_XC_Mux_GetUsingSources(_InputSource);
			#endif

            XC_LOG_TRACE(XC_DBGLEVEL_MUX,"Total using source current %d \r\n",_source_count);

			#if ENABLE_ADC_INPUTSOURCE_COUNTER_MODE
			MApi_XC_ADC_DeleteInputSource( _InputSource, _source_count);
			#else
			// Mantis 0296003:  if CVBSO with YPbPr left only, not set ADC input source to avoid flick //
		 	if ( !(Is_YPbPr_CVBSO_Path_Existed() && 1>=_source_count) )
		 	{
				MApi_XC_ADC_SetInputSource( _InputSource, _source_count);
		 	}
			#endif

        }

        if ( IsSrcTypeHDMI(src) || IsSrcTypeDVI(src)  )
        {
            // Power down all HDMI power
            // because HDMI cannot PIP with HDMI, so we can power down all the HDMIs
            Hal_SC_mux_set_dvi_mux(0xFF);
        }

        if ( IsSrcTypeYPbPr(src) || IsSrcTypeVga(src)  )
        {
            // ADC have to set as free run
            if (!_MApi_XC_Mux_IsOtherPathUsingSameSource(src, PathId))
            {
				MDrv_XC_ADC_Set_Freerun(TRUE);
            }
        }

        //Disable SUB_Window CLK
        if(dest == OUTPUT_SCALER_SUB_WINDOW)
        {
        #ifdef MULTI_SCALER_SUPPORTED
            MApi_XC_EnableCLK_for_SUB(DISABLE, SUB_WINDOW);
        #else
            MApi_XC_EnableCLK_for_SUB(DISABLE);
        #endif
        }

        eWindow = convert_to_window(dest);
        MDrv_XC_ClearSizeOfLB(eWindow);

        XC_LOG_TRACE(XC_DBGLEVEL_MUX,"=========== Path deleted success =========== \n");

        // Decrease src create time
        if(s_InputSource_Mux_attr.u16SrcCreateTime[src] > 0)
        {
            s_InputSource_Mux_attr.u16SrcCreateTime[src]--;
        }

#if (ENABLE_NONSTD_INPUT_MCNR == 1)
        MS_U8 i;

        for(i=0; i<_source_count; i++)
        {
            if(IsSrcTypeATV(_InputSource[i]))
            {
                break;
            }
        }

        if(i==_source_count)
        {
            // there is no more ATV input, de-attach ISR
            MDrv_XC_InterruptDeAttach(XC_INT_VSINT, _MApi_XC_CVBSUnStdISR, &u8UnStdOutput[MAIN_WINDOW]);
        }
#endif

        _XC_RETURN();

        return TRUE;
    }
    else
    {
        XC_LOG_TRACE(XC_DBGLEVEL_MUX,"This path is not exist\n");
        XC_LOG_TRACE(XC_DBGLEVEL_MUX,"=========== Path deleted fail =========== \n");
        _XC_RETURN();
        return -1;
    }
}

//-------------------------------------------------------------------------------------------------
/// Trigger Sync Event on Mux Controller.
/// The Mux Controller will pass src and *para to each Sync Event Handler (refer synchronous_event_handler of MUX_DATA_PATH also).
///  ( Delegate Function:
///    Send necessary parameter to SyncEventHandler of each synchronous_path which source is 'src' )
/// @param src          \b IN: The input source which triggers Sync Event
/// @param para         \b IN: A pointer points to parameters which need to pass to Event Handler
//-------------------------------------------------------------------------------------------------
void MApi_XC_Mux_TriggerPathSyncEvent( INPUT_SOURCE_TYPE_t src , void* para)
{
    MS_S16 i;
    for ( i = 0 ; i < MAX_SYNC_DATA_PATH_SUPPORTED ; i++)
    {
        if (  ( s_InputSource_Mux_attr.Mux_DataPaths[i].u8PathStatus & ( DATA_PATH_ENABLE | DATA_PATH_USING ) )
              != ( DATA_PATH_ENABLE | DATA_PATH_USING ) )
        {
            continue;
        }

        // send 'src' to event handler of each path
        if ( s_InputSource_Mux_attr.Mux_DataPaths[i].src == src &&
             s_InputSource_Mux_attr.Mux_DataPaths[i].synchronous_event_handler != NULL)
        {
            s_InputSource_Mux_attr.Mux_DataPaths[i].synchronous_event_handler(src, para);
        }
    }
}

/*!
 *  Trigger Sync Event on Mux Controller.
 *  The Mux Controller will pass src and *para to each Sync Event Handler (refer to synchronous_event_handler
 *   of MUX_DATA_PATH also).
 *
 *  ( Delegate Function:
 *    Send necessary parameter to SyncEventHandler of each synchronous_path which source is 'src' )
 *  @param src The input source which triggers Sync Event
 *  @param para A pointer points to parameters which need to pass to Event Handler
 */
//-------------------------------------------------------------------------------------------------
/// Set the specific window
/// @param  src             \b IN: the source type for handler
/// @param para         \b IN: A pointer points to parameters which need to pass to Event Handler
//-------------------------------------------------------------------------------------------------
void MApi_XC_Mux_TriggerDestOnOffEvent( INPUT_SOURCE_TYPE_t src , void* para)
{
    MS_S16 i;
    for ( i = 0 ; i < MAX_SYNC_DATA_PATH_SUPPORTED ; i++)
    {
        if (  ( s_InputSource_Mux_attr.Mux_DataPaths[i].u8PathStatus & ( DATA_PATH_ENABLE | DATA_PATH_USING ) )
              != ( DATA_PATH_ENABLE | DATA_PATH_USING ) )
        {
            continue;
        }

        // send 'src' to event handler of each path
        if ( s_InputSource_Mux_attr.Mux_DataPaths[i].src == src &&
             s_InputSource_Mux_attr.Mux_DataPaths[i].dest_on_off_event_handler != NULL)
        {
            s_InputSource_Mux_attr.Mux_DataPaths[i].dest_on_off_event_handler(src, para);
        }
    }
}

//-------------------------------------------------------------------------------------------------
/// This function was used to enable/disable the destination periodic handler
/// After enabled periodic hander, Mux Controller will pass the parameters to this handler and execute it periodically
/// @param  src             \b IN: the source type for handler
/// @param  bEnable         \b IN: Enable/Disable the priodic handler.
/// @return 1 if successfuly. Return -1 if failed.
//-------------------------------------------------------------------------------------------------
MS_S16 MApi_XC_Mux_OnOffPeriodicHandler( INPUT_SOURCE_TYPE_t src, MS_BOOL bEnable)
{
    MS_S16 i;
    for ( i = 0 ; i < MAX_SYNC_DATA_PATH_SUPPORTED ; i++)
    {
        if (  ( s_InputSource_Mux_attr.Mux_DataPaths[i].u8PathStatus & ( DATA_PATH_ENABLE | DATA_PATH_USING ) )
              != ( DATA_PATH_ENABLE | DATA_PATH_USING ) )
        {
            continue;
        }

        // send 'src' to event handler of each path
        if(bEnable)
        {
            s_InputSource_Mux_attr.Mux_DataPaths[i].u8PathStatus |= ENABLE_PERIODIC_HANDLER;
        }
        else
        {
            s_InputSource_Mux_attr.Mux_DataPaths[i].u8PathStatus &= ~ENABLE_PERIODIC_HANDLER;
        }
    }

    return i;
}

typedef enum
{
    E_OnPathAdd = 0,
    E_OnPathDel= 1,
    E_PathMax,
}E_PathStatus;


//-------------------------------------------------------------------------------------------------
/// Create a path in Mux Controller.
/// @param  Path_Info               \b IN: the information of the path
/// @param  u32InitDataLen          \b IN: the length of the Path_Info
/// @return @ref MS_S16 return the path id, or -1 when any error is happened
//-------------------------------------------------------------------------------------------------
MS_S16 MApi_XC_Mux_CreatePath(XC_MUX_PATH_INFO* Path_Info , MS_U32 u32InitDataLen)
{
    MS_S16 PathId;
    MS_U8 u8Index=0;

    // Maximum number of sources is the number of pathes
    INPUT_SOURCE_TYPE_t _InputSource[MAX_SYNC_DATA_PATH_SUPPORTED] ;
    MS_U8 _source_count = 0;

    if(u32InitDataLen != sizeof(XC_MUX_PATH_INFO) || Path_Info == NULL)
    {
        // ASSERT when driver is not initiallized
        MS_ASSERT(0);
        return -1;
    }

    _XC_ENTRY();

    XC_LOG_TRACE(XC_DBGLEVEL_MUX,"=========== MApi_XC_Mux_CreatePath is called =========== \n");
    XC_LOG_TRACE(XC_DBGLEVEL_MUX,"Source : %d   Destination : %d \n",Path_Info->src,Path_Info->dest);

    if (s_InputSource_Mux_attr.u8PathCount >= MAX_DATA_PATH_SUPPORTED)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_MUX,"There is no avaliable path. Current Total Path Count : %d ",s_InputSource_Mux_attr.u8PathCount );
        _XC_RETURN();
        return -1; // No avaliable path.
    }

    if ( _MApi_XC_Mux_SearchPath(Path_Info->src,Path_Info->dest) != -1)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_MUX,"Path already exist\n");
        _XC_RETURN();
        return -1; // Path exist
    }

    if ( _MApi_XC_Mux_CheckValidPath(Path_Info->src, Path_Info->dest) == FALSE)
    {
        XC_LOG_TRACE(XC_DBGLEVEL_MUX,"Path is invalid\n");
        _XC_RETURN();
        return -1; // Path is invalid
    }


    if(_XC_Mutex == -1)
    {
        MS_ASSERT(0);
        XC_LOG_TRACE(XC_DBGLEVEL_MUX,"XC dirver have to be initiallized first \n");
        return -1;
    }

    //Add one path
    PathId= _MApi_XC_Mux_GetAvailablePath(Path_Info->Path_Type);

    if ( PathId == -1 )
    {
        XC_LOG_TRACE(XC_DBGLEVEL_MUX,"There is no avaliable path. Current Total Path Count : %d ",s_InputSource_Mux_attr.u8PathCount );
        _XC_RETURN();
        return -1; // no available path
    }

    // Path create.
    s_InputSource_Mux_attr.u8PathCount++;
    s_InputSource_Mux_attr.Mux_DataPaths[PathId].e_PathType = Path_Info->Path_Type;
    s_InputSource_Mux_attr.Mux_DataPaths[PathId].src = Path_Info->src;
    s_InputSource_Mux_attr.Mux_DataPaths[PathId].dest = Path_Info->dest;
    s_InputSource_Mux_attr.Mux_DataPaths[PathId].u8PathStatus = DATA_PATH_USING;
    s_InputSource_Mux_attr.Mux_DataPaths[PathId].path_thread = Path_Info->path_thread;
    s_InputSource_Mux_attr.Mux_DataPaths[PathId].synchronous_event_handler = Path_Info->SyncEventHandler;
    s_InputSource_Mux_attr.Mux_DataPaths[PathId].dest_on_off_event_handler = Path_Info->DestOnOff_Event_Handler;
    s_InputSource_Mux_attr.Mux_DataPaths[PathId].dest_periodic_handler = Path_Info->dest_periodic_handler;


    // Set physical registers after path created.
    _MApi_XC_Mux_SetPath(Path_Info->src,Path_Info->dest);

    // Initial _InputSource
    for (; u8Index < MAX_SYNC_DATA_PATH_SUPPORTED; u8Index++)
    {
        _InputSource[u8Index] = INPUT_SOURCE_NONE;
    }


    // Update ADC setting for multi-source.
    #if ENABLE_ADC_INPUTSOURCE_COUNTER_MODE
	*_InputSource = Path_Info->src;
	_source_count = 1;
	#else
    _source_count = _MApi_XC_Mux_GetUsingSources(_InputSource);
	#endif

    XC_LOG_TRACE(XC_DBGLEVEL_MUX,"Total using source current %d \r\n",_source_count);


	#if ENABLE_ADC_INPUTSOURCE_COUNTER_MODE

	MApi_XC_ADC_AddInputSource( _InputSource, _source_count);
	// Check if need calibration or not
	if( s_InputSource_Mux_attr.u16SrcCreateTime[Path_Info->src] == 0 )
	{
		// calibrate input source
		MApi_XC_ADC_Source_Calibrate(Path_Info->src);

		if(IsSrcTypeYPbPr(Path_Info->src))
		{
			MApi_XC_ADC_SetSoGCal();
			MsOS_DelayTask(10);
		}

		if ( IsSrcTypeYPbPr(Path_Info->src) ||
			 IsSrcTypeVga(Path_Info->src)  )
		{
			// ADC have to set as free run
			MDrv_XC_ADC_Set_Freerun(TRUE);
		}
	}

	#else

	// Mantis 0296003: if CVBSO with YPbPr, not set again to avoid flick //
 	if ( !(Is_YPbPr_CVBSO_Path_Existed() && (IsSrcTypeYPbPr(Path_Info->src) && OUTPUT_SCALER_MAIN_WINDOW==Path_Info->dest)) )
	{
		MApi_XC_ADC_SetInputSource( _InputSource, _source_count);

		// Check if need calibration or not
	    if( s_InputSource_Mux_attr.u16SrcCreateTime[Path_Info->src] == 0 )
	    {
	        // calibrate input source
	        MApi_XC_ADC_Source_Calibrate(Path_Info->src);

	        if(IsSrcTypeYPbPr(Path_Info->src))
	        {
	            MApi_XC_ADC_SetSoGCal();
	            MsOS_DelayTask(10);
	        }

	        if ( IsSrcTypeYPbPr(Path_Info->src) ||
	             IsSrcTypeVga(Path_Info->src)  )
	        {
	            // ADC have to set as free run
	            MDrv_XC_ADC_Set_Freerun(TRUE);
	        }
	    }

 	}

	#endif


    // Increate src create time
    if(s_InputSource_Mux_attr.u16SrcCreateTime[Path_Info->src] < 0xFFFF)
    {
        s_InputSource_Mux_attr.u16SrcCreateTime[Path_Info->src]++;
    }



    //Enable Sub_Window CLK
    if(Path_Info->dest == OUTPUT_SCALER_SUB_WINDOW)
    {
        #ifdef MULTI_SCALER_SUPPORTED
            MApi_XC_EnableCLK_for_SUB(ENABLE, SUB_WINDOW);
        #else
            MApi_XC_EnableCLK_for_SUB(ENABLE);
        #endif
    }

    _XC_RETURN();

    XC_LOG_TRACE(XC_DBGLEVEL_MUX,"=========== Path created success =========== \n");

#if (ENABLE_NONSTD_INPUT_MCNR == 1)
    if(IsSrcTypeATV(Path_Info->src) &&
      (!MApi_XC_InterruptIsAttached(XC_INT_VSINT, _MApi_XC_CVBSUnStdISR, &u8UnStdOutput[MAIN_WINDOW])))
    {
        // Re-init state machine
        _MApi_XC_Sys_Detect_UnStd_Input_Status_Init();

        // Attach ISR
        MApi_XC_InterruptAttach(XC_INT_VSINT, _MApi_XC_CVBSUnStdISR, &u8UnStdOutput[MAIN_WINDOW]);
    }
#endif



    return PathId;
}

//-------------------------------------------------------------------------------------------------
/// Map input source to VD Ymux port
/// @param  u8InputSourceType      \b IN: input source type
/// @return @ref MS_U8
//-------------------------------------------------------------------------------------------------
MS_U8 MApi_XC_MUX_MapInputSourceToVDYMuxPORT( INPUT_SOURCE_TYPE_t u8InputSourceType )
{
    MS_U8 u8Checksrc = INPUT_PORT_NONE_PORT;
    E_MUX_INPUTPORT enPorts[3] ={INPUT_PORT_NONE_PORT, INPUT_PORT_NONE_PORT, INPUT_PORT_NONE_PORT};
    MS_U8 u8Port_count = 0;

    MApi_XC_Mux_GetPort(u8InputSourceType, enPorts , &u8Port_count );
    if(IsSrcTypeAV(u8InputSourceType) || IsSrcTypeSV(u8InputSourceType) || IsSrcTypeYPbPr(u8InputSourceType))
    {
        u8Checksrc = enPorts[0];
    }
    else if(IsSrcTypeVga(u8InputSourceType))
    {
        u8Checksrc = enPorts[1];
    }
    else if(u8InputSourceType == INPUT_SOURCE_HDMI)
    {
        u8Checksrc = INPUT_PORT_DVI0;
    }
    else if(u8InputSourceType == INPUT_SOURCE_HDMI2)
    {
        u8Checksrc = INPUT_PORT_DVI1;
    }
    else if(u8InputSourceType == INPUT_SOURCE_HDMI3)
    {
        u8Checksrc = INPUT_PORT_DVI2;
    }
    else if(u8InputSourceType == INPUT_SOURCE_HDMI4)
    {
        u8Checksrc = INPUT_PORT_DVI3;
    }
    else if(IsSrcTypeStorage(u8InputSourceType))
    {
        u8Checksrc = INPUT_PORT_MVOP;
    }
    else
    {
        msg_video(printf("no such kind of VD Y Mux Port for the input source type:%u\n",u8InputSourceType));
    }

    return u8Checksrc;
}

//-------------------------------------------------------------------------------------------------
/// Get Paths in driver.
/// @param  Paths      \b OUT: Path information
/// @return @ref MS_U8
//-------------------------------------------------------------------------------------------------
MS_U8 MApi_XC_Mux_GetPathInfo(XC_MUX_PATH_INFO* Paths)
{
    MS_S16 i;
    MS_U8 count;

    for ( i = 0, count = 0 ; i < MAX_SYNC_DATA_PATH_SUPPORTED ; i++)
    {
        if (s_InputSource_Mux_attr.Mux_DataPaths[i].u8PathStatus & DATA_PATH_USING)
        {
            Paths[count].src = s_InputSource_Mux_attr.Mux_DataPaths[i].src;
            Paths[count].dest = s_InputSource_Mux_attr.Mux_DataPaths[i].dest;
            Paths[count].path_thread = s_InputSource_Mux_attr.Mux_DataPaths[i].path_thread;
            Paths[count].SyncEventHandler =  s_InputSource_Mux_attr.Mux_DataPaths[i].synchronous_event_handler;
            Paths[count].DestOnOff_Event_Handler =  s_InputSource_Mux_attr.Mux_DataPaths[i].dest_on_off_event_handler;
            Paths[count].dest_periodic_handler =  s_InputSource_Mux_attr.Mux_DataPaths[i].dest_periodic_handler;
            count++;
            //printf(" Path Id: [%d]  Source: %d   Destination: %d \n",i, s_InputSource_Mux_attr.Mux_DataPaths[i].src,
            //s_InputSource_Mux_attr.Mux_DataPaths[i].dest);
        }
    }

    return count;
}

//-------------------------------------------------------------------------------------------------
/// Get XC library version
/// @param  ppVersion                  \b OUT: store the version in this member
/// @return @ref E_APIXC_ReturnValue
//-------------------------------------------------------------------------------------------------
E_APIXC_ReturnValue MApi_XC_GetLibVer(const MSIF_Version **ppVersion)
{
    if (!ppVersion)
    {
        return E_APIXC_RET_FAIL;
    }

    *ppVersion = &_api_xc_version;
    return E_APIXC_RET_OK;
}

//-------------------------------------------------------------------------------------------------
/// Get XC Information
/// @return @ref XC_ApiInfo returnthe XC information in this member
//-------------------------------------------------------------------------------------------------
const XC_ApiInfo * MApi_XC_GetInfo(void)
{
    MDrv_XC_GetInfo(&_stXC_ApiInfo);
    return &_stXC_ApiInfo;
}


//-------------------------------------------------------------------------------------------------
/// Get XC Status of specific window(Main/Sub)
/// @param  pDrvStatusEx                  \b OUT: store the status
/// @param  eWindow                     \b IN: which window(Main/Sub) is going to get status
/// @return @ref MS_BOOL
//-------------------------------------------------------------------------------------------------
MS_U16 MApi_XC_GetStatusEx(XC_ApiStatusEx *pDrvStatusEx, SCALER_WIN eWindow)
{
    XC_ApiStatusEx DrvStatusEx;
    MS_U16 u16CopiedLength = 0;

    if((NULL == pDrvStatusEx) || (pDrvStatusEx->u16ApiStatusEX_Length == 0))
    {
        //We cosider compatible operation form version2 , so reject the info init when version invalid
        XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "MApi_XC_GetStatusEx: Null paramter or Wrong u16ApiStatusEX_Length!!\n")

        return 0;
    }

    // the version control is coming in with version 1
    if(pDrvStatusEx->u32ApiStatusEx_Version < 1)
    {
        //We cosider compatible operation form version1 , so reject the info init when version invalid
        XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "MApi_XC_GetStatusEx: please check your u32ApiStatusEx_Version, it should not set to 0!!\n")

        return 0;
    }

    //new AP + old lib, only the length corresponding to old lib has meaning.
    if(pDrvStatusEx->u32ApiStatusEx_Version > API_STATUS_EX_VERSION)
    {
        //We cosider compatible operation form version1 , so reject the info init when version invalid
        XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "MApi_XC_GetStatusEx: this old version XC lib has only length:%u driver status!!\n",
                     sizeof(XC_ApiStatusEx))
        //we still copy the min size of both structure, but AP should take care of it.
        u16CopiedLength = sizeof(XC_ApiStatusEx);
    }

    //old AP + new lib, driver shouldn't access to the space which doesn't exist in old structure
    if((pDrvStatusEx->u32ApiStatusEx_Version < API_STATUS_EX_VERSION) || (pDrvStatusEx->u16ApiStatusEX_Length < sizeof(XC_ApiStatusEx)))
    {
        //We cosider compatible operation form version1 , so reject the info init when version invalid
        XC_LOG_TRACE(XC_DBGLEVEL_SETTIMING, "MApi_XC_GetStatusEx: new version XC lib shouldn't access to the space which doesn't exist in old structure!!\n")
        u16CopiedLength = (MS_U16)pDrvStatusEx->u32ApiStatusEx_Version;
    }

    DrvStatusEx.u32ApiStatusEx_Version = pDrvStatusEx->u32ApiStatusEx_Version;
    DrvStatusEx.u16ApiStatusEX_Length = pDrvStatusEx->u16ApiStatusEX_Length;


    _XC_ENTRY();
    //----------------------
    // Customer setting
    //----------------------
    DrvStatusEx.enInputSourceType = gSrcInfo[eWindow].enInputSourceType;

    //----------------------
    // Window
    //----------------------
    memcpy(&DrvStatusEx.stCapWin, &gSrcInfo[eWindow].stCapWin, sizeof(MS_WINDOW_TYPE));
    memcpy(&DrvStatusEx.stCropWin, &gSrcInfo[eWindow].stCropWin, sizeof(MS_WINDOW_TYPE));
    /* real crop win in memory */
    memcpy(&DrvStatusEx.ScaledCropWin, &gSrcInfo[eWindow].ScaledCropWin, sizeof(MS_WINDOW_TYPE));
    //Assign display window for AP layer
    memcpy(&DrvStatusEx.stDispWin, &gSrcInfo[eWindow].stDispWin, sizeof(MS_WINDOW_TYPE));
    if(DrvStatusEx.stDispWin.x >= gSrcInfo[MAIN_WINDOW].Status2.stXCPanelDefaultTiming.u16HStart)
    {
        DrvStatusEx.stDispWin.x -= gSrcInfo[MAIN_WINDOW].Status2.stXCPanelDefaultTiming.u16HStart;
    }
    if(DrvStatusEx.stDispWin.y >= gSrcInfo[MAIN_WINDOW].Status2.stXCPanelDefaultTiming.u16VStart)
    {
        DrvStatusEx.stDispWin.y -= gSrcInfo[MAIN_WINDOW].Status2.stXCPanelDefaultTiming.u16VStart;
    }

    //----------------------
    // Timing
    //----------------------
    DrvStatusEx.bInterlace = gSrcInfo[eWindow].bInterlace;
    DrvStatusEx.bHDuplicate = gSrcInfo[eWindow].bHDuplicate;
    DrvStatusEx.u16InputVFreq = gSrcInfo[eWindow].u16InputVFreq;
    DrvStatusEx.u16InputVTotal = gSrcInfo[eWindow].u16InputVTotal;
    DrvStatusEx.u16DefaultHtotal = gSrcInfo[eWindow].u16DefaultHtotal;
    DrvStatusEx.u8DefaultPhase = gSrcInfo[eWindow].u8DefaultPhase;

    //----------------------
    // customized scaling
    //----------------------
    DrvStatusEx.bHCusScaling = gSrcInfo[eWindow].bHCusScaling;
    DrvStatusEx.u16HCusScalingSrc = gSrcInfo[eWindow].u16HCusScalingSrc;
    DrvStatusEx.u16HCusScalingDst = gSrcInfo[eWindow].u16HCusScalingDst;
    DrvStatusEx.bVCusScaling = gSrcInfo[eWindow].bVCusScaling;
    DrvStatusEx.u16VCusScalingSrc = gSrcInfo[eWindow].u16VCusScalingSrc;
    DrvStatusEx.u16VCusScalingDst = gSrcInfo[eWindow].u16VCusScalingDst;

    DrvStatusEx.bPreHCusScaling = gSrcInfo[eWindow].Status2.bPreHCusScaling;
    DrvStatusEx.u16PreHCusScalingSrc = gSrcInfo[eWindow].Status2.u16PreHCusScalingSrc;
    DrvStatusEx.u16PreHCusScalingDst = gSrcInfo[eWindow].Status2.u16PreHCusScalingDst;
    DrvStatusEx.bPreVCusScaling = gSrcInfo[eWindow].Status2.bPreVCusScaling;
    DrvStatusEx.u16PreVCusScalingSrc = gSrcInfo[eWindow].Status2.u16PreVCusScalingSrc;
    DrvStatusEx.u16PreVCusScalingDst = gSrcInfo[eWindow].Status2.u16PreVCusScalingDst;

    //--------------
    // 9 lattice
    //--------------
    DrvStatusEx.bDisplayNineLattice = gSrcInfo[eWindow].bDisplayNineLattice;

    //----------------------
    // XC internal setting
    //----------------------

    /* scaling ratio */
    DrvStatusEx.u16H_SizeAfterPreScaling = gSrcInfo[eWindow].u16H_SizeAfterPreScaling;
    DrvStatusEx.u16V_SizeAfterPreScaling = gSrcInfo[eWindow].u16V_SizeAfterPreScaling;
    DrvStatusEx.bPreV_ScalingDown = gSrcInfo[eWindow].bPreV_ScalingDown;


    /* others */
    DrvStatusEx.u32Op2DclkSet = gSrcInfo[eWindow].u32Op2DclkSet;

    /* Video screen status */
    DrvStatusEx.bBlackscreenEnabled = gSrcInfo[eWindow].bBlackscreenEnabled;
    DrvStatusEx.bBluescreenEnabled = gSrcInfo[eWindow].bBluescreenEnabled;
    DrvStatusEx.u16VideoDark = gSrcInfo[eWindow].u16VideoDark;

    DrvStatusEx.u16V_Length = gSrcInfo[eWindow].u16V_Length;
    DrvStatusEx.u16BytePerWord = gSrcInfo[eWindow].u16BytePerWord;
    DrvStatusEx.u16OffsetPixelAlignment = gSrcInfo[eWindow].u16OffsetPixelAlignment;
    DrvStatusEx.u8BitPerPixel = gSrcInfo[eWindow].u8BitPerPixel;

    DrvStatusEx.eDeInterlaceMode = gSrcInfo[eWindow].eDeInterlaceMode;
    DrvStatusEx.u8DelayLines = gSrcInfo[eWindow].u8DelayLines;
    DrvStatusEx.bMemFmt422 = gSrcInfo[eWindow].bMemFmt422;
    DrvStatusEx.eMemory_FMT = gSrcInfo[eWindow].eMemory_FMT;

    DrvStatusEx.bForceNRoff = gSrcInfo[eWindow].bForceNRoff;
    DrvStatusEx.bEn3DNR = gSrcInfo[eWindow].bEn3DNR;
    DrvStatusEx.bUseYUVSpace = gSrcInfo[eWindow].bUseYUVSpace;
    DrvStatusEx.bMemYUVFmt = gSrcInfo[eWindow].bMemYUVFmt;
    DrvStatusEx.bForceRGBin = gSrcInfo[eWindow].bForceRGBin;
    DrvStatusEx.bLinearMode = gSrcInfo[eWindow].bLinearMode;

    // frame lock related
    // only main can select FB/FBL because panel output timing depends on main window, sub will always use FB
    DrvStatusEx.bFBL = gSrcInfo[eWindow].bFBL;
    DrvStatusEx.bFastFrameLock = gSrcInfo[eWindow].bFastFrameLock;
    DrvStatusEx.bDoneFPLL = MDrv_Scaler_GetFPLLDoneStatus();
    DrvStatusEx.bEnableFPLL = gSrcInfo[eWindow].bEnableFPLL;
    DrvStatusEx.bFPLL_LOCK = gSrcInfo[eWindow].bFPLL_LOCK;

    // Capture_Memory
    DrvStatusEx.u32IPMBase0 = gSrcInfo[eWindow].u32IPMBase0;
    DrvStatusEx.u32IPMBase1 = gSrcInfo[eWindow].u32IPMBase1;
    DrvStatusEx.u32IPMBase2 = gSrcInfo[eWindow].u32IPMBase2;
    DrvStatusEx.u16IPMOffset = gSrcInfo[eWindow].u16IPMOffset;
    DrvStatusEx.u16IPMFetch = gSrcInfo[eWindow].u16IPMFetch;

    //HSizeChangedBeyondPQ status
    DrvStatusEx.bPQSetHSD = !(gSrcInfo[eWindow].bHCusScaling || gSrcInfo[eWindow].Status2.bPreHCusScaling
                                || MDrv_XC_GetHSizeChangeManuallyFlag(eWindow));
    _XC_RETURN();

    //shorter Ap structure with longer lib structure, we should handle it to avoid access-violation.
    memcpy(pDrvStatusEx, &DrvStatusEx, u16CopiedLength);


    return u16CopiedLength;
}

//-------------------------------------------------------------------------------------------------
/// Get XC Status of specific window(Main/Sub)
/// @param  pDrvStatus                  \b OUT: store the status
/// @param  eWindow                     \b IN: which window(Main/Sub) is going to get status
/// @return @ref MS_BOOL
//-------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_GetStatus(XC_ApiStatus *pDrvStatus, SCALER_WIN eWindow)
{
    _XC_ENTRY();
    //----------------------
    // Customer setting
    //----------------------
    pDrvStatus->enInputSourceType = gSrcInfo[eWindow].enInputSourceType;

    //----------------------
    // Window
    //----------------------
    memcpy(&pDrvStatus->stCapWin, &gSrcInfo[eWindow].stCapWin, sizeof(MS_WINDOW_TYPE));
    memcpy(&pDrvStatus->stCropWin, &gSrcInfo[eWindow].stCropWin, sizeof(MS_WINDOW_TYPE));
    /* real crop win in memory */
    memcpy(&pDrvStatus->ScaledCropWin, &gSrcInfo[eWindow].ScaledCropWin, sizeof(MS_WINDOW_TYPE));
    //Assign display window for AP layer
    memcpy(&pDrvStatus->stDispWin, &gSrcInfo[eWindow].stDispWin, sizeof(MS_WINDOW_TYPE));
    if(pDrvStatus->stDispWin.x >= gSrcInfo[MAIN_WINDOW].Status2.stXCPanelDefaultTiming.u16HStart)
    {
        pDrvStatus->stDispWin.x -= gSrcInfo[MAIN_WINDOW].Status2.stXCPanelDefaultTiming.u16HStart;
    }
    if(pDrvStatus->stDispWin.y >= gSrcInfo[MAIN_WINDOW].Status2.stXCPanelDefaultTiming.u16VStart)
    {
        pDrvStatus->stDispWin.y -= gSrcInfo[MAIN_WINDOW].Status2.stXCPanelDefaultTiming.u16VStart;
    }

    //----------------------
    // Timing
    //----------------------
    pDrvStatus->bInterlace = gSrcInfo[eWindow].bInterlace;
    pDrvStatus->bHDuplicate = gSrcInfo[eWindow].bHDuplicate;
    pDrvStatus->u16InputVFreq = gSrcInfo[eWindow].u16InputVFreq;
    pDrvStatus->u16InputVTotal = gSrcInfo[eWindow].u16InputVTotal;
    pDrvStatus->u16DefaultHtotal = gSrcInfo[eWindow].u16DefaultHtotal;
    pDrvStatus->u8DefaultPhase = gSrcInfo[eWindow].u8DefaultPhase;

    //----------------------
    // customized scaling
    //----------------------
    pDrvStatus->bHCusScaling = gSrcInfo[eWindow].bHCusScaling;
    pDrvStatus->u16HCusScalingSrc = gSrcInfo[eWindow].u16HCusScalingSrc;
    pDrvStatus->u16HCusScalingDst = gSrcInfo[eWindow].u16HCusScalingDst;
    pDrvStatus->bVCusScaling = gSrcInfo[eWindow].bVCusScaling;
    pDrvStatus->u16VCusScalingSrc = gSrcInfo[eWindow].u16VCusScalingSrc;
    pDrvStatus->u16VCusScalingDst = gSrcInfo[eWindow].u16VCusScalingDst;

    //--------------
    // 9 lattice
    //--------------
    pDrvStatus->bDisplayNineLattice = gSrcInfo[eWindow].bDisplayNineLattice;

    //----------------------
    // XC internal setting
    //----------------------

    /* scaling ratio */
    pDrvStatus->u16H_SizeAfterPreScaling = gSrcInfo[eWindow].u16H_SizeAfterPreScaling;
    pDrvStatus->u16V_SizeAfterPreScaling = gSrcInfo[eWindow].u16V_SizeAfterPreScaling;
    pDrvStatus->bPreV_ScalingDown = gSrcInfo[eWindow].bPreV_ScalingDown;


    /* others */
    pDrvStatus->u32Op2DclkSet = gSrcInfo[eWindow].u32Op2DclkSet;

    /* Video screen status */
    pDrvStatus->bBlackscreenEnabled = gSrcInfo[eWindow].bBlackscreenEnabled;
    pDrvStatus->bBluescreenEnabled = gSrcInfo[eWindow].bBluescreenEnabled;
    pDrvStatus->u16VideoDark = gSrcInfo[eWindow].u16VideoDark;

    pDrvStatus->u16V_Length = gSrcInfo[eWindow].u16V_Length;
    pDrvStatus->u16BytePerWord = gSrcInfo[eWindow].u16BytePerWord;
    pDrvStatus->u16OffsetPixelAlignment = gSrcInfo[eWindow].u16OffsetPixelAlignment;
    pDrvStatus->u8BitPerPixel = gSrcInfo[eWindow].u8BitPerPixel;

    pDrvStatus->eDeInterlaceMode = gSrcInfo[eWindow].eDeInterlaceMode;
    pDrvStatus->u8DelayLines = gSrcInfo[eWindow].u8DelayLines;
    pDrvStatus->bMemFmt422 = gSrcInfo[eWindow].bMemFmt422;
    pDrvStatus->eMemory_FMT = gSrcInfo[eWindow].eMemory_FMT;

    pDrvStatus->bForceNRoff = gSrcInfo[eWindow].bForceNRoff;
    pDrvStatus->bEn3DNR = gSrcInfo[eWindow].bEn3DNR;
    pDrvStatus->bUseYUVSpace = gSrcInfo[eWindow].bUseYUVSpace;
    pDrvStatus->bMemYUVFmt = gSrcInfo[eWindow].bMemYUVFmt;
    pDrvStatus->bForceRGBin = gSrcInfo[eWindow].bForceRGBin;
    pDrvStatus->bLinearMode = gSrcInfo[eWindow].bLinearMode;

    // frame lock related
    // only main can select FB/FBL because panel output timing depends on main window, sub will always use FB
    pDrvStatus->bFBL = gSrcInfo[eWindow].bFBL;
    pDrvStatus->bFastFrameLock = gSrcInfo[eWindow].bFastFrameLock;
    pDrvStatus->bDoneFPLL = MDrv_Scaler_GetFPLLDoneStatus();
    pDrvStatus->bEnableFPLL = gSrcInfo[eWindow].bEnableFPLL;
    pDrvStatus->bFPLL_LOCK = gSrcInfo[eWindow].bFPLL_LOCK;

    // Capture_Memory
    pDrvStatus->u32IPMBase0 = gSrcInfo[eWindow].u32IPMBase0;
    pDrvStatus->u32IPMBase1 = gSrcInfo[eWindow].u32IPMBase1;
    pDrvStatus->u32IPMBase2 = gSrcInfo[eWindow].u32IPMBase2;
    pDrvStatus->u16IPMOffset = gSrcInfo[eWindow].u16IPMOffset;
    pDrvStatus->u16IPMFetch = gSrcInfo[eWindow].u16IPMFetch;
    _XC_RETURN();

    return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// Set XC debug level
/// @param  u16DbgSwitch                \b IN: turn on/off debug switch
//-------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_SetDbgLevel(MS_U16 u16DbgSwitch)
{
    _u16XCDbgSwitch_InternalUseOnly = u16DbgSwitch;
    return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// Get scaler output vertical frequency
/// @return @ref MS_U16 return output vertical frequency x 100
//-------------------------------------------------------------------------------------------------
MS_U16 MApi_XC_GetOutputVFreqX100(void)
{
    MS_U16 u16OutputVfreqX100;
    u16OutputVfreqX100 = MDrv_SC_GetOutputVFreqX100();
    return u16OutputVfreqX100;
}

//-------------------------------------------------------------------------------------------------
/// set NR on/off
/// @param bEn              \b IN: enable/disable NR
/// @param eWindow          \b IN: which window(Main/Sub) is going to get status
//-------------------------------------------------------------------------------------------------
void MApi_XC_Set_NR(MS_BOOL bEn, SCALER_WIN eWindow)
{
#if TEST_NEW_DYNAMIC_NR
    _XC_ENTRY();
    MDrv_Scaler_EnableNR(bEn, eWindow);
    _XC_RETURN();
#endif
}

//-------------------------------------------------------------------------------------------------
/// Create a function for PQ in SC.
/// @param  PQ_Function_Info               \b IN: the information of the function
/// @param  u32InitDataLen          \b IN: the length of the PQ_Function_Info
//-------------------------------------------------------------------------------------------------
void MApi_XC_PQ_LoadFunction(PQ_Function_Info* function_Info , MS_U32 u32InitDataLen)
{
    if(function_Info == NULL)
    {
        MS_ASSERT(0);
        s_PQ_Function_Info.pq_deside_srctype = NULL;
        s_PQ_Function_Info.pq_disable_filmmode = NULL;
        s_PQ_Function_Info.pq_load_scalingtable = NULL;
        s_PQ_Function_Info.pq_set_420upsampling = NULL;
        s_PQ_Function_Info.pq_set_csc = NULL;
        s_PQ_Function_Info.pq_get_memyuvfmt= NULL;
        s_PQ_Function_Info.pq_set_memformat = NULL;
        s_PQ_Function_Info.pq_set_modeInfo = NULL;
        s_PQ_Function_Info.pq_ioctl = NULL;
    }
    else
    {
        // Path create.
        s_PQ_Function_Info.pq_deside_srctype = function_Info->pq_deside_srctype;
        s_PQ_Function_Info.pq_disable_filmmode = function_Info->pq_disable_filmmode;
        s_PQ_Function_Info.pq_load_scalingtable = function_Info->pq_load_scalingtable;
        s_PQ_Function_Info.pq_set_420upsampling = function_Info->pq_set_420upsampling;
        s_PQ_Function_Info.pq_set_csc = function_Info->pq_set_csc;
        s_PQ_Function_Info.pq_get_memyuvfmt= function_Info->pq_get_memyuvfmt;
        s_PQ_Function_Info.pq_set_memformat = function_Info->pq_set_memformat;
        s_PQ_Function_Info.pq_set_modeInfo = function_Info->pq_set_modeInfo;
        s_PQ_Function_Info.pq_ioctl = function_Info->pq_ioctl;

    }
}

void MApi_XC_SetOSD2VEMode(EN_VOP_SEL_OSD_XC2VE_MUX  eVOPSelOSD_MUX)
{
    MDrv_XC_SetOSD2VEmode(eVOPSelOSD_MUX);
}


void MApi_XC_IP2_PreFilter_Enable(MS_BOOL bEnable)
{
    _XC_ENTRY();
    MDrv_SC_IP2_PreFilter_Enable(bEnable);
    _XC_RETURN();
}

void MApi_XC_KeepPixelPointerAppear(MS_BOOL bEnable)
{
    _XC_ENTRY();
    Hal_SC_enable_cursor_report(bEnable);
    s_bKeepPixelPointerAppear = bEnable;
    _XC_RETURN();
}

E_APIXC_GET_PixelRGB_ReturnValue MApi_XC_Get_Pixel_RGB(XC_Get_Pixel_RGB *pData, SCALER_WIN eWindow)
{
    E_APIXC_GET_PixelRGB_ReturnValue enRet;

    MS_WINDOW_TYPE DispWin;
    MS_WINDOW_TYPE DEWin;

    _XC_ENTRY();

	#ifdef MULTI_SCALER_SUPPORTED
	Hal_SC_get_disp_de_window(&DEWin, eWindow);
	#else
    Hal_SC_get_disp_de_window(&DEWin);
	#endif

    if(pData->enStage == E_XC_GET_PIXEL_STAGE_AFTER_DLC)
        MDrv_XC_get_dispwin_from_reg(eWindow,&DispWin );
    else
        MDrv_XC_get_dispwin_from_reg(MAIN_WINDOW, &DispWin);

    pData->u16x += DEWin.x;
    pData->u16y += DEWin.y;

    if(pData->u16x > DEWin.x + DEWin.width ||
       pData->u16y > DEWin.y + DEWin.height)
    {
        enRet = E_GET_PIXEL_RET_OUT_OF_RANGE;
    }
    else
    {
        if(Hal_SC_get_pixel_rgb(pData))
            enRet = E_GET_PIXEL_RET_OK;
        else
            enRet = E_GET_PIXEL_RET_FAIL;
    }

    _XC_RETURN();
    return enRet;
}

MS_BOOL MApi_XC_Set_MemFmt(MS_XC_MEM_FMT eMemFmt)
{
    MS_BOOL bret;

    if(eMemFmt <= E_MS_XC_MEM_FMT_422)
    {
        s_eMemFmt = eMemFmt;
        bret = TRUE;
    }
    else
    {
        bret = FALSE;
    }

    return bret;
}

MS_BOOL MApi_XC_IsRequestFrameBufferLessMode(void)
{
    return Hal_SC_Detect_RequestFBL_Mode();
}

//-------------------------------------------------------------------------------------------------
/// Set Flag to bypass software reset in MApi_XC_Init()
/// @param  bFlag               \b IN: TRUE: initialize XC by skipping SW reset; FALSE: initialize XC in normal case
/// @return E_APIXC_RET_OK or E_APIXC_RET_FAIL
//-------------------------------------------------------------------------------------------------
E_APIXC_ReturnValue MApi_XC_SkipSWReset(MS_BOOL bFlag)
{
    _bSkipSWReset = bFlag;

    return E_APIXC_RET_OK;
}

//-------------------------------------------------------------------------------------------------
/// Select which DE to vop
/// @param  bFlag               \b IN: @ref E_OP2VOP_DE_SEL
/// @return E_APIXC_RET_OK or E_APIXC_RET_FAIL
//-------------------------------------------------------------------------------------------------
E_APIXC_ReturnValue MApi_XC_OP2VOPDESel(E_OP2VOP_DE_SEL eVopDESel)
{
    E_APIXC_ReturnValue eRet;
    _XC_ENTRY();
    eRet = Hal_SC_OP2VOPDESel(eVopDESel);
    _XC_RETURN();
    return eRet;
}

//-------------------------------------------------------------------------------------------------
/// Set Video and OSD Layer at Scaler VOP
/// @return E_APIXC_RET_OK or E_APIXC_RET_FAIL
//-------------------------------------------------------------------------------------------------
E_APIXC_ReturnValue MApi_XC_SetOSDLayer(E_VOP_OSD_LAYER_SEL  eVOPOSDLayer, SCALER_WIN eWindow)
{
    return MDrv_XC_SetOSDLayer(eVOPOSDLayer, eWindow);
}

//-------------------------------------------------------------------------------------------------
/// Get Video and OSD Layer Enum at Scaler VOP
/// @return Enum value
//-------------------------------------------------------------------------------------------------
E_VOP_OSD_LAYER_SEL MApi_XC_GetOSDLayer(SCALER_WIN eWindow)
{
    return MDrv_XC_GetOSDLayer(eWindow);
}

//-------------------------------------------------------------------------------------------------
/// Set Constant Alpha Value of Video
/// @return E_APIXC_RET_OK or E_APIXC_RET_FAIL
//-------------------------------------------------------------------------------------------------
E_APIXC_ReturnValue MApi_XC_SetVideoAlpha(MS_U8 u8Val, SCALER_WIN eWindow)
{
    return MDrv_XC_SetVideoAlpha(u8Val, eWindow);
}

//-------------------------------------------------------------------------------------------------
/// Get Constant Alpha Value of Video
/// @return E_APIXC_RET_OK or E_APIXC_RET_FAIL
//-------------------------------------------------------------------------------------------------
E_APIXC_ReturnValue MApi_XC_GetVideoAlpha(MS_U8 *pu8Val, SCALER_WIN eWindow)
{
    return MDrv_XC_GetVideoAlpha(pu8Val, eWindow);
}

//-------------------------------------------------------------------------------------------------
/// Get field packing mode support status
/// @return TRUE(success) or FALSE(fail)
//-------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_IsFieldPackingModeSupported(void)
{
    return _FIELD_PACKING_MODE_SUPPORTED;
}



//-------------------------------------------------------------------------------------------------
// Skip Wait Vsync
/// @param eWindow               \b IN: Enable
/// @param Skip wait Vsync      \b IN: Disable wait Vsync
//-------------------------------------------------------------------------------------------------
E_APIXC_ReturnValue MApi_XC_SkipWaitVsync( MS_BOOL eWindow,MS_BOOL bIsSkipWaitVsyn)
{
    MDrv_SC_SetSkipWaitVsync( eWindow, bIsSkipWaitVsyn);
    return E_APIXC_RET_OK;
}



E_APIXC_ReturnValue MApi_XC_Set_TGen(XC_TGEN_INFO_t stTGenInfo, SCALER_WIN eWindow)
{
    MDrv_XC_Set_TGen(eWindow, stTGenInfo);
    return E_APIXC_RET_OK;
}

E_APIXC_ReturnValue MApi_XC_Get_TGen_H_Backporch(SCALER_WIN eWindow, MS_U16 *pu16Hbackporch)
{
    if(MDrv_XC_Get_TGen_SCTbl_H_Bacporch(eWindow, pu16Hbackporch) == TRUE)
    {
        return E_APIXC_RET_OK;
    }
    else
    {
        return E_APIXC_RET_FAIL;
    }
}


E_APIXC_ReturnValue MApi_XC_Get_TGen_HV_Start(SCALER_WIN eWindow, MS_U16 *pu16HStart, MS_U16 *pu16VStart)
{
    if(MDrv_XC_Get_TGen_SCTbl_HV_Start(eWindow, pu16HStart, pu16VStart))
    {
        return E_APIXC_RET_OK;
    }
    else
    {
        return E_APIXC_RET_FAIL;
    }
}


E_APIXC_ReturnValue MApi_XC_Set_SC2_ODCLK(MS_U16 u16Htt, MS_U16 u16Vtt, MS_U16 u16FrameRate)
{
#ifdef MULTI_SCALER_SUPPORTED

    MS_U8 u8SC2_Synth = CKG_SC2_ODCLK_SYN_216MHZ;
    MS_U32 u32SC2_SynthCtl;
    MS_U64 u64SynCtl;

    u64SynCtl = ((MS_U64)(216000000) * (MS_U64)(1048576));
    do_div(u64SynCtl, ((MS_U64)u16Htt * (MS_U64)u16Vtt * (MS_U64)u16FrameRate));
    u32SC2_SynthCtl = (MS_U32) u64SynCtl;

    Hal_SC2_set_ODCLK(u32SC2_SynthCtl, u8SC2_Synth);
    return E_APIXC_RET_OK;
#else
    return E_APIXC_RET_FAIL;
#endif

}
