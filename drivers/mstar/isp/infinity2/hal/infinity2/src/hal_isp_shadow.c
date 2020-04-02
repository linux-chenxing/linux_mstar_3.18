#include <hal_isp.h>
//#include <stdio.h>
//#include <stdlib.h>
#include <string.h>
#include "hal_isp_private.h"
#include <drv_ms_isp_general.h>
//#include <sys_MsWrapper_cus_os_mem.h>
//#include <cpu_mem_map.hc>
#include <hal_clkpad.h>
//#include "hal_drv_util.h"
#include <hal_isp_shadow.h>
#include <hal_isp_private.h>

#define APPLY_REG(a,b,reg) ((a)->##reg=(b)->##reg)
//APPLY_REG(dest,src,reg_isp_obc_r_ofst);

static int _HalIsp_RegCopy(unsigned short *dest,unsigned short *src_start,unsigned short *src_end)
{
    int n;
    int nRegs = ((unsigned int)src_end - (unsigned int)src_start)/4;
    for(n=0;n<=nRegs;++n)
    {
        dest[n*2] = src_start[n*2];
    }
    return 0;
}

/*
    {.bank= 0, .offset= 0, .mask= 0x04}, //DBG_IQ_OBC_EN
    {.bank= 1, .offset= 206, .mask= 0xFFFF}, //DBG_IQ_OBC_R_OFST
    {.bank= 1, .offset= 196, .mask= 0xFFFF}, //DBG_IQ_OBC_GR_OFST
    {.bank= 1, .offset= 210, .mask= 0xFFFF}, //DBG_IQ_OBC_GB_OFST
    {.bank= 1, .offset= 198, .mask= 0xFFFF}, //DBG_IQ_OBC_B_OFST
    {.bank= 1, .offset= 212, .mask= 0x3FFF}, //DBG_IQ_OBC_R_GAIN
    {.bank= 1, .offset= 214, .mask= 0x3FFF}, //DBG_IQ_OBC_GR_GAIN
    {.bank= 1, .offset= 216, .mask= 0x3FFF}, //DBG_IQ_OBC_GB_GAIN
    {.bank= 1, .offset= 218, .mask= 0x3FFF}, //DBG_IQ_OBC_B_GAIN
    {.bank= 1, .offset= 8, .mask= 0x01}, //DBG_IQ_OBC_DITHER_EN
    {.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_OBC_END
 */
int HalIsp_ApplyOBC(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage)
{
    isp_hal_handle *hal = (isp_hal_handle*)hnd;
    volatile  infinity2_reg_isp1 *dest = hal->isp1_cfg;
    infinity2_reg_isp1 *src = &pIspShd->isp1_cfg;
    volatile infinity2_reg_isp0 *dest0 = hal->isp0_cfg;
    infinity2_reg_isp0 *src0 = &pIspShd->isp0_cfg;

    //UartSendTrace("%s\n",__FUNCTION__);
    dest0->reg_en_isp_obc = src0->reg_en_isp_obc;

    dest->reg_isp_obc_r_ofst = src->reg_isp_obc_r_ofst;
    dest->reg_isp_obc_gr_ofst = src->reg_isp_obc_gr_ofst;
    dest->reg_isp_obc_gb_ofst = src->reg_isp_obc_gb_ofst;
    dest->reg_isp_obc_b_ofst = src->reg_isp_obc_b_ofst;

    dest->reg_isp_obc_r_gain = src->reg_isp_obc_r_gain;
    dest->reg_isp_obc_gr_gain = src->reg_isp_obc_gr_gain;
    dest->reg_isp_obc_gb_gain = src->reg_isp_obc_gb_gain;
    dest->reg_isp_obc_b_gain = src->reg_isp_obc_b_gain;

    dest->reg_obc_dith_en = src->reg_obc_dith_en;
    return 0;
}

/*
{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_DPC_START
{.bank= 2, .offset= 0, .mask= 0x01}, //DBG_IQ_DPC_EN
{.bank= 2, .offset= 0, .mask= 0x06}, //DBG_IQ_DPC_MODE
{.bank= 2, .offset= 0, .mask= 0x08}, //DBG_IQ_DPC_AWBG_EN
{.bank= 2, .offset= 0, .mask= 0x10}, //DBG_IQ_DPC_EDGE_MODE
{.bank= 2, .offset= 0, .mask= 0x20}, //DBG_IQ_DPC_LIGHT_EN
{.bank= 2, .offset= 0, .mask= 0x40}, //DBG_IQ_DPC_DARK_EN
{.bank= 2, .offset= 2, .mask= 0x0FFF}, //DBG_IQ_DPC_TH_R
{.bank= 2, .offset= 4, .mask= 0x0FFF}, //DBG_IQ_DPC_TH_GR
{.bank= 2, .offset= 6, .mask= 0x0FFF}, //DBG_IQ_DPC_TH_GB
{.bank= 2, .offset= 8, .mask= 0x0FFF}, //DBG_IQ_DPC_TH_B
{.bank= 2, .offset= 10, .mask= 0x01}, //DBG_IQ_DPC_CLUSTER_LR_EN
{.bank= 2, .offset= 10, .mask= 0x02}, //DBG_IQ_DPC_CLUSTER_LGR_EN
{.bank= 2, .offset= 10, .mask= 0x04}, //DBG_IQ_DPC_CLUSTER_LGB_EN
{.bank= 2, .offset= 10, .mask= 0x08}, //DBG_IQ_DPC_CLUSTER_LB_EN
{.bank= 2, .offset= 10, .mask= 0x10}, //DBG_IQ_DPC_CLUSTER_MODE_LOFF
{.bank= 2, .offset= 10, .mask= 0x100}, //DBG_IQ_DPC_CLUSTER_DR_EN
{.bank= 2, .offset= 10, .mask= 0x200}, //DBG_IQ_DPC_CLUSTER_DGR_EN
{.bank= 2, .offset= 10, .mask= 0x400}, //DBG_IQ_DPC_CLUSTER_DGB_EN
{.bank= 2, .offset= 10, .mask= 0x800}, //DBG_IQ_DPC_CLUSTER_DB_EN
{.bank= 2, .offset= 10, .mask= 0x1000}, //DBG_IQ_DPC_CLUSTER_MODE_DOFF
{.bank= 2, .offset= 12, .mask= 0x03}, //DBG_IQ_DPC_TH_SEL
{.bank= 2, .offset= 12, .mask= 0x04}, //DBG_IQ_DPC_LIGHT_TH_ADJ
{.bank= 2, .offset= 12, .mask= 0x08}, //DBG_IQ_DPC_DARK_TH_ADJ
{.bank= 2, .offset= 14, .mask= 0x0F}, //DBG_IQ_DPC_TH_LIGHT_SRC
{.bank= 2, .offset= 14, .mask= 0xF0}, //DBG_IQ_DPC_TH_LIGHT_BASE
{.bank= 2, .offset= 14, .mask= 0x0F00}, //DBG_IQ_DPC_TH_DARK_SRC
{.bank= 2, .offset= 14, .mask= 0xF000}, //DBG_IQ_DPC_TH_DARK_BASE

{.bank= 2, .offset= 16, .mask= 0xFF}, //DBG_IQ_DPC_NEI_SMOOTH
{.bank= 2, .offset= 18, .mask= 0x0FFF}, //DBG_IQ_DPC_NEI_DELTA_TH
{.bank= 2, .offset= 20, .mask= 0x0F}, //DBG_IQ_DPC_NEI_DELTA_SFT_X
{.bank= 2, .offset= 26, .mask= 0xFF}, //DBG_IQ_DPC_NEI_DELTA_GAIN

{.bank= 2, .offset= 32, .mask= 0xFFF}, //DBG_IQ_DPC_COLOR_RATIO_LR
{.bank= 2, .offset= 34, .mask= 0xFFF}, //DBG_IQ_DPC_COLOR_RATIO_LGR
{.bank= 2, .offset= 36, .mask= 0xFFF}, //DBG_IQ_DPC_COLOR_RATIO_LGB
{.bank= 2, .offset= 38, .mask= 0xFFF}, //DBG_IQ_DPC_COLOR_RATIO_LB
{.bank= 2, .offset= 40, .mask= 0xFFF}, //DBG_IQ_DPC_COLOR_RATIO_LOFF

{.bank= 2, .offset= 42, .mask= 0xFFF}, //DBG_IQ_DPC_COLOR_RATIO_DR
{.bank= 2, .offset= 44, .mask= 0xFFF}, //DBG_IQ_DPC_COLOR_RATIO_DGR
{.bank= 2, .offset= 46, .mask= 0xFFF}, //DBG_IQ_DPC_COLOR_RATIO_DGB
{.bank= 2, .offset= 48, .mask= 0xFFF}, //DBG_IQ_DPC_COLOR_RATIO_DB
{.bank= 2, .offset= 50, .mask= 0xFFF}, //DBG_IQ_DPC_COLOR_RATIO_DOFF

{.bank= 2, .offset= 52, .mask= 0xFF}, //DBG_IQ_DPC_LIGHT_TH_ADD
{.bank= 2, .offset= 84, .mask= 0xFF}, //DBG_IQ_DPC_DARK_TH_ADD

{.bank= 2, .offset= 160, .mask= 0x01}, //DBG_IQ_DPC_SORT_EN
{.bank= 2, .offset= 160, .mask= 0x02}, //DBG_IQ_DPC_SORT_R_EN
{.bank= 2, .offset= 160, .mask= 0x04}, //DBG_IQ_DPC_SORT_GR_EN
{.bank= 2, .offset= 160, .mask= 0x08}, //DBG_IQ_DPC_SORT_GB_EN
{.bank= 2, .offset= 160, .mask= 0x10}, //DBG_IQ_DPC_SORT_B_EN
{.bank= 2, .offset= 160, .mask= 0x20}, //DBG_IQ_DPC_SORT_1x3_MODE_EN
{.bank= 2, .offset= 162, .mask= 0x0F}, //DBG_IQ_DPC_SORT_LUMTBL_X
{.bank= 2, .offset= 166, .mask= 0xFFF}, //DBG_IQ_DPC_SORT_LUMTBL_L
{.bank= 2, .offset= 178, .mask= 0xFFF}, //DBG_IQ_DPC_SORT_LUMTBL_D
{.bank= 2, .offset= 190, .mask= 0xFFF}, //DBG_IQ_DPC_SORT_LUMTBL_S
{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_DPC_END
*/

