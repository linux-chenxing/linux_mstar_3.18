
#include <linux/ioctl.h>
#include <linux/uaccess.h>
#include <linux/semaphore.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/uaccess.h>

#include <ms_platform.h>
#include <ms_msys.h>

#include <mdrv_mvhe_io.h>
#include <mdrv_rqct_io.h>

#include <mvhe_defs.h>

#define _ALIGN(a,b) (((a)+(1<<(b))-1)&(~((1<<(b))-1)))

static int _vhectx_streamon(void* pctx, int on);
static int _vhectx_set_parm(void* pctx, mvhe_parm* parm);
static int _vhectx_get_parm(void* pctx, mvhe_parm* parm);
static int _vhectx_set_ctrl(void* pctx, mvhe_ctrl* ctrl);
static int _vhectx_get_ctrl(void* pctx, mvhe_ctrl* ctrl);
static int _vhectx_enc_pict(void* pctx, mvhe_buff* buff);
static int _vhectx_get_bits(void* pctx, mvhe_buff* buff);
static int _vhectx_compress(void* pctx, mvhe_buff* buff);
static int _vhectx_put_data(void* pctx, mvhe_buff* buff);
static int _vhectx_set_rqcf(void* pctx, rqct_conf* rqcf);
static int _vhectx_get_rqcf(void* pctx, rqct_conf* rqcf);
static void _vhectx_release(void* pctx)
{
    mvhe_ctx* mctx = pctx;
    if (atomic_dec_and_test(&mctx->i_refcnt))
    {
        mhve_ops* mops = mctx->p_handle;
        while (mctx->i_dmems > 0)
            msys_release_dmem(&mctx->m_dmems[--mctx->i_dmems]);
        mutex_lock(&mctx->m_stream);
        mctx->p_handle = NULL;
        mops->release(mops);
        mutex_unlock(&mctx->m_stream);
#if MVHE_TIMER_SIZE>0
        kfree(mctx->p_timer);
#endif
        kfree(mctx->p_usrdt);
        kfree(mctx);
    }
}
static void _vhectx_adduser(void* pctx)
{
    mvhe_ctx* mctx = pctx;
    atomic_inc(&mctx->i_refcnt);
}

void* mvheops_acquire(int);

mvhe_ctx*
mvhectx_acquire(
    mvhe_dev*   mdev)
{
    mvhe_ctx* mctx = NULL;
    mhve_ops* mops = NULL;
    if (!mdev || !(mctx = kzalloc(sizeof(mvhe_ctx), GFP_KERNEL)))
        return mctx;
    do
    {
        if (!(mops = mvheops_acquire(mdev->i_rctidx)))
            break;
        if (!(mctx->p_usrdt = kzalloc(MVHE_USER_DATA_SIZE, GFP_KERNEL)))
            break;
        mctx->i_usrsz = mctx->i_usrcn = 0;
#if MVHE_TIMER_SIZE>0
        mctx->p_timer = kzalloc(MVHE_TIMER_SIZE, GFP_KERNEL);
        mctx->i_numbr = 0;
#endif
        mutex_init(&mctx->m_stream);
        mutex_init(&mctx->m_encode);
        mctx->i_state = MVHE_CTX_STATE_NULL;
        mctx->release =_vhectx_release;
        mctx->adduser =_vhectx_adduser;
        atomic_set(&mctx->i_refcnt, 1);
        mctx->p_handle = mops;
        return mctx;
    }
    while (0);
    if (mops)
        mops->release(mops);
    kfree(mctx);
    return NULL;
}

long
mvhectx_actions(
    mvhe_ctx*       mctx,
    unsigned int    cmd,
    void*           arg)
{
    int err = 0;
    switch (cmd)
    {
    case IOCTL_MVHE_S_PARM:
        err = _vhectx_set_parm(mctx, (mvhe_parm*)arg);
        break;
    case IOCTL_MVHE_G_PARM:
        err = _vhectx_get_parm(mctx, (mvhe_parm*)arg);
        break;
    case IOCTL_MVHE_STREAMON:
        err = _vhectx_streamon(mctx, 1);
        break;
    case IOCTL_MVHE_STREAMOFF:
        err = _vhectx_streamon(mctx, 0);
        break;
    case IOCTL_MVHE_S_PICT:
        err = _vhectx_enc_pict(mctx, (mvhe_buff*)arg);
        break;
    case IOCTL_MVHE_G_BITS:
        err = _vhectx_get_bits(mctx, (mvhe_buff*)arg);
        break;
    case IOCTL_MVHE_S_CTRL:
        err = _vhectx_set_ctrl(mctx, (mvhe_ctrl*)arg);
        break;
    case IOCTL_MVHE_G_CTRL:
        err = _vhectx_get_ctrl(mctx, (mvhe_ctrl*)arg);
        break;
    case IOCTL_MVHE_ENCODE:
        err = _vhectx_compress(mctx, (mvhe_buff*)arg);
        break;
    case IOCTL_MVHE_S_DATA:
        err = _vhectx_put_data(mctx, (mvhe_buff*)arg);
        break;
    case IOCTL_RQCT_S_CONF:
        err = _vhectx_set_rqcf(mctx, (rqct_conf*)arg);
        break;
    case IOCTL_RQCT_G_CONF:
        err = _vhectx_get_rqcf(mctx, (rqct_conf*)arg);
        break;
    default:
        err = -EINVAL;
        break;
    }
    return err;
}

