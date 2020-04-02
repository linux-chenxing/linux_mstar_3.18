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
#ifndef K_C_DM_MOD_CTRL_H
#define K_C_DM_MOD_CTRL_H


#define MSTAR_DRIVER_MOD 1


////// DM version
#define DM_VER_CTRL             2   // 3 for 3.x, 2 for 2.x
#define DM_VER_CTRL_MINOR       120 // minor version, for DM_VER_CTRL 2 only 110 or 120 makes difference
                                     // for DM_VER_CTRL 3 only 0 and 1 is defined

////// For a typical set up, enable one of the following two macro.
// disable both will result in a dtv only solution
// enable both will get you an all in one solution,
// multi-scale is enabled when DM_DTV is on. to turn multi-scale off, use -msOff.
#define DM_STB 0

#define DM_DTV 1

////// float or fix point impl control

/****************************************************************************
* End of build configuration for targeted distribution
****************************************************************************/


/****************************************************************************
* Derived and fine tuned settings
* System Configurations
****************************************************************************/


#if DM_VER_CTRL == 2 && DM_VER_CTRL_MINOR != 110 && DM_VER_CTRL_MINOR != 120
#error  DM_VER_CTRL 2 only has DM_VER_CTRL_MINOR 1 and 2 defined
#endif

#define DM_VER_IS(mj_, mn_)   (DM_VER_CTRL == mj_ && DM_VER_CTRL_MINOR == mn_)
#define DM_VER_LOWER_THAN(mj_, mn_)   (DM_VER_CTRL < mj_ || (DM_VER_CTRL == mj_ && DM_VER_CTRL_MINOR < mn_))
#define DM_VER_LOWER_THAN212          (DM_VER_LOWER_THAN(2, 120))
#define DM_VER_HIGHER_THAN211         (!DM_VER_LOWER_THAN(2, 120))


////// Set for reduced complexity
#define REDUCED_COMPLEXITY      1

////// Set for matching float kernel to fixed
#define K_FLOAT_MATCH_FIXED     1

////// debug control
// to save binary debug result, VdrDM.cpp and kernels have to be synced. so define it here
// 1: enable save result in FloatComp_t into debug buf and then write to file
#define ENABLE_DBG_BUF          0

#define ENABLE_DBG_DM212        0 // debug the DM2.12 ksOMap in DM3 domain

#define EN_CHK_POINT            (1 && !KDM_FLOAT_POINT) // check point control
#define SIMPLE_CP_FILE_NAMING    0 // 0: extension .txt/.bin,  single component suffix _0, _1, _2
                                   // 1: .txt/.color(rgb, ipt, yuv, rgb, rgba, alpha), _r, _g, _b etc

// use external ccm, ignore metadata bit stream
#define DM_EXTERNAL_CCM         (1 && DM_FULL_SRC_SPEC)

// to support direct kernel matrix spec
#define EN_KS_DM_IN             1

  #define DmCfg_t       DmCfgFxp_t
  #define HDmCfg_t      HDmCfgFxp_t
  typedef struct DmCfgFxp_t_ *HDmCfgFxp_t;

  #define SrcSigEnv_t   SrcSigEnvFxp_t
  #define TgtSigEnv_t   TgtSigEnvFxp_t
  #define GrcSigEnv_t   GrcSigEnvFxp_t

  #define DmCtrl_t      DmCtrlFxp_t
  #define TmCtrl_t      TmCtrlFxp_t
  #define MsCtrl_t      MsCtrlFxp_t
  #define GdCtrl_t      GdCtrlFxp_t

  #define Dm_t          DmCtxtFxp_t
  #define HDm_t         HDmFxp_t
  #define DmCtxt_t      DmCtxtFxp_t
  typedef struct DmCtxtFxp_t_ *HDmFxp_t;

  #define DmKs_t        DmKsFxp_t
  #define HDmKs_t       HDmKsFxp_t
  typedef struct DmKsFxp_t_  *HDmKsFxp_t;
  #define DmKsBack_t    DmKsFxpBack_t
  #define HDmKsBack_t   HDmKsFxpBack_t
  typedef struct DmKsFxpBack_t_  *HDmKsFxpBack_t;

////// metadata bitstream endian
#define BIN_MD_BE                   1 // it is a big endian

////// if need specify full source info
#define DM_FULL_SRC_SPEC            1 // 1: can run without mds=>need CCM only

////// feature control
#define EN_GLOBAL_DIMMING           (1 && DM_DTV) // global dimming
#define EN_AOI                      (0 && (DM_VER_CTRL > 2)) // area of interesting

