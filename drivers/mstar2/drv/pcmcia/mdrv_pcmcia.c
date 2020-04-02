/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include "dvb_ca_en50221.h"
#include "mdrv_pcmcia.h"
#include "mdrv_types.h"
#include "drvPCMCIA.h"
#include "dvbdev.h"
// MACRO for debug message
#define CI_DBG_ENABLE             0

#if CI_DBG_ENABLE
#define DBG_CI(_f)               (_f)
#define DBG_CI_ERR(_f)           (_f)
#define DBG_CI_ENTRY(_f)         (_f)
#define DBG_CI_INFO(_f)          (_f)
#define DBG_CI_EXIT(_f)          (_f)
#else
#define DBG_CI(_f)
#define DBG_CI_ERR(_f)           (_f)
#define DBG_CI_ENTRY(_f)
#define DBG_CI_INFO(_f)
#define DBG_CI_EXIT(_f)
#endif

#define OK  0
#define NOT_OK 1

DVB_DEFINE_MOD_OPT_ADAPTER_NR(adapter_nr);

static BOOL bIOMode = FALSE;
static mstar_ci *gCI = NULL;

void func_isr_callback(void* pstISR,  void* pModule)
{
    dvb_ca_en50221_frda_irq(&gCI->ca, (int)(*(PCMCIA_MODULE*)pModule));
}

int ci_read_attribute_mem(struct dvb_ca_en50221 *ca, int slot, int address)
{
    U8 u8Data = 0;
    int attrAdress = address/2;
    mdelay(10);

    MDrv_PCMCIA_ReadAttribMem((U16)attrAdress, &u8Data);
    DBG_CI_ENTRY(printk("%s, addr %d, 0x%x\n", __FUNCTION__, address, u8Data));
    return u8Data;
}

int ci_write_attribute_mem(struct dvb_ca_en50221 *ca, int slot, int address, u8 value)
{    
    mdelay(10);
    DBG_CI_ENTRY(printk("%s, addr %d, 0x%x\n", __FUNCTION__, address, value));
    MDrv_PCMCIA_WriteAttribMem((U16)address,value);
    return 0;
}

int ci_read_cam_control(struct dvb_ca_en50221 *ca, int slot, u8 address)
{
    U8 data = 0;
    bIOMode = TRUE;

    if(address != 0)
        mdelay(10);

    data = MDrv_PCMCIA_ReadIOMem((U16)address);
    DBG_CI_ENTRY(printk("%s, addr %d, 0x%x\n", __FUNCTION__, address, data));


    //printk("read 0x%x\n", data);        
    return data;
}

int ci_write_cam_control(struct dvb_ca_en50221 *ca, int slot, u8 address,u8 value)
{    
    bIOMode = TRUE;
    //printk("write 0x%x\n", value);
    DBG_CI_ENTRY(printk("%s, addr %d, 0x%x\n", __FUNCTION__, address, value));

    MDrv_PCMCIA_WriteIOMem(address,value);
    return 0;
}

int ci_slot_reset(struct dvb_ca_en50221 *ca, int slot)
{
    bIOMode = FALSE;
    
    MDrv_PCMCIA_ResetHW();
    return OK;
}

int ci_slot_shutdown(struct dvb_ca_en50221 *ca, int slot)
{
    bIOMode = FALSE;
   MDrv_PCMCIA_ResetHW();
   return OK;
}

int ci_slot_ts_enable(struct dvb_ca_en50221 *ca, int slot)
{
    return OK;
}

int ci_poll_slot_status(struct dvb_ca_en50221 *ca, int slot, int open)
{
    BOOL result = MDrv_PCMCIA_Detect();
    U8 tmp = 0;
    if(result) 
    { 
        if(bIOMode)
            tmp = 0x1D;
        else
            MDrv_PCMCIA_ReadAttribMem(0, &tmp);
            
        if(tmp == 0x1D)
            return DVB_CA_EN50221_POLL_CAM_PRESENT | DVB_CA_EN50221_POLL_CAM_READY; 
        else
            return DVB_CA_EN50221_POLL_CAM_PRESENT;
    }
    else
    {
        return 0;
    }
}

#if 0

void ci_slot_set_frontend_source(struct dvb_ca_en50221 *ca, int slot,ca_set_frontend_t source)
{	 
    if (source == CA_SOURCE_FRONTEND0)
        return ;
    else 
        return ;
    
}

