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
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/workqueue.h>
#include <linux/poll.h>
#include <linux/wait.h>
#include <linux/cdev.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/version.h>
#include <linux/err.h>
#include <linux/types.h>
#include <linux/i2c.h>
//#include <linux/clk.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>
#include <linux/of.h>
#include <asm/io.h>
//#include <linux/clk-provider.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>
#include <linux/dma-mapping.h>
#include <linux/dmapool.h>
#include <linux/miscdevice.h>
#include <linux/vmalloc.h>
#include <asm/uaccess.h>
#include <ms_platform.h>
#include <asm/string.h>
#include <asm/div64.h>
#include <vif_common.h>

#include "drv_vif.h"
#include "hal_vif.h"
#include "mdrv_vif_io.h"
#include "8051.h"

#include "hal_dma.h"
#include "arch/infinity2_reg_padtop1.h"
#include "arch/infinity2_reg_block_ispsc.h"
#define REG_R(base,offset) (*(unsigned short*)(base+(offset*4)))
#define REG_W(base,offset,val) ((*(unsigned short*)((base)+ (offset*4)))=(val))
#define MAILBOX_HEART_REG (0x6D)
#define MAILBOX_CONCTRL_REG (0x6E)
#define MAILBOX_STATE_REG (0x6F)

extern void *g_ptVIF;
extern void *g_ptVIF2;
extern void *g_BDMA;
extern void *g_MCU8051;
extern void *g_PMSLEEP;
extern void *g_MAILBOX;
extern void *g_TOPPAD1;
extern void *g_ISP_ClkGen;
extern volatile VifRingBufShm_t *SHM_Ring;

static int mstar_vif_probe(struct platform_device *pdev);
static int mstar_vif_remove(struct platform_device *pdev);


#if 1
/* Match table for of_platform binding */
static const struct of_device_id mstar_vif_of_match[] = {
	{ .compatible = "mstar,vif", 0},
	{},
};
MODULE_DEVICE_TABLE(of, mstar_vif_of_match);
#endif

static struct platform_driver mstar_vif_driver = {
	.probe   = mstar_vif_probe,
	.remove  = mstar_vif_remove,
	.driver  = {
		.name  = "mstar-vif",
		.owner = THIS_MODULE,
		.of_match_table = mstar_vif_of_match,
	},
};

struct miscdevice vif_dev;
static int vif_open(struct inode *inode, struct file *fp);
static int vif_release(struct inode *inode, struct file *fp);
static ssize_t vif_fread(struct file *fp, char __user *buf, size_t size, loff_t *ppos);
static ssize_t vif_fwrite(struct file *fp,const char __user *buf, size_t size, loff_t *ppos);
static long vif_ioctl(struct file *fp, unsigned int cmd, unsigned long arg);
struct file_operations vif_fops =
{
    .owner = THIS_MODULE,
    .open = vif_open,
    .release = vif_release,
    .read = vif_fread,
    .write = vif_fwrite,
    .unlocked_ioctl = vif_ioctl,
};


#define IPC_RAM_SIZE (64*1024)
#define TIMER_RATIO 12

#if IPC_DMA_ALLOC_COHERENT
extern u32 IPCRamPhys;
extern char *IPCRamVirtAddr;
#else
extern unsigned long IPCRamPhys;
extern void *IPCRamVirtAddr;
#endif

void DrvVif_msleep(u32 val){

    msleep(val);

    return;
}

u8 DrvVif_FakeIrq(irqreturn_t func)
{
    return true;
}
EXPORT_SYMBOL(DrvVif_FakeIrq);

// MCU control function

void DrvVif_dumpInfo(void) {
    int idx=0, sub_idx=0;
    int bound = VIF_PORT_NUM>>1;
    printk("\n\n\n");
    for (idx=0; idx<bound; ++idx) {
        printk("  ");
        for (sub_idx=0; sub_idx<VIF_RING_QUEUE_SIZE; ++sub_idx) {
            if (SHM_Ring[idx].nReadIdx == sub_idx) {
                printk("R ");
            } else if (SHM_Ring[idx].nWriteIdx == sub_idx){
                printk("W ");
            } else {
                printk("  ");
            }
        }
        printk("   readIdx=%d writeidx=%d     ",SHM_Ring[idx].nReadIdx,SHM_Ring[idx].nWriteIdx);

        for (sub_idx=0; sub_idx<VIF_RING_QUEUE_SIZE; ++sub_idx) {
            if (SHM_Ring[idx+bound].nReadIdx == sub_idx) {
                printk("R ");
            } else if (SHM_Ring[idx+bound].nWriteIdx == sub_idx){
                printk("W ");
            } else {
                printk("  ");
            }
        }
        printk("   readIdx=%d writeidx=%d \n",SHM_Ring[idx+bound].nReadIdx,SHM_Ring[idx+bound].nWriteIdx);

        printk("< ");
        for (sub_idx=0; sub_idx<VIF_RING_QUEUE_SIZE; ++sub_idx) {
            /* Print ring buffer status */

            switch(SHM_Ring[idx].data[sub_idx].nStatus) {
                case VIF_BUF_EMPTY:
                    printk("E ");
                break;
                case VIF_BUF_FILLING:
                    printk("F ");
                break;
                case VIF_BUF_READY:
                    printk("R ");
                break;
                case VIF_BUF_INVALID:
                    printk("I ");
                break;
                default:
                break;
            }
        }

        printk(" >                        < ");
        for (sub_idx=0; sub_idx<VIF_RING_QUEUE_SIZE; ++sub_idx) {
            /* Print ring buffer status */

            switch(SHM_Ring[idx+bound].data[sub_idx].nStatus) {
                case VIF_BUF_EMPTY:
                    printk("E ");
                break;
                case VIF_BUF_FILLING:
                    printk("F ");
                break;
                case VIF_BUF_READY:
                    printk("R ");
                break;
                case VIF_BUF_INVALID:
                    printk("I ");
                break;
                default:
                break;
            }
        }

        printk(" >\n");
    }
}

