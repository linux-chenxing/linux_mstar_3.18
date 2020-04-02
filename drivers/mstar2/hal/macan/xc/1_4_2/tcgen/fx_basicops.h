
#ifndef _FX_PRIMITIVES_H_
#define _FX_PRIMITIVES_H_
/*!
*********************************************************************************************************
* This product contains one or more programs protected under international and U.S. copyright laws 
* as unpublished works.  They are confidential and proprietary to Dolby Laboratories.  
* Their reproduction or disclosure, in whole or in part, or the production of derivative works therefrom 
* without the express permission of Dolby Laboratories is prohibited.  


* Copyright 2011 - 2015 by Dolby Laboratories.  All rights reserved.
*********************************************************************************************************
*/


//#include <stdio.h>
//#include <stdlib.h>
//#include <stdint.h>
#include "dolbyTypeToMstarType.h"
#define SIGN_BIT        0x80000000
#define LSIGN_BIT       0x8000000000000000LL

#define SCF(x)    ((int32_t) (((double)(x)*INT32_MAX)+0.5))  
void fx_assert(int,char *);


/* Function Naming Convention Examples*/
/* sFx_ss(a)    - Signed Output , a argument is Signed , Saturation Protected */
/* sFx_ss(a,b)  - Signed Output , a & b argument is Signed - No Saturation Protection if occurs*/
/* sFx_sss(a,b) - Signed Output , a & b argument is Signed , Saturation Protected*/
/* uFx_suu(a,b) - Unsigned Output , a & b argument is Unsigned , Saturation Protected*/


/* min of two  variables , result in short */
#define sminss(a,b)     ((a)<(b) ? (a) : (b))

/* max of two  variables , result in short */
#define smaxss(a,b)     ((a)>(b) ? (a) : (b))

/* integer multiply  , result in long */
__inline static int64_t lmpy_ss (int32_t x, int32_t y)      {return ((int64_t)x*y);}

/* integer addition , result in short - No saturate Protection */
__inline static int32_t sadd_ss (int32_t x, int32_t y)      {return (x+y);}

/* integer subtraction , result in short - No saturate Protection */
__inline static int32_t ssub_ss (int32_t x, int32_t y)      {return (x-y);}


/* Unsigned Shift Right of Short with Unsigned Input  - Saturation Protected*/
__inline static int32_t sshr_ssu(int32_t x, int32_t y)      {
fx_assert(y>=0 , "sshr_ssu() called with negative shift value");
if(y>31) y=31;
return( x >> y);}

/* Unsigned Shift Right of Long with Unsigned Input- Saturation Protected */
__inline static int32_t sshr_slu(int64_t x, int32_t y)      {
fx_assert(y>=0 , "sshr_slu() called with negative shift value");
if(y>63) y=63;
x = x >> y;
if(x >  INT32_MAX)   x = INT32_MAX;
if(x <  INT32_MIN)   x = INT32_MIN;

return((int32_t)x);}

/* Unsigned Shift Right of Long with Unsigned Input- Saturation Protected */
__inline static uint32_t sshr_ulu(uint64_t x, int32_t y)      {
fx_assert(y>=0 , "sshr_ulu() called with negative shift value");
if(y>63) y=63;
x = x >> y;
if(x >  UINT32_MAX)   x = UINT32_MAX;
//if(x < 0)    x = 0;

return((uint32_t)x);}

/* Unsigned Shift Right of Long with Unsigned Input- Saturation Protected */
__inline static uint32_t sshr_uls(uint64_t x, int32_t y)      {

if (y<0)
        return(sshr_ulu(x,-y));
else
        return(sshr_ulu(x,y));

}


/* Unsigned Shift Left of Signed Variable -  Saturation Not-Protected */
__inline static int32_t sshl_su(int32_t a , int32_t b)
{   
    //if(b<0)  return 0;
    fx_assert(b>=0 , "sshl_su() called with negative shift value");
    return( a << b);
}

/* Signed Shift right of Signed Variable - Saturation Protected */
int32_t sshr_sss(int32_t a , int32_t b);

/* Unsigned Shift right of Signed Variable - Saturation Protected */
int32_t sshl_ssu(int32_t a , int32_t b);


/* fractional multiply  , result in short */
int32_t smpy_ss(int32_t a, int32_t b);

/* Integer add - Saturation Protected */
int32_t sadd_sss(int32_t x, int32_t y);

/* Integer sub - Saturation Protected */
int32_t ssub_sss(int32_t x, int32_t y);

/* Absolute of Signed Number - Saturation Protected */
uint64_t uabs_ls(int64_t x);

/* Absolute of Signed Number - Saturation Protected */
uint32_t uabs_ss(int32_t x);

/* Cound Leading sign bits of a Signed number  - used in normalizing*/
uint32_t unorm_s(int32_t a);

/* Cound Leading sign bits of a Unsigned number - used in normalizing */
uint32_t unorm_u(uint32_t a);

/* Cound Leading sign bits of a signed long number - used in normalizing */
uint32_t unorm_sl(int64_t a);

/* Cound Leading sign bits of a Unsigned long number - used in normalizing */
uint32_t unorm_ul(uint64_t a);


/* Divide a Long Numerator by a short denominator - Result in Short - Saturation Protected */
int32_t sdiv_sls(int64_t a, int32_t b);





#endif

