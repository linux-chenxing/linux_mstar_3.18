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
#ifndef VDRIMGIO_H
#define VDRIMGIO_H

#include "AdmType.h"

typedef enum {
  CFileFmtRaw = 0, 
  CFileFmtTiff, 
  CFileFmtNum = 2+1  // +1 one for 'tif' mapping
} CFileFmt_t;

typedef enum {
  CFileClrYuv = 0, 
  CFileClrRgb, 
  CFileClrIpt, 
  CFileClrNum
} CFileClr_t;

////// init/destroy image I/O
// input selection
// <s|t>Rgb = 1: for rgb/rgba, 0 for YUV, 
// <s|t>Ffmt for tiff/tif or raw format for rgb case
int  InitVdrImgIo(const char *srcImgDir, const char *dstImgDir, 
      CFileClr_t sFileClr, CFileFmt_t sFileFmt, CFileClr_t tFileClr, CFileFmt_t tFileFmt);
void DestroyVdrImgIo(void);

////// get tiff file name sequentailly
// convert frame number into non-fully qualified file name
const char *GetInputImgName(int frmNo);
// sequentially get non-fully qualified file name under source dir
int GetInputImgName(char *fn);

// local name to fully qualified name
int GetInputImgNameFqNoExt(int frmNo, char *fullFn);
int GetInputImgNameFq(int frmNo, char *fullFn);
int GetInputImgNameFq(const char *fnIn, char *fullFn);

////// frame format based. Frame buffer shall preallocated already
//// with fully qualified name. clor spec must match name extension
// tiff: format convertion, in file U16 MSB aligned
int ReadFromTIFF(const char *tiffFn, const IoFrmFmt_t *pFrmFmt); 
int WriteToTIFF (const char *tiffFn, const IoFrmFmt_t *pFrmFmt);
// raw rgb: no format conversion, data in file matches frame format
int ReadFromRGB(const char *rgbFn, const IoFrmFmt_t *pFrmFmt);
int WriteToRGB (const char *rgbFn, const IoFrmFmt_t *pFrmFmt);
// raw yuv: no format conversion, data in file matches frame format
int ReadFromYUV(const char *yuvFn, const IoFrmFmt_t *pFrmFmt);
int WriteToYUV (const char *yuvFn, const IoFrmFmt_t *pFrmFmt);
//// that only whether extension is .tiff or not matters
// graphics
int InputVdrImgFullFnExt(const char *fullFn, const IoFrmFmt_t *pFrmFmt);
int OutputVdrImgFullFnExt(const char *fullFn, const IoFrmFmt_t *pFrmFmt);
// alpha channel
int InputAlphaFullFn(const char *alpFile, const IoFrmFmt_t *pFrmFmtA);

//// with local name or frame number
// input frame and converted it according to the pFrmFmt for tiff
int InputVdrImg(int frmNo,       const IoFrmFmt_t *pFrmFmt);
int InputVdrImg(const char *fn,  const IoFrmFmt_t *pFrmFmt);
// output frame and perform convertion if necessary for tiff
int OutputVdrImg(int frmNo,      const IoFrmFmt_t *pFrmFmt);
int OutputVdrImg(const char *fn, const IoFrmFmt_t *pFrmFmt);
// input image from source according to the ext in fn
int InputVdrImgExt(const char *fn, const IoFrmFmt_t *pFrmFmt);
// output file to destination dir with given extension instead of stored one
int OutputVdrImgExt(const char *fn, const IoFrmFmt_t *pFrmFmt);

// with fully qualified name
int InputGrOverlayFullFn(const char *grFile, const IoFrmFmt_t *pFrmFmtGr);
// with local name video file name
int InputGrOverlay(const char *grFile, const IoFrmFmt_t *pFrmFmtGr);

// MATLAB code can load the following files for comparision
// save binary component in row order, input data data layout is Matrix[DM_MAX_ROW_NUM][DM_MAX_COL_NUM]. 
template <class T>
void SaveBinCompExt(const char *fn, T *comp, int rowNum, int colNum, int outHdr = 1);
// save binary image in row order, input data layout is Matrix[rowNumTtl][colNumTtl]. 
// fn name extension will change to bin8/4/2/1 according to data size
template <class T>
void SaveBinImg(const char *fn, T *imgR, T *imgG, T *imgB, int rowNum, int colNum, 
                int rowPitchNum, int outHdr = 1);


#endif // VDRIMGIO_H
