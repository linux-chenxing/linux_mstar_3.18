///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2006 - 2008 MStar Semiconductor, Inc.
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

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    mdrv_temp.c
/// @brief  TEMP Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/string.h>
#include <linux/poll.h>
#include <linux/wait.h>
#if defined(CONFIG_MIPS)
#elif defined(CONFIG_ARM) || defined(CONFIG_ARM64)
#include <asm/io.h>
#endif
#include <asm/uaccess.h>
#include <linux/sched.h>
#include <linux/version.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/jiffies.h>
#include <linux/slab.h>
#include <linux/time.h>
#include <linux/sched.h>
#include <linux/proc_fs.h>
#include <linux/kthread.h>

//drver header files
#include "chip_int.h"
#include "mdrv_mstypes.h"
#include "mdrv_dlc.h"
#include "mhal_xc.h"
#include "mdrv_xc.h"
#ifdef CONFIG_MSTAR_XC_HDR_SUPPORT
#include "mstar/mstar_chip.h"
#include "mhal_dlc.h"
#include "color_format_input.h"
#include "dolby_hdr_mem.h"
#include "mhal_dynamicscaling.h"
#include "mhal_xc_chip_config.h"

#ifdef XC_PQ_ADP_ENABLE
extern void MHal_Run_PQ_Adaptive(void);
#endif

#if defined(CONFIG_MSTAR_XC_HDR_SUPPORT)
#define     XC_HDR_ENABLE
#endif

#ifdef      SUPPORT_TCH
#include "tc_hdr_mem.h"
#endif

//#define XC_DEBUG_ENABLE
#ifdef      XC_DEBUG_ENABLE
#define     XC_KDBG(_fmt, _args...)        printk("[XC (Driver)][%s:%05d] " _fmt, __FUNCTION__, __LINE__, ##_args)
#define     XC_ASSERT(_con)   do {\
                                                            if (!(_con)) {\
                                                                printk(KERN_CRIT "BUG at %s:%d assert(%s)\n",\
                                                                                                    __FILE__, __LINE__, #_con);\
                                                                BUG();\
                                                            }\
                                                          } while (0)
#else
#define     XC_KDBG(fmt, args...)
#define     XC_ASSERT(arg)
#endif

#define     XC_TCHDBG(_fmt, _args...)        printk("[XC (Hal)][%s:%05d] " _fmt, __FUNCTION__, __LINE__, ##_args)

//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

#ifndef XC_FILM_DRIVER_VER
#define XC_FILM_DRIVER_VER               1
#endif

#ifndef XC_DISABLE_KERNEL_FILM_CONTROL
#define XC_DISABLE_KERNEL_FILM_CONTROL   0
#endif

extern void KHal_SC_WriteSWDSCommand(EN_KDRV_SC_DEVICE u32DeviceID,EN_KDRV_WINDOW eWindow,E_DS_CLIENT client, MS_U32 u32CmdRegAddr, MS_U16 u16CmdRegValue,k_ds_reg_ip_op_sel IPOP_Sel,k_ds_reg_source_sel Source_Select,K_XC_DS_CMDCNT *pstXC_DS_CmdCnt,MS_U8 u8DSIndex);
extern void KHal_SC_WriteSWDSCommand_Mask(EN_KDRV_SC_DEVICE u32DeviceID,EN_KDRV_WINDOW eWindow,E_DS_CLIENT client, MS_U32 u32CmdRegAddr, MS_U16 u16CmdRegValue,k_ds_reg_ip_op_sel IPOP_Sel,k_ds_reg_source_sel Source_Select,K_XC_DS_CMDCNT *pstXC_DS_CmdCnt,MS_U8 u8DSIndex,MS_U16 u16Mask);
extern MS_BOOL KApi_XC_MLoad_WriteCmd(EN_MLOAD_CLIENT_TYPE _client_type,MS_U32 u32Addr, MS_U16 u16Data, MS_U16 u16Mask);
//-------------------------------------------------------------------------------------------------
//  Local Structurs
//-------------------------------------------------------------------------------------------------
#define XC_SHARE_MEMORY_SIZE (HDR_MEM_COMMON_ENTRY_SIZE >= 128 ? HDR_MEM_COMMON_ENTRY_SIZE: 128) //128: Main 64bytes + Sub 64bytes
#define XC_NO_INPUT_SIGNAL_TIMEOUT 1000

#define XC_HZ_TIMER 0
#define XC_HR_TIMER 1
#define XC_TIMER_TYPE  XC_HZ_TIMER
#define HDR_SWITCH_COUNTER   10
#define SDR_SWITCH_COUNTER   15

#if ( !defined (CONFIG_HIGH_RES_TIMERS) && (XC_TIMER_TYPE == XC_HR_TIMER))
#error "can't using high resolution timer when kernel config not support CONFIG_HIGH_RES_TIMERS"
#endif

#if (XC_TIMER_TYPE == XC_HR_TIMER)
#define XC_TIMER_CHECK_TIME             2 //Check every 2ms
#else
#define XC_TIMER_CHECK_TIME             ((12 * HZ < 1000) ? 1 : (12 * HZ / 1000))  //Check every 12ms
#endif

#define MS_TO_NS(x) (x * 1E6L)
#define XC_INPUT_MD_MAX_SIZE 0x150000
#define VDEC_BASE_ADDR 0x13900000
#define XC_HDR_BASE_ADD 0x0001060000
#define IS_DOLBY_CRC_PASS (MHal_XC_IsCRCPass(0) || MHal_XC_IsCRCPass(1) || MHal_XC_IsCRCPass(2))
#define DOLBY_OTT_CACHED_BUFFER 1
#define DOLBY_OTT_MULTI_TIMER 1
//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
EN_KDRV_XC_HDR_TYPE _enHDRType = E_KDRV_XC_HDR_NONE;
EN_KDRV_XC_INPUT_SOURCE_TYPE _enInputSourceType = E_KDRV_XC_INPUT_SOURCE_NONE;
static ST_KDRV_XC_SHARE_MEMORY_INFO _stShareMemInfo = {0, 0, 0};
static MS_U8 *_pu8ShareMemBaseAddr = NULL;
#if DOLBY_OTT_CACHED_BUFFER
static MS_U8 *_pu8ShareMemBaseAddr_Cached = NULL;
#endif
static MS_BOOL _bIOUnmapShareMem = FALSE;
static MS_U16 _u16CurrentIndx = 0xFFFF;
MS_BOOL _bEnableHDR = FALSE;
static MS_U8 *_pu8DolbyMetadata = NULL;
static EN_KDRV_XC_HDR_COLOR_FORMAT _enColorFormat;
extern ST_KDRV_XC_CFD_HDR _stCfdHdr[2];
static ST_KDRV_XC_HDR_DOLBY_MEMORY_FORMAT_EX *_pstDolbyHDRShareMem = NULL;
static DEFINE_MUTEX(_hdr_mutex);
MS_U32 _u32NoSignalCount = 0;
extern ST_KDRV_XC_CFD_HDMI _stCfdHdmi[2];
extern MS_BOOL _bTimingChanged;
MS_BOOL _ISMMDolby = FALSE;
MS_BOOL _ISHDMIDolby = FALSE;
DEFINE_MUTEX(_cfd_mutex);
static MS_U8 _u8CRCErrCnt = 0;
static MS_BOOL _bPreHDR = FALSE;

static ST_KDRV_XC_3DLUT_INFO st3DLutInfoForSTR;
#if !DOLBY_OTT_MULTI_TIMER
#if (XC_TIMER_TYPE == XC_HR_TIMER)
static struct hrtimer _stXC_dolby_hdr_hrtimer;
#else
static struct timer_list _stXC_dolby_hdr_timer;
#endif
#endif
static spinlock_t _spinlock_xc_dolby_hdr;
static spinlock_t _sethdr_info_lock;
static MS_BOOL _bTimerStart = FALSE;
static MS_U8 *_pu8InputMDAddr = NULL;
#if DOLBY_OTT_CACHED_BUFFER
static MS_U8 *_pu8InputMDAddr_Cached = NULL;
#endif
static MS_U8 *_pu8RegSetAddr = NULL;
static MS_U8 *_pu8LutsAddr = NULL;
#if DOLBY_OTT_CACHED_BUFFER
static MS_U8 *_pu8RegSetAddr_Cached = NULL;
static MS_U8 *_pu8LutsAddr_Cached = NULL;
#endif
static MS_PHY _phyMDAddr = 0xFFFFFFFF;
static MS_BOOL _bIOUnmapInputMDMem = FALSE;
MS_U8 u8tmpCurrentIndex = 0;
extern MS_BOOL bJump;
#if DOLBY_OTT_MULTI_TIMER
static struct timer_list astXC_dolby_hdr_timer[NR_CPUS];
#endif
#endif

#ifdef      SUPPORT_TCH
MS_BOOL _bEnableTCH = FALSE;
static ST_KDRV_XC_HDR_TC_MEMORY_FORMAT_EX *_pstTCHDRShareMem = NULL;
static MS_BOOL _ISTCH = FALSE;
MS_BOOL _bTCHStart = FALSE;
static MS_BOOL _bTCHOneTimeSet = FALSE;
#endif


#ifdef SUPPORT_SEAMLESS_HDR
extern MS_U8 u8CallCFDinControl;
extern STU_CFDAPI_MM_PARSER gstMMParam;
extern STU_CFDAPI_MM_PARSER _stCfdMm[2];
extern MS_BOOL KApi_XC_MLoad_Fire(EN_MLOAD_CLIENT_TYPE _client_type,MS_BOOL bImmediate);
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
extern void Chip_Flush_Cache_Range(unsigned long u32Addr, unsigned long u32Size); //Clean & Invalid L1/L2 cache
extern void Chip_Inv_Cache_Range(unsigned long u32Addr, unsigned long u32Size);  //Invalid L1/L2 cache


//-------------------------------------------------------------------------------------------------
/// Handle GFLIP Interrupt notification handler
/// @param  irq                  \b IN: interrupt number
/// @param  devid                  \b IN: device id
/// @return IRQ_HANDLED
/// @attention
/// <b>[OBAMA] <em></em></b>
//-------------------------------------------------------------------------------------------------
irqreturn_t MDrv_XC_IntHandler(int irq,void *devid)
{
#ifdef XC_PQ_ADP_ENABLE
    MHal_Run_PQ_Adaptive();
#endif

#ifdef XC_HDR_ENABLE
    MDrv_XC_ProcessCFDIRQ();
#endif
    MDrv_DLC_ProcessIRQ();

    if(MHal_XC_IsBlackVideoEnable(MAIN_WINDOW))
    {
        return IRQ_HANDLED;
    }

#if (CONFIG_MSTAR_FRC_SOFTWARE_TOGGLE == 1)
    MHal_XC_FRCR2SoftwareToggle();
#endif

#if (XC_DISABLE_KERNEL_FILM_CONTROL == 0)
#if (XC_FILM_DRIVER_VER == 1)
    MDrv_XC_FilmDriverHWVer1();
#elif (XC_FILM_DRIVER_VER == 2)
    MDrv_XC_FilmDriverHWVer2();
#endif
#endif

    return IRQ_HANDLED;
}

#ifdef CONFIG_MSTAR_XC_HDR_SUPPORT
#if (XC_TIMER_TYPE == XC_HR_TIMER)
static enum hrtimer_restart _MDrv_XC_DolbyHDR_Timer_Callback(struct hrtimer *timer)
#else
#if DOLBY_OTT_MULTI_TIMER
static void _MDrv_XC_DolbyHDR_Timer_Callback(unsigned long data)
#else
static void _MDrv_XC_DolbyHDR_Timer_Callback(void)
#endif
#endif
{
    //avoid reentry
#if (XC_TIMER_TYPE == XC_HR_TIMER)
    spin_lock(&_spinlock_xc_dolby_hdr);
#else
#if DOLBY_OTT_MULTI_TIMER
    {
        if(data<NR_CPUS)
        {
            astXC_dolby_hdr_timer[data].function = _MDrv_XC_DolbyHDR_Timer_Callback;
            astXC_dolby_hdr_timer[data].expires = jiffies + XC_TIMER_CHECK_TIME;
            if(cpu_online(data))
                add_timer_on(&astXC_dolby_hdr_timer[data],data);
            else
                add_timer(&astXC_dolby_hdr_timer[data]);
        }
    }


    if(!spin_trylock_irq(&_spinlock_xc_dolby_hdr))
        return;

#else
    spin_lock_irq(&_spinlock_xc_dolby_hdr);
#endif
#endif

    if (_ISMMDolby)
    {
#if DOLBY_OTT_CACHED_BUFFER
        unsigned long ulFlushAddr[2]= {0,0};
        unsigned long ulFlushLen[2]= {0,0};
#endif
        do
        {
            if (_pstDolbyHDRShareMem != NULL && _pstDolbyHDRShareMem->u16Version == 2)
            {
                if (!isHDRInfoEntryEmpty(_pstDolbyHDRShareMem))
                {
                    MS_U32 u32RPtr = getDolbyHDRMemRPtr(_pstDolbyHDRShareMem);
                    ST_KDRV_XC_HDR_INFO_ENTRY stEntry;
                    memset(&stEntry,0,sizeof(ST_KDRV_XC_HDR_INFO_ENTRY));
                    MS_BOOL bRetVal = FALSE;

                    // Get current entry
                    bRetVal = getDolbyHDRInfoEntry(_pstDolbyHDRShareMem, u32RPtr, &stEntry);
                    XC_KDBG("Rptr: %d, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", u32RPtr, stEntry.u32InputMDAddr, stEntry.u32HDRRegsetAddr, stEntry.u32HDRRegsetSize, stEntry.u32HDRLutAddr, stEntry.u32HDRLutSize);
                    if ((bRetVal == TRUE)&&(!stEntry.u16Entry_Skip))
                    {
                        // Get InputMD data
                        MHal_XC_PrepareDolbyInfo();

                        MS_U16 u16MDMiuSel = getDolbyHDRMDMiuSel(_pstDolbyHDRShareMem);
                        MS_U32 u32InputMDAddr = stEntry.u32InputMDAddr;
                        MS_U32 u32Offset = 0;

                        if (MHal_XC_GetMiuOffset(u16MDMiuSel, &u32Offset) != TRUE)
                        {
                            printk("GetMiuOffset u16MDMiuSel: %d fail.\n", u16MDMiuSel);
                            continue;
                        }

                        if (u32InputMDAddr != 0)
                        {
                            MS_U8 *pu8InputMDData = NULL;
                            ST_KDRV_XC_HDR_INPUT_MD_FORMAT stFormatInfo;

                            pu8InputMDData = ((MS_U8 *)_pstDolbyHDRShareMem) + (u32InputMDAddr - _stShareMemInfo.phyBaseAddr);

                            if (pu8InputMDData == NULL)
                            {
                                printk("metadata mmap pa fail.\n");
                                continue;
                            }

                            bRetVal = getDolbyHDRInputMDFormatInfo(pu8InputMDData, &stFormatInfo);

                            if (bRetVal == TRUE)
                            {
                                MS_U32 u32RegsetAddr = 0;
                                MS_U16 u16LutMiuSel = 0;
                                KHAL_DS_STORED_ADDR_INFO stDSStoredAddr;
                                MS_U32 u32LutAddr = 0;
                                MS_U32 u32LutSize = 0;
                                MS_U8 *pu8LutAddr = NULL;

                                if ((stFormatInfo.u32ComposerAddr == 0) && ((stFormatInfo.u32DmAddr == 0)))
                                {
                                    printk("%d: addr fail.\n", __LINE__);
                                    continue;
                                }

                                XC_KDBG("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x %d %d\n", stFormatInfo.u8CurrentIndex, stFormatInfo.u32DmLength, stFormatInfo.u32DmAddr,
                                        stFormatInfo.u8DmMiuNo, stFormatInfo.u32ComposerLength, stFormatInfo.u32ComposerAddr, stFormatInfo.u8ComposerMiuNo,
                                        stFormatInfo.bEnableComposer, stFormatInfo.bEnableDM);
                                // get stored regset addr
                                u32RegsetAddr = stEntry.u32HDRRegsetAddr;
                                if (u32RegsetAddr == 0)
                                {
                                    printk("%d: u32HDRRegsetAddr fail.\n", __LINE__);
                                    continue;
                                }

                                u16LutMiuSel = getDolbyHDRLutMiuSel(_pstDolbyHDRShareMem);
                                if (MHal_XC_GetMiuOffset(u16LutMiuSel, &u32Offset) != TRUE)
                                {
                                    printk("%d: MHal_XC_GetMiuOffset fail.\n", __LINE__);
                                    continue;
                                }
                                // set stored regset addr into DS
                                MHal_XC_PrepareDolbyInfo();

                                memset(&stDSStoredAddr, 0, sizeof(KHAL_DS_STORED_ADDR_INFO));
                                stDSStoredAddr.stIPMAddr.phyDSAddr = (MS_PHY)(u32RegsetAddr + u32Offset);
                                stDSStoredAddr.stIPMAddr.u32DSSize = HDR_OUTPUT_REG_SIZE;
#if DOLBY_OTT_CACHED_BUFFER
                                stDSStoredAddr.stIPMAddr.virtDSAddr = (MS_VIRT)(_pu8RegSetAddr_Cached+ (u32RegsetAddr - (_stShareMemInfo.phyBaseAddr + HDR_MEM_COMMON_ENTRY_SIZE + HDR_TOTAL_INPUT_MD_SIZE)));
                                ulFlushAddr[0]=stDSStoredAddr.stIPMAddr.virtDSAddr;
                                ulFlushLen[0]=stDSStoredAddr.stIPMAddr.u32DSSize;
#else
                                stDSStoredAddr.stIPMAddr.virtDSAddr = (MS_VIRT)(_pu8RegSetAddr + (u32RegsetAddr - (_stShareMemInfo.phyBaseAddr + HDR_MEM_COMMON_ENTRY_SIZE + HDR_TOTAL_INPUT_MD_SIZE)));
#endif
                                KHal_SC_Set_DS_StoredAddr(&stDSStoredAddr);

                                u32LutAddr = stEntry.u32HDRLutAddr;
                                u32LutSize = getDolbyHDRLutSize(u32RPtr);
#if DOLBY_OTT_CACHED_BUFFER
                                pu8LutAddr = (MS_U8 *)(_pu8LutsAddr_Cached+ (u32LutAddr - (_stShareMemInfo.phyBaseAddr + HDR_MEM_COMMON_ENTRY_SIZE + HDR_TOTAL_INPUT_MD_SIZE + HDR_TOTAL_REGSET_SIZE)));
                                ulFlushAddr[1]=pu8LutAddr;
                                ulFlushLen[1]=u32LutSize;
#else
                                pu8LutAddr = (MS_U8 *)(_pu8LutsAddr + (u32LutAddr - (_stShareMemInfo.phyBaseAddr + HDR_MEM_COMMON_ENTRY_SIZE + HDR_TOTAL_INPUT_MD_SIZE + HDR_TOTAL_REGSET_SIZE)));
#endif
                                bRetVal = MHal_XC_ConfigHDRAutoDownloadStoredInfo((MS_PHY)(u32LutAddr + u32Offset), pu8LutAddr, u32LutSize);
                                if (bRetVal == FALSE)
                                {
                                    printk("MHal_XC_ConfigAutoDownload E_KDRV_XC_AUTODOWNLOAD_CLIENT_HDR fail.\n");
                                    continue;
                                }

                                // update composer
                                if (stFormatInfo.u32ComposerAddr != 0)
                                {
                                    MS_U8 *pu8CompData = NULL;

                                    if (stFormatInfo.u32ComposerLength == 0)
                                    {
                                        printk("%d: u32ComposerLength fail.\n", __LINE__);
                                        continue;
                                    }

                                    if (MHal_XC_GetMiuOffset(stFormatInfo.u8ComposerMiuNo, &u32Offset) != TRUE)
                                    {
                                        printk("%d: MHal_XC_GetMiuOffset fail.\n", __LINE__);
                                        continue;
                                    }
#if DOLBY_OTT_CACHED_BUFFER
                                    pu8CompData = _pu8InputMDAddr_Cached+ (stFormatInfo.u32ComposerAddr - _phyMDAddr);
                                    Chip_Inv_Cache_Range(pu8CompData, stFormatInfo.u32ComposerLength);
#else
                                    pu8CompData = _pu8InputMDAddr + (stFormatInfo.u32ComposerAddr - _phyMDAddr);
#endif

                                    if (pu8CompData == NULL)
                                    {
                                        printk("composer mmap pa fail.\n");
                                        continue;
                                    }

                                    // set compser data into color-format hw
                                    if(MHal_XC_SetDolbyCompData(pu8CompData, stFormatInfo.u32ComposerLength)== FALSE)
                                    {
                                        printk("MHal_XC_SetDolbyCompData fail, u32RPtr: %d, CFDReady: 0x%llx\n", _pstDolbyHDRShareMem->u32RdPtr, _pstDolbyHDRShareMem->u64Reserved);
                                    }
                                }

                                if (stFormatInfo.u32DmLength == 0)
                                {
                                    printk("%d: u32DmLength fail.\n", __LINE__);
                                    continue;
                                }

                                if (MHal_XC_GetMiuOffset(stFormatInfo.u8DmMiuNo, &u32Offset) != TRUE)
                                {
                                    printk("%d: MHal_XC_GetMiuOffset fail.\n", __LINE__);
                                    continue;
                                }
                                // turn on settings
                                MHal_XC_SetInputSourceType(E_KDRV_XC_INPUT_SOURCE_OTT);
                                MHal_XC_SetHDRType(E_KDRV_XC_HDR_DOLBY);

                                // update metadata
                                if ((stFormatInfo.bEnableDM) && (stFormatInfo.u32DmAddr != 0))
                                {
                                    MS_U8 *pu8Metadata = NULL;
#if DOLBY_OTT_CACHED_BUFFER
                                    pu8Metadata = _pu8InputMDAddr_Cached+ (stFormatInfo.u32DmAddr - _phyMDAddr);
                                    Chip_Inv_Cache_Range(pu8Metadata, stFormatInfo.u32DmLength);
#else
                                    pu8Metadata = _pu8InputMDAddr + (stFormatInfo.u32DmAddr - _phyMDAddr);
#endif

                                    if (pu8Metadata == NULL)
                                    {
                                        printk("metadata mmap pa fail.\n");
                                        continue;
                                    }

                                    // set metadata into color-format hw
                                    if (MHal_XC_SetDolbyMetaData(pu8Metadata, stFormatInfo.u32DmLength) == FALSE)
                                    {
                                        printk("MHal_XC_SetDolbyMetaData fail.\n");
                                    }
                                }

                                stEntry.u32HDRLutSize = MHal_XC_GetHDRAutoDownloadStoredSize();
                                stEntry.u32HDRRegsetSize = MHal_XC_GetRegsetCnt();
                            }
                        }
                        else
                        {
                            stEntry.u32HDRLutSize = MHal_XC_GetHDRAutoDownloadStoredSize();
                            stEntry.u32HDRRegsetSize = MHal_XC_GetRegsetCnt();
                        }

                        stEntry.u16CFDReady = TRUE;
#if DOLBY_OTT_CACHED_BUFFER
                        if (ulFlushAddr[0] != 0)
                        {
                            Chip_Flush_Cache_Range(ulFlushAddr[0],ulFlushLen[0]);
                        }
                        if (ulFlushAddr[1] != 0)
                        {
                            Chip_Flush_Cache_Range(ulFlushAddr[1],ulFlushLen[1]);
                        }
#endif
                        setDolbyHDRInfoEntry(_pstDolbyHDRShareMem, u32RPtr, &stEntry);
                        incDolbyHDRMemRPtr(_pstDolbyHDRShareMem,stEntry.u16Entry_Skip);
                    }
                    else
                    {
                        incDolbyHDRMemRPtr(_pstDolbyHDRShareMem,stEntry.u16Entry_Skip);
#if (XC_TIMER_TYPE == XC_HR_TIMER)
#else
                        spin_unlock_irq(&_spinlock_xc_dolby_hdr);
#if !DOLBY_OTT_MULTI_TIMER
                        _stXC_dolby_hdr_timer.function = _MDrv_XC_DolbyHDR_Timer_Callback;
                        _stXC_dolby_hdr_timer.expires = jiffies + HZ/100;
                        mod_timer(&_stXC_dolby_hdr_timer, _stXC_dolby_hdr_timer.expires);
#endif
                        return;
#endif
                    }
                }
            }
        }
        while(0);
    }

#if (XC_TIMER_TYPE == XC_HR_TIMER)
    spin_unlock(&_spinlock_xc_dolby_hdr);

    hrtimer_forward_now(timer, ns_to_ktime(MS_TO_NS(XC_TIMER_CHECK_TIME)));

    return HRTIMER_RESTART;
#else
    spin_unlock_irq(&_spinlock_xc_dolby_hdr);

#if !DOLBY_OTT_MULTI_TIMER
    _stXC_dolby_hdr_timer.function = _MDrv_XC_DolbyHDR_Timer_Callback;
    _stXC_dolby_hdr_timer.expires = jiffies + XC_TIMER_CHECK_TIME;
    mod_timer(&_stXC_dolby_hdr_timer, _stXC_dolby_hdr_timer.expires);
#endif
#endif
}