/*******************************************************************************************8*/


#if 0
void DrvVif_WaitFSInts(VIF_CHANNEL_e ch)
{
    rtk_flag_value_t event_flag = 0;

	switch(ch)
	{
		case VIF_CHANNEL_0:
			event_flag = VIF_CH0_FRAME_START_INTS;
			break;
		case VIF_CHANNEL_1:
			event_flag = VIF_CH1_FRAME_START_INTS;
			break;
		case VIF_CHANNEL_2:
			event_flag = VIF_CH2_FRAME_START_INTS;
			break;
		default:
			UartSendTrace("[%s] err, over VIF chanel number \n", __func__);
	}

    event_flag = MsFlagWait(&_ints_event_flag, event_flag, MS_FLAG_WAITMODE_AND | MS_FLAG_WAITMODE_CLR);
    UartSendTrace("###[%s], event_flag = %#x, _ints_event_flag = %#x \n", __func__, event_flag, _ints_event_flag.FlagValue);
}

void DrvVif_WaitFEInts(VIF_CHANNEL_e ch)
{
    rtk_flag_value_t event_flag = 0;
	switch(ch)
	{
		case VIF_CHANNEL_0:
			event_flag = VIF_CH0_FRAME_END_INTS;
			break;
		case VIF_CHANNEL_1:
			event_flag = VIF_CH1_FRAME_END_INTS;
			break;
		case VIF_CHANNEL_2:
			event_flag = VIF_CH2_FRAME_END_INTS;
			break;
		default:
			UartSendTrace("[%s] err, over VIF chanel number \n", __func__);
	}
    event_flag = MsFlagWait(&_ints_event_flag, event_flag, MS_FLAG_WAITMODE_AND | MS_FLAG_WAITMODE_CLR);
    UartSendTrace("###[%s], event_flag = %#x, _ints_event_flag = %#x \n", __func__, event_flag, _ints_event_flag.FlagValue);
}
#endif
#if 0
void DrvVif_FS_Task_Init(void* pDummy)
{
    while(1)
    {
        DrvVif_WaitFSInts(VIF_CHANNEL_0);
    }
}

void DrvVif_FS_Parser(vm_msg_t* pMessage)
{

}

void DrvVif_FE_Task_Init(void* pDummy)
{
    while(1)
    {
        DrvVif_WaitFEInts(VIF_CHANNEL_0);
    }
}

void DrvVif_FE_Parser(vm_msg_t* pMessage)
{

}

int DrvVif_CreateFSTask(void)
{
    MsTaskCreateArgs_t TaskArgs;

    if(u8VifFSTaskId != 0xFF)
	{
		UartSendTrace("VIF_FS_TASK Task Create Fail, Taskid = %d \n", u8VifFSTaskId);
		return -VIF_FAIL;
    }
	UartSendTrace("VIF_FS_TASK Task Create\n");

    // Note the task name space is 8-1 chars
    strcpy(TaskArgs.TaskName, "VIF_FS_TASK");

    TaskArgs.AppliInit = DrvVif_FS_Task_Init;
    TaskArgs.AppliParser = DrvVif_FS_Parser;
    TaskArgs.ImmediatelyStart = TRUE;
    TaskArgs.StackSize = VIF_TASK_STACK_SIZE;
    TaskArgs.Priority = CUS58_PRIO;
    TaskArgs.pInitArgs = NULL;
    TaskArgs.TaskId = &u8VifFSTaskId;
    TaskArgs.pStackTop = MsAllocateMem(VIF_TASK_STACK_SIZE);
    TaskArgs.TimeSliceLeft = RTK_MS_TO_TICK(5);
    TaskArgs.TimeSliceMax = RTK_MS_TO_TICK(5);
    pVifFSStackTop = TaskArgs.pStackTop;

    if(MS_OK != MsCreateTask(&TaskArgs))
    {
        MsReleaseMemory(pVifFSStackTop);
        UartSendTrace("VIF_FS_TASK Task Create Fail!\n");
        return -VIF_FAIL;
    }

    return VIF_SUCCESS;
}

