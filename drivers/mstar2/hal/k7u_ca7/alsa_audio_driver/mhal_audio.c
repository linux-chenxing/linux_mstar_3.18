///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2010 - 2011 MStar Semiconductor, Inc.
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


/*
 * ============================================================================
 * Include Headers
 * ============================================================================
 */
#include <linux/delay.h>
#include <linux/version.h>
#include <linux/platform_device.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,7,1))
#include <linux/module.h>
#endif
#include <asm/io.h>

#include <linux/fs.h>
#include <linux/mm.h>
#include <asm/uaccess.h>

#include "mstar/mstar_chip.h"
#include "mhal_audio.h"


/*
 * ============================================================================
 * Constant Definitions
 * ============================================================================
 */
#define _MAD_CHIP_MODEL    "MAD K6LITE"

#if defined(CONFIG_ARM64)
extern ptrdiff_t mstar_pm_base;
#define _MAD_PM_MODE_REG_BASE    mstar_pm_base
#else
#define _MAD_PM_MODE_REG_BASE    0xfd000000
#endif
#define _MAD_NON_PM_MODE_REG_BASE    0xfd200000
#define _MAD_PHYSICAL_MEM_BASE    _MAD_PM_MODE_REG_BASE
#define _MAD_KERNEL_SPACE_MEMORY_OFFSET    0xA0000000
#define _MAD_PHYSICAL_ADDR_TO_BUS_ADDR    MSTAR_MIU0_BUS_BASE

#define _MAD_BYTES_IN_LINE    16
#define _MAD_ADDR_CONVERTOR    0x1000
#define _MAD_MAILBOX_OFFSET    0x110000
#define _MAD_MAILBOX_OFFSET2    0x160000

#define _MAD_MAX_CHANNEL    2

#define _MAD_DSP2_DRAM_SIZE    0x2C0000
#define _MAD_DSP2_TO_COMMON_DRAM_OFFSET    (_MAD_DSP2_DRAM_SIZE)

#define _MAD_DMA_READER_BASE_OFFSET    (0x00000000 + _MAD_DSP2_TO_COMMON_DRAM_OFFSET)
#define _MAD_DMA_READER_BUF_UNIT    4096
#define _MAD_DMA_READER_TOTAL_BUF_SIZE    0x20000 /* 128 KByte */
#define _MAD_DMA_READER_MIN_AVAIL    (_MAD_DMA_READER_TOTAL_BUF_SIZE >> 3) /* The minimal available size should be reserved */
#define _MAD_DMA_READER_HIGH_THRESHOLD    (_MAD_DMA_READER_TOTAL_BUF_SIZE - _MAD_DMA_READER_MIN_AVAIL)
#define _MAD_DMA_READER_BUF_SIZE    (_MAD_DMA_READER_TOTAL_BUF_SIZE >> 1) /* Only half size can be used */
#define _MAD_DMA_READER_PERIOD_SIZE    (_MAD_DMA_READER_BUF_SIZE >> 2)
#define _MAD_DMA_READER_QUEUE_SIZE    100 /* ms */

#define _MAD_DMA_READER2_BASE_OFFSET    (0x2EA000 + _MAD_DSP2_TO_COMMON_DRAM_OFFSET)
#define _MAD_DMA_READER2_BUF_UNIT    4096
#define _MAD_DMA_READER2_TOTAL_BUF_SIZE    0x10000 /* 64 KByte */
#define _MAD_DMA_READER2_MIN_AVAIL    (_MAD_DMA_READER2_TOTAL_BUF_SIZE >> 3) /* The minimal available size should be reserved */
#define _MAD_DMA_READER2_HIGH_THRESHOLD    (_MAD_DMA_READER2_TOTAL_BUF_SIZE - _MAD_DMA_READER2_MIN_AVAIL)
#define _MAD_DMA_READER2_BUF_SIZE    _MAD_DMA_READER2_TOTAL_BUF_SIZE
#define _MAD_DMA_READER2_PERIOD_SIZE    (_MAD_DMA_READER2_BUF_SIZE >> 2)
#define _MAD_DMA_READER2_QUEUE_SIZE    100 /* ms */

#define _MAD_PCM_CAPTURE1_BASE_OFFSET   (0x30000 + _MAD_DSP2_TO_COMMON_DRAM_OFFSET)
#define _MAD_PCM_CAPTURE2_BASE_OFFSET   (0x300000 + _MAD_DSP2_TO_COMMON_DRAM_OFFSET)
#define _MAD_PCM_CAPTURE_BUF_UNIT    128
#define _MAD_PCM_CAPTURE1_BUF_SIZE    0x18000 /* 96 KByte */
#define _MAD_PCM_CAPTURE2_BUF_SIZE    0xC000 /* 48 KByte */

#define _MAD_READ_BYTE(_reg)    (*(volatile unsigned char*)(_reg))
#define _MAD_READ_WORD(_reg)    (*(volatile unsigned short*)(_reg))
#define _MAD_WRITE_BYTE(_reg, _val)    { (*((volatile unsigned char*)(_reg))) = (unsigned char)(_val); }
#define _MAD_WRITE_WORD(_reg, _val)    { (*((volatile unsigned short*)(_reg))) = (unsigned short)(_val); }
#define _MAD_R1BYTE(u32Addr, u8mask)    (_MAD_READ_BYTE (_MAD_PHYSICAL_MEM_BASE + ((u32Addr) << 1) - ((u32Addr) & 1)) & (u8mask))
#define _MAD_AU_AbsReadByte(u32Reg)    (_MAD_READ_BYTE (_MAD_PHYSICAL_MEM_BASE + ((u32Reg) << 1) - ((u32Reg) & 1)))
#define _MAD_AU_AbsRead2Byte(u32Reg)    (_MAD_READ_WORD (_MAD_PHYSICAL_MEM_BASE + ((u32Reg) << 1)) )
#define _MAD_AU_AbsWriteByte(u32Reg, u8Val) \
	do { \
		(_MAD_WRITE_BYTE(_MAD_PHYSICAL_MEM_BASE + ((u32Reg) << 1) - ((u32Reg) & 1), u8Val)); \
	} while(0)
#define _MAD_AU_AbsWriteMaskByte(u32Reg, u8Mask, u8Val) \
	do { \
		(_MAD_WRITE_BYTE(_MAD_PHYSICAL_MEM_BASE + ((u32Reg) << 1) - ((u32Reg) & 1), (_MAD_R1BYTE((u32Reg), 0xFF) & ~(u8Mask)) | ((u8Val) & (u8Mask)))); \
	} while(0)
#define _MAD_AU_AbsWrite2Byte(u32Reg, u16Val) \
	do { \
		(_MAD_WRITE_WORD(_MAD_PHYSICAL_MEM_BASE + ((u32Reg) << 1), u16Val)); \
	} while(0)


/*
 * ============================================================================
 * Forward Declarations
 * ============================================================================
 */
/* Read/Write Register */
#if 0 /* It's not in used for the moment, might be TODO */
static unsigned char _mhal_alsa_read_byte(unsigned int u32RegAddr);
#endif
static unsigned short _mhal_alsa_read_reg(unsigned int u32RegAddr);
#if 0 /* It's not in used for the moment, might be TODO */
static void _mhal_alsa_write_byte(unsigned int u32RegAddr, unsigned char u8Val);
#endif
static void _mhal_alsa_write_mask_byte(unsigned int u32RegAddr, unsigned char u8Mask, unsigned char u8Val);
static void _mhal_alsa_write_reg(unsigned int u32RegAddr, unsigned short u16Val);
static void _mhal_alsa_write_mask_reg(unsigned int u32RegAddr, unsigned short u16Mask, unsigned short u16Val);
static unsigned int _mhal_alsa_get_device_status(void);

/* DMA Reader */
static int _mhal_alsa_dma_reader_init(void);
static int _mhal_alsa_dma_reader_exit(void);
static int _mhal_alsa_dma_reader_start(void);
static int _mhal_alsa_dma_reader_stop(void);
static int _mhal_alsa_dma_reader_resume(void);
static int _mhal_alsa_dma_reader_suspend(void);
static unsigned int _mhal_alsa_dma_reader_write(void* buffer, unsigned int bytes);
static int _mhal_alsa_dma_reader_get(int cmd, unsigned int *param);
static int _mhal_alsa_dma_reader_set(int cmd, unsigned int *param);
static int _mhal_alsa_dma_reader_set_sample_rate(unsigned int sample_rate);
static int _mhal_alsa_dma_reader_set_channel_mode(unsigned int channel_mode);
static int _mhal_alsa_dma_reader_get_inused_lines(void);
static int _mhal_alsa_dma_reader_get_avail_lines(void);

/* DMA Reader2 */
static int _mhal_alsa_dma_reader2_init(void);
static int _mhal_alsa_dma_reader2_exit(void);
static int _mhal_alsa_dma_reader2_start(void);
static int _mhal_alsa_dma_reader2_stop(void);
static int _mhal_alsa_dma_reader2_resume(void);
static int _mhal_alsa_dma_reader2_suspend(void);
static unsigned int _mhal_alsa_dma_reader2_write(void* buffer, unsigned int bytes);
static int _mhal_alsa_dma_reader2_get(int cmd, unsigned int *param);
static int _mhal_alsa_dma_reader2_set(int cmd, unsigned int *param);
static int _mhal_alsa_dma_reader2_set_sample_rate(unsigned int sample_rate);
static int _mhal_alsa_dma_reader2_set_channel_mode(unsigned int channel_mode);
static int _mhal_alsa_dma_reader2_get_inused_lines(void);
static int _mhal_alsa_dma_reader2_get_avail_lines(void);

/* DMA Reader3 */
static int _mhal_alsa_dma_reader3_init(void);
static int _mhal_alsa_dma_reader3_exit(void);
static int _mhal_alsa_dma_reader3_start(void);
static int _mhal_alsa_dma_reader3_stop(void);
static int _mhal_alsa_dma_reader3_resume(void);
static int _mhal_alsa_dma_reader3_suspend(void);
static unsigned int _mhal_alsa_dma_reader3_write(void* buffer, unsigned int bytes);
static int _mhal_alsa_dma_reader3_get(int cmd, unsigned int *param);
static int _mhal_alsa_dma_reader3_set(int cmd, unsigned int *param);
static int _mhal_alsa_dma_reader3_set_sample_rate(unsigned int sample_rate);
static int _mhal_alsa_dma_reader3_set_channel_mode(unsigned int channel_mode);
static int _mhal_alsa_dma_reader3_get_inused_lines(void);
static int _mhal_alsa_dma_reader3_get_avail_lines(void);

/* DMA Reader4 */
static int _mhal_alsa_dma_reader4_init(void);
static int _mhal_alsa_dma_reader4_exit(void);
static int _mhal_alsa_dma_reader4_start(void);
static int _mhal_alsa_dma_reader4_stop(void);
static int _mhal_alsa_dma_reader4_resume(void);
static int _mhal_alsa_dma_reader4_suspend(void);
static unsigned int _mhal_alsa_dma_reader4_write(void* buffer, unsigned int bytes);
static int _mhal_alsa_dma_reader4_get(int cmd, unsigned int *param);
static int _mhal_alsa_dma_reader4_set(int cmd, unsigned int *param);
static int _mhal_alsa_dma_reader4_set_sample_rate(unsigned int sample_rate);
static int _mhal_alsa_dma_reader4_set_channel_mode(unsigned int channel_mode);
static int _mhal_alsa_dma_reader4_get_inused_lines(void);
static int _mhal_alsa_dma_reader4_get_avail_lines(void);

/* PCM Capture1 */
static int _mhal_alsa_pcm_capture1_init(void);
static int _mhal_alsa_pcm_capture1_exit(void);
static int _mhal_alsa_pcm_capture1_start(void);
static int _mhal_alsa_pcm_capture1_stop(void);
static int _mhal_alsa_pcm_capture1_resume(void);
static int _mhal_alsa_pcm_capture1_suspend(void);
static unsigned int _mhal_alsa_pcm_capture1_read(void *buffer, unsigned int bytes);
static int _mhal_alsa_pcm_capture1_get(int cmd, unsigned int *param);
static int _mhal_alsa_pcm_capture1_set(int cmd, unsigned int *param);
static unsigned int _mhal_alsa_pcm_capture1_get_new_avail_bytes(void);
static unsigned int _mhal_alsa_pcm_capture1_get_total_avail_bytes(void);
static int _mhal_alsa_pcm_capture1_set_buffer_size(unsigned int buffer_size);

/* PCM Capture2 */
static int _mhal_alsa_pcm_capture2_init(void);
static int _mhal_alsa_pcm_capture2_exit(void);
static int _mhal_alsa_pcm_capture2_start(void);
static int _mhal_alsa_pcm_capture2_stop(void);
static int _mhal_alsa_pcm_capture2_resume(void);
static int _mhal_alsa_pcm_capture2_suspend(void);
static unsigned int _mhal_alsa_pcm_capture2_read(void *buffer, unsigned int bytes);
static int _mhal_alsa_pcm_capture2_get(int cmd, unsigned int *param);
static int _mhal_alsa_pcm_capture2_set(int cmd, unsigned int *param);
static unsigned int _mhal_alsa_pcm_capture2_get_new_avail_bytes(void);
static unsigned int _mhal_alsa_pcm_capture2_get_total_avail_bytes(void);
static int _mhal_alsa_pcm_capture2_set_buffer_size(unsigned int buffer_size);


/*
 * ============================================================================
 * Local Variables
 * ============================================================================
 */
/* MStar Audio DSP */
static struct MStar_MAD_Info MStar_MAD;

/* Supported Audio Rates by MStar Audio DSP */
static unsigned int mad_rates[] = {
	8000,
	11025,
	12000,
	16000,
	22050,
	24000,
	32000,
	44100,
	48000,
};

/* MStar Audio DSP - DMA Reader */
static struct MStar_DMA_Reader_Struct g_dma_reader = {
	.buffer = {
		.addr = NULL,
		.size = _MAD_DMA_READER_TOTAL_BUF_SIZE,
		.w_ptr = NULL,
		.r_ptr = NULL,
	},
	.str_mode_info = {
		.status = E_STOP,
		.physical_addr = 0,
		.bus_addr = 0,
		.virtual_addr = 0,
	},
	.initialized_status = MAD_FALSE,
	.channel_mode = 0,
	.sample_rate = 0,
	.period_size = _MAD_DMA_READER_PERIOD_SIZE,
	.high_threshold = _MAD_DMA_READER_HIGH_THRESHOLD,
	.remain_size = 0,
	.written_size = 0,
};

static struct MStar_MAD_Ops MStar_DMA_Reader_Ops = {
	.open = _mhal_alsa_dma_reader_init,
	.close = _mhal_alsa_dma_reader_exit,
	.start = _mhal_alsa_dma_reader_start,
	.stop = _mhal_alsa_dma_reader_stop,
	.resume = _mhal_alsa_dma_reader_resume,
	.suspend = _mhal_alsa_dma_reader_suspend,
	.read = NULL,
	.write = _mhal_alsa_dma_reader_write,
	.get = _mhal_alsa_dma_reader_get,
	.set = _mhal_alsa_dma_reader_set,
};

/* MStar Audio DSP - DMA Reader2 */
static struct MStar_DMA_Reader_Struct g_dma_reader2 = {
	.buffer = {
		.addr = NULL,
		.size = _MAD_DMA_READER2_TOTAL_BUF_SIZE,
		.w_ptr = NULL,
		.r_ptr = NULL,
	},
	.str_mode_info = {
		.status = E_STOP,
		.physical_addr = 0,
		.bus_addr = 0,
		.virtual_addr = 0,
	},
	.initialized_status = MAD_FALSE,
	.channel_mode = 0,
	.sample_rate = 0,
	.period_size = _MAD_DMA_READER2_PERIOD_SIZE,
	.high_threshold = _MAD_DMA_READER2_HIGH_THRESHOLD,
	.remain_size = 0,
	.written_size = 0,
};

static struct MStar_MAD_Ops MStar_DMA_Reader2_Ops = {
	.open = _mhal_alsa_dma_reader2_init,
	.close = _mhal_alsa_dma_reader2_exit,
	.start = _mhal_alsa_dma_reader2_start,
	.stop = _mhal_alsa_dma_reader2_stop,
	.resume = _mhal_alsa_dma_reader2_resume,
	.suspend = _mhal_alsa_dma_reader2_suspend,
	.read = NULL,
	.write = _mhal_alsa_dma_reader2_write,
	.get = _mhal_alsa_dma_reader2_get,
	.set = _mhal_alsa_dma_reader2_set,
};

/* MStar Audio DSP - DMA Reader3 */
static struct MStar_DMA_Reader_Struct g_dma_reader3 = {
	.buffer = {
		.addr = NULL,
		.size = _MAD_DMA_READER2_TOTAL_BUF_SIZE,
		.w_ptr = NULL,
		.r_ptr = NULL,
	},
	.str_mode_info = {
		.status = E_STOP,
		.physical_addr = 0,
		.bus_addr = 0,
		.virtual_addr = 0,
	},
	.initialized_status = MAD_FALSE,
	.channel_mode = 0,
	.sample_rate = 0,
	.period_size = _MAD_DMA_READER2_PERIOD_SIZE,
	.high_threshold = _MAD_DMA_READER2_HIGH_THRESHOLD,
	.remain_size = 0,
	.written_size = 0,
};