int HalIsp_ApplyDPC(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage)
{
    isp_hal_handle *hal = (isp_hal_handle*)hnd;
    volatile  infinity2_reg_isp2 *dest = hal->isp2_cfg;
    infinity2_reg_isp2 *src = &pIspShd->isp2_cfg;

    //UartSendTrace("%s\n",__FUNCTION__);

    dest->reg_dpc_en = src->reg_dpc_en;
    dest->reg_dpc_mode = src->reg_dpc_mode;
    dest->reg_dpc_awbg_en = src->reg_dpc_awbg_en;
    dest->reg_dpc_edge_mode = src->reg_dpc_edge_mode;
    dest->reg_dpc_light_en = src->reg_dpc_light_en;
    dest->reg_dpc_dark_en = src->reg_dpc_dark_en;

    dest->reg_dpc_th_r = src->reg_dpc_th_r;
    dest->reg_dpc_th_gr = src->reg_dpc_th_gr;
    dest->reg_dpc_th_gb = src->reg_dpc_th_gb;
    dest->reg_dpc_th_b = src->reg_dpc_th_b;

    dest->reg_dpc_cluster_lr_en = src->reg_dpc_cluster_lr_en;
    dest->reg_dpc_cluster_lgr_en = src->reg_dpc_cluster_lgr_en;
    dest->reg_dpc_cluster_lgb_en = src->reg_dpc_cluster_lgb_en;
    dest->reg_dpc_cluster_lb_en = src->reg_dpc_cluster_lb_en;
    dest->reg_dpc_cluster_mode_loff = src->reg_dpc_cluster_mode_loff;

    dest->reg_dpc_cluster_dr_en = src->reg_dpc_cluster_dr_en;
    dest->reg_dpc_cluster_dgr_en = src->reg_dpc_cluster_dgr_en;
    dest->reg_dpc_cluster_dgb_en = src->reg_dpc_cluster_dgb_en;
    dest->reg_dpc_cluster_db_en = src->reg_dpc_cluster_db_en;
    dest->reg_dpc_cluster_mode_doff = src->reg_dpc_cluster_mode_doff;

    dest->reg_dpc_th_sel = src->reg_dpc_th_sel;
    dest->reg_dpc_light_th_adj = src->reg_dpc_light_th_adj;
    dest->reg_dpc_dark_th_adj = src->reg_dpc_dark_th_adj;
    dest->reg_dpc_th_light_src = src->reg_dpc_th_light_src;
    dest->reg_dpc_th_light_base = src->reg_dpc_th_light_base;
    dest->reg_dpc_th_dark_src = src->reg_dpc_th_dark_src;
    dest->reg_dpc_th_dark_base = src->reg_dpc_th_dark_base;

    dest->reg_dpc_nei_smooth = src->reg_dpc_nei_smooth;
    dest->reg_dpc_nei_delta_th = src->reg_dpc_nei_delta_th;
    dest->reg_dpc_nei_delta_sft_x0 = src->reg_dpc_nei_delta_sft_x0;
    dest->reg_dpc_nei_delta_sft_x1 = src->reg_dpc_nei_delta_sft_x1;
    dest->reg_dpc_nei_delta_sft_x2 = src->reg_dpc_nei_delta_sft_x2;
    dest->reg_dpc_nei_delta_sft_x3 = src->reg_dpc_nei_delta_sft_x3;
    dest->reg_dpc_nei_delta_sft_x4 = src->reg_dpc_nei_delta_sft_x4;

    dest->reg_dpc_nei_delta_gain0 = src->reg_dpc_nei_delta_gain0;
    dest->reg_dpc_nei_delta_gain1 = src->reg_dpc_nei_delta_gain1;
    dest->reg_dpc_nei_delta_gain2 = src->reg_dpc_nei_delta_gain2;
    dest->reg_dpc_nei_delta_gain3 = src->reg_dpc_nei_delta_gain3;
    dest->reg_dpc_nei_delta_gain4 = src->reg_dpc_nei_delta_gain4;
    dest->reg_dpc_nei_delta_gain5 = src->reg_dpc_nei_delta_gain5;

    dest->reg_dpc_color_ratio_lr = src->reg_dpc_color_ratio_lr;
    dest->reg_dpc_color_ratio_lgr = src->reg_dpc_color_ratio_lgr;
    dest->reg_dpc_color_ratio_lgb = src->reg_dpc_color_ratio_lgb;
    dest->reg_dpc_color_ratio_lb = src->reg_dpc_color_ratio_lb;
    dest->reg_dpc_color_ratio_loff = src->reg_dpc_color_ratio_loff;

    dest->reg_dpc_color_ratio_dr = src->reg_dpc_color_ratio_dr;
    dest->reg_dpc_color_ratio_dgr = src->reg_dpc_color_ratio_dgr;
    dest->reg_dpc_color_ratio_dgb = src->reg_dpc_color_ratio_dgb;
    dest->reg_dpc_color_ratio_db = src->reg_dpc_color_ratio_db;
    dest->reg_dpc_color_ratio_doff = src->reg_dpc_color_ratio_doff;

    //dest->reg_dpc_light_th_add00 = src->reg_dpc_light_th_add00;
    _HalIsp_RegCopy( (unsigned short *)reg_addr(dest,isp2_reg_dpc_light_th_add00),
                     (unsigned short *)reg_addr(src,isp2_reg_dpc_light_th_add00),
                     (unsigned short *)reg_addr(src,isp2_reg_dpc_light_th_add31)
                   );

    //dest->reg_dpc_dark_th_add00 = src->reg_dpc_dark_th_add00;
    _HalIsp_RegCopy( (unsigned short *)reg_addr(dest,isp2_reg_dpc_dark_th_add00),
                     (unsigned short *)reg_addr(src,isp2_reg_dpc_dark_th_add00),
                     (unsigned short *)reg_addr(src,isp2_reg_dpc_dark_th_add31)
                   );

    dest->reg_dpc_sort_en = src->reg_dpc_sort_en;
    dest->reg_dpc_sort_r_en = src->reg_dpc_sort_r_en;
    dest->reg_dpc_sort_gr_en = src->reg_dpc_sort_gr_en;
    dest->reg_dpc_sort_gb_en = src->reg_dpc_sort_gb_en;
    dest->reg_dpc_sort_b_en = src->reg_dpc_sort_b_en;
    dest->reg_dpc_sort_1x3_mode_en = src->reg_dpc_sort_1x3_mode_en;
    dest->reg_dpc_sort_lumtbl_x0 = src->reg_dpc_sort_lumtbl_x0;
    dest->reg_dpc_sort_lumtbl_x1 = src->reg_dpc_sort_lumtbl_x1;
    dest->reg_dpc_sort_lumtbl_x2 = src->reg_dpc_sort_lumtbl_x2;
    dest->reg_dpc_sort_lumtbl_x3 = src->reg_dpc_sort_lumtbl_x3;
    dest->reg_dpc_sort_lumtbl_x4 = src->reg_dpc_sort_lumtbl_x4;

    //dest->reg_dpc_sort_lumtbl_l0 = src->reg_dpc_sort_lumtbl_l0;
    _HalIsp_RegCopy( (unsigned short *)reg_addr(dest,isp2_reg_dpc_sort_lumtbl_l0),
                     (unsigned short *)reg_addr(src,isp2_reg_dpc_sort_lumtbl_l0),
                     (unsigned short *)reg_addr(src,isp2_reg_dpc_sort_lumtbl_l5)
                   );

    //dest->reg_dpc_sort_lumtbl_d0 = src->reg_dpc_sort_lumtbl_d0;
    _HalIsp_RegCopy( (unsigned short *)reg_addr(dest,isp2_reg_dpc_sort_lumtbl_d0),
                     (unsigned short *)reg_addr(src,isp2_reg_dpc_sort_lumtbl_l0),
                     (unsigned short *)reg_addr(src,isp2_reg_dpc_sort_lumtbl_d5)
                   );

    //dest->reg_dpc_sort_lumtbl_s0 = src->reg_dpc_sort_lumtbl_s0;
    _HalIsp_RegCopy( (unsigned short *)reg_addr(dest,isp2_reg_dpc_sort_lumtbl_s0),
                     (unsigned short *)reg_addr(src,isp2_reg_dpc_sort_lumtbl_s0),
                     (unsigned short *)reg_addr(src,isp2_reg_dpc_sort_lumtbl_s5)
                   );

    return 0;
}

/*
     {.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_SpikeNR_START
    {.bank= 1, .offset= 242, .mask= 0x01}, //DBG_IQ_SpikeNR_EN
    {.bank= 9, .offset= 164, .mask= 0x01}, //DBG_IQ_SpikeNR_DIST_EN
    {.bank= 1, .offset= 242, .mask= 0xF00}, //DBG_IQ_SpikeNR_NR_COEF

    {.bank= 1, .offset= 244, .mask= 0xFF}, //DBG_IQ_SpikeNR_P_THRD
    {.bank= 1, .offset= 244, .mask= 0x1F00}, //DBG_IQ_SpikeNR_P_THRD_1
    {.bank= 1, .offset= 246, .mask= 0xFF}, //DBG_IQ_SpikeNR_P_THRD_2
    {.bank= 1, .offset= 246, .mask= 0xFF00}, //DBG_IQ_SpikeNR_P_THRD_3

    {.bank= 1, .offset= 248, .mask= 0x07}, //DBG_IQ_SpikeNR_D_11_21_STEP
    {.bank= 1, .offset= 248, .mask= 0x70}, //DBG_IQ_SpikeNR_D_31_STEP
    {.bank= 1, .offset= 248, .mask= 0x700}, //DBG_IQ_SpikeNR_YP_22_STEP

    {.bank= 9, .offset= 164, .mask= 0x1C}, //DBG_IQ_SpikeNR_ORIENT
    {.bank= 9, .offset= 166, .mask= 0x1FFF}, //DBG_IQ_SpikeNR_OFFSET_X
    {.bank= 9, .offset= 168, .mask= 0x1FFF}, //DBG_IQ_SpikeNR_OFFSET_Y
    {.bank= 9, .offset= 170, .mask= 0x1FFF}, //DBG_IQ_SpikeNR_CEN_X
    {.bank= 9, .offset= 172, .mask= 0x1FFF}, //DBG_IQ_SpikeNR_CEN_Y
    {.bank= 9, .offset= 174, .mask= 0xFF}, //DBG_IQ_SpikeNR_RATE_X
    {.bank= 9, .offset= 174, .mask= 0xFF00}, //DBG_IQ_SpikeNR_RATE_Y

    {.bank= 9, .offset= 176, .mask= 0x0F}, //DBG_IQ_SpikeNR_DIST_SFT
    {.bank= 9, .offset= 182, .mask= 0xFF}, //DBG_IQ_SpikeNR_DIST_LUT

    {.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_SpikeNR_END
 */

int HalIsp_ApplySpikeNR(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage)
{
    isp_hal_handle *hal = (isp_hal_handle*)hnd;
    volatile  infinity2_reg_isp0 *dest0 = hal->isp0_cfg;
    volatile  infinity2_reg_isp1 *dest = hal->isp1_cfg;
    infinity2_reg_isp1 *src = &pIspShd->isp1_cfg;
    volatile  infinity2_reg_isp9 *dest1 = hal->isp9_cfg;
    infinity2_reg_isp9 *src1 = &pIspShd->isp9_cfg;

    //UartSendTrace("%s\n",__FUNCTION__);

    dest->reg_spike_nr_en = src->reg_spike_nr_en;
    dest1->reg_spike_nr_dist_en = src1->reg_spike_nr_dist_en;
    dest->reg_spike_nr_coef = src->reg_spike_nr_coef;

    dest->reg_p_thrd = src->reg_p_thrd;
    dest->reg_p_thrd_1 = src->reg_p_thrd_1;
    dest->reg_p_thrd_2 = src->reg_p_thrd_2;
    dest->reg_p_thrd_3 = src->reg_p_thrd_3;

    dest->reg_d_11_21_step = src->reg_d_11_21_step;
    dest->reg_d_31_step = src->reg_d_31_step;
    dest->reg_yp_22_step = src->reg_yp_22_step;

    dest1->reg_bnr_orient = src1->reg_bnr_orient;
    dest1->reg_bnr_offset_x = src1->reg_bnr_offset_x;
    dest1->reg_bnr_offset_y = src1->reg_bnr_offset_y;
    dest1->reg_bnr_cen_x = src1->reg_bnr_cen_x;
    dest1->reg_bnr_cen_y = src1->reg_bnr_cen_y;
    dest1->reg_bnr_rate_x = src1->reg_bnr_rate_x;
    dest1->reg_bnr_rate_y = src1->reg_bnr_rate_y;

    dest1->reg_bnr_dist_sft_x0 = src1->reg_bnr_dist_sft_x0;
    dest1->reg_bnr_dist_sft_x1 = src1->reg_bnr_dist_sft_x1;
    dest1->reg_bnr_dist_sft_x2 = src1->reg_bnr_dist_sft_x2;
    dest1->reg_bnr_dist_sft_x3 = src1->reg_bnr_dist_sft_x3;
    dest1->reg_bnr_dist_sft_x4 = src1->reg_bnr_dist_sft_x4;

    dest1->reg_bnr_dist_lut_y0 = src1->reg_bnr_dist_lut_y0;
    dest1->reg_bnr_dist_lut_y1 = src1->reg_bnr_dist_lut_y1;
    dest1->reg_bnr_dist_lut_y2 = src1->reg_bnr_dist_lut_y2;
    dest1->reg_bnr_dist_lut_y3 = src1->reg_bnr_dist_lut_y3;
    dest1->reg_bnr_dist_lut_y4 = src1->reg_bnr_dist_lut_y4;
    dest1->reg_bnr_dist_lut_y5 = src1->reg_bnr_dist_lut_y5;

    //-------------------------------------------------------------------//
    if (src->reg_spike_nr_en == true)
        dest0->reg_en_isp_denoise = true;
    //-------------------------------------------------------------------//

    return 0;
}

