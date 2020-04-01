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
#ifndef _HWREG_UTILITY_H_
#define _HWREG_UTILITY_H_

#include "ve_hwreg.h"
#include "MsCommon.h"

//!! Do not include this header in driver or api level

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
extern MS_U32 _VE_RIU_BASE;      // This should be inited before XC library starting.

#ifdef MSOS_TYPE_CE
__declspec(dllimport) MS_BOOL g_bVeDisableRegWrite;
#else
extern MS_BOOL g_bVeDisableRegWrite;
#endif

#define _BITMASK(loc_msb, loc_lsb) ((1U << (loc_msb)) - (1U << (loc_lsb)) + (1U << (loc_msb)))
#define BITMASK(x) _BITMASK(1?x, 0?x)
#define HBMASK    0xFF00
#define LBMASK    0x00FF

#define RIU_MACRO_START     do {
#define RIU_MACRO_END       } while (0)

// Address bus of RIU is 16 bits.

#if (defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_LINUX_KERNEL))
#define RIU_READ_BYTE(addr)         ( READ_BYTE( _VE_RIU_BASE + (addr) ) )
#define RIU_READ_2BYTE(addr)        ( READ_WORD( _VE_RIU_BASE + (addr) ) )
#define RIU_WRITE_BYTE(addr, val)   WRITE_BYTE( _VE_RIU_BASE + (addr), val)
#define RIU_WRITE_2BYTE(addr, val)  WRITE_WORD( _VE_RIU_BASE + (addr), val)
#else
#define RIU_READ_BYTE(addr)         ( READ_BYTE( _VE_RIU_BASE + (addr) ) )
#define RIU_READ_2BYTE(addr)        ( READ_WORD( _VE_RIU_BASE + (addr) ) )
#define RIU_WRITE_BYTE(addr, val)   { WRITE_BYTE( _VE_RIU_BASE + (addr), val) }
#define RIU_WRITE_2BYTE(addr, val)  { WRITE_WORD( _VE_RIU_BASE + (addr), val) }
#endif

//=============================================================
// Standard Form

#define MDrv_ReadByte( u32Reg )   RIU_READ_BYTE(((u32Reg) << 1) - ((u32Reg) & 1))

#define MDrv_Read2Byte( u32Reg )    (RIU_READ_2BYTE((u32Reg)<<1))

#define MDrv_Read4Byte( u32Reg )   ( (MS_U32)RIU_READ_2BYTE((u32Reg)<<1) | ((MS_U32)RIU_READ_2BYTE(((u32Reg)+2)<<1)<<16 )  )

#define MDrv_ReadRegBit( u32Reg, u8Mask )   (RIU_READ_BYTE(((u32Reg)<<1) - ((u32Reg) & 1)) & (u8Mask))



#define MDrv_WriteRegBit( u32Reg, bEnable, u8Mask )                                     \
    RIU_MACRO_START                                                                     \
    if(g_bVeDisableRegWrite == TRUE)                            \
    {                                                           \
        break;                                                  \
    }                                                           \
    RIU_WRITE_BYTE( (((u32Reg) <<1) - ((u32Reg) & 1)) , (bEnable) ? (RIU_READ_BYTE(  (((u32Reg) <<1) - ((u32Reg) & 1))  ) |  (u8Mask)) :                           \
                                (RIU_READ_BYTE( (((u32Reg) <<1) - ((u32Reg) & 1)) ) & ~(u8Mask)));                            \
    RIU_MACRO_END



#define MDrv_WriteByte( u32Reg, u8Val )                                                 \
    RIU_MACRO_START                                                                     \
    if(g_bVeDisableRegWrite == TRUE)                            \
    {                                                           \
        break;                                                  \
    }                                                           \
    RIU_WRITE_BYTE(((u32Reg) << 1) - ((u32Reg) & 1), u8Val);   \
    RIU_MACRO_END



#define MDrv_Write2Byte( u32Reg, u16Val )                                               \
    RIU_MACRO_START                                                                     \
    if(g_bVeDisableRegWrite == TRUE)                            \
    {                                                           \
        break;                                                  \
    }                                                           \
    if ( ((u32Reg) & 0x01) )                                                        \
    {                                                                               \
        RIU_WRITE_BYTE(((u32Reg) << 1) - 1, (MS_U8)((u16Val)));                                  \
        RIU_WRITE_BYTE(((u32Reg) + 1) << 1, (MS_U8)((u16Val) >> 8));                             \
    }                                                                               \
    else                                                                            \
    {                                                                               \
        RIU_WRITE_2BYTE( ((u32Reg)<<1) ,  u16Val);                                                       \
    }                                                                               \
    RIU_MACRO_END



