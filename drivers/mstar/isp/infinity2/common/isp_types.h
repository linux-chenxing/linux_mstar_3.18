////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2011 MStar Semiconductor, Inc.
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
///
/// @file   MsTypes.h
/// @brief  MStar General Data Types
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __MS_TYPES_H__
#define __MS_TYPES_H__
#include <isp_porting_cfg.h>

//-------------------------------------------------------------------------------------------------
//  System Data Type
//-------------------------------------------------------------------------------------------------

#if 0
/// data type unsigned char, data length 1 byte
typedef unsigned char               u8;                                 // 1 byte
/// data type unsigned short, data length 2 byte
typedef unsigned short              u16;                                // 2 bytes
/// data type unsigned int, data length 4 byte
#if INTPTR_MAX == INT32_MAX
typedef unsigned int               u32;
#else
typedef unsigned long               u32;                                // 4 bytes
#endif
/// data type unsigned int, data length 8 byte
typedef unsigned long long          u64;                                // 8 bytes
/// data type signed char, data length 1 byte
typedef signed char                 s8;                                 // 1 byte
/// data type signed short, data length 2 byte
typedef signed short                s16;                                // 2 bytes
/// data type signed int, data length 4 byte
//typedef signed long                 s32;                                // 4 bytes
/// data type signed int, data length 8 byte
typedef signed long long            s64;                                // 8 bytes
#endif

/// data type float, data length 4 byte
typedef float                       FLOAT;                              // 4 bytes
/// data type unsigned char, data length 1 byte
typedef unsigned char               MS_U8;                              // 1 byte
/// data type unsigned short, data length 2 byte
typedef unsigned short              MS_U16;                             // 2 bytes
/// data type unsigned int, data length 4 byte
typedef unsigned long               MS_U32;                             // 4 bytes
/// data type unsigned int, data length 8 byte
typedef unsigned long long          MS_U64;                             // 8 bytes
/// data type signed char, data length 1 byte
typedef signed char                 MS_S8;                              // 1 byte
/// data type signed short, data length 2 byte
typedef signed short                MS_S16;                             // 2 bytes
/// data type signed int, data length 4 byte
typedef signed long                 MS_S32;                             // 4 bytes
/// data type signed int, data length 8 byte
typedef signed long long            MS_S64;                             // 8 bytes
/// data type float, data length 4 byte
typedef float                       MS_FLOAT;                           // 4 bytes
/// data type bool, data length 4 byte
typedef int                         MS_BOOL;                           // 4 bytes

#define FAIL                    (-1)
#define SUCCESS                 (0)

#if 0
/// data type null pointer
#ifdef NULL
#undef NULL
#endif
#define NULL                        0

/// data type hardware physical address
typedef unsigned long               MS_PHYADDR;                         // 32bit physical address

//-------------------------------------------------------------------------------------------------
//  Software Data Type
//-------------------------------------------------------------------------------------------------

/// definition for MS_BOOL
typedef unsigned char               MS_BOOL;

/// definition for MS_BOOL
#ifdef WIN32
#else
typedef unsigned char               BOOL;
#endif
#if !defined(__cplusplus)
#if defined(bool)
#undef bool
#endif
typedef unsigned char               bool;
#endif
/// definition for VOID
#ifdef WIN32
#else
//typedef void                        VOID;
#endif
/// definition for FILEID
//typedef s32                         FILEID;

//[TODO] use MS_U8, ... instead
// data type for 8051 code
//typedef MS_U16                      WORD;
//typedef MS_U8                       BYTE;


#ifndef true
/// definition for true
#define true                        1
/// definition for false
#define false                       0
#endif


#if !defined(TRUE) && !defined(FALSE)
/// definition for TRUE
#define TRUE                        1
/// definition for FALSE
#define FALSE                       0
#endif


#if defined(ENABLE) && (ENABLE!=1)
#warning ENALBE is not 1
#else
#define ENABLE                      1
#endif

#if defined(DISABLE) && (DISABLE!=0)
#warning DISABLE is not 0
#else
#define DISABLE                     0
#endif

#define PTR2U32(x)              (u32)(intptr_t)(x)
#define PTR2U64(x)              (u64)(intptr_t)(x)

#define LIMIT(x, low, high)     (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

#define min(a,b)                (((a) >= (b)) ? (b) : (a))
#define max(a,b)                (((a) >= (b)) ? (a) : (b))
#define sign(a)                 (((a) > 0) ? (1) : (-1))

#define BIT_GET_VALUE(v, num)   (((v) >> (num)) & 1)
#define BIT_CTRL_0(v, num)      ((v) & ~(1 << (num)))
#define BIT_CTRL_1(v, num)      ((v) | (1 << (num)))

#if defined(__cplusplus) && defined(__GNUC__) && (__GNUC__>=4) && (__GNUC_MINOR__>=6)
#define opaque_enum(e) enum e : int
#else
#define opaque_enum(e) enum e
#endif
#define typedef_opaque_enum(e1, e2) \
    opaque_enum(e1); \
    typedef enum e1 e2



//#define MSG(...) fprintf(stdout, __VA_ARGS__)

#define F_IN()  fprintf(stdout,"%s +\n",__func__);
#define F_OUT()   fprintf(stdout,"%s -\n",__func__);

#ifdef WIN32
#define DES_INIT(a, ...) __VA_ARGS__
#else
#define DES_INIT(a, ...) a = __VA_ARGS__
#endif
#endif

#if _OS_SEL_ == _LINUX_KERNEL_
/**/
#elif _OS_SEL_ == _RTK_
#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))
#define min(a,b)    (((a) >= (b))?(b):(a))
#define max(a,b)    (((a) >= (b))?(a):(b))
#endif

#endif // __MS_TYPES_H__
