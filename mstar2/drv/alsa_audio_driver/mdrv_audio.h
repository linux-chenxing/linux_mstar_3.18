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
 * MStar's Audio Driver for ALSA.
 * Author: Darren Chen <darren.chen@mstarsemi.com>
 *
 */

#ifndef _MDRV_ALSA_DRIVER_HEADER
#define _MDRV_ALSA_DRIVER_HEADER

/*
 * ============================================================================
 * Include Headers
 * ============================================================================
 */
#include "mdrv_public.h"
#include "mdrv_version.h"


/*
 * ============================================================================
 * Constant Definitions
 * ============================================================================
 */
/* Define a Ring Buffer data structure for MStar Audio DSP */
struct MStar_Device_Buffer_Struct {
	unsigned char *addr;
	unsigned int size;
	unsigned int avail_size;
	unsigned int inused_size;
	unsigned int consumed_size;
};

/* Define a Substream data structure for MStar Audio DSP */
struct MStar_Substream_Struct {
	struct snd_pcm_substream *substream;
	unsigned int substream_status;
};

/* Define a Monitor data structure for MStar Audio DSP */
struct MStar_Monitor_Struct {
	unsigned int monitor_status;
	unsigned int expiration_counter;
	snd_pcm_uframes_t last_appl_ptr;
};

/* Define a Runtime data structure for MStar Audio DSP */
struct MStar_Runtime_Struct {
	struct snd_pcm_hw_constraint_list constraints_rates;
	struct MStar_MAD_Ops ops;
	struct MStar_Substream_Struct substreams[MAD_MAX_SUBSTREAMS];
	struct MStar_Device_Buffer_Struct buffer[MAD_MAX_SUBSTREAMS];
	struct MStar_Device_Buffer_Struct convert_buffer[MAD_MAX_SUBSTREAMS];
	struct MStar_Monitor_Struct monitor;
	struct timer_list timer;
	struct mutex mutex_lock;
	spinlock_t spin_lock;
	unsigned int active_substreams;
	unsigned int max_substreams;
	unsigned int channel_mode;
	unsigned int sample_rate;
	unsigned int runtime_status;
	unsigned int device_status;
};

/* Define a Device data structure for MStar Audio DSP */
struct MStar_Device_Struct {
	struct snd_card *card;
	struct snd_pcm *pcm[MAD_MAX_DEVICES];
	struct MStar_Runtime_Struct pcm_playback[MAD_MAX_DEVICES];
	struct MStar_Runtime_Struct pcm_capture[MAD_MAX_DEVICES];
	unsigned int active_playback_devices;
	unsigned int active_capture_devices;
};

#endif /* _MDRV_ALSA_DRIVER_HEADER */