/*
    {.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_BSNR_START

    {.bank= 1, .offset= 224, .mask= 0x01}, //DBG_IQ_BSNR_EN
    {.bank= 9, .offset= 162, .mask= 0x02}, //DBG_IQ_BSNR_DIST_EN

    {.bank= 1, .offset= 162, .mask= 0xE0}, //DBG_IQ_BSNR_ALPHA_STEP_R
    {.bank= 1, .offset= 226, .mask= 0xE0}, //DBG_IQ_BSNR_ALPHA_STEP_G
    {.bank= 1, .offset= 162, .mask= 0xE000}, //DBG_IQ_BSNR_ALPHA_STEP_B

    {.bank= 1, .offset= 160, .mask= 0xFF}, //DBG_IQ_BSNR_STD_LOW_THRD_R
    {.bank= 1, .offset= 224, .mask= 0xFF00}, //DBG_IQ_BSNR_STD_LOW_THRD_G
    {.bank= 1, .offset= 160, .mask= 0xFF00}, //DBG_IQ_BSNR_STD_LOW_THRD_B

    {.bank= 1, .offset= 162, .mask= 0x0F}, //DBG_IQ_BSNR_STRENGTH_GAIN_R
    {.bank= 1, .offset= 226, .mask= 0x0F}, //DBG_IQ_BSNR_STRENGTH_GAIN_G
    {.bank= 1, .offset= 162, .mask= 0xF00}, //DBG_IQ_BSNR_STRENGTH_GAIN_B

    {.bank= 1, .offset= 164, .mask= 0x0F}, //DBG_IQ_BSNR_TABLE_R
    {.bank= 1, .offset= 228, .mask= 0x0F}, //DBG_IQ_BSNR_TABLE_G
    {.bank= 1, .offset= 172, .mask= 0x0F}, //DBG_IQ_BSNR_TABLE_B
    {.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_BSNR_END
 */

int HalIsp_ApplyBSNR(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage)
{
    isp_hal_handle *hal = (isp_hal_handle*)hnd;
    volatile  infinity2_reg_isp0 *dest0 = hal->isp0_cfg;
    volatile  infinity2_reg_isp1 *dest = hal->isp1_cfg;
    infinity2_reg_isp1 *src = &pIspShd->isp1_cfg;
    volatile  infinity2_reg_isp9 *dest1 = hal->isp9_cfg;
    infinity2_reg_isp9 *src1 = &pIspShd->isp9_cfg;

    //UartSendTrace("%s\n",__FUNCTION__);

    dest->reg_snr_en = src->reg_snr_en;
    dest1->reg_snr_mean_dist_en = src1->reg_snr_mean_dist_en;

    dest->reg_snr_alpha_step_r_pix = src->reg_snr_alpha_step_r_pix;
    dest->reg_snr_alpha_step = src->reg_snr_alpha_step;
    dest->reg_snr_alpha_step_b_pix = src->reg_snr_alpha_step_b_pix;

    dest->reg_snr_std_low_thrd_r_pix = src->reg_snr_std_low_thrd_r_pix;
    dest->reg_snr_std_low_thrd = src->reg_snr_std_low_thrd;
    dest->reg_snr_std_low_thrd_b_pix = src->reg_snr_std_low_thrd_b_pix;

    dest->reg_snr_strength_gain_r_pix= src->reg_snr_strength_gain_r_pix;
    dest->reg_snr_strength_gain= src->reg_snr_strength_gain;
    dest->reg_snr_strength_gain_b_pix= src->reg_snr_strength_gain_b_pix;

    _HalIsp_RegCopy( (unsigned short *)reg_addr(dest,isp1_reg_snr_table_r_pix),
                     (unsigned short *)reg_addr(src,isp1_reg_snr_table_r_pix),
                     (unsigned short *)reg_addr(src,isp1_reg_snr_table_r_pix_3)
                   );
    _HalIsp_RegCopy( (unsigned short *)reg_addr(dest,isp1_reg_snr_table),
                     (unsigned short *)reg_addr(src,isp1_reg_snr_table),
                     (unsigned short *)reg_addr(src,isp1_reg_snr_table_3)
                   );
    _HalIsp_RegCopy( (unsigned short *)reg_addr(dest,isp1_reg_snr_table_b_pix),
                     (unsigned short *)reg_addr(src,isp1_reg_snr_table_b_pix),
                     (unsigned short *)reg_addr(src,isp1_reg_snr_table_b_pix_3)
                   );

    //-------------------------------------------------------------------//
    if (src->reg_snr_en == true)
        dest0->reg_en_isp_denoise = true;
    //-------------------------------------------------------------------//

    return 0;
}
/*
{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_DM_START
{.bank= 0, .offset= 224, .mask= 0x01}, //DBG_IQ_DM_EN
{.bank= 0, .offset= 224, .mask= 0x02}, //DBG_IQ_DM_CFAI_BYPASS

{.bank= 0, .offset= 226, .mask= 0x0F}, //DBG_IQ_DM_DVH_SLOPE
{.bank= 0, .offset= 226, .mask= 0xF00}, //DBG_IQ_DM_DVH_THRD

{.bank= 0, .offset= 228, .mask= 0x0F}, //DBG_IQ_DM_CPXWEI_SLOPE
{.bank= 0, .offset= 228, .mask= 0xFF00}, //DBG_IQ_DM_CPXWEI_THRD

{.bank= 0, .offset= 230, .mask= 0x0F}, //DBG_IQ_DM_NB_SLOPE_M
{.bank= 0, .offset= 230, .mask= 0xF00}, //DBG_IQ_DM_NB_SLOPE_S

{.bank= 0, .offset= 234, .mask= 0x7F}, //DBG_IQ_DM_NB_EDGE_ADAPT_OFT
{.bank= 0, .offset= 234, .mask= 0x7F00}, //DBG_IQ_DM_NB_CROSSCHK_OFT

{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_DM_END
*/
int HalIsp_ApplyDM(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage)
{
    isp_hal_handle *hal = (isp_hal_handle*)hnd;
    volatile  infinity2_reg_isp0 *dest = hal->isp0_cfg;
    infinity2_reg_isp0 *src = &pIspShd->isp0_cfg;

    //UartSendTrace("%s\n",__FUNCTION__);

    dest->reg_cfai_en = src->reg_cfai_en;
    dest->reg_cfai_bypass = src->reg_cfai_bypass;

    dest->reg_demosaic_dvh_slope = src->reg_demosaic_dvh_slope;
    dest->reg_demosaic_dvh_thrd = src->reg_demosaic_dvh_thrd;

    dest->reg_demosaic_cpxwei_slope = src->reg_demosaic_cpxwei_slope;
    dest->reg_demosaic_cpxwei_thrd = src->reg_demosaic_cpxwei_thrd;

    dest->reg_nb_slope_m = src->reg_nb_slope_m;
    dest->reg_nb_slope_s= src->reg_nb_slope_s;

    dest->reg_edgeadapffset= src->reg_edgeadapffset;
    dest->reg_crosscheckffset= src->reg_crosscheckffset;

    //----------------------------------------------------------//


    return 0;
}


/*
    {.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_GAMA_A2A_ISP_START
    {.bank= 0, .offset= 140, .mask= 0x01}, //DBG_IQ_GAMA_A2A_ISP_EN
    {.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_GAMA_A2A_ISP_LUT_R
    {.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_GAMA_A2A_ISP_LUT_G
    {.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_GAMA_A2A_ISP_LUT_B

    {.bank= 0, .offset= 140, .mask= 0x02}, //DBG_IQ_GAMA_A2A_ISP_MAX_EN
    {.bank= 0, .offset= 140, .mask= 0xFFC}, //DBG_IQ_GAMA_A2A_ISP_MAX_DATA

    {.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_GAMA_A2A_ISP_END
 */
int HalIsp_GammaA2A(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage)
{
    isp_hal_handle *hal = (isp_hal_handle*)hnd;
    volatile  infinity2_reg_isp0 *dest = hal->isp0_cfg;
    infinity2_reg_isp0 *src = &pIspShd->isp0_cfg;

    //UartSendTrace("%s\n",__FUNCTION__);

    dest->reg_isp_gamma_10to10_en = src->reg_isp_gamma_10to10_en;
    dest->reg_isp_gamma_10to10_max_en = src->reg_isp_gamma_10to10_max_en;
    dest->reg_isp_gamma_10to10_max_data = src->reg_isp_gamma_10to10_max_data;
    return 0;
}
/*
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_GE_START
	{.bank= 1, .offset= 128, .mask= 0x01}, //DBG_IQ_GE_EN
	{.bank= 1, .offset= 128, .mask= 0x3FF0}, //DBG_IQ_GE_DIFF_TH
	{.bank= 1, .offset= 136, .mask= 0x07}, //DBG_IQ_GE_DIFF_STEP
	{.bank= 1, .offset= 136, .mask= 0x700}, //DBG_IQ_GE_DIFF_STEP2
	{.bank= 1, .offset= 138, .mask= 0x3FF}, //DBG_IQ_GE_DIFF_TH2
	{.bank= 1, .offset= 140, .mask= 0x3FF}, //DBG_IQ_GE_DIFF_MAX
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_GE_END
*/

int HalIsp_ApplyGE(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage)
{
    isp_hal_handle *hal = (isp_hal_handle*)hnd;
    volatile  infinity2_reg_isp1 *dest = hal->isp1_cfg;
    infinity2_reg_isp1 *src = &pIspShd->isp1_cfg;

    //UartSendTrace("%s\n",__FUNCTION__);

    dest->reg_isp_geq_enable = src->reg_isp_geq_enable;
    dest->reg_isp_geq_th = src->reg_isp_geq_th;
    dest->reg_isp_geq_abs_shift = src->reg_isp_geq_abs_shift;
    dest->reg_isp_geq_abs_shift2 = src->reg_isp_geq_abs_shift2;
    dest->reg_isp_geq_th2 = src->reg_isp_geq_th2;
    dest->reg_isp_geq_th_max = src->reg_isp_geq_th_max;
    return 0;
}
/*
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_ANTICT_START
	{.bank= 1, .offset= 142, .mask= 0x01}, //DBG_IQ_ANTICT_EN
	{.bank= 1, .offset= 144, .mask= 0xFF}, //DBG_IQ_ANTICT_TH
	{.bank= 1, .offset= 144, .mask= 0xFF00}, //DBG_IQ_ANTICT_SFT
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_ANTICT_END
*/

