////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2011 MStar Semiconductor, Inc.
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

/**
 *  @file drv_scl_mgwin_ioctl.h
 *  @brief SCLMGWIN Driver IOCTL interface
 */

/**
 * \defgroup sclhvsp_group  SCLMGWIN driver
 * \note
 *
 *
 * @{
 */

#ifndef __DRV_SCL_MGWIN_IOCTL_H__
#define __DRV_SCL_MGWIN_IOCTL_H__

//=============================================================================
// Includs
//=============================================================================


//=============================================================================
// IOCTRL defines
//=============================================================================

#define IOCTL_SCLMGWIN_SET_FRAME_CONFIG_NR          (0)     ///< The IOCTL NR definition for IOCTL_SCLMGWIN_SET_FRAME_CONFIG_NR
#define IOCTL_SCLMGWIN_SET_FRAME_ONOFF_CONFIG_NR    (1)     ///< The IOCTL NR definition for IOCTL_SCLMGWIN_SET_FRAME_ONONFF_CONFIG_NR
#define IOCTL_SCLMGWIN_SET_SUBWIN_CONFIG_NR         (2)     ///< The IOCTL NR definition for IOCTL_SCLMGWIN_SET_SUBWIN_CONFIG_NR
#define IOCTL_SCLMGWIN_SET_SUBWIN_FLIP_CONFIG_NR    (3)     ///< The IOCTL NR definition for IOCTL_SCLMGWIN_SET_SUBWIN_FLIP_CONFIG_NR
#define IOCTL_SCLMGWIN_SET_SUBWIN_ONOFF_CONFIG_NR   (4)     ///< The IOCTL NR definition for IOCTL_SCLMGWIN_SET_SUBWIN_ONOFF_CONFIG_NR
#define IOCTL_SCLMGWIN_SET_DBF_CONFIG_NR            (5)     ///< The IOCTL NR definition for IOCTL_SCLMGWIN_SET_DBF_CONFIG_NR
#define IOCTL_SCLMGWIN_SET_FRAME_PATH_CONFIG_NR     (6)     ///< THe IOCTL NR definition for IOCTL_SCLMGWIN_SET_FRAME_PATH_CONFIG_NR
#define IOCTL_SCLMGWIN_GET_DBF_CONFIG_NR            (7)     ///< THe IOCTL NR definition for IOCTL_SCLMGWIN_GET_DBF_CONFIG_NR
#define IOCTL_SCLMGWIN_SET_REGISTER_CONFIG_NR       (8)     ///< THe IOCTL NR definition for IOCTL_SCLMGWIN_SET_REGISTER_CONFIG_NR
#define IOCTL_SCLMGWIN_MAX_NR                       (9)     ///< The Max IOCTL NR for SCLMGWIN driver


// use 'm' as magic number
#define IOCTL_SCLMGWIN_MAGIC                       ('3')///< The Type definition of IOCTL for hvsp driver


#define IOCTL_SCLMGWIN_SET_FRAME_CONFIG             _IO(IOCTL_SCLMGWIN_MAGIC,   IOCTL_SCLMGWIN_SET_FRAME_CONFIG_NR)
#define IOCTL_SCLMGWIN_SET_FRAME_ONOFF_CONFIG       _IO(IOCTL_SCLMGWIN_MAGIC,   IOCTL_SCLMGWIN_SET_FRAME_ONOFF_CONFIG_NR)
#define IOCTL_SCLMGWIN_SET_SUBWIN_CONFIG            _IO(IOCTL_SCLMGWIN_MAGIC,   IOCTL_SCLMGWIN_SET_SUBWIN_CONFIG_NR)
#define IOCTL_SCLMGWIN_SET_SUBWIN_FLIP_CONFIG       _IO(IOCTL_SCLMGWIN_MAGIC,   IOCTL_SCLMGWIN_SET_SUBWIN_FLIP_CONFIG_NR)
#define IOCTL_SCLMGWIN_SET_SUBWIN_ONOFF_CONFIG      _IO(IOCTL_SCLMGWIN_MAGIC,   IOCTL_SCLMGWIN_SET_SUBWIN_ONOFF_CONFIG_NR)
#define IOCTL_SCLMGWIN_SET_FRAME_PATH_CONFIG        _IO(IOCTL_SCLMGWIN_MAGIC,   IOCTL_SCLMGWIN_SET_FRAME_PATH_CONFIG_NR)
#define IOCTL_SCLMGWIN_SET_DBF_CONFIG               _IO(IOCTL_SCLMGWIN_MAGIC,   IOCTL_SCLMGWIN_SET_DBF_CONFIG_NR)
#define IOCTL_SCLMGWIN_GET_DBF_CONFIG               _IO(IOCTL_SCLMGWIN_MAGIC,   IOCTL_SCLMGWIN_GET_DBF_CONFIG_NR)
#define IOCTL_SCLMGWIN_SET_REGISTER_CONFIG          _IO(IOCTL_SCLMGWIN_MAGIC,   IOCTL_SCLMGWIN_SET_REGISTER_CONFIG_NR)

#endif //
/** @} */ // end of sclhvsp_group