void ci_slot_set_ts_route_mode(struct dvb_ca_en50221 *ca, int slot,ca_set_route_mode_t mode)
{	 
    switch(mode)
    {
        case CA_TS_ROUTE_MODE_DISCONNECT:
            printk("  CA_TS_ROUTE_MODE_DISCONNECT  %d\n", mode);
            break;
        case CA_TS_ROUTE_MODE_BYPASS:
	     printk("  CA_TS_ROUTE_MODE_BYPASS  %d\n", mode);
            _CI_SetCardByPass();
            break;
        case CA_TS_ROUTE_MODE_THROUGH:
	     printk("  CA_TS_ROUTE_MODE_THROUGH  %d\n", mode);
            _CI_SetCardThrough();
            break;
        default:
            DBG_CI_ERR(printk("Invalid route mode %d\n", mode));
            return ;	
    }	
    return ;
}

int ci_slot_get_ts_route_mode(struct dvb_ca_en50221 *ca, int slot,ca_set_route_mode_t *mode)
{
    return OK;
}

#endif

int ci_register(mstar_ci *ci, struct dvb_adapter *adapter)
{
    struct dvb_ca_en50221 *ca =&ci->ca;
    int result = OK;
    ca->data = ci;
    ca->owner = THIS_MODULE;
    ca->slot_reset = ci_slot_reset;
    ca->poll_slot_status = ci_poll_slot_status;
    ca->read_attribute_mem = ci_read_attribute_mem;
    ca->write_attribute_mem = ci_write_attribute_mem;
    ca->read_cam_control = ci_read_cam_control;
    ca->write_cam_control = ci_write_cam_control;
//    ca->slot_set_frontend_source = ci_slot_set_frontend_source;
//    ca->slot_set_ts_route_mode = ci_slot_set_ts_route_mode;
    ca->slot_ts_enable = ci_slot_ts_enable;
    ca->slot_shutdown = ci_slot_shutdown;

    if ((result = dvb_ca_en50221_init(adapter, ca, 0, 1)) != 0) 
    {  
        ci_unregister(ci);
        DBG_CI_ERR(printk("ci initialisation failed.\n"));
    }
    
    MDrv_PCMCIA_InstarllIsrCallback(func_isr_callback);
    DBG_CI_INFO(printk("ci interface initialised.\n"));
    return result;
}

int ci_unregister(mstar_ci * ci)
{
    dvb_ca_en50221_release(&ci->ca);
    return OK;
}
 
int ci_probe(struct platform_device *pdev, mstar_ci *ci)
{
    struct dvb_adapter *dvbadapter = &ci->adapter;
    int r = -ENOENT;
        
    MDrv_PCMCIA_Init(FALSE);
    
    dvb_register_adapter(dvbadapter, DRIVER_NAME, THIS_MODULE, &pdev->dev, adapter_nr);
    
    r = ci_register(ci,dvbadapter);
    if (r < 0) 
    {
        DBG_CI_ERR(printk("ci_probe failed!.\n"));
        return r;
    }
    return OK;
}

int ci_remove(mstar_ci *ci)
{
    int bRet = OK;

    MDrv_PCMCIA_Enable_Interrupt(FALSE);
    MDrv_PCMCIA_Set_InterruptStatus(FALSE);

    MDrv_PCMCIA_Exit();
	
    ci_unregister(ci);
    return bRet;
}

#ifdef CONFIG_PM
int __ref ci_suspend()
{
    bIOMode = FALSE;

    if( FALSE == MDrv_PCMCIA_SetPowerState(E_POWER_SUSPEND))
        return NOT_OK;
	
    return OK;
}

int __ref ci_resume()
{
    if( FALSE == MDrv_PCMCIA_SetPowerState(E_POWER_RESUME))
        return NOT_OK;
	
    return OK;
}
#endif

static int mstar_mdrv_pcmcia_remove(struct platform_device *pdev)
{
    mstar_ci *ci = platform_get_drvdata(pdev);

    DBG_CI_ENTRY(printk("[K][%s][%d].\n", __FUNCTION__, __LINE__));

    if( ci_remove(ci)!= OK )
    {
        DBG_CI_ERR(printk("[K][%s][%d] mstar_mdrv_pcmcia_remove() != OK .\n", __FUNCTION__, __LINE__));
    }

    DBG_CI_EXIT(printk("[K][%s][%d].\n", __FUNCTION__, __LINE__));

    return 0;
}

