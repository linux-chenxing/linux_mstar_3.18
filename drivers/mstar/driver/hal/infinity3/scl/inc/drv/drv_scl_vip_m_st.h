////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2011 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// ("MStar Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////
#ifndef _MDRV_VIP_ST_H
#define _MDRV_VIP_ST_H

//=============================================================================
// Defines
//=============================================================================
#define MDRV_SCLVIP_LCE_CURVE_SECTION_NUM       16          ///< MDRV_SCLVIP_LCE_CURVE_SECTION_NUM
#define MDRV_SCLVIP_PEAKING_BAND_NUM            8           ///< MDRV_SCLVIP_PEAKING_BAND_NUM
#define MDRV_SCLVIP_PEAKING_ADP_Y_LUT_NUM       8           ///< MDRV_SCLVIP_PEAKING_ADP_Y_LUT_NUM
#define MDRV_SCLVIP_PEAKING_BAND_TERM_NUM       16          ///< MDRV_SCLVIP_PEAKING_BAND_TERM_NUM
#define MDRV_SCLVIP_DLC_HISTOGRAM_SECTION_NUM   7           ///< MDRV_SCLVIP_DLC_HISTOGRAM_SECTION_NUM
#define MDRV_SCLVIP_DLC_HISTOGRAM_REPORT_NUM    8           ///< MDRV_SCLVIP_DLC_HISTOGRAM_REPORT_NUM
#define MDRV_SCLVIP_DLC_LUMA_SECTION_NUM        64          ///< MDRV_SCLVIP_DLC_LUMA_SECTION_NUM
#define MDRV_SCLVIP_IHC_COLOR_NUM               16          ///< MDRV_SCLVIP_IHC_COLOR_NUM
#define MDRV_SCLVIP_IHC_USER_COLOR_NUM          16          ///< MDRV_SCLVIP_IHC_USER_COLOR_NUM
#define MDRV_SCLVIP_ICE_COLOR_NUM               16          ///< MDRV_SCLVIP_ICE_COLOR_NUM
#define MDRV_SCLVIP_IBC_COLOR_NUM               16          ///< MDRV_SCLVIP_IBC_COLOR_NUM
#define MDRV_SCLVIP_FCC_YWIN_LUT_ENTRY_NUM      17          ///< MDRV_SCLVIP_FCC_YWIN_LUT_ENTRY_NUM
#define MDRV_SCLVIP_NLM_DISTWEIGHT_NUM          9           ///< MDRV_SCLVIP_NLM_DISTWEIGHT_NUM
#define MDRV_SCLVIP_NLM_WEIGHT_NUM              32          ///< MDRV_SCLVIP_NLM_WEIGHT_NUM
#define MDRV_SCLVIP_NLM_LUMAGAIN_NUM            64          ///< MDRV_SCLVIP_NLM_LUMAGAIN_NUM
#define MDRV_SCLVIP_NLM_POSTLUMA_NUM            16          ///< MDRV_SCLVIP_NLM_POSTLUMA_NUM
#define MDRV_SCLVIP_CMDQ_MEM_256K               0x0040000   ///< MDRV_SCLVIP_CMDQ_MEM_256K
#define MDRV_SCLVIP_CMDQ_MEM_128K               0x0020000   ///< MDRV_SCLVIP_CMDQ_MEM_128K
#define MDRV_SCLVIP_CMDQ_MEM_64K                0x0010000   ///< MDRV_SCLVIP_CMDQ_MEM_64K
#define MDRV_SCLVIP_CMDQ_MEM_32K                0x0008000   ///< MDRV_SCLVIP_CMDQ_MEM_32K
#define MDRV_SCLVIP_CMDQ_MEM_16K                0x0004000   ///< MDRV_SCLVIP_CMDQ_MEM_16K
#define MDRV_SCLVIP_CMDQ_MEM_TEST               0x0001000   ///< MDRV_SCLVIP_CMDQ_MEM_TEST
#define MDRV_SCLVIP_VTRACK_KEY_SETTING_LENGTH       8       ///< MDRV_SCLVIP_VTRACK_KEY_SETTING_LENGTH
#define MDRV_SCLVIP_VTRACK_SETTING_LENGTH           23      ///< MDRV_SCLVIP_VTRACK_SETTING_LENGTH

//
//=============================================================================
// enum
//=============================================================================
/**
* Used to setup vsrc of vip device
*/
typedef enum
{
    E_MDRV_SCLVIP_SRC_ISP,  ///< VIP src ISP
    E_MDRV_SCLVIP_SRC_BT656,///< VIP src BT656
    E_MDRV_SCLVIP_SRC_DRAM, ///< VIP src DRAM
    E_MDRV_SCLVIP_SRC_NUM,  ///< VIP src max number
}__attribute__ ((__packed__))MDrvSclVipSrcType_e;

/**
* Used to setup LCE AVE of vip device
*/
typedef enum
{
    E_MDRV_SCLVIP_LCE_Y_AVE_5X11   = 0x0,      ///<  mask 0x10
    E_MDRV_SCLVIP_LCE_Y_AVE_5X7    = 0x10,     ///<  mask 0x10
}__attribute__ ((__packed__))MDrvSclVipLceYAveSelType_e;


/**
* Used to setup UVC_ADP_Y_INPUT_SEL of vip device
*/
typedef enum
{
    E_MDRV_SCLVIP_UVC_ADP_Y_INPUT_SEL_UVC_LOCATE    = 0x0,      ///<  mask 0xC0
    E_MDRV_SCLVIP_UVC_ADP_Y_INPUT_SEL_RGB_Y_OUTPUT  = 0x40,     ///<  mask 0xC0
    E_MDRV_SCLVIP_UVC_ADP_Y_INPUT_SEL_DLC_Y_INPUT   = 0x80,     ///<  mask 0xC0
    E_MDRV_SCLVIP_UVC_ADP_Y_INPUT_SEL_RGB_Y_INPUT   = 0xC0,     ///<  mask 0xC0
}__attribute__ ((__packed__))MDrvSclVipUvcAdpYInputSelType_e;

/**
* Used to setup LDC_BYPASS of vip device
*/
typedef enum
{
    E_MDRV_SCLVIP_LDC_MENULOAD,         ///< no bypass
    E_MDRV_SCLVIP_LDC_BYPASS,           ///< bypass
    E_MDRV_SCLVIP_LDC_BYPASS_TYPE_NUM,  ///< no use
}__attribute__ ((__packed__))MDrvSclVipLdcBypassType_e;
/**
* Used to setup LDCLCBANKMODE of vip device
*/
typedef enum
{
    E_MDRV_SCLVIP_LDCLCBANKMODE_64, ///< 64p
    E_MDRV_SCLVIP_LDCLCBANKMODE_128, ///< 128p
}__attribute__ ((__packed__))MDrvSclVipLdcLcBankModeType_e;

/**
* Used to setup VIP_LDC_422_444 of vip device
*/
typedef enum
{
    E_MDRV_SCLVIP_LDC_422_444_DUPLICATE = 0x1,  ///< mask 0x3
    E_MDRV_SCLVIP_LDC_422_444_QUARTER   = 0x2,  ///< mask 0x3
    E_MDRV_SCLVIP_LDC_422_444_AVERAGE   = 0x3,  ///< mask 0x3
}__attribute__ ((__packed__))MDrvSclVipLdc422To444Type_e;
/**
* Used to setup VIP_LDC_444_422 of vip device
*/
typedef enum
{
    E_MDRV_SCLVIP_LDC_444_422_DROP      = 0x0,  ///< mask 0x1C
    E_MDRV_SCLVIP_LDC_444_422_AVERAGE   = 0x4,  ///< mask 0x1C
}__attribute__ ((__packed__))MDrvSclVipLdc444To422Type_e;
/**
* Used to VIP_NLM_Average of vip device
*/
typedef enum
{
    E_MDRV_SCLVIP_NLM_Average_3x3_mode = 0x0,   ///< mask 0x2
    E_MDRV_SCLVIP_NLM_Average_5x5_mode = 0x2,   ///< mask 0x2
}__attribute__ ((__packed__))MDrvSclVipNlmAverageType_e;
/**
* Used to NLM_DSW of vip device
*/
typedef enum
{
    E_MDRV_SCLVIP_NLM_DSW_16x8_mode     = 0x0,  ///< mask 0x20
    E_MDRV_SCLVIP_NLM_DSW_32x16_mode    = 0x20, ///< mask 0x20
}__attribute__ ((__packed__))MDrvSclVipNlmDswType_e;
/**
* Used to VIP_FCC_Y of vip device
*/
typedef enum
{
    E_MDRV_SCLVIP_FCC_Y_DIS_CR_DOWN,            ///< cr down
    E_MDRV_SCLVIP_FCC_Y_DIS_CR_UP,              ///< cr up
    E_MDRV_SCLVIP_FCC_Y_DIS_CB_DOWN,            ///< cb down
    E_MDRV_SCLVIP_FCC_Y_DIS_CB_UP,              ///< cb up
    E_MDRV_SCLVIP_FCC_Y_DIS_NUM,                ///< 4type
}__attribute__ ((__packed__))MDrvSclVipFccYDisType_e;
/**
* Used to setup IHC_ICE_ADP_Y of vip device
*/
typedef enum
{
    E_MDRV_SCLVIP_IHC_ICE_ADP_Y_SECTION_0,      ///< section
    E_MDRV_SCLVIP_IHC_ICE_ADP_Y_SECTION_1,      ///< section
    E_MDRV_SCLVIP_IHC_ICE_ADP_Y_SECTION_2,      ///< section
    E_MDRV_SCLVIP_IHC_ICE_ADP_Y_SECTION_3,      ///< section
    E_MDRV_SCLVIP_IHC_ICE_ADP_Y_SECTION_NUM,    ///< section
}__attribute__ ((__packed__))MDrvSclVipIhcIceAdpYSectionType_e;
/**
* Used to setup suspend of vip device
*/
typedef enum
{
    E_MDRV_SCLVIP_ACK_CONFIG           = 0x1,      ///< ACK
    E_MDRV_SCLVIP_IBC_CONFIG           = 0x2,      ///< IBC
    E_MDRV_SCLVIP_IHCICC_CONFIG        = 0x4,      ///< ICCIHC
    E_MDRV_SCLVIP_ICC_CONFIG           = 0x8,      ///< ICE
    E_MDRV_SCLVIP_IHC_CONFIG           = 0x10,     ///< IHC
    E_MDRV_SCLVIP_FCC_CONFIG           = 0x20,     ///< FCC
    E_MDRV_SCLVIP_UVC_CONFIG           = 0x40,     ///< UVC
    E_MDRV_SCLVIP_DLC_HISTOGRAM_CONFIG = 0x80,    ///< HIST
    E_MDRV_SCLVIP_DLC_CONFIG           = 0x100,     ///< DLC
    E_MDRV_SCLVIP_LCE_CONFIG           = 0x200,    ///< LCE
    E_MDRV_SCLVIP_PEAKING_CONFIG       = 0x400,    ///< PK
    E_MDRV_SCLVIP_NLM_CONFIG           = 0x800,    ///< NLM
    E_MDRV_SCLVIP_LDC_MD_CONFIG        = 0x1000,   ///< LDCMD
    E_MDRV_SCLVIP_LDC_DMAP_CONFIG      = 0x2000,   ///< LDCDMAP
    E_MDRV_SCLVIP_LDC_SRAM_CONFIG      = 0x4000,   ///< LDC SRAM
    E_MDRV_SCLVIP_LDC_CONFIG           = 0x8000,   ///< LDC
    E_MDRV_SCLVIP_CONFIG               = 0x10000,  ///< 19 bit to control 19 IOCTL
    E_MDRV_SCLVIP_MCNR_CONFIG          = 0x20000,  ///< DNR
}__attribute__ ((__packed__))MDrvSclVipConfigType_e;
/**
* Used to setup the vtrack status of vip device
*/
typedef enum
{
    E_MDRV_SCLVIP_VTRACK_ENABLE_ON,      ///< Vtrack on
    E_MDRV_SCLVIP_VTRACK_ENABLE_OFF,     ///< Vtrack off
    E_MDRV_SCLVIP_VTRACK_ENABLE_DEBUG,   ///< Vtrack debug
}MDrvSclVipVtrackEnableType_e;
typedef enum
{
    E_MDRV_SCLVIP_DAZA_BMCNR        = 0x00000002,
    E_MDRV_SCLVIP_DAZA_BNLM         = 0x00000004,
    E_MDRV_SCLVIP_DAZA_BXNR         = 0x00000008,
    E_MDRV_SCLVIP_DAZA_BGMA2C       = 0x00000010,
    E_MDRV_SCLVIP_DAZA_BGMC2A       = 0x00000020,
}MDrvSclVipDazaEvent_e;

