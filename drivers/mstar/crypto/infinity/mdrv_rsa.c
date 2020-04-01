/*
 *#############################################################################
 *
 * Copyright (c) 2006-2011 MStar Semiconductor, Inc.
 * All rights reserved.
 *
 * Unless otherwise stipulated in writing, any and all information contained
 * herein regardless in any format shall remain the sole proprietary of
 * MStar Semiconductor Inc. and be kept in strict confidence
 * (!¡±MStar Confidential Information!¡L) by the recipient.
 * Any unauthorized act including without limitation unauthorized disclosure,
 * copying, use, reproduction, sale, distribution, modification, disassembling,
 * reverse engineering and compiling of the contents of MStar Confidential
 * Information is unlawful and strictly prohibited. MStar hereby reserves the
 * rights to any and all damages, losses, costs and expenses resulting therefrom.
 *
 *#############################################################################
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <../drivers/mstar/include/ms_msys.h>
#include <../drivers/mstar/include/ms_platform.h>
#include <../drivers/mstar/include/mdrv_hvsp_io_st.h>
#include <../drivers/mstar/include/mdrv_hvsp_io.h>
#include <asm/uaccess.h>
#include "mdrv_rsa.h"
#include "halAESDMA.h"

#define DRIVER_NAME "rsa"
static unsigned int test_ioctl_major = 0;
static unsigned int num_of_dev = 1;
static struct cdev test_ioctl_cdev;
static int ioctl_num = 0;


static const char* DMEM_RSA_ENG_INPUT="RSA_ENG_IN";

struct rsaConfig{
    U16 val;
	rwlock_t lock;
	U32 *pu32RSA_Sig;
	U32 *pu32RSA_KeyN;
	U32 *pu32RSA_KeyE;
	U32 *pu32RSA_Output;
	U32 u32RSA_KeyNLen;
	U32 u32RSA_SigLen;
	U32 u32RSA_KeyELen;
	bool bRSAHwKey;
	bool bRSAPublicKey;
	U32 *pu32aesKey;
};

static long test_ioctl_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    struct rsaConfig *ioctl_data = filp->private_data;
    int retval;
    struct ioctl_arg data;
    int nOutSize;
    memset(&data, 0, sizeof(data));

    switch (cmd) {

         case MDrv_RSA_Reset:
            HAL_RSA_Reset();
         break;

         case MDrv_RSA_Setkeylen:
            if (copy_from_user(&data, (int __user *)arg, sizeof(data))) {
                retval = -EFAULT;
                goto done;
            }
            write_lock(&ioctl_data->lock);
            ioctl_data->u32RSA_KeyNLen = data.u32RSA_KeyNLen;
            ioctl_data->bRSAHwKey =data.bRSAHwKey;
            ioctl_data->bRSAPublicKey =data.bRSAPublicKey;
            write_unlock(&ioctl_data->lock);
            HAL_RSA_SetKeyLength(((ioctl_data->u32RSA_KeyNLen)-1) & 0x3F);
            HAL_RSA_SetKeyType(ioctl_data->bRSAHwKey, ioctl_data->bRSAPublicKey);
         break;

         case MDrv_RSA_Calculate:

             if (copy_from_user(&data, (int __user *)arg, sizeof(data))) {
                retval = -EFAULT;
                goto done;
             }
             write_lock(&ioctl_data->lock);
             ioctl_data->pu32RSA_Sig = data.pu32RSA_Sig;
             ioctl_data->pu32RSA_Output = data.pu32RSA_Output;
             ioctl_data->pu32RSA_KeyN = data.pu32RSA_KeyN;
             ioctl_data->pu32RSA_KeyE = data.pu32RSA_KeyE;
             ioctl_data->u32RSA_KeyELen = data.u32RSA_KeyELen;
             ioctl_data->u32RSA_KeyNLen = data.u32RSA_KeyNLen;
             ioctl_data->u32RSA_SigLen = data.u32RSA_SigLen;
             write_unlock(&ioctl_data->lock);
             HAL_RSA_Ind32Ctrl(1);//1:write
             HAL_RSA_LoadSignInverse(ioctl_data->pu32RSA_Sig,  ioctl_data->u32RSA_SigLen);

             if((!ioctl_data->bRSAHwKey) && (ioctl_data->pu32RSA_KeyN))
             {
                  HAL_RSA_LoadKeyNInverse(ioctl_data->pu32RSA_KeyN,  ioctl_data->u32RSA_KeyNLen);
             }

             if((!ioctl_data->bRSAHwKey) && (ioctl_data->pu32RSA_KeyE))
             {
                HAL_RSA_LoadKeyE(ioctl_data->pu32RSA_KeyE, ioctl_data->u32RSA_KeyELen);
             }

             HAL_RSA_ExponetialStart();
             while((HAL_RSA_GetStatus() & RSA_STATUS_RSA_DONE) != RSA_STATUS_RSA_DONE);

             if((ioctl_data->bRSAHwKey) || (ioctl_data->u32RSA_KeyNLen == 2048))
             {
                nOutSize = (2048/8)/4;
             }
             else
             {
                nOutSize = (1024/8)/4;
             }

             HAL_RSA_Ind32Ctrl(0);
             int i = 0;
             for( i = 0; i<nOutSize; i++)
             {

                HAL_RSA_SetFileOutAddr(i);
                HAL_RSA_FileOutStart();
               *(ioctl_data->pu32RSA_Output + (nOutSize-1) - i) = HAL_RSA_FileOut();
             }

            HAL_RSA_FileOutEnd();
            HAL_RSA_Reset();
            unsigned char* val;
            read_lock(&ioctl_data->lock);
            val = (unsigned char*)ioctl_data->pu32RSA_Output;
            read_unlock(&ioctl_data->lock);
            data.pu32RSA_Output= val;


            if (copy_to_user((int __user *)arg, &data, sizeof(data)) ) {
                                retval = -EFAULT;
            goto done;}
            break;

            case MDrv_AESDMA_SetKey:
                if (copy_from_user(&data, (int __user *)arg, sizeof(data))) {
                retval = -EFAULT;
                goto done;
                 }
                printk(KERN_ALERT "IOCTL set pu32aesKey:%x\n", data.pu32aesKey);
                write_lock(&ioctl_data->lock);
                ioctl_data->pu32aesKey= data.pu32aesKey;
                write_unlock(&ioctl_data->lock);
                HAL_AESDMA_UseCipherKey();
                HAL_AESDMA_SetCipherKey((U16*)ioctl_data->pu32aesKey);
            break;

            case MDrv_AESDMA_SelEng:

            break;



            case MDrv_AESDMA_Init:
                HAL_AESDMA_Reset();
            break;



        default:

         retval = -ENOTTY;
 }

    done:

    return retval;
}

ssize_t test_ioctl_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    struct rsaConfig *ioctl_data = filp->private_data;
    unsigned char val;
    int retval;
    int i = 0;

    read_lock(&ioctl_data->lock);
    val = ioctl_data->val;
    read_unlock(&ioctl_data->lock);

    for (;i < count ;i++) {
        if (copy_to_user(&buf[i], &val, 1)) {
        retval = -EFAULT;
    goto out;
     }
 }

 retval = count;

out:
 return retval;
}

static int test_ioctl_close(struct inode *inode, struct file *filp)
{
    printk(KERN_ALERT "%s call.\n", __func__);
        if (filp->private_data) {
            kfree(filp->private_data);
            filp->private_data = NULL;
        }

 return 0;
}

static int test_ioctl_open(struct inode *inode, struct file *filp)
{

    struct rsaConfig *ioctl_data;
    printk(KERN_ALERT "%s call.\n", __func__);

    ioctl_data = kmalloc(sizeof(struct rsaConfig), GFP_KERNEL);
    if (ioctl_data == NULL)
    return -ENOMEM;

    rwlock_init(&ioctl_data->lock);


    filp->private_data = ioctl_data;

 return 0;
}

struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = test_ioctl_open,
    .release = test_ioctl_close,
    .read = test_ioctl_read,
    .unlocked_ioctl = test_ioctl_ioctl,
};

static int test_ioctl_init(void)
{
    dev_t dev = MKDEV(test_ioctl_major, 0);
    int alloc_ret = 0;
    int cdev_ret = 0;

    alloc_ret = alloc_chrdev_region(&dev, 0, num_of_dev, DRIVER_NAME);
    if (alloc_ret)
    goto error;

    test_ioctl_major = MAJOR(dev);

    cdev_init(&test_ioctl_cdev, &fops);
    cdev_ret = cdev_add(&test_ioctl_cdev, dev, num_of_dev);
    if (cdev_ret)
        goto error;

    printk(KERN_ALERT "%s driver(major: %d) installed.\n", DRIVER_NAME, test_ioctl_major);
    return 0;
    error:
    if (cdev_ret == 0)
        cdev_del(&test_ioctl_cdev);
    if (alloc_ret == 0)
        unregister_chrdev_region(dev, num_of_dev);

 return -1;
}

static void test_ioctl_exit(void)
{
    dev_t dev = MKDEV(test_ioctl_major, 0);
    cdev_del(&test_ioctl_cdev);
    unregister_chrdev_region(dev, num_of_dev);
    printk(KERN_ALERT "%s driver removed.\n", DRIVER_NAME);
}

module_init(test_ioctl_init);
module_exit(test_ioctl_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("mstar");
MODULE_DESCRIPTION("This is test_ioctl module.");
