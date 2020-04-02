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

#ifndef C_DM_MMG_H
#define C_DM_MMG_H

/* for memory alloc flexibility, use a manager to centralize the requirement */
typedef struct Mmg_t_ *HMmg_t;

#if defined(c_plusplus) || defined(__cplusplus)
extern "C" {
#endif

extern HMmg_t InitMmg(HMmg_t hMmg);

typedef int MmgCbFun_t(void *pCbParam, void *buf);
// a simple assignement MmgCbFun: *((FloatComp_t **)pBuf) = (FloatComp_t *)buf
extern int MemAssignCbFunFc(void *pBuf, void *buf);

extern int RegMemReq(unsigned size, unsigned align, int inDev, 
  void *pCbParam, MmgCbFun_t *pCbFun, HMmg_t hMmg);

#if defined(c_plusplus) || defined(__cplusplus)
}
#endif

typedef struct MmgReq_t_ {
  unsigned size;
  unsigned align;
  int inDev;

  void *pCbParam;
  MmgCbFun_t *pCbFun;
} MmgReq_t;

typedef struct Mmg_t_ {
# define  MMG_REQ_ITEM_MAX      32
  //// the req
  MmgReq_t mmgReqs[MMG_REQ_ITEM_MAX];
  // book keeping
  void *mmgBufs[MMG_REQ_ITEM_MAX];  // the buf allocated/given for later release

  int reqNum;

  //// fixed req
  unsigned dmCtxtSize;
  unsigned mdsExtSize;
  unsigned gmLutSize;
  unsigned pq2gLutSize; // not used in float point case, otherwise, have to include KCdmModCtrl.h
  unsigned dmKsSize;
} Mmg_t;

#endif /* C_DM_MMG_H */
