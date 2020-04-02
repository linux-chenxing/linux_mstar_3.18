////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2016-2017 MStar Semiconductor, Inc.
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
/// @file   mdrv_graphic.c
/// @brief  MStar graphic Interface
/// @author MStar Semiconductor Inc.
/// @attention
/// <b><em></em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

#define _MDRV_GRAPHIC_C

//=============================================================================
// Include Files
//=============================================================================
#include <linux/kernel.h>
#include <linux/jiffies.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#if defined(CONFIG_MIPS)
#elif defined(CONFIG_ARM)
#include <asm/io.h>
#endif

#include "mdrv_mstypes.h"
#include "mdrv_graphic.h"
#include "mhal_graphic.h"
#include "mhal_graphic_reg.h"

//=============================================================================
// Macros
//=============================================================================
#define ALIGN_CHECK(value,factor) ((value + factor-1) & (~(factor-1)))

//=============================================================================
// Global Variables
//=============================================================================
MS_BOOL bForceWriteIn = FALSE;

static MS_U16 _GOP_GetBPP(EN_DRV_GOPColorType fbFmt)
{
    MS_U16 u16bpp=0;

    switch (fbFmt)
    {
        case E_DRV_GOP_COLOR_RGB555_BLINK:
        case E_DRV_GOP_COLOR_RGB565:
        case E_DRV_GOP_COLOR_ARGB1555:
        case E_DRV_GOP_COLOR_RGBA5551:
        case E_DRV_GOP_COLOR_ARGB4444:
        case E_DRV_GOP_COLOR_RGBA4444:
        case E_DRV_GOP_COLOR_YUV422:
        case E_DRV_GOP_COLOR_RGB555YUV422:
        case E_DRV_GOP_COLOR_2266:
            u16bpp = 2;
            break;
        case E_DRV_GOP_COLOR_ARGB8888:
        case E_DRV_GOP_COLOR_ABGR8888:
            u16bpp = 4;
            break;
        case E_DRV_GOP_COLOR_I8:
            u16bpp = 1;
            break;
        default:
            u16bpp = 0xFFFF;
            break;
    }
    return u16bpp;
}

MS_BOOL _MDrv_GRAPHIC_Init(MS_U32 u32GopIdx)
{
    MS_U8 u8Idx;
    MS_U32 bankoffset=0;

    _HAL_GOP_GetBnkOfstByGop(u32GopIdx,&bankoffset);
    _HAL_GOP_Write16Reg(bankoffset+GOP_4G_CTRL0, 0x0, GOP_BIT0);                      // GOP rst
    _HAL_GOP_Write16Reg(bankoffset+GOP_4G_CTRL0, 0x0,(GOP_BIT1|GOP_BIT2));   // clear H/V sync reserve
    _HAL_GOP_Write16Reg(bankoffset+GOP_4G_CTRL1, 0x4100, GOP_REG_HW_MASK);            // Set REGDMA interval

    _HAL_GOP_Write16Reg(bankoffset+GOP_4G_HSTRCH, 0x1000, GOP_REG_WORD_MASK);         // Sanger 070713 For REGDMA Ready issue.
    _HAL_GOP_Write16Reg(bankoffset+GOP_4G_VSTRCH, 0x1000, GOP_REG_WORD_MASK);
    _HAL_GOP_Write16Reg(bankoffset+GOP_4G_CTRL0, GOP_BIT14, GOP_BIT14);               // Set mask Hsync when VFDE is low

    _HAL_GOP_Write16Reg(bankoffset+GOP_4G_RATE, 0x0098, GOP_REG_WORD_MASK);           // enable blink capability - for ttx usage
    _HAL_GOP_Write16Reg(bankoffset+GOP_4G_CTRL0, 0x0000, GOP_BIT10);                  // Enable RGB output

    _HAL_GOP_Write16Reg(bankoffset+GOP_4G_BW, GOP_FIFO_BURST_ALL, GOP_FIFO_BURST_MASK);  //set GOP DMA Burst length to "32"
    _HAL_GOP_Write16Reg(bankoffset+GOP_4G_BW, GOP_FIFO_THRESHOLD, GOP_REG_LW_MASK);  //set DMA FIFO threshold to 3/4 FIFO length

    _HAL_GOP_Write16Reg(bankoffset+GOP_4G_OLDADDR, 0x0, (GOP_BIT1 | GOP_BIT0)); //temporally disable GOP clk dynamical gated - to avoid update palette problem

    if (u32GopIdx==0)
    {
        for (u8Idx=0; u8Idx<MAX_GOP0_GWIN; u8Idx++)
        {
                _HAL_GOP_Write16Reg(GOP_4G_DRAM_VSTR_L(u8Idx),0, GOP_REG_WORD_MASK);
                _HAL_GOP_Write16Reg(GOP_4G_DRAM_VSTR_H(u8Idx),0, GOP_REG_WORD_MASK);
                _HAL_GOP_Write16Reg(GOP_4G_DRAM_HSTR(u8Idx),0, GOP_REG_WORD_MASK);
                _HAL_GOP_Write16Reg(GOP_4G_DRAM_HVSTOP_L(u8Idx),0, GOP_REG_WORD_MASK);
                _HAL_GOP_Write16Reg(GOP_4G_DRAM_HVSTOP_H(u8Idx),0, GOP_REG_WORD_MASK);
        }
    }
    return TRUE;
}

