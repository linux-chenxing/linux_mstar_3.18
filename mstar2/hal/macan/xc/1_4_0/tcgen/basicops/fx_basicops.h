
#ifndef _FX_PRIMITIVES_H_
#define _FX_PRIMITIVES_H_
/*!
*********************************************************************************************************
* This product contains one or more programs protected under international and U.S. copyright laws
* as unpublished works.  They are confidential and proprietary to Dolby Laboratories.
* Their reproduction or disclosure, in whole or in part, or the production of derivative works therefrom
* without the express permission of Dolby Laboratories is prohibited.


* Copyright 2011 - 2013 by Dolby Laboratories.  All rights reserved.
*********************************************************************************************************
*/


#include "typedefs.h"

#define SIGN_BIT        0x80000000
#define LSIGN_BIT       0x8000000000000000LL

#define SCF(x)    ((S32) (((double)(x)*INT32_MAX)+0.5))
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
__inline static S64 lmpy_ss (S32 x, S32 y)      {return ((S64)x*y);}

/* integer addition , result in short - No saturate Protection */
__inline static S32 sadd_ss (S32 x, S32 y)      {return (x+y);}

/* integer subtraction , result in short - No saturate Protection */
__inline static S32 ssub_ss (S32 x, S32 y)      {return (x-y);}


/* Unsigned Shift Right of Short with Unsigned Input  - Saturation Protected*/
__inline static S32 sshr_ssu(S32 x, S32 y)      {
fx_assert(y>=0 , "sshr_ssu() called with negative shift value");
if(y>31) y=31;
return( x >> y);}

/* Unsigned Shift Right of Long with Unsigned Input- Saturation Protected */
__inline static S32 sshr_slu(S64 x, S32 y)      {
fx_assert(y>=0 , "sshr_slu() called with negative shift value");
if(y>63) y=63;
x = x >> y;
if(x >  INT32_MAX)   x = INT32_MAX;
if(x <  INT32_MIN)   x = INT32_MIN;

return((S32)x);}

/* Unsigned Shift Right of Long with Unsigned Input- Saturation Protected */
__inline static U32 sshr_ulu(U64 x, S32 y)      {
fx_assert(y>=0 , "sshr_ulu() called with negative shift value");
if(y>63) y=63;
x = x >> y;
if(x >  UINT32_MAX)   x = UINT32_MAX;
//if(x < 0)    x = 0;

return((U32)x);}

/* Unsigned Shift Right of Long with Unsigned Input- Saturation Protected */
__inline static U32 sshr_uls(U64 x, S32 y)      {

if (y<0)
        return(sshr_ulu(x,-y));
else
        return(sshr_ulu(x,y));

}


/* Unsigned Shift Left of Signed Variable -  Saturation Not-Protected */
__inline static S32 sshl_su(S32 a , S32 b)
{
    //if(b<0)  return 0;
    fx_assert(b>=0 , "sshl_su() called with negative shift value");
    return( a << b);
}

/* Signed Shift right of Signed Variable - Saturation Protected */
S32 sshr_sss(S32 a , S32 b);

/* Unsigned Shift right of Signed Variable - Saturation Protected */
S32 sshl_ssu(S32 a , S32 b);


/* fractional multiply  , result in short */
S32 smpy_ss(S32 a, S32 b);

/* Integer add - Saturation Protected */
S32 sadd_sss(S32 x, S32 y);

/* Integer sub - Saturation Protected */
S32 ssub_sss(S32 x, S32 y);

/* Absolute of Signed Number - Saturation Protected */
U64 uabs_ls(S64 x);

/* Absolute of Signed Number - Saturation Protected */
U32 uabs_ss(S32 x);

/* Cound Leading sign bits of a Signed number  - used in normalizing*/
U32 unorm_s(S32 a);

/* Cound Leading sign bits of a Unsigned number - used in normalizing */
U32 unorm_u(U32 a);

/* Cound Leading sign bits of a signed long number - used in normalizing */
U32 unorm_sl(S64 a);

/* Cound Leading sign bits of a Unsigned long number - used in normalizing */
U32 unorm_ul(U64 a);


/* Divide a Long Numerator by a short denominator - Result in Short - Saturation Protected */
S32 sdiv_sls(S64 a, S32 b);





#endif

