////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2011 MStar Semiconductor, Inc.
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
#define __DRV_SCL_DMA_IO_WRAPPER_C__


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------

#include "drv_scl_os.h"
#include "drv_scl_dbg.h"
#include "hal_scl_util.h"

#include "drv_scl_verchk.h"
#include "drv_scl_dma_m.h"
#include "drv_scl_hvsp_m.h"
#include "drv_scl_multiinst_m_st.h"
#include "drv_scl_multiinst_m.h"
#include "drv_scl_dma_io_st.h"
#include "drv_scl_dma_io_wrapper.h"
//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------
typedef struct
{
    MDrvSclMultiInstLockIdType_e enMultiInstLockId;
    MDrvSclMultiInstEntryIdType_e enMultiInstId;
    void *pPrivateData;
}DrvSclDmaIoMultiInstConfig_t;

//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
DrvSclDmaIoHandleConfig_t _gstSclDmaHandler[DRV_SCLDMA_HANDLER_MAX];
DrvSclDmaIoFunctionConfig_t _gstSclDmaFunc;

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
DrvSclDmaIoVersionChkConfig_t _DrvSclDmaIoFillVersionChkStruct(u32 u32StructSize,u32 u32VersionSize,u32 *pVersion)
{
    DrvSclDmaIoVersionChkConfig_t stVersion;
    stVersion.u32StructSize  = (u32)u32StructSize;
    stVersion.u32VersionSize = (u32)u32VersionSize;
    stVersion.pVersion      = (u32 *)pVersion;
    return stVersion;
}
s32 _DrvSclDmaIoVersionCheck(DrvSclDmaIoVersionChkConfig_t stVersion)
{
    if ( CHK_VERCHK_HEADER(stVersion.pVersion) )
    {
        if( CHK_VERCHK_MAJORVERSION_LESS( stVersion.pVersion, DRV_SCLDMA_VERSION) )
        {

            VERCHK_ERR("[SCLDMA] Version(%04lx) < %04x!!! \n",
                *(stVersion.pVersion) & VERCHK_VERSION_MASK,
                DRV_SCLDMA_VERSION);

            return -1;
        }
        else
        {
            if( CHK_VERCHK_SIZE( &stVersion.u32VersionSize, stVersion.u32StructSize) == 0 )
            {
                VERCHK_ERR("[SCLDMA] Size(%04lx) != %04lx!!! \n",
                    stVersion.u32StructSize,
                    stVersion.u32VersionSize);

                return -1;
            }
            else
            {
                SCL_DBG(SCL_DBG_LV_IOCTL()&EN_DBGMG_IOCTLEVEL_ELSE, "[SCLDMA] Size(%ld) \n",stVersion.u32StructSize );
                return VersionCheckSuccess;
            }
        }
    }
    else
    {
        VERCHK_ERR("[SCLDMA] No Header !!! \n");
        SCL_ERR( "[SCLDMA]   %s %d  \n", __FUNCTION__, __LINE__);
        return -1;
    }
}

u8 _DrvSclDmaIoGetIdOpenTime(DrvSclDmaIoIdType_e enDmaId)
{
    s16 i = 0;
    u8 u8Cnt = 0;
    for(i = 0; i < DRV_SCLDMA_HANDLER_MAX; i++)
    {
        if(_gstSclDmaHandler[i].enSclDmaId == enDmaId && _gstSclDmaHandler[i].s32Handle != -1)
        {
            u8Cnt ++;
        }
    }
    return u8Cnt;
}

bool _DrvSclDmaIoGetMdrvIdType(s32 s32Handler, MDrvSclDmaIdType_e *penSclDmaId)
{
    s16 i;
    s16 s16Idx = -1;
    bool bRet = TRUE;

    for(i = 0; i < DRV_SCLDMA_HANDLER_MAX; i++)
    {
        if(_gstSclDmaHandler[i].s32Handle == s32Handler)
        {
            s16Idx = i;
            break;
        }
    }

    if(s16Idx == -1)
    {
        *penSclDmaId = E_MDRV_SCLDMA_ID_NUM;
        bRet = FALSE;
    }
    else
    {
        bRet = TRUE;
        switch(_gstSclDmaHandler[s16Idx].enSclDmaId)
        {
            case E_DRV_SCLDMA_IO_ID_1:
                *penSclDmaId = E_MDRV_SCLDMA_ID_1;
                break;
            case E_DRV_SCLDMA_IO_ID_2:
                *penSclDmaId = E_MDRV_SCLDMA_ID_2;
                break;
            case E_DRV_SCLDMA_IO_ID_3:
                *penSclDmaId = E_MDRV_SCLDMA_ID_3;
                break;
            case E_DRV_SCLDMA_IO_ID_4:
                *penSclDmaId = E_MDRV_SCLDMA_ID_PNL;
                break;
            default:
                bRet = FALSE;
                *penSclDmaId = E_MDRV_SCLDMA_ID_NUM;
                break;
        }

    }

    return bRet;
}

MDrvSclDmaBufferConfig_t _DrvSclDmaIoFillBufferConfig(DrvSclDmaIoBufferConfig_t *stIODMABufferCfg)
{
    MDrvSclDmaBufferConfig_t stDMABufferCfg;
    stDMABufferCfg.u8Flag = stIODMABufferCfg->u8Flag;
    stDMABufferCfg.enBufMDType = stIODMABufferCfg->enBufMDType;
    stDMABufferCfg.enColorType = stIODMABufferCfg->enColorType;
    stDMABufferCfg.enMemType = stIODMABufferCfg->enMemType;
    stDMABufferCfg.u16BufNum = stIODMABufferCfg->u16BufNum;
    stDMABufferCfg.u16Height = stIODMABufferCfg->u16Height;
    stDMABufferCfg.u16Width = stIODMABufferCfg->u16Width;
    DrvSclOsMemcpy(stDMABufferCfg.u32Base_Y,stIODMABufferCfg->u32Base_Y,sizeof(unsigned long)*BUFFER_BE_ALLOCATED_MAX);
    DrvSclOsMemcpy(stDMABufferCfg.u32Base_C,stIODMABufferCfg->u32Base_C,sizeof(unsigned long)*BUFFER_BE_ALLOCATED_MAX);
    DrvSclOsMemcpy(stDMABufferCfg.u32Base_V,stIODMABufferCfg->u32Base_V,sizeof(unsigned long)*BUFFER_BE_ALLOCATED_MAX);

    return stDMABufferCfg;
}

