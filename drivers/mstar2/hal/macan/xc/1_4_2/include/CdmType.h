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
#ifndef C_DM_TYPE_H
#define C_DM_TYPE_H

#include "KCdmModCtrl.h"

/*! @brief DM configuration type and macros

    DM configuration/control layer type and macros

*/
typedef enum {
  CPlatformCpu = 0, 
  CPlatformCuda,
  CPlatformOpenCl,
  CPlatformNum  
} CPlatform_t;

typedef enum {
  CMsMethodDbEdge = 0,
  CMsMethodOff,
  CMsMethodNum
} CMsMethod_t;

////// pixel definition, force to unsigned char
typedef enum {
  CClrYuv = 0,
  CClrRgb,
  CClrRgba,
  CClrIpt,
  CClrGrey, // for separate alpha channel
  CClrNum
} CClr_t;

typedef enum {
  CChrm420 = 0,
  CChrm422,
  CChrm444,
  CChrmNum
} CChrm_t;

typedef enum {
  CDtpU16 = 0,
  CDtpU8,
  CDtpF32,
//PxlDtpF16,
  CDtpNum
} CDtp_t;

// bit depth is of type unsigned char, numerical value. 
// MSB aliagned will take the whole bits of an interger data type
typedef int         CBdp_t;
#define CBdpMin   ((CBdp_t)8)
#define CBdpMax   ((CBdp_t)16)

typedef enum {
  CLocHost = 0,
  CLocDev,
  CLocNum
} CLoc_t;

typedef enum {
  CWeavPlnr = 0,
  CWeavIntl, // per component interleave, YUVYUV..., RGBRGB...
  CWeavUyVy, // UYVY and UYVY only
  CWeavNum
} CWeav_t;

typedef enum {
  CEotfBt1886 = 0,
  CEotfPq,
  CEotfPower,
  CEotfNum
} CEotf_t;

typedef enum {
  CRngNarrow = 0, // head
  CRngFull = 1, // will be the in data type(bits) range
  CRngSdi = 2, // pq
  CRngNum
} CRng_t;

typedef enum {
  CRgbDefP3d65 = 0,
  CRgbDefDci,
  CRgbDefR709,
  CRgbDefR2020,
  CRgbDefAces,
  CRgbDefAlexa,
  
  CRgbDefNum
} CRgbDef_t;

typedef enum {
  CYuvXferSpecP3d65 = CRgbDefP3d65,
  CYuvXferSpecDci   = CRgbDefDci,
  CYuvXferSpecR709  = CRgbDefR709,
  CYuvXferSpecR2020 = CRgbDefR2020,
  CYuvXferSpecAces  = CRgbDefAces,
  CYuvXferSpecAlexa = CRgbDefAlexa,

  CYuvXferSpecNum
} CYuvXferSpec_t;

// change flag in 24 bits field to indicate which part of ctrl/data is changed from last frame
#define FLAG_CHANGE_CFG         0x000001      // general control config change
#define FLAG_CHANGE_MDS         0x000002      // general metadata change
#define FLAG_CHANGE_MDS_CFG     0x000004      // metadata config (below level 2) change
#define FLAG_CHANGE_GD          0x000008      // global dimming change
#define FLAG_CHANGE_TC          0x000010      // tone curve lut change
#define FLAG_CHANGE_TC2         0x000020      // 2nd tone curve (graphics) lut change
#define FLAG_CHANGE_L2NL        0x000040      // L2NL lut change (for DM3.1)

#define GET_PXL_COMP_BYTES(cDtp) (  ( (cDtp) == CDtpU16 ) ? 2 : \
                                    ( (cDtp) == CDtpU8 )  ? 1 : \
                                    ( (cDtp) == CDtpF32 ) ? 4 : \
                                    2 )

#define GET_PXL_ECMP_NUM(cClr, cChrm)   ( ( (cClr)  == CClrRgba ) ? 4 : \
                                          ( (cClr)  == CClrGrey ) ? 1 : \
                                          ( (cChrm) == CChrm422 ) ? 2 :\
                                          3 )

// UYVY is interleaved as UY at even, VY at odd, treat as 2 effective component

#define GET_PXL_COL_PITCH(cClr, cChrm, cWeav, cDtp)  (  (cWeav == CWeavPlnr) ?            \
    GET_PXL_COMP_BYTES(cDtp) : GET_PXL_ECMP_NUM(cClr, cChrm) * GET_PXL_COMP_BYTES(cDtp) )

#define HAVE_PXL_ALPH_COMP(cClr)    ( (cClr) == CClrRgba )

#endif //C_DM_TYPE_H
