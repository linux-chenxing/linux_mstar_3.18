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
//#include <stdint.h>
#include "KCdmModCtrl.h"
#include "dolbyTypeToMstarType.h"
#ifndef KDMCP_H
#define KDMCP_H

#if EN_CHK_POINT

#include <stdio.h> // needed by FILE*

// define the check point location Id: 
// CpId + 
// module name(source, graphics, target and Csc or Cvm: <s|g|t>Csc, <s|g>Cvm) 
// at which terminal(input, output termianl: It, Ot)
// which operation(input, output: I, O)
typedef enum {
  // for loop to work, we define them this way
  //// all outputs
  CpIdFirstO = 0, // output: first one

  // for source up sample
  CpIdsUdsItO = CpIdFirstO,
  CpIdsUdsOtO,

  // source csc, cvm
  CpIdsCscItO, // input terminal output
  CpIdsCscOtO, // output terminal output
  CpIdsCvmItO,
  CpIdsCvmOtO,     

  // graphic csc, cvm
  CpIdgCscItO,
  CpIdgCscOtO,     
  CpIdgCvmItO,
  CpIdgCvmOtO,     

  // for Alpha "module"
  CpIdAlphaItO,

  // for blender
  CpIdsBldItO,
  CpIdgBldItO,
  CpIdBldOtO, // two input, one output

  // for ms
  CpIdMsIItO,
  CpIdMsAItO,
  CpIdMsOtO, // two input, one output

  // target csc
  CpIdtCscAItO,
  CpIdtCscItO,
  CpIdtCscOtO,

  // for target down sample
  CpIdtUdsItO,
  CpIdtUdsOtO,

  // sub-block, source csc, cvm
  // input terminal output
  CpIdsCsc03ItO, // B01-03
  CpIdsCsc04ItO, // B01-04
  CpIdsCsc05ItO, // B01-05
  CpIdsCsc06ItO, // B01-06
  CpIdsCsc07ItO, // B01-07
  // output terminal output
  CpIdsCsc02OtO, // B01-02
  CpIdsCsc03OtO, // B01-03
  CpIdsCsc04OtO, // B01-04
  CpIdsCsc05OtO, // B01-05
  CpIdsCsc06OtO, // B01-06

  // sub-block, target csc
  // input terminal output
  CpIdtCsc05ItO,  // B05
  // output terminal output
  CpIdtCsc04OtO,  // B04
  
  CpIdLastO = CpIdtCsc04OtO, // output: last

  //// all input terminal input
  CpIdFirstI, // input: first one

  // for source up sample
  CpIdsUdsItI = CpIdFirstI,

  // for source csc, cvm
  CpIdsCscItI, // input terminal input
  CpIdsCvmItI,

  // for graphic csc, cvm
  CpIdgCscItI,
  CpIdgCvmItI,

  // for Alpha "module"
  CpIdAlphaItI,

  // for blender
  CpIdsBldItI,
  CpIdgBldItI,

  // for ms
  CpIdMsIItI,
  CpIdMsAItI,

  // for target csc
  CpIdtCscAItI,
  CpIdtCscItI,

  // for target down sample
  CpIdtUdsItI,

  // sub-block, source csc, cvm
  // input terminal input
  CpIdsCsc03ItI, // B01-03
  CpIdsCsc04ItI, // B01-04
  CpIdsCsc05ItI, // B01-05
  CpIdsCsc06ItI, // B01-06
  CpIdsCsc07ItI, // B01-07

  // sub-block, target csc
  // input terminal input
  CpIdtCsc05ItI,  // B05

  CpIdLastI = CpIdtCsc05ItI, // input: last one

  CpIdNum
} CpId_t;


typedef struct CpIoCtrl_t_ {
  // it is debug code, a simple and uniformed interface is the goal
  // use 3 components for all modules
  // format: test(ASCII) or binary,
  int binaryIO;
  // format: three planar files/bufs(single) or one(compound) interleave 444, uyva 422 or planr 420
  int compoundIO;
  // I/O media
  int bufIO; // 0: file, 1 internal buffer, 2 external buf
  // file pointer: file I/O
  FILE *fp[CpIdNum][3];
  // buf: I/O
  uint8_t *buf[CpIdNum][3];   // the buf starting addr
  uint16_t **pBuf[CpIdNum][3]; // that used during I/O
} CpIoCtrl_t;

// typedef opaque handles to avoid struct * in function declaration to make gcc happy
typedef struct CpIoCtrl_t_ * HCpIoCtrl;


#if defined(c_plusplus) || defined(__cplusplus)
# define DEFINE_CP_DATA()   extern "C" { HCpIoCtrl pCpIoCtrl_ = 0; }
# define DECLARE_CP_DATA()  extern "C" { HCpIoCtrl pCpIoCtrl_; }
#else
# define DEFINE_CP_DATA()          HCpIoCtrl pCpIoCtrl_ = 0
# define DECLARE_CP_DATA()  extern HCpIoCtrl pCpIoCtrl_
#endif

