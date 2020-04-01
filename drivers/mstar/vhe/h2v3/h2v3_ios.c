
#include <linux/delay.h>

#include <mhve_ios.h>

#include <h2v3_def.h>
#include <h2v3_reg.h>
#include <h2v3_ios.h>

static int  _set_bank(mhve_ios*, mhve_reg*);
static int  _enc_fire(mhve_ios*, mhve_job*);
static int  _enc_poll(mhve_ios*);
static int  _isr_func(mhve_ios*, int);
static int  _irq_mask(mhve_ios*, int);
static void _ios_free(void* p) { if (p) MEM_FREE(p); }

mhve_ios* mvheios_acquire(char* tags)
{
    h2v3_ios* asic = MEM_ALLC(sizeof(h2v3_ios));
    if (asic)
    {
        mhve_ios* mios = &asic->mios;
        mios->release  = _ios_free;
        mios->set_bank = _set_bank;
        mios->enc_fire = _enc_fire;
        mios->enc_poll = _enc_poll;
        mios->isr_func = _isr_func;
        mios->irq_mask = _irq_mask;
    }
    return (mhve_ios*)asic;
}

static int _set_bank(mhve_ios* mios, mhve_reg* mreg)
{
    h2v3_ios* asic = (h2v3_ios*)mios;
    switch (mreg->i_id)
    {
    case 0:
        asic->bank = mreg->base;
        break;
    case 1:
        asic->conf = mreg->base;
        break;
    default:
        return -1;
    }
    return 0;
}

#define REGW(r)             (*((volatile unsigned short*)(r)))
#define REGW_SAVE(r,i,v)    do { REGW(r+(2*i))=(uint16)(v); } while(0)
#define REGW_LOAD(r,i)      ({ uint16 v=REGW(r+(2*i)); (v); })

static void irq_clr(uint16* conf)
{
    uint16 w = REGW_LOAD(conf,0x04)|0xF00;
    REGW_SAVE(conf,0x04,w);
}

static void irq_msk(uint16* conf, uint16 mask)
{
    uint16 w = REGW_LOAD(conf,0x48)&~0xF;
    w = w | (0xF&mask);
    REGW_SAVE(conf,0x48,w);
}

#define REGL(r)             (*((volatile unsigned int*)(r)))
#define REGL_LO(v)          (((v)>> 0)&0xFFFF)
#define REGL_HI(v)          (((v)>>16)&0xFFFF)
#define REGL_SAVE(r,i,v)    do { REGL(r+(2*i)+0)=REGL_LO(v); REGL(r+(2*i)+1)=REGL_HI(v); } while(0)
#define REGL_LOAD(r,i)      ({ uint l=REGL(r+(2*i));uint h=REGL(r+(2*i)+1); ((h<<16)|l); })

static int _enc_fire(mhve_ios* mios, mhve_job* mjob)
{
    h2v3_ios* asic = (h2v3_ios*)mios;
    h2v3_mir* mirr = (h2v3_mir*)mjob;
    uint* base = asic->bank;
    uint* regs = mirr->regs;
    int i;
    asic->pjob = mirr;
    irq_clr(asic->conf);
    irq_msk(asic->conf, 0x0);
    /* flush registers */
    for (i = 1; i < H2V3_REG_SIZE; i++)
        REGL_SAVE(base,i,regs[i]);
    /* trigger encoder */
    REGL_SAVE(base,ASIC_ENC,regs[ASIC_ENC]|ASIC_ENC_TRIGGER);
    return 0;
}

static int _enc_poll(mhve_ios* mios)
{
    h2v3_ios* asic = (h2v3_ios*)mios;
    h2v3_mir* mirr = asic->pjob;
    uint* base = asic->bank;
    uint* regs = mirr->regs;
    uint size, tick, regv;
    int err = ASIC_SLICE_DONE;
    do
    {
        regv = REGL_LOAD(base,ASIC_STATUS);
        if (!(regv&ASIC_STATUS_ALL))
        {
            msleep(3);
            continue;
        }
        regv &= ~ASIC_STATUS_IRQ_LINE;
        REGL_SAVE(base,ASIC_STATUS,regv&~ASIC_STATUS_SLICE_DONE);
        if (regv & ASIC_STATUS_ERROR)
            err = ASIC_ERROR;
        else if (regv & ASIC_STATUS_HW_TIMEOUT)
            err = ASIC_ERR_HWTIMEOUT;
        else if (regv & ASIC_STATUS_FRAME_DONE)
            err = ASIC_FRAME_DONE;
        else if (regv & ASIC_STATUS_BUFF_FULL)
            err = ASIC_ERR_BUFF_FULL;
        else if (regv & ASIC_STATUS_HW_RESET)
            err = ASIC_ERR_HWTIMEOUT;
        else if (regv & ASIC_STATUS_FUSE)
            err = ASIC_ERROR;
        else
            err = ASIC_SLICE_DONE;
    }
    while (ASIC_SLICE_DONE == err);
    regv = REGL_LOAD(base,ASIC_ENC);
    REGL_SAVE(base,ASIC_ENC,regv&~ASIC_ENC_TRIGGER);
    regs[ASIC_OUTPUT_SIZE] = REGL_LOAD(base,ASIC_OUTPUT_SIZE);
    regs[ASIC_CYCLE_COUNT] = REGL_LOAD(base,ASIC_CYCLE_COUNT);
    H2GetRegVal(regs, HWIF_ENC_OUTPUT_STRM_BUFFER_LIMIT, &size);
    H2GetRegVal(regs, HWIF_ENC_HW_PERFORMANCE,           &tick);
    mirr->mjob.i_bits = size*8;
    mirr->mjob.i_tick = tick;
    asic->pjob = NULL;
    return 0;
}

static int _isr_func(mhve_ios* mios, int irq)
{
    h2v3_ios* asic = (h2v3_ios*)mios;
    h2v3_mir* mirr = asic->pjob;
    uint* base = asic->bank;
    uint* regs = mirr->regs;
    uint size, tick, bits = REGL_LOAD(base,ASIC_STATUS);
    if (!(bits&ASIC_STATUS_IRQ_LINE))
        return 0;
    REGL_SAVE(base,ASIC_STATUS,0);
    if (bits & ASIC_STATUS_FRAME_DONE)
    {
        regs[ASIC_OUTPUT_SIZE] = REGL_LOAD(base,ASIC_OUTPUT_SIZE);
        regs[ASIC_CYCLE_COUNT] = REGL_LOAD(base,ASIC_CYCLE_COUNT);
        H2GetRegVal(regs, HWIF_ENC_OUTPUT_STRM_BUFFER_LIMIT, &size);
        H2GetRegVal(regs, HWIF_ENC_HW_PERFORMANCE,           &tick);
        mirr->mjob.i_bits = size*8;
        mirr->mjob.i_tick = tick;
        irq_clr(asic->conf);
        irq_msk(asic->conf, 0xF);
        asic->pjob = NULL;
        return 0;
    }
    if (bits & ASIC_STATUS_BUFF_FULL)
        return -2;
    return -1;
}

static int _irq_mask(mhve_ios* mios, int msk)
{
    h2v3_ios* asic = (h2v3_ios*)mios;
    irq_clr(asic->conf);
    irq_msk(asic->conf, 0xF);
    return 0;
}
