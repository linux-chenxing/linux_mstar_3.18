//<MStar Software>
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// * Copyright (c) 2010 - 2012 Mstar Semiconductor, Inc.
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
/// file    mdrv_ldm_init.c
/// @brief  local dimming Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////

#if (defined (CONFIG_HAS_LD) || defined (CONFIG_HAS_LD_MODULE))

#include <linux/module.h>
#include <linux/types.h>
#include <asm/io.h>
#include <mstar/mstar_chip.h>
#include "mst_devid.h"
#include <linux/slab.h>
#include <linux/vmalloc.h>


#include "mdrv_mstypes.h"
#include "mdrv_ldm_io.h"
#include "mdrv_ldm_common.h"
#include "mdrv_ldm_init.h"
#include "mdrv_ldm_parse.h"
#include "mhal_ldm.h"
#include "reg_ldm.h"



#ifndef UNUSED
#define UNUSED(x)  ((x) = (x))
#endif

#ifndef MSTAR_MIU2_BUS_BASE
#define MSTAR_MIU2_BUS_BASE ((size_t)-1)
#endif



#define GETMIUOFFSET(addr, Offset) \
    if((addr) >= ARM_MIU2_BASE_ADDR) \
    {   \
        Offset = MSTAR_MIU2_BUS_BASE - ARM_MIU2_BASE_ADDR; \
    }   \
    else if((addr) >= ARM_MIU1_BASE_ADDR) \
    {   \
        Offset = MSTAR_MIU1_BUS_BASE - ARM_MIU1_BASE_ADDR; \
    }   \
    else \
    {   \
        Offset = MSTAR_MIU0_BUS_BASE - ARM_MIU0_BASE_ADDR; \
    }


#define LD_CUS_FILE                 "/config/ldm/"
#define LD_CUS_FILE_U               "/mnt/usb/sda1/ldm/"

#define LD_BIN_LENGTH               0x80000     //bin buffer
#define LD_BIN_OFFSET               0x20        //bin header length
#define LDMMAPLENGTH                0x100000    //mmap length
#define LDFALIGN                    8           //align bits
#define RATIOSHIFT                  16          //
#define LDFSHIFT                    20          //
#define LD_COMPENSATION_LENGTH      0x200       //Compensation.bin length

#ifdef LD_SUPPORT_1XN_NX1
MS_BOOL bLed1xN = FALSE;
MS_BOOL bLedNx1 = FALSE;
#endif


ST_DRV_LD_INFO* psDrvLdInfo = NULL;
ST_DRV_LD_PQ_INFO* psDrvLdPQInfo = NULL;

ST_DRV_LD_INFO stDrvLdInfo;
ST_DRV_LD_PQ_INFO stDrvLdPQInfo;
ST_DRV_LD_MISC_INFO stDrvLdMiscInfo;
EN_LD_DEBUG_LEVEL enDbgLevel = E_LD_DEBUG_LEVEL_ERROR;//EN_DEBUG_LEVEL_ALL;
ST_DRV_LD_CUS_PATH stCusPath;
ST_DRV_LD_CUS_PATH stCusPathBck;
MS_U32 LDF_mem_size = 0;

//=============================================================================
// Global Variables
//=============================================================================
void __iomem * u64Vaddr = 0;
MS_PHY ld_addr_base;
MS_U8 u8LdInitProtectFlag;

//=============================================================================
// Extern Variables
//=============================================================================
extern MS_BOOL gbLdInit;
extern MS_U8 u8LdSuspend;



extern void MDrv_LD_Enable(MS_BOOL bOn, MS_U8 u8Luma);
extern void MDrv_LD_SetupGammaTable(MS_U8* u8GammaBlocks);
extern MS_S8 MDrv_LD_PrepareBuffs(void);


MS_BOOL MDrv_LD_GetLinearEn(void)
{
    return stDrvLdPQInfo.bSFLinearEn;
}

