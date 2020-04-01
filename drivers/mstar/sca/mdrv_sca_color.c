///////////////////////////////////////////////////////////////////////////////
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
///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//
// @file   mdrv_sca_io.c
// @brief  SCA KMD Driver Interface
// @author MStar Semiconductor Inc.
//////////////////////////////////////////////////////////////////////////////////////////////////

//=============================================================================
// Include Files
//=============================================================================
#define MDRV_SCA_COLOR_C

#include <linux/module.h>
#include <linux/kernel.h>



#include "ms_platform.h"

#include "MsTypes.h"
#include "MsDevice.h"
#include "MsCommon.h"
#include "drvXC_IOPort.h"
#include "drvPQ.h"
#include "drvPQ_Define.h"
#include "apiXC.h"
#include "apiXC_PCMonitor.h"
#include "apiXC_ModeParse.h"
#include "apiPNL.h"
#include "apiXC_Ace.h"
#include "drvTVEncoder.h"

#include "drvMMIO.h"
#include "mdrv_sca_color.h"
#include "mdrv_sca_st.h"
#include "mdrv_sca.h"

//drver header files
#define COLOR_DBG(x) //
//=============================================================================
// Local Defines
//=============================================================================
MS_PICTURE_CONFIG stPictureCfg[MAX_WINDOW] =
{
    {
    //DTV
    {50, 50, 50, 50, 50, COLOR_TEMP_NORMAL, {INIT_SUB_BRIGHTNESS_DATA, INIT_SUB_CONTRAST_DATA}},
    //AV
    {50, 50, 50, 50, 50, COLOR_TEMP_NORMAL, {INIT_SUB_BRIGHTNESS_DATA, INIT_SUB_CONTRAST_DATA}},
    //YPbPr
    {50, 50, 50, 50, 50, COLOR_TEMP_NORMAL, {INIT_SUB_BRIGHTNESS_DATA, INIT_SUB_CONTRAST_DATA}},
    //PC
    {50,50, 50, 50, 50, COLOR_TEMP_NORMAL,  {INIT_SUB_BRIGHTNESS_DATA, INIT_SUB_CONTRAST_DATA}},
    //BT656
    {50,50, 50, 50, 50, COLOR_TEMP_NORMAL,  {INIT_SUB_BRIGHTNESS_DATA, INIT_SUB_CONTRAST_DATA}},
    },

    {
    //DTV
    {50, 50, 50, 50, 50, COLOR_TEMP_NORMAL, {INIT_SUB_BRIGHTNESS_DATA, INIT_SUB_CONTRAST_DATA}},
    //AV
    {50, 50, 50, 50, 50, COLOR_TEMP_NORMAL, {INIT_SUB_BRIGHTNESS_DATA, INIT_SUB_CONTRAST_DATA}},
    //YPbPr
    {50, 50, 50, 50, 50, COLOR_TEMP_NORMAL, {INIT_SUB_BRIGHTNESS_DATA, INIT_SUB_CONTRAST_DATA}},
    //PC
    {50,50, 50, 50, 50, COLOR_TEMP_NORMAL,  {INIT_SUB_BRIGHTNESS_DATA, INIT_SUB_CONTRAST_DATA}},
    //BT656
    {50,50, 50, 50, 50, COLOR_TEMP_NORMAL,  {INIT_SUB_BRIGHTNESS_DATA, INIT_SUB_CONTRAST_DATA}},
    },

    {
    //DTV
    {50, 50, 50, 50, 50, COLOR_TEMP_NORMAL, {INIT_SUB_BRIGHTNESS_DATA, INIT_SUB_CONTRAST_DATA}},
    //AV
    {50, 50, 50, 50, 50, COLOR_TEMP_NORMAL, {INIT_SUB_BRIGHTNESS_DATA, INIT_SUB_CONTRAST_DATA}},
    //YPbPr
    {50, 50, 50, 50, 50, COLOR_TEMP_NORMAL, {INIT_SUB_BRIGHTNESS_DATA, INIT_SUB_CONTRAST_DATA}},
    //PC
    {50,50, 50, 50, 50, COLOR_TEMP_NORMAL,  {INIT_SUB_BRIGHTNESS_DATA, INIT_SUB_CONTRAST_DATA}},
    //BT656
    {50,50, 50, 50, 50, COLOR_TEMP_NORMAL,  {INIT_SUB_BRIGHTNESS_DATA, INIT_SUB_CONTRAST_DATA}},
    },

    {
    //DTV
    {50, 50, 50, 50, 50, COLOR_TEMP_NORMAL, {INIT_SUB_BRIGHTNESS_DATA, INIT_SUB_CONTRAST_DATA}},
    //AV
    {50, 50, 50, 50, 50, COLOR_TEMP_NORMAL, {INIT_SUB_BRIGHTNESS_DATA, INIT_SUB_CONTRAST_DATA}},
    //YPbPr
    {50, 50, 50, 50, 50, COLOR_TEMP_NORMAL, {INIT_SUB_BRIGHTNESS_DATA, INIT_SUB_CONTRAST_DATA}},
    //PC
    {50,50, 50, 50, 50, COLOR_TEMP_NORMAL,  {INIT_SUB_BRIGHTNESS_DATA, INIT_SUB_CONTRAST_DATA}},
    //BT656
    {50,50, 50, 50, 50, COLOR_TEMP_NORMAL,  {INIT_SUB_BRIGHTNESS_DATA, INIT_SUB_CONTRAST_DATA}},
    },

    {
    //DTV
    {50, 50, 50, 50, 50, COLOR_TEMP_NORMAL, {INIT_SUB_BRIGHTNESS_DATA, INIT_SUB_CONTRAST_DATA}},
    //AV
    {50, 50, 50, 50, 50, COLOR_TEMP_NORMAL, {INIT_SUB_BRIGHTNESS_DATA, INIT_SUB_CONTRAST_DATA}},
    //YPbPr
    {50, 50, 50, 50, 50, COLOR_TEMP_NORMAL, {INIT_SUB_BRIGHTNESS_DATA, INIT_SUB_CONTRAST_DATA}},
    //PC
    {50,50, 50, 50, 50, COLOR_TEMP_NORMAL,  {INIT_SUB_BRIGHTNESS_DATA, INIT_SUB_CONTRAST_DATA}},
    //BT656
    {50,50, 50, 50, 50, COLOR_TEMP_NORMAL,  {INIT_SUB_BRIGHTNESS_DATA, INIT_SUB_CONTRAST_DATA}},
    },

    {
    //DTV
    {50, 50, 50, 50, 50, COLOR_TEMP_NORMAL, {INIT_SUB_BRIGHTNESS_DATA, INIT_SUB_CONTRAST_DATA}},
    //AV
    {50, 50, 50, 50, 50, COLOR_TEMP_NORMAL, {INIT_SUB_BRIGHTNESS_DATA, INIT_SUB_CONTRAST_DATA}},
    //YPbPr
    {50, 50, 50, 50, 50, COLOR_TEMP_NORMAL, {INIT_SUB_BRIGHTNESS_DATA, INIT_SUB_CONTRAST_DATA}},
    //PC
    {50,50, 50, 50, 50, COLOR_TEMP_NORMAL,  {INIT_SUB_BRIGHTNESS_DATA, INIT_SUB_CONTRAST_DATA}},
    //BT656
    {50,50, 50, 50, 50, COLOR_TEMP_NORMAL,  {INIT_SUB_BRIGHTNESS_DATA, INIT_SUB_CONTRAST_DATA}},
    },

};

