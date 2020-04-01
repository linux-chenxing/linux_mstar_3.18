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

#include <linux/pfn.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>          /* seems do not need this */
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <asm/string.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>

#if defined(NETLINK_SAMPLE)
#include <linux/netlink.h>
#include <linux/sched.h>
#include <net/sock.h>
#include <linux/proc_fs.h>
#endif

#include "ms_platform.h"
#include "ms_msys.h"
#include "irqs.h"

#include "MsTypes.h"
#include "drvXC_IOPort.h"
#include "apiXC.h"
#include "apiXC_PCMonitor.h"
#include "apiXC_ModeParse.h"
#include "apiXC_Adc.h"
#include "apiXC_Auto.h"
#include "apiXC_Ace.h"
#include "apiPNL.h"
#include "drvTVEncoder.h"
#include "drvMMIO.h"
#include "drvMVOP.h"
#include "drvAVD.h"
#include "msapi_vd.h"
#include "drvPQ_Define.h"
#include "apiXC_Dlc.h"
#include "apiHDMITx.h"

#include "mdrv_sca_modeparse.h"
#include "mdrv_sca_color.h"
//#include "../mst701/mdrv_mst701.h" Ryan
#include "mdrv_sca_inputsource.h"
#include "mdrv_sca_st.h"
#include "mdrv_sca.h"
#include "mdrv_sca_avd.h"
#include "mdrv_sca_io.h"
#include "mdrv_sca_hdmitx.h"
#include "drv_sc_isr.h"
#include "drvXC_ADC_Internal.h"
#include "mdrv_idac.h"
//#define UIO_INT_EXAMPLE //userspace interrupt handling (UIO)
//drver header files
#ifdef UIO_INT_EXAMPLE
#include <linux/uio_driver.h>
#endif


//=============================================================================
// Local Defines
//=============================================================================
#define MDRV_SCA_DEVICE_COUNT   1
#define MDRV_SCA_NAME           "msca"
#define MAX_FILE_HANDLE_SUPPRT  64
#define MDRV_NAME_SCA           "msca"
#define MDRV_MAJOR_SCA          0xea
#define MDRV_MINOR_SCA          0x00


//-------------------------------------------------------------------------------------------------
// Macros
//-------------------------------------------------------------------------------------------------
#define CMD_PARSING(x)  (x==IOCTL_SCA_CONNECT ? "IOCTL_SCA_CONNECT" : \
                         x==IOCTL_SCA_DISCONNECT ? "IOCTL_SCA_DISCONNECT" : \
                         x==IOCTL_SCA_SET_TIMING_WINDOW ? "IOCTL_SCA_SET_TIMING_WINDOW" :\
                         x==IOCTL_SCA_SET_MVOP ? "IOCTL_SCA_SET_MVOP" : \
                         x==IOCTL_SCA_SET_DISPLAY_MUTE ? "IOCTL_SCA_SET_DISPLAY_MUTE": \
                         x==IOCTL_SCA_YPBPR_VGA_MODE_MON_PAR ? "IOCTL_SCA_YPBPR_VGA_MODE_MON_PAR" : \
                         x==IOCTL_SCA_GET_MODE ? "IOCTL_SCA_GET_MODE" : \
                         x==IOCTL_SCA_SET_VE ? "IOCTL_SCA_SET_VE" : \
                         x==IOCTL_SCA_SET_WINDOW_ONOFF ? "IOCTL_SCA_SET_WINDOW_ONOFF" : \
                         x==IOCTL_SCA_SET_COLOR_KEY ? "IOCTL_SCA_SET_COLOR_KEY" : \
                         x==IOCTL_SCA_SET_MVOP_BASEADDR ? "IOCTL_SCA_SET_MVOP_BASEADDR" :\
                         x==IOCTL_SCA_GET_MVOP_BASEADDR ? "IOCTL_SCA_GET_MVOP_BASEADDR" : \
                         x==IOCTL_SCA_CHANGE_WINDOW ? "IOCTL_SCA_CHANGE_WINDOW" :\
                         x==IOCTL_SCA_SET_PICTURE ? "IOCTL_SCA_SET_PICTURE" :\
                         x==IOCTL_SCA_GET_PICTURE ? "IOCTL_SCA_GET_PICTURE" : \
                         x==IOCTL_SCA_SET_OUTPUT_TIMING ? "IOCTL_SCA_SET_OUTPUT_TIMING" : \
                         x==IOCTL_SCA_CALIBRATION ? "IOCTL_SCA_CALIBRATION" : \
                         x==IOCTL_SCA_LOAD_ADC_SETTING ? "IOCTL_SCA_LOAD_ADC_SETTING" : \
                         x==IOCTL_SCA_AVD_CONNECT ? "IOCTL_SCA_AVD_CONNECT" : \
                         x==IOCTL_SCA_AVD_CHECK_VIDEO_STD ? "IOCTL_SCA_AVD_CHECK_VIDEO_STD" :\
                         x==IOCTL_SCA_GET_LIB_VER ? "IOCTL_SCA_GET_LIB_VER" : \
                         x==IOCTL_SCA_SET_CONSTANTALPHA_VALUE ? "IOCTL_SCA_SET_CONSTANTALPHA_VALUE" :\
                         x==IOCTL_SCA_SET_DISP_INTR ? "IOCTL_SCA_SET_DISP_INTR" : \
                         x==IOCTL_SCA_GET_DISP_INTR ? "IOCTL_SCA_GET_DISP_INTR" : \
                         x==IOCTL_SCA_RW_REGISTER ? "IOCTL_SCA_RW_REGISTER" :\
                         x==IOCTL_SCA_GET_ACE_INFO ? "IOCTL_SCA_GET_ACE_INFO" : \
                         x==IOCTL_SCA_GET_DLC_INFO ? "IOCTL_SCA_GET_DLC_INFO" :\
                         x==IOCTL_SCA_GET_DISP_PATH_CONFIG ? "IOCTL_SCA_GET_DISP_PATH_CONFIG" : \
                         x==IOCTL_SCA_SET_MIRROR_CONFIG ? "IOCTL_SCA_SET_MIRROR_CONFIG" : \
                         x==IOCTL_SCA_SET_PQ_BIN ? "IOCTL_SCA_SET_PQ_BIN" : \
                         x==IOCTL_SCA_SET_DIP_CONFIG ? "IOCTL_SCA_SET_DIP_CONFIG" : \
                         x==IOCTL_SCA_GET_DISP_INTR_STATUS ? "IOCTL_SCA_GET_DISP_INTR_STATUS" : \
                         x==IOCTL_SCA_SET_HDMITX_CONFIG ? "IOCTL_SCA_SET_HDMITX_CONFIG" : \
                         x==IOCTL_SCA_SET_DIP_WONCE_BASE_CONFIG ? "IOCTL_SCA_SET_DIP_WONCE_BASE_CONFIG" : \
                         x==IOCTL_SCA_SET_FREEZE_CONFIG ? "IOCTL_SCA_SET_FREEZE_CONFIG" : \
                         x==IOCTL_SCA_INIT_MST701 ? "IOCTL_SCA_INIT_MST701" : \
                         x==IOCTL_SCA_GEOMETRY_CALIBRATION ? "IOCTL_SCA_GEOMETRY_CALIBRATION" : \
                         x==IOCTL_SCA_LOAD_GEOMETRY_SETTING ? "IOCTL_SCA_LOAD_GEOMETRY_SETTING" : \
                         x==IOCTL_SCA_SET_ANALOG_POLLING_CONFIG ? "IOCTL_SCA_SET_ANALOG_POLLING_CONFIG" :\
                         x==IOCTL_SCA_SET_CLONE_SCREEN_CONFIG ? "IOCTL_SCA_SET_CLONE_SCREEN_CONFIG" :\
                         x==IOCTL_SCA_GET_CLONE_SCREEN_CONFIG ? "IOCTL_SCA_GET_CLONE_SCREEN_CONFIG" :\
                         x==IOCTL_SCA_SET_PNL_SSC_CONFIG ? "IOCTL_SCA_SET_PNL_SSC_CONFIG" : \
                         x==IOCTL_SCA_SET_CONSTANTALPHA_STATE ? "IOCTL_SCA_SET_CONSTANTALPHA_STATE" : \
                         x==IOCTL_SCA_SET_PNL_TIMING_CONFIG ? "IOCTL_SCA_SET_PNL_TIMING_CONFIG" : \
                         x==IOCTL_SCA_SET_CAMERA_INPUTTIMING_CONFIG ? "IOCTL_SCA_SET_CAMERA_INPUTTIMING_CONFIG" : \
                         x==IOCTL_SCA_SET_PQ_BIN_IBC? "IOCTL_SCA_SET_PQ_BIN_IBC" : \
                         x==IOCTL_SCA_SET_PQ_BIN_ICC? "IOCTL_SCA_SET_PQ_BIN_ICC" : \
                         x==IOCTL_SCA_SET_PQ_BIN_IHC? "IOCTL_SCA_SET_PQ_BIN_IHC" : \
                         "unknown")

#define ENABLE_MONITOR_THREAD          1
#define ENABLE_DIPW_INTR               0

#define MAK_UID(s, d, p, dbg, t)	((s<<28)|(d<<24)|(p<<20)|(dbg<<16)|(t&0xFFFF))
#define NONE_UID                    (0xFFFFFFFF)

//time can't be 0
#define DBG_UID                     (0xBEEF0000)
//--------------------------------------------------------------------------------------------------
// IOCtrl functions declaration
//--------------------------------------------------------------------------------------------------
#if ENABLE_MONITOR_THREAD

typedef struct
{
    AVD_CHECK_VIDEOSTD_CONFIG stVideoStdCfg;
    SCA_MODE_MONITOR_PARSE_OUT_CONFIG stModeParseOutCfg;
    SCA_SRC_TYPE enSrcType;
    SCA_DEST_TYPE enDestType;
    struct task_struct *pThread;
    MS_BOOL bRunning;
    MS_U32 u32CnntID;
}SCA_MONITOR_CONFIG;

typedef struct
{
    SCA_WINDOW_CONFIG stWinConfig;
    SCA_SRC_TYPE enSrcType;
}SCA_MONITOR_WIN_CONFIG;

char ThreadName[SCA_DEST_NUM][30] =
{
    {"MONITOR_THREAD_SC0_M"},
    {"MONITOR_THREAD_SC0_S"},
    {"MONITOR_THREAD_SC1_M"},
    {"MONITOR_THREAD_SC2_M"},
    {"MONITOR_THREAD_SC2_S"},
    {"MONITOR_THREAD_CVBSO"},
    {"MONITOR_THREAD_HDMITX"},
};

SCA_MONITOR_CONFIG         g_stMonitorThread[SCA_DEST_NUM];
SCA_MONITOR_WIN_CONFIG     g_stMonitorWinCfg[SCA_DEST_NUM];
SCA_SRC_TYPE               g_enPreSrcType[SCA_DEST_NUM];
SCA_MONITOR_STATUS_CONFIG  g_stMonitorStatusCfg[SCA_DEST_NUM];
#endif

SCA_DISP_PATH_CONFIG g_stDispPathCfg[SCA_DEST_NUM];

char DLC_Monitor_Thread_name[30] = {"MONITOR_THREAD_DLC"};
struct task_struct *pDLCMonitorThread = NULL;
struct task_struct *pHDMITxHPDThread = NULL;
struct kobject *gpkobj = NULL;

static SCA_DISP_INTR_SETTING g_stDispIntrSetting;
MS_BOOL g_bMST701Inited = FALSE;
int g_bScaAlive = FALSE;

XC_TGEN_TIMING_TYPE g_enTGenTiming = E_XC_TGEN_VE_480_I_60;

#if defined(CONFIG_MS_CLONE_SCREEN)
SCA_CLONE_SCREEN_TYPE g_enCloeScreenMode = SCA_CLONE_SCREEN_GOP;
MS_U16 g_u16Clone_Hratiox10 = 0;
MS_U16 g_u16Clone_Vratiox10 = 0;
#endif

SCA_HDMITX_CONFIG g_HDMI_Cfg = {0};
//------------------------------------------------------------------------------
#define SCA_MUTEX_USE_SEM  1
#define SCA_MUTEX_DBG      0
#define _SCA_MUTEX_TIME_OUT 5000

#if SCA_MUTEX_USE_SEM
struct semaphore _SCA_Mute_Mutex;
struct semaphore _SCA_Cnnt_Mutex;
struct semaphore _SCA_MoniWinCfg_Mutex;
struct semaphore _SCA_XC_AVD_Init_Mutex;
struct semaphore _SCA_PowerState_Mutex;
#else
//todo: change to semaphore
MS_S32  _SCA_Mute_Mutex = -1;
MS_S32  _SCA_Cnnt_Mutex = -1;
MS_S32  _SCA_MoniWinCfg_Mutex = -1;
MS_S32  _SCA_XC_AVD_Init_Mutex = -1;
MS_S32  _SCA_PowerState_Mutex = -1
#endif

//choose to use the semaphore
#if SCA_MUTEX_USE_SEM

#define SCA_WAIT_MUTEX_FOREVER(_mutex_)		down(&_mutex_)
#define SCA_RELEASE_MUTEX_FOREVER(_mutex_)  up(&_mutex_)

#if SCA_MUTEX_DBG

#define SCA_WAIT_MUTEX(_mutex_)                                                 \
        printk(KERN_INFO "+++ [%s]_1_[%d] \n", __FUNCTION__, __LINE__);         \
        if(down_timeout(&_mutex_, msecs_to_jiffies(_SCA_MUTEX_TIME_OUT)) < 0)  \
        {                                                                       \
            printk(KERN_INFO "[%s] [%d], SCA Mutex Time Out \n", __FUNCTION__, __LINE__); \
        }   \
        printk(KERN_INFO "+++ [%s]_2_[%d] \n", __FUNCTION__, __LINE__);


#define SCA_RELEASE_MUTEX(_mutex_)                                              \
        printk(KERN_INFO "---  [%s] [%d] \n", __FUNCTION__, __LINE__);          \
        up(&_mutex_);


#else
#define SCA_WAIT_MUTEX(_mutex_)                                               \
        if(down_timeout(&_mutex_, msecs_to_jiffies(_SCA_MUTEX_TIME_OUT)) < 0)    \
        {                                                                     \
            printk(KERN_INFO "[%s] [%d], SCA Mutex Time Out \n", __FUNCTION__, __LINE__); \
        }

#define SCA_RELEASE_MUTEX(_mutex_)  up(&_mutex_)

#endif //SCA_MUTEX_DBG

#else

#if SCA_MUTEX_DBG

#define SCA_WAIT_MUTEX(_mutex_)                                                 \
        printk(KERN_INFO "+++ [%s]_1_[%d] \n", __FUNCTION__, __LINE__);         \
        if(!MsOS_ObtainMutex(_mutex_, _SCA_MUTEX_TIME_OUT))                     \
        {                                                                       \
            printk(KERN_INFO "[%s] [%d], SCA Mutex Time Out \n", __FUNCTION__, __LINE__); \
        }   \
        printk(KERN_INFO "+++ [%s]_2_[%d] \n", __FUNCTION__, __LINE__);


#define SCA_RELEASE_MUTEX(_mutex_)                                              \
        printk(KERN_INFO "---  [%s] [%d] \n", __FUNCTION__, __LINE__);          \
        MsOS_ReleaseMutex(_mutex_);


#else
#define SCA_WAIT_MUTEX(_mutex_)                                               \
        if(!MsOS_ObtainMutex(_mutex_, _SCA_MUTEX_TIME_OUT))                   \
        {                                                                     \
        }

#define SCA_RELEASE_MUTEX(_mutex_)  MsOS_ReleaseMutex(_mutex_)

#endif //SCA_MUTEX_DBG

#endif //SCA_MUTEX_USE_SEM

struct completion _SCA_MoniThread_Comp;
struct completion _SCA_MoniThread_Stop_Comp;

#if  defined(NETLINK_SAMPLE)
static struct sock *netlink_sock;
#endif

//------------------------------------------------------------------------------
static int mstar_sca_drv_open (struct inode *inode, struct file *filp);
static int mstar_sca_drv_release(struct inode *inode, struct file *filp);
static long mstar_sca_drv_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
static int _MDrv_SCAIO_ModuleInit(void);
static void _MDrv_SCAIO_ModuleExit(void);

static void _Prog_Window(INPUT_SOURCE_TYPE_t enInputSourceType, SCALER_WIN eWindow, SCA_WINDOW_CONFIG *pWinCfg );


MS_BOOL SCA_Connect(PSCA_CONNECT_CONFIG pConfig);
static MS_BOOL _Set_Display_Mute(PSCA_DISPLAY_MUTE_CONFIG pConfig, E_SCREEN_MUTE_STATUS enMuteStatus);
MS_BOOL SCA_Set_Display_Mute(PSCA_DISPLAY_MUTE_CONFIG pConfig, E_SCREEN_MUTE_STATUS enMuteStatus);
MS_BOOL SCA_YPBPR_VGA_Mode_Monitor_Parse(PSCA_MODE_MONITOR_PARSE_IN_CONFIG pInConfig, PSCA_MODE_MONITOR_PARSE_OUT_CONFIG pOutConfig);
MS_BOOL SCA_Set_Timing_Window(PSCA_SET_TIMING_WINDOW_CONFIG pConfig);
MS_BOOL SCA_AVD_Check_VideoStd(AVD_CHECK_VIDEOSTD_CONFIG *pCfg);
void mstar_sca_drv_init_pnl(MS_BOOL bSkipTime);

extern MS_VIDEO_Window_Info CVBS_WinInfo[SIG_NUMS][1];

// PQ Bin

SCA_PQ_BIN_DATA stPQBinData[SCA_PQ_BIN_ID_NUM] =
{
    {SCA_PQ_BIN_ID_NONE, 0, 0}, {SCA_PQ_BIN_ID_NONE, 0, 0},
    {SCA_PQ_BIN_ID_NONE, 0, 0}, {SCA_PQ_BIN_ID_NONE, 0, 0},
    {SCA_PQ_BIN_ID_NONE, 0, 0}, {SCA_PQ_BIN_ID_NONE, 0, 0},
    {SCA_PQ_BIN_ID_NONE, 0, 0}, {SCA_PQ_BIN_ID_NONE, 0, 0},
    {SCA_PQ_BIN_ID_NONE, 0, 0}, {SCA_PQ_BIN_ID_NONE, 0, 0},
};
//=============================================================================
// Local Variables: Device handler
//=============================================================================
typedef struct
{
    int s32Major;
    int s32Minor;
    int refCnt;
    struct cdev cdev;
    struct file_operations fops;
}SCA_DEV;

static SCA_DEV _devSCA =
{
    .s32Major = MDRV_MAJOR_SCA,
    .s32Minor = MDRV_MINOR_SCA,
    .refCnt = 0,
    .cdev =
    {
        .kobj = {.name= MDRV_NAME_SCA, },
        .owner = THIS_MODULE,
    },
    .fops =
    {
        .open = mstar_sca_drv_open,
        .release = mstar_sca_drv_release,
        .unlocked_ioctl = mstar_sca_drv_ioctl,
    }
};

static struct class * sca_class;
static char * sca_classname = "mstar_sca_class";

//=============================================================================
// Local Function Prototypes
//=============================================================================
dma_addr_t  sg_sca_sc0_main_bus_addr;
dma_addr_t  sg_sca_sc0_sub_bus_addr;
dma_addr_t  sg_sca_sc1_main_bus_addr;
dma_addr_t  sg_sca_sc2_main_bus_addr;
dma_addr_t  sg_sca_sc2_sub_bus_addr;
dma_addr_t  sg_sca_mload_bus_addr;
dma_addr_t  sg_sca_avd_bus_addr;

static void *sg_sca_sc0_main_vir_addr = 0;
static void *sg_sca_sc0_sub_vir_addr = 0;
static void *sg_sca_sc1_main_vir_addr = 0;
static void *sg_sca_sc2_main_vir_addr = 0;
static void *sg_sca_sc2_sub_vir_addr = 0;
static void *sg_sca_mload_vir_addr = 0;
static void *sg_sca_avd_vir_addr = 0;

static u_long sg_sca_main_size  = 0x00C0A800 ;
static u_long sg_sca_sub_size   = 0x00610000 ;
static u_long sg_sca_mload_size = 0x00004000 ;
static u_long sg_sca_avd_size   = 0x00400000;

static SCA_LIB_VER sca_lib_ver = { MSIF_SCA_LIB_CODE,
                                   MSIF_SCA_BUILDNUM,
                                   MSIF_SCA_LIBVER,
                                   MSIF_SCA_CHANGELIST,
                                  };
static XC_INIT_INFO gstInitInfo;
//------------------------------------------------------------------------------

unsigned long long ms_mem_bus_to_MIU(unsigned long long x)
{
  #define MIU0_BUS_ADDR           0x40000000

	return ((x) - MIU0_BUS_ADDR);
}


static INPUT_SOURCE_TYPE_t _Trans_To_Drv_Src(SCA_SRC_TYPE enSrc)
{
    INPUT_SOURCE_TYPE_t enInputSourceType;
    switch(enSrc)
    {
    case SCA_SRC_VGA:
        enInputSourceType = INPUT_SOURCE_VGA;
        break;

    case SCA_SRC_YPBPR:
        enInputSourceType = INPUT_SOURCE_YPBPR;
        break;

    case SCA_SRC_DTV:
        enInputSourceType = INPUT_SOURCE_DTV;
        break;

    case SCA_SRC_CVBS:
        enInputSourceType = INPUT_SOURCE_CVBS;
        break;
    case SCA_SRC_CVBS2:
        enInputSourceType = INPUT_SOURCE_CVBS2;
        break;
   case SCA_SRC_CVBS3:
        enInputSourceType = INPUT_SOURCE_CVBS3;
        break;

	// add s-video input source //
	case SCA_SRC_SVIDEO:
        enInputSourceType = INPUT_SOURCE_SVIDEO;
        break;

    case SCA_SRC_SC0_VOP:
        enInputSourceType = INPUT_SOURCE_SC0_VOP;
        break;

    case SCA_SRC_SC1_VOP:
        enInputSourceType = INPUT_SOURCE_SC1_VOP;
        break;

    case SCA_SRC_SC2_VOP:
        enInputSourceType = INPUT_SOURCE_SC2_VOP;
        break;

    case SCA_SRC_BT656:
        enInputSourceType = INPUT_SOURCE_BT656_0;
        break;

    case SCA_SRC_BT656_1:
        enInputSourceType = INPUT_SOURCE_BT656_1;
        break;

    case SCA_SRC_CAMERA:
        enInputSourceType = INPUT_SOURCE_CAMERA;
        break;

    default:
        SCA_DBG(SCA_DBG_LV_0, "Trans Src: SRC Type Unknown %d !! \n", enSrc);
        enInputSourceType = INPUT_SOURCE_NONE;
        break;
    }

    return enInputSourceType;
}

static SCALER_WIN _Trans_To_Drv_Dest(SCA_DEST_TYPE enDest)
{
    SCALER_WIN eWindow;
    switch(enDest)
    {
    case SCA_DEST_MAIN:
        eWindow = MAIN_WINDOW;
        break;
    case SCA_DEST_SUB:
        eWindow = SUB_WINDOW;
        break;
    case SCA_DEST_MAIN_1:
        eWindow = SC1_MAIN_WINDOW;
        break;
    case SCA_DEST_MAIN_2:
        eWindow = SC2_MAIN_WINDOW;
        break;
    case SCA_DEST_SUB_2:
        eWindow = SC2_SUB_WINDOW;
        break;
    default:
    SCA_DBG(SCA_DBG_LV_0, "Trans_Dest: Dest Type Unknown !! \n");
    eWindow = MAX_WINDOW;
        break;
    }
    return eWindow;
}


static MS_BOOL _Trans_To_Drv_Adc_Calib(const MS_ADC_SETTING *pstMsAdcSetting, SCA_ADC_CALIB_INFO *pstAdcCalibSettingOut)
{
	if (!pstMsAdcSetting || !pstAdcCalibSettingOut)
		return FALSE;

	pstAdcCalibSettingOut->stAdcGainOffset.u16BlueGain   = pstMsAdcSetting->stAdcGainOffsetSetting.u16BlueGain;
	pstAdcCalibSettingOut->stAdcGainOffset.u16GreenGain  = pstMsAdcSetting->stAdcGainOffsetSetting.u16GreenGain;
	pstAdcCalibSettingOut->stAdcGainOffset.u16RedGain    = pstMsAdcSetting->stAdcGainOffsetSetting.u16RedGain;
	pstAdcCalibSettingOut->stAdcGainOffset.u16BlueOffset = pstMsAdcSetting->stAdcGainOffsetSetting.u16BlueOffset;
	pstAdcCalibSettingOut->stAdcGainOffset.u16GreenOffset= pstMsAdcSetting->stAdcGainOffsetSetting.u16GreenOffset;
	pstAdcCalibSettingOut->stAdcGainOffset.u16RedOffset  = pstMsAdcSetting->stAdcGainOffsetSetting.u16RedOffset;
	pstAdcCalibSettingOut->u8AdcCalOK= pstMsAdcSetting->u8AdcCalOK;

	return TRUE;
}

static MS_BOOL _Trans_To_Local_Adc_Calib(const SCA_ADC_CALIB_INFO *pstAdcCalibSetting, MS_ADC_SETTING *pstMsAdcSettingOut)
{
	if (!pstMsAdcSettingOut || !pstAdcCalibSetting)
		return FALSE;

	pstMsAdcSettingOut->stAdcGainOffsetSetting.u16BlueGain  = pstAdcCalibSetting->stAdcGainOffset.u16BlueGain;
	pstMsAdcSettingOut->stAdcGainOffsetSetting.u16GreenGain = pstAdcCalibSetting->stAdcGainOffset.u16GreenGain;
	pstMsAdcSettingOut->stAdcGainOffsetSetting.u16RedGain   = pstAdcCalibSetting->stAdcGainOffset.u16RedGain;
	pstMsAdcSettingOut->stAdcGainOffsetSetting.u16BlueOffset  = pstAdcCalibSetting->stAdcGainOffset.u16BlueOffset;
	pstMsAdcSettingOut->stAdcGainOffsetSetting.u16GreenOffset = pstAdcCalibSetting->stAdcGainOffset.u16GreenOffset;
	pstMsAdcSettingOut->stAdcGainOffsetSetting.u16RedOffset   = pstAdcCalibSetting->stAdcGainOffset.u16RedOffset;

	pstMsAdcSettingOut->u8AdcCalOK = pstAdcCalibSetting->u8AdcCalOK;

	return TRUE;

}

static E_ADC_SET_INDEX _Trans_To_Local_ADC_Set_Idx(SCA_SRC_TYPE eDrvInputSource, MS_BOOL bIsHdMode)
{
	E_ADC_SET_INDEX eAdcSetIdx = ADC_SET_VGA;

	switch (eDrvInputSource)
	{
		case SCA_SRC_VGA:
			eAdcSetIdx = ADC_SET_VGA;
			break;

		case SCA_SRC_YPBPR:
			eAdcSetIdx = (bIsHdMode? ADC_SET_YPBPR_HD:
				                     ADC_SET_YPBPR_SD );
			break;

		default:
			break;
	}

	return eAdcSetIdx;
}

#if defined(NETLINK_SAMPLE)

#define NLMSG_NEW(skb, pid, seq, type, len, flags) \
({	if (unlikely(skb_tailroom(skb) < (int)NLMSG_SPACE(len))) \
        goto nlmsg_failure; \
    __nlmsg_put(skb, pid, seq, type, len, flags); })

#define NLMSG_PUT(skb, pid, seq, type, len) \
    NLMSG_NEW(skb, pid, seq, type, len, 0)



static void SCA_NetLink_Broadcast(int gid,void *pBuffer)
{
	struct sk_buff	*skb;
	struct nlmsghdr	*nlh;
	int size=strlen(pBuffer)+1;
	int		len = NLMSG_SPACE(size);
	void		*data;
	int ret;

	skb = alloc_skb(len, GFP_KERNEL);
	if (!skb)
		return;

	nlh= NLMSG_PUT(skb, 0, 0, 0, size);
	nlh->nlmsg_flags = 0;
	data=NLMSG_DATA(nlh);
	memcpy(data, pBuffer, size);
	NETLINK_CB(skb).portid = 0;         /* from kernel */
	NETLINK_CB(skb).dst_group = gid;  /* unicast */

	//ret=netlink_unicast(netlink_sock, skb, pid, MSG_DONTWAIT);
	SCA_DBG(SCA_DBG_LV_0,"[sca netlink brocase]: %s\n", (char *)pBuffer);
	ret=netlink_broadcast(netlink_sock, skb, 0, gid, GFP_KERNEL);

	if (ret <0)
	{
	    SCA_DBG(SCA_DBG_LV_0, "netlink brocase, send failed\n");
		return;
	}
	return;

nlmsg_failure:			/* Used by NLMSG_PUT */
	if (skb)
		kfree_skb(skb);
}



static void SCA_NetLink_UniCast(int pid,int seq,void *pBuffer)
{
    struct sk_buff *skb;
    struct nlmsghdr *nlh;
    int size=strlen(pBuffer)+1;
    int len = NLMSG_SPACE(size);
    void *data;
    int ret;

    skb = alloc_skb(len, GFP_ATOMIC);
    if (!skb)
    return;


    nlh= NLMSG_PUT(skb, pid, seq, 0, size);
    nlh->nlmsg_flags = 0;
    data=NLMSG_DATA(nlh);
    memcpy(data, pBuffer, size);
    NETLINK_CB(skb).portid = 0; /* from kernel */
    NETLINK_CB(skb).dst_group = 0; /* unicast */

    SCA_DBG(SCA_DBG_LV_0,"[sca netlink unicast]: %s\n", (char *)pBuffer);
    ret=netlink_unicast(netlink_sock, skb, pid, MSG_DONTWAIT);
    if (ret <0)
    {
        SCA_DBG(SCA_DBG_LV_0, "[sca netlink unicast]: send failed\n");
        return;
    }
    return;

nlmsg_failure: /* Used by NLMSG_PUT */
    if (skb)
    kfree_skb(skb);
}

    /* Receive messages from netlink socket. */
static void sca_netlink_receive(struct sk_buff *skb)
{
    u_int uid, pid, seq;
    void *data;
    struct nlmsghdr *nlh;

    nlh = (struct nlmsghdr *)skb->data;
    pid = NETLINK_CREDS(skb)->pid;
    uid = NETLINK_CREDS(skb)->uid;
    seq = nlh->nlmsg_seq;
    data = NLMSG_DATA(nlh);
    SCA_DBG(SCA_DBG_LV_0, "[sca netlink recieve]: recv skb from user space uid:%d pid:%d seq:%d \n",uid,pid,seq);
    SCA_DBG(SCA_DBG_LV_0, "[sca netlink recieve]: data is :%s\n",(char *)data);
    SCA_NetLink_UniCast(pid,seq,data);
    SCA_NetLink_Broadcast(1,"KERNEL NETLINK BROADCAST\n");
    return ;
}
#endif


#if defined(SCA_ENABLE_HDMITX_HPD)

static MS_BOOL SCA_Set_UEvent_Broadcast(MS_U16 u16Size, MS_U8 *pBuf)
{
    char *envp[3] = {NULL, NULL, NULL};


    if(gpkobj == NULL)
    {
        SCA_DBG(SCA_DBG_LV_HDMITX_HP, "kobj is NULL !!!! \n");
        return 0;
    }

    envp[0] = pBuf;
    kobject_uevent_env(gpkobj, KOBJ_CHANGE, envp);

    return 1;
}


static int SCA_HDMITX_HPD_Thread(void *arg)
{
    static MS_BOOL bPreHPD = FALSE;
    MS_BOOL bCurHPD;
    char *buff;
    char *envp[3] = {NULL, NULL, NULL};

    SCA_DBG(SCA_DBG_LV_HDMITX_HP, "SCA_HDMITX_HPD_Thread Create !!!! \n");

    if(gpkobj == NULL)
    {
        SCA_DBG(SCA_DBG_LV_HDMITX_HP, "kobj is NULL !!!! \n");
        return 0;
    }


    while(1)
    {
        if (kthread_should_stop())
        {
            break;
        }
        else
        {
            bCurHPD = MApi_HDMITx_GetRxStatus();

            if(bCurHPD != bPreHPD)
            {
                buff  = kmalloc(32,GFP_ATOMIC);

                if(bCurHPD == TRUE)
                {
                    SCA_DBG(SCA_DBG_LV_HDMITX_HP,"HDMITX_ON\n");
                    snprintf(buff , 32 , "HDMITX_HPD=1");
                    envp[0] = buff;
                    kobject_uevent_env(gpkobj, KOBJ_CHANGE, envp);
                }
                else
                {
                    SCA_DBG(SCA_DBG_LV_HDMITX_HP,"HDMITX_off \n");
                    snprintf(buff , 32 , "HDMITX_HPD=0");
                    envp[0] = buff;
                    kobject_uevent_env(gpkobj, KOBJ_CHANGE, envp);
                }

                kfree(buff);

            }
            bPreHPD = bCurHPD;
        }
        msleep(20);

    }
    return 1;
}

MS_BOOL SCA_Init_HDMI_HPD_Thread(void)
{
    const char pName[] = {"MONITOR_THREAD_HDMITX_HPD"};

    if(pHDMITxHPDThread)
    {
        SCA_DBG(SCA_DBG_LV_0, "HPD Thread already init \n");
        return FALSE;
    }

    pHDMITxHPDThread = kthread_create(SCA_HDMITX_HPD_Thread, NULL,  pName);

    if (IS_ERR(pHDMITxHPDThread))
    {
        pHDMITxHPDThread = NULL;
        SCA_DBG(SCA_DBG_LV_HDMITX_HP, "Create HDMI_HPD Thread Fail \n");
        return FALSE;
    }
    else
    {
        wake_up_process(pHDMITxHPDThread);
    }
    return TRUE;

}
#endif

#if defined(SCA_ENABLE_DLC)

static int SCA_DLC_Monitor_Thread(void *arg)
{
    MS_DLC_MONITOR_CONFIG *pstCfg = (MS_DLC_MONITOR_CONFIG *)arg;
    static MS_BOOL g_bHistogramReady=0;

    SCA_DBG(SCA_DBG_LV_0, "DLC_Monitor_Thread Create !!!! \n");
    while(1)
    {
        if (kthread_should_stop())
        {
            break;
        }
        else
        {
            if(pstCfg->bEn)
            {
                if( g_bHistogramReady )
                {
                    MApi_XC_DLC_Handler(MAIN_WINDOW);
                    MApi_XC_DLC_CGC_Handler();

                    g_bHistogramReady = FALSE;
                }

                if( !g_bHistogramReady )
                {

                    if( MApi_XC_DLC_GetHistogramHandler(MAIN_WINDOW) == TRUE )
                    {
                        g_bHistogramReady = TRUE;
                    }
                }
            }
        }

        msleep(20);
    }

    return 0;
}

MS_BOOL SCA_Init_DLC_Thread(void)
{
    const char *pName = &DLC_Monitor_Thread_name[0];

    g_stDLCMonitorCfg.bEn = FALSE;
    g_stDLCMonitorCfg.bUserOnOff = FALSE;
    g_stDLCMonitorCfg.bUserControl = TRUE;
    pDLCMonitorThread = kthread_create(SCA_DLC_Monitor_Thread,
                                       (void *)&g_stDLCMonitorCfg,
                                        pName);

    if (IS_ERR(pDLCMonitorThread))
    {
        pDLCMonitorThread = NULL;
        SCA_DBG(SCA_DBG_LV_0, "Create DLC Monitor Thread Fail \n");
        return FALSE;
    }
    else
    {
        wake_up_process(pDLCMonitorThread);
    }
    return TRUE;
}
#endif

int mstar_sca_is_alive(void)
{
    return g_bScaAlive;
}

MS_BOOL SCA_Init_Disp_Path(void)
{
    MS_U8 i;

    SCA_WAIT_MUTEX(_SCA_Cnnt_Mutex);
    for(i=0; i<SCA_DEST_NUM; i++)
    {
        g_stDispPathCfg[i].bChanged = FALSE;
        g_stDispPathCfg[i].bDisConnect = FALSE;
        g_stDispPathCfg[i].enPriType = SCA_DISP_PRI_NONE;
        g_stDispPathCfg[i].enSrcType = SCA_SRC_NUM;
        g_stDispPathCfg[i].enDestType = SCA_DEST_NUM;
        g_stDispPathCfg[i].dwUID = NONE_UID;
    }
    SCA_RELEASE_MUTEX(_SCA_Cnnt_Mutex);
    return TRUE;
}