/**
* Used to setup the AIP  of vip device
*/
typedef enum
{
    E_MDRV_SCLVIP_AIP_YEE = 0,           ///< yee
    E_MDRV_SCLVIP_AIP_YEE_AC_LUT,        ///< yee ac lut//1
    E_MDRV_SCLVIP_AIP_WDR_GLOB,          ///< wdr glob
    E_MDRV_SCLVIP_AIP_WDR_LOC,           ///< wdr loc
    E_MDRV_SCLVIP_AIP_MXNR,              ///< mxnr
    E_MDRV_SCLVIP_AIP_UVADJ,             ///< uvadj
    E_MDRV_SCLVIP_AIP_XNR,               ///< xnr
    E_MDRV_SCLVIP_AIP_YCUVM,             ///< ycuvm
    E_MDRV_SCLVIP_AIP_COLORTRAN,         ///< ct
    E_MDRV_SCLVIP_AIP_GAMMA,             ///< gamma
    E_MDRV_SCLVIP_AIP_422TO444,          ///< 422to444 //10
    E_MDRV_SCLVIP_AIP_YUVTORGB,          ///< yuv2rgb
    E_MDRV_SCLVIP_AIP_GM10TO12,          ///< 10 to 12
    E_MDRV_SCLVIP_AIP_CCM,               ///< ccm
    E_MDRV_SCLVIP_AIP_HSV,               ///< hsv
    E_MDRV_SCLVIP_AIP_GM12TO10,          ///< gm12to10//15
    E_MDRV_SCLVIP_AIP_RGBTOYUV,          ///< rgb2yuv
    E_MDRV_SCLVIP_AIP_444TO422,          ///< 4442422
    E_MDRV_SCLVIP_AIP_NUM,               ///< Num
}MDrvSclVipAipType_e;
typedef enum
{
    E_MDRV_SCLVIP_GAMMA_ENABLE_GM10TO12 = 0x1,          ///< 10 to 12
    E_MDRV_SCLVIP_GAMMA_ENABLE_GM12TO10 = 0x2,          ///< gm12to10//15
}MDrvSclVipGammaEnableType_e;

typedef enum
{
    E_MDRV_SCLVIP_CMDQ_CHECK_RETURN_ORI        = 0,     // reset before test
    E_MDRV_SCLVIP_CMDQ_CHECK_PQ                = 1,     // autotest PQ
    E_MDRV_SCLVIP_CMDQ_CHECK_ALREADY_SETINNG   = 2,     // test CMDQ
    E_MDRV_SCLVIP_CMDQ_CHECK_AUTOSETTING       = 3,     // autotest CMDQ (value = mask)
}MDrvSclVipCmdqCheckType_e;
typedef enum
{
    E_MDRV_SCLVIP_RESET_ZERO = 0,
    E_MDRV_SCLVIP_RESET_ALREADY ,
    E_MDRV_SCLVIP_FILL_GOLBAL_FULL ,
}MDrvSclVipResetType_e;
typedef enum
{
    E_MDRV_SCLVIP_AIP_SRAM_GAMMA_Y, ///< gamma y
    E_MDRV_SCLVIP_AIP_SRAM_GAMMA_U, ///< gamma u
    E_MDRV_SCLVIP_AIP_SRAM_GAMMA_V, ///< gamma v
    E_MDRV_SCLVIP_AIP_SRAM_GM10to12_R, ///< gamma R
    E_MDRV_SCLVIP_AIP_SRAM_GM10to12_G, ///< gamma G
    E_MDRV_SCLVIP_AIP_SRAM_GM10to12_B, ///< gamma B
    E_MDRV_SCLVIP_AIP_SRAM_GM12to10_R, ///< gamma R
    E_MDRV_SCLVIP_AIP_SRAM_GM12to10_G, ///< gamma G
    E_MDRV_SCLVIP_AIP_SRAM_GM12to10_B, ///< gamma B
    E_MDRV_SCLVIP_AIP_SRAM_WDR, ///< wdr
}MDrvSclVipAipSramType_e;


//=============================================================================
// struct
//=============================================================================
// MDRV_VIP_MDRV_SET_DNR_CONFIG

typedef struct
{
    u32 u32CMDQ_Phy;
    u32 u32CMDQ_Size;
    u32 u32CMDQ_Vir;

}MDrvSclVipCmdqInitConfig_t;

typedef struct
{
    u32 u32RiuBase;
    MDrvSclVipCmdqInitConfig_t CMDQCfg;
}MDrvSclVipInitConfig_t;

typedef struct
{
    u32 u32StructSize;
    u8   *pGolbalStructAddr;
    bool bSetConfigFlag;
    u8  *pPointToCfg;
    u32 enPQIPType;
    u8  (*pfForSet)(void *);
}MDrvSclVipSetPqConfig_t;
typedef struct
{
    u32 u32Viraddr; ///< AIP setting
    MDrvSclVipAipSramType_e enAIPType;
} __attribute__ ((__packed__)) MDrvSclVipAipSramConfig_t;

typedef struct
{
    u32 u32StructSize;
    u32 *pVersion;
    u32 u32VersionSize;
}MDrvSclVipVersionChkConfig_t;

/**
* Used to setup CMDQ be used of vip device
*/
typedef struct
{
    bool bEn;         ///<  enable CMDQ
    u8  u8framecnt;  ///<  assign framecount
}MDrvSclVipFcconfig_t;
/**
* Used to setup snr format of vip device
*/


typedef struct
{
    MDrvSclVipFcconfig_t stFCfg;      ///< CMDQ
    u32 u32Viraddr; ///< AIP setting
    u16 u16AIPType;
} __attribute__ ((__packed__)) MDrvSclVipAipConfig_t;
typedef struct
{
    MDrvSclVipFcconfig_t stFCfg;      ///< CMDQ
    bool bEnMCNR;
    bool bEnCIIR;
    u32 u32Viraddr; ///< MCNR setting
} __attribute__ ((__packed__)) MDrvSclVipMcnrConfig_t;



/**
* Used to setup LDC onoff of vip device
*/
typedef struct
{
    u8  bLdc_path_sel;   ///<  reg_ldc_path_sel
    u8  bEn_ldc;         ///<  reg_en_ldc(nonuse just bypass)
} __attribute__ ((__packed__)) MDrvSclVipLdcOnOffConfig_t;
/**
* Used to setup LDC mode of vip device
*/
typedef struct
{
    MDrvSclVipLdc422To444Type_e en422to444;    ///<  reg_422to444_md
    MDrvSclVipLdc444To422Type_e en444to422;    ///<  reg_444to422_md
} __attribute__ ((__packed__)) MDrvSclVipLdc422444Config_t;

