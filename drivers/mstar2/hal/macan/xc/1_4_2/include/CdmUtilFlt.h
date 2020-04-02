/****************************************************************************
* This product contains one or more programs protected under international
* and U.S. copyright laws as unpublished works.  They are confidential and
* proprietary to Dolby Laboratories.  Their reproduction or disclosure, in
* whole or in part, or the production of derivative works therefrom without
* the express permission of Dolby Laboratories is prohibited.
*
*             Copyright 2011 - 2015 by Dolby Laboratories.
*                     All rights reserved.
****************************************************************************/
#ifndef C_DM_UTIL_FLT_H
#define C_DM_UTIL_FLT_H

//#include <stdio.h>
#include "CdmTypeFlt.h"
#include "KCdmTypeFlt.h"
#include "CdmTypeFxp.h"

#define MAX2S(a_, b_)  (((a_) >= (b_)) ? (a_) : (b_))
#define MIN2S(a_, b_)  (((a_) <= (b_)) ? (a_) : (b_))
#define CLAMPS(a_, mn_, mx_)  ( ((a_) >= (mn_)) ? ( ((a_) <= (mx_)) ? (a_) : (mx_) ) : (mn_) )


#endif // C_DM_UTIL_FLT_H
