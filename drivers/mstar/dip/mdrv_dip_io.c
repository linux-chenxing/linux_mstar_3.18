#include <linux/pfn.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>          /* seems do not need this */
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/time.h>

#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <asm/string.h>
#include <linux/cdev.h>
#include <linux/platform_device.h>
#include <linux/poll.h>
#include <linux/irqreturn.h>
#include <linux/interrupt.h>

//#include "irqs.h"
#include "ms_platform.h"
#include "ms_msys.h"

#include "mdrv_dip.h"
#include "mdrv_dip_io_st.h"
#include "mdrv_dip_io.h"
#include "MsTypes.h"
//#include "drvXC_IOPort.h"
#include "apiXC.h"
//#include "drv_sc_isr.h"
#include "mhal_divp_datatype.h"
#include "mhal_divp.h"

#ifndef DIP_UT_ENABLE_RIU
    #include "mhal_cmdq.h"
#endif

extern int debug_level;
module_param(debug_level, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(debug_level, "\nLevels:\n"
                 " [1] Debug level 1\n"
                 " [2] Debug level 2\n"
                 " [3] Debug level 3\n"
                 " [4] Debug level 4");

extern MS_U16 MDrv_XC_DIVP_MDWIN_GetIntrStatus(void* pstCmdQInfo);
extern void MDrv_XC_DIVP_COVER_SetConfig(void* pstCmdQInfo, DIP_COVER_PROPERTY *stDIPCoverProperty);


#define MDRV_DIP_DEVICE_COUNT   1
#define MDRV_DIP_NAME           "mdip"
#define MAX_FILE_HANDLE_SUPPRT  64
#define MDRV_NAME_DIP           "mdip"
#define MDRV_MAJOR_DIP          0xea
#define MDRV_MINOR_DIP          0x02


#define CMD_PARSING(x)  (x==IOCTL_DIP_SET_INTERRUPT ?         "IOCTL_DIP_SET_INTERRUPT" : \
                         x==IOCTL_DIP_SET_CONFIG ?            "IOCTL_DIP_SET_CONFIG" : \
                         x==IOCTL_DIP_SET_WONCE_BASE_CONFIG ? "IOCTL_DIP_SET_WONCE_BASE_CONFIG" : \
                         x==IOCTL_DIP_GET_INTERRUPT_STATUS ?  "IOCTL_DIP_GET_INTERRUPT_STATUS" : \
                         x==IOCTL_DIVP_INIT_CONFIG ?  "IOCTL_DIVP_INIT_CONFIG" : \
                         x==IOCTL_DIVP_DEINIT_CONFIG ?  "IOCTL_DIVP_DEINIT_CONFIG" : \
                         x==IOCTL_DIVP_CREATE_INST ?  "IOCTL_DIVP_CREATE_INST" : \
                         x==IOCTL_DIVP_DESTROY_INST ?  "IOCTL_DIVP_DESTROY_INST" : \
                         x==IOCTL_DIVP_PROCESS_DRAM_DATA ?  "IOCTL_DIVP_PROCESS_DRAM_DATA" : \
                         x==IOCTL_DIVP_CAPTURE_TIMING ?  "IOCTL_DIVP_CAPTURE_TIMING" : \
                         x==IOCTL_DIVP_SET_ATTR_TNR ?  "IOCTL_DIVP_SET_ATTR_TNR" : \
                         x==IOCTL_DIVP_SET_ATTR_DITYPE ?  "IOCTL_DIVP_SET_ATTR_DITYPE" : \
                         x==IOCTL_DIVP_SET_ATTR_ROTATE ?  "IOCTL_DIVP_SET_ATTR_ROTATE" : \
                         x==IOCTL_DIVP_SET_ATTR_WINDOW ?  "IOCTL_DIVP_SET_ATTR_WINDOW" : \
                         x==IOCTL_DIVP_SET_ATTR_MIRROR ?  "IOCTL_DIVP_SET_ATTR_MIRROR" : \
                         x==IOCTL_DIVP_SET_COVER ?  "IOCTL_DIVP_SET_COVER" : \
                         "unknown")


int mstar_dip_drv_open(struct inode *inode, struct file *filp);
int mstar_dip_drv_release(struct inode *inode, struct file *filp);
long mstar_dip_drv_ioctl(struct file *filp, unsigned int u32Cmd, unsigned long u32Arg);
static int mstar_dip_drv_probe(struct platform_device *pdev);
static int mstar_dip_drv_remove(struct platform_device *pdev);
static int mstar_dip_drv_suspend(struct platform_device *dev, pm_message_t state);
static int mstar_dip_drv_resume(struct platform_device *dev);
static void mstar_dip_drv_platfrom_release(struct device *device);
static unsigned int mstar_dipw_drv_poll(struct file *filp, struct poll_table_struct *wait);



extern atomic_t DIPW_intr_count;

//-------------------------------------------------------------------------------------------------

typedef struct ut_divp_ChnContex_s
{
    MS_U32 u32DeviceId;
    MS_U32 u32ChnId;
    void* pHalCtx;
} ut_divp_ChnContex_t;

typedef struct
{
    int s32Major;
    int s32Minor;
    int refCnt;
    struct cdev cdev;
    struct file_operations fops;
} DIP_DEV;

static DIP_DEV _devDIP =
{
    .s32Major = MDRV_MAJOR_DIP,
    .s32Minor = MDRV_MINOR_DIP,
    .refCnt = 0,
    .cdev =
    {
        .kobj = {.name = MDRV_NAME_DIP, },
        .owner = THIS_MODULE,
    },
    .fops =
    {
        .open = mstar_dip_drv_open,
        .release = mstar_dip_drv_release,
        .unlocked_ioctl = mstar_dip_drv_ioctl,
        .poll = mstar_dipw_drv_poll,
    }
};

static struct class * dip_class;
static char * dip_classname = "mstar_dip_class";