void _MDrv_GRAPHIC_SetHMirror(MS_U8 u8GOP,MS_BOOL bEnable)
{
    MS_U32 u32bankoffset=0;
    _HAL_GOP_GetBnkOfstByGop(u8GOP, &u32bankoffset);
    _HAL_GOP_Write16Reg(u32bankoffset+GOP_4G_CTRL0, bEnable?GOP_BIT12:0, GOP_BIT12);
}

void _MDrv_GRAPHIC_SetVMirror(MS_U8 u8GOP,MS_BOOL bEnable)
{
    MS_U32 u32bankoffset=0;
    _HAL_GOP_GetBnkOfstByGop(u8GOP, &u32bankoffset);
    _HAL_GOP_Write16Reg(u32bankoffset+GOP_4G_CTRL0, bEnable?GOP_BIT13:0, GOP_BIT13);
}

void _MDrv_GRAPHIC_OutputColor_EX(MS_U8 u8GOP,EN_DRV_GOP_OUTPUT_COLOR type)
{
    MS_U32 u32bankoffset=0;
    _HAL_GOP_GetBnkOfstByGop(u8GOP, &u32bankoffset);

    if(type == DRV_GOPOUT_RGB)
        _HAL_GOP_Write16Reg(u32bankoffset+GOP_4G_CTRL0, 0, GOP_BIT10);
    else
        _HAL_GOP_Write16Reg(u32bankoffset+GOP_4G_CTRL0, GOP_BIT10, GOP_BIT10);
}

void _MDrv_GRAPHIC_MIUSel(MS_U8 u8GOP, E_DRV_GOP_SEL_TYPE MiuSel)
{
    MS_U32 u32bankoffset=0;

    _HAL_GOP_GetBnkOfstByGop(u8GOP, &u32bankoffset);
    _HAL_GOP_Write16Reg(u32bankoffset+GOP_4G_MIU_SEL, MiuSel, GOP_BIT0|GOP_BIT1);
}

void _MDrv_GRAPHIC_SetGOPDst(MS_U8 u8GOP, EN_DRV_GOP_DST_TYPE eDsttype, MS_BOOL bOnlyCheck)
{
   _HAL_GOP_GWIN_SetDstPlane(u8GOP,eDsttype,bOnlyCheck);
}

