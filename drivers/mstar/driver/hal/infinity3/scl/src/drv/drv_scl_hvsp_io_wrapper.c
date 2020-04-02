//<MStar Software>
//******************************************************************************
// MStar Software
// Copyright (c) 2010 - 2012 MStar Semiconductor, Inc. All rights reserved.
// All software, firmware and related documentation herein ("MStar Software") are
// intellectual property of MStar Semiconductor, Inc. ("MStar") and protected by
// law, including, but not limited to, copyright law and international treaties.
// Any use, modification, reproduction, retransmission, or republication of all
// or part of MStar Software is expressly prohibited, unless prior written
// permission has been granted by MStar.
//
// By accessing, browsing and/or using MStar Software, you acknowledge that you
// have read, understood, and agree, to be bound by below terms ("Terms") and to
// comply with all applicable laws and regulations:
//
// 1. MStar shall retain any and all right, ownership and interest to MStar
//    Software and any modification/derivatives thereof.
//    No right, ownership, or interest to MStar Software and any
//    modification/derivatives thereof is transferred to you under Terms.
//
// 2. You understand that MStar Software might include, incorporate or be
//    supplied together with third party`s software and the use of MStar
//    Software may require additional licenses from third parties.
//    Therefore, you hereby agree it is your sole responsibility to separately
//    obtain any and all third party right and license necessary for your use of
//    such third party`s software.
//
// 3. MStar Software and any modification/derivatives thereof shall be deemed as
//    MStar`s confidential information and you agree to keep MStar`s
//    confidential information in strictest confidence and not disclose to any
//    third party.
//
// 4. MStar Software is provided on an "AS IS" basis without warranties of any
//    kind. Any warranties are hereby expressly disclaimed by MStar, including
//    without limitation, any warranties of merchantability, non-infringement of
//    intellectual property rights, fitness for a particular purpose, error free
//    and in conformity with any international standard.  You agree to waive any
//    claim against MStar for any loss, damage, cost or expense that you may
//    incur related to your use of MStar Software.
//    In no event shall MStar be liable for any direct, indirect, incidental or
//    consequential damages, including without limitation, lost of profit or
//    revenues, lost or damage of data, and unauthorized system use.
//    You agree that this Section 4 shall still apply without being affected
//    even if MStar Software has been modified by MStar in accordance with your
//    request or instruction for your use, except otherwise agreed by both
//    parties in writing.
//
// 5. If requested, MStar may from time to time provide technical supports or
//    services in relation with MStar Software to you for your use of
//    MStar Software in conjunction with your or your customer`s product
//    ("Services").
//    You understand and agree that, except otherwise agreed by both parties in
//    writing, Services are provided on an "AS IS" basis and the warranty
//    disclaimer set forth in Section 4 above shall apply.
//
// 6. Nothing contained herein shall be construed as by implication, estoppels
//    or otherwise:
//    (a) conferring any license or right to use MStar name, trademark, service
//        mark, symbol or any other identification;
//    (b) obligating MStar or any of its affiliates to furnish any person,
//        including without limitation, you and your customers, any assistance
//        of any kind whatsoever, or any information; or
//    (c) conferring any license or right under any intellectual property right.
//
// 7. These terms shall be governed by and construed in accordance with the laws
//    of Taiwan, R.O.C., excluding its conflict of law rules.
//    Any and all dispute arising out hereof or related hereto shall be finally
//    settled by arbitration referred to the Chinese Arbitration Association,
//    Taipei in accordance with the ROC Arbitration Law and the Arbitration
//    Rules of the Association by three (3) arbitrators appointed in accordance
//    with the said Rules.
//    The place of arbitration shall be in Taipei, Taiwan and the language shall
//    be English.
//    The arbitration award shall be final and binding to both parties.
//
//******************************************************************************
//<MStar Software>
////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2008-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// ("MStar Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////
#define __DRV_SCL_HVSP_IO_WRAPPER_C__


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "drv_scl_os.h"
#include "drv_scl_dbg.h"
#include "hal_scl_util.h"

#include "drv_scl_verchk.h"
#include "drv_scl_irq_st.h"
#include "drv_scl_hvsp_m.h"
#include "drv_scl_dma_m.h"
#include "drv_scl_multiinst_m_st.h"
#include "drv_scl_multiinst_m.h"
#include "drv_scl_hvsp_io_st.h"
#include "drv_scl_hvsp_io_wrapper.h"
//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------
typedef struct
{
    MDrvSclMultiInstEntryIdType_e enMultiInstId;
    void *pPrivateData;
}DrvSclHvspMultiInstConfig_t;

//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
DrvSclHvspHandleConfig_t _gstSclHvspHandler[DRV_SCLHVSP_HANDLER_MAX];

DrvSclOsDmemBusType_t sg_hvsp1_mcnr_size = 1920 * 1080 * 2 * 2 ;

DrvSclOsDmemBusType_t  sg_hvsp1_mcnr_yc_bus_addr = 0;
void *sg_hvsp1_mcnr_yc_vir_addr = NULL;
char* KEY_DMEM_SCL_MCNR_YC = "SCL_MCNR_YC";

DrvSclOsDmemBusType_t  sg_hvsp1_mcnr_ciir_bus_addr = 0;
void *sg_hvsp1_mcnr_ciir_vir_addr = NULL;
char* KEY_DMEM_SCL_MCNR_CIIR = "SCL_MCNR_CIIR";

DrvSclOsDmemBusType_t  sg_hvsp1_mcnr_m_bus_addr = 0;
void *sg_hvsp1_mcnr_m_vir_addr = NULL;
char* KEY_DMEM_SCL_MCNR_M = "SCL_MCNR_M";

DrvSclOsDmemBusType_t sg_hvsp1_release_mcnr_size;



u32 SCL_IRQ_ID = 0; //INT_IRQ_AU_SYSTEM;
u32 CMDQ_IRQ_ID = 0; //INT_IRQ_AU_SYSTEM;

u32 gu8FrameBufferReadyNum = 0; //extern
u8  gbdbgmessage[EN_DBGMG_NUM_CONFIG];//extern
u32 gVSyncCount = 0;
u32 gMonitorErrCount = 0;
bool gbCIIR = 0;

DrvSclHvspIoFunctionConfig_t _gstSclHvspIoFunc;


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

void* _DrvSclHvspIoAllocDmem(const char* name, u32 size, DrvSclOsDmemBusType_t *addr)
{
    void *pDmem = NULL;;

    pDmem = DrvSclOsDirectMemAlloc(name, size, addr);

    if(pDmem)
    {
        gu8FrameBufferReadyNum = (MdrvSclHvspGetFrameBufferCountInformation());
        sg_hvsp1_release_mcnr_size = sg_hvsp1_mcnr_size;
    }

    return pDmem;
}

void _DrvSclHvspIoFreeDmem(const char* name, unsigned int size, void *virt, u32 addr)
{
    DrvSclOsDirectMemFree(name, size, virt, addr);
}