static  MS_NONLINEAR_CURVE_SETTING tNonLinearCurveSetting[MAX_WINDOW] =
{
    {
    // DTV
    {
        {0, 80, 128, 160, 200},  // Contrast
        {0, 64, 128, 192, 255},  // Brightness
        {0, 96, 128, 160, 192},  // Color
        {0, 8, 15, 23, 63},      // Sharpness
        {30, 40, 50, 60, 70},    // Tint
    },

    // AV
    {
        {0, 80, 128, 160, 200},   // Contrast
        {0, 64, 128, 192, 255},  // Brightness
        {0, 96, 128, 160, 192},  // Color
        {0, 8, 15, 23, 63},      // Sharpness
        {30, 40, 50, 60, 70},    // Tint
    },

    // YPbPr
    {
        {0, 80, 128, 160, 200},   // Contrast
        {0, 64, 128, 192, 255},  // Brightness
        {0, 96, 128, 160, 192},  // Color
        {0, 8, 15, 23, 63},      // Sharpness
        {30, 40, 50, 60, 70},    // Tint
    },

    // PC
    {
        {80, 100, 120, 140, 160}, // Contrast
        {0, 64, 128, 192, 255},   // Brightness
        {0, 96, 128, 160, 192},   // Color
        {0, 8, 15, 23, 63},       // Sharpness
        {30, 40, 50, 60, 70},     // Tint
    },

    // BT656
    {
        {0, 80, 128, 160, 200},  // Contrast
        {0, 64, 128, 192, 255},  // Brightness
        {0, 96, 128, 160, 192},  // Color
        {0, 8, 15, 23, 63},      // Sharpness
        {30, 40, 50, 60, 70},    // Tint
    }
    },

    {
    // DTV
    {
        {0, 80, 128, 160, 200},  // Contrast
        {0, 64, 128, 192, 255},  // Brightness
        {0, 96, 128, 160, 192},  // Color
        {0, 8, 15, 23, 63},      // Sharpness
        {30, 40, 50, 60, 70},    // Tint
    },

    // AV
    {
        {0, 80, 128, 160, 200},   // Contrast
        {0, 64, 128, 192, 255},  // Brightness
        {0, 96, 128, 160, 192},  // Color
        {0, 8, 15, 23, 63},      // Sharpness
        {30, 40, 50, 60, 70},    // Tint
    },

    // YPbPr
    {
        {0, 80, 128, 160, 200},   // Contrast
        {0, 64, 128, 192, 255},  // Brightness
        {0, 96, 128, 160, 192},  // Color
        {0, 8, 15, 23, 63},      // Sharpness
        {30, 40, 50, 60, 70},    // Tint
    },

    // PC
    {
        {80, 100, 120, 140, 160}, // Contrast
        {0, 64, 128, 192, 255},   // Brightness
        {0, 96, 128, 160, 192},   // Color
        {0, 8, 15, 23, 63},       // Sharpness
        {30, 40, 50, 60, 70},     // Tint
    },

    // BT656
    {
        {0, 80, 128, 160, 200},  // Contrast
        {0, 64, 128, 192, 255},  // Brightness
        {0, 96, 128, 160, 192},  // Color
        {0, 8, 15, 23, 63},      // Sharpness
        {30, 40, 50, 60, 70},    // Tint
    }
    },

    {
    // DTV
    {
        {0, 80, 128, 160, 200},  // Contrast
        {0, 64, 128, 192, 255},  // Brightness
        {0, 96, 128, 160, 192},  // Color
        {0, 8, 15, 23, 63},      // Sharpness
        {30, 40, 50, 60, 70},    // Tint
    },

    // AV
    {
        {0, 80, 128, 160, 200},   // Contrast
        {0, 64, 128, 192, 255},  // Brightness
        {0, 96, 128, 160, 192},  // Color
        {0, 8, 15, 23, 63},      // Sharpness
        {30, 40, 50, 60, 70},    // Tint
    },

    // YPbPr
    {
        {0, 80, 128, 160, 200},   // Contrast
        {0, 64, 128, 192, 255},  // Brightness
        {0, 96, 128, 160, 192},  // Color
        {0, 8, 15, 23, 63},      // Sharpness
        {30, 40, 50, 60, 70},    // Tint
    },

    // PC
    {
        {80, 100, 120, 140, 160}, // Contrast
        {0, 64, 128, 192, 255},   // Brightness
        {0, 96, 128, 160, 192},   // Color
        {0, 8, 15, 23, 63},       // Sharpness
        {30, 40, 50, 60, 70},     // Tint
    },

    // BT656
    {
        {0, 80, 128, 160, 200},  // Contrast
        {0, 64, 128, 192, 255},  // Brightness
        {0, 96, 128, 160, 192},  // Color
        {0, 8, 15, 23, 63},      // Sharpness
        {30, 40, 50, 60, 70},    // Tint
    }
    },

    {
    // DTV
    {
        {0, 80, 128, 160, 200},  // Contrast
        {0, 64, 128, 192, 255},  // Brightness
        {0, 96, 128, 160, 192},  // Color
        {0, 8, 15, 23, 63},      // Sharpness
        {30, 40, 50, 60, 70},    // Tint
    },

    // AV
    {
        {0, 80, 128, 160, 200},   // Contrast
        {0, 64, 128, 192, 255},  // Brightness
        {0, 96, 128, 160, 192},  // Color
        {0, 8, 15, 23, 63},      // Sharpness
        {30, 40, 50, 60, 70},    // Tint
    },

    // YPbPr
    {
        {0, 80, 128, 160, 200},   // Contrast
        {0, 64, 128, 192, 255},  // Brightness
        {0, 96, 128, 160, 192},  // Color
        {0, 8, 15, 23, 63},      // Sharpness
        {30, 40, 50, 60, 70},    // Tint
    },

    // PC
    {
        {80, 100, 120, 140, 160}, // Contrast
        {0, 64, 128, 192, 255},   // Brightness
        {0, 96, 128, 160, 192},   // Color
        {0, 8, 15, 23, 63},       // Sharpness
        {30, 40, 50, 60, 70},     // Tint
    },

    // BT656
    {
        {0, 80, 128, 160, 200},  // Contrast
        {0, 64, 128, 192, 255},  // Brightness
        {0, 96, 128, 160, 192},  // Color
        {0, 8, 15, 23, 63},      // Sharpness
        {30, 40, 50, 60, 70},    // Tint
    }
    },

    {
    // DTV
    {
        {0, 80, 128, 160, 200},  // Contrast
        {0, 64, 128, 192, 255},  // Brightness
        {0, 96, 128, 160, 192},  // Color
        {0, 8, 15, 23, 63},      // Sharpness
        {30, 40, 50, 60, 70},    // Tint
    },

    // AV
    {
        {0, 80, 128, 160, 200},   // Contrast
        {0, 64, 128, 192, 255},  // Brightness
        {0, 96, 128, 160, 192},  // Color
        {0, 8, 15, 23, 63},      // Sharpness
        {30, 40, 50, 60, 70},    // Tint
    },

    // YPbPr
    {
        {0, 80, 128, 160, 200},   // Contrast
        {0, 64, 128, 192, 255},  // Brightness
        {0, 96, 128, 160, 192},  // Color
        {0, 8, 15, 23, 63},      // Sharpness
        {30, 40, 50, 60, 70},    // Tint
    },

    // PC
    {
        {80, 100, 120, 140, 160}, // Contrast
        {0, 64, 128, 192, 255},   // Brightness
        {0, 96, 128, 160, 192},   // Color
        {0, 8, 15, 23, 63},       // Sharpness
        {30, 40, 50, 60, 70},     // Tint
    },

    // BT656
    {
        {0, 80, 128, 160, 200},  // Contrast
        {0, 64, 128, 192, 255},  // Brightness
        {0, 96, 128, 160, 192},  // Color
        {0, 8, 15, 23, 63},      // Sharpness
        {30, 40, 50, 60, 70},    // Tint
    }
    },

    {
    // DTV
    {
        {0, 80, 128, 160, 200},  // Contrast
        {0, 64, 128, 192, 255},  // Brightness
        {0, 96, 128, 160, 192},  // Color
        {0, 8, 15, 23, 63},      // Sharpness
        {30, 40, 50, 60, 70},    // Tint
    },

    // AV
    {
        {0, 80, 128, 160, 200},   // Contrast
        {0, 64, 128, 192, 255},  // Brightness
        {0, 96, 128, 160, 192},  // Color
        {0, 8, 15, 23, 63},      // Sharpness
        {30, 40, 50, 60, 70},    // Tint
    },

    // YPbPr
    {
        {0, 80, 128, 160, 200},   // Contrast
        {0, 64, 128, 192, 255},  // Brightness
        {0, 96, 128, 160, 192},  // Color
        {0, 8, 15, 23, 63},      // Sharpness
        {30, 40, 50, 60, 70},    // Tint
    },

    // PC
    {
        {80, 100, 120, 140, 160}, // Contrast
        {0, 64, 128, 192, 255},   // Brightness
        {0, 96, 128, 160, 192},   // Color
        {0, 8, 15, 23, 63},       // Sharpness
        {30, 40, 50, 60, 70},     // Tint
    },

    // BT656
    {
        {0, 80, 128, 160, 200},  // Contrast
        {0, 64, 128, 192, 255},  // Brightness
        {0, 96, 128, 160, 192},  // Color
        {0, 8, 15, 23, 63},      // Sharpness
        {30, 40, 50, 60, 70},    // Tint
    }
    },
};


