
#ifndef _MHVE_OPS_H_
#define _MHVE_OPS_H_

typedef struct mhve_ops mhve_ops;
typedef struct mhve_vpb mhve_vpb;
typedef struct mhve_cpb mhve_cpb;
typedef union  mhve_cfg mhve_cfg;

typedef signed long long   int64;
typedef unsigned int       uint;

#define _MAX(a,b)   ((a)>(b)?(a):(b))
#define _MIN(a,b)   ((a)<(b)?(a):(b))

struct mhve_vpb { // video picture buffer
    int     i_index;
    uint    u_flags;
    int64   i_stamp;
    int     i_pitch;
    struct {
    uint    u_phys;
    int     i_bias;
    int     i_size;
    } planes[2];
    /* ISP informations */
};

struct mhve_cpb { // coded picture buffer
    int     i_index;
    int     i_flags;
    int64   i_stamp;
    int     i_count;
    struct {
    uint    u_phys;
    int     i_size;
    } planes[2];
};

typedef enum mhve_pix_e {
    MHVE_PIX_NV12=0,
    MHVE_PIX_NV21,
    MHVE_PIX_YUYV,
    MHVE_PIX_YVYU,
} mhve_pix_e;

union  mhve_cfg {
    enum mhve_cfg_e {
        MHVE_CFG_RES=0,
        MHVE_CFG_DMA,
        MHVE_CFG_MOT,
        MHVE_CFG_AVC,
        MHVE_CFG_HEV,
        MHVE_CFG_VUI,
        MHVE_CFG_LFT,
        MHVE_CFG_SPL,
        MHVE_CFG_BAC,
    } type;
    struct {
    enum mhve_cfg_e i_type;
    mhve_pix_e  e_pixf;
    short       i_pixw, i_pixh;
    int         i_rpbn;
#define MHVE_CFG_COMPR  (1<<0)
#define MHVE_CFG_OMMAP  (1<<1)
    uint        u_conf;
    } res;
    struct {
    enum mhve_cfg_e i_type;
    int         i_dmem;
    void*       p_vptr;
    uint        u_phys;
    int         i_size[6];
    } dma;
    struct {
    enum mhve_cfg_e i_type;
    int         i_subp;
    short       i_dmvx;
    short       i_dmvy;
    uint        i_blkp[2];
    } mot;
    struct {
    enum mhve_cfg_e i_type;
    unsigned short  i_profile;                  //!< profile.
    unsigned short  i_level;                    //!< level.
    unsigned char   i_num_ref_frames;           //!< ref.frames count.
    unsigned char   i_poc_type;                 //!< poc_type: support 0,2.
    unsigned char   b_entropy_coding_type;      //!< entropy: cabac/cavlc.
    unsigned char   b_constrained_intra_pred;   //!< contrained intra pred.
    unsigned char   b_deblock_filter_control;   //!< deblock filter control.
    unsigned char   i_disable_deblocking_idc;   //!< disable deblocking idc.
      signed char   i_alpha_c0_offset;          //!< offset alpha div2.
      signed char   i_beta_offset;              //!< offset beta div2.
    } avc;
    struct {
    enum mhve_cfg_e i_type;
    unsigned short  i_profile;                      //!< profile.
    unsigned short  i_level;                        //!< level.
    unsigned char   i_log2_max_cb_size;             //!< max ctb size.
    unsigned char   i_log2_min_cb_size;             //!< min ctb size.
    unsigned char   i_log2_max_tr_size;             //!< max trb size.
    unsigned char   i_log2_min_tr_size;             //!< min trb size.
    unsigned char   i_tr_depth_intra;               //!< tr depth intra.
    unsigned char   i_tr_depth_inter;               //!< tr depth inter.
    unsigned char   b_scaling_list_enable;          //!< scaling list enable.
    unsigned char   b_sao_enable;                   //!< sao enable.
    unsigned char   b_strong_intra_smoothing;       //!< strong intra smoothing.
    unsigned char   b_ctu_qp_delta_enable;          //!< ctu qp delta enable.
    unsigned char   b_constrained_intra_pred;       //!< constrained intra prediction.
    unsigned char   b_deblocking_override_enable;   //!< deblocking override enable.
      signed char   i_cqp_offset;                   //!< cqp offset: -12 to 12 (inclusive)
    unsigned char   b_deblocking_disable;           //!< deblocking disable.
      signed char   i_tc_offset_div2;               //!< tc_offset_div2: -6 to 6 (inclusive)
      signed char   i_beta_offset_div2;             //!< beta_offset_div2: -6 to 6 (inclusive)
    } hev;
    struct {
    enum mhve_cfg_e i_type;
    unsigned char   b_video_full_range;
    } vui;
    struct {
    enum mhve_cfg_e i_type;
      signed char   b_override;
      signed char   b_disable;
      signed char   i_offsetA;
      signed char   i_offsetB;
    } lft;
    struct {
    enum mhve_cfg_e i_type;
    short       i_rows;
    short       i_bits;
    } spl;
    struct {
    enum mhve_cfg_e i_type;
    int         b_init;
    } bac;
};

struct mhve_ops {
    char   mhvename[16];
    void  (*release) (mhve_ops*);
    void*(*rqct_ops) (mhve_ops*);
    void*(*mhve_job) (mhve_ops*);
    int  (*seq_sync) (mhve_ops*);
    int  (*seq_conf) (mhve_ops*);
    int  (*seq_done) (mhve_ops*);
    int  (*enc_buff) (mhve_ops*, mhve_vpb*);
    int  (*deq_buff) (mhve_ops*, mhve_vpb*);
    int  (*enc_conf) (mhve_ops*);
    int  (*put_data) (mhve_ops*, void*,int);
    int  (*enc_done) (mhve_ops*);
    int  (*enc_undo) (mhve_ops*);
    int  (*out_buff) (mhve_ops*, mhve_cpb*);
    int  (*set_conf) (mhve_ops*, mhve_cfg*);
    int  (*get_conf) (mhve_ops*, mhve_cfg*);
};

#endif/*_MHVE_OPS_H_*/
