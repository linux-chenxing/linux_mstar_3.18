///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2008 - 2009 MStar Semiconductor, Inc.
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
/// file    mhal_pq_adaptive.c
/// @brief  MStar GFLIP DDI HAL LEVEL
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _HAL_PQ_ADAPTIVE_C
#define _HAL_PQ_ADAPTIVE_C

#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include "mdrv_mstypes.h"
#include "mdrv_xc_st.h"
#include "mhal_xc.h"
//#include "mhal_dlc.h"
#include "mhal_pq_adaptive.h"
//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//  Local Variables for DBK
//-------------------------------------------------------------------------------------------------
#define MaxWidth 4097
#define paraSize 32
static MS_U8 WidthIIR = 8;

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define MSIF_ADAPTIVE_LIB_CODE_C            {'A','D','A','P','T','I','V','E','_'}
#define MSIF_ADAPTIVE_VERSION_C             {'0','0','0','0','0','1'}
#define PQ_ADAPTIVE_VERSION  0x9133

MS_U32 Get_PQ_Adaptive_Version(void)
{
    return PQ_ADAPTIVE_VERSION;
}

#ifndef UNUSED //to avoid compile warnings...
#define UNUSED(var) (void)((var) = (var))
#endif

//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------
//extern MS_U16 _u16PQSrcType_DBK_Detect[PQ_MAX_WINDOW]; //For Auto_DBK SW driver used
//static MS_U16 _u16PQPre_SrcType = 255;          //For Auto_DBK SW driver used
//static MS_BOOL bdataread = true;                //For Auto_DBK SW driver used
static MS_BOOL bg_bAnyCandenceEnable = TRUE;

static MS_BOOL bisHDSource =true;
static XC_MCNRLutQmapSetting m_stMCNRLutQmapSetting = {{0, 0, 0, 0}, {0, 0, 0, 0},  {0, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0}};

#define SZ_PATTERN_ENABLE 1
#define MCNR_LUT_LOAD_FROM_QMAP 1
#define PDNR_LUT_LOAD_FROM_QMAP 1

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
MS_S32 minmax(MS_S32 s32TempA, MS_S32 s32TempB, MS_S32 s32TempC) // make sure s32TempB < s32TempC
{
    if (s32TempA<=s32TempB)
        return s32TempB;
    else if (s32TempA >= s32TempC)
        return s32TempC;
    else
        return s32TempA;
}

MS_S32 max_1(MS_S32 s32TempA, MS_S32 s32TempB)
{
    if (s32TempA>=s32TempB)
        return s32TempA;
    else
        return s32TempB;
}

MS_S32 max_3(MS_S32 s32TempA, MS_S32 s32TempB, MS_S32 s32TempC)
{
    if ( (s32TempA>=s32TempB) && (s32TempA>=s32TempC) )
        return s32TempA;
    else if ((s32TempB>=s32TempA) && (s32TempB>=s32TempC) )
        return s32TempB;
    else
        return s32TempC;
}

MS_S32 min_1(MS_S32 s32TempA, MS_S32 s32TempB)
{
    if (s32TempA>=s32TempB)
        return s32TempB;
    else
        return s32TempA;
}

MS_S32 min_3(MS_S32 s32TempA, MS_S32 s32TempB, MS_S32 s32TempC)
{
    if ( (s32TempA<=s32TempB) && (s32TempA<=s32TempC) )
        return s32TempA;
    else if ((s32TempB<=s32TempA) && (s32TempB<=s32TempC) )
        return s32TempB;
    else
        return s32TempC;
}

/******************************************************************************/
///Get control register for adaptive tuning function
///@return MS_U8: Control status
/******************************************************************************/
MS_U8 MDrv_SC_get_adaptive_ctrl(void)
{
    MS_U8 u8Ctrl;

    u8Ctrl = (MS_U8)MHal_XC_R2BYTEMSK(REG_SC_BK22_7C_L, 0x00FF);

    return u8Ctrl;
}

/******************************************************************************/
///Get control register for adaptive tuning function
///@return MS_U8: Control status
/******************************************************************************/
MS_U8 MDrv_SC_get_adaptive_ctrl2(void)
{
    MS_U8 u8Ctrl;

    u8Ctrl = (MS_U8)(MHal_XC_R2BYTEMSK(REG_SC_BK22_7C_L, 0xFF00) >> 8);

    return u8Ctrl;
}

/******************************************************************************/
///Get control register for adaptive tuning function
///@return MS_U8: Control status
/******************************************************************************/
MS_U8 MDrv_SC_get_adaptive_ctrl3(void)
{
    MS_U8 u8Ctrl;

    u8Ctrl = (MS_U8) (MHal_XC_R2BYTEMSK(REG_SC_BK30_01_L, 0x8000) >> 8);

    return u8Ctrl;
}

/******************************************************************************/
///Read motion value (F2 motion status)
///@return MS_U8: Motion value
/******************************************************************************/
MS_U32 MDrv_SC_read_motion_value1(void)
{
    MS_U32 u32MotionValue;
    MS_U32 u32RegMadi_1C, u32RegMadi_1B, u32RegMadi_1A;

    u32RegMadi_1C = (MS_U32)MHal_XC_R2BYTEMSK(REG_SC_BK22_0E_L, 0x3F);
    u32RegMadi_1B = (MS_U32)MHal_XC_R2BYTEMSK(REG_SC_BK22_0D_L, 0xFF00);
    u32RegMadi_1A = (MS_U32)MHal_XC_R2BYTEMSK(REG_SC_BK22_0D_L, 0x00FF);

    u32RegMadi_1C = u32RegMadi_1C * 0x10000UL;

    u32MotionValue = (u32RegMadi_1C + u32RegMadi_1B + u32RegMadi_1A)  ;

    DRVSCA_DBG(printf("MotionValue = 0x%lx\n", u32MotionValue));

    return u32MotionValue;
}

/******************************************************************************/
///Read motion value (Motion count status)
///@return MS_U8: Motion value
/******************************************************************************/
MS_U32 MDrv_SC_read_motion_value2(void)
{
    MS_U32 u32MotionValue;

    u32MotionValue = (MS_U32)MHal_XC_R2BYTEMSK(REG_SC_BK0A_18_L, 0xFFF) + (MS_U32)MHal_XC_R2BYTEMSK(REG_SC_BK0A_1A_L, 0x7F);

    return u32MotionValue;
}

/******************************************************************************/
///Read motion value (Motion count status)
///@return MS_U8: Motion value
/******************************************************************************/
MS_U8 MDrv_SC_read_motion_value3(void)
{
    MS_U8 u8MotionValue;

    u8MotionValue = MHal_XC_R2BYTEMSK(REG_SC_BK0A_1A_L, 0x7F);

    return u8MotionValue;
}
MS_U32 MDrv_SC_read_motion_value4(void)
{
    MS_U32 u32MotionValue;

    u32MotionValue = (MS_U32)MHal_XC_R2BYTEMSK(REG_SC_BK0A_2E_L, 0x7FFF);

    return u32MotionValue;
}

//-------------------------------------------------------------------------------------------------
/// Read motion value (Motion count status)
/// @return MS_U8: Motion value
//-------------------------------------------------------------------------------------------------
MS_U8 Hal_PQ_GetCurrentMotionValue(void)
{
    MS_U8 u8MotionValue;

    u8MotionValue  = (MS_U8)((MHal_XC_R2BYTEMSK(REG_SC_BK22_0E_L, 0x3F) << 2) |
                             (MHal_XC_R2BYTEMSK(REG_SC_BK22_0D_L, 0xC000) >> 6));

    return u8MotionValue;
}
/******************************************************************************/
///Read Crop Size
///@return MS_BOOL: Success/Failed
/******************************************************************************/
MS_BOOL MApi_XC_GetStatus(XC_ApiStatus *stXCStatus , MS_BOOL k)
{
    MS_U16 SC_BK01_1F_VAL;
    MS_U16 SC_BK01_20_VAL;
    stXCStatus->stCapWin.height = MHal_XC_R2BYTEMSK(REG_SC_BK01_06_L , 0x1FFF);
    stXCStatus->stCapWin.width  = MHal_XC_R2BYTEMSK(REG_SC_BK01_07_L , 0x3FFF);

    stXCStatus->stCropWin.height = MHal_XC_R2BYTEMSK(REG_SC_BK01_06_L , 0x1FFF); //unknow register wait David
    stXCStatus->stCropWin.width  = MHal_XC_R2BYTEMSK(REG_SC_BK01_07_L , 0x3FFF); //unknow register wait David

    stXCStatus->ScaledCropWin.height = MHal_XC_R2BYTEMSK(REG_SC_BK01_06_L , 0x1FFF); //unknow register wait David
    stXCStatus->ScaledCropWin.width  = MHal_XC_R2BYTEMSK(REG_SC_BK01_07_L , 0x3FFF); //unknow register wait David

    stXCStatus->bInterlace = MHal_XC_R2BYTEMSK(REG_SC_BK01_1E_L , 0x0800) >> 11;

    stXCStatus->u16H_SizeAfterPreScaling = stXCStatus->stCapWin.width; //bcz maserati didn't have pre down scaling so = stCapWin.width


     SC_BK01_1F_VAL = MHal_XC_R2BYTEMSK(REG_SC_BK01_1F_L , 0x1FFF);
     SC_BK01_20_VAL = MHal_XC_R2BYTEMSK(REG_SC_BK01_20_L , 0x3FFF);

    if( SC_BK01_1F_VAL <= 1 || SC_BK01_20_VAL == 0)
    {
        return 0;
    }
    else
    {
        if(stXCStatus->bInterlace == 1) //i mode
        {
            stXCStatus->u16InputVFreq = 24000000 * 10 / ( ((SC_BK01_1F_VAL) -1)*(SC_BK01_20_VAL));
        }else //p mode
        {
            stXCStatus->u16InputVFreq = 12000000 * 10 / ( ((SC_BK01_1F_VAL) -1)*(SC_BK01_20_VAL));
        }
    }

    return 1;
}
/******************************************************************************/
///Read Luma weight/Count
///@return MS_U8:luma Average value
/******************************************************************************/
MS_U8 MDrv_SC_GetAverageLuminous(void) // = external msGetAverageLuminousValue()
{
    MS_U16 uwTmpAvr;
    MS_U8 ucHistogramTotal_L = MHal_XC_ReadByte( REG_SC_BK1A_06_L );
    MS_U8 ucHistogramTotal_H = MHal_XC_ReadByte( REG_SC_BK1A_06_H );
    MS_U16 wLumiAverageTemp = ((WORD)ucHistogramTotal_H<<8) + ucHistogramTotal_L;
    MS_U16 wLumiTotalCount = ((WORD)MHal_XC_ReadByte(REG_SC_BK1A_07_H) << 8) + MHal_XC_ReadByte(REG_SC_BK1A_07_L);
    // Normalize
    if (wLumiAverageTemp!=0)
        uwTmpAvr = (256 * (DWORD)wLumiAverageTemp + (wLumiTotalCount/2)) / wLumiTotalCount;
    else
        uwTmpAvr = 0;

    if (uwTmpAvr >= 255)
        uwTmpAvr = 255;
    return (BYTE)uwTmpAvr;
}
/******************************************************************************/
///Read VCnt value
///@return MS_U8: VCnt value
/******************************************************************************/
MS_U8 MDrv_SC_read_v_cnt(void)
{
    MS_U8 u8VCnt;

    u8VCnt = (MS_U8)(MHal_XC_R2BYTEMSK(REG_SC_BK22_0E_L, 0xC0) >> 6);
    return u8VCnt;
}

/******************************************************************************/
///Read SDHDD Detect Threshold value
///@return MS_U16: HDSDD_Det_Threshold value
/******************************************************************************/
MS_U16 MDrv_SC_read_HDSDD_Det_Threshold_value(void)
{
    MS_U16 u16Value;

    u16Value = 0x30;    //(MS_U16)MHal_XC_R2BYTEMSK(REG_SC_BK22_7F_L, 0xFFFF);

    return u16Value;
}

/******************************************************************************/
///MCDi Driver
/******************************************************************************/

#define MF_Gain            30
MS_U32 MDrv_SC_T12_UC_SmallMoving(void)
{
    MS_U32 u32beforeMCfeather;
    MS_U32 u32nonMV0Comfirm, u32MVContradict;
    MS_U32 u32MovingFeather, u32MVConfirm;
    static MS_U32 u32PrevMVConfirm = 0;

    u32beforeMCfeather = MHal_XC_R2BYTE(REG_SC_BK2A_77_L) ;
    u32nonMV0Comfirm = MHal_XC_R2BYTE(REG_SC_BK2A_79_L);
    u32MVContradict = MHal_XC_R2BYTE(REG_SC_BK2A_7A_L);

    u32MVConfirm = (u32nonMV0Comfirm + u32MVContradict) >> 1;

    u32MVConfirm = (u32PrevMVConfirm * 3 + u32MVConfirm) >> 2;
    u32PrevMVConfirm = u32MVConfirm;

    if(u32MVConfirm > (u32beforeMCfeather * MF_Gain))
    {
        u32MovingFeather = u32MVConfirm - ((u32beforeMCfeather * MF_Gain)) ;
    }
    else
    {
        u32MovingFeather = 0;
     }

    return u32MovingFeather;  // "Big": in seach range "Small": out of seach range
}

#define OOSRF_Gain            4
#define OOSRF_Power         2
MS_U32 MDrv_SC_T12_UC_InSearchRange_Feather(void)
{
    MS_U32 u32beforeMCfeather, u32afterMCfeather;
    MS_U32 u32MCOOSRF;
    static MS_U32 u32PrevbeforeMCfeather = 0;
    static MS_U32 u32PrevafterMCfeather = 0;

    u32beforeMCfeather = MHal_XC_R2BYTE(REG_SC_BK2A_77_L) ;
    u32afterMCfeather = MHal_XC_R2BYTE(REG_SC_BK2A_7B_L);

    u32beforeMCfeather = (u32PrevbeforeMCfeather * 3 + u32beforeMCfeather) >> 2;
    u32PrevbeforeMCfeather = u32beforeMCfeather;

    u32afterMCfeather = (u32PrevafterMCfeather * 3 + u32afterMCfeather) >> 2;
    u32PrevafterMCfeather = u32afterMCfeather;

    if(u32beforeMCfeather > (u32afterMCfeather * OOSRF_Gain) >> OOSRF_Power)
    {
        u32MCOOSRF = u32beforeMCfeather - ((u32afterMCfeather * OOSRF_Gain) >> OOSRF_Power) ;
    }
    else
    {
        u32MCOOSRF = 0;
    }

    return u32MCOOSRF;  // "Big": in seach range "Small": out of seach range
}

#define OOSRMC_Gain         20
MS_U32 MDrv_SC_T12_UC_InSearchRange_MvComfirm(void)
{
    MS_U32 u32MV0Comfirm,  u32nonMV0Comfirm, u32MVContradict;
    static MS_U32 u32PrevMV0Comfirm = 0;
    static MS_U32 u32PrevnonMV0Comfirm = 0;
    static MS_U32 u32PrevMVContradict = 0;
    MS_U32 u32MVComfirm, u32MCOOSRMC;

    u32MV0Comfirm = MHal_XC_R2BYTE(REG_SC_BK2A_78_L);
    u32nonMV0Comfirm = MHal_XC_R2BYTE(REG_SC_BK2A_79_L);
    u32MVContradict = MHal_XC_R2BYTE(REG_SC_BK2A_7A_L);

    u32MV0Comfirm = (u32PrevMV0Comfirm * 3 + u32MV0Comfirm) >> 2;
    u32PrevMV0Comfirm = u32MV0Comfirm;

    u32nonMV0Comfirm = (u32PrevnonMV0Comfirm * 3 + u32nonMV0Comfirm) >> 2;
    u32PrevnonMV0Comfirm = u32nonMV0Comfirm;

    u32MVContradict = (u32PrevMVContradict * 3 + u32MVContradict) >> 2;
    u32PrevMVContradict = u32MVContradict;

    u32MVComfirm = u32MV0Comfirm + u32nonMV0Comfirm;

    if(u32MVComfirm > u32MVContradict * OOSRMC_Gain)
    {
        u32MCOOSRMC = u32MVComfirm - u32MVContradict * OOSRMC_Gain;
    }
    else
    {
        u32MCOOSRMC = 0;
     }

    return u32MCOOSRMC;  // "Big": in seach range "Small": out of seach range

}

#define Confirm_Gain         5
MS_U8 MDrv_SC_UC_InSearchRange_MR_Offset(void)
{
    MS_U32 u32MV0Comfirm,  u32nonMV0Comfirm, u32MVContradict;
    static MS_U32 u32PrevMV0Comfirm = 0;
    static MS_U32 u32PrevnonMV0Comfirm = 0;
    static MS_U32 u32PrevMVContradict = 0;
    MS_U32 u32MVComfirm;
    MS_U8 u8MROffset;

    u32MV0Comfirm = MHal_XC_R2BYTE(REG_SC_BK2A_78_L);
    u32nonMV0Comfirm = MHal_XC_R2BYTE(REG_SC_BK2A_79_L);
    u32MVContradict = MHal_XC_R2BYTE(REG_SC_BK2A_7A_L);

    u32MV0Comfirm = (u32PrevMV0Comfirm * 3 + u32MV0Comfirm) >> 2;
    u32PrevMV0Comfirm = u32MV0Comfirm;

    u32nonMV0Comfirm = (u32PrevnonMV0Comfirm * 3 + u32nonMV0Comfirm) >> 2;
    u32PrevnonMV0Comfirm = u32nonMV0Comfirm;

    u32MVContradict = (u32PrevMVContradict * 3 + u32MVContradict) >> 2;
    u32PrevMVContradict = u32MVContradict;

    u32MVComfirm = u32MV0Comfirm + u32nonMV0Comfirm;

    if(u32MVComfirm > (u32MVContradict * Confirm_Gain))
    {
        u8MROffset = u32nonMV0Comfirm;
    }
    else
    {
        u8MROffset = 0;
    }

    return u8MROffset;
}

#define ISR_NONMV0_Gain         15
MS_U32 MDrv_SC_T12_UC_InSearchRange_nonMv0Comfirm(void)
{
    MS_U32 u32MV0Comfirm,  u32nonMV0Comfirm, u32MVContradict;
    static MS_U32 u32PrevMV0Comfirm = 0;
    static MS_U32 u32PrevnonMV0Comfirm = 0;
    static MS_U32 u32PrevMVContradict = 0;
    MS_U32 u32MVComfirm, u32ComfirmValue;

    u32MV0Comfirm = MHal_XC_R2BYTE(REG_SC_BK2A_78_L);
    u32nonMV0Comfirm = MHal_XC_R2BYTE(REG_SC_BK2A_79_L);
    u32MVContradict = MHal_XC_R2BYTE(REG_SC_BK2A_7A_L);

    u32MV0Comfirm = (u32PrevMV0Comfirm * 3 + u32MV0Comfirm) >> 2;
    u32PrevMV0Comfirm = u32MV0Comfirm;

    u32nonMV0Comfirm = (u32PrevnonMV0Comfirm * 3 + u32nonMV0Comfirm) >> 2;
    u32PrevnonMV0Comfirm = u32nonMV0Comfirm;

    u32MVContradict = (u32PrevMVContradict * 3 + u32MVContradict) >> 2;
    u32PrevMVContradict = u32MVContradict;

    u32MVComfirm = /*MV0_Comfim +*/u32nonMV0Comfirm;

    if(u32MVComfirm > (u32MVContradict * ISR_NONMV0_Gain))
   {
        u32ComfirmValue = u32MVComfirm - u32MVContradict * ISR_NONMV0_Gain;
   }
    else
   {
        u32ComfirmValue = 0;
   }

    return u32ComfirmValue;  // "Big": in seach range "Small": out of seach range

}

void MDrv_SC_T12_DIPF_TemporalGainControl(MS_U32 u32OOSRMC, MS_U32 u32OOSRF, MS_U32 u32ISRNONMV0)
{
    static MS_U16 u16TemporalGainControl = 0x0F;

    UNUSED(u32OOSRMC);
    UNUSED(u32OOSRF);

    if(u32ISRNONMV0 > 1000)  //&& OOSRF > 50)
    {
        if(u16TemporalGainControl > 0x00)
       {
            u16TemporalGainControl--;
        }
        else
        {
            u16TemporalGainControl = 0;
        }
    }
    else
    {
        if(u16TemporalGainControl < 0x0F)
        {
            u16TemporalGainControl++;
        }
        else
        {
            u16TemporalGainControl = 0x0F;
        }
    }
    MHal_XC_W2BYTEMSK(REG_SC_BK22_14_L, (MS_U16)u16TemporalGainControl, 0x000F);

}

#define ComplexOffset_MaxValue 0x06
#define ComplexOffset_MinValue 0x00
void MDrv_SC_T12_UCDi_ComplexOffset(MS_U32 u32OOSRMC, MS_U32 u32OOSRF, MS_U32 u32ISRNONMV0)
{
    static MS_U16 u16ComplexOffset = ComplexOffset_MaxValue;

    UNUSED(u32OOSRMC);
    UNUSED(u32OOSRF);

    if(u32ISRNONMV0 > 1000)  //&& OOSRF > 50)
    {
        if(u16ComplexOffset > ComplexOffset_MinValue)
        {
            u16ComplexOffset--;
        }
        else
        {
            u16ComplexOffset = ComplexOffset_MinValue;
        }
    }
    else
    {
        if(u16ComplexOffset < ComplexOffset_MaxValue)
        {
            u16ComplexOffset++;
        }
        else
        {
            u16ComplexOffset = ComplexOffset_MaxValue;
        }
    }
    MHal_XC_W2BYTEMSK(REG_SC_BK2A_2E_L, (MS_U16)u16ComplexOffset << 4, 0x00F0);
}

#define OP1_HisWeight_MaxValue 0x0E
#define OP1_HisWeight_MinValue 0x04
void MDrv_SC_T12_UCDi_OP1_HisWeight(MS_U8 u8MotLvl)
{
    MS_BOOL bfilm;

    bfilm = ((MS_U16)MHal_XC_R2BYTEMSK(REG_SC_BK0A_21_L, 0x0008) == 0x0008) ||   // film22
           ((MS_U16)MHal_XC_R2BYTEMSK(REG_SC_BK0A_21_L, 0x0010) == 0x0010) ||   // film32
           ((MS_U16)MHal_XC_R2BYTEMSK(REG_SC_BK0A_21_L, 0x0040) == 0x0040) ;    // film any

    if((u8MotLvl >= 1) || (true == bfilm))
    {
        MHal_XC_W2BYTEMSK(REG_SC_BK2A_02_L, OP1_HisWeight_MaxValue, 0x000e);  // history weight = 6
    }
    else
    {
        MHal_XC_W2BYTEMSK(REG_SC_BK2A_02_L, OP1_HisWeight_MinValue, 0x000e);  // history weight = 2
    }
}

