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





#include "MsCommon.h"



//!! Do not include this header in driver or api level



//-------------------------------------------------------------------------------------------------

//  Driver Compiler Options

//-------------------------------------------------------------------------------------------------

#define XC_MUTEX

#define XC_MUTEX_DBG	0



//-------------------------------------------------------------------------------------------------

//  Macro and Define

//-------------------------------------------------------------------------------------------------




#ifdef MSOS_TYPE_CE

#ifdef ASIC_VERIFY
	#ifdef MHAL_SC_C
	#define INTERFACE
	#else
	#define INTERFACE	extern
	#endif


	INTERFACE MS_U32 _XC_RIU_BASE;     // This should be inited before XC library starting.
	INTERFACE MS_U32 _HDCP_RIU_BASE;
#else
	#if (1==WITHIN_MHAL_XC_DLL)
	#define INTERFACE	extern
	#else
	#define INTERFACE	__declspec(dllimport)
	#endif


	#ifdef MHAL_SC_C

	#pragma data_seg(".XCRiu")
	MS_U32 _XC_RIU_BASE 	=0x0;	 // This should be inited before XC library starting.
	MS_U32 _HDCP_RIU_BASE	=0x0;
	#pragma data_seg()
	#pragma comment(linker,"/SECTION:.XCRiu,RWS")

	#else

	INTERFACE MS_U32 _XC_RIU_BASE;     // This should be inited before XC library starting.
	INTERFACE MS_U32 _HDCP_RIU_BASE;

	#endif
#endif

#else

	#ifdef MHAL_SC_C
	#define INTERFACE
	#else
	#define INTERFACE	extern
	#endif


	INTERFACE MS_U32 _XC_RIU_BASE;     // This should be inited before XC library starting.
	INTERFACE MS_U32 _HDCP_RIU_BASE;

#endif
#undef INTERFACE


extern MS_U32 _PM_RIU_BASE;



#define _BITMASK(loc_msb, loc_lsb) ((1U << (loc_msb)) - (1U << (loc_lsb)) + (1U << (loc_msb)))

#define BITMASK(x) _BITMASK(1?x, 0?x)

#define HBMASK    0xFF00

#define LBMASK    0x00FF



#define RIU_MACRO_START     do {

#define RIU_MACRO_END       } while (0)



// Address bus of RIU is 16 bits.


#if ( defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_NOS) || defined(MSOS_TYPE_LINUX_KERNEL))

#define RIU_READ_BYTE(addr)         ( READ_BYTE( _XC_RIU_BASE+ (addr) ) )
#define RIU_READ_2BYTE(addr)        ( READ_WORD( _XC_RIU_BASE + (addr) ) )
#define RIU_WRITE_BYTE(addr, val)    WRITE_BYTE( (_XC_RIU_BASE + (addr)), val)
#define RIU_WRITE_2BYTE(addr, val)   WRITE_WORD( _XC_RIU_BASE + (addr), val)
#else
#define RIU_READ_BYTE(addr)         ( READ_BYTE( _XC_RIU_BASE + (addr) ) )
#define RIU_READ_2BYTE(addr)        ( READ_WORD( _XC_RIU_BASE + (addr) ) )
#define RIU_WRITE_BYTE(addr, val)   { WRITE_BYTE( _XC_RIU_BASE + (addr), val) }
#define RIU_WRITE_2BYTE(addr, val)  { WRITE_WORD( _XC_RIU_BASE + (addr), val) }
#endif


// Address bus of RIU is 16 bits for PM //alex_tung

#define PM_RIU_READ_BYTEM(addr)        ( READ_BYTE( _PM_RIU_BASE + (addr) ) )

#define PM_RIU_READ_2BYTE(addr)        ( READ_WORD( _PM_RIU_BASE + (addr) ) )

#define PM_RIU_WRITE_BYTE(addr, val)   { WRITE_BYTE( _PM_RIU_BASE + (addr), val) }

#if ( defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_NOS) || defined(MSOS_TYPE_LINUX_KERNEL))

#define PM_RIU_WRITE_2BYTE(addr, val)  WRITE_WORD( _PM_RIU_BASE + (addr), val)
#else
#define PM_RIU_WRITE_2BYTE(addr, val)  { WRITE_WORD( _PM_RIU_BASE + (addr), val) }
#endif


