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
/// @file   mhal_graphic.c
/// @brief  MStar graphic Interface
/// @author MStar Semiconductor Inc.
/// @attention
/// <b><em></em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////

//=============================================================================
// Include Files
//=============================================================================
#include <linux/kernel.h>   /* printk() */

#include "mdrv_mstypes.h"
//#include "mdrv_graphic.h"
#include "mhal_graphic.h"
#include "mhal_graphic_reg.h"

#define GOP_DST_MASK                   0xFUL

//Kastor have 5 GOP(s):GOP0 GOP1 GOP2 GOP3 GOP5
//The last gop num is 4 for API,but 5 needs to be set for register
//So we need to do a conversion.
#define GOPNUM_SW_MAX_ID                  (4UL)
#define GOPNUM_HW_MAX_ID                  (5UL)
#define GOPNUM_TRANSFORM(num)          (((num) == GOPNUM_SW_MAX_ID) ? GOPNUM_HW_MAX_ID : (num))
#define GOPNUM_TRANSFORM_INVERSE(num)  (((num) == GOPNUM_HW_MAX_ID) ? GOPNUM_SW_MAX_ID : (num))

//Mux offset
static MS_U8 GOP_Mux_Offset[MAX_GOP_SUPPORT] = {0x0, 0x1, 0x2, 0x3, 0x4};
static MS_U8 GOP_MUX_Delta = 0x4;
static MS_U8 u16PnlHStr[MAX_GOP_SUPPORT] = {0};
static MS_U16 GOP_OP1_PD = 0xA2;
static MS_U16 GOP_PD = 0xDB;
static MS_U16 GOP_NonVS_PD_Offset =  0x5;
static MS_BOOL bGOPVscalePipeDelay[MAX_GOP_SUPPORT+1] = {FALSE, FALSE, FALSE, TRUE, FALSE, FALSE};

//Chip configuration
static MS_U32 _HAL_XC_DEVICE_OFFSET[MAX_XC_DEVICE_NUM] = {MAX_XC_DEVICE0_OFFSET, MAX_XC_DEVICE1_OFFSET};
static MS_U32 u32DeviceID = 0;

#define MAX_TIMMING_INFO 3

typedef struct
{
    MS_U16 timingWidth;
    MS_U16 timingHeight;
    MS_U16 hstart;
}PnlHstarInfo;
//Hard code used for Verify. Support 480P,720P,1080P
PnlHstarInfo  hstartInfos [MAX_XC_DEVICE_NUM * MAX_TIMMING_INFO] =
{
    //DeviceId 0 info
    {720,480,122},
    {1280, 720, 260},
    {1920,1080,192},
    //DeviceId 1 info
    {720,480,122},
    {1280, 720, 260},
    {1920,1080,192},
};

void _HAL_GOP_Write16Reg(MS_U32 u32addr, MS_U16 u16val, MS_U16 mask)
{
    MS_U16 u16tmp = 0;
    u16tmp = GOP_READ2BYTE(u32addr);
    u16tmp &= ~mask;
    u16val &=  mask;
    u16val |=  u16tmp;
    GOP_WRITE2BYTE(u32addr,u16val);
}

void _HAL_GOP_Read16Reg(MS_U32 u32addr, MS_U16* pu16ret)
{
    *pu16ret = GOP_READ2BYTE(u32addr);
}

void _HAL_GOP_Write32Reg(MS_U32 u32addr, MS_U32 u32val)
{
    GOP_WRITE2BYTE(u32addr, (u32val&0xFFFF));
    GOP_WRITE2BYTE(u32addr+2, (u32val&0xFFFF0000)>>16);
}

MS_U8 _HAL_GOP_GetBnkOfstByGop(MS_U8 gop, MS_U32 *pBnkOfst)
{
    if (gop==0)
        *pBnkOfst = GOP_4G_OFST<<8;
    else if (gop==1)
        *pBnkOfst = GOP_2G_OFST<<8;
    else if (gop==2)
        *pBnkOfst = GOP_1G_OFST<<8;
    else if (gop==3)
        *pBnkOfst = GOP_1GX_OFST<<8;
    else if (gop==4)
        *pBnkOfst = GOP_DW_OFST<<8;
    else
        return FALSE;
    return TRUE;
}

