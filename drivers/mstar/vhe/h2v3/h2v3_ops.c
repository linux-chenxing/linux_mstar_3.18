
#include <mdrv_mvhe_st.h>

#include <mhve_ops.h>
#include <mhve_ios.h>

#include <rqct_ops.h>
#include <h2v3_def.h>
#include <h265_enc.h>
#include <h2v3_rqc.h>
#include <h2v3_reg.h>
#include <h2v3_ios.h>
#include <h2v3_ops.h>

static int   _seq_sync(mhve_ops*);
static int   _seq_conf(mhve_ops*);
static int   _seq_done(mhve_ops*);
static int   _enc_buff(mhve_ops*,mhve_vpb*);
static int   _deq_buff(mhve_ops*,mhve_vpb*);
static int   _enc_conf(mhve_ops*);
static int   _put_data(mhve_ops*,void*,int);
static int   _enc_done(mhve_ops*);
static int   _out_buff(mhve_ops*,mhve_cpb*);
static int   _set_conf(mhve_ops*,mhve_cfg*);
static int   _get_conf(mhve_ops*,mhve_cfg*);
static void* _rqct_ops(mhve_ops* mops)
{
    h2v3_ops* h2v3 = (h2v3_ops*)mops;
    return h2v3->rqct;
}
static void* _mvhe_job(mhve_ops* mops)
{
    h2v3_ops* h2v3 = (h2v3_ops*)mops;
    return h2v3->mirr;
}
static void __ops_free(mhve_ops* mops)
{
    h2v3_ops* h2v3 = (h2v3_ops*)mops;
    rqct_ops* rqct = h2v3->rqct;
    h265_enc* h265 = h2v3->h265;
    h2v3_mir* mirr = h2v3->mirr;
    if (rqct)
        rqct->release(rqct);
    if (h265)
        h265->release(h265);
    if (mirr)
        MEM_FREE(mirr);
    MEM_FREE(h2v3);
}

void* mvheops_acquire(int idx)
{
    mhve_ops* mops;
    while (NULL != (mops = MEM_ALLC(sizeof(h2v3_ops))))
    {
        h2v3_ops* h2v3;
        mops->release = __ops_free;
        mops->rqct_ops = _rqct_ops;
        mops->mhve_job = _mvhe_job;
        mops->seq_sync = _seq_sync;
        mops->seq_done = _seq_done;
        mops->seq_conf = _seq_conf;
        mops->enc_buff = _enc_buff;
        mops->deq_buff = _deq_buff;
        mops->enc_conf = _enc_conf;
        mops->put_data = _put_data;
        mops->enc_done = _enc_done;
        mops->out_buff = _out_buff;
        mops->set_conf = _set_conf;
        mops->get_conf = _get_conf;
        MEM_COPY(mops->mhvename,"h2v3@0.0-00",12);
        h2v3 = (h2v3_ops*)mops;
        /* default setting of h265 */
        h2v3->rqct = rqctvhe_acquire(idx);
        h2v3->h265 = h265enc_acquire();
        h2v3->mirr = MEM_ALLC(sizeof(h2v3_mir));
        if (!h2v3->rqct || !h2v3->h265 || !h2v3->mirr)
        {
            mops->release(mops);
            break;
        }
        /* default setting */
        h2v3->i_rpbn = 2;
        return mops;
    }
    return NULL;
}

static int _seq_sync(mhve_ops* mops)
{
    h2v3_ops* h2v3 = (h2v3_ops*)mops;
    rqct_ops* rqct = h2v3->rqct;
    h265_enc* h265 = h2v3->h265;
    int err = 0;
    if (0 != (err = h265_seq_init(h265, h2v3->i_rpbn)))
    {
        mops->seq_done(mops);
        return err;
    }
    h2v3->i_seqn = 0;
    h2v3->b_seqh = 1;
    rqct->seq_sync(rqct);
    return err;
}

static int _seq_conf(mhve_ops* mops)
{
    h2v3_ops* h2v3 = (h2v3_ops*)mops;
    h2v3_mir* mirr = h2v3->mirr;
    h265_enc* h265 = h2v3->h265;
    uint* regs = mirr->regs;
    H2PutRegVal(regs, HWIF_ENC_AXI_WR_ID_E,         0);
    H2PutRegVal(regs, HWIF_ENC_AXI_RD_ID_E,         0);
    H2PutRegVal(regs, HWIF_ENC_IRQ_DIS,             ENCH2_IRQ_DISABLE);
    H2PutRegVal(regs, HWIF_ENC_INPUT_READ_CHUNK,    ENCH2_INPUT_READ_CHUNK);
    H2PutRegVal(regs, HWIF_ENC_AXI_READ_ID,         ENCH2_AXI_RD_ID);
    H2PutRegVal(regs, HWIF_ENC_AXI_WRITE_ID,        ENCH2_AXI_WR_ID);
    H2PutRegVal(regs, HWIF_ENC_BURST_DISABLE,       ENCH2_BURST_SCMD_DISABLE&(1));
    H2PutRegVal(regs, HWIF_ENC_BURST_INCR,          ENCH2_BURST_INCR_TYPE_ENABLED&(1));
    H2PutRegVal(regs, HWIF_ENC_DATA_DISCARD,        ENCH2_BURST_DATA_DISCARD_ENABLED&(1));
    H2PutRegVal(regs, HWIF_ENC_CLOCK_GATING,        ENCH2_ASIC_CLOCK_GATING_ENABLED&(1));
    H2PutRegVal(regs, HWIF_ENC_AXI_DUAL_CH,         ENCH2_AXI_2CH_DISABLE);
    H2PutRegVal(regs, HWIF_ENC_STRM_SWAP,           ENCH2_OUT_SWAP);
    H2PutRegVal(regs, HWIF_ENC_PIC_SWAP,            ENCH2_PIC_SWAP_YUV);
    H2PutRegVal(regs, HWIF_ENC_ROI_DQ_SWAP,         ENCH2_DQM_SWAP);
    H2PutRegVal(regs, HWIF_ENC_CTB_RC_SWAP,         ENCH2_RCM_SWAP);
    H2PutRegVal(regs, HWIF_ENC_SCALEDOUT_SWAP,      ENCH2_SCA_SWAP);
    H2PutRegVal(regs, HWIF_ENC_MODE,                ENCH2_CODEC_HEVC);
    /* bias */
    H2PutRegVal(regs, HWIF_ENC_BITS_EST_BIAS_INTRA_CU_8,       22);    // 25
    H2PutRegVal(regs, HWIF_ENC_BITS_EST_BIAS_INTRA_CU_16,      40);    // 48
    H2PutRegVal(regs, HWIF_ENC_BITS_EST_BIAS_INTRA_CU_32,      86);    // 108
    H2PutRegVal(regs, HWIF_ENC_BITS_EST_BIAS_INTRA_CU_64,      38*8);  // 48*8
    H2PutRegVal(regs, HWIF_ENC_INTER_SKIP_BIAS,                124);
    /* penalties */
    if (h265->i_picw < 832 && h265->i_pich < 480)
    {
        H2PutRegVal(regs, HWIF_ENC_BITS_EST_1N_CU_PENALTY,     0);
        H2PutRegVal(regs, HWIF_ENC_BITS_EST_TU_SPLIT_PENALTY,  2);
    }
    else
    {
        H2PutRegVal(regs, HWIF_ENC_BITS_EST_1N_CU_PENALTY,     5);
        H2PutRegVal(regs, HWIF_ENC_BITS_EST_TU_SPLIT_PENALTY,  3);
    }
    H2PutRegVal(regs, HWIF_ENC_TIMEOUT_INT,        ENCH2_TIMEOUT_INTERRUPT&(1));
    H2PutRegVal(regs, HWIF_ENC_MAX_BURST,          ENCH2_AXI40_BURST_LENGTH);
    H2PutRegVal(regs, HWIF_TIMEOUT_OVERRIDE_E,     ENCH2_ASIC_TIMEOUT_OVERRIDE_ENABLE);
    H2PutRegVal(regs, HWIF_TIMEOUT_CYCLES,         ENCH2_ASIC_TIMEOUT_CYCLES);
    H2PutRegVal(regs, HWIF_ENC_INTRA_AREA_LEFT,    0xFF);
    H2PutRegVal(regs, HWIF_ENC_INTRA_AREA_RIGHT,   0xFF);
    H2PutRegVal(regs, HWIF_ENC_INTRA_AREA_TOP,     0xFF);
    H2PutRegVal(regs, HWIF_ENC_INTRA_AREA_BOTTOM,  0xFF);
    H2PutRegVal(regs, HWIF_ENC_CIR_START,          0);
    H2PutRegVal(regs, HWIF_ENC_CIR_INTERVAL,       0);
#define INTRA_SIZE_FACTOR0    506
#define INTRA_SIZE_FACTOR1    506
#define INTRA_SIZE_FACTOR2    709
#define INTRA_SIZE_FACTOR3    709
#define INTRA_MODE_FACTOR0     24
#define INTRA_MODE_FACTOR1     37
#define INTRA_MODE_FACTOR2     78
    H2PutRegVal(regs, HWIF_ENC_INTRA_SIZE_FACTOR_0, INTRA_SIZE_FACTOR0);
    H2PutRegVal(regs, HWIF_ENC_INTRA_SIZE_FACTOR_1, INTRA_SIZE_FACTOR1);
    H2PutRegVal(regs, HWIF_ENC_INTRA_SIZE_FACTOR_2, INTRA_SIZE_FACTOR2);
    H2PutRegVal(regs, HWIF_ENC_INTRA_SIZE_FACTOR_3, INTRA_SIZE_FACTOR3);
    H2PutRegVal(regs, HWIF_ENC_INTRA_MODE_FACTOR_0, INTRA_MODE_FACTOR0);
    H2PutRegVal(regs, HWIF_ENC_INTRA_MODE_FACTOR_1, INTRA_MODE_FACTOR1);
    H2PutRegVal(regs, HWIF_ENC_INTRA_MODE_FACTOR_2, INTRA_MODE_FACTOR2);
    return 0;
}