bool  _DrvSclHvspIoCheckModifyMemSize(DrvSclHvspIoReqMemConfig_t  *stReqMemCfg)
{
    if( (stReqMemCfg->u16Vsize & (15)) || (stReqMemCfg->u16Pitch & (15)))
    {
        SCL_ERR(
            "[HVSP] Size must be align 16, Vsize=%d, Pitch=%d\n", stReqMemCfg->u16Vsize, stReqMemCfg->u16Pitch);
        if((stReqMemCfg->u16Pitch & (15)) && (stReqMemCfg->u16Vsize & (15)))
        {
            stReqMemCfg->u32MemSize = (((stReqMemCfg->u16Vsize / 16) + 1) * 16) * ((stReqMemCfg->u16Pitch / 16) + 1) * 16 * 4;
        }
        else if(stReqMemCfg->u16Pitch & (15))
        {
            stReqMemCfg->u32MemSize = (stReqMemCfg->u16Vsize) * ((stReqMemCfg->u16Pitch / 16) + 1) * 16 * 4;
        }
        else if(stReqMemCfg->u16Vsize & (15))
        {
            stReqMemCfg->u32MemSize = (((stReqMemCfg->u16Vsize / 16) + 1) * 16) * stReqMemCfg->u16Pitch * 4;
        }
    }
    else
    {
        stReqMemCfg->u32MemSize = (stReqMemCfg->u16Vsize) * (stReqMemCfg->u16Pitch) * 4;
    }
    if(MdrvSclHvspGetFrameBufferCountInformation() == 1)
    {
        SCL_ERR(
            "[HVSP] Buffer is single, Vsize=%d, Pitch=%d\n", stReqMemCfg->u16Vsize, stReqMemCfg->u16Pitch);
        stReqMemCfg->u32MemSize = stReqMemCfg->u32MemSize / 2 ;
    }
    sg_hvsp1_mcnr_size = stReqMemCfg->u32MemSize;
    if(MdrvSclHvspGetFrameBufferCountInformation() == 1)
    {
        if((u32)(stReqMemCfg->u16Vsize * stReqMemCfg->u16Pitch * 2) > stReqMemCfg->u32MemSize)
        {
            SCL_ERR( "[HVSP] Memory size is too small, Vsize*Pitch*2=%lx, MemSize=%lx\n",
                     (u32)(stReqMemCfg->u16Vsize * stReqMemCfg->u16Pitch * 2), stReqMemCfg->u32MemSize);
            return FALSE;
        }
    }
    else if((u32)(stReqMemCfg->u16Vsize * stReqMemCfg->u16Pitch * 4) > stReqMemCfg->u32MemSize)
    {
        SCL_ERR( "[HVSP] Memory size is too small, Vsize*Pitch*4=%lx, MemSize=%lx\n",
                 (u32)(stReqMemCfg->u16Vsize * stReqMemCfg->u16Pitch * 4), stReqMemCfg->u32MemSize);
        return FALSE;
    }
    SCL_DBG(SCL_DBG_LV_HVSP()&EN_DBGMG_HVSPLEVEL_HVSP1,
            "[HVSP], Vsize=%d, Pitch=%d, Size=%lx\n", stReqMemCfg->u16Vsize, stReqMemCfg->u16Pitch, stReqMemCfg->u32MemSize);
    return TRUE;
}

void _DrvSclHvspIoMemFreeYCbuffer(void)
{
    if(sg_hvsp1_mcnr_yc_vir_addr != 0)
    {
        SCL_DBG(SCL_DBG_LV_HVSP()&EN_DBGMG_HVSPLEVEL_HVSP1, "[HVSP] YC free\n");
        _DrvSclHvspIoFreeDmem(KEY_DMEM_SCL_MCNR_YC,
                            PAGE_ALIGN(sg_hvsp1_mcnr_size),
                            sg_hvsp1_mcnr_yc_vir_addr,
                            sg_hvsp1_mcnr_yc_bus_addr);

        sg_hvsp1_mcnr_yc_vir_addr = 0;
        sg_hvsp1_mcnr_yc_bus_addr = 0;
        gu8FrameBufferReadyNum = 0;
    }
}
void _DrvSclHvspIoMemFreeMBuffer(void)
{
    if(sg_hvsp1_mcnr_m_vir_addr != 0)
    {
        SCL_DBG(SCL_DBG_LV_HVSP()&EN_DBGMG_HVSPLEVEL_HVSP1, "[HVSP] Motion free\n");
        _DrvSclHvspIoFreeDmem(KEY_DMEM_SCL_MCNR_M,
                            PAGE_ALIGN(sg_hvsp1_mcnr_size / 4),
                            sg_hvsp1_mcnr_m_vir_addr,
                            sg_hvsp1_mcnr_m_bus_addr);

        sg_hvsp1_mcnr_m_vir_addr = 0;
        sg_hvsp1_mcnr_m_bus_addr = 0;
    }
}
void _DrvSclHvspIoMemFreeCIIRbuffer(void)
{
    if(sg_hvsp1_mcnr_ciir_vir_addr != 0)
    {
        SCL_DBG(SCL_DBG_LV_HVSP()&EN_DBGMG_HVSPLEVEL_HVSP1, "[HVSP] CIIR free\n");
        _DrvSclHvspIoFreeDmem(KEY_DMEM_SCL_MCNR_CIIR,
                            PAGE_ALIGN(sg_hvsp1_mcnr_size / 2),
                            sg_hvsp1_mcnr_ciir_vir_addr,
                            sg_hvsp1_mcnr_ciir_bus_addr);

        sg_hvsp1_mcnr_ciir_vir_addr = 0;
        sg_hvsp1_mcnr_ciir_bus_addr = 0;
    }
}

bool _DrvSclHvspIoMemAllocate(void)
{

    SCL_DBG(SCL_DBG_LV_HVSP()&EN_DBGMG_HVSPLEVEL_HVSP1, "[HVSP] allocate memory\n");

    if (!(sg_hvsp1_mcnr_yc_vir_addr = _DrvSclHvspIoAllocDmem(KEY_DMEM_SCL_MCNR_YC,
                                      PAGE_ALIGN(sg_hvsp1_mcnr_size),
                                      &sg_hvsp1_mcnr_yc_bus_addr)))
    {
        SCL_ERR( "%s: unable to allocate YC memory\n", __FUNCTION__);
        return 0;
    }
    if (!(sg_hvsp1_mcnr_m_vir_addr = _DrvSclHvspIoAllocDmem(KEY_DMEM_SCL_MCNR_M,
                                     PAGE_ALIGN(sg_hvsp1_mcnr_size / 4),
                                     &sg_hvsp1_mcnr_m_bus_addr)))
    {
        SCL_ERR( "%s: unable to allocate Montion memory\n", __FUNCTION__);
        _DrvSclHvspIoMemFreeYCbuffer();
        return 0;
    }
    if(CIIROPEN)
    {
        if (!(sg_hvsp1_mcnr_ciir_vir_addr = _DrvSclHvspIoAllocDmem(KEY_DMEM_SCL_MCNR_CIIR,
                                            PAGE_ALIGN(sg_hvsp1_mcnr_size / 2),
                                            &sg_hvsp1_mcnr_ciir_bus_addr)))
        {
            SCL_ERR( "%s: unable to allocate CIIR memory\n", __FUNCTION__);
            return 0;
        }
        else
        {
            gu8FrameBufferReadyNum |= (MdrvSclHvspGetFrameBufferCountInformation() << 2);
        }
    }

    SCL_ERR( "[HVSP]: MCNR YC: Phy:%x  Vir:%lx\n", sg_hvsp1_mcnr_yc_bus_addr, (u32)sg_hvsp1_mcnr_yc_vir_addr);
    SCL_ERR( "[HVSP]: MCNR CIIR: Phy:%x  Vir:%lx\n", sg_hvsp1_mcnr_ciir_bus_addr, (u32)sg_hvsp1_mcnr_ciir_vir_addr);
    SCL_ERR( "[HVSP]: MCNR M: Phy:%x  Vir:%lx\n", sg_hvsp1_mcnr_m_bus_addr, (u32)sg_hvsp1_mcnr_m_vir_addr);

    return 1;
}

void _DrvSclHvspIoMemFree(void)
{
    _DrvSclHvspIoMemFreeYCbuffer();
    _DrvSclHvspIoMemFreeMBuffer();
    _DrvSclHvspIoMemFreeCIIRbuffer();
}

