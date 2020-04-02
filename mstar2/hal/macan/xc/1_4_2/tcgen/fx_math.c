/*!
*********************************************************************************************************
* This product contains one or more programs protected under international and U.S. copyright laws 
* as unpublished works.  They are confidential and proprietary to Dolby Laboratories.  
* Their reproduction or disclosure, in whole or in part, or the production of derivative works therefrom 
* without the express permission of Dolby Laboratories is prohibited.  


* Copyright 2011 - 2015 by Dolby Laboratories.  All rights reserved.
*********************************************************************************************************
*/


#include "fx_math.h"

/* Log 2 - Taylor Series Method
 * Implementation Notes:
 * y = log(2) * ln(x) with x = M(x)*2^P(x) = M*2^P
 * y = log(2) * (ln(M) + ln(2)*P)
 * y = log(2) * (ln(2*M) + (P−1)*ln(2))
 * y = log(2) * (ln((2*M−1)+1) + (P−1)*ln(2))
 * y = log(2) * (f(2*M−1) + (P−1)*ln(2))
 * with f(u) = ln(1+u).
 * We use a polynomial approximation for f(u) :
 * f(u) = (((((C6*u+C5)*u+C4)*u+C3)*u+C2)*u+C1)*u+C0
 * for 0<= u <= 1.
 * The polynomial coefficients Ci are as follows:
 * Scale = 1/(2*log(2));
 * C0 = 0.000001472  * Scale
 * C1 = 0.999847766  * Scale
 * C2 = −0.497373368 * Scale
 * C3 = 0.315747760  * Scale
 * C4 = −0.190354944 * Scale
 * C5 = 0.082691584  * Scale
 * C6 = −0.017414144 * Scale

 * Usage : Output = log2(Input) * (1<<sScaleOut)
 */
int32_t fxLOG2Taylor(int32_t sInput, int32_t sScaleOut)
{
// Taylor Series Method
#define LOG2_C6 -26975937
#define LOG2_C5  128096045
#define LOG2_C4 -294875418
#define LOG2_C3  489119173
#define LOG2_C2 -770472134
#define LOG2_C1  1548846182
#define LOG2_C0  2280


    int32_t sNorm,ltemp,lIn;

    fx_assert(sInput>=0,"Error!!!Input to fxLOG2Taylor() cannot be negative");
    
    if(sInput==0)
        return(1<<sScaleOut);

    /* Normalize the input to utilize full dynamic range */
    sNorm  = unorm_s(sInput);
    ltemp  = sshl_su(sInput,sNorm);
    
    // 2*M-1
    ltemp =  ltemp << 2;
    lIn   =  (uint32_t)ltemp >> 1;
    
    // C5 + C6*x
    ltemp = smpy_ss(lIn,LOG2_C6); 
    ltemp = sadd_ss(ltemp,LOG2_C5);   
     
    //C4 + (C6*x + C5)*x
    ltemp = smpy_ss(lIn,ltemp);
    ltemp = sadd_ss(ltemp,LOG2_C4);   
    
    //C3 + (C6*x^2 + C5*x + C4)*x
    ltemp = smpy_ss(lIn,ltemp);
    ltemp = sadd_ss(ltemp,LOG2_C3);   
    
    //C2 + (C6*x^3 + C5*x^2 + C4*x + C3)*x
    ltemp = smpy_ss(lIn,ltemp);
    ltemp = sadd_ss(ltemp,LOG2_C2);   
    
    //C1 + (C6*x^4 +C5*x^5 + C4*x^2 + C3*x +C2)*x
    ltemp = smpy_ss(lIn,ltemp);
    ltemp = sadd_ss(ltemp,LOG2_C1);  
    
    //C0 + (C6*x^5 + C5*x^4 + C4*x^3 + C3*x^2 + C2*x +C1)*x
    ltemp = smpy_ss(lIn,ltemp);
    ltemp = sadd_ss(ltemp,LOG2_C0);  
    
    /* Compensate for the input normalizing */
    ltemp =  sshr_ssu(ltemp,31-sScaleOut-1);
    sNorm  = sshl_su(31-sNorm-1, sScaleOut);
    
    return sadd_ss(sNorm,ltemp);
}


/* Power 2- taylor Series 

    C0 = 0.5000000000; 
    C1 = 0.3465742469; 
    C2 = 0.1201133728; 
    C3 = 0.0277463794; 
    C4 = 0.0048083663; 
    C5 = 0.0006790758; 
    C6 = 0.0000782609; 


 * Usage : Output = 2^(sInput/(1<<sScaleIn)) * (1<<sScaleOut)
  
 */
int32_t fxPOW2Taylor(int32_t sInput,int32_t sScaleIn,int32_t sScaleOut)
{

#define POW2_C6  168064
#define POW2_C5  1458304
#define POW2_C4  10325888
#define POW2_C3  59584896
#define POW2_C2  257941504
#define POW2_C1  744262528
#define POW2_C0  1073741824

    int32_t sIntPart,sFracPart,lIn,ltemp;

    // integer portion
    sIntPart = sshr_ssu(sInput,sScaleIn);
    sIntPart = sIntPart + 1;
    
    // fractional part
    sFracPart = sInput << (32-sScaleIn);
    lIn       = (uint32_t)(sFracPart) >> 1;

    // C5 + C6*x
    ltemp = smpy_ss(lIn,POW2_C6); 
    ltemp = sadd_ss(ltemp,POW2_C5);   
     
    //C4 + (C6*x + C5)*x
    ltemp = smpy_ss(lIn,ltemp);
    ltemp = sadd_ss(ltemp,POW2_C4);   
    
    //C3 + (C6*x^2 + C5*x + C4)*x
    ltemp = smpy_ss(lIn,ltemp);
    ltemp = sadd_ss(ltemp,POW2_C3);
    
    //C2 + (C6*x^3 + C5*x^2 + C4*x + C3)*x
    ltemp = smpy_ss(lIn,ltemp);
    ltemp = sadd_ss(ltemp,POW2_C2);
    
    //C1 + (C6*x^4 + C5*x^5 + C4*x^2 + C3*x + C2)*x
    ltemp = smpy_ss(lIn,ltemp);
    ltemp = sadd_ss(ltemp,POW2_C1);  
    
    //C0 + (C6*x^5 + C5*x^4 + C4*x^3 + C3*x^2 + C2*x + C1)*x
    ltemp = smpy_ss(lIn,ltemp);
    ltemp = sadd_ss(ltemp,POW2_C0);  
    
    return(sshr_sss(ltemp,31-sIntPart-sScaleOut));
}