static MS_U16 MDrv_LD_PreInitVariable(void)
{
    MS_BOOL bRet = FALSE;
    memset((void *)&stDrvLdInfo,0,sizeof(ST_DRV_LD_INFO));
    memset((void *)&stDrvLdPQInfo,0,sizeof(ST_DRV_LD_PQ_INFO));
    memset((void *)&stDrvLdMiscInfo,0,sizeof(ST_DRV_LD_MISC_INFO));

    if((0 == strlen(stCusPath.aCusPath)) || (0 == strlen(stCusPath.aCusPathU)))
    {
        LD_INFO(" trace! using default ini and bin path \n");
        strcpy(stCusPath.aCusPath,LD_CUS_FILE);
        strcpy(stCusPath.aCusPathU,LD_CUS_FILE_U);
        strncat(stCusPath.aCusPath,"ldm.ini",7);
        strncat(stCusPath.aCusPathU,"ldm.ini",7);
    }
    else if(strchr(stCusPath.aCusPath, '.'))
    {
        LD_INFO(" customer path:%s, customer u_path:%s  \n", stCusPath.aCusPath, stCusPath.aCusPathU);

        // load "ldm.ini"
        //strlen("BacklightGamma.bin") is 18, replace "BacklightGamma.bin" with "ldm.ini"
        strcpy((stCusPath.aCusPath+strlen(stCusPath.aCusPath)-18),"ldm.ini");
        strcpy((stCusPath.aCusPathU+strlen(stCusPath.aCusPathU)-18),"ldm.ini");
    }
    else
    {
        strncat(stCusPath.aCusPath,"ldm.ini",7);
        strncat(stCusPath.aCusPathU,"ldm.ini",7);
    }
    //strncat(stCusPath.aCusPath,"ldm.ini",7);
    //strncat(stCusPath.aCusPathU,"ldm.ini",7);

    bRet = MDrv_LD_ParseIni(stCusPath.aCusPath, stCusPath.aCusPathU);
    if(FALSE == bRet)
    {
        return E_LD_STATUS_PARAMETER_ERROR;
    }

    psDrvLdInfo = &stDrvLdInfo;
    psDrvLdPQInfo = (ST_DRV_LD_PQ_INFO*)&stDrvLdPQInfo;

    //misc
    if(E_LD_DEBUG_LEVEL_MAX <= stDrvLdMiscInfo.u8DebugLevel)
    {
        stDrvLdMiscInfo.u8DebugLevel = E_LD_DEBUG_LEVEL_ALL;
    }
    else if(E_LD_DEBUG_LEVEL_ERROR > stDrvLdMiscInfo.u8DebugLevel)
    {
        stDrvLdMiscInfo.u8DebugLevel = E_LD_DEBUG_LEVEL_ERROR;
    }

    if ((stDrvLdMiscInfo.bDbgEn)&&
        ((0 < stDrvLdMiscInfo.u8DebugLevel)&&(E_LD_DEBUG_LEVEL_MAX > stDrvLdMiscInfo.u8DebugLevel)))
    {
        enDbgLevel = stDrvLdMiscInfo.u8DebugLevel;
    }

    return E_LD_STATUS_SUCCESS;
}


MS_U16 MDrv_LD_InitLDF(void)
{
    MS_U32 LDF_pack_per_row;

    if(stDrvLdMiscInfo.u8ClkHz == 120)
    {
    LDF_pack_per_row = ((psDrvLdPQInfo->u8LDFWidth/2 *4 - 1) / psDrvLdInfo->u16PackLength) + 1;
    }
    else
    {
    LDF_pack_per_row = ((psDrvLdPQInfo->u8LDFWidth *4 - 1) / psDrvLdInfo->u16PackLength) + 1;
    }
    LDF_mem_size = LDF_pack_per_row * psDrvLdInfo->u16PackLength * psDrvLdPQInfo->u8LDFHeight;

    MHal_LD_SetLDFBacklightWidth(psDrvLdPQInfo->u8LDFWidth);
    MHal_LD_SetLDFBacklightHeight(psDrvLdPQInfo->u8LDFHeight);

    MHal_LD_SetInputHScalingDownRatio(((MS_U32)psDrvLdPQInfo->u8LDFWidth << LDFSHIFT)/psDrvLdInfo->u16PanelWidth + 1); // W_ledx1024x1024/W_lcd + 1
    MHal_LD_SetInputVScalingDownRatio(((MS_U32)psDrvLdPQInfo->u8LDFHeight << LDFSHIFT)/psDrvLdInfo->u16PanelHeight + 1); // H_ledx1024x1024/H_lcd + 1

    MHal_LD_SetLDFFrameBufBaseAddr_0((psDrvLdInfo->u32BaseAddr) / psDrvLdInfo->u16PackLength);
    LDF_mem_size = ((LDF_mem_size + 0xFF) >> LDFALIGN) << LDFALIGN; // align at 0x100
    MHal_LD_SetLDFFrameBufBaseAddr_1((psDrvLdInfo->u32BaseAddr + LDF_mem_size) / psDrvLdInfo->u16PackLength);
    if(stDrvLdMiscInfo.u8ClkHz == 120)
    {
    MHal_LD_SetLDFFrameBufBaseAddr_R0((psDrvLdInfo->u32BaseAddr + 2*LDF_mem_size) / psDrvLdInfo->u16PackLength);
    MHal_LD_SetLDFFrameBufBaseAddr_R1((psDrvLdInfo->u32BaseAddr + 3*LDF_mem_size) / psDrvLdInfo->u16PackLength);
    }
    MHal_LD_SetLDFHLPFEn(TRUE);
    MHal_LD_SetLDFHLPFThreshold(psDrvLdPQInfo->u8NRStrength);
    MHal_LD_SetLDFHLPFStep(0);

    MHal_LD_SetLDFDCMaxAlpha(psDrvLdPQInfo->u8DCMaxAlpha);
    MHal_LD_SetLDFWriteDCMaxEn(DISABLE);
    MHal_LD_SetLDFWriteDCMaxOFEn(ENABLE);
    MHal_LD_SetLDFPixelWhiteModeEn(ENABLE);
    LD_INFO(" trace!  u8LDFWidth:0x%x, u8LDFHeight:0x%x, u32BaseAddr:0x%x \n",
        psDrvLdPQInfo->u8LDFWidth, psDrvLdPQInfo->u8LDFHeight, (MS_U32)(psDrvLdInfo->u32BaseAddr));

    return E_LD_STATUS_SUCCESS;
}