//Address bus of RIU for HDCP for T3
#if ( defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_NOS) || defined(MSOS_TYPE_LINUX_KERNEL))


#define HDCP_RIU_READ_BYTE(addr)         ( READ_BYTE( _HDCP_RIU_BASE + (addr) ) )
#define HDCP_RIU_READ_2BYTE(addr)        ( READ_WORD( _HDCP_RIU_BASE + (addr) ) )
#define HDCP_RIU_WRITE_BYTE(addr, val)   { WRITE_BYTE( _HDCP_RIU_BASE + (addr), val) }
#define HDCP_RIU_WRITE_2BYTE(addr, val)   WRITE_WORD( _HDCP_RIU_BASE + (addr), val)

#else

#define HDCP_RIU_READ_BYTE(addr)         ( READ_BYTE( _HDCP_RIU_BASE + (addr) ) )
#define HDCP_RIU_READ_2BYTE(addr)        ( READ_WORD( _HDCP_RIU_BASE + (addr) ) )
#define HDCP_RIU_WRITE_BYTE(addr, val)   { WRITE_BYTE( _HDCP_RIU_BASE + (addr), val) }
#define HDCP_RIU_WRITE_2BYTE(addr, val)  { WRITE_WORD( _HDCP_RIU_BASE + (addr), val) }

#endif




//Address bus of RIU for DDC for T3
#if ( defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_NOS) || defined(MSOS_TYPE_LINUX_KERNEL))


#define DDC_RIU_READ_1BYTE(addr)         ( READ_BYTE( _HDCP_RIU_BASE + (addr) ) )
#define DDC_RIU_READ_2BYTE(addr)         ( READ_WORD( _HDCP_RIU_BASE + (addr) ) )
#define DDC_RIU_WRITE_1BYTE(addr, val)   { WRITE_BYTE( _HDCP_RIU_BASE + (addr), val) }
#define DDC_RIU_WRITE_2BYTE(addr, val)   WRITE_WORD( _HDCP_RIU_BASE + (addr), val)

#else

#define DDC_RIU_READ_1BYTE(addr)         ( READ_BYTE( _HDCP_RIU_BASE + (addr) ) )
#define DDC_RIU_READ_2BYTE(addr)         ( READ_WORD( _HDCP_RIU_BASE + (addr) ) )
#define DDC_RIU_WRITE_1BYTE(addr, val)   { WRITE_BYTE( _HDCP_RIU_BASE + (addr), val) }
#define DDC_RIU_WRITE_2BYTE(addr, val)   { WRITE_WORD( _HDCP_RIU_BASE + (addr), val) }
#endif



#define _XC_MUTEX_TIME_OUT                 5000//MSOS_WAIT_FOREVER



// Mutex function

#define _XC_ENTRY()     _XC_ENTRY_MUTEX(_XC_Mutex)

#define _XC_RETURN()    _XC_RETURN_MUTEX(_XC_Mutex)



#ifdef XC_MUTEX

    #if(XC_MUTEX_DBG)

        #define _XC_ENTRY_MUTEX(_mutex_)                                                                  \
                printf("1,==========================\n");                              \
                printf("[%s][%s][%06d]\n",__FILE__,__FUNCTION__,__LINE__);    \
                if(!MsOS_ObtainMutex(_mutex_, _XC_MUTEX_TIME_OUT))                     \
                {                                                                        \
                    printf("==========================\n");                              \
                    printf("[%s][%s][%06d] Mutex taking timeout\n",__FILE__,__FUNCTION__,__LINE__);    \
                }
        #define _XC_RETURN_MUTEX(_mutex_)                                                                \
                printf("0,==========================\n");                              \
                printf("[%s][%s][%06d] \n",__FILE__,__FUNCTION__,__LINE__);    \
                MsOS_ReleaseMutex(_mutex_);
                                    //return _ret;
    #else
        #define _XC_ENTRY_MUTEX(_mutex_)                                                                  \
                if(!MsOS_ObtainMutex(_mutex_, _XC_MUTEX_TIME_OUT))                     \
                {                                                                        \
                }

        #define _XC_RETURN_MUTEX(_mutex_)    MsOS_ReleaseMutex(_mutex_);

                                    //return _ret;

    #endif