#define MVHE_COEF_LEN  (36*1024)
#define MVHE_NALU_LEN  ( 4*1024)
#define MVHE_MISC_LEN  (MVHE_COEF_LEN+MVHE_NALU_LEN)

static int
_vhectx_streamon(
    void*           pctx,
    int             on)
{
    mvhe_ctx* mctx = pctx;
    mvhe_dev* mdev = mctx->p_device;
    mhve_ops* mops = mctx->p_handle;
    rqct_ops* rqct = mops->rqct_ops(mops);
    rqct_cfg  rqcf;
    int i, err = 0;

    mutex_lock(&mctx->m_stream);
    do
    if (on)
    {
        mhve_cfg mcfg;
        int pixels_size, luma4n_size, chroma_size, output_size;
        int compr_ysize = 0;
        int rpbuff_size;
        int outsize;
        int score, rpbn;
        uint  addr;
        char* kptr;
        msys_mem* pdmem;
        if (MVHE_CTX_STATE_NULL != mctx->i_state)
            break;
        mctx->i_dmems = 0;
        mcfg.type = MHVE_CFG_RES;
        mops->get_conf(mops, &mcfg);
        pixels_size = (int)mctx->i_max_w*mctx->i_max_h;
        luma4n_size = (pixels_size)/4;
        chroma_size = (pixels_size)/2;
        output_size = _ALIGN(pixels_size,19);
        if (mcfg.res.u_conf&MHVE_CFG_OMMAP)
            output_size = 0;
        if (mcfg.res.u_conf&MHVE_CFG_COMPR)
            compr_ysize = pixels_size/512;
        if (mctx->i_imode == MVHE_IMODE_PLUS)
            pixels_size = chroma_size = 0;
        rpbn = mcfg.res.i_rpbn;
        rpbuff_size = pixels_size + luma4n_size + chroma_size + compr_ysize;
        score = pixels_size >> 12;
        do
        {
            outsize = output_size + MVHE_MISC_LEN;
            pdmem = &mctx->m_dmems[mctx->i_dmems++];
            snprintf(pdmem->name,15,"S%d:VHEDMOUT",mctx->i_index);
            pdmem->length = outsize;
            if (0 != (err = msys_request_dmem(pdmem)))
                break;
            addr = Chip_Phys_to_MIU(pdmem->phys);
            kptr = (char*)((uintptr_t)pdmem->kvirt);
            mctx->i_omode = MVHE_OMODE_MMAP;
            mctx->p_ovptr = NULL;
            mctx->u_ophys = 0;
            mctx->i_osize = mctx->i_ormdr = 0;
            if (output_size > 0)
            {
                mcfg.type = MHVE_CFG_DMA;
                mcfg.dma.i_dmem = -1;
                mcfg.dma.p_vptr = kptr;
                mcfg.dma.u_phys = addr;
                mcfg.dma.i_size[0] = output_size;
                mops->set_conf(mops, &mcfg);
                mctx->i_omode = MVHE_OMODE_USER;
                mctx->p_ovptr = kptr;
                mctx->u_ophys = addr;
                mctx->i_osize = output_size;
                kptr += output_size;
                addr += output_size;
            }
            mcfg.type = MHVE_CFG_DMA;
            mcfg.dma.i_dmem = -2;
            mcfg.dma.p_vptr = kptr;
            mcfg.dma.u_phys = addr;
            mcfg.dma.i_size[0] = MVHE_NALU_LEN;
            mcfg.dma.i_size[1] = compr_ysize?MVHE_COEF_LEN:0;
            mops->set_conf(mops, &mcfg);

            for (i = 0; i < rpbn; i++)
            {
                pdmem = &mctx->m_dmems[mctx->i_dmems++];
                snprintf(pdmem->name,15,"S%d:VHEDMRP%d",mctx->i_index,i);
                pdmem->length = rpbuff_size;
                if (0 != (err = msys_request_dmem(pdmem)))
                    break;
                addr = Chip_Phys_to_MIU(pdmem->phys);
                kptr = (char*)((uintptr_t)pdmem->kvirt);
                mcfg.type = MHVE_CFG_DMA;
                mcfg.dma.i_dmem = i;
                mcfg.dma.u_phys = addr;
                mcfg.dma.p_vptr = kptr;
                mcfg.dma.i_size[0] = pixels_size;
                mcfg.dma.i_size[1] = chroma_size;
                mcfg.dma.i_size[2] = luma4n_size;
                mcfg.dma.i_size[3] = compr_ysize;
                mops->set_conf(mops, &mcfg);
            }
            rqcf.type = RQCT_CFG_FPS;
            rqct->get_rqcf(rqct, &rqcf);
            score *= (int)rqcf.fps.n_fps;
            score /= (int)rqcf.fps.d_fps;
            mctx->i_score = score;
        }
        while (0);

        if (!err && !(err = mops->seq_sync(mops)))
        {
            mops->seq_conf(mops);
            mvhedev_poweron(mdev, mctx->i_score);
            mdev->i_counts[mctx->i_index][0] = mdev->i_counts[mctx->i_index][1] = 0;
            mdev->i_counts[mctx->i_index][2] = mdev->i_counts[mctx->i_index][3] = 0;
            mctx->i_state = MVHE_CTX_STATE_IDLE;
            break;
        }
    }
    else
    {
        if (MVHE_CTX_STATE_NULL == mctx->i_state)
            break;
        mvhedev_poweron(mdev,-mctx->i_score);
        mctx->i_state = MVHE_CTX_STATE_NULL;
        mctx->i_score = 0;
        mops->seq_done(mops);
#if MVHE_TIMER_SIZE>0
        printk("<%d>vhe performance:\n",mctx->i_id);
        for (i = 0; i < MVHE_TIMER_SIZE/8; i++)
            printk("<%d>%4d/%4d/%8d\n",mctx->i_id,mctx->p_timer[i].tm_dur[0],mctx->p_timer[i].tm_dur[1],mctx->p_timer[i].tm_cycles);
#endif
    }
    while (0);
    if (!on || err)
    {
        while (mctx->i_dmems > 0)
            msys_release_dmem(&mctx->m_dmems[--mctx->i_dmems]);
    }
    mutex_unlock(&mctx->m_stream);

    return err;
}