int DrvVif_DeleteFSTask(void)
{
	MsDeleteTask(u8VifFSTaskId);
	MsReleaseMemory(pVifFSStackTop);
	u8VifFSTaskId = 0xFF;
    return VIF_SUCCESS;
}

int DrvVif_CreateFETask(void)
{
    MsTaskCreateArgs_t TaskArgs;

    if(u8VifFETaskId != 0xFF)
	{
		UartSendTrace("VIF_FE_TASK Task Create Fail, Taskid = %d \n", u8VifFETaskId);
		return -VIF_FAIL;
    }
	UartSendTrace("VIF_FE_TASK Task Create\n");

    // Note the task name space is 8-1 chars
    strcpy(TaskArgs.TaskName, "VIF_FE_TASK");

    TaskArgs.AppliInit = DrvVif_FE_Task_Init;
    TaskArgs.AppliParser = DrvVif_FE_Parser;
    TaskArgs.ImmediatelyStart = TRUE;
    TaskArgs.StackSize = VIF_TASK_STACK_SIZE;
    TaskArgs.Priority = CUS58_PRIO;
    TaskArgs.pInitArgs = NULL;
    TaskArgs.TaskId = &u8VifFETaskId;
    TaskArgs.pStackTop = MsAllocateMem(VIF_TASK_STACK_SIZE);
    TaskArgs.TimeSliceLeft = RTK_MS_TO_TICK(5);
    TaskArgs.TimeSliceMax = RTK_MS_TO_TICK(5);
    pVifFEStackTop = TaskArgs.pStackTop;

    if(MS_OK != MsCreateTask(&TaskArgs))
    {
        MsReleaseMemory(pVifFEStackTop);
        UartSendTrace("VIF_FE_TASK Task Create Fail!\n");
        return -VIF_FAIL;
    }

    return VIF_SUCCESS;
}

int DrvVif_DeleteFETask(void)
{
	MsDeleteTask(u8VifFETaskId);
	MsReleaseMemory(pVifFEStackTop);
	u8VifFETaskId = 0xFF;
    return VIF_SUCCESS;
}

void DrvVif_ClearInt(volatile u32 *clear, VIF_INTERRUPT_e int_num)
{
    *clear |= (0x01 << int_num);
}
#endif



s32 DrvVif_Init(void)
{
	int ret = E_HAL_VIF_SUCCESS, rc = 0;
	struct resource *res;
	struct device_node *dev_node;
	struct platform_device *pdev;

	dev_node = of_find_compatible_node(NULL, NULL, "mstar,vif");
	if (!dev_node) {
		if (!dev_node)
			return -ENODEV;
	}

	pdev = of_find_device_by_node(dev_node);
	if (!pdev) {
		of_node_put(dev_node);
		return -ENODEV;
	}

    pr_info("DrvVif_Init\n");

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	g_ptVIF = devm_ioremap_resource(&pdev->dev, res);
	pr_info("g_ptVIF: %p\n", g_ptVIF);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 1);
	g_ptVIF2 = devm_ioremap_resource(&pdev->dev, res);
	pr_info("g_ptVIF2: %p\n", g_ptVIF2);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 2);
	g_BDMA = devm_ioremap_resource(&pdev->dev, res);
	pr_info("BDMA: %p\n", g_BDMA);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 3);
	g_MCU8051 = devm_ioremap_resource(&pdev->dev, res);
	pr_info("g_MCU8051: %p\n", g_MCU8051);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 4);
	g_PMSLEEP = devm_ioremap_resource(&pdev->dev, res);
	pr_info("g_PMSLEEP: %p\n", g_PMSLEEP);

    res = platform_get_resource(pdev, IORESOURCE_MEM, 5);
    g_MAILBOX = devm_ioremap_resource(&pdev->dev, res);
    pr_info("MAILBOX: %p\n", g_MAILBOX);

    res = platform_get_resource(pdev, IORESOURCE_MEM, 6);
    g_TOPPAD1 = devm_ioremap_resource(&pdev->dev, res);
    pr_info("g_TOPPAD1: %p\n", g_TOPPAD1);

    res = platform_get_resource(pdev, IORESOURCE_MEM, 9);
    g_ISP_ClkGen = devm_ioremap_resource(&pdev->dev, res);
    pr_info("g_ISP_ClkGen: %p\n", g_ISP_ClkGen);

#if IPC_DMA_ALLOC_COHERENT
    pr_info("IPCRamVirtAddr allocated by DMA coherent!!\n");
