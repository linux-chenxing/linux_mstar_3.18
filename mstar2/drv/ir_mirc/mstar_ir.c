#include <linux/autoconf.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/poll.h>
#include <linux/wait.h>
#include <linux/time.h>  //added
#include <linux/timer.h> //added
#include <linux/types.h> //added
#include <linux/input.h>
#include <linux/spinlock.h>
#include <linux/semaphore.h>
#include <linux/platform_device.h>
#include <asm/io.h>
#include "mst_devid.h"
#include "mstar_ir.h"
#include "ir_core.h"
#include "ir_common.h"
#include "chip_int.h"
#include "mhal_ir_reg.h"
#include "mdrv_ir_st.h"
#include "mdrv_ir_io.h"
#include "ir_config.h"
//-------------------------------------------------------------------------------------------------
#define REG(addr)                   (*(volatile u32 *)(addr))


#define MSTAR_IR_DEVICE_NAME    "MStar Smart TV IR Receiver"
#define MSTAR_IR_DRIVER_NAME    "MStar_IR"
#define MOD_IR_DEVICE_COUNT     1

#ifdef CONFIG_MIRC_INPUT_DEVICE
#define INPUTID_VENDOR 0x3697UL;
#define INPUTID_PRODUCT 0x0001;
#define INPUTID_VERSION 0x0001;
#endif



/*******************  Global Variable  Start ******************/
static u8 ir_irq_enable = 0xFF; //for record irq enable status
static u8 ir_irq_sel = 0xFF;   //for record irq type (IR_RC or IR_ALL)
static pid_t MasterPid = 0;
static IR_Mode_e ir_config_mode = IR_TYPE_MAX_MODE;
//for fulldecode ir headcode config
u32 u32IRHeaderCode[2]={IR_DEFAULT_CUSTOMER_CODE0,IR_DEFAULT_CUSTOMER_CODE1};
//for record repeat times
static u8 u8RepeatCount = 0;
//for record get ir key time
static u32 u32KeyTime = 0;
static spinlock_t  irq_read_lock; 


/*******************  Global Variable  End ******************/

static int  _mod_ir_open (struct inode *inode, struct file *filp);
static int  _mod_ir_release(struct inode *inode, struct file *filp);
static ssize_t _mod_ir_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
static ssize_t _mod_ir_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);
static unsigned int _mod_ir_poll(struct file *filp, poll_table *wait);
#ifdef HAVE_UNLOCKED_IOCTL
static long _mod_ir_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
#else
static int _mod_ir_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);
#endif
static int                      _mod_ir_fasync(int fd, struct file *filp, int mode);

IRModHandle IRDev=
{
    .s32IRMajor=               MDRV_MAJOR_IR,
    .s32IRMinor=               MDRV_MINOR_IR,
    .cDevice=
    {
        .kobj=                  {.name= MDRV_NAME_IR, },
        .owner  =               THIS_MODULE,
    },
    .IRFop=
    {
        .open=                  _mod_ir_open,
        .release=               _mod_ir_release,
        .read=                  _mod_ir_read,
        .write=                 _mod_ir_write,
        .poll=                  _mod_ir_poll,
#ifdef HAVE_UNLOCKED_IOCTL
        .unlocked_ioctl =       _mod_ir_ioctl,
#else
        .ioctl =                _mod_ir_ioctl,
#endif
        .fasync =               _mod_ir_fasync,
    },
};

static void _Mdrv_IR_SetHeaderCode(u32 u32Headcode)
{
    IRModHandle* mstar_dev = &IRDev;
    u8 i = 0;
    if (mstar_dev->pIRDev->ir_mode == IR_TYPE_FULLDECODE_MODE)
    {
        u32IRHeaderCode[0] = u32Headcode;
        //set register for full mode decoder
        REG(REG_IR_CCODE) = ((u32IRHeaderCode[0]&0xff)<<8)|((u32IRHeaderCode[0]>>8) &0xff);
    }

    for(i = 0;i< mstar_dev->pIRDev->support_num;i++)
    {
        if(mstar_dev->pIRDev->support_ir[i].eIRType == IR_TYPE_NEC)
        {
            mstar_dev->pIRDev->support_ir[i].u32HeadCode = u32Headcode;
            return ;
        }

    }
    IRDBG_WRN("Warnning: ioctl: MDRV_IR_SET_HEADER only support NEC type,please check\n");
    return ;
}

/*=======================IR  Setting Functions========================*/

static void mstar_ir_nec_clearfifo(void)
{
    unsigned long i;
    for (i=0; i<8; i++)
    {
        unsigned char u8Garbage;

        if (REG(REG_IR_SHOT_CNT_H_FIFO_STATUS) & IR_FIFO_EMPTY)
            break;

        u8Garbage = REG(REG_IR_CKDIV_NUM_KEY_DATA) >> 8;
        REG(REG_IR_FIFO_RD_PULSE) |= 0x0001UL; //read
    }
    return;
}
static void mstar_ir_rc_clearfifo(void)
{
    unsigned long i;
    for (i=0; i<8; i++)
    {
        unsigned char u8Garbage;

        if (REG(REG_IR_RC_KEY_FIFO_STATUS) & IR_RC_FIFO_EMPTY)
            break;

        u8Garbage = REG(REG_IR_RC_KEY_COMMAND_ADD) >> 8;
        REG(REG_IR_RC_FIFO_RD_PULSE) |= 0x0001UL; //read
    }
    return ;
}
static void _MDrv_IR_Timing(void)
{
    // header code upper bound
    REG(REG_IR_HDC_UPB) = IR_HDC_UPB;

    // header code lower bound
    REG(REG_IR_HDC_LOB) = IR_HDC_LOB;

    // off code upper bound
    REG(REG_IR_OFC_UPB) = IR_OFC_UPB;

    // off code lower bound
    REG(REG_IR_OFC_LOB) = IR_OFC_LOB;

    // off code repeat upper bound
    REG(REG_IR_OFC_RP_UPB) = IR_OFC_RP_UPB;

    // off code repeat lower bound
    REG(REG_IR_OFC_RP_LOB) = IR_OFC_RP_LOB;

    // logical 0/1 high upper bound
    REG(REG_IR_LG01H_UPB) = IR_LG01H_UPB;

    // logical 0/1 high lower bound
    REG(REG_IR_LG01H_LOB) = IR_LG01H_LOB;

    // logical 0 upper bound
    REG(REG_IR_LG0_UPB) = IR_LG0_UPB;

    // logical 0 lower bound
    REG(REG_IR_LG0_LOB) = IR_LG0_LOB;

    // logical 1 upper bound
    REG(REG_IR_LG1_UPB) = IR_LG1_UPB;

    // logical 1 lower bound
    REG(REG_IR_LG1_LOB) = IR_LG1_LOB;

    // timeout cycles
    REG(REG_IR_TIMEOUT_CYC_L) = IR_RP_TIMEOUT & 0xFFFFUL;
}