static int _seq_done(mhve_ops* mops)
{
    h2v3_ops* h2v3 = (h2v3_ops*)mops;
    h265_enc* h265 = h2v3->h265;
    rqct_ops* rqct = h2v3->rqct;
    rqct->seq_done(rqct);
    h265_seq_done(h265);
    return 0;
}

static int _enc_buff(mhve_ops* mops, mhve_vpb* mvpb)
{
    h2v3_ops* h2v3 = (h2v3_ops*)mops;
    rqct_ops* rqct = h2v3->rqct;
    h265_enc* h265 = h2v3->h265;
    mhve_job* mjob = (mhve_job*)h2v3->mirr;
    rqct_buf  rqcb;
    int type, rpsi = 0;
    int err = 0, idx;
    h2v3->m_encp = *mvpb;
    mvpb = &h2v3->m_encp;
    if (h2v3->b_seqh)
    {
        h265_seq_conf(h265);
        rqct->seq_sync(rqct);
    }
    rqcb.u_config = 0;
    if (mvpb->u_flags&MVHE_FLAGS_IDR)
        rqct->seq_sync(rqct);
    if (mvpb->u_flags&MVHE_FLAGS_DISPOSABLE)
        rqct->b_unrefp = 1;
    rqct->enc_buff(rqct,&rqcb);
    rqct->enc_conf(rqct, mjob);
    if (IS_IPIC(rqct->i_pictyp))
    {
        type = HEVC_ISLICE;
        mvpb->u_flags = MVHE_FLAGS_IDR;
        h2v3->b_seqh = 1;
    }
    else
    {
        type = HEVC_PSLICE;
        if (mvpb->u_flags&MVHE_FLAGS_DISPOSABLE)
            rpsi = !rqct->b_unrefp;
    }
    h265->i_picq = rqct->i_enc_qp;
    if (rqct->b_unrefp == 0)
        mvpb->u_flags &=~MVHE_FLAGS_DISPOSABLE;
    if (0 <= (idx = h265_enc_buff(h265, type, !rqct->b_unrefp)))
    {
        rpb_t* rpb = h2v3->m_rpbs + idx;
        rpb->i_index = mvpb->i_index;
        if (!rpb->b_valid)
        {
            rpb->u_planes[RPB_YPIXEL] = mvpb->planes[0].u_phys;
            rpb->u_planes[RPB_CPIXEL] = mvpb->planes[1].u_phys;
            mvpb->i_index = -1;
        }
    }
    return err;
}

static int _deq_buff(mhve_ops* mops, mhve_vpb* mvpb)
{
    h2v3_ops* h2v3 = (h2v3_ops*)mops;
    *mvpb = h2v3->m_encp;
    return 0;
}