#else
    res = platform_get_resource(pdev, IORESOURCE_MEM, 10);
    IPCRamVirtAddr = ioremap(res->start, 0x10000);
    //IPCRamVirtAddr = __va(res->start);
    IPCRamPhys = res->start;
    pr_info("IPCRamVirtAddr: %p IPCRamPhys: 0x%lx\n", IPCRamVirtAddr, IPCRamPhys);
#endif

    vif_dev.minor = MISC_DYNAMIC_MINOR;
    vif_dev.name = "vif";
    vif_dev.fops = &vif_fops;
    vif_dev.parent = &pdev->dev;
    misc_register(&vif_dev);

#if IPC_DMA_ALLOC_COHERENT
	IPCRamVirtAddr = dma_alloc_coherent(NULL, IPC_RAM_SIZE, &IPCRamPhys, GFP_KERNEL);
	pr_info("[DMA coherent] IPCRamVirtAddr:%p, IPCRamPhys:%#x, size:%#x\n", IPCRamVirtAddr, IPCRamPhys, IPC_RAM_SIZE);
#endif

    // Write 8051 Share memory Address to mailbox
    REG_W(g_MAILBOX, MAILBOX_HEART_REG, ((Chip_Phys_to_MIU(IPCRamPhys) >> 16) & 0xFFFF));
    REG_W(g_MAILBOX, MAILBOX_STATE_REG, 0x0);
    REG_W(g_MAILBOX, MAILBOX_CONCTRL_REG, 0x0);

	//copy 8051 bin file
	memcpy(IPCRamVirtAddr,hk51,sizeof(hk51));
	initialSHMRing();
	DrvVif_MCULoadBin();

	rc = DrvVif_stopMCU();
	if (rc) {
		pr_err("Init stop MCU fail\n");
	}

	rc = DrvVif_startMCU();
	if (rc) {
		pr_err("Start MCU fail\n");
	}

	rc = DrvVif_pollingMCU();
	if (rc) {
		pr_err("Polling MCU fail\n");
	}

	// Set VIF CLK
	DrvVif_CLK();

	// initial ISR mask
	//DrvVif_SetDefaultIntsMask();

	// reg base addr maapping assign
	DrvVif_SetVifChanelBaseAddr();

	//DrvVif_Init();
	HalDma_Init();

    return ret;
}
EXPORT_SYMBOL(DrvVif_Init);

s32 DrvVif_Deinit(void)
{
	int ret = E_HAL_VIF_SUCCESS;

	struct device_node *dev_node;
	struct platform_device *pdev;

	dev_node = of_find_compatible_node(NULL, NULL, "mstar,vif");
	if (!dev_node) {
		if (!dev_node)
			return -ENODEV;
	}

	pdev = of_find_device_by_node(dev_node);
	if (!pdev) {
		of_node_put(dev_node);
		return -ENODEV;
	}
#if 0
    MsFlagDestroy(&_ints_event_flag);
#endif
	devm_iounmap(&pdev->dev, g_ptVIF);
	devm_iounmap(&pdev->dev, g_ptVIF2);
	devm_iounmap(&pdev->dev, g_BDMA);
	devm_iounmap(&pdev->dev, g_MCU8051);
	devm_iounmap(&pdev->dev, g_PMSLEEP);
	devm_iounmap(&pdev->dev, g_MAILBOX);
	devm_iounmap(&pdev->dev, g_TOPPAD1);
	devm_iounmap(&pdev->dev, g_ISP_ClkGen);
#if IPC_DMA_ALLOC_COHERENT
        dma_free_coherent(NULL, IPC_RAM_SIZE, IPCRamVirtAddr, IPCRamPhys);
#else
        iounmap(IPCRamVirtAddr);
#endif

    return ret;
}
EXPORT_SYMBOL(DrvVif_Deinit);


static int vif_open(struct inode *inode, struct file *fp)
{
    return 0;
}

static int vif_release(struct inode *inode, struct file *fp)
{
    return 0;
}
static ssize_t vif_fread(struct file *fp, char __user *buf, size_t size, loff_t *ppos)
{
    return 0;
}