static int
_vhectx_set_parm(
    void*           pctx,
    mvhe_parm*      parm)
{
    mvhe_ctx* mctx = pctx;
    mhve_ops* mops = mctx->p_handle;
    rqct_ops* rqct = mops->rqct_ops(mops);
    int err = -EINVAL;

    mutex_lock(&mctx->m_stream);
    if (MVHE_CTX_STATE_NULL == mctx->i_state)
    {
        mhve_cfg mcfg;
        rqct_cfg rqcf;
        switch (parm->type)
        {
        case MVHE_PARM_RES:
            if ((unsigned)parm->res.i_pixfmt > MVHE_PIXFMT_YUYV)
                break;
            if ((parm->res.i_pict_w%16) || (parm->res.i_pict_h%8))
                break;
            mctx->i_max_w = _ALIGN(parm->res.i_pict_w,6);
            mctx->i_max_h = _ALIGN(parm->res.i_pict_h,6);
            mcfg.type = MHVE_CFG_RES;
            mcfg.res.e_pixf = parm->res.i_pixfmt;
            mcfg.res.i_pixw = parm->res.i_pict_w;
            mcfg.res.i_pixh = parm->res.i_pict_h;
            mcfg.res.i_rpbn = 2;
            mcfg.res.u_conf = 0;
            if (parm->res.i_flags&MVHE_FLAGS_COMPR)
                mcfg.res.u_conf |= MHVE_CFG_COMPR;
            if (parm->res.i_outlen < 0)
                mcfg.res.u_conf |= MHVE_CFG_OMMAP;
            if (parm->res.i_outlen <-1)
                mctx->i_imode = MVHE_IMODE_PLUS;
            mops->set_conf(mops, &mcfg);
            rqcf.type = RQCT_CFG_RES;
            rqcf.res.i_picw = parm->res.i_pict_w;
            rqcf.res.i_pich = parm->res.i_pict_h;
            rqct->set_rqcf(rqct, &rqcf);
            rqcf.type = RQCT_CFG_DQM;
            rqcf.dqm.i_dqmw = mctx->i_max_w>>6;
            rqcf.dqm.i_dqmh = mctx->i_max_h>>6;
            rqct->set_rqcf(rqct, &rqcf);
            err = 0;
            break;
        case MVHE_PARM_FPS:
            if (parm->fps.i_den == 0 || parm->fps.i_num == 0)
                break;
            rqcf.type = RQCT_CFG_FPS;
            rqcf.fps.n_fps = (short)parm->fps.i_num;
            rqcf.fps.d_fps = (short)parm->fps.i_den;
            err = rqct->set_rqcf(rqct, &rqcf);
            break;
        case MVHE_PARM_GOP:
            if (parm->gop.i_pframes >= 0)
            {
                rqcf.type = RQCT_CFG_SEQ;
                if (!rqct->get_rqcf(rqct, &rqcf))
                {
                    rqcf.seq.i_period = parm->gop.i_pframes+1;
                    if (!rqct->set_rqcf(rqct, &rqcf))
                    {
                        parm->gop.i_bframes = 0;
                        err = 0;
                    }
                }
            }
            break;
        case MVHE_PARM_BPS:
            if ((unsigned)parm->bps.i_method <= RQCT_METHOD_VBR)
            {
                rqcf.type = RQCT_CFG_SEQ;
                if (!rqct->get_rqcf(rqct, &rqcf))
                {
                    rqcf.seq.i_method = parm->bps.i_method;
                    rqcf.seq.i_leadqp = parm->bps.i_ref_qp;
                    rqcf.seq.i_btrate = parm->bps.i_bps;
                    err = rqct->set_rqcf(rqct, &rqcf);
                }
            }
            break;
        case MVHE_PARM_HEVC:
            if ((parm->hevc.i_log2_max_cb_size-3) > 3 ||
                (parm->hevc.i_log2_min_cb_size-3) > 3 ||
                parm->hevc.i_log2_max_cb_size < parm->hevc.i_log2_min_cb_size)
                break;
            if ((parm->hevc.i_log2_max_tr_size-2) > 3 ||
                (parm->hevc.i_log2_min_tr_size-2) > 3 ||
                parm->hevc.i_log2_max_tr_size < parm->hevc.i_log2_min_tr_size ||
                parm->hevc.i_log2_max_tr_size < parm->hevc.i_log2_min_cb_size)
                break;
            if (parm->hevc.i_tr_depth_intra > (parm->hevc.i_log2_max_cb_size-parm->hevc.i_log2_min_tr_size) ||
                parm->hevc.i_tr_depth_inter > (parm->hevc.i_log2_max_cb_size-parm->hevc.i_log2_min_tr_size))
                break;
            if ((unsigned)(parm->hevc.i_tc_offset_div2+6) > 12 ||
                (unsigned)(parm->hevc.i_beta_offset_div2+6) > 12 ||
                (unsigned)(parm->hevc.i_cqp_offset+12) > 24)
                break;
            mcfg.type = MHVE_CFG_HEV;
            mcfg.hev.i_profile = parm->hevc.i_profile;
            mcfg.hev.i_level = parm->hevc.i_level;
            mcfg.hev.i_log2_max_cb_size = parm->hevc.i_log2_max_cb_size;
            mcfg.hev.i_log2_min_cb_size = parm->hevc.i_log2_min_cb_size;
            mcfg.hev.i_log2_max_tr_size = parm->hevc.i_log2_max_tr_size;
            mcfg.hev.i_log2_min_tr_size = parm->hevc.i_log2_min_tr_size;
            mcfg.hev.i_tr_depth_intra = parm->hevc.i_tr_depth_intra;
            mcfg.hev.i_tr_depth_inter = parm->hevc.i_tr_depth_inter;
            mcfg.hev.b_scaling_list_enable = parm->hevc.b_scaling_list_enable;
            mcfg.hev.b_ctu_qp_delta_enable = parm->hevc.b_ctu_qp_delta_enable;
            mcfg.hev.b_sao_enable = parm->hevc.b_sao_enable;
            mcfg.hev.i_cqp_offset = parm->hevc.i_cqp_offset;
            mcfg.hev.b_strong_intra_smoothing = parm->hevc.b_strong_intra_smoothing;
            mcfg.hev.b_constrained_intra_pred = parm->hevc.b_constrained_intra_pred;
            mcfg.hev.b_deblocking_override_enable = parm->hevc.b_deblocking_override_enable;
            mcfg.hev.b_deblocking_disable = parm->hevc.b_deblocking_disable;
            mcfg.hev.i_tc_offset_div2 = parm->hevc.i_tc_offset_div2;
            mcfg.hev.i_beta_offset_div2 = parm->hevc.i_beta_offset_div2;
            err = mops->set_conf(mops, &mcfg);
            break;
        case MVHE_PARM_VUI:
            mcfg.type = MHVE_CFG_VUI;
            mcfg.vui.b_video_full_range = parm->vui.b_video_full_range!=0;
            err = mops->set_conf(mops, &mcfg);
            break;
        default:
            break;
        }
    }
    mutex_unlock(&mctx->m_stream);
    return err;
}