static struct MStar_MAD_Ops MStar_DMA_Reader3_Ops = {
	.open = _mhal_alsa_dma_reader3_init,
	.close = _mhal_alsa_dma_reader3_exit,
	.start = _mhal_alsa_dma_reader3_start,
	.stop = _mhal_alsa_dma_reader3_stop,
	.resume = _mhal_alsa_dma_reader3_resume,
	.suspend = _mhal_alsa_dma_reader3_suspend,
	.read = NULL,
	.write = _mhal_alsa_dma_reader3_write,
	.get = _mhal_alsa_dma_reader3_get,
	.set = _mhal_alsa_dma_reader3_set,
};

/* MStar Audio DSP - DMA Reader4 */
static struct MStar_DMA_Reader_Struct g_dma_reader4 = {
	.buffer = {
		.addr = NULL,
		.size = _MAD_DMA_READER2_TOTAL_BUF_SIZE,
		.w_ptr = NULL,
		.r_ptr = NULL,
	},
	.str_mode_info = {
		.status = E_STOP,
		.physical_addr = 0,
		.bus_addr = 0,
		.virtual_addr = 0,
	},
	.initialized_status = MAD_FALSE,
	.channel_mode = 0,
	.sample_rate = 0,
	.period_size = _MAD_DMA_READER2_PERIOD_SIZE,
	.high_threshold = _MAD_DMA_READER2_HIGH_THRESHOLD,
	.remain_size = 0,
	.written_size = 0,
};

static struct MStar_MAD_Ops MStar_DMA_Reader4_Ops = {
	.open = _mhal_alsa_dma_reader4_init,
	.close = _mhal_alsa_dma_reader4_exit,
	.start = _mhal_alsa_dma_reader4_start,
	.stop = _mhal_alsa_dma_reader4_stop,
	.resume = _mhal_alsa_dma_reader4_resume,
	.suspend = _mhal_alsa_dma_reader4_suspend,
	.read = NULL,
	.write = _mhal_alsa_dma_reader4_write,
	.get = _mhal_alsa_dma_reader4_get,
	.set = _mhal_alsa_dma_reader4_set,
};

/* MStar Audio DSP - PCM Capture1 */
static struct MStar_PCM_Capture_Struct g_pcm_capture1 = {
	.buffer = {
		.addr = NULL,
		.size = _MAD_PCM_CAPTURE1_BUF_SIZE,
		.w_ptr = NULL,
		.r_ptr = NULL,
	},
	.str_mode_info = {
		.status = E_STOP,
		.physical_addr = 0,
		.bus_addr = 0,
		.virtual_addr = 0,
	},
	.initialized_status = MAD_FALSE,
	.channel_mode = 2,
	.sample_rate = 48000,
};

static struct MStar_MAD_Ops MStar_PCM_Capture1_Ops = {
	.open = _mhal_alsa_pcm_capture1_init,
	.close = _mhal_alsa_pcm_capture1_exit,
	.start = _mhal_alsa_pcm_capture1_start,
	.stop = _mhal_alsa_pcm_capture1_stop,
	.resume = _mhal_alsa_pcm_capture1_resume,
	.suspend = _mhal_alsa_pcm_capture1_suspend,
	.read = _mhal_alsa_pcm_capture1_read,
	.write = NULL,
	.get = _mhal_alsa_pcm_capture1_get,
	.set = _mhal_alsa_pcm_capture1_set,
};

/* MStar Audio DSP - PCM Capture2 */
static struct MStar_PCM_Capture_Struct g_pcm_capture2 = {
	.buffer = {
		.addr = NULL,
		.size = _MAD_PCM_CAPTURE2_BUF_SIZE,
		.w_ptr = NULL,
		.r_ptr = NULL,
	},
	.str_mode_info = {
		.status = E_STOP,
		.physical_addr = 0,
		.bus_addr = 0,
		.virtual_addr = 0,
	},
	.initialized_status = MAD_FALSE,
	.channel_mode = 2,
	.sample_rate = 48000,
};

static struct MStar_MAD_Ops MStar_PCM_Capture2_Ops = {
	.open = _mhal_alsa_pcm_capture2_init,
	.close = _mhal_alsa_pcm_capture2_exit,
	.start = _mhal_alsa_pcm_capture2_start,
	.stop = _mhal_alsa_pcm_capture2_stop,
	.resume = _mhal_alsa_pcm_capture2_resume,
	.suspend = _mhal_alsa_pcm_capture2_suspend,
	.read = _mhal_alsa_pcm_capture2_read,
	.write = NULL,
	.get = _mhal_alsa_pcm_capture2_get,
	.set = _mhal_alsa_pcm_capture2_set,
};

static ptrdiff_t g_pcm_dmaRdr1_base_va = 0;
static ptrdiff_t g_pcm_dmaRdr2_base_va = 0;
static ptrdiff_t g_pcm_dmaRdr3_base_va = 0;
static ptrdiff_t g_pcm_dmaRdr4_base_va = 0;
static ptrdiff_t g_pcm_capture1_base_va = 0;
static ptrdiff_t g_pcm_capture2_base_va = 0;

/*
 * ============================================================================
 * Function Implementations
 * ============================================================================
 */
#if 0 /* It's not in used for the moment, might be TODO */
static unsigned char _mhal_alsa_read_byte(unsigned int u32RegAddr)
{
	return (_MAD_AU_AbsReadByte(u32RegAddr+_MAD_MAILBOX_OFFSET));
}
#endif

static unsigned short _mhal_alsa_read_reg(unsigned int u32RegAddr)
{
	return (_MAD_AU_AbsRead2Byte(u32RegAddr+_MAD_MAILBOX_OFFSET));
}

#if 0 /* It's not in used for the moment, might be TODO */
static void _mhal_alsa_write_byte(unsigned int u32RegAddr, unsigned char u8Val)
{
	_MAD_AU_AbsWriteByte((u32RegAddr+_MAD_MAILBOX_OFFSET), u8Val);
}
#endif

static void _mhal_alsa_write_mask_byte(unsigned int u32RegAddr, unsigned char u8Mask, unsigned char u8Val)
{
	_MAD_AU_AbsWriteMaskByte((u32RegAddr+_MAD_MAILBOX_OFFSET), u8Mask, u8Val);
}

static void _mhal_alsa_write_reg(unsigned int u32RegAddr, unsigned short u16Val)
{
	_MAD_AU_AbsWrite2Byte((u32RegAddr+_MAD_MAILBOX_OFFSET), u16Val);
}

static void _mhal_alsa_write_mask_reg(unsigned int u32RegAddr, unsigned short u16Mask, unsigned short u16Val)
{
	unsigned short u16RegVal;

	u16RegVal = _mhal_alsa_read_reg(u32RegAddr);
	u16RegVal = ((u16RegVal & (~(u16Mask))) | (u16Val & u16Mask));
	_mhal_alsa_write_reg(u32RegAddr, u16RegVal);
}

#if 0 /* It's not in used for the moment, might be TODO */
static unsigned char _mhal_alsa_read2_byte(unsigned int u32RegAddr)
{
	return (_MAD_AU_AbsReadByte(u32RegAddr+_MAD_MAILBOX_OFFSET));
}
#endif

static unsigned short _mhal_alsa_read2_reg(unsigned int u32RegAddr)
{
	return (_MAD_AU_AbsRead2Byte(u32RegAddr+_MAD_MAILBOX_OFFSET2));
}

#if 0 /* It's not in used for the moment, might be TODO */
static void _mhal_alsa_write2_byte(unsigned int u32RegAddr, unsigned char u8Val)
{
	_MAD_AU_AbsWriteByte((u32RegAddr+_MAD_MAILBOX_OFFSET), u8Val);
}

static void _mhal_alsa_write2_mask_byte(unsigned int u32RegAddr, unsigned char u8Mask, unsigned char u8Val)
{
	_MAD_AU_AbsWriteMaskByte((u32RegAddr+_MAD_MAILBOX_OFFSET2), u8Mask, u8Val);
}
#endif

static void _mhal_alsa_write2_reg(unsigned int u32RegAddr, unsigned short u16Val)
{
	_MAD_AU_AbsWrite2Byte((u32RegAddr+_MAD_MAILBOX_OFFSET2), u16Val);
}

static void _mhal_alsa_write2_mask_reg(unsigned int u32RegAddr, unsigned short u16Mask, unsigned short u16Val)
{
	unsigned short u16RegVal;

	u16RegVal = _mhal_alsa_read2_reg(u32RegAddr);
	u16RegVal = ((u16RegVal & (~(u16Mask))) | (u16Val & u16Mask));
	_mhal_alsa_write2_reg(u32RegAddr, u16RegVal);
}


static unsigned int _mhal_alsa_get_device_status(void)
{
	if (((_mhal_alsa_read_reg(0x2D30) & 0x0200) == 0x0200) || (_mhal_alsa_read_reg(0x2A90) == 0x0000)) {
		return MAD_FALSE;
	}
	else {
		return MAD_TRUE;
	}
}

/* Initiate DMA Reader */
static int _mhal_alsa_dma_reader_init(void)
{
	struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader;
	unsigned int audio_pcm_dmaRdr_bufSz = 0;  /* must be multiple of DMA_RDR_PCM_BUF_UNIT*2 (= 0x2000) */
	ptrdiff_t audio_pcm_dmaRdr_base_pa = 0; /* DMA Reader Input buffer (DM_Prefetch) */
	ptrdiff_t audio_pcm_dmaRdr_base_ba = 0;
	ptrdiff_t audio_pcm_dmaRdr_base_va = 0;
	MAD_PRINT(KERN_INFO "Initiate MStar PCM Playback1 engine\n");

	if ((dma_reader->initialized_status != MAD_TRUE) || (dma_reader->str_mode_info.status != E_RESUME)) {
		audio_pcm_dmaRdr_bufSz = dma_reader->buffer.size;
		audio_pcm_dmaRdr_base_pa = ((ptrdiff_t)_mhal_alsa_read_reg(0x2A90) * _MAD_ADDR_CONVERTOR) + _MAD_DMA_READER_BASE_OFFSET;
		audio_pcm_dmaRdr_base_pa += (((_mhal_alsa_read_reg(0x2AC0) & 0x0F) << 16) * _MAD_ADDR_CONVERTOR);
		audio_pcm_dmaRdr_base_ba = audio_pcm_dmaRdr_base_pa + _MAD_PHYSICAL_ADDR_TO_BUS_ADDR;

		if ((audio_pcm_dmaRdr_base_ba % 0x1000)) {
			MAD_PRINT(KERN_ERR "Error! Invalid MStar PCM reader bus address, it should be aligned by 4 KB!\n");
			return -EFAULT;
		}

		/* convert Bus Address to Virtual Address */
		if (g_pcm_dmaRdr1_base_va == 0) {
			g_pcm_dmaRdr1_base_va = (ptrdiff_t)ioremap_nocache(audio_pcm_dmaRdr_base_ba, audio_pcm_dmaRdr_bufSz);
			if (g_pcm_dmaRdr1_base_va == 0) {
				MAD_PRINT(KERN_ERR "Error! fail to convert PCM Playback1 Buffer Bus Address to Virtual Address\n");
				return -ENOMEM;
			}
		}
		audio_pcm_dmaRdr_base_va = g_pcm_dmaRdr1_base_va;

		dma_reader->str_mode_info.physical_addr = audio_pcm_dmaRdr_base_pa;
		dma_reader->str_mode_info.bus_addr = audio_pcm_dmaRdr_base_ba;
		dma_reader->str_mode_info.virtual_addr = audio_pcm_dmaRdr_base_va;

		dma_reader->initialized_status = MAD_TRUE;
	}
	else {
		audio_pcm_dmaRdr_bufSz = dma_reader->buffer.size;
		audio_pcm_dmaRdr_base_pa = dma_reader->str_mode_info.physical_addr;
		audio_pcm_dmaRdr_base_ba = dma_reader->str_mode_info.bus_addr;
		audio_pcm_dmaRdr_base_va = dma_reader->str_mode_info.virtual_addr;
	}

	/* init DMA writer address */
	dma_reader->buffer.addr = (unsigned char *)audio_pcm_dmaRdr_base_va;
	dma_reader->buffer.w_ptr = dma_reader->buffer.addr;
	//MAD_PRINT(KERN_INFO "PCM Playback1 buffer start address = 0x%08X\n", dma_reader->buffer.addr);
	//MAD_PRINT(KERN_INFO "PCM Playback1 buffer end address = 0x%08X\n", (dma_reader->buffer.addr + dma_reader->buffer.size));

	/* initial DMA Reader path & clk select (DMA reader -> CH8) */
	_mhal_alsa_write_mask_byte(0x2CB0, 0x07, 0x04); /* SEL_CLK_DMA_READER */
	_mhal_alsa_write_mask_byte(0x2C6B, 0xFF, 0x8F); /* CH8 sel to DMA Rdr */

	_mhal_alsa_write_mask_reg(0x2B82, 0xFFFF, 0x0000); /* reset dma reader */
	_mhal_alsa_write_mask_byte(0x2B8E, 0xFF, ((audio_pcm_dmaRdr_base_pa / _MAD_BYTES_IN_LINE) & 0xFF)); /* RD_BASE_ADDR[7:0] */
	_mhal_alsa_write_mask_reg(0x2B84, 0xFFFF, (((audio_pcm_dmaRdr_base_pa / _MAD_BYTES_IN_LINE) >> 8) & 0xFFFF)); /* RD_BASE_ADDR[23:8] */
	_mhal_alsa_write_mask_byte(0x2B8F, 0x0F, (((audio_pcm_dmaRdr_base_pa / _MAD_BYTES_IN_LINE) >> 24) & 0x0F)); /* RD_BASE_ADDR[27:24] */
	_mhal_alsa_write_mask_reg(0x2B86, 0xFFFF, (audio_pcm_dmaRdr_bufSz / _MAD_BYTES_IN_LINE)); /* setting : DMA Reader Size */
	_mhal_alsa_write_mask_reg(0x2B88, 0xFFFF, (audio_pcm_dmaRdr_bufSz - 12)); /* setting : DMA Reader Overrun Thr */
	_mhal_alsa_write_mask_reg(0x2B8A, 0xFFFF, 0x0012); /* setting : DMA Reader Underrun Thr */

	/* reset and start DMA Reader */
	_mhal_alsa_write_mask_reg(0x2B80, 0xFFFF, 0x6402);
	_mhal_alsa_write_mask_reg(0x2B80, 0xFFFF, 0x2402);

	/* reset remain size */
	dma_reader->remain_size = 0;

	/* reset written size */
	dma_reader->written_size = 0;

	/* clear DMA reader buffer */
	memset((void *)dma_reader->buffer.addr, 0x00, dma_reader->buffer.size);
	Chip_Flush_Memory();

	return 0;
}

/* Exit DMA Reader */
static int _mhal_alsa_dma_reader_exit(void)
{
	struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader;
	MAD_PRINT(KERN_INFO "Exit MStar PCM Playback1 engine\n");

	if (g_pcm_dmaRdr1_base_va != 0) {
		if (dma_reader->buffer.addr) {
			iounmap((void *)dma_reader->buffer.addr);
			dma_reader->buffer.addr = 0;
		}
		else {
			MAD_PRINT(KERN_ERR "Error! MStar PCM Playback1 buffer address should not be 0 !\n");
		}

		g_pcm_dmaRdr1_base_va = 0;
	}

	dma_reader->str_mode_info.status = E_STOP;
	dma_reader->initialized_status = MAD_FALSE;

	return 0;
}

/* Start DMA Reader */
static int _mhal_alsa_dma_reader_start(void)
{
	struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader;
	//MAD_PRINT(KERN_INFO "Start MStar PCM Playback1 engine\n");

	/* Initial DMA Reader path & clk select (DMA reader -> CH8) */
	_mhal_alsa_write_mask_byte(0x2CB0, 0x07, 0x04); /* SEL_CLK_DMA_READER */
	_mhal_alsa_write_mask_byte(0x2C6B, 0xFF, 0x8F); /* CH8 sel to DMA Rdr */

	_mhal_alsa_write_mask_reg(0x2B80, 0x0001, 0x0001);
	_mhal_alsa_write_mask_reg(0x2B80, 0x0100, 0x0100);

	dma_reader->str_mode_info.status = E_START;

	return 0;
}