/* Power 2- taylor Series 

    C0 = 0.5000000000; 
    C1 = 0.3465742469; 
    C2 = 0.1201133728; 
    C3 = 0.0277463794; 
    C4 = 0.0048083663; 
    C5 = 0.0006790758; 
    C6 = 0.0000782609; 


 * Usage : Output = 2^(sInput/(1<<sScaleIn)) * 2^32
  
 */
int64_t LfxPOW2Taylor(int32_t sInput,int32_t sScaleIn)
{

#define POW2_C6  168064
#define POW2_C5  1458304
#define POW2_C4  10325888
#define POW2_C3  59584896
#define POW2_C2  257941504
#define POW2_C1  744262528
#define POW2_C0  1073741824


    int32_t sIntPart,sFracPart,lIn,ltemp;
    int64_t llresult;


    // integer portion
    sIntPart = sshr_ssu(sInput,sScaleIn);
    sIntPart = sIntPart + 1;
    
    // fractional part
    sFracPart = sInput << (32-sScaleIn);
    lIn       = (uint32_t)(sFracPart) >> 1;

    // C5 + C6*x
    ltemp = smpy_ss(lIn,POW2_C6); 
    ltemp = sadd_ss(ltemp,POW2_C5);   
     
    //C4 + (C6*x + C5)*x
    ltemp = smpy_ss(lIn,ltemp);
    ltemp = sadd_ss(ltemp,POW2_C4);   
    
    //C3 + (C6*x^2 + C5*x + C4)*x
    ltemp = smpy_ss(lIn,ltemp);
    ltemp = sadd_ss(ltemp,POW2_C3);
    
    //C2 + (C6*x^3 + C5*x^2 + C4*x + C3)*x
    ltemp = smpy_ss(lIn,ltemp);
    ltemp = sadd_ss(ltemp,POW2_C2);
    
    //C1 + (C6*x^4 + C5*x^5 + C4*x^2 + C3*x + C2)*x
    ltemp = smpy_ss(lIn,ltemp);
    ltemp = sadd_ss(ltemp,POW2_C1);  
    
    //C0 + (C6*x^5 + C5*x^4 + C4*x^3 + C3*x^2 + C2*x + C1)*x
    ltemp = smpy_ss(lIn,ltemp);
    ltemp = sadd_ss(ltemp,POW2_C0);  
    llresult = (int64_t)ltemp;

    ltemp = 32-(31-sIntPart);
    if(ltemp>0)
        llresult <<= ltemp;
    else        
        llresult >>= (31-sIntPart)-32;
    
    return(llresult);
}

/* Sqrt 
 * Usage : Output = sqrt(2/2^31) * 2^31
 */
uint32_t ssqrt_s(uint32_t uInput){


// Input  0 to 1 in Q31
// Output 0 to 1 in Q31

//if Other Q input is used , this has to be compensated at the output 
// for ease all use odd Q as input 
// For Ex in In is in Q21
// Out = Out * 2^((31-21)/2)

    #define SQRT_C0      (int64_t)247744756 //SCF(0.1153651422)
    #define SQRT_C1      (int64_t)1386910770 //SCF(0.6458306544)     
    #define SQRT_C2      (int64_t)-957104405 //SCF(-0.4456864692)    
    #define SQRT_C3      (int64_t)521259316 //SCF(0.2427302840)     
    #define SQRT_C4      (int64_t)-125079372 //SCF(-0.0582446212)
    #define SQRT_HALF    (int64_t)1518500250 //SCF(0.707106781186548)

    int32_t input_scale,input_mult,iscaleO2,output;
    uint32_t input_norm;
    int64_t Acc;

    input_scale  = unorm_u(uInput);
    input_norm   = sshl_su(uInput,input_scale);

    //+= x^0 * C0
    Acc = SQRT_C0 << 31;
    
    //+= x^1 *C1
    Acc = Acc + input_norm * SQRT_C1;
    
    //+= x^2 *C2
    input_mult = smpy_ss(input_norm,input_norm);
    Acc = Acc + input_mult * SQRT_C2;
    
    //+= x^3 * C3
    input_mult = smpy_ss(input_mult,input_norm);
    Acc = Acc + input_mult * SQRT_C3;
    
    //+= x^4 * C4
    input_mult = smpy_ss(input_mult,input_norm);
    Acc = Acc + input_mult * SQRT_C4;
    
	  iscaleO2 = (input_scale>>1);

    output = (int32_t) sshr_slu(Acc,iscaleO2+30);
	
    // When Input norm is odd , then compensate with 1/sqrt(2)
    if(input_scale&1)         output = smpy_ss(output, SQRT_HALF);

    return(output);
}