static T_MS_WHITEBALANCE  astDefaultWhiteBalanceDataTbl[MAX_WINDOW]=
{
    {
    {// DTV
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_B,

                INIT_VIDEO_COLOR_TEMP_COOL_R,
                INIT_VIDEO_COLOR_TEMP_COOL_G,
                INIT_VIDEO_COLOR_TEMP_COOL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_B,

                INIT_VIDEO_COLOR_TEMP_NORMAL_R,
                INIT_VIDEO_COLOR_TEMP_NORMAL_G,
                INIT_VIDEO_COLOR_TEMP_NORMAL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_B,

                INIT_VIDEO_COLOR_TEMP_WARM_R,
                INIT_VIDEO_COLOR_TEMP_WARM_G,
                INIT_VIDEO_COLOR_TEMP_WARM_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_B,

                INIT_VIDEO_COLOR_TEMP_USER_R,
                INIT_VIDEO_COLOR_TEMP_USER_G,
                INIT_VIDEO_COLOR_TEMP_USER_B,

                0x32,
                0x32,
                0x32,
            },

    },

    {// AV
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_B,

                INIT_VIDEO_COLOR_TEMP_COOL_R,
                INIT_VIDEO_COLOR_TEMP_COOL_G,
                INIT_VIDEO_COLOR_TEMP_COOL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_B,

                INIT_VIDEO_COLOR_TEMP_NORMAL_R,
                INIT_VIDEO_COLOR_TEMP_NORMAL_G,
                INIT_VIDEO_COLOR_TEMP_NORMAL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_B,

                INIT_VIDEO_COLOR_TEMP_WARM_R,
                INIT_VIDEO_COLOR_TEMP_WARM_G,
                INIT_VIDEO_COLOR_TEMP_WARM_B,

                0x32,
                0x32,
                0x32,
            },

            {
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_B,

                INIT_VIDEO_COLOR_TEMP_USER_R,
                INIT_VIDEO_COLOR_TEMP_USER_G,
                INIT_VIDEO_COLOR_TEMP_USER_B,

                0x32,
                0x32,
                0x32,
            },
    },

    {// YPBPR
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_B,

                INIT_VIDEO_COLOR_TEMP_COOL_R,
                INIT_VIDEO_COLOR_TEMP_COOL_G,
                INIT_VIDEO_COLOR_TEMP_COOL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_B,

                INIT_VIDEO_COLOR_TEMP_NORMAL_R,
                INIT_VIDEO_COLOR_TEMP_NORMAL_G,
                INIT_VIDEO_COLOR_TEMP_NORMAL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_B,

                INIT_VIDEO_COLOR_TEMP_WARM_R,
                INIT_VIDEO_COLOR_TEMP_WARM_G,
                INIT_VIDEO_COLOR_TEMP_WARM_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_B,

                INIT_VIDEO_COLOR_TEMP_USER_R,
                INIT_VIDEO_COLOR_TEMP_USER_G,
                INIT_VIDEO_COLOR_TEMP_USER_B,

                0x32,
                0x32,
                0x32,
            },
    },

    {// RGB
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_B,

                INIT_VIDEO_COLOR_TEMP_COOL_R,
                INIT_VIDEO_COLOR_TEMP_COOL_G,
                INIT_VIDEO_COLOR_TEMP_COOL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_B,

                INIT_VIDEO_COLOR_TEMP_NORMAL_R,
                INIT_VIDEO_COLOR_TEMP_NORMAL_G,
                INIT_VIDEO_COLOR_TEMP_NORMAL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_B,

                INIT_VIDEO_COLOR_TEMP_WARM_R,
                INIT_VIDEO_COLOR_TEMP_WARM_G,
                INIT_VIDEO_COLOR_TEMP_WARM_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_B,

                INIT_VIDEO_COLOR_TEMP_USER_R,
                INIT_VIDEO_COLOR_TEMP_USER_G,
                INIT_VIDEO_COLOR_TEMP_USER_B,

                0x32,
                0x32,
                0x32,
            },
    },

    {// BT656
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_B,

                INIT_VIDEO_COLOR_TEMP_COOL_R,
                INIT_VIDEO_COLOR_TEMP_COOL_G,
                INIT_VIDEO_COLOR_TEMP_COOL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_B,

                INIT_VIDEO_COLOR_TEMP_NORMAL_R,
                INIT_VIDEO_COLOR_TEMP_NORMAL_G,
                INIT_VIDEO_COLOR_TEMP_NORMAL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_B,

                INIT_VIDEO_COLOR_TEMP_WARM_R,
                INIT_VIDEO_COLOR_TEMP_WARM_G,
                INIT_VIDEO_COLOR_TEMP_WARM_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_B,

                INIT_VIDEO_COLOR_TEMP_USER_R,
                INIT_VIDEO_COLOR_TEMP_USER_G,
                INIT_VIDEO_COLOR_TEMP_USER_B,

                0x32,
                0x32,
                0x32,
            },
    },

    },

    {
    {// DTV
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_B,

                INIT_VIDEO_COLOR_TEMP_COOL_R,
                INIT_VIDEO_COLOR_TEMP_COOL_G,
                INIT_VIDEO_COLOR_TEMP_COOL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_B,

                INIT_VIDEO_COLOR_TEMP_NORMAL_R,
                INIT_VIDEO_COLOR_TEMP_NORMAL_G,
                INIT_VIDEO_COLOR_TEMP_NORMAL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_B,

                INIT_VIDEO_COLOR_TEMP_WARM_R,
                INIT_VIDEO_COLOR_TEMP_WARM_G,
                INIT_VIDEO_COLOR_TEMP_WARM_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_B,

                INIT_VIDEO_COLOR_TEMP_USER_R,
                INIT_VIDEO_COLOR_TEMP_USER_G,
                INIT_VIDEO_COLOR_TEMP_USER_B,

                0x32,
                0x32,
                0x32,
            },

    },

    {// AV
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_B,

                INIT_VIDEO_COLOR_TEMP_COOL_R,
                INIT_VIDEO_COLOR_TEMP_COOL_G,
                INIT_VIDEO_COLOR_TEMP_COOL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_B,

                INIT_VIDEO_COLOR_TEMP_NORMAL_R,
                INIT_VIDEO_COLOR_TEMP_NORMAL_G,
                INIT_VIDEO_COLOR_TEMP_NORMAL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_B,

                INIT_VIDEO_COLOR_TEMP_WARM_R,
                INIT_VIDEO_COLOR_TEMP_WARM_G,
                INIT_VIDEO_COLOR_TEMP_WARM_B,

                0x32,
                0x32,
                0x32,
            },

            {
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_B,

                INIT_VIDEO_COLOR_TEMP_USER_R,
                INIT_VIDEO_COLOR_TEMP_USER_G,
                INIT_VIDEO_COLOR_TEMP_USER_B,

                0x32,
                0x32,
                0x32,
            },
    },

    {// YPBPR
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_B,

                INIT_VIDEO_COLOR_TEMP_COOL_R,
                INIT_VIDEO_COLOR_TEMP_COOL_G,
                INIT_VIDEO_COLOR_TEMP_COOL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_B,

                INIT_VIDEO_COLOR_TEMP_NORMAL_R,
                INIT_VIDEO_COLOR_TEMP_NORMAL_G,
                INIT_VIDEO_COLOR_TEMP_NORMAL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_B,

                INIT_VIDEO_COLOR_TEMP_WARM_R,
                INIT_VIDEO_COLOR_TEMP_WARM_G,
                INIT_VIDEO_COLOR_TEMP_WARM_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_B,

                INIT_VIDEO_COLOR_TEMP_USER_R,
                INIT_VIDEO_COLOR_TEMP_USER_G,
                INIT_VIDEO_COLOR_TEMP_USER_B,

                0x32,
                0x32,
                0x32,
            },
    },

    {// RGB
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_B,

                INIT_VIDEO_COLOR_TEMP_COOL_R,
                INIT_VIDEO_COLOR_TEMP_COOL_G,
                INIT_VIDEO_COLOR_TEMP_COOL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_B,

                INIT_VIDEO_COLOR_TEMP_NORMAL_R,
                INIT_VIDEO_COLOR_TEMP_NORMAL_G,
                INIT_VIDEO_COLOR_TEMP_NORMAL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_B,

                INIT_VIDEO_COLOR_TEMP_WARM_R,
                INIT_VIDEO_COLOR_TEMP_WARM_G,
                INIT_VIDEO_COLOR_TEMP_WARM_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_B,

                INIT_VIDEO_COLOR_TEMP_USER_R,
                INIT_VIDEO_COLOR_TEMP_USER_G,
                INIT_VIDEO_COLOR_TEMP_USER_B,

                0x32,
                0x32,
                0x32,
            },
    },

    {// BT656
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_B,

                INIT_VIDEO_COLOR_TEMP_COOL_R,
                INIT_VIDEO_COLOR_TEMP_COOL_G,
                INIT_VIDEO_COLOR_TEMP_COOL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_B,

                INIT_VIDEO_COLOR_TEMP_NORMAL_R,
                INIT_VIDEO_COLOR_TEMP_NORMAL_G,
                INIT_VIDEO_COLOR_TEMP_NORMAL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_B,

                INIT_VIDEO_COLOR_TEMP_WARM_R,
                INIT_VIDEO_COLOR_TEMP_WARM_G,
                INIT_VIDEO_COLOR_TEMP_WARM_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_B,

                INIT_VIDEO_COLOR_TEMP_USER_R,
                INIT_VIDEO_COLOR_TEMP_USER_G,
                INIT_VIDEO_COLOR_TEMP_USER_B,

                0x32,
                0x32,
                0x32,
            },
    },
    },

    {
    {// DTV
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_B,

                INIT_VIDEO_COLOR_TEMP_COOL_R,
                INIT_VIDEO_COLOR_TEMP_COOL_G,
                INIT_VIDEO_COLOR_TEMP_COOL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_B,

                INIT_VIDEO_COLOR_TEMP_NORMAL_R,
                INIT_VIDEO_COLOR_TEMP_NORMAL_G,
                INIT_VIDEO_COLOR_TEMP_NORMAL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_B,

                INIT_VIDEO_COLOR_TEMP_WARM_R,
                INIT_VIDEO_COLOR_TEMP_WARM_G,
                INIT_VIDEO_COLOR_TEMP_WARM_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_B,

                INIT_VIDEO_COLOR_TEMP_USER_R,
                INIT_VIDEO_COLOR_TEMP_USER_G,
                INIT_VIDEO_COLOR_TEMP_USER_B,

                0x32,
                0x32,
                0x32,
            },

    },

    {// AV
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_B,

                INIT_VIDEO_COLOR_TEMP_COOL_R,
                INIT_VIDEO_COLOR_TEMP_COOL_G,
                INIT_VIDEO_COLOR_TEMP_COOL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_B,

                INIT_VIDEO_COLOR_TEMP_NORMAL_R,
                INIT_VIDEO_COLOR_TEMP_NORMAL_G,
                INIT_VIDEO_COLOR_TEMP_NORMAL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_B,

                INIT_VIDEO_COLOR_TEMP_WARM_R,
                INIT_VIDEO_COLOR_TEMP_WARM_G,
                INIT_VIDEO_COLOR_TEMP_WARM_B,

                0x32,
                0x32,
                0x32,
            },

            {
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_B,

                INIT_VIDEO_COLOR_TEMP_USER_R,
                INIT_VIDEO_COLOR_TEMP_USER_G,
                INIT_VIDEO_COLOR_TEMP_USER_B,

                0x32,
                0x32,
                0x32,
            },
    },

    {// YPBPR
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_B,

                INIT_VIDEO_COLOR_TEMP_COOL_R,
                INIT_VIDEO_COLOR_TEMP_COOL_G,
                INIT_VIDEO_COLOR_TEMP_COOL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_B,

                INIT_VIDEO_COLOR_TEMP_NORMAL_R,
                INIT_VIDEO_COLOR_TEMP_NORMAL_G,
                INIT_VIDEO_COLOR_TEMP_NORMAL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_B,

                INIT_VIDEO_COLOR_TEMP_WARM_R,
                INIT_VIDEO_COLOR_TEMP_WARM_G,
                INIT_VIDEO_COLOR_TEMP_WARM_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_B,

                INIT_VIDEO_COLOR_TEMP_USER_R,
                INIT_VIDEO_COLOR_TEMP_USER_G,
                INIT_VIDEO_COLOR_TEMP_USER_B,

                0x32,
                0x32,
                0x32,
            },
    },

    {// RGB
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_B,

                INIT_VIDEO_COLOR_TEMP_COOL_R,
                INIT_VIDEO_COLOR_TEMP_COOL_G,
                INIT_VIDEO_COLOR_TEMP_COOL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_B,

                INIT_VIDEO_COLOR_TEMP_NORMAL_R,
                INIT_VIDEO_COLOR_TEMP_NORMAL_G,
                INIT_VIDEO_COLOR_TEMP_NORMAL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_B,

                INIT_VIDEO_COLOR_TEMP_WARM_R,
                INIT_VIDEO_COLOR_TEMP_WARM_G,
                INIT_VIDEO_COLOR_TEMP_WARM_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_B,

                INIT_VIDEO_COLOR_TEMP_USER_R,
                INIT_VIDEO_COLOR_TEMP_USER_G,
                INIT_VIDEO_COLOR_TEMP_USER_B,

                0x32,
                0x32,
                0x32,
            },
    },

    {// BT656
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_B,

                INIT_VIDEO_COLOR_TEMP_COOL_R,
                INIT_VIDEO_COLOR_TEMP_COOL_G,
                INIT_VIDEO_COLOR_TEMP_COOL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_B,

                INIT_VIDEO_COLOR_TEMP_NORMAL_R,
                INIT_VIDEO_COLOR_TEMP_NORMAL_G,
                INIT_VIDEO_COLOR_TEMP_NORMAL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_B,

                INIT_VIDEO_COLOR_TEMP_WARM_R,
                INIT_VIDEO_COLOR_TEMP_WARM_G,
                INIT_VIDEO_COLOR_TEMP_WARM_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_B,

                INIT_VIDEO_COLOR_TEMP_USER_R,
                INIT_VIDEO_COLOR_TEMP_USER_G,
                INIT_VIDEO_COLOR_TEMP_USER_B,

                0x32,
                0x32,
                0x32,
            },
    },
    },
    {
    {// DTV
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_B,

                INIT_VIDEO_COLOR_TEMP_COOL_R,
                INIT_VIDEO_COLOR_TEMP_COOL_G,
                INIT_VIDEO_COLOR_TEMP_COOL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_B,

                INIT_VIDEO_COLOR_TEMP_NORMAL_R,
                INIT_VIDEO_COLOR_TEMP_NORMAL_G,
                INIT_VIDEO_COLOR_TEMP_NORMAL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_B,

                INIT_VIDEO_COLOR_TEMP_WARM_R,
                INIT_VIDEO_COLOR_TEMP_WARM_G,
                INIT_VIDEO_COLOR_TEMP_WARM_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_B,

                INIT_VIDEO_COLOR_TEMP_USER_R,
                INIT_VIDEO_COLOR_TEMP_USER_G,
                INIT_VIDEO_COLOR_TEMP_USER_B,

                0x32,
                0x32,
                0x32,
            },

    },

    {// AV
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_B,

                INIT_VIDEO_COLOR_TEMP_COOL_R,
                INIT_VIDEO_COLOR_TEMP_COOL_G,
                INIT_VIDEO_COLOR_TEMP_COOL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_B,

                INIT_VIDEO_COLOR_TEMP_NORMAL_R,
                INIT_VIDEO_COLOR_TEMP_NORMAL_G,
                INIT_VIDEO_COLOR_TEMP_NORMAL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_B,

                INIT_VIDEO_COLOR_TEMP_WARM_R,
                INIT_VIDEO_COLOR_TEMP_WARM_G,
                INIT_VIDEO_COLOR_TEMP_WARM_B,

                0x32,
                0x32,
                0x32,
            },

            {
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_B,

                INIT_VIDEO_COLOR_TEMP_USER_R,
                INIT_VIDEO_COLOR_TEMP_USER_G,
                INIT_VIDEO_COLOR_TEMP_USER_B,

                0x32,
                0x32,
                0x32,
            },
    },

    {// YPBPR
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_B,

                INIT_VIDEO_COLOR_TEMP_COOL_R,
                INIT_VIDEO_COLOR_TEMP_COOL_G,
                INIT_VIDEO_COLOR_TEMP_COOL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_B,

                INIT_VIDEO_COLOR_TEMP_NORMAL_R,
                INIT_VIDEO_COLOR_TEMP_NORMAL_G,
                INIT_VIDEO_COLOR_TEMP_NORMAL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_B,

                INIT_VIDEO_COLOR_TEMP_WARM_R,
                INIT_VIDEO_COLOR_TEMP_WARM_G,
                INIT_VIDEO_COLOR_TEMP_WARM_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_B,

                INIT_VIDEO_COLOR_TEMP_USER_R,
                INIT_VIDEO_COLOR_TEMP_USER_G,
                INIT_VIDEO_COLOR_TEMP_USER_B,

                0x32,
                0x32,
                0x32,
            },
    },

    {// RGB
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_B,

                INIT_VIDEO_COLOR_TEMP_COOL_R,
                INIT_VIDEO_COLOR_TEMP_COOL_G,
                INIT_VIDEO_COLOR_TEMP_COOL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_B,

                INIT_VIDEO_COLOR_TEMP_NORMAL_R,
                INIT_VIDEO_COLOR_TEMP_NORMAL_G,
                INIT_VIDEO_COLOR_TEMP_NORMAL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_B,

                INIT_VIDEO_COLOR_TEMP_WARM_R,
                INIT_VIDEO_COLOR_TEMP_WARM_G,
                INIT_VIDEO_COLOR_TEMP_WARM_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_B,

                INIT_VIDEO_COLOR_TEMP_USER_R,
                INIT_VIDEO_COLOR_TEMP_USER_G,
                INIT_VIDEO_COLOR_TEMP_USER_B,

                0x32,
                0x32,
                0x32,
            },
    },

    {// BT656
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_B,

                INIT_VIDEO_COLOR_TEMP_COOL_R,
                INIT_VIDEO_COLOR_TEMP_COOL_G,
                INIT_VIDEO_COLOR_TEMP_COOL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_B,

                INIT_VIDEO_COLOR_TEMP_NORMAL_R,
                INIT_VIDEO_COLOR_TEMP_NORMAL_G,
                INIT_VIDEO_COLOR_TEMP_NORMAL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_B,

                INIT_VIDEO_COLOR_TEMP_WARM_R,
                INIT_VIDEO_COLOR_TEMP_WARM_G,
                INIT_VIDEO_COLOR_TEMP_WARM_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_B,

                INIT_VIDEO_COLOR_TEMP_USER_R,
                INIT_VIDEO_COLOR_TEMP_USER_G,
                INIT_VIDEO_COLOR_TEMP_USER_B,

                0x32,
                0x32,
                0x32,
            },
    },
    },
    {
    {// DTV
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_B,

                INIT_VIDEO_COLOR_TEMP_COOL_R,
                INIT_VIDEO_COLOR_TEMP_COOL_G,
                INIT_VIDEO_COLOR_TEMP_COOL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_B,

                INIT_VIDEO_COLOR_TEMP_NORMAL_R,
                INIT_VIDEO_COLOR_TEMP_NORMAL_G,
                INIT_VIDEO_COLOR_TEMP_NORMAL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_B,

                INIT_VIDEO_COLOR_TEMP_WARM_R,
                INIT_VIDEO_COLOR_TEMP_WARM_G,
                INIT_VIDEO_COLOR_TEMP_WARM_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_B,

                INIT_VIDEO_COLOR_TEMP_USER_R,
                INIT_VIDEO_COLOR_TEMP_USER_G,
                INIT_VIDEO_COLOR_TEMP_USER_B,

                0x32,
                0x32,
                0x32,
            },

    },

    {// AV
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_B,

                INIT_VIDEO_COLOR_TEMP_COOL_R,
                INIT_VIDEO_COLOR_TEMP_COOL_G,
                INIT_VIDEO_COLOR_TEMP_COOL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_B,

                INIT_VIDEO_COLOR_TEMP_NORMAL_R,
                INIT_VIDEO_COLOR_TEMP_NORMAL_G,
                INIT_VIDEO_COLOR_TEMP_NORMAL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_B,

                INIT_VIDEO_COLOR_TEMP_WARM_R,
                INIT_VIDEO_COLOR_TEMP_WARM_G,
                INIT_VIDEO_COLOR_TEMP_WARM_B,

                0x32,
                0x32,
                0x32,
            },

            {
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_B,

                INIT_VIDEO_COLOR_TEMP_USER_R,
                INIT_VIDEO_COLOR_TEMP_USER_G,
                INIT_VIDEO_COLOR_TEMP_USER_B,

                0x32,
                0x32,
                0x32,
            },
    },

    {// YPBPR
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_B,

                INIT_VIDEO_COLOR_TEMP_COOL_R,
                INIT_VIDEO_COLOR_TEMP_COOL_G,
                INIT_VIDEO_COLOR_TEMP_COOL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_B,

                INIT_VIDEO_COLOR_TEMP_NORMAL_R,
                INIT_VIDEO_COLOR_TEMP_NORMAL_G,
                INIT_VIDEO_COLOR_TEMP_NORMAL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_B,

                INIT_VIDEO_COLOR_TEMP_WARM_R,
                INIT_VIDEO_COLOR_TEMP_WARM_G,
                INIT_VIDEO_COLOR_TEMP_WARM_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_B,

                INIT_VIDEO_COLOR_TEMP_USER_R,
                INIT_VIDEO_COLOR_TEMP_USER_G,
                INIT_VIDEO_COLOR_TEMP_USER_B,

                0x32,
                0x32,
                0x32,
            },
    },

    {// RGB
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_B,

                INIT_VIDEO_COLOR_TEMP_COOL_R,
                INIT_VIDEO_COLOR_TEMP_COOL_G,
                INIT_VIDEO_COLOR_TEMP_COOL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_B,

                INIT_VIDEO_COLOR_TEMP_NORMAL_R,
                INIT_VIDEO_COLOR_TEMP_NORMAL_G,
                INIT_VIDEO_COLOR_TEMP_NORMAL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_B,

                INIT_VIDEO_COLOR_TEMP_WARM_R,
                INIT_VIDEO_COLOR_TEMP_WARM_G,
                INIT_VIDEO_COLOR_TEMP_WARM_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_B,

                INIT_VIDEO_COLOR_TEMP_USER_R,
                INIT_VIDEO_COLOR_TEMP_USER_G,
                INIT_VIDEO_COLOR_TEMP_USER_B,

                0x32,
                0x32,
                0x32,
            },
    },

    {// BT656
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_B,

                INIT_VIDEO_COLOR_TEMP_COOL_R,
                INIT_VIDEO_COLOR_TEMP_COOL_G,
                INIT_VIDEO_COLOR_TEMP_COOL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_B,

                INIT_VIDEO_COLOR_TEMP_NORMAL_R,
                INIT_VIDEO_COLOR_TEMP_NORMAL_G,
                INIT_VIDEO_COLOR_TEMP_NORMAL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_B,

                INIT_VIDEO_COLOR_TEMP_WARM_R,
                INIT_VIDEO_COLOR_TEMP_WARM_G,
                INIT_VIDEO_COLOR_TEMP_WARM_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_B,

                INIT_VIDEO_COLOR_TEMP_USER_R,
                INIT_VIDEO_COLOR_TEMP_USER_G,
                INIT_VIDEO_COLOR_TEMP_USER_B,

                0x32,
                0x32,
                0x32,
            },
    },
    },

    {
    {// DTV
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_B,

                INIT_VIDEO_COLOR_TEMP_COOL_R,
                INIT_VIDEO_COLOR_TEMP_COOL_G,
                INIT_VIDEO_COLOR_TEMP_COOL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_B,

                INIT_VIDEO_COLOR_TEMP_NORMAL_R,
                INIT_VIDEO_COLOR_TEMP_NORMAL_G,
                INIT_VIDEO_COLOR_TEMP_NORMAL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_B,

                INIT_VIDEO_COLOR_TEMP_WARM_R,
                INIT_VIDEO_COLOR_TEMP_WARM_G,
                INIT_VIDEO_COLOR_TEMP_WARM_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_B,

                INIT_VIDEO_COLOR_TEMP_USER_R,
                INIT_VIDEO_COLOR_TEMP_USER_G,
                INIT_VIDEO_COLOR_TEMP_USER_B,

                0x32,
                0x32,
                0x32,
            },

    },

    {// AV
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_B,

                INIT_VIDEO_COLOR_TEMP_COOL_R,
                INIT_VIDEO_COLOR_TEMP_COOL_G,
                INIT_VIDEO_COLOR_TEMP_COOL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_B,

                INIT_VIDEO_COLOR_TEMP_NORMAL_R,
                INIT_VIDEO_COLOR_TEMP_NORMAL_G,
                INIT_VIDEO_COLOR_TEMP_NORMAL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_B,

                INIT_VIDEO_COLOR_TEMP_WARM_R,
                INIT_VIDEO_COLOR_TEMP_WARM_G,
                INIT_VIDEO_COLOR_TEMP_WARM_B,

                0x32,
                0x32,
                0x32,
            },

            {
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_B,

                INIT_VIDEO_COLOR_TEMP_USER_R,
                INIT_VIDEO_COLOR_TEMP_USER_G,
                INIT_VIDEO_COLOR_TEMP_USER_B,

                0x32,
                0x32,
                0x32,
            },
    },

    {// YPBPR
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_B,

                INIT_VIDEO_COLOR_TEMP_COOL_R,
                INIT_VIDEO_COLOR_TEMP_COOL_G,
                INIT_VIDEO_COLOR_TEMP_COOL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_B,

                INIT_VIDEO_COLOR_TEMP_NORMAL_R,
                INIT_VIDEO_COLOR_TEMP_NORMAL_G,
                INIT_VIDEO_COLOR_TEMP_NORMAL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_B,

                INIT_VIDEO_COLOR_TEMP_WARM_R,
                INIT_VIDEO_COLOR_TEMP_WARM_G,
                INIT_VIDEO_COLOR_TEMP_WARM_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_B,

                INIT_VIDEO_COLOR_TEMP_USER_R,
                INIT_VIDEO_COLOR_TEMP_USER_G,
                INIT_VIDEO_COLOR_TEMP_USER_B,

                0x32,
                0x32,
                0x32,
            },
    },

    {// RGB
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_B,

                INIT_VIDEO_COLOR_TEMP_COOL_R,
                INIT_VIDEO_COLOR_TEMP_COOL_G,
                INIT_VIDEO_COLOR_TEMP_COOL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_B,

                INIT_VIDEO_COLOR_TEMP_NORMAL_R,
                INIT_VIDEO_COLOR_TEMP_NORMAL_G,
                INIT_VIDEO_COLOR_TEMP_NORMAL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_B,

                INIT_VIDEO_COLOR_TEMP_WARM_R,
                INIT_VIDEO_COLOR_TEMP_WARM_G,
                INIT_VIDEO_COLOR_TEMP_WARM_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_B,

                INIT_VIDEO_COLOR_TEMP_USER_R,
                INIT_VIDEO_COLOR_TEMP_USER_G,
                INIT_VIDEO_COLOR_TEMP_USER_B,

                0x32,
                0x32,
                0x32,
            },
    },

    {// BT656
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_COOL_B,

                INIT_VIDEO_COLOR_TEMP_COOL_R,
                INIT_VIDEO_COLOR_TEMP_COOL_G,
                INIT_VIDEO_COLOR_TEMP_COOL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_NORMAL_B,

                INIT_VIDEO_COLOR_TEMP_NORMAL_R,
                INIT_VIDEO_COLOR_TEMP_NORMAL_G,
                INIT_VIDEO_COLOR_TEMP_NORMAL_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_WARM_B,

                INIT_VIDEO_COLOR_TEMP_WARM_R,
                INIT_VIDEO_COLOR_TEMP_WARM_G,
                INIT_VIDEO_COLOR_TEMP_WARM_B,

                0x32,
                0x32,
                0x32,
            },
            {
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_R,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_G,
                INIT_VIDEO_COLOR_BRIGHTNESS_USER_B,

                INIT_VIDEO_COLOR_TEMP_USER_R,
                INIT_VIDEO_COLOR_TEMP_USER_G,
                INIT_VIDEO_COLOR_TEMP_USER_B,

                0x32,
                0x32,
                0x32,
            },
    },
    },

};