#define A5_HIS_MOTION_TH 0x0A
void MDrv_SC_A5_UCDi_OP1_HisWeight(const MS_U32 u32MotionValue,const MS_U32 u32FeatherSum,const MS_U8 u8MotionValue3)
{
    static MS_U32 u32reghisweight = OP1_HisWeight_MinValue;
    static MS_U32 u32MotionValue_prev = 0;
    MS_BOOL bisSDsource;
    MS_U32 u32Htotalpix = 0;
    MS_U32 u32Vtotalpix = 0;
    u32Htotalpix = MHal_XC_R2BYTE(REG_SC_BK02_3E_L);
    u32Vtotalpix = MHal_XC_R2BYTE(REG_SC_BK02_3F_L);

    if( u32MotionValue_prev < u32MotionValue )
    {
        u32MotionValue_prev = ((u32MotionValue_prev*3 )+ u32MotionValue ) >> 2;
    }
    else
    {
        u32MotionValue_prev = u32MotionValue;
    }

    if(u8MotionValue3 < A5_HIS_MOTION_TH)
    {
        u32reghisweight = OP1_HisWeight_MinValue;
    }
    else if( u32MotionValue_prev < A5_HIS_MOTION_TH )
    {
        u32reghisweight = OP1_HisWeight_MinValue;
    }
    else
    {
        if( u32reghisweight < OP1_HisWeight_MaxValue )
        {
            u32reghisweight++;
        }
        else
        {
            u32reghisweight = OP1_HisWeight_MaxValue;
        }
    }

    if((u32Htotalpix < 0x5AA)&&(u32Vtotalpix < 0x120))
    {
        bisSDsource = true;
    }
    else
    {
        bisSDsource = false;
    }

    if((u32FeatherSum > 0x3500)&&(bisSDsource == true))
    {
        MHal_XC_W2BYTEMSK(REG_SC_BK2A_02_L, OP1_HisWeight_MinValue, 0x000e);  // history weight = 6
    }
    else
    {
        MHal_XC_W2BYTEMSK(REG_SC_BK2A_02_L, u32reghisweight, 0x000E);
    }
}

#define FeatherValueGain_MaxValue 0x02
#define FeatherValueGain_MinValue 0x00
void MDrv_SC_T12_UCDi_FeatherValueGain(MS_U32 u32OOSRMC, MS_U32 u32OOSRF, MS_U32 u32ISRNONMV0)
{
    static MS_U16 u16FeatherValue = FeatherValueGain_MaxValue;

    UNUSED(u32OOSRMC);
    UNUSED(u32OOSRF);

    if(u32ISRNONMV0 > 1000)  //&& OOSRF > 50)
    {
        if(u16FeatherValue > FeatherValueGain_MinValue)
        {
            u16FeatherValue--;
        }
        else
        {
            u16FeatherValue = FeatherValueGain_MinValue;
        }
    }
    else
    {
        if(u16FeatherValue < FeatherValueGain_MaxValue)
        {
            u16FeatherValue++;
        }
        else
        {
            u16FeatherValue = FeatherValueGain_MaxValue;
        }
    }
    MHal_XC_W2BYTEMSK(REG_SC_BK2A_2D_L, (MS_U16)u16FeatherValue << 12, 0xF000);
}

#define W2SadGain_MaxValue 0x0C
#define W2SadGain_MinValue 0x08
void MDrv_SC_T12_UCDi_W2SadGain(MS_U32 u32OOSRMC, MS_U32 u32OOSRF, MS_U32 u32ISRNONMV0)
{
    static MS_U16 u16W2SadGain = FeatherValueGain_MaxValue;

    UNUSED(u32OOSRMC);
    UNUSED(u32OOSRF);

    if(u32ISRNONMV0 > 1000)  //&& OOSRF > 50)
    {
        if(u16W2SadGain < W2SadGain_MaxValue)
        {
            u16W2SadGain++;
        }
        else
        {
            u16W2SadGain = W2SadGain_MaxValue;
        }
    }
    else
    {
        if(u16W2SadGain > W2SadGain_MinValue)
        {
            u16W2SadGain--;
        }
        else
        {
            u16W2SadGain = W2SadGain_MinValue;
        }
    }
    MHal_XC_W2BYTEMSK(REG_SC_BK2A_2D_L, (MS_U16)u16W2SadGain << 8, 0x0F00);
}

#define VerticalMovingUdRatio_MaxValue 0x08
#define VerticalMovingUdRatio_MinValue 0x07
void MDrv_SC_T12_UCDi_VerticalMovingUdRatio(MS_U32 u32OOSRMC, MS_U32 u32OOSRF, MS_U32 u32ISRNONMV0)
{
    static MS_U16 u16VerticalMovingUdRatioValue = VerticalMovingUdRatio_MaxValue;

    UNUSED(u32OOSRMC);
    UNUSED(u32ISRNONMV0);

    if( u32OOSRF > 0x300 )
    {
        if(u16VerticalMovingUdRatioValue > VerticalMovingUdRatio_MinValue)
        {
            u16VerticalMovingUdRatioValue--;
        }
        else
        {
            u16VerticalMovingUdRatioValue = VerticalMovingUdRatio_MinValue;
        }
    }
    else
    {
        if(u16VerticalMovingUdRatioValue < VerticalMovingUdRatio_MaxValue)
        {
            u16VerticalMovingUdRatioValue++;
        }
        else
        {
            u16VerticalMovingUdRatioValue = VerticalMovingUdRatio_MaxValue;
        }
    }
    MHal_XC_W2BYTEMSK(REG_SC_BK2A_28_L, (MS_U16)u16VerticalMovingUdRatioValue, 0x000F);
}

#define KFC_8bitMotionGain_MaxValue 0x08
#define KFC_8bitMotionGain_MinValue 0x04 //0 have feather issue.
void MDrv_SC_T12_KFC_8bitMotionGain(MS_U32 SmallMoving)
{
    static MS_U16 u16KFC8bitMotionGainValue = KFC_8bitMotionGain_MaxValue;

    if(SmallMoving > 500)
    {
        if(u16KFC8bitMotionGainValue > KFC_8bitMotionGain_MinValue)
        {
            u16KFC8bitMotionGainValue--;
        }
        else
        {
            u16KFC8bitMotionGainValue = KFC_8bitMotionGain_MinValue;
        }
    }
    else
    {
        if(u16KFC8bitMotionGainValue < KFC_8bitMotionGain_MaxValue)
        {
            u16KFC8bitMotionGainValue++;
        }
        else
        {
            u16KFC8bitMotionGainValue = KFC_8bitMotionGain_MaxValue;
        }
    }
    MHal_XC_W2BYTEMSK(REG_SC_BK2A_3B_L, (MS_U16)u16KFC8bitMotionGainValue << 4, 0x00F0);
}

#define UCDi_EODiWGain_MaxValue 0x0F
#define UCDi_EODiWGain_MinValue 0x00
void MDrv_SC_T12_UCDi_EODiWGain(MS_U32 u32OOSRMC, MS_U32 u32OOSRF, MS_U32 u32ISRNONMV0)
{
    static MS_U16 u16EODiWGainValue = UCDi_EODiWGain_MaxValue;

    UNUSED(u32OOSRMC);

    if((u32ISRNONMV0 > 1000) && (u32OOSRF > 50))
    {
        if(u16EODiWGainValue > UCDi_EODiWGain_MinValue)
        {
            u16EODiWGainValue--;
        }
        else
        {
            u16EODiWGainValue = UCDi_EODiWGain_MinValue;
        }
    }
    else
    {
        if(u16EODiWGainValue < UCDi_EODiWGain_MaxValue)
        {
            u16EODiWGainValue++;
        }
        else
        {
            u16EODiWGainValue = UCDi_EODiWGain_MaxValue;
        }
    }
    MHal_XC_W2BYTEMSK(REG_SC_BK22_2A_L, (MS_U16)u16EODiWGainValue << 12, 0xF000);
}

#define UCDi_HistoryRatio_MaxValue 0x07
#define UCDi_HistoryRatio_MinValue 0x04
void MDrv_SC_T12_UCDi_HistoryRatio(MS_U32 u32OOSRMC, MS_U32 u32OOSRF, MS_U32 u32ISRNONMV0)
{
    static MS_U16 u16HistoryRatioValue = UCDi_HistoryRatio_MaxValue;

    UNUSED(u32OOSRMC);

    if((u32ISRNONMV0 > 1000) && (u32OOSRF > 50))
    {
        if(u16HistoryRatioValue > UCDi_HistoryRatio_MinValue)
       {
            u16HistoryRatioValue--;
        }
        else
        {
            u16HistoryRatioValue = UCDi_HistoryRatio_MinValue;
        }
    }
    else
    {
        if(u16HistoryRatioValue < UCDi_HistoryRatio_MaxValue)
        {
            u16HistoryRatioValue++;
        }
        else
        {
            u16HistoryRatioValue = UCDi_HistoryRatio_MaxValue;
        }
    }
    MHal_XC_W2BYTEMSK(REG_SC_BK2A_2E_L, (MS_U16)u16HistoryRatioValue << 12, 0xF000);
}

#define KFC_EODIW_GAIN_MAX 4
#define KFC_EODIW_GAIN_MIN 0
void MDrv_SC_A5_KFC_EODiW(const MS_U32 u32IsVerticalMoving)
{
    MS_U32 u32Statisticscplxpixel;
    static MS_U32 u32Statisticscplxpixeliir = KFC_EODIW_GAIN_MIN;
    static MS_U32 u32regkfceodiwgain = KFC_EODIW_GAIN_MAX ;

    u32Statisticscplxpixel = ((u32Statisticscplxpixeliir*3 )+ MHal_XC_R2BYTE(REG_SC_BK2A_7F_L) ) >> 2;
    u32Statisticscplxpixeliir = u32Statisticscplxpixel;

    if( (u32IsVerticalMoving < 0x150) && (u32Statisticscplxpixeliir > 0x1000) )
    {
        if( u32regkfceodiwgain > KFC_EODIW_GAIN_MIN )
       {
            u32regkfceodiwgain--;
        }
        else
        {
            u32regkfceodiwgain  = KFC_EODIW_GAIN_MIN;
        }
    }
    else
    {
        if( u32regkfceodiwgain < KFC_EODIW_GAIN_MAX )
        {
            u32regkfceodiwgain++;
        }
        else
        {
            u32regkfceodiwgain = KFC_EODIW_GAIN_MAX;
        }
    }
     MHal_XC_W2BYTEMSK(REG_SC_BK22_28_L, (MS_U16)u32regkfceodiwgain << 12, 0xF000);
}

void MDrv_SC_SST_Ctrl(const MS_U32 u32FeatherSum, const MS_BOOL bisHDSrc)
{
    static MS_BOOL bReg_SST_en = true;
    //MS_U32 u32mv0_feather;
    MS_U32 u32hor_feather;
    //u32mv0_feather = MHal_XC_R2BYTE(REG_SC_BK2A_77_L);
    u32hor_feather = MHal_XC_R2BYTE(REG_SC_BK2A_7B_L);
    //if( (u32FeatherSum > 0x1200) && (u32mv0_feather>0x1100) && (!bisHDSrc) && (SZ_PATTERN_ENABLE ==1) ) //Add feathersum for TSB#3 capter4 white noise
    if( (u32FeatherSum > 0x1200) && (u32hor_feather>0x1100) && (!bisHDSrc) && (SZ_PATTERN_ENABLE ==1) )
    {
        bReg_SST_en = false;
    }
    else
    {
        bReg_SST_en = true;
    }
    //printf("===u32FeatherSum[%x] u32mv0_feather[%x] u32hor_feather[%x] !bisHDSrc[%d] SZ_PATTERN_ENABLE[%d]====\n",u32FeatherSum,u32mv0_feather,u32hor_feather,(!bisHDSrc),SZ_PATTERN_ENABLE);
    MHal_XC_W2BYTEMSK(REG_SC_BK2A_04_L, (MS_U16)(bReg_SST_en << 8), 0x0100);
}

#define SONYC_KFC_EN_MAX 1
#define SONYC_KFC_EN_MIN 0
#define SONYC_CPLX_GAIN_MAX 4
#define SONYC_CPLX_GAIN_MIN 0
void MDrv_SC_A5_SonyChurch(const MS_U32 u32IsVerticalMoving, const MS_U32 u32IsHorizontalMoving, const MS_U32 u32MotionValue, const MS_BOOL bL107_en)
{
    static MS_U32 u32regkfcen = SONYC_KFC_EN_MAX ;
    static MS_U32 u32regmcdicplxgain = SONYC_CPLX_GAIN_MAX;

    //IsHorizontalMoving=IsHorizontalMoving;
    if( (u32IsVerticalMoving > 0xF0) && (u32IsHorizontalMoving < 1) && (u32MotionValue > 0x40000) )
    {
        if( u32regmcdicplxgain > SONYC_CPLX_GAIN_MIN )
        {
            u32regmcdicplxgain--;
        }
        else
        {
            u32regmcdicplxgain = SONYC_CPLX_GAIN_MIN;
        }

        if( u32regkfcen > SONYC_KFC_EN_MIN )
        {
            u32regkfcen--;
        }
        else
        {
            u32regkfcen = SONYC_KFC_EN_MIN;
        }
    }
    else
    {
        if( u32regmcdicplxgain < SONYC_CPLX_GAIN_MAX )
        {
            u32regmcdicplxgain++;
        }
        else
        {
            u32regmcdicplxgain = SONYC_CPLX_GAIN_MAX;
        }

        if( u32regkfcen < SONYC_KFC_EN_MAX )
        {
            u32regkfcen++;
        }
        else
        {
            u32regkfcen = SONYC_KFC_EN_MAX;
        }
    }

    if( !bL107_en )
    {
        MHal_XC_W2BYTEMSK(REG_SC_BK2A_04_L, (MS_U16)u32regkfcen<<1, 0x0002);
        MHal_XC_W2BYTEMSK(REG_SC_BK2A_2E_L, (MS_U16)u32regmcdicplxgain<<8, 0x0F00);
    }
}

#define L107_FEATHER_GAIN_MAX 2
#define L107_FEATHER_GAIN_MIN 0
#define L107_CPLX_OFFSET_MAX 6
#define L107_CPLX_OFFSET_MIN 0
#define L107_CPLX_GAIN_MAX 15
#define L107_CPLX_GAIN_MIN 4
MS_BOOL MDrv_SC_A5_L107(const MS_U32 u32IsVerticalMoving, const MS_U32 u32IsHorizontalMoving, const MS_U32 u32TotalFeather, const MS_U32 u32TotalMvComfirm, const MS_U32 u8colorSum,const MS_U32 u32MotionValue, const MS_U32 u32uComplex )
{
    static MS_U32 u32regmcdifeathergain = L107_FEATHER_GAIN_MAX ;
    static MS_U32 u32regmcdicplxoffset = L107_CPLX_OFFSET_MAX ;
    static MS_U32 u32regmcdicplxgain = L107_CPLX_GAIN_MIN ;
    MS_BOOL bL107_en;

    if( (u32IsVerticalMoving < 20) && (u32IsHorizontalMoving < 200) && (u32TotalFeather < 100) && (u32TotalMvComfirm > 200) && (u8colorSum > 200) && (u32MotionValue > 0x3500) && (u32uComplex < 0x700))
    {
        bL107_en = TRUE;

        if( u32regmcdifeathergain > L107_FEATHER_GAIN_MIN )
        {
            u32regmcdifeathergain--;
        }
        else
        {
            u32regmcdifeathergain = L107_FEATHER_GAIN_MIN;
        }

        if( u32regmcdicplxoffset > L107_CPLX_OFFSET_MIN )
        {
            u32regmcdicplxoffset--;
        }
        else
        {
            u32regmcdicplxoffset = L107_CPLX_OFFSET_MIN;
        }

        if( u32regmcdicplxgain < L107_CPLX_GAIN_MAX )
        {
            u32regmcdicplxgain++;
        }
        else
        {
            u32regmcdicplxgain = L107_CPLX_GAIN_MAX;
        }
    }
    else
    {
        bL107_en = FALSE;

        if( u32regmcdifeathergain < L107_FEATHER_GAIN_MAX )
        {
            u32regmcdifeathergain++;
        }
        else
        {
            u32regmcdifeathergain = L107_FEATHER_GAIN_MAX;
        }

        if( u32regmcdicplxoffset < L107_CPLX_OFFSET_MAX )
        {
            u32regmcdicplxoffset++;
        }
        else
        {
            u32regmcdicplxoffset = L107_CPLX_OFFSET_MAX;
        }

        if( u32regmcdicplxgain > L107_CPLX_GAIN_MIN )
        {
            u32regmcdicplxgain--;
        }
        else
        {
            u32regmcdicplxgain = L107_CPLX_GAIN_MIN;
        }
    }

    MHal_XC_W2BYTEMSK(REG_SC_BK2A_2D_L, (MS_U16)u32regmcdifeathergain<<12, 0xF000); // 2 to 0
    MHal_XC_W2BYTEMSK(REG_SC_BK2A_2E_L, (MS_U16)u32regmcdicplxoffset<<4, 0x00F0);// 6 to 0
    MHal_XC_W2BYTEMSK(REG_SC_BK2A_2E_L, (MS_U16)u32regmcdicplxgain<<8, 0x0F00);// 4 to F

    return bL107_en;
}

MS_U32 MDrv_SC_A5_ComplexSum(void)
{
    static MS_U32 u32IsComplex_iir = 0xFFFF;
    MS_U32 u32IsComplex;

    u32IsComplex = MHal_XC_R2BYTE(REG_SC_BK2A_7F_L);

    u32IsComplex = (u32IsComplex_iir*3 + u32IsComplex ) >> 2;
    u32IsComplex_iir = u32IsComplex;

    return u32IsComplex_iir;
}

MS_U32 MDrv_SC_A5_KFC_IsVerticalMoving(void)
{
    MS_U32 u32mvc, u32mvu, u32mvd;
    MS_U32 u32mevertical;
    static MS_U32 u32IsVerticalMovingiir = 0;

    u32mvc     = MHal_XC_R2BYTE(REG_SC_BK2A_7C_L);
    u32mvu     = MHal_XC_R2BYTE(REG_SC_BK2A_7D_L);
    u32mvd     = MHal_XC_R2BYTE(REG_SC_BK2A_7E_L);

    if ((u32mvu > u32mvc) && (u32mvu > u32mvd) /*&& (u32_mvu > 0x200*/)
    {
        if( u32mvu > (u32mvc+u32mvd)/2 )
        {
            u32mevertical = (u32mvu - (u32mvc+u32mvd)/2);
        }
        else
        {
            u32mevertical = 0;
        }
    }
    else if((u32mvd > u32mvc) && (u32mvd > u32mvu) /*&& (u32_mvd > 0x200)*/)
    {
        if( u32mvd > (u32mvc+u32mvu)/2 )
        {
            u32mevertical = (u32mvd - (u32mvc+u32mvu)/2);
        }
        else
        {
            u32mevertical = 0;
        }
    }
    else
    {
        u32mevertical = 0;
    }

    if( u32mevertical > 255 )
    {
        u32mevertical = 255;
    }

    u32mevertical = (u32IsVerticalMovingiir*3 + u32mevertical ) >> 2;
    u32IsVerticalMovingiir = u32mevertical;

    return u32mevertical;
}

MS_U32 MDrv_SC_A5_IsHorizontalMoving(void)
{
    MS_U32 u32mvc, u32mvu, u32mvd;
    MS_U32 u32mehorizontal;
    static MS_U32 u32IsHorizontalMovingiir = 0;

    u32mvc     = MHal_XC_R2BYTE(REG_SC_BK2A_7C_L);
    u32mvu     = MHal_XC_R2BYTE(REG_SC_BK2A_7D_L);
    u32mvd     = MHal_XC_R2BYTE(REG_SC_BK2A_7E_L);

    if ((u32mvc > u32mvu) && (u32mvc > u32mvd) /*&& (u32_mvu > 0x200*/)
    {
        if( u32mvc > (u32mvu+u32mvd)/2 )
        {
            u32mehorizontal = (u32mvc - (u32mvu+u32mvd)/2);
        }
        else
        {
            u32mehorizontal = 0;
        }
    }
    else
    {
        u32mehorizontal = 0;
    }

    if( u32mehorizontal > 255 )
    {
        u32mehorizontal = 255;
    }

    u32mehorizontal = (u32IsHorizontalMovingiir*3 + u32mehorizontal ) >> 2;
    u32IsHorizontalMovingiir = u32mehorizontal;

    return u32mehorizontal;
}


MS_U32 MDrv_SC_A5_ColorPixelSum(void)
{
   MS_U32 u32colorSum;
   static MS_U32 u32colorSum_iir = 0;

   u32colorSum  = MHal_XC_R2BYTE(REG_SC_BK1A_6E_L);
   u32colorSum  = u32colorSum / 8;

   if( u32colorSum > 255 )
   {
    u32colorSum = 255;
   }

   u32colorSum = ((u32colorSum_iir*3 )+ u32colorSum ) >> 2;
   u32colorSum_iir = u32colorSum;

   return u32colorSum_iir;
}


MS_U32 MDrv_SC_A5_TotalFeather(void)
{
    MS_U32 u32mv0feather, u32nonMv0feather;
    MS_U32 u32totalfeather;
    static MS_U32 u32totalfeatheriir = 255;

    u32mv0feather      = MHal_XC_R2BYTE(REG_SC_BK2A_77_L);
    u32nonMv0feather   = MHal_XC_R2BYTE(REG_SC_BK2A_7B_L);

    u32totalfeather = ( u32mv0feather + u32nonMv0feather ) / 4;

    if( u32totalfeather > 255 )
    {
        u32totalfeather = 255;
    }

    u32totalfeather = ((u32totalfeatheriir*3 )+ u32totalfeather ) >> 2;
    u32totalfeatheriir = u32totalfeather;

    return u32totalfeatheriir;
}