void _HAL_GOP_GWIN_SetDstPlane(MS_U8 u8GOP, EN_DRV_GOP_DST_TYPE eDsttype, MS_BOOL bOnlyCheck)
{
    MS_BOOL b1pEnable;
    MS_U16 u16RegVal = 0, u16HSyncMask=GOP_BIT14;
    MS_U32 u32Bankoffset=0;

    switch (eDsttype)
    {
        case E_DRV_GOP_DST_IP0:
            u16RegVal = 0x0;
            b1pEnable = FALSE;
            u16HSyncMask=0;
            break;

        case E_DRV_GOP_DST_IP0_SUB:
            u16RegVal = 0x1;
            b1pEnable = FALSE;
            u16HSyncMask=0;
            break;

        case E_DRV_GOP_DST_OP0:
            u16RegVal = 0x2;
            b1pEnable = FALSE;
            u16HSyncMask=GOP_BIT14;
            break;

        case E_DRV_GOP_DST_VOP:
            u16RegVal = 0x3;
            b1pEnable = FALSE;
            u16HSyncMask=GOP_BIT14;
            break;

        case E_DRV_GOP_DST_VOP_SUB:
            u16RegVal = 0x4;
            b1pEnable = FALSE;
            u16HSyncMask=GOP_BIT14;
            break;

        case E_DRV_GOP_DST_FRC:
            u16RegVal = 0x6;
            b1pEnable = TRUE;
            u16HSyncMask=GOP_BIT14;
            break;

        case E_DRV_GOP_DST_VE:
            u16RegVal = 0x7;
            b1pEnable = TRUE;
            u16HSyncMask=GOP_BIT14;
            break;
        case E_DRV_GOP_DST_DIP:
            u16RegVal = 0x8;
            b1pEnable = TRUE;
            u16HSyncMask=GOP_BIT14;
            break;
        case E_DRV_GOP_DST_OP_DUAL_RATE:
            u16RegVal = 0xB;
            b1pEnable = FALSE;
            u16HSyncMask=0x0;
            break;
        default:
            printk("[%s] invalid DST value\n", __FUNCTION__);
            return;
    }

    if(bOnlyCheck == FALSE)
    {
        _HAL_GOP_GetBnkOfstByGop(u8GOP, &u32Bankoffset);
        _HAL_GOP_Write16Reg(u32Bankoffset + GOP_4G_SRAM_BORROW, b1pEnable?GOP_BIT14:0,GOP_BIT14);
        _HAL_GOP_Write16Reg(u32Bankoffset+GOP_4G_CTRL1, u16RegVal, BMASK(GOP_BIT3:0));
        _HAL_GOP_Write16Reg(u32Bankoffset+GOP_4G_CTRL0, u16HSyncMask, GOP_BIT14);               // Set mask Hsync when VFDE is low
    }
}