static int _enc_conf(mhve_ops* mops)
{
    h2v3_ops* h2v3 = (h2v3_ops*)mops;
    h265_enc* h265 = h2v3->h265;
    mhve_vpb* mvpb =&h2v3->m_encp;
    h2v3_mir* mirr = h2v3->mirr;
    pps_t *pps = h265_find_set(h265, HEVC_PPS, 0);
    sps_t *sps = pps->p_sps;
    pic_t *ref, *rec = h265->p_recn;
    rpb_t *rpb;
    uint *regs = mirr->regs;
    H2PutRegVal(regs, HWIF_ENC_SLICE_SIZE,                  h265->i_rows);
    H2PutRegVal(regs, HWIF_ENC_OUTPUT_STRM_BASE,            h2v3->u_otbs+h2v3->u_used);
    H2PutRegVal(regs, HWIF_ENC_OUTPUT_STRM_BUFFER_LIMIT,    h2v3->u_otsz-h2v3->u_used);
    H2PutRegVal(regs, HWIF_ENC_FRAME_CODING_TYPE,           h265->i_type==HEVC_PSLICE?0:1);
    H2PutRegVal(regs, HWIF_ENC_STRONG_INTRA_SMOOTHING_ENABLED_FLAG, sps->b_strong_intra_smoothing_enabled);
    H2PutRegVal(regs, HWIF_ENC_CONSTRAINED_INTRA_PRED_FLAG, pps->b_constrained_intra_pred);
    H2PutRegVal(regs, HWIF_ENC_POC, h265->i_poc);
    H2PutRegVal(regs, HWIF_ENC_OUTPUT_STRM_MODE,            0); // 0:stream, 1:nalu
    /* curr picture */
    H2PutRegVal(regs, HWIF_ENC_INPUT_Y_BASE,                mvpb->planes[0].u_phys+mvpb->planes[0].i_bias);
    H2PutRegVal(regs, HWIF_ENC_INPUT_CB_BASE,               mvpb->planes[1].u_phys+mvpb->planes[1].i_bias);
    H2PutRegVal(regs, HWIF_ENC_CHROMA_SWAP,                 0);
    switch (h2v3->e_pixf)
    {
    case MHVE_PIX_NV21:
        H2PutRegVal(regs, HWIF_ENC_CHROMA_SWAP,             1);
    case MHVE_PIX_NV12:
        H2PutRegVal(regs, HWIF_ENC_INPUT_FORMAT,            1);
        break;
    case MHVE_PIX_YVYU:
        H2PutRegVal(regs, HWIF_ENC_CHROMA_SWAP,             1);
    case MHVE_PIX_YUYV:
        H2PutRegVal(regs, HWIF_ENC_INPUT_FORMAT,            2);
    default:
        break;
    }
    H2PutRegVal(regs, HWIF_ENC_MIN_CB_SIZE,                 sps->i_log2_min_cb_size-3);
    H2PutRegVal(regs, HWIF_ENC_MAX_CB_SIZE,                 sps->i_log2_max_cb_size-3);
    H2PutRegVal(regs, HWIF_ENC_MIN_TRB_SIZE,                sps->i_log2_min_tr_size-2);
    H2PutRegVal(regs, HWIF_ENC_MAX_TRB_SIZE,                sps->i_log2_max_tr_size-2);
    H2PutRegVal(regs, HWIF_ENC_PIC_WIDTH,                   sps->i_pic_w_min_cbs / 8);
    H2PutRegVal(regs, HWIF_ENC_PIC_HEIGHT,                  sps->i_pic_h_min_cbs / 8);
    H2PutRegVal(regs, HWIF_ENC_PPS_DEBLOCKING_FILTER_OVERRIDE_ENABLED_FLAG, pps->b_deblocking_filter_override_enabled);
    H2PutRegVal(regs, HWIF_ENC_SLICE_DEBLOCKING_FILTER_OVERRIDE_FLAG,       h265->b_deblocking_override);
    H2PutRegVal(regs, HWIF_ENC_PIC_INIT_QP,                 pps->i_init_qp);
    H2PutRegVal(regs, HWIF_ENC_PIC_QP,                      h265->i_picq);
    H2PutRegVal(regs, HWIF_ENC_CU_QP_DELTA_ENABLED,         pps->b_cu_qp_delta_enabled);
    H2PutRegVal(regs, HWIF_ENC_DIFF_CU_QP_DELTA_DEPTH,      pps->i_diff_cu_qp_delta_depth);
    H2PutRegVal(regs, HWIF_ENC_CHROMA_QP_OFFSET,            pps->i_cb_qp_offset&0x1F);
    H2PutRegVal(regs, HWIF_ENC_SAO_ENABLE,                  sps->b_sao_enabled);
    H2PutRegVal(regs, HWIF_ENC_MAX_TRANS_HIERARCHY_DEPTH_INTER, sps->i_max_tr_hierarchy_depth_inter);//val->maxTransHierarchyDepthInter);
    H2PutRegVal(regs, HWIF_ENC_MAX_TRANS_HIERARCHY_DEPTH_INTRA, sps->i_max_tr_hierarchy_depth_intra);//val->maxTransHierarchyDepthIntra);
    /* recn picture */
    rpb =&h2v3->m_rpbs[rec->i_id];
    H2PutRegVal(regs, HWIF_ENC_RECON_Y_BASE,                    rpb->u_planes[RPB_YPIXEL]);
    H2PutRegVal(regs, HWIF_ENC_RECON_CHROMA_BASE,               rpb->u_planes[RPB_CPIXEL]);
    H2PutRegVal(regs, HWIF_ENC_RECON_LUMA_4N_BASE,              rpb->u_planes[RPB_LUMA4N]);
    H2PutRegVal(regs, HWIF_ENC_RECON_LUMA_COMPRESSOR_ENABLE,    rpb->u_planes[RPB_YCOMPR]!=0);
    H2PutRegVal(regs, HWIF_ENC_RECON_CHROMA_COMPRESSOR_ENABLE,  rpb->u_planes[RPB_CCOMPR]!=0);
    H2PutRegVal(regs, HWIF_ENC_RECON_LUMA_COMPRESS_TABLE_BASE,  rpb->u_planes[RPB_YCOMPR]);
    H2PutRegVal(regs, HWIF_ENC_RECON_CHROMA_COMPRESS_TABLE_BASE,rpb->u_planes[RPB_CCOMPR]);
    H2PutRegVal(regs, HWIF_ENC_NUM_SHORT_TERM_REF_PIC_SETS_V2,  sps->i_num_short_term_ref_pic_sets);
    if (rec->p_rps)
    {
        rps_t* rps = rec->p_rps;
        H2PutRegVal(regs, HWIF_ENC_RPS_ID_V2,               rps->i_id);
        H2PutRegVal(regs, HWIF_ENC_NUM_NEGATIVE_PICS,       rps->i_num_neg_pics);
        H2PutRegVal(regs, HWIF_ENC_NUM_POSITIVE_PICS,       rps->i_num_pos_pics);
        H2PutRegVal(regs, HWIF_ENC_L0_DELTA_POC0,           rps->i_dpoc[0]&0x3FF);
        H2PutRegVal(regs, HWIF_ENC_L0_USED_BY_CURR_PIC0,    rps->b_used[0]);
        if (rps->i_num_neg_pics > 1)
        {
            H2PutRegVal(regs, HWIF_ENC_L0_DELTA_POC1,       rps->i_dpoc[1]&0x3FF);
            H2PutRegVal(regs, HWIF_ENC_L0_USED_BY_CURR_PIC1,rps->b_used[1]);
        }
    }
    /* reference-list0 */
    if (h265->i_active_l0_count > 0)
    {
        ref = h265_find_rpl(h265,0,0);
        rpb =&h2v3->m_rpbs[ref->i_id];
        H2PutRegVal(regs, HWIF_ENC_REFPIC_RECON_L0_Y0,                  rpb->u_planes[RPB_YPIXEL]);
        H2PutRegVal(regs, HWIF_ENC_REFPIC_RECON_L0_CHROMA0,             rpb->u_planes[RPB_CPIXEL]);
        H2PutRegVal(regs, HWIF_ENC_REFPIC_RECON_L0_4N0_BASE,            rpb->u_planes[RPB_LUMA4N]);
        H2PutRegVal(regs, HWIF_ENC_L0_REF0_LUMA_COMPRESSOR_ENABLE,      rpb->u_planes[RPB_YCOMPR]!=0);
        H2PutRegVal(regs, HWIF_ENC_L0_REF0_CHROMA_COMPRESSOR_ENABLE,    rpb->u_planes[RPB_CCOMPR]!=0);
        H2PutRegVal(regs, HWIF_ENC_L0_REF0_LUMA_COMPRESS_TABLE_BASE,    rpb->u_planes[RPB_YCOMPR]);
        H2PutRegVal(regs, HWIF_ENC_L0_REF0_CHROMA_COMPRESS_TABLE_BASE,  rpb->u_planes[RPB_CCOMPR]);
    }
    /* reference-list0 */
    if (h265->i_active_l0_count > 1)
    {
        ref = h265_find_rpl(h265,0,1);
        rpb =&h2v3->m_rpbs[ref->i_id];
        H2PutRegVal(regs, HWIF_ENC_REFPIC_RECON_L0_Y1,                  rpb->u_planes[RPB_YPIXEL]);
        H2PutRegVal(regs, HWIF_ENC_REFPIC_RECON_L0_CHROMA1,             rpb->u_planes[RPB_CPIXEL]);
        H2PutRegVal(regs, HWIF_ENC_REFPIC_RECON_L0_4N1_BASE,            rpb->u_planes[RPB_LUMA4N]);
        H2PutRegVal(regs, HWIF_ENC_L0_REF1_LUMA_COMPRESSOR_ENABLE,      rpb->u_planes[RPB_YCOMPR]!=0);
        H2PutRegVal(regs, HWIF_ENC_L0_REF1_CHROMA_COMPRESSOR_ENABLE,    rpb->u_planes[RPB_CCOMPR]!=0);
        H2PutRegVal(regs, HWIF_ENC_L0_REF1_LUMA_COMPRESS_TABLE_BASE,    rpb->u_planes[RPB_YCOMPR]);
        H2PutRegVal(regs, HWIF_ENC_L0_REF1_CHROMA_COMPRESS_TABLE_BASE,  rpb->u_planes[RPB_CCOMPR]);
    }
    H2PutRegVal(regs, HWIF_ENC_ACTIVE_L0_CNT,               h265->i_active_l0_count);
    H2PutRegVal(regs, HWIF_ENC_ACTIVE_L1_CNT,               0); // NOT supporrt B-Frame.
    H2PutRegVal(regs, HWIF_ENC_ACTIVE_OVERRIDE_FLAG,        h265->b_active_override);
    H2PutRegVal(regs, HWIF_ENC_SCALING_LIST_ENABLED_FLAG,   sps->b_scaling_list_enable);
    H2PutRegVal(regs, HWIF_ENC_RCROI_ENABLE,                0); // FIXME: force disable rc&roi
#if 0
    H2PutRegVal(regs, HWIF_ENC_ROIMAPDELTAQPADDR,       val->roiMapDeltaQpAddr);
    H2PutRegVal(regs, HWIF_ENC_INTRA_AREA_LEFT,         val->intraAreaLeft);
    H2PutRegVal(regs, HWIF_ENC_INTRA_AREA_RIGHT,        val->intraAreaRight);
    H2PutRegVal(regs, HWIF_ENC_INTRA_AREA_TOP,          val->intraAreaTop);
    H2PutRegVal(regs, HWIF_ENC_INTRA_AREA_BOTTOM,       val->intraAreaBottom);
    H2PutRegVal(regs, HWIF_ENC_ROI1_LEFT,               val->roi1Left);
    H2PutRegVal(regs, HWIF_ENC_ROI1_RIGHT,              val->roi1Right);
    H2PutRegVal(regs, HWIF_ENC_ROI1_TOP,                val->roi1Top);
    H2PutRegVal(regs, HWIF_ENC_ROI1_BOTTOM,             val->roi1Bottom);
    H2PutRegVal(regs, HWIF_ENC_ROI2_LEFT,               val->roi2Left);
    H2PutRegVal(regs, HWIF_ENC_ROI2_RIGHT,              val->roi2Right);
    H2PutRegVal(regs, HWIF_ENC_ROI2_TOP,                val->roi2Top);
    H2PutRegVal(regs, HWIF_ENC_ROI2_BOTTOM,             val->roi2Bottom);

    H2PutRegVal(regs, HWIF_ENC_ROI1_DELTA_QP_RC,        val->roi1DeltaQp);
    H2PutRegVal(regs, HWIF_ENC_ROI2_DELTA_QP_RC,        val->roi2DeltaQp);
    H2PutRegVal(regs, HWIF_ENC_QP_MIN,                  val->qpMin);
    H2PutRegVal(regs, HWIF_ENC_QP_MAX,                  val->qpMax);
    H2PutRegVal(regs, HWIF_ENC_RC_QPDELTA_RANGE,        val->rcQpDeltaRange);
#endif
    H2PutRegVal(regs, HWIF_ENC_DEBLOCKING_FILTER_CTRL,  h265->b_deblocking_disable);
    H2PutRegVal(regs, HWIF_ENC_DEBLOCKING_TC_OFFSET,   (h265->i_tc_offset  /2)&0xF);
    H2PutRegVal(regs, HWIF_ENC_DEBLOCKING_BETA_OFFSET, (h265->i_beta_offset/2)&0xF);
    H2PutRegVal(regs, HWIF_ENC_SIZE_TBL_BASE,           h2v3->u_nalb);
    H2PutRegVal(regs, HWIF_ENC_NAL_SIZE_WRITE,          h2v3->u_nalb!=0);
    H2PutRegVal(regs, HWIF_ENC_CHROFFSET,               0);
    H2PutRegVal(regs, HWIF_ENC_LUMOFFSET,               0);
    H2PutRegVal(regs, HWIF_ENC_ROWLENGTH,               h2v3->m_encp.i_pitch);
    H2PutRegVal(regs, HWIF_ENC_COMPRESSEDCOEFF_BASE,    h2v3->u_coef);
    H2PutRegVal(regs, HWIF_ENC_CABAC_INIT_FLAG,         h265->b_cabac_init);
    H2PutRegVal(regs, HWIF_ENC_SLICE_INT,               0); /*rejoect slice-done interrupt */
    H2PutRegVal(regs, HWIF_ENC_OUTPUT_BITWIDTH_CHROMA,  0);
    H2PutRegVal(regs, HWIF_ENC_OUTPUT_BITWIDTH_LUM,     0);
#if 0
    H2PutRegVal(regs, HWIF_ENC_INPUT_ROTATION,          val->inputImageRotation);
    H2PutRegVal(regs, HWIF_ENC_RGBCOEFFA,               val->colorConversionCoeffA);
    H2PutRegVal(regs, HWIF_ENC_RGBCOEFFB,               val->colorConversionCoeffB);
    H2PutRegVal(regs, HWIF_ENC_RGBCOEFFC,               val->colorConversionCoeffC);
    H2PutRegVal(regs, HWIF_ENC_RGBCOEFFE,               val->colorConversionCoeffE);
    H2PutRegVal(regs, HWIF_ENC_RGBCOEFFF,               val->colorConversionCoeffF);
    H2PutRegVal(regs, HWIF_ENC_RMASKMSB,                val->rMaskMsb);
    H2PutRegVal(regs, HWIF_ENC_GMASKMSB,                val->gMaskMsb);
    H2PutRegVal(regs, HWIF_ENC_BMASKMSB,                val->bMaskMsb);
    H2PutRegVal(regs, HWIF_ENC_XFILL,                   val->xFill);
    H2PutRegVal(regs, HWIF_ENC_YFILL,                   val->yFill);
    H2PutRegVal(regs, HWIF_ENC_SCALE_MODE,              val->scaledWidth > 0 ? 2 : 0);
    H2PutRegVal(regs, HWIF_ENC_BASESCALEDOUTLUM,        val->scaledLumBase);
    H2PutRegVal(regs, HWIF_ENC_SCALEDOUTWIDTH,          val->scaledWidth);
    H2PutRegVal(regs, HWIF_ENC_SCALEDOUTWIDTHRATIO,     val->scaledWidthRatio);
    H2PutRegVal(regs, HWIF_ENC_SCALEDOUTHEIGHT,         val->scaledHeight);
    H2PutRegVal(regs, HWIF_ENC_SCALEDOUTHEIGHTRATIO,    val->scaledHeightRatio);
    H2PutRegVal(regs, HWIF_ENC_SCALEDSKIPLEFTPIXELCOLUMN,   val->scaledSkipLeftPixelColumn);
    H2PutRegVal(regs, HWIF_ENC_SCALEDSKIPTOPPIXELROW,   val->scaledSkipTopPixelRow);
    H2PutRegVal(regs, HWIF_ENC_VSCALE_WEIGHT_EN,        val->scaledVertivalWeightEn);
    H2PutRegVal(regs, HWIF_ENC_SCALEDHORIZONTALCOPY,    val->scaledHorizontalCopy);
    H2PutRegVal(regs, HWIF_ENC_SCALEDVERTICALCOPY,      val->scaledVerticalCopy);
#endif
#if 0
    /* rate-controller */
    H2PutRegVal(regs, HWIF_ENC_TARGETPICSIZE,           val->targetPicSize);
    H2PutRegVal(regs, HWIF_ENC_MINPICSIZE,              val->minPicSize);
    H2PutRegVal(regs, HWIF_ENC_MAXPICSIZE,              val->maxPicSize);
    H2PutRegVal(regs, HWIF_ENC_CTBRCBITMEMADDR_CURRENT, val->ctbRcBitMemAddrCur);
    H2PutRegVal(regs, HWIF_ENC_CTBRCBITMEMADDR_PREVIOUS,val->ctbRcBitMemAddrPre);
    H2PutRegVal(regs, HWIF_ENC_CTBRCTHRDMIN,            val->ctbRcThrdMin);
    H2PutRegVal(regs, HWIF_ENC_CTBRCTHRDMAX,            val->ctbRcThrdMax);
    H2PutRegVal(regs, HWIF_ENC_CTBBITSMIN,              val->ctbBitsMin);
    H2PutRegVal(regs, HWIF_ENC_CTBBITSMAX,              val->ctbBitsMax);
    H2PutRegVal(regs, HWIF_ENC_TOTALLCUBITS,            val->totalLcuBits);
    H2PutRegVal(regs, HWIF_ENC_BITSRATIO,               val->bitsRatio);
#endif
#if 0
    /* reference pic lists modification */
    H2PutRegVal(regs, HWIF_ENC_LISTS_MODI_PRESENT_FLAG,     val->lists_modification_present_flag);
    H2PutRegVal(regs, HWIF_ENC_REF_PIC_LIST_MODI_FLAG_L0,   val->ref_pic_list_modification_flag_l0);
    H2PutRegVal(regs, HWIF_ENC_LIST_ENTRY_L0_PIC0,          val->list_entry_l0[0]);
    H2PutRegVal(regs, HWIF_ENC_LIST_ENTRY_L0_PIC1,          val->list_entry_l0[1]);
    H2PutRegVal(regs, HWIF_ENC_REF_PIC_LIST_MODI_FLAG_L1,   val->ref_pic_list_modification_flag_l1);
    H2PutRegVal(regs, HWIF_ENC_LIST_ENTRY_L1_PIC0,          val->list_entry_l1[0]);
    H2PutRegVal(regs, HWIF_ENC_LIST_ENTRY_L1_PIC1,          val->list_entry_l1[1]);
#endif
#if 0
    /* cost function related */
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SAO_LUMA,      val->lamda_SAO_luma);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SAO_CHROMA,    val->lamda_SAO_chroma);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_MOTION_SSE,    val->lamda_motion_sse);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SATD_ME_0,     val->lambda_satd_me[0]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SATD_ME_1,     val->lambda_satd_me[1]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SATD_ME_2,     val->lambda_satd_me[2]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SATD_ME_3,     val->lambda_satd_me[3]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SATD_ME_4,     val->lambda_satd_me[4]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SATD_ME_5,     val->lambda_satd_me[5]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SATD_ME_6,     val->lambda_satd_me[6]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SATD_ME_7,     val->lambda_satd_me[7]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SATD_ME_8,     val->lambda_satd_me[8]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SATD_ME_9,     val->lambda_satd_me[9]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SATD_ME_10,    val->lambda_satd_me[10]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SATD_ME_11,    val->lambda_satd_me[11]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SATD_ME_12,    val->lambda_satd_me[12]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SATD_ME_13,    val->lambda_satd_me[13]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SATD_ME_14,    val->lambda_satd_me[14]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SATD_ME_15,    val->lambda_satd_me[15]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SATD_ME_16,    val->lambda_satd_me[16]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SATD_ME_17,    val->lambda_satd_me[17]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SATD_ME_18,    val->lambda_satd_me[18]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SATD_ME_19,    val->lambda_satd_me[19]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SATD_ME_20,    val->lambda_satd_me[20]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SATD_ME_21,    val->lambda_satd_me[21]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SATD_ME_22,    val->lambda_satd_me[22]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SATD_ME_23,    val->lambda_satd_me[23]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SATD_ME_24,    val->lambda_satd_me[24]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SATD_ME_25,    val->lambda_satd_me[25]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SATD_ME_26,    val->lambda_satd_me[26]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SATD_ME_27,    val->lambda_satd_me[27]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SATD_ME_28,    val->lambda_satd_me[28]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SATD_ME_29,    val->lambda_satd_me[29]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SATD_ME_30,    val->lambda_satd_me[30]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SATD_ME_31,    val->lambda_satd_me[31]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SSE_ME_0,      val->lambda_sse_me[0]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SSE_ME_1,      val->lambda_sse_me[1]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SSE_ME_2,      val->lambda_sse_me[2]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SSE_ME_3,      val->lambda_sse_me[3]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SSE_ME_4,      val->lambda_sse_me[4]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SSE_ME_5,      val->lambda_sse_me[5]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SSE_ME_6,      val->lambda_sse_me[6]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SSE_ME_7,      val->lambda_sse_me[7]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SSE_ME_8,      val->lambda_sse_me[8]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SSE_ME_9,      val->lambda_sse_me[9]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SSE_ME_10,     val->lambda_sse_me[10]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SSE_ME_11,     val->lambda_sse_me[11]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SSE_ME_12,     val->lambda_sse_me[12]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SSE_ME_13,     val->lambda_sse_me[13]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SSE_ME_14,     val->lambda_sse_me[14]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SSE_ME_15,     val->lambda_sse_me[15]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SSE_ME_16,     val->lambda_sse_me[16]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SSE_ME_17,     val->lambda_sse_me[17]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SSE_ME_18,     val->lambda_sse_me[18]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SSE_ME_19,     val->lambda_sse_me[19]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SSE_ME_20,     val->lambda_sse_me[20]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SSE_ME_21,     val->lambda_sse_me[21]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SSE_ME_22,     val->lambda_sse_me[22]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SSE_ME_23,     val->lambda_sse_me[23]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SSE_ME_24,     val->lambda_sse_me[24]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SSE_ME_25,     val->lambda_sse_me[25]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SSE_ME_26,     val->lambda_sse_me[26]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SSE_ME_27,     val->lambda_sse_me[27]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SSE_ME_28,     val->lambda_sse_me[28]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SSE_ME_29,     val->lambda_sse_me[29]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SSE_ME_30,     val->lambda_sse_me[30]);
    H2PutRegVal(regs, HWIF_ENC_LAMDA_SSE_ME_31,     val->lambda_sse_me[31]);
    H2PutRegVal(regs, HWIF_ENC_INTRA_SATD_LAMDA_0,  val->lambda_satd_ims[0]);
    H2PutRegVal(regs, HWIF_ENC_INTRA_SATD_LAMDA_1,  val->lambda_satd_ims[1]);
    H2PutRegVal(regs, HWIF_ENC_INTRA_SATD_LAMDA_2,  val->lambda_satd_ims[2]);
    H2PutRegVal(regs, HWIF_ENC_INTRA_SATD_LAMDA_3,  val->lambda_satd_ims[3]);
    H2PutRegVal(regs, HWIF_ENC_INTRA_SATD_LAMDA_4,  val->lambda_satd_ims[4]);
    H2PutRegVal(regs, HWIF_ENC_INTRA_SATD_LAMDA_5,  val->lambda_satd_ims[5]);
    H2PutRegVal(regs, HWIF_ENC_INTRA_SATD_LAMDA_6,  val->lambda_satd_ims[6]);
    H2PutRegVal(regs, HWIF_ENC_INTRA_SATD_LAMDA_7,  val->lambda_satd_ims[7]);
    H2PutRegVal(regs, HWIF_ENC_INTRA_SATD_LAMDA_8,  val->lambda_satd_ims[8]);
    H2PutRegVal(regs, HWIF_ENC_INTRA_SATD_LAMDA_9,  val->lambda_satd_ims[9]);
    H2PutRegVal(regs, HWIF_ENC_INTRA_SATD_LAMDA_10, val->lambda_satd_ims[10]);
    H2PutRegVal(regs, HWIF_ENC_INTRA_SATD_LAMDA_11, val->lambda_satd_ims[11]);
    H2PutRegVal(regs, HWIF_ENC_INTRA_SATD_LAMDA_12, val->lambda_satd_ims[12]);
    H2PutRegVal(regs, HWIF_ENC_INTRA_SATD_LAMDA_13, val->lambda_satd_ims[13]);
    H2PutRegVal(regs, HWIF_ENC_INTRA_SATD_LAMDA_14, val->lambda_satd_ims[14]);
    H2PutRegVal(regs, HWIF_ENC_INTRA_SATD_LAMDA_15, val->lambda_satd_ims[15]);
    H2PutRegVal(regs, HWIF_ENC_INTRA_SATD_LAMDA_16, val->lambda_satd_ims[16]);
    H2PutRegVal(regs, HWIF_ENC_INTRA_SATD_LAMDA_17, val->lambda_satd_ims[17]);
    H2PutRegVal(regs, HWIF_ENC_INTRA_SATD_LAMDA_18, val->lambda_satd_ims[18]);
    H2PutRegVal(regs, HWIF_ENC_INTRA_SATD_LAMDA_19, val->lambda_satd_ims[19]);
    H2PutRegVal(regs, HWIF_ENC_INTRA_SATD_LAMDA_20, val->lambda_satd_ims[20]);
    H2PutRegVal(regs, HWIF_ENC_INTRA_SATD_LAMDA_21, val->lambda_satd_ims[21]);
    H2PutRegVal(regs, HWIF_ENC_INTRA_SATD_LAMDA_22, val->lambda_satd_ims[22]);
    H2PutRegVal(regs, HWIF_ENC_INTRA_SATD_LAMDA_23, val->lambda_satd_ims[23]);
    H2PutRegVal(regs, HWIF_ENC_INTRA_SATD_LAMDA_24, val->lambda_satd_ims[24]);
    H2PutRegVal(regs, HWIF_ENC_INTRA_SATD_LAMDA_25, val->lambda_satd_ims[25]);
    H2PutRegVal(regs, HWIF_ENC_INTRA_SATD_LAMDA_26, val->lambda_satd_ims[26]);
    H2PutRegVal(regs, HWIF_ENC_INTRA_SATD_LAMDA_27, val->lambda_satd_ims[27]);
    H2PutRegVal(regs, HWIF_ENC_INTRA_SATD_LAMDA_28, val->lambda_satd_ims[28]);
    H2PutRegVal(regs, HWIF_ENC_INTRA_SATD_LAMDA_29, val->lambda_satd_ims[29]);
    H2PutRegVal(regs, HWIF_ENC_INTRA_SATD_LAMDA_30, val->lambda_satd_ims[30]);
    H2PutRegVal(regs, HWIF_ENC_INTRA_SATD_LAMDA_31, val->lambda_satd_ims[31]);
