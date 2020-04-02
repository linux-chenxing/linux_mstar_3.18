#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/spinlock.h>

#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/poll.h>
#include <linux/sched.h>

#include <linux/clk.h>
#include <linux/clk-provider.h>

#include <linux/mm.h>
#include <linux/dma-mapping.h>

#include "mdrv_cwa.h"
#include "mdrv_cwa_io.h"
#include "mdrv_cwa_io_st.h"
#include "drv_cwa.h"

#define MDRV_CWA_DEVICE_COUNT                   (1) // How many device will be installed
#define MDRV_CWA_NAME           "mstar_cwa"
#define MDRV_CWA_MINOR                          (0)
#define MDRV_CWA_CLASS_NAME     "mstar_cwa_class"
/*******************************************************************************************************************
 * mdrv_cwa_drv_isr_post_proc
 *   Post process of ISR
 *
 * Parameters:
 *   wq:
 *
 *
 * Return:
 *
 */
void mdrv_cwa_drv_isr_post_proc(struct work_struct *wq)
{
    cwa_dev_data  *dev_data;
    cwa_file_data *file_data;

    //get device data
    dev_data = (cwa_dev_data*)container_of(wq, cwa_dev_data, work_queue);

    // Enter cirtical section
    mutex_lock(&dev_data->mutex);

    //Post process
    file_data = cwa_drv_post_process(&dev_data->drv_handle);

    //Disable clok
//    mdrv_cwa_clock_dis(dev_data);

    // Leave critical section
    mutex_unlock(&dev_data->mutex);

    if (file_data == NULL)
    {
        CWA_MSG(CWA_MSG_ERR, "isr post process get NULL of file_data!!\n");
        return;
    }

    CWA_MSG(CWA_MSG_DBG, "post porcess 0x%p\n", &file_data->wait_queue);

    // set ready and wake up waiting thread/process
    wake_up_interruptible(&file_data->wait_queue);

}
/*******************************************************************************************************************
 * mdrv_cwa_drv_isr
 *   ISR handler
 *
 * Parameters:
 *   irq:      IRQ
 *   dev_data: Device data which is assigned from request_irq()
 *
 * Return:
 *   Always IRQ_HANDLED to stop parsing ISR
 */
irqreturn_t  mdrv_cwa_drv_isr(s32 irq, void* data)
{
    cwa_dev_data  *dev_data = (cwa_dev_data*)data;
    CWA_DRV_STATE  state;

    state = cwa_drv_isr_handler(irq, &dev_data->drv_handle); //check and clear IRQ

    switch(state)
    {
        case CWA_DRV_STATE_DONE:
            INIT_WORK(&dev_data->work_queue, mdrv_cwa_drv_isr_post_proc);
            schedule_work(&dev_data->work_queue);
            return IRQ_HANDLED;

        default:
            return IRQ_NONE;
    }

    return IRQ_NONE;
}
/*******************************************************************************************************************
 * mdrv_cwa_drv_open
 *   File open handler
 *   The device can has a instance at the same time, and the open
 *   operator also enable the clock and request q ISR.
 *
 * Parameters:
 *   inode: inode
 *   filp:  file structure
 *
 * Return:
 *   standard return value
 */

static s32 mdrv_cwa_drv_open(struct inode *inode, struct file *filp)
{
    cwa_dev_data    *dev_data;
    cwa_file_data  *file_data;
    s32 err = 0;

    //get dev_data struct pointer
    dev_data = container_of(inode->i_cdev, cwa_dev_data, cdev);

    //allocate buffer for file_data
    file_data = devm_kcalloc(&dev_data->pdev->dev, 1, sizeof(cwa_file_data), GFP_KERNEL);
    if (file_data == NULL)
    {
         CWA_MSG(CWA_MSG_ERR, "error: can't allocate buffer\n");
         return -ENOSPC; //?
    }

    CWA_MSG(CWA_MSG_DBG, "filp: 0x%p, file_data: 0x%p\n", filp, file_data);

     //Assgin dev_data and keep file_data in the file structure
    file_data->state = CWA_FILE_STATE_READY;
    file_data->dev_data = dev_data;

    //save file data to private
    filp->private_data = file_data;

    //Init wait queue
    init_waitqueue_head(&file_data->wait_queue);

//     Enable clock
//    err = mdrv_cwa_clock_en(dev_data);
//    if (err != 0)
//    {
//        err = -EIO; //?
//        goto ERROR_1;
//    }
//    return 0;
//ERROR_1:
//    devm_kfree(&dev_data->pdev->dev, file_data);

    return err;
}
/*******************************************************************************************************************
 * mdrv_cwa_drv_release
 *   File close handler
 *   The operator will release clock & ISR
 *
 * Parameters:
 *   inode: inode
 *   filp:  file structure
 *
 * Return:
 *   standard return value
 */