int CpInput3U16(CpId_t idI,  HCpIoCtrl pCpIoCtrl, uint16_t *x0, uint16_t *x1, uint16_t *x2);
void CpOutput3U16(CpId_t idO, HCpIoCtrl pCpIoCtrl, uint16_t x0, uint16_t x1, uint16_t x2);
int CpInput3U32(CpId_t idI,  HCpIoCtrl pCpIoCtrl, uint32_t *x0, uint32_t *x1, uint32_t *x2);
void CpOutput3U32(CpId_t idO, HCpIoCtrl pCpIoCtrl, uint32_t x0, uint32_t x1, uint32_t x2);


int CpInput1U16(CpId_t idI,  HCpIoCtrl pCpIoCtrl, uint16_t *x, int comp);
void CpOutput1U16(CpId_t idO, HCpIoCtrl pCpIoCtrl, uint16_t x, int comp);

void CpIoFrmU16(CpId_t idO, HCpIoCtrl pCpIoCtrl, uint16_t *buf0, uint16_t *buf1, uint16_t *buf2, 
                int rowNum, int colNum, int rowPitchNum, int colPitchNum, int rowPitchNumC, int chrm);


//// pixel based
#define CP_I_3U16(mod_, term_, x0, x1, x2) CpInput3U16(CpId##mod_##term_##I, pCpIoCtrl_, (uint16_t*)x0, (uint16_t*)x1, (uint16_t*)x2)
#define CP_O_3U16(mod_, term_, x0, x1, x2) CpOutput3U16(CpId##mod_##term_##O, pCpIoCtrl_, (uint16_t)x0, (uint16_t)x1, (uint16_t)x2)

#define CP_I_3U32(mod_, term_, x0, x1, x2) CpInput3U32(CpId##mod_##term_##I, pCpIoCtrl_, (uint32_t*)x0, (uint32_t*)x1, (uint32_t*)x2)
#define CP_O_3U32(mod_, term_, x0, x1, x2) CpOutput3U32(CpId##mod_##term_##O, pCpIoCtrl_, (uint32_t)x0, (uint32_t)x1, (uint32_t)x2)

#define CP_I_1U16(mod_, term_, x, comp) CpInput1U16(CpId##mod_##term_##I, pCpIoCtrl_, (uint16_t*)x, comp)
#define CP_O_1U16(mod_, term_, x, comp) CpOutput1U16(CpId##mod_##term_##O, pCpIoCtrl_, (uint16_t)x, comp)

//// frame buffer based
#define CP_I_FRMU16(mod_, term_, buf0, buf1, buf2, rowNum, colNum, rowPitch, colPitch, rowPitchC, chrm) \
  CpIoFrmU16(CpId##mod_##term_##I, pCpIoCtrl_, buf0, buf1, buf2, rowNum, colNum, (rowPitch)/2, (colPitch)/2, (rowPitchC)/2, chrm)

#define CP_O_FRMU16(mod_, term_, buf0, buf1, buf2, rowNum, colNum, rowPitch, colPitch, rowPitchC, chrm) \
  CpIoFrmU16(CpId##mod_##term_##O, pCpIoCtrl_, buf0, buf1, buf2, rowNum, colNum, (rowPitch)/2, (colPitch)/2, (rowPitchC)/2, chrm)

#define CP_OI_FRMU16(mod_, term_, buf0, buf1, buf2, rowNum, colNum, rowPitch, colPitch, rowPitchC, chrm)\
  CP_O_FRMU16(mod_, term_, buf0, buf1, buf2, rowNum, colNum, rowPitch, colPitch, rowPitchC, chrm);      \
  CP_I_FRMU16(mod_, term_, buf0, buf1, buf2, rowNum, colNum, rowPitch, colPitch, rowPitchC, chrm)

//// a signal buffer based

#else //EN_CHK_POINT

# define DEFINE_CP_DATA()
# define DECLARE_CP_DATA()
# define CREATE_CP_ENV(pIoEnv, sClr, gClr, tClr)
# define DESTROY_CP_ENV()

#define CP_I_3U16(mod_, term_, x0, x1, x2)
#define CP_O_3U16(mod_, term_, x0, x1, x2)

#define CP_I_3U32(mod_, term_, x0, x1, x2)
#define CP_O_3U32(mod_, term_, x0, x1, x2)

#define CP_I_1U16(mod_, term_, x, comp)
#define CP_O_1U16(mod_, term_, x, comp)

#define CP_O_FRMU16(mod_, term_, buf0, buf1, buf2, rowNum, colNum, rowPitch, colPitch, rowPitchC, chrm)
#define CP_I_FRMU16(mod_, term_, buf0, buf1, buf2, rowNum, colNum, rowPitch, colPitch, rowPitchC, chrm)
#define CP_OI_FRMU16(mod_, term_, buf0, buf1, buf2, rowNum, colNum, rowPitch, colPitch, rowPitchC, chrm)


#endif // EN_CHK_POINT

#endif // KDMCP_H
