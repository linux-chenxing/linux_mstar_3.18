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
//==============================================================================
// [mhal_sc.c]
// Date: 20081203
// Descriptions: Add a new layer for HW setting
//==============================================================================
#define  MDRV_PIP_C
#ifdef MSOS_TYPE_LINUX_KERNEL
#include <linux/wait.h>
#endif

#include "mhal_xc_chip_config.h"
#include "xc_hwreg_utility2.h"
#include "drvXC_IOPort.h"
#include "apiXC.h"
#include "mhal_pip.h"
#include "mvideo_context.h"
#include "drv_sc_ip.h"
#include "mhal_sc.h"
#include "drv_sc_menuload.h"
#include "mdrv_sc_3d.h"
#include "drv_sc_display.h"


//-------------------------------------------------------------------------------------------------
/// Control the output window(Main/Sub) on or off
/// @param  bEnable                \b IN: bEnable =1, Turn on the window; bEnable =0, Turn off the window
/// @param  eWindow              \b IN: eWindow =0, for MAIN; eWindow =1, for SUB;
//-------------------------------------------------------------------------------------------------
void MApi_XC_EnableWindow(MS_BOOL bEnable, SCALER_WIN eWindow)
{
    _XC_ENTRY();

    if (MApi_XC_MLoad_GetStatus(eWindow) == E_MLOAD_ENABLED)
    {
        Hal_SC_enable_window_burst(bEnable, eWindow);
    }
    else
    {
        MDrv_XC_wait_output_vsync(2, 200, eWindow);
        Hal_SC_enable_window(bEnable, eWindow);
    }

    // When Sub Window is disabled, the CLK for sub also need to turn off
    if((eWindow == SUB_WINDOW)&&(bEnable == 0))
    {
    #ifdef MULTI_SCALER_SUPPORTED
        MDrv_XC_EnableCLK_for_SUB(FALSE, eWindow);
    #else
        MDrv_XC_EnableCLK_for_SUB(FALSE);
    #endif
    }
    _XC_RETURN();
}

//-------------------------------------------------------------------------------------------------
/// Control the main window on or off
/// @param  bEnable                \b IN: bEnable =1, Turn on the window; bEnable =0, Turn off the window
//-------------------------------------------------------------------------------------------------
void MApi_XC_EnableMainWindow(MS_BOOL bEnable)
{
    _XC_ENTRY();
    if (MApi_XC_MLoad_GetStatus(MAIN_WINDOW) == E_MLOAD_ENABLED)
    {
        Hal_SC_enable_window_burst(bEnable,MAIN_WINDOW);
    }
    else
    {
        Hal_SC_enable_window(bEnable,MAIN_WINDOW);
    }
    _XC_RETURN();

}

