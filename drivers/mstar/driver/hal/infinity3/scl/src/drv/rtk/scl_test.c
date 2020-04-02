////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (¡§MStar Confidential Information¡¨) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

/**
* @file    drv_scl.c
* @version
* @brief   scl driver
*
*/

#define SCL_TEST_C__
/*=============================================================*/
// Include files
/*=============================================================*/
#include "vm_types.ht"
#include "sys_traces.ho"

#include "sys_sys_isw_cli.h"
#include "sys_sys_isw_uart.h"
#include "sys_sys.h"

#include "sys_sys_isw_uart.h"
#include "sys_MsWrapper_cus_os_msg.h"
#include "sys_MsWrapper_cus_os_timer.h"
#include "sys_MsWrapper_cus_os_mem.h"
#include "sys_MsWrapper_cus_os_util.h"
#include "sys_MsWrapper_cus_os_int_pub.h"
#include "sys_MsWrapper_cus_os_int_ctrl.h"
#include "sys_MsWrapper_cus_os_flag.h"
#include "sys_MsWrapper_cus_os_sem.h"


#include "vm_stdio.ho"

#include "kernel.h"
#include "drv_scl_os.h"
#include "drv_scl.h"
#include "drv_scl_dbg.h"
#include "drv_scl_hvsp_io_st.h"
#include "drv_scl_dma_io_st.h"
#include "drv_scl_pnl_io_st.h"
#include "drv_scl_hvsp_io.h"
#include "drv_scl_dma_io.h"
#include "drv_scl_pnl_io.h"
#include "drv_scl.h"
#include "drv_scl_irq_st.h"
#include "drv_scl_irq.h"
#include "sys_MsWrapper_cus_os_int_ctrl.h"
#include "drv_scl_verchk.h"
#include "hal_drv_util.h"
#include "drv_scl_pq_define.h"
#include "drv_scl_pq.h"
#include "drv_scl_vip_io_st.h"
#include "drv_scl_vip_io.h"
#include "cam_os_wrapper.h"
/*=============================================================*/
// Macro definition
/*=============================================================*/
#define POLLIN      0x0001
#define POLLPRI     0x0002
#define POLLOUT     0x0004
#define POLLERR     0x0008

#define VIP_DNR_Y_RANGE_NUM             4
#define VIP_DNR_C_RANGE_NUM             4
#define DRV_SCLVIP_IO_LCE_CURVE_SECTION_NUM       16
#define DRV_SCLVIP_IO_PEAKING_BAND_NUM            8
#define DRV_SCLVIP_IO_PEAKING_BAND_TERM_NUM       16
#define DRV_SCLVIP_IO_PEAKING_ADP_Y_LUT_NUM       8
#define DRV_SCLVIP_IO_DLC_HISTOGRAM_SECTION_NUM   7
#define DRV_SCLVIP_IO_DLC_HISTOGRAM_REPORT_NUM    8
#define DRV_SCLVIP_IO_DLC_LUMA_SECTION_NUM        64
#define DRV_SCLVIP_IO_IHC_COLOR_NUM               16
#define DRV_SCLVIP_IO_IHC_USER_COLOR_NUM          16
#define DRV_SCLVIP_IO_ICE_COLOR_NUM               16
#define DRV_SCLVIP_IO_IBC_COLOR_NUM               16
#define DRV_SCLVIP_IO_FCC_YWIN_LUT_ENTRY_NUM      17

#define VIP_DNR_SET_CHAR          "VIPDNR"
#define VIP_LDC_SET_CHAR          "VIPLDC"
#define VIP_PK_SET_CHAR           "VIPPK"
#define VIP_DLC_SET_CHAR          "VIPDLC"
#define VIP_LCE_SET_CHAR          "VIPLCE"
#define VIP_UVC_SET_CHAR          "VIPUVC"
#define VIP_IHC_SET_CHAR          "VIPIHC"
#define VIP_ICE_SET_CHAR          "VIPICE"
#define VIP_IBC_SET_CHAR          "VIPIBC"
#define VIP_FCC_SET_CHAR          "VIPFCC"
#define VIP_NLM_SET_CHAR          "VIPNLM"
#define VIP_SNR_SET_CHAR          "VIPSNR"
#define VIP_ACK_SET_CHAR          "VIPACK"
#define VIP_MIX_SET_CHAR          "VIPMIX"
#define VIP_VTRACK_SET_CHAR       "VIPSETVTRACK"
#define VIP_VTRACK_ONOFF_CHAR     "VIPONVTRACK"
#define VIP_SET_ALL_CHAR          "VIPALL"
#define VIP_SET_AIP_CHAR          "VIPAIP"
#define VIP_HIST_CHAR             "VIPHIST"

typedef enum
{
    E_DRV_ID_HVSP1,
    E_DRV_ID_HVSP2,
    E_DRV_ID_HVSP3,
    E_DRV_ID_SCLDMA1,
    E_DRV_ID_SCLDMA2,
    E_DRV_ID_SCLDMA3,
    E_DRV_ID_SCLDMA4,
    E_DRV_ID_VIP,
    E_DRV_ID_PNL,
    E_DRV_ID_MSYS,
    E_DRV_ID_MEM,
    E_DRV_ID_NUM,
}DrvIdType_e;

typedef enum
{
    E_DRV_SC3_SINGLE,
    E_DRV_SC3_MUTI,
    E_DRV_SC3_NUM,
}EN_DRV_SC3_TYPE;

typedef enum
{
    E_DRV_SC1_DSP,
    E_DRV_SC2_DSP,
    E_DRV_SC1_SC2_DSP,
    E_DRV_SC1_SC3_DSP,
    E_DRV_SC2_SC3_DSP,
    E_DRV_SC2_NUM,
}EN_DRV_SC2_TYPE;


/*=============================================================*/
// Data type definition
/*=============================================================*/
typedef struct
{
    u16 u16Src_Width;
    u16 u16Src_Height;
    u16 u16Dsp_Width;
    u16 u16Dsp_Height;

    DrvSclDmaIoBufferModeType_e enBufferMd;
    u16 u16BufNumber;
    DrvSclDmaIoColorType_e enColor;
    DrvSclDmaIoMemType_e enMem;
    DrvSclHvspIoSrcType_e enSrc;
    u32 u32count;
}FPGA_CONFIG;
typedef struct
{
    u16 u16Src_Width;
    u16 u16Src_Height;
    u16 u16sc1out_Width;
    u16 u16sc1out_Height;
    u16 u16Dsp_Width;
    u16 u16Dsp_Height;
    DrvSclDmaIoBufferModeType_e enBufferMd;
    DrvSclDmaIoBufferModeType_e enBufferMdsc2;
    EN_DRV_SC2_TYPE enSC2Dsp;
    u16 u16BufNumber;
    DrvSclDmaIoColorType_e enColor;
    DrvSclDmaIoColorType_e enColorsc2;
    DrvSclDmaIoMemType_e enMem;
    DrvSclDmaIoMemType_e enMemsc2;
    DrvSclHvspIoSrcType_e enSrc;
}FPGASC2_CONFIG;

typedef struct
{
    u16 u16Src_Width;
    u16 u16Src_Height;
    u16 u16crop_Width;
    u16 u16crop_Height;
    u16 u16Dsp_Width;
    u16 u16Dsp_Height;
    u16 u16crop_X;
    u16 u16crop_Y;
    DrvSclHvspIoSrcType_e enSrc;

}CROP_CONFIG;

typedef struct
{
    void *pVirAddr;
    u32 u32PhyAddr;
    u32 u32MIUAddr;
}SclMemConfig_t;


typedef struct
{
    u32 u32Base_Y[4];
    u32 u32Base_C[4];
    u32 u32Base_Y2[4];
    u32 u32Base_C2[4];
    u16 u16Width;
    u16 u16Height;
    u16 u16Width2;
    u16 u16Height2;
    DrvSclDmaIoColorType_e enColor;
    DrvSclDmaIoMemType_e enMem;
    DrvSclDmaIoColorType_e enColorsc2;
    DrvSclDmaIoMemType_e enMemsc2;
}ScldmaSwapBufferConfig_t;




typedef struct
{
    unsigned int Count;
    DrvIdType_e  enID[5];
    DrvSclDmaIoMemType_e enMem[5];
}SclDmaGetActiveConfig_t;

typedef struct
{
    bool bEn;
    u16 u16BufNumber;
    DrvSclDmaIoBufferModeType_e enBufferMd;
    DrvSclDmaIoBufferModeType_e enBufferMdsc2;
    EN_DRV_SC3_TYPE enSC3Md;
    EN_DRV_SC2_TYPE enSC2Md;
    ScldmaSwapBufferConfig_t stSwapCfg;
    SclDmaGetActiveConfig_t   stGetActCfg;
    bool bswitchpnl;
    bool bsnp;
}SclDmaPollConfig_t;

typedef void (*Task_CB) (void * );

typedef struct
{
    void *pStackTop;
    MsTaskId_e TaskId;
}SclThreadConfig_t;

typedef enum
{
    E_MULTI_INST_SC1_2_THREAD_1 = 0x001,
    E_MULTI_INST_SC1_2_THREAD_2 = 0x002,
    E_MULTI_INST_SC1_2_THREAD_3 = 0x004,
    E_MULTI_INST_SC3_THREAD_1   = 0x008,
    E_MULTI_INST_SC3_THREAD_2   = 0x010,
    E_MULTI_INST_SC3_THREAD_3   = 0x020,
    E_MULTI_INST_DISP_THREAD_1  = 0x040,
    E_MULTI_INST_DISP_THREAD_2  = 0x080,
    E_MULTI_INST_DISP_THREAD_3  = 0x100,
}MULTIINST_TEST_OPTION;


typedef struct
{
    DrvSclVipIoFcConfig_t stFCfg;
    bool bEn;                              // DNR enable
    bool bCoreEn;                          // DNR core function enable
    u16 u16Width;
    u16 u16Height;
    u8 u8Table_Y[VIP_DNR_Y_RANGE_NUM];   // [7:0] DNR Y strength in Range 0 ~ 7
    u8 u8Table_C[VIP_DNR_C_RANGE_NUM];   // [7:0] DNR C strength in Range 0 ~ 7
}VipDnrConfig_t;

typedef struct
{
    DrvSclVipIoFcConfig_t stFCfg;
    bool bEn;
    DrvSclVipIoLdcBypassType_e enbypass;
    u16 u16Width;
    u16 u16Height;

}VipLdcConfig_t;

typedef struct
{
    DrvSclVipIoFcConfig_t stFCfg;
    bool bEn2D;
    bool bEnband;
    bool bEnADPY;
    bool bEnADPYLPF;
    bool bEnOSD;
}VipPkConfig_t;

typedef struct
{
    DrvSclVipIoFcConfig_t stFCfg;
    bool bVariable_Section;
    bool  bDLC_En;
    bool  bDither_En;
    bool  bDLC_SubWinEn;
    bool  bCurvefit_PW_En;
}VipDlcConfig_t;


typedef struct
{
    DrvSclVipIoFcConfig_t stFCfg;
    bool  bLCE_En;
    bool  bDering_Alpha_En;
    bool  bColor_Alpha_En;
    bool  bDither_En;
    bool  b3curve_En;
    bool  bLCE_SODC_En;
    DrvSclVipIoLceYAveSelType_e LCEtype;
}VipLceConfig_t;

typedef struct
{
    DrvSclVipIoFcConfig_t stFCfg;
    bool bUVC_En;
    bool bUVC_adpEn;
    bool bFllRGB_Mode;
    DrvSclVipIoUvcAdpYInputSelType_e enAdp_Ysel;
}VipUvcConfig_t;


typedef struct
{
    DrvSclVipIoFcConfig_t stFCfg;
    bool bIHC_En;
    bool bAdp_Ydiff_en;
    bool bAdp_Y_en;
}VipIhcConfig_t;

typedef struct
{
    DrvSclVipIoFcConfig_t stFCfg;
    bool bICE_En;
    bool bAdp_Ydiff_en;
    bool bAdp_Y_en;
}VipIceConfig_t;

typedef struct
{
    DrvSclVipIoFcConfig_t stFCfg;
    bool bEn;
}VipIbcConfig_t;

typedef struct
{
    DrvSclVipIoFcConfig_t stFCfg;
    bool bEn;
}VipFccConfig_t;



/*=============================================================*/
// Variable definition
/*=============================================================*/
static char _szSclRegRWHelpTxt[] = "Register R/W";
static char _szSclRegRWUsageTxt[] = "[Addr] / [Addr Val]";


static char _szSclOsEventHelpTxt[] = "OsEvent";
static char _szSclOsEventUsageTxt[] = "[set event] [wait event] [mode]";

static char _szSclOsMemHelpTxt[] = "OsMem";
static char _szSclOsMemUsageTxt[] = "[size]";

static char _szSclsPnlHelpTxt[] = "Pnl";
static char _szSclsPnlUsageTxt[] = "[width] [height]";

static char _szSclFpga14HelpTxt[] = "Fpga";
static char _szSclFpga14UsageTxt[] = "";

static char _szSclHvspHelpTxt[] = "Hvsp";
static char _szSclHvspUsageTxt[] = "";

static char _szSclFpga24HelpTxt[] = "Fpga24";
static char _szSclFpga24UsageTxt[] = "";

static char _szSclFpgaSC2HelpTxt[] = "FpgaSC2";
static char _szSclFpgaSC2UsageTxt[] = "";

static char _szSclMultiHelpTxt[] = "Multi";
static char _szSclMultiUsageTxt[] = "";

static char _szSclASICCROPHelpTxt[] = "ASICCROP";
static char _szSclASICCROPUsageTxt[] = "";

static char _szSclVIPHelpTxt[] = "ASICCROP";
static char _szSclVIPPUsageTxt[] = "";

/*=============================================================*/
// Global Variable definition
/*=============================================================*/

int _SclRegRW(CLI_t *pCli, char *p);
int _SclOsEvent(CLI_t *pCli, char *p);
int _SclOsMem(CLI_t *pCli, char *p);
int _SclPnl(CLI_t *pCli, char *p);
int _SclFpga14(CLI_t *pCli, char *p);
int _SclHvsp(CLI_t *pCli, char *p);
int _SclFpga24(CLI_t *pCli, char *p);
int _SclFpgaSC2(CLI_t *pCli, char *p);
int _SclMulti(CLI_t *pCli, char *p);
int _SclASICCROP(CLI_t *pCli, char *p);
int _IspRun(CLI_t *pCli, char *p);
int _SclVip(CLI_t *pCli, char *p);

CliParseToken_t g_atSclMenuTbl[] =
    {
        {"REG",      _szSclRegRWHelpTxt,      _szSclRegRWUsageTxt,      _SclRegRW,     NULL},
        {"Event",    _szSclOsEventHelpTxt,    _szSclOsEventUsageTxt,    _SclOsEvent, NULL},
        {"Mem",      _szSclOsMemHelpTxt,      _szSclOsMemUsageTxt,      _SclOsMem,   NULL},
        {"Pnl",      _szSclsPnlHelpTxt,       _szSclsPnlUsageTxt,       _SclPnl,       NULL},
        {"FPGA14",   _szSclFpga14HelpTxt,     _szSclFpga14UsageTxt,     _SclFpga14,    NULL},
        {"HVSP",     _szSclHvspHelpTxt,       _szSclHvspUsageTxt,       _SclHvsp,      NULL},
        {"FPGA24",   _szSclFpga24HelpTxt,     _szSclFpga24UsageTxt,     _SclFpga24,    NULL},
        {"FPGASC2",  _szSclFpgaSC2HelpTxt,   _szSclFpgaSC2UsageTxt,     _SclFpgaSC2,   NULL},
        {"MULTI",    _szSclMultiHelpTxt,      _szSclMultiUsageTxt,      _SclMulti,     NULL},
        {"ASICCROP", _szSclASICCROPHelpTxt, _szSclASICCROPUsageTxt,     _SclASICCROP,  NULL},
        {"VIP",      _szSclVIPHelpTxt,        _szSclVIPPUsageTxt,       _SclVip,       NULL},
        PARSE_TOKEN_DELIMITER
    };


/*=============================================================*/
// Local function definition
/*=============================================================*/
s32 g_FD[E_DRV_ID_NUM] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

/*=============================================================*/
//          Parsing Function
/*=============================================================*/

u32 _ParsingNum(CLI_t *pCli)
{
    u32 value;

    if(CliTokenPopNum(pCli, &value, 10) != eCLI_PARSE_OK)
    {
        return 0;
    }
    else
    {
        return value;
    }
}

u32 _ParsingHexNum(CLI_t *pCli)
{
    u32 value;

    if(CliTokenPopNum(pCli, &value, 16) != eCLI_PARSE_OK)
    {
        return 0;
    }
    else
    {
        return value;
    }
}


int Parsing_String(CLI_t *pCli, char *dest)
{
    char *src;

    pCli->tokenLvl++;
    src = CliTokenPop(pCli);

    if(strcmp(src, dest) == 0)
        return 1;
    else
        return 0;
}

int Parsing_VipString(char *src, char *dest)
{
    if(strcmp(src, dest) == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

DrvSclHvspIoSrcType_e _ParsingSclHvspSrc(CLI_t *pCli)
{
    char *pStr;
    DrvSclHvspIoSrcType_e enSrc = E_DRV_SCLHVSP_IO_SRC_NUM;

    pCli->tokenLvl++;
    pStr = CliTokenPop(pCli);

    if(strcmp(pStr, "ISP") == 0)
        enSrc = E_DRV_SCLHVSP_IO_SRC_ISP;
    else if(strcmp(pStr, "BT656") == 0)
        enSrc = E_DRV_SCLHVSP_IO_SRC_BT656;
    else if(strcmp(pStr, "DRAM") == 0)
        enSrc = E_DRV_SCLHVSP_IO_SRC_DRAM;
    else if(strcmp(pStr, "HVSP") == 0)
        enSrc = E_DRV_SCLHVSP_IO_SRC_HVSP;
    else if(strcmp(pStr, "PAT") == 0)
        enSrc = E_DRV_SCLHVSP_IO_SRC_PAT_TGEN;

    return enSrc;
}

DrvSclHvspIoIdType_e  _ParsingSclHvspId(CLI_t *pCli)
{
    char *pStr;
    DrvSclHvspIoIdType_e enID = E_DRV_SCLHVSP_IO_ID_NUM;

    pCli->tokenLvl++;
    pStr = CliTokenPop(pCli);

    if(strcmp(pStr, "SC1") == 0)
        enID = E_DRV_SCLHVSP_IO_ID_1;
    else if(strcmp(pStr, "SC2") == 0)
        enID = E_DRV_SCLHVSP_IO_ID_2;
    else if(strcmp(pStr, "SC3") == 0)
        enID = E_DRV_SCLHVSP_IO_ID_3;

    return enID;
}

DrvSclDmaIoBufferModeType_e _ParsingSclDmaBufferMode(CLI_t *pCli)
{
    char *pStr;
    DrvSclDmaIoBufferModeType_e enBufMode;

    pCli->tokenLvl++;
    pStr = CliTokenPop(pCli);
    enBufMode = E_DRV_SCLDMA_IO_BUFFER_MD_SINGLE;

   if(strcmp(pStr, "RING") == 0)
       enBufMode = E_DRV_SCLDMA_IO_BUFFER_MD_RING;
   else if(strcmp(pStr, "SING") == 0)
       enBufMode = E_DRV_SCLDMA_IO_BUFFER_MD_SINGLE;
   else if(strcmp(pStr, "SWRING") == 0)
       enBufMode = E_DRV_SCLDMA_IO_BUFFER_MD_SWRING;

   return enBufMode;
}

DrvSclDmaIoColorType_e _ParsingSclDmaColor(CLI_t *pCli)
{
    char *pStr;
    DrvSclDmaIoColorType_e enColor = E_DRV_SCLDMA_IO_COLOR_NUM;

    pCli->tokenLvl++;
    pStr = CliTokenPop(pCli);

    if(strcmp(pStr, "YUV420Sep") == 0)
        enColor = E_DRV_SCLDMA_IO_COLOR_YUVSep420;
    else if(strcmp(pStr, "YUV422Sep") == 0)
        enColor = E_DRV_SCLDMA_IO_COLOR_YUVSep422;
    else if(strcmp(pStr, "YUV422") == 0)
        enColor = E_DRV_SCLDMA_IO_COLOR_YUV422;
    else if(strcmp(pStr, "YUV420") == 0)
        enColor = E_DRV_SCLDMA_IO_COLOR_YUV420;
    else if(strcmp(pStr, "YC422Sep") == 0)
        enColor = E_DRV_SCLDMA_IO_COLOR_YCSep422;
    return enColor;
}

DrvSclDmaIoMemType_e _ParsingSclDmaMem(CLI_t *pCli)
{
    char *pStr;
    DrvSclDmaIoMemType_e enMem = E_DRV_SCLDMA_IO_MEM_NUM;

    pCli->tokenLvl++;
    pStr = CliTokenPop(pCli);

    if(strcmp(pStr, "FRM") == 0)
        enMem = E_DRV_SCLDMA_IO_MEM_FRM;
    else if(strcmp(pStr, "SNP") == 0)
        enMem = E_DRV_SCLDMA_IO_MEM_SNP;
    else if(strcmp(pStr, "IMI") == 0)
        enMem = E_DRV_SCLDMA_IO_MEM_IMI;
    else if(strcmp(pStr, "FRM2") == 0)
        enMem = E_DRV_SCLDMA_IO_MEM_FRM2;

    return enMem;
}

EN_DRV_SC2_TYPE _ParsingSC2Md(CLI_t *pCli)
{
    char *pStr;
    EN_DRV_SC2_TYPE enBufMode = E_DRV_SC2_NUM;

    pCli->tokenLvl++;
    pStr = CliTokenPop(pCli);

    if(strcmp(pStr, "MUTI") == 0)
        enBufMode = E_DRV_SC1_SC2_DSP;
    else if(strcmp(pStr, "SC1") == 0)
        enBufMode = E_DRV_SC1_DSP;
    else if(strcmp(pStr, "SC2") == 0)
        enBufMode = E_DRV_SC2_DSP;
    else if(strcmp(pStr, "SC1SC3") == 0)
        enBufMode = E_DRV_SC1_SC3_DSP;
    else if(strcmp(pStr, "SC2SC3") == 0)
        enBufMode = E_DRV_SC2_SC3_DSP;
    return enBufMode;
}
/*=============================================================*/
//          Unit Test Function
/*=============================================================*/
bool Init_Device(void)
{
    if( DrvSclOsGetProbeInformation(E_DRV_SCLOS_INIT_HVSP) == 0)
    {
        if(DrvSclHvspIoInit() != E_DRV_SCLHVSP_IO_ERR_OK)
        {
            sclprintf("DrvSclHvspIoInit fail\n");
            return FALSE;
        }
    }

    if( DrvSclOsGetProbeInformation(E_DRV_SCLOS_INIT_PNL) == 0)
    {
        if(DrvSclPnlIoInit() != E_DRV_SCLPNL_IO_ERR_OK)
        {
            sclprintf("DrvSclPnlIoInit fail\n");
            return FALSE;
        }
    }

    if( DrvSclOsGetProbeInformation(E_DRV_SCLOS_INIT_DMA) == 0)
    {
        if(DrvSclDmaIoInit() != E_DRV_SCLDMA_IO_ERR_OK)
        {
            sclprintf("DrvSclDmaIoInit fail\n");
            return FALSE;
        }
    }

    if( DrvSclOsGetProbeInformation(E_DRV_SCLOS_INIT_VIP) == 0)
    {
        if(DrvSclVipIoInit() != E_DRV_SCLVIP_IO_ERR_OK)
        {
            sclprintf("DrvSclVipIoInit fail\n");
            return FALSE;
        }
    }

    Reset_DBGMG_FLAG();
    return TRUE;
}

void Init_Debug_Msg(void)
{
    Reset_DBGMG_FLAG();
    Set_DBGMG_FLAG(EN_DBGMG_PRIORITY_CONFIG,1);
    Set_DBGMG_FLAG(EN_DBGMG_DRVSCLDMA_CONFIG, 1);
    Set_DBGMG_FLAG(EN_DBGMG_DRVHVSP_CONFIG, 1);
    Set_DBGMG_FLAG(EN_DBGMG_DRVSCLIRQ_CONFIG, 1);
    Set_DBGMG_FLAG(EN_DBGMG_HVSP_CONFIG, 1);
    Set_DBGMG_FLAG(EN_DBGMG_MDRV_CONFIG, 1);
    Set_DBGMG_FLAG(EN_DBGMG_INST_ENTRY_CONFIG, 0xFF);
    Set_DBGMG_FLAG(EN_DBGMG_INST_LOCK_CONFIG, 0xFF);
    Set_DBGMG_FLAG(EN_DBGMG_INST_FUNC_CONFIG, 1);
    Set_DBGMG_FLAG(EN_DBGMG_DRVPQ_CONFIG, 1);
    Set_DBGMG_FLAG(EN_DBGMG_VIP_CONFIG, 0xFF);
    Set_DBGMG_FLAG(EN_DBGMG_DRVHVSP_CONFIG, 0xFF);
}
int Close_Device(DrvIdType_e enDrvID)
{
    int ret = 1;
    char device_name[E_DRV_ID_NUM][50] =
    {
        {"mhvsp1"},
        {"mhvsp2"},
        {"mhvsp3"},
        {"mscldma1"},
        {"mscldma2"},
        {"mscldma3"},
        {"mscldma4"},
        {"mvip"},
        {"mpnl"},
        {"msys"},
        {"mem"},
    };
    if(enDrvID >= E_DRV_ID_NUM)
    {
        sclprintf("ID is not correct\n");
        return 0;
    }

    if(g_FD[enDrvID] != -1)
    {
        switch(enDrvID)
        {
        case E_DRV_ID_HVSP1:
            DrvSclHvspIoRelease(g_FD[enDrvID]);
            break;

        case E_DRV_ID_HVSP2:
            DrvSclHvspIoRelease(g_FD[enDrvID]);
            break;
        case E_DRV_ID_HVSP3:
            DrvSclHvspIoRelease(g_FD[enDrvID]);
            break;
        case E_DRV_ID_SCLDMA1:
            DrvSclDmaIoRelease(g_FD[enDrvID]);
            break;

        case E_DRV_ID_SCLDMA2:
            DrvSclDmaIoRelease(g_FD[enDrvID]);
            break;

        case E_DRV_ID_SCLDMA3:
            DrvSclDmaIoRelease(g_FD[enDrvID]);
            break;

        case E_DRV_ID_SCLDMA4:
            DrvSclDmaIoRelease(g_FD[enDrvID]);
            break;

        case E_DRV_ID_PNL:
            DrvSclPnlIoRelease(g_FD[enDrvID]);
            break;
            case E_DRV_ID_VIP:
                DrvSclVipIoRelease(g_FD[enDrvID]);
                break;
        default:

        case E_DRV_ID_MSYS:
        case E_DRV_ID_MEM:
            g_FD[enDrvID] = -1;
            break;
        }
        g_FD[enDrvID] = -1;
        if(g_FD[enDrvID] == -1)
        {
            sclprintf("close %s sucess\n", &device_name[enDrvID][0]);
            ret = 0;
        }
        else
        {
            sclprintf("[open]close  %s  %d fail\n", &device_name[enDrvID][0],g_FD[enDrvID]);
            ret = 1;
        }
    }
    return ret;
}

int Open_Device(DrvIdType_e enDrvID)
{
    int ret = 1;
    char device_name[E_DRV_ID_NUM][50] =
    {
        {"mhvsp1"},
        {"mhvsp2"},
        {"mhvsp3"},
        {"mscldma1"},
        {"mscldma2"},
        {"mscldma3"},
        {"mscldma4"},
        {"mvip"},
        {"mpnl"},
        {"msys"},
        {"mem"},
    };
    if(enDrvID >= E_DRV_ID_NUM)
    {
        sclprintf("ID is not correct\n");
        return 0;
    }

    if(g_FD[enDrvID] != -1)
    {
    }
    else
    {
        switch(enDrvID)
        {
        case E_DRV_ID_HVSP1:
            g_FD[enDrvID] = DrvSclHvspIoOpen(E_DRV_SCLHVSP_IO_ID_1);
            break;

        case E_DRV_ID_HVSP2:
            g_FD[enDrvID] = DrvSclHvspIoOpen(E_DRV_SCLHVSP_IO_ID_2);
            break;
        case E_DRV_ID_HVSP3:
            g_FD[enDrvID] = DrvSclHvspIoOpen(E_DRV_SCLHVSP_IO_ID_3);
            break;
        case E_DRV_ID_SCLDMA1:
            g_FD[enDrvID] = DrvSclDmaIoOpen(E_DRV_SCLDMA_IO_ID_1);
            break;

        case E_DRV_ID_SCLDMA2:
            g_FD[enDrvID] = DrvSclDmaIoOpen(E_DRV_SCLDMA_IO_ID_2);
            break;

        case E_DRV_ID_SCLDMA3:
            g_FD[enDrvID] = DrvSclDmaIoOpen(E_DRV_SCLDMA_IO_ID_3);
            break;

        case E_DRV_ID_SCLDMA4:
            g_FD[enDrvID] = DrvSclDmaIoOpen(E_DRV_SCLDMA_IO_ID_4);
            break;

        case E_DRV_ID_PNL:
            g_FD[enDrvID] = DrvSclPnlIoOpen(E_DRV_SCLPNL_IO_ID_1);
            break;

        default:
        case E_DRV_ID_VIP:
            g_FD[enDrvID] = DrvSclVipIoOpen(E_DRV_SCLVIP_IO_ID_1);
            break;

        case E_DRV_ID_MSYS:
        case E_DRV_ID_MEM:
            g_FD[enDrvID] = -1;
            break;
        }

        if(g_FD[enDrvID] == -1)
        {
            sclprintf("open %s fail\n", &device_name[enDrvID][0]);
            ret = 0;
        }
        else
        {
            sclprintf("[open]open %s  %d sucess\n", &device_name[enDrvID][0],g_FD[enDrvID]);
            ret = 1;
        }
    }
    return ret;
}

void _Request_IPM(u16 u16Src_Width,u16 u16Src_Height)
{
    // HVSP1 Request IPM memory
    DrvSclHvspIoReqMemConfig_t stHvspReqMemCfg;
    stHvspReqMemCfg.u16Pitch = u16Src_Width;
    stHvspReqMemCfg.u16Vsize = u16Src_Height;
    stHvspReqMemCfg.u32MemSize = u16Src_Width * u16Src_Height *2 * 2;
    stHvspReqMemCfg =
      FILL_VERCHK_TYPE(stHvspReqMemCfg, stHvspReqMemCfg.VerChk_Version, stHvspReqMemCfg.VerChk_Size,DRV_SCLHVSP_VERSION);
    DrvSclHvspIoReqmemConfig(g_FD[E_DRV_ID_HVSP1],&stHvspReqMemCfg);

}

DrvSclHvspIoInputConfig_t _GetInputTiming(u16 u16Src_Width,u16 u16Src_Height)
{
    DrvSclHvspIoInputConfig_t   stHvspInCfg;
    if(u16Src_Width == 1920 && u16Src_Height == 1080)
    {  // 1920x1080_30P
        stHvspInCfg.stTimingCfg.u16Htotal = 2200;
        stHvspInCfg.stTimingCfg.u16Vtotal = 1125;
        stHvspInCfg.stTimingCfg.u16Vfrequency = 30;
    }
    else if(u16Src_Width == 1024 && u16Src_Height == 768)
    { // 1024_768_60p
        stHvspInCfg.stTimingCfg.u16Htotal = 1344;
        stHvspInCfg.stTimingCfg.u16Vtotal = 806;
        stHvspInCfg.stTimingCfg.u16Vfrequency = 60;
    }
    else if(u16Src_Width == 640 && u16Src_Height == 480)
    { //640x480_60p
        stHvspInCfg.stTimingCfg.u16Htotal = 800;
        stHvspInCfg.stTimingCfg.u16Vtotal = 525;
        stHvspInCfg.stTimingCfg.u16Vfrequency = 60;
    }
    else
    {
        stHvspInCfg.stTimingCfg.u16Htotal = 0x7FF;
        stHvspInCfg.stTimingCfg.u16Vtotal = 0x7FF;
        stHvspInCfg.stTimingCfg.u16Vfrequency = 60;
    }
    stHvspInCfg.stTimingCfg.bInterlace = 0;
    return stHvspInCfg;
}

void _HVSPInConfig
    (DrvIdType_e enType, DrvSclHvspIoSrcType_e enSrc,u16 u16Src_Width,u16 u16Src_Height)
{
    DrvSclHvspIoInputConfig_t   stHvspInCfg;
    memset(&stHvspInCfg, 0, sizeof(DrvSclHvspIoInputConfig_t));
    if(enSrc==E_DRV_SCLHVSP_IO_SRC_PAT_TGEN && enType==E_DRV_ID_HVSP1)
    {
        stHvspInCfg = _GetInputTiming(u16Src_Width,u16Src_Height);
    }
    stHvspInCfg.stCaptureWin.u16X = 0;
    stHvspInCfg.stCaptureWin.u16Y = 0;
    stHvspInCfg.stCaptureWin.u16Width= u16Src_Width;
    stHvspInCfg.stCaptureWin.u16Height= u16Src_Height;
    stHvspInCfg.enSrcType = enSrc;
    stHvspInCfg = FILL_VERCHK_TYPE(stHvspInCfg, stHvspInCfg.VerChk_Version, stHvspInCfg.VerChk_Size,DRV_SCLHVSP_VERSION);
    DrvSclHvspIoSetInputConfig(g_FD[enType], &stHvspInCfg);
}

DrvSclHvspIoWindowConfig_t _FillHVSPScaleCropCfg
    (u16 u16Src_Width,u16 u16Src_Height,u16 u16X,u16 u16Y)
{
    DrvSclHvspIoWindowConfig_t stCropWin;
    stCropWin.u16X      = u16X;
    stCropWin.u16Y      = u16Y;
    stCropWin.u16Width  = u16Src_Width;
    stCropWin.u16Height = u16Src_Height;
    return stCropWin;
}

DrvSclHvspIoScalingConfig_t _FillHVSPScaleCfg
    (u16 u16Src_Width,u16 u16Src_Height,u16 u16Dsp_Width,u16 u16Dsp_Height)
{
    DrvSclHvspIoScalingConfig_t stHvspScaleCfg;
    stHvspScaleCfg.u16Src_Width  = u16Src_Width;
    stHvspScaleCfg.u16Src_Height = u16Src_Height;
    stHvspScaleCfg.u16Dsp_Width  = u16Dsp_Width;
    stHvspScaleCfg.u16Dsp_Height = u16Dsp_Height;
    return stHvspScaleCfg;

}

void _HVSPScaling(DrvIdType_e enType, DrvSclHvspIoScalingConfig_t stHvspScaleCfg, DrvSclHvspIoSrcType_e enSrc)
{
    stHvspScaleCfg = FILL_VERCHK_TYPE(stHvspScaleCfg, stHvspScaleCfg.VerChk_Version,
        stHvspScaleCfg.VerChk_Size,DRV_SCLHVSP_VERSION);
    DrvSclHvspIoSetScalingConfig(g_FD[enType], &stHvspScaleCfg);
}

SclMemConfig_t _AllocateDmem
    (const char *name ,u16 u16BufNumber,u32 buffsize)
{
    SclMemConfig_t stMsysMemInfo;

    stMsysMemInfo.pVirAddr = DrvSclOsMemalloc(buffsize * u16BufNumber * 2, 0);
    stMsysMemInfo.u32PhyAddr = (u32)MsVA2PA(stMsysMemInfo.pVirAddr);
    stMsysMemInfo.u32MIUAddr = HalUtilPHY2MIUAddr(stMsysMemInfo.u32PhyAddr);


    sclprintf("[FPGA0]Phy:%x,Vir:%x\n",(stMsysMemInfo.u32PhyAddr),(u32)stMsysMemInfo.pVirAddr);
    return stMsysMemInfo;
}

void _FreeDmem(SclMemConfig_t stMemCfg)
{
    DrvSclOsMemFree(stMemCfg.pVirAddr);
}

SclMemConfig_t _TranslationPhyToMIU(SclMemConfig_t stMsysMemInfo)
{
    stMsysMemInfo.u32MIUAddr = HalUtilPHY2MIUAddr(stMsysMemInfo.u32PhyAddr);
    return stMsysMemInfo;
}

bool _FPGA_Pnl(u16 width,u16 height)
{
    DrvSclPnlIoTimingConfig_t tPnlTimingCfg;

    u16 width2,height2;
    if(height<0x1E0)
     height2=0x1E0;
    else
     height2=height;
    if(width<0x320)
    {
        width2=0x320;
    }
    else
    {
        width2=width;
    }

    tPnlTimingCfg.u16Vsync_St  = 0x0000;
    tPnlTimingCfg.u16Vsync_End = 0x0003;
    tPnlTimingCfg.u16Vde_St    = 0x0010;
    tPnlTimingCfg.u16Vde_End   = 0x000F+height;
    tPnlTimingCfg.u16Vfde_St   = 0x0010;
    tPnlTimingCfg.u16Vfde_End  = 0x000F+height2;
    tPnlTimingCfg.u16Vtt       = 0x020D;
    tPnlTimingCfg.u16Hsync_St  = 0x0000;
    tPnlTimingCfg.u16Hsync_End = 0x0030;
    tPnlTimingCfg.u16Hde_St    = 0x0058;
    tPnlTimingCfg.u16Hde_End   = 0x0057+width;
    tPnlTimingCfg.u16Hfde_St   = 0x0058;
    tPnlTimingCfg.u16Hfde_End  = 0x0057+width2;
    tPnlTimingCfg.u16Htt       = 0x03FF;
    tPnlTimingCfg.u16VFreqx10  = 680;
    tPnlTimingCfg = FILL_VERCHK_TYPE(tPnlTimingCfg, tPnlTimingCfg.VerChk_Version, tPnlTimingCfg.VerChk_Size, DRV_SCLPNL_VERSION);

    if(DrvSclPnlIoSetTimingConfig(g_FD[E_DRV_ID_PNL], &tPnlTimingCfg) != E_DRV_SCLPNL_IO_ERR_OK)
    {
        sclprintf("DrvSclPnlIoSetTimingConfig fail\n");
        return FALSE;
    }

    return TRUE;

}

void _ISP_Input(u16 Width,u16 Height)
{
    DrvSclHvspIoMiscConfig_t stHvspMiscCfg;
    u8 input_tgen_buf[] =
    {
        0x18, 0x12, 0x88, 0x20, 0xFF,
        0x18, 0x12, 0x89, 0x0C, 0xFF,
        0x18, 0x12, 0x86, 0x02, 0xFF,
        0x18, 0x12, 0xE0, 0x00, 0xFF,
    };
    stHvspMiscCfg.u8Cmd = 0;
    stHvspMiscCfg.u32Size = sizeof(input_tgen_buf);
    stHvspMiscCfg.u32Addr = (u32)input_tgen_buf;
    DrvSclHvspIoSetMiscConfig(g_FD[E_DRV_ID_HVSP1], &stHvspMiscCfg);
}


void _FPGA_Chiptop(void)
{
    DrvSclHvspIoMiscConfig_t stHvspMiscCfg;
    u8 input_tgen_buf[] =
    {
        0x1e, 0x10, 0x1e, 0x40, 0xFF,
        0x1e, 0x10, 0xA0, 0x00, 0xFF,
        0x1e, 0x10, 0xA1, 0x00, 0xFF,

    };

    // Input tgen setting
    stHvspMiscCfg.u8Cmd = 0;
    stHvspMiscCfg.u32Size = sizeof(input_tgen_buf);
    stHvspMiscCfg.u32Addr = (u32)input_tgen_buf;
    sclprintf("Size=%d\n", stHvspMiscCfg.u32Size);
    DrvSclHvspIoSetMiscConfig(g_FD[E_DRV_ID_HVSP1], &stHvspMiscCfg);

}

DrvSclDmaIoBufferConfig_t _FillBufferConfigByFPGAConfig
    (DrvIdType_e enType,FPGA_CONFIG stCfg,SclMemConfig_t stMsysMemInfo, bool bin)
{
    u16 BufNum;
    DrvSclDmaIoBufferConfig_t stSCLDMACfg;
    int BufferIdx;
    u32 buffsize = stCfg.u16Dsp_Width * stCfg.u16Dsp_Height ; // 2bpp
    BufNum = stCfg.enBufferMd == E_DRV_SCLDMA_IO_BUFFER_MD_SINGLE ? 1 : stCfg.u16BufNumber;
    stSCLDMACfg.enBufMDType = stCfg.enBufferMd;
    stSCLDMACfg.enMemType   = stCfg.enMem;
    stSCLDMACfg.enColorType = stCfg.enColor;
    stSCLDMACfg.u16Height   = stCfg.u16Dsp_Height;
    stSCLDMACfg.u16Width    = stCfg.u16Dsp_Width;
    stSCLDMACfg.u16BufNum    = BufNum;

    stSCLDMACfg.bvFlag.btsBase_0 = stSCLDMACfg.u16BufNum >=1 ? 1 : 0;
    stSCLDMACfg.bvFlag.btsBase_1 = stSCLDMACfg.u16BufNum >=2 ? 1 : 0;
    stSCLDMACfg.bvFlag.btsBase_2 = stSCLDMACfg.u16BufNum >=3 ? 1 : 0;
    stSCLDMACfg.bvFlag.btsBase_3 = stSCLDMACfg.u16BufNum >=4 ? 1 : 0;

    if (stSCLDMACfg.enColorType    == E_DRV_SCLDMA_IO_COLOR_YUV422)
    {
        for(BufferIdx=0; BufferIdx<stSCLDMACfg.u16BufNum ; BufferIdx++)
        {
            stSCLDMACfg.u32Base_Y[BufferIdx] = stMsysMemInfo.u32PhyAddr + (buffsize) *2* BufferIdx;
            stSCLDMACfg.u32Base_C[BufferIdx] = stSCLDMACfg.u32Base_Y[BufferIdx]+16;
        }
    }
    else if ((stSCLDMACfg.enColorType == E_DRV_SCLDMA_IO_COLOR_YUVSep422))
    {
        for(BufferIdx=0; BufferIdx<stSCLDMACfg.u16BufNum ; BufferIdx++)
        {
            stSCLDMACfg.u32Base_Y[BufferIdx] = stMsysMemInfo.u32PhyAddr + (buffsize*2*BufferIdx);
            stSCLDMACfg.u32Base_C[BufferIdx] = stSCLDMACfg.u32Base_Y[BufferIdx] + (buffsize);
            stSCLDMACfg.u32Base_V[BufferIdx] = stSCLDMACfg.u32Base_C[BufferIdx] + (buffsize/2);
        }
    }
    else if ((stSCLDMACfg.enColorType == E_DRV_SCLDMA_IO_COLOR_YUVSep420))
    {
        for(BufferIdx=0; BufferIdx<stSCLDMACfg.u16BufNum ; BufferIdx++)
        {
            stSCLDMACfg.u32Base_Y[BufferIdx] = stMsysMemInfo.u32PhyAddr + (buffsize*3/2*BufferIdx);
            stSCLDMACfg.u32Base_C[BufferIdx] = stSCLDMACfg.u32Base_Y[BufferIdx] + (buffsize);
            stSCLDMACfg.u32Base_V[BufferIdx] = stSCLDMACfg.u32Base_C[BufferIdx] + (buffsize/4);
        }
    }
    else if ((stSCLDMACfg.enColorType == E_DRV_SCLDMA_IO_COLOR_YCSep422))
    {
        for(BufferIdx=0; BufferIdx<stCfg.u16BufNumber ; BufferIdx++)
        {
            stSCLDMACfg.u32Base_Y[BufferIdx] = stMsysMemInfo.u32PhyAddr + (buffsize*2*BufferIdx);
            stSCLDMACfg.u32Base_C[BufferIdx] = stSCLDMACfg.u32Base_Y[BufferIdx] + (buffsize);
        }
    }
    else
    {
        for(BufferIdx=0; BufferIdx<stCfg.u16BufNumber ; BufferIdx++)
        {
            stSCLDMACfg.u32Base_Y[BufferIdx] = stMsysMemInfo.u32PhyAddr + (buffsize*3/2*BufferIdx);
            stSCLDMACfg.u32Base_C[BufferIdx] = stSCLDMACfg.u32Base_Y[BufferIdx] + (buffsize);
        }
    }

    if(bin)
    {
        stSCLDMACfg.enMemType   = E_DRV_SCLDMA_IO_MEM_FRM;
        stSCLDMACfg = FILL_VERCHK_TYPE(stSCLDMACfg, stSCLDMACfg.VerChk_Version, stSCLDMACfg.VerChk_Size,DRV_SCLDMA_VERSION);
        DrvSclDmaIoSetInBufferConfig(g_FD[enType], &stSCLDMACfg);

    }
    else
    {
        stSCLDMACfg = FILL_VERCHK_TYPE(stSCLDMACfg, stSCLDMACfg.VerChk_Version, stSCLDMACfg.VerChk_Size,DRV_SCLDMA_VERSION);
        DrvSclDmaIoSetOutBufferConfig(g_FD[enType], &stSCLDMACfg);
    }
    return stSCLDMACfg;
}
FPGA_CONFIG _FPGACfgTran2FPGASC2(FPGASC2_CONFIG stCfg ,unsigned char bSc2)
{
    FPGA_CONFIG stFPGACfg;
    if(bSc2)
    {
        stFPGACfg.u16Src_Width   = stCfg.u16sc1out_Width;
        stFPGACfg.u16Src_Height  = stCfg.u16sc1out_Height;
        stFPGACfg.u16Dsp_Width   = stCfg.u16Dsp_Width;
        stFPGACfg.u16Dsp_Height  = stCfg.u16Dsp_Height;
        stFPGACfg.enBufferMd     = stCfg.enBufferMdsc2;
        stFPGACfg.enColor        = stCfg.enColorsc2;
        stFPGACfg.enMem          = stCfg.enMemsc2;
    }
    else
    {
        stFPGACfg.u16Src_Width   = stCfg.u16Src_Width;
        stFPGACfg.u16Src_Height  = stCfg.u16Src_Height;
        stFPGACfg.u16Dsp_Width   = stCfg.u16sc1out_Width;
        stFPGACfg.u16Dsp_Height  = stCfg.u16sc1out_Height;
        stFPGACfg.enBufferMd     = stCfg.enBufferMd;
        stFPGACfg.enColor        = stCfg.enColor;
        stFPGACfg.enMem          = stCfg.enMem;
    }
    stFPGACfg.u16BufNumber   = stCfg.u16BufNumber;
    stFPGACfg.enSrc          = stCfg.enSrc;
    return stFPGACfg;
}
DrvSclDmaIoActiveBufferConfig_t _SetRPoint(DrvIdType_e enType, DrvSclDmaIoMemType_e enMemType, u8 u8RPoint)
{
    DrvSclDmaIoActiveBufferConfig_t stSCLDMABFCfg;
    stSCLDMABFCfg.u8ActiveBuffer=(E_DRV_SCLDMA_IO_ACTIVE_BUFFER_OMX|u8RPoint);
    stSCLDMABFCfg.enMemType = enMemType;
    stSCLDMABFCfg = FILL_VERCHK_TYPE(stSCLDMABFCfg, stSCLDMABFCfg.VerChk_Version,
        stSCLDMABFCfg.VerChk_Size,DRV_SCLDMA_VERSION);

    DrvSclDmaIoGetOutActiveBufferConfig(g_FD[enType], &stSCLDMABFCfg);
    return stSCLDMABFCfg;
}

DrvSclDmaIoActiveBufferConfig_t _GetRPoint(DrvIdType_e enType, DrvSclDmaIoMemType_e enMemType)
{
    DrvSclDmaIoActiveBufferConfig_t stSCLDMABFCfg;
    stSCLDMABFCfg.enMemType = enMemType;
    stSCLDMABFCfg = FILL_VERCHK_TYPE(stSCLDMABFCfg, stSCLDMABFCfg.VerChk_Version,
        stSCLDMABFCfg.VerChk_Size,DRV_SCLDMA_VERSION);

    DrvSclDmaIoGetOutActiveBufferConfig(g_FD[enType], &stSCLDMABFCfg);
    return stSCLDMABFCfg;
}
short Get_Poll(int *enId)
{
    DrvSclDmaIoPollConfig_t   stPoll;
    short bpoll = 0;
    //sclprintf("[FPGA] %d\n",*(enId));
    stPoll.pfnCb = NULL;
    stPoll.u32Timeout = 500;
    stPoll.u8pollval = POLLIN|POLLPRI|POLLERR;
    if(*enId == g_FD[E_DRV_ID_SCLDMA4])
    {
        stPoll.u8pollval |= 0x4;
    }
    stPoll.u8retval = 0;
    if(!DrvSclDmaIoPoll(*enId,&stPoll))
    {
        //sclprintf("[FPGA]POLL:%d\n",err);
        if(stPoll.u8retval & (0x1|0x2|0x8|0x4))
        {
            //sclprintf("[FPGA]Revent %d\n",stPoll.u8retval);
            bpoll = stPoll.u8retval;
        }
    }
    return bpoll;
}

void _SetDMATrig(DrvIdType_e enType,DrvSclDmaIoMemType_e enMemType,bool bEn)
{
    DrvSclDmaIoTriggerConfig_t  stSCLDMATrigCfg;
    stSCLDMATrigCfg.bEn = bEn;
    stSCLDMATrigCfg.enMemType = enMemType;
    stSCLDMATrigCfg = FILL_VERCHK_TYPE(stSCLDMATrigCfg, stSCLDMATrigCfg.VerChk_Version,
        stSCLDMATrigCfg.VerChk_Size,DRV_SCLDMA_VERSION);

    DrvSclDmaIoSetOutTriggerConfig(g_FD[enType], &stSCLDMATrigCfg);
}

void _SetDMAinTrig(DrvIdType_e enType, DrvSclDmaIoMemType_e enMemType, bool bEn)
{
    DrvSclDmaIoTriggerConfig_t stSCLDMATrigCfg;
    stSCLDMATrigCfg.bEn = bEn;
    stSCLDMATrigCfg.enMemType = enMemType;
    stSCLDMATrigCfg = FILL_VERCHK_TYPE(stSCLDMATrigCfg, stSCLDMATrigCfg.VerChk_Version,
        stSCLDMATrigCfg.VerChk_Size,DRV_SCLDMA_VERSION);
    DrvSclDmaIoSetInTriggerConfig(g_FD[enType], &stSCLDMATrigCfg);
}

static void emptyParser(vm_msg_t *pMessage)
{
    sclprintf("%s %d\n", __FUNCTION__, __LINE__);
}

static void emptyInit(void *userdata)
{
    sclprintf("%s %d\n", __FUNCTION__, __LINE__);

}

static void MonitorFunction(void *arg)
{
    char pst[64];
    int* pchoice = NULL;
    while(1)
    {
        memset(pst,0,64);
        CamOsScanf("%s", pst);
        //choice = CliGetChar();
        pchoice = (int*)arg;
        *pchoice = pst[0];
        MsSleep(200);
        sclprintf("%s %d char:%s\n", __FUNCTION__, __LINE__,pchoice);
        if(((char)*pchoice) == '?')
        {
            DrvSclOsSetDbgTaskWork();
        }
    }
}

SclThreadConfig_t _CreateMonitorThread(Task_CB pfunc, u32 *p32Char)
{
    MsTaskId_e SclTaskid;
    static int u32count = 2;
    MsTaskCreateArgs_t SclTaskArgs[2] =
    {
        {9, 0, NULL, emptyInit, emptyParser, NULL, &SclTaskid , NULL, FALSE, "Mo0", 0, 0},
        {10, 0, NULL, emptyInit, emptyParser, NULL, &SclTaskid , NULL, FALSE, "Mo1", 0, 0},
    };

    SclThreadConfig_t stCfg;

    SclTaskArgs[0].AppliInit = pfunc;
    SclTaskArgs[0].pInitArgs = (void *)p32Char;
    SclTaskArgs[0].AppliParser = emptyParser;
    SclTaskArgs[0].StackSize = 4096;
    SclTaskArgs[0].pStackTop = (u32*)MsAllocateMem(SclTaskArgs[0].StackSize);
    if(u32count%2)
    {
        strcpy(SclTaskArgs[0].TaskName, "Mo0");
        u32count++;
    }
    else
    {
        strcpy(SclTaskArgs[0].TaskName, "Mo1");
        u32count++;
    }

    sclprintf("%s %d, Time=%d\n", __FUNCTION__, __LINE__, DrvSclOsGetSystemTime());

    MsCreateTask(&SclTaskArgs[0]);

    MsStartTask(SclTaskid);

    stCfg.TaskId = SclTaskid;
    stCfg.pStackTop = SclTaskArgs[0].pStackTop;

    return stCfg;
}

SclThreadConfig_t _CreateTestThread(Task_CB pfunc, SclDmaPollConfig_t *pstScldmaPollCfg)
{
    MsTaskId_e SclTaskid;
    static int u32count = 2;
    MsTaskCreateArgs_t SclTaskArgs[2] =
    {
        {11, 0, NULL, emptyInit, emptyParser, NULL, &SclTaskid , NULL, FALSE, "task0", 0, 0},
        {12, 0, NULL, emptyInit, emptyParser, NULL, &SclTaskid , NULL, FALSE, "task1", 0, 0},
    };

    SclThreadConfig_t stCfg;

    SclTaskArgs[0].AppliInit = pfunc;
    SclTaskArgs[0].pInitArgs = (void *)pstScldmaPollCfg;
    SclTaskArgs[0].AppliParser = emptyParser;
    SclTaskArgs[0].StackSize = 4096;
    SclTaskArgs[0].pStackTop = (u32*)MsAllocateMem(SclTaskArgs[0].StackSize);
    if(u32count%2)
    {
        strcpy(SclTaskArgs[0].TaskName, "task0");
        u32count++;
    }
    else
    {
        strcpy(SclTaskArgs[0].TaskName, "task1");
        u32count++;
    }

    sclprintf("%s %d, Time=%d\n", __FUNCTION__, __LINE__, DrvSclOsGetSystemTime());

    MsCreateTask(&SclTaskArgs[0]);

    MsStartTask(SclTaskid);

    stCfg.TaskId = SclTaskid;
    stCfg.pStackTop = SclTaskArgs[0].pStackTop;

    return stCfg;
}

void _DeleteTask(MsTaskId_e eTaskId, void *pStak)
{
    MsDeleteTask(eTaskId);
    MsReleaseMemory(pStak);
}

/*============================================================================*/

static void _scl_poll_Thread(void *userdata)
{
    SclDmaPollConfig_t *pstScldmaPollCfg = (SclDmaPollConfig_t *)userdata;
    u16 i = 0;

    sclprintf("%s %d\n", __FUNCTION__, __LINE__);

    while(1)
    {
        sclprintf(" i=%x, Time%d, %d \n", i++, DrvSclOsGetSystemTime(), pstScldmaPollCfg->bEn);
        DrvSclRegisterWrite(0x121802, i);

        if(i == 5)
            break;
    }
    sclprintf("%s %d\n", __FUNCTION__, __LINE__);

}
void _Switch_Scldma_2_Single_Buf(ScldmaSwapBufferConfig_t *pCfg, int bufidx)
{
    DrvSclDmaIoBufferConfig_t stSCLDMABufCfg;
    sclprintf("%s enter\n", __FUNCTION__);
    stSCLDMABufCfg.enBufMDType    = E_DRV_SCLDMA_IO_BUFFER_MD_SINGLE;
    stSCLDMABufCfg.enColorType    = pCfg->enColorsc2;
    stSCLDMABufCfg.enMemType      = pCfg->enMemsc2;
    stSCLDMABufCfg.u16BufNum      = 1;
    stSCLDMABufCfg.u16Width       = pCfg->u16Width2;
    stSCLDMABufCfg.u16Height      = pCfg->u16Height2;
    stSCLDMABufCfg.bvFlag.btsBase_0 = 1;
    stSCLDMABufCfg.bvFlag.btsBase_1 = 0;
    stSCLDMABufCfg.bvFlag.btsBase_2 = 0;
    stSCLDMABufCfg.bvFlag.btsBase_3 = 0;


    if(bufidx == 0)
    {
        stSCLDMABufCfg.u32Base_Y[0]   = pCfg->u32Base_Y2[1];
        if (stSCLDMABufCfg.enColorType    == E_DRV_SCLDMA_IO_COLOR_YUV422)
            stSCLDMABufCfg.u32Base_C[0]   = pCfg->u32Base_Y2[1]+16;
        else
            stSCLDMABufCfg.u32Base_C[0]   = pCfg->u32Base_C2[1];
        stSCLDMABufCfg.enMemType      = pCfg->enMemsc2;
        stSCLDMABufCfg = FILL_VERCHK_TYPE(stSCLDMABufCfg, stSCLDMABufCfg.VerChk_Version, stSCLDMABufCfg.VerChk_Size,DRV_SCLDMA_VERSION);
        DrvSclDmaIoSetOutBufferConfig(g_FD[E_DRV_ID_SCLDMA2], &stSCLDMABufCfg);

    }
    else
    {
        stSCLDMABufCfg.u32Base_Y[0]   = pCfg->u32Base_Y2[0];
        if (stSCLDMABufCfg.enColorType    == E_DRV_SCLDMA_IO_COLOR_YUV422)
                stSCLDMABufCfg.u32Base_C[0]   = pCfg->u32Base_Y2[0]+16;
        else
                stSCLDMABufCfg.u32Base_C[0]   = pCfg->u32Base_C2[0];
        stSCLDMABufCfg.enMemType      = pCfg->enMemsc2;
        stSCLDMABufCfg = FILL_VERCHK_TYPE(stSCLDMABufCfg, stSCLDMABufCfg.VerChk_Version, stSCLDMABufCfg.VerChk_Size,DRV_SCLDMA_VERSION);
        DrvSclDmaIoSetOutBufferConfig(g_FD[E_DRV_ID_SCLDMA2], &stSCLDMABufCfg);
    }
    //sclprintf("[FPGA]pCfg->u32Base_Y[0]:%lx  pCfg->u32Base_Y[1]:%lx",pCfg->u32Base_Y[0],pCfg->u32Base_Y[1]);

    // trig SCLDMA2
    stSCLDMABufCfg.enMemType      = pCfg->enMemsc2;
    _SetDMATrig(E_DRV_ID_SCLDMA2,stSCLDMABufCfg.enMemType,1);
}

void _Switch_Scldma_2_4_Single_Buf(ScldmaSwapBufferConfig_t *pCfg, int bufidx)
{
    DrvSclDmaIoBufferConfig_t stSCLDMABufCfg;
    stSCLDMABufCfg.enBufMDType    = E_DRV_SCLDMA_IO_BUFFER_MD_SINGLE;
    stSCLDMABufCfg.enColorType    = pCfg->enColorsc2;
    stSCLDMABufCfg.enMemType      = pCfg->enMemsc2;
    stSCLDMABufCfg.u16BufNum      = 1;
    stSCLDMABufCfg.u16Width       = pCfg->u16Width2;
    stSCLDMABufCfg.u16Height      = pCfg->u16Height2;
    stSCLDMABufCfg.bvFlag.btsBase_0 = 1;
    stSCLDMABufCfg.bvFlag.btsBase_1 = 0;
    stSCLDMABufCfg.bvFlag.btsBase_2 = 0;
    stSCLDMABufCfg.bvFlag.btsBase_3 = 0;


    if(bufidx == 0)
    {
        stSCLDMABufCfg.u32Base_Y[0]   = pCfg->u32Base_Y2[1];
        if (stSCLDMABufCfg.enColorType    == E_DRV_SCLDMA_IO_COLOR_YUV422)
            stSCLDMABufCfg.u32Base_C[0]   = pCfg->u32Base_Y2[1]+16;
        else
            stSCLDMABufCfg.u32Base_C[0]   = pCfg->u32Base_C2[1];
        stSCLDMABufCfg.enMemType      = pCfg->enMemsc2;
        stSCLDMABufCfg = FILL_VERCHK_TYPE(stSCLDMABufCfg, stSCLDMABufCfg.VerChk_Version, stSCLDMABufCfg.VerChk_Size,DRV_SCLDMA_VERSION);
        DrvSclDmaIoSetOutBufferConfig(g_FD[E_DRV_ID_SCLDMA2], &stSCLDMABufCfg);

        stSCLDMABufCfg.u32Base_Y[0]   = pCfg->u32Base_Y2[0];
        if (stSCLDMABufCfg.enColorType    == E_DRV_SCLDMA_IO_COLOR_YUV422)
            stSCLDMABufCfg.u32Base_C[0]   = pCfg->u32Base_Y2[0]+16;
        else
            stSCLDMABufCfg.u32Base_C[0]   = pCfg->u32Base_C2[0];
        stSCLDMABufCfg.enMemType      = E_DRV_SCLDMA_IO_MEM_FRM;
        stSCLDMABufCfg = FILL_VERCHK_TYPE(stSCLDMABufCfg, stSCLDMABufCfg.VerChk_Version, stSCLDMABufCfg.VerChk_Size,DRV_SCLDMA_VERSION);
        DrvSclDmaIoSetInBufferConfig(g_FD[E_DRV_ID_SCLDMA4], &stSCLDMABufCfg);

    }
    else
    {
        stSCLDMABufCfg.u32Base_Y[0]   = pCfg->u32Base_Y2[0];
        if (stSCLDMABufCfg.enColorType    == E_DRV_SCLDMA_IO_COLOR_YUV422)
                stSCLDMABufCfg.u32Base_C[0]   = pCfg->u32Base_Y2[0]+16;
        else
                stSCLDMABufCfg.u32Base_C[0]   = pCfg->u32Base_C2[0];
        stSCLDMABufCfg.enMemType      = pCfg->enMemsc2;
        stSCLDMABufCfg = FILL_VERCHK_TYPE(stSCLDMABufCfg, stSCLDMABufCfg.VerChk_Version, stSCLDMABufCfg.VerChk_Size,DRV_SCLDMA_VERSION);
        DrvSclDmaIoSetOutBufferConfig(g_FD[E_DRV_ID_SCLDMA2], &stSCLDMABufCfg);


        stSCLDMABufCfg.u32Base_Y[0]   = pCfg->u32Base_Y2[1];
        if (stSCLDMABufCfg.enColorType    == E_DRV_SCLDMA_IO_COLOR_YUV422)
                stSCLDMABufCfg.u32Base_C[0]   = pCfg->u32Base_Y2[1]+16;
        else
                stSCLDMABufCfg.u32Base_C[0]   = pCfg->u32Base_C2[1];

        stSCLDMABufCfg.enMemType      = E_DRV_SCLDMA_IO_MEM_FRM;
        stSCLDMABufCfg = FILL_VERCHK_TYPE(stSCLDMABufCfg, stSCLDMABufCfg.VerChk_Version, stSCLDMABufCfg.VerChk_Size,DRV_SCLDMA_VERSION);
        DrvSclDmaIoSetInBufferConfig(g_FD[E_DRV_ID_SCLDMA4], &stSCLDMABufCfg);
    }
    //sclprintf("[FPGA]pCfg->u32Base_Y[0]:%lx  pCfg->u32Base_Y[1]:%lx",pCfg->u32Base_Y[0],pCfg->u32Base_Y[1]);

    // trig SCLDMA2
    stSCLDMABufCfg.enMemType      = pCfg->enMemsc2;
    _SetDMATrig(E_DRV_ID_SCLDMA2,stSCLDMABufCfg.enMemType,1);
    // trig SCLDMA4
    //_SetDMAinTrig(E_DRV_ID_SCLDMA4,E_DRV_SCLDMA_IO_MEM_FRM,1);
}

void _Switch_Scldma_1_4_Single_Buf(ScldmaSwapBufferConfig_t *pCfg, int bufidx)
{
    DrvSclDmaIoBufferConfig_t stSCLDMABufCfg;
    //DrvSclDmaIoTriggerConfig_t stSCLDMATrigCfg;
    //sclprintf("%s enter\n", __FUNCTION__);
    stSCLDMABufCfg.enBufMDType    = E_DRV_SCLDMA_IO_BUFFER_MD_SINGLE;
    stSCLDMABufCfg.enColorType    = pCfg->enColor;
    stSCLDMABufCfg.enMemType      = pCfg->enMem;
    stSCLDMABufCfg.u16BufNum      = 1;
    stSCLDMABufCfg.u16Width       = pCfg->u16Width;
    stSCLDMABufCfg.u16Height      = pCfg->u16Height;
    stSCLDMABufCfg.bvFlag.btsBase_0 = 1;
    stSCLDMABufCfg.bvFlag.btsBase_1 = 0;
    stSCLDMABufCfg.bvFlag.btsBase_2 = 0;
    stSCLDMABufCfg.bvFlag.btsBase_3 = 0;


    if(bufidx == 0)
    {
        stSCLDMABufCfg.u32Base_Y[0]   = pCfg->u32Base_Y[1];
        if (stSCLDMABufCfg.enColorType    == E_DRV_SCLDMA_IO_COLOR_YUV422)
            stSCLDMABufCfg.u32Base_C[0]   = pCfg->u32Base_Y[1]+16;
        else
            stSCLDMABufCfg.u32Base_C[0]   = pCfg->u32Base_C[1];
        stSCLDMABufCfg.enMemType      = pCfg->enMem;
        stSCLDMABufCfg = FILL_VERCHK_TYPE(stSCLDMABufCfg, stSCLDMABufCfg.VerChk_Version, stSCLDMABufCfg.VerChk_Size,DRV_SCLDMA_VERSION);
        DrvSclDmaIoSetOutBufferConfig(g_FD[E_DRV_ID_SCLDMA1], &stSCLDMABufCfg);

        stSCLDMABufCfg.u32Base_Y[0]   = pCfg->u32Base_Y[0];
        if (stSCLDMABufCfg.enColorType    == E_DRV_SCLDMA_IO_COLOR_YUV422)
            stSCLDMABufCfg.u32Base_C[0]   = pCfg->u32Base_Y[0]+16;
        else
            stSCLDMABufCfg.u32Base_C[0]   = pCfg->u32Base_C[0];

        stSCLDMABufCfg.enMemType      = E_DRV_SCLDMA_IO_MEM_FRM;
        stSCLDMABufCfg = FILL_VERCHK_TYPE(stSCLDMABufCfg, stSCLDMABufCfg.VerChk_Version, stSCLDMABufCfg.VerChk_Size,DRV_SCLDMA_VERSION);
        DrvSclDmaIoSetInBufferConfig(g_FD[E_DRV_ID_SCLDMA4], &stSCLDMABufCfg);
    }
    else
    {
        stSCLDMABufCfg.u32Base_Y[0]   = pCfg->u32Base_Y[0];
        if (stSCLDMABufCfg.enColorType    == E_DRV_SCLDMA_IO_COLOR_YUV422)
                stSCLDMABufCfg.u32Base_C[0]   = pCfg->u32Base_Y[0]+16;
        else
                stSCLDMABufCfg.u32Base_C[0]   = pCfg->u32Base_C[0];

        stSCLDMABufCfg.enMemType      = pCfg->enMem;
        stSCLDMABufCfg = FILL_VERCHK_TYPE(stSCLDMABufCfg, stSCLDMABufCfg.VerChk_Version, stSCLDMABufCfg.VerChk_Size,DRV_SCLDMA_VERSION);
        DrvSclDmaIoSetOutBufferConfig(g_FD[E_DRV_ID_SCLDMA1], &stSCLDMABufCfg);

        stSCLDMABufCfg.u32Base_Y[0]   = pCfg->u32Base_Y[1];
        if (stSCLDMABufCfg.enColorType    == E_DRV_SCLDMA_IO_COLOR_YUV422)
                stSCLDMABufCfg.u32Base_C[0]   = pCfg->u32Base_Y[1]+16;
        else
                stSCLDMABufCfg.u32Base_C[0]   = pCfg->u32Base_C[1];

        stSCLDMABufCfg.enMemType      = E_DRV_SCLDMA_IO_MEM_FRM;
        stSCLDMABufCfg = FILL_VERCHK_TYPE(stSCLDMABufCfg, stSCLDMABufCfg.VerChk_Version, stSCLDMABufCfg.VerChk_Size,DRV_SCLDMA_VERSION);
        DrvSclDmaIoSetInBufferConfig(g_FD[E_DRV_ID_SCLDMA4], &stSCLDMABufCfg);
    }
    //sclprintf("[FPGA]pCfg->u32Base_Y[0]:%lx  pCfg->u32Base_Y[1]:%lx",pCfg->u32Base_Y[0],pCfg->u32Base_Y[1]);

    // trig SCLDMA1
    stSCLDMABufCfg.enMemType      = pCfg->enMem;
    _SetDMATrig(E_DRV_ID_SCLDMA1,pCfg->enMem,1);
    // trig SCLDMA4
    //_SetDMAinTrig(E_DRV_ID_SCLDMA4,E_DRV_SCLDMA_IO_MEM_FRM,1);
}

void _Switch_Scldma_1_Single_Buf(ScldmaSwapBufferConfig_t *pCfg, int bufidx)
{
    DrvSclDmaIoBufferConfig_t stSCLDMABufCfg;
    //DrvSclDmaIoTriggerConfig_t stSCLDMATrigCfg;
    sclprintf("%s enter\n", __FUNCTION__);
    stSCLDMABufCfg.enBufMDType    = E_DRV_SCLDMA_IO_BUFFER_MD_SINGLE;
    stSCLDMABufCfg.enColorType    = pCfg->enColor;
    stSCLDMABufCfg.enMemType      = pCfg->enMem;
    stSCLDMABufCfg.u16BufNum      = 1;
    stSCLDMABufCfg.u16Width       = pCfg->u16Width;
    stSCLDMABufCfg.u16Height      = pCfg->u16Height;
    stSCLDMABufCfg.bvFlag.btsBase_0 = 1;
    stSCLDMABufCfg.bvFlag.btsBase_1 = 0;
    stSCLDMABufCfg.bvFlag.btsBase_2 = 0;
    stSCLDMABufCfg.bvFlag.btsBase_3 = 0;


    if(bufidx == 0)
    {
        stSCLDMABufCfg.u32Base_Y[0]   = pCfg->u32Base_Y[1];
        if (stSCLDMABufCfg.enColorType    == E_DRV_SCLDMA_IO_COLOR_YUV422)
            stSCLDMABufCfg.u32Base_C[0]   = pCfg->u32Base_Y[1]+16;
        else
            stSCLDMABufCfg.u32Base_C[0]   = pCfg->u32Base_C[1];
        stSCLDMABufCfg.enMemType      = pCfg->enMem;
        stSCLDMABufCfg = FILL_VERCHK_TYPE(stSCLDMABufCfg, stSCLDMABufCfg.VerChk_Version, stSCLDMABufCfg.VerChk_Size,DRV_SCLDMA_VERSION);
        DrvSclDmaIoSetOutBufferConfig(g_FD[E_DRV_ID_SCLDMA1], &stSCLDMABufCfg);

    }
    else
    {
        stSCLDMABufCfg.u32Base_Y[0]   = pCfg->u32Base_Y[0];
        if (stSCLDMABufCfg.enColorType    == E_DRV_SCLDMA_IO_COLOR_YUV422)
                stSCLDMABufCfg.u32Base_C[0]   = pCfg->u32Base_Y[0]+16;
        else
                stSCLDMABufCfg.u32Base_C[0]   = pCfg->u32Base_C[0];

        stSCLDMABufCfg.enMemType      = pCfg->enMem;
        stSCLDMABufCfg = FILL_VERCHK_TYPE(stSCLDMABufCfg, stSCLDMABufCfg.VerChk_Version, stSCLDMABufCfg.VerChk_Size,DRV_SCLDMA_VERSION);
        DrvSclDmaIoSetOutBufferConfig(g_FD[E_DRV_ID_SCLDMA1], &stSCLDMABufCfg);

    }
    stSCLDMABufCfg.enMemType      = pCfg->enMem;
    _SetDMATrig(E_DRV_ID_SCLDMA1,pCfg->enMem,1);
}

void _Switch_Scldma_1_2_4_Single_Buf(ScldmaSwapBufferConfig_t *pCfg, int bufidx)
{
    DrvSclDmaIoBufferConfig_t stSCLDMABufCfg,stSCLDMABufCfg2;

    stSCLDMABufCfg.enBufMDType    = E_DRV_SCLDMA_IO_BUFFER_MD_SINGLE;
    stSCLDMABufCfg.enColorType    = pCfg->enColor;
    stSCLDMABufCfg.enMemType      = pCfg->enMem;
    stSCLDMABufCfg.u16BufNum      = 1;
    stSCLDMABufCfg.u16Width       = pCfg->u16Width;
    stSCLDMABufCfg.u16Height      = pCfg->u16Height;
    stSCLDMABufCfg.bvFlag.btsBase_0 = 1;
    stSCLDMABufCfg.bvFlag.btsBase_1 = 0;
    stSCLDMABufCfg.bvFlag.btsBase_2 = 0;
    stSCLDMABufCfg.bvFlag.btsBase_3 = 0;

    stSCLDMABufCfg2.enBufMDType    = E_DRV_SCLDMA_IO_BUFFER_MD_SINGLE;
    stSCLDMABufCfg2.enColorType    = pCfg->enColorsc2;
    stSCLDMABufCfg2.enMemType      = pCfg->enMemsc2;
    stSCLDMABufCfg2.u16BufNum      = 1;
    stSCLDMABufCfg2.u16Width       = pCfg->u16Width2;
    stSCLDMABufCfg2.u16Height      = pCfg->u16Height2;
    stSCLDMABufCfg2.bvFlag.btsBase_0 = 1;
    stSCLDMABufCfg2.bvFlag.btsBase_1 = 0;
    stSCLDMABufCfg2.bvFlag.btsBase_2 = 0;
    stSCLDMABufCfg2.bvFlag.btsBase_3 = 0;

    if(bufidx == 0)//sc1
    {
        stSCLDMABufCfg.u32Base_Y[0]   = pCfg->u32Base_Y[1];
        if (stSCLDMABufCfg.enColorType    == E_DRV_SCLDMA_IO_COLOR_YUV422)
            stSCLDMABufCfg.u32Base_C[0]   = pCfg->u32Base_Y[1]+16;
        else
            stSCLDMABufCfg.u32Base_C[0]   = pCfg->u32Base_C[1];
        stSCLDMABufCfg.enMemType      = pCfg->enMem;
        stSCLDMABufCfg = FILL_VERCHK_TYPE(stSCLDMABufCfg, stSCLDMABufCfg.VerChk_Version, stSCLDMABufCfg.VerChk_Size,DRV_SCLDMA_VERSION);
        DrvSclDmaIoSetOutBufferConfig(g_FD[E_DRV_ID_SCLDMA1], &stSCLDMABufCfg);


        stSCLDMABufCfg.u32Base_Y[0]   = pCfg->u32Base_Y[0];
        if (stSCLDMABufCfg.enColorType    == E_DRV_SCLDMA_IO_COLOR_YUV422)
            stSCLDMABufCfg.u32Base_C[0]   = pCfg->u32Base_Y[0]+16;
        else
            stSCLDMABufCfg.u32Base_C[0]   = pCfg->u32Base_C[0];
        stSCLDMABufCfg.enMemType      = E_DRV_SCLDMA_IO_MEM_FRM;
        stSCLDMABufCfg = FILL_VERCHK_TYPE(stSCLDMABufCfg, stSCLDMABufCfg.VerChk_Version, stSCLDMABufCfg.VerChk_Size,DRV_SCLDMA_VERSION);
        DrvSclDmaIoSetInBufferConfig(g_FD[E_DRV_ID_SCLDMA4], &stSCLDMABufCfg);
        _FPGA_Pnl(stSCLDMABufCfg.u16Width,stSCLDMABufCfg.u16Height);
    }
    else if(bufidx == 1)//sc2
    {
        stSCLDMABufCfg2.u32Base_Y[0]   = pCfg->u32Base_Y2[1];
        if (stSCLDMABufCfg.enColorType    == E_DRV_SCLDMA_IO_COLOR_YUV422)
                stSCLDMABufCfg2.u32Base_C[0]   = pCfg->u32Base_Y2[1]+16;
        else
                stSCLDMABufCfg2.u32Base_C[0]   = pCfg->u32Base_C2[1];
        stSCLDMABufCfg = FILL_VERCHK_TYPE(stSCLDMABufCfg, stSCLDMABufCfg.VerChk_Version, stSCLDMABufCfg.VerChk_Size,DRV_SCLDMA_VERSION);
        DrvSclDmaIoSetOutBufferConfig(g_FD[E_DRV_ID_SCLDMA2], &stSCLDMABufCfg);
        stSCLDMABufCfg2.enMemType      = pCfg->enMemsc2;

        stSCLDMABufCfg2.u32Base_Y[0]   = pCfg->u32Base_Y2[0];
        if (stSCLDMABufCfg2.enColorType    == E_DRV_SCLDMA_IO_COLOR_YUV422)
                stSCLDMABufCfg2.u32Base_C[0]   = pCfg->u32Base_Y2[0]+16;
        else
                stSCLDMABufCfg2.u32Base_C[0]   = pCfg->u32Base_C2[0];
        stSCLDMABufCfg.enMemType      = E_DRV_SCLDMA_IO_MEM_FRM;
        stSCLDMABufCfg = FILL_VERCHK_TYPE(stSCLDMABufCfg, stSCLDMABufCfg.VerChk_Version, stSCLDMABufCfg.VerChk_Size,DRV_SCLDMA_VERSION);
        DrvSclDmaIoSetInBufferConfig(g_FD[E_DRV_ID_SCLDMA4], &stSCLDMABufCfg);
        _FPGA_Pnl(stSCLDMABufCfg2.u16Width,stSCLDMABufCfg2.u16Height);
    }
    else if(bufidx == 2)//sc1
    {
        stSCLDMABufCfg.u32Base_Y[0]   = pCfg->u32Base_Y[0];
        if (stSCLDMABufCfg.enColorType    == E_DRV_SCLDMA_IO_COLOR_YUV422)
                stSCLDMABufCfg.u32Base_C[0]   = pCfg->u32Base_Y[0]+16;
        else
                stSCLDMABufCfg.u32Base_C[0]   = pCfg->u32Base_C[0];
        stSCLDMABufCfg = FILL_VERCHK_TYPE(stSCLDMABufCfg, stSCLDMABufCfg.VerChk_Version, stSCLDMABufCfg.VerChk_Size,DRV_SCLDMA_VERSION);
        DrvSclDmaIoSetOutBufferConfig(g_FD[E_DRV_ID_SCLDMA1], &stSCLDMABufCfg);
        stSCLDMABufCfg.enMemType      = pCfg->enMem;

        stSCLDMABufCfg.u32Base_Y[0]   = pCfg->u32Base_Y[1];
        if (stSCLDMABufCfg.enColorType    == E_DRV_SCLDMA_IO_COLOR_YUV422)
                stSCLDMABufCfg.u32Base_C[0]   = pCfg->u32Base_Y[1]+16;
        else
                stSCLDMABufCfg.u32Base_C[0]   = pCfg->u32Base_C[1];
        stSCLDMABufCfg.enMemType      = E_DRV_SCLDMA_IO_MEM_FRM;
        stSCLDMABufCfg = FILL_VERCHK_TYPE(stSCLDMABufCfg, stSCLDMABufCfg.VerChk_Version, stSCLDMABufCfg.VerChk_Size,DRV_SCLDMA_VERSION);
        DrvSclDmaIoSetInBufferConfig(g_FD[E_DRV_ID_SCLDMA4], &stSCLDMABufCfg);
        _FPGA_Pnl(stSCLDMABufCfg.u16Width,stSCLDMABufCfg.u16Height);
    }
    else if(bufidx == 3)//sc2
    {
        stSCLDMABufCfg2.u32Base_Y[0]   = pCfg->u32Base_Y2[0];
        if (stSCLDMABufCfg2.enColorType    == E_DRV_SCLDMA_IO_COLOR_YUV422)
                stSCLDMABufCfg2.u32Base_C[0]   = pCfg->u32Base_Y2[0]+16;
        else
                stSCLDMABufCfg2.u32Base_C[0]   = pCfg->u32Base_C2[0];
        stSCLDMABufCfg2 = FILL_VERCHK_TYPE(stSCLDMABufCfg2, stSCLDMABufCfg2.VerChk_Version, stSCLDMABufCfg2.VerChk_Size,DRV_SCLDMA_VERSION);
        DrvSclDmaIoSetOutBufferConfig(g_FD[E_DRV_ID_SCLDMA2], &stSCLDMABufCfg);
        stSCLDMABufCfg2.enMemType      = pCfg->enMemsc2;

        stSCLDMABufCfg2.u32Base_Y[0]   = pCfg->u32Base_Y2[1];
        if (stSCLDMABufCfg2.enColorType    == E_DRV_SCLDMA_IO_COLOR_YUV422)
                stSCLDMABufCfg2.u32Base_C[0]   = pCfg->u32Base_Y2[1]+16;
        else
                stSCLDMABufCfg2.u32Base_C[0]   = pCfg->u32Base_C2[1];
        stSCLDMABufCfg.enMemType      = E_DRV_SCLDMA_IO_MEM_FRM;
        stSCLDMABufCfg2 = FILL_VERCHK_TYPE(stSCLDMABufCfg2, stSCLDMABufCfg2.VerChk_Version, stSCLDMABufCfg2.VerChk_Size,DRV_SCLDMA_VERSION);
        DrvSclDmaIoSetInBufferConfig(g_FD[E_DRV_ID_SCLDMA4], &stSCLDMABufCfg);
        _FPGA_Pnl(stSCLDMABufCfg2.u16Width,stSCLDMABufCfg2.u16Height);
    }
    //sclprintf("[FPGA]pCfg->u32Base_Y[0]:%lx  pCfg->u32Base_Y[1]:%lx",pCfg->u32Base_Y[0],pCfg->u32Base_Y[1]);

    // trig SCLDMA1
    stSCLDMABufCfg.enMemType      = pCfg->enMem;
    _SetDMATrig(E_DRV_ID_SCLDMA1,stSCLDMABufCfg.enMemType,1);
    // trig SCLDMA2
    stSCLDMABufCfg2.enMemType      = pCfg->enMemsc2;
    _SetDMATrig(E_DRV_ID_SCLDMA2,stSCLDMABufCfg2.enMemType,1);
    // trig SCLDMA4
    _SetDMAinTrig(E_DRV_ID_SCLDMA4,E_DRV_SCLDMA_IO_MEM_FRM,1);
}
static void _ScldmaSC2_Poll_Thread_SingalBuf(void *arg)
{
    SclDmaPollConfig_t *pstScldmaPollCfg = (SclDmaPollConfig_t *)arg;
    int err_SCLDMA1 = 0;
    int err_SCLDMA2 = 0;
    int buffer_index = 0;
    EN_DRV_SC2_TYPE sc2type;
    int enID[2];
    short bpoll = 0;
    short bpoll2 = 0;
    sclprintf("%s enter\n", __FUNCTION__);
    sc2type=pstScldmaPollCfg->enSC2Md;

    while(pstScldmaPollCfg->bEn)
    {
        if(sc2type==E_DRV_SC1_SC2_DSP)
        {
            if(pstScldmaPollCfg->enBufferMd==E_DRV_SCLDMA_IO_BUFFER_MD_SINGLE || pstScldmaPollCfg->enBufferMdsc2==E_DRV_SCLDMA_IO_BUFFER_MD_SINGLE)
            {
                enID[0] = g_FD[E_DRV_ID_SCLDMA1];
                enID[1] = g_FD[E_DRV_ID_SCLDMA2];
                bpoll = Get_Poll(&enID[0]);
                bpoll2 = Get_Poll(&enID[1]);
                //sclprintf("[poll]:[0]:%hhx [1]:%hhx\n", stPoll_SCLDMA12[0].revents,stPoll_SCLDMA12[1].revents);
                if((bpoll&0x3))
                {
                    err_SCLDMA1 = (bpoll&0x3);
                }
                if(((bpoll2&0x3)))
                {
                    err_SCLDMA2=(bpoll2&0x3);
                }
            }
            else
            {
                bpoll =0x0;
                bpoll2 =0x0;
            }
        }
        else if(sc2type==E_DRV_SC1_DSP)
        {
            enID[0] = g_FD[E_DRV_ID_SCLDMA1];
            if(pstScldmaPollCfg->enBufferMd==E_DRV_SCLDMA_IO_BUFFER_MD_SINGLE)
                err_SCLDMA1 = Get_Poll(&enID[0]);
            else
                err_SCLDMA1 = POLLIN;
        }
        else if(sc2type==E_DRV_SC2_DSP)
        {
            enID[1] = g_FD[E_DRV_ID_SCLDMA2];
            if(pstScldmaPollCfg->enBufferMdsc2==E_DRV_SCLDMA_IO_BUFFER_MD_SINGLE)
                err_SCLDMA2 = Get_Poll(&enID[1]);
            else
                err_SCLDMA2=POLLIN;
        }
        //err_SCLDMA4 = poll(&stPoll_SCLDMA4, 1, 10);
        //sclprintf("[FPGA]stPoll_SCLDMA1.revents:%hx   stPoll_SCLDMA4.revent:%hx",stPoll_SCLDMA1.revents,stPoll_SCLDMA4.revents);

        if(((err_SCLDMA1 & (POLLIN|POLLPRI))&&(sc2type==E_DRV_SC1_DSP)) || ((err_SCLDMA2 & (POLLIN|POLLPRI))&&(sc2type==E_DRV_SC2_DSP))
            || ((sc2type==E_DRV_SC1_SC2_DSP)&&((err_SCLDMA2 & (POLLIN|POLLPRI))||(err_SCLDMA1 & (POLLIN|POLLPRI)))))
        {
            if(sc2type==E_DRV_SC1_SC2_DSP)
            {
                if(((err_SCLDMA1 & (POLLIN|POLLPRI))&&(err_SCLDMA2 & (POLLIN|POLLPRI))))
                {
                    MsSleep(10);
                    _Switch_Scldma_1_2_4_Single_Buf(&pstScldmaPollCfg->stSwapCfg, buffer_index);
                    MsSleep(1000);
                    buffer_index = (buffer_index+1)%4;
                    err_SCLDMA1 = 0;
                    err_SCLDMA2 = 0;
                }
                else
                {
                    sclprintf("[POLL]one ok\n");
                }
            }
            else if(sc2type==E_DRV_SC2_DSP)
            {
                MsSleep(50);
                _Switch_Scldma_1_Single_Buf(&pstScldmaPollCfg->stSwapCfg, buffer_index);
                _Switch_Scldma_2_4_Single_Buf(&pstScldmaPollCfg->stSwapCfg, buffer_index);
                buffer_index = (buffer_index+1)%2;
            }
            else if(sc2type==E_DRV_SC1_DSP)
            {
                MsSleep(50);
                _Switch_Scldma_2_Single_Buf(&pstScldmaPollCfg->stSwapCfg, buffer_index);
                _Switch_Scldma_1_4_Single_Buf(&pstScldmaPollCfg->stSwapCfg, buffer_index);
                buffer_index = (buffer_index+1)%2;
            }

        }
        else
        {
            //sleep(2);
        }
    }

    sclprintf("%s exit\n", __FUNCTION__);
}
static void _Scldma_Poll_Thread_GetQueue(void *arg)
{
    SclDmaPollConfig_t *pstScldmaPollCfg = (SclDmaPollConfig_t *)arg;
    DrvSclDmaIoBufferQueueConfig_t stSCLDMABFCfg;
    DrvSclDmaIoBufferConfig_t stSCLDMACfg;
    int buffsize;
    int idx = 0,jdx = 0,k = 0,leng = 0;
    int err_SCLDMA1;
    sclprintf("%s enter\n", __FUNCTION__);
    buffsize = pstScldmaPollCfg->stSwapCfg.u16Width* pstScldmaPollCfg->stSwapCfg.u16Height ; // 2bpp
    stSCLDMABFCfg.enMemType = pstScldmaPollCfg->stGetActCfg.enMem[jdx];
    stSCLDMABFCfg = FILL_VERCHK_TYPE(stSCLDMABFCfg, stSCLDMABFCfg.VerChk_Version, stSCLDMABFCfg.VerChk_Size,DRV_SCLDMA_VERSION);
    //printf("u8InQueueCount:%hhd enMemType:%d @:%llu\n",stSCLDMABFCfg.u8InQueueCount,stSCLDMABFCfg.enMemType,u64Time);
    //stSCLDMATrigCfg.bEn = 1;
    //stSCLDMATrigCfg.enMemType = E_IOCTL_SCLDMA_MEM_SNP;
    //stSCLDMATrigCfg = FILL_VERCHK_TYPE(stSCLDMATrigCfg, stSCLDMATrigCfg.VerChk_Version, stSCLDMATrigCfg.VerChk_Size,DRV_SCLDMA_VERSION);
    //ioctl(g_FD[E_DRV_ID_SCLDMA1], IOCTL_SCLDMA_SET_OUT_TRIGGER_CONFIG, &stSCLDMATrigCfg);
    while(pstScldmaPollCfg->bEn)
    {
        err_SCLDMA1 = Get_Poll(&g_FD[pstScldmaPollCfg->stGetActCfg.enID[0]]);
        //printf("%s POLLIN2  Fd:%d event:%d @:%lld\n", __FUNCTION__,PollFd[1].fd,PollFd[1].revents,u64Time);
        if(err_SCLDMA1& (POLLIN|POLLPRI))
        {
            if(err_SCLDMA1 & POLLPRI)
            {
                //printf("%s POLLIN  Fd:%d event:%x @:%llu\n", __FUNCTION__,PollFd.fd,PollFd.revents,u64Time);
                if(pstScldmaPollCfg->stGetActCfg.enID[0] == E_DRV_ID_SCLDMA1)
                {
                    _SetDMATrig(E_DRV_ID_SCLDMA1,E_DRV_SCLDMA_IO_MEM_SNP,1);
                }
                else if(pstScldmaPollCfg->stGetActCfg.enID[0] == E_DRV_ID_SCLDMA2)
                {
                    _SetDMATrig(E_DRV_ID_SCLDMA2,E_DRV_SCLDMA_IO_MEM_FRM2,1);
                }

            }
            stSCLDMABFCfg.enMemType = pstScldmaPollCfg->stGetActCfg.enMem[jdx];
            //stSCLDMABFCfg.enMemType = E_IOCTL_SCLDMA_MEM_FRM;
            stSCLDMABFCfg = FILL_VERCHK_TYPE(stSCLDMABFCfg, stSCLDMABFCfg.VerChk_Version, stSCLDMABFCfg.VerChk_Size,DRV_SCLDMA_VERSION);
            stSCLDMABFCfg.enUsedType = E_DRV_SCLDMA_IO_BUFFER_QUEUE_TYPE_PEEKQUEUE;
            DrvSclDmaIoBufferQueueHandleConfig(g_FD[pstScldmaPollCfg->stGetActCfg.enID[0]], &stSCLDMABFCfg);
            leng = stSCLDMABFCfg.u8InQueueCount;
            if(stSCLDMABFCfg.u8InQueueCount >0)
            {
                for(k=0 ;k<leng ;k++)
                {
                    stSCLDMABFCfg.enMemType = pstScldmaPollCfg->stGetActCfg.enMem[jdx];
                    //stSCLDMABFCfg.enMemType = E_IOCTL_SCLDMA_MEM_FRM;
                    stSCLDMABFCfg = FILL_VERCHK_TYPE
                        (stSCLDMABFCfg, stSCLDMABFCfg.VerChk_Version, stSCLDMABFCfg.VerChk_Size,DRV_SCLDMA_VERSION);
                    stSCLDMABFCfg.enUsedType = E_DRV_SCLDMA_IO_BUFFER_QUEUE_TYPE_DEQUEUE;
                    DrvSclDmaIoBufferQueueHandleConfig(g_FD[pstScldmaPollCfg->stGetActCfg.enID[0]], &stSCLDMABFCfg);

                    if(pstScldmaPollCfg->bswitchpnl)
                    {
                        sclprintf("[DeQueue]Fd :%d addr(%hhd):%lx, InQueue Count:%hhu ISP count:%hhd @%lld\n",
                            g_FD[pstScldmaPollCfg->stGetActCfg.enID[0]],stSCLDMABFCfg.stRead.u8FrameAddrIdx,
                            stSCLDMABFCfg.stRead.u32FrameAddr,stSCLDMABFCfg.u8InQueueCount,
                            stSCLDMABFCfg.stRead.u8ISPcount,stSCLDMABFCfg.stRead.u64FRMDoneTime);
                        stSCLDMACfg.enBufMDType = E_DRV_SCLDMA_IO_BUFFER_MD_RING;;
                        stSCLDMACfg.enMemType   = E_DRV_SCLDMA_IO_MEM_FRM;
                        stSCLDMACfg.enColorType = pstScldmaPollCfg->stSwapCfg.enColor;
                        stSCLDMACfg.u16Height   = pstScldmaPollCfg->stSwapCfg.u16Height;
                        stSCLDMACfg.u16Width    = pstScldmaPollCfg->stSwapCfg.u16Width;
                        stSCLDMACfg.u16BufNum    = 1;
                        stSCLDMACfg.bvFlag.btsBase_0 = stSCLDMACfg.u16BufNum >=1 ? 1 : 0;

                        if (stSCLDMACfg.enColorType    == E_DRV_SCLDMA_IO_COLOR_YUV422)
                        {
                            for(idx=0; idx<stSCLDMACfg.u16BufNum ; idx++)
                            {
                                stSCLDMACfg.u32Base_Y[idx] = stSCLDMABFCfg.stRead.u32FrameAddr;
                                stSCLDMACfg.u32Base_C[idx] = stSCLDMACfg.u32Base_Y[idx]+16;
                            }
                        }
                        else
                        {
                            for(idx=0; idx<stSCLDMACfg.u16BufNum; idx++)
                            {
                                stSCLDMACfg.u32Base_Y[idx] = stSCLDMABFCfg.stRead.u32FrameAddr;
                                stSCLDMACfg.u32Base_C[idx] = stSCLDMACfg.u32Base_Y[idx] + (buffsize);
                            }
                        }
                        stSCLDMACfg = FILL_VERCHK_TYPE(stSCLDMACfg, stSCLDMACfg.VerChk_Version, stSCLDMACfg.VerChk_Size,DRV_SCLDMA_VERSION);
                        DrvSclDmaIoSetInBufferConfig(g_FD[E_DRV_ID_SCLDMA4],&stSCLDMACfg);
                        _FPGA_Pnl(pstScldmaPollCfg->stSwapCfg.u16Width,pstScldmaPollCfg->stSwapCfg.u16Height);
                    }
                    stSCLDMABFCfg.enUsedType = E_DRV_SCLDMA_IO_BUFFER_QUEUE_TYPE_ENQUEUE;
                    stSCLDMABFCfg.u8EnqueueIdx = stSCLDMABFCfg.stRead.u8FrameAddrIdx;
                    DrvSclDmaIoBufferQueueHandleConfig(g_FD[pstScldmaPollCfg->stGetActCfg.enID[0]], &stSCLDMABFCfg);
                    if(pstScldmaPollCfg->bswitchpnl)
                    {
                        sclprintf("[EnQueue] Fd:%d (%hhd)\n",g_FD[pstScldmaPollCfg->stGetActCfg.enID[0]],stSCLDMABFCfg.u8EnqueueIdx);
                    }
                }
                //if(stSCLDMACfg.u32Base_Y[0])
                //printf("[Queue]set Addr SC4 Fd:%d (%lx,%lx)@%lld\n",PollFd.fd,stSCLDMACfg.u32Base_Y[0],stSCLDMACfg.u32Base_C[0],u64Time);

            }
        }
    }
}

static void _Scldma_Poll_Thread_SingalBuf(void *arg)
{
#if 1 //RTK_ToDo
    SclDmaPollConfig_t *pstScldmaPollCfg = (SclDmaPollConfig_t *)arg;
    int err_SCLDMA1;

    int buffer_index = 0;

    sclprintf("%s enter\n", __FUNCTION__);


    while(pstScldmaPollCfg->bEn)
    {
        MsSleep(20);
        err_SCLDMA1 = Get_Poll(&g_FD[pstScldmaPollCfg->stGetActCfg.enID[0]]);
        //sclprintf("[FPGA]revents:%hx   \n",err_SCLDMA1);
        if((err_SCLDMA1 & (POLLIN|POLLPRI)) )
        {
            if(pstScldmaPollCfg->stGetActCfg.enID[0] ==E_DRV_ID_SCLDMA1)
            {
                if(pstScldmaPollCfg->enSC2Md == E_DRV_SC1_DSP)
                {
                    _Switch_Scldma_1_4_Single_Buf(&pstScldmaPollCfg->stSwapCfg, buffer_index);
                }
                else
                {
                    _Switch_Scldma_1_Single_Buf(&pstScldmaPollCfg->stSwapCfg, buffer_index);
                }
                //sclprintf("[FPGA]1_4\n");
            }
            else if(pstScldmaPollCfg->stGetActCfg.enID[0] == E_DRV_ID_SCLDMA2)
            {
                if(pstScldmaPollCfg->enSC2Md == E_DRV_SC2_DSP)
                {
                    _Switch_Scldma_2_4_Single_Buf(&pstScldmaPollCfg->stSwapCfg, buffer_index);
                }
                else
                {
                    _Switch_Scldma_2_Single_Buf(&pstScldmaPollCfg->stSwapCfg, buffer_index);
                }
                //sclprintf("[FPGA]2_4\n");
            }
            buffer_index = (buffer_index+1)%2;
        }
        else
        {
            //sleep(2);
        }
    }
#endif

    sclprintf("%s exit\n", __FUNCTION__);
}

static void _Scldma_Poll_Thread_GetActive(void *arg)
{
    SclDmaPollConfig_t *pstScldmaPollCfg = (SclDmaPollConfig_t *)arg;
    DrvSclDmaIoActiveBufferConfig_t stSCLDMABFCfg;
    int i = 0;
    int err_SCLDMA1;
    u8 actid=0x5;
    sclprintf("%s enter\n", __FUNCTION__);

    while(1)
    {
        err_SCLDMA1 = Get_Poll(&g_FD[pstScldmaPollCfg->stGetActCfg.enID[0]]);
        err_SCLDMA1 = err_SCLDMA1;
        for(i=0;i<pstScldmaPollCfg->stGetActCfg.Count;i++)
        {
            stSCLDMABFCfg = _SetRPoint
                (pstScldmaPollCfg->stGetActCfg.enID[i],pstScldmaPollCfg->stGetActCfg.enMem[i],actid);

            if(stSCLDMABFCfg.u8ActiveBuffer&0x20)
            {
                actid=(stSCLDMABFCfg.u8ActiveBuffer&0xF);
                stSCLDMABFCfg = _SetRPoint
                    (pstScldmaPollCfg->stGetActCfg.enID[i],pstScldmaPollCfg->stGetActCfg.enMem[i],actid);
                actid=0x5;
                stSCLDMABFCfg = _SetRPoint
                    (pstScldmaPollCfg->stGetActCfg.enID[i],pstScldmaPollCfg->stGetActCfg.enMem[i],actid);
            }
            else
            {
                actid=0x5;
                stSCLDMABFCfg.u8ActiveBuffer=(0x10|actid);
            }
        }
        MsSleep(20);
        //sleep(1);
    }
    sclprintf("%s exit\n", __FUNCTION__);
}
static void _Scldma_Poll_Thread_GetActiveChangBuffer(void *arg)
{
    SclDmaPollConfig_t *pstScldmaPollCfg = (SclDmaPollConfig_t *)arg;
    DrvSclDmaIoActiveBufferConfig_t stSCLDMABFCfg;
    DrvSclDmaIoBufferConfig_t stSCLDMACfg;
    int i = 0;
    int idx = 0;
    int buffsize = 0;
    int err_SCLDMA1 = 0;
    u8 actid = 0;
    sclprintf("%s enter\n", __FUNCTION__);

    while(1)
    {
        buffsize = (pstScldmaPollCfg->stSwapCfg.u16Width* pstScldmaPollCfg->stSwapCfg.u16Height) ; // 2bpp
        err_SCLDMA1 = Get_Poll(&g_FD[pstScldmaPollCfg->stGetActCfg.enID[0]]);
        if(err_SCLDMA1& (POLLIN|POLLPRI|POLLERR))
        {
            for(i=0;i<pstScldmaPollCfg->stGetActCfg.Count;i++)
            {
                stSCLDMABFCfg = _GetRPoint
                    (pstScldmaPollCfg->stGetActCfg.enID[i],pstScldmaPollCfg->stGetActCfg.enMem[i]);
                actid = (stSCLDMABFCfg.u8ActiveBuffer&0xF);
                if(pstScldmaPollCfg->bswitchpnl &&(actid !=0xF))
                {
                    stSCLDMACfg.enBufMDType = E_DRV_SCLDMA_IO_BUFFER_MD_RING;;
                    stSCLDMACfg.enMemType   = E_DRV_SCLDMA_IO_MEM_FRM;
                    stSCLDMACfg.enColorType = pstScldmaPollCfg->stSwapCfg.enColor;
                    stSCLDMACfg.u16Height   = pstScldmaPollCfg->stSwapCfg.u16Height;
                    stSCLDMACfg.u16Width    = pstScldmaPollCfg->stSwapCfg.u16Width;
                    stSCLDMACfg.u16BufNum    = 1;
                    stSCLDMACfg.bvFlag.btsBase_0 = stSCLDMACfg.u16BufNum >=1 ? 1 : 0;

                    if (stSCLDMACfg.enColorType    == E_DRV_SCLDMA_IO_COLOR_YUV422)
                    {
                        for(idx=0; idx<stSCLDMACfg.u16BufNum ; idx++)
                        {
                            stSCLDMACfg.u32Base_Y[idx] = pstScldmaPollCfg->stSwapCfg.u32Base_Y[actid];
                            stSCLDMACfg.u32Base_C[idx] = stSCLDMACfg.u32Base_Y[idx]+16;
                        }
                    }
                    else
                    {
                        for(idx=0; idx<stSCLDMACfg.u16BufNum; idx++)
                        {
                            stSCLDMACfg.u32Base_Y[idx] = pstScldmaPollCfg->stSwapCfg.u32Base_Y[actid];
                            stSCLDMACfg.u32Base_C[idx] = stSCLDMACfg.u32Base_Y[idx] + (buffsize);
                        }
                    }
                    stSCLDMACfg = FILL_VERCHK_TYPE(stSCLDMACfg, stSCLDMACfg.VerChk_Version, stSCLDMACfg.VerChk_Size,DRV_SCLDMA_VERSION);
                    DrvSclDmaIoSetInBufferConfig(g_FD[E_DRV_ID_SCLDMA4],&stSCLDMACfg);
                    _FPGA_Pnl(pstScldmaPollCfg->stSwapCfg.u16Width,pstScldmaPollCfg->stSwapCfg.u16Height);
                }
                stSCLDMABFCfg = _SetRPoint
                    (pstScldmaPollCfg->stGetActCfg.enID[i],pstScldmaPollCfg->stGetActCfg.enMem[i],actid);
            }
        }
        //MsSleep(20);
        //sleep(1);
    }
    sclprintf("%s exit\n", __FUNCTION__);
}


void Test_FPGA_SCLDMA_1_4(FPGA_CONFIG stCfg)
{
    u32 buffsize;
    u32 idx;
    DrvSclHvspIoScalingConfig_t stHvspScaleCfg;
    SclMemConfig_t stMsysMemInfo;
    DrvSclDmaIoBufferConfig_t stSCLDMACfg;
    SclDmaPollConfig_t stScldmaPollCfg;
    SclThreadConfig_t  Scldma_Poll_Thread;
    DrvSclHvspIoPollConfig_t stHvspPollCfg;
    SclThreadConfig_t  Mo_Thread;
    u32 choise = 54;
    if(Init_Device() == FALSE)
    {
        return;
    }

    if(Open_Device(E_DRV_ID_HVSP1) == 0)
    {
        return;
    }

    if(Open_Device(E_DRV_ID_HVSP2) == 0)
    {
        return;
    }


    if(Open_Device(E_DRV_ID_SCLDMA1) == 0)
    {
        return;
    }
    if(Open_Device(E_DRV_ID_SCLDMA2) == 0)
    {
        return;
    }
    if(Open_Device(E_DRV_ID_SCLDMA4) == 0)
    {
        return;
    }

    if(Open_Device(E_DRV_ID_PNL) == 0)
    {
        return;
    }


    if(stCfg.u16BufNumber < 2)
    {
        sclprintf("BufferNumber must be 2 ~ 4");
        return;
    }
    // PNL, Timing Configure
    _FPGA_Pnl(stCfg.u16Dsp_Width,stCfg.u16Dsp_Height);

    if(stCfg.enSrc==E_DRV_SCLHVSP_IO_SRC_ISP)
    {
        _ISP_Input(stCfg.u16Src_Width,stCfg.u16Src_Height);
    }

    // HVSP1 Requenst IPM memory
    _Request_IPM(stCfg.u16Src_Width,stCfg.u16Src_Height);

    // HVSP1 in configure
    _HVSPInConfig(E_DRV_ID_HVSP1,stCfg.enSrc,stCfg.u16Src_Width,stCfg.u16Src_Height);

    // HVSP1 scaling configure
    memset(&stHvspScaleCfg, 0, sizeof(DrvSclHvspIoScalingConfig_t));
    stHvspScaleCfg = _FillHVSPScaleCfg(stCfg.u16Src_Width,stCfg.u16Src_Height,stCfg.u16Dsp_Width,stCfg.u16Dsp_Height);
    stHvspScaleCfg.stCropWin = _FillHVSPScaleCropCfg(stCfg.u16Src_Width,stCfg.u16Src_Height,0,0);
    _HVSPScaling(E_DRV_ID_HVSP1,stHvspScaleCfg,stCfg.enSrc);

    // SCLDMA1 OUT BUFFER Configure
    buffsize = stCfg.u16Dsp_Width * stCfg.u16Dsp_Height ; // 2bpp
    stMsysMemInfo = _AllocateDmem("FGPA14DMA",stCfg.u16BufNumber,buffsize);
    if(stMsysMemInfo.pVirAddr == NULL)
    {
        sclprintf("Allocate Mem fail !!!!!");
        return;
    }

    stSCLDMACfg = _FillBufferConfigByFPGAConfig(E_DRV_ID_SCLDMA1,stCfg,stMsysMemInfo,0);


    // config DMA4 buffer
    memset(&stSCLDMACfg, 0, sizeof(DrvSclDmaIoBufferConfig_t));
    stSCLDMACfg = _FillBufferConfigByFPGAConfig(E_DRV_ID_SCLDMA4,stCfg, stMsysMemInfo,1);
    //Set RPoint
    _SetRPoint(E_DRV_ID_SCLDMA1,stCfg.enMem,0x5);

    // create thread for single buffer
    if(stCfg.enBufferMd == E_DRV_SCLDMA_IO_BUFFER_MD_SINGLE)
    {
        // trig SCLDMA1
        _SetDMATrig(E_DRV_ID_SCLDMA1,E_DRV_SCLDMA_IO_MEM_IMI,1);
        _SetDMATrig(E_DRV_ID_SCLDMA1,stCfg.enMem,1);
        _SetDMAinTrig(E_DRV_ID_SCLDMA4,E_DRV_SCLDMA_IO_MEM_FRM,1);
        memset(&stScldmaPollCfg, 0, sizeof(SclDmaPollConfig_t));
        stScldmaPollCfg.bEn = 1;
        stScldmaPollCfg.enSC2Md = E_DRV_SC1_DSP;
        stScldmaPollCfg.stSwapCfg.enColor = stCfg.enColor;
        stScldmaPollCfg.stSwapCfg.enMem = stCfg.enMem;
        stScldmaPollCfg.stSwapCfg.u16Height   = stCfg.u16Dsp_Height;
        stScldmaPollCfg.stSwapCfg.u16Width    = stCfg.u16Dsp_Width;

        //0: front, 1: back
        if (stSCLDMACfg.enColorType    == E_DRV_SCLDMA_IO_COLOR_YUV422)
        {
            stScldmaPollCfg.stSwapCfg.u32Base_Y[0] = stMsysMemInfo.u32PhyAddr + (buffsize)*2;
            stScldmaPollCfg.stSwapCfg.u32Base_Y[1] = stMsysMemInfo.u32PhyAddr + (buffsize)*2 * 0;

        }
        else
        {
            stScldmaPollCfg.stSwapCfg.u32Base_Y[0] = stMsysMemInfo.u32PhyAddr + (buffsize)*1;
            stScldmaPollCfg.stSwapCfg.u32Base_C[0] = stMsysMemInfo.u32PhyAddr + (buffsize)*3;
            stScldmaPollCfg.stSwapCfg.u32Base_Y[1] = stMsysMemInfo.u32PhyAddr + (buffsize)*0 ;
            stScldmaPollCfg.stSwapCfg.u32Base_C[1] = stMsysMemInfo.u32PhyAddr + (buffsize)*2;
        }
        stScldmaPollCfg.stGetActCfg.enID[0] = E_DRV_ID_SCLDMA1;
        Scldma_Poll_Thread = _CreateTestThread(_Scldma_Poll_Thread_SingalBuf, &stScldmaPollCfg);
    }
    else if(stCfg.enBufferMd == E_DRV_SCLDMA_IO_BUFFER_MD_SWRING)
    {
        // trig SCLDMA1
        _SetDMATrig(E_DRV_ID_SCLDMA1,stCfg.enMem,1);
        // trig SCLDMA4
        _SetDMAinTrig(E_DRV_ID_SCLDMA4,E_DRV_SCLDMA_IO_MEM_FRM,1);

        stScldmaPollCfg.bEn = 1;
        stScldmaPollCfg.bswitchpnl = 1;
        stScldmaPollCfg.stGetActCfg.enMem[0]=stCfg.enMem;
        stScldmaPollCfg.stGetActCfg.enID[0] = E_DRV_ID_SCLDMA1;
        stScldmaPollCfg.stGetActCfg.Count = 1;
        stScldmaPollCfg.stSwapCfg.enColor = stCfg.enColor;
        ;
        stScldmaPollCfg.stSwapCfg.enMem=stCfg.enMem;
        stScldmaPollCfg.stSwapCfg.u16Height   = stCfg.u16Dsp_Height;
        stScldmaPollCfg.stSwapCfg.u16Width    = stCfg.u16Dsp_Width;
        stScldmaPollCfg.stSwapCfg.u32Base_Y[0]   = stMsysMemInfo.u32PhyAddr;
        stScldmaPollCfg.u16BufNumber = stCfg.u16BufNumber;
        Scldma_Poll_Thread = _CreateTestThread(_Scldma_Poll_Thread_GetQueue,&stScldmaPollCfg);
    }
    else if(stCfg.enBufferMd == E_DRV_SCLDMA_IO_BUFFER_MD_RING)
    {
        // trig SCLDMA1
        _SetDMATrig(E_DRV_ID_SCLDMA1,E_DRV_SCLDMA_IO_MEM_IMI,1);
        _SetDMATrig(E_DRV_ID_SCLDMA1,stCfg.enMem,1);
        // trig SCLDMA4
        _SetDMAinTrig(E_DRV_ID_SCLDMA4,E_DRV_SCLDMA_IO_MEM_FRM,1);

        stScldmaPollCfg.bEn = 1;
        stScldmaPollCfg.bswitchpnl = 1;
        stScldmaPollCfg.stGetActCfg.enMem[0]=stCfg.enMem;
        stScldmaPollCfg.stGetActCfg.enID[0] = E_DRV_ID_SCLDMA1;
        stScldmaPollCfg.stGetActCfg.Count = 1;
        stScldmaPollCfg.stSwapCfg.enColor = stCfg.enColor;
        stScldmaPollCfg.stSwapCfg.enMem=stCfg.enMem;
        stScldmaPollCfg.stSwapCfg.u16Height   = stCfg.u16Dsp_Height;
        stScldmaPollCfg.stSwapCfg.u16Width    = stCfg.u16Dsp_Width;
        for(idx=0 ;idx<stCfg.u16BufNumber;idx++)
        {
            if (stSCLDMACfg.enColorType    == E_DRV_SCLDMA_IO_COLOR_YUV420)
            {
                stScldmaPollCfg.stSwapCfg.u32Base_Y[idx]   = stMsysMemInfo.u32PhyAddr + ((buffsize*3/2)*idx);
            }
            else
            {
                stScldmaPollCfg.stSwapCfg.u32Base_Y[idx]   = stMsysMemInfo.u32PhyAddr + ((buffsize*2)*idx);
            }
        }
        stScldmaPollCfg.u16BufNumber = stCfg.u16BufNumber;
        Scldma_Poll_Thread = _CreateTestThread(_Scldma_Poll_Thread_GetActiveChangBuffer,&stScldmaPollCfg);
    }
    else
    {
        // trig SCLDMA1
        _SetDMATrig(E_DRV_ID_SCLDMA1,E_DRV_SCLDMA_IO_MEM_IMI,1);
        _SetDMATrig(E_DRV_ID_SCLDMA1,stCfg.enMem,1);

        // trig SCLDMA4
        _SetDMAinTrig(E_DRV_ID_SCLDMA4,E_DRV_SCLDMA_IO_MEM_FRM,1);
        stScldmaPollCfg.bEn = 1;
        stScldmaPollCfg.stGetActCfg.enMem[0]=stCfg.enMem;
        stScldmaPollCfg.stGetActCfg.enID[0] = E_DRV_ID_SCLDMA1;
        stScldmaPollCfg.stGetActCfg.Count=1;
        Scldma_Poll_Thread = _CreateTestThread(_Scldma_Poll_Thread_GetActive,&stScldmaPollCfg);
    }
    stHvspPollCfg.u32Timeout = 500;
    stHvspPollCfg.u8retval = POLLIN;
    DrvSclHvspIoPoll(g_FD[E_DRV_ID_HVSP1],&stHvspPollCfg);
    _SetRPoint(E_DRV_ID_SCLDMA1,stCfg.enMem,0x5);
    if(stCfg.enSrc==E_DRV_SCLHVSP_IO_SRC_ISP)
    {
        _IspRun(NULL,NULL);
    }
    Mo_Thread = _CreateMonitorThread(MonitorFunction, &choise);

    while(1)
    {
        MsSleep(1000);
        if(choise == 'Q' || choise == 'q')
        {
            stScldmaPollCfg.bEn = 0;
            sclprintf("sleep over \n");
            _SetDMATrig(E_DRV_ID_SCLDMA1,stCfg.enMem,0);
            // trig SCLDMA4
            _SetDMAinTrig(E_DRV_ID_SCLDMA4,E_DRV_SCLDMA_IO_MEM_FRM,0);
            sclprintf("close  start \n");
            if(Close_Device(E_DRV_ID_HVSP1) == 0)
            if(Close_Device(E_DRV_ID_HVSP2) == 0)
            if(Close_Device(E_DRV_ID_SCLDMA1) == 0)
            if(Close_Device(E_DRV_ID_SCLDMA2) == 0)
            if(Close_Device(E_DRV_ID_SCLDMA4) == 0)
            if(Close_Device(E_DRV_ID_PNL) == 0)
            if(stCfg.enSrc==E_DRV_SCLHVSP_IO_SRC_PAT_TGEN)
            sclprintf("close  over \n");
            _DeleteTask(Scldma_Poll_Thread.TaskId ,Scldma_Poll_Thread.pStackTop);
            _DeleteTask(Mo_Thread.TaskId ,Mo_Thread.pStackTop);
            break;
        }
        else if(choise=='C')
        {
            // HVSP1 scaling configure
            memset(&stHvspScaleCfg, 0, sizeof(DrvSclHvspIoScalingConfig_t));
            stHvspScaleCfg = _FillHVSPScaleCfg(stCfg.u16Src_Width,stCfg.u16Src_Height,stCfg.u16Dsp_Width,stCfg.u16Dsp_Height);
            stHvspScaleCfg.stCropWin = _FillHVSPScaleCropCfg
                (stCfg.u16Dsp_Width/4*3,stCfg.u16Dsp_Height/4*3,stCfg.u16Dsp_Width/8,stCfg.u16Dsp_Height/8);
            stHvspScaleCfg.bCropEn = 1;
            _HVSPScaling(E_DRV_ID_HVSP1,stHvspScaleCfg,stCfg.enSrc);
        }
        else if(choise=='K')
        {
            // HVSP1 scaling configure
            memset(&stHvspScaleCfg, 0, sizeof(DrvSclHvspIoScalingConfig_t));
            stHvspScaleCfg = _FillHVSPScaleCfg(stCfg.u16Src_Width,stCfg.u16Src_Height,stCfg.u16Dsp_Width,stCfg.u16Dsp_Height);
            stHvspScaleCfg.stCropWin = _FillHVSPScaleCropCfg
                (stCfg.u16Dsp_Width/2,stCfg.u16Dsp_Height/2,stCfg.u16Dsp_Width/4,stCfg.u16Dsp_Height/4);
            stHvspScaleCfg.bCropEn = 1;
            _HVSPScaling(E_DRV_ID_HVSP1,stHvspScaleCfg,stCfg.enSrc);
        }
        else if(choise=='c')
        {
            // HVSP1 scaling configure
            memset(&stHvspScaleCfg, 0, sizeof(DrvSclHvspIoScalingConfig_t));
            stHvspScaleCfg = _FillHVSPScaleCfg(stCfg.u16Src_Width,stCfg.u16Src_Height,stCfg.u16Dsp_Width,stCfg.u16Dsp_Height);
            stHvspScaleCfg.stCropWin = _FillHVSPScaleCropCfg
                (stCfg.u16Src_Width,stCfg.u16Src_Height,0,0);
            stHvspScaleCfg.bCropEn = 0;
            _HVSPScaling(E_DRV_ID_HVSP1,stHvspScaleCfg,stCfg.enSrc);
        }
        else if(choise=='M')
        {
            DrvSclHvspIoPriMaskConfig_t stPRICfg;
            DrvSclHvspIoPriMaskTriggerConfig_t stTrigger;
            stPRICfg.bMask = 1;
            stPRICfg.u8idx = 0;
            stPRICfg.stMaskWin.u16Height = stCfg.u16Dsp_Height/4;
            stPRICfg.stMaskWin.u16Width  = stCfg.u16Dsp_Width/4;
            stPRICfg.stMaskWin.u16X      = stCfg.u16Dsp_Width/4;
            stPRICfg.stMaskWin.u16Y      = stCfg.u16Dsp_Height/4;
            stPRICfg = FILL_VERCHK_TYPE(stPRICfg, stPRICfg.VerChk_Version, stPRICfg.VerChk_Size,DRV_SCLHVSP_VERSION);
            DrvSclHvspIoSetPriMaskConfig(g_FD[E_DRV_ID_HVSP1], &stPRICfg);

            stPRICfg.bMask = 1;
            stPRICfg.u8idx = 2;
            stPRICfg.stMaskWin.u16Height = stCfg.u16Dsp_Height/8;
            stPRICfg.stMaskWin.u16Width  = stCfg.u16Dsp_Width/2;
            stPRICfg.stMaskWin.u16X      = stCfg.u16Dsp_Width/4;
            stPRICfg.stMaskWin.u16Y      = stCfg.u16Dsp_Height/4*3;
            stPRICfg = FILL_VERCHK_TYPE(stPRICfg, stPRICfg.VerChk_Version, stPRICfg.VerChk_Size,DRV_SCLHVSP_VERSION);
            DrvSclHvspIoSetPriMaskConfig(g_FD[E_DRV_ID_HVSP1], &stPRICfg);

            stTrigger.bEn = 1;
            stTrigger = FILL_VERCHK_TYPE(stTrigger, stTrigger.VerChk_Version, stTrigger.VerChk_Size,DRV_SCLHVSP_VERSION);
            DrvSclHvspIoPirMaskTrigerConfig(g_FD[E_DRV_ID_HVSP1], &stTrigger);
        }
        else if(choise=='m')
        {
            DrvSclHvspIoPriMaskConfig_t stPRICfg;
            DrvSclHvspIoPriMaskTriggerConfig_t stTrigger;
            int idx;
            stPRICfg.bMask = 1;
            stPRICfg.u8idx = 8;
            stPRICfg.stMaskWin.u16Height = 30;
            stPRICfg.stMaskWin.u16Width  = 30;
            stPRICfg.stMaskWin.u16X      = 0;
            stPRICfg.stMaskWin.u16Y      = 0;
            stPRICfg = FILL_VERCHK_TYPE(stPRICfg, stPRICfg.VerChk_Version, stPRICfg.VerChk_Size,DRV_SCLHVSP_VERSION);
            DrvSclHvspIoSetPriMaskConfig(g_FD[E_DRV_ID_HVSP1], &stPRICfg);

            stPRICfg.bMask = 1;
            stPRICfg.u8idx = 0;
            stPRICfg.stMaskWin.u16Height = 30;
            stPRICfg.stMaskWin.u16Width  = 30;
            stPRICfg.stMaskWin.u16X      = 224;
            stPRICfg.stMaskWin.u16Y      = 448;
            stPRICfg = FILL_VERCHK_TYPE(stPRICfg, stPRICfg.VerChk_Version, stPRICfg.VerChk_Size,DRV_SCLHVSP_VERSION);
            DrvSclHvspIoSetPriMaskConfig(g_FD[E_DRV_ID_HVSP1], &stPRICfg);

            for(idx=1;idx<8;idx++)
            {
                stPRICfg.bMask = 1;
                stPRICfg.u8idx = idx;
                stPRICfg.stMaskWin.u16Height = 30;
                stPRICfg.stMaskWin.u16Width  = 30;
                stPRICfg.stMaskWin.u16X      = 32*idx;
                stPRICfg.stMaskWin.u16Y      = 32*idx;
                stPRICfg = FILL_VERCHK_TYPE(stPRICfg, stPRICfg.VerChk_Version, stPRICfg.VerChk_Size,DRV_SCLHVSP_VERSION);
                DrvSclHvspIoSetPriMaskConfig(g_FD[E_DRV_ID_HVSP1], &stPRICfg);
            }
            stTrigger.bEn = 1;
            stTrigger = FILL_VERCHK_TYPE(stTrigger, stTrigger.VerChk_Version, stTrigger.VerChk_Size,DRV_SCLHVSP_VERSION);
            DrvSclHvspIoPirMaskTrigerConfig(g_FD[E_DRV_ID_HVSP1], &stTrigger);
        }
        else if(choise=='n')
        {
            DrvSclHvspIoPriMaskConfig_t stPRICfg;
            DrvSclHvspIoPriMaskTriggerConfig_t stTrigger;
            stPRICfg.bMask = 0;
            stPRICfg.stMaskWin.u16Height = stCfg.u16Dsp_Height/4;
            stPRICfg.stMaskWin.u16Width  = stCfg.u16Dsp_Width/4;
            stPRICfg.stMaskWin.u16X      = stCfg.u16Dsp_Width/4;
            stPRICfg.stMaskWin.u16Y      = stCfg.u16Dsp_Height/4;
            stPRICfg = FILL_VERCHK_TYPE(stPRICfg, stPRICfg.VerChk_Version, stPRICfg.VerChk_Size,DRV_SCLHVSP_VERSION);
            DrvSclHvspIoSetPriMaskConfig(g_FD[E_DRV_ID_HVSP1], &stPRICfg);
            stTrigger.bEn = 1;
            stTrigger = FILL_VERCHK_TYPE(stTrigger, stTrigger.VerChk_Version, stTrigger.VerChk_Size,DRV_SCLHVSP_VERSION);
            DrvSclHvspIoPirMaskTrigerConfig(g_FD[E_DRV_ID_HVSP1], &stTrigger);
        }
        else if(choise=='N')
        {
            DrvSclHvspIoPriMaskTriggerConfig_t stTrigger;
            stTrigger.bEn = 0;
            stTrigger = FILL_VERCHK_TYPE(stTrigger, stTrigger.VerChk_Version, stTrigger.VerChk_Size,DRV_SCLHVSP_VERSION);
            DrvSclHvspIoPirMaskTrigerConfig(g_FD[E_DRV_ID_HVSP1], &stTrigger);
        }
        else if (choise=='B')
        {
            if(stCfg.enBufferMd == E_DRV_SCLDMA_IO_BUFFER_MD_RING)
            {
                sclprintf("FPGA14 over");
            }
            else
            {
                stScldmaPollCfg.bEn = 0;

                //pthread_join(Scldma_Poll_Thread, NULL);

            }
            break;
        }
        else if(choise =='P')
        {
            sclprintf("update PNL\n");
            _FPGA_Pnl(stCfg.u16Dsp_Width,stCfg.u16Dsp_Height);

        }
        else if(choise =='S')
        {
            //system("echo mem > /sys/power/state");
        }


    }


}
void Test_FPGA_SCLDMA_2_4(FPGA_CONFIG stCfg)
{
    u32 buffsize;
    int idx;
    DrvSclHvspIoScalingConfig_t stHvspScaleCfg;
    SclMemConfig_t stMsysMemInfo;
    DrvSclDmaIoBufferConfig_t stSCLDMACfg;
    SclDmaPollConfig_t stScldmaPollCfg;
    SclThreadConfig_t  Scldma_Poll_Thread;
    SclThreadConfig_t  Mo_Thread;
    FPGA_CONFIG stCfg2;
    u32 choise = 54;

    if(Init_Device() == FALSE)
    {
        return;
    }

    if(Open_Device(E_DRV_ID_HVSP1) == 0)
    {
        return;
    }

    if(Open_Device(E_DRV_ID_HVSP2) == 0)
    {
        return;
    }


    if(Open_Device(E_DRV_ID_SCLDMA1) == 0)
    {
        return;
    }
    if(Open_Device(E_DRV_ID_SCLDMA2) == 0)
    {
        return;
    }
    if(Open_Device(E_DRV_ID_SCLDMA4) == 0)
    {
        return;
    }

    if(Open_Device(E_DRV_ID_PNL) == 0)
    {
        return;
    }


    if(stCfg.u16BufNumber < 2)
    {
        sclprintf("BufferNumber must be 2 ~ 4");
        return;
    }
    // PNL, Timing Configure
    _FPGA_Pnl(stCfg.u16Dsp_Width,stCfg.u16Dsp_Height);

    if(stCfg.enSrc==E_DRV_SCLHVSP_IO_SRC_ISP)
    {
        _ISP_Input(stCfg.u16Src_Width,stCfg.u16Src_Height);
    }

    // HVSP1 Requenst IPM memory
    _Request_IPM(stCfg.u16Src_Width,stCfg.u16Src_Height);

    // HVSP1 in configure
    _HVSPInConfig(E_DRV_ID_HVSP1,stCfg.enSrc,stCfg.u16Src_Width,stCfg.u16Src_Height);

    // HVSP1 scaling configure
    memset(&stHvspScaleCfg, 0, sizeof(DrvSclHvspIoScalingConfig_t));
    stHvspScaleCfg = _FillHVSPScaleCfg(stCfg.u16Src_Width,stCfg.u16Src_Height,stCfg.u16Src_Width,stCfg.u16Src_Height);
    stHvspScaleCfg.stCropWin = _FillHVSPScaleCropCfg(stCfg.u16Src_Width,stCfg.u16Src_Height,0,0);
    _HVSPScaling(E_DRV_ID_HVSP1,stHvspScaleCfg,stCfg.enSrc);
     // HVSP2 in configure
    _HVSPInConfig(E_DRV_ID_HVSP2,E_DRV_SCLHVSP_IO_SRC_HVSP,stCfg.u16Src_Width,stCfg.u16Src_Height);

    // HVSP2 scaling configure
    memset(&stHvspScaleCfg, 0, sizeof(DrvSclHvspIoScalingConfig_t));
    stHvspScaleCfg = _FillHVSPScaleCfg(stCfg.u16Src_Width,stCfg.u16Src_Height,stCfg.u16Dsp_Width,stCfg.u16Dsp_Height);
    stHvspScaleCfg.stCropWin = _FillHVSPScaleCropCfg(stCfg.u16Src_Width,stCfg.u16Src_Height,0,0);
    _HVSPScaling(E_DRV_ID_HVSP2,stHvspScaleCfg,stCfg.enSrc);
    // SCLDMA1 OUT BUFFER Configure
    buffsize = stCfg.u16Dsp_Width * stCfg.u16Dsp_Height ; // 2bpp
    stCfg2 = stCfg;
    stCfg2.u16Dsp_Height = stCfg.u16Src_Height;
    stCfg2.u16Dsp_Width = stCfg.u16Src_Width;
    stCfg2.enMem = E_DRV_SCLDMA_IO_MEM_FRM;
    stMsysMemInfo = _AllocateDmem("FGPA14DMA",stCfg2.u16BufNumber,buffsize);
    if(stMsysMemInfo.pVirAddr == NULL)
    {
        sclprintf("Allocate Mem fail !!!!!");
        return;
    }

    stSCLDMACfg = _FillBufferConfigByFPGAConfig(E_DRV_ID_SCLDMA1,stCfg2,stMsysMemInfo,0);
    // SCLDMA2 OUT BUFFER Configure
    buffsize = stCfg.u16Dsp_Width * stCfg.u16Dsp_Height ; // 2bpp
    stMsysMemInfo = _AllocateDmem("FGPA24DMA",stCfg.u16BufNumber,buffsize);
    if(stMsysMemInfo.pVirAddr == 0)
    {
        sclprintf("Allocate Mem fail !!!!!");
        return;
    }
    stSCLDMACfg = _FillBufferConfigByFPGAConfig(E_DRV_ID_SCLDMA2,stCfg,stMsysMemInfo,0);

    // config DMA4 buffer
    memset(&stSCLDMACfg, 0, sizeof(DrvSclDmaIoBufferConfig_t));
    stSCLDMACfg = _FillBufferConfigByFPGAConfig(E_DRV_ID_SCLDMA4,stCfg, stMsysMemInfo,1);
    //Set RPoint
    _SetRPoint(E_DRV_ID_SCLDMA2,stCfg.enMem,0x5);
    // create thread for single buffer
    if(stCfg.enBufferMd == E_DRV_SCLDMA_IO_BUFFER_MD_SINGLE)
    {
        // trig SCLDMA2
        _SetDMATrig(E_DRV_ID_SCLDMA1,E_DRV_SCLDMA_IO_MEM_IMI,1);
        _SetDMATrig(E_DRV_ID_SCLDMA2,stCfg.enMem,1);
        _SetDMAinTrig(E_DRV_ID_SCLDMA4,E_DRV_SCLDMA_IO_MEM_FRM,1);
        memset(&stScldmaPollCfg, 0, sizeof(SclDmaPollConfig_t));
        stScldmaPollCfg.bEn = 1;
        stScldmaPollCfg.enSC2Md = E_DRV_SC2_DSP;
        stScldmaPollCfg.stSwapCfg.enColorsc2 = stCfg.enColor;
        stScldmaPollCfg.stSwapCfg.enMemsc2 = stCfg.enMem;
        stScldmaPollCfg.stSwapCfg.u16Height2   = stCfg.u16Dsp_Height;
        stScldmaPollCfg.stSwapCfg.u16Width2    = stCfg.u16Dsp_Width;
        stScldmaPollCfg.stGetActCfg.enID[0] = E_DRV_ID_SCLDMA2;

        //0: front, 1: back
        if (stSCLDMACfg.enColorType    == E_DRV_SCLDMA_IO_COLOR_YUV422)
        {
            stScldmaPollCfg.stSwapCfg.u32Base_Y2[0] = stMsysMemInfo.u32PhyAddr + (buffsize)*2;
            stScldmaPollCfg.stSwapCfg.u32Base_Y2[1] = stMsysMemInfo.u32PhyAddr + (buffsize)*2 * 0;

        }
        else
        {
            stScldmaPollCfg.stSwapCfg.u32Base_Y2[0] = stMsysMemInfo.u32PhyAddr + (buffsize)*1;
            stScldmaPollCfg.stSwapCfg.u32Base_C2[0] = stMsysMemInfo.u32PhyAddr + (buffsize)*3;
            stScldmaPollCfg.stSwapCfg.u32Base_Y2[1] = stMsysMemInfo.u32PhyAddr + (buffsize)*0 ;
            stScldmaPollCfg.stSwapCfg.u32Base_C2[1] = stMsysMemInfo.u32PhyAddr + (buffsize)*2;
        }
        Scldma_Poll_Thread = _CreateTestThread(_Scldma_Poll_Thread_SingalBuf, &stScldmaPollCfg);
    }
    else if(stCfg.enBufferMd == E_DRV_SCLDMA_IO_BUFFER_MD_SWRING)
    {
        // trig SCLDMA2
        _SetDMATrig(E_DRV_ID_SCLDMA2,stCfg.enMem,1);
        // trig SCLDMA4
        _SetDMAinTrig(E_DRV_ID_SCLDMA4,E_DRV_SCLDMA_IO_MEM_FRM,1);

        stScldmaPollCfg.bEn = 1;
        stScldmaPollCfg.bswitchpnl = 1;
        stScldmaPollCfg.stGetActCfg.enMem[0]=stCfg.enMem;
        stScldmaPollCfg.stGetActCfg.enID[0] = E_DRV_ID_SCLDMA2;
        stScldmaPollCfg.stGetActCfg.Count = 1;
        stScldmaPollCfg.stSwapCfg.enColor = stCfg.enColor;
        ;
        stScldmaPollCfg.stSwapCfg.enMem=stCfg.enMem;
        stScldmaPollCfg.stSwapCfg.u16Height   = stCfg.u16Dsp_Height;
        stScldmaPollCfg.stSwapCfg.u16Width    = stCfg.u16Dsp_Width;
        stScldmaPollCfg.stSwapCfg.u32Base_Y[0]   = stMsysMemInfo.u32PhyAddr;
        stScldmaPollCfg.u16BufNumber = stCfg.u16BufNumber;
        Scldma_Poll_Thread = _CreateTestThread(_Scldma_Poll_Thread_GetQueue,&stScldmaPollCfg);
    }
    else if(stCfg.enBufferMd == E_DRV_SCLDMA_IO_BUFFER_MD_RING)
    {
        // trig SCLDMA2
        _SetDMATrig(E_DRV_ID_SCLDMA1,E_DRV_SCLDMA_IO_MEM_IMI,1);
        _SetDMATrig(E_DRV_ID_SCLDMA2,stCfg.enMem,1);
        // trig SCLDMA4
        _SetDMAinTrig(E_DRV_ID_SCLDMA4,E_DRV_SCLDMA_IO_MEM_FRM,1);

        stScldmaPollCfg.bEn = 1;
        stScldmaPollCfg.bswitchpnl = 1;
        stScldmaPollCfg.stGetActCfg.enMem[0]=stCfg.enMem;
        stScldmaPollCfg.stGetActCfg.enID[0] = E_DRV_ID_SCLDMA2;
        stScldmaPollCfg.stGetActCfg.Count = 1;
        stScldmaPollCfg.stSwapCfg.enColor = stCfg.enColor;
        stScldmaPollCfg.stSwapCfg.enMem=stCfg.enMem;
        stScldmaPollCfg.stSwapCfg.u16Height   = stCfg.u16Dsp_Height;
        stScldmaPollCfg.stSwapCfg.u16Width    = stCfg.u16Dsp_Width;
        for(idx=0 ;idx<stCfg.u16BufNumber;idx++)
        {
            if (stSCLDMACfg.enColorType    == E_DRV_SCLDMA_IO_COLOR_YUV420)
            {
                stScldmaPollCfg.stSwapCfg.u32Base_Y[idx]   = stMsysMemInfo.u32PhyAddr + ((buffsize*3/2)*idx);
            }
            else
            {
                stScldmaPollCfg.stSwapCfg.u32Base_Y[idx]   = stMsysMemInfo.u32PhyAddr + ((buffsize*2)*idx);
            }
        }
        stScldmaPollCfg.u16BufNumber = stCfg.u16BufNumber;
        Scldma_Poll_Thread = _CreateTestThread(_Scldma_Poll_Thread_GetActiveChangBuffer,&stScldmaPollCfg);
    }
    else
    {
        // trig SCLDMA2
        _SetDMATrig(E_DRV_ID_SCLDMA2,stCfg.enMem,1);
        _SetDMATrig(E_DRV_ID_SCLDMA1,E_DRV_SCLDMA_IO_MEM_IMI,1);

        // trig SCLDMA4
        _SetDMAinTrig(E_DRV_ID_SCLDMA4,E_DRV_SCLDMA_IO_MEM_FRM,1);
        stScldmaPollCfg.bEn = 1;
        stScldmaPollCfg.stGetActCfg.enMem[0]=stCfg.enMem;
        stScldmaPollCfg.stGetActCfg.enID[0] = E_DRV_ID_SCLDMA2;
        stScldmaPollCfg.stGetActCfg.Count=1;
        Scldma_Poll_Thread = _CreateTestThread(_Scldma_Poll_Thread_GetActive,&stScldmaPollCfg);
    }
    _SetRPoint(E_DRV_ID_SCLDMA2,stCfg.enMem,0x5);
    Mo_Thread = _CreateMonitorThread(MonitorFunction, &choise);

    while(1)
    {
        MsSleep(2000);
        if(choise == 'Q'||choise == 'q')
        {
            // trig SCLDMA4
            stScldmaPollCfg.bEn = 0;
            _SetDMATrig(E_DRV_ID_SCLDMA2,stCfg.enMem,0);
            _SetDMATrig(E_DRV_ID_SCLDMA1,E_DRV_SCLDMA_IO_MEM_IMI,0);
            // trig SCLDMA4
            _SetDMAinTrig(E_DRV_ID_SCLDMA4,E_DRV_SCLDMA_IO_MEM_FRM,0);
            if(Close_Device(E_DRV_ID_HVSP1) == 0)
            if(Close_Device(E_DRV_ID_HVSP2) == 0)
            if(Close_Device(E_DRV_ID_SCLDMA1) == 0)
            if(Close_Device(E_DRV_ID_SCLDMA2) == 0)
            if(Close_Device(E_DRV_ID_SCLDMA4) == 0)
            if(Close_Device(E_DRV_ID_PNL) == 0)
            if(stCfg.enSrc==E_DRV_SCLHVSP_IO_SRC_PAT_TGEN)
            sclprintf("close  over \n");
            _DeleteTask(Scldma_Poll_Thread.TaskId ,Scldma_Poll_Thread.pStackTop);
            _DeleteTask(Mo_Thread.TaskId ,Mo_Thread.pStackTop);
            break;
        }
        else if (choise=='B')
        {
            if(stCfg.enBufferMd == E_DRV_SCLDMA_IO_BUFFER_MD_RING)
            {
                sclprintf("FPGA14 over");
            }
            else
            {
                stScldmaPollCfg.bEn = 0;

                //pthread_join(Scldma_Poll_Thread, NULL);

            }
            break;
        }
        else if(choise =='P')
        {
            sclprintf("update PNL\n");
            _FPGA_Pnl(stCfg.u16Dsp_Width,stCfg.u16Dsp_Height);

        }
        else if(choise =='p')
        {
            sclprintf("static pat\n");
            //_Static_Pat();

        }
        else if(choise =='S')
        {
            //system("echo mem > /sys/power/state");
        }


    }


}

void Test_FPGA_SCLDMA_1_2_4(FPGASC2_CONFIG stCfg)
{
    u32 buffsize,buffsize2;
    DrvSclHvspIoScalingConfig_t stHvspScaleCfg;
    SclMemConfig_t stMsysMemInfo;
    SclMemConfig_t stMsysMemSC2Info;
    DrvSclDmaIoBufferConfig_t stSCLDMACfg;
    SclDmaPollConfig_t stScldmaPollCfg;
    SclDmaPollConfig_t stScldmaPollCfg2;
    SclThreadConfig_t  Scldma_Poll_Thread;
    SclThreadConfig_t  Scldma_Poll_Thread2;
    FPGA_CONFIG stCpyCfg;
    DrvSclDmaIoActiveBufferConfig_t stSCLDMABFCfg;
    u32 choise = 54;
    SclThreadConfig_t  Mo_Thread;
    Init_Device();

    if(Open_Device(E_DRV_ID_HVSP1) == 0)
    {
        return;
    }

    if(Open_Device(E_DRV_ID_HVSP2) == 0)
    {
        return;
    }


    if(Open_Device(E_DRV_ID_SCLDMA1) == 0)
    {
        return;
    }
    if(Open_Device(E_DRV_ID_SCLDMA2) == 0)
    {
        return;
    }
    if(Open_Device(E_DRV_ID_SCLDMA4) == 0)
    {
        return;
    }

    if(Open_Device(E_DRV_ID_PNL) == 0)
    {
        return;
    }


    if(stCfg.u16BufNumber < 2)
    {
        sclprintf("BufferNumber must be 2 ~ 4");
        return;
    }

    // PNL, Timing Configure
    if (stCfg.enSC2Dsp==E_DRV_SC2_DSP)
        _FPGA_Pnl(stCfg.u16Dsp_Width,stCfg.u16Dsp_Height);
    else
        _FPGA_Pnl(stCfg.u16sc1out_Width,stCfg.u16sc1out_Height);

    // chip top set
    //_FPGA_Chiptop();

    // HVSP1 Requenst IPM memory
    _Request_IPM(stCfg.u16Src_Width,stCfg.u16Src_Height);

    // HVSP1 in configure
    _HVSPInConfig(E_DRV_ID_HVSP1,stCfg.enSrc,stCfg.u16Src_Width,stCfg.u16Src_Height);

    // HVSP1 scaling configure
    memset(&stHvspScaleCfg, 0, sizeof(DrvSclHvspIoScalingConfig_t));
    stHvspScaleCfg = _FillHVSPScaleCfg(stCfg.u16Src_Width,stCfg.u16Src_Height,stCfg.u16sc1out_Width,stCfg.u16sc1out_Height);
    stHvspScaleCfg.stCropWin = _FillHVSPScaleCropCfg(stCfg.u16Src_Width,stCfg.u16Src_Height,0,0);
    _HVSPScaling(E_DRV_ID_HVSP1,stHvspScaleCfg,stCfg.enSrc);

     // HVSP2 in configure
    _HVSPInConfig(E_DRV_ID_HVSP2,E_DRV_SCLHVSP_IO_SRC_HVSP,stCfg.u16Src_Width,stCfg.u16Src_Height);

    // HVSP2 scaling configure
    memset(&stHvspScaleCfg, 0, sizeof(DrvSclHvspIoScalingConfig_t));
    stHvspScaleCfg = _FillHVSPScaleCfg(stCfg.u16sc1out_Width,stCfg.u16sc1out_Height,stCfg.u16Dsp_Width,stCfg.u16Dsp_Height);
    _HVSPScaling(E_DRV_ID_HVSP2,stHvspScaleCfg,stCfg.enSrc);


    // SCLDMA1 OUT BUFFER Configure
    memset(&stMsysMemInfo, 0, sizeof(SclMemConfig_t));
    buffsize = stCfg.u16sc1out_Width * stCfg.u16sc1out_Height ; // 2bpp
    stMsysMemInfo = _AllocateDmem("FGPA1DMA",(stCfg.u16BufNumber),buffsize);
    if(stMsysMemInfo.pVirAddr == NULL)
    {
        sclprintf("Allocate Mem fail !!!!!");
        return;
    }


    stCpyCfg = _FPGACfgTran2FPGASC2(stCfg,0);
    stSCLDMACfg = _FillBufferConfigByFPGAConfig(E_DRV_ID_SCLDMA1,stCpyCfg,stMsysMemInfo,0);
    if(stCfg.enSC2Dsp==E_DRV_SC1_DSP)
    {
        // config DMA4 buffer
        stSCLDMACfg = _FillBufferConfigByFPGAConfig(E_DRV_ID_SCLDMA4,stCpyCfg,stMsysMemInfo,1);
    }
    // SCLDMA2 OUT BUFFER Configure
    memset(&stMsysMemSC2Info, 0, sizeof(SclMemConfig_t));
    buffsize2 = stCfg.u16Dsp_Width * stCfg.u16Dsp_Height ; // 2bpp
    stMsysMemSC2Info = _AllocateDmem("FGPASC2DMA",stCfg.u16BufNumber,buffsize2);
    if(stMsysMemSC2Info.pVirAddr == 0)
    {
        sclprintf("Allocate Mem fail !!!!!");
        return;
    }
    stCpyCfg = _FPGACfgTran2FPGASC2(stCfg,1);
    stSCLDMACfg = _FillBufferConfigByFPGAConfig(E_DRV_ID_SCLDMA2,stCpyCfg,stMsysMemSC2Info,0);
    if(stCfg.enSC2Dsp==E_DRV_SC2_DSP)
    {
        // config DMA4 buffer
        stSCLDMACfg = _FillBufferConfigByFPGAConfig(E_DRV_ID_SCLDMA4,stCpyCfg,stMsysMemSC2Info,1);
    }

    //fix warning
    stSCLDMACfg=stSCLDMACfg;

    // create thread for single buffer
    if(stCfg.enBufferMd == E_DRV_SCLDMA_IO_BUFFER_MD_SINGLE && stCfg.enBufferMdsc2 == E_DRV_SCLDMA_IO_BUFFER_MD_SINGLE)
    {
        _SetDMATrig(E_DRV_ID_SCLDMA1,E_DRV_SCLDMA_IO_MEM_IMI,1);
        // trig SCLDMA1
        _SetDMATrig(E_DRV_ID_SCLDMA2,stCfg.enMemsc2,1);
        _SetDMATrig(E_DRV_ID_SCLDMA1,stCfg.enMem,1);

        _SetDMAinTrig(E_DRV_ID_SCLDMA4,E_DRV_SCLDMA_IO_MEM_FRM,1);
        memset(&stScldmaPollCfg, 0, sizeof(SclDmaPollConfig_t));
        stScldmaPollCfg.bEn                   = 1;
        stScldmaPollCfg.enBufferMd            = stCfg.enBufferMd;
        stScldmaPollCfg.enBufferMdsc2         = stCfg.enBufferMdsc2;
        stScldmaPollCfg.enSC2Md               = stCfg.enSC2Dsp;
        stScldmaPollCfg.u16BufNumber          = stCfg.u16BufNumber;
        stScldmaPollCfg.stSwapCfg.enColor     = stCfg.enColor;
        stScldmaPollCfg.stSwapCfg.enMem       = stCfg.enMem;
        stScldmaPollCfg.stSwapCfg.enColorsc2  = stCfg.enColorsc2;

        stScldmaPollCfg.stSwapCfg.enMemsc2    = stCfg.enMemsc2;
        stScldmaPollCfg.stSwapCfg.u16Height2   = stCfg.u16Dsp_Height;
        stScldmaPollCfg.stSwapCfg.u16Width2    = stCfg.u16Dsp_Width;
        stScldmaPollCfg.stSwapCfg.u16Height   = stCfg.u16sc1out_Height;
        stScldmaPollCfg.stSwapCfg.u16Width    = stCfg.u16sc1out_Width;

        //0: front, 1: back
        if (stSCLDMACfg.enColorType    == E_DRV_SCLDMA_IO_COLOR_YUV422)
        {
            stScldmaPollCfg.stSwapCfg.u32Base_Y[0] = stMsysMemInfo.u32PhyAddr + (buffsize)*2;
            stScldmaPollCfg.stSwapCfg.u32Base_Y[1] = stMsysMemInfo.u32PhyAddr + (buffsize)*2 * 0;
            stScldmaPollCfg.stSwapCfg.u32Base_Y2[0]= stMsysMemSC2Info.u32PhyAddr + (buffsize2)*2;
            stScldmaPollCfg.stSwapCfg.u32Base_Y2[1]= stMsysMemSC2Info.u32PhyAddr + (buffsize2)*2 * 0;
        }
        else
        {
            stScldmaPollCfg.stSwapCfg.u32Base_Y[0] = stMsysMemInfo.u32PhyAddr + (buffsize)*1;
            stScldmaPollCfg.stSwapCfg.u32Base_C[0] = stMsysMemInfo.u32PhyAddr + (buffsize)*3;
            stScldmaPollCfg.stSwapCfg.u32Base_Y[1] = stMsysMemInfo.u32PhyAddr + (buffsize)*0 ;
            stScldmaPollCfg.stSwapCfg.u32Base_C[1] = stMsysMemInfo.u32PhyAddr + (buffsize)*2;
            stScldmaPollCfg.stSwapCfg.u32Base_Y2[0] = stMsysMemSC2Info.u32PhyAddr + (buffsize2)*1;
            stScldmaPollCfg.stSwapCfg.u32Base_C2[0] = stMsysMemSC2Info.u32PhyAddr + (buffsize2)*3;
            stScldmaPollCfg.stSwapCfg.u32Base_Y2[1] = stMsysMemSC2Info.u32PhyAddr + (buffsize2)*0 ;
            stScldmaPollCfg.stSwapCfg.u32Base_C2[1] = stMsysMemSC2Info.u32PhyAddr + (buffsize2)*2;
        }
        Scldma_Poll_Thread = _CreateTestThread(_ScldmaSC2_Poll_Thread_SingalBuf,&stScldmaPollCfg);
        Scldma_Poll_Thread2 = Scldma_Poll_Thread2;
    }
    else if((stCfg.enBufferMd == E_DRV_SCLDMA_IO_BUFFER_MD_SINGLE && stCfg.enBufferMdsc2 == E_DRV_SCLDMA_IO_BUFFER_MD_RING)
        || (stCfg.enBufferMd == E_DRV_SCLDMA_IO_BUFFER_MD_RING&& stCfg.enBufferMdsc2 == E_DRV_SCLDMA_IO_BUFFER_MD_SINGLE))
    {
        _SetDMATrig(E_DRV_ID_SCLDMA1,E_DRV_SCLDMA_IO_MEM_IMI,1);
        // trig SCLDMA1
        _SetDMATrig(E_DRV_ID_SCLDMA2,stCfg.enMemsc2,1);
        _SetDMATrig(E_DRV_ID_SCLDMA1,stCfg.enMem,1);

        _SetDMAinTrig(E_DRV_ID_SCLDMA4,E_DRV_SCLDMA_IO_MEM_FRM,1);
        memset(&stScldmaPollCfg, 0, sizeof(SclDmaPollConfig_t));
        stScldmaPollCfg.bEn                   = 1;
        stScldmaPollCfg.enSC2Md = stCfg.enSC2Dsp;
        stScldmaPollCfg.enBufferMd            = stCfg.enBufferMd;
        stScldmaPollCfg.enBufferMdsc2         = stCfg.enBufferMdsc2;
        stScldmaPollCfg.enSC2Md               = stCfg.enSC2Dsp;
        stScldmaPollCfg.u16BufNumber          = stCfg.u16BufNumber;
        stScldmaPollCfg.stSwapCfg.enColor     = stCfg.enColor;
        stScldmaPollCfg.stSwapCfg.enMem       = stCfg.enMem;
        stScldmaPollCfg.stSwapCfg.enColorsc2  = stCfg.enColorsc2;

        stScldmaPollCfg.stSwapCfg.enMemsc2    = stCfg.enMemsc2;
        stScldmaPollCfg.stSwapCfg.u16Height2   = stCfg.u16Dsp_Height;
        stScldmaPollCfg.stSwapCfg.u16Width2    = stCfg.u16Dsp_Width;
        stScldmaPollCfg.stSwapCfg.u16Height   = stCfg.u16sc1out_Height;
        stScldmaPollCfg.stSwapCfg.u16Width    = stCfg.u16sc1out_Width;
        if(stCfg.enBufferMdsc2 == E_DRV_SCLDMA_IO_BUFFER_MD_RING)
        {
            stScldmaPollCfg.stGetActCfg.enMem[0]=stCfg.enMem;
            stScldmaPollCfg.stGetActCfg.enID[0] = E_DRV_ID_SCLDMA1;
        }
        else
        {
            stScldmaPollCfg.stGetActCfg.enMem[0]=stCfg.enMemsc2;
            stScldmaPollCfg.stGetActCfg.enID[0] = E_DRV_ID_SCLDMA2;
        }
        //0: front, 1: back
        if (stSCLDMACfg.enColorType    == E_DRV_SCLDMA_IO_COLOR_YUV422)
        {
            stScldmaPollCfg.stSwapCfg.u32Base_Y[0] = stMsysMemInfo.u32PhyAddr + (buffsize)*2;
            stScldmaPollCfg.stSwapCfg.u32Base_Y[1] = stMsysMemInfo.u32PhyAddr + (buffsize)*2 * 0;
            stScldmaPollCfg.stSwapCfg.u32Base_Y2[0]= stMsysMemSC2Info.u32PhyAddr + (buffsize2)*2;
            stScldmaPollCfg.stSwapCfg.u32Base_Y2[1]= stMsysMemSC2Info.u32PhyAddr + (buffsize2)*2 * 0;
        }
        else
        {
            stScldmaPollCfg.stSwapCfg.u32Base_Y[0] = stMsysMemInfo.u32PhyAddr + (buffsize)*1;
            stScldmaPollCfg.stSwapCfg.u32Base_C[0] = stMsysMemInfo.u32PhyAddr + (buffsize)*3;
            stScldmaPollCfg.stSwapCfg.u32Base_Y[1] = stMsysMemInfo.u32PhyAddr + (buffsize)*0 ;
            stScldmaPollCfg.stSwapCfg.u32Base_C[1] = stMsysMemInfo.u32PhyAddr + (buffsize)*2;
            stScldmaPollCfg.stSwapCfg.u32Base_Y2[0] = stMsysMemSC2Info.u32PhyAddr + (buffsize2)*1;
            stScldmaPollCfg.stSwapCfg.u32Base_C2[0] = stMsysMemSC2Info.u32PhyAddr + (buffsize2)*3;
            stScldmaPollCfg.stSwapCfg.u32Base_Y2[1] = stMsysMemSC2Info.u32PhyAddr + (buffsize2)*0 ;
            stScldmaPollCfg.stSwapCfg.u32Base_C2[1] = stMsysMemSC2Info.u32PhyAddr + (buffsize2)*2;
        }
        Scldma_Poll_Thread = _CreateTestThread(_Scldma_Poll_Thread_SingalBuf,&stScldmaPollCfg);
        stScldmaPollCfg2.bEn = 1;
        if(stCfg.enBufferMdsc2 == E_DRV_SCLDMA_IO_BUFFER_MD_RING)
        {
            stScldmaPollCfg2.stGetActCfg.enMem[0]=stCfg.enMemsc2;
            stScldmaPollCfg2.stGetActCfg.enID[0] = E_DRV_ID_SCLDMA2;
            stSCLDMABFCfg = _SetRPoint(E_DRV_ID_SCLDMA2,stCfg.enMemsc2,0x5);
        }
        else
        {
            stScldmaPollCfg2.stGetActCfg.enMem[0]=stCfg.enMem;
            stScldmaPollCfg2.stGetActCfg.enID[0] = E_DRV_ID_SCLDMA1;
            stSCLDMABFCfg = _SetRPoint(E_DRV_ID_SCLDMA1,stCfg.enMem,0x5);
        }
        stScldmaPollCfg2.stGetActCfg.Count=1;
        Scldma_Poll_Thread2 = _CreateTestThread(_Scldma_Poll_Thread_GetActive,&stScldmaPollCfg2);
    }
    else if(stCfg.enBufferMd == E_DRV_SCLDMA_IO_BUFFER_MD_SWRING && stCfg.enBufferMdsc2 == E_DRV_SCLDMA_IO_BUFFER_MD_SWRING)
    {
        // trig SCLDMA2
        _SetDMATrig(E_DRV_ID_SCLDMA1,E_DRV_SCLDMA_IO_MEM_IMI,1);
        _SetDMATrig(E_DRV_ID_SCLDMA1,stCfg.enMem,1);
        _SetDMATrig(E_DRV_ID_SCLDMA2,stCfg.enMemsc2,1);
        // trig SCLDMA4
        _SetDMAinTrig(E_DRV_ID_SCLDMA4,E_DRV_SCLDMA_IO_MEM_FRM,1);

        stScldmaPollCfg.bEn = 1;
        stScldmaPollCfg.bswitchpnl = (stCfg.enSC2Dsp==E_DRV_SC1_DSP)? 1 : 0;
        stScldmaPollCfg.stGetActCfg.enMem[0]=stCfg.enMem;
        stScldmaPollCfg.stGetActCfg.enID[0] = E_DRV_ID_SCLDMA1;
        stScldmaPollCfg.stGetActCfg.Count = 1;
        stScldmaPollCfg.stSwapCfg.enColor = stCfg.enColor;
        stScldmaPollCfg.stSwapCfg.enMem=stCfg.enMem;
        stScldmaPollCfg.stSwapCfg.u16Height   = stCfg.u16sc1out_Height;
        stScldmaPollCfg.stSwapCfg.u16Width    = stCfg.u16sc1out_Width;
        stScldmaPollCfg.stSwapCfg.u32Base_Y[0]   = stMsysMemInfo.u32PhyAddr;
        stScldmaPollCfg.u16BufNumber = stCfg.u16BufNumber;
        stScldmaPollCfg2.bEn = 1;
        stScldmaPollCfg2.bswitchpnl = (stCfg.enSC2Dsp==E_DRV_SC2_DSP)? 1 : 0;
        stScldmaPollCfg2.stGetActCfg.enMem[0]=stCfg.enMemsc2;
        stScldmaPollCfg2.stGetActCfg.enID[0] = E_DRV_ID_SCLDMA2;
        stScldmaPollCfg2.stGetActCfg.Count = 1;
        stScldmaPollCfg2.stSwapCfg.enColor = stCfg.enColorsc2;
        stScldmaPollCfg2.stSwapCfg.enMem=stCfg.enMemsc2;
        stScldmaPollCfg2.stSwapCfg.u16Height   = stCfg.u16Dsp_Height;
        stScldmaPollCfg2.stSwapCfg.u16Width    = stCfg.u16Dsp_Width;
        stScldmaPollCfg2.stSwapCfg.u32Base_Y[0]   = stMsysMemSC2Info.u32PhyAddr;
        stScldmaPollCfg2.u16BufNumber = stCfg.u16BufNumber;
        Scldma_Poll_Thread = _CreateTestThread(_Scldma_Poll_Thread_GetQueue,&stScldmaPollCfg);
        Scldma_Poll_Thread2 = _CreateTestThread(_Scldma_Poll_Thread_GetQueue,&stScldmaPollCfg2);
    }
    else
    {
        // trig SCLDMA1
        _SetDMATrig(E_DRV_ID_SCLDMA1,E_DRV_SCLDMA_IO_MEM_IMI,1);
        _SetDMATrig(E_DRV_ID_SCLDMA1,stCfg.enMem,1);
        stSCLDMABFCfg = _SetRPoint(E_DRV_ID_SCLDMA1,stCfg.enMem,0x5);
        // trig SCLDMA2
        _SetDMATrig(E_DRV_ID_SCLDMA2,stCfg.enMemsc2,1);
        stSCLDMABFCfg = _SetRPoint(E_DRV_ID_SCLDMA2,stCfg.enMemsc2,0x5);
        // trig SCLDMA4
        _SetDMAinTrig(E_DRV_ID_SCLDMA4,E_DRV_SCLDMA_IO_MEM_FRM,1);
        stScldmaPollCfg.bEn = 1;
        stScldmaPollCfg.stGetActCfg.enMem[0]=stCfg.enMem;
        stScldmaPollCfg.stGetActCfg.enID[0] = E_DRV_ID_SCLDMA1;
        stScldmaPollCfg.stGetActCfg.Count=1;
        stScldmaPollCfg2.bEn = 1;
        stScldmaPollCfg2.stGetActCfg.enMem[0]=stCfg.enMemsc2;
        stScldmaPollCfg2.stGetActCfg.enID[0] = E_DRV_ID_SCLDMA2;
        stScldmaPollCfg2.stGetActCfg.Count=1;
        Scldma_Poll_Thread = _CreateTestThread(_Scldma_Poll_Thread_GetActive,&stScldmaPollCfg);
        Scldma_Poll_Thread2 = _CreateTestThread(_Scldma_Poll_Thread_GetActive,&stScldmaPollCfg2);
        //fix warning
        stSCLDMABFCfg=stSCLDMABFCfg;
    }
    Mo_Thread = _CreateMonitorThread(MonitorFunction, &choise);
    while(1)
    {
        MsSleep(1000);
        if(choise == 'Q'||choise == 'q')
        {
            MsSleep(10000);
            _SetDMATrig(E_DRV_ID_SCLDMA1,stCfg.enMem,0);
            _SetDMATrig(E_DRV_ID_SCLDMA2,stCfg.enMemsc2,0);
            _SetDMATrig(E_DRV_ID_SCLDMA1,E_DRV_SCLDMA_IO_MEM_IMI,0);
            // trig SCLDMA4
            _SetDMAinTrig(E_DRV_ID_SCLDMA4,E_DRV_SCLDMA_IO_MEM_FRM,0);
            stScldmaPollCfg.bEn = 0;
            _DeleteTask(Scldma_Poll_Thread.TaskId ,Scldma_Poll_Thread.pStackTop);
            _DeleteTask(Scldma_Poll_Thread2.TaskId ,Scldma_Poll_Thread2.pStackTop);
            _DeleteTask(Mo_Thread.TaskId ,Mo_Thread.pStackTop);
            if(Close_Device(E_DRV_ID_HVSP1) == 0)
            if(Close_Device(E_DRV_ID_HVSP2) == 0)
            if(Close_Device(E_DRV_ID_SCLDMA1) == 0)
            if(Close_Device(E_DRV_ID_SCLDMA2) == 0)
            if(Close_Device(E_DRV_ID_SCLDMA4) == 0)
            if(Close_Device(E_DRV_ID_PNL) == 0)
            if(stCfg.enSrc==E_DRV_SCLHVSP_IO_SRC_PAT_TGEN)
            sclprintf("close  over \n");
            if(stCfg.enSrc==E_DRV_SCLHVSP_IO_SRC_HVSP)
            {
                //_Close_Input_Tgen();
            }
            break;
        }
        else if(choise =='S')
        {
            //system("echo mem > /sys/power/state");
        }


    }


}
void Test_ASIC_CROP(CROP_CONFIG stCfg)
{
    u16 u16BufferIdx;
    u32 buffsize;
    DrvSclDmaIoBufferConfig_t stSCLDMACfg;
     DrvSclDmaIoActiveBufferConfig_t stSCLDMABFCfg;
    SclMemConfig_t stMsysMemInfo;
    DrvSclHvspIoScalingConfig_t stHvspScaleCfg;
    u16 BufNum;
    SclDmaPollConfig_t stScldmaPollCfg;
    SclThreadConfig_t  Scldma_Poll_Thread;
    SclThreadConfig_t  Mo_Thread;
    u32 choise = 54;

    Init_Device();

    if(Open_Device(E_DRV_ID_HVSP1) == 0)
    {
        return;
    }

    if(Open_Device(E_DRV_ID_HVSP2) == 0)
    {
        return;
    }


    if(Open_Device(E_DRV_ID_SCLDMA1) == 0)
    {
        return;
    }

    if(Open_Device(E_DRV_ID_SCLDMA4) == 0)
    {
        return;
    }

    if(Open_Device(E_DRV_ID_PNL) == 0)
    {
        return;
    }


    // PNL, Timing Configure
    _FPGA_Pnl(stCfg.u16Dsp_Width,stCfg.u16Dsp_Height);

    if(stCfg.enSrc==E_DRV_SCLHVSP_IO_SRC_ISP)
    {
        _ISP_Input(stCfg.u16Src_Width,stCfg.u16Src_Height);
    }
    if(stCfg.enSrc==E_DRV_SCLHVSP_IO_SRC_ISP)
    {
        _IspRun(NULL,NULL);
    }

    // chip top set
    //_FPGA_Chiptop();

    // HVSP1 Requenst IPM memory
    _Request_IPM(stCfg.u16Src_Width,stCfg.u16Src_Height);

    // HVSP1 in configure
    _HVSPInConfig(E_DRV_ID_HVSP1,stCfg.enSrc,stCfg.u16Src_Width,stCfg.u16Src_Height);

    // HVSP1 scaling configure
    memset(&stHvspScaleCfg, 0, sizeof(DrvSclHvspIoScalingConfig_t));
    stHvspScaleCfg = _FillHVSPScaleCfg(stCfg.u16Src_Width,stCfg.u16Src_Height,stCfg.u16Dsp_Width,stCfg.u16Dsp_Height);
    stHvspScaleCfg.stCropWin = _FillHVSPScaleCropCfg(stCfg.u16crop_Width,stCfg.u16crop_Height,stCfg.u16crop_X,stCfg.u16crop_Y);
    stHvspScaleCfg.bCropEn = 1;
    _HVSPScaling(E_DRV_ID_HVSP1,stHvspScaleCfg,stCfg.enSrc);

    // SCLDMA1 OUT BUFFER Configure
    memset(&stMsysMemInfo, 0, sizeof(SclMemConfig_t));
    buffsize = stCfg.u16Dsp_Width * stCfg.u16Dsp_Height ; // 2bpp
    stMsysMemInfo = _AllocateDmem("FGPA14DMA",(2),buffsize);
    if(stMsysMemInfo.pVirAddr == NULL)
    {
        sclprintf("Allocate Mem fail !!!!!");
        return;
    }


    BufNum = 2;


    stSCLDMACfg.enBufMDType = E_DRV_SCLDMA_IO_BUFFER_MD_RING;
    stSCLDMACfg.enMemType   = E_DRV_SCLDMA_IO_MEM_FRM;
    stSCLDMACfg.enColorType = E_DRV_SCLDMA_IO_COLOR_YUV420;
    stSCLDMACfg.u16Height   = stCfg.u16Dsp_Height;
    stSCLDMACfg.u16Width    = stCfg.u16Dsp_Width;
    stSCLDMACfg.u16BufNum    = BufNum;

    stSCLDMACfg.bvFlag.btsBase_0 = stSCLDMACfg.u16BufNum >=1 ? 1 : 0;
    stSCLDMACfg.bvFlag.btsBase_1 = stSCLDMACfg.u16BufNum >=2 ? 1 : 0;
    stSCLDMACfg.bvFlag.btsBase_2 = stSCLDMACfg.u16BufNum >=3 ? 1 : 0;
    stSCLDMACfg.bvFlag.btsBase_3 = stSCLDMACfg.u16BufNum >=4 ? 1 : 0;

    if (stSCLDMACfg.enColorType    == E_DRV_SCLDMA_IO_COLOR_YUV422)
    {
        for(u16BufferIdx=0; u16BufferIdx<stSCLDMACfg.u16BufNum ; u16BufferIdx++)
        {
            stSCLDMACfg.u32Base_Y[u16BufferIdx] = stMsysMemInfo.u32PhyAddr+ (buffsize) *2* u16BufferIdx;
            stSCLDMACfg.u32Base_C[u16BufferIdx] = stSCLDMACfg.u32Base_Y[u16BufferIdx]+16;
        }
    }
    else
    {
        for(u16BufferIdx=0; u16BufferIdx<stSCLDMACfg.u16BufNum ; u16BufferIdx++)
        {
            stSCLDMACfg.u32Base_Y[u16BufferIdx] = stMsysMemInfo.u32PhyAddr + (buffsize) * u16BufferIdx;
            stSCLDMACfg.u32Base_C[u16BufferIdx] = stMsysMemInfo.u32PhyAddr + (buffsize) * (u16BufferIdx+2);
        }
    }
    stSCLDMACfg = FILL_VERCHK_TYPE(stSCLDMACfg, stSCLDMACfg.VerChk_Version, stSCLDMACfg.VerChk_Size,DRV_SCLDMA_VERSION);
    DrvSclDmaIoSetOutBufferConfig(g_FD[E_DRV_ID_SCLDMA1], &stSCLDMACfg);


    // config DMA4 buffer
    memset(&stSCLDMACfg, 0, sizeof(DrvSclDmaIoBufferConfig_t));

    stSCLDMACfg.enBufMDType = E_DRV_SCLDMA_IO_BUFFER_MD_RING;
    stSCLDMACfg.enMemType   = E_DRV_SCLDMA_IO_MEM_FRM;
    stSCLDMACfg.enColorType = E_DRV_SCLDMA_IO_COLOR_YUV420;
    stSCLDMACfg.u16Height   = stCfg.u16Dsp_Height;
    stSCLDMACfg.u16Width    = stCfg.u16Dsp_Width;
    stSCLDMACfg.u16BufNum    = BufNum;

    stSCLDMACfg.bvFlag.btsBase_0 = stSCLDMACfg.u16BufNum >=1 ? 1 : 0;
    stSCLDMACfg.bvFlag.btsBase_1 = stSCLDMACfg.u16BufNum >=2 ? 1 : 0;
    stSCLDMACfg.bvFlag.btsBase_2 = stSCLDMACfg.u16BufNum >=3 ? 1 : 0;
    stSCLDMACfg.bvFlag.btsBase_3 = stSCLDMACfg.u16BufNum >=4 ? 1 : 0;
    if (stSCLDMACfg.enColorType    == E_DRV_SCLDMA_IO_COLOR_YUV422)
    {
        for(u16BufferIdx=0; u16BufferIdx<stSCLDMACfg.u16BufNum ; u16BufferIdx++)
        {

            stSCLDMACfg.u32Base_Y[u16BufferIdx] = stMsysMemInfo.u32PhyAddr + (buffsize) *2* u16BufferIdx;
            stSCLDMACfg.u32Base_C[u16BufferIdx] = stSCLDMACfg.u32Base_Y[u16BufferIdx]+16;
        }
    }
    else
    {
        for(u16BufferIdx=0; u16BufferIdx<stSCLDMACfg.u16BufNum; u16BufferIdx++)
        {
            stSCLDMACfg.u32Base_Y[u16BufferIdx] = stMsysMemInfo.u32PhyAddr + (buffsize) * u16BufferIdx;
            stSCLDMACfg.u32Base_C[u16BufferIdx] = stMsysMemInfo.u32PhyAddr + (buffsize) * (u16BufferIdx+2);
            sclprintf("[FPGA]stSCLDMACfg.u32Base_Y[%hd]=%lx\n",u16BufferIdx, stSCLDMACfg.u32Base_Y[u16BufferIdx]);
        }
    }
    stSCLDMACfg = FILL_VERCHK_TYPE(stSCLDMACfg, stSCLDMACfg.VerChk_Version, stSCLDMACfg.VerChk_Size,DRV_SCLDMA_VERSION);
    DrvSclDmaIoSetInBufferConfig(g_FD[E_DRV_ID_SCLDMA4], &stSCLDMACfg);


    // trig SCLDMA1
    _SetDMATrig(E_DRV_ID_SCLDMA1,E_DRV_SCLDMA_IO_MEM_FRM,1);
    _SetDMATrig(E_DRV_ID_SCLDMA1,E_DRV_SCLDMA_IO_MEM_IMI,1);
    // trig SCLDMA4
    _SetDMAinTrig(E_DRV_ID_SCLDMA4,E_DRV_SCLDMA_IO_MEM_FRM,1);
    stScldmaPollCfg.bEn = 1;
    stScldmaPollCfg.stGetActCfg.enMem[0]=E_DRV_SCLDMA_IO_MEM_FRM;
    stScldmaPollCfg.stGetActCfg.enID[0] = E_DRV_ID_SCLDMA1;
    stScldmaPollCfg.stGetActCfg.Count=1;
    Scldma_Poll_Thread = _CreateTestThread(_Scldma_Poll_Thread_GetActive,&stScldmaPollCfg);
    //fix warning
    Scldma_Poll_Thread=Scldma_Poll_Thread;
    stSCLDMABFCfg = _SetRPoint(E_DRV_ID_SCLDMA1,E_DRV_SCLDMA_IO_MEM_FRM,0x5);
    //fix warning
    stSCLDMABFCfg=stSCLDMABFCfg;
    Mo_Thread = _CreateMonitorThread(MonitorFunction, &choise);
    while(1)
    {
        MsSleep(1000);
        if(choise == 'Q'||choise == 'q')
        {
            MsSleep(10000);
            stScldmaPollCfg.bEn = 0;
            sclprintf("sleep over \n");
            memset(&stHvspScaleCfg, 0, sizeof(DrvSclHvspIoScalingConfig_t));
            stHvspScaleCfg = _FillHVSPScaleCfg(stCfg.u16Src_Width,stCfg.u16Src_Height,stCfg.u16Dsp_Width,stCfg.u16Dsp_Height);
            stHvspScaleCfg.stCropWin = _FillHVSPScaleCropCfg
                (stCfg.u16Src_Width,stCfg.u16Src_Height,stCfg.u16Src_Width,stCfg.u16Src_Height);
            stHvspScaleCfg.bCropEn = 0;
            _HVSPScaling(E_DRV_ID_HVSP1,stHvspScaleCfg,stCfg.enSrc);
            _SetDMATrig(E_DRV_ID_SCLDMA1,E_DRV_SCLDMA_IO_MEM_FRM,0);
            _SetDMATrig(E_DRV_ID_SCLDMA1,E_DRV_SCLDMA_IO_MEM_IMI,0);
            // trig SCLDMA4
            _SetDMAinTrig(E_DRV_ID_SCLDMA4,E_DRV_SCLDMA_IO_MEM_FRM,0);
            sclprintf("close  start \n");
            if(Close_Device(E_DRV_ID_HVSP1) == 0)
            if(Close_Device(E_DRV_ID_HVSP2) == 0)
            if(Close_Device(E_DRV_ID_SCLDMA1) == 0)
            if(Close_Device(E_DRV_ID_SCLDMA2) == 0)
            if(Close_Device(E_DRV_ID_SCLDMA4) == 0)
            if(Close_Device(E_DRV_ID_PNL) == 0)
            if(stCfg.enSrc==E_DRV_SCLHVSP_IO_SRC_PAT_TGEN)
            sclprintf("close  over \n");
            _DeleteTask(Scldma_Poll_Thread.TaskId ,Scldma_Poll_Thread.pStackTop);
            _DeleteTask(Mo_Thread.TaskId ,Mo_Thread.pStackTop);
            break;
        }
        else if(choise == 'D')
        {
            // HVSP1 scaling configure
            //memset(&stHvspScaleCfg, 0, sizeof(ST_IOCTL_HVSP_SCALING_CONFIG));
            stHvspScaleCfg = _FillHVSPScaleCfg(stCfg.u16Src_Width,stCfg.u16Src_Height,stCfg.u16Dsp_Width,stCfg.u16Dsp_Height);
            stHvspScaleCfg.stCropWin = _FillHVSPScaleCropCfg
                (stHvspScaleCfg.stCropWin.u16Width-stCfg.u16crop_Width/4,
                stHvspScaleCfg.stCropWin.u16Height-stCfg.u16crop_Height/4,
                stCfg.u16crop_X,stCfg.u16crop_Y);
            stHvspScaleCfg.bCropEn = 1;
            _HVSPScaling(E_DRV_ID_HVSP1,stHvspScaleCfg,stCfg.enSrc);
        }
        else if(choise == 'U')
        {
            // HVSP1 scaling configure
            //memset(&stHvspScaleCfg, 0, sizeof(ST_IOCTL_HVSP_SCALING_CONFIG));
            stHvspScaleCfg = _FillHVSPScaleCfg(stCfg.u16Src_Width,stCfg.u16Src_Height,stCfg.u16Dsp_Width,stCfg.u16Dsp_Height);
            stHvspScaleCfg.stCropWin = _FillHVSPScaleCropCfg
                (stHvspScaleCfg.stCropWin.u16Width+stCfg.u16crop_Width/4,
                stHvspScaleCfg.stCropWin.u16Height+stCfg.u16crop_Height/4,
                stCfg.u16crop_X,stCfg.u16crop_Y);
            stHvspScaleCfg.bCropEn = 1;
            _HVSPScaling(E_DRV_ID_HVSP1,stHvspScaleCfg,stCfg.enSrc);
        }
        else if(choise == 'S')
        {
            // HVSP1 scaling configure
            stHvspScaleCfg.bCropEn = 1;
            stHvspScaleCfg.stCropWin.u16X      = stCfg.u16crop_X+16;
            stHvspScaleCfg.stCropWin.u16Y      = stCfg.u16crop_Y;

            _HVSPScaling(E_DRV_ID_HVSP1,stHvspScaleCfg,stCfg.enSrc);

        }
        else if(choise == 'X')
        {
            // HVSP1 scaling configure
            stHvspScaleCfg.bCropEn = 1;
            stHvspScaleCfg.stCropWin.u16X      = stCfg.u16crop_X-16;
            stHvspScaleCfg.stCropWin.u16Y      = stCfg.u16crop_Y;
            _HVSPScaling(E_DRV_ID_HVSP1,stHvspScaleCfg,stCfg.enSrc);

        }
        else if(choise == 'Y')
        {
            // HVSP1 scaling configure
            stHvspScaleCfg.bCropEn = 1;
            stHvspScaleCfg.stCropWin.u16X      = stCfg.u16crop_X;
            stHvspScaleCfg.stCropWin.u16Y      = stCfg.u16crop_Y-16;
            _HVSPScaling(E_DRV_ID_HVSP1,stHvspScaleCfg,stCfg.enSrc);

        }
        else if(choise == 'L')
        {
            // HVSP1 scaling configure
            stHvspScaleCfg.bCropEn = 1;
            stHvspScaleCfg.stCropWin.u16X      = stCfg.u16crop_X;
            stHvspScaleCfg.stCropWin.u16Y      = stCfg.u16crop_Y+16;
            _HVSPScaling(E_DRV_ID_HVSP1,stHvspScaleCfg,stCfg.enSrc);

        }
        else if(choise=='M')
        {
            DrvSclHvspIoPriMaskConfig_t stPRICfg;
            DrvSclHvspIoPriMaskTriggerConfig_t stTrigger;
            stPRICfg.bMask = 1;
            stPRICfg.stMaskWin.u16Height = stCfg.u16Dsp_Height/4;
            stPRICfg.stMaskWin.u16Width  = stCfg.u16Dsp_Width/4;
            stPRICfg.stMaskWin.u16X      = stCfg.u16Dsp_Width/4;
            stPRICfg.stMaskWin.u16Y      = stCfg.u16Dsp_Height/4;
            stPRICfg = FILL_VERCHK_TYPE(stPRICfg, stPRICfg.VerChk_Version, stPRICfg.VerChk_Size,DRV_SCLHVSP_VERSION);
            DrvSclHvspIoSetPriMaskConfig(g_FD[E_DRV_ID_HVSP1], &stPRICfg);
            stPRICfg.bMask = 1;
            stPRICfg.stMaskWin.u16Height = stCfg.u16Dsp_Height/8;
            stPRICfg.stMaskWin.u16Width  = stCfg.u16Dsp_Width/2;
            stPRICfg.stMaskWin.u16X      = stCfg.u16Dsp_Width/4;
            stPRICfg.stMaskWin.u16Y      = stCfg.u16Dsp_Height/4*3;
            stPRICfg = FILL_VERCHK_TYPE(stPRICfg, stPRICfg.VerChk_Version, stPRICfg.VerChk_Size,DRV_SCLHVSP_VERSION);
            DrvSclHvspIoSetPriMaskConfig(g_FD[E_DRV_ID_HVSP1], &stPRICfg);
            stTrigger.bEn = 1;
            stTrigger = FILL_VERCHK_TYPE(stTrigger, stTrigger.VerChk_Version, stTrigger.VerChk_Size,DRV_SCLHVSP_VERSION);
            DrvSclHvspIoPirMaskTrigerConfig(g_FD[E_DRV_ID_HVSP1], &stTrigger);
        }
        else if(choise=='m')
        {
            DrvSclHvspIoPriMaskConfig_t stPRICfg;
            DrvSclHvspIoPriMaskTriggerConfig_t stTrigger;
            int idx;
            stPRICfg.bMask = 1;
            stPRICfg.stMaskWin.u16Height = 30;
            stPRICfg.stMaskWin.u16Width  = 30;
            stPRICfg.stMaskWin.u16X      = 0;
            stPRICfg.stMaskWin.u16Y      = 0;
            stPRICfg = FILL_VERCHK_TYPE(stPRICfg, stPRICfg.VerChk_Version, stPRICfg.VerChk_Size,DRV_SCLHVSP_VERSION);
            DrvSclHvspIoSetPriMaskConfig(g_FD[E_DRV_ID_HVSP1], &stPRICfg);
            stPRICfg.bMask = 1;
            stPRICfg.stMaskWin.u16Height = 30;
            stPRICfg.stMaskWin.u16Width  = 30;
            stPRICfg.stMaskWin.u16X      = 224;
            stPRICfg.stMaskWin.u16Y      = 448;
            stPRICfg = FILL_VERCHK_TYPE(stPRICfg, stPRICfg.VerChk_Version, stPRICfg.VerChk_Size,DRV_SCLHVSP_VERSION);
            DrvSclHvspIoSetPriMaskConfig(g_FD[E_DRV_ID_HVSP1], &stPRICfg);
            for(idx=1;idx<8;idx++)
            {
                stPRICfg.bMask = 1;
                stPRICfg.stMaskWin.u16Height = 30;
                stPRICfg.stMaskWin.u16Width  = 30;
                stPRICfg.stMaskWin.u16X      = 32*idx;
                stPRICfg.stMaskWin.u16Y      = 32*idx;
                stPRICfg = FILL_VERCHK_TYPE(stPRICfg, stPRICfg.VerChk_Version, stPRICfg.VerChk_Size,DRV_SCLHVSP_VERSION);
                DrvSclHvspIoSetPriMaskConfig(g_FD[E_DRV_ID_HVSP1], &stPRICfg);
            }
            stTrigger.bEn = 1;
            stTrigger = FILL_VERCHK_TYPE(stTrigger, stTrigger.VerChk_Version, stTrigger.VerChk_Size,DRV_SCLHVSP_VERSION);
            DrvSclHvspIoPirMaskTrigerConfig(g_FD[E_DRV_ID_HVSP1], &stTrigger);
        }
        else if(choise=='n')
        {
            DrvSclHvspIoPriMaskConfig_t stPRICfg;
            DrvSclHvspIoPriMaskTriggerConfig_t stTrigger;
            stPRICfg.bMask = 0;
            stPRICfg.stMaskWin.u16Height = stCfg.u16Dsp_Height/4;
            stPRICfg.stMaskWin.u16Width  = stCfg.u16Dsp_Width/4;
            stPRICfg.stMaskWin.u16X      = stCfg.u16Dsp_Width/4;
            stPRICfg.stMaskWin.u16Y      = stCfg.u16Dsp_Height/4;
            stPRICfg = FILL_VERCHK_TYPE(stPRICfg, stPRICfg.VerChk_Version, stPRICfg.VerChk_Size,DRV_SCLHVSP_VERSION);
            DrvSclHvspIoSetPriMaskConfig(g_FD[E_DRV_ID_HVSP1], &stPRICfg);
            stTrigger.bEn = 1;
            stTrigger = FILL_VERCHK_TYPE(stTrigger, stTrigger.VerChk_Version, stTrigger.VerChk_Size,DRV_SCLHVSP_VERSION);
            DrvSclHvspIoPirMaskTrigerConfig(g_FD[E_DRV_ID_HVSP1], &stTrigger);
        }
        else if(choise=='N')
        {
            DrvSclHvspIoPriMaskTriggerConfig_t stTrigger;
            stTrigger.bEn = 0;
            stTrigger = FILL_VERCHK_TYPE(stTrigger, stTrigger.VerChk_Version, stTrigger.VerChk_Size,DRV_SCLHVSP_VERSION);
            DrvSclHvspIoPirMaskTrigerConfig(g_FD[E_DRV_ID_HVSP1], &stTrigger);
        }
    }
}

int MultiInst_Func(void)
{
    #define SCLDMA_HANDLER_MAX  4
    #define SCLHVSP_HANDLER_MAX 3

    s32 s32SclDmaHandler[SCLDMA_HANDLER_MAX];
    s32 s32SclHvspHandler[SCLHVSP_HANDLER_MAX];
    u8  i;

    if(Init_Device() == FALSE)
    {
        return eCLI_PARSE_QUIT;
    }

    //Init_Debug_Msg();

    for(i=0; i<SCLHVSP_HANDLER_MAX; i++)
    {
        DrvSclHvspIoIdType_e enSclHvspId;

        enSclHvspId = ((i%3) == 0) ? E_DRV_SCLHVSP_IO_ID_1 :
                      ((i%3) == 1) ? E_DRV_SCLHVSP_IO_ID_2 :
                                     E_DRV_SCLHVSP_IO_ID_3;

        s32SclHvspHandler[i] =  DrvSclHvspIoOpen(enSclHvspId);
        if(s32SclHvspHandler[i] == -1)
        {
            sclprintf("OpenHvsp fail, i=%d, Id=%d\n", i, enSclHvspId);
            return eCLI_PARSE_QUIT;
        }
        else
        {
            sclprintf("Open i=%d, SclHvsp_%d %x\n", i,enSclHvspId, s32SclHvspHandler[i]);
        }
    }

    for(i=0; i<SCLDMA_HANDLER_MAX; i++)
    {
        DrvSclDmaIoIdType_e enSclDmaId;

        enSclDmaId = ((i%4) == 0) ? E_DRV_SCLDMA_IO_ID_1 :
                     ((i%4) == 1) ? E_DRV_SCLDMA_IO_ID_2 :
                     ((i%4) == 2) ? E_DRV_SCLDMA_IO_ID_3 :
                                    E_DRV_SCLDMA_IO_ID_4;

        s32SclDmaHandler[i] =  DrvSclDmaIoOpen(enSclDmaId);
        if(s32SclDmaHandler[i] == -1)
        {
            sclprintf("OpenDma fail, i=%d, Id=%d\n", i, enSclDmaId);
            return eCLI_PARSE_QUIT;
        }
        else
        {
            sclprintf("Open i=%d, SclDma_%d, %x\n", i, enSclDmaId, s32SclDmaHandler[i]);
        }

    }
    sclprintf("=================================================\n");
    sclprintf("GetPrivateData \n");
    sclprintf("=================================================\n");
    DrvSclHvspIoPrivateIdConfig_t stSclHvspPrivateCfg[2];

    for(i=0; i<2; i++)
    {
        if(DrvSclHvspIoGetPrivateIdConfig(s32SclHvspHandler[i], &stSclHvspPrivateCfg[i]) != E_DRV_SCLHVSP_IO_ERR_OK)
        {
            sclprintf("GetPrivateData fail \n");
            return eCLI_PARSE_QUIT;
        }
        else
        {
            sclprintf("SclHvsp Private_Id=%x\n", stSclHvspPrivateCfg[i].s32Id);
        }
    }

    DrvSclDmaIoPrivateIdConfig_t stSclDmaPrivateCfg[2];
    for(i=0; i<2; i++)
    {
        if(DrvSclDmaIoGetPrivateIdConfig(s32SclDmaHandler[i], &stSclDmaPrivateCfg[i]) == FALSE)
        {
            sclprintf("GetPrivateData fail \n");
            return eCLI_PARSE_QUIT;
        }
        else
        {
            sclprintf("SclDma Private_Ix=%x\n", stSclDmaPrivateCfg[i].s32Id);
        }
    }

    sclprintf("=================================================\n");
    sclprintf("Lock / Unlock \n");
    sclprintf("=================================================\n");
    DrvSclDmaIoLockConfig_t stSclDmaLockCfg_1;
    stSclDmaLockCfg_1.ps32IdBuf = DrvSclOsMemalloc(sizeof(s32)*4, 0);

    stSclDmaLockCfg_1.u8BufSize = 4;
    stSclDmaLockCfg_1.ps32IdBuf[0] = stSclDmaPrivateCfg[0].s32Id;
    stSclDmaLockCfg_1.ps32IdBuf[1] = stSclHvspPrivateCfg[0].s32Id;
    stSclDmaLockCfg_1.ps32IdBuf[2] = stSclDmaPrivateCfg[1].s32Id;
    stSclDmaLockCfg_1.ps32IdBuf[3] = stSclHvspPrivateCfg[1].s32Id;
    stSclDmaLockCfg_1 = FILL_VERCHK_TYPE(stSclDmaLockCfg_1, stSclDmaLockCfg_1.VerChk_Version, stSclDmaLockCfg_1.VerChk_Size,DRV_SCLDMA_VERSION);

    while(1)
    {
        if( DrvSclDmaIoSetLockConfig(s32SclDmaHandler[0], &stSclDmaLockCfg_1) )
        {
            sclprintf("LOCK SUCCESS\n");
            break;
        }
        else
        {
            sclprintf("SC_1_2 lock fail\n");
            MsSleep(RTK_MS_TO_TICK(2));
        }
    }

    if(DrvSclDmaIoSetUnlockConfig(s32SclDmaHandler[0], &stSclDmaLockCfg_1) != E_DRV_SCLDMA_IO_ERR_OK)
    {
        sclprintf("SC_1_2 unlock fail\n");
        return eCLI_PARSE_QUIT;
    }

    sclprintf("=================================================\n");
    sclprintf("Release \n");
    sclprintf("=================================================\n");

    for(i=0; i<SCLHVSP_HANDLER_MAX; i++)
    {
        if(DrvSclHvspIoRelease(s32SclHvspHandler[i]) != E_DRV_SCLHVSP_IO_ERR_OK)
        {
            sclprintf("DrvSclHvspIoRelease fail, i=%d \n", i);
            return eCLI_PARSE_QUIT;
        }
        else
        {
            sclprintf("Release SclHvsp i=%d\n", i);
        }
    }

    for(i=0; i<SCLDMA_HANDLER_MAX; i++)
    {
        if(DrvSclDmaIoRelease(s32SclDmaHandler[i]) != E_DRV_SCLDMA_IO_ERR_OK)
        {
            sclprintf("DrvSclDmaIoRelease fail, i=%d \n", i);
            return eCLI_PARSE_QUIT;
        }
        else
        {
            sclprintf("Release SclDma i=%d\n", i);
        }

    }
    return eCLI_PARSE_OK;
}

static void Test_SC1_2_Thread1_func(void *arg)
{
    DrvSclHvspIoInputConfig_t stInCfg;
    DrvSclHvspIoOutputConfig_t stOutCfg;
    DrvSclDmaIoBufferConfig_t stBufCfg;
    DrvSclDmaIoLockConfig_t   stSclmda_1LockCfg;
    DrvSclDmaIoPrivateIdConfig_t stScldma_1PrivateIdCfg, stScldma_2PrivateIdCfg;
    DrvSclHvspIoPrivateIdConfig_t stHvsp_1PrivateIdCfg, stHvsp_2PrivateIdCfg;
    unsigned char *bEn = (unsigned char *)arg;

    SclMemConfig_t stMsysMemInfo;
    u32  buffersize;
    s32 Hvsp1_Fd, Scldma1_Fd;
    s32 Hvsp2_Fd, Scldma2_Fd;

    u16 i;

    //-------------------------------------------------------------
    // Data Init
    //-------------------------------------------------------------
    stInCfg.enColor = E_DRV_SCLHVSP_IO_COLOR_YUV420;
    stInCfg.enSrcType = E_DRV_SCLHVSP_IO_SRC_ISP;
    stInCfg.stCaptureWin.u16X = 0;
    stInCfg.stCaptureWin.u16Y = 0;
    stInCfg.stCaptureWin.u16Width = 120;
    stInCfg.stCaptureWin.u16Height = 120;
    stInCfg.stTimingCfg.bInterlace = 0;
    stInCfg.stTimingCfg.u16Htotal = 858;
    stInCfg.stTimingCfg.u16Vfrequency = 600;
    stInCfg.stTimingCfg.u16Vtotal = 525;


    stOutCfg.enColor = E_DRV_SCLHVSP_IO_COLOR_YUV420;
    stOutCfg.stDisplayWin.u16X = 0;
    stOutCfg.stDisplayWin.u16Y = 0;
    stOutCfg.stDisplayWin.u16Width = 120;
    stOutCfg.stDisplayWin.u16Height = 120;
    stOutCfg.stTimingCfg.bInterlace = 0;
    stOutCfg.stTimingCfg.u16Htotal = 858;
    stOutCfg.stTimingCfg.u16Vfrequency = 600;
    stOutCfg.stTimingCfg.u16Vtotal = 525;

    //-------------------------------------------------------------
    // Memory alloc
    //-------------------------------------------------------------
    memset(&stBufCfg, 0 , sizeof(DrvSclDmaIoBufferConfig_t));
    stBufCfg.enColorType = E_DRV_SCLDMA_IO_COLOR_YUV420;
    stBufCfg.enMemType   = E_DRV_SCLDMA_IO_MEM_FRM;
    stBufCfg.enBufMDType = E_DRV_SCLDMA_IO_BUFFER_MD_SINGLE;
    stBufCfg.u16Height   = 160;
    stBufCfg.u16Width    = 160;
    stBufCfg.u16BufNum   = 1;

    memset(&stMsysMemInfo, 0, sizeof(SclMemConfig_t));
    buffersize = stBufCfg.u16Height * stBufCfg.u16Width * 2; //2bpp
    stMsysMemInfo = _AllocateDmem("SC1_2_MULTI_MEM_1",stBufCfg.u16BufNum,buffersize);

    if(stMsysMemInfo.pVirAddr == 0)
    {
        sclprintf("%s:++++++++++++++++ allocate memory fail ++++++++++++++++\n", __FUNCTION__);
        return;
    }

    for(i=0; i<stBufCfg.u16BufNum;  i++)
    {
        if(stBufCfg.enColorType == E_DRV_SCLDMA_IO_COLOR_YUV420)
        {
            stBufCfg.u32Base_Y[i] = stMsysMemInfo.u32PhyAddr + (buffersize * i);
            stBufCfg.u32Base_C[i] = stMsysMemInfo.u32PhyAddr + (stBufCfg.u16BufNum * buffersize) + (buffersize * i);
        }
        else
        {
            stBufCfg.u32Base_Y[i] = stMsysMemInfo.u32PhyAddr + (buffersize*2* i);
            stBufCfg.u32Base_C[i] = stBufCfg.u32Base_Y[i] + 16;
        }
    }

    stBufCfg.bvFlag.btsBase_0= stBufCfg.u16BufNum >=1 ? 1 : 0;
    stBufCfg.bvFlag.btsBase_1= stBufCfg.u16BufNum >=2 ? 1 : 0;
    stBufCfg.bvFlag.btsBase_2= stBufCfg.u16BufNum >=3 ? 1 : 0;
    stBufCfg.bvFlag.btsBase_3= stBufCfg.u16BufNum >=4 ? 1 : 0;


    //------------------------------------------------------------
    // OPEN
    //------------------------------------------------------------

    Hvsp1_Fd = DrvSclHvspIoOpen(E_DRV_SCLHVSP_IO_ID_1);
    Scldma1_Fd = DrvSclDmaIoOpen(E_DRV_SCLDMA_IO_ID_1);

    Hvsp2_Fd = DrvSclHvspIoOpen(E_DRV_SCLHVSP_IO_ID_2);
    Scldma2_Fd = DrvSclDmaIoOpen(E_DRV_SCLDMA_IO_ID_2);

    //------------------------------------------------------------
    // Get Private ID
    //------------------------------------------------------------
    DrvSclDmaIoGetPrivateIdConfig(Scldma1_Fd, &stScldma_1PrivateIdCfg);
    DrvSclHvspIoGetPrivateIdConfig(Hvsp1_Fd, &stHvsp_1PrivateIdCfg);

    DrvSclDmaIoGetPrivateIdConfig(Scldma2_Fd,  &stScldma_2PrivateIdCfg);
    DrvSclHvspIoGetPrivateIdConfig(Hvsp2_Fd,  &stHvsp_2PrivateIdCfg);

    //------------------------------------------------------------
    // Lock
    //------------------------------------------------------------

    stSclmda_1LockCfg.ps32IdBuf = DrvSclOsMemalloc(sizeof(signed long)*4, 0);

    if(stSclmda_1LockCfg.ps32IdBuf == NULL)
    {
    }

    stSclmda_1LockCfg.u8BufSize = 4;
    stSclmda_1LockCfg.ps32IdBuf[0] = stScldma_1PrivateIdCfg.s32Id;
    stSclmda_1LockCfg.ps32IdBuf[1] = stHvsp_1PrivateIdCfg.s32Id;
    stSclmda_1LockCfg.ps32IdBuf[2] = stScldma_2PrivateIdCfg.s32Id;
    stSclmda_1LockCfg.ps32IdBuf[3] = stHvsp_2PrivateIdCfg.s32Id;

    while(*bEn)
    {

        sclprintf("\n \n\n !!!!!!!!!!!!!!!!!! TEST_SC1_2_MULTI_1 START  !!!!!!!!!!!!!!!!!!\n");
        stSclmda_1LockCfg = FILL_VERCHK_TYPE(stSclmda_1LockCfg, stSclmda_1LockCfg.VerChk_Version, stSclmda_1LockCfg.VerChk_Size,DRV_SCLDMA_VERSION);
        while(DrvSclDmaIoSetLockConfig(Scldma1_Fd, &stSclmda_1LockCfg) != E_DRV_SCLDMA_IO_ERR_OK)
        {
            sclprintf("\n\n !!!!!!!!!!!!!!!!!!  SC1_2_Thread_1_LOCK Fail  !!!!!!!!!!!!!!!!!! \n\n");
            MsSleep(RTK_MS_TO_TICK(5));
        }


        //------------------------------------------------------------
        // IOCTL
        //------------------------------------------------------------
        stOutCfg = FILL_VERCHK_TYPE(stOutCfg, stOutCfg.VerChk_Version, stOutCfg.VerChk_Size,DRV_SCLHVSP_VERSION);
        stInCfg = FILL_VERCHK_TYPE(stInCfg, stInCfg.VerChk_Version, stInCfg.VerChk_Size,DRV_SCLHVSP_VERSION);
        stBufCfg = FILL_VERCHK_TYPE(stBufCfg, stBufCfg.VerChk_Version, stBufCfg.VerChk_Size,DRV_SCLDMA_VERSION);
        DrvSclHvspIoSetOutputConfig(Hvsp1_Fd, &stOutCfg);

        stInCfg.enSrcType = E_DRV_SCLHVSP_IO_SRC_ISP;
        DrvSclHvspIoSetInputConfig(Hvsp1_Fd, &stInCfg);

        DrvSclDmaIoSetInBufferConfig(Scldma1_Fd, &stBufCfg);
        DrvSclDmaIoSetOutBufferConfig(Scldma1_Fd, &stBufCfg);


        DrvSclHvspIoSetOutputConfig(Hvsp2_Fd, &stOutCfg);

        stInCfg.enSrcType = E_DRV_SCLHVSP_IO_SRC_HVSP;
        DrvSclHvspIoSetInputConfig(Hvsp1_Fd, &stInCfg);

        DrvSclHvspIoSetInputConfig(Hvsp2_Fd, &stInCfg);
        DrvSclDmaIoSetInBufferConfig(Scldma2_Fd, &stBufCfg);
        DrvSclDmaIoSetOutBufferConfig(Scldma2_Fd, &stBufCfg);

        //------------------------------------------------------------
        // UnLock
        //------------------------------------------------------------
        DrvSclDmaIoSetUnlockConfig(Scldma1_Fd, &stSclmda_1LockCfg);

        sclprintf("\n \n\n!!!!!!!!!!!!!!!!!! TEST_SC1_2_MULTI_1 END !!!!!!!!!!!!!!!!!!\n");
        MsSleep(RTK_MS_TO_TICK(2));
    }

    if(stSclmda_1LockCfg.ps32IdBuf)
    {
        DrvSclOsMemFree(stSclmda_1LockCfg.ps32IdBuf);
    }
}

static void Test_SC1_2_Thread2_func(void *arg)
{
    DrvSclHvspIoInputConfig_t stInCfg;
    DrvSclHvspIoOutputConfig_t stOutCfg;
    DrvSclDmaIoBufferConfig_t stBufCfg;
    DrvSclDmaIoLockConfig_t   stSclmda_1LockCfg;
    DrvSclDmaIoPrivateIdConfig_t stScldma_1PrivateIdCfg, stScldma_2PrivateIdCfg;
    DrvSclHvspIoPrivateIdConfig_t stHvsp_1PrivateIdCfg, stHvsp_2PrivateIdCfg;
    unsigned char *bEn = (unsigned char *)arg;

    SclMemConfig_t stMsysMemInfo;
    u32  buffersize;
    s32 Hvsp1_Fd, Scldma1_Fd;
    s32 Hvsp2_Fd, Scldma2_Fd;

    u16 i;

    //-------------------------------------------------------------
    // Data Init
    //-------------------------------------------------------------
    stInCfg.enColor = E_DRV_SCLHVSP_IO_COLOR_YUV420;
    stInCfg.enSrcType = E_DRV_SCLHVSP_IO_SRC_ISP;
    stInCfg.stCaptureWin.u16X = 0;
    stInCfg.stCaptureWin.u16Y = 0;
    stInCfg.stCaptureWin.u16Width = 120;
    stInCfg.stCaptureWin.u16Height = 120;
    stInCfg.stTimingCfg.bInterlace = 0;
    stInCfg.stTimingCfg.u16Htotal = 858;
    stInCfg.stTimingCfg.u16Vfrequency = 600;
    stInCfg.stTimingCfg.u16Vtotal = 525;


    stOutCfg.enColor = E_DRV_SCLHVSP_IO_COLOR_YUV420;
    stOutCfg.stDisplayWin.u16X = 0;
    stOutCfg.stDisplayWin.u16Y = 0;
    stOutCfg.stDisplayWin.u16Width = 120;
    stOutCfg.stDisplayWin.u16Height = 120;
    stOutCfg.stTimingCfg.bInterlace = 0;
    stOutCfg.stTimingCfg.u16Htotal = 858;
    stOutCfg.stTimingCfg.u16Vfrequency = 600;
    stOutCfg.stTimingCfg.u16Vtotal = 525;

    //-------------------------------------------------------------
    // Memory alloc
    //-------------------------------------------------------------
    memset(&stBufCfg, 0 , sizeof(DrvSclDmaIoBufferConfig_t));
    stBufCfg.enColorType = E_DRV_SCLDMA_IO_COLOR_YUV420;
    stBufCfg.enMemType   = E_DRV_SCLDMA_IO_MEM_FRM;
    stBufCfg.enBufMDType = E_DRV_SCLDMA_IO_BUFFER_MD_SINGLE;
    stBufCfg.u16Height   = 160;
    stBufCfg.u16Width    = 160;
    stBufCfg.u16BufNum   = 1;

    memset(&stMsysMemInfo, 0, sizeof(SclMemConfig_t));
    buffersize = stBufCfg.u16Height * stBufCfg.u16Width * 2; //2bpp
    stMsysMemInfo = _AllocateDmem("SC1_2_MULTI_MEM_2",stBufCfg.u16BufNum,buffersize);

    if(stMsysMemInfo.pVirAddr == 0)
    {
        sclprintf("%s:++++++++++++++++ allocate memory fail ++++++++++++++++\n", __FUNCTION__);
        return;
    }

    for(i=0; i<stBufCfg.u16BufNum;  i++)
    {
        if(stBufCfg.enColorType == E_DRV_SCLDMA_IO_COLOR_YUV420)
        {
            stBufCfg.u32Base_Y[i] = stMsysMemInfo.u32PhyAddr + (buffersize * i);
            stBufCfg.u32Base_C[i] = stMsysMemInfo.u32PhyAddr + (stBufCfg.u16BufNum * buffersize) + (buffersize * i);
        }
        else
        {
            stBufCfg.u32Base_Y[i] = stMsysMemInfo.u32PhyAddr + (buffersize*2* i);
            stBufCfg.u32Base_C[i] = stBufCfg.u32Base_Y[i] + 16;
        }
    }

    stBufCfg.bvFlag.btsBase_0= stBufCfg.u16BufNum >=1 ? 1 : 0;
    stBufCfg.bvFlag.btsBase_1= stBufCfg.u16BufNum >=2 ? 1 : 0;
    stBufCfg.bvFlag.btsBase_2= stBufCfg.u16BufNum >=3 ? 1 : 0;
    stBufCfg.bvFlag.btsBase_3= stBufCfg.u16BufNum >=4 ? 1 : 0;


    //------------------------------------------------------------
    // OPEN
    //------------------------------------------------------------

    Hvsp1_Fd = DrvSclHvspIoOpen(E_DRV_SCLHVSP_IO_ID_1);
    Scldma1_Fd = DrvSclDmaIoOpen(E_DRV_SCLDMA_IO_ID_1);

    Hvsp2_Fd = DrvSclHvspIoOpen(E_DRV_SCLHVSP_IO_ID_2);
    Scldma2_Fd = DrvSclDmaIoOpen(E_DRV_SCLDMA_IO_ID_2);

    //------------------------------------------------------------
    // Get Private ID
    //------------------------------------------------------------
    DrvSclDmaIoGetPrivateIdConfig(Scldma1_Fd, &stScldma_1PrivateIdCfg);
    DrvSclHvspIoGetPrivateIdConfig(Hvsp1_Fd, &stHvsp_1PrivateIdCfg);

    DrvSclDmaIoGetPrivateIdConfig(Scldma2_Fd,  &stScldma_2PrivateIdCfg);
    DrvSclHvspIoGetPrivateIdConfig(Hvsp2_Fd,  &stHvsp_2PrivateIdCfg);

    //------------------------------------------------------------
    // Lock
    //------------------------------------------------------------

    stSclmda_1LockCfg.ps32IdBuf = DrvSclOsMemalloc(sizeof(signed long)*4, 0);

    if(stSclmda_1LockCfg.ps32IdBuf == NULL)
    {
    }

    stSclmda_1LockCfg.u8BufSize = 4;
    stSclmda_1LockCfg.ps32IdBuf[0] = stScldma_1PrivateIdCfg.s32Id;
    stSclmda_1LockCfg.ps32IdBuf[1] = stHvsp_1PrivateIdCfg.s32Id;
    stSclmda_1LockCfg.ps32IdBuf[2] = stScldma_2PrivateIdCfg.s32Id;
    stSclmda_1LockCfg.ps32IdBuf[3] = stHvsp_2PrivateIdCfg.s32Id;

    while(*bEn)
    {

        sclprintf("\n \n\n !!!!!!!!!!!!!!!!!! TEST_SC1_2_MULTI_2 START  !!!!!!!!!!!!!!!!!!\n");
        stSclmda_1LockCfg = FILL_VERCHK_TYPE(stSclmda_1LockCfg, stSclmda_1LockCfg.VerChk_Version, stSclmda_1LockCfg.VerChk_Size,DRV_SCLDMA_VERSION);
        while(DrvSclDmaIoSetLockConfig(Scldma1_Fd, &stSclmda_1LockCfg) != E_DRV_SCLDMA_IO_ERR_OK)
        {
            sclprintf("\n\n !!!!!!!!!!!!!!!!!!  SC1_2_Thread_2_LOCK Fail  !!!!!!!!!!!!!!!!!! \n\n");
            MsSleep(RTK_MS_TO_TICK(5));
        }

        sclprintf("\n\n !!!!!!!!!!!!!!!!!!  SC1_2_Thread_2_LOCK SUCCESS  !!!!!!!!!!!!!!!!!! \n\n");

        //------------------------------------------------------------
        // IOCTL
        //------------------------------------------------------------
        stOutCfg = FILL_VERCHK_TYPE(stOutCfg, stOutCfg.VerChk_Version, stOutCfg.VerChk_Size,DRV_SCLHVSP_VERSION);
        stInCfg = FILL_VERCHK_TYPE(stInCfg, stInCfg.VerChk_Version, stInCfg.VerChk_Size,DRV_SCLHVSP_VERSION);
        stBufCfg = FILL_VERCHK_TYPE(stBufCfg, stBufCfg.VerChk_Version, stBufCfg.VerChk_Size,DRV_SCLDMA_VERSION);
        DrvSclHvspIoSetOutputConfig(Hvsp1_Fd, &stOutCfg);

        stInCfg.enSrcType = E_DRV_SCLHVSP_IO_SRC_ISP;
        DrvSclHvspIoSetInputConfig(Hvsp1_Fd, &stInCfg);

        DrvSclDmaIoSetInBufferConfig(Scldma1_Fd, &stBufCfg);
        DrvSclDmaIoSetOutBufferConfig(Scldma1_Fd, &stBufCfg);


        DrvSclHvspIoSetOutputConfig(Hvsp2_Fd, &stOutCfg);

        stInCfg.enSrcType = E_DRV_SCLHVSP_IO_SRC_HVSP;
        DrvSclHvspIoSetInputConfig(Hvsp1_Fd, &stInCfg);

        DrvSclHvspIoSetInputConfig(Hvsp2_Fd, &stInCfg);
        DrvSclDmaIoSetInBufferConfig(Scldma2_Fd, &stBufCfg);
        DrvSclDmaIoSetOutBufferConfig(Scldma2_Fd, &stBufCfg);

        //------------------------------------------------------------
        // UnLock
        //------------------------------------------------------------
        DrvSclDmaIoSetUnlockConfig(Scldma1_Fd, &stSclmda_1LockCfg);

        sclprintf("\n \n\n!!!!!!!!!!!!!!!!!! TEST_SC1_2_MULTI_2 END !!!!!!!!!!!!!!!!!!\n");
        MsSleep(RTK_MS_TO_TICK(3));
    }

    if(stSclmda_1LockCfg.ps32IdBuf)
    {
        DrvSclOsMemFree(stSclmda_1LockCfg.ps32IdBuf);
    }
}



static void Test_SC1_2_Thread3_func(void *arg)
{
    DrvSclHvspIoInputConfig_t stInCfg;
    DrvSclHvspIoOutputConfig_t stOutCfg;
    DrvSclDmaIoBufferConfig_t stBufCfg;
    DrvSclDmaIoLockConfig_t   stSclmda_1LockCfg;
    DrvSclDmaIoPrivateIdConfig_t stScldma_1PrivateIdCfg, stScldma_2PrivateIdCfg;
    DrvSclHvspIoPrivateIdConfig_t stHvsp_1PrivateIdCfg, stHvsp_2PrivateIdCfg;
    unsigned char *bEn = (unsigned char *)arg;

    SclMemConfig_t stMsysMemInfo;
    u32  buffersize;
    s32 Hvsp1_Fd, Scldma1_Fd;
    s32 Hvsp2_Fd, Scldma2_Fd;

    u16 i;

    //-------------------------------------------------------------
    // Data Init
    //-------------------------------------------------------------
    stInCfg.enColor = E_DRV_SCLHVSP_IO_COLOR_YUV420;
    stInCfg.enSrcType = E_DRV_SCLHVSP_IO_SRC_ISP;
    stInCfg.stCaptureWin.u16X = 0;
    stInCfg.stCaptureWin.u16Y = 0;
    stInCfg.stCaptureWin.u16Width = 120;
    stInCfg.stCaptureWin.u16Height = 120;
    stInCfg.stTimingCfg.bInterlace = 0;
    stInCfg.stTimingCfg.u16Htotal = 858;
    stInCfg.stTimingCfg.u16Vfrequency = 600;
    stInCfg.stTimingCfg.u16Vtotal = 525;


    stOutCfg.enColor = E_DRV_SCLHVSP_IO_COLOR_YUV420;
    stOutCfg.stDisplayWin.u16X = 0;
    stOutCfg.stDisplayWin.u16Y = 0;
    stOutCfg.stDisplayWin.u16Width = 120;
    stOutCfg.stDisplayWin.u16Height = 120;
    stOutCfg.stTimingCfg.bInterlace = 0;
    stOutCfg.stTimingCfg.u16Htotal = 858;
    stOutCfg.stTimingCfg.u16Vfrequency = 600;
    stOutCfg.stTimingCfg.u16Vtotal = 525;

    //-------------------------------------------------------------
    // Memory alloc
    //-------------------------------------------------------------
    memset(&stBufCfg, 0 , sizeof(DrvSclDmaIoBufferConfig_t));
    stBufCfg.enColorType = E_DRV_SCLDMA_IO_COLOR_YUV420;
    stBufCfg.enMemType   = E_DRV_SCLDMA_IO_MEM_FRM;
    stBufCfg.enBufMDType = E_DRV_SCLDMA_IO_BUFFER_MD_SINGLE;
    stBufCfg.u16Height   = 160;
    stBufCfg.u16Width    = 160;
    stBufCfg.u16BufNum   = 1;

    memset(&stMsysMemInfo, 0, sizeof(SclMemConfig_t));
    buffersize = stBufCfg.u16Height * stBufCfg.u16Width * 2; //2bpp
    stMsysMemInfo = _AllocateDmem("SC1_2_MULTI_MEM_3",stBufCfg.u16BufNum,buffersize);

    if(stMsysMemInfo.pVirAddr == 0)
    {
        sclprintf("%s:++++++++++++++++ allocate memory fail ++++++++++++++++\n", __FUNCTION__);
        return;
    }

    for(i=0; i<stBufCfg.u16BufNum;  i++)
    {
        if(stBufCfg.enColorType == E_DRV_SCLDMA_IO_COLOR_YUV420)
        {
            stBufCfg.u32Base_Y[i] = stMsysMemInfo.u32PhyAddr + (buffersize * i);
            stBufCfg.u32Base_C[i] = stMsysMemInfo.u32PhyAddr + (stBufCfg.u16BufNum * buffersize) + (buffersize * i);
        }
        else
        {
            stBufCfg.u32Base_Y[i] = stMsysMemInfo.u32PhyAddr + (buffersize*2* i);
            stBufCfg.u32Base_C[i] = stBufCfg.u32Base_Y[i] + 16;
        }
    }

    stBufCfg.bvFlag.btsBase_0= stBufCfg.u16BufNum >=1 ? 1 : 0;
    stBufCfg.bvFlag.btsBase_1= stBufCfg.u16BufNum >=2 ? 1 : 0;
    stBufCfg.bvFlag.btsBase_2= stBufCfg.u16BufNum >=3 ? 1 : 0;
    stBufCfg.bvFlag.btsBase_3= stBufCfg.u16BufNum >=4 ? 1 : 0;


    //------------------------------------------------------------
    // OPEN
    //------------------------------------------------------------

    Hvsp1_Fd = DrvSclHvspIoOpen(E_DRV_SCLHVSP_IO_ID_1);
    Scldma1_Fd = DrvSclDmaIoOpen(E_DRV_SCLDMA_IO_ID_1);

    Hvsp2_Fd = DrvSclHvspIoOpen(E_DRV_SCLHVSP_IO_ID_2);
    Scldma2_Fd = DrvSclDmaIoOpen(E_DRV_SCLDMA_IO_ID_2);

    //------------------------------------------------------------
    // Get Private ID
    //------------------------------------------------------------
    DrvSclDmaIoGetPrivateIdConfig(Scldma1_Fd, &stScldma_1PrivateIdCfg);
    DrvSclHvspIoGetPrivateIdConfig(Hvsp1_Fd, &stHvsp_1PrivateIdCfg);

    DrvSclDmaIoGetPrivateIdConfig(Scldma2_Fd,  &stScldma_2PrivateIdCfg);
    DrvSclHvspIoGetPrivateIdConfig(Hvsp2_Fd,  &stHvsp_2PrivateIdCfg);

    //------------------------------------------------------------
    // Lock
    //------------------------------------------------------------

    stSclmda_1LockCfg.ps32IdBuf = DrvSclOsMemalloc(sizeof(signed long)*4, 0);

    if(stSclmda_1LockCfg.ps32IdBuf == NULL)
    {
    }

    stSclmda_1LockCfg.u8BufSize = 4;
    stSclmda_1LockCfg.ps32IdBuf[0] = stScldma_1PrivateIdCfg.s32Id;
    stSclmda_1LockCfg.ps32IdBuf[1] = stHvsp_1PrivateIdCfg.s32Id;
    stSclmda_1LockCfg.ps32IdBuf[2] = stScldma_2PrivateIdCfg.s32Id;
    stSclmda_1LockCfg.ps32IdBuf[3] = stHvsp_2PrivateIdCfg.s32Id;

    while(*bEn)
    {

        sclprintf("\n \n\n !!!!!!!!!!!!!!!!!! TEST_SC1_2_MULTI_2 START  !!!!!!!!!!!!!!!!!!\n");
        stSclmda_1LockCfg = FILL_VERCHK_TYPE(stSclmda_1LockCfg, stSclmda_1LockCfg.VerChk_Version, stSclmda_1LockCfg.VerChk_Size,DRV_SCLDMA_VERSION);
#if 0
        while(DrvSclDmaIoSetLockConfig(Scldma1_Fd, &stSclmda_1LockCfg) != E_DRV_SCLDMA_IO_ERR_OK)
        {
            sclprintf("\n\n !!!!!!!!!!!!!!!!!!  SC1_2_Thread3_LOCK Fail  !!!!!!!!!!!!!!!!!! \n\n");
            MsSleep(RTK_MS_TO_TICK(5));
        }
#endif
        //------------------------------------------------------------
        // IOCTL
        //------------------------------------------------------------
        stOutCfg = FILL_VERCHK_TYPE(stOutCfg, stOutCfg.VerChk_Version, stOutCfg.VerChk_Size,DRV_SCLHVSP_VERSION);
        stInCfg = FILL_VERCHK_TYPE(stInCfg, stInCfg.VerChk_Version, stInCfg.VerChk_Size,DRV_SCLHVSP_VERSION);
        stBufCfg = FILL_VERCHK_TYPE(stBufCfg, stBufCfg.VerChk_Version, stBufCfg.VerChk_Size,DRV_SCLDMA_VERSION);
        DrvSclHvspIoSetOutputConfig(Hvsp1_Fd, &stOutCfg);

        stInCfg.enSrcType = E_DRV_SCLHVSP_IO_SRC_ISP;
        DrvSclHvspIoSetInputConfig(Hvsp1_Fd, &stInCfg);

        DrvSclDmaIoSetInBufferConfig(Scldma1_Fd, &stBufCfg);
        DrvSclDmaIoSetOutBufferConfig(Scldma1_Fd, &stBufCfg);


        DrvSclHvspIoSetOutputConfig(Hvsp2_Fd, &stOutCfg);

        stInCfg.enSrcType = E_DRV_SCLHVSP_IO_SRC_HVSP;
        DrvSclHvspIoSetInputConfig(Hvsp1_Fd, &stInCfg);

        DrvSclHvspIoSetInputConfig(Hvsp2_Fd, &stInCfg);
        DrvSclDmaIoSetInBufferConfig(Scldma2_Fd, &stBufCfg);
        DrvSclDmaIoSetOutBufferConfig(Scldma2_Fd, &stBufCfg);

        //------------------------------------------------------------
        // UnLock
        //------------------------------------------------------------
        //DrvSclDmaIoSetUnlockConfig(Scldma1_Fd, &stSclmda_1LockCfg);

        sclprintf("\n \n\n!!!!!!!!!!!!!!!!!! TEST_SC1_2_MULTI_3 END !!!!!!!!!!!!!!!!!!\n");
        MsSleep(3);
    }

    if(stSclmda_1LockCfg.ps32IdBuf)
    {
        DrvSclOsMemFree(stSclmda_1LockCfg.ps32IdBuf);
    }
}



void TEST_MULTI_INST(int test_combine)
{
    SclDmaPollConfig_t SC1_2_Multi_Thread_1, SC1_2_Multi_Thread_2, SC1_2_Multi_Thread_3;


    Init_Device();
    //Init_Debug_Msg();
    if(test_combine & E_MULTI_INST_SC1_2_THREAD_1)
    {
        _CreateTestThread(Test_SC1_2_Thread1_func, &SC1_2_Multi_Thread_1);
        MsSleep(1);
    }
    if(test_combine & E_MULTI_INST_SC1_2_THREAD_2)
    {
        _CreateTestThread(Test_SC1_2_Thread2_func, &SC1_2_Multi_Thread_2);
        MsSleep(1);
    }
    if(test_combine & E_MULTI_INST_SC1_2_THREAD_3)
    {
        _CreateTestThread(Test_SC1_2_Thread3_func, &SC1_2_Multi_Thread_3);
        MsSleep(1);
    }


    MsSleep(RTK_MS_TO_TICK(500000));
}


void Test_VIP_All(void)
{
    DrvSclVipIoAllSetConfig_t  stVipCfg;
    u32 buffsize;
    void *u64Vir_data;
    u32 i;
    if(Open_Device(E_DRV_ID_VIP) == 0)
    {
         return ;
    }
    memset(&stVipCfg,0xFF,sizeof(DrvSclVipIoAllSetConfig_t));
    buffsize = 512;//6
    u64Vir_data = DrvSclOsMemalloc(buffsize, 0);
    stVipCfg.stvipCfg.stmcnr.u32Viraddr = (u32)u64Vir_data;
    memset((void*)stVipCfg.stvipCfg.stmcnr.u32Viraddr ,0xFF,buffsize);
    for( i=E_DRV_SCLVIP_IO_AIP_YEE ;i<E_DRV_SCLVIP_IO_AIP_NUM ;i++)
    {
        u64Vir_data = DrvSclOsMemalloc(buffsize, 0);
        stVipCfg.stvipCfg.staip[i].enAIPType = i;
        stVipCfg.stvipCfg.staip[i].u32Viraddr = (u32)u64Vir_data;
        memset((void*)stVipCfg.stvipCfg.staip[i].u32Viraddr ,0xFF,buffsize);
    }
    stVipCfg = FILL_VERCHK_TYPE(stVipCfg, stVipCfg.VerChk_Version, stVipCfg.VerChk_Size,DRV_SCLVIP_VERSION);
    DrvSclVipIoSetAllVip(g_FD[E_DRV_ID_VIP], &stVipCfg);

    for( i=E_DRV_SCLVIP_IO_AIP_YEE ;i<E_DRV_SCLVIP_IO_AIP_NUM ;i++)
    {
        DrvSclOsMemFree((void*)stVipCfg.stvipCfg.staip[i].u32Viraddr);
    }
    DrvSclOsMemFree((void*)stVipCfg.stvipCfg.stmcnr.u32Viraddr);
}

void Test_VIP_LDC(VipLdcConfig_t stCfg)
{
    DrvSclVipIoLdcConfig_t stVIPCfg;
    DrvSclVipIoLdcDmapConfig_t stVIPDmapCfg;
    DrvSclVipIoLdcSramConfig_t stVIPSramCfg;
    //u32 i;
    u32 buffsize;
    u32           u64MIU_Addr;
    //u64  u64Vir_Addr,u64Vir_Addrhor,u64Vir_Addrver;
    //void *u64Vir_datammap;
    //void *u64Vir_data;
    //void *u64Vir_datahor;
    //void *u64Vir_dataver;
    //u32 u32viraddr;
    DrvSclHvspIoReqMemConfig_t stHvspReqMemCfg;
    SclMemConfig_t stMsysMemInfo;
    //char *pu8Buffer,*pu8Bufferhor,*pu8Bufferver;
    //long lFileSize;
    //char dmappath[] = "/mnt/usb/DumpDataLDC_DistMapMIUPack_0000.bin";
    //char horpath[] = "/mnt/usb/DumpDataLDC_Coeff_HorMIUPack_0000.bin";
    //char verpath[] = "/mnt/usb/DumpDataLDC_Coeff_VerMIUPack_0000.bin";
    //char dmappath[] = "/system/vendor/DumpDataLDC_DistMapMIUPack_0000_golden.bin";
    //char horpath[] = "/system/vendor/DumpDataLDC_Coeff_HorMIUPack_0000_golden.bin";
    //char verpath[] = "/system/vendor/DumpDataLDC_Coeff_VerMIUPack_0000_golden.bin";
    DrvSclHvspIoMiscConfig_t  stHvspMiscCfg;
    u8 input_tgen_buf[] ={
        0x18, 0x12, 0xE0, 0x01, 0xFF,
        0x18, 0x12, 0xE1, 0x00, 0xFF,};
    stHvspMiscCfg.u8Cmd = 0;
    stHvspMiscCfg.u32Size = sizeof(input_tgen_buf);
    stHvspMiscCfg.u32Addr = (unsigned long)input_tgen_buf;

    if(Open_Device(E_DRV_ID_HVSP1) == 0)
    {
        return;
    }
    if(Open_Device(E_DRV_ID_VIP) == 0)
    {
         return ;
    }


    sclprintf("Size=%d\n", stHvspMiscCfg.u32Size);
    // HVSP1 Requenst IPM memory
    if(stCfg.u16Width == 0 || stCfg.u16Height ==0)
    {
        sclprintf("skip ask buffer\n");
    }
    else
    {
        stHvspReqMemCfg.u16Pitch = stCfg.u16Width;
        stHvspReqMemCfg.u16Vsize = stCfg.u16Height;
        stHvspReqMemCfg.u32MemSize = stCfg.u16Width * stCfg.u16Height *2 * 2;
        DrvSclHvspIoReqmemConfig(g_FD[E_DRV_ID_HVSP1], &stHvspReqMemCfg);
    }
    //OnOff
    stVIPCfg.stEn.bLdc_path_sel=0x10; //on
    stVIPCfg.stEn.bEn_ldc=0x1;
    stVIPCfg.stmd.en422to444=E_DRV_SCLVIP_IO_LDC_422_444_QUARTER;
    stVIPCfg.stmd.en444to422=E_DRV_SCLVIP_IO_LDC_444_422_AVERAGE;
    stVIPCfg.stFCfg.bEn=stCfg.stFCfg.bEn;
    stVIPCfg.stFCfg.u8framecnt=stCfg.stFCfg.u8framecnt;
    // Dmap
    // DMAP BUFFER Configure
    memset(&stMsysMemInfo, 0, sizeof(SclMemConfig_t));
	buffsize = 256* 256;
    stMsysMemInfo = _AllocateDmem("FGPALDCDMAP",1,buffsize);
    if(stMsysMemInfo.pVirAddr== 0)
    {
        sclprintf("Allocate Mem fail !!!!!\n");
        return;
    }

    //u64Vir_Addr    = (u32)stMsysMemInfo.pVirAddr;
    //u64Vir_Addrhor = (u32)stMsysMemInfo.pVirAddr+buffsize;
    //u64Vir_Addrver = (u32)stMsysMemInfo.pVirAddr+buffsize+(buffsize/2);
    //sclprintf("u64Vir_Addr:%llx u64Vir_Addrhor:%llx u64Vir_Addrver:%llx u64MIU_Addr:%llx!!!!!\n",u64Vir_Addr,u64Vir_Addrhor,u64Vir_Addrver,u64MIU_Addr.addr);
    //u64Vir_data   = mmap(NULL,stMsysMemInfo.length/2,PROT_READ|PROT_WRITE,MAP_SHARED,g_FD[E_DRV_ID_MEM],stMsysMemInfo.phys);
    //u64Vir_datahor= mmap(NULL,stMsysMemInfo.length/4,PROT_READ|PROT_WRITE,MAP_SHARED,g_FD[E_DRV_ID_MEM],stMsysMemInfo.phys+buffsize);
    //u64Vir_dataver= mmap(NULL,stMsysMemInfo.length/4,PROT_READ|PROT_WRITE,MAP_SHARED,g_FD[E_DRV_ID_MEM],stMsysMemInfo.phys+buffsize+(buffsize/2));
    //sclprintf("u64Vir_data:%llx \n",u64Vir_data);
    //sclprintf("u64Vir_datahor:%llx\n",u64Vir_datahor);
    //sclprintf("u64Vir_dataver:%llx \n",u64Vir_dataver);

    u64MIU_Addr = stMsysMemInfo.u32MIUAddr;

    //_ReadFile(&u64Vir_data, &lFileSize,dmappath);
    //_ReadFile(&u64Vir_datahor, &lFileSize,horpath);
    //_ReadFile(&u64Vir_dataver, &lFileSize,verpath);
    //Dmap
    stVIPDmapCfg.bEnPowerSave=0;
    stVIPDmapCfg.stFCfg.bEn=stCfg.stFCfg.bEn;
    stVIPDmapCfg.stFCfg.u8framecnt=stCfg.stFCfg.u8framecnt;
    stVIPDmapCfg.u16DMAPWidth=stCfg.u16Width;
    stVIPDmapCfg.u32DMAPaddr=u64MIU_Addr;
    stVIPDmapCfg.u8DMAPoffset=0;
    //sram
    stVIPSramCfg.stFCfg.bEn=stCfg.stFCfg.bEn;
    stVIPSramCfg.stFCfg.u8framecnt=stCfg.stFCfg.u8framecnt;
    stVIPSramCfg.u16SRAMhoramount=0x107;
    stVIPSramCfg.u16SRAMveramount=0x107;
    stVIPSramCfg.u16SRAMhorstr=0;
    stVIPSramCfg.u16SRAMverstr=0;
    stVIPSramCfg.u32loadhoraddr=u64MIU_Addr+buffsize;
    stVIPSramCfg.u32loadveraddr=u64MIU_Addr+buffsize+(buffsize/2);
    stVIPDmapCfg = FILL_VERCHK_TYPE(stVIPDmapCfg, stVIPDmapCfg.VerChk_Version, stVIPDmapCfg.VerChk_Size,DRV_SCLVIP_VERSION);
    stVIPSramCfg = FILL_VERCHK_TYPE(stVIPSramCfg, stVIPSramCfg.VerChk_Version, stVIPSramCfg.VerChk_Size,DRV_SCLVIP_VERSION);
    stVIPCfg = FILL_VERCHK_TYPE(stVIPCfg, stVIPCfg.VerChk_Version, stVIPCfg.VerChk_Size,DRV_SCLVIP_VERSION);
    DrvSclVipIoSetLdcDmapConfig(g_FD[E_DRV_ID_VIP], &stVIPDmapCfg);
    DrvSclVipIoSetLdcSramConfig(g_FD[E_DRV_ID_VIP], &stVIPSramCfg);
    DrvSclVipIoSetLdcConfig(g_FD[E_DRV_ID_VIP], &stVIPCfg);
    /*
    input_tgen_buf[3]=0x0;
    stHvspMiscCfg.u8Cmd = 0;
    stHvspMiscCfg.u32Size = sizeof(input_tgen_buf);
    stHvspMiscCfg.u32Addr = (unsigned long)input_tgen_buf;
    sleep(1);
    ioctl(g_FD[E_DRV_ID_HVSP1], IOCTL_SCLHVSP_SET_MISC_CONFIG, &stHvspMiscCfg);
    */
}


void Test_VIP_MCNR(VipDnrConfig_t stCfg)
{
    DrvSclVipIoMcnrConfig_t stVIPCfg;
    //u16 i;
    u32 buffsize;
    void *u64Vir_data;
    //DrvSclHvspIoReqMemConfig_t  stHvspReqMemCfg;
    DrvSclHvspIoMiscConfig_t  stHvspMiscCfg;
    u8 input_tgen_buf[] ={
        0x18, 0x12, 0xE0, 0x01, 0xFF,
        0x18, 0x12, 0xE1, 0x00, 0xFF,};
    stHvspMiscCfg.u8Cmd = 0;
    stHvspMiscCfg.u32Size = sizeof(input_tgen_buf);
    stHvspMiscCfg.u32Addr = (u32)input_tgen_buf;
    if(Open_Device(E_DRV_ID_VIP) == 0)
    {
         return ;
    }

    if(Open_Device(E_DRV_ID_HVSP1) == 0)
    {
        return;
    }
    // HVSP1 Requenst IPM memory
    if(stCfg.u16Width == 0 || stCfg.u16Height ==0)
    {
        sclprintf("skip ask buffer\n");
    }
    else
    {
        _Request_IPM(stCfg.u16Width,stCfg.u16Height);
        DrvSclHvspIoSetMiscConfig(g_FD[E_DRV_ID_HVSP1], &stHvspMiscCfg );
    }
    //
    stVIPCfg.stFCfg.bEn=stCfg.stFCfg.bEn;
    stVIPCfg.stFCfg.u8framecnt=stCfg.stFCfg.u8framecnt;
    buffsize = 256;//6
    u64Vir_data = DrvSclOsMemalloc(buffsize, 0);
    stVIPCfg.u32Viraddr = (u32)u64Vir_data;
    stVIPCfg.bEnCIIR = 0;
    stVIPCfg.bEnMCNR = 1;
    sclprintf("[%s]Vir:%lx,Vir:%lx\n", __FUNCTION__, (u32)u64Vir_data,stVIPCfg.u32Viraddr);

    memset(u64Vir_data,0x99,buffsize);

    stVIPCfg = FILL_VERCHK_TYPE(stVIPCfg, stVIPCfg.VerChk_Version, stVIPCfg.VerChk_Size,DRV_SCLVIP_VERSION);
    DrvSclVipIoSetMcnrConfig(g_FD[E_DRV_ID_VIP], &stVIPCfg);

    DrvSclOsMemFree(u64Vir_data);

}

void Test_VIP_PK(VipPkConfig_t stCfg)
{
    DrvSclVipIoPeakingConfig_t stVIPCfg;
    u32 i;
    u8 u8Band_Under_0[DRV_SCLVIP_IO_PEAKING_BAND_NUM]={1,2,4,8,16,32,64,128};
    u8 u8Band_Under_1[DRV_SCLVIP_IO_PEAKING_BAND_NUM]={0,0,0,0,0,0,0,0};
    u8 u8Band_Under_2[DRV_SCLVIP_IO_PEAKING_BAND_NUM]={48,48,48,48,48,48,48,48};
    u8 u8Band_Under_3[DRV_SCLVIP_IO_PEAKING_BAND_NUM]={16,16,16,16,16,16,16,16};
    u8 u8Band_Under_4[DRV_SCLVIP_IO_PEAKING_BAND_NUM]={16,16,16,16,16,16,16,16};
    u8 u8Band_Under_5[DRV_SCLVIP_IO_PEAKING_BAND_NUM]={8,128,8,128,8,128,8,128};
    u8 u8Band_Under_6[DRV_SCLVIP_IO_PEAKING_BAND_NUM]={1,2,4,8,16,32,64,128};
    u8 u8Band_Under_7[DRV_SCLVIP_IO_PEAKING_BAND_NUM]={15,3,3,6,7,10,12,15};
    u8 u8Band_Under_8[DRV_SCLVIP_IO_PEAKING_BAND_NUM]={3,64,4,96,0,128,8,128};
    u8 u8Band_Filter_Term_0[DRV_SCLVIP_IO_PEAKING_BAND_TERM_NUM]={0x0,0x10,0x2,0x30,0x0,0x10,0x2,0x30,0x4,0x40,0x8,0x60,0x6,0x70,0x7,0x70};
    u8 u8Band_Filter_Term_1[DRV_SCLVIP_IO_PEAKING_BAND_TERM_NUM]={54,52,50,48,46,44,42,40,38,36,34,32,30,28,26,24};
    u8 u8Band_Filter_Term_2[DRV_SCLVIP_IO_PEAKING_BAND_TERM_NUM]={54,52,50,48,46,44,42,40,38,36,34,32,30,28,26,24};
    u8 u8Band_Filter_Term_3[DRV_SCLVIP_IO_PEAKING_BAND_TERM_NUM]={54,52,50,48,46,44,42,40,38,36,34,32,30,28,26,24};
    u8 u8Band_Filter_Term_4[DRV_SCLVIP_IO_PEAKING_BAND_TERM_NUM]={8,64,7,96,3,96,4,0,4,96,4,96,4,96,4,96};
    if(Open_Device(E_DRV_ID_VIP) == 0)
    {
         return ;
    }
    stVIPCfg.stFCfg.bEn=stCfg.stFCfg.bEn;
    stVIPCfg.stFCfg.u8framecnt=stCfg.stFCfg.u8framecnt;
    stVIPCfg.stHLPF.u8main_y_lpf_coef=64;
    stVIPCfg.stHLPFDith.hlpf_dither_en=0;
    stVIPCfg.stVLPFcoef1.main_v_lpf_coef=4;
    stVIPCfg.stVLPFcoef2.main_v_lpf_coef=64;
    stVIPCfg.stVLPFDith.vlpf_dither_en=0;

    stVIPCfg.stOnOff.bpost_peaking_en=1;
    stVIPCfg.stOnOff.u8vps_sram_act=128;
    stVIPCfg.stOnOff.u8band6_dia_filter_sel=64;

    for(i=0;i<DRV_SCLVIP_IO_PEAKING_BAND_NUM;i++)
    {
        stVIPCfg.stBand.bBand_En[i]=u8Band_Under_0[i];
        stVIPCfg.stBand.u8Band_COEF_STEP[i]=u8Band_Under_1[i];
        stVIPCfg.stBand.u8Band_COEF[i]=u8Band_Under_2[i];
        stVIPCfg.stBand.u8Band_Over[i]=u8Band_Under_3[i];
        stVIPCfg.stBand.u8Band_Under[i]=u8Band_Under_4[i];
        stVIPCfg.stBand.u8Band_coring_thrd[i]=u8Band_Under_5[i];

        stVIPCfg.stAdp.badaptive_en[i]=u8Band_Under_6[i];

        stVIPCfg.stAdpY.u8coring_adp_y_alpha_lut[i]=u8Band_Under_7[i];

        stVIPCfg.stGainAdpY.u8pk_adp_y_alpha_lut[i]=u8Band_Under_8[i];
    }
    stVIPCfg.stBand.u8alpha_thrd=0xFF;

    for(i=0;i<DRV_SCLVIP_IO_PEAKING_BAND_TERM_NUM;i++)
    {
        stVIPCfg.stBand.u8peaking_term[i]=u8Band_Filter_Term_0[i];
        stVIPCfg.stAdp.u8hor_lut[i]=u8Band_Filter_Term_1[i];
        stVIPCfg.stAdp.u8ver_lut[i]=u8Band_Filter_Term_2[i];
        stVIPCfg.stAdp.u8dia_lut[i]=u8Band_Filter_Term_3[i];
        stVIPCfg.stAdp.u8low_diff_thrd_and_adaptive_gain_step[i]=u8Band_Filter_Term_4[i];
    }

    stVIPCfg.stPcor.u8coring_thrd_1=1;
    stVIPCfg.stPcor.u8coring_thrd_2=48;
    stVIPCfg.stPcor.u8coring_thrd_step=0;

    stVIPCfg.stAdpY.bcoring_adp_y_en=1;
    stVIPCfg.stAdpY.bcoring_adp_y_alpha_lpf_en=2;
    stVIPCfg.stAdpY.u8coring_y_low_thrd=133;
    stVIPCfg.stAdpY.u8coring_adp_y_step=48;

    stVIPCfg.stGain.u8osd_sharpness_ctrl=16;
    stVIPCfg.stGain.bosd_sharpness_sep_hv_en=0;
    stVIPCfg.stGain.u8osd_sharpness_ctrl_h=24;
    stVIPCfg.stGain.u8osd_sharpness_ctrl_v=24;

    stVIPCfg.stGainAdpY.bpk_adp_y_en=0;
    stVIPCfg.stGainAdpY.bpk_adp_y_alpha_lpf_en=2;
    stVIPCfg.stGainAdpY.u8pk_y_low_thrd=0;
    stVIPCfg.stGainAdpY.u8pk_adp_y_step=32;
    stVIPCfg = FILL_VERCHK_TYPE(stVIPCfg, stVIPCfg.VerChk_Version, stVIPCfg.VerChk_Size,DRV_SCLVIP_VERSION);
    DrvSclVipIoSetPeakingConfig(g_FD[E_DRV_ID_VIP], &stVIPCfg);
}

void Test_VIP_Hist(VipDlcConfig_t stCfg)
{
    DrvSclVipIoDlcHistogramConfig_t stVIPhistCfg;
    DrvSclVipIoDlcHistogramReport_t stVIPhistRep;
    unsigned long buffsize;
    u32           u64MIU_Addr;
    unsigned long i;
    SclMemConfig_t stMsysMemInfo;
    u8 *u64Vir_data=NULL;
    //u8 u8hist_val1[256];
    //u8 u8hist_val2[256];

    //char savename[] = "/system/vendor/DLChist.bin";
    //char savename2[] = "/system/vendor/DLChist2.bin";
    //FILE *pFile = NULL;
    //FILE *pFile2 = NULL;

    if(Open_Device(E_DRV_ID_VIP) == 0)
    {
         return ;
    }


    //CMDQ
    stVIPhistCfg.stFCfg.bEn=stCfg.stFCfg.bEn;
    stVIPhistCfg.stFCfg.u8framecnt=stCfg.stFCfg.u8framecnt;
    //allocate kernel memory
    memset(&stMsysMemInfo, 0, sizeof(SclMemConfig_t));
    buffsize = 256*2; // 1bpp
    stMsysMemInfo = _AllocateDmem("DLC_MEM",1,buffsize);
    if(stMsysMemInfo.pVirAddr== 0)
    {
        sclprintf("Allocate Mem fail !!!!!\n");
        return;
    }
    u64MIU_Addr = stMsysMemInfo.u32MIUAddr;

    //HISTOGRAM
    DrvSclVipIoGetDlcHistogramConfig(g_FD[E_DRV_ID_VIP], &stVIPhistRep);
    sclprintf("u32PixelCount:%lx,u32PixelWeight:%lx,u8Baseidx:%hhx \n", stVIPhistRep.u32PixelCount,stVIPhistRep.u32PixelWeight,stVIPhistRep.u8Baseidx);
    for(i=0;i<DRV_SCLVIP_IO_DLC_HISTOGRAM_SECTION_NUM;i++)
    {
        stVIPhistCfg.u8Histogram_Range[i]= stVIPhistRep.u32Histogram[i];
    }
    //tb1
    stVIPhistCfg.bVariable_Section=1;
    stVIPhistCfg.bcurve_fit_en = 0;
    stVIPhistCfg.bcurve_fit_rgb_en = 0;
    stVIPhistCfg.bDLCdither_en = 0;
    stVIPhistCfg.bhis_y_rgb_mode_en = 0;
    stVIPhistCfg.bstatic = 1;
    //tb2
    stVIPhistCfg.bRange = 1;
    stVIPhistCfg.u16Vst = 6;//display st =1*8
    stVIPhistCfg.u16Hst = 6;//display st =1*8
    stVIPhistCfg.u16Vnd = 8;//display st =60*8=480
    stVIPhistCfg.u16Hnd = 8;//display st =100*8 =800
    //tb3
    stVIPhistCfg.bstat_MIU = 1;
    stVIPhistCfg.u8HistSft = 0;
    stVIPhistCfg.u8trig_ref_mode = 1;
    stVIPhistCfg.u32StatBase[0] = u64MIU_Addr;
    stVIPhistCfg.u32StatBase[1] = u64MIU_Addr+buffsize;
    stVIPhistCfg = FILL_VERCHK_TYPE(stVIPhistCfg, stVIPhistCfg.VerChk_Version, stVIPhistCfg.VerChk_Size,DRV_SCLVIP_VERSION);
    DrvSclVipIoSetDlcHistogramConfig(g_FD[E_DRV_ID_VIP], &stVIPhistCfg);

    u64Vir_data   =  stMsysMemInfo.pVirAddr;
    //memcpy(u8hist_val1, u64Vir_data, buffsize);
    //memcpy(u8hist_val2, u64Vir_data+buffsize, buffsize);

    sclprintf("Mem  u64Vir_data:%lx!\n",u64Vir_data);
    //pFile=_OpenFile(savename);
    //_SaveFile(pFile,stMsysMemInfo,(buffsize),0,u64Vir_data);
    //pFile2=_OpenFile(savename2);
    //_SaveFile(pFile2,stMsysMemInfo,(buffsize),buffsize,u64Vir_data);
    //munmap(u64Vir_data,buffsize*2);
    /*
    while(1)
    {
        ioctl(g_FD[E_DRV_ID_VIP], IOCTL_SCLVIP_GET_DLC_HISTOGRAM_REPORT, &stVIPhistRep);
        sclprintf("u32PixelCount:%lx,u32PixelWeight:%lx,u8Baseidx:%hhx \n", stVIPhistRep.u32PixelCount,stVIPhistRep.u32PixelWeight,stVIPhistRep.u8Baseidx);
        sleep(2);
    }
    */
}


DrvSclVipIoNlmSramConfig_t Test_VIP_NLM_Autodownload(unsigned char value)
{
    DrvSclVipIoNlmSramConfig_t stSRAM;

    u32 buffsize;
    //u64           u64MIU_Addr;
    u32 i;
    SclMemConfig_t stMsysMemInfo;
    u8 *u64Vir_data=NULL;

    memset(&stSRAM, 0x00, sizeof(DrvSclVipIoNlmSramConfig_t));
    if(Open_Device(E_DRV_ID_VIP) == 0)
    {
        sclprintf("open vip fail\n");
         return  stSRAM;
    }

    sclprintf("Test_VIP_NLM_Autodownload!!!!!\n");
    //allocate kernel memory
    memset(&stMsysMemInfo, 0, sizeof(SclMemConfig_t));
    buffsize = 1104*16; // 1bpp
    stMsysMemInfo = _AllocateDmem("NLM_MEM",1,buffsize/2);
    if(stMsysMemInfo.pVirAddr == NULL)
    {
        sclprintf("Allocate Mem fail !!!!!\n");
        return  stSRAM;
    }

    //Auto download setting
    stSRAM.u32Baseadr = stMsysMemInfo.u32MIUAddr;
    stSRAM.bEn =1;
    stSRAM.u32viradr = (u32)stMsysMemInfo.pVirAddr;

    sclprintf("u32viradr=%lx\n",stSRAM.u32viradr);
    u64Vir_data = (u8 *)stMsysMemInfo.pVirAddr;
    // put value
    if(value)
    {
        for(i=0;i<500;i++)
        {
            //memset(u64Vir_data+(i*16)+0,(value|(value<<5)),1);
            //memset(u64Vir_data+(i*16)+1,(value>>3|(value<<2)),1);
            //memset(u64Vir_data+(i*16)+2,(value>>1),1);
            memset(u64Vir_data+(i*16)+0,0x10,1);
            memset(u64Vir_data+(i*16)+1,0x42,1);
            memset(u64Vir_data+(i*16)+2,0x08,1);
            memset(u64Vir_data+(i*16)+3,0x00,13);
        }
        memset(u64Vir_data+500*16,0,buffsize-500*16);
    }
    else
    {
        memset(u64Vir_data,0,buffsize);
    }
    return stSRAM;
}

void Test_VIP_DLC(VipDlcConfig_t stCfg)
{
    //DrvSclVipIoDlcHistogramConfig_t stVIPhistCfg;
    //DrvSclVipIoDlcHistogramReport_t stVIPhistRep;
    DrvSclVipIoDlcConfig_t stVIPDLCCfg;
    //u32 buffsize;
    //u64           u64MIU_Addr;
    u32 i;
    //SclMemConfig_t stMsysMemInfo;

    if(Open_Device(E_DRV_ID_VIP) == 0)
    {
         return ;
    }

    //CMDQ
    //stVIPhistCfg.stFCfg.bEn=stCfg.stFCfg.bEn;
    //stVIPhistCfg.stFCfg.u8framecnt=stCfg.stFCfg.u8framecnt;
    stVIPDLCCfg.stFCfg.bEn=stCfg.stFCfg.bEn;
    stVIPDLCCfg.stFCfg.u8framecnt=stCfg.stFCfg.u8framecnt;
    //allocate kernel memory
    /*
    memset(&stMsysMemInfo, 0, sizeof(MSYS_DMEM_INFO));
    strcpy(stMsysMemInfo.name, "DLC_MEM");
    ioctl(g_FD[E_DRV_ID_MSYS], IOCTL_MSYS_RELEASE_DMEM, &stMsysMemInfo);

    buffsize = 256; // 1bpp
    stMsysMemInfo.length =  buffsize * 2; // 2 buffer
    ioctl(g_FD[E_DRV_ID_MSYS], IOCTL_MSYS_REQUEST_DMEM, &stMsysMemInfo);
    if(stMsysMemInfo.kvirt == 0)
    {
        sclprintf("Allocate Mem fail !!!!!\n");
        return;
    }
    u64MIU_Addr = stMsysMemInfo.phys;
    ioctl(g_FD[E_DRV_ID_MSYS], IOCTL_MSYS_PHYS_TO_MIU, &u64MIU_Addr);

    //HISTOGRAM
    ioctl(g_FD[E_DRV_ID_VIP], IOCTL_SCLVIP_GET_DLC_HISTOGRAM_REPORT, &stVIPhistRep);
    sclprintf("u32PixelCount:%lx,u32PixelWeight:%lx,u8Baseidx:%hhx \n", stVIPhistRep.u32PixelCount,stVIPhistRep.u32PixelWeight,stVIPhistRep.u8Baseidx);
    for(i=0;i<DRV_SCLVIP_IO_DLC_HISTOGRAM_SECTION_NUM;i++)
    {
        stVIPhistCfg.u8Histogram_Range[i]= stVIPhistRep.u32Histogram[i];
    }
    stVIPhistCfg.bVariable_Section=stCfg.bVariable_Section;
    stVIPhistCfg.bstat_MIU = 0;
    stVIPhistCfg.bcurve_fit_en = 0;
    stVIPhistCfg.bcurve_fit_rgb_en = 0;
    stVIPhistCfg.bDLCdither_en = 0;
    stVIPhistCfg.bhis_y_rgb_mode_en = 0;
    stVIPhistCfg.bstatic = 1;
    stVIPhistCfg.u8HistSft = 0;
    stVIPhistCfg.u8trig_ref_mode = 0;
    stVIPhistCfg.u32StatBase[0] = u64MIU_Addr;
    stVIPhistCfg.u32StatBase[1] = u64MIU_Addr+buffsize;
    ioctl(g_FD[E_DRV_ID_VIP], IOCTL_SCLVIP_SET_DLC_HISTOGRAM_CONFIG, &stVIPhistCfg);
    */
    //config
    stVIPDLCCfg.stGainOffset.u16Dlc_in_y_gain=0xFFFF;
    stVIPDLCCfg.stGainOffset.u16Dlc_in_y_offset=0xFFFF;
    stVIPDLCCfg.stGainOffset.u16Dlc_in_c_gain=0xFFFF;
    stVIPDLCCfg.stGainOffset.u16Dlc_in_c_offset=0xFFFF;
    stVIPDLCCfg.stGainOffset.u16Dlc_out_y_gain=0xFFFF;
    stVIPDLCCfg.stGainOffset.u16Dlc_out_y_offset=0xFFFF;
    stVIPDLCCfg.stGainOffset.u16Dlc_out_c_gain=0xFFFF;
    stVIPDLCCfg.stGainOffset.u16Dlc_out_c_offset=0xFFFF;

    stVIPDLCCfg.stEn.bcurve_fit_en=0xFF;
    stVIPDLCCfg.stEn.bstatistic_en=0xFF;
    stVIPDLCCfg.stEn.bcurve_fit_var_pw_en=0xFF;
    for (i=0;i<DRV_SCLVIP_IO_DLC_LUMA_SECTION_NUM;i++)
    {
        stVIPDLCCfg.stEn.stCurve.u8InLuma[i]=0xFF;
        stVIPDLCCfg.stEn.stCurve.u8InLumaLSB[i]=0xFF;
        stVIPDLCCfg.stEn.stCurve.ProgCfg.u8VARCP[i]=0xFF;
    }
    stVIPDLCCfg.stEn.stCurve.ProgCfg.u8Tbln0=0xFF;
    stVIPDLCCfg.stEn.stCurve.ProgCfg.u8Tbln0LSB=0xFF;
    stVIPDLCCfg.stEn.stCurve.ProgCfg.u8Tbln0sign=0xFF;
    stVIPDLCCfg.stEn.stCurve.ProgCfg.u16Tbl64=0xFFFF;
    stVIPDLCCfg.stEn.stCurve.ProgCfg.u8Tbl64LSB=0xFF;

    stVIPDLCCfg.stDither.bDLCdither_en=0xFF;

    stVIPDLCCfg.sthist.u8brange_en=0xFF;
    stVIPDLCCfg.stHistH.u16statistic_h_start=0xFFFF;
    stVIPDLCCfg.stHistH.u16statistic_h_end=0xFFFF;
    stVIPDLCCfg.stHistV.u16statistic_v_start=0xFFFF;
    stVIPDLCCfg.stHistV.u16statistic_v_end=0xFFFF;

    stVIPDLCCfg.stPC.bhis_y_rgb_mode_en=0xFF;
    stVIPDLCCfg.stPC.bcurve_fit_rgb_en=0xFF;
    stVIPDLCCfg = FILL_VERCHK_TYPE(stVIPDLCCfg, stVIPDLCCfg.VerChk_Version, stVIPDLCCfg.VerChk_Size,DRV_SCLVIP_VERSION);
    DrvSclVipIoSetDlcConfig(g_FD[E_DRV_ID_VIP], &stVIPDLCCfg);
}


void Test_VIP_LCE(VipLceConfig_t stCfg)
{
    DrvSclVipIoLceConfig_t stVIPLCECfg;
    u32 i;

    if(Open_Device(E_DRV_ID_VIP) == 0)
    {
         return ;
    }
    //CMDQ
    stVIPLCECfg.stFCfg.bEn=stCfg.stFCfg.bEn;
    stVIPLCECfg.stFCfg.u8framecnt=stCfg.stFCfg.u8framecnt;
    // LCE config
    stVIPLCECfg.stOnOff.bLCE_En=0xFF;

    stVIPLCECfg.stDITHER.bLCE_Dither_En=0xFF;

    stVIPLCECfg.stSet.bLCE_sodc_alpha_en=0xFF;
    stVIPLCECfg.stSet.bLce_dering_alpha_en=0xFF;
    stVIPLCECfg.stSet.enLce_y_ave_sel=E_DRV_SCLVIP_IO_LCE_Y_AVE_5X7;
    stVIPLCECfg.stSet.bLce_3curve_en=0xFF;
    stVIPLCECfg.stSet.u8Lce_std_slop1=0xFF;
    stVIPLCECfg.stSet.u8Lce_std_slop2=0xFF;
    stVIPLCECfg.stSet.u8Lce_std_th1=0xFF;
    stVIPLCECfg.stSet.u8Lce_std_th2=0xFF;
    stVIPLCECfg.stSet.u8Lce_gain_min=0xFF;
    stVIPLCECfg.stSet.u8Lce_sodc_low_alpha=0xFF;
    stVIPLCECfg.stSet.u8Lce_sodc_low_th=0xFF;
    stVIPLCECfg.stSet.u8Lce_sodc_slop=0xFF;
    stVIPLCECfg.stSet.u8Lce_diff_gain=0xFF;
    stVIPLCECfg.stSet.u8Lce_gain_complex=0xFF;
    stVIPLCECfg.stSet.u8Lce_dsw_minsadgain=0xFF;
    stVIPLCECfg.stSet.u8Lce_dsw_gain=0xFF;
    stVIPLCECfg.stSet.u8Lce_gain_max=0xFF;
    stVIPLCECfg.stSet.u8LCE_dsw_thrd=0xFF;

    for(i=0;i<4;i++)
    stVIPLCECfg.stCurve.u8Curve_Thread[i]=i;

    for(i=0;i<DRV_SCLVIP_IO_LCE_CURVE_SECTION_NUM;i++)
    {
        stVIPLCECfg.stCurve.u16Curve1[i]=0x200+i;
        stVIPLCECfg.stCurve.u16Curve2[i]=0x100+i;
        stVIPLCECfg.stCurve.u16Curve3[i]=i;
    }
    stVIPLCECfg = FILL_VERCHK_TYPE(stVIPLCECfg, stVIPLCECfg.VerChk_Version, stVIPLCECfg.VerChk_Size,DRV_SCLVIP_VERSION);
    DrvSclVipIoSetLceConfig(g_FD[E_DRV_ID_VIP], &stVIPLCECfg);
}


void Test_VIP_UVC(VipUvcConfig_t stCfg)
{
    DrvSclVipIoUvcConfig_t stVIPUVCCfg;

    if(Open_Device(E_DRV_ID_VIP) == 0)
    {
         return ;
    }
    //CMDQ
    stVIPUVCCfg.stFCfg.bEn=stCfg.stFCfg.bEn;
    stVIPUVCCfg.stFCfg.u8framecnt=stCfg.stFCfg.u8framecnt;
    //config
    stVIPUVCCfg.stUVC.buvc_en=0xFF;
    stVIPUVCCfg.stUVC.u8uvc_locate=0xFF;
    stVIPUVCCfg.stUVC.u8uvc_gain_high_limit_lsb=0xFF;
    stVIPUVCCfg.stUVC.u8uvc_gain_high_limit_msb=0xFF;
    stVIPUVCCfg.stUVC.u8uvc_gain_low_limit_lsb=0xFF;
    stVIPUVCCfg.stUVC.u8uvc_gain_low_limit_msb=0xFF;
    stVIPUVCCfg.stUVC.buvc_adaptive_luma_en=0xFF;
    stVIPUVCCfg.stUVC.u8uvc_adaptive_luma_y_input=0xFF;
    stVIPUVCCfg.stUVC.u8uvc_adaptive_luma_black_step=0xFF;
    stVIPUVCCfg.stUVC.u8uvc_adaptive_luma_white_step=0xFF;
    stVIPUVCCfg.stUVC.u8uvc_adaptive_luma_black_th=0xFF;
    stVIPUVCCfg.stUVC.u8uvc_adaptive_luma_white_th=0xFF;
    stVIPUVCCfg.stUVC.u8uvc_adaptive_luma_gain_low=0xFF;
    stVIPUVCCfg.stUVC.u8uvc_adaptive_luma_gain_med=0xFF;
    stVIPUVCCfg.stUVC.u8uvc_adaptive_luma_gain_high=0xFF;
    stVIPUVCCfg.stUVC.buvc_rgb_en=0xFF;
    stVIPUVCCfg.stUVC.buvc_dlc_fullrange_en=0xFF;
    stVIPUVCCfg.stUVC.u8uvc_low_sat_prot_thrd=0xFF;
    stVIPUVCCfg.stUVC.u8uvc_low_sat_min_strength=0xFF;
    stVIPUVCCfg.stUVC.buvc_low_y_sat_prot_en=0xFF;
    stVIPUVCCfg.stUVC.buvc_lpf_en=0xFF;
    stVIPUVCCfg.stUVC.buvc_low_sat_prot_en=0xFF;
    stVIPUVCCfg.stUVC.buvc_low_y_prot_en=0xFF;
    stVIPUVCCfg.stUVC.u8uvc_low_sat_prot_slope=0xFF;
    stVIPUVCCfg.stUVC.u8uvc_low_y_prot_slope=0xFF;
    stVIPUVCCfg.stUVC.enAdp_Ysel=E_DRV_SCLVIP_IO_UVC_ADP_Y_INPUT_SEL_DLC_Y_INPUT;
    stVIPUVCCfg.stUVC.u8uvc_low_y_prot_thrd=0xFF;
    stVIPUVCCfg.stUVC.u8uvc_low_y_min_strength=0xFF;
    stVIPUVCCfg = FILL_VERCHK_TYPE(stVIPUVCCfg, stVIPUVCCfg.VerChk_Version, stVIPUVCCfg.VerChk_Size,DRV_SCLVIP_VERSION);
    DrvSclVipIoSetUvcConfig(g_FD[E_DRV_ID_VIP], &stVIPUVCCfg);
}


void Test_VIP_IHC(VipIhcConfig_t stCfg)
{
    DrvSclVipIoIhcConfig_t stVIPIHCCfg;
    DrvSclVipIoIhcIccConfig_t stIHCICC;
    u32 i;
    //u32 j;

    if(Open_Device(E_DRV_ID_VIP) == 0)
    {
         return ;
    }
    //CMDQ
    stVIPIHCCfg.stFCfg.bEn=stCfg.stFCfg.bEn;
    stVIPIHCCfg.stFCfg.u8framecnt=stCfg.stFCfg.u8framecnt;
    //config
    stVIPIHCCfg.stOnOff.bIHC_en=0xFF;

    stVIPIHCCfg.stYmd.bIHC_y_mode_en=0xFF;
    stVIPIHCCfg.stYmd.bIHC_y_mode_diff_color_en=0xFF;

    stVIPIHCCfg.stDither.bIHC_dither_en=0xFF;
    for (i=0;i<DRV_SCLVIP_IO_IHC_USER_COLOR_NUM;i++)
    {
        stVIPIHCCfg.stset.stIHC_color[i].u8hue_user_color=0xFF;
        stVIPIHCCfg.stset.stIHC_color[i].u8hue_user_color_sec0=0xFF;
        stVIPIHCCfg.stset.stIHC_color[i].u8hue_user_color_sec1=0xFF;
        stVIPIHCCfg.stset.stIHC_color[i].u8hue_user_color_sec2=0xFF;

        stIHCICC.stYmdset.stYmode_Yvalue_color[i].u8ihc_icc_color_sec0=0xFF;
        stIHCICC.stYmdset.stYmode_Yvalue_color[i].u8ihc_icc_color_sec1=0xFF;
        stIHCICC.stYmdset.stYmode_Yvalue_color[i].u8ihc_icc_color_sec2=0xFF;
        stIHCICC.stYmdset.stYmode_Yvalue_color[i].u8ihc_icc_color_sec3=0xFF;
    }

    //IHC ICC
    stIHCICC.stFCfg.bEn=stCfg.stFCfg.bEn;
    stIHCICC.stFCfg.u8framecnt=stCfg.stFCfg.u8framecnt;
    for (i=0;i<4;i++)
    {
        stIHCICC.stYmdall.u8ihc_icc_y[i]=0xFF;
    }
    stVIPIHCCfg = FILL_VERCHK_TYPE(stVIPIHCCfg, stVIPIHCCfg.VerChk_Version, stVIPIHCCfg.VerChk_Size,DRV_SCLVIP_VERSION);
    stIHCICC = FILL_VERCHK_TYPE(stIHCICC, stIHCICC.VerChk_Version, stIHCICC.VerChk_Size,DRV_SCLVIP_VERSION);
    DrvSclVipIoSetIhcConfig(g_FD[E_DRV_ID_VIP],  &stVIPIHCCfg);
    DrvSclVipIoSetIhcIceAdpYConfig(g_FD[E_DRV_ID_VIP], &stIHCICC);
}


void Test_VIP_ICE(VipIceConfig_t stCfg)
{
    DrvSclVipIoIccConfig_t stVIPICCCfg;
    u32 i;
    //u32 j;

    if(Open_Device(E_DRV_ID_VIP) == 0)
    {
         return ;
    }
    //CMDQ
    stVIPICCCfg.stFCfg.bEn=stCfg.stFCfg.bEn;
    stVIPICCCfg.stFCfg.u8framecnt=stCfg.stFCfg.u8framecnt;
    //config
    stVIPICCCfg.stEn.bICC_en=0xFF;
    stVIPICCCfg.stEn.bcbcr_to_uv_en=0xFF;
    stVIPICCCfg.stEn.u8common_minus_gain=0xFF;
    stVIPICCCfg.stEn.u8sa_min=0xFF;
    stVIPICCCfg.stEn.u16step_sa_user=0xFFFF;

    stVIPICCCfg.stYmd.icc_y_mode_en=0xFF;
    stVIPICCCfg.stYmd.icc_y_mode_diff_color_en=0xFF;
    for(i=0;i<DRV_SCLVIP_IO_ICE_COLOR_NUM;i++)
    {
        stVIPICCCfg.stYmd.stICC_color[i].u8sa_user_color_sec0=0xFF;
        stVIPICCCfg.stYmd.stICC_color[i].u8sign_sa_user_color_sec0=0xFF;
        stVIPICCCfg.stYmd.stICC_color[i].u8sa_user_color_sec1=0xFF;
        stVIPICCCfg.stYmd.stICC_color[i].u8sign_sa_user_color_sec1=0xFF;
        stVIPICCCfg.stYmd.stICC_color[i].u8sa_user_color_sec2=0xFF;
        stVIPICCCfg.stYmd.stICC_color[i].u8sign_sa_user_color_sec2=0xFF;


        stVIPICCCfg.stSet.stICC_color[i].u8sa_user_color=0xFF;
        stVIPICCCfg.stSet.stICC_color[i].u8sign_sa_user_color=0xFF;
    }
    stVIPICCCfg.stDither.bICC_dither_en=0xFF;
    stVIPICCCfg = FILL_VERCHK_TYPE(stVIPICCCfg, stVIPICCCfg.VerChk_Version, stVIPICCCfg.VerChk_Size,DRV_SCLVIP_VERSION);
    DrvSclVipIoSetIccConfig(g_FD[E_DRV_ID_VIP], &stVIPICCCfg);
}

void Test_VIP_IBC(VipIbcConfig_t stCfg)
{
    DrvSclVipIoIbcConfig_t stVIPCfg;
    u8 i;
    if(Open_Device(E_DRV_ID_VIP) == 0)
    {
         return ;
    }

    //config
    stVIPCfg.stFCfg.bEn =stCfg.stFCfg.bEn;
    stVIPCfg.stFCfg.u8framecnt=stCfg.stFCfg.u8framecnt;
    stVIPCfg.stEn.bIBC_en=0xFF;
    stVIPCfg.stEn.u8IBC_coring_thrd=0xFF;
    stVIPCfg.stEn.bIBC_y_adjust_lpf_en=0xFF;

    for(i=0;i<DRV_SCLVIP_IO_ICE_COLOR_NUM;i++)
    {

        stVIPCfg.stSet.u8ycolor_adj[i]=0xFF;
    }
    stVIPCfg.stDither.bIBC_dither_en=0xFF;
    stVIPCfg = FILL_VERCHK_TYPE(stVIPCfg, stVIPCfg.VerChk_Version, stVIPCfg.VerChk_Size,DRV_SCLVIP_VERSION);
    DrvSclVipIoSetIbcConfig(g_FD[E_DRV_ID_VIP], &stVIPCfg);
}

void Test_VIP_FCC(VipFccConfig_t stCfg)
{
    DrvSclVipIoFccConfig_t stVIPCfg;
    u8 i, j;

    if(Open_Device(E_DRV_ID_VIP) == 0)
    {
         return ;
    }
    //CMDQ
    stVIPCfg.stFCfg.bEn=stCfg.stFCfg.bEn;
    stVIPCfg.stFCfg.u8framecnt=stCfg.stFCfg.u8framecnt;
    //config
    stVIPCfg.stfr.bfcc_fr_en=0xFF;
    for(i=0;i<4;i++)
    {
        stVIPCfg.stfr.bEn[i]=0;
        stVIPCfg.stfr.u8YWinNum[i]=0x44;
        for(j=0;j<DRV_SCLVIP_IO_FCC_YWIN_LUT_ENTRY_NUM;j++)
        {
            stVIPCfg.stfr.u8Ywin_LUT[i].u8fcc_adp_Y_LUT[j]=0xFF;
        }
    }
    for(i=0;i<8;i++)
    {
        stVIPCfg.stT[i].bEn=0;
        stVIPCfg.stT[i].u8Cb=128;
        stVIPCfg.stT[i].u8Cr=128;
        stVIPCfg.stT[i].u8K=255;
        stVIPCfg.stT[i].u8LSB_Cb=0x22;
        stVIPCfg.stT[i].u8LSB_Cr=0;
        for(j=0;j<E_DRV_SCLVIP_IO_FCC_Y_DIS_NUM;j++)
        {
            stVIPCfg.stT[i].u8Range[j]=0;
            stVIPCfg.stT[i].u16FullRange[j]=0x3FF;
        }
    }
    stVIPCfg.stT9.bEn=0;
    stVIPCfg.stT9.u8K=0xFF;
    stVIPCfg.stT9.u8Cr=0xFF;
    stVIPCfg.stT9.u8Cb=0xFF;
    stVIPCfg.stT9.bfirstEn=0;
    stVIPCfg.stT9.u8frCb=0xFF;
    stVIPCfg.stT9.u8LSB_Cb=0xFF;
    stVIPCfg.stT9.u8frCr=0xFF;
    stVIPCfg.stT9.u8LSB_Cr=0xFF;
    for(j=0;j<E_DRV_SCLVIP_IO_FCC_Y_DIS_NUM;j++)
    {
        stVIPCfg.stT9.u16FullRange[j]=0x5FF;
    }
    stVIPCfg = FILL_VERCHK_TYPE(stVIPCfg, stVIPCfg.VerChk_Version, stVIPCfg.VerChk_Size,DRV_SCLVIP_VERSION);
    DrvSclVipIoSetFccConfig(g_FD[E_DRV_ID_VIP], &stVIPCfg);

}

void Test_VIP_NLM(VipFccConfig_t stCfg)
{
    DrvSclVipIoNlmConfig_t stVIPCfg;
    //u8 i;
    u8 j;

    if(Open_Device(E_DRV_ID_VIP) == 0)
    {
         return ;
    }
    //CMDQ
    stVIPCfg.stFCfg.bEn=stCfg.stFCfg.bEn;
    stVIPCfg.stFCfg.u8framecnt=stCfg.stFCfg.u8framecnt;

    stVIPCfg.stNLM.bNlm_en=0xFF;
    stVIPCfg.stNLM.enAvgmode=E_DRV_SCLVIP_IO_NLM_Average_5x5_mode;
    stVIPCfg.stNLM.bnlm_bdry_en=0xFF;
    stVIPCfg.stNLM.bnlm_post_luma_adap_en=0xFF;
    stVIPCfg.stNLM.bnlm_luma_adap_en=0xFF;
    stVIPCfg.stNLM.bnlm_dsw_adap_en=0xFF;
    stVIPCfg.stNLM.bnlmdsw_lpf_en=0xFF;
    stVIPCfg.stNLM.bnlm_region_adap_en=0xFF;
    stVIPCfg.stNLM.u8nlm_region_adap_size_config=E_DRV_SCLVIP_IO_NLM_DSW_32x16_mode;
    stVIPCfg.stNLM.bnlm_bypass_en=0;
    stVIPCfg.stNLM.u8nlm_fin_gain=16;
    stVIPCfg.stNLM.u8nlm_sad_shift=0;
    stVIPCfg.stNLM.u8nlm_sad_gain=16;
    stVIPCfg.stNLM.u8nlm_dsw_ratio=8;
    stVIPCfg.stNLM.u8nlm_dsw_offset=8;
    stVIPCfg.stNLM.u8nlm_dsw_shift=0xFF;
    for(j=0;j<DRV_SCLVIP_IO_NLM_WEIGHT_NUM;j++)
    {
        stVIPCfg.stNLM.u8nlm_weight_lut[j]=j+2;
    }

    for(j=0;j<DRV_SCLVIP_IO_NLM_LUMAGAIN_NUM;j++)
    {
        stVIPCfg.stNLM.u8nlm_luma_adap_gain_lut[j]=0x1F;
    }

    for(j=0;j<DRV_SCLVIP_IO_NLM_POSTLUMA_NUM;j++)
    {
        stVIPCfg.stNLM.u8nlm_post_luma_adap_gain_lut[j]=3;
    }

    for(j=0;j<DRV_SCLVIP_IO_NLM_DISTWEIGHT_NUM;j++)
    {
        stVIPCfg.stNLM.u8nlm_dist_weight_7x7_lut[j]=0;
    }

    //region gain
    //for(i=0;i<17;i++)
    //{
        stVIPCfg.stSRAM =Test_VIP_NLM_Autodownload(1);
        stVIPCfg = FILL_VERCHK_TYPE(stVIPCfg, stVIPCfg.VerChk_Version, stVIPCfg.VerChk_Size,DRV_SCLVIP_VERSION);
        DrvSclVipIoSetNlmConfig(g_FD[E_DRV_ID_VIP], &stVIPCfg);
        sclprintf("u32Baseadr=%lx\n",stVIPCfg.stSRAM.u32Baseadr);
        sclprintf("u32viradr=%lx\n",stVIPCfg.stSRAM.u32viradr);

    //}

}

void Test_VIP_AIPSet(void)
{
    DrvSclVipIoAipConfig_t  stCfg;
    DrvSclVipIoAipSramConfig_t stSRAMCfg;
    SclMemConfig_t stMsysMemInfo;
    u32 buffsize;
    void *u64Vir_data;
    void *u64Vir_data2;
    void *u64Vir_data3;
    DrvSclVipIoAipType_e enty;
    DrvSclVipIoAipSramType_e  ensrty;
    u8 pBuf[50];
    if(Open_Device(E_DRV_ID_VIP) == 0)
    {
         return ;
    }
    //CMDQ
    stCfg.stFCfg.bEn=0;
    stCfg.stFCfg.u8framecnt=0;

    //if size more than 0x1000
    memset(&stMsysMemInfo, 0, sizeof(SclMemConfig_t));
    buffsize = 256;//11
    stMsysMemInfo = _AllocateDmem("FGPASNR",1,buffsize);
    if(stMsysMemInfo.pVirAddr == NULL)
    {
        sclprintf("Allocate Mem fail !!!!!\n");
        return;
    }
    sclprintf("[FPGA1]Phy:%llx,Vir:%lx\n",(stMsysMemInfo.u32PhyAddr),(u32)stMsysMemInfo.pVirAddr);

    u64Vir_data   = stMsysMemInfo.pVirAddr;

    memset(u64Vir_data,0xFF,buffsize);

    stCfg.u32Viraddr = (u32)stMsysMemInfo.pVirAddr;
    stCfg.enAIPType = E_DRV_SCLVIP_IO_AIP_GM10TO12;
    stCfg = FILL_VERCHK_TYPE(stCfg, stCfg.VerChk_Version, stCfg.VerChk_Size,DRV_SCLVIP_VERSION);
    DrvSclVipIoSetAipConfig(g_FD[E_DRV_ID_VIP], &stCfg);
    _FreeDmem(stMsysMemInfo);

    //if size less than 0x1000
    buffsize = 512;//6
    u64Vir_data = DrvSclOsMemalloc(buffsize, 0);
    stCfg.u32Viraddr = (u32)u64Vir_data;

    sclprintf("[FPGA3]Vir:%lx,Vir:%lx\n",(u32)u64Vir_data,stCfg.u32Viraddr);

    memset(u64Vir_data,0xFF,buffsize);


    for(enty =E_DRV_SCLVIP_IO_AIP_YEE;enty<E_DRV_SCLVIP_IO_AIP_NUM;enty++)
    {
        stCfg.enAIPType = enty;
        stCfg = FILL_VERCHK_TYPE(stCfg, stCfg.VerChk_Version, stCfg.VerChk_Size,DRV_SCLVIP_VERSION);
        sclprintf("[FPGA3]entype:%d\n",enty);
        DrvSclVipIoSetAipConfig(g_FD[E_DRV_ID_VIP], &stCfg);
    }

    // others
    stCfg.u32Viraddr = (u32)pBuf;
    sclprintf("[FPGA3]Vir:%lx,Vir:%lx\n",(u32)u64Vir_data,stCfg.u32Viraddr);
    memset(pBuf,0x5,50);

    stCfg.enAIPType = E_DRV_SCLVIP_IO_AIP_WDR_LOC;
    stCfg = FILL_VERCHK_TYPE(stCfg, stCfg.VerChk_Version, stCfg.VerChk_Size,DRV_SCLVIP_VERSION);
    DrvSclVipIoSetAipConfig(g_FD[E_DRV_ID_VIP], &stCfg);
    DrvSclOsMemFree(u64Vir_data);

    //if size less than 0x1000
    buffsize = 512;//6
    u64Vir_data2 = DrvSclOsMemalloc(buffsize, 0);
    stSRAMCfg.u32Viraddr = (u32)u64Vir_data2;
    sclprintf("[FPGA3]Vir:%lx,Vir:%lx\n",(u32)u64Vir_data2,stCfg.u32Viraddr);
    memset(u64Vir_data2,0x01,buffsize);
    *((u16*)u64Vir_data2+10) = 0x0;
    *((u16*)u64Vir_data2+11) = 0x0;
    *((u16*)u64Vir_data2+12) = 0x0;
    *((u16*)u64Vir_data2+13) = 0x0;

    for(ensrty = E_DRV_SCLVIP_IO_AIP_SRAM_GAMMA_Y;ensrty<E_DRV_SCLVIP_IO_AIP_SRAM_WDR;ensrty++)
    {
        stSRAMCfg.enAIPType = ensrty;
        stSRAMCfg = FILL_VERCHK_TYPE(stSRAMCfg, stSRAMCfg.VerChk_Version, stSRAMCfg.VerChk_Size,DRV_SCLVIP_VERSION);
        DrvSclVipIoSetAipSramConfig(g_FD[E_DRV_ID_VIP], &stSRAMCfg);
    }
    DrvSclOsMemFree(u64Vir_data2);

    buffsize = 162*8;//6
    u64Vir_data3 = DrvSclOsMemalloc(buffsize, 0);
    stSRAMCfg.u32Viraddr = (u32)u64Vir_data3;
    sclprintf("[FPGA3]Vir:%lx,buffersize:%ld\n",(u32)u64Vir_data3,buffsize);
    memset(u64Vir_data3,0x03,buffsize);

    stSRAMCfg.enAIPType = E_DRV_SCLVIP_IO_AIP_SRAM_WDR;
    stSRAMCfg = FILL_VERCHK_TYPE(stSRAMCfg, stSRAMCfg.VerChk_Version, stSRAMCfg.VerChk_Size,DRV_SCLVIP_VERSION);
    DrvSclVipIoSetAipSramConfig(g_FD[E_DRV_ID_VIP], &stSRAMCfg);
    DrvSclOsMemFree(u64Vir_data3);
}


void Test_VIP_ACK(VipFccConfig_t stCfg)
{
    DrvSclVipIoAckConfig_t stVIPCfg;
    //u8 i, j;

    if(Open_Device(E_DRV_ID_VIP) == 0)
    {
         sclprintf("Open VIP fail\n");
         return ;
    }
    //CMDQ
    stVIPCfg.stFCfg.bEn=stCfg.stFCfg.bEn;
    stVIPCfg.stFCfg.u8framecnt=stCfg.stFCfg.u8framecnt;

    stVIPCfg.stACK.backen          = 64;
    stVIPCfg.stACK.bYswitch_dithen = 16;
    stVIPCfg.stACK.bYswitchen      = 0;
    stVIPCfg.stACK.u8Uswitch       = 0;
    stVIPCfg.stACK.u8Vswitch       = 0;
    stVIPCfg.stACK.u8Ythrd         = 0;
    stVIPCfg.stACK.u8offset        = 0;
    stVIPCfg.stACK.u8yslot         = 0;
    stVIPCfg.stACK.u8limit         = 64;
    stVIPCfg.stACK.bCcompen        = 0;
    stVIPCfg.stACK.u8Cthrd         = 0;
    stVIPCfg.stACK.u8Crange        = 0;
    stVIPCfg.stclip.bclamp_en       = 1;
    stVIPCfg.stclip.u16y_max_clamp  = 1023;
    stVIPCfg.stclip.u16y_min_clamp  = 0;
    stVIPCfg.stclip.u16cb_max_clamp = 512;
    stVIPCfg.stclip.u16cb_min_clamp = 512;
    stVIPCfg.stclip.u16cr_max_clamp = 512;
    stVIPCfg.stclip.u16cr_min_clamp = 512;
/*
    stVIPCfg.stACK.backen=0xFF;
    stVIPCfg.stACK.bYswitch_dithen=0xFF;
    stVIPCfg.stACK.bYswitchen=0xFF;
    stVIPCfg.stACK.u8Uswitch=0xFF;
    stVIPCfg.stACK.u8Vswitch=0xFF;
    stVIPCfg.stACK.u8Ythrd=0xFF;
    stVIPCfg.stACK.u8offset=0xFF;
    stVIPCfg.stACK.u8yslot=0xFF;
    stVIPCfg.stACK.u8limit=0xFF;
    stVIPCfg.stACK.bCcompen=0xFF;
    stVIPCfg.stACK.u8Cthrd=0xFF;
    stVIPCfg.stACK.u8Crange=0xFF;


    stVIPCfg.stclip.bclamp_en=0xFF;
    stVIPCfg.stclip.u16y_max_clamp=0xFFF;
    stVIPCfg.stclip.u16y_min_clamp=0xFFF;
    stVIPCfg.stclip.u16cb_max_clamp=0xFFF;
    stVIPCfg.stclip.u16cb_min_clamp=0xFFF;
    stVIPCfg.stclip.u16cr_max_clamp=0xFFF;
    stVIPCfg.stclip.u16cr_min_clamp=0xFFF;
*/
    stVIPCfg = FILL_VERCHK_TYPE(stVIPCfg, stVIPCfg.VerChk_Version, stVIPCfg.VerChk_Size,DRV_SCLVIP_VERSION);
    DrvSclVipIoSetAckConfig(g_FD[E_DRV_ID_VIP], &stVIPCfg);
}

void Test_VIP_Set_Vtrack(DrvSclVipIoVtrackConfig_t stCfg)
{
    u32 i;
    u8 u8VtrackSetting[23]=
    {
        0x00, 0x00, 0xaa, 0xbb, 0xcc,
        0xdd, 0xee, 0xff, 0x11, 0x22,
        0x33, 0x44, 0x55, 0x66, 0x77,
        0x88, 0x99, 0x12, 0x34, 0x56,
        0x78, 0x90, 0xab,
    };

    u8 u8VtrackKey[8]=
    {
        0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,
    };
    if(Open_Device(E_DRV_ID_VIP) == 0)
    {
         return ;
    }
    for(i =0;i<8;i++)
    stCfg.u8SetKey[i] = u8VtrackKey[i];
    for(i =0;i<23;i++)
    stCfg.u8SetUserDef[i] = u8VtrackSetting[i];
    stCfg = FILL_VERCHK_TYPE(stCfg, stCfg.VerChk_Version, stCfg.VerChk_Size,DRV_SCLVIP_VERSION);
    DrvSclVipIoSetVtrackConfig(g_FD[E_DRV_ID_VIP], &stCfg);
}

void Test_VIP_Vtrack_ON(DrvSclVipIoVtrackOnOffConfig_t stCfg)
{
    if(Open_Device(E_DRV_ID_VIP) == 0)
    {
         return ;
    }
    stCfg = FILL_VERCHK_TYPE(stCfg, stCfg.VerChk_Version, stCfg.VerChk_Size,DRV_SCLVIP_VERSION);
    DrvSclVipIoSetVtrackOnOffConfig(g_FD[E_DRV_ID_VIP], &stCfg);
}

void Test_VIP_Mixed(VipFccConfig_t stCfg)
{
    DrvSclVipIoConfig_t stVIPCfg;
    //u8 i, j;

    if(Open_Device(E_DRV_ID_VIP) == 0)
    {
         return ;
    }
    //CMDQ
    stVIPCfg.stFCfg.bEn=stCfg.stFCfg.bEn;
    stVIPCfg.stFCfg.u8framecnt=stCfg.stFCfg.u8framecnt;

    stVIPCfg.st422_444.bvip_422to444_en=0;
    stVIPCfg.st422_444.u8vip_422to444_md=0;
    stVIPCfg.stBypass.bvip_fun_bypass_en=0;
    stVIPCfg.stLB.u8vps_sram_act=0;

    stVIPCfg = FILL_VERCHK_TYPE(stVIPCfg, stVIPCfg.VerChk_Version, stVIPCfg.VerChk_Size,DRV_SCLVIP_VERSION);
    DrvSclVipIoSetVipConfig(g_FD[E_DRV_ID_VIP], &stVIPCfg);
}


void PaserVIP(char *str)
{
    if( Parsing_VipString(str, VIP_DNR_SET_CHAR) )
    {
        VipDnrConfig_t stCfg;
        stCfg.stFCfg.bEn = 0;
        stCfg.stFCfg.u8framecnt = 0;
        stCfg.u16Width = 0;
        stCfg.u16Height = 0;
        Test_VIP_MCNR(stCfg);
    }
    else if( Parsing_VipString(str, VIP_LDC_SET_CHAR) )
    {
        VipLdcConfig_t stCfg;
        stCfg.stFCfg.bEn = 0;
        stCfg.stFCfg.u8framecnt = 0;
        stCfg.u16Width = 0;
        stCfg.u16Height = 0;

        Test_VIP_LDC(stCfg);
    }
    else if( Parsing_VipString(str, VIP_PK_SET_CHAR) )
    {
        VipPkConfig_t stpkCfg;
        stpkCfg.stFCfg.bEn = 0;
        stpkCfg.stFCfg.u8framecnt = 0;
        Test_VIP_PK(stpkCfg);
    }
    else if( Parsing_VipString(str, VIP_HIST_CHAR) )
    {
        VipDlcConfig_t stCfg;
        stCfg.stFCfg.bEn = 0;
        stCfg.stFCfg.u8framecnt = 0;

        Test_VIP_Hist(stCfg);
    }
    else if( Parsing_VipString(str, VIP_DLC_SET_CHAR) )
    {
        VipDlcConfig_t stCfg;
        stCfg.stFCfg.bEn = 0;
        stCfg.stFCfg.u8framecnt = 0;

        Test_VIP_DLC(stCfg);
    }
    else if( Parsing_VipString(str, VIP_LCE_SET_CHAR) )
    {
        VipLceConfig_t stCfg;
        stCfg.stFCfg.bEn = 0;
        stCfg.stFCfg.u8framecnt = 0;

        Test_VIP_LCE(stCfg);
    }
    else if( Parsing_VipString(str, VIP_UVC_SET_CHAR) )
    {
        VipUvcConfig_t stCfg;
        stCfg.stFCfg.bEn = 0;
        stCfg.stFCfg.u8framecnt = 0;
        Test_VIP_UVC(stCfg);
    }
    else if( Parsing_VipString(str, VIP_IHC_SET_CHAR) )
    {
        VipIhcConfig_t stCfg;
        stCfg.stFCfg.bEn = 0;
        stCfg.stFCfg.u8framecnt = 0;

        Test_VIP_IHC(stCfg);
    }
    else if( Parsing_VipString(str, VIP_ICE_SET_CHAR) )
    {
        VipIceConfig_t stCfg;
        stCfg.stFCfg.bEn = 0;
        stCfg.stFCfg.u8framecnt = 0;
        Test_VIP_ICE(stCfg);
    }
    else if( Parsing_VipString(str, VIP_IBC_SET_CHAR) )
    {
        VipIbcConfig_t stCfg;
        stCfg.stFCfg.bEn = 0;
        stCfg.stFCfg.u8framecnt = 0;

        Test_VIP_IBC(stCfg);
    }
    else if( Parsing_VipString(str, VIP_FCC_SET_CHAR) )
    {
        VipFccConfig_t stCfg;
        stCfg.stFCfg.bEn = 0;
        stCfg.stFCfg.u8framecnt = 0;

        Test_VIP_FCC(stCfg);
    }
    else if( Parsing_VipString(str, VIP_NLM_SET_CHAR) )
    {
        VipFccConfig_t stCfg;
        stCfg.stFCfg.bEn = 0;
        stCfg.stFCfg.u8framecnt = 0;

        Test_VIP_NLM(stCfg);
    }
    else if( Parsing_VipString(str, VIP_ACK_SET_CHAR) )
    {
        VipFccConfig_t stCfg;
        stCfg.stFCfg.bEn = 0;
        stCfg.stFCfg.u8framecnt = 0;

        Test_VIP_ACK(stCfg);
    }
    else if( Parsing_VipString(str, VIP_MIX_SET_CHAR) )
    {
        VipFccConfig_t stCfg;
        stCfg.stFCfg.bEn = 0;
        stCfg.stFCfg.u8framecnt = 0;

        Test_VIP_Mixed(stCfg);
    }
    else if( Parsing_VipString(str, VIP_SET_ALL_CHAR) )
    {
        //Test_VIP_All();
    }
    else if( Parsing_VipString(str, VIP_SET_AIP_CHAR) )
    {
        Test_VIP_AIPSet();
    }
    else if( Parsing_VipString(str, VIP_VTRACK_SET_CHAR) )
    {
        DrvSclVipIoVtrackConfig_t stCfg;
        stCfg.bSetKey       = 1;
        stCfg.bSetUserDef   = 1;
        stCfg.u16Timecode   = 0x2;
        stCfg.u8OperatorID  = 0x4;
        Test_VIP_Set_Vtrack(stCfg);
    }
    else if( Parsing_VipString(str, VIP_VTRACK_ONOFF_CHAR) )
    {
        DrvSclVipIoVtrackOnOffConfig_t stCfg;
        stCfg.EnType = E_DRV_SCLVIP_IO_VTRACK_ENABLE_DEBUG;
        stCfg.u8framerate = 29;

        Test_VIP_Vtrack_ON(stCfg);
    }
    else
    {
        sclprintf("%s %d, Unknown String %s\n", __FUNCTION__, __LINE__, str);
    }


}

void Test_FGPA_SCLDMA_1_4_VIP(FPGA_CONFIG stCfg, char *VipToken)
{
    u32 buffsize;
    DrvSclHvspIoScalingConfig_t stHvspScaleCfg;
    SclMemConfig_t stMsysMemInfo;
    DrvSclDmaIoBufferConfig_t stSCLDMACfg;
    SclDmaPollConfig_t stScldmaPollCfg;
    SclThreadConfig_t  Scldma_Poll_Thread;
    SclThreadConfig_t  Mo_Thread;
    u32 choise = 54;
    if(Init_Device() == FALSE)
    {
        return;
    }


    Reset_DBGMG_FLAG();
#if 0
    Set_DBGMG_FLAG(EN_DBGMG_PRIORITY_CONFIG, 0xFF);
    Set_DBGMG_FLAG(EN_DBGMG_DRVPQ_CONFIG, 0xFF);
    Set_DBGMG_FLAG(EN_DBGMG_VIP_CONFIG, 0xFF);
    Set_DBGMG_FLAG(EN_DBGMG_DRVVIP_CONFIG, 0xFF);
#endif

    if(Open_Device(E_DRV_ID_HVSP1) == 0)
    {
        return;
    }

    if(Open_Device(E_DRV_ID_HVSP2) == 0)
    {
        return;
    }


    if(Open_Device(E_DRV_ID_SCLDMA1) == 0)
    {
        return;
    }
    if(Open_Device(E_DRV_ID_SCLDMA2) == 0)
    {
        return;
    }
    if(Open_Device(E_DRV_ID_SCLDMA4) == 0)
    {
        return;
    }

    if(Open_Device(E_DRV_ID_PNL) == 0)
    {
        return;
    }

    if(stCfg.u16BufNumber < 2)
    {
        sclprintf("BufferNumber must be 2 ~ 4");
        return;
    }
    // PNL, Timing Configure
    _FPGA_Pnl(stCfg.u16Dsp_Width,stCfg.u16Dsp_Height);

    if(stCfg.enSrc==E_DRV_SCLHVSP_IO_SRC_ISP)
    {
        _IspRun(NULL,NULL);
        _ISP_Input(stCfg.u16Src_Width,stCfg.u16Src_Height);
    }

    // HVSP1 Requenst IPM memory
    _Request_IPM(stCfg.u16Src_Width,stCfg.u16Src_Height);

    // HVSP1 in configure
    _HVSPInConfig(E_DRV_ID_HVSP1,stCfg.enSrc,stCfg.u16Src_Width,stCfg.u16Src_Height);

    // HVSP1 scaling configure
    memset(&stHvspScaleCfg, 0, sizeof(DrvSclHvspIoScalingConfig_t));
    stHvspScaleCfg = _FillHVSPScaleCfg(stCfg.u16Src_Width,stCfg.u16Src_Height,stCfg.u16Dsp_Width,stCfg.u16Dsp_Height);
    stHvspScaleCfg.stCropWin = _FillHVSPScaleCropCfg(stCfg.u16Src_Width,stCfg.u16Src_Height,0,0);
    _HVSPScaling(E_DRV_ID_HVSP1,stHvspScaleCfg,stCfg.enSrc);

    // SCLDMA1 OUT BUFFER Configure
    buffsize = stCfg.u16Dsp_Width * stCfg.u16Dsp_Height ; // 2bpp
    stMsysMemInfo = _AllocateDmem("FGPA14DMA",stCfg.u16BufNumber,buffsize);
    if(stMsysMemInfo.pVirAddr == NULL)
    {
        sclprintf("Allocate Mem fail !!!!!");
        return;
    }

    stSCLDMACfg = _FillBufferConfigByFPGAConfig(E_DRV_ID_SCLDMA1,stCfg,stMsysMemInfo,0);


    // config DMA4 buffer
    memset(&stSCLDMACfg, 0, sizeof(DrvSclDmaIoBufferConfig_t));
    stSCLDMACfg = _FillBufferConfigByFPGAConfig(E_DRV_ID_SCLDMA4,stCfg, stMsysMemInfo,1);
    //Set RPoint
    _SetRPoint(E_DRV_ID_SCLDMA1,stCfg.enMem,0x5);

    // create thread for single buffer
    if(stCfg.enBufferMd == E_DRV_SCLDMA_IO_BUFFER_MD_SINGLE)
    {
        // trig SCLDMA1
        _SetDMATrig(E_DRV_ID_SCLDMA1,E_DRV_SCLDMA_IO_MEM_IMI,1);
        _SetDMATrig(E_DRV_ID_SCLDMA1,stCfg.enMem,1);
        _SetDMAinTrig(E_DRV_ID_SCLDMA4,E_DRV_SCLDMA_IO_MEM_FRM,1);
        memset(&stScldmaPollCfg, 0, sizeof(SclDmaPollConfig_t));
        stScldmaPollCfg.bEn = 1;
        stScldmaPollCfg.enSC2Md = E_DRV_SC1_DSP;
        stScldmaPollCfg.stSwapCfg.enColor = stCfg.enColor;
        stScldmaPollCfg.stSwapCfg.enMem = stCfg.enMem;
        stScldmaPollCfg.stSwapCfg.u16Height   = stCfg.u16Dsp_Height;
        stScldmaPollCfg.stSwapCfg.u16Width    = stCfg.u16Dsp_Width;

        //0: front, 1: back
        if (stSCLDMACfg.enColorType    == E_DRV_SCLDMA_IO_COLOR_YUV422)
        {
            stScldmaPollCfg.stSwapCfg.u32Base_Y[0] = stMsysMemInfo.u32PhyAddr + (buffsize)*2;
            stScldmaPollCfg.stSwapCfg.u32Base_Y[1] = stMsysMemInfo.u32PhyAddr + (buffsize)*2 * 0;

        }
        else
        {
            stScldmaPollCfg.stSwapCfg.u32Base_Y[0] = stMsysMemInfo.u32PhyAddr + (buffsize)*1;
            stScldmaPollCfg.stSwapCfg.u32Base_C[0] = stMsysMemInfo.u32PhyAddr + (buffsize)*3;
            stScldmaPollCfg.stSwapCfg.u32Base_Y[1] = stMsysMemInfo.u32PhyAddr + (buffsize)*0 ;
            stScldmaPollCfg.stSwapCfg.u32Base_C[1] = stMsysMemInfo.u32PhyAddr + (buffsize)*2;
        }
        stScldmaPollCfg.stGetActCfg.enID[0] = E_DRV_ID_SCLDMA1;
        Scldma_Poll_Thread = _CreateTestThread(_Scldma_Poll_Thread_SingalBuf, &stScldmaPollCfg);
    }
    else
    {
        // trig SCLDMA1
        _SetDMATrig(E_DRV_ID_SCLDMA1,E_DRV_SCLDMA_IO_MEM_IMI,1);
        _SetDMATrig(E_DRV_ID_SCLDMA1,stCfg.enMem,1);

        // trig SCLDMA4
        _SetDMAinTrig(E_DRV_ID_SCLDMA4,E_DRV_SCLDMA_IO_MEM_FRM,1);
        stScldmaPollCfg.bEn = 1;
        stScldmaPollCfg.stGetActCfg.enMem[0]=stCfg.enMem;
        stScldmaPollCfg.stGetActCfg.enID[0] = E_DRV_ID_SCLDMA1;
        stScldmaPollCfg.stGetActCfg.Count=1;
        Scldma_Poll_Thread = _CreateTestThread(_Scldma_Poll_Thread_GetActive,&stScldmaPollCfg);
    }


    Scldma_Poll_Thread=Scldma_Poll_Thread; // Fix compile warning
    _SetRPoint(E_DRV_ID_SCLDMA1,stCfg.enMem,0x5);

    Mo_Thread = _CreateMonitorThread(MonitorFunction, &choise);

    while(1)
    {
        PaserVIP(VipToken);
        sclprintf("VIP Toekn=%s\n", VipToken);
        MsSleep(200);
        if(choise == 'Q'||choise == 'q')
        {
            stScldmaPollCfg.bEn = 0;
            sclprintf("sleep over \n");
            _SetDMATrig(E_DRV_ID_SCLDMA1,stCfg.enMem,0);
            // trig SCLDMA4
            _SetDMAinTrig(E_DRV_ID_SCLDMA4,E_DRV_SCLDMA_IO_MEM_FRM,0);
            sclprintf("close  start \n");
            if(Close_Device(E_DRV_ID_HVSP1) == 0)
            if(Close_Device(E_DRV_ID_HVSP2) == 0)
            if(Close_Device(E_DRV_ID_SCLDMA1) == 0)
            if(Close_Device(E_DRV_ID_SCLDMA2) == 0)
            if(Close_Device(E_DRV_ID_SCLDMA4) == 0)
            if(Close_Device(E_DRV_ID_PNL) == 0)
            if(Close_Device(E_DRV_ID_VIP) == 0)
            if(stCfg.enSrc==E_DRV_SCLHVSP_IO_SRC_PAT_TGEN)
            sclprintf("close  over \n");
            _DeleteTask(Scldma_Poll_Thread.TaskId ,Scldma_Poll_Thread.pStackTop);
            _DeleteTask(Mo_Thread.TaskId ,Mo_Thread.pStackTop);
            break;
        }
    }


}


/*=============================================================*/
//          Menu function
/*=============================================================*/
int _SclVip(CLI_t *pCli, char *p)
{
    int ParamCnt;
    FPGA_CONFIG stFPGACfg;
    char *src = NULL;

    ParamCnt = CliTokenCount(pCli);

    memset(&stFPGACfg, 0, sizeof(FPGA_CONFIG));

    if(ParamCnt == 1)
    {
        stFPGACfg.u16Src_Width   = 800;
        stFPGACfg.u16Src_Height  = 480;
        stFPGACfg.u16Dsp_Width   = 800;
        stFPGACfg.u16Dsp_Height  = 480;
        stFPGACfg.enBufferMd     = E_DRV_SCLDMA_IO_BUFFER_MD_RING;
        stFPGACfg.enColor        = E_DRV_SCLDMA_IO_COLOR_YUV420;
        stFPGACfg.enMem          = E_DRV_SCLDMA_IO_MEM_FRM;
        stFPGACfg.enSrc          = E_DRV_SCLHVSP_IO_SRC_PAT_TGEN;
        stFPGACfg.u16BufNumber   = 2;

        pCli->tokenLvl++;
        src = CliTokenPop(pCli);

        Test_FGPA_SCLDMA_1_4_VIP(stFPGACfg, src);
    }
    else if(ParamCnt == 8)
    {
        stFPGACfg.u16Src_Width   = _ParsingNum(pCli);
        stFPGACfg.u16Src_Height  = _ParsingNum(pCli);
        stFPGACfg.u16Dsp_Width   = _ParsingNum(pCli);
        stFPGACfg.u16Dsp_Height  = _ParsingNum(pCli);
        stFPGACfg.enBufferMd     = _ParsingSclDmaBufferMode(pCli);
        stFPGACfg.enColor        = E_DRV_SCLDMA_IO_COLOR_YUV420;
        stFPGACfg.enMem          = E_DRV_SCLDMA_IO_MEM_FRM;
        stFPGACfg.enSrc          = _ParsingSclHvspSrc(pCli);
        stFPGACfg.u16BufNumber   = (u16)_ParsingNum(pCli);

        pCli->tokenLvl++;
        src = CliTokenPop(pCli);

        Test_FGPA_SCLDMA_1_4_VIP(stFPGACfg, src);
    }

    return eCLI_PARSE_OK;
}


int _SclMulti(CLI_t *pCli, char *p)
{
    int ParamCnt;

    ParamCnt = CliTokenCount(pCli);

    if(ParamCnt == 0)
    {
        MultiInst_Func();
    }
    else if(ParamCnt == 1)
    {
        u32 u32value = _ParsingHexNum(pCli);
        TEST_MULTI_INST(u32value);
    }



    return eCLI_PARSE_OK;
}

int _SclHvsp(CLI_t *pCli, char *p)
{
    int i;
    s32 s32Handler[E_DRV_SCLHVSP_IO_ID_NUM] = {-1, -1, -1};

    Reset_DBGMG_FLAG();
    Set_DBGMG_FLAG(EN_DBGMG_PRIORITY_CONFIG,1);
    Set_DBGMG_FLAG(EN_DBGMG_DRVSCLDMA_CONFIG, 1);
    Set_DBGMG_FLAG(EN_DBGMG_DRVHVSP_CONFIG, 1);
    Set_DBGMG_FLAG(EN_DBGMG_DRVSCLIRQ_CONFIG, 1);
    Set_DBGMG_FLAG(EN_DBGMG_HVSP_CONFIG, 1);
    Set_DBGMG_FLAG(EN_DBGMG_MDRV_CONFIG, 1);


    Init_Device();

    // Open
    s32Handler[E_DRV_SCLHVSP_IO_ID_1] = DrvSclHvspIoOpen(E_DRV_SCLHVSP_IO_ID_1);
    s32Handler[E_DRV_SCLHVSP_IO_ID_2] = DrvSclHvspIoOpen(E_DRV_SCLHVSP_IO_ID_2);
    s32Handler[E_DRV_SCLHVSP_IO_ID_3] = DrvSclHvspIoOpen(E_DRV_SCLHVSP_IO_ID_3);

    for(i=0; i<E_DRV_SCLHVSP_IO_ID_NUM; i++)
    {
        if(s32Handler[i] == -1)
        {
            sclprintf("DrvSclHvspIoOpen, %d fail\n", i);
        }
    }

    // Request Memory
    DrvSclHvspIoReqMemConfig_t tReqMemCfg;

    tReqMemCfg.u16Vsize = 1080;
    tReqMemCfg.u16Pitch = 1920;
    tReqMemCfg.u32MemSize = 1920 * 1080 * 2;

    tReqMemCfg = FILL_VERCHK_TYPE(tReqMemCfg, tReqMemCfg.VerChk_Version, tReqMemCfg.VerChk_Size, DRV_SCLHVSP_VERSION);
    if( DrvSclHvspIoReqmemConfig(s32Handler[E_DRV_SCLHVSP_IO_ID_1], &tReqMemCfg) != E_DRV_SCLHVSP_IO_ERR_OK)
    {
        sclprintf("%s %d, DrvSclHvspIoReqmemConfig fail\n", __FUNCTION__, __LINE__);
    }

    // Input Config
    DrvSclHvspIoInputConfig_t tHvspInCfg;
    tHvspInCfg.VerChk_Version  = DRV_SCLHVSP_VERSION;
    tHvspInCfg.enSrcType =  E_DRV_SCLHVSP_IO_SRC_PAT_TGEN;
    tHvspInCfg.enColor = E_DRV_SCLHVSP_IO_COLOR_YUV420;
    tHvspInCfg.stCaptureWin.u16X = 0;
    tHvspInCfg.stCaptureWin.u16Y = 0;
    tHvspInCfg.stCaptureWin.u16Width = 1920;
    tHvspInCfg.stCaptureWin.u16Height = 1080;
    tHvspInCfg.stTimingCfg.bInterlace = FALSE;
    tHvspInCfg.stTimingCfg.u16Htotal = 4400;
    tHvspInCfg.stTimingCfg.u16Vtotal = 2250;
    tHvspInCfg.stTimingCfg.u16Vfrequency = 60;
    tHvspInCfg = FILL_VERCHK_TYPE(tHvspInCfg, tHvspInCfg.VerChk_Version, tHvspInCfg.VerChk_Size, DRV_SCLHVSP_VERSION);

    tHvspInCfg.VerChk_Size = sizeof(DrvSclHvspIoInputConfig_t);
    if( DrvSclHvspIoSetInputConfig(s32Handler[E_DRV_SCLHVSP_IO_ID_1], &tHvspInCfg) != E_DRV_SCLHVSP_IO_ERR_OK)
    {
       sclprintf("%s %d, DrvSclHvspIoSetInputConfig fail\n", __FUNCTION__, __LINE__);
    }


    //Scaling Config
    DrvSclHvspIoScalingConfig_t tScalingCfg;
    tScalingCfg.u16Src_Height = 1080;
    tScalingCfg.u16Src_Width = 1920;
    tScalingCfg.u16Dsp_Height = 480;
    tScalingCfg.u16Dsp_Width = 800;
    tScalingCfg.bCropEn = 0;
    memset(&tScalingCfg.stCropWin, 0, sizeof(DrvSclHvspIoWindowConfig_t));
    tScalingCfg = FILL_VERCHK_TYPE(tScalingCfg, tScalingCfg.VerChk_Version, tScalingCfg.VerChk_Size, DRV_SCLHVSP_VERSION);


    if(DrvSclHvspIoSetScalingConfig(s32Handler[E_DRV_SCLHVSP_IO_ID_1],&tScalingCfg) != E_DRV_SCLHVSP_IO_ERR_OK)
    {
        sclprintf("%s %d, DrvSclHvspIoSetScalingConfig fail\n", __FUNCTION__, __LINE__);
    }


    // PostScaling
    DrvSclHvspIoPostCropConfig_t tPostScalingCfg;
    tPostScalingCfg.bCropEn = 1;
    tPostScalingCfg.u16X = 0;
    tPostScalingCfg.u16Y = 0;
    tPostScalingCfg.u16Width = 800;
    tPostScalingCfg.u16Height = 480;
    tPostScalingCfg.bFmCntEn = 0;
    tPostScalingCfg = FILL_VERCHK_TYPE(tPostScalingCfg, tPostScalingCfg.VerChk_Version, tPostScalingCfg.VerChk_Size, DRV_SCLHVSP_VERSION);

    if(DrvSclHvspIoSetPostCropConfig(s32Handler[E_DRV_SCLHVSP_IO_ID_1],&tPostScalingCfg) != E_DRV_SCLHVSP_IO_ERR_OK)
    {
        sclprintf("%s %d, DrvSclHvspIoSetPostCropConfig fail\n", __FUNCTION__, __LINE__);
    }

    // Get ScrInform
    DrvSclHvspIoScInformConfig_t stInformCfg;

    if(DrvSclHvspIoGetInformConfig(s32Handler[E_DRV_SCLHVSP_IO_ID_1],&stInformCfg) != E_DRV_SCLHVSP_IO_ERR_OK)
    {
        sclprintf("%s %d, DrvSclHvspIoGetInformConfig fail\n", __FUNCTION__, __LINE__);
    }
    else
    {
        sclprintf("%d %d %d %d %d %d %d %d\n",
            stInformCfg.u16X, stInformCfg.u16Y, stInformCfg.u16Width,stInformCfg.u16Height,
            stInformCfg.u16crop2inWidth, stInformCfg.u16crop2inHeight,
            stInformCfg.u16crop2OutWidth, stInformCfg.u16crop2OutHeight);
    }


    //Release Memory
    if( DrvSclHvspIoReleaseMemConfig(s32Handler[E_DRV_SCLHVSP_IO_ID_1]) != E_DRV_SCLHVSP_IO_ERR_OK)
    {
        sclprintf("%s %d, DrvSclHvspIoReleaseMemConfig fail\n", __FUNCTION__, __LINE__);
    }

    // Set Osd
    DrvSclHvspIoOsdConfig_t stOsdCfg;
    stOsdCfg.bEn       = 1;
    stOsdCfg.enOSD_loc = E_DRV_SCLHVSP_IO_OSD_LOC_AFTER;
    stOsdCfg.bOSDBypass = 0;
    stOsdCfg.bWTMBypass = 0;

    stOsdCfg = FILL_VERCHK_TYPE(stOsdCfg, stOsdCfg.VerChk_Version, stOsdCfg.VerChk_Size, DRV_SCLHVSP_VERSION);
    if(DrvSclHvspIoSetOsdConfig(s32Handler[E_DRV_SCLHVSP_IO_ID_1], &stOsdCfg) != E_DRV_SCLHVSP_IO_ERR_OK)
    {
        sclprintf("%s %d, DrvSclHvspIoSetOsdConfig fail\n", __FUNCTION__, __LINE__);
    }

    // Set PriMask
    DrvSclHvspIoPriMaskConfig_t stPriMaskCfg;

    stPriMaskCfg.bMask = 1;
    stPriMaskCfg.u8idx = 0;
    stPriMaskCfg.stMaskWin.u16X = 10;
    stPriMaskCfg.stMaskWin.u16Y = 10;
    stPriMaskCfg.stMaskWin.u16Width = 10;
    stPriMaskCfg.stMaskWin.u16Height = 10;
    stPriMaskCfg = FILL_VERCHK_TYPE(stPriMaskCfg, stPriMaskCfg.VerChk_Version, stPriMaskCfg.VerChk_Size, DRV_SCLHVSP_VERSION);

    if(DrvSclHvspIoSetPriMaskConfig(s32Handler[E_DRV_SCLHVSP_IO_ID_1], &stPriMaskCfg) != E_DRV_SCLHVSP_IO_ERR_OK)
    {
        sclprintf("%s %d, DrvSclHvspIoSetPriMaskConfig fail\n", __FUNCTION__, __LINE__);
    }

    // Set Primask Trigger
    DrvSclHvspIoPriMaskTriggerConfig_t tPriMaskTriggerCfg;
    tPriMaskTriggerCfg.bEn = 1;
    tPriMaskTriggerCfg = FILL_VERCHK_TYPE(tPriMaskTriggerCfg, tPriMaskTriggerCfg.VerChk_Version, tPriMaskTriggerCfg.VerChk_Size, DRV_SCLHVSP_VERSION);
    if(DrvSclHvspIoPirMaskTrigerConfig(s32Handler[E_DRV_SCLHVSP_IO_ID_1], &tPriMaskTriggerCfg) != E_DRV_SCLHVSP_IO_ERR_OK)
    {
        sclprintf("%s %d, DrvSclHvspIoPirMaskTrigerConfig fail\n", __FUNCTION__, __LINE__);
    }

    // FB
    DrvSclHvspIoSetFbManageConfig_t stFbManageCfg;
    stFbManageCfg.enSet = E_DRV_SCLHVSP_IO_FBMG_SET_CIIR_OFF;
    stFbManageCfg = FILL_VERCHK_TYPE(stFbManageCfg, stFbManageCfg.VerChk_Version, stFbManageCfg.VerChk_Size, DRV_SCLHVSP_VERSION);
    if( DrvSclHvspIoSetFbConfig(s32Handler[E_DRV_SCLHVSP_IO_ID_1], &stFbManageCfg) != E_DRV_SCLHVSP_IO_ERR_OK)
    {
        sclprintf("%s %d, DrvSclHvspIoSetFbConfig fail\n", __FUNCTION__, __LINE__);

    }

    // Get Version
    DrvSclHvspIoVersionConfig_t stVersionCfg;
    stVersionCfg = FILL_VERCHK_TYPE(stVersionCfg, stVersionCfg.VerChk_Version, stVersionCfg.VerChk_Size, DRV_SCLHVSP_VERSION);
    if( DrvSclHvspIoGetVersion(s32Handler[E_DRV_SCLHVSP_IO_ID_1], &stVersionCfg) != E_DRV_SCLHVSP_IO_ERR_OK)
    {
        sclprintf("%s %d, DrvSclHvspIoGetVersion fail\n", __FUNCTION__, __LINE__);
    }
    else
    {
        sclprintf("%08x, %08x %08x \n", stVersionCfg.VerChk_Version, stVersionCfg.u32Version, stVersionCfg.VerChk_Size);
    }

    return eCLI_PARSE_OK;

}
int _SclFpga14(CLI_t *pCli, char *p)
{
    int ParamCnt;
    FPGA_CONFIG stFPGACfg;

    ParamCnt = CliTokenCount(pCli);

    memset(&stFPGACfg, 0, sizeof(FPGA_CONFIG));
    if(ParamCnt ==  9)
    {
        stFPGACfg.u16Src_Width   = _ParsingNum(pCli);
        stFPGACfg.u16Src_Height  = _ParsingNum(pCli);
        stFPGACfg.u16Dsp_Width   = _ParsingNum(pCli);
        stFPGACfg.u16Dsp_Height  = _ParsingNum(pCli);
        stFPGACfg.enBufferMd     = _ParsingSclDmaBufferMode(pCli);
        stFPGACfg.enColor        = _ParsingSclDmaColor(pCli);
        stFPGACfg.enMem          = _ParsingSclDmaMem(pCli);
        stFPGACfg.enSrc          = _ParsingSclHvspSrc(pCli);
        stFPGACfg.u16BufNumber   = (u16)_ParsingNum(pCli);
        Test_FPGA_SCLDMA_1_4(stFPGACfg);
    }
    else if(ParamCnt ==  7)
    {
        stFPGACfg.u16Src_Width   = _ParsingNum(pCli);
        stFPGACfg.u16Src_Height  = _ParsingNum(pCli);
        stFPGACfg.u16Dsp_Width   = _ParsingNum(pCli);
        stFPGACfg.u16Dsp_Height  = _ParsingNum(pCli);
        stFPGACfg.enBufferMd     = _ParsingSclDmaBufferMode(pCli);
        stFPGACfg.enColor        = E_DRV_SCLDMA_IO_COLOR_YUV420;
        stFPGACfg.enMem          = E_DRV_SCLDMA_IO_MEM_FRM;
        stFPGACfg.enSrc          = _ParsingSclHvspSrc(pCli);
        stFPGACfg.u16BufNumber   = (u16)_ParsingNum(pCli);

        Test_FPGA_SCLDMA_1_4(stFPGACfg);
    }
    else
    {
        return eCLI_PARSE_INPUT_ERROR;
    }

    return eCLI_PARSE_OK;
}
int _SclFpga24(CLI_t *pCli, char *p)
{
    int ParamCnt;
    FPGA_CONFIG stFPGACfg;

    ParamCnt = CliTokenCount(pCli);

    memset(&stFPGACfg, 0, sizeof(FPGA_CONFIG));
    if(ParamCnt ==  9)
    {
        stFPGACfg.u16Src_Width   = _ParsingNum(pCli);
        stFPGACfg.u16Src_Height  = _ParsingNum(pCli);
        stFPGACfg.u16Dsp_Width   = _ParsingNum(pCli);
        stFPGACfg.u16Dsp_Height  = _ParsingNum(pCli);
        stFPGACfg.enBufferMd     = _ParsingSclDmaBufferMode(pCli);
        stFPGACfg.enColor        = _ParsingSclDmaColor(pCli);
        stFPGACfg.enMem          = _ParsingSclDmaMem(pCli);
        stFPGACfg.enSrc          = _ParsingSclHvspSrc(pCli);
        stFPGACfg.u16BufNumber   = (u16)_ParsingNum(pCli);
        Test_FPGA_SCLDMA_2_4(stFPGACfg);
    }
    else if(ParamCnt ==  7)
    {
        stFPGACfg.u16Src_Width   = _ParsingNum(pCli);
        stFPGACfg.u16Src_Height  = _ParsingNum(pCli);
        stFPGACfg.u16Dsp_Width   = _ParsingNum(pCli);
        stFPGACfg.u16Dsp_Height  = _ParsingNum(pCli);
        stFPGACfg.enBufferMd     = _ParsingSclDmaBufferMode(pCli);
        stFPGACfg.enColor        = E_DRV_SCLDMA_IO_COLOR_YUV420;
        stFPGACfg.enMem          = E_DRV_SCLDMA_IO_MEM_FRM;
        stFPGACfg.enSrc          = _ParsingSclHvspSrc(pCli);
        stFPGACfg.u16BufNumber   = (u16)_ParsingNum(pCli);

        Test_FPGA_SCLDMA_2_4(stFPGACfg);
    }
    else
    {
        return eCLI_PARSE_INPUT_ERROR;
    }

    return eCLI_PARSE_OK;
}
int _SclFpgaSC2(CLI_t *pCli, char *p)
{
    int ParamCnt;
    FPGASC2_CONFIG stFPGACfg;

    ParamCnt = CliTokenCount(pCli);

    memset(&stFPGACfg, 0, sizeof(FPGA_CONFIG));
    if(ParamCnt ==  13)
    {
        stFPGACfg.u16Src_Width   = _ParsingNum(pCli);
        stFPGACfg.u16Src_Height  = _ParsingNum(pCli);
        stFPGACfg.u16sc1out_Width= _ParsingNum(pCli);
        stFPGACfg.u16sc1out_Height=_ParsingNum(pCli);
        stFPGACfg.u16Dsp_Width   = _ParsingNum(pCli);
        stFPGACfg.u16Dsp_Height  = _ParsingNum(pCli);
        stFPGACfg.enSC2Dsp       = _ParsingSC2Md(pCli);
        stFPGACfg.enBufferMd     = _ParsingSclDmaBufferMode(pCli);
        stFPGACfg.enBufferMdsc2  = _ParsingSclDmaBufferMode(pCli);
        stFPGACfg.enMem       = _ParsingSclDmaMem(pCli);
        stFPGACfg.enMemsc2       = _ParsingSclDmaMem(pCli);
        stFPGACfg.enColor        = E_DRV_SCLDMA_IO_COLOR_YUV420;
        stFPGACfg.enColorsc2     = E_DRV_SCLDMA_IO_COLOR_YUV420;
        stFPGACfg.enSrc          = _ParsingSclHvspSrc(pCli);
        stFPGACfg.u16BufNumber   = _ParsingNum(pCli);
        Test_FPGA_SCLDMA_1_2_4(stFPGACfg);
    }
    else if(ParamCnt ==  11)
    {
        stFPGACfg.u16Src_Width   = _ParsingNum(pCli);
        stFPGACfg.u16Src_Height  = _ParsingNum(pCli);
        stFPGACfg.u16sc1out_Width= _ParsingNum(pCli);
        stFPGACfg.u16sc1out_Height=_ParsingNum(pCli);
        stFPGACfg.u16Dsp_Width   = _ParsingNum(pCli);
        stFPGACfg.u16Dsp_Height  = _ParsingNum(pCli);
        stFPGACfg.enSC2Dsp       = _ParsingSC2Md(pCli);
        stFPGACfg.enBufferMd     = _ParsingSclDmaBufferMode(pCli);
        stFPGACfg.enBufferMdsc2  = stFPGACfg.enBufferMd;
        stFPGACfg.enMemsc2       = _ParsingSclDmaMem(pCli);
        if(stFPGACfg.enMemsc2==E_DRV_SCLDMA_IO_MEM_FRM2)
        {
            stFPGACfg.enMem          = E_DRV_SCLDMA_IO_MEM_FRM;
        }
        else if(stFPGACfg.enMemsc2==E_DRV_SCLDMA_IO_MEM_SNP)
        {
            stFPGACfg.enMem          = E_DRV_SCLDMA_IO_MEM_SNP;
            stFPGACfg.enMemsc2       = E_DRV_SCLDMA_IO_MEM_FRM;
        }
        if(stFPGACfg.enMem==E_DRV_SCLDMA_IO_MEM_NUM)
        {
            stFPGACfg.enMem      = E_DRV_SCLDMA_IO_MEM_FRM;
            stFPGACfg.enMemsc2   = E_DRV_SCLDMA_IO_MEM_FRM;
            stFPGACfg.enColor        = _ParsingSclDmaColor(pCli);
            stFPGACfg.enColorsc2     = stFPGACfg.enColor;
        }
        else
        {
            stFPGACfg.enColor        = E_DRV_SCLDMA_IO_COLOR_YUV420;
            stFPGACfg.enColorsc2     = E_DRV_SCLDMA_IO_COLOR_YUV420;

        }
        stFPGACfg.enSrc          = _ParsingSclHvspSrc(pCli);
        stFPGACfg.u16BufNumber   = _ParsingNum(pCli);
        Test_FPGA_SCLDMA_1_2_4(stFPGACfg);
    }
    else if(ParamCnt ==  10)
    {
        stFPGACfg.u16Src_Width   = _ParsingNum(pCli);
        stFPGACfg.u16Src_Height  = _ParsingNum(pCli);
        stFPGACfg.u16sc1out_Width= _ParsingNum(pCli);
        stFPGACfg.u16sc1out_Height=_ParsingNum(pCli);
        stFPGACfg.u16Dsp_Width   = _ParsingNum(pCli);
        stFPGACfg.u16Dsp_Height  = _ParsingNum(pCli);
        stFPGACfg.enSC2Dsp       = _ParsingSC2Md(pCli);
        stFPGACfg.enBufferMd     = _ParsingSclDmaBufferMode(pCli);
        stFPGACfg.enColor        = E_DRV_SCLDMA_IO_COLOR_YUV420;
        stFPGACfg.enMem          = E_DRV_SCLDMA_IO_MEM_FRM;
        stFPGACfg.enBufferMdsc2  = stFPGACfg.enBufferMd;
        stFPGACfg.enColorsc2     = E_DRV_SCLDMA_IO_COLOR_YUV420;
        stFPGACfg.enMemsc2       = E_DRV_SCLDMA_IO_MEM_FRM;
        stFPGACfg.enSrc          = _ParsingSclHvspSrc(pCli);
        stFPGACfg.u16BufNumber   = _ParsingNum(pCli);
        Test_FPGA_SCLDMA_1_2_4(stFPGACfg);
    }
    else
    {
        return eCLI_PARSE_INPUT_ERROR;
    }

    return eCLI_PARSE_OK;
}
int _SclASICCROP(CLI_t *pCli, char *p)
{
    int ParamCnt;
    CROP_CONFIG stCROPCfg;

    ParamCnt = CliTokenCount(pCli);

    memset(&stCROPCfg, 0, sizeof(CROP_CONFIG));
    if(ParamCnt == 9)
    {
        stCROPCfg.u16Src_Width   = _ParsingNum(pCli);
        stCROPCfg.u16Src_Height  = _ParsingNum(pCli);
        stCROPCfg.u16crop_Width  = _ParsingNum(pCli);
        stCROPCfg.u16crop_Height = _ParsingNum(pCli);
        stCROPCfg.u16crop_X      = _ParsingNum(pCli);
        stCROPCfg.u16crop_Y      = _ParsingNum(pCli);
        stCROPCfg.u16Dsp_Width   = _ParsingNum(pCli);
        stCROPCfg.u16Dsp_Height  = _ParsingNum(pCli);
        stCROPCfg.enSrc          = _ParsingSclHvspSrc(pCli);
        Test_ASIC_CROP(stCROPCfg);
    }
    else
    {
        return eCLI_PARSE_INPUT_ERROR;
    }

    return eCLI_PARSE_OK;
}

int _SclPnl(CLI_t *pCli, char *p)
{
    int i, ParamCnt;
    u32 value[10];

    ParamCnt = CliTokenCount(pCli);

    if (ParamCnt != 2)
    {
      return eCLI_PARSE_INPUT_ERROR;
    }

    for(i=0;i<ParamCnt; i++)
    {
        if(CliTokenPopNum(pCli, &value[i], 10) != eCLI_PARSE_OK)
        {
          return eCLI_PARSE_INPUT_ERROR;
        }
    }

    if(DrvSclPnlIoInit() != E_DRV_SCLPNL_IO_ERR_OK)
    {
        sclprintf("DrvSclPnlIoInit fail\n");
        return eCLI_PARSE_QUIT;
    }

    if(Open_Device(E_DRV_ID_PNL) == FALSE)
    {
        return eCLI_PARSE_QUIT;
    }

    _FPGA_Pnl(value[0], value[1]);

    if(DrvSclPnlIoRelease(g_FD[E_DRV_ID_PNL]) != E_DRV_SCLPNL_IO_ERR_OK)
    {
        sclprintf("DrvSclPnlIoRelease fail\n");
        return eCLI_PARSE_QUIT;
    }

    return eCLI_PARSE_OK;

}

int _SclOsMem(CLI_t *pCli, char *p)
{
    int i, ParamCnt;
    u8 *pBuf;
    u32 value[10];
    void *pVir;
    u32  u32Phy;

    ParamCnt = CliTokenCount(pCli);

    if (ParamCnt != 1)
    {
      return eCLI_PARSE_INPUT_ERROR;
    }

    for(i=0;i<ParamCnt; i++)
    {
        if(CliTokenPopNum(pCli, &value[i], 10) != eCLI_PARSE_OK)
        {
          return eCLI_PARSE_INPUT_ERROR;
        }
    }

    pBuf = DrvSclOsMemalloc(value[0], 0);

    if(pBuf == NULL)
    {
        sclprintf("DrvSclOsMemalloc fail \n");
        return eCLI_PARSE_QUIT;
    }

    for(i=0; i<value[0]; i++)
    {
        pBuf[i] = i + 100;
        sclprintf("%d,", pBuf[i]);
    }
    sclprintf("\n");


    DrvSclOsMemFree(pBuf);

    pBuf = DrvSclOsVirMemalloc(value[0]);

    if(pBuf == NULL)
    {
        sclprintf("DrvSclOsVirMemalloc fail \n");
        return eCLI_PARSE_QUIT;
    }
    for(i=0; i<value[0]; i++)
    {
        pBuf[i] = i  + 10;
        sclprintf("%d,", pBuf[i]);
    }
    sclprintf("\n");
    DrvSclOsVirMemFree(pBuf);

    pVir = DrvSclOsVirMemalloc(value[0]);
    *(u32 *)pVir = 0xAABBCCDD;

    u32Phy =(u32) MsVA2PA(pVir);

    sclprintf("%Phy=%08x =  %x\n", u32Phy, *(u32 *)pVir);

    DrvSclOsVirMemFree(pVir);

    return eCLI_PARSE_OK;

}


void _SclMutex1(s32 s32Mutex1)
{
    sclprintf("%s %d\n", __FUNCTION__, __LINE__);
    MsConsumeSem(SCL_DONE_SEM);

    sclprintf("%s %d\n", __FUNCTION__, __LINE__);

    DrvSclOsObtainMutex(s32Mutex1, 100);
    sclprintf("%s %d\n", __FUNCTION__, __LINE__);


    DrvSclOsReleaseMutex(s32Mutex1);

    sclprintf("%s %d\n", __FUNCTION__, __LINE__);

}

int _SclOsEvent(CLI_t *pCli, char *p)
{
    s32 s32EventId;
    u32 u32retrivalevent;
    DrvSclOsInit();
    int i, ParamCnt;
    u32 value[10];
    u32 u32WaitTime = SCLOS_WAIT_FOREVER;
    DrvSclOsEventWaitMoodeType_e enMode;

    ParamCnt = CliTokenCount(pCli);
    if (ParamCnt != 3  && ParamCnt != 4)
    {
      return eCLI_PARSE_INPUT_ERROR;
    }

    for(i=0;i< 2; i++)
    {
        if(CliTokenPopNum(pCli, &value[i], 16) != eCLI_PARSE_OK)
        {
          return eCLI_PARSE_INPUT_ERROR;
        }
        sclprintf("value_%d=%x\n", i, value[i]);

    }

    char *src;

    pCli->tokenLvl++;
    src = CliTokenPop(pCli);

    if(strcmp(src,"OR") == 0)
    {
        enMode = E_DRV_SCLOS_EVENT_MD_OR;
    }
    else if(strcmp(src,"AND") == 0)
    {
        enMode = E_DRV_SCLOS_EVENT_MD_AND;
    }
    else if(strcmp(src,"AND_CLEAR") == 0)
    {

        enMode = E_DRV_SCLOS_EVENT_MD_AND_CLEAR;
    }
    else if(strcmp(src,"OR_CLEAR") == 0)
    {
        enMode = E_DRV_SCLOS_EVENT_MD_OR_CLEAR;
    }
    else
    {
        return eCLI_PARSE_INVALID_PARAMETER;
    }

    if(ParamCnt == 4)
    {
        if(CliTokenPopNum(pCli, &value[3], 16) != eCLI_PARSE_OK)
        {
          return eCLI_PARSE_INPUT_ERROR;
        }

        u32WaitTime = (ParamCnt == 4) ? value[3] : SCLOS_WAIT_FOREVER;
    }

    s32EventId = DrvSclOsCreateEventGroup("SCLEve");

    if(-1 == s32EventId)
    {

        sclprintf("create event fail");
        return eCLI_PARSE_QUIT;
    }

    DrvSclOsSetEvent(s32EventId, value[0]);

    DrvSclOsDelayTask(500);

    sclprintf("%s %d, time=%d\n", __FUNCTION__, __LINE__, DrvSclOsGetSystemTime());

    if(DrvSclOsWaitEvent(s32EventId, value[1], &u32retrivalevent, enMode, u32WaitTime))
    {
        sclprintf("%s %d, time=%d, even=%08x\n", __FUNCTION__, __LINE__, DrvSclOsGetSystemTime(), u32retrivalevent);
    }
    else
    {

        sclprintf("Wait Event Fail\n");
    }

    SclThreadConfig_t stThreadId;
    SclDmaPollConfig_t stPollCfg;

    stPollCfg.bEn = 1;
    stThreadId = _CreateTestThread(_scl_poll_Thread, &stPollCfg);

    MsSleep(500);
    _DeleteTask(stThreadId.TaskId, stThreadId.pStackTop);

    return eCLI_PARSE_OK;
}


int _SclRegRW(CLI_t *pCli, char *p)
{
    u32 value[3];
    int i, ParamCnt;

    ParamCnt = CliTokenCount(pCli);
    if (ParamCnt > 2 || ParamCnt == 0)
    {
      return eCLI_PARSE_INPUT_ERROR;
    }

    for(i=0;i<ParamCnt; i++)
    {
        if(CliTokenPopNum(pCli, &value[i], 16) != eCLI_PARSE_OK)
        {
          return eCLI_PARSE_INPUT_ERROR;
        }
        sclprintf("value_%d=%x\n", i, value[i]);

    }

    if(ParamCnt == 1)
    {
        sclprintf("reg_%06x=%04x\n", value[0], DrvSclRegisterRead(value[0]));
    }

    if(ParamCnt == 2)
    {
        DrvSclRegisterWrite(value[0], (U16)value[1]);
        sclprintf("reg_%06x=%04x\n", value[0], DrvSclRegisterRead(value[0]));
    }
    return eCLI_PARSE_OK;
}