void _MDrv_GRAPHIC_EnableTransClr_EX(MS_U8 u8GOP,EN_DRV_GOP_TRANSCLR_FMT eFmt, MS_BOOL bEnable)
{
    MS_U16 u16Regval = 0;
    MS_U32 u32BankOffSet = 0;

    _HAL_GOP_GetBnkOfstByGop(u8GOP, &u32BankOffSet);
    _HAL_GOP_Read16Reg(u32BankOffSet+GOP_4G_CTRL0, &u16Regval);

    if(bEnable)
    {
        if(eFmt == GOPTRANSCLR_FMT0)
            u16Regval |= GOP_RGB_TRANSPARENT_ENABLE;
        else
            u16Regval |= GOP_YUV_TRANSPARENT_ENABLE;
    }
    else
    {
        if(eFmt == GOPTRANSCLR_FMT0)
            u16Regval &= ~(GOP_RGB_TRANSPARENT_ENABLE);
        else
            u16Regval &= ~(GOP_YUV_TRANSPARENT_ENABLE);
    }

    _HAL_GOP_Write16Reg(u32BankOffSet+GOP_4G_CTRL0, u16Regval, (GOP_YUV_TRANSPARENT_ENABLE|GOP_RGB_TRANSPARENT_ENABLE));
}

void _MDrv_GRAPHIC_SetBlending(MS_U8 u8GOP,MS_U8 u8win, MS_BOOL bEnable, MS_U8 u8coef)
{
    _HAL_GOP_SetBlending(u8GOP, u8win, bEnable, u8coef);
}

void _MDrv_GOP_SetGwinInfo(MS_U8 u8GOP, MS_U8 u8win,DRV_GWIN_INFO WinInfo)
{
     MS_U16 u16Regval = 0, u16Bpp = 0;
     MS_U32 u32BankOffSet = 0, u32RingBuffer = 0;

     _HAL_GOP_GetBnkOfstByGop(u8GOP, &u32BankOffSet);

     u16Bpp = _GOP_GetBPP(WinInfo.clrType);
     if(u16Bpp == 0xFFFF)
     {
        printk("[%s] invalud color format\n",__FUNCTION__);
        return;
     }

     WinInfo.u32Addr = ALIGN_CHECK(WinInfo.u32Addr,GOP_WordUnit);
     WinInfo.u16Pitch = ALIGN_CHECK(WinInfo.u16Pitch,(GOP_WordUnit/u16Bpp));
     WinInfo.u16HStart = ALIGN_CHECK(WinInfo.u16HStart,(GOP_WordUnit/u16Bpp));
     WinInfo.u16HEnd = ALIGN_CHECK(WinInfo.u16HEnd,(GOP_WordUnit/u16Bpp));
     //Color Fmt
    _HAL_GOP_Write16Reg(u32BankOffSet+GOP_4G_GWIN0_CTRL(u8win), WinInfo.clrType <<4,0x00f0);
    //Address
    _HAL_GOP_Write32Reg(u32BankOffSet+GOP_4G_DRAM_RBLK_L(u8win), WinInfo.u32Addr);
    //Pitch
     _HAL_GOP_Write16Reg(u32BankOffSet+GOP_4G_DRAM_RBLK_HSIZE(u8win), WinInfo.u16Pitch , GOP_REG_WORD_MASK);
    //HStart
    _HAL_GOP_Write16Reg(u32BankOffSet+GOP_4G_HSTR(u8win), (WinInfo.u16HStart*u16Bpp) , GOP_REG_WORD_MASK);
    //HEnd
    _HAL_GOP_Write16Reg(u32BankOffSet+GOP_4G_HEND(u8win), WinInfo.u16HEnd , GOP_REG_WORD_MASK);
    //VStart
    _HAL_GOP_Write16Reg(u32BankOffSet+GOP_4G_VSTR(u8win), WinInfo.u16VStart , GOP_REG_WORD_MASK);
    //VEnd
    _HAL_GOP_Write16Reg(u32BankOffSet+GOP_4G_VEND(u8win), WinInfo.u16VEnd, GOP_REG_WORD_MASK);
    //Ring Buffer
    u32RingBuffer = WinInfo.u16Pitch*(WinInfo.u16VEnd - WinInfo.u16VStart)*u16Bpp;
    _HAL_GOP_Write32Reg(u32BankOffSet+GOP_4G_DRAM_RBLK_SIZE_L(u8win), u32RingBuffer);

}