static void mstar_ir_hw_fulldecode_config(void)
{
    //1. set customer code0
    REG(REG_IR_CCODE) = ((u32IRHeaderCode[0]&0xff)<<8)|((u32IRHeaderCode[0]>>8) &0xff);
    //2. set customer code1
    REG(REG_IR_CCODE1) =((u32IRHeaderCode[1]&0xff)<<8)|((u32IRHeaderCode[1]>>8) &0xff);

    if(ir_config_mode == IR_TYPE_FULLDECODE_MODE)
    {
        return ;
    }
    ir_config_mode = IR_TYPE_FULLDECODE_MODE;
    IR_PRINT("##### Mstar IR HW Full Decode Config #####\n");
    //3. set NEC timing & FIFO depth 16 bytes
    _MDrv_IR_Timing();
    REG(REG_IR_SEPR_BIT_FIFO_CTRL) = 0xF00UL;//[10:8]: FIFO depth, [11]:Enable FIFO full
    REG(REG_IR_CCODE1_CHK_EN) |= IR_CCODE1_CHK_EN;
    //4. Glitch Remove (for prevent Abnormal waves)  reg_ir_glhrm_num = 4£¬reg_ir_glhrm_en = 1
    REG(REG_IR_GLHRM_NUM) = 0x804UL;
    //5.set full decode mode
    REG(REG_IR_GLHRM_NUM) |= (0x3UL <<12);
    //6.set Data bits ===>two byte customer code +IR DATA bits(32bits)+timeout clear+timeout value 4 high bits
    REG(REG_IR_TIMEOUT_CYC_H_CODE_BYTE) = IR_CCB_CB | 0x30UL | ((IR_RP_TIMEOUT >> 16) & 0x0FUL);
    //7.set IR clock DIV
    REG(REG_IR_CKDIV_NUM_KEY_DATA) = IR_CKDIV_NUM;   //set clock div --->IR clock =1MHZ
    //8.reg_ir_wkup_key_sel ---> out key value
    REG(REG_IR_FIFO_RD_PULSE) |= 0x0020UL; //wakeup key sel
    //9. set ctrl enable IR
    REG(REG_IR_CTRL) = IR_TIMEOUT_CHK_EN |
                       IR_INV            |
                       IR_RPCODE_EN      |
                       IR_LG01H_CHK_EN   |
                       IR_DCODE_PCHK_EN  |
                       IR_CCODE_CHK_EN   |
                       IR_LDCCHK_EN      |
                       IR_EN;
}

int mstar_ir_isr_getdata_fulldecode(IRModHandle *mstar_dev)
{
    int ret = 0;
    u8 u8Ir_Index = 0;
    u8 u8Keycode = 0;
    u8 u8Repeat = 0;
    DEFINE_IR_RAW_DATA(ev);
    u8Keycode = REG(REG_IR_CKDIV_NUM_KEY_DATA) >> 8;
    u8Repeat = (REG(REG_IR_SHOT_CNT_H_FIFO_STATUS) & IR_RPT_FLAG)? 1 : 0;
    u8Ir_Index = REG(REG_IR_SHOT_CNT_H_FIFO_STATUS) & 0x40? 1 : 0;//IR remote controller TX1 send flag
    REG(REG_IR_FIFO_RD_PULSE) |= 0x0001;

    ev.duration = (u32IRHeaderCode[u8Ir_Index]<<8) | u8Keycode;
    ev.pulse = u8Repeat;
    printk(" u8Ir_Index = %d\n",u8Ir_Index);
    IRDBG_INFO("[Full Decode] Headcode =%x Key =%x\n",u32IRHeaderCode[u8Ir_Index],u8Keycode);
    mstar_dev->pIRDev->map_num = mstar_dev->pIRDev->support_ir[u8Ir_Index].u32HeadCode;

    if(u8Repeat)
    {
        if(u8RepeatCount == 0)//cut first repeat
        {
            u8RepeatCount ++;
            mstar_ir_nec_clearfifo();
            return -1;
        }
    }
    else
    {
        u8RepeatCount = 0;
    }

    ret = MIRC_Data_Store(mstar_dev->pIRDev, &ev);
    if (ret < 0)
    {
        IRDBG_ERR("Store IR data Error!\n");
    }
    mstar_ir_nec_clearfifo();
    return ret;
}

static void mstar_ir_hw_rawdecode_config(void)
{
    if(ir_config_mode == IR_TYPE_RAWDATA_MODE)
        return ;
    ir_config_mode = IR_TYPE_RAWDATA_MODE;
    IR_PRINT("##### Mstar IR HW RAW Decode Config #####\n");
    //1. set NEC timing  & FIFO depth 16 bytes
    _MDrv_IR_Timing();
    REG(REG_IR_SEPR_BIT_FIFO_CTRL) = 0xF00UL;//[10:8]: FIFO depth, [11]:Enable FIFO full
    //2. Glitch Remove (for prevent Abnormal waves)  reg_ir_glhrm_num = 4£¬reg_ir_glhrm_en = 1
    REG(REG_IR_GLHRM_NUM) = 0x804UL;
    //3.set RAW decode mode
    REG(REG_IR_GLHRM_NUM) |= (0x2UL <<12);
    //4.reg_ir_wkup_key_sel ---> out key value
    REG(REG_IR_FIFO_RD_PULSE) |= 0x0020UL; //wakeup key sel
    //5.set IR clock DIV
    REG(REG_IR_CKDIV_NUM_KEY_DATA) = IR_CKDIV_NUM;   //set clock div --->IR clock =1MHZ
    //6.set Data bits ===>two byte customer code +IR DATA bits(32bits)+timeout clear+timeout value 4 high bits
    REG(REG_IR_TIMEOUT_CYC_H_CODE_BYTE) = IR_CCB_CB | 0x30UL | ((IR_RP_TIMEOUT >> 16) & 0x0FUL);
    //7. set ctrl enable IR
    REG(REG_IR_CTRL) = IR_TIMEOUT_CHK_EN |
                       IR_INV            |
                       IR_RPCODE_EN      |
                       IR_LG01H_CHK_EN   |
                       IR_LDCCHK_EN      |
                       IR_EN;
}

int mstar_ir_isr_getdata_rawdecode(IRModHandle *mstar_dev)
{
    int j =0,i =0;
    unsigned char  _u8IRRawModeBuf[4]={0};
    int ret = -1;
    DEFINE_IR_RAW_DATA(ev);

    ev.pulse = 1;
    for (j=0; j<4; j++)
    {
        if ( REG(REG_IR_SHOT_CNT_H_FIFO_STATUS) & IR_FIFO_EMPTY)  // check FIFO empty
            break;
        _u8IRRawModeBuf[j] = REG(REG_IR_CKDIV_NUM_KEY_DATA) >> 8;
        REG(REG_IR_FIFO_RD_PULSE) |= 0x0001UL; //read
        for (i=0; i<10; i++); //For Delay
    }
    IRDBG_INFO("[Raw Decode] Headcode =%x Key =%x\n",(_u8IRRawModeBuf[0]<<8|_u8IRRawModeBuf[1]),_u8IRRawModeBuf[2]);
    for(i = 0;i< mstar_dev->pIRDev->support_num;i++)
    {
        if((_u8IRRawModeBuf[0] == (((u8)((mstar_dev->pIRDev->support_ir[i].u32HeadCode)>> 8))&0xff))&&(_u8IRRawModeBuf[1] == (((u8)(mstar_dev->pIRDev->support_ir[i].u32HeadCode))&0xff) ))
        {
            if (_u8IRRawModeBuf[2] == (u8)(~_u8IRRawModeBuf[3]))
            {
                if((u32)(MIRC_Get_System_Time()-u32KeyTime) <= NEC_REPEAT_TIMEOUT)
                {
                    u32KeyTime = MIRC_Get_System_Time();
                    ev.pulse = 1;
                }
                else
                {
                    u32KeyTime = MIRC_Get_System_Time();
                    ev.pulse = 0;
                }
                mstar_dev->pIRDev->map_num = mstar_dev->pIRDev->support_ir[i].u32HeadCode;
                ev.duration = (mstar_dev->pIRDev->support_ir[i].u32HeadCode<<8) | _u8IRRawModeBuf[2];

                ret = MIRC_Data_Store(mstar_dev->pIRDev, &ev);
                if (ret < 0)
                {
                    IRDBG_ERR("Store IR data Error!\n");
                }
                mstar_ir_nec_clearfifo();
                return ret;
            }
            else
            {
                IRDBG_ERR("IR FULL Decode data error!\n");
            }
        }
    }
    IRDBG_ERR("NO IR Matched!!!\n");
    mstar_ir_nec_clearfifo();
    return ret;
}

