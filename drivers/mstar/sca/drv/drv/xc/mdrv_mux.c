////////////////////////////////////////////////////////////////////////////////

//

// Copyright (c) 2008-2009 MStar Semiconductor, Inc.

// All rights reserved.

//

// Unless otherwise stipulated in writing, any and all information contained

// herein regardless in any format shall remain the sole proprietary of

// MStar Semiconductor Inc. and be kept in strict confidence

// ("MStar Confidential Information") by the recipient.

// Any unauthorized act including without limitation unauthorized disclosure,

// copying, use, reproduction, sale, distribution, modification, disassembling,

// reverse engineering and compiling of the contents of MStar Confidential

// Information is unlawful and strictly prohibited. MStar hereby reserves the

// rights to any and all damages, losses, costs and expenses resulting therefrom.

//

////////////////////////////////////////////////////////////////////////////////

//==============================================================================

// [Mdrv_mux.c]

// Date: 20090225

// Descriptions: Add a new mux layer for HW setting

//==============================================================================



#define  MDRV_MUX_C



#if defined(MSOS_TYPE_LINUX_KERNEL)
#include <linux/wait.h>
#endif


#include "MsCommon.h"

#include "drvXC_IOPort.h"

#include "apiXC.h"



#include "mhal_mux.h"



void MDrv_XC_mux_set_sync_port_by_dataport(E_MUX_INPUTPORT src_port )

{

    Hal_SC_set_sync_port_by_dataport(src_port);

}



void MDrv_XC_mux_dispatch(E_MUX_INPUTPORT src , E_MUX_OUTPUTPORT dest)

{

    Hal_SC_mux_dispatch(src, dest);

}



void MDrv_XC_mux_turnoff_ymux(void)

{

    Hal_SC_mux_set_adc_y_mux(0x0F);

}



void MDrv_XC_mux_turnoff_cmux(void)

{

    Hal_SC_mux_set_adc_c_mux(0x0F);

}

void MDrv_XC_mux_setAV_ymux(MS_U8 PortId)

{

    Hal_SC_mux_set_adc_AV_ymux(PortId);

}

void MDrv_BT656test_bus(void )
{
	Hal_BT656test_bus();
}


#undef MDRV_MUX_C

