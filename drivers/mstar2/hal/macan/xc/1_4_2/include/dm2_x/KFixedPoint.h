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
#ifndef KFIXEDPOINT_H_
#define KFIXEDPOINT_H_

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "KdmType.h"
#include "KdmTypeFxpCmn.h"

// constant LUTs. Can be hardcoded in hardware
extern const uint32_t l2pqLutA[DEF_L2PQ_LUT_NODES];
extern const uint32_t l2pqLutB[DEF_L2PQ_LUT_NODES];
extern const uint32_t l2pqLutX[DEF_L2PQ_LUT_NODES];
extern const uint32_t depq1024Lut[DEF_PQ2L_LUT_SIZE_1024];

#if (REDUCED_COMPLEXITY == 0)
#include "KdmLutG2L.h"
#include "KdmG2L.h"

extern const uint32_t depqLut[DEF_PQ2L_LUT_SIZE_1024];
#endif

// convert one pixel
void YUV2RGB(uint16_t y, uint16_t u, uint16_t v,
					int16_t m33[3][3], int32_t off[3],
					int32_t scale, int32_t clip,
					uint16_t *pR, uint16_t *pG, uint16_t *pB)
{
  int32_t i;
	int64_t iSum[3];

	// xfer by matrix, and offset
	for (i = 0; i < 3; ++i) {
		iSum[i] = (int64_t)m33[i][0]*y + (int64_t)m33[i][1]*u + (int64_t)m33[i][2]*v - off[i];
		iSum[i] += (int64_t)(1<<(scale-1));			// round
		iSum[i] = CLAMPS(iSum[i], 0, clip);	// clipping
	}

	*pR = (uint16_t)(iSum[0]>>scale);
	*pG = (uint16_t)(iSum[1]>>scale);
	*pB = (uint16_t)(iSum[2]>>scale);

}

int16_t FindNodeIdxFixPnt(uint32_t l)
{
    int16_t il = 0, num = DEF_L2PQ_LUT_NODES;// num = 2^n
    int16_t ic;
    // do a binary search
    while (num > 1) {
        num >>= 1;
        ic = il + num;
        if (l < l2pqLutX[ic]) {}
        else if (l > l2pqLutX[ic]) il = ic;
        else return ic;
    }

    return il;
}

void L2PQ(uint32_t u32_x0, uint32_t u32_x1, uint32_t u32_x2, uint16_t *u16_x0, uint16_t *u16_x1, uint16_t *u16_x2)
{
  //// L=>PQ, already >= 0
  int16_t index;

  index = FindNodeIdxFixPnt(u32_x0);
  *u16_x0 = (uint16_t)((((int64_t)(u32_x0 - l2pqLutX[index]) * l2pqLutA[index]) >> (DEF_L2PQ_LUT_X_SCALE2P + DEF_L2PQ_LUT_A_SCALE2P - DEF_L2PQ_LUT_B_SCALE2P)) + l2pqLutB[index]);

  index = FindNodeIdxFixPnt(u32_x1);
  *u16_x1 = (uint16_t)((((int64_t)(u32_x1 - l2pqLutX[index]) * l2pqLutA[index]) >> (DEF_L2PQ_LUT_X_SCALE2P + DEF_L2PQ_LUT_A_SCALE2P - DEF_L2PQ_LUT_B_SCALE2P)) + l2pqLutB[index]);

  index = FindNodeIdxFixPnt(u32_x2);
  *u16_x2 = (uint16_t)((((int64_t)(u32_x2 - l2pqLutX[index]) * l2pqLutA[index]) >> (DEF_L2PQ_LUT_X_SCALE2P + DEF_L2PQ_LUT_A_SCALE2P - DEF_L2PQ_LUT_B_SCALE2P)) + l2pqLutB[index]);
}

#if REDUCED_COMPLEXITY