void mstar_ir_hw_rc5decode_config(void)
{
    if(ir_config_mode == IR_TYPE_HWRC5_MODE)
        return ;
    ir_config_mode = IR_TYPE_HWRC5_MODE;

    IR_PRINT("##### Mstar IR HW RC5 Decode Config #####\n");
    //00 register set rc5 enable
    REG(REG_IR_RC_CTRL) = IR_RC_EN;

    //01 register set rc5 long pulse threshold , IR head detect 889*2=1778
    REG(REG_IR_RC_LONGPULSE_THR) = IR_RC5_LONGPULSE_THR;

    //02 register set rc6 long pulse margin
    REG(REG_IR_RC_LONGPULSE_MAR) = IR_RC5_LONGPULSE_MAR;// for rc6 only ,old version retain

    //03 register set RC5 Integrator threshold * 8. To judge whether it is 0 or 1. ++  RC CLK DIV
    REG(REG_IR_RC_CLK_INT_THR) = (IR_RC_INTG_THR(IR_RC5_INTG_THR_TIME)<<1) + (IR_RC_CLK_DIV(IR_CLK)<<8);

    //04 register set RC5 watch dog counter and RC5 timeout counter
    REG(REG_IR_RC_WD_TIMEOUT_CNT) = IR_RC_WDOG_CNT(IR_RC5_WDOG_TIME) + (IR_RC_TMOUT_CNT(IR_RC5_TIMEOUT_TIME)<<8);

    //05 register set rc5 power wakeup key1&key2
    REG(REG_IR_RC_COMP_KEY1_KEY2) = 0xffffUL;

    //0A register set RC5 power wakeup address & enable power key wakeup
    REG(REG_IR_RC_CMP_RCKEY) = IR_RC_POWER_WAKEUP_EN + IR_RC5_POWER_WAKEUP_KEY;

    //50 register set code bytes
    REG(REG_IR_TIMEOUT_CYC_H_CODE_BYTE) = (IR_RC5_BITS<<8) | 0x30UL;//[6:4]=011 : timeout be clear at Key Data Code check pass
}

int mstar_ir_isr_getdata_rc5decode(IRModHandle *mstar_dev)
{
    u8 u8KeyAddr = 0;
    u8 u8KeyCmd = 0;
    u8 u8Flag = 0;
    u8KeyAddr = REG(REG_IR_RC_KEY_COMMAND_ADD)&0x3f;//RC5: {2'b0,toggle,address[4:0]} reg[7:0]
    u8KeyCmd = (REG(REG_IR_RC_KEY_COMMAND_ADD)&0x3f00)>>8;//RC5: {repeat,1'b0,command[13:8]} reg[15:8]
    u8Flag = (REG(REG_IR_RC_KEY_COMMAND_ADD)&0x8000)>>15;//repeat
    mstar_ir_rc_clearfifo();
    IRDBG_INFO("[RC5] u8KeyAddr = %x  u8KeyCmd = %x  u8Flag = %d .\n",u8KeyAddr,u8KeyCmd,u8Flag);
    return 0;
}

int mstar_ir_isr_getdata_rc5xdecode(IRModHandle *mstar_dev)
{
    u8 u8KeyAddr = 0;
    u8 u8KeyCmd = 0;
    u8 u8Flag = 0;
    u8KeyAddr = REG(REG_IR_RC_KEY_COMMAND_ADD)&0x3f;//RC5: {2'b0,toggle,address[4:0]} reg[7:0]
    u8KeyCmd = (REG(REG_IR_RC_KEY_COMMAND_ADD)&0x7f00)>>8;//RC5EXT: {repeat,command[6:0]}
    u8Flag = (REG(REG_IR_RC_KEY_COMMAND_ADD)&0x8000)>>15;//repeat
    mstar_ir_rc_clearfifo();
    IRDBG_INFO("[RC5] u8KeyAddr = %x  u8KeyCmd = %x  u8Flag = %d .\n",u8KeyAddr,u8KeyCmd,u8Flag);
    return 0;
}

void mstar_ir_hw_rc6decode_config(void)
{
    if(ir_config_mode == IR_TYPE_HWRC6_MODE)
        return ;
    ir_config_mode = IR_TYPE_HWRC6_MODE;

    IR_PRINT("##### Mstar IR HW RC6 Decode Config #####\n");
    //00 register set rc6 enable
    REG(REG_IR_RC_CTRL) = IR_RC_AUTOCONFIG|IR_RC_EN|IR_RC6_EN|(((IR_RC6_LDPS_THR(IR_RC6_LEADPULSE)>>5)&0x1f));

    //01 register set rc6 long pulse threshold , IR head detect 444*2=889
    REG(REG_IR_RC_LONGPULSE_THR) = IR_RC6_LONGPULSE_THR;

    //02 register set rc6 long pulse margin
    REG(REG_IR_RC_LONGPULSE_MAR) = (IR_RC6_LGPS_MAR(IR_RC6_LONGPULSE_MAR)&0x1ff)|
                                                (((IR_RC6_LDPS_THR(IR_RC6_LEADPULSE)>>8)&0x1c)<<8);
    //03 register set RC6 Integrator threshold * 8. To judge whether it is 0 or 1. ++  RC CLK DIV  ++RC6 ECO function enable
    REG(REG_IR_RC_CLK_INT_THR) = (IR_RC_INTG_THR(IR_RC6_INTG_THR_TIME)<<1) + (IR_RC_CLK_DIV(IR_CLK)<<8)+ IR_RC6_ECO_EN;

    //04 register set RC6 watch dog counter and RC6 timeout counter
    REG(REG_IR_RC_WD_TIMEOUT_CNT) = IR_RC_WDOG_CNT(IR_RC6_WDOG_TIME) + (IR_RC_TMOUT_CNT(IR_RC6_TIMEOUT_TIME)<<8);

    //05 register set rc6 power wakeup key1&key2
    REG(REG_IR_RC_COMP_KEY1_KEY2) = 0xffffUL;

    //0A register set RC5 power wakeup address & enable power key wakeup
    REG(REG_IR_RC_CMP_RCKEY) = IR_RC_POWER_WAKEUP_EN + IR_RC6_POWER_WAKEUP_KEY;

    //50 register set code bytes
    REG(REG_IR_TIMEOUT_CYC_H_CODE_BYTE) = (IR_RC6_BITS<<8) | 0x30UL;//[6:4]=011 : timeout be clear at Key Data Code check pass

}

