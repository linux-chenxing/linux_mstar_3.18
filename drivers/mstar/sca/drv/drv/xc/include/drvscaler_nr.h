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

#ifndef DRV_SCALER_NR_H
#define DRV_SCALER_NR_H

/******************************************************************************/
/*                     Macro                                                  */
/* ****************************************************************************/


/******************************************************************************/
/*                     definition                                             */
/* ****************************************************************************/

#ifdef DRV_SCALER_NR_C
#define INTERFACE
#else
#define INTERFACE extern
#endif

/******************************************************************************/
/*                           Global Variables                                 */
/******************************************************************************/

//*************************************************************************
//          Enums
//*************************************************************************

//*************************************************************************
//          Structures
//*************************************************************************



/********************************************************************************/
/*                   Function Prototypes                                        */
/********************************************************************************/
INTERFACE void MDrv_Scaler_EnableNR(MS_BOOL bEn, SCALER_WIN eWindow);
INTERFACE void MDrv_Scaler_Enable3DNR(MS_BOOL bEnable, SCALER_WIN eWindow);
#undef INTERFACE

#endif /* DRV_SCALER_H */