// designed for DEF_G2L_LUT_SIZE lut size
#define DEF_G2L_LUT_FRAC_BIT_MASK   ((1 << (16 - DEF_G2L_LUT_SIZE_2P)) - 1)
void G2L(const uint32_t *lut, uint16_t u16_x0, uint16_t u16_x1, uint16_t u16_x2, uint32_t *u32_x0, uint32_t *u32_x1, uint32_t *u32_x2)
{
  uint32_t y_1, y_2;
  int16_t iIdx;
  int16_t frac;

  iIdx = u16_x0 >> (16 - DEF_G2L_LUT_SIZE_2P);
  frac = u16_x0 & DEF_G2L_LUT_FRAC_BIT_MASK;
  y_1 = lut[iIdx];
  if (iIdx < DEF_G2L_LUT_SIZE - 1)
  {
      y_2 = lut[iIdx + 1];
      y_1 += ((int64_t)(y_2 - y_1) * frac) >> (16 - DEF_G2L_LUT_SIZE_2P); // interpolate using LSB
  }
  else
      y_1 = lut[DEF_G2L_LUT_SIZE - 1];
  *u32_x0 = y_1;

  iIdx = u16_x1 >> (16 - DEF_G2L_LUT_SIZE_2P);
  frac = u16_x1 & DEF_G2L_LUT_FRAC_BIT_MASK;
  y_1 = lut[iIdx];
  if (iIdx < DEF_G2L_LUT_SIZE - 1)
  {
      y_2 = lut[iIdx + 1];
      y_1 += ((int64_t)(y_2 - y_1) * frac) >> (16 - DEF_G2L_LUT_SIZE_2P); // interpolate using LSB
  }
  else
      y_1 = lut[DEF_G2L_LUT_SIZE - 1];
  *u32_x1 = y_1;

  iIdx = u16_x2 >> (16 - DEF_G2L_LUT_SIZE_2P);
  frac = u16_x2 & DEF_G2L_LUT_FRAC_BIT_MASK;
  y_1 = lut[iIdx];
  if (iIdx < DEF_G2L_LUT_SIZE - 1)
  {
      y_2 = lut[iIdx + 1];
      y_1 += ((int64_t)(y_2 - y_1) * frac) >> (16 - DEF_G2L_LUT_SIZE_2P); // interpolate using LSB
  }
  else
      y_1 = lut[DEF_G2L_LUT_SIZE - 1];
  *u32_x2 = y_1;
}

// designed for 1024 lut size
void PQ2L(uint16_t u16_x0, uint16_t u16_x1, uint16_t u16_x2, uint32_t *u32_x0, uint32_t *u32_x1, uint32_t *u32_x2)
{
  uint32_t y_1, y_2;
  int16_t iIdx;
  int16_t frac;

  iIdx = u16_x0 >> 6; // 10 bits index
  frac = u16_x0 & 0x3f;
  y_1 = depq1024Lut[iIdx];
  if (iIdx != (1<<10) - 1)
  {
      y_2 = depq1024Lut[iIdx + 1];
      y_1 += ((int64_t)(y_2 - y_1) * frac) >> 6; // interpolate using LSB
  }
  *u32_x0 = y_1;

  iIdx = u16_x1 >> 6; // 10 bits index
  frac = u16_x1 & 0x3f;
  y_1 = depq1024Lut[iIdx];
  if (iIdx != (1<<10) - 1)
  {
      y_2 = depq1024Lut[iIdx + 1];
      y_1 += ((int64_t)(y_2 - y_1) * frac) >> 6; // interpolate using LSB
  }
  *u32_x1 = y_1;

  iIdx = u16_x2 >> 6; // 10 bits index
  frac = u16_x2 & 0x3f;
  y_1 = depq1024Lut[iIdx];
  if (iIdx != (1<<10) - 1)
  {
      y_2 = depq1024Lut[iIdx + 1];
      y_1 += ((int64_t)(y_2 - y_1) * frac) >> 6; // interpolate using LSB
  }
  *u32_x2 = y_1;
}