int HalIsp_ApplyANTICT(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage)
{
    isp_hal_handle *hal = (isp_hal_handle*)hnd;
    volatile infinity2_reg_isp1 *dest = hal->isp1_cfg;
    infinity2_reg_isp1 *src = &pIspShd->isp1_cfg;

    //UartSendTrace("%s\n",__FUNCTION__);

    dest->reg_ci_anti_ct_en = src->reg_ci_anti_ct_en;
    dest->reg_ci_anti_ct_th = src->reg_ci_anti_ct_th;
    dest->reg_ci_anti_ct_sft = src->reg_ci_anti_ct_sft;
    return 0;
}
/*
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_RGBIR_START
	{.bank= 8, .offset= 0, .mask= 0x01}, //DBG_IQ_RGBIR_EN
	{.bank= 8, .offset= 0, .mask= 0x02}, //DBG_IQ_RGBIR_MODE4x4
	{.bank= 8, .offset= 0, .mask= 0x04}, //DBG_IQ_RGBIR_MODE
	{.bank= 8, .offset= 0, .mask= 0x08}, //DBG_IQ_RGBIR_OFFSET_EN
	{.bank= 8, .offset= 0, .mask= 0x10}, //DBG_IQ_RGBIR_HIST_IR_EN
	{.bank= 8, .offset= 0, .mask= 0x20}, //DBG_IQ_RGBIR_IMONITOR_EN
	{.bank= 8, .offset= 8, .mask= 0x0F}, //DBG_IQ_RGBIR_OFT_RATIO_BY_Y
	{.bank= 8, .offset= 12, .mask= 0xFFF}, //DBG_IQ_RGBIR_OFT_R_RATIO_BY_Y
	{.bank= 8, .offset= 24, .mask= 0xFFF}, //DBG_IQ_RGBIR_OFT_G_RATIO_BY_Y
	{.bank= 8, .offset= 36, .mask= 0xFFF}, //DBG_IQ_RGBIR_OFT_B_RATIO_BY_Y
	{.bank= 8, .offset= 48, .mask= 0xFFFF}, //DBG_IQ_RGBIR_DUMMY
	{.bank= 8, .offset= 50, .mask= 0xFFF}, //DBG_IQ_RGBIR_OFT_CMP_RATIO
	{.bank= 8, .offset= 50, .mask= 0xF000}, //DBG_IQ_RGBIR_DIR_SCALER
	{.bank= 5, .offset= 2, .mask= 0x10}, //DBG_IQ_RGBIR_2x2_G_LOC
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_RGBIR_END
*/

int HalIsp_ApplyRGBIR(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage)
{
    isp_hal_handle *hal = (isp_hal_handle*)hnd;
    volatile  infinity2_reg_isp8 *dest = hal->isp8_cfg;
    infinity2_reg_isp8 *src = &pIspShd->isp8_cfg;
    volatile  infinity2_reg_isp5 *dest1 = hal->isp5_cfg;
    infinity2_reg_isp5 *src1 = &pIspShd->isp5_cfg;
    //UartSendTrace("%s\n",__FUNCTION__);

    dest->reg_rgbir_en = src->reg_rgbir_en;
    dest->reg_mode4x4 = src->reg_mode4x4;
    dest->reg_ir_mode = src->reg_ir_mode;
    dest->reg_ir_offset_en = src->reg_ir_offset_en;
    dest->reg_hist_ir_en = src->reg_hist_ir_en;
    dest->reg_imonitor_en = src->reg_imonitor_en;
    dest->reg_ir_oft_ratio_by_y_shift_0 = src->reg_ir_oft_ratio_by_y_shift_0;
    dest->reg_ir_oft_ratio_by_y_shift_1 = src->reg_ir_oft_ratio_by_y_shift_1;
    dest->reg_ir_oft_ratio_by_y_shift_2 = src->reg_ir_oft_ratio_by_y_shift_2;
    dest->reg_ir_oft_ratio_by_y_shift_3 = src->reg_ir_oft_ratio_by_y_shift_3;
    dest->reg_ir_oft_ratio_by_y_shift_4 = src->reg_ir_oft_ratio_by_y_shift_4;

    dest->reg_ir_oft_r_ratio_by_y_0 = src->reg_ir_oft_r_ratio_by_y_0;
    dest->reg_ir_oft_r_ratio_by_y_1 = src->reg_ir_oft_r_ratio_by_y_1;
    dest->reg_ir_oft_r_ratio_by_y_2 = src->reg_ir_oft_r_ratio_by_y_2;
    dest->reg_ir_oft_r_ratio_by_y_3 = src->reg_ir_oft_r_ratio_by_y_3;
    dest->reg_ir_oft_r_ratio_by_y_4 = src->reg_ir_oft_r_ratio_by_y_4;
    dest->reg_ir_oft_r_ratio_by_y_5 = src->reg_ir_oft_r_ratio_by_y_5;

    dest->reg_ir_oft_g_ratio_by_y_0 = src->reg_ir_oft_g_ratio_by_y_0;
    dest->reg_ir_oft_g_ratio_by_y_1 = src->reg_ir_oft_g_ratio_by_y_1;
    dest->reg_ir_oft_g_ratio_by_y_2 = src->reg_ir_oft_g_ratio_by_y_2;
    dest->reg_ir_oft_g_ratio_by_y_3 = src->reg_ir_oft_g_ratio_by_y_3;
    dest->reg_ir_oft_g_ratio_by_y_4 = src->reg_ir_oft_g_ratio_by_y_4;
    dest->reg_ir_oft_g_ratio_by_y_5 = src->reg_ir_oft_g_ratio_by_y_5;

    dest->reg_ir_oft_b_ratio_by_y_0 = src->reg_ir_oft_b_ratio_by_y_0;
    dest->reg_ir_oft_b_ratio_by_y_1 = src->reg_ir_oft_b_ratio_by_y_1;
    dest->reg_ir_oft_b_ratio_by_y_2 = src->reg_ir_oft_b_ratio_by_y_2;
    dest->reg_ir_oft_b_ratio_by_y_3 = src->reg_ir_oft_b_ratio_by_y_3;
    dest->reg_ir_oft_b_ratio_by_y_4 = src->reg_ir_oft_b_ratio_by_y_4;
    dest->reg_ir_oft_b_ratio_by_y_5 = src->reg_ir_oft_b_ratio_by_y_5;

    dest->reg_ir_dummy = src->reg_ir_dummy;
    dest->reg_ir_oft_cmp_ratio = src->reg_ir_oft_cmp_ratio;
    dest->reg_ir_dir_scaler = src->reg_ir_dir_scaler;
    dest1->reg_rgbir2x2_g_loc = src1->reg_rgbir2x2_g_loc;
    return 0;
}
/*
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_FalseColor_START
	{.bank= 0, .offset= 232, .mask= 0x01}, //DBG_IQ_FalseColor_EN
	{.bank= 0, .offset= 232, .mask= 0x700}, //DBG_IQ_FalseColor_R_RATIO
	{.bank= 0, .offset= 232, .mask= 0x7000}, //DBG_IQ_FalseColor_B_RATIO
	{.bank= 0, .offset= 232, .mask= 0x70}, //DBG_IQ_FalseColor_COLOR_RATIO
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_FalseColor_END
*/

int HalIsp_ApplyFalseColor(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage)
{
    isp_hal_handle *hal = (isp_hal_handle*)hnd;
    volatile  infinity2_reg_isp0 *dest = hal->isp0_cfg;
    infinity2_reg_isp0 *src = &pIspShd->isp0_cfg;

    //UartSendTrace("%s\n",__FUNCTION__);

    dest->reg_rgb_falsecolor_en = src->reg_rgb_falsecolor_en;
    dest->reg_rgb_falsecolor_r_ratio = src->reg_rgb_falsecolor_r_ratio;
    dest->reg_rgb_falsecolor_b_ratio = src->reg_rgb_falsecolor_b_ratio;
    dest->reg_rgb_falsecolor_color_ratio = src->reg_rgb_falsecolor_color_ratio;

    //-------------------------------------------------------------------//
    dest->reg_rgb_falsecolor_minmax_round = 1;  //minmax rounding enable
    //-------------------------------------------------------------------//

    return 0;
}
/*
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_DECOMP_START
	{.bank= 9, .offset= 96, .mask= 0x01}, //DBG_IQ_DECOMP_EN
	{.bank= 9, .offset= 98, .mask= 0xFFFF}, //DBG_IQ_DECOMP_R0_F0
	{.bank= 9, .offset= 100, .mask= 0x0F}, //DBG_IQ_DECOMP_R0_F1
	{.bank= 9, .offset= 102, .mask= 0xFFFF}, //DBG_IQ_DECOMP_R0_F2
	{.bank= 9, .offset= 104, .mask= 0xFFFF}, //DBG_IQ_DECOMP_R1_F0
	{.bank= 9, .offset= 106, .mask= 0x0F}, //DBG_IQ_DECOMP_R1_F1
	{.bank= 9, .offset= 108, .mask= 0xFFFF}, //DBG_IQ_DECOMP_R1_F2
	{.bank= 9, .offset= 110, .mask= 0xFFFF}, //DBG_IQ_DECOMP_R2_F0
	{.bank= 9, .offset= 112, .mask= 0x0F}, //DBG_IQ_DECOMP_R2_F1
	{.bank= 9, .offset= 114, .mask= 0xFFFF}, //DBG_IQ_DECOMP_R2_F2
	{.bank= 9, .offset= 116, .mask= 0xFFFF}, //DBG_IQ_DECOMP_R3_F0
	{.bank= 9, .offset= 118, .mask= 0x0F}, //DBG_IQ_DECOMP_R3_F1
	{.bank= 9, .offset= 120, .mask= 0xFFFF}, //DBG_IQ_DECOMP_R3_F2
	{.bank= 9, .offset= 122, .mask= 0xFFFF}, //DBG_IQ_DECOMP_RANGE0
	{.bank= 9, .offset= 124, .mask= 0xFFFF}, //DBG_IQ_DECOMP_RANGE1
	{.bank= 9, .offset= 126, .mask= 0xFFFF}, //DBG_IQ_DECOMP_RANGE2
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_DECOMP_END
*/

