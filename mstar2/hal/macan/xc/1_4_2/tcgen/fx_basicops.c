/*!
*********************************************************************************************************
* This product contains one or more programs protected under international and U.S. copyright laws 
* as unpublished works.  They are confidential and proprietary to Dolby Laboratories.  
* Their reproduction or disclosure, in whole or in part, or the production of derivative works therefrom 
* without the express permission of Dolby Laboratories is prohibited.  


* Copyright 2011 - 2015 by Dolby Laboratories.  All rights reserved.
*********************************************************************************************************
*/


#include "fx_basicops.h"

/* Integer add - Saturation Protected */
 int32_t sadd_sss(int32_t x, int32_t y)
{
	uint32_t ux = x;
	uint32_t uy = y;
	uint32_t res = ux + uy;
	
    /* Check for Saturation */
	ux = (ux >> 31) + INT32_MAX;
	

	if ((int32_t) ((ux ^ uy) | ~(uy ^ res)) >= 0)
	{
		res = ux;
	}
		
	return res;
}

/* Integer sub - Saturation Protected */
 int32_t ssub_sss(int32_t x, int32_t y)
{
	uint32_t ux = x;
	uint32_t uy = y;
	uint32_t res = ux - uy;
	
   /* Check for Saturation */
	ux = (ux >> 31) + INT32_MAX;
	
	
	if ((int32_t)((ux ^ uy) & (ux ^ res)) < 0)
	{
		res = ux;
	}
		
	return res;
}


/* Absolute of Signed Number - Saturation Protected */
 uint32_t uabs_ss(int32_t x)
{

	if (x >= 0) return x;/* Positive Number */
    if (x == INT32_MIN) return INT32_MAX; /* Most negative number , return the most positive number */
	return (uint32_t)(-x);   /* Invert all other numbers */

}

/* Absolute of Signed Number - Saturation Protected */
 uint64_t uabs_ls(int64_t x)
{

	if (x >= 0) return x;/* Positive Number */
    if (x == INT64_MIN) return INT64_MAX; /* Most negative number , return the most positive number */
	return (uint64_t)(-x);   /* Invert all other numbers */

}

/* fractional multiply  , result in short */
 int32_t smpy_ss(int32_t a, int32_t b){

	int64_t temp;
	temp = (int64_t)a*b;
	return (int32_t)(temp>>31);
}

/* Cound Leading sign bits of a Signed number  - used in normalizing*/
uint32_t unorm_s(int32_t a){

    uint32_t in;
    in = uabs_ss(a);

    return(unorm_u(in));    
}


/* Cound Leading sign bits of a Unsigned number - used in normalizing */
uint32_t unorm_u(uint32_t a){

     uint32_t cnt=0;
    
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
uint32_t unorm_sl(int64_t a){

    uint64_t in;
    in = uabs_ls(a);

    return(unorm_ul(in));    
}

/* Cound Leading sign bits of a Unsigned number - used in normalizing */
uint32_t unorm_ul(uint64_t a){

     uint32_t cnt=0;
    
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
int32_t sshl_ssu(int32_t a , int32_t b)
{
    int64_t res=0;
    int32_t hi;
	int32_t lo ;
 	uint32_t res2;

    fx_assert((b & SIGN_BIT) == 0 , "Possible use of negative shift in sshl_ssu");
    fx_assert(b>=0 , "Use of negative shift value in sshl_ssu");
    
    if(b> 31) b=31;

	res2    = ((uint32_t) (a) >> 31) + INT32_MAX;
    /* the input to 64-bit and shift tthere */
    res = (int64_t)a << b;	
        
    /* Split the result into high and low parts */
    hi      = (int32_t) (res >> 32);
    lo      = (int32_t) res;

    /* Check for overflow into high part and saturate*/
	if (hi != (lo >> 31)) res = res2;

    
    return((int32_t)res);
}


/* Signed Shift right of Signed Variable - Saturation Protected */
int32_t sshr_sss(int32_t a , int32_t b)
{
    if(b>=0)
        /* Unsigned Shift if if b is positive */
        return(sshr_ssu( a,b));
    else
        /* invert b and unsigned shift left if if b is negative */    
        return(sshl_ssu( a,b*-1));
}

/* Divide a Long Numerator by a short denominator - Result in Short - Saturation Protected - Divide by 0 protected*/
int32_t sdiv_sls(int64_t a, int32_t b)
{
    int64_t res;
    int32_t hi,lo;
    uint32_t res2;
    res=0;
    if(b !=0)
    {

        res = a/b;

        hi      = (int32_t) (res >> 32);
        lo      = (int32_t) res;
        res2    = ((uint32_t) (hi) >> 31) + INT32_MAX;
        if (hi != (lo >> 31)) res = res2;
    }
    return((int32_t)res);
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