////// data path module selection
#define DM_SEC_INPUT                DM_STB  // if support second input channel
#define DM_SIMPLE_SEC_INPUT         (0 && DM_SEC_INPUT)  // 2nd to use 256 lut size and 2 LUTs(tmLutS and smLutS is all 1)
#define EN_GENERAL_SEC_INPUT        (1 && DM_SEC_INPUT) // 2nd just like 1st and can with separate alpha
#define MAP_2_INPUT_IPT             (1 && DM_VER_HIGHER_THAN211) // map to input inpt pq space
#define EN_UP_DOWN_SAMPLE_OPTION    1       // up/downsample support
#define EN_420_OUTPUT               (0 && EN_UP_DOWN_SAMPLE_OPTION) // if 420 output, not fully tested
#define EN_MS_OPTION                DM_DTV  // multi-scale support
#define EN_MS_FILTER_CFG            1                        // support configurable ms filter, only 1 for now
#define EN_NEW_IPT_RANGE            (0 && (DM_VER_CTRL >= 3))  // disable for now
#define EN_IPT_PQ_ONLY_OPTION       (0 && !DM_SEC_INPUT && (DM_VER_CTRL == 3))  // only IPT pq input is supported
#define EN_IPT_PQ_OUTPUT_SHORT_CUT  ((0 || EN_IPT_PQ_ONLY_OPTION) && (DM_VER_CTRL == 3)) // if run short cut data path for IPT PQ I/O
#define EN_IPT_PQ_INPUT_SHORT_CUT   ((0 || EN_IPT_PQ_ONLY_OPTION) && (DM_VER_CTRL == 3)) // if run short cut data path for IPT PQ I/O
#define EN_SKIP_TMAP_MS             0 //  if run short cut data path when CVM is identical
#define EN_RUN_MODE                 (1 && DM_DTV)
#define DM_HARDWIRE_BYPASS          (EN_RUN_MODE && 0 && (DM_VER_CTRL >= 3))
#define DM_SOFTWARE_BYPASS          (EN_RUN_MODE && (0 || !DM_HARDWIRE_BYPASS) && EN_UP_DOWN_SAMPLE_OPTION) // the 1 || is for testing purpose

#define MS_VERT_FILTER_FIRST        REDUCED_COMPLEXITY       // This applies fixed point only
#define EN_EDGE_SIMPLE              0
#define REDUCED_TC_LUT              REDUCED_COMPLEXITY       // This applies fixed point only. !DM_VER_IS(2, 110), float to fixed doesn't work
#define USE_12BITS_IN_3D_LUT        REDUCED_COMPLEXITY
#if REDUCED_COMPLEXITY
//Pick either 2 taps or 4 taps YUV chroma up sample filter
#define _USE_2_TAP_CHROMA_VERT_FILTER_
//#define _USE_4_TAP_CHROMA_VERT_FILTER_
#define MATCH_IP_CORE               0
#if MATCH_IP_CORE == 1
#ifdef MS_VERT_FILTER_FIRST
#undef MS_VERT_FILTER_FIRST
#endif
#define MS_VERT_FILTER_FIRST        0
#ifdef USE_12BITS_IN_3D_LUT
#undef USE_12BITS_IN_3D_LUT
#endif
#define USE_12BITS_IN_3D_LUT        0
#define USE_12BITS_MSB_IN_3D_LUT    1
#endif
#endif

// Set for IP RTL optimization. Need to turn REDUCED_COMPLEXITY off
#if REDUCED_COMPLEXITY == 0
#define IP_RTL_OPT                  0
#define USE_12BITS_MSB_IN_3D_LUT    IP_RTL_OPT
#if IP_RTL_OPT
#define _USE_2_TAP_CHROMA_VERT_FILTER_
#endif
#endif

// comapre with matlab
#if EN_GLOBAL_DIMMING && !K_FLOAT_MATCH_FIXED
#undef USE_12BITS_IN_3D_LUT
#undef USE_12BITS_MSB_IN_3D_LUT
#endif

#if DM_VER_IS(3, 1) && !K_FLOAT_MATCH_FIXED
#undef _USE_2_TAP_CHROMA_VERT_FILTER_
#endif

////// to support CUDA code
#ifndef __CUDA_ARCH__
# define CONST_MODIFIER const
#else
# define CONST_MODIFIER __constant__
#endif

////// limits
//// maximum supported frame size
#define DM_FRAME_SIZE_1080P       0
#define DM_FRAME_SIZE_2K          1
#define DM_FRAME_SIZE_4K          2

#define DM_FRAME_SIZE_SUPPORT      DM_FRAME_SIZE_4K

#define DM_ROW_NUM_4K    2160
#define DM_COL_NUM_4K    4096

#define DM_ROW_NUM_2K    1080
#define DM_COL_NUM_2K    2048

#define DM_ROW_NUM_1080P  1080
#define DM_COL_NUM_1080P  1920

//// in pixel size: up to 4K for now
#if (DM_FRAME_SIZE_SUPPORT == DM_FRAME_SIZE_4K)
# define DM_MAX_ROW_NUM    DM_ROW_NUM_4K
# define DM_MAX_COL_NUM    DM_COL_NUM_4K
#elif (DM_FRAME_SIZE_SUPPORT == DM_FRAME_SIZE_2K)
# define DM_MAX_ROW_NUM    DM_ROW_NUM_2K
# define DM_MAX_COL_NUM    DM_COL_NUM_2K
#else
# define DM_MAX_ROW_NUM    DM_ROW_NUM_1080P
# define DM_MAX_COL_NUM    DM_COL_NUM_1080P
#endif

#define DM_MAX_IMG_SIZE  (DM_MAX_ROW_NUM * DM_MAX_COL_NUM)

//// derfault frame size to use
#define DM_DEF_ROW_NUM    DM_ROW_NUM_1080P
#define DM_DEF_COL_NUM    DM_COL_NUM_1080P
#define DM_DEF_IMG_SIZE (DM_DEF_ROW_NUM * DM_DEF_COL_NUM)


//// tmKs lut size
#if DM_VER_CTRL == 3 || EN_GLOBAL_DIMMING
# define TM1_LUT_MAX_SIZE     512
#else
# define TM1_LUT_MAX_SIZE     4096
#endif


// TM_LUT_MAX_SIZE: obsolete

//// 3D gmLut size
# define GMLUT_MAX_DIM        17

#endif // K_C_DM_MOD_CTRL_H