#if ENABLE_MONITOR_THREAD
static int SCA_PCModeMonitor_Thread(void *arg)
{
    SCA_MODE_MONITOR_PARSE_IN_CONFIG  stModeParseInCfg;
    SCA_MODE_MONITOR_PARSE_OUT_CONFIG stModeParseOutCfg;
    SCA_SET_TIMING_WINDOW_CONFIG      stWin_Timing_Cfg;
    SCA_MODE_MOINITOR_STATUS          ePreviousStatus = SCA_MODE_MONITOR_NOSYNC;
    SCA_DISPLAY_MUTE_CONFIG           stDisplayMute;
    SCA_MONITOR_CONFIG *pCfg = (SCA_MONITOR_CONFIG *)arg;
    MS_U32 u32CnntID;

    stModeParseInCfg.enSrcType = pCfg->enSrcType;
    stModeParseInCfg.enDestType = pCfg->enDestType;
    u32CnntID = pCfg->u32CnntID;
    SCA_DBG(SCA_DBG_LV_MONTHREAD, "PCMonitorThread: S:%d, D:%d \n", stModeParseInCfg.enSrcType, stModeParseInCfg.enDestType);
    stDisplayMute.bEn = TRUE;
    stDisplayMute.enDestType = stModeParseInCfg.enDestType;
    stDisplayMute.dwUID = u32CnntID;
    _Set_Display_Mute(&stDisplayMute, E_SCREEN_MUTE_MONITOR);

    SCA_WAIT_MUTEX(_SCA_MoniWinCfg_Mutex);
    g_stMonitorStatusCfg[stModeParseInCfg.enDestType].enDestType = stModeParseInCfg.enDestType;
    g_stMonitorStatusCfg[stModeParseInCfg.enDestType].bSetModeDone = FALSE;
    g_stMonitorStatusCfg[stModeParseInCfg.enDestType].enSrcType = stModeParseInCfg.enSrcType;
    SCA_RELEASE_MUTEX(_SCA_MoniWinCfg_Mutex);

    pCfg->bRunning = TRUE;
    complete(&_SCA_MoniThread_Comp);

    while(1)
    {
        if (kthread_should_stop())
        {
            break;
        }
        else
        {
            if(SCA_YPBPR_VGA_Mode_Monitor_Parse(&stModeParseInCfg, &stModeParseOutCfg))
            {
                if((stModeParseOutCfg.eModeMonitorStatus == SCA_MODE_MONITOR_NOSYNC ||
                    stModeParseOutCfg.eModeMonitorStatus == SCA_MODE_MONITOR_UNSTABLE)  &&
                   (ePreviousStatus != stModeParseOutCfg.eModeMonitorStatus)
                  )
                { // no-sync or unstable
                    SCA_DBG(SCA_DBG_LV_MONTHREAD, "PCMonitor Unstalbe: S:%d, D:%d \n", stModeParseInCfg.enSrcType, stModeParseInCfg.enDestType);

                    SCA_WAIT_MUTEX(_SCA_MoniWinCfg_Mutex);
                    g_stMonitorStatusCfg[stModeParseInCfg.enDestType].bSetModeDone = FALSE;
                    SCA_RELEASE_MUTEX(_SCA_MoniWinCfg_Mutex);

                    stDisplayMute.bEn = TRUE;
                    stDisplayMute.enDestType = stModeParseInCfg.enDestType;
                    stDisplayMute.dwUID = u32CnntID;
                    _Set_Display_Mute(&stDisplayMute, E_SCREEN_MUTE_MONITOR);
                }
                else if( stModeParseOutCfg.eModeMonitorStatus == SCA_MODE_MONITOR_STABLE_SUPPORT_MODE &&
                         ePreviousStatus != stModeParseOutCfg.eModeMonitorStatus)
                {

                    pCfg->stModeParseOutCfg.u8ModeIdx          = stModeParseOutCfg.u8ModeIdx;
                    pCfg->stModeParseOutCfg.u16HorizontalStart = stModeParseOutCfg.u16HorizontalStart;
                    pCfg->stModeParseOutCfg.u16VerticalStart   = stModeParseOutCfg.u16VerticalStart;
                    pCfg->stModeParseOutCfg.u16HorizontalTotal = stModeParseOutCfg.u16HorizontalTotal;

                    memset(&stWin_Timing_Cfg, 0, sizeof(SCA_SET_TIMING_WINDOW_CONFIG));
                    stWin_Timing_Cfg.enSrcType = stModeParseInCfg.enSrcType;
                    stWin_Timing_Cfg.enDestType = stModeParseInCfg.enDestType;

                    SCA_WAIT_MUTEX(_SCA_MoniWinCfg_Mutex);

                    if(g_stMonitorWinCfg[stModeParseInCfg.enDestType].enSrcType == stModeParseInCfg.enSrcType)
                    {
                        memcpy(&stWin_Timing_Cfg.stWinConfig, &g_stMonitorWinCfg[stModeParseInCfg.enDestType].stWinConfig, sizeof(SCA_WINDOW_CONFIG));


                        SCA_DBG(SCA_DBG_LV_MONTHREAD, "PCMonit  SRC:%d, Dest:%d bSrc:%d, %d %d %d %d, bCrop:%d %d %d %d %d, bDisp:%d %d %d %d %d \r\n",
                            (int)stModeParseInCfg.enSrcType, (int)stModeParseInCfg.enDestType,
                            g_stMonitorWinCfg[stModeParseInCfg.enDestType].stWinConfig.bSrcWin,
                            g_stMonitorWinCfg[stModeParseInCfg.enDestType].stWinConfig.stSrcWin.x,
                            g_stMonitorWinCfg[stModeParseInCfg.enDestType].stWinConfig.stSrcWin.y,
                            g_stMonitorWinCfg[stModeParseInCfg.enDestType].stWinConfig.stSrcWin.width,
                            g_stMonitorWinCfg[stModeParseInCfg.enDestType].stWinConfig.stSrcWin.height,
                            g_stMonitorWinCfg[stModeParseInCfg.enDestType].stWinConfig.bCropWin,
                            g_stMonitorWinCfg[stModeParseInCfg.enDestType].stWinConfig.stCropWin.x,
                            g_stMonitorWinCfg[stModeParseInCfg.enDestType].stWinConfig.stCropWin.y,
                            g_stMonitorWinCfg[stModeParseInCfg.enDestType].stWinConfig.stCropWin.width,
                            g_stMonitorWinCfg[stModeParseInCfg.enDestType].stWinConfig.stCropWin.height,
                            g_stMonitorWinCfg[stModeParseInCfg.enDestType].stWinConfig.bDispWin,
                            g_stMonitorWinCfg[stModeParseInCfg.enDestType].stWinConfig.stDispWin.x,
                            g_stMonitorWinCfg[stModeParseInCfg.enDestType].stWinConfig.stDispWin.y,
                            g_stMonitorWinCfg[stModeParseInCfg.enDestType].stWinConfig.stDispWin.width,
                            g_stMonitorWinCfg[stModeParseInCfg.enDestType].stWinConfig.stDispWin.height
                            );
                    }
                    else
                    {
                        stWin_Timing_Cfg.stWinConfig.bCropWin = FALSE;
                        stWin_Timing_Cfg.stWinConfig.bDispWin = FALSE;
                        stWin_Timing_Cfg.stWinConfig.bSrcWin  = FALSE;
                    }

                    SCA_RELEASE_MUTEX(_SCA_MoniWinCfg_Mutex);

                    if(kthread_should_stop())
                    {
                        break;
                    }

                    stWin_Timing_Cfg.stTimingConfig.bUpdate = FALSE;
                    stWin_Timing_Cfg.dwUID = u32CnntID;
                    SCA_Set_Timing_Window(&stWin_Timing_Cfg);

                    SCA_WAIT_MUTEX(_SCA_MoniWinCfg_Mutex);
                    g_stMonitorStatusCfg[stModeParseInCfg.enDestType].bSetModeDone = TRUE;
                    SCA_RELEASE_MUTEX(_SCA_MoniWinCfg_Mutex);

                    if(kthread_should_stop())
                    {
                        break;
                    }

                    stDisplayMute.bEn = FALSE;
                    stDisplayMute.enDestType = stModeParseInCfg.enDestType;
                    stDisplayMute.dwUID = u32CnntID;
                    _Set_Display_Mute(&stDisplayMute, E_SCREEN_MUTE_MONITOR);

                }
                else
                {
                    //RETAILMSG (TRUE,(TEXT ("SCA_PCModeMonitor_Thread un-support mode \r\n")));
                }
            }
            else
            {

                SCA_DBG(SCA_DBG_LV_MONTHREAD, "SCA_PCModeMonitor_Thread: Mode Parse Error!!! \r\n");
            }

            ePreviousStatus = stModeParseOutCfg.eModeMonitorStatus;
            pCfg->stModeParseOutCfg.eModeMonitorStatus = stModeParseOutCfg.eModeMonitorStatus;
        }

        msleep(MApi_XC_PCMonitor_GetPollingPeroid());
    }

    pCfg->bRunning = FALSE;
    complete(&_SCA_MoniThread_Stop_Comp);

    return 0;
}


static int SCA_AVDModeMonitor_Thrad(void *arg)
{
    SCA_SRC_TYPE enSrcType;
    SCA_DEST_TYPE enDestType;
    AVD_CHECK_VIDEOSTD_CONFIG    stVideoStdCfg;
    SCA_DISPLAY_MUTE_CONFIG      stDisplayMute;
    SCA_SET_TIMING_WINDOW_CONFIG stWin_Timing_Cfg;
    AVD_VIDEO_STANDARD  enPreVideoStd = AVD_VIDEOSTANDARD_NOTSTANDARD;
    AVD_VIDEO_DETECT_STATE enPreVideoDetState = AVD_VIDEO_DETECT_UNSTABLE;
    SCA_MONITOR_CONFIG *pCfg = (SCA_MONITOR_CONFIG *)arg;
    MS_U32 u32CnntID;


    enSrcType = pCfg->enSrcType;
    enDestType = pCfg->enDestType;
    u32CnntID = pCfg->u32CnntID;
    SCA_DBG(SCA_DBG_LV_MONTHREAD, "AVDMonitorThread: S:%d, D:%d \n", enSrcType, enDestType);

    stDisplayMute.bEn = TRUE;
    stDisplayMute.enDestType = enDestType;
    stDisplayMute.dwUID = u32CnntID;
    _Set_Display_Mute(&stDisplayMute, E_SCREEN_MUTE_MONITOR);


    SCA_WAIT_MUTEX(_SCA_MoniWinCfg_Mutex);
    g_stMonitorStatusCfg[enDestType].bSetModeDone = FALSE;
    g_stMonitorStatusCfg[enDestType].enSrcType = enSrcType;
    g_stMonitorStatusCfg[enDestType].enDestType = enDestType;
    SCA_RELEASE_MUTEX(_SCA_MoniWinCfg_Mutex);

    pCfg->bRunning = TRUE;
    complete(&_SCA_MoniThread_Comp);

    while(1)
    {
        if (kthread_should_stop())
        {
            break;
        }
        else
        {

            SCA_AVD_Check_VideoStd(&stVideoStdCfg);

            if((((stVideoStdCfg.enVideoStandard == AVD_VIDEOSTANDARD_NOTSTANDARD) && (enPreVideoStd != stVideoStdCfg.enVideoStandard)) ||
                ((stVideoStdCfg.enDetectState == AVD_VIDEO_DETECT_UNSTABLE) &&
               (enPreVideoDetState != stVideoStdCfg.enDetectState)))
              )
            {
                SCA_DBG(SCA_DBG_LV_MONTHREAD, "AVDMonitor Unstalbe:S:%d, D:%d \n", enSrcType, enDestType);

                SCA_WAIT_MUTEX(_SCA_MoniWinCfg_Mutex);
                g_stMonitorStatusCfg[enDestType].bSetModeDone = FALSE;
                SCA_RELEASE_MUTEX(_SCA_MoniWinCfg_Mutex);

                stDisplayMute.bEn = TRUE;
                stDisplayMute.enDestType = enDestType;
                stDisplayMute.dwUID = u32CnntID;
                _Set_Display_Mute(&stDisplayMute, E_SCREEN_MUTE_MONITOR);
            }
            else if((stVideoStdCfg.enVideoStandard != AVD_VIDEOSTANDARD_NOTSTANDARD && enPreVideoStd != stVideoStdCfg.enVideoStandard) ||
                    ((stVideoStdCfg.enVideoStandard != AVD_VIDEOSTANDARD_NOTSTANDARD) && (stVideoStdCfg.enDetectState == AVD_VIDEO_DETECT_STABLE) && (enPreVideoDetState != stVideoStdCfg.enDetectState)))
            {

                memset(&stWin_Timing_Cfg, 0, sizeof(SCA_SET_TIMING_WINDOW_CONFIG));
                stWin_Timing_Cfg.enSrcType = enSrcType;
                stWin_Timing_Cfg.enDestType = enDestType;

                SCA_WAIT_MUTEX(_SCA_MoniWinCfg_Mutex);
                if(g_stMonitorWinCfg[enDestType].enSrcType == enSrcType)
                {
                    memcpy(&stWin_Timing_Cfg.stWinConfig, &g_stMonitorWinCfg[enDestType].stWinConfig, sizeof(SCA_WINDOW_CONFIG));

                    SCA_DBG(SCA_DBG_LV_MONTHREAD, "AVDMoniThread  SRC:%d, DEST:%d, bSrc:%d, %d %d %d %d, bCrop:%d %d %d %d %d, bDisp:%d %d %d %d %d \r\n",
                        (int)enSrcType, (int)enDestType,
                        g_stMonitorWinCfg[enDestType].stWinConfig.bSrcWin,
                        g_stMonitorWinCfg[enDestType].stWinConfig.stSrcWin.x,
                        g_stMonitorWinCfg[enDestType].stWinConfig.stSrcWin.y,
                        g_stMonitorWinCfg[enDestType].stWinConfig.stSrcWin.width,
                        g_stMonitorWinCfg[enDestType].stWinConfig.stSrcWin.height,
                        g_stMonitorWinCfg[enDestType].stWinConfig.bCropWin,
                        g_stMonitorWinCfg[enDestType].stWinConfig.stCropWin.x,
                        g_stMonitorWinCfg[enDestType].stWinConfig.stCropWin.y,
                        g_stMonitorWinCfg[enDestType].stWinConfig.stCropWin.width,
                        g_stMonitorWinCfg[enDestType].stWinConfig.stCropWin.height,
                        g_stMonitorWinCfg[enDestType].stWinConfig.bDispWin,
                        g_stMonitorWinCfg[enDestType].stWinConfig.stDispWin.x,
                        g_stMonitorWinCfg[enDestType].stWinConfig.stDispWin.y,
                        g_stMonitorWinCfg[enDestType].stWinConfig.stDispWin.width,
                        g_stMonitorWinCfg[enDestType].stWinConfig.stDispWin.height
                        );
                }
                else
                {
                    stWin_Timing_Cfg.stWinConfig.bCropWin = FALSE;
                    stWin_Timing_Cfg.stWinConfig.bDispWin = FALSE;
                    stWin_Timing_Cfg.stWinConfig.bSrcWin  = FALSE;
                }

                SCA_RELEASE_MUTEX(_SCA_MoniWinCfg_Mutex);

                if(kthread_should_stop())
                {
                    break;
                }

                stWin_Timing_Cfg.stTimingConfig.bUpdate = FALSE;
                stWin_Timing_Cfg.dwUID = u32CnntID;
                SCA_Set_Timing_Window(&stWin_Timing_Cfg);


                SCA_WAIT_MUTEX(_SCA_MoniWinCfg_Mutex);
                g_stMonitorStatusCfg[enDestType].bSetModeDone = TRUE;
                SCA_RELEASE_MUTEX(_SCA_MoniWinCfg_Mutex);

                if(kthread_should_stop())
                {
                    break;
                }

                stDisplayMute.bEn = FALSE;
                stDisplayMute.enDestType = enDestType;
                stDisplayMute.dwUID = u32CnntID;
                _Set_Display_Mute(&stDisplayMute, E_SCREEN_MUTE_MONITOR);
            }
            else
            {
                //RETAILMSG (TRUE,(TEXT ("VideoStd %d \r\n"), stVideoStdCfg.enVideoStandard ));
            }

            enPreVideoStd = stVideoStdCfg.enVideoStandard;
            enPreVideoDetState = stVideoStdCfg.enDetectState;
        }
        msleep(20);
    }

    pCfg->bRunning = FALSE;
    complete(&_SCA_MoniThread_Stop_Comp);

    return 0;
}

MS_BOOL SCA_Init_Thread(void)
{
    MS_U16 i;

    SCA_WAIT_MUTEX(_SCA_MoniWinCfg_Mutex);
    memset(&g_stMonitorWinCfg, 0, sizeof(SCA_MONITOR_WIN_CONFIG)*SCA_DEST_NUM);
    memset(&g_stMonitorStatusCfg, 0, sizeof(SCA_MONITOR_STATUS_CONFIG)*SCA_DEST_NUM);

    for(i=0; i<SCA_DEST_NUM; i++)
    {
        g_enPreSrcType[i] = SCA_SRC_NUM;
        g_stMonitorThread[i].pThread = NULL;
        g_stMonitorThread[i].enSrcType = SCA_SRC_NUM;
        g_stMonitorThread[i].enDestType = SCA_DEST_NUM;
        g_stMonitorThread[i].stModeParseOutCfg.eModeMonitorStatus = SCA_MODE_MONITOR_NOSYNC;
        g_stMonitorThread[i].stVideoStdCfg.enVideoStandard = AVD_VIDEOSTANDARD_MAX;
        g_stMonitorThread[i].bRunning = FALSE;
        g_stMonitorThread[i].u32CnntID = NONE_UID;
        g_stMonitorWinCfg[i].enSrcType = SCA_SRC_NUM;

        g_stMonitorStatusCfg[i].enSrcType = SCA_SRC_NUM;
        g_stMonitorStatusCfg[i].enDestType = SCA_DEST_NUM;
    }
    SCA_RELEASE_MUTEX(_SCA_MoniWinCfg_Mutex);
    return TRUE;
}



MS_BOOL SCA_Create_Monitor_Thread(SCA_SRC_TYPE enSrcType, SCA_DEST_TYPE enDestType, MS_U32 u32CnntID)
{
    INPUT_SOURCE_TYPE_t enInputSourceType;
    MS_BOOL bCreate = FALSE;
    const char *pName;
    MS_U32 u32CurTime;

    enInputSourceType = _Trans_To_Drv_Src(enSrcType);

    if(!IsSrcTypeAnalog(enInputSourceType) && !IsSrcTypeDigitalVD(enInputSourceType) && !IsSrcTypeBT656(enInputSourceType))
    {
        // only take care VGA, YPBPR, VD BT656
        return TRUE;
    }

    SCA_WAIT_MUTEX(_SCA_MoniWinCfg_Mutex);
    g_stMonitorThread[enDestType].enDestType = enDestType;
    g_stMonitorThread[enDestType].enSrcType = enSrcType;
    g_stMonitorThread[enDestType].u32CnntID = u32CnntID;
    SCA_RELEASE_MUTEX(_SCA_MoniWinCfg_Mutex);
    pName = &ThreadName[enDestType][0];

    SCA_DBG(SCA_DBG_LV_MONTHREAD, "++ SCA_Create_Monitor_Thread S:%d, D:%d  ID:%x ++ \n", enSrcType, enDestType, (int)u32CnntID);

    u32CurTime = MsOS_GetSystemTime();
    while(1)
    {
        if(!g_stMonitorThread[enDestType].bRunning ||
           ((MsOS_GetSystemTime() - u32CurTime) > 500))
        {
            break;
        }
        msleep(20);
    }

    if(IsSrcTypeAnalog(enInputSourceType)|| IsSrcTypeBT656(enInputSourceType))
    {
        g_stMonitorThread[enDestType].pThread = kthread_create(SCA_PCModeMonitor_Thread,
                                                               (void *)&g_stMonitorThread[enDestType],
                                                               pName);
        bCreate = TRUE;
    }
    else if(IsSrcTypeDigitalVD(enInputSourceType))
    {
        g_stMonitorThread[enDestType].pThread = kthread_create(SCA_AVDModeMonitor_Thrad,
                                                               (void *)&g_stMonitorThread[enDestType],
                                                               pName);
        bCreate = TRUE;

    }

    if(bCreate)
    {
        if (IS_ERR(g_stMonitorThread[enDestType].pThread))
        {
            g_stMonitorThread[enDestType].pThread = NULL;
            SCA_DBG(SCA_DBG_LV_0, "Create Task Fail \n");
            return FALSE;
        }
        else
        {
            wake_up_process(g_stMonitorThread[enDestType].pThread);

            wait_for_completion(&_SCA_MoniThread_Comp);
        }
    }


    return TRUE;
}

MS_BOOL SCA_Delete_Monitor_Thread(SCA_SRC_TYPE enSrcType, SCA_DEST_TYPE enDestType)
{
    INPUT_SOURCE_TYPE_t enInputSourceType;
    enInputSourceType = _Trans_To_Drv_Src(enSrcType);

    if(IsSrcTypeAnalog(enInputSourceType) || IsSrcTypeDigitalVD(enInputSourceType)|| IsSrcTypeBT656(enInputSourceType))
    {
        if(g_stMonitorThread[enDestType].pThread)
        {
            kthread_stop(g_stMonitorThread[enDestType].pThread);
            g_stMonitorThread[enDestType].pThread = NULL;
            g_stMonitorThread[enDestType].u32CnntID = NONE_UID;
            wait_for_completion(&_SCA_MoniThread_Stop_Comp);
            SCA_DBG(SCA_DBG_LV_MONTHREAD, "++ SCA_Delete_Monitor_Thread ++ S:%d D:%d \n",enSrcType, enDestType);
        }
    }
    return TRUE;
}

MS_BOOL Delete_Monitor_Thread_by_Dest(SCA_DEST_TYPE enDestType)
{
	if(g_stMonitorThread[enDestType].pThread)
	{
		kthread_stop(g_stMonitorThread[enDestType].pThread);
		g_stMonitorThread[enDestType].pThread = NULL;
		g_stMonitorThread[enDestType].u32CnntID = NONE_UID;
		wait_for_completion(&_SCA_MoniThread_Stop_Comp);
		printk("++ Delete_Monitor_Thread_by_Dest ++ D:%d \n", enDestType);
	}

	return TRUE;
}

MS_BOOL SCA_Get_Monitor_Status_Config(PSCA_MONITOR_STATUS_CONFIG stCfg)
{

    MS_BOOL bRet = TRUE;

    SCA_WAIT_MUTEX(_SCA_MoniWinCfg_Mutex);
    if(g_stMonitorStatusCfg[stCfg->enDestType].enSrcType == stCfg->enSrcType)
    {

        stCfg->bSetModeDone = g_stMonitorStatusCfg[stCfg->enDestType].bSetModeDone;
        bRet = TRUE;
    }
    else
    {
        stCfg->bSetModeDone = FALSE;
        bRet = FALSE;
    }
    SCA_RELEASE_MUTEX(_SCA_MoniWinCfg_Mutex);
    return bRet;

}

#endif

MS_BOOL SCA_Connect(PSCA_CONNECT_CONFIG pConfig)
{
    MS_BOOL bRet = TRUE;

    INPUT_SOURCE_TYPE_t enInputSourceType;
    SCALER_WIN eWindow;
    static MS_U8 u8ConnectIncr = 0;
    MS_U32 u32ConnectID;

    SCA_WAIT_MUTEX(_SCA_Cnnt_Mutex);
    if(pConfig->enPriType < g_stDispPathCfg[pConfig->enDestType].enPriType)
    {
        SCA_DBG(SCA_DBG_LV_0, "Priority is too low: Cur:%d, Pre:%d \n", pConfig->enPriType, g_stDispPathCfg[pConfig->enDestType].enPriType);

        SCA_RELEASE_MUTEX(_SCA_Cnnt_Mutex);
        return FALSE;
    }

    enInputSourceType = _Trans_To_Drv_Src(pConfig->enSrcType);

#if ENABLE_MONITOR_THREAD
    if(pConfig->bAutoDetect)
    {
        SCA_Delete_Monitor_Thread(g_enPreSrcType[pConfig->enDestType], pConfig->enDestType);
    }
#endif

    if(enInputSourceType == INPUT_SOURCE_NONE)
    {
        bRet = FALSE;
    }


    u32ConnectID = MAK_UID(pConfig->enSrcType,
                             pConfig->enDestType,
                             pConfig->enPriType,
                             (u8ConnectIncr & 0xF),
                             MsOS_GetSystemTime() );


    if(pConfig->enDestType == SCA_DEST_CVBSO)
    {
        if(bRet)
		{
            MApi_VE_SourceSwitch(enInputSourceType, OUTPUT_CVBS1);
        }
    }
    else
    {

        eWindow = _Trans_To_Drv_Dest(pConfig->enDestType);

        if(eWindow == MAX_WINDOW)
            bRet = FALSE;

        if(bRet)
        {
        #if defined(SCA_ENABLE_DLC)
            if(eWindow == MAIN_WINDOW)
            {
                g_stDLCMonitorCfg.bEn = FALSE;
            }
        #endif
            MApi_InputSource_SwitchSource(enInputSourceType, eWindow);
            MApi_PCMode_Init(NULL);

            if(pConfig->enSrcType == SCA_SRC_DTV)
            {
                MApi_XC_set_FD_Mask(TRUE, eWindow);
            }
        }
    }

    SCA_DBG(SCA_DBG_LV_0, "Disp Change Priority, Cur:%d, Pre:%d \n", pConfig->enPriType, g_stDispPathCfg[pConfig->enDestType].enPriType );

    g_stDispPathCfg[pConfig->enDestType].bChanged = TRUE;
    g_stDispPathCfg[pConfig->enDestType].bDisConnect = FALSE;
    g_stDispPathCfg[pConfig->enDestType].enSrcType = pConfig->enSrcType;
    g_stDispPathCfg[pConfig->enDestType].enDestType = pConfig->enDestType;

    u8ConnectIncr = (0xF == u8ConnectIncr) ? 0 : u8ConnectIncr + 1;
    pConfig->dwUID = u32ConnectID;
    g_stDispPathCfg[pConfig->enDestType].dwUID = pConfig->dwUID;
    g_stDispPathCfg[pConfig->enDestType].enPriType = pConfig->enPriType;

    if(pConfig->enDestType == SCA_DEST_SUB)
    {
        MApi_Set_Window_OnOFF(TRUE, TRUE);
    }


#if ENABLE_MONITOR_THREAD
    if(pConfig->bAutoDetect)
    {
        SCA_Create_Monitor_Thread(pConfig->enSrcType, pConfig->enDestType, u32ConnectID);
    }
#endif


    g_enPreSrcType[pConfig->enDestType] = pConfig->enSrcType;
    SCA_RELEASE_MUTEX(_SCA_Cnnt_Mutex);

    return bRet;
}

MS_BOOL SCA_DisConnect(PSCA_CONNECT_CONFIG pConfig)
{
    MS_BOOL bRet = TRUE;
    SCALER_WIN eWindow;
    SCA_DISPLAY_MUTE_CONFIG stMuteCfg;

    SCA_WAIT_MUTEX(_SCA_Cnnt_Mutex);

    if(pConfig->enPriType < g_stDispPathCfg[pConfig->enDestType].enPriType)
    {
        SCA_DBG(SCA_DBG_LV_0, "SCA_DisConnect ,Priority is too low: Cur:%d, Pre:%d \n", pConfig->enPriType, g_stDispPathCfg[pConfig->enDestType].enPriType);
        SCA_RELEASE_MUTEX(_SCA_Cnnt_Mutex);
		return FALSE;
    }

#if ENABLE_MONITOR_THREAD
    if(pConfig->bAutoDetect)
    {
        SCA_Delete_Monitor_Thread(pConfig->enSrcType, pConfig->enDestType);

        stMuteCfg.bEn = FALSE;
        stMuteCfg.enDestType = pConfig->enDestType;
        stMuteCfg.dwUID = NONE_UID;
        _Set_Display_Mute(&stMuteCfg, E_SCREEN_MUTE_MONITOR);

    }
#endif

    stMuteCfg.bEn = TRUE;
    stMuteCfg.enDestType = pConfig->enDestType;
    stMuteCfg.dwUID = NONE_UID;
    _Set_Display_Mute(&stMuteCfg, E_SCREEN_MUTE_INPUT);

    eWindow = _Trans_To_Drv_Dest(pConfig->enDestType);

    if(g_stDispPathCfg[pConfig->enDestType].enSrcType == SCA_SRC_DTV)
    {
        MApi_XC_set_FD_Mask(FALSE, eWindow);
    }

    if(pConfig->enDestType == SCA_DEST_SUB)
    {
        MApi_Set_Window_OnOFF(TRUE, FALSE);
    }


    g_stDispPathCfg[pConfig->enDestType].bDisConnect = TRUE;
    g_stDispPathCfg[pConfig->enDestType].enPriType = SCA_DISP_PRI_NONE;
    g_stDispPathCfg[pConfig->enDestType].enSrcType = SCA_SRC_NUM;
    g_stDispPathCfg[pConfig->enDestType].enDestType = SCA_DEST_NUM;
    g_stDispPathCfg[pConfig->enDestType].bChanged = FALSE;
    g_stDispPathCfg[pConfig->enDestType].dwUID =  NONE_UID;
	g_enPreSrcType[pConfig->enDestType] = SCA_SRC_NUM;
	SCA_RELEASE_MUTEX(_SCA_Cnnt_Mutex);


#if ENABLE_MONITOR_THREAD
    if(pConfig->bAutoDetect)
    {
        SCA_WAIT_MUTEX(_SCA_MoniWinCfg_Mutex);
        g_stMonitorWinCfg[pConfig->enDestType].enSrcType = SCA_SRC_NUM;
        memset(&g_stMonitorWinCfg[pConfig->enDestType].stWinConfig, 0, sizeof(SCA_WINDOW_CONFIG));

        g_stMonitorStatusCfg[pConfig->enDestType].enSrcType = SCA_SRC_NUM;
        g_stMonitorStatusCfg[pConfig->enDestType].bSetModeDone = FALSE;
        g_stMonitorStatusCfg[pConfig->enDestType].enDestType = SCA_DEST_NUM;
        SCA_RELEASE_MUTEX(_SCA_MoniWinCfg_Mutex);
    }
#endif


#if defined(SCA_ENABLE_DLC)
    if(eWindow == MAIN_WINDOW)
    {
        g_stDLCMonitorCfg.bEn = FALSE;
        MApi_XC_DLC_SetOnOff(DISABLE, MAIN_WINDOW);
    }
#endif

    return bRet;
}


MS_BOOL SCA_Set_Timing_Window(PSCA_SET_TIMING_WINDOW_CONFIG pConfig)
{
    MS_BOOL bRet = TRUE;
    MS_TIMING_CONFIG *pTimingCfg;
    MS_TIMING_CONFIG stTimingCfg;
    INPUT_SOURCE_TYPE_t enInputSourceType;
    SCALER_WIN eWindow;


#ifdef __DUAL_SCREEN__
    SCA_SRC_TYPE enSC1_Src;
    SCA_WINDOW_CONFIG stWincfg;
    MS_BOOL bDUAL_SCREEN_SetMode = FALSE;

    SCA_WAIT_MUTEX(_SCA_Cnnt_Mutex);
    enSC1_Src = g_stDispPathCfg[SCA_DEST_MAIN_1].enSrcType;
    SCA_RELEASE_MUTEX(_SCA_Cnnt_Mutex);

    SCA_WAIT_MUTEX(_SCA_MoniWinCfg_Mutex);

    if(pConfig->enDestType == SCA_DEST_MAIN_1 && pConfig->enSrcType == SCA_SRC_SC0_VOP)
    {
        memcpy(&g_stMonitorWinCfg[SCA_DEST_MAIN_1].stWinConfig, &pConfig->stWinConfig, sizeof(SCA_WINDOW_CONFIG));
    }

    if(pConfig->enDestType == SCA_DEST_MAIN &&  enSC1_Src == SCA_SRC_SC0_VOP)
    {
        memcpy(&stWincfg, &g_stMonitorWinCfg[SCA_DEST_MAIN_1].stWinConfig, sizeof(SCA_WINDOW_CONFIG));
        bDUAL_SCREEN_SetMode = TRUE;
    }
    SCA_RELEASE_MUTEX(_SCA_MoniWinCfg_Mutex);
#endif

	//MS_PQ_Vd_Info stPQVDInfo;
    SCA_WAIT_MUTEX(_SCA_Cnnt_Mutex);
    if((g_stDispPathCfg[pConfig->enDestType].dwUID != pConfig->dwUID)
        &&(pConfig->dwUID != DBG_UID)
      )
    {
        SCA_DBG(SCA_DBG_LV_0,"[SCA] SCA_Set_Timing_Window: ID not match: %x != %x \n", (int)pConfig->dwUID, (int)g_stDispPathCfg[pConfig->enDestType].dwUID);
        SCA_RELEASE_MUTEX(_SCA_Cnnt_Mutex);
        return FALSE;
    }


    enInputSourceType = _Trans_To_Drv_Src(pConfig->enSrcType);
    if(enInputSourceType == INPUT_SOURCE_NONE)
    {
        bRet = FALSE;
    }

    eWindow = _Trans_To_Drv_Dest(pConfig->enDestType);
    if(eWindow == MAX_WINDOW)
    {
       bRet = FALSE;
    }

#if defined(SCA_ENABLE_DLC)
    MApi_XC_DLC_CGC_Reset();
#endif

    if(pConfig->stTimingConfig.bUpdate)
    {
        pTimingCfg = &stTimingCfg;
        pTimingCfg->bInterlace     = pConfig->stTimingConfig.bInterlace;
        pTimingCfg->u16InputVFreq  = pConfig->stTimingConfig.u16InputVFreq;
        pTimingCfg->u16InputVTotal = pConfig->stTimingConfig.u16InputVTotal;
    }
    else
    {
        pTimingCfg = NULL;
    }

    if(bRet)
    {

#ifdef __DUAL_SCREEN__

        if(bDUAL_SCREEN_SetMode)
        {
            // display black screen as a compromise of blurred screen
            MApi_SetScreenMute(E_SCREEN_MUTE_BLOCK, ENABLE, 0, SC1_MAIN_WINDOW);
        }
#endif
        _Prog_Window(enInputSourceType, eWindow, &pConfig->stWinConfig);
        MApi_SetColor(enInputSourceType, eWindow);
        MApi_Scaler_SetTiming(enInputSourceType, eWindow, pTimingCfg);
/*
        if (pConfig->enDestType == SCA_DEST_MAIN)
            printk("SCA_Set_Timing_Window prio: %d, ID: 0x%08x pID: 0x%08x\n", g_stDispPathCfg[pConfig->enDestType].enPriType, g_stDispPathCfg[pConfig->enDestType].dwUID, pConfig->dwUID);
*/
#ifdef __DUAL_SCREEN__

        if(bDUAL_SCREEN_SetMode)
        {
            _Prog_Window(INPUT_SOURCE_SC0_VOP, SC1_MAIN_WINDOW, &stWincfg);

            MApi_SetScreenMute(E_SCREEN_MUTE_BLOCK, DISABLE, 0, SC1_MAIN_WINDOW);
        }
#endif

        SCA_RELEASE_MUTEX(_SCA_Cnnt_Mutex);
    }
    else
    {
        SCA_RELEASE_MUTEX(_SCA_Cnnt_Mutex);
    }


	if ( IsSrcTypeAV(enInputSourceType)  || IsSrcTypeSV(enInputSourceType) )
	{
	    MS_U32 u32Reg = eWindow == MAIN_WINDOW     ? 0x13010A :
	                    eWindow == SUB_WINDOW      ? 0x13030A :
	                    eWindow == SC1_MAIN_WINDOW ? 0x13810A :
	                    eWindow == SC2_MAIN_WINDOW ? 0x13410A :
	                                                 0x13430A;


		// !!! Cedric patch: force to NTSC mode and disable comb memory protection 2012-06-26 !!! ///
////		MApi_XC_Write2ByteMask(0x10358E, 0x0000, 0x1000);
////		MApi_XC_Write2ByteMask(0x103580, 0x767B, 0xFFFF);
////		MApi_XC_Write2ByteMask(0x103582, 0xA4,   0xFF);
////		MApi_XC_Write2ByteMask(0x10352A, 0xA0,   0xFF);
		MApi_XC_Write2ByteMask(0x1035A2, 0x20,   0xFF);
////		MApi_XC_Write2ByteMask(0x10364C, 0x00,   0x80);
		MApi_XC_Write2ByteMask(0x103648, 0x20,   0x20);

		SCA_DBG(SCA_DBG_LV_0, "!!! Insert VD NTSC mode patch !!!\n");

		//!!! Patch: set capture window x start point !!!//
		if ( IsSrcTypeSV(enInputSourceType) )
		{
			MApi_XC_Write2ByteMask(u32Reg, 0x2C,   0x3FF);
		}
		//else
		//{
		//	MApi_XC_Write2ByteMask(u32Reg, 0x6B,	 0x3FF);
		//}

		SCA_DBG(SCA_DBG_LV_0, "!!! [Patch] Relocation capture window start point !!!\n");


		//!!! patch: s-video bypass comb filter !!!//
//		if ( IsSrcTypeSV(enInputSourceType) )
//		{
//			MApi_XC_Write2ByteMask(0x10357E, 0x0000, 0x400);
//
//			RETAILMSG(1, (L"!!! [Patch] bypass SV comb filter !!!\n"));
//		}

	}

    return bRet;
}

static void _Prog_Window(INPUT_SOURCE_TYPE_t enInputSourceType, SCALER_WIN eWindow, PSCA_WINDOW_CONFIG pWinCfg )
{
    MS_WINDOW_TYPE *pCropWin;
    MS_WINDOW_TYPE *pSrcWin;
    MS_WINDOW_TYPE *pDispWin;
    MS_WINDOW_TYPE stCropWin;
    MS_WINDOW_TYPE stSrcWin;
    MS_WINDOW_TYPE stDispWin;
    MS_BOOL bRet = TRUE;

    if(pWinCfg->bSrcWin)
    {
        pSrcWin = &stCropWin;
        pSrcWin->x      = pWinCfg->stSrcWin.x;
        pSrcWin->y      = pWinCfg->stSrcWin.y;
        pSrcWin->width  = pWinCfg->stSrcWin.width;
        pSrcWin->height = pWinCfg->stSrcWin.height;
    }
    else
    {
        pSrcWin = NULL;
    }

    if(pWinCfg->bCropWin)
    {
        pCropWin = &stSrcWin;
        pCropWin->x      = pWinCfg->stCropWin.x;
        pCropWin->y      = pWinCfg->stCropWin.y;
        pCropWin->width  = pWinCfg->stCropWin.width;
        pCropWin->height = pWinCfg->stCropWin.height;
    }
    else
    {
        pCropWin = NULL;
    }

    if(pWinCfg->bDispWin)
    {
        pDispWin= &stDispWin;
        pDispWin->x      = pWinCfg->stDispWin.x;
        pDispWin->y      = pWinCfg->stDispWin.y;
        pDispWin->width  = pWinCfg->stDispWin.width;
        pDispWin->height = pWinCfg->stDispWin.height;
    }
    else
    {
        pDispWin = NULL;
    }

    bRet = MApi_Scaler_SetWindow(pSrcWin, pCropWin, pDispWin, enInputSourceType, eWindow);

    if( bRet == FALSE  && pDispWin)
    {
        XC_PANEL_INFO stPanelInfo;

        MApi_XC_Get_PanelInfo(&stPanelInfo);
        if(eWindow == MAIN_WINDOW || eWindow == SUB_WINDOW)
        {
            pDispWin->x += stPanelInfo.u16HStart;
            pDispWin->y += stPanelInfo.u16VStart;
        }
        else if(eWindow == SC1_MAIN_WINDOW || eWindow == SC2_MAIN_WINDOW)
        {
            MS_U16 u16Hstart = 0;
            MS_U16 u16Vstart = 0;
            MApi_XC_Get_TGen_HV_Start(eWindow, &u16Hstart, &u16Vstart);
            pDispWin->x += u16Hstart; // from TGen table
            pDispWin->y += u16Vstart;
        }
        else
        {
            SCA_DBG(SCA_DBG_LV_0, "ToDo!!! SC2 display window x/y start !!!!! \n");
        }


        MApi_XC_SetDispWinToReg(pDispWin, eWindow);
    }
}