MDrvSclMultiInstEntryIdType_e _DrvSclDmaIoTransMultiInstId(DrvSclDmaIoIdType_e enSclDmaId)
{
    MDrvSclMultiInstEntryIdType_e enMulitInstId = E_MDRV_SCLMULTI_INST_ENTRY_ID_MAX;

    switch(enSclDmaId)
    {
        case E_DRV_SCLDMA_IO_ID_1:
            enMulitInstId = E_MDRV_SCLMULTI_INST_ENTRY_ID_SCLDMA1;
            break;

        case E_DRV_SCLDMA_IO_ID_2:
            enMulitInstId = E_MDRV_SCLMULTI_INST_ENTRY_ID_SCLDMA2;
            break;

        case E_DRV_SCLDMA_IO_ID_3:
            enMulitInstId = E_MDRV_SCLMULTI_INST_ENTRY_ID_SCLDMA3;
            break;

        case E_DRV_SCLDMA_IO_ID_4:
            enMulitInstId = E_MDRV_SCLMULTI_INST_ENTRY_ID_DISP;
            break;

        case E_DRV_SCLDMA_IO_ID_NUM:
            enMulitInstId = E_MDRV_SCLMULTI_INST_ENTRY_ID_MAX;
            break;
    }
    return enMulitInstId;
}


bool _DrvSclDmaIoGetMultiInstConfig(s32 s32Handler, DrvSclDmaIoMultiInstConfig_t *pCfg)
{
    s16 i;
    s16 s16Idx = -1;
    bool bRet = TRUE;

    for(i = 0; i < DRV_SCLDMA_HANDLER_MAX; i++)
    {
        if(_gstSclDmaHandler[i].s32Handle == s32Handler)
        {
            s16Idx = i;
            break;
        }
    }

    if(s16Idx == -1)
    {
        pCfg->enMultiInstId = E_MDRV_SCLMULTI_INST_ENTRY_ID_MAX;
        pCfg->enMultiInstLockId = E_MDRV_SCLMULTI_INST_LOCK_ID_MAX;
        pCfg->pPrivateData = NULL;
        bRet = FALSE;
    }
    else
    {
        bRet = TRUE;
        pCfg->pPrivateData = _gstSclDmaHandler[s16Idx].pPrivate_Data;
        switch(_gstSclDmaHandler[s16Idx].enSclDmaId)
        {
            case E_DRV_SCLDMA_IO_ID_1:
                pCfg->enMultiInstLockId = E_MDRV_SCLMULTI_INST_LOCK_ID_SC_1_2;
                pCfg->enMultiInstId     = E_MDRV_SCLMULTI_INST_ENTRY_ID_SCLDMA1;
                break;
            case E_DRV_SCLDMA_IO_ID_2:
                pCfg->enMultiInstLockId = E_MDRV_SCLMULTI_INST_LOCK_ID_SC_1_2;
                pCfg->enMultiInstId     = E_MDRV_SCLMULTI_INST_ENTRY_ID_SCLDMA2;
                break;
            case E_DRV_SCLDMA_IO_ID_3:
                pCfg->enMultiInstLockId = E_MDRV_SCLMULTI_INST_LOCK_ID_SC_3;
                pCfg->enMultiInstId     = E_MDRV_SCLMULTI_INST_ENTRY_ID_SCLDMA3;
                break;
            case E_DRV_SCLDMA_IO_ID_4:
                pCfg->enMultiInstLockId = E_MDRV_SCLMULTI_INST_LOCK_ID_DISP;
                pCfg->enMultiInstId     = E_MDRV_SCLMULTI_INST_ENTRY_ID_DISP;
                break;
            default:
                bRet = FALSE;
                pCfg->enMultiInstLockId = E_MDRV_SCLMULTI_INST_ENTRY_ID_MAX;
                pCfg->enMultiInstId     = E_MDRV_SCLMULTI_INST_ENTRY_ID_MAX;
                break;
        }
    }

    return bRet;
}