static int
_vhectx_get_parm(
    void*           pctx,
    mvhe_parm*      parm)
{
    int err = 0;
    mvhe_ctx* mctx = pctx;
    mhve_ops* mops = mctx->p_handle;
    rqct_ops* rqct = mops->rqct_ops(mops);
    mhve_cfg  mcfg;
    rqct_cfg  rqcf;

    mutex_lock(&mctx->m_stream);
    switch (parm->type)
    {
    case MVHE_PARM_IDX:
        parm->idx.i_stream = mctx->i_index;
        break;
    case MVHE_PARM_RES:
        mcfg.type = MHVE_CFG_RES;
        mops->get_conf(mops, &mcfg);
        parm->res.i_pict_w = mcfg.res.i_pixw;
        parm->res.i_pict_h = mcfg.res.i_pixh;
        parm->res.i_pixfmt = mcfg.res.e_pixf;
        parm->res.i_outlen = 0;
        parm->res.i_flags = (mcfg.res.u_conf&MHVE_CFG_COMPR)?MVHE_FLAGS_COMPR:0;
        break;
    case MVHE_PARM_FPS:
        rqcf.type = RQCT_CFG_FPS;
        if (!(err = rqct->get_rqcf(rqct, &rqcf)))
        {
            parm->fps.i_den = rqcf.fps.d_fps;
            parm->fps.i_num = rqcf.fps.n_fps;
        }
        break;
    case MVHE_PARM_GOP:
    case MVHE_PARM_BPS:
        rqcf.type = RQCT_CFG_SEQ;
        if (!(err = rqct->get_rqcf(rqct, &rqcf)))
        {
            if (parm->type == MVHE_PARM_BPS)
            {
                parm->bps.i_method = rqcf.seq.i_method;
                parm->bps.i_ref_qp = rqcf.seq.i_leadqp;
                parm->bps.i_bps = rqcf.seq.i_btrate;
                break;
            }
            parm->gop.i_pframes = rqcf.seq.i_period-1;
            parm->gop.i_bframes = 0; /* not support b-frame */
        }
        break;
    case MVHE_PARM_HEVC:
        mcfg.type = MHVE_CFG_HEV;
        if (0 != (err = mops->get_conf(mops, &mcfg)))
            break;
        parm->hevc.i_profile = mcfg.hev.i_profile;
        parm->hevc.i_level = mcfg.hev.i_level;
        parm->hevc.i_log2_max_cb_size = mcfg.hev.i_log2_max_cb_size;
        parm->hevc.i_log2_min_cb_size = mcfg.hev.i_log2_min_cb_size;
        parm->hevc.i_log2_max_tr_size = mcfg.hev.i_log2_max_tr_size;
        parm->hevc.i_log2_min_tr_size = mcfg.hev.i_log2_min_tr_size;
        parm->hevc.i_tr_depth_intra = mcfg.hev.i_tr_depth_intra;
        parm->hevc.i_tr_depth_inter = mcfg.hev.i_tr_depth_inter;
        parm->hevc.b_scaling_list_enable = mcfg.hev.b_scaling_list_enable;
        parm->hevc.b_ctu_qp_delta_enable = mcfg.hev.b_ctu_qp_delta_enable;
        parm->hevc.b_sao_enable = mcfg.hev.b_sao_enable;
        parm->hevc.b_strong_intra_smoothing = mcfg.hev.b_strong_intra_smoothing;
        parm->hevc.b_constrained_intra_pred = mcfg.hev.b_constrained_intra_pred;
        parm->hevc.b_deblocking_disable = mcfg.hev.b_deblocking_disable;
        parm->hevc.b_deblocking_override_enable = mcfg.hev.b_deblocking_override_enable;
        parm->hevc.i_tc_offset_div2 = mcfg.hev.i_tc_offset_div2;
        parm->hevc.i_beta_offset_div2 = mcfg.hev.i_beta_offset_div2;
        parm->hevc.i_cqp_offset = mcfg.hev.i_cqp_offset;
        break;
    case MVHE_PARM_VUI:
        mcfg.type = MHVE_CFG_VUI;
        if (!(err = mops->get_conf(mops, &mcfg)))
            parm->vui.b_video_full_range = 0!=mcfg.vui.b_video_full_range;
        break;
    default:
        err = -EINVAL;
        break;
    }
    mutex_unlock(&mctx->m_stream);
    return err;
}