static struct platform_driver Mstar_dip_driver =
{
    .probe 		= mstar_dip_drv_probe,
    .remove 	= mstar_dip_drv_remove,
    .suspend    = mstar_dip_drv_suspend,
    .resume     = mstar_dip_drv_resume,

    .driver = {
        .name	= "mdip",
        .owner  = THIS_MODULE,
    }
};

static u64 sg_mstar_device_dip_dmamask = 0xffffffffUL;

static struct platform_device sg_mdrv_dip_device =
{
    .name = "mdip",
    .id = 0,
    .dev =
    {
        .release = mstar_dip_drv_platfrom_release,
        .dma_mask = &sg_mstar_device_dip_dmamask,
        .coherent_dma_mask = 0xffffffffUL
    }
};
//-------------------------------------------------------------------------------------------------

#define MAX_NUM_DIVP_INST    32

static ut_divp_ChnContex_t gstDivpChnCtx[MAX_NUM_DIVP_INST + 1];

//void pCtx[MAX_NUM_DIVP_INST + 1];
MHAL_DIVP_InputInfo_t g_stDIVPInputInfo;
MHAL_DIVP_OutPutInfo_t g_stDIVPOutputInfo;
MHAL_DIVP_CaptureInfo_t g_stDIVPCaptureInfo;

#ifndef DIP_UT_ENABLE_RIU
    MHAL_CMDQ_CmdqInterface_t *stCmdQInf;
    static MS_U32 g_u32Fence = 0;
#endif
#ifndef DIP_UT_WO_IRQ
    static MS_U32 u32IrqId = 0;
    static MS_BOOL g_bDivpIsrFrameDone = FALSE;
#endif
//-------------------------------------------------------------------------------------------------
// IOCtrl Driver interface functions
//-------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------

#ifndef DIP_UT_WO_IRQ
static irqreturn_t divp_Isr(int nIRQ_ID, void *data)
{
    //clear ISR
    MHAL_DIVP_CleanFrameDoneIsr();
    //schedule_work(divp_isr_waitqueue);

    g_bDivpIsrFrameDone = TRUE;

#ifndef MI_DIVP_FPGA_TEST
    MHAL_DIVP_EnableFrameDoneIsr(TRUE);
#endif
    printk("[divp_Isr]");

    return IRQ_HANDLED;
}
#endif

MS_BOOL DIP_Set_Config(PDIP_CONFIG pCfg)
{
    MS_BOOL bret = TRUE;
#if 0
    XC_DIP_CONFIG_t stDipConfig;

    if(pCfg->bTriggle == FALSE)
    {

        if(pCfg->enTrigMode == DIP_TRIGGER_LOOP)
        {
            //            MApi_XC_Set_Dip_Trig(FALSE);
        }
        else
        {
            //            MApi_XC_DIP_Set_Dwin_Wt_Once(FALSE);
        }
    }
    else
    {

        memset(&stDipConfig, 0, sizeof(XC_DIP_CONFIG_t));

        // clip window
        stDipConfig.bClipEn = pCfg->bClipEn;
        stDipConfig.stClipWin.x = pCfg->stClipWin.x < 1 ? 1 : pCfg->stClipWin.x;
        stDipConfig.stClipWin.y = pCfg->stClipWin.y < 1 ? 1 : pCfg->stClipWin.y;
        stDipConfig.stClipWin.width = pCfg->stClipWin.width > pCfg->u16Width ? pCfg->u16Width : pCfg->stClipWin.width;
        stDipConfig.stClipWin.height = pCfg->stClipWin.height > pCfg->u16Height ? pCfg->u16Height : pCfg->stClipWin.height;


        // pitch
        stDipConfig.u16Pitch = pCfg->u16Width;

        // width/height
        stDipConfig.u16Width = pCfg->u16Width;
        stDipConfig.u16Height = pCfg->u16Height;

        // frame number
        stDipConfig.u8FrameNum = pCfg->u8FrameNum;

        // base address & offset
        stDipConfig.u32BaseAddr = pCfg->u32BuffAddress;
        stDipConfig.u32BaseSize = pCfg->u32BuffSize;

        // C base address & offset
        if(pCfg->enDestFmtType == DIP_DEST_FMT_YC420_MVOP ||
                pCfg->enDestFmtType == DIP_DEST_FMT_YC420_MFE )
        {
            stDipConfig.u32C_BaseAddr = pCfg->u32C_BuffAddress;
            stDipConfig.u32C_BaseSize = pCfg->u32C_BuffSize;
            stDipConfig.bSrc422 = TRUE;
            stDipConfig.b444To422 = TRUE;

        }
        else if(DIP_DEST_FMT_YC422 == pCfg->enDestFmtType)
        {
            stDipConfig.bSrc422   = TRUE;
            stDipConfig.b444To422 = TRUE;
        }
        else
        {
            stDipConfig.bSrc422 = FALSE;
            stDipConfig.b444To422 = FALSE;
        }

        stDipConfig.eDstFMT = pCfg->enDestFmtType == DIP_DEST_FMT_YC422      ? DIP_DST_FMT_YC422 :
                              pCfg->enDestFmtType == DIP_DEST_FMT_ARGB8888   ? DIP_DST_FMT_ARGB8888 :
                              pCfg->enDestFmtType == DIP_DEST_FMT_RGB565     ? DIP_DST_FMT_RGB565 :
                              pCfg->enDestFmtType == DIP_DEST_FMT_YC420_MVOP ? DIP_DST_FMT_YC420_MVOP :
                              DIP_DST_FMT_YC420_MFE;
        stDipConfig.eSrcFMT = DIP_SRC_FMT_OP;
        stDipConfig.bSrcYUV = TRUE;
        stDipConfig.bWOnce = pCfg->enTrigMode == DIP_TRIGGER_ONCE ? TRUE : FALSE;

        DIPDBG("SetDip: clip(%d, %d, %d, %d), w=%d, h:%d, FBnum:%d, Base:%x, Size:%x, CBase:%x, CSize:%x\n",
               stDipConfig.stClipWin.x, stDipConfig.stClipWin.y, stDipConfig.stClipWin.width, stDipConfig.stClipWin.height,
               stDipConfig.u16Width, stDipConfig.u16Height, stDipConfig.u8FrameNum,
               (int)stDipConfig.u32BaseAddr, (int)stDipConfig.u32BaseSize,
               (int)stDipConfig.u32C_BaseAddr, (int)stDipConfig.u32C_BaseSize);
#if 0
        if(MApi_XC_Set_Dip_Config(stDipConfig) == E_APIXC_RET_FAIL)
        {
            bret = FALSE;
        }
        else
        {
            if(pCfg->enTrigMode == DIP_TRIGGER_LOOP)
            {
                MApi_XC_Set_Dip_Trig(TRUE);
            }
            else
            {
                DIPDBG("Wonce On \n");
                MApi_XC_DIP_Set_Dwin_Wt_Once(TRUE);
                MApi_XC_DIP_Set_Dwin_Wt_Once_Trig(TRUE);
            }
        }
#endif

    }
#endif

    return bret;
}


