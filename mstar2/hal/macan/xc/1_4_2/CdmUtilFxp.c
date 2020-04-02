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

//#include <stdio.h>
//#include <math.h> // for pow, exp, floor only
#include <linux/string.h>
#include <linux/ctype.h>  // for tolower only
//#include <assert.h>
#include "tcgen/fx_basicops.h"
#include "tcgen/fx_math.h"
#include "CdmUtilFxp.h"
#include "KdmTypeFxpCmn.h"
#include "KdmTypeFxp.h"
#include "dolbyTypeToMstarType.h"

#include "dm2_x/VdrDmAPIpFxp.h"
extern void DM2xAdaptor(struct DmKsFxp_t_ *pKs, struct DmKsFxpBack_t_ *pKsBack);
extern void DM2xLoad3DLut(struct DmKsFxp_t_ *pKs, struct DmKsFxpBack_t_ *pKsBack);
#if REDUCED_COMPLEXITY == 0
extern void DM286Adaptor(struct DmKsFxp_t_ *pKs, struct DmExecFxp286_t_ *pKs286);
extern uint16_t* DM286AdaptorTCLut(struct DmKsFxp_t_ *pKs);
void DM2xLoad3DLut286(struct DmKsFxp_t_ *pKs, struct DmExecFxp286_t_ *pKs286);
#endif


int16_t M33TimesM33(int32_t m33a[3][3], int16_t scale_a, int32_t m33b[3][3], int16_t scale_b, int16_t m33[3][3])
{
  int32_t acc;
  int16_t i, j;
  int16_t scale_out;

  scale_out = scale_b; // not changing scale at moment

  for (i = 0; i < 3; ++i) {
    for (j = 0; j < 3; ++j) {
//      acc = ((m33a[i][0]*m33b[0][j] + m33a[i][1]*m33b[1][j] + m33a[i][2]*m33b[2][j]) + (1 << (scale_a - 1))) >> (scale_a + scale_b - scale_out);
      acc = (m33a[i][0]*m33b[0][j] + m33a[i][1]*m33b[1][j] + m33a[i][2]*m33b[2][j]) >> (scale_a + scale_b - scale_out);
      m33[i][j] = (int16_t)(acc);
    }
  }
  return scale_out;
}

void Yuv2RgbOffsetRgbFxp(int16_t m33[3][3], uint32_t yOff, uint32_t uvOff, int32_t rgbOff[3])
{
  rgbOff[0] = ((int64_t)m33[0][0]*yOff + (int64_t)(m33[0][1] + (int32_t)m33[0][2])*uvOff) >> 16;
  rgbOff[1] = ((int64_t)m33[1][0]*yOff + (int64_t)(m33[1][1] + (int32_t)m33[1][2])*uvOff) >> 16;
  rgbOff[2] = ((int64_t)m33[2][0]*yOff + (int64_t)(m33[2][1] + (int32_t)m33[2][2])*uvOff) >> 16;
}

void GetYuvRgbOffFxp(CRng_t rng, int bits, int32_t v3YuvRgbOff[3])
{
  v3YuvRgbOff[0] = ((rng == CRngNarrow) ? (1<<(bits - 4)) : 0);
  v3YuvRgbOff[1] = v3YuvRgbOff[2] = (1<<(bits - 1));
}


void MemCpyByte(void *pDst, const void *pSrc, int len)
{
  char *pD = (char *)pDst;
  const char *pS = (char *)pSrc;
  const char *pSE = pS + len;


  while (pS != pSE) *pD++ = *pS++;
}

int MemEqByte(const void *pD1, const void *pD2, int len)
{
  const char *pB1 = (char *)pD1;
  const char *pB2 = (char *)pD2;
  const char *pB1E = pB1 + len;


  while (pB1 != pB1E && *pB1 == *pB2) {
    ++pB1;
    ++pB2;
  }

  return pB1 == pB1E;
}

int32_t abs_int32(int32_t x)
{
  if (x >= 0) return x;
    if (x == INT32_MIN) return INT32_MAX;
  return (-x);
}

int64_t abs_int64(int64_t x)
{
  if (x >= 0) return x;
    if (x == INT64_MIN) return INT64_MAX;
  return (-x);
}


static int16_t FindInt64M33Precision(int64_t pm33[3][3])
{
  int16_t i, j, precision;
  int64_t maxc = 0;

  for (i=0; i<3; i++) {
    for (j=0; j<3; j++) {
      if (abs_int64(pm33[i][j]) > maxc)
        maxc = abs_int64(pm33[i][j]);
    }
  }

  for (precision = 0; precision < 64; precision++) {
    if (((int64_t)1 << precision) >= maxc) {
      break;
    }
  }
  return precision;
}