int mstar_ir_isr_getdata_rc6decode(IRModHandle *mstar_dev)
{
    u8 u8KeyAddr = 0;
    u8 u8KeyCmd = 0;
    u8 u8Flag = 0;
    u8 u8Mode = 0;
    u8KeyAddr = REG(REG_IR_RC_KEY_COMMAND_ADD)&0xff;//RC6_address
    u8KeyCmd = (REG(REG_IR_RC_KEY_COMMAND_ADD)&0xff00)>>8;
    u8Flag = (REG(REG_IR_RC_KEY_MISC)&0x10)>>4;//repeat
    u8Mode = REG(REG_IR_RC_KEY_MISC)&0x05;
    mstar_ir_rc_clearfifo();
    IRDBG_INFO("[RC6_MODE%d] u8KeyAddr = %x  u8KeyCmd = %x  u8Flag = %d .\n",u8Mode,u8KeyAddr,u8KeyCmd,u8Flag);
    return 0;
}

static void mstar_ir_sw_decode_config(void)
{
    if(ir_config_mode == IR_TYPE_SWDECODE_MODE)
        return ;
    ir_config_mode = IR_TYPE_SWDECODE_MODE;

    IR_PRINT("##### Mstar IR SW Decode Config #####\n");
    //1. set ctrl enable IR
    REG(REG_IR_CTRL) = IR_TIMEOUT_CHK_EN |
                       IR_INV            |
                       IR_RPCODE_EN      |
                       IR_LG01H_CHK_EN   |
                       IR_DCODE_PCHK_EN  |
                       IR_CCODE_CHK_EN   |
                       IR_LDCCHK_EN      |
                       IR_EN;
    //2.set IR clock DIV
    REG(REG_IR_CKDIV_NUM_KEY_DATA) = IR_CKDIV_NUM;
    //3.set SW decode mode & Glitch Remove (for prevent Abnormal waves)  reg_ir_glhrm_num = 4£¬reg_ir_glhrm_en = 1
    REG(REG_IR_GLHRM_NUM) = 0x1804UL;
    //4.set PN shot interrupt
    REG(REG_IR_SEPR_BIT_FIFO_CTRL) |= 0x3UL <<12;
    //5.IR SW FIFO enable
    REG(REG_IR_SEPR_BIT_FIFO_CTRL) |= (0x1UL <<14);
    REG(REG_IR_CKDIV_NUM_KEY_DATA) = 0x00CFUL ;
    REG(REG_IR_SEPR_BIT_FIFO_CTRL) |= (0x1UL <<6);
    mstar_ir_nec_clearfifo();
    return;
}
int mstar_ir_isr_getdata_swdecode(IRModHandle *mstar_dev)
{
    int ret = 0;
    DEFINE_IR_RAW_DATA(ev);
    ev.duration = ((REG(REG_IR_SHOT_CNT_H_FIFO_STATUS)&0xF) << 16) | ((REG(REG_IR_SHOT_CNT_L))&0xFFFF);
    ev.pulse = (REG(REG_IR_SHOT_CNT_H_FIFO_STATUS))&0x10?false:true;
    ret = MIRC_Data_Store(mstar_dev->pIRDev, &ev);
    if (ret < 0)
    {
        IRDBG_ERR("Store IR data Error!\n");
    }
    return ret;
}

/*========================= end Register Setting Functions ==================*/

irqreturn_t _MDrv_IR_RC_ISR(int irq, void *dev_id)
{
    IRModHandle *mstar_dev = dev_id;
    int ret = 0;
    if(mstar_dev->pIRDev->ir_mode == IR_TYPE_HWRC5_MODE)
    ret = mstar_ir_isr_getdata_rc5decode(mstar_dev);
    else if(mstar_dev->pIRDev->ir_mode == IR_TYPE_HWRC5X_MODE)
    ret = mstar_ir_isr_getdata_rc5xdecode(mstar_dev);
    else// for IR_TYPE_HWRC6_MODE
    ret = mstar_ir_isr_getdata_rc6decode(mstar_dev);

    if(ret >= 0)
        MIRC_Data_Wakeup(mstar_dev->pIRDev);
    return IRQ_HANDLED;
}

irqreturn_t _MDrv_IR_ISR(int irq, void *dev_id)
{
    IRModHandle *mstar_dev = dev_id;
    int ret = 0;
    if (NULL == mstar_dev)
        return -EINVAL;
    if(mstar_dev->pIRDev->ir_mode == IR_TYPE_FULLDECODE_MODE)
        ret = mstar_ir_isr_getdata_fulldecode(mstar_dev);
    else if(mstar_dev->pIRDev->ir_mode == IR_TYPE_RAWDATA_MODE)
        ret = mstar_ir_isr_getdata_rawdecode(mstar_dev);
    else
    {
        spin_lock(&irq_read_lock);
        while ( ((REG(REG_IR_SHOT_CNT_H_FIFO_STATUS) & IR_FIFO_EMPTY) != IR_FIFO_EMPTY))
        {
            ret = mstar_ir_isr_getdata_swdecode(mstar_dev);
            if(ret < 0)
            {
                spin_unlock(&irq_read_lock);
                return IRQ_HANDLED;
            }
            REG(REG_IR_FIFO_RD_PULSE) |= 0x0001;
        } 
        spin_unlock(&irq_read_lock);
    }
    if(ret >= 0)
        MIRC_Data_Wakeup(mstar_dev->pIRDev);

    return IRQ_HANDLED;
}

static void mstar_ir_enable(u8 bEnableIR)
{
    struct mstar_ir_dev *dev = IRDev.pIRDev;
    if(NULL == dev)
        return ;
    if (bEnableIR)
    {
        if (ir_irq_enable == 0)
        {
            mstar_ir_nec_clearfifo();
            mstar_ir_rc_clearfifo();
            enable_irq(E_FIQ_IR);
            if((dev->ir_mode == IR_TYPE_HWRC5_MODE) ||(dev->ir_mode == IR_TYPE_HWRC5X_MODE)||(dev->ir_mode == IR_TYPE_HWRC6_MODE))
                REG(REG_IR_RC_CTRL) |= IR_RC_EN;
            else
                REG(REG_IR_CTRL) |= IR_EN;
            ir_irq_enable = 1;
        }
    }
    else
    {
        if(ir_irq_enable == 1)
        {
            if((dev->ir_mode == IR_TYPE_HWRC5_MODE) ||(dev->ir_mode == IR_TYPE_HWRC5X_MODE)||(dev->ir_mode == IR_TYPE_HWRC6_MODE))
                REG(REG_IR_RC_CTRL) &= (u16)~IR_RC_EN;
            else
                REG(REG_IR_CTRL) &= (u16)~IR_EN;
            disable_irq(E_FIQ_IR);
            ir_irq_enable = 0;
        }
    }
}
static void mstar_ir_customer_config(void)
{
    MIRC_Set_IRDBG_Level(ir_dbglevel);
    MIRC_Set_IRSpeed_Level(ir_speed);
    MIRC_IRCustomer_Config(ir_config,IR_SUPPORT_NUM);
    return ;
}
void mstar_ir_reinit(void)
{
    int ret;
    struct mstar_ir_dev *dev = IRDev.pIRDev;
    if(NULL == dev)
        return ;
    switch(dev->ir_mode)
    {
        case IR_TYPE_FULLDECODE_MODE:
        {
            mstar_ir_hw_fulldecode_config();
        }
        break;
        case IR_TYPE_RAWDATA_MODE :
        {
            mstar_ir_hw_rawdecode_config();
        }
        break;
        case IR_TYPE_HWRC5_MODE :
        {
            mstar_ir_hw_rc5decode_config();
        }
        break;
        case IR_TYPE_HWRC5X_MODE :
        {
            mstar_ir_hw_rc5decode_config();
        }
        break;
        case IR_TYPE_HWRC6_MODE :
        {
            mstar_ir_hw_rc6decode_config();
        }
        break;
        case IR_TYPE_SWDECODE_MODE :
        {
            mstar_ir_sw_decode_config();
        }
        break;
        default:
            IRDBG_INFO("No IR Mode Support!\n");
        break;
    }
    if((dev->ir_mode == IR_TYPE_HWRC5_MODE)||(dev->ir_mode == IR_TYPE_HWRC5X_MODE)||(dev->ir_mode == IR_TYPE_HWRC6_MODE))
    {
        if(ir_irq_sel == 0)
        {
#ifdef E_FIQEXPL_IR_INT_RC
            free_irq(INT_NUM_IR_ALL,&IRDev);
            ir_irq_enable = 0;
            ret = request_irq(E_FIQEXPL_IR_INT_RC, _MDrv_IR_RC_ISR, SA_INTERRUPT, "IR_RC", &IRDev);
            if (ret < 0)
            {
                IRDBG_ERR("IR IRQ registartion ERROR!\n");
            }
            else
            {
                ir_irq_enable = 1;
                IRDBG_INFO("IR IRQ registartion OK!\n");
            }
            ir_irq_sel = 1;
#endif
        }
    }
    else
    {
        if(ir_irq_sel == 1)
        {
#ifdef E_FIQEXPL_IR_INT_RC
            free_irq(E_FIQEXPL_IR_INT_RC,&IRDev);
            ir_irq_enable = 0;
#endif
            ret = request_irq(INT_NUM_IR_ALL, _MDrv_IR_ISR, SA_INTERRUPT, "IR", &IRDev);
            if (ret < 0)
            {
                IRDBG_ERR("IR IRQ registartion ERROR!\n");
            }
            else
            {
                ir_irq_enable = 1;
                IRDBG_INFO("IR IRQ registartion OK!\n");
            }
            ir_irq_sel = 0;
        }
    }
    return ;
}
EXPORT_SYMBOL_GPL(mstar_ir_reinit);