MS_U16 MDrv_LD_InitLDB(void)
{
    MS_U16 u16HRatio = (MS_U16)MIN((((MS_U32)psDrvLdPQInfo->u8LEDWidth)<<RATIOSHIFT) / psDrvLdPQInfo->u8LSFWidth, 0xFFFF);
    MS_U16 u16VRatio = (MS_U16)MIN((((MS_U32)psDrvLdPQInfo->u8LEDHeight)<<RATIOSHIFT) / psDrvLdPQInfo->u8LSFHeight, 0xFFFF);
    MS_U32 LDF_pack_per_row = 0;
    MS_U8 u8LDBWidth = 0;
    MS_U16 u16GuardBand = 0;
    MS_U32 u32LSFAccMaster = 0;
    MS_U32 u32LSFMasterLast = 0;
    MS_U32 u32LSFAccSlave = 0;
    MS_U32 u32LSFSlaveFirst = 0;
    MS_U32 u32LSFPhaseH2 = 0;
    MS_U32 u32LEDAccSlave = 0;
    MS_U32 u32LEDSlaveFirst = 0;
    MS_U32 u32LEDPhaseH2 = 0;
    MS_U8 u8LDBHeight = 0;
    MS_U32 LDF_mem_size = 0;
    MS_U32 LDB_pack_per_row = 0;
    MS_U32 LDB_mem_size = 0;
    MS_U32 EDGE2D_mem_size = 0;
    MS_U32 u32LDBBaseAddr = 0;
    MS_U32 u32Edge2DBaseAddr = 0;
    MS_U32 u32LSFRatioH = 0;
    MS_U32 u32LSFRatioV = 0;

    if(stDrvLdMiscInfo.u8ClkHz == 120)
    {
     LDF_pack_per_row = ((psDrvLdPQInfo->u8LDFWidth/2 *4 - 1) / psDrvLdInfo->u16PackLength) + 1;
     u8LDBWidth = psDrvLdPQInfo->bEdge2DEn
                  ? (psDrvLdPQInfo->enLEDType == E_LD_LED_EDGE_LR_TYPE ? psDrvLdPQInfo->u8LEDHeight: psDrvLdPQInfo->u8LEDWidth)
                  : psDrvLdPQInfo->u8LEDWidth/2 + 1;
    }
    else
    {
     LDF_pack_per_row = ((psDrvLdPQInfo->u8LDFWidth *4 - 1) / psDrvLdInfo->u16PackLength) + 1;
     u8LDBWidth = psDrvLdPQInfo->bEdge2DEn
                  ? (psDrvLdPQInfo->enLEDType == E_LD_LED_EDGE_LR_TYPE ? psDrvLdPQInfo->u8LEDHeight: psDrvLdPQInfo->u8LEDWidth)
                  : psDrvLdPQInfo->u8LEDWidth;
    }
    u8LDBHeight = psDrvLdPQInfo->bEdge2DEn
                   ? (psDrvLdPQInfo->enLEDType == E_LD_LED_EDGE_LR_TYPE ? psDrvLdPQInfo->u8LEDWidth : psDrvLdPQInfo->u8LEDHeight)
                   : psDrvLdPQInfo->u8LEDHeight;
    LDF_mem_size = LDF_pack_per_row * psDrvLdInfo->u16PackLength * psDrvLdPQInfo->u8LDFHeight;
    LDB_pack_per_row = (psDrvLdPQInfo->bEdge2DEn && psDrvLdPQInfo->enLEDType == E_LD_LED_LOCAL_TYPE) ? 2 :
                                ((u8LDBWidth - 1) / psDrvLdInfo->u16PackLength) + 1;
    LDB_mem_size = LDB_pack_per_row * psDrvLdInfo->u16PackLength * u8LDBHeight;
    EDGE2D_mem_size = (psDrvLdPQInfo->u8LSFWidth * psDrvLdPQInfo->u8LSFHeight) * psDrvLdInfo->u16PackLength;
    u32LSFRatioH = ((MS_U32)psDrvLdPQInfo->u8LSFWidth<<LDFSHIFT) / (psDrvLdInfo->u16PanelWidth - 1); // (W_led + 1)x1024x1024/(W_lcd - 1)
    u32LSFRatioV = ((MS_U32)psDrvLdPQInfo->u8LSFHeight<<LDFSHIFT) / (psDrvLdInfo->u16PanelHeight - 1); // H_ledx1024x1024/(H_lcd - 1)
    if(stDrvLdMiscInfo.u8ClkHz == 120)
    {
    u16GuardBand = MHal_LD_LR_GetGuradbandDeEn() ? (MHal_LD_LR_GetGuradbandXEdL - MHal_LD_LR_GetGuradbandXStL + 1)*2 - psDrvLdInfo->u16PanelWidth/2 : 0;
    u32LSFAccMaster = (psDrvLdPQInfo->u32LsfInitialPhase_H + (psDrvLdInfo->u16PanelWidth/2 + u16GuardBand - 1)*u32LSFRatioH);
    u32LSFMasterLast = (u32LSFAccMaster >> LDFSHIFT);
    u32LSFAccSlave = (psDrvLdPQInfo->u32LsfInitialPhase_H + (psDrvLdInfo->u16PanelWidth/2 - u16GuardBand)*u32LSFRatioH);
    u32LSFSlaveFirst = (u32LSFAccSlave >> LDFSHIFT) - 1;
    u32LSFPhaseH2 = u32LSFAccSlave & 0xFFFFFul;
    u32LEDAccSlave = u16HRatio/2 + u32LSFSlaveFirst*u16HRatio;
    u32LEDSlaveFirst = u32LEDAccSlave >> RATIOSHIFT;
    u32LEDPhaseH2 = u32LEDAccSlave & 0xFFFF;
    }

    MHal_LD_SetLDBBacklightWidth(u8LDBWidth);
    MHal_LD_SetLDBBacklightHeight(u8LDBHeight);
    if(stDrvLdMiscInfo.u8ClkHz == 120)
    {
    MHal_LD_SetLSFBacklightWidth(psDrvLdPQInfo->bEdge2DEn ? psDrvLdPQInfo->u8LSFWidth : psDrvLdPQInfo->u8LEDWidth/2 + 1);
    }
    else
    {
    MHal_LD_SetLSFBacklightWidth(psDrvLdPQInfo->u8LSFWidth);
    }
    MHal_LD_SetLSFBacklightHeight(psDrvLdPQInfo->u8LSFHeight);

    MHal_LD_SetFrameWidth(psDrvLdInfo->u16PanelWidth);
    MHal_LD_SetFrameHeight(psDrvLdInfo->u16PanelHeight);

    LDF_mem_size = (LDF_mem_size + 0xFF) >> LDFALIGN << LDFALIGN; // align at 0x100
    LDB_mem_size = (LDB_mem_size + 0xFF) >> LDFALIGN << LDFALIGN; // align at 0x100
    EDGE2D_mem_size = (EDGE2D_mem_size + 0xFF) >> LDFALIGN << LDFALIGN; // align at 0x100

    if(stDrvLdMiscInfo.u8ClkHz == 120)
    {
    u32LDBBaseAddr = psDrvLdInfo->u32BaseAddr + 4*LDF_mem_size;
    u32Edge2DBaseAddr = u32LDBBaseAddr + 2*LDB_mem_size;
    }
    else
    {
    u32LDBBaseAddr = psDrvLdInfo->u32BaseAddr + 2*LDF_mem_size;
    u32Edge2DBaseAddr = u32LDBBaseAddr + 1*LDB_mem_size;
    }

    MHal_LD_SetLDBFrameBufBaseAddr_0(u32LDBBaseAddr / psDrvLdInfo->u16PackLength);
    MHal_LD_SetLDBFrameBufBaseAddr_1(u32LDBBaseAddr / psDrvLdInfo->u16PackLength);
    if(stDrvLdMiscInfo.u8ClkHz == 120)
    {
    MHal_LD_SetLDBFrameBufBaseAddr_R0((u32LDBBaseAddr + LDB_mem_size) / psDrvLdInfo->u16PackLength);
    MHal_LD_SetLDBFrameBufBaseAddr_R1((u32LDBBaseAddr + LDB_mem_size) / psDrvLdInfo->u16PackLength);
    }
    LD_INFO(" trace!  u32BaseAddr:0x%lx, u32Edge2DBaseAddr:0x%x \n", (unsigned long)psDrvLdInfo->u32BaseAddr, u32Edge2DBaseAddr);
    MHal_LD_SetEdge2DBufBaseAddr((u32Edge2DBaseAddr) / psDrvLdInfo->u16PackLength);

    MHal_LD_SetEdge2DEn(psDrvLdPQInfo->bEdge2DEn);
    MHal_LD_SetEdge2DGainEn(FALSE);
    MHal_LD_SetEdge2DDirectTypeEn(psDrvLdPQInfo->enLEDType == E_LD_LED_DIRECT_TYPE ? ENABLE : DISABLE);
    MHal_LD_SetEdge2DLocalTypeEn(psDrvLdPQInfo->enLEDType == E_LD_LED_LOCAL_TYPE ? ENABLE : DISABLE);

    MHal_LD_SetOutputHScalingUpRatio(u32LSFRatioH); // (W_lsf + 1)x(2^20)/(W_lcd - 1)
    MHal_LD_SetOutputVScalingUpRatio(u32LSFRatioV); // H_lsfx(2^20)/(H_lcd - 1)
    MHal_LD_SetLSFCBHRatio(u16HRatio); // W_ledx(2^16)/W_lsf
    MHal_LD_SetLSFCBVRatio(u16VRatio); // H_ledx(2^16)/H_lsf

    MHal_LD_SetLSFHInitialPhase(psDrvLdPQInfo->u32LsfInitialPhase_H); // need fine tune
    MHal_LD_SetLSFVInitialPhase(psDrvLdPQInfo->u32LsfInitialPhase_V); // need fine tune

    MHal_LD_SetLSFHBoundary(ENABLE); // ENABLE -> duplicate
    MHal_LD_SetLSFVBoundary(ENABLE);
    if(stDrvLdMiscInfo.u8ClkHz == 120)
   {
    LD_INFO(" trace!  u32LSFRatioH:0x%x, u32LSFRatioV:0x%x, u16HRatio:0x%x, u16VRatio:0x%x \n", u32LSFRatioH, u32LSFRatioV,u16HRatio,u16VRatio);
    LD_INFO(" trace!  u32LsfInitialPhase_H:0x%x, u32LsfInitialPhase_V:0x%x, u16PanelWidth:0x%x \n", psDrvLdPQInfo->u32LsfInitialPhase_H, psDrvLdPQInfo->u32LsfInitialPhase_V,psDrvLdInfo->u16PanelWidth);
    LD_INFO(" trace!  u32LSFPhaseH2:0x%x, u32LEDPhaseH2:0x%x, u32LSFMasterLast:0x%x, u32LSFSlaveFirst:0x%x, u32LEDSlaveFirst:0x%x \n",
        u32LSFPhaseH2, u32LEDPhaseH2,u32LSFMasterLast,u32LSFSlaveFirst,u32LEDSlaveFirst);
    MHal_LD_SetLSFHInitialPhase_R(u32LSFPhaseH2);
    MHal_LD_SetLSFHBoundary_R(DISABLE);
    MHal_LD_SetLSFCBHInit(u32LEDPhaseH2);
    MHal_LD_SetLSFMasterLast(u32LSFMasterLast);
    MHal_LD_SetLSFSlaveFirst(u32LSFSlaveFirst);
    //MHal_LD_Set_LED_MasterLast(u32LEDMasterLast);
    MHal_LD_SetLEDSlaveFirst(u32LEDSlaveFirst);
    }
    MHal_LD_SetEdgeLevelShift(0); // 0:normal  1:x2  2:x4
    MHal_LD_SetBlendingDitherEn(TRUE);

    MHal_LD_SetTempFilterMaxSpeed(0x00);
    u8LdInitProtectFlag = 1;

    LD_INFO(" trace!  u8LSFWidth:0x%x, u8LSFHeight:0x%x \n",
        psDrvLdPQInfo->u8LSFWidth, psDrvLdPQInfo->u8LSFHeight);

    return E_LD_STATUS_SUCCESS;
}