void _HAL_GOP_SetBlending(MS_U8 u8GOP, MS_U8 u8win, MS_BOOL bEnable, MS_U8 u8coef)
{
    MS_U32 u32bankoffset = 0;

    _HAL_GOP_GetBnkOfstByGop(u8GOP, &u32bankoffset);
    _HAL_GOP_Write16Reg(u32bankoffset+GOP_4G_GWIN0_CTRL(u8win), bEnable<<14, 0x4000);
    _HAL_GOP_Write16Reg(u32bankoffset+GOP_4G_GWIN_ALPHA01(u8win), u8coef, 0xff);
}
MS_BOOL _HAL_GOP_IsSupportMultiAlpha(MS_U8 u8Gop)
{
    return TRUE;
}
MS_BOOL _HAL_GOP_GetDstPlane(MS_U8 u8GOP, EN_DRV_GOP_DST_TYPE *pGopDst)
{
    MS_U16 u16Regval;
    MS_U32 u32pBankOffSet=0;
    MS_BOOL ret = FALSE;

    if (u8GOP > MAX_GOP_SUPPORT)
    {
        *pGopDst = E_DRV_GOP_DST_INVALID;
        return FALSE;
    }
    _HAL_GOP_GetBnkOfstByGop(u8GOP, &u32pBankOffSet);
    _HAL_GOP_Read16Reg(u32pBankOffSet + GOP_4G_CTRL1, &u16Regval);
    switch (u16Regval & GOP_DST_MASK)
    {
        case 0:
            *pGopDst = E_DRV_GOP_DST_IP0;
            ret = TRUE;
            break;
        case 1:
            *pGopDst = E_DRV_GOP_DST_IP0_SUB;
            ret = TRUE;
            break;
        case 2:
            *pGopDst = E_DRV_GOP_DST_OP0;
            ret = TRUE;
            break;
        case 5:
            *pGopDst = E_DRV_GOP_DST_IP1;
            ret = TRUE;
            break;
        case 6:
            *pGopDst = E_DRV_GOP_DST_OP1;
            ret = TRUE;
            break;
        case 7:
            *pGopDst = E_DRV_GOP_DST_VE;
            ret = TRUE;
            break;
        case 8:
            *pGopDst = E_DRV_GOP_DST_DIP;
            ret = TRUE;
            break;
        default:
            *pGopDst = E_DRV_GOP_DST_INVALID;
            ret  =TRUE;
            break;
    }
    return ret;
}
MS_BOOL _HAL_GOP_EnableVENewAlphaMode(MS_U8 veMuxNum, MS_BOOL bEnable)
{
    MS_U16 u16val = 0x0;
    u16val = (bEnable? BIT(2): 0);
    //Enable OP per pixel new alpha mode
#if 0
    switch (veMuxNum)
    {
        case 0:
    }
#endif
    return TRUE;
}

void _HAL_GOP_InitMux(void)
{
    /*T8
    OP path: support 3 mux (mux0, mux 2, mux3) to blend with SC simultaneously
    IP  path: support mux0 and mux1 to IPMain/IPSub. Only one mux of mux0 and mux1 can be blended to IPMain/IPSub
    SW default setting=> mux0:gop1g, mux1:gop1gx, mux2:gop2g, mux3:gop4g
    */
    MS_U8 gop4g=0, gop2g=1, gop1g=2, gop1gx=3, gop1gs=5;
    _HAL_GOP_Write16Reg(GOP_MUX,((gop1gs<<(GOP_MUX_SHIFT*4))|(gop4g<<(GOP_MUX_SHIFT*3))|(gop2g<<(GOP_MUX_SHIFT*2))|(gop1gx<<(GOP_MUX_SHIFT*1))|gop1g), GOP_REG_WORD_MASK);
    //It's not necessary to consider VE case
    //HAL_GOP_VE_SetOSDEnable(pGOPHalLocal, FALSE, EN_OSD_0, 0x4);
    //HAL_GOP_VE_SetOSDEnable(pGOPHalLocal, FALSE, EN_OSD_1, 0x4);

    _HAL_GOP_Write16Reg(GOP_SC1_VOPNBL, 1<<7, GOP_BIT7);
}

void _HAL_GOP_SetMux(MS_U8 u8Gop, Gop_MuxSel eGopMux)
{
    u8Gop = GOPNUM_TRANSFORM(u8Gop);
    switch(eGopMux)
    {
        case E_GOP_MUX0:
        case E_GOP_MUX1:
        case E_GOP_MUX2:
        case E_GOP_MUX3:
        case E_GOP_MUX4:
        {
            _HAL_GOP_Write16Reg(GOP_MUX, u8Gop<<(GOP_MUX_SHIFT*eGopMux), GOP_REGMUX_MASK<<(GOP_MUX_SHIFT*eGopMux));
        }
        break;
        case E_GOP_OP1_MUX:
        {
            _HAL_GOP_Write16Reg(GOP_MUX_SC1, u8Gop <<12, GOP_REGMUX_MASK<<12);
        }
        break;
        default:
            break;
    }
}

