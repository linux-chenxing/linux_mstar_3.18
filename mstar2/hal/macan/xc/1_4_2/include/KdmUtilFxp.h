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


#ifndef K_DM_UTIL_FXP_H
#define K_DM_UTIL_FXP_H

#include "KdmTypeFxp.h"

////// Get/Set pixel value for data type F32, U16, U8 and frame arrangement of planar or interleaved
#define GET_OFFSET(frmFmt_, row_, col_)   ((row_)*(frmFmt_).rowPitch + (col_)*(frmFmt_).colPitch)

#define GET_444_TYPE_VALUE(dtp_t, byteOff, frmBuf0, frmBuf1, frmBuf2, x0, x1, x2) \
  x0 =  *(dtp_t *)(frmBuf0 + byteOff);                                            \
  x1 =  *(dtp_t *)(frmBuf1 + byteOff);                                            \
  x2 =  *(dtp_t *)(frmBuf2 + byteOff)


#define GET_444_VALUE(dtp, byteOff, frmBuf0, frmBuf1, frmBuf2, x0, x1, x2)            \
  if (dtp == KDtpU8) {                                                            \
    GET_444_TYPE_VALUE(uint8_t, byteOff, frmBuf0, frmBuf1, frmBuf2, x0, x1, x2);\
  }                                                                                   \
  else {                                                                              \
    GET_444_TYPE_VALUE(uint16_t, byteOff, frmBuf0, frmBuf1, frmBuf2, x0, x1, x2);\
  }


#define SET_444_TYPE_VALUE(dtp_t,  byteOff, x0, x1, x2, frmBuf0, frmBuf1, frmBuf2)  \
  *(dtp_t *)(frmBuf0 + byteOff) = (dtp_t)(x0);                                      \
  *(dtp_t *)(frmBuf1 + byteOff) = (dtp_t)(x1);                                      \
  *(dtp_t *)(frmBuf2 + byteOff) = (dtp_t)(x2)

#define SET_444_VALUE(dtp, byteOff, x0, x1, x2, frmBuf0, frmBuf1, frmBuf2)    \
  if (dtp == KDtpU8) {                                                    \
    SET_444_TYPE_VALUE(uint8_t, byteOff, (x0), (x1), (x2), frmBuf0, frmBuf1, frmBuf2);\
  }                                                                           \
  else {                                                                      \
    SET_444_TYPE_VALUE(uint16_t, byteOff, (x0), (x1), (x2), frmBuf0, frmBuf1, frmBuf2);\
  }

#define SET_422_VALUE_INC_ADDR(dtp, x, pUyVy, scale)        \
  if (dtp == KDtpU8) {                                      \
    *(uint8_t *)(pUyVy) = (uint8_t)(x >> scale);            \
    pUyVy += sizeof(uint8_t);                               \
  }                                                         \
  else if (dtp == KDtpU16) {                                \
    *(uint16_t *)(pUyVy) = (uint16_t)(x >> scale);          \
    pUyVy += sizeof(uint16_t);                              \
  }

#define SET_VALUE(dtp, x, mn, mx, pV)                                  \
  if (dtp == KDtpU8) {                                            \
    *(uint8_t *)(pV)  = ((x) < (mn)) ? (uint8_t)(mn)  : ((x) < (mx)) ? (uint8_t)((x)+DT5)  : (uint8_t)(mx); \
  }                                                                    \
  else {                                                               \
    *(uint16_t *)(pV) = ((x) < (mn)) ? (uint16_t)(mn) : ((x) < (mx)) ? (uint16_t)((x)+DT5) : (uint16_t)(mx);\
  }

#endif // K_DM_UTIL_FXP_H