MS_U16 MDrv_LD_InitSat(void)
{
    MS_U8 u8SaturationA = psDrvLdPQInfo->u8SaturationA = 0;
    MS_U32 u16SaturationC = psDrvLdPQInfo->u16SaturationC = 0x400;
    MS_U8 u8SaturationCShift = psDrvLdPQInfo->u8SaturationCShift = 0;

    MHal_LD_SetSatA(u8SaturationA);
    MHal_LD_SetSatC(u16SaturationC);
    MHal_LD_SetSatCSft(u8SaturationCShift);

    return E_LD_STATUS_SUCCESS;
}

MS_U16 MDrv_LD_LoadCompensationTable(const MS_U8* pu8CompTable)
{
    MS_U16 u16Lut = 0;
    MS_U16 i = 0;
    //printk("\n++++++++++++++  MDrv_LD_LoadCompensationTable data:\n");
    MHal_LD_SetCompensationMode(FALSE); // TRUE -> incremental mode
    for (i = 0; i < LD_COMPENSATION_LENGTH/2; i++)
    {
        //if(0==(2*i)%16)printk("\n");
        MHal_LD_SetCompensationLUTAddress((MS_U8)i);
        u16Lut = *(pu8CompTable++);
        u16Lut |= (*(pu8CompTable++)) << 8;
        //printk("0x%x\t0x%x\t",u16Lut&0xff,(u16Lut>>8)&0xff);
        MHal_LD_WriteCompensationLUT(u16Lut);
        MHal_LD_SetCompensationWriteEn(ENABLE);
    }

    return E_LD_STATUS_SUCCESS;
}