#else //#if REDUCED_COMPLEXITY

// designed for 4096 lut size
void PQ2L(uint16_t u16_x0, uint16_t u16_x1, uint16_t u16_x2, uint32_t *u32_x0, uint32_t *u32_x1, uint32_t *u32_x2)
{
  uint32_t y_1, y_2;
  int16_t iIdx;
  int16_t frac;

  iIdx = u16_x0 >> 4; // 12 bits index
  frac = u16_x0 & 0xf;
  y_1 = depqLut[iIdx];
  if (iIdx != (1<<12) - 1)
  {
      y_2 = depqLut[iIdx + 1];
      y_1 += ((int64_t)(y_2 - y_1) * frac) >> 4; // interpolate using LSB
  }
  *u32_x0 = y_1;

  iIdx = u16_x1 >> 4; // 12 bits index
  frac = u16_x1 & 0xf;
  y_1 = depqLut[iIdx];
  if (iIdx != (1<<12) - 1)
  {
      y_2 = depqLut[iIdx + 1];
      y_1 += ((int64_t)(y_2 - y_1) * frac) >> 4; // interpolate using LSB
  }
  *u32_x1 = y_1;

  iIdx = u16_x2 >> 4; // 12 bits index
  frac = u16_x2 & 0xf;
  y_1 = depqLut[iIdx];
  if (iIdx != (1<<12) - 1)
  {
      y_2 = depqLut[iIdx + 1];
      y_1 += ((int64_t)(y_2 - y_1) * frac) >> 4; // interpolate using LSB
  }
  *u32_x2 = y_1;
}

#endif //#if REDUCED_COMPLEXITY

#if REDUCED_TC_LUT

// input 12 bits unsigned; output interpolated lut data
// NOTE: lut needs 3 entries extra: duplicate edge points
//    toneCurve515LUT[0] = toneCurve515LUT[1];
//    toneCurve515LUT[513] = toneCurve515LUT[512];
//    toneCurve515LUT[514] = toneCurve515LUT[512];
uint16_t IntFilt512(uint16_t *pLut515, uint16_t lutclip, uint16_t x)
{
    uint16_t *toneCurveLUT;
    int64_t s;
    uint16_t y = 0;

    // intfilt(8,2,0.1)
    #define INT_FILT_Q  16
    #define INT8_FILT_C1  60523
    #define INT8_FILT_C2  53827
    #define INT8_FILT_C3  45833
    #define INT8_FILT_C4  36929
    #define INT8_FILT_C5  27505
    #define INT8_FILT_C6  17949
    #define INT8_FILT_C7  8651
    #define INT8_FILT_C8  0
    #define INT8_FILT_C9  -2272
    #define INT8_FILT_C10 -3638
    #define INT8_FILT_C11 -4225
    #define INT8_FILT_C12 -4161
    #define INT8_FILT_C13 -3577
    #define INT8_FILT_C14 -2602
    #define INT8_FILT_C15 -1366

    toneCurveLUT = pLut515 + (x >> 3);

    switch ((x & 0x7)) {
    case 0:
      //X0 = P0
      y = toneCurveLUT[1];
      break;
    case 1:
      //X1 =  C9*P0 + C1*P1 + C7*P2 + C15*P3
      s  = (int64_t)INT8_FILT_C9 * toneCurveLUT[0];
      s += (int64_t)INT8_FILT_C1 * toneCurveLUT[1];
      s += (int64_t)INT8_FILT_C7 * toneCurveLUT[2];
      s += (int64_t)INT8_FILT_C15 * toneCurveLUT[3];
      s += (uint32_t)0x1 << (INT_FILT_Q-1);
      y = (uint16_t)CLAMPS(s >> (INT_FILT_Q), toneCurveLUT[1], lutclip);
      break;
    case 2:
      //X2 =  C10*P0 + C2*P1 + C6*P2 + C14*P3
      s  = (int64_t)INT8_FILT_C10 * toneCurveLUT[0];
      s += (int64_t)INT8_FILT_C2 * toneCurveLUT[1];
      s += (int64_t)INT8_FILT_C6 * toneCurveLUT[2];
      s += (int64_t)INT8_FILT_C14 * toneCurveLUT[3];
      s += (uint32_t)0x1 << (INT_FILT_Q-1);
      y = (uint16_t)CLAMPS(s >> (INT_FILT_Q), toneCurveLUT[1], lutclip);
      break;
    case 3:
     //X3 =  C11*P0 + C3*P1 + C5*P2 + C13*P3
      s  = (int64_t)INT8_FILT_C11 * toneCurveLUT[0];
      s += (int64_t)INT8_FILT_C3 * toneCurveLUT[1];
      s += (int64_t)INT8_FILT_C5 * toneCurveLUT[2];
      s += (int64_t)INT8_FILT_C13 * toneCurveLUT[3];
      s += (uint32_t)0x1 << (INT_FILT_Q-1);
      y = (uint16_t)CLAMPS(s >> (INT_FILT_Q), toneCurveLUT[1], lutclip);
      break;
    case 4:
     //X4 =  C12 * (P0 + P3) + C4*(P1 + P2)
      s  = (int64_t)INT8_FILT_C12 * (toneCurveLUT[0] + toneCurveLUT[3]);
      s += (int64_t)INT8_FILT_C4 * (toneCurveLUT[1] + toneCurveLUT[2]);
      s += (uint32_t)0x1 << (INT_FILT_Q-1);
      y = (uint16_t)CLAMPS(s >> (INT_FILT_Q), toneCurveLUT[1], lutclip);
      break;
    case 5:
      //X5 =  C11*P3 + C3*P2 + C5*P1 + C13*P0
      s  = (int64_t)INT8_FILT_C11 * toneCurveLUT[3];
      s += (int64_t)INT8_FILT_C3 * toneCurveLUT[2];
      s += (int64_t)INT8_FILT_C5 * toneCurveLUT[1];
      s += (int64_t)INT8_FILT_C13 * toneCurveLUT[0];
      s += (uint32_t)0x1 << (INT_FILT_Q-1);
      y = (uint16_t)CLAMPS(s >> (INT_FILT_Q), toneCurveLUT[1], lutclip);
      break;
    case 6:
      //X6 =  C10*P3 + C2*P2 + C6*P1 + C14*P0
      s  = (int64_t)INT8_FILT_C10 * toneCurveLUT[3];
      s += (int64_t)INT8_FILT_C2 * toneCurveLUT[2];
      s += (int64_t)INT8_FILT_C6 * toneCurveLUT[1];
      s += (int64_t)INT8_FILT_C14 * toneCurveLUT[0];
      s += (uint32_t)0x1 << (INT_FILT_Q-1);
      y = (uint16_t)CLAMPS(s >> (INT_FILT_Q), toneCurveLUT[1], lutclip);
      break;
    case 7:
     //X7 =  C9*P3 + C1*P2 + C7*P1 + C15*P0
      s  = (int64_t)INT8_FILT_C9 * toneCurveLUT[3];
      s += (int64_t)INT8_FILT_C1 * toneCurveLUT[2];
      s += (int64_t)INT8_FILT_C7 * toneCurveLUT[1];
      s += (int64_t)INT8_FILT_C15 * toneCurveLUT[0];
      s += (uint32_t)0x1 << (INT_FILT_Q-1);
      y = (uint16_t)CLAMPS(s >> (INT_FILT_Q), toneCurveLUT[1], lutclip);
      break;
    default: // no way to be here
      break;
    }

    return y;
}
#endif //REDUCED_TC_LUT

#endif //KFIXEDPOINT_H_
