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


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#if defined(MSOS_TYPE_LINUX_KERNEL)
#include <linux/kernel.h>
#endif

#ifdef MSOS_TYPE_LINUX_KERNEL
#include <asm/div64.h>
#else
#define do_div(x,y) ((x)/=(y))
#endif

// Common Definition
#include "MsTypes.h"
#include "MsCommon.h"
#include "MsIRQ.h"
#include "MsOS.h"
#include "drvMVOP.h"
#include "drvMIU.h"

// Internal Definition
#include "regMVOP.h"
#include "halMVOP.h"


//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define MPLL_CLOCK           (432000000ULL)

#ifndef BIT0
#define BIT0    BIT(0)
#define BIT1    BIT(1)
#define BIT2    BIT(2)
#define BIT3    BIT(3)
#define BIT4    BIT(4)
#define BIT5    BIT(5)
#define BIT6    BIT(6)
#define BIT7    BIT(7)
#define BIT15   BIT(15)
#endif

#define ALIGN_UPTO_16(x)  ((((x) + 15) >> 4) << 4)
#define ALIGN_UPTO_4(x)   ((((x) + 3) >> 2) << 2)

static MS_U32 u32RiuBaseAdd=0;
// Main mvop
static MS_BOOL bMirrorModeVer = 0;
static MS_BOOL bMirrorModeHor = 0;
static MS_BOOL bMirrorMode=0;
static MS_BOOL bEnableFreerunMode = FALSE;
static MS_BOOL b3DLRMode=0;    /// 3D L/R dual buffer mode
static MS_BOOL b3DLRAltOutput = FALSE;    /// 3D L/R line alternative output
static MS_BOOL bNewVSyncMode = FALSE;
static MVOP_RptFldMode eRepeatField = E_MVOP_RPTFLD_NONE;      /// mvop output repeating fields for single field input.
static MVOP_RgbFormat eMainRgbFmt = E_MVOP_RGB_NONE;
static MS_U16  u16InsVBlank=0;

// Sub mvop
static MS_BOOL bSubMirrorModeVer = 0;
static MS_BOOL bSubMirrorModeHor = 0;
static MS_BOOL bSubMirrorMode=0;
static MS_BOOL bSubEnableFreerunMode = FALSE;
static MS_BOOL bSub3DLRMode=0;
static MVOP_RptFldMode eSubRepeatField = E_MVOP_RPTFLD_NONE;   /// mvop output repeating fields for single field input.
static MVOP_RgbFormat eSubRgbFmt = E_MVOP_RGB_NONE;
static MS_BOOL _bSubEnable60P = FALSE;

// 3rd mvop
static MS_BOOL bExMirrorModeVer[3] = { 0, 0, 0};
static MS_BOOL bExMirrorModeHor[3] = { 0, 0, 0};
static MS_BOOL bExMirrorMode[3] = { 0, 0, 0};
static MS_BOOL bExEnableFreerunMode[3] = { 0, 0, 0};
static MS_BOOL bEx3DLRMode[3] = { 0, 0, 0};
static MS_BOOL bExEnable60P[3] = { 0, 0, 0};
static MVOP_RgbFormat eExRgbFmt[3] = {E_MVOP_RGB_NONE, E_MVOP_RGB_NONE, E_MVOP_RGB_NONE};

#define RIU_MAP u32RiuBaseAdd  //obtained in init

#define RIU8    ((unsigned char  volatile *) RIU_MAP)
#define MST_MACRO_START     do {
#define MST_MACRO_END       } while (0)

#if defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_LINUX_KERNEL)
#define __builtin_constant_p(x)                         (0)
#endif

#define HAL_WriteByte( u32Reg, u8Val )                                                 \
    MST_MACRO_START                                                                     \
    if ( __builtin_constant_p( u32Reg ) )                                               \
    {                                                                                   \
        RIU8[((u32Reg) * 2) - ((u32Reg) & 1)] = (unsigned char)u8Val;                                  \
    }                                                                                   \
    else                                                                                \
    {                                                                                   \
        RIU8[(u32Reg << 1) - (u32Reg & 1)] = (unsigned char)u8Val;                                     \
    }                                                                                   \
    MST_MACRO_END

#define HAL_ReadByte( u32Reg )                                                         \
    (__builtin_constant_p( u32Reg ) ?                                                   \
        (((u32Reg) & 0x01) ? RIU8[(u32Reg) * 2 - 1] : RIU8[(u32Reg) * 2]) :             \
        (RIU8[(u32Reg << 1) - (u32Reg & 1)]))

#define HAL_ReadRegBit( u32Reg, u8Mask )                                               \
        (RIU8[(u32Reg) * 2 - ((u32Reg) & 1)] & (u8Mask))

#define HAL_WriteRegBit( u32Reg, bEnable, u8Mask )                                     \
    MST_MACRO_START                                                                     \
    MS_U32 u32Reg8 = ((u32Reg) * 2) - ((u32Reg) & 1);                                   \
    RIU8[u32Reg8] = (bEnable) ? (RIU8[u32Reg8] |  (u8Mask)) :                           \
                                (RIU8[u32Reg8] & ~(u8Mask));                            \
    MST_MACRO_END

#define HAL_WriteByteMask( u32Reg, u8Val, u8Msk )                                      \
    MST_MACRO_START                                                                     \
    MS_U32 u32Reg8 = ((u32Reg) * 2) - ((u32Reg) & 1);                                   \
    RIU8[u32Reg8] = (RIU8[u32Reg8] & ~(u8Msk)) | ((u8Val) & (u8Msk));                   \
    MST_MACRO_END


#define SUB_REG(x)         (x-MVOP_REG_BASE+MVOP_SUB_REG_BASE)

const MS_U16 u16MvopRegBase[3] = { MVOP_REG_BASE, MVOP_SUB_REG_BASE, MVOP_3RD_REG_BASE};
#define MAP_REG(_id, _reg)        ((_reg) - MVOP_REG_BASE + u16MvopRegBase[(_id)])

#define _FUNC_NOT_USED()        do {} while ( 0 )
#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

#define LOWBYTE(u16)            ((MS_U8)(u16))
#define HIGHBYTE(u16)           ((MS_U8)((u16) >> 8))


#define VOP_ON_MIU1                     ((HAL_ReadByte(0x12F2) & BIT3) == BIT3) //g'13
#define _MaskMiuReq_MVOP_R( m )         HAL_WriteRegBit(0x1266, m, BIT3)
#define _MaskMiu1Req_MVOP_R( m )        HAL_WriteRegBit(0x0666, m, BIT3)

#define HAL_MIU_SetReqMask( miu_clients, mask ) \
   do { \
       if (VOP_ON_MIU1 == 0) \
           _MaskMiuReq_##miu_clients( mask ); \
       else \
           _MaskMiu1Req_##miu_clients( mask ); \
   }while(0)

#define SUBVOP_ON_MIU1                  ((HAL_ReadByte(0x12F2) & BIT4) == BIT4) //g'14
#define _MaskMiuReq_SUBMVOP_R( m )         HAL_WriteRegBit(0x1266, m, BIT4)
#define _MaskMiu1Req_SUBMVOP_R( m )        HAL_WriteRegBit(0x0666, m, BIT4)

#define HAL_MIU_SubSetReqMask( miu_clients, mask ) \
   do { \
       if (SUBVOP_ON_MIU1 == 0) \
           _MaskMiuReq_##miu_clients( mask ); \
       else \
           _MaskMiu1Req_##miu_clients( mask ); \
   }while(0)

#define MVOP_DBG_ENABLE 0
#if MVOP_DBG_ENABLE
#if defined( MSOS_TYPE_CE ) || defined(MSOS_TYPE_LINUX_KERNEL)
#define MVOP_DBG(fmt, ...)       printf(fmt, ##__VA_ARGS__)
#else
#define MVOP_DBG(fmt, args...)       printf(fmt, ##args)
#endif
#else
#if defined( MSOS_TYPE_CE ) || defined(MSOS_TYPE_LINUX_KERNEL)
#define MVOP_DBG(fmt, ...)       {}
#else
#define MVOP_DBG(fmt, args...)       {}
#endif
#endif

//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

void HAL_MVOP_RegSetBase(MS_U32 u32Base)
{
#if defined(MSOS_TYPE_LINUX_KERNEL)
    u32RiuBaseAdd = u32Base + 0xDE000000;
#else
    u32RiuBaseAdd = u32Base;
#endif
}

void HAL_MVOP_InitMirrorMode(MS_BOOL bMir)
{
    //set bit[3:7] to support mirror mode
    HAL_WriteRegBit(VOP_MIRROR_CFG, bMir, VOP_MIRROR_CFG_ENABLE);
}

void HAL_MVOP_SetVerticalMirrorMode(MS_BOOL bEnable)
{
    if (VOP_MIRROR_CFG_ENABLE != HAL_ReadRegBit(VOP_MIRROR_CFG, VOP_MIRROR_CFG_ENABLE))
    {
        //printf("Setup mirror mode\n");
        HAL_MVOP_InitMirrorMode(TRUE);
    }

    HAL_WriteRegBit(VOP_MIRROR_CFG, bEnable, VOP_MIRROR_CFG_VEN);
    bMirrorModeVer = bEnable;
}

void HAL_MVOP_SetHorizontallMirrorMode(MS_BOOL bEnable)
{
    if (VOP_MIRROR_CFG_ENABLE != HAL_ReadRegBit(VOP_MIRROR_CFG, VOP_MIRROR_CFG_ENABLE))
    {
        //printf("Setup mirror mode\n");
        HAL_MVOP_InitMirrorMode(TRUE);
    }

    HAL_WriteRegBit(VOP_MIRROR_CFG, bEnable, VOP_MIRROR_CFG_HEN);
    bMirrorModeHor = bEnable;
}

void HAL_MVOP_Init(void)
{
    HAL_WriteByte(VOP_TST_IMG, 0x40);
    HAL_MVOP_Set3DLRAltOutput(DISABLE); //reset to default: disable 3D L/R alternative output.
    HAL_MVOP_Set3DLR2ndCfg(DISABLE);    //reset to default: disable 3D L/R 2nd pitch.
    HAL_MVOP_InitMirrorMode(TRUE);
    HAL_MVOP_SetRgbFormat(E_MVOP_RGB_NONE);

    //Enable dynamic clock gating
    HAL_WriteByteMask(VOP_GCLK, VOP_GCLK_MIU_ON|VOP_GCLK_VCLK_ON, VOP_GCLK_MIU_ON|VOP_GCLK_VCLK_ON);

    HAL_WriteRegBit(VOP_CTRL1, 0, VOP_MVD_VS_MD); //default use original vsync

    // set 432 ->86.4
    HAL_WriteByte(REG_STC0SYN_CW_LL, 0x00);
    HAL_WriteByte(REG_STC0SYN_CW_LL, 0x00);
    HAL_WriteByte(REG_STC0SYN_CW_HL, 0x00);
    HAL_WriteByte(REG_STC0SYN_CW_HH, 0x28);
    HAL_WriteRegBit(REG_UPDATE_STC0_CW, 1, UPDATE_STC0_CW);
}

void HAL_MVOP_SetFieldInverse(MS_BOOL b2MVD, MS_BOOL b2IP)
{
    // Set fld inv & ofld_inv
    HAL_WriteRegBit(VOP_CTRL0+1, b2MVD, BIT3); //inverse the field to MVD
    HAL_WriteRegBit(VOP_CTRL0+1, b2IP, BIT4);  //inverse the field to IP
}

void HAL_MVOP_SetChromaWeighting(MS_BOOL bEnable)
{
    HAL_WriteRegBit(VOP_REG_WEIGHT_CTRL, bEnable, BIT1);
}

//load new value into active registers 0x20-0x26
void HAL_MVOP_LoadReg(void)
{
    HAL_WriteRegBit(VOP_REG_WR, 1, BIT0);
    HAL_WriteRegBit(VOP_REG_WR, 0, BIT0);
}


void HAL_MVOP_SetMIUReqMask(MS_BOOL bEnable)
{
#if 0   //[FIXME]
    if (bEnable)
    {   // mask MVOP2MI to protect MIU
        HAL_MIU_SetReqMask(MVOP_R, 1);
        MsOS_DelayTask(1);
    }
    else
    {   // unmask MVOP2MI
        MsOS_DelayTask(1);
        HAL_MIU_SetReqMask(MVOP_R, 0);
    }
#endif
    MS_U8 u8Miu = VOP_ON_MIU1;
    eMIUClientID eClientID = MIU_CLIENT_MVOP_128BIT_R;
    //printf("Enter %s bEnable=%x ReqMask=0x%x, 0x%x\n", __FUNCTION__, bEnable,
    //    HAL_ReadByte(0x1266), HAL_ReadByte(0x0666));
    if (bEnable)
    {   // mask MVOP2MI to protect MIU
        MDrv_MIU_MaskReq(u8Miu,eClientID);
        MsOS_DelayTask(1);
    }
    else
    {   // unmask MVOP2MI
        MsOS_DelayTask(1);
        MDrv_MIU_UnMaskReq(u8Miu,eClientID);
    }

    //printf(">Exit %s bEnable=%x ReqMask=0x%x, 0x%x\n", __FUNCTION__, bEnable,
    //    HAL_ReadByte(0x1266), HAL_ReadByte(0x0666));
}

void HAL_MVOP_Rst(void)
{
    HAL_WriteRegBit(VOP_CTRL0, 0, BIT0);
    HAL_WriteRegBit(VOP_CTRL0, 1, BIT0);
}

void HAL_MVOP_Enable(MS_BOOL bEnable)
{
    MS_U8 regval;

    regval = HAL_ReadByte(VOP_CTRL0);

    if ( bEnable )
    {
        regval |= 0x1;
    }
    else
    {
        regval &= ~0x1;
    }

    HAL_WriteByte(VOP_CTRL0, regval);
}

MS_BOOL HAL_MVOP_GetEnableState(void)
{
    return (HAL_ReadRegBit(VOP_CTRL0, BIT0));
}

HALMVOPFREQUENCY HAL_MVOP_GetMaxFreerunClk(void)
{
    return HALMVOP_160MHZ;
}

void HAL_MVOP_SetFrequency(HALMVOPFREQUENCY enFrequency)
{
    // clear
    HAL_WriteByteMask(REG_CKG_DC0, 0, CKG_DC0_MASK);
    switch(enFrequency)
    {
        case HALMVOP_SYNCMODE:
            HAL_WriteByteMask(REG_CKG_DC0, CKG_DC0_SYNCHRONOUS, CKG_DC0_MASK);
            break;
        case HALMVOP_FREERUNMODE:
            HAL_WriteByteMask(REG_CKG_DC0, CKG_DC0_FREERUN, CKG_DC0_MASK);
            break;
        case HALMVOP_160MHZ:
            HAL_WriteByteMask(REG_CKG_DC0, CKG_DC0_160MHZ, CKG_DC0_MASK);
            break;
        case HALMVOP_144MHZ:
            HAL_WriteByteMask(REG_CKG_DC0, CKG_DC0_144MHZ, CKG_DC0_MASK);
            break;
        default:
            HAL_WriteByteMask(REG_CKG_DC0, CKG_DC0_SYNCHRONOUS, CKG_DC0_MASK);
            printf("Attention! In HAL_MVOP_SetFrequency default path!\n");
            break;
    }
}

void HAL_MVOP_SetOutputInterlace(MS_BOOL bEnable)
{
    MS_U8 regval;

    regval = HAL_ReadByte(VOP_CTRL0);

    if ( bEnable )
    {
        regval |= 0x80;
    }
    else
    {
        regval &= ~0x80;
    }

    HAL_WriteByte(VOP_CTRL0, regval);
}

void HAL_MVOP_SetPattern(MVOP_Pattern enMVOPPattern)
{
    HAL_WriteByteMask(VOP_TST_IMG, enMVOPPattern, BIT2 | BIT1 | BIT0);
}