static void mstar_ir_init(int bResumeInit)
{
    int ret = 0;
    struct mstar_ir_dev *dev = IRDev.pIRDev;
    if(NULL == dev)
        return ;
    switch(dev->ir_mode)
    {
        case IR_TYPE_FULLDECODE_MODE:
        {
            mstar_ir_hw_fulldecode_config();
        }
        break;
        case IR_TYPE_RAWDATA_MODE :
        {
            mstar_ir_hw_rawdecode_config();
        }
        break;
        case IR_TYPE_HWRC5_MODE :
        {
            mstar_ir_hw_rc5decode_config();
        }
        break;
        case IR_TYPE_HWRC5X_MODE :
        {
            mstar_ir_hw_rc5decode_config();
        }
        break;
        case IR_TYPE_HWRC6_MODE :
        {
            mstar_ir_hw_rc6decode_config();
        }
        break;
        case IR_TYPE_SWDECODE_MODE :
        {
            mstar_ir_sw_decode_config();
        }
        break;
        default:
            IRDBG_INFO("No IR Mode Support!\n");
        break;
    }
    if ((!bResumeInit) &&(ir_irq_sel ==0xFF))
    {
        ir_irq_enable = 0;
        if((dev->ir_mode == IR_TYPE_HWRC5_MODE)||(dev->ir_mode == IR_TYPE_HWRC5X_MODE)||(dev->ir_mode == IR_TYPE_HWRC6_MODE))
        {
        #ifdef E_FIQEXPL_IR_INT_RC
            ret = request_irq(E_FIQEXPL_IR_INT_RC, _MDrv_IR_RC_ISR, SA_INTERRUPT, "IR_RC", &IRDev);
            ir_irq_sel = 1;
        #else
            printk("E_FIQEXPL_IR_INT_RC IRQ not Found\n");
        #endif
        }
        else
        {
            ret = request_irq(INT_NUM_IR_ALL, _MDrv_IR_ISR, SA_INTERRUPT, "IR", &IRDev);
            ir_irq_sel = 0;
        }

        if (ret < 0)
        {
            IRDBG_ERR("IR IRQ registartion ERROR!\n");
        }

        else
        {
            ir_irq_enable = 1;
            IRDBG_INFO("IR IRQ registartion OK!\n");
        }
    }
    return ;
}

int mstar_ir_register_device(struct platform_device *pdev)
{
    struct mstar_ir_dev *dev = NULL;
    int ret;
#ifdef CONFIG_MIRC_INPUT_DEVICE
    int i = 0;
#endif
    //1. alloc struct ir_dev
    dev = kzalloc(sizeof(*dev), GFP_KERNEL);
    if (!dev)
        return -ENOMEM;
    //2. init locks
    spin_lock_init(&dev->keylock);
    spin_lock_init(&irq_read_lock);
    
    mutex_init(&dev->lock);
    mutex_lock(&dev->lock);
    //3. init other args
    dev->priv = &IRDev;
    //4. init readfifo & sem & waitqueue
    ret= kfifo_alloc(&dev->read_fifo,MAX_IR_DATA_SIZE,GFP_KERNEL);
    if (ret<0)
    {
        IRDBG_ERR("ERROR kfifo_alloc!\n");
        goto out_dev;
    }
    sema_init(&dev->sem, 1);
    init_waitqueue_head(&dev->read_wait);

#ifdef CONFIG_MIRC_INPUT_DEVICE
    //5. alloc struct input_dev
    dev->input_dev = input_allocate_device();
    if (!dev->input_dev)
    {
        ret = -ENOMEM;
        goto out_kfifo;
    }
    input_set_drvdata(dev->input_dev, dev);
    dev->input_name = MSTAR_IR_DEVICE_NAME;
    dev->input_phys = "/dev/ir";
    dev->driver_name = MSTAR_IR_DRIVER_NAME;
    dev->map_num = NUM_KEYMAP_MSTAR_TV; //default :mstar keymap
    //6. init&register  input device
    dev->input_dev->id.bustype = BUS_I2C;
    dev->input_dev->id.vendor = INPUTID_VENDOR;
    dev->input_dev->id.product = INPUTID_PRODUCT;
    dev->input_dev->id.version = INPUTID_VERSION;
    set_bit(EV_KEY, dev->input_dev->evbit);
    set_bit(EV_REP, dev->input_dev->evbit);
    set_bit(EV_MSC, dev->input_dev->evbit);
    set_bit(MSC_SCAN, dev->input_dev->mscbit);
    dev->input_dev->dev.parent = &(pdev->dev);
    dev->input_dev->phys = dev->input_phys;
    dev->input_dev->name = dev->input_name;
    for (i = 0; i<KEY_CNT; i++)
    {
        __set_bit(i, dev->input_dev->keybit);
    }
    __clear_bit(BTN_TOUCH,dev->input_dev->keybit);// IR device without this case

    ret = input_register_device(dev->input_dev);
#endif
    //7. register IR Data ctrl
    ret = MIRC_Data_Ctrl_Init(dev);
    if (ret < 0)
    {
        IRDBG_ERR("Init IR Raw Data Ctrl Failed!\n");

        goto out_unlock;

        return -EINVAL;
    }
    IRDev.pIRDev= dev;
    mutex_unlock(&dev->lock);
    return 0;
out_unlock:
    mutex_unlock(&dev->lock);
#ifdef CONFIG_MIRC_INPUT_DEVICE
    input_unregister_device(dev->input_dev);
    dev->input_dev = NULL;
#endif
out_kfifo:
    kfifo_free(&dev->read_fifo);
out_dev:
    kfree(dev);
    return ret;
}