#endif
#if 0
    /* denoise related */
    H2PutRegVal(regs, HWIF_ENC_NOISE_REDUCTION_ENABLE, val->noiseReductionEnable);
    H2PutRegVal(regs, HWIF_ENC_NOISE_LOW, val->noiseLow);
    val->nrMbNumInvert = (1<<MB_NUM_BIT_WIDTH)/(((((val->picWidth + 63) >> 6) << 6) * (((val->picHeight + 63) >> 6) << 6))/(16*16));
    H2PutRegVal(regs, HWIF_ENC_NR_MBNUM_INVERT_REG, val->nrMbNumInvert);
    H2PutRegVal(regs, HWIF_ENC_SLICEQP_PREV, val->nrSliceQPPrev& mask_6b);
    H2PutRegVal(regs, HWIF_ENC_THRESH_SIGMA_CUR, val->nrThreshSigmaCur& mask_21b);
    H2PutRegVal(regs, HWIF_ENC_SIGMA_CUR, val->nrSigmaCur& mask_16b);
#endif
    return 0;
}

static int _put_data(mhve_ops* mops, void* data, int size)
{
    h2v3_ops* h2v3 = (h2v3_ops*)mops;
    void* dst;
    int len = 0;
    /* copy sequence-header bits */
    if (h2v3->b_seqh)
    {
        h265_enc* h265 = h2v3->h265;
        dst = (char*)h2v3->p_otpt + h2v3->u_used;
        MEM_COPY(dst, h265->m_seqh, h265->i_seqh);
        h2v3->u_used += h265->i_seqh;
        len += h265->i_seqh;
    }
    /* insert user data */
    if (size > 0)
    {
        dst = (char*)h2v3->p_otpt + h2v3->u_used;
        MEM_COPY(dst, data, size);
        h2v3->u_used += size;
        len += (int)size;
    }
    return len;
}

