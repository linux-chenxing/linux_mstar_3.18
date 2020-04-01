
#ifndef _H2V3_IOS_H_
#define _H2V3_IOS_H_

#include <mhve_ios.h>

typedef struct h2v3_mir h2v3_mir;
typedef struct h2v3_ios h2v3_ios;

#define H2V3_REG_SIZE   (165)

struct h2v3_mir {
    mhve_job    mjob;
    uint        regs[H2V3_REG_SIZE];
};

struct h2v3_ios {
    mhve_ios    mios;
    void*       bank;
    void*       conf;
    void*       pjob;
};

/* error code */
#define ASIC_SLICE_DONE         2
#define ASIC_FRAME_DONE         1
#define ASIC_OKAY               0
#define ASIC_ERROR             -1
#define ASIC_ERR_HWTIMEOUT     -2
#define ASIC_ERR_HWRESET       -3
#define ASIC_ERR_BUFF_FULL     -4

/* specific registers */
/* trigger controller register */
#define ASIC_ENC                5
#define ASIC_ENC_TRIGGER        0x001
/* status report register */
#define ASIC_STATUS             1
#define ASIC_STATUS_ALL         0x3FD
#define ASIC_STATUS_FUSE        0x200
#define ASIC_STATUS_SLICE_DONE  0x100
#define ASIC_STATUS_HW_TIMEOUT  0x040
#define ASIC_STATUS_BUFF_FULL   0x020
#define ASIC_STATUS_HW_RESET    0x010
#define ASIC_STATUS_ERROR       0x008
#define ASIC_STATUS_FRAME_DONE  0x004
#define ASIC_STATUS_IRQ_LINE    0x001
/* slice-number report register */
#define ASIC_SLICE              7
#define ASIC_SLICE_NR(reg)      (((reg)>>17)&0xFF)
/* tcnt/cycles report register */
#define ASIC_OUTPUT_SIZE        9
#define ASIC_CYCLE_COUNT        82

#endif/*_H2V3_IOS_H*/