MS_U16 MDrv_LD_LoadCompensationTable2(const MS_U8* pu8CompTable)
{
    MS_U16 u16Lut = 0;
    MS_U16 i = 0;
    //printk("\n++++++++++++++  MDrv_LD_LoadCompensationTable2 data:\n");
    MHal_LD_SetCompensationLowMode(FALSE); // TRUE -> incremental mode
    for (i = 0; i < LD_COMPENSATION_LENGTH/2; i++)
    {
        //if(0==(2*i)%16)printk("\n");
        MHal_LD_SetCompensationLUTAddressLow((MS_U8)(i));
        u16Lut = *(pu8CompTable++);
        u16Lut |= (*(pu8CompTable++)) << 8;
        //printk("0x%x 0x%x ",u16Lut&0xff,(u16Lut>>8)&0xff);
        MHal_LD_WriteCompensationLowLUT(u16Lut);
        MHal_LD_SetCompensationLowWriteEn(ENABLE);
    }

    return E_LD_STATUS_SUCCESS;
}

/*
Edge 2D Table Format
Normal: Width = LED Number(HxV) / 4
        Height = Backlight size (HxV)

Special(for Edge-LR panel and odd V size)
        Width = (LED Height+1) x V / 4
        Height = Backlight size (HxV)

H-direction: zero padding to Packet Length (16byte)
*/
MS_U16 MDrv_LD_LoadEdge2DTable(const MS_U8* pu8Edge2DTable)
{
    MS_U16 led_width = MHal_LD_GetLEDBacklightWidth();
    MS_U16 led_height = MHal_LD_GetLEDBacklightHeight();
    MS_U16 lsf_width = MHal_LD_GetLSFBacklightWidth();
    MS_U16 lsf_height = MHal_LD_GetLSFBacklightHeight();
    MS_U16 table_width = (((led_width-1)>>1) + 1) * (((led_height-1)>>1) + 1);
    MS_U16 table_height = lsf_width * lsf_height;
    MS_PHY addr_base = MHal_LD_GetEdge2DBufBaseAddr() * psDrvLdInfo->u16PackLength;
    MS_U32 addr_offset = 0;
    MS_U16 i = 0, j = 0;
    MS_U16 pack_length = psDrvLdInfo->u16PackLength;

    if (psDrvLdPQInfo->enLEDType == E_LD_LED_LOCAL_TYPE)
        table_width = 25;

    LD_INFO(" trace!  u64Vaddr:0x%p, edge2Daddr_base:0x%lx, ld_addr_base:0x%lx, pu8Edge2DTable:0x%p \n",
        u64Vaddr,(unsigned long)addr_base,(unsigned long)ld_addr_base, pu8Edge2DTable);
    #if 0
    memcpy((void *)u32Vaddr, (void *)pu8Edge2DTable, table_width*table_height);
    #else
    for (j = 0; j < table_height; j++)
    {
        addr_offset = j * pack_length;
        for (i = 0; i < table_width; i++)
            *((volatile MS_U8*)(u64Vaddr + (addr_base-ld_addr_base) + (addr_offset++))) = *(pu8Edge2DTable++);
            //MDrv_LD_MIUWriteByte(addr_base, addr_offset++, *pu8Edge2DTable++);
    }
    #endif

    return E_LD_STATUS_SUCCESS;
}