static s32 mdrv_cwa_drv_release(struct inode *inode, struct file *filp)
{
    cwa_file_data *file_data;
    cwa_dev_data  *dev_data ;

    file_data = (cwa_file_data*)filp->private_data;
    dev_data = file_data->dev_data;

    CWA_MSG(CWA_MSG_DBG, "filp: 0x%p\n", filp);

    // Release memory
    devm_kfree(&dev_data->pdev->dev, file_data);

   //Need to relase clock & ISR?

    return 0;
}
/*******************************************************************************************************************
 * mdrv_cwa_drv_ioctl_process
 *   IOCTL handler for CWA_IOC_PROCESS
 *
 * Parameters:
 *   file_data: file private data
 *   arg:       argument, a pointer of cwa_ioc_config from userspace
 *
 * Return:
 *   CWA_IOC_RET
 */
static CWA_IOC_ERROR mdrv_cwa_drv_ioctl_process(cwa_file_data *file_data, unsigned long arg)
{
    unsigned long proc_config_status = 0;
    CWA_IOC_ERROR ret = CWA_STATUS_OK;

    if(file_data->state != CWA_FILE_STATE_READY)
    {
        CWA_MSG(CWA_MSG_ERR, "One file can request once at the same time only\n");
        return CWA_STATUS_BUSY;
    }

    //get config setting from user space
    proc_config_status = copy_from_user(&file_data->ioc_config, (void*)arg, sizeof(cwa_ioc_config) );
    if(proc_config_status != 0)
    {
        CWA_MSG(CWA_MSG_ERR, "Can't copy config from user space\n");
        return CWA_STATUS_PROC_CONFIG_ERROR;
    }

    //config pointer of user space
    file_data->user_io_config = (cwa_ioc_config*)arg;

    //debug
    //image buffer
    //debug_hal_buffer_check((unsigned char*)file_data->ioc_config.input_data.p_data[CWA_IMAGE_PLANE_Y]);

    //ceva warp process
    ret = cwa_drv_process(&(file_data->dev_data->drv_handle),  file_data);

    return ret;

}
/*******************************************************************************************************************
 * mdrv_cwa_drv_ioctl
 *   IOCTL handler entry for file operator
 *
 * Parameters:
 *   filp: pointer of file structure
 *   cmd:  command
 *   arg:  argument from user space
 *
 * Return:
 *   standard return value
 */
long mdrv_cwa_drv_ioctl(struct file *filp, u32 cmd, unsigned long arg)
{
    cwa_file_data *file_data;
    cwa_dev_data  *dev_data;
    CWA_IOC_ERROR err = CWA_STATUS_OK;
    //s32 clk_status = 0;

    //get file_data and device_data struct pointer
    file_data = (cwa_file_data*)filp->private_data;
    dev_data = file_data->dev_data;

    CWA_MSG(CWA_MSG_DBG, "filp: 0x%p, file_data: 0x%p\n", filp, file_data);

    // Enter critical section
    mutex_lock(&dev_data->mutex);

    //clock enable
//    clk_status = mdrv_cwa_clock_en(dev_data);
//    if(clk_status)
//    {
//        err = CWA_STATUS_CLK_ERROR;
//        goto  RETURN;
//    }

    switch(cmd)
    {
        case CWA_IOC_PROCESS:
                err = mdrv_cwa_drv_ioctl_process(file_data, arg);
                break;

        default:
                err = ESRCH;
                break;
    }

//RETURN:
    // Leave critical section
    mutex_unlock(&dev_data->mutex);

    return err;
}
/*******************************************************************************************************************
 * mdrv_cwa_drv_ioctl
 *   poll handler entry for file operator
 *
 * Parameters:
 *   filp: pointer of file structure
 *   wait: wait queue
 *
 * Return:
 *   only 0 or POLLIN | POLLRDNORM
 */
