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

/*! @file VdrDmApi.h
    @brief DM API header file

    This header file defines the interface for DM API.

    @note

*/

#ifndef VDR_DM_API_H
#define VDR_DM_API_H

#include "CdmTypeFlt.h"
#include "CdmTypeFxp.h"
#include "CdmMmg.h"

/*! @brief DM configure/control handle

    It is a pointer to DM control structure.

    HDmCfg_t
*/

/*! @brief DM kernel handle

    It is a pointer to DM kernel context, a private structure for DM kernel.

    HDmKs_t
*/

/*! @brief DM handle

    It is a pointer to DM context, a private control structure for DM APIs.

    HDm_t
*/

/*! @brief DM metadata handle

    It is a pointer to a private DM metadata.

*/
typedef struct MdsExt_t_ *HMdsExt_t;

# if DM_VER_LOWER_THAN212
/*! @brief DM gmLut handle

    It is a pointer to a private DM gamut mapping lut.

*/
typedef struct GmLut_t_  *HGmLut_t;

#if (EN_GLOBAL_DIMMING && (CDM_FLOAT_POINT ==0))
/*! @brief DM pq2gLut handle

    It is a pointer to a private DM pq2gLut mapping lut.

*/
typedef struct Pq2GLut_t_ *HPq2GLut_t;
#endif
#endif


/*! @brief DM handle

    It is a pointer to DM context, a private control structure for DM APIs.

    HDm_t
*/