int HalIsp_ApplyDECOMP(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage)
{
    isp_hal_handle *hal = (isp_hal_handle*)hnd;
    volatile  infinity2_reg_isp9 *dest = hal->isp9_cfg;
    infinity2_reg_isp9 *src = &pIspShd->isp9_cfg;

    //UartSendTrace("%s\n",__FUNCTION__);

    dest->reg_decomp_en = src->reg_decomp_en;
    dest->reg_decomp_r0_f0 = src->reg_decomp_r0_f0;
    dest->reg_decomp_r0_f1 = src->reg_decomp_r0_f1;
    dest->reg_decomp_r0_f2 = src->reg_decomp_r0_f2;
    dest->reg_decomp_r1_f0 = src->reg_decomp_r1_f0;
    dest->reg_decomp_r1_f1 = src->reg_decomp_r1_f1;
    dest->reg_decomp_r1_f2 = src->reg_decomp_r1_f2;
    dest->reg_decomp_r2_f0 = src->reg_decomp_r2_f0;
    dest->reg_decomp_r2_f1 = src->reg_decomp_r2_f1;
    dest->reg_decomp_r2_f2 = src->reg_decomp_r2_f2;
    dest->reg_decomp_r3_f0 = src->reg_decomp_r3_f0;
    dest->reg_decomp_r3_f1 = src->reg_decomp_r3_f1;
    dest->reg_decomp_r3_f2 = src->reg_decomp_r3_f2;
    dest->reg_decomp_range0 = src->reg_decomp_range0;
    dest->reg_decomp_range1 = src->reg_decomp_range1;
    dest->reg_decomp_range2 = src->reg_decomp_range2;
    return 0;
}
/*
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_FPN_START
	{.bank= 4, .offset= 128, .mask= 0x01}, //DBG_IQ_FPN_EN
	{.bank= 4, .offset= 130, .mask= 0x0FFF}, //DBG_IQ_FPN_ROI_X
	{.bank= 4, .offset= 132, .mask= 0x0FFF}, //DBG_IQ_FPN_ROI_Y
	{.bank= 4, .offset= 134, .mask= 0x0FFF}, //DBG_IQ_FPN_WIDTH
	{.bank= 4, .offset= 136, .mask= 0x001F}, //DBG_IQ_FPN_HEIGHT
	{.bank= 4, .offset= 138, .mask= 0x7FFF}, //DBG_IQ_FPN_PRE_OFFSET
	{.bank= 4, .offset= 138, .mask= 0x8000}, //DBG_IQ_FPN_PRE_OFFSET_SIGN
	{.bank= 4, .offset= 140, .mask= 0xFFFF}, //DBG_IQ_FPN_DIFF_TH
	{.bank= 4, .offset= 142, .mask= 0xFFF}, //DBG_IQ_FPN_CMP_RATIO
	{.bank= 4, .offset= 144, .mask= 0x001F}, //DBG_IQ_FPN_FRAME_NUM
	{.bank= 4, .offset= 128, .mask= 0x8}, //DBG_IQ_FPN_SW_OFFSET_EN
	{.bank= 255, .offset= 0, .mask= 0xFFF}, //DBG_IQ_FPN_SW_TABLE
	{.bank= 255, .offset= 0, .mask= 0xFFF}, //DBG_IQ_FPN_SW_CMP_RATIO
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_FPN_END
*/

int HalIsp_ApplyFPN(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage)
{
    isp_hal_handle *hal = (isp_hal_handle*)hnd;
    volatile  infinity2_reg_isp4 *dest = hal->isp4_cfg;
    infinity2_reg_isp4 *src = &pIspShd->isp4_cfg;

    //UartSendTrace("%s\n",__FUNCTION__);

    dest->reg_fpn_en = src->reg_fpn_en;
    dest->reg_fpn_start_x = src->reg_fpn_start_x;
    dest->reg_fpn_start_y = src->reg_fpn_start_y;
    dest->reg_fpn_width = src->reg_fpn_width;
    dest->reg_fpn_height = src->reg_fpn_height;
    dest->reg_fpn_preoffset = src->reg_fpn_preoffset;
    dest->reg_fpn_preoffset_sign = src->reg_fpn_preoffset_sign;
    dest->reg_fpn_diff_th = src->reg_fpn_diff_th;
    dest->reg_fpn_cmp_ratio = src->reg_fpn_cmp_ratio;
    dest->reg_fpn_frame_num = src->reg_fpn_frame_num;
    dest->reg_sw_offset_on = src->reg_sw_offset_on;
    return 0;
}
/*
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_FPN_P1_START
	{.bank= 11, .offset= 0, .mask= 0x01}, //DBG_IQ_FPN_P1_EN
	{.bank= 11, .offset= 2, .mask= 0x0FFF}, //DBG_IQ_FPN_P1_ROI_X
	{.bank= 11, .offset= 4, .mask= 0x0FFF}, //DBG_IQ_FPN_P1_ROI_Y
	{.bank= 11, .offset= 6, .mask= 0x0FFF}, //DBG_IQ_FPN_P1_WIDTH
	{.bank= 11, .offset= 0, .mask= 0x01F0}, //DBG_IQ_FPN_P1_HEIGHT
	{.bank= 11, .offset= 8, .mask= 0x7FFF}, //DBG_IQ_FPN_P1_PRE_OFFSET
	{.bank= 11, .offset= 8, .mask= 0x8000}, //DBG_IQ_FPN_P1_PRE_OFFSET_SIGN
	{.bank= 11, .offset= 10, .mask= 0xFFFF}, //DBG_IQ_FPN_P1_DIFF_TH
	{.bank= 11, .offset= 12, .mask= 0xFFF}, //DBG_IQ_FPN_P1_CMP_RATIO
	{.bank= 11, .offset= 0, .mask= 0x7C00}, //DBG_IQ_FPN_P1_FRAME_NUM
	{.bank= 11, .offset= 0, .mask= 0x8}, //DBG_IQ_FPN_P1_SW_OFFSET_EN
	{.bank= 255, .offset= 0, .mask= 0xFFF}, //DBG_IQ_FPN_P1_SW_TABLE
	{.bank= 255, .offset= 0, .mask= 0xFFF}, //DBG_IQ_FPN_P1_SW_CMP_RATIO
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_FPN_P1_END
*/

int HalIsp_ApplyFPNP1(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage)
{
    isp_hal_handle *hal = (isp_hal_handle*)hnd;
    volatile  infinity2_reg_isp11 *dest = hal->isp11_cfg;
    infinity2_reg_isp11 *src = &pIspShd->isp11_cfg;

    //UartSendTrace("%s\n",__FUNCTION__);

    dest->reg_fpn_en = src->reg_fpn_en;
    dest->reg_fpn_start_x = src->reg_fpn_start_x;
    dest->reg_fpn_start_y = src->reg_fpn_start_y;
    dest->reg_fpn_width = src->reg_fpn_width;
    dest->reg_fpn_height = src->reg_fpn_height;
    dest->reg_fpn_preoffset = src->reg_fpn_preoffset;
    dest->reg_fpn_preoffset_sign = src->reg_fpn_preoffset_sign;
    dest->reg_fpn_diff_th = src->reg_fpn_diff_th;
    dest->reg_fpn_cmp_ratio = src->reg_fpn_cmp_ratio;
    dest->reg_fpn_frame_num = src->reg_fpn_frame_num;
    dest->reg_sw_offset_on = src->reg_sw_offset_on;
    return 0;
}
/*
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_OBC_P1_START
	{.bank= 11, .offset= 48, .mask= 0x02}, //DBG_IQ_OBC_P1_EN
	{.bank= 11, .offset= 32, .mask= 0xFFF}, //DBG_IQ_OBC_P1_R_OFST
	{.bank= 11, .offset= 34, .mask= 0xFFF}, //DBG_IQ_OBC_P1_GR_OFST
	{.bank= 11, .offset= 36, .mask= 0xFFF}, //DBG_IQ_OBC_P1_GB_OFST
	{.bank= 11, .offset= 38, .mask= 0xFFF}, //DBG_IQ_OBC_P1_B_OFST
	{.bank= 11, .offset= 40, .mask= 0x3FFF}, //DBG_IQ_OBC_P1_R_GAIN
	{.bank= 11, .offset= 42, .mask= 0x3FFF}, //DBG_IQ_OBC_P1_GR_GAIN
	{.bank= 11, .offset= 44, .mask= 0x3FFF}, //DBG_IQ_OBC_P1_GB_GAIN
	{.bank= 11, .offset= 46, .mask= 0x3FFF}, //DBG_IQ_OBC_P1_B_GAIN
	{.bank= 11, .offset= 48, .mask= 0x04}, //DBG_IQ_OBC_P1_DITHER_EN
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_OBC_P1_END
*/

int HalIsp_ApplyOBCP1(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage)
{
    isp_hal_handle *hal = (isp_hal_handle*)hnd;
    volatile  infinity2_reg_isp11 *dest = hal->isp11_cfg;
    infinity2_reg_isp11 *src = &pIspShd->isp11_cfg;

    //UartSendTrace("%s\n",__FUNCTION__);

    dest->reg_isp_obc_en = src->reg_isp_obc_en;
    dest->reg_isp_obc_r_ofst = src->reg_isp_obc_r_ofst;
    dest->reg_isp_obc_gr_ofst = src->reg_isp_obc_gr_ofst;
    dest->reg_isp_obc_gb_ofst = src->reg_isp_obc_gb_ofst;
    dest->reg_isp_obc_b_ofst = src->reg_isp_obc_b_ofst;
    dest->reg_isp_obc_r_gain = src->reg_isp_obc_r_gain;
    dest->reg_isp_obc_gr_gain = src->reg_isp_obc_gr_gain;
    dest->reg_isp_obc_gb_gain = src->reg_isp_obc_gb_gain;
    dest->reg_isp_obc_b_gain = src->reg_isp_obc_b_gain;
    dest->reg_obc_dith_en = src->reg_obc_dith_en;
    return 0;
}
/*
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_GAMA_16to16_ISP_START
	{.bank= 1, .offset= 10, .mask= 0x01}, //DBG_IQ_GAMA_16to16_ISP_EN
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_GAMA_16to16_ISP_LUT_R
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_GAMA_16to16_ISP_LUT_G
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_GAMA_16to16_ISP_LUT_B
	{.bank= 1, .offset= 10, .mask= 0x02}, //DBG_IQ_GAMA_16to16_ISP_MAX_EN
	{.bank= 1, .offset= 12, .mask= 0xFFFF}, //DBG_IQ_GAMA_16to16_ISP_MAX_DATA
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_GAMA_16to16_ISP_END
*/

int HalIsp_ApplyGAMA16to16ISP(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage)
{
    isp_hal_handle *hal = (isp_hal_handle*)hnd;
    volatile  infinity2_reg_isp1 *dest = hal->isp1_cfg;
    infinity2_reg_isp1 *src = &pIspShd->isp1_cfg;

    //UartSendTrace("%s\n",__FUNCTION__);

    dest->reg_gm16to16_en = src->reg_gm16to16_en;
    dest->reg_gm16to16_max_en = src->reg_gm16to16_max_en;
    dest->reg_gm16to16_max_data = src->reg_gm16to16_max_data;
    return 0;
}
/*
    {.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_GAMA_C2C_P1_ISP_START
    {.bank= 11, .offset= 52, .mask= 0x1000}, //DBG_IQ_GAMA_C2C_P1_ISP_EN
    {.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_GAMA_C2C_P1_ISP_LUT_R
    {.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_GAMA_C2C_P1_ISP_LUT_G
    {.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_GAMA_C2C_P1_ISP_LUT_B
    {.bank= 11, .offset= 52, .mask= 0x2000}, //DBG_IQ_GAMA_C2C_P1_ISP_MAX_EN
    {.bank= 11, .offset= 52, .mask= 0xFFF}, //DBG_IQ_GAMA_C2C_P1_ISP_MAX_DATA
    {.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_GAMA_C2C_P1_ISP_END
*/