static bool _DrvSclDmaIoMultiInstSet(s32 s32Handler, MDrvSclMultiInstCmdType_e enType, void *stCfg)
{
    MDrvSclMultiInstStatusType_e enMultiInstRet;
    bool bRet = 0;
    DrvSclDmaIoMultiInstConfig_t stMultiInstCfg;

    if(_DrvSclDmaIoGetMultiInstConfig(s32Handler, &stMultiInstCfg) == FALSE)
    {
        SCL_DBG(SCL_DBG_LV_0, "%s %d:: _DrvSclHvspMultiInstSet fail\n", __FUNCTION__, __LINE__);
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

DrvSclOsClkIdType_e _DrvSclDmaIoTransClkId(MDrvSclDmaIdType_e enMDrvSclDmaId)
{
    DrvSclOsClkIdType_e enClkId = E_DRV_SCLOS_CLK_ID_NUM;
    switch(enMDrvSclDmaId)
    {
        case E_MDRV_SCLDMA_ID_1:
            enClkId = E_DRV_SCLOS_CLK_ID_DMA1;
            break;

        case E_MDRV_SCLDMA_ID_2:
            enClkId = E_DRV_SCLOS_CLK_ID_DMA2;
            break;

        case E_MDRV_SCLDMA_ID_3:
            enClkId = E_DRV_SCLOS_CLK_ID_DMA3;
            break;

        case E_MDRV_SCLDMA_ID_PNL:
            enClkId = E_DRV_SCLOS_CLK_ID_DMA4;
            break;

        case E_MDRV_SCLDMA_ID_NUM:
            enClkId = E_DRV_SCLOS_CLK_ID_NUM;
            break;
    }
    return enClkId;
}

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
void _DrvSclDmaIoInitHandler(DrvSclDmaIoIdType_e enSclDmaId)
{
    u16 i, start, end;
    start = enSclDmaId * MDRV_SCLMULTI_INST_PRIVATE_ID_NUM;
    end = start + MDRV_SCLMULTI_INST_PRIVATE_ID_NUM;

    for(i=start; i<end; i++)
    {
        _gstSclDmaHandler[i].s32Handle = -1;
        _gstSclDmaHandler[i].pPrivate_Data = NULL;
        _gstSclDmaHandler[i].enSclDmaId = E_DRV_SCLDMA_IO_ID_NUM;
    }

    if(enSclDmaId == E_DRV_SCLDMA_IO_ID_1)
    {
        DrvSclOsMemset(&_gstSclDmaFunc, 0, sizeof(DrvSclDmaIoFunctionConfig_t));
        _gstSclDmaFunc.DrvSclDmaIoSetInBufferConfig           = _DrvSclDmaIoSetInBufferConfig;
        _gstSclDmaFunc.DrvSclDmaIoSetInTriggerConfig          = _DrvSclDmaIoSetInTriggerConfig;
        _gstSclDmaFunc.DrvSclDmaIoSetOutBufferConfig          = _DrvSclDmaIoSetOutBufferConfig;
        _gstSclDmaFunc.DrvSclDmaIoSetOutTriggerConfig         = _DrvSclDmaIoSetOutTriggerConfig;
        _gstSclDmaFunc.DrvSclDmaIoGetInformationConfig        = _DrvSclDmaIoGetInformationConfig;
        _gstSclDmaFunc.DrvSclDmaIoGetInActiveBufferConfig     = _DrvSclDmaIoGetInActiveBufferConfig;
        _gstSclDmaFunc.DrvSclDmaIoGetOutActiveBufferConfig    = _DrvSclDmaIoGetOutActiveBufferConfig;
        _gstSclDmaFunc.DrvSclDmaIoBufferQueueHandleConfig     = _DrvSclDmaIoBufferQueueHandleConfig;
        _gstSclDmaFunc.DrvSclDmaIoGetPrivateIdConfig          = _DrvSclDmaIoGetPrivateIdConfig;
        _gstSclDmaFunc.DrvSclDmaIoSetLockConfig               = _DrvSclDmaIoSetLockConfig;
        _gstSclDmaFunc.DrvSclDmaIoSetUnlockConfig             = _DrvSclDmaIoSetUnlockConfig;
        _gstSclDmaFunc.DrvSclDmaIoGetVersion                  = _DrvSclDmaIoGetVersion;
    }
}

s32 _DrvSclDmaIoOpen(DrvSclDmaIoIdType_e enSclDmaId)
{
    s32 s32Handle = -1;
    s16 s16Idx = -1;
    s16 i ;

    for(i=0; i<DRV_SCLDMA_HANDLER_MAX; i++)
    {
        if(_gstSclDmaHandler[i].s32Handle == -1)
        {
            s16Idx = i;
            break;
        }
    }

    if(s16Idx == -1)
    {
        s32Handle = -1;
    }
    else
    {
        if(MDrvSclMultiInstEntryAlloc(_DrvSclDmaIoTransMultiInstId(enSclDmaId), &_gstSclDmaHandler[s16Idx].pPrivate_Data) == 0)
        {
            s32Handle = -1;
        }
        else
        {
            MDrvSclDmaIdType_e enMdrvIdType;
            s32Handle = s16Idx | DRV_SCLDMA_HANDLER_PRE_FIX;
            _gstSclDmaHandler[s16Idx].s32Handle = s32Handle ;
            _gstSclDmaHandler[s16Idx].enSclDmaId = enSclDmaId;
            if(_DrvSclDmaIoGetIdOpenTime(enSclDmaId)== 1)
            {
                _DrvSclDmaIoGetMdrvIdType(s32Handle, &enMdrvIdType);
                if(enSclDmaId == E_DRV_SCLDMA_IO_ID_1)
                {
                    MDrvSclDmaSysInit(1);
                }
            }

        }
    }

    return s32Handle;
}

DrvSclDmaIoErrType_e _DrvSclDmaIoRelease(s32 s32Handler)
{
    s16 s16Idx = -1;
    s16 i ;
    DrvSclDmaIoErrType_e eRet = E_DRV_SCLDMA_IO_ERR_OK;

    for(i=0; i<DRV_SCLDMA_HANDLER_MAX; i++)
    {
        if(_gstSclDmaHandler[i].s32Handle == s32Handler)
        {
            s16Idx = i;
            break;
        }
    }

    if(s16Idx == -1)
    {
        SCL_ERR( "[SCLDMA]   %s %d  \n", __FUNCTION__, __LINE__);
        eRet = E_DRV_SCLDMA_IO_ERR_FAULT;
    }
    else
    {

        MDrvSclDmaIdType_e enMdrvIdType;

        if(_DrvSclDmaIoGetMdrvIdType(s32Handler, &enMdrvIdType) == FALSE)
        {
            SCL_ERR( "[SCLDMA]   %s %d  \n", __FUNCTION__, __LINE__);
            eRet = E_DRV_SCLDMA_IO_ERR_INVAL;
        }
        else
        {
            DrvSclDmaIoMultiInstConfig_t stMultiInstCfg;
            if(_DrvSclDmaIoGetMultiInstConfig(s32Handler, &stMultiInstCfg) == FALSE)
            {
                SCL_ERR( "[SCLDMA]   %s %d  \n", __FUNCTION__, __LINE__);
                eRet = E_DRV_SCLDMA_IO_ERR_INVAL;
            }
            else
            {
                MDrvSclMultiInstEntryFree( stMultiInstCfg.enMultiInstId, _gstSclDmaHandler[s16Idx].pPrivate_Data);

                if( !IsMDrvScldmaIdType_2(enMdrvIdType))
                {
                    MDrvSclMultiInstLockFree(stMultiInstCfg.enMultiInstLockId, NULL);
                }
                if(_DrvSclDmaIoGetIdOpenTime(_gstSclDmaHandler[s16Idx].enSclDmaId)== 1)
                {
                    MDrvSclDmaRelease(enMdrvIdType,  (MDrvSclDmaClkConfig_t *)DrvSclOsClkGetConfig(_DrvSclDmaIoTransClkId(enMdrvIdType)));
                }
                _gstSclDmaHandler[s16Idx].s32Handle = -1;
                _gstSclDmaHandler[s16Idx].enSclDmaId = E_DRV_SCLDMA_IO_ID_NUM;
                _gstSclDmaHandler[s16Idx].pPrivate_Data = NULL;
                eRet = E_DRV_SCLDMA_IO_ERR_OK;
            }
        }
    }
    return eRet;
}
void _DrvSclDmaIoPollLinux(MDrvSclDmaIdType_e enMdrvIdType, DrvSclDmaIoWrapperPollConfig_t *pstIoInCfg)
{
    bool bFRMDone;
    MDrvSclDmaBufferDoneConfig_t stDoneCfg;

    pstIoInCfg->u8retval = 0;

    MDrvSclDmaSetPollWait(pstIoInCfg->stPollWaitCfg);
    if(enMdrvIdType == E_MDRV_SCLDMA_ID_3 || enMdrvIdType == E_MDRV_SCLDMA_ID_PNL)
    {
        bFRMDone = 0;
        if(MDrvSclDmaGetOutBufferDoneEvent(enMdrvIdType, E_MDRV_SCLDMA_MEM_FRM, &stDoneCfg))
        {
            bFRMDone = stDoneCfg.bDone ? 1: 0;
        }
        else
        {
            bFRMDone = 2;
        }

        if(bFRMDone == 1)
        {
            pstIoInCfg->u8retval |= (enMdrvIdType == E_MDRV_SCLDMA_ID_3) ?
                                    (POLLIN | POLLOUT) :
                                    (POLLOUT | POLLWRNORM);
        }
        else
        {
            pstIoInCfg->u8retval = 0;
        }
    }
    else
    {
        bFRMDone = 2;

        if(MDrvSclDmaGetOutBufferDoneEvent(enMdrvIdType, E_MDRV_SCLDMA_MEM_FRM, &stDoneCfg))
        {
            bFRMDone = (stDoneCfg.bDone == 0x1 ) ? 0x1 :
                       (stDoneCfg.bDone == 0x2 ) ? 0x2 :
                       (stDoneCfg.bDone == 0x3 ) ? 0x3 :
                       (stDoneCfg.bDone == 0xF ) ? 0xF : 0;
        }
        else
        {
            bFRMDone = 0;
        }
        if(bFRMDone &&bFRMDone!=0xF)
        {
            if(bFRMDone& 0x1)
            {
                pstIoInCfg->u8retval |= POLLIN; /* read */
            }
            if(bFRMDone& 0x2)
            {
                pstIoInCfg->u8retval |= POLLPRI;
            }
        }
        else if(bFRMDone ==0)
        {
            pstIoInCfg->u8retval = 0;
        }
        else
        {
            pstIoInCfg->u8retval = POLLERR;
        }
    }
}


DrvSclDmaIoErrType_e _DrvSclDmaIoGetPollEventCfg(s32 s32Handler, s32 *ps32EventId, u32 *pu32WaitEvent)
{
    MDrvSclDmaIdType_e enMdrvIdType;
    s32 *s32EventId = NULL;
    u32 u32WaitEvent = 0;

    if(_DrvSclDmaIoGetMdrvIdType(s32Handler, &enMdrvIdType) == FALSE)
    {
        SCL_ERR( "[SCLDMA]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    s32EventId = MDrvSclDmaGetWaitQueueHead(enMdrvIdType);
    u32WaitEvent = MDrvSclDmaGetWaitEvent(enMdrvIdType);

    *ps32EventId = *s32EventId;
    *pu32WaitEvent = u32WaitEvent;
    return E_DRV_SCLDMA_IO_ERR_OK;
}

void _DrvSclDmaIoPollRtk(MDrvSclDmaIdType_e enMdrvIdType, DrvSclDmaIoWrapperPollConfig_t *pstIoInCfg)
{
    bool bFRMDone;
    MDrvSclDmaBufferDoneConfig_t stDoneCfg;
    u32 u32Timeout;
    u32 u32Time = 0;
    u32Time = (((u32)DrvSclOsGetSystemTimeStamp()));
    bFRMDone = 2;
    while(1)
    {
        if(MDrvSclDmaGetOutBufferDoneEvent(enMdrvIdType, E_MDRV_SCLDMA_MEM_FRM, &stDoneCfg))
        {
            bFRMDone = (stDoneCfg.bDone == 0x1 )? 0x1 :
                       (stDoneCfg.bDone == 0x2 )? 0x2 :
                       (stDoneCfg.bDone == 0x3 )? 0x3 :
                       (stDoneCfg.bDone == 0xF ) ? 0xF : 0;
        }
        else
        {
            bFRMDone = 0;
        }
        if(bFRMDone &&bFRMDone!=0xF)
        {
            if(bFRMDone& 0x1)
            {
                pstIoInCfg->u8retval |= 0x1; /* read */
            }
            if(bFRMDone& 0x2)
            {
                pstIoInCfg->u8retval |= 0x2;
            }
        }
        else if(bFRMDone ==0)
        {
            pstIoInCfg->u8retval = 0;
        }
        else
        {
            pstIoInCfg->u8retval = 0x8;//POLLERR
        }
        pstIoInCfg->u8retval &= (pstIoInCfg->u8pollval|0x8);

        if( pstIoInCfg->pfnCb )
        {
            if(pstIoInCfg->u8retval)
            {
                pstIoInCfg->pfnCb();
            }
            break;
        }
        else
        {
            if(pstIoInCfg->u8retval)
            {
                break;
            }
            else
            {
                u32Timeout = MDrvSclDmaSetPollWait(pstIoInCfg->stPollWaitCfg);
                if(u32Timeout)
                {
                    SCL_ERR("%s %d, POLL TIMEOUT :%d @:%lu\n",__FUNCTION__, __LINE__,enMdrvIdType,u32Time);
                    break;
                }
            }
        }
    }
}

DrvSclDmaIoErrType_e _DrvSclDmaIoPoll(s32 s32Handler, DrvSclDmaIoWrapperPollConfig_t *pstIoInCfg)
{
    MDrvSclDmaIdType_e enMdrvIdType;
    MDrvSclMultiInstStatusType_e enMultiInstRet;
    DrvSclDmaIoMultiInstConfig_t stMultiInstCfg;

    if(_DrvSclDmaIoGetMdrvIdType(s32Handler, &enMdrvIdType) == FALSE)
    {
        SCL_ERR( "[SCLDMA]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }
    if(IsMDrvScldmaIdType_Max(enMdrvIdType))
    {
        SCL_ERR( "[SCLDMA]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    if(_DrvSclDmaIoGetMultiInstConfig(s32Handler, &stMultiInstCfg) == FALSE)
    {
        SCL_ERR( "[SCLDMA]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }
    enMultiInstRet = MDrvSclMultiInstEtnryIsFree(stMultiInstCfg.enMultiInstId, stMultiInstCfg.pPrivateData);

    if(enMultiInstRet == E_MDRV_SCLMULTI_INST_STATUS_SUCCESS)
    {
        if(pstIoInCfg->bWaitQueue)
        {
            _DrvSclDmaIoPollLinux(enMdrvIdType, pstIoInCfg);
        }
        else
        {
            _DrvSclDmaIoPollRtk(enMdrvIdType, pstIoInCfg);
        }
    }
    else
    {
        pstIoInCfg->u8retval = 0;
        return  E_DRV_SCLDMA_IO_ERR_MULTI;
    }

    return E_DRV_SCLDMA_IO_ERR_OK;
}

DrvSclDmaIoErrType_e _DrvSclDmaIoSetInBufferConfig(s32 s32Handler, DrvSclDmaIoBufferConfig_t *pstIoInCfg)
{
    MDrvSclDmaBufferConfig_t stDMABufferCfg;
    DrvSclDmaIoVersionChkConfig_t stVersion;
    MDrvSclDmaIdType_e enMdrvIdType;

    stVersion =  _DrvSclDmaIoFillVersionChkStruct(sizeof(DrvSclDmaIoBufferConfig_t),
                                                 pstIoInCfg->VerChk_Size,
                                                 &pstIoInCfg->VerChk_Version);

    if(_DrvSclDmaIoVersionCheck(stVersion))
    {
        SCL_ERR( "[SCLDMA]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    if(_DrvSclDmaIoGetMdrvIdType(s32Handler, &enMdrvIdType) == FALSE)
    {
        SCL_ERR( "[SCLDMA]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    if(IsMDrvScldmaIdType_Max(enMdrvIdType))
    {
        SCL_ERR( "[SCLDMA]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    //scldma1, scldma2 not support
    if(IsMDrvScldmaIdType_1(enMdrvIdType) || IsMDrvScldmaIdType_2(enMdrvIdType))
    {
        SCL_ERR( "[SCLDMA] Not Support %s %d\n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    //do when id is scldma3 & scldma4
    stDMABufferCfg=_DrvSclDmaIoFillBufferConfig(pstIoInCfg);

    if(_DrvSclDmaIoMultiInstSet(s32Handler, E_MDRV_SCLMULTI_INST_CMD_SCLDMA_IN_BUFFER_CONFIG, (void *)&stDMABufferCfg) == FALSE)
    {
        return E_DRV_SCLDMA_IO_ERR_MULTI;
    }
    else
    {
        if(!MDrvSclDmaSetDmaReadClientConfig(enMdrvIdType,  &stDMABufferCfg))
        {
            return E_DRV_SCLDMA_IO_ERR_FAULT;
        }
    }

    return E_DRV_SCLDMA_IO_ERR_OK;
}

DrvSclDmaIoErrType_e _DrvSclDmaIoSetInTriggerConfig(s32 s32Handler, DrvSclDmaIoTriggerConfig_t *pstIoInCfg)
{
    MDrvSclDmaTriggerConfig_t stDrvTrigCfg;
    DrvSclDmaIoVersionChkConfig_t stVersion;
    MDrvSclDmaIdType_e enMdrvIdType;

    stVersion =  _DrvSclDmaIoFillVersionChkStruct(sizeof(DrvSclDmaIoTriggerConfig_t),
                                                 pstIoInCfg->VerChk_Size,
                                                 &pstIoInCfg->VerChk_Version);

    if(_DrvSclDmaIoVersionCheck(stVersion))
    {
        SCL_ERR( "[SCLDMA]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    if(_DrvSclDmaIoGetMdrvIdType(s32Handler, &enMdrvIdType) == FALSE)
    {
        SCL_ERR( "[SCLDMA]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    if(IsMDrvScldmaIdType_Max(enMdrvIdType))
    {
        SCL_ERR( "[SCLDMA]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    //scldma1, scldma2 not support
    if(IsMDrvScldmaIdType_1(enMdrvIdType) || IsMDrvScldmaIdType_2(enMdrvIdType))
    {
        SCL_ERR( "[SCLDMA] Not Support %s %d\n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    //do when id is scldma3 & scldma4
    stDrvTrigCfg.bEn = pstIoInCfg->bEn;
    stDrvTrigCfg.enMemType = pstIoInCfg->enMemType;
    stDrvTrigCfg.stclk = (MDrvSclDmaClkConfig_t *)DrvSclOsClkGetConfig(_DrvSclDmaIoTransClkId(enMdrvIdType));

    if(_DrvSclDmaIoMultiInstSet(s32Handler, E_MDRV_SCLMULTI_INST_CMD_SCLDMA_IN_TRIGGER_CONFIG, (void *)&stDrvTrigCfg) == FALSE)
    {
        return E_DRV_SCLDMA_IO_ERR_MULTI;
    }
    else
    {
        if(!MDrvSclDmaSetDmaReadClientTrigger(enMdrvIdType,  &stDrvTrigCfg))
        {
            return E_DRV_SCLDMA_IO_ERR_FAULT;
        }
    }

    return E_DRV_SCLDMA_IO_ERR_OK;
}

DrvSclDmaIoErrType_e _DrvSclDmaIoSetOutBufferConfig(s32 s32Handler, DrvSclDmaIoBufferConfig_t *pstIoInCfg)
{
    MDrvSclDmaBufferConfig_t stDMABufferCfg;
    DrvSclDmaIoVersionChkConfig_t stVersion;
    MDrvSclDmaIdType_e enMdrvIdType;

    stVersion =  _DrvSclDmaIoFillVersionChkStruct(sizeof(DrvSclDmaIoBufferConfig_t),
                                                 pstIoInCfg->VerChk_Size,
                                                 &pstIoInCfg->VerChk_Version);

    if(_DrvSclDmaIoVersionCheck(stVersion))
    {
        SCL_ERR( "[SCLDMA]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }


    if(_DrvSclDmaIoGetMdrvIdType(s32Handler, &enMdrvIdType) == FALSE)
    {
        SCL_ERR( "[SCLDMA]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    if(IsMDrvScldmaIdType_Max(enMdrvIdType))
    {
        SCL_ERR( "[SCLDMA]   %s %d  \n", __FUNCTION__, __LINE__);
        return FALSE;
    }

    //scldma4 not support
    if(IsMDrvScldmaIdType_PNL(enMdrvIdType) )
    {
        SCL_ERR( "[SCLDMA] Not Support %s %d\n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    stDMABufferCfg = _DrvSclDmaIoFillBufferConfig(pstIoInCfg);

    if(_DrvSclDmaIoMultiInstSet(s32Handler, E_MDRV_SCLMULTI_INST_CMD_SCLDMA_OUT_BUFFER_CONFIG, (void *)&stDMABufferCfg) == FALSE)
    {
        return E_DRV_SCLDMA_IO_ERR_MULTI;
    }
    else
    {
        if(!MDrvSclDmaSetDmaWriteClientConfig(enMdrvIdType,  &stDMABufferCfg))
        {
            return E_DRV_SCLDMA_IO_ERR_FAULT;
        }
    }

    return E_DRV_SCLDMA_IO_ERR_OK;
}

DrvSclDmaIoErrType_e _DrvSclDmaIoSetOutTriggerConfig(s32 s32Handler, DrvSclDmaIoTriggerConfig_t *pstIoInCfg)
{
    //DrvSclDmaIoErrType_e bret = FALSE;
    MDrvSclDmaTriggerConfig_t stDrvTrigCfg;
    DrvSclDmaIoVersionChkConfig_t stVersion;
    MDrvSclDmaIdType_e enMdrvIdType;

    stVersion =  _DrvSclDmaIoFillVersionChkStruct(sizeof(DrvSclDmaIoTriggerConfig_t),
                                                 pstIoInCfg->VerChk_Size,
                                                 &pstIoInCfg->VerChk_Version);

    if(_DrvSclDmaIoVersionCheck(stVersion))
    {
        SCL_ERR( "[SCLDMA]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    if(_DrvSclDmaIoGetMdrvIdType(s32Handler, &enMdrvIdType) == FALSE)
    {
        SCL_ERR( "[SCLDMA]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    if(IsMDrvScldmaIdType_Max(enMdrvIdType))
    {
        SCL_ERR( "[SCLDMA]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    //scldma4 not support
    if(IsMDrvScldmaIdType_PNL(enMdrvIdType))
    {
        SCL_ERR( "[SCLDMA] Not support %s %d\n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    //do when id is scldma1, scldma2 & scldma3
    stDrvTrigCfg.bEn = pstIoInCfg->bEn;
    stDrvTrigCfg.enMemType = pstIoInCfg->enMemType;
    stDrvTrigCfg.stclk = (MDrvSclDmaClkConfig_t *)DrvSclOsClkGetConfig(_DrvSclDmaIoTransClkId(enMdrvIdType));

    if(_DrvSclDmaIoMultiInstSet(s32Handler, E_MDRV_SCLMULTI_INST_CMD_SCLDMA_OUT_TRIGGER_CONFIG, (void *)&stDrvTrigCfg) == FALSE)
    {
        return E_DRV_SCLDMA_IO_ERR_MULTI;
    }
    else
    {
        if(!MDrvSclDmaSetDmaWriteClientTrigger(enMdrvIdType,  &stDrvTrigCfg))
        {
            return E_DRV_SCLDMA_IO_ERR_FAULT;
        }
    }
    return E_DRV_SCLDMA_IO_ERR_OK;
}

DrvSclDmaIoErrType_e _DrvSclDmaIoGetInformationConfig(s32 s32Handler, DrvSclDmaIoGetInformationConfig_t *pstIoInCfg)
{
    MDrvSclDmaAttrType_t stDmaInfo;
    DrvSclDmaIoVersionChkConfig_t stVersion;
    MDrvSclDmaIdType_e enMdrvIdType;
    u32 u32Bufferidx;

    stVersion =  _DrvSclDmaIoFillVersionChkStruct(sizeof(DrvSclDmaIoGetInformationConfig_t),
                                                 pstIoInCfg->VerChk_Size,
                                                 &pstIoInCfg->VerChk_Version);

    if(_DrvSclDmaIoVersionCheck(stVersion))
    {
        SCL_ERR( "[SCLDMA]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    if(_DrvSclDmaIoGetMdrvIdType(s32Handler, &enMdrvIdType) == FALSE)
    {
        SCL_ERR( "[SCLDMA]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    if(IsMDrvScldmaIdType_Max(enMdrvIdType))
    {
        SCL_ERR( "[SCLDMA]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    if(pstIoInCfg->enMemType == E_DRV_SCLDMA_IO_MEM_FRM)
    {
        stDmaInfo = MDrvSclDmaGetDmaInformationByClient(E_MDRV_SCLDMA_ID_1, E_MDRV_SCLDMA_MEM_FRM,0);
    }
    else if(pstIoInCfg->enMemType == E_DRV_SCLDMA_IO_MEM_SNP)
    {
        stDmaInfo = MDrvSclDmaGetDmaInformationByClient(E_MDRV_SCLDMA_ID_1, E_MDRV_SCLDMA_MEM_SNP,0);
    }
    else if(pstIoInCfg->enMemType == E_DRV_SCLDMA_IO_MEM_IMI)
    {
        stDmaInfo = MDrvSclDmaGetDmaInformationByClient(E_MDRV_SCLDMA_ID_1, E_MDRV_SCLDMA_MEM_IMI,0);
    }
    else
    {
        SCL_ERR( "[SCLDMA] Not Support %s %d\n", __FUNCTION__, __LINE__);
    }
    pstIoInCfg->enBufMDType = stDmaInfo.enBufMDType;
    pstIoInCfg->enColorType = stDmaInfo.enColorType;
    pstIoInCfg->u16BufNum = stDmaInfo.u16BufNum;
    pstIoInCfg->u16DMAH = stDmaInfo.u16DMAH;
    pstIoInCfg->u16DMAV = stDmaInfo.u16DMAV;
    for(u32Bufferidx=0;u32Bufferidx<BUFFER_BE_ALLOCATED_MAX;u32Bufferidx++)
    {
        pstIoInCfg->u32Base_C[u32Bufferidx] = stDmaInfo.u32Base_C[u32Bufferidx];
        pstIoInCfg->u32Base_V[u32Bufferidx] = stDmaInfo.u32Base_V[u32Bufferidx];
        pstIoInCfg->u32Base_Y[u32Bufferidx] = stDmaInfo.u32Base_Y[u32Bufferidx];
    }

    return E_DRV_SCLDMA_IO_ERR_OK;

}
DrvSclDmaIoErrType_e _DrvSclDmaIoGetInActiveBufferConfig(s32 s32Handler, DrvSclDmaIoActiveBufferConfig_t *pstIoInCfg)
{
    MDrvSclDmaActiveBufferConfig_t stActiveCfg;
    DrvSclDmaIoVersionChkConfig_t stVersion;
    MDrvSclDmaIdType_e enMdrvIdType;
    MDrvSclMultiInstStatusType_e enMultiInstRet;
    DrvSclDmaIoMultiInstConfig_t stMultiInstCfg;
    DrvSclDmaIoErrType_e eRet = E_DRV_SCLDMA_IO_ERR_OK;

    stVersion =  _DrvSclDmaIoFillVersionChkStruct(sizeof(DrvSclDmaIoActiveBufferConfig_t),
                                                 pstIoInCfg->VerChk_Size,
                                                 &pstIoInCfg->VerChk_Version);

    if(_DrvSclDmaIoVersionCheck(stVersion))
    {
        SCL_ERR( "[SCLDMA]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    if(_DrvSclDmaIoGetMdrvIdType(s32Handler, &enMdrvIdType) == FALSE)
    {
        SCL_ERR( "[SCLDMA]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    if(IsMDrvScldmaIdType_Max(enMdrvIdType))
    {
        SCL_ERR( "[SCLDMA]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    //scldma1, scldma2 not support
    if(IsMDrvScldmaIdType_1(enMdrvIdType) || IsMDrvScldmaIdType_2(enMdrvIdType))
    {
        SCL_ERR( "[SCLDMA] Not support %s %d\n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    if(_DrvSclDmaIoGetMultiInstConfig(s32Handler, &stMultiInstCfg) == FALSE)
    {
        SCL_ERR( "[SCLDMA]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_FAULT;
    }

    stActiveCfg.enMemType = pstIoInCfg->enMemType;
    stActiveCfg.u8ActiveBuffer = pstIoInCfg->u8ActiveBuffer;
    stActiveCfg.stOnOff.stclk = (MDrvSclDmaClkConfig_t *)DrvSclOsClkGetConfig(_DrvSclDmaIoTransClkId(enMdrvIdType));

    enMultiInstRet = MDrvSclMultiInstEtnryIsFree(stMultiInstCfg.enMultiInstId, stMultiInstCfg.pPrivateData);

    if(enMultiInstRet == E_MDRV_SCLMULTI_INST_STATUS_LOCKED)
    {
        stActiveCfg.u8ActiveBuffer = 0xFF;
        eRet = E_DRV_SCLDMA_IO_ERR_MULTI;
    }
    else if(enMultiInstRet == E_MDRV_SCLMULTI_INST_STATUS_FAIL)
    {
        eRet = E_DRV_SCLDMA_IO_ERR_MULTI;
    }
    else
    {

        if(!MDrvSclDmaGetDmaReadBufferActiveIdx(enMdrvIdType,  &stActiveCfg))
        {
            eRet = E_DRV_SCLDMA_IO_ERR_FAULT;
        }
        else
        {
            pstIoInCfg->u8ActiveBuffer  = stActiveCfg.u8ActiveBuffer;
            pstIoInCfg->enMemType       = stActiveCfg.enMemType;
            pstIoInCfg->u8ISPcount      = stActiveCfg.u8ISPcount;
            pstIoInCfg->u64FRMDoneTime  = stActiveCfg.u64FRMDoneTime;
            eRet = E_DRV_SCLDMA_IO_ERR_OK;
        }
    }

    return eRet;

}
DrvSclDmaIoErrType_e _DrvSclDmaIoGetOutActiveBufferConfig(s32 s32Handler, DrvSclDmaIoActiveBufferConfig_t *pstIoInCfg)
{
    MDrvSclDmaActiveBufferConfig_t stActiveCfg;
    DrvSclDmaIoVersionChkConfig_t stVersion;
    MDrvSclDmaIdType_e enMdrvIdType;
    MDrvSclMultiInstStatusType_e enMultiInstRet;
    DrvSclDmaIoErrType_e eRet = E_DRV_SCLDMA_IO_ERR_OK;
    DrvSclDmaIoMultiInstConfig_t stMultiInstCfg;

    stVersion =  _DrvSclDmaIoFillVersionChkStruct(sizeof(DrvSclDmaIoActiveBufferConfig_t),
                                                 pstIoInCfg->VerChk_Size,
                                                 &pstIoInCfg->VerChk_Version);

    if(_DrvSclDmaIoVersionCheck(stVersion))
    {
        SCL_ERR( "[SCLDMA]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    if(_DrvSclDmaIoGetMdrvIdType(s32Handler, &enMdrvIdType) == FALSE)
    {
        SCL_ERR( "[SCLDMA]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    if(IsMDrvScldmaIdType_Max(enMdrvIdType))
    {
        SCL_ERR( "[SCLDMA]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    //scldma4 not support
    if(IsMDrvScldmaIdType_PNL(enMdrvIdType) )
    {
        SCL_ERR( "[SCLDMA] Notsupport %s %d\n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    if(_DrvSclDmaIoGetMultiInstConfig(s32Handler, &stMultiInstCfg) == FALSE)
    {
        SCL_ERR( "[SCLDMA]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    stActiveCfg.enMemType = pstIoInCfg->enMemType;
    stActiveCfg.u8ActiveBuffer = pstIoInCfg->u8ActiveBuffer;
    stActiveCfg.stOnOff.stclk = (MDrvSclDmaClkConfig_t *)DrvSclOsClkGetConfig(_DrvSclDmaIoTransClkId(enMdrvIdType));


    enMultiInstRet = MDrvSclMultiInstEtnryIsFree(stMultiInstCfg.enMultiInstId, stMultiInstCfg.pPrivateData);

    if(enMultiInstRet == E_MDRV_SCLMULTI_INST_STATUS_LOCKED)
    {
        stActiveCfg.u8ActiveBuffer = 0xFF;
        eRet = E_DRV_SCLDMA_IO_ERR_MULTI;

    }
    else if(enMultiInstRet == E_MDRV_SCLMULTI_INST_STATUS_FAIL)
    {
        eRet = E_DRV_SCLDMA_IO_ERR_MULTI;
    }
    else
    {

        if(!MDrvSclDmaGetDmaWriteBufferAcitveIdx(enMdrvIdType, &stActiveCfg))
        {
            eRet = E_DRV_SCLDMA_IO_ERR_FAULT;
        }
        else
        {
            pstIoInCfg->u8ActiveBuffer  = stActiveCfg.u8ActiveBuffer;
            pstIoInCfg->enMemType       = stActiveCfg.enMemType;
            pstIoInCfg->u8ISPcount      = stActiveCfg.u8ISPcount;
            pstIoInCfg->u64FRMDoneTime  = stActiveCfg.u64FRMDoneTime;
            eRet = E_DRV_SCLDMA_IO_ERR_OK;
        }

    }
    return eRet;
}
DrvSclDmaIoErrType_e _DrvSclDmaIoBufferQueueHandleConfig(s32 s32Handler, DrvSclDmaIoBufferQueueConfig_t *pstIoInCfg)
{
    MDrvSclDmaBUfferQueueConfig_t stBufferQCfg;
    DrvSclDmaIoVersionChkConfig_t stVersion;
    MDrvSclDmaIdType_e enMdrvIdType;
    MDrvSclMultiInstStatusType_e enMultiInstRet;
    DrvSclDmaIoMultiInstConfig_t stMultiInstCfg;
    DrvSclDmaIoErrType_e eRet = E_DRV_SCLDMA_IO_ERR_OK;

    stVersion =  _DrvSclDmaIoFillVersionChkStruct(sizeof(DrvSclDmaIoBufferQueueConfig_t),
                                                 pstIoInCfg->VerChk_Size,
                                                 &pstIoInCfg->VerChk_Version);

    if(_DrvSclDmaIoVersionCheck(stVersion))
    {
        SCL_ERR( "[SCLDMA]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    if(_DrvSclDmaIoGetMdrvIdType(s32Handler, &enMdrvIdType) == FALSE)
    {
        SCL_ERR( "[SCLDMA]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    if(_DrvSclDmaIoGetMultiInstConfig(s32Handler, &stMultiInstCfg) == FALSE)
    {
        SCL_ERR( "[SCLDMA]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    stBufferQCfg.enMemType = pstIoInCfg->enMemType;
    stBufferQCfg.enUsedType = pstIoInCfg->enUsedType;
    stBufferQCfg.u8EnqueueIdx = pstIoInCfg->u8EnqueueIdx;

    enMultiInstRet = MDrvSclMultiInstEtnryIsFree(stMultiInstCfg.enMultiInstId, stMultiInstCfg.pPrivateData);
    if(enMultiInstRet == E_MDRV_SCLMULTI_INST_STATUS_LOCKED)
    {
        eRet = E_DRV_SCLDMA_IO_ERR_FAULT;
    }
    else if(enMultiInstRet == E_MDRV_SCLMULTI_INST_STATUS_FAIL)
    {
        eRet = E_DRV_SCLDMA_IO_ERR_FAULT;
    }
    else
    {

        if(!MDrvSclDmaBufferQueueHandle(enMdrvIdType,  &stBufferQCfg))
        {
            eRet = E_DRV_SCLDMA_IO_ERR_FAULT;
        }
        else
        {
            DrvSclOsMemcpy(&pstIoInCfg->stRead,&stBufferQCfg.stRead,MDRV_SCLDMA_BUFFER_QUEUE_OFFSET);
            pstIoInCfg->u8InQueueCount = stBufferQCfg.u8InQueueCount;
            pstIoInCfg->u8EnqueueIdx   = stBufferQCfg.u8EnqueueIdx;
            eRet = E_DRV_SCLDMA_IO_ERR_OK;
        }
    }

    return eRet;
}

DrvSclDmaIoErrType_e _DrvSclDmaIoGetPrivateIdConfig(s32 s32Handler, DrvSclDmaIoPrivateIdConfig_t *pstIoInCfg)
{
    DrvSclDmaIoErrType_e eRet = E_DRV_SCLDMA_IO_ERR_OK;
    DrvSclDmaIoMultiInstConfig_t stMultiInstCfg;

    if(_DrvSclDmaIoGetMultiInstConfig(s32Handler, &stMultiInstCfg) == FALSE)
    {
        SCL_ERR( "[SCLDMA]   %s %d \n", __FUNCTION__, __LINE__);
        eRet = E_DRV_SCLDMA_IO_ERR_INVAL;
    }
    else
    {
        if(!MDrvSclMultiInstEntryGetPirvateId(stMultiInstCfg.enMultiInstId, stMultiInstCfg.pPrivateData, &pstIoInCfg->s32Id))
        {
            SCL_ERR( "[SCLDMA]   %s %d \n", __FUNCTION__, __LINE__);
            eRet = E_DRV_SCLDMA_IO_ERR_FAULT;
        }
        else
        {
            eRet = E_DRV_SCLDMA_IO_ERR_OK;
        }

    }
    return eRet;
}


DrvSclDmaIoErrType_e _DrvSclDmaIoSetLockConfig(s32 s32Handler, DrvSclDmaIoLockConfig_t *pstIoInCfg)
{
    MDrvSclMultiInstLockConfig_t stMultiInstLockCfg;
    DrvSclDmaIoVersionChkConfig_t stVersion;
    DrvSclDmaIoMultiInstConfig_t stMultiInstCfg;
    MDrvSclDmaIdType_e enMdrvIdType;

    stVersion = _DrvSclDmaIoFillVersionChkStruct(sizeof(DrvSclDmaIoLockConfig_t),
                                              (pstIoInCfg->VerChk_Size),
                                              &(pstIoInCfg->VerChk_Version));

    if(_DrvSclDmaIoVersionCheck(stVersion))
    {
        SCL_ERR( "[SCLDMA]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    if(pstIoInCfg->ps32IdBuf == NULL || *(pstIoInCfg->ps32IdBuf) == 0)
    {
        SCL_ERR( "[SCLDMA]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    if(_DrvSclDmaIoGetMdrvIdType(s32Handler, &enMdrvIdType) == FALSE)
    {
        SCL_ERR( "[SCLDMA]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    if(_DrvSclDmaIoGetMultiInstConfig(s32Handler, &stMultiInstCfg) == FALSE)
    {
        SCL_ERR( "[SCLDMA]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    if( IsMDrvScldmaIdType_2(enMdrvIdType))
    {
        SCL_ERR( "[SCLDMA]   %s %d: SCLDMA2 not support  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    stMultiInstLockCfg.ps32PrivateID    = pstIoInCfg->ps32IdBuf;
    stMultiInstLockCfg.u8IDNum          = pstIoInCfg->u8BufSize;

    if( !MDrvSclMultiInstLockAlloc(stMultiInstCfg.enMultiInstLockId, stMultiInstLockCfg) )
    {
        return E_DRV_SCLDMA_IO_ERR_FAULT;
    }

    return E_DRV_SCLDMA_IO_ERR_OK;

}

DrvSclDmaIoErrType_e _DrvSclDmaIoSetUnlockConfig(s32 s32Handler, DrvSclDmaIoLockConfig_t *pstIoInCfg)
{
    MDrvSclMultiInstLockConfig_t stMultiInstLockCfg;
    DrvSclDmaIoVersionChkConfig_t stVersion;
    DrvSclDmaIoMultiInstConfig_t stMultiInstCfg;
    MDrvSclDmaIdType_e enMdrvIdType;

    stVersion = _DrvSclDmaIoFillVersionChkStruct(sizeof(DrvSclDmaIoLockConfig_t),
        (pstIoInCfg->VerChk_Size),
        &(pstIoInCfg->VerChk_Version));

    if(_DrvSclDmaIoVersionCheck(stVersion))
    {
        SCL_ERR( "[SCLDMA]   %s  \n", __FUNCTION__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    if(pstIoInCfg->ps32IdBuf == NULL || *(pstIoInCfg->ps32IdBuf) == 0)
    {
        SCL_ERR( "[SCLDMA]   %s  \n", __FUNCTION__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    if(_DrvSclDmaIoGetMdrvIdType(s32Handler, &enMdrvIdType) == FALSE)
    {
        SCL_ERR( "[SCLDMA]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    if( IsMDrvScldmaIdType_2(enMdrvIdType))
    {
        SCL_ERR( "[SCLDMA]   %s %d: SCLDMA2 Not Support  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    if(_DrvSclDmaIoGetMultiInstConfig(s32Handler, &stMultiInstCfg) == FALSE)
    {
        SCL_ERR( "[SCLDMA]   %s %d  \n", __FUNCTION__, __LINE__);
        return E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    stMultiInstLockCfg.ps32PrivateID    = pstIoInCfg->ps32IdBuf;
    stMultiInstLockCfg.u8IDNum          = pstIoInCfg->u8BufSize;

    if( !MDrvSclMultiInstLockFree(stMultiInstCfg.enMultiInstLockId, &stMultiInstLockCfg) )
    {
        return E_DRV_SCLDMA_IO_ERR_FAULT;
    }

    return E_DRV_SCLDMA_IO_ERR_OK;
}

DrvSclDmaIoErrType_e _DrvSclDmaIoGetVersion(s32 s32Handler, DrvSclDmaIoVersionConfig_t*pstIoInCfg)
{
    DrvSclDmaIoErrType_e eRet = E_DRV_SCLDMA_IO_ERR_OK;

    if (CHK_VERCHK_HEADER( &(pstIoInCfg->VerChk_Version)) )
    {
        if( CHK_VERCHK_MAJORVERSION_LESS( &(pstIoInCfg->VerChk_Version), DRV_SCLDMA_VERSION) )
        {

            VERCHK_ERR("[SCLDMA] Version(%04lx) < %04x!!! \n",
                pstIoInCfg->VerChk_Version & VERCHK_VERSION_MASK,
                DRV_SCLDMA_VERSION);

            eRet = E_DRV_SCLDMA_IO_ERR_INVAL;
        }
        else
        {
            if( CHK_VERCHK_SIZE( &(pstIoInCfg->VerChk_Size),sizeof(DrvSclDmaIoLockConfig_t)) == 0 )
            {
                VERCHK_ERR("[SCLDMA] Size(%04x) != %04lx!!! \n",
                    sizeof(DrvSclDmaIoVersionChkConfig_t),
                    (pstIoInCfg->VerChk_Size));

                eRet = E_DRV_SCLDMA_IO_ERR_INVAL;
            }
            else
            {
                DrvSclDmaIoVersionConfig_t stCfg;

                stCfg = FILL_VERCHK_TYPE(stCfg, stCfg.VerChk_Version, stCfg.VerChk_Size, DRV_SCLDMA_VERSION);
                stCfg.u32Version = DRV_SCLDMA_VERSION;
                DrvSclOsMemcpy(pstIoInCfg, &stCfg, sizeof(DrvSclDmaIoVersionConfig_t));
                eRet = E_DRV_SCLDMA_IO_ERR_OK;
            }
        }
    }
    else
    {
        VERCHK_ERR("[SCLDMA] No Header !!! \n");
        SCL_ERR( "[SCLDMA]   %s %d  \n", __FUNCTION__, __LINE__);
        eRet = E_DRV_SCLDMA_IO_ERR_INVAL;
    }

    return eRet;
}