MS_BOOL SCA_Set_MVOP(PSCA_MVOP_CONFIG pConfig)
{
    MVOP_InputCfg dc_param;
    MVOP_VidStat videoSetting;
    MVOP_InputSel enInputSel;
    MVOP_TileFormat enTileFmt;

    MDrv_MVOP_Init();
    MDrv_MVOP_EnableBlackBG();
    MDrv_MVOP_Enable(FALSE);

    dc_param.u16HSize = pConfig->stInConfig.u16HSize;
    dc_param.u16VSize = pConfig->stInConfig.u16VSize;
    dc_param.u32YOffset = pConfig->stInConfig.u32YOffset;
    dc_param.u32UVOffset = pConfig->stInConfig.u32UVOffset;
    dc_param.bSD = pConfig->stInConfig.bSD;
    dc_param.bYUV422 = pConfig->stInConfig.bYUV422;
    dc_param.bProgressive = pConfig->stInConfig.bProgressive;
    dc_param.bUV7bit = pConfig->stInConfig.bUV7bit;
    dc_param.bDramRdContd = pConfig->stInConfig.bDramRdContd;
    dc_param.bField = pConfig->stInConfig.bField;
    dc_param.b422pack = pConfig->stInConfig.b422pack;
    dc_param.u16StripSize = pConfig->stInConfig.u16StripSize;

    enInputSel = pConfig->stInConfig.enInputSel == SCA_MVOP_INPUT_DRAM     ? MVOP_INPUT_DRAM :
                 pConfig->stInConfig.enInputSel == SCA_MVOP_INPUT_H264     ? MVOP_INPUT_H264 :
                 pConfig->stInConfig.enInputSel == SCA_MVOP_INPUT_MVD      ? MVOP_INPUT_MVD :
                 pConfig->stInConfig.enInputSel == SCA_MVOP_INPUT_RVD      ? MVOP_INPUT_RVD :
                 pConfig->stInConfig.enInputSel == SCA_MVOP_INPUT_CLIP     ? MVOP_INPUT_CLIP :
                 pConfig->stInConfig.enInputSel == SCA_MVOP_INPUT_JPD      ? MVOP_INPUT_JPD :
                 pConfig->stInConfig.enInputSel == SCA_MVOP_INPUT_HVD_3DLR ? MVOP_INPUT_HVD_3DLR :
                 pConfig->stInConfig.enInputSel == SCA_MVOP_INPUT_MVD_3DLR ? MVOP_INPUT_MVD_3DLR :
                                                                             MVOP_INPUT_UNKNOWN;
    if(MDrv_MVOP_SetInputCfg(enInputSel, &dc_param) != E_MVOP_OK)
    {
        return FALSE;
    }


    memset(&videoSetting, 0, sizeof(MVOP_VidStat));
    videoSetting.u16HorSize = pConfig->stOutConfig.u16HorSize;
    videoSetting.u16VerSize = pConfig->stOutConfig.u16VerSize;
    videoSetting.u16FrameRate = pConfig->stOutConfig.u16FrameRate;
    videoSetting.u8AspectRate = pConfig->stOutConfig.u8AspectRate;
    videoSetting.u8Interlace = pConfig->stOutConfig.u8Interlace;

    if(MDrv_MVOP_SetOutputCfg(&videoSetting, FALSE) != E_MVOP_OK)
    {
        return FALSE;
    }

    enTileFmt = pConfig->enTileType == SCA_MVOP_TILE_16x32 ? E_MVOP_TILE_16x32 :
                pConfig->enTileType == SCA_MVOP_TILE_8x32  ? E_MVOP_TILE_8x32  :
                                                             E_MVOP_TILE_NONE  ;
    MDrv_MVOP_SetTileFormat(enTileFmt);

    MDrv_MVOP_Enable(TRUE);


    return TRUE;
}

static MS_BOOL _Set_Display_Mute(PSCA_DISPLAY_MUTE_CONFIG pConfig, E_SCREEN_MUTE_STATUS enMuteStatus)
{
    SCALER_WIN eWindow;
    MS_BOOL bret = TRUE;

    eWindow = _Trans_To_Drv_Dest(pConfig->enDestType);

    if(eWindow == MAX_WINDOW)
    {
        bret = FALSE;
    }

    SCA_WAIT_MUTEX(_SCA_Mute_Mutex);
    if(pConfig->bEn)
    {
        MApi_SetScreenMute(enMuteStatus, ENABLE, 0, eWindow);
        MApi_SetBlueScreen(ENABLE, MApi_Scaler_Get_DisplayMute_ColorType(eWindow), DEFAULT_SCREEN_UNMUTE_TIME, eWindow);
    }
    else
    {
        MApi_SetBlueScreen(DISABLE, MApi_Scaler_Get_DisplayMute_ColorType(eWindow), 0, eWindow);
        MApi_SetScreenMute(E_SCREEN_MUTE_TEMPORARY, DISABLE, 0, eWindow);
        MApi_SetScreenMute(enMuteStatus, DISABLE, 0, eWindow);
    }
    SCA_RELEASE_MUTEX(_SCA_Mute_Mutex);

	return bret;
}

MS_BOOL SCA_Set_Display_Mute(PSCA_DISPLAY_MUTE_CONFIG pConfig, E_SCREEN_MUTE_STATUS enMuteStatus)
{
    MS_BOOL bret = TRUE;;

    SCA_WAIT_MUTEX(_SCA_Cnnt_Mutex);
    if((g_stDispPathCfg[pConfig->enDestType].dwUID != pConfig->dwUID)
        &&(pConfig->dwUID != DBG_UID)
      )
    {
        SCA_DBG(SCA_DBG_LV_0,"[SCA] Display Mute: ID not match or invalid: %x , %x \n", (int)pConfig->dwUID, (int)g_stDispPathCfg[pConfig->enDestType].dwUID);
        SCA_RELEASE_MUTEX(_SCA_Cnnt_Mutex);
        return FALSE;
    }
    SCA_RELEASE_MUTEX(_SCA_Cnnt_Mutex);

    bret = _Set_Display_Mute(pConfig, enMuteStatus);

    return bret;
}

MS_BOOL SCA_Set_Display_Mute_export(PSCA_DISPLAY_MUTE_CONFIG pConfig)
{
    return SCA_Set_Display_Mute(pConfig, E_SCREEN_MUTE_INPUT);
}

MS_BOOL SCA_YPBPR_VGA_Mode_Monitor_Parse(PSCA_MODE_MONITOR_PARSE_IN_CONFIG pInConfig, PSCA_MODE_MONITOR_PARSE_OUT_CONFIG pOutConfig)
{

    SCALER_WIN eWindow;
    INPUT_SOURCE_TYPE_t enInputSourceType;
    XC_MODEPARSE_RESULT stModeParseResult;
    PCMODE_SyncStatus   stPCMonitorStatus;

    enInputSourceType = _Trans_To_Drv_Src(pInConfig->enSrcType);
    if(enInputSourceType == INPUT_SOURCE_NONE)
    {
        return FALSE;
    }

    eWindow = _Trans_To_Drv_Dest(pInConfig->enDestType);
    if(eWindow == MAX_WINDOW)
    {
        return FALSE;
    }

    stModeParseResult = MApi_PC_MainWin_Handler(enInputSourceType, eWindow);

    stPCMonitorStatus = MApi_PCMode_GetCurrentState(eWindow);

    pOutConfig->eModeMonitorStatus = stPCMonitorStatus == E_PCMODE_NOSYNC                 ? SCA_MODE_MONITOR_NOSYNC :
                                     stPCMonitorStatus == E_PCMODE_STABLE_SUPPORT_MODE    ? SCA_MODE_MONITOR_STABLE_SUPPORT_MODE :
                                     stPCMonitorStatus == E_PCMODE_STABLE_UN_SUPPORT_MODE ? SCA_MODE_MONITOR_STABLE_UN_SUPPORT_MODE :
                                                                                            SCA_MODE_MONITOR_UNSTABLE;

    if(stPCMonitorStatus == E_PCMODE_STABLE_SUPPORT_MODE &&
       stModeParseResult == XC_MODEPARSE_SUPPORT_MODE)
    {
        MS_PCADC_MODETABLE_TYPE stModeTbl;

        pOutConfig->u8ModeIdx = MApi_PCMode_Get_Mode_Idx(eWindow);
        MApi_PCMode_GetModeTbl(pOutConfig->u8ModeIdx, &stModeTbl);

        pOutConfig->u16HorizontalStart = stModeTbl.u16HorizontalStart;
        pOutConfig->u16VerticalStart   = stModeTbl.u16VerticalStart;
        pOutConfig->u16HorizontalTotal = stModeTbl.u16HorizontalTotal;
    }

    SCA_DBG(SCA_DBG_LV_MODE_PARSE, "eModeMonitorStatus SCA:%d  DRV:%d\r\n", pOutConfig->eModeMonitorStatus, stPCMonitorStatus);
    return TRUE;
}

 MS_BOOL SCA_Get_Mode(PSCA_GET_MODE_IN_CONFIG pInConfig, PSCA_GET_MODE_OUT_CONFIG pOutConfig)
{
    SCALER_WIN eWindow;
    INPUT_SOURCE_TYPE_t enInputSourceType;

    MS_BOOL bRet = TRUE;

    enInputSourceType = _Trans_To_Drv_Src(pInConfig->enSrcType);
    if(enInputSourceType == INPUT_SOURCE_NONE)
    {
        return FALSE;
    }

    eWindow = _Trans_To_Drv_Dest(pInConfig->enDestType);
    if(eWindow == MAX_WINDOW)
    {
        return FALSE;
    }

    if(IsSrcTypeAnalog(enInputSourceType))
    {

        MS_U8 u8CurrentSyncStatus = MApi_XC_PCMonitor_GetSyncStatus(eWindow);
        MS_PCADC_MODESETTING_TYPE  stModeSetting = MApi_PCMode_Get_ModeInfo(eWindow);

        pOutConfig->bInterlace      = u8CurrentSyncStatus & XC_MD_INTERLACE_BIT ? TRUE : FALSE;
        pOutConfig->u16InputVFreq   = MApi_XC_PCMonitor_Get_VFreqx10(eWindow);
        pOutConfig->u16InputVtotal  = MApi_XC_PCMonitor_Get_Vtotal(eWindow);
        pOutConfig->stSrcWin.x      = stModeSetting.u16DefaultHStart;
        pOutConfig->stSrcWin.y      = stModeSetting.u16DefaultVStart;
        pOutConfig->stSrcWin.width  = MApi_PCMode_Get_HResolution( eWindow ,TRUE);
        pOutConfig->stSrcWin.height = MApi_PCMode_Get_VResolution( eWindow ,TRUE);
    }
    else if (IsSrcTypeDTV(enInputSourceType))
    {
        MVOP_Timing stMVOPTiming;

        MDrv_MVOP_GetOutputTiming(&stMVOPTiming);

        pOutConfig->bInterlace      = stMVOPTiming.bInterlace;
        if(stMVOPTiming.bInterlace)
        {
            pOutConfig->u16InputVFreq  = (MS_U16)((stMVOPTiming.u16ExpFrameRate * 2 + 50) / 100);
        }
        else
        {
            pOutConfig->u16InputVFreq  = (MS_U16)((stMVOPTiming.u16ExpFrameRate + 50) / 100);
        }

        pOutConfig->u16InputVtotal  = stMVOPTiming.u16V_TotalCount;
        pOutConfig->stSrcWin.x      = MDrv_MVOP_GetHStart();
        pOutConfig->stSrcWin.y      = MDrv_MVOP_GetVStart();
        pOutConfig->stSrcWin.width  = stMVOPTiming.u16Width;
        pOutConfig->stSrcWin.height = stMVOPTiming.u16Height;
    }
    else if(IsSrcTypeCamera(enInputSourceType))
    {
        MS_INPUT_TIMING_CONFIG stCameraTiming;

        MApi_InputSrouce_GetCameraTiming(&stCameraTiming);
        pOutConfig->bInterlace      = stCameraTiming.bInterlace;
        pOutConfig->u16InputVFreq   = stCameraTiming.u16VFreq;
        pOutConfig->u16InputVtotal  = stCameraTiming.u16Vtotal;
        pOutConfig->stSrcWin.x      = stCameraTiming.u16x;
        pOutConfig->stSrcWin.y      = stCameraTiming.u16y;
        pOutConfig->stSrcWin.width  = stCameraTiming.u16Width;
        pOutConfig->stSrcWin.height = stCameraTiming.u16Height;
    }
    else if (IsSrcTypeDigitalVD(enInputSourceType))
    {
        EN_VD_SIGNALTYPE enVideoSystem = mvideo_vd_get_videosystem();
        MS_U16 u16InputHFreq;
        MS_WINDOW_TYPE stVDCaptreWin;

        if(enVideoSystem == SIG_NONE)
        {
            enVideoSystem = SIG_PAL;
        }

        u16InputHFreq = MApi_XC_CalculateHFreqx10(msAPI_Scaler_VD_GetHPeriod(eWindow, enInputSourceType, GET_SYNC_VIRTUAL));
        pOutConfig->u16InputVtotal = msAPI_Scaler_VD_GetVTotal(eWindow, enInputSourceType, GET_SYNC_VIRTUAL, u16InputHFreq);
        pOutConfig->bInterlace     = TRUE;
        pOutConfig->u16InputVFreq  = MApi_XC_CalculateVFreqx10(u16InputHFreq, pOutConfig->u16InputVtotal)*2;







#if(PQ_ENABLE_VD_SAMPLING)
    {
        MS_PQ_VD_Sampling_Info stVDSamplingInfo;
        MS_PQ_Vd_Info stPQVDInfo;

        switch(msAPI_AVD_GetVideoStandard())
        {
        case E_VIDEOSTANDARD_NTSC_M:
            stPQVDInfo.enVideoStandard = E_PQ_VIDEOSTANDARD_NTSC_M;
            break;

        case E_VIDEOSTANDARD_PAL_BGHI:
            stPQVDInfo.enVideoStandard = E_PQ_VIDEOSTANDARD_PAL_BGHI;
            break;

        case E_VIDEOSTANDARD_SECAM:
            stPQVDInfo.enVideoStandard = E_PQ_VIDEOSTANDARD_SECAM;
            break;

        case E_VIDEOSTANDARD_NTSC_44:
            stPQVDInfo.enVideoStandard = E_PQ_VIDEOSTANDARD_NTSC_44;
            break;

        case E_VIDEOSTANDARD_PAL_M:
            stPQVDInfo.enVideoStandard = E_PQ_VIDEOSTANDARD_PAL_M;
            break;

        case E_VIDEOSTANDARD_PAL_N:
            stPQVDInfo.enVideoStandard = E_PQ_VIDEOSTANDARD_PAL_N;
            break;

        case E_VIDEOSTANDARD_PAL_60:
            stPQVDInfo.enVideoStandard = E_PQ_VIDEOSTANDARD_PAL_60;
            break;

        default:
            MS_ASSERT(0);
            stPQVDInfo.enVideoStandard = E_PQ_VIDEOSTANDARD_PAL_BGHI;
            break;
        }

        memset(&stVDSamplingInfo, 0, sizeof(MS_PQ_VD_Sampling_Info));

        if(IsSrcTypeATV(enInputSourceType))
        {
            MDrv_PQ_Get_VDSampling_Info(PQ_MAIN_WINDOW, PQ_INPUT_SOURCE_TV, stPQVDInfo.enVideoStandard, &stVDSamplingInfo);
        }
        else if(IsSrcTypeAV(enInputSourceType))
        {
            MDrv_PQ_Get_VDSampling_Info(PQ_MAIN_WINDOW, PQ_INPUT_SOURCE_CVBS, stPQVDInfo.enVideoStandard, &stVDSamplingInfo);
        }
        else if(IsSrcTypeSV(enInputSourceType))
        {
            MDrv_PQ_Get_VDSampling_Info(PQ_MAIN_WINDOW, PQ_INPUT_SOURCE_SVIDEO, stPQVDInfo.enVideoStandard, &stVDSamplingInfo);
        }
        else if(IsSrcTypeScart(enInputSourceType))
        {
            MDrv_PQ_Get_VDSampling_Info(PQ_MAIN_WINDOW, PQ_INPUT_SOURCE_SCART, stPQVDInfo.enVideoStandard, &stVDSamplingInfo);
        }

        msAPI_AVD_GetCaptureWindowByPQ(&stVDCaptreWin, enVideoSystem, &stVDSamplingInfo);
    }
#else
        msAPI_AVD_GetCaptureWindow(&stVDCaptreWin, enVideoSystem, FALSE);
#endif

#if (PQ_ENABLE_VD_SAMPLING)
        if(stVDSamplingInfo.eType == PQ_VD_SAMPLING_ON)
        {
            pOutConfig->stSrcWin.x = stVDSamplingInfo.u16Hstart;
            pOutConfig->stSrcWin.y = stVDSamplingInfo.u16Vstart;
        }
        else
#endif
        {
            pOutConfig->stSrcWin.x = CVBS_WinInfo[enVideoSystem][0].u16H_CapStart;
            pOutConfig->stSrcWin.y = CVBS_WinInfo[enVideoSystem][0].u16V_CapStart;
        }

        pOutConfig->stSrcWin.width  = stVDCaptreWin.width;
        pOutConfig->stSrcWin.height = stVDCaptreWin.height;


        switch ( enVideoSystem )
        {
        case SIG_NTSC:
        case SIG_NTSC_443:
        case SIG_PAL_M:
            if ( (pOutConfig->u16InputVFreq > 610) || (pOutConfig->u16InputVFreq < 590) )
            {
                pOutConfig->u16InputVFreq = 600;
            }
            break;

        default:
        case SIG_PAL:
        case SIG_SECAM:
        case SIG_PAL_NC:
            if ( (pOutConfig->u16InputVFreq > 510) || (pOutConfig->u16InputVFreq < 490) )
            {
                pOutConfig->u16InputVFreq = 500;
            }
            break;
        }
    }
	else if(enInputSourceType >= INPUT_SOURCE_BT656_0 && enInputSourceType < INPUT_SOURCE_BT656_MAX)
	{

        U8 u8CurrentSyncStatus = MApi_XC_PCMonitor_GetSyncStatus(eWindow);
        MS_PCADC_MODESETTING_TYPE  stModeSetting = MApi_PCMode_Get_ModeInfo(eWindow);
	//		RETAILMSG(FALSE,(TEXT("[BT656] get mode : sync : %d \r\n"),u8CurrentSyncStatus));

        pOutConfig->bInterlace      = u8CurrentSyncStatus & XC_MD_INTERLACE_BIT ? TRUE : FALSE;
        pOutConfig->u16InputVFreq   = MApi_XC_PCMonitor_Get_VFreqx10(eWindow);
        pOutConfig->u16InputVtotal  = MApi_XC_PCMonitor_Get_Vtotal(eWindow);
        pOutConfig->stSrcWin.x      = stModeSetting.u16DefaultHStart;
        pOutConfig->stSrcWin.y      = stModeSetting.u16DefaultVStart;
        pOutConfig->stSrcWin.width  = MApi_PCMode_Get_HResolution( eWindow ,TRUE);
        pOutConfig->stSrcWin.height = MApi_PCMode_Get_VResolution( eWindow ,TRUE);
	}
    else
    {
        bRet = FALSE;
    }

    return bRet;
}

MS_BOOL SCA_Set_VE(PSCA_VE_CONFIG pConfig)
{
    MS_VE_VIDEOSYS enVideoSys;
    MS_VE_Set_Mode_Type stVEModeType;
    INPUT_SOURCE_TYPE_t enInputSourceType;
    enInputSourceType = _Trans_To_Drv_Src(pConfig->enSrcType);
    if(enInputSourceType == INPUT_SOURCE_NONE)
    {
        return FALSE;
    }

    if(pConfig->enVideoStd == SCA_VE_OUTPUT_NTSC)
    {
        enVideoSys = MS_VE_NTSC;
        g_enTGenTiming = E_XC_TGEN_VE_480_I_60;
    }
    else
    {
        enVideoSys = MS_VE_PAL;
        g_enTGenTiming = E_XC_TGEN_VE_576_I_50;
    }

    SCA_DBG(SCA_DBG_LV_0, "SET_VE:%d \n", g_enTGenTiming);
    stVEModeType.bHDuplicate   = pConfig->bHDuplicate;
    stVEModeType.bSrcInterlace = pConfig->bSrcInterlace;
    stVEModeType.u16InputVFreq = pConfig->u16InputVFreq;

    MDrv_SC_iDAC_SetEnabled(FALSE);
    if(enInputSourceType == INPUT_SOURCE_DTV)
    {
        MVOP_Timing stMVOPTiming;

        MDrv_MVOP_GetOutputTiming(&stMVOPTiming);
        stVEModeType.u16H_CapSize  = stMVOPTiming.u16Width;
        stVEModeType.u16V_CapSize  = stMVOPTiming.u16Height;
        stVEModeType.u16H_CapStart = MDrv_MVOP_GetHStart();
        stVEModeType.u16V_CapStart = MDrv_MVOP_GetVStart();
        stVEModeType.u16H_SC_CapStart = stVEModeType.u16H_CapStart;
        stVEModeType.u16V_SC_CapStart = stVEModeType.u16V_CapStart;
    }
    else
    {
        stVEModeType.u16H_CapSize     = pConfig->u16H_CapSize;
        stVEModeType.u16V_CapSize     = pConfig->u16V_CapSize;
        stVEModeType.u16H_CapStart    = pConfig->u16H_CapStart;
        stVEModeType.u16V_CapStart    = pConfig->u16V_CapStart;
        stVEModeType.u16H_SC_CapStart = pConfig->u16H_CapStart;
        stVEModeType.u16V_SC_CapStart = pConfig->u16V_CapStart;
    }

    stVEModeType.u16H_SC_CapSize  = pConfig->u16H_CapSize;
    stVEModeType.u16V_SC_CapSize  = pConfig->u16V_CapSize;

    MDrv_SC_iDAC_SetEnabled(TRUE);
    MApi_SetVE(enInputSourceType, OUTPUT_CVBS1, enVideoSys, stVEModeType);

    MApi_XC_Set_OutputColor(SC1_MAIN_WINDOW, E_XC_OUTPUT_COLOR_YUV);


    MApi_XC_SetFrameColor(0x800080, SC1_MAIN_WINDOW);
    MApi_XC_SetFrameColor_En(TRUE, SC1_MAIN_WINDOW);
    return TRUE;
}

MS_BOOL SCA_Set_Window_OnOFF(PSCA_DISPLAY_WINDOW_ON_OFF_CONFIG pConfig)
{

    MApi_Set_Window_OnOFF(pConfig->bMainEn, pConfig->bSubEn);
    return TRUE;
}

MS_BOOL SCA_Set_ConstantAlpah_Value(PSCA_CONSTANT_ALPHA_SETTING pConfig)
{
    #define GOP_CONSTALPHA_MASK		(0xFF)
    #define REG_GOP0_1_BASEADDR     (0x120300)
    #define REG_GOP1_1_BASEADDR     (0x120600)
    #define REG_GOP_0A_L            (0x0A*2)
    #define REG_GOP_7F_H            (0x7F*2+1)
	MS_BOOL bRet;
    MS_U32 u32RegAlpha;
    MS_U32 u32RegBkSel;

	switch (pConfig->u8View)
	{
		case 0:
			u32RegAlpha = REG_GOP0_1_BASEADDR + REG_GOP_0A_L;
			u32RegBkSel = REG_GOP0_1_BASEADDR + REG_GOP_7F_H;
			bRet = TRUE;
			break;

		case 1:
			u32RegAlpha  = REG_GOP1_1_BASEADDR + REG_GOP_0A_L;
			u32RegBkSel  = REG_GOP1_1_BASEADDR + REG_GOP_7F_H;
			bRet = TRUE;
			break;

		default:
			bRet = FALSE;
			SCA_DBG(SCA_DBG_LV_0, "Invalid View Number:%d", pConfig->u8View);
			break;
	}

	if (bRet)
	{
	    MS_U8 u8OldBkSel = MApi_XC_ReadByte(u32RegBkSel);

	    MApi_XC_WriteByte(u32RegBkSel, u8OldBkSel | 0x02); // force write
    	MApi_XC_Write2ByteMask(u32RegAlpha, pConfig->u8AlphaValue, GOP_CONSTALPHA_MASK);
    	MApi_XC_WriteByte(u32RegBkSel, u8OldBkSel);
	}

	return bRet;
}


MS_BOOL SCA_Set_ConstantAlpha_State(PSCA_CONSTANTALPHA_STATE pConfig)
{
    #define GOP_CONSTALPHA_MASK        (0xFF)
    #define REG_GOP0_0_BASEADDR     (0x120200)
    #define REG_GOP0_1_BASEADDR     (0x120300)

    #define REG_GOP1_0_BASEADDR     (0x120500)
    #define REG_GOP1_1_BASEADDR     (0x120600)

    #define REG_GOP2_0_BASEADDR     (0x120800)
    #define REG_GOP2_1_BASEADDR     (0x120900)

    #define REG_GOP3_0_BASEADDR     (0x120B00)
    #define REG_GOP3_1_BASEADDR     (0x120C00)

    #define REG_GOP_00_H            (0x00*2+1)
    #define REG_GOP_7F_H            (0x7F*2+1)
    MS_BOOL bRet;
    MS_U32 u32RegAlpha_Type_BkSel;
    MS_U32 u32RegAlpha_out_inv_BkSel;

        switch (pConfig->u8View)
        {
            case 0:
                u32RegAlpha_Type_BkSel   = REG_GOP0_1_BASEADDR + REG_GOP_7F_H;
                u32RegAlpha_out_inv_BkSel= REG_GOP0_0_BASEADDR + REG_GOP_7F_H;

                bRet = TRUE;
                break;

            case 1:
                u32RegAlpha_Type_BkSel    = REG_GOP1_1_BASEADDR + REG_GOP_7F_H;
                u32RegAlpha_out_inv_BkSel = REG_GOP1_0_BASEADDR + REG_GOP_7F_H;
                bRet = TRUE;
                break;

            case 2:
                u32RegAlpha_Type_BkSel   = REG_GOP2_1_BASEADDR + REG_GOP_7F_H;
                u32RegAlpha_out_inv_BkSel= REG_GOP2_0_BASEADDR + REG_GOP_7F_H;

                bRet = TRUE;
                break;

             case 3:
                u32RegAlpha_Type_BkSel    = REG_GOP3_1_BASEADDR + REG_GOP_7F_H;
                u32RegAlpha_out_inv_BkSel = REG_GOP3_0_BASEADDR + REG_GOP_7F_H;
                bRet = TRUE;
                break;



            default:
                bRet = FALSE;
                SCA_DBG(SCA_DBG_LV_0, "Invalid View Number:%d", pConfig->u8View);
                break;
        }

        if (bRet)
        {
            MS_U8 u8Old_1_BkSel = MApi_XC_ReadByte(u32RegAlpha_Type_BkSel);
            MS_U8 u8Old_2_BkSel = MApi_XC_ReadByte(u32RegAlpha_out_inv_BkSel);
            if (pConfig->Benable == 1)
            {
                //printk("Switch to constant alpha \n");

                MApi_XC_WriteByte(u32RegAlpha_Type_BkSel,    u8Old_1_BkSel | 0x02); // force write
                MApi_XC_WriteByte(u32RegAlpha_out_inv_BkSel, u8Old_2_BkSel | 0x02); // force write


                MApi_XC_Write2ByteMask(REG_GOP0_0_BASEADDR, 0x0000, 0x8000);
                MApi_XC_Write2ByteMask(REG_GOP0_1_BASEADDR, 0x0000, 0x4000);
            }
            else
            {
                //printk("Switch to pixel alpha \n");

                MApi_XC_WriteByte(u32RegAlpha_Type_BkSel,    u8Old_1_BkSel | 0x02); // force write
                MApi_XC_WriteByte(u32RegAlpha_out_inv_BkSel, u8Old_2_BkSel | 0x02); // force write


                MApi_XC_Write2ByteMask(REG_GOP0_0_BASEADDR, 0x8000, 0x8000);
                MApi_XC_Write2ByteMask(REG_GOP0_1_BASEADDR, 0x4000, 0x4000);

            }

            MApi_XC_WriteByte(u32RegAlpha_Type_BkSel, u8Old_1_BkSel);
            MApi_XC_WriteByte(u32RegAlpha_out_inv_BkSel, u8Old_2_BkSel);

        }

        return bRet;
}


MS_BOOL SCA_Set_Color_Key(PSCA_COLOR_KEY pConfig)
{
    MS_U32 u32regbase = 0x120200;
	//DisableOverlay();
	if (pConfig->View == 0)
	{
	    u32regbase = 0x120200;
	}
	else if (pConfig->View == 1)
	{
	    u32regbase = 0x120500;

	}
    else if (pConfig->View == 2)
    {
        u32regbase = 0x120800;
    }
    else if (pConfig->View == 3)
    {
        u32regbase = 0x120B00;
    }
	else
	{
	    u32regbase = 0x120200;
	}

	if (TRUE == pConfig->Benable)
	{
		MApi_XC_Write2ByteMask(u32regbase+(0x00*2), 0x0800, 0x0800);
		MApi_XC_Write2ByteMask(u32regbase+(0x24*2),((0x0000|(pConfig->u8G<<8))|(pConfig->u8B)), 0xFFFF);
		MApi_XC_Write2ByteMask(u32regbase+(0x25*2), (0x0000|(pConfig->u8R)), 0xFFFF);
	}
	else
	{
		MApi_XC_Write2ByteMask(u32regbase+(0x00*2), 0x0000, 0x0800);
	}

    MApi_XC_Write2ByteMask(u32regbase+(0x7F*2), 0x0100, 0x0100);
    MApi_XC_Write2ByteMask(u32regbase+(0x7F*2), 0x0000, 0x0100);

	return TRUE;

/*	  // RGB565 only

	if ((pConfig->u8R> 0x1F) ||(pConfig->u8B > 0x1F) || (pConfig->u8G > 0x3F))
	{
			return FALSE;
	}

	if (TRUE == pConfig->Benable)
	{
		MApi_XC_Write2ByteMask(0x101F00+(0x7F*2), 0x0200, 0xFFFF);

		MApi_XC_Write2ByteMask(0x101F00+(0x00*2), 0x0800, 0x0800);
		MApi_XC_Write2ByteMask(0x101F00+(0x24*2),((0x0000|(pConfig->u8G<<2)|(pConfig->u8G>>3))|(0x0000|(pConfig->u8B<<3)|(pConfig->u8B>>2))), 0xFFFF);
		MApi_XC_Write2ByteMask(0x101F00+(0x25*2), (0x0000|(pConfig->u8R<<3)|(pConfig->u8R>>2)), 0xFFFF);
	}
	else
	{
		MApi_XC_Write2ByteMask(0x101F00+(0x7F*2), 0x0200, 0xFFFF);
		MApi_XC_Write2ByteMask(0x101F00+(0x00*2), 0x0000, 0x0800);
	}

	return TRUE;
*/
}

MS_BOOL SCA_GetLibVer(PSCA_LIB_VER pstOutLibVer)
{
   if (!pstOutLibVer)
       return FALSE;

   *pstOutLibVer = sca_lib_ver;
    return TRUE;
}

MS_BOOL SCA_Set_MVOP_BaseAddr(PSCA_MVOP_BASE_ADDR_CONFIG pConfig)
{
    return MDrv_MVOP_SetBaseAdd(pConfig->u32YOffset, pConfig->u32UVOffset, pConfig->bProgressive, pConfig->b422pack);
}

MS_BOOL SCA_Get_MVOP_BaseAddr(PSCA_MVOP_BASE_ADDR_CONFIG pConfig)
{
   MS_U32 u32YOffset;
   MS_U32 u32UVOffset;
   MDrv_MVOP_GetBaseAdd(&u32YOffset, &u32UVOffset);
   pConfig->u32YOffset = u32YOffset;
   pConfig->u32UVOffset = u32UVOffset;
   return TRUE;
}

MS_BOOL SCA_Change_Window(PSCA_CHANGE_WINDOW_CONFIG pConfig)
{
    MS_BOOL bRet = TRUE;
    INPUT_SOURCE_TYPE_t enInputSourceType;
    SCALER_WIN eWindow;

    SCA_WAIT_MUTEX(_SCA_Cnnt_Mutex);
    if((g_stDispPathCfg[pConfig->enDestType].dwUID != pConfig->dwUID)
        &&(pConfig->dwUID != DBG_UID)
      )
    {
        SCA_DBG(SCA_DBG_LV_0,"[SCA] change window: ID not match: %x != %x \n", (int)pConfig->dwUID, (int)g_stDispPathCfg[pConfig->enDestType].dwUID);
        SCA_RELEASE_MUTEX(_SCA_Cnnt_Mutex);
        return FALSE;
    }
    SCA_RELEASE_MUTEX(_SCA_Cnnt_Mutex);


    enInputSourceType = _Trans_To_Drv_Src(pConfig->enSrcType);
    if(enInputSourceType == INPUT_SOURCE_NONE)
    {
        bRet = FALSE;
    }

    eWindow = _Trans_To_Drv_Dest(pConfig->enDestType);
    if(eWindow == MAX_WINDOW)
    {
       bRet = FALSE;
    }

#if ENABLE_MONITOR_THREAD

    if(IsSrcTypeAnalog(enInputSourceType) ||
       IsSrcTypeDigitalVD(enInputSourceType) ||
       IsSrcTypeBT656(enInputSourceType)
#ifdef __DUAL_SCREEN__
       || IsSrcTypeSC_VOP(enInputSourceType)
#endif
      )
    {
        SCA_WAIT_MUTEX(_SCA_MoniWinCfg_Mutex);

        g_stMonitorWinCfg[pConfig->enDestType].enSrcType = pConfig->enSrcType;
        memcpy(&g_stMonitorWinCfg[pConfig->enDestType].stWinConfig, &pConfig->stWinConfig, sizeof(SCA_WINDOW_CONFIG));

        SCA_DBG(SCA_DBG_LV_PROGWIN, "SCA_Change_Window:: SRC:%d, DEST:%d bSrc:%d, %d %d %d %d, bCrop:%d %d %d %d %d, bDisp:%d %d %d %d %d \n",
            (int)pConfig->enSrcType, (int)pConfig->enDestType,
            g_stMonitorWinCfg[pConfig->enDestType].stWinConfig.bSrcWin,
            g_stMonitorWinCfg[pConfig->enDestType].stWinConfig.stSrcWin.x,
            g_stMonitorWinCfg[pConfig->enDestType].stWinConfig.stSrcWin.y,
            g_stMonitorWinCfg[pConfig->enDestType].stWinConfig.stSrcWin.width,
            g_stMonitorWinCfg[pConfig->enDestType].stWinConfig.stSrcWin.height,
            g_stMonitorWinCfg[pConfig->enDestType].stWinConfig.bCropWin,
            g_stMonitorWinCfg[pConfig->enDestType].stWinConfig.stCropWin.x,
            g_stMonitorWinCfg[pConfig->enDestType].stWinConfig.stCropWin.y,
            g_stMonitorWinCfg[pConfig->enDestType].stWinConfig.stCropWin.width,
            g_stMonitorWinCfg[pConfig->enDestType].stWinConfig.stCropWin.height,
            g_stMonitorWinCfg[pConfig->enDestType].stWinConfig.bDispWin,
            g_stMonitorWinCfg[pConfig->enDestType].stWinConfig.stDispWin.x,
            g_stMonitorWinCfg[pConfig->enDestType].stWinConfig.stDispWin.y,
            g_stMonitorWinCfg[pConfig->enDestType].stWinConfig.stDispWin.width,
            g_stMonitorWinCfg[pConfig->enDestType].stWinConfig.stDispWin.height);
        SCA_RELEASE_MUTEX(_SCA_MoniWinCfg_Mutex);
    }

#endif


    if(bRet && pConfig->bImmediateUpdate)
    {
        _Prog_Window(enInputSourceType, eWindow, &pConfig->stWinConfig);
        MApi_SetColor(enInputSourceType, eWindow);
    }

#ifdef __DUAL_SCREEN__
{
    SCA_WINDOW_CONFIG stWinCfg;
    SCA_SRC_TYPE enSC1_SrcType;

    SCA_WAIT_MUTEX(_SCA_MoniWinCfg_Mutex);
    memcpy(&stWinCfg, &g_stMonitorWinCfg[SCA_DEST_MAIN_1].stWinConfig, sizeof(SCA_WINDOW_CONFIG));
    SCA_RELEASE_MUTEX(_SCA_MoniWinCfg_Mutex);

    SCA_WAIT_MUTEX(_SCA_Cnnt_Mutex);
    enSC1_SrcType = g_stDispPathCfg[SCA_DEST_MAIN_1].enSrcType ;
    SCA_RELEASE_MUTEX(_SCA_Cnnt_Mutex);

    if(pConfig->enDestType == SCA_DEST_MAIN && enSC1_SrcType == SCA_SRC_SC0_VOP)
    {
        SCA_DISPLAY_MUTE_CONFIG stDisplayMute;
        // display black screen as a compromise of blurred screen
        stDisplayMute.bEn = TRUE;
        stDisplayMute.enDestType = SCA_DEST_MAIN_1;
        stDisplayMute.dwUID = g_stDispPathCfg[SCA_DEST_MAIN_1].dwUID;
        _Set_Display_Mute(&stDisplayMute, E_SCREEN_MUTE_MONITOR);

        _Prog_Window(INPUT_SOURCE_SC0_VOP, SC1_MAIN_WINDOW, &stWinCfg);

        stDisplayMute.bEn = FALSE;
        stDisplayMute.enDestType = SCA_DEST_MAIN_1;
        stDisplayMute.dwUID = g_stDispPathCfg[SCA_DEST_MAIN_1].dwUID;
        _Set_Display_Mute(&stDisplayMute, E_SCREEN_MUTE_MONITOR);
    }
}
#endif
    return bRet;
}