/* Stop DMA Reader */
static int _mhal_alsa_dma_reader_stop(void)
{
	struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader;
	//MAD_PRINT(KERN_INFO "Stop MStar PCM Playback1 engine\n");

	/* clear wr cntrs */
	_mhal_alsa_write_mask_reg(0x2B82, 0xFFFF, 0x0000);

	/* reset and start DMA Reader */
	_mhal_alsa_write_mask_reg(0x2B80, 0xFFFE, 0x6402);
	_mhal_alsa_write_mask_reg(0x2B80, 0xFFFE, 0x2402);

	/* reset Write Pointer */
	dma_reader->buffer.w_ptr = dma_reader->buffer.addr;

	/* reset remain size */
	dma_reader->remain_size = 0;

	/* reset written size */
	dma_reader->written_size = 0;

	_mhal_alsa_write_mask_reg(0x2B80, 0x0001, 0x0000);
	_mhal_alsa_write_mask_reg(0x2B80, 0x0100, 0x0000);

	return 0;
}

/* Resume DMA Reader */
static int _mhal_alsa_dma_reader_resume(void)
{
	struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader;
	//MAD_PRINT(KERN_INFO "Resume MStar PCM Playback1 engine\n");

	dma_reader->str_mode_info.status = E_RESUME;

	return 0;
}

/* Suspend DMA Reader */
static int _mhal_alsa_dma_reader_suspend(void)
{
	struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader;
	//MAD_PRINT(KERN_INFO "Suspend MStar PCM Playback1 engine\n");

	dma_reader->str_mode_info.status = E_SUSPEND;

	return 0;
}

/* Write PCM to DMA Reader */
static unsigned int _mhal_alsa_dma_reader_write(void *buffer, unsigned int bytes)
{
	struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader;
	bool replay_flag = MAD_FALSE;
	unsigned char *bufptr = (unsigned char *)buffer;
	unsigned char tmp_bufptr1 = 0;
	unsigned char tmp_bufptr2 = 0;
	int loop = 0;
	int inused_lines = 0;
	unsigned int copy_lr_sample = 0;
	unsigned int copy_size = 0;

	inused_lines = _mhal_alsa_dma_reader_get_inused_lines();
	if (inused_lines <= 6) {
		if ((_mhal_alsa_read_reg(0x2B80) & 0x0001) == 0x0001)
			replay_flag = MAD_TRUE;

		/* clear wr cntrs */
		_mhal_alsa_write_mask_reg(0x2B82, 0xFFFF, 0x0000);

		/* reset and start DMA Reader */
		_mhal_alsa_write_mask_reg(0x2B80, 0xFFFE, 0x6402);
		_mhal_alsa_write_mask_reg(0x2B80, 0xFFFE, 0x2402);

		/* reset Write Pointer */
		dma_reader->buffer.w_ptr = dma_reader->buffer.addr;

		/* reset remain size */
		dma_reader->remain_size = 0;

		/* reset written size */
		dma_reader->written_size = 0;

		//MAD_PRINT(KERN_INFO "***** PCM Playback1 Buffer empty !! ***** \n");

		inused_lines = _mhal_alsa_dma_reader_get_inused_lines();
	}

	copy_lr_sample = bytes / 2; /* L + R samples */
	copy_size = (dma_reader->channel_mode == E_MONO) ? (bytes * 4) : (bytes * 2);

	/* copy data to DMA Reader buffer */
	if ( ((inused_lines * _MAD_BYTES_IN_LINE) + copy_size) < dma_reader->high_threshold) {
		if (dma_reader->channel_mode == E_MONO) {
			for (loop = 0; loop < copy_lr_sample; loop++) {
				tmp_bufptr1 = *bufptr++;
				tmp_bufptr2 = *bufptr++;

				*(dma_reader->buffer.w_ptr++) = 0;
				*(dma_reader->buffer.w_ptr++) = tmp_bufptr1;
				*(dma_reader->buffer.w_ptr++) = tmp_bufptr2;
				*(dma_reader->buffer.w_ptr++) = 0;
				*(dma_reader->buffer.w_ptr++) = 0;
				*(dma_reader->buffer.w_ptr++) = tmp_bufptr1;
				*(dma_reader->buffer.w_ptr++) = tmp_bufptr2;
				*(dma_reader->buffer.w_ptr++) = 0;

				if (dma_reader->buffer.w_ptr >= (dma_reader->buffer.addr + dma_reader->buffer.size))
					dma_reader->buffer.w_ptr = dma_reader->buffer.addr;
			}
		}
		else {
			for (loop = 0; loop < copy_lr_sample; loop++) {
				*(dma_reader->buffer.w_ptr++) = 0;
				*(dma_reader->buffer.w_ptr++) = *bufptr++;
				*(dma_reader->buffer.w_ptr++) = *bufptr++;
				*(dma_reader->buffer.w_ptr++) = 0;

				if (dma_reader->buffer.w_ptr >= (dma_reader->buffer.addr + dma_reader->buffer.size))
					dma_reader->buffer.w_ptr = dma_reader->buffer.addr;
			}
		}

		/* flush MIU */
		Chip_Flush_Memory();

		/* update copied size to DMA Reader */
		copy_size += dma_reader->remain_size;
		_mhal_alsa_write_mask_reg(0x2B82, 0xFFFF, (copy_size / _MAD_BYTES_IN_LINE));
		dma_reader->remain_size = copy_size % _MAD_BYTES_IN_LINE;
		dma_reader->written_size += ((copy_size - dma_reader->remain_size) >> 1);

		if (replay_flag == MAD_TRUE) {
			_mhal_alsa_write_mask_reg(0x2B80, 0x0001, 0x0001);
			_mhal_alsa_write_mask_reg(0x2B80, 0x0100, 0x0100);
		}

		return bytes;
	}

	//MAD_PRINT(KERN_INFO "***** PCM Playback1 Buffer busy !! ***** \n");

	return 0;
}

/* Get information from DMA Reader */
static int _mhal_alsa_dma_reader_get(int cmd, unsigned int *param)
{
	int err = 0;
	//MAD_PRINT(KERN_INFO "Get parameter from PCM Playback1\n");

	switch(cmd) {
		case E_PCM_PLAYBACK_GET_BUFFER_SIZE:
		{
			struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader;

			*param = dma_reader->buffer.size >> 1; /* Only return valid bytes */
			break;
		}

		case E_PCM_PLAYBACK_GET_PERIOD_SIZE:
		{
			struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader;

			*param = dma_reader->period_size;
			break;
		}

		case E_PCM_PLAYBACK_GET_SAMPLE_RATE:
		{
			struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader;

			*param = dma_reader->sample_rate;
			break;
		}

		case E_PCM_PLAYBACK_GET_CHANNEL_MODE:
		{
			struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader;

			*param = dma_reader->channel_mode;
			break;
		}

		case E_PCM_PLAYBACK_GET_MAX_CHANNEL:
		{
			*param = _MAD_MAX_CHANNEL;
			break;
		}

		case E_PCM_PLAYBACK_GET_CONSTRAINTS_COUNT:
		{
			*param = sizeof(mad_rates) / sizeof(mad_rates[0]);
			break;
		}

		case E_PCM_PLAYBACK_GET_CONSTRAINTS_LIST:
		{
			*param = (ptrdiff_t)&mad_rates;
			break;
		}

		case E_PCM_PLAYBACK_GET_CONSTRAINTS_MASK:
		{
			*param = 0;
			break;
		}

		case E_PCM_PLAYBACK_GET_BUFFER_INUSED_BYTES:
		{
			*param = (_mhal_alsa_dma_reader_get_inused_lines() * _MAD_BYTES_IN_LINE) >> 1; /* Only return valid bytes */
			break;
		}

		case E_PCM_PLAYBACK_GET_BUFFER_AVAIL_BYTES:
		{
			*param = (_mhal_alsa_dma_reader_get_avail_lines() * _MAD_BYTES_IN_LINE) >> 1; /* Only return valid bytes */
			break;
		}

		case E_PCM_PLAYBACK_GET_BUFFER_CONSUMED_BYTES:
		{
			struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader;
			int inused_bytes = 0;
			int consumed_bytes = 0;

			inused_bytes = (_mhal_alsa_dma_reader_get_inused_lines() * _MAD_BYTES_IN_LINE) >> 1; /* Only return valid bytes */
			consumed_bytes = dma_reader->written_size - inused_bytes;
			*param = consumed_bytes;
			dma_reader->written_size = inused_bytes;
			break;
		}

		case E_PCM_PLAYBACK_GET_STR_STATUS:
		{
			struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader;

			*param = dma_reader->str_mode_info.status;
			break;
		}

		case E_PCM_PLAYBACK_GET_DEVICE_STATUS:
		{
			*param = _mhal_alsa_get_device_status();
			break;
		}

		default:
		{
			MAD_PRINT(KERN_INFO "Invalid GET command %d\n", cmd);
			err = -EINVAL;
			break;
		}
	}

	return err;
}

/* Set information to DMA Reader */
static int _mhal_alsa_dma_reader_set(int cmd, unsigned int *param)
{
	int err = 0;
	//MAD_PRINT(KERN_INFO "Set parameter to PCM Playback1\n");

	switch(cmd) {
		case E_PCM_PLAYBACK_SET_SAMPLE_RATE:
		{
			_mhal_alsa_dma_reader_set_sample_rate(*param);
			break;
		}

		case E_PCM_PLAYBACK_SET_CHANNEL_MODE:
		{
			_mhal_alsa_dma_reader_set_channel_mode(*param);
			break;
		}

		default:
		{
			MAD_PRINT(KERN_INFO "Invalid SET command %d\n", cmd);
			err = -EINVAL;
			break;
		}
	}

	return err;
}

/* Get DMA Reader PCM buffer level */
static int _mhal_alsa_dma_reader_get_inused_lines(void)
{
	int inused_lines = 0;

	/* Mask LEVEL_CNT_MASK before read */
	_mhal_alsa_write_mask_reg(0x2B80, 0x0010, 0x0010);
	inused_lines = _mhal_alsa_read_reg(0x2B8C);
	_mhal_alsa_write_mask_reg(0x2B80, 0x0010, 0x0000);

	return inused_lines;
}

/* Get DMA Reader PCM avail bytes */
static int _mhal_alsa_dma_reader_get_avail_lines(void)
{
	struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader;
	int inused_lines = 0;
	int avail_lines = 0;

	inused_lines = _mhal_alsa_dma_reader_get_inused_lines();
	avail_lines = (dma_reader->high_threshold / _MAD_BYTES_IN_LINE) - inused_lines;
	if (avail_lines < 0) {
		MAD_PRINT(KERN_ERR "Error! Incorrect inused lines %d!\n", inused_lines);
		avail_lines = 0;
	}

	return avail_lines;
}

/* Set smaple rate to DMA Reader */
static int _mhal_alsa_dma_reader_set_sample_rate(unsigned int sample_rate)
{
	struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader;
	unsigned short synthrate, divisor;
	MAD_PRINT(KERN_INFO "Target sample rate is %u\n", sample_rate);

	dma_reader->sample_rate = sample_rate;

	/* New DMA Reader setting
	 * Formula is :
	 * Synthesizer Rate = 216MHz / Divisor(1, 2, 4 or 8) * 1024 / 256 / Sampling Rate(32Khz, 44.1KHz or 48KHz)
	 */
	switch(sample_rate) {
		case 8000:
		{
			divisor = 2;
			synthrate = 0x6978;
			break;
		}

		case 11025:
		{
			divisor = 2;
			synthrate = 0x4C87;
			break;
		}

		case 12000:
		{
			divisor = 2;
			synthrate = 0x4650;
			break;
		}

		case 16000:
		{
			divisor = 1;
			synthrate = 0x6978;
			break;
		}

		case 22050:
		{
			divisor = 1;
			synthrate = 0x4C87;
			break;
		}

		case 24000:
		{
			divisor = 1;
			synthrate = 0x4650;
			break;
		}

		case 32000:
		{
			divisor = 0;
			synthrate = 0x6978;
			break;
		}

		case 44100:
		{
			divisor = 0;
			synthrate = 0x4C87;
			break;
		}

		case 48000:
		{
			divisor = 0;
			synthrate = 0x4650;
			break;
		}

		default:
		{
			MAD_PRINT(KERN_ERR "Error! un-supported sample rate %u !!!\n", sample_rate);
			divisor = 0;
			synthrate = 0x4650;
			dma_reader->sample_rate = 48000;
			break;
		}
	}

	/* synthersizer setting update (DMA reader (DVB4 clock) -> CH8) */
	_mhal_alsa_write_mask_byte(0x2CA4, 0x20, 0x20); /* enable DMA synthesizer */
	_mhal_alsa_write_mask_reg(0x2CB0, 0x6000, (divisor << 13)); /* Configure REG_SEL_CLK_214MHZ_DMA_RD_FIX_SYNTH */
	_mhal_alsa_write_mask_reg(0x2C20, 0xFFFF, synthrate); /* DMA synthesizer N.F. */

	return 0;
}

/* Set channel mode to DMA Reader */
static int _mhal_alsa_dma_reader_set_channel_mode(unsigned int channel_mode)
{
	struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader;
	unsigned int buffer_size = 0;
	MAD_PRINT(KERN_INFO "Target channel mode is %u\n", channel_mode);

	dma_reader->channel_mode = channel_mode;
	buffer_size = ((dma_reader->sample_rate << dma_reader->channel_mode) * _MAD_DMA_READER_QUEUE_SIZE) / 1000;
	if ((buffer_size % _MAD_BYTES_IN_LINE))
		buffer_size += (_MAD_BYTES_IN_LINE - (buffer_size % _MAD_BYTES_IN_LINE));

	dma_reader->buffer.size = (dma_reader->channel_mode == E_MONO) ? (buffer_size * 4) : (buffer_size * 2);
	dma_reader->period_size = dma_reader->buffer.size >> 2;
	dma_reader->high_threshold = dma_reader->buffer.size - (dma_reader->buffer.size >> 3);

	return 0;
}

/* Initiate DMA Reader2 */
static int _mhal_alsa_dma_reader2_init(void)
{
	struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader2;
	unsigned int audio_pcm_dmaRdr_bufSz = 0;  /* must be multiple of DMA_RDR_PCM_BUF_UNIT*2 (= 0x2000) */
	ptrdiff_t audio_pcm_dmaRdr_base_pa = 0; /* DMA Reader2 Input buffer (DM_Prefetch) */
	ptrdiff_t audio_pcm_dmaRdr_base_ba = 0;
	ptrdiff_t audio_pcm_dmaRdr_base_va = 0;
	MAD_PRINT(KERN_INFO "Initiate MStar PCM Playback2 engine\n");

	if ((dma_reader->initialized_status != MAD_TRUE) || (dma_reader->str_mode_info.status != E_RESUME)) {
		audio_pcm_dmaRdr_bufSz = dma_reader->buffer.size;
		audio_pcm_dmaRdr_base_pa = ((ptrdiff_t)_mhal_alsa_read_reg(0x2A90) * _MAD_ADDR_CONVERTOR) + _MAD_DMA_READER2_BASE_OFFSET;
		audio_pcm_dmaRdr_base_pa += (((_mhal_alsa_read_reg(0x2AC0) & 0x0F) << 16) * _MAD_ADDR_CONVERTOR);
		audio_pcm_dmaRdr_base_ba = audio_pcm_dmaRdr_base_pa + _MAD_PHYSICAL_ADDR_TO_BUS_ADDR;

		if ((audio_pcm_dmaRdr_base_ba % 0x1000)) {
			MAD_PRINT(KERN_ERR "Error! Invalid MStar PCM reader bus address, it should be aligned by 4 KB!\n");
			return -EFAULT;
		}

		/* convert Bus Address to Virtual Address */
		if (g_pcm_dmaRdr2_base_va == 0)	{
			g_pcm_dmaRdr2_base_va = (ptrdiff_t)ioremap_nocache(audio_pcm_dmaRdr_base_ba, audio_pcm_dmaRdr_bufSz);
			if (g_pcm_dmaRdr2_base_va == 0) {
				MAD_PRINT(KERN_ERR "Error! fail to convert PCM Playback2 Buffer Bus Address to Virtual Address\n");
				return -ENOMEM;
			}
		}
		audio_pcm_dmaRdr_base_va = g_pcm_dmaRdr2_base_va;

		dma_reader->str_mode_info.physical_addr = audio_pcm_dmaRdr_base_pa;
		dma_reader->str_mode_info.bus_addr = audio_pcm_dmaRdr_base_ba;
		dma_reader->str_mode_info.virtual_addr = audio_pcm_dmaRdr_base_va;

		dma_reader->initialized_status = MAD_TRUE;
	}
	else {
		audio_pcm_dmaRdr_bufSz = dma_reader->buffer.size;
		audio_pcm_dmaRdr_base_pa = dma_reader->str_mode_info.physical_addr;
		audio_pcm_dmaRdr_base_ba = dma_reader->str_mode_info.bus_addr;
		audio_pcm_dmaRdr_base_va = dma_reader->str_mode_info.virtual_addr;
	}

	/* init DMA writer address */
	dma_reader->buffer.addr = (unsigned char *)audio_pcm_dmaRdr_base_va;
	dma_reader->buffer.w_ptr = dma_reader->buffer.addr;
	//MAD_PRINT(KERN_INFO "PCM Playback2 buffer start address = 0x%08X\n", dma_reader->buffer.addr);
	//MAD_PRINT(KERN_INFO "PCM Playback2 buffer end address = 0x%08X\n", (dma_reader->buffer.addr + dma_reader->buffer.size));

	/* initial DMA Reader2 */
	_mhal_alsa_write2_reg(0x3E40, 0x0);
	_mhal_alsa_write2_reg(0x3E42, ((audio_pcm_dmaRdr_base_pa /_MAD_BYTES_IN_LINE) & 0xFFFF));
	_mhal_alsa_write2_reg(0x3E44, (((audio_pcm_dmaRdr_base_pa /_MAD_BYTES_IN_LINE) >> 16) & 0xFFFF));
	_mhal_alsa_write2_reg(0x3E46, (audio_pcm_dmaRdr_bufSz /_MAD_BYTES_IN_LINE)); /* setting : DMA Reader2 Size */
	_mhal_alsa_write2_mask_reg(0x3E4A, 0xFFFF, 0x00012); /* setting : DMA Reader2 Underrun Thr */

	/* reset and start DMA Reader2 */
	_mhal_alsa_write2_mask_reg(0x3E40, 0xFFFF, 0x800B);
	_mhal_alsa_write2_mask_reg(0x3E40, 0xFFFF, 0x000B);

	/* reset remain size */
	dma_reader->remain_size = 0;

	/* reset written size */
	dma_reader->written_size = 0;

	/* clear DMA reader buffer */
	memset((void *)dma_reader->buffer.addr, 0x00, dma_reader->buffer.size);
	Chip_Flush_Memory();

	return 0;
}