#else // #if not def XC_MUTEX

    #define _XC_ENTRY_MUTEX(_mutex_)     while(0)

    #define _XC_RETURN_MUTEX(_mutex_)    while(0)

#endif // #ifdef _XC_MUTEX



//-------------------------------------------------------------------------------------------------

//  Function and Variable

//-------------------------------------------------------------------------------------------------

#ifdef MVIDEO_C

#define INTERFACE

INTERFACE MS_S32            _XC_Mutex = -1;

INTERFACE MS_S32            _XC_ISR_Mutex = -1;

#ifdef MSOS_TYPE_LINUX_KERNEL

INTERFACE wait_queue_head_t _XC_EventQueue;

INTERFACE MS_U32            _XC_EventFlag = 0;

INTERFACE MS_BOOL           _XC_VSyncRun = FALSE;

INTERFACE MS_U32            _XC_VSyncCount = 0;

INTERFACE MS_U32            _XC_VSyncMax = 0;

#endif

#else
#define INTERFACE extern
INTERFACE MS_S32            _XC_Mutex;

INTERFACE MS_S32            _XC_ISR_Mutex;

#ifdef MSOS_TYPE_LINUX_KERNEL

INTERFACE wait_queue_head_t _XC_EventQueue;

INTERFACE MS_U32            _XC_EventFlag;

INTERFACE MS_BOOL           _XC_VSyncRun;

INTERFACE MS_U32            _XC_VSyncCount;

INTERFACE MS_U32            _XC_VSyncMax;

#endif

#endif



//=============================================================

// Standard Form



#define MDrv_ReadByte( u32Reg )   RIU_READ_BYTE(((u32Reg) << 1) - ((u32Reg) & 1))



#define MDrv_Read2Byte( u32Reg )    (RIU_READ_2BYTE((u32Reg)<<1))



#define MDrv_Read4Byte( u32Reg )   ( (MS_U32)RIU_READ_2BYTE((u32Reg)<<1) | ((MS_U32)RIU_READ_2BYTE(((u32Reg)+2)<<1)<<16 )  )



#define MDrv_ReadRegBit( u32Reg, u8Mask )   (RIU_READ_BYTE(((u32Reg)<<1) - ((u32Reg) & 1)) & (u8Mask))



#define MDrv_WriteRegBit( u32Reg, bEnable, u8Mask )                                     \
    RIU_MACRO_START                                                                     \
    RIU_WRITE_BYTE( (((u32Reg) <<1) - ((u32Reg) & 1)) , (bEnable) ? (RIU_READ_BYTE(  (((u32Reg) <<1) - ((u32Reg) & 1))  ) |  (u8Mask)) :                           \
                                (RIU_READ_BYTE( (((u32Reg) <<1) - ((u32Reg) & 1)) ) & ~(u8Mask)));                            \
    RIU_MACRO_END



#define MDrv_WriteByte( u32Reg, u8Val )                                                 \
    RIU_MACRO_START                                                                     \
    RIU_WRITE_BYTE(((u32Reg) << 1) - ((u32Reg) & 1), u8Val);   \
    RIU_MACRO_END

#if ( defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_NOS) || defined(MSOS_TYPE_LINUX_KERNEL))

#define MDrv_Write2ByteMask( u32Reg, u16Val, u16Mask) \
    RIU_MACRO_START                                     \
    RIU_WRITE_2BYTE(u32Reg<<1,   (RIU_READ_2BYTE(u32Reg<<1) & ~(u16Mask)) | (u16Val & u16Mask)); \
    RIU_MACRO_END
#else
#define MDrv_Write2ByteMask( u32Reg, u16Val, u16Mask) \
    RIU_MACRO_START                                     \
    RIU_WRITE_2BYTE(u32Reg<<1,   (RIU_READ_2BYTE(u32Reg<<1) & ~(u16Mask)) | (u16Val & u16Mask)) \
    RIU_MACRO_END
#endif

#if ( defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_NOS) || defined(MSOS_TYPE_LINUX_KERNEL))