MS_BOOL SCA_Set_Picture(PSCA_SET_PICTURE_CONFIG pConfig)
{
    MS_BOOL bRet = TRUE;
    INPUT_SOURCE_TYPE_t enInputSourceType;
    SCALER_WIN eWindow;

    enInputSourceType = _Trans_To_Drv_Src(pConfig->enSrcType);
    if(enInputSourceType == INPUT_SOURCE_NONE)
    {
        bRet = FALSE;
    }

    eWindow = _Trans_To_Drv_Dest(pConfig->enDestType);
    if(eWindow != MAIN_WINDOW)
    {
       bRet = FALSE;
    }

    if(bRet)
    {
        MS_BOOL bYUVSpace = MApi_XC_IsYUVSpace(eWindow);
        MS_COLOR_TRANS_TYPE enColorTrans;

        SCA_DBG(SCA_DBG_LV_COLOR, "SCA_Set_Picture::: Win:%d, Output <%s -> %s> \n",
                eWindow,
                bYUVSpace ? "YUV" : "RGB",
                (MApi_XC_Get_OutputColor(eWindow) == E_XC_OUTPUT_COLOR_YUV) ? "YUV" : "RGB");

        if(eWindow == MAIN_WINDOW || eWindow == SUB_WINDOW)
        {

            enColorTrans = bYUVSpace ? MS_COLOR_YUV_TO_RGB : MS_COLOR_RGB_TO_RGB;
        }
        else
        {
            if(MApi_XC_Get_OutputColor(eWindow) == E_XC_OUTPUT_COLOR_YUV)
            {
                enColorTrans = bYUVSpace ? MS_COLOR_YUV_TO_YUV : MS_COLOR_RGB_TO_YUV;
            }
            else
            {
                enColorTrans = bYUVSpace ? MS_COLOR_YUV_TO_RGB : MS_COLOR_RGB_TO_RGB;
            }


        #if PQ_ENABLE_COLOR
            if(eWindow == SC1_MAIN_WINDOW)
            {
                MApi_XC_ACE_ColorCorrectionTable(eWindow, (MS_S16 *)S16SC1_ColorCorrectMatrix);
                MApi_XC_ACE_PCsRGBTable(eWindow, (MS_S16 *)S16SC1_ColorCorrectMatrix);
            }
            else if(eWindow == SC2_MAIN_WINDOW)
            {
                MApi_XC_ACE_ColorCorrectionTable(eWindow, (MS_S16 *)S16SC2_ColorCorrectMatrix);
                MApi_XC_ACE_PCsRGBTable(eWindow, (MS_S16 *)S16SC2_ColorCorrectMatrix);
            }
        #else
            MApi_XC_ACE_ColorCorrectionTable(eWindow, (MS_S16 *)S16ByPassColorCorrectionMatrix);
            MApi_XC_ACE_PCsRGBTable(eWindow, (MS_S16 *)S16ByPassDefaultRGB);
        #endif
        }

        if(enColorTrans == MS_COLOR_YUV_TO_RGB ||  enColorTrans == MS_COLOR_YUV_TO_YUV) // do YUV -> RGB
        {
            if(enColorTrans == MS_COLOR_YUV_TO_YUV)
            {
                MApi_XC_ACE_SetPCYUV2RGB( eWindow, FALSE);
                MApi_XC_ACE_SelectYUVtoRGBMatrix( eWindow, E_XC_ACE_YUV_TO_RGB_MATRIX_USER, (MS_S16 *)S16ByPassDefaultRGB);
            }
            else
            {
                MApi_XC_ACE_SetPCYUV2RGB( eWindow, TRUE);
                MApi_XC_ACE_SelectYUVtoRGBMatrix( eWindow, E_XC_ACE_YUV_TO_RGB_MATRIX_SDTV, NULL);
            }
        }
        else if(enColorTrans == MS_COLOR_RGB_TO_RGB)// don't YUV -> RGB, need bypass
        {
            MApi_XC_ACE_SetPCYUV2RGB( eWindow, FALSE);
        }
        else // MS_COLOR_RGB_2_YUV
        {

        }


         MApi_XC_ACE_SkipWaitVsync(eWindow, TRUE);

        if(pConfig->enType & SCA_PICTURE_BRIGHTNESS)
        {
            MS_U8 u8SubBrightness;
            MS_U8 u8BR, u8BG, u8BB;

            MApi_SetPictureSetting(enInputSourceType, PICTURE_BRIGHTNESS, pConfig->u8Brightness, eWindow);

            u8SubBrightness = MApi_GetSubColor(enInputSourceType, SUB_COLOR_BRIGHTNESS, eWindow);

            u8BR = MApi_ACE_transfer_Bri((MApi_FactoryAdjBrightness(MApi_PictureBrightnessN100toReallyValue(eWindow, enInputSourceType, pConfig->u8Brightness),u8SubBrightness)), BRIGHTNESS_R);
            u8BG = MApi_ACE_transfer_Bri((MApi_FactoryAdjBrightness(MApi_PictureBrightnessN100toReallyValue(eWindow, enInputSourceType, pConfig->u8Brightness),u8SubBrightness)), BRIGHTNESS_G);
            u8BB = MApi_ACE_transfer_Bri((MApi_FactoryAdjBrightness(MApi_PictureBrightnessN100toReallyValue(eWindow, enInputSourceType, pConfig->u8Brightness),u8SubBrightness)), BRIGHTNESS_B);

            SCA_DBG(SCA_DBG_LV_COLOR_MATRIX, "SCA_Set_Picture::Brightness: %d %d %d  !!! \n", u8BR, u8BG, u8BB);

            MApi_XC_ACE_PicSetBrightnessInVsync(eWindow, u8BR, u8BG, u8BB);

        }

        if(pConfig->enType & SCA_PICTURE_CONTRAST)
        {
            MS_U8 u8Contrast, u8SubContrast;

            MApi_SetPictureSetting(enInputSourceType, PICTURE_CONTRAST, pConfig->u8Contrast, eWindow);

            u8SubContrast = MApi_GetSubColor(enInputSourceType, SUB_COLOR_CONTRAST, eWindow);
            u8Contrast = MApi_FactoryContrast(MApi_PictureContrastN100toReallyValue(eWindow, enInputSourceType, pConfig->u8Contrast),u8SubContrast);

            SCA_DBG(SCA_DBG_LV_COLOR_MATRIX, "SCA_Set_Picture::Win:%d, Contrast: %d !!! \n", eWindow, u8Contrast);

            if(enColorTrans == MS_COLOR_YUV_TO_YUV)
            {
                SCA_DBG(SCA_DBG_LV_COLOR_MATRIX, "SCA_Set_Picture::Only apply on Y Contrast \n");
                MApi_XC_ACE_PicSetContrast_G(eWindow, bYUVSpace, u8Contrast);
            }
            else
            {
                MApi_XC_ACE_PicSetContrast(eWindow, bYUVSpace, u8Contrast);
            }
        }

        if(pConfig->enType & SCA_PICTURE_HUE)
        {
            MApi_SetPictureSetting(enInputSourceType, PICTURE_HUE, pConfig->u8Hue, eWindow);

            if(enColorTrans == MS_COLOR_YUV_TO_RGB ||  enColorTrans == MS_COLOR_YUV_TO_YUV)
            {
                MS_U8 u8Hue;

                u8Hue = MApi_PictureHueN100toReallyValue(eWindow, enInputSourceType, MApi_GetPictureSetting(enInputSourceType, PICTURE_HUE, eWindow));
                SCA_DBG(SCA_DBG_LV_COLOR_MATRIX, "SCA_Set_Picture::Hue: %d !!! \n", u8Hue);
                MApi_XC_ACE_PicSetHue( eWindow, bYUVSpace, u8Hue);
            }
        }

        if(pConfig->enType & SCA_PICTURE_SATURATION)
        {
            MApi_SetPictureSetting(enInputSourceType, PICTURE_SATURATION, pConfig->u8Saturation, eWindow);

            if(enColorTrans == MS_COLOR_YUV_TO_RGB ||  enColorTrans == MS_COLOR_YUV_TO_YUV)
            {
                MS_U8 u8Saturation;

                u8Saturation = MApi_PictureSaturationN100toReallyValue( eWindow, enInputSourceType, MApi_GetPictureSetting(enInputSourceType, PICTURE_SATURATION, eWindow));
                SCA_DBG(SCA_DBG_LV_COLOR_MATRIX, "SCA_Set_Picture::win:%d Saturation: %d !!! \n", eWindow, u8Saturation);

                MApi_XC_ACE_PicSetSaturation(eWindow, bYUVSpace, u8Saturation);
            }
        }

        if(pConfig->enType & SCA_PICTURE_SHARPNESS)
        {
            MS_U8 u8Shaprness;
            MApi_SetPictureSetting(enInputSourceType, PICTURE_SHARPNESS, pConfig->u8Sharpness, eWindow);

            u8Shaprness = MApi_PictureSharpnessN100toReallyValue( eWindow, enInputSourceType, MApi_GetPictureSetting(enInputSourceType, PICTURE_SHARPNESS, eWindow));

            SCA_DBG(SCA_DBG_LV_COLOR_MATRIX, "SCA_Set_Picture::win:%d Sharpness: %d !!! \n", eWindow, u8Shaprness);

            MApi_XC_ACE_PicSetSharpness( eWindow, u8Shaprness);


        }

        if(pConfig->enType & SCA_PICTURE_COLOR_TEMPERATURE)
        {
            MS_COLOR_TEMP_TYPE enColorTempType;
            T_MS_COLOR_TEMP    stColorTemp;

            enColorTempType = pConfig->enColorTemp ==  SCA_COLOR_TEMP_COOL ?  COLOR_TEMP_COOL :
                                                       SCA_COLOR_TEMP_WARM ?  COLOR_TEMP_WARM :
                                                                              COLOR_TEMP_NORMAL ;
            MApi_SetColorTemp(enInputSourceType, enColorTempType, eWindow);

            stColorTemp = MApi_GetColorTempTBL(enInputSourceType, eWindow);

            MApi_XC_ACE_PicSetColorTemp( eWindow, bYUVSpace, (XC_ACE_color_temp *) &stColorTemp );

            SCA_DBG(SCA_DBG_LV_COLOR_MATRIX, "SCA_Set_Picture::win:%d colo, r:%d g:%d, b:%d!!! \n",
                eWindow, stColorTemp.cRedOffset, stColorTemp.cGreenOffset, stColorTemp.cBlueOffset);

            MApi_XC_ACE_PicSetBrightnessInVsync(eWindow,
                                                MApi_ACE_transferRGB_Bri(stColorTemp.cRedOffset, BRIGHTNESS_R ),
                                                MApi_ACE_transferRGB_Bri(stColorTemp.cGreenOffset, BRIGHTNESS_G),
                                                MApi_ACE_transferRGB_Bri(stColorTemp.cBlueOffset, BRIGHTNESS_B));
        }

    }

    MApi_XC_ACE_SkipWaitVsync(eWindow, FALSE);
    return TRUE;
}


MS_BOOL SCA_Get_Picture(PSCA_PICTURE_DATA pConfig)
{
    MS_BOOL bRet = TRUE;
    INPUT_SOURCE_TYPE_t enInputSourceType;
    SCALER_WIN eWindow;

    enInputSourceType = _Trans_To_Drv_Src(pConfig->enSrcType);
    if(enInputSourceType == INPUT_SOURCE_NONE)
    {
        bRet = FALSE;
    }

    eWindow = _Trans_To_Drv_Dest(pConfig->enDestType);
    if(eWindow != MAIN_WINDOW)
    {
       bRet = FALSE;
    }

    if(bRet)
    {

        pConfig->u8Brightness = MApi_GetPictureSetting(enInputSourceType, PICTURE_BRIGHTNESS, eWindow);
        pConfig->u8Contrast   = MApi_GetPictureSetting(enInputSourceType, PICTURE_CONTRAST, eWindow);
        pConfig->u8Saturation = MApi_GetPictureSetting(enInputSourceType, PICTURE_SATURATION, eWindow);
        pConfig->u8Hue        = MApi_GetPictureSetting(enInputSourceType, PICTURE_HUE, eWindow);
        pConfig->u8Sharpness  = MApi_GetPictureSetting(enInputSourceType, PICTURE_SHARPNESS, eWindow);

        switch(MApi_GetColorTemp(enInputSourceType, eWindow))
        {
        case COLOR_TEMP_COOL:
            pConfig->enColorTemp  = SCA_COLOR_TEMP_COOL;
            break;
        case COLOR_TEMP_WARM:
            pConfig->enColorTemp  = SCA_COLOR_TEMP_WARM;
            break;

        default:
            pConfig->enColorTemp  = SCA_COLOR_TEMP_NORMAL;
            break;
        }

    }

    return bRet;
}

MS_BOOL SCA_Set_Output_Timing(PSCA_OUTPUT_TIMING_CONFIG pConfig)
{
    MS_BOOL bRet = TRUE;
    XC_TGEN_INFO_t stTiming;
    SCALER_WIN eWindow;
    MS_U16 u16FrameRate;

    if(pConfig->enDestType == SCA_DEST_MAIN || pConfig->enDestType == SCA_DEST_SUB)
    {
        SCA_DBG(SCA_DBG_LV_0,"only for SC1/SC2");
        return FALSE;
    }

    if(pConfig->enDestType == SCA_DEST_MAIN_1 &&
       (pConfig->enTimingType == SCA_OUTPUT_480_P_30 || pConfig->enTimingType == SCA_OUTPUT_720_P_30)
      )
    {
        SCA_DBG(SCA_DBG_LV_0,"30P only for SC2");
        return FALSE;
    }

    eWindow = _Trans_To_Drv_Dest(pConfig->enDestType);


    stTiming.enBits = E_XC_HDMITX_8_BITS;
    switch(pConfig->enTimingType)
    {
    case SCA_OUTPUT_480_I_60:
        stTiming.enTimeType = E_XC_TGEN_HDMI_480_I_60;
        u16FrameRate = 60;
         break;
    case SCA_OUTPUT_480_P_60:
        stTiming.enTimeType = E_XC_TGEN_HDMI_480_P_60;
        u16FrameRate = 60;
        break;
    case SCA_OUTPUT_576_I_50:
        stTiming.enTimeType = E_XC_TGEN_HDMI_576_I_50;
        u16FrameRate = 50;
        break;
    case SCA_OUTPUT_576_P_50:
        stTiming.enTimeType = E_XC_TGEN_HDMI_576_P_50;
        u16FrameRate = 50;
        break;
    case SCA_OUTPUT_720_P_50:
        stTiming.enTimeType = E_XC_TGEN_HDMI_720_P_50;
        u16FrameRate = 50;
        break;
    case SCA_OUTPUT_720_P_60:
        stTiming.enTimeType = E_XC_TGEN_HDMI_720_P_60;
        u16FrameRate = 60;
        break;
    case SCA_OUTPUT_1080_P_50:
        stTiming.enTimeType = E_XC_TGEN_HDMI_1080_P_50;
        u16FrameRate = 50;
        break;
    case SCA_OUTPUT_1080_P_60:
        stTiming.enTimeType = E_XC_TGEN_HDMI_1080_P_60;
        u16FrameRate = 60;
        break;
    case SCA_OUTPUT_480_P_30:
        stTiming.enTimeType = E_XC_TGEN_HDMI_480_P_60;
        u16FrameRate = 30;
        break;
    case SCA_OUTPUT_720_P_30:
        stTiming.enTimeType = E_XC_TGEN_HDMI_720_P_60;
        u16FrameRate = 30;
        break;
    default:
        bRet = FALSE;
        break;
    }

    if(bRet)
    {
        SCA_DBG(SCA_DBG_LV_PROGWIN, "IOCTL_SCATL_Set_Output_Timing: %d  !!! \r\n", stTiming.enTimeType);
        MApi_XC_Set_TGen(stTiming, eWindow);

        if(eWindow == SC2_MAIN_WINDOW || eWindow == SC2_SUB_WINDOW)
        {
            MS_U16 u16Htt, u16Vtt;

            MApi_XC_Get_Htt_Vtt(&u16Htt, &u16Vtt, eWindow);
            SCA_DBG(SCA_DBG_LV_PROGWIN, "IOCTL_SCATL_Set_Output_Timing: Htt:%d, Vtt:%d  FrameRate:%d!!! \r\n", u16Htt, u16Vtt, u16FrameRate);
            MApi_XC_Set_SC2_ODCLK(u16Htt, u16Vtt, u16FrameRate);

        }
    }

    return bRet;
}

void saveVGACalibrationValue(MS_ADC_SETTING *stMsAdcSettingOut)
{
    struct file *fd;
    size_t t;
    mm_segment_t old_fs;
    loff_t off;
    old_fs = get_fs();
    set_fs(KERNEL_DS);

    fd = filp_open("/dev/block/mmcblk0p1", O_RDWR, 0);
    if(fd)
    {
        off = 0x890;
        t = vfs_write(fd, (char *)(stMsAdcSettingOut), sizeof(MS_ADC_SETTING), &off);
        printk(KERN_ERR "write to/dev/block/mmcblk0p1 %d bytes \n", t);
        vfs_fsync(fd, 0);
        filp_close(fd, NULL);
    }
    else
        printk(KERN_ERR "open /dev/block/mmcblk0p1 failed\n");

    set_fs(old_fs);
}

void saveYPBPRCalibrationValue(MS_ADC_SETTING *stMsAdcSettingOut)
{
    struct file *fd;
    size_t t;
    mm_segment_t old_fs;
    loff_t off;
    old_fs = get_fs();
    set_fs(KERNEL_DS);

    fd = filp_open("/dev/block/mmcblk0p1", O_RDWR, 0);
    if(fd)
    {
        off = 0x880;
        t = vfs_write(fd, (char *)(stMsAdcSettingOut), sizeof(MS_ADC_SETTING), &off);
        printk(KERN_ERR "write to/dev/block/mmcblk0p1 %d bytes \n", t);
        vfs_fsync(fd, 0);
        filp_close(fd, NULL);
    }
    else
        printk(KERN_ERR "open /dev/block/mmcblk0p1 failed\n");

    set_fs(old_fs);
}


MS_BOOL SCA_Calibration(SCA_SRC_TYPE *ptInputSource, PSCA_ADC_CALIB_INFO pstDrvAdcSetting)
{
	MS_BOOL bRet = FALSE;
	INPUT_SOURCE_TYPE_t enInputSourceType = INPUT_SOURCE_NONE;
	MS_ADC_SETTING		stMsAdcSettingOut;

    memset(&stMsAdcSettingOut, 0, sizeof(MS_ADC_SETTING));
	enInputSourceType = _Trans_To_Drv_Src(*ptInputSource);

	if ( IsSrcTypeYPbPr(enInputSourceType) )
	{
        bRet = (MS_BOOL) MApp_YPbPr_Setting_Auto(E_XC_EXTERNAL_CALIBRATION, MAIN_WINDOW, &stMsAdcSettingOut);
		saveYPBPRCalibrationValue(&stMsAdcSettingOut);
	}
	else if ( IsSrcTypeVga(enInputSourceType) )
	{
		bRet = (MS_BOOL) MApp_RGB_Setting_Auto(E_XC_EXTERNAL_CALIBRATION, MAIN_WINDOW, &stMsAdcSettingOut);
		saveVGACalibrationValue(&stMsAdcSettingOut);
	}
	else
	{
		SCA_DBG(SCA_DBG_LV_COLOR, "[IOCTL_SCATRL_CALIB] Only YPbPr input calibration supported, %d !!! \r\n", enInputSourceType);
	}

	if (bRet)
	{
		bRet = _Trans_To_Drv_Adc_Calib( (const MS_ADC_SETTING*)&stMsAdcSettingOut, pstDrvAdcSetting);
	}


	return bRet;
}

MS_BOOL SCA_Load_ADC_Setting(const PSCA_ADC_CALIB_LOAD pstAdcCalibLoad)
{
	MS_BOOL bRet = FALSE;
	MS_ADC_SETTING		stMsAdcSettingOut;
	E_ADC_SET_INDEX		eAdcSetIdx        = ADC_SET_NUMS;

    memset(&stMsAdcSettingOut, 0, sizeof(MS_ADC_SETTING));
	eAdcSetIdx = _Trans_To_Local_ADC_Set_Idx(pstAdcCalibLoad->eSourceInput, pstAdcCalibLoad->bIsHDMode);

	_Trans_To_Local_Adc_Calib( &(pstAdcCalibLoad->stAdcCalibInfo), &stMsAdcSettingOut);

	bRet = MApp_SetADCSetting(eAdcSetIdx, (const MS_ADC_SETTING *)&stMsAdcSettingOut);

	return bRet;
}
MS_BOOL SCA_RW_Register(PSCA_RW_REGISTER_CONFIG pConfig)
{
    MS_BOOL bret = TRUE;
    MS_U8 u8val;

    switch(pConfig->enType)
    {
    case SCA_REG_R_BYTE:
        pConfig->u16value = ((MS_U16)MApi_XC_ReadByte(pConfig->u32addr)) & 0x00FF;
        break;

    case SCA_REG_XC_R2BYTEMSK:
        pConfig->u16value = MApi_XC_R2BYTEMSK(pConfig->u32addr, pConfig->u16mask);
        break;

    case SCA_REG_W_BYTE:
        u8val = (MS_U8)(pConfig->u16value & 0xFF);
        MApi_XC_WriteByte(pConfig->u32addr, u8val);
        break;

    case SCA_REG_XC_W2BYTEMSK:
        MApi_XC_W2BYTEMSK(pConfig->u32addr, pConfig->u16value, pConfig->u16mask);
        break;

    default:
        bret = FALSE;
    }

    return bret;
}



MS_BOOL SCA_Get_ACE_Info(PSCA_ACE_INFO_CONFIG pConfig)
{
    SCALER_WIN eWindow;

    eWindow = _Trans_To_Drv_Dest(pConfig->enDestType);
    if(eWindow == MAX_WINDOW)
    {
        return FALSE;
    }

    pConfig->u16version     = MApi_XC_ACE_GetACEInfo(eWindow, E_XC_ACE_INFO_TYPE_VERSION);
    pConfig->u16contrast    = MApi_XC_ACE_GetACEInfo(eWindow, E_XC_ACE_INFO_TYPE_CONTRAST);
    pConfig->u16r_gain      = MApi_XC_ACE_GetACEInfo(eWindow, E_XC_ACE_INFO_TYPE_R_GAIN);
    pConfig->u16g_gain      = MApi_XC_ACE_GetACEInfo(eWindow, E_XC_ACE_INFO_TYPE_G_GAIN);
    pConfig->u16b_gain      = MApi_XC_ACE_GetACEInfo(eWindow, E_XC_ACE_INFO_TYPE_B_GAIN);
    pConfig->u16saturation  = MApi_XC_ACE_GetACEInfo(eWindow, E_XC_ACE_INFO_TYPE_SATURATION);
    pConfig->u16hue         = MApi_XC_ACE_GetACEInfo(eWindow, E_XC_ACE_INFO_TYPE_HUE);

    pConfig->u16color_correct_xy_r = MApi_XC_ACE_GetACEInfo(eWindow, E_XC_ACE_INFO_TYPE_COLOR_CORRECTION_XY_R);
    pConfig->u16color_correct_xy_g = MApi_XC_ACE_GetACEInfo(eWindow, E_XC_ACE_INFO_TYPE_COLOR_CORRECTION_XY_G);
    pConfig->u16color_correct_xy_b = MApi_XC_ACE_GetACEInfo(eWindow, E_XC_ACE_INFO_TYPE_COLOR_CORRECTION_XY_B);

    pConfig->u16color_correct_offset_r = MApi_XC_ACE_GetACEInfo(eWindow, E_XC_ACE_INFO_TYPE_COLOR_CORRECTION_OFFSET_R);
    pConfig->u16color_correct_offset_g = MApi_XC_ACE_GetACEInfo(eWindow, E_XC_ACE_INFO_TYPE_COLOR_CORRECTION_OFFSET_G);
    pConfig->u16color_correct_offset_b = MApi_XC_ACE_GetACEInfo(eWindow, E_XC_ACE_INFO_TYPE_COLOR_CORRECTION_OFFSET_B);

    return TRUE;

}

static MS_U8 gu8DLC_CmdBuf[1024] = {0};
static MS_U8 gu8DLC_DataBuf[1024] = {0};
static MS_U16 u16DLC_Data_idx = 0;

void SCA_DLC_Print(MS_U8 u8val)
{
    //SCA_DBG(SCA_DBG_LV_0, "DLC:%x \n", u8val);
    gu8DLC_DataBuf[u16DLC_Data_idx++] = u8val;
}


MS_BOOL SCA_Get_DLC_INFO(PSCA_DLC_INFO_CONFIG pConfig)
{
    tDLC_CONTROL_PARAMS stDLCParm;

    stDLCParm.pCmdBuff = gu8DLC_CmdBuf;
    stDLCParm.u16PNL_Height = MApi_GetProjPaneltype()->m_wPanelHeight;
    stDLCParm.u16PNL_Width = MApi_GetProjPaneltype()->m_wPanelWidth;
    stDLCParm.fnDLC_Putchar = SCA_DLC_Print;
    u16DLC_Data_idx = 0;
    MApi_XC_DLC_DecodeExtCmd(&stDLCParm);

    pConfig->u16DataLen = u16DLC_Data_idx;

    return TRUE;
}

MS_BOOL SCA_Get_Disp_Path_Config(PSCA_DISP_PATH_CONFIG pCfg)
{
    MS_BOOL bret = TRUE;

	SCA_WAIT_MUTEX(_SCA_Cnnt_Mutex);

    if(1)//pCfg->enDestType == SCA_DEST_MAIN)
    {
        pCfg->enSrcType = g_stDispPathCfg[pCfg->enDestType].enSrcType;
        pCfg->enPriType = g_stDispPathCfg[pCfg->enDestType].enPriType;
        pCfg->bChanged = g_stDispPathCfg[pCfg->enDestType].bChanged;
        pCfg->bDisConnect = g_stDispPathCfg[pCfg->enDestType].bDisConnect;
        pCfg->dwUID = g_stDispPathCfg[pCfg->enDestType].dwUID;
        bret = TRUE;
    }
    else
    {
        bret = FALSE;
    }

	SCA_RELEASE_MUTEX(_SCA_Cnnt_Mutex);

    return bret;
}

MS_BOOL SCA_Set_Mirror_Config(PSCA_MIRROR_CONFIG pCfg)
{
    MS_BOOL bret = TRUE;

    if(pCfg->enDestType == SCA_DEST_MAIN)
    {
        if(pCfg->bEn)
        {
            MApi_XC_EnableMirrorMode2(MIRROR_H_ONLY, MAIN_WINDOW);
        }
        else
        {
            MApi_XC_EnableMirrorMode2(MIRROR_NORMAL, MAIN_WINDOW);
        }
    }
    else
    {
        bret = FALSE;
    }



    return bret;
}
#ifdef __BOOT_PNL__//paul_test
//MApi_XC_Write2ByteMask(0x1202FFReg,  0x200val, 0x200mask);
//SC_W2BYTEMSK,MApi_XC_W2BYTEMSK
MS_BOOL SCA_Set_PQ_Bin_IHC(PSCA_PQ_IHC_CFG pCfg)
{
    MS_BOOL bret = TRUE;
    //en
    printk("     [SCA] set IHC \r\n");
    printk("     [SCA] en:%d \r\n",pCfg->ihc_en);
    printk("     [SCA] R value:%d \r\n",pCfg->ihc_gain_R.u16value);
    if (pCfg->ihc_en)
    {
        MApi_XC_W2BYTEMSK((((MS_U16)(0x18) << 8) | (MS_U16)((0x60)*2)),  0x80, 0x80);
        //RGB...7
        MApi_XC_W2BYTEMSK((((MS_U16)(0x18) << 8) | (MS_U16)((0x61)*2)),pCfg->ihc_gain_R.u16value+pCfg->ihc_gain_R.bnegative*0x40, 0x7F);//R
        MApi_XC_W2BYTEMSK((((MS_U16)(0x18) << 8) | (MS_U16)((0x61)*2)),(pCfg->ihc_gain_G.u16value+pCfg->ihc_gain_G.bnegative*0x40)<<8, 0x7F00);
        MApi_XC_W2BYTEMSK((((MS_U16)(0x18) << 8) | (MS_U16)((0x62)*2)),pCfg->ihc_gain_B.u16value+pCfg->ihc_gain_B.bnegative*0x40, 0x7F);
        MApi_XC_W2BYTEMSK((((MS_U16)(0x18) << 8) | (MS_U16)((0x62)*2)),(pCfg->ihc_gain_C.u16value+pCfg->ihc_gain_C.bnegative*0x40)<<8, 0x7F00);
        MApi_XC_W2BYTEMSK((((MS_U16)(0x18) << 8) | (MS_U16)((0x63)*2)),pCfg->ihc_gain_M.u16value+pCfg->ihc_gain_M.bnegative*0x40, 0x7F);
        MApi_XC_W2BYTEMSK((((MS_U16)(0x18) << 8) | (MS_U16)((0x63)*2)),(pCfg->ihc_gain_Y.u16value+pCfg->ihc_gain_Y.bnegative*0x40)<<8, 0x7F00);
    }
    else
    {
        MApi_XC_W2BYTEMSK((((MS_U16)(0x18) << 8) | (MS_U16)((0x60)*2)),  0x00, 0x80);
    }
    return bret;

}
MS_BOOL SCA_Set_PQ_Bin_ICC(PSCA_PQ_ICC_CFG pCfg)
{
    MS_BOOL bret = TRUE;
    //en
    printk("     [SCA] set ICC \r\n");
    printk("     [SCA] en:%d \r\n",pCfg->icc_en);
    printk("     [SCA] R value:%d \r\n",pCfg->icc_gain_R.u16value);
    if (pCfg->icc_en)
    {
        MApi_XC_W2BYTEMSK((((MS_U16)(0x18) << 8) | (MS_U16)((0x30)*2)),  0xC0, 0xC0);
        MApi_XC_W2BYTEMSK((((MS_U16)(0x18) << 8) | (MS_U16)((0x36)*2)),  0x06, 0x1F);
        MApi_XC_W2BYTEMSK((((MS_U16)(0x18) << 8) | (MS_U16)((0x36)*2)),  0x6000, 0x7F00);
        //RGB...8
        MApi_XC_W2BYTEMSK((((MS_U16)(0x18) << 8) | (MS_U16)((0x31)*2)), 0x00+pCfg->icc_gain_R.u16value, 0x0F);//R
        MApi_XC_W2BYTEMSK((((MS_U16)(0x18) << 8) | (MS_U16)((0x31)*2)), (0x00+pCfg->icc_gain_G.u16value)<<8, 0x0F00);
        MApi_XC_W2BYTEMSK((((MS_U16)(0x18) << 8) | (MS_U16)((0x32)*2)), 0x00+pCfg->icc_gain_B.u16value, 0x0F);
        MApi_XC_W2BYTEMSK((((MS_U16)(0x18) << 8) | (MS_U16)((0x32)*2)), (0x00+pCfg->icc_gain_C.u16value)<<8, 0x0F00);
        MApi_XC_W2BYTEMSK((((MS_U16)(0x18) << 8) | (MS_U16)((0x33)*2)), 0x00+pCfg->icc_gain_M.u16value, 0x0F);
        MApi_XC_W2BYTEMSK((((MS_U16)(0x18) << 8) | (MS_U16)((0x33)*2)), (0x00+pCfg->icc_gain_Y.u16value)<<8, 0x0F00);
        MApi_XC_W2BYTEMSK((((MS_U16)(0x18) << 8) | (MS_U16)((0x34)*2)), 0x00+pCfg->icc_gain_F.u16value, 0x0F);
        MApi_XC_W2BYTEMSK((((MS_U16)(0x18) << 8) | (MS_U16)((0x37)*2)), 0x00+pCfg->icc_gain_NC.u16value, 0x0F);

        MApi_XC_W2BYTEMSK((((MS_U16)(0x18) << 8) | (MS_U16)((0x35)*2)), 0x00+pCfg->icc_gain_R.bnegative*0x02, 0x02);
        MApi_XC_W2BYTEMSK((((MS_U16)(0x18) << 8) | (MS_U16)((0x35)*2)), 0x00+pCfg->icc_gain_G.bnegative*0x04, 0x04);
        MApi_XC_W2BYTEMSK((((MS_U16)(0x18) << 8) | (MS_U16)((0x35)*2)), 0x00+pCfg->icc_gain_B.bnegative*0x08, 0x08);
        MApi_XC_W2BYTEMSK((((MS_U16)(0x18) << 8) | (MS_U16)((0x35)*2)), 0x00+pCfg->icc_gain_C.bnegative*0x10, 0x10);
        MApi_XC_W2BYTEMSK((((MS_U16)(0x18) << 8) | (MS_U16)((0x35)*2)), 0x00+pCfg->icc_gain_M.bnegative*0x20, 0x20);
        MApi_XC_W2BYTEMSK((((MS_U16)(0x18) << 8) | (MS_U16)((0x35)*2)), 0x00+pCfg->icc_gain_Y.bnegative*0x40, 0x40);
        MApi_XC_W2BYTEMSK((((MS_U16)(0x18) << 8) | (MS_U16)((0x35)*2)), 0x00+pCfg->icc_gain_F.bnegative*0x80, 0x80);
        MApi_XC_W2BYTEMSK((((MS_U16)(0x18) << 8) | (MS_U16)((0x35)*2)), 0x00+pCfg->icc_gain_NC.bnegative*0x80, 0x80);

    }
    else
    {
        MApi_XC_W2BYTEMSK((((MS_U16)(0x18) << 8) | (MS_U16)((0x30)*2)),  0x00, 0xC0);
        MApi_XC_W2BYTEMSK((((MS_U16)(0x18) << 8) | (MS_U16)((0x36)*2)),  0x00, 0x1F);
        MApi_XC_W2BYTEMSK((((MS_U16)(0x18) << 8) | (MS_U16)((0x36)*2)),  0x00<<8, 0x7F00);
    }
    return bret;

}
MS_BOOL SCA_Set_PQ_Bin_IBC(PSCA_PQ_IBC_CFG pCfg)
{
    MS_BOOL bret = TRUE;
    //en
    printk("     [SCA] set IBC \r\n");
    printk("     [SCA] en:%d \r\n",pCfg->ibc_en);
    printk("     [SCA] R value:%d \r\n",pCfg->ibc_gain_R.u16value);
    if (pCfg->ibc_en)
    {
        MApi_XC_W2BYTEMSK((((MS_U16)(0x18) << 8) | (MS_U16)((0x40)*2)),  0xA0, 0xAF);
        //RGB...7
        MApi_XC_W2BYTEMSK((((MS_U16)(0x18) << 8) | (MS_U16)((0x41)*2)), 0x00+pCfg->ibc_gain_R.u16value, 0x3F);//R
        MApi_XC_W2BYTEMSK((((MS_U16)(0x18) << 8) | (MS_U16)((0x41)*2)), (0x00+pCfg->ibc_gain_G.u16value)<<8, 0x3F00);
        MApi_XC_W2BYTEMSK((((MS_U16)(0x18) << 8) | (MS_U16)((0x42)*2)), 0x00+pCfg->ibc_gain_B.u16value, 0x3F);
        MApi_XC_W2BYTEMSK((((MS_U16)(0x18) << 8) | (MS_U16)((0x42)*2)), (0x00+pCfg->ibc_gain_C.u16value)<<8, 0x3F00);
        MApi_XC_W2BYTEMSK((((MS_U16)(0x18) << 8) | (MS_U16)((0x43)*2)), 0x00+pCfg->ibc_gain_M.u16value, 0x3F);
        MApi_XC_W2BYTEMSK((((MS_U16)(0x18) << 8) | (MS_U16)((0x43)*2)), (0x00+pCfg->ibc_gain_Y.u16value)<<8, 0x3F00);
        MApi_XC_W2BYTEMSK((((MS_U16)(0x18) << 8) | (MS_U16)((0x44)*2)), 0x00+pCfg->ibc_gain_F.u16value, 0x3F);
    }
    else
    {
        MApi_XC_W2BYTEMSK((((MS_U16)(0x18) << 8) | (MS_U16)((0x40)*2)),  0x00, 0xAF);
    }
    return bret;
}

#endif