int16_t Adjust64M33ScaleTo16M33(int64_t inM33[3][3], int16_t scale, int16_t outM33[3][3])
{
  int16_t i, j;
  int16_t precision = FindInt64M33Precision(inM33);

  if (precision > 15) { // output to precision 15 if bigger than 15
    for (i = 0; i < 3; i++) {
      for (j = 0; j < 3; j++) {
        outM33[i][j] = (int16_t)((inM33[i][j] >> (precision - 15)));
      }
    }
    scale = scale - (precision - 15);
  }
  else { // let precision as it is if less than 15
    for (i = 0; i < 3; i++) {
      for (j = 0; j < 3; j++) {
        outM33[i][j] = (int16_t)inM33[i][j];
      }
    }
  }

  return scale;
}

int16_t InvM33(int16_t in_mat[3][3], int16_t out_mat[3][3], int16_t in_scale, int16_t out_scale)
{
  int16_t i,j;
  int64_t x, det=0;
  int32_t maxPos = 0, minNeg = -1;
  int32_t mat[3][3];
  int16_t rShift = 0;

  for (i=0; i<3; i++) {
    det = det + (in_mat[0][i]*(in_mat[1][(i+1)%3]*(int64_t)in_mat[2][(i+2)%3] -
                   in_mat[1][(i+2)%3]*(int64_t)in_mat[2][(i+1)%3]));
  }

  if (det == 0)
    return -1; // not invertible

  for (i=0; i<3; i++) {
    for (j=0; j<3; j++) {
      x = (in_mat[(i+1)%3][(j+1)%3] * (int64_t)in_mat[(i+2)%3][(j+2)%3]) * ((int64_t)1 << (in_scale + out_scale + 3)) -
        (in_mat[(i+1)%3][(j+2)%3] * (int64_t)in_mat[(i+2)%3][(j+1)%3]) * ((int64_t)1 << (in_scale + out_scale + 3));

      x = (((x / det) >> 2) + 1) >> 1;

      mat[j][i] = (int32_t)x;

      maxPos = (mat[j][i] > maxPos) ? mat[j][i] : maxPos;
      minNeg = (mat[j][i] < minNeg) ? mat[j][i] : minNeg;
    }
  }

  while (maxPos > 0x7fff || minNeg < -(int)0x8000) {
    maxPos >>= 1;
    minNeg >>= 1;
    ++rShift;
  }

  if (rShift > 0) {
    for (i = 0; i < 3; ++i) {
      for (j = 0; j < 3; ++j) {
        // RISK: rounding may overflow the expected scale but chance is so low. I opt to get precision first.
        out_mat[i][j] = (int16_t)(((mat[i][j] >> (rShift - 1)) + 1) >> 1);
      }
    }
  }
  else {
    for (i = 0; i < 3; ++i)
      for (j = 0; j < 3; ++j)
        out_mat[i][j] = (int16_t)mat[i][j];
  }

  return rShift;
}

/* M33TimesM33 auto scale version */
/* return output scale */
/* caveat: no overflow detection, input scales shouldn't be too close to 31 if m33 > 1.0 */
static int16_t M33TimesM33AS(const int32_t m33a[3][3], int16_t scale_a, const int32_t m33b[3][3], int16_t scale_b, int16_t m33[3][3])
{
  int16_t i, j;
  int16_t scale_out;
  int64_t m33Temp[3][3];

  if (scale_a + scale_b >= 31) { // scale to 31, let 1 sign bit
    scale_out = 31;
    for (i = 0; i < 3; ++i) {
      for (j = 0; j < 3; ++j) {
        m33Temp[i][j] = ((int64_t)m33a[i][0]*m33b[0][j] + (int64_t)m33a[i][1]*m33b[1][j] + (int64_t)m33a[i][2]*m33b[2][j])
            >> (scale_a + scale_b - scale_out);
      }
    }
  }
  else {  // scale as two inputs
    for (i = 0; i < 3; ++i) {
      for (j = 0; j < 3; ++j) {
        m33Temp[i][j] = (int64_t)m33a[i][0]*m33b[0][j] + (int64_t)m33a[i][1]*m33b[1][j] + (int64_t)m33a[i][2]*m33b[2][j];
      }
    }
    scale_out = scale_a + scale_b;
  }

  scale_out = Adjust64M33ScaleTo16M33(m33Temp, scale_out, m33);

  return scale_out;
}

void AssignV3I2I(const int32_t v3In[3], int32_t v3Out[3])
{
  v3Out[0] = v3In[0];
  v3Out[1] = v3In[1];
  v3Out[2] = v3In[2];
}

void AssignM33I322I(const int32_t m33In[3][3], int16_t m33Out[3][3])
{
  int16_t i, j;
  for (i = 0; i < 3; ++i)
    for (j = 0; j < 3; ++j)
      m33Out[i][j] = (int16_t)m33In[i][j];
}

void AssignM33I2I(const int16_t m33In[3][3], int16_t m33Out[3][3])
{
  int16_t i, j;
  for (i = 0; i < 3; ++i)
    for (j = 0; j < 3; ++j)
      m33Out[i][j] = m33In[i][j];
}