int mstar_ir_unregister_device(void)
{
    struct mstar_ir_dev *dev = IRDev.pIRDev;
    if(NULL == dev)
        return -EINVAL;
    IRDBG_INFO("mstar_ir_unregister_device start \n");
    kfifo_free(&dev->read_fifo);
#ifdef CONFIG_MIRC_INPUT_DEVICE
    input_unregister_device(dev->input_dev);
    dev->input_dev = NULL;
#endif
    MIRC_Data_Ctrl_DeInit(dev);
    kfree(dev);
    return 0;
}


/*=========================IR Cdev fileoperations Functions=============*/
/* Used to keep track of known keymaps */
static int _mod_ir_open(struct inode *inode, struct file *filp)
{
    IRModHandle *dev;
    dev = container_of(inode->i_cdev, IRModHandle, cDevice);
    filp->private_data = dev;

    return 0;
}

static int _mod_ir_release(struct inode *inode, struct file *filp)
{
    return 0;
}

static ssize_t _mod_ir_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    return 0;
}

static ssize_t _mod_ir_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    IRModHandle *ir = &IRDev;
    u32 read_data = 0;
    int ret = 0;
    if (down_interruptible(&ir->pIRDev->sem))
    {
        return -EAGAIN;
    }
    while (!kfifo_len(&ir->pIRDev->read_fifo))
    {
        up(&ir->pIRDev->sem);
        if (filp->f_flags & O_NONBLOCK)
        {
            return -EAGAIN;
        }
        if (wait_event_interruptible(ir->pIRDev->read_wait,(kfifo_len(&ir->pIRDev->read_fifo)>0)))
        {
            return -ERESTARTSYS;
        }
        if (down_interruptible(&ir->pIRDev->sem))
        {
            return -ERESTARTSYS;
        }
    }
    ret = 0;
    while (kfifo_len(&ir->pIRDev->read_fifo) && ret < count)
    {
        kfifo_out(&ir->pIRDev->read_fifo, &read_data, sizeof(read_data));

        if (copy_to_user(buf + ret, &read_data, sizeof(read_data)))
        {
            ret = -EFAULT;
            goto out;
        }
        ret += sizeof(read_data);
    }
out:
    up(&ir->pIRDev->sem);
    return ret;

}


static unsigned int _mod_ir_poll(struct file *filp, poll_table *wait)
{
    IRModHandle *ir = &IRDev;
    unsigned int mask=0;
    int ret = 0;
    if (down_interruptible(&ir->pIRDev->sem))
    {
        return -EAGAIN;
    }
    poll_wait(filp, &ir->pIRDev->read_wait, wait);
    ret = kfifo_len(&ir->pIRDev->read_fifo);
    if (ret>0)
    {
        mask |= POLLIN|POLLRDNORM;
    }
    up(&ir->pIRDev->sem);
    return mask;
}

static int _mod_ir_fasync(int fd, struct file *filp, int mode)
{
    return fasync_helper(fd, filp, mode, &IRDev.async_queue);
}

#ifdef HAVE_UNLOCKED_IOCTL
static long _mod_ir_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
#else
static int _mod_ir_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
#endif
{
    int retval = 0;
    int err= 0;
    MS_IR_KeyInfo keyinfo;
    u32 keyvalue=0;
    u8 bEnableIR;
    pid_t masterPid;
    u32 u32HeadCode = 0;
    if ((IR_IOC_MAGIC!= _IOC_TYPE(cmd)) || (_IOC_NR(cmd)> IR_IOC_MAXNR))
    {
        return -ENOTTY;
    }
    if (_IOC_DIR(cmd) & _IOC_READ)
    {
        err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
    }
    else if (_IOC_DIR(cmd) & _IOC_WRITE)
    {
        err =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
    }
    if (err)
    {
        return -EFAULT;
    }

    switch (cmd)
    {
    case MDRV_IR_INIT:
        IRDBG_INFO("ioctl:MDRV_IR_INIT\n");
        mstar_ir_init(0);
        IRDev.u32IRFlag |= (IRFLAG_IRENABLE|IRFLAG_HWINITED);
        break;
    case MDRV_IR_SEND_KEY:
        IRDBG_INFO("ioctl:MDRV_IR_SEND_KEY\n");
        retval = __get_user(keyinfo.u8Key, &(((MS_IR_KeyInfo __user *)arg)->u8Key));
        retval = __get_user(keyinfo.u8Flag, &(((MS_IR_KeyInfo __user *)arg)->u8Flag));
        keyvalue = (keyinfo.u8Key<<8 | keyinfo.u8Flag);
        keyvalue |= (0x01 << 28);
        //sned key
        if (down_trylock(&IRDev.pIRDev->sem)==0)
        {
            if (kfifo_in(&IRDev.pIRDev->read_fifo, &keyvalue, sizeof(u32)) != sizeof(u32))
                return -ENOMEM;
            up(&IRDev.pIRDev->sem);
            wake_up_interruptible(&IRDev.pIRDev->read_wait);
        }
        break;

    case MDRV_IR_SET_DELAYTIME:
        IRDBG_INFO("ioctl:MDRV_IR_GET_LASTKEYTIME\n");
        break;
        /*
                case MDRV_IR_GET_KEY:
                    IRDBG_INFO("ioctl:MDRV_IR_GET_KEY\n");
                    break;

                case MDRV_IR_GET_LASTKEYTIME:
                    IRDBG_INFO("ioctl:MDRV_IR_GET_LASTKEYTIME\n");
                    keytime.time = MIRC_Get_LastKey_Time();
                    retval = __put_user(keytime.time, &(((MS_IR_LastKeyTime __user *)arg)->time));
                    break;

                case MDRV_IR_PARSE_KEY:
                    IRDBG_INFO("ioctl: MDRV_IR_PARSE_KEY\n");
                    break;

                case MDRV_IR_TEST:
                    IRDBG_INFO("ioctl: MDRV_IR_TEST\n");
                    break;
        */
    case MDRV_IR_ENABLE_IR:
        IRDBG_INFO("ioctl: MDRV_IR_ENABLE_IR\n");
        retval = __get_user(bEnableIR, (int __user *)arg);
        mstar_ir_enable(bEnableIR);
        if (bEnableIR) {
            IRDev.u32IRFlag |= IRFLAG_IRENABLE;
        } else {
            IRDev.u32IRFlag &= ~IRFLAG_IRENABLE;
        }
        break;

        /* case MDRV_IR_IS_FANTASY_PROTOCOL_SUPPORTED:
             IRDBG_INFO("ioctl:MDRV_IR_IS_FANTASY_PROTOCOL_SUPPORTED\n");
        #ifdef CONFIG_MIRC_INPUT_DEVICE
             bIsFantasyProtocolSupported = 0;
        #else
             bIsFantasyProtocolSupported = 1;
        #endif
             retval = __put_user(bIsFantasyProtocolSupported,(u8 __user *)arg);
             break;*/

    case MDRV_IR_SET_MASTER_PID:
        IRDBG_INFO("ioctl:MDRV_IR_SET_MASTER_PID\n");
        retval = __get_user(masterPid, (pid_t __user *)arg);
        MasterPid = masterPid;
        break;

    case MDRV_IR_GET_MASTER_PID:
        IRDBG_INFO("ioctl:MDRV_IR_GET_MASTER_PID\n");
        masterPid = MasterPid;
        retval = __put_user(masterPid, (pid_t __user *)arg);
        break;

    case MDRV_IR_INITCFG:
        IRDBG_INFO("ioctl:MDRV_IR_INITCFG\n");
        break;

    case MDRV_IR_TIMECFG:
        IRDBG_INFO("ioctl:MDRV_IR_TIMECFG\n");
        break;
    case MDRV_IR_GET_SWSHOT_BUF:
        IRDBG_INFO("ioctl:MDRV_IR_GET_SWSHOT_BUF\n");
        break;
    case MDRV_IR_SET_HEADER:
        IRDBG_INFO("ioctl:MDRV_IR_SET_HEADER\n");
        if (__get_user(u32HeadCode, (int __user *)arg))
        {
            return EFAULT;
        }
        IRDBG_INFO("[SET_HEADER] u32HeadCode = %x",u32HeadCode);
        _Mdrv_IR_SetHeaderCode(u32HeadCode);
        break;
    case MDRV_IR_SET_PROTOCOL:
        IRDBG_INFO("ioctl:MDRV_IR_SET_PROTOCOL\n");
        break;
    default:
        IR_PRINT("ioctl: unknown command\n");
        return -ENOTTY;
    }
    return 0;
}

