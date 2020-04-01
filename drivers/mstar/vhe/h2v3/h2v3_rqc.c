
#include <mhve_ops.h>
#include <mhve_ios.h>
#include <rqct_ops.h>
#include <h2v3_def.h>
#include <h2v3_rqc.h>

#define HRQC_NAME       "HRQC"
#define HRQC_VER_MJR    0
#define HRQC_VER_MNR    0
#define HRQC_VER_EXT    00

static void* hrqc_allocate(void);
static char* hrqc_describe(void);

#define HRQC_FACTORIES_NR   8

static struct hrqc_factory {
    void* (*rqc_alloc)(void);
    char* (*rqc_descr)(void);
}
factories[HRQC_FACTORIES_NR] =
{
{hrqc_allocate,hrqc_describe},
{NULL,NULL},
};

char* rqctvhe_comment(int idx)
{
    if ((unsigned)idx < HRQC_FACTORIES_NR && factories[idx].rqc_descr)
        return (factories[idx].rqc_descr)();
    return NULL;
}

void* rqctvhe_acquire(int idx)
{
    if ((unsigned)idx < HRQC_FACTORIES_NR && factories[idx].rqc_alloc)
        return (factories[idx].rqc_alloc)();
    return (factories[0].rqc_alloc)();
}

static int  _seq_init(rqct_ops*);
static int  _seq_done(rqct_ops*);
static int  _seq_conf(rqct_ops*);
static int  _enc_done(rqct_ops* rqct, mhve_job* mjob);
static void _hrq_free(rqct_ops* rqct) { MEM_FREE(rqct); }

static void* hrqc_allocate(void)
{
    rqct_ops* rqct = NULL;
    h2v3_rqc* rqcx;
    if (!(rqct = MEM_ALLC(sizeof(h2v3_rqc))))
        return rqcx;
    MEM_COPY(rqct->name, "vrqc", 5);
    rqct->release =  _hrq_free;
    rqct->seq_sync = _seq_init;
    rqct->seq_done = _seq_done;
    rqct->seq_conf = _seq_conf;
    rqct->set_rqcf = hrqc_set_conf;
    rqct->get_rqcf = hrqc_get_conf;
    rqct->enc_buff = hrqc_enc_buff;
    rqct->enc_conf = hrqc_enc_conf;
    rqct->enc_done = _enc_done;
    rqct->i_enc_nr = 0;
    rqct->i_enc_bs = 0;
    rqcx = (h2v3_rqc*)rqct;
    rqcx->attr.i_method = RQCT_METHOD_CQP;
    rqcx->attr.i_pict_w = 0;
    rqcx->attr.i_pict_h = 0;
    rqcx->attr.n_fmrate =30;
    rqcx->attr.d_fmrate = 1;
    rqcx->attr.i_leadqp =-1;
    rqcx->attr.i_deltaq = 3;
    rqcx->attr.i_btrate = 0;
    rqcx->attr.b_logoff = 1;
    rqcx->attr.b_dqmstat=-1;
    rqcx->i_config = 0;
    rqcx->i_period = 0;
    rqcx->i_pcount = 0;
    return rqct;
}

static char* hrqc_describe(void)
{
    static char line[64];
    sprintf(line, "%s@v%d.%d-%02d:basic rq-controller.",HRQC_NAME,HRQC_VER_MJR,HRQC_VER_MNR,HRQC_VER_EXT);
    return line;
}

static int _seq_init(rqct_ops* rqct)
{
    h2v3_rqc* rqcx = (h2v3_rqc*)rqct;
    rqcx->i_pcount = 0;
    return 0;
}

static int _seq_done(rqct_ops* rqct)
{
    return 0;
}

static int _seq_conf(rqct_ops* rqct)
{
    h2v3_rqc* rqcx = (h2v3_rqc*)rqct;
    if (rqcx->attr.i_leadqp < 0)
        rqcx->attr.i_leadqp =36;
    rqcx->i_pcount =
    rqcx->i_period = rqcx->attr.i_period;
    return 0;
}

static int _enc_done(rqct_ops* rqct, mhve_job* mjob)
{
    h2v3_rqc* rqcx = (h2v3_rqc*)rqct;
    rqct->i_bitcnt = mjob->i_bits;
    rqct->i_enc_bs+= mjob->i_bits/8;
    rqct->i_enc_nr++;
    if (!rqcx->attr.b_logoff)
        snprintf(rqct->print_line,RQCT_LINE_SIZE-1,"<%s#%04d@%c:%2d> bitcnt:%8d",\
                 rqct->name,rqct->i_enc_nr,IS_IPIC(rqct->i_pictyp)?'I':'P',rqct->i_enc_qp,rqct->i_bitcnt);
    return 0;
}