int HalIsp_ApplyGAMAC2C_P1(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage)
{
    isp_hal_handle *hal = (isp_hal_handle*)hnd;
    volatile  infinity2_reg_isp11 *dest = hal->isp11_cfg;
    infinity2_reg_isp11 *src = &pIspShd->isp11_cfg;

    //UartSendTrace("%s\n",__FUNCTION__);

    dest->reg_gm12to12_en = src->reg_gm12to12_en;
    dest->reg_gm12to12_max_en = src->reg_gm12to12_max_en;
    dest->reg_gm12to12_max_data = src->reg_gm12to12_max_data;
    return 0;
}


/*
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_LSC_START
	{.bank= 0, .offset= 2, .mask= 0x10}, //DBG_IQ_LSC_EN
	{.bank= 1, .offset= 208, .mask= 0xFF}, //DBG_IQ_LSC_GLB_GAIN
	{.bank= 1, .offset= 2, .mask= 0xFFF}, //DBG_IQ_LSC_CENTER_X
	{.bank= 1, .offset= 4, .mask= 0xFFF}, //DBG_IQ_LSC_CENTER_Y
	{.bank= 1, .offset= 6, .mask= 0x1F}, //DBG_IQ_LSC_SHIFT
	{.bank= 1, .offset= 8, .mask= 0x02}, //DBG_IQ_LSC_DITH_EN
	{.bank= 1, .offset= 32, .mask= 0xFF}, //DBG_IQ_LSC_R_GAIN_TABLE
	{.bank= 1, .offset= 64, .mask= 0xFF}, //DBG_IQ_LSC_G_GAIN_TABLE
	{.bank= 1, .offset= 96, .mask= 0xFF}, //DBG_IQ_LSC_B_GAIN_TABLE
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_LSC_END
*/

int HalIsp_ApplyLSC(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage)
{
    isp_hal_handle *hal = (isp_hal_handle*)hnd;
    volatile  infinity2_reg_isp0 *dest = hal->isp0_cfg;
    infinity2_reg_isp0 *src = &pIspShd->isp0_cfg;
    volatile  infinity2_reg_isp1 *dest1 = hal->isp1_cfg;
    infinity2_reg_isp1 *src1 = &pIspShd->isp1_cfg;

    //UartSendTrace("%s\n",__FUNCTION__);

    dest->reg_en_isp_lsc = src->reg_en_isp_lsc;
    dest1->reg_isp_lsc_glb_gain = src1->reg_isp_lsc_glb_gain;
    dest1->reg_isp_lsc_center_x = src1->reg_isp_lsc_center_x;
    dest1->reg_isp_lsc_center_y = src1->reg_isp_lsc_center_y;
    dest1->reg_isp_lsc_shift = src1->reg_isp_lsc_shift;
    dest1->reg_lsc_dith_en = src1->reg_lsc_dith_en;
    //dest1->reg_isp_lsc_r_gain_table_0 = src1->reg_isp_lsc_r_gain_table_0;
    _HalIsp_RegCopy( (unsigned short *)reg_addr(dest1,isp1_reg_isp_lsc_r_gain_table_0),
                     (unsigned short *)reg_addr(src1,isp1_reg_isp_lsc_r_gain_table_0),
                     (unsigned short *)reg_addr(src1,isp1_reg_isp_lsc_r_gain_table_31)
                   );
    //dest1->reg_isp_lsc_g_gain_table_0 = src1->reg_isp_lsc_g_gain_table_0;
    _HalIsp_RegCopy( (unsigned short *)reg_addr(dest1,isp1_reg_isp_lsc_g_gain_table_0),
                     (unsigned short *)reg_addr(src1,isp1_reg_isp_lsc_g_gain_table_0),
                     (unsigned short *)reg_addr(src1,isp1_reg_isp_lsc_g_gain_table_31)
                   );
    //dest1->reg_isp_lsc_b_gain_table_0 = src1->reg_isp_lsc_b_gain_table_0;
    _HalIsp_RegCopy( (unsigned short *)reg_addr(dest1,isp1_reg_isp_lsc_b_gain_table_0),
                     (unsigned short *)reg_addr(src1,isp1_reg_isp_lsc_b_gain_table_0),
                     (unsigned short *)reg_addr(src1,isp1_reg_isp_lsc_b_gain_table_31)
                   );
    return 0;
}
/*
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_LSC_P1_START
	{.bank= 11, .offset= 90, .mask= 0x01}, //DBG_IQ_LSC_P1_EN
	{.bank= 11, .offset= 98, .mask= 0xFF}, //DBG_IQ_LSC_P1_GLB_GAIN
	{.bank= 11, .offset= 92, .mask= 0xFFF}, //DBG_IQ_LSC_P1_CENTER_X
	{.bank= 11, .offset= 94, .mask= 0xFFF}, //DBG_IQ_LSC_P1_CENTER_Y
	{.bank= 11, .offset= 96, .mask= 0x1F}, //DBG_IQ_LSC_P1_SHIFT
	{.bank= 11, .offset= 90, .mask= 0x02}, //DBG_IQ_LSC_P1_DITH_EN
	{.bank= 11, .offset= 100, .mask= 0xFF}, //DBG_IQ_LSC_P1_R_GAIN_TABLE
	{.bank= 11, .offset= 132, .mask= 0xFF}, //DBG_IQ_LSC_P1_G_GAIN_TABLE
	{.bank= 11, .offset= 164, .mask= 0xFF}, //DBG_IQ_LSC_P1_B_GAIN_TABLE
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_LSC_P1_END
*/

int HalIsp_ApplyLSCP1(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage)
{
    isp_hal_handle *hal = (isp_hal_handle*)hnd;
    volatile infinity2_reg_isp11 *dest = hal->isp11_cfg;
    infinity2_reg_isp11 *src = &pIspShd->isp11_cfg;

    //UartSendTrace("%s\n",__FUNCTION__);

    dest->reg_lsc_en = src->reg_lsc_en;
    dest->reg_isp_lsc_glb_gain = src->reg_isp_lsc_glb_gain;
    dest->reg_isp_lsc_center_x = src->reg_isp_lsc_center_x;
    dest->reg_isp_lsc_center_y = src->reg_isp_lsc_center_y;
    dest->reg_isp_lsc_shift = src->reg_isp_lsc_shift;
    dest->reg_lsc_dith_en = src->reg_lsc_dith_en;
    //dest->reg_isp_lsc_r_gain_table_0 = src->reg_isp_lsc_r_gain_table_0;
    _HalIsp_RegCopy( (unsigned short *)reg_addr(dest,isp11_reg_isp_lsc_r_gain_table_0),
                     (unsigned short *)reg_addr(src,isp11_reg_isp_lsc_r_gain_table_0),
                     (unsigned short *)reg_addr(src,isp11_reg_isp_lsc_r_gain_table_31)
                   );
    //dest->reg_isp_lsc_g_gain_table_0 = src->reg_isp_lsc_g_gain_table_0;
    _HalIsp_RegCopy( (unsigned short *)reg_addr(dest,isp11_reg_isp_lsc_g_gain_table_0),
                     (unsigned short *)reg_addr(src,isp11_reg_isp_lsc_g_gain_table_0),
                     (unsigned short *)reg_addr(src,isp11_reg_isp_lsc_g_gain_table_31)
                   );
    //dest->reg_isp_lsc_b_gain_table_0 = src->reg_isp_lsc_b_gain_table_0;
    _HalIsp_RegCopy( (unsigned short *)reg_addr(dest,isp11_reg_isp_lsc_b_gain_table_0),
                     (unsigned short *)reg_addr(src,isp11_reg_isp_lsc_b_gain_table_0),
                     (unsigned short *)reg_addr(src,isp11_reg_isp_lsc_b_gain_table_31)
                   );
    return 0;
}

/*
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_ALSC_START
	{.bank= 3, .offset= 64, .mask= 0x01}, //DBG_IQ_ALSC_EN
	{.bank= 3, .offset= 64, .mask= 0x04}, //DBG_IQ_ALSC_DITHER_EN
	{.bank= 3, .offset= 64, .mask= 0x30}, //DBG_IQ_ALSC_X_BLK_MODE
	{.bank= 3, .offset= 64, .mask= 0xC0}, //DBG_IQ_ALSC_Y_BLK_MODE
	{.bank= 3, .offset= 64, .mask= 0x700}, //DBG_IQ_ALSC_DBG_EN
	{.bank= 3, .offset= 64, .mask= 0x1800}, //DBG_IQ_ALSC_DBG_SHIFT
	{.bank= 3, .offset= 66, .mask= 0xFFF}, //DBG_IQ_ALSC_OFFSETX
	{.bank= 3, .offset= 68, .mask= 0xFFF}, //DBG_IQ_ALSC_OFFSETY
	{.bank= 255, .offset= 86, .mask= 0xFFF}, //DBG_IQ_ALSC_SCALEX
	{.bank= 255, .offset= 88, .mask= 0xFFF}, //DBG_IQ_ALSC_SCALEY
	{.bank= 255, .offset= 0, .mask= 0xFFFF}, //DBG_IQ_ALSC_CCT_THR
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_ALSC_R_GAIN_TABLE
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_ALSC_G_GAIN_TABLE
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_ALSC_B_GAIN_TABLE
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_ALSC_END
*/
#if 0//if HW register is enabled
typedef struct
{
    // h0020, bit: 0
        /* ALSC enable*/
        //#define offset_of_reg_en_alsc (64)
        //#define mask_of_reg_en_alsc (0x1)
        unsigned int reg_en_alsc:1;

        // h0020, bit: 1
        /* */
        unsigned int :1;

        // h0020, bit: 2
        /* ALSC dither enable*/
        //#define offset_of_reg_alsc_dither_en (64)
        //#define mask_of_reg_alsc_dither_en (0x4)
        unsigned int reg_alsc_dither_en:1;

        // h0020, bit: 3
        /* */
        unsigned int :1;

        // h0020, bit: 5
        /* ALSC x block mode :  (blk size)
        0: 8
        1: 16
        2: 32
        3: 64*/
        //#define offset_of_reg_alsc_x_blk_mode (64)
        //#define mask_of_reg_alsc_x_blk_mode (0x30)
        unsigned int reg_alsc_x_blk_mode:2;

        // h0020, bit: 7
        /* ALSC y block mode :  (blk size)
        0: 8
        1: 16
        2: 32
        3: 64*/
        //#define offset_of_reg_alsc_y_blk_mode (64)
        //#define mask_of_reg_alsc_y_blk_mode (0xc0)
        unsigned int reg_alsc_y_blk_mode:2;

        // h0020, bit: 10
        /* ALSC debug mode enable ([0]:R; [1]:G;[2]:B)*/
        //#define offset_of_reg_alsc_dbg_en (64)
        //#define mask_of_reg_alsc_dbg_en (0x700)
        unsigned int reg_alsc_dbg_en:3;

        // h0020, bit: 12
        /* ALSC debug gain right shift*/
        //#define offset_of_reg_alsc_dbg_shift (64)
        //#define mask_of_reg_alsc_dbg_shift (0x1800)
        unsigned int reg_alsc_dbg_shift:2;

        // h0020, bit: 14
        /* */
        unsigned int :3;
}isp3_alsc_en;
#endif
int HalIsp_ApplyALSC(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage)
{
    isp_hal_handle *hal = (isp_hal_handle*)hnd;
    volatile infinity2_reg_isp3 *dest = (infinity2_reg_isp3*)hal->isp3_cfg;
    infinity2_reg_isp3 *src = &pIspShd->isp3_cfg;

#if 1
    dest->reg_en_alsc = src->reg_en_alsc;
    dest->reg_alsc_dither_en = src->reg_alsc_dither_en;
    dest->reg_alsc_x_blk_mode = src->reg_alsc_x_blk_mode;
    dest->reg_alsc_y_blk_mode = src->reg_alsc_y_blk_mode;
    dest->reg_alsc_dbg_en = src->reg_alsc_dbg_en;
    dest->reg_alsc_dbg_shift = src->reg_alsc_dbg_shift;
#else //if HW register shadow is enabled
    isp3_alsc_en reg;
    memset(&reg,0,sizeof(reg));
    reg.reg_en_alsc = 0;//src->reg_en_alsc;
    reg.reg_alsc_dither_en = src->reg_alsc_dither_en;
    reg.reg_alsc_x_blk_mode = src->reg_alsc_x_blk_mode;
    reg.reg_alsc_y_blk_mode = src->reg_alsc_y_blk_mode;
    reg.reg_alsc_dbg_en = src->reg_alsc_dbg_en;
    reg.reg_alsc_dbg_shift = src->reg_alsc_dbg_shift;
    *(unsigned short *)reg_addr(dest,isp3_reg_en_alsc) = *(unsigned short *) &reg;
#endif
    dest->reg_asymlsc_orientation = src->reg_asymlsc_orientation;
    dest->reg_alsc_offsetx = src->reg_alsc_offsetx;
    dest->reg_alsc_offsety = src->reg_alsc_offsety;
    dest->reg_asymlsc_scale_x = src->reg_asymlsc_scale_x;
    dest->reg_asymlsc_scale_y = src->reg_asymlsc_scale_y;

    dest->reg_alsc_gtbl_width_m1 = src->reg_alsc_gtbl_width_m1;
    dest->reg_alsc_gtbl_height_m1 = src->reg_alsc_gtbl_height_m1;
    dest->reg_alsc_gtbl_pitch = src->reg_alsc_gtbl_pitch;

/***************************************/
    dest->reg_asymlsc_scale_x = 1024;
    dest->reg_asymlsc_scale_y = 1024;
/***************************************/
    return 0;
}
/*
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_ALSC_P1_START
	{.bank= 11, .offset= 196, .mask= 0x01}, //DBG_IQ_ALSC_P1_EN
	{.bank= 11, .offset= 196, .mask= 0x04}, //DBG_IQ_ALSC_P1_DITHER_EN
	{.bank= 11, .offset= 196, .mask= 0x30}, //DBG_IQ_ALSC_P1_X_BLK_MODE
	{.bank= 11, .offset= 196, .mask= 0xC0}, //DBG_IQ_ALSC_P1_Y_BLK_MODE
	{.bank= 11, .offset= 196, .mask= 0x700}, //DBG_IQ_ALSC_P1_DBG_EN
	{.bank= 11, .offset= 196, .mask= 0x1800}, //DBG_IQ_ALSC_P1_DBG_SHIFT
	{.bank= 11, .offset= 198, .mask= 0xFFF}, //DBG_IQ_ALSC_P1_OFFSETX
	{.bank= 11, .offset= 200, .mask= 0xFFF}, //DBG_IQ_ALSC_P1_OFFSETY
	{.bank= 255, .offset= 86, .mask= 0xFFF}, //DBG_IQ_ALSC_P1_SCALEX
	{.bank= 255, .offset= 88, .mask= 0xFFF}, //DBG_IQ_ALSC_P1_SCALEY
	{.bank= 255, .offset= 0, .mask= 0xFFFF}, //DBG_IQ_ALSC_P1_CCT_THR
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_ALSC_P1_R_GAIN_TABLE
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_ALSC_P1_G_GAIN_TABLE
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_ALSC_P1_B_GAIN_TABLE
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_ALSC_P1_END
*/