static int mstar_mdrv_pcmcia_probe(struct platform_device *pdev)
{

    int r = 0;

    DBG_CI_ENTRY(printk("[TSD Entry][%s][%d].\n", __FUNCTION__, __LINE__));

    //ci = devm_kzalloc(&pdev->dev, sizeof(struct mstar_ci), GFP_KERNEL);
    gCI = kzalloc(sizeof(mstar_ci), GFP_KERNEL);
    if( !gCI )
    {
        DBG_CI_ERR(printk("[K][CI Error][%s][%d] kzalloc error.\n", __FUNCTION__, __LINE__));
        return -ENOMEM;
    }

    gCI->pdev = pdev;

    r = ci_probe(pdev,gCI);

    if( r < 0 )
    {
        DBG_CI_ERR(printk("[K][CI Error][%s][%d] ci_probe() error.\n", __FUNCTION__, __LINE__));
        ci_remove(gCI);
        return r;
    }

    platform_set_drvdata(pdev, gCI);

    DBG_CI_EXIT(printk("[K][CI Exit][%s][%d].\n", __FUNCTION__, __LINE__));
    return 0;
}

#ifdef CONFIG_PM
static int __ref mstar_mdrv_pcmcia_suspend(struct platform_device *pdev, pm_message_t msg)
{
    //struct mstar_ci *ci = platform_get_drvdata(pdev);

    DBG_CI_ENTRY(printk("[K][%s][%d].\n", __FUNCTION__, __LINE__));

    if( ci_suspend() != OK )
    {
        DBG_CI_ERR(printk("[K][%s][%d] ci_suspend() != OK .\n", __FUNCTION__, __LINE__));
    }

    DBG_CI_EXIT(printk("[K][%s][%d].\n", __FUNCTION__, __LINE__));

    return 0;
}

static int __ref mstar_mdrv_pcmcia_resume(struct platform_device *pdev)
{
    //struct mstar_ci *ci = platform_get_drvdata(pdev);

    DBG_CI_ENTRY(printk("[K][%s][%d].\n", __FUNCTION__, __LINE__));

    if( ci_resume() != OK )
    {
        DBG_CI_ERR(printk("[K][%s][%d] ci_resume() != OK .\n", __FUNCTION__, __LINE__));
    }

    DBG_CI_EXIT(printk("[K][%s][%d].\n", __FUNCTION__, __LINE__));

    return 0;
}
#endif

static struct platform_driver mstar_pcmcia_driver = {
    .probe      = mstar_mdrv_pcmcia_probe,
    .remove     = mstar_mdrv_pcmcia_remove,
    .driver     = {
        .name   = DRIVER_NAME,
        .owner  = THIS_MODULE,
    },
#ifdef CONFIG_PM
    .suspend = mstar_mdrv_pcmcia_suspend,
    .resume = mstar_mdrv_pcmcia_resume,
#endif    
};


static struct platform_device mstar_pcmcia_device =
{
    .name       = DRIVER_NAME,
    .id         = -1,
};

static struct platform_device *pcmcia_devices[] __initdata =
{
    &mstar_pcmcia_device,
};

static int __init mstar_pcmcia_init(void)
{
    int retval = 0;

    DBG_CI_ENTRY(printk("[K][%s][%d].\n", __FUNCTION__, __LINE__));
    DBG_CI(printk("[K]module compile date is [%s] module compile time is [%s].\n", __DATE__, __TIME__));

    //register device
    platform_add_devices(pcmcia_devices, ARRAY_SIZE(pcmcia_devices));

    // register driver
    retval = platform_driver_register(&mstar_pcmcia_driver);

    DBG_CI_EXIT(printk("[K][%s][%d].\n", __FUNCTION__, __LINE__));

    return retval;

}

static void __exit mstar_pcmcia_exit(void)
{
    DBG_CI_ENTRY(printk("[K][%s][%d].\n", __FUNCTION__, __LINE__));

    platform_driver_unregister(&mstar_pcmcia_driver);

    DBG_CI_EXIT(printk("[K][%s][%d].\n", __FUNCTION__, __LINE__));
}

module_init(mstar_pcmcia_init);
module_exit(mstar_pcmcia_exit);

MODULE_DESCRIPTION("Mstar PCMCIA driver");
MODULE_AUTHOR("MSTAR Author");
MODULE_LICENSE("GPL");