static int _enc_done(mhve_ops* mops)
{
    h2v3_ops* h2v3 = (h2v3_ops*)mops;
    h265_enc* h265 = h2v3->h265;
    rqct_ops* rqct = h2v3->rqct;
    h2v3_mir* mirr = h2v3->mirr;
    int idx;
    /* feedback to rate-controller */
    rqct->enc_done(rqct, &mirr->mjob);
    h2v3->u_size = rqct->i_bitcnt/8;
    h2v3->u_used+= h2v3->u_size;
    h2v3->u_otrm = h2v3->u_used;
    if (0 <= (idx = h265_enc_done(h265)))
    {
        mhve_vpb* vpb = &h2v3->m_encp;
        rpb_t* rpb = h2v3->m_rpbs+idx;
        if (!rpb->b_valid)
            vpb->i_index = rpb->i_index;
        rpb->i_index = -1;
    }
    h2v3->i_seqn++;
    h2v3->b_seqh=0;
    return 0;
}

#define MVHE_FLAGS_CTRL (MVHE_FLAGS_IDR|MVHE_FLAGS_DISPOSABLE)

static int _out_buff(mhve_ops* mops, mhve_cpb* mcpb)
{
    h2v3_ops* h2v3 = (h2v3_ops*)mops;
    int err = 0;
    if (mcpb->i_index >= 0)
    {
        h2v3->u_used = h2v3->u_otrm = 0;
        mcpb->planes[0].u_phys = 0;
        mcpb->planes[0].i_size = 0;
        return err;
    }
    err = h2v3->u_used;
    mcpb->i_index = 0;
    mcpb->planes[0].u_phys = h2v3->u_otbs;
    mcpb->planes[0].i_size = err;
    mcpb->i_stamp = h2v3->m_encp.i_stamp;
    mcpb->i_flags = (MVHE_FLAGS_SOP|MVHE_FLAGS_EOP)|(h2v3->m_encp.u_flags&MVHE_FLAGS_CTRL);
    return err;
}