void _MDrv_GRAPHIC_SetStretchWin(MS_U8 u8GOP,MS_U16 u16x, MS_U16 u16y, MS_U16 u16width, MS_U16 u16height)
{
    MS_U32 u32BankOffSet = 0;

    _HAL_GOP_GetBnkOfstByGop(u8GOP, &u32BankOffSet);
    _HAL_GOP_Write16Reg(u32BankOffSet + GOP_4G_STRCH_HSTR, u16x, GOP_REG_WORD_MASK);
    _HAL_GOP_Write16Reg(u32BankOffSet + GOP_4G_STRCH_VSTR, u16y , GOP_REG_WORD_MASK);
    _HAL_GOP_Write16Reg(u32BankOffSet + GOP_4G_STRCH_HSZ, (u16width/GOP_STRETCH_WIDTH_UNIT) , GOP_REG_WORD_MASK);
    _HAL_GOP_Write16Reg(u32BankOffSet + GOP_4G_STRCH_VSZ, u16height , GOP_REG_WORD_MASK);
    _HAL_GOP_Write16Reg(u32BankOffSet + GOP_4G_RDMA_HT, (u16width+3)/2, 0x07ff);
}

void _MDrv_GRAPHIC_SetHScale(MS_U8 u8GOP,MS_BOOL bEnable, MS_U16 u16src, MS_U16 u16dst)
{
    MS_U32 u32hratio =0x1000,u32BankOffSet=0;

    _HAL_GOP_GetBnkOfstByGop(u8GOP, &u32BankOffSet);
    if (bEnable)
    {
        u32hratio = (MS_U32)(u16src) * SCALING_MULITPLIER;
        if(u32hratio %(MS_U32)u16dst !=0)
            u32hratio = (u32hratio/(MS_U32)u16dst)+1;
        else
            u32hratio /= (MS_U32)u16dst;
    }
    else
        u32hratio = SCALING_MULITPLIER;

    _HAL_GOP_Write16Reg(u32BankOffSet+GOP_4G_HSTRCH, u32hratio , GOP_REG_WORD_MASK);
    _HAL_GOP_Write16Reg(u32BankOffSet+GOP_4G_HSTRCH_INI, 0 , GOP_REG_WORD_MASK);
}

void _MDrv_GRAPHIC_SetVScale(MS_U8 u8GOP,MS_BOOL bEnable, MS_U16 u16src, MS_U16 u16dst)
{
    MS_U32 u32vratio =0x1000,u32BankOffSet=0;
    _HAL_GOP_GetBnkOfstByGop(u8GOP, &u32BankOffSet);

    if (bEnable)
    {
        u32vratio = (MS_U32)(u16src) * SCALING_MULITPLIER;
        if(u32vratio %(MS_U32)u16dst !=0)
            u32vratio = (u32vratio/(MS_U32)u16dst)+1;
        else
            u32vratio /= (MS_U32)u16dst;
    }
    else
        u32vratio = SCALING_MULITPLIER;

    _HAL_GOP_Write16Reg(u32BankOffSet+GOP_4G_VSTRCH, u32vratio , GOP_REG_WORD_MASK);
    _HAL_GOP_Write16Reg(u32BankOffSet+GOP_4G_VSTRCH_INI, 0 , GOP_REG_WORD_MASK);
}

void _MDrv_GRAPHIC_GWIN_Enable(MS_U8 u8GOP, MS_U8 u8win,MS_BOOL bEnable)
{
    MS_U32 u32BankOffSet=0;

    _HAL_GOP_GetBnkOfstByGop(u8GOP, &u32BankOffSet);
    _HAL_GOP_Write16Reg(u32BankOffSet+GOP_4G_CTRL0, 0x0, GOP_BIT11);       // Enable Transparent Color
    _HAL_GOP_Write16Reg(u32BankOffSet+GOP_4G_GWIN0_CTRL(u8win), bEnable, 0x1);
}