//=============================================================================
// Local Function Prototypes
//=============================================================================
MS_BOOL MApi_GetColorTempTBL_From_PQ(SCALER_WIN eWindow, INPUT_SOURCE_TYPE_t enInputSourceType, T_MS_COLOR_TEMP *pColorTemp)
{
    MS_BOOL bRet = TRUE;
    MS_U8 u8Buf[E_PQ_PIC_COLORTEMP_NUM];
    PQ_WIN PQWin = eWindow == MAIN_WINDOW     ? PQ_MAIN_WINDOW :
                   eWindow == SUB_WINDOW      ? PQ_MAIN_WINDOW :
                   eWindow == SC2_MAIN_WINDOW ? PQ_SC2_MAIN_WINDOW :
                   eWindow == SC2_SUB_WINDOW  ? PQ_SC2_MAIN_WINDOW :
                                                PQ_SC1_MAIN_WINDOW;


    PQ_INPUT_SOURCE_TYPE enPQInput = IsSrcTypeDigitalVD(enInputSourceType)  ? PQ_INPUT_SOURCE_CVBS :
                                     IsSrcTypeYPbPr(enInputSourceType)      ? PQ_INPUT_SOURCE_YPBPR :
                                     IsSrcTypeVga(enInputSourceType)        ? PQ_INPUT_SOURCE_VGA :
                                     PQ_INPUT_SOURCE_DTV;

    if(MDrv_PQ_Get_Picture(PQWin, enPQInput, NULL, E_PQ_PICTURE_COLOR_TEMP, (void *)u8Buf))
    {
        SCA_DBG(SCA_DBG_LV_PQ_COLOR, "ColorTemp: color(%02x %02x %02x),  offset(%02x %02x %02x), scale(%02x %02x %02x) \n",
            u8Buf[E_PQ_PIC_COLORTEMP_VALUE_R], u8Buf[E_PQ_PIC_COLORTEMP_VALUE_G], u8Buf[E_PQ_PIC_COLORTEMP_VALUE_B],
            u8Buf[E_PQ_PIC_COLORTEMP_OFFSET_R], u8Buf[E_PQ_PIC_COLORTEMP_OFFSET_G], u8Buf[E_PQ_PIC_COLORTEMP_OFFSET_B],
            u8Buf[E_PQ_PIC_COLORTEMP_SCALE_R], u8Buf[E_PQ_PIC_COLORTEMP_SCALE_G], u8Buf[E_PQ_PIC_COLORTEMP_SCALE_B]);

        pColorTemp->cRedColor   = u8Buf[E_PQ_PIC_COLORTEMP_VALUE_R];
        pColorTemp->cGreenColor = u8Buf[E_PQ_PIC_COLORTEMP_VALUE_G];
        pColorTemp->cBlueColor  = u8Buf[E_PQ_PIC_COLORTEMP_VALUE_B];

        pColorTemp->cRedOffset   = u8Buf[E_PQ_PIC_COLORTEMP_OFFSET_R];
        pColorTemp->cGreenOffset = u8Buf[E_PQ_PIC_COLORTEMP_OFFSET_G];
        pColorTemp->cBlueOffset  = u8Buf[E_PQ_PIC_COLORTEMP_OFFSET_B];


        pColorTemp->cRedScaleValue   = u8Buf[E_PQ_PIC_COLORTEMP_SCALE_R];
        pColorTemp->cGreenScaleValue = u8Buf[E_PQ_PIC_COLORTEMP_SCALE_G];
        pColorTemp->cBlueScaleValue  = u8Buf[E_PQ_PIC_COLORTEMP_SCALE_B];
    }
    else
    {
        bRet = FALSE;
    }

    return bRet;
}

