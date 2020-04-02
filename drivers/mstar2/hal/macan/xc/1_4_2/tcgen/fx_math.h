/*!
*********************************************************************************************************
* This product contains one or more programs protected under international and U.S. copyright laws 
* as unpublished works.  They are confidential and proprietary to Dolby Laboratories.  
* Their reproduction or disclosure, in whole or in part, or the production of derivative works therefrom 
* without the express permission of Dolby Laboratories is prohibited.  


* Copyright 2011 - 2015 by Dolby Laboratories.  All rights reserved.
*********************************************************************************************************
*/


#ifndef _FX_MATH_H_
#define _FX_MATH_H_

#include "fx_basicops.h"

int32_t fxLOG2Taylor(int32_t sInput,int32_t sScaleOut);
int32_t fxPOW2Taylor(int32_t sInput,int32_t sScaleIn,int32_t sScaleOut);
int64_t LfxPOW2Taylor(int32_t sInput,int32_t sScaleIn);
uint32_t ssqrt_s(uint32_t sInput);

#endif

