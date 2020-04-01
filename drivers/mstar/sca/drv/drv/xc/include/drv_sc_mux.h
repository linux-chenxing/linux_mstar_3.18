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

#ifndef _DRV_MUX_H_
#define _DRV_MUX_H_


//------------------------------
// Mux related
//------------------------------
void MDrv_XC_mux_dispatch(E_MUX_INPUTPORT src , E_MUX_OUTPUTPORT dest);
void MDrv_XC_mux_set_sync_port_by_dataport(E_MUX_INPUTPORT src_port );

void MDrv_XC_mux_turnoff_cmux(void);
void MDrv_XC_mux_turnoff_ymux(void);
void MDrv_XC_mux_setAV_ymux(MS_U8 PortId);
void MDrv_BT656test_bus(void);

#endif