static int
_vhectx_set_ctrl(
    void*       pctx,
    mvhe_ctrl*  ctrl)
{
    int err = -EINVAL;
    mvhe_ctx* mctx = pctx;
    mhve_ops* mops = mctx->p_handle;
    rqct_ops* rqct = mops->rqct_ops(mops);
    rqct_cfg rqcf;
    mhve_cfg mcfg;

    mutex_lock(&mctx->m_stream);
    if (MVHE_CTX_STATE_NULL < mctx->i_state)
    switch (ctrl->type)
    {
    case MVHE_CTRL_ROI:
        mcfg.type = MHVE_CFG_HEV;
        mops->get_conf(mops, &mcfg);
        if (!mcfg.hev.b_ctu_qp_delta_enable)
            break;
        rqcf.type = RQCT_CFG_ROI;
        rqcf.roi.i_roiidx = ctrl->roi.i_index;
        rqcf.roi.i_roidqp = ctrl->roi.i_dqp;
        rqcf.roi.i_posx = ctrl->roi.i_cbx;
        rqcf.roi.i_posy = ctrl->roi.i_cby;
        rqcf.roi.i_recw = ctrl->roi.i_cbw;
        rqcf.roi.i_rech = ctrl->roi.i_cbh;
        err = rqct->set_rqcf(rqct, &rqcf);
        break;
    case MVHE_CTRL_SPL:
        mcfg.type = MHVE_CFG_SPL;
        mcfg.spl.i_rows = ctrl->spl.i_rows;
        mcfg.spl.i_bits = 0;
        err = mops->set_conf(mops, &mcfg);
        break;
    case MVHE_CTRL_BAC:
        mcfg.type = MHVE_CFG_BAC;
        mcfg.bac.b_init = ctrl->bac.b_init!=0;
        err = mops->set_conf(mops, &mcfg);
        break;
    case MVHE_CTRL_DBK:
        mcfg.type = MHVE_CFG_LFT;
        mcfg.lft.b_override = ctrl->dbk.b_override;
        mcfg.lft.b_disable = ctrl->dbk.b_disable;
        mcfg.lft.i_offsetA = ctrl->dbk.i_tc_offset_div2;
        mcfg.lft.i_offsetB = ctrl->dbk.i_beta_offset_div2;
        err = mops->set_conf(mops, &mcfg);
        break;
    case MVHE_CTRL_SEQ:
        if ((unsigned)ctrl->seq.i_pixfmt <= MVHE_PIXFMT_YUYV &&
            ctrl->seq.i_pixelw >= 128 && ctrl->seq.i_pixelw <= mctx->i_max_w &&
            ctrl->seq.i_pixelh >= 128 && ctrl->seq.i_pixelh <= mctx->i_max_h &&
            ctrl->seq.d_fps > 0 &&
            ctrl->seq.n_fps > 0)
        {
            int cbw, cbh;
            mcfg.type = MHVE_CFG_RES;
            mops->get_conf(mops, &mcfg);
            mcfg.res.e_pixf = ctrl->seq.i_pixfmt;
            mcfg.res.i_pixw = ctrl->seq.i_pixelw;
            mcfg.res.i_pixh = ctrl->seq.i_pixelh;
            if ((err = mops->set_conf(mops, &mcfg)))
                break;
            cbw = _ALIGN(mcfg.res.i_pixw,6)>>6;
            cbh = _ALIGN(mcfg.res.i_pixh,6)>>6;
            rqcf.type = RQCT_CFG_FPS;
            rqcf.fps.n_fps = ctrl->seq.n_fps;
            rqcf.fps.d_fps = ctrl->seq.d_fps;
            if ((err = rqct->set_rqcf(rqct, &rqcf)))
                break;
            rqcf.type = RQCT_CFG_RES;
            rqcf.res.i_picw = ctrl->seq.i_pixelw;
            rqcf.res.i_pich = ctrl->seq.i_pixelh;
            if ((err = rqct->set_rqcf(rqct, &rqcf)))
                break;
            /* disable ROI */
            rqcf.type = RQCT_CFG_ROI;
            rqcf.roi.i_roiidx = -1;
            rqct->set_rqcf(rqct, &rqcf);
            /* reset dqm */
            rqcf.type = RQCT_CFG_DQM;
            rqct->get_rqcf(rqct, &rqcf);
            rqcf.dqm.i_dqmw = cbw;
            rqcf.dqm.i_dqmh = cbh;
            rqct->set_rqcf(rqct, &rqcf);
        }
        break;
    default:
        break;
    }
    mutex_unlock(&mctx->m_stream);
    return err;
}