MS_BOOL Dip_Set_WOnce_Base(PDIP_WONCE_BASE_CONFIG pCfg)
{
    MS_BOOL bret = TRUE;

#if 0
    if(pCfg->bTrig)
    {
        if(MApi_XC_DIP_Set_Base(pCfg->u32BuffAddress, pCfg->u32C_BuffAddress))
        {
            MApi_XC_DIP_Set_Dwin_Wt_Once_Trig(TRUE);
        }
        else
        {
            bret = FALSE;
        }
    }
#endif

    return bret;
}



int _MDrv_DIPIO_IOC_Set_Interrupt(struct file *filp, unsigned long arg)
{
    DIP_INTERRUPT_CONFIG stCfg;
    if(copy_from_user(&stCfg, (DIP_INTERRUPT_CONFIG __user *)arg, sizeof(DIP_INTERRUPT_CONFIG)))
    {
        printk("DIP FAIL %d ...\n", __LINE__);
        return -EFAULT;
    }

#if 0
    if(stCfg.bEn  == true && MDrv_DIPW_InterruptIsAttached(0, NULL, NULL) == false )
    {
        MDrv_DIPW_InterruptAttach(0, NULL, NULL);
    }
    else if(stCfg.bEn == false && MDrv_DIPW_InterruptIsAttached(0, NULL, NULL) == true )
    {
        MDrv_DIPW_InterruptDeAttach(0, NULL, NULL);
    }

    if(stCfg.u32DIPW_Signal_PID)
    {
        MDrv_DIPW_Set_Signal_PID(stCfg.u32DIPW_Signal_PID);
    }
#endif

    return 0;
}

int _MDrv_DIPIO_IOC_Set_Dip_Config(struct file *filp, unsigned long arg)
{
    int ret  = 0;
    DIP_CONFIG stCfg;

    if(copy_from_user(&stCfg, (DIP_CONFIG __user *)arg, sizeof(DIP_CONFIG)))
    {
        return -EFAULT;
    }

    if(!DIP_Set_Config(&stCfg))
    {
        ret = -EFAULT;
    }
    return ret;
}


int _MDrv_DIPIO_IOC_Set_Dip_WOnce_Base(struct file *filp, unsigned long arg)
{
    int ret  = 0;
    DIP_WONCE_BASE_CONFIG stCfg;

    if(copy_from_user(&stCfg, (DIP_WONCE_BASE_CONFIG __user *)arg, sizeof(DIP_WONCE_BASE_CONFIG)))
    {
        return -EFAULT;
    }

    if(!Dip_Set_WOnce_Base(&stCfg))
    {
        ret = -EFAULT;
    }

    return ret;
}

int _MDrv_DIPIO_IO_Get_Interrupt_Status(struct file *filp, unsigned long arg)
{
    //DIP_INTR_STATUS stIntrStatus;

#if 0
    memset(&stIntrStatus, 0, sizeof(DIP_INTR_STATUS));
    stIntrStatus.u8DIPW_Status = MDrv_DIPW_Get_ISR_RecFlag();

    if(copy_to_user((DIP_INTR_STATUS __user *)arg, &stIntrStatus, sizeof(DIP_INTR_STATUS)))
    {
        printk("SCA FAIL : %d \n", __LINE__);
        return -EFAULT;
    }
#endif

    return 0;
}

int _MDrv_DIVPIO_IO_Init_Config(struct file *filp, unsigned long arg)
{
    int ret  = 0;
    MS_U32 stCfg;

    if(copy_from_user(&stCfg, (MS_U32 __user *)arg, sizeof(MS_U32)))
    {
        return -EFAULT;
    }

    //printk("[DIP] ====== _MDrv_DIVPIO_IO_Init_Config ======, stCfg = %d\n", stCfg);
    //stCfg = E_MHAL_DIVP_Device0;

    if(!MHAL_DIVP_Init((MHAL_DIVP_DeviceId_e)stCfg))
    {
        ret = -EFAULT;
    }


    return ret;
}


int _MDrv_DIVPIO_IO_Deinit_Config(struct file *filp, unsigned long arg)
{
    int ret  = 0;
    MS_U32 stCfg;

    if(copy_from_user(&stCfg, (MS_U32 __user *)arg, sizeof(MS_U32)))
    {
        return -EFAULT;
    }

    //printk("[DIP] ====== _MDrv_DIVPIO_IO_Deinit_Config ======, stCfg = %d\n", stCfg);
    //stCfg = E_MHAL_DIVP_Device0;

    if(!MHAL_DIVP_DeInit((MHAL_DIVP_DeviceId_e)stCfg))
    {
        ret = -EFAULT;
    }


    return ret;
}