//-------------------------------------------------------------------------------------------------
/// Enable sub window and control the window size
/// @param  *pstDispWin                \b IN: Window type
//-------------------------------------------------------------------------------------------------
#ifdef MULTI_SCALER_SUPPORTED
	void MApi_XC_EnableSubWindow(MS_WINDOW_TYPE *pstDispWin, SCALER_WIN eWindow)
	{
	    _XC_ENTRY();

	    if( g_XC_InitData.bMirror[eWindow] )
	    {
	        if(!MDrv_SC_3D_Is3DProcessByPIP(SUB_WINDOW))
	        {
	            if( g_XC_InitData.stPanelInfo.u16Width > ( pstDispWin->x + pstDispWin->width ) )
	            {
	                pstDispWin->x = g_XC_InitData.stPanelInfo.u16Width - ( pstDispWin->x + pstDispWin->width );
	            }
	            else
	            {
	                pstDispWin->x = 0;
	            }

	            if( g_XC_InitData.stPanelInfo.u16Height > ( pstDispWin->y + pstDispWin->height ) )
	            {
	                pstDispWin->y = g_XC_InitData.stPanelInfo.u16Height - ( pstDispWin->y + pstDispWin->height );
	            }
	            else
	            {
	                pstDispWin->y = 0;
	            }
	        }
	    }

	    pstDispWin->x += g_XC_InitData.stPanelInfo.u16HStart;
	    pstDispWin->y += g_XC_InitData.stPanelInfo.u16VStart;
	    //pstDispWin->width += g_XC_InitData.stPanelInfo.u16HStart;
	    //pstDispWin->height += g_XC_InitData.stPanelInfo.u16VStart;
	    //printf("Disp x=%d,y=%d,width=%d,height=%d\n",pstDispWin->x,pstDispWin->y,pstDispWin->width,pstDispWin->height);
	    //printf("Panel HStart =%d, VStart =%d\n", g_XC_InitData.stPanelInfo.u16HStart, g_XC_InitData.stPanelInfo.u16VStart);

	    if (MApi_XC_MLoad_GetStatus(eWindow) == E_MLOAD_ENABLED)
	    {
	        Hal_SC_Sub_SetDisplayWindow_burst(pstDispWin, eWindow);

	        //this part was originally inside Hal_SC_Sub_SetDisplayWindow_burst(), it is moved out because
	        //if we let REG_SC_BK20_10_L[1] to enable with other register settings in the same menuload fire
	        //it will produce a rectangular horizontal noise. so we move outside to let those register settings
	        //to be set first then let REG_SC_BK20_10_L[1] on.

	        //MApi_XC_MLoad_WriteCmd(REG_SC_BK20_10_L, (TRUE<<1), BIT(1));
	        //MApi_XC_MLoad_Fire(TRUE);
	    }
	    else
	    {
	        MDrv_XC_wait_output_vsync(2, 200, MAIN_WINDOW);
	        Hal_SC_Sub_SetDisplayWindow(pstDispWin, eWindow);
	    }

	    _XC_RETURN();

	    MApi_XC_EnableWindow(ENABLE,SUB_WINDOW);
	}

#else	// #ifdef MULTI_SCALER_SUPPORTED

	void MApi_XC_EnableSubWindow(MS_WINDOW_TYPE *pstDispWin)
	{
	    _XC_ENTRY();

	    if( g_XC_InitData.bMirror )
	    {
	        if(!MDrv_SC_3D_Is3DProcessByPIP(SUB_WINDOW))
	        {
	            if( g_XC_InitData.stPanelInfo.u16Width > ( pstDispWin->x + pstDispWin->width ) )
	            {
	                pstDispWin->x = g_XC_InitData.stPanelInfo.u16Width - ( pstDispWin->x + pstDispWin->width );
	            }
	            else
	            {
	                pstDispWin->x = 0;
	            }

	            if( g_XC_InitData.stPanelInfo.u16Height > ( pstDispWin->y + pstDispWin->height ) )
	            {
	                pstDispWin->y = g_XC_InitData.stPanelInfo.u16Height - ( pstDispWin->y + pstDispWin->height );
	            }
	            else
	            {
	                pstDispWin->y = 0;
	            }
	        }
	    }

	    pstDispWin->x += g_XC_InitData.stPanelInfo.u16HStart;
	    pstDispWin->y += g_XC_InitData.stPanelInfo.u16VStart;
	    //pstDispWin->width += g_XC_InitData.stPanelInfo.u16HStart;
	    //pstDispWin->height += g_XC_InitData.stPanelInfo.u16VStart;
	    //printf("Disp x=%d,y=%d,width=%d,height=%d\n",pstDispWin->x,pstDispWin->y,pstDispWin->width,pstDispWin->height);
	    //printf("Panel HStart =%d, VStart =%d\n", g_XC_InitData.stPanelInfo.u16HStart, g_XC_InitData.stPanelInfo.u16VStart);

	    if (MApi_XC_MLoad_GetStatus(MAIN_WINDOW) == E_MLOAD_ENABLED)
	    {
	        Hal_SC_Sub_SetDisplayWindow_burst(pstDispWin);

	        //this part was originally inside Hal_SC_Sub_SetDisplayWindow_burst(), it is moved out because
	        //if we let REG_SC_BK20_10_L[1] to enable with other register settings in the same menuload fire
	        //it will produce a rectangular horizontal noise. so we move outside to let those register settings
	        //to be set first then let REG_SC_BK20_10_L[1] on.

	        //MApi_XC_MLoad_WriteCmd(REG_SC_BK20_10_L, (TRUE<<1), BIT(1));
	        //MApi_XC_MLoad_Fire(TRUE);
	    }
	    else
	    {
	        MDrv_XC_wait_output_vsync(2, 200, MAIN_WINDOW);
	        Hal_SC_Sub_SetDisplayWindow(pstDispWin);
	    }

	    _XC_RETURN();

	    MApi_XC_EnableWindow(ENABLE,SUB_WINDOW);
	}
