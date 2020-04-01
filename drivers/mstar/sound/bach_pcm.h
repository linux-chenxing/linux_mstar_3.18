/*------------------------------------------------------------------------------
	Copyright (c) 2008 MStar Semiconductor, Inc.  All rights reserved.
------------------------------------------------------------------------------*/
#ifndef _BACH_PCM_H_
#define _BACH_PCM_H_

//------------------------------------------------------------------------------
//  Include Files
//------------------------------------------------------------------------------
#include "bach.h"

//------------------------------------------------------------------------------
//  Macros
//------------------------------------------------------------------------------
#define DMA_EMPTY 		   0
#define DMA_UNDERRUN		 1
#define DMA_OVERRUN      2
#define DMA_FULL         3
#define DMA_NORMAL 		   4

// hardware dma variable
#define PCM_DMA_SIZE		(40*1024)
#define PCM_DMA_SIZE_ORDER	(3)				// calculate in page size
#define PCM_DMA_OVERRUN		(PCM_DMA_SIZE << 1)
#define PCM_DMA_UNDERRUN	(32*1024)

//------------------------------------------------------------------------------
//  Variables
//------------------------------------------------------------------------------

struct bach_pcm_dma_data
{
  U8 *name;    /* stream identifier */
  U32 channel;    /* Channel ID */
  dma_addr_t dma_addr;
  S32 dma_size;    /* Size of the DMA transfer */
};

#endif /* _BACH_PCM_H_ */