int _MDrv_DIVPIO_IO_Create_Instance(struct file *filp, unsigned long arg)
{
    int ret  = 0;
    IOCTL_DIVP_Instance_t stCfg;

    if(copy_from_user(&stCfg, (IOCTL_DIVP_Instance_t __user *)arg, sizeof(IOCTL_DIVP_Instance_t)))
    {
        return -EFAULT;
    }

    if(!MHAL_DIVP_CreateInstance(stCfg.eId, stCfg.u16MaxWidth, stCfg.u16MaxHeight, NULL, NULL, 0, &gstDivpChnCtx[stCfg.Ctxid].pHalCtx))
    {
        ret = -EFAULT;
    }
    //printk("[DIP] Create_Instance, eId = %d, gstDivpChnCtx[%d].pHalCtx = 0x%x!!! \n", stCfg.eId, stCfg.Ctxid, (int)gstDivpChnCtx[stCfg.Ctxid].pHalCtx);

    return ret;
}

int _MDrv_DIVPIO_IO_Destroy_Instance(struct file *filp, unsigned long arg)
{
    int ret  = 0;
    IOCTL_DIVP_Instance_t stCfg;

    if(copy_from_user(&stCfg, (IOCTL_DIVP_Instance_t __user *)arg, sizeof(IOCTL_DIVP_Instance_t)))
    {
        return -EFAULT;
    }

    //printk("[DIP] Destroy_Instance gstDivpChnCtx[%d].pHalCtx = 0x%x!!! \n", stCfg.Ctxid, (int)gstDivpChnCtx[stCfg.Ctxid].pHalCtx);
    if(!MHAL_DIVP_DestroyInstance(gstDivpChnCtx[stCfg.Ctxid].pHalCtx))
    {
        ret = -EFAULT;
    }

    return ret;
}

extern void HAL_XC_DIP_DumpIPMUXReg(void);

int _MDrv_DIVPIO_IO_Process_DRAM_Data(struct file *filp, unsigned long arg)
{
    IOCTL_DIVP_ProcessDramInfo_t stCfg;
    MS_S32 s32Ret = 0;
#ifndef DIP_UT_WO_IRQ
    MS_U16 u16Count = 0;
#endif
    MS_U32 u32CtxId;
    struct timespec proc_start;

    if(copy_from_user(&stCfg, (IOCTL_DIVP_ProcessDramInfo_t __user *)arg, sizeof(IOCTL_DIVP_ProcessDramInfo_t)))
    {
        return -EFAULT;
    }

    u32CtxId = stCfg.Ctxid;
    memcpy(&g_stDIVPInputInfo, &stCfg.stInputInfo, sizeof(MHAL_DIVP_InputInfo_t));
    memcpy(&g_stDIVPOutputInfo, &stCfg.stOutputInfo, sizeof(MHAL_DIVP_OutPutInfo_t));

    //Chip_Flush_MIU_Pipe();                          //wait for CPU write to mem
    // flush input data L2 cache memory
    //Chip_Flush_Cache_Range(g_stDIVPOutputInfo.u64BufAddr[2], g_stDIVPInputInfo.u32BufSize);

    //printk("[DIP] Process DRAM, gstDivpChnCtx[%d].pHalCtx = 0x%x\n", u32CtxId, (int)gstDivpChnCtx[u32CtxId].pHalCtx);

#ifndef DIP_UT_WO_IRQ
    g_bDivpIsrFrameDone = FALSE;
#endif

#if 1
    {
        getnstimeofday(&proc_start);
    }
#endif

#ifdef DIP_UT_ENABLE_RIU
    s32Ret = MHAL_DIVP_ProcessDramData(gstDivpChnCtx[u32CtxId].pHalCtx, &g_stDIVPInputInfo, &g_stDIVPOutputInfo, NULL);
#else
    // clean CMDQ trigger bus signal and check CMDQ status.
    stCmdQInf->MHAL_CMDQ_ClearTriggerEvent(stCmdQInf);
    s32Ret = MHAL_DIVP_ProcessDramData(gstDivpChnCtx[u32CtxId].pHalCtx, &g_stDIVPInputInfo, &g_stDIVPOutputInfo, stCmdQInf);

    if(0 == s32Ret)
    {
        g_u32Fence += 1;

        if(g_stDIVPOutputInfo.ePxlFmt == E_MHAL_DIVP_PIXEL_FORMAT_YUV_MST_420)
        {
            MHAL_DIVP_WaitCmdQFrameDone(stCmdQInf, E_MHAL_CMDQEVE_S1_MDW_W_DONE);
            //stCmdQInf->MHAL_CMDQ_CmdqAddWaitEventCmd(stCmdQInf, E_MHAL_CMDQEVE_S1_MDW_W_DONE);
        }
        else
        {
            MHAL_DIVP_WaitCmdQFrameDone(stCmdQInf, E_MHAL_CMDQEVE_DIP_TRIG);
            //stCmdQInf->MHAL_CMDQ_CmdqAddWaitEventCmd(stCmdQInf, E_MHAL_CMDQEVE_DIP_TRIG);
        }
        stCmdQInf->MHAL_CMDQ_WriteDummyRegCmdq(stCmdQInf, g_u32Fence);
        stCmdQInf->MHAL_CMDQ_KickOffCmdq(stCmdQInf);
        printk("[Success]add ProcessDram task to CmdQ list, g_u32Fence = %d\n", g_u32Fence);
    }
    else
    {
        printk("[Fail]Fail to process DRAM data!!!\n");
        ///drop added commands
        stCmdQInf->MHAL_CMDQ_CmdqAbortBuffer(stCmdQInf);
    }
#endif

#ifdef DIP_UT_WO_IRQ

    //clear ISR
    MHAL_DIVP_CleanFrameDoneIsr();

#else
    // polling flag "g_bDivpIsrFrameDone" is TRUE or not
    u16Count = 0;
    do
    {
        if(g_stDIVPOutputInfo.ePxlFmt == E_MHAL_DIVP_PIXEL_FORMAT_YUV_MST_420)
        {
            if(MDrv_XC_DIVP_MDWIN_GetIntrStatus(NULL) > 0)
                break;
        }
        else
        {
            if(g_bDivpIsrFrameDone == TRUE)
                break;
        }
        u16Count++;
        msleep_interruptible(1);  // sleep 1 ms
    }
    while(u16Count < 100);

    printk("[Cnt = %d]\n", u16Count);
    if(u16Count >= 100)
    {
        //clear ISR
        MHAL_DIVP_CleanFrameDoneIsr();
        printk("[ERROR] Process One Frame time out....\n");
    }
    else
    {
        if(g_stDIVPOutputInfo.ePxlFmt == E_MHAL_DIVP_PIXEL_FORMAT_YUV_MST_420)
        {
            MHAL_DIVP_CleanFrameDoneIsr();
        }
    }

#endif

#if 1 //
    {
        MS_U32 delta;
        struct timespec ct;

        getnstimeofday(&ct);
        delta = ct.tv_nsec + ((proc_start.tv_sec != ct.tv_sec) ? NSEC_PER_SEC : 0)
                - proc_start.tv_nsec;

        printk("process time = %d us\n", (int)(delta / 1000));
    }
#endif

    return s32Ret;
}