#endif

//-------------------------------------------------------------------------------------------------
/// Disable sub window
/// @param  void                \b IN: none
//-------------------------------------------------------------------------------------------------
#ifdef MULTI_SCALER_SUPPORTED

	void MApi_XC_DisableSubWindow(SCALER_WIN eWindow)
	{
	    _XC_ENTRY();

	    if (MApi_XC_MLoad_GetStatus(eWindow) == E_MLOAD_ENABLED)
	    {
	        Hal_SC_subwindow_disable_burst(eWindow);
	    }
	    else
	    {
	        MDrv_XC_wait_output_vsync(2, 200, MAIN_WINDOW);
	        Hal_SC_subwindow_disable(eWindow);
	    }
	    MDrv_XC_EnableCLK_for_SUB(FALSE, eWindow);
	    _XC_RETURN();
	}

	// Non-mutex version.
	void MDrv_XC_DisableSubWindow(SCALER_WIN eWindow)
	{
	    Hal_SC_subwindow_disable(eWindow);
	}

#else	//#ifdef MULTI_SCALER_SUPPORTED
	void MApi_XC_DisableSubWindow(void)
	{
	    _XC_ENTRY();

	    if (MApi_XC_MLoad_GetStatus(SUB_WINDOW) == E_MLOAD_ENABLED)
	    {
	        Hal_SC_subwindow_disable_burst();
	    }
	    else
	    {
	        MDrv_XC_wait_output_vsync(2, 200, MAIN_WINDOW);
	        Hal_SC_subwindow_disable();
	    }
	    MDrv_XC_EnableCLK_for_SUB(FALSE);
	    _XC_RETURN();
	}

	// Non-mutex version.
	void MDrv_XC_DisableSubWindow(void)
	{
	    Hal_SC_subwindow_disable();
	}
#endif
//-------------------------------------------------------------------------------------------------
/// Is sub window enable?
/// @return @ref MS_BOOL
//-------------------------------------------------------------------------------------------------
#ifdef MULTI_SCALER_SUPPORTED
MS_BOOL MApi_XC_Is_SubWindowEanble(SCALER_WIN eWindow)
{
    MS_BOOL bRet = FALSE;

    _XC_ENTRY();
     bRet = Hal_SC_Is_subwindow_enable(eWindow);
    _XC_RETURN();
    return bRet;
}

#else	//#ifdef MULTI_SCALER_SUPPORTED
MS_BOOL MApi_XC_Is_SubWindowEanble(void)
{
    MS_BOOL bRet = FALSE;

    _XC_ENTRY();
     bRet = Hal_SC_Is_subwindow_enable();
    _XC_RETURN();
    return bRet;
}
#endif
//-------------------------------------------------------------------------------------------------
/// Set Border format
/// @param  u8WidthIn                \b IN: In-box width
/// @param  u8WidthOut               \b IN: Out-box width
/// @param  u8color                  \b IN: border color
/// @param  eWindow                  \b IN: which window we are going to set
//-------------------------------------------------------------------------------------------------
void MApi_XC_SetBorderFormat( MS_U8 u8Left, MS_U8 u8Right, MS_U8 u8Up, MS_U8 u8Down, MS_U8 u8color, SCALER_WIN eWindow )
{
    _XC_ENTRY();
    Hal_SC_set_border_format( u8Left, u8Right, u8Up, u8Down, u8color, eWindow );
    _XC_RETURN();
}

//-------------------------------------------------------------------------------------------------
/// enable or disable Border
/// @param  bEnable                  \b IN: Enable or Disable
/// @param  eWindow                  \b IN: which window we are going to set
//-------------------------------------------------------------------------------------------------
void MApi_XC_EnableBorder( MS_BOOL bEnable, SCALER_WIN eWindow )
{
    _XC_ENTRY();
    Hal_SC_border_enable( bEnable, eWindow);
    _XC_RETURN();
}

