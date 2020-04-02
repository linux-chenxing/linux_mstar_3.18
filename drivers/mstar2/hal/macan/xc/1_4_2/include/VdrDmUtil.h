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

#ifndef VDRDMUTIL_H_
#define VDRDMUTIL_H_
#include <string>
#include "KCdmModCtrl.h"

#ifdef WIN32
#define STRCASECMP _stricmp
#else
#define STRCASECMP strcasecmp
#endif

//// log and timer
void InitLogTimer(int enLog);
void StartLogTimer(void);
void ClearLogTimer(void);
void PauseLogTimer(void);
void ContinueLogTimer(void);
void ReportLogTimer(const char *fmt);
void ReportLogTimer(const char *fmt, int frameNum);
void DestroyLogTimer(void);

// if dir is valid
int CheckDir(std::string path);
// add slash if not ended with it
void AddSlash(std::string &path);
void AddSlash(char *path);

//// file name related
// get frame no from frame name
int GetFrmNo(const char *frmFn, int *pFrmNoDigits = 0, char *pExt = 0);
// replace externsion
int ReplaceExt(char *fn, const char *newExt); // inplace replace
int ReplaceExt(const char *fn, const char *newExt, char *newFn);

//// aligned memory alloc/free
/* ! allocate aligned memory

  This function allocate memory from heap

    @param[in] memSize memory size
    @param[in] MemAlign alignemt requirement, must be of value 2^n with n > 0
*/
extern void *AllocMemAlign(unsigned memSize, unsigned MemAlign);
extern void FreeMemAlign(void *ptr);


//// dump kernel structure
int DumpKsTxt(int dumpLut, const HDmKs_t hKs, const char* fn);
int DumpKsBin(const HDmKs_t hKs, const char* fn);

//// load kernel bin structure
int LoadKsBin(const HDmKs_t hKs, const char *fn);
HDmKsBack_t LoadKsBinBack(const HDmKs_t hKs, const char *fn);

#endif /* VDRDMUTIL_H_ */