static ssize_t vif_fwrite(struct file *fp,const char __user *buf, size_t size, loff_t *ppos)
{
    int rc = 0;
    int cmd = 0;
    sscanf(buf, "%d", &cmd);
    //pr_info("buf %s\n",buf);
    switch(cmd) {
        case 1:
        {
            rc = DrvVif_stopMCU();
            if (rc) {
                pr_err("Init stop MCU fail\n");
                goto VIF_FAIL;
            }
            pr_info("Stop MCU done!\n");
        }
        break;
        case 2:
        {
            rc = DrvVif_startMCU();
            if (rc) {
                pr_err("Start MCU fail\n");
                goto VIF_FAIL;
            }
            pr_info("Start MCU done!\n");
        }
        break;
        case 3:
        {
            rc = DrvVif_pollingMCU();
            if (rc) {
                pr_err("Polling MCU fail\n");
                goto VIF_FAIL;
            }
            pr_info("Polling MCU done!\n");
        }
        break;
        case 4:
        {
            rc = DrvVif_pauseMCU();
            if (rc) {
                pr_err("Pause MCU fail\n");
                goto VIF_FAIL;
            }
            pr_info("Pause MCU done!\n");
        }
        break;

        //Ring buffer queue/dequeue
        case 10:
        {
             MHal_VIF_RingBufElm_t temp = { {0x2E600000,0x2E620000}, {720, 720}, 0, 0, 720, 240, 264960, VIF_BUF_INVALID, 0};
             MHal_VIF_PORT u32ChnPort = 0;
             u32 channel = 0;
             u32 main_addr=0, sub_addr=0;
             sscanf(buf, "%*d %d %d %x %x", &channel, &u32ChnPort, &main_addr, &sub_addr);
             temp.u64PhyAddr[0] = main_addr;
             temp.u64PhyAddr[1] = sub_addr;
             printk("main addr %x, sub addr %x\n",main_addr,sub_addr);
             DrvVif_QueueFrameBuffer(channel, u32ChnPort,&temp);
        }
        break;
        case 11:
        {
             u32 channel = 0;
             u32 num = 0;
             MHal_VIF_PORT u32ChnPort = 0;
             DrvVif_QueryFrames(channel, u32ChnPort,&num);
             pr_info("Query number of READY frame = %d\n",num);
        }
        break;
        case 12:
        {
             //MHal_VIF_RingBufElm_t temp = {0};
             MHal_VIF_RingBufElm_t temp;
             u32 num = 0;
             u8 idx = 0;
             u32 channel = 0;
             MHal_VIF_PORT u32ChnPort = 0;
             DrvVif_QueryFrames(channel, u32ChnPort,&num);
             memset(&temp,0,sizeof(MHal_VIF_RingBufElm_t));
             for (idx = 0;idx<num;idx++) {
                 DrvVif_DequeueFrameBuffer(channel, u32ChnPort, &temp);
                 pr_info("Dequeue buffer info: AddrY=0x%x AddrC=0x%x width=%d height=%d pitch=%d nPTS=0x%x\n",
                         (u32)temp.u64PhyAddr[0], (u32)temp.u64PhyAddr[1], temp.nCropW, temp.nCropH, temp.u32Stride[0], temp.nPTS);
             }
             pr_info("Query number of READY frame = %d\n",num);
             pr_info("Pre_idx = %d\n",SHM_Ring[channel].pre_nReadIdx);
        }
        break;
        case 13:
        {
             MHal_VIF_PORT u32ChnPort = 0;
             u32 channel = 0;
             u32 eFrameRate;
             sscanf(buf, "%*d %d %d %d", &channel, &u32ChnPort, &eFrameRate);
             DrvVif_setChnFPSBitMask(channel, u32ChnPort, eFrameRate, NULL);
        }
        break;
        case 254:
        {
             DrvVif_dumpInfo();
        }
        break;
        default:
            pr_err("Invalid command[%d]!!",cmd);
        break;
    }
    return size;
VIF_FAIL:
    //unInitialSHMRing();
    return size;
}

