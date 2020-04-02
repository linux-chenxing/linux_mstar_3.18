///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2008 - 2009 Mstar Semiconductor, Inc.
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
/// @file   mdrv_ldm_algorithm.c
/// @brief  MStar local dimming   file
/// @author MStar Semiconductor Inc.
/// @attention
/// <b><em></em></b>
///////////////////////////////////////////////////////////////////////////////////////////////////


//=============================================================================
// Include Files
//=============================================================================
#if (defined (CONFIG_HAS_LD_DMA_MODE))

#include <linux/types.h>
#include <linux/fs.h>
#include <asm/io.h>

#include "mdrv_mstypes.h"
#include "mdrv_ldm_io.h"
#include "mdrv_ldm_common.h"
#include "mdrv_ldm_init.h"
#include "mdrv_ldm_interface.h"
#include "mdrv_ldm_algorithm.h"
#include "mdrv_ldm_parse.h"
#include "mdrv_ldm_cus.h"
#include "mhal_ldm.h"
#include "reg_ldm.h"
#include "mdrv_mbx.h"

#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/delay.h>

#include <mstar/mstar_chip.h>
//#include <linux/clkm.h>
#include "mdrv_pwm.h"

//=============================================================================
// Compile options
//=============================================================================

extern void Chip_Flush_Memory(void);
extern void Chip_Flush_Cache_Range_VA_PA(unsigned long u32VAddr,unsigned long u32PAddr,unsigned long u32Size);

//=============================================================================
// Local Defines
//=============================================================================


//=============================================================================
// Macros
//=============================================================================
#define _MDRV_LDM_ALOG_C

#define LDM_LEN             0x100000        //
#define SPI_BUFF_OFFSET     0x80000        //
#define PM_MBX_TIMEOUT      300

#define GAMMA_TABLE_LEN     256

//local dimming miu read / write  use va:u64Vaddr, not pa:addr_base_L
#define MDrv_LD_MIUReadByte(addr_base_L, offset)           (*((unsigned char  volatile *)(u64Vaddr + (addr_base_L) + (offset))))
#define MDrv_LD_MIURead2Bytes(addr_base_L, offset)           (*((volatile MS_U16 *) (u64Vaddr + (addr_base_L) + (offset))))
#define MDrv_LD_MIUWriteByte(addr_base_L, offset, value)    (*((unsigned char  volatile *)(u64Vaddr + (addr_base_L) + (offset)))) = ((MS_U8)(value))
#define MDrv_LD_MIUWrite2Bytes(addr_base_L, offset, val)  (*((volatile MS_U16*)(u64Vaddr + (addr_base_L) + (offset)))) = (MS_U16)(val)

//=============================================================================
// Local Variables
//=============================================================================
static DEFINE_MUTEX(Semutex_LD);
static DEFINE_SPINLOCK(spinlock_ld);


static MS_U8 *pLDF_blocks = NULL;
static MS_U16 *pSD_blocks = NULL;
static MS_U16 *pTF_blocks = NULL;
static MS_U16 *pSF_blocks = NULL;
static MS_U8 *pLDB_blocks = NULL;
static MS_U16 *pLuma_blocks = NULL; //temp buffer
MS_U16 *pSPI_blocksBuffer0 = NULL;
MS_U16 *pSPI_blocksBuffer1 = NULL;
MS_U16 *pSPI_blocks = NULL;

MS_U8  gu8FrameIdx = 0;
extern MS_U8 u8LEDData[];


static MS_U8 *pGamma_blocks = NULL;
MS_U16 gu16LedWidth;
MS_U16 gu16LedHeight;

static MS_BOOL bLdOn = FALSE;
static struct work_struct *pstWork = NULL;
static MS_BOOL bLdReady = FALSE;

static MS_U8 gu8LedMod = 0;
static MS_U8 gu8LedID = 0;
static MS_U8 gu8LedStr = 0;
static MS_BOOL gbMarqueue = FALSE;
static MS_BOOL gbCompEn = FALSE;
static MS_U8 gu8LsfMod = 0;
static MS_U16 gTMPblocks[LD_MAX_BLOCK];
#define LD_MAX_BLOCK_SIZE 48*48*2
static U16 au16SD_Mean_blocks[LD_MAX_BLOCK_SIZE]={0};
static MS_U16 gu16LedLuma = 0;    //using to constant luma and led number
static MS_U8 gu8Compesation = 0;

//=============================================================================
// Global Variables
//=============================================================================
MS_U8 u8LdSuspend = 0;
MS_U8 u8bl_letterbox_flag = 0;

//=============================================================================
// Extern Variables
//=============================================================================
extern MS_U8 u8LdInitProtectFlag;
extern MS_PHY ld_addr_base;
extern void __iomem * u64Vaddr;
extern ST_DRV_LD_PQ_INFO* psDrvLdPQInfo;
extern MS_BOOL gbLdInit;
extern MS_U32 LDF_mem_size;


// *************************programed by initial function***********************************************//
#ifdef LD_SUPPORT_1XN_NX1
extern MS_BOOL bLed1xN;
extern MS_BOOL bLedNx1;
#endif

extern MS_BOOL MDrv_LD_GetLinearEn(void);