#define MDrv_Write2Byte( u32Reg, u16Val )                                               \
    RIU_MACRO_START                                                                     \
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
#define MDrv_Write2Byte_ve( u32Reg, u16Val )                                               \
    RIU_MACRO_START                                                                     \
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
#else
#define MDrv_Write2Byte( u32Reg, u16Val )                                               \
    RIU_MACRO_START                                                                     \
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
#endif


#if ( defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_NOS) || defined(MSOS_TYPE_LINUX_KERNEL))

#define MDrv_Write3Byte( u32Reg, u32Val )   \
    RIU_MACRO_START                         \
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
#else
#define MDrv_Write3Byte( u32Reg, u32Val )   \
    RIU_MACRO_START                         \
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
#endif

#define MDrv_Write4Byte( u32Reg, u32Val )                                               \
    RIU_MACRO_START                                                                     \
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
    RIU_WRITE_BYTE( (((u32Reg) <<1) - ((u32Reg) & 1)), (RIU_READ_BYTE((((u32Reg) <<1) - ((u32Reg) & 1))) & ~(u8Msk)) | ((u8Val) & (u8Msk)));                   \
    RIU_MACRO_END








//=============================================================

// Just for Scaler

#if ENABLE_REGISTER_SPREAD


	#if  (defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_LINUX_KERNEL))

	#define SC_R2BYTEMSK( u32Reg, u16mask)\
        ( ( RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) ) << 1) & (u16mask) ) )

	#define SC_W2BYTEMSK( u32Reg, u16Val, u16Mask)\
        ( ( RIU_WRITE_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) ) << 1, (RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) ) << 1) & ~(u16Mask) ) | ((u16Val) & (u16Mask)) ) ))

	#define SC_W2BYTE( u32Reg, u16Val)\
		( ( RIU_WRITE_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) ) << 1 , u16Val  ) ) )

	#define SC_R4BYTE( u32Reg )\
        ( ( RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) ) << 1 ) | (MS_U32)(RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) + 2 ) << 1 )) << 16))

	#define SC_W4BYTE( u32Reg, u32Val)\
		 RIU_WRITE_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) ) <<1, (MS_U16)((u32Val) & 0x0000FFFF) ) ; \
		 RIU_WRITE_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) + 2 ) << 1, (MS_U16)(((u32Val) >> 16) & 0x0000FFFF) )

	#define SC_R2BYTE( u32Reg ) \
        ( ( RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) ) <<1 ) ) )

	#elif  defined(MSOS_TYPE_NOS)

	#define SC_W2BYTE( u32Reg, u16Val)\
 		RIU_WRITE_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) ) << 1 , u16Val  )

	#define SC_R2BYTE( u32Reg ) \
 		RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) ) <<1 )

	#define SC_W4BYTE( u32Reg, u32Val)\
 		RIU_WRITE_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) ) <<1, (MS_U16)((u32Val) & 0x0000FFFF) ) ; \
 		RIU_WRITE_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) + 2 ) << 1, (MS_U16)(((u32Val) >> 16) & 0x0000FFFF) )

	#define SC_R4BYTE( u32Reg )\
 		(RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) ) << 1 ) | (MS_U32)(RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) + 2 ) << 1 )) << 16)

	#define SC_R2BYTEMSK( u32Reg, u16mask)\
 		(RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) ) << 1) & (u16mask))

	#define SC_W2BYTEMSK( u32Reg, u16Val, u16Mask)\
		RIU_WRITE_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) ) << 1, (RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) ) << 1) & ~(u16Mask) ) | ((u16Val) & (u16Mask)) )

	#else

	#define SC_W2BYTEMSK( u32Reg, u16Val, u16Mask)\
        ( { RIU_WRITE_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) ) << 1, (RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) ) << 1) & ~(u16Mask) ) | ((u16Val) & (u16Mask)) ) ; })

	#define SC_W2BYTE( u32Reg, u16Val)\
        ( { RIU_WRITE_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) ) << 1 , u16Val  ) ; } )

	#define SC_R2BYTEMSK( u32Reg, u16mask)\
			( { RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) ) << 1) & (u16mask) ; } )

	#define SC_R4BYTE( u32Reg )\
        ( { RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) ) << 1 ) | (MS_U32)(RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) + 2 ) << 1 )) << 16; })

	#define SC_W4BYTE( u32Reg, u32Val)\
        ( { RIU_WRITE_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) ) <<1, (MS_U16)((u32Val) & 0x0000FFFF) ) ; \
            RIU_WRITE_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) + 2 ) << 1, (MS_U16)(((u32Val) >> 16) & 0x0000FFFF) ); } )

	#define SC_R2BYTE( u32Reg ) \
        ( { RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFFFF) ) <<1 ) ; } )

	#endif