int _MDrv_DIVPIO_IO_Capture_Timing(struct file *filp, unsigned long arg)
{
    IOCTL_DIVP_CaptureInfo_t stCfg;
    MS_S32 s32Ret = 0;
#ifndef DIP_UT_WO_IRQ
    MS_U16 u16Count = 0;
#endif
    struct timespec proc_start;

    if(copy_from_user(&stCfg, (IOCTL_DIVP_CaptureInfo_t __user *)arg, sizeof(IOCTL_DIVP_CaptureInfo_t)))
    {
        return -EFAULT;
    }

    memcpy(&g_stDIVPCaptureInfo, &stCfg.stCaptureInfo, sizeof(MHAL_DIVP_CaptureInfo_t));

    //printk("[DIP] Capture Timing, gstDivpChnCtx[%d].pHalCtx = 0x%x\n", stCfg.Ctxid, (int)gstDivpChnCtx[stCfg.Ctxid].pHalCtx);

#ifndef DIP_UT_WO_IRQ
    g_bDivpIsrFrameDone = FALSE;
#endif

#if 1
    {
        getnstimeofday(&proc_start);
    }
#endif

#ifdef DIP_UT_ENABLE_RIU
    s32Ret = MHAL_DIVP_CaptureTiming(gstDivpChnCtx[stCfg.Ctxid].pHalCtx, &g_stDIVPCaptureInfo, NULL);
#else
    // clean CMDQ trigger bus signal and check CMDQ status.
    stCmdQInf->MHAL_CMDQ_ClearTriggerEvent(stCmdQInf);
    s32Ret = MHAL_DIVP_CaptureTiming(gstDivpChnCtx[stCfg.Ctxid].pHalCtx, &g_stDIVPCaptureInfo, stCmdQInf);

    if(0 == s32Ret)
    {
        g_u32Fence += 1;
        MHAL_DIVP_WaitCmdQFrameDone(stCmdQInf, E_MHAL_CMDQEVE_DIP_TRIG);
        //stCmdQInf->MHAL_CMDQ_CmdqAddWaitEventCmd(stCmdQInf, E_MHAL_CMDQEVE_DIP_TRIG);
        stCmdQInf->MHAL_CMDQ_WriteDummyRegCmdq(stCmdQInf, g_u32Fence);
        stCmdQInf->MHAL_CMDQ_KickOffCmdq(stCmdQInf);
        printk("[Success]add Capture task to CmdQ list, g_u32Fence = %d\n", g_u32Fence);
    }
    else
    {
        printk("[Fail]Fail to process capture display screen task!!!\n");
        ///drop added commands
        stCmdQInf->MHAL_CMDQ_CmdqAbortBuffer(stCmdQInf);
    }
#endif

#ifdef DIP_UT_WO_IRQ

    //clear ISR
    MHAL_DIVP_CleanFrameDoneIsr();

#else
    // polling flag "g_bDivpIsrFrameDone" is TRUE or not
    u16Count = 0;
    do
    {
        if(g_bDivpIsrFrameDone == TRUE)
            break;
        u16Count++;
        msleep_interruptible(1);  // sleep 1 ms
    }
    while(u16Count < 100);

    printk("[Cnt = %d]\n", u16Count);
    if(u16Count >= 100)
    {
        //clear ISR
        MHAL_DIVP_CleanFrameDoneIsr();
        printk("[ERROR] Capture One Frame time out....\n");
    }

#endif

#if 1 //
    {
        MS_U32 delta;
        struct timespec ct;

        getnstimeofday(&ct);
        delta = ct.tv_nsec + ((proc_start.tv_sec != ct.tv_sec) ? NSEC_PER_SEC : 0)
                - proc_start.tv_nsec;

        printk("capture time = %d us\n", (int)(delta / 1000));
        HAL_XC_DIP_DumpIPMUXReg();
    }
#endif

    return s32Ret;
}

int _MDrv_DIVPIO_IO_Set_Attr_Tnr(struct file *filp, unsigned long arg)
{
    int ret  = 0;
    IOCTL_DIVP_TnrLevel_t stCfg;
    MHAL_DIVP_TnrLevel_e eDIVPTnrAttr = 0;

    if(copy_from_user(&stCfg, (IOCTL_DIVP_TnrLevel_t __user *)arg, sizeof(IOCTL_DIVP_TnrLevel_t)))
    {
        return -EFAULT;
    }

    eDIVPTnrAttr = stCfg.eTnrLevel;
    MHAL_DIVP_SetAttr(gstDivpChnCtx[stCfg.Ctxid].pHalCtx, E_MHAL_DIVP_ATTR_TNR, &eDIVPTnrAttr, NULL);

    //printk("[DIP] Set Attr TNR, gstDivpChnCtx[%d].pHalCtx = 0x%x, value = %d!!! \n", stCfg.Ctxid, (int)gstDivpChnCtx[stCfg.Ctxid].pHalCtx, eDIVPTnrAttr);

    return ret;
}

