////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2007 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (¡§MStar Confidential Information¡¨) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    mdrv_gpio.c
/// @brief  GPIO Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
//#include "MsCommon.h"
//#include <linux/autoconf.h>
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
#include <linux/cdev.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <asm/io.h>

//#include "mst_devid.h"

#include "mdrv_gpio.h"
#include "mhal_gpio_reg.h"
#include "mhal_gpio.h"

//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------
// Forward declaration
//--------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
#if 0//reserved
static	struct semaphore	pm_gpio_mutex ;
#define	_MUTEX_INIT()   	init_MUTEX( &pm_gpio_mutex )
#define	_MUTEX_LOCK()   	down( &pm_gpio_mutex )
#define	_MUTEX_UNLOCK()		up( &pm_gpio_mutex )
#endif

//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
/// GPIO chiptop initialization
/// @return None
/// @note   Called only once at system initialization
//-------------------------------------------------------------------------------------------------
void MDrv_GPIO_Init(void)
{
	MHal_GPIO_Init();
}
EXPORT_SYMBOL(MDrv_GPIO_Init);

//-------------------------------------------------------------------------------------------------
/// select one pad to set
/// @param  u8IndexGPIO              \b IN:  pad index
/// @return None
/// @note
//-------------------------------------------------------------------------------------------------
void MDrv_GPIO_Pad_Set(U16 u16IndexGPIO)
{
    MHal_GPIO_Pad_Set(u16IndexGPIO);
}
EXPORT_SYMBOL(MDrv_GPIO_Pad_Set);

//-------------------------------------------------------------------------------------------------
/// enable output for selected one pad
/// @param  u8IndexGPIO              \b IN:  pad index
/// @return None
/// @note
//-------------------------------------------------------------------------------------------------
void MDrv_GPIO_Pad_Oen(U16 u16IndexGPIO)
{
    MHal_GPIO_Pad_Oen(u16IndexGPIO);
}
EXPORT_SYMBOL(MDrv_GPIO_Pad_Oen);

//-------------------------------------------------------------------------------------------------
/// enable input for selected one pad
/// @param  u8IndexGPIO              \b IN:  pad index
/// @return None
/// @note
//-------------------------------------------------------------------------------------------------
void MDrv_GPIO_Pad_Odn(U16 u16IndexGPIO)
{
    MHal_GPIO_Pad_Odn(u16IndexGPIO);
}
EXPORT_SYMBOL(MDrv_GPIO_Pad_Odn);

//-------------------------------------------------------------------------------------------------
/// read data from selected one pad
/// @param  u8IndexGPIO              \b IN:  pad index
/// @return None
/// @note
//-------------------------------------------------------------------------------------------------
U8 MDrv_GPIO_Pad_Read(U16 u16IndexGPIO)
{
    return MHal_GPIO_Pad_Level(u16IndexGPIO);
}
EXPORT_SYMBOL(MDrv_GPIO_Pad_Read);

//-------------------------------------------------------------------------------------------------
/// read pad direction for selected one pad
/// @param  u8IndexGPIO              \b IN:  pad index
/// @return None
/// @note
//-------------------------------------------------------------------------------------------------
U8 MDrv_GPIO_Pad_InOut(U16 u16IndexGPIO)
{
    return MHal_GPIO_Pad_InOut(u16IndexGPIO);
}
EXPORT_SYMBOL(MDrv_GPIO_Pad_InOut);

//-------------------------------------------------------------------------------------------------
/// output pull high for selected one pad
/// @param  u8IndexGPIO              \b IN:  pad index
/// @return None
/// @note
//-------------------------------------------------------------------------------------------------
void MDrv_GPIO_Pull_High(U16 u16IndexGPIO)
{
    MHal_GPIO_Pull_High(u16IndexGPIO);
}
EXPORT_SYMBOL(MDrv_GPIO_Pull_High);

//-------------------------------------------------------------------------------------------------
/// output pull low for selected one pad
/// @param  u8IndexGPIO              \b IN:  pad index
/// @return None
/// @note
//-------------------------------------------------------------------------------------------------
void MDrv_GPIO_Pull_Low(U16 u16IndexGPIO)
{
    MHal_GPIO_Pull_Low(u16IndexGPIO);
}
EXPORT_SYMBOL(MDrv_GPIO_Pull_Low);

//-------------------------------------------------------------------------------------------------
/// output set high for selected one pad
/// @param  u8IndexGPIO              \b IN:  pad index
/// @return None
/// @note
//-------------------------------------------------------------------------------------------------
void MDrv_GPIO_Set_High(U16 u16IndexGPIO)
{
    MHal_GPIO_Set_High(u16IndexGPIO);
}
EXPORT_SYMBOL(MDrv_GPIO_Set_High);

//-------------------------------------------------------------------------------------------------
/// output set low for selected one pad
/// @param  u8IndexGPIO              \b IN:  pad index
/// @return None
/// @note
//-------------------------------------------------------------------------------------------------
void MDrv_GPIO_Set_Low(U16 u16IndexGPIO)
{
    MHal_GPIO_Set_Low(u16IndexGPIO);
}
EXPORT_SYMBOL(MDrv_GPIO_Set_Low);

//-------------------------------------------------------------------------------------------------
/// enable GPIO int for selected one pad
/// @param  u8IndexGPIO              \b IN:  pad index
/// @return None
/// @note
//-------------------------------------------------------------------------------------------------
void MDrv_Enable_GPIO_INT(U16 u16IndexGPIO)
{
    MHal_Enable_GPIO_INT(u16IndexGPIO);
}
EXPORT_SYMBOL(MDrv_Enable_GPIO_INT);

int MDrv_GPIO_To_Irq(U16 u16IndexGPIO)
{
    return MHal_GPIO_To_Irq(u16IndexGPIO);
}
EXPORT_SYMBOL(MDrv_GPIO_To_Irq);



//-------------------------------------------------------------------------------------------------
/// set GPIO int polarity for selected one pad
/// @param  u8IndexGPIO              \b IN:  pad index
/// @return None
/// @note
//-------------------------------------------------------------------------------------------------
void MDrv_GPIO_Set_POLARITY(U16 u16IndexGPIO, U16 reverse)
{
    MHal_GPIO_Set_POLARITY(u16IndexGPIO,reverse);
}
EXPORT_SYMBOL(MDrv_GPIO_Set_POLARITY);