MS_BOOL MApi_GetNonliearValue_From_PQ(SCALER_WIN eWindow, INPUT_SOURCE_TYPE_t enInputSourceType, P_MS_USER_NONLINEAR_CURVE pNonLinearCurve, EN_MS_PICTURE_TYPE ePictureType)
{
    MS_BOOL bRet = TRUE;
    MS_U8 u8Buf[E_PQ_PIC_COLOR_NUM];
    PQ_WIN PQWin = eWindow == MAIN_WINDOW     ? PQ_MAIN_WINDOW :
                   eWindow == SUB_WINDOW      ? PQ_MAIN_WINDOW :
                   eWindow == SC2_MAIN_WINDOW ? PQ_SC2_MAIN_WINDOW :
                   eWindow == SC2_SUB_WINDOW  ? PQ_SC2_MAIN_WINDOW :
                                                PQ_SC1_MAIN_WINDOW;

    PQ_INPUT_SOURCE_TYPE enPQInput = IsSrcTypeDigitalVD(enInputSourceType)  ? PQ_INPUT_SOURCE_CVBS :
                                     IsSrcTypeYPbPr(enInputSourceType)      ? PQ_INPUT_SOURCE_YPBPR :
                                     IsSrcTypeVga(enInputSourceType)        ? PQ_INPUT_SOURCE_VGA :
                                     PQ_INPUT_SOURCE_DTV;
    if(ePictureType == PICTURE_SHARPNESS)
    {
        MS_U8 u8Sharpness = 0;
        if( MDrv_PQ_Get_Sharpness(PQWin, enPQInput, NULL, &u8Sharpness))
        {
            pNonLinearCurve->u8OSD_50 = u8Sharpness;
            SCA_DBG(SCA_DBG_LV_PQ_COLOR, "win:%d, Type:Sha, %x \n", eWindow, u8Sharpness);
            bRet = TRUE;
        }
        else
        {
            bRet = FALSE;
        }

    }
    else if(MDrv_PQ_Get_Picture(PQWin, enPQInput, NULL, E_PQ_PICTURE_COLOR_SETTING, (void *)u8Buf))
    {
        SCA_DBG(SCA_DBG_LV_PQ_COLOR, "win:%d, Type:%s Curve:%x, CON:%x, BRI:%x, HUE:%x, SAT:%x, SHA:%x \n",
                            eWindow,
                            ePictureType == PICTURE_CONTRAST    ? "con" :
                            ePictureType == PICTURE_BRIGHTNESS  ? "bri" :
                            ePictureType == PICTURE_HUE         ? "hue" :
                            ePictureType == PICTURE_SATURATION  ? "sat" :
                                                                  "sha",
                            pNonLinearCurve->u8OSD_50,
                            u8Buf[E_PQ_PIC_COLOR_CON],
                            u8Buf[E_PQ_PIC_COLOR_BRI],
                            u8Buf[E_PQ_PIC_COLOR_HUE],
                            u8Buf[E_PQ_PIC_COLOR_SAT],
                            u8Buf[E_PQ_PIC_COLOR_SHA]);


        switch(ePictureType)
        {
        case PICTURE_CONTRAST:
            pNonLinearCurve->u8OSD_50 = u8Buf[E_PQ_PIC_COLOR_CON];
            break;

        case PICTURE_BRIGHTNESS:
            pNonLinearCurve->u8OSD_50 = u8Buf[E_PQ_PIC_COLOR_BRI];
            break;

        case PICTURE_HUE:
            pNonLinearCurve->u8OSD_50 = u8Buf[E_PQ_PIC_COLOR_HUE];
            break;

        case PICTURE_SATURATION:
            pNonLinearCurve->u8OSD_50 = u8Buf[E_PQ_PIC_COLOR_SAT];
            break;

        case PICTURE_SHARPNESS:
        default:
            bRet = FALSE;
            break;
        }

    }
    else
    {
        bRet = FALSE;

    }

    return bRet;
}