MS_U16 MDrv_LD_InitSW(void)
{
    char *buf;
    MS_S8 err = 0;

    //apply buffer for load compensation.bin Edge2D.bin BacklightGamma.bin
    buf = (char *)vmalloc(LD_BIN_LENGTH);
    if(!buf)
    {
        LD_ERROR(" load compensation bin kzalloc fail buf:%p  \n", buf);
        return E_LD_STATUS_FAIL;
    }
    //apply buffer from (baseaddres +0x80000) for sw process and gamma table
    err = MDrv_LD_PrepareBuffs();
    if(-1 == err)
    {
        LD_ERROR(" ioremap global buffers fail, check first!  \n");
        vfree(buf);
        return E_LD_STATUS_FAIL;
    }
    MHal_LD_SetLEDType((MS_U8)psDrvLdPQInfo->enLEDType);
    MHal_LD_SetLEDBacklightWidth(psDrvLdPQInfo->u8LEDWidth);
    MHal_LD_SetLEDBacklightHeight(psDrvLdPQInfo->u8LEDHeight);

    MHal_LD_SetSWLocalDimmingStrength(psDrvLdPQInfo->u8LDStrength);
    MHal_LD_SetSWGlobalDimmingStrength(psDrvLdPQInfo->u8GDStrength);
    MHal_LD_SetSWBacklightInGamma(psDrvLdPQInfo->u8InGamma);
    MHal_LD_SetSWBacklightOutGamma(psDrvLdPQInfo->u8OutGamma);
    MHal_LD_SetSWMinClampValue(psDrvLdPQInfo->u8BLMinValue);

    MHal_LD_SetSWSpatialFilterStrength(psDrvLdPQInfo->u8SFStrength);
    MHal_LD_SetSWSpatialFilterStrength2(psDrvLdPQInfo->u8SFStrength2);
    MHal_LD_SetSWSpatialFilterStrength3(psDrvLdPQInfo->u8SFStrength3);
    MHal_LD_SetSWSpatialFilterStrength4(psDrvLdPQInfo->u8SFStrength4);
    MHal_LD_SetSWSpatialFilterStrength5(psDrvLdPQInfo->u8SFStrength5);

    MHal_LD_SetSWTemporalFilterStrengthUp(psDrvLdPQInfo->u8TFStrengthUp);
    MHal_LD_SetSWTemporalFilterStrengthDn(psDrvLdPQInfo->u8TFStrengthDn);
    MHal_LD_SetSWTemporalFilterLowTh(psDrvLdPQInfo->u8TFLowThreshold);
    MHal_LD_SetSWTemporalFilterHighTh(psDrvLdPQInfo->u8TFHightThreshold);

    MHal_LD_SetSWMaxThreshold(0x00);  //MaxThreshold initial

    MHal_LD_SetSWPulseMode(0);
    MHal_LD_SetSWPulseId(0);
    MHal_LD_SetSWPulseLEDIntensity(0);
    MHal_LD_SetSWPulseLDBIntensity(0);


    MHal_LD_SetLSFEncodingEn(psDrvLdPQInfo->bLSFEncodingEn);
    MHal_LD_SetCompLowGainEn(psDrvLdPQInfo->bCompLowGainEn);
    MHal_LD_SetSaturationEn(psDrvLdPQInfo->bSaturationEn);

    MHal_LD_SetCompensationEn(0);
    MHal_LD_SetCompensationBlend(psDrvLdPQInfo->u8CompenGain);
    MHal_LD_SetCompensationDitherEn(TRUE);

    MHal_LD_SetSWReadLDFEn(ENABLE);
    MHal_LD_SetSWAlgorithmEn(ENABLE);
    MHal_LD_SetSWWriteLDBEn(ENABLE);
    MHal_LD_SetSWWriteSPIEn(ENABLE);

    //strlen("ldm.ini") is 7, replace "ldm.ini" with "Compensation.bin"
    strcpy((stCusPath.aCusPath+strlen(stCusPath.aCusPath)-7),"Compensation.bin");
    strcpy((stCusPath.aCusPathU+strlen(stCusPath.aCusPathU)-7),"Compensation.bin");
    //load Compensation.bin and write them to regs
    if (psDrvLdPQInfo->bEdge2DEn)
    {
        if(FALSE == MDrv_LD_ParseBin(buf, LD_BIN_LENGTH, (char *)stCusPath.aCusPath, (char *)stCusPath.aCusPathU))
        {
             vfree(buf);
            LD_ERROR(" load compensation bin fail \n");
            return E_LD_STATUS_PARAMETER_ERROR;
        }

        psDrvLdPQInfo->pu8CompTable = buf + LD_BIN_OFFSET;
        LD_INFO(" trace!  addr buf:0x%p, addr comp:0x%p \n", buf, psDrvLdPQInfo->pu8CompTable);
        if (psDrvLdPQInfo->bCompLowGainEn)
        {
            psDrvLdPQInfo->pu8CompTable1 = psDrvLdPQInfo->pu8CompTable + LD_COMPENSATION_LENGTH;
            psDrvLdPQInfo->pu8CompTable2 = psDrvLdPQInfo->pu8CompTable1 + LD_COMPENSATION_LENGTH;
            MDrv_LD_LoadCompensationTable(psDrvLdPQInfo->pu8CompTable1);
            MDrv_LD_LoadCompensationTable2(psDrvLdPQInfo->pu8CompTable2);
        }
        else
        {
            MDrv_LD_LoadCompensationTable(psDrvLdPQInfo->pu8CompTable);
        }
    }

    //strlen("Compensation.bin") is 16, replace "Compensation.bin" with "Edge2D.bin"
    strcpy((stCusPath.aCusPath+strlen(stCusPath.aCusPath)-16),"Edge2D.bin");
    strcpy((stCusPath.aCusPathU+strlen(stCusPath.aCusPathU)-16),"Edge2D.bin");
    //load Edge2D.bin and write them to address of edge2D
    if (psDrvLdPQInfo->bEdge2DEn)
    {
        if(FALSE == MDrv_LD_ParseBin(buf, LD_BIN_LENGTH, (char *)stCusPath.aCusPath, (char *)stCusPath.aCusPathU))
        {
            vfree(buf);
            LD_ERROR("load Edge2D bin fail \n");
            return E_LD_STATUS_PARAMETER_ERROR;
        }
        psDrvLdPQInfo->pu8Edge2DTable = buf + LD_BIN_OFFSET;
        LD_INFO(" trace!  addr buf:0x%p, addr edge:0x%p \n", buf, psDrvLdPQInfo->pu8Edge2DTable);
        MDrv_LD_LoadEdge2DTable(psDrvLdPQInfo->pu8Edge2DTable);
    }

    //strlen("Edge2D.bin") is 10, replace "Edge2D.bin" with "BacklightGamma.bin"
    strncpy((stCusPath.aCusPath+strlen(stCusPath.aCusPath)-10),"BacklightGamma.bin", 18);
    strncpy((stCusPath.aCusPathU+strlen(stCusPath.aCusPathU)-10),"BacklightGamma.bin", 18);
    //load BacklightGamma.bin and write them to dram ,the address is pGamma_blocks
    if(FALSE == MDrv_LD_ParseBin(buf, LD_BIN_LENGTH, (char *)stCusPath.aCusPath, (char *)stCusPath.aCusPathU))
    {
        vfree(buf);
        LD_ERROR("load BacklightGamma bin fail \n");
        return E_LD_STATUS_PARAMETER_ERROR;
    }

    if(1 == u8LdSuspend)
    {
        vfree(buf);
        return E_LD_STATUS_SUCCESS;
    }

    MDrv_LD_SetupGammaTable(buf + LD_BIN_OFFSET);

    vfree(buf);
    LD_INFO(" trace!  u8LEDWidth:0x%x, u8LEDHeight:0x%x \n",
        psDrvLdPQInfo->u8LEDWidth, psDrvLdPQInfo->u8LEDHeight);

    return E_LD_STATUS_SUCCESS;
}