//-------------------------------------------------------------------------------------------------
/// Set Z-order main window first or not
/// @param  bMainFirst               \b IN: TRUE: Main window first; FALSE: Sub window first
//-------------------------------------------------------------------------------------------------
#ifdef MULTI_SCALER_SUPPORTED

void MApi_XC_ZorderMainWindowFirst(MS_BOOL bMainFirst, SCALER_WIN eWindow)
{
    MS_XC_DST_DispInfo stBottomWin;

    _XC_ENTRY();
    // Get bottom window info.
    MApi_XC_GetDstInfo_IP1(&stBottomWin, sizeof(MS_XC_DST_DispInfo), bMainFirst ? SUB_WINDOW : MAIN_WINDOW);

    if (MApi_XC_MLoad_GetStatus(eWindow) == E_MLOAD_ENABLED)
    {
        Hal_SC_set_Zorder_main_first_burst(bMainFirst, eWindow);
        Hal_SC_set_Zextra_y_half_burst(stBottomWin.bInterlaceMode, eWindow);
        //printf("shawn (1) MApi_XC_ZorderMainWindowFirst(bMainTop(%d)) \r\n", bMainFirst);
    }
    else
    {
        Hal_SC_set_Zorder_main_first(bMainFirst, eWindow);
        Hal_SC_set_Zextra_y_half(stBottomWin.bInterlaceMode, eWindow);
        //printf("shawn (2) MApi_XC_ZorderMainWindowFirst(bMainTop(%d)) \r\n", bMainFirst);
    }
    _XC_RETURN();
}

#else
void MApi_XC_ZorderMainWindowFirst(MS_BOOL bMainFirst)
{
    MS_XC_DST_DispInfo stBottomWin;

    _XC_ENTRY();
    // Get bottom window info.
    MApi_XC_GetDstInfo_IP1(&stBottomWin, sizeof(MS_XC_DST_DispInfo), bMainFirst ? SUB_WINDOW : MAIN_WINDOW);

    if (MApi_XC_MLoad_GetStatus(MAIN_WINDOW) == E_MLOAD_ENABLED)
    {
        Hal_SC_set_Zorder_main_first_burst(bMainFirst);
        Hal_SC_set_Zextra_y_half_burst(stBottomWin.bInterlaceMode);
        //printf("shawn (1) MApi_XC_ZorderMainWindowFirst(bMainTop(%d)) \r\n", bMainFirst);
    }
    else
    {
        Hal_SC_set_Zorder_main_first(bMainFirst);
        Hal_SC_set_Zextra_y_half(stBottomWin.bInterlaceMode);
        //printf("shawn (2) MApi_XC_ZorderMainWindowFirst(bMainTop(%d)) \r\n", bMainFirst);
    }
    _XC_RETURN();
}
#endif
//-------------------------------------------------------------------------------------------------
/// enable or disable Subwindow FRC
/// @param  bEnable                  \b IN: Enable or Disable FRC.
/// @param  u8Threashold             \b IN: 0~7: ignore frames number before buffer being Read. larger
///                                              number means ingnore more. 0~7:1, 2, 4, 8, 16, 32, 64, 128
//-------------------------------------------------------------------------------------------------
void MApi_XC_SetPIP_FRC(MS_BOOL enable, MS_U8 u8IgonreLinesNum)
{
    _XC_ENTRY();

    Hal_SC_SetPIP_FRC(enable, u8IgonreLinesNum);

    _XC_RETURN();
}

