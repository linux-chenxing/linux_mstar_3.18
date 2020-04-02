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

#define DRV_SCALER_NR_C

/******************************************************************************/

/*                           Header Files                                     */

/******************************************************************************/

// Common Definition
#if defined(MSOS_TYPE_LINUX_KERNEL)
#include <linux/wait.h>
#endif

#include "MsCommon.h"

#include "MsOS.h"

//#include "Debug.h"

#include "drvXC_IOPort.h"

#include "apiXC.h"

#include "mvideo_context.h"



#include "drv_sc_ip.h"



#include "mhal_sc.h"


#if TEST_NEW_DYNAMIC_NR



/******************************************************************************/

/*                           Constants                                        */

/******************************************************************************/



/******************************************************************************/

/*                           Local Prototypes                                 */

/******************************************************************************/

#define DynamicNR_DBG(x) //x



/******************************************************************************/

/*                           Variables                                        */

/******************************************************************************/





/******************************************************************************/

///Enable/Disable 3DNR

///@param bEnable \b IN: enable/disable

/******************************************************************************/

void MDrv_Scaler_Enable3DNR(MS_BOOL bEnable, SCALER_WIN eWindow)

{

    if (gSrcInfo[eWindow].bForceNRoff) // force NR to be disabled when GWIN is consuming bandwidth in HD modes

    {

        bEnable = DISABLE;

    }



    HAL_SC_enable_dnr(0x01, eWindow);

    HAL_SC_enable_field_avg_y(bEnable, eWindow); //Main Window Y average mode when dotline cycle

    HAL_SC_enable_field_avg_c(DISABLE, eWindow); //Main Window C average mode when dotline cycle

                                                 //always DISABLE

}





void MDrv_Scaler_EnableNR(MS_BOOL bEn, SCALER_WIN eWindow)

{

    if (bEn == 0)

    {

        mvideo_sc_set_3dnr(DISABLE, eWindow);

        MDrv_Scaler_Enable3DNR(DISABLE, eWindow);

    }

    else

    {

        mvideo_sc_set_3dnr(ENABLE, eWindow);

        // NR start to work immediately, but it does not fetch memory until next frame

        // enable NR in vblank to avoid wrong result

        MDrv_XC_wait_input_vsync(1, 50, eWindow);

        MDrv_Scaler_Enable3DNR(ENABLE, eWindow);

    }



//T2 doesn't have Pre CCS

//    if( MApi_XC_IsYUVSpace() )

//        MDrv_Scaler_SetCNR(MS_CNR_ON);

//    else

//        MDrv_Scaler_SetCNR(MS_CNR_OFF);



//    MDrv_Scaler_SetDNR(MS_DNR_OFF);

}



#endif  //TEST_NEW_DYNAMIC_NR