/* Exit DMA Reader2 */
static int _mhal_alsa_dma_reader2_exit(void)
{
	struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader2;
	MAD_PRINT(KERN_INFO "Exit MStar PCM Playback2 engine\n");

	if (g_pcm_dmaRdr2_base_va != 0) {
		if (dma_reader->buffer.addr) {
			iounmap((void *)dma_reader->buffer.addr);
			dma_reader->buffer.addr = 0;
		}
		else {
			MAD_PRINT(KERN_ERR "Error! MStar PCM Playback2 buffer address should not be 0 !\n");
		}

		g_pcm_dmaRdr2_base_va = 0;
	}

	dma_reader->str_mode_info.status = E_STOP;
	dma_reader->initialized_status = MAD_FALSE;

	return 0;
}

/* Start DMA Reader2 */
static int _mhal_alsa_dma_reader2_start(void)
{
	struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader2;
	//MAD_PRINT(KERN_INFO "Start MStar PCM Playback2 engine\n");

	_mhal_alsa_write_mask_byte(0x2C67, 0x9F, 0x90); /* CH6 sel to HW DMA Reader2 */

	dma_reader->str_mode_info.status = E_START;

	return 0;
}

/* Stop DMA Reader2 */
static int _mhal_alsa_dma_reader2_stop(void)
{
	struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader2;
	//MAD_PRINT(KERN_INFO "Stop MStar PCM Playback2 engine\n");

	/* clear wr cntrs */
	_mhal_alsa_write2_mask_reg(0x3E48, 0xFFFF, 0x0000);

	/* reset and start DMA Reader2 */
	_mhal_alsa_write2_mask_reg(0x3E40, 0xFFFF, 0x800B);
	_mhal_alsa_write2_mask_reg(0x3E40, 0xFFFF, 0x000B);

	/* reset Write Pointer */
	dma_reader->buffer.w_ptr = dma_reader->buffer.addr;

	/* reset remain size */
	dma_reader->remain_size = 0;

	/* reset written size */
	dma_reader->written_size = 0;

	return 0;
}

/* Resume DMA Reader2 */
static int _mhal_alsa_dma_reader2_resume(void)
{
	struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader2;
	//MAD_PRINT(KERN_INFO "Resume MStar PCM Playback2 engine\n");

	dma_reader->str_mode_info.status = E_RESUME;

	return 0;
}

/* Suspend DMA Reader2 */
static int _mhal_alsa_dma_reader2_suspend(void)
{
	struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader2;
	//MAD_PRINT(KERN_INFO "Suspend MStar PCM Playback2 engine\n");

	dma_reader->str_mode_info.status = E_SUSPEND;

	return 0;
}

/* Write PCM to DMA Reader2 */
static unsigned int _mhal_alsa_dma_reader2_write(void *buffer, unsigned int bytes)
{
	struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader2;
	bool replay_flag = MAD_FALSE;
	unsigned char *bufptr = (unsigned char *)buffer;
	unsigned char tmp_bufptr1 = 0;
	unsigned char tmp_bufptr2 = 0;
	int loop = 0;
	int inused_lines = 0;
	unsigned int copy_lr_sample = 0;
	unsigned int copy_size = 0;

	inused_lines = _mhal_alsa_dma_reader2_get_inused_lines();
	if (inused_lines <= 6) {
		if ((_mhal_alsa_read2_reg(0x3E56) & 0x0002) == 0x0002)
			replay_flag = MAD_TRUE;

		/* clear wr cntrs */
		_mhal_alsa_write2_mask_reg(0x3E48, 0xFFFF, 0x0000);

		/* reset and start DMA Reader2 */
		_mhal_alsa_write2_mask_reg(0x3E40, 0xFFFF, 0x800B);
		_mhal_alsa_write2_mask_reg(0x3E40, 0xFFFF, 0x000B);

		/* reset Write Pointer */
		dma_reader->buffer.w_ptr = dma_reader->buffer.addr;

		/* reset remain size */
		dma_reader->remain_size = 0;

		/* reset written size */
		dma_reader->written_size = 0;

		//MAD_PRINT(KERN_INFO "***** PCM Playback2 Buffer empty !! ***** \n");

		inused_lines = _mhal_alsa_dma_reader2_get_inused_lines();
	}

	copy_lr_sample = bytes / 2; /* L + R samples */
	copy_size = (dma_reader->channel_mode == E_MONO) ? (bytes * 2) : bytes;

	/* copy data to DMA Reader2 buffer */
	if ( ((inused_lines * _MAD_BYTES_IN_LINE) + copy_size) < dma_reader->high_threshold) {
		if (dma_reader->channel_mode == E_MONO) {
			for (loop = 0; loop < copy_lr_sample; loop++) {
				tmp_bufptr1 = *bufptr++;
				tmp_bufptr2 = *bufptr++;

				*(dma_reader->buffer.w_ptr++) = tmp_bufptr1;
				*(dma_reader->buffer.w_ptr++) = tmp_bufptr2;
				*(dma_reader->buffer.w_ptr++) = tmp_bufptr1;
				*(dma_reader->buffer.w_ptr++) = tmp_bufptr2;

				if (dma_reader->buffer.w_ptr >= (dma_reader->buffer.addr + dma_reader->buffer.size))
					dma_reader->buffer.w_ptr = dma_reader->buffer.addr;
			}
		}
		else {
			for (loop = 0; loop < copy_lr_sample; loop++) {
				*(dma_reader->buffer.w_ptr++) = *bufptr++;
				*(dma_reader->buffer.w_ptr++) = *bufptr++;

				if (dma_reader->buffer.w_ptr >= (dma_reader->buffer.addr + dma_reader->buffer.size))
					dma_reader->buffer.w_ptr = dma_reader->buffer.addr;
			}
		}

		/* flush MIU */
		Chip_Flush_Memory();

		/* update copied size to DMA Reader2 */
		copy_size += dma_reader->remain_size;
		_mhal_alsa_write2_mask_reg(0x3E40, 0x0010, 0x0000);
		_mhal_alsa_write2_mask_reg(0x3E48, 0xFFFF, (copy_size / _MAD_BYTES_IN_LINE));
		_mhal_alsa_write2_mask_reg(0x3E40, 0x0010, 0x0010);
		dma_reader->remain_size = copy_size % _MAD_BYTES_IN_LINE;
		dma_reader->written_size += (copy_size - dma_reader->remain_size);

		if (replay_flag == MAD_TRUE) {
			/* do nothing for DMA Reader2 */
		}

		return bytes;
	}

	//MAD_PRINT(KERN_INFO "***** PCM Playback2 Buffer busy !! ***** \n");

	return 0;
}

/* Get information from DMA Reader2 */
static int _mhal_alsa_dma_reader2_get(int cmd, unsigned int *param)
{
	int err = 0;
	//MAD_PRINT(KERN_INFO "Get parameter from PCM Playback2\n");

	switch(cmd) {
		case E_PCM_PLAYBACK_GET_BUFFER_SIZE:
		{
			struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader2;

			*param = dma_reader->buffer.size;
			break;
		}

		case E_PCM_PLAYBACK_GET_PERIOD_SIZE:
		{
			struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader2;

			*param = dma_reader->period_size;
			break;
		}

		case E_PCM_PLAYBACK_GET_SAMPLE_RATE:
		{
			struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader2;

			*param = dma_reader->sample_rate;
			break;
		}

		case E_PCM_PLAYBACK_GET_CHANNEL_MODE:
		{
			struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader2;

			*param = dma_reader->channel_mode;
			break;
		}

		case E_PCM_PLAYBACK_GET_MAX_CHANNEL:
		{
			*param = _MAD_MAX_CHANNEL;
			break;
		}

		case E_PCM_PLAYBACK_GET_CONSTRAINTS_COUNT:
		{
			*param = sizeof(mad_rates) / sizeof(mad_rates[0]);
			break;
		}

		case E_PCM_PLAYBACK_GET_CONSTRAINTS_LIST:
		{
			*param = (ptrdiff_t)&mad_rates;
			break;
		}

		case E_PCM_PLAYBACK_GET_CONSTRAINTS_MASK:
		{
			*param = 0;
			break;
		}

		case E_PCM_PLAYBACK_GET_BUFFER_INUSED_BYTES:
		{
			*param = _mhal_alsa_dma_reader2_get_inused_lines() * _MAD_BYTES_IN_LINE;
			break;
		}

		case E_PCM_PLAYBACK_GET_BUFFER_AVAIL_BYTES:
		{
			*param = _mhal_alsa_dma_reader2_get_avail_lines() * _MAD_BYTES_IN_LINE;
			break;
		}

		case E_PCM_PLAYBACK_GET_BUFFER_CONSUMED_BYTES:
		{
			struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader2;
			int inused_bytes = 0;
			int consumed_bytes = 0;

			inused_bytes = _mhal_alsa_dma_reader2_get_inused_lines() * _MAD_BYTES_IN_LINE;
			consumed_bytes = dma_reader->written_size - inused_bytes;
			*param = consumed_bytes;
			dma_reader->written_size = inused_bytes;
			break;
		}

		case E_PCM_PLAYBACK_GET_STR_STATUS:
		{
			struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader2;

			*param = dma_reader->str_mode_info.status;
			break;
		}

		case E_PCM_PLAYBACK_GET_DEVICE_STATUS:
		{
			*param = _mhal_alsa_get_device_status();
			break;
		}

		default:
		{
			MAD_PRINT(KERN_INFO "Invalid GET command %d\n", cmd);
			err = -EINVAL;
			break;
		}
	}

	return err;
}

/* Set information to DMA Reader2 */
static int _mhal_alsa_dma_reader2_set(int cmd, unsigned int *param)
{
	int err = 0;
	//MAD_PRINT(KERN_INFO "Set parameter to PCM Playback2\n");

	switch(cmd) {
		case E_PCM_PLAYBACK_SET_SAMPLE_RATE:
		{
			_mhal_alsa_dma_reader2_set_sample_rate(*param);
			break;
		}

		case E_PCM_PLAYBACK_SET_CHANNEL_MODE:
		{
			_mhal_alsa_dma_reader2_set_channel_mode(*param);
			break;
		}

		default:
		{
			MAD_PRINT(KERN_INFO "Invalid SET command %d\n", cmd);
			err = -EINVAL;
			break;
		}
	}

	return err;
}

/* Get DMA Reader2 PCM buffer level */
static int _mhal_alsa_dma_reader2_get_inused_lines(void)
{
	int inused_lines = 0;

	/* Mask LEVEL_CNT_MASK before read */
	_mhal_alsa_write2_mask_reg(0x3E40, 0x0020, 0x0020);
	inused_lines = _mhal_alsa_read2_reg(0x3E54);
	_mhal_alsa_write2_mask_reg(0x3E40, 0x0020, 0x0000);

	return inused_lines;
}

/* Get DMA Reader2 PCM avail bytes */
static int _mhal_alsa_dma_reader2_get_avail_lines(void)
{
	struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader2;
	int inused_lines = 0;
	int avail_lines = 0;

	inused_lines = _mhal_alsa_dma_reader2_get_inused_lines();
	avail_lines = (dma_reader->high_threshold / _MAD_BYTES_IN_LINE) - inused_lines;
	if (avail_lines < 0) {
		MAD_PRINT(KERN_ERR "Error! Incorrect inused lines %d!\n", inused_lines);
		avail_lines = 0;
	}

	return avail_lines;
}

/* Set smaple rate to DMA Reader2 */
static int _mhal_alsa_dma_reader2_set_sample_rate(unsigned int sample_rate)
{
	struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader2;
	unsigned short synthrate, divisor;
	MAD_PRINT(KERN_INFO "Target sample rate is %u\n", sample_rate);

	dma_reader->sample_rate = sample_rate;

	/* New DMA Reader2 setting
	 * Formula is :
	 * Synthesizer Rate = 216MHz / Divisor(1, 2, 4 or 8) * 1024 / 256 / Sampling Rate(32Khz, 44.1KHz or 48KHz)
	 */
	switch(sample_rate) {
		case 8000:
		{
			divisor = 2;
			synthrate = 0x6978;
			break;
		}

		case 11025:
		{
			divisor = 2;
			synthrate = 0x4C87;
			break;
		}

		case 12000:
		{
			divisor = 2;
			synthrate = 0x4650;
			break;
		}

		case 16000:
		{
			divisor = 1;
			synthrate = 0x6978;
			break;
		}

		case 22050:
		{
			divisor = 1;
			synthrate = 0x4C87;
			break;
		}

		case 24000:
		{
			divisor = 1;
			synthrate = 0x4650;
			break;
		}

		case 32000:
		{
			divisor = 0;
			synthrate = 0x6978;
			break;
		}

		case 44100:
		{
			divisor = 0;
			synthrate = 0x4C87;
			break;
		}

		case 48000:
		{
			divisor = 0;
			synthrate = 0x4650;
			break;
		}

		default:
		{
			MAD_PRINT(KERN_ERR "Error! un-supported sample rate %u !!!\n", sample_rate);
			divisor = 0;
			synthrate = 0x4650;
			dma_reader->sample_rate = 48000;
			break;
		}
	}

	/* synthersizer setting update */
	_mhal_alsa_write2_mask_reg(0x3E4C, 0x0030, (divisor << 4)); /* set divisor */
	_mhal_alsa_write2_reg(0x3E4E, synthrate); /* DMA synthesizer N.F. */
	_mhal_alsa_write2_mask_reg(0x3E4C, 0x0145, 0x0145); /* enable DMA synthesizer */

	return 0;
}

/* Set channel mode to DMA Reader2 */
static int _mhal_alsa_dma_reader2_set_channel_mode(unsigned int channel_mode)
{
	struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader2;
	unsigned int buffer_size = 0;
	MAD_PRINT(KERN_INFO "Target channel mode is %u\n", channel_mode);

	dma_reader->channel_mode = channel_mode;
	buffer_size = ((dma_reader->sample_rate << dma_reader->channel_mode) * _MAD_DMA_READER2_QUEUE_SIZE) / 1000;
	if ((buffer_size % _MAD_BYTES_IN_LINE))
		buffer_size += (_MAD_BYTES_IN_LINE - (buffer_size % _MAD_BYTES_IN_LINE));

	dma_reader->buffer.size = (dma_reader->channel_mode == E_MONO) ? (buffer_size * 2) : buffer_size;
	dma_reader->period_size = dma_reader->buffer.size >> 2;
	dma_reader->high_threshold = dma_reader->buffer.size - (dma_reader->buffer.size >> 3);

	return 0;
}