#if defined(c_plusplus) || defined(__cplusplus)
extern "C"
{
#endif


/*! @brief init DM control plan/configuration.

    This function sets default DM configuration used by Dolby. User may tailor it to
    specific system setup.

    @param[in] platform CPU, CUDA or OpenCl
    @param[in] hMmg memory manager handle
    @param[out] pDmCfg Pointer to DM config structure
    @return
        @li !NULL the given the input DM config pointer as a hanle
        @li NULL  fail to init DM config.
*/
extern HDmCfg_t InitDmCfg(CPlatform_t platform, HMmg_t hMmg, DmCfg_t *pDmCfg);

/*! @brief init DM data plan/kernel structure.

    This function init DM data plan and register its memory requirement

    @param[in] hDmCfg DM config handle
    @param[out] hDmKs DM kernel hanlde
    @return
        @li !NULL the given DM config hanle.
        @li NULL  fail to init DM handle.
*/
extern HDmKs_t InitDmKs(const HDmCfg_t hDmCfg, HDmKs_t hDmKs);

/*! @brief init DM context structure.

    This function init DM context and register its memory requirement(not impl yet)

    @param[in] hDmCfg DM config handle
    @param[out] hDm DM context hanlde. It is used to invoke the data plan.
    @return
        @li !NULL the given DM context hanle for DM control layer API calls.
        @li NULL  fail to init DM context.
*/
extern HDm_t InitDm(const HDmCfg_t hDmCfg, HDm_t hDm);

/*! @brief Convert control parameters to data path parameters.

    All in one function to convert all info from ctrl plan to data plan.

    @param[in] hDmCfg handle to DM control plan
    @param[in] hDmKs handle to DM data plan
    @param[out] hDm handle to DM context
    @return 0 success without any change from config.
    @return > 0 success with change from config. Change in flag FLAG_CHANGE_*
    @return < 0 failed
*/
int CommitDmCfg(const HDmCfg_t hDmCfg, HDmKs_t hDmKs, HDm_t hDm);


/*! @brief init metadata module.

    @param[in] hDmCfg handle to DM control plan
    @param[out] hMdsExt handle to DM metadata
    @return none
*/
extern void  InitMdsExt(const HDmCfg_t hDmCfg, HMdsExt_t hMdsExt);

/*! @brief parsing metadata.

    @param[in] owm if ignore some value in metadata. For test only
    @param[in] pDmCfg handle to DM config structure. For test only
    @param[in] buf contain metadata
    @param[in] dataSize data size in the buf
    @param[out] hMdsExt hanlde to metadata structure
    @return
        @li number of the bytes consumed after metadata is successfully parsed
        @li -1  fail.
*/
extern int ParseMds(unsigned char *buf, int dataSize, HMdsExt_t hMdsExt, int owm, const DmCfg_t *pDmCfg);

# if DM_VER_LOWER_THAN212
/*! @brief parse gm lut header.

    @param[in] buf contain gm lut header
    @param[in] dataSize gm lut deader size in the buf
    @param[out] hGmLut hanlde to gm lut
    @return
        @li 0  OK
        @li -1  fail.
*/
extern int ParseGmLutHdr(const unsigned char *buf, int dataSize, HGmLut_t hGmLut);

/*! @brief parse gm lut header.

    @param[in] lut contain the data of size returned by GetGmLutMapSize()
    @param[in] buf for parsed gm lut map of size returned by GetGmLutMapSize()
    @param[out] hGmLut hanlde to gm lut
    @return none
*/
extern void ParseGmLutMap(const unsigned short *lut, unsigned short *lutMap, int enMask, HGmLut_t hGmLut);

#if EN_GLOBAL_DIMMING
extern void ParsePq2GLut(const int32_t *lut, HPq2GLut_t hPq2GLut);
extern void GdSetGmLut(unsigned short *lutMap, HGmLut_t hGmLut, const HDmCfg_t hCfg, HDm_t hDm);
extern HGmLut_t GdGetGmLut(HDm_t hDm);
#endif

/*! @brief get gm lut map size
*/
extern unsigned GetGmLutMapSize(HGmLut_t hGmLut);
/*! @brief get gm lut map itself
*/
extern void *GetGmLutMap(HGmLut_t hGmLut);
#endif //DM_VER_LOWER_THAN212

/*! @brief Convert metadata stream parameters to data path parameters.

  This function converts metadata params to data plan params

    @param[in] hMdsExt handle to extended metadata structure
    @param[out] hDm handle to DM control structure
    @return 0 success without any change from last frame.
    @return >0 success with change from last frame. Change in flag FLAG_CHANGE_*
*/
extern int CommitMds(HMdsExt_t hMdsExt, HDm_t hDm);

/*! @brief get kernel handle after commit into context
    The handle can be use in DmProcess()
*/
extern HDmKs_t GetHDmKs(HDm_t hDm);

/*! @brief get control handle after commit into context
*/
extern HDmCfg_t GetHDmCfg(HDm_t hDm);


/*! @brief get pointer to source signal setting after commit into context
*/
extern const SrcSigEnv_t *GetSrcSigEnvPtr(HDm_t hDm);

/*! @brief get pointer to target signal setting after commit into context
*/
extern const TgtSigEnv_t *GetTgtSigEnvPtr(HDm_t hDm);

#if DM_HARDWIRE_BYPASS
/*! @brief DM process one frame

  These function are the interface to data path.

  DmBypass, if provided, simply converts 420 input into output 422 by row duplication and
  bit shifting to output bit depth. If input is already in 422, only shifting is performed

  Both input and output frame buffers must be allocated by caller.
  These are synchronized (blocking) call.

    @param[in] hKs handle ti the kernel
    @param[in] inBuf0 the interleaved frame or the first plan for planar weaved input frame.
    @param[in] inBuf1 the second plan for planar weaved input frame.
    @param[in] inBuf2 the third plan for planar weaved input frame.
    @param[out] outBuf0 the interleaved frame or the first plan for planar weaved output frame.
    @param[out] outBuf1 the seconf plan for planar weaved output frame.
    @param[out] outBuf2 the third plan for planar weaved output frame.
    @return
        @li 0 Succeed.
        @li <0 Error condition (not implement at moment).
*/
extern void
DmBypass(const HDmKs_t hKs,
         unsigned char *inBuf0, unsigned char *inBuf1, unsigned char *inBuf2,
         unsigned char *outBuf0, unsigned char *outBuf1, unsigned char *outBuf2
         );
#endif // DM_HARDWIRE_BYPASS

extern int
DmProcess(const HDmKs_t hKs,
          unsigned char *inBuf0, unsigned char *inBuf1, unsigned char *inBuf2,
          unsigned char *outBuf0, unsigned char *outBuf1, unsigned char *outBuf2
         );
extern int
DmProcessBack(const void *pKs,
          unsigned char *inBuf0, unsigned char *inBuf1, unsigned char *inBuf2,
          unsigned char *outBuf0, unsigned char *outBuf1, unsigned char *outBuf2
         );

// it cuda conterpart, if any


/*! @brief Synchronize source spec with metadata parameters

  This function synchronizes source information to metadata so the corresponding
  get funtion gets latest info.

    @param[in] hMdsExt handle to metadata structure
    @param[out] hDm handle to DM control structure
    @return 0 success.
*/
extern int SyncSigEnvWithMds(HMdsExt_t hMdsExt, HDm_t hDm);

/*! Init/Get functions

  These functions init/get default initial values to configure DM system
  To get the value updated by meta data bit stream, the corresponding Sync function
  must be called before the Get function.

    @param[in] hDm the DM context handle. Default to get init value
    @param[out] p* output property pointer.
    @return
        @li 0 Succeed.
        @li <0 Error condition (not implement at moment).
*/
extern void InitSrcSigEnv(SrcSigEnv_t *pSigEnv);
extern int GetSrcSigEnv(SrcSigEnv_t *pSigEnv, HDm_t hDm);

extern void InitTgtSigEnv(TgtSigEnv_t *pSigEnv);
extern int GetTgtSigEnv(TgtSigEnv_t *pSigEnv, HDm_t hDm);

extern void InitTmCtrl(TmCtrl_t *pTmCtrl);
extern int GetTmCtrl(TmCtrl_t *pTmCtrl, HDm_t hDm);

#if EN_MS_OPTION
extern void InitMsCtrl(MsCtrl_t *pMsCtrl);
extern int GetMsCtrl(MsCtrl_t *pMsCtrl, HDm_t hDm);
#endif

#if EN_GLOBAL_DIMMING
extern void InitGdCtrl(GdCtrl_t *pGdCtrl);
extern int GetGdCtrl(GdCtrl_t *pGdCtrl, HDm_t hDm);
#endif

extern void InitDmCtrl(DmCtrl_t *pDmCtrl);
extern int GetDmCtrl(DmCtrl_t *pDmCtrl, HDm_t hDm);

extern int GetDmCfg(DmCfg_t *pDmCfg, HDm_t hDm);

////// help function for frame memory handling
// return frame buffer required with compact layout setting
extern unsigned GetCompactFrmMemLayout(
  int rowNum, int colNum, CClr_t clr, CChrm_t chrm, CWeav_t weav, CDtp_t dtp,
  int *pRowPitch, int *pRowPitchC);

extern unsigned GetMinFrmMemSize(
  int rowNum, int colNum, CClr_t clr, CChrm_t chrm, CWeav_t weav, CDtp_t dtp);

extern int UpdateSrcEnvFrameMemLayout(HDm_t hDm);
extern int UpdateKsInFrameMemLayout(int rowPitch, int rowPitchC, HDm_t hDm);

#if EN_GLOBAL_DIMMING
int GetGdActiveFltrdMean(uint16_t *pFmPqDn, uint32_t *pFm, HDm_t hDm); // to obsolete
int GetGdActiveFltrdAnchor(uint16_t *pFmPqDn, uint32_t *pFm, HDm_t hDm);
int GetGdActiveTgtWindow(uint32_t *pWMin, uint32_t *pWMax, HDm_t hDm);
#endif
int GetTcLutDir(HDm_t hDm);

#if EN_RUN_MODE
// return level 255 metadata ifpresent, otherwise return the configured mode
int GetRunMode(HDm_t hDm);
#endif

// Utility functions
extern uint32_t PQ12ToL(uint16_t u16_x0);
extern uint16_t LToPQ12(uint32_t u32_x0);
extern int32_t DumpKsBinBuf(const HDmKs_t hKs, char *outBuf);
extern int32_t LoadKsBinBuf(const HDmKs_t hKs, char *inBuf);
extern HDmKsBack_t LoadKsBinBufBack(const HDmKs_t hKs, char *inBuf);

/*! @brief Set hardware interface registers
    This function shows firmware and hardware interfacing, which may need to tweak to particular hardware.
    It assumes hardware interface register map is same as DmKsFxp_t.
    @param[in] pKs the DM kernel structure
    @param[out] pRegStartAddr pointer to hardware registers
*/
extern void SetHardwareInterface(const HDmKs_t pKs, uint8_t *pRegStartAddr);

#ifdef __cplusplus
}
#endif

#endif /* VDR_DM_API_H */