void _MDrv_GRAPHIC_UpdateReg(MS_U8 u8Gop)
{
    MS_U16 u16GOPOfst = 0;
    MS_U32 u32BankOffSet=0;

    if(bForceWriteIn)
    {
        _HAL_GOP_Write16Reg(GOP_BAK_SEL, GOP_VAL_FWR, GOP_REG_HW_MASK);
        _HAL_GOP_Write16Reg(GOP_BAK_SEL, 0x0000, GOP_REG_HW_MASK);
    }
    else
    {
        _HAL_GOP_GetBnkOfstByGop(u8Gop,&u32BankOffSet);

        if(u8Gop == 0)
            _HAL_GOP_Write16Reg(GOP_BAK_SEL, GOP_BIT12, GOP_BIT12);
        else if(u8Gop == 1)
            _HAL_GOP_Write16Reg(GOP_BAK_SEL, GOP_BIT13, GOP_BIT13);
        else if(u8Gop == 2)
            _HAL_GOP_Write16Reg(GOP_BAK_SEL, GOP_BIT14, GOP_BIT14);
        else if(u8Gop == 3)
            _HAL_GOP_Write16Reg(GOP_BAK_SEL, GOP_BIT15, GOP_BIT15);
        else if(u8Gop == 4)
            _HAL_GOP_Write16Reg(GOP_BAK_SEL, GOP_BIT4, GOP_BIT4);
    }
}

void _MDrv_GRAPHIC_SetForceWrite(MS_BOOL bEnable)
{
    bForceWriteIn = bEnable;
}

MS_U16 _MDrv_GRAPHIC_GetBpp(MI_FB_ColorFmt_e eColorFmt)
{
    return   _GOP_GetBPP((EN_DRV_GOPColorType)eColorFmt);
}

 void _MDrv_Graphic_SetTransClr_8888(MS_U8 u8Gop, MS_U32 clr, MS_U32 mask)
 {
    MS_U16 regval;
    MS_U32 u32BankOffSet = 0;

    _HAL_GOP_GetBnkOfstByGop(u8Gop, &u32BankOffSet);
    regval = (MS_U16)(clr&0xffff);
    _HAL_GOP_Write16Reg(u32BankOffSet + GOP_4G_TRSCLR_L, regval, GOP_REG_WORD_MASK);

    regval = (MS_U16)((clr>>16&0xff))|(MS_U16)((mask&0xff)<<8) ;
    _HAL_GOP_Write16Reg(u32BankOffSet+GOP_4G_TRSCLR_H, regval, GOP_REG_WORD_MASK);
 }

void _MDrv_Graphic_EnableMultiAlpha(MS_U8 u8Gop, MS_BOOL bEnable)
{
    MS_U32 u32BankOffSet = 0;
    _HAL_GOP_GetBnkOfstByGop(u8Gop,&u32BankOffSet);
    _HAL_GOP_Write16Reg(u32BankOffSet + GOP_4G_MULTI_ALPHA, bEnable<<7, GOP_BIT7);
}

