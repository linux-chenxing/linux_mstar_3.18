///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2010 - 2014 MStar Semiconductor, Inc.
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

#ifndef _MSTAR_PCIE_AXI2MI_BRIDGE_HEADER
#define _MSTAR_PCIE_AXI2MI_BRIDGE_HEADER

/*
 * ============================================================================
 * INCLUDE HEADERS
 * ============================================================================
 */
#include "mstar_pcie_addrmap.h"

/*
 * ============================================================================
 * LOCAL VARIABLES DEFINITIONS
 * ============================================================================
 */

/*
 * ============================================================================
 * CONSTANT DEFINITIONS
 * ============================================================================
 */

#define REG_PCIE_CM_ADDR            (0x0000*4 + (REG_AXI2MI_BASE))		/* 32 BITS */
#define REG_PCIE_CM_WDATA           (0x0002*4 + (REG_AXI2MI_BASE))		/* 32 BITS */
#define REG_PCIE_CM_RDATA           (0x0004*4 + (REG_AXI2MI_BASE))		/* 32 BITS */
#define REG_PCIE_CM_WSTRB  	        (0x0006*4 + (REG_AXI2MI_BASE))		/* 4 BITS */
		#define REG_PCIE_CM_WSTRB_MASK            0x0000000FU
#define REG_PCIE_CM_DIRECTION  	    (0x0006*4 + (REG_AXI2MI_BASE))		/* 1 BITS */
		#define REG_PCIE_CM_DIRECTION_MASK        0x00000010U
#define REG_PCIE_CM_DONE_INT_EN	    (0x0006*4 + (REG_AXI2MI_BASE))	  /* 1 BITS */
		#define REG_PCIE_CM_DONE_INT_EN_MASK      0x00000020U
#define REG_PCIE_CM_AXI_ID          (0x0006*4 + (REG_AXI2MI_BASE))	  /* 8 BITS */
		#define REG_PCIE_CM_AXI_ID_MASK           0x0000FF00U
#define REG_PCIE_CM_AXI_RESP        (0x0007*4 + (REG_AXI2MI_BASE))	  /* 2 BITS */
		#define REG_PCIE_CM_AXI_RESP_MASK         0x00000003U
#define REG_PCIE_CM_DONE_INT        (0x0007*4 + (REG_AXI2MI_BASE))	  /* 1 BITS */
		#define REG_PCIE_CM_DONE_INT_MASK         0x00000004U
#define REG_PCIE_CM_FIRE            (0x0008*4 + (REG_AXI2MI_BASE))	  /* 1 BITS */
		#define REG_PCIE_CM_FIRE_MASK             0x00000001U
#define REG_PCIE_CM_DONE_INT_CLR    (0x0008*4 + (REG_AXI2MI_BASE))	  /* 1 BITS */
		#define REG_PCIE_CM_DONE_INT_CLR_MASK     0x00000002U
#define REG_PCIE_BRI_SW_RST         (0x0009*4 + (REG_AXI2MI_BASE))	  /* 1 BITS */
		#define REG_PCIE_BRI_SW_RST_MASK          0x00000001U
#define REG_PCIE_OBWP_LOCK          (0x000A*4 + (REG_AXI2MI_BASE))	  /* 1 BITS */
		#define REG_PCIE_OBWP_LOCK_MASK           0x00000001U
#define REG_CH0_HEAD_DESC_ADDR      (0x0010*4 + (REG_AXI2MI_BASE))	  /* 28 BITS */
		#define REG_CH0_HEAD_DESC_ADDR_MASK       0x0FFFFFFFU
#define REG_CH0_HEAD_DESC_MI_PRI    (0x0012*4 + (REG_AXI2MI_BASE))    /* 1 BITS */
		#define REG_CH0_HEAD_DESC_MI_PRI_MASK     0x00000001U
#define REG_CH0_LLIST_DONE_INT_EN   (0x0012*4 + (REG_AXI2MI_BASE))    /* 1 BITS */
		#define REG_CH0_LLIST_DONE_INT_EN_MASK    0x00000002U
#define REG_CH0_HEAD_DESC_FIRE      (0x0013*4 + (REG_AXI2MI_BASE))    /* 1 BITS */
		#define REG_CH0_HEAD_DESC_FIRE_MASK       0x00000001U
#define REG_CH0_LLIST_DONE_INT_CLR  (0x0013*4 + (REG_AXI2MI_BASE))    /* 1 BITS */
		#define REG_CH0_LLIST_DONE_INT_CLR_MASK   0x00000002U
#define REG_CH0_DESC_DONE_INT_CLR   (0x0013*4 + (REG_AXI2MI_BASE))    /* 1 BITS */
		#define REG_CH0_DESC_DONE_INT_CLR_MASK    0x00000004U
#define REG_CH0_LLIST_DONE_INT      (0x0014*4 + (REG_AXI2MI_BASE))    /* 1 BITS */
		#define REG_CH0_LLIST_DONE_INT_MASK       0x00000001U
