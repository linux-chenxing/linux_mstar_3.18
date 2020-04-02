/*!
*********************************************************************************************************
* This product contains one or more programs protected under international and U.S. copyright laws 
* as unpublished works.  They are confidential and proprietary to Dolby Laboratories.  
* Their reproduction or disclosure, in whole or in part, or the production of derivative works therefrom 
* without the express permission of Dolby Laboratories is prohibited.  


* Copyright 2011 - 2013 by Dolby Laboratories.  All rights reserved.
*********************************************************************************************************
*/


#ifndef _FX_MATH_H_
#define _FX_MATH_H_

#include "fx_basicops.h"

S32 fxLOG2Taylor(S32 sInput,S32 sScaleOut);
S32 fxPOW2Taylor(S32 sInput,S32 sScaleIn,S32 sScaleOut);
S64 LfxPOW2Taylor(S32 sInput,S32 sScaleIn);
U32 ssqrt_s(U32 sInput);

#endif