MS_BOOL SCA_Set_PQ_Bin(PSCA_PQ_BIN_CONFIG pCfg)
{
    int i;
    MS_BOOL bret = TRUE;
    SCA_PQ_BIN_ID_TYPE enPQID;

    SCA_WAIT_MUTEX(_SCA_Cnnt_Mutex);
    for(i=0;i<SCA_PQ_BIN_ID_NUM; i++)
    {
        SCA_DBG(SCA_DBG_LV_PQ_BIN,"i=%d, ID:%d, Len:%d, Addr:%x \n",
            i, pCfg->stPQBinInfo[i].enPQBinIDType, (int)pCfg->stPQBinInfo[i].u32PQBin_Size, (int)pCfg->stPQBinInfo[i].u32PQBin_Addr);

        enPQID = pCfg->stPQBinInfo[i].enPQBinIDType;
        if(pCfg->stPQBinInfo[i].u32PQBin_Addr && pCfg->stPQBinInfo[i].u32PQBin_Size)
        {
            if(stPQBinData[enPQID].pBuff == NULL)
            {
                stPQBinData[enPQID].pBuff = kmalloc(pCfg->stPQBinInfo[i].u32PQBin_Size, GFP_KERNEL);
            }

            stPQBinData[enPQID].enPQBinIDType = enPQID;
            stPQBinData[enPQID].u32Len = pCfg->stPQBinInfo[i].u32PQBin_Size;

            if(copy_from_user(stPQBinData[enPQID].pBuff, (SCA_PQ_BIN_CONFIG __user *)pCfg->stPQBinInfo[i].u32PQBin_Addr, pCfg->stPQBinInfo[i].u32PQBin_Size))
            {
                SCA_DBG(SCA_DBG_LV_PQ_BIN, "copy Bin Fail: %d\n", enPQID);
                bret = FALSE;
                break;
            }
        }
    }

    if(bret)
    {
        MApi_XC_Sys_Init_PQ((MApi_GetProjPaneltype()->m_wPanelWidth), (MApi_GetProjPaneltype()->m_wPanelVTotal));

#if PQ_ENABLE_COLOR
#if defined(SCA_ENABLE_DLC)
        MApi_XC_Sys_Init_DLC();
        MApi_XC_DLC_CGC_ResetCGain();

        MApi_XC_DLC_CGC_CheckCGainInPQCom();
#endif

        // Gamma Table
        MApi_XC_GetGamma_From_PQ(MAIN_WINDOW, tSC0_NormalGammaR, tSC0_NormalGammaG, tSC0_NormalGammaB);
        MApi_PNL_SetGammaTbl(E_APIPNL_GAMMA_12BIT, tSC0_AllGammaTab, E_APIPNL_GAMMA_8BIT_MAPPING);

        // 3x3 matrix
        MApi_XC_Get_ColorCorrectMatix_From_PQ(MAIN_WINDOW,  (MS_U16 *)S16SC0_ColorCorrectMatrix);
        MApi_XC_ACE_ColorCorrectionTable(MAIN_WINDOW, (MS_S16 *)S16SC0_ColorCorrectMatrix);
        MApi_XC_ACE_SetPCsRGBTable(MAIN_WINDOW);

        MApi_XC_Get_ColorCorrectMatix_From_PQ(SC1_MAIN_WINDOW,  (MS_U16 *)S16SC1_ColorCorrectMatrix);
        MApi_XC_ACE_ColorCorrectionTable(SC1_MAIN_WINDOW, (MS_S16 *)S16SC1_ColorCorrectMatrix);
        MApi_XC_ACE_SetPCsRGBTable(SC1_MAIN_WINDOW);

        MApi_XC_Get_ColorCorrectMatix_From_PQ(SC2_MAIN_WINDOW,  (MS_U16 *)S16SC2_ColorCorrectMatrix);
        MApi_XC_ACE_ColorCorrectionTable(SC2_MAIN_WINDOW, (MS_S16 *)S16SC2_ColorCorrectMatrix);
        MApi_XC_ACE_SetPCsRGBTable(SC2_MAIN_WINDOW);
#endif


#if ENABLE_2_GOP_BLENDING_PATCH
        MApi_XC_SetMemoryWriteRequest(FALSE, MAIN_WINDOW);
        MApi_XC_DisableInputSource(FALSE, MAIN_WINDOW);
        MApi_XC_SetMemoryWriteRequest(FALSE, SC1_MAIN_WINDOW);
        MApi_XC_DisableInputSource(FALSE, SC1_MAIN_WINDOW);

        MApi_XC_SetMemoryWriteRequest(FALSE, SC2_MAIN_WINDOW);
        MApi_XC_DisableInputSource(FALSE, SC2_MAIN_WINDOW);
        MApi_XC_SetMemoryWriteRequest(FALSE, SC2_SUB_WINDOW);
        MApi_XC_DisableInputSource(FALSE, SC2_SUB_WINDOW);
        MApi_XC_SetMemoryWriteRequest(FALSE, SUB_WINDOW);
        MApi_XC_DisableInputSource(FALSE, SUB_WINDOW);

        MApi_XC_GenerateBlackVideo( ENABLE, MAIN_WINDOW );
        MApi_XC_GenerateBlackVideo( ENABLE, SUB_WINDOW );
        MApi_XC_GenerateBlackVideo( ENABLE, SC1_MAIN_WINDOW );
        MApi_XC_GenerateBlackVideo( ENABLE, SC2_MAIN_WINDOW );
        MApi_XC_GenerateBlackVideo( ENABLE, SC2_SUB_WINDOW );
#endif

    }
    SCA_RELEASE_MUTEX(_SCA_Cnnt_Mutex);

    return bret;
}

MS_BOOL SCA_Set_Dip_Config(PSCA_DIP_CONFIG pCfg)
{
    MS_BOOL bret = TRUE;
    XC_DIP_CONFIG_t stDipConfig;

    if(pCfg->bTriggle == FALSE)
    {

        if(pCfg->enTrigMode == SCA_DIP_TRIGGER_LOOP)
        {
            MApi_XC_Set_Dip_Trig(FALSE);
        }
        else
        {
            MApi_XC_DIP_Set_Dwin_Wt_Once(FALSE);
        }
    }
    else
    {

        memset(&stDipConfig, 0, sizeof(XC_DIP_CONFIG_t));

        // clip window
        stDipConfig.bClipEn = pCfg->bClipEn;
        stDipConfig.stClipWin.x = pCfg->stClipWin.x < 1 ? 1 : pCfg->stClipWin.x;
        stDipConfig.stClipWin.y = pCfg->stClipWin.y < 1 ? 1 : pCfg->stClipWin.y;
        stDipConfig.stClipWin.width = pCfg->stClipWin.width > pCfg->u16Width ? pCfg->u16Width : pCfg->stClipWin.width;
        stDipConfig.stClipWin.height = pCfg->stClipWin.height > pCfg->u16Height ? pCfg->u16Height : pCfg->stClipWin.height;


        // pitch
        stDipConfig.u16Pitch = pCfg->u16Width;

        // width/height
        stDipConfig.u16Width = pCfg->u16Width;
        stDipConfig.u16Height = pCfg->u16Height;

        // frame number
        stDipConfig.u8FrameNum = pCfg->u8FrameNum;

        // base address & offset
        stDipConfig.u32BaseAddr = pCfg->u32BuffAddress;
        stDipConfig.u32BaseSize = pCfg->u32BuffSize;

        // C base address & offset
        if(pCfg->enDestFmtType == SCA_DIP_DEST_FMT_YC420_MVOP ||
           pCfg->enDestFmtType == SCA_DIP_DEST_FMT_YC420_MFE )
        {
            stDipConfig.u32C_BaseAddr = pCfg->u32C_BuffAddress;
            stDipConfig.u32C_BaseSize = pCfg->u32C_BuffSize;
            stDipConfig.bSrc422 = TRUE;
            stDipConfig.b444To422 = TRUE;
        }
        else if(SCA_DIP_DEST_FMT_YC422==pCfg->enDestFmtType)
        {
            stDipConfig.bSrc422   = TRUE;
            stDipConfig.b444To422 = TRUE;
        }
        else
        {
            stDipConfig.bSrc422 = FALSE;
            stDipConfig.b444To422 = FALSE;
        }

        stDipConfig.eDstFMT = pCfg->enDestFmtType == SCA_DIP_DEST_FMT_YC422      ? DIP_DST_FMT_YC422 :
                              pCfg->enDestFmtType == SCA_DIP_DEST_FMT_ARGB8888   ? DIP_DST_FMT_ARGB8888:
                              pCfg->enDestFmtType == SCA_DIP_DEST_FMT_RGB565     ? DIP_DST_FMT_RGB565:
                              pCfg->enDestFmtType == SCA_DIP_DEST_FMT_YC420_MVOP ? DIP_DST_FMT_YC420_MVOP :
                                                                                   DIP_DST_FMT_YC420_MFE;
        stDipConfig.eSrcFMT = DIP_SRC_FMT_OP;
        stDipConfig.bSrcYUV = TRUE;
        stDipConfig.bWOnce = pCfg->enTrigMode == SCA_DIP_TRIGGER_ONCE ? TRUE : FALSE;

        SCA_DBG(SCA_DBG_LV_PROGWIN, "SetDip: clip(%d, %d, %d, %d), w=%d, h:%d, FBnum:%d, Base:%x, Size:%x, CBase:%x, CSize:%x\n",
            stDipConfig.stClipWin.x, stDipConfig.stClipWin.y, stDipConfig.stClipWin.width, stDipConfig.stClipWin.height,
            stDipConfig.u16Width, stDipConfig.u16Height, stDipConfig.u8FrameNum,
            (int)stDipConfig.u32BaseAddr, (int)stDipConfig.u32BaseSize,
            (int)stDipConfig.u32C_BaseAddr, (int)stDipConfig.u32C_BaseSize);

        if(MApi_XC_Set_Dip_Config(stDipConfig) == E_APIXC_RET_FAIL)
        {
            bret = FALSE;
        }
        else
        {
            if(pCfg->enTrigMode == SCA_DIP_TRIGGER_LOOP)
            {
                MApi_XC_Set_Dip_Trig(TRUE);
            }
            else
            {
                SCA_DBG(SCA_DBG_LV_0, "Wonce On \n");
                MApi_XC_DIP_Set_Dwin_Wt_Once(TRUE);
                MApi_XC_DIP_Set_Dwin_Wt_Once_Trig(TRUE);
            }
        }
    }

    return bret;
}

MS_BOOL SCA_Set_Dip_WOnce_Base(PSCA_DIP_WONCE_BASE_CONFIG pCfg)
{
    MS_BOOL bret = TRUE;

    if(pCfg->bTrig)
    {
        if(MApi_XC_DIP_Set_Base(pCfg->u32BuffAddress, pCfg->u32C_BuffAddress))
        {
            MApi_XC_DIP_Set_Dwin_Wt_Once_Trig(TRUE);
        }
        else
        {
            bret = FALSE;
        }
    }

    return bret;
}

MS_BOOL SCA_Set_HDMITX_Config(PSCA_HDMITX_CONFIG pCfg)
{
    E_XC_HDMITX_OUTPUT_MODE enOutputMode;
    E_XC_HDMITX_OUTPUT_TIMING_TYPE enOutputTiming;


    switch(pCfg->enHDMITx_Mode)
    {
    case SCA_HDMITX_OUT_HDMI:
        enOutputMode = E_XC_HDMITX_OUTPUT_HDMI;
        break;

    case SCA_HDMITX_OUT_HDMI_HDCP:
        enOutputMode = E_XC_HDMITX_OUTPUT_HDMI_HDCP;
        break;

    default:
        SCA_DBG(SCA_DBG_LV_0, "UNKNOWN SCA_HDMITX_MODE_TYPE \n");
        return FALSE;
    }

    switch(pCfg->enHDMITx_OutputTiming)
    {
    case SCA_HDMITX_480_I_60:
        enOutputTiming =  E_XC_HDMITX_480_60I;
        g_enTGenTiming =  E_XC_TGEN_HDMI_480_I_60;
        break;

    case SCA_HDMITX_480_P_60:
        enOutputTiming =  E_XC_HDMITX_480_60P;
        g_enTGenTiming =  E_XC_TGEN_HDMI_480_P_60;
        break;

    case SCA_HDMITX_576_I_50:
        enOutputTiming =  E_XC_HDMITX_576_50I;
        g_enTGenTiming =  E_XC_TGEN_HDMI_576_I_50;
        break;

    case SCA_HDMITX_576_P_50:
        enOutputTiming =  E_XC_HDMITX_576_50P;
        g_enTGenTiming =  E_XC_TGEN_HDMI_576_P_50;
        break;

    case SCA_HDMITX_720_P_50:
        enOutputTiming =  E_XC_HDMITX_720_50P;
        g_enTGenTiming =  E_XC_TGEN_HDMI_720_P_50;
        break;

    case SCA_HDMITX_720_P_60:
        enOutputTiming =  E_XC_HDMITX_720_60P;
        g_enTGenTiming =  E_XC_TGEN_HDMI_720_P_60;
        break;

    case SCA_HDMITX_1080_I_60:
        enOutputTiming =  E_XC_HDMITX_1080_60I;
        g_enTGenTiming =  E_XC_TGEN_HDMI_1080_I_60;
        break;

    case SCA_HDMITX_1080_P_60:
        enOutputTiming =  E_XC_HDMITX_1080_60P;
        g_enTGenTiming =  E_XC_TGEN_HDMI_1080_P_60;
        break;

    case SCA_HDMITX_1080_I_50:
        enOutputTiming =  E_XC_HDMITX_1080_50I;
        g_enTGenTiming =  E_XC_TGEN_HDMI_1080_I_50;
        break;

    case SCA_HDMITX_1080_P_50:
        enOutputTiming =  E_XC_HDMITX_1080_50P;
        g_enTGenTiming =  E_XC_TGEN_HDMI_1080_P_50;
        break;

    case SCA_HDMITX_1080_P_30:
        enOutputTiming =  E_XC_HDMITX_1080_30P;
        g_enTGenTiming =  E_XC_TGEN_HDMI_1080_P_30;
        break;

    case SCA_HDMITX_1080_P_25:
        enOutputTiming =  E_XC_HDMITX_1080_25P;
        g_enTGenTiming =  E_XC_TGEN_HDMI_1080_P_25;
        break;

    case SCA_HDMITX_1080_P_24:
        enOutputTiming =  E_XC_HDMITX_1080_24P;
        g_enTGenTiming =  E_XC_TGEN_HDMI_1080_P_24;
        break;

    default:
        SCA_DBG(SCA_DBG_LV_0, "UNKNOWN SCA_HDMITX_OUTPUT_TIMINE_TYPE \n");
        return FALSE;
    }

    SCA_DBG(SCA_DBG_LV_HDMITX, "HDMI:En:%d, OutputMode:%d, Timing:%d Color:%d \n",
                pCfg->bEn, enOutputMode, enOutputTiming, pCfg->enHDMITx_ColorType);

    SCA_DBG(SCA_DBG_LV_0, "SET_HDMITX:%d \n", g_enTGenTiming);

    MDrv_SC_iDAC_SetEnabled(FALSE);
    if(pCfg->enHDMITx_ColorType == SCA_HDMITX_OUTPUT_COLOR_RGB)
    {
        MApi_HDMITx_SetColorFormat(HDMITX_VIDEO_COLOR_RGB444, HDMITX_VIDEO_COLOR_RGB444);
        MApi_XC_Set_OutputColor(SC1_MAIN_WINDOW, E_XC_OUTPUT_COLOR_RGB);
        MApi_XC_SetFrameColor(0x00000, SC1_MAIN_WINDOW);
        MApi_XC_SetFrameColor_En(TRUE, SC1_MAIN_WINDOW);

    }
    else
    {
        MApi_HDMITx_SetColorFormat(HDMITX_VIDEO_COLOR_YUV444, HDMITX_VIDEO_COLOR_YUV444);
        MApi_XC_Set_OutputColor(SC1_MAIN_WINDOW, E_XC_OUTPUT_COLOR_YUV);
        MApi_XC_SetFrameColor(0x800080, SC1_MAIN_WINDOW);
        MApi_XC_SetFrameColor_En(TRUE, SC1_MAIN_WINDOW);
    }


    MApi_Config_HDMITx(pCfg->bEn, enOutputMode, enOutputTiming);

    memcpy(&g_HDMI_Cfg, pCfg, sizeof(SCA_HDMITX_CONFIG));
    return TRUE;
}

MS_BOOL SCA_Set_Freeze_Config(PSCA_FREEZE_CONFIG pCfg)
{
    SCALER_WIN eWindow  = _Trans_To_Drv_Dest(pCfg->enDestType);

    if(eWindow == MAX_WINDOW)
    {
        return FALSE;
    }

    SCA_DBG(SCA_DBG_LV_0, "SCA_Set_Freeze_Config: DEST:%d  OnOFF%d \n", pCfg->bEn, eWindow);

    MApi_XC_FreezeImg(pCfg->bEn, eWindow);

    return TRUE;

}

#if defined(CONFIG_MS_MST701)
MS_BOOL SCA_Init_MST701(PSCA_MST701_CONFIG pCfg)
{
    MS_U8 *pBinData;

    if((!pCfg->bFroceInit) && g_bMST701Inited)
    {
        SCA_DBG(SCA_DBG_LV_0, "IMST701 Inited before!!!");
        return TRUE;
    }

    if(pCfg->u32MST701Bin_Bytes > 4*1024)
    {
        printk("SCA_Init_MST701 bin data too large %ld \n", pCfg->u32MST701Bin_Bytes);
        return FALSE;
    }

    pBinData = kmalloc(pCfg->u32MST701Bin_Bytes, GFP_KERNEL);
    if(NULL == pBinData)
    {
        printk("SCA_Init_MST701 malloc MEM fail \n");
        return FALSE;
    }
    if(copy_from_user(pBinData, (PSCA_MST701_CONFIG __user *)pCfg->u32MST701Bin_Addr, pCfg->u32MST701Bin_Bytes))
    {
        printk("SCA_Init_MST701 copy Bin fail \n");
        return FALSE;
    }

    if(!MApi_Init_MST701(pCfg->u32MST701Bin_Bytes, pBinData))
    {
        printk("SCA_Init_MST701 fail \n");
        return FALSE;
    }

    kfree(pBinData);
    g_bMST701Inited = TRUE;
    return TRUE;
}
#endif

MS_BOOL SCA_Geometry_Calibration(PSCA_GEOMETRY_CALI_CONFIG pCfg)
{

    MS_BOOL bRet = TRUE;
    SCALER_WIN eWindow;
    SCA_WINDOW_CONFIG stWinCfg;

    SCA_WAIT_MUTEX(_SCA_MoniWinCfg_Mutex);

    if(pCfg->enSrcType != SCA_SRC_VGA ||
       pCfg->enSrcType != g_stMonitorThread[pCfg->enDestType].enSrcType
      )
    {
        bRet = FALSE;
        SCA_DBG(SCA_DBG_LV_CALI,"Only support VGA:Dest:%d In:%d, Cur:%d \n", pCfg->enDestType, pCfg->enSrcType, g_stMonitorThread[pCfg->enDestType].enSrcType);
    }
    else
    {
        memcpy(&stWinCfg, &g_stMonitorWinCfg[pCfg->enDestType].stWinConfig, sizeof(SCA_WINDOW_CONFIG));
    }
    SCA_RELEASE_MUTEX(_SCA_MoniWinCfg_Mutex);

    eWindow = _Trans_To_Drv_Dest(pCfg->enDestType);

    if(eWindow == MAX_WINDOW)
    {
        bRet =  FALSE;
        SCA_DBG(SCA_DBG_LV_CALI,"dest is not correct");
    }


    if(bRet)
    {
        if(MApi_PCMode_Enable_SelfAuto(TRUE, eWindow))
        {
            MS_PCADC_MODESETTING_TYPE stModeSetting;

            MApi_PCMode_RunSelfAuto(eWindow);


            stModeSetting = MApi_PCMode_Get_ModeInfo(eWindow);

            if(stModeSetting.u8AutoSign)
            {
                bRet = TRUE;

                pCfg->bSuccess = TRUE;
                pCfg->u16Hstart = stModeSetting.u16HorizontalStart;
                pCfg->u16Vstart = stModeSetting.u16VerticalStart;
                pCfg->u16Htotal = stModeSetting.u16HorizontalTotal;
                pCfg->u16Phase  = stModeSetting.u16Phase;
                pCfg->u8ModeIndex = stModeSetting.u8ModeIndex;

                SCA_DBG(SCA_DBG_LV_CALI, "Geometry Success: ModeIdx:%d, Success:%d, Htt:%d, Hstar:%d, Vstar:%d, Phase:%d\n",
                        pCfg->u8ModeIndex, pCfg->bSuccess, pCfg->u16Htotal, pCfg->u16Hstart, pCfg->u16Vstart, pCfg->u16Phase);

                //MApi_PCMode_SetUserModeSetting(stModeSetting.u8ModeIndex, stModeSetting);
            }
            else
            {
                bRet = FALSE;
                memset(pCfg, 0, sizeof(PSCA_GEOMETRY_CALI_CONFIG));
                pCfg->bSuccess = FALSE;
                SCA_DBG(SCA_DBG_LV_CALI, "Geometry Cali Fail \n");
            }
        }
        else
        {
            bRet = FALSE;
            memset(pCfg, 0, sizeof(PSCA_GEOMETRY_CALI_CONFIG));
            pCfg->bSuccess = FALSE;
            SCA_DBG(SCA_DBG_LV_CALI, "Loss Signal in Geometry Calibration \n");
        }

    }

    return bRet;

}

MS_BOOL SCA_Load_Geometry_Setting(PSCA_GEOMETRY_CALI_CONFIG pCfg)
{
    MS_BOOL bRet = TRUE;

    MS_PCADC_MODESETTING_TYPE stModeSetting;
    SCALER_WIN eWindow;
    eWindow = _Trans_To_Drv_Dest(pCfg->enDestType);

    if(eWindow == MAX_WINDOW)
    {
        bRet =  FALSE;
        SCA_DBG(SCA_DBG_LV_CALI,"dest is not correct");
    }
    else
    {
        // update ADC phase & CLK
        MApi_XC_ADC_SetPcClock(pCfg->u16Htotal); // setting ADC clock
        MDrv_XC_ADC_SetPhaseEx(pCfg->u16Phase); // setting ADC phase


        // update Hstar, Vstart
        MApi_XC_SetCaptureWindowHstart(pCfg->u16Hstart, eWindow);
        MApi_XC_SetCaptureWindowVstart(pCfg->u16Vstart, eWindow);


        // save to User Mode Table
        memset(&stModeSetting, 0, sizeof(MS_PCADC_MODESETTING_TYPE));
        stModeSetting.u8ModeIndex = pCfg->u8ModeIndex;
        stModeSetting.u8AutoSign = 1;
        stModeSetting.u16DefaultHStart = pCfg->u16Hstart;
        stModeSetting.u16DefaultVStart = pCfg->u16Vstart;
        stModeSetting.u16DefaultHTotal = pCfg->u16Htotal;

        stModeSetting.u16HorizontalStart = pCfg->u16Hstart;
        stModeSetting.u16VerticalStart = pCfg->u16Vstart;
        stModeSetting.u16HorizontalTotal = pCfg->u16Htotal;
        stModeSetting.u16Phase = pCfg->u16Phase;


        MApi_PCMode_SetUserModeSetting(pCfg->u8ModeIndex, stModeSetting);
    }
    return bRet;
}

MS_BOOL SCA_Set_Analog_Polling_Config(PSCA_ANALOG_POLLING_CONFIG pCfg)
{
    MS_BOOL bRet = TRUE;

    MApi_XC_PCMonitor_SetTimingCount(pCfg->u16SableCnt, pCfg->u16NoSyncCnt);
    MApi_XC_PCMonitor_SetPollingPeriod(pCfg->u32PollingPeriod);

    return bRet;
}

#if defined(CONFIG_MS_CLONE_SCREEN)

MS_BOOL SCA_Set_Clone_Screen_Config(PSCA_CLONE_SCREEN_CONFIG pCfg)
{

    g_enCloeScreenMode = pCfg->enCloneScreenType;

    if(g_enCloeScreenMode == SCA_CLONE_SCREEN_GOP)
    {
        MApi_XC_Write2ByteMask(0x138000+(0x06*2), 0xB008, 0xFFFF);
    }
    else if(g_enCloeScreenMode == SCA_CLONE_SCREEN_NONE)
    {
        MApi_XC_Write2ByteMask(0x138000+(0x06*2), 0x0008, 0xFFFF);
    }

    return TRUE;
}


MS_BOOL SCA_Get_Clone_Screen_Config(PSCA_CLONE_SCREEN_CONFIG pCfg)
{
    pCfg->enCloneScreenType = g_enCloeScreenMode;
    return TRUE;
}

void SCA_CloneScr_SetDisplayRatio(MS_U16 u16H_Ratio, MS_U16 u16V_Ratio)
{
    g_u16Clone_Hratiox10 = u16H_Ratio;
    g_u16Clone_Vratiox10 = u16V_Ratio;
}


void SCA_CloneScr_GetOutputSize(
    MS_U16 *u16Hstart,
    MS_U16 *u16Vstart,
    MS_U16 *u16Hend,
    MS_U16 *u16Vend,
    MS_U16 *pu16Width,
    MS_U16 *pu16Height,
    MS_BOOL *bInterlace)
{
    MS_U32 u32Tmp;

    switch(g_enTGenTiming)
    {
    default:
    case E_XC_TGEN_VE_480_I_60:
        *pu16Width = 720;
        *pu16Height = 480/2;
        *bInterlace = 1;
        break;

    case E_XC_TGEN_VE_576_I_50:
        *pu16Width = 720;
        *pu16Height= 576/2;
        *bInterlace = 1;
        break;

    case E_XC_TGEN_HDMI_480_I_60:
        *pu16Width = 1440;
        *pu16Height = 480/2;
        *bInterlace = 1;
        break;

    case E_XC_TGEN_HDMI_576_I_50:
        *pu16Width = 1440;
        *pu16Height = 576/2;
        *bInterlace = 1;
        break;

    case E_XC_TGEN_HDMI_480_P_60:
        *pu16Width = 720;
        *pu16Height = 480;
        *bInterlace = 0;
        break;

    case E_XC_TGEN_HDMI_576_P_50:
        *pu16Width = 720;
        *pu16Height = 576;
        break;

    case E_XC_TGEN_HDMI_720_P_50:
    case E_XC_TGEN_HDMI_720_P_60:
        *pu16Width = 1280;
        *pu16Height = 720;
        *bInterlace = 0;
        break;


    case E_XC_TGEN_HDMI_1080_I_50:
    case E_XC_TGEN_HDMI_1080_I_60:
        *pu16Width = 1920;
        *pu16Height = 1080/2;
        *bInterlace = 1;
        break;

    case E_XC_TGEN_HDMI_1080_P_50:
    case E_XC_TGEN_HDMI_1080_P_60:
    case E_XC_TGEN_HDMI_1080_P_30:
    case E_XC_TGEN_HDMI_1080_P_25:
    case E_XC_TGEN_HDMI_1080_P_24:
        *pu16Width = 1920;
        *pu16Height = 1080;
        *bInterlace = 0;
        break;
    }

    if(g_u16Clone_Hratiox10)
    {
        u32Tmp = ((MS_U32)*pu16Width * (MS_U32)g_u16Clone_Hratiox10 + 500) / 1000;

        u32Tmp = (u32Tmp + 0x07) & (~0x07);
        *u16Hstart = (MS_U16)(u32Tmp/2);
        *u16Hend = *u16Hstart + *pu16Width - (MS_U16)(u32Tmp);
    }
    else
    {
        *u16Hstart = 0;
        *u16Hend = *pu16Width;
    }

    if(g_u16Clone_Vratiox10)
    {
        u32Tmp = ((MS_U32)*pu16Height* (MS_U32)g_u16Clone_Vratiox10 + 500) / 1000;
        u32Tmp = (u32Tmp + 0x01) & (~0x01);
        *u16Vstart = (MS_U16)(u32Tmp/2);
        *u16Vend = *u16Vstart + *pu16Height - (MS_U16)(u32Tmp);
    }
    else
    {
        *u16Vstart = 0;
        *u16Vend = *pu16Height;
    }
}


MS_BOOL SCA_CloneScr_Connect(PSCA_CONNECT_CONFIG pCfg)
{
    MS_BOOL bRet = TRUE;
    SCA_CHANGE_WINDOW_CONFIG  stChangeWinCfg;
    SCA_CONNECT_CONFIG stConnectCfg;

    if(pCfg->enDestType == SCA_DEST_MAIN)
    {
        MS_BOOL bInterlaceOut;
        MS_U16 u16Hstart, u16Hend, u16Vstart, u16Vend;
        MS_U16 u16Hsize, u16Vsize;

        memset(&stChangeWinCfg, 0, sizeof(SCA_CHANGE_WINDOW_CONFIG));
        stChangeWinCfg.bImmediateUpdate = FALSE;
        stChangeWinCfg.enDestType = SCA_DEST_MAIN_1;
        stChangeWinCfg.enSrcType = pCfg->enSrcType;
        stChangeWinCfg.stWinConfig.bDispWin = 1;

        SCA_CloneScr_GetOutputSize(&u16Hstart, &u16Vstart,
                                   &u16Hend, &u16Vend,
                                   &u16Hsize, &u16Vsize,
                                   &bInterlaceOut);

        stChangeWinCfg.stWinConfig.stDispWin.x = u16Hstart;
        stChangeWinCfg.stWinConfig.stDispWin.y = u16Vstart;
        stChangeWinCfg.stWinConfig.stDispWin.width = u16Hend - u16Hstart;
        stChangeWinCfg.stWinConfig.stDispWin.height = u16Vend - u16Vstart;


        SCA_WAIT_MUTEX(_SCA_Cnnt_Mutex);
        stChangeWinCfg.dwUID = g_stDispPathCfg[SCA_DEST_MAIN_1].dwUID;
        SCA_RELEASE_MUTEX(_SCA_Cnnt_Mutex);

        if(SCA_Change_Window(&stChangeWinCfg) == FALSE)
        {
            bRet = FALSE;
        }
        else
        {
            memcpy(&stConnectCfg, pCfg, sizeof(SCA_CONNECT_CONFIG));
            stConnectCfg.enDestType = SCA_DEST_MAIN_1;
            if(SCA_Connect(&stConnectCfg) == FALSE)
            {
                bRet = FALSE;
            }
        }
    }

    return bRet;
}

MS_BOOL SCA_CloneScr_DisConnect(PSCA_CONNECT_CONFIG pCfg)
{
    MS_BOOL bRet = TRUE;
    SCA_CONNECT_CONFIG stConnectCfg;
    if(pCfg->enDestType == SCA_DEST_MAIN)
    {
        stConnectCfg.enDestType = SCA_DEST_MAIN_1;
        stConnectCfg.bAutoDetect = pCfg->bAutoDetect;
        stConnectCfg.enSrcType = pCfg->enSrcType;

        SCA_WAIT_MUTEX(_SCA_Cnnt_Mutex);
        stConnectCfg.dwUID = g_stDispPathCfg[SCA_DEST_MAIN_1].dwUID;
        stConnectCfg.enPriType = g_stDispPathCfg[SCA_DEST_MAIN_1].enPriType;
        SCA_RELEASE_MUTEX(_SCA_Cnnt_Mutex);

        if(SCA_DisConnect(&stConnectCfg) == FALSE)
        {
            bRet = FALSE;
        }

    }

    return bRet;
}


MS_BOOL SCA_CloneScr_Set_DisplayMute(PSCA_DISPLAY_MUTE_CONFIG pCfg, E_SCREEN_MUTE_STATUS enMuteStatus)
{
    MS_BOOL bRet = TRUE;
    SCA_DISPLAY_MUTE_CONFIG stMuteCfg;
    if(pCfg->enDestType == SCA_DEST_MAIN)
    {
        stMuteCfg.bEn = pCfg->bEn;
        stMuteCfg.enDestType = SCA_DEST_MAIN_1;

        SCA_WAIT_MUTEX(_SCA_Cnnt_Mutex);
        stMuteCfg.dwUID = g_stDispPathCfg[SCA_DEST_MAIN_1].dwUID;
        SCA_RELEASE_MUTEX(_SCA_Cnnt_Mutex);

        if(SCA_Set_Display_Mute(&stMuteCfg, enMuteStatus) == FALSE)
        {
            bRet = FALSE;
        }
    }
    return bRet;
}

MS_BOOL SCA_CloneScr_Set_Timing_window(PSCA_SET_TIMING_WINDOW_CONFIG pCfg)
{
    MS_BOOL bRet = TRUE;

    SCA_SET_TIMING_WINDOW_CONFIG stTimingWinCfg;

    if(pCfg->enDestType == SCA_DEST_MAIN)
    {
        memcpy(&stTimingWinCfg, pCfg, sizeof(SCA_SET_TIMING_WINDOW_CONFIG));

        SCA_WAIT_MUTEX(_SCA_Cnnt_Mutex);
        stTimingWinCfg.dwUID = g_stDispPathCfg[SCA_DEST_MAIN_1].dwUID;
        SCA_RELEASE_MUTEX(_SCA_Cnnt_Mutex);

        stTimingWinCfg.enDestType = SCA_DEST_MAIN_1;

        if(pCfg->stWinConfig.bDispWin)
        {
            MS_U16  u16PNL_Width  = MApi_GetProjPaneltype()->m_wPanelWidth;
            MS_U16  u16PNL_Height = MApi_GetProjPaneltype()->m_wPanelHeight;
            MS_U16  u16CVBSOut_Width, u16CVBSOut_Height;
            MS_U16  u16CVBSOut_Hstart, u16CVBSOut_Vstart;
            MS_U16  u16CVBSOut_Hend, u16CVBSOut_Vend;
            MS_U16  u16DisplayWidth, u16DisplayHeight;
            MS_BOOL bInterlaceOut;

            SCA_CloneScr_GetOutputSize(&u16CVBSOut_Hstart, &u16CVBSOut_Vstart,
                                       &u16CVBSOut_Hend, &u16CVBSOut_Vend,
                                       &u16CVBSOut_Width, &u16CVBSOut_Height, &bInterlaceOut);

            u16DisplayWidth = u16CVBSOut_Hend - u16CVBSOut_Hstart;
            u16DisplayHeight = u16CVBSOut_Vend - u16CVBSOut_Vstart;

            stTimingWinCfg.stWinConfig.stDispWin.x=
                u16CVBSOut_Hstart + (MS_U16)((MS_U32)pCfg->stWinConfig.stDispWin.x * (MS_U32)u16DisplayWidth/ (MS_U32)u16PNL_Width);

            stTimingWinCfg.stWinConfig.stDispWin.y=
                u16CVBSOut_Vstart + (MS_U16)((MS_U32)pCfg->stWinConfig.stDispWin.y* (MS_U32)u16DisplayHeight / (MS_U32)u16PNL_Height);

            stTimingWinCfg.stWinConfig.stDispWin.width=
                (MS_U16)((MS_U32)pCfg->stWinConfig.stDispWin.width * (MS_U32)u16DisplayWidth/ (MS_U32)u16PNL_Width);

            stTimingWinCfg.stWinConfig.stDispWin.height=
                (MS_U16)((MS_U32)pCfg->stWinConfig.stDispWin.height* (MS_U32)u16DisplayHeight / (MS_U32)u16PNL_Height);
        }

        if(SCA_Set_Timing_Window(&stTimingWinCfg) == FALSE)
        {
            SCA_DBG(SCA_DBG_LV_PROGWIN, "SCA_CloneScr_Set_Timing_window Fail \n");
            bRet = FALSE;
        }

    }
    return bRet;
}


MS_BOOL SCA_CloneScr_Change_Window(PSCA_CHANGE_WINDOW_CONFIG pCfg)
{
    MS_BOOL bRet = TRUE;
    SCA_CHANGE_WINDOW_CONFIG stChangeWinCfg;

    if(pCfg->enDestType == SCA_DEST_MAIN)
    {
        memcpy(&stChangeWinCfg, pCfg, sizeof(SCA_CHANGE_WINDOW_CONFIG));

        stChangeWinCfg.enDestType = SCA_DEST_MAIN_1;

        SCA_WAIT_MUTEX(_SCA_Cnnt_Mutex);
        stChangeWinCfg.dwUID = g_stDispPathCfg[SCA_DEST_MAIN_1].dwUID;
        SCA_RELEASE_MUTEX(_SCA_Cnnt_Mutex);

        if(pCfg->stWinConfig.bDispWin)
        {
            MS_U16  u16PNL_Width  = MApi_GetProjPaneltype()->m_wPanelWidth;
            MS_U16  u16PNL_Height = MApi_GetProjPaneltype()->m_wPanelHeight;
            MS_U16  u16CVBSOut_Width, u16CVBSOut_Height;
            MS_U16  u16CVBSOut_Hstart, u16CVBSOut_Vstart;
            MS_U16  u16CVBSOut_Hend, u16CVBSOut_Vend;
            MS_U16  u16DisplayWidth, u16DisplayHeight;

            MS_BOOL bInterlaceOut;

            SCA_CloneScr_GetOutputSize(&u16CVBSOut_Hstart, &u16CVBSOut_Vstart,
                                       &u16CVBSOut_Hend, &u16CVBSOut_Vend,
                                       &u16CVBSOut_Width, &u16CVBSOut_Height, &bInterlaceOut);

            u16DisplayWidth = u16CVBSOut_Hend - u16CVBSOut_Hstart;
            u16DisplayHeight = u16CVBSOut_Vend - u16CVBSOut_Vstart;

            stChangeWinCfg.stWinConfig.stDispWin.x=
                u16CVBSOut_Hstart + (MS_U16)((MS_U32)pCfg->stWinConfig.stDispWin.x * (MS_U32)u16DisplayWidth/ (MS_U32)u16PNL_Width);

            stChangeWinCfg.stWinConfig.stDispWin.y=
                u16CVBSOut_Vstart + (MS_U16)((MS_U32)pCfg->stWinConfig.stDispWin.y* (MS_U32)u16DisplayHeight / (MS_U32)u16PNL_Height);

            stChangeWinCfg.stWinConfig.stDispWin.width=
                (MS_U16)((MS_U32)pCfg->stWinConfig.stDispWin.width * (MS_U32)u16DisplayWidth/ (MS_U32)u16PNL_Width);

            stChangeWinCfg.stWinConfig.stDispWin.height=
                (MS_U16)((MS_U32)pCfg->stWinConfig.stDispWin.height* (MS_U32)u16DisplayHeight / (MS_U32)u16PNL_Height);
        }

        if(SCA_Change_Window(&stChangeWinCfg) == FALSE)
        {
            SCA_DBG(SCA_DBG_LV_PROGWIN, "SCA_CloneScr_Change_Window Fail \n");
            bRet = FALSE;
        }


    }

    return bRet;
}
#endif