void _DrvSclhvspFrameBufferMemoryAllocate(void)
{
    if(gu8FrameBufferReadyNum == 0)
    {
        _DrvSclHvspIoMemAllocate();
        MDrvSclHvspSetMemoryAllocateReady(gu8FrameBufferReadyNum);
    }
    else if(gu8FrameBufferReadyNum != 0 && sg_hvsp1_mcnr_size > sg_hvsp1_release_mcnr_size)
    {
        _DrvSclHvspIoMemFree();
        gu8FrameBufferReadyNum = 0;
        _DrvSclHvspIoMemAllocate();
        MDrvSclHvspSetMemoryAllocateReady((gu8FrameBufferReadyNum > 0) ? 1 : 0);
    }
}

MDrvSclHvspIpmConfig_t _DrvSclHvspIoFillIPMStructForDriver(DrvSclHvspIoReqMemConfig_t *pstReqMemCfg)
{
    MDrvSclHvspIpmConfig_t stIPMCfg;
    stIPMCfg.u16Height = pstReqMemCfg->u16Vsize;
    stIPMCfg.u16Width  = pstReqMemCfg->u16Pitch;
    stIPMCfg.u32MemSize = pstReqMemCfg->u32MemSize;
    if(gu8FrameBufferReadyNum & 0x3)
    {
        if((stIPMCfg.u16Height <= (FHDHeight + 8)) && (stIPMCfg.u16Width <= FHDWidth))
        {
            stIPMCfg.enRW       = E_MDRV_SCLHVSP_MCNR_YCM_W;
        }
        else
        {
            stIPMCfg.enRW       = E_MDRV_SCLHVSP_MCNR_YCM_RW;
        }
        sg_hvsp1_mcnr_size   = pstReqMemCfg->u32MemSize;
    }
    else
    {
        stIPMCfg.enRW       = E_MDRV_SCLHVSP_MCNR_NON;
        sg_hvsp1_mcnr_size   = pstReqMemCfg->u32MemSize;
    }
    if(sg_hvsp1_mcnr_yc_bus_addr)
    {
        stIPMCfg.u32YCPhyAddr = DrvSclOsPa2Miu(sg_hvsp1_mcnr_yc_bus_addr);
    }
    if(sg_hvsp1_mcnr_m_bus_addr)
    {
        stIPMCfg.u32MPhyAddr = DrvSclOsPa2Miu(sg_hvsp1_mcnr_m_bus_addr);
    }
    if(sg_hvsp1_mcnr_ciir_bus_addr)
    {
        stIPMCfg.u32CIIRPhyAddr = DrvSclOsPa2Miu(sg_hvsp1_mcnr_ciir_bus_addr);
    }
    return stIPMCfg;
}



DrvSclHvspVersionChkConfig_t _DrvSclHvspIoFillVersionChkStruct(u32 u32StructSize, u32 u32VersionSize, u32 *pVersion)
{
    DrvSclHvspVersionChkConfig_t stVersion;
    stVersion.u32StructSize  = (u32)u32StructSize;
    stVersion.u32VersionSize = (u32)u32VersionSize;
    stVersion.pVersion      = (u32 *)pVersion;
    return stVersion;
}

s32 _DrvSclHvspIoVersionCheck(DrvSclHvspVersionChkConfig_t stVersion)
{
    if ( CHK_VERCHK_HEADER(stVersion.pVersion) )
    {
        if( CHK_VERCHK_MAJORVERSION_LESS( stVersion.pVersion, DRV_SCLHVSP_VERSION) )
        {

            VERCHK_ERR("[HVSP] Version(%04lx) < %04x!!! \n",
                       *(stVersion.pVersion) & VERCHK_VERSION_MASK,
                       DRV_SCLHVSP_VERSION);

            return -1;
        }
        else
        {
            if( CHK_VERCHK_SIZE( &stVersion.u32VersionSize, stVersion.u32StructSize) == 0 )
            {
                VERCHK_ERR("[HVSP] Size(%04lx) != %04lx!!! \n",
                           stVersion.u32StructSize,
                           stVersion.u32VersionSize);

                return -1;
            }
            else
            {
                return VersionCheckSuccess;
            }
        }
    }
    else
    {
        VERCHK_ERR("[HVSP] No Header !!! \n");
        SCL_ERR( "[HVSP]   %s  \n", __FUNCTION__);
        return -1;
    }
}
u8 _DrvSclHvspIoGetIdOpenTime(DrvSclHvspIoIdType_e enHvspId)
{
    s16 i = 0;
    u8 u8Cnt = 0;
    for(i = 0; i < DRV_SCLHVSP_HANDLER_MAX; i++)
    {
        if(_gstSclHvspHandler[i].enSclHvspId == enHvspId && _gstSclHvspHandler[i].s32Handle != -1)
        {
            u8Cnt ++;
        }
    }
    return u8Cnt;
}
bool _DrvSclHvspIoGetMdrvIdType(s32 s32Handler, MDrvSclHvspIdType_e *penHvspId)
{
    s16 i;
    s16 s16Idx = -1;
    bool bRet = TRUE;

    for(i = 0; i < DRV_SCLHVSP_HANDLER_MAX; i++)
    {
        if(_gstSclHvspHandler[i].s32Handle == s32Handler)
        {
            s16Idx = i;
            break;
        }
    }

    if(s16Idx == -1)
    {
        *penHvspId = E_MDRV_SCLHVSP_ID_MAX;
        bRet = FALSE;
    }
    else
    {
        bRet = TRUE;
        switch(_gstSclHvspHandler[s16Idx].enSclHvspId)
        {
            case E_DRV_SCLHVSP_IO_ID_1:
                *penHvspId = E_MDRV_SCLHVSP_ID_1;
                break;
            case E_DRV_SCLHVSP_IO_ID_2:
                *penHvspId = E_MDRV_SCLHVSP_ID_2;
                break;
            case E_DRV_SCLHVSP_IO_ID_3:
                *penHvspId = E_MDRV_SCLHVSP_ID_3;
                break;
            default:
                *penHvspId = E_MDRV_SCLHVSP_ID_MAX;
                bRet = FALSE;
                break;
        }
    }
    return bRet;
}

MDrvSclMultiInstEntryIdType_e _DrvSclHvspIoTransMultiInstId(DrvSclHvspIoIdType_e enSclHvspId)
{
    MDrvSclMultiInstEntryIdType_e enMulitInstId = E_MDRV_SCLMULTI_INST_ENTRY_ID_MAX;

    switch(enSclHvspId)
    {
        case E_DRV_SCLHVSP_IO_ID_1:
            enMulitInstId = E_MDRV_SCLMULTI_INST_ENTRY_ID_HVSP1;
            break;

        case E_DRV_SCLHVSP_IO_ID_2:
            enMulitInstId = E_MDRV_SCLMULTI_INST_ENTRY_ID_HVSP2;
            break;

        case E_DRV_SCLHVSP_IO_ID_3:
            enMulitInstId = E_MDRV_SCLMULTI_INST_ENTRY_ID_HVSP3;
            break;
		default:
        case E_DRV_SCLHVSP_IO_ID_NUM:
            enMulitInstId = E_MDRV_SCLMULTI_INST_ENTRY_ID_MAX;
            break;
    }
    return enMulitInstId;
}


bool _DrvSclHvspIoGetMultiInstConfig(s32 s32Handler, DrvSclHvspMultiInstConfig_t *pCfg)
{
    s16 i;
    s16 s16Idx = -1;
    bool bRet = TRUE;

    for(i = 0; i < DRV_SCLHVSP_HANDLER_MAX; i++)
    {
        if(_gstSclHvspHandler[i].s32Handle == s32Handler)
        {
            s16Idx = i;
            break;
        }
    }

    if(s16Idx == -1)
    {
        pCfg->enMultiInstId = E_MDRV_SCLMULTI_INST_ENTRY_ID_MAX;
        pCfg->pPrivateData = NULL;
        bRet = FALSE;
    }
    else
    {
        bRet = TRUE;
        pCfg->pPrivateData = _gstSclHvspHandler[s16Idx].pPrivate_Data;
        switch(_gstSclHvspHandler[s16Idx].enSclHvspId)
        {
            case E_DRV_SCLHVSP_IO_ID_1:
                pCfg->enMultiInstId = E_MDRV_SCLMULTI_INST_ENTRY_ID_HVSP1;
                break;
            case E_DRV_SCLHVSP_IO_ID_2:
                pCfg->enMultiInstId = E_MDRV_SCLMULTI_INST_ENTRY_ID_HVSP2;
                break;
            case E_DRV_SCLHVSP_IO_ID_3:
                pCfg->enMultiInstId = E_MDRV_SCLMULTI_INST_ENTRY_ID_HVSP3;
                break;
            default:
                bRet = FALSE;
                pCfg->enMultiInstId = E_MDRV_SCLMULTI_INST_ENTRY_ID_MAX;
                break;
        }
    }
    return bRet;
}