#define REG_CH0_DESC_DONE_INT       (0x0014*4 + (REG_AXI2MI_BASE))    /* 1 BITS */
		#define REG_CH0_DESC_DONE_INT_MASK        0x00000002U
#define REG_CH0_DESC_CNT            (0x0014*4 + (REG_AXI2MI_BASE))    /* 4 BITS */
		#define REG_CH0_DESC_CNT_MASK             0x0000003CU
#define REG_CH0_DESC_CNT_CLR        (0x0019*4 + (REG_AXI2MI_BASE))    /* 1 BITS */
		#define REG_CH0_DESC_CNT_CLR_MASK         0x00000001U

#define REG_CH0_DESC_STATUS_LO      (0x0030*4 + (REG_AXI2MI_BASE))    /* 64 BITS */
		#define REG_CH0_DESC_STATUS_LO_MASK       0xFFFFFFFFU
#define REG_CH0_DESC_STATUS_HI      (0x0032*4 + (REG_AXI2MI_BASE))
		#define REG_CH0_DESC_STATUS_HI_MASK       0xFFFFFFFFU
#define REG_PCIE_TARGET_AWUSER_LO   (0x0036*4 + (REG_AXI2MI_BASE))    /* 80 BITS */
		#define REG_PCIE_TARGET_AWUSER_LO_MASK    0xFFFFFFFFU
#define REG_PCIE_TARGET_AWUSER_MI   (0x0038*4 + (REG_AXI2MI_BASE))
		#define REG_PCIE_TARGET_AWUSER_MI_MASK    0xFFFFFFFFU
#define REG_PCIE_TARGET_AWUSER_HI   (0x003A*4 + (REG_AXI2MI_BASE))
		#define REG_PCIE_TARGET_AWUSER_HI_MASK    0x0000FFFFU
#define REG_PCIE_TARGET_ARUSER_LO   (0x003B*4 + (REG_AXI2MI_BASE))    /* 80 BITS */
		#define REG_PCIE_TARGET_ARUSER_LO_MASK    0xFFFFFFFFU
#define REG_PCIE_TARGET_ARUSER_MI   (0x003D*4 + (REG_AXI2MI_BASE))
		#define REG_PCIE_TARGET_ARUSER_MI_MASK    0xFFFFFFFFU
#define REG_PCIE_TARGET_ARUSER_HI   (0x003F*4 + (REG_AXI2MI_BASE))
		#define REG_PCIE_TARGET_ARUSER_HI_MASK    0x0000FFFFU

#define REG_BRESP_FAST_U0           (0x0040*4 + (REG_AXI2MI_BASE))    /* 1 BITS */
		#define REG_BRESP_FAST_U0_MASK            0x00000001U
#define REG_64B_WFUL_ON             (0x0040*4 + (REG_AXI2MI_BASE))    /* 1 BITS */
		#define REG_64B_WFUL_ON_MASK              0x00000100U
#define REG_RESET_MI_REQ            (0x0041*4 + (REG_AXI2MI_BASE))    /* 1 BITS */
		#define REG_RESET_MI_REQ_MASK             0x00000001U
#define REG_G3D_BRI_STATUS_SEL      (0x0042*4 + (REG_AXI2MI_BASE))    /* 3 BITS */
		#define REG_G3D_BRI_STATUS_SEL_MASK       0x00000007U
#define REG_G3D_STATUS_CLR          (0x0042*4 + (REG_AXI2MI_BASE))    /* 1 BITS */
		#define REG_G3D_STATUS_CLR_MASK           0x00000008U
#define REG_G3D_WPROT_EN            (0x0042*4 + (REG_AXI2MI_BASE))    /* 1 BITS */
		#define REG_G3D_WPROT_EN_MASK             0x00000010U
#define REG_G3D_RREQ_RD_LEN         (0x0042*4 + (REG_AXI2MI_BASE))    /* 3 BITS */
		#define REG_G3D_RREQ_RD_LEN_MASK          0x000000E0U
#define REG_TIMEOUT_FLUSH           (0x0042*4 + (REG_AXI2MI_BASE))    /* 8 BITS */
		#define REG_TIMEOUT_FLUSH_MASK            0x0000FF00U
#define REG_G3D_RREQ_THRD           (0x0043*4 + (REG_AXI2MI_BASE))    /* 4 BITS */
		#define REG_G3D_RREQ_THRD_MASK            0x0000000FU
#define REG_G3D_RREQ_PRI            (0x0043*4 + (REG_AXI2MI_BASE))    /* 4 BITS */
		#define REG_G3D_RREQ_PRI_MASK             0x000000F0U
#define REG_G3D_RREQ_MAX            (0x0043*4 + (REG_AXI2MI_BASE))    /* 8 BITS */
		#define REG_G3D_RREQ_MAX_MASK             0x0000FF00U
#define REG_G3D_WREQ_THRD           (0x0044*4 + (REG_AXI2MI_BASE))    /* 4 BITS */
		#define REG_G3D_WREQ_THRD_MASK            0x0000000FU