MS_BOOL SCA_Set_Pnl_SSC_Config(PSCA_PNL_SSC_CONFIG pCfg)
{
    if(pCfg->bEn)
    {
        MApi_PNL_SetSSC_Fmodulation(pCfg->u16Modulation);
        MApi_PNL_SetSSC_Rdeviation(pCfg->u16Deviation);
        MApi_PNL_SetSSC_En(TRUE);
    }
    else
    {
        MApi_PNL_SetSSC_En(FALSE);
        MApi_PNL_SetSSC_Fmodulation(pCfg->u16Modulation);
        MApi_PNL_SetSSC_Rdeviation(pCfg->u16Deviation);
    }

    return TRUE;
}



//------------------------------------------------------------------------------
// W: Sync Width
// B: back porch
// A: Active size
// F: Front porch
// T: Total
//------------------------------------------------------------------------------
//       _ _ _       Horizontal        _ _ _                         _ _ _
//      |     |                       |     |                       |     |
// _ _ _|     |_ _ _ _ _ _ _ _ _ _ _ _|     |_ _ _ _ _ _ _ _ _ _ _ _|     |_ _ _
//      |     |   |               |   |
//         W    B         A         F
//  HSync_End = W
//  HDE_Start = W + B
//  HDE_End   = W + B + A
//------------------------------------------------------------------------------
//       _ _ _                         _ _ _                         _ _ _
//      |     |      Vertical         |     |                       |     |
// _ _ _|     |_ _ _ _ _ _ _ _ _ _ _ _|     |_ _ _ _ _ _ _ _ _ _ _ _|     |_ _ _
//                |               |   |     |   |
//                        A         F    W    B
//
// VTotal = A + F + W + B
// VDE_Start = 0
// VDE_End   = A
// VSync_Start = VTotal - W - B
// VSync_End   = VTotal -  B
MS_BOOL SCA_Set_Pnl_Timing_Config(PSCA_PNL_TIMING_CONFIG pCfg)
{
    XC_PANEL_INFO stPanelInfo;
    #define PNL_V_START 0

    if( (pCfg->u16HSync_Width+ pCfg->u16HSync_Bporch + pCfg->u16HSync_Fporch + pCfg->u16H_Active) != pCfg->u16Htt)
    {
        SCA_DBG(SCA_DBG_LV_0, "PNL Timing: H setting is not correct \n");
        return FALSE;
    }

    if( (pCfg->u16VSync_Width+ pCfg->u16VSync_Bporch + pCfg->u16VSync_Fporch + pCfg->u16V_Active) != pCfg->u16Vtt)
    {
        SCA_DBG(SCA_DBG_LV_0, "PNL Timing: V setting is not correct \n");
        return FALSE;
    }



    // update XC data
    MApi_XC_Get_PanelInfo(&stPanelInfo);

    stPanelInfo.u16DefaultVFreq  = (MS_U16)(((MS_U32)pCfg->u16DCLK_MHz * 10000000) / ((MS_U32)pCfg->u16Htt * (MS_U32)pCfg->u16Vtt));
    stPanelInfo.u16DefaultHTotal = pCfg->u16Htt;
    stPanelInfo.u16DefaultVTotal = pCfg->u16Vtt;

    stPanelInfo.u16HTotal = pCfg->u16Htt;
    stPanelInfo.u16VTotal = pCfg->u16Vtt;
    stPanelInfo.u16Height = pCfg->u16V_Active;
    stPanelInfo.u16Width  = pCfg->u16H_Active;
    stPanelInfo.u16HStart = pCfg->u16HSync_Width + pCfg->u16HSync_Bporch;
    stPanelInfo.u16VStart = PNL_V_START;

    MApi_XC_Set_PanelInfo(&stPanelInfo);

    // update Pnl data
    (MApi_GetProjPaneltype()->m_wPanelHTotal) = pCfg->u16Htt;
    (MApi_GetProjPaneltype()->m_wPanelVTotal) = pCfg->u16Vtt;

    (MApi_GetProjPaneltype()->m_wPanelWidth)  = pCfg->u16H_Active;
    (MApi_GetProjPaneltype()->m_wPanelHeight) = pCfg->u16V_Active;

    (MApi_GetProjPaneltype()->m_ucPanelHSyncWidth)     = (MS_U8)pCfg->u16HSync_Width;
    (MApi_GetProjPaneltype()->m_ucPanelHSyncBackPorch) = (MS_U8)pCfg->u16HSync_Bporch;
    (MApi_GetProjPaneltype()->m_wPanelHStart)          = pCfg->u16HSync_Width + pCfg->u16HSync_Bporch;;

    (MApi_GetProjPaneltype()->m_wPanelVStart)      = PNL_V_START;
    (MApi_GetProjPaneltype()->m_ucPanelVSyncWidth) = (MS_U8)pCfg->u16VSync_Width;
    (MApi_GetProjPaneltype()->m_ucPanelVBackPorch) = (MS_U8)pCfg->u16VSync_Bporch;

    (MApi_GetProjPaneltype()->m_dwPanelDCLK) = (MS_U8)(pCfg->u16DCLK_MHz);

    // set to HW
    MApi_PNL_Reset_Param(MApi_GetProjPaneltype());

    // update GOP pipe delay
    MApi_XC_Write2ByteMask(0x12021E,  0x50+pCfg->u16HSync_Bporch, 0xFFFF);
    MApi_XC_Write2ByteMask(0x1202FF,  0x200, 0x200);
    MApi_XC_Write2ByteMask(0x1202FF,  0x000, 0x200);
    return TRUE;
}

MS_BOOL SCA_Set_User_Display_Config(PSCA_USER_DISPLAY_CONFIG pCfg)
{
    INPUT_SOURCE_TYPE_t enInputSourceType;

    enInputSourceType = _Trans_To_Drv_Src(pCfg->enSrcType);

    SCA_WAIT_MUTEX(_SCA_Cnnt_Mutex);

    MApi_InputSource_Set_User_Display_Config(
        MAX_WINDOW,
        enInputSourceType,
        pCfg->u16H_Overscan, pCfg->u16V_Overscan,
        pCfg->s16H_Offset, pCfg->s16V_Offset,
        pCfg->bCVBS_NTSC,
        pCfg->bEn);

    SCA_RELEASE_MUTEX(_SCA_Cnnt_Mutex);

    return TRUE;
}


MS_BOOL SCA_Set_User_Display_Config_EX(PSCA_USER_DISPLAY_CONFIG_EX pCfg)
{
    INPUT_SOURCE_TYPE_t enInputSourceType;
    SCALER_WIN eWin;

    enInputSourceType = _Trans_To_Drv_Src(pCfg->enSrcType);
    eWin = _Trans_To_Drv_Dest(pCfg->enDestType);

    SCA_WAIT_MUTEX(_SCA_Cnnt_Mutex);

    MApi_InputSource_Set_User_Display_Config(
        eWin,
        enInputSourceType,
        pCfg->u16H_Overscan, pCfg->u16V_Overscan,
        pCfg->s16H_Offset, pCfg->s16V_Offset,
        pCfg->bCVBS_NTSC,
        pCfg->bEn);

    SCA_RELEASE_MUTEX(_SCA_Cnnt_Mutex);

    return TRUE;
}


MS_BOOL SCA_Set_DisplayMute_Color_Config(PSCA_DISPLAY_MUTE_COLOR_CONFIG pCfg)
{
    SCALER_WIN eWindow;
    MS_BOOL bRet = TRUE;

    eWindow = _Trans_To_Drv_Dest(pCfg->enDestType);

    if(eWindow == MAX_WINDOW)
    {
        bRet =  FALSE;
        SCA_DBG(SCA_DBG_LV_CALI,"dest is not correct");
    }
    else
    {
        XC_FREERUN_COLOR enColor;


        enColor = pCfg->enColorType == SCA_DISPLAY_MUTE_BLACK ? E_XC_FREE_RUN_COLOR_BLACK :
                  pCfg->enColorType == SCA_DISPLAY_MUTE_WHITE ? E_XC_FREE_RUN_COLOR_WHITE :
                  pCfg->enColorType == SCA_DISPLAY_MUTE_BLUE  ? E_XC_FREE_RUN_COLOR_BLUE :
                  pCfg->enColorType == SCA_DISPLAY_MUTE_RED   ? E_XC_FREE_RUN_COLOR_RED :
                  pCfg->enColorType == SCA_DISPLAY_MUTE_GREEN ? E_XC_FREE_RUN_COLOR_GREEN :
                                                                E_XC_FREE_RUN_COLOR_MAX;

        MApi_Scaler_set_DisplayMute_Color(enColor, eWindow);
    }

    return bRet;
}

MS_BOOL SCA_Set_DLC_Init_Config(PSCA_DLC_INIT_CONFIG pCfg)
{
    MS_BOOL bEn;
    XC_DLC_ApiStatus stDLCStatus;
    XC_DLC_init stDLCInitData;
    MS_U8 i;


    bEn = g_stDLCMonitorCfg.bEn;
    g_stDLCMonitorCfg.bEn = FALSE;

    if(MApi_XC_DLC_GetStatus(&stDLCStatus, MAIN_WINDOW))
    {
        memcpy(&stDLCInitData, &stDLCStatus.DLCinit, sizeof(XC_DLC_init));
    }
    else
    {
        memset(&stDLCInitData, 0, sizeof(XC_DLC_init));
    }

    for (i=0; i<16; ++i)
    {
        stDLCInitData.DLC_MFinit_Ex.ucLumaCurve[i]     = pCfg->ucLumaCurve[i];
        stDLCInitData.DLC_MFinit_Ex.ucLumaCurve2_a[i]  = pCfg->ucLumaCurve2_a[i];
        stDLCInitData.DLC_MFinit_Ex.ucLumaCurve2_b[i]  = pCfg->ucLumaCurve2_b[i];
    }

    stDLCInitData.DLC_MFinit_Ex.u8_L_L_U = pCfg->u8_L_L_U;
    stDLCInitData.DLC_MFinit_Ex.u8_L_L_D = pCfg->u8_L_L_D;
    stDLCInitData.DLC_MFinit_Ex.u8_L_H_U = pCfg->u8_L_H_U;
    stDLCInitData.DLC_MFinit_Ex.u8_L_H_D = pCfg->u8_L_H_D;
    stDLCInitData.DLC_MFinit_Ex.u8_S_L_U = pCfg->u8_S_L_U;
    stDLCInitData.DLC_MFinit_Ex.u8_S_L_D = pCfg->u8_S_L_D;
    stDLCInitData.DLC_MFinit_Ex.u8_S_H_U = pCfg->u8_S_H_U;
    stDLCInitData.DLC_MFinit_Ex.u8_S_H_D = pCfg->u8_S_H_D;


    stDLCInitData.DLC_MFinit_Ex.ucDlcPureImageMode     = pCfg->ucDlcPureImageMode;
    stDLCInitData.DLC_MFinit_Ex.ucDlcLevelLimit        = pCfg->ucDlcLevelLimit;
    stDLCInitData.DLC_MFinit_Ex.ucDlcAvgDelta          = pCfg->ucDlcAvgDelta;
    stDLCInitData.DLC_MFinit_Ex.ucDlcAvgDeltaStill     = pCfg->ucDlcAvgDeltaStill;
    stDLCInitData.DLC_MFinit_Ex.ucDlcFastAlphaBlending = pCfg->ucDlcFastAlphaBlending;

    stDLCInitData.DLC_MFinit_Ex.ucDlcYAvgThresholdL    = pCfg->ucDlcYAvgThresholdL;
    stDLCInitData.DLC_MFinit_Ex.ucDlcYAvgThresholdH    = pCfg->ucDlcYAvgThresholdH;
    stDLCInitData.DLC_MFinit_Ex.ucDlcBLEPoint          = pCfg->ucDlcBLEPoint;
    stDLCInitData.DLC_MFinit_Ex.ucDlcWLEPoint          = pCfg->ucDlcWLEPoint;
    stDLCInitData.DLC_MFinit_Ex.bEnableBLE             = pCfg->bEnableBLE;
    stDLCInitData.DLC_MFinit_Ex.bEnableWLE             = pCfg->bEnableWLE;

    stDLCInitData.DLC_MFinit_Ex.ucDlcYAvgThresholdM    = pCfg->ucDlcYAvgThresholdM;
    stDLCInitData.DLC_MFinit_Ex.ucDlcCurveMode         = pCfg->ucDlcCurveMode;
    stDLCInitData.DLC_MFinit_Ex.ucDlcCurveModeMixAlpha = pCfg->ucDlcCurveModeMixAlpha;

    stDLCInitData.DLC_MFinit_Ex.ucDlcAlgorithmMode     = pCfg->ucDlcAlgorithmMode;

    stDLCInitData.DLC_MFinit_Ex.ucDlcSepPointH         = pCfg->ucDlcSepPointH;
    stDLCInitData.DLC_MFinit_Ex.ucDlcSepPointL         = pCfg->ucDlcSepPointL;
    stDLCInitData.DLC_MFinit_Ex.uwDlcBleStartPointTH   = pCfg->uwDlcBleStartPointTH;
    stDLCInitData.DLC_MFinit_Ex.uwDlcBleEndPointTH     = pCfg->uwDlcBleEndPointTH;
    stDLCInitData.DLC_MFinit_Ex.ucDlcCurveDiff_L_TH    = pCfg->ucDlcCurveDiff_L_TH;
    stDLCInitData.DLC_MFinit_Ex.ucDlcCurveDiff_H_TH    = pCfg->ucDlcCurveDiff_H_TH;
    stDLCInitData.DLC_MFinit_Ex.uwDlcBLESlopPoint_1    = pCfg->uwDlcBLESlopPoint_1;
    stDLCInitData.DLC_MFinit_Ex.uwDlcBLESlopPoint_2    = pCfg->uwDlcBLESlopPoint_2;
    stDLCInitData.DLC_MFinit_Ex.uwDlcBLESlopPoint_3    = pCfg->uwDlcBLESlopPoint_3;
    stDLCInitData.DLC_MFinit_Ex.uwDlcBLESlopPoint_4    = pCfg->uwDlcBLESlopPoint_4;
    stDLCInitData.DLC_MFinit_Ex.uwDlcBLESlopPoint_5    = pCfg->uwDlcBLESlopPoint_5;
    stDLCInitData.DLC_MFinit_Ex.uwDlcDark_BLE_Slop_Min = pCfg->uwDlcDark_BLE_Slop_Min;
    stDLCInitData.DLC_MFinit_Ex.ucDlcCurveDiffCoringTH = pCfg->ucDlcCurveDiffCoringTH;
    stDLCInitData.DLC_MFinit_Ex.ucDlcAlphaBlendingMin  = pCfg->ucDlcAlphaBlendingMin;
    stDLCInitData.DLC_MFinit_Ex.ucDlcAlphaBlendingMax  = pCfg->ucDlcAlphaBlendingMax;
    stDLCInitData.DLC_MFinit_Ex.ucDlcFlicker_alpha     = pCfg->ucDlcFlicker_alpha;
    stDLCInitData.DLC_MFinit_Ex.ucDlcYAVG_L_TH         = pCfg->ucDlcYAVG_L_TH;
    stDLCInitData.DLC_MFinit_Ex.ucDlcYAVG_H_TH         = pCfg->ucDlcYAVG_H_TH;
    stDLCInitData.DLC_MFinit_Ex.ucDlcDiffBase_L        = pCfg->ucDlcDiffBase_L;
    stDLCInitData.DLC_MFinit_Ex.ucDlcDiffBase_M        = pCfg->ucDlcDiffBase_M;
    stDLCInitData.DLC_MFinit_Ex.ucDlcDiffBase_H        = pCfg->ucDlcDiffBase_H;

    stDLCInitData.DLC_MFinit_Ex.bCGCCGainCtrl          = pCfg->bCGCCGainCtrl;
    stDLCInitData.DLC_MFinit_Ex.ucCGCCGain_offset      = pCfg->ucCGCCGain_offset;
    stDLCInitData.DLC_MFinit_Ex.ucCGCChroma_GainLimitH = pCfg->ucCGCChroma_GainLimitH;
    stDLCInitData.DLC_MFinit_Ex.ucCGCChroma_GainLimitL = pCfg->ucCGCChroma_GainLimitL;
    stDLCInitData.DLC_MFinit_Ex.ucCGCYCslope           = pCfg->ucCGCYCslope;
    stDLCInitData.DLC_MFinit_Ex.ucCGCYth               = pCfg->ucCGCYth;


    if(MApi_XC_DLC_Init_Ex(&stDLCInitData, sizeof(XC_DLC_init)) == FALSE)
    {
        SCA_DBG(SCA_DBG_LV_0, "MApi_XC_DLC_Init_Ex Fail \n");
        return FALSE;
    }

#if SCA_DBG_LV_DLC
    SCA_DBG(SCA_DBG_LV_0, "------------------------AFTER------------------------ ");

    SCA_DBG(SCA_DBG_LV_0, "Curve: ");
    for(i=0; i<16;i++)
    {
        SCA_DBG(SCA_DBG_LV_0, "%02x, ", stDLCInitData.DLC_MFinit_Ex.ucLumaCurve[i]);
    }
    SCA_DBG(SCA_DBG_LV_0, "\n");

    SCA_DBG(SCA_DBG_LV_0, "Curve2a: ");
    for(i=0; i<16;i++)
    {
        SCA_DBG(SCA_DBG_LV_0, "%02x, ", stDLCInitData.DLC_MFinit_Ex.ucLumaCurve2_a[i]);
    }
    SCA_DBG(SCA_DBG_LV_0, "\n");

    SCA_DBG(SCA_DBG_LV_0, "Curve2b: ");
    for(i=0; i<16;i++)
    {
        SCA_DBG(SCA_DBG_LV_0, "%02x, ", stDLCInitData.DLC_MFinit_Ex.ucLumaCurve2_b[i]);
    }
    SCA_DBG(SCA_DBG_LV_0, "\n");

    SCA_DBG(SCA_DBG_LV_0,"u8_L_L_U: %d\n", stDLCInitData.DLC_MFinit_Ex.u8_L_L_U);
    SCA_DBG(SCA_DBG_LV_0,"u8_L_L_D: %d\n", stDLCInitData.DLC_MFinit_Ex.u8_L_L_D);
    SCA_DBG(SCA_DBG_LV_0,"u8_L_H_U: %d\n", stDLCInitData.DLC_MFinit_Ex.u8_L_H_U);
    SCA_DBG(SCA_DBG_LV_0,"u8_L_H_D: %d\n", stDLCInitData.DLC_MFinit_Ex.u8_L_H_D);
    SCA_DBG(SCA_DBG_LV_0,"u8_S_L_U: %d\n", stDLCInitData.DLC_MFinit_Ex.u8_S_L_U);
    SCA_DBG(SCA_DBG_LV_0,"u8_S_L_D: %d\n", stDLCInitData.DLC_MFinit_Ex.u8_S_L_D);
    SCA_DBG(SCA_DBG_LV_0,"u8_S_H_U: %d\n", stDLCInitData.DLC_MFinit_Ex.u8_S_H_U);
    SCA_DBG(SCA_DBG_LV_0,"u8_S_H_D: %d\n", stDLCInitData.DLC_MFinit_Ex.u8_S_H_D);

    SCA_DBG(SCA_DBG_LV_0,"ucDlcPureImageMode: %d\n", stDLCInitData.DLC_MFinit_Ex.ucDlcPureImageMode);
    SCA_DBG(SCA_DBG_LV_0,"ucDlcLevelLimit: %d\n", stDLCInitData.DLC_MFinit_Ex.ucDlcLevelLimit);
    SCA_DBG(SCA_DBG_LV_0,"ucDlcAvgDelta: %d\n", stDLCInitData.DLC_MFinit_Ex.ucDlcAvgDelta);
    SCA_DBG(SCA_DBG_LV_0,"ucDlcAvgDeltaStill: %d\n", stDLCInitData.DLC_MFinit_Ex.ucDlcAvgDeltaStill);
    SCA_DBG(SCA_DBG_LV_0,"ucDlcFastAlphaBlending: %d\n", stDLCInitData.DLC_MFinit_Ex.ucDlcFastAlphaBlending);


    SCA_DBG(SCA_DBG_LV_0,"ucDlcYAvgThresholdL: %d\n", stDLCInitData.DLC_MFinit_Ex.ucDlcYAvgThresholdL);
    SCA_DBG(SCA_DBG_LV_0,"ucDlcYAvgThresholdH: %d\n", stDLCInitData.DLC_MFinit_Ex.ucDlcYAvgThresholdH);
    SCA_DBG(SCA_DBG_LV_0,"ucDlcBLEPoint      : %d\n", stDLCInitData.DLC_MFinit_Ex.ucDlcBLEPoint      );
    SCA_DBG(SCA_DBG_LV_0,"ucDlcWLEPoint      : %d\n", stDLCInitData.DLC_MFinit_Ex.ucDlcWLEPoint      );
    SCA_DBG(SCA_DBG_LV_0,"bEnableBLE: %d\n", stDLCInitData.DLC_MFinit_Ex.bEnableBLE);
    SCA_DBG(SCA_DBG_LV_0,"bEnableWLE: %d\n", stDLCInitData.DLC_MFinit_Ex.bEnableWLE);
    SCA_DBG(SCA_DBG_LV_0,"bCGCCGainCtrl    : %d\n", stDLCInitData.DLC_MFinit_Ex.bCGCCGainCtrl    );
    SCA_DBG(SCA_DBG_LV_0,"ucCGCCGain_offset: %d\n", stDLCInitData.DLC_MFinit_Ex.ucCGCCGain_offset);

    SCA_DBG(SCA_DBG_LV_0,"ucCGCChroma_GainLimitH: %d\n", stDLCInitData.DLC_MFinit_Ex.ucCGCChroma_GainLimitH);
    SCA_DBG(SCA_DBG_LV_0,"ucCGCChroma_GainLimitL: %d\n", stDLCInitData.DLC_MFinit_Ex.ucCGCChroma_GainLimitL);
    SCA_DBG(SCA_DBG_LV_0,"ucCGCYCslope          : %d\n", stDLCInitData.DLC_MFinit_Ex.ucCGCYCslope          );
    SCA_DBG(SCA_DBG_LV_0,"ucCGCYth              : %d\n", stDLCInitData.DLC_MFinit_Ex.ucCGCYth              );

#endif


    g_stDLCMonitorCfg.bEn = bEn;

    return TRUE;
}

MS_BOOL SCA_Set_DLC_OnOff_Config(PSCA_DLC_ONOFF_CONFIG pCfg)
{

    if(pCfg->enDestType != SCA_DEST_MAIN)
    {
        return FALSE;
    }

    g_stDLCMonitorCfg.bUserControl = pCfg->bUserControl;
    g_stDLCMonitorCfg.bUserOnOff = pCfg->bDLCOnOff;
    return TRUE;
}


MS_BOOL SCA_Set_CVBSOUT_DAC_Config(PSCA_CVBSOUT_DAC_CONFIG pCfg)
{
    MDrv_SC_iDAC_SetEnabled(pCfg->bEn);
    return TRUE;
}

MS_BOOL SCA_Set_Camera_InputTiming_Config(PSCA_CAMERA_INPUTTIMING_CONFIG pCfg)
{
    MS_INPUT_TIMING_CONFIG stTimingCfg;

    stTimingCfg.u16x       = pCfg->u16x;
    stTimingCfg.u16y       = pCfg->u16y;
    stTimingCfg.u16Width   = pCfg->u16Width;
    stTimingCfg.u16Height  = pCfg->u16Height;
    stTimingCfg.u16Vtotal  = pCfg->u16Vtotal;
    stTimingCfg.u16VFreq   = pCfg->u16VFreq;
    stTimingCfg.bInterlace = pCfg->bInterlace;
    MApi_InputSource_SetCameraTiming(stTimingCfg);

    return TRUE;
}

//------------------------------------------------------------------------------
// AVD
//------------------------------------------------------------------------------
#define IS_AVD_CVBS(x)		((x>= INPUT_SOURCE_CVBS) 	&& (x<= INPUT_SOURCE_CVBS8))
#define IS_AVD_SVIDEO(x)	((x>= INPUT_SOURCE_SVIDEO) 	&& (x<= INPUT_SOURCE_SVIDEO4))
#define IsAVDSrc(x)   		( IS_AVD_CVBS(x) || IS_AVD_SVIDEO(x) )

INPUT_SOURCE_TYPE_t _TransToDrvSrc(AVD_SRC_TYPE enSrcType)
{
#if 1	//!! Add s-video input source !!//

	INPUT_SOURCE_TYPE_t eAvdInputSrc = INPUT_SOURCE_NONE;

	switch (enSrcType)
	{
		case AVD_SRC_CVBS:
			eAvdInputSrc = INPUT_SOURCE_CVBS;
			break;

		case AVD_SRC_SVIDEO:
			eAvdInputSrc = INPUT_SOURCE_SVIDEO;
			break;

		default:
			eAvdInputSrc = INPUT_SOURCE_NONE;
			break;
	}

	return eAvdInputSrc;

#else

    if(enSrcType == AVD_SRC_CVBS)
        return INPUT_SOURCE_CVBS;
    else
        return INPUT_SOURCE_NONE;

#endif
}


MS_BOOL SCA_AVD_Connect(AVD_CONNECT_CONFIG *pCfg)
{
    INPUT_SOURCE_TYPE_t enInputSourceType;

    msAPI_AVD_InitVideoSystem();
    m_dwATVAVHandlerTimer = 0;
    m_wTVAVTimer = WAIT_0ms;

    enInputSourceType = _TransToDrvSrc(pCfg->enSrcType);

    if(!IsAVDSrc(enInputSourceType))
        return FALSE;

    MApi_InputSource_ChangeVideoSource(enInputSourceType);

    return TRUE;
}