//// lms<=>ipt xfer matrix in 16 bit fixed point
#define m33Lms2IptScale2P     12
static const int16_t m33Lms2IptFxp[3][3] =
{   { 1638,   1638,   819},
    {18248, -19870,  1622},
    { 3300,   1463, -4763}  };


int16_t GetLms2IptM33Fxp(int16_t m33Lms2Ipt[3][3])
{
  AssignM33I2I(m33Lms2IptFxp, m33Lms2Ipt);
  return m33Lms2IptScale2P;
}

#define m33Ipt2LmsScale2P     15
static const int16_t m33Ipt2LmsFxp[3][3] =
{   {32767,  3196,  6725},
    {32767, -3731,  4365},
    {32767,  1069, -22180}  };

int16_t GetIpt2LmsM33Fxp(int16_t m33Ipt2Lms[3][3])
{
  AssignM33I2I(m33Ipt2LmsFxp, m33Ipt2Lms);
  return m33Ipt2LmsScale2P;
}

#define m33Rgb2XyzScale2P     26
static void GetRgb2XyzM33FromPrims(int32_t rx, int32_t ry, int32_t gx, int32_t gy, int32_t bx, int32_t by,
    int32_t wx, int32_t wy, int32_t m33Rgb2Xyz[3][3])
{
  //// calculate u, v, w
  int64_t D =  (rx - bx)*(gy - by) - (ry - by)*(gx - bx);
  int64_t u = ((((wx - bx)*(gy - by) - (wy - by)*(gx - bx)) << 30) / D) >> 30;
  int64_t v = ((((rx - bx)*(wy - by) - (ry - by)*(wx - bx)) << 30) / D) >> 30;
  int64_t w = (1 << m33Rgb2XyzScale2P) - u - v;

  //// scale u, v, w by wy
  u = ((u << 30) / wy) >> 30;
  v = ((v << 30) / wy) >> 30;
  w = ((w << 30) / wy) >> 30;

  //// get m33Rgb2Xyz
  m33Rgb2Xyz[0][0] = (int32_t)(rx*u);
  m33Rgb2Xyz[0][1] = (int32_t)(gx*v);
  m33Rgb2Xyz[0][2] = (int32_t)(bx*w);

  m33Rgb2Xyz[1][0] = (int32_t)(ry*u);
  m33Rgb2Xyz[1][1] = (int32_t)(gy*v);
  m33Rgb2Xyz[1][2] = (int32_t)(by*w);

  m33Rgb2Xyz[2][0] = (int32_t)(((1 << m33Rgb2XyzScale2P) - rx - ry)*u); // rz*u
  m33Rgb2Xyz[2][1] = (int32_t)(((1 << m33Rgb2XyzScale2P) - gx - gy)*v); // gz*u
  m33Rgb2Xyz[2][2] = (int32_t)(((1 << m33Rgb2XyzScale2P) - bx - by)*w); // bz*u
}