void _HAL_GOP_GetMux(MS_U8* pU8Gop, Gop_MuxSel eGopMux)
{
     MS_U16 u16GopMux=0;
     switch (eGopMux)
     {
        case E_GOP_MUX0:
        case E_GOP_MUX1:
        case E_GOP_MUX2:
        case E_GOP_MUX3:
        case E_GOP_MUX4:
        {
            _HAL_GOP_Read16Reg(GOP_MUX, &u16GopMux);
            *pU8Gop = (u16GopMux >> (eGopMux*GOP_MUX_SHIFT))& GOP_REGMUX_MASK;
        }
        break;
        case E_GOP_OP1_MUX:
        {
            _HAL_GOP_Read16Reg(GOP_MUX_SC1, &u16GopMux);
            *pU8Gop = (u16GopMux& ((BMASK((GOP_MUX_SHIFT-1):0))<<12))>>12;
        }
        break;
        default:
            break;
        //Transform GOPNum for app user
         *pU8Gop = GOPNUM_TRANSFORM_INVERSE(*pU8Gop);
     }
}

void _HAL_GOP_GetPDByDst(MS_U8 u8Gop, EN_DRV_GOP_DST_TYPE gopDst, MS_U16* u16StrwinStr)
{
    switch (gopDst)
    {
        case E_DRV_GOP_DST_OP1:
        {
            *u16StrwinStr = GOP_OP1_PD;
        }
        break;
        case E_DRV_GOP_DST_OP0:
        {
            *u16StrwinStr = u16PnlHStr[u8Gop] + GOP_PD;
        }
        break;
        default:
            *u16StrwinStr = GOP_PD;
            break;
    }
}
void _HAL_GOP_AdjustHSPD(MS_U8 u8Gop, MS_U16 u16StrwinStr, EN_DRV_GOP_DST_TYPE gopDst)
{
    MS_U8 u8Offset = 0;
    MS_U8 i = 0;
    MS_U8 u8Gop_num = 0;
    MS_U16 u16HSPipe = 0;
    MS_U16 u16NonVS_PD_Delay=0;
    MS_U32 u32BankOffSet = 0;

    switch (gopDst)
    {
        case E_DRV_GOP_DST_OP1:
        {
            u8Offset = 0;
        }
        break;
        case E_DRV_GOP_DST_OP0:
        default:
        {
            for (i=0; i < MAX_GOP_MUX; i++)
            {
                _HAL_GOP_GetMux(&u8Gop_num, (Gop_MuxSel)i);
                if (u8Gop_num == u8Gop)
                {
                    u8Offset = GOP_Mux_Offset[i] * GOP_MUX_Delta;
                }
            }
        }
        break;
    }
    u16HSPipe = u16StrwinStr + u8Offset;
    if (bGOPVscalePipeDelay[u8Gop]) {
        if ( E_DRV_GOP_DST_IP0 != gopDst) {
            u16NonVS_PD_Delay = GOP_NonVS_PD_Offset * 2;
            u16HSPipe += u16NonVS_PD_Delay;
        }
    }
    _HAL_GOP_GetBnkOfstByGop(u8Gop, &u32BankOffSet);
    _HAL_GOP_Write16Reg(u32BankOffSet+GOP_4G_HS_PIPE, u16HSPipe, 0x1FFF);
}
void _HAL_GOP_SetHstart(MS_U8 u8Gop, MS_U16 u16PanelHStr)
{
    u16PnlHStr[u8Gop] = u16PanelHStr;
}
void _HAL_GOP_GetHstarByTiming(MS_U16 timingWidth, MS_U16 timingHeight, MS_U16*  hstart)
{
    int i = u32DeviceID * MAX_TIMMING_INFO;
    int j = 0;
    *hstart = 192;
    for(j= 0; j < MAX_TIMMING_INFO; j++)
    {
        if (hstartInfos[i+j].timingWidth == timingWidth
            && hstartInfos[i+j].timingHeight== timingHeight) {
             *hstart  =  hstartInfos[i+j].hstart;
              break;
        }
    }
}