/**
* Used to setup LDC config of vip device
*/
typedef struct
{
    u32   VerChk_Version ; ///< VerChk version
    MDrvSclVipFcconfig_t stFCfg;          ///< CMDQ
    MDrvSclVipLdcOnOffConfig_t stEn;      ///< be bypass
    MDrvSclVipLdc422444Config_t stmd;   ///< set mode
    MDrvSclVipLdcLcBankModeType_e enLDCType;   ///< LDC 64p or 128p
    // VerChk_Version & VerChk_Size must be the latest 2 parameter and
    // the order can't be changed
    u32   VerChk_Size; ///< VerChk Size
} __attribute__ ((__packed__)) MDrvSclVipLdcConfig_t;

/**
* Used to setup LDC mode of vip device
*/
typedef struct
{
    u32   VerChk_Version ; ///< VerChk version
    MDrvSclVipFcconfig_t stFCfg;      ///< CMDQ
    u8  u8FBidx;              ///<  reg_ldc_fb_sw_idx
    u8  u8FBrwdiff;           ///<  reg_ldc_fb_hw_rw_diff
    u8  bEnSWMode;            ///<  reg_ldc_fb_sw_mode
    MDrvSclVipLdcBypassType_e enbypass;    ///<  reg_ldc_ml_bypass
    MDrvSclVipLdcLcBankModeType_e enLDCType;///< LDC 64p or 128p
    // VerChk_Version & VerChk_Size must be the latest 2 parameter and
    // the order can't be changed
    u32   VerChk_Size; ///< VerChk Size
} __attribute__ ((__packed__)) MDrvSclVipLdcMdConfig_t;
/**
* Used to setup LDC DMAP address  of vip device
*/
typedef struct
{
    u32   VerChk_Version ; ///< VerChk version
    MDrvSclVipFcconfig_t stFCfg;      ///< CMDQ
    u32 u32DMAPaddr;          ///<  reg_ldc_dmap_st_addr
    u16 u16DMAPWidth;        ///<  reg_ldc_dmap_pitch
    u8  u8DMAPoffset;         ///<  reg_ldc_dmap_blk_xstart
    u8  bEnPowerSave;         ///<  reg_ldc_en_power_saving_mode
    // VerChk_Version & VerChk_Size must be the latest 2 parameter and
    // the order can't be changed
    u32   VerChk_Size; ///< VerChk Size
} __attribute__ ((__packed__)) MDrvSclVipLdcDmaPConfig_t;

/**
* Used to setup SRAM address of vip device
*/
typedef struct
{
    u32   VerChk_Version ; ///< VerChk version
    MDrvSclVipFcconfig_t stFCfg;      ///< CMDQ
    u32 u32loadhoraddr;       ///<  reg_ldc_load_st_addr0
    u16 u16SRAMhorstr;       ///<  reg_ldc_sram_st_addr0
    u16 u16SRAMhoramount;    ///<  reg_ldc_load_amount0
    u32 u32loadveraddr;       ///<  reg_ldc_load_st_addr1
    u16 u16SRAMverstr;       ///<  reg_ldc_sram_st_addr1
    u16 u16SRAMveramount;    ///<  reg_ldc_load_amount1
    // VerChk_Version & VerChk_Size must be the latest 2 parameter and
    // the order can't be changed
    u32   VerChk_Size; ///< VerChk Size
} __attribute__ ((__packed__)) MDrvSclVipLdcSramConfig_t;

/**
* Used to setup NLM setting of vip device
*/
typedef struct
{
    u8  bNlm_en;                                             ///<  reg_nlm_en
    MDrvSclVipNlmAverageType_e enAvgmode;                                 ///<  reg_nlm_avg_mode :0:3x3 1:5x5
    u8  bnlm_bdry_en;                                        ///<  reg_nlm_bdry_en
    u8  bnlm_post_luma_adap_en;                              ///<  reg_nlm_post_luma_adap_en
    u8  bnlm_luma_adap_en;                                   ///<  reg_nlm_luma_adap_en
    u8  bnlm_dsw_adap_en;                                    ///<  reg_nlm_dsw_adap_en
    u8  bnlmdsw_lpf_en;                                      ///<  reg_nlm_dsw_lpf_en
    u8  bnlm_region_adap_en;                                 ///<  reg_nlm_region_adap_en
    MDrvSclVipNlmDswType_e u8nlm_region_adap_size_config;                 ///<  reg_nlm_region_adap_size_config 0:16x8 1:32x16
    u8  bnlm_histIIR_en;                                     ///<  reg_nlm_histiir_adap_en
    u8  bnlm_bypass_en;                                      ///<  reg_nlm_bypass_en
    u8  u8nlm_fin_gain;                                      ///<  reg_nlm_fin_gain
    u8  u8nlm_histIIR;                                       ///<  u8nlm_histIIR
    u8  u8nlm_sad_shift;                                     ///<  reg_nlm_sad_shift
    u8  u8nlm_sad_gain;                                      ///<  reg_nlm_sad_gain
    u8  u8nlm_dsw_ratio;                                     ///<  reg_nlm_dsw_ratio
    u8  u8nlm_dsw_offset;                                    ///<  reg_nlm_dsw_offset
    u8  u8nlm_dsw_shift;                                     ///<  reg_nlm_dsw_shift
    u8  u8nlm_weight_lut[MDRV_SCLVIP_NLM_WEIGHT_NUM];                ///<  reg_nlm_weight_lut0-31 ,Qmap has adjust register squence
    u8  u8nlm_luma_adap_gain_lut[MDRV_SCLVIP_NLM_LUMAGAIN_NUM];      ///<  reg_nlm_luma_adap_gain_lut0-63,adjust register squence
    u8  u8nlm_post_luma_adap_gain_lut[MDRV_SCLVIP_NLM_POSTLUMA_NUM]; ///<  reg_nlm_post_luma_adap_gain_lut0-15,adjust register squence
    u8  u8nlm_dist_weight_7x7_lut[MDRV_SCLVIP_NLM_DISTWEIGHT_NUM];   ///<  reg_nlm_dist_weight_7x7_lut0-8,adjust register squence
    u8  u8nlm_main_snr_lut[MDRV_SCLVIP_NLM_POSTLUMA_NUM];   ///<  reg_main_snr_lut
    u8  u8nlm_wb_snr_lut[MDRV_SCLVIP_NLM_POSTLUMA_NUM];   ///<  reg_wb_snr_lut
} __attribute__ ((__packed__)) MDrvSclVipNlmMainConfig_t;
/**
* Used to setup NLM autodown load of vip device
*/
typedef struct
{
    bool bEn;         ///<  enable auto downlaod
    u32 u32Baseadr;  ///<  auto download phy addr
    u32 u32viradr;   ///<  disable auto download need virtual
} __attribute__ ((__packed__)) MDrvSclVipNlmSramConfig_t;
/**
* Used to setup NLM config of vip device
*/
typedef struct
{
    u32   VerChk_Version ; ///< VerChk version
    MDrvSclVipFcconfig_t stFCfg;          ///< CMDQ
    MDrvSclVipNlmMainConfig_t stNLM;     ///< NLM setting
    MDrvSclVipNlmSramConfig_t stSRAM;    ///< Autodownload
    // VerChk_Version & VerChk_Size must be the latest 2 parameter and
    // the order can't be changed
    u32   VerChk_Size; ///< VerChk Size
} __attribute__ ((__packed__)) MDrvSclVipNlmConfig_t;


/**
* Used to setup 422to444 of vip device
*/
typedef struct
{
    u8  bvip_422to444_en;    ///<  reg_vip_422to444_en
    u8  u8vip_422to444_md;   ///<  reg_vip_422to444_md
} __attribute__ ((__packed__)) MDrvSclVip422To444Config_t;

/**
* Used to setup bypass MACE of vip device
*/
typedef struct
{
    u8  bvip_fun_bypass_en;  ///<  reg_vip_fun_bypass_en :except DNR,SNR,NLM,LDC
} __attribute__ ((__packed__)) MDrvSclVipByPassConfig_t;

/**
* Used to setup the LB of vip device
*/
typedef struct
{
    u8  u8vps_sram_act;      ///<  reg_vps_sram_act
} __attribute__ ((__packed__)) MDrvSclVipLineBUfferonfig_t;
/**
* Used to setup mix vip  of vip device
*/
typedef struct
{
    u32   VerChk_Version ; ///< VerChk version
    MDrvSclVipFcconfig_t stFCfg;              ///< CMDQ
    MDrvSclVip422To444Config_t st422_444;      ///< 422 to 444
    MDrvSclVipByPassConfig_t stBypass;        ///< bypass
    MDrvSclVipLineBUfferonfig_t stLB;        ///<  VIP Mixed
    // VerChk_Version & VerChk_Size must be the latest 2 parameter and
    // the order can't be changed
    u32   VerChk_Size; ///< VerChk Size
} __attribute__ ((__packed__)) MDrvSclVipConfig_t;

/**
* Used to setup PK HLPF of vip device
*/
typedef struct
{
    u8  u8main_y_lpf_coef;           ///<  reg_main_y_lpf_coef
} __attribute__ ((__packed__)) MDrvSclVipHlpfConfig_t;

/**
* Used to setup PK HLPF dither of vip device
*/
typedef struct
{
    u8  hlpf_dither_en;              ///<  reg_hlpf_dither_en
} __attribute__ ((__packed__)) MDrvSclVipHlpfDitherConfig_t;

/**
* Used to setup PK VLPF of vip device
*/
typedef struct
{
    u8  main_v_lpf_coef;             ///<  reg_main_v_lpf_coef_1,2
} __attribute__ ((__packed__)) MDrvSclVipVlpfCoefConfig_t;

/**
* Used to PK VLPF dither of vip device
*/
typedef struct
{
    u8  vlpf_dither_en;              ///<  reg_vlpf_dither_en
} __attribute__ ((__packed__)) MDrvSclVipVlpfDitherConfig_t;


