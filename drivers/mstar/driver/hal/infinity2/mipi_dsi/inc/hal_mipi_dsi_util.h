////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2008 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (¡§MStar Confidential Information¡¨) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////


/**
* @file     hal_scl_util.h
* @version
* @Platform I2
* @brief    This file defines the HAL MIPI TX DSI utility interface
*
*/

#ifndef __HAL_MIPI_DSI_UTIL_H__
#define __HAL_MIPI_DSI_UTIL_H__

#define MIPI_DSI_RIU_BASE 0xFD000000

#define WRITE_REG(addr, type, data)  ((*(volatile type *)(addr)) = (data))
#define READ_REG(addr, type)         ((*(volatile type *)(addr)))



#define READ_BYTE(x)         READ_REG(x, u8)
#define READ_WORD(x)         READ_REG(x, u16)
#define READ_LONG(x)         READ_REG(x, u32)
#define WRITE_BYTE(x, y)     WRITE_REG(x, u8, y)
#define WRITE_WORD(x, y)     WRITE_REG(x, u16, y)

#if 0
#define WRITE_LONG(x, y)      do{\
                                                WRITE_REG(0x1f207002, u32, 0x10); \
                                                WRITE_REG(x, u32, y); \
                                                WRITE_REG(0x1f207002, u32, 0x00); \
                                                }while(0);
#else
#define WRITE_LONG(x, y)    WRITE_REG(x, u32, y)
#endif

#define CHIP_BASE(type, BankAddr,offset,shift)   (MIPI_DSI_RIU_BASE+ (BankAddr)+ (offset<<shift))

#define RIU_READ_BYTE(addr)         ( READ_BYTE( MIPI_DSI_RIU_BASE+ (addr) ) )
#define RIU_READ_2BYTE(addr)        ( READ_WORD( MIPI_DSI_RIU_BASE + (addr) ) )
#define RIU_READ_4BYTE(addr)        ( READ_LONG( MIPI_DSI_RIU_BASE + (addr) ) )

#define RIU_WRITE_BYTE(addr, val)    WRITE_BYTE( (MIPI_DSI_RIU_BASE + (addr)), val)
#define RIU_WRITE_2BYTE(addr, val)   WRITE_WORD( MIPI_DSI_RIU_BASE + (addr), val)
#define RIU_WRITE_4BYTE(addr, val)   WRITE_LONG( MIPI_DSI_RIU_BASE + (addr), val)

#define R2BYTE( u32Reg ) RIU_READ_2BYTE( (u32Reg) << 1)

#define R2BYTEMSK( u32Reg, u16mask)\
			( ( RIU_READ_2BYTE( (u32Reg)<< 1) & u16mask  ) )

#define R4BYTEMSK( u32Reg, u32mask)\
			( ( RIU_READ_4BYTE( (u32Reg)<< 1) & u32mask  ) )

//#define R4BYTE( u32Reg )
//            ( { ((RIU_READ_2BYTE( (u32Reg) << 1)) | ((U32)(RIU_READ_2BYTE( ( (u32Reg) + 2 ) << 1) ) << 16)) ; } )


#define R4BYTE( u32Reg )\
            ( {  (RIU_READ_4BYTE( (u32Reg)<<1)) ; } )

#define WBYTE(u32Reg, u8Val) RIU_WRITE_BYTE( ((u32Reg) << 1), u8Val )

#define WBYTEMSK(u32Reg, u8Val, u8Mask) \
               RIU_WRITE_BYTE( (((u32Reg)<<1) - ((u32Reg) & 1)), ( RIU_READ_BYTE( (((u32Reg)<<1) - ((u32Reg) & 1)) ) & ~(u8Mask)) | ((u8Val) & (u8Mask)) )

#define W2BYTE( u32Reg, u16Val) RIU_WRITE_2BYTE( (u32Reg) << 1 , u16Val )

#define W2BYTEMSK( u32Reg, u16Val, u16Mask)\
              RIU_WRITE_2BYTE( (u32Reg)<< 1 , (RIU_READ_2BYTE((u32Reg) << 1) & ~(u16Mask)) | ((u16Val) & (u16Mask)) )

#if 0
#define W4BYTE( u32Reg, u32Val)\
            ( { RIU_WRITE_2BYTE( (u32Reg) << 1, ((u32Val) & 0x0000FFFF) ); \
                RIU_WRITE_2BYTE( ( (u32Reg) + 2) << 1 , (((u32Val) >> 16) & 0x0000FFFF)) ; } )

#else
#define W4BYTE( u32Reg, u32Val)\
            ( { RIU_WRITE_4BYTE( (u32Reg)<<1, u32Val);})
#endif

#define W4BYTEMSK( u32Reg, u32Val, u32Mask)\
              RIU_WRITE_4BYTE( (u32Reg)<< 1 , (RIU_READ_4BYTE((u32Reg) << 1) & ~(u32Mask)) | ((u32Val) & (u32Mask)) )


#define W3BYTE( u32Reg, u32Val)\
            ( { RIU_WRITE_2BYTE( (u32Reg) << 1,  u32Val); \
                RIU_WRITE_BYTE( (u32Reg + 2) << 1 ,  ((u32Val) >> 16)); } )

#define ReadRegBit( u32Reg, u32Mask )                                               \
         RIU_READ_4BYTE( (u32Reg) << 1) & (u32Mask)

#define WriteRegBit( u32Reg, bEnable, u16Mask )                                     \
	RIU_WRITE_2BYTE((u32Reg)<<1,  (bEnable) ? RIU_READ_2BYTE((u32Reg)<<1)|u16Mask :  \
									RIU_READ_2BYTE((u32Reg)<<1) & (~u16Mask));


#define WriteLongRegBit( u32Reg, bEnable, u32Mask )                                     \
	RIU_WRITE_4BYTE((u32Reg)<<1,  (bEnable) ? RIU_READ_4BYTE((u32Reg)<<1)|u32Mask :  \
									RIU_READ_4BYTE((u32Reg)<<1) & (~u32Mask));
extern s8 UartSendTrace(const char *strFormat, ...);

#endif /*__HAL_MIPI_DSI_UTIL_H__*/
