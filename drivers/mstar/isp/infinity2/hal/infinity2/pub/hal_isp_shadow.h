#ifndef ISP_SHADOW_H
#define ISP_SHADOW_H
#include <infinity2_reg.h>
#include <hal_isp.h>
/* IQ shadow IndBlk*/
typedef enum
{
    eIsoInit,
    eIsoRuntime,
}IspStage_e;

typedef struct
{
    //IspStage_e eStage;
    infinity2_reg_isp0       isp0_cfg;
    infinity2_reg_isp1       isp1_cfg;
    infinity2_reg_isp2       isp2_cfg;
    infinity2_reg_isp3       isp3_cfg;
    infinity2_reg_isp4       isp4_cfg;
    infinity2_reg_isp5       isp5_cfg;
    infinity2_reg_isp6       isp6_cfg;
    infinity2_reg_isp7       isp7_cfg;
    infinity2_reg_isp8       isp8_cfg;
    infinity2_reg_isp9       isp9_cfg;
    infinity2_reg_isp10      isp10_cfg;
    infinity2_reg_isp11      isp11_cfg;
    infinity2_reg_isp12      isp12_cfg;

	//alsc
    //u32  alsc_h_tbl[33];
    //u32  alsc_v_tbl[33];

    //awb
    IQ_WBG_CFG wbg;
}HalIspRegShadow_t;

/* IQ shadow API*/
typedef enum
{
    eIspShdInit,
    eIspShdRuntime,
    eIspShdVsyncEnd,
}IspShdStage_e;


int HalIsp_ApplyDECOMP(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage);
int HalIsp_ApplyFPN(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage);
int HalIsp_ApplyFPNP1(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage);
int HalIsp_ApplyOBC(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage);
int HalIsp_ApplyOBCP1(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage);
int HalIsp_ApplyGAMA16to16ISP(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage);
int HalIsp_ApplyGAMAC2C_P1(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage);
int HalIsp_ApplyLSC(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage);
int HalIsp_ApplyLSCP1(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage);
int HalIsp_ApplyALSC(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage);
int HalIsp_ApplyALSCP1(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage);
int HalIsp_ApplyHDR(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage);
int HalIsp_ApplyHDR16to10(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage);
int HalIsp_ApplyDPC(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage);
int HalIsp_ApplyGE(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage);
int HalIsp_ApplyANTICT(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage);
int HalIsp_ApplyRGBIR(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage);
int HalIsp_ApplySpikeNR(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage);
int HalIsp_ApplySDC(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage);
int HalIsp_ApplyBSNR(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage);
int HalIsp_ApplyDM(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage);
int HalIsp_ApplyNM(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage);
int HalIsp_ApplyPostDN(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage);
int HalIsp_ApplyFalseColor(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage);
int HalIsp_GammaA2A(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage);
int HalISP_ApplyShadow(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage);

#endif