void _MDrv_Graphic_SetAlpha0(MS_U8 u8Gop, MS_U8 u8Alpha0)
{
}
void _MDrv_Graphic_SetAlpha1(MS_U8 u8Gop, MS_U8 u8Alpha1)
{
}
MS_BOOL _MDrv_Graphic_IsSupportMultiAlpha(MS_U8 u8Gop)
{
    return _HAL_GOP_IsSupportMultiAlpha(u8Gop);
}
void _MDrv_Graphic_GetGwinNewAlphaModeEnable(MS_U8 u8Gop, MS_U8 u8win, MS_BOOL* pEnable)
{
    MS_U16 u16Val =0x0;
    MS_U32 u32BankOffSet=0;

    _HAL_GOP_GetBnkOfstByGop(u8Gop, &u32BankOffSet);
    if (0 == u8Gop)
    {
        _HAL_GOP_Read16Reg(u32BankOffSet + GOP_4G_GWIN_ALPHA01(u8win), &u16Val);
        *pEnable = (MS_BOOL)(u16Val>>15);
    }
    else if (1 == u8Gop)
    {
        _HAL_GOP_Read16Reg(u32BankOffSet + GOP_2G_GWIN_ALPHA01(u8win - MAX_GOP0_GWIN), &u16Val);
        *pEnable = (MS_BOOL)(u16Val>>15);
    }
    else if (2 == u8Gop)
    {
        _HAL_GOP_Read16Reg(u32BankOffSet + GOP_1G_GWIN_ALPHA01, &u16Val);
        *pEnable = (MS_BOOL)(u16Val>>15);
    }
    else if (3 == u8Gop)
    {
        _HAL_GOP_Read16Reg(u32BankOffSet + GOP_1GX_GWIN_ALPHA01, &u16Val);
         *pEnable = (MS_BOOL)(u16Val>>15);
    }
    else if (4 == u8Gop)
    {
        _HAL_GOP_Read16Reg(u32BankOffSet + GOP_1GS0_GWIN_ALPHA01,&u16Val);
        *pEnable = (MS_BOOL)(u16Val>>15);
    }
}
void _MDrv_Graphic_SetNewAlphaMode(MS_U8 u8Gop, MS_U8 u8win, MS_BOOL bEnable)
{
    MS_U32 u32BankOffSet = 0;
    MS_U16 u16Val = bEnable << 15;
    MS_U16 regval;
    EN_DRV_GOP_DST_TYPE gopDstType;

    _HAL_GOP_GetBnkOfstByGop(u8Gop, &u32BankOffSet);

    if (0 == u8Gop)
    {
        _HAL_GOP_Write16Reg(u32BankOffSet + GOP_4G_GWIN_ALPHA01(u8win), u16Val, GOP_BIT15);
    }
    else if (1 == u8Gop)
    {
        _HAL_GOP_Write16Reg(u32BankOffSet + GOP_2G_GWIN_ALPHA01(u8win - MAX_GOP0_GWIN), u16Val, GOP_BIT15);
    }
    else if (2 == u8Gop)
    {
        _HAL_GOP_Write16Reg(u32BankOffSet + GOP_1G_GWIN_ALPHA01, u16Val, GOP_BIT15);
    }
    else if (3 == u8Gop)
    {
        _HAL_GOP_Write16Reg(u32BankOffSet + GOP_1GX_GWIN_ALPHA01, u16Val, GOP_BIT15);
    }
    else if (4 == u8Gop)
    {
        _HAL_GOP_Write16Reg(u32BankOffSet + GOP_1GS0_GWIN_ALPHA01, u16Val, GOP_BIT15);
    }
    //get gop Dst
    _HAL_GOP_GetDstPlane(u8Gop, &gopDstType);
    if (E_DRV_GOP_DST_VE == gopDstType)
    {
        _HAL_GOP_Read16Reg(u32BankOffSet + GOP_MUX4_MIX_VE, &regval);
        if(u8Gop == ((regval&(GOP_BIT6|GOP_BIT7|GOP_BIT8))>>6))
        {
             _HAL_GOP_EnableVENewAlphaMode(0, bEnable);
        }
        else if(u8Gop == ((regval&(GOP_BIT9|GOP_BIT10|GOP_BIT11))>>9))
        {
            _HAL_GOP_EnableVENewAlphaMode(1, bEnable);
        }
    }
}
void _MDrv_Graphic_SetGopMux(MS_U8 u8Gop, Gop_MuxSel eGopMux)
{
    _HAL_GOP_SetMux(u8Gop, eGopMux);
}
void _MDrv_Graphic_SetGopHstart(MS_U8 u8Gop, MS_U16 hstart, EN_DRV_GOP_DST_TYPE eDsttype, MS_BOOL bInitHSPD)
{
    MS_U16 u16StrwinStr = 0;
    _HAL_GOP_SetHstart(u8Gop, hstart);
    if (bInitHSPD)
    {
        _HAL_GOP_GetPDByDst(u8Gop, eDsttype, &u16StrwinStr);
        _HAL_GOP_AdjustHSPD(u8Gop, u16StrwinStr, eDsttype);
   }
}
void _MDrv_Graphic_InitHSPDByGop(MS_U8 u8Gop)
{
    EN_DRV_GOP_DST_TYPE gopDst = E_DRV_GOP_DST_INVALID;
    MS_U16 u16StrwinStr = 0;
    _HAL_GOP_GetDstPlane(u8Gop, &gopDst);
    _HAL_GOP_GetPDByDst(u8Gop, gopDst, &u16StrwinStr);
    _HAL_GOP_AdjustHSPD(u8Gop,u16StrwinStr,gopDst);
}