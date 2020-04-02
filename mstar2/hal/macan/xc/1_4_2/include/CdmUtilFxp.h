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
#ifndef C_DM_UTIL_FXP_H
#define C_DM_UTIL_FXP_H

#include "CdmTypeFxp.h"
#include "CdmTypeFlt.h"

#define MAX2S(a_, b_)  (((a_) >= (b_)) ? (a_) : (b_))
#define MIN2S(a_, b_)  (((a_) <= (b_)) ? (a_) : (b_))
#define CLAMPS(a_, mn_, mx_)  ( ((a_) >= (mn_)) ? ( ((a_) <= (mx_)) ? (a_) : (mx_) ) : (mn_) )

#if defined(c_plusplus) || defined(__cplusplus)
extern "C"
{
#endif

void GetYuvRgbOffFxp(CRng_t rng, int32_t bits, int32_t v3YuvRgbOff[3]);


// to avoid c lib memcpy
void MemCpyByte(void *pDst, const void *pSrc, int len);
// to avoid c lib memcmp
int MemEqByte(const void *pD1, const void *pD2, int len);

// assign matrix and vector
void AssignM33I322I(const int32_t m33In[3][3], int16_t m33Out[3][3]);
void AssignM33I2I(const int16_t m33In[3][3], int16_t m33Out[3][3]);
void AssignV3I2I(const int32_t v3In[3], int32_t v3Out[3]);

int16_t Adjust64M33ScaleTo16M33(int64_t inM33[3][3], int16_t scale, int16_t outM33[3][3]);
int16_t InvM33(int16_t in_mat[3][3], int16_t out_mat[3][3], int16_t in_scale, int16_t out_scale);

#if DM_FULL_SRC_SPEC || DM_VER_HIGHER_THAN211 || DM_SEC_INPUT
int16_t GetRgb2YuvM33Fxp(CYuvXferSpec_t yuvXferSpec, int16_t m33Rgb2Yuv[3][3]);
int16_t GetRgb2YuvM33NarrowFxp(CYuvXferSpec_t yuvXferSpec, int bits, int16_t m33Rgb2Yuv[3][3]);
int16_t GetRgb2WpLmsCtM33Fxp(CRgbDef_t rgbDef, const int32_t v3Wp[3], int16_t wpScale, int32_t crossTalk, int16_t m33Rgb2WpCtLms[3][3]);
int16_t GetWpLmsCt2RgbM33Fxp(CRgbDef_t rgbDef, const int32_t v3Wp[3], int16_t wpScale, int32_t crossTalk, int16_t m33WpCtLms2Rgb[3][3]);
#endif

#if DM_FULL_SRC_SPEC || DM_SEC_INPUT
int16_t GetYuv2RgbM33Fxp(CYuvXferSpec_t yuvXferSpec, int16_t m33Yuv2Rgb[3][3]);
int16_t GetYuv2RgbM33NarrowFxp(CYuvXferSpec_t yuvXferSpec, int bits, int16_t m33Yuv2Rgb[3][3]);
#endif

int16_t GetRgb2LmsWpByDefV3Fxp(CRgbDef_t rgbDef, int32_t v3Wp[3]);
void GetRgb2LmsWpV3(int16_t m33Rgb2Lms[3][3], int32_t v3Wp[3]);
int16_t GetRgb2LmsByPrimsM33Fxp(int32_t rx, int32_t ry, int32_t gx, int32_t gy, int32_t bx, int32_t by,
    int32_t wx, int32_t wy, int16_t m33Rgb2Lms[3][3]);
int16_t GetRgb2LmsByDefM33(CRgbDef_t rgbDef, int16_t m33Rgb2Lms[3][3]);
int16_t GetRgb2LmsCtWpM33Fxp(int16_t m33[3][3], int16_t m33Scale, int32_t crossTalk, const int32_t v3Wp[3], int16_t wpScale, int16_t m33Rgb2Lms[3][3]);

int16_t GetRgb2WpLmsM33Fxp(int16_t m33[3][3], int16_t m33Scale, const int32_t v3Wp[3], int16_t wpScale, int16_t m33Rgb2WpLms[3][3]);
int16_t GetLms2RgbCtWpM33Fxp(int16_t m33[3][3], int16_t m33Scale, int32_t crossTalk, const int32_t v3Wp[3], int16_t wpScale, int16_t m33Lms2Rgb[3][3]);
int16_t GetLms2RgbByDefM33Fxp(CRgbDef_t rgbDef, int16_t m33Lms2Rgb[3][3]);
int16_t GetLms2RgbByPrimsM33Fxp(int32_t rx, int32_t ry, int32_t gx, int32_t gy, int32_t bx, int32_t by, int32_t wx, int32_t wy, int16_t m33Lms2Rgb[3][3]);

int16_t GetLms2IptM33Fxp(int16_t m33Lms2Ipt[3][3]);
int16_t GetIpt2LmsM33Fxp(int16_t m33Ipt2Lms[3][3]);


int32_t powTaylor(uint32_t in, uint32_t exp);
void GenerateL2GLut(uint32_t gammaR, uint64_t *lutA, uint16_t *lutB, uint32_t *lutX);

#if defined(c_plusplus) || defined(__cplusplus)
}
#endif

#endif // C_DM_UTIL_FLT_H
