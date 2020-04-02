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
#ifndef VDRDMCLI_H
#define VDRDMCLI_H

#include "AdmType.h"
#include "VdrImgIo.h"
#if EN_CHK_POINT
#include "VdrCpIo.h"
#endif
//////////////////////// cli changable params ////////////////////////
typedef struct IoEnv_t_ {
  ////// runing I/O env
  NameStr_t prgName;
  //// dir spec
  std::string srcImgDir;  // source image dir
  std::string dstImgDir;  // dst image dir
  //// spec file direclty
  std::string configFile;  // config file
  std::string mdSpec;      // the metadata file / MDS dir spec

# if EN_CHK_POINT
  std::string cpMod;
  std::string cpRootFn[CpIdNum];
  bool cpEnable[CpIdNum];
  bool cpEnableAllOut;
  bool cpEnableAllIn;
# endif

# if DM_VER_LOWER_THAN212
  std::string gmLutFile;
# if EN_GLOBAL_DIMMING
  std::string gmLutFileA;
  std::string gmLutFileB;
  std::string pq2gLutFile;
# endif
# endif
  CFileFmt_t sFileFmt;
  CFileFmt_t tFileFmt;


  ////// frame to be processed ctrl, I/O
  int frmStartNum;// Frame start number to be processed
  int frmEndNum;  // Frame ending number to be processed
  int testFrameNum;  // for test: frame# to test
  int frmIntNum;     // test every frmIntNum

  // default behaviour: single in=>single out; mul in=>mul out
  bool singleFrmOut; // force single frame output in one file
  bool mulFrmOut;    // force multiple frames in one file
  // if both of above defined=>revert to default setting

  // Dump the kernel structure out
  std::string  dumpKsStr;
  unsigned dumpKsOpt;
    // 1xx run control and commit only
    // x1x dump kernel structure into file under the dir
    // x01 no tcLut dump
  std::string dumpKsDir;
  std::string loadKs;  // source binary kernel directory or file
  // Dump input image to ASCII files
  bool bDumpInput;
  // Take test input image from ASCII files
  bool bTestInput;

  // level
  // 0: output all,
  // 1: no per frame output
  int quietLevel;
  // over write mds: right now only 0 for no, 1 for frmFmt
  int owm;
  // back door: to skip unknow parameters
  bool bSkipUnknown;

  //// to handle Matlab cfg file only value
  int ChromaticAdaptation;
  int GlobalDimming;
} IoEnv_t;

// default to be consistent with pDmCfgFlt
void InitIoEnv(IoEnv_t *pIoEnv);
int DmParseCli(int argc, const char **argv, IoEnv_t *pIoEnv, DmCfg_t *pDmCfg);
// santity check for I/O related setting test, will exit if failed
int SanityCheckIoEnv(const HDmCfg_t hDmCfg, IoEnv_t *pIoEnv);
void ReportSrcSigEvn(const SrcSigEnv_t &sigEnv, const char *hdr);
void ReportSrcMdEvn (const SrcSigEnv_t &sigEnv, const char *hdr);
template<class T> void ReportTgtSigEvn(const T &sigEnv, const char *hdr);
void ReportSettings(const HDmCfg_t hDmCfg, const IoEnv_t &ioEnv);

#endif // VDRDMCLI_H