static MS_BOOL _MDrv_XC_InitDolbyHDRTimer()
{
#if DOLBY_OTT_MULTI_TIMER
    {
        int i=0;
        for(i=0; i<NR_CPUS; i++)
        {
            init_timer(&astXC_dolby_hdr_timer[i]);
        }
    }
#elif (XC_TIMER_TYPE == XC_HR_TIMER)
    hrtimer_init(&_stXC_dolby_hdr_hrtimer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
#else
    init_timer(&_stXC_dolby_hdr_timer);
#endif

    return TRUE;
}

static MS_BOOL _MDrv_XC_DelDolbyHDRTimer()
{
    if (_bTimerStart == TRUE)
    {
        XC_KDBG("del Timer.\n");
        mutex_lock(&_hdr_mutex);
#if DOLBY_OTT_MULTI_TIMER
        {
            int i=0;
            for(i=0; i<NR_CPUS; i++)
            {
                del_timer_sync(&astXC_dolby_hdr_timer[i]);
            }
        }
#elif (XC_TIMER_TYPE == XC_HR_TIMER)
        hrtimer_cancel(&_stXC_dolby_hdr_hrtimer);
#else
        del_timer_sync(&_stXC_dolby_hdr_timer);
#endif
        _bTimerStart = FALSE;

        mutex_unlock(&_hdr_mutex);
    }

    return TRUE;
}

static MS_BOOL _MDrv_XC_StartDolbyHDRTimer()
{
    if (_bTimerStart == FALSE)
    {
        XC_KDBG("start Timer.\n");
        mutex_lock(&_hdr_mutex);
#if DOLBY_OTT_MULTI_TIMER
        {
            int i=0;
            int msdelay=jiffies_to_msecs(XC_TIMER_CHECK_TIME)/NR_CPUS;
            for(i=0; i<NR_CPUS; i++)
            {
                if(FALSE == timer_pending(&astXC_dolby_hdr_timer[i])) //no call back timer, then add one
                {
                    astXC_dolby_hdr_timer[i].function = _MDrv_XC_DolbyHDR_Timer_Callback;
                    astXC_dolby_hdr_timer[i].expires = jiffies + XC_TIMER_CHECK_TIME;
                    astXC_dolby_hdr_timer[i].data=i;
                    if(cpu_online(i))
                        add_timer_on(&astXC_dolby_hdr_timer[i],i);
                    else
                        add_timer(&astXC_dolby_hdr_timer[i]);
                    msleep(msdelay);
                }
            }
        }
#elif (XC_TIMER_TYPE == XC_HR_TIMER)
        if(FALSE == hrtimer_active(&_stXC_dolby_hdr_hrtimer)) //no call back timer, then add one
        {
            _MDrv_XC_DolbyHDR_Timer_Callback(&_stXC_dolby_hdr_hrtimer);

            _stXC_dolby_hdr_hrtimer.function = _MDrv_XC_DolbyHDR_Timer_Callback;
            hrtimer_start(&_stXC_dolby_hdr_hrtimer, ns_to_ktime(MS_TO_NS(XC_TIMER_CHECK_TIME)), HRTIMER_MODE_REL);//2ms
        }
#else
        if(FALSE == timer_pending(&_stXC_dolby_hdr_timer)) //no call back timer, then add one
        {
            _stXC_dolby_hdr_timer.function = _MDrv_XC_DolbyHDR_Timer_Callback;
            _stXC_dolby_hdr_timer.expires = jiffies + XC_TIMER_CHECK_TIME;
            mod_timer(&_stXC_dolby_hdr_timer, _stXC_dolby_hdr_timer.expires);
        }

        _MDrv_XC_DolbyHDR_Timer_Callback();
#endif
        _bTimerStart = TRUE;

        mutex_unlock(&_hdr_mutex);
    }

    return TRUE;
}

static MS_BOOL _CFD_GetShareMemData(MS_U8 *pu8Data, ST_KDRV_XC_SHARE_MEMORY_FORMAT_Info *pstFormatInfo, MS_U8 u8Win, EN_KDRV_XC_CFD_HDR_TYPE enHDRtype)
{
    if (pu8Data != NULL)
    {
        if(u8Win == 0)
        {
            //Main Window
            pstFormatInfo->u8Version = *(pu8Data);
            pu8Data += 1;

            // what we get from dram must equal to what we want to retrive
            if ((pstFormatInfo->u8Version == 0) && (enHDRtype == E_KDRV_XC_CFD_HDR_TYPE_DOLBY)) // Dolby HDR == E_KDRV_XC_CFD_HDR_TYPE_DOLBY
            {
                pstFormatInfo->HDRMemFormat.stHDRMemFormatDolby.u8CurrentIndex = *(pu8Data);
                pu8Data += 1;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatDolby.u32DmLength = (*pu8Data) | (*(pu8Data + 1) << 8) | (*(pu8Data + 2) << 16) | (*(pu8Data + 3) << 24);
                pu8Data += 4;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatDolby.u32DmAddr = (*pu8Data) | (*(pu8Data + 1) << 8) | (*(pu8Data + 2) << 16) | (*(pu8Data + 3) << 24);
                pu8Data += 4;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatDolby.u8DmMiuNo = *(pu8Data);
                pu8Data += 1;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatDolby.u32ComposerLength = (*pu8Data) | (*(pu8Data + 1) << 8) | (*(pu8Data + 2) << 16) | (*(pu8Data + 3) << 24);
                pu8Data += 4;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatDolby.u32ComposerAddr = (*pu8Data) | (*(pu8Data + 1) << 8) | (*(pu8Data + 2) << 16) | (*(pu8Data + 3) << 24);
                pu8Data += 4;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatDolby.u8ComposerMiuNo = *(pu8Data);
                pu8Data += 1;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatDolby.bEnableComposer = *(pu8Data);
                pu8Data += 1;

                if ((pstFormatInfo->HDRMemFormat.stHDRMemFormatDolby.u32DmAddr ==0) &&
                      (pstFormatInfo->HDRMemFormat.stHDRMemFormatDolby.u32ComposerAddr == 0))
                {
                    return FALSE;
                }
            }
            else if ((pstFormatInfo->u8Version == 2) && (enHDRtype == E_KDRV_XC_CFD_HDR_TYPE_DOLBY))
            {

            }
            else if((pstFormatInfo->u8Version == 1) && (enHDRtype == E_KDRV_XC_CFD_HDR_TYPE_OPEN)) // CFD HDR == E_KDRV_XC_CFD_HDR_TYPE_OPEN
            {
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u8CurrentIndex = *(pu8Data);
                pu8Data += 1;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u8InputFormat = *(pu8Data);
                pu8Data += 1;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u8InputDataFormat = *(pu8Data);
                pu8Data += 1;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u8Video_Full_Range_Flag = *(pu8Data);
                pu8Data += 1;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.bVUIValid = *(pu8Data);
                pu8Data += 1;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u8Colour_primaries = *(pu8Data);
                pu8Data += 1;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u8Transfer_Characteristics = *(pu8Data);
                pu8Data += 1;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u8Matrix_Coeffs = *(pu8Data);
                pu8Data += 1;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.bSEIValid = *(pu8Data);
                pu8Data += 1;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16Display_Primaries_x[0] = (*pu8Data) | (*(pu8Data + 1) << 8);
                pu8Data += 2;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16Display_Primaries_x[1] = (*pu8Data) | (*(pu8Data + 1) << 8);
                pu8Data += 2;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16Display_Primaries_x[2] = (*pu8Data) | (*(pu8Data + 1) << 8);
                pu8Data += 2;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16Display_Primaries_y[0] = (*pu8Data) | (*(pu8Data + 1) << 8);
                pu8Data += 2;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16Display_Primaries_y[1] = (*pu8Data) | (*(pu8Data + 1) << 8);
                pu8Data += 2;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16Display_Primaries_y[2] = (*pu8Data) | (*(pu8Data + 1) << 8);
                pu8Data += 2;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16White_point_x = (*pu8Data) | (*(pu8Data + 1) << 8);
                pu8Data += 2;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16White_point_y = (*pu8Data) | (*(pu8Data + 1) << 8);
                pu8Data += 2;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u32Master_Panel_Max_Luminance = (*pu8Data) | (*(pu8Data + 1) << 8) | (*(pu8Data + 2) << 16) | (*(pu8Data + 3) << 24);
                pu8Data += 4;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u32Master_Panel_Min_Luminance = (*pu8Data) | (*(pu8Data + 1) << 8) | (*(pu8Data + 2) << 16) | (*(pu8Data + 3) << 24);
                pu8Data += 4;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.bContentLightLevelEnabled = (*pu8Data);
                pu8Data += 1;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16maxContentLightLevel = (*pu8Data) | (*(pu8Data + 1) << 8);
                pu8Data += 2;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16maxPicAverageLightLevel = (*pu8Data) | (*(pu8Data + 1) << 8);
                pu8Data += 2;
                return TRUE;
            }
#ifdef      SUPPORT_TCH
            else if((pstFormatInfo->u8Version == 3) && (enHDRtype == E_KDRV_XC_CFD_HDR_TYPE_TCH))
            {
            }
#endif
            else
            {
                return FALSE;
            }
        }
        else if(u8Win == 1)
        {
            //Sub Window
            pu8Data += 64;
            pstFormatInfo->u8Version = *(pu8Data);
            pu8Data += 1;

            if((pstFormatInfo->u8Version == 1) && (enHDRtype == E_KDRV_XC_CFD_HDR_TYPE_OPEN)) // CFD HDR == E_KDRV_XC_CFD_HDR_TYPE_OPEN
            {
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u8CurrentIndex = *(pu8Data);
                pu8Data += 1;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u8InputFormat = *(pu8Data);
                pu8Data += 1;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u8InputDataFormat = *(pu8Data);
                pu8Data += 1;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u8Video_Full_Range_Flag = *(pu8Data);
                pu8Data += 1;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.bVUIValid = *(pu8Data);
                pu8Data += 1;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u8Colour_primaries = *(pu8Data);
                pu8Data += 1;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u8Transfer_Characteristics = *(pu8Data);
                pu8Data += 1;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u8Matrix_Coeffs = *(pu8Data);
                pu8Data += 1;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.bSEIValid = *(pu8Data);
                pu8Data += 1;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16Display_Primaries_x[0] = (*pu8Data) | (*(pu8Data + 1) << 8);
                pu8Data += 2;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16Display_Primaries_x[1] = (*pu8Data) | (*(pu8Data + 1) << 8);
                pu8Data += 2;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16Display_Primaries_x[2] = (*pu8Data) | (*(pu8Data + 1) << 8);
                pu8Data += 2;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16Display_Primaries_y[0] = (*pu8Data) | (*(pu8Data + 1) << 8);
                pu8Data += 2;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16Display_Primaries_y[1] = (*pu8Data) | (*(pu8Data + 1) << 8);
                pu8Data += 2;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16Display_Primaries_y[2] = (*pu8Data) | (*(pu8Data + 1) << 8);
                pu8Data += 2;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16White_point_x = (*pu8Data) | (*(pu8Data + 1) << 8);
                pu8Data += 2;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16White_point_y = (*pu8Data) | (*(pu8Data + 1) << 8);
                pu8Data += 2;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u32Master_Panel_Max_Luminance = (*pu8Data) | (*(pu8Data + 1) << 8) | (*(pu8Data + 2) << 16) | (*(pu8Data + 3) << 24);
                pu8Data += 4;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u32Master_Panel_Min_Luminance = (*pu8Data) | (*(pu8Data + 1) << 8) | (*(pu8Data + 2) << 16) | (*(pu8Data + 3) << 24);
                pu8Data += 4;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.bContentLightLevelEnabled = (*pu8Data);
                pu8Data += 1;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16maxContentLightLevel = (*pu8Data) | (*(pu8Data + 1) << 8);
                pu8Data += 2;
                pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16maxPicAverageLightLevel = (*pu8Data) | (*(pu8Data + 1) << 8);
                pu8Data += 2;
            }
            else
            {
                return FALSE;
            }
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }
    return TRUE;
}

/// remove miu base addr
MS_BOOL _MDrv_XC_AddrNormalization(MS_U64 *phyBaseAddr)
{
    if (*phyBaseAddr > ARM_MIU3_BASE_ADDR)
    {
        *phyBaseAddr -= ARM_MIU3_BASE_ADDR;
    }
    else if (*phyBaseAddr > ARM_MIU2_BASE_ADDR)
    {
        *phyBaseAddr -= ARM_MIU2_BASE_ADDR;
    }
    else if (*phyBaseAddr > ARM_MIU1_BASE_ADDR)
    {
        *phyBaseAddr -= ARM_MIU1_BASE_ADDR;
    }
    else if (*phyBaseAddr > ARM_MIU0_BASE_ADDR)
    {
        *phyBaseAddr -= ARM_MIU0_BASE_ADDR;
    }
    return TRUE;
}
#ifdef      SUPPORT_TCH
//[TCH]
#include "technicolor_driver.h"
MS_U32 u32logct = 0;
#define PRI_NUM 3000
#endif

#ifdef SUPPORT_SEAMLESS_HDR
static void _MHal_XC_Convert_XCDRAM_to_CFDMMParam(MS_BOOL bIsHdMode, STU_CFDAPI_MM_PARSER *pstMMParam, ST_KDRV_XC_SHARE_MEMORY_FORMAT_Info *pstFormatInfo)
{
    if ((pstFormatInfo == NULL) || (pstMMParam == NULL))
    {
        return -1;
    }

    pstMMParam->u32Version                            = CFD_MM_ST_VERSION;
    pstMMParam->u16Length                             = sizeof(STU_CFDAPI_MM_PARSER);
    pstMMParam->u8Video_Full_Range_Flag    = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u8Video_Full_Range_Flag;

    if (pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.bVUIValid == FALSE)
    {
        // Algorithm team's suggestion, when VUI is not valid, please assign them the value of following
        if(bIsHdMode == TRUE)
        {
            // HD default value
            pstMMParam->u8Colour_primaries          = E_CFD_CFIO_CP_BT709_SRGB_SYCC;
            pstMMParam->u8Transfer_Characteristics  = E_CFD_CFIO_TR_BT709;
            pstMMParam->u8Matrix_Coeffs             = E_CFD_CFIO_MC_BT709_XVYCC709;
        }
        else
        {
            // SD default value
            pstMMParam->u8Colour_primaries          = E_CFD_CFIO_CP_BT601625;
            pstMMParam->u8Transfer_Characteristics  = E_CFD_CFIO_TR_BT601525_601625;
            pstMMParam->u8Matrix_Coeffs             = E_CFD_CFIO_MC_BT601625_XVYCC601_SYCC;
        }
    }
    else
    {
        pstMMParam->u8Colour_primaries            = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u8Colour_primaries;
        pstMMParam->u8Transfer_Characteristics    = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u8Transfer_Characteristics;
        pstMMParam->u8Matrix_Coeffs               = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u8Matrix_Coeffs;
    }

    //for mastering SEI
    //The order in MM is G->B->R
    //the order in CFD mm is R->G->B
    pstMMParam->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_x[0] = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16Display_Primaries_x[2];
    pstMMParam->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_x[1] = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16Display_Primaries_x[0];
    pstMMParam->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_x[2] = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16Display_Primaries_x[1];
    pstMMParam->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_y[0] = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16Display_Primaries_y[2];
    pstMMParam->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_y[1] = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16Display_Primaries_y[0];
    pstMMParam->stu_Cfd_MM_MasterPanel_ColorMetry.u16Display_Primaries_y[2] = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16Display_Primaries_y[1];
    pstMMParam->stu_Cfd_MM_MasterPanel_ColorMetry.u16White_point_x          = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16White_point_x;
    pstMMParam->stu_Cfd_MM_MasterPanel_ColorMetry.u16White_point_y          = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16White_point_y;
    pstMMParam->u32Master_Panel_Max_Luminance                               = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u32Master_Panel_Max_Luminance;
    pstMMParam->u32Master_Panel_Min_Luminance                               = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u32Master_Panel_Min_Luminance;
    pstMMParam->u8Mastering_Display_Infor_Valid                             = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.bSEIValid;

    pstMMParam->u8MM_HDR_ContentLightMetaData_Valid     = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.bContentLightLevelEnabled;
    pstMMParam->u16Max_content_light_level        = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16maxContentLightLevel;
    pstMMParam->u16Max_pic_average_light_level     = pstFormatInfo->HDRMemFormat.stHDRMemFormatCFD.u16maxPicAverageLightLevel;

    //treat default value as not valid
    if (0xff == pstMMParam->u8MM_HDR_ContentLightMetaData_Valid)
    {
        pstMMParam->u8MM_HDR_ContentLightMetaData_Valid = 0;
        pstMMParam->u16Max_content_light_level = 0;
        pstMMParam->u16Max_pic_average_light_level = 0;
    }

    if ((0 != pstMMParam->u16Max_content_light_level) ||(0 != pstMMParam->u16Max_pic_average_light_level))
    {
        pstMMParam->u8MM_HDR_ContentLightMetaData_Valid = 1;
    }

    memcpy(&gstMMParam,pstMMParam,sizeof(STU_CFDAPI_MM_PARSER));
    return E_CFD_MC_ERR_NOERR;
}
#endif
void MDrv_XC_ProcessCFDIRQ(void)
{
    MS_U16 u16IRQValue = 0;
    MS_S8 s8Status = 0;
    mutex_lock(&_cfd_mutex);
    // read XC ISR status
    u16IRQValue = MHal_XC_R2BYTE(REG_SC_VSYNC_IRQ);
    XC_KDBG("MDrv_XC_ProcessCFDIRQ interrupt 0x%04x.\n", u16IRQValue);
    s8Status = 0;

    if (u16IRQValue & BIT(5))
    {
        ST_KDRV_XC_CFD_INIT stCfdInit[2];
        stCfdInit[0].u8Win = 0; // retrieve Main window data
        stCfdInit[1].u8Win = 1; // retrieve Sub window data
        _u32NoSignalCount = 0;

        // main window process START
        if (MHal_XC_CFD_GetInitParam(&stCfdInit[0]) == E_CFD_MC_ERR_NOERR)
        {
            if (IS_DOLBY_HDR(stCfdInit[0].u8Win))
            {
                if (CFD_IS_MM(stCfdInit[0].u8InputSource))
                {
                    MS_U8 *pu8CompData = NULL;
                    MS_U8 *pu8Metadata = NULL;
                    MS_BOOL bComposerIOUnmap = FALSE;
                    MS_BOOL bDMIOUnmap = FALSE;

                    // get share memory format data
                    ST_KDRV_XC_SHARE_MEMORY_FORMAT_Info stFormatInfo;
                    memset(&stFormatInfo, 0, sizeof(ST_KDRV_XC_SHARE_MEMORY_FORMAT_Info));
                    if (_CFD_GetShareMemData(_pu8ShareMemBaseAddr, &stFormatInfo, stCfdInit[0].u8Win, E_KDRV_XC_CFD_HDR_TYPE_DOLBY) == TRUE)
                    {
                        if (stFormatInfo.u8Version == 0)
                        {
                            if (MHal_XC_SupportDolbyHDR() == TRUE)
                            {
                                MS_U32 u32Offset = 0;
                                XC_KDBG(KERN_CRIT "0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", stFormatInfo.HDRMemFormat.stHDRMemFormatDolby.u8CurrentIndex, stFormatInfo.HDRMemFormat.stHDRMemFormatDolby.u32DmLength, stFormatInfo.HDRMemFormat.stHDRMemFormatDolby.u32DmAddr,
                                        stFormatInfo.HDRMemFormat.stHDRMemFormatDolby.u8DmMiuNo, stFormatInfo.HDRMemFormat.stHDRMemFormatDolby.u32ComposerLength, stFormatInfo.HDRMemFormat.stHDRMemFormatDolby.u32ComposerAddr, stFormatInfo.HDRMemFormat.stHDRMemFormatDolby.u8ComposerMiuNo,
                                        stFormatInfo.HDRMemFormat.stHDRMemFormatDolby.bEnableComposer);

                                // START : METADATA shared memory check
                                if (stFormatInfo.HDRMemFormat.stHDRMemFormatDolby.u32DmAddr == 0)
                                {
                                    s8Status = -1;
                                }

                                if (stFormatInfo.HDRMemFormat.stHDRMemFormatDolby.u32DmLength == 0)
                                {
                                    s8Status = -1;
                                }

                                if (MHal_XC_GetMiuOffset(stFormatInfo.HDRMemFormat.stHDRMemFormatDolby.u8DmMiuNo, &u32Offset) != TRUE)
                                {
                                    s8Status = -1;
                                }

                                if (pfn_valid(__phys_to_pfn(stFormatInfo.HDRMemFormat.stHDRMemFormatDolby.u32DmAddr + u32Offset)))
                                {
                                    pu8Metadata = __va(stFormatInfo.HDRMemFormat.stHDRMemFormatDolby.u32DmAddr + u32Offset);
                                }
                                else
                                {
                                    pu8Metadata = (MS_U8 __iomem *)ioremap_cached(stFormatInfo.HDRMemFormat.stHDRMemFormatDolby.u32DmAddr + u32Offset, stFormatInfo.HDRMemFormat.stHDRMemFormatDolby.u32DmLength);
                                    bDMIOUnmap = TRUE;
                                }

                                if (pu8Metadata == NULL)
                                {
                                    XC_KDBG("metadata mmap pa fail.\n");
                                    s8Status = -1;
                                }
                                // END : METADATA shared memory check

                                // START : COMPOSER shared memory check
                                if (stFormatInfo.HDRMemFormat.stHDRMemFormatDolby.u32ComposerAddr == 0)
                                {
                                    s8Status = -1;
                                }

                                if (stFormatInfo.HDRMemFormat.stHDRMemFormatDolby.u32ComposerLength == 0)
                                {
                                    s8Status = -1;
                                }

                                if (MHal_XC_GetMiuOffset(stFormatInfo.HDRMemFormat.stHDRMemFormatDolby.u8ComposerMiuNo, &u32Offset) != TRUE)
                                {
                                    s8Status = -1;
                                }

                                if (pfn_valid(__phys_to_pfn(stFormatInfo.HDRMemFormat.stHDRMemFormatDolby.u32ComposerAddr + u32Offset)))
                                {
                                    pu8CompData = __va(stFormatInfo.HDRMemFormat.stHDRMemFormatDolby.u32ComposerAddr + u32Offset);
                                }
                                else
                                {
                                    pu8CompData = (MS_U8 __iomem *)ioremap_cached(stFormatInfo.HDRMemFormat.stHDRMemFormatDolby.u32ComposerAddr + u32Offset, stFormatInfo.HDRMemFormat.stHDRMemFormatDolby.u32ComposerLength);
                                    bComposerIOUnmap = TRUE;
                                }

                                if (pu8CompData == NULL)
                                {
                                    XC_KDBG("composer mmap pa fail.\n");
                                    s8Status = -1;
                                }
                                // END : COMPOSER shared memory check

                                if (s8Status >= 0)
                                {
                                    MS_BOOL bMetaDataChanged = FALSE;
                                    XC_KDBG("XC OTT dolby\n");

                                    // patch here, needs to be removed after CFD has fixed
                                    _ISMMDolby = TRUE;
                                    // turn on settings
                                    MHal_XC_SetInputSourceType(E_KDRV_XC_INPUT_SOURCE_OTT);
                                    MHal_XC_SetHDRType(E_KDRV_XC_HDR_DOLBY);

                                    if (stFormatInfo.HDRMemFormat.stHDRMemFormatDolby.bEnableComposer)
                                    {
                                        // enable enhanced layer
                                        MHal_XC_EnableEL(TRUE);
                                    }
                                    else
                                    {
                                        // disable enhanced layer
                                        MHal_XC_EnableEL(FALSE);
                                    }

                                    bMetaDataChanged = (stFormatInfo.HDRMemFormat.stHDRMemFormatDolby.u8CurrentIndex != _u16CurrentIndx) ? TRUE : FALSE;
                                    // metadata changed
                                    if (bMetaDataChanged == TRUE)
                                    {
                                        // update composer
                                        // set compser data into color-format hw
                                        if(MHal_XC_SetDolbyCompData(pu8CompData, stFormatInfo.HDRMemFormat.stHDRMemFormatDolby.u32ComposerLength)==FALSE)
                                        {
                                            printk("MHal_XC_SetDolbyCompData fail, u32RPtr: %d\n", _pstDolbyHDRShareMem->u32RdPtr);
                                        }
                                        //iounmap composer addr
                                        if ((pu8CompData != NULL) && (bComposerIOUnmap == TRUE))
                                        {
                                            iounmap(pu8CompData);
                                            pu8CompData = NULL;
                                        }

                                        // update metadata
                                        // set metadata into color-format hw
                                        MHal_XC_SetDolbyMetaData(pu8Metadata, stFormatInfo.HDRMemFormat.stHDRMemFormatDolby.u32DmLength);

                                        //iounmap dm addr
                                        if ((pu8Metadata != NULL) && (bDMIOUnmap == TRUE))
                                        {
                                            iounmap(pu8Metadata);
                                            pu8Metadata = NULL;
                                        }

                                        _u16CurrentIndx = stFormatInfo.HDRMemFormat.stHDRMemFormatDolby.u8CurrentIndex;
                                    }
                                }
                                else
                                {
                                    // patch here, needs to be removed after CFD has fixed
                                    _ISMMDolby = TRUE;
                                    MHal_XC_MuteHDR();
                                }
                            }
                        }
                        else if (stFormatInfo.u8Version == 2)
                        {
                            _ISMMDolby = TRUE;
                            // turn on settings
                            MHal_XC_SetInputSourceType(E_KDRV_XC_INPUT_SOURCE_OTT);
                            MHal_XC_SetHDRType(E_KDRV_XC_HDR_DOLBY);

                            if (MHal_XC_SupportDolbyHDR() == TRUE)
                            {
                                if (_pstDolbyHDRShareMem == NULL)
                                {
                                    _pstDolbyHDRShareMem = (ST_KDRV_XC_HDR_DOLBY_MEMORY_FORMAT_EX *)_pu8ShareMemBaseAddr;
                                }

                                if (_phyMDAddr != _pstDolbyHDRShareMem->u32MDAddr)
                                {
                                    if (_pu8InputMDAddr != NULL)
                                    {
                                        if(_bIOUnmapInputMDMem == TRUE)
                                        {
                                            iounmap(_pu8InputMDAddr);
#if DOLBY_OTT_CACHED_BUFFER
                                            iounmap(_pu8InputMDAddr_Cached);
#endif
                                        }
                                        _pu8InputMDAddr = NULL;
                                    }

                                    if (_pu8InputMDAddr == NULL)
                                    {
                                        MS_U32 u32Offset = 0;
                                        _phyMDAddr = _pstDolbyHDRShareMem->u32MDAddr;
                                        MHal_XC_GetMiuOffset(_pstDolbyHDRShareMem->u16MDMiuSel, &u32Offset);
                                        if (pfn_valid(__phys_to_pfn(_phyMDAddr + u32Offset)))
                                        {
                                            _pu8InputMDAddr = __va(_phyMDAddr + u32Offset);
#if DOLBY_OTT_CACHED_BUFFER
                                            _pu8InputMDAddr_Cached= _pu8InputMDAddr;
#endif
                                            _bIOUnmapInputMDMem = FALSE;
                                        }
                                        else
                                        {
#if DOLBY_OTT_CACHED_BUFFER
                                            _pu8InputMDAddr_Cached= (MS_U8 __iomem *)ioremap_cached(_phyMDAddr + u32Offset, XC_INPUT_MD_MAX_SIZE);  //leo add
#endif
                                            _pu8InputMDAddr = (MS_U8 __iomem *)ioremap_cached(_phyMDAddr + u32Offset, XC_INPUT_MD_MAX_SIZE);
                                            _bIOUnmapInputMDMem = TRUE;
                                        }
                                    }
                                }

                                if (_pu8RegSetAddr == NULL)
                                {
                                    _pu8RegSetAddr = _pu8ShareMemBaseAddr + HDR_MEM_COMMON_ENTRY_SIZE + HDR_TOTAL_INPUT_MD_SIZE;
#if DOLBY_OTT_CACHED_BUFFER
                                    _pu8RegSetAddr_Cached= _pu8ShareMemBaseAddr_Cached+ HDR_MEM_COMMON_ENTRY_SIZE + HDR_TOTAL_INPUT_MD_SIZE;
#endif
                                }

                                if (_pu8LutsAddr == NULL)
                                {
                                    _pu8LutsAddr = _pu8ShareMemBaseAddr + HDR_MEM_COMMON_ENTRY_SIZE + HDR_TOTAL_INPUT_MD_SIZE + HDR_TOTAL_REGSET_SIZE;
#if DOLBY_OTT_CACHED_BUFFER
                                    _pu8LutsAddr_Cached= _pu8ShareMemBaseAddr_Cached+ HDR_MEM_COMMON_ENTRY_SIZE + HDR_TOTAL_INPUT_MD_SIZE + HDR_TOTAL_REGSET_SIZE;
#endif
                                }

                                _MDrv_XC_StartDolbyHDRTimer();
                            }
                            else
                            {
                                MHal_XC_MuteHDR();
                            }
                        }
                    }
                    else
                    {
                        // patch here, needs to be removed after CFD has fixed
                        _ISMMDolby = FALSE;
                    }
                }
                else
                {
                    // patch here, needs to be removed after CFD has fixed
                    _ISMMDolby = FALSE;
                    // reset HDR type and Input source Type When leave DB MM.
                    _enHDRType = E_KDRV_XC_HDR_NONE;
                    _enInputSourceType = E_KDRV_XC_INPUT_SOURCE_NONE;
                }
            }
            else
            {
                // patch here, needs to be removed after CFD has fixed
                _ISMMDolby = FALSE;
            }
#ifdef      SUPPORT_TCH
            u32logct++;
            if(u32logct == PRI_NUM)
            {
                printk("[%s][%d] TC_HDR_Print_Start, " ,__FUNCTION__,__LINE__);
            }

            if (IS_TC_HDR(stCfdInit[0].u8Win))
            {
                if(u32logct == PRI_NUM)
                {
                    printk(" a, ");
                }

                if (CFD_IS_MM(stCfdInit[0].u8InputSource))
                {
                    if(u32logct == PRI_NUM)
                    {
                        printk(" b, ");
                    }

                    MS_U8 *pu8CompData = NULL;
                    MS_U8 *pu8Metadata = NULL;
                    MS_BOOL bComposerIOUnmap = FALSE;
                    MS_BOOL bDMIOUnmap = FALSE;

                    // get share memory format data
                    ST_KDRV_XC_SHARE_MEMORY_FORMAT_Info stFormatInfo;
                    if (_CFD_GetShareMemData(_pu8ShareMemBaseAddr, &stFormatInfo, stCfdInit[0].u8Win, E_KDRV_XC_CFD_HDR_TYPE_TCH) == TRUE)
                    {
                        if(u32logct == PRI_NUM)
                        {
                            printk(" c, ");
                        }

                        if (stFormatInfo.u8Version == 3)
                        {
                            _ISTCH = TRUE;
                            // turn on settings
                            //MHal_XC_SetInputSourceType(E_KDRV_XC_INPUT_SOURCE_OTT);
                            MHal_XC_SetHDRType(E_KDRV_XC_HDR_TECHNICOLOR);

                            if (MHal_XC_SupportTCH() == TRUE)
                            {
                                if(u32logct == PRI_NUM)
                                {
                                    printk(" d, ");
                                }

                                if (_pstTCHDRShareMem == NULL)
                                {
                                    _pstTCHDRShareMem = (ST_KDRV_XC_HDR_TC_MEMORY_FORMAT_EX *)_pu8ShareMemBaseAddr;
                                }

                                if (_phyMDAddr != _pstTCHDRShareMem->u32MDAddr)
                                {
                                    if (_pu8InputMDAddr != NULL)
                                    {
                                        if(_bIOUnmapInputMDMem == TRUE)
                                        {
                                            iounmap(_pu8InputMDAddr);
                                        }
                                        _pu8InputMDAddr = NULL;
                                    }

                                    if (_pu8InputMDAddr == NULL)
                                    {
                                        MS_U32 u32Offset = 0;
                                        _phyMDAddr = _pstTCHDRShareMem->u32MDAddr;
                                        MHal_XC_GetMiuOffset(_pstTCHDRShareMem->u16MDMiuSel, &u32Offset);
                                        if (pfn_valid(__phys_to_pfn(_phyMDAddr + u32Offset)))
                                        {
                                            _pu8InputMDAddr = __va(_phyMDAddr + u32Offset);
                                            _bIOUnmapInputMDMem = FALSE;
                                        }
                                        else
                                        {
                                            _pu8InputMDAddr = (MS_U8 __iomem *)ioremap_wc(_phyMDAddr + u32Offset, XC_INPUT_MD_MAX_SIZE);
                                            _bIOUnmapInputMDMem = TRUE;
                                        }
                                    }
                                }

                                if (_pu8RegSetAddr == NULL)
                                {
                                    _pu8RegSetAddr = _pu8ShareMemBaseAddr + TC_HDR_MEM_COMMON_ENTRY_SIZE + TC_HDR_TOTAL_INPUT_MD_SIZE;
                                }

                                _bTCHStart = TRUE;
                                //_MDrv_XC_StartDolbyHDRTimer();
                            }
                            else
                            {
                                MHal_XC_MuteHDR();
                            }
                        }
                    }
                    else
                    {
                        // patch here, needs to be removed after CFD has fixed
                        _ISTCH = FALSE;
                        _bTCHStart = FALSE;
                    }

                    if (stFormatInfo.u8Version == 0xFF)
                    {
                        _ISTCH = FALSE;
                        _bTCHStart = FALSE;
                    }
                }
                else
                {
                    // patch here, needs to be removed after CFD has fixed
                    _ISTCH = FALSE;
                    _bTCHStart = FALSE;
                    // reset HDR type and Input source Type When leave DB MM.
                    _enHDRType = E_KDRV_XC_HDR_NONE;
                    _enInputSourceType = E_KDRV_XC_INPUT_SOURCE_NONE;
                }
            }
            else
            {
                // patch here, needs to be removed after CFD has fixed
                _ISTCH = FALSE;
                _bTCHStart = FALSE;
            }
            if(u32logct == PRI_NUM)
            {
                printk("isTCH %d, startTCH %d, TCH_End_Print\n",_ISTCH,_bTCHStart);
            }
#endif
            if (CFD_IS_MM(stCfdInit[0].u8InputSource) || CFD_IS_DTV(stCfdInit[0].u8InputSource))
            {
                // for main window, get share memory format data
                ST_KDRV_XC_SHARE_MEMORY_FORMAT_Info stFormatInfo;
#if 1 // patch
                memset(&stFormatInfo, 0, sizeof(ST_KDRV_XC_SHARE_MEMORY_FORMAT_Info));
#endif
                if (_CFD_GetShareMemData(_pu8ShareMemBaseAddr, &stFormatInfo, stCfdInit[0].u8Win, E_KDRV_XC_CFD_HDR_TYPE_OPEN) == TRUE)
                {
                    // turn on settings
                    if ((IS_OPEN_HDR(stCfdInit[0].u8Win)) || (IS_HLG_HDR(stCfdInit[0].u8Win)))
                    {
                        MHal_XC_SetInputSourceType(E_KDRV_XC_INPUT_SOURCE_OTT);
                        MHal_XC_SetHDRType(E_KDRV_XC_HDR_OPEN);  // marked on purpose, because now it is controlled by CFD driver.
                        // patch here, needs to be removed after CFD has fixed
                        _ISMMDolby = FALSE;
                    }
                    MHal_XC_CFD_DepositeMMParam(&stFormatInfo, stCfdInit[0].u8Win);

#ifdef SUPPORT_SEAMLESS_HDR
                    if (MHAL_XC_IsCFDInitFinished() == TRUE)
                    {
                        STU_CFDAPI_MM_PARSER stMMParam;
                        MS_U16 u16RetVal = 0;
                        MS_BOOL bChanged = FALSE;
                        ST_KDRV_XC_SHARE_MEMORY_FORMAT_Info stFormatInfo;
                        memset(&stMMParam, 0, sizeof(STU_CFDAPI_MM_PARSER));

                        MHal_XC_CFD_WithdrawMMParam(&stFormatInfo, stCfdInit[0].u8Win);
                        _MHal_XC_Convert_XCDRAM_to_CFDMMParam(TRUE, &stMMParam,&stFormatInfo);

                        if(memcmp(&stMMParam, &_stCfdMm[stCfdInit[0].u8Win], sizeof(STU_CFDAPI_MM_PARSER)) != 0)
                        {
                            memcpy(&_stCfdMm[stCfdInit[0].u8Win], &stMMParam, sizeof(STU_CFDAPI_MM_PARSER));

                            u16RetVal = Mapi_Cfd_inter_MM_Param_Check(&stMMParam);
                            Mapi_Cfd_inter_MM_Param_Set(&stMMParam);
                            if (u16RetVal == E_CFD_MC_ERR_NOERR)
                            {
                                //printk("Mapi_Cfd_inter_MM_Param_Check\n");
                            }
                            else
                            {
                                printk("Mapi_Cfd_inter_MM_Param_Check fail, errCode: %d\n", u16RetVal);
                            }

                            u16RetVal = MHal_XC_CFD_SetMainCtrl_MMinput(&stMMParam, &stFormatInfo);
                            u8CallCFDinControl = 1;
                            Color_Format_Driver();
                            KApi_XC_MLoad_Fire(E_CLIENT_MAIN_HDR, TRUE);
                        }
                        else
                        {
                                //printk("Same parameters!!\n");
                        }



                    }
#endif
                }
#if 1 // patch
                else
                {
                    MHal_XC_CFD_DepositeMMParam(&stFormatInfo, stCfdInit[0].u8Win);
                }
#endif
            }
            else
            {
                if ((IS_OPEN_HDR(stCfdInit[0].u8Win)) || (IS_HLG_HDR(stCfdInit[0].u8Win)))
                {
                    // patch here, needs to be removed after CFD has fixed
                    _ISMMDolby = FALSE;
                    // reset HDR type and Input source Type When leave HDR10 MM.
                    _enHDRType = E_KDRV_XC_HDR_NONE;
                    _enInputSourceType = E_KDRV_XC_INPUT_SOURCE_NONE;
                }
            }

        }
#if 1  // patch
        else
        {
            ST_KDRV_XC_SHARE_MEMORY_FORMAT_Info stFormatInfo;
            memset(&stFormatInfo, 0, sizeof(ST_KDRV_XC_SHARE_MEMORY_FORMAT_Info));
            MHal_XC_CFD_DepositeMMParam(&stFormatInfo, stCfdInit[0].u8Win);
        }
#endif
        // main window process END


        // sub window process START
        if (MHal_XC_CFD_GetInitParam(&stCfdInit[1]) == E_CFD_MC_ERR_NOERR)
        {
            // Currently, there is no sub window Dolby Vision,

            if (CFD_IS_MM(stCfdInit[1].u8InputSource) || CFD_IS_DTV(stCfdInit[1].u8InputSource))
            {
                ST_KDRV_XC_SHARE_MEMORY_FORMAT_Info stFormatInfo;
                if (_CFD_GetShareMemData(_pu8ShareMemBaseAddr, &stFormatInfo, stCfdInit[1].u8Win,E_KDRV_XC_CFD_HDR_TYPE_OPEN) == TRUE)
                {
                    // patch here, needs to be removed after CFD has fixed
                    if ((IS_OPEN_HDR(stCfdInit[1].u8Win)) || (IS_HLG_HDR(stCfdInit[1].u8Win)))
                    {
                        _ISMMDolby = FALSE;
                        // turn on settings
                        MHal_XC_SetInputSourceType(E_KDRV_XC_INPUT_SOURCE_OTT);
                        MHal_XC_SetHDRType(E_KDRV_XC_HDR_OPEN);  // marked on purpose, because now it is controlled by CFD driver.
                    }
                    MHal_XC_CFD_DepositeMMParam(&stFormatInfo, stCfdInit[1].u8Win);
                }
            }
        }
        // sub window process END
    }
    else
    {
        _u32NoSignalCount++;
        if (_u32NoSignalCount > XC_NO_INPUT_SIGNAL_TIMEOUT)
        {
            _u32NoSignalCount = XC_NO_INPUT_SIGNAL_TIMEOUT;
            // patch here, needs to be removed after CFD has fixed
            _ISMMDolby = FALSE;
        }
    }

    // BIT(6) == descramble done interrupt happened
    if (u16IRQValue & BIT(6)) // this is for HDMI Dolby HDR only.
    {
        ST_KDRV_XC_CFD_INIT stCfdInit;
        stCfdInit.u8Win = 0; // retrieve Main window data
        if (MHal_XC_CFD_GetInitParam(&stCfdInit) == E_CFD_MC_ERR_NOERR)
        {
            if (CFD_IS_HDMI(stCfdInit.u8InputSource))
            {
                E_CFD_MC_FORMAT enHDMIDataFormat = MHal_XC_HDMI_Color_Data_Format(_stCfdHdmi[0].u8PixelFormat);
                if (((enHDMIDataFormat == E_CFD_MC_FORMAT_RGB) || (enHDMIDataFormat == E_CFD_MC_FORMAT_YUV422)) && _stCfdHdmi[0].bIsFullRange)
                {
                    MS_U16 u16MetadataLength = 0;
                    XC_KDBG("XC hdmi dolby\n");

                    // turn on settings
                    MHal_XC_SetInputSourceType(E_KDRV_XC_INPUT_SOURCE_HDMI);

                    u16MetadataLength = 0;
                    static MS_U8 u8count =0;
                    if (IS_DOLBY_CRC_PASS && MHal_XC_VSIF_Dolby_Status())
                    {
#if Dolby_SDR_HDR_MUTE
                        if((_bTimingChanged == FALSE) && (_bPreHDR == FALSE))  //This is a patch to mute EOS testing item
                        {
                            if(u8count<=HDR_SWITCH_COUNTER)
                            {
                                Mapi_Cfd_GenerateBlackVideo(TRUE);
                                u8count++;
                            }
                            else
                            {
                                Mapi_Cfd_GenerateBlackVideo(FALSE);
                                u8count=0;
                                _bPreHDR = TRUE;
                            }
                        }
                        else if((_bTimingChanged == FALSE) && (_bPreHDR == TRUE))
                        {
                            Mapi_Cfd_GenerateBlackVideo(FALSE);
                            u8count=0;
                        }
#endif
                        // patch here, needs to be removed after CFD has fixed
                        _ISHDMIDolby = TRUE;
                        _u8CRCErrCnt = 0;
                        if (MHal_XC_SupportDolbyHDR() == TRUE)
                        {
                            if (IS_DOLBY_HDR(stCfdInit.u8Win))
                            {
                                MHal_XC_SetDMPQBypass(DISABLE);
                            }
                            else
                            {
                                // for UI Dolby OFF but actually a HDMI Dolby Content
                                // we should still turn on DMA MIU request
                                // and also bypass DM PQ to get the correct result.
                                MHal_XC_SetHDR_DMARequestOFF(DISABLE, TRUE);
                                MHal_XC_SetDMPQBypass(ENABLE);
                            }

                            MHal_XC_SetHDRType(E_KDRV_XC_HDR_DOLBY);
                            if (MHal_XC_GetHdmiMetadata(_pu8DolbyMetadata, &u16MetadataLength) == TRUE)
                            {
                                // set metadata into color-format hw
                                MHal_XC_SetDolbyMetaData(_pu8DolbyMetadata, u16MetadataLength);
                            }
                            Color_Format_Driver_Dolby_Set(E_CFIO_MODE_HDR1);
                            _bTimingChanged = FALSE;
                        }
                        else
                        {
                            MHal_XC_MuteHDR();
                            Color_Format_Driver_Dolby_Set(E_CFIO_MODE_SDR);
                            _bTimingChanged = FALSE;
                        }
                    }
                    else
                    {
#if Dolby_SDR_HDR_MUTE
                        if((_bTimingChanged == FALSE) && (_bPreHDR == TRUE)) //This is a patch to mute EOS testing item
                        {
                            if(u8count <= SDR_SWITCH_COUNTER)
                            {
                                Mapi_Cfd_GenerateBlackVideo(TRUE);
                                u8count++;
                            }
                            else
                            {
                                Mapi_Cfd_GenerateBlackVideo(FALSE);
                                u8count = 0;
                                _bPreHDR = FALSE;
                            }
                        }
#endif
                        MS_U8 u8MaxCRCErrCnt = 0;
                        _u8CRCErrCnt++;
                        u8MaxCRCErrCnt = (MHal_XC_R2BYTE(REG_SC_BK79_04_L) & 0x1F00) >> 8;
                        if (_u8CRCErrCnt > u8MaxCRCErrCnt)
                        {
                            _u8CRCErrCnt = u8MaxCRCErrCnt;
                            _enHDRType = E_KDRV_XC_HDR_NONE;
                            // patch here, needs to be removed after CFD has fixed
                            if(_ISHDMIDolby==TRUE)
                            {
                                KApi_XC_MLoad_WriteCmd(E_CLIENT_MAIN_HDR,REG_SC_BK79_04_L, 0, BIT(15));  //closed auto seamless to avoid green screen, mantis 1123499
                            }

                            _ISHDMIDolby = FALSE;
                            Color_Format_Driver_Dolby_Set(E_CFIO_MODE_SDR);
                        }
                    }
                    _bTimingChanged=FALSE;
                }
                else
                {
                    // patch here, needs to be removed after CFD has fixed
                    ST_KDRV_XC_CFD_HDMI stCfdHdmi;
                    memset(&stCfdHdmi, 0, sizeof(ST_KDRV_XC_CFD_HDMI));
                    stCfdHdmi.u8Win = stCfdInit.u8Win;    // Main
                    if (CFD_IS_HDMI(stCfdInit.u8InputSource) && (MHal_XC_CFD_GetHdmiParam(&stCfdHdmi) == E_CFD_MC_ERR_NOERR))
                    {
                        if ((stCfdHdmi.bHDRInfoFrameValid == TRUE) && (stCfdHdmi.u8EOTF == 2))
                        {
                            MHal_XC_UpdatePath(E_KDRV_XC_HDR_PATH_OPEN_HDMI);
                        }
                        else
                        {
                            MHal_XC_UpdatePath(E_KDRV_XC_HDR_PATH_NON_HDR_HDMI);
                            // reset HDR type and Input source Type When leave DB HDMI.
                            _enHDRType = E_KDRV_XC_HDR_NONE;
                        }
                    }
                    else
                    {
                        MHal_XC_UpdatePath(E_KDRV_XC_HDR_PATH_NON_HDR_HDMI);
                        // reset HDR type and Input source Type When leave DB HDMI.
                        _enHDRType = E_KDRV_XC_HDR_NONE;
                    }
                    _ISHDMIDolby = FALSE;
                    Color_Format_Driver_Dolby_Set(E_CFIO_MODE_SDR);
                }
            }
        }
        // clear ISR
        MHal_XC_W2BYTEMSK(REG_SC_CLEAR_VSYNC_IRQ, BIT(6), BIT(6));
    }

    if (!_ISMMDolby)
    {
        _MDrv_XC_DelDolbyHDRTimer();
    }

    if (_ISMMDolby || _ISHDMIDolby)
    {
        _bEnableHDR = TRUE;
    }
    else
    {
#ifdef      SUPPORT_TCH
        if(_ISTCH)
        {
            _bEnableHDR = FALSE;
        }
        else
        {
            _enHDRType = E_KDRV_XC_HDR_NONE;
            _bEnableHDR = FALSE;
        }
#else
        _enHDRType = E_KDRV_XC_HDR_NONE;
        _bEnableHDR = FALSE;
#endif
    }
#ifdef      SUPPORT_TCH
#if TCH_DEV
    if(_bTCHStart) //test every vsync
    {
        static MS_BOOL TCHFlag = TRUE;
        if(TCHFlag)
        {
        do
        {
            if (_pstTCHDRShareMem != NULL && _pstTCHDRShareMem->u16Version == 3)
            {
                if (!isTCHDRInfoEntryEmpty(_pstTCHDRShareMem))
                {
                    MS_U32 u32RPtr = getTCHDRMemRPtr(_pstTCHDRShareMem);
                    ST_KDRV_XC_TC_HDR_INFO_ENTRY stEntry;
                    MS_BOOL bRetVal = FALSE;

                    // Get current entry
                    bRetVal = getTCHDRInfoEntry(_pstTCHDRShareMem, u32RPtr, &stEntry);
                    if(u32logct == PRI_NUM)
                    {
                        XC_TCHDBG("Rptr: %d, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n", u32RPtr, stEntry.u32InputMDAddr, stEntry.u32HDRRegsetAddr, stEntry.u32HDRRegsetSize, stEntry.u32HDRLutAddr, stEntry.u32HDRLutSize);
                    }

                    if (bRetVal == TRUE)
                    {
                        // Get InputMD data
                        MS_U16 u16MDMiuSel = getTCHDRMDMiuSel(_pstTCHDRShareMem);
                        MS_U32 u32InputMDAddr = stEntry.u32InputMDAddr;
                        MS_U32 u32Offset = 0;
                        if (MHal_XC_GetMiuOffset(u16MDMiuSel, &u32Offset) != TRUE)
                        {
                            printk("GetMiuOffset u16MDMiuSel: %d fail.\n", u16MDMiuSel);
                            continue;
                        }
                        if (u32InputMDAddr != 0)
                        {
                            MS_U8 *pu8InputMDData = NULL;
                            ST_KDRV_XC_TC_HDR_INPUT_MD_FORMAT stFormatInfo;
                            if(u32logct == PRI_NUM)
                            {
                                printk("[%s][%d] IS_TC_HDR 0x%lx, 0x%lx\n" ,__FUNCTION__,__LINE__,u32InputMDAddr,_stShareMemInfo.phyBaseAddr);
                            }
                            pu8InputMDData = ((MS_U8 *)_pstTCHDRShareMem) + (u32InputMDAddr - _stShareMemInfo.phyBaseAddr);

                            if (pu8InputMDData == NULL)
                            {
                                printk("metadata mmap pa fail.\n");
                                continue;
                            }

                            bRetVal = getTCHDRInputMDFormatInfo(pu8InputMDData, &stFormatInfo);

                            if (bRetVal == TRUE)
                            {
                                MS_U32 u32RegsetAddr = 0;
                                MS_U16 u16LutMiuSel = 0;
                                KHAL_DS_STORED_ADDR_INFO stDSStoredAddr;
                                MS_U32 u32LutAddr = 0;
                                MS_U32 u32LutSize = 0;
                                MS_U8 *pu8LutAddr = NULL;

                                if ((stFormatInfo.u32T35Addr == 0) && ((stFormatInfo.u32CRAddr == 0)))
                                {
                                    printk("%d: addr fail.\n", __LINE__);
                                    continue;
                                }
                                if(u32logct == PRI_NUM)
                                {
                                    XC_TCHDBG("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", stFormatInfo.u8CurrentIndex, stFormatInfo.u8T35MiuNo, stFormatInfo.u8T35Enable,
                                            stFormatInfo.u32T35Length, stFormatInfo.u32T35Addr, stFormatInfo.u8CRMiuNo, stFormatInfo.u8CREnable,
                                            stFormatInfo.u32CRLength, stFormatInfo.u32CRAddr);
                                }
                                // get stored regset addr
                                u32RegsetAddr = stEntry.u32HDRRegsetAddr;
                                if (u32RegsetAddr == 0)
                                {
                                    printk("%d: u32HDRRegsetAddr fail.\n", __LINE__);
                                    continue;
                                }

                                // set stored regset addr into DS
                                TCH_Prepare_DS(0);
                                memset(&stDSStoredAddr, 0, sizeof(KHAL_DS_STORED_ADDR_INFO));
                                stDSStoredAddr.stIPMAddr.phyDSAddr = (MS_PHY)(u32RegsetAddr + u32Offset);
                                stDSStoredAddr.stIPMAddr.u32DSSize = TC_HDR_OUTPUT_REG_SIZE;
                                stDSStoredAddr.stIPMAddr.virtDSAddr = (MS_VIRT)(_pu8RegSetAddr + (u32RegsetAddr - (_stShareMemInfo.phyBaseAddr + HDR_MEM_COMMON_ENTRY_SIZE + HDR_TOTAL_INPUT_MD_SIZE)));
                                if(u32logct == PRI_NUM)
                                {
                                    XC_TCHDBG("[%s][%d] IS_TC_HDR 0x%x 0x%x 0x%x   0x%x 0x%x 0x%x\n",__FUNCTION__,__LINE__, stDSStoredAddr.stIPMAddr.phyDSAddr, _pu8RegSetAddr,u32RegsetAddr,
                                        _stShareMemInfo.phyBaseAddr,HDR_MEM_COMMON_ENTRY_SIZE,HDR_TOTAL_INPUT_MD_SIZE);
                                }
                                KHal_SC_Set_DS_StoredAddr(&stDSStoredAddr);

                                if ((stFormatInfo.u8T35Enable) && (stFormatInfo.u32T35Length == 0))
                                {
                                    printk("%d: u32T35Length fail.\n", __LINE__);
                                    continue;
                                }
                                else
                                {
                                    if (MHal_XC_GetMiuOffset(stFormatInfo.u8T35MiuNo, &u32Offset) != TRUE)
                                    {
                                        printk("%d: MHal_XC_GetMiuOffset fail.\n", __LINE__);
                                        continue;
                                    }
                                }

                                if ((stFormatInfo.u8CREnable) && (stFormatInfo.u32CRLength == 0))
                                {
                                    printk("%d: u32CRLength fail.\n", __LINE__);
                                    continue;
                                }
                                else
                                {
                                    if (MHal_XC_GetMiuOffset(stFormatInfo.u8CRMiuNo, &u32Offset) != TRUE)
                                    {
                                        printk("%d: MHal_XC_GetMiuOffset fail.\n", __LINE__);
                                        continue;
                                    }
                                }

                                // turn on settings
                                MHal_XC_SetInputSourceType(E_KDRV_XC_INPUT_SOURCE_OTT);
                                MHal_XC_SetHDRType(E_KDRV_XC_HDR_TECHNICOLOR);

                                // update metadata
                                if ((stFormatInfo.u8T35Enable) && (stFormatInfo.u32T35Addr != 0))
                                {
                                    MS_U8 *pu8Metadata_T35 = NULL;
                                    pu8Metadata_T35 = _pu8InputMDAddr + (stFormatInfo.u32T35Addr - _phyMDAddr);
                                    MS_U8 *pu8Metadata_CR = NULL;
                                    if(stFormatInfo.u8CREnable)
                                    {
                                        pu8Metadata_CR = _pu8InputMDAddr + (stFormatInfo.u32CRAddr - _phyMDAddr);
                                    }
                                    if(u32logct == PRI_NUM)
                                    {
                                        XC_TCHDBG("[%s][%d] IS_TC_HDR 0x%x 0x%x 0x%x 0x%x\n",__FUNCTION__,__LINE__,pu8Metadata_T35,_pu8InputMDAddr,
                                            stFormatInfo.u32T35Addr,_phyMDAddr);

                                        XC_TCHDBG("MetaData: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
                                            *pu8Metadata_T35, *(pu8Metadata_T35+1), *(pu8Metadata_T35+2), *(pu8Metadata_T35+3),
                                            *(pu8Metadata_T35+4), *(pu8Metadata_T35+5), *(pu8Metadata_T35+6), *(pu8Metadata_T35+7));
                                    }
                                    //[Todo] Send Metadata to CFD
                                    //XC_TCHDBG("%d %d 0x%x 0x%x\n",stFormatInfo.u8T35Enable,stFormatInfo.u8CREnable,stFormatInfo.u32T35Addr,stFormatInfo.u32CRAddr);

                                    sl_hdr1_metadata sttchmetadata;
                                    memset(&sttchmetadata, 0, sizeof(sl_hdr1_metadata));
                                    SetTCHParameter(stFormatInfo.u8T35Enable,pu8Metadata_T35,stFormatInfo.u8CREnable,pu8Metadata_CR, &sttchmetadata);
                                    Mdrv_TCH_handler(&sttchmetadata);
                                }
                                else
                                {
                                    printk("stFormatInfo.u8T35Enable not enable!!\n");
                                }
                                stEntry.u32HDRRegsetSize = TCH_GetCmdCnt();
                            }
                        }

                        stEntry.u32CFDReady = TRUE;
                        setTCHDRInfoEntry(_pstTCHDRShareMem, u32RPtr, &stEntry);
                        incTCHDRMemRPtr(_pstTCHDRShareMem);
                    }
                }
            }
        }
        while(0);
        //TCHFlag = FALSE;
        }
    }
    else
    {
        MHal_XC_W2BYTEMSK(REG_SC_YAHAN_BK71_01_L, 0x0000, 0x0001);
    }
    if(1)//u32logct == PRI_NUM) //disable log
    {
        u32logct = 0;
    }
#endif
#endif
    mutex_unlock(&_cfd_mutex);

}

MS_BOOL MDrv_XC_SetHDRType(EN_KDRV_XC_HDR_TYPE enHDRType)
{
    return MHal_XC_SetHDRType(enHDRType);
}

MS_BOOL MDrv_XC_GetHDRType(EN_KDRV_XC_HDR_TYPE *penHDRType)
{
    mutex_lock(&_cfd_mutex);
    *penHDRType = _enHDRType;
    mutex_unlock(&_cfd_mutex);

    return TRUE;
}

MS_BOOL MDrv_XC_SetInputSourceType(EN_KDRV_XC_INPUT_SOURCE_TYPE enInputSourceType)
{
    return MHal_XC_SetInputSourceType(enInputSourceType);
}

MS_BOOL MDrv_XC_GetInputSourceType(EN_KDRV_XC_INPUT_SOURCE_TYPE *penInputSourceType)
{
    *penInputSourceType = _enInputSourceType;

    return TRUE;
}

MS_BOOL MDrv_XC_SetOpenMetadataInfo(ST_KDRV_XC_OPEN_METADATA_INFO* pstMetadataInfo)
{
    //set metadata into color-format hw
    return TRUE;
}

MS_BOOL MDrv_XC_Set3DLutInfo(ST_KDRV_XC_3DLUT_INFO *pst3DLutInfo)
{

    //update STR 3D LUT
    if(st3DLutInfoForSTR.u32Size!=pst3DLutInfo->u32Size)
    {
        //if size of table is differnt, then allocate new space for 3D LUT
        if(st3DLutInfoForSTR.pu8Data)
        {
            kfree(st3DLutInfoForSTR.pu8Data);
        }
        st3DLutInfoForSTR.pu8Data = kmalloc(pst3DLutInfo->u32Size, GFP_KERNEL);

    }
    st3DLutInfoForSTR.u32Size = pst3DLutInfo->u32Size;
    memcpy(((MS_U8 *)st3DLutInfoForSTR.pu8Data), pst3DLutInfo->pu8Data, st3DLutInfoForSTR.u32Size);

    return MHal_XC_Set3DLutInfo(pst3DLutInfo->pu8Data, pst3DLutInfo->u32Size);
}

MS_BOOL MDrv_XC_GetShareMemInfo(ST_KDRV_XC_SHARE_MEMORY_INFO *pstShmemInfo)
{
    memcpy(pstShmemInfo,&_stShareMemInfo,sizeof(_stShareMemInfo));
    return TRUE;
}

MS_BOOL MDrv_XC_SetShareMemInfo(ST_KDRV_XC_SHARE_MEMORY_INFO *pstShmemInfo)
{
    MS_U32 u32Offset = 0;

    if (pstShmemInfo->u32Size < XC_SHARE_MEMORY_SIZE || pstShmemInfo->phyBaseAddr == 0)
    {
        printk("Invalid size|addr.\n");
        return FALSE;
    }

    if (MHal_XC_GetMiuOffset(pstShmemInfo->u32MiuNo, &u32Offset) != TRUE)
    {
        printk("Invalid miuno.\n");
        return FALSE;
    }

    _stShareMemInfo = *pstShmemInfo;
    if (!_MDrv_XC_AddrNormalization(&(_stShareMemInfo.phyBaseAddr)))
    {
        printk("Invalid phyBaseAddr.\n");
        return FALSE;
    }

    XC_KDBG("addr: 0x%08x, miuno: %d, u32Size: %d.\n", _stShareMemInfo.phyBaseAddr, _stShareMemInfo.u32MiuNo, _stShareMemInfo.u32Size);

    if (pfn_valid(__phys_to_pfn(_stShareMemInfo.phyBaseAddr + u32Offset)))
    {
        _pu8ShareMemBaseAddr = __va(_stShareMemInfo.phyBaseAddr + u32Offset);
#if DOLBY_OTT_CACHED_BUFFER
        _pu8ShareMemBaseAddr_Cached= _pu8ShareMemBaseAddr;
#endif
        _bIOUnmapShareMem = FALSE;
    }
    else
    {
#if DOLBY_OTT_CACHED_BUFFER
        _pu8ShareMemBaseAddr_Cached= (MS_U8 __iomem *)ioremap_cached(_stShareMemInfo.phyBaseAddr + u32Offset, _stShareMemInfo.u32Size); //leo add
#endif
        _pu8ShareMemBaseAddr = (MS_U8 __iomem *)ioremap(_stShareMemInfo.phyBaseAddr + u32Offset, _stShareMemInfo.u32Size);
        _bIOUnmapShareMem = TRUE;
    }

    if (_pu8ShareMemBaseAddr == NULL)
    {
        printk("share memory mmap pa fail.\n");
        return FALSE;
    }
    else
    {
        memset(_pu8ShareMemBaseAddr, 0, XC_SHARE_MEMORY_SIZE);
        // share memory version 0xFF is invalid.
        ((ST_KDRV_XC_SHARE_MEMORY_FORMAT_Info *)_pu8ShareMemBaseAddr)->u8Version = 0xFF;
    }

#if 0 //for debug
    printk("\nShare memory data:\n");
    int i = 0;
    for (i = 0; i < XC_SHARE_MEMORY_SIZE; i++)
    {
        printk("data[%2d]: 0x%02x\n", i, *(_pu8ShareMemBaseAddr + i));
    }
#endif

    return TRUE;
}

MS_BOOL MDrv_XC_EnableHDR(MS_BOOL bEnableHDR)
{
    return MHal_XC_EnableHDR(bEnableHDR);
}

MS_BOOL MDrv_XC_SetHDRWindow(ST_KDRV_XC_WINDOW_INFO *pstWindowInfo)
{
    return MHal_XC_SetHDRWindow(pstWindowInfo->u16Width, pstWindowInfo->u16Height);
}

MS_BOOL MDrv_XC_ConfigAutoDownload(ST_KDRV_XC_AUTODOWNLOAD_CONFIG_INFO *pstConfigInfo)
{
    if (!_MDrv_XC_AddrNormalization(&(pstConfigInfo->phyBaseAddr)))
    {
        printk("Invalid phyBaseAddr.\n");
        return FALSE;
    }
    return MHal_XC_ConfigAutoDownload(pstConfigInfo->enClient, pstConfigInfo->bEnable,
                                      pstConfigInfo->enMode, pstConfigInfo->phyBaseAddr, pstConfigInfo->u32Size, pstConfigInfo->u32MiuNo);
}

MS_BOOL MDrv_XC_WriteAutoDownload(ST_KDRV_XC_AUTODOWNLOAD_DATA_INFO *pstDataInfo)
{
    return MHal_XC_WriteAutoDownload(pstDataInfo->enClient, pstDataInfo->pu8Data, pstDataInfo->u32Size, pstDataInfo->pParam);
}

MS_BOOL MDrv_XC_FireAutoDownload(EN_KDRV_XC_AUTODOWNLOAD_CLIENT enClient)
{
    return MHal_XC_FireAutoDownload(enClient);
}

MS_BOOL KDrv_XC_GetAutoDownloadCaps(ST_KDRV_XC_AUTODOWNLOAD_CLIENT_SUPPORTED_CAPS *pstAutoDownlaodCaps)
{
    return MHal_XC_GetAutoDownloadCaps(pstAutoDownlaodCaps->enClient, &pstAutoDownlaodCaps->bSupported);
}

MS_BOOL MDrv_XC_SetColorFormat(EN_KDRV_XC_HDR_COLOR_FORMAT enColorFormat)
{
    _enColorFormat = enColorFormat;

    if (IS_HDMI_OPEN)
    {
        switch (enColorFormat)
        {
            case E_KDRV_XC_HDR_COLOR_YUV422:
            {
                MHal_XC_SetColorFormat(TRUE);
                break;
            }
            case E_KDRV_XC_HDR_COLOR_YUV420:
            case E_KDRV_XC_HDR_COLOR_YUV444:
            case E_KDRV_XC_HDR_COLOR_RGB:
            {
                MHal_XC_SetColorFormat(FALSE);
                break;
            }
            default:
            {
                return FALSE;
            }
        }
    }

    return TRUE;
}

MS_BOOL MDrv_XC_CFDControl(ST_KDRV_XC_CFD_CONTROL_INFO *pstKdrvCFDCtrlInfo)
{
    return MHal_XC_CFDControl(pstKdrvCFDCtrlInfo);
}

#ifdef      SUPPORT_TCH
#define DBGDSINFO(x) //x
#endif
MS_BOOL MDrv_XC_SetDSHDRInfo(ST_KDRV_XC_DS_HDRInfo *pstDSHDRInfo)
{
    static MS_U32 err_cnt=0;
    ST_KDRV_XC_HDR_INFO_ENTRY stEntry;
    if (_bEnableHDR)
    {
        if (pstDSHDRInfo->bDMEnable)
        {
            MS_BOOL bCFDReady = FALSE;
            MS_U8 u8DSIdx = 0;
            MS_PHY phyLutAddr = 0;
            MS_PHY phyRegsetAddr = 0;
            MS_U32 u32RegsetSize = 0;
            MS_U32 u32LutSize = 0;
            MS_U32 u32RPtr = 0;
            MS_U32 u32WPtr = 0;
            MS_U32 u32LutMiuSel = 0;
            MS_PHY phyFireAdlAddr  = 0;
            K_XC_DS_CMDCNT stXCDSCmdCnt;
            MS_U32 u32Depth = 0;
            MS_U8 *pu8LutAddr = NULL;
            MS_U8 *pu8RegsetAddr = NULL;
            //MS_U32 u32MDMiuSel = 0; //u32MDMiuSel no use

            if (_pstDolbyHDRShareMem == NULL)
            {
                printk("_pstDolbyHDRShareMem is NULL.\n");
                return FALSE;
            }

            // dolby case
            bCFDReady = isCFDDone(_pstDolbyHDRShareMem, pstDSHDRInfo->u8CurrentIndex);
            while((_pstDolbyHDRShareMem != NULL) && !bCFDReady)
            {
                //waiting 1ms
                printk("MDrv_XC_SetDSHDRInfo waiting index(%d) done.....u32WPtr: %d, u32RPtr: %d\n", pstDSHDRInfo->u8CurrentIndex, _pstDolbyHDRShareMem->u32WdPtr, _pstDolbyHDRShareMem->u32RdPtr);
                u8tmpCurrentIndex = pstDSHDRInfo->u8CurrentIndex;
                msleep(1);
                bCFDReady = isCFDDone(_pstDolbyHDRShareMem, pstDSHDRInfo->u8CurrentIndex);
                if(err_cnt >= 200)
                {
                    printk("TIMEOUT, ASSERT!!!!\n");
                    err_cnt = 0;
                    return FALSE;
                }
                err_cnt++;
            }
            u8tmpCurrentIndex = 0;
            bJump=FALSE;
            spin_lock_irq(&_sethdr_info_lock);
            u8DSIdx = 0;
            KHal_XC_GetDynamicScalingCurrentIndex(E_KDRV_XC_SC0, E_KDRV_MAIN_WINDOW, &u8DSIdx);
            getDolbyHDRInfoEntry(_pstDolbyHDRShareMem, pstDSHDRInfo->u8CurrentIndex, &stEntry);

            phyLutAddr = pstDSHDRInfo->phyLutAddr;
            phyRegsetAddr = pstDSHDRInfo->phyRegAddr;
            u32RegsetSize = 0;
            u32LutSize = 0;
            u32LutSize = stEntry.u32HDRLutSize;
            u32RegsetSize = stEntry.u32HDRRegsetSize;

            u32RPtr = getDolbyHDRMemRPtr(_pstDolbyHDRShareMem);
            u32WPtr = getDolbyHDRMemWPtr(_pstDolbyHDRShareMem);

            u32LutMiuSel = getDolbyHDRLutMiuSel(_pstDolbyHDRShareMem);
            XC_KDBG("%s: u32WPtr: %d, u32RPtr: %d, u8CurrentIndex: %d, u8DSIdx: %d, phyRegsetAddr: 0x%x, u32RegsetSize: %d, phyLutAddr: 0x%x, u32LutSize: %d, bCompEnable: %d.\n",
                    __FUNCTION__ , u32WPtr, u32RPtr, pstDSHDRInfo->u8CurrentIndex, u8DSIdx, phyRegsetAddr, u32RegsetSize, phyLutAddr, u32LutSize, pstDSHDRInfo->bCompEnable);

            // write stored adl data
            phyFireAdlAddr  = 0;
            u32Depth = 0;
#ifdef XC_REPLACE_MEMCPY_BY_BDMA
            pu8LutAddr = phyLutAddr;
#else
            pu8LutAddr = (MS_U8 *)(_pu8LutsAddr + (phyLutAddr - (_stShareMemInfo.phyBaseAddr + HDR_MEM_COMMON_ENTRY_SIZE + HDR_TOTAL_INPUT_MD_SIZE + HDR_TOTAL_REGSET_SIZE)));
#endif
            MHal_XC_WriteStoredHDRAutoDownload(pu8LutAddr, u32LutSize, u8DSIdx, &phyFireAdlAddr, &u32Depth);

            XC_KDBG("%s: 0x%x, %d,\n", __FUNCTION__, phyFireAdlAddr, u32Depth);
            // write fire adl cmd into ds memory

            memset(&stXCDSCmdCnt, 0, sizeof(K_XC_DS_CMDCNT));

            //set phyaddr
            KHal_SC_WriteSWDSCommand(E_KDRV_XC_SC0, E_KDRV_MAIN_WINDOW, E_DS_CLIENT_HDR, REG_SC_BK67_29_L, (phyFireAdlAddr / BYTE_PER_WORD) & 0x0000FFFF, K_DS_IP, K_DS_XC, &stXCDSCmdCnt, u8DSIdx);
            KHal_SC_WriteSWDSCommand(E_KDRV_XC_SC0, E_KDRV_MAIN_WINDOW, E_DS_CLIENT_HDR, REG_SC_BK67_2A_L, (phyFireAdlAddr / BYTE_PER_WORD) >> 16 & 0x0000FFFF, K_DS_IP, K_DS_XC, &stXCDSCmdCnt, u8DSIdx);
            // set depth
            KHal_SC_WriteSWDSCommand(E_KDRV_XC_SC0, E_KDRV_MAIN_WINDOW, E_DS_CLIENT_HDR, REG_SC_BK67_2B_L, u32Depth, K_DS_IP, K_DS_XC, &stXCDSCmdCnt, u8DSIdx);
            KHal_SC_WriteSWDSCommand(E_KDRV_XC_SC0, E_KDRV_MAIN_WINDOW, E_DS_CLIENT_HDR, REG_SC_BK67_2C_L, u32Depth, K_DS_IP, K_DS_XC, &stXCDSCmdCnt, u8DSIdx);
            // enable auto download
            KHal_SC_WriteSWDSCommand(E_KDRV_XC_SC0, E_KDRV_MAIN_WINDOW, E_DS_CLIENT_HDR, REG_SC_BK67_28_L, 0x1, K_DS_IP, K_DS_XC, &stXCDSCmdCnt, u8DSIdx);

            // write stored ds cmd into ds memory
            /*
            MS_U32 u32Offset = 0;
            if (MHal_XC_GetMiuOffset(_stShareMemInfo.u32MiuNo, &u32Offset) != TRUE)
            {
                printk("Invalid miuno.\n");
                return FALSE;
            }
            */
#ifdef XC_REPLACE_MEMCPY_BY_BDMA
            pu8RegsetAddr = phyRegsetAddr;
#else
            pu8RegsetAddr = _pu8RegSetAddr + (phyRegsetAddr - (_stShareMemInfo.phyBaseAddr + HDR_MEM_COMMON_ENTRY_SIZE + HDR_TOTAL_INPUT_MD_SIZE));
#endif
            KHal_XC_WriteStoredSWDSCommand(E_KDRV_XC_SC0, E_KDRV_MAIN_WINDOW, E_DS_CLIENT_HDR, pu8RegsetAddr, u32RegsetSize, K_DS_IP, K_DS_XC, &stXCDSCmdCnt, u8DSIdx);
            KHal_SC_Add_NullCommand(E_KDRV_XC_SC0, E_KDRV_MAIN_WINDOW, E_DS_CLIENT_HDR, K_DS_IP, &stXCDSCmdCnt, u8DSIdx);
            spin_unlock_irq(&_sethdr_info_lock);
        }


        if (pstDSHDRInfo->bCompEnable)
        {
            // enable enhanced layer
            MHal_XC_EnableEL(TRUE);
        }
        else
        {
            // disable enhanced layer
            MHal_XC_EnableEL(FALSE);
        }
    }
#ifdef      SUPPORT_TCH
    else if (_bTCHOneTimeSet || _bTCHStart)
    {
#if TCH_DEV
        if(_bTCHOneTimeSet)
        {
//SN
//#define TEMP_SHM_ADDR 0x1fc81040
//#define TEMP_MD_ADDR  (TEMP_SHM_ADDR+0x4A0)
//#define TEMP_T35_ADDR  0x13800000
//#define TEMP_REG_ADDR  TEMP_SHM_ADDR //(TEMP_SHM_ADDR+0xAA0)

            MS_U64 u32RegsetAddr = 0;
            KHAL_DS_STORED_ADDR_INFO stDSStoredAddr;
            MS_U32 u32Offset0 = 0;

            if (MHal_XC_GetMiuOffset(_stShareMemInfo.u32MiuNo, &u32Offset0) != TRUE)
            {
                printk("Invalid miuno.\n");
                return FALSE;
            }

            // get stored regset addr
            u32RegsetAddr = _stShareMemInfo.phyBaseAddr + 0x40;
            u32RegsetAddr += u32Offset0;
            memset(&stDSStoredAddr, 0, sizeof(KHAL_DS_STORED_ADDR_INFO));
            stDSStoredAddr.stIPMAddr.phyDSAddr = (MS_PHY)(u32RegsetAddr);
            stDSStoredAddr.stIPMAddr.u32DSSize = HDR_OUTPUT_REG_SIZE;
            stDSStoredAddr.stIPMAddr.virtDSAddr = 0;
            KHal_SC_Set_DS_StoredAddr(&stDSStoredAddr);
            TCH_Prepare_DS(0);
            Mdrv_TCH_handler(NULL);

            MS_BOOL bCFDReady = FALSE;
            MS_U8 u8DSIdx = 0;
            MS_U32 u32RegsetSize = 0;
            K_XC_DS_CMDCNT stXCDSCmdCnt;
            MS_U8 *pu8RegsetAddr = NULL;
            //MS_U32 u32MDMiuSel = 0; //u32MDMiuSel no use

            u8DSIdx = 0;
            KHal_XC_GetDynamicScalingCurrentIndex(E_KDRV_XC_SC0, E_KDRV_MAIN_WINDOW, &u8DSIdx);
            //getDolbyHDRInfoEntry(_pstDolbyHDRShareMem, pstDSHDRInfo->u8CurrentIndex, &stEntry);
            printk("u8DSIdx %d\n", u8DSIdx);
            u32RegsetSize = TCH_GetCmdCnt();
            printk("%s: u8CurrentIndex: %d, u8DSIdx: %d, u32RegsetSize: %d\n",
                    __FUNCTION__ , pstDSHDRInfo->u8CurrentIndex, u8DSIdx, u32RegsetSize);

            // write fire adl cmd into ds memory
            memset(&stXCDSCmdCnt, 0, sizeof(K_XC_DS_CMDCNT));
            pu8RegsetAddr = ((MS_U8 *)_pu8ShareMemBaseAddr) + 0x40;
            KHal_XC_WriteStoredSWDSCommand(E_KDRV_XC_SC0, E_KDRV_MAIN_WINDOW, E_DS_CLIENT_HDR, pu8RegsetAddr, u32RegsetSize, K_DS_IP, K_DS_XC, &stXCDSCmdCnt, u8DSIdx);

            KHal_SC_Add_NullCommand(E_KDRV_XC_SC0, E_KDRV_MAIN_WINDOW, E_DS_CLIENT_HDR, K_DS_IP, &stXCDSCmdCnt, u8DSIdx);

        }
        else
        {
            if (pstDSHDRInfo->u32RegSize)
            //if (pstDSHDRInfo->bDMEnable)
            {
                MS_BOOL bCFDReady = FALSE;
                MS_U8 u8DSIdx = 0;
                MS_PHY phyRegsetAddr = 0;
                MS_U32 u32RegsetSize = 0;
                MS_U32 u32RPtr = 0;
                MS_U32 u32WPtr = 0;
                MS_PHY phyFireAdlAddr  = 0;
                K_XC_DS_CMDCNT stXCDSCmdCnt;
                MS_U32 u32Depth = 0;
                MS_U8 *pu8RegsetAddr = NULL;
                //MS_U32 u32MDMiuSel = 0; //u32MDMiuSel no use

                if (_pstTCHDRShareMem == NULL)
                {
                    printk("_pstDolbyHDRShareMem is NULL.\n");
                    return FALSE;
                }

                // dolby case
                bCFDReady = isCFDTCHDone(_pstTCHDRShareMem, pstDSHDRInfo->u8CurrentIndex);
                while((_pstTCHDRShareMem != NULL) && !bCFDReady)
                {
                    //waiting 1ms
                    printk("MDrv_XC_SetDSHDRInfo waiting index(%d) done.....u32WPtr: %d, u32RPtr: %d\n", pstDSHDRInfo->u8CurrentIndex, _pstTCHDRShareMem->u32WdPtr, _pstTCHDRShareMem->u32RdPtr);
                    msleep(1);
                    bCFDReady = isCFDTCHDone(_pstTCHDRShareMem, pstDSHDRInfo->u8CurrentIndex);
                    if(err_cnt >= 200)
                    {
                        printk("TIMEOUT, ASSERT!!!!\n");
                        err_cnt = 0;
                        return FALSE;
                    }
                    err_cnt++;
                }

                u8DSIdx = 0;
                KHal_XC_GetDynamicScalingCurrentIndex(E_KDRV_XC_SC0, E_KDRV_MAIN_WINDOW, &u8DSIdx);
                getTCHDRInfoEntry(_pstTCHDRShareMem, pstDSHDRInfo->u8CurrentIndex, &stEntry);

                phyRegsetAddr = pstDSHDRInfo->phyRegAddr;
                u32RegsetSize = stEntry.u32HDRRegsetSize;

                u32RPtr = getTCHDRMemRPtr(_pstTCHDRShareMem);
                u32WPtr = getTCHDRMemWPtr(_pstTCHDRShareMem);

                //u32LutMiuSel = getTCHDRLutMiuSel(_pstTCHDRShareMem);
                DBGDSINFO(XC_TCHDBG("%s: u32WPtr: %d, u32RPtr: %d, u8CurrentIndex: %d, u8DSIdx: %d, phyRegsetAddr: 0x%x, u32RegsetSize: %d\n",
                        __FUNCTION__ , u32WPtr, u32RPtr, pstDSHDRInfo->u8CurrentIndex, u8DSIdx, (int)phyRegsetAddr, u32RegsetSize));

                memset(&stXCDSCmdCnt, 0, sizeof(K_XC_DS_CMDCNT));

                // write stored ds cmd into ds memory
                pu8RegsetAddr = _pu8RegSetAddr + (phyRegsetAddr - (_stShareMemInfo.phyBaseAddr + HDR_MEM_COMMON_ENTRY_SIZE + HDR_TOTAL_INPUT_MD_SIZE));
                DBGDSINFO(XC_TCHDBG("%s: 0x%x 0x%x 0x%x 0x%x\n",
                        __FUNCTION__ , (int)pu8RegsetAddr, (int)_pu8RegSetAddr, (int)phyRegsetAddr, (int)_stShareMemInfo.phyBaseAddr));
                KHal_XC_WriteStoredSWDSCommand(E_KDRV_XC_SC0, E_KDRV_MAIN_WINDOW, E_DS_CLIENT_HDR, pu8RegsetAddr, u32RegsetSize, K_DS_IP, K_DS_XC, &stXCDSCmdCnt, u8DSIdx);
                KHal_SC_Add_NullCommand(E_KDRV_XC_SC0, E_KDRV_MAIN_WINDOW, E_DS_CLIENT_HDR, K_DS_IP, &stXCDSCmdCnt, u8DSIdx);
            }
            else
            {
                // no TCH base
                MDrv_XC_SetHDRType(E_KDRV_XC_HDR_NONE);
                //MHal_XC_UpdatePath(E_KDRV_XC_HDR_PATH_ORIGIN);
            }
        }
#endif
    }
#endif
    else
    {
        printk("Warning: HDR is off.\n");
    }

    return TRUE;
}

MS_BOOL MDrv_XC_Init(void)
{
    MS_U16 u16MetadataLength = XC_HDR_DOLBY_PACKET_LENGTH * XC_HDR_HW_SUPPORT_MAX_DOLBY_PACKET; // for now, dolby's packet size is 128 byte, but hw support two packet2
    if (_pu8DolbyMetadata == NULL)
    {
        _pu8DolbyMetadata = kmalloc(u16MetadataLength, GFP_KERNEL);
        if (_pu8DolbyMetadata == NULL)
        {
            printk("alloc dobly metadata memory fail.\n");
        }
    }

    spin_lock_init(&_spinlock_xc_dolby_hdr);

    MHal_XC_Init();
    MHal_XC_InitHDR();
    _MDrv_XC_InitDolbyHDRTimer();

    return TRUE;
}

MS_BOOL MDrv_XC_Exit(void)
{
    if (_pu8DolbyMetadata != NULL)
    {
        kfree(_pu8DolbyMetadata);
        _pu8DolbyMetadata = NULL;
    }

    if ((_pu8ShareMemBaseAddr != NULL) && (_bIOUnmapShareMem == TRUE))
    {
        iounmap(_pu8ShareMemBaseAddr);
#if DOLBY_OTT_CACHED_BUFFER
        iounmap(_pu8ShareMemBaseAddr_Cached);
#endif
        _pu8ShareMemBaseAddr = NULL;
        _pstDolbyHDRShareMem = NULL;
    }

    MHal_XC_ExitHDR();

    _MDrv_XC_DelDolbyHDRTimer();

    return TRUE;
}
#endif

void MDrv_XC_FilmDriverHWVer1(void)
{
    MS_BOOL bIsFilmXOO = 0;
    MS_U32 u32Film22AdaptiveCounter1Th = 0;
    static MS_U16 _u16RdBank_Prv=0;
    MS_U16 u16RdBank;
    MS_U32 u32CurCounter1=0, u32CurCounter3=0, u32FixThofC3=0;
    static MS_U32 _u32Counter3_2=0, _u32Counter3_3=0;
    static MS_U32 _u32Counter1_2=0, _u32Counter1_3=0, _u32Counter1_4=0;
    static MS_U32 _u32CounterA_2=0, _u32CounterA_3=0;
    static MS_U32 _u32CounterB_2=0, _u32CounterB_3=0;
    static MS_U8 _u8Film22LikeCounter, _u8Film22LikeCounter2;
    static MS_U8 _u8Film22LikeCounter_ForSlowMotionDisableMCDi;
    static MS_U8 _u8SceneChangeFrame;
    static MS_U8 _u8IsFilmXOOCounter=0;
    static MS_BOOL _bIs2ndFilmXOOCounter;
    MS_BOOL bCounter1Have2xDiff, bFilm, bFilmAny;
    MS_U32 u32CurCounterA, u32CurCounterB;
    static MS_U8 _u8SmallCounter3Cnt;
    static MS_U8 _u8Film22LikeCounterA, _u8Film22LikeCounterB;
    static MS_U8 _u8Film22LikeCounter3_Big;
    static MS_U8 _u8Film32LikeCounter3_Big;
    static MS_U8 _u8Film22LikeCounter1_Big;
    static MS_U8 _u8VerticalCnt, _u8Unmatch3Cnt;
    static MS_U8 _u8FilmsmallCounter1 = 0 ;
    MS_BOOL bIsHDSource;
    MS_U16 u16Unmatch_3, u16Unmatch_1;
    static MS_U8 _u8NoCounter3_Cnt, _u8BigCounter3_Cnt, _u8BigCounter1_Cnt, _u8BigCounter_Cnt3;
    static MS_U8 _u8BigCounter1_Cnt2, _u8SmallCounter1_Cnt, _u8BigCounter3_Cnt2;
    MS_BOOL bVerticalMovingU ;
    MS_BOOL bIs60Hz = ( MHal_XC_R2BYTE(REG_SC_BK0A_10_L) & 0x8000 ) != 0; // checkfilm32 enable for 60Hz source
    static MS_U8 _u8StillCounter3;
    MS_U32 u32Counter3;
    static MS_U8 _u8MovingCharacterCounter;
#ifdef PRJ_MCNR  // For A3, A5,A7P,T12
    MS_U32 u32Reg7C, u32Reg7D, u32Reg7E;
#endif
    MS_U16 u16InvStillFrameCnt;
    MS_U16 u16KuroPatchEn = 0; //for KURO patch

    //Check the IPM 3D input and OP 3D output or 2D to 3D enable.
    if((( (MS_U16)MHal_XC_R2BYTEMSK(REG_SC_BK12_30_L, 0x01) == 0x01 )
        && ( (MS_U16)MHal_XC_R2BYTEMSK(REG_SC_BK12_33_L, 0x80) == 0x80 ))
       ||((( (MS_U16)MHal_XC_R2BYTEMSK(REG_SC_BK62_06_L, 0x7FFF) != 0x0 )
           || ( (MS_U16)MHal_XC_R2BYTEMSK(REG_SC_BK62_0E_L, 0x7FFF) != 0x0 ))
          && ( (MS_U16)MHal_XC_R2BYTEMSK(REG_SC_BK62_08_L, 0x10) == 0x10 )
          && ( (MS_U16)MHal_XC_R2BYTEMSK(REG_SC_BK63_61_L, 0x01) == 0x01 )))
    {
        //For 3D mode or 2D to 3D mode need to close the film driver .
        MHal_XC_W2BYTEMSK(REG_SC_BK0A_10_L, 0, 0xC000);
        MHal_XC_W2BYTEMSK(REG_SC_BK0A_24_L, 0, 0x8000);
        return ;
    }

    u16RdBank = ( MHal_XC_R2BYTE(REG_SC_BK12_3A_L) & 0xF0 ) >> 4;
    bIsHDSource = ( MHal_XC_R2BYTEMSK(REG_SC_BK12_0F_L, 0xFFFF) > 0x700 );

    u32CurCounterA = 0  ; // MHal_XC_R2BYTE(REG_SC_BK0A_4A_L);
    u32CurCounterB = 0  ; // MHal_XC_R2BYTE(REG_SC_BK0A_4B_L);

    u32CurCounter1 = MHal_XC_R2BYTE(REG_SC_BK0A_0A_L);
    u32CurCounter3 = MHal_XC_R2BYTE(REG_SC_BK0A_0B_L);

    if(u16RdBank == _u16RdBank_Prv)  // Not change read bank
    {
        return ;
    }

    bFilmAny = ( (MS_U16)MHal_XC_R2BYTEMSK(REG_SC_BK0A_21_L, 0x0040) == 0x0040);
    bFilm = (( (MS_U16)MHal_XC_R2BYTEMSK(REG_SC_BK0A_21_L, 0x0008) == 0x0008) ||  // film22
             ( (MS_U16)MHal_XC_R2BYTEMSK(REG_SC_BK0A_21_L, 0x0010) == 0x0010) ||  // film32
             ( (MS_U16)MHal_XC_R2BYTEMSK(REG_SC_BK0A_21_L, 0x0040) == 0x0040) );   // film any

    // check XOO
    if( ( (!bFilm) && (u32CurCounter3 > 0x200) && (_u32Counter3_2< 0x80) && (_u32Counter3_3< 0x80) ) || //not film
        ( (!bFilm) && (u32CurCounter3 > 0x80) && (_u32Counter3_2< 0x20) && (_u32Counter3_3< 0x20) ) ) //not film
    {
        if ( _u8IsFilmXOOCounter > 0 )
        {
            _bIs2ndFilmXOOCounter = true;
        }

        _u8IsFilmXOOCounter = 10; // need to hit XOO on every 10 frames
    }
    else if( bFilm && (!bFilmAny) && (u32CurCounter3 > 0x200) && (u32CurCounter3 > (_u32Counter3_2*2)) && (u32CurCounter3 > (_u32Counter3_3*2)) ) //32 or 22
    {
        if( _u8IsFilmXOOCounter > 0 )
        {
            _bIs2ndFilmXOOCounter = true;
        }
        _u8IsFilmXOOCounter = 10; // need to hit XOO on every 10 frames
    }
    else if(  bFilm && (u32CurCounter3 > 0x80)  && (u32CurCounter3 > (_u32Counter3_2*2)) && (u32CurCounter3 > (_u32Counter3_3*2)) ) // film-any
    {
        _bIs2ndFilmXOOCounter = true;
        _u8IsFilmXOOCounter = 10; // need to hit XOO on every 10 frames
    }
    else if( _u8IsFilmXOOCounter != 0 )
    {
        _u8IsFilmXOOCounter --;
    }
    else
    {
        _bIs2ndFilmXOOCounter = false;
    }

    //detect scene change into film...
    if( (u32CurCounter1 > (_u32Counter1_4*4)) && (u32CurCounter1 > (_u32Counter1_2*4)) && (u32CurCounter1 > (_u32Counter1_3*4)) )
        _u8SceneChangeFrame = 60;
    else if( _u8SceneChangeFrame != 0 )
        _u8SceneChangeFrame --;

    if( (u32CurCounter3 > 0x80) && (_u32Counter3_2 < 0x10) && (_u32Counter3_3 > 0x80) )
        _u8Film22LikeCounter += 2;
    else if( (u32CurCounter3 < 0x10) && (_u32Counter3_2 > 0x80) && (_u32Counter3_3 < 0x10) )
        _u8Film22LikeCounter += 2;
    else if( (u32CurCounter1 > (_u32Counter1_2 + 0x200)) && (_u32Counter1_3 > (_u32Counter1_2 + 0x200)) )
        _u8Film22LikeCounter += 1;
    else if( (_u32Counter1_2 > (u32CurCounter1 + 0x200)) && (_u32Counter1_2 > (_u32Counter1_3 + 0x200)) )
        _u8Film22LikeCounter += 1;
    else if ( _u8Film22LikeCounter > 0 )
        _u8Film22LikeCounter --;

    if( u32CurCounter3 < 0x80 ) // for slow motion film22-like
    {
        if( (u32CurCounter1 > (_u32Counter1_2*2)) && (_u32Counter1_3 > (_u32Counter1_2*2)) )
            _u8Film22LikeCounter_ForSlowMotionDisableMCDi += 1;
        else if ( (_u32Counter1_2 > (u32CurCounter1*2)) && (_u32Counter1_2 > (_u32Counter1_3*2)) )
            _u8Film22LikeCounter_ForSlowMotionDisableMCDi += 1;
        else if ( _u8Film22LikeCounter_ForSlowMotionDisableMCDi > 0 )
            _u8Film22LikeCounter_ForSlowMotionDisableMCDi --;
    }
    else if( _u8Film22LikeCounter_ForSlowMotionDisableMCDi > 0 )
    {
        _u8Film22LikeCounter_ForSlowMotionDisableMCDi --;
    }

    if( _u8Film22LikeCounter_ForSlowMotionDisableMCDi > 10 )
        _u8Film22LikeCounter_ForSlowMotionDisableMCDi = 10;

    if( (u32CurCounter3 > 0x80) && (_u32Counter3_2 < 0x10) && (_u32Counter3_3 > 0x80) )
        _u8Film22LikeCounter2 += 1;
    else if( (u32CurCounter3 < 0x10) && (_u32Counter3_2 > 0x80) && (_u32Counter3_3 < 0x10) )
        _u8Film22LikeCounter2 += 1;
    else
        _u8Film22LikeCounter2 = 0;

    if( _u8Film22LikeCounter2 > 10 )
        _u8Film22LikeCounter2 = 10;

    if( _u8Film22LikeCounter > 10 ) // clamp
        _u8Film22LikeCounter = 10;


    if(u32CurCounter1<0xF)
        _u8FilmsmallCounter1 ++;
    else
        _u8FilmsmallCounter1 = 0;

    if(_u8FilmsmallCounter1>10)
        _u8FilmsmallCounter1 = 10;

    if( _u32Counter3_2 < _u32Counter3_3 )
        u32FixThofC3 = _u32Counter3_2*8;    //u32FixThofC3 = _u32Counter3_2*4;
    else
        u32FixThofC3 = _u32Counter3_3*8;    //u32FixThofC3 = _u32Counter3_3*4;

    bCounter1Have2xDiff = (((u32CurCounter1 < 0xf000) && (_u32Counter1_2 < 0xf000)) &&
                           ((u32CurCounter1 > (_u32Counter1_2*2)) || ((u32CurCounter1*2) < _u32Counter1_2)));

    if( u32FixThofC3 > 0x4C0)
        u32FixThofC3 = 0x4C0;
    else if( u32FixThofC3 < 0x100)
        u32FixThofC3 = 0x100;

    if((!bFilm) && (_u8Film22LikeCounter2 > 5) && (!bCounter1Have2xDiff) ) //when moving sub-pixel, counter3 will like film22, check counter1 don't have big difference
        u32FixThofC3 = 0x100;   // moving shibasogu buildingg

    MHal_XC_W2BYTE(REG_SC_BK0A_0D_L, (MS_U16)u32FixThofC3);
    MHal_XC_W2BYTE(REG_SC_BK0A_41_L, 0xFFFF);
    MHal_XC_W2BYTE(REG_SC_BK0A_5C_L, 0);
    MHal_XC_W2BYTE(REG_SC_BK0A_5E_L, 0);

    //MHal_XC_W2BYTE(REG_SC_BK0A_20_L, 0x10);


    if( u32CurCounter1 > _u32Counter1_2 ) //max
        u32Film22AdaptiveCounter1Th = u32CurCounter1;
    else
        u32Film22AdaptiveCounter1Th = _u32Counter1_2;

    if(u32CurCounter3<5)
    {
        _u8SmallCounter3Cnt += 1;

        if(_u8SmallCounter3Cnt>10)
            _u8SmallCounter3Cnt = 10;
    }
    else
    {
        _u8SmallCounter3Cnt --  ;
    }

    if( (u32CurCounterA > (_u32CounterA_2 + 0x200/0x10)) && (_u32CounterA_3 > (_u32CounterA_2 + 0x200/0x10)))
        _u8Film22LikeCounterA += 1;
    else if(( _u32CounterA_2 > (u32CurCounterA + 0x200/0x10)) && (_u32CounterA_2 >( _u32CounterA_3 + 0x200/0x10)))
        _u8Film22LikeCounterA += 1;
    else if( _u8Film22LikeCounterA > 0 )
        _u8Film22LikeCounterA --;

    if(_u8Film22LikeCounterA>10)
        _u8Film22LikeCounterA = 10;

    if((u32CurCounterB > (_u32CounterB_2 + 0x200/0x10)) &&( _u32CounterB_3 > (_u32CounterB_2 + 0x200/0x10)))
        _u8Film22LikeCounterB += 1;
    else if(( _u32CounterB_2 > (u32CurCounterB + 0x200/0x10)) && (_u32CounterB_2 > (_u32CounterB_3 + 0x200/0x10)))
        _u8Film22LikeCounterB += 1;
    else if( _u8Film22LikeCounterB > 0 )
        _u8Film22LikeCounterB --;

    if(_u8Film22LikeCounterB>10)
        _u8Film22LikeCounterB = 10;


    if ((u32CurCounter3 > ((_u32Counter3_2+1) *10)) && (_u32Counter3_3 > ((_u32Counter3_2+1)*10)))
        _u8Film22LikeCounter3_Big += 1;
    else if((_u32Counter3_2 > ((u32CurCounter3+1)*10)) && (_u32Counter3_2 > ((_u32Counter3_3+1)*10)))
        _u8Film22LikeCounter3_Big += 1;
    else if(_u8Film22LikeCounter3_Big > 0 )
        _u8Film22LikeCounter3_Big --;

    if(_u8Film22LikeCounter3_Big>10)
        _u8Film22LikeCounter3_Big = 10;

    if( ((u32CurCounter3 > (_u32Counter3_2*4))||(u32CurCounter3 > (_u32Counter3_3*4)) ) && (u32CurCounter3>0x40) )
        _u8Film32LikeCounter3_Big += 1;
    else if( ( (_u32Counter3_2 > (_u32Counter3_3*4))||(_u32Counter3_2 > (u32CurCounter3*4)) ) && (_u32Counter3_2>0x40))
        _u8Film32LikeCounter3_Big += 1;
    else if( ( (_u32Counter3_3 > (u32CurCounter3*4))||(_u32Counter3_3 > (_u32Counter3_2*4)) ) && (_u32Counter3_3>0x40))
        _u8Film32LikeCounter3_Big += 1;
    else
        _u8Film32LikeCounter3_Big --;

    if(_u8Film32LikeCounter3_Big>10)
        _u8Film32LikeCounter3_Big = 10;

    if ( (u32CurCounter1 > ((_u32Counter1_2+1)*20)) && (_u32Counter1_3 > ((_u32Counter1_2+1)*20)))
        _u8Film22LikeCounter1_Big += 1;
    else if (( _u32Counter1_2 > ((u32CurCounter1+1)*20)) && (_u32Counter1_2 > ((_u32Counter1_3+1)*20)))
        _u8Film22LikeCounter1_Big += 1;
    else if ( _u8Film22LikeCounter1_Big > 0 )
        _u8Film22LikeCounter1_Big --;

    if(_u8Film22LikeCounter1_Big>10)
        _u8Film22LikeCounter1_Big = 10;

    //for KURO patch
    if( u32CurCounter3 >_u32Counter3_2+0x20 && u32CurCounter1 < _u32Counter1_2+0x20 && u32CurCounter1==0 )
        u16KuroPatchEn = 1;
    else if( u32CurCounter3 <_u32Counter3_2+0x20 && u32CurCounter1 > _u32Counter1_2+0x20 && _u32Counter1_2==0)
        u16KuroPatchEn = 1;


    if( (MS_U16)MHal_XC_R2BYTEMSK(REG_SC_BK0A_21_L, 0x0008) == 0x0008 )
    {
        //in film22
        if(((u32CurCounter3 > (_u32Counter3_2+1)*10)||(_u32Counter3_2 > (u32CurCounter3+1)*10)))
        {
            if(_u8Film22LikeCounter1_Big == 10)
            {
                u32Film22AdaptiveCounter1Th = (u32Film22AdaptiveCounter1Th)*3/8;
            }
            else
            {
                u32Film22AdaptiveCounter1Th = (u32Film22AdaptiveCounter1Th)*9/8;

                if( bIsHDSource )
                {
                    if( u32Film22AdaptiveCounter1Th < 0x400/0x2 *6)
                        u32Film22AdaptiveCounter1Th = 0x400/0x2 *6;
                }
                else
                {
                    if( u32Film22AdaptiveCounter1Th < 0x400/0x2 )
                        u32Film22AdaptiveCounter1Th = 0x400/0x2;
                }
            }
        }
        else
        {
            if ( MHal_XC_R2BYTEMSK(REG_SC_BK12_03_L, BIT(1)) ) // 2R
            {
                u32Film22AdaptiveCounter1Th = 0xFFFF;
            }
            else
            {
                if(_u8Film22LikeCounter1_Big == 10)
                    u32Film22AdaptiveCounter1Th = (u32Film22AdaptiveCounter1Th)*3/8;
                else
                    u32Film22AdaptiveCounter1Th = (u32Film22AdaptiveCounter1Th)*6/8;
            }

        }
    }
    else if( (MS_U16)MHal_XC_R2BYTEMSK(REG_SC_BK0A_21_L, 0x0010) == 0x0010 )
    {
        //in film32
        if((u32CurCounter3 > (_u32Counter3_2*10))||(_u32Counter3_2 > (u32CurCounter3*10))||(_u32Counter3_3 > (_u32Counter3_2*10)))
        {
            if( (u32CurCounter1>= _u32Counter1_2) && (u32CurCounter1 >= _u32Counter1_3)  )
                u32Film22AdaptiveCounter1Th = u32CurCounter1 * 2;
            else if((_u32Counter1_2>= u32CurCounter1) && (_u32Counter1_2 >= _u32Counter1_3))
                u32Film22AdaptiveCounter1Th = _u32Counter1_2 * 2;
            else if((_u32Counter1_3>= u32CurCounter1) &&( _u32Counter1_3 >= _u32Counter1_2))
                u32Film22AdaptiveCounter1Th = _u32Counter1_3 * 2;

            if( bIsHDSource )
            {
                if( u32Film22AdaptiveCounter1Th < (0x400/0x2*6) )
                    u32Film22AdaptiveCounter1Th = 0x400/0x2 *6;
            }
            else
            {
                if( u32Film22AdaptiveCounter1Th < (0x400/0x2) )
                    u32Film22AdaptiveCounter1Th = 0x400/0x2;
            }
        }
        else
        {
            if ( MHal_XC_R2BYTEMSK(REG_SC_BK12_03_L, BIT(1)) ) // 2R
            {
                u32Film22AdaptiveCounter1Th = 0xFFFF;
            }
            else
            {
                u32Film22AdaptiveCounter1Th = (u32Film22AdaptiveCounter1Th)*6/8;
            }
        }
    }
    else if( (MS_U16)MHal_XC_R2BYTEMSK(REG_SC_BK0A_21_L, 0x0040) == 0x0040 )
    {
        //in filmany
        u32Film22AdaptiveCounter1Th = 0x800;
    }
    else
    {
        if(_u8SmallCounter3Cnt==10)
        {
            if((_u8Film22LikeCounterA>5) || (_u8Film22LikeCounterB>5))
            {
                u32Film22AdaptiveCounter1Th = (u32Film22AdaptiveCounter1Th)*6/8;
                if( bIsHDSource )
                {
                    if( u32Film22AdaptiveCounter1Th < (0x400/0x2*6) )
                        u32Film22AdaptiveCounter1Th = 0x400/0x2*6;
                }
                else
                {
                    if( u32Film22AdaptiveCounter1Th < 0x400/0x2 )
                        u32Film22AdaptiveCounter1Th = 0x400/0x2;
                }
            }
            else
            {
                if( bIsHDSource )
                {
                    u32Film22AdaptiveCounter1Th = 0x400/0x2*6;
                }
                else
                {
                    u32Film22AdaptiveCounter1Th = 0x400/0x2;
                }
            }
        }
        else
        {
            u32Film22AdaptiveCounter1Th = (u32Film22AdaptiveCounter1Th)*6/8;

            if( bIsHDSource )
            {
                if( u32Film22AdaptiveCounter1Th < 0x400/0x2*6 )
                    u32Film22AdaptiveCounter1Th = 0x400/0x2*6;
            }
            else
            {
                if( u32Film22AdaptiveCounter1Th < 0x400/0x2 )
                    u32Film22AdaptiveCounter1Th = 0x400/0x2;
            }
        }

        if(_u8Film22LikeCounter3_Big>8 || _u8Film32LikeCounter3_Big>8)
            u32Film22AdaptiveCounter1Th  = 0x7500;
    }


    if( bIsHDSource )
    {
        if( u32Film22AdaptiveCounter1Th < 0x400/0x4*6 )
            u32Film22AdaptiveCounter1Th = 0x400/0x4*6;
    }
    else
    {
        if( u32Film22AdaptiveCounter1Th < 0x800)
            u32Film22AdaptiveCounter1Th = 0x800;
    }

    _u32Counter3_3 = _u32Counter3_2;
    _u32Counter3_2  = u32CurCounter3;
    _u32Counter1_4 = _u32Counter1_3;
    _u32Counter1_3 = _u32Counter1_2;
    _u32Counter1_2  = u32CurCounter1;

    _u32CounterA_3 = _u32CounterA_2;
    _u32CounterA_2  = u32CurCounterA;
    _u32CounterB_3 = _u32CounterB_2;
    _u32CounterB_2  = u32CurCounterB;

    bIsFilmXOO = (_u8IsFilmXOOCounter != 0);

#ifdef PRJ_MCNR  // For A3, A5,A7P,T12
    u32Reg7C = MHal_XC_R2BYTE(REG_SC_BK2A_7C_L);
    u32Reg7D = MHal_XC_R2BYTE(REG_SC_BK2A_7D_L);
    u32Reg7E = MHal_XC_R2BYTE(REG_SC_BK2A_7E_L);

    // for all chip
    if( bIsHDSource )
    {
        // for MCNR only
        bVerticalMovingU = ((u32Reg7E > 0x0200) && (u32Reg7E > (8*u32Reg7C)) && (u32Reg7E > (8*u32Reg7D)));
    }
    else
    {
        // for MCNR only
        bVerticalMovingU = ((u32Reg7E > 0x0100) && (u32Reg7E > (8*u32Reg7C)) && (u32Reg7E > (8*u32Reg7D)));
    }
#else
    // for no MCNR
    bVerticalMovingU = false;
#endif

    u16Unmatch_3 = MHal_XC_R2BYTE(REG_SC_BK0A_0B_L);
    u16Unmatch_1 = MHal_XC_R2BYTE(REG_SC_BK0A_0A_L);

    // for sec , activate counter 1
    if( u16Unmatch_3 < 0x40 )
    {
        _u8NoCounter3_Cnt ++;
    }
    else
    {
        _u8NoCounter3_Cnt = 0;
    }

    // deactivate counter 1 jump out
    if( u16Unmatch_3 > 0x6000  )
    {
        _u8BigCounter3_Cnt = 10;
    }
    else if(_u8BigCounter3_Cnt > 0)
    {
        _u8BigCounter3_Cnt --;
    }

    if( u16Unmatch_3 > 0x1000  )
    {
        _u8BigCounter3_Cnt2 = 10;
    }
    else if(_u8BigCounter3_Cnt2 > 0)
    {
        _u8BigCounter3_Cnt2 --;
    }

    // deactivate counter 1 jump out
#ifdef PRJ_MCNR  // For A3, A5,A7P,T12
    if( u16Unmatch_1 > 0xF000  )
#else
    if( u16Unmatch_1 > 0xA000  )
#endif
    {
        _u8BigCounter1_Cnt = 10;
    }
    else if(_u8BigCounter1_Cnt > 0)
    {
        _u8BigCounter1_Cnt --;
    }

    if( u16Unmatch_1 > 0xc000)
    {
        if( _u8BigCounter_Cnt3 < 80)
            _u8BigCounter_Cnt3 ++;
    }
    else
        _u8BigCounter_Cnt3 = 0;

    if( u16Unmatch_1 > 0x1000 )
        _u8BigCounter1_Cnt2 = 0x8;
    else if( _u8BigCounter1_Cnt2 != 0)
        _u8BigCounter1_Cnt2 --;

    if( u16Unmatch_1 < 0x0800 )
    {
        _u8SmallCounter1_Cnt = 10;
    }
    else if(_u8SmallCounter1_Cnt > 0)
    {
        _u8SmallCounter1_Cnt --;
    }

    if((MHal_XC_R2BYTEMSK(REG_SC_BK12_01_L, 0xFF00)==0x2500) || (MHal_XC_R2BYTEMSK(REG_SC_BK12_01_L, 0xFF00)==0x2700) )//(stXCStatus.bInterlace)
    {
        if(_u8NoCounter3_Cnt > 20)
        {
            MHal_XC_W2BYTEMSK(REG_SC_BK0A_02_L, ((MS_U16)0x01)<<8, 0x1F00);
            MHal_XC_W2BYTEMSK(REG_SC_BK0A_05_L, ((MS_U16)0x10)<<8, 0xFF00);
        }
        else
        {
            MHal_XC_W2BYTEMSK(REG_SC_BK0A_02_L, ((MS_U16)0x04)<<8, 0x1F00);  // for SD cross hatch
            MHal_XC_W2BYTEMSK(REG_SC_BK0A_05_L, ((MS_U16)0x10)<<8, 0xFF00);
        }

        // Film32 On,f2 32 film mode enable
        if( (MHal_XC_R2BYTEMSK( REG_SC_BK0A_10_L, BIT(14) ) ==  BIT(14) )
            || (MHal_XC_R2BYTEMSK( REG_SC_BK0A_10_L, BIT(15) ) ==  BIT(15) ) )
        {
            ///--> new add function for skip film mode in still pattern
            if((!MHal_XC_R2BYTEMSK(REG_SC_BK12_03_L, BIT(1)))&&(!MHal_XC_R2BYTEMSK(REG_SC_BK0A_02_L, BIT(14))))    // 2R mode can't enter this loop
            {
                u16Unmatch_3 = MHal_XC_R2BYTE(REG_SC_BK0A_0B_L);

                if( (u16Unmatch_3 > 0x80) )
                    _u8Unmatch3Cnt = 0;
                else if(_u8Unmatch3Cnt == 0xFF)
                    _u8Unmatch3Cnt = 0xFF;
                else
                    _u8Unmatch3Cnt = _u8Unmatch3Cnt + 1;

                ///--> Jump out Film for LG tower
                if( !bVerticalMovingU )
                    _u8VerticalCnt = 0;
                else if(_u8VerticalCnt == 0xFF)
                    _u8VerticalCnt = 0xFF;
                else
                    _u8VerticalCnt = _u8VerticalCnt + 1;
            }
        }
        else
        {
            // DEFAULT value of hardware and software film mode suggested by SY.Shih
            MHal_XC_W2BYTEMSK(REG_SC_BK0A_1E_L, 0, BIT(1));
            MHal_XC_W2BYTEMSK(REG_SC_BK0A_21_L, 0, BIT(1) | BIT(2));
        }

        // film-any continuous error line patch, not easy to enter film, but easy to exit film
        if ( (MS_U16)MHal_XC_R2BYTEMSK(REG_SC_BK0A_21_L, 0x0040) == 0x0040 ) // film-any status
            MHal_XC_W2BYTEMSK(REG_SC_BK0A_57_L, 0x08, 0xff);
        else
            MHal_XC_W2BYTEMSK(REG_SC_BK0A_57_L, 0x20, 0xff);
#if 0
        if ( bIsHDSource && (!bFilm) )
            MHal_XC_W2BYTEMSK(REG_SC_BK0A_15_L, 0x0008, 0x000F ); // for film32, also favor de-bouncing
        else
            MHal_XC_W2BYTEMSK(REG_SC_BK0A_15_L, 0x0008, 0x000F ); // for De-bouncing
#endif
        if(_u8VerticalCnt>0x03)  // only active at Film
        {
            MHal_XC_W2BYTEMSK(REG_SC_BK0A_23_L, 0, BIT(15));
        }
        else
        {
            if(u32CurCounter3<0x9 )
            {
                _u8StillCounter3++;
            }
            else
            {
                _u8StillCounter3 = 0;
            }

            if(_u8StillCounter3>=8)
            {
                _u8StillCounter3 = 8;
            }

            if(u32Film22AdaptiveCounter1Th>0xFFFF)
                u32Film22AdaptiveCounter1Th = 0xFFFF;

            MHal_XC_W2BYTE(REG_SC_BK0A_0C_L, (MS_U16)u32Film22AdaptiveCounter1Th );


            if(_u8StillCounter3==8 || u16KuroPatchEn==1 || _u8FilmsmallCounter1==10) //default
            {
                if ( MHal_XC_R2BYTEMSK(REG_SC_BK12_03_L, BIT(1)) )  // 2R
                {

                }
                else //4R
                {
                    MHal_XC_W2BYTE(REG_SC_BK0A_0C_L, 0 );
                    MHal_XC_W2BYTE(REG_SC_BK0A_0D_L, 0 );
                }
            }

            if( bIs60Hz ) //NTSC -> turn off fast film22
                MHal_XC_W2BYTEMSK(REG_SC_BK0A_23_L, 0, BIT(15));
            else // PAL
                MHal_XC_W2BYTEMSK(REG_SC_BK0A_23_L, BIT(15), BIT(15));
        }

        // for SEC pattern gen
        if(_u8NoCounter3_Cnt > 20)
            MHal_XC_W2BYTEMSK(REG_SC_BK0A_25_L, 0x00FF, 0xFFFF); // disable film-any
        else if ( !bIsFilmXOO )
            MHal_XC_W2BYTEMSK(REG_SC_BK0A_25_L, 0x0080, 0xFFFF); // disable film-any
        else
            MHal_XC_W2BYTEMSK(REG_SC_BK0A_25_L, 0x0810, 0xFFFF);

        // for auto 1
        // film32
        u32Counter3  = MHal_XC_R2BYTE(REG_SC_BK0A_0B_L);


        if( u32Counter3 < 0x100 )
            _u8MovingCharacterCounter = 0; //
        else if(_u8MovingCharacterCounter != 0xff )
            _u8MovingCharacterCounter ++;


        if ( bIsHDSource )
        {
            MHal_XC_W2BYTE(REG_SC_BK0A_06_L, 0xCF40);
            MHal_XC_W2BYTEMSK(REG_SC_BK0A_17_L, BIT(13),BIT(13) );
            //MHal_XC_W2BYTE(REG_SC_BK0A_1F_L, 0x0fff);
        }
        else
        {
            MHal_XC_W2BYTE(REG_SC_BK0A_06_L, 0xA810);
            MHal_XC_W2BYTEMSK(REG_SC_BK0A_17_L, 0x00 ,BIT(13) );
            MHal_XC_W2BYTE(REG_SC_BK0A_1F_L, 0x0250);
        }

        //MHal_XC_W2BYTE(REG_SC_BK0A_16_L, 0x00D8);
        if ( bIsHDSource )
        {
            MHal_XC_W2BYTE(REG_SC_BK0A_16_L, 0x0078);// 1080i
        }
        else
        {
            MHal_XC_W2BYTE(REG_SC_BK0A_16_L, 0x00D8);  // 480i
        }


        MHal_XC_W2BYTE(REG_SC_BK0A_1E_L, 0x0551);


        if ( MHal_XC_R2BYTEMSK(REG_SC_BK22_7C_L, BIT(12)) ) // auto 1 , deal with video-over-film
        {
        }
        else // default
        {
            if ( bIsHDSource )
            {
                MHal_XC_W2BYTE(REG_SC_BK0A_07_L, 0x7F10);

            }
            else
            {
                MHal_XC_W2BYTE(REG_SC_BK0A_07_L, 0x7F08);
            }
        }
    } // end interlace loop
    _u16RdBank_Prv = u16RdBank;

    if(MHal_XC_R2BYTEMSK(REG_SC_BK2A_02_L, BIT(15)))
    {
        u16InvStillFrameCnt = 15;
    }
    else
    {
        u16InvStillFrameCnt = 0;
    }

    if((_u8Film22LikeCounter > 0x5) || (_u8Film22LikeCounter_ForSlowMotionDisableMCDi > 0x5))
    {
        MHal_XC_W2BYTEMSK(REG_SC_BK2A_2F_L, 0x8000, 0xFFFF); //disable MCDi
        MHal_XC_W2BYTEMSK(REG_SC_BK2A_29_L, 0xF000, 0xFF00); //Favor SST
        MHal_XC_W2BYTEMSK(REG_SC_BK2A_2A_L, 0x0000, 0xFF00); //Disable KFC-cplx
    }
    else
    {
        MHal_XC_W2BYTEMSK(REG_SC_BK2A_2F_L, 0x0000, 0xFFFF);
        MHal_XC_W2BYTEMSK(REG_SC_BK2A_29_L, 0x2100, 0xFF00);
        MHal_XC_W2BYTEMSK(REG_SC_BK2A_2A_L, ((MS_U16)0xF000)|((MS_U16)(u16InvStillFrameCnt<<8)), 0xFF00);
        MHal_XC_W2BYTEMSK(REG_SC_BK22_2A_L, (MS_U16)(u16InvStillFrameCnt<<4), 0x00F0);
    }
}

void MDrv_XC_FilmDriverHWVer2(void)
{
    MS_BOOL bIsFilmXOO = 0;
    MS_U32 u32Film22AdaptiveCounter1Th = 0;
    static MS_U16 _u16RdBank_Prv=0;
    MS_U16 u16RdBank;
    MS_U32 u32CurCounter1=0, u32CurCounter3=0, u32FixThofC3=0;
    static MS_U32 _u32Counter3_2=0, _u32Counter3_3=0;
    static MS_U32 _u32Counter1_2=0, _u32Counter1_3=0, _u32Counter1_4=0;
    static MS_U32 _u32CounterA_2=0, _u32CounterA_3=0;
    static MS_U32 _u32CounterB_2=0, _u32CounterB_3=0;
    static MS_U8 _u8Film22LikeCounter, _u8Film22LikeCounter2;
    static MS_U8 _u8Film22LikeCounter_ForSlowMotionDisableMCDi;
    static MS_U8 _u8SceneChangeFrame;
    static MS_U8 _u8IsFilmXOOCounter=0;
    static MS_BOOL _bIs2ndFilmXOOCounter;
    MS_BOOL bCounter1Have2xDiff, bFilm, bFilmAny;
    MS_U32 u32CurCounterA, u32CurCounterB;
    static MS_U8 _u8SmallCounter3Cnt;
    static MS_U8 _u8Film22LikeCounterA, _u8Film22LikeCounterB;
    static MS_U8 _u8Film22LikeCounter3_Big;
    static MS_U8 _u8Film32LikeCounter3_Big;
    static MS_U8 _u8Film22LikeCounter1_Big;
    static MS_U8 _u8VerticalCnt, _u8Unmatch3Cnt;
    MS_BOOL bIsHDSource;
    MS_U16 u16Unmatch_3, u16Unmatch_1;
    static MS_U8 _u8NoCounter3_Cnt, _u8BigCounter3_Cnt, _u8BigCounter1_Cnt, _u8BigCounter_Cnt3;
    static MS_U8 _u8BigCounter1_Cnt2, _u8SmallCounter1_Cnt, _u8BigCounter3_Cnt2;
    MS_BOOL bVerticalMovingU ;
    MS_BOOL bIs60Hz = ( MHal_XC_R2BYTE(REG_SC_BK0A_10_L) & 0x8000 ) != 0; // checkfilm32 enable for 60Hz source
    static MS_U8 _u8StillCounter3;
    MS_U32 u32Counter3;
    static MS_U8 _u8MovingCharacterCounter;
#ifdef PRJ_MCNR  // For A3, A5,A7P,T12
    MS_U32 u32Reg7C, u32Reg7D, u32Reg7E;
#endif
    MS_U16 u16KuroPatchEn = 0;  //for KURO patch

    //Check the IPM 3D input and OP 3D output or 2D to 3D enable.
    if((( (MS_U16)MHal_XC_R2BYTEMSK(REG_SC_BK12_30_L, 0x01) == 0x01 )
        && ( (MS_U16)MHal_XC_R2BYTEMSK(REG_SC_BK12_33_L, 0x80) == 0x80 ))
       ||((( (MS_U16)MHal_XC_R2BYTEMSK(REG_SC_BK62_06_L, 0x7FFF) != 0x0 )
           || ( (MS_U16)MHal_XC_R2BYTEMSK(REG_SC_BK62_0E_L, 0x7FFF) != 0x0 ))
          && ( (MS_U16)MHal_XC_R2BYTEMSK(REG_SC_BK62_08_L, 0x10) == 0x10 )
          && ( (MS_U16)MHal_XC_R2BYTEMSK(REG_SC_BK63_61_L, 0x01) == 0x01 )))
    {
        //For 3D mode or 2D to 3D mode need to close the film driver .
        MHal_XC_W2BYTEMSK(REG_SC_BK0A_10_L, 0, 0xC000);
        MHal_XC_W2BYTEMSK(REG_SC_BK0A_24_L, 0, 0x8000);
        return ;
    }

    u16RdBank = ( MHal_XC_R2BYTE(REG_SC_BK12_3A_L) & 0xF0 ) >> 4;
    bIsHDSource = ( MHal_XC_R2BYTEMSK(REG_SC_BK12_0F_L, 0xFFFF) > 0x700 );

    u32CurCounterA = MHal_XC_R2BYTE(REG_SC_BK0A_4A_L);
    u32CurCounterB = MHal_XC_R2BYTE(REG_SC_BK0A_4B_L);

    u32CurCounter1 = MHal_XC_R2BYTE(REG_SC_BK0A_0A_L);
    u32CurCounter3 = MHal_XC_R2BYTE(REG_SC_BK0A_0B_L);

    if(u16RdBank == _u16RdBank_Prv)  // Not change read bank
    {
        return ;
    }

    bFilmAny = ( (MS_U16)MHal_XC_R2BYTEMSK(REG_SC_BK0A_21_L, 0x0040) == 0x0040);
    bFilm = (( (MS_U16)MHal_XC_R2BYTEMSK(REG_SC_BK0A_21_L, 0x0008) == 0x0008) ||  // film22
             ( (MS_U16)MHal_XC_R2BYTEMSK(REG_SC_BK0A_21_L, 0x0010) == 0x0010) ||  // film32
             ( (MS_U16)MHal_XC_R2BYTEMSK(REG_SC_BK0A_21_L, 0x0040) == 0x0040) );   // film any

    // check XOO
    if( ( (!bFilm) && (u32CurCounter3 > 0x200) && (_u32Counter3_2< 0x80) && (_u32Counter3_3< 0x80) ) || //not film
        ( (!bFilm) && (u32CurCounter3 > 0x80) && (_u32Counter3_2< 0x20) && (_u32Counter3_3< 0x20) ) ) //not film
    {
        if ( _u8IsFilmXOOCounter > 0 )
        {
            _bIs2ndFilmXOOCounter = true;
        }

        _u8IsFilmXOOCounter = 10; // need to hit XOO on every 10 frames
    }
    else if( bFilm && (!bFilmAny) && (u32CurCounter3 > 0x200) && (u32CurCounter3 > (_u32Counter3_2*2)) && (u32CurCounter3 > (_u32Counter3_3*2)) ) //32 or 22
    {
        if( _u8IsFilmXOOCounter > 0 )
        {
            _bIs2ndFilmXOOCounter = true;
        }
        _u8IsFilmXOOCounter = 10; // need to hit XOO on every 10 frames
    }
    else if(  bFilm && (u32CurCounter3 > 0x80)  && (u32CurCounter3 > (_u32Counter3_2*2)) && (u32CurCounter3 > (_u32Counter3_3*2)) ) // film-any
    {
        _bIs2ndFilmXOOCounter = true;
        _u8IsFilmXOOCounter = 10; // need to hit XOO on every 10 frames
    }
    else if( _u8IsFilmXOOCounter != 0 )
    {
        _u8IsFilmXOOCounter --;
    }
    else
    {
        _bIs2ndFilmXOOCounter = false;
    }

    //detect scene change into film...
    if( (u32CurCounter1 > (_u32Counter1_4*4)) && (u32CurCounter1 > (_u32Counter1_2*4)) && (u32CurCounter1 > (_u32Counter1_3*4)) )
        _u8SceneChangeFrame = 60;
    else if( _u8SceneChangeFrame != 0 )
        _u8SceneChangeFrame --;

    if( (u32CurCounter3 > 0x80) && (_u32Counter3_2 < 0x10) && (_u32Counter3_3 > 0x80) )
        _u8Film22LikeCounter += 2;
    else if( (u32CurCounter3 < 0x10) && (_u32Counter3_2 > 0x80) && (_u32Counter3_3 < 0x10) )
        _u8Film22LikeCounter += 2;
    else if( (u32CurCounter1 > (_u32Counter1_2 + 0x20)) && (_u32Counter1_3 > (_u32Counter1_2 + 0x20)) )
        _u8Film22LikeCounter += 1;
    else if( (_u32Counter1_2 > (u32CurCounter1 + 0x20)) && (_u32Counter1_2 > (_u32Counter1_3 + 0x20)) )
        _u8Film22LikeCounter += 1;
    else if ( _u8Film22LikeCounter > 0 )
        _u8Film22LikeCounter --;

    if( u32CurCounter3 < 0x80 ) // for slow motion film22-like
    {
        if( (u32CurCounter1 > (_u32Counter1_2*2)) && (_u32Counter1_3 > (_u32Counter1_2*2)) )
            _u8Film22LikeCounter_ForSlowMotionDisableMCDi += 1;
        else if ( (_u32Counter1_2 > (u32CurCounter1*2)) && (_u32Counter1_2 > (_u32Counter1_3*2)) )
            _u8Film22LikeCounter_ForSlowMotionDisableMCDi += 1;
        else if ( _u8Film22LikeCounter_ForSlowMotionDisableMCDi > 0 )
            _u8Film22LikeCounter_ForSlowMotionDisableMCDi --;
    }
    else if( _u8Film22LikeCounter_ForSlowMotionDisableMCDi > 0 )
    {
        _u8Film22LikeCounter_ForSlowMotionDisableMCDi --;
    }

    if( _u8Film22LikeCounter_ForSlowMotionDisableMCDi > 10 )
        _u8Film22LikeCounter_ForSlowMotionDisableMCDi = 10;

    if( (u32CurCounter3 > 0x80) && (_u32Counter3_2 < 0x10) && (_u32Counter3_3 > 0x80) )
        _u8Film22LikeCounter2 += 1;
    else if( (u32CurCounter3 < 0x10) && (_u32Counter3_2 > 0x80) && (_u32Counter3_3 < 0x10) )
        _u8Film22LikeCounter2 += 1;
    else
        _u8Film22LikeCounter2 = 0;

    if( _u8Film22LikeCounter2 > 10 )
        _u8Film22LikeCounter2 = 10;

    if( _u8Film22LikeCounter > 10 ) // clamp
        _u8Film22LikeCounter = 10;

    if( _u32Counter3_2 < _u32Counter3_3 )
        u32FixThofC3 = _u32Counter3_2*4;
    else
        u32FixThofC3 = _u32Counter3_3*4;

    bCounter1Have2xDiff = (((u32CurCounter1 < 0xf000) && (_u32Counter1_2 < 0xf000)) &&
                           ((u32CurCounter1 > (_u32Counter1_2*2)) || ((u32CurCounter1*2) < _u32Counter1_2)));

    if( u32FixThofC3 > 0x4C0)
        u32FixThofC3 = 0x4C0;
    else if( u32FixThofC3 < 0x100)
        u32FixThofC3 = 0x100;

    if((!bFilm) && (_u8Film22LikeCounter2 > 5) && (!bCounter1Have2xDiff) ) //when moving sub-pixel, counter3 will like film22, check counter1 don't have big difference
        u32FixThofC3 = 0x100;   // moving shibasogu buildingg


    MHal_XC_W2BYTE(REG_SC_BK0A_0D_L, (MS_U16)u32FixThofC3);
    MHal_XC_W2BYTE(REG_SC_BK0A_41_L, 0xFFFF);
    MHal_XC_W2BYTE(REG_SC_BK0A_5C_L, 0);
    MHal_XC_W2BYTE(REG_SC_BK0A_5E_L, 0);
    //MHal_XC_W2BYTEMSK(REG_SC_BK0A_02_L, 0, 0x0F );

    //MHal_XC_W2BYTE(REG_SC_BK0A_20_L, 0x10);

    if( u32CurCounter1 > _u32Counter1_2 ) //max
        u32Film22AdaptiveCounter1Th = u32CurCounter1;
    else
        u32Film22AdaptiveCounter1Th = _u32Counter1_2;

    if(u32CurCounter3<5)
    {
        _u8SmallCounter3Cnt += 1;

        if(_u8SmallCounter3Cnt>10)
            _u8SmallCounter3Cnt = 10;
    }
    else
    {
        _u8SmallCounter3Cnt --  ;
    }

    if( (u32CurCounterA > (_u32CounterA_2 + 0x200/0x10)) && (_u32CounterA_3 > (_u32CounterA_2 + 0x200/0x10)))
        _u8Film22LikeCounterA += 1;
    else if(( _u32CounterA_2 > (u32CurCounterA + 0x200/0x10)) && (_u32CounterA_2 >( _u32CounterA_3 + 0x200/0x10)))
        _u8Film22LikeCounterA += 1;
    else if( _u8Film22LikeCounterA > 0 )
        _u8Film22LikeCounterA --;

    if(_u8Film22LikeCounterA>10)
        _u8Film22LikeCounterA = 10;

    if((u32CurCounterB > (_u32CounterB_2 + 0x200/0x10)) &&( _u32CounterB_3 > (_u32CounterB_2 + 0x200/0x10)))
        _u8Film22LikeCounterB += 1;
    else if(( _u32CounterB_2 > (u32CurCounterB + 0x200/0x10)) && (_u32CounterB_2 > (_u32CounterB_3 + 0x200/0x10)))
        _u8Film22LikeCounterB += 1;
    else if( _u8Film22LikeCounterB > 0 )
        _u8Film22LikeCounterB --;

    if(_u8Film22LikeCounterB>10)
        _u8Film22LikeCounterB = 10;


    if ((u32CurCounter3 > ((_u32Counter3_2+1) *10)) && (_u32Counter3_3 > ((_u32Counter3_2+1)*10)))
        _u8Film22LikeCounter3_Big += 1;
    else if((_u32Counter3_2 > ((u32CurCounter3+1)*10)) && (_u32Counter3_2 > ((_u32Counter3_3+1)*10)))
        _u8Film22LikeCounter3_Big += 1;
    else if(_u8Film22LikeCounter3_Big > 0 )
        _u8Film22LikeCounter3_Big --;

    if(_u8Film22LikeCounter3_Big>10)
        _u8Film22LikeCounter3_Big = 10;

    if( ((u32CurCounter3 > (_u32Counter3_2*4))||(u32CurCounter3 > (_u32Counter3_3*4)) ) && (u32CurCounter3>0x40) )
        _u8Film32LikeCounter3_Big += 1;
    else if( ( (_u32Counter3_2 > (_u32Counter3_3*4))||(_u32Counter3_2 > (u32CurCounter3*4)) ) && (_u32Counter3_2>0x40))
        _u8Film32LikeCounter3_Big += 1;
    else if( ( (_u32Counter3_3 > (u32CurCounter3*4))||(_u32Counter3_3 > (_u32Counter3_2*4)) ) && (_u32Counter3_3>0x40))
        _u8Film32LikeCounter3_Big += 1;
    else
        _u8Film32LikeCounter3_Big --;

    if(_u8Film32LikeCounter3_Big>10)
        _u8Film32LikeCounter3_Big = 10;

    if ( (u32CurCounter1 > ((_u32Counter1_2+1)*20)) && (_u32Counter1_3 > ((_u32Counter1_2+1)*20)))
        _u8Film22LikeCounter1_Big += 1;
    else if (( _u32Counter1_2 > ((u32CurCounter1+1)*20)) && (_u32Counter1_2 > ((_u32Counter1_3+1)*20)))
        _u8Film22LikeCounter1_Big += 1;
    else if ( _u8Film22LikeCounter1_Big > 0 )
        _u8Film22LikeCounter1_Big --;

    if(_u8Film22LikeCounter1_Big>10)
        _u8Film22LikeCounter1_Big = 10;


    //for KURO patch
    if( u32CurCounter3 >_u32Counter3_2+0x20 && u32CurCounter1 < _u32Counter1_2+0x20 && u32CurCounter1==0 )
        u16KuroPatchEn = 1;
    else if( u32CurCounter3 <_u32Counter3_2+0x20 && u32CurCounter1 > _u32Counter1_2+0x20 && _u32Counter1_2==0)
        u16KuroPatchEn = 1;

    if( (MS_U16)MHal_XC_R2BYTEMSK(REG_SC_BK0A_21_L, 0x0008) == 0x0008 )
    {
        //in film22
        if(((u32CurCounter3 > (_u32Counter3_2+1)*10)||(_u32Counter3_2 > (u32CurCounter3+1)*10)))
        {
            if(_u8Film22LikeCounter1_Big == 10)
                u32Film22AdaptiveCounter1Th = (u32Film22AdaptiveCounter1Th)*3/8;

            else
            {
                u32Film22AdaptiveCounter1Th = (u32Film22AdaptiveCounter1Th)*9/8;

                if( u32Film22AdaptiveCounter1Th < 0x400/0x2 )
                    u32Film22AdaptiveCounter1Th = 0x400/0x2;
            }
        }
        else
        {
            if(_u8Film22LikeCounter1_Big == 10)
                u32Film22AdaptiveCounter1Th = (u32Film22AdaptiveCounter1Th)*3/8;
            else
                u32Film22AdaptiveCounter1Th = (u32Film22AdaptiveCounter1Th)*6/8;

        }
    }
    else if( (MS_U16)MHal_XC_R2BYTEMSK(REG_SC_BK0A_21_L, 0x0010) == 0x0010 )
    {
        //in film32
        if((u32CurCounter3 > (_u32Counter3_2*10))||(_u32Counter3_2 > (u32CurCounter3*10))||(_u32Counter3_3 > (_u32Counter3_2*10)))
        {
            if( (u32CurCounter1>= _u32Counter1_2) && (u32CurCounter1 >= _u32Counter1_3)  )
                u32Film22AdaptiveCounter1Th = u32CurCounter1 * 2;
            else if((_u32Counter1_2>= u32CurCounter1) && (_u32Counter1_2 >= _u32Counter1_3))
                u32Film22AdaptiveCounter1Th = _u32Counter1_2 * 2;
            else if((_u32Counter1_3>= u32CurCounter1) &&( _u32Counter1_3 >= _u32Counter1_2))
                u32Film22AdaptiveCounter1Th = _u32Counter1_3 * 2;

            if( u32Film22AdaptiveCounter1Th < (0x400/0x2) )
                u32Film22AdaptiveCounter1Th = 0x400/0x2;
        }
        else
        {
            u32Film22AdaptiveCounter1Th = (u32Film22AdaptiveCounter1Th)*6/8;
        }
    }
    else if( (MS_U16)MHal_XC_R2BYTEMSK(REG_SC_BK0A_21_L, 0x0040) == 0x0040 )
    {
        //in filmany
        u32Film22AdaptiveCounter1Th = 0x5F00;
    }
    else
    {
        if(_u8SmallCounter3Cnt==10)
        {
            if((_u8Film22LikeCounterA>5) || (_u8Film22LikeCounterB>5))
            {
                u32Film22AdaptiveCounter1Th = (u32Film22AdaptiveCounter1Th)*6/8;

                if( u32Film22AdaptiveCounter1Th > 0x400/0x2 )
                    u32Film22AdaptiveCounter1Th = 0x400/0x2;

            }
            else
            {
                u32Film22AdaptiveCounter1Th = 0x400/0x2;
            }
        }
        else
        {
            u32Film22AdaptiveCounter1Th = (u32Film22AdaptiveCounter1Th)*6/8;

            if( u32Film22AdaptiveCounter1Th < 0x400/0x2 )
                u32Film22AdaptiveCounter1Th = 0x400/0x2;
        }

        if(_u8Film22LikeCounter3_Big>8 || _u8Film32LikeCounter3_Big>8)
            u32Film22AdaptiveCounter1Th  = 0xFFFF;
    }

    if( (MS_U16)MHal_XC_R2BYTEMSK(REG_SC_BK0A_21_L, 0x0010) == 0x0010 ) // in film32
    {
        if( u32Film22AdaptiveCounter1Th < 0x400/0x2)
            u32Film22AdaptiveCounter1Th = 0x400/0x2;
    }
    else //other film mode
    {
        if( u32Film22AdaptiveCounter1Th < 0x400/0x4)
            u32Film22AdaptiveCounter1Th = 0x400/0x4;
    }

    _u32Counter3_3 = _u32Counter3_2;
    _u32Counter3_2  = u32CurCounter3;
    _u32Counter1_4 = _u32Counter1_3;
    _u32Counter1_3 = _u32Counter1_2;
    _u32Counter1_2  = u32CurCounter1;

    _u32CounterA_3 = _u32CounterA_2;
    _u32CounterA_2  = u32CurCounterA;
    _u32CounterB_3 = _u32CounterB_2;
    _u32CounterB_2  = u32CurCounterB;

    bIsFilmXOO = (_u8IsFilmXOOCounter != 0);

#ifdef PRJ_MCNR  // For A3, A5,A7P,T12
    u32Reg7C = MHal_XC_R2BYTE(REG_SC_BK2A_7C_L);
    u32Reg7D = MHal_XC_R2BYTE(REG_SC_BK2A_7D_L);
    u32Reg7E = MHal_XC_R2BYTE(REG_SC_BK2A_7E_L);

    // for all chip
    if( bIsHDSource )
    {
        // for MCNR only
        bVerticalMovingU = ((u32Reg7E > 0x0200) && (u32Reg7E > (8*u32Reg7C)) && (u32Reg7E > (8*u32Reg7D)));
    }
    else
    {
        // for MCNR only
        bVerticalMovingU = ((u32Reg7E > 0x0100) && (u32Reg7E > (8*u32Reg7C)) && (u32Reg7E > (8*u32Reg7D)));
    }
#else
    // for no MCNR
    bVerticalMovingU = false;
#endif

    u16Unmatch_3 = MHal_XC_R2BYTE(REG_SC_BK0A_0B_L);
    u16Unmatch_1 = MHal_XC_R2BYTE(REG_SC_BK0A_0A_L);

    // for sec , activate counter 1
    if( u16Unmatch_3 < 0x40 )
    {
        _u8NoCounter3_Cnt ++;
    }
    else
    {
        _u8NoCounter3_Cnt = 0;
    }

    // deactivate counter 1 jump out
    if( u16Unmatch_3 > 0x6000  )
    {
        _u8BigCounter3_Cnt = 10;
    }
    else if(_u8BigCounter3_Cnt > 0)
    {
        _u8BigCounter3_Cnt --;
    }

    if( u16Unmatch_3 > 0x1000  )
    {
        _u8BigCounter3_Cnt2 = 10;
    }
    else if(_u8BigCounter3_Cnt2 > 0)
    {
        _u8BigCounter3_Cnt2 --;
    }

    // deactivate counter 1 jump out
#ifdef PRJ_MCNR  // For A3, A5,A7P,T12
    if( u16Unmatch_1 > 0xF000  )
#else
    if( u16Unmatch_1 > 0xA000  )
#endif
    {
        _u8BigCounter1_Cnt = 10;
    }
    else if(_u8BigCounter1_Cnt > 0)
    {
        _u8BigCounter1_Cnt --;
    }

    if( u16Unmatch_1 > 0xc000)
    {
        if( _u8BigCounter_Cnt3 < 80)
            _u8BigCounter_Cnt3 ++;
    }
    else
        _u8BigCounter_Cnt3 = 0;

    if( u16Unmatch_1 > 0x1000 )
        _u8BigCounter1_Cnt2 = 0x8;
    else if( _u8BigCounter1_Cnt2 != 0)
        _u8BigCounter1_Cnt2 --;

    if( u16Unmatch_1 < 0x0800 )
    {
        _u8SmallCounter1_Cnt = 10;
    }
    else if(_u8SmallCounter1_Cnt > 0)
    {
        _u8SmallCounter1_Cnt --;
    }

    if((MHal_XC_R2BYTEMSK(REG_SC_BK12_01_L, 0xFF00)==0x2500)
       || (MHal_XC_R2BYTEMSK(REG_SC_BK12_01_L, 0xFF00)==0x2700)
       || (MHal_XC_R2BYTEMSK(REG_SC_BK12_01_L, 0xFF00)==0x2400)
       || (MHal_XC_R2BYTEMSK(REG_SC_BK12_01_L, 0xFF00)==0x2600)
      )//(stXCStatus.bInterlace)
    {
        if(_u8NoCounter3_Cnt > 20)
        {
            MHal_XC_W2BYTEMSK(REG_SC_BK0A_02_L, ((MS_U16)0x01)<<8, 0x1F00);
            MHal_XC_W2BYTEMSK(REG_SC_BK0A_05_L, ((MS_U16)0x10)<<8, 0xFF00);
        }
        else
        {
            MHal_XC_W2BYTEMSK(REG_SC_BK0A_02_L, ((MS_U16)0x04)<<8, 0x1F00);  // for SD cross hatch
            MHal_XC_W2BYTEMSK(REG_SC_BK0A_05_L, ((MS_U16)0x10)<<8, 0xFF00);
        }

        // Film32 On,f2 32 film mode enable
        if( (MHal_XC_R2BYTEMSK( REG_SC_BK0A_10_L, BIT(14) ) ==  BIT(14) )
            || (MHal_XC_R2BYTEMSK( REG_SC_BK0A_10_L, BIT(15) ) ==  BIT(15) ) )
        {
            ///--> new add function for skip film mode in still pattern
            if((!MHal_XC_R2BYTEMSK(REG_SC_BK12_03_L, BIT(1)))&&(!MHal_XC_R2BYTEMSK(REG_SC_BK0A_02_L, BIT(14))))    // 2R mode can't enter this loop
            {
                u16Unmatch_3 = MHal_XC_R2BYTE(REG_SC_BK0A_0B_L);

                if( (u16Unmatch_3 > 0x80) )
                    _u8Unmatch3Cnt = 0;
                else if(_u8Unmatch3Cnt == 0xFF)
                    _u8Unmatch3Cnt = 0xFF;
                else
                    _u8Unmatch3Cnt = _u8Unmatch3Cnt + 1;

                ///--> Jump out Film for LG tower
                if( !bVerticalMovingU )
                    _u8VerticalCnt = 0;
                else if(_u8VerticalCnt == 0xFF)
                    _u8VerticalCnt = 0xFF;
                else
                    _u8VerticalCnt = _u8VerticalCnt + 1;
            }
        }
        else
        {
            // DEFAULT value of hardware and software film mode suggested by SY.Shih
            MHal_XC_W2BYTEMSK(REG_SC_BK0A_1E_L, 0, BIT(1));
            MHal_XC_W2BYTEMSK(REG_SC_BK0A_21_L, 0, BIT(1) | BIT(2));
        }

        // film-any continuous error line patch, not easy to enter film, but easy to exit film
        if ( (MS_U16)MHal_XC_R2BYTEMSK(REG_SC_BK0A_21_L, 0x0040) == 0x0040 ) // film-any status
            MHal_XC_W2BYTEMSK(REG_SC_BK0A_57_L, 0x08, 0xff);
        else
            MHal_XC_W2BYTEMSK(REG_SC_BK0A_57_L, 0x20, 0xff);


        if(_u8VerticalCnt>0x03)          // only active at Film
        {
            MHal_XC_W2BYTEMSK(REG_SC_BK0A_23_L, 0, BIT(15));
        }
        else
        {
            if((u32CurCounter3<0x9) && (u32CurCounter1<0x9))
            {
                _u8StillCounter3++;
            }
            else
            {
                _u8StillCounter3 = 0;
            }

            if(_u8StillCounter3>=8)
            {
                _u8StillCounter3 = 8;
            }

            if(u32Film22AdaptiveCounter1Th>0xFFFF)
                u32Film22AdaptiveCounter1Th = 0xFFFF;

            MHal_XC_W2BYTE(REG_SC_BK0A_0C_L, (MS_U16)u32Film22AdaptiveCounter1Th );

            if(_u8StillCounter3==8 || u16KuroPatchEn==1) //default
            {
                MHal_XC_W2BYTE(REG_SC_BK0A_0C_L, 0 );
                MHal_XC_W2BYTE(REG_SC_BK0A_0D_L, 0 );
            }

            if( bIs60Hz ) //NTSC -> turn off fast film22
                MHal_XC_W2BYTEMSK(REG_SC_BK0A_23_L, 0, BIT(15));
            else // PAL
                MHal_XC_W2BYTEMSK(REG_SC_BK0A_23_L, BIT(15), BIT(15));
        }

        // for SEC pattern gen
        if(_u8NoCounter3_Cnt > 20)
            MHal_XC_W2BYTEMSK(REG_SC_BK0A_25_L, 0x00FF, 0xFFFF); // disable film-any
        else if ( !bIsFilmXOO )
            MHal_XC_W2BYTEMSK(REG_SC_BK0A_25_L, 0x0080, 0xFFFF); // disable film-any
        else
        {
            if (bIsHDSource)
                MHal_XC_W2BYTEMSK(REG_SC_BK0A_25_L, 0x0804, 0xFFFF);
            else
                MHal_XC_W2BYTEMSK(REG_SC_BK0A_25_L, 0x0808, 0xFFFF);
        }

        // for auto 1
        // film32
        u32Counter3  = MHal_XC_R2BYTE(REG_SC_BK0A_0B_L);


        if( u32Counter3 < 0x100 )
            _u8MovingCharacterCounter = 0; //
        else if(_u8MovingCharacterCounter != 0xff )
            _u8MovingCharacterCounter ++;

        if ( bIsHDSource )
        {
            MHal_XC_W2BYTEMSK(REG_SC_BK0A_17_L, BIT(13),BIT(13) );
        }
        else
        {
            MHal_XC_W2BYTEMSK(REG_SC_BK0A_17_L, 0x00 ,BIT(13) );
            MHal_XC_W2BYTE(REG_SC_BK0A_1F_L, 0x0250);
        }

        //MHal_XC_W2BYTE(REG_SC_BK0A_16_L, 0x00D8);
        MHal_XC_W2BYTE(REG_SC_BK0A_1E_L, 0x0551);

    } // end interlace loop
    _u16RdBank_Prv = u16RdBank;
}

int MDrv_XC_Suspend(void)
{
#if defined(CONFIG_MSTAR_XC_HDR_SUPPORT)
    MHal_XC_Suspend();
#endif
    return 0;
}

int MDrv_XC_Resume(void)
{
#if defined(CONFIG_MSTAR_XC_HDR_SUPPORT)
    MHal_XC_Resume();
    MDrv_XC_Set3DLutInfo(&st3DLutInfoForSTR);
#endif
    return 0;
}

#ifdef CONFIG_MSTAR_XC_HDR_SUPPORT
EXPORT_SYMBOL(MDrv_XC_SetHDRType);
EXPORT_SYMBOL(MDrv_XC_SetInputSourceType);
EXPORT_SYMBOL(MDrv_XC_Set3DLutInfo);
EXPORT_SYMBOL(MDrv_XC_SetShareMemInfo);
EXPORT_SYMBOL(MDrv_XC_EnableHDR);
EXPORT_SYMBOL(MDrv_XC_SetHDRWindow);
EXPORT_SYMBOL(MDrv_XC_ConfigAutoDownload);
EXPORT_SYMBOL(MDrv_XC_WriteAutoDownload);
EXPORT_SYMBOL(MDrv_XC_FireAutoDownload);
EXPORT_SYMBOL(KDrv_XC_GetAutoDownloadCaps);
EXPORT_SYMBOL(MDrv_XC_SetColorFormat);
EXPORT_SYMBOL(MDrv_XC_CFDControl);
EXPORT_SYMBOL(MDrv_XC_SetDSHDRInfo);
EXPORT_SYMBOL(MDrv_XC_GetHDRType);
#endif
