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
#include "KCdmModCtrl.h"

#if EN_MS_OPTION

#ifndef K_DM_MS_BLEND_FXP_H
#define K_DM_MS_BLEND_FXP_H

#include "KdmTypeFlt.h"

extern int MsBlend(const DmKsMsFxp_t *pMsBlend, int16_t * const lin, int16_t * const adm,
               int32_t rowNum, int32_t colNum, int32_t rowPitchNum, int16_t *bld
            );

#endif // K_DM_MS_BLEND_FXP_H
#endif // EN_MS_OPTION