#define REG_G3D_WREQ_PRI            (0x0044*4 + (REG_AXI2MI_BASE))    /* 4 BITS */
		#define REG_G3D_WREQ_PRI_MASK             0x000000F0U
#define REG_G3D_WREQ_MAX            (0x0044*4 + (REG_AXI2MI_BASE))    /* 8 BITS */
		#define REG_G3D_WREQ_MAX_MASK             0x0000FF00U
#define REG_G3D_WPROT_LOW_ADR       (0x0045*4 + (REG_AXI2MI_BASE))    /* 28 BITS */
		#define REG_G3D_WPROT_LOW_ADR_MASK        0x0FFFFFFFU
#define REG_G3D_WPROT_HIGH_ADR      (0x0047*4 + (REG_AXI2MI_BASE))    /* 28 BITS */
		#define REG_G3D_WPROT_HIGH_ADR_MASK       0x0FFFFFFFU
#define REG_BRI_DUMMY_REG0          (0x0049*4 + (REG_AXI2MI_BASE))    /* 16 BITS */
		#define REG_BRI_DUMMY_REG0_MASK           0x0000FFFFU
#define REG_BRI_DUMMY_REG1          (0x004A*4 + (REG_AXI2MI_BASE))    /* 16 BITS */
		#define REG_U03_DYNAMIC_MIU01_SELECTION         (1<<0)
		#define REG_U03_DYNAMIC_MIU01_CROSSING_CHECK    (1<<1)
		#define REG_U03_VAPA_ADDRESS_TRANSLATION        (1<<4)
		#define REG_U03_FIX_AXI_RLAST                   (1<<5)
		#define REG_U03_VAPA_ADDRESS_OFFSET_VALUE        8                /* 4 BITS [11:8] */
		#define REG_BRI_DUMMY_REG1_MASK           0x0000FFFFU
#define REG_PCIE_BRI_BIST_FAIL      (0x004B*4 + (REG_AXI2MI_BASE))    /* 6 BITS */
		#define REG_PCIE_BRI_BIST_FAIL_MASK       0x0000003FU
#define REG_PCIE_BRI_STATUS_0       (0x004C*4 + (REG_AXI2MI_BASE))    /* 16 BITS */
		#define REG_PCIE_BRI_STATUS_0_MASK        0x0000FFFFU
#define REG_PCIE_BRI_STATUS_1       (0x004D*4 + (REG_AXI2MI_BASE))    /* 16 BITS */
		#define REG_PCIE_BRI_STATUS_1_MASK        0x0000FFFFU

#define REG_MSIRCV_MADDR            (0x0050*4 + (REG_AXI2MI_BASE))    /* 32 BITS */
		#define REG_MSIRCV_MADDR_MASK             0xFFFFFFFFU
#define REG_MSIRCV_MDATA            (0x0052*4 + (REG_AXI2MI_BASE))    /* 32 BITS */
		#define REG_MSIRCV_MDATA_MASK             0xFFFFFFFFU
#define REG_MSIRCV_CONTROL          (0x0054*4 + (REG_AXI2MI_BASE))
		#define REG_MSIRCV_MSI2QUEUE_EN           (1<<1)
		#define REG_MSIRCV_MSI2VTABLE_EN          (1<<2)
		#define REG_MSIRCV_QUEUE_RD_EN            (1<<3)
		#define REG_MSIRCV_QUEUE_DATA_CLR         (1<<4)
		#define REG_MSIRCV_QUEUE_FULL_INT_CLR     (1<<5)
		#define REG_MSIRCV_QUEUE_EMPTY_INT_CLR    (1<<6)
		#define REG_MSIRCV_QUEUE_NEMPTY_INT_CLR   (1<<7)
		#define REG_MSIRCV_VTABLE_CLR             (1<<8)
		#define REG_MSIRCV_MSI_ADDR2MIU_EN        (1<<12)
		#define REG_MSIRCV_MSI_DATA2MIU_EN        (1<<13)

#define REG_MSIRCV_VTABLE_FUNC0     (0x0058*4 + (REG_AXI2MI_BASE))    /* 32 BITS */
		#define REG_MSIRCV_VTABLE_FUNC0_MASK      0xFFFFFFFFU
#define REG_MSIRCV_QUEUE_RDATA      (0x005A*4 + (REG_AXI2MI_BASE))    /* 32 BITS */
		#define REG_MSIRCV_QUEUE_RDATA_MASK       0xFFFFFFFFU
#define REG_MSIRCV_QUEUE_CONTROL    (0x005C*4 + (REG_AXI2MI_BASE))
		#define REG_MSIRCV_QUEUE_DEEPTH           0x0000003FU
		#define REG_MSIRCV_QUEUE_OVERFLOW         0x00000100U
		#define REG_MSIRCV_QUEUE_FULL_INT         0x00001000U
		#define REG_MSIRCV_QUEUE_EMPTY_INT        0x00002000U
		#define REG_MSIRCV_QUEUE_NEMPTY_IN        0x00004000U
		#define REG_MSIRCV_VTABLE_INT0            0x00008000U

#endif