int HalIsp_ApplyALSCP1(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage)
{
    isp_hal_handle *hal = (isp_hal_handle*)hnd;
    volatile infinity2_reg_isp11 *dest = (infinity2_reg_isp11*)hal->isp11_cfg;
    infinity2_reg_isp11 *src = &pIspShd->isp11_cfg;

    //UartSendTrace("%s\n",__FUNCTION__);

    dest->reg_en_alsc = src->reg_en_alsc;
    dest->reg_alsc_dither_en = src->reg_alsc_dither_en;
    dest->reg_alsc_x_blk_mode = src->reg_alsc_x_blk_mode;
    dest->reg_alsc_y_blk_mode = src->reg_alsc_y_blk_mode;
    dest->reg_alsc_dbg_en = src->reg_alsc_dbg_en;
    dest->reg_alsc_dbg_shift = src->reg_alsc_dbg_shift;

    dest->reg_asymlsc_orientation = src->reg_asymlsc_orientation;
    dest->reg_alsc_offsetx = src->reg_alsc_offsetx;
    dest->reg_alsc_offsety = src->reg_alsc_offsety;
    dest->reg_asymlsc_scale_x = src->reg_asymlsc_scale_x;
    dest->reg_asymlsc_scale_y = src->reg_asymlsc_scale_y;

    dest->reg_alsc_gtbl_width_m1 = src->reg_alsc_gtbl_width_m1;
    dest->reg_alsc_gtbl_height_m1 = src->reg_alsc_gtbl_height_m1;
    dest->reg_alsc_gtbl_pitch = src->reg_alsc_gtbl_pitch;

/***************************************/
    dest->reg_asymlsc_scale_x = 1024;
    dest->reg_asymlsc_scale_y = 1024;
/***************************************/

    return 0;
}
/*
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_HDR_START
	{.bank= 10, .offset= 0, .mask= 0x01}, //DBG_IQ_HDR_EN
	{.bank= 10, .offset= 0, .mask= 0x02}, //DBG_IQ_HDR_STITCH_EN
	{.bank= 10, .offset= 0, .mask= 0x04}, //DBG_IQ_HDR_MAPPING_EN
	{.bank= 10, .offset= 2, .mask= 0xFFFF}, //DBG_IQ_HDR_RATIO_VS
	{.bank= 10, .offset= 4, .mask= 0x3FF}, //DBG_IQ_HDR_TH1
	{.bank= 10, .offset= 6, .mask= 0x3FF}, //DBG_IQ_HDR_TH2
	{.bank= 10, .offset= 8, .mask= 0x0F}, //DBG_IQ_HDR_UVTBL_X
	{.bank= 10, .offset= 12, .mask= 0x1FFF}, //DBG_IQ_HDR_UVTBL_Y
	{.bank= 10, .offset= 36, .mask= 0x1FF}, //DBG_IQ_HDR_RATIO_VS_128
	{.bank= 10, .offset= 40, .mask= 0x01}, //DBG_IQ_HDR_DYN_RATIO_EN
	{.bank= 10, .offset= 42, .mask= 0xFFFF}, //DBG_IQ_HDR_RATIO_GAIN
	{.bank= 10, .offset= 74, .mask= 0xFFFF}, //DBG_IQ_HDR_SW_RATIO_GAIN
	{.bank= 10, .offset= 108, .mask= 0xFFFF}, //DBG_IQ_HDR_MAX_IN
	{.bank= 10, .offset= 112, .mask= 0x3F}, //DBG_IQ_HDR_GAMA_X
	{.bank= 10, .offset= 134, .mask= 0x3FF}, //DBG_IQ_HDR_GAMA_Y
	{.bank= 10, .offset= 224, .mask= 0xFFFF}, //DBG_IQ_HDR_HIST_SHFT
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_HDR_END
*/

int HalIsp_ApplyHDR(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage)
{
    isp_hal_handle *hal = (isp_hal_handle*)hnd;
    volatile infinity2_reg_isp10 *dest = (infinity2_reg_isp10*)hal->isp10_cfg;
    infinity2_reg_isp10 *src = &pIspShd->isp10_cfg;

    //UartSendTrace("%s\n",__FUNCTION__);

    dest->reg_hdr_en = src->reg_hdr_en;
    dest->reg_hdr_stitch_en = src->reg_hdr_stitch_en;
    dest->reg_hdr_mapping_en = src->reg_hdr_mapping_en;
    dest->reg_hdr_ratio_vs = src->reg_hdr_ratio_vs;
    dest->reg_hdr_th1_vs_s = src->reg_hdr_th1_vs_s;
    dest->reg_hdr_th2_vs_s = src->reg_hdr_th2_vs_s;
    //dest->reg_hdr_uvtbl_x0 = src->reg_hdr_uvtbl_x0;
    _HalIsp_RegCopy( (unsigned short *)reg_addr(dest,isp10_reg_hdr_uvtbl_x0),
                     (unsigned short *)reg_addr(src,isp10_reg_hdr_uvtbl_x0),
                     (unsigned short *)reg_addr(src,isp10_reg_hdr_uvtbl_x7)
                   );
    //dest->reg_hdr_uvtbl_y0 = src->reg_hdr_uvtbl_y0;
    _HalIsp_RegCopy( (unsigned short *)reg_addr(dest,isp10_reg_hdr_uvtbl_y0),
                     (unsigned short *)reg_addr(src,isp10_reg_hdr_uvtbl_y0),
                     (unsigned short *)reg_addr(src,isp10_reg_hdr_uvtbl_y8)
                   );
    dest->reg_hdr_vs_ratio_128 = src->reg_hdr_vs_ratio_128;
    dest->reg_hdr_dyn_rat_en = src->reg_hdr_dyn_rat_en;
    //dest->reg_hdr_ratio_gain0 = src->reg_hdr_ratio_gain0;
    _HalIsp_RegCopy( (unsigned short *)reg_addr(dest,isp10_reg_hdr_ratio_gain0),
                     (unsigned short *)reg_addr(src,isp10_reg_hdr_ratio_gain0),
                     (unsigned short *)reg_addr(src,isp10_reg_hdr_ratio_gain15)
                   );
    //dest->reg_hdr_sw_ratio_gain0 = src->reg_hdr_sw_ratio_gain0;
    _HalIsp_RegCopy( (unsigned short *)reg_addr(dest,isp10_reg_hdr_sw_ratio_gain0),
                     (unsigned short *)reg_addr(src,isp10_reg_hdr_sw_ratio_gain0),
                     (unsigned short *)reg_addr(src,isp10_reg_hdr_sw_ratio_gain15)
                   );
    dest->reg_hdr_end_in = src->reg_hdr_end_in;
    //dest->reg_hdr_gamma_x0 = src->reg_hdr_gamma_x0;
    _HalIsp_RegCopy( (unsigned short *)reg_addr(dest,isp10_reg_hdr_gamma_x0),
                     (unsigned short *)reg_addr(src,isp10_reg_hdr_gamma_x0),
                     (unsigned short *)reg_addr(src,isp10_reg_hdr_gamma_x21)
                   );
    //dest->reg_hdr_gamma_y0 = src->reg_hdr_gamma_y0;
    _HalIsp_RegCopy( (unsigned short *)reg_addr(dest,isp10_reg_hdr_gamma_y0),
                     (unsigned short *)reg_addr(src,isp10_reg_hdr_gamma_y0),
                     (unsigned short *)reg_addr(src,isp10_reg_hdr_gamma_y22)
                   );
    dest->reg_hdr_histogram_shft = src->reg_hdr_histogram_shft;

    return 0;
}
/*
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_HDR_16to10_START
	{.bank= 9, .offset= 0, .mask= 0x01}, //DBG_IQ_HDR_16to10_GAIN_EN
	{.bank= 9, .offset= 0, .mask= 0x02}, //DBG_IQ_HDR_16to10_GBL_SMOOTH_EN
	{.bank= 9, .offset= 0, .mask= 0x3C}, //DBG_IQ_HDR_16to10_GAIN_BASE
	{.bank= 9, .offset= 2, .mask= 0xFFFF}, //DBG_IQ_HDR_16to10_GAIN_L
	{.bank= 9, .offset= 4, .mask= 0xFFFF}, //DBG_IQ_HDR_16to10_GAIN_H
	{.bank= 9, .offset= 6, .mask= 0xFFFF}, //DBG_IQ_HDR_16to10_OFFSET_R
	{.bank= 9, .offset= 8, .mask= 0xFFFF}, //DBG_IQ_HDR_16to10_OFFSET_G
	{.bank= 9, .offset= 10, .mask= 0xFFFF}, //DBG_IQ_HDR_16to10_OFFSET_B
	{.bank= 9, .offset= 22, .mask= 0xFFFF}, //DBG_IQ_HDR_16to10_CURVE_LUT
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_HDR_16to10_END
*/