//------------------------------------------------------------------------------
//  Internal function
//------------------------------------------------------------------------------

#if 0
U16 N100toReallyValue ( U16 wValue, U16 wMinValue, U16 wMaxValue )
{
    double dfTemp;
    wMaxValue = wMaxValue - wMinValue;
    if(!wValue)
        wValue=wMinValue;
    else
    {
        dfTemp=(double)(((wValue)*wMaxValue)/100.0+wMinValue);
        wValue=((wValue)*wMaxValue)/100+wMinValue;

        if((double)(dfTemp-wValue)>0.4)
            wValue=wValue+1;
    }
    return(U16) wValue;
}
#endif

U8 MApi_NonLinearCalculate(P_MS_USER_NONLINEAR_CURVE pNonLinearCurve, U8 AdjustValue)
{
    //dual direction
    MS_U8 rValue,ucY0,ucY1,ucX0,ucX1,ucIntercept;
    MS_U16 wDistanceOfY, wDistanceOfX;
    //if (AdjustValue < stNonLinearCurve.u8OSD_25)
    if (AdjustValue < 25)
    {
          ucY0 = pNonLinearCurve->u8OSD_0;
          ucY1 = pNonLinearCurve->u8OSD_25;
          ucX0 = 0;
          ucX1 = 25;
    }
    //else if (AdjustValue < stNonLinearCurve.u8OSD_50)
    else if (AdjustValue < 50)
    {
          ucY0 = pNonLinearCurve->u8OSD_25;
          ucY1 = pNonLinearCurve->u8OSD_50;
          ucX0 = 25;
          ucX1 = 50;
    }
    //else if (AdjustValue < stNonLinearCurve.u8OSD_75)
    else if (AdjustValue < 75)
    {
          ucY0 = pNonLinearCurve->u8OSD_50;
          ucY1 = pNonLinearCurve->u8OSD_75;
          ucX0 = 50;
          ucX1 = 75;
    }
    else
    {
          ucY0 = pNonLinearCurve->u8OSD_75;
          ucY1 = pNonLinearCurve->u8OSD_100;
          ucX0 = 75;
          ucX1 = 100;
    }

    if (ucY1 > ucY0)
    {
        wDistanceOfY = ucY1 - ucY0;
        wDistanceOfX = ucX1 - ucX0;
        ucIntercept  = ucY0;
        AdjustValue  = AdjustValue - ucX0;
    }
    else
    {
        wDistanceOfY = ucY0 - ucY1;
        wDistanceOfX = ucX1 - ucX0;
        ucIntercept  = ucY1;
        AdjustValue  = ucX1 - AdjustValue;
    }

 // printf("wDistanceOfY %u\n", wDistanceOfY);
 // printf("wDistanceOfX %u\n", wDistanceOfX);
 // printf("ucIntercept %bu\n", ucIntercept);
 // printf("AdjustValue %bu\n", AdjustValue);

    rValue = ((MS_U16)wDistanceOfY*AdjustValue/(wDistanceOfX)) + ucIntercept;
    return rValue;
}

P_MS_USER_NONLINEAR_CURVE MApi_GetNonLinearCurve(SCALER_WIN eWindow, INPUT_SOURCE_TYPE_t enInputSourceType, EN_MS_PICTURE_TYPE ePictureType)
{
    P_MS_USER_NONLINEAR_CURVE pCurveTable;

    if(ePictureType >= PICTURE_NUM)
        return NULL;

    if(IsSrcTypeDTV(enInputSourceType) || IsSrcTypeSC_VOP(enInputSourceType) || IsSrcTypeCamera(enInputSourceType))
    {
        pCurveTable = &tNonLinearCurveSetting[eWindow].DTVPictureCurve[ePictureType];
    }
    else if(IsSrcTypeVga(enInputSourceType))
    {
        pCurveTable = &tNonLinearCurveSetting[eWindow].PCPictureCurve[ePictureType];
    }
    else if(IsSrcTypeYPbPr(enInputSourceType))
    {
        pCurveTable = &tNonLinearCurveSetting[eWindow].YPbPrPictureCurve[ePictureType];
    }
    else if(IsSrcTypeAV(enInputSourceType) || IsSrcTypeSV(enInputSourceType))
    {
        pCurveTable = &tNonLinearCurveSetting[eWindow].AVPictureCurve[ePictureType];
    }
	else if(IsSrcTypeBT656(enInputSourceType))
	{
        pCurveTable = &tNonLinearCurveSetting[eWindow].BT656PictureCurve[ePictureType];
	}
    else
    {
        pCurveTable = NULL;
    }

#if PQ_ENABLE_COLOR
    if(pCurveTable)
    {
        MApi_GetNonliearValue_From_PQ(eWindow, enInputSourceType, pCurveTable, ePictureType);
    }
#endif

    return pCurveTable;
}



void MApi_SetColorTemp(INPUT_SOURCE_TYPE_t enInputSourceType, MS_COLOR_TEMP_TYPE enColorTempType, SCALER_WIN eWindow)
{
    if(IsSrcTypeDTV(enInputSourceType) || IsSrcTypeSC_VOP(enInputSourceType) || IsSrcTypeCamera(enInputSourceType))
    {
        stPictureCfg[eWindow].stDTVPicture.enColorTemp = enColorTempType;
    }
    else if(IsSrcTypeVga(enInputSourceType))
    {
        stPictureCfg[eWindow].stPCPicture.enColorTemp = enColorTempType;
    }
    else if(IsSrcTypeYPbPr(enInputSourceType))
    {
        stPictureCfg[eWindow].stYPbPrPicture.enColorTemp = enColorTempType;
    }
    else
    {
        stPictureCfg[eWindow].stAVPicture.enColorTemp = enColorTempType;
    }

    SCA_DBG(SCA_DBG_LV_COLOR, "SetColorTemp:Src %d  Temp %d\r\n",enInputSourceType ,enColorTempType);
}

MS_COLOR_TEMP_TYPE MApi_GetColorTemp(INPUT_SOURCE_TYPE_t enInputSourceType, SCALER_WIN eWindow)
{
    MS_COLOR_TEMP_TYPE enColorTempType;


    if(IsSrcTypeDTV(enInputSourceType) || IsSrcTypeSC_VOP(enInputSourceType) || IsSrcTypeCamera(enInputSourceType))
    {
        enColorTempType = stPictureCfg[eWindow].stDTVPicture.enColorTemp;
    }
    else if(IsSrcTypeVga(enInputSourceType))
    {
        enColorTempType = stPictureCfg[eWindow].stPCPicture.enColorTemp;
    }
    else if(IsSrcTypeYPbPr(enInputSourceType))
    {
        enColorTempType = stPictureCfg[eWindow].stYPbPrPicture.enColorTemp;
    }
    else
    {
        enColorTempType = stPictureCfg[eWindow].stAVPicture.enColorTemp;
    }

    SCA_DBG(SCA_DBG_LV_COLOR, "GetColorTemp:Src %d  Temp %d\r\n",enInputSourceType ,enColorTempType);

    return enColorTempType;
}