#else


	#if  ( defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_NOS) || defined(MSOS_TYPE_LINUX_KERNEL))
	#define SC_W2BYTE( u32Reg, u16Val)\
            ( ( RIU_WRITE_2BYTE(REG_SCALER_BASE << 1, ((u32Reg) >> 8) & 0x00FF ) ; \
                RIU_WRITE_2BYTE( (REG_SCALER_BASE +((u32Reg) & 0xFF) ) << 1 , u16Val  )  ) )

	#define SC_W4BYTE( u32Reg, u32Val)\
				 { RIU_WRITE_2BYTE(REG_SCALER_BASE<<1, ((u32Reg) >> 8) & 0x00FF ) ; \
					RIU_WRITE_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFF) ) <<1, (MS_U16)((u32Val) & 0x0000FFFF) ) ; \
					RIU_WRITE_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFF) + 2 ) << 1, (MS_U16)(((u32Val) >> 16) & 0x0000FFFF) ); }

	#define SC_R2BYTEMSK( u32Reg, u16mask)\
             RIU_WRITE_2BYTE(REG_SCALER_BASE << 1, ((u32Reg) >> 8) & 0x00FF ) ; \
                RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFF) ) << 1) & (u16mask)

	#define SC_W2BYTEMSK( u32Reg, u16Val, u16Mask)\
            ( ( RIU_WRITE_2BYTE(REG_SCALER_BASE << 1, ((u32Reg) >> 8) & 0x00FF ) ; \
                RIU_WRITE_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFF) ) << 1, (RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFF) ) << 1) & ~(u16Mask) ) | ((u16Val) & (u16Mask)) )  ))
	#define SC_W2BYTEMSK1( u32Reg, u16Val, u16Mask)\
            ( ( RIU_WRITE_2BYTE(REG_SCALER_BASE << 1, ((u32Reg) >> 8) & 0x00FF ) ; \
                RIU_WRITE_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFF) ) << 1, (RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFF) ) << 1) & ~(u16Mask) ) | ((u16Val) & (u16Mask)) )  ))

	#else
	#define SC_W2BYTE( u32Reg, u16Val)\
            ( { RIU_WRITE_2BYTE(REG_SCALER_BASE << 1, ((u32Reg) >> 8) & 0x00FF ) ; \
                RIU_WRITE_2BYTE( (REG_SCALER_BASE +((u32Reg) & 0xFF) ) << 1 , u16Val  ) ; } )

	#define SC_W4BYTE( u32Reg, u32Val)\
            ( { RIU_WRITE_2BYTE(REG_SCALER_BASE<<1, ((u32Reg) >> 8) & 0x00FF ) ; \
                RIU_WRITE_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFF) ) <<1, (MS_U16)((u32Val) & 0x0000FFFF) ) ; \
                RIU_WRITE_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFF) + 2 ) << 1, (MS_U16)(((u32Val) >> 16) & 0x0000FFFF) ); } )
	#define SC_R2BYTEMSK( u32Reg, u16mask)\
				( { RIU_WRITE_2BYTE(REG_SCALER_BASE << 1, ((u32Reg) >> 8) & 0x00FF ) ; \
					RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFF) ) << 1) & (u16mask) ; } )
	#define SC_W2BYTEMSK( u32Reg, u16Val, u16Mask)\
				( { RIU_WRITE_2BYTE(REG_SCALER_BASE << 1, ((u32Reg) >> 8) & 0x00FF ) ; \
					RIU_WRITE_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFF) ) << 1, (RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFF) ) << 1) & ~(u16Mask) ) | ((u16Val) & (u16Mask)) ) ; })

	#endif

	#define SC_R2BYTE( u32Reg ) \
            ( { RIU_WRITE_2BYTE(REG_SCALER_BASE << 1, ( (u32Reg) >> 8) & 0x00FF ) ; \
                RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFF) )<<1 ) ; } )

	#define SC_R4BYTE( u32Reg )\
            ( { RIU_WRITE_2BYTE(REG_SCALER_BASE << 1, ((u32Reg) >> 8) & 0x00FF ) ; \
                RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFF) ) << 1 ) | (MS_U32)(RIU_READ_2BYTE( (REG_SCALER_BASE + ((u32Reg) & 0xFF) + 2 ) << 1 )) << 16; } )