static int
_vhectx_get_ctrl(
    void*       pctx,
    mvhe_ctrl*  ctrl)
{
    int err = -EINVAL;
    mvhe_ctx* mctx = pctx;
    mhve_ops* mops = mctx->p_handle;
    rqct_ops* rqct = mops->rqct_ops(mops);
    rqct_cfg rqcf;
    mhve_cfg mcfg;

    mutex_lock(&mctx->m_stream);
    if (MVHE_CTX_STATE_NULL < mctx->i_state)
    switch (ctrl->type)
    {
    case MVHE_CTRL_ROI:
        rqcf.type = RQCT_CFG_ROI;
        rqcf.roi.i_roiidx = ctrl->roi.i_index;
        if (!(err = rqct->get_rqcf(rqct, &rqcf)))
        {
            ctrl->roi.i_dqp = rqcf.roi.i_roidqp;
            ctrl->roi.i_cbx = rqcf.roi.i_posx;
            ctrl->roi.i_cby = rqcf.roi.i_posy;
            ctrl->roi.i_cbw = rqcf.roi.i_recw;
            ctrl->roi.i_cbh = rqcf.roi.i_rech;
        }
        break;
    case MVHE_CTRL_SPL:
        mcfg.type = MHVE_CFG_SPL;
        if (!(err = mops->get_conf(mops, &mcfg)))
        {
            ctrl->spl.i_rows = mcfg.spl.i_rows;
            ctrl->spl.i_bits = 0;
        }
        break;
    case MVHE_CTRL_BAC:
        mcfg.type = MHVE_CFG_BAC;
        if (!(err = mops->get_conf(mops, &mcfg)))
            ctrl->bac.b_init = mcfg.bac.b_init;
        break;
    case MVHE_CTRL_DBK:
        mcfg.type = MHVE_CFG_LFT;
        if (!(err = mops->get_conf(mops, &mcfg)))
        {
            ctrl->dbk.b_override = (short)mcfg.lft.b_override;
            ctrl->dbk.b_disable  = (short)mcfg.lft.b_disable;
            ctrl->dbk.i_tc_offset_div2   = (short)mcfg.lft.i_offsetA;
            ctrl->dbk.i_beta_offset_div2 = (short)mcfg.lft.i_offsetB;
        }
        break;
    case MVHE_CTRL_SEQ:
        mcfg.type = MHVE_CFG_RES;
        mops->get_conf(mops, &mcfg);
        ctrl->seq.i_pixelw = mcfg.res.i_pixw;
        ctrl->seq.i_pixelh = mcfg.res.i_pixh;
        ctrl->seq.i_pixfmt = mcfg.res.e_pixf;
        rqcf.type = RQCT_CFG_FPS;
        rqct->get_rqcf(rqct, &rqcf);
        ctrl->seq.n_fps = (int)rqcf.fps.n_fps;
        ctrl->seq.d_fps = (int)rqcf.fps.d_fps;
        err = 0;
        break;
    default:
        break;
    }
    mutex_unlock(&mctx->m_stream);

    return err;
}

#define MVHE_FLAGS_CONTROL  (MVHE_FLAGS_IDR|MVHE_FLAGS_DISPOSABLE|MVHE_FLAGS_NIGHT_MODE)

