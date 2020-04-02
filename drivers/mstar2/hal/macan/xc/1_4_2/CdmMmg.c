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
//#include <assert.h>

#include "CdmMmg.h"
#include "KCdmModCtrl.h"
#include "CdmTypePriFxp.h"
#include "KdmTypeFxp.h"

HMmg_t InitMmg(Mmg_t *pMmg)
{
  pMmg->dmCtxtSize = sizeof(DmCtxt_t);
  pMmg->mdsExtSize = sizeof(MdsExt_t);

# if DM_VER_LOWER_THAN212
  pMmg->gmLutSize = sizeof(GmLut_t);
  pMmg->pq2gLutSize = sizeof(Pq2GLut_t);
# endif

  pMmg->dmKsSize = sizeof(DmKs_t);

  return pMmg;
}

// register a request
int RegMemReq(unsigned size, unsigned align, int inDev,
  void *pCbParam, MmgCbFun_t *pCbFun, HMmg_t hMmg)
{
  MmgReq_t *pItem;

  assert(hMmg->reqNum < MMG_REQ_ITEM_MAX);

  if (hMmg->reqNum == MMG_REQ_ITEM_MAX) {
    return -1;
  }
  if (!align) {
    align = 4;
  }

  pItem = hMmg->mmgReqs + hMmg->reqNum;

  pItem->size      = size;
  pItem->align    = align;
  pItem->inDev    = inDev;
  pItem->pCbParam  = pCbParam;
  pItem->pCbFun    = pCbFun;

  hMmg->mmgBufs[hMmg->reqNum] = 0;

  ++hMmg->reqNum;

  return 0;
}

int MemAssignCbFunFc(void *pBuf, void *buf)
{
  *((void **)pBuf) = (void *)buf;

  return 0;
}