MS_U32 MDrv_SC_TotalFeather_Sum(void)
{
    MS_U32 u32mv0feather, u32nonMv0feather;
    MS_U32 u32totalfeather;
    static MS_U32 u32totalfeatheriir = 0;

    u32mv0feather = MHal_XC_R2BYTE(REG_SC_BK2A_77_L);
    u32nonMv0feather = MHal_XC_R2BYTE(REG_SC_BK2A_7B_L);

    u32totalfeather =  u32mv0feather + u32nonMv0feather;

    u32totalfeather = ((u32totalfeatheriir*3 )+ u32totalfeather ) >> 2;
    u32totalfeatheriir = u32totalfeather;

    return u32totalfeatheriir;
}

#define TOTAL_MV_TIME 10
MS_U32 MDrv_SC_A5_TotalMvComfirm(void)
{
    MS_U32 u32mv0, u32nonMv0, u32notFound;
    MS_U32 u32totalmv, u32mvComfirm;
    static MS_U32 u32mvComfirmiir = 0;

    u32mv0      = MHal_XC_R2BYTE(REG_SC_BK2A_78_L);
    u32nonMv0   = MHal_XC_R2BYTE(REG_SC_BK2A_79_L);
    u32notFound   = MHal_XC_R2BYTE(REG_SC_BK2A_7A_L);

    u32totalmv = u32mv0 + u32nonMv0;

    if( u32totalmv > u32notFound*TOTAL_MV_TIME )
    {
        u32mvComfirm =   u32totalmv - u32notFound*TOTAL_MV_TIME;
    }
    else
    {
        u32mvComfirm = 0;
    }

    if( u32mvComfirm > 255 )
    {
        u32mvComfirm = 255;
    }

    u32mvComfirm = ((u32mvComfirmiir*3 )+ u32mvComfirm ) >> 2;
    u32mvComfirmiir = u32mvComfirm;

    return u32mvComfirmiir;
}


MS_U8 MDrv_SC_A5_UC_PanScan(void)
{
    MS_U32 u32nonMV0Found, u32MVNotFound;
    static MS_U32 u32PrevnonMV0Found = 0;
    static MS_U32 u32PrevMVNotFound = 0;
    static MS_U8  u8panCnt = 0;

    u32nonMV0Found    = MHal_XC_R2BYTE(REG_SC_BK2A_79_L);
    u32MVNotFound     = MHal_XC_R2BYTE(REG_SC_BK2A_7A_L);

    u32nonMV0Found = (u32PrevnonMV0Found*3 + u32nonMV0Found ) >> 2;
    u32PrevnonMV0Found = u32nonMV0Found;

    u32MVNotFound = (u32PrevMVNotFound*3 + u32MVNotFound ) >> 2;
    u32PrevMVNotFound = u32MVNotFound;

    if( u32nonMV0Found > u32MVNotFound*30 )
    {
        if(u8panCnt != 10)
        {
            u8panCnt++;
        }
    }
    else if(u8panCnt != 0)
    {
        u8panCnt--;
    }

    return u8panCnt;
}

#define LG107_FEATHER_GAIN_MAX 2
#define LG107_FEATHER_GAIN_MIN 0
#define LG107_CPLX_OFFSET_MAX 6
#define LG107_CPLX_OFFSET_MIN 0
#define LG107_CPLX_GAIN_MAX 15
#define LG107_CPLX_GAIN_MIN 4
void MDrv_SC_A5_UC_PanScanSetting( const MS_U8 PanScan )
{
    static MS_U16 u16regmcdifeathergain = LG107_FEATHER_GAIN_MAX ;
    static MS_U16 u16regmcdicplxoffset = LG107_CPLX_OFFSET_MAX ;

    if( PanScan > 8 )
    {
        u16regmcdifeathergain = LG107_FEATHER_GAIN_MIN;
        u16regmcdicplxoffset = LG107_FEATHER_GAIN_MIN;
    }
    else
    {
        u16regmcdifeathergain = LG107_FEATHER_GAIN_MAX;
        u16regmcdicplxoffset = LG107_FEATHER_GAIN_MAX;
    }

    MHal_XC_W2BYTEMSK(REG_SC_BK2A_2D_L, (MS_U16)u16regmcdifeathergain<<12, 0xF000); // 2 to 0
    MHal_XC_W2BYTEMSK(REG_SC_BK2A_2E_L, (MS_U16)u16regmcdicplxoffset<<4, 0x00F0);// 6 to 0
}

/******************************************************************************/
///MCNR Driver
/******************************************************************************/
/******************************************************************************/
///UCNR Driver from Monaco driver
/******************************************************************************/
MS_BOOL Hal_PQ_LoadMCNRLutQmapSetting()
{
    //UCNR LUT
#if MCNR_LUT_LOAD_FROM_QMAP
    m_stMCNRLutQmapSetting.mcnrLut[0] = MHal_XC_R2BYTE(REG_SC_BK2A_08_L);
    m_stMCNRLutQmapSetting.mcnrLut[1] = MHal_XC_R2BYTE(REG_SC_BK2A_09_L);
    m_stMCNRLutQmapSetting.mcnrLut[2] = MHal_XC_R2BYTE(REG_SC_BK2A_0A_L);
    m_stMCNRLutQmapSetting.mcnrLut[3] = MHal_XC_R2BYTE(REG_SC_BK2A_0B_L);
    m_stMCNRLutQmapSetting.mcnrLut[4] = MHal_XC_R2BYTE(REG_SC_BK2A_0C_L);
    m_stMCNRLutQmapSetting.mcnrLut[5] = MHal_XC_R2BYTE(REG_SC_BK2A_0D_L);
    m_stMCNRLutQmapSetting.mcnrLut[6] = MHal_XC_R2BYTE(REG_SC_BK2A_0E_L);
    m_stMCNRLutQmapSetting.mcnrLut[7] = MHal_XC_R2BYTE(REG_SC_BK2A_0F_L);
#else
    m_stMCNRLutQmapSetting.mcnrLut[0] = 0xFFEE;
    m_stMCNRLutQmapSetting.mcnrLut[1] = 0xDDCC;
    m_stMCNRLutQmapSetting.mcnrLut[2] = 0xBBAA;
    m_stMCNRLutQmapSetting.mcnrLut[3] = 0x9988;
    m_stMCNRLutQmapSetting.mcnrLut[4] = 0x7766;
    m_stMCNRLutQmapSetting.mcnrLut[5] = 0x5544;
    m_stMCNRLutQmapSetting.mcnrLut[6] = 0x3322;
    m_stMCNRLutQmapSetting.mcnrLut[7] = 0x1100;
#endif

    //PDNR LUT
#if PDNR_LUT_LOAD_FROM_QMAP
    m_stMCNRLutQmapSetting.pdnrlowLut[0] = MHal_XC_R2BYTE(REG_SC_BK2A_20_L);
    m_stMCNRLutQmapSetting.pdnrlowLut[1] = MHal_XC_R2BYTE(REG_SC_BK2A_21_L);
    m_stMCNRLutQmapSetting.pdnrlowLut[2] = MHal_XC_R2BYTE(REG_SC_BK2A_22_L);
    m_stMCNRLutQmapSetting.pdnrlowLut[3] = MHal_XC_R2BYTE(REG_SC_BK2A_23_L);
    m_stMCNRLutQmapSetting.pdnrhighLut[0] = MHal_XC_R2BYTE(REG_SC_BK2A_24_L);
    m_stMCNRLutQmapSetting.pdnrhighLut[1] = MHal_XC_R2BYTE(REG_SC_BK2A_25_L);
    m_stMCNRLutQmapSetting.pdnrhighLut[2] = MHal_XC_R2BYTE(REG_SC_BK2A_26_L);
    m_stMCNRLutQmapSetting.pdnrhighLut[3] = MHal_XC_R2BYTE(REG_SC_BK2A_27_L);
#else
    m_stMCNRLutQmapSetting.pdnrlowLut[0] = 0x8888;
    m_stMCNRLutQmapSetting.pdnrlowLut[1] = 0x8888;
    m_stMCNRLutQmapSetting.pdnrlowLut[2] = 0x7654;
    m_stMCNRLutQmapSetting.pdnrlowLut[3] = 0x3210;

    if(_bIsHDSource)
    {
        m_stMCNRLutQmapSetting.pdnrhighLut[0] = 0x8888;
        m_stMCNRLutQmapSetting.pdnrhighLut[1] = 0x8888;
        m_stMCNRLutQmapSetting.pdnrhighLut[2] = 0x8887;
        m_stMCNRLutQmapSetting.pdnrhighLut[3] = 0x6420;
    }
    else
    {
        m_stMCNRLutQmapSetting.pdnrhighLut[0] = 0x8876;
        m_stMCNRLutQmapSetting.pdnrhighLut[1] = 0x5432;
        m_stMCNRLutQmapSetting.pdnrhighLut[2] = 0x1000;
        m_stMCNRLutQmapSetting.pdnrhighLut[3] = 0x0000;
    }
#endif

    return true;
}
MS_U16 MDrv_SC_NR_LUT_Aux(  MS_U16 nr_lut, MS_U8 sc)
{
    sc = sc < 16 ? sc : 16;

    MS_U8 h0 = (nr_lut >>  0) & 0xF;
    MS_U8 h1 = (nr_lut >>  4) & 0xF;
    MS_U8 h2 = (nr_lut >>  8) & 0xF;
    MS_U8 h3 = (nr_lut >> 12) & 0xF;

    XC_ApiStatus DrvStatus;
    MApi_XC_GetStatus(&DrvStatus, MAIN_WINDOW);
    if ((!DrvStatus.bInterlace) && (DrvStatus.u16InputVFreq < 300)) //reduce gain value if 24p
    {
        h0 = ((h0 * sc)/3) >> 4;
        h1 = ((h1 * sc)/3) >> 4;
        h2 = ((h2 * sc)/3) >> 4;
        h3 = ((h3 * sc)/3) >> 4;
    }
    else
    {
        h0 = (h0 * sc) >> 4;
        h1 = (h1 * sc) >> 4;
        h2 = (h2 * sc) >> 4;
        h3 = (h3 * sc) >> 4;
    }

    return (h3 << 12) | (h2 << 8) | (h1 << 4) | (h0 << 0);
}
void MDrv_SC_Set_MCNR_Gain( MS_U8 gain)
{
    MHal_XC_W2BYTE(REG_SC_BK2A_08_L, MDrv_SC_NR_LUT_Aux( m_stMCNRLutQmapSetting.mcnrLut[0], gain ) );
    MHal_XC_W2BYTE(REG_SC_BK2A_09_L, MDrv_SC_NR_LUT_Aux( m_stMCNRLutQmapSetting.mcnrLut[1], gain ) );
    MHal_XC_W2BYTE(REG_SC_BK2A_0A_L, MDrv_SC_NR_LUT_Aux( m_stMCNRLutQmapSetting.mcnrLut[2], gain ) );
    MHal_XC_W2BYTE(REG_SC_BK2A_0B_L, MDrv_SC_NR_LUT_Aux( m_stMCNRLutQmapSetting.mcnrLut[3], gain ) );
    MHal_XC_W2BYTE(REG_SC_BK2A_0C_L, MDrv_SC_NR_LUT_Aux( m_stMCNRLutQmapSetting.mcnrLut[4], gain ) );
    MHal_XC_W2BYTE(REG_SC_BK2A_0D_L, MDrv_SC_NR_LUT_Aux( m_stMCNRLutQmapSetting.mcnrLut[5], gain ) );
    MHal_XC_W2BYTE(REG_SC_BK2A_0E_L, MDrv_SC_NR_LUT_Aux( m_stMCNRLutQmapSetting.mcnrLut[6], gain ) );
    MHal_XC_W2BYTE(REG_SC_BK2A_0F_L, MDrv_SC_NR_LUT_Aux( m_stMCNRLutQmapSetting.mcnrLut[7], gain ) );
}
void MDrv_SC_mcnr_driver( MS_U32 nonHistoryMotionPixel, MS_BOOL bisHDSource)
{
    MS_U32 u32_is_mv0, u32_non_mv0, u32_mvNotFound, u32_mvFeathering, u32_mvc, u32_mvu, u32_mvd, u32_cplxPixel, u32temp1, u32temp2;
    MS_BOOL bmenotconfirm , bmevertical, bis24to60Hz/*, scene_change*/;
    MS_U8 u8MCNR_Gain=16;
    static MS_U8 u8vMOvingReduceNR_HouseHDPatch_counter;
    static MS_U8 u8notConfirmCnt;
    //static MS_U32 u32lastmvNotFound;
    static MS_U8 u8nrWeightPatchForVSweep = 0;
    static MS_U16 au16NR_Lut[8];
    XC_ApiStatus stXCStatus;
    MS_BOOL bMCNR_LUT_Reload = (MHal_XC_R2BYTEMSK(REG_SC_BK22_7E_L, 0x0400)>>8);
    if(MApi_XC_GetStatus(&stXCStatus, MAIN_WINDOW) == FALSE)
    {
        return;
        //printf("MApi_XC_GetStatus failed because of InitData wrong, please update header file and compile again\n");
    }

    // Update MCNR/PDNR LUT from Qmap from Monaco driver

    if(bMCNR_LUT_Reload)
    {
        if(Hal_PQ_LoadMCNRLutQmapSetting())
            MHal_XC_W2BYTEMSK(REG_SC_BK22_7E_L, 0x0000, 0x0400);
    }
    bis24to60Hz = (!stXCStatus.bInterlace) && (stXCStatus.u16InputVFreq < 300);

    u32_is_mv0  = MHal_XC_R2BYTE(REG_SC_BK2A_78_L);
    u32_non_mv0 = MHal_XC_R2BYTE(REG_SC_BK2A_79_L);
    u32_mvNotFound = MHal_XC_R2BYTE(REG_SC_BK2A_7A_L);
    u32_mvFeathering = MHal_XC_R2BYTE(REG_SC_BK2A_7B_L);
    u32_mvc     = MHal_XC_R2BYTE(REG_SC_BK2A_7C_L);
    u32_mvu     = MHal_XC_R2BYTE(REG_SC_BK2A_7D_L);
    u32_mvd     = MHal_XC_R2BYTE(REG_SC_BK2A_7E_L);
    u32_cplxPixel = MHal_XC_R2BYTE(REG_SC_BK2A_7F_L);


    if ( (u32_cplxPixel > 0x1000) && (u32_is_mv0 > 0x1000) && (nonHistoryMotionPixel == 0) ) // patch for VSweep converge too slow
    {
        if ( u8nrWeightPatchForVSweep > 4 )
        {
            u8nrWeightPatchForVSweep = u8nrWeightPatchForVSweep - 4;
        }
        else
        {
            u8nrWeightPatchForVSweep = 0;
        }
    }
    else
    {
        if ( u8nrWeightPatchForVSweep < 0xD )
        {
            u8nrWeightPatchForVSweep += 1;
        }
    }

    /////////////////////////////////////////////////////////////////
    //  NR Patch 1
    /////////////////////////////////////////////////////////////////
    bmenotconfirm = (u32_is_mv0 < 0x0200) && (u32_non_mv0 < 0x0200);
    bmevertical    = ((u32_mvu > u32_mvc) && (u32_mvu > u32_mvd) && (u32_mvu > 0x200)) ||
                     ((u32_mvd > u32_mvc) && (u32_mvd > u32_mvu) && (u32_mvd > 0x200));

    if(false == bmenotconfirm)
    {
        if(u8notConfirmCnt > 0)
        {
            u8notConfirmCnt--;
        }
    }
    else
    {
        if(u8notConfirmCnt <= 0x40)
        {
            u8notConfirmCnt++;
        }
    }

    if(1)
    {
        MS_BOOL bBool = FALSE;

        if (MHal_XC_R2BYTEMSK(REG_SC_BK20_10_L , 0x03) & 0x03)
        {
            bBool = (u8notConfirmCnt > 0x30);//main&sub
        }
        else if (MHal_XC_R2BYTEMSK(REG_SC_BK20_10_L , BIT(1)) & BIT(1))
        {
            bBool = (u8notConfirmCnt > 0x30);//sub only
        }
        else if (MHal_XC_R2BYTEMSK(REG_SC_BK20_10_L , BIT(0)) & BIT(0))
        {
            bBool = ( (u8notConfirmCnt > 0x30) || bmevertical ); // low nr strength
        }

        if (bBool) // low nr strength
        {
            if ( !bisHDSource )
            {
                MHal_XC_W2BYTEMSK(REG_SC_BK2A_1B_L, (MS_U16)0x0064, 0x00FF); // moving picture, increase random motion
            }
            else
            {
                MHal_XC_W2BYTEMSK(REG_SC_BK2A_1B_L, (MS_U16)0x00C2, 0x00FF); // Default 64 -> C2, moving picture, increase random motion
            }

            if ( u8nrWeightPatchForVSweep >= 7 )
            {
                u8MCNR_Gain=9;
            }
            else //if ( nrWeightPatchForVSweep >= 4 )
            {
                u8MCNR_Gain=7;
            }
        }
        else if(u8notConfirmCnt < 0x10)
        {
            if ( (u8nrWeightPatchForVSweep >= 0xD) && (!bis24to60Hz) )
            {
                u8MCNR_Gain=16;
            }
            else if ( (u8nrWeightPatchForVSweep >= 0xA) && (!bis24to60Hz) )
            {
                u8MCNR_Gain=11;
            }
            else if ( u8nrWeightPatchForVSweep >= 7 )
            {
                u8MCNR_Gain=9;
            }
            else //if ( nrWeightPatchForVSweep >= 4 )
            {
                u8MCNR_Gain=7;
            }
        }
        else
        {
            u8MCNR_Gain=16;
        }
        // MCNR gain
        MDrv_SC_Set_MCNR_Gain( u8MCNR_Gain);
/*        static MS_U16 au16NR_Lut[8];
        au16NR_Lut[0]=0xFFEE;
        au16NR_Lut[1]=0xDDCC;
        au16NR_Lut[2]=0xBBAA;
        au16NR_Lut[3]=0x9988;
        au16NR_Lut[4]=0x7766;
        au16NR_Lut[5]=0x5544;
        au16NR_Lut[6]=0x3322;
        au16NR_Lut[7]=0x1100;

        MApi_XC_W2BYTEMSK(REG_SC_BK2A_08_L, MDrv_SC_NR_LUT_Aux(au16NR_Lut[0],u8MCNR_Gain), 0xFFFF);
        MApi_XC_W2BYTEMSK(REG_SC_BK2A_09_L, MDrv_SC_NR_LUT_Aux(au16NR_Lut[1],u8MCNR_Gain), 0xFFFF);
        MApi_XC_W2BYTEMSK(REG_SC_BK2A_0A_L, MDrv_SC_NR_LUT_Aux(au16NR_Lut[2],u8MCNR_Gain), 0xFFFF);
        MApi_XC_W2BYTEMSK(REG_SC_BK2A_0B_L, MDrv_SC_NR_LUT_Aux(au16NR_Lut[3],u8MCNR_Gain), 0xFFFF);
        MApi_XC_W2BYTEMSK(REG_SC_BK2A_0C_L, MDrv_SC_NR_LUT_Aux(au16NR_Lut[4],u8MCNR_Gain), 0xFFFF);
        MApi_XC_W2BYTEMSK(REG_SC_BK2A_0D_L, MDrv_SC_NR_LUT_Aux(au16NR_Lut[5],u8MCNR_Gain), 0xFFFF);
        MApi_XC_W2BYTEMSK(REG_SC_BK2A_0E_L, MDrv_SC_NR_LUT_Aux(au16NR_Lut[6],u8MCNR_Gain), 0xFFFF);
        MApi_XC_W2BYTEMSK(REG_SC_BK2A_0F_L, MDrv_SC_NR_LUT_Aux(au16NR_Lut[7],u8MCNR_Gain), 0xFFFF);
*/


        if ( bisHDSource && (u32_mvc < 0x10) && (( u32_mvu + u32_mvd ) > 0x40) )
        {
            u8vMOvingReduceNR_HouseHDPatch_counter = 16;
        }
        else if ( (u32_is_mv0 <= 0x10) && (u32_non_mv0 > 0x2000) && (u32_mvNotFound > 0x80) && (u32_mvFeathering > 0x8) && (u32_cplxPixel < 0x200) ) // patch shibasoku a little out-of-search range
        {
            u8vMOvingReduceNR_HouseHDPatch_counter = 16;
        }
        else if ( u8vMOvingReduceNR_HouseHDPatch_counter > 0 )
        {
            u8vMOvingReduceNR_HouseHDPatch_counter --;
        }

        if ( bis24to60Hz )
        {
            MHal_XC_W2BYTEMSK(REG_SC_BK2A_1B_L, (MS_U16)0x00F0, 0x00FF);
        }
        else if ( (u32_is_mv0 > 0x200) && ((u32_is_mv0/2) > u32_non_mv0) && ((u32_is_mv0/2) > u32_mvNotFound) && (u8vMOvingReduceNR_HouseHDPatch_counter == 0) )
        {
            if ( stXCStatus.bInterlace )
            {
                MHal_XC_W2BYTEMSK(REG_SC_BK2A_1B_L, (MS_U16)0x006c, 0x00FF); // still picture, reduce random motion
            }
            else
            {
                MHal_XC_W2BYTEMSK(REG_SC_BK2A_1B_L, (MS_U16)0x0064, 0x00FF); // still picture, reduce random motion
            }
        }
        else if ( (u32_non_mv0 > u32_is_mv0) || (u8vMOvingReduceNR_HouseHDPatch_counter > 0 )) // moving picture, increase random motion
        {
            if ( !bisHDSource )
        {
                MHal_XC_W2BYTEMSK(REG_SC_BK2A_1B_L, (MS_U16)0x0060, 0x00FF);
            }
            else
            {
                if ( stXCStatus.bInterlace )
                {
                    u32temp1 = ( 0xE * u8vMOvingReduceNR_HouseHDPatch_counter + 0x6 * ( 16 - u8vMOvingReduceNR_HouseHDPatch_counter ) ) >> 4;
                    u32temp2 = ( 0x0 * u8vMOvingReduceNR_HouseHDPatch_counter + 0xC * ( 16 - u8vMOvingReduceNR_HouseHDPatch_counter ) ) >> 4;
                }
                else
                {
                    u32temp1 = ( 0xE * u8vMOvingReduceNR_HouseHDPatch_counter + 0x6 * ( 16 - u8vMOvingReduceNR_HouseHDPatch_counter ) ) >> 4;
                    u32temp2 = ( 0x0 * u8vMOvingReduceNR_HouseHDPatch_counter + 0x4 * ( 16 - u8vMOvingReduceNR_HouseHDPatch_counter ) ) >> 4;
                }
                MHal_XC_W2BYTEMSK(REG_SC_BK2A_1B_L, (MS_U16)((u32temp1<<4)|u32temp2), 0x00FF); // 60 -> E0. moving picture, increase random motion
            }
        }
        else // default
        {
            if ( !bisHDSource )
            {
                MHal_XC_W2BYTEMSK(REG_SC_BK2A_1B_L, (MS_U16)0x0064, 0x00FF); // default random motion
            }
            else
            {
                if ( stXCStatus.bInterlace )
                {
                    MHal_XC_W2BYTEMSK(REG_SC_BK2A_1B_L, (MS_U16)0x00C2, 0x00FF); // Default 64 -> C2, moving picture, increase random motion
                }
                else
                {
                    MHal_XC_W2BYTEMSK(REG_SC_BK2A_1B_L, (MS_U16)0x00C2, 0x00FF); // Default 64 -> C2, moving picture, increase random motion
                }
        }
    }
    }
    //u32lastmvNotFound = u32_mvNotFound;
}