bool _DrvSclHvspIoMultiInstSet(s32 s32Handler, MDrvSclMultiInstCmdType_e enType, void *stCfg)
{
    DrvSclHvspMultiInstConfig_t stMultiInstCfg;
    MDrvSclMultiInstStatusType_e enMultiInstRet;
    bool bRet = TRUE;

    if(_DrvSclHvspIoGetMultiInstConfig(s32Handler, &stMultiInstCfg) == FALSE)
    {
        SCL_DBG(SCL_DBG_LV_0, "%s %d:: _DrvSclHvspIoMultiInstSet fail\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        enMultiInstRet = MDrvSclMultiInstEntryFlashData(stMultiInstCfg.enMultiInstId, stMultiInstCfg.pPrivateData, enType, stCfg);

        if(enMultiInstRet == E_MDRV_SCLMULTI_INST_STATUS_LOCKED)
        {
            bRet = FALSE;
        }
        else if (enMultiInstRet == E_MDRV_SCLMULTI_INST_STATUS_FAIL)
        {
            bRet = FALSE;
        }
        else
        {
            bRet = TRUE;
        }
    }

    return bRet;
}

DrvSclOsClkIdType_e _DrvSclHvspIoTransClkId(MDrvSclHvspIdType_e enMDrvSclHvspId)
{
    DrvSclOsClkIdType_e enClkId = E_DRV_SCLOS_CLK_ID_NUM;
    switch(enMDrvSclHvspId)
    {
        case E_MDRV_SCLHVSP_ID_1:
            enClkId = E_DRV_SCLOS_CLK_ID_HVSP1;
            break;

        case E_MDRV_SCLHVSP_ID_2:
            enClkId = E_DRV_SCLOS_CLK_ID_HVSP2;
            break;

        case E_MDRV_SCLHVSP_ID_3:
            enClkId = E_DRV_SCLOS_CLK_ID_HVSP3;
            break;

        case E_MDRV_SCLHVSP_ID_MAX:
            enClkId = E_DRV_SCLOS_CLK_ID_NUM;
            break;
    }
    return enClkId;
}

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
void _DrvSclHvspIoInitHandler(DrvSclHvspIoIdType_e enSclHvspId)
{
    u16 i, start, end;
    start = (u16)enSclHvspId  * MDRV_SCLMULTI_INST_PRIVATE_ID_NUM;
    end = start + MDRV_SCLMULTI_INST_PRIVATE_ID_NUM;

    for(i = start; i < end; i++)
    {
        _gstSclHvspHandler[i].s32Handle = -1;
        _gstSclHvspHandler[i].pPrivate_Data = NULL;
        _gstSclHvspHandler[i].enSclHvspId = E_DRV_SCLHVSP_IO_ID_NUM;
    }

    if(enSclHvspId == E_DRV_SCLHVSP_IO_ID_1)
    {
        DrvSclOsMemset(&_gstSclHvspIoFunc, 0, sizeof(DrvSclHvspIoFunctionConfig_t));
        _gstSclHvspIoFunc.DrvSclHvspIoSetInputConfig        = _DrvSclHvspIoSetInputConfig;
        _gstSclHvspIoFunc.DrvSclHvspIoSetOutputConfig       = _DrvSclHvspIoSetOutputConfig;
        _gstSclHvspIoFunc.DrvSclHvspIoSetScalingConfig      = _DrvSclHvspIoSetScalingConfig;
        _gstSclHvspIoFunc.DrvSclHvspIoReqmemConfig          = _DrvSclHvspIoReqmemConfig;
        _gstSclHvspIoFunc.DrvSclHvspIoSetMiscConfig         = _DrvSclHvspIoSetMiscConfig;
        _gstSclHvspIoFunc.DrvSclHvspIoSetPostCropConfig     = _DrvSclHvspIoSetPostCropConfig;
        _gstSclHvspIoFunc.DrvSclHvspIoGetPrivateIdConfig    = _DrvSclHvspIoGetPrivateIdConfig;
        _gstSclHvspIoFunc.DrvSclHvspIoGetInformConfig       = _DrvSclHvspIoGetInformConfig;
        _gstSclHvspIoFunc.DrvSclHvspIoReleaseMemConfig      = _DrvSclHvspIoReleaseMemConfig;
        _gstSclHvspIoFunc.DrvSclHvspIoSetOsdConfig          = _DrvSclHvspIoSetOsdConfig;
        _gstSclHvspIoFunc.DrvSclHvspIoSetPriMaskConfig      = _DrvSclHvspIoSetPriMaskConfig;
        _gstSclHvspIoFunc.DrvSclHvspIoPirMaskTrigerConfig   = _DrvSclHvspIoPirMaskTrigerConfig;
        _gstSclHvspIoFunc.DrvSclHvspIoSetFbConfig           = _DrvSclHvspIoSetFbConfig;
        _gstSclHvspIoFunc.DrvSclHvspIoGetVersion            = _DrvSclHvspIoGetVersion;
    }
}

s32 _DrvSclHvspIoOpen(DrvSclHvspIoIdType_e enSclHvspId)
{
    s32 s32Handle = -1;
    s16 s16Idx = -1;
    s16 i ;

    for(i = 0; i < DRV_SCLHVSP_HANDLER_MAX; i++)
    {
        if(_gstSclHvspHandler[i].s32Handle == -1)
        {
            s16Idx = i;
            break;
        }
    }

    if(s16Idx == -1)
    {
        s32Handle = -1;
        SCL_ERR( "[HVSP]   %s %d \n", __FUNCTION__, __LINE__);
    }
    else
    {
        if(MDrvSclMultiInstEntryAlloc(_DrvSclHvspIoTransMultiInstId(enSclHvspId), &_gstSclHvspHandler[s16Idx].pPrivate_Data) == 0)
        {
            s32Handle = -1;
        }
        else
        {
            MDrvSclHvspIdType_e enMdrvIdType;

            s32Handle = s16Idx | DRV_SCLHVSP_HANDLER_PRE_FIX;
            _gstSclHvspHandler[s16Idx].s32Handle = s32Handle ;
            _gstSclHvspHandler[s16Idx].enSclHvspId = enSclHvspId;
            if(_DrvSclHvspIoGetIdOpenTime(enSclHvspId)== 1)
            {
                _DrvSclHvspIoGetMdrvIdType(s32Handle, &enMdrvIdType);
                MDrvSclHvspOpen(enMdrvIdType);
            }
        }
    }

    return s32Handle;
}

DrvSclHvspIoErrType_e _DrvSclHvspIoRelease(s32 s32Handler)
{
    s16 s16Idx = -1;
    s16 i ;
    DrvSclHvspIoErrType_e eRet = TRUE;

    for(i = 0; i < DRV_SCLHVSP_HANDLER_MAX; i++)
    {
        if(_gstSclHvspHandler[i].s32Handle == s32Handler)
        {
            s16Idx = i;
            break;
        }
    }

    if(s16Idx == -1)
    {
        SCL_ERR( "[HVSP]   %s %d \n", __FUNCTION__, __LINE__);
        eRet = E_DRV_SCLHVSP_IO_ERR_INVAL;
    }
    else
    {
        DrvSclHvspMultiInstConfig_t stMultiInstCfg;
        MDrvSclHvspIdType_e enMdrvIdType;

        if(_DrvSclHvspIoGetMultiInstConfig(s32Handler, &stMultiInstCfg) == FALSE)
        {
            SCL_ERR( "[SCLDMA]   %s %d \n", __FUNCTION__, __LINE__);
            eRet = E_DRV_SCLHVSP_IO_ERR_INVAL;
        }
        else
        {
            MDrvSclMultiInstEntryFree(stMultiInstCfg.enMultiInstId, _gstSclHvspHandler[s16Idx].pPrivate_Data);
            if(_DrvSclHvspIoGetIdOpenTime(_gstSclHvspHandler[s16Idx].enSclHvspId)== 1)
            {
                _DrvSclHvspIoGetMdrvIdType(s32Handler, &enMdrvIdType);
                if(enMdrvIdType == E_MDRV_SCLHVSP_ID_1)
                {
                    MDrvSclHvspIdclkRelease((MDrvSclHvspClkConfig_t*)DrvSclOsClkGetConfig(E_DRV_SCLOS_CLK_ID_HVSP1));
                }
                MDrvSclHvspRelease(enMdrvIdType);
            }
            _gstSclHvspHandler[s16Idx].s32Handle = -1;
            _gstSclHvspHandler[s16Idx].pPrivate_Data = NULL;
            _gstSclHvspHandler[s16Idx].enSclHvspId = E_DRV_SCLHVSP_IO_ID_NUM;
            eRet = E_DRV_SCLHVSP_IO_ERR_OK;
        }
    }

    return eRet;
}

void _DrvSclHvspIoPollLinux(MDrvSclHvspIdType_e enMdrvIdType, DrvSclHvspIoWrapperPollConfig_t *pstIoPollCfg)
{
    MDrvSclHvspSetPollWait(pstIoPollCfg->stPollWaitCfg);
    if(MDrvSclHvspGetCmdqDoneStatus(enMdrvIdType))
    {
        pstIoPollCfg->u8retval = POLLIN;
    }
    else
    {
        pstIoPollCfg->u8retval = 0;
    }
    SCL_ERR("%s %d, ID=%d, ret=%d\n", __FUNCTION__, __LINE__, enMdrvIdType, pstIoPollCfg->u8retval);
}

void _DrvSclHvspIoPollRtk(MDrvSclHvspIdType_e enMdrvIdType, DrvSclHvspIoWrapperPollConfig_t *pstIoPollCfg)
{
    bool bTimeout = 0;

    while(1)
    {
        if(MDrvSclHvspGetCmdqDoneStatus(enMdrvIdType))
        {
            bTimeout = 0;
            break;
        }
        else
        {
            bTimeout = MDrvSclHvspSetPollWait(pstIoPollCfg->stPollWaitCfg);
            if(bTimeout)
            {
                SCL_ERR("%s %d, POLL TIMEOUT :%d\n",__FUNCTION__, __LINE__,enMdrvIdType);
            }
            break;
        }
    }
    if(bTimeout)
    {
        pstIoPollCfg->u8retval = 0;
    }
    else
    {
        pstIoPollCfg->u8retval = POLLIN;
    }

}


DrvSclHvspIoErrType_e _DrvSclHvspIoPoll(s32 s32Handler, DrvSclHvspIoWrapperPollConfig_t *pstIoPollCfg)
{
    MDrvSclHvspIdType_e enMdrvIdType;
    MDrvSclMultiInstStatusType_e enMultiInstRet;
    DrvSclHvspMultiInstConfig_t stMultiInstCfg;

    if(_DrvSclHvspIoGetMdrvIdType(s32Handler, &enMdrvIdType) == FALSE)
    {
        SCL_ERR( "[HVSP]   %s %d \n", __FUNCTION__, __LINE__);
        return FALSE;
    }
    if(_DrvSclHvspIoGetMultiInstConfig(s32Handler, &stMultiInstCfg) == FALSE)
    {
        SCL_ERR( "[HVSP]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLHVSP_IO_ERR_INVAL;
    }
    enMultiInstRet = MDrvSclMultiInstEtnryIsFree(stMultiInstCfg.enMultiInstId, stMultiInstCfg.pPrivateData);
    if(enMultiInstRet == E_MDRV_SCLMULTI_INST_STATUS_SUCCESS)
    {
        if(pstIoPollCfg->bWaitQueue)
        {
            _DrvSclHvspIoPollLinux(enMdrvIdType, pstIoPollCfg);
        }
        else
        {
            _DrvSclHvspIoPollRtk(enMdrvIdType, pstIoPollCfg);
        }
    }
    else
    {
        pstIoPollCfg->u8retval = 0;
        return E_DRV_SCLHVSP_IO_ERR_MULTI;
    }
    return E_DRV_SCLHVSP_IO_ERR_OK;
}
DrvSclHvspIoErrType_e _DrvSclHvspIoSetInputConfig(s32 s32Handler, DrvSclHvspIoInputConfig_t *pstIoInCfg)
{
    DrvSclHvspIoErrType_e eRet = E_DRV_SCLHVSP_IO_ERR_FAULT;
    MDrvSclHvspInputConfig_t stInCfg;
    DrvSclHvspVersionChkConfig_t stVersion;
    MDrvSclHvspIdType_e enMdrvIdType;

    stVersion =  _DrvSclHvspIoFillVersionChkStruct(sizeof(DrvSclHvspIoInputConfig_t),
                 pstIoInCfg->VerChk_Size,
                 &pstIoInCfg->VerChk_Version);

    if(_DrvSclHvspIoVersionCheck(stVersion))
    {
        SCL_ERR( "[HVSP]   %s %d \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLHVSP_IO_ERR_INVAL;
    }

    if(_DrvSclHvspIoGetMdrvIdType(s32Handler, &enMdrvIdType) == FALSE)
    {
        SCL_ERR( "[HVSP]   %s %d \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLHVSP_IO_ERR_INVAL;
    }

    stInCfg.enColor       = pstIoInCfg->enColor;
    stInCfg.enSrcType     = pstIoInCfg->enSrcType;
    DrvSclOsMemcpy(&stInCfg.stCaptureWin, &pstIoInCfg->stCaptureWin, sizeof(MDrvSclHvspWindowConfig_t));
    DrvSclOsMemcpy(&stInCfg.stTimingCfg, &pstIoInCfg->stTimingCfg, sizeof(MDrvSclHvspTimingConfig_t));

    stInCfg.stclk = (MDrvSclHvspClkConfig_t *)DrvSclOsClkGetConfig(_DrvSclHvspIoTransClkId(enMdrvIdType));

    if(_DrvSclHvspIoMultiInstSet(s32Handler, E_MDRV_SCLMULTI_INST_CMD_HVSP_IN_CONFIG,(void *)&stInCfg) == FALSE)
    {
        eRet = E_DRV_SCLHVSP_IO_ERR_MULTI;
    }
    else
    {
        if(!MDrvSclHvspSetInputConfig(enMdrvIdType,  &stInCfg))
        {
            eRet = E_DRV_SCLHVSP_IO_ERR_FAULT;
        }
        else
        {
            eRet = E_DRV_SCLHVSP_IO_ERR_OK;
        }
    }

    return eRet;
}

DrvSclHvspIoErrType_e _DrvSclHvspIoSetOutputConfig(s32 s32Handler, DrvSclHvspIoOutputConfig_t *pstIoOutCfg)
{
    DrvSclHvspVersionChkConfig_t stVersion;
    MDrvSclHvspIdType_e enMdrvIdType;

    stVersion =  _DrvSclHvspIoFillVersionChkStruct(sizeof(DrvSclHvspIoOutputConfig_t),
                 pstIoOutCfg->VerChk_Size,
                 &pstIoOutCfg->VerChk_Version);

    if(_DrvSclHvspIoVersionCheck(stVersion))
    {
        SCL_ERR( "[HVSP]   %s  \n", __FUNCTION__);
        return E_DRV_SCLHVSP_IO_ERR_INVAL;
    }

    if(_DrvSclHvspIoGetMdrvIdType(s32Handler, &enMdrvIdType) == FALSE)
    {
        SCL_ERR( "[HVSP]   %s %d \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLHVSP_IO_ERR_INVAL;
    }

    return E_DRV_SCLHVSP_IO_ERR_OK;
}

DrvSclHvspIoErrType_e _DrvSclHvspIoSetScalingConfig(s32 s32Handler, DrvSclHvspIoScalingConfig_t *pstIOSclCfg)
{
    MDrvSclHvspScalingConfig_t stSclCfg;
    DrvSclHvspIoErrType_e eRet = E_DRV_SCLHVSP_IO_ERR_OK;
    DrvSclHvspVersionChkConfig_t stVersion;
    MDrvSclHvspIdType_e enMdrvIdType;

    stVersion = _DrvSclHvspIoFillVersionChkStruct(sizeof(DrvSclHvspIoScalingConfig_t),
                pstIOSclCfg->VerChk_Size,
                &pstIOSclCfg->VerChk_Version);

    if(_DrvSclHvspIoVersionCheck(stVersion))
    {
        SCL_ERR( "[HVSP]   %s  \n", __FUNCTION__);
        return E_DRV_SCLHVSP_IO_ERR_INVAL;
    }

    if(_DrvSclHvspIoGetMdrvIdType(s32Handler, &enMdrvIdType) == FALSE)
    {
        SCL_ERR( "[HVSP]   %s %d \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLHVSP_IO_ERR_INVAL;
    }

    stSclCfg.stclk = (MDrvSclHvspClkConfig_t *)DrvSclOsClkGetConfig(_DrvSclHvspIoTransClkId(enMdrvIdType));
    stSclCfg.stCropWin.bEn = pstIOSclCfg->bCropEn;
    stSclCfg.stCropWin.u16Height = pstIOSclCfg->stCropWin.u16Height;
    stSclCfg.stCropWin.u16Width = pstIOSclCfg->stCropWin.u16Width;
    stSclCfg.stCropWin.u16X = pstIOSclCfg->stCropWin.u16X;
    stSclCfg.stCropWin.u16Y = pstIOSclCfg->stCropWin.u16Y;
    stSclCfg.u16Dsp_Height = pstIOSclCfg->u16Dsp_Height;
    stSclCfg.u16Dsp_Width = pstIOSclCfg->u16Dsp_Width;
    stSclCfg.u16Src_Height = pstIOSclCfg->u16Src_Height;
    stSclCfg.u16Src_Width = pstIOSclCfg->u16Src_Width;

    if(IsMdrvIdType_HVSP_1(enMdrvIdType))
    {
        MDrvSclHvspSetPriMaskInstId(MDrvSclMultiInstGetHvspQuantifyPreInstId());
    }

    if(_DrvSclHvspIoMultiInstSet(s32Handler, E_MDRV_SCLMULTI_INST_CMD_HVSP_SCALING_CONFIG, (void *)&stSclCfg) == FALSE)
    {
        eRet = E_DRV_SCLHVSP_IO_ERR_MULTI;
    }
    else
    {
        if(!MDrvSclHvspSetScalingConfig(enMdrvIdType,  &stSclCfg ))
        {
            eRet = E_DRV_SCLHVSP_IO_ERR_FAULT;
        }
        else
        {
            eRet = E_DRV_SCLHVSP_IO_ERR_OK;
        }
    }

    return eRet;
}

DrvSclHvspIoErrType_e _DrvSclHvspIoReqmemConfig(s32 s32Handler, DrvSclHvspIoReqMemConfig_t*pstReqMemCfg)
{
    MDrvSclHvspIpmConfig_t stIPMCfg;
    DrvSclHvspIoErrType_e eRet = E_DRV_SCLHVSP_IO_ERR_OK;
    DrvSclHvspVersionChkConfig_t stVersion;
    MDrvSclHvspIdType_e enMdrvIdType;

    stVersion =  _DrvSclHvspIoFillVersionChkStruct(sizeof(DrvSclHvspIoReqMemConfig_t),
                 pstReqMemCfg->VerChk_Size,
                 &pstReqMemCfg->VerChk_Version);

    if(_DrvSclHvspIoVersionCheck(stVersion))
    {
        SCL_ERR( "[HVSP]   %s %d \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLHVSP_IO_ERR_INVAL;
    }

    if(_DrvSclHvspIoGetMdrvIdType(s32Handler, &enMdrvIdType) == FALSE)
    {
        SCL_ERR( "[HVSP]   %s %d \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLHVSP_IO_ERR_INVAL;
    }

    if( !IsMdrvIdType_HVSP_1(enMdrvIdType))
    {
        SCL_ERR( "[HVSP]   %s %d \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLHVSP_IO_ERR_INVAL;
    }

    _DrvSclHvspIoCheckModifyMemSize(pstReqMemCfg);
    _DrvSclhvspFrameBufferMemoryAllocate();
    stIPMCfg = _DrvSclHvspIoFillIPMStructForDriver(pstReqMemCfg);

    if(_DrvSclHvspIoMultiInstSet(s32Handler, E_MDRV_SCLMULTI_INST_CMD_HVSP_MEM_REQ_CONFIG, (void *)&stIPMCfg) == FALSE)
    {
        eRet = E_DRV_SCLHVSP_IO_ERR_MULTI;
    }
    else
    {
        if(MDrvSclHvspSetInitIpmConfig(E_MDRV_SCLHVSP_ID_1, &stIPMCfg))
        {
            eRet = E_DRV_SCLHVSP_IO_ERR_OK;
        }
        else
        {
            eRet = E_DRV_SCLHVSP_IO_ERR_FAULT;
        }

        if(gu8FrameBufferReadyNum == 0)
        {
            eRet = E_DRV_SCLHVSP_IO_ERR_FAULT;
        }
    }

    return eRet;
}


DrvSclHvspIoErrType_e _DrvSclHvspIoSetMiscConfig(s32 s32Handler, DrvSclHvspIoMiscConfig_t *pstIOMiscCfg)
{
    MDrvSclHvspMiscConfig_t stMiscCfg;
    MDrvSclHvspIdType_e enMdrvIdType;

    if(_DrvSclHvspIoGetMdrvIdType(s32Handler, &enMdrvIdType) == FALSE)
    {
        SCL_ERR( "[HVSP]   %s %d \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLHVSP_IO_ERR_INVAL;
    }

    stMiscCfg.u8Cmd = pstIOMiscCfg->u8Cmd;
    stMiscCfg.u32Size = pstIOMiscCfg->u32Size;
    stMiscCfg.u32Addr = pstIOMiscCfg->u32Addr;

    if(MDrvSclHvspSetMiscConfig(&stMiscCfg))
    {
        return E_DRV_SCLHVSP_IO_ERR_OK;
    }
    else
    {
        return E_DRV_SCLHVSP_IO_ERR_FAULT;
    }
}

DrvSclHvspIoErrType_e _DrvSclHvspIoSetPostCropConfig(s32 s32Handler, DrvSclHvspIoPostCropConfig_t *pstIOPostCfg)
{
    MDrvSclHvspPostCropConfig_t stPostCfg;
    DrvSclHvspIoErrType_e eRet = E_DRV_SCLHVSP_IO_ERR_OK;
    DrvSclHvspVersionChkConfig_t stVersion;
    MDrvSclHvspIdType_e enMdrvIdType;

    stVersion = _DrvSclHvspIoFillVersionChkStruct(sizeof(DrvSclHvspIoPostCropConfig_t),
                pstIOPostCfg->VerChk_Size,
                &pstIOPostCfg->VerChk_Version);

    if(_DrvSclHvspIoVersionCheck(stVersion))
    {
        SCL_ERR( "[HVSP]   %s %d \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLHVSP_IO_ERR_INVAL;
    }

    if(_DrvSclHvspIoGetMdrvIdType(s32Handler, &enMdrvIdType) == FALSE)
    {
        SCL_ERR( "[HVSP]   %s %d \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLHVSP_IO_ERR_INVAL;
    }

    stPostCfg.bCropEn = pstIOPostCfg->bCropEn;
    stPostCfg.bFmCntEn = pstIOPostCfg->bFmCntEn;
    stPostCfg.stclk = (MDrvSclHvspClkConfig_t *)DrvSclOsClkGetConfig(_DrvSclHvspIoTransClkId(enMdrvIdType));
    stPostCfg.u16Height = pstIOPostCfg->u16Height;
    stPostCfg.u16Width = pstIOPostCfg->u16Width;
    stPostCfg.u16X = pstIOPostCfg->u16X;
    stPostCfg.u16Y = pstIOPostCfg->u16Y;
    stPostCfg.u8FmCnt = pstIOPostCfg->u8FmCnt;


    if(_DrvSclHvspIoMultiInstSet(s32Handler, E_MDRV_SCLMULTI_INST_CMD_HVSP_POST_CROP_CONFIG, (void *)&stPostCfg) == FALSE)
    {
        SCL_ERR( "[HVSP]   %s %d \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLHVSP_IO_ERR_MULTI;
    }
    else
    {
        MDrvSclHvspSetPriMaskInstId(MDrvSclMultiInstGetHvspQuantifyPreInstId());
        if(!MDrvSclHvspSetPostCropConfig(E_MDRV_SCLHVSP_ID_1,  &stPostCfg))
        {
            eRet = E_DRV_SCLHVSP_IO_ERR_FAULT;
        }
        else
        {
            eRet = E_DRV_SCLHVSP_IO_ERR_OK;
        }
    }

    return eRet;
}

DrvSclHvspIoErrType_e _DrvSclHvspIoGetPrivateIdConfig(s32 s32Handler, DrvSclHvspIoPrivateIdConfig_t *pstIOCfg)
{
    DrvSclHvspIoErrType_e eRet = E_DRV_SCLHVSP_IO_ERR_OK;
    DrvSclHvspMultiInstConfig_t stMultiInstCfg;

    if(_DrvSclHvspIoGetMultiInstConfig(s32Handler, &stMultiInstCfg) == FALSE)
    {
        SCL_ERR( "[HVSP]   %s %d \n", __FUNCTION__, __LINE__);
        eRet = E_DRV_SCLHVSP_IO_ERR_INVAL;
    }
    else
    {
        if(!MDrvSclMultiInstEntryGetPirvateId(stMultiInstCfg.enMultiInstId, stMultiInstCfg.pPrivateData, &pstIOCfg->s32Id))
        {
            SCL_ERR( "[HVSP]   %s %d \n", __FUNCTION__, __LINE__);
            eRet = E_DRV_SCLHVSP_IO_ERR_FAULT;
        }
        else
        {
            eRet = E_DRV_SCLHVSP_IO_ERR_OK;
        }
    }
    return eRet;
}


DrvSclHvspIoErrType_e _DrvSclHvspIoGetInformConfig(s32 s32Handler, DrvSclHvspIoScInformConfig_t *pstIOInfoCfg)
{
    MDrvSclHvspScInformConfig_t stInfoCfg;
    DrvSclHvspIoErrType_e eRet = E_DRV_SCLHVSP_IO_ERR_OK;
    MDrvSclHvspIdType_e enMdrvIdType;


    if(_DrvSclHvspIoGetMdrvIdType(s32Handler, &enMdrvIdType) == FALSE)
    {
        SCL_ERR( "[HVSP]   %s %d \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLHVSP_IO_ERR_INVAL;
    }

    if(!MDrvSclHvspGetSCLInform( enMdrvIdType,  &stInfoCfg))
    {
        eRet = E_DRV_SCLHVSP_IO_ERR_FAULT;
    }
    else
    {
        DrvSclOsMemcpy(pstIOInfoCfg, &stInfoCfg, sizeof(DrvSclHvspIoScInformConfig_t));
        eRet = E_DRV_SCLHVSP_IO_ERR_OK;
    }

    return eRet;

}

DrvSclHvspIoErrType_e _DrvSclHvspIoReleaseMemConfig(s32 s32Handler)
{

    MDrvSclHvspIdType_e enMdrvIdType;

    if(_DrvSclHvspIoGetMdrvIdType(s32Handler, &enMdrvIdType) == FALSE)
    {
        SCL_ERR( "[HVSP]   %s %d \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLHVSP_IO_ERR_INVAL;
    }

    if( !IsMdrvIdType_HVSP_1(enMdrvIdType))
    {
        SCL_ERR( "[HVSP]   %s %d \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLHVSP_IO_ERR_INVAL;
    }

    _DrvSclHvspIoMemFree();
    return E_DRV_SCLHVSP_IO_ERR_OK;
}

DrvSclHvspIoErrType_e _DrvSclHvspIoSetOsdConfig(s32 s32Handler, DrvSclHvspIoOsdConfig_t *pstIOOSDCfg)
{
    MDrvSclHvspOsdConfig_t stOSDCfg;
    DrvSclHvspIoErrType_e eRet = E_DRV_SCLHVSP_IO_ERR_OK;
    DrvSclHvspVersionChkConfig_t stVersion;
    MDrvSclHvspIdType_e enMdrvIdType;

    stVersion = _DrvSclHvspIoFillVersionChkStruct(sizeof(DrvSclHvspIoOsdConfig_t),
                (pstIOOSDCfg->VerChk_Size),
                &(pstIOOSDCfg->VerChk_Version));

    if(_DrvSclHvspIoVersionCheck(stVersion))
    {
        SCL_ERR( "[HVSP]   %s %d \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLHVSP_IO_ERR_INVAL;
    }
    else
    {
        stOSDCfg.enOSD_loc = pstIOOSDCfg->enOSD_loc;
        stOSDCfg.stOsdOnOff.bOSDEn = pstIOOSDCfg->bEn;
    }

    if(_DrvSclHvspIoGetMdrvIdType(s32Handler, &enMdrvIdType) == FALSE)
    {
        SCL_ERR( "[HVSP]   %s %d \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLHVSP_IO_ERR_INVAL;
    }

    if(_DrvSclHvspIoMultiInstSet(s32Handler, E_MDRV_SCLMULTI_INST_CMD_HVSP_SET_OSD_CONFIG, (void *)&stOSDCfg) == FALSE)
    {
        SCL_ERR( "[HVSP]   %s %d \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLHVSP_IO_ERR_MULTI;
    }
    else
    {
        if(!MDrvSclHvspSetOsdConfig(enMdrvIdType,  &stOSDCfg))
        {
            eRet = E_DRV_SCLHVSP_IO_ERR_FAULT;
        }
        else
        {
            eRet = E_DRV_SCLHVSP_IO_ERR_OK;
        }
    }
    return eRet;
}

DrvSclHvspIoErrType_e _DrvSclHvspIoSetPriMaskConfig(s32 s32Handler, DrvSclHvspIoPriMaskConfig_t *pstIOPriMaskCfg)
{
    MDrvSclHvspPriMaskConfig_t stCfg;
    DrvSclHvspIoErrType_e eRet = E_DRV_SCLHVSP_IO_ERR_OK;
    DrvSclHvspVersionChkConfig_t stVersion;
    MDrvSclHvspIdType_e enMdrvIdType;
    MDrvSclMultiInstStatusType_e enMultiInstRet;
    DrvSclHvspMultiInstConfig_t stMultiInstCfg;

    stVersion = _DrvSclHvspIoFillVersionChkStruct(sizeof(DrvSclHvspIoPriMaskConfig_t),
                (pstIOPriMaskCfg->VerChk_Size),
                &(pstIOPriMaskCfg->VerChk_Version));
    if(_DrvSclHvspIoVersionCheck(stVersion))
    {
        SCL_ERR( "[HVSP]   %s %d \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLHVSP_IO_ERR_INVAL;
    }
    else
    {
        stCfg.bMask = pstIOPriMaskCfg->bMask;
        stCfg.u8idx = pstIOPriMaskCfg->u8idx;
        DrvSclOsMemcpy(&stCfg.stMaskWin, &pstIOPriMaskCfg->stMaskWin, sizeof(DrvSclHvspIoPriMaskConfig_t));
    }

    if(_DrvSclHvspIoGetMdrvIdType(s32Handler, &enMdrvIdType) == FALSE)
    {
        SCL_ERR( "[HVSP]   %s %d \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLHVSP_IO_ERR_INVAL;
    }

    if( !IsMdrvIdType_HVSP_1(enMdrvIdType))
    {
        SCL_ERR( "[HVSP]   %s %d \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLHVSP_IO_ERR_INVAL;
    }

    if( _DrvSclHvspIoGetMultiInstConfig(s32Handler, &stMultiInstCfg) == FALSE)
    {
        SCL_ERR( "[HVSP]   %s %d \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLHVSP_IO_ERR_INVAL;
    }

    enMultiInstRet = MDrvSclMultiInstEtnryIsFree(stMultiInstCfg.enMultiInstId, stMultiInstCfg.pPrivateData);

    if(enMultiInstRet == E_MDRV_SCLMULTI_INST_STATUS_SUCCESS)
    {
        MDrvSclHvspSetPriMaskInstId(MDrvSclMultiInstGetHvspQuantifyPreInstId());
        if(!MDrvSclHvspSetPriMaskConfig(stCfg))
        {
            eRet = E_DRV_SCLHVSP_IO_ERR_FAULT;
        }
        else
        {
            eRet = E_DRV_SCLHVSP_IO_ERR_OK;
        }
    }
    else
    {
        SCL_ERR( "[HVSP]   %s %d \n", __FUNCTION__, __LINE__);
        return FALSE;
    }

    return eRet;
}

DrvSclHvspIoErrType_e _DrvSclHvspIoPirMaskTrigerConfig(s32 s32Handler, DrvSclHvspIoPriMaskTriggerConfig_t *pstIOPriMaskTrigCfg)
{
    bool bEn;
    DrvSclHvspIoErrType_e eRet = E_DRV_SCLHVSP_IO_ERR_OK;
    DrvSclHvspVersionChkConfig_t stVersion;
    MDrvSclHvspIdType_e enMdrvIdType;

    stVersion = _DrvSclHvspIoFillVersionChkStruct(sizeof(DrvSclHvspIoPriMaskTriggerConfig_t),
                (pstIOPriMaskTrigCfg->VerChk_Size),
                &(pstIOPriMaskTrigCfg->VerChk_Version));
    if(_DrvSclHvspIoVersionCheck(stVersion))
    {
        SCL_ERR( "[HVSP]   %s %d \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLHVSP_IO_ERR_INVAL;
    }
    else
    {
        bEn = pstIOPriMaskTrigCfg->bEn;
    }

    if(_DrvSclHvspIoGetMdrvIdType(s32Handler, &enMdrvIdType) == FALSE)
    {
        SCL_ERR( "[HVSP]   %s %d \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLHVSP_IO_ERR_INVAL;
    }


    if( !IsMdrvIdType_HVSP_1(enMdrvIdType))
    {
        SCL_ERR( "[HVSP]   %s %d \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLHVSP_IO_ERR_INVAL;
    }

    if(!MDrvSclHvspSetPriMaskTrigger(bEn))
    {
        eRet = E_DRV_SCLHVSP_IO_ERR_FAULT;
    }
    else
    {
        eRet = E_DRV_SCLHVSP_IO_ERR_OK;
    }

    return eRet;
}

DrvSclHvspIoErrType_e _DrvSclHvspIoSetFbConfig(s32 s32Handler, DrvSclHvspIoSetFbManageConfig_t *pstIOFbMgCfg)
{
    DrvSclHvspIoErrType_e eRet = E_DRV_SCLHVSP_IO_ERR_OK;
    DrvSclHvspVersionChkConfig_t stVersion;
    MDrvSclHvspIdType_e enMdrvIdType;

    stVersion = _DrvSclHvspIoFillVersionChkStruct(sizeof(DrvSclHvspIoSetFbManageConfig_t),
                (pstIOFbMgCfg->VerChk_Size),
                &(pstIOFbMgCfg->VerChk_Version));

    if(_DrvSclHvspIoVersionCheck(stVersion))
    {
        SCL_ERR( "[HVSP]   %s %d \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLHVSP_IO_ERR_INVAL;
    }


    if(_DrvSclHvspIoGetMdrvIdType(s32Handler, &enMdrvIdType) == FALSE)
    {
        SCL_ERR( "[HVSP]   %s %d \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLHVSP_IO_ERR_INVAL;
    }

    if(!MDrvSclHvspSetFbManageConfig((MDrvSclHvspFbmgSetType_e)pstIOFbMgCfg->enSet))
    {
        eRet = E_DRV_SCLHVSP_IO_ERR_FAULT;
    }
    else
    {
        eRet = E_DRV_SCLHVSP_IO_ERR_OK;
    }

    return eRet;
}

DrvSclHvspIoErrType_e _DrvSclHvspIoGetVersion(s32 s32Hander, DrvSclHvspIoVersionConfig_t *psIOVersionCfg)
{
    DrvSclHvspIoErrType_e eRet;

    if (CHK_VERCHK_HEADER( &(psIOVersionCfg->VerChk_Version)) )
    {
        if( CHK_VERCHK_VERSION_LESS( &(psIOVersionCfg->VerChk_Version), DRV_SCLHVSP_VERSION) )
        {

            VERCHK_ERR("[HVSP] Version(%04lx) < %04x!!! \n",
                       psIOVersionCfg->VerChk_Version & VERCHK_VERSION_MASK,
                       DRV_SCLHVSP_VERSION);

            eRet = E_DRV_SCLHVSP_IO_ERR_INVAL;
        }
        else
        {
            if( CHK_VERCHK_SIZE( &(psIOVersionCfg->VerChk_Size), sizeof(DrvSclHvspIoVersionConfig_t)) == 0 )
            {
                VERCHK_ERR("[HVSP] Size(%04x) != %04lx!!! \n",
                           sizeof(DrvSclHvspIoVersionConfig_t),
                           (psIOVersionCfg->VerChk_Size));

                eRet = E_DRV_SCLHVSP_IO_ERR_INVAL;
            }
            else
            {
                DrvSclHvspIoVersionConfig_t stCfg;

                stCfg = FILL_VERCHK_TYPE(stCfg, stCfg.VerChk_Version, stCfg.VerChk_Size, DRV_SCLHVSP_VERSION);
                stCfg.u32Version = DRV_SCLHVSP_VERSION;
                DrvSclOsMemcpy(psIOVersionCfg, &stCfg, sizeof(DrvSclHvspIoVersionConfig_t));
                eRet = E_DRV_SCLHVSP_IO_ERR_OK;
            }
        }
    }
    else
    {
        VERCHK_ERR("[HVSP] No Header !!! \n");
        SCL_ERR( "[HVSP]   %s %d \n", __FUNCTION__, __LINE__);
        eRet = E_DRV_SCLHVSP_IO_ERR_INVAL;
    }

    return eRet;
}