int hrqc_set_conf(rqct_ops* rqct, rqct_cfg* conf)
{
    h2v3_rqc* rqcx = (h2v3_rqc*)rqct;
    int err = -1;
    switch (conf->type)
    {
    case RQCT_CFG_SEQ:
        if ((unsigned)conf->seq.i_method > RQCT_METHOD_VBR)
            break;
        rqcx->attr.i_method = conf->seq.i_method;
        rqcx->attr.i_period = conf->seq.i_period;
        rqcx->attr.i_btrate = conf->seq.i_btrate;
        rqcx->attr.i_leadqp = conf->seq.i_leadqp;
        rqcx->i_pcount = 0;
        err = 0;
        break;
    case RQCT_CFG_DQP:
        if ((unsigned)conf->dqp.i_dqp > 8)
            break;
        rqcx->attr.i_deltaq = conf->dqp.i_dqp;
        err = 0;
        break;
    case RQCT_CFG_QPR:
        if ((unsigned)conf->qpr.i_upperq > 51 ||
            (unsigned)conf->qpr.i_lowerq > 51 ||
            conf->qpr.i_lowerq > conf->qpr.i_upperq)
            break;
        rqcx->attr.i_upperq = conf->qpr.i_upperq;
        rqcx->attr.i_lowerq = conf->qpr.i_lowerq;
        err = 0;
        break;
    case RQCT_CFG_RES:
        rqcx->attr.i_pict_w = (short)conf->res.i_picw;
        rqcx->attr.i_pict_h = (short)conf->res.i_pich;
        err = 0;
        break;
    case RQCT_CFG_FPS:
        rqcx->attr.n_fmrate = (short)conf->fps.n_fps;
        rqcx->attr.d_fmrate = (short)conf->fps.d_fps;
        err = 0;
        break;
    default:
        break;
    }
    return err;
}

int hrqc_get_conf(rqct_ops* rqct, rqct_cfg* conf)
{
    h2v3_rqc* rqcx = (h2v3_rqc*)rqct;
    int err = -1;
    switch (conf->type)
    {
    case RQCT_CFG_SEQ:
        conf->seq.i_method = rqcx->attr.i_method;
        conf->seq.i_period = rqcx->attr.i_period;
        conf->seq.i_btrate = rqcx->attr.i_btrate;
        conf->seq.i_leadqp = rqcx->attr.i_leadqp;
        err = 0;
        break;
    case RQCT_CFG_DQP:
        conf->dqp.i_dqp = rqcx->attr.i_deltaq;
        err = 0;
        break;
    case RQCT_CFG_QPR:
        conf->qpr.i_upperq = rqcx->attr.i_upperq;
        conf->qpr.i_lowerq = rqcx->attr.i_lowerq;
        err = 0;
        break;
    case RQCT_CFG_RES:
        conf->res.i_picw = (int)rqcx->attr.i_pict_w;
        conf->res.i_pich = (int)rqcx->attr.i_pict_h;
        err = 0;
        break;
    case RQCT_CFG_FPS:
        conf->fps.n_fps = (int)rqcx->attr.n_fmrate;
        conf->fps.d_fps = (int)rqcx->attr.d_fmrate;
        err = 0;
        break;
    default:
        break;
    }
    return err;
}

int hrqc_enc_buff(rqct_ops* rqct, rqct_buf* buff)
{
    h2v3_rqc* rqcx = (h2v3_rqc*)rqct;
    if (rqcx->i_pcount == 0)
        rqct->seq_conf(rqct);
    /* picture-type decision */
    if (rqcx->i_pcount == rqcx->i_period)
    {
        rqcx->i_pcount = 1;
        rqct->i_pictyp = RQCT_PICTYP_I;
        rqct->i_enc_qp = rqcx->attr.i_leadqp - rqcx->attr.i_deltaq;
        rqct->b_unrefp = 0;
    }
    else
    {
        rqcx->i_pcount++;
        rqct->i_pictyp = RQCT_PICTYP_P;
        rqct->i_enc_qp = rqcx->attr.i_leadqp;
        if (rqct->b_unrefp)
            rqct->b_unrefp = !(rqcx->i_pcount%2);
    }
    return 0;
}

int hrqc_enc_conf(rqct_ops* rqct, mhve_job* mjob)
{
    return 0;
}