/* Initiate DMA Reader3 */
static int _mhal_alsa_dma_reader3_init(void)
{
	struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader3;
	unsigned int audio_pcm_dmaRdr_bufSz = 0;  /* must be multiple of DMA_RDR_PCM_BUF_UNIT*2 (= 0x2000) */
	ptrdiff_t audio_pcm_dmaRdr_base_pa = 0; /* DMA Reader2 Input buffer (DM_Prefetch) */
	ptrdiff_t audio_pcm_dmaRdr_base_ba = 0;
	ptrdiff_t audio_pcm_dmaRdr_base_va = 0;
	MAD_PRINT(KERN_INFO "Initiate MStar PCM Playback3 engine\n");

	if ((dma_reader->initialized_status != MAD_TRUE) || (dma_reader->str_mode_info.status != E_RESUME)) {
		audio_pcm_dmaRdr_bufSz = dma_reader->buffer.size;
		audio_pcm_dmaRdr_base_pa = ((ptrdiff_t)_mhal_alsa_read_reg(0x2A90) * _MAD_ADDR_CONVERTOR) + _MAD_DMA_READER2_BASE_OFFSET;
		audio_pcm_dmaRdr_base_pa += (((_mhal_alsa_read_reg(0x2AC0) & 0x0F) << 16) * _MAD_ADDR_CONVERTOR);
		audio_pcm_dmaRdr_base_ba = audio_pcm_dmaRdr_base_pa + _MAD_PHYSICAL_ADDR_TO_BUS_ADDR;

		if ((audio_pcm_dmaRdr_base_ba % 0x1000)) {
			MAD_PRINT(KERN_ERR "Error! Invalid MStar PCM reader bus address, it should be aligned by 4 KB!\n");
			return -EFAULT;
		}

		/* convert Bus Address to Virtual Address */
		if (g_pcm_dmaRdr3_base_va == 0) {
			g_pcm_dmaRdr3_base_va = (ptrdiff_t)ioremap_nocache(audio_pcm_dmaRdr_base_ba, audio_pcm_dmaRdr_bufSz);
			if (g_pcm_dmaRdr3_base_va == 0) {
				MAD_PRINT(KERN_ERR "Error! fail to convert PCM Playback2 Buffer Bus Address to Virtual Address\n");
				return -ENOMEM;
			}
		}
		audio_pcm_dmaRdr_base_va = g_pcm_dmaRdr3_base_va;

		dma_reader->str_mode_info.physical_addr = audio_pcm_dmaRdr_base_pa;
		dma_reader->str_mode_info.bus_addr = audio_pcm_dmaRdr_base_ba;
		dma_reader->str_mode_info.virtual_addr = audio_pcm_dmaRdr_base_va;

		dma_reader->initialized_status = MAD_TRUE;
	}
	else {
		audio_pcm_dmaRdr_bufSz = dma_reader->buffer.size;
		audio_pcm_dmaRdr_base_pa = dma_reader->str_mode_info.physical_addr;
		audio_pcm_dmaRdr_base_ba = dma_reader->str_mode_info.bus_addr;
		audio_pcm_dmaRdr_base_va = dma_reader->str_mode_info.virtual_addr;
	}

	/* init DMA writer address */
	dma_reader->buffer.addr = (unsigned char *)audio_pcm_dmaRdr_base_va;
	dma_reader->buffer.w_ptr = dma_reader->buffer.addr;
	//MAD_PRINT(KERN_INFO "PCM Playback2 buffer start address = 0x%08X\n", dma_reader->buffer.addr);
	//MAD_PRINT(KERN_INFO "PCM Playback2 buffer end address = 0x%08X\n", (dma_reader->buffer.addr + dma_reader->buffer.size));

	/* initial DMA Reader2 */
	_mhal_alsa_write2_reg(0x3E40, 0x0);
	_mhal_alsa_write2_reg(0x3E42, ((audio_pcm_dmaRdr_base_pa /_MAD_BYTES_IN_LINE) & 0xFFFF));
	_mhal_alsa_write2_reg(0x3E44, (((audio_pcm_dmaRdr_base_pa /_MAD_BYTES_IN_LINE) >> 16) & 0xFFFF));
	_mhal_alsa_write2_reg(0x3E46, (audio_pcm_dmaRdr_bufSz /_MAD_BYTES_IN_LINE)); /* setting : DMA Reader2 Size */
	_mhal_alsa_write2_mask_reg(0x3E4A, 0xFFFF, 0x00012); /* setting : DMA Reader2 Underrun Thr */

	/* reset and start DMA Reader2 */
	_mhal_alsa_write2_mask_reg(0x3E40, 0xFFFF, 0x800B);
	_mhal_alsa_write2_mask_reg(0x3E40, 0xFFFF, 0x000B);

	/* reset remain size */
	dma_reader->remain_size = 0;

	/* reset written size */
	dma_reader->written_size = 0;

	/* clear DMA reader buffer */
	memset((void *)dma_reader->buffer.addr, 0x00, dma_reader->buffer.size);
	Chip_Flush_Memory();

	return 0;
}

/* Exit DMA Reader3 */
static int _mhal_alsa_dma_reader3_exit(void)
{
	struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader3;
	MAD_PRINT(KERN_INFO "Exit MStar PCM Playback3 engine\n");

	if (g_pcm_dmaRdr3_base_va != 0) {
		if (dma_reader->buffer.addr) {
			iounmap((void *)dma_reader->buffer.addr);
			dma_reader->buffer.addr = 0;
		}
		else {
			MAD_PRINT(KERN_ERR "Error! MStar PCM Playback3 buffer address should not be 0 !\n");
		}

		g_pcm_dmaRdr3_base_va = 0;
	}

	dma_reader->str_mode_info.status = E_STOP;
	dma_reader->initialized_status = MAD_FALSE;

	return 0;
}

/* Start DMA Reader3 */
static int _mhal_alsa_dma_reader3_start(void)
{
	struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader3;
	//MAD_PRINT(KERN_INFO "Start MStar PCM Playback3 engine\n");

	_mhal_alsa_write_mask_byte(0x2C67, 0x9F, 0x90); /* CH6 sel to HW DMA Reader2 */

	dma_reader->str_mode_info.status = E_START;

	return 0;
}

/* Stop DMA Reader3 */
static int _mhal_alsa_dma_reader3_stop(void)
{
	struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader3;
	//MAD_PRINT(KERN_INFO "Stop MStar PCM Playback3 engine\n");

	/* clear wr cntrs */
	_mhal_alsa_write2_mask_reg(0x3E48, 0xFFFF, 0x0000);

	/* reset and start DMA Reader2 */
	_mhal_alsa_write2_mask_reg(0x3E40, 0xFFFF, 0x800B);
	_mhal_alsa_write2_mask_reg(0x3E40, 0xFFFF, 0x000B);

	/* reset Write Pointer */
	dma_reader->buffer.w_ptr = dma_reader->buffer.addr;

	/* reset remain size */
	dma_reader->remain_size = 0;

	/* reset written size */
	dma_reader->written_size = 0;

	return 0;
}

/* Resume DMA Reader3 */
static int _mhal_alsa_dma_reader3_resume(void)
{
	struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader3;
	//MAD_PRINT(KERN_INFO "Resume MStar PCM Playback3 engine\n");

	dma_reader->str_mode_info.status = E_RESUME;

	return 0;
}

/* Suspend DMA Reader3 */
static int _mhal_alsa_dma_reader3_suspend(void)
{
	struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader3;
	//MAD_PRINT(KERN_INFO "Suspend MStar PCM Playback3 engine\n");

	dma_reader->str_mode_info.status = E_SUSPEND;

	return 0;
}

/* Write PCM to DMA Reader3 */
static unsigned int _mhal_alsa_dma_reader3_write(void *buffer, unsigned int bytes)
{
	struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader3;
	bool replay_flag = MAD_FALSE;
	unsigned char *bufptr = (unsigned char *)buffer;
	unsigned char tmp_bufptr1 = 0;
	unsigned char tmp_bufptr2 = 0;
	int loop = 0;
	int inused_lines = 0;
	unsigned int copy_lr_sample = 0;
	unsigned int copy_size = 0;

	inused_lines = _mhal_alsa_dma_reader3_get_inused_lines();
	if (inused_lines <= 6) {
		if ((_mhal_alsa_read2_reg(0x3E56) & 0x0002) == 0x0002)
			replay_flag = MAD_TRUE;

		/* clear wr cntrs */
		_mhal_alsa_write2_mask_reg(0x3E48, 0xFFFF, 0x0000);

		/* reset and start DMA Reader2 */
		_mhal_alsa_write2_mask_reg(0x3E40, 0xFFFF, 0x800B);
		_mhal_alsa_write2_mask_reg(0x3E40, 0xFFFF, 0x000B);

		/* reset Write Pointer */
		dma_reader->buffer.w_ptr = dma_reader->buffer.addr;

		/* reset remain size */
		dma_reader->remain_size = 0;

		/* reset written size */
		dma_reader->written_size = 0;

		//MAD_PRINT(KERN_INFO "***** PCM Playback3 Buffer empty !! ***** \n");

		inused_lines = _mhal_alsa_dma_reader3_get_inused_lines();
	}

	copy_lr_sample = bytes / 2; /* L + R samples */
	copy_size = (dma_reader->channel_mode == E_MONO) ? (bytes * 2) : bytes;

	/* copy data to DMA Reader2 buffer */
	if ( ((inused_lines * _MAD_BYTES_IN_LINE) + copy_size) < dma_reader->high_threshold) {
		if (dma_reader->channel_mode == E_MONO) {
			for (loop = 0; loop < copy_lr_sample; loop++) {
				tmp_bufptr1 = *bufptr++;
				tmp_bufptr2 = *bufptr++;

				*(dma_reader->buffer.w_ptr++) = tmp_bufptr1;
				*(dma_reader->buffer.w_ptr++) = tmp_bufptr2;
				*(dma_reader->buffer.w_ptr++) = tmp_bufptr1;
				*(dma_reader->buffer.w_ptr++) = tmp_bufptr2;

				if (dma_reader->buffer.w_ptr >= (dma_reader->buffer.addr + dma_reader->buffer.size))
					dma_reader->buffer.w_ptr = dma_reader->buffer.addr;
			}
		}
		else {
			for (loop = 0; loop < copy_lr_sample; loop++) {
				*(dma_reader->buffer.w_ptr++) = *bufptr++;
				*(dma_reader->buffer.w_ptr++) = *bufptr++;

				if (dma_reader->buffer.w_ptr >= (dma_reader->buffer.addr + dma_reader->buffer.size))
					dma_reader->buffer.w_ptr = dma_reader->buffer.addr;
			}
		}

		/* flush MIU */
		Chip_Flush_Memory();

		/* update copied size to DMA Reader2 */
		copy_size += dma_reader->remain_size;
		_mhal_alsa_write2_mask_reg(0x3E40, 0x0010, 0x0000);
		_mhal_alsa_write2_mask_reg(0x3E48, 0xFFFF, (copy_size / _MAD_BYTES_IN_LINE));
		_mhal_alsa_write2_mask_reg(0x3E40, 0x0010, 0x0010);
		dma_reader->remain_size = copy_size % _MAD_BYTES_IN_LINE;
		dma_reader->written_size += (copy_size - dma_reader->remain_size);

		if (replay_flag == MAD_TRUE) {
			/* do nothing for DMA Reader2 */
		}

		return bytes;
	}

	//MAD_PRINT(KERN_INFO "***** PCM Playback3 Buffer busy !! ***** \n");

	return 0;
}

/* Get information from DMA Reader3 */
static int _mhal_alsa_dma_reader3_get(int cmd, unsigned int *param)
{
	int err = 0;
	//MAD_PRINT(KERN_INFO "Get parameter from PCM Playback3\n");

	switch(cmd) {
		case E_PCM_PLAYBACK_GET_BUFFER_SIZE:
		{
			struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader3;

			*param = dma_reader->buffer.size;
			break;
		}

		case E_PCM_PLAYBACK_GET_PERIOD_SIZE:
		{
			struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader3;

			*param = dma_reader->period_size;
			break;
		}

		case E_PCM_PLAYBACK_GET_SAMPLE_RATE:
		{
			struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader3;

			*param = dma_reader->sample_rate;
			break;
		}

		case E_PCM_PLAYBACK_GET_CHANNEL_MODE:
		{
			struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader3;

			*param = dma_reader->channel_mode;
			break;
		}

		case E_PCM_PLAYBACK_GET_MAX_CHANNEL:
		{
			*param = _MAD_MAX_CHANNEL;
			break;
		}

		case E_PCM_PLAYBACK_GET_CONSTRAINTS_COUNT:
		{
			*param = sizeof(mad_rates) / sizeof(mad_rates[0]);
			break;
		}

		case E_PCM_PLAYBACK_GET_CONSTRAINTS_LIST:
		{
			*param = (ptrdiff_t)&mad_rates;
			break;
		}

		case E_PCM_PLAYBACK_GET_CONSTRAINTS_MASK:
		{
			*param = 0;
			break;
		}

		case E_PCM_PLAYBACK_GET_BUFFER_INUSED_BYTES:
		{
			*param = _mhal_alsa_dma_reader3_get_inused_lines() * _MAD_BYTES_IN_LINE;
			break;
		}

		case E_PCM_PLAYBACK_GET_BUFFER_AVAIL_BYTES:
		{
			*param = _mhal_alsa_dma_reader3_get_avail_lines() * _MAD_BYTES_IN_LINE;
			break;
		}

		case E_PCM_PLAYBACK_GET_BUFFER_CONSUMED_BYTES:
		{
			struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader3;
			int inused_bytes = 0;
			int consumed_bytes = 0;

			inused_bytes = _mhal_alsa_dma_reader3_get_inused_lines() * _MAD_BYTES_IN_LINE;
			consumed_bytes = dma_reader->written_size - inused_bytes;
			*param = consumed_bytes;
			dma_reader->written_size = inused_bytes;
			break;
		}

		case E_PCM_PLAYBACK_GET_STR_STATUS:
		{
			struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader3;

			*param = dma_reader->str_mode_info.status;
			break;
		}

		case E_PCM_PLAYBACK_GET_DEVICE_STATUS:
		{
			*param = _mhal_alsa_get_device_status();
			break;
		}

		default:
		{
			MAD_PRINT(KERN_INFO "Invalid GET command %d\n", cmd);
			err = -EINVAL;
			break;
		}
	}

	return err;
}

/* Set information to DMA Reader3 */
static int _mhal_alsa_dma_reader3_set(int cmd, unsigned int *param)
{
	int err = 0;
	//MAD_PRINT(KERN_INFO "Set parameter to PCM Playback3\n");

	switch(cmd) {
		case E_PCM_PLAYBACK_SET_SAMPLE_RATE:
		{
			_mhal_alsa_dma_reader3_set_sample_rate(*param);
			break;
		}

		case E_PCM_PLAYBACK_SET_CHANNEL_MODE:
		{
			_mhal_alsa_dma_reader3_set_channel_mode(*param);
			break;
		}

		default:
		{
			MAD_PRINT(KERN_INFO "Invalid SET command %d\n", cmd);
			err = -EINVAL;
			break;
		}
	}

	return err;
}

/* Get DMA Reader3 PCM buffer level */
static int _mhal_alsa_dma_reader3_get_inused_lines(void)
{
	int inused_lines = 0;

	/* Mask LEVEL_CNT_MASK before read */
	_mhal_alsa_write2_mask_reg(0x3E40, 0x0020, 0x0020);
	inused_lines = _mhal_alsa_read2_reg(0x3E54);
	_mhal_alsa_write2_mask_reg(0x3E40, 0x0020, 0x0000);

	return inused_lines;
}

/* Get DMA Reader3 PCM avail bytes */
static int _mhal_alsa_dma_reader3_get_avail_lines(void)
{
	struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader3;
	int inused_lines = 0;
	int avail_lines = 0;

	inused_lines = _mhal_alsa_dma_reader3_get_inused_lines();
	avail_lines = (dma_reader->high_threshold / _MAD_BYTES_IN_LINE) - inused_lines;
	if (avail_lines < 0) {
		MAD_PRINT(KERN_ERR "Error! Incorrect inused lines %d!\n", inused_lines);
		avail_lines = 0;
	}

	return avail_lines;
}

/* Set smaple rate to DMA Reader3 */
static int _mhal_alsa_dma_reader3_set_sample_rate(unsigned int sample_rate)
{
	struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader3;
	unsigned short synthrate, divisor;
	MAD_PRINT(KERN_INFO "Target sample rate is %u\n", sample_rate);

	dma_reader->sample_rate = sample_rate;

	/* New DMA Reader2 setting
	 * Formula is :
	 * Synthesizer Rate = 216MHz / Divisor(1, 2, 4 or 8) * 1024 / 256 / Sampling Rate(32Khz, 44.1KHz or 48KHz)
	 */
	switch(sample_rate) {
		case 8000:
		{
			divisor = 2;
			synthrate = 0x6978;
			break;
		}

		case 11025:
		{
			divisor = 2;
			synthrate = 0x4C87;
			break;
		}

		case 12000:
		{
			divisor = 2;
			synthrate = 0x4650;
			break;
		}

		case 16000:
		{
			divisor = 1;
			synthrate = 0x6978;
			break;
		}

		case 22050:
		{
			divisor = 1;
			synthrate = 0x4C87;
			break;
		}

		case 24000:
		{
			divisor = 1;
			synthrate = 0x4650;
			break;
		}

		case 32000:
		{
			divisor = 0;
			synthrate = 0x6978;
			break;
		}

		case 44100:
		{
			divisor = 0;
			synthrate = 0x4C87;
			break;
		}

		case 48000:
		{
			divisor = 0;
			synthrate = 0x4650;
			break;
		}

		default:
		{
			MAD_PRINT(KERN_ERR "Error! un-supported sample rate %u !!!\n", sample_rate);
			divisor = 0;
			synthrate = 0x4650;
			dma_reader->sample_rate = 48000;
			break;
		}
	}

	/* synthersizer setting update */
	_mhal_alsa_write2_mask_reg(0x3E4C, 0x0030, (divisor << 4)); /* set divisor */
	_mhal_alsa_write2_reg(0x3E4E, synthrate); /* DMA synthesizer N.F. */
	_mhal_alsa_write2_mask_reg(0x3E4C, 0x0145, 0x0145); /* enable DMA synthesizer */

	return 0;
}

