
#ifndef _RQCT_CFG_H_
#define _RQCT_CFG_H_

#include <mdrv_rqct_st.h>

typedef union  rqct_cfg rqct_cfg;
typedef struct rqct_buf rqct_buf;

union  rqct_cfg {
    enum rqct_cfg_e {
    RQCT_CFG_SEQ=RQCT_CONF_SEQ,
    RQCT_CFG_DQP=RQCT_CONF_DQP,
    RQCT_CFG_QPR=RQCT_CONF_QPR,
    RQCT_CFG_LOG=RQCT_CONF_LOG,
    RQCT_CFG_RES=32,
    RQCT_CFG_FPS,
    RQCT_CFG_ROI,
    RQCT_CFG_DQM,
    } type;
    struct _seq seq;
    struct _dqp dqp;
    struct _qpr qpr;
    struct _log log;
    struct _res {
    enum rqct_cfg_e i_type; //!< MUST BE "RQCT_CFG_RES"
    short i_picw;
    short i_pich;
    } res;
    struct _fps {
    enum rqct_cfg_e i_type; //!< MUST BE "RQCT_CFG_FPS"
    short n_fps;
    short d_fps;
    } fps;
    struct _roi {
    enum rqct_cfg_e i_type; //!< MUST BE "RQCT_CFG_ROI"
    short i_roiidx;
    short i_roidqp;
    short i_posx;
    short i_posy;
    short i_recw;
    short i_rech;
    } roi;
    struct _dqm {
    enum rqct_cfg_e i_type; //!< MUST BE "RQCT_CFG_DQM"
    unsigned int u_phys;
    void*        p_kptr;
    short i_dqmw;
    short i_dqmh;
    int i_size;
    } dqm;
};

struct  rqct_buf {
    unsigned int u_config;
};

#endif//_RQCT_CFG_H_
