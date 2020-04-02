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
#ifndef VDRMDIO_H
#define VDRMDIO_H

#include "VdrDmApi.h"
#include "AdmType.h"

typedef enum MdSpecType_t_ { MdSpecTypeDir = 0, MdSpecTypeRn, MdSpecTypeName, MdSpecTypeStatic} MdSpecType_t;

//// mds read control structure to support a flexible mds I/O
#define MDS_SIZE_MAX    512
typedef struct MdIoCtrl_t_ {
  // copy from IoEnv_t
  NameStr_t mdSpec;
  int owm;
  
  // derived
  MdSpecType_t mdSpecType;
  // the current md file
  int mdNo;
  NameStr_t mdFn;
  FILE *mdFp;

  // temp buf for the md read in
  unsigned char dataBuf[MDS_SIZE_MAX];
  int dataSize, dataOffset;
} MdIoCtrl_t;

int InitMdEnv(struct IoEnv_t_ *pIoEnv, MdIoCtrl_t *pMdIoCtrl);

// for MdTypeFxpBin case
// owm and pDmCfg for test only: to over write md
const char *ReadMds(const DmCfg_t *pDmCfg, MdIoCtrl_t *pMdIoCtrl, const char *imgFn, HMdsExt_t hMdsExt);

#if DM_VER_LOWER_THAN212
int ReadGmLut(const char *gmLutFn, int enMask, HGmLut_t hGmLut, char *errMsg = 0, bool parseHdrOnly = false);
void FreeDmLut(HGmLut_t hGmLut);
#if ((CDM_FLOAT_POINT ==0) && EN_GLOBAL_DIMMING)
int ReadPq2GLut(const char *lutFn, HPq2GLut_t hLut, char *errMsg);
void FreePq2GLut(HPq2GLut_t hPq2GLut);
#endif
#endif

#endif // VDRMDIO_H