int _MDrv_DIVPIO_IO_Set_Attr_DiType(struct file *filp, unsigned long arg)
{
    int ret  = 0;
    IOCTL_DIVP_DiType_t stCfg;
    MHAL_DIVP_DiType_e eDIVPDiAttr = 0;

    if(copy_from_user(&stCfg, (IOCTL_DIVP_DiType_t __user *)arg, sizeof(IOCTL_DIVP_DiType_t)))
    {
        return -EFAULT;
    }

    eDIVPDiAttr = stCfg.eDiType;
    MHAL_DIVP_SetAttr(gstDivpChnCtx[stCfg.Ctxid].pHalCtx, E_MHAL_DIVP_ATTR_DI, &eDIVPDiAttr, NULL);

    //printk("[DIP] Set Attr DITYPE, gstDivpChnCtx[%d].pHalCtx = 0x%x, value = %d!!! \n", stCfg.Ctxid, (int)gstDivpChnCtx[stCfg.Ctxid].pHalCtx, eDIVPDiAttr);

    return ret;
}

int _MDrv_DIVPIO_IO_Set_Attr_Rotate(struct file *filp, unsigned long arg)
{
    int ret  = 0;
    IOCTL_DIVP_Rotate_t stCfg;
    MHAL_DIVP_Rotate_e eDIVPRotAttr = 0;

    if(copy_from_user(&stCfg, (IOCTL_DIVP_Rotate_t __user *)arg, sizeof(IOCTL_DIVP_Rotate_t)))
    {
        return -EFAULT;
    }

    eDIVPRotAttr = stCfg.eRotateType;
    MHAL_DIVP_SetAttr(gstDivpChnCtx[stCfg.Ctxid].pHalCtx, E_MHAL_DIVP_ATTR_ROTATE, &eDIVPRotAttr, NULL);

    //printk("[DIP] Set Attr ROTATE, gstDivpChnCtx[%d].pHalCtx = 0x%x, value = %d!!! \n", stCfg.Ctxid, (int)gstDivpChnCtx[stCfg.Ctxid].pHalCtx, eDIVPRotAttr);

    return ret;
}

int _MDrv_DIVPIO_IO_Set_Attr_Window(struct file *filp, unsigned long arg)
{
    int ret  = 0;
    IOCTL_DIVP_Window_t stCfg;
    MHAL_DIVP_Window_t stDIVPWinAttr = {0};

    if(copy_from_user(&stCfg, (IOCTL_DIVP_Window_t __user *)arg, sizeof(IOCTL_DIVP_Window_t)))
    {
        return -EFAULT;
    }

    stDIVPWinAttr.u16X = stCfg.stCropWin.u16X;
    stDIVPWinAttr.u16Y = stCfg.stCropWin.u16Y;
    stDIVPWinAttr.u16Width = stCfg.stCropWin.u16Width;
    stDIVPWinAttr.u16Height = stCfg.stCropWin.u16Height;
    MHAL_DIVP_SetAttr(gstDivpChnCtx[stCfg.Ctxid].pHalCtx, E_MHAL_DIVP_ATTR_CROP, &stDIVPWinAttr, NULL);

    //printk("[DIP] Set Attr TNR, gstDivpChnCtx[%d].pHalCtx = 0x%x!!! \n", stCfg.Ctxid, (int)gstDivpChnCtx[stCfg.Ctxid].pHalCtx);

    return ret;
}

int _MDrv_DIVPIO_IO_Set_Attr_Mirror(struct file *filp, unsigned long arg)
{
    int ret  = 0;
    IOCTL_DIVP_Mirror_t stCfg;
    MHAL_DIVP_Mirror_t stDIVPMirrorAttr = {0};

    if(copy_from_user(&stCfg, (IOCTL_DIVP_Mirror_t __user *)arg, sizeof(IOCTL_DIVP_Mirror_t)))
    {
        return -EFAULT;
    }

    stDIVPMirrorAttr.bHMirror = stCfg.stMirror.bHMirror;
    stDIVPMirrorAttr.bVMirror = stCfg.stMirror.bVMirror;
    MHAL_DIVP_SetAttr(gstDivpChnCtx[stCfg.Ctxid].pHalCtx, E_MHAL_DIVP_ATTR_MIRROR, &stDIVPMirrorAttr, NULL);

    //printk("[DIP] Set Attr TNR, gstDivpChnCtx[%d].pHalCtx = 0x%x!!! \n", stCfg.Ctxid, (int)gstDivpChnCtx[stCfg.Ctxid].pHalCtx);

    return ret;
}

int _MDrv_DIVPIO_IO_Set_Cover(struct file *filp, unsigned long arg)
{
    int ret  = 0;
    IOCTL_DIVP_Cover_t stCfg;
    DIP_COVER_PROPERTY stDIVPCoverProperty = {0};

    if(copy_from_user(&stCfg, (IOCTL_DIVP_Cover_t __user *)arg, sizeof(IOCTL_DIVP_Cover_t)))
    {
        return -EFAULT;
    }

    memcpy(&stDIVPCoverProperty, &stCfg.stCoverProperty, sizeof(DIP_COVER_PROPERTY));

#ifdef DIP_UT_ENABLE_RIU
    MDrv_XC_DIVP_COVER_SetConfig(NULL, &stDIVPCoverProperty);
#else
    MDrv_XC_DIVP_COVER_SetConfig(stCmdQInf, &stDIVPCoverProperty);
#endif

    return ret;
}