#ifdef MULTI_SCALER_SUPPORTED
void MDrv_XC_EnableCLK_for_SUB(MS_BOOL bEnable, SCALER_WIN eWindow)
{
    // Enable sub window clock
    if(bEnable)
    {

        if(eWindow == SUB_WINDOW)
        {
            // Enable FIClk1
            MDrv_WriteRegBit(REG_CKG_FICLK_F1, DISABLE, CKG_FICLK_F1_INVERT);               // Not Invert
            MDrv_WriteRegBit(REG_CKG_FICLK_F1, DISABLE, CKG_FICLK_F1_GATED);                // Enable clock
            // Enable IDClk1
            MDrv_WriteRegBit(REG_CKG_IDCLK1, DISABLE, CKG_IDCLK1_INVERT);                   // Not Invert
            MDrv_WriteRegBit(REG_CKG_IDCLK1, DISABLE, CKG_IDCLK1_GATED);                    // Enable clock
        }
        else if(eWindow == SC2_SUB_WINDOW)
        {
            // Enable FIClk1
            MDrv_WriteRegBit(REG_CKG_SC2_FICLK_F1, DISABLE, CKG_SC2_FICLK_F1_INVERT);       // Not Invert
            MDrv_WriteRegBit(REG_CKG_SC2_FICLK_F1, DISABLE, CKG_SC2_FICLK_F1_GATED);        // Enable clock
            // Enable IDClk1
            MDrv_WriteRegBit(REG_CKG_SC2_IDCLK1, DISABLE, CKG_SC2_IDCLK1_INVERT);           // Not Invert
            MDrv_WriteRegBit(REG_CKG_SC2_IDCLK1, DISABLE, CKG_SC2_IDCLK1_GATED);            // Enable clock
        }

    }
    else
    {
        if(eWindow == SUB_WINDOW)
        {
            // Disable FIClk1
            MDrv_WriteRegBit(REG_CKG_FICLK_F1, ENABLE, CKG_FICLK_F1_INVERT);               // Not Invert
            MDrv_WriteRegBit(REG_CKG_FICLK_F1, ENABLE, CKG_FICLK_F1_GATED);                // Enable clock

            // Disable IDClk1
            MDrv_WriteRegBit(REG_CKG_IDCLK1, ENABLE, CKG_IDCLK1_INVERT);                   // Not Invert
            MDrv_WriteRegBit(REG_CKG_IDCLK1, ENABLE, CKG_IDCLK1_GATED);                    // Enable clock
        }
        else if(eWindow == SC2_SUB_WINDOW)
        {
            // Disable FIClk1
            MDrv_WriteRegBit(REG_CKG_SC2_FICLK_F1, ENABLE, CKG_SC2_FICLK_F1_INVERT);       // Not Invert
            MDrv_WriteRegBit(REG_CKG_SC2_FICLK_F1, ENABLE, CKG_SC2_FICLK_F1_GATED);        // Enable clock

            // Disable IDClk1
            MDrv_WriteRegBit(REG_CKG_SC2_IDCLK1, ENABLE, CKG_SC2_IDCLK1_INVERT);           // Not Invert
            MDrv_WriteRegBit(REG_CKG_SC2_IDCLK1, ENABLE, CKG_SC2_IDCLK1_GATED);            // Enable clock
        }

    }
}


#else
void MDrv_XC_EnableCLK_for_SUB(MS_BOOL bEnable)
{
    // Enable sub window clock
    if(bEnable)
    {
        // Enable FIClk1
        MDrv_WriteRegBit(REG_CKG_FICLK_F1, DISABLE, CKG_FICLK_F1_INVERT);               // Not Invert
        MDrv_WriteRegBit(REG_CKG_FICLK_F1, DISABLE, CKG_FICLK_F1_GATED);                // Enable clock

        // Enable IDClk1
        MDrv_WriteRegBit(REG_CKG_IDCLK1, DISABLE, CKG_IDCLK1_INVERT);                   // Not Invert
        MDrv_WriteRegBit(REG_CKG_IDCLK1, DISABLE, CKG_IDCLK1_GATED);                    // Enable clock
    }
    else
    {
        // Disable FIClk1
        MDrv_WriteRegBit(REG_CKG_FICLK_F1, ENABLE, CKG_FICLK_F1_INVERT);               // Not Invert
        MDrv_WriteRegBit(REG_CKG_FICLK_F1, ENABLE, CKG_FICLK_F1_GATED);                // Enable clock

        // Disable IDClk1
        MDrv_WriteRegBit(REG_CKG_IDCLK1, ENABLE, CKG_IDCLK1_INVERT);                   // Not Invert
        MDrv_WriteRegBit(REG_CKG_IDCLK1, ENABLE, CKG_IDCLK1_GATED);                    // Enable clock
    }
}
#endif

#undef  MDRV_PIP_C