MS_S8 MDrv_LD_Init(MS_PHY mmapaddr)
{
    MS_U16 flag = 0;
    MS_PHY u64Addr_Offset = 0;
    MS_U16 U16Ret = FALSE;

    MHal_LD_Set_SRAM_Power_Down(FALSE);

    U16Ret = MDrv_LD_PreInitVariable();
    if(E_LD_STATUS_SUCCESS != U16Ret)
    {
        LD_ERROR("error! parse customer file fail  \n");
        return -1;
    }

    psDrvLdInfo->u16PackLength = MHAL_LD_PACKLENGTH;

    if((NULL == mmapaddr) || (((size_t)-1) == mmapaddr))
    {
        ld_addr_base = psDrvLdInfo->u32BaseAddr;
    }
    else
    {
        ld_addr_base = psDrvLdInfo->u32BaseAddr = mmapaddr;
    }

    if(!ld_addr_base)
    {
        LD_ERROR("error!  addr_base_LDF null \n");
        return -1;
    }

    LD_INFO(" trace!  init prameter:0x%lx, ld_addr_base:0x%lx, u32BaseAddr:0x%lx \n",
        (unsigned long)mmapaddr, (unsigned long)ld_addr_base, (unsigned long)(psDrvLdInfo->u32BaseAddr));

    //only ioremap_cached when ac on ,when dc on should not do it
    if(NULL == u64Vaddr)
    {
    GETMIUOFFSET(ld_addr_base, u64Addr_Offset);
    u64Vaddr = ioremap_cached(ld_addr_base+u64Addr_Offset, LDMMAPLENGTH);
    }
    if((NULL == u64Vaddr) || ((void*)((size_t)-1) == u64Vaddr))
    {
        LD_ERROR("error! ioremap_cached paddr:0x%x,length:%d\n", (MS_U32)(ld_addr_base+u64Addr_Offset),LDMMAPLENGTH);
        return -1;
    }
    LD_INFO("LD_VA_BASE = 0x:%p, length = 0x:%x \n", u64Vaddr,LDMMAPLENGTH);

#ifdef LD_SUPPORT_1XN_NX1
    if (psDrvLdPQInfo->u8LEDWidth == 1)
    {
        bLed1xN = TRUE;
        psDrvLdPQInfo->u8LEDWidth = 2;
        if((psDrvLdPQInfo->u8LDFWidth)<(psDrvLdPQInfo->u8LEDWidth))
            psDrvLdPQInfo->u8LDFWidth=psDrvLdPQInfo->u8LEDWidth;
    }
    if (psDrvLdPQInfo->u8LEDHeight == 1)
    {
        bLedNx1 = TRUE;
        psDrvLdPQInfo->u8LEDHeight = 2;
        if((psDrvLdPQInfo->u8LDFHeight)<(psDrvLdPQInfo->u8LEDHeight))
            psDrvLdPQInfo->u8LDFHeight=psDrvLdPQInfo->u8LEDHeight;
    }
#endif

    flag = MDrv_LD_InitLDF();
    if (flag != E_LD_STATUS_SUCCESS)
    {
        LD_ERROR("flag[%d] \n", flag);
        return E_LD_STATUS_FAIL;
    }

    flag = MDrv_LD_InitLDB();
    if (flag != E_LD_STATUS_SUCCESS)
    {
        LD_ERROR("flag[%d] \n", flag);
        return E_LD_STATUS_FAIL;
    }

    if(TRUE == psDrvLdPQInfo->bSaturationEn)
    {
        flag = MDrv_LD_InitSat();
        if (flag != E_LD_STATUS_SUCCESS)
        {
            LD_ERROR("flag[%d] \n", flag);
            return E_LD_STATUS_FAIL;
        }
    }

    flag = MDrv_LD_InitSW();
    if (flag != E_LD_STATUS_SUCCESS)
    {
        LD_ERROR("flag[%d] \n", flag);
        return E_LD_STATUS_FAIL;
    }

    gbLdInit  = TRUE;
    LD_INFO("  done \n");
    return E_LD_STATUS_SUCCESS;
}

#endif