#define MDrv_Write3Byte( u32Reg, u32Val )   \
    RIU_MACRO_START                         \
    if(g_bVeDisableRegWrite == TRUE)                            \
    {                                                           \
        break;                                                  \
    }                                                           \
    if ((u32Reg) & 0x01)                                                                \
    {                                                                                               \
        RIU_WRITE_BYTE((u32Reg << 1) - 1, u32Val);                                    \
        RIU_WRITE_2BYTE( (u32Reg + 1)<<1 , ((u32Val) >> 8));                                      \
    }                                                                                           \
    else                                                                                        \
    {                                                                                               \
        RIU_WRITE_2BYTE( (u32Reg) << 1,  u32Val);                                                         \
        RIU_WRITE_BYTE( (u32Reg + 2) << 1 ,  ((u32Val) >> 16));                             \
    }                           \
    RIU_MACRO_END



#define MDrv_Write4Byte( u32Reg, u32Val )                                               \
    RIU_MACRO_START                                                                     \
    if(g_bVeDisableRegWrite == TRUE)                            \
    {                                                           \
        break;                                                  \
    }                                                           \
    if ((u32Reg) & 0x01)                                                      \
    {                                                                                               \
        RIU_WRITE_BYTE( ((u32Reg) << 1) - 1 ,  u32Val);                                         \
        RIU_WRITE_2BYTE( ((u32Reg) + 1)<<1 , ( (u32Val) >> 8));                                      \
        RIU_WRITE_BYTE( (((u32Reg) + 3) << 1) ,  ((u32Val) >> 24));                           \
    }                                                                                               \
    else                                                                                                \
    {                                                                                                   \
        RIU_WRITE_2BYTE( (u32Reg) <<1 ,  u32Val);                                                             \
        RIU_WRITE_2BYTE(  ((u32Reg) + 2)<<1 ,  ((u32Val) >> 16));                                             \
    }                                                                     \
    RIU_MACRO_END



#define MDrv_WriteByteMask( u32Reg, u8Val, u8Msk )                                      \
    RIU_MACRO_START                                                                     \
    if(g_bVeDisableRegWrite == TRUE)                            \
    {                                                           \
        break;                                                  \
    }                                                           \
    RIU_WRITE_BYTE( (((u32Reg) <<1) - ((u32Reg) & 1)), (RIU_READ_BYTE((((u32Reg) <<1) - ((u32Reg) & 1))) & ~(u8Msk)) | ((u8Val) & (u8Msk)));                   \
    RIU_MACRO_END



//============================================================

// For Dump Table

// u32Addr must be 16bit aligned
#define READ2BYTE(u32Addr, u16mask)            \
    (READ_WORD (_VE_RIU_BASE + (u32Addr << 1)) & BMASK(u16mask))

// u32Addr must be 16bit aligned
#define W2BYTE_TAB(u32Addr, u16Val, u16mask)    \
    (WRITE_WORD(_VE_RIU_BASE + ( (u32Addr)  << 1), (READ2BYTE( (u32Addr) , 15:0) & ~u16mask) | (  (u16Val)  & u16mask)))





//=============================================================

// Just for Scaler

#define SC_W2BYTE( u32Reg, u16Val)\
            ( { RIU_WRITE_2BYTE(REG_SCALER_BASE << 1, ((u32Reg) >> 8) & 0x00FF ) ; \
                RIU_WRITE_2BYTE( (REG_SCALER_BASE +((u32Reg) & 0xFF) ) << 1 , u16Val  ) ; } )

#define SC_R2BYTE( u32Reg ) \
            ( { RIU_WRITE_2BYTE(REG_SCALER_BASE << 1, ( (u32Reg) >> 8) & 0x00FF ) ; \
                RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFF) )<<1 ) ; } )

#define SC_W4BYTE( u32Reg, u32Val)\
            ( { RIU_WRITE_2BYTE(REG_SCALER_BASE<<1, ((u32Reg) >> 8) & 0x00FF ) ; \
                RIU_WRITE_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFF) ) <<1, (MS_U16)((u32Val) & 0x0000FFFF) ) ; \
                RIU_WRITE_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFF) + 2 ) << 1, (MS_U16)(((u32Val) >> 16) & 0x0000FFFF) ); } )