static int _set_conf(mhve_ops* mops, mhve_cfg* mcfg)
{
    h2v3_ops* h2v3 = (h2v3_ops*)mops;
    h265_enc* h265 = h2v3->h265;
    int err = -1;
    switch (mcfg->type)
    {
        pps_t* pps;
        sps_t* sps;
    case MHVE_CFG_RES:
        if ((unsigned)mcfg->res.e_pixf <= MHVE_PIX_YUYV)
        {
            pps = h265_find_set(h265, HEVC_PPS, 0);
            sps = pps->p_sps;
            h2v3->e_pixf = mcfg->res.e_pixf;
            h2v3->i_pixw = mcfg->res.i_pixw;
            h2v3->i_pixh = mcfg->res.i_pixh;
            h2v3->i_rpbn = mcfg->res.i_rpbn;
            h2v3->u_conf = mcfg->res.u_conf;
            h265->i_picw = h2v3->i_pixw;
            h265->i_pich = h2v3->i_pixh;
            h265->i_cb_w = (h265->i_picw + ((1 << sps->i_log2_max_cb_size) - 1)) >> sps->i_log2_max_cb_size;
            h265->i_cb_h = (h265->i_pich + ((1 << sps->i_log2_max_cb_size) - 1)) >> sps->i_log2_max_cb_size;
            h2v3->b_seqh = 1;
            err = 0;
        }
        break;
    case MHVE_CFG_DMA:
        if ((unsigned)mcfg->dma.i_dmem < 2)
        {
            uint addr = mcfg->dma.u_phys;
            int index = mcfg->dma.i_dmem;
            rpb_t* ref = h2v3->m_rpbs+index;
            ref->i_index = -1;
            ref->u_planes[RPB_YPIXEL] = !mcfg->dma.i_size[0]?0:addr;
            addr += mcfg->dma.i_size[0];
            ref->u_planes[RPB_CPIXEL] = !mcfg->dma.i_size[1]?0:addr;
            addr += mcfg->dma.i_size[1];
            ref->u_planes[RPB_LUMA4N] = addr;
            addr += mcfg->dma.i_size[2];
            ref->u_planes[RPB_YCOMPR] = !mcfg->dma.i_size[3]?0:addr;
            addr += mcfg->dma.i_size[3];
            ref->u_planes[RPB_CCOMPR] = !mcfg->dma.i_size[4]?0:addr;
            ref->b_valid = !ref->u_planes[RPB_YPIXEL]?0:1;
            err = 0;
        }
        else if (mcfg->dma.i_dmem == -1)
        {
            h2v3->p_otpt = mcfg->dma.p_vptr;
            h2v3->u_otbs = mcfg->dma.u_phys;
            h2v3->u_otsz = mcfg->dma.i_size[0];
            h2v3->u_otrm = h2v3->u_used = 0;
            err = 0;
        }
        else if (mcfg->dma.i_dmem == -2)
        {
            uint  addr = mcfg->dma.u_phys;
            char* vptr = mcfg->dma.p_vptr;
            h2v3->p_nalp = (uint*)vptr;
            h2v3->u_nalb = addr;
            addr += mcfg->dma.i_size[0];
            h2v3->u_coef = 0;
            if (mcfg->dma.i_size[1])
                h2v3->u_coef = addr;
            err = 0;
        }
        break;
    case MHVE_CFG_HEV:
        pps = h265_find_set(h265, HEVC_PPS, 0);
        sps = pps->p_sps;
        h265->i_profile = mcfg->hev.i_profile;
        h265->i_level = mcfg->hev.i_level;
        sps->i_log2_max_cb_size = mcfg->hev.i_log2_max_cb_size;
        sps->i_log2_min_cb_size = mcfg->hev.i_log2_min_cb_size;
        sps->i_log2_max_tr_size = mcfg->hev.i_log2_max_tr_size;
        sps->i_log2_min_tr_size = mcfg->hev.i_log2_min_tr_size;
        sps->i_max_tr_hierarchy_depth_intra = mcfg->hev.i_tr_depth_intra;
        sps->i_max_tr_hierarchy_depth_inter = mcfg->hev.i_tr_depth_inter;
        sps->b_scaling_list_enable = mcfg->hev.b_scaling_list_enable;
        sps->b_sao_enabled = mcfg->hev.b_sao_enable;
        sps->b_strong_intra_smoothing_enabled = mcfg->hev.b_strong_intra_smoothing;
        pps->b_cu_qp_delta_enabled = mcfg->hev.b_ctu_qp_delta_enable;
        pps->b_constrained_intra_pred = mcfg->hev.b_constrained_intra_pred;
        pps->b_deblocking_filter_override_enabled = mcfg->hev.b_deblocking_override_enable;
        pps->i_cb_qp_offset = mcfg->hev.i_cqp_offset;
        pps->i_cr_qp_offset = mcfg->hev.i_cqp_offset;
        pps->b_deblocking_filter_disabled =
        h265->b_deblocking_disable = mcfg->hev.b_deblocking_disable;
        if (h265->b_deblocking_disable)
            h265->i_tc_offset = h265->i_beta_offset = pps->i_tc_offset = pps->i_beta_offset = 0;
        else
        {
            h265->i_tc_offset = pps->i_tc_offset = mcfg->hev.i_tc_offset_div2*2;
            h265->i_beta_offset = pps->i_beta_offset = mcfg->hev.i_beta_offset_div2*2;
        }
        h2v3->b_seqh = 1;
        err = 0;
        break;
    case MHVE_CFG_VUI:
        sps = h265_find_set(h265, HEVC_SPS, 0);
        sps->b_vui_param_pres = 0;
        if (0 != (sps->vui.b_video_full_range = (mcfg->vui.b_video_full_range!=0)))
        {
            sps->vui.b_video_signal_pres = 1;
            sps->vui.i_video_format = 5;
            sps->vui.b_colour_desc_pres = 0;
            sps->b_vui_param_pres = 1;
        }
        h2v3->b_seqh = 1;
        err = 0;
        break;
    case MHVE_CFG_LFT:
        pps = h265_find_set(h265, HEVC_PPS, 0);
        if (!pps->b_deblocking_filter_override_enabled)
            break;
        if (!mcfg->lft.b_override)
        {
            h265->b_deblocking_override = 0;
            h265->b_deblocking_disable = pps->b_deblocking_filter_disabled;
            h265->i_tc_offset = pps->i_tc_offset;
            h265->i_beta_offset = pps->i_beta_offset;
            err = 0;
            break;
        }
        if (!mcfg->lft.b_disable && ((unsigned)(mcfg->lft.i_offsetA+6)>12 || (unsigned)(mcfg->lft.i_offsetB+6)>12))
            break;
        err = 0;
        h265->b_deblocking_override = 1;
        if (!(h265->b_deblocking_disable = mcfg->lft.b_disable))
        {
            h265->i_tc_offset  = mcfg->lft.i_offsetA*2;
            h265->i_beta_offset= mcfg->lft.i_offsetB*2;
        }
        break;
    case MHVE_CFG_SPL:
        h265->i_rows = mcfg->spl.i_rows;
        err = 0;
        break;
    case MHVE_CFG_BAC:
        h265->b_cabac_init = mcfg->bac.b_init;
        err = 0;
        break;
    default:
        break;
    }
    return err;
}