static u32 mdrv_cwa_drv_poll(struct file *filp, struct poll_table_struct *wait)
{
    cwa_file_data *file_data ;

    file_data = (cwa_file_data*)filp->private_data;

    CWA_MSG(CWA_MSG_DBG, "polling 0x%p 0x%X\n", &file_data->wait_queue, file_data->state);

    //no need pending for HW
    if (file_data->state == CWA_FILE_STATE_READY)
    {
        return POLLIN | POLLRDNORM;
    }

    poll_wait(filp, &file_data->wait_queue, wait); //add file_data to wait list, then block until HW finish (isr_post_process)

    switch(file_data->state )
    {
        case CWA_FILE_STATE_DONE:

            CWA_MSG(CWA_MSG_DBG, "Operation ready\n");
            file_data->state = CWA_FILE_STATE_READY;

            return POLLIN | POLLRDNORM;

        case CWA_FILE_STATE_AXI_ERROR:

            CWA_MSG(CWA_MSG_ERR, "AXI error occurs\n");
            file_data->state = CWA_FILE_STATE_READY;

            return POLLERR;

        default:

            break;

    }//switch(status)
    return 0;
}
//-------------------------------------------------------------------------------------------------
// Platform functions
//-------------------------------------------------------------------------------------------------
// Use a struct to gather all global variable
static struct
{
    s32 major;                      // cdev major number
    s32 minor_star;             // begining of cdev minor number
    s32 reg_count;              // registered count
    struct class *class;  //class pointer
} g_cwa_drv = {0, 0, 0, NULL};

static const struct file_operations cwa_fops = {
    .owner          = THIS_MODULE,
    .open           = mdrv_cwa_drv_open,
    .release        = mdrv_cwa_drv_release,
    .unlocked_ioctl = mdrv_cwa_drv_ioctl,
    .poll           = mdrv_cwa_drv_poll,
};
/*******************************************************************************************************************
 * mdrv_cwa_drv_probe
 *   Platform device prob handler
 *
 * Parameters:
 *   pdev: platfrom device
 *
 * Return:
 *   standard return value
 */