static long vif_ioctl(struct file *fp, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
        VIF_CMD_st VifCmd;

	if (_IOC_TYPE(cmd) != VIF_IOCTL_MAGIC) return -ENOTTY;
	switch(cmd)
	{
            case IOCTL_VIF_MCU_DEV_SET_CFG:
            {
                memset(&VifCmd,0,sizeof(VIF_CMD_st));
                if(copy_from_user(&VifCmd, (VIF_CMD_st *)arg, sizeof(VIF_CMD_st)))
                    return -EFAULT;

                pr_info("IOCTL_VIF_MCU_SET_CFG\n");
                ret = DrvVif_DevSetConfig(VifCmd.MCU_DevSetConfig.u32VifDev,(MHal_VIF_DevCfg_t *) &(VifCmd.MCU_DevSetConfig.pstDevAttr));
                return ret;
            }
            break;
            case IOCTL_VIF_MCU_DEV_ENABLE:
            {
                memset(&VifCmd,0,sizeof(VIF_CMD_st));
                if(copy_from_user(&VifCmd, (VIF_CMD_st *)arg, sizeof(VIF_CMD_st)))
                    return -EFAULT;

                pr_info("IOCTL_VIF_MCU_DEV_ENABLE\n");
                ret = DrvVif_DevEnable(VifCmd.MCU_DevEnable.u32VifDev);
                return ret;
            }
            break;
            case IOCTL_VIF_MCU_DEV_DISABLE:
            {
                memset(&VifCmd,0,sizeof(VIF_CMD_st));
                if(copy_from_user(&VifCmd, (VIF_CMD_st *)arg, sizeof(VIF_CMD_st)))
                    return -EFAULT;

                pr_info("IOCTL_VIF_MCU_DEV_DISABLE\n");
                ret = DrvVif_DevDisable(VifCmd.MCU_DevDisable.u32VifDev);
                return ret;
            }
            break;

            case IOCTL_VIF_MCU_CHN_SET_CFG:
            {
                memset(&VifCmd,0,sizeof(VIF_CMD_st));
                if(copy_from_user(&VifCmd, (VIF_CMD_st *)arg, sizeof(VIF_CMD_st)))
                    return -EFAULT;

                pr_info("IOCTL_VIF_MCU_CHN_SET_CFG :%d\n",VifCmd.MCU_ChnSetConfig.pstAttr.stCapRect.u32Width);
                ret = DrvVif_ChnSetConfig(VifCmd.MCU_ChnSetConfig.u32VifChn, (MHal_VIF_ChnCfg_t *)&(VifCmd.MCU_ChnSetConfig.pstAttr));
                return ret;
            }
            break;
            case IOCTL_VIF_MCU_CHN_ENABLE:
            {
                memset(&VifCmd,0,sizeof(VIF_CMD_st));
                if(copy_from_user(&VifCmd, (VIF_CMD_st *)arg, sizeof(VIF_CMD_st)))
                    return -EFAULT;

                pr_info("IOCTL_VIF_MCU_CHN_ENABLE\n");
                ret = DrvVif_ChnEnable(VifCmd.MCU_ChnEnable.u32VifChn);
                return ret;
            }
            break;
            case IOCTL_VIF_MCU_CHN_DISABLE:
            {
                memset(&VifCmd,0,sizeof(VIF_CMD_st));
                if(copy_from_user(&VifCmd, (VIF_CMD_st *)arg, sizeof(VIF_CMD_st)))
                    return -EFAULT;

                pr_info("IOCTL_VIF_MCU_CHN_DISABLE\n");
                ret = DrvVif_ChnDisable(VifCmd.MCU_ChnEnable.u32VifChn);
                return ret;
            }
            break;
            case IOCTL_VIF_MCU_CHN_QUERY:
            {
                memset(&VifCmd,0,sizeof(VIF_CMD_st));
                if(copy_from_user(&VifCmd, (VIF_CMD_st *)arg, sizeof(VIF_CMD_st)))
                    return -EFAULT;

                pr_info("IOCTL_VIF_MCU_QUERY\n");
                ret = DrvVif_ChnQuery(VifCmd.MCU_ChnQuery.u32VifChn,(MHal_VIF_ChnStat_t *) &(VifCmd.MCU_ChnQuery.pstStat));
                return ret;
            }
            break;

            case IOCTL_VIF_SUB_MCU_CHN_SET_CFG:
            {
                memset(&VifCmd,0,sizeof(VIF_CMD_st));
                if(copy_from_user(&VifCmd, (VIF_CMD_st *)arg, sizeof(VIF_CMD_st)))
                    return -EFAULT;

                pr_info("IOCTL_VIF_SUB_MCU_CHN_SET_CFG\n");
                ret = DrvVif_SubChnSetConfig(VifCmd.MCU_SubChnSetConfig.u32VifChn,(MHal_VIF_SubChnCfg_t *) &(VifCmd.MCU_SubChnSetConfig.pstAttr));
                return ret;
            }
            break;
            case IOCTL_VIF_SUB_MCU_CHN_ENABLE:
            {
                memset(&VifCmd,0,sizeof(VIF_CMD_st));
                if(copy_from_user(&VifCmd, (VIF_CMD_st *)arg, sizeof(VIF_CMD_st)))
                    return -EFAULT;

                pr_info("IOCTL_VIF_MCU_CHN_ENABLE\n");
                ret = DrvVif_SubChnEnable(VifCmd.MCU_SubChnEnable.u32VifChn);
                return ret;
            }
            break;
            case IOCTL_VIF_SUB_MCU_CHN_DISABLE:
            {
                memset(&VifCmd,0,sizeof(VIF_CMD_st));
                if(copy_from_user(&VifCmd, (VIF_CMD_st *)arg, sizeof(VIF_CMD_st)))
                    return -EFAULT;

                pr_info("IOCTL_VIF_MCU_CHN_DISABLE\n");
                ret = DrvVif_SubChnDisable(VifCmd.MCU_SubChnDisable.u32VifChn);
                return ret;
            }
            break;
            case IOCTL_VIF_SUB_MCU_CHN_QUERY:
            {
                memset(&VifCmd,0,sizeof(VIF_CMD_st));
                if(copy_from_user(&VifCmd, (VIF_CMD_st *)arg, sizeof(VIF_CMD_st)))
                    return -EFAULT;

                pr_info("IOCTL_VIF_MCU_CHN_QUERY\n");
                ret = DrvVif_SubChnQuery(VifCmd.MCU_SubChnQuery.u32VifChn, (MHal_VIF_ChnStat_t *)&(VifCmd.MCU_SubChnQuery.pstStat));
                return ret;
            }
            break;

            case IOCTL_VIF_MCU_QUEUE_FRAME:
            {
                memset(&VifCmd,0,sizeof(VIF_CMD_st));
                if(copy_from_user(&VifCmd, (VIF_CMD_st *)arg, sizeof(VIF_CMD_st)))
                    return -EFAULT;

                pr_info("IOCTL_VIF_MCU_QUEUE_FRAME\n");
                ret = DrvVif_QueueFrameBuffer(VifCmd.MCU_Queue_Frame.u32VifChn, VifCmd.MCU_Queue_Frame.u32ChnPort, (MHal_VIF_RingBufElm_t *)&(VifCmd.MCU_Queue_Frame.ptFbInfo));
                return ret;
            }
            break;
            case IOCTL_VIF_MCU_QUERY_FRAME:
            {
                memset(&VifCmd,0,sizeof(VIF_CMD_st));
                if(copy_from_user(&VifCmd, (VIF_CMD_st *)arg, sizeof(VIF_CMD_st)))
                    return -EFAULT;

                pr_info("IOCTL_VIF_MCU_QUERY_FRAME\n");
                ret = DrvVif_QueryFrames(VifCmd.MCU_Query_Frame.u32VifChn, VifCmd.MCU_Query_Frame.u32ChnPort,(u32 *)&(VifCmd.MCU_Query_Frame.pNumBuf));
				pr_info("ready frame num:%d\n",VifCmd.MCU_Query_Frame.pNumBuf);
                return ret;
            }
            break;
            case IOCTL_VIF_MCU_DEQUEUE_FRAME:
            {
                memset(&VifCmd,0,sizeof(VIF_CMD_st));
                if(copy_from_user(&VifCmd, (VIF_CMD_st *)arg, sizeof(VIF_CMD_st)))
                    return -EFAULT;

                pr_info("IOCTL_VIF_MCU_DEQUEUE_FRAME\n");
                ret = DrvVif_DequeueFrameBuffer(VifCmd.MCU_DeQueue_Frame.u32VifChn, VifCmd.MCU_DeQueue_Frame.u32ChnPort,(MHal_VIF_RingBufElm_t *)&(VifCmd.MCU_DeQueue_Frame.ptFbInfo));
                return ret;
            }
            break;
            case IOCTL_VIF_MCU_TEST_FLOW:
            {
                memset(&VifCmd,0,sizeof(VIF_CMD_st));
                if(copy_from_user(&VifCmd, (VIF_CMD_st *)arg, sizeof(VIF_CMD_st)))
                    return -EFAULT;

                pr_info("IOCTL_VIF_MCU_TEST_FLOW\n");
                //ret = DrvVif_DequeueFrameBuffer(VifCmd.MCU_DeQueue_Frame.u32VifChn, VifCmd.MCU_DeQueue_Frame.u32ChnPort,VifCmd.MCU_DeQueue_Frame.ptFbInfo);
                return ret;
            }
            break;
            default:
                pr_err("Invalid ioctl command\n");
	}
    return 0;
}