T_MS_COLOR_TEMP  MApi_GetColorTempTBL(INPUT_SOURCE_TYPE_t enInputSourceType, SCALER_WIN eWindow)
{
    T_MS_COLOR_TEMP stColorTemp;

    if(IsSrcTypeDTV(enInputSourceType) || IsSrcTypeSC_VOP(enInputSourceType) || IsSrcTypeCamera(enInputSourceType))
    {
        stColorTemp = astDefaultWhiteBalanceDataTbl[eWindow].DTVColorTempTbl[stPictureCfg[eWindow].stDTVPicture.enColorTemp];
    }
    else if(IsSrcTypeVga(enInputSourceType))
    {
        stColorTemp = astDefaultWhiteBalanceDataTbl[eWindow].PCColorTempTbl[stPictureCfg[eWindow].stPCPicture.enColorTemp];
    }
    else if(IsSrcTypeYPbPr(enInputSourceType))
    {
        stColorTemp = astDefaultWhiteBalanceDataTbl[eWindow].YPbPrColorTempTbl[stPictureCfg[eWindow].stYPbPrPicture.enColorTemp];
    }
	else if(IsSrcTypeBT656(enInputSourceType))
	{
        stColorTemp = astDefaultWhiteBalanceDataTbl[eWindow].BT656ColorTempTbl[stPictureCfg[eWindow].stBT656Picture.enColorTemp];
	}
    else
    {
        stColorTemp = astDefaultWhiteBalanceDataTbl[eWindow].AVColorTempTbl[stPictureCfg[eWindow].stAVPicture.enColorTemp];
    }

    return stColorTemp;
}

void MApi_SetSubColor(INPUT_SOURCE_TYPE_t enInputSourceType, MS_SUB_COLOR_TYPE enType, U8 u8val, SCALER_WIN eWindow)
{
    MS_PICTURE_COLOR_SETTING *pColorSetting = NULL;

    if(IsSrcTypeDTV(enInputSourceType) || IsSrcTypeSC_VOP(enInputSourceType) || IsSrcTypeCamera(enInputSourceType))
    {
        pColorSetting = &stPictureCfg[eWindow].stDTVPicture;
    }
    else if(IsSrcTypeVga(enInputSourceType))
    {
        pColorSetting = &stPictureCfg[eWindow].stPCPicture;
    }
    else if(IsSrcTypeYPbPr(enInputSourceType))
    {
        pColorSetting = &stPictureCfg[eWindow].stYPbPrPicture;
    }
    else
    {
        pColorSetting = &stPictureCfg[eWindow].stAVPicture;
    }

    if(pColorSetting)
    {
        if( enType == SUB_COLOR_CONTRAST)
        {
            pColorSetting->stSubColor.u8SubContrast = u8val;
        }
        else
        {
            pColorSetting->stSubColor.u8SubBrightness = u8val;
        }
    }
}

U8 MApi_GetSubColor(INPUT_SOURCE_TYPE_t enInputSourceType, MS_SUB_COLOR_TYPE enType, SCALER_WIN eWindow)
{
    U8 u8val;
    MS_PICTURE_COLOR_SETTING *pColorSetting = NULL;

    if(IsSrcTypeDTV(enInputSourceType) || IsSrcTypeSC_VOP(enInputSourceType) || IsSrcTypeCamera(enInputSourceType))
    {
        pColorSetting = &stPictureCfg[eWindow].stDTVPicture;
    }
    else if(IsSrcTypeVga(enInputSourceType))
    {
        pColorSetting = &stPictureCfg[eWindow].stPCPicture;
    }
    else if(IsSrcTypeYPbPr(enInputSourceType))
    {
        pColorSetting = &stPictureCfg[eWindow].stYPbPrPicture;
    }
	else if(IsSrcTypeBT656(enInputSourceType))
	{
        pColorSetting = &stPictureCfg[eWindow].stBT656Picture;
	}
    else
    {
        pColorSetting = &stPictureCfg[eWindow].stAVPicture;
    }

    if(pColorSetting)
    {
        u8val = enType == SUB_COLOR_CONTRAST ? pColorSetting->stSubColor.u8SubContrast : pColorSetting->stSubColor.u8SubBrightness;
    }
    else
    {
        u8val = enType == SUB_COLOR_CONTRAST ? INIT_SUB_CONTRAST_DATA : INIT_SUB_BRIGHTNESS_DATA ;
    }

    SCA_DBG(SCA_DBG_LV_COLOR, "GetSubColor:      type:%d, val:%d \n", enType, u8val);
    return u8val;
}

void MApi_SetPictureSetting(INPUT_SOURCE_TYPE_t enInputSourceType, EN_MS_PICTURE_TYPE ePictureType, U8 u8val, SCALER_WIN eWindow)
{
    MS_PICTURE_COLOR_SETTING *pColorSetting = NULL;

    if(ePictureType >= PICTURE_NUM)
        return;

    if(u8val > 100)
    {
        u8val = 100;
    }

    if(IsSrcTypeDTV(enInputSourceType) || IsSrcTypeSC_VOP(enInputSourceType) || IsSrcTypeCamera(enInputSourceType))
    {
        pColorSetting = &stPictureCfg[eWindow].stDTVPicture;
    }
    else if(IsSrcTypeVga(enInputSourceType))
    {
        pColorSetting = &stPictureCfg[eWindow].stPCPicture;
    }
    else if(IsSrcTypeYPbPr(enInputSourceType))
    {
        pColorSetting = &stPictureCfg[eWindow].stYPbPrPicture;
    }
    else
    {
        pColorSetting = &stPictureCfg[eWindow].stAVPicture;
    }

    if(pColorSetting)
    {
        switch(ePictureType)
        {
        case PICTURE_CONTRAST:
            pColorSetting->u8Contrast = u8val;
            break;
        case PICTURE_BRIGHTNESS:
            pColorSetting->u8Brightness = u8val;
            break;
        case PICTURE_HUE:
            pColorSetting->u8Hue = u8val;
            break;
        case PICTURE_SATURATION:
            pColorSetting->u8Saturation = u8val;
            break;
        case PICTURE_SHARPNESS:
            pColorSetting->u8Sharpness = u8val;
            break;
        default:
            break;
        }
    }
}

U8 MApi_GetPictureSetting(INPUT_SOURCE_TYPE_t enInputSourceType, EN_MS_PICTURE_TYPE ePictureType, SCALER_WIN eWindow)
{
    U8 u8val;
    MS_PICTURE_COLOR_SETTING *pColorSetting = NULL;

    if(ePictureType >= PICTURE_NUM)
        return 50;

    if(IsSrcTypeDTV(enInputSourceType) || IsSrcTypeSC_VOP(enInputSourceType) || IsSrcTypeCamera(enInputSourceType))
    {
        pColorSetting = &stPictureCfg[eWindow].stDTVPicture;
    }
    else if(IsSrcTypeVga(enInputSourceType))
    {
        pColorSetting = &stPictureCfg[eWindow].stPCPicture;
    }
    else if(IsSrcTypeYPbPr(enInputSourceType))
    {
        pColorSetting = &stPictureCfg[eWindow].stYPbPrPicture;
    }
    else if(IsSrcTypeAV(enInputSourceType) || IsSrcTypeSV(enInputSourceType))
    {
        pColorSetting = &stPictureCfg[eWindow].stAVPicture;
    }
	else if(IsSrcTypeBT656(enInputSourceType))
	{
        pColorSetting = &stPictureCfg[eWindow].stBT656Picture;
	}
    else
    {
        u8val = 50;
    }

    if(pColorSetting)
    {
        switch(ePictureType)
        {
        case PICTURE_CONTRAST:
            u8val = pColorSetting->u8Contrast;
            break;
        case PICTURE_BRIGHTNESS:
            u8val = pColorSetting->u8Brightness;
            break;
        case PICTURE_HUE:
            u8val = pColorSetting->u8Hue;
            break;
        case PICTURE_SATURATION:
            u8val = pColorSetting->u8Saturation;
            break;
        case PICTURE_SHARPNESS:
            u8val = pColorSetting->u8Sharpness;
            break;
        default:
            u8val = 50;
            break;
        }
    }

    SCA_DBG(SCA_DBG_LV_COLOR, "GetPictureSetting: type:%d, val:%d\r\n", ePictureType, u8val);
    return u8val;
}



/******************************************************************************/
///-This function will get true value of contrast for picture mode
///@param u8value: scaled value
///@return true value of contrast
//*************************************************************************
U8 MApi_PictureContrastN100toReallyValue ( SCALER_WIN eWindow, INPUT_SOURCE_TYPE_t enInputSourceType, U8 u8value )
{
    U8 u8RealValue = MApi_NonLinearCalculate(MApi_GetNonLinearCurve(eWindow, enInputSourceType, PICTURE_CONTRAST),u8value);
    SCA_DBG(SCA_DBG_LV_COLOR, "Real Contrast:%d -> %d \n",u8value ,u8RealValue);
    return u8RealValue;
}


/******************************************************************************/
///-This function will get true value of Brightness for picture mode
///@param u8value: scaled value
///@return true value of Brightness
//*************************************************************************
U8 MApi_PictureBrightnessN100toReallyValue ( SCALER_WIN eWindow, INPUT_SOURCE_TYPE_t enInputSourceType, U8 u8value )
{
    U8 u8RealValue = MApi_NonLinearCalculate(MApi_GetNonLinearCurve(eWindow, enInputSourceType, PICTURE_BRIGHTNESS),u8value);
    SCA_DBG(SCA_DBG_LV_COLOR, "Real Brightenss : %d -> %d \n",u8value,  u8RealValue);
    return u8RealValue;
}