static int mstar_ir_cdev_init(void)
{
    int ret;
    dev_t dev;
    IR_PRINT("##### Mstar IR Cdev Init #####\n");

    if (IRDev.s32IRMajor) {
        dev = MKDEV(IRDev.s32IRMajor, IRDev.s32IRMinor);
        ret = register_chrdev_region(dev, MOD_IR_DEVICE_COUNT, MDRV_NAME_IR);
    } else {
        ret = alloc_chrdev_region(&dev, IRDev.s32IRMinor, MOD_IR_DEVICE_COUNT, MDRV_NAME_IR);
        IRDev.s32IRMajor = MAJOR(dev);
    }

    if ( 0 > ret) {
        IRDBG_ERR("Unable to get major %d\n", IRDev.s32IRMajor);
        return ret;
    }

    cdev_init(&IRDev.cDevice, &IRDev.IRFop);
    if (0!= (ret= cdev_add(&IRDev.cDevice, dev, MOD_IR_DEVICE_COUNT))) {
        IRDBG_ERR("Unable add a character device\n");
        unregister_chrdev_region(dev, MOD_IR_DEVICE_COUNT);
        return ret;
    }
    return 0;
}

/*=================end IR Cdev fileoperations Functions ========================*/

/***=============== IR ATTR Functions=================***/
static ssize_t mstar_ir_show_protocols(struct device *device,struct device_attribute *mattr, char *buf)
{
    u64 enable_protocols=0;
    u8 i = 0;
    ssize_t ret = 0;
    IR_PRINT("===========Protocols Attr Show========\n");
    enable_protocols = MIRC_Get_Protocols();
    IR_PRINT("Eenable_Protocols =0x%llx \n",enable_protocols);
    strcat(buf,"Eenable Protocols Name:");
    for (i =0; i<IR_TYPE_MAX; i++)
    {
        if ((enable_protocols>>i)&0x01)
        {
            switch (i)
            {
            case IR_TYPE_NEC :
                strcat(buf,"NEC  ");
                break;
            case IR_TYPE_RCA :
                strcat(buf,"RCA  ");
                break;
            case IR_TYPE_P7051:
                strcat(buf,"PANASONIC 7051  ");
                break;
            default:
                break;
            }
        }
    }
    strcat(buf,"\n");
    ret = strlen(buf)+1;
    return ret;
}

static ssize_t mstar_ir_store_protocols(struct device *device,struct device_attribute *mattr,const char *data,size_t len)
{
    u64 protocol = 0;
    IR_PRINT("===========Protocols Attr Store========\n");
    sscanf(data,"%llx",&protocol);
    IR_PRINT("Set IR Protocol :0x%llx\n",protocol);
    MIRC_Set_Protocols(protocol);
    return strlen(data);
}

static ssize_t mstar_ir_show_debug(struct device *device,struct device_attribute *mattr, char *buf)
{
    ssize_t ret = 0;
    IR_DBG_LEVEL_e level = IR_DBG_NONE;
    IR_PRINT("===========Mstar IR Debug Show========\n");
    IR_PRINT("Debug Level Table:\n");
    IR_PRINT("          0:IR_DBG_NONE\n");
    IR_PRINT("          1:IR_DBG_ERR\n");
    IR_PRINT("          2:IR_DBG_WRN\n");
    IR_PRINT("          3:IR_DBG_MSG\n");
    IR_PRINT("          4:IR_DBG_INFO\n");
    IR_PRINT("          5:IR_DBG_ALL\n");
    level = MIRC_Get_IRDBG_Level();
    sprintf(buf, "Current Debug Level:  %d\n",level);
    ret = strlen(buf)+1;
    return ret;
}

static ssize_t mstar_ir_store_debug(struct device *device,struct device_attribute *mattr,const char *data,size_t len)
{
    unsigned int level = 0;
    IR_PRINT("===========Mstar IR Debug Store========\n");
    level = (unsigned int)simple_strtoul(data, NULL, 10);
    //sscanf(data,"%d",&level);
    if (level>IR_DBG_ALL)
        level = IR_DBG_ALL;
    IR_PRINT("Set IR Debug Level :%d\n",level);
    MIRC_Set_IRDBG_Level(level);
    return strlen(data);
}
static ssize_t mstar_ir_show_speed(struct device *device,struct device_attribute *mattr, char *buf)
{
    ssize_t ret = 0;
    IR_DBG_LEVEL_e level = IR_DBG_NONE;
    IR_PRINT("===========Mstar IR Speed Show========\n");
    IR_PRINT("IR Speed Level Table:\n");
    IR_PRINT("          0:IR_SPEED_FAST_H\n");
    IR_PRINT("          1:IR_SPEED_FAST_N\n");
    IR_PRINT("          2:IR_SPEED_FAST_L\n");
    IR_PRINT("          3:IR_SPEED_NORMAL\n");
    IR_PRINT("          4:IR_SPEED_SLOW_H\n");
    IR_PRINT("          5:IR_SPEED_SLOW_N\n");
    IR_PRINT("          6:IR_SPEED_SLOW_L\n");
    level = MIRC_Get_IRSpeed_Level();
    sprintf(buf, "Current Speed Level:  %d\n",level);
    ret = strlen(buf)+1;
    return ret;
}

static ssize_t mstar_ir_store_speed(struct device *device,struct device_attribute *mattr,const char *data,size_t len)
{
    unsigned int speed = 0;
    IR_PRINT("===========Mstar IR Speed Store========\n");
    speed = (unsigned int)simple_strtoul(data, NULL, 10);
    //sscanf(data,"%d",&level);
    if (speed>IR_SPEED_SLOW_L)
        speed = IR_SPEED_SLOW_L;
    IR_PRINT("Set IR Speed Level :%d\n",speed);
    MIRC_Set_IRSpeed_Level(speed);
    return strlen(data);
}
static ssize_t mstar_ir_show_enable(struct device *device,struct device_attribute *mattr, char *buf)
{
    ssize_t ret = 0;
    unsigned int IsEnable = 0;
    IsEnable = MIRC_Get_IRSpeed_Level();
    sprintf(buf, "Current IR IsEnable:  %d\n",IsEnable);
    ret = strlen(buf)+1;
    return ret;
}