/******************************************************************************/
///DeFeathering
///@param u32MotionValue \     IN: Motion value
/******************************************************************************/
void MDrv_SC_de_feathering(MS_U32 u32MotionValue)
{
    static MS_U32 u32DeFeatherCntLv1 = 0;
    static MS_U32 u32DeFeatherCntLv2 = 0;
    static MS_U32 u32DeFeatherCntLv3 = 0;
    MS_U8 u8SST_Static_Core_TH;
    MS_U32 u32regdefetheringlv1cnt;
    MS_U32 u32regdefetheringlv2cnt;
    MS_U32 u32regmdpcnt;

    u32regdefetheringlv1cnt = 0x0A;
    u32regdefetheringlv2cnt = 0x08;
    u32regmdpcnt = 0x01;

    // motion level count
    if(u32MotionValue >= DEFETHERING_LV1_TH)
    {
        if(u32DeFeatherCntLv1 < u32regdefetheringlv1cnt)
        {
            u32DeFeatherCntLv1++;
        }
    }
    else
    {
        if(u32DeFeatherCntLv1 >= u32regmdpcnt)
        {
            u32DeFeatherCntLv1 = u32DeFeatherCntLv1 - u32regmdpcnt;
        }
    }

    if(u32MotionValue >= DEFETHERING_LV2_TH)
    {
        if(u32DeFeatherCntLv2 < u32regdefetheringlv2cnt)
        {
            u32DeFeatherCntLv2++;
        }
    }
    else
    {
        if(u32DeFeatherCntLv2 >= u32regmdpcnt)
        {
            u32DeFeatherCntLv2 = u32DeFeatherCntLv2 - u32regmdpcnt;
        }
    }

    if(u32MotionValue >= DEFETHERING_LV3_TH)
    {
        if(u32DeFeatherCntLv3 < DEFETHERING_LV3_CNT)
        {
            u32DeFeatherCntLv3++;
        }
    }
    else
    {
        if(u32DeFeatherCntLv3 >= u32regmdpcnt)
        {
            u32DeFeatherCntLv3 = u32DeFeatherCntLv3 - u32regmdpcnt;
        }
    }

    //DeFeathering begin
    if(u32DeFeatherCntLv1 >= u32regdefetheringlv1cnt)
    {
        u8SST_Static_Core_TH    = SST_STATIC_CORE_TH_LV1_VALUE;
    }
    else if(u32DeFeatherCntLv2 >= u32regdefetheringlv2cnt)
    {
        u8SST_Static_Core_TH    = SST_STATIC_CORE_TH_LV2_VALUE;
    }
    else if(u32DeFeatherCntLv3 >= DEFETHERING_LV3_CNT)
    {
        u8SST_Static_Core_TH    = SST_STATIC_CORE_TH_LV3_VALUE;
    }
    else
    {
        u8SST_Static_Core_TH    = SST_STATIC_CORE_TH_LV4_VALUE;
    }

    MHal_XC_W2BYTEMSK(REG_SC_BK22_1A_L, (MS_U16)u8SST_Static_Core_TH, 0xFF);
}


int MDrv_SC_motion_level(MS_U32 u32MotionValue)
{
    static MS_U32 u32DeFeatherCntLv1 = 0;
    static MS_U32 u32DeFeatherCntLv2 = 0;
    static MS_U32 u32DeFeatherCntLv3 = 0;
    MS_U8 u8MotLvl;
    MS_U32 u32regdefetheringlv1cnt;
    MS_U32 u32regdefetheringlv2cnt;
    MS_U32 u32regmdpcnt;

    u32regdefetheringlv1cnt = 0x0A;
    u32regdefetheringlv2cnt = 0x08;
    u32regmdpcnt = 0x01;

    // motion level count
    if(u32MotionValue >= DEFETHERING_LV1_TH)
    {
        if(u32DeFeatherCntLv1 < u32regdefetheringlv1cnt)
        {
            u32DeFeatherCntLv1++;
        }
    }
    else
    {
        if(u32DeFeatherCntLv1 >= u32regmdpcnt)
        {
            u32DeFeatherCntLv1 = u32DeFeatherCntLv1 - u32regmdpcnt;
        }
    }

    if(u32MotionValue >= DEFETHERING_LV2_TH)
    {
        if(u32DeFeatherCntLv2 < u32regdefetheringlv2cnt)
        {
            u32DeFeatherCntLv2++;
        }
    }
    else
    {
        if(u32DeFeatherCntLv2 >= u32regmdpcnt)
        {
            u32DeFeatherCntLv2 = u32DeFeatherCntLv2 - u32regmdpcnt;
        }
    }

    if(u32MotionValue >= DEFETHERING_LV3_TH)
    {
        if(u32DeFeatherCntLv3 < DEFETHERING_LV3_CNT)
        {
            u32DeFeatherCntLv3++;
        }
    }
    else
    {
        if(u32DeFeatherCntLv3 >= u32regmdpcnt)
        {
            u32DeFeatherCntLv3 = u32DeFeatherCntLv3 - u32regmdpcnt;
        }
    }

    //level begin
    if(u32DeFeatherCntLv1 >= u32regdefetheringlv1cnt)
    {
        u8MotLvl    = 3;
    }
    else if(u32DeFeatherCntLv2 >= u32regdefetheringlv2cnt)
    {
        u8MotLvl    = 2;
    }
    else if(u32DeFeatherCntLv3 >= DEFETHERING_LV3_CNT)
    {
        u8MotLvl    = 1;
    }
    else
    {
        u8MotLvl    = 0;
    }

    return u8MotLvl;
}


/******************************************************************************/
///DeFlickering
///@param u32MotionValue \     IN: Motion value
/******************************************************************************/
void MDrv_SC_de_flickering(MS_U32 u32MotionValue)
{
    static MS_S32 s32DeFlickerCnt = 0;
    MS_U32 u32regmfeatsmoothhleth, u32regmfeatsmoothshrink;

    u32regmfeatsmoothhleth = (MS_U32)MHal_XC_R2BYTEMSK(REG_SC_BK22_1E_L, 0xF000); // Feat Smooth HLE TH
    u32regmfeatsmoothshrink = (MS_U32)MHal_XC_R2BYTEMSK(REG_SC_BK22_1E_L, 0x8F);

    if(u32MotionValue >= DEFLICKERING_TH)
    {
        if(s32DeFlickerCnt < 65535)
        {
            s32DeFlickerCnt++;
        }
    }
    else
    {
        s32DeFlickerCnt = 0;
    }

    if(s32DeFlickerCnt >= DEFLICKERING_CNT)
    {
        u32regmfeatsmoothhleth += 0x0300;
        u32regmfeatsmoothshrink += 0x10;
    }
    else
    {
        u32regmfeatsmoothhleth += 0x0700;
        u32regmfeatsmoothshrink += 0x30;
    }

    MHal_XC_W2BYTEMSK(REG_SC_BK22_1E_L, u32regmfeatsmoothshrink, 0xFF);
    MHal_XC_W2BYTEMSK(REG_SC_BK22_1E_L, u32regmfeatsmoothhleth, 0xFF00);
}

/******************************************************************************/
///DeBouncing
///@param u32MotionValue \     IN: Motion value
/******************************************************************************/
#define DEBOUNCING_GAIN 1 //0
void MDrv_SC_de_bouncing(MS_U32 u32MotionValue)
{
    static MS_S32 s32DeBouncingCnt = 0;
    MS_U32 u32reghiswtf2;
    MS_U32 u32regdebouncingth;
    MS_U32 u32regdebouncingcnt;
    MS_BOOL bfilm;

    u32regdebouncingth = 0x0A;
    u32regdebouncingcnt = 0x03;

    u32reghiswtf2 = (MS_U32)MHal_XC_R2BYTEMSK(REG_SC_BK22_0A_L, 0xF8); // history ratio weighting


    bfilm = ( (MS_U16)MHal_XC_R2BYTEMSK(REG_SC_BK0A_21_L, 0x0008) == 0x0008) ||  // film22
           ( (MS_U16)MHal_XC_R2BYTEMSK(REG_SC_BK0A_21_L, 0x0010) == 0x0010) ||  // film32
           ( (MS_U16)MHal_XC_R2BYTEMSK(REG_SC_BK0A_21_L, 0x0040) == 0x0040) ;   // film any

    if(u32MotionValue <= u32regdebouncingth * DEBOUNCING_GAIN)
    {
        s32DeBouncingCnt = 0;
    }
    else
    {
        if(s32DeBouncingCnt < 65535)
        {
            s32DeBouncingCnt++;
        }
    }

    if(bfilm || (s32DeBouncingCnt >= (MS_S32)u32regdebouncingcnt))
    {
        u32reghiswtf2 += 0x06; // history = 6 moving
    }
    else
    {
        u32reghiswtf2 += 0x03; // history = 3 still
    }
    MHal_XC_W2BYTEMSK(REG_SC_BK22_0A_L, u32reghiswtf2, 0xFF);
}

#define HD_ACT_TH 10
#define HDSDD_SPEED 1
#define HDSDD_DIV 0
#define HD_SPEED 2
#define SD_SPEED 8

void MDrv_SC_NEW_SDHD_DETECT_driver(void)
{
    MS_U8 u8Value,u8FEATHERCNTTH;
    MS_U16 u16MinPixel,u16MaxPixel, u16MaxMinPixel;
    MS_U32 u32HDcntAct;
    //MS_U32 HDcnt_NonAct;
    MS_U32 u32FeatherCnt;
    MS_U32 u32SDcntAct;
    //MS_U32 SDcnt_NonAct;
    static MS_U32 u32PreFeatherCnt = 0;
    static MS_U16 u16HDConfirmCnt = 0x0000; // 0x7FFF == 0
    static MS_U8 u8PrevLuma = 0;
    static MS_U16 u16PrevMinPixel = 0;
    static MS_U16 u16PrevMaxPixel = 0;
    MS_U32 u32HDConfirmCntReg;
    MS_U16 u16HDSDDthreshold,u16HDSDDoffset;

    u16HDSDDoffset = 0x30;  //(MS_U16)MHal_XC_R2BYTEMSK(REG_SC_BK22_7F_L, 0xFFFF);

    u32FeatherCnt = MHal_XC_R2BYTE(REG_SC_BK23_56_L);       //New add for A1 chip
    u8FEATHERCNTTH = 0xC8;  //= MHal_XC_R2BYTEMSK(REG_SC_BK22_7D_L, 0xFF00)>>8;

    u32FeatherCnt = ( u32PreFeatherCnt*3 + u32FeatherCnt ) >> 2;
    u32PreFeatherCnt = u32FeatherCnt;

    u8Value=(MS_U32)((MS_U32)u8PrevLuma*3 + MDrv_SC_GetAverageLuminous())>>2;
    u8PrevLuma = u8Value;

    u16MaxPixel = MHal_XC_R2BYTEMSK(REG_SC_BK1A_0B_L, 0x00FF);
    u16MaxPixel = (u16PrevMaxPixel*3 + u16MaxPixel ) >> 2;
    u16PrevMaxPixel = u16MaxPixel;

    u16MinPixel = MHal_XC_R2BYTEMSK(REG_SC_BK1A_0B_L, 0xFF00)>>8;
    u16MinPixel = (u16PrevMinPixel*3 + u16MinPixel ) >> 2;
    u16PrevMinPixel = u16MinPixel;

    u16MaxMinPixel = u16MaxPixel - u16MinPixel;

    if(u8Value>=0xFF)
    {
        u8Value = 0xFF;
    }

    u16HDSDDthreshold = u16HDSDDoffset +((u16MaxMinPixel*6)>>2) + ((u8Value*5)>>2);

    if(u16HDSDDthreshold >=0xFFFF)
    {
        u16HDSDDthreshold=0xFFFF;
    }
    MHal_XC_W2BYTEMSK(REG_SC_BK02_71_L, (MS_U16)u16HDSDDthreshold, 0xFFFF);
    MHal_XC_W2BYTEMSK(REG_SC_BK02_72_L, (MS_U16)(u16HDSDDthreshold>>1), 0xFFFF);

    u32HDcntAct = (MHal_XC_R2BYTE(REG_SC_BK02_75_L) << 16) + MHal_XC_R2BYTE(REG_SC_BK02_74_L);
    u32SDcntAct = (MHal_XC_R2BYTE(REG_SC_BK02_77_L) << 16) + MHal_XC_R2BYTE(REG_SC_BK02_76_L);

    if( (u32HDcntAct > HD_ACT_TH) && (u32SDcntAct != 0) )
    {
        if(u16HDConfirmCnt < HD_SPEED)
        {
            u16HDConfirmCnt = 0;
        }
        else
        {
            u16HDConfirmCnt = u16HDConfirmCnt - HD_SPEED;
        }
    }
    else if( (u32SDcntAct != 0) && (u32FeatherCnt > u8FEATHERCNTTH) )//&& HDcnt_NonAct !=0) // && (HDcnt_NonAct < 0x90000))
    {
        if(u16HDConfirmCnt > (0xFF - SD_SPEED))
        {
            u16HDConfirmCnt = 0xFF;
        }
        else
        {
            u16HDConfirmCnt = u16HDConfirmCnt + SD_SPEED;
        }
    }
    else
    {
        if(u16HDConfirmCnt < 1)
        {
            u16HDConfirmCnt = 0;
        }
        else
        {
            u16HDConfirmCnt = u16HDConfirmCnt - 1;
        }
    }

    if( u16HDConfirmCnt > 0x80 )
    {
        u32HDConfirmCntReg = u16HDConfirmCnt - 0x80;
    }
    else
    {
        u32HDConfirmCntReg = 0;
    }

    u32HDConfirmCntReg = (u32HDConfirmCntReg*HDSDD_SPEED) >> HDSDD_DIV;

    if( u32HDConfirmCntReg > 0x3F )
    {
        u32HDConfirmCntReg = 0x3F;
    }
    MHal_XC_W2BYTEMSK(REG_SC_BK23_54_L, (MS_U16)u32HDConfirmCntReg, 0x003F);
}

void MDrv_SC_A5_verticalMovingReduceKFC( MS_BOOL bVerticalMovingForDHD )
{
    // reduce KFC reduce history motion when vertical moving
    if ( bVerticalMovingForDHD || ((MS_U16)MHal_XC_R2BYTEMSK(REG_SC_BK22_7C_L, 0x2000) == 0x2000) )
    {
        MHal_XC_W2BYTEMSK(REG_SC_BK2A_28_L, 0x0000, 0x0800);
    }
    else
    {
        MHal_XC_W2BYTEMSK(REG_SC_BK2A_28_L, 0x0800, 0x0800);
    }
}

void MDrv_SC_Amber5_DHD_driver1( MS_U8 u8SD2HDValue )
{
    // dhd news report patch: enhance dhd strength
    static MS_U32 u32sd2hdCnt = 0;
    MS_U32 u32checkwidth = MHal_XC_R2BYTEMSK(REG_SC_BK12_0F_L, 0xFFFF);
    MS_U32 u32isPAL = (MHal_XC_R2BYTE(REG_SC_BK05_12_L)&0x4000)>>14;

    MS_U8 u8MV0Comfirm = MHal_XC_R2BYTE(REG_SC_BK2A_78_L)>>8;

    if((u8SD2HDValue>=60)&&(u32checkwidth>0x700)&&(u8MV0Comfirm>0x20)&&(u32isPAL==0)) //When in HD case, detect SD
    {
        u32sd2hdCnt++;

        if( u32sd2hdCnt >= 100 )
        {
            u32sd2hdCnt = 100;
        }
    }
    else
    {
        if( u32sd2hdCnt == 0 )
        {
            u32sd2hdCnt = 0;
        }
        else
        {
            u32sd2hdCnt--;
        }
    }

    // divide into 11 level
    if( u32sd2hdCnt <= 2 )
    {
        MHal_XC_W2BYTEMSK(REG_SC_BK05_26_L, 0x0030, 0x00FF); //c edge gain
        MHal_XC_W2BYTEMSK(REG_SC_BK05_26_L, 0x1000, 0xFF00); //y edge gain
        MHal_XC_W2BYTEMSK(REG_SC_BK05_23_L, 0x0010, 0x00FF); //c value gain
    }
    else if( u32sd2hdCnt == 3 )
    {
        MHal_XC_W2BYTEMSK(REG_SC_BK05_26_L, 0x0030, 0x00FF); //c edge gain
        MHal_XC_W2BYTEMSK(REG_SC_BK05_26_L, 0x2000, 0xFF00); //y edge gain
        MHal_XC_W2BYTEMSK(REG_SC_BK05_23_L, 0x0020, 0x00FF); //c value gain
    }
    else if( u32sd2hdCnt == 4 )
    {
        MHal_XC_W2BYTEMSK(REG_SC_BK05_26_L, 0x0030, 0x00FF); //c edge gain
        MHal_XC_W2BYTEMSK(REG_SC_BK05_26_L, 0x3000, 0xFF00); //y edge gain
        MHal_XC_W2BYTEMSK(REG_SC_BK05_23_L, 0x0020, 0x00FF); //c value gain
    }
    else if( u32sd2hdCnt == 5 )
    {
        MHal_XC_W2BYTEMSK(REG_SC_BK05_26_L, 0x0040, 0x00FF); //c edge gain
        MHal_XC_W2BYTEMSK(REG_SC_BK05_26_L, 0x4000, 0xFF00); //y edge gain
        MHal_XC_W2BYTEMSK(REG_SC_BK05_23_L, 0x0030, 0x00FF); //c value gain
    }
    else if( u32sd2hdCnt == 6 )
    {
        MHal_XC_W2BYTEMSK(REG_SC_BK05_26_L, 0x0050, 0x00FF); //c edge gain
        MHal_XC_W2BYTEMSK(REG_SC_BK05_26_L, 0x5000, 0xFF00); //y edge gain
        MHal_XC_W2BYTEMSK(REG_SC_BK05_23_L, 0x0040, 0x00FF); //c value gain
    }
    else if( u32sd2hdCnt == 7 )
    {
        MHal_XC_W2BYTEMSK(REG_SC_BK05_26_L, 0x0060, 0x00FF); //c edge gain
        MHal_XC_W2BYTEMSK(REG_SC_BK05_26_L, 0x6000, 0xFF00); //y edge gain
        MHal_XC_W2BYTEMSK(REG_SC_BK05_23_L, 0x0050, 0x00FF); //c value gain
    }
    else if( u32sd2hdCnt == 8 )
    {
        MHal_XC_W2BYTEMSK(REG_SC_BK05_26_L, 0x0070, 0x00FF); //c edge gain
        MHal_XC_W2BYTEMSK(REG_SC_BK05_26_L, 0x7000, 0xFF00); //y edge gain
        MHal_XC_W2BYTEMSK(REG_SC_BK05_23_L, 0x0060, 0x00FF); //c value gain
    }
    else if( u32sd2hdCnt == 9 )
    {
        MHal_XC_W2BYTEMSK(REG_SC_BK05_26_L, 0x0080, 0x00FF); //c edge gain
        MHal_XC_W2BYTEMSK(REG_SC_BK05_26_L, 0x8000, 0xFF00); //y edge gain
        MHal_XC_W2BYTEMSK(REG_SC_BK05_23_L, 0x0070, 0x00FF); //c value gain
    }
    else if( u32sd2hdCnt == 10 )
    {
        MHal_XC_W2BYTEMSK(REG_SC_BK05_26_L, 0x0090, 0x00FF); //c edge gain
        MHal_XC_W2BYTEMSK(REG_SC_BK05_26_L, 0x9000, 0xFF00); //y edge gain
        MHal_XC_W2BYTEMSK(REG_SC_BK05_23_L, 0x0080, 0x00FF); //c value gain
    }
    else if( u32sd2hdCnt == 11 )
    {
        MHal_XC_W2BYTEMSK(REG_SC_BK05_26_L, 0x00A0, 0x00FF); //c edge gain
        MHal_XC_W2BYTEMSK(REG_SC_BK05_26_L, 0xA000, 0xFF00); //y edge gain
        MHal_XC_W2BYTEMSK(REG_SC_BK05_23_L, 0x0090, 0x00FF); //c value gain
    }
    else if( u32sd2hdCnt >= 12 )
    {
        MHal_XC_W2BYTEMSK(REG_SC_BK05_26_L, 0x00B0, 0x00FF); //c edge gain
        MHal_XC_W2BYTEMSK(REG_SC_BK05_26_L, 0xB000, 0xFF00); //y edge gain
        MHal_XC_W2BYTEMSK(REG_SC_BK05_23_L, 0x00A0, 0x00FF); //c value gain
    }
}

