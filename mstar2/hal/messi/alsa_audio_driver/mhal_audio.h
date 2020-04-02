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


/*
 * ============================================================================
 * Forward Declarations
 * ============================================================================
 */
extern void Chip_Flush_Memory(void);

#endif /* _MHAL_ALSA_DRIVER_HEADER */