static ssize_t mstar_ir_store_enable(struct device *device,struct device_attribute *mattr,const char *data,size_t len)
{
    unsigned int Enable = 0;
    Enable = (unsigned int)simple_strtoul(data, NULL, 10);
    if(Enable)
    {
        printk("=======Enable IR!\n");
    }
    else
    {
        printk("=======Disable IR!\n");
    }
    mstar_ir_enable(Enable);
    return strlen(data);
}

#ifdef CONFIG_MIRC_INPUT_DEVICE
static ssize_t mstar_ir_show_timeout(struct device *device,struct device_attribute *mattr, char *buf)
{
    ssize_t ret = 0;
    unsigned long timeout = 0;
    IR_PRINT("===========Mstar IR Timeout Show========\n");
    timeout = MIRC_Get_Event_Timeout();
    sprintf(buf, "Current EventTimeout:  %lu\n",timeout);
    ret = strlen(buf)+1;
    return ret;
}

static ssize_t mstar_ir_store_timeout(struct device *device,struct device_attribute *mattr,const char *data,size_t len)
{
    unsigned long timeout = 0;
    IR_PRINT("===========Mstar IR Timeout Store========\n");
    timeout = (unsigned long)simple_strtoul(data, NULL, 10);
    IR_PRINT("Set IR EventTimeout  :%lu\n",timeout);
    MIRC_Set_Event_Timeout(timeout);
    return strlen(data);
}
static DEVICE_ATTR(IRTimeout, S_IRUGO | S_IWUSR, mstar_ir_show_timeout, mstar_ir_store_timeout);
#endif

static DEVICE_ATTR(IRProtocols, S_IRUGO | S_IWUSR, mstar_ir_show_protocols, mstar_ir_store_protocols);
static DEVICE_ATTR(IRDebug, S_IRUGO | S_IWUSR, mstar_ir_show_debug, mstar_ir_store_debug);
static DEVICE_ATTR(IRSpeed, S_IRUGO | S_IWUSR, mstar_ir_show_speed, mstar_ir_store_speed);
static DEVICE_ATTR(IREnable, S_IRUGO | S_IWUSR, mstar_ir_show_enable, mstar_ir_store_enable);

static int mstar_ir_creat_sysfs_attr(struct platform_device *pdev)
{
    struct device *dev = &(pdev->dev);
    int ret = 0;
    IRDBG_INFO("Debug Mstar IR Creat Sysfs\n");
    if ((ret = device_create_file(dev, &dev_attr_IRProtocols)))
        goto err_out;
    if ((ret = device_create_file(dev, &dev_attr_IRDebug)))
        goto err_out;
    if ((ret = device_create_file(dev, &dev_attr_IRSpeed)))
        goto err_out;
    if ((ret = device_create_file(dev, &dev_attr_IREnable)))
        goto err_out;
#ifdef CONFIG_MIRC_INPUT_DEVICE
    if ((ret = device_create_file(dev, &dev_attr_IRTimeout)))
        goto err_out;
#endif
    return 0;
err_out:
    return ret;
}
/**===============end IR ATTR Functions ==================**/
static int mstar_ir_drv_probe(struct platform_device *pdev)
{
    int ret=0;
    if (!(pdev->name) || strcmp(pdev->name,"Mstar-ir")|| pdev->id!=0)
    {
        ret = -ENXIO;
    }
    IRDev.u32IRFlag = 0;

    ret = mstar_ir_cdev_init();
    if (ret < 0)
    {
        IRDBG_ERR("mstar_ir_cdev_init Failed! \n");
    }
    ret = mstar_ir_creat_sysfs_attr(pdev);
    if (ret < 0)
    {
        IRDBG_ERR("mstar_ir_creat_sysfs_attr Failed! \n");
    }
    ret = mstar_ir_register_device(pdev);
    if (ret < 0)
    {
        IRDBG_ERR("mstar_ir_register_device Failed! \n");
    }

    pdev->dev.platform_data=&IRDev;

    mstar_ir_customer_config();

#ifdef CONFIG_MIRC_INPUT_DEVICE
    mstar_ir_init(0);
    IRDev.u32IRFlag |= (IRFLAG_IRENABLE|IRFLAG_HWINITED);
#endif
    return ret;
}

static int mstar_ir_drv_remove(struct platform_device *pdev)
{
    if (!(pdev->name) || strcmp(pdev->name,"Mstar-ir")|| pdev->id!=0)
    {
        return -1;
    }
    IR_PRINT("##### IR Driver Remove #####\n");
    cdev_del(&IRDev.cDevice);
    unregister_chrdev_region(MKDEV(IRDev.s32IRMajor, IRDev.s32IRMinor), MOD_IR_DEVICE_COUNT);
    mstar_ir_unregister_device();
    IRDev.u32IRFlag = 0;
    pdev->dev.platform_data=NULL;
    return 0;
}

static int mstar_ir_drv_suspend(struct platform_device *dev, pm_message_t state)
{
    IRModHandle *pIRHandle=(IRModHandle *)(dev->dev.platform_data);
    IR_PRINT("##### IR Driver Suspend #####\n");
    if (pIRHandle && (pIRHandle->u32IRFlag&IRFLAG_HWINITED))
    {
        if (pIRHandle->u32IRFlag & IRFLAG_IRENABLE)
        {
            mstar_ir_enable(0);
            ir_config_mode = IR_TYPE_MAX_MODE;
        }
    }
    return 0;
}

static int mstar_ir_drv_resume(struct platform_device *dev)
{
    IRModHandle *pIRHandle=(IRModHandle *)(dev->dev.platform_data);
    IR_PRINT("##### IR Driver Resume #####\n");
    if (pIRHandle && (pIRHandle->u32IRFlag&IRFLAG_HWINITED))
    {
        mstar_ir_init(1);
        if (pIRHandle->u32IRFlag & IRFLAG_IRENABLE)
        {
            mstar_ir_enable(1);
        }
    }
    return 0;
}

#if defined (CONFIG_ARM64)
static struct of_device_id mstarir_of_device_ids[] = {
    {.compatible = "Mstar-ir"},
                   {},
               };
#endif

static struct platform_driver Mstar_ir_driver = {
    .probe      = mstar_ir_drv_probe,
    .remove     = mstar_ir_drv_remove,
    .suspend    = mstar_ir_drv_suspend,
    .resume     = mstar_ir_drv_resume,

    .driver = {
#if defined(CONFIG_ARM64)
        .of_match_table = mstarir_of_device_ids,
#endif
        .name   = "Mstar-ir",
        .owner  = THIS_MODULE,
        .bus   = &platform_bus_type,
    }
};

static int __init mstar_ir_drv_init_module(void)
{
    int ret = 0;
    ret = platform_driver_register(&Mstar_ir_driver);
    if (ret)
    {
        IRDBG_ERR("Register Mstar IR Platform Driver Failed!");
    }
    return ret;
}

static void __exit mstar_ir_drv_exit_module(void)
{
    platform_driver_unregister(&Mstar_ir_driver);
}

module_init(mstar_ir_drv_init_module);
module_exit(mstar_ir_drv_exit_module);

MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("IR driver");
MODULE_LICENSE("GPL");