void MDrv_SC_Amber5_DHD_driver2( MS_U32 u32uComplex, MS_U32 u32IsHorizontalMoving, MS_U8 u8SD2HDValue, MS_BOOL bVerticalMovingForDHD )
{
    // dhd stadium patch: lower dhd strength
    static MS_U32 u32filmCnt = 0;
    MS_U32 u32checkwidth = MHal_XC_R2BYTEMSK(REG_SC_BK12_0F_L, 0xFFFF);
    UNUSED(u32IsHorizontalMoving);

    if (((u8SD2HDValue<10) && (u32uComplex>200)
        && (u32checkwidth>0x700)) || (bVerticalMovingForDHD && (u32checkwidth>0x700) )
        || ( (u8SD2HDValue<5) && (u32checkwidth>0x700)))
    {
        u32filmCnt++;

        if( u32filmCnt >= 50 )
        {
            u32filmCnt = 50;
        }
    }
    else
    {
        if( u32filmCnt == 0 )
        {
            u32filmCnt = 0;
        }
        else
        {
            u32filmCnt--;
        }
    }

    if( u32filmCnt <= 2 )
        MHal_XC_W2BYTEMSK(REG_SC_BK05_2C_L, 0x000F, 0x000F); //dhd user weight
    else if( u32filmCnt == 3 )
        MHal_XC_W2BYTEMSK(REG_SC_BK05_2C_L, 0x000D, 0x000F); //dhd user weight
    else if( u32filmCnt == 4 )
        MHal_XC_W2BYTEMSK(REG_SC_BK05_2C_L, 0x000B, 0x000F); //dhd user weight
    else if( u32filmCnt == 5 )
        MHal_XC_W2BYTEMSK(REG_SC_BK05_2C_L, 0x0009, 0x000F); //dhd user weight
    else if( u32filmCnt == 6 )
        MHal_XC_W2BYTEMSK(REG_SC_BK05_2C_L, 0x0007, 0x000F); //dhd user weight
    else if( u32filmCnt == 7 )
        MHal_XC_W2BYTEMSK(REG_SC_BK05_2C_L, 0x0005, 0x000F); //dhd user weight
    else if( u32filmCnt == 8 )
        MHal_XC_W2BYTEMSK(REG_SC_BK05_2C_L, 0x0003, 0x000F); //dhd user weight
    else if( u32filmCnt == 9 )
        MHal_XC_W2BYTEMSK(REG_SC_BK05_2C_L, 0x0001, 0x000F); //dhd user weight
    else if( u32filmCnt >= 10 )
        MHal_XC_W2BYTEMSK(REG_SC_BK05_2C_L, 0x0000, 0x000F); //dhd user weight
}

/******************************************************************************/
///CCS Driver
/******************************************************************************/

void MDrv_SC_Amber5_CCS_driver(MS_U32 u32uComplex)
{
    // ccs moving color dot pattern patch: lower ccs strength
    static MS_U32 u32videoCnt = 0;

    if( u32uComplex < 16 )
    {
        u32videoCnt++;

        if( u32videoCnt >= 200 )
        {
            u32videoCnt = 200;
        }
    }
    else
    {
        if( u32videoCnt == 0 )
        {
            u32videoCnt = 0;
        }
        else
        {
            u32videoCnt--;
        }
    }

    if( u32videoCnt <= 2 )
        MHal_XC_W2BYTEMSK(REG_SC_BK05_5C_L, 0x000F, 0x000F); //ccs user weight
    else if( u32videoCnt == 3 )
        MHal_XC_W2BYTEMSK(REG_SC_BK05_5C_L, 0x000D, 0x000F); //ccs user weight
    else if( u32videoCnt == 4 )
        MHal_XC_W2BYTEMSK(REG_SC_BK05_5C_L, 0x000B, 0x000F); //ccs user weight
    else if( u32videoCnt == 5 )
        MHal_XC_W2BYTEMSK(REG_SC_BK05_5C_L, 0x0009, 0x000F); //ccs user weight
    else if( u32videoCnt == 6 )
        MHal_XC_W2BYTEMSK(REG_SC_BK05_5C_L, 0x0007, 0x000F); //ccs user weight
    else if( u32videoCnt == 7 )
        MHal_XC_W2BYTEMSK(REG_SC_BK05_5C_L, 0x0005, 0x000F); //ccs user weight
    else if( u32videoCnt == 8 )
        MHal_XC_W2BYTEMSK(REG_SC_BK05_5C_L, 0x0003, 0x000F); //ccs user weight
    else if( u32videoCnt == 9 )
        MHal_XC_W2BYTEMSK(REG_SC_BK05_5C_L, 0x0001, 0x000F); //ccs user weight
    else if( u32videoCnt >= 10 )
        MHal_XC_W2BYTEMSK(REG_SC_BK05_5C_L, 0x0000, 0x000F); //ccs user weight
}