static int mstar_vif_probe(struct platform_device *pdev)
{
	struct resource *res;
	int rc = 0;
#if 0
    struct clk **vif_clks;
	int num_parents, i,rc = 0;

    num_parents = of_clk_get_parent_count(pdev->dev.of_node);
    vif_clks = kzalloc((sizeof(struct clk *) * num_parents), GFP_KERNEL);

    //enable all clk
    for(i = 0; i < num_parents; i++)
    {
        vif_clks[i] = of_clk_get(pdev->dev.of_node, i);
        if (IS_ERR(vif_clks[i]))
        {
            pr_err( "[%s] Fail to get clk!\n", __func__);
            kfree(vif_clks);
            return -1;
        }
        else
        {
            clk_prepare_enable(vif_clks[i]);
            if(i == 0)
	            clk_set_rate(vif_clks[i], 12000000);
        }
    }
    kfree(vif_clks);
#endif

    pr_info("vif driver probe\n");

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	g_ptVIF = devm_ioremap_resource(&pdev->dev, res);
	pr_info("g_ptVIF: %p\n", g_ptVIF);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 1);
	g_ptVIF2 = devm_ioremap_resource(&pdev->dev, res);
	pr_info("g_ptVIF2: %p\n", g_ptVIF2);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 2);
	g_BDMA = devm_ioremap_resource(&pdev->dev, res);
	pr_info("BDMA: %p\n", g_BDMA);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 3);
	g_MCU8051 = devm_ioremap_resource(&pdev->dev, res);
	pr_info("g_MCU8051: %p\n", g_MCU8051);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 4);
	g_PMSLEEP = devm_ioremap_resource(&pdev->dev, res);
	pr_info("g_PMSLEEP: %p\n", g_PMSLEEP);

    res = platform_get_resource(pdev, IORESOURCE_MEM, 5);
    g_MAILBOX = devm_ioremap_resource(&pdev->dev, res);
    pr_info("MAILBOX: %p\n", g_MAILBOX);

    res = platform_get_resource(pdev, IORESOURCE_MEM, 6);
    g_TOPPAD1 = devm_ioremap_resource(&pdev->dev, res);
    pr_info("g_TOPPAD1: %p\n", g_TOPPAD1);

    res = platform_get_resource(pdev, IORESOURCE_MEM, 9);
    g_ISP_ClkGen = devm_ioremap_resource(&pdev->dev, res);
    pr_info("g_ISP_ClkGen: %p\n", g_ISP_ClkGen);

