/*!
*********************************************************************************************************
* This product contains one or more programs protected under international and U.S. copyright laws
* as unpublished works.  They are confidential and proprietary to Dolby Laboratories.
* Their reproduction or disclosure, in whole or in part, or the production of derivative works therefrom
* without the express permission of Dolby Laboratories is prohibited.


* Copyright 2011 - 2013 by Dolby Laboratories.  All rights reserved.
*********************************************************************************************************
*/


#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_

#include <linux/types.h>
#include <linux/kernel.h>

#define INT32_MAX         INT_MAX
#define INT32_MIN         INT_MIN
#define UINT32_MAX        UINT_MAX
#define INT64_MAX         LONG_MAX
#define INT64_MIN         LONG_MIN

typedef uint64_t        U64;
typedef int64_t         S64;
typedef uint32_t        U32;
typedef int32_t         S32;
typedef uint16_t        U16;
typedef int16_t         S16;
typedef uint8_t         U8;
typedef int8_t          S8;

#endif // _TYPEDEF_H_




