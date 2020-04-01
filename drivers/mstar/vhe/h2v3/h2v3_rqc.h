
#ifndef _H2V3_RQC_H_
#define _H2V3_RQC_H_

typedef struct h2v3_rqc h2v3_rqc;

struct h2v3_rqc {
    rqct_ops    rqct;
    rqct_att    attr;

    int         i_period;
    int         i_pcount;
    int         i_config;
};

void* rqctvhe_acquire(int);
char* rqctvhe_comment(int);

int hrqc_set_conf(rqct_ops* rqct, rqct_cfg* conf);
int hrqc_get_conf(rqct_ops* rqct, rqct_cfg* conf);
int hrqc_enc_buff(rqct_ops* rqct, rqct_buf* buff);
int hrqc_enc_conf(rqct_ops* rqct, mhve_job* mjob);

#endif/*_H2V3_RQC_H_*/