void MDrv_SC_Amber5_CCS_driver2(MS_U32 u32OOSRMC, MS_U32 u32IsHorizontalMoving)
{
    // horizontal moving => lower ccs weight
    static MS_U32 u32videoCnt = 0;

    if( (u32OOSRMC < 20) || (u32IsHorizontalMoving >25) )
    {
        u32videoCnt++;

        if( u32videoCnt >= 200 )
        {
            u32videoCnt = 200;
        }
    }
    else
    {
        if( u32videoCnt == 0 )
        {
            u32videoCnt = 0;
        }
        else
        {
            u32videoCnt--;
        }
    }

    if( u32videoCnt <= 2 )
        MHal_XC_W2BYTEMSK(REG_SC_BK05_5C_L, 0x000F, 0x000F); //ccs user weight
    else if( u32videoCnt == 3 )
        MHal_XC_W2BYTEMSK(REG_SC_BK05_5C_L, 0x000D, 0x000F); //ccs user weight
    else if( u32videoCnt == 4 )
        MHal_XC_W2BYTEMSK(REG_SC_BK05_5C_L, 0x000B, 0x000F); //ccs user weight
    else if( u32videoCnt == 5 )
        MHal_XC_W2BYTEMSK(REG_SC_BK05_5C_L, 0x0009, 0x000F); //ccs user weight
    else if( u32videoCnt == 6 )
        MHal_XC_W2BYTEMSK(REG_SC_BK05_5C_L, 0x0007, 0x000F); //ccs user weight
    else if( u32videoCnt == 7 )
        MHal_XC_W2BYTEMSK(REG_SC_BK05_5C_L, 0x0005, 0x000F); //ccs user weight
    else if( u32videoCnt == 8 )
        MHal_XC_W2BYTEMSK(REG_SC_BK05_5C_L, 0x0003, 0x000F); //ccs user weight
    else if( u32videoCnt == 9 )
        MHal_XC_W2BYTEMSK(REG_SC_BK05_5C_L, 0x0001, 0x000F); //ccs user weight
    else if( u32videoCnt >= 10 )
        MHal_XC_W2BYTEMSK(REG_SC_BK05_5C_L, 0x0000, 0x000F); //ccs user weight
}
XC_LumaHistogramStates MDrv_SC_GetLumaHistogram_PQ(void)
{
    //all registers move to UFSC BK55 in Maserati
    XC_LumaHistogramStates lumaStatus;
    int i = 0;
    MS_U8 u8HistogramPosition = 0;// 0:SC0 1:FSC 2:IP
    if(u8HistogramPosition == 0)
    {
        lumaStatus.lumaBin[0] =   MHal_XC_R2BYTEMSK( REG_SC_BK1A_40_L , 0xFFFF);
        lumaStatus.lumaBin[1] =   MHal_XC_R2BYTEMSK( REG_SC_BK1A_41_L , 0xFFFF);
        lumaStatus.lumaBin[2] =   MHal_XC_R2BYTEMSK( REG_SC_BK1A_42_L , 0xFFFF);
        lumaStatus.lumaBin[3] =   MHal_XC_R2BYTEMSK( REG_SC_BK1A_43_L , 0xFFFF);
        lumaStatus.lumaBin[4] =   MHal_XC_R2BYTEMSK( REG_SC_BK1A_44_L , 0xFFFF);
        lumaStatus.lumaBin[5] =   MHal_XC_R2BYTEMSK( REG_SC_BK1A_45_L , 0xFFFF);
        lumaStatus.lumaBin[6] =   MHal_XC_R2BYTEMSK( REG_SC_BK1A_46_L , 0xFFFF);
        lumaStatus.lumaBin[7] =   MHal_XC_R2BYTEMSK( REG_SC_BK1A_47_L , 0xFFFF);
        lumaStatus.lumaBin[8] =   MHal_XC_R2BYTEMSK( REG_SC_BK1A_48_L , 0xFFFF);
        lumaStatus.lumaBin[9] =   MHal_XC_R2BYTEMSK( REG_SC_BK1A_49_L , 0xFFFF);
        lumaStatus.lumaBin[10] =  MHal_XC_R2BYTEMSK( REG_SC_BK1A_4A_L , 0xFFFF);
        lumaStatus.lumaBin[11] =  MHal_XC_R2BYTEMSK( REG_SC_BK1A_4B_L , 0xFFFF);
        lumaStatus.lumaBin[12] =  MHal_XC_R2BYTEMSK( REG_SC_BK1A_4C_L , 0xFFFF);
        lumaStatus.lumaBin[13] =  MHal_XC_R2BYTEMSK( REG_SC_BK1A_4D_L , 0xFFFF);
        lumaStatus.lumaBin[14] =  MHal_XC_R2BYTEMSK( REG_SC_BK1A_4E_L , 0xFFFF);
        lumaStatus.lumaBin[15] =  MHal_XC_R2BYTEMSK( REG_SC_BK1A_4F_L , 0xFFFF);
        lumaStatus.lumaBin[16] =  MHal_XC_R2BYTEMSK( REG_SC_BK1A_50_L , 0xFFFF);
        lumaStatus.lumaBin[17] =  MHal_XC_R2BYTEMSK( REG_SC_BK1A_51_L , 0xFFFF);
        lumaStatus.lumaBin[18] =  MHal_XC_R2BYTEMSK( REG_SC_BK1A_52_L , 0xFFFF);
        lumaStatus.lumaBin[19] =  MHal_XC_R2BYTEMSK( REG_SC_BK1A_53_L , 0xFFFF);
        lumaStatus.lumaBin[20] =  MHal_XC_R2BYTEMSK( REG_SC_BK1A_54_L , 0xFFFF);
        lumaStatus.lumaBin[21] =  MHal_XC_R2BYTEMSK( REG_SC_BK1A_55_L , 0xFFFF);
        lumaStatus.lumaBin[22] =  MHal_XC_R2BYTEMSK( REG_SC_BK1A_56_L , 0xFFFF);
        lumaStatus.lumaBin[23] =  MHal_XC_R2BYTEMSK( REG_SC_BK1A_57_L , 0xFFFF);
        lumaStatus.lumaBin[24] =  MHal_XC_R2BYTEMSK( REG_SC_BK1A_58_L , 0xFFFF);
        lumaStatus.lumaBin[25] =  MHal_XC_R2BYTEMSK( REG_SC_BK1A_59_L , 0xFFFF);
        lumaStatus.lumaBin[26] =  MHal_XC_R2BYTEMSK( REG_SC_BK1A_5A_L , 0xFFFF);
        lumaStatus.lumaBin[27] =  MHal_XC_R2BYTEMSK( REG_SC_BK1A_5B_L , 0xFFFF);
        lumaStatus.lumaBin[28] =  MHal_XC_R2BYTEMSK( REG_SC_BK1A_5C_L , 0xFFFF);
        lumaStatus.lumaBin[29] =  MHal_XC_R2BYTEMSK( REG_SC_BK1A_5D_L , 0xFFFF);
        lumaStatus.lumaBin[30] =  MHal_XC_R2BYTEMSK( REG_SC_BK1A_5E_L , 0xFFFF);
        lumaStatus.lumaBin[31] =  MHal_XC_R2BYTEMSK( REG_SC_BK1A_5F_L , 0xFFFF);
    }
    else
    {
        lumaStatus.lumaBin[0] =  MHal_XC_R2BYTEMSK( REG_FSC_BK55_30_L , 0xFFFF)+(MHal_XC_R2BYTEMSK( REG_FSC_BK55_31_L , 0x000F)<<16);
        lumaStatus.lumaBin[1] =  MHal_XC_R2BYTEMSK( REG_FSC_BK55_32_L , 0xFFFF)+(MHal_XC_R2BYTEMSK( REG_FSC_BK55_33_L , 0x000F)<<16);
        lumaStatus.lumaBin[2] =  MHal_XC_R2BYTEMSK( REG_FSC_BK55_34_L , 0xFFFF)+(MHal_XC_R2BYTEMSK( REG_FSC_BK55_35_L , 0x000F)<<16);
        lumaStatus.lumaBin[3] =  MHal_XC_R2BYTEMSK( REG_FSC_BK55_36_L , 0xFFFF)+(MHal_XC_R2BYTEMSK( REG_FSC_BK55_37_L , 0x000F)<<16);
        lumaStatus.lumaBin[4] =  MHal_XC_R2BYTEMSK( REG_FSC_BK55_38_L , 0xFFFF)+(MHal_XC_R2BYTEMSK( REG_FSC_BK55_39_L , 0x000F)<<16);
        lumaStatus.lumaBin[5] =  MHal_XC_R2BYTEMSK( REG_FSC_BK55_3A_L , 0xFFFF)+(MHal_XC_R2BYTEMSK( REG_FSC_BK55_3B_L , 0x000F)<<16);
        lumaStatus.lumaBin[6] =  MHal_XC_R2BYTEMSK( REG_FSC_BK55_3C_L , 0xFFFF)+(MHal_XC_R2BYTEMSK( REG_FSC_BK55_3D_L , 0x000F)<<16);
        lumaStatus.lumaBin[7] =  MHal_XC_R2BYTEMSK( REG_FSC_BK55_3E_L , 0xFFFF)+(MHal_XC_R2BYTEMSK( REG_FSC_BK55_3F_L , 0x000F)<<16);
        lumaStatus.lumaBin[8] =  MHal_XC_R2BYTEMSK( REG_FSC_BK55_40_L , 0xFFFF)+(MHal_XC_R2BYTEMSK( REG_FSC_BK55_41_L , 0x000F)<<16);
        lumaStatus.lumaBin[9] =  MHal_XC_R2BYTEMSK( REG_FSC_BK55_42_L , 0xFFFF)+(MHal_XC_R2BYTEMSK( REG_FSC_BK55_43_L , 0x000F)<<16);
        lumaStatus.lumaBin[10] =  MHal_XC_R2BYTEMSK( REG_FSC_BK55_44_L , 0xFFFF)+(MHal_XC_R2BYTEMSK( REG_FSC_BK55_45_L , 0x000F)<<16);
        lumaStatus.lumaBin[11] =  MHal_XC_R2BYTEMSK( REG_FSC_BK55_46_L , 0xFFFF)+(MHal_XC_R2BYTEMSK( REG_FSC_BK55_47_L , 0x000F)<<16);
        lumaStatus.lumaBin[12] =  MHal_XC_R2BYTEMSK( REG_FSC_BK55_48_L , 0xFFFF)+(MHal_XC_R2BYTEMSK( REG_FSC_BK55_49_L , 0x000F)<<16);
        lumaStatus.lumaBin[13] =  MHal_XC_R2BYTEMSK( REG_FSC_BK55_4A_L , 0xFFFF)+(MHal_XC_R2BYTEMSK( REG_FSC_BK55_4B_L , 0x000F)<<16);
        lumaStatus.lumaBin[14] =  MHal_XC_R2BYTEMSK( REG_FSC_BK55_4C_L , 0xFFFF)+(MHal_XC_R2BYTEMSK( REG_FSC_BK55_4D_L , 0x000F)<<16);
        lumaStatus.lumaBin[15] =  MHal_XC_R2BYTEMSK( REG_FSC_BK55_4E_L , 0xFFFF)+(MHal_XC_R2BYTEMSK( REG_FSC_BK55_4F_L , 0x000F)<<16);
        lumaStatus.lumaBin[16] =  MHal_XC_R2BYTEMSK( REG_FSC_BK55_50_L , 0xFFFF)+(MHal_XC_R2BYTEMSK( REG_FSC_BK55_51_L , 0x000F)<<16);
        lumaStatus.lumaBin[17] =  MHal_XC_R2BYTEMSK( REG_FSC_BK55_52_L , 0xFFFF)+(MHal_XC_R2BYTEMSK( REG_FSC_BK55_53_L , 0x000F)<<16);
        lumaStatus.lumaBin[18] =  MHal_XC_R2BYTEMSK( REG_FSC_BK55_54_L , 0xFFFF)+(MHal_XC_R2BYTEMSK( REG_FSC_BK55_55_L , 0x000F)<<16);
        lumaStatus.lumaBin[19] =  MHal_XC_R2BYTEMSK( REG_FSC_BK55_56_L , 0xFFFF)+(MHal_XC_R2BYTEMSK( REG_FSC_BK55_57_L , 0x000F)<<16);
        lumaStatus.lumaBin[20] =  MHal_XC_R2BYTEMSK( REG_FSC_BK55_58_L , 0xFFFF)+(MHal_XC_R2BYTEMSK( REG_FSC_BK55_59_L , 0x000F)<<16);
        lumaStatus.lumaBin[21] =  MHal_XC_R2BYTEMSK( REG_FSC_BK55_5A_L , 0xFFFF)+(MHal_XC_R2BYTEMSK( REG_FSC_BK55_5B_L , 0x000F)<<16);
        lumaStatus.lumaBin[22] =  MHal_XC_R2BYTEMSK( REG_FSC_BK55_5C_L , 0xFFFF)+(MHal_XC_R2BYTEMSK( REG_FSC_BK55_5D_L , 0x000F)<<16);
        lumaStatus.lumaBin[23] =  MHal_XC_R2BYTEMSK( REG_FSC_BK55_5E_L , 0xFFFF)+(MHal_XC_R2BYTEMSK( REG_FSC_BK55_5F_L , 0x000F)<<16);
        lumaStatus.lumaBin[24] =  MHal_XC_R2BYTEMSK( REG_FSC_BK55_60_L , 0xFFFF)+(MHal_XC_R2BYTEMSK( REG_FSC_BK55_61_L , 0x000F)<<16);
        lumaStatus.lumaBin[25] =  MHal_XC_R2BYTEMSK( REG_FSC_BK55_62_L , 0xFFFF)+(MHal_XC_R2BYTEMSK( REG_FSC_BK55_63_L , 0x000F)<<16);
        lumaStatus.lumaBin[26] =  MHal_XC_R2BYTEMSK( REG_FSC_BK55_64_L , 0xFFFF)+(MHal_XC_R2BYTEMSK( REG_FSC_BK55_65_L , 0x000F)<<16);
        lumaStatus.lumaBin[27] =  MHal_XC_R2BYTEMSK( REG_FSC_BK55_66_L , 0xFFFF)+(MHal_XC_R2BYTEMSK( REG_FSC_BK55_67_L , 0x000F)<<16);
        lumaStatus.lumaBin[28] =  MHal_XC_R2BYTEMSK( REG_FSC_BK55_68_L , 0xFFFF)+(MHal_XC_R2BYTEMSK( REG_FSC_BK55_69_L , 0x000F)<<16);
        lumaStatus.lumaBin[29] =  MHal_XC_R2BYTEMSK( REG_FSC_BK55_6A_L , 0xFFFF)+(MHal_XC_R2BYTEMSK( REG_FSC_BK55_6B_L , 0x000F)<<16);
        lumaStatus.lumaBin[30] =  MHal_XC_R2BYTEMSK( REG_FSC_BK55_6C_L , 0xFFFF)+(MHal_XC_R2BYTEMSK( REG_FSC_BK55_6D_L , 0x000F)<<16);
        lumaStatus.lumaBin[31] =  MHal_XC_R2BYTEMSK( REG_FSC_BK55_6E_L , 0xFFFF)+(MHal_XC_R2BYTEMSK( REG_FSC_BK55_6F_L , 0x000F)<<16);
    }

    for (i=0; i<32; i++)
    {
        lumaStatus.lumaBin[i]= lumaStatus.lumaBin[i]>>4;
    }
    return lumaStatus;
}
#define ScenceChange_TH        0x1000
MS_BOOL MDrv_SC_Agate_SceneChangeY_PQ( const  XC_LumaHistogramStates lumaHistogram )
{
    //MS_U32 ScenceChange_TH = MHal_XC_R2BYTEMSK( REG_FSC_BK30_0E_L , 0xFFFF);
    //XC_DLC_ApiStatus DLC_Status;
    //MApi_XC_DLC_GetStatus_Ex(&DLC_Status,MAIN_WINDOW);
    static MS_U32 Pre_LumaHistogram[32], Max_HistogramDiff = 0;
    MS_U16 HistogramDiff = 0;
    int i = 0;

    for(i=0;i<32;i++)
    {

        HistogramDiff = abs( lumaHistogram.lumaBin[i] - Pre_LumaHistogram[i] );

        if(HistogramDiff > Max_HistogramDiff)
        {
            Max_HistogramDiff = HistogramDiff;
        }

        Pre_LumaHistogram[i] = lumaHistogram.lumaBin[i];

    }


    if( Max_HistogramDiff > ScenceChange_TH )
    {
        Max_HistogramDiff = 0;
        return true;
    }
    else
    {
        Max_HistogramDiff = 0;
        return false;
    }
}
 #define MAX_NM_GAIN 0x8
 #define MIN_NM_GAIN 0x0
 void NoiseMaskingControl(int DBKStr,MS_BOOL isSceneChange)
 {
     //int NM_Gain = minmax((((166*DBKStr)>>8) - 96), 0, 16);
     int NM_Gain = minmax((((166*DBKStr)>>8) - 96), MIN_NM_GAIN, MAX_NM_GAIN);
     MHal_XC_W2BYTEMSK(REG_SC_BK0C_52_L, NM_Gain, 0x003F);//Y noise masking gain
     if (isSceneChange )
     {
         MHal_XC_W2BYTEMSK(REG_SC_BK0C_50_L, 0x00, 0x0008); //Y noise masking off
     }
     else
     {
         MHal_XC_W2BYTEMSK(REG_SC_BK0C_50_L, 0x08, 0x0008); //Y noise masking on
     }
 }
 MS_S8 normFactorWidth(MS_U32 u32AvgBLKN)
 {
     MS_S8 s8NormFac = 0;
     if (u32AvgBLKN > 1048576*1024)
     {
         s8NormFac = 25;
     }
     else if (u32AvgBLKN > 1048576*512)
     {
         s8NormFac = 24;
     }
     else if (u32AvgBLKN > 1048576*256)
     {
         s8NormFac = 23;
     }
     else if (u32AvgBLKN > 1048576*128)
     {
         s8NormFac = 22;
     }
     else if (u32AvgBLKN > 1048576*64)
     {
         s8NormFac = 21;
     }
     else if (u32AvgBLKN > 1048576*32)
     {
         s8NormFac = 20;
     }
     else if (u32AvgBLKN > 1048576*16)
     {
         s8NormFac = 19;
     }
     else if (u32AvgBLKN > 1048576*8)
     {
         s8NormFac = 18;
     }
     else if (u32AvgBLKN > 1048576*4)
     {
         s8NormFac = 17;
     }
     else if (u32AvgBLKN > 1048576*2)
     {
         s8NormFac = 16;
     }
     else if (u32AvgBLKN > 1048576)
     {
         s8NormFac = 15;
     }
     else if (u32AvgBLKN > 524288)
     {
         s8NormFac = 14;
     }
     else if (u32AvgBLKN > 262144)
     {
         s8NormFac = 13;
     }
     else if (u32AvgBLKN > 131072)
     {
         s8NormFac = 12;
     }
     else if (u32AvgBLKN > 65536)
     {
         s8NormFac = 11;
     }
     else if (u32AvgBLKN > 32768)
     {
         s8NormFac = 10;
     }
     else if (u32AvgBLKN > 16384)
     {
         s8NormFac = 9;
     }
     else if (u32AvgBLKN > 8192)
     {
         s8NormFac = 8;
     }
     else if (u32AvgBLKN > 4096)
     {
         s8NormFac = 7;
     }
     else if (u32AvgBLKN > 2048)
     {
         s8NormFac = 6;
     }
     else if (u32AvgBLKN > 1024)
     {
         s8NormFac = 5;
     }
     else if (u32AvgBLKN > 512)
     {
         s8NormFac = 4;
     }
     else if (u32AvgBLKN > 256)
     {
         s8NormFac = 3;
     }
     else if (u32AvgBLKN > 128)
     {
         s8NormFac = 2;
     }
     else if (u32AvgBLKN > 64)
     {
         s8NormFac = 1;
     }
     else if (u32AvgBLKN > 32)
     {
         s8NormFac = 0;
     }
     else if (u32AvgBLKN > 16)
     {
         s8NormFac = -1;
     }
     else if (u32AvgBLKN > 8)
     {
         s8NormFac = -2;
     }
     else
     {
         s8NormFac = 0;
     }

     return s8NormFac;
 }
 MS_U8 readHistogramPart(MS_U16* au16Histogram, MS_U8 u8CountForDBK, MS_U16 u16Initial_point, MS_U16 u16End_point)
 {
     MS_U8 u8Data_ack_en;
     MS_U16 u16Bin;

         //--------- Blockiness Histogram Request start ---------//
     if (u8CountForDBK == 0)
     {
// write 1 to request register
         MHal_XC_W2BYTEMSK(REG_SC_BK0C_0D_L, 1, 0x0001);
     }

     if (1)
     {
         //--------- Blockiness Histogram Acknowledge start ---------//
         u8Data_ack_en = MHal_XC_R2BYTEMSK(REG_SC_BK0C_0D_L, 0x0002) ;
         //--------- Function Process start ---------//
         if(u8Data_ack_en)
         {
             // write 1 to sram_io_en register
             MHal_XC_W2BYTEMSK(REG_SC_BK0C_0D_L, 0x0100, 0x0100);
             for(u16Bin = u16Initial_point; u16Bin < u16End_point; u16Bin++)
             {
                 MHal_XC_W2BYTEMSK(REG_SC_BK0C_0E_L, (MS_U16)(0x8000 + u16Bin), 0xFFFF);
                 // read data from selected address
                 au16Histogram[u16Bin] = MHal_XC_R2BYTEMSK(REG_SC_BK0C_0F_L, 0xFFFF);
             }
             MHal_XC_W2BYTEMSK(REG_SC_BK0C_0D_L, 0, 0x0001);
             return 1;
         }
         else//(!data_ack_en)
         {
             return 0;
         }
     }
 }
 MS_S8 DBK_WidthPartHW(MS_U16 *u16WidthDiffHW, MS_U16* u16Avg,MS_BOOL isSceneChangeY,MS_U8* u8WidthConfidence)
{
     MS_U8 u8Index;
     MS_U32 u32MaxWidthDiff  = 0;
     MS_U32 u32MinWidthDiff  = 0x7FFFFFFF;
     MS_U32 u32Min2WidthDiff = 0x7FFFFFFF;
     MS_U32 u32Min3WidthDiff = 0x7FFFFFFF;
     MS_U32 u32Min4WidthDiff = 0x7FFFFFFF;
     MS_S8 s8MinIdx = -1, s8Min2Idx = -1, s8Min3Idx = -1, s8Min4Idx = -1;// maxIdx = -1;
     static MS_U8 u8TempWidth = 4;
     MS_U16 u16DiffThrd = 2048;
     static MS_S16 s16ConfidenceLevel8 = 0,s16ConfidenceLevel11 = 0,s16ConfidenceLevel16 = 0,s16ConfidenceLevel21 = 0;
     int log_en = 0;
     //og_en = (MS_U32)MHal_XC_R2BYTEMSK(REG_SC_BK30_01_L, 0x000F); //easter

     u16WidthDiffHW[0 ] = MHal_XC_R2BYTEMSK(REG_SC_BK60_40_L,0xFFFF) ; // width 4
     u16WidthDiffHW[1 ] = MHal_XC_R2BYTEMSK(REG_SC_BK60_41_L,0xFFFF) ; // width 5
     u16WidthDiffHW[2 ] = MHal_XC_R2BYTEMSK(REG_SC_BK60_42_L,0xFFFF) ; // width 6
     u16WidthDiffHW[3 ] = MHal_XC_R2BYTEMSK(REG_SC_BK60_43_L,0xFFFF) ; // width 7
     u16WidthDiffHW[4 ] = MHal_XC_R2BYTEMSK(REG_SC_BK60_44_L,0xFFFF) ; // width 8
     u16WidthDiffHW[5 ] = MHal_XC_R2BYTEMSK(REG_SC_BK60_45_L,0xFFFF) ; // width 9
     u16WidthDiffHW[6 ] = MHal_XC_R2BYTEMSK(REG_SC_BK60_46_L,0xFFFF) ; // width 10
     u16WidthDiffHW[7 ] = MHal_XC_R2BYTEMSK(REG_SC_BK60_47_L,0xFFFF) ; // width 11
     u16WidthDiffHW[8 ] = MHal_XC_R2BYTEMSK(REG_SC_BK60_48_L,0xFFFF) ; // width 12
     u16WidthDiffHW[9 ] = MHal_XC_R2BYTEMSK(REG_SC_BK60_49_L,0xFFFF) ; // width 13
     u16WidthDiffHW[10] = MHal_XC_R2BYTEMSK(REG_SC_BK60_4A_L,0xFFFF) ; // width 14
     u16WidthDiffHW[11] = MHal_XC_R2BYTEMSK(REG_SC_BK60_4B_L,0xFFFF) ; // width 15
     u16WidthDiffHW[12] = MHal_XC_R2BYTEMSK(REG_SC_BK60_4C_L,0xFFFF) ; // width 16
     u16WidthDiffHW[13] = MHal_XC_R2BYTEMSK(REG_SC_BK60_4D_L,0xFFFF) ; // width 17
     u16WidthDiffHW[14] = MHal_XC_R2BYTEMSK(REG_SC_BK60_4E_L,0xFFFF) ; // width 18
     u16WidthDiffHW[15] = MHal_XC_R2BYTEMSK(REG_SC_BK60_4F_L,0xFFFF) ; // width 19
     u16WidthDiffHW[16] = MHal_XC_R2BYTEMSK(REG_SC_BK60_50_L,0xFFFF) ; // width 20
     u16WidthDiffHW[17] = MHal_XC_R2BYTEMSK(REG_SC_BK60_51_L,0xFFFF) ; // width 21
     u16WidthDiffHW[18] = MHal_XC_R2BYTEMSK(REG_SC_BK60_52_L,0xFFFF) ; // width 22
     u16WidthDiffHW[19] = MHal_XC_R2BYTEMSK(REG_SC_BK60_53_L,0xFFFF) ; // width 23
     u16WidthDiffHW[20] = MHal_XC_R2BYTEMSK(REG_SC_BK60_54_L,0xFFFF) ; // width 24
     u16WidthDiffHW[21] = MHal_XC_R2BYTEMSK(REG_SC_BK60_55_L,0xFFFF) ; // width 25
     u16WidthDiffHW[22] = MHal_XC_R2BYTEMSK(REG_SC_BK60_56_L,0xFFFF) ; // width 26
     u16WidthDiffHW[23] = MHal_XC_R2BYTEMSK(REG_SC_BK60_57_L,0xFFFF) ; // width 27
     u16WidthDiffHW[24] = MHal_XC_R2BYTEMSK(REG_SC_BK60_58_L,0xFFFF) ; // width 28
     u16WidthDiffHW[25] = MHal_XC_R2BYTEMSK(REG_SC_BK60_59_L,0xFFFF) ; // width 29
     u16WidthDiffHW[26] = MHal_XC_R2BYTEMSK(REG_SC_BK60_5A_L,0xFFFF) ; // width 30
     u16WidthDiffHW[27] = MHal_XC_R2BYTEMSK(REG_SC_BK60_5B_L,0xFFFF) ; // width 31
     u16WidthDiffHW[28] = MHal_XC_R2BYTEMSK(REG_SC_BK60_5C_L,0xFFFF) ; // width 32
     u16WidthDiffHW[29] = MHal_XC_R2BYTEMSK(REG_SC_BK60_5D_L,0xFFFF) ; // width 33
     u16WidthDiffHW[30] = MHal_XC_R2BYTEMSK(REG_SC_BK60_5E_L,0xFFFF) ; // width 34
     u16WidthDiffHW[31] = MHal_XC_R2BYTEMSK(REG_SC_BK60_5F_L,0xFFFF) ; // width 35

     for(u8Index = 1 ; u8Index < paraSize ; u8Index++)
     {
         if (u16WidthDiffHW[u8Index] > u32MaxWidthDiff)
         {
             u32MaxWidthDiff = u16WidthDiffHW[u8Index];
         }
     }

     for(u8Index = 1 ; u8Index < paraSize -1; u8Index+=2)
     {
         if ( (u8Index==3 || u8Index==11 || u8Index==19 || u8Index==27) && (u16WidthDiffHW[u8Index+1] - u16WidthDiffHW[u8Index ] <= u16DiffThrd) )
         {
             u16WidthDiffHW[u8Index+1]= min_1(u16WidthDiffHW[u8Index ], u16WidthDiffHW[u8Index+1]);
             u16WidthDiffHW[u8Index  ]= u32MaxWidthDiff;
         }
         else if ( (u8Index==7) && ( (u16WidthDiffHW[u8Index  ] - u16WidthDiffHW[u8Index-1 ] <= u16DiffThrd) || (u16WidthDiffHW[u8Index  ] - u16WidthDiffHW[u8Index+1 ] <= u16DiffThrd)) )
         {
             u16WidthDiffHW[u8Index  ]= min_3(u16WidthDiffHW[u8Index ], u16WidthDiffHW[u8Index-1], u16WidthDiffHW[u8Index+1]);
             u16WidthDiffHW[u8Index-1]= u32MaxWidthDiff;
             u16WidthDiffHW[u8Index+1]= u32MaxWidthDiff;
         }
     }

     for(u8Index = 1 ; u8Index < paraSize ; u8Index++)
     {
         (*u16Avg) += u16WidthDiffHW[u8Index];
         if (u16WidthDiffHW[u8Index] < u32MinWidthDiff)
         {
             u32Min4WidthDiff = u32Min3WidthDiff;
             s8Min4Idx = s8Min3Idx;
             u32Min3WidthDiff = u32Min2WidthDiff;
             s8Min3Idx = s8Min2Idx;
             u32Min2WidthDiff = u32MinWidthDiff;
             s8Min2Idx = s8MinIdx;
             u32MinWidthDiff = u16WidthDiffHW[u8Index];
             s8MinIdx = u8Index;
         }
         else if (u16WidthDiffHW[u8Index] < u32Min2WidthDiff)
         {
             u32Min4WidthDiff = u32Min3WidthDiff;
             s8Min4Idx = s8Min3Idx;
             u32Min3WidthDiff = u32Min2WidthDiff;
             s8Min3Idx = s8Min2Idx;
             u32Min2WidthDiff = u16WidthDiffHW[u8Index];
             s8Min2Idx = u8Index;
         }
         else if (u16WidthDiffHW[u8Index] < u32Min3WidthDiff)
         {
             u32Min4WidthDiff = u32Min3WidthDiff;
             s8Min4Idx = s8Min3Idx;
             u32Min3WidthDiff = u16WidthDiffHW[u8Index];
             s8Min3Idx = u8Index;
         }
         else if (u16WidthDiffHW[u8Index] < u32Min4WidthDiff)
         {
             u32Min4WidthDiff = u16WidthDiffHW[u8Index];
             s8Min4Idx = u8Index;
         }
     }

     *u16Avg = (*u16Avg) / (paraSize-1);
    if(isSceneChangeY)
    {
        s16ConfidenceLevel8 = 0;
        s16ConfidenceLevel11 = 0;
        s16ConfidenceLevel16 = 0;
        s16ConfidenceLevel21 = 0;
    }
     s16ConfidenceLevel8 = minmax(s16ConfidenceLevel8,0,16);
     s16ConfidenceLevel11 = minmax(s16ConfidenceLevel11,0,16);
     s16ConfidenceLevel16 = minmax(s16ConfidenceLevel16,0,16);
     s16ConfidenceLevel21 = minmax(s16ConfidenceLevel21,0,16);
     if(s16ConfidenceLevel8>15)
     {
         s16ConfidenceLevel11 = 0;
         s16ConfidenceLevel16 = 0;
         s16ConfidenceLevel21 = 0;
     }
     else if(s16ConfidenceLevel11>15)
     {
         s16ConfidenceLevel8 = 0;
         s16ConfidenceLevel16 = 0;
         s16ConfidenceLevel21 = 0;
     }
     else if(s16ConfidenceLevel16>15)
     {
         s16ConfidenceLevel8 = 0;
         s16ConfidenceLevel11 = 0;
         s16ConfidenceLevel21 = 0;
     }
     else if(s16ConfidenceLevel21>15)
     {
         s16ConfidenceLevel8 = 0;
         s16ConfidenceLevel16 = 0;
         s16ConfidenceLevel11 = 0;
     }

     if(log_en==5)
     {
         //printf("s8MinIdx:%d s8Min2Idx:%d s8Min3Idx:%d s8Min4Idx:%d\n",s8MinIdx,s8Min2Idx,s8Min3Idx,s8Min4Idx);
         //printf("u16ConfidenceLevel8:%d u16ConfidenceLevel11:%d\n",s16ConfidenceLevel8,s16ConfidenceLevel11);
         //printf("u16ConfidenceLevel16:%d u16ConfidenceLevel21:%d\n",s16ConfidenceLevel16,s16ConfidenceLevel21);
     }
     *u8WidthConfidence = max_3(s16ConfidenceLevel8,s16ConfidenceLevel11,s16ConfidenceLevel16);
     *u8WidthConfidence = max_1((*u8WidthConfidence),s16ConfidenceLevel21);
     /*if(u8TempWidth == 4 )//&& s16ConfidenceLevel8>10)
     {
         if((abs(s8MinIdx-u8TempWidth) <2)||(abs(s8Min2Idx-u8TempWidth) <2)||(abs(s8Min3Idx-u8TempWidth) <2)||(abs(s8Min4Idx-u8TempWidth) <2))
         {
             return 4;
         }
     }
     else*/
     if(u8TempWidth == 7 && s16ConfidenceLevel11>10)
     {
         if ((((s8MinIdx == 7) || (s8Min2Idx == 7) || (s8Min3Idx == 7) || (s8Min4Idx == 7))|| ((s8MinIdx == 16) || (s8Min2Idx == 16) || (s8Min3Idx == 16) || (s8Min4Idx == 16))
                 || ((s8MinIdx == 17) || (s8Min2Idx == 17) || (s8Min3Idx == 17) || (s8Min4Idx == 17)) ) && ((s8MinIdx == 28) || (s8Min2Idx == 28) || (s8Min3Idx == 28) || (s8Min4Idx == 28)))
         {
            s16ConfidenceLevel11++;
             return 7;
         }
         else if((abs(s8MinIdx-u8TempWidth) <2)||(abs(s8Min2Idx-u8TempWidth) <2)||(abs(s8Min3Idx-u8TempWidth) <2)||(abs(s8Min4Idx-u8TempWidth) <2))
         {
            s16ConfidenceLevel11--;
             return 7;
         }
     }
     else if(u8TempWidth == 17 && s16ConfidenceLevel21>10)
     {
          if ((((s8MinIdx == 16 || s8Min2Idx == 16 || s8Min3Idx == 16) || (s8MinIdx == 18 || s8Min2Idx == 18 || s8Min3Idx == 18) ) && (s8MinIdx == 17 || s8Min2Idx == 17 || s8Min3Idx == 17)))
         {
            s16ConfidenceLevel21++;
             u8TempWidth = 17;
             return 17;
         }
         else if((abs(s8MinIdx-u8TempWidth) <4)||(abs(s8Min2Idx-u8TempWidth) <4)||(abs(s8Min3Idx-u8TempWidth) <4)||(abs(s8Min4Idx-u8TempWidth) <4))
         {
            s16ConfidenceLevel21--;
             return 17;
         }
     }

     if ((((s8MinIdx == 7) || (s8Min2Idx == 7) || (s8Min3Idx == 7) || (s8Min4Idx == 7))|| ((s8MinIdx == 16) || (s8Min2Idx == 16) || (s8Min3Idx == 16) || (s8Min4Idx == 16))
             || ((s8MinIdx == 17) || (s8Min2Idx == 17) || (s8Min3Idx == 17) || (s8Min4Idx == 17)) ) && ((s8MinIdx == 28) || (s8Min2Idx == 28) || (s8Min3Idx == 28) || (s8Min4Idx == 28)))
     {
         if(u8TempWidth == 7)
         {
             s16ConfidenceLevel11++;
         }
         else
         {
             s16ConfidenceLevel8--;
             s16ConfidenceLevel16--;
             s16ConfidenceLevel21--;
         }
         u8TempWidth = 7;
         return 7;
     }
     else if ((((s8MinIdx == 16 || s8Min2Idx == 16 || s8Min3Idx == 16) || (s8MinIdx == 18 || s8Min2Idx == 18 || s8Min3Idx == 18) ) && (s8MinIdx == 17 || s8Min2Idx == 17 || s8Min3Idx == 17)))
     {
         if(u8TempWidth == 17)
         {
             s16ConfidenceLevel21++;
         }
         else
         {
             s16ConfidenceLevel8--;
             s16ConfidenceLevel16--;
             s16ConfidenceLevel11--;
         }
         u8TempWidth = 17;
         return 17;
     }

     if (s8MinIdx == 4 || s8Min2Idx == 4 || s8Min3Idx == 4 || s8Min4Idx == 4)
     {
         if(u8TempWidth == 4)
         {
             s16ConfidenceLevel8++;
         }
         else
         {
             s16ConfidenceLevel11--;
             s16ConfidenceLevel16--;
             s16ConfidenceLevel21--;
         }
         u8TempWidth = 4;
         return 4;
     }
     else if (s8MinIdx == 12 || s8Min2Idx == 12 || s8Min3Idx == 12 || s8Min4Idx == 12)
     {
        if(u8TempWidth == 12)
        {
            s16ConfidenceLevel16++;
        }
        else
        {
            s16ConfidenceLevel8--;
            s16ConfidenceLevel11--;
            s16ConfidenceLevel21--;
        }
         u8TempWidth = 12;
         return 12;
     }

     return -1;
 }
 MS_S8 DBK_WidthPartTop(MS_U16 *u16WidthDiffHW, MS_U16* u16MaxMinDiff, MS_U16* u16Avg,MS_BOOL isSceneChangeY,MS_U8* u8WidthConfidence)
 {
     MS_U8 u8Bin;
     MS_U32 u32MinValue = 0x7FFFFFFF;
     MS_U16 u16MaxValue = 0;
     MS_S8 s8NormFac;
     MS_S8 s8MinIndex;
     s8MinIndex = DBK_WidthPartHW(u16WidthDiffHW, u16Avg,isSceneChangeY,u8WidthConfidence);
     if (s8MinIndex == -1)
     {
         for (u8Bin =0; u8Bin<paraSize;u8Bin++)
         {
             if (u16WidthDiffHW[u8Bin] > u16MaxValue )
            {
                 u16MaxValue = u16WidthDiffHW[u8Bin];
            }

             if (u16WidthDiffHW[u8Bin] < u32MinValue )
            {
                 u32MinValue = u16WidthDiffHW[u8Bin];
            }
         }
         *u16MaxMinDiff = u16MaxValue - u32MinValue;
         s8NormFac = normFactorWidth(*u16MaxMinDiff);
         if(s8NormFac >0)
         {
             for (u8Bin=0;u8Bin<paraSize;u8Bin++)
             {
                 u16WidthDiffHW[u8Bin] = (u16WidthDiffHW[u8Bin] - u32MinValue) >> s8NormFac;
             }
             *u16Avg = (*u16Avg-u32MinValue) >> s8NormFac;
             *u16MaxMinDiff = (*u16MaxMinDiff) >>s8NormFac;
         }
         else if(s8NormFac ==0)
         {
             for (u8Bin=0;u8Bin<paraSize;u8Bin++)
             {
                 u16WidthDiffHW[u8Bin] = (u16WidthDiffHW[u8Bin] - u32MinValue);
             }
             *u16Avg = (*u16Avg-u32MinValue);
             *u16MaxMinDiff = (*u16MaxMinDiff);
         }
         else
         {
             s8NormFac = 0-s8NormFac;
             for (u8Bin=0;u8Bin<paraSize;u8Bin++)
             {
                 u16WidthDiffHW[u8Bin] = (u16WidthDiffHW[u8Bin] - u32MinValue) << s8NormFac;
             }
             *u16Avg = (*u16Avg-u32MinValue) <<s8NormFac;
             *u16MaxMinDiff = (*u16MaxMinDiff) <<s8NormFac;
         }
         return -1;
     }
     else
     {
         return s8MinIndex;
     }
 }
 void WidthIIR_H(MS_U8 u8Width) // Width IIR Protection
 {
     static MS_U8 u8HysteresisCount = 0;
     static MS_U8 u8LastWidth = 8;
     int log_en = 0;
     //log_en = (MS_U32)MHal_XC_R2BYTEMSK(REG_SC_BK30_01_L, 0x000F); //easter
     if(u8HysteresisCount<6)
     {
         if(log_en==5)
         {
             //printf("new width:%d last width:%d final width:%d hysteresisCount:%d\n",u8Width,u8LastWidth,WidthIIR,u8HysteresisCount);
         }

         if(u8Width == u8LastWidth)
         {
             u8HysteresisCount = 0;
             WidthIIR = u8Width;
         }
         else
         {
             u8HysteresisCount++;
             WidthIIR = u8LastWidth;
         }
     }
     else
     {
         WidthIIR = u8Width;
         u8HysteresisCount = 0;
     }
     u8LastWidth = WidthIIR;
 }
 MS_S8 DBK_ComputeWidthPart(  MS_U16 *u16WidthDiffHW,MS_U8 u8LastminIdex, MS_U16 u16Avg,MS_U8 u8WidthConfidence)
 {
     MS_BOOL bWidthCandidate[paraSize];
     MS_U16 u16WTh = u16Avg;
     MS_U8 u8MinIndex=0;
     MS_U8 u8NumW=0;
     MS_U8 u8Bin;
     MS_BOOL bIf8 = 0, bIf11 = 0, bIf21 = 0, bIf32 = 0;

     for (u8Bin=0;u8Bin<paraSize;u8Bin++)
     {
         if (u16WidthDiffHW[u8Bin]<= u16WTh)
         {
             bWidthCandidate[u8Bin] = 1;
         }
         else
         {
             bWidthCandidate[u8Bin] = 0;
         }

     }

     if(bWidthCandidate[u8LastminIdex] ==1 && u8WidthConfidence>10)
     {
         return u8LastminIdex;
     }
     else
     {
         if (bWidthCandidate[4] == 1 ) //8
         {
             if (bWidthCandidate[3] == 1) //7
                 bWidthCandidate[3]  = 0;
             if (bWidthCandidate[11] == 1) // 15
                 bWidthCandidate[11] = 0;
             if (bWidthCandidate[12] == 1) // 16
                 bWidthCandidate[12] = 0;
             if (bWidthCandidate[19] == 1) // 23
                 bWidthCandidate[19] = 0;
             if (bWidthCandidate[20] == 1) // 24
                 bWidthCandidate[20] = 0;
             bIf8 = 1;
         }
         if (bWidthCandidate[7] == 1 ) //11
         {
             if (bWidthCandidate[6] == 1) //10
                 bWidthCandidate[6]  = 0;
             if (bWidthCandidate[8] == 1) //12
                 bWidthCandidate[8]  = 0;
             bIf11 = 1;
         }
         if ( ( bWidthCandidate[16] == 1 ) || ( bWidthCandidate[17] == 1 ) ||( bWidthCandidate[18] == 1 ) ) //20 21 22
         {
             bWidthCandidate[16] = 0;
             bWidthCandidate[17] = 1;
             bWidthCandidate[18] = 0;
             bIf21 = 1;
         }
         if ( bWidthCandidate[27] == 1 && bWidthCandidate[28] == 1)
             bIf32 = 1;
         if (bIf11 && bIf21 && bIf32) //11/21/32 all true -> source = 11 ->delete 21
         {
             bWidthCandidate[17] = 0;
         }
         if (bIf8 || bIf11 || bIf21)
         {
             if (bWidthCandidate[27] == 1) // 31
                 bWidthCandidate[27] = 0;
             if (bWidthCandidate[28] == 1) // 32
                 bWidthCandidate[28] = 0;
         }

     }

     for (u8Bin=0;u8Bin<paraSize;u8Bin++) // count number of frequencies
     {
         if (bWidthCandidate[u8Bin] == 1)
         {
             u8MinIndex = u8Bin;
             u8NumW++;
         }
     }
     if (u8NumW == 1)
     {
         return u8MinIndex;
     }
     else
     {
         return -1;
     }
 }
 void WidthToRegister(MS_U8 u8Width) // Write width to register
 {
     MS_U8 u8Left1,u8Right1;
     if(u8Width<8)
     {
         u8Width = 8;
         u8Left1 = 8;
         u8Right1 = 9;
     }
     else if (u8Width==8) //test 0808 for performance
     {
         u8Left1 = 8;
         u8Right1 = 8;
     }
     else
     {
         u8Left1 = u8Width - 1; //fix prevent defect
         u8Right1 = (u8Width >=31) ? 31 : u8Width + 1;
     }
 //write interval left && interval right
     MHal_XC_W2BYTEMSK(REG_SC_BK0C_39_L, (MS_U16)((u8Left1<<8)|u8Right1), 0xFFFF);
 }
 MS_U8 DBK_Width_Detect(MS_BOOL isSceneChangeY)//width detection with hardware accelaration
 {
     MS_S8 s8MinIndex = -1;     //minIndex+width_offset = candidate width
     //For width calculation
     //int WidthCandidate[paraSize];
     MS_U16 au16WidthDiffHW[paraSize];
     MS_U16 u16MaxMinDiff = 0;
     MS_U16 u16WidthAvg = 0;
     MS_U8 u8Width = 8;
     static MS_U8 u8LastminIdex = 4;
     MS_U8 u8WidthConfidence = 0;
     int log_en = 0;
     //log_en = (MS_U32)MHal_XC_R2BYTEMSK(REG_SC_BK30_01_L, 0x000F); //easter
     u16MaxMinDiff = 0;
     u16WidthAvg = 0;

     s8MinIndex = DBK_WidthPartTop( au16WidthDiffHW, &u16MaxMinDiff, &u16WidthAvg,isSceneChangeY,&u8WidthConfidence);

     if (s8MinIndex == -1)
     {
         s8MinIndex = DBK_ComputeWidthPart( au16WidthDiffHW ,u8LastminIdex, u16WidthAvg,u8WidthConfidence);
     }

     if (s8MinIndex >= 0 && s8MinIndex <28)//if (width == 32) //no such pattern yet, only artificials
     {
         u8Width = s8MinIndex + 4;
     }
     else
     {
         u8Width = 8;//can't find any possible width set to default value
     }

     WidthIIR_H(u8Width);
     WidthToRegister(WidthIIR);
     //FeedBackSD2HD(WidthIIR, 0);

     if(log_en==5)
     {
         //printf("**bforeIIR[%d]********\n",u8Width);
         //printf("*********[%d]*********\n",WidthIIR);
         //printf("**Confiden[%d]*********\n",u8WidthConfidence);
     }

     u8LastminIdex = WidthIIR -4;

     return WidthIIR;
 }
 void Mdrv_PQ_DBK(MS_BOOL isSceneChangeY,MS_U32 u32MotionValue,MS_U16 u16Width)
 {
         //DBK Variables
     static MS_U16 au16Histogram[MaxWidth] = {0};
     static MS_BOOL bDBK_initial = 0;
     MS_U8 u8ReadDBKHistAck = 0;
     MS_U16 u16Scan_range = 320;
     MS_U16 u16Initial_point = 160;
     MS_U16 u16End_point = u16Initial_point+u16Scan_range;
     static MS_U8 u8CountForSample = 0;
     static MS_U8 u8CountForDBK = 0;
     static MS_U8 u8Widdth = 8;
     MS_U32 u32Strength[paraSize] = {0};
     static MS_U32 u32Strength_IIR[paraSize] ={0};
     MS_U8 u8MaxBin = 0;
     MS_U32 u32TotalStrength = 0;
     MS_U32 u32StrengthLPF =0;
     MS_U32 u32AvergeStrength = 1;
     MS_U16 u16Bin = 0;
     static MS_U8 au8MaxBin_pre[25] = {0};
     MS_U8 u8Pre_bin = 0;
     MS_S8 s8Boundary_left = 0;
     MS_U8 u8Boundary_right = 0;
     MS_U16 u16GridReport_l = 0;
     MS_U16 u16GridReport_m = 0;
     MS_U16 u16Non_gridReport_l = 0;
     MS_U16 u16Non_gridReport_m = 0;
     MS_U32 u32GridReport_sum=0;
     MS_U32 u32Non_gridReport_sum=0;
     MS_U32 u32Total_Report_sum=0;
     MS_U32 u32AvgReport =25000000,u32DiffReport = 5000000;
     static MS_U32 u32GridReport_sumIIR=0;
     static MS_U32 u32Non_gridReport_sumIIR=0;
     MS_U64 gridReport_percent = 0;
     MS_U64 adaptiveRatio_hwReport = 0,adjust_step = 0;
     MS_U16 maxbin_thrd = 1126;
     static MS_S16 percentage_offset = 0;
     MS_U8 u8DBKGain_hw=0;
     MS_U32 u8DBK_coase_TH = 2;
     MS_U32 u8DetailGain_hw = 2;
     MS_U8 u8STD_Threshold_hw=0;
     MS_U32 u32Max_bin_thrd = 0;
     MS_U16 scaleUp_factor_w11 = 1024;
     static MS_U32 u32StaticValueTemp=0,u32StaticCount=0;
     static MS_U64 adaptiveRatio_hwReport_IIR = 0;
     int log_en = 0;
     //log_en = (MS_U32)MHal_XC_R2BYTEMSK(REG_SC_BK30_02_L, 0x000F); //easter

    if (isSceneChangeY)
    {
        u8CountForDBK = 0;
        for(u16Bin=0;u16Bin<25;u16Bin++)
        {
         au8MaxBin_pre[u16Bin] = 0;
        }
    }
    if(u32MotionValue ==0 && u32StaticValueTemp==0)
    {
        u32StaticCount++;
    }
    else
    {
        u32StaticCount = 0;
    }
    u32StaticValueTemp = u32MotionValue;

    if((u16End_point+u16Scan_range*u8CountForSample) > u16Width)
    {
        u8CountForSample = 0;
    }


     u16Initial_point =min_1((u16Initial_point + u16Scan_range*u8CountForSample),(MaxWidth-1));
     u16End_point = min_1((u16End_point+u16Scan_range*u8CountForSample),(MaxWidth-1));

    if ( (u8CountForDBK == 0)  )// Read Histogram
    {
        u8ReadDBKHistAck = readHistogramPart(au16Histogram, u8CountForDBK, u16Initial_point, u16End_point);
        bDBK_initial = bDBK_initial | true;
        if((u8ReadDBKHistAck==1)&& (u32StaticCount < 15))
        {
            u8Widdth = DBK_Width_Detect(isSceneChangeY);
        }
        u8Widdth = max_1(min_1(u8Widdth,(MS_U32)(paraSize-1)),0);
        if (u8ReadDBKHistAck)
        {
            u8CountForDBK++;
        }
    }
    else if  (u8CountForDBK == 1)
    {
        if ( bDBK_initial == 1)
        {
            if(u32StaticCount < 15)
            {
                for (u16Bin = u16Initial_point; u16Bin <u16End_point; u16Bin++)
                {
                    u32Strength[(u16Bin)%u8Widdth] += (au16Histogram[u16Bin]>>8);
                }

                for (u16Bin = 0; u16Bin <u8Widdth; u16Bin++)
                {
                    u32Strength_IIR[u16Bin] = (6*u32Strength_IIR[u16Bin] + 2*u32Strength[u16Bin])>>3;
                    u32TotalStrength += u32Strength_IIR[u16Bin];
                }

                if(u8Widdth==8)
                {
                    maxbin_thrd = 1126; // 1.1*1024;
                }
                else
                {
                    maxbin_thrd = 1024; // 1.0*1024;
                }

                u32AvergeStrength = max_1(u32TotalStrength/max_1(u8Widdth,1), 1);
                u32Max_bin_thrd = u32AvergeStrength*(maxbin_thrd)>>10;

                if(log_en == 1)
                {
                    //printf(" ===== states iir start=======\n");
                    for(u16Bin = 0; u16Bin <u8Widdth; u16Bin++)
                    {
                        //printf(" %d = %05d\n",u16Bin,u32Strength_IIR[u16Bin]);
                    }
                    //printf(" ===== states iir end=======\n");
                }

                for (u16Bin = 0; u16Bin < u8Widdth; u16Bin++)
                {
                    if (u16Bin == 0)
                    {
                        u32StrengthLPF = (2*u32Strength_IIR[u16Bin]+ u32Strength_IIR[u16Bin+1] + u32Strength_IIR[u8Widdth-1])>>2;
                    }
                    else if (u16Bin == u8Widdth-1)
                    {
                        u32StrengthLPF = (2*u32Strength_IIR[u16Bin]+ u32Strength_IIR[u16Bin-1] + u32Strength_IIR[0])>>2;
                    }
                    else
                    {
                        u32StrengthLPF = (2*u32Strength_IIR[u16Bin]+ u32Strength_IIR[u16Bin-1] + u32Strength_IIR[u16Bin+1])>>2;
                    }

                //if (str3pt >  maxStr)
                //{
                //    maxStr = str3pt;
                //}

                    if(log_en ==1)
                    {
                        //printf("u32StrengthLPF:%d u32Max_bin_thrd:%d\n",u32StrengthLPF,u32Max_bin_thrd);
                    }

                    if (u32StrengthLPF >  u32Max_bin_thrd)
                    {
                        u32Max_bin_thrd = u32StrengthLPF;
                        au8MaxBin_pre[u8CountForSample] = u16Bin;
                    }
                    else if(u32Strength_IIR[u16Bin] > u32Max_bin_thrd)
                    {
                        u32Max_bin_thrd = u32Strength_IIR[u16Bin];
                        au8MaxBin_pre[u8CountForSample] = u16Bin;
                    }
                    else
                    {
                        u8Pre_bin = (u8CountForSample-1<0)?u8CountForSample:(u8CountForSample-1);
                        au8MaxBin_pre[u8CountForSample] = au8MaxBin_pre[u8Pre_bin];
                    }
                }

                for(u16Bin = 0;u16Bin<(u8CountForSample+1);u16Bin++)
                {
                    u8MaxBin+= au8MaxBin_pre[u16Bin];
                    if(log_en ==1)
                    {
                        //printf("au8MaxBin_pre[%d]:%d u8CountForSample:%d\n",u16Bin,au8MaxBin_pre[u16Bin],u8CountForSample);
                    }
                }

                u8MaxBin = u8MaxBin/(u8CountForSample+1);
                u32AvergeStrength = max_1(u32TotalStrength/max_1(u8Widdth,1), 1);

                if(u8Widdth%4 == 0)
                {
                     s8Boundary_left = u8MaxBin - u8Widdth/4;
                     u8Boundary_right = u8MaxBin + u8Widdth/4;
                }
                else if(u8Widdth%4 == 1)
                {
                     s8Boundary_left = u8MaxBin - u8Widdth/4;
                     u8Boundary_right = u8MaxBin + u8Widdth/4;
                }
                else if(u8Widdth%4 == 2)
                {
                     s8Boundary_left = u8MaxBin - (u8Widdth/4);
                     u8Boundary_right = u8MaxBin + (u8Widdth/4+1);
                }
                else
                {
                     s8Boundary_left = u8MaxBin - (u8Widdth/4+1);
                     u8Boundary_right = u8MaxBin + (u8Widdth/4+1);
                }

                if(s8Boundary_left < 0)
                {
                     s8Boundary_left = s8Boundary_left+u8Widdth;
                }

                if(u8Boundary_right >=u8Widdth)
                {
                     u8Boundary_right = u8Boundary_right -u8Widdth;
                }

                if(log_en ==5)
                {
                 //printf("=====Final Width:[%d]=====\n",u8Widdth);
                 //printf("=====Motion :[%u]=====\n",u32MotionValue);
                 //printf("===u32StaticCount:%d===\n",u32StaticCount);
                }

                 MHal_XC_W2BYTEMSK(REG_SC_BK0C_29_L, s8Boundary_left+(u8Boundary_right<<8), 0x1F1F);
                 MHal_XC_W2BYTEMSK(REG_SC_BK0C_28_L, u8Widdth, 0x1F1F);
            }

            if(u8Widdth == 11)
            {
             scaleUp_factor_w11 = 1228; // 1.2;
            }
            else
            {
             scaleUp_factor_w11 = 1024; // 1.0
            }

            u16Non_gridReport_l = MHal_XC_R2BYTEMSK(REG_SC_BK0C_60_L, 0xFFFF);
            u16Non_gridReport_m = MHal_XC_R2BYTEMSK(REG_SC_BK0C_61_L, 0xFFFF);
            u16GridReport_l = MHal_XC_R2BYTEMSK(REG_SC_BK0C_68_L, 0xFFFF);
            u16GridReport_m = MHal_XC_R2BYTEMSK(REG_SC_BK0C_69_L, 0xFFFF);
            u32GridReport_sum= (u16GridReport_l + (u16GridReport_m<<16));
            u32Non_gridReport_sum = (u16Non_gridReport_l + (u16Non_gridReport_m<<16));

            if(u32GridReport_sum == 0 && u32Non_gridReport_sum == 0)
            {
                return;
            }
            u32GridReport_sumIIR = (6*u32GridReport_sumIIR + 2*u32GridReport_sum)>>3;
            u32Non_gridReport_sumIIR = (6*u32Non_gridReport_sumIIR + 2*u32Non_gridReport_sum)>>3;
            gridReport_percent = (((MS_U64)u32GridReport_sumIIR)*256)/(u32GridReport_sumIIR+u32Non_gridReport_sumIIR);

            if(log_en ==1)
            printk("gridReport_percent:%lld %d\n",gridReport_percent,__LINE__);

            gridReport_percent = (gridReport_percent*scaleUp_factor_w11+512)>>10;

            if(log_en ==1)
            printk("gridReport_percent:%lld %d\n",gridReport_percent,__LINE__);

            u32Total_Report_sum = u32GridReport_sumIIR+u32Non_gridReport_sumIIR;

            if(log_en ==1)
            {
                printk("s8Boundary_left:%d,u8Boundary_right:%d,u8Widdth:%d gridReport_percent:%lld\n",s8Boundary_left,u8Boundary_right,u8Widdth,gridReport_percent);
                printk("u32Total_Report_sum:%d,u32GridReport_sumIIR:%d,u32Non_gridReport_sumIIR:%d\n",u32Total_Report_sum,u32GridReport_sumIIR,u32Non_gridReport_sumIIR);
            }

            if((gridReport_percent > 128) && (u32Total_Report_sum > u32AvgReport) && ((u32GridReport_sumIIR - u32Non_gridReport_sumIIR) > u32DiffReport))
            {//for black white water flow ,both of blocky and detail are strong which can't be distinguish by the ratio of blocky/total
                percentage_offset+= adjust_step;
                //printf("percentage_offset:%d\n",(int)(percentage_offset*100));
            }
            else
            {
                percentage_offset-= adjust_step;
            }

            if(log_en ==1)
            printk("percentage_offset:%d\n",percentage_offset);

            percentage_offset = minmax((percentage_offset), 0, 51);
            //percentage_offset/=100.0;

            if(gridReport_percent > 179)
            {
             adaptiveRatio_hwReport = (((921*(gridReport_percent+percentage_offset)+128)>>8) -235);
            }
            else if(gridReport_percent > 128)
            {
             adaptiveRatio_hwReport = (((1766*(gridReport_percent+percentage_offset)+128)>>8) -870);
            }
            else
            {
             adaptiveRatio_hwReport = 0;
            }


            adaptiveRatio_hwReport_IIR = (6*adaptiveRatio_hwReport_IIR + 2*adaptiveRatio_hwReport)>>3;
            u8DetailGain_hw = ((19584*(gridReport_percent+percentage_offset)+128)>>8) - 9511;
            u8DBK_coase_TH =((5120*(gridReport_percent+percentage_offset)+128)>>8) - 2048;
            /*
            if(log_en ==1)
            {
             //printf(" Max bin :%05d width:%5d u32AvergeStrength:%5d\n", u8MaxBin,u8Widdth,u32AvergeStrength);
             //printf("left:%5d right:%5d\n",s8Boundary_left,u8Boundary_right);
             //printf("total_bin:%d u16Width:%d\n",u32TotalStrength,u16Width);
             //printf("grid_report :%x non-grid report:%x bin:%d\n",u32GridReport_sum,u32Non_gridReport_sum,u8MaxBin);
             //printf("gridReport_percent:%d u32GridReport_sumIIR:%d u32Non_gridReport_sumIIR:%d\n",(int)(100*gridReport_percent),u32GridReport_sumIIR,u32Non_gridReport_sumIIR);
            }
            */
             //int strDiff = max( 0, (maxStr-avgStr) );
             //adaptiveRatio_cur = (float)(strDiff)/(float)avgStr;
             //adaptiveRatio = adaptiveRatio_cur;
             //int binCnt = 1;
             //old method
             //adaptiveRatioIIR = (adaptiveRatioIIR*13 + adaptiveRatio*3 ) / 16;
             //dbkGain = (int)(8 + 8*adaptiveRatioIIR);
             //detailGain = max(6-(int)6*adaptiveRatioIIR, 0);
             //STD_Th = minmax((int)(63*adaptiveRatioIIR), 0x3F, 0xFF);

             //**new method with HW Accelerator**
            u8DBKGain_hw = (8 + ((8*adaptiveRatio_hwReport_IIR +128)>>8));
            u8DBK_coase_TH = minmax(((u8DBK_coase_TH +128)>>8), 0x02, 0x06);
            u8DetailGain_hw = minmax(((u8DetailGain_hw +128)>>8), 0x02, 0x0A);
            u8STD_Threshold_hw = minmax(((63*adaptiveRatio_hwReport_IIR +128)>>8), 0x3F, 0xFF);
            u8DBKGain_hw = minmax(u8DBKGain_hw, 0x8, 0xF);

            if(u8Widdth == 21 ||u8Widdth == 16)//Scaling up SD2HD
            {
             u8STD_Threshold_hw = 0xBF ;
             u8DBKGain_hw = 0x0F;
             u8DBK_coase_TH = 0x0A;
             u8DetailGain_hw = 0x0F;
             gridReport_percent = 179; // 0.7;
            }


            if(log_en ==1)
            {
             printk("percentage_offset:%d\n",percentage_offset);
             printk("adaptiveRatio_hwReport:[%lld]\n",adaptiveRatio_hwReport_IIR);
             printk("u8DBKGain_hw:[%3d] u8DBK_coase_TH:[%3d]\n",u8DBKGain_hw,u8DBK_coase_TH);
             printk("u8DetailGain_hw:[%3d]\n ",u8DetailGain_hw);
             printk("u8STD_Threshold_hw:[%3d]\n ",u8STD_Threshold_hw);
            }

             NoiseMaskingControl(gridReport_percent, isSceneChangeY);
             MHal_XC_W2BYTEMSK(REG_SC_BK0C_11_L, u8DBKGain_hw, 0x000F); //DBK Gain
             MHal_XC_W2BYTEMSK(REG_SC_BK0C_10_L, u8STD_Threshold_hw<<8, 0xFF00); //DBK STD threshold
             MHal_XC_W2BYTEMSK(REG_SC_BK0C_40_L, (u8DBK_coase_TH<<8), 0xFF00); //DBK coase threshold
             MHal_XC_W2BYTEMSK(REG_SC_BK0C_42_L, (u8DetailGain_hw<<12), 0xF000); //blockness gain
             MHal_XC_W2BYTEMSK(REG_SC_BK60_14_L, 0x000F, 0x000F); //User coring threshold
             MHal_XC_W2BYTEMSK(REG_SC_BK60_30_L, 0x0008, 0x0008); //cont3
             MHal_XC_W2BYTEMSK(REG_SC_BK0C_41_L, 0x0000, 0x0001); //hd mode disable
             MHal_XC_W2BYTEMSK(REG_SC_BK0C_71_L, 0x0000, 0x00C0); //switch to Y 5tap filter
             MHal_XC_W2BYTEMSK(REG_SC_BK0C_72_L, 0x0002, 0x0006); //smooth step 1
             MHal_XC_W2BYTEMSK(REG_SC_BK0C_02_L, 0x0002, 0x0006); //switch to C 3tap filter
             MHal_XC_W2BYTEMSK(REG_SC_BK0C_42_L, 0x0904, 0x0F87); //blockness extend disable/switch pulse filter
             if(u8Widdth == 21 ||u8Widdth == 16)//Scaling up SD2HD
             {
                 MHal_XC_W2BYTEMSK(REG_SC_BK0C_41_L, 0x0001, 0x0001); //hd mode enable
                 MHal_XC_W2BYTEMSK(REG_SC_BK0C_71_L, 0x00C0, 0x00C0); //switch to Y 7tap filter
                 MHal_XC_W2BYTEMSK(REG_SC_BK0C_02_L, 0x0006, 0x0006); //switch to C 7tap filter
                 MHal_XC_W2BYTEMSK(REG_SC_BK0C_72_L, 0x0000, 0x0006); //smooth step 0
                 MHal_XC_W2BYTEMSK(REG_SC_BK0C_42_L, 0x0A86, 0x0F87); //blockness extend enable/switch pulse filter
             }


        }
        u8CountForDBK++;
        u8CountForSample++;

    }
    if (u8CountForDBK >= 2)
    {
        u8CountForDBK = 0;
    }
}
/*
#if (Auto_DeBlock_En)
#define ADBW_HD_Loop_Ini_Start 40
#define ADBW_HD_Max_Range_End 1440
#define ADBW_SD_Loop_Ini_Start 20
#define ADBW_SD_Max_Range_End  720
//static MS_U32 Auto_DBK_Width_val = 8;
//static MS_U32 Auto_DBK_Width_val_tmp = 32; // format is xxxxx.xx
//static MS_U32 Pre_DBK_Width_val = 8;
#endif
*/
static MS_U16 g_u16BK0A_24 = 0;
static MS_U16 g_u16BK0C_10 = 0;
static MS_U16 g_u16BK0C_39 = 0;
static MS_U16 g_u16BK0C_3A = 0;
static MS_U16 g_u16BK0C_40 = 0;
static MS_U16 g_u16BK0C_41 = 0;
static MS_U16 g_u16BK0C_42 = 0;
static MS_U16 g_u16BK26_10 = 0;
static MS_U16 g_u16BK26_11 = 0;
static MS_U16 g_u16BK26_12 = 0;
static MS_U16 g_u16BK26_13 = 0;
static MS_U16 g_u16BK26_18 = 0;