static int
_vhectx_enc_pict(
    void*           pctx,
    mvhe_buff*      buff)
{
    int err = -EINVAL;
    int pitch = 0;
    mvhe_ctx* mctx = pctx;
    mvhe_dev* mdev = mctx->p_device;
    mhve_ops* mops = mctx->p_handle;

    if (buff->i_memory != MVHE_MEMORY_MMAP)
        return -EINVAL;
    pitch = buff->i_stride;
    if (pitch < buff->i_width)
        pitch = buff->i_width;

    mutex_lock(&mctx->m_stream);
    do
    if (MVHE_CTX_STATE_IDLE == mctx->i_state && MVHE_OMODE_USER == mctx->i_omode)
    {
        mhve_vpb mvpb;
        mhve_cfg mcfg;

        mcfg.type = MHVE_CFG_RES;
        mops->get_conf(mops, &mcfg);
        if (buff->i_planes != 2 && (MHVE_PIX_NV21 >= mcfg.res.e_pixf))
            break;
        if (buff->i_planes != 1 && (MHVE_PIX_YUYV == mcfg.res.e_pixf))
            break;
        if (mcfg.res.i_pixw != buff->i_width || mcfg.res.i_pixh != buff->i_height)
            break;

        mvpb.i_index = buff->i_index;
        mvpb.u_flags = buff->i_flags&MVHE_FLAGS_CONTROL;
        mvpb.i_stamp = buff->i_timecode;
        mvpb.i_pitch = pitch;
        mvpb.planes[1].u_phys = 0;
        mvpb.planes[1].i_bias = 0;
        mvpb.planes[0].u_phys = buff->planes[0].mem.phys;
        mvpb.planes[0].i_bias = buff->planes[0].i_bias;
        if (mcfg.res.e_pixf <= MHVE_PIX_NV12)
        {
            mvpb.planes[1].u_phys = buff->planes[1].mem.phys;
            mvpb.planes[1].i_bias = buff->planes[1].i_bias;
        }

        mctx->m_mcpbs->i_index = -1;
        mctx->i_state = MVHE_CTX_STATE_BUSY;

        mops->enc_buff(mops, &mvpb);
        mops->put_data(mops, mctx->p_usrdt, mctx->i_usrsz);
        mops->enc_conf(mops);
        mvhedev_pushjob(mdev, mctx);
        mops->enc_done(mops);
        err =
        mops->out_buff(mops, mctx->m_mcpbs);

        mctx->i_ormdr = err;
        mctx->i_usrcn = mctx->i_usrsz = 0;
    }
    while (0);
    mutex_unlock(&mctx->m_stream);

    return err;
}

static int
_vhectx_get_bits(
    void*           pctx,
    mvhe_buff*      buff)
{
    mvhe_ctx* mctx = pctx;
    mhve_ops* mops = mctx->p_handle;
    int err = -ENODATA;

    if (buff->i_memory != MVHE_MEMORY_USER || buff->i_planes != 1)
        return -EINVAL;

    mutex_lock(&mctx->m_stream);
    if (MVHE_CTX_STATE_BUSY == mctx->i_state && MVHE_OMODE_USER == mctx->i_omode)
    {
        int flags = 0;
        int  rmdr = mctx->i_ormdr;
        int  size = mctx->m_mcpbs->planes[0].i_size;
        char* ptr = mctx->p_ovptr + size - rmdr;

        if (rmdr == size)
            flags = MVHE_FLAGS_SOP;
        if (buff->planes[0].i_size < rmdr)
            rmdr = buff->planes[0].i_size;
        buff->i_flags = 0;
        if (copy_to_user(buff->planes[0].mem.uptr, ptr, rmdr))
            err = -EFAULT;
        else
        {
            buff->planes[0].i_used = rmdr;
            buff->i_timecode = mctx->m_mcpbs->i_stamp;
            mctx->i_ormdr -= rmdr;
            err = 0;
        }
        if (0 == mctx->i_ormdr)
        {
            mctx->i_state = MVHE_CTX_STATE_IDLE;
            flags |= MVHE_FLAGS_EOP;
            mops->out_buff(mops, mctx->m_mcpbs);
        }
        if (!err)
            buff->i_flags = flags|(mctx->m_mcpbs->i_flags&MVHE_FLAGS_CONTROL);
    }
    mutex_unlock(&mctx->m_stream);

    return err;
}

extern int msys_find_dmem_by_phys(unsigned long long phys, msys_mem* pdmem);

static void*
phys2kptr(
    unsigned long long  phys)
{
    msys_mem dmem;
    char* kptr = NULL;
    if (0 == msys_find_dmem_by_phys(phys, &dmem))
    {
        kptr = (char*)(uintptr_t)dmem.kvirt;
        kptr += (phys - dmem.phys);
    }
    return kptr;
}