#if IPC_DMA_ALLOC_COHERENT
    pr_info("IPCRamVirtAddr allocated by DMA coherent!!\n");
#else
    res = platform_get_resource(pdev, IORESOURCE_MEM, 10);
    IPCRamVirtAddr = ioremap(res->start, 0x10000);
    //IPCRamVirtAddr = __va(res->start);
    IPCRamPhys = res->start;
    pr_info("IPCRamVirtAddr: %p IPCRamPhys: 0x%lx\n", IPCRamVirtAddr, IPCRamPhys);
#endif

    vif_dev.minor = MISC_DYNAMIC_MINOR;
    vif_dev.name = "vif";
    vif_dev.fops = &vif_fops;
    vif_dev.parent = &pdev->dev;
    misc_register(&vif_dev);

#if IPC_DMA_ALLOC_COHERENT
	IPCRamVirtAddr = dma_alloc_coherent(NULL, IPC_RAM_SIZE, &IPCRamPhys, GFP_KERNEL);
	pr_info("[DMA coherent] IPCRamVirtAddr:%p, IPCRamPhys:%#x, size:%#x\n", IPCRamVirtAddr, IPCRamPhys, IPC_RAM_SIZE);
#endif

    // Write 8051 Share memory Address to mailbox
    REG_W(g_MAILBOX, MAILBOX_HEART_REG, ((Chip_Phys_to_MIU(IPCRamPhys) >> 16) & 0xFFFF));
    REG_W(g_MAILBOX, MAILBOX_STATE_REG, 0x0);
    REG_W(g_MAILBOX, MAILBOX_CONCTRL_REG, 0x0);

	//copy 8051 bin file
	memcpy(IPCRamVirtAddr,hk51,sizeof(hk51));
	initialSHMRing();
	DrvVif_MCULoadBin();

	rc = DrvVif_stopMCU();
	if (rc) {
		pr_err("Init stop MCU fail\n");
	}

	rc = DrvVif_startMCU();
	if (rc) {
		pr_err("Start MCU fail\n");
	}

	rc = DrvVif_pollingMCU();
	if (rc) {
		pr_err("Polling MCU fail\n");
	}

	// Set VIF CLK
	DrvVif_CLK();

	// initial ISR mask
	//DrvVif_SetDefaultIntsMask();

	// reg base addr maapping assign
	DrvVif_SetVifChanelBaseAddr();

	//DrvVif_Init();
	HalDma_Init();
	return 0;
}

static int mstar_vif_remove(struct platform_device *pdev)
{
	devm_iounmap(&pdev->dev, g_ptVIF);
	devm_iounmap(&pdev->dev, g_ptVIF2);
	devm_iounmap(&pdev->dev, g_BDMA);
	devm_iounmap(&pdev->dev, g_MCU8051);
	devm_iounmap(&pdev->dev, g_PMSLEEP);
	devm_iounmap(&pdev->dev, g_MAILBOX);
	devm_iounmap(&pdev->dev, g_TOPPAD1);
	devm_iounmap(&pdev->dev, g_ISP_ClkGen);
#if IPC_DMA_ALLOC_COHERENT
        dma_free_coherent(NULL, IPC_RAM_SIZE, IPCRamVirtAddr, IPCRamPhys);
#else
        iounmap(IPCRamVirtAddr);
#endif
	//DrvVif_Deinit();

	return 0;
}

static int __init mstar_vif_init_driver(void)
{
	return platform_driver_register(&mstar_vif_driver);
}

static void __exit mstar_vif_exit_driver(void)
{
	platform_driver_unregister(&mstar_vif_driver);
}

subsys_initcall(mstar_vif_init_driver);
module_exit(mstar_vif_exit_driver);

MODULE_DESCRIPTION("MStar VIF driver");
MODULE_AUTHOR("MSTAR");
//MODULE_LICENSE("GPL");
