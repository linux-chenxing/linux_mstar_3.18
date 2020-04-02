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
#define _MAD_CHIP_MODEL    "MAD MACAN"

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

#define _MAD_DSP2_DRAM_SIZE    0x310000
#define _MAD_DSP2_TO_COMMON_DRAM_OFFSET    (_MAD_DSP2_DRAM_SIZE)

#define _MAD_MAJOR_VERSION_MASK    0xFFFF0000
#define _MAD_MINOR_VERSION_MASK    0x0000FFFF

#define _MAD_SW_MIXER_CLIENT_VERSION    0x00020001

#define _MAD_SW_MIXER1    "SW MIXER1"
#define _MAD_SW_MIXER2    "SW MIXER2"
#define _MAD_SW_MIXER3    "SW MIXER3"
#define _MAD_SW_MIXER4    "SW MIXER4"
#define _MAD_SW_MIXER5    "SW MIXER5"
#define _MAD_SW_MIXER6    "SW MIXER6"
#define _MAD_SW_MIXER7    "SW MIXER7"
#define _MAD_SW_MIXER8    "SW MIXER8"

#define _MAD_PCM_MAXIMUM                20
#define _MAD_PCM_SWMIXER_SERVER_MAXIMUM    1
#define _MAD_PCM_SWMIXER_CLIENT_MAXIMUM    8
#define _MAD_PCM_SWMIXER_CHANNEL    2

#define _MAD_PCM_SWMIXER_GROUP_NONE    0x00000000
#define _MAD_PCM_SWMIXER_GROUP_ALL    0xFFFFFFFF
#define _MAD_PCM_SWMIXER_GROUP_1    0x00000001
#define _MAD_PCM_SWMIXER_GROUP_2    0x00000002
#define _MAD_PCM_SWMIXER_GROUP_3    0x00000004
#define _MAD_PCM_SWMIXER_GROUP_4    0x00000008
#define _MAD_PCM_SWMIXER_GROUP_5    0x00000010
#define _MAD_PCM_SWMIXER_GROUP_6    0x00000020
#define _MAD_PCM_SWMIXER_GROUP_7    0x00000040
#define _MAD_PCM_SWMIXER_GROUP_8    0x00000080
#define _MAD_PCM_SWMIXER_GROUP_MAX    _MAD_PCM_SWMIXER_GROUP_1

#define _MAD_PCM_SWMIXER_CLIENT_INFO_BASE_OFFSET    (0x378000 + _MAD_DSP2_TO_COMMON_DRAM_OFFSET)
#define _MAD_PCM_SWMIXER_CLIENT_INFO_TOTAL_SIZE    0x4000
#define _MAD_PCM_SWMIXER_CLIENT_BUFFER_BASE_OFFSET    (0x37C000 + _MAD_DSP2_TO_COMMON_DRAM_OFFSET)
#define _MAD_PCM_SWMIXER_CLIENT_BUFFER_TOTAL_SIZE    0x78000
#define _MAD_PCM_SWMIXER_CLIENT_BUFFER_SIZE    (_MAD_PCM_SWMIXER_CLIENT_BUFFER_TOTAL_SIZE / _MAD_PCM_SWMIXER_CLIENT_MAXIMUM)
#define _MAD_PCM_SWMIXER_CLIENT_PERIOD_SIZE    (_MAD_PCM_SWMIXER_CLIENT_BUFFER_SIZE >> 3)
#define _MAD_PCM_SWMIXER_CLIENT_MIN_AVAIL    _MAD_PCM_SWMIXER_CLIENT_PERIOD_SIZE
#define _MAD_PCM_SWMIXER_CLIENT_HIGH_THRESHOLD    (_MAD_PCM_SWMIXER_CLIENT_BUFFER_SIZE - _MAD_PCM_SWMIXER_CLIENT_PERIOD_SIZE)

#define _MAD_DMA_READER2_BASE_OFFSET    (0x147000 + _MAD_DSP2_TO_COMMON_DRAM_OFFSET)
#define _MAD_DMA_READER2_BUF_UNIT    4096
#define _MAD_DMA_READER2_TOTAL_BUF_SIZE    0x10000 /* 64 KByte */
#define _MAD_DMA_READER2_MIN_AVAIL    (_MAD_DMA_READER2_TOTAL_BUF_SIZE >> 3) /* The minimal available size should be reserved */
#define _MAD_DMA_READER2_HIGH_THRESHOLD    (_MAD_DMA_READER2_TOTAL_BUF_SIZE - _MAD_DMA_READER2_MIN_AVAIL)
#define _MAD_DMA_READER2_BUF_SIZE    _MAD_DMA_READER2_TOTAL_BUF_SIZE
#define _MAD_DMA_READER2_PERIOD_SIZE    (_MAD_DMA_READER2_BUF_SIZE >> 2)
#define _MAD_DMA_READER2_QUEUE_SIZE    100 /* ms */