//==============================================================================
long mstar_dip_drv_ioctl(struct file *filp, unsigned int u32Cmd, unsigned long u32Arg)
{
    //    int err = 0;
    int retval = 0;

    if(_devDIP.refCnt <= 0)
    {
        printk("[DIP] DIPIO_IOCTL refCnt =%d!!! \n", _devDIP.refCnt);
        return -EFAULT;
    }
    /* check u32Cmd valid */
    if(IOCTL_DIP_MAGIC == _IOC_TYPE(u32Cmd))
    {
        if(_IOC_NR(u32Cmd) >= IOCTL_DIP_MAX_NR)
        {
            printk("[DIP] IOCtl NR Error!!! (Cmd=%x)\n", u32Cmd);
            return -ENOTTY;
        }
    }
    else
    {
        printk("[DIP] IOCtl MAGIC Error!!! (Cmd=%x)\n", u32Cmd);
        return -ENOTTY;
    }
#if 0
    /* verify Access */
    if (_IOC_DIR(u32Cmd) & _IOC_READ)
    {
        err = !access_ok(VERIFY_WRITE, (void __user *)u32Arg, _IOC_SIZE(u32Cmd));
    }
    else if (_IOC_DIR(u32Cmd) & _IOC_WRITE)
    {
        err =  !access_ok(VERIFY_READ, (void __user *)u32Arg, _IOC_SIZE(u32Cmd));
    }
    if (err)
    {
        return -EFAULT;
    }
#endif

    /* not allow query or command once driver suspend */

    //printk("[DIP] IOCTL: ==%s==\n", (CMD_PARSING(u32Cmd)));
    //printk("[DIP] === DIPIO_IOCTL %d === \n", (u32Cmd));

    switch(u32Cmd)
    {
        case IOCTL_DIP_SET_INTERRUPT:
            retval = _MDrv_DIPIO_IOC_Set_Interrupt(filp, u32Arg);
            break;

        case IOCTL_DIP_SET_CONFIG:
            retval = _MDrv_DIPIO_IOC_Set_Dip_Config(filp, u32Arg);
            break;

        case IOCTL_DIP_SET_WONCE_BASE_CONFIG:
            retval = _MDrv_DIPIO_IOC_Set_Dip_WOnce_Base(filp, u32Arg);
            break;

        case IOCTL_DIP_GET_INTERRUPT_STATUS:
            retval = _MDrv_DIPIO_IO_Get_Interrupt_Status(filp, u32Arg);
            break;

        case IOCTL_DIVP_INIT_CONFIG:
            retval = _MDrv_DIVPIO_IO_Init_Config(filp, u32Arg);
            break;

        case IOCTL_DIVP_DEINIT_CONFIG:
            retval = _MDrv_DIVPIO_IO_Deinit_Config(filp, u32Arg);
            break;

        case IOCTL_DIVP_CREATE_INST:
            retval = _MDrv_DIVPIO_IO_Create_Instance(filp, u32Arg);
            break;

        case IOCTL_DIVP_DESTROY_INST:
            retval = _MDrv_DIVPIO_IO_Destroy_Instance(filp, u32Arg);
            break;

        case IOCTL_DIVP_PROCESS_DRAM_DATA:
            retval = _MDrv_DIVPIO_IO_Process_DRAM_Data(filp, u32Arg);
            break;

        case IOCTL_DIVP_CAPTURE_TIMING:
            retval = _MDrv_DIVPIO_IO_Capture_Timing(filp, u32Arg);
            break;

        case IOCTL_DIVP_SET_ATTR_TNR:
            retval = _MDrv_DIVPIO_IO_Set_Attr_Tnr(filp, u32Arg);
            break;

        case IOCTL_DIVP_SET_ATTR_DITYPE:
            retval = _MDrv_DIVPIO_IO_Set_Attr_DiType(filp, u32Arg);
            break;

        case IOCTL_DIVP_SET_ATTR_ROTATE:
            retval = _MDrv_DIVPIO_IO_Set_Attr_Rotate(filp, u32Arg);
            break;

        case IOCTL_DIVP_SET_ATTR_WINDOW:
            retval = _MDrv_DIVPIO_IO_Set_Attr_Window(filp, u32Arg);
            break;

        case IOCTL_DIVP_SET_ATTR_MIRROR:
            retval = _MDrv_DIVPIO_IO_Set_Attr_Mirror(filp, u32Arg);
            break;

        case IOCTL_DIVP_SET_COVER:
            retval = _MDrv_DIVPIO_IO_Set_Cover(filp, u32Arg);
            break;

        default:  /* redundant, as cmd was checked against MAXNR */
            printk("[DIP] ERROR IOCtl number %x\n ", u32Cmd);
            retval = -ENOTTY;
            break;
    }

    return retval;
}


static unsigned int mstar_dipw_drv_poll(struct file *filp, struct poll_table_struct *wait)
{
#if 0
    if (atomic_read(&DIPW_intr_count) > 0)
    {
        atomic_set(&DIPW_intr_count, 0);
        return POLLIN | POLLRDNORM; /* readable */
    }
#endif
    return 0;
}