MS_BOOL HAL_MVOP_SetTileFormat(MVOP_TileFormat eTileFmt)
{
    if (eTileFmt == E_MVOP_TILE_8x32)
    {
        HAL_WriteRegBit(VOP_REG_WR, 0, BIT1);
        return TRUE;
    }
    else if (eTileFmt == E_MVOP_TILE_16x32)
    {
        HAL_WriteRegBit(VOP_REG_WR, 1, BIT1);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

MS_BOOL HAL_MVOP_GetSupportRgbFormat(MVOP_RgbFormat eRgbFmt)
{
    if ((eRgbFmt == E_MVOP_RGB_565) || (eRgbFmt == E_MVOP_RGB_888))
    {
        return TRUE;
    }
    return FALSE;
}

MS_BOOL HAL_MVOP_SetRgbFormat(MVOP_RgbFormat eRgbFmt)
{
    MS_BOOL bRet = FALSE;
    if (eRgbFmt == E_MVOP_RGB_NONE)
    {
        HAL_WriteByteMask(VOP_RGB_FMT, 0, VOP_RGB_FMT_SEL);
        bRet = TRUE;
    }
    else if (eRgbFmt == E_MVOP_RGB_565)
    {
        HAL_WriteByteMask(VOP_RGB_FMT, VOP_RGB_FMT_565, VOP_RGB_FMT_SEL);
        bRet = TRUE;
    }
    else if (eRgbFmt == E_MVOP_RGB_888)
    {
        HAL_WriteByteMask(VOP_RGB_FMT, VOP_RGB_FMT_888, VOP_RGB_FMT_SEL);
        bRet = TRUE;
    }

    if (bRet == TRUE)
    {
        eMainRgbFmt = eRgbFmt;
    }
    return bRet;
}

void HAL_MVOP_SetBlackBG(void)
{
    MS_U8 regval;

    //set MVOP test pattern to black
    HAL_WriteByte((VOP_TST_IMG + 1), 0x10);
    HAL_WriteByte(VOP_U_PAT      , 0x80);
    HAL_WriteByte((VOP_U_PAT   + 1), 0x80);

    regval = HAL_ReadByte(VOP_TST_IMG);
    HAL_WriteByte(VOP_TST_IMG, 0x02);
    HAL_WriteByte(VOP_TST_IMG, 0x00);
    HAL_WriteByte(VOP_TST_IMG, regval);
}

void HAL_MVOP_SetCropWindow(MVOP_InputCfg *pparam)
{
#if 1
    UNUSED(pparam);
#else // enable it when test code is ready
    MS_U32 x, y;
    MS_U32 u32offset;

    if(!pparam)
    {
        return;
    }
    //set MVOP test pattern to black
    HAL_MVOP_SetBlackBG();
#if 0
    if((pparam->enVideoType == MVOP_H264) && (pparam->u16StripSize == 1920))
    {
        pparam->u16StripSize = 1952;
    }
#endif
    if((pparam->enVideoType == MVOP_MPG) || (pparam->enVideoType == MVOP_MPEG4))
    {
        pparam->u16CropX = (pparam->u16CropX >> 3) << 3; // 8 bytes align
        pparam->u16CropY = (pparam->u16CropY >> 6) << 6; // 64 bytes align
    }
    else if((pparam->enVideoType == MVOP_H264) || (pparam->enVideoType == MVOP_RM))
    {
        pparam->u16CropX = (pparam->u16CropX >> 4) << 4; // 16 bytes align
        pparam->u16CropY = (pparam->u16CropY >> 6) << 6; // 64 bytes align
    }
    else
    {
        MS_ASSERT(0);
    }

    x = (MS_U32)pparam->u16CropX;
    y = (MS_U32)pparam->u16CropY;

    // y offset
    u32offset = ((y * pparam->u16StripSize + (x << 5)) >> 3);
    HAL_WriteByte((VOP_JPG_YSTR0_L    ), (MS_U8)(u32offset));
    HAL_WriteByte((VOP_JPG_YSTR0_L + 1), (MS_U8)(u32offset >> 8));
    HAL_WriteByte((VOP_JPG_YSTR0_H    ), (MS_U8)(u32offset >> 16));
    HAL_WriteByte((VOP_JPG_YSTR0_H + 1), (MS_U8)((u32offset >> 24) & VOP_YUV_STR_HIBITS));

    // uv offset
    u32offset = ((y >> 1) * pparam->u16StripSize + (x << 5)) >> 3;
    HAL_WriteByte((VOP_JPG_UVSTR0_L    ), (MS_U8)(u32offset));
    HAL_WriteByte((VOP_JPG_UVSTR0_L + 1), (MS_U8)(u32offset >> 8));
    HAL_WriteByte((VOP_JPG_UVSTR0_H    ), (MS_U8)(u32offset >> 16));
    HAL_WriteByte((VOP_JPG_UVSTR0_H + 1), (MS_U8)((u32offset >> 24) & VOP_YUV_STR_HIBITS));

    pparam->u16CropWidth= (pparam->u16CropWidth >> 3) << 3;
    // HSize, VSize
    HAL_WriteByte((VOP_JPG_HSIZE    ), LOWBYTE(pparam->u16CropWidth ));
    HAL_WriteByte((VOP_JPG_HSIZE + 1), HIGHBYTE(pparam->u16CropWidth ));
    HAL_WriteByte((VOP_JPG_VSIZE    ), LOWBYTE(pparam->u16CropHeight));
    HAL_WriteByte((VOP_JPG_VSIZE + 1), HIGHBYTE(pparam->u16CropHeight ));

    HAL_WriteByteMask(VOP_MPG_JPG_SWITCH, BIT0, BIT1|BIT0);

    // clear extend strip len bit by default
    HAL_WriteRegBit(VOP_REG_STRIP_ALIGN, 0, BIT0);
    if((pparam->enVideoType == MVOP_MPG) || (pparam->enVideoType == MVOP_MPEG4))
    {
        // Disable H264 or RM Input
        HAL_WriteRegBit(VOP_INPUT_SWITCH, 0, BIT2|BIT3);
        //8*32 tile format
        HAL_WriteRegBit(VOP_REG_WR, 0, BIT1);
    }
    else if((pparam->enVideoType == MVOP_H264) || (pparam->enVideoType == MVOP_RM))
    {
        //16*32 tile format
        HAL_WriteRegBit(VOP_REG_WR, 1, BIT1);
        // SVD mode enable
        HAL_WriteByteMask(VOP_INPUT_SWITCH, BIT3, BIT2|BIT3);
        // set mvop to 64bit interface
        HAL_WriteByteMask(VOP_MIU_IF, VOP_MIU_128BIT, VOP_MIU_128BIT|VOP_MIU_128B_I64);
    }
    HAL_MVOP_LoadReg();
#endif
}

void HAL_MVOP_SetRepeatField(MVOP_RptFldMode eMode)
{
    eRepeatField = eMode;
}

void HAL_MVOP_SetInputMode( VOPINPUTMODE mode, MVOP_InputCfg *pparam )
{
    MS_U8 regval;
    MS_U16 u16strip;
    MS_U16 u16HSize = 0;
    MS_U16 u16VSize = 0;

    //set MVOP test pattern to black
    HAL_MVOP_SetBlackBG();

    //regval = HAL_ReadByte(VOP_MPG_JPG_SWITCH);
    regval = 0;
    regval |= ( mode & 0x3 );

    // clear extend strip len bit by default
    HAL_WriteRegBit(VOP_REG_STRIP_ALIGN, 0, BIT0);

    // set mvop to 128bit_i128 interface
    HAL_WriteByteMask(VOP_MIU_IF, VOP_MIU_128BIT, VOP_MIU_128BIT|VOP_MIU_128B_I64);

    // Disable H264 or RM Input
    HAL_WriteRegBit(VOP_INPUT_SWITCH, 0, BIT2|BIT3);
    //8*32 tile format
    HAL_WriteRegBit(VOP_REG_WR, 0, BIT1);
    HAL_WriteRegBit(VOP_MIRROR_CFG_HI, 0, VOP_REF_SELF_FLD);
    HAL_MVOP_SetFieldInverse(ENABLE, ENABLE);

    if ( mode == VOPINPUT_HARDWIRE )
    {
        HAL_WriteByte(VOP_MPG_JPG_SWITCH, regval);
    }
    else if ( mode == VOPINPUT_HARDWIRECLIP )
    {
        HAL_WriteByte(VOP_MPG_JPG_SWITCH, regval);

        // HSize, VSize
        HAL_WriteByte(VOP_JPG_HSIZE    , LOWBYTE( pparam->u16HSize ));
        HAL_WriteByte((VOP_JPG_HSIZE + 1), HIGHBYTE( pparam->u16HSize ));
        HAL_WriteByte(VOP_JPG_VSIZE    , LOWBYTE( pparam->u16VSize ));
        HAL_WriteByte((VOP_JPG_VSIZE + 1), HIGHBYTE( pparam->u16VSize ));
    }
    else if (mode == VOPINPUT_MCUCTRL)
    {
        bMirrorMode = (bMirrorModeVer||bMirrorModeHor);
        if ( pparam->bProgressive )
            regval |= 0x4;
        else
        {
            regval &= ~0x4;
            regval |= 0x1;  //reg_dc_md=b'11 for interlace input
            if (E_MVOP_RPTFLD_NONE == eRepeatField)
            {
                MVOP_DBG("%s normal NOT repeat field %x\n", __FUNCTION__, eRepeatField);
                //To support mcu mode interlace, need to set h'3B[9]=1,
                //h'11[12]=0, and Y1/UV1 address equal to Y0/UV0 address.
                HAL_WriteRegBit(VOP_MIRROR_CFG_HI, 1, VOP_REF_SELF_FLD);
                HAL_MVOP_SetFieldInverse(ENABLE, DISABLE);
            }
        }

        if ( pparam->bYUV422 )
            regval |= 0x10;
        else
            regval &= ~0x10;

        if ( pparam->b422pack )
            regval |= 0x80;

        if ( pparam->bDramRdContd == 1 )
            regval |= 0x20;
        else
            regval &= ~0x20;

        // for backward compatable to saturn
        // [3] UV-7bit mode don't care
        // [5] dram_rd_md =0
        // [6] Fld don't care
        // [7] 422pack don'care
        HAL_WriteByte(VOP_MPG_JPG_SWITCH, regval);

        if (pparam->u16StripSize == 0)
        {
            if (pparam->bSD)
                u16strip = 720;
            else
                u16strip = 1920;
        }
        else
        {
            u16strip = pparam->u16StripSize;
        }

        // set dc_strip[7:0]
        if ( pparam->bDramRdContd == 0 ) {
            u16strip = u16strip/8;
        }
        else
        {
            if ( pparam->b422pack )
            {
                if (E_MVOP_RGB_888 == eMainRgbFmt)
                {
                    u16strip *= 2; //4bytes/pixel (yuv422:2bytes/pixel)
                }
                // [071016 Andy] support YUV422 pack mode
                if ((u16strip < 1024) || bMirrorMode)
                {
                    u16strip = u16strip/4;
                    // dont extend strip len
                    HAL_WriteRegBit(VOP_REG_STRIP_ALIGN, 0, BIT0);
                }
                else
                {
                    u16strip = u16strip/8;
                    // extend strip len to 2048
                    HAL_WriteRegBit(VOP_REG_STRIP_ALIGN, 1, BIT0);
                }
            }
            else
            {
                u16strip = u16strip/8;
            }
        }

        if (u16strip >= 256 )
        {
            HAL_WriteByte(VOP_DC_STRIP_H, (u16strip>>8));
            //reg_dc_strip_h[2:0] = reg_dc_strip[10:8]
        }
        else
        {
            HAL_WriteByteMask(VOP_DC_STRIP_H, 0, BIT0 | BIT1 | BIT2);
        }

        regval = (MS_U8)u16strip;
        HAL_WriteByte(VOP_DC_STRIP, regval);

        HAL_MVOP_SetYUVBaseAdd(pparam->u32YOffset, pparam->u32UVOffset,
                               pparam->bProgressive, pparam->b422pack);

        if (E_MVOP_RPTFLD_NONE != eRepeatField)
        {
            MVOP_DBG("%s reset eRepeatField=%x ==>", __FUNCTION__, eRepeatField);
            //To output the same field for single field input,
            //do NOT set h'3B[9]=1 and h'11[12]=0
            eRepeatField = E_MVOP_RPTFLD_NONE;    //reset the flag to repeat field
            MVOP_DBG(" %x\n", eRepeatField);
        }

        // HSize
        u16HSize = ALIGN_UPTO_16(pparam->u16HSize);
        if (u16HSize != pparam->u16HSize)
        {
            MVOP_DBG(printf("\n\n Change HSize %d to %d\n", pparam->u16HSize, u16HSize);)
        }
        HAL_WriteByte(VOP_JPG_HSIZE    , LOWBYTE( u16HSize ));
        HAL_WriteByte((VOP_JPG_HSIZE + 1), HIGHBYTE( u16HSize ));

        // VSize
        u16VSize = pparam->u16VSize;
        if (bMirrorModeVer)
        {
            u16VSize = ALIGN_UPTO_4(pparam->u16VSize);
            MVOP_DBG(printf("\n\n Change VSize %d to %d\n", pparam->u16VSize, u16VSize);)
        }
        HAL_WriteByte(VOP_JPG_VSIZE    , LOWBYTE( u16VSize ));
        HAL_WriteByte((VOP_JPG_VSIZE + 1), HIGHBYTE( u16VSize ));
    }

    HAL_MVOP_LoadReg();
}


void HAL_MVOP_EnableUVShift(MS_BOOL bEnable)
{
    MS_U8 regval;

    regval = HAL_ReadByte(VOP_MPG_JPG_SWITCH);

    if (((regval & BIT4) == BIT4) && ((regval & 0x3)== 0x2))
    {   // 422 with MCU control mode
        if (bEnable)
        {
            MS_ASSERT(0);
        }
    }

    // output 420 and interlace
    //[IP - Sheet] : Main Page --- 420CUP
    //[Project] :  Titania2
    //[Description]:   Chroma artifacts when 420to422 is applied duplicate method.
    //[Root cause]: Apply 420to422 average algorithm to all DTV input cases.
    //The average algorithm must cooperate with MVOP.
    HAL_WriteByteMask(VOP_UV_SHIFT, (bEnable)?1:0, 0x3);
}

static MS_BOOL _bEnable60P = false;
void HAL_MVOP_SetEnable60P(MS_BOOL bEnable)
{
    _bEnable60P = bEnable;
}

void HAL_MVOP_EnableFreerunMode(MS_BOOL bEnable)
{
    bEnableFreerunMode = bEnable;
}

void HAL_MVOP_SetVSyncMode(MS_U8 u8Mode)
{
    if (1==u8Mode)
    {
        bNewVSyncMode = TRUE;
    }
    else
    {
        bNewVSyncMode = FALSE;
    }
}

void HAL_MVOP_SetOutputTiming( MVOP_Timing *ptiming )
{
    MS_U8 regval;
    MS_U16 u16BottomField_VS2MVD = 0;
    MS_U16 u16TopField_VS2MVD = 0;

    HAL_WriteByte(VOP_FRAME_VCOUNT    , LOWBYTE( ptiming->u16V_TotalCount ));
    HAL_WriteByte((VOP_FRAME_VCOUNT + 1), HIGHBYTE( ptiming->u16V_TotalCount ));
    HAL_WriteByte(VOP_FRAME_HCOUNT    , LOWBYTE( ptiming->u16H_TotalCount ));
    HAL_WriteByte((VOP_FRAME_HCOUNT + 1), HIGHBYTE( ptiming->u16H_TotalCount ));

    HAL_WriteByte((VOP_VB0_STR     ), LOWBYTE( ptiming->u16VBlank0_Start ));
    HAL_WriteByte((VOP_VB0_STR  + 1), HIGHBYTE( ptiming->u16VBlank0_Start ));
    HAL_WriteByte((VOP_VB0_END     ), LOWBYTE( ptiming->u16VBlank0_End ));
    HAL_WriteByte((VOP_VB0_END  + 1), HIGHBYTE( ptiming->u16VBlank0_End ));
    HAL_WriteByte((VOP_VB1_STR     ), LOWBYTE( ptiming->u16VBlank1_Start ));
    HAL_WriteByte((VOP_VB1_STR  + 1), HIGHBYTE( ptiming->u16VBlank1_Start ));
    HAL_WriteByte((VOP_VB1_END     ), LOWBYTE( ptiming->u16VBlank1_End ));
    HAL_WriteByte((VOP_VB1_END  + 1), HIGHBYTE( ptiming->u16VBlank1_End ));
    HAL_WriteByte((VOP_TF_STR      ), LOWBYTE( ptiming->u16TopField_Start ));
    HAL_WriteByte((VOP_TF_STR   + 1), HIGHBYTE( ptiming->u16TopField_Start ));
    HAL_WriteByte((VOP_BF_STR      ), LOWBYTE( ptiming->u16BottomField_Start ));
    HAL_WriteByte((VOP_BF_STR   + 1), HIGHBYTE( ptiming->u16BottomField_Start ));
    HAL_WriteByte((VOP_HACT_STR    ), LOWBYTE( ptiming->u16HActive_Start ));
    HAL_WriteByte((VOP_HACT_STR + 1), HIGHBYTE( ptiming->u16HActive_Start ));

    HAL_WriteByte((VOP_TF_VS      ), LOWBYTE( ptiming->u16TopField_VS ));
    HAL_WriteByte((VOP_TF_VS   + 1), HIGHBYTE( ptiming->u16TopField_VS ));
    HAL_WriteByte((VOP_BF_VS      ), LOWBYTE( ptiming->u16BottomField_VS ));
    HAL_WriteByte((VOP_BF_VS   + 1), HIGHBYTE( ptiming->u16BottomField_VS ));

    if (bNewVSyncMode)
    {
        MVOP_DBG(printf("MVOP use [NEW] VSync mode\n");)
        #define NEW_VSYNC_MODE_ADVANCE_LINECNT 30

        u16BottomField_VS2MVD = ptiming->u16BottomField_VS - NEW_VSYNC_MODE_ADVANCE_LINECNT;
        MVOP_DBG(printf("BottomField VS ori=0x%x, new=0x%x\n", ptiming->u16BottomField_VS, u16BottomField_VS2MVD);)
        HAL_WriteByte((VOP_BF_VS_MVD    ), LOWBYTE( u16BottomField_VS2MVD ));
        HAL_WriteByte((VOP_BF_VS_MVD + 1), HIGHBYTE( u16BottomField_VS2MVD ));

        u16TopField_VS2MVD = ptiming->u16V_TotalCount - NEW_VSYNC_MODE_ADVANCE_LINECNT;
        MVOP_DBG(printf("TopField VS Vtt=0x%x, new=0x%x\n", ptiming->u16V_TotalCount, u16TopField_VS2MVD);)
        HAL_WriteByte((VOP_TF_VS_MVD    ), LOWBYTE( u16TopField_VS2MVD ));
        HAL_WriteByte((VOP_TF_VS_MVD + 1), HIGHBYTE( u16TopField_VS2MVD ));

        HAL_WriteRegBit(VOP_GCLK, 0, VOP_GCLK_MIU_ON);

        HAL_WriteRegBit(VOP_CTRL1, 1, VOP_MVD_VS_MD); //Use new vsync

        bNewVSyncMode = FALSE; //restore to original mode
    }

    // + S3, set default IMG_HSTR, IMG_VSTR0, IMG_VSTR1
#ifdef _SUPPORT_IMG_OFFSET_
    HAL_WriteByte((VOP_IMG_HSTR    ), LOWBYTE( ptiming->u16HImg_Start));
    HAL_WriteByte((VOP_IMG_HSTR + 1), HIGHBYTE( ptiming->u16HImg_Start ));
    HAL_WriteByte((VOP_IMG_VSTR0   ), LOWBYTE( ptiming->u16VImg_Start0));
    HAL_WriteByte((VOP_IMG_VSTR0+ 1), HIGHBYTE( ptiming->u16VImg_Start0 ));
    HAL_WriteByte((VOP_IMG_VSTR1   ), LOWBYTE( ptiming->u16VImg_Start1 ));
    HAL_WriteByte((VOP_IMG_VSTR1+ 1), HIGHBYTE( ptiming->u16VImg_Start1 ));
#else
    HAL_WriteByte((VOP_IMG_HSTR    ), LOWBYTE( ptiming->u16HActive_Start ));
    HAL_WriteByte((VOP_IMG_HSTR + 1), HIGHBYTE( ptiming->u16HActive_Start ));
    HAL_WriteByte((VOP_IMG_VSTR0   ), LOWBYTE( ptiming->u16VBlank0_End ));
    HAL_WriteByte((VOP_IMG_VSTR0+ 1), HIGHBYTE( ptiming->u16VBlank0_End ));
    HAL_WriteByte((VOP_IMG_VSTR1   ), LOWBYTE( ptiming->u16VBlank1_End ));
    HAL_WriteByte((VOP_IMG_VSTR1+ 1), HIGHBYTE( ptiming->u16VBlank1_End ));
#endif

    if (u16InsVBlank)
    {
        if (HIGHBYTE(u16InsVBlank) != 0)
        {
            printf("%s warning: u16InsVBlank %d is too large! Highbyte will be ignored!\n", __FUNCTION__, u16InsVBlank);
        }
        HAL_WriteByte((VOP_INST_BLANK_VSIZE), LOWBYTE(u16InsVBlank));
    }

    // select mvop output from frame color(black)
    HAL_WriteByte((VOP_TST_IMG + 1), 0x10);
    HAL_WriteByte((VOP_U_PAT      ), 0x80);
    HAL_WriteByte((VOP_U_PAT   + 1), 0x80);
    // set mvop src to test pattern
    regval = HAL_ReadByte(VOP_TST_IMG);
    HAL_WriteByte(VOP_TST_IMG, 0x02);
    // make changed registers take effect
    HAL_MVOP_LoadReg();

    HAL_MVOP_SetMIUReqMask(TRUE);
    // reset mvop to avoid timing change cause mvop hang-up
    HAL_MVOP_Rst();
    HAL_MVOP_SetMIUReqMask(FALSE);

    // select mvop output from mvd
    HAL_WriteByte(VOP_TST_IMG, 0x00);
    HAL_WriteByte(VOP_TST_IMG, regval);
    HAL_WriteRegBit(VOP_CTRL0, ptiming->bHDuplicate, BIT2);// H pixel duplicate

#if 0
    MVOP_DBG(printf("\nMVOP SetOutputTiming\n");)
    MVOP_DBG(printf(" VTot=%u,\t",ptiming->u16V_TotalCount);)
    MVOP_DBG(printf(" HTot=%u,\t",ptiming->u16H_TotalCount);)
    MVOP_DBG(printf(" I/P=%u\n",ptiming->bInterlace);)
    MVOP_DBG(printf(" W=%u,\t",ptiming->u16Width);)
    MVOP_DBG(printf(" H=%u,\t",ptiming->u16Height);)
    MVOP_DBG(printf(" FRate=%u,\t",ptiming->u8Framerate);)
    MVOP_DBG(printf(" HFreq=%u\n",ptiming->u16H_Freq);)
    MVOP_DBG(printf(" Num=0x%x,\t",ptiming->u16Num);)
    MVOP_DBG(printf(" Den=0x%x,\t",ptiming->u16Den);)
    MVOP_DBG(printf(" u16ExpFRate=%u #\n\n", ptiming->u16ExpFrameRate);)
#endif
}

void HAL_MVOP_SetDCClk(MS_U8 clkNum, MS_BOOL bEnable)
{
    MS_ASSERT( (clkNum==0) || (clkNum==1) );
    if (clkNum==0)
    {
        HAL_WriteRegBit(REG_CKG_DC0, !bEnable, CKG_DC0_GATED);
    }
}


void HAL_MVOP_SetSynClk(MVOP_Timing *ptiming)
{
    if(bEnableFreerunMode)
    {
    #if !defined(MSOS_TYPE_LINUX_KERNEL)
        MS_U32 u32FreerunClk = (MS_U32)((MS_U64)(MPLL_CLOCK << 27) / ((MS_U32)ptiming->u16H_TotalCount * (MS_U32)ptiming->u16V_TotalCount * (MS_U32)ptiming->u8Framerate));
    #else
        MS_U32 u32FreerunClk;
        MS_U64 u64MPLL_CLK =  ((MS_U64)MPLL_CLOCK<<27);
        do_div( u64MPLL_CLK, ((MS_U64)ptiming->u16H_TotalCount * (MS_U64)ptiming->u16V_TotalCount * (MS_U64)ptiming->u8Framerate));
        u32FreerunClk = (MS_U32) u64MPLL_CLK;
    #endif
        HAL_MVOP_SetFrequency(HALMVOP_FREERUNMODE);
        HAL_WriteByte((REG_DC0_FREERUN_CW_L  ), LOWBYTE((MS_U16)u32FreerunClk));
        HAL_WriteByte((REG_DC0_FREERUN_CW_L+1), HIGHBYTE((MS_U16)u32FreerunClk));
        HAL_WriteByte((REG_DC0_FREERUN_CW_H  ), LOWBYTE((MS_U16)(u32FreerunClk >> 16)));
        HAL_WriteByte((REG_DC0_FREERUN_CW_H+1), HIGHBYTE((MS_U16)(u32FreerunClk >> 16)));
        HAL_WriteRegBit(REG_UPDATE_DC0_CW, 1, UPDATE_DC0_FREERUN_CW);
        HAL_WriteRegBit(REG_UPDATE_DC0_CW, 0, UPDATE_DC0_FREERUN_CW);
    }
    else if (_bEnable60P)
    {
        //Set DC1 Timing
        MS_U32 u32FrameRate = (MS_U32)ptiming->u16ExpFrameRate;
        MS_U32 u32VSize = 1024;
        MS_U32 u32HSize = ((86400000 / u32FrameRate) * 1000) / u32VSize;

        MS_ASSERT(u32HSize <= 4096);

        HAL_MVOP_SetFrequency(HAL_MVOP_GetMaxFreerunClk());

        HAL_WriteRegBit(VOP_REG_FRAME_RST, 0, BIT15); // reg_frame_rst = 0

        HAL_WriteRegBit(VOP_CTRL0, DISABLE, VOP_FSYNC_EN); // frame sync disable
    }
    else
    {
        HAL_MVOP_SetFrequency(HALMVOP_SYNCMODE);
        HAL_WriteByte((REG_DC0_NUM  ), LOWBYTE( ptiming->u16Num));
        HAL_WriteByte((REG_DC0_NUM+1), HIGHBYTE(ptiming->u16Num));
        HAL_WriteByte((REG_DC0_DEN  ), LOWBYTE( ptiming->u16Den));
        HAL_WriteByte((REG_DC0_DEN+1), HIGHBYTE(ptiming->u16Den));
        HAL_WriteRegBit(REG_UPDATE_DC0_CW, 1, UPDATE_DC0_SYNC_CW);
        HAL_WriteRegBit(REG_UPDATE_DC0_CW, 0, UPDATE_DC0_SYNC_CW);
    }
}


void HAL_MVOP_SetMonoMode(MS_BOOL bEnable)
{
    if(bEnable)
    {
        HAL_WriteByte(VOP_U_PAT  , 0x80);
        HAL_WriteByte((VOP_U_PAT+1), 0x80);

        HAL_WriteRegBit(VOP_INPUT_SWITCH, 1, BIT1);    // Mono mode enable
    }
    else
    {
        HAL_WriteRegBit(VOP_INPUT_SWITCH, 0, BIT1);    //Mono mode disable
    }
}

/******************************************************************************/
/// Set MVOP for H264  Hardwire Mode
/******************************************************************************/
void HAL_MVOP_SetH264HardwireMode(void)
{
    // Hardwire mode
    HAL_WriteByte(VOP_MPG_JPG_SWITCH, 0x00);

    HAL_WriteRegBit(VOP_REG_STRIP_ALIGN, 0, BIT0);

    //16*32 tile format
    HAL_WriteRegBit(VOP_REG_WR, 1, BIT1);

    // SVD mode enable
    HAL_WriteByteMask(VOP_INPUT_SWITCH, BIT3, BIT2|BIT3);

    // set mvop to 128bit_i128 interface
    HAL_WriteByteMask(VOP_MIU_IF, VOP_MIU_128BIT, VOP_MIU_128BIT|VOP_MIU_128B_I64);

    // Write trigger
    HAL_MVOP_LoadReg();
}

/******************************************************************************/
/// Set MVOP for RM  Hardwire Mode
/******************************************************************************/
void HAL_MVOP_SetRMHardwireMode(void)
{
    HAL_MVOP_SetH264HardwireMode();
}

/******************************************************************************/
/// Set MVOP for JPEG Hardwire Mode
/******************************************************************************/
void HAL_MVOP_SetJpegHardwireMode(void)
{
    MS_U8 regval = 0x00;

    regval |= 0x80; // packmode
    regval |= 0x20; // Dram Rd Contd
    HAL_WriteByte(VOP_MPG_JPG_SWITCH, regval);

    // Write trigger
    HAL_MVOP_LoadReg();
}

///Enable 3D L/R dual buffer mode
MS_BOOL HAL_MVOP_Enable3DLR(MS_BOOL bEnable)
{
    HAL_WriteRegBit(VOP_MULTI_WIN_CFG0, bEnable, VOP_LR_BUF_MODE);
    b3DLRMode = bEnable;
    return TRUE;
}

///Get if 3D L/R mode is enabled
MS_BOOL HAL_MVOP_Get3DLRMode(void)
{
    return b3DLRMode;
}

///Get if 3D L/R mode is enabled
MS_BOOL HAL_MVOP_Set3DLRInsVBlank(MS_U16 u16InsLines)
{
    u16InsVBlank = u16InsLines;
    return TRUE;
}

MS_BOOL HAL_MVOP_GetTimingInfoFromRegisters(MVOP_TimingInfo_FromRegisters *pMvopTimingInfo)
{
    if(NULL == pMvopTimingInfo)
    {
        printf("HAL_MVOP_GetTimingInfoFromRegisters():pMvopTimingInfo is NULL\n");
        return FALSE;
    }
    if(HAL_MVOP_GetEnableState() == FALSE)
    {
        printf("MVOP is not enabled!\n");
        pMvopTimingInfo->bEnabled = FALSE;
        return FALSE;
    }
    pMvopTimingInfo->bEnabled = TRUE;
    pMvopTimingInfo->u16H_TotalCount = (HAL_ReadByte((VOP_FRAME_HCOUNT + 1))<< 8) | (HAL_ReadByte((VOP_FRAME_HCOUNT)));
    pMvopTimingInfo->u16V_TotalCount = (HAL_ReadByte((VOP_FRAME_VCOUNT + 1))<< 8) | (HAL_ReadByte((VOP_FRAME_VCOUNT)));
    pMvopTimingInfo->u16VBlank0_Start = (HAL_ReadByte((VOP_VB0_STR + 1))<< 8) | (HAL_ReadByte((VOP_VB0_STR)));
    pMvopTimingInfo->u16VBlank0_End = (HAL_ReadByte((VOP_VB0_END + 1))<< 8) | (HAL_ReadByte((VOP_VB0_END)));
    pMvopTimingInfo->u16VBlank1_Start = (HAL_ReadByte((VOP_VB1_STR + 1))<< 8) | (HAL_ReadByte((VOP_VB1_STR)));
    pMvopTimingInfo->u16VBlank1_End = (HAL_ReadByte((VOP_VB1_END + 1))<< 8) | (HAL_ReadByte((VOP_VB1_END)));
    pMvopTimingInfo->u16TopField_Start = (HAL_ReadByte((VOP_TF_STR + 1))<< 8) | (HAL_ReadByte((VOP_TF_STR)));
    pMvopTimingInfo->u16BottomField_Start = (HAL_ReadByte((VOP_BF_STR + 1))<< 8) | (HAL_ReadByte((VOP_BF_STR)));
    pMvopTimingInfo->u16HActive_Start = (HAL_ReadByte((VOP_HACT_STR + 1))<< 8) | (HAL_ReadByte((VOP_HACT_STR)));
    pMvopTimingInfo->u16TopField_VS = (HAL_ReadByte((VOP_TF_VS + 1))<< 8) | (HAL_ReadByte((VOP_TF_VS)));
    pMvopTimingInfo->u16BottomField_VS = (HAL_ReadByte((VOP_BF_VS + 1))<< 8) | (HAL_ReadByte((VOP_BF_VS)));
    pMvopTimingInfo->bInterlace = (HAL_ReadRegBit(VOP_CTRL0, BIT7) == BIT7);
    return TRUE;
}

void HAL_MVOP_SetYUVBaseAdd(MS_U32 u32YOffset, MS_U32 u32UVOffset, MS_BOOL bProgressive, MS_BOOL b422pack)
{
    MS_U32 u32tmp = 0;
    // Y offset
    u32tmp = u32YOffset >> 3;
    if ( !bProgressive )
    {   //Refine Y offset for interlace repeat bottom field
        if (E_MVOP_RPTFLD_BOT == eRepeatField)
        {
            MVOP_DBG("%d eRepeatField(%x) is bottom!\n", __LINE__, eRepeatField);
            u32tmp += 2;
        }
        else
        {
            MVOP_DBG("%d eRepeatField(%x) is TOP or NONE.\n", __LINE__, eRepeatField);
        }
    }
    HAL_WriteByte(VOP_JPG_YSTR0_L, u32tmp & 0xff);
    HAL_WriteByte((VOP_JPG_YSTR0_L+1), (u32tmp >> 8) & 0xff);
    HAL_WriteByte((VOP_JPG_YSTR0_L+2), (u32tmp >> 16) & 0xff);
    HAL_WriteByte((VOP_JPG_YSTR0_L+3), (u32tmp >> 24) & VOP_YUV_STR_HIBITS);

    if (!bProgressive )
    {   //Y offset of bottom field if interlace
        HAL_WriteByte(VOP_JPG_YSTR1_L, u32tmp & 0xff);
        HAL_WriteByte((VOP_JPG_YSTR1_L+1), (u32tmp >> 8) & 0xff);
        HAL_WriteByte((VOP_JPG_YSTR1_L+2), (u32tmp >> 16) & 0xff);
        HAL_WriteByte((VOP_JPG_YSTR1_L+3), (u32tmp >> 24) & VOP_YUV_STR_HIBITS);
    }

    if (b422pack)
    {
        u32UVOffset = u32YOffset + 16; //add 16 for 128bit; add 8 for 64bit
    }
    // UV offset
    u32tmp = u32UVOffset >> 3;
    if( !bProgressive )
    {  //Refine UV offset for interlace repeat bottom field
        if (E_MVOP_RPTFLD_BOT == eRepeatField)
        {
            MVOP_DBG("%d eRepeatField(%x) is bottom!\n", __LINE__, eRepeatField);
            u32tmp += 2;
        }
        else
        {
            MVOP_DBG("%d eRepeatField(%x) is TOP/NONE.\n", __LINE__, eRepeatField);
        }
    }
    HAL_WriteByte(VOP_JPG_UVSTR0_L, u32tmp & 0xff);
    HAL_WriteByte((VOP_JPG_UVSTR0_L+1), (u32tmp >> 8) & 0xff);
    HAL_WriteByte((VOP_JPG_UVSTR0_L+2), (u32tmp >> 16) & 0xff);
    HAL_WriteByte((VOP_JPG_UVSTR0_L+3), (u32tmp >> 24) & VOP_YUV_STR_HIBITS);

    if( !bProgressive )
    {  //UV offset of bottom field if interlace
        HAL_WriteByte(VOP_JPG_UVSTR1_L, u32tmp & 0xff);
        HAL_WriteByte((VOP_JPG_UVSTR1_L+1), (u32tmp >> 8) & 0xff);
        HAL_WriteByte((VOP_JPG_UVSTR1_L+2), (u32tmp >> 16) & 0xff);
        HAL_WriteByte((VOP_JPG_UVSTR1_L+3), (u32tmp >> 24) & VOP_YUV_STR_HIBITS);
    }

    return;
}

MS_U32 HAL_MVOP_GetYBaseAdd(void)
{
    MS_U32 u32YOffset;
    u32YOffset = HAL_ReadByte(VOP_JPG_YSTR0_L)&0xff;
    u32YOffset |=((HAL_ReadByte((VOP_JPG_YSTR0_L+1))<<8)&0xff00);
    u32YOffset |=((HAL_ReadByte((VOP_JPG_YSTR0_L+2))<<16)&0xff0000);
    u32YOffset |= ((HAL_ReadByte((VOP_JPG_YSTR0_L+3)) & VOP_YUV_STR_HIBITS) <<24);
    return u32YOffset;
}

MS_U32 HAL_MVOP_GetUVBaseAdd(void)
{
    MS_U32 u32UVOffset;
    u32UVOffset = HAL_ReadByte(VOP_JPG_UVSTR0_L)&0xff;
    u32UVOffset |=((HAL_ReadByte((VOP_JPG_UVSTR0_L+1))<<8)&0xff00);
    u32UVOffset |=((HAL_ReadByte((VOP_JPG_UVSTR0_L+2))<<16)&0xff0000);
    u32UVOffset |= ((HAL_ReadByte((VOP_JPG_UVSTR0_L+3)) & VOP_YUV_STR_HIBITS) <<24);
    return u32UVOffset;
}

MS_BOOL HAL_MVOP_Set3DLRAltOutput(MS_BOOL bEnable)
{
    MS_BOOL bEnDualBuff = bEnable ? ENABLE : DISABLE;     //enable dual buffer
    MS_BOOL bEnSWDualBuff = bEnable ? DISABLE : ENABLE;   //buffer controlled by HK instead of FW
    MS_BOOL bEnMirrMaskBase = bEnable ? DISABLE : ENABLE; //do not mask LSB

    //Set 0x27[2] = 1 (enable SW dual buffer mode)
    HAL_WriteRegBit(VOP_REG_WR, bEnDualBuff, VOP_BUF_DUAL);

    //Set 0x38[8] = 0 (use SW dual buffer mode)
    HAL_WriteRegBit(VOP_INFO_FROM_CODEC_H, bEnSWDualBuff, VOP_INFO_FROM_CODEC_DUAL_BUFF);

    //Set 0x3b[7] = 0 (use MVD/HVD firmware send base)
    HAL_WriteRegBit(VOP_MIRROR_CFG, bEnMirrMaskBase, VOP_MASK_BASE_LSB);

    b3DLRAltOutput = bEnable;
    return TRUE;
}

MS_BOOL HAL_MVOP_Get3DLRAltOutput(void)
{
    return b3DLRAltOutput;
}

MS_BOOL HAL_MVOP_Set3DLR2ndCfg(MS_BOOL bEnable)
{
    //Set 0x3c[7] as 1 to enable
    HAL_WriteRegBit(VOP_MULTI_WIN_CFG0, bEnable, VOP_LR_DIFF_SIZE);
    return TRUE;
}

MS_BOOL HAL_MVOP_Get3DLR2ndCfg(void)
{
    MS_BOOL bEnable = FALSE;
    if (VOP_LR_DIFF_SIZE == (VOP_LR_DIFF_SIZE & HAL_ReadRegBit(VOP_MULTI_WIN_CFG0, VOP_LR_DIFF_SIZE)))
    {
        bEnable = TRUE;
    }
    return bEnable;
}

MVOP_DrvMirror HAL_MVOP_GetMirrorMode(MVOP_DevID eID)
{
    MVOP_DrvMirror enMirror = E_VOPMIRROR_NONE;
    switch(eID)
    {
        case E_MVOP_DEV_0:
            if(bMirrorModeVer && bMirrorModeHor)
            {
                enMirror = E_VOPMIRROR_HVBOTH;
            }
            else if(bMirrorModeHor)
            {
                enMirror = E_VOPMIRROR_HORIZONTALL;
            }
            else if(bMirrorModeVer)
            {
                enMirror = E_VOPMIRROR_VERTICAL;
            }
            break;
        case E_MVOP_DEV_1:
            if(bSubMirrorModeVer && bSubMirrorModeHor)
            {
                enMirror = E_VOPMIRROR_HVBOTH;
            }
            else if(bSubMirrorModeHor)
            {
                enMirror = E_VOPMIRROR_HORIZONTALL;
            }
            else if(bSubMirrorModeVer)
            {
                enMirror = E_VOPMIRROR_VERTICAL;
            }
            break;
        default:
            break;
    }

    if(enMirror == E_VOPMIRROR_NONE)
    {
        if(bExMirrorModeVer[eID] && bExMirrorModeHor[eID])
        {
            enMirror = E_VOPMIRROR_HVBOTH;
        }
        else if(bExMirrorModeHor[eID])
        {
            enMirror = E_VOPMIRROR_HORIZONTALL;
        }
        else if(bExMirrorModeVer[eID])
        {
            enMirror = E_VOPMIRROR_VERTICAL;
        }
    }

    return enMirror;
}

MS_BOOL HAL_MVOP_SetVerDup(MS_BOOL bEnable)
{
    HAL_WriteRegBit(VOP_CTRL0, bEnable, BIT3);// V line duplicate
    return TRUE;
}

MS_BOOL HAL_MVOP_GetVerDup(void)
{
    return (HAL_ReadRegBit(VOP_CTRL0, BIT3) == BIT3);
}

///////////////////////   Sub MVOP   ////////////////////////
#if 1
void HAL_MVOP_SubRegSetBase(MS_U32 u32Base)
{
    u32RiuBaseAdd = u32Base;
}

void HAL_MVOP_SubInitMirrorMode(MS_BOOL bMir)
{
    //set bit[3:7] to support mirror mode
    HAL_WriteRegBit(SUB_REG(VOP_MIRROR_CFG), bMir, VOP_MIRROR_CFG_ENABLE);
}

void HAL_MVOP_SubInit(void)
{
    HAL_WriteByte(SUB_REG(VOP_TST_IMG), 0x40);
    HAL_MVOP_SubInitMirrorMode(TRUE);
    HAL_MVOP_SubSetRgbFormat(E_MVOP_RGB_NONE);

    //Enable dynamic clock gating
    HAL_WriteByteMask(SUB_REG(VOP_GCLK), VOP_GCLK_MIU_ON|VOP_GCLK_VCLK_ON, VOP_GCLK_MIU_ON|VOP_GCLK_VCLK_ON);
}

void HAL_MVOP_SubSetVerticalMirrorMode(MS_BOOL bEnable)
{
    if (VOP_MIRROR_CFG_ENABLE != HAL_ReadRegBit(SUB_REG(VOP_MIRROR_CFG), VOP_MIRROR_CFG_ENABLE))
    {
        //printf("Setup mirror mode\n");
        HAL_MVOP_SubInitMirrorMode(TRUE);
    }

    HAL_WriteRegBit(SUB_REG(VOP_MIRROR_CFG), bEnable, VOP_MIRROR_CFG_VEN);
    bSubMirrorModeVer = bEnable;
}

void HAL_MVOP_SubSetHorizontallMirrorMode(MS_BOOL bEnable)
{
    if (VOP_MIRROR_CFG_ENABLE != HAL_ReadRegBit(SUB_REG(VOP_MIRROR_CFG), VOP_MIRROR_CFG_ENABLE))
    {
        //printf("Setup mirror mode\n");
        HAL_MVOP_SubInitMirrorMode(TRUE);
    }

    HAL_WriteRegBit(SUB_REG(VOP_MIRROR_CFG), bEnable, VOP_MIRROR_CFG_HEN);
    bSubMirrorModeHor = bEnable;
}

void HAL_MVOP_SubSetFieldInverse(MS_BOOL b2MVD, MS_BOOL b2IP)
{
    // Set fld inv & ofld_inv
    HAL_WriteRegBit(SUB_REG(VOP_CTRL0+1), b2MVD, BIT3); //inverse the field to MVD
    HAL_WriteRegBit(SUB_REG(VOP_CTRL0+1), b2IP, BIT4);  //inverse the field to IP
}

void HAL_MVOP_SubSetChromaWeighting(MS_BOOL bEnable)
{
    HAL_WriteRegBit(SUB_REG(VOP_REG_WEIGHT_CTRL), bEnable, BIT1);
}

//load new value into active registers 0x20-0x26
void HAL_MVOP_SubLoadReg(void)
{
    HAL_WriteRegBit(SUB_REG(VOP_REG_WR), 1, BIT0);
    HAL_WriteRegBit(SUB_REG(VOP_REG_WR), 0, BIT0);
}

void HAL_MVOP_SubSetMIUReqMask(MS_BOOL bEnable)
{
    return; //FIXME
}

void HAL_MVOP_SubRst(void)
{
    HAL_WriteRegBit(SUB_REG(VOP_CTRL0), 0, BIT0);
    HAL_WriteRegBit(SUB_REG(VOP_CTRL0), 1, BIT0);
}

void HAL_MVOP_SubEnable(MS_BOOL bEnable)
{
    MS_U8 regval;

    regval = HAL_ReadByte(SUB_REG(VOP_CTRL0));

    if ( bEnable )
    {
        regval |= 0x1;
    }
    else
    {
        regval &= ~0x1;
    }

    HAL_WriteByte(SUB_REG(VOP_CTRL0), regval);
}

MS_BOOL HAL_MVOP_SubGetEnableState(void)
{
    return (HAL_ReadRegBit(SUB_REG(VOP_CTRL0), BIT0));
}

HALMVOPFREQUENCY HAL_MVOP_SubGetMaxFreerunClk()
{
    return HALMVOP_160MHZ;
}

//FIXME
void HAL_MVOP_SubSetFrequency(HALMVOPFREQUENCY enFrequency)
{
    // clear
    HAL_WriteByteMask(REG_CKG_SUB_DC0, 0, CKG_SUB_DC0_MASK);
    switch(enFrequency)
    {
        case HALMVOP_SYNCMODE:
            HAL_WriteByteMask(REG_CKG_SUB_DC0, CKG_SUB_DC0_SYNCHRONOUS, CKG_SUB_DC0_MASK);
            break;
        case HALMVOP_FREERUNMODE:
            HAL_WriteByteMask(REG_CKG_SUB_DC0, CKG_SUB_DC0_FREERUN, CKG_SUB_DC0_MASK);
            break;
        case HALMVOP_160MHZ:
            HAL_WriteByteMask(REG_CKG_SUB_DC0, CKG_SUB_DC0_160MHZ, CKG_SUB_DC0_MASK);
            break;
        case HALMVOP_144MHZ:
            HAL_WriteByteMask(REG_CKG_SUB_DC0, CKG_SUB_DC0_144MHZ, CKG_SUB_DC0_MASK);
            break;
        default:
            HAL_WriteByteMask(REG_CKG_SUB_DC0, CKG_SUB_DC0_SYNCHRONOUS, CKG_SUB_DC0_MASK);
            printf("Attention! In HAL_MVOP_SubSetFrequency default path!\n");
            break;
    }
}

void HAL_MVOP_SubSetOutputInterlace(MS_BOOL bEnable)
{
    MS_U8 regval;

    regval = HAL_ReadByte(SUB_REG(VOP_CTRL0));

    if ( bEnable )
    {
        regval |= 0x80;
    }
    else
    {
        regval &= ~0x80;
    }

    HAL_WriteByte(SUB_REG(VOP_CTRL0), regval);
}

void HAL_MVOP_SubSetPattern(MVOP_Pattern enMVOPPattern)
{
    HAL_WriteByteMask(SUB_REG(VOP_TST_IMG), enMVOPPattern, BIT2 | BIT1 | BIT0);
}

MS_BOOL HAL_MVOP_SubSetTileFormat(MVOP_TileFormat eTileFmt)
{
    if (eTileFmt == E_MVOP_TILE_8x32)
    {
        HAL_WriteRegBit(SUB_REG(VOP_REG_WR), 0, BIT1);
        return TRUE;
    }
    else if (eTileFmt == E_MVOP_TILE_16x32)
    {
        HAL_WriteRegBit(SUB_REG(VOP_REG_WR), 1, BIT1);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

MS_BOOL HAL_MVOP_SubSetRgbFormat(MVOP_RgbFormat eRgbFmt)
{
    MS_BOOL bRet = FALSE;
    if (eRgbFmt == E_MVOP_RGB_NONE)
    {
        HAL_WriteByteMask(SUB_REG(VOP_RGB_FMT), 0, VOP_RGB_FMT_SEL);
        bRet = TRUE;
    }
    else if (eRgbFmt == E_MVOP_RGB_565)
    {
        HAL_WriteByteMask(SUB_REG(VOP_RGB_FMT), VOP_RGB_FMT_565, VOP_RGB_FMT_SEL);
        bRet = TRUE;
    }
    else if (eRgbFmt == E_MVOP_RGB_888)
    {
        HAL_WriteByteMask(SUB_REG(VOP_RGB_FMT), VOP_RGB_FMT_888, VOP_RGB_FMT_SEL);
        bRet = TRUE;
    }

    if (bRet == TRUE)
    {
        eSubRgbFmt = eRgbFmt;
    }
    return bRet;
}

void HAL_MVOP_SubSetBlackBG(void)
{
    MS_U8 regval;

    //set MVOP test pattern to black
    HAL_WriteByte(SUB_REG(VOP_TST_IMG + 1), 0x10);
    HAL_WriteByte(SUB_REG(VOP_U_PAT      ), 0x80);
    HAL_WriteByte(SUB_REG(VOP_U_PAT   + 1), 0x80);

    regval = HAL_ReadByte(SUB_REG(VOP_TST_IMG));
    HAL_WriteByte(SUB_REG(VOP_TST_IMG), 0x02);
    HAL_WriteByte(SUB_REG(VOP_TST_IMG), 0x00);
    HAL_WriteByte(SUB_REG(VOP_TST_IMG), regval);
}

void HAL_MVOP_SubSetCropWindow(MVOP_InputCfg *pparam)
{
#if 1
    UNUSED(pparam);
#else // enable it when test code is ready
    MS_U32 x, y;
    MS_U32 u32offset;

    if(!pparam)
    {
        return;
    }
    //set MVOP test pattern to black
    HAL_MVOP_SubSetBlackBG();
#if 0
    if((pparam->enVideoType == MVOP_H264) && (pparam->u16StripSize == 1920))
    {
        pparam->u16StripSize = 1952;
    }
#endif
    if((pparam->enVideoType == MVOP_MPG) || (pparam->enVideoType == MVOP_MPEG4))
    {
        pparam->u16CropX = (pparam->u16CropX >> 3) << 3; // 8 bytes align
        pparam->u16CropY = (pparam->u16CropY >> 6) << 6; // 64 bytes align
    }
    else if((pparam->enVideoType == MVOP_H264) || (pparam->enVideoType == MVOP_RM))
    {
        pparam->u16CropX = (pparam->u16CropX >> 4) << 4; // 16 bytes align
        pparam->u16CropY = (pparam->u16CropY >> 6) << 6; // 64 bytes align
    }
    else
    {
        MS_ASSERT(0);
    }

    x = (MS_U32)pparam->u16CropX;
    y = (MS_U32)pparam->u16CropY;

    // y offset
    u32offset = ((y * pparam->u16StripSize + (x << 5)) >> 3);
    HAL_WriteByte(SUB_REG(VOP_JPG_YSTR0_L    ), (MS_U8)(u32offset));
    HAL_WriteByte(SUB_REG(VOP_JPG_YSTR0_L + 1), (MS_U8)(u32offset >> 8));
    HAL_WriteByte(SUB_REG(VOP_JPG_YSTR0_H    ), (MS_U8)(u32offset >> 16));
    HAL_WriteByte(SUB_REG(VOP_JPG_YSTR0_H + 1), (MS_U8)((u32offset >> 24) & VOP_YUV_STR_HIBITS));

    // uv offset
    u32offset = ((y >> 1) * pparam->u16StripSize + (x << 5)) >> 3;
    HAL_WriteByte(SUB_REG(VOP_JPG_UVSTR0_L    ), (MS_U8)(u32offset));
    HAL_WriteByte(SUB_REG(VOP_JPG_UVSTR0_L + 1), (MS_U8)(u32offset >> 8));
    HAL_WriteByte(SUB_REG(VOP_JPG_UVSTR0_H    ), (MS_U8)(u32offset >> 16));
    HAL_WriteByte(SUB_REG(VOP_JPG_UVSTR0_H + 1), (MS_U8)((u32offset >> 24) & VOP_YUV_STR_HIBITS));

    pparam->u16CropWidth= (pparam->u16CropWidth >> 3) << 3;
    // HSize, VSize
    HAL_WriteByte(SUB_REG(VOP_JPG_HSIZE    ), LOWBYTE(pparam->u16CropWidth ));
    HAL_WriteByte(SUB_REG(VOP_JPG_HSIZE + 1), HIGHBYTE(pparam->u16CropWidth ));
    HAL_WriteByte(SUB_REG(VOP_JPG_VSIZE    ), LOWBYTE(pparam->u16CropHeight));
    HAL_WriteByte(SUB_REG(VOP_JPG_VSIZE + 1), HIGHBYTE(pparam->u16CropHeight ));

    HAL_WriteByteMask(SUB_REG(VOP_MPG_JPG_SWITCH), BIT0, BIT1|BIT0);

    // clear extend strip len bit by default
    HAL_WriteRegBit(SUB_REG(VOP_REG_STRIP_ALIGN), 0, BIT0);
    if((pparam->enVideoType == MVOP_MPG) || (pparam->enVideoType == MVOP_MPEG4))
    {
        // Disable H264 or RM Input
        HAL_WriteRegBit(SUB_REG(VOP_INPUT_SWITCH), 0, BIT2|BIT3);
        //8*32 tile format
        HAL_WriteRegBit(SUB_REG(VOP_REG_WR), 0, BIT1);
    }
    else if((pparam->enVideoType == MVOP_H264) || (pparam->enVideoType == MVOP_RM))
    {
        //16*32 tile format
        HAL_WriteRegBit(SUB_REG(VOP_REG_WR), 1, BIT1);
        // SVD mode enable
        HAL_WriteByteMask(SUB_REG(VOP_INPUT_SWITCH), BIT3, BIT2|BIT3);
        // set mvop to 128bit_i128 interface
        HAL_WriteByteMask(SUB_REG(VOP_MIU_IF), VOP_MIU_128BIT, VOP_MIU_128BIT|VOP_MIU_128B_I64);
    }
    HAL_MVOP_SubLoadReg();
#endif
}

void HAL_MVOP_SubSetRepeatField(MVOP_RptFldMode eMode)
{
    eSubRepeatField = eMode;
}

void HAL_MVOP_SubSetInputMode( VOPINPUTMODE mode, MVOP_InputCfg *pparam )
{
    MS_U8 regval;
    MS_U16 u16strip;
    MS_U16 u16HSize = 0;
    MS_U16 u16VSize = 0;

    //set MVOP test pattern to black
    HAL_MVOP_SubSetBlackBG();

    //regval = HAL_ReadByte(SUB_REG(VOP_MPG_JPG_SWITCH);
    regval = 0;
    regval |= ( mode & 0x3 );

    // clear extend strip len bit by default
    HAL_WriteRegBit(SUB_REG(VOP_REG_STRIP_ALIGN), 0, BIT0);

    // set mvop to 128bit_i128 interface
    HAL_WriteByteMask(SUB_REG(VOP_MIU_IF), VOP_MIU_128BIT, VOP_MIU_128BIT|VOP_MIU_128B_I64);

    // Disable H264 or RM Input
    HAL_WriteRegBit(SUB_REG(VOP_INPUT_SWITCH), 0, BIT2|BIT3);
    //8*32 tile format
    HAL_WriteRegBit(SUB_REG(VOP_REG_WR), 0, BIT1);
    HAL_WriteRegBit(SUB_REG(VOP_MIRROR_CFG_HI), 0, VOP_REF_SELF_FLD);
    HAL_MVOP_SubSetFieldInverse(ENABLE, ENABLE);

    if ( mode == VOPINPUT_HARDWIRE )
    {
        HAL_WriteByte(SUB_REG(VOP_MPG_JPG_SWITCH), regval);
    }
    else if ( mode == VOPINPUT_HARDWIRECLIP )
    {
        HAL_WriteByte(SUB_REG(VOP_MPG_JPG_SWITCH), regval);

        // HSize, VSize
        HAL_WriteByte(SUB_REG(VOP_JPG_HSIZE    ), LOWBYTE( pparam->u16HSize ));
        HAL_WriteByte(SUB_REG(VOP_JPG_HSIZE + 1), HIGHBYTE( pparam->u16HSize ));
        HAL_WriteByte(SUB_REG(VOP_JPG_VSIZE    ), LOWBYTE( pparam->u16VSize ));
        HAL_WriteByte(SUB_REG(VOP_JPG_VSIZE + 1), HIGHBYTE( pparam->u16VSize ));
    }
    else if (mode == VOPINPUT_MCUCTRL)
    {
        bSubMirrorMode = (bSubMirrorModeVer||bSubMirrorModeHor);
        if ( pparam->bProgressive )
            regval |= 0x4;
        else
        {
            regval &= ~0x4;
            regval |= 0x1;  //reg_dc_md=b'11 for interlace input
            if (E_MVOP_RPTFLD_NONE == eSubRepeatField)
            {
                MVOP_DBG("%s normal NOT repeat field %x\n", __FUNCTION__, eSubRepeatField);
                //To support mcu mode interlace, need to set h'3B[9]=1,
                //h'11[12]=0, and Y1/UV1 address equal to Y0/UV0 address.
                HAL_WriteRegBit(SUB_REG(VOP_MIRROR_CFG_HI), 1, VOP_REF_SELF_FLD);
                HAL_MVOP_SubSetFieldInverse(ENABLE, DISABLE);
            }
        }

        if ( pparam->bYUV422 )
            regval |= 0x10;
        else
            regval &= ~0x10;

        if ( pparam->b422pack )
            regval |= 0x80;

        if ( pparam->bDramRdContd == 1 )
            regval |= 0x20;
        else
            regval &= ~0x20;

        // for backward compatable to saturn
        // [3] UV-7bit mode don't care
        // [5] dram_rd_md =0
        // [6] Fld don't care
        // [7] 422pack don'care
        HAL_WriteByte(SUB_REG(VOP_MPG_JPG_SWITCH), regval);

        if (pparam->u16StripSize == 0)
        {
            if (pparam->bSD)
                u16strip = 720;
            else
                u16strip = 1920;
        }
        else
        {
            u16strip = pparam->u16StripSize;
        }

        // set dc_strip[7:0]
        if (pparam->bDramRdContd == 0)
        {
            u16strip = u16strip/8;
        }
        else
        {
            if ( pparam->b422pack )
            {
                if (E_MVOP_RGB_888 == eSubRgbFmt)
                {
                    u16strip *= 2;
                }

                if ((u16strip < 1024) || bSubMirrorMode)
                {
                    u16strip = u16strip/4;
                    // dont extend strip len
                    HAL_WriteRegBit(SUB_REG(VOP_REG_STRIP_ALIGN), 0, BIT0);
                }
                else
                {
                    u16strip = u16strip/8;
                    // extend strip len to 2048
                    HAL_WriteRegBit(SUB_REG(VOP_REG_STRIP_ALIGN), 1, BIT0);
                }
            }
            else
            {
                u16strip = u16strip/8;
            }
        }

        if (u16strip >= 256 )
        {
            HAL_WriteByte(SUB_REG(VOP_DC_STRIP_H), (u16strip>>8));
            //reg_dc_strip_h[2:0] = reg_dc_strip[10:8]
        }
        else
        {
            HAL_WriteByteMask(SUB_REG(VOP_DC_STRIP_H), 0, BIT0 | BIT1 | BIT2);
        }

        regval = (MS_U8)u16strip;
        HAL_WriteByte(SUB_REG(VOP_DC_STRIP), regval);

        HAL_MVOP_SubSetYUVBaseAdd(pparam->u32YOffset, pparam->u32UVOffset,
                               pparam->bProgressive, pparam->b422pack);

        if (E_MVOP_RPTFLD_NONE != eSubRepeatField)
        {
            MVOP_DBG("%s reset eRepeatField=%x ==>", __FUNCTION__, eSubRepeatField);
            //To output the same field for single field input,
            //do NOT set h'3B[9]=1 and h'11[12]=0
            eSubRepeatField = E_MVOP_RPTFLD_NONE;    //reset the flag to repeat field
            MVOP_DBG(" %x\n", eSubRepeatField);
        }

        // HSize
        u16HSize = ALIGN_UPTO_16(pparam->u16HSize);
        if (u16HSize != pparam->u16HSize)
        {
            MVOP_DBG(printf("\n\n Change HSize %d to %d\n", pparam->u16HSize, u16HSize);)
        }
        HAL_WriteByte(SUB_REG(VOP_JPG_HSIZE    ), LOWBYTE( u16HSize ));
        HAL_WriteByte(SUB_REG(VOP_JPG_HSIZE + 1), HIGHBYTE( u16HSize ));

        // VSize
        u16VSize = pparam->u16VSize;
        if (bSubMirrorModeVer)
        {
            u16VSize = ALIGN_UPTO_4(pparam->u16VSize);
            MVOP_DBG(printf("\n\n Change VSize %d to %d\n", pparam->u16VSize, u16VSize);)
        }
        HAL_WriteByte(SUB_REG(VOP_JPG_VSIZE    ), LOWBYTE( u16VSize ));
        HAL_WriteByte(SUB_REG(VOP_JPG_VSIZE + 1), HIGHBYTE( u16VSize ));
    }

    HAL_MVOP_SubLoadReg();
}


void HAL_MVOP_SubEnableUVShift(MS_BOOL bEnable)
{
    MS_U8 regval;

    regval = HAL_ReadByte(SUB_REG(VOP_MPG_JPG_SWITCH));

    if (((regval & BIT4) == BIT4) && ((regval & 0x3)== 0x2))
    {   // 422 with MCU control mode
        if (bEnable)
        {
            MS_ASSERT(0);
        }
    }

    // output 420 and interlace
    //[IP - Sheet] : Main Page --- 420CUP
    //[Project] :  Titania2
    //[Description]:   Chroma artifacts when 420to422 is applied duplicate method.
    //[Root cause]: Apply 420to422 average algorithm to all DTV input cases.
    //The average algorithm must cooperate with MVOP.
    HAL_WriteByteMask(SUB_REG(VOP_UV_SHIFT), (bEnable)?1:0, 0x3);
}


void HAL_MVOP_SubSetEnable60P(MS_BOOL bEnable)
{
    _bSubEnable60P = bEnable;
}

void HAL_MVOP_SubEnableFreerunMode(MS_BOOL bEnable)
{
    bSubEnableFreerunMode = bEnable;
}

void HAL_MVOP_SubSetOutputTiming( MVOP_Timing *ptiming )
{
    MS_U8 regval;

    HAL_WriteByte(SUB_REG(VOP_FRAME_VCOUNT    ), LOWBYTE( ptiming->u16V_TotalCount ));
    HAL_WriteByte(SUB_REG(VOP_FRAME_VCOUNT + 1), HIGHBYTE( ptiming->u16V_TotalCount ));
    HAL_WriteByte(SUB_REG(VOP_FRAME_HCOUNT    ), LOWBYTE( ptiming->u16H_TotalCount ));
    HAL_WriteByte(SUB_REG(VOP_FRAME_HCOUNT + 1), HIGHBYTE( ptiming->u16H_TotalCount ));

    HAL_WriteByte(SUB_REG(VOP_VB0_STR     ), LOWBYTE( ptiming->u16VBlank0_Start ));
    HAL_WriteByte(SUB_REG(VOP_VB0_STR  + 1), HIGHBYTE( ptiming->u16VBlank0_Start ));
    HAL_WriteByte(SUB_REG(VOP_VB0_END     ), LOWBYTE( ptiming->u16VBlank0_End ));
    HAL_WriteByte(SUB_REG(VOP_VB0_END  + 1), HIGHBYTE( ptiming->u16VBlank0_End ));
    HAL_WriteByte(SUB_REG(VOP_VB1_STR     ), LOWBYTE( ptiming->u16VBlank1_Start ));
    HAL_WriteByte(SUB_REG(VOP_VB1_STR  + 1), HIGHBYTE( ptiming->u16VBlank1_Start ));
    HAL_WriteByte(SUB_REG(VOP_VB1_END     ), LOWBYTE( ptiming->u16VBlank1_End ));
    HAL_WriteByte(SUB_REG(VOP_VB1_END  + 1), HIGHBYTE( ptiming->u16VBlank1_End ));
    HAL_WriteByte(SUB_REG(VOP_TF_STR      ), LOWBYTE( ptiming->u16TopField_Start ));
    HAL_WriteByte(SUB_REG(VOP_TF_STR   + 1), HIGHBYTE( ptiming->u16TopField_Start ));
    HAL_WriteByte(SUB_REG(VOP_BF_STR      ), LOWBYTE( ptiming->u16BottomField_Start ));
    HAL_WriteByte(SUB_REG(VOP_BF_STR   + 1), HIGHBYTE( ptiming->u16BottomField_Start ));
    HAL_WriteByte(SUB_REG(VOP_HACT_STR    ), LOWBYTE( ptiming->u16HActive_Start ));
    HAL_WriteByte(SUB_REG(VOP_HACT_STR + 1), HIGHBYTE( ptiming->u16HActive_Start ));

    HAL_WriteByte(SUB_REG(VOP_TF_VS      ), LOWBYTE( ptiming->u16TopField_VS ));
    HAL_WriteByte(SUB_REG(VOP_TF_VS   + 1), HIGHBYTE( ptiming->u16TopField_VS ));
    HAL_WriteByte(SUB_REG(VOP_BF_VS      ), LOWBYTE( ptiming->u16BottomField_VS ));
    HAL_WriteByte(SUB_REG(VOP_BF_VS   + 1), HIGHBYTE( ptiming->u16BottomField_VS ));

    // + S3, set default IMG_HSTR, IMG_VSTR0, IMG_VSTR1
#ifdef _SUPPORT_IMG_OFFSET_
    HAL_WriteByte(SUB_REG(VOP_IMG_HSTR    ), LOWBYTE( ptiming->u16HImg_Start));
    HAL_WriteByte(SUB_REG(VOP_IMG_HSTR + 1), HIGHBYTE( ptiming->u16HImg_Start ));
    HAL_WriteByte(SUB_REG(VOP_IMG_VSTR0   ), LOWBYTE( ptiming->u16VImg_Start0));
    HAL_WriteByte(SUB_REG(VOP_IMG_VSTR0+ 1), HIGHBYTE( ptiming->u16VImg_Start0 ));
    HAL_WriteByte(SUB_REG(VOP_IMG_VSTR1   ), LOWBYTE( ptiming->u16VImg_Start1 ));
    HAL_WriteByte(SUB_REG(VOP_IMG_VSTR1+ 1), HIGHBYTE( ptiming->u16VImg_Start1 ));
#else
    HAL_WriteByte(SUB_REG(VOP_IMG_HSTR    ), LOWBYTE( ptiming->u16HActive_Start ));
    HAL_WriteByte(SUB_REG(VOP_IMG_HSTR + 1), HIGHBYTE( ptiming->u16HActive_Start ));
    HAL_WriteByte(SUB_REG(VOP_IMG_VSTR0   ), LOWBYTE( ptiming->u16VBlank0_End ));
    HAL_WriteByte(SUB_REG(VOP_IMG_VSTR0+ 1), HIGHBYTE( ptiming->u16VBlank0_End ));
    HAL_WriteByte(SUB_REG(VOP_IMG_VSTR1   ), LOWBYTE( ptiming->u16VBlank1_End ));
    HAL_WriteByte(SUB_REG(VOP_IMG_VSTR1+ 1), HIGHBYTE( ptiming->u16VBlank1_End ));
#endif
    // select mvop output from frame color(black)
    HAL_WriteByte(SUB_REG(VOP_TST_IMG + 1), 0x10);
    HAL_WriteByte(SUB_REG(VOP_U_PAT      ), 0x80);
    HAL_WriteByte(SUB_REG(VOP_U_PAT   + 1), 0x80);
    // set mvop src to test pattern
    regval = HAL_ReadByte(SUB_REG(VOP_TST_IMG));
    HAL_WriteByte(SUB_REG(VOP_TST_IMG), 0x02);
    // make changed registers take effect
    HAL_MVOP_SubLoadReg();

    HAL_MVOP_SubSetMIUReqMask(TRUE);
    // reset mvop to avoid timing change cause mvop hang-up
    HAL_MVOP_SubRst();
    HAL_MVOP_SubSetMIUReqMask(FALSE);

    // select mvop output from mvd
    HAL_WriteByte(SUB_REG(VOP_TST_IMG), 0x00);
    HAL_WriteByte(SUB_REG(VOP_TST_IMG), regval);
    HAL_WriteRegBit(SUB_REG(VOP_CTRL0), ptiming->bHDuplicate, BIT2);// H pixel duplicate

#if 0
    MVOP_DBG(printf("\nMVOP SetOutputTiming\n");)
    MVOP_DBG(printf(" VTot=%u,\t",ptiming->u16V_TotalCount);)
    MVOP_DBG(printf(" HTot=%u,\t",ptiming->u16H_TotalCount);)
    MVOP_DBG(printf(" I/P=%u\n",ptiming->bInterlace);)
    MVOP_DBG(printf(" W=%u,\t",ptiming->u16Width);)
    MVOP_DBG(printf(" H=%u,\t",ptiming->u16Height);)
    MVOP_DBG(printf(" FRate=%u,\t",ptiming->u8Framerate);)
    MVOP_DBG(printf(" HFreq=%u\n",ptiming->u16H_Freq);)
    MVOP_DBG(printf(" Num=0x%x,\t",ptiming->u16Num);)
    MVOP_DBG(printf(" Den=0x%x,\t",ptiming->u16Den);)
    MVOP_DBG(printf(" u16ExpFRate=%u #\n\n", ptiming->u16ExpFrameRate);)
#endif
}

void HAL_MVOP_SubSetDCClk(MS_U8 clkNum, MS_BOOL bEnable)
{
    MS_ASSERT( (clkNum==0) || (clkNum==1) );
    if (clkNum==0)
    {
        HAL_WriteRegBit(REG_CKG_SUB_DC0, !bEnable, CKG_SUB_DC0_GATED);
    }
}


void HAL_MVOP_SubSetSynClk(MVOP_Timing *ptiming)
{
    if(bSubEnableFreerunMode)
    {
    #if !defined(MSOS_TYPE_LINUX_KERNEL)
        MS_U32 u32FreerunClk = (MS_U32)((MS_U64)(MPLL_CLOCK << 27) / ((MS_U32)ptiming->u16H_TotalCount * (MS_U32)ptiming->u16V_TotalCount * (MS_U32)ptiming->u8Framerate));
    #else
        MS_U32 u32FreerunClk;
        MS_U64 u64MPLL_CLK = (MS_U64)MPLL_CLOCK << 27;
        do_div(u64MPLL_CLK, ((MS_U64)ptiming->u16H_TotalCount * (MS_U64)ptiming->u16V_TotalCount * (MS_U64)ptiming->u8Framerate));
        u32FreerunClk = (MS_U32)u64MPLL_CLK;
    #endif
        HAL_MVOP_SubSetFrequency(HALMVOP_FREERUNMODE);
        HAL_WriteByte((REG_DC1_FREERUN_CW_L  ), LOWBYTE((MS_U16)u32FreerunClk));
        HAL_WriteByte((REG_DC1_FREERUN_CW_L+1), HIGHBYTE((MS_U16)u32FreerunClk));
        HAL_WriteByte((REG_DC1_FREERUN_CW_H  ), LOWBYTE((MS_U16)(u32FreerunClk >> 16)));
        HAL_WriteByte((REG_DC1_FREERUN_CW_H+1), HIGHBYTE((MS_U16)(u32FreerunClk >> 16)));
        HAL_WriteRegBit(REG_UPDATE_DC1_CW, 1, UPDATE_DC1_FREERUN_CW);
        HAL_WriteRegBit(REG_UPDATE_DC1_CW, 0, UPDATE_DC1_FREERUN_CW);
    }
    else if (_bSubEnable60P)
    {
        //Set DC1 Timing
        MS_U32 u32FrameRate = (MS_U32)ptiming->u16ExpFrameRate;
        MS_U32 u32VSize = 1024;
        MS_U32 u32HSize = ((86400000 / u32FrameRate) * 1000) / u32VSize;

        MS_ASSERT(u32HSize <= 4096);

        HAL_MVOP_SubSetFrequency(HAL_MVOP_SubGetMaxFreerunClk());

        HAL_WriteRegBit(SUB_REG(VOP_REG_FRAME_RST), 0, BIT15); // reg_frame_rst = 0

        HAL_WriteRegBit(SUB_REG(VOP_CTRL0), DISABLE, VOP_FSYNC_EN); // frame sync disable
    }
    else
    {
        HAL_MVOP_SubSetFrequency(HALMVOP_SYNCMODE);
        HAL_WriteByte((REG_DC1_NUM  ), LOWBYTE( ptiming->u16Num));
        HAL_WriteByte((REG_DC1_NUM+1), HIGHBYTE(ptiming->u16Num));
        HAL_WriteByte((REG_DC1_DEN  ), LOWBYTE( ptiming->u16Den));
        HAL_WriteByte((REG_DC1_DEN+1), HIGHBYTE(ptiming->u16Den));
        HAL_WriteRegBit(REG_UPDATE_DC1_CW, 1, UPDATE_DC1_SYNC_CW);
        HAL_WriteRegBit(REG_UPDATE_DC1_CW, 0, UPDATE_DC1_SYNC_CW);
    }
}


void HAL_MVOP_SubSetMonoMode(MS_BOOL bEnable)
{
    if(bEnable)
    {
        HAL_WriteByte(SUB_REG(VOP_U_PAT  ), 0x80);
        HAL_WriteByte(SUB_REG(VOP_U_PAT+1), 0x80);

        HAL_WriteRegBit(SUB_REG(VOP_INPUT_SWITCH), 1, BIT1);    // Mono mode enable
    }
    else
    {
        HAL_WriteRegBit(SUB_REG(VOP_INPUT_SWITCH), 0, BIT1);    //Mono mode disable
    }
}

/******************************************************************************/
/// Set MVOP for H264  Hardwire Mode
/******************************************************************************/
void HAL_MVOP_SubSetH264HardwireMode(void)
{
    // Hardwire mode
    HAL_WriteByte(SUB_REG(VOP_MPG_JPG_SWITCH), 0x00);

    HAL_WriteRegBit(SUB_REG(VOP_REG_STRIP_ALIGN), 0, BIT0);

    //16*32 tile format
    HAL_WriteRegBit(SUB_REG(VOP_REG_WR), 1, BIT1);

    // SVD mode enable
    HAL_WriteByteMask(SUB_REG(VOP_INPUT_SWITCH), BIT3, BIT2|BIT3);


    // set mvop to 64bit interface
    HAL_WriteByteMask(SUB_REG(VOP_MIU_IF), VOP_MIU_128BIT, VOP_MIU_128BIT|VOP_MIU_128B_I64);

    // Write trigger
    HAL_MVOP_SubLoadReg();
}

void HAL_MVOP_SubEnableMVDInterface(MS_BOOL bEnable)
{
    MS_BOOL bMVOPMain2MVD = TRUE;
    bMVOPMain2MVD = (bEnable) ? FALSE : TRUE;

    //This bit is only valid in main mvop bank.
    //Select which mvop interrupt that mvd f/w recieve: 1 for main; 0 for sub.
    HAL_WriteByteMask(VOP_INPUT_SWITCH, bMVOPMain2MVD, VOP_MVD_EN);

    //No need to "Write trigger" since HAL_MVOP_SubSetInputMode() will do it later.
    //HAL_MVOP_SubLoadReg();
}

/******************************************************************************/
/// Set MVOP for RM  Hardwire Mode
/******************************************************************************/
void HAL_MVOP_SubSetRMHardwireMode(void)
{
    HAL_MVOP_SubSetH264HardwireMode();
}

/******************************************************************************/
/// Set MVOP for JPEG Hardwire Mode
/******************************************************************************/
void HAL_MVOP_SubSetJpegHardwireMode(void)
{
    MS_U8 regval = 0x00;

    regval |= 0x80; // packmode
    regval |= 0x20; // Dram Rd Contd
    HAL_WriteByte(SUB_REG(VOP_MPG_JPG_SWITCH), regval);

    // Write trigger
    HAL_MVOP_SubLoadReg();
}

///Enable 3D L/R dual buffer mode
MS_BOOL HAL_MVOP_SubEnable3DLR(MS_BOOL bEnable)
{
    HAL_WriteRegBit(SUB_REG(VOP_MULTI_WIN_CFG0), bEnable, VOP_LR_BUF_MODE);
    bSub3DLRMode = bEnable;
    return TRUE;
}

///Get if 3D L/R mode is enabled
MS_BOOL HAL_MVOP_SubGet3DLRMode(void)
{
    return bSub3DLRMode;
}

MS_BOOL HAL_MVOP_SubGetTimingInfoFromRegisters(MVOP_TimingInfo_FromRegisters *pMvopTimingInfo)
{
    if(NULL == pMvopTimingInfo)
    {
        printf("HAL_MVOP_SubGetTimingInfoFromRegisters():pMvopTimingInfo is NULL\n");
        return FALSE;
    }
    if(HAL_MVOP_SubGetEnableState() == FALSE)
    {
        printf("MVOP is not enabled!\n");
        pMvopTimingInfo->bEnabled = FALSE;
        return FALSE;
    }
    pMvopTimingInfo->bEnabled = TRUE;
    pMvopTimingInfo->u16H_TotalCount = (HAL_ReadByte(SUB_REG(VOP_FRAME_HCOUNT + 1)<< 8)) | (HAL_ReadByte(SUB_REG(VOP_FRAME_HCOUNT)));
    pMvopTimingInfo->u16V_TotalCount = (HAL_ReadByte(SUB_REG(VOP_FRAME_VCOUNT + 1)<< 8)) | (HAL_ReadByte(SUB_REG(VOP_FRAME_VCOUNT)));
    pMvopTimingInfo->u16VBlank0_Start = (HAL_ReadByte(SUB_REG(VOP_VB0_STR + 1)<< 8)) | (HAL_ReadByte(SUB_REG(VOP_VB0_STR)));
    pMvopTimingInfo->u16VBlank0_End = (HAL_ReadByte(SUB_REG(VOP_VB0_END + 1)<< 8)) | (HAL_ReadByte(SUB_REG(VOP_VB0_END)));
    pMvopTimingInfo->u16VBlank1_Start = (HAL_ReadByte(SUB_REG(VOP_VB1_STR + 1)<< 8)) | (HAL_ReadByte(SUB_REG(VOP_VB1_STR)));
    pMvopTimingInfo->u16VBlank1_End = (HAL_ReadByte(SUB_REG(VOP_VB1_END + 1)<< 8)) | (HAL_ReadByte(SUB_REG(VOP_VB1_END)));
    pMvopTimingInfo->u16TopField_Start = (HAL_ReadByte(SUB_REG(VOP_TF_STR + 1)<< 8)) | (HAL_ReadByte(SUB_REG(VOP_TF_STR)));
    pMvopTimingInfo->u16BottomField_Start = (HAL_ReadByte(SUB_REG(VOP_BF_STR + 1)<< 8)) | (HAL_ReadByte(SUB_REG(VOP_BF_STR)));
    pMvopTimingInfo->u16HActive_Start = (HAL_ReadByte(SUB_REG(VOP_HACT_STR + 1)<< 8)) | (HAL_ReadByte(SUB_REG(VOP_HACT_STR)));
    pMvopTimingInfo->u16TopField_VS = (HAL_ReadByte(SUB_REG(VOP_TF_VS + 1)<< 8)) | (HAL_ReadByte(SUB_REG(VOP_TF_VS)));
    pMvopTimingInfo->u16BottomField_VS = (HAL_ReadByte(SUB_REG(VOP_BF_VS + 1)<< 8)) | (HAL_ReadByte(SUB_REG(VOP_BF_VS)));
    pMvopTimingInfo->bInterlace = (HAL_ReadRegBit(SUB_REG(VOP_CTRL0), BIT7) == BIT7);
    return TRUE;
}

void HAL_MVOP_SubSetYUVBaseAdd(MS_U32 u32YOffset, MS_U32 u32UVOffset, MS_BOOL bProgressive, MS_BOOL b422pack)
{
    MS_U32 u32tmp = 0;
    // Y offset
    u32tmp = u32YOffset >> 3;
    if ( !bProgressive )
    {   //Refine Y offset for interlace repeat bottom field
        if (E_MVOP_RPTFLD_BOT == eSubRepeatField)
        {
            MVOP_DBG("%d eRepeatField(%x) is bottom!\n", __LINE__, eSubRepeatField);
            u32tmp += 2;
        }
        else
        {
            MVOP_DBG("%d eRepeatField(%x) is TOP or NONE.\n", __LINE__, eSubRepeatField);
        }
    }
    HAL_WriteByte(SUB_REG(VOP_JPG_YSTR0_L), u32tmp & 0xff);
    HAL_WriteByte(SUB_REG(VOP_JPG_YSTR0_L+1), (u32tmp >> 8) & 0xff);
    HAL_WriteByte(SUB_REG(VOP_JPG_YSTR0_L+2), (u32tmp >> 16) & 0xff);
    HAL_WriteByte(SUB_REG(VOP_JPG_YSTR0_L+3), (u32tmp >> 24) & VOP_YUV_STR_HIBITS);

    if (!bProgressive )
    {   //Y offset of bottom field if interlace
        HAL_WriteByte(SUB_REG(VOP_JPG_YSTR1_L), u32tmp & 0xff);
        HAL_WriteByte(SUB_REG(VOP_JPG_YSTR1_L+1), (u32tmp >> 8) & 0xff);
        HAL_WriteByte(SUB_REG(VOP_JPG_YSTR1_L+2), (u32tmp >> 16) & 0xff);
        HAL_WriteByte(SUB_REG(VOP_JPG_YSTR1_L+3), (u32tmp >> 24) & VOP_YUV_STR_HIBITS);
    }

    if (b422pack)
    {
        if (HAL_ReadRegBit(SUB_REG(VOP_MIU_IF), VOP_MIU_128B_I64) != VOP_MIU_128B_I64) //128-bit
        {
            u32UVOffset = u32YOffset + 16; //add 16 for 128bit; add 8 for 64bit
        }
        else    //64-bit
        {
            u32UVOffset = u32YOffset + 8; //add 16 for 128bit; add 8 for 64bit
        }
    }
    // UV offset
    u32tmp = u32UVOffset >> 3;
    if( !bProgressive )
    {  //Refine UV offset for interlace repeat bottom field
        if (E_MVOP_RPTFLD_BOT == eSubRepeatField)
        {
            MVOP_DBG("%d eRepeatField(%x) is bottom!\n", __LINE__, eSubRepeatField);
            u32tmp += 2;
        }
        else
        {
            MVOP_DBG("%d eRepeatField(%x) is TOP/NONE.\n", __LINE__, eSubRepeatField);
        }
    }
    HAL_WriteByte(SUB_REG(VOP_JPG_UVSTR0_L), u32tmp & 0xff);
    HAL_WriteByte(SUB_REG(VOP_JPG_UVSTR0_L+1), (u32tmp >> 8) & 0xff);
    HAL_WriteByte(SUB_REG(VOP_JPG_UVSTR0_L+2), (u32tmp >> 16) & 0xff);
    HAL_WriteByte(SUB_REG(VOP_JPG_UVSTR0_L+3), (u32tmp >> 24) & VOP_YUV_STR_HIBITS);

    if( !bProgressive )
    {  //UV offset of bottom field if interlace
        HAL_WriteByte(SUB_REG(VOP_JPG_UVSTR1_L), u32tmp & 0xff);
        HAL_WriteByte(SUB_REG(VOP_JPG_UVSTR1_L+1), (u32tmp >> 8) & 0xff);
        HAL_WriteByte(SUB_REG(VOP_JPG_UVSTR1_L+2), (u32tmp >> 16) & 0xff);
        HAL_WriteByte(SUB_REG(VOP_JPG_UVSTR1_L+3), (u32tmp >> 24) & VOP_YUV_STR_HIBITS);
    }

    return;
}

#endif


///////////////////////   3rd MVOP   ////////////////////////
#if 1
void HAL_MVOP_EX_RegSetBase(MS_U32 u32Base)
{
    u32RiuBaseAdd = u32Base;
}

void HAL_MVOP_EX_InitMirrorMode(MVOP_DevID eID, MS_BOOL bMir)
{
    //set bit[3:7] to support mirror mode
    HAL_WriteRegBit(MAP_REG(eID, VOP_MIRROR_CFG), bMir, VOP_MIRROR_CFG_ENABLE);
}

MS_BOOL HAL_MVOP_EX_Init(MVOP_DevID eID)
{
    HAL_WriteByte(MAP_REG(eID, VOP_TST_IMG), 0x40);
    HAL_MVOP_EX_InitMirrorMode(eID, TRUE);
    HAL_MVOP_EX_SetRgbFormat(eID, E_MVOP_RGB_NONE);

    //Enable dynamic clock gating
    HAL_WriteByteMask(MAP_REG(eID, VOP_GCLK), VOP_GCLK_MIU_ON|VOP_GCLK_VCLK_ON, VOP_GCLK_MIU_ON|VOP_GCLK_VCLK_ON);
    return TRUE;
}

void HAL_MVOP_EX_SetVerticalMirrorMode(MVOP_DevID eID, MS_BOOL bEnable)
{
    if (VOP_MIRROR_CFG_ENABLE != HAL_ReadRegBit(MAP_REG(eID, VOP_MIRROR_CFG), VOP_MIRROR_CFG_ENABLE))
    {
        //printf("Setup mirror mode\n");
        HAL_MVOP_EX_InitMirrorMode(eID, TRUE);
    }

    HAL_WriteRegBit(MAP_REG(eID, VOP_MIRROR_CFG), bEnable, VOP_MIRROR_CFG_VEN);
    bExMirrorModeVer[eID] = bEnable;

}

void HAL_MVOP_EX_SetHorizontallMirrorMode(MVOP_DevID eID, MS_BOOL bEnable)
{
    if (VOP_MIRROR_CFG_ENABLE != HAL_ReadRegBit(MAP_REG(eID, VOP_MIRROR_CFG), VOP_MIRROR_CFG_ENABLE))
    {
        //printf("Setup mirror mode\n");
        HAL_MVOP_EX_InitMirrorMode(eID, TRUE);
    }

    HAL_WriteRegBit(MAP_REG(eID, VOP_MIRROR_CFG), bEnable, VOP_MIRROR_CFG_HEN);
    bExMirrorModeHor[eID] = bEnable;

}

void HAL_MVOP_EX_SetFieldInverse(MVOP_DevID eID, MS_BOOL b2MVD, MS_BOOL b2IP)
{
    // Set fld inv & ofld_inv
    HAL_WriteRegBit(MAP_REG(eID, VOP_CTRL0+1), b2MVD, BIT3); //inverse the field to MVD
    HAL_WriteRegBit(MAP_REG(eID, VOP_CTRL0+1), b2IP, BIT4);  //inverse the field to IP
}

void HAL_MVOP_EX_SetChromaWeighting(MVOP_DevID eID, MS_BOOL bEnable)
{
    HAL_WriteRegBit(MAP_REG(eID, VOP_REG_WEIGHT_CTRL), bEnable, BIT1);
}

//load new value into active registers 0x20-0x26
void HAL_MVOP_EX_LoadReg(MVOP_DevID eID)
{
    HAL_WriteRegBit(MAP_REG(eID, VOP_REG_WR), 1, BIT0);
    HAL_WriteRegBit(MAP_REG(eID, VOP_REG_WR), 0, BIT0);
}

void HAL_MVOP_EX_SetMIUReqMask(MVOP_DevID eID, MS_BOOL bEnable)
{
    //Do nothing because Agate Sub and 3rd MVOP share one miu client.
    return;
}

void HAL_MVOP_EX_Rst(MVOP_DevID eID)
{
    HAL_WriteRegBit(MAP_REG(eID, VOP_CTRL0), 0, BIT0);
    HAL_WriteRegBit(MAP_REG(eID, VOP_CTRL0), 1, BIT0);
}

void HAL_MVOP_EX_Enable(MVOP_DevID eID, MS_BOOL bEnable)
{
    MS_U8 regval;

    regval = HAL_ReadByte(MAP_REG(eID, VOP_CTRL0));

    if ( bEnable )
    {
        regval |= 0x1;
    }
    else
    {
        regval &= ~0x1;
    }

    HAL_WriteByte(MAP_REG(eID, VOP_CTRL0), regval);
}

MS_BOOL HAL_MVOP_EX_GetEnableState(MVOP_DevID eID)
{
    return (HAL_ReadRegBit(MAP_REG(eID, VOP_CTRL0), BIT0));
}

HALMVOPFREQUENCY HAL_MVOP_EX_GetMaxFreerunClk(MVOP_DevID eID)
{
    return HALMVOP_160MHZ;
}

HALMVOPFREQUENCY HAL_MVOP_EX_GetClk(MS_U32 u32PixClk)
{
    HALMVOPFREQUENCY eClkFreq = HALMVOP_86MHZ;
    if (u32PixClk > HALMVOP_86MHZ)
    {
        if (u32PixClk <= HALMVOP_144MHZ)
        {
            eClkFreq = HALMVOP_144MHZ;
        }
        else if (u32PixClk <= HALMVOP_160MHZ)
        {
            eClkFreq = HALMVOP_160MHZ;
        }
        else
        {
            printf("%s u32PixClk(%ld) > 160MHz!\n", __FUNCTION__, u32PixClk);
            eClkFreq = HALMVOP_160MHZ;
        }
    }
    else if (u32PixClk < HALMVOP_86MHZ)
    {
        if (u32PixClk <= HALMVOP_27MHZ)
        {
            eClkFreq = HALMVOP_27MHZ;
        }
        else if (u32PixClk <= HALMVOP_54MHZ)
        {
            eClkFreq = HALMVOP_54MHZ;
        }
    }
    MVOP_DBG("%s u32PixClk:%ld, eClkFreq:%d\n", __FUNCTION__, u32PixClk, eClkFreq);
    return eClkFreq;
}

void HAL_MVOP_EX_SetFrequency(MVOP_DevID eID, HALMVOPFREQUENCY enFrequency)
{
    // clear
    HAL_WriteByteMask(REG_CKG_DC_F3, 0, CKG_DC_F3_MASK);
    switch(enFrequency)
    {
        case HALMVOP_86MHZ: //86.4MHz
            HAL_WriteByteMask(REG_CKG_DC_F3, CKG_DC_F3_86MHZ, CKG_DC_F3_MASK);
            break;
        case HALMVOP_160MHZ:
            HAL_WriteByteMask(REG_CKG_DC_F3, CKG_DC_F3_160MHZ, CKG_DC_F3_MASK);
            break;
        case HALMVOP_54MHZ:
            HAL_WriteByteMask(REG_CKG_DC_F3, CKG_DC_F3_54MHZ, CKG_DC_F3_MASK);
            break;
        case HALMVOP_27MHZ:
            HAL_WriteByteMask(REG_CKG_DC_F3, CKG_DC_F3_27MHZ, CKG_DC_F3_MASK);
            break;
        default:
            HAL_WriteByteMask(REG_CKG_DC_F3, CKG_DC_F3_86MHZ, CKG_DC_F3_MASK);
            printf("Attention! In HAL_MVOP_EX_SetFrequency default path! enFrequency=%d\n", enFrequency);
            break;
    }
}

void HAL_MVOP_EX_SetOutputInterlace(MVOP_DevID eID, MS_BOOL bEnable)
{
    MS_U8 regval;

    regval = HAL_ReadByte(MAP_REG(eID, VOP_CTRL0));

    if ( bEnable )
    {
        regval |= 0x80;
    }
    else
    {
        regval &= ~0x80;
    }

    HAL_WriteByte(MAP_REG(eID, VOP_CTRL0), regval);
}

void HAL_MVOP_EX_SetPattern(MVOP_DevID eID, MVOP_Pattern enMVOPPattern)
{
    HAL_WriteByteMask(MAP_REG(eID, VOP_TST_IMG), enMVOPPattern, BIT2 | BIT1 | BIT0);
}

MS_BOOL HAL_MVOP_EX_SetTileFormat(MVOP_DevID eID, MVOP_TileFormat eTileFmt)
{
    if (eTileFmt == E_MVOP_TILE_8x32)
    {
        HAL_WriteRegBit(MAP_REG(eID, VOP_REG_WR), 0, BIT1);
        return TRUE;
    }
    else if (eTileFmt == E_MVOP_TILE_16x32)
    {
        HAL_WriteRegBit(MAP_REG(eID, VOP_REG_WR), 1, BIT1);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

MS_BOOL HAL_MVOP_EX_SetRgbFormat(MVOP_DevID eID, MVOP_RgbFormat eRgbFmt)
{
    MS_BOOL bRet = FALSE;
    MS_U8 u8Val = 0;

    if (eRgbFmt == E_MVOP_RGB_NONE)
    {
        u8Val = 0;
        bRet = TRUE;
    }
    else if (eRgbFmt == E_MVOP_RGB_565)
    {
        u8Val = VOP_RGB_FMT_565;
        bRet = TRUE;
    }
    else if (eRgbFmt == E_MVOP_RGB_888)
    {
        u8Val = VOP_RGB_FMT_888;
        bRet = TRUE;
    }

    if (bRet == TRUE)
    {
        HAL_WriteByteMask(MAP_REG(eID, VOP_RGB_FMT), u8Val, VOP_RGB_FMT_SEL);
        eExRgbFmt[eID] = eRgbFmt;
    }
    return bRet;
}

void HAL_MVOP_EX_SetBlackBG(MVOP_DevID eID)
{
    MS_U8 regval;

    //set MVOP test pattern to black
    HAL_WriteByte(MAP_REG(eID, VOP_TST_IMG + 1), 0x10);
    HAL_WriteByte(MAP_REG(eID, VOP_U_PAT      ), 0x80);
    HAL_WriteByte(MAP_REG(eID, VOP_U_PAT   + 1), 0x80);

    regval = HAL_ReadByte(MAP_REG(eID, VOP_TST_IMG));
    HAL_WriteByte(MAP_REG(eID, VOP_TST_IMG), 0x02);
    HAL_WriteByte(MAP_REG(eID, VOP_TST_IMG), 0x00);
    HAL_WriteByte(MAP_REG(eID, VOP_TST_IMG), regval);
}

void HAL_MVOP_EX_SetCropWindow(MVOP_DevID eID, MVOP_InputCfg *pparam)
{
#if 1
    pparam = pparam;
#else // enable it when test code is ready
    MS_U32 x, y;
    MS_U32 u32offset;

    if(!pparam)
    {
        return;
    }
    //set MVOP test pattern to black
    HAL_MVOP_EX_SetBlackBG();
#if 0
    if((pparam->enVideoType == MVOP_H264) && (pparam->u16StripSize == 1920))
    {
        pparam->u16StripSize = 1952;
    }
#endif
    if((pparam->enVideoType == MVOP_MPG) || (pparam->enVideoType == MVOP_MPEG4))
    {
        pparam->u16CropX = (pparam->u16CropX >> 3) << 3; // 8 bytes align
        pparam->u16CropY = (pparam->u16CropY >> 6) << 6; // 64 bytes align
    }
    else if((pparam->enVideoType == MVOP_H264) || (pparam->enVideoType == MVOP_RM))
    {
        pparam->u16CropX = (pparam->u16CropX >> 4) << 4; // 16 bytes align
        pparam->u16CropY = (pparam->u16CropY >> 6) << 6; // 64 bytes align
    }
    else
    {
        MS_ASSERT(0);
    }

    x = (MS_U32)pparam->u16CropX;
    y = (MS_U32)pparam->u16CropY;

    // y offset
    u32offset = ((y * pparam->u16StripSize + (x << 5)) >> 3);
    HAL_WriteByte(MAP_REG(eID, VOP_JPG_YSTR0_L    ), (MS_U8)(u32offset));
    HAL_WriteByte(MAP_REG(eID, VOP_JPG_YSTR0_L + 1), (MS_U8)(u32offset >> 8));
    HAL_WriteByte(MAP_REG(eID, VOP_JPG_YSTR0_H    ), (MS_U8)(u32offset >> 16));
    HAL_WriteByte(MAP_REG(eID, VOP_JPG_YSTR0_H + 1), (MS_U8)((u32offset >> 24) & VOP_YUV_STR_HIBITS));

    // uv offset
    u32offset = ((y >> 1) * pparam->u16StripSize + (x << 5)) >> 3;
    HAL_WriteByte(MAP_REG(eID, VOP_JPG_UVSTR0_L    ), (MS_U8)(u32offset));
    HAL_WriteByte(MAP_REG(eID, VOP_JPG_UVSTR0_L + 1), (MS_U8)(u32offset >> 8));
    HAL_WriteByte(MAP_REG(eID, VOP_JPG_UVSTR0_H    ), (MS_U8)(u32offset >> 16));
    HAL_WriteByte(MAP_REG(eID, VOP_JPG_UVSTR0_H + 1), (MS_U8)((u32offset >> 24) & VOP_YUV_STR_HIBITS));

    pparam->u16CropWidth= (pparam->u16CropWidth >> 3) << 3;
    // HSize, VSize
    HAL_WriteByte(MAP_REG(eID, VOP_JPG_HSIZE    ), LOWBYTE(pparam->u16CropWidth ));
    HAL_WriteByte(MAP_REG(eID, VOP_JPG_HSIZE + 1), HIGHBYTE(pparam->u16CropWidth ));
    HAL_WriteByte(MAP_REG(eID, VOP_JPG_VSIZE    ), LOWBYTE(pparam->u16CropHeight));
    HAL_WriteByte(MAP_REG(eID, VOP_JPG_VSIZE + 1), HIGHBYTE(pparam->u16CropHeight ));

    HAL_WriteByteMask(MAP_REG(eID, VOP_MPG_JPG_SWITCH), BIT0, BIT1|BIT0);

    // clear extend strip len bit by default
    HAL_WriteRegBit(MAP_REG(eID, VOP_REG_STRIP_ALIGN), 0, BIT0);
    if((pparam->enVideoType == MVOP_MPG) || (pparam->enVideoType == MVOP_MPEG4))
    {
        // Disable H264 or RM Input
        HAL_WriteRegBit(MAP_REG(eID, VOP_INPUT_SWITCH), 0, BIT2|BIT3);
        //8*32 tile format
        HAL_WriteRegBit(MAP_REG(eID, VOP_REG_WR), 0, BIT1);
    }
    else if((pparam->enVideoType == MVOP_H264) || (pparam->enVideoType == MVOP_RM))
    {
        //16*32 tile format
        HAL_WriteRegBit(MAP_REG(eID, VOP_REG_WR), 1, BIT1);
        // SVD mode enable
        HAL_WriteByteMask(MAP_REG(eID, VOP_INPUT_SWITCH), BIT3, BIT2|BIT3);
        // set mvop to 128bit_i128 interface
        HAL_WriteByteMask(MAP_REG(eID, VOP_MIU_IF), VOP_MIU_128BIT, VOP_MIU_128BIT|VOP_MIU_128B_I64);
    }
    HAL_MVOP_EX_LoadReg();
#endif
}

void HAL_MVOP_EX_SetInputMode(MVOP_DevID eID, VOPINPUTMODE mode, MVOP_InputCfg *pparam )
{
    MS_U32 u32tmp;
    MS_U8 regval;
    MS_U16 u16strip;

    //set MVOP test pattern to black
    HAL_MVOP_EX_SetBlackBG(eID);

    //regval = HAL_ReadByte(MAP_REG(eID, VOP_MPG_JPG_SWITCH);
    regval = 0;
    regval |= ( mode & 0x3 );

    // clear extend strip len bit by default
    HAL_WriteRegBit(MAP_REG(eID, VOP_REG_STRIP_ALIGN), 0, BIT0);

    // set mvop to 128bit_i128 interface
    HAL_WriteByteMask(MAP_REG(eID, VOP_MIU_IF), VOP_MIU_128BIT, VOP_MIU_128BIT|VOP_MIU_128B_I64);

    // Disable H264 or RM Input
    HAL_WriteRegBit(MAP_REG(eID, VOP_INPUT_SWITCH), 0, BIT2|BIT3);
    //8*32 tile format
    HAL_WriteRegBit(MAP_REG(eID, VOP_REG_WR), 0, BIT1);

    if ( mode == VOPINPUT_HARDWIRE )
    {
        HAL_WriteByte(MAP_REG(eID, VOP_MPG_JPG_SWITCH), regval);
    }
    else if ( mode == VOPINPUT_HARDWIRECLIP )
    {
        HAL_WriteByte(MAP_REG(eID, VOP_MPG_JPG_SWITCH), regval);

        // HSize, VSize
        HAL_WriteByte(MAP_REG(eID, VOP_JPG_HSIZE    ), LOWBYTE( pparam->u16HSize ));
        HAL_WriteByte(MAP_REG(eID, VOP_JPG_HSIZE + 1), HIGHBYTE( pparam->u16HSize ));
        HAL_WriteByte(MAP_REG(eID, VOP_JPG_VSIZE    ), LOWBYTE( pparam->u16VSize ));
        HAL_WriteByte(MAP_REG(eID, VOP_JPG_VSIZE + 1), HIGHBYTE( pparam->u16VSize ));
    }
    else if (mode == VOPINPUT_MCUCTRL)
    {
        bExMirrorMode[eID] = (bExMirrorModeVer[eID] || bExMirrorModeHor[eID]);
        if ( pparam->bProgressive )
            regval |= 0x4;
        else
        {
            regval &= ~0x4;
            regval |= 0x1;  //reg_dc_md=b'11 for interlace input
        }

        if ( pparam->bYUV422 )
            regval |= 0x10;
        else
            regval &= ~0x10;

        if ( pparam->b422pack )
            regval |= 0x80;

        if ( pparam->bDramRdContd == 1 )
            regval |= 0x20;
        else
            regval &= ~0x20;

        // for backward compatable to saturn
        // [3] UV-7bit mode don't care
        // [5] dram_rd_md =0
        // [6] Fld don't care
        // [7] 422pack don'care
        HAL_WriteByte(MAP_REG(eID, VOP_MPG_JPG_SWITCH), regval);

        if (pparam->u16StripSize == 0)
        {
            if (pparam->bSD)
                u16strip = 720;
            else
                u16strip = 1920;
        }
        else
        {
            u16strip = pparam->u16StripSize;
        }

        // set dc_strip[7:0]
        if (pparam->bDramRdContd == 0)
        {
            u16strip = u16strip/8;
        }
        else
        {
            if ( pparam->b422pack )
            {
                if ((u16strip < 1024) || bExMirrorMode[eID])
                {
                    u16strip = u16strip/4;
                    // dont extend strip len
                    HAL_WriteRegBit(MAP_REG(eID, VOP_REG_STRIP_ALIGN), 0, BIT0);
                }
                else
                {
                    u16strip = u16strip/8;
                    // extend strip len to 2048
                    HAL_WriteRegBit(MAP_REG(eID, VOP_REG_STRIP_ALIGN), 1, BIT0);
                }
            }
            else
            {
                u16strip = u16strip/8;
            }
        }

        if (u16strip >= 256 )
        {
            HAL_WriteByte(MAP_REG(eID, VOP_DC_STRIP_H), (u16strip>>8));
            //reg_dc_strip_h[2:0] = reg_dc_strip[10:8]
        }
        else
        {
            HAL_WriteByteMask(MAP_REG(eID, VOP_DC_STRIP_H), 0, BIT0 | BIT1 | BIT2);
        }

        regval = (MS_U8)u16strip;
        HAL_WriteByte(MAP_REG(eID, VOP_DC_STRIP), regval);

        // Y offset
        u32tmp = pparam->u32YOffset >> 3;
        HAL_WriteByte(MAP_REG(eID, VOP_JPG_YSTR0_L), u32tmp & 0xff);
        HAL_WriteByte(MAP_REG(eID, VOP_JPG_YSTR0_L+1), (u32tmp >> 8) & 0xff);
        HAL_WriteByte(MAP_REG(eID, VOP_JPG_YSTR0_L+2), (u32tmp >> 16) & 0xff);
        HAL_WriteByte(MAP_REG(eID, VOP_JPG_YSTR0_L+3), (u32tmp >> 24) & VOP_YUV_STR_HIBITS);

        if( !pparam->bProgressive )
        {  //Y offset of bottom field if interlace
                u32tmp += 1;
            HAL_WriteByte(MAP_REG(eID, VOP_JPG_YSTR1_L), u32tmp & 0xff);
            HAL_WriteByte(MAP_REG(eID, VOP_JPG_YSTR1_L+1), (u32tmp >> 8) & 0xff);
            HAL_WriteByte(MAP_REG(eID, VOP_JPG_YSTR1_L+2), (u32tmp >> 16) & 0xff);
            HAL_WriteByte(MAP_REG(eID, VOP_JPG_YSTR1_L+3), (u32tmp >> 24) & VOP_YUV_STR_HIBITS);
        }

        if (pparam->b422pack)
        {

            if (HAL_ReadRegBit(MAP_REG(eID, VOP_MIU_IF), VOP_MIU_128B_I64) != VOP_MIU_128B_I64) //128-bit
            {
                pparam->u32UVOffset = pparam->u32YOffset + 16;
            }
            else    //64-bit
            {
                pparam->u32UVOffset = pparam->u32YOffset + 8;
            }
        }
        // UV offset
        u32tmp = pparam->u32UVOffset >> 3;
        //printf("YOffset=0x%lx, u32tmp=0x%lx\n", pparam->u32YOffset, u32tmp);
        HAL_WriteByte(MAP_REG(eID, VOP_JPG_UVSTR0_L), u32tmp & 0xff);
        HAL_WriteByte(MAP_REG(eID, VOP_JPG_UVSTR0_L+1), (u32tmp >> 8) & 0xff);
        HAL_WriteByte(MAP_REG(eID, VOP_JPG_UVSTR0_L+2), (u32tmp >> 16) & 0xff);
        HAL_WriteByte(MAP_REG(eID, VOP_JPG_UVSTR0_L+3), (u32tmp >> 24) & VOP_YUV_STR_HIBITS);

        if (!pparam->bProgressive)
        {  //UV offset of bottom field if interlace
                u32tmp += 1;
            HAL_WriteByte(MAP_REG(eID, VOP_JPG_UVSTR1_L), u32tmp & 0xff);
            HAL_WriteByte(MAP_REG(eID, VOP_JPG_UVSTR1_L+1), (u32tmp >> 8) & 0xff);
            HAL_WriteByte(MAP_REG(eID, VOP_JPG_UVSTR1_L+2), (u32tmp >> 16) & 0xff);
            HAL_WriteByte(MAP_REG(eID, VOP_JPG_UVSTR1_L+3), (u32tmp >> 24) & VOP_YUV_STR_HIBITS);
        }

        // HSize, VSize
        HAL_WriteByte(MAP_REG(eID, VOP_JPG_HSIZE    ), LOWBYTE( pparam->u16HSize ));
        HAL_WriteByte(MAP_REG(eID, VOP_JPG_HSIZE + 1), HIGHBYTE( pparam->u16HSize ));
        HAL_WriteByte(MAP_REG(eID, VOP_JPG_VSIZE    ), LOWBYTE( pparam->u16VSize ));
        HAL_WriteByte(MAP_REG(eID, VOP_JPG_VSIZE + 1), HIGHBYTE( pparam->u16VSize ));
    }

    HAL_MVOP_EX_LoadReg(eID);
}


void HAL_MVOP_EX_EnableUVShift(MVOP_DevID eID, MS_BOOL bEnable)
{
    MS_U8 regval;

    regval = HAL_ReadByte(MAP_REG(eID, VOP_MPG_JPG_SWITCH));

    if (((regval & BIT4) == BIT4) && ((regval & 0x3)== 0x2))
    {   // 422 with MCU control mode
        if (bEnable)
        {
            MS_ASSERT(0);
        }
    }

    // output 420 and interlace
    //[IP - Sheet] : Main Page --- 420CUP
    //[Project] :  Titania2
    //[Description]:   Chroma artifacts when 420to422 is applied duplicate method.
    //[Root cause]: Apply 420to422 average algorithm to all DTV input cases.
    //The average algorithm must cooperate with MVOP.
    HAL_WriteByteMask(MAP_REG(eID, VOP_UV_SHIFT), (bEnable)?1:0, 0x3);
}


void HAL_MVOP_EX_SetEnable60P(MVOP_DevID eID, MS_BOOL bEnable)
{
    bExEnable60P[eID] = bEnable;
}

MVOP_Result HAL_MVOP_EX_EnableFreerunMode(MVOP_DevID eID, MS_BOOL bEnable)
{
    //Agate 3rd mvop clock source only has fixed clocks: 160M, 86.4M, 54M and 27M.
    //No freerun mode can be enabled for the 3rd mvop.
    if (TRUE == bEnable)
    {
        printf("%s unsupported!\n", __FUNCTION__);
        bEnable = FALSE;
    }
    bExEnableFreerunMode[eID] = bEnable;
    return E_MVOP_UNSUPPORTED;
}

void HAL_MVOP_EX_SetOutputTiming(MVOP_DevID eID, MVOP_Timing *ptiming )
{
    MS_U8 regval;

    HAL_WriteByte(MAP_REG(eID, VOP_FRAME_VCOUNT    ), LOWBYTE( ptiming->u16V_TotalCount ));
    HAL_WriteByte(MAP_REG(eID, VOP_FRAME_VCOUNT + 1), HIGHBYTE( ptiming->u16V_TotalCount ));
    HAL_WriteByte(MAP_REG(eID, VOP_FRAME_HCOUNT    ), LOWBYTE( ptiming->u16H_TotalCount ));
    HAL_WriteByte(MAP_REG(eID, VOP_FRAME_HCOUNT + 1), HIGHBYTE( ptiming->u16H_TotalCount ));

    HAL_WriteByte(MAP_REG(eID, VOP_VB0_STR     ), LOWBYTE( ptiming->u16VBlank0_Start ));
    HAL_WriteByte(MAP_REG(eID, VOP_VB0_STR  + 1), HIGHBYTE( ptiming->u16VBlank0_Start ));
    HAL_WriteByte(MAP_REG(eID, VOP_VB0_END     ), LOWBYTE( ptiming->u16VBlank0_End ));
    HAL_WriteByte(MAP_REG(eID, VOP_VB0_END  + 1), HIGHBYTE( ptiming->u16VBlank0_End ));
    HAL_WriteByte(MAP_REG(eID, VOP_VB1_STR     ), LOWBYTE( ptiming->u16VBlank1_Start ));
    HAL_WriteByte(MAP_REG(eID, VOP_VB1_STR  + 1), HIGHBYTE( ptiming->u16VBlank1_Start ));
    HAL_WriteByte(MAP_REG(eID, VOP_VB1_END     ), LOWBYTE( ptiming->u16VBlank1_End ));
    HAL_WriteByte(MAP_REG(eID, VOP_VB1_END  + 1), HIGHBYTE( ptiming->u16VBlank1_End ));
    HAL_WriteByte(MAP_REG(eID, VOP_TF_STR      ), LOWBYTE( ptiming->u16TopField_Start ));
    HAL_WriteByte(MAP_REG(eID, VOP_TF_STR   + 1), HIGHBYTE( ptiming->u16TopField_Start ));
    HAL_WriteByte(MAP_REG(eID, VOP_BF_STR      ), LOWBYTE( ptiming->u16BottomField_Start ));
    HAL_WriteByte(MAP_REG(eID, VOP_BF_STR   + 1), HIGHBYTE( ptiming->u16BottomField_Start ));
    HAL_WriteByte(MAP_REG(eID, VOP_HACT_STR    ), LOWBYTE( ptiming->u16HActive_Start ));
    HAL_WriteByte(MAP_REG(eID, VOP_HACT_STR + 1), HIGHBYTE( ptiming->u16HActive_Start ));

    HAL_WriteByte(MAP_REG(eID, VOP_TF_VS      ), LOWBYTE( ptiming->u16TopField_VS ));
    HAL_WriteByte(MAP_REG(eID, VOP_TF_VS   + 1), HIGHBYTE( ptiming->u16TopField_VS ));
    HAL_WriteByte(MAP_REG(eID, VOP_BF_VS      ), LOWBYTE( ptiming->u16BottomField_VS ));
    HAL_WriteByte(MAP_REG(eID, VOP_BF_VS   + 1), HIGHBYTE( ptiming->u16BottomField_VS ));

    // + S3, set default IMG_HSTR, IMG_VSTR0, IMG_VSTR1
#ifdef _SUPPORT_IMG_OFFSET_
    HAL_WriteByte(MAP_REG(eID, VOP_IMG_HSTR    ), LOWBYTE( ptiming->u16HImg_Start));
    HAL_WriteByte(MAP_REG(eID, VOP_IMG_HSTR + 1), HIGHBYTE( ptiming->u16HImg_Start ));
    HAL_WriteByte(MAP_REG(eID, VOP_IMG_VSTR0   ), LOWBYTE( ptiming->u16VImg_Start0));
    HAL_WriteByte(MAP_REG(eID, VOP_IMG_VSTR0+ 1), HIGHBYTE( ptiming->u16VImg_Start0 ));
    HAL_WriteByte(MAP_REG(eID, VOP_IMG_VSTR1   ), LOWBYTE( ptiming->u16VImg_Start1 ));
    HAL_WriteByte(MAP_REG(eID, VOP_IMG_VSTR1+ 1), HIGHBYTE( ptiming->u16VImg_Start1 ));
#else
    HAL_WriteByte(MAP_REG(eID, VOP_IMG_HSTR    ), LOWBYTE( ptiming->u16HActive_Start ));
    HAL_WriteByte(MAP_REG(eID, VOP_IMG_HSTR + 1), HIGHBYTE( ptiming->u16HActive_Start ));
    HAL_WriteByte(MAP_REG(eID, VOP_IMG_VSTR0   ), LOWBYTE( ptiming->u16VBlank0_End ));
    HAL_WriteByte(MAP_REG(eID, VOP_IMG_VSTR0+ 1), HIGHBYTE( ptiming->u16VBlank0_End ));
    HAL_WriteByte(MAP_REG(eID, VOP_IMG_VSTR1   ), LOWBYTE( ptiming->u16VBlank1_End ));
    HAL_WriteByte(MAP_REG(eID, VOP_IMG_VSTR1+ 1), HIGHBYTE( ptiming->u16VBlank1_End ));
#endif
    // select mvop output from frame color(black)
    HAL_WriteByte(MAP_REG(eID, VOP_TST_IMG + 1), 0x10);
    HAL_WriteByte(MAP_REG(eID, VOP_U_PAT      ), 0x80);
    HAL_WriteByte(MAP_REG(eID, VOP_U_PAT   + 1), 0x80);
    // set mvop src to test pattern
    regval = HAL_ReadByte(MAP_REG(eID, VOP_TST_IMG));
    HAL_WriteByte(MAP_REG(eID, VOP_TST_IMG), 0x02);
    // make changed registers take effect
    HAL_MVOP_EX_LoadReg(eID);

    HAL_MVOP_EX_SetMIUReqMask(eID, TRUE);
    // reset mvop to avoid timing change cause mvop hang-up
    HAL_MVOP_EX_Rst(eID);
    HAL_MVOP_EX_SetMIUReqMask(eID, FALSE);

    // select mvop output from mvd
    HAL_WriteByte(MAP_REG(eID, VOP_TST_IMG), 0x00);
    HAL_WriteByte(MAP_REG(eID, VOP_TST_IMG), regval);
    HAL_WriteRegBit(MAP_REG(eID, VOP_CTRL0), ptiming->bHDuplicate, BIT2);// H pixel duplicate

#if 0
    MVOP_DBG(printf("\nMVOP SetOutputTiming\n");)
    MVOP_DBG(printf(" VTot=%u,\t",ptiming->u16V_TotalCount);)
    MVOP_DBG(printf(" HTot=%u,\t",ptiming->u16H_TotalCount);)
    MVOP_DBG(printf(" I/P=%u\n",ptiming->bInterlace);)
    MVOP_DBG(printf(" W=%u,\t",ptiming->u16Width);)
    MVOP_DBG(printf(" H=%u,\t",ptiming->u16Height);)
    MVOP_DBG(printf(" FRate=%u,\t",ptiming->u8Framerate);)
    MVOP_DBG(printf(" HFreq=%u\n",ptiming->u16H_Freq);)
    MVOP_DBG(printf(" Num=0x%x,\t",ptiming->u16Num);)
    MVOP_DBG(printf(" Den=0x%x,\t",ptiming->u16Den);)
    MVOP_DBG(printf(" u16ExpFRate=%u #\n\n", ptiming->u16ExpFrameRate);)
#endif
}

void HAL_MVOP_EX_SetDCClk(MVOP_DevID eID, MS_U8 clkNum, MS_BOOL bEnable)
{
    if ((clkNum==0) && (E_MVOP_DEV_2==eID))
    {
        HAL_WriteRegBit(REG_CKG_DC_F3, !bEnable, CKG_DC_F3_GATED);
    }
}


void HAL_MVOP_EX_SetSynClk(MVOP_DevID eID, MVOP_Timing *ptiming, HALMVOPFREQUENCY eFreq)
{
    HAL_MVOP_EX_SetFrequency(eID, eFreq);
    return;
}


void HAL_MVOP_EX_SetMonoMode(MVOP_DevID eID, MS_BOOL bEnable)
{
    if(bEnable)
    {
        HAL_WriteByte(MAP_REG(eID, VOP_U_PAT  ), 0x80);
        HAL_WriteByte(MAP_REG(eID, VOP_U_PAT+1), 0x80);

        HAL_WriteRegBit(MAP_REG(eID, VOP_INPUT_SWITCH), 1, BIT1);    // Mono mode enable
    }
    else
    {
        HAL_WriteRegBit(MAP_REG(eID, VOP_INPUT_SWITCH), 0, BIT1);    //Mono mode disable
    }
}

/******************************************************************************/
/// Set MVOP for H264  Hardwire Mode
/******************************************************************************/
void HAL_MVOP_EX_SetH264HardwireMode(MVOP_DevID eID)
{
    // Hardwire mode
    HAL_WriteByte(MAP_REG(eID, VOP_MPG_JPG_SWITCH), 0x00);

    HAL_WriteRegBit(MAP_REG(eID, VOP_REG_STRIP_ALIGN), 0, BIT0);

    //16*32 tile format
    HAL_WriteRegBit(MAP_REG(eID, VOP_REG_WR), 1, BIT1);

    // SVD mode enable
    HAL_WriteByteMask(MAP_REG(eID, VOP_INPUT_SWITCH), BIT3, BIT2|BIT3);


    // set mvop to 64bit interface
    HAL_WriteByteMask(MAP_REG(eID, VOP_MIU_IF), VOP_MIU_128BIT, VOP_MIU_128BIT|VOP_MIU_128B_I64);

    // Write trigger
    HAL_MVOP_EX_LoadReg(eID);
}

void HAL_MVOP_EX_EnableMVDInterface(MVOP_DevID eID, MS_BOOL bEnable)
{
    MS_BOOL bMVOPMain2MVD = TRUE;
    bMVOPMain2MVD = (bEnable) ? FALSE : TRUE;

    //This bit is only valid in main mvop bank.
    //Select which mvop interrupt that mvd f/w recieve: 1 for main; 0 for sub.
    HAL_WriteByteMask(VOP_INPUT_SWITCH, bMVOPMain2MVD, VOP_MVD_EN);

    //No need to "Write trigger" since HAL_MVOP_EX_SetInputMode() will do it later.
    //HAL_MVOP_EX_LoadReg();
}

/******************************************************************************/
/// Set MVOP for JPEG Hardwire Mode
/******************************************************************************/
void HAL_MVOP_EX_SetJpegHardwireMode(MVOP_DevID eID)
{
    MS_U8 regval = 0x00;

    regval |= 0x80; // packmode
    regval |= 0x20; // Dram Rd Contd
    HAL_WriteByte(MAP_REG(eID, VOP_MPG_JPG_SWITCH), regval);

    // Write trigger
    HAL_MVOP_EX_LoadReg(eID);
}

///Enable 3D L/R dual buffer mode
MS_BOOL HAL_MVOP_EX_Enable3DLR(MVOP_DevID eID, MS_BOOL bEnable)
{
    HAL_WriteRegBit(MAP_REG(eID, VOP_MULTI_WIN_CFG0), bEnable, VOP_LR_BUF_MODE);
    bEx3DLRMode[eID] = bEnable;
    return TRUE;
}

///Get if 3D L/R mode is enabled
MS_BOOL HAL_MVOP_EX_Get3DLRMode(MVOP_DevID eID)
{
    return bEx3DLRMode[eID];
}

MS_BOOL HAL_MVOP_EX_GetTimingInfoFromRegisters(MVOP_DevID eID, MVOP_TimingInfo_FromRegisters *pMvopTimingInfo)
{
    if(NULL == pMvopTimingInfo)
    {
        printf("HAL_MVOP_EX_GetTimingInfoFromRegisters():pMvopTimingInfo is NULL\n");
        return FALSE;
    }
    if(HAL_MVOP_EX_GetEnableState(eID) == FALSE)
    {
        printf("MVOP is not enabled!\n");
        pMvopTimingInfo->bEnabled = FALSE;
        return FALSE;
    }
    pMvopTimingInfo->bEnabled = TRUE;
    pMvopTimingInfo->u16H_TotalCount = (HAL_ReadByte(MAP_REG(eID, VOP_FRAME_HCOUNT + 1)<< 8)) | (HAL_ReadByte(MAP_REG(eID, VOP_FRAME_HCOUNT)));
    pMvopTimingInfo->u16V_TotalCount = (HAL_ReadByte(MAP_REG(eID, VOP_FRAME_VCOUNT + 1)<< 8)) | (HAL_ReadByte(MAP_REG(eID, VOP_FRAME_VCOUNT)));
    pMvopTimingInfo->u16VBlank0_Start = (HAL_ReadByte(MAP_REG(eID, VOP_VB0_STR + 1)<< 8)) | (HAL_ReadByte(MAP_REG(eID, VOP_VB0_STR)));
    pMvopTimingInfo->u16VBlank0_End = (HAL_ReadByte(MAP_REG(eID, VOP_VB0_END + 1)<< 8)) | (HAL_ReadByte(MAP_REG(eID, VOP_VB0_END)));
    pMvopTimingInfo->u16VBlank1_Start = (HAL_ReadByte(MAP_REG(eID, VOP_VB1_STR + 1)<< 8)) | (HAL_ReadByte(MAP_REG(eID, VOP_VB1_STR)));
    pMvopTimingInfo->u16VBlank1_End = (HAL_ReadByte(MAP_REG(eID, VOP_VB1_END + 1)<< 8)) | (HAL_ReadByte(MAP_REG(eID, VOP_VB1_END)));
    pMvopTimingInfo->u16TopField_Start = (HAL_ReadByte(MAP_REG(eID, VOP_TF_STR + 1)<< 8)) | (HAL_ReadByte(MAP_REG(eID, VOP_TF_STR)));
    pMvopTimingInfo->u16BottomField_Start = (HAL_ReadByte(MAP_REG(eID, VOP_BF_STR + 1)<< 8)) | (HAL_ReadByte(MAP_REG(eID, VOP_BF_STR)));
    pMvopTimingInfo->u16HActive_Start = (HAL_ReadByte(MAP_REG(eID, VOP_HACT_STR + 1)<< 8)) | (HAL_ReadByte(MAP_REG(eID, VOP_HACT_STR)));
    pMvopTimingInfo->u16TopField_VS = (HAL_ReadByte(MAP_REG(eID, VOP_TF_VS + 1)<< 8)) | (HAL_ReadByte(MAP_REG(eID, VOP_TF_VS)));
    pMvopTimingInfo->u16BottomField_VS = (HAL_ReadByte(MAP_REG(eID, VOP_BF_VS + 1)<< 8)) | (HAL_ReadByte(MAP_REG(eID, VOP_BF_VS)));
    pMvopTimingInfo->bInterlace = (HAL_ReadRegBit(MAP_REG(eID, VOP_CTRL0), BIT7) == BIT7);
    return TRUE;
}
#endif


