#ifndef _H2V3_OPS_H_
#define _H2V3_OPS_H_

typedef struct rpb_t rpb_t;

struct rpb_t {
    short   b_valid;
    short   i_index;
#define RPB_YPIXEL  0
#define RPB_CPIXEL  1
#define RPB_LUMA4N  2
#define RPB_YCOMPR  3
#define RPB_CCOMPR  4
#define RPB_PLANES  5
    uint    u_planes[RPB_PLANES];
};

#define MAX_RPB_NR  4

typedef struct h2v3_ops h2v3_ops;

struct h2v3_ops {
    mhve_ops    mops;
    rqct_ops*   rqct;
    h2v3_mir*   mirr;
    h265_enc*   h265;
    /* current resource */
    mhve_pix_e  e_pixf;
    short       i_pixw, i_pixh;
    int         i_rpbn;
    uint        u_conf;
    /* video-picture-buffer */
    mhve_vpb    m_encp;
    int         i_seqn;     /* seq_cnt: 0->force idr, others: ... */
    int         b_seqh;
    /* output bitstream */
    void*       p_otpt;
    uint        u_otbs;
    uint        u_otsz;
    uint        u_otrm;
    uint        u_used;
    uint        u_size;
    /* other attributes */
    uint*       p_nalp;
    uint        u_nalb;
    uint        u_coef;
    rpb_t       m_rpbs[MAX_RPB_NR];
};

#endif/*_H2V3_OPS_H_*/