/******************************************************************************/
///Film mode control for Any Candence
///@param bEnable \     IN: Enable/Disable control
/******************************************************************************/
void Hal_PQ_FilmMode_AnyCandence_Enable(MS_BOOL bEnable)
{
    MHal_XC_W2BYTEMSK(REG_SC_BK0A_24_L, (bEnable ? (BIT(15)) : 0), BIT(15));
    bg_bAnyCandenceEnable = bEnable;
}

/******************************************************************************/
///Store the Current value
/******************************************************************************/
void Hal_PQ_StoreCurrentValue(void)
{
    //for AnyCandence Film Mode
    g_u16BK0A_24 = MHal_XC_R2BYTEMSK(REG_SC_BK0A_24_L, BIT(15));

    //for DBK
    g_u16BK0C_10 = MHal_XC_R2BYTE(REG_SC_BK0C_10_L);
    g_u16BK0C_39 = MHal_XC_R2BYTE(REG_SC_BK0C_39_L);
    g_u16BK0C_3A = MHal_XC_R2BYTE(REG_SC_BK0C_3A_L);
    g_u16BK0C_40 = MHal_XC_R2BYTE(REG_SC_BK0C_40_L);
    g_u16BK0C_41 = MHal_XC_R2BYTE(REG_SC_BK0C_41_L);
    g_u16BK0C_42 = MHal_XC_R2BYTE(REG_SC_BK0C_42_L);

    //for DMS
    g_u16BK26_10 = MHal_XC_R2BYTE(REG_SC_BK26_10_L);
    g_u16BK26_11 = MHal_XC_R2BYTE(REG_SC_BK26_11_L);
    g_u16BK26_12 = MHal_XC_R2BYTE(REG_SC_BK26_12_L);
    g_u16BK26_13 = MHal_XC_R2BYTE(REG_SC_BK26_13_L);
    g_u16BK26_18 = MHal_XC_R2BYTE(REG_SC_BK26_18_L);
}


