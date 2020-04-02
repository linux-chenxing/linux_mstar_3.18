/****************************************************************************
* This product contains one or more programs protected under international
* and U.S. copyright laws as unpublished works.  They are confidential and
* proprietary to Dolby Laboratories.  Their reproduction or disclosure, in
* whole or in part, or the production of derivative works therefrom without
* the express permission of Dolby Laboratories is prohibited.
*
*             Copyright 2011 - 2015 by Dolby Laboratories.
* 			              All rights reserved.
****************************************************************************/
#ifndef KMSBLEND_DBEDGE_FXP_H
#define KMSBLEND_DBEDGE_FXP_H

#include "VdrDmAPIpFxp.h"

// I/O data start and end in the implied buffer: CPU version in host, GPU version in device
int MsBlend(int16_t *lin, int16_t *adm, int32_t rowNum, int32_t colNum, int16_t *bld, struct DmKsFxpBack_t_ *pKsDm);

#endif //KMSBLEND_DBEDGE_FXP_H