/* Set channel mode to DMA Reader3 */
static int _mhal_alsa_dma_reader3_set_channel_mode(unsigned int channel_mode)
{
	struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader3;
	unsigned int buffer_size = 0;
	MAD_PRINT(KERN_INFO "Target channel mode is %u\n", channel_mode);

	dma_reader->channel_mode = channel_mode;
	buffer_size = ((dma_reader->sample_rate << dma_reader->channel_mode) * _MAD_DMA_READER2_QUEUE_SIZE) / 1000;
	if ((buffer_size % _MAD_BYTES_IN_LINE))
		buffer_size += (_MAD_BYTES_IN_LINE - (buffer_size % _MAD_BYTES_IN_LINE));

	dma_reader->buffer.size = (dma_reader->channel_mode == E_MONO) ? (buffer_size * 2) : buffer_size;
	dma_reader->period_size = dma_reader->buffer.size >> 2;
	dma_reader->high_threshold = dma_reader->buffer.size - (dma_reader->buffer.size >> 3);

	return 0;
}

/* Initiate DMA Reader4 */
static int _mhal_alsa_dma_reader4_init(void)
{
	struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader4;
	unsigned int audio_pcm_dmaRdr_bufSz = 0;  /* must be multiple of DMA_RDR_PCM_BUF_UNIT*2 (= 0x2000) */
	ptrdiff_t audio_pcm_dmaRdr_base_pa = 0; /* DMA Reader2 Input buffer (DM_Prefetch) */
	ptrdiff_t audio_pcm_dmaRdr_base_ba = 0;
	ptrdiff_t audio_pcm_dmaRdr_base_va = 0;
	MAD_PRINT(KERN_INFO "Initiate MStar PCM Playback4 engine\n");

	if ((dma_reader->initialized_status != MAD_TRUE) || (dma_reader->str_mode_info.status != E_RESUME)) {
		audio_pcm_dmaRdr_bufSz = dma_reader->buffer.size;
		audio_pcm_dmaRdr_base_pa = ((ptrdiff_t)_mhal_alsa_read_reg(0x2A90) * _MAD_ADDR_CONVERTOR) + _MAD_DMA_READER2_BASE_OFFSET;
		audio_pcm_dmaRdr_base_pa += (((_mhal_alsa_read_reg(0x2AC0) & 0x0F) << 16) * _MAD_ADDR_CONVERTOR);
		audio_pcm_dmaRdr_base_ba = audio_pcm_dmaRdr_base_pa + _MAD_PHYSICAL_ADDR_TO_BUS_ADDR;

		if ((audio_pcm_dmaRdr_base_ba % 0x1000)) {
			MAD_PRINT(KERN_ERR "Error! Invalid MStar PCM reader bus address, it should be aligned by 4 KB!\n");
			return -EFAULT;
		}

		/* convert Bus Address to Virtual Address */
		if (g_pcm_dmaRdr4_base_va == 0) {
			g_pcm_dmaRdr4_base_va = (ptrdiff_t)ioremap_nocache(audio_pcm_dmaRdr_base_ba, audio_pcm_dmaRdr_bufSz);
			if (g_pcm_dmaRdr4_base_va == 0) {
				MAD_PRINT(KERN_ERR "Error! fail to convert PCM Playback2 Buffer Bus Address to Virtual Address\n");
				return -ENOMEM;
			}
		}
		audio_pcm_dmaRdr_base_va = g_pcm_dmaRdr4_base_va;

		dma_reader->str_mode_info.physical_addr = audio_pcm_dmaRdr_base_pa;
		dma_reader->str_mode_info.bus_addr = audio_pcm_dmaRdr_base_ba;
		dma_reader->str_mode_info.virtual_addr = audio_pcm_dmaRdr_base_va;

		dma_reader->initialized_status = MAD_TRUE;
	}
	else {
		audio_pcm_dmaRdr_bufSz = dma_reader->buffer.size;
		audio_pcm_dmaRdr_base_pa = dma_reader->str_mode_info.physical_addr;
		audio_pcm_dmaRdr_base_ba = dma_reader->str_mode_info.bus_addr;
		audio_pcm_dmaRdr_base_va = dma_reader->str_mode_info.virtual_addr;
	}

	/* init DMA writer address */
	dma_reader->buffer.addr = (unsigned char *)audio_pcm_dmaRdr_base_va;
	dma_reader->buffer.w_ptr = dma_reader->buffer.addr;
	//MAD_PRINT(KERN_INFO "PCM Playback2 buffer start address = 0x%08X\n", dma_reader->buffer.addr);
	//MAD_PRINT(KERN_INFO "PCM Playback2 buffer end address = 0x%08X\n", (dma_reader->buffer.addr + dma_reader->buffer.size));

	/* initial DMA Reader2 */
	_mhal_alsa_write2_reg(0x3E40, 0x0);
	_mhal_alsa_write2_reg(0x3E42, ((audio_pcm_dmaRdr_base_pa /_MAD_BYTES_IN_LINE) & 0xFFFF));
	_mhal_alsa_write2_reg(0x3E44, (((audio_pcm_dmaRdr_base_pa /_MAD_BYTES_IN_LINE) >> 16) & 0xFFFF));
	_mhal_alsa_write2_reg(0x3E46, (audio_pcm_dmaRdr_bufSz /_MAD_BYTES_IN_LINE)); /* setting : DMA Reader2 Size */
	_mhal_alsa_write2_mask_reg(0x3E4A, 0xFFFF, 0x00012); /* setting : DMA Reader2 Underrun Thr */

	/* reset and start DMA Reader2 */
	_mhal_alsa_write2_mask_reg(0x3E40, 0xFFFF, 0x800B);
	_mhal_alsa_write2_mask_reg(0x3E40, 0xFFFF, 0x000B);

	/* reset remain size */
	dma_reader->remain_size = 0;

	/* reset written size */
	dma_reader->written_size = 0;

	/* clear DMA reader buffer */
	memset((void *)dma_reader->buffer.addr, 0x00, dma_reader->buffer.size);
	Chip_Flush_Memory();

	return 0;
}

/* Exit DMA Reader4 */
static int _mhal_alsa_dma_reader4_exit(void)
{
	struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader4;
	MAD_PRINT(KERN_INFO "Exit MStar PCM Playback4 engine\n");

	if (g_pcm_dmaRdr4_base_va != 0) {
		if (dma_reader->buffer.addr) {
			iounmap((void *)dma_reader->buffer.addr);
			dma_reader->buffer.addr = 0;
		}
		else {
			MAD_PRINT(KERN_ERR "Error! MStar PCM Playback4 buffer address should not be 0 !\n");
		}

		g_pcm_dmaRdr4_base_va = 0;
	}

	dma_reader->str_mode_info.status = E_STOP;
	dma_reader->initialized_status = MAD_FALSE;

	return 0;
}

/* Start DMA Reader4 */
static int _mhal_alsa_dma_reader4_start(void)
{
	struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader4;
	//MAD_PRINT(KERN_INFO "Start MStar PCM Playback4 engine\n");

	_mhal_alsa_write_mask_byte(0x2C67, 0x9F, 0x90); /* CH6 sel to HW DMA Reader2 */

	dma_reader->str_mode_info.status = E_START;

	return 0;
}

/* Stop DMA Reader4 */
static int _mhal_alsa_dma_reader4_stop(void)
{
	struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader4;
	//MAD_PRINT(KERN_INFO "Stop MStar PCM Playback4 engine\n");

	/* clear wr cntrs */
	_mhal_alsa_write2_mask_reg(0x3E48, 0xFFFF, 0x0000);

	/* reset and start DMA Reader2 */
	_mhal_alsa_write2_mask_reg(0x3E40, 0xFFFF, 0x800B);
	_mhal_alsa_write2_mask_reg(0x3E40, 0xFFFF, 0x000B);

	/* reset Write Pointer */
	dma_reader->buffer.w_ptr = dma_reader->buffer.addr;

	/* reset remain size */
	dma_reader->remain_size = 0;

	/* reset written size */
	dma_reader->written_size = 0;

	return 0;
}

/* Resume DMA Reader4 */
static int _mhal_alsa_dma_reader4_resume(void)
{
	struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader4;
	//MAD_PRINT(KERN_INFO "Resume MStar PCM Playback4 engine\n");

	dma_reader->str_mode_info.status = E_RESUME;

	return 0;
}

/* Suspend DMA Reader4 */
static int _mhal_alsa_dma_reader4_suspend(void)
{
	struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader4;
	//MAD_PRINT(KERN_INFO "Suspend MStar PCM Playback4 engine\n");

	dma_reader->str_mode_info.status = E_SUSPEND;

	return 0;
}

/* Write PCM to DMA Reader4 */
static unsigned int _mhal_alsa_dma_reader4_write(void *buffer, unsigned int bytes)
{
	struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader4;
	bool replay_flag = MAD_FALSE;
	unsigned char *bufptr = (unsigned char *)buffer;
	unsigned char tmp_bufptr1 = 0;
	unsigned char tmp_bufptr2 = 0;
	int loop = 0;
	int inused_lines = 0;
	unsigned int copy_lr_sample = 0;
	unsigned int copy_size = 0;

	inused_lines = _mhal_alsa_dma_reader4_get_inused_lines();
	if (inused_lines <= 6) {
		if ((_mhal_alsa_read2_reg(0x3E56) & 0x0002) == 0x0002)
			replay_flag = MAD_TRUE;

		/* clear wr cntrs */
		_mhal_alsa_write2_mask_reg(0x3E48, 0xFFFF, 0x0000);

		/* reset and start DMA Reader2 */
		_mhal_alsa_write2_mask_reg(0x3E40, 0xFFFF, 0x800B);
		_mhal_alsa_write2_mask_reg(0x3E40, 0xFFFF, 0x000B);

		/* reset Write Pointer */
		dma_reader->buffer.w_ptr = dma_reader->buffer.addr;

		/* reset remain size */
		dma_reader->remain_size = 0;

		/* reset written size */
		dma_reader->written_size = 0;

		//MAD_PRINT(KERN_INFO "***** PCM Playback4 Buffer empty !! ***** \n");

		inused_lines = _mhal_alsa_dma_reader4_get_inused_lines();
	}

	copy_lr_sample = bytes / 2; /* L + R samples */
	copy_size = (dma_reader->channel_mode == E_MONO) ? (bytes * 2) : bytes;

	/* copy data to DMA Reader2 buffer */
	if ( ((inused_lines * _MAD_BYTES_IN_LINE) + copy_size) < dma_reader->high_threshold) {
		if (dma_reader->channel_mode == E_MONO) {
			for (loop = 0; loop < copy_lr_sample; loop++) {
				tmp_bufptr1 = *bufptr++;
				tmp_bufptr2 = *bufptr++;

				*(dma_reader->buffer.w_ptr++) = tmp_bufptr1;
				*(dma_reader->buffer.w_ptr++) = tmp_bufptr2;
				*(dma_reader->buffer.w_ptr++) = tmp_bufptr1;
				*(dma_reader->buffer.w_ptr++) = tmp_bufptr2;

				if (dma_reader->buffer.w_ptr >= (dma_reader->buffer.addr + dma_reader->buffer.size))
					dma_reader->buffer.w_ptr = dma_reader->buffer.addr;
			}
		}
		else {
			for (loop = 0; loop < copy_lr_sample; loop++) {
				*(dma_reader->buffer.w_ptr++) = *bufptr++;
				*(dma_reader->buffer.w_ptr++) = *bufptr++;

				if (dma_reader->buffer.w_ptr >= (dma_reader->buffer.addr + dma_reader->buffer.size))
					dma_reader->buffer.w_ptr = dma_reader->buffer.addr;
			}
		}

		/* flush MIU */
		Chip_Flush_Memory();

		/* update copied size to DMA Reader2 */
		copy_size += dma_reader->remain_size;
		_mhal_alsa_write2_mask_reg(0x3E40, 0x0010, 0x0000);
		_mhal_alsa_write2_mask_reg(0x3E48, 0xFFFF, (copy_size / _MAD_BYTES_IN_LINE));
		_mhal_alsa_write2_mask_reg(0x3E40, 0x0010, 0x0010);
		dma_reader->remain_size = copy_size % _MAD_BYTES_IN_LINE;
		dma_reader->written_size += (copy_size - dma_reader->remain_size);

		if (replay_flag == MAD_TRUE) {
			/* do nothing for DMA Reader2 */
		}

		return bytes;
	}

	//MAD_PRINT(KERN_INFO "***** PCM Playback4 Buffer busy !! ***** \n");

	return 0;
}

/* Get information from DMA Reader4 */
static int _mhal_alsa_dma_reader4_get(int cmd, unsigned int *param)
{
	int err = 0;
	//MAD_PRINT(KERN_INFO "Get parameter from PCM Playback4\n");

	switch(cmd) {
		case E_PCM_PLAYBACK_GET_BUFFER_SIZE:
		{
			struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader4;

			*param = dma_reader->buffer.size;
			break;
		}

		case E_PCM_PLAYBACK_GET_PERIOD_SIZE:
		{
			struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader4;

			*param = dma_reader->period_size;
			break;
		}

		case E_PCM_PLAYBACK_GET_SAMPLE_RATE:
		{
			struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader4;

			*param = dma_reader->sample_rate;
			break;
		}

		case E_PCM_PLAYBACK_GET_CHANNEL_MODE:
		{
			struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader4;

			*param = dma_reader->channel_mode;
			break;
		}

		case E_PCM_PLAYBACK_GET_MAX_CHANNEL:
		{
			*param = _MAD_MAX_CHANNEL;
			break;
		}

		case E_PCM_PLAYBACK_GET_CONSTRAINTS_COUNT:
		{
			*param = sizeof(mad_rates) / sizeof(mad_rates[0]);
			break;
		}

		case E_PCM_PLAYBACK_GET_CONSTRAINTS_LIST:
		{
			*param = (ptrdiff_t)&mad_rates;
			break;
		}

		case E_PCM_PLAYBACK_GET_CONSTRAINTS_MASK:
		{
			*param = 0;
			break;
		}

		case E_PCM_PLAYBACK_GET_BUFFER_INUSED_BYTES:
		{
			*param = _mhal_alsa_dma_reader4_get_inused_lines() * _MAD_BYTES_IN_LINE;
			break;
		}

		case E_PCM_PLAYBACK_GET_BUFFER_AVAIL_BYTES:
		{
			*param = _mhal_alsa_dma_reader4_get_avail_lines() * _MAD_BYTES_IN_LINE;
			break;
		}

		case E_PCM_PLAYBACK_GET_BUFFER_CONSUMED_BYTES:
		{
			struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader4;
			int inused_bytes = 0;
			int consumed_bytes = 0;

			inused_bytes = _mhal_alsa_dma_reader4_get_inused_lines() * _MAD_BYTES_IN_LINE;
			consumed_bytes = dma_reader->written_size - inused_bytes;
			*param = consumed_bytes;
			dma_reader->written_size = inused_bytes;
			break;
		}

		case E_PCM_PLAYBACK_GET_STR_STATUS:
		{
			struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader4;

			*param = dma_reader->str_mode_info.status;
			break;
		}

		case E_PCM_PLAYBACK_GET_DEVICE_STATUS:
		{
			*param = _mhal_alsa_get_device_status();
			break;
		}

		default:
		{
			MAD_PRINT(KERN_INFO "Invalid GET command %d\n", cmd);
			err = -EINVAL;
			break;
		}
	}

	return err;
}

/* Set information to DMA Reader4 */
static int _mhal_alsa_dma_reader4_set(int cmd, unsigned int *param)
{
	int err = 0;
	//MAD_PRINT(KERN_INFO "Set parameter to PCM Playback4\n");

	switch(cmd) {
		case E_PCM_PLAYBACK_SET_SAMPLE_RATE:
		{
			_mhal_alsa_dma_reader4_set_sample_rate(*param);
			break;
		}

		case E_PCM_PLAYBACK_SET_CHANNEL_MODE:
		{
			_mhal_alsa_dma_reader4_set_channel_mode(*param);
			break;
		}

		default:
		{
			MAD_PRINT(KERN_INFO "Invalid SET command %d\n", cmd);
			err = -EINVAL;
			break;
		}
	}

	return err;
}

/* Get DMA Reader4 PCM buffer level */
static int _mhal_alsa_dma_reader4_get_inused_lines(void)
{
	int inused_lines = 0;

	/* Mask LEVEL_CNT_MASK before read */
	_mhal_alsa_write2_mask_reg(0x3E40, 0x0020, 0x0020);
	inused_lines = _mhal_alsa_read2_reg(0x3E54);
	_mhal_alsa_write2_mask_reg(0x3E40, 0x0020, 0x0000);

	return inused_lines;
}