static s32 mdrv_cwa_drv_probe(struct platform_device *pdev)
{
    s32 err;
    s32 init_status = 0;
    cwa_dev_data *dev_data;
    struct resource *res0;
    struct resource *res_sys, *res_axi2miu0, *res_axi2miu1, *res_axi2miu2, *res_axi2miu3;
    struct device *dev;

    // create drv data buffer
    dev_data = devm_kcalloc(&pdev->dev, 1, sizeof(cwa_dev_data), GFP_KERNEL);
    if (dev_data == NULL)
    {
        CWA_MSG(CWA_MSG_ERR, "can't allocate dev data buffer\n");
        return -ENOMEM;
    }
    CWA_MSG(CWA_MSG_DBG, "dev_data: 0x%p (size = %d)\n", dev_data, sizeof(cwa_dev_data));

    // Get base address
    res0 = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    res_sys = platform_get_resource(pdev, IORESOURCE_MEM, 1);
    res_axi2miu0 = platform_get_resource(pdev, IORESOURCE_MEM, 2);
    res_axi2miu1 = platform_get_resource(pdev, IORESOURCE_MEM, 3);
    res_axi2miu2 = platform_get_resource(pdev, IORESOURCE_MEM, 4);
    res_axi2miu3 = platform_get_resource(pdev, IORESOURCE_MEM, 5);

    if ((res0 == NULL) ||(res_sys == NULL) || (res_axi2miu0 == NULL) || (res_axi2miu1 == NULL) || (res_axi2miu2 == NULL) || (res_axi2miu3 == NULL))
    {
        CWA_MSG(CWA_MSG_ERR, "can't find base address\n");
        err = -ENODEV;
        goto ERROR_1;
    }
    //Inti mutex
    mutex_init(&dev_data->mutex);

    // Init dev_data
    dev_data->pdev = pdev;
    init_status = cwa_drv_init(&dev_data->drv_handle, pdev,
                                res0->start,
                                res_sys->start,
                                res_axi2miu0->start,
                                res_axi2miu1->start,
                                res_axi2miu2->start,
                                res_axi2miu3->start);

    if (init_status < 0)
    {
        CWA_MSG(CWA_MSG_ERR, "can't init driver\n");
        err = -ENODEV;
        goto ERROR_1;
    }

    // Init clock
//    init_status = mdrv_cwa_clock_init(dev_data);
    init_status = clk_hal_init();

    if (init_status )
    {
        CWA_MSG(CWA_MSG_ERR, "can't init clock\n");
        err = -ENODEV;
        goto ERROR_1;
    }

    // Retrieve IRQ
    dev_data->irq = irq_of_parse_and_map(pdev->dev.of_node, 0);
    if (dev_data->irq < 0)
    {
        CWA_MSG(CWA_MSG_ERR, "can't find IRQ\n");
        err = -ENODEV;
        goto ERROR_2;
    }

    // Register a ISR
    err = request_irq(dev_data->irq, mdrv_cwa_drv_isr, IRQ_TYPE_LEVEL_HIGH, "cwa isr", dev_data);
    if (err != 0)
    {
        CWA_MSG(CWA_MSG_ERR, "cwa interrupt failed (irq: %d, errno:%d)\n", dev_data->irq, err);
        err = -ENODEV;
        goto ERROR_2;
    }

    //enable warp IRQ for ceva wrapper
    err = cwa_drv_enable_irq(&dev_data->drv_handle);
    if (err < 0)
    {
        CWA_MSG(CWA_MSG_ERR, "can't enable IRQ\n");
        err = -ENODEV;
        goto ERROR_2;
    }

    // Add cdev
    cdev_init(&dev_data->cdev, &cwa_fops);
    err= cdev_add(&dev_data->cdev, MKDEV(g_cwa_drv.major, g_cwa_drv.minor_star + g_cwa_drv.reg_count), 1);
    if (err)
    {
        CWA_MSG(CWA_MSG_ERR, "Unable add a character device\n");
        goto ERROR_3;
    }

    // Create a instance in class
    dev = device_create(g_cwa_drv.class,
                        NULL,
                        MKDEV(g_cwa_drv.major, g_cwa_drv.minor_star + g_cwa_drv.reg_count),
                        dev_data,
                        MDRV_CWA_NAME);

    if (IS_ERR(dev))
    {
        CWA_MSG(CWA_MSG_ERR, "can't create device\n");
        err = -ENODEV;
        goto ERROR_4;
    }

    // Increase registered count
    g_cwa_drv.reg_count++;

    dev_set_drvdata(&pdev->dev, dev_data);

    return 0;

ERROR_4:
    cdev_del(&dev_data->cdev);

ERROR_3:
    free_irq(dev_data->irq, dev_data);

ERROR_2:
//    mdrv_cwa_clock_release(dev_data);

ERROR_1:
    devm_kfree(&dev_data->pdev->dev, dev_data);

    return err;
}
/*******************************************************************************************************************
 * mdrv_cwa_drv_remove
 *   Platform device remove handler
 *
 * Parameters:
 *   pdev: platfrom device
 *
 * Return:
 *   standard return value
 */
static s32 mdrv_cwa_drv_remove(struct platform_device *pdev)
{
   cwa_dev_data* dev_data;

    dev_data = (cwa_dev_data*)dev_get_drvdata(&pdev->dev);

    CWA_MSG(CWA_MSG_DBG, "dev_data: 0x%p\n", dev_data);

    //clock release
    //mdrv_cwa_clock_release(dev_data);

    //IRQ release
    free_irq(dev_data->irq, dev_data);

    //drv release
    cwa_drv_release(&dev_data->drv_handle);

    //instance release
    device_destroy(g_cwa_drv.class, dev_data->cdev.dev);

    //cdev delete (fops)
    cdev_del(&dev_data->cdev);

    //dev_data release
    devm_kfree(&dev_data->pdev->dev, dev_data);

    return 0;
}
/*******************************************************************************************************************
 * mdrv_cwa_drv_suspend
 *   Platform device suspend handler, but nothing to do here
 *
 * Parameters:
 *   pdev: platfrom device
 *
 * Return:
 *   standard return value
 */
