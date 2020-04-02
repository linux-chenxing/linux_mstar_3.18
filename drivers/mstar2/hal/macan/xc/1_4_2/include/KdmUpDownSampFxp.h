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

#if EN_UP_DOWN_SAMPLE_OPTION

#ifndef K_DM_UP_DOWN_SAMP_FXP_H
#define K_DM_UP_DOWN_SAMP_FXP_H

#include "KdmTypeFxp.h"

extern int UYVYToYUV444InDtp(const DmKsUsFxp_t *pKsUds,
                    const DmKsFrmFmt_t *pFrmFmtUYVY, const uint8_t *pUyVy,
                    uint16_t * pY, uint16_t * pU, uint16_t * pV, int16_t rowPitchNum
                    );

extern int YUV420ToYUV444InDtp(const DmKsUsFxp_t *pKsUds,
                      const DmKsFrmFmt_t *pFrmFmt420,
                      const uint8_t *pInBuf0, const uint8_t *pInBuf1, const uint8_t *pInBuf2,
                      uint16_t * pY, uint16_t * pU, uint16_t * pV, int16_t rowPitchNum
                      );

extern int YUV444ToUYVY(const DmKsDsFxp_t *pKsUds,
                const uint16_t * pY, const uint16_t * pU, const uint16_t * pV,
                int16_t rowNum, int16_t colNum, int16_t rowPitchNum,
                const DmKsFrmFmt_t *pFrmFmtUyVy, uint8_t *pUyVy
                );

#endif // K_DM_UP_DOWN_SAMP_FXP_H
#endif // EN_UP_DOWN_SAMPLE_OPTION