/******************************************************************************/
///-This function will get true value of hue for picture mode
///@param u8value: scaled value
///@return  true value of hue true value of hue
//*************************************************************************
U8 MApi_PictureHueN100toReallyValue ( SCALER_WIN eWindow, INPUT_SOURCE_TYPE_t enInputSourceType, U8 u8value )
{
    U8 u8RealValue = MApi_NonLinearCalculate(MApi_GetNonLinearCurve(eWindow, enInputSourceType, PICTURE_HUE),u8value);
    SCA_DBG(SCA_DBG_LV_COLOR, "Real Hue :%d -> %d \n",u8value, u8RealValue);
    return u8RealValue;
}

/******************************************************************************/
///-This function will get true value of saturation for picture mode
///@param u8value: scaled saturation
///@return true value of saturation
//*************************************************************************
U8 MApi_PictureSaturationN100toReallyValue ( SCALER_WIN eWindow, INPUT_SOURCE_TYPE_t enInputSourceType, U8 u8value )
{
    U8 u8RealValue = MApi_NonLinearCalculate(MApi_GetNonLinearCurve(eWindow, enInputSourceType, PICTURE_SATURATION),u8value);
    SCA_DBG(SCA_DBG_LV_COLOR, "Real Saturation :%d -> %d  \n",u8value,  u8RealValue);

    return u8RealValue;
}

/******************************************************************************/
///-This function will get true value of sharpness for picture mode
///@param u8value: scaled sharpness
///@return true value of sharpness
//*************************************************************************
U8 MApi_PictureSharpnessN100toReallyValue ( SCALER_WIN eWindow, INPUT_SOURCE_TYPE_t enInputSourceType, U8 u8value )
{
    U8 u8RealValue = MApi_NonLinearCalculate(MApi_GetNonLinearCurve(eWindow, enInputSourceType, PICTURE_SHARPNESS),u8value);
    SCA_DBG(SCA_DBG_LV_COLOR, "Real Shaprness :%d -> %d  \n",u8value, u8RealValue);
    return u8RealValue;

}


static U8 su8Red_ADJ_Brightness = BRIGHTNESS_BYPASS;
static U8 su8Green_ADJ_Brightness = BRIGHTNESS_BYPASS;
static U8 su8Blue_ADJ_Brightness = BRIGHTNESS_BYPASS;
static U8 su8RedBrightness = BRIGHTNESS_BYPASS;
static U8 su8GreenBrightness = BRIGHTNESS_BYPASS;
static U8 su8BlueBrightness = BRIGHTNESS_BYPASS;

U8 MApi_ACE_transferRGB_Bri(U8 u8Brightness, U8 u8Brightnesstype)
{
    U8 su8Brightness = 0;
    U16 u16TempBrightness;
    switch(u8Brightnesstype)
    {
        case BRIGHTNESS_R:
            su8Red_ADJ_Brightness = u8Brightness;
            su8Brightness = su8RedBrightness;
            break;
        case BRIGHTNESS_G:
            su8Green_ADJ_Brightness = u8Brightness;
            su8Brightness = su8GreenBrightness;
            break;
        case BRIGHTNESS_B:
            su8Blue_ADJ_Brightness = u8Brightness;
            su8Brightness = su8BlueBrightness;
            break;
        default:
            break;
    }

    u16TempBrightness = (U16)(su8Brightness * u8Brightness) / BRIGHTNESS_BYPASS;
    u16TempBrightness = MIN(u16TempBrightness, 255);
    return (U8)u16TempBrightness;
}

U8 MApi_ACE_transfer_Bri(U8 u8Brightness, U8 u8Brightnesstype)
{
    U8 su8Brightness = 0;
    U16 u16TempBrightness;
    switch(u8Brightnesstype)
    {
        case BRIGHTNESS_R:
            su8RedBrightness =u8Brightness;
            su8Brightness = su8Red_ADJ_Brightness;
            break;
        case BRIGHTNESS_G:
            su8GreenBrightness =u8Brightness;
            su8Brightness = su8Green_ADJ_Brightness;
            break;
        case BRIGHTNESS_B:
            su8BlueBrightness =u8Brightness;
            su8Brightness = su8Blue_ADJ_Brightness;
            break;
        default:
            break;
    }

    u16TempBrightness = (U16)(su8Brightness * u8Brightness) / BRIGHTNESS_BYPASS;
    u16TempBrightness = MIN(u16TempBrightness, 255);
    return (U8)u16TempBrightness;
}

U8 MApi_FactoryAdjBrightness(U8 u8Brightness, U8 u8SubBrightness)
{
    U16 u16TempBrightness;
    u16TempBrightness = (U16)u8Brightness * u8SubBrightness / SUB_BRIGHTNESS_BYPASS;
    u16TempBrightness = MIN(u16TempBrightness, 255);
    return (U8)u16TempBrightness;
}

U8 MApi_FactoryContrast(U8 u8Contrast, U8 u8SubContrast)
{
    U16 u16TempContrast;
    u16TempContrast = (U16)u8Contrast * u8SubContrast / SUB_CONTRAST_BYPASS;

    u16TempContrast = MIN(u16TempContrast, 255);
    return (U8)u16TempContrast;
}

#if defined(SCA_ENABLE_DLC)
#if DLC_PARAMETER_ALGORITHM_MODE   //for NEW DLC ALGORITHM

MS_U8 tLumaCurve[16] =
    { 0x01, 0x09, 0x17, 0x31, 0x4F, 0x6A, 0x84, 0x9A, 0xAE, 0xC0, 0xCE, 0xDA, 0xE4, 0xEC, 0xF3, 0xFA };

MS_U8 tLumaCurve2_a[16] =
    { 0x02, 0x24, 0x43, 0x62, 0x7E, 0x98, 0xAE, 0xC0, 0xCE, 0xDA, 0xE2, 0xE9, 0xEF, 0xF4, 0xF8, 0xFC };

MS_U8 tLumaCurve2_b[16] =
    { 0x06, 0x10, 0x1B, 0x26, 0x33, 0x42, 0x53, 0x64, 0x76, 0x88, 0x9A, 0xAC, 0xBE, 0xD0, 0xE2, 0xF4 };

MS_U8 tDlcSlopLimit[17] =
    { 0x88, 0x90, 0x93, 0x92, 0x8E, 0x89, 0x84, 0x80, 0x7E, 0x7D, 0x7E, 0x81, 0x85, 0x88, 0x8B, 0x8C, 0x8A };

#else //for OLD DLC ALGORITHM

MS_U8 tLumaCurve[16] =
    { 0x02, 0x0C, 0x1C, 0x2F, 0x41, 0x53, 0x65, 0x78, 0x8B, 0x9B, 0xAC, 0xBA, 0xC8, 0xD5, 0xE5, 0xF7 };

MS_U8 tLumaCurve2_a[16] =
    { 0x02, 0x0C, 0x1A, 0x2C, 0x3E, 0x4D, 0x5D, 0x6E, 0x7E, 0x8F, 0x9E, 0xB0, 0xC0, 0xD2, 0xE3, 0xF6 };

MS_U8 tLumaCurve2_b[16] =
    { 0x07, 0x1E, 0x33, 0x43, 0x51, 0x5F, 0x6D, 0x7B, 0x89, 0x9A, 0xA8, 0xB7, 0xC7, 0xD7, 0xE7, 0xF8 };

MS_U8 tDlcSlopLimit[17] =
    { 0x88, 0x90, 0x93, 0x92, 0x8E, 0x89, 0x84, 0x80, 0x7E, 0x7D, 0x7E, 0x81, 0x85, 0x88, 0x8B, 0x8C, 0x8A };

#endif


#define PQ_DLC_BUF_SIZE (16*3+17)
MS_U8 g_u8PQ_DLC_Curve[PQ_DLC_BUF_SIZE];


MS_BOOL MApi_DLC_Get_From_PQ(SCALER_WIN eWindow)
{
    MS_U16 i;
    MS_BOOL bRet = FALSE;
    PQ_WIN PQWin = eWindow == MAIN_WINDOW     ? PQ_MAIN_WINDOW :
                   eWindow == SUB_WINDOW      ? PQ_MAIN_WINDOW :
                   eWindow == SC2_MAIN_WINDOW ? PQ_SC2_MAIN_WINDOW :
                   eWindow == SC2_SUB_WINDOW  ? PQ_SC2_MAIN_WINDOW :
                                                PQ_SC1_MAIN_WINDOW;

    if(MDrv_PQ_Get_Picture(PQWin, PQ_INPUT_SOURCE_NONE, NULL, E_PQ_PICTURE_DLC_CURVE, (void *)g_u8PQ_DLC_Curve))
    {
       for(i=0;i<16;i++)
        {
            tLumaCurve[i]    = g_u8PQ_DLC_Curve[i];
            tLumaCurve2_a[i] = g_u8PQ_DLC_Curve[i+16];
            tLumaCurve2_b[i] = g_u8PQ_DLC_Curve[i+32];
        }
        for(i=0;i<17;i++)
        {
            tDlcSlopLimit[i] = g_u8PQ_DLC_Curve[i+48];
        }

        bRet = TRUE;
    }
    return bRet;
}

MS_U8 mApi_DLC_GetLumaCurve(MS_U8 u8Index)
{
    return tLumaCurve[u8Index];
}

MS_U8 mApi_DLC_GetLumaCurve2_a(MS_U8 u8Index)
{
    return tLumaCurve2_a[u8Index];
}

MS_U8 mApi_DLC_GetLumaCurve2_b(MS_U8 u8Index)
{
    return tLumaCurve2_b[u8Index];
}
MS_U8 mApi_DLC_GetDlcHistogramLimitCurve(MS_U8 u8Index)
{
    return tDlcSlopLimit[u8Index];
}
#endif // defined(SCA_ENABLE_DLC)

