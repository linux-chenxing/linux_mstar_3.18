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

#ifndef _MHAL_ALSA_DRIVER_HEADER
#define _MHAL_ALSA_DRIVER_HEADER

/*
 * ============================================================================
 * Include Headers
 * ============================================================================
 */
#include "mdrv_public.h"
#include "mhal_version.h"


/*
 * ============================================================================
 * Constant Definitions
 * ============================================================================
 */
/* Define a Ring Buffer data structure for MStar Audio DSP */
struct MStar_Ring_Buffer_Struct {
	unsigned char *addr;
	unsigned int size;
	unsigned char *w_ptr;
	unsigned char *r_ptr;
};

/* Define a STR (Suspend To Ram) data structure for MStar Audio DSP */
 struct MStar_STR_MODE_Struct {
	unsigned int status;
	ptrdiff_t physical_addr;
	ptrdiff_t bus_addr;
	ptrdiff_t virtual_addr;
};

/* Define a DMA Reader data structure for MStar Audio DSP */
struct MStar_DMA_Reader_Struct {
	struct MStar_Ring_Buffer_Struct buffer;
	struct MStar_STR_MODE_Struct str_mode_info;
	unsigned int initialized_status;
	unsigned int channel_mode;
	unsigned int sample_rate;
	unsigned int period_size;
	unsigned int high_threshold;
	unsigned int remain_size;
	unsigned int written_size;
};

/* Define a DMA Reader data structure for MStar Audio DSP */
struct MStar_PCM_Capture_Struct {
	struct MStar_Ring_Buffer_Struct buffer;
	struct MStar_STR_MODE_Struct str_mode_info;
	unsigned int initialized_status;
	unsigned int channel_mode;
	unsigned int sample_rate;
};

/* Define a PCM Mixer Element Info data structure for MStar Audio DSP */
struct MStar_PCM_Info_Struct {
	unsigned int struct_version;
	unsigned int struct_size;
	unsigned char connect_flag;
	unsigned char start_flag;
	unsigned char name[32];
	unsigned char non_blocking_flag;
	unsigned char multi_channel_flag;
	unsigned char mixing_flag;
	unsigned int mixing_group;
	unsigned int buffer_duration;
	unsigned int channel;
	unsigned int sample_rate;
	unsigned int bit_width;
	unsigned int big_endian;
	unsigned int timestamp;
	unsigned int weighting;
	unsigned int volume;
	unsigned int buffer_level;
	unsigned int capture_flag;
};

/* Define a PCM buffer structure for MStar Audio DSP */
struct MStar_PCM_Buffer_Info_Struct {
	unsigned char *phy_addr;
	unsigned int r_offset;
	unsigned int w_offset;
	unsigned int size;
	unsigned int high_threshold;
	unsigned int remain_size;
	unsigned int next_r_offset;
};

/* Define a PCM Mixer data structure for MStar Audio DSP */
struct MStar_PCM_SWMixer_Client_Struct {
	struct MStar_STR_MODE_Struct str_mode_info[2];
	struct MStar_PCM_Info_Struct *pcm_info;
	struct MStar_PCM_Buffer_Info_Struct *pcm_buffer_info;
	unsigned char *addr;
	unsigned int client_id;
	unsigned int initialized_status;
	unsigned int channel_mode;
	unsigned int sample_rate;
	unsigned int period_size;
	unsigned int written_size;
	unsigned int group_id;
};


/*
 * ============================================================================
 * Forward Declarations
 * ============================================================================
 */
extern void Chip_Flush_Memory(void);

#endif /* _MHAL_ALSA_DRIVER_HEADER */