int HalIsp_ApplyHDR16to10(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage)
{
    isp_hal_handle *hal = (isp_hal_handle*)hnd;
    volatile infinity2_reg_isp9 *dest = (infinity2_reg_isp9*) hal->isp9_cfg;
    infinity2_reg_isp9 *src = &pIspShd->isp9_cfg;

    //UartSendTrace("%s\n",__FUNCTION__);

    dest->reg_hdr_16to12_gain_en = src->reg_hdr_16to12_gain_en;
    dest->reg_hdr_16to12_gbl_smooth_en = src->reg_hdr_16to12_gbl_smooth_en;
    dest->reg_hdr_16to12_gain_base = src->reg_hdr_16to12_gain_base;
    dest->reg_hdr_16to12_gain_lw = src->reg_hdr_16to12_gain_lw;
    dest->reg_hdr_16to12_gain_hi = src->reg_hdr_16to12_gain_hi;
    dest->reg_hdr_16to12_offset_r = src->reg_hdr_16to12_offset_r;
    dest->reg_hdr_16to12_offset_g = src->reg_hdr_16to12_offset_g;
    dest->reg_hdr_16to12_offset_b = src->reg_hdr_16to12_offset_b;
    //dest->reg_hdr_16to12_curve_0 = src->reg_hdr_16to12_curve_0;
    _HalIsp_RegCopy( (unsigned short *)reg_addr(dest,isp9_reg_hdr_16to12_curve_0),
                     (unsigned short *)reg_addr(src,isp9_reg_hdr_16to12_curve_0),
                     (unsigned short *)reg_addr(src,isp9_reg_hdr_16to12_curve_20)
                   );

    //-------------------------------------------------------------------//
    dest->reg_hdr_16to12_curve_x_13 = 0x0A;
    dest->reg_hdr_16to12_curve_x_14 = 0x0B;
    dest->reg_hdr_16to12_curve_x_15 = 0x0C;
    dest->reg_hdr_16to12_curve_x_16 = 0x0D;
    dest->reg_hdr_16to12_curve_x_17 = 0x0E;
    dest->reg_hdr_16to12_curve_x_18 = 0x0E;
    dest->reg_hdr_16to12_curve_x_19 = 0x0E;
    //-------------------------------------------------------------------//

    return 0;
}
/*
    {.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_SDC_START
    {.bank= 1, .offset= 180, .mask= 0x02}, //DBG_IQ_SDC_EN
    {.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_SDC_TBL
    {.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_SDC_END
*/

int HalIsp_ApplySDC(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage)
{
    isp_hal_handle *hal = (isp_hal_handle*)hnd;
    volatile infinity2_reg_isp1 *dest = (infinity2_reg_isp1*)hal->isp1_cfg;
    infinity2_reg_isp1 *src = &pIspShd->isp1_cfg;

    //UartSendTrace("%s\n",__FUNCTION__);

    dest->reg_isp_def_pix_debug_en = src->reg_isp_def_pix_debug_en;
    return 0;
}
/*
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_NM_START
	{.bank= 1, .offset= 236, .mask= 0x01}, //DBG_IQ_NM_EN
	{.bank= 1, .offset= 192, .mask= 0x3F}, //DBG_IQ_NM_GAIN_R
	{.bank= 1, .offset= 238, .mask= 0x3F}, //DBG_IQ_NM_GAIN_G
	{.bank= 1, .offset= 192, .mask= 0x3F00}, //DBG_IQ_NM_GAIN_B
	{.bank= 1, .offset= 194, .mask= 0x0F}, //DBG_IQ_NM_MAX_THRD_R
	{.bank= 1, .offset= 240, .mask= 0x0F}, //DBG_IQ_NM_MAX_THRD_G
	{.bank= 1, .offset= 194, .mask= 0xF00}, //DBG_IQ_NM_MAX_THRD_B
	{.bank= 1, .offset= 194, .mask= 0xF0}, //DBG_IQ_NM_MIN_THRD_R
	{.bank= 1, .offset= 240, .mask= 0xF0}, //DBG_IQ_NM_MIN_THRD_G
	{.bank= 1, .offset= 194, .mask= 0xF000}, //DBG_IQ_NM_MIN_THRD_B
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_NM_END
*/

int HalIsp_ApplyNM(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage)
{
    isp_hal_handle *hal = (isp_hal_handle*)hnd;
    volatile infinity2_reg_isp0 *dest0 = (infinity2_reg_isp0*)hal->isp0_cfg;
    volatile infinity2_reg_isp1 *dest = (infinity2_reg_isp1*)hal->isp1_cfg;
    infinity2_reg_isp1 *src = &pIspShd->isp1_cfg;

    //UartSendTrace("%s\n",__FUNCTION__);

    dest->reg_snr_nm_filter_en = src->reg_snr_nm_filter_en;
    dest->reg_snr_nm_gain_r_pix = src->reg_snr_nm_gain_r_pix;
    dest->reg_snr_nm_gain = src->reg_snr_nm_gain;
    dest->reg_snr_nm_gain_b_pix = src->reg_snr_nm_gain_b_pix;
    dest->reg_snr_nm_max_thrd_r_pix = src->reg_snr_nm_max_thrd_r_pix;
    dest->reg_snr_nm_max_thrd = src->reg_snr_nm_max_thrd;
    dest->reg_snr_nm_max_thrd_b_pix = src->reg_snr_nm_max_thrd_b_pix;
    dest->reg_snr_nm_min_thrd_r_pix = src->reg_snr_nm_min_thrd_r_pix;
    dest->reg_snr_nm_min_thrd = src->reg_snr_nm_min_thrd;
    dest->reg_snr_nm_min_thrd_b_pix = src->reg_snr_nm_min_thrd_b_pix;

    //-------------------------------------------------------------------//
    if (src->reg_snr_nm_filter_en == true)
        dest0->reg_en_isp_denoise = true;
    //-------------------------------------------------------------------//

    return 0;
}
/*
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_PostDN_START
	{.bank= 0, .offset= 224, .mask= 0x08}, //DBG_IQ_PostDN_EN
	{.bank= 0, .offset= 224, .mask= 0x7000}, //DBG_IQ_PostDN_G_RATIO
	{.bank= 0, .offset= 224, .mask= 0x700}, //DBG_IQ_PostDN_RB_RATIO
	{.bank= 0, .offset= 224, .mask= 0x04}, //DBG_IQ_PostDN_GAVG_REF_EN
	{.bank= 255, .offset= 0, .mask= 0x00}, //DBG_IQ_PostDN_END
*/

int HalIsp_ApplyPostDN(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage)
{
    isp_hal_handle *hal = (isp_hal_handle*)hnd;
    volatile infinity2_reg_isp0 *dest = (infinity2_reg_isp0*)hal->isp0_cfg;
    infinity2_reg_isp0 *src = &pIspShd->isp0_cfg;

    //UartSendTrace("%s\n",__FUNCTION__);

    dest->reg_cfai_dnr_en = src->reg_cfai_dnr_en;
    dest->reg_cfai_dnr_g_ratio = src->reg_cfai_dnr_g_ratio;
    dest->reg_cfai_dnr_rb_ratio = src->reg_cfai_dnr_rb_ratio;
    dest->reg_cfai_dnr_gavg_ref_en = src->reg_cfai_dnr_gavg_ref_en;

    //-------------------------------------------------------------------//
    if (src->reg_cfai_dnr_en == true)
        dest->reg_en_isp_denoise = true;
    //-------------------------------------------------------------------//

    return 0;
}
int HalISP_ApplyShadow(ISP_HAL_HANDLE hnd,HalIspRegShadow_t *pIspShd,IspShdStage_e eStage)
{
    isp_hal_handle *hal = (isp_hal_handle*)hnd;

    if(eStage == eIspShdInit)
    {
        //ISP_IQ_WBG_Cfg(hal,&pIspShd->wbg);
        HalIsp_ApplyOBC(hnd,pIspShd,eStage);
        HalIsp_ApplyDPC(hnd,pIspShd,eStage);
        HalIsp_ApplySpikeNR(hnd,pIspShd,eStage);
        HalIsp_ApplyBSNR(hnd,pIspShd,eStage);
        HalIsp_ApplyDM(hnd,pIspShd,eStage);
        HalIsp_GammaA2A(hnd,pIspShd,eStage);
        HalIsp_ApplyGAMA16to16ISP(hnd,pIspShd,eStage);
    }

    if(eStage == eIspShdRuntime)
    {
        ISP_IQ_WBG_Cfg(hal,&pIspShd->wbg);
        HalIsp_ApplyDECOMP(hnd,pIspShd,eStage);
        HalIsp_ApplyFPN(hnd,pIspShd,eStage);
        HalIsp_ApplyFPNP1(hnd,pIspShd,eStage);
        HalIsp_ApplyOBC(hnd,pIspShd,eStage);
        HalIsp_ApplyOBCP1(hnd,pIspShd,eStage);
        HalIsp_ApplyGAMA16to16ISP(hnd,pIspShd,eStage);
        HalIsp_ApplyGAMAC2C_P1(hnd,pIspShd,eStage);
        HalIsp_ApplyLSC(hnd,pIspShd,eStage);
        HalIsp_ApplyLSCP1(hnd,pIspShd,eStage);
        HalIsp_ApplyALSC(hnd,pIspShd,eStage);
        HalIsp_ApplyALSCP1(hnd,pIspShd,eStage);
        HalIsp_ApplyHDR(hnd,pIspShd,eStage);
        HalIsp_ApplyHDR16to10(hnd,pIspShd,eStage);
        HalIsp_ApplySDC(hnd,pIspShd,eStage);
        HalIsp_ApplyDPC(hnd,pIspShd,eStage);
        HalIsp_ApplyGE(hnd,pIspShd,eStage);
        HalIsp_ApplyANTICT(hnd,pIspShd,eStage);
        HalIsp_ApplyRGBIR(hnd,pIspShd,eStage);
        HalIsp_ApplySpikeNR(hnd,pIspShd,eStage);
        HalIsp_ApplyBSNR(hnd,pIspShd,eStage);
        HalIsp_ApplyFalseColor(hnd,pIspShd,eStage);
        HalIsp_ApplyNM(hnd,pIspShd,eStage);
        HalIsp_ApplyDM(hnd,pIspShd,eStage);
        HalIsp_ApplyPostDN(hnd,pIspShd,eStage);
        HalIsp_GammaA2A(hnd,pIspShd,eStage);
    }

    return 0;
}
