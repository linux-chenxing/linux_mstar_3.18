
// mstar hardware video encoder I/F (mhveif)

#ifndef _MHVE_IOS_H_
#define _MHVE_IOS_H_

typedef struct mhve_ios mhve_ios;
typedef struct mhve_reg mhve_reg;
typedef struct mhve_job mhve_job;

struct mhve_ios {
    void     (*release)(void*);
    int     (*set_bank)(mhve_ios*, mhve_reg*);
    int     (*enc_fire)(mhve_ios*, mhve_job*);
    int     (*enc_poll)(mhve_ios*);
    int     (*isr_func)(mhve_ios*, int);
    int     (*irq_mask)(mhve_ios*, int);
};

struct mhve_reg {
    int     i_id;
    void*   base;
    int     size;
};

struct mhve_job {
    int     i_code;
    int     i_tick;
    int     i_bits;
};

#endif//_MHVE_IOS_H_