/**
* Used to setup PK onoff of vip device
*/
typedef struct
{
    u8  bpost_peaking_en;            ///<  reg_main_post_peaking_en
    u8  u8vps_sram_act;              ///<  reg_vps_sram_act
    u8  u8band6_dia_filter_sel;      ///<  reg_main_band6_dia_filter_sel
    u8  u8peaking_ac_yee_mode;      ///<  reg_peaking_ac_yee_mode
} __attribute__ ((__packed__)) MDrvSclVipPeakingOnOffConfig_t;

/**
* Used to setup PK band of vip device
*/
typedef struct
{
    u8  bBand_En[MDRV_SCLVIP_PEAKING_BAND_NUM];              ///<  reg_main_band1_peaking_en 1-8
    u8  u8Band_COEF_STEP[MDRV_SCLVIP_PEAKING_BAND_NUM];      ///<  reg_main_band1_coef_step 1-8
    u8  u8Band_COEF[MDRV_SCLVIP_PEAKING_BAND_NUM];           ///<  reg_main_band1_coef 1-8
    u8  u8peaking_term[MDRV_SCLVIP_PEAKING_BAND_TERM_NUM];   ///< reg_main_peaking_term1_select 1-16
    u8  u8Band_Over[MDRV_SCLVIP_PEAKING_BAND_NUM];           ///<  reg_band1_overshoot_limit 1-8
    u8  u8Band_Under[MDRV_SCLVIP_PEAKING_BAND_NUM];          ///<  reg_band1_undershoot_limit 1-8
    u8  u8Band_coring_thrd[MDRV_SCLVIP_PEAKING_BAND_NUM];    ///<  reg_main_band1_coring_thrd 1-8
    u8  u8alpha_thrd;                                ///<  reg_main_alpha_thrd
} __attribute__ ((__packed__)) MDrvSclVipPeakingBandConfig_t;

/**
* Used to setup pk adptive of vip device
*/
typedef struct
{
    u8  badaptive_en[MDRV_SCLVIP_PEAKING_BAND_NUM];                                  ///<  reg_main_band1_adaptive_en 1-8
    u8  u8hor_lut[MDRV_SCLVIP_PEAKING_BAND_TERM_NUM];                                ///<  reg_hor_lut_0-15
    u8  u8ver_lut[MDRV_SCLVIP_PEAKING_BAND_TERM_NUM];                                ///<  reg_ver_lut_0-15
    u8  u8low_diff_thrd_and_adaptive_gain_step[MDRV_SCLVIP_PEAKING_BAND_TERM_NUM];   ///<  alternation(reg_band1_adaptive_gain_step, reg_band1_low_diff_thrd) 1-8
    u8  u8dia_lut[MDRV_SCLVIP_PEAKING_BAND_TERM_NUM];                                ///<  reg_dia_lut_0-15
} __attribute__ ((__packed__)) MDrvSclVipPeakingAdptiveConfig_t;

/**
* Used to setup PK preCoring of vip device
*/
typedef struct
{
    u8  u8coring_thrd_1;     ///<  reg_main_coring_thrd_1
    u8  u8coring_thrd_2;     ///<  reg_main_coring_thrd_2
    u8  u8coring_thrd_step;  ///<  reg_main_coring_thrd_step
} __attribute__ ((__packed__)) MDrvSclVipPeakingPCoringConfig_t;

/**
* Used to setup pk ADP_Y of vip device
*/
typedef struct
{
    u8  bcoring_adp_y_en;                                    ///<  reg_main_coring_adp_y_en
    u8  bcoring_adp_y_alpha_lpf_en;                          ///<  reg_main_coring_adp_y_alpha_lpf_en
    u8  u8coring_y_low_thrd;                                 ///<  reg_main_coring_y_low_thrd
    u8  u8coring_adp_y_step;                                 ///<  reg_main_coring_adp_y_step
    u8  u8coring_adp_y_alpha_lut[MDRV_SCLVIP_PEAKING_ADP_Y_LUT_NUM]; ///<  reg_main_coring_adp_y_alpha_lut_0-7
} __attribute__ ((__packed__)) MDrvSclVipPeakingAdpYConfig_t;

/**
* Used to setup PK gain of vip device
*/
typedef struct
{
    u8  u8osd_sharpness_ctrl ;       ///<  reg_main_osd_sharpness_ctrl
    u8  bosd_sharpness_sep_hv_en;    ///<  reg_main_osd_sharpness_sep_hv_en
    u8  u8osd_sharpness_ctrl_h  ;    ///<  reg_main_osd_sharpness_ctrl_h
    u8  u8osd_sharpness_ctrl_v ;     ///<  reg_main_osd_sharpness_ctrl_v
} __attribute__ ((__packed__)) MDrvSclVipPeakingGainConfig_t;

/**
* Used to setup PK adpy gain of vip device
*/
typedef struct
{
    u8  bpk_adp_y_en;                                    ///<  reg_main_coring_adp_y_en
    u8  bpk_adp_y_alpha_lpf_en;                          ///<  reg_main_coring_adp_y_alpha_lpf_en
    u8  u8pk_y_low_thrd;                                 ///<  reg_main_coring_y_low_thrd
    u8  u8pk_adp_y_step;                                 ///<  reg_main_coring_adp_y_step
    u8  u8pk_adp_y_alpha_lut[MDRV_SCLVIP_PEAKING_ADP_Y_LUT_NUM]; ///<  reg_main_coring_adp_y_alpha_lut_0-7
} __attribute__ ((__packed__)) MDrvSclVipPeakingGainApdYConfig_t;

/**
* Used to setup pk yc gain of vip device
*/
typedef struct
{
    u16 u16Dlc_in_y_gain;   ///<  reg_dlc_in_y_gain 16bit
    u16 u16Dlc_in_y_offset; ///<  reg_dlc_in_y_offset 16 bit
    u16 u16Dlc_in_c_gain;   ///<  reg_dlc_in_c_gain 16 bit
    u16 u16Dlc_in_c_offset; ///<  reg_dlc_in_c_offset 16 bit
    u16 u16Dlc_out_y_gain;  ///<  reg_dlc_out_y_gain 16 bit
    u16 u16Dlc_out_y_offset;///<  reg_dlc_out_y_offset 16 bit
    u16 u16Dlc_out_c_gain;  ///<  reg_dlc_out_c_gain 16 bit
    u16 u16Dlc_out_c_offset;///<  reg_dlc_out_c_offset 16 bit
} __attribute__ ((__packed__)) MDrvSclVipYCGainOffsetConfig_t;

/**
* Used to setup pk config of vip device
*/
typedef struct
{
    u32   VerChk_Version ; ///< VerChk version
    MDrvSclVipFcconfig_t stFCfg;                      ///< CMDQ
    MDrvSclVipHlpfConfig_t stHLPF;                    ///< HLPF
    MDrvSclVipHlpfDitherConfig_t stHLPFDith;         ///< HDither
    MDrvSclVipVlpfCoefConfig_t stVLPFcoef1;          ///< VLPF coef1
    MDrvSclVipVlpfCoefConfig_t stVLPFcoef2;          ///< VLPF coef2
    MDrvSclVipVlpfDitherConfig_t stVLPFDith;         ///< VDither
    MDrvSclVipPeakingOnOffConfig_t stOnOff;           ///< pkonoff
    MDrvSclVipPeakingBandConfig_t stBand;            ///< pkband
    MDrvSclVipPeakingAdptiveConfig_t stAdp;          ///< pk adp
    MDrvSclVipPeakingPCoringConfig_t stPcor;         ///< pk precore
    MDrvSclVipPeakingAdpYConfig_t stAdpY;           ///< pk adp y
    MDrvSclVipPeakingGainConfig_t stGain;            ///< pk gain
    MDrvSclVipPeakingGainApdYConfig_t stGainAdpY;  ///< pk Y gain
    // VerChk_Version & VerChk_Size must be the latest 2 parameter and
    // the order can't be changed
    u32   VerChk_Size; ///< VerChk Size
} __attribute__ ((__packed__)) MDrvSclVipPeakingConfig_t;


/**
* Used to setup LCE of vip device
*/
typedef struct
{
    bool bLCE_En;  ///< LCE en
    u8  u8ControlNum;  ///<  vip control guard pipe number
} __attribute__ ((__packed__)) MDrvSclVipLceOnOffConfig_t;


/**
* Used to setup LCE dither of vip device
*/
typedef struct
{
    bool bLCE_Dither_En;   ///< LCE dither
} __attribute__ ((__packed__)) MDrvSclVipLceDitherConfig_t;

/**
* Used to setup LCE config of vip device
*/
typedef struct
{
    u8  bLCE_sodc_alpha_en;          ///<  reg_main_lce_sodc_alpha_en
    u8  bLce_dering_alpha_en;        ///<  reg_main_lce_dering_alpha_en
    MDrvSclVipLceYAveSelType_e enLce_y_ave_sel; ///<  reg_main_lce_y_ave_sel (1'b1: 5x7; 1'b0:5x11)
    u8  bLce_3curve_en;              ///<  reg_lce_3curve_en
    u8  u8Lce_std_slop1;             ///<  reg_main_lce_std_slop1
    u8  u8Lce_std_slop2;             ///<  reg_main_lce_std_slop2
    u8  u8Lce_std_th1;               ///<  reg_main_lce_std_th1
    u8  u8Lce_std_th2;               ///<  reg_main_lce_std_th2
    u8  u8Lce_gain_min;              ///<  reg_main_lce_gain_min
    u8  u8Lce_gain_max;              ///<  reg_main_lce_gain_max
    u8  u8Lce_sodc_low_alpha;        ///<  reg_main_lce_sodc_low_alpha
    u8  u8Lce_sodc_low_th;           ///<  reg_main_lce_sodc_low_th
    u8  u8Lce_sodc_slop;             ///<  reg_main_lce_sodc_slop
    u8  u8Lce_diff_gain;             ///<  reg_main_lce_diff_gain
    u8  u8Lce_gain_complex;          ///<  reg_main_lce_gain_complex
    u8  u8Lce_dsw_minsadgain;        ///<  reg_dsptch_lce_dsw_minsadgain
    u8  u8Lce_dsw_gain;              ///<  reg_dsptch_lce_dsw_gian
    u8  u8LCE_dsw_thrd;              ///<  reg_dsptch_lce_dsw_thrd
} __attribute__ ((__packed__)) MDrvSclVipLceSettingConfig_t;

/**
* Used to setup LCE curve of vip device
*/
typedef struct
{
    u8  u8Curve_Thread[4];                       ///<  reg_main_lce_curve_a-d
    u16 u16Curve1[MDRV_SCLVIP_LCE_CURVE_SECTION_NUM];   ///<  reg_lce_curve_lut1_08-f8 (lsb|msb) 16bit
    u16 u16Curve2[MDRV_SCLVIP_LCE_CURVE_SECTION_NUM];   ///<  reg_lce_curve_lut2_08-f8 (lsb|msb) 16bit
    u16 u16Curve3[MDRV_SCLVIP_LCE_CURVE_SECTION_NUM];   ///<  reg_lce_curve_lut3_08-f8 (lsb|msb) 16bit
} __attribute__ ((__packed__)) MDrvSclVipLceCruveConfig_t;

/**
* Used to setup LCE of vip device
*/
typedef struct
{
    u32   VerChk_Version ; ///< VerChk version
    MDrvSclVipFcconfig_t stFCfg;              ///< CMDQ
    MDrvSclVipLceOnOffConfig_t stOnOff;      ///< bEn
    MDrvSclVipLceDitherConfig_t stDITHER;    ///< dither
    MDrvSclVipLceSettingConfig_t stSet;      ///< config
    MDrvSclVipLceCruveConfig_t stCurve;      ///< curve
    // VerChk_Version & VerChk_Size must be the latest 2 parameter and
    // the order can't be changed
    u32   VerChk_Size; ///< VerChk Size
} __attribute__ ((__packed__)) MDrvSclVipLceConfig_t;

/**
* Used to setup DLC prog of vip device
*/
typedef struct
{
    u8    u8VARCP[MDRV_SCLVIP_DLC_LUMA_SECTION_NUM]; ///<  reg_curve_fit_var_cp1 -64
    u8    u8Tbln0;                           ///<  reg_main_curve_table_n0
    u8    u8Tbln0LSB;                        ///<  reg_main_curve_table_n0_LSB
    u8    u8Tbln0sign;                       ///<  reg_main_curve_table_n0_sign
    u16  u16Tbl64;                          ///<  reg_main_curve_table64 16 bit
    u8    u8Tbl64LSB;                        ///<  reg_main_curve_table64_LSB
} __attribute__ ((__packed__)) MDrvSclVipDlcProgConfig_t;

//MDRV_VIP_SET_DLC_CURVE_CONFIG
/**
* Used to setup DLC curve config of vip device
*/
typedef struct
{
    u8    u8InLuma[MDRV_SCLVIP_DLC_LUMA_SECTION_NUM];    ///< reg_main_curve_table0-63
    u8    u8InLumaLSB[MDRV_SCLVIP_DLC_LUMA_SECTION_NUM]; ///<  reg_main_curve_table0-63 LSB
    MDrvSclVipDlcProgConfig_t ProgCfg;                   ///< dlc proc
} __attribute__ ((__packed__)) MDrvSclVipDlcCurveConfig_t;

/**
* Used to DLC enable of vip device
*/
typedef struct
{
    u8   bcurve_fit_var_pw_en;   ///<  reg_main_curve_fit_var_pw_en
    MDrvSclVipDlcCurveConfig_t stCurve;  ///< curve config
    u8   bcurve_fit_en;          ///<  reg_main_curve_fit_en
    u8   bstatistic_en;          ///<  reg_main_statistic_en
} __attribute__ ((__packed__)) MDrvSclVipDlcEnableConfig_t;

/**
* Used to setup dic dither of vip device
*/
typedef struct
{
    u8   bDLCdither_en;   ///< bdlc dither
} __attribute__ ((__packed__)) MDrvSclVipDlcDitherConfig_t;

/**
* Used to setup DLC range of vip device
*/
typedef struct
{
    u8  u8brange_en;  ///< brange
} __attribute__ ((__packed__)) MDrvSclVipDlcHistogramEnConfig_t;

/**
* Used to setup DLC H of vip device
*/
typedef struct
{
    u16 u16statistic_h_start;   ///< reg_main_statistic_h_start 16 bit
    u16 u16statistic_h_end;     ///< reg_main_statistic_h_end  16 bit
} __attribute__ ((__packed__)) MDrvSclVipDlcHistogramHConfig_t;

/**
* Used to setup  DLC V of vip device
*/
typedef struct
{
    u16 u16statistic_v_start;   ///< reg_main_statistic_v_start 16 bit
    u16 u16statistic_v_end;     ///< reg_main_statistic_v_end  16 bit
} __attribute__ ((__packed__)) MDrvSclVipDlcHistogramVConfig_t;

/**
* Used to setup DLC PC of vip device
*/
typedef struct
{
    u8  bhis_y_rgb_mode_en;      ///<  reg_his_y_rgb_mode_en
    u8  bcurve_fit_rgb_en;       ///<  reg_main_curve_fit_rgb_en
} __attribute__ ((__packed__)) MDrvSclVipDlcPcConfig_t;
/**
* Used to setup DLC of vip device
*/
typedef struct
{
    u32   VerChk_Version ; ///< VerChk version
    MDrvSclVipFcconfig_t stFCfg;                      ///< CMDQ
    MDrvSclVipYCGainOffsetConfig_t stGainOffset;    ///< Gain
    MDrvSclVipDlcEnableConfig_t stEn;                ///< EN
    MDrvSclVipDlcDitherConfig_t stDither;            ///< Dither
    MDrvSclVipDlcHistogramEnConfig_t sthist;        ///< hist
    MDrvSclVipDlcHistogramHConfig_t stHistH;        ///<hhsit
    MDrvSclVipDlcHistogramVConfig_t stHistV;        ///<vhist
    MDrvSclVipDlcPcConfig_t stPC;                    ///< PC
    // VerChk_Version & VerChk_Size must be the latest 2 parameter and
    // the order can't be changed
    u32   VerChk_Size; ///< VerChk Size
} __attribute__ ((__packed__)) MDrvSclVipDlcConfig_t;
/**
* Used to setup HIST of vip device
*/
typedef struct
{
    u32   VerChk_Version ; ///< VerChk version
    MDrvSclVipFcconfig_t stFCfg;
    u8  bVariable_Section;                               ///< 1E04 reg_variable_range_en
    u8  bstatic;                                         ///< 1E04 reg_main_statistic_en
    u8  bcurve_fit_en;                                   ///< 1E04 reg_main_curve_fit_en
    u8  bhis_y_rgb_mode_en;                              ///< 1E04 reg_his_y_rgb_mode_en
    u8  bcurve_fit_rgb_en;                               ///< 1E04 reg_main_curve_fit_rgb_en
    u8  bDLCdither_en;                                   ///< 1E04 reg_ycv_dither_en
    u8  bstat_MIU;                                       ///<  reg_vip_stat_miu_en
    u8  bRange;                                          ///<  reg_main_range_en
    u16 u16Vst;                                         ///<  reg_main_statistic_v_start
    u16 u16Vnd;                                         ///<  reg_main_statistic_v_end
    u16 u16Hst;                                         ///<  reg_main_statistic_h_start
    u16 u16Hnd;                                         ///<  reg_main_statistic_v_end
    u8  u8HistSft;                                       ///<  reg_histrpt_sft
    u8  u8trig_ref_mode;                                 ///<  reg_vip_stat_trig_ref_md
    u32 u32StatBase[2];                                  ///<  reg_vip_stat_base0,reg_vip_stat_base1
    u8  u8Histogram_Range[MDRV_SCLVIP_DLC_HISTOGRAM_SECTION_NUM];///<  reg_histogram_range1-7
    // VerChk_Version & VerChk_Size must be the latest 2 parameter and
    // the order can't be changed
    u32   VerChk_Size; ///< VerChk Size
} __attribute__ ((__packed__)) MDrvSclVipDlcHistogramConfig_t;

//MDRV_VIP_GET_DLC_HISTOGRAM_REPORT
/**
* Used to setup hist report of vip device
*/
typedef struct
{
    u32 u32Histogram[MDRV_SCLVIP_DLC_HISTOGRAM_REPORT_NUM];  ///<  reg_total_1f-ff_00
    u32 u32PixelWeight;                              ///<  reg_main_total_pixel_weight
    u32 u32PixelCount;                               ///<  reg_main_total_pixel_count
    u8  u8MinPixel;                                  ///<  reg_main_min_pixel
    u8  u8MaxPixel;                                  ///<  reg_main_max_pixel
    u8  u8Baseidx;                                   ///<  reg_vip_stat_baseidx
} __attribute__ ((__packed__)) MDrvSclVipDlcHistogramReport_t;

/**
* Used to setup the UVC of vip device
*/
typedef struct
{
    u8  buvc_en;                         ///<  reg_main_uvc_en
    u8  u8uvc_locate;                    ///<  reg_main_uvc_locate
    u8  u8uvc_gain_high_limit_lsb;       ///<  reg_main_uvc_gain_high_limit_lsb
    u8  u8uvc_gain_high_limit_msb;       ///<  reg_main_uvc_gain_high_limit_msb
    u8  u8uvc_gain_low_limit_lsb;        ///<  reg_main_uvc_gain_low_limit_lsb
    u8  u8uvc_gain_low_limit_msb;        ///<  reg_main_uvc_gain_low_limit_msb
    u8  buvc_adaptive_luma_en;           ///<  reg_main_uvc_adaptive_luma_en
    u8  u8uvc_adaptive_luma_y_input;     ///<  reg_main_uvc_adaptive_luma_y_input
    u8  u8uvc_adaptive_luma_black_step;  ///<  reg_main_uvc_adaptive_luma_black_step
    u8  u8uvc_adaptive_luma_white_step;  ///<  reg_main_uvc_adaptive_luma_white_step
    u8  u8uvc_adaptive_luma_black_th;    ///<  reg_main_uvc_adaptive_luma_black_thrd
    u8  u8uvc_adaptive_luma_white_th;    ///<  reg_main_uvc_adaptive_luma_white_thrd
    u8  u8uvc_adaptive_luma_gain_low;    ///<  reg_main_uvc_adaptive_luma_gain_low
    u8  u8uvc_adaptive_luma_gain_med;    ///<  reg_main_uvc_adaptive_luma_gain_med
    u8  u8uvc_adaptive_luma_gain_high;   ///<  reg_main_uvc_adaptive_luma_gain_high
    u8  buvc_rgb_en;                     ///<  reg_main_uvc_rgb_en
    u8  buvc_dlc_fullrange_en;           ///<  reg_main_uvc_dlc_fullrange_en
    u8  u8uvc_low_sat_prot_thrd;         ///<  reg_main_uvc_low_sat_prot_thrd
    u8  u8uvc_low_sat_min_strength;      ///<  reg_main_uvc_low_sat_min_strength
    u8  buvc_low_y_sat_prot_en;          ///<  reg_main_uvc_low_y_sat_prot_en
    u8  buvc_lpf_en;                     ///<  reg_main_uvc_lpf_en
    u8  buvc_low_sat_prot_en;            ///<  reg_main_uvc_low_sat_prot_en
    u8  buvc_low_y_prot_en;              ///<  reg_main_uvc_low_y_prot_en
    u8  u8uvc_low_sat_prot_slope;        ///<  reg_main_uvc_low_sat_prot_slope
    u8  u8uvc_low_y_prot_slope;          ///<  reg_main_uvc_low_y_prot_slope
    MDrvSclVipUvcAdpYInputSelType_e enAdp_Ysel;    ///<  reg_main_uvc_low_y_sel
    u8  u8uvc_low_y_prot_thrd;           ///<  reg_main_uvc_low_y_prot_thrd
    u8  u8uvc_low_y_min_strength;        ///<  reg_main_uvc_low_y_min_strength
} __attribute__ ((__packed__)) MDrvSclVipUvcMainConfig_t;
/**
* Used to setup UVC format of vip device
*/
typedef struct
{
    u32   VerChk_Version ; ///< VerChk version
    MDrvSclVipFcconfig_t stFCfg;      ///< CMDQ
    MDrvSclVipUvcMainConfig_t stUVC; ///< UVC
    // VerChk_Version & VerChk_Size must be the latest 2 parameter and
    // the order can't be changed
    u32   VerChk_Size; ///< VerChk Size
} __attribute__ ((__packed__)) MDrvSclVipUvcConfig_t;
/**
* Used to setup FCC adp of vip device
*/
typedef struct
{
    u8  u8fcc_adp_Y_LUT[MDRV_SCLVIP_FCC_YWIN_LUT_ENTRY_NUM];  ///< ///<  reg_main_fcc_adp_Y_LUT_win0_0-16
} __attribute__ ((__packed__)) MDrvSclVipFccAdpYLutConfig_t;

/**
* Used to setup fcc full range of vip device
*/
typedef struct
{
    u8  bfcc_fr_en;                      ///<  reg_main_fcc_fr_en
    u8  bEn[4];                          ///<  reg_main_fcc_adp_Y_win0_en 0-3
    u8  u8YWinNum[4];                    ///<  reg_main_fcc_adp_Y_win0_NUM 0-3
    MDrvSclVipFccAdpYLutConfig_t u8Ywin_LUT[4];///<  reg_main_fcc_adp_Y_LUT_win0-3_0
} __attribute__ ((__packed__)) MDrvSclVipFccFullRangeConfig_t;

/**
* Used to setup FCC Tx of vip device
*/
typedef struct
{
    u8  bEn;                                 ///<  reg_main_fcc_t_en
    u8  u8Cb;                                ///<  reg_fcc_cb_t
    u8  u8Cr;                                ///<  reg_fcc_cr_t
    u8  u8K;                                 ///<  reg_fcc_k_t
    u8  u8Range[E_MDRV_SCLVIP_FCC_Y_DIS_NUM];        ///<  reg_fcc_win_cr_down~reg_fcc_win_cb_up
    u8  u8LSB_Cb;                            ///<  reg_fcc_fr_cb_t_lsb  for FR reg_fcc_cb_t lsb
    u8  u8LSB_Cr;                            ///<  reg_fcc_fr_cr_t_lsb  for FR reg_fcc_cr_t lab
    u16 u16FullRange[E_MDRV_SCLVIP_FCC_Y_DIS_NUM];  ///<  reg_fcc_fr_win_cr_down ~reg_fcc_fr_win_cb_up full range 16 bit
} __attribute__ ((__packed__)) MDrvSclVipFccTConfig_t;

/**
* Used to setup FCC T9 of vip device
*/
typedef struct
{
    u8  bEn;                                 ///<  reg_main_fcc_9t_en
    u8  u8K;                                 ///<  reg_fcc_k_9t
    u8  u8Cr;                                ///<  reg_fcc_win9_cr
    u8  u8Cb;                                ///<  reg_fcc_win9_cb
    u8  bfirstEn;                            ///<  reg_main_fcc_9t_first_en
    u8  u8frCb;                              ///<  reg_fcc_fr_cb_t9
    u8  u8LSB_Cb;                            ///<  reg_fcc_fr_cb_t9_lsb
    u8  u8frCr;                              ///<  reg_fcc_fr_cr_t9
    u8  u8LSB_Cr;                            ///<  reg_fcc_fr_cr_t9_lsb
    u16 u16FullRange[E_MDRV_SCLVIP_FCC_Y_DIS_NUM];  ///<  reg_fcc_fr_win9_cr_down ~reg_fcc_fr_win9_cb_up full range 16 bit
} __attribute__ ((__packed__)) MDrvSclVipFccT9Config_t;

/**
* Used to setup FCC of vip device
*/
typedef struct
{
    u32   VerChk_Version ; ///< VerChk version
    MDrvSclVipFcconfig_t stFCfg;          ///< CMDQ
    MDrvSclVipFccFullRangeConfig_t stfr; ///< full range
    MDrvSclVipFccTConfig_t stT[8];       ///< STx
    MDrvSclVipFccT9Config_t stT9;        ///< ST9
    // VerChk_Version & VerChk_Size must be the latest 2 parameter and
    // the order can't be changed
    u32   VerChk_Size; ///< VerChk Size
} __attribute__ ((__packed__)) MDrvSclVipFccConfig_t;

/**
* Used to setup IHC on of vip device
*/
typedef struct
{
    u8  bIHC_en;    ///<  reg_main_ihc_en
} __attribute__ ((__packed__)) MDrvSclVipIhcOnOffConfig_t;

/**
* Used to setup IHC Ymode of vip device
*/
typedef struct
{
    u8  bIHC_y_mode_en;                  ///<  reg_main_ihc_y_mode_en
    u8  bIHC_y_mode_diff_color_en;       ///<  reg_main_ihc_y_mode_diff_color_en
} __attribute__ ((__packed__)) MDrvSclVipIhcYmodeConfig_t;

/**
* Used to setup IHC dither of vip device
*/
typedef struct
{
    u8  bIHC_dither_en;    ///<  reg_ihc_dither_en
} __attribute__ ((__packed__)) MDrvSclVipIhcDitherConfig_t;

/**
* Used to setup IHC user of vip device
*/
typedef struct
{
    u8  u8hue_user_color;        ///<  reg_main_hue_user_color0
    u8  u8hue_user_color_sec0;   ///<  reg_main_hue_user_color0_0
    u8  u8hue_user_color_sec1;   ///<  reg_main_hue_user_color0_1
    u8  u8hue_user_color_sec2;   ///<  reg_main_hue_user_color0_2
} __attribute__ ((__packed__)) MDrvSclVipIhcUserConfig_t;

/**
* Used to setup IHC setting of vip device
*/
typedef struct
{
    MDrvSclVipIhcUserConfig_t stIHC_color[MDRV_SCLVIP_IHC_USER_COLOR_NUM];///<  0-15
} __attribute__ ((__packed__)) MDrvSclVipIhcSettingConfig_t;

/**
* Used to setup IHC of vip device
*/
typedef struct
{
    u32   VerChk_Version ; ///< VerChk version
    MDrvSclVipFcconfig_t stFCfg;          ///<CMDQ
    MDrvSclVipIhcOnOffConfig_t stOnOff;  ///<onoff
    MDrvSclVipIhcYmodeConfig_t stYmd;    ///<Ymode
    MDrvSclVipIhcDitherConfig_t stDither;///<dither
    MDrvSclVipIhcSettingConfig_t stset;  ///<set
    // VerChk_Version & VerChk_Size must be the latest 2 parameter and
    // the order can't be changed
    u32   VerChk_Size; ///< VerChk Size
} __attribute__ ((__packed__)) MDrvSclVipIhcConfig_t;

/**
* Used to setup ICC of vip device
*/
typedef struct
{
    u8  bICC_en;             ///<  reg_main_icc_en
    u8  bcbcr_to_uv_en;      ///<  reg_main_cbcr_to_uv
    u8  u8common_minus_gain; ///<  reg_common_minus_gain
    u8  u8sa_min;            ///<  reg_sa_min
    u16 u16step_sa_user;    ///<  reg_main_step_sa_user 16 bit
} __attribute__ ((__packed__)) MDrvSclVipIccEnableConfig_t;