#define _MAD_PCM_CAPTURE1_BASE_OFFSET   (0x30000 + _MAD_DSP2_TO_COMMON_DRAM_OFFSET)
#define _MAD_PCM_CAPTURE2_BASE_OFFSET   (0x3C000 + _MAD_DSP2_TO_COMMON_DRAM_OFFSET)
#define _MAD_PCM_CAPTURE_BUF_UNIT    128
#define _MAD_PCM_CAPTURE_BUF_SIZE    0xC000 /* 48 KByte */

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
#define _MAD_DO_ALIGNMENT(Value, AlignmentSize) \
	do { \
		Value = (Value /AlignmentSize) * AlignmentSize; \
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
#if 0 /* It's not in used for the moment, might be TODO */
static void _mhal_alsa_write_mask_reg(unsigned int u32RegAddr, unsigned short u16Mask, unsigned short u16Val);
static unsigned char _mhal_alsa_read2_byte(unsigned int u32RegAddr);
#endif
static unsigned short _mhal_alsa_read2_reg(unsigned int u32RegAddr);
#if 0
static void _mhal_alsa_write2_byte(unsigned int u32RegAddr, unsigned char u8Val);
static void _mhal_alsa_write2_mask_byte(unsigned int u32RegAddr, unsigned char u8Mask, unsigned char u8Val);
#endif
static void _mhal_alsa_write2_reg(unsigned int u32RegAddr, unsigned short u16Val);
static void _mhal_alsa_write2_mask_reg(unsigned int u32RegAddr, unsigned short u16Mask, unsigned short u16Val);

/* Get Device Status */
static unsigned int _mhal_alsa_get_device_status(void);

/* PCM Mixer Client */
static int _mhal_alsa_pcm_mixer_client_init(char *target_name, struct MStar_PCM_SWMixer_Client_Struct *pcm_playback);
static int _mhal_alsa_pcm_mixer_client_exit(struct MStar_PCM_SWMixer_Client_Struct *pcm_playback);
static int _mhal_alsa_pcm_mixer_client_start(struct MStar_PCM_SWMixer_Client_Struct *pcm_playback);
static int _mhal_alsa_pcm_mixer_client_stop(struct MStar_PCM_SWMixer_Client_Struct *pcm_playback);
static int _mhal_alsa_pcm_mixer_client_resume(struct MStar_PCM_SWMixer_Client_Struct *pcm_playback);
static int _mhal_alsa_pcm_mixer_client_suspend(struct MStar_PCM_SWMixer_Client_Struct *pcm_playback);
static unsigned int _mhal_alsa_pcm_mixer_client_write(struct MStar_PCM_SWMixer_Client_Struct *pcm_playback, void *buffer, unsigned int bytes);
static int _mhal_alsa_pcm_mixer_client_get(struct MStar_PCM_SWMixer_Client_Struct *pcm_playback, int cmd, unsigned int *param);
static int _mhal_alsa_pcm_mixer_client_set(struct MStar_PCM_SWMixer_Client_Struct *pcm_playback, int cmd, unsigned int *param);
static int _mhal_alsa_pcm_mixer_client_get_inused_bytes(struct MStar_PCM_SWMixer_Client_Struct *pcm_playback);
static int _mhal_alsa_pcm_mixer_client_get_avail_bytes(struct MStar_PCM_SWMixer_Client_Struct *pcm_playback);
static int _mhal_alsa_pcm_mixer_client_set_sample_rate(struct MStar_PCM_SWMixer_Client_Struct *pcm_playback, unsigned int sample_rate);
static int _mhal_alsa_pcm_mixer_client_set_channel_mode(struct MStar_PCM_SWMixer_Client_Struct *pcm_playback, unsigned int channel_mode);

/* PCM Mixer1 Group 0 */
static int _mhal_alsa_pcm_mixer1_group0_init(void);
static int _mhal_alsa_pcm_mixer1_group0_exit(void);
static int _mhal_alsa_pcm_mixer1_group0_start(void);
static int _mhal_alsa_pcm_mixer1_group0_stop(void);
static int _mhal_alsa_pcm_mixer1_group0_resume(void);
static int _mhal_alsa_pcm_mixer1_group0_suspend(void);
static unsigned int _mhal_alsa_pcm_mixer1_group0_write(void* buffer, unsigned int bytes);
static int _mhal_alsa_pcm_mixer1_group0_get(int cmd, unsigned int *param);
static int _mhal_alsa_pcm_mixer1_group0_set(int cmd, unsigned int *param);

/* PCM Mixer2 Group 0 */
static int _mhal_alsa_pcm_mixer2_group0_init(void);
static int _mhal_alsa_pcm_mixer2_group0_exit(void);
static int _mhal_alsa_pcm_mixer2_group0_start(void);
static int _mhal_alsa_pcm_mixer2_group0_stop(void);
static int _mhal_alsa_pcm_mixer2_group0_resume(void);
static int _mhal_alsa_pcm_mixer2_group0_suspend(void);
static unsigned int _mhal_alsa_pcm_mixer2_group0_write(void* buffer, unsigned int bytes);
static int _mhal_alsa_pcm_mixer2_group0_get(int cmd, unsigned int *param);
static int _mhal_alsa_pcm_mixer2_group0_set(int cmd, unsigned int *param);

/* PCM Mixer3 Group 0 */
static int _mhal_alsa_pcm_mixer3_group0_init(void);
static int _mhal_alsa_pcm_mixer3_group0_exit(void);
static int _mhal_alsa_pcm_mixer3_group0_start(void);
static int _mhal_alsa_pcm_mixer3_group0_stop(void);
static int _mhal_alsa_pcm_mixer3_group0_resume(void);
static int _mhal_alsa_pcm_mixer3_group0_suspend(void);
static unsigned int _mhal_alsa_pcm_mixer3_group0_write(void* buffer, unsigned int bytes);
static int _mhal_alsa_pcm_mixer3_group0_get(int cmd, unsigned int *param);
static int _mhal_alsa_pcm_mixer3_group0_set(int cmd, unsigned int *param);

/* PCM Mixer4 Group 0 */
static int _mhal_alsa_pcm_mixer4_group0_init(void);
static int _mhal_alsa_pcm_mixer4_group0_exit(void);
static int _mhal_alsa_pcm_mixer4_group0_start(void);
static int _mhal_alsa_pcm_mixer4_group0_stop(void);
static int _mhal_alsa_pcm_mixer4_group0_resume(void);
static int _mhal_alsa_pcm_mixer4_group0_suspend(void);
static unsigned int _mhal_alsa_pcm_mixer4_group0_write(void* buffer, unsigned int bytes);
static int _mhal_alsa_pcm_mixer4_group0_get(int cmd, unsigned int *param);
static int _mhal_alsa_pcm_mixer4_group0_set(int cmd, unsigned int *param);

/* PCM Mixer5 Group 0 */
static int _mhal_alsa_pcm_mixer5_group0_init(void);
static int _mhal_alsa_pcm_mixer5_group0_exit(void);
static int _mhal_alsa_pcm_mixer5_group0_start(void);
static int _mhal_alsa_pcm_mixer5_group0_stop(void);
static int _mhal_alsa_pcm_mixer5_group0_resume(void);
static int _mhal_alsa_pcm_mixer5_group0_suspend(void);
static unsigned int _mhal_alsa_pcm_mixer5_group0_write(void* buffer, unsigned int bytes);
static int _mhal_alsa_pcm_mixer5_group0_get(int cmd, unsigned int *param);
static int _mhal_alsa_pcm_mixer5_group0_set(int cmd, unsigned int *param);

/* PCM Mixer6 Group 0 */
static int _mhal_alsa_pcm_mixer6_group0_init(void);
static int _mhal_alsa_pcm_mixer6_group0_exit(void);
static int _mhal_alsa_pcm_mixer6_group0_start(void);
static int _mhal_alsa_pcm_mixer6_group0_stop(void);
static int _mhal_alsa_pcm_mixer6_group0_resume(void);
static int _mhal_alsa_pcm_mixer6_group0_suspend(void);
static unsigned int _mhal_alsa_pcm_mixer6_group0_write(void* buffer, unsigned int bytes);
static int _mhal_alsa_pcm_mixer6_group0_get(int cmd, unsigned int *param);
static int _mhal_alsa_pcm_mixer6_group0_set(int cmd, unsigned int *param);

/* PCM Mixer7 Group 0 */
static int _mhal_alsa_pcm_mixer7_group0_init(void);
static int _mhal_alsa_pcm_mixer7_group0_exit(void);
static int _mhal_alsa_pcm_mixer7_group0_start(void);
static int _mhal_alsa_pcm_mixer7_group0_stop(void);
static int _mhal_alsa_pcm_mixer7_group0_resume(void);
static int _mhal_alsa_pcm_mixer7_group0_suspend(void);
static unsigned int _mhal_alsa_pcm_mixer7_group0_write(void* buffer, unsigned int bytes);
static int _mhal_alsa_pcm_mixer7_group0_get(int cmd, unsigned int *param);
static int _mhal_alsa_pcm_mixer7_group0_set(int cmd, unsigned int *param);

/* PCM Mixer8 Group 0 */
static int _mhal_alsa_pcm_mixer8_group0_init(void);
static int _mhal_alsa_pcm_mixer8_group0_exit(void);
static int _mhal_alsa_pcm_mixer8_group0_start(void);
static int _mhal_alsa_pcm_mixer8_group0_stop(void);
static int _mhal_alsa_pcm_mixer8_group0_resume(void);
static int _mhal_alsa_pcm_mixer8_group0_suspend(void);
static unsigned int _mhal_alsa_pcm_mixer8_group0_write(void* buffer, unsigned int bytes);
static int _mhal_alsa_pcm_mixer8_group0_get(int cmd, unsigned int *param);
static int _mhal_alsa_pcm_mixer8_group0_set(int cmd, unsigned int *param);

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
static struct MStar_MAD_Info MStar_MAD[MSTAR_SND_CARDS];

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

/* MStar Audio DSP - PCM Mixer1 Group0 */
static struct MStar_PCM_SWMixer_Client_Struct g_pcm_mixer1_group0 = {
	.str_mode_info[0] = {
		.status = E_STOP,
		.physical_addr = 0,
		.bus_addr = 0,
		.virtual_addr = 0,
	},
	.str_mode_info[1] = {
		.status = E_STOP,
		.physical_addr = 0,
		.bus_addr = 0,
		.virtual_addr = 0,
	},
	.pcm_info = NULL,
	.pcm_buffer_info = NULL,
	.client_id = 0,
	.initialized_status = MAD_FALSE,
	.period_size = _MAD_PCM_SWMIXER_CLIENT_PERIOD_SIZE,
	.written_size = 0,
	.group_id = _MAD_PCM_SWMIXER_GROUP_1,
};

static struct MStar_MAD_Ops MStar_PCM_Mixer1_group0_Ops = {
	.open = _mhal_alsa_pcm_mixer1_group0_init,
	.close = _mhal_alsa_pcm_mixer1_group0_exit,
	.start = _mhal_alsa_pcm_mixer1_group0_start,
	.stop = _mhal_alsa_pcm_mixer1_group0_stop,
	.resume = _mhal_alsa_pcm_mixer1_group0_resume,
	.suspend = _mhal_alsa_pcm_mixer1_group0_suspend,
	.read = NULL,
	.write = _mhal_alsa_pcm_mixer1_group0_write,
	.get = _mhal_alsa_pcm_mixer1_group0_get,
	.set = _mhal_alsa_pcm_mixer1_group0_set,
};

/* MStar Audio DSP - PCM Mixer2 Group0 */
static struct MStar_PCM_SWMixer_Client_Struct g_pcm_mixer2_group0 = {
	.str_mode_info[0] = {
		.status = E_STOP,
		.physical_addr = 0,
		.bus_addr = 0,
		.virtual_addr = 0,
	},
	.str_mode_info[1] = {
		.status = E_STOP,
		.physical_addr = 0,
		.bus_addr = 0,
		.virtual_addr = 0,
	},
	.pcm_info = NULL,
	.pcm_buffer_info = NULL,
	.client_id = 1,
	.initialized_status = MAD_FALSE,
	.period_size = _MAD_PCM_SWMIXER_CLIENT_PERIOD_SIZE,
	.written_size = 0,
	.group_id = _MAD_PCM_SWMIXER_GROUP_1,
};

static struct MStar_MAD_Ops MStar_PCM_Mixer2_group0_Ops = {
	.open = _mhal_alsa_pcm_mixer2_group0_init,
	.close = _mhal_alsa_pcm_mixer2_group0_exit,
	.start = _mhal_alsa_pcm_mixer2_group0_start,
	.stop = _mhal_alsa_pcm_mixer2_group0_stop,
	.resume = _mhal_alsa_pcm_mixer2_group0_resume,
	.suspend = _mhal_alsa_pcm_mixer2_group0_suspend,
	.read = NULL,
	.write = _mhal_alsa_pcm_mixer2_group0_write,
	.get = _mhal_alsa_pcm_mixer2_group0_get,
	.set = _mhal_alsa_pcm_mixer2_group0_set,
};

/* MStar Audio DSP - PCM Mixer3 Group0 */
static struct MStar_PCM_SWMixer_Client_Struct g_pcm_mixer3_group0 = {
	.str_mode_info[0] = {
		.status = E_STOP,
		.physical_addr = 0,
		.bus_addr = 0,
		.virtual_addr = 0,
	},
	.str_mode_info[1] = {
		.status = E_STOP,
		.physical_addr = 0,
		.bus_addr = 0,
		.virtual_addr = 0,
	},
	.pcm_info = NULL,
	.pcm_buffer_info = NULL,
	.client_id = 2,
	.initialized_status = MAD_FALSE,
	.period_size = _MAD_PCM_SWMIXER_CLIENT_PERIOD_SIZE,
	.written_size = 0,
	.group_id = _MAD_PCM_SWMIXER_GROUP_1,
};

static struct MStar_MAD_Ops MStar_PCM_Mixer3_group0_Ops = {
	.open = _mhal_alsa_pcm_mixer3_group0_init,
	.close = _mhal_alsa_pcm_mixer3_group0_exit,
	.start = _mhal_alsa_pcm_mixer3_group0_start,
	.stop = _mhal_alsa_pcm_mixer3_group0_stop,
	.resume = _mhal_alsa_pcm_mixer3_group0_resume,
	.suspend = _mhal_alsa_pcm_mixer3_group0_suspend,
	.read = NULL,
	.write = _mhal_alsa_pcm_mixer3_group0_write,
	.get = _mhal_alsa_pcm_mixer3_group0_get,
	.set = _mhal_alsa_pcm_mixer3_group0_set,
};

/* MStar Audio DSP - PCM Mixer4 Group0 */
static struct MStar_PCM_SWMixer_Client_Struct g_pcm_mixer4_group0 = {
	.str_mode_info[0] = {
		.status = E_STOP,
		.physical_addr = 0,
		.bus_addr = 0,
		.virtual_addr = 0,
	},
	.str_mode_info[1] = {
		.status = E_STOP,
		.physical_addr = 0,
		.bus_addr = 0,
		.virtual_addr = 0,
	},
	.pcm_info = NULL,
	.pcm_buffer_info = NULL,
	.client_id = 3,
	.initialized_status = MAD_FALSE,
	.period_size = _MAD_PCM_SWMIXER_CLIENT_PERIOD_SIZE,
	.written_size = 0,
	.group_id = _MAD_PCM_SWMIXER_GROUP_1,
};

static struct MStar_MAD_Ops MStar_PCM_Mixer4_group0_Ops = {
	.open = _mhal_alsa_pcm_mixer4_group0_init,
	.close = _mhal_alsa_pcm_mixer4_group0_exit,
	.start = _mhal_alsa_pcm_mixer4_group0_start,
	.stop = _mhal_alsa_pcm_mixer4_group0_stop,
	.resume = _mhal_alsa_pcm_mixer4_group0_resume,
	.suspend = _mhal_alsa_pcm_mixer4_group0_suspend,
	.read = NULL,
	.write = _mhal_alsa_pcm_mixer4_group0_write,
	.get = _mhal_alsa_pcm_mixer4_group0_get,
	.set = _mhal_alsa_pcm_mixer4_group0_set,
};

/* MStar Audio DSP - PCM Mixer5 Group0 */
static struct MStar_PCM_SWMixer_Client_Struct g_pcm_mixer5_group0 = {
	.str_mode_info[0] = {
		.status = E_STOP,
		.physical_addr = 0,
		.bus_addr = 0,
		.virtual_addr = 0,
	},
	.str_mode_info[1] = {
		.status = E_STOP,
		.physical_addr = 0,
		.bus_addr = 0,
		.virtual_addr = 0,
	},
	.pcm_info = NULL,
	.pcm_buffer_info = NULL,
	.client_id = 4,
	.initialized_status = MAD_FALSE,
	.period_size = _MAD_PCM_SWMIXER_CLIENT_PERIOD_SIZE,
	.written_size = 0,
	.group_id = _MAD_PCM_SWMIXER_GROUP_1,
};

static struct MStar_MAD_Ops MStar_PCM_Mixer5_group0_Ops = {
	.open = _mhal_alsa_pcm_mixer5_group0_init,
	.close = _mhal_alsa_pcm_mixer5_group0_exit,
	.start = _mhal_alsa_pcm_mixer5_group0_start,
	.stop = _mhal_alsa_pcm_mixer5_group0_stop,
	.resume = _mhal_alsa_pcm_mixer5_group0_resume,
	.suspend = _mhal_alsa_pcm_mixer5_group0_suspend,
	.read = NULL,
	.write = _mhal_alsa_pcm_mixer5_group0_write,
	.get = _mhal_alsa_pcm_mixer5_group0_get,
	.set = _mhal_alsa_pcm_mixer5_group0_set,
};

/* MStar Audio DSP - PCM Mixer6 Group0 */
static struct MStar_PCM_SWMixer_Client_Struct g_pcm_mixer6_group0 = {
	.str_mode_info[0] = {
		.status = E_STOP,
		.physical_addr = 0,
		.bus_addr = 0,
		.virtual_addr = 0,
	},
	.str_mode_info[1] = {
		.status = E_STOP,
		.physical_addr = 0,
		.bus_addr = 0,
		.virtual_addr = 0,
	},
	.pcm_info = NULL,
	.pcm_buffer_info = NULL,
	.client_id = 5,
	.initialized_status = MAD_FALSE,
	.period_size = _MAD_PCM_SWMIXER_CLIENT_PERIOD_SIZE,
	.written_size = 0,
	.group_id = _MAD_PCM_SWMIXER_GROUP_1,
};

static struct MStar_MAD_Ops MStar_PCM_Mixer6_group0_Ops = {
	.open = _mhal_alsa_pcm_mixer6_group0_init,
	.close = _mhal_alsa_pcm_mixer6_group0_exit,
	.start = _mhal_alsa_pcm_mixer6_group0_start,
	.stop = _mhal_alsa_pcm_mixer6_group0_stop,
	.resume = _mhal_alsa_pcm_mixer6_group0_resume,
	.suspend = _mhal_alsa_pcm_mixer6_group0_suspend,
	.read = NULL,
	.write = _mhal_alsa_pcm_mixer6_group0_write,
	.get = _mhal_alsa_pcm_mixer6_group0_get,
	.set = _mhal_alsa_pcm_mixer6_group0_set,
};

/* MStar Audio DSP - PCM Mixer7 Group0 */
static struct MStar_PCM_SWMixer_Client_Struct g_pcm_mixer7_group0 = {
	.str_mode_info[0] = {
		.status = E_STOP,
		.physical_addr = 0,
		.bus_addr = 0,
		.virtual_addr = 0,
	},
	.str_mode_info[1] = {
		.status = E_STOP,
		.physical_addr = 0,
		.bus_addr = 0,
		.virtual_addr = 0,
	},
	.pcm_info = NULL,
	.pcm_buffer_info = NULL,
	.client_id = 6,
	.initialized_status = MAD_FALSE,
	.period_size = _MAD_PCM_SWMIXER_CLIENT_PERIOD_SIZE,
	.written_size = 0,
	.group_id = _MAD_PCM_SWMIXER_GROUP_1,
};

static struct MStar_MAD_Ops MStar_PCM_Mixer7_group0_Ops = {
	.open = _mhal_alsa_pcm_mixer7_group0_init,
	.close = _mhal_alsa_pcm_mixer7_group0_exit,
	.start = _mhal_alsa_pcm_mixer7_group0_start,
	.stop = _mhal_alsa_pcm_mixer7_group0_stop,
	.resume = _mhal_alsa_pcm_mixer7_group0_resume,
	.suspend = _mhal_alsa_pcm_mixer7_group0_suspend,
	.read = NULL,
	.write = _mhal_alsa_pcm_mixer7_group0_write,
	.get = _mhal_alsa_pcm_mixer7_group0_get,
	.set = _mhal_alsa_pcm_mixer7_group0_set,
};

/* MStar Audio DSP - PCM Mixer8 Group0 */
static struct MStar_PCM_SWMixer_Client_Struct g_pcm_mixer8_group0 = {
	.str_mode_info[0] = {
		.status = E_STOP,
		.physical_addr = 0,
		.bus_addr = 0,
		.virtual_addr = 0,
	},
	.str_mode_info[1] = {
		.status = E_STOP,
		.physical_addr = 0,
		.bus_addr = 0,
		.virtual_addr = 0,
	},
	.pcm_info = NULL,
	.pcm_buffer_info = NULL,
	.client_id = 7,
	.initialized_status = MAD_FALSE,
	.period_size = _MAD_PCM_SWMIXER_CLIENT_PERIOD_SIZE,
	.written_size = 0,
	.group_id = _MAD_PCM_SWMIXER_GROUP_1,
};

static struct MStar_MAD_Ops MStar_PCM_Mixer8_group0_Ops = {
	.open = _mhal_alsa_pcm_mixer8_group0_init,
	.close = _mhal_alsa_pcm_mixer8_group0_exit,
	.start = _mhal_alsa_pcm_mixer8_group0_start,
	.stop = _mhal_alsa_pcm_mixer8_group0_stop,
	.resume = _mhal_alsa_pcm_mixer8_group0_resume,
	.suspend = _mhal_alsa_pcm_mixer8_group0_suspend,
	.read = NULL,
	.write = _mhal_alsa_pcm_mixer8_group0_write,
	.get = _mhal_alsa_pcm_mixer8_group0_get,
	.set = _mhal_alsa_pcm_mixer8_group0_set,
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

/* MStar Audio DSP - PCM Capture1 */
static struct MStar_PCM_Capture_Struct g_pcm_capture1 = {
	.buffer = {
		.addr = NULL,
		.size = _MAD_PCM_CAPTURE_BUF_SIZE,
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
		.size = _MAD_PCM_CAPTURE_BUF_SIZE,
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

static ptrdiff_t g_pcm_info_base_va = 0;
static ptrdiff_t g_pcm_buffer_base_va = 0;
static ptrdiff_t g_pcm_dmaRdr2_base_va = 0;
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

#if 0 /* It's not in used for the moment, might be TODO */
static void _mhal_alsa_write_mask_reg(unsigned int u32RegAddr, unsigned short u16Mask, unsigned short u16Val)
{
	unsigned short u16RegVal;

	u16RegVal = _mhal_alsa_read_reg(u32RegAddr);
	u16RegVal = ((u16RegVal & (~(u16Mask))) | (u16Val & u16Mask));
	_mhal_alsa_write_reg(u32RegAddr, u16RegVal);
}

static unsigned char _mhal_alsa_read2_byte(unsigned int u32RegAddr)
{
	return (_MAD_AU_AbsReadByte(u32RegAddr+_MAD_MAILBOX_OFFSET));
}
#endif
static unsigned short _mhal_alsa_read2_reg(unsigned int u32RegAddr)
{
	return (_MAD_AU_AbsRead2Byte(u32RegAddr+_MAD_MAILBOX_OFFSET2));
}
#if 0
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

/* Initiate PCM Mixer Client */
static int _mhal_alsa_pcm_mixer_client_init(char *target_name, struct MStar_PCM_SWMixer_Client_Struct *pcm_playback)
{
	struct MStar_PCM_Info_Struct *pcm_info = NULL;
	struct MStar_PCM_Buffer_Info_Struct *pcm_buffer_info = NULL;
	unsigned int device_id = 0;
	unsigned int client_id = 0;
	unsigned int target_channel = 2;
	unsigned int target_alignment_size = 0;
	unsigned int target_buffer_size = 0;
	unsigned int pcm_info_base_pa = 0;
	unsigned int pcm_info_base_ba = 0;
	unsigned int pcm_info_base_va = 0;
	unsigned int pcm_buffer_base_pa = 0;
	unsigned int pcm_buffer_base_ba = 0;
	unsigned int pcm_buffer_base_va = 0;
	//MAD_PRINT(KERN_INFO "Initiate MStar PCM Mixer Client '%s'\n", target_name);

	client_id = pcm_playback->client_id;

	if ((pcm_playback->initialized_status != MAD_TRUE) || (pcm_playback->str_mode_info[0].status != E_RESUME)) {
		pcm_info_base_pa = (((ptrdiff_t)_mhal_alsa_read_reg(0x2A90) * _MAD_ADDR_CONVERTOR) + _MAD_PCM_SWMIXER_CLIENT_INFO_BASE_OFFSET);
		pcm_info_base_pa += (((_mhal_alsa_read_reg(0x2AC0) & 0x0F) << 16) * _MAD_ADDR_CONVERTOR);
		pcm_info_base_ba = pcm_info_base_pa + _MAD_PHYSICAL_ADDR_TO_BUS_ADDR;

		pcm_buffer_base_pa = (((ptrdiff_t)_mhal_alsa_read_reg(0x2A90) * _MAD_ADDR_CONVERTOR) + _MAD_PCM_SWMIXER_CLIENT_BUFFER_BASE_OFFSET);
		pcm_buffer_base_pa += (((_mhal_alsa_read_reg(0x2AC0) & 0x0F) << 16) * _MAD_ADDR_CONVERTOR);
		pcm_buffer_base_ba = pcm_buffer_base_pa + _MAD_PHYSICAL_ADDR_TO_BUS_ADDR;

		if ((pcm_info_base_ba % 0x1000) || (pcm_buffer_base_ba % 0x1000)) {
			MAD_PRINT(KERN_ERR "Error! Invalid MStar PCM playback bus address, it should be aligned by 4 KB!\n");
			return -EFAULT;
		}

		/* convert Bus Address to Virtual Address */
		if (g_pcm_info_base_va == 0) {
			g_pcm_info_base_va = (ptrdiff_t)ioremap_nocache(pcm_info_base_ba, _MAD_PCM_SWMIXER_CLIENT_INFO_TOTAL_SIZE);
			if (g_pcm_info_base_va == 0) {
				MAD_PRINT(KERN_ERR "Error! fail to convert PCM Mixer Info Bus Address to Virtual Address\n");

				return -ENOMEM;
			}
		}
		pcm_info_base_va = g_pcm_info_base_va;
		pcm_info = (struct MStar_PCM_Info_Struct *)pcm_info_base_va;
		pcm_buffer_info = (struct MStar_PCM_Buffer_Info_Struct *)(pcm_info_base_va + (sizeof(struct MStar_PCM_Info_Struct) * _MAD_PCM_MAXIMUM));

		if (g_pcm_buffer_base_va == 0) {
			g_pcm_buffer_base_va = (ptrdiff_t)ioremap_nocache(pcm_buffer_base_ba, _MAD_PCM_SWMIXER_CLIENT_BUFFER_TOTAL_SIZE);
			if (g_pcm_buffer_base_va == 0) {
				MAD_PRINT(KERN_ERR "Error! fail to convert PCM Mixer Buffer Bus Address to Virtual Address\n");
				return -ENOMEM;
			}
		}
		pcm_buffer_base_va = g_pcm_buffer_base_va;

		for (device_id = 0; device_id < _MAD_PCM_MAXIMUM; device_id++) {
			if (strcmp(pcm_info->name, target_name) == 0) {
				if (pcm_info->connect_flag == MAD_TRUE) {
					MAD_PRINT(KERN_ERR "Error! PCM Mixer Client %u '%s' is busy!\n", client_id, target_name);
					return -EBUSY;
				}

				pcm_info->connect_flag = MAD_TRUE;
				break;
			}

			pcm_info++;
			pcm_buffer_info++;
			pcm_info_base_pa += sizeof(struct MStar_PCM_Info_Struct);
			pcm_info_base_ba += sizeof(struct MStar_PCM_Info_Struct);
			pcm_info_base_va += sizeof(struct MStar_PCM_Info_Struct);
		}

		if (device_id == _MAD_PCM_MAXIMUM) {
			MAD_PRINT(KERN_ERR "Error! fail to find PCM Mixer Client '%s'\n", target_name);
			return -ENXIO;
		}

		//MAD_PRINT(KERN_INFO "Get PCM Mixer Client %u '%s'\n", client_id, target_name);

		pcm_playback->str_mode_info[0].physical_addr = pcm_info_base_pa;
		pcm_playback->str_mode_info[0].bus_addr = pcm_info_base_ba;
		pcm_playback->str_mode_info[0].virtual_addr = pcm_info_base_va;

		pcm_buffer_base_pa += (client_id * _MAD_PCM_SWMIXER_CLIENT_BUFFER_SIZE);
		pcm_buffer_base_ba += (client_id * _MAD_PCM_SWMIXER_CLIENT_BUFFER_SIZE);
		pcm_buffer_base_va += (client_id * _MAD_PCM_SWMIXER_CLIENT_BUFFER_SIZE);

		pcm_playback->str_mode_info[1].physical_addr = pcm_buffer_base_pa;
		pcm_playback->str_mode_info[1].bus_addr = pcm_buffer_base_ba;
		pcm_playback->str_mode_info[1].virtual_addr = pcm_buffer_base_va;

		pcm_playback->initialized_status = MAD_TRUE;
	}
	else {
		pcm_info_base_pa = pcm_playback->str_mode_info[0].physical_addr;
		pcm_info_base_ba = pcm_playback->str_mode_info[0].bus_addr;
		pcm_info_base_va = pcm_playback->str_mode_info[0].virtual_addr;

		pcm_buffer_base_pa = pcm_playback->str_mode_info[1].physical_addr;
		pcm_buffer_base_ba = pcm_playback->str_mode_info[1].bus_addr;
		pcm_buffer_base_va = pcm_playback->str_mode_info[1].virtual_addr;
	}

	pcm_info->struct_version = _MAD_SW_MIXER_CLIENT_VERSION;
	pcm_info->struct_size = sizeof(struct MStar_PCM_Info_Struct);
	pcm_info->connect_flag = MAD_TRUE;
	pcm_info->start_flag = MAD_FALSE;
	pcm_info->non_blocking_flag = MAD_FALSE;
	pcm_info->multi_channel_flag = MAD_FALSE;
	pcm_info->mixing_flag = MAD_TRUE;
	pcm_info->mixing_group = pcm_playback->group_id;
	pcm_info->buffer_duration = 100;
	pcm_info->channel = pcm_playback->channel_mode;
	pcm_info->sample_rate = pcm_playback->sample_rate;
	pcm_info->bit_width = 16;
	pcm_info->big_endian = MAD_FALSE;
	pcm_info->timestamp = 0;
	pcm_info->weighting = 100;
	pcm_info->volume = 96;
	pcm_info->buffer_level = 0;
	pcm_info->capture_flag = MAD_FALSE;

	target_channel = 2;
	target_alignment_size = target_channel * (pcm_info->bit_width / 8) * _MAD_BYTES_IN_LINE;
	target_buffer_size = (pcm_info->buffer_duration * pcm_info->sample_rate * target_channel * (pcm_info->bit_width / 8)) / 1000;
	_MAD_DO_ALIGNMENT(target_buffer_size, target_alignment_size);
	if (target_buffer_size > _MAD_PCM_SWMIXER_CLIENT_BUFFER_SIZE) {
		MAD_PRINT(KERN_INFO "Warning! Too large buffer size(%u) is required! Please check the settings: duration %u, sample rate %u\n", target_buffer_size, pcm_info->buffer_duration, pcm_info->sample_rate);
		target_buffer_size = _MAD_PCM_SWMIXER_CLIENT_BUFFER_SIZE;
		pcm_info->buffer_duration = ((target_buffer_size * 1000) / (pcm_info->sample_rate * target_channel * (pcm_info->bit_width / 8)));
		MAD_PRINT(KERN_INFO "Warning! Force duration to %u, which is the maximun value\n", pcm_info->buffer_duration);
	}

	pcm_buffer_info->phy_addr = (unsigned char *)pcm_buffer_base_pa;
	pcm_buffer_info->r_offset = 0;
	pcm_buffer_info->w_offset = 0;
	pcm_buffer_info->size = target_buffer_size;
	pcm_buffer_info->high_threshold = target_buffer_size - (target_buffer_size >> 4);
	_MAD_DO_ALIGNMENT(pcm_buffer_info->high_threshold, target_alignment_size);

	//MAD_PRINT(KERN_INFO "PCM Mixer Client %u '%s' buffer start address = 0x%08X\n", client_id, target_name, pcm_buffer_info->phy_addr);
	//MAD_PRINT(KERN_INFO "PCM Mixer Client %u '%s' buffer end address = 0x%08X\n", client_id, target_name, (pcm_buffer_info->phy_addr + pcm_buffer_info->size));

	pcm_playback->addr = (unsigned char *)pcm_buffer_base_va;
	memset((void *)pcm_playback->addr, 0x00, _MAD_PCM_SWMIXER_CLIENT_BUFFER_SIZE);
	Chip_Flush_Memory();

	pcm_playback->written_size = 0;

	pcm_playback->pcm_info = pcm_info;
	pcm_playback->pcm_buffer_info = pcm_buffer_info;

	return 0;
}

/* Exit PCM Mixer1 */
static int _mhal_alsa_pcm_mixer_client_exit(struct MStar_PCM_SWMixer_Client_Struct *pcm_playback)
{
	int inused_bytes = 0;

	//MAD_PRINT(KERN_INFO "Exit MStar PCM Mixer Client '%s'\n", pcm_playback->pcm_info->name);

	if (pcm_playback->pcm_info != NULL) {
		pcm_playback->pcm_info->start_flag = MAD_FALSE;
		pcm_playback->pcm_info->connect_flag = MAD_FALSE;
		pcm_playback->pcm_info = NULL;
	}

	if (pcm_playback->addr != NULL) {
		memset((void *)pcm_playback->addr, 0x00, _MAD_PCM_SWMIXER_CLIENT_BUFFER_SIZE);
		pcm_playback->addr = NULL;
	}

	if (pcm_playback->pcm_buffer_info != NULL) {
		memset((void *)pcm_playback->pcm_buffer_info, 0x00, sizeof(struct MStar_PCM_Buffer_Info_Struct));
		pcm_playback->pcm_buffer_info = NULL;
	}

	pcm_playback->str_mode_info[0].status = E_STOP;
	pcm_playback->str_mode_info[1].status = E_STOP;
	pcm_playback->initialized_status = MAD_FALSE;

	return 0;
}

/* Start PCM Mixer1 */
static int _mhal_alsa_pcm_mixer_client_start(struct MStar_PCM_SWMixer_Client_Struct *pcm_playback)
{
	//MAD_PRINT(KERN_INFO "Start MStar PCM Mixer Client '%s'\n", pcm_playback->pcm_info->name);

	pcm_playback->str_mode_info[0].status = E_START;
	pcm_playback->str_mode_info[1].status = E_START;

	if (pcm_playback->pcm_info != NULL) {
		pcm_playback->pcm_info->start_flag = MAD_TRUE;
	}

	return 0;
}

/* Stop PCM Mixer1 */
static int _mhal_alsa_pcm_mixer_client_stop(struct MStar_PCM_SWMixer_Client_Struct *pcm_playback)
{
	//MAD_PRINT(KERN_INFO "Stop MStar PCM Mixer Client '%s'\n", pcm_playback->pcm_info->name);

	if (pcm_playback->pcm_info != NULL) {
		pcm_playback->pcm_info->start_flag = MAD_FALSE;
	}

	return 0;
}

/* Resume PCM Mixer1 */
static int _mhal_alsa_pcm_mixer_client_resume(struct MStar_PCM_SWMixer_Client_Struct *pcm_playback)
{
	//MAD_PRINT(KERN_INFO "Resume MStar PCM Mixer Client '%s'\n", pcm_playback->pcm_info->name);

	pcm_playback->str_mode_info[0].status = E_RESUME;
	pcm_playback->str_mode_info[1].status = E_RESUME;

	return 0;
}

/* Suspend PCM Mixer1 */
static int _mhal_alsa_pcm_mixer_client_suspend(struct MStar_PCM_SWMixer_Client_Struct *pcm_playback)
{
	//MAD_PRINT(KERN_INFO "Suspend MStar PCM Mixer Client '%s'\n", pcm_playback->pcm_info->name);

	pcm_playback->str_mode_info[0].status = E_SUSPEND;
	pcm_playback->str_mode_info[1].status = E_SUSPEND;

	return 0;
}

/* Write PCM to PCM Mixer1 */
static unsigned int _mhal_alsa_pcm_mixer_client_write(struct MStar_PCM_SWMixer_Client_Struct *pcm_playback, void *buffer, unsigned int bytes)
{
	unsigned char *buffer_tmp = (unsigned char *)buffer;
	unsigned int copy_lr_sample = 0;
	unsigned int copy_size = 0;
	unsigned int w_offset_tmp = 0;
	int inused_bytes = 0;
	int loop = 0;

	copy_lr_sample = bytes / 2; /* L + R samples */
	copy_size = (pcm_playback->channel_mode == E_MONO) ? (bytes * 2) : bytes;
	inused_bytes = _mhal_alsa_pcm_mixer_client_get_inused_bytes(pcm_playback);

	if (inused_bytes == 0) {
		//MAD_PRINT(KERN_INFO "***** PCM Mixer Client '%s' Buffer empty !! *****\n", pcm_playback->pcm_info->name);
		pcm_playback->written_size = 0;
	}
	else if ((inused_bytes + copy_size) > pcm_playback->pcm_buffer_info->high_threshold) {
		//MAD_PRINT(KERN_INFO "***** PCM Mixer Client '%' Buffer full !! *****\n", pcm_playback->pcm_info->name);
		return 0;
	}

	w_offset_tmp = pcm_playback->pcm_buffer_info->w_offset;

	if (pcm_playback->channel_mode == E_MONO) {
		for (loop = 0; loop < copy_lr_sample; loop++) {
			unsigned char sample_lo = *(buffer_tmp++);
			unsigned char sample_hi = *(buffer_tmp++);

			*(pcm_playback->addr + w_offset_tmp + 0) = sample_lo;
			*(pcm_playback->addr + w_offset_tmp + 1) = sample_hi;
			*(pcm_playback->addr + w_offset_tmp + 2) = sample_lo;
			*(pcm_playback->addr + w_offset_tmp + 3) = sample_hi;
			w_offset_tmp += 4;

			if (w_offset_tmp >= pcm_playback->pcm_buffer_info->size) {
				w_offset_tmp = 0;
			}
		}
	}
	else {
		for (loop = 0; loop < copy_lr_sample; loop++) {
			*(pcm_playback->addr + w_offset_tmp + 0) = *(buffer_tmp++);;
			*(pcm_playback->addr + w_offset_tmp + 1) = *(buffer_tmp++);
			w_offset_tmp += 2;

			if (w_offset_tmp >= pcm_playback->pcm_buffer_info->size) {
				w_offset_tmp = 0;
			}
		}
	}

	/* flush MIU */
	Chip_Flush_Memory();

	pcm_playback->written_size += copy_size;
	pcm_playback->pcm_buffer_info->w_offset = w_offset_tmp;

	return bytes;
}

/* Get information from PCM Mixer1 */
static int _mhal_alsa_pcm_mixer_client_get(struct MStar_PCM_SWMixer_Client_Struct *pcm_playback, int cmd, unsigned int *param)
{
	int err = 0;
	//MAD_PRINT(KERN_INFO "Get parameter from MStar PCM Mixer Client '%s'\n", pcm_playback->pcm_info->name);

	switch(cmd) {
		case E_PCM_PLAYBACK_GET_BUFFER_SIZE:
		{
			*param = pcm_playback->pcm_buffer_info->size;
			break;
		}

		case E_PCM_PLAYBACK_GET_PERIOD_SIZE:
		{
			*param = pcm_playback->period_size;
			break;
		}

		case E_PCM_PLAYBACK_GET_SAMPLE_RATE:
		{
			*param = pcm_playback->sample_rate;
			break;
		}

		case E_PCM_PLAYBACK_GET_CHANNEL_MODE:
		{
			*param = pcm_playback->channel_mode;
			break;
		}

		case E_PCM_PLAYBACK_GET_MAX_CHANNEL:
		{
			*param = _MAD_PCM_SWMIXER_CHANNEL;
			break;
		}

		case E_PCM_PLAYBACK_GET_CONSTRAINTS_COUNT:
		{
			*param = sizeof(mad_rates) / sizeof(mad_rates[0]);
			break;
		}

		case E_PCM_PLAYBACK_GET_CONSTRAINTS_LIST:
		{
			*param = (unsigned int)&mad_rates;
			break;
		}

		case E_PCM_PLAYBACK_GET_CONSTRAINTS_MASK:
		{
			*param = 0;
			break;
		}

		case E_PCM_PLAYBACK_GET_BUFFER_INUSED_BYTES:
		{
			*param = _mhal_alsa_pcm_mixer_client_get_inused_bytes(pcm_playback);
			break;
		}

		case E_PCM_PLAYBACK_GET_BUFFER_AVAIL_BYTES:
		{
			*param = _mhal_alsa_pcm_mixer_client_get_avail_bytes(pcm_playback);
			break;
		}

		case E_PCM_PLAYBACK_GET_BUFFER_CONSUMED_BYTES:
		{
			int inused_bytes = 0;
			int consumed_bytes = 0;

			inused_bytes = _mhal_alsa_pcm_mixer_client_get_inused_bytes(pcm_playback);
			consumed_bytes = pcm_playback->written_size - inused_bytes;

			if (pcm_playback->channel_mode == E_MONO) {
				consumed_bytes = (consumed_bytes >> 1);
			}

			*param = consumed_bytes;
			pcm_playback->written_size = inused_bytes;
			break;
		}

		case E_PCM_PLAYBACK_GET_STR_STATUS:
		{
			*param = pcm_playback->str_mode_info[0].status;
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

/* Set information to PCM Mixer Client */
static int _mhal_alsa_pcm_mixer_client_set(struct MStar_PCM_SWMixer_Client_Struct *pcm_playback, int cmd, unsigned int *param)
{
	int err = 0;
	//MAD_PRINT(KERN_INFO "Set parameter to MStar PCM Mixer Client '%s'\n", pcm_playback->pcm_info->name);

	switch(cmd) {
		case E_PCM_PLAYBACK_SET_SAMPLE_RATE:
		{
			_mhal_alsa_pcm_mixer_client_set_sample_rate(pcm_playback, *param);
			break;
		}

		case E_PCM_PLAYBACK_SET_CHANNEL_MODE:
		{
			_mhal_alsa_pcm_mixer_client_set_channel_mode(pcm_playback, *param);
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

/* Get PCM Mixer Client buffer level */
static int _mhal_alsa_pcm_mixer_client_get_inused_bytes(struct MStar_PCM_SWMixer_Client_Struct *pcm_playback)
{
	int inused_bytes = 0;

	inused_bytes = pcm_playback->pcm_buffer_info->w_offset - pcm_playback->pcm_buffer_info->r_offset;
	if (inused_bytes < 0) {
		inused_bytes += pcm_playback->pcm_buffer_info->size;
	}

	return inused_bytes;
}

/* Get PCM Mixer Client avail bytes */
static int _mhal_alsa_pcm_mixer_client_get_avail_bytes(struct MStar_PCM_SWMixer_Client_Struct *pcm_playback)
{
	int inused_bytes = 0;
	int avail_bytes = 0;

	inused_bytes = _mhal_alsa_pcm_mixer_client_get_inused_bytes(pcm_playback);
	avail_bytes = pcm_playback->pcm_buffer_info->high_threshold - inused_bytes;
	if (avail_bytes < 0) {
		MAD_PRINT(KERN_ERR "Error! Incorrect inused bytes %d!\n", inused_bytes);
		avail_bytes = 0;
	}

	if (pcm_playback->channel_mode == E_MONO) {
		avail_bytes = (avail_bytes >> 1);
	}

	return avail_bytes;
}

/* Set smaple rate to PCM Mixer Client */
static int _mhal_alsa_pcm_mixer_client_set_sample_rate(struct MStar_PCM_SWMixer_Client_Struct *pcm_playback, unsigned int sample_rate)
{
	//MAD_PRINT(KERN_INFO "Target sample rate is %u\n", sample_rate);

	pcm_playback->sample_rate = sample_rate;

	return 0;
}

/* Set channel mode to PCM Mixer Client */
static int _mhal_alsa_pcm_mixer_client_set_channel_mode(struct MStar_PCM_SWMixer_Client_Struct *pcm_playback, unsigned int channel_mode)
{
	//MAD_PRINT(KERN_INFO "Target channel mode is %u\n", channel_mode);

	pcm_playback->channel_mode = channel_mode;

	return 0;
}

/* Initiate PCM Mixer1 Group0 */
static int _mhal_alsa_pcm_mixer1_group0_init(void)
{
	return _mhal_alsa_pcm_mixer_client_init(_MAD_SW_MIXER1, &g_pcm_mixer1_group0);
}

/* Exit PCM Mixer1 Group0 */
static int _mhal_alsa_pcm_mixer1_group0_exit(void)
{
	return _mhal_alsa_pcm_mixer_client_exit(&g_pcm_mixer1_group0);
}

/* Start PCM Mixer1 Group0 */
static int _mhal_alsa_pcm_mixer1_group0_start(void)
{
	return _mhal_alsa_pcm_mixer_client_start(&g_pcm_mixer1_group0);
}

/* Stop PCM Mixer1 Group0 */
static int _mhal_alsa_pcm_mixer1_group0_stop(void)
{
	return _mhal_alsa_pcm_mixer_client_stop(&g_pcm_mixer1_group0);
}

/* Resume PCM Mixer1 Group0 */
static int _mhal_alsa_pcm_mixer1_group0_resume(void)
{
	return _mhal_alsa_pcm_mixer_client_resume(&g_pcm_mixer1_group0);
}

/* Suspend PCM Mixer1 Group0 */
static int _mhal_alsa_pcm_mixer1_group0_suspend(void)
{
	return _mhal_alsa_pcm_mixer_client_suspend(&g_pcm_mixer1_group0);
}

/* Write PCM to PCM Mixer1 Group0 */
static unsigned int _mhal_alsa_pcm_mixer1_group0_write(void *buffer, unsigned int bytes)
{
	return _mhal_alsa_pcm_mixer_client_write(&g_pcm_mixer1_group0, buffer, bytes);
}

/* Get information from PCM Mixer1 Group0 */
static int _mhal_alsa_pcm_mixer1_group0_get(int cmd, unsigned int *param)
{
	return _mhal_alsa_pcm_mixer_client_get(&g_pcm_mixer1_group0, cmd, param);
}

/* Set information to PCM Mixer1 Group0 */
static int _mhal_alsa_pcm_mixer1_group0_set(int cmd, unsigned int *param)
{
	return _mhal_alsa_pcm_mixer_client_set(&g_pcm_mixer1_group0, cmd, param);
}

/* Initiate PCM Mixer2 Group0 */
static int _mhal_alsa_pcm_mixer2_group0_init(void)
{
	return _mhal_alsa_pcm_mixer_client_init(_MAD_SW_MIXER2, &g_pcm_mixer2_group0);
}

/* Exit PCM Mixer2 Group0 */
static int _mhal_alsa_pcm_mixer2_group0_exit(void)
{
	return _mhal_alsa_pcm_mixer_client_exit(&g_pcm_mixer2_group0);
}

/* Start PCM Mixer2 Group0 */
static int _mhal_alsa_pcm_mixer2_group0_start(void)
{
	return _mhal_alsa_pcm_mixer_client_start(&g_pcm_mixer2_group0);
}

/* Stop PCM Mixer2 Group0 */
static int _mhal_alsa_pcm_mixer2_group0_stop(void)
{
	return _mhal_alsa_pcm_mixer_client_stop(&g_pcm_mixer2_group0);
}

/* Resume PCM Mixer2 Group0 */
static int _mhal_alsa_pcm_mixer2_group0_resume(void)
{
	return _mhal_alsa_pcm_mixer_client_resume(&g_pcm_mixer2_group0);
}

/* Suspend PCM Mixer2 Group0 */
static int _mhal_alsa_pcm_mixer2_group0_suspend(void)
{
	return _mhal_alsa_pcm_mixer_client_suspend(&g_pcm_mixer2_group0);
}

/* Write PCM to PCM Mixer2 Group0 */
static unsigned int _mhal_alsa_pcm_mixer2_group0_write(void *buffer, unsigned int bytes)
{
	return _mhal_alsa_pcm_mixer_client_write(&g_pcm_mixer2_group0, buffer, bytes);
}

/* Get information from PCM Mixer2 Group0 */
static int _mhal_alsa_pcm_mixer2_group0_get(int cmd, unsigned int *param)
{
	return _mhal_alsa_pcm_mixer_client_get(&g_pcm_mixer2_group0, cmd, param);
}

/* Set information to PCM Mixer2 Group0 */
static int _mhal_alsa_pcm_mixer2_group0_set(int cmd, unsigned int *param)
{
	return _mhal_alsa_pcm_mixer_client_set(&g_pcm_mixer2_group0, cmd, param);
}

/* Initiate PCM Mixer3 Group0 */
static int _mhal_alsa_pcm_mixer3_group0_init(void)
{
	return _mhal_alsa_pcm_mixer_client_init(_MAD_SW_MIXER3, &g_pcm_mixer3_group0);
}

/* Exit PCM Mixer3 Group0 */
static int _mhal_alsa_pcm_mixer3_group0_exit(void)
{
	return _mhal_alsa_pcm_mixer_client_exit(&g_pcm_mixer3_group0);
}

/* Start PCM Mixer3 Group0 */
static int _mhal_alsa_pcm_mixer3_group0_start(void)
{
	return _mhal_alsa_pcm_mixer_client_start(&g_pcm_mixer3_group0);
}

/* Stop PCM Mixer3 Group0 */
static int _mhal_alsa_pcm_mixer3_group0_stop(void)
{
	return _mhal_alsa_pcm_mixer_client_stop(&g_pcm_mixer3_group0);
}

/* Resume PCM Mixer3 Group0 */
static int _mhal_alsa_pcm_mixer3_group0_resume(void)
{
	return _mhal_alsa_pcm_mixer_client_resume(&g_pcm_mixer3_group0);
}

/* Suspend PCM Mixer3 Group0 */
static int _mhal_alsa_pcm_mixer3_group0_suspend(void)
{
	return _mhal_alsa_pcm_mixer_client_suspend(&g_pcm_mixer3_group0);
}

/* Write PCM to PCM Mixer3 Group0 */
static unsigned int _mhal_alsa_pcm_mixer3_group0_write(void *buffer, unsigned int bytes)
{
	return _mhal_alsa_pcm_mixer_client_write(&g_pcm_mixer3_group0, buffer, bytes);
}

/* Get information from PCM Mixer3 Group0 */
static int _mhal_alsa_pcm_mixer3_group0_get(int cmd, unsigned int *param)
{
	return _mhal_alsa_pcm_mixer_client_get(&g_pcm_mixer3_group0, cmd, param);
}

/* Set information to PCM Mixer3 Group0 */
static int _mhal_alsa_pcm_mixer3_group0_set(int cmd, unsigned int *param)
{
	return _mhal_alsa_pcm_mixer_client_set(&g_pcm_mixer3_group0, cmd, param);
}

/* Initiate PCM Mixer4 Group0 */
static int _mhal_alsa_pcm_mixer4_group0_init(void)
{
	return _mhal_alsa_pcm_mixer_client_init(_MAD_SW_MIXER4, &g_pcm_mixer4_group0);
}

/* Exit PCM Mixer4 Group0 */
static int _mhal_alsa_pcm_mixer4_group0_exit(void)
{
	return _mhal_alsa_pcm_mixer_client_exit(&g_pcm_mixer4_group0);
}

/* Start PCM Mixer4 Group0 */
static int _mhal_alsa_pcm_mixer4_group0_start(void)
{
	return _mhal_alsa_pcm_mixer_client_start(&g_pcm_mixer4_group0);
}

/* Stop PCM Mixer4 Group0 */
static int _mhal_alsa_pcm_mixer4_group0_stop(void)
{
	return _mhal_alsa_pcm_mixer_client_stop(&g_pcm_mixer4_group0);
}

/* Resume PCM Mixer4 Group0 */
static int _mhal_alsa_pcm_mixer4_group0_resume(void)
{
	return _mhal_alsa_pcm_mixer_client_resume(&g_pcm_mixer4_group0);
}

/* Suspend PCM Mixer4 Group0 */
static int _mhal_alsa_pcm_mixer4_group0_suspend(void)
{
	return _mhal_alsa_pcm_mixer_client_suspend(&g_pcm_mixer4_group0);
}

/* Write PCM to PCM Mixer4 Group0 */
static unsigned int _mhal_alsa_pcm_mixer4_group0_write(void *buffer, unsigned int bytes)
{
	return _mhal_alsa_pcm_mixer_client_write(&g_pcm_mixer4_group0, buffer, bytes);
}

/* Get information from PCM Mixer4 Group0 */
static int _mhal_alsa_pcm_mixer4_group0_get(int cmd, unsigned int *param)
{
	return _mhal_alsa_pcm_mixer_client_get(&g_pcm_mixer4_group0, cmd, param);
}

/* Set information to PCM Mixer4 Group0 */
static int _mhal_alsa_pcm_mixer4_group0_set(int cmd, unsigned int *param)
{
	return _mhal_alsa_pcm_mixer_client_set(&g_pcm_mixer4_group0, cmd, param);
}

/* Initiate PCM Mixer5 Group0 */
static int _mhal_alsa_pcm_mixer5_group0_init(void)
{
	return _mhal_alsa_pcm_mixer_client_init(_MAD_SW_MIXER5, &g_pcm_mixer5_group0);
}

/* Exit PCM Mixer5 Group0 */
static int _mhal_alsa_pcm_mixer5_group0_exit(void)
{
	return _mhal_alsa_pcm_mixer_client_exit(&g_pcm_mixer5_group0);
}

/* Start PCM Mixer5 Group0 */
static int _mhal_alsa_pcm_mixer5_group0_start(void)
{
	return _mhal_alsa_pcm_mixer_client_start(&g_pcm_mixer5_group0);
}

/* Stop PCM Mixer5 Group0 */
static int _mhal_alsa_pcm_mixer5_group0_stop(void)
{
	return _mhal_alsa_pcm_mixer_client_stop(&g_pcm_mixer5_group0);
}

/* Resume PCM Mixer5 Group0 */
static int _mhal_alsa_pcm_mixer5_group0_resume(void)
{
	return _mhal_alsa_pcm_mixer_client_resume(&g_pcm_mixer5_group0);
}

/* Suspend PCM Mixer5 Group0 */
static int _mhal_alsa_pcm_mixer5_group0_suspend(void)
{
	return _mhal_alsa_pcm_mixer_client_suspend(&g_pcm_mixer5_group0);
}

/* Write PCM to PCM Mixer5 Group0 */
static unsigned int _mhal_alsa_pcm_mixer5_group0_write(void *buffer, unsigned int bytes)
{
	return _mhal_alsa_pcm_mixer_client_write(&g_pcm_mixer5_group0, buffer, bytes);
}

/* Get information from PCM Mixer5 Group0 */
static int _mhal_alsa_pcm_mixer5_group0_get(int cmd, unsigned int *param)
{
	return _mhal_alsa_pcm_mixer_client_get(&g_pcm_mixer5_group0, cmd, param);
}

/* Set information to PCM Mixer5 Group0 */
static int _mhal_alsa_pcm_mixer5_group0_set(int cmd, unsigned int *param)
{
	return _mhal_alsa_pcm_mixer_client_set(&g_pcm_mixer5_group0, cmd, param);
}

/* Initiate PCM Mixer6 Group0 */
static int _mhal_alsa_pcm_mixer6_group0_init(void)
{
	return _mhal_alsa_pcm_mixer_client_init(_MAD_SW_MIXER6, &g_pcm_mixer6_group0);
}

/* Exit PCM Mixer6 Group0 */
static int _mhal_alsa_pcm_mixer6_group0_exit(void)
{
	return _mhal_alsa_pcm_mixer_client_exit(&g_pcm_mixer6_group0);
}

/* Start PCM Mixer6 Group0 */
static int _mhal_alsa_pcm_mixer6_group0_start(void)
{
	return _mhal_alsa_pcm_mixer_client_start(&g_pcm_mixer6_group0);
}

/* Stop PCM Mixer6 Group0 */
static int _mhal_alsa_pcm_mixer6_group0_stop(void)
{
	return _mhal_alsa_pcm_mixer_client_stop(&g_pcm_mixer6_group0);
}

/* Resume PCM Mixer6 Group0 */
static int _mhal_alsa_pcm_mixer6_group0_resume(void)
{
	return _mhal_alsa_pcm_mixer_client_resume(&g_pcm_mixer6_group0);
}

/* Suspend PCM Mixer6 Group0 */
static int _mhal_alsa_pcm_mixer6_group0_suspend(void)
{
	return _mhal_alsa_pcm_mixer_client_suspend(&g_pcm_mixer6_group0);
}

/* Write PCM to PCM Mixer6 Group0 */
static unsigned int _mhal_alsa_pcm_mixer6_group0_write(void *buffer, unsigned int bytes)
{
	return _mhal_alsa_pcm_mixer_client_write(&g_pcm_mixer6_group0, buffer, bytes);
}

/* Get information from PCM Mixer6 Group0 */
static int _mhal_alsa_pcm_mixer6_group0_get(int cmd, unsigned int *param)
{
	return _mhal_alsa_pcm_mixer_client_get(&g_pcm_mixer6_group0, cmd, param);
}

/* Set information to PCM Mixer6 Group0 */
static int _mhal_alsa_pcm_mixer6_group0_set(int cmd, unsigned int *param)
{
	return _mhal_alsa_pcm_mixer_client_set(&g_pcm_mixer6_group0, cmd, param);
}

/* Initiate PCM Mixer7 Group0 */
static int _mhal_alsa_pcm_mixer7_group0_init(void)
{
	return _mhal_alsa_pcm_mixer_client_init(_MAD_SW_MIXER7, &g_pcm_mixer7_group0);
}

/* Exit PCM Mixer7 Group0 */
static int _mhal_alsa_pcm_mixer7_group0_exit(void)
{
	return _mhal_alsa_pcm_mixer_client_exit(&g_pcm_mixer7_group0);
}

/* Start PCM Mixer7 Group0 */
static int _mhal_alsa_pcm_mixer7_group0_start(void)
{
	return _mhal_alsa_pcm_mixer_client_start(&g_pcm_mixer7_group0);
}

/* Stop PCM Mixer7 Group0 */
static int _mhal_alsa_pcm_mixer7_group0_stop(void)
{
	return _mhal_alsa_pcm_mixer_client_stop(&g_pcm_mixer7_group0);
}

/* Resume PCM Mixer7 Group0 */
static int _mhal_alsa_pcm_mixer7_group0_resume(void)
{
	return _mhal_alsa_pcm_mixer_client_resume(&g_pcm_mixer7_group0);
}

/* Suspend PCM Mixer7 Group0 */
static int _mhal_alsa_pcm_mixer7_group0_suspend(void)
{
	return _mhal_alsa_pcm_mixer_client_suspend(&g_pcm_mixer7_group0);
}

/* Write PCM to PCM Mixer7 Group0 */
static unsigned int _mhal_alsa_pcm_mixer7_group0_write(void *buffer, unsigned int bytes)
{
	return _mhal_alsa_pcm_mixer_client_write(&g_pcm_mixer7_group0, buffer, bytes);
}

/* Get information from PCM Mixer7 Group0 */
static int _mhal_alsa_pcm_mixer7_group0_get(int cmd, unsigned int *param)
{
	return _mhal_alsa_pcm_mixer_client_get(&g_pcm_mixer7_group0, cmd, param);
}

/* Set information to PCM Mixer7 Group0 */
static int _mhal_alsa_pcm_mixer7_group0_set(int cmd, unsigned int *param)
{
	return _mhal_alsa_pcm_mixer_client_set(&g_pcm_mixer7_group0, cmd, param);
}

/* Initiate PCM Mixer8 Group0 */
static int _mhal_alsa_pcm_mixer8_group0_init(void)
{
	return _mhal_alsa_pcm_mixer_client_init(_MAD_SW_MIXER8, &g_pcm_mixer8_group0);
}

/* Exit PCM Mixer8 Group0 */
static int _mhal_alsa_pcm_mixer8_group0_exit(void)
{
	return _mhal_alsa_pcm_mixer_client_exit(&g_pcm_mixer8_group0);
}

/* Start PCM Mixer8 Group0 */
static int _mhal_alsa_pcm_mixer8_group0_start(void)
{
	return _mhal_alsa_pcm_mixer_client_start(&g_pcm_mixer8_group0);
}

/* Stop PCM Mixer8 Group0 */
static int _mhal_alsa_pcm_mixer8_group0_stop(void)
{
	return _mhal_alsa_pcm_mixer_client_stop(&g_pcm_mixer8_group0);
}

/* Resume PCM Mixer8 Group0 */
static int _mhal_alsa_pcm_mixer8_group0_resume(void)
{
	return _mhal_alsa_pcm_mixer_client_resume(&g_pcm_mixer8_group0);
}

/* Suspend PCM Mixer8 Group0 */
static int _mhal_alsa_pcm_mixer8_group0_suspend(void)
{
	return _mhal_alsa_pcm_mixer_client_suspend(&g_pcm_mixer8_group0);
}

/* Write PCM to PCM Mixer8 Group0 */
static unsigned int _mhal_alsa_pcm_mixer8_group0_write(void *buffer, unsigned int bytes)
{
	return _mhal_alsa_pcm_mixer_client_write(&g_pcm_mixer8_group0, buffer, bytes);
}

/* Get information from PCM Mixer8 Group0 */
static int _mhal_alsa_pcm_mixer8_group0_get(int cmd, unsigned int *param)
{
	return _mhal_alsa_pcm_mixer_client_get(&g_pcm_mixer8_group0, cmd, param);
}

/* Set information to PCM Mixer8 Group0 */
static int _mhal_alsa_pcm_mixer8_group0_set(int cmd, unsigned int *param)
{
	return _mhal_alsa_pcm_mixer_client_set(&g_pcm_mixer8_group0, cmd, param);
}

/* Initiate DMA Reader2 */
static int _mhal_alsa_dma_reader2_init(void)
{
	struct MStar_DMA_Reader_Struct *dma_reader = &g_dma_reader2;
	unsigned int audio_pcm_dmaRdr_bufSz = 0;  /* must be multiple of DMA_RDR_PCM_BUF_UNIT*2 (= 0x2000) */
	ptrdiff_t audio_pcm_dmaRdr_base_pa = 0; /* DMA Reader2 Input buffer (DM_Prefetch) */
	ptrdiff_t audio_pcm_dmaRdr_base_ba = 0;
	ptrdiff_t audio_pcm_dmaRdr_base_va = 0;
	//MAD_PRINT(KERN_INFO "Initiate MStar PCM Playback2 engine\n");

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
	//MAD_PRINT(KERN_INFO "Exit MStar PCM Playback2 engine\n");

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
	//MAD_PRINT(KERN_INFO "Target sample rate is %u\n", sample_rate);

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
	//MAD_PRINT(KERN_INFO "Target channel mode is %u\n", channel_mode);

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
	//MAD_PRINT(KERN_INFO "Initiate MStar PCM Capture1 engine\n");

	if ((pcm_capture->initialized_status != MAD_TRUE) || (pcm_capture->str_mode_info.status != E_RESUME)) {
		audio_pcm_capture_base_pa = ((ptrdiff_t)_mhal_alsa_read_reg(0x2A90) * _MAD_ADDR_CONVERTOR) + _MAD_PCM_CAPTURE1_BASE_OFFSET;
		audio_pcm_capture_base_pa += (((_mhal_alsa_read_reg(0x2AC0) & 0x0F) << 16) * _MAD_ADDR_CONVERTOR);
		audio_pcm_capture_base_ba = audio_pcm_capture_base_pa + _MAD_PHYSICAL_ADDR_TO_BUS_ADDR;

		if ((audio_pcm_capture_base_ba % 0x1000)) {
			MAD_PRINT(KERN_ERR "Error! Invalid MStar PCM capture bus address, it should be aligned by 4 KB!\n");
			return -EFAULT;
		}

		/* convert Bus Address to Virtual Address */
		pcm_capture->buffer.size = _MAD_PCM_CAPTURE_BUF_SIZE;
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
	memset((void *)pcm_capture->buffer.addr, 0x00, _MAD_PCM_CAPTURE_BUF_SIZE);
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
	//MAD_PRINT(KERN_INFO "Exit MStar PCM Capture1 engine\n");

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
	unsigned int rest_size_to_buffer_end = (pcm_capture->buffer.addr + _MAD_PCM_CAPTURE_BUF_SIZE) - pcm_capture->buffer.r_ptr;
	unsigned int r_ptr_offset = 0;
	unsigned int read_size = 0;
	//MAD_PRINT(KERN_INFO "Read PCM from PCM Capture1 engine\n");

	read_size = (rest_size_to_buffer_end > bytes) ? bytes : rest_size_to_buffer_end;

	memcpy(buffer, pcm_capture->buffer.r_ptr, read_size);
	Chip_Flush_Memory();

	pcm_capture->buffer.r_ptr += read_size;
	if (pcm_capture->buffer.r_ptr == (pcm_capture->buffer.addr + _MAD_PCM_CAPTURE_BUF_SIZE))
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
			if (*param > _MAD_PCM_CAPTURE_BUF_SIZE) {
				*param = _MAD_PCM_CAPTURE_BUF_SIZE;
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
		new_avail_bytes += _MAD_PCM_CAPTURE_BUF_SIZE;

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
		avail_bytes += _MAD_PCM_CAPTURE_BUF_SIZE;

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
	//MAD_PRINT(KERN_INFO "Initiate MStar PCM Capture2 engine\n");

	if ((pcm_capture->initialized_status != MAD_TRUE) || (pcm_capture->str_mode_info.status != E_RESUME)) {
		audio_pcm_capture_base_pa = ((ptrdiff_t)_mhal_alsa_read_reg(0x2A90) * _MAD_ADDR_CONVERTOR) + _MAD_PCM_CAPTURE2_BASE_OFFSET;
		audio_pcm_capture_base_pa += (((_mhal_alsa_read_reg(0x2AC0) & 0x0F) << 16) * _MAD_ADDR_CONVERTOR);
		audio_pcm_capture_base_ba = audio_pcm_capture_base_pa + _MAD_PHYSICAL_ADDR_TO_BUS_ADDR;

		if ((audio_pcm_capture_base_ba % 0x1000)) {
			MAD_PRINT(KERN_ERR "Error! Invalid MStar PCM capture bus address, it should be aligned by 4 KB!\n");
			return -EFAULT;
		}

		/* convert Bus Address to Virtual Address */
		pcm_capture->buffer.size = _MAD_PCM_CAPTURE_BUF_SIZE;
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
	memset((void *)pcm_capture->buffer.addr, 0x00, _MAD_PCM_CAPTURE_BUF_SIZE);
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
	//MAD_PRINT(KERN_INFO "Exit MStar PCM Capture2 engine\n");

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
	unsigned int rest_size_to_buffer_end = (pcm_capture->buffer.addr + _MAD_PCM_CAPTURE_BUF_SIZE) - pcm_capture->buffer.r_ptr;
	unsigned int r_ptr_offset = 0;
	unsigned int read_size = 0;
	//MAD_PRINT(KERN_INFO "Read PCM from PCM Capture2 engine\n");

	read_size = (rest_size_to_buffer_end > bytes) ? bytes : rest_size_to_buffer_end;

	memcpy(buffer, pcm_capture->buffer.r_ptr, read_size);
	Chip_Flush_Memory();

	pcm_capture->buffer.r_ptr += read_size;
	if (pcm_capture->buffer.r_ptr == (pcm_capture->buffer.addr + _MAD_PCM_CAPTURE_BUF_SIZE))
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
			if (*param > _MAD_PCM_CAPTURE_BUF_SIZE) {
				*param = _MAD_PCM_CAPTURE_BUF_SIZE;
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
		new_avail_bytes += _MAD_PCM_CAPTURE_BUF_SIZE;

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
		avail_bytes += _MAD_PCM_CAPTURE_BUF_SIZE;

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

	memset(&MStar_MAD[0], 0x00, sizeof(struct MStar_MAD_Info));
	memset(&MStar_MAD[1], 0x00, sizeof(struct MStar_MAD_Info));

	str_len = (strlen(_MAD_CHIP_MODEL) > sizeof(MStar_MAD[0].name)) ? sizeof(MStar_MAD[0].name) : strlen(_MAD_CHIP_MODEL);
	memcpy(MStar_MAD[0].name, _MAD_CHIP_MODEL, str_len);
	sprintf(MStar_MAD[0].version, "%d.%d.%d", _MAD_ALSA_HAL_VERSION_MAJOR, _MAD_ALSA_HAL_VERSION_MINOR, _MAD_ALSA_HAL_VERSION_REVISION);

	/* Hook Playback Operators */
	MStar_MAD[0].playback_pcm_ops[0] = &MStar_PCM_Mixer1_group0_Ops;
	MStar_MAD[0].playback_pcm_ops[1] = &MStar_DMA_Reader2_Ops;
	MStar_MAD[0].playback_pcm_ops[2] = &MStar_PCM_Mixer3_group0_Ops;
	MStar_MAD[0].playback_pcm_ops[3] = &MStar_PCM_Mixer4_group0_Ops;
	MStar_MAD[0].playback_pcm_ops[4] = &MStar_PCM_Mixer5_group0_Ops;
	MStar_MAD[0].playback_pcm_ops[5] = &MStar_PCM_Mixer6_group0_Ops;
	MStar_MAD[0].playback_pcm_ops[6] = &MStar_PCM_Mixer7_group0_Ops;
	MStar_MAD[0].playback_pcm_ops[7] = &MStar_PCM_Mixer8_group0_Ops;

	/* Hook Capture Operators */
	MStar_MAD[0].capture_pcm_ops[0] = &MStar_PCM_Capture1_Ops;
	MStar_MAD[0].capture_pcm_ops[1] = &MStar_PCM_Capture2_Ops;

	MStar_MAD[0].number = 0;

	err = _mdrv_alsa_hook_device(&MStar_MAD[0]);
	if (err < 0) {
		MAD_PRINT(KERN_ERR "Error(%d)! fail to hook PCM operators\n", err);
		return err;
	}

	/* Hook Playback Operators */
	MStar_MAD[1].playback_pcm_ops[7] = &MStar_DMA_Reader2_Ops;

	MStar_MAD[1].number = 1;

	err = _mdrv_alsa_hook_device(&MStar_MAD[1]);
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