#endif

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
#if ( defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_NOS) || defined(MSOS_TYPE_LINUX_KERNEL))
#define W2BYTE( u32Reg, u16Val) RIU_WRITE_2BYTE( (u32Reg) << 1 , u16Val )
#else
#define W2BYTE( u32Reg, u16Val) RIU_WRITE_2BYTE( (u32Reg) << 1 , u16Val )
#endif

#if (defined( MSOS_TYPE_CE) || defined(MSOS_TYPE_LINUX_KERNEL))
#define W2BYTE1( u32Reg, u16Val) RIU_WRITE_2BYTE( (u32Reg) << 1 , u16Val )
#endif





#define R2BYTE( u32Reg ) RIU_READ_2BYTE( (u32Reg) << 1)



#define W4BYTE( u32Reg, u32Val)\
            ( { RIU_WRITE_2BYTE( (u32Reg) << 1, ((u32Val) & 0x0000FFFF) ); \
                RIU_WRITE_2BYTE( ( (u32Reg) + 2) << 1 , (((u32Val) >> 16) & 0x0000FFFF)) ; } )



#define R4BYTE( u32Reg )\
            ( { ((RIU_READ_2BYTE( (u32Reg) << 1)) | ((MS_U32)(RIU_READ_2BYTE( ( (u32Reg) + 2 ) << 1) ) << 16)) ; } )


#if ( defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_NOS) || defined(MSOS_TYPE_LINUX_KERNEL))
#define R2BYTEMSK( u32Reg, u16mask)\
			( ( RIU_READ_2BYTE( (u32Reg)<< 1) & u16mask  ) )
#else
#define R2BYTEMSK( u32Reg, u16mask)\
            ( { RIU_READ_2BYTE( (u32Reg)<< 1) & u16mask ; } )
#endif



#if ( defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_NOS) || defined(MSOS_TYPE_LINUX_KERNEL))
#define W2BYTEMSK( u32Reg, u16Val, u16Mask)\
	  RIU_WRITE_2BYTE( (u32Reg)<< 1 , (RIU_READ_2BYTE((u32Reg) << 1) & ~(u16Mask)) | ((u16Val) & (u16Mask)) )
#define W2BYTEMSK1( u32Reg, u16Val, u16Mask)\
	( ( RIU_WRITE_2BYTE( (u32Reg)<< 1 , (RIU_READ_2BYTE((u32Reg) << 1) & ~(u16Mask)) | ((u16Val) & (u16Mask)) )  ) )
#else
#define W2BYTEMSK( u32Reg, u16Val, u16Mask)\
            ( { RIU_WRITE_2BYTE( (u32Reg)<< 1 , (RIU_READ_2BYTE((u32Reg) << 1) & ~(u16Mask)) | ((u16Val) & (u16Mask)) ) ; } )
#endif


#define W3BYTE( u32Reg, u32Val)\
            ( { RIU_WRITE_2BYTE( (u32Reg) << 1,  u32Val); \
                RIU_WRITE_BYTE( (u32Reg + 2) << 1 ,  ((u32Val) >> 16)); } )





//=============================================================

// Just for PM registers



/*
    R/W register forced to use 8 bit address, everytime need to r/w 2 bytes with mask

    32bit address     16 bit address        8 bit address
        0                   0                       0
        1                   x                       1
        2
        3
        4                   1                       2
        5                   x                       3
        6
        7
        8                   2                       4
        9                   x                       5
        A
        B                                                                                  */

// to read 0x2F03[3], please use R1BYTE(0x2F03, 3:3)
#define PM_R1BYTE(u32Addr, u8mask)            \
    (READ_BYTE (_PM_RIU_BASE + (u32Addr << 1) - (u32Addr & 1)) & BMASK(u8mask))