static s32 mdrv_cwa_drv_suspend(struct platform_device *pdev, pm_message_t state)
{
    cwa_dev_data *dev_data = dev_get_drvdata(&pdev->dev);

    CWA_MSG(CWA_MSG_DBG, "dev_data: 0x%p\n", dev_data);

    return 0;
}
/*******************************************************************************************************************
 * mdrv_cwa_drv_resume
 *   Platform device resume handler, but nothing to do here
 *
 * Parameters:
 *   pdev: platfrom device
 *
 * Return:
 *   standard return value
 */
static s32 mdrv_cwa_drv_resume(struct platform_device *pdev)
{
    cwa_dev_data *dev_data = dev_get_drvdata(&pdev->dev);

    CWA_MSG(CWA_MSG_DBG, "dev_data: 0x%p\n", dev_data);

    return 0;
}
//-------------------------------------------------------------------------------------------------
// Data strucure for device driver
//-------------------------------------------------------------------------------------------------
static const struct of_device_id mdrv_cwag_match[] = {
    {
        .compatible = "mstar,infinity2-cwa",
        /*.data = NULL,*/
    },
    {},
};

static struct platform_driver mdrv_cwa_driver = {
    .probe          = mdrv_cwa_drv_probe,
    .remove     = mdrv_cwa_drv_remove,
    .suspend    = mdrv_cwa_drv_suspend,
    .resume     = mdrv_cwa_drv_resume,

    .driver = {
        .of_match_table = of_match_ptr(mdrv_cwag_match),
        .name   = "mstar_cwa",
        .owner  = THIS_MODULE,
    }
};
/*************************************************************************************************
 * mdrv_cwa_module_init
 *   module init function
 *
 * Parameters:
 *   N/A
 *
 * Return:
 *   standard return value
 */
s32 mdrv_cwa_module_init(void)
{
    s32 err;
    dev_t dev;

    CWA_MSG(CWA_MSG_DBG, "Moudle Init\n");

    // Allocate cdev id
    err = alloc_chrdev_region(&dev, MDRV_CWA_MINOR, MDRV_CWA_DEVICE_COUNT, MDRV_CWA_NAME);
    if (err)
    {
        CWA_MSG(CWA_MSG_ERR, "Unable allocate cdev id\n");
        return err;
    }

    g_cwa_drv.major = MAJOR(dev);
    g_cwa_drv.minor_star = MINOR(dev);
    g_cwa_drv.reg_count = 0;

    // Register device class
    g_cwa_drv.class = class_create(THIS_MODULE, MDRV_CWA_CLASS_NAME);

    if (IS_ERR(g_cwa_drv.class))
    {
        CWA_MSG(CWA_MSG_ERR, "Failed at class_create().Please exec [mknod] before operate the device\n");
        err = PTR_ERR(g_cwa_drv.class);
        goto ERR_RETURN_1;
    }

    // Register platform driver
    err = platform_driver_register(&mdrv_cwa_driver);
    if (err != 0)
    {
        CWA_MSG(CWA_MSG_ERR, "Fail at platform_driver_register().\n");
        goto ERR_RETURN_2;
    }

    CWA_MSG(CWA_MSG_DBG, "pass all mdrv_cwa_module_init function. \n");

    return 0;

ERR_RETURN_2:
    class_destroy(g_cwa_drv.class);

ERR_RETURN_1:
    unregister_chrdev_region(MKDEV(g_cwa_drv.major, g_cwa_drv.minor_star), MDRV_CWA_DEVICE_COUNT);

    return err;
}
/*******************************************************************************************************************
 * mdrv_cwa_module_exit
 *   module exit function
 *
 * Parameters:
 *   N/A
 *
 * Return:
 *   standard return value
 */
void mdrv_cwa_module_exit(void)
{
    /*de-initial the who GFLIPDriver */
    CWA_MSG(CWA_MSG_DBG, "Modules Exit\n");

    platform_driver_unregister(&mdrv_cwa_driver);
    class_destroy(g_cwa_drv.class);
    unregister_chrdev_region(MKDEV(g_cwa_drv.major, g_cwa_drv.minor_star), MDRV_CWA_DEVICE_COUNT);
}

module_init(mdrv_cwa_module_init);
module_exit(mdrv_cwa_module_exit);

MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("CWA ioctrl driver");
MODULE_LICENSE("GPL");
