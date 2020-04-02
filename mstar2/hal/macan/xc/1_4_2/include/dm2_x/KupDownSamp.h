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
#ifndef UP_DOWN_SAMP_H
#define UP_DOWN_SAMP_H

//#include <stdint.h>
#include "KdmType.h"
#include "dolbyTypeToMstarType.h"
int	YUV420ToYUV444(const DmKsFrmFmt_t *pFrmFmt420,
		const uint16_t *pInBuf0, const uint16_t *pInBuf1, const uint16_t *pInBuf2,
		uint16_t * pY, uint16_t * pU, uint16_t * pV, int32_t rowPitchNum);
int UYVYToYUV444(const DmKsFrmFmt_t *pFrmFmtUYVY, const uint16_t *pUyVy,
		uint16_t * pY, uint16_t * pU, uint16_t * pV, int32_t rowPitchNum);
int	YUV444ToUYVY(const uint16_t * pY, const uint16_t * pU, const uint16_t * pV,
		int32_t rowNum, int32_t colNum, int32_t rowPitchNum,
		const DmKsFrmFmt_t *pFrmFmtUyVy, uint8_t *pUyVy);

#endif // UP_DOWN_SAMP_H