U8* pTbl_LD_Gamma[16] = {
    NULL, // NULL indicates linear
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

U8* pTbl_LD_Remap[16] = {
    NULL, // NULL indicates linear
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};


//=============================================================================
// Local Function Prototypes
//=============================================================================

static void MDrv_LD_FlushDRAMData(unsigned long u32VAddr,unsigned long u32PAddr,unsigned long u32Size);

//=============================================================================
// Local Function
//=============================================================================

//////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////
// Assertion : 0 <= IN_blocks[block_idx] < 0xFF00 ----> 0 <= SPI_blocks[block_idx] < 0xFFFF, 0 <= LDB_blocks[block_idx] < 0xFF
static void MDrv_LD_AlgoPostProcess(const MS_U16* IN_blocks, MS_U16* SPI_blocks, MS_U8* LDB_blocks)
{
    MS_U16 n_leds = gu16LedWidth * gu16LedHeight;
    MS_U16 block_idx = 0;
#ifdef LD_MEM_MODE
    MS_BOOL MEM_init = FALSE;
    static MS_U16 MEM_blocks[LD_MAX_BLOCK]; // for measurement
#endif
    MS_U16 backlight = 0;

    MS_U8 led_mod = MHal_LD_GetSWPulseMode();
    MS_U16 led_id = MHal_LD_GetSWPulseId();
    MS_U16 led_str = MHal_LD_GetSWPulseLEDIntensity();
    MS_U16 ldb_str = MHal_LD_GetSWPulseLDBIntensity();


    CHECK_POINTER_V(IN_blocks);
    CHECK_POINTER_V(SPI_blocks);
    CHECK_POINTER_V(LDB_blocks);

    if (gu8Compesation < 10)
    {
        // Form SPI signal & LDB data
        for (block_idx = 0; block_idx < n_leds; block_idx++)
        {
            backlight = 0xFFFF;
            SPI_blocks[block_idx] = backlight + (backlight >> 8);
            LDB_blocks[block_idx] = 0xFF;
        }
        if (gu8Compesation == 9)
        {
          MHal_LD_SetCompensationEn(psDrvLdPQInfo->bCompensationEn);
        }
        gu8Compesation++;
     }
    else
    {
        // Form SPI signal & LDB data
        for (block_idx = 0; block_idx < n_leds; block_idx++)
        {
            backlight = IN_blocks[block_idx];
            // transform (0, ..., 0xFF00) to (0, ..., 0xFFFF)
            SPI_blocks[block_idx] = backlight + (backlight >> 8);
            // rounding to 8-bit
            LDB_blocks[block_idx] = (backlight + 0x80) >> 8;
        }
     }

    if(stDrvLdMiscInfo.u8MirrorPanel == 1) //for HVmirror panel just test for Public version 0-normal panel 1-mirror panel
    {
        MS_U8 u8LDBtemp = 0;
        for (block_idx = 0; block_idx < n_leds/2; block_idx++)
        {
            u8LDBtemp  = LDB_blocks[block_idx];
            LDB_blocks[block_idx] = LDB_blocks[n_leds-block_idx-1];
            LDB_blocks[n_leds-block_idx-1] = u8LDBtemp;
        }

    }
#ifdef LD_MEM_MODE
    if (!MEM_init)
    {
        #if 1
        memset(MEM_blocks, 0x00, n_leds*sizeof(MS_U16));
        #else
        for (block_idx = 0; block_idx < n_leds; block_idx++)
            MEM_blocks[block_idx] = 0;
        #endif
        MEM_init = TRUE;
    }
#endif

    if (led_mod == 0x0)
    {
        if (led_id == 0x1) // Left: right LSF, Right: LD on
        {
            for (block_idx = 0; block_idx < n_leds; block_idx++)
            {
                if (block_idx % gu16LedWidth >= gu16LedWidth/2)
                {
                    SPI_blocks[block_idx] = (led_str << 8) | led_str;
                    LDB_blocks[block_idx] = led_str;
                }
            }
        }
        else if (led_id == 0x2) // Left: LD off, Right: LD on
        {
            for (block_idx = 0; block_idx < n_leds; block_idx++)
            {
                if (block_idx % gu16LedWidth < gu16LedWidth/2)
                {
                    SPI_blocks[block_idx] = (led_str << 8) | led_str;
                    LDB_blocks[block_idx] = led_str;
                }
            }
        }
        else if (led_id == 0x3) // Left: LD on, Right: left LSF
        {
            for (block_idx = 0; block_idx < n_leds; block_idx++)
            {
                if (block_idx % gu16LedWidth >= gu16LedWidth/2)
                {
                    SPI_blocks[block_idx] = SPI_blocks[block_idx - gu16LedWidth/2];
                    LDB_blocks[block_idx] = LDB_blocks[block_idx - gu16LedWidth/2];
                }
            }
        }
        else if (led_id == 0x4) // Left: right LSF, Right: LD on
        {
            for (block_idx = 0; block_idx < n_leds; block_idx++)
            {
                if (block_idx % gu16LedWidth < gu16LedWidth/2)
                {
                    SPI_blocks[block_idx] = SPI_blocks[block_idx + gu16LedWidth/2];
                    LDB_blocks[block_idx] = LDB_blocks[block_idx + gu16LedWidth/2];
                }
            }
        }
    }
    else if (led_mod == 0xf)     // 22[15:12]=4'b1111  real backlight
    {
        if (led_id == 0xfff)
        {
            #if 1
            memset(SPI_blocks, 0xff, n_leds*sizeof(MS_U16));
            #else
            for (block_idx = 0; block_idx < n_leds; block_idx++)
                SPI_blocks[block_idx] = 0xffff;
            #endif
        }
        else if (led_id < n_leds)
        {
            #if 1
            memset(SPI_blocks, 0x00, n_leds*sizeof(MS_U16));
            #else
            for (block_idx = 0; block_idx < n_leds; block_idx++)
                SPI_blocks[block_idx] = 0;
            #endif
            SPI_blocks[led_id] = 0xffff;
        }
        MHal_LD_SetLSFOutMode(0x1);
    }
    else if (led_mod == 0xe)      // 22[15:12]=4'b1110  digital backlight
    {
        #if 1
        memset(SPI_blocks, 0xff, n_leds*sizeof(MS_U16));
        #else
        for (block_idx = 0; block_idx < n_leds; block_idx++)
            SPI_blocks[block_idx] = 0xffff;
        #endif
        if (led_id == 0xfff)
        {
            #if 1
            memset(LDB_blocks, 0xff, n_leds);
            #else
            for (block_idx = 0; block_idx < n_leds; block_idx++)
                LDB_blocks[block_idx] = 0xff;
            #endif
        }
        else if (led_id < n_leds)
        {
            #if 1
            memset(LDB_blocks, 0x00, n_leds);
            #else
            for (block_idx = 0; block_idx < n_leds; block_idx++)
                LDB_blocks[block_idx] = 0x00;
            #endif
            LDB_blocks[led_id] = 0xff;
        }
        MHal_LD_SetLSFOutMode(0x5);
    }
    else if (led_mod == 0xd)      // 22[15:12]=4'b1101 tunable real backlight
    {
        if (led_id == 0xfff)
        {
            #if 0
            memset(SPI_blocks, ((led_str << 8) | ldb_str), n_leds*sizeof(MS_U16));
            #else
            for (block_idx = 0; block_idx < n_leds; block_idx++)
                SPI_blocks[block_idx] = (led_str << 8) | ldb_str;;
            #endif
        }
        else if (led_id < n_leds)
        {
            #if 0
            memset(SPI_blocks, 0x00, n_leds*sizeof(MS_U16));
            #else
            for (block_idx = 0; block_idx < n_leds; block_idx++)
                SPI_blocks[block_idx] = 0;
            #endif
            SPI_blocks[led_id] = (led_str << 8) | ldb_str;;
        }
    }
    else if (led_mod == 0xc)      // 22[15:12]=4'b1100 tunable digital backlight
    {
        #if 0
        MS_U16 u16Temp = (led_str << 8) | led_str;
        memset(SPI_blocks, u16Temp, n_leds*sizeof(MS_U16));
        #else
        for (block_idx = 0; block_idx < n_leds; block_idx++)
            SPI_blocks[block_idx] = (led_str << 8) | led_str;
        #endif
        if (led_id == 0xfff)
        {
            #if 0
            memset(LDB_blocks, ldb_str, n_leds);
            #else
            for (block_idx = 0; block_idx < n_leds; block_idx++)
                LDB_blocks[block_idx] = ldb_str;
            #endif
        }
        else if (led_id < n_leds)
        {
            #if 1
            memset(LDB_blocks, 0x00, n_leds);
            #else
            for (block_idx = 0; block_idx < n_leds; block_idx++)
                LDB_blocks[block_idx] = 0;
            #endif
            LDB_blocks[led_id] = ldb_str;
        }
    }
#ifdef LD_MEM_MODE
    else if (led_mod == 0xb)      // 22[15:12]=4'b1011 set SPI
    {
        if (led_id == 0xfff)
        {
            #if 0
            MS_U16 u16Temp = (led_str << 8) | ldb_str;
            memset(MEM_blocks, u16Temp, n_leds*sizeof(MS_U16));
            memset(SPI_blocks, u16Temp, n_leds*sizeof(MS_U16));
            #else
            for (block_idx = 0; block_idx < n_leds; block_idx++)
                SPI_blocks[block_idx] = MEM_blocks[block_idx] = (led_str << 8) | ldb_str;
            #endif
        }
        else if (led_id < n_leds)
        {
            #if 0
            memcpy(SPI_blocks, MEM_blocks, n_leds*sizeof(MS_U16));
            #else
            for (block_idx = 0; block_idx < n_leds; block_idx++)
                SPI_blocks[block_idx] = MEM_blocks[block_idx];
            #endif
            SPI_blocks[led_id] = MEM_blocks[led_id] = (led_str << 8) | ldb_str;;
        }
    }
    else if (led_mod == 0xa)      // 22[15:12]=4'b1010 set LDB
    {
        #if 0
        MS_U16 u16Temp = (led_str << 8) | led_str;
        memset(SPI_blocks, u16Temp, n_leds*sizeof(MS_U16));
        #else
        for (block_idx = 0; block_idx < n_leds; block_idx++)
            SPI_blocks[block_idx] = (led_str << 8) | led_str;
        #endif
        if (led_id == 0xfff)
        {
            #if 0
            memset(MEM_blocks, ldb_str, n_leds*sizeof(MS_U16));
            memset(SPI_blocks, ldb_str, n_leds*sizeof(MS_U16));
            #else
            for (block_idx = 0; block_idx < n_leds; block_idx++)
                LDB_blocks[block_idx] = MEM_blocks[block_idx] = ldb_str;
            #endif
        }
        else if (led_id < n_leds)
        {
            #if 1
            memcpy(LDB_blocks, (MS_U8 *)MEM_blocks, n_leds*sizeof(MS_U16));
            #else
            for (block_idx = 0; block_idx < n_leds; block_idx++)
                LDB_blocks[block_idx] = MEM_blocks[block_idx];
            #endif
            LDB_blocks[led_id] = MEM_blocks[led_id] = ldb_str;
        }
    }
#endif

#ifdef LD_SUPPORT_1XN_NX1
    // Map 2xN back to 1xN LED
    if (bLed1xN)
    {
        for (block_idx = 0; block_idx < n_leds; block_idx += 2)
        {
            if (SPI_blocks[block_idx] < SPI_blocks[block_idx+1])
                SPI_blocks[block_idx] = SPI_blocks[block_idx+1];
            SPI_blocks[block_idx+1] = 0;
            if (LDB_blocks[block_idx] < LDB_blocks[block_idx+1])
                LDB_blocks[block_idx] = LDB_blocks[block_idx+1];
            LDB_blocks[block_idx+1] = 0;
        }
    }
    // Map Nx2 back to Nx1 LED
    if (bLedNx1)
    {
        for (block_idx = 0; block_idx < gu16LedWidth; block_idx ++)
        {
            if (SPI_blocks[block_idx] < SPI_blocks[block_idx+gu16LedWidth])
                SPI_blocks[block_idx] = SPI_blocks[block_idx+gu16LedWidth];
            SPI_blocks[block_idx+gu16LedWidth] = 0;
            if (LDB_blocks[block_idx] < LDB_blocks[block_idx+gu16LedWidth])
                LDB_blocks[block_idx] = LDB_blocks[block_idx+gu16LedWidth];
            LDB_blocks[block_idx+gu16LedWidth] = 0;
        }
    }
#endif
}
static void MDrv_LD_AlgoDetLetterBox(MS_U16* BL_blocks)
{
    MS_U16 n_leds = gu16LedWidth * gu16LedHeight;
    MS_U16 block_idx = 0;
    MS_U8 u8up_zero_cnt = 0,u8down_zero_cnt = 0;

    CHECK_POINTER_V(BL_blocks);

    for (block_idx = 0; block_idx < gu16LedWidth; block_idx++)
    {
        if (BL_blocks[block_idx] == 0)
            u8up_zero_cnt++;
        else
            u8up_zero_cnt = 0;
    }
    for (block_idx = (gu16LedWidth * (gu16LedHeight-1)); block_idx < n_leds; block_idx++)
    {
        if (BL_blocks[block_idx] == 0)
            u8down_zero_cnt++;
        else
            u8down_zero_cnt = 0;
    }

    if((u8up_zero_cnt==gu16LedWidth)&&(u8down_zero_cnt==gu16LedWidth))
        u8bl_letterbox_flag = 1;
    else
        u8bl_letterbox_flag = 0;

}
static void MDrv_LD_AlgoApplyLetterBox(MS_U16* BL_blocks)
{
    MS_U16 n_leds = gu16LedWidth * gu16LedHeight;
    MS_U16 block_idx = 0;

    CHECK_POINTER_V(BL_blocks);
    if(u8bl_letterbox_flag==1)
    {
        for (block_idx = 0; block_idx < gu16LedWidth; block_idx++)
        {
            BL_blocks[block_idx] = 0;
        }
        for (block_idx = (gu16LedWidth * (gu16LedHeight-1)); block_idx < n_leds; block_idx++)
        {
            BL_blocks[block_idx] = 0;
        }
    }
}

static void MDrv_LD_AlgoLocalDimmingStrength(MS_U16* BL_blocks)
{
    MS_U16 n_leds = gu16LedWidth * gu16LedHeight;
    MS_U16 block_idx = 0;
    MS_U16 bl_val = 0, max_bl_val = 0;
    static MS_U16 u16InitDelayCnt = 0;

    MS_U32 ld_str = MHal_LD_GetSWLocalDimmingStrength(); // 0 ~ 0xFF
    MS_U16 min_bl_val = MHal_LD_GetSWMinClampValue();
    static MS_U32 ld_str_cnt = 0;

    CHECK_POINTER_V(BL_blocks);

    ld_str = (ld_str > 0x80) ? ld_str+1 : ld_str;
    min_bl_val <<= 8; // 0 ~ 0xFF00

    max_bl_val = 0xFF00;

    /////////init  protection/////////////////////////////////////////
    if(u8LdInitProtectFlag==1)
    {
        u16InitDelayCnt++;
        //REG_W2B((LD2_BK | (0x42)), u16InitDelayCnt);
        if(u16InitDelayCnt==0x20)
        {
            ld_str_cnt++;
            u16InitDelayCnt = 0;
        }
        if(ld_str_cnt>=ld_str)
    {
            ld_str_cnt = ld_str;
            u8LdInitProtectFlag = 0;
        }
    }
    else
    {
        ld_str_cnt = ld_str;
    }

    for (block_idx = 0; block_idx < n_leds; block_idx++)
    {
        bl_val = (BL_blocks[block_idx] * ld_str_cnt + max_bl_val * (256 - ld_str_cnt) + 0x80) >> 8; // 0 ~ 0xFF00
        if (bl_val < min_bl_val)
            bl_val = min_bl_val;
        BL_blocks[block_idx] = bl_val;
    }

}

// Assertion : 0 <= BL_blocks[block_idx] <= 0xFFFF
static void MDrv_LD_AlgoGlobalDimmingStrength(MS_U16* BL_blocks)
{
    static MS_U8 ReEntrance = 0;
    MS_U16 block_idx = 0;
    MS_U16 n_leds = gu16LedWidth * gu16LedHeight;
    MS_U32 gd_str = MHal_LD_GetSWGlobalDimmingStrength(); // 0 ~ 0xFF

    if(1 == ReEntrance)
    {
        LD_ERROR("warning!  resend! \n");
        return;
    }

    CHECK_POINTER_V(BL_blocks);

    for (block_idx = 0; block_idx < n_leds; block_idx++)
    {
        BL_blocks[block_idx] = (BL_blocks[block_idx] * gd_str) / 255;
    }

    if(FALSE == bLdOn)
        return;

    ReEntrance = 1;
    n_leds = MDrv_LD_ConvertLumaData2SPIData(BL_blocks, n_leds);

    MDrv_LD_FlushDRAMData((unsigned long)pSPI_blocks,(unsigned long)(ld_addr_base+((void*)pSPI_blocks-u64Vaddr)), n_leds);

    ReEntrance = 0;

}

// Assertion : 0 <= LDF_blocks[block_idx] <= 0xFF ----> 0 <= SD_blocks[block_idx] <= 0xFF00
static void MDrv_LD_AlgoScalingDown(const MS_U8* LDF_blocks, MS_U16* SD_blocks)
{
    MS_U16 ldf_width = MHal_LD_GetLDFBacklightWidth();
    MS_U16 ldf_height = MHal_LD_GetLDFBacklightHeight();
    MS_U16 n_leds = gu16LedWidth * gu16LedHeight;
    MS_U16 hsd_factor = ldf_width / gu16LedWidth;
    MS_U16 vsd_factor = ldf_height / gu16LedHeight;
    MS_U16 LDF_idx = 0, SD_idx = 0, i = 0, j = 0;

    /*
       * modify MS_U16 SortMax_blocks[LD_MAX_BW*LD_MAX_BH];
       *            --> static MS_U16 SortMax_blocks[LD_MAX_BW*LD_MAX_BH]; because:
       * 1. compile warning:warning: the frame size of 2064 bytes is larger than 1536 bytes [-Wframe-larger-than=],
       *     and it's coredump when run:
       */
    //MS_U16 SortMax_blocks[LD_MAX_BW*LD_MAX_BH];
    static MS_U16 SortMax_blocks[LD_MAX_BW*LD_MAX_BH];
    MS_U16 m = 0, n = 0, k = 0;
    MS_U16 u16Max_nth = MHal_LD_GetSWMaxThreshold();
    MS_U32 alpha = MHal_LD_GetLDFDCMaxAlpha();
    MS_U32 block_cur = 0, block_mean = 0, block_max = 0; // 0 ~ 0xFF00

    u16Max_nth = MIN(u16Max_nth, hsd_factor*vsd_factor - 1);

    CHECK_POINTER_V(LDF_blocks);
    CHECK_POINTER_V(SD_blocks);

    #if 1
    memset(SD_blocks, 0x00, n_leds*sizeof(MS_U16));
    #else
    for (SD_idx = 0; SD_idx < n_leds; SD_idx++)
        SD_blocks[SD_idx] = 0;
    #endif

    if (MHal_LD_GetLDFWriteDCMaxOFEn()) // 4 bytes : OF[15:8] OF[7:0] Max DC
    {
        #if 0//def LD_SUPPORT_1XN_NX1
        if (bLed1xN)
        {
            gu16LedWidth = 1;
            hsd_factor = ldf_width;
        }
        if (bLedNx1)
        {
            gu16LedHeight = 1;
            vsd_factor = ldf_height;
        }
        #endif

        for (n = 0; n < gu16LedHeight; n++)
        {
            for (m = 0; m < gu16LedWidth; m++)
            {
                SD_idx = m + n*gu16LedWidth;

                for (k = 0; k <= u16Max_nth; k++)
                    SortMax_blocks[k] = 0;

                block_cur = 0;
                for (j = 0; j < vsd_factor; j++)
                {
                    for (i = 0; i < hsd_factor; i++)
                    {
                        LDF_idx = (i + m*hsd_factor) + (j + n*vsd_factor)*ldf_width;

                        block_mean = LDF_blocks[LDF_idx*4];
                        block_max = LDF_blocks[LDF_idx*4 + 1];

                        block_cur += block_mean;

                        if (block_max > SortMax_blocks[u16Max_nth])
                        {
                            for (k = u16Max_nth+1; k > 0; k--)
                            {
                                if (block_max <= SortMax_blocks[k-1])
                                    break;
                                SortMax_blocks[k] = SortMax_blocks[k-1];
                            }
                            SortMax_blocks[k] = block_max;
                        }
                    }
                }
                block_mean = (block_cur << 8) / (hsd_factor * vsd_factor);
                block_max = SortMax_blocks[u16Max_nth] << 8;
                SD_blocks[SD_idx] = (alpha * block_max + (8-alpha) * block_mean + (0x1 << 2)) >> 3;
                au16SD_Mean_blocks[SD_idx]=block_mean;
            }
        }

        #if 0//def LD_SUPPORT_1XN_NX1
        if (bLed1xN)
        {
            for (n = gu16LedHeight; n > 0; n--)
                SD_blocks[n*2 - 2] = SD_blocks[n*2 - 1] = SD_blocks[n - 1];
        }
        if (bLedNx1)
        {
            for (m = 0; m < gu16LedWidth; m++)
                SD_blocks[m + gu16LedWidth] = SD_blocks[m];
        }
        #endif
    }
    else if (MHal_LD_GetLDFWriteDCMaxEn()) // 2 bytes : Max DC
    {
        for (j = 0; j < ldf_height; j++)
        {
            for (i = 0; i < ldf_width; i++)
            {
                LDF_idx = i + j*ldf_width;

                block_mean = LDF_blocks[LDF_idx*2];
                block_mean <<= 8;
                block_max = LDF_blocks[LDF_idx*2 + 1];
                block_max <<= 8;
                block_cur = (alpha * block_max + (8-alpha) * block_mean + (0x1 << 2)) >> 3;

                SD_idx = (i / hsd_factor) + (j / vsd_factor)*gu16LedWidth;
                if (block_cur > SD_blocks[SD_idx])
                    SD_blocks[SD_idx] = block_cur;
                au16SD_Mean_blocks[SD_idx]=block_mean;
            }
        }
    }
    else // 1 bytes : BLEND
    {
        for (j = 0; j < ldf_height; j++)
        {
            for (i = 0; i < ldf_width; i++)
            {
                LDF_idx = i + j*ldf_width;
                block_cur = LDF_blocks[LDF_idx];
                block_cur <<= 8;

                SD_idx = (i / hsd_factor) + (j / vsd_factor)*gu16LedWidth;
                if (block_cur > SD_blocks[SD_idx])
                    SD_blocks[SD_idx] = block_cur;
            }
        }
    }

#ifdef LD_HISTOGRAM
    MS_U16 bin_idx = 0;
    for (bin_idx = 0; bin_idx < 32; bin_idx++)
        HISTOGRAM_bins[bin_idx] = 0;
    for (block_idx = 0; block_idx < n_leds; block_idx++)
        HISTOGRAM_bins[SD_blocks[block_idx] >> 12] ++;
#endif
}

static void MDrv_LD_AlgoBacklightGamma(MS_U16* BL_blocks, const MS_U8* Table)
{
    MS_U16 n_leds = gu16LedWidth * gu16LedHeight;
    MS_U16 block_idx = 0;
    MS_U16 bl_msb = 0, bl_lsb = 0; // 0 ~ 0xFF
    MS_U32 bl_low = 0, bl_high = 0; // 0 ~ 0xFF00

    CHECK_POINTER_V(BL_blocks);
    //CHECK_POINTER_V(Table)   //maybe null

    if(NULL == Table)
    {
        return; //do nothing
    }

    for (block_idx = 0; block_idx < n_leds; block_idx++)
    {
        bl_msb = BL_blocks[block_idx] >> 8;
        bl_lsb = BL_blocks[block_idx] & 0xFF;
        bl_low = Table[bl_msb];
        bl_low <<= 8;
        bl_high = Table[(bl_msb == 0xFF) ? bl_msb : bl_msb + 1];
        bl_high <<= 8;
        BL_blocks[block_idx] = ((256-bl_lsb)*bl_low + bl_lsb*bl_high + 0x80) >> 8;
    }
}

// Assertion : 0 <= IN_blocks[block_idx] <= 0xFF00 ----> 0 <= OUT_blocks[block_idx] <= 0xFF00
static void MDrv_LD_AlgoTemporalFilter(const MS_U16* IN_blocks, MS_U16* OUT_blocks)
{
    static MS_U16 luma_pre = 0;
    static MS_U16 PRE_blocks[LD_MAX_BLOCK];

    MS_U16 n_leds = gu16LedWidth * gu16LedHeight;
    MS_U16 block_idx = 0;
    MS_U32 tf_strength_dn = MHal_LD_GetSWTemporalFilterStrengthDn(); // 0 ~ 0xFF
    MS_U32 tf_strength_up = MHal_LD_GetSWTemporalFilterStrengthUp(); // 0 ~ 0xFF
    MS_U32 luma_low = MHal_LD_GetSWTemporalFilterLowTh(); // 0 ~ 0xFF
    MS_U32 luma_high = MHal_LD_GetSWTemporalFilterHighTh(); // 0 ~ 0xFF
    MS_U32 alpha = 0, alpha_up = 0, alpha_dn = 0; // 0 ~ 0xFF
    MS_U16 max_speed = MHal_LD_GetTempFilterMaxSpeed();
    MS_U16 block_cur = 0, block_pre = 0;

    // get frame average luma
    MS_U32 luma_cur = 0, luma_diff = 0;
    MS_U32 luma_sum = 0;

    CHECK_POINTER_V(IN_blocks);
    CHECK_POINTER_V(OUT_blocks);

   for (block_idx = 0; block_idx < n_leds; block_idx++)
        luma_sum += IN_blocks[block_idx];
    luma_cur = luma_sum / n_leds; // 0 ~ 0xFF00
    luma_diff = (luma_cur > luma_pre) ? luma_cur - luma_pre : luma_pre - luma_cur;
    luma_pre = luma_cur; // 0 ~ 0xFF00

    // get blending alpha based on luma diff
    luma_low <<= 8;
    luma_high <<= 8;
    if (luma_diff >= luma_high)
    {
        alpha_up = 0;
        alpha_dn = 0;
    }
    else if (luma_diff <= luma_low)
    {
        alpha_up = tf_strength_up;
        alpha_dn = tf_strength_dn;
    }
    else
    {
        alpha_up = tf_strength_up * (luma_high - luma_diff) / (luma_high - luma_low);
        alpha_dn = tf_strength_dn * (luma_high - luma_diff) / (luma_high - luma_low);
    }

    max_speed <<= 8;
    for (block_idx = 0; block_idx < n_leds; block_idx++)
    {
        block_cur = IN_blocks[block_idx]; // 0 ~ 0xFF00
        block_pre = PRE_blocks[block_idx]; // 0 ~ 0xFF00

        // 16-bit IIR
        alpha =  (block_cur > block_pre) ? alpha_up : alpha_dn;
        block_cur = (alpha*block_pre + (256-alpha)*block_cur + 0x80) >> 8;
        if (block_cur == block_pre)
        {
            if (block_cur < IN_blocks[block_idx])
                block_cur ++;
            else if (block_cur > IN_blocks[block_idx])
                block_cur --;
        }

        // limit the max speed
        if (max_speed > 0)
        {
            if (block_cur > block_pre && block_cur - block_pre > max_speed)
                block_cur = block_pre + max_speed;
            else if (block_cur < block_pre && block_pre - block_cur > max_speed)
                block_cur = block_pre - max_speed;
        }

        OUT_blocks[block_idx] = block_cur;
        PRE_blocks[block_idx] = block_cur;
    }
}

// Assertion : 0 <= IN_blocks[block_idx] <= 0xFF00 ----> 0 <= OUT_blocks[block_idx] <= 0xFF00
static void MDrv_LD_AlgoSpatialFilter(const MS_U16* IN_blocks, MS_U16* OUT_blocks)
{
    MS_U16 block_idx = 0, neighbor_idx = 0;
    MS_U32 i = 0, j = 0, k = 0, p = 0, q = 0;
    MS_U16 sf_str[5];
    MS_U16 n_leds = gu16LedWidth * gu16LedHeight;
    EN_LD_TYPE led_type = (EN_LD_TYPE)MHal_LD_GetLEDType();

    CHECK_POINTER_V(IN_blocks);
    CHECK_POINTER_V(OUT_blocks);

    for (block_idx = 0; block_idx < n_leds; block_idx++)
        OUT_blocks[block_idx] = IN_blocks[block_idx];
    sf_str[0] = MHal_LD_GetSWSpatialFilterStrength();
    sf_str[1] = MHal_LD_GetSWSpatialFilterStrength2();
    sf_str[2] = MHal_LD_GetSWSpatialFilterStrength3();
    sf_str[3] = MHal_LD_GetSWSpatialFilterStrength4();
    sf_str[4] = MHal_LD_GetSWSpatialFilterStrength5();

    //if (MHal_LD_GetLinearEn()) // linear style
    if(MDrv_LD_GetLinearEn())
    {
        MS_U32 block_sum = 0;
        if (1) // anti-dumbness
        {
            // Make sure (sf_str[0] + 2*sf_str[1] + 2*sf_str[2] + 2*sf_str[3] + 2*sf_str[4] == 256)
            MS_U16 str_left = 256;
            str_left -= sf_str[0];
            if ((str_left >> 1) < sf_str[1])
                sf_str[1] = str_left >> 1;
            str_left -= (sf_str[1] << 1);
            if ((str_left >> 1) < sf_str[2])
                sf_str[2] = str_left >> 1;
            str_left -= (sf_str[2] << 1);
            if ((str_left >> 1) < sf_str[3])
                sf_str[3] = str_left >> 1;
            str_left -= (sf_str[3] << 1);
            if ((str_left >> 1) < sf_str[4])
                sf_str[4] = str_left >> 1;
            str_left -= (sf_str[4] << 1);
            sf_str[0] += str_left;
        }

        if (led_type == E_LD_LED_EDGE_TB_TYPE)
        {
            MS_U8 nk = (sf_str[4] > 0) ? 5 : (sf_str[3] > 0) ? 4 : (sf_str[2] > 0) ? 3 : (sf_str[1] > 0) ? 2 : 1;
            for (j = 0; j < 2; j++) // led_height == 2
            {
                for (i = 0; i < gu16LedWidth; i++)
                {
                    block_idx = i + gu16LedWidth*j;
                    block_sum = IN_blocks[block_idx] * sf_str[0];
                    for (k = 1; k < nk; k++)
                    {
                        p = MAX(i - k, 0); // left
                        neighbor_idx = p + gu16LedWidth*j;
                        block_sum += (MS_U32)IN_blocks[neighbor_idx] * sf_str[k];

                        p = MIN(i + k, gu16LedWidth-1); // right
                        neighbor_idx = p + gu16LedWidth*j;
                        block_sum += (MS_U32)IN_blocks[neighbor_idx] * sf_str[k];
                    }
                    OUT_blocks[block_idx] = MAX(IN_blocks[block_idx], (block_sum + 0x80) >> 8);
                }
            }
        }
        else if (led_type == E_LD_LED_EDGE_LR_TYPE)
        {
            MS_U8 nk = (sf_str[4] > 0) ? 5 : (sf_str[3] > 0) ? 4 : (sf_str[2] > 0) ? 3 : (sf_str[1] > 0) ? 2 : 1;
            for (i = 0; i < 2; i++) // led_width == 2
            {
                for (j = 0; j < gu16LedHeight; j++)
                {
                    block_idx = i + gu16LedWidth*j;
                    block_sum = IN_blocks[block_idx] * sf_str[0];
                    for (k = 1; k < nk; k++)
                    {
                        q = MAX(j - k, 0); // up
                        neighbor_idx = i + gu16LedWidth*q;
                        block_sum += (MS_U32)IN_blocks[neighbor_idx] * sf_str[k];

                        q = MIN(j + k, gu16LedHeight-1); // down
                        neighbor_idx = i + gu16LedWidth*q;
                        block_sum += (MS_U32)IN_blocks[neighbor_idx] * sf_str[k];
                    }
                    OUT_blocks[block_idx] = MAX(IN_blocks[block_idx], (block_sum + 0x80) >> 8);
                }
            }
        }
        else// if (led_type == E_LD_LED_DIRECT_TYPE)
        {
            //MS_U16 TMP_blocks[LD_MAX_BLOCK];    //kernel frame size is 1536 bytes, changing to global variable
            MS_U8 nk = (sf_str[4] > 0) ? 5 : (sf_str[3] > 0) ? 4 : (sf_str[2] > 0) ? 3 : (sf_str[1] > 0) ? 2 : 1;

            // horizontal filtering
            for (j = 0; j < gu16LedHeight; j++)
            {
                for (i = 0; i < gu16LedWidth; i++)
                {
                    block_idx = i + gu16LedWidth*j;
                    block_sum = IN_blocks[block_idx] * sf_str[0];
                    for (k = 1; k < nk; k++)
                    {
                        p = MAX(i - k, 0); // left
                        neighbor_idx = p + gu16LedWidth*j;
                        block_sum += (MS_U32)IN_blocks[neighbor_idx] * sf_str[k];

                        p = MIN(i + k, gu16LedWidth-1); // right
                        neighbor_idx = p + gu16LedWidth*j;
                        block_sum += (MS_U32)IN_blocks[neighbor_idx] * sf_str[k];
                    }
                    gTMPblocks[block_idx] = MAX(IN_blocks[block_idx], (block_sum + 0x80) >> 8);
                }
            }

            // vertical filtering
            for (i = 0; i < gu16LedWidth; i++)
            {
                for (j = 0; j < gu16LedHeight; j++)
                {
                    block_idx = i + gu16LedWidth*j;
                    block_sum = gTMPblocks[block_idx] * sf_str[0];
                    for (k = 1; k < nk; k++)
                    {
                        q = MAX(j - k, 0); // up
                        neighbor_idx = i + gu16LedWidth*q;
                        block_sum += (MS_U32)gTMPblocks[neighbor_idx] * sf_str[k];

                        q = MIN(j + k, gu16LedHeight-1); // down
                        neighbor_idx = i + gu16LedWidth*q;
                        block_sum += (MS_U32)gTMPblocks[neighbor_idx] * sf_str[k];
                    }
                    OUT_blocks[block_idx] = MAX(gTMPblocks[block_idx], (block_sum + 0x80) >> 8);
                }
            }
        }
    }
    else // nonlinear style
    {
        // direct type
        const MS_S32 N1_direct = 8; // dist = 1.0
        const MS_S32 xoffset1_direct[8]  = { -1,  0,  1,  0, -1,  1,  1, -1 };
        const MS_S32 yoffset1_direct[8]  = {  0, -1,  0,  1, -1, -1,  1,  1 };
        const MS_S32 N2_direct = 16; // dist = 2.0
        const MS_S32 xoffset2_direct[16] = { -2,  0,  2,  0, -2, -1,  1,  2,  2,  1, -1, -2, -2,  2,  2, -2 };
        const MS_S32 yoffset2_direct[16] = {  0, -2,  0,  2, -1, -2, -2, -1,  1,  2,  2,  1, -2, -2,  2,  2 };
        const MS_S32 N3_direct = 24; // dist = 3.0
        const MS_S32 xoffset3_direct[24] = { -3,  0,  3,  0, -3, -1,  1,  3,  3,  1, -1, -3, -3, -2,  2,  3,  3,  2, -2, -3, -3,  3,  3, -3 };
        const MS_S32 yoffset3_direct[24] = {  0, -3,  0,  3, -1, -3, -3, -1,  1,  3,  3,  1, -2, -3, -3, -2,  2,  3,  3,  2, -3, -3,  3,  3 };
        const MS_S32 N4_direct = 32; // dist = 4.0
        const MS_S32 xoffset4_direct[32]  = { -4,  0,  4,  0, -4, -1,  1,  4,  4,  1, -1, -4, -4, -2,  2,  4,  4,  2, -2, -4, -4, -3,  3,  4,  4,  3, -3, -4, -4,  4,  4, -4 };
        const MS_S32 yoffset4_direct[32]  = {  0, -4,  0,  4, -1, -4, -4, -1,  1,  4,  4,  1, -2, -4, -4, -2,  2,  4,  4,  2, -3, -4, -4, -3,  3,  4,  4,  3, -4, -4,  4,  4 };
        const MS_S32 N5_direct = 40; // dist = 5.0
        const MS_S32 xoffset5_direct[40]  = { -5,  0,  5,  0, -5, -1,  1,  5,  5,  1, -1, -5, -5, -2,  2,  5,  5,  2, -2, -5, -5, -3,  3,  5,  5,  3, -3, -5, -5, -4,  4,  5,  5,  4, -4, -5, -5,  5,  5, -5 };
        const MS_S32 yoffset5_direct[40]  = {  0, -5,  0,  5, -1, -5, -5, -1,  1,  5,  5,  1, -2, -5, -5, -2,  2,  5,  5,  2, -3, -5, -5, -3,  3,  5,  5,  3, -4, -5, -5, -4,  4,  5,  5,  4, -5, -5,  5,  5 };

        // left-right type
        const MS_S32 N1_LR = 2; // dist = 1.0
        const MS_S32 xoffset1_LR[2] = {  0,  0 };
        const MS_S32 yoffset1_LR[2] = {  1, -1 };
        const MS_S32 N2_LR = 2; // dist = 2.0
        const MS_S32 xoffset2_LR[2] = {  0,  0 };
        const MS_S32 yoffset2_LR[2] = {  2, -2 };
        const MS_S32 N3_LR = 2; // dist = 3.0
        const MS_S32 xoffset3_LR[2] = {  0,  0 };
        const MS_S32 yoffset3_LR[2] = {  3, -3 };
        const MS_S32 N4_LR = 2; // dist = 4.0
        const MS_S32 xoffset4_LR[2] = {  0,  0 };
        const MS_S32 yoffset4_LR[2] = {  4, -4 };
        const MS_S32 N5_LR = 2; // dist = 5.0
        const MS_S32 xoffset5_LR[2] = {  0,  0 };
        const MS_S32 yoffset5_LR[2] = {  5, -5 };

        // top-bottom type
        const MS_S32 N1_TB = 2; // dist = 1.0
        const MS_S32 xoffset1_TB[2] = {  1, -1 };
        const MS_S32 yoffset1_TB[2] = {  0,  0 };
        const MS_S32 N2_TB = 2; // dist = 2.0
        const MS_S32 xoffset2_TB[2] = {  2, -2 };
        const MS_S32 yoffset2_TB[2] = {  0,  0 };
        const MS_S32 N3_TB = 2; // dist = 3.0
        const MS_S32 xoffset3_TB[2] = {  3, -3 };
        const MS_S32 yoffset3_TB[2] = {  0,  0 };
        const MS_S32 N4_TB = 2; // dist = 4.0
        const MS_S32 xoffset4_TB[2] = {  4, -4 };
        const MS_S32 yoffset4_TB[2] = {  0,  0 };
        const MS_S32 N5_TB = 2; // dist = 5.0
        const MS_S32 xoffset5_TB[2] = {  5, -5 };
        const MS_S32 yoffset5_TB[2] = {  0,  0 };

        MS_S32 N1 = 0, N2 = 0, N3 = 0, N4 = 0, N5 = 0;
        const MS_S32 *xoffset1 = NULL, *xoffset2 = NULL, *xoffset3 = NULL, *xoffset4 = NULL, *xoffset5 = NULL;
        const MS_S32 *yoffset1 = NULL, *yoffset2 = NULL, *yoffset3 = NULL, *yoffset4 = NULL, *yoffset5 = NULL;
        if (led_type == E_LD_LED_EDGE_TB_TYPE)
        {
           N1 = N1_TB; N2 = N2_TB; N3 = N3_TB;
            xoffset1 = xoffset1_TB; xoffset2 = xoffset2_TB; xoffset3 = xoffset3_TB;
            yoffset1 = yoffset1_TB; yoffset2 = yoffset2_TB; yoffset3 = yoffset3_TB;
            N4 = N4_TB; N5 = N5_TB;
            xoffset4 = xoffset4_TB; xoffset5 = xoffset5_TB;
            yoffset4 = yoffset4_TB; yoffset5 = yoffset5_TB;
        }
        else if (led_type == E_LD_LED_EDGE_LR_TYPE)
        {
            N1 = N1_LR; N2 = N2_LR; N3 = N3_LR;
            xoffset1 = xoffset1_LR; xoffset2 = xoffset2_LR; xoffset3 = xoffset3_LR;
            yoffset1 = yoffset1_LR; yoffset2 = yoffset2_LR; yoffset3 = yoffset3_LR;
            N4 = N4_LR; N5 = N5_LR;
            xoffset4 = xoffset4_LR; xoffset5 = xoffset5_LR;
            yoffset4 = yoffset4_LR; yoffset5 = yoffset5_LR;
        }
        else// if (led_type == E_LD_LED_DIRECT_TYPE)
        {
            N1 = N1_direct; N2 = N2_direct; N3 = N3_direct;
            xoffset1 = xoffset1_direct; xoffset2 = xoffset2_direct; xoffset3 = xoffset3_direct;
            yoffset1 = yoffset1_direct; yoffset2 = yoffset2_direct; yoffset3 = yoffset3_direct;
            N4 = N4_direct; N5 = N5_direct;
            xoffset4 = xoffset4_direct; xoffset5 = xoffset5_direct;
            yoffset4 = yoffset4_direct; yoffset5 = yoffset5_direct;
        }

        for (k = 0; k < 5; k++)
            sf_str[k] <<= 8; // 0 ~ 0xFF00

        for (j = 0; j < gu16LedHeight; j++)
        {
            for (i = 0; i < gu16LedWidth; i++)
            {
                block_idx = i + gu16LedWidth*j;

                // 3x3 non-linear filter
                if (sf_str[0] < 0xFF00)
                {
                    for (k = 0; k < N1; k++)
                    {
                        q = j + yoffset1[k];
                        if (q < 0 || q >= gu16LedHeight) continue;
                        p = i + xoffset1[k];
                        if (p < 0 || p >= gu16LedWidth) continue;

                        neighbor_idx = p + gu16LedWidth*q;
                        if (sf_str[0] + OUT_blocks[block_idx] < IN_blocks[neighbor_idx])
                            OUT_blocks[block_idx] = IN_blocks[neighbor_idx] - sf_str[0];
                    }
                }

                // 5x5 non-linear filter
                if (sf_str[1] < 0xFF00)
                {
                    for (k = 0; k < N2; k++)
                    {
                        q = j + yoffset2[k];
                        if (q < 0 || q >= gu16LedHeight) continue;
                        p = i + xoffset2[k];
                        if (p < 0 || p >= gu16LedWidth) continue;

                        neighbor_idx = p + gu16LedWidth*q;
                        if (sf_str[1] + OUT_blocks[block_idx] < IN_blocks[neighbor_idx])
                            OUT_blocks[block_idx] = IN_blocks[neighbor_idx] - sf_str[1];
                    }
                }

                // 7x7 non-linear filter
                if (sf_str[2] < 0xFF00)
                {
                    for (k = 0; k < N3; k++)
                    {
                        q = j + yoffset3[k];
                        if (q < 0 || q >= gu16LedHeight) continue;
                        p = i + xoffset3[k];
                        if (p < 0 || p >= gu16LedWidth) continue;

                        neighbor_idx = p + gu16LedWidth*q;
                        if (sf_str[2] + OUT_blocks[block_idx] < IN_blocks[neighbor_idx])
                            OUT_blocks[block_idx] = IN_blocks[neighbor_idx] - sf_str[2];
                    }
                }

                // 9x9 non-linear filter
                if (sf_str[3] < 0xFF00)
                {
                    for (k = 0; k < N4; k++)
                    {
                        q = j + yoffset4[k];
                        if (q < 0 || q >= gu16LedHeight) continue;
                        p = i + xoffset4[k];
                        if (p < 0 || p >= gu16LedWidth) continue;

                        neighbor_idx = p + gu16LedWidth*q;
                        if (sf_str[3] + OUT_blocks[block_idx] < IN_blocks[neighbor_idx])
                            OUT_blocks[block_idx] = IN_blocks[neighbor_idx] - sf_str[3];
                    }
                }

                // 11x11 non-linear filter
                if (sf_str[4] < 0xFF00)
                {
                    for (k = 0; k < N5; k++)
                    {
                        q = j + yoffset5[k];
                        if (q < 0 || q >= gu16LedHeight) continue;
                        p = i + xoffset5[k];
                        if (p < 0 || p >= gu16LedWidth) continue;

                        neighbor_idx = p + gu16LedWidth*q;
                        if (sf_str[4] + OUT_blocks[block_idx] < IN_blocks[neighbor_idx])
                            OUT_blocks[block_idx] = IN_blocks[neighbor_idx] - sf_str[4];
                    }
                }
           }
        }
    }
}

static void MDrv_LD_LoadLDFFrameStatistics(MS_U8* LDF_blocks)
{
    MS_U16 ldf_width = MHal_LD_GetLDFBacklightWidth();
    MS_U16 ldf_height = MHal_LD_GetLDFBacklightHeight();
    MS_PHY addr_base_L = MHal_LD_GetLDFFrameBufBaseAddr_L0();
    MS_PHY addr_base_R = 0;
    MS_U32 addr_offset_R = 0;
    MS_U32 addr_offset_L = 0;
    MS_U16 pack_length = 0;
    MS_U16 i = 0, j = 0;
    if(stDrvLdMiscInfo.u8ClkHz == 120)
    {
    addr_base_R = MHal_LD_GetLDFFrameBufBaseAddr_R0();
    addr_offset_R = 0;
    }

    gu8FrameIdx = MHal_LD_GetCurFrameIdx();

    //sw should write the spibuffer1 when frameindex = 0 and hw dma will read the spibuffer0 and send it
    pSPI_blocks = pSPI_blocksBuffer1;

    CHECK_POINTER_V(LDF_blocks);

#ifdef LD_HW_PINGPONG_MODE
    if (MHal_LD_GetCurFrameIdx() == 1)
    {
        addr_base_L = MHal_LD_GetLDFFrameBufBaseAddr_L1();
        if(stDrvLdMiscInfo.u8ClkHz == 120)
        {
        addr_base_R = MHal_LD_GetLDFFrameBufBaseAddr_R1();
        }
        pSPI_blocks = pSPI_blocksBuffer0;
    }
#endif

    if(!addr_base_L)
    {
        LD_ERROR("error!  addr_base_L null \n");
        return;
    }

    if(stDrvLdMiscInfo.u8ClkHz == 120)
    {
    ldf_width /= 2; // left/right 2 engine
    }

    if (MHal_LD_GetLDFWriteDCMaxOFEn()) // 4 bytes : OF[15:8] OF[7:0] Max DC
    {
        ldf_width *= 4;
    }
    else if (MHal_LD_GetLDFWriteDCMaxEn()) // 2 bytes : Max DC
    {
        ldf_width *= 2;
    }
    //else // 1 bytes : BLEND
    //    ldf_width *= 1;

    if (ldf_width > 0)
        pack_length = (((ldf_width - 1) / MHAL_LD_PACKLENGTH) + 1) * MHAL_LD_PACKLENGTH;

    //    1 2 3 4 5 6
    //    7 8 9 a b c
    //    d e f g h i
    //    j k l m n o
    for (j = 0; j < ldf_height; j++)
    {
        addr_offset_L = j * pack_length;
        for (i = 0; i < ldf_width; i++)
        {
            *(LDF_blocks++) = MDrv_LD_MIUReadByte(addr_base_L-ld_addr_base, addr_offset_L++);
        }
        if(stDrvLdMiscInfo.u8ClkHz == 120)
        {
          addr_offset_R = j * pack_length;
          for (i = 0; i < ldf_width; i++)
          {
            *(LDF_blocks++) = MDrv_LD_MIUReadByte(addr_base_R-ld_addr_base, addr_offset_R++);
          }
        }
    }
    MDrv_LD_FlushDRAMData((unsigned long)(u64Vaddr+addr_base_L-ld_addr_base), addr_base_L,LDF_mem_size);
    if(stDrvLdMiscInfo.u8ClkHz == 120)
    {
      MDrv_LD_FlushDRAMData((unsigned long)(u64Vaddr+addr_base_R-ld_addr_base), addr_base_R,LDF_mem_size);
    }
}

static void MDrv_LD_OutputLDBFrameBacklight(const MS_U8* LDB_blocks)
{
    MS_PHY addr_base_L = MHal_LD_GetLDBFrameBufBaseAddr_L0();
    MS_PHY addr_base_R = 0 ;
    MS_U16 led_half_width = (gu16LedWidth + 1) >> 1;
    MS_U16 led_half_height = (gu16LedHeight + 1) >> 1;
    MS_U16 block_idx = 0;

    MS_U16 ldb_width = MHal_LD_GetLDBBacklightWidth();
    MS_U32 addr_offset = 0;
    MS_U16 pack_length = 0;
    MS_U16 i = 0, j = 0;
    if(stDrvLdMiscInfo.u8ClkHz == 120)
    {
     addr_base_R = MHal_LD_GetLDBFrameBufBaseAddr_R0();
    }

#ifdef LD_HW_PINGPONG_MODE
    if (MHal_LD_GetCurFrameIdx() == 0)
    {
        addr_base_L = MHal_LD_GetLDBFrameBufBaseAddr_L1();
        if(stDrvLdMiscInfo.u8ClkHz == 120)
        {
        addr_base_R = MHal_LD_GetLDBFrameBufBaseAddr_R1();
        }
    }
#endif

    CHECK_POINTER_V(LDB_blocks);

    if(!addr_base_L)
    {
        LD_ERROR("error!  addr_base_L null \n");
        return;
    }

    if (gu16LedWidth > 0)
        pack_length = (((ldb_width - 1) / MHAL_LD_PACKLENGTH) + 1) * MHAL_LD_PACKLENGTH;

    if (MHal_LD_GetEdge2DLocalTypeEn())
    {
        pack_length = MHAL_LD_PACKLENGTH * 2;
        for (j = 0; j < gu16LedHeight; j++)
        {
            addr_offset = j*pack_length;
            for (i = 0; i < ldb_width; i++)
            {
                MDrv_LD_MIUWriteByte(addr_base_L-ld_addr_base, addr_offset, *LDB_blocks);
                if(stDrvLdMiscInfo.u8ClkHz == 120)
                MDrv_LD_MIUWriteByte(addr_base_R-ld_addr_base, addr_offset, *LDB_blocks);

                addr_offset++;
                LDB_blocks++;
            }
        }
    }
    else if (MHal_LD_GetEdge2DEn())
    {
        block_idx = 0;
        addr_offset = 0;

        // top-left quater
        for (j = 0; j < led_half_height; j++)
        {
            for (i = 0; i < led_half_width; i++)
            {
                MDrv_LD_MIUWriteByte(addr_base_L-ld_addr_base, addr_offset, LDB_blocks[i + j*gu16LedWidth]);
                if(stDrvLdMiscInfo.u8ClkHz == 120)
                MDrv_LD_MIUWriteByte(addr_base_R-ld_addr_base, addr_offset, LDB_blocks[i + j*gu16LedWidth]);

                addr_offset ++;
                if (block_idx % ldb_width == ldb_width - 1)
                    addr_offset += pack_length - ldb_width;
                block_idx ++;
            }
        }

        // top-left quater
        for (j = 0; j < led_half_height; j++)
        {
            for (i = led_half_width; i < gu16LedWidth; i++)
            {
                MDrv_LD_MIUWriteByte(addr_base_L-ld_addr_base, addr_offset, LDB_blocks[i + j*gu16LedWidth]);
                if(stDrvLdMiscInfo.u8ClkHz == 120)
                MDrv_LD_MIUWriteByte(addr_base_R-ld_addr_base, addr_offset, LDB_blocks[i + j*gu16LedWidth]);

                addr_offset ++;
                if (block_idx % ldb_width == ldb_width - 1)
                    addr_offset += pack_length - ldb_width;
                block_idx ++;
            }
        }

        // bottom-left quater
        for (j = led_half_height; j < gu16LedHeight; j++)
        {
            for (i = 0; i < led_half_width; i++)
            {
                MDrv_LD_MIUWriteByte(addr_base_L-ld_addr_base, addr_offset, LDB_blocks[i + j*gu16LedWidth]);
                if(stDrvLdMiscInfo.u8ClkHz == 120)
                MDrv_LD_MIUWriteByte(addr_base_R-ld_addr_base, addr_offset, LDB_blocks[i + j*gu16LedWidth]);
                addr_offset ++;
                if (block_idx % ldb_width == ldb_width - 1)
                    addr_offset += pack_length - ldb_width;
                block_idx ++;
            }
        }

        // bottom-left quater
        for (j = led_half_height; j < gu16LedHeight; j++)
        {
            for (i = led_half_width; i < gu16LedWidth; i++)
            {
                MDrv_LD_MIUWriteByte(addr_base_L-ld_addr_base, addr_offset, LDB_blocks[i + j*gu16LedWidth]);
                if(stDrvLdMiscInfo.u8ClkHz == 120)
                MDrv_LD_MIUWriteByte(addr_base_R-ld_addr_base, addr_offset, LDB_blocks[i + j*gu16LedWidth]);

                addr_offset ++;
                if (block_idx % ldb_width == ldb_width - 1)
                    addr_offset += pack_length - ldb_width;
                block_idx ++;
            }
        }
    }
    else
    {
        for (j = 0; j < gu16LedHeight; j++)
        {
            addr_offset = j*pack_length;
            for (i = 0; i < ldb_width; i++)
                MDrv_LD_MIUWriteByte(addr_base_L-ld_addr_base, addr_offset++, *LDB_blocks++);
            if(stDrvLdMiscInfo.u8ClkHz == 120)
           {
            LDB_blocks -= 2;
            addr_offset = j*pack_length;
            for (i = gu16LedWidth-ldb_width; i < gu16LedWidth; i++)
                MDrv_LD_MIUWriteByte(addr_base_R-ld_addr_base, addr_offset++, *LDB_blocks++);
            }
        }
    }

    MDrv_LD_FlushDRAMData((unsigned long)(u64Vaddr+addr_base_L-ld_addr_base), addr_base_L, gu16LedHeight*pack_length);
   if(stDrvLdMiscInfo.u8ClkHz == 120)
    MDrv_LD_FlushDRAMData((unsigned long)(u64Vaddr+addr_base_R-ld_addr_base), addr_base_R, gu16LedHeight*pack_length);

}

void MDrv_LD_MarqueeDemo(void)
{
    static MS_U16 u16Counter = 0;
    //MS_S8 ret = 0;
    MS_U16 u16Leds = gu16LedWidth * gu16LedHeight;
    //MS_U16 u16SPIBlocks[LD_MAX_BW*LD_MAX_BH];

    if(u16Leds > LD_MAX_BW*LD_MAX_BH)
    {
        LD_ERROR(" buffer is not enough, enlarge first! n_leds:%d\n",u16Leds);
        return;
    }

    //memset(u16SPIBlocks,0,LD_MAX_BW*LD_MAX_BH);    //mark for real bl
    memset(pLDB_blocks,0,LD_MAX_BW*LD_MAX_BH);    //mark for digital
    MHal_LD_SetCompensationEn(FALSE);
    MHal_LD_SetLSFOutMode(0x5); //0x1 0x5
    for(;u16Counter<u16Leds;u16Counter++)
    {
        pLDB_blocks[u16Counter] = 0xff;
        MDrv_LD_OutputLDBFrameBacklight(pLDB_blocks); // digital backlight
        msleep(stDrvLdMiscInfo.u32MarqueeDelay);

        pLDB_blocks[u16Counter] = 0;
        //u16SPIBlocks[u16Counter] = 0;
    }
}

void MDrv_LD_AlgoFilterEnhanceSecFunc(U16* SD_blocks)
{
    U16 led_width = MHal_LD_GetLEDBacklightWidth();
    U16 led_height = MHal_LD_GetLEDBacklightHeight();
    U16 n_leds = led_width * led_height;
    U16 block_idx = 0,Max_idx = 0;
    //U8 Write_Flag_block[288] = {0};
    ///int k,q,p,neighbor_idx;
    //U32 Max_ld_str=0,Dect_Max_block =0,u32SD_blocks[LD_MAX_BLOCK_SIZE] = {0};///u32Neighbor_blocks[8]= {0};//Diff_ld_Str=0,;
    U32 Max_ld_str=0,Dect_Max_block =0;///u32Neighbor_blocks[8]= {0};//Diff_ld_Str=0,;
    static U32 u32SD_blocks[LD_MAX_BLOCK_SIZE] = {0};///u32Neighbor_blocks[8]= {0};//Diff_ld_Str=0,;
    //const int N1_direct = 8; // dist = 1.0
    //const int xoffset1_direct[8]  = { -1,  0,  1,  0, -1,  1,  1, -1 };
    //const int yoffset1_direct[8]  = {  0, -1,  0,  1, -1, -1,  1,  1 };
    /////////Add average data to prevent LED Flicker in dark field////////////////////
    U32 u32AverageSDBlkData = 0,u32AverageDarkFieldData = 0;
    U8 u8DarkFieldCnt = 0;

    memset(u32SD_blocks, 0, LD_MAX_BLOCK_SIZE*sizeof(U32));
    for (block_idx = 0; block_idx < n_leds; block_idx++)
    {
        u32SD_blocks[block_idx] = SD_blocks[block_idx];
        if(Dect_Max_block < u32SD_blocks[block_idx])
        {
            Max_idx = block_idx;
            Dect_Max_block = u32SD_blocks[block_idx];
        }
        /////////Add average data to prevent LED Flicker in dark field////////////////////
        u32AverageSDBlkData = u32AverageSDBlkData+u32SD_blocks[block_idx];///new add

        if((u32SD_blocks[block_idx]<0x3000)&&(u32SD_blocks[block_idx]>=0x0A00))
        {
            u32AverageDarkFieldData = u32AverageDarkFieldData+u32SD_blocks[block_idx];
            u8DarkFieldCnt++;
        }
    }

    /////////Add average data to prevent LED Flicker in dark field////////////////////
    if(n_leds>0)
        u32AverageSDBlkData = u32AverageSDBlkData/n_leds;///new add
    if(u8DarkFieldCnt>1)
        u32AverageDarkFieldData = u32AverageDarkFieldData/u8DarkFieldCnt;

    if(au16SD_Mean_blocks[Max_idx]>0)
    {
        for (block_idx = 0; block_idx < n_leds; block_idx++)
        {
            if(au16SD_Mean_blocks[Max_idx] > au16SD_Mean_blocks[block_idx])
            {
                if(au16SD_Mean_blocks[block_idx]>0x8000)
                    Max_ld_str = (((0x10000-au16SD_Mean_blocks[block_idx])*0xffff)/au16SD_Mean_blocks[Max_idx]);
                else
                    Max_ld_str = ((au16SD_Mean_blocks[block_idx]*0xffff)/au16SD_Mean_blocks[Max_idx]);
            }
            else
                Max_ld_str = 0xffff;

            /////////Add average data to prevent LED Flicker in dark field////////////////////
            if((u8DarkFieldCnt>(n_leds/2))&&(u32SD_blocks[block_idx]<u32AverageSDBlkData)&&(u32SD_blocks[block_idx]<0x3000)&&(u32SD_blocks[block_idx]>=0x0A00))
            {
                u32SD_blocks[block_idx] = u32AverageDarkFieldData+(Dect_Max_block-u32AverageSDBlkData)/2;
            }

            else
            {
                u32SD_blocks[block_idx] = (u32SD_blocks[Max_idx] * Max_ld_str + u32SD_blocks[block_idx] * (0x10000 - Max_ld_str) + 0x8000) >> 16; // 0 ~ 0xFF00
            }
        }
        ///printf("\n");
    }

    for (block_idx = 0; block_idx < n_leds; block_idx++)
    {
        SD_blocks[block_idx] = (U16)u32SD_blocks[block_idx];
    }
}

//Add read/write backlight gamma of local dimming by MSTV_TOOL.
static void MDrv_LDAlgo_ResetGammaTable(U8* LD_Gamma, U8* LD_Remap)
{
    MS_BOOL bResetGammaTableMode = MHal_LD_Get_ResetGammaTable_Mode();  //0:gamma 1:remap
    MS_BOOL bResetGammaTableRW = MHal_LD_Get_ResetGammaTable_RW();    //0:Read 1:Write
    MS_U8 u8ResetGammaTableIdx = MHal_LD_Get_ResetGammaTable_Index();    //Index 0~255
    MS_U8 u8ResetGammaTableData;   //Data 0~255

    if(bResetGammaTableRW==0)   //Read
    {
        if(bResetGammaTableMode==0) //gamma
            u8ResetGammaTableData=LD_Gamma[u8ResetGammaTableIdx];
        else    //remap
            u8ResetGammaTableData=LD_Remap[u8ResetGammaTableIdx];
        MHal_LD_Set_ResetGammaTable_Data(u8ResetGammaTableData);
    }
    else    //Write
    {
        u8ResetGammaTableData = MHal_LD_Get_ResetGammaTable_Data();
        if(bResetGammaTableMode==0) //gamma
            LD_Gamma[u8ResetGammaTableIdx]=u8ResetGammaTableData;
        else    //remap
            LD_Remap[u8ResetGammaTableIdx]=u8ResetGammaTableData;
    }

    MHal_LD_Set_ResetGammaTable_En(FALSE);
}

void MDrv_LD_Main(void)
{
    MS_U8 in_gamma = 0;
    MS_U8 out_gamma = 0;

    in_gamma = MHal_LD_GetSWBacklightInGamma();
    out_gamma = MHal_LD_GetSWBacklightOutGamma();

    if((NULL == pLDF_blocks) || (NULL == pSD_blocks) || (NULL == pTF_blocks) ||
        (NULL == pSF_blocks) || (NULL == pLDB_blocks) || (NULL == pSPI_blocksBuffer0) ||
        (NULL == pSPI_blocksBuffer1) ||(NULL == pLuma_blocks) ||(FALSE == bLdOn)||
        (NULL == pGamma_blocks))
    {
        LD_ERROR("error!  init failed\n");
        return ;
    }
    bLdReady = TRUE;

    // SW input
    if (MHal_LD_GetSWReadLDFEn())
        MDrv_LD_LoadLDFFrameStatistics(pLDF_blocks);

    // SW algorithm begin
    if (MHal_LD_GetSWAlgorithmEn())
    {
        gu16LedWidth = MHal_LD_GetLEDBacklightWidth();
        gu16LedHeight = MHal_LD_GetLEDBacklightHeight();

        //Add read/write backlight gamma of local dimming by MSTV_TOOL.only susport with ld2_bk
#ifdef  LD2_BK
        if(MHal_LD_Get_ResetGammaTable_En())
        {
         MDrv_LDAlgo_ResetGammaTable(pTbl_LD_Gamma[in_gamma], pTbl_LD_Remap[out_gamma]);
        }
#endif

        MDrv_LD_AlgoScalingDown(pLDF_blocks, pSD_blocks);       // LDF -> SD
        MDrv_LD_AlgoBacklightGamma(pSD_blocks, pTbl_LD_Gamma[in_gamma]);
        MDrv_LD_AlgoSpatialFilter(pSD_blocks, pSF_blocks);     // SD -> SF
        MDrv_LD_AlgoTemporalFilter(pSF_blocks, pTF_blocks);      // TF -> TF
        MDrv_LD_AlgoLocalDimmingStrength(pTF_blocks);
        MDrv_LD_AlgoBacklightGamma(pTF_blocks, pTbl_LD_Remap[out_gamma]);
        MDrv_LD_AlgoPostProcess(pTF_blocks, pLuma_blocks, pLDB_blocks);      // process SPI & LDB

        MDrv_LD_AlgoGlobalDimmingStrength(pLuma_blocks);
    }
    // SW algorithm end

    // SW output
    if (MHal_LD_GetSWWriteLDBEn())
        MDrv_LD_OutputLDBFrameBacklight(pLDB_blocks); // digital backlight

   // printk("++++ MDrv_LD_Main take  %lu jiff, %lu ms \n",jiffies, jiffies_to_msecs(jiffies));
}


static void MDrv_LD_FlushDRAMData(unsigned long u32VAddr,unsigned long u32PAddr,unsigned long u32Size)
{
    Chip_Flush_Memory();

    if(u32PAddr >= ARM_MIU3_BASE_ADDR)
        Chip_Flush_Cache_Range_VA_PA(u32VAddr, (u32PAddr - ARM_MIU3_BASE_ADDR) + ARM_MIU3_BUS_BASE , u32Size);
    else if((u32PAddr >= ARM_MIU2_BASE_ADDR) && (u32PAddr < ARM_MIU3_BASE_ADDR))
        Chip_Flush_Cache_Range_VA_PA(u32VAddr, (u32PAddr - ARM_MIU2_BASE_ADDR) + ARM_MIU2_BUS_BASE , u32Size);
    else if((u32PAddr >= ARM_MIU1_BASE_ADDR) && (u32PAddr < ARM_MIU2_BASE_ADDR))
        Chip_Flush_Cache_Range_VA_PA(u32VAddr, (u32PAddr - ARM_MIU1_BASE_ADDR) + ARM_MIU1_BUS_BASE , u32Size);
    else
        Chip_Flush_Cache_Range_VA_PA(u32VAddr, u32PAddr + ARM_MIU0_BUS_BASE , u32Size);
}

void MDrv_LD_Suspend(void)
{
    if(FALSE == gbLdInit || u8LdSuspend == 1)
    {
      LD_INFO("please init first or already suspended u8LdSuspend=%d,gbLdInit = %d\n",u8LdSuspend,gbLdInit);
      return ;
    }
    LD_INFO(" entrance \n");
    u8LdSuspend = 1;
    gu8Compesation = 0;
    MHal_LD_SetLocalDimmingEn(FALSE);
}

void MDrv_LD_Resume(void)
{
    MS_S8 err = 0;
    if(FALSE == gbLdInit)
    {
      LD_INFO(" please init first \n");
      return ;
    }
    LD_INFO(" entrance \n");

    strcpy( stCusPath.aCusPath, stCusPathBck.aCusPath);
    strcpy( stCusPath.aCusPathU, stCusPathBck.aCusPathU);
    {
        err = MDrv_LD_Init(ld_addr_base);
        if(0 != err)
        {
            LD_ERROR(" MDrv_LD_Init FAIL \n");
            return;
        }
    }
    bLdOn = TRUE;
    u8LdSuspend = 0;
    MHal_LD_SetLocalDimmingEn(TRUE);
}

MS_BOOL MDrv_LD_ProcessIRQ(void)
{
    if ((pstWork)&&(0 == u8LdSuspend))
    {
        schedule_work(pstWork);
    }

    return TRUE;
}

static void MDrv_LD_SendConstantLuma(void)
{
    static MS_U32 i;
    MS_U16 n_leds = MHal_LD_GetLEDBacklightWidth() * MHal_LD_GetLEDBacklightHeight();

    if(0 == ++i%7000) //
    {
        i = 0;
        LD_INFO(" send   gbMarqueue:%d, u8Luma:%d, n_leds:%d \n", gbMarqueue, gu16LedLuma, n_leds);
    }
    if(TRUE == gbMarqueue) //for marqueue case only light up one led
    {
        memset(pSPI_blocks,0,n_leds*sizeof(MS_U16));
        pSPI_blocks[gu16LedLuma] = 0xffff;       //gu16LedLuma using for led number
    }
    else                 //for disable ld case ,all led are same luma
    {
        MS_U16 j = 0;
        for (j = 0; j < n_leds; j++)
            pSPI_blocks[j] = gu16LedLuma;         //gu16LedLuma using constant luma
    }

    n_leds = MDrv_LD_ConvertLumaData2SPIData(pSPI_blocks, n_leds);

    MDrv_LD_FlushDRAMData((unsigned long)pSPI_blocks,(unsigned long)(ld_addr_base+((void*)pSPI_blocksBuffer0-u64Vaddr)), n_leds);
    MDrv_LD_FlushDRAMData((unsigned long)pSPI_blocks,(unsigned long)(ld_addr_base+((void*)pSPI_blocksBuffer1-u64Vaddr)), n_leds);

}

static void do_ld(struct work_struct *work)
{
    if (TRUE == bLdOn)
        MDrv_LD_Main();
    //else if(TRUE == bLdReady)
   //     MDrv_LD_SendConstantLuma();
   // else
    //    ;
}

void MDrv_LD_Enable(MS_BOOL bOn, MS_U8 u8Luma)
{
    if(FALSE == gbLdInit)
    {
      LD_INFO(" please init first \n");
      return ;
    }
    if(FALSE == bOn)
    {
        MS_U16 n_leds = MHal_LD_GetLEDBacklightWidth() * MHal_LD_GetLEDBacklightHeight();

        LD_INFO(" disable, u8Luma:%d, n_leds:%d \n", u8Luma, n_leds);

        //Set Demo mode
        MHal_LD_SetSWPulseMode(0xD0);
        gu8LedMod = MHal_LD_GetSWPulseMode();
        //led_num
        MHal_LD_SetLSFOutMode(0x0);
        MHal_LD_SetSWPulseId(0xfff);

        //led_luma
        MHal_LD_SetSWPulseLEDIntensity(u8Luma); //hight 8bit
        MHal_LD_SetSWPulseLDBIntensity(u8Luma); //low  8bit
        //disable Compensation
        MHal_LD_SetCompensationEn(0x0);
        }
    else
    {
        LD_INFO(" enable \n");
        //for dynamic control backlight
        if( gu8LedMod ==0xD )
        {
            MHal_LD_SetSWPulseMode(0x0);
            gu8LedMod = MHal_LD_GetSWPulseMode();
            //led_num
            MHal_LD_SetLSFOutMode(0x0);
            MHal_LD_SetSWPulseId(0x0);

            //led_luma
            MHal_LD_SetSWPulseLEDIntensity(0x0); //hight 8bit
            MHal_LD_SetSWPulseLDBIntensity(0x0); //low  8bit
            //enable Compensation
            MHal_LD_SetCompensationEn(0x1);
        }
        else //after init and should enable
        {
              bLdOn = bOn;
            MHal_LD_SetLocalDimmingEn(TRUE);
        }
}
}

MS_S8 MDrv_LD_PrepareBuffs(void)
{
    if(pGamma_blocks)
    {
        LD_INFO(" test! u64Vaddr 0x:%p, contents 0x:%x \n", u64Vaddr,*(MS_U8*)u64Vaddr);
        LD_INFO(" test! pSPI_blocksBuffer0 0x:%p, contents 0x:%x \n", pSPI_blocksBuffer0,*(MS_U8*)pSPI_blocksBuffer0);
        LD_INFO(" test! pSPI_blocksBuffer1 0x:%p, contents 0x:%x \n", pSPI_blocksBuffer1,*(MS_U8*)pSPI_blocksBuffer1);
        LD_INFO(" test! pLDF_blocks 0x:%p, contents 0x:%x \n", pLDF_blocks,*(MS_U8*)pLDF_blocks);
        LD_INFO(" test! pSD_blocks 0x:%p, contents 0x:%x \n", pSD_blocks,*(MS_U8*)pSD_blocks);
        LD_INFO(" test! pSF_blocks 0x:%p, contents 0x:%x \n", pSF_blocks,*(MS_U8*)pSF_blocks);
        LD_INFO(" test! pTF_blocks 0x:%p, contents 0x:%x \n", pTF_blocks,*(MS_U8*)pTF_blocks);
        LD_INFO(" test! pLDB_blocks 0x:%p, contents 0x:%x \n", pLDB_blocks,*(MS_U8*)pLDB_blocks);
        LD_INFO(" test! pLuma_blocks 0x:%p, contents 0x:%x \n", pLuma_blocks,*(MS_U8*)pLuma_blocks);
        LD_INFO(" test! pGamma_blocks 0x:%p, contents 0x:%x \n", pGamma_blocks,*(MS_U8*)pGamma_blocks);
        LD_WARN("  re-entrance! \n");
        return 0;
    }

    pSPI_blocksBuffer0 = (MS_U16*)(u64Vaddr+SPI_BUFF_OFFSET); //mmapbaseaddress + 0x80000
    pSPI_blocksBuffer1 = (MS_U16*)((MS_U8*)pSPI_blocksBuffer0 + LD_MAX_BLOCK); //pSPI_blocksBuffer0 + 0X2800
    pLDF_blocks = (MS_U8*)((MS_U8*)pSPI_blocksBuffer1+LD_MAX_BLOCK); //pSPI_blocksBuffer1 + 0X2800
    pSD_blocks = (MS_U16*)((MS_U8*)pLDF_blocks+LD_MAX_BLOCK); //LDF_blocks + 0X2800
    pSF_blocks = (MS_U16*)((MS_U8*)pSD_blocks+LD_MAX_BLOCK);  //SD_blocks + 0X2800
    pTF_blocks = (MS_U16*)((MS_U8*)pSF_blocks+LD_MAX_BLOCK);  //SF_blocks + 0X2800
    pLDB_blocks = (MS_U8*)((MS_U8*)pTF_blocks+LD_MAX_BLOCK);  //TF_blocks + 0X2800
    pLuma_blocks = (MS_U16*)((MS_U8*)pLDB_blocks+LD_MAX_BLOCK);  //pLDB_blocks + 0X2800
    pGamma_blocks = (MS_U8*)((MS_U8*)pLuma_blocks+LD_MAX_BLOCK);  //pLDB_blocks + 0X2800, SIZE 256*32

    if(enDbgLevel&E_LD_DEBUG_LEVEL_INFO)
    {
        memset(((void *)u64Vaddr),0x15,32);
        LD_INFO(" test! LD_VA_BASE 0x:%p, contents 0x:%x \n", u64Vaddr,*(MS_U8*)u64Vaddr);
        memset(((void *)(u64Vaddr+0x400/(sizeof(MS_U8)))),0x24,32);
        LD_INFO(" (LD_VA_BASE+0x400) 0x:%p, contents 0x:%x \n",(u64Vaddr+0x400/(sizeof(MS_U8))),*(MS_U8*)(u64Vaddr+0x400/(sizeof(MS_U8))));
        memset(((void *)(u64Vaddr+0x4000/(sizeof(MS_U8)))),0x30,32);
        LD_INFO(" (LD_VA_BASE+0x4000) 0x:%p, contents 0x:%x \n",(u64Vaddr+0x4000/(sizeof(MS_U8))),*(MS_U8*)(u64Vaddr+0x4000/(sizeof(MS_U8))));
        memset(((void *)(u64Vaddr+0x8000/(sizeof(MS_U8)))),0x44,32);
        LD_INFO(" (LD_VA_BASE+0x8000) 0x:%p, contents 0x:%x \n",(u64Vaddr+0x8000/(sizeof(MS_U8))),*(MS_U8*)(u64Vaddr+0x8000/(sizeof(MS_U8))));
        memset(((void *)(u64Vaddr+SPI_BUFF_OFFSET/(sizeof(MS_U8)))),0x55,32);
        LD_INFO(" (LD_VA_BASE+SPI_BUFF_OFFSET) 0x:%p, contents 0x:%x \n",(u64Vaddr+SPI_BUFF_OFFSET/(sizeof(MS_U8))), *(MS_U8*)(u64Vaddr+SPI_BUFF_OFFSET/(sizeof(MS_U8))));
        LD_INFO(" (LD_VA_BASE+SPI_BUFF_OFFSET) 0x:%x, (u64Vaddr+0x4000) 0x:%x \n", MDrv_LD_MIUReadByte(0x80000, 0), MDrv_LD_MIUReadByte(0x4000, 0));
    }

    memset(((void *)pSPI_blocksBuffer0),0xFF,LDM_LEN/2);    //follow spi to the end of buffer
    MDrv_LD_FlushDRAMData((unsigned long)pSPI_blocksBuffer0, ld_addr_base+SPI_BUFF_OFFSET, LDM_LEN/2);
    LD_INFO(" test! pSPI_blocksBuffer0 0x:%p, contents 0x:%x \n", pSPI_blocksBuffer0,*(MS_U8*)pSPI_blocksBuffer0);
    LD_INFO(" test! pSPI_blocksBuffer1 0x:%p, contents 0x:%x \n", pSPI_blocksBuffer1,*(MS_U8*)pSPI_blocksBuffer1);
    LD_INFO(" test! pLDF_blocks 0x:%p, contents 0x:%x \n", pLDF_blocks,*(MS_U8*)pLDF_blocks);
    LD_INFO(" test! pSD_blocks 0x:%p, contents 0x:%x \n", pSD_blocks,*(MS_U8*)pSD_blocks);
    LD_INFO(" test! pSF_blocks 0x:%p, contents 0x:%x \n", pSF_blocks,*(MS_U8*)pSF_blocks);
    LD_INFO(" test! pTF_blocks 0x:%p, contents 0x:%x \n", pTF_blocks,*(MS_U8*)pTF_blocks);
    LD_INFO(" test! pLDB_blocks 0x:%p, contents 0x:%x \n", pLDB_blocks,*(MS_U8*)pLDB_blocks);
    LD_INFO(" test! pLuma_blocks 0x:%p, contents 0x:%x \n", pLuma_blocks,*(MS_U8*)pLuma_blocks);
    LD_INFO(" test! pGamma_blocks 0x:%p, contents 0x:%x \n", pGamma_blocks,*(MS_U8*)pGamma_blocks);

    memset(pLDB_blocks, 0xFF, LD_MAX_BLOCK);
    MDrv_LD_OutputLDBFrameBacklight(pLDB_blocks);
    LD_INFO("  done \n");
    return 0;
}

MS_S8 MDrv_LD_Setup(void)
{
    mutex_lock(&Semutex_LD);

    pstWork = kmalloc(sizeof(*pstWork), GFP_ATOMIC);
    if (pstWork)
    {
        INIT_WORK(pstWork, do_ld);
    }

    mutex_unlock(&Semutex_LD);

    //backup for demo pattern
    gu8LedMod = MHal_LD_GetSWPulseMode();
    gu8LedID = MHal_LD_GetSWPulseId();
    gu8LedStr = MHal_LD_GetSWPulseLEDIntensity();
    gbCompEn = MHal_LD_GetCompensationEn();
    gu8LsfMod = MHal_LD_GetLSFOutMode();

    LD_INFO("  done \n");
    return 0;
}

void MDrv_LD_SetGlobalStrength(MS_U8 u8Strength)
{
    if(FALSE == gbLdInit)
    {
      LD_INFO(" please init first \n");
      return ;
    }
    LD_INFO(" u8GlobalStrength:%d \n", u8Strength);
    MHal_LD_SetSWGlobalDimmingStrength(u8Strength);
}

void MDrv_LD_SetLocalStrength(MS_U8 u8Strength)
{
    if(FALSE == gbLdInit)
    {
      LD_INFO(" please init first \n");
      return ;
    }
    LD_INFO(" u8LocalStrength:%d \n", u8Strength);
    MHal_LD_SetSWLocalDimmingStrength(u8Strength);
}


MS_PHY MDrv_LD_GetDataAddr(EN_LD_GET_DATA_TYPE type)
{
    MS_PHY uAddr = 0;
    if(FALSE == gbLdInit)
    {
      LD_INFO(" please init first \n");
      return 0;
    }

    switch(type)
    {
        case E_LD_DATA_TYPE_LDF:
        {
            //uAddr = MHal_LD_GetLDFFrameBufBaseAddr_L0();
            uAddr = ((void*)pLDF_blocks - u64Vaddr) + MHal_LD_GetLDFFrameBufBaseAddr_L0();
            LD_INFO(" ldf uAddr:0x%lx \n",(unsigned long)uAddr);
            break ;
        }
        case E_LD_DATA_TYPE_LDB:
        {
            //uAddr = MHal_LD_GetLDBFrameBufBaseAddr_0();
            uAddr = ((void*)pLDB_blocks - u64Vaddr) + MHal_LD_GetLDFFrameBufBaseAddr_L0();
            LD_INFO(" ldb uAddr:0x%lx \n",(unsigned long)uAddr);
            break ;
        }
        case E_LD_DATA_TYPE_SPI:
        {
            uAddr = MHal_LD_GetLDFFrameBufBaseAddr_L0();
            uAddr += SPI_BUFF_OFFSET;
            LD_INFO(" spi uAddr:0x%lx \n",(unsigned long)uAddr);
            break ;
        }

        default:
            LD_ERROR("Unknown type %d\n", type);
            return 0;
    }

    return uAddr;
}

void MDrv_LD_SetDemoPattern(MS_U8 u8Pattern, MS_BOOL bOn, MS_U16 u16LEDNum)
{
    if(FALSE == gbLdInit)
    {
      LD_INFO(" please init first \n");
      return ;
    }

    LD_INFO(" demo Pattern:%d \n", u8Pattern);
    switch(u8Pattern)
    {
        case E_LD_DEMO_PATTERN_SWITCH_SINGLE_LED:
        {
            MS_U16 n_leds = MHal_LD_GetLEDBacklightWidth() * MHal_LD_GetLEDBacklightHeight();

            LD_INFO(" E_DEMO_PATTERN_SWITCH_SINGLE_LED isOn:%d, uLEDNum:%d, totalLEDNum:%d\n",
                    bOn,u16LEDNum, n_leds);
            if(TRUE == bOn)
            {
                //Set Demo mode
                MHal_LD_SetSWPulseMode(0xD0);
                if(n_leds < u16LEDNum)
                {
                    u16LEDNum = n_leds - 1;
                }
                MHal_LD_SetLSFOutMode(0x1);
                //led_num
                MHal_LD_SetSWPulseId(u16LEDNum);

                //led_luma
                MHal_LD_SetSWPulseLEDIntensity(0xff); //hight 8bit
                MHal_LD_SetSWPulseLDBIntensity(0xff); //low  8bit
                //disable Compensation,maybe flash white when disable compensation so delay it
                msleep(50);
                MHal_LD_SetCompensationEn(0x0);
                //set  LSF output mode
            }
            else
            {
                //Set Demo mode
                MHal_LD_SetSWPulseMode(0x0);
                //led_num
                MHal_LD_SetSWPulseId(0x0);
                //led_luma
                MHal_LD_SetSWPulseLEDIntensity(0x0); //hight 8bit
                MHal_LD_SetSWPulseLDBIntensity(0x0); //low  8bit
                //disable Compensation
                MHal_LD_SetCompensationEn(0x1);
                //set  LSF output mode
                MHal_LD_SetLSFOutMode(0x0);
            }
            break ;
        }
        case E_LD_DEMO_PATTERN_LEFT_RIGHT_HALF:
        {
            LD_INFO(" E_DEMO_PATTERN_LEFT_RIGHT_HALF isOn:%d\n", bOn);

            if(TRUE == bOn)
            {
                MHal_LD_SetSWPulseMode(0);
                MHal_LD_SetSWPulseId(0x0002);
                MHal_LD_SetSWPulseLEDIntensity(0xFF);
            }
            else
            {
                MHal_LD_SetSWPulseMode(gu8LedMod);
                MHal_LD_SetSWPulseId(gu8LedID);
                MHal_LD_SetSWPulseLEDIntensity(gu8LedStr);
            }
            break ;
        }
        case E_LD_DEMO_PATTERN_MARQUEE:
        {
            LD_INFO(" E_DEMO_PATTERN_MARQUEE isOn:%d,  delay:%d\n", bOn, stDrvLdMiscInfo.u32MarqueeDelay);
            break ;
        }
        case E_LD_DEMO_PATTERN_LEFT_RIGHT_COLOR_SHELTER:
        {
            LD_INFO(" E_DEMO_PATTERN_LEFT_RIGHT_COLOR_SHELTER isOn:%d\n", bOn);
            if(TRUE == bOn)
            {
                gbCompEn = MHal_LD_GetCompensationEn();
                MHal_LD_SetCompensationEn(FALSE);
                //MHal_LD_SetLSFOutMode(0x5);//0x1  //no ld panel to test real bl, using digital bl
                MHal_LD_SetLSFOutMode(0x1);//0x1  //to test real ld panel, using real bl
                //MHal_LD_SetSWPulseLEDIntensity(0xFF);
                MHal_LD_SetReportWindowStartHorizon(0x07FF);//
                MHal_LD_SetReportWindowSwap();//L
            }
            else
            {
                MHal_LD_SetLSFOutMode(0);
                MHal_LD_SetReportWindowStartHorizon(0x0FFF);//
                MHal_LD_ClearReportWindowSwap();//L

                MHal_LD_SetCompensationEn(gbCompEn);
                //msleep(200);
            }
            break ;
        }
        default:
            LD_ERROR("Unknown pattern %d\n", u8Pattern);
            break;
    }
}

void MDrv_LD_DumpDRAM(EN_LD_GET_DATA_TYPE type)
{
    MS_U16 counter = 0;
    MS_U8 *u8Buff = NULL;
    LD_INFO(" entrance type:%d \n", type);

    u8Buff = kmalloc(LD_MAX_BLOCK_SIZE*4, GFP_KERNEL);
    if (!u8Buff)
    {
        LD_ERROR("malloc memory fail \n");
    }

    memset(u8Buff,0,LD_MAX_BLOCK_SIZE*4);

    switch(type)
    {
        case E_LD_DATA_TYPE_LDF:
        {
            #if 0
            MS_PHY uAddr = 0;
            MS_U32 addr_offset_L = 0;
            MS_U16 pack_length = 0;
            MS_U16 i = 0, j = 0, counter = 0;
            MS_U16 ldf_width = MHal_LD_GetLDFBacklightWidth();
            MS_U16 ldf_height = MHal_LD_GetLDFBacklightHeight();
            uAddr = MHal_LD_GetLDFFrameBufBaseAddr_L0();
            LD_INFO(" ldf uAddr:0x%llx \n", uAddr);

            if (MHal_LD_GetLDFWriteDCMaxOFEn()) // 4 bytes : OF[15:8] OF[7:0] Max DC
            {
                ldf_width *= 4;
            }
            else if (MHal_LD_GetLDFWriteDCMaxEn()) // 2 bytes : Max DC
            {
                ldf_width *= 2;
            }

            if (ldf_width > 0)
                pack_length = (((ldf_width - 1) / MHAL_LD_PACKLENGTH) + 1) * MHAL_LD_PACKLENGTH;

            printk("+++++++++++++++ get dump ldf dram data: \n", 0);
            for (j = 0; j < ldf_height; j++)
            {
                addr_offset_L = j * pack_length;
                for (i = 0; i < ldf_width; i++)
                {
                    printk("0x%x \t", MDrv_LD_MIUReadByte(uAddr-ld_addr_base, addr_offset_L++));
                    if( 0 == (++counter) % 32)
                        printk(" \n");
                }
            }
            printk(" \n");
            #else
            MS_U16 ldf_width = MHal_LD_GetLDFBacklightWidth();
            MS_U16 ldf_height = MHal_LD_GetLDFBacklightHeight();

            if (MHal_LD_GetLDFWriteDCMaxOFEn()) // 4 bytes : OF[15:8] OF[7:0] Max DC
            {
                ldf_width *= 4;
            }
            else if (MHal_LD_GetLDFWriteDCMaxEn()) // 2 bytes : Max DC
            {
                ldf_width *= 2;
            }

            spin_lock_irq(&spinlock_ld);
            memcpy(u8Buff,pLDF_blocks,ldf_width*ldf_height);
            spin_unlock_irq(&spinlock_ld);
            printk("+++++++++++++++  dump ldf dram data \n");
            for (counter = 0; counter < ldf_width*ldf_height; )
            {
                printk("0x%02x ", *(u8Buff+counter));
                if( 0 == (++counter) % 16)
                    printk(" \n");
            }
            if( 0 != counter % 16)
            printk(" \n");
            #endif
            break ;
        }
        case E_LD_DATA_TYPE_LDB:
        {
            MS_U16 ldb_width = MHal_LD_GetLDBBacklightWidth();
            MS_U16 ldb_height = MHal_LD_GetLDBBacklightHeight();

            spin_lock_irq(&spinlock_ld);
            memcpy(u8Buff,pLDB_blocks,ldb_width*ldb_height);
            spin_unlock_irq(&spinlock_ld);
            printk("+++++++++++++++  dump ldb dram data \n");
            for (counter = 0; counter < ldb_width*ldb_height; )
            {
                printk("0x%02x ", *(u8Buff+counter));
                if( 0 == (++counter) % 16)
                    printk(" \n");
            }
            if( 0 != counter % 16)
            printk(" \n");
            break ;
        }
        case E_LD_DATA_TYPE_SPI:
        {
            MS_U16 n_leds = gu16LedWidth * gu16LedHeight;

            switch(stDrvLdMiscInfo.u8SPIBits)
            {
                case E_LD_SPI_DATA_16_BIT_LH:
                case E_LD_SPI_DATA_12_BIT_HL:
                case E_LD_SPI_DATA_12_BIT_LH:
                case E_LD_SPI_DATA_16_BIT_HL:
                   n_leds = n_leds*2;
                   break;
               case E_LD_SPI_DATA_8_BIT:
                   break;
               case E_LD_SPI_DATA_12_BIT:
                   n_leds = (n_leds*3)/2;
                   break;
           case E_LD_SPI_DATA_CUS_BIT:
                   n_leds = n_leds*2;
                   break;
               default:
                   LD_ERROR("Unknown type %d\n", stDrvLdMiscInfo.u8SPIBits);
                   break;
           }
            spin_lock_irq(&spinlock_ld);
            memcpy(u8Buff,u8LEDData,n_leds);
            spin_unlock_irq(&spinlock_ld);

            printk("\033[41;33m%s[%d]pSPI_blocks = 0x%lx u8FrameIdx =%d\033[0m\n", __FUNCTION__, __LINE__,(unsigned long)(ld_addr_base+((void*)pSPI_blocks-u64Vaddr)),gu8FrameIdx);
            printk("+++++++++++++++  dump spi dram data \n");
            for (counter = 0; counter < n_leds; )
            {
                printk("0x%02x ", *(u8Buff+counter));
                if( 0 == (++counter) % 16)
                    printk(" \n");
            }
            if( 0 != counter % 16)
            printk(" \n");
            break ;
        }

        default:
            LD_ERROR("Unknown type %d\n", type);
    }
    kfree(u8Buff);
}

void MDrv_LD_SetupGammaTable(MS_U8* u8GammaBlocks)
{
    MS_U8 u8Counter = 0;
    //32 items, sizeof item is 256
    memcpy(pGamma_blocks,u8GammaBlocks,GAMMA_TABLE_LEN*32);
    //pGamma_blocks = u8GammaBlocks;
    //the length of pTbl_LD_Gamma/pTbl_LD_Remap is 16,  and the layout is in order
    for(;u8Counter < 16; u8Counter++)
    {
        pTbl_LD_Gamma[u8Counter] = pGamma_blocks + u8Counter*GAMMA_TABLE_LEN;
        pTbl_LD_Remap[u8Counter] = pGamma_blocks + (16 + u8Counter)*GAMMA_TABLE_LEN;
    }
}

void MDrv_LD_CleanupModule(void)
{
    spin_lock_irq(&spinlock_ld);
    LD_INFO(" entrance \n");

    MHal_LD_SetLocalDimmingEn(FALSE);
    MHal_LD_Set_SRAM_Power_Down(TRUE);
    if(NULL != pstWork)
    {
        kfree(pstWork);
        pstWork = NULL;
    }

    if(NULL != pLDF_blocks)
    {
        pLDF_blocks = NULL;
    }
    if(NULL != pSD_blocks)
    {
        pSD_blocks = NULL;
    }
    if(NULL != pTF_blocks)
    {
        pTF_blocks = NULL;
    }
    if(NULL != pSF_blocks)
    {
        pSF_blocks = NULL;
    }
    if(NULL != pLDB_blocks)
    {
        pLDB_blocks = NULL;
    }
    if(NULL != pLuma_blocks)
    {
        pLuma_blocks = NULL;
    }
    if(NULL != pGamma_blocks)
    {
        pGamma_blocks = NULL;
    }

    iounmap(u64Vaddr);
    u64Vaddr = 0;
    spin_unlock_irq(&spinlock_ld);
    bLdOn = FALSE;
    bLdReady = FALSE;
    u8LdSuspend = 0;
}

#endif