static int _get_conf(mhve_ops* mops, mhve_cfg* mcfg)
{
    h2v3_ops* h2v3 = (h2v3_ops*)mops;
    h265_enc* h265 = h2v3->h265;
    int err = -1;
    switch (mcfg->type)
    {
        pps_t* pps;
        sps_t* sps;
    case MHVE_CFG_RES:
        mcfg->res.e_pixf = h2v3->e_pixf;
        mcfg->res.i_pixw = h2v3->i_pixw;
        mcfg->res.i_pixh = h2v3->i_pixh;
        mcfg->res.i_rpbn = h2v3->i_rpbn;
        mcfg->res.u_conf = h2v3->u_conf;
        err = 0;
        break;
    case MHVE_CFG_DMA:
        if (mcfg->dma.i_dmem == -1)
        {
            mcfg->dma.p_vptr = h2v3->p_otpt;
            mcfg->dma.u_phys = h2v3->u_otbs;
            mcfg->dma.i_size[0] = (int)h2v3->u_otsz;
            err = 0;
        }
        break;
    case MHVE_CFG_HEV:
        pps = h265_find_set(h265, HEVC_PPS, 0);
        sps = pps->p_sps;
        mcfg->hev.i_profile = h265->i_profile;
        mcfg->hev.i_level = h265->i_level;
        mcfg->hev.i_log2_max_cb_size = sps->i_log2_max_cb_size;
        mcfg->hev.i_log2_min_cb_size = sps->i_log2_min_cb_size;
        mcfg->hev.i_log2_max_tr_size = sps->i_log2_max_tr_size;
        mcfg->hev.i_log2_min_tr_size = sps->i_log2_min_tr_size;
        mcfg->hev.i_tr_depth_intra = sps->i_max_tr_hierarchy_depth_intra;
        mcfg->hev.i_tr_depth_inter = sps->i_max_tr_hierarchy_depth_inter;
        mcfg->hev.b_scaling_list_enable = sps->b_scaling_list_enable;
        mcfg->hev.b_sao_enable = sps->b_sao_enabled;
        mcfg->hev.b_strong_intra_smoothing = sps->b_strong_intra_smoothing_enabled;
        mcfg->hev.b_ctu_qp_delta_enable = pps->b_cu_qp_delta_enabled;
        mcfg->hev.b_constrained_intra_pred = pps->b_constrained_intra_pred;
        mcfg->hev.b_deblocking_override_enable = pps->b_deblocking_filter_override_enabled;
        mcfg->hev.i_cqp_offset = pps->i_cb_qp_offset;
        mcfg->hev.b_deblocking_disable = pps->b_deblocking_filter_disabled;
        mcfg->hev.i_tc_offset_div2 = pps->i_tc_offset/2;
        mcfg->hev.i_beta_offset_div2 = pps->i_beta_offset/2;
        err = 0;
        break;
    case MHVE_CFG_VUI:
        sps = h265_find_set(h265, HEVC_SPS, 0);
        mcfg->vui.b_video_full_range = sps->b_vui_param_pres!=0 && sps->vui.b_video_full_range!=0;
        err = 0;
        break;
    case MHVE_CFG_LFT:
        mcfg->lft.b_override= (signed char)h265->b_deblocking_override;
        mcfg->lft.b_disable = (signed char)h265->b_deblocking_disable;
        mcfg->lft.i_offsetA = (signed char)h265->i_tc_offset/2;
        mcfg->lft.i_offsetB = (signed char)h265->i_beta_offset/2;
        err = 0;
        break;
    case MHVE_CFG_SPL:
        mcfg->spl.i_rows = h265->i_rows;
        mcfg->spl.i_bits = err = 0;
        break;
    case MHVE_CFG_BAC:
        mcfg->bac.b_init = h265->b_cabac_init!=0;
        err = 0;
        break;
    default:
        break;
    }
    return err;
}