#define SC_R4BYTE( u32Reg )\
            ( { RIU_WRITE_2BYTE(REG_SCALER_BASE << 1, ((u32Reg) >> 8) & 0x00FF ) ; \
                RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFF) ) << 1 ) | (MS_U32)(RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFF) + 2 ) << 1 )) << 16; } )

#define SC_R2BYTEMSK( u32Reg, u16mask)\
            ( { RIU_WRITE_2BYTE(REG_SCALER_BASE << 1, ((u32Reg) >> 8) & 0x00FF ) ; \
                RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFF) ) << 1) & (u16mask) ; } )

#define SC_W2BYTEMSK( u32Reg, u16Val, u16Mask)\
            ( { RIU_WRITE_2BYTE(REG_SCALER_BASE << 1, ((u32Reg) >> 8) & 0x00FF ) ; \
                RIU_WRITE_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFF) ) << 1, (RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFF) ) << 1) & ~(u16Mask) ) | ((u16Val) & (u16Mask)) ) ; })

//=============================================================
// Just for MOD
#define MOD_W2BYTE( u32Reg, u16Val)\
            ( { RIU_WRITE_2BYTE(REG_MOD_BASE << 1, ((u32Reg) >> 8) & 0x00FF ); \
                RIU_WRITE_2BYTE( (REG_MOD_BASE + ((u32Reg) & 0xFF) ) << 1, u16Val ); } )

#define MOD_R2BYTE( u32Reg ) \
            ( { RIU_WRITE_2BYTE(REG_MOD_BASE << 1, ((u32Reg) >> 8) & 0x00FF ); \
                RIU_READ_2BYTE( (REG_MOD_BASE + ((u32Reg) & 0xFF) ) << 1) ; } )

#define MOD_R2BYTEMSK( u32Reg, u16mask)\
            ( { RIU_WRITE_2BYTE(REG_MOD_BASE << 1, ((u32Reg) >> 8) & 0x00FF ); \
                RIU_READ_2BYTE( (REG_MOD_BASE + ((u32Reg) & 0xFF) ) << 1) & (u16mask); })

#define MOD_W2BYTEMSK( u32Reg, u16Val, u16Mask)\
            ( { RIU_WRITE_2BYTE(REG_MOD_BASE << 1, ((u32Reg) >> 8) & 0x00FF ); \
                RIU_WRITE_2BYTE( (REG_MOD_BASE + ((u32Reg) & 0xFF) )<<1 , (RIU_READ_2BYTE( (REG_MOD_BASE + ((u32Reg) & 0xFF) ) << 1 ) & ~(u16Mask)) | ((u16Val) & (u16Mask)) ); } )



//=============================================================

//General ( Make sure u32Reg is not ODD

#define W2BYTE( u32Reg, u16Val) RIU_WRITE_2BYTE( (u32Reg) << 1 , u16Val )

#define R2BYTE( u32Reg ) RIU_READ_2BYTE( (u32Reg) << 1)

#define W4BYTE( u32Reg, u32Val)\
            ( { RIU_WRITE_2BYTE( (u32Reg) << 1, ((u32Val) & 0x0000FFFF) ); \
                RIU_WRITE_2BYTE( ( (u32Reg) + 2) << 1 , (((u32Val) >> 16) & 0x0000FFFF)) ; } )

#define R4BYTE( u32Reg )\
            ( { ((RIU_READ_2BYTE( (u32Reg) << 1)) | ((MS_U32)(RIU_READ_2BYTE( ( (u32Reg) + 2 ) << 1) ) << 16)) ; } )

#if (defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_LINUX_KERNEL))
#define R2BYTEMSK( u32Reg, u16mask)\
            RIU_READ_2BYTE( (u32Reg)<< 1) & (u16mask)
#else
#define R2BYTEMSK( u32Reg, u16mask)\
            ( { RIU_READ_2BYTE( (u32Reg)<< 1) & (u16mask) ; } )
#endif


#if (defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_LINUX_KERNEL))
#define W2BYTEMSK( u32Reg, u16Val, u16Mask)\
	RIU_WRITE_2BYTE( (u32Reg)<< 1 , (RIU_READ_2BYTE((u32Reg) << 1) & ~(u16Mask)) | ((u16Val) & (u16Mask)) )
#else
#define W2BYTEMSK( u32Reg, u16Val, u16Mask)\
            ( { RIU_WRITE_2BYTE( (u32Reg)<< 1 , (RIU_READ_2BYTE((u32Reg) << 1) & ~(u16Mask)) | ((u16Val) & (u16Mask)) ) ; } )
#endif


//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------

#endif