// to write 0x2F02[4:3] with 2'b10, please use W1BYTE(0x2F02, 0x02, 4:3)
#define PM_W1BYTE(u32Addr, u8Val, u8mask)     \
    (WRITE_BYTE(_PM_RIU_BASE + (u32Addr << 1) - (u32Addr & 1), (PM_R1BYTE(u32Addr, 7:0) & ~BMASK(u8mask)) | (BITS(u8mask, u8Val) & BMASK(u8mask))))

#if 1//alex_tung

// access PM registers

// u32Addr must be 16bit aligned

#define PM_R2BYTE( u32Reg, u16mask)\
            ( PM_RIU_READ_2BYTE( (u32Reg)<< 1) & u16mask )

    // u32Addr must be 16bit aligned

#define PM_W2BYTE( u32Reg, u16Val, u16Mask)\
            (  PM_RIU_WRITE_2BYTE( (u32Reg)<< 1 , (PM_RIU_READ_2BYTE((u32Reg) << 1) & ~(u16Mask)) | ((u16Val) & (u16Mask)) )  )

#else

// u32Addr must be 16bit aligned
#define PM_R2BYTE(u32Addr, u16mask)            \
    (READ_WORD (_PM_RIU_BASE + (u32Addr << 1)) & BMASK(u16mask))


// u32Addr must be 16bit aligned
#define PM_W2BYTE(u32Addr, u16Val, u16mask)    \
    (WRITE_WORD(_PM_RIU_BASE + (u32Addr << 1), (PM_R2BYTE(u32Addr, 15:0) & ~BMASK(u16mask)) | (BITS(u16mask, u16Val) & BMASK(u16mask))))

#endif

//=============================================================

// Just for HDCP registers for T3

#define HDCP_W2BYTE( u32Reg, u16Val) HDCP_RIU_WRITE_2BYTE( (u32Reg) << 1 , u16Val )



#define HDCP_R2BYTE( u32Reg )  HDCP_RIU_READ_2BYTE( (u32Reg) << 1)



#define HDCP_R2BYTEMSK( u32Reg, u16mask)\
            ( { HDCP_RIU_READ_2BYTE( (u32Reg)<< 1) & u16mask ; } )


#if ( defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_NOS) || defined(MSOS_TYPE_LINUX_KERNEL))
#define HDCP_W2BYTEMSK( u32Reg, u16Val, u16Mask)\
            (  HDCP_RIU_WRITE_2BYTE( (u32Reg)<< 1 , (HDCP_RIU_READ_2BYTE((u32Reg) << 1) & ~(u16Mask)) | ((u16Val) & (u16Mask)))   )
#else
#define HDCP_W2BYTEMSK( u32Reg, u16Val, u16Mask)\
            ( { HDCP_RIU_WRITE_2BYTE( (u32Reg)<< 1 , (HDCP_RIU_READ_2BYTE((u32Reg) << 1) & ~(u16Mask)) | ((u16Val) & (u16Mask))) ; } )
#endif


//=============================================================

//for DDC registers for T3



#define DDC_WRITE( u32Reg, u16Val) DDC_RIU_WRITE_2BYTE( (u32Reg) << 1 , u16Val )



#define DDC_READ( u32Reg )  DDC_RIU_READ_2BYTE( (u32Reg) << 1)

#define DDC_READ_MASK( u32Reg, u16mask)\
            ( { DDC_RIU_READ_2BYTE( (u32Reg)<< 1) & u16mask ; } )

#if ( defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_NOS) )
#define DDC_WRITE_MASK( u32Reg, u16Val, u16Mask)\
			DDC_RIU_WRITE_2BYTE( (u32Reg)<< 1 , (DDC_RIU_READ_2BYTE((u32Reg) << 1) & ~(u16Mask)) | ((u16Val) & (u16Mask)))
#else
#define DDC_WRITE_MASK( u32Reg, u16Val, u16Mask)\
            ( { DDC_RIU_WRITE_2BYTE( (u32Reg)<< 1 , (DDC_RIU_READ_2BYTE((u32Reg) << 1) & ~(u16Mask)) | ((u16Val) & (u16Mask))) ; } )
#endif


//-------------------------------------------------------------------------------------------------

//  Type and Structure

//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------

//  Function and Variable

//-------------------------------------------------------------------------------------------------













#undef INTERFACE



#endif