static int mstar_dip_drv_probe(struct platform_device *pdev)
{
#ifndef DIP_UT_ENABLE_RIU
    MHAL_CMDQ_BufDescript_t stCmdqBufDesp;
    MS_S32 u32CmdqNum = 0;
#endif
#ifndef DIP_UT_WO_IRQ
    MS_S32 s32IrqRet = 0;
#endif

    printk("[DIP] mstar_dip_drv_probe\n");

#ifndef DIP_UT_ENABLE_RIU
    memset(&stCmdqBufDesp, 0, sizeof(stCmdqBufDesp));
    stCmdqBufDesp.u32CmdqBufSize = 4096 * 8;
    stCmdqBufDesp.u32CmdqBufSizeAlign = 32;//256bit allgin
    stCmdqBufDesp.u32MloadBufSize = 0;
    stCmdqBufDesp.u16MloadBufSizeAlign = 32;
    stCmdQInf = MHAL_CMDQ_GetSysCmdqService(E_MHAL_CMDQ_ID_DIVP, &stCmdqBufDesp, FALSE);
    u32CmdqNum = stCmdQInf->MHAL_CMDQ_CheckBufAvailable(stCmdQInf, 256);//0x800 = 256 commands * 8 bytes/command
    printk("[CmdQ]MHAL_CMDQ_GetSysCmdqService handle: 0x%08X, CmdqNum = %d\n", (MS_U32)stCmdQInf, u32CmdqNum);
#endif

#ifndef DIP_UT_WO_IRQ
    MHAL_DIVP_GetIrqNum(&u32IrqId);
    printk("u32IrqId = %d.\n", u32IrqId);
    s32IrqRet = request_irq(u32IrqId, divp_Isr, NULL, "divp_isr", NULL);
    if(0 != s32IrqRet)
    {
        printk("request_irq failed. u32IrqId = %u, s32IrqRet = %d.\n\n ", u32IrqId, s32IrqRet);
    }
    //MHAL_DIVP_EnableFrameDoneIsr(TRUE);
    disable_irq(u32IrqId);
    enable_irq(u32IrqId);
#endif

    return 0;
}

static int mstar_dip_drv_remove(struct platform_device *pdev)
{
    printk("[DIP] mstar_dip_drv_remove\n");

#ifndef DIP_UT_ENABLE_RIU
    printk("[CmdQ]MHAL_CMDQ_ReleaseSysCmdqService\n");
    MHAL_CMDQ_ReleaseSysCmdqService(E_MHAL_CMDQ_ID_DIVP);
#endif

#ifndef DIP_UT_WO_IRQ
    //MHAL_DIVP_EnableFrameDoneIsr(FALSE);
    disable_irq(u32IrqId);
    free_irq(u32IrqId, NULL);
#endif

    return 0;
}


static int mstar_dip_drv_suspend(struct platform_device *dev, pm_message_t state)
{
    printk("[DIP] mstar_dip_drv_suspend\n");


    return 0;
}


static int mstar_dip_drv_resume(struct platform_device *dev)
{
    printk("[DIP] mstar_dip_drv_resume\n");


    return 0;
}

static void mstar_dip_drv_platfrom_release(struct device *device)
{
    printk("[DIP] mstar_dip_drv_platfrom_release\n");

}


int mstar_dip_drv_open(struct inode *inode, struct file *filp)
{
    //printk("[DIP] DIP DRIVER OPEN\n");

    DIPIO_ASSERT(_devDIP.refCnt >= 0);
    _devDIP.refCnt++;

    printk("[DIP] Open refCnt = %d\n", _devDIP.refCnt);


    return 0;
}


int mstar_dip_drv_release(struct inode *inode, struct file *filp)
{

    printk("[DIP] DIP DRIVER RELEASE\n");
    _devDIP.refCnt--;
    DIPIO_ASSERT(_devDIP.refCnt >= 0);

    //    free_irq(INT_IRQ_DIPW, MDrv_DIPW_isr);

    return 0;
}


//-------------------------------------------------------------------------------------------------
// Module functions
//-------------------------------------------------------------------------------------------------
int _MDrv_DIPIO_ModuleInit(void)
{
    int ret = 0;
    dev_t  dev;
    int s32Ret;
    printk("[DIPIO]_Init \n");

    if(_devDIP.s32Major)
    {
        dev = MKDEV(_devDIP.s32Major, _devDIP.s32Minor);
        s32Ret = register_chrdev_region(dev, MDRV_DIP_DEVICE_COUNT, MDRV_DIP_NAME);
    }
    else
    {
        s32Ret = alloc_chrdev_region(&dev, _devDIP.s32Minor, MDRV_DIP_DEVICE_COUNT, MDRV_DIP_NAME);
        _devDIP.s32Major = MAJOR(dev);
    }

    if (0 > s32Ret)
    {
        printk("[DIP] Unable to get major %d\n", _devDIP.s32Major);
        return s32Ret;
    }

    cdev_init(&_devDIP.cdev, &_devDIP.fops);
    if (0 != (s32Ret = cdev_add(&_devDIP.cdev, dev, MDRV_DIP_DEVICE_COUNT)))
    {
        printk("[DIP] Unable add a character device\n");
        unregister_chrdev_region(dev, MDRV_DIP_DEVICE_COUNT);
        return s32Ret;
    }

    dip_class = class_create(THIS_MODULE, dip_classname);
    if(IS_ERR(dip_class))
    {
        printk(KERN_WARNING"Failed at class_create().Please exec [mknod] before operate the device/n");
    }
    else
    {
        device_create(dip_class, NULL, dev, NULL, "mdip");
    }
    /* initial the whole DIP Driver */
    ret = platform_driver_register(&Mstar_dip_driver);

    if (!ret)
    {
        ret = platform_device_register(&sg_mdrv_dip_device);
        if (ret)    /* if register device fail, then unregister the driver.*/
        {
            platform_driver_unregister(&Mstar_dip_driver);
            printk("[DIP] register failed\n");

        }
        else
        {
            printk("[DIP] register success\n");
        }
    }


    return ret;
}


void _MDrv_DIPIO_ModuleExit(void)
{
    /*de-initial the who GFLIPDriver */
    printk("[DIPIO]_Exit \n");
    cdev_del(&_devDIP.cdev);
    device_destroy(dip_class, MKDEV(_devDIP.s32Major, _devDIP.s32Minor));
    class_destroy(dip_class);
    unregister_chrdev_region(MKDEV(_devDIP.s32Major, _devDIP.s32Minor), MDRV_DIP_DEVICE_COUNT);
    platform_driver_unregister(&Mstar_dip_driver);
}


module_init(_MDrv_DIPIO_ModuleInit);
module_exit(_MDrv_DIPIO_ModuleExit);

MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("MS DIP driver");
MODULE_LICENSE("GPL");


