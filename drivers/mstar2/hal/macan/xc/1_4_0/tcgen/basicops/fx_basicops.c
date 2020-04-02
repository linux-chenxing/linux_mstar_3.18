/*!
*********************************************************************************************************
* This product contains one or more programs protected under international and U.S. copyright laws 
* as unpublished works.  They are confidential and proprietary to Dolby Laboratories.  
* Their reproduction or disclosure, in whole or in part, or the production of derivative works therefrom 
* without the express permission of Dolby Laboratories is prohibited.  


* Copyright 2011 - 2013 by Dolby Laboratories.  All rights reserved.
*********************************************************************************************************
*/


#include "fx_basicops.h"

/* Integer add - Saturation Protected */
 S32 sadd_sss(S32 x, S32 y)
{
	U32 ux = x;
	U32 uy = y;
	U32 res = ux + uy;
	
    /* Check for Saturation */
	ux = (ux >> 31) + INT32_MAX;
	

	if ((S32) ((ux ^ uy) | ~(uy ^ res)) >= 0)
	{
		res = ux;
	}
		
	return res;
}

/* Integer sub - Saturation Protected */
 S32 ssub_sss(S32 x, S32 y)
{
	U32 ux = x;
	U32 uy = y;
	U32 res = ux - uy;
	
   /* Check for Saturation */
	ux = (ux >> 31) + INT32_MAX;
	
	
	if ((S32)((ux ^ uy) & (ux ^ res)) < 0)
	{
		res = ux;
	}
		
	return res;
}


/* Absolute of Signed Number - Saturation Protected */
 U32 uabs_ss(S32 x)
{

	if (x >= 0) return x;/* Positive Number */
    if (x == INT32_MIN) return INT32_MAX; /* Most negative number , return the most positive number */
	return (U32)(-x);   /* Invert all other numbers */

}

/* Absolute of Signed Number - Saturation Protected */
 U64 uabs_ls(S64 x)
{

	if (x >= 0) return x;/* Positive Number */
    if (x == INT64_MIN) return INT64_MAX; /* Most negative number , return the most positive number */
	return (U64)(-x);   /* Invert all other numbers */

}

/* fractional multiply  , result in short */
 S32 smpy_ss(S32 a, S32 b){

	S64 temp;
	temp = (S64)a*b;
	return (S32)(temp>>31);
}

/* Cound Leading sign bits of a Signed number  - used in normalizing*/
U32 unorm_s(S32 a){

    U32 in;
    in = uabs_ss(a);

    return(unorm_u(in));    
}


/* Cound Leading sign bits of a Unsigned number - used in normalizing */
U32 unorm_u(U32 a){

     U32 cnt=0;
    
     if(a==0) return 0;  

    /* Count the leading sign bits by repeated shifts and comparing */       
     while(!(a & SIGN_BIT)){
       a <<= 1;        
       cnt++;
       };

       if(cnt) cnt -=1;

    return(cnt);    
}


/* Cound Leading sign bits of a Signed number  - used in normalizing*/
U32 unorm_sl(S64 a){

    U64 in;
    in = uabs_ls(a);

    return(unorm_ul(in));    
}

/* Cound Leading sign bits of a Unsigned number - used in normalizing */
U32 unorm_ul(U64 a){

     U32 cnt=0;
    
     if(a==0) return 0;  

    /* Count the leading sign bits by repeated shifts and comparing */       
     while(!(a & LSIGN_BIT)){
       a <<= 1;        
       cnt++;
       };

       if(cnt) cnt -=1;

    return(cnt);    
}


/* Unsigned Shift right of Signed Variable - Saturation Protected */
S32 sshl_ssu(S32 a , S32 b)
{
    S64 res=0;
    S32 hi;
	S32 lo ;
 	U32 res2;

    fx_assert((b & SIGN_BIT) == 0 , "Possible use of negative shift in sshl_ssu");
    fx_assert(b>=0 , "Use of negative shift value in sshl_ssu");
    
    if(b> 31) b=31;

	res2    = ((U32) (a) >> 31) + INT32_MAX;
    /* the input to 64-bit and shift tthere */
    res = (S64)a << b;	
        
    /* Split the result into high and low parts */
    hi      = (S32) (res >> 32);
    lo      = (S32) res;

    /* Check for overflow into high part and saturate*/
	if (hi != (lo >> 31)) res = res2;

    
    return((S32)res);
}


/* Signed Shift right of Signed Variable - Saturation Protected */
S32 sshr_sss(S32 a , S32 b)
{
    if(b>=0)
        /* Unsigned Shift if if b is positive */
        return(sshr_ssu( a,b));
    else
        /* invert b and unsigned shift left if if b is negative */    
        return(sshl_ssu( a,b*-1));
}

/* Divide a Long Numerator by a short denominator - Result in Short - Saturation Protected - Divide by 0 protected*/
S32 sdiv_sls(S64 a, S32 b)
{
    S64 res;
    S32 hi,lo;
    U32 res2;
    res=0;
    if(b !=0)
    {

        res = a/b;

        hi      = (S32) (res >> 32);
        lo      = (S32) res;
        res2    = ((U32) (hi) >> 31) + INT32_MAX;
        if (hi != (lo >> 31)) res = res2;
    }
    return((S32)res);
}

#if  _DEBUG
#include <stdio.h>
void fx_assert(int test,char *message){
if(!test)
 {
    printf("\nError!!!%s\n",message);
    getchar();
    //exit(-1);
 }
}
#else
void fx_assert(int test,char *message) {}
#endif









