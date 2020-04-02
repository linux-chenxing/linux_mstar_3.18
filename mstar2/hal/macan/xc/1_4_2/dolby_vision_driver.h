#ifndef _DOLBY_VISION_H_
#define _DOLBY_VISION_H_

//#define DEBUG_DOLBY_VISION_ENABLE

// =====================================================================================
// HDR Vision Instances and Functions
// =====================================================================================
#define DOLBY_3DLUT_DIM0        (17)
#define DOLBY_3DLUT_DIM1        (17)
#define DOLBY_3DLUT_DIM2        (17)
#define DOLBY_3DLUT_SIZE        (DOLBY_3DLUT_DIM0 * DOLBY_3DLUT_DIM1 * DOLBY_3DLUT_DIM2)

#define DOLBY_DS_VERSION 2
// Composer

typedef enum DoVi_DM_InputType_t_
{
    DOVI_DM_INPUT_TYPE_OTT = 0,
    DOVI_DM_INPUT_TYPE_HDMI = 1,
    DOVI_DM_INPUT_TYPE_RESERVED = 2

} DM_InputType_t;

typedef struct DoVi_Config_t_
{
    // Basic SPEC
    MS_U8 u8InputBits; // Input bit depth of DM
    MS_U8 u8OutputBits; // Output bit depth of DM
    MS_PHY phyLutBaseAddr; // DRAM base address for degamma, gamma, TMO, 3DLUT
    MS_U8 u8Lut3DIdx; // Select 3DLUT

    // Video SPEC : will be changed by metadatra
    MS_U16 u16SminPQ; // = 62;   // 12b
    MS_U16 u16SmaxPQ; // = 2081; // 100 -> 2081, 500 -> 2771, 600 -> 2851, 1000 -> 3079, 2000 -> 3388, 4000 -> 3696
    MS_U16 u16Sgamma; // = 39322; // Format 2.14
    MS_U8  u8SMode; // = 0; // 0 Rec709
    //MS_U8  u8SBitDepth; // = 16;
    MS_U8  u8SEOTF; // = 0; // // Bt1886 0, PQ 1
    MS_U8  u8SSignalRange; // = 1; // Narrow 0, Full 1, SDI 2
    MS_U16 u16Sdiagonalinches; // = 42;

    // Panel SPEC
    MS_S32 s32CrossTalk; // = 655 // =0.02  (0.15)
    MS_U16 u16TminPQ; // = 62;   // 12b
    MS_U16 u16TmaxPQ; // = 2081; // 100 -> 2081, 500 -> 2771, 600 -> 2851, 1000 -> 3079, 2000 -> 3388, 4000 -> 3696
    MS_U16 u16Tgamma; // = 39322; // Format 2.14
    MS_U8  u8TMode; // = 0; // 0 Rec709, 1 P3D65, 2 Rec2020, 3 ACES
    //MS_U8  u8TBitDepth; // = 16;
    MS_U8  u8TEOTF; // = 0; // // 0 Bt1886, 1 PQ
    MS_U8  u8TSignalRange; // = 1; // Narrow 0, Full 1, SDI 2
    MS_U16 u16Tdiagonalinches; // = 42;

    // Tone Mapping SPEC
    MS_U8  u8RunMode;
    MS_S16 s16TMinPQBias; // = 0
    MS_S16 s16TMidPQBias; // = 0
    MS_S16 s16TMaxPQBias; // = 0
    MS_S16 s16TrimSlopeBias; // = 0
    MS_S16 s16TrimOffsetBias; // = 0
    MS_S16 s16TrimPowerBias; // = 0
    MS_S16 s16TContrast; // = 0
    MS_S16 s16TBrightness; // = 0
    MS_S32 s32Rolloff; // = 1/3

    // Detail Restoration SPEC
    MS_U8  u8MsMethod; // = 4 // 0 : MS_METHOD_OFF 4 : MS_METHOD_DB_EDGE
    MS_S16 s16MsWeight; // = 2048
    MS_S16 s16MsWeightBias; // = 0
    MS_U16 u16MsWeightEdge; // = 16380

    // Adjustment SPEC
    MS_U16 u16CrossTalk; // = 655 // = 0.02 // Format 1.15
    MS_U16 u16ChromaWeight; // = 0 //
    MS_S16 s16ChromaWeightBias; // = 0
    MS_U16 u16CcGain; // = 2048 // = 0.5 // Format 0.12
    MS_U16 u16CcOffset; // = 4095 // = 1.0 // Format 0.15
    MS_U16 u16SaturationGain; // = 4095
    MS_S16 s16SaturationGainBias; // = 0

    // 3DLUT SPEC
    MS_S16 s16Lut3DMin[3];
    MS_S16 s16Lut3DMax[3];

    // INPUT TYPE
    //0 : OTT
    //1: HDMI
    MS_U8  u8InputType;

} DoVi_Config_t;

// Utility
void DoVi_isOpenHdr(MS_BOOL);
void MsHdr_CompAllocRegTable(void);
void MsHdr_CompFreeRegTable(void);
// Function for debug
void DoVi_CompDumpConfig(void);
// Function called after system boot
void DoVi_CompSetDefaultConfig(void);
// Function called after metadata ready
int DoVi_CompReadMetadata(const MS_U8* pu8MDS, MS_U16 u16Len);
// Function called after /DoVi_DmReadMetadata
void DoVi_CompFrameDeCalculate(void);
// Function called at blanking interrupt
void DoVi_CompBlankingUpdate(void);

// DM

// Utility
void MsHdr_DmAllocRegTable(void);
void MsHdr_DmFreeRegTable(void);
// Function called after system boot
void DoVi_DmSetDefaultConfig(void);
// Function called after metadata ready
int DoVi_DmSetDolbyMetaData(MS_U8* pu8Data, MS_U32 u32Size);
// Function called after /DoVi_DmReadMetadata
void DoVi_DmFrameDeCalculate(void);
// Function called at blanking interrupt
void DoVi_DmBlankingUpdate(void);

// Call this to update 3DLUT
int DoVi_Replace_3DLut(const MS_U8* pu8Array, MS_U32 u32Size);
void DoVi_Prepare_DS(MS_U8 u8Version);
void DoVi_GetCmdCnt(K_XC_DS_CMDCNT *pstXCDSCmdCnt);
// sets information about it.
void DoVi_Set_HDRInputType(MS_U8* pConfig, MS_U8 u8InputType);
void DoVi_setHdrMetadata(MS_U8* pu8Data);
void DoVi_UpdateBackLight(MS_U16  u16BackLightUiVal);
void DoVi_UpdateBrightness(MS_U16  u16BrightnessUiVal);
void DoVi_UpdateContrast(MS_U16  u16ContrastUiVal);
MS_U16 DoVi_Backlight_Value(void);
void DoVi_UpdateViewMode(MS_U8 u8ModeId);
MS_BOOL DoVi_IsSupportGD();
#endif
