///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2006 - 2014 MStar Semiconductor, Inc.
// This program is free software.
// You can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation;
// either version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with this program;
// if not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//  Include files
//-------------------------------------------------------------------------------------------------

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/platform_device.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>

#ifndef __MDRV_DVFS_H__
#include "mdrv_dvfs.h"
#endif

DEFINE_MUTEX(MDrvDvfsMutex);

static struct proc_dir_entry *proc_mstar_dvfs_dir;


//=================================================================================================
U32 MDrvDvfsProc(U32 dwInputCpuClock, U8 dwCpu)
{
    U32 dwOutputCpuClock = 0;

    mutex_lock(&MDrvDvfsMutex);
    dwOutputCpuClock = MHalDvfsProc(dwInputCpuClock / 1000, dwCpu);
    mutex_unlock(&MDrvDvfsMutex);

    return (dwOutputCpuClock * 1000);
}

//=================================================================================================
void MDrvDvfsInit(void)
{
    MDrvHalDvfsInit();
}

//=================================================================================================
void MDrvDvfsCpuDisplay(void)
{
    MHalDvfsCpuDisplay();
}

//=================================================================================================
void MDrvDvfsCpuPowerAdjustment(U32 dwCpuPowerVoltage)
{
    MHalDvfsCpuPowerAdjustment(dwCpuPowerVoltage);
}

//=================================================================================================
void MDrvDvfsCorePowerAdjustment(U32 dwCorePowerVoltage)
{
    MHalDvfsCorePowerAdjustment(dwCorePowerVoltage);
}

//=================================================================================================
U32 MDrvDvfsQueryCpuClock(U32 dwCpuClockType)
{
    return (MHalDvfsQueryCpuClock(dwCpuClockType) * 1000);
}

U32 MDrvDvfsQueryCpuPower(U32 dwCpuPowerType)
{
    return MHalDvfsQueryCpuPower(dwCpuPowerType);
}

//=================================================================================================
U32 MDrvDvfsQueryCpuClockByTemperature(U8 dwCpu)
{
    U32 dwOutputCpuClock = 0;

    //mutex_lock(&MDrvDvfsMutex);
    dwOutputCpuClock = MHalDvfsQueryCpuClockByTemperature(dwCpu);
    //mutex_unlock(&MDrvDvfsMutex);

    return (dwOutputCpuClock * 1000);
}
//=================================================================================================
U32 MDrvDvfsQueryTemperature(void)
{
   U32 dwTemperature = 0;
   dwTemperature = MHalDvfsQueryTemperature();

   return dwTemperature;
}

#ifdef CONFIG_MP_DVFS_CPUHOTPLUG
//=================================================================================================
U32 MDrvDvfsGetOverTemperatureFlag(void)
{
   return MHalDvfsGetOverTemperatureFlag();
}
#endif

//=================================================================================================
U32 MDrvDvfsGetCpuTemperature(void)
{
    return MHalDvfsGetCpuTemperature();
}

//=================================================================================================
#ifdef CONFIG_MP_DVFS_CPUHOTPLUG
U32  MdrvDvfsGetAntutuCpuNR()
{
      return MHalDvfsGetAntutuCpuNR();
}
#endif
//=================================================================================================
static int MDrvDvfsProbe(struct platform_device *pdev)
{
    int wReturnValue = 0;

    if(!(pdev->name) || \
       strcmp(pdev->name, "Mstar_DVFS") || \
       pdev->id != 0)
    {
        wReturnValue = -ENXIO;
    }

    MDrvDvfsInit();

    return wReturnValue;
}

//=================================================================================================
static int MDrvDvfsRemove(struct platform_device *pdev)
{
    return 0;
}

//=================================================================================================
static int MDrvDvfsSuspend(struct platform_device *dev, pm_message_t state)
{
    return 0;
}

//=================================================================================================
static int MDrvDvfsResume(struct platform_device *dev)
{
    return 0;
}

//=================================================================================================
static struct platform_driver MstarDvfsDriver =
{
    .probe      = MDrvDvfsProbe,
    .remove     = MDrvDvfsRemove,
    .suspend    = MDrvDvfsSuspend,
    .resume     = MDrvDvfsResume,

    .driver =
    {
        .name   = "Mstar_DVFS",
        .owner  = THIS_MODULE,
    }
};


//=================================================================================================
static int dvfs_temp_seq_show(struct seq_file *s, void *v)
{
	unsigned int temperature = 0;


	temperature = MDrvDvfsGetCpuTemperature();
	//printk("temperature:%d \n", i, temperature);
	seq_printf(s, "%d\n", temperature);
	return 0;
}

static int dvfs_temp_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, &dvfs_temp_seq_show, NULL);
}

static const struct file_operations proc_dvfs_temp_fileops = {
	.owner		= THIS_MODULE,
	.open       = dvfs_temp_proc_open,
	.read		= seq_read,
	.llseek     = seq_lseek,
	.release    = seq_release,
};
//=================================================================================================

static int __init MDrvDvfsModuleInit(void)
{
	struct proc_dir_entry *entry;

    proc_mstar_dvfs_dir = proc_mkdir("mstar_dvfs", NULL);

	if (!proc_mstar_dvfs_dir)
		return -ENOMEM;

	entry = proc_create("temperature", S_IRUSR | S_IWUSR, proc_mstar_dvfs_dir, &proc_dvfs_temp_fileops);
	if (!entry)
		goto fail;

	return (int) platform_driver_register(&MstarDvfsDriver);

fail:
	return -ENOMEM;
}

//=================================================================================================
static void __exit MDrvDvfsModuleExit(void)
{
	remove_proc_entry("temperature", proc_mstar_dvfs_dir);
	remove_proc_entry("mstar_dvfs", NULL);

    platform_driver_unregister(&MstarDvfsDriver);
}

//=================================================================================================
module_init(MDrvDvfsModuleInit);
module_exit(MDrvDvfsModuleExit);

MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("Mstar DVFS Driver");
MODULE_LICENSE("GPL");

//=================================================================================================
