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
#ifndef VDRCPIP_H
#define VDRCPIP_H

#include "KdmCp.h"

#if EN_CHK_POINT

typedef struct IoEnv_t_    * HIoEnvCp;

#if defined(c_plusplus) || defined(__cplusplus)
extern "C"
{
#endif

void InitCpFileName(HIoEnvCp pIoEnv);
HCpIoCtrl CreateCpEnv(const HIoEnvCp pIoEnv, const char *pSClr, const char *pGClr, const char *TtClr);
void DestroyCpEnv(HCpIoCtrl pCpIoCtrl);

#if defined(c_plusplus) || defined(__cplusplus)
}
#endif

#define INIT_CP_FILE_NAME(pIoEnv)   InitCpFileName(pIoEnv)

#define CREATE_CP_ENV(pIoEnv, sClr, gClr,  tClr)  static const char *ClrName[CClrNum+1] =                                 \
                                            {"yuv", "rgb", "rgba", "ipt", "grey", "---"};                                   \
                                           pCpIoCtrl_ = CreateCpEnv(pIoEnv, ClrName[sClr], ClrName[gClr], ClrName[tClr]); \
                                           if (!pCpIoCtrl_) exit(-1)

#define DESTROY_CP_ENV()                   DestroyCpEnv(pCpIoCtrl_)

#else
#define INIT_CP_FILE_NAME(pIoEnv)

#endif // EN_CHK_POINT

#endif // VDRCPIP_H