/**
* Used to setup ICC Ymode of vip device
*/
typedef struct
{
    u8  u8sa_user_color_sec0;    ///<  control en
    u8  u8sign_sa_user_color_sec0; ///< [6:0] User adjust hue, color 0~9
    u8  u8sa_user_color_sec1;    ///<  control en
    u8  u8sign_sa_user_color_sec1; ///< [6:0] User adjust hue, color 0~9
    u8  u8sa_user_color_sec2;    ///<  control en
    u8  u8sign_sa_user_color_sec2; ///< [6:0] User adjust hue, color 0~9
} __attribute__ ((__packed__)) MDrvSclVipIccYmodeinConfig_t;

/**
* Used to setup ICC Ymode of vip device
*/
typedef struct
{
    u8  icc_y_mode_en;                                   ///<  reg_main_icc_y_mode_en
    u8  icc_y_mode_diff_color_en;                        ///<  reg_main_icc_y_mode_diff_color_en
    MDrvSclVipIccYmodeinConfig_t stICC_color[MDRV_SCLVIP_ICE_COLOR_NUM];///<  MDrvSclVipIccYmodeinConfig_t
} __attribute__ ((__packed__)) MDrvSclVipIccYmodeConfig_t;

/**
* Used to setup icc dither of vip device
*/
typedef struct
{
    u8  bICC_dither_en;    ///<  reg_icc_dither_en
} __attribute__ ((__packed__)) MDrvSclVipIccDitherConfig_t;
/**
* Used to setup ICC user of vip device
*/
typedef struct
{
    u8  u8sa_user_color;     ///<  reg_main_sa_user_colo
    u8  u8sign_sa_user_color;///< reg_main_sign_sa_user_color
} __attribute__ ((__packed__)) MDrvSclVipIccUserConfig_t;

/**
* Used to setup ICC of vip device
*/
typedef struct
{
    MDrvSclVipIccUserConfig_t stICC_color[MDRV_SCLVIP_ICE_COLOR_NUM];   ///< 0-15
} __attribute__ ((__packed__)) MDrvSclVipIccSettingConfig_t;

/**
* Used to setup ICC of vip device
*/
typedef struct
{
    u32   VerChk_Version ; ///< VerChk version
    MDrvSclVipFcconfig_t stFCfg;              ///<CMDQ
    MDrvSclVipIccEnableConfig_t stEn;        ///<En
    MDrvSclVipIccYmodeConfig_t stYmd;        ///<Ymode
    MDrvSclVipIccDitherConfig_t stDither;    ///<dither
    MDrvSclVipIccSettingConfig_t stSet;      ///<set
    // VerChk_Version & VerChk_Size must be the latest 2 parameter and
    // the order can't be changed
    u32   VerChk_Size; ///< VerChk Size
} __attribute__ ((__packed__)) MDrvSclVipIccConfig_t;

/**
* Used to setup ihcicc of vip device
*/
typedef struct
{
    u8  u8ihc_icc_y[E_MDRV_SCLVIP_IHC_ICE_ADP_Y_SECTION_NUM];               ///< reg_main_ihc_icc_y_0-3
} __attribute__ ((__packed__)) MDrvSclVipYmodeYvalueAllConfig_t;

/**
* Used to setup ihcicc of vip device
*/
typedef struct
{
    u8  u8ihc_icc_color_sec0; ///<sec0
    u8  u8ihc_icc_color_sec1; ///<sec1
    u8  u8ihc_icc_color_sec2; ///<sec2
    u8  u8ihc_icc_color_sec3; ///<sec3
} __attribute__ ((__packed__)) MDrvSclVipYvalueUserConfig_t;

/**
* Used to setup ICCIHC of vip device
*/
typedef struct
{
    MDrvSclVipYvalueUserConfig_t stYmode_Yvalue_color[MDRV_SCLVIP_ICE_COLOR_NUM];///< 0-15
} __attribute__ ((__packed__)) MDrvSclVipYmodeYvalueSettingConfig_t;

/**
* Used to setup ICCIHC of vip device
*/
typedef struct
{
    u32   VerChk_Version ; ///< VerChk version
    MDrvSclVipFcconfig_t stFCfg;                      ///<CMDQ
    MDrvSclVipYmodeYvalueAllConfig_t stYmdall;      ///<Ymode
    MDrvSclVipYmodeYvalueSettingConfig_t stYmdset;  ///<Ymodeset
    // VerChk_Version & VerChk_Size must be the latest 2 parameter and
    // the order can't be changed
    u32   VerChk_Size; ///< VerChk Size
} __attribute__ ((__packed__)) MDrvSclVipIhcIccConfig_t;

/**
* Used to setup IBC of vip device
*/
typedef struct
{
    u8  bIBC_en;             ///<  reg_main_ibc_en
    u8  u8IBC_coring_thrd;   ///<  reg_ibc_coring_thrd
    u8  bIBC_y_adjust_lpf_en;///<  reg_ibc_y_adjust_lpf_en
} __attribute__ ((__packed__)) MDrvSclVipIbcEnableConfig_t;

/**
* Used to setup IBC dither of vip device
*/
typedef struct
{
    u8  bIBC_dither_en;    ///<  reg_ibc_dither_en
} __attribute__ ((__packed__)) MDrvSclVipIbcDitherConfig_t;

/**
* Used to setup IBC setting of vip device
*/
typedef struct
{
    u8  u8ycolor_adj[MDRV_SCLVIP_IBC_COLOR_NUM];  ///<  reg_main_ycolor0_adj 0-15
    u8  u8weightcminlimit;                ///< reg_weight_c_min_limit
    u8  u8weightcmaxlimit;                ///< reg_weight_y_min_limit
} __attribute__ ((__packed__)) MDrvSclVipIbcSettingConfig_t;

/**
* Used to setup IBC of vip device
*/
typedef struct
{
    u32   VerChk_Version ; ///< VerChk version
    MDrvSclVipFcconfig_t stFCfg;              ///< CMDQ
    MDrvSclVipIbcEnableConfig_t stEn;        ///< bEn
    MDrvSclVipIbcDitherConfig_t stDither;    ///< dither
    MDrvSclVipIbcSettingConfig_t stSet;      ///< setting
    // VerChk_Version & VerChk_Size must be the latest 2 parameter and
    // the order can't be changed
    u32   VerChk_Size; ///< VerChk Size
} __attribute__ ((__packed__)) MDrvSclVipIbcConfig_t;

/**
* Used to setup ack of vip device
*/
typedef struct
{
    u8  backen;              ///<  reg_main_ack_en
    u8  bYswitch_dithen;     ///<  reg_y_switch_dither_en
    u8  bYswitchen;          ///<  reg_main_y_switch_en
    u8  u8Uswitch;           ///<  reg_u_switch_coef
    u8  u8Vswitch;           ///<  reg_v_switch_coef
    u8  u8Ythrd;             ///<  reg_ack_y_thrd
    u8  u8offset;            ///<  reg_ack_offset
    u8  u8yslot;             ///<  reg_ack_y_slop
    u8  u8limit;             ///<  reg_ack_limit
    u8  bCcompen;            ///<  reg_ack_c_comp_en
    u8  u8Cthrd;             ///<  reg_ack_c_thrd
    u8  u8Crange;            ///<  reg_ack_c_range
} __attribute__ ((__packed__)) MDrvSclVipAckMainConfig_t;
/**
* Used to setup clamp of vip device
*/
typedef struct
{
    u8  bclamp_en;       ///<  reg_vip_main_clamp_en
    u16 u16y_max_clamp; ///<  reg_main_y_max_clamp 16bit
    u16 u16y_min_clamp; ///<  reg_main_y_min_clamp 16bit
    u16 u16cb_max_clamp;///<  reg_main_cb_max_clamp 16bit
    u16 u16cb_min_clamp;///<  reg_main_cr_min_clamp 16bit
    u16 u16cr_max_clamp;///<  reg_main_cb_max_clamp 16bit
    u16 u16cr_min_clamp;///<  reg_main_cr_min_clamp 16bit
} __attribute__ ((__packed__)) MDrvSclVipYcbcrClipMainConfig_t;

/**
* Used to setup ack config of vip device
*/
typedef struct
{
    u32   VerChk_Version ; ///< VerChk version
    MDrvSclVipFcconfig_t stFCfg;              ///<CMDQ
    MDrvSclVipAckMainConfig_t stACK;         ///<ACK
    MDrvSclVipYcbcrClipMainConfig_t stclip; ///<clamp
    // VerChk_Version & VerChk_Size must be the latest 2 parameter and
    // the order can't be changed
    u32   VerChk_Size; ///< VerChk Size
} __attribute__ ((__packed__)) MDrvSclVipAckConfig_t;


/**
* Used to setup the CMDQ of vip device
*/
typedef struct
{
    MDrvSclVipFcconfig_t stFCfg;  ///< CMDQ
    u32 u32Addr;          ///< address
    u16 u16Data;         ///< cmd
    u16 u16Mask;         ///< mask
    u8  u8framecnt;       ///< count
    bool bfire;            ///< fire
    bool bCnt;             ///< bframecount
} __attribute__ ((__packed__)) MDrvSclVipCmdqConfig_t;

