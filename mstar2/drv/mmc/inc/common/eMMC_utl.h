//<MStar Software>
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2010 - 2012 MStar Semiconductor, Inc.
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

#ifndef __eMMC_UTL_H__
#define __eMMC_UTL_H__

#include "eMMC.h"


typedef eMMC_PACK0 struct _eMMC_TEST_ALIGN_PACK {

	U8	u8_0;
	U16	u16_0;
	U32	u32_0, u32_1;

} eMMC_PACK1 eMMC_TEST_ALIGN_PACK_t;

extern  U32  eMMC_CheckAlignPack(U8 u8_AlignByteCnt);
extern void  eMMC_dump_mem(unsigned char *buf, U32 cnt);
extern void  eMMC_dump_mem_32(U32 *buf, U32 cnt);
extern  U32  eMMC_ComapreData(U8 *pu8_Buf0, U8 *pu8_Buf1, U32 u32_ByteCnt);
extern  U32  eMMC_ChkSum(U8 *pu8_Data, U32 u32_ByteCnt);
extern  U32  eMMC_PrintDeviceInfo(void);
extern  U32  eMMC_CompareCISTag(U8 *tag);
extern  void eMMC_dump_nni(eMMC_NNI_t *peMMCInfo);
extern  void eMMC_dump_pni(eMMC_PNI_t *pPartInfo);
extern  void eMMC_dump_WR_Count(void);
#endif // __eMMC_UTL_H__