/******************************************************************************/
///Store the Current value
/******************************************************************************/
void Hal_PQ_ReStoreToDefault(void)
{
    //for AnyCandence Film Mode
    MHal_XC_W2BYTEMSK(REG_SC_BK0A_24_L, g_u16BK0A_24, BIT(15));

    //for DBK
    MHal_XC_W2BYTE(REG_SC_BK0C_10_L, g_u16BK0C_10);
    MHal_XC_W2BYTE(REG_SC_BK0C_39_L, g_u16BK0C_39);
    MHal_XC_W2BYTE(REG_SC_BK0C_3A_L, g_u16BK0C_3A);
    MHal_XC_W2BYTE(REG_SC_BK0C_40_L, g_u16BK0C_40);
    MHal_XC_W2BYTE(REG_SC_BK0C_41_L, g_u16BK0C_41);
    MHal_XC_W2BYTE(REG_SC_BK0C_42_L, g_u16BK0C_42);

    //for DMS
    MHal_XC_W2BYTE(REG_SC_BK26_10_L, g_u16BK26_10);
    MHal_XC_W2BYTE(REG_SC_BK26_11_L, g_u16BK26_11);
    MHal_XC_W2BYTE(REG_SC_BK26_12_L, g_u16BK26_12);
    MHal_XC_W2BYTE(REG_SC_BK26_13_L, g_u16BK26_13);
    MHal_XC_W2BYTE(REG_SC_BK26_18_L, g_u16BK26_18);
}

#define FPLL_PHASE_VAR_LIMIT    (0x10)          // If phase error variation exceed this, means un-stable
#define FPLL_PRD_THRESHOLD      (0x100)         // The PRD must be smaller than this value to indicate almost finished PRD lock
#define FPLL_PRD_VAR_LIMIT      (0x10)          // If PRD variation exceed this, means un-stable

static MS_BOOL _Hal_PQ_FrameLockCheck (void)
{
    MS_U16 u16NewPhase,u16NewPrd;
    static MS_U16 _u16OrigPhase,_u16OrigPrd;

    u16NewPrd = MHal_XC_R2BYTEMSK(REG_LPLL_13_L, 0xFFFF);
    u16NewPhase = MHal_XC_R2BYTEMSK(REG_LPLL_11_L, 0xFFFF);

    if((u16NewPrd < FPLL_PRD_THRESHOLD) && (abs(_u16OrigPrd - u16NewPrd) < FPLL_PRD_VAR_LIMIT)
        && (abs(_u16OrigPhase - u16NewPhase) < FPLL_PHASE_VAR_LIMIT))
    {
        _u16OrigPhase = u16NewPhase;
        _u16OrigPrd = u16NewPrd;
        return TRUE;
    }
    else
    {
        _u16OrigPhase = u16NewPhase;
        _u16OrigPrd = u16NewPrd;
        return FALSE;
    }
}
static MS_U16 _u8CurrentSourceType = 0;
static MS_U8 _u8PreSource = 0;
/******************************************************************************/
///Source change from CFD
///@param bEnable \     IN: Enable/Disable control
/******************************************************************************/
void MDrv_SetCurrentSource(MS_U8 u8Type)
{
    _u8PreSource = _u8CurrentSourceType;
    _u8CurrentSourceType = u8Type;
    //printk("\033[44;37m[PQ] %s[%d] ::  %d \033[0m\n",__PRETTY_FUNCTION__ ,__LINE__,_u8CurrentSourceType);
}
MS_U8 MDrv_SourceChange(void)
{
    MS_U8 u8CurrentSource = 0,u8SourceChangeFlag = 0;

    //u8CurrentSource = MApi_XC_R2BYTEMSK(REG_SC_BK01_02_L, 0x0007);
    u8CurrentSource = _u8CurrentSourceType;
    if(u8CurrentSource != _u8PreSource)
    {
        //printk("**************u8CurrentSource:%d**************_u8PreSource:%d**************\n",u8CurrentSource,_u8PreSource);
        u8SourceChangeFlag = 1;
    }
    else
    {
        u8SourceChangeFlag = 0;
    }
    _u8PreSource = u8CurrentSource;
    //printk("u8CurrentSource:%d u8PreSource:%d u8SourceChangeFlag:%d\n",u8CurrentSource,_u8PreSource,u8SourceChangeFlag);
    return u8SourceChangeFlag;
}
/******************************************************************************/
///Force Motion once source change
///@param bEnable \     IN: Enable/Disable control
/******************************************************************************/
#define ForceMotionTH 240
void MDrv_ForceMotion_Control(MS_U8 u8SourceChangeFlag)
{
    static MS_U8 u8ForceMotionFlag = 0;
    static MS_U16 u16ForceFrameCnt = 0;

    if(u8SourceChangeFlag == 1)
    {
        //printk("******u8SourceChangeFlag:%d******\n",u8SourceChangeFlag);
        //printf("**********************************Source Change Force motion******************************\n");
        MHal_XC_W2BYTEMSK(REG_SC_BK22_78_L, 0x8F8F, 0x8F8F);
        u8ForceMotionFlag = 1;
    }

    if(u8ForceMotionFlag == 1)
    {
        if(_Hal_PQ_FrameLockCheck())
        {
            //printk("\033[44;37m[PQ] %s[%d] ::   \033[0m\n",__PRETTY_FUNCTION__ ,__LINE__);
            u16ForceFrameCnt = ForceMotionTH;
        }
        u16ForceFrameCnt++;
        //printf("******u8ForceFrameCnt:%d u8ForceMotionFlag:%d******\n",u16ForceFrameCnt,u8ForceMotionFlag);
    }
    else
    {
        u16ForceFrameCnt = 0;
    }
    //printf("******u8ForceFrameCnt:%d u8ForceMotionFlag:%d******\n",u16ForceFrameCnt,u8ForceMotionFlag);
    if(u16ForceFrameCnt > ForceMotionTH)
    {
        //printk("**********************************Reset motion to default******************************\n");
        MHal_XC_W2BYTEMSK(REG_SC_BK22_78_L, 0x0F00, 0x8F8F);
        u8ForceMotionFlag = 0;
    }
}

static MS_BOOL bFakeOutCusEnable = TRUE;

MS_BOOL Hal_PQ_SetFakeOutEnable(MS_BOOL bEnable)
{
    bFakeOutCusEnable = bEnable;
    return TRUE;
}

#define ENABLE_SCALER_DEFEATHERING           0x01   //BK22_7C_L
#define ENABLE_SCALER_DEFLICKERING           0x02
#define ENABLE_SCALER_DEBOUNCING             0x04
#define ENABLE_SCALER_MCNR_DRIVER            0x08
#define ENABLE_SCALER_T12_UCDI_DRIVER        0x10
#define ENABLE_SCALER_NEW_SDHD_DETECT_DRIVER 0x20
#define ENABLE_SCALER_AUTO_DBK_HD_DRIVER     0x40
#define ENABLE_SCALER_AUTO_DBK_DRIVER        0x80

//-------------------------------------------------------------------------------------------------
/// this function does adaptive tuning periodic
//-------------------------------------------------------------------------------------------------
void MHal_PQ_Adaptive_Tuning(void)
{
    #if 0   //check driver run time
    static MS_U32 u32lastTimeStamp=0;
    MS_U32 u32nowTimeStamp;
    u32nowTimeStamp = MsOS_GetSystemTime();
    if(u32nowTimeStamp-u32lastTimeStamp>16)
        printf("====executing time[%d]====\n",(u32nowTimeStamp-u32lastTimeStamp));
    u32lastTimeStamp=u32nowTimeStamp;
    #endif

    static MS_U8 u8PreviousVcnt = 0;
    MS_U8 u8CurrentVcnt;
    MS_U8  u8Ctrl,u8MROffset;
    MS_U32 u32MotionValue,u32MotionValue2,u32OOSRMC,u32OOSRF,u32ISRNONMV0,u32SmallMoving,u32VerticalMoving,u32MotionValue4;//u32MotionValue3,
    MS_U32 u32FeatherSum,u32MotionValue_Sum;
    MS_U8 u8MotionValue3,u8SourceChangeFlag;
    XC_ApiStatus stXCStatus;
    MS_BOOL isSceneChangeY = 0;// MDrv_SC_Agate_SceneChangeY();
    XC_LumaHistogramStates lumaHistogramStates;

    memset(&stXCStatus, 0x00, sizeof(stXCStatus));

    bisHDSource = ( MHal_XC_R2BYTEMSK(REG_SC_BK12_0F_L, 0xFFFF) > 0x700 );
    //
    // Check VCnt
    //
    u8CurrentVcnt = MDrv_SC_read_v_cnt();
    if (u8PreviousVcnt == u8CurrentVcnt)
    {
        return;
    }
    else
    {
        u8PreviousVcnt = u8CurrentVcnt;
    }

    u8SourceChangeFlag = MDrv_SourceChange();
    lumaHistogramStates = MDrv_SC_GetLumaHistogram_PQ();
    isSceneChangeY = MDrv_SC_Agate_SceneChangeY_PQ(lumaHistogramStates);
    //
    // Get adaptive function control
    //
    u8Ctrl  = MDrv_SC_get_adaptive_ctrl();
    //u8Ctrl2 = MDrv_SC_get_adaptive_ctrl2();
    //u8Ctrl3 = MDrv_SC_get_adaptive_ctrl3();
    //
    // Get motion value
    //
    u32MotionValue = MDrv_SC_read_motion_value1();
    u32MotionValue2 = MDrv_SC_read_motion_value2();
    u8MotionValue3 = MDrv_SC_read_motion_value3();
    u32MotionValue4 = MDrv_SC_read_motion_value4();
    //
    // Get UCDi status
    //
    u32OOSRMC = MDrv_SC_T12_UC_InSearchRange_MvComfirm();
    u32OOSRF = MDrv_SC_T12_UC_InSearchRange_Feather();
    u32ISRNONMV0 = MDrv_SC_T12_UC_InSearchRange_nonMv0Comfirm();
    u32SmallMoving = MDrv_SC_T12_UC_SmallMoving();
    u32VerticalMoving = MDrv_SC_A5_KFC_IsVerticalMoving();
    //u32HorizontalMoving = MDrv_SC_A5_IsHorizontalMoving();
    //u32TotalFeather = MDrv_SC_A5_TotalFeather();
    u32FeatherSum = MDrv_SC_TotalFeather_Sum();
    //u32TotalMvComfirm = MDrv_SC_A5_TotalMvComfirm();
    //u8ColorSum = MDrv_SC_A5_ColorPixelSum();
    //u32uComplex = MDrv_SC_A5_ComplexSum();
    u8MROffset = MDrv_SC_UC_InSearchRange_MR_Offset();
    u32MotionValue_Sum = u32MotionValue2 + u8MROffset;

    /*
    // Adaptive functions
    if(_u16PQPre_SrcType != _u16PQSrcType_DBK_Detect[PQ_MAIN_WINDOW])            //For Auto_DBK SW driver used
    {
        _u16PQPre_SrcType = _u16PQSrcType_DBK_Detect[PQ_MAIN_WINDOW];
        bdataread = true;
    }
    */
    /******************************************************************************/
    ///Auto DBK Driver
    /******************************************************************************/
    if ((u8Ctrl & ENABLE_SCALER_AUTO_DBK_DRIVER)||(u8Ctrl & ENABLE_SCALER_AUTO_DBK_HD_DRIVER))   //New Add for AUTO DBK Test
    {
        if(MApi_XC_GetStatus(&stXCStatus, MAIN_WINDOW) == FALSE)
        {
            return;//printk("MApi_XC_GetStatus failed because of InitData wrong, please update header file and compile again\n");
        }
         Mdrv_PQ_DBK(isSceneChangeY,u32MotionValue4,stXCStatus.stCropWin.width);
    }

    /******************************************************************************/
    ///UCDi Driver
    /******************************************************************************/
    if (u8Ctrl & ENABLE_SCALER_T12_UCDI_DRIVER)
    {
        MDrv_SC_T12_UCDi_FeatherValueGain(u32OOSRMC, u32OOSRF, u32ISRNONMV0);
        MDrv_SC_T12_UCDi_VerticalMovingUdRatio(u32OOSRMC, u32OOSRF, u32ISRNONMV0);
        MDrv_SC_T12_UCDi_ComplexOffset(u32OOSRMC, u32OOSRF, u32ISRNONMV0);
        MDrv_SC_T12_UCDi_W2SadGain(u32OOSRMC, u32OOSRF, u32ISRNONMV0);
        MDrv_SC_T12_KFC_8bitMotionGain(u32SmallMoving);
        MDrv_SC_T12_UCDi_EODiWGain(u32OOSRMC, u32OOSRF, u32ISRNONMV0);
        MDrv_SC_T12_UCDi_HistoryRatio(u32OOSRMC, u32OOSRF, u32ISRNONMV0);
        MDrv_SC_A5_UCDi_OP1_HisWeight(u32MotionValue_Sum,u32FeatherSum,u8MotionValue3);
        MDrv_SC_A5_KFC_EODiW(u32VerticalMoving);
        MDrv_SC_SST_Ctrl(u32FeatherSum, bisHDSource);
        MDrv_ForceMotion_Control(u8SourceChangeFlag);
    }

    /******************************************************************************/
    ///UCNR Driver
    /******************************************************************************/

    if (u8Ctrl & ENABLE_SCALER_MCNR_DRIVER)
    {
        MDrv_SC_mcnr_driver(u32MotionValue2, bisHDSource);
    }

    /******************************************************************************/
    ///SDHD Driver
    /******************************************************************************/

    if (u8Ctrl & ENABLE_SCALER_NEW_SDHD_DETECT_DRIVER)
    {
        MDrv_SC_NEW_SDHD_DETECT_driver();
    }


    /******************************************************************************/
    ///MADi Driver
    /******************************************************************************/

    if (u8Ctrl & ENABLE_SCALER_DEFEATHERING)
    {
        MDrv_SC_de_feathering(u32MotionValue);
    }

    if (u8Ctrl & ENABLE_SCALER_DEFLICKERING)
    {
        MDrv_SC_de_flickering(u32MotionValue);
    }

    if (u8Ctrl & ENABLE_SCALER_DEBOUNCING)
    {
        MDrv_SC_de_bouncing(u32MotionValue2);
        MDrv_SC_A5_UCDi_OP1_HisWeight(u32MotionValue_Sum,u32FeatherSum,u8MotionValue3);
    }

    if( (MHal_XC_R2BYTEMSK(REG_SC_BK0A_10_L, BIT(14)) == BIT(14))
            || (MHal_XC_R2BYTEMSK(REG_SC_BK0A_10_L, BIT(15)) == BIT(15)) )
    {
        if(!_Hal_PQ_FrameLockCheck()) // Make not frame LOCKED then turn off FilmMode
        {
            MHal_XC_W2BYTEMSK(REG_SC_BK0A_1E_L, BIT(1), BIT(1)); //hardware film mode DISABLE
            MHal_XC_W2BYTEMSK(REG_SC_BK0A_21_L, BIT(1)|BIT(2), BIT(1) | BIT(2)); //software film mode DISABLE
        }
        else    // Make sure frame LOCKED then turn on FilmMode
        {
            if(bFakeOutCusEnable)
            {
                MHal_XC_W2BYTEMSK(REG_SC_BK0A_1E_L, 0, BIT(1)); //hardware film mode ENABLE
                MHal_XC_W2BYTEMSK(REG_SC_BK0A_21_L, 0, BIT(1) | BIT(2)); //software film mode ENABLE
            }
            else
            {
                MHal_XC_W2BYTEMSK(REG_SC_BK0A_1E_L, BIT(1), BIT(1)); //hardware film mode DISABLE
                MHal_XC_W2BYTEMSK(REG_SC_BK0A_21_L, BIT(1)|BIT(2), BIT(1) | BIT(2)); //software film mode DISABLE
            }
        }
    }
}
void MHal_PQ_Adaptive_Top(void)
{
    MHal_PQ_Adaptive_Tuning();

    //menuLoad Test
    /*MApi_GFLIP_XC_W2BYTEMSK(REG_FSC_BK30_06_L, 0x0043 ,0xFFF0);
    KApi_XC_MLoad_WriteCmd_NonXC(E_CLIENT_MAIN_HDR,0x1430, 0x0004,0x0042 ,0x0070);
    KApi_XC_MLoad_WriteCmd(E_CLIENT_MAIN_HDR,REG_SC_BK30_01_L, 0x4300 ,0xFF00);
    MLoad_UFSC_W2BYTEMSK(REG_FSC_BK30_04_L, 0x0042 ,0xFFF0);*/
    //KApi_XC_MLoad_Fire(E_CLIENT_MAIN_HDR, TRUE);

}

void MHal_Run_PQ_Adaptive(void)
{

    MS_U16 u16IRQValue1 = MHal_XC_R2BYTE(REG_SC_BK00_10_L);
    if(u16IRQValue1 & BIT(4))
    {
        MHal_PQ_Adaptive_Top();
    // clear ISR
        MHal_XC_W2BYTEMSK(REG_SC_BK00_12_L, BIT(4), BIT(4));
    }

    //MHal_PQ_Adaptive_Top();

    /*static struct timespec NowTS;
    static struct timespec previousTS;

    getnstimeofday(&NowTS);

    long diff = NowTS.tv_sec * 1000000 + NowTS.tv_nsec/1000 - previousTS.tv_sec * 1000000 - previousTS.tv_nsec/1000;
    printk("t_diff = %d ms\n",diff);
    previousTS = NowTS;*/



}
#endif