// getRgb2XyzM33(): returns the matrix required to go from the specified RGB space to XYZ
//  - The target space can be one of,
//    'p3d65', 'dci', 'r709', 'r2020',  'aces'
static void GetRgb2XyzM33(CRgbDef_t rgbDef, int32_t m33Rgb2Xyz[3][3])
{
  if (rgbDef == CRgbDefP3d65) {
    m33Rgb2Xyz[0][0] = (int32_t)(0.48657094864821604 *  (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[0][1] = (int32_t)(0.26566769316909294 *  (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[0][2] = (int32_t)(0.19821728523436252 *  (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[1][0] = (int32_t)(0.22897456406974873 *  (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[1][1] = (int32_t)(0.69173852183650597 *  (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[1][2] = (int32_t)(0.079286914093745012 * (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[2][0] = (int32_t)(-3.9720755169334861e-017 * (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[2][1] = (int32_t)(0.045113381858902610 * (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[2][2] = (int32_t)(1.0439443689009760 *   (1 << m33Rgb2XyzScale2P));
  }
  else if (rgbDef == CRgbDefDci) {
    m33Rgb2Xyz[0][0] = (int32_t)(0.445169815564552 *  (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[0][1] = (int32_t)(0.277134409206778 * (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[0][2] = (int32_t)(0.172282669815564 *   (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[1][0] = (int32_t)(0.209491677912731 *  (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[1][1] = (int32_t)(0.721595254161044 * (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[1][2] = (int32_t)(0.068913067926226 *   (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[2][0] = (int32_t)(0.000000000000000 * (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[2][1] = (int32_t)(0.047060560053981 *  (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[2][2] = (int32_t)(0.907355394361973 *   (1 << m33Rgb2XyzScale2P));
  }
  else if (rgbDef == CRgbDefR709) {
    m33Rgb2Xyz[0][0] = (int32_t)(0.41239079926595934 *  (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[0][1] = (int32_t)(0.35758433938387801 *  (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[0][2] = (int32_t)(0.18048078840183429 *  (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[1][0] = (int32_t)(0.21263900587151030 *  (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[1][1] = (int32_t)(0.71516867876775603 *  (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[1][2] = (int32_t)(0.072192315360733714 * (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[2][0] = (int32_t)(0.019330818715591825 * (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[2][1] = (int32_t)(0.11919477979462598 *  (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[2][2] = (int32_t)(0.95053215224966059 *  (1 << m33Rgb2XyzScale2P));
  }
  else if (rgbDef == CRgbDefR2020) {
    m33Rgb2Xyz[0][0] = (int32_t)(0.636958048301291 * (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[0][1] = (int32_t)(0.144616903586208 * (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[0][2] = (int32_t)(0.168880975164172 *   (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[1][0] = (int32_t)(0.262700212011267 *  (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[1][1] = (int32_t)(0.677998071518871 * (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[1][2] = (int32_t)(0.059301716469862 *   (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[2][0] = (int32_t)(0.000000000000000 * (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[2][1] = (int32_t)(0.028072693049087 *  (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[2][2] = (int32_t)(1.060985057710792 *   (1 << m33Rgb2XyzScale2P));
  }
  else if (rgbDef == CRgbDefAces) {
    m33Rgb2Xyz[0][0] = (int32_t)(0.952552395938186 *  (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[0][1] = (int32_t)(0.000000000000000 * (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[0][2] = (int32_t)(0.000093678631660 *   (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[1][0] = (int32_t)(0.343966449765075 *  (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[1][1] = (int32_t)(0.728166096613486 *  (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[1][2] = (int32_t)(-0.072132546378561 *   (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[2][0] = (int32_t)(0.000000000000000 * (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[2][1] = (int32_t)(0.000000000000000 *  (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[2][2] = (int32_t)(1.008825184351586 *   (1 << m33Rgb2XyzScale2P));
  }
  else if (rgbDef == CRgbDefAlexa) {
    m33Rgb2Xyz[0][0] = (int32_t)(0.638007619283725 * (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[0][1] = (int32_t)(0.214703856337102 * (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[0][2] = (int32_t)(0.097744451430844 *   (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[1][0] = (int32_t)(0.291953778999716 *  (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[1][1] = (int32_t)(0.823841041510691 *  (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[1][2] = (int32_t)(-0.115794820510407 *   (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[2][0] = (int32_t)(0.002798279031946 *  (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[2][1] = (int32_t)(-0.067034235688959 *  (1 << m33Rgb2XyzScale2P));
    m33Rgb2Xyz[2][2] = (int32_t)(1.153293707416892 *   (1 << m33Rgb2XyzScale2P));
  }
  else {
    assert(rgbDef < CRgbDefNum);
  }
}

static void GetRgb2YuvFromRgb2XyzM33(int32_t m33Rgb2Xyz[3][3], int64_t m33Rgb2Yuv[3][3])
{
  m33Rgb2Yuv[0][0] = m33Rgb2Xyz[1][0];
  m33Rgb2Yuv[0][1] = m33Rgb2Xyz[1][1];
  m33Rgb2Yuv[0][2] = m33Rgb2Xyz[1][2];

  m33Rgb2Yuv[1][0] = -((int64_t)(m33Rgb2Xyz[1][0] >> 1) << m33Rgb2XyzScale2P)/((1 << m33Rgb2XyzScale2P) - m33Rgb2Xyz[1][2]);
  m33Rgb2Yuv[1][1] = -((int64_t)(m33Rgb2Xyz[1][1] >> 1) << m33Rgb2XyzScale2P)/((1 << m33Rgb2XyzScale2P) - m33Rgb2Xyz[1][2]);
  m33Rgb2Yuv[1][2] = (1 << (m33Rgb2XyzScale2P - 1));

  m33Rgb2Yuv[2][0] = (1 << (m33Rgb2XyzScale2P - 1));
  m33Rgb2Yuv[2][1] = -((int64_t)(m33Rgb2Xyz[1][1] >> 1) << m33Rgb2XyzScale2P)/((1 << m33Rgb2XyzScale2P) - m33Rgb2Xyz[1][0]);
  m33Rgb2Yuv[2][2] = -((int64_t)(m33Rgb2Xyz[1][2] >> 1) << m33Rgb2XyzScale2P)/((1 << m33Rgb2XyzScale2P) - m33Rgb2Xyz[1][0]);
}

int16_t GetRgb2YuvM33Fxp(CYuvXferSpec_t yuvXferSpec, int16_t m33Rgb2Yuv[3][3])
{
  int32_t m33Rgb2Xyz[3][3];
  int16_t scale;
  int64_t m33Temp[3][3];

  GetRgb2XyzM33((CRgbDef_t)yuvXferSpec, m33Rgb2Xyz);
  GetRgb2YuvFromRgb2XyzM33(m33Rgb2Xyz, m33Temp);

  scale = Adjust64M33ScaleTo16M33(m33Temp, m33Rgb2XyzScale2P, m33Rgb2Yuv);

  return scale;
}

#define m33Xyz2LmsFxpScale2P  28
static const int32_t m33Xyz2LmsFxp[3][3] =
  { { (int32_t)(0.400238220153002 * (1 << m33Xyz2LmsFxpScale2P)),
      (int32_t)(0.707593156246992 * (1 << m33Xyz2LmsFxpScale2P)),
      (int32_t)(-0.080805581487871 * (1 << m33Xyz2LmsFxpScale2P))},
    { (int32_t)(-0.226298103801373 * (1 << m33Xyz2LmsFxpScale2P)),
      (int32_t)(1.165315591308231 * (1 << m33Xyz2LmsFxpScale2P)),
      (int32_t)(0.045700774541672 * (1 << m33Xyz2LmsFxpScale2P))},
    { (int32_t)(0.0),
      (int32_t)(0.0),
      (int32_t)(0.918224951158247 * (1 << m33Xyz2LmsFxpScale2P))
  } };

/* RGB<=>LMS without white balance */
/* return scale */
int16_t GetRgb2LmsM33(CRgbDef_t rgbDef, int16_t m33Rgb2Lms[3][3])
{
  int32_t m33Rgb2Xyz[3][3];

  GetRgb2XyzM33(rgbDef, m33Rgb2Xyz);
  return M33TimesM33AS((const int32_t (*)[3])m33Xyz2LmsFxp, m33Xyz2LmsFxpScale2P, (const int32_t (*)[3])m33Rgb2Xyz, m33Rgb2XyzScale2P, m33Rgb2Lms);
}

int16_t GetRgb2LmsByDefM33(CRgbDef_t rgbDef, int16_t m33Rgb2Lms[3][3])
{
  return GetRgb2LmsM33(rgbDef, m33Rgb2Lms);
}

void GetRgb2LmsWpV3(int16_t m33Rgb2Lms[3][3], int32_t v3Wp[3])
{
  int16_t r;

  for (r = 0; r < 3; ++r) {
    v3Wp[r] = (int32_t)m33Rgb2Lms[r][0] + (int32_t)m33Rgb2Lms[r][1] + (int32_t)m33Rgb2Lms[r][2];
  }
}

/* return scale */
int16_t GetRgb2LmsWpByDefV3Fxp(CRgbDef_t rgbDef, int32_t v3Wp[3])
{
  int16_t scale;
  int16_t m33Rgb2Lms[3][3];

  scale = GetRgb2LmsByDefM33(rgbDef, m33Rgb2Lms);
  GetRgb2LmsWpV3(m33Rgb2Lms, v3Wp);

  return scale;
}

/* RGB<=>LMS with white balance */
/* return scale */
int16_t GetRgb2WpLmsM33Fxp(int16_t m33[3][3], int16_t m33Scale, const int32_t v3Wp[3], int16_t wpScale, int16_t m33Rgb2WpLms[3][3])
{
  int16_t r, c, scale;
  int64_t m33Temp[3][3];

  for (r = 0; r < 3; r++) {
    for (c = 0; c < 3; c++) {
      m33Temp[r][c] = ((int64_t)m33[r][c] * (1<< 16)) / v3Wp[r];
    }
  }
  scale = 16 + m33Scale - wpScale;

  scale = Adjust64M33ScaleTo16M33(m33Temp, scale, m33Rgb2WpLms);

  return scale;
}

int16_t GetRgb2LmsByPrimsM33Fxp(int32_t rx, int32_t ry, int32_t gx, int32_t gy, int32_t bx, int32_t by,
    int32_t wx, int32_t wy, int16_t m33Rgb2Lms[3][3])
{
  int32_t m33Rgb2Xyz[3][3];

  GetRgb2XyzM33FromPrims(rx,ry,gx,gy,bx,by,wx,wy, m33Rgb2Xyz);
  return M33TimesM33AS((const int32_t (*)[3])m33Xyz2LmsFxp, m33Xyz2LmsFxpScale2P, (const int32_t (*)[3])m33Rgb2Xyz, m33Rgb2XyzScale2P, m33Rgb2Lms);
}

int16_t GetRgb2LmsCtWpM33Fxp(int16_t m33[3][3], int16_t m33Scale, int32_t crossTalk, const int32_t v3Wp[3], int16_t wpScale, int16_t m33Rgb2Lms[3][3])
{
  int64_t colFct[3];
  int32_t d1m3ct;
  int32_t i, j, scale;
  int64_t m33Temp[3][3];
  int16_t ct_scale = 15;

  d1m3ct = (1 << ct_scale) - 3*crossTalk;

  for (j = 0; j < 3; ++j) {
    colFct[j] = (int64_t)crossTalk*((int32_t)m33[0][j] + (int32_t)m33[1][j] + (int32_t)m33[2][j]);
  }

  for (i = 0; i < 3; ++i) {
    for (j = 0; j < 3; ++j) {
      m33Temp[i][j] = ((((int64_t)d1m3ct*m33[i][j]) + colFct[j]) * (1<< 16)) / v3Wp[i];
    }
  }
  scale = 16 + m33Scale + ct_scale - wpScale;

  scale = Adjust64M33ScaleTo16M33(m33Temp, scale, m33Rgb2Lms);

  return scale;
}

int16_t GetRgb2LmsWpCtM33Fxp(const int32_t m33[3][3], int16_t m33Scale, const int32_t v3Wp[3], int16_t wpScale, int32_t crossTalk, int16_t m33Rgb2Lms[3][3])
{
  int64_t colFct[3];
  int32_t d1m3ct;
  int32_t i, j, scale;
  int64_t m33Temp[3][3];
  int16_t ct_scale = 15;

  d1m3ct = (1 << ct_scale) - 3*crossTalk;

  for (j = 0; j < 3; ++j) {
    colFct[j] = (int64_t)crossTalk*(((int64_t)m33[0][j]<<16)/v3Wp[0] + ((int64_t)m33[1][j]<<16)/v3Wp[1] + ((int64_t)m33[2][j]<<16)/v3Wp[2]);
  }

  for (i = 0; i < 3; ++i) {
    for (j = 0; j < 3; ++j) {
      m33Temp[i][j] = (((int64_t)d1m3ct*m33[i][j]) << 16)/v3Wp[i] + colFct[j];
    }
  }
  scale = 16 + m33Scale + ct_scale - wpScale; // check this

  scale = Adjust64M33ScaleTo16M33(m33Temp, scale, m33Rgb2Lms);

  return scale;
}

int16_t GetLms2RgbByDefM33Fxp(CRgbDef_t rgbDef, int16_t m33Lms2Rgb[3][3])
{
  int16_t scale, rshift;

  scale = GetRgb2LmsByDefM33(rgbDef, m33Lms2Rgb);

  rshift = InvM33(m33Lms2Rgb, m33Lms2Rgb, scale, scale);
  scale -= rshift;

  return scale;
}

int16_t GetLms2RgbByPrimsM33Fxp(int32_t rx, int32_t ry, int32_t gx, int32_t gy, int32_t bx, int32_t by,
    int32_t wx, int32_t wy, int16_t m33Lms2Rgb[3][3])
{
  int16_t scale, rshift;

  scale = GetRgb2LmsByPrimsM33Fxp(rx,ry,gx,gy,bx,by,wx,wy, m33Lms2Rgb);

  rshift = InvM33(m33Lms2Rgb, m33Lms2Rgb, scale, scale);
  scale -= rshift;

  return scale;
}

int16_t GetLms2RgbCtWpM33Fxp(int16_t m33[3][3], int16_t m33Scale, int32_t crossTalk, const int32_t v3Wp[3], int16_t wpScale, int16_t m33Lms2Rgb[3][3])
{
  int64_t rowFct[3];
  int32_t d1m3ct;
  int32_t i, j, scale;
  int64_t m33Temp[3][3];
  int16_t ct_scale = 15;

  d1m3ct = (1 << ct_scale) - 3*crossTalk;

  for (i = 0; i < 3; ++i) {
    rowFct[i] = (int64_t)crossTalk*((int32_t)m33[i][0] + (int32_t)m33[i][1] + (int32_t)m33[i][2]);
  }

  for (i = 0; i < 3; ++i) {
    for (j = 0; j < 3; ++j) {
      m33Temp[i][j] = (((((int64_t)m33[i][j] * ((int64_t)1 << ct_scale)) - rowFct[i]) << 16) / d1m3ct) * v3Wp[j];
    }
  }
  scale = 16 + m33Scale + wpScale;

  scale = Adjust64M33ScaleTo16M33(m33Temp, scale, m33Lms2Rgb);

  return scale;
}

int16_t GetLms2RgbWpCtM33(const int32_t m33[3][3], int16_t m33Scale, const int32_t v3Wp[3], int16_t wpScale, int32_t crossTalk, int16_t m33Lms2Rgb[3][3])
{
  int64_t rowFct[3];
  int32_t d1m3ct;
  int32_t i, j, scale;
  int64_t m33Temp[3][3];
  int16_t ct_scale = 15;

  d1m3ct = (1 << ct_scale) - 3*crossTalk;

  for (i = 0; i < 3; ++i) {
    rowFct[i] = (int64_t)crossTalk*(m33[i][0]*v3Wp[0] + m33[i][1]*v3Wp[1] + m33[i][2]*v3Wp[2]);
  }

  for (i = 0; i < 3; ++i) {
    for (j = 0; j < 3; ++j) {
      m33Temp[i][j] = (((((int64_t)m33[i][j]*v3Wp[j]) * ((int64_t)1 << ct_scale)) - rowFct[i]) << 16) / d1m3ct;
    }
  }
  scale = 16 + m33Scale + wpScale;

  scale = Adjust64M33ScaleTo16M33(m33Temp, scale, m33Lms2Rgb);

  return scale;
}

/*
int16_t GetWpLmsCt2RgbM33Fxp(CRgbDef_t rgbDef, const int32_t v3Wp[3], int16_t wpScale, int32_t crossTalk, int16_t m33WpCtLms2Rgb[3][3])
{
  int16_t scale, rshift;

  scale = GetRgb2WpLmsCtM33Fxp(rgbDef, v3Wp, wpScale, crossTalk, m33WpCtLms2Rgb);

  rshift = InvM33(m33WpCtLms2Rgb, m33WpCtLms2Rgb, scale, scale);
  scale -= rshift;

  return scale;
}
*/
int16_t GetYuv2RgbM33Fxp(CYuvXferSpec_t yuvXferSpec, int16_t m33Yuv2Rgb[3][3])
{
  int16_t scale, rshift;
  int16_t m33Temp16[3][3];

  scale = GetRgb2YuvM33Fxp(yuvXferSpec, m33Temp16);

  rshift = InvM33(m33Temp16, m33Yuv2Rgb, scale, scale);
  scale -= rshift;

  return scale;
}

static int16_t Yuv2RgbM33Full2Narrow(int16_t m33Full[3][3], int16_t scale, int16_t bits, int16_t m33Narrow[3][3])
{
  // from full to narrow
  int64_t m33Temp[3][3];
  int32_t yRation =  (int32_t)(((1<<bits) - 1) << 16)/( (235-16)*(1<<(bits - 8)) );
  int32_t uvRation = (int32_t)(((1<<bits) - 1) << 16)/( (240-16)*(1<<(bits - 8)) );
  int16_t r;

  for (r = 0; r < 3; ++r) {
    m33Temp[r][0] = (int64_t)m33Full[r][0]*yRation;
    m33Temp[r][1] = (int64_t)m33Full[r][1]*uvRation;
    m33Temp[r][2] = (int64_t)m33Full[r][2]*uvRation;
  }
  scale += 16;
  scale = Adjust64M33ScaleTo16M33(m33Temp, scale, m33Narrow);

  return scale;
}

int16_t GetYuv2RgbM33NarrowFxp(CYuvXferSpec_t yuvXferSpec, int bits, int16_t m33Yuv2Rgb[3][3])
{
  int16_t scale;
  int16_t m33Yuv2RgbFull[3][3];

  scale = GetYuv2RgbM33Fxp(yuvXferSpec, m33Yuv2RgbFull);
  scale = Yuv2RgbM33Full2Narrow(m33Yuv2RgbFull, scale, bits, m33Yuv2Rgb);

  return scale;
}

/* narrow version */
static int16_t Rgb2YuvM33Full2Narrow(int16_t m33Full[3][3], int16_t scale, int bits, int16_t m33Narrow[3][3])
{
  // from full to narrow
  int64_t m33Temp[3][3];
  int32_t yRation =  ( ((235-16)*(int32_t)(1<<(bits - 8))) << 16 )/((1<<bits) - 1);
  int32_t uvRation = ( ((240-16)*(int32_t)(1<<(bits - 8))) << 16 )/((1<<bits) - 1);
  int16_t c;

  for (c = 0; c < 3; ++c) {
    m33Temp[0][c] = (int64_t)m33Full[0][c]*yRation;
    m33Temp[1][c] = (int64_t)m33Full[1][c]*uvRation;
    m33Temp[2][c] = (int64_t)m33Full[2][c]*uvRation;
  }
  scale += 16;
  scale = Adjust64M33ScaleTo16M33(m33Temp, scale, m33Narrow);

  return scale;
}

int16_t GetRgb2YuvM33NarrowFxp(CYuvXferSpec_t yuvXferSpec, int bits, int16_t m33Rgb2Yuv[3][3])
{
  int16_t scale;
  int16_t m33Rgb2YuvFull[3][3];

  scale = GetRgb2YuvM33Fxp(yuvXferSpec, m33Rgb2YuvFull);
  scale = Rgb2YuvM33Full2Narrow(m33Rgb2YuvFull, scale, bits, m33Rgb2Yuv);

  return scale;
}


#define L2G_SEC_GRP_NUM      5
#define LOG2_LUT_SIZE        16
#define POW_IN_SCALE         31
#define LOG2_OUT_SCALE       26
#define POW2_OUT_SCALE       31

// input/output scales in 31
int32_t powTaylor(uint32_t in, uint32_t exp)
{
  int32_t temp;

  temp = fxLOG2Taylor(in, LOG2_OUT_SCALE); // scale in 26
  temp = (int64_t)temp - ((int64_t)31 << LOG2_OUT_SCALE);
  temp = ((int64_t)temp * exp) >> 31; // scale in 26
  temp = fxPOW2Taylor(temp, LOG2_OUT_SCALE, POW2_OUT_SCALE); // scale in 31

  return temp;
}

// log10()
// input scale 31, output 26
#define LOG10_2  ((int32_t)(0.301029995664 * ((int64_t)1 << 31))) // the value of 10 based log 2 in scale 31
int32_t log10Taylor(int32_t x)
{
  int32_t y;

  y = fxLOG2Taylor(x, LOG2_OUT_SCALE); // scale in 26
  y = (int64_t)y - ((int64_t)31 << LOG2_OUT_SCALE);
  y = ((int64_t)y * LOG10_2) >> 31;   // convert to log10 based

  return y;
}

// exp_n()
// input scale 30
#define LOG2_N  ((int32_t)(1.44269504089 * ((int64_t)1 << 30))) // the value of 2 based log e in scale 30
int32_t expNTaylor(int32_t x, int32_t sScaleOut)
{
    x = ((int64_t)x * LOG2_N) >> 30;
    return fxPOW2Taylor(x, 30, sScaleOut);
}

// input gamma scale: 14; min/max scale: 18
void GenerateL2GLut(uint32_t gammaR, uint64_t *lutA, uint16_t *lutB, uint32_t *lutX)
{
    const int32_t l2gSecGrpTbl[L2G_SEC_GRP_NUM + 1] = {31, 17, 12, 9, 2, 0};
    uint32_t l2gLnLut[DEF_L2G_LUT_NODES + 1]; // scale 2^32 - 1
    uint32_t l2gGmLut[DEF_L2G_LUT_NODES + 1]; // scale 2^16 - 1
    uint32_t twoGmR;
    int32_t secGrpNew, secGrpIdx;
    int32_t secIdPw, secNdNumPw, secNdNum, secNdIdx;
    int32_t lutIdx, lutIdxSecPrev;
    uint32_t secNdDelta;
    int32_t lutIdxSec;

    twoGmR = fxPOW2Taylor(gammaR, 31, 30); // scale in 30

    //// 1st section
    secGrpIdx = 0;
    secIdPw = l2gSecGrpTbl[0];
    secNdNumPw = 0;
    secNdNum = 1;
    lutIdx = 0;

    l2gLnLut[0] = ((int64_t)1 << POW_IN_SCALE) / ((int64_t)1 << secIdPw); // scale in 31
    l2gGmLut[0] = powTaylor(l2gLnLut[0], gammaR);

    lutIdxSecPrev = lutIdx;
    ++lutIdx;

    //// following section but the last
    while (--secIdPw > 0) {
        secNdIdx = 0;
        secGrpNew = (secIdPw == l2gSecGrpTbl[secGrpIdx + 1]);
        if (secGrpNew) {
            // start new section group
            ++secGrpIdx;
            secNdNumPw += 1;
            secNdNum *= 2;
            secNdDelta = l2gLnLut[lutIdxSecPrev] >> (secNdNumPw - 1); // shift
        }

        lutIdxSec = lutIdx;// debug only

        while (secNdIdx < secNdNum) {
            l2gLnLut[lutIdx] = 2*l2gLnLut[lutIdxSecPrev]; // shift
            l2gGmLut[lutIdx] = ((int64_t)twoGmR * l2gGmLut[lutIdxSecPrev]) >> 30; // scale
            ++lutIdx;
            ++secNdIdx;

            ++lutIdxSecPrev;

            if (secGrpNew) {
                l2gLnLut[lutIdx] = l2gLnLut[lutIdx - 1] + secNdDelta;
                l2gGmLut[lutIdx] = powTaylor(l2gLnLut[lutIdx], gammaR);

                ++lutIdx;
                ++secNdIdx;
            } // section node need calculation
        } // secNdIdx

        assert(lutIdxSec == lutIdxSecPrev);
    } // secIdPw

    // remaining entries
    do {
        l2gLnLut[lutIdx] = ((int64_t)1 << POW2_OUT_SCALE) - 1;
        l2gGmLut[lutIdx] = ((int64_t)1 << POW2_OUT_SCALE) - 1;
    } while (++lutIdx < DEF_L2G_LUT_NODES + 1);

    // populate non-linear distributed luts (piecewise linear interpolation luts)
    for (lutIdxSec = 0; lutIdxSec < lutIdx - 1; ++lutIdxSec) {
        lutX[lutIdxSec] = l2gLnLut[lutIdxSec];
        lutB[lutIdxSec] = MIN2S((l2gGmLut[lutIdxSec] + (1 << (POW2_OUT_SCALE - 16 - 1)))>> (POW2_OUT_SCALE - 16), 0xffff);
        if (l2gLnLut[lutIdxSec + 1] == l2gLnLut[lutIdxSec])
          lutA[lutIdxSec] = 0;
        else
          lutA[lutIdxSec] = ((int64_t)(l2gGmLut[lutIdxSec + 1] - l2gGmLut[lutIdxSec]) << 31) / (l2gLnLut[lutIdxSec + 1] - l2gLnLut[lutIdxSec]);
    }
}

