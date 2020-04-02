#ifndef _MSTAR_IR_H_
#define _MSTAR_IR_H_

#define   MSTAR_TIMER2_DRV_NAME "timer2" 

#define   mstar_pm_base          0xFD000000
#define   RIU_REG_BASE           mstar_pm_base

#define   IRQ_BANK               0x2B	
#define   FIQ_MASK_ADDR          0x04
#define   FIQ_STATUS_ADDR        0x0C

#define   TIMER2_BANK            0x30
#define   TIMER2_EN_ADDR         0x30
#define   TIMER2_MAX_ADDR_LO     0x32
#define   TIMER2_MAX_ADDR_HI     0x33

#define   MSTAR_TIMER2_IOC_MAGIC           ('T')
#define   MSTAR_TIMER2_IOC_SET_MAX_VALUE   _IOW(MSTAR_TIMER2_IOC_MAGIC,   0, U32)

#define   TIMER2_MAX_VALUE       12000000

#define E_IRQ_DISABLE			(0)
#define E_IRQ_ENABLE			(1)
#define E_IRQ_ACK			    (2)

struct timer2_st {
	wait_queue_head_t q;
	atomic_t count;
	char devname[TASK_COMM_LEN];
};

#endif