static int
_vhectx_compress(
    void*       pctx,
    mvhe_buff*  buff)
{
    int err = -EINVAL;
    int pitch = 0;
    mvhe_buff* out = buff + 1;
    mvhe_ctx* mctx = pctx;
    mvhe_dev* mdev = mctx->p_device;
    mhve_ops* mops = mctx->p_handle;

    if (buff->i_memory != MVHE_MEMORY_MMAP)
        return -EINVAL;
    if (out->i_memory != MVHE_MEMORY_MMAP || out->i_planes != 1)
        return -EINVAL;
    pitch = buff->i_stride;
    if (pitch < buff->i_width)
        pitch = buff->i_width;

    mutex_lock(&mctx->m_stream);
    do
    if (MVHE_CTX_STATE_IDLE == mctx->i_state && MVHE_OMODE_MMAP == mctx->i_omode)
    {
        uint  addr;
        void* kptr;
        mhve_cpb mcpb;
        mhve_vpb mvpb;
        mhve_cfg mcfg;

        mcfg.type = MHVE_CFG_RES;
        mops->get_conf(mops, &mcfg);
        if (buff->i_planes != 2 && (MHVE_PIX_NV21 >= mcfg.res.e_pixf))
            break;
        if (buff->i_planes != 1 && (MHVE_PIX_YUYV == mcfg.res.e_pixf))
            break;
        if (mcfg.res.i_pixw != buff->i_width || mcfg.res.i_pixh != buff->i_height)
            break;

        mcpb.i_index = -1;
        mvpb.i_index = buff->i_index;
        mvpb.i_stamp = buff->i_timecode;
        mvpb.u_flags = buff->i_flags&MVHE_FLAGS_CONTROL;
        mvpb.i_pitch = pitch;
        mvpb.planes[1].u_phys = 0;
        mvpb.planes[1].i_bias = 0;
        mvpb.planes[0].u_phys = buff->planes[0].mem.phys;
        mvpb.planes[0].i_bias = buff->planes[0].i_bias;
        if (mcfg.res.e_pixf <= MHVE_PIX_NV21)
        {
            mvpb.planes[1].u_phys = buff->planes[1].mem.phys;
            mvpb.planes[1].i_bias = buff->planes[1].i_bias;
        }

        addr = Chip_Phys_to_MIU(out->planes[0].mem.phys); 
        if (!(kptr = phys2kptr(out->planes[0].mem.phys)))
            break;
        mcfg.type = MHVE_CFG_DMA;
        mcfg.dma.i_dmem = -1;
        mcfg.dma.p_vptr = kptr;
        mcfg.dma.u_phys = addr;
        mcfg.dma.i_size[0] = out->planes[0].i_size;

        mctx->i_state = MVHE_CTX_STATE_BUSY;

        mops->set_conf(mops, &mcfg);
        mops->enc_buff(mops, &mvpb);
        mops->put_data(mops, mctx->p_usrdt, mctx->i_usrsz);
        mops->enc_conf(mops);
        mvhedev_pushjob(mdev, mctx);
        mops->enc_done(mops);
        mops->deq_buff(mops, &mvpb);
        mops->out_buff(mops, &mcpb);

        err =
        out->planes[0].i_used = mcpb.planes[0].i_size;
        out->i_timecode = mcpb.i_stamp;
        out->i_flags = mcpb.i_flags;

        buff->i_index = mvpb.i_index;

        mctx->i_usrsz = mctx->i_usrcn = 0;
        mctx->i_state = MVHE_CTX_STATE_IDLE;
    }
    while (0);
    mutex_unlock(&mctx->m_stream);

    return err;
}

static int
_vhectx_put_data(
    void*       pctx,
    mvhe_buff*  buff)
{
    int err = -EINVAL;
    mvhe_ctx* mctx = pctx;
    void* dst;

    if (buff->i_memory != MVHE_MEMORY_USER || buff->i_planes != 1)
        return err;
    if (buff->planes[0].mem.uptr == NULL ||
        buff->planes[0].i_size < buff->planes[0].i_used ||
        buff->planes[0].i_used > 1024)
        return err;

    mutex_lock(&mctx->m_stream);
    do
    if (MVHE_CTX_STATE_NULL != mctx->i_state && 4 > mctx->i_usrcn)
    {
        dst = mctx->p_usrdt + mctx->i_usrsz;
        err = -EFAULT;
        if (copy_from_user(dst, buff->planes[0].mem.uptr, buff->planes[0].i_used))
            break;
        mctx->i_usrsz += buff->planes[0].i_used;
        mctx->i_usrcn++;
        err = 0;
    }
    while (0);
    mutex_unlock(&mctx->m_stream);

    return err;
}

static int
_vhectx_set_rqcf(
    void*       pctx,
    rqct_conf*  rqcf)
{
    int err = -EINVAL;
    mvhe_ctx* mctx = pctx;
    mhve_ops* mops = mctx->p_handle;
    rqct_ops* rqct = mops->rqct_ops(mops);

    mutex_lock(&mctx->m_stream);
    if (!rqct->set_rqcf(rqct, (rqct_cfg*)rqcf))
        err = 0;
    mutex_unlock(&mctx->m_stream);

    return err;
}

static int
_vhectx_get_rqcf(
    void*       pctx,
    rqct_conf*  rqcf)
{
    int err = -EINVAL;
    mvhe_ctx* mctx = pctx;
    mhve_ops* mops = mctx->p_handle;
    rqct_ops* rqct = mops->rqct_ops(mops);

    mutex_lock(&mctx->m_stream);
    if (!rqct->get_rqcf(rqct, (rqct_cfg*)rqcf))
        err = 0;
    mutex_unlock(&mctx->m_stream);

    return err;
}