/* Get DMA Reader4 PCM avail bytes */
static int _mhal_alsa_dma_reader4_get_avail_lines(void)
{
	struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader4;
	int inused_lines = 0;
	int avail_lines = 0;

	inused_lines = _mhal_alsa_dma_reader4_get_inused_lines();
	avail_lines = (dma_reader->high_threshold / _MAD_BYTES_IN_LINE) - inused_lines;
	if (avail_lines < 0) {
		MAD_PRINT(KERN_ERR "Error! Incorrect inused lines %d!\n", inused_lines);
		avail_lines = 0;
	}

	return avail_lines;
}

/* Set smaple rate to DMA Reader4 */
static int _mhal_alsa_dma_reader4_set_sample_rate(unsigned int sample_rate)
{
	struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader4;
	unsigned short synthrate, divisor;
	MAD_PRINT(KERN_INFO "Target sample rate is %u\n", sample_rate);

	dma_reader->sample_rate = sample_rate;

	/* New DMA Reader2 setting
	 * Formula is :
	 * Synthesizer Rate = 216MHz / Divisor(1, 2, 4 or 8) * 1024 / 256 / Sampling Rate(32Khz, 44.1KHz or 48KHz)
	 */
	switch(sample_rate) {
		case 8000:
		{
			divisor = 2;
			synthrate = 0x6978;
			break;
		}

		case 11025:
		{
			divisor = 2;
			synthrate = 0x4C87;
			break;
		}

		case 12000:
		{
			divisor = 2;
			synthrate = 0x4650;
			break;
		}

		case 16000:
		{
			divisor = 1;
			synthrate = 0x6978;
			break;
		}

		case 22050:
		{
			divisor = 1;
			synthrate = 0x4C87;
			break;
		}

		case 24000:
		{
			divisor = 1;
			synthrate = 0x4650;
			break;
		}

		case 32000:
		{
			divisor = 0;
			synthrate = 0x6978;
			break;
		}

		case 44100:
		{
			divisor = 0;
			synthrate = 0x4C87;
			break;
		}

		case 48000:
		{
			divisor = 0;
			synthrate = 0x4650;
			break;
		}

		default:
		{
			MAD_PRINT(KERN_ERR "Error! un-supported sample rate %u !!!\n", sample_rate);
			divisor = 0;
			synthrate = 0x4650;
			dma_reader->sample_rate = 48000;
			break;
		}
	}

	/* synthersizer setting update */
	_mhal_alsa_write2_mask_reg(0x3E4C, 0x0030, (divisor << 4)); /* set divisor */
	_mhal_alsa_write2_reg(0x3E4E, synthrate); /* DMA synthesizer N.F. */
	_mhal_alsa_write2_mask_reg(0x3E4C, 0x0145, 0x0145); /* enable DMA synthesizer */

	return 0;
}

/* Set channel mode to DMA Reader4 */
static int _mhal_alsa_dma_reader4_set_channel_mode(unsigned int channel_mode)
{
	struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader4;
	unsigned int buffer_size = 0;
	MAD_PRINT(KERN_INFO "Target channel mode is %u\n", channel_mode);

	dma_reader->channel_mode = channel_mode;
	buffer_size = ((dma_reader->sample_rate << dma_reader->channel_mode) * _MAD_DMA_READER2_QUEUE_SIZE) / 1000;
	if ((buffer_size % _MAD_BYTES_IN_LINE))
		buffer_size += (_MAD_BYTES_IN_LINE - (buffer_size % _MAD_BYTES_IN_LINE));

	dma_reader->buffer.size = (dma_reader->channel_mode == E_MONO) ? (buffer_size * 2) : buffer_size;
	dma_reader->period_size = dma_reader->buffer.size >> 2;
	dma_reader->high_threshold = dma_reader->buffer.size - (dma_reader->buffer.size >> 3);

	return 0;
}

/* Initiate PCM Capture1 */
static int _mhal_alsa_pcm_capture1_init(void)
{
	struct MStar_PCM_Capture_Struct *pcm_capture = &g_pcm_capture1;
	ptrdiff_t audio_pcm_capture_base_pa = 0;
	ptrdiff_t audio_pcm_capture_base_ba = 0;
	ptrdiff_t audio_pcm_capture_base_va = 0;
	unsigned int w_ptr_offset = 0;
	MAD_PRINT(KERN_INFO "Initiate MStar PCM Capture1 engine\n");

	if ((pcm_capture->initialized_status != MAD_TRUE) || (pcm_capture->str_mode_info.status != E_RESUME)) {
		audio_pcm_capture_base_pa = ((ptrdiff_t)_mhal_alsa_read_reg(0x2A90) * _MAD_ADDR_CONVERTOR) + _MAD_PCM_CAPTURE1_BASE_OFFSET;
		audio_pcm_capture_base_pa += (((_mhal_alsa_read_reg(0x2AC0) & 0x0F) << 16) * _MAD_ADDR_CONVERTOR);
		audio_pcm_capture_base_ba = audio_pcm_capture_base_pa + _MAD_PHYSICAL_ADDR_TO_BUS_ADDR;

		if ((audio_pcm_capture_base_ba % 0x1000)) {
			MAD_PRINT(KERN_ERR "Error! Invalid MStar PCM capture bus address, it should be aligned by 4 KB!\n");
			return -EFAULT;
		}

		/* convert Bus Address to Virtual Address */
		pcm_capture->buffer.size = _MAD_PCM_CAPTURE1_BUF_SIZE;
		if (g_pcm_capture1_base_va == 0) {
			g_pcm_capture1_base_va = (ptrdiff_t)ioremap_nocache(audio_pcm_capture_base_ba, pcm_capture->buffer.size);
			if (g_pcm_capture1_base_va == 0) {
				MAD_PRINT(KERN_ERR "Error! fail to convert PCM Capture1 Buffer Bus Address to Virtual Address\n");
				return -ENOMEM;
			}
		}
		audio_pcm_capture_base_va = g_pcm_capture1_base_va;

		pcm_capture->str_mode_info.physical_addr = audio_pcm_capture_base_pa;
		pcm_capture->str_mode_info.bus_addr = audio_pcm_capture_base_ba;
		pcm_capture->str_mode_info.virtual_addr = audio_pcm_capture_base_va;

		pcm_capture->initialized_status = MAD_TRUE;
	}
	else {
		audio_pcm_capture_base_pa = pcm_capture->str_mode_info.physical_addr;
		audio_pcm_capture_base_ba = pcm_capture->str_mode_info.bus_addr;
		audio_pcm_capture_base_va = pcm_capture->str_mode_info.virtual_addr;
	}

	/* init PCM capture1 buffer address */
	pcm_capture->buffer.addr = (unsigned char *)audio_pcm_capture_base_va;
	//MAD_PRINT(KERN_INFO "PCM Capture1 buffer start address = 0x%08X\n", pcm_capture->buffer.addr);
	//MAD_PRINT(KERN_INFO "PCM Capture1 buffer end address = 0x%08X\n", (pcm_capture->buffer.addr + pcm_capture->buffer.size));

	/* clear all PCM capture1 buffer */
	memset((void *)pcm_capture->buffer.addr, 0x00, _MAD_PCM_CAPTURE1_BUF_SIZE);
	Chip_Flush_Memory();

	/* reset PCM capture1 write pointer */
	w_ptr_offset = _mhal_alsa_read_reg(0x2DF0) * _MAD_BYTES_IN_LINE;
	pcm_capture->buffer.w_ptr = pcm_capture->buffer.addr + w_ptr_offset;

	/* reset PCM capture1 read pointer */
	pcm_capture->buffer.r_ptr = pcm_capture->buffer.w_ptr;
	_mhal_alsa_write_reg(0x2DD4, (unsigned short)(w_ptr_offset / _MAD_BYTES_IN_LINE));

	/* reset PCM capture1 buffer size */
	_mhal_alsa_write_reg(0x2DD6, (unsigned short)(pcm_capture->buffer.size / _MAD_BYTES_IN_LINE));

	return 0;
}

/* Exit PCM Capture1 */
static int _mhal_alsa_pcm_capture1_exit(void)
{
	struct MStar_PCM_Capture_Struct *pcm_capture = &g_pcm_capture1;
	MAD_PRINT(KERN_INFO "Exit MStar PCM Capture1 engine\n");

	/* clear PCM capture1 buffer size */
	_mhal_alsa_write_reg(0x2DD6, 0x0000);

	/* clear PCM capture1 read pointer */
	_mhal_alsa_write_reg(0x2DD4, 0x0000);

	if (g_pcm_capture1_base_va != 0) {
		if (pcm_capture->buffer.addr) {
			iounmap((void *)pcm_capture->buffer.addr);
			pcm_capture->buffer.addr = 0;
		}
		else {
			MAD_PRINT(KERN_ERR "Error! MStar PCM Capture1 buffer address should not be 0 !\n");
		}

		g_pcm_capture1_base_va = 0;
	}

	pcm_capture->str_mode_info.status = E_STOP;
	pcm_capture->initialized_status = MAD_FALSE;

	return 0;
}

/* Start PCM Capture1 */
static int _mhal_alsa_pcm_capture1_start(void)
{
	struct MStar_PCM_Capture_Struct *pcm_capture = &g_pcm_capture1;
	//MAD_PRINT(KERN_INFO "Start MStar PCM Capture1 engine\n");

	pcm_capture->str_mode_info.status = E_START;

	return 0;
}

/* Stop PCM Capture1 */
static int _mhal_alsa_pcm_capture1_stop(void)
{
	//MAD_PRINT(KERN_INFO "Stop MStar PCM Capture1 engine\n");

	return 0;
}

/* Resume PCM Capture1 */
static int _mhal_alsa_pcm_capture1_resume(void)
{
	struct MStar_PCM_Capture_Struct *pcm_capture = &g_pcm_capture1;
	//MAD_PRINT(KERN_INFO "Resume MStar PCM Capture1 engine\n");

	pcm_capture->str_mode_info.status = E_RESUME;

	return 0;
}

/* Suspend PCM Capture1 */
static int _mhal_alsa_pcm_capture1_suspend(void)
{
	struct MStar_PCM_Capture_Struct *pcm_capture = &g_pcm_capture1;
	//MAD_PRINT(KERN_INFO "Suspend MStar PCM Capture1 engine\n");

	pcm_capture->str_mode_info.status = E_SUSPEND;

	return 0;
}

/* Read PCM from PCM Capture1 */
static unsigned int _mhal_alsa_pcm_capture1_read(void *buffer, unsigned int bytes)
{
	struct MStar_PCM_Capture_Struct *pcm_capture = &g_pcm_capture1;
	unsigned int rest_size_to_buffer_end = (pcm_capture->buffer.addr + _MAD_PCM_CAPTURE1_BUF_SIZE) - pcm_capture->buffer.r_ptr;
	unsigned int r_ptr_offset = 0;
	unsigned int read_size = 0;
	//MAD_PRINT(KERN_INFO "Read PCM from PCM Capture1 engine\n");

	read_size = (rest_size_to_buffer_end > bytes) ? bytes : rest_size_to_buffer_end;

	memcpy(buffer, pcm_capture->buffer.r_ptr, read_size);
	Chip_Flush_Memory();

	pcm_capture->buffer.r_ptr += read_size;
	if (pcm_capture->buffer.r_ptr == (pcm_capture->buffer.addr + _MAD_PCM_CAPTURE1_BUF_SIZE))
		pcm_capture->buffer.r_ptr = pcm_capture->buffer.addr;

	r_ptr_offset = pcm_capture->buffer.r_ptr - pcm_capture->buffer.addr;
	_mhal_alsa_write_reg(0x2DD4, (unsigned short)(r_ptr_offset / _MAD_BYTES_IN_LINE));

	return read_size;
}

/* Get infromation from PCM Capture1 */
static int _mhal_alsa_pcm_capture1_get(int cmd, unsigned int *param)
{
	int err = 0;
	//MAD_PRINT(KERN_INFO "Get parameter from PCM Capture1 engine\n");

	switch(cmd) {
		case E_PCM_CAPTURE_GET_NEW_PCM_AVAIL_BYTES:
		{
			*param = _mhal_alsa_pcm_capture1_get_new_avail_bytes();
			break;
		}

		case E_PCM_CAPTURE_GET_TOTAL_PCM_AVAIL_BYTES:
		{
			*param = _mhal_alsa_pcm_capture1_get_total_avail_bytes();
			break;
		}

		case E_PCM_CAPTURE_GET_DEVICE_STATUS:
		{
			*param = _mhal_alsa_get_device_status();
			break;
		}

		default:
		{
			MAD_PRINT(KERN_INFO "Invalid GET command %d\n", cmd);
			err = -EINVAL;
			break;
		}
	}

	return err;
}

/* Set information to PCM Capture1 */
static int _mhal_alsa_pcm_capture1_set(int cmd, unsigned int *param)
{
	int err = 0;
	//MAD_PRINT(KERN_INFO "Set parameter to PCM Capture1 engine\n");

	switch(cmd) {
		case E_PCM_CAPTURE_SET_BUFFER_SIZE:
		{
			if (*param > _MAD_PCM_CAPTURE1_BUF_SIZE) {
				*param = _MAD_PCM_CAPTURE1_BUF_SIZE;
				MAD_PRINT(KERN_INFO "Target buffer is too large, reset to %u\n", *param);
			}

			if ((*param % _MAD_BYTES_IN_LINE)) {
				*param = (*param / _MAD_BYTES_IN_LINE) * _MAD_BYTES_IN_LINE;
				MAD_PRINT(KERN_INFO "Target buffer is not aligned, reset to %u\n", *param);
			}

			_mhal_alsa_pcm_capture1_set_buffer_size(*param);
			break;
		}

		default:
		{
			MAD_PRINT(KERN_INFO "Invalid SET command %d\n", cmd);
			err = -EINVAL;
			break;
		}
	}

	return err;
}

/* Get PCM Capture1's new PCM available bytes */
static unsigned int _mhal_alsa_pcm_capture1_get_new_avail_bytes(void)
{
	struct MStar_PCM_Capture_Struct *pcm_capture = &g_pcm_capture1;
	unsigned char *new_w_ptr = NULL;
	unsigned int new_w_ptr_offset = 0;
	int new_avail_bytes = 0;

	new_w_ptr_offset = _mhal_alsa_read_reg(0x2DF0) * _MAD_BYTES_IN_LINE;
	new_w_ptr = pcm_capture->buffer.addr + new_w_ptr_offset;

	new_avail_bytes = new_w_ptr - pcm_capture->buffer.w_ptr;
	if (new_avail_bytes < 0)
		new_avail_bytes += _MAD_PCM_CAPTURE1_BUF_SIZE;

	pcm_capture->buffer.w_ptr = new_w_ptr;

	return new_avail_bytes;
}

/* Get PCM Capture1's total PCM available bytes */
static unsigned int _mhal_alsa_pcm_capture1_get_total_avail_bytes(void)
{
	struct MStar_PCM_Capture_Struct *pcm_capture = &g_pcm_capture1;
	unsigned int r_ptr_offset = 0;
	unsigned int w_ptr_offset = 0;
	int avail_bytes = 0;
	int loop = 0;
	int loop_timeout = 100;

	r_ptr_offset = pcm_capture->buffer.r_ptr - pcm_capture->buffer.addr;
	w_ptr_offset = _mhal_alsa_read_reg(0x2DF0) * _MAD_BYTES_IN_LINE;

	avail_bytes = w_ptr_offset - r_ptr_offset;
	if (avail_bytes < 0)
		avail_bytes += _MAD_PCM_CAPTURE1_BUF_SIZE;

	if (avail_bytes >= (pcm_capture->buffer.size - _MAD_PCM_CAPTURE_BUF_UNIT)) {
		MAD_PRINT(KERN_INFO "***** Audio PCM Capture1 Buffer is overrun !! ***** \n");

		/* clear PCM capture buffer size */
		_mhal_alsa_write_reg(0x2DD6, 0x0000);

		/* clear PCM capture read pointer */
		_mhal_alsa_write_reg(0x2DD4, 0x0000);

		/* check if PCM capture receives reset command */
		while(_mhal_alsa_read_reg(0x2DF0) != 0) {
			mdelay(1);

			if ((++loop) >= loop_timeout)
				break;
		}

		/* reset PCM capture write pointer */
		w_ptr_offset = _mhal_alsa_read_reg(0x2DF0) * _MAD_BYTES_IN_LINE;
		pcm_capture->buffer.w_ptr = pcm_capture->buffer.addr + w_ptr_offset;

		/* reset PCM capture read pointer */
		pcm_capture->buffer.r_ptr = pcm_capture->buffer.w_ptr;
		_mhal_alsa_write_reg(0x2DD4, (unsigned short)(w_ptr_offset / _MAD_BYTES_IN_LINE));

		/* reset PCM capture buffer size */
		_mhal_alsa_write_reg(0x2DD6, (unsigned short)(pcm_capture->buffer.size / _MAD_BYTES_IN_LINE));

		return 0;
	}

	return avail_bytes;
}

