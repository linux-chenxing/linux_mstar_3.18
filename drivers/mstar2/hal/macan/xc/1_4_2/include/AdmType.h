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
#ifndef A_DM_UTIL_H
#define A_DM_UTIL_H

#include <sstream>
#include "CdmType.h"

// for frame number, name size control
#define DM_MAX_NAME    512    // max length names
typedef char NameStr_t[DM_MAX_NAME+1];


// frame description for transport layer
typedef struct FrmDesc_t_ {
  unsigned int BufSize;
  unsigned int FrmNo;
  unsigned char *FrmBuf0, *FrmBuf1, *FrmBuf2, *FrmBuf3;
} FrmDesc_t;

//// to help file image IO
typedef struct IoFrmFmt_t_ {
  //// frame size and buffer layout
  int RowNum, ColNum; 
  // for Y; R,G,B...
  int RowPitch;  // row pitch in byte
  // for U, V;
  int RowPitchC; 
  unsigned FrmByteSize; // help external mem allocator to obtain the mini size required

  //// signal definition
  CClr_t Clr;
  CChrm_t Chrm;
  CWeav_t Weav;
  CDtp_t Dtp;
  CBdp_t Bdp;

  unsigned char *FrmBuf0, *FrmBuf3;   // the bufs
  unsigned char *FrmBuf1, *FrmBuf2;   // for G, B; Y, U;.. 0 means not used
} IoFrmFmt_t;

#endif // A_DM_UTIL_H