MS_BOOL SCA_AVD_Check_VideoStd(AVD_CHECK_VIDEOSTD_CONFIG *pCfg)
{
    MApi_ATVProc_Handler();

    if(msAPI_AVD_CheckStdDetStableCnt() == 0 && msAPI_AVD_ChekcStdDetStableState() == E_DETECTION_DETECT)
    {
        pCfg->enDetectState = AVD_VIDEO_DETECT_STABLE;
    }
    else
    {
        pCfg->enDetectState = AVD_VIDEO_DETECT_UNSTABLE;
    }

    if(!IsVDHasSignal())
    {
        pCfg->enVideoStandard = AVD_VIDEOSTANDARD_NOTSTANDARD;
    }
    else
    {
       switch(msAPI_AVD_GetVideoStandard())
       {
        case E_VIDEOSTANDARD_PAL_BGHI:
            pCfg->enVideoStandard = AVD_VIDEOSTANDARD_PAL_BGHI;
            break;
        case E_VIDEOSTANDARD_NTSC_M:
            pCfg->enVideoStandard = AVD_VIDEOSTANDARD_NTSC_M;
            break;
        case E_VIDEOSTANDARD_SECAM:
            pCfg->enVideoStandard = AVD_VIDEOSTANDARD_SECAM;
            break;
        case E_VIDEOSTANDARD_NTSC_44:
            pCfg->enVideoStandard = AVD_VIDEOSTANDARD_NTSC_44;
            break;

        case E_VIDEOSTANDARD_PAL_M:
            pCfg->enVideoStandard = AVD_VIDEOSTANDARD_PAL_M;
            break;

        case E_VIDEOSTANDARD_PAL_N:
            pCfg->enVideoStandard = AVD_VIDEOSTANDARD_PAL_N;
            break;

        case E_VIDEOSTANDARD_PAL_60:
            pCfg->enVideoStandard = AVD_VIDEOSTANDARD_PAL_60;
            break;

        default:
            pCfg->enVideoStandard = E_VIDEOSTANDARD_NOTSTANDARD;
            break;
       }
    }
    return TRUE;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------------------------
// IOCtrl Driver interface functions
//-------------------------------------------------------------------------------------------------
int _MDrv_SCAIO_IOC_Connect(struct file *filp, unsigned long arg)
{
    SCA_CONNECT_CONFIG stCfg;

    if(copy_from_user(&stCfg, (SCA_CONNECT_CONFIG __user *)arg, sizeof(SCA_CONNECT_CONFIG)))
    {
        return -EFAULT;
    }

    if(!SCA_Connect(&stCfg))
    {
        return -EFAULT;
    }

#if defined(CONFIG_MS_CLONE_SCREEN)
    if(g_enCloeScreenMode == SCA_CLONE_SCREEN_GOP)
    {
        if(!SCA_CloneScr_Connect(&stCfg))
        {
            return -EFAULT;
        }
    }
#endif

    if(copy_to_user((SCA_CONNECT_CONFIG __user *)arg, &stCfg, sizeof(SCA_CONNECT_CONFIG)))
    {
        return -EFAULT;
    }

    return 0;
}


int _MDrv_SCAIO_IOC_DisConnect(struct file *filp, unsigned long arg)
{
    SCA_CONNECT_CONFIG stCfg;

    if(copy_from_user(&stCfg, (SCA_CONNECT_CONFIG __user *)arg, sizeof(SCA_CONNECT_CONFIG)))
    {
        return -EFAULT;
    }


    if(!SCA_DisConnect(&stCfg))
    {
        return -EFAULT;
    }

#if defined(CONFIG_MS_CLONE_SCREEN)
    if(g_enCloeScreenMode == SCA_CLONE_SCREEN_GOP)
    {
        if(!SCA_CloneScr_DisConnect(&stCfg))
        {
            return -EFAULT;
        }
    }
#endif

    return 0;
}


int _MDrv_SCAIO_IOC_Set_Timing_Window(struct file *filp, unsigned long arg)
{
    SCA_SET_TIMING_WINDOW_CONFIG stCfg;

    if(copy_from_user(&stCfg, (SCA_SET_TIMING_WINDOW_CONFIG __user *)arg, sizeof(SCA_SET_TIMING_WINDOW_CONFIG)))
    {
        return -EFAULT;
    }

    if(!SCA_Set_Timing_Window(&stCfg))
    {
        return -EFAULT;
    }

#if defined(CONFIG_MS_CLONE_SCREEN)
    if(g_enCloeScreenMode == SCA_CLONE_SCREEN_GOP)
    {
        if(!SCA_CloneScr_Set_Timing_window(&stCfg))
        {
            return -EFAULT;
        }
    }
#endif

    return 0;
}



int _MDrv_SCAIO_IOC_Set_MVOP(struct file *filp, unsigned long arg)
{
    SCA_MVOP_CONFIG stCfg;
    if(copy_from_user(&stCfg, (SCA_MVOP_CONFIG __user *)arg, sizeof(SCA_MVOP_CONFIG)))
    {
        return -EFAULT;
    }

    if(!SCA_Set_MVOP(&stCfg))
    {
        return -EFAULT;
    }

    return 0;
}

int _MDrv_SCAIO_IOC_Set_Display_Mute(struct file *filp, unsigned long arg)
{
    SCA_DISPLAY_MUTE_CONFIG stCfg;
    if(copy_from_user(&stCfg, (SCA_DISPLAY_MUTE_CONFIG __user *)arg, sizeof(SCA_DISPLAY_MUTE_CONFIG)))
    {
        return -EFAULT;
    }

    if(!SCA_Set_Display_Mute(&stCfg, E_SCREEN_MUTE_INPUT))
    {
        return -EFAULT;
    }

#if defined(CONFIG_MS_CLONE_SCREEN)
    if(g_enCloeScreenMode == SCA_CLONE_SCREEN_GOP)
    {
        if(!SCA_CloneScr_Set_DisplayMute(&stCfg, E_SCREEN_MUTE_INPUT))
        {
            return -EFAULT;
        }
    }
#endif

    return 0;
}

int _MDrv_SCAIO_IOC_YPBPR_VGA_Mode_Monitor_Parse(struct file *filp, unsigned long arg)
{
    SCA_MODE_MONITOR_PARSE_CONFIG stCfg;
    if(copy_from_user(&stCfg, (SCA_MODE_MONITOR_PARSE_CONFIG __user *)arg, sizeof(SCA_MODE_MONITOR_PARSE_CONFIG)))
    {
        return -EFAULT;
    }

    if(!SCA_YPBPR_VGA_Mode_Monitor_Parse(&stCfg.stMode_MP_InCfg, &stCfg.stMode_MP_OutCfg))
    {
        return -EFAULT;
    }

    if(copy_to_user((SCA_MODE_MONITOR_PARSE_CONFIG __user *)arg, &stCfg, sizeof(SCA_MODE_MONITOR_PARSE_CONFIG)))
    {
        return -EFAULT;
    }
    return 0;
}

int _MDrv_SCAIO_IOC_Get_Mode(struct file *filp, unsigned long arg)
{
    SCA_GET_MODE_CONFIG stCfg;
    if(copy_from_user(&stCfg, (SCA_GET_MODE_CONFIG __user *)arg, sizeof(SCA_GET_MODE_CONFIG)))
    {
        return -EFAULT;
    }

    if(!SCA_Get_Mode(&stCfg.stGetModeInCfg, &stCfg.stGetModeOutCfg))
    {
        return -EFAULT;
    }

    if(copy_to_user((SCA_GET_MODE_CONFIG __user *)arg, &stCfg, sizeof(SCA_GET_MODE_CONFIG)))
    {
        return -EFAULT;
    }

    return 0;
}

int _MDrv_SCAIO_IOC_Set_VE(struct file *filp, unsigned long arg)
{
    SCA_VE_CONFIG stCfg;
    if(copy_from_user(&stCfg, (SCA_VE_CONFIG __user *)arg, sizeof(SCA_VE_CONFIG)))
    {
        return -EFAULT;
    }

    if(!SCA_Set_VE(&stCfg))
    {
        return -EFAULT;
    }

    return 0;
}


int _MDrv_SCAIO_IOC_Set_Window_OnOFF(struct file *filp, unsigned long arg)
{
    SCA_DISPLAY_WINDOW_ON_OFF_CONFIG stCfg;

    if(copy_from_user(&stCfg, (SCA_DISPLAY_WINDOW_ON_OFF_CONFIG __user *)arg, sizeof(SCA_DISPLAY_WINDOW_ON_OFF_CONFIG)))
    {
        return -EFAULT;
    }

    if(!SCA_Set_Window_OnOFF(&stCfg))
    {
        return -EFAULT;
    }
    return 0;
}



int _MDrv_SCAIO_IOC_Set_ConstantAlpha_State(struct file *filp, unsigned long arg)
{
    SCA_CONSTANTALPHA_STATE stCfg;
    if(copy_from_user(&stCfg, (SCA_CONSTANTALPHA_STATE __user *)arg, sizeof(SCA_CONSTANTALPHA_STATE)))
    {
        return -EFAULT;
    }

    if(!SCA_Set_ConstantAlpha_State(&stCfg))
    {
        return -EFAULT;
    }


    return 0;
}

int _MDrv_SCAIO_IOC_Set_ColorKey(struct file *filp, unsigned long arg)
{
    SCA_COLOR_KEY stCfg;
    if(copy_from_user(&stCfg, (SCA_COLOR_KEY __user *)arg, sizeof(SCA_COLOR_KEY)))
    {
        return -EFAULT;
    }

    if(!SCA_Set_Color_Key(&stCfg))
    {
        return -EFAULT;
    }

#if defined(CONFIG_MS_CLONE_SCREEN)
    if(g_enCloeScreenMode == SCA_CLONE_SCREEN_GOP)
    {
        if(stCfg.View == 0)
        {
            stCfg.View = 1;
            if(!SCA_Set_Color_Key(&stCfg))
            {
                return -EFAULT;
            }
        }
    }
#endif
    return 0;
}

int _MDrv_SCAIO_IOC_Set_ConstantAlpha_Value(struct file *filp, unsigned long arg)
{
    SCA_CONSTANT_ALPHA_SETTING stCfg;
    if(copy_from_user(&stCfg, (SCA_CONSTANT_ALPHA_SETTING __user *)arg, sizeof(SCA_CONSTANT_ALPHA_SETTING)))
    {
        return -EFAULT;
    }

    if(!SCA_Set_ConstantAlpah_Value(&stCfg))
    {
        return -EFAULT;
    }
    return 0;
}

int _MDrv_SCAIO_IOC_GetLibVer(struct file *filp, unsigned long arg)
{
    SCA_LIB_VER stLibVer;

    if (!arg)
    {
       return -EFAULT;
    }

    if (!SCA_GetLibVer(&stLibVer))
    {
       return -EFAULT;
    }

    if (copy_to_user( (SCA_LIB_VER __user *)arg, &stLibVer, sizeof(SCA_LIB_VER) ))
    {
       return -EFAULT;
    }

    return 0;
}

int _MDrv_SCAIO_IOC_Set_MVOP_BaseAddr(struct file *filp, unsigned long arg)
{
    SCA_MVOP_BASE_ADDR_CONFIG stCfg;

    if(copy_from_user(&stCfg, (SCA_MVOP_BASE_ADDR_CONFIG __user *)arg, sizeof(SCA_MVOP_BASE_ADDR_CONFIG)))
    {
        return -EFAULT;
    }

    if(!SCA_Set_MVOP_BaseAddr(&stCfg))
    {
        return -EFAULT;
    }
    return 0;
}

int _MDrv_SCAIO_IOC_Get_MVOP_BaseAddr(struct file *filp, unsigned long arg)
{
    SCA_MVOP_BASE_ADDR_CONFIG stCfg;

    if(copy_from_user(&stCfg, (SCA_MVOP_BASE_ADDR_CONFIG __user *)arg, sizeof(SCA_MVOP_BASE_ADDR_CONFIG)))
    {
        return -EFAULT;
    }

    if(!SCA_Get_MVOP_BaseAddr(&stCfg))
    {
        return -EFAULT;
    }

    if(copy_to_user((SCA_MVOP_BASE_ADDR_CONFIG __user *)arg, &stCfg, sizeof(SCA_MVOP_BASE_ADDR_CONFIG)))
    {
        return -EFAULT;
    }

    return 0;
}

int _MDrv_SCAIO_IOC_Change_Window(struct file *filp, unsigned long arg)
{
    SCA_CHANGE_WINDOW_CONFIG stCfg;

    if(copy_from_user(&stCfg, (SCA_CHANGE_WINDOW_CONFIG __user *)arg, sizeof(SCA_CHANGE_WINDOW_CONFIG)))
    {
        return -EFAULT;
    }

    if(!SCA_Change_Window(&stCfg))
    {
        return -EFAULT;
    }

#if defined(CONFIG_MS_CLONE_SCREEN)
    if(g_enCloeScreenMode == SCA_CLONE_SCREEN_GOP)
    {
        if(!SCA_CloneScr_Change_Window(&stCfg))
        {
            return -EFAULT;
        }
    }
#endif

    return 0;
}

int _MDrv_SCAIO_IOC_Set_Picture(struct file *filp, unsigned long arg)
{
    SCA_SET_PICTURE_CONFIG stCfg;

    if(copy_from_user(&stCfg, (SCA_SET_PICTURE_CONFIG __user *)arg, sizeof(SCA_SET_PICTURE_CONFIG)))
    {
        return -EFAULT;
    }

    if(!SCA_Set_Picture(&stCfg))
    {
        return -EFAULT;
    }

    return 0;
}

int _MDrv_SCAIO_IOC_Get_Picture(struct file *filp, unsigned long arg)
{
    SCA_PICTURE_DATA stCfg;

    if(copy_from_user(&stCfg, (SCA_PICTURE_DATA __user *)arg, sizeof(SCA_PICTURE_DATA)))
    {
        return -EFAULT;
    }

    if(!SCA_Get_Picture(&stCfg))
    {
        return -EFAULT;
    }

    if(copy_to_user((SCA_PICTURE_DATA __user *)arg, &stCfg, sizeof(SCA_PICTURE_DATA)))
    {
        return -EFAULT;
    }
    return 0;
}

int _MDrv_SCAIO_IOC_Set_Output_Timing(struct file *filp, unsigned long arg)
{
    SCA_OUTPUT_TIMING_CONFIG stCfg;

    if(copy_from_user(&stCfg, (SCA_OUTPUT_TIMING_CONFIG __user *)arg, sizeof(SCA_OUTPUT_TIMING_CONFIG)))
    {
        return -EFAULT;
    }

    if(!SCA_Set_Output_Timing(&stCfg))
    {
        return -EFAULT;
    }

    return 0;
}

int _MDrv_SCAIO_IOC_Calibration(struct file *filp, unsigned long arg)
{
    SCA_CALIB_CONFIG stCfg;

    if(copy_from_user(&stCfg, (SCA_CALIB_CONFIG __user *)arg, sizeof(SCA_CALIB_CONFIG)))
    {
        return -EFAULT;
    }

    if(!SCA_Calibration(&stCfg.enSrcType, &stCfg.stCalibInfo))
    {
        return -EFAULT;
    }

    if(copy_to_user((SCA_CALIB_CONFIG __user *)arg, &stCfg, sizeof(SCA_CALIB_CONFIG)))
    {
        return -EFAULT;
    }
    return 0;
}

int _MDrv_SCAIO_IOC_Load_ADC_Setting(struct file *filp, unsigned long arg)
{
    SCA_ADC_CALIB_LOAD stCfg;

    if(copy_from_user(&stCfg, (SCA_ADC_CALIB_LOAD __user *)arg, sizeof(SCA_ADC_CALIB_LOAD)))
    {
        return -EFAULT;
    }

    if(!SCA_Load_ADC_Setting(&stCfg))
    {
        return -EFAULT;
    }

    if(copy_to_user((SCA_ADC_CALIB_LOAD __user *)arg, &stCfg, sizeof(SCA_ADC_CALIB_LOAD)))
    {
        return -EFAULT;
    }
    return 0;
}

int _MDrv_SCAIO_IOC_AVD_Connect(struct file *filp, unsigned long arg)
{
    AVD_CONNECT_CONFIG stCfg;
    if(copy_from_user(&stCfg, (AVD_CONNECT_CONFIG __user *)arg, sizeof(AVD_CONNECT_CONFIG)))
    {
        return -EFAULT;
    }

    if(!SCA_AVD_Connect(&stCfg))
    {
        return -EFAULT;
    }

    return 0;
}

int _MDrv_SCAIO_IOC_AVD_Check_VideoStd(struct file *filp, unsigned long arg)
{
    AVD_CHECK_VIDEOSTD_CONFIG stCfg;

    if(!SCA_AVD_Check_VideoStd(&stCfg))
    {
        return -EFAULT;
    }

    if(copy_to_user((AVD_CHECK_VIDEOSTD_CONFIG __user *)arg, &stCfg, sizeof(AVD_CHECK_VIDEOSTD_CONFIG)))
    {
        return -EFAULT;
    }

    return 0;
}

static void set_intr(SCA_DISP_INTR_CONFIG *config,
		MS_BOOL (*fp_isAttached)(SC_INT_SRC eIntNum, SC_InterruptCb pIntCb, void * pParam),
		MS_BOOL (*fp_Attach)(SC_INT_SRC eIntNum, SC_InterruptCb pIntCb, void * pParam),
		MS_BOOL (*fp_deAttach)(SC_INT_SRC eIntNum, SC_InterruptCb pIntCb, void * pParam)
		)
{

#define GET_INTR_CONFIG(IDX)  \
	(   (*((unsigned int *)(config))) & (1<<IDX) )

	unsigned intrIdx = 0;

	for(; intrIdx <32; intrIdx ++)
	{
		if(GET_INTR_CONFIG(intrIdx))// && false == fp_isAttached(intrIdx, NULL,NULL))
		{
			//printk(" SCA INTR %d \n", intrIdx);
			fp_Attach(intrIdx, NULL, NULL);
		}
		else if( !GET_INTR_CONFIG(intrIdx) )//&& true == fp_isAttached(intrIdx, NULL, NULL))
		{
			//printk(" SCA DE INTR %d \n", intrIdx);
			fp_deAttach(intrIdx, NULL, NULL);
		}
	}
}

int _MDrv_SCAIO_IOC_Set_Display_Interrupt(struct file *filp, unsigned long arg)
{
	int scaIdx = 0;
	SCA_DISP_INTR_SETTING stCfg;
    if(copy_from_user(&stCfg, (SCA_DISP_INTR_SETTING __user *)arg, sizeof(SCA_DISP_INTR_SETTING)))
    {
    	printk("SCA FAIL %d ...\n",__LINE__);
        return -EFAULT;
    }

    for(; scaIdx < SCA_NUM; scaIdx ++)
    {
    	switch(scaIdx)
    	{
    	case 0:
    		//printk("SCA DEBUG PATH 0\n");
    		set_intr(&stCfg.stIntrConfig[0], MDrv_XC_InterruptIsAttached, MDrv_XC_InterruptAttach, MDrv_XC_InterruptDeAttach);
    		break;
    	case 1:
    		//printk("SCA DEBUG PATH 1\n");
    		set_intr(&stCfg.stIntrConfig[1], MDrv_XC1_InterruptIsAttached, MDrv_XC1_InterruptAttach, MDrv_XC1_InterruptDeAttach);
    		break;
    	case 2:
    		//printk("SCA DEBUG PATH 2 \n");
    		set_intr(&stCfg.stIntrConfig[2], MDrv_XC2_InterruptIsAttached, MDrv_XC2_InterruptAttach, MDrv_XC2_InterruptDeAttach);
    		break;
    	default :
    		break;
    	}
    }

#if ENABLE_DIPW_INTR
    /*for dipw*/
	if(stCfg.bIntrConfig_DIPW == true && MDrv_DIPW_InterruptIsAttached(0, NULL, NULL) == false )
	{
		MDrv_DIPW_InterruptAttach(0, NULL, NULL);
    }
	else if(stCfg.bIntrConfig_DIPW == false && MDrv_DIPW_InterruptIsAttached(0, NULL ,NULL) == true )
	{
		MDrv_DIPW_InterruptDeAttach(0, NULL, NULL);
    }
    MDrv_DIPW_Set_Signal_PID(stCfg.u32DIPW_Signal_PID);
#endif

    memcpy(&g_stDispIntrSetting, &stCfg, sizeof(g_stDispIntrSetting));
    return 0;
}

int _MDrv_SCAIO_IO_Get_Display_Interrupt(struct file *filp, unsigned long arg)
{
    if(copy_to_user((SCA_DISP_INTR_SETTING __user *)arg, &g_stDispIntrSetting, sizeof(g_stDispIntrSetting)))
    {
    	printk("SCA FAIL : %d \n",__LINE__);
        return -EFAULT;
    }
    return 0;
}

int _MDrv_SCAIO_IO_Get_Display_Interrupt_Status(struct file *filp, unsigned long arg)
{
    SCA_DISP_INTR_STATUS stIntrStatus;

    memset(&stIntrStatus, 0, sizeof(SCA_DISP_INTR_STATUS));
    stIntrStatus.u8DIPW_Status = MDrv_DIPW_Get_ISR_RecFlag();

     if(copy_to_user((SCA_DISP_INTR_STATUS __user *)arg, &stIntrStatus, sizeof(SCA_DISP_INTR_STATUS)))
    {
    	printk("SCA FAIL : %d \n",__LINE__);
        return -EFAULT;
    }
    return 0;
}



int _MDrv_SCAIO_IOC_RW_Register(struct file *filp, unsigned long arg)
{
    SCA_RW_REGISTER_CONFIG stCfg;
    int ret  = 0;

    if(copy_from_user(&stCfg, (SCA_RW_REGISTER_CONFIG __user *)arg, sizeof(SCA_RW_REGISTER_CONFIG)))
    {
        return -EFAULT;
    }

    if(!SCA_RW_Register(&stCfg))
    {
        ret = -EFAULT;
    }

    if(stCfg.enType == SCA_REG_R_BYTE || stCfg.enType == SCA_REG_XC_R2BYTEMSK)
    {
        if(copy_to_user((SCA_RW_REGISTER_CONFIG __user *)arg, &stCfg, sizeof(SCA_RW_REGISTER_CONFIG)))
        {
            return -EFAULT;
        }
    }

    return ret;
}

int _MDrv_SCAIO_IOC_Get_ACE_Info(struct file *filp, unsigned long arg)
{
    SCA_ACE_INFO_CONFIG stCfg;
    int ret  = 0;

    if(copy_from_user(&stCfg, (SCA_ACE_INFO_CONFIG __user *)arg, sizeof(SCA_ACE_INFO_CONFIG)))
    {
        return -EFAULT;
    }

    if(!SCA_Get_ACE_Info(&stCfg))
    {
        ret = -EFAULT;
    }

    if(copy_to_user((SCA_ACE_INFO_CONFIG __user *)arg, &stCfg, sizeof(SCA_ACE_INFO_CONFIG)))
    {
        return -EFAULT;
    }
    return ret;
}


int _MDrv_SCAIO_IOC_Get_DLC_Info(struct file *filp, unsigned long arg)
{
    SCA_DLC_INFO_CONFIG stCfg;
    int ret  = 0;


    if(copy_from_user(&stCfg, (SCA_DLC_INFO_CONFIG __user *)arg, sizeof(SCA_DLC_INFO_CONFIG)))
    {
        SCA_DBG(SCA_DBG_LV_0,"Get DLC Param Fail \n");
        return -EFAULT;
    }

    if(stCfg.u16CmdBufLen > 1024)
    {
        SCA_DBG(SCA_DBG_LV_0, "Get DLC Info: Cmd Buffer overflow");
        return -EFAULT;
    }

    if(stCfg.u32CmdBufAddr && stCfg.u16CmdBufLen)
    {
        if(copy_from_user(gu8DLC_CmdBuf, (void __user*)stCfg.u32CmdBufAddr, stCfg.u16CmdBufLen))
        {
            SCA_DBG(SCA_DBG_LV_0,"copy DLC Cmd Buf error \n");
            return -EFAULT;
        }
    }

    SCA_Get_DLC_INFO(&stCfg);

    if(stCfg.u16DataLen)
    {
        if(stCfg.u16DataLen > 1024)
        {
            SCA_DBG(SCA_DBG_LV_0, "Get DLC Info: buffer overflow");
        }

        if(copy_to_user((SCA_DLC_INFO_CONFIG __user *)arg, &stCfg, sizeof(SCA_DLC_INFO_CONFIG)))
        {
            return -EFAULT;
        }

        if(copy_to_user((void __user*)stCfg.u32CmdBufAddr, gu8DLC_DataBuf, stCfg.u16DataLen))
        {
            SCA_DBG(SCA_DBG_LV_0,"copy DLC Data Buf error \n");
            return -EFAULT;
        }
    }


    return ret;
}

int _MDrv_SCAIO_IOC_Get_Disp_Path_Config(struct file *filp, unsigned long arg)
{
    int ret  = 0;
    SCA_DISP_PATH_CONFIG stCfg;

    if(copy_from_user(&stCfg, (SCA_DISP_PATH_CONFIG __user *)arg, sizeof(SCA_DISP_PATH_CONFIG)))
    {
        return -EFAULT;
    }

    if(!SCA_Get_Disp_Path_Config(&stCfg))
    {
        ret = -EFAULT;
    }

    if(copy_to_user((SCA_DISP_PATH_CONFIG __user *)arg, &stCfg, sizeof(SCA_DISP_PATH_CONFIG)))
    {
        return -EFAULT;
    }

    return ret;
}

int _MDrv_SCAIO_IOC_Set_Mirror_Config(struct file *filp, unsigned long arg)
{
    int ret  = 0;
    SCA_MIRROR_CONFIG stCfg;

    if(copy_from_user(&stCfg, (SCA_MIRROR_CONFIG __user *)arg, sizeof(SCA_MIRROR_CONFIG)))
    {
        return -EFAULT;
    }

    if(!SCA_Set_Mirror_Config(&stCfg))
    {
        ret = -EFAULT;
    }

    return ret;
}

int _MDrv_SCAIO_IOC_Set_PQ_Bin(struct file *filp, unsigned long arg)
{
    int ret  = 0;
    SCA_PQ_BIN_CONFIG stCfg;

    if(copy_from_user(&stCfg, (SCA_PQ_BIN_CONFIG __user *)arg, sizeof(SCA_PQ_BIN_CONFIG)))
    {
        return -EFAULT;
    }


    if(!SCA_Set_PQ_Bin(&stCfg))
    {
        ret = -EFAULT;
    }

    return ret;
}

int _MDrv_SCAIO_IOC_Set_Dip_WOnce_Base(struct file *filp, unsigned long arg)
{
    int ret  = 0;
    SCA_DIP_WONCE_BASE_CONFIG stCfg;

    if(copy_from_user(&stCfg, (SCA_DIP_WONCE_BASE_CONFIG __user *)arg, sizeof(SCA_DIP_WONCE_BASE_CONFIG)))
    {
        return -EFAULT;
    }

    if(!SCA_Set_Dip_WOnce_Base(&stCfg))
    {
        ret = -EFAULT;
    }

    return ret;
}

int _MDrv_SCAIO_IOC_Set_Dip_Config(struct file *filp, unsigned long arg)
{
    int ret  = 0;
    SCA_DIP_CONFIG stCfg;

    if(copy_from_user(&stCfg, (SCA_DIP_CONFIG __user *)arg, sizeof(SCA_DIP_CONFIG)))
    {
        return -EFAULT;
    }

    if(!SCA_Set_Dip_Config(&stCfg))
    {
        ret = -EFAULT;
    }
    return ret;
}

int _MDrv_SCAIO_IOC_Set_HDMITX_Config(struct file *filp, unsigned long arg)
{
    int ret  = 0;
    SCA_HDMITX_CONFIG stCfg;

    if(copy_from_user(&stCfg, (SCA_HDMITX_CONFIG __user *)arg, sizeof(SCA_HDMITX_CONFIG)))
    {
        return -EFAULT;
    }

    if(!SCA_Set_HDMITX_Config(&stCfg))
    {
        ret = -EFAULT;
    }
    return ret;

}

int _MDrv_SCAIO_IOC_Set_Freeze_Config(struct file *filp, unsigned long arg)
{
    int ret  = 0;
    SCA_FREEZE_CONFIG stCfg;
    if(copy_from_user(&stCfg, (SCA_FREEZE_CONFIG __user *)arg, sizeof(SCA_FREEZE_CONFIG)))
    {
        return -EFAULT;
    }

    if(!SCA_Set_Freeze_Config(&stCfg))
    {
        ret = -EFAULT;
    }

    return ret;
}

int _MDrv_SCAIO_IOC_Init_MST701(struct file *filp, unsigned long arg)
{
#if defined(CONFIG_MS_MST701)
    int ret  = 0;
    SCA_MST701_CONFIG stCfg;

    if(copy_from_user(&stCfg, (SCA_MST701_CONFIG __user *)arg, sizeof(SCA_MST701_CONFIG)))
    {
        return -EFAULT;
    }

    if(!SCA_Init_MST701(&stCfg))
    {
        ret = -EFAULT;
    }

    return ret;
#else
    return -EFAULT;
#endif
}


int _MDrv_SCAIO_IOC_Geometry_Calibration(struct file *filp, unsigned long arg)
{
    int ret  = 0;
    SCA_GEOMETRY_CALI_CONFIG stCfg;

    if(copy_from_user(&stCfg, (SCA_GEOMETRY_CALI_CONFIG __user *)arg, sizeof(SCA_GEOMETRY_CALI_CONFIG)))
    {
        return -EFAULT;
    }

    if(!SCA_Geometry_Calibration(&stCfg))
    {
        ret = -EFAULT;
    }

    if(copy_to_user((SCA_GEOMETRY_CALI_CONFIG __user *)arg, &stCfg, sizeof(SCA_GEOMETRY_CALI_CONFIG)))
    {
        return -EFAULT;
    }

    return ret;
}

int _MDrv_SCAIO_IOC_Load_Geometry_Setting(struct file *filp, unsigned long arg)
{
    int ret  = 0;
    SCA_GEOMETRY_CALI_CONFIG stCfg;

    if(copy_from_user(&stCfg, (SCA_GEOMETRY_CALI_CONFIG __user *)arg, sizeof(SCA_GEOMETRY_CALI_CONFIG)))
    {
        return -EFAULT;
    }

    if(!SCA_Load_Geometry_Setting(&stCfg))
    {
        ret = -EFAULT;
    }


    return ret;
}

int _MDrv_SCAIO_IOC_Set_Analog_Polling_Config(struct file *filp, unsigned long arg)
{
    int ret  = 0;
    SCA_ANALOG_POLLING_CONFIG stCfg;

    if(copy_from_user(&stCfg, (SCA_ANALOG_POLLING_CONFIG __user *)arg, sizeof(SCA_ANALOG_POLLING_CONFIG)))
    {
        return -EFAULT;
    }

    if(!SCA_Set_Analog_Polling_Config(&stCfg))
    {
        ret = -EFAULT;
    }


    return ret;
}


int _MDrv_SCAIO_IOC_Set_Clone_Screen_Config(struct file *filp, unsigned long arg)
{
#if defined(CONFIG_MS_CLONE_SCREEN)
    int ret  = 0;
    SCA_CLONE_SCREEN_CONFIG stCfg;

    if(copy_from_user(&stCfg, (SCA_CLONE_SCREEN_CONFIG __user *)arg, sizeof(SCA_CLONE_SCREEN_CONFIG)))
    {
        return -EFAULT;
    }

    if(!SCA_Set_Clone_Screen_Config(&stCfg))
    {
        ret = -EFAULT;
    }
    return ret;
#else

    return 0;
#endif
}


int _MDrv_SCAIO_IOC_Get_Clone_Screen_Config(struct file *filp, unsigned long arg)
{
#if defined(CONFIG_MS_CLONE_SCREEN)
    int ret  = 0;
    SCA_CLONE_SCREEN_CONFIG stCfg;


    if(!SCA_Get_Clone_Screen_Config(&stCfg))
    {
        ret = -EFAULT;
    }

    if(copy_to_user((SCA_CLONE_SCREEN_CONFIG __user *)arg, &stCfg, sizeof(SCA_CLONE_SCREEN_CONFIG)))
    {
        return -EFAULT;
    }

    return ret;
#else
    return 0;
#endif
}

int _MDrv_SCAIO_IOC_Set_Pnl_SSC_Config(struct file *filp, unsigned long arg)
{
    int ret  = 0;
    SCA_PNL_SSC_CONFIG stCfg;

    if(copy_from_user(&stCfg, (SCA_PNL_SSC_CONFIG __user *)arg, sizeof(SCA_PNL_SSC_CONFIG)))
    {
        return -EFAULT;
    }

    if(!SCA_Set_Pnl_SSC_Config(&stCfg))
    {
        ret = -EFAULT;
    }

    return ret;
}

int _MDrv_SCAIO_IOC_Set_Pnl_Timing_Config(struct file *filp, unsigned long arg)
{
    int ret  = 0;
    SCA_PNL_TIMING_CONFIG stCfg;

    if(copy_from_user(&stCfg, (SCA_PNL_TIMING_CONFIG __user *)arg, sizeof(SCA_PNL_TIMING_CONFIG)))
    {
        return -EFAULT;
    }

    if(!SCA_Set_Pnl_Timing_Config(&stCfg))
    {
        ret = -EFAULT;
    }

    return ret;
}

int _MDrv_SCAIO_IOC_Get_Monitor_Status_Config(struct file *filp, unsigned long arg)
{
    int ret  = 0;
    SCA_MONITOR_STATUS_CONFIG stCfg;

    if(copy_from_user(&stCfg, (SCA_MONITOR_STATUS_CONFIG __user *)arg, sizeof(SCA_MONITOR_STATUS_CONFIG)))
    {
        return -EFAULT;
    }

    if(!SCA_Get_Monitor_Status_Config(&stCfg))
    {
        ret = -EFAULT;
    }

    if(copy_to_user((SCA_MONITOR_STATUS_CONFIG __user *)arg, &stCfg, sizeof(SCA_MONITOR_STATUS_CONFIG)))
    {
        return -EFAULT;
    }
    return ret;
}

int _MDrv_SCAIO_IOC_Set_User_Display_Config(struct file *filp, unsigned long arg)
{
    int ret  = 0;
    SCA_USER_DISPLAY_CONFIG stCfg;

    if(copy_from_user(&stCfg, (SCA_USER_DISPLAY_CONFIG __user *)arg, sizeof(SCA_USER_DISPLAY_CONFIG)))
    {
        return -EFAULT;
    }

    if(!SCA_Set_User_Display_Config(&stCfg))
    {
        return -EFAULT;
    }

    return ret;
}

int _MDrv_SCAIO_IOC_Set_User_Display_Config_EX(struct file *filp, unsigned long arg)
{
    int ret  = 0;
    SCA_USER_DISPLAY_CONFIG_EX stCfg;

    if(copy_from_user(&stCfg, (SCA_USER_DISPLAY_CONFIG_EX __user *)arg, sizeof(SCA_USER_DISPLAY_CONFIG_EX)))
    {
        return -EFAULT;
    }

    if(!SCA_Set_User_Display_Config_EX(&stCfg))
    {
        return -EFAULT;
    }

    return ret;
}

int _MDrv_SCAIO_IOC_Set_Display_Mute_Color_Config(struct file *filp, unsigned long arg)
{
    int ret  = 0;
    SCA_DISPLAY_MUTE_COLOR_CONFIG stCfg;

    if(copy_from_user(&stCfg, (SCA_DISPLAY_MUTE_COLOR_CONFIG __user *)arg, sizeof(SCA_DISPLAY_MUTE_COLOR_CONFIG)))
    {
        return -EFAULT;
    }

    if(!SCA_Set_DisplayMute_Color_Config(&stCfg))
    {
        return -EFAULT;
    }

    return ret;
}


int _MDrv_SCAIO_IOC_Set_Clone_Screen_Ratio_Config(struct file *filp, unsigned long arg)
{
#if defined(CONFIG_MS_CLONE_SCREEN)
    int ret  = 0;
    SCA_CLONE_SCREEN_DISPLAY_RATIO_CONFIG stCfg;

    if(copy_from_user(&stCfg, (SCA_CLONE_SCREEN_DISPLAY_RATIO_CONFIG __user *)arg, sizeof(SCA_CLONE_SCREEN_DISPLAY_RATIO_CONFIG)))
    {
        return -EFAULT;
    }

    if(stCfg.bEn)
    {
        SCA_CloneScr_SetDisplayRatio(stCfg.u16H_ratio, stCfg.u16V_ratio);
    }
    else
    {
        SCA_CloneScr_SetDisplayRatio(0, 0);
    }

    return ret;
#else
    return 0;
#endif
}

int _MDrv_SCAIO_IOC_Set_DLC_Init_Config(struct file *filp, unsigned long arg)
{
    int ret  = 0;
    SCA_DLC_INIT_CONFIG stCfg;

    if(copy_from_user(&stCfg, (SCA_DLC_INIT_CONFIG __user *)arg, sizeof(SCA_DLC_INIT_CONFIG)))
    {
        return -EFAULT;
    }


    if(!SCA_Set_DLC_Init_Config(&stCfg))
    {
        return -EFAULT;
    }


    return ret;
}

int _MDrv_SCAIO_IOC_Set_DLC_OnOff_Config(struct file *filp, unsigned long arg)
{
    int ret  = 0;
    SCA_DLC_ONOFF_CONFIG stCfg;

    if(copy_from_user(&stCfg, (SCA_DLC_ONOFF_CONFIG __user *)arg, sizeof(SCA_DLC_ONOFF_CONFIG)))
    {
        return -EFAULT;
    }


    if(!SCA_Set_DLC_OnOff_Config(&stCfg))
    {
        return -EFAULT;
    }


    return ret;

}


int _MDrv_SCAIO_IOC_Set_UEvent_Config(struct file *filp, unsigned long arg)
{
#if defined(SCA_ENABLE_HDMITX_HPD)

    int ret  = 0;
    SCA_UEVENT_CONFIG stCfg;

    if(copy_from_user(&stCfg, (SCA_UEVENT_CONFIG __user *)arg, sizeof(SCA_UEVENT_CONFIG)))
    {
        return -EFAULT;
    }

    if(stCfg.bEn)
    {
        if(stCfg.u16Size)
        {
            if(SCA_Set_UEvent_Broadcast(stCfg.u16Size, stCfg.u8Msg) == FALSE)
            {
                ret = -EFAULT;
            }
        }

        if(pHDMITxHPDThread == NULL)
        {
            SCA_Init_HDMI_HPD_Thread();

            if(pHDMITxHPDThread == NULL)
            {
                ret = -EFAULT;
            }
        }

    }
    else
    {
        if(pHDMITxHPDThread)
        {
            kthread_stop(pHDMITxHPDThread);
            pHDMITxHPDThread = NULL;
        }

    }
    return ret;

#else

    return -EFAULT;

#endif

}


int _MDrv_SCAIO_IOC_Set_CVBSOUT_DAC_Config(struct file *filp, unsigned long arg)
{
    int ret  = 0;
    SCA_CVBSOUT_DAC_CONFIG stCfg;

    if(copy_from_user(&stCfg, (SCA_CVBSOUT_DAC_CONFIG __user *)arg, sizeof(SCA_CVBSOUT_DAC_CONFIG)))
    {
        return -EFAULT;
    }


    if(!SCA_Set_CVBSOUT_DAC_Config(&stCfg))
    {
        return -EFAULT;
    }

    return ret;
}


int _MDrv_SCAIO_IOC_Set_Camera_InputTiming_Config(struct file *filp, unsigned long arg)
{
    int ret  = 0;
    SCA_CAMERA_INPUTTIMING_CONFIG stCfg;

    if(copy_from_user(&stCfg, (SCA_CAMERA_INPUTTIMING_CONFIG __user *)arg, sizeof(SCA_CAMERA_INPUTTIMING_CONFIG)))
    {
        return -EFAULT;
    }

    if(!SCA_Set_Camera_InputTiming_Config(&stCfg))
    {
        return -EFAULT;
    }

    return ret;
}
#ifdef __BOOT_PNL__//paul_test

int _MDrv_SCAIO_IOC_Set_PQ_Bin_ICC(struct file *filp, unsigned long arg)
{
    int ret  = 0;
    SCA_PQ_ICC_CFG stCfg;

    if(copy_from_user(&stCfg, (SCA_PQ_ICC_CFG __user *)arg, sizeof(SCA_PQ_ICC_CFG)))
    {
    printk("[SCA] copy IBC fail \r\n");
        return -EFAULT;
    }


    if(!SCA_Set_PQ_Bin_ICC(&stCfg))
    {
    printk("[SCA] go IBC fail \r\n");
        ret = -EFAULT;
    }

    return ret;
}

int _MDrv_SCAIO_IOC_Set_PQ_Bin_IBC(struct file *filp, unsigned long arg)
{
    int ret  = 0;
    SCA_PQ_IBC_CFG stCfg;

    if(copy_from_user(&stCfg, (SCA_PQ_IBC_CFG __user *)arg, sizeof(SCA_PQ_IBC_CFG)))
    {
    printk("[SCA] copy IBC fail \r\n");
        return -EFAULT;
    }


    if(!SCA_Set_PQ_Bin_IBC(&stCfg))
    {
    printk("[SCA] go IBC fail \r\n");
        ret = -EFAULT;
    }

    return ret;
}

int _MDrv_SCAIO_IOC_Set_PQ_Bin_IHC(struct file *filp, unsigned long arg)
{
    int ret  = 0;
    SCA_PQ_IHC_CFG stCfg;

    if(copy_from_user(&stCfg, (SCA_PQ_IHC_CFG __user *)arg, sizeof(SCA_PQ_IHC_CFG)))
    {
        return -EFAULT;
        printk("[SCA] copy IBC fail \r\n");
    }


    if(!SCA_Set_PQ_Bin_IHC(&stCfg))
    {
        printk("[SCA] copy IBC fail \r\n");
        ret = -EFAULT;
    }

    return ret;
}





#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
#define BHALF_TYPE_NONE 0
#define BHALF_TYPE_TASKLET 1
#define BHALF_TYPE_WKQ 2
#define BHALF_TYPE BHALF_TYPE_NONE
void sca_vsync_bhalf(unsigned long);
#if (BHALF_TYPE == BHALF_TYPE_TASKLET)
void sca_vsync_tasklet(unsigned long)
{

}
DECLARE_TASKLET(sca_vsync_tasklet, sca_vsync_bhalf, 0);

#elif( BHALF_TYPE == BHALF_TYPE_WKQ)
struct work_struct sca_vsnc_wkq;

#endif


int mstar_sca_drv_open(struct inode *inode, struct file *filp)
{
    SCA_DBG(SCA_DBG_LV_IOCTL, "[SCA] SCA DRIVER OPEN\n");

    SCAIO_ASSERT(_devSCA.refCnt>=0);
    _devSCA.refCnt++;


    if(!IsRTKCvbsInputLocked() && mstar_sca_is_alive() )
    {
        if(!IsRTKCarBacking())
        {
            SCA_WAIT_MUTEX(_SCA_XC_AVD_Init_Mutex);

        	if ( !MApi_XC_GetSysInitFlag() )
        	{
        	    SCA_DBG(SCA_DBG_LV_0, " MApi_XC_Sys_Init in SCA OPEN\n");

            	MApi_XC_Sys_Init(gstInitInfo);
        	}

            SCA_RELEASE_MUTEX(_SCA_XC_AVD_Init_Mutex);
        }


        SCA_WAIT_MUTEX(_SCA_XC_AVD_Init_Mutex);
    	if ( !MApi_AVD_Sys_ReInit() )
    	{
            SCA_DBG(SCA_DBG_LV_0, "ReInit AVD Fail \n");
    	}
        SCA_RELEASE_MUTEX(_SCA_XC_AVD_Init_Mutex);
	}
    return 0;
}

//for kernel use only
int mstar_sca_drv_open_export(void)
{
    SCA_DBG(SCA_DBG_LV_IOCTL, "[SCA] SCA DRIVER OPEN export\n");

    return mstar_sca_drv_open(NULL, NULL);
}

int mstar_sca_drv_release(struct inode *inode, struct file *filp)
{

    SCA_DBG(SCA_DBG_LV_IOCTL, "[SCA] SCA DRIVER RELEASE\n");
    _devSCA.refCnt--;
    SCAIO_ASSERT(_devSCA.refCnt>=0);

    //free_irq(INT_IRQ_DISP , MDrv_SC_isr);
    //free_irq(INT_IRQ_DISP1 , MDrv_SC1_isr);
    //free_irq(INT_IRQ_DISP2 , MDrv_SC2_isr);

#if ENABLE_DIPW_INTR
    free_irq(INT_IRQ_DIPW, MDrv_DIPW_isr);
#endif

    return 0;
}

long mstar_sca_drv_ioctl(struct file *filp, unsigned int u32Cmd, unsigned long u32Arg)
{
    int err = 0;
    int retval = 0;

    if(_devSCA.refCnt <= 0)
    {
        SCA_DBG(SCA_DBG_LV_IOCTL, "[SCA] SCAIO_IOCTL refCnt =%d!!! \n", _devSCA.refCnt);
        return -EFAULT;
    }
    /* check u32Cmd valid */
    if(IOCTL_SCA_MAGIC == _IOC_TYPE(u32Cmd))
    {
        if(_IOC_NR(u32Cmd) >= IOCTL_SCA_MAX_NR)
        {
            SCA_DBG(SCA_DBG_LV_0, "[SCA] IOCtl NR Error!!! (Cmd=%x)\n",u32Cmd);
            return -ENOTTY;
        }
    }
    else
    {
        SCA_DBG(SCA_DBG_LV_0, "[SCA] IOCtl MAGIC Error!!! (Cmd=%x)\n",u32Cmd);
        return -ENOTTY;
    }

    /* verify Access */
    if (_IOC_DIR(u32Cmd) & _IOC_READ)
    {
        err = !access_ok(VERIFY_WRITE, (void __user *)u32Arg, _IOC_SIZE(u32Cmd));
    }
    else if (_IOC_DIR(u32Cmd) & _IOC_WRITE)
    {
        err =  !access_ok(VERIFY_READ, (void __user *)u32Arg, _IOC_SIZE(u32Cmd));
    }
    if (err)
    {
        return -EFAULT;
    }
	/* not allow query or command once driver suspend */
	if ( !mstar_sca_is_alive() )
    {
		printk("[SCA] scaler suspend: ignore IOCTL commands \r\n");
        return -EFAULT;
    }

    SCA_WAIT_MUTEX(_SCA_XC_AVD_Init_Mutex);
	if ( !MApi_XC_GetSysInitFlag() )
	{
	    SCA_DBG(SCA_DBG_LV_0, " MApi_XC_Sys_Init in SCA IOCTL\n");

    	MApi_XC_Sys_Init(gstInitInfo);
	}

    MApi_AVD_Sys_ReInit();
    SCA_RELEASE_MUTEX(_SCA_XC_AVD_Init_Mutex);


	SCA_WAIT_MUTEX(_SCA_PowerState_Mutex);

	if ( !mstar_sca_is_alive() )
    {
    	SCA_RELEASE_MUTEX(_SCA_PowerState_Mutex);
		printk("[SCA] not allow IOCTLs once suspend \r\n");
        return -EFAULT;
    }

    /* handle u32Cmd */
#if SCA_DBG_LV_IOCTL
    if(u32Cmd != IOCTL_SCA_GET_DISP_PATH_CONFIG &&
       u32Cmd != IOCTL_SCA_GET_MVOP_BASEADDR &&
       u32Cmd != IOCTL_SCA_GET_CLONE_SCREEN_CONFIG &&
       u32Cmd != IOCTL_SCA_YPBPR_VGA_MODE_MON_PAR)
    {
        SCA_DBG(SCA_DBG_LV_IOCTL, "[SCA] SCAIO_IOCTL %s!!! \n", (CMD_PARSING(u32Cmd)));
    }
#endif
    switch(u32Cmd)
    {
    case IOCTL_SCA_CONNECT:
        retval = _MDrv_SCAIO_IOC_Connect(filp, u32Arg);
        break;

    case IOCTL_SCA_DISCONNECT:
        retval = _MDrv_SCAIO_IOC_DisConnect(filp, u32Arg);
        break;

    case IOCTL_SCA_SET_TIMING_WINDOW:
        retval = _MDrv_SCAIO_IOC_Set_Timing_Window(filp, u32Arg);
        break;

    case IOCTL_SCA_SET_MVOP:
        retval = _MDrv_SCAIO_IOC_Set_MVOP(filp, u32Arg);
        break;

    case IOCTL_SCA_SET_DISPLAY_MUTE:
        retval = _MDrv_SCAIO_IOC_Set_Display_Mute(filp, u32Arg);
        break;

    case IOCTL_SCA_YPBPR_VGA_MODE_MON_PAR:
        retval = _MDrv_SCAIO_IOC_YPBPR_VGA_Mode_Monitor_Parse(filp, u32Arg);
        break;

    case IOCTL_SCA_GET_MODE:
        retval = _MDrv_SCAIO_IOC_Get_Mode(filp, u32Arg);
        break;

    case IOCTL_SCA_SET_VE:
        retval = _MDrv_SCAIO_IOC_Set_VE(filp, u32Arg);
        break;

    case IOCTL_SCA_SET_WINDOW_ONOFF:
        retval = _MDrv_SCAIO_IOC_Set_Window_OnOFF(filp, u32Arg);
        break;

    case IOCTL_SCA_SET_COLOR_KEY:
        retval = _MDrv_SCAIO_IOC_Set_ColorKey(filp, u32Arg);
        break;

    case IOCTL_SCA_SET_CONSTANTALPHA_STATE:
        retval = _MDrv_SCAIO_IOC_Set_ConstantAlpha_State(filp, u32Arg);
        break;


    case IOCTL_SCA_SET_MVOP_BASEADDR:
        retval = _MDrv_SCAIO_IOC_Set_MVOP_BaseAddr(filp, u32Arg);
        break;

    case IOCTL_SCA_GET_MVOP_BASEADDR:
        retval = _MDrv_SCAIO_IOC_Get_MVOP_BaseAddr(filp, u32Arg);
        break;

    case IOCTL_SCA_CHANGE_WINDOW:
        retval = _MDrv_SCAIO_IOC_Change_Window(filp, u32Arg);
        break;

    case IOCTL_SCA_SET_PICTURE:
        retval = _MDrv_SCAIO_IOC_Set_Picture(filp, u32Arg);
        break;

    case IOCTL_SCA_GET_PICTURE:
        retval = _MDrv_SCAIO_IOC_Get_Picture(filp, u32Arg);
        break;

    case IOCTL_SCA_SET_OUTPUT_TIMING:
        retval = _MDrv_SCAIO_IOC_Set_Output_Timing(filp, u32Arg);
        break;

    case IOCTL_SCA_CALIBRATION:
        retval = _MDrv_SCAIO_IOC_Calibration(filp, u32Arg);
        break;

    case IOCTL_SCA_LOAD_ADC_SETTING:
        retval = _MDrv_SCAIO_IOC_Load_ADC_Setting(filp, u32Arg);
        break;

    case IOCTL_SCA_AVD_CONNECT:
        retval = _MDrv_SCAIO_IOC_AVD_Connect(filp, u32Arg);
        break;

    case IOCTL_SCA_AVD_CHECK_VIDEO_STD:
        retval = _MDrv_SCAIO_IOC_AVD_Check_VideoStd(filp, u32Arg);
        break;

    case IOCTL_SCA_SET_CONSTANTALPHA_VALUE:
        retval = _MDrv_SCAIO_IOC_Set_ConstantAlpha_Value(filp, u32Arg);
        break;
    case IOCTL_SCA_SET_DISP_INTR:
    	retval = _MDrv_SCAIO_IOC_Set_Display_Interrupt(filp, u32Arg);
    	break;
    case IOCTL_SCA_GET_DISP_INTR:
    	retval = _MDrv_SCAIO_IO_Get_Display_Interrupt(filp, u32Arg);
    	break;
    case IOCTL_SCA_GET_LIB_VER:
        retval = _MDrv_SCAIO_IOC_GetLibVer(filp, u32Arg);
        break;

    case IOCTL_SCA_RW_REGISTER:
        retval = _MDrv_SCAIO_IOC_RW_Register(filp, u32Arg);
        break;

    case IOCTL_SCA_GET_ACE_INFO:
        retval = _MDrv_SCAIO_IOC_Get_ACE_Info(filp, u32Arg);
        break;

    case IOCTL_SCA_GET_DLC_INFO:
        retval = _MDrv_SCAIO_IOC_Get_DLC_Info(filp, u32Arg);
        break;

    case IOCTL_SCA_GET_DISP_PATH_CONFIG:
        retval = _MDrv_SCAIO_IOC_Get_Disp_Path_Config(filp, u32Arg);
        break;

    case IOCTL_SCA_SET_PQ_BIN:
        retval = _MDrv_SCAIO_IOC_Set_PQ_Bin(filp, u32Arg);
        break;
#ifdef __BOOT_PNL__//paul_test
    case IOCTL_SCA_SET_PQ_BIN_IBC:
        retval = _MDrv_SCAIO_IOC_Set_PQ_Bin_IBC(filp, u32Arg);
        break;
    case IOCTL_SCA_SET_PQ_BIN_ICC:
        retval = _MDrv_SCAIO_IOC_Set_PQ_Bin_ICC(filp, u32Arg);
        break;
    case IOCTL_SCA_SET_PQ_BIN_IHC:
        retval = _MDrv_SCAIO_IOC_Set_PQ_Bin_IHC(filp, u32Arg);
        break;
#endif

    case IOCTL_SCA_SET_MIRROR_CONFIG:
        retval = _MDrv_SCAIO_IOC_Set_Mirror_Config(filp, u32Arg);
        break;

    case IOCTL_SCA_SET_DIP_CONFIG:
        retval = _MDrv_SCAIO_IOC_Set_Dip_Config(filp, u32Arg);
        break;

    case IOCTL_SCA_GET_DISP_INTR_STATUS:
        retval = _MDrv_SCAIO_IO_Get_Display_Interrupt_Status(filp, u32Arg);
        break;

    case IOCTL_SCA_SET_HDMITX_CONFIG:
        retval = _MDrv_SCAIO_IOC_Set_HDMITX_Config(filp, u32Arg);
        break;

    case IOCTL_SCA_SET_DIP_WONCE_BASE_CONFIG:
        retval = _MDrv_SCAIO_IOC_Set_Dip_WOnce_Base(filp, u32Arg);
        break;

    case IOCTL_SCA_SET_FREEZE_CONFIG:
        retval = _MDrv_SCAIO_IOC_Set_Freeze_Config(filp, u32Arg);
        break;

	case IOCTL_SCA_INIT_MST701:
        retval = _MDrv_SCAIO_IOC_Init_MST701(filp, u32Arg);
        break;

    case IOCTL_SCA_GEOMETRY_CALIBRATION:
        retval = _MDrv_SCAIO_IOC_Geometry_Calibration(filp, u32Arg);
        break;

    case IOCTL_SCA_LOAD_GEOMETRY_SETTING:
        retval = _MDrv_SCAIO_IOC_Load_Geometry_Setting(filp, u32Arg);
        break;

    case IOCTL_SCA_SET_ANALOG_POLLING_CONFIG:
        retval = _MDrv_SCAIO_IOC_Set_Analog_Polling_Config(filp, u32Arg);
        break;

    case IOCTL_SCA_SET_CLONE_SCREEN_CONFIG:
        retval = _MDrv_SCAIO_IOC_Set_Clone_Screen_Config(filp, u32Arg);
        break;

    case IOCTL_SCA_GET_CLONE_SCREEN_CONFIG:
        retval = _MDrv_SCAIO_IOC_Get_Clone_Screen_Config(filp, u32Arg);
        break;

    case IOCTL_SCA_SET_PNL_SSC_CONFIG:
        retval = _MDrv_SCAIO_IOC_Set_Pnl_SSC_Config(filp, u32Arg);
        break;

    case IOCTL_SCA_SET_PNL_TIMING_CONFIG:
        retval = _MDrv_SCAIO_IOC_Set_Pnl_Timing_Config(filp, u32Arg);
        break;

    case IOCTL_SCA_GET_MONITOR_STATUS_CONFIG:
        retval = _MDrv_SCAIO_IOC_Get_Monitor_Status_Config(filp, u32Arg);
        break;

    case IOCTL_SCA_SET_USER_DISPLAY_CONFIG:
        retval = _MDrv_SCAIO_IOC_Set_User_Display_Config(filp, u32Arg);
        break;

    case IOCTL_SCA_SET_USER_DISPLAY_CONFIG_EX:
        retval = _MDrv_SCAIO_IOC_Set_User_Display_Config_EX(filp, u32Arg);
        break;

    case IOCTL_SCA_SET_DISPLAY_MUTE_COLOR:
        retval = _MDrv_SCAIO_IOC_Set_Display_Mute_Color_Config(filp, u32Arg);
        break;

    case IOCTL_SCA_SET_CLONE_SCREEN_RATIO:
        retval = _MDrv_SCAIO_IOC_Set_Clone_Screen_Ratio_Config(filp, u32Arg);
        break;

    case IOCTL_SCA_SET_DLC_INIT_CONFIG:
        retval = _MDrv_SCAIO_IOC_Set_DLC_Init_Config(filp, u32Arg);
        break;
    case IOCTL_SCA_SET_DLC_ONOFF_CONFIG:
        retval = _MDrv_SCAIO_IOC_Set_DLC_OnOff_Config(filp, u32Arg);
        break;

    case IOCTL_SCA_SET_UEVENT_CONFIG:
        retval = _MDrv_SCAIO_IOC_Set_UEvent_Config(filp, u32Arg);
        break;

    case IOCTL_SCA_SET_CVBSOUT_DAC_CONFIG:
        retval = _MDrv_SCAIO_IOC_Set_CVBSOUT_DAC_Config(filp, u32Arg);
        break;

    case IOCTL_SCA_SET_CAMERA_INPUTTIMING_CONFIG:
        retval = _MDrv_SCAIO_IOC_Set_Camera_InputTiming_Config(filp, u32Arg);
        break;

    default:  /* redundant, as cmd was checked against MAXNR */
        SCA_DBG(SCA_DBG_LV_0, "[SCA] ERROR IOCtl number %x\n ",u32Cmd);
        retval = -ENOTTY;
        break;
    }

	SCA_RELEASE_MUTEX(_SCA_PowerState_Mutex);

    return retval;
}

//for kernel use only
long mstar_sca_drv_ioctl_export(unsigned int u32Cmd, unsigned long u32Arg)
{
    SCA_DBG(SCA_DBG_LV_IOCTL, "[SCA] SCAIO_IOCTL export!!! \n");

    return mstar_sca_drv_ioctl(NULL, u32Cmd, u32Arg);
}

static void mstar_sca_drv_platfrom_release(struct device *device)
{
    if (NULL == device)
    {
        SCA_DBG(SCA_DBG_LV_0, "ERROR: in mstar_sca_drv_platfrom_release, \
                device is NULL pointer !\n");
    }
    else
    {
        SCA_DBG(SCA_DBG_LV_IOCTL, "in mstar_sca_drv_platfrom_release, module unload!\n");
    }

}

static const char* KEY_DMEM_SCA_MLOAD="SCA_MLOAD";
static const char* KEY_DMEM_SCA_SC0_MAIN="SCA_SC0_M";
static const char* KEY_DMEM_SCA_SC1_MAIN="SCA_SC1_M";
static const char* KEY_DMEM_SCA_SC2_MAIN="SCA_SC2_M";
static const char* KEY_DMEM_SCA_SC0_SUB="SCA_SC0_S";
static const char* KEY_DMEM_SCA_SC2_SUB="SCA_SC2_S";
static const char* KEY_DMEM_SCA_AVD="SCA_AVD";

static void* alloc_dmem(const char* name, unsigned int size, dma_addr_t *addr)
{
	MSYS_DMEM_INFO dmem;
	memcpy(dmem.name,name,strlen(name)+1);
	dmem.length=size;
	if(0!=msys_request_dmem(&dmem)){
		return NULL;
	}

	*addr=dmem.phys;
	return (void *)((uintptr_t)dmem.kvirt);
}

static void free_dmem(const char* name, unsigned int size, void *virt, dma_addr_t addr)
{
	MSYS_DMEM_INFO dmem;
	memcpy(dmem.name,name,strlen(name)+1);
	dmem.length=size;
	dmem.kvirt=(unsigned long long)((uintptr_t)virt);
	dmem.phys=(unsigned long long)((uintptr_t)addr);
	msys_release_dmem(&dmem);
}

static int _sca_alloc_mem(struct platform_device *pdev)
{



    if (!(sg_sca_mload_vir_addr = alloc_dmem(KEY_DMEM_SCA_MLOAD,
                                                    PAGE_ALIGN(sg_sca_mload_size),
                                                    &sg_sca_mload_bus_addr)) )
    {
        SCA_DBG(SCA_DBG_LV_0, "[SCA]: unable to allocate screen memory\n");
        return FALSE;
    }

    SCA_DBG(SCA_DBG_LV_INIT_XC, "[SCA]: MLoad: Phy:%x  Vir:%x\n", sg_sca_mload_bus_addr, (u32)sg_sca_mload_vir_addr);


    if (!(sg_sca_sc0_main_vir_addr = alloc_dmem(KEY_DMEM_SCA_SC0_MAIN,
                                                    PAGE_ALIGN(sg_sca_main_size),
                                                    &sg_sca_sc0_main_bus_addr)) )
    {
        SCA_DBG(SCA_DBG_LV_0, "[SCA]: unable to allocate screen memory\n");
        return FALSE;
    }

    SCA_DBG(SCA_DBG_LV_INIT_XC, "[SCA]: SC0 Main: Phy:%x  Vir:%x\n", sg_sca_sc0_main_bus_addr, (u32)sg_sca_sc0_main_vir_addr);

    if (!(sg_sca_sc1_main_vir_addr = alloc_dmem(KEY_DMEM_SCA_SC1_MAIN,
                                                    PAGE_ALIGN(sg_sca_main_size),
                                                    &sg_sca_sc1_main_bus_addr)))
    {
        SCA_DBG(SCA_DBG_LV_0, "[SCA]: unable to allocate screen memory\n");
        return FALSE;
    }
    SCA_DBG(SCA_DBG_LV_INIT_XC, "[SCA]: SC1 Main: Phy:%x  Vir:%x\n", sg_sca_sc1_main_bus_addr, (u32)sg_sca_sc1_main_vir_addr);


    if (!(sg_sca_sc2_main_vir_addr = alloc_dmem(KEY_DMEM_SCA_SC2_MAIN,
                                                    PAGE_ALIGN(sg_sca_main_size),
                                                    &sg_sca_sc2_main_bus_addr)))
    {
        SCA_DBG(SCA_DBG_LV_0, "[SCA]: unable to allocate screen memory\n");
        return FALSE;
    }

    SCA_DBG(SCA_DBG_LV_INIT_XC, "[SCA]: SC2 Main: Phy:%x  Vir:%x\n", sg_sca_sc2_main_bus_addr, (u32)sg_sca_sc2_main_vir_addr);

    if (!(sg_sca_sc0_sub_vir_addr = alloc_dmem(KEY_DMEM_SCA_SC0_SUB,
                                                    PAGE_ALIGN(sg_sca_sub_size),
                                                    &sg_sca_sc0_sub_bus_addr)))
    {
        SCA_DBG(SCA_DBG_LV_0, "[SCA]: unable to allocate screen memory\n");
        return FALSE;
    }

    SCA_DBG(SCA_DBG_LV_INIT_XC, "[SCA]: SC0 Sub: Phy:%x  Vir:%x\n", sg_sca_sc0_sub_bus_addr, (u32)sg_sca_sc0_sub_vir_addr);


    if (!(sg_sca_sc2_sub_vir_addr = alloc_dmem(KEY_DMEM_SCA_SC2_SUB,
                                                    PAGE_ALIGN(sg_sca_sub_size),
                                                    &sg_sca_sc2_sub_bus_addr)))
    {
        SCA_DBG(SCA_DBG_LV_0, "[SCA]: unable to allocate screen memory\n");
        return FALSE;
    }

    SCA_DBG(SCA_DBG_LV_INIT_XC, "[SCA]: SC2 Sub: Phy:%x  Vir:%x\n", sg_sca_sc2_sub_bus_addr, (u32)sg_sca_sc2_sub_vir_addr);


    if (!(sg_sca_avd_vir_addr = alloc_dmem(KEY_DMEM_SCA_AVD,
                                                    PAGE_ALIGN(sg_sca_avd_size),
                                                    &sg_sca_avd_bus_addr)))
    {
        SCA_DBG(SCA_DBG_LV_0, "[SCA]: unable to allocate screen memory\n");
        return FALSE;
    }
    SCA_DBG(SCA_DBG_LV_INIT_XC, "[SCA]: AVD: Phy:%x  Vir:%x\n", sg_sca_avd_bus_addr, (u32)sg_sca_avd_vir_addr);

    return TRUE;
}

static void _sca_init_varialbe(void)
{

    memset(&gstInitInfo, 0, sizeof(XC_INIT_INFO));
    gstInitInfo.u32MLoad_Phy  = ms_mem_bus_to_MIU(sg_sca_mload_bus_addr);
    gstInitInfo.u32MLoad_Size = sg_sca_mload_size;
    gstInitInfo.u32MLoad_Vir  = (MS_U32)sg_sca_mload_vir_addr;


    gstInitInfo.u32SC0_Main_Phy  = ms_mem_bus_to_MIU(sg_sca_sc0_main_bus_addr);
    gstInitInfo.u32SC0_Main_Size = sg_sca_main_size;

    gstInitInfo.u32SC0_Sub_Phy  = ms_mem_bus_to_MIU(sg_sca_sc0_sub_bus_addr);
    gstInitInfo.u32SC0_Sub_Size = sg_sca_sub_size;

    gstInitInfo.u32SC1_Main_Phy  = ms_mem_bus_to_MIU(sg_sca_sc1_main_bus_addr);
    gstInitInfo.u32SC1_Main_Size = sg_sca_main_size;

    gstInitInfo.u32SC2_Main_Phy  = ms_mem_bus_to_MIU(sg_sca_sc2_main_bus_addr);
    gstInitInfo.u32SC2_Main_Size = sg_sca_main_size;

    gstInitInfo.u32SC2_Sub_Phy  = ms_mem_bus_to_MIU(sg_sca_sc2_sub_bus_addr);
    gstInitInfo.u32SC2_Sub_Size = sg_sca_sub_size;

    SCA_DBG(SCA_DBG_LV_INIT_XC, "[SCA] MLoad: %lx %lx \n", gstInitInfo.u32MLoad_Phy, gstInitInfo.u32MLoad_Size);
    SCA_DBG(SCA_DBG_LV_INIT_XC, "[SCA] SC0 M: %lx %lx \n", gstInitInfo.u32SC0_Main_Phy, gstInitInfo.u32SC0_Main_Size);
    SCA_DBG(SCA_DBG_LV_INIT_XC, "[SCA] SC0 S: %lx %lx \n", gstInitInfo.u32SC0_Sub_Phy, gstInitInfo.u32SC0_Sub_Size);
    SCA_DBG(SCA_DBG_LV_INIT_XC, "[SCA] SC1 M: %lx %lx \n", gstInitInfo.u32SC1_Main_Phy, gstInitInfo.u32SC1_Main_Size);
    SCA_DBG(SCA_DBG_LV_INIT_XC, "[SCA] SC2 M: %lx %lx \n", gstInitInfo.u32SC2_Main_Phy, gstInitInfo.u32SC2_Main_Size);
    SCA_DBG(SCA_DBG_LV_INIT_XC, "[SCA] SC2 S: %lx %lx \n", gstInitInfo.u32SC2_Sub_Phy, gstInitInfo.u32SC2_Sub_Size);

}

static void _sca_free_mem(struct platform_device *pdev)
{
	free_dmem(KEY_DMEM_SCA_MLOAD,
                      PAGE_ALIGN(sg_sca_mload_size),
                      sg_sca_mload_vir_addr,
                      sg_sca_mload_bus_addr);

	free_dmem(KEY_DMEM_SCA_SC0_MAIN,
                      PAGE_ALIGN(sg_sca_main_size),
                      sg_sca_sc0_main_vir_addr,
                      sg_sca_sc0_main_bus_addr);

	free_dmem(KEY_DMEM_SCA_SC1_MAIN,
                      PAGE_ALIGN(sg_sca_main_size),
                      sg_sca_sc1_main_vir_addr,
                      sg_sca_sc1_main_bus_addr);

	free_dmem(KEY_DMEM_SCA_SC0_SUB,
                      PAGE_ALIGN(sg_sca_sub_size),
                      sg_sca_sc0_sub_vir_addr,
                      sg_sca_sc0_sub_bus_addr);

	free_dmem(KEY_DMEM_SCA_AVD,
                      PAGE_ALIGN(sg_sca_avd_size),
                      sg_sca_avd_vir_addr,
                      sg_sca_avd_bus_addr);
}


static int mstar_sca_drv_suspend(struct platform_device *dev, pm_message_t state)
{

    SCA_DBG(SCA_DBG_LV_IOCTL, "[SCA] SCA_DRV_SUSPEND !!! \n");

	SCA_WAIT_MUTEX_FOREVER(_SCA_PowerState_Mutex);

	g_bScaAlive = FALSE;

#if ENABLE_MONITOR_THREAD
	/* check and delete monitor threads */
	Delete_Monitor_Thread_by_Dest(SCA_DEST_MAIN);
	Delete_Monitor_Thread_by_Dest(SCA_DEST_SUB);
	Delete_Monitor_Thread_by_Dest(SCA_DEST_MAIN_1);
	Delete_Monitor_Thread_by_Dest(SCA_DEST_MAIN_2);
	Delete_Monitor_Thread_by_Dest(SCA_DEST_SUB_2);
#endif

    MApi_XC_ClearSysInitFlag();
    bInitPNL = FALSE;
    MApi_PNL_SkipTimingChange(FALSE);

    MApi_PNL_ResetGlobalVar();
    MApi_XC_MLoad_ResetGlobalVariable();
    MApi_XC_ACE_ResetGlobalVar();
    MDrv_PQ_Exit();
    MApi_Set_SC1_LoadPQ(0);
    MApi_Clear_AVDFlag();
    //_sca_free_mem(dev);


    if(pDLCMonitorThread)
    {
        kthread_stop(pDLCMonitorThread);
        pDLCMonitorThread = NULL;
    }

    if(pHDMITxHPDThread)
    {
        kthread_stop(pHDMITxHPDThread);
        pHDMITxHPDThread = NULL;
    }

    MApi_XC_Exit();

	SCA_RELEASE_MUTEX_FOREVER(_SCA_PowerState_Mutex);

    return 0;
}

static int SCA_Resume_Thread(void *arg)
{
#if SCA_MUTEX_USE_SEM
struct semaphore *pSCA_XC_AVD_Init_Mutex = (struct semaphore *)arg;
#else
MS_U32 *pSCA_XC_AVD_Init_Mutex = (MS_U32 *)arg;
#endif


    SCA_WAIT_MUTEX(*pSCA_XC_AVD_Init_Mutex);

    SCA_DBG(SCA_DBG_LV_INIT_XC,"SCA_Rsume_Thread \n");
    MApi_AVD_Sys_Init(TRUE);

    MApi_XC_Sys_Init(gstInitInfo);

#if ENABLE_MONITOR_THREAD
    SCA_Init_Thread();
#endif

#if defined(SCA_ENABLE_DLC)
    SCA_Init_DLC_Thread();
#endif

    SCA_Init_Disp_Path();

    g_bScaAlive = TRUE;
    SCA_RELEASE_MUTEX(*pSCA_XC_AVD_Init_Mutex);
    return 0;
}

static int mstar_sca_drv_resume(struct platform_device *dev)
{
    struct task_struct *pRsumeThread;

    SCA_DBG(SCA_DBG_LV_IOCTL, "[SCA] SCA_DRV_RESUME !!! \n");

	SCA_WAIT_MUTEX(_SCA_PowerState_Mutex);
	if ( mstar_sca_is_alive() )
	{
		SCA_RELEASE_MUTEX(_SCA_PowerState_Mutex);
		return 0;
	}
	//g_bScaAlive = TRUE;	// scaler alive but not workable //

    _sca_init_varialbe();

    SCA_DBG(SCA_DBG_LV_0, " AVD Resume init \n");

    MDrv_MMIO_Init();

    MApi_AVD_Sys_SetBuf( ms_mem_bus_to_MIU(sg_sca_avd_bus_addr), sg_sca_avd_size);

    mstar_sca_drv_init_pnl(FALSE);

    pRsumeThread = kthread_create(SCA_Resume_Thread,
                                  (void *)&_SCA_XC_AVD_Init_Mutex,
                                  "SCA_RESUME_THREAD");

    wake_up_process(pRsumeThread);

	SCA_RELEASE_MUTEX(_SCA_PowerState_Mutex);

    return 0;
}

#ifdef UIO_INT_EXAMPLE
static irqreturn_t test_handler(int irq, struct uio_info *dev_info)
{
printk("interrupt happened\n");
MDrv_SC_set_interrupt(SC_INT_VSINT, DISABLE);
return IRQ_HANDLED;
}
#endif

static int mstar_sca_drv_probe(struct platform_device *pdev)
{
#ifdef UIO_INT_EXAMPLE
    struct uio_info *info;
    int ret;
#endif
     printk(KERN_EMERG "[SCA] SCA_DRV_PROBE !!! \n");

    init_completion(&_SCA_MoniThread_Comp);
    init_completion(&_SCA_MoniThread_Stop_Comp);

#if SCA_MUTEX_USE_SEM
    sema_init(&_SCA_Mute_Mutex, 1);
    sema_init(&_SCA_Cnnt_Mutex, 1);
    sema_init(&_SCA_MoniWinCfg_Mutex, 1);
    sema_init(&_SCA_XC_AVD_Init_Mutex, 1);
	sema_init(&_SCA_PowerState_Mutex, 1);
#else
    _SCA_Mute_Mutex = = MsOS_CreateMutex(E_MSOS_FIFO, "_SCA_Mute_Mutex", MSOS_PROCESS_SHARED);

    if (_SCA_Mute_Mutex == -1)
    {
        SCA_DBG(SCA_DBG_LV_0, "Create SCA Mutex Fail \n");
        return -EFAULT;
    }

    _SCA_Cnnt_Mutex = MsOS_CreateMutex(E_MSOS_FIFO, "_SCA_Cnnt_Mutex", MSOS_PROCESS_SHARED);

    if (_SCA_Cnnt_Mutex == -1)
    {
        SCA_DBG(SCA_DBG_LV_0, "Create SCA Mutex Fail \n");
        return -EFAULT;
    }

    _SCA_MoniWinCfg_Mutex = MsOS_CreateMutex(E_MSOS_FIFO, "_SCA_MoniWinCfg_Mutex", MSOS_PROCESS_SHARED);

    if (_SCA_MoniWinCfg_Mutex == -1)
    {
        SCA_DBG(SCA_DBG_LV_0, "Create SCA Mutex Fail \n");
        return -EFAULT;
    }

    _SCA_XC_AVD_Init_Mutex = MsOS_CreateMutex(E_MSOS_FIFO, "_SCA_XC_AVD_Init_Mutex", MSOS_PROCESS_SHARED);

    if (_SCA_XC_AVD_Init_Mutex == -1)
    {
        SCA_DBG(SCA_DBG_LV_0, "Create SCA Mutex Fail \n");
        return -EFAULT;
    }

    _SCA_PowerState_Mutex = MsOS_CreateMutex(E_MSOS_FIFO, "_SCA_PowerState_Mutex", MSOS_PROCESS_SHARED);

    if (_SCA_PowerState_Mutex == -1)
    {
        SCA_DBG(SCA_DBG_LV_0, "Create SCA Mutex Fail \n");
        return -EFAULT;
    }

    SCA_DBG(SCA_DBG_LV_0, "Mutex: Cnnt:%lx, TimWin:%lx \n", _SCA_Cnnt_Mutex, _SCA_MoniWinCfg_Mutex);
#endif

    if(_sca_alloc_mem(pdev) == FALSE)
    {
        return -EFAULT;
    }

    MsOS_Init();

    _sca_init_varialbe();

    MDrv_MMIO_Init();

    // AVD Init
    MApi_AVD_Sys_SetBuf( ms_mem_bus_to_MIU(sg_sca_avd_bus_addr), sg_sca_avd_size);
    //removed to avoid timing issue with rtk, this will be done in open or ioctl
    //MApi_AVD_Sys_Init(FALSE);


    /*
    removed to avoid timing issue with rtk, this will be done in open or ioctl
    if( !MApi_XC_GetSysInitFlag() &&  !IsRTKCvbsInputLocked())
    {
        MApi_XC_Sys_Init(gstInitInfo);
    }
    */
#if ENABLE_MONITOR_THREAD
    SCA_Init_Thread();
#endif

#if defined(SCA_ENABLE_DLC)
    SCA_Init_DLC_Thread();
#endif

#if defined(SCA_ENABLE_HDMITX_HPD)
    gpkobj = &pdev->dev.kobj;
#endif


    SCA_Init_Disp_Path();

#if 0
#ifdef UIO_INT_EXAMPLE
	if(request_irq(INT_IRQ_DISP ,MDrv_SC_isr, IRQF_SHARED,"SCA DISP_INT", NULL)) //parameter 5 should be set correctly,but for interrupt testing now it can work.
#else
	if(request_irq(INT_IRQ_DISP ,MDrv_SC_isr, 0,"SCA DISP_INT", NULL))
#endif
    	   SCA_DBG(SCA_DBG_LV_IOCTL,"[SCA] request irq for sc fail!\n");

	if(request_irq(INT_IRQ_DISP1 ,MDrv_SC1_isr, 0,"SCA DISP_INT1", NULL))
    	   SCA_DBG(SCA_DBG_LV_IOCTL,"[SCA] request irq for sc1 fail!\n");

	if(request_irq(INT_IRQ_DISP2 ,MDrv_SC2_isr, 0,"SCA DISP_INT2", NULL))
    	   SCA_DBG(SCA_DBG_LV_IOCTL,"[SCA] request irq for sc2 fail!\n");
#endif

#if ENABLE_DIPW_INTR
	if(request_irq(INT_IRQ_DIPW, MDrv_DIPW_isr, 0, "SCA DIPW ", NULL))
		   SCA_DBG(SCA_DBG_LV_IOCTL, "[SCA] request irq for dipw fail!\n");
#endif

 #if 0// BHALF_TYPE == BHALF_TYPE_WKQ
    INIT_WORK(&sca_vsnc_wkq, (void (*)(void *))sca_vsync_bhalf, NULL)
 #endif

#ifdef UIO_INT_EXAMPLE
    printk(KERN_EMERG "UIO EXAMPLE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    printk(KERN_EMERG "size %d\n",sizeof(struct uio_info));
    info = kzalloc(sizeof(struct uio_info), GFP_KERNEL);
    if (!info)
        return -ENOMEM;

    info->mem[0].addr = (unsigned long)kmalloc(1024,GFP_KERNEL);

    if(info->mem[0].addr == 0)
    {
        printk("Invalid memory resource\n");
        return -ENOMEM;
    }

    info->mem[0].memtype = UIO_MEM_LOGICAL;
    info->mem[0].size = 1024;
    info->version = "0.1";
    info->name="msca";
    info->irq=INT_IRQ_DISP;
    info->irq_flags = IRQF_SHARED;
    info->handler = test_handler;

    ret = uio_register_device(&pdev->dev, info);
    if (ret)
    {
        printk("uio_register failed %d\n",ret);
        iounmap(info->mem[0].internal_addr);
       // printk("uio_register failed %d\n",ret);
        return -ENODEV;
    }
    platform_set_drvdata(pdev, info);
    printk(KERN_EMERG "UIO EXAMPLE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	#endif

    g_bScaAlive = TRUE;

    return 0;

}

static int mstar_sca_drv_remove(struct platform_device *pdev)
{
    int i;
    SCA_DBG(SCA_DBG_LV_IOCTL, "[SCA] SCA_DRV_REMOVE !!! \n");


    _sca_free_mem(pdev);

    for(i=0; i<SCA_PQ_BIN_ID_NUM; i++)
    {
        if(stPQBinData[i].pBuff)
        {
            kfree(stPQBinData[i].pBuff);
            stPQBinData[i].pBuff = NULL;
            stPQBinData[i].u32Len = 0;
        }
    }

#if !SCA_MUTEX_USE_SEM
    if(_SCA_Mute_Mutex != -1)
    {
        MsOS_DeleteMutex(_SCA_Mute_Mutex);
    }

    if(_SCA_Cnnt_Mutex != -1)
    {
        MsOS_DeleteMutex(_SCA_Cnnt_Mutex);
    }

    if(_SCA_MoniWinCfg_Mutex != -1)
    {
        MsOS_DeleteMutex(_SCA_MoniWinCfg_Mutex);
    }

    if(_SCA_XC_AVD_Init_Mutex != -1)
    {
        MsOS_DeleteMutex(_SCA_XC_AVD_Init_Mutex);
    }

	if (_SCA_PowerState_Mutex != -1)
	{
		MsOS_DeleteMutex(_SCA_PowerState_Mutex);
	}

#endif

    g_bScaAlive = FALSE;

    return 0;
}

//==============================================================================


static struct platform_driver Mstar_sca_driver = {
	.probe 		= mstar_sca_drv_probe,
	.remove 	= mstar_sca_drv_remove,
    .suspend    = mstar_sca_drv_suspend,
    .resume     = mstar_sca_drv_resume,

	.driver = {
		.name	= "msca",
        .owner  = THIS_MODULE,
	}
};

static u64 sg_mstar_device_sca_dmamask = 0xffffffffUL;

static struct platform_device sg_mdrv_sca_device =
{
    .name = "msca",
    .id = 0,
    .dev =
    {
        .release = mstar_sca_drv_platfrom_release,
        .dma_mask = &sg_mstar_device_sca_dmamask,
        .coherent_dma_mask = 0xffffffffUL
    }
};

//==============================================================================

//-------------------------------------------------------------------------------------------------
// Module functions
//-------------------------------------------------------------------------------------------------


int _MDrv_SCAIO_ModuleInit(void)
{
    int s32Ret;
    dev_t  dev;
    int ret = 0;

    SCA_DBG(SCA_DBG_LV_IOCTL, "[SCAIO]_Init \n");

    if(_devSCA.s32Major)
    {
        dev = MKDEV(_devSCA.s32Major, _devSCA.s32Minor);
        s32Ret = register_chrdev_region(dev, MDRV_SCA_DEVICE_COUNT, MDRV_SCA_NAME);
    }
    else
    {
        s32Ret = alloc_chrdev_region(&dev, _devSCA.s32Minor, MDRV_SCA_DEVICE_COUNT, MDRV_SCA_NAME);
        _devSCA.s32Major = MAJOR(dev);
    }

    if (0 > s32Ret)
    {
        SCA_DBG(SCA_DBG_LV_0, "[SCA] Unable to get major %d\n", _devSCA.s32Major);
        return s32Ret;
    }

    cdev_init(&_devSCA.cdev, &_devSCA.fops);
    if (0 != (s32Ret= cdev_add(&_devSCA.cdev, dev, MDRV_SCA_DEVICE_COUNT)))
    {
        SCA_DBG(SCA_DBG_LV_0, "[SCA] Unable add a character device\n");
        unregister_chrdev_region(dev, MDRV_SCA_DEVICE_COUNT);
        return s32Ret;
    }

    sca_class = class_create(THIS_MODULE, sca_classname);
    if(IS_ERR(sca_class))
    {
        printk(KERN_WARNING"Failed at class_create().Please exec [mknod] before operate the device/n");
    }
    else
    {
        device_create(sca_class, NULL, dev,NULL, "msca");
    }

    /* initial the whole SCA Driver */
    ret = platform_driver_register(&Mstar_sca_driver);

    if (!ret)
    {
        ret = platform_device_register(&sg_mdrv_sca_device);
        if (ret)    /* if register device fail, then unregister the driver.*/
        {
            platform_driver_unregister(&Mstar_sca_driver);
            SCA_DBG(SCA_DBG_LV_0, "[SCA] register failed\n");

        }
        else
        {
            SCA_DBG(SCA_DBG_LV_IOCTL, "[SCA] register success\n");
        }
    }

    /*inital global function */
    memset(&g_stDispIntrSetting, 0, sizeof(g_stDispIntrSetting));

#if defined(NETLINK_SAMPLE)
{

    struct netlink_kernel_cfg stCfg = {
        .groups = 0,
        .flags = 0,
        .input = sca_netlink_receive,
        .cb_mutex = NULL,
        .bind = NULL,
    };

    netlink_sock = netlink_kernel_create(&init_net, NETLINK_USERSOCK,  &stCfg);

    if (!netlink_sock)
    {
        SCA_DBG(SCA_DBG_LV_0, "Fail to create netlink socket.\n");
    }
    else
    {
        SCA_DBG(SCA_DBG_LV_0, "Success to create netlink socket.\n");
    }
}
#endif

    return ret;
}


void _MDrv_SCAIO_ModuleExit(void)
{
    /*de-initial the who GFLIPDriver */
    SCA_DBG(SCA_DBG_LV_0, "[SCAIO]_Exit \n");

    cdev_del(&_devSCA.cdev);
    device_destroy(sca_class, MKDEV(_devSCA.s32Major, _devSCA.s32Minor));
    class_destroy(sca_class);
    unregister_chrdev_region(MKDEV(_devSCA.s32Major, _devSCA.s32Minor), MDRV_SCA_DEVICE_COUNT);
    platform_driver_unregister(&Mstar_sca_driver);

#if defined(NETLINK_SAMPLE)
    sock_release(netlink_sock->sk_socket);
#endif
}



module_init(_MDrv_SCAIO_ModuleInit);
module_exit(_MDrv_SCAIO_ModuleExit);

MODULE_AUTHOR("MSTAR");
MODULE_DESCRIPTION("SCA ioctrl driver");
MODULE_LICENSE("SCA");



int mstar_sca_drv_get_pnl_width(void)
{
    return g_IPanel.Width();
}

int mstar_sca_drv_get_pnl_height(void)
{
    return g_IPanel.Height();
}

int mstar_sca_drv_get_pnl_refresh_rate(void)
{
	return (g_IPanel.DefaultVFreq()/10);
}

void mstar_sca_drv_init_pnl(MS_BOOL bSkipTime)
{
    printk(KERN_WARNING"[SCA] Pnl init: %d \n", bSkipTime);
#ifdef __BOOT_PNL__
    bSkipTime = FALSE;
#endif
    MApi_PNL_SkipTimingChange(bSkipTime);
    MApi_XC_Sys_Init_Pnl();
    bInitPNL = TRUE;
}

int mstar_sca_drv_get_h_backporch(int sc_num)
{
    MS_U16 backporch;

    if(sc_num == 0)
    {
        backporch = g_IPanel.HSynBackPorch();
    }
    else
    {
        SCALER_WIN eWindow = sc_num == 1 ? SC1_MAIN_WINDOW : SC2_MAIN_WINDOW;

        if(MApi_XC_Get_TGen_H_Backporch(eWindow, &backporch) == E_APIXC_RET_FAIL)
        {
            backporch = 0x3E;
        }
    }

    return (int)backporch;
}

int mstar_sca_drv_check_outputyuv(int sc_num)
{
    int byuv = FALSE;

    if(sc_num == 0)
    {
        byuv = FALSE;
    }
    else
    {
        if(g_enTGenTiming == E_XC_TGEN_VE_480_I_60 ||
           g_enTGenTiming == E_XC_TGEN_VE_576_I_50)
        {
            byuv = TRUE;
        }
        else
        {
            byuv =
                g_HDMI_Cfg.enHDMITx_ColorType == SCA_HDMITX_OUTPUT_COLOR_RGB ? FALSE : TRUE;
        }
    }

    return byuv;
}