/**
* Used to setup the susupend of vip device
*/
typedef struct
{
    u32 bresetflag;                   ///< flag
    u32 bAIPreflag;                   ///< flag
    MDrvSclVipAckConfig_t stack;              ///< ack
    MDrvSclVipIbcConfig_t stibc;              ///< ibc
    MDrvSclVipIhcIccConfig_t stihcicc;        ///< iccihc
    MDrvSclVipIccConfig_t sticc;              ///< icc
    MDrvSclVipIhcConfig_t stihc;              ///< ihc
    MDrvSclVipFccConfig_t stfcc;              ///< fcc
    MDrvSclVipUvcConfig_t stuvc;              ///< uvc
    MDrvSclVipDlcHistogramConfig_t sthist;   ///< hist
    MDrvSclVipDlcConfig_t stdlc;              ///< dlc
    MDrvSclVipLceConfig_t stlce;              ///< lce
    MDrvSclVipPeakingConfig_t stpk;           ///< pk
    MDrvSclVipNlmConfig_t stnlm;              ///< nlm
    MDrvSclVipLdcMdConfig_t stldcmd;         ///< ldc
    MDrvSclVipLdcDmaPConfig_t stldcdmap;     ///< ldc
    MDrvSclVipLdcSramConfig_t stldcsram;     ///< ldc
    MDrvSclVipLdcConfig_t stldc;              ///< ldc
    MDrvSclVipMcnrConfig_t stmcnr;            ///<Mcnr
    MDrvSclVipConfig_t stvip;                  ///< vipmix
    MDrvSclVipAipConfig_t staip[E_MDRV_SCLVIP_AIP_NUM]; ///<AIP
} __attribute__ ((__packed__)) MDrvSclVipSuspendConfig_t;
/**
* Used to setup the susupend of vip device
*/
typedef struct
{
    u32   VerChk_Version ; ///< VerChk version
    MDrvSclVipSuspendConfig_t stvipCfg;
    u32   VerChk_Size; ///< VerChk Size
}__attribute__ ((__packed__)) MDrvSclVipAllSEtConfig_t;

/**
* Used to setup vtrack of vip device
*/
typedef struct
{
    u32   VerChk_Version ; ///< VerChk version
    MDrvSclVipVtrackEnableType_e EnType; ///< en
    u8  u8framerate;              ///< rate
    // VerChk_Version & VerChk_Size must be the latest 2 parameter and
    // the order can't be changed
    u32   VerChk_Size; ///< VerChk Size
}__attribute__ ((__packed__)) MDrvSclVipVtrackOnOffConfig_t;
/**
* Used to vtrack config of vip device
*/
typedef struct
{
    u32   VerChk_Version ; ///< VerChk version
    bool bSetKey;                                  ///< key
    bool bSetUserDef;                              ///< user def
    u8  u8SetKey[MDRV_SCLVIP_VTRACK_KEY_SETTING_LENGTH];  ///< key
    u8  u8SetUserDef[MDRV_SCLVIP_VTRACK_SETTING_LENGTH];  ///< userdef
    u8  u8OperatorID;                             ///< operator
    u16 u16Timecode;                             ///< timecode
    // VerChk_Version & VerChk_Size must be the latest 2 parameter and
    // the order can't be changed
    u32   VerChk_Size; ///< VerChk Size
}__attribute__ ((__packed__)) MDrvSclVipVtrackConfig_t;
typedef enum
{
    PQ_IP_LDC_Size = sizeof(MDrvSclVipLdcOnOffConfig_t),  //4
    PQ_IP_LDC_422_444_422_Size = sizeof(MDrvSclVipLdc422444Config_t),  //5
    PQ_IP_NLM_Size = sizeof(MDrvSclVipNlmMainConfig_t),  //6
    PQ_IP_422to444_Size = sizeof(MDrvSclVip422To444Config_t),  //7
    PQ_IP_VIP_Size = sizeof(MDrvSclVipByPassConfig_t),  //8
    PQ_IP_VIP_pseudo_Size = 0,  //9
    PQ_IP_VIP_LineBuffer_Size = sizeof(MDrvSclVipLineBUfferonfig_t),  //10
    PQ_IP_VIP_HLPF_Size = sizeof(MDrvSclVipHlpfConfig_t),  //11
    PQ_IP_VIP_HLPF_dither_Size = sizeof(MDrvSclVipHlpfDitherConfig_t),  //12
    PQ_IP_VIP_VLPF_coef1_Size = sizeof(MDrvSclVipVlpfCoefConfig_t),  //13
    PQ_IP_VIP_VLPF_coef2_Size = sizeof(MDrvSclVipVlpfCoefConfig_t),  //14
    PQ_IP_VIP_VLPF_dither_Size = sizeof(MDrvSclVipVlpfDitherConfig_t),  //15
    PQ_IP_VIP_Peaking_Size = sizeof(MDrvSclVipPeakingOnOffConfig_t),  //16
    PQ_IP_VIP_Peaking_band_Size = sizeof(MDrvSclVipPeakingBandConfig_t),  //17
    PQ_IP_VIP_Peaking_adptive_Size = sizeof(MDrvSclVipPeakingAdptiveConfig_t),  //18
    PQ_IP_VIP_Peaking_Pcoring_Size = sizeof(MDrvSclVipPeakingPCoringConfig_t),  //19
    PQ_IP_VIP_Peaking_Pcoring_ad_Y_Size = sizeof(MDrvSclVipPeakingAdpYConfig_t),  //20
    PQ_IP_VIP_Peaking_gain_Size = sizeof(MDrvSclVipPeakingGainConfig_t),  //21
    PQ_IP_VIP_Peaking_gain_ad_Y_Size = sizeof(MDrvSclVipPeakingGainApdYConfig_t),  //22
    PQ_IP_VIP_LCE_Size = sizeof(MDrvSclVipLceOnOffConfig_t),  //24
    PQ_IP_VIP_LCE_dither_Size = sizeof(MDrvSclVipLceDitherConfig_t),  //25
    PQ_IP_VIP_LCE_setting_Size = sizeof(MDrvSclVipLceSettingConfig_t),  //26
    PQ_IP_VIP_LCE_curve_Size = sizeof(MDrvSclVipLceCruveConfig_t),  //27
    PQ_IP_VIP_DLC_His_range_Size = sizeof(MDrvSclVipDlcHistogramEnConfig_t),  //30
    PQ_IP_VIP_DLC_Size = sizeof(MDrvSclVipDlcEnableConfig_t),  //28
    PQ_IP_VIP_DLC_dither_Size = sizeof(MDrvSclVipDlcDitherConfig_t),  //29
    PQ_IP_VIP_DLC_His_rangeH_Size = sizeof(MDrvSclVipDlcHistogramHConfig_t),  //31
    PQ_IP_VIP_DLC_His_rangeV_Size = sizeof(MDrvSclVipDlcHistogramVConfig_t),  //32
    PQ_IP_VIP_DLC_PC_Size = sizeof(MDrvSclVipDlcPcConfig_t),  //33
    PQ_IP_VIP_YC_gain_offset_Size = sizeof(MDrvSclVipYCGainOffsetConfig_t),  //23
    PQ_IP_VIP_UVC_Size = sizeof(MDrvSclVipUvcMainConfig_t),  //34
    PQ_IP_VIP_FCC_full_range_Size = sizeof(MDrvSclVipFccFullRangeConfig_t),  //35
    PQ_IP_VIP_FCC_bdry_dist_Size = 0,  //36
    PQ_IP_VIP_FCC_T1_Size = sizeof(MDrvSclVipFccTConfig_t),  //37
    PQ_IP_VIP_FCC_T2_Size = sizeof(MDrvSclVipFccTConfig_t),  //38
    PQ_IP_VIP_FCC_T3_Size = sizeof(MDrvSclVipFccTConfig_t),  //39
    PQ_IP_VIP_FCC_T4_Size = sizeof(MDrvSclVipFccTConfig_t),  //40
    PQ_IP_VIP_FCC_T5_Size = sizeof(MDrvSclVipFccTConfig_t),  //41
    PQ_IP_VIP_FCC_T6_Size = sizeof(MDrvSclVipFccTConfig_t),  //42
    PQ_IP_VIP_FCC_T7_Size = sizeof(MDrvSclVipFccTConfig_t),  //43
    PQ_IP_VIP_FCC_T8_Size = sizeof(MDrvSclVipFccTConfig_t),  //44
    PQ_IP_VIP_FCC_T9_Size = sizeof(MDrvSclVipFccT9Config_t),  //45
    PQ_IP_VIP_IHC_Size = sizeof(MDrvSclVipIhcOnOffConfig_t),  //46
    PQ_IP_VIP_IHC_Ymode_Size = sizeof(MDrvSclVipIhcYmodeConfig_t),  //47
    PQ_IP_VIP_IHC_dither_Size = sizeof(MDrvSclVipIhcDitherConfig_t),  //48
    PQ_IP_VIP_IHC_CRD_SRAM_Size = 0,  //49
    PQ_IP_VIP_IHC_SETTING_Size = sizeof(MDrvSclVipIhcSettingConfig_t),  //50
    PQ_IP_VIP_ICC_Size = sizeof(MDrvSclVipIccEnableConfig_t),  //51
    PQ_IP_VIP_ICC_Ymode_Size = sizeof(MDrvSclVipIccYmodeConfig_t),  //52
    PQ_IP_VIP_ICC_dither_Size = sizeof(MDrvSclVipIccDitherConfig_t),  //53
    PQ_IP_VIP_ICC_CRD_SRAM_Size = 0,  //54
    PQ_IP_VIP_ICC_SETTING_Size = sizeof(MDrvSclVipIccSettingConfig_t),  //55
    PQ_IP_VIP_Ymode_Yvalue_ALL_Size = sizeof(MDrvSclVipYmodeYvalueAllConfig_t),  //56
    PQ_IP_VIP_Ymode_Yvalue_SETTING_Size = sizeof(MDrvSclVipYmodeYvalueSettingConfig_t),  //57
    PQ_IP_VIP_IBC_Size = sizeof(MDrvSclVipIbcEnableConfig_t),  //58
    PQ_IP_VIP_IBC_dither_Size = sizeof(MDrvSclVipIbcDitherConfig_t),  //59
    PQ_IP_VIP_IBC_SETTING_Size = sizeof(MDrvSclVipIbcSettingConfig_t),  //60
    PQ_IP_VIP_ACK_Size = sizeof(MDrvSclVipAckMainConfig_t),  //61
    PQ_IP_VIP_YCbCr_Clip_Size = sizeof(MDrvSclVipYcbcrClipMainConfig_t),  //62
}PQ_IPTYPE_Size;

//=============================================================================

//=============================================================================

#endif//