/* Set PCM Capture1's PCM buffer size */
static int _mhal_alsa_pcm_capture1_set_buffer_size(unsigned int buffer_size)
{
	struct MStar_PCM_Capture_Struct *pcm_capture = &g_pcm_capture1;
	MAD_PRINT(KERN_INFO "Target buffer size is %u\n", buffer_size);

	pcm_capture->buffer.size = buffer_size;
	_mhal_alsa_write_reg(0x2DD6, (unsigned short)(buffer_size / _MAD_BYTES_IN_LINE));

	return 0;
}

/* Initiate PCM Capture2 */
static int _mhal_alsa_pcm_capture2_init(void)
{
	struct MStar_PCM_Capture_Struct *pcm_capture = &g_pcm_capture2;
	ptrdiff_t audio_pcm_capture_base_pa = 0;
	ptrdiff_t audio_pcm_capture_base_ba = 0;
	ptrdiff_t audio_pcm_capture_base_va = 0;
	unsigned int w_ptr_offset = 0;
	MAD_PRINT(KERN_INFO "Initiate MStar PCM Capture2 engine\n");

	if ((pcm_capture->initialized_status != MAD_TRUE) || (pcm_capture->str_mode_info.status != E_RESUME)) {
		audio_pcm_capture_base_pa = ((ptrdiff_t)_mhal_alsa_read_reg(0x2A90) * _MAD_ADDR_CONVERTOR) + _MAD_PCM_CAPTURE2_BASE_OFFSET;
		audio_pcm_capture_base_pa += (((_mhal_alsa_read_reg(0x2AC0) & 0x0F) << 16) * _MAD_ADDR_CONVERTOR);
		audio_pcm_capture_base_ba = audio_pcm_capture_base_pa + _MAD_PHYSICAL_ADDR_TO_BUS_ADDR;

		if ((audio_pcm_capture_base_ba % 0x1000)) {
			MAD_PRINT(KERN_ERR "Error! Invalid MStar PCM capture bus address, it should be aligned by 4 KB!\n");
			return -EFAULT;
		}

		/* convert Bus Address to Virtual Address */
		pcm_capture->buffer.size = _MAD_PCM_CAPTURE2_BUF_SIZE;
		if (g_pcm_capture2_base_va == 0) {
			g_pcm_capture2_base_va = (ptrdiff_t)ioremap_nocache(audio_pcm_capture_base_ba, pcm_capture->buffer.size);
			if (g_pcm_capture2_base_va == 0) {
				MAD_PRINT(KERN_ERR "Error! fail to convert PCM Capture2 Buffer Bus Address to Virtual Address\n");
				return -ENOMEM;
			}
		}
		audio_pcm_capture_base_va = g_pcm_capture2_base_va;

		pcm_capture->str_mode_info.physical_addr = audio_pcm_capture_base_pa;
		pcm_capture->str_mode_info.bus_addr = audio_pcm_capture_base_ba;
		pcm_capture->str_mode_info.virtual_addr = audio_pcm_capture_base_va;

		pcm_capture->initialized_status = MAD_TRUE;
	}
	else {
		audio_pcm_capture_base_pa = pcm_capture->str_mode_info.physical_addr;
		audio_pcm_capture_base_ba = pcm_capture->str_mode_info.bus_addr;
		audio_pcm_capture_base_va = pcm_capture->str_mode_info.virtual_addr;
	}

	/* init PCM capture2 buffer address */
	pcm_capture->buffer.addr = (unsigned char *)audio_pcm_capture_base_va;
	//MAD_PRINT(KERN_INFO "PCM Capture2 buffer start address = 0x%08X\n", pcm_capture->buffer.addr);
	//MAD_PRINT(KERN_INFO "PCM Capture2 buffer end address = 0x%08X\n", (pcm_capture->buffer.addr + pcm_capture->buffer.size));

	/* clear all PCM capture2 buffer */
	memset((void *)pcm_capture->buffer.addr, 0x00, _MAD_PCM_CAPTURE2_BUF_SIZE);
	Chip_Flush_Memory();

	/* reset PCM capture2 write pointer */
	w_ptr_offset = _mhal_alsa_read_reg(0x2DF4) * _MAD_BYTES_IN_LINE;
	pcm_capture->buffer.w_ptr = pcm_capture->buffer.addr + w_ptr_offset;

	/* reset PCM capture2 read pointer */
	pcm_capture->buffer.r_ptr = pcm_capture->buffer.w_ptr;
	_mhal_alsa_write_reg(0x2D38, (unsigned short)(w_ptr_offset / _MAD_BYTES_IN_LINE));

	/* reset PCM capture2 buffer size */
	_mhal_alsa_write_reg(0x2D3A, (unsigned short)(pcm_capture->buffer.size / _MAD_BYTES_IN_LINE));

	return 0;
}

/* Exit PCM Capture2 */
static int _mhal_alsa_pcm_capture2_exit(void)
{
	struct MStar_PCM_Capture_Struct *pcm_capture = &g_pcm_capture2;
	MAD_PRINT(KERN_INFO "Exit MStar PCM Capture2 engine\n");

	/* clear PCM capture2 buffer size */
	_mhal_alsa_write_reg(0x2D3A, 0x0000);

	/* clear PCM capture2 read pointer */
	_mhal_alsa_write_reg(0x2D38, 0x0000);

	if (g_pcm_capture2_base_va != 0) {
		if (pcm_capture->buffer.addr) {
			iounmap((void *)pcm_capture->buffer.addr);
			pcm_capture->buffer.addr = 0;
		}
		else {
			MAD_PRINT(KERN_ERR "Error! MStar PCM Capture2 buffer address should not be 0 !\n");
		}

		g_pcm_capture2_base_va = 0;
	}

	pcm_capture->str_mode_info.status = E_STOP;
	pcm_capture->initialized_status = MAD_FALSE;

	return 0;
}

/* Start PCM Capture2 */
static int _mhal_alsa_pcm_capture2_start(void)
{
	struct MStar_PCM_Capture_Struct *pcm_capture = &g_pcm_capture2;
	//MAD_PRINT(KERN_INFO "Start MStar PCM Capture2 engine\n");

	pcm_capture->str_mode_info.status = E_START;

	return 0;
}

/* Stop PCM Capture2 */
static int _mhal_alsa_pcm_capture2_stop(void)
{
	//MAD_PRINT(KERN_INFO "Stop MStar PCM Capture2 engine\n");

	return 0;
}

/* Resume PCM Capture2 */
static int _mhal_alsa_pcm_capture2_resume(void)
{
	struct MStar_PCM_Capture_Struct *pcm_capture = &g_pcm_capture2;
	//MAD_PRINT(KERN_INFO "Resume MStar PCM Capture2 engine\n");

	pcm_capture->str_mode_info.status = E_RESUME;

	return 0;
}

/* Suspend PCM Capture2 */
static int _mhal_alsa_pcm_capture2_suspend(void)
{
	struct MStar_PCM_Capture_Struct *pcm_capture = &g_pcm_capture2;
	//MAD_PRINT(KERN_INFO "Suspend MStar PCM Capture2 engine\n");

	pcm_capture->str_mode_info.status = E_SUSPEND;

	return 0;
}

/* Read PCM from PCM Capture2 */
static unsigned int _mhal_alsa_pcm_capture2_read(void *buffer, unsigned int bytes)
{
	struct MStar_PCM_Capture_Struct *pcm_capture = &g_pcm_capture2;
	unsigned int rest_size_to_buffer_end = (pcm_capture->buffer.addr + _MAD_PCM_CAPTURE2_BUF_SIZE) - pcm_capture->buffer.r_ptr;
	unsigned int r_ptr_offset = 0;
	unsigned int read_size = 0;
	//MAD_PRINT(KERN_INFO "Read PCM from PCM Capture2 engine\n");

	read_size = (rest_size_to_buffer_end > bytes) ? bytes : rest_size_to_buffer_end;

	memcpy(buffer, pcm_capture->buffer.r_ptr, read_size);
	Chip_Flush_Memory();

	pcm_capture->buffer.r_ptr += read_size;
	if (pcm_capture->buffer.r_ptr == (pcm_capture->buffer.addr + _MAD_PCM_CAPTURE2_BUF_SIZE))
		pcm_capture->buffer.r_ptr = pcm_capture->buffer.addr;

	r_ptr_offset = pcm_capture->buffer.r_ptr - pcm_capture->buffer.addr;
	_mhal_alsa_write_reg(0x2D38, (unsigned short)(r_ptr_offset / _MAD_BYTES_IN_LINE));

	return read_size;
}

/* Get infromation from PCM Capture2 */
static int _mhal_alsa_pcm_capture2_get(int cmd, unsigned int *param)
{
	int err = 0;
	//MAD_PRINT(KERN_INFO "Get parameter from PCM Capture2 engine\n");

	switch(cmd) {
		case E_PCM_CAPTURE_GET_NEW_PCM_AVAIL_BYTES:
		{
			*param = _mhal_alsa_pcm_capture2_get_new_avail_bytes();
			break;
		}

		case E_PCM_CAPTURE_GET_TOTAL_PCM_AVAIL_BYTES:
		{
			*param = _mhal_alsa_pcm_capture2_get_total_avail_bytes();
			break;
		}

		case E_PCM_CAPTURE_GET_DEVICE_STATUS:
		{
			*param = _mhal_alsa_get_device_status();
			break;
		}

		default:
		{
			MAD_PRINT(KERN_INFO "Invalid GET command %d\n", cmd);
			err = -EINVAL;
			break;
		}
	}

	return err;
}

/* Set information to PCM Capture2 */
static int _mhal_alsa_pcm_capture2_set(int cmd, unsigned int *param)
{
	int err = 0;
	//MAD_PRINT(KERN_INFO "Set parameter to PCM Capture2 engine\n");

	switch(cmd) {
		case E_PCM_CAPTURE_SET_BUFFER_SIZE:
		{
			if (*param > _MAD_PCM_CAPTURE2_BUF_SIZE) {
				*param = _MAD_PCM_CAPTURE2_BUF_SIZE;
				MAD_PRINT(KERN_INFO "Target buffer is too large, reset to %u\n", *param);
			}

			if ((*param % _MAD_BYTES_IN_LINE)) {
				*param = (*param / _MAD_BYTES_IN_LINE) * _MAD_BYTES_IN_LINE;
				MAD_PRINT(KERN_INFO "Target buffer is not aligned, reset to %u\n", *param);
			}

			_mhal_alsa_pcm_capture2_set_buffer_size(*param);
			break;
		}

		default:
		{
			MAD_PRINT(KERN_INFO "Invalid SET command %d\n", cmd);
			err = -EINVAL;
			break;
		}
	}

	return err;
}

/* Get PCM Capture2's new PCM available bytes */
static unsigned int _mhal_alsa_pcm_capture2_get_new_avail_bytes(void)
{
	struct MStar_PCM_Capture_Struct *pcm_capture = &g_pcm_capture2;
	unsigned char *new_w_ptr = NULL;
	unsigned int new_w_ptr_offset = 0;
	int new_avail_bytes = 0;

	new_w_ptr_offset = _mhal_alsa_read_reg(0x2DF4) * _MAD_BYTES_IN_LINE;
	new_w_ptr = pcm_capture->buffer.addr + new_w_ptr_offset;

	new_avail_bytes = new_w_ptr - pcm_capture->buffer.w_ptr;
	if (new_avail_bytes < 0)
		new_avail_bytes += _MAD_PCM_CAPTURE2_BUF_SIZE;

	pcm_capture->buffer.w_ptr = new_w_ptr;

	return new_avail_bytes;
}

/* Get PCM Capture2's total PCM available bytes */
static unsigned int _mhal_alsa_pcm_capture2_get_total_avail_bytes(void)
{
	struct MStar_PCM_Capture_Struct *pcm_capture = &g_pcm_capture2;
	unsigned int r_ptr_offset = 0;
	unsigned int w_ptr_offset = 0;
	int avail_bytes = 0;
	int loop = 0;
	int loop_timeout = 100;

	r_ptr_offset = pcm_capture->buffer.r_ptr - pcm_capture->buffer.addr;
	w_ptr_offset = _mhal_alsa_read_reg(0x2DF4) * _MAD_BYTES_IN_LINE;

	avail_bytes = w_ptr_offset - r_ptr_offset;
	if (avail_bytes < 0)
		avail_bytes += _MAD_PCM_CAPTURE2_BUF_SIZE;

	if (avail_bytes >= (pcm_capture->buffer.size - _MAD_PCM_CAPTURE_BUF_UNIT)) {
		MAD_PRINT(KERN_INFO "***** Audio PCM Capture2 Buffer is overrun !! ***** \n");

		/* clear PCM capture buffer size */
		_mhal_alsa_write_reg(0x2D3A, 0x0000);

		/* clear PCM capture read pointer */
		_mhal_alsa_write_reg(0x2D38, 0x0000);

		/* check if PCM capture receives reset command */
		while(_mhal_alsa_read_reg(0x2DF4) != 0) {
			mdelay(1);

			if ((++loop) >= loop_timeout)
				break;
		}

		/* reset PCM capture write pointer */
		w_ptr_offset = _mhal_alsa_read_reg(0x2DF4) * _MAD_BYTES_IN_LINE;
		pcm_capture->buffer.w_ptr = pcm_capture->buffer.addr + w_ptr_offset;

		/* reset PCM capture read pointer */
		pcm_capture->buffer.r_ptr = pcm_capture->buffer.w_ptr;
		_mhal_alsa_write_reg(0x2D38, (unsigned short)(w_ptr_offset / _MAD_BYTES_IN_LINE));

		/* reset PCM capture buffer size */
		_mhal_alsa_write_reg(0x2D3A, (unsigned short)(pcm_capture->buffer.size / _MAD_BYTES_IN_LINE));

		return 0;
	}

	return avail_bytes;
}

/* Set PCM Capture2's PCM buffer size */
static int _mhal_alsa_pcm_capture2_set_buffer_size(unsigned int buffer_size)
{
	struct MStar_PCM_Capture_Struct *pcm_capture = &g_pcm_capture2;
	MAD_PRINT(KERN_INFO "Target buffer size is %u\n", buffer_size);

	pcm_capture->buffer.size = buffer_size;
	_mhal_alsa_write_reg(0x2D3A, (unsigned short)(buffer_size / _MAD_BYTES_IN_LINE));

	return 0;
}

static int __init _mhal_alsa_init(void)
{
	int err = 0;
	unsigned int str_len = 0;

	MAD_PRINT(KERN_INFO "Initiate MStar ALSA core driver\n");

	memset(&MStar_MAD, 0x00, sizeof(struct MStar_MAD_Info));

	str_len = (strlen(_MAD_CHIP_MODEL) > sizeof(MStar_MAD.name)) ? sizeof(MStar_MAD.name) : strlen(_MAD_CHIP_MODEL);
	memcpy(MStar_MAD.name, _MAD_CHIP_MODEL, str_len);
	sprintf(MStar_MAD.version, "%d.%d.%d", _MAD_ALSA_HAL_VERSION_MAJOR, _MAD_ALSA_HAL_VERSION_MINOR, _MAD_ALSA_HAL_VERSION_REVISION);

	/* Hook Playback Operators */
	MStar_MAD.playback_pcm_ops[0] = &MStar_DMA_Reader_Ops;
	MStar_MAD.playback_pcm_ops[1] = &MStar_DMA_Reader2_Ops;
	MStar_MAD.playback_pcm_ops[2] = &MStar_DMA_Reader3_Ops;
	MStar_MAD.playback_pcm_ops[3] = &MStar_DMA_Reader4_Ops;

	/* Hook Capture Operators */
	MStar_MAD.capture_pcm_ops[0] = &MStar_PCM_Capture1_Ops;
	MStar_MAD.capture_pcm_ops[1] = &MStar_PCM_Capture2_Ops;

	err = _mdrv_alsa_hook_device(&MStar_MAD);
	if (err < 0) {
		MAD_PRINT(KERN_ERR "Error(%d)! fail to hook PCM operators\n", err);
		return err;
	}

	return 0;
}

static void __exit _mhal_alsa_exit(void)
{
	int err = 0;

	MAD_PRINT(KERN_INFO "Exit MStar ALSA core driver\n");

	err = _mdrv_alsa_unhook_device();
	if (err < 0) {
		MAD_PRINT(KERN_ERR "Error(%d)! fail to unhook PCM operators\n", err);
		return;
	}

	return;
}

/*
 * ============================================================================
 * Module Information
 * ============================================================================
 */
module_init(_mhal_alsa_init);
module_exit(_mhal_alsa_exit);

MODULE_AUTHOR("MStar Semiconductor, Inc.");
MODULE_DESCRIPTION("MStar ALSA Driver - HAL Layer");
MODULE_SUPPORTED_DEVICE("MAD DEVICE");
MODULE_LICENSE("Proprietary");
