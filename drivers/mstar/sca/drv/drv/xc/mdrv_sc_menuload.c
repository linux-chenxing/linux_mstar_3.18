////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2008-2009 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// (!§MStar Confidential Information!‥) by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////
//==============================================================================
#define DRV_SC_MENULOAD_C

// Common Definition
#ifdef MSOS_TYPE_LINUX_KERNEL
#include <linux/string.h>
#include <linux/wait.h>
#else
#include <string.h>
#endif
#include "MsCommon.h"
#include "MsOS.h"
#include "mhal_xc_chip_config.h"

//#include "Debug.h"
#include "xc_Analog_Reg.h"
#include "xc_hwreg_utility2.h"
#include "drvXC_IOPort.h"
#include "apiXC.h"
#include "mvideo_context.h"
#include "drv_sc_ip.h"
#include "mhal_sc.h"
#include "mhal_menuload.h"
#include "drv_sc_menuload.h"
#include "halCHIP.h"

#define  MLDBG(x) //(printf("[MLOAD] "), x)
#define  MLG(x) //(printf("[MLG] "), x)

#define _AlignTo(value, align)  ( ((value) + ((align)-1)) & ~((align)-1) )

typedef struct
{
    MS_U16 u16WPoint;
    MS_U16 u16RPoint;
    MS_U16 u16FPoint;
    MS_PHYADDR PhyAddr;
    MS_U16 u16MaxCmdCnt;
    MS_BOOL bEnable;
    MS_U32 u32VirAddr;
}MS_MLoad_Info;

#if defined(MSOS_TYPE_NOS)

typedef union _MS_MLoad_Data
{
    struct _CMD
    {
    	MS_U16 u16Data;
    	MS_U8 u8Addr;
    	MS_U8 u8Bank;
    }CMD;
    MS_U32 u32Cmd;
}MS_MLoad_Data;

#else

typedef struct
{
    union
    {
        struct
        {
        	MS_U16 u16Data;
        	MS_U8 u8Addr;
        	MS_U8 u8Bank;
        };
        MS_U32 u32Cmd;
    };
}MS_MLoad_Data;

#endif

typedef struct
{
    MS_U16 u16B;
    MS_U16 u16G;
    MS_U16 u16R;
    MS_U16 u16Enable;
    MS_U16 u16Dummy[4];
} MS_SC_MLG_TBL;


typedef enum
{
    SC_MLOAD_SC0 = 0,
    SC_MLOAD_SC1 = 1,
    SC_MLOAD_SC2 = 2,
    SC_MLOAD_MAX = 3,
}EN_SC_MLOAD_TYPE;


MS_MLoad_Info stMLoadInfo[SC_MLOAD_MAX] = {{0,0,0,0,0,FALSE}, {0,0,0,0,0,FALSE}, {0,0,0,0,0,FALSE}};
MS_MLoad_Info stMLGInfo   = {0,0,0,0,0,FALSE};

static EN_SC_MLOAD_TYPE _TransWinType(SCALER_WIN eWindow)
{
    if(eWindow == MAIN_WINDOW || eWindow == SUB_WINDOW)
        return SC_MLOAD_SC0;
    else if(eWindow == SC2_MAIN_WINDOW || eWindow == SC2_SUB_WINDOW)
        return SC_MLOAD_SC2;
    else
        return SC_MLOAD_SC1;
}
#if 0
void dumpcmd(void)
{
    MS_U16 i, j;
    MS_U8 *p = (MS_U8 *)MS_PA2KSEG1(stMLoadInfo[SC_MLOAD_SC0].PhyAddr);

    MLDBG(printf("dump cmd buffer: %d\n", stMLoadInfo[SC_MLOAD_SC0].u16WPoint));
    for (i = 0; i < stMLoadInfo[SC_MLOAD_SC0].u16WPoint; i++)
    {
        printf("%06d: ", i);
        for (j = 0; j < MS_MLOAD_CMD_LEN; j++)
        {
            printf("%02x", p[i*MS_MLOAD_CMD_LEN + j]);
        }
        printf(" \n");
    }
}
#endif

MS_BOOL MDrv_XC_MLoad_Check_Done(SCALER_WIN eWindow)
{
    MS_BOOL bEn;

    if(Hal_XC_MLoad_get_status(eWindow))
        bEn = FALSE;
    else
        bEn = TRUE;
    return bEn;
}

void MDrv_XC_MLoad_Init(MS_PHYADDR phyAddr, SCALER_WIN eWindow)
{
    MS_U8 u8MIUSel = 0;
    MS_U16 u16Cmd;
    EN_SC_MLOAD_TYPE enMLoadType = _TransWinType(eWindow);

    MLDBG(printf("%s: 0x%lx\n", __FUNCTION__, phyAddr));

    if (phyAddr >= HAL_MIU1_BASE)
    {
        u8MIUSel = 1;
        MLDBG(printf("%s: Init at MIU 1\n", __FUNCTION__));
    }
    else
    {
        u8MIUSel = 0;
        MLDBG(printf("%s: Init at MIU 0\n", __FUNCTION__));
    }

    //Hal_XC_MLoad_set_on_off(DISABLE);
    Hal_XC_MLoad_Set_req_th(0x86, eWindow);
    Hal_XC_MLoad_set_trigger_timing(TRIG_SRC_DELAY_LINE, eWindow); //By REG_MLOAD_TRIG_DLY
    Hal_XC_MLoad_set_trigger_delay(0x05, eWindow);
    Hal_XC_MLoad_set_trig_p(0x08, 0x0A, eWindow);

    Hal_XC_MLoad_set_miusel(u8MIUSel, eWindow);

    if (MDrv_XC_MLoad_GetCaps(eWindow))
    {
        for (u16Cmd = 0; u16Cmd < 32; u16Cmd++)
        {
            MDrv_XC_MLoad_AddCmd(MS_MLOAD_NULL_CMD, eWindow);
        }

        MDrv_XC_MLoad_AddNull(eWindow);
        stMLoadInfo[enMLoadType].u16FPoint = stMLoadInfo[enMLoadType].u16WPoint - 1;

        // when system is power on, the read count of MLoad may be not 0.
        // If we enable MLoad RIU_En, the garbage data in fifo will over-write
        // unexpected register.
        // Hence, add below code before Kickoff to avoid it.

        // 1001_02[4] = 1, RIU_XARB不收menuload過來的register
        Hal_XC_MLoad_set_riu_cs(TRUE);
        MsOS_DelayTask(1);

        // menuload a RIU_XARB enable, FIFO 裡面的東西因為無法寫到RIU_XARB, 所以還沒法寫
        Hal_XC_MLoad_Set_riu(1, eWindow);

        // 切bank到0xFF, 搶bank switch的主動權
        MDrv_WriteByte(0x102F00, 0xFF);

        // 1001_02[4] = 0, FIFO裡面的東西開始往RIU_XARB吐, 因為menuload是disable的, 所以bank切換的主動權不在menuload
        Hal_XC_MLoad_set_riu_cs(FALSE);

        MsOS_DelayTask(1);

        MDrv_XC_MLoad_KickOff(eWindow);
    }
}

MS_BOOL MDrv_XC_MLoad_GetCaps(SCALER_WIN eWindow)
{
    return Hal_XC_MLoad_GetCaps(eWindow);
}

void MDrv_XC_MLoad_Trigger(MS_PHYADDR startAddr, MS_U16 u16CmdCnt, SCALER_WIN eWindow)
{
    MLDBG(printf("%s 0x%lx, %u\n\n",__FUNCTION__, startAddr, u16CmdCnt));

    //dumpcmd();
    #if 1

    Hal_XC_MLoad_set_base_addr(startAddr, eWindow);
    Hal_XC_MLoad_set_depth(u16CmdCnt, eWindow);
    Hal_XC_MLoad_set_len(MS_MLOAD_REG_LEN, eWindow);//length of DMA request
    Hal_XC_MLoad_set_on_off(ENABLE, eWindow);
    MsOS_DelayTaskUs(3);
    Hal_XC_MLoad_Set_riu(ENABLE, eWindow);

    #else
    Hal_XC_MLoad_trigger(startAddr, MS_MLOAD_REG_LEN, (MS_U8)u16CmdCnt);
    #endif
}

void MDrv_XC_MLoad_AddCmd(MS_U32 u32Cmd, SCALER_WIN eWindow)
{
    MS_U32 *pu32Addr;
    MS_PHYADDR DstAddr;
    EN_SC_MLOAD_TYPE enMLoadType = _TransWinType(eWindow);

#if defined(MSOS_TYPE_LINUX_KERNEL)
    DstAddr = stMLoadInfo[enMLoadType].u32VirAddr + stMLoadInfo[enMLoadType].u16WPoint * MS_MLOAD_CMD_LEN;
    pu32Addr = (MS_U32 *)(DstAddr);
#else
    DstAddr = stMLoadInfo[enMLoadType].PhyAddr + stMLoadInfo[enMLoadType].u16WPoint * MS_MLOAD_CMD_LEN;
    pu32Addr = (MS_U32 *)MS_PA2KSEG1(DstAddr);
#endif
    *pu32Addr = u32Cmd;
    stMLoadInfo[enMLoadType].u16WPoint++;
}

void MDrv_XC_MLoad_SetCmd(MS_PHYADDR DstAddr, MS_U32 u32Cmd)
{
    MS_U32 *pu32Addr;
#if defined(MSOS_TYPE_LINUX_KERNEL)
    pu32Addr = (MS_U32 *)DstAddr;
#else
    pu32Addr = (MS_U32 *)MS_PA2KSEG1(DstAddr);
#endif
    *pu32Addr = u32Cmd;
}

MS_U32 MDrv_XC_MLoad_GetCmd(MS_PHYADDR DstAddr)
{
    MS_U32 u32Addr;

#if defined(MSOS_TYPE_LINUX_KERNEL)
    u32Addr = DstAddr;
#else
    u32Addr = MS_PA2KSEG1(DstAddr);
#endif
    return (MS_U32)(*(MS_U32 *)u32Addr);
}

MS_BOOL MDrv_XC_MLoad_BufferEmpty(SCALER_WIN eWindow)
{
    EN_SC_MLOAD_TYPE enMLoadType = _TransWinType(eWindow);

    if(MDrv_XC_MLoad_Check_Done(eWindow) &&
       (stMLoadInfo[enMLoadType].u16WPoint == stMLoadInfo[enMLoadType].u16RPoint) &&
       (stMLoadInfo[enMLoadType].u16WPoint == stMLoadInfo[enMLoadType].u16FPoint+1))
        return TRUE;
    else
        return FALSE;
}

//-------------------------------------------------------------------------------------------------
/// Write command to the Menuload buffer
/// @param  u32Cmd                 \b IN: the command to write into the buffer
/// @return  TRUE if succeed, FALSE if failed
//-------------------------------------------------------------------------------------------------
MS_BOOL MDrv_XC_MLoad_WriteCmd(MS_U32 u32Cmd, SCALER_WIN eWindow)
{
    MS_BOOL bRet;
    MS_U16 u16DummyCmdIdx;
    MS_U16 u16QueueCmd;
    EN_SC_MLOAD_TYPE enMLoadType = _TransWinType(eWindow);

    if( MDrv_XC_MLoad_BufferEmpty(eWindow) )
    {
        stMLoadInfo[enMLoadType].u16RPoint = 0;
        stMLoadInfo[enMLoadType].u16WPoint = 0;
        stMLoadInfo[enMLoadType].u16FPoint = 0;
        MDrv_XC_MLoad_AddCmd(u32Cmd, eWindow);
        bRet = TRUE;
    }
    else
    {
        u16DummyCmdIdx = _AlignTo(stMLoadInfo[enMLoadType].u16WPoint+1 + 16 , 4);

        if (u16DummyCmdIdx == stMLoadInfo[enMLoadType].u16WPoint+1)
        {
            u16DummyCmdIdx = stMLoadInfo[enMLoadType].u16WPoint+1+MS_MLOAD_CMD_ALIGN;
        }

        if(u16DummyCmdIdx < stMLoadInfo[enMLoadType].u16MaxCmdCnt)
        {
            u16QueueCmd = stMLoadInfo[enMLoadType].u16WPoint - stMLoadInfo[enMLoadType].u16RPoint + 1;

            if(u16QueueCmd == MS_MLOAD_MAX_CMD_CNT - 1) //max cmd depth is MS_MLOAD_MAX_CMD_CNT
            {
                MDrv_XC_MLoad_AddCmd(MS_MLOAD_NULL_CMD, eWindow);
            }
            MDrv_XC_MLoad_AddCmd(u32Cmd, eWindow);

            bRet = TRUE;

        }
        else
        {
            MLDBG(printf("WPoint=%x, MaxCnt=%x, DummyIdx=%x\n", stMLoadInfo[enMLoadType].u16WPoint, stMLoadInfo[enMLoadType].u16MaxCmdCnt, u16DummyCmdIdx));
            bRet = FALSE;
        }
    }

    return bRet;
}

MS_BOOL MDrv_XC_MLoad_KickOff(SCALER_WIN eWindow)
{
    MS_BOOL bRet;
    MS_U16 u16CmdCnt;
    MS_U32 u32EndCmd;
    MS_U16 u16QueueCmd, u16FireIdx;
    MS_PHYADDR CmdBufAddr;
    EN_SC_MLOAD_TYPE enMLoadType = _TransWinType(eWindow);

    MLDBG(printf("%s\n",__FUNCTION__));

    if(MDrv_XC_MLoad_Check_Done(eWindow) && stMLoadInfo[enMLoadType].u16FPoint)
    {
        if(stMLoadInfo[enMLoadType].u16FPoint > stMLoadInfo[enMLoadType].u16RPoint)
        {
            u16QueueCmd = stMLoadInfo[enMLoadType].u16WPoint - stMLoadInfo[enMLoadType].u16RPoint;

            if(u16QueueCmd > MS_MLOAD_MAX_CMD_CNT)
            {
                u16FireIdx = stMLoadInfo[enMLoadType].u16RPoint + MS_MLOAD_MAX_CMD_CNT - 1;
                MLDBG(printf("Queue Too Many !!!!!!!!!!!!!!!!!\n"));
                MLDBG(printf("WPoint=%d, FirePoint=%d, RPoint=%d\n", stMLoadInfo[enMLoadType].u16WPoint, stMLoadInfo[enMLoadType].u16FPoint, stMLoadInfo[enMLoadType].u16RPoint));
            }
            else
            {
                u16FireIdx = stMLoadInfo[enMLoadType].u16FPoint;
            }

            MLDBG(printf("u16QueueCmd= %u, u16FireIdx= %u\n", u16QueueCmd, u16FireIdx));

            //check the last command to see whether it is null command
        #if defined(MSOS_TYPE_LINUX_KERNEL)
            CmdBufAddr = stMLoadInfo[enMLoadType].u32VirAddr + u16FireIdx * MS_MLOAD_CMD_LEN;
        #else
            CmdBufAddr = stMLoadInfo[enMLoadType].PhyAddr + u16FireIdx * MS_MLOAD_CMD_LEN;
        #endif

            u32EndCmd = MDrv_XC_MLoad_GetCmd(CmdBufAddr);
            if(u32EndCmd != MS_MLOAD_NULL_CMD)
            {
                MLDBG(printf("WPoint=%d, FirePoint=%d, RPoint=%d\n", stMLoadInfo[enMLoadType].u16WPoint, stMLoadInfo[enMLoadType].u16FPoint, stMLoadInfo[enMLoadType].u16RPoint));
                MLDBG(printf("KickOff: Non Null Cmd\n"));
                MS_ASSERT(0);
                bRet = FALSE;
            }
            else
            {
                u16CmdCnt = u16FireIdx - stMLoadInfo[enMLoadType].u16RPoint + 1;


                u32EndCmd = enMLoadType == SC_MLOAD_SC0 ? MS_MLOAD_END_CMD(((MS_U32)u16CmdCnt)):
                                                          MS_MLOAD_END_CMD_SC1(((MS_U32)u16CmdCnt));

                MLDBG(printf("CmdBufAddr = 0x%lx, u32EndCmd = 0x%lx\n", CmdBufAddr, u32EndCmd));
                MDrv_XC_MLoad_SetCmd(CmdBufAddr, u32EndCmd);
                MsOS_FlushMemory();

                CmdBufAddr = stMLoadInfo[enMLoadType].PhyAddr + stMLoadInfo[enMLoadType].u16RPoint * MS_MLOAD_CMD_LEN;
                MDrv_XC_MLoad_Trigger(CmdBufAddr, u16CmdCnt, eWindow);
                stMLoadInfo[enMLoadType].u16RPoint = u16FireIdx + 1;
                bRet = TRUE;
                MLDBG(printf("after fire u16WPoint=%u u16RPoint=%u u16FPoint=%u\n",
                    stMLoadInfo[enMLoadType].u16WPoint,
                    stMLoadInfo[enMLoadType].u16RPoint,
                    stMLoadInfo[enMLoadType].u16FPoint));
            }
        }
        else if(stMLoadInfo[enMLoadType].u16FPoint == stMLoadInfo[enMLoadType].u16RPoint &&
                stMLoadInfo[enMLoadType].u16FPoint == stMLoadInfo[enMLoadType].u16WPoint-1)
        {
            bRet = TRUE;
        }
        else
        {
            MLDBG(printf("MenuLoad: WPoint=%u, FPoint=%u, RPoint=%u !!\n",
                    stMLoadInfo[enMLoadType].u16WPoint,
                    stMLoadInfo[enMLoadType].u16FPoint,
                    stMLoadInfo[enMLoadType].u16RPoint));
            bRet = FALSE;
        }
    }
    else
    {
        bRet = TRUE;
    }
    return bRet;
}

void MDrv_XC_MLoad_AddNull(SCALER_WIN eWindow)
{
    EN_SC_MLOAD_TYPE enMLoadType = _TransWinType(eWindow);

    MLDBG(printf("%s\n",__FUNCTION__));

    do
    {
        MDrv_XC_MLoad_AddCmd(MS_MLOAD_NULL_CMD, eWindow);
    }while (stMLoadInfo[enMLoadType].u16WPoint%MS_MLOAD_CMD_ALIGN != 0);
}

void MApi_XC_MLoad_ResetGlobalVariable(void)
{
    if(_MLOAD_MUTEX)
    {
		OS_DELETE_MUTEX(_MLOAD_MUTEX);
		_MLOAD_MUTEX = -1;
    }

}
//-------------------------------------------------------------------------------------------------
/// Initialize the Menuload
/// @param  PhyAddr                 \b IN: the physical address for the menuload
/// @param  u32BufByteLen           \b IN: the buffer length of the menu load commands
//-------------------------------------------------------------------------------------------------
#if defined(MSOS_TYPE_LINUX_KERNEL)
void MApi_XC_MLoad_Init(MS_PHYADDR PhyAddr, MS_U32 u32VirAddr, MS_U32 u32BufByteLen)
#else
void MApi_XC_MLoad_Init(MS_PHYADDR PhyAddr, MS_U32 u32BufByteLen)
#endif
{
    char word[] = {"_MLOAD_MUTEX"};
    MS_PHYADDR SC0_PhyAddr, SC1_PhyAddr;
    MS_U32 u32MLoadBufSize;
    MS_U32 u32MLoadNum = 1;
#if defined(MSOS_TYPE_LINUX_KERNEL)
    MS_U32 u32SC0_Vir,  u32SC1_Vir;
#endif

    MLDBG(printf("%s: 0x%lx, 0x%lx\n", __FUNCTION__, PhyAddr, u32BufByteLen));


    if (_MLOAD_MUTEX != -1)
    {
        (printf("[MAPI MLOAD][%06d] Initialization more than once\n", __LINE__));
        return;
    }

    _MLOAD_MUTEX = MsOS_CreateMutex(E_MSOS_FIFO, word, MSOS_PROCESS_SHARED);
    if (_MLOAD_MUTEX == -1)
    {
        (printf("[MAPI MLOAD][%06d] create mutex fail\n", __LINE__));
        return;
    }

    if(MDrv_XC_MLoad_GetCaps(SC1_MAIN_WINDOW))
    {
        u32MLoadNum++;
    }


    u32MLoadBufSize = u32BufByteLen / u32MLoadNum;

    SC0_PhyAddr = PhyAddr;
    SC1_PhyAddr = SC0_PhyAddr + u32MLoadBufSize;

#if defined(MSOS_TYPE_LINUX_KERNEL)
    u32SC0_Vir = u32VirAddr;
    u32SC1_Vir = u32VirAddr + u32MLoadBufSize;
#endif

#if defined(MSOS_TYPE_LINUX_KERNEL)
    memset((void *)u32VirAddr, 0x00, u32BufByteLen);
#else
    memset((void *)MS_PA2KSEG1(PhyAddr), 0x00, u32BufByteLen);
#endif

    if(MDrv_XC_MLoad_GetCaps(MAIN_WINDOW))
    {
        _MLOAD_ENTRY();

        stMLoadInfo[SC_MLOAD_SC0].bEnable = FALSE;
        stMLoadInfo[SC_MLOAD_SC0].u16RPoint = 0;
        stMLoadInfo[SC_MLOAD_SC0].u16WPoint = 0;
        stMLoadInfo[SC_MLOAD_SC0].u16FPoint = 0;
        stMLoadInfo[SC_MLOAD_SC0].u16MaxCmdCnt = (MS_U16)(u32MLoadBufSize / MS_MLOAD_CMD_LEN);
        stMLoadInfo[SC_MLOAD_SC0].PhyAddr = SC0_PhyAddr;
#if defined(MSOS_TYPE_LINUX_KERNEL)
        stMLoadInfo[SC_MLOAD_SC0].u32VirAddr = u32SC0_Vir;
#endif
        MDrv_XC_MLoad_Init(PhyAddr, MAIN_WINDOW);

        _MLOAD_RETURN();
    }

    if(MDrv_XC_MLoad_GetCaps(SC1_MAIN_WINDOW))
    {
        _MLOAD_ENTRY();

        stMLoadInfo[SC_MLOAD_SC1].bEnable = FALSE;
        stMLoadInfo[SC_MLOAD_SC1].u16RPoint = 0;
        stMLoadInfo[SC_MLOAD_SC1].u16WPoint = 0;
        stMLoadInfo[SC_MLOAD_SC1].u16FPoint = 0;
        stMLoadInfo[SC_MLOAD_SC1].u16MaxCmdCnt = (MS_U16)(u32MLoadBufSize / MS_MLOAD_CMD_LEN);
        stMLoadInfo[SC_MLOAD_SC1].PhyAddr = SC1_PhyAddr;
#if defined(MSOS_TYPE_LINUX_KERNEL)
        stMLoadInfo[SC_MLOAD_SC0].u32VirAddr = u32SC1_Vir;
#endif

        MDrv_XC_MLoad_Init(SC1_PhyAddr, SC1_MAIN_WINDOW);

        printf("--Init SC1 MLoad \r\n");

        _MLOAD_RETURN();
    }


}

//-------------------------------------------------------------------------------------------------
/// Enable/Disable the MLoad
/// @return  void
//-------------------------------------------------------------------------------------------------
void MApi_XC_MLoad_Enable(MS_BOOL bEnable)
{

    _MLOAD_ENTRY();

    stMLoadInfo[SC_MLOAD_SC0].bEnable = bEnable;

    _MLOAD_RETURN();
}

//-------------------------------------------------------------------------------------------------
/// Get the status of MLoad
/// @return  MLOAD_TYPE
//-------------------------------------------------------------------------------------------------
MLOAD_TYPE MApi_XC_MLoad_GetStatus(SCALER_WIN eWindow)
{
    MLOAD_TYPE type = E_MLOAD_UNSUPPORTED;
    MS_U8 u8MLoadIdx;

    switch(eWindow)
    {
    default:
    case MAIN_WINDOW:
    case SUB_WINDOW:
        u8MLoadIdx = SC_MLOAD_SC0;
        break;

    case SC1_MAIN_WINDOW:
        u8MLoadIdx = SC_MLOAD_SC1;
        break;

    case SC2_MAIN_WINDOW:
    case SC2_SUB_WINDOW:
        u8MLoadIdx = SC_MLOAD_SC2;
        break;

    }

    _MLOAD_ENTRY();

    if (MDrv_XC_MLoad_GetCaps(eWindow))
    {
        if (stMLoadInfo[u8MLoadIdx].bEnable)
        {
            type = E_MLOAD_ENABLED;
        }
        else
        {
            type = E_MLOAD_DISABLED;
        }
    }

    _MLOAD_RETURN();
    return type;
}

//-------------------------------------------------------------------------------------------------
/// Fire the Menuload commands
/// @return  TRUE if succeed, FALSE if failed
//-------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_MLoad_Fire(MS_BOOL bImmediate)
{
    MS_U32 u32Delayms = 0;
    MS_U16 u16Boundary = 16, i = 0;

    MLDBG(printf("%s, u16WPoint=%u u16RPoint=%u u16FPoint=%u\n", __FUNCTION__,
        stMLoadInfo[SC_MLOAD_SC0].u16WPoint,
        stMLoadInfo[SC_MLOAD_SC0].u16RPoint,
        stMLoadInfo[SC_MLOAD_SC0].u16FPoint));

    Hal_XC_MLoad_set_sw_dynamic_idx_en(ENABLE); // for T8/T9 patch

    // 16 entry as boundary for menuload to prevent cmd sent unpredictable.
    for ( ; i < u16Boundary ; i++)
    {
        MDrv_XC_MLoad_AddCmd(MS_MLOAD_NULL_CMD, MAIN_WINDOW);
    }

    MDrv_XC_MLoad_AddNull(MAIN_WINDOW);

    stMLoadInfo[SC_MLOAD_SC0].u16FPoint = stMLoadInfo[SC_MLOAD_SC0].u16WPoint - 1;

    MLDBG(printf("u16WPoint=%u u16RPoint=%u u16FPoint=%u\n",
        stMLoadInfo[SC_MLOAD_SC0].u16WPoint,
        stMLoadInfo[SC_MLOAD_SC0].u16RPoint,
        stMLoadInfo[SC_MLOAD_SC0].u16FPoint));

    MDrv_XC_MLoad_KickOff(MAIN_WINDOW);

    if(bImmediate)
    {    // we need to make sure the value is
        while((MDrv_XC_MLoad_Check_Done(MAIN_WINDOW) == FALSE) && u32Delayms < 20)
        {
            MsOS_DelayTask(1);
            u32Delayms++;
        }

        // Patch HW outpu Vsync plus width too short and cause MLoad missing.
        // T3 U06 will fix it.
        if(MDrv_XC_MLoad_Check_Done(MAIN_WINDOW) == FALSE)
        {
            Hal_XC_MLoad_set_on_off(DISABLE, MAIN_WINDOW);
            MsOS_DelayTask(5);
            SC_W2BYTEMSK(REG_SC_BK00_02_L, BIT(6), BIT(6));
            MsOS_DelayTask(5);
            SC_W2BYTEMSK(REG_SC_BK00_02_L, 0, BIT(6));
            Hal_XC_MLoad_set_on_off(ENABLE, MAIN_WINDOW);

            MDrv_XC_WaitOutputVSync(2, 200, MAIN_WINDOW);
            if(MDrv_XC_MLoad_Check_Done(MAIN_WINDOW) == FALSE)
            {
                MS_U8 i;

                (printf("MLoad, TimeOut\n"));
                (printf("W:%d, R:%d, Fire:%d \n",stMLoadInfo[SC_MLOAD_SC0].u16WPoint, stMLoadInfo[SC_MLOAD_SC0].u16RPoint, stMLoadInfo[SC_MLOAD_SC0].u16FPoint));
                stMLoadInfo[SC_MLOAD_SC0].bEnable = FALSE;
                for(i=0;i<=stMLoadInfo[SC_MLOAD_SC0].u16WPoint; i++)
                {
                    MS_U32 *pu32Addr;
                    MS_PHYADDR DstAddr;
                    EN_SC_MLOAD_TYPE enMLoadType = SC_MLOAD_SC0;

                #if defined(MSOS_TYPE_LINUX_KERNEL)
                    DstAddr = stMLoadInfo[enMLoadType].u32VirAddr + i * MS_MLOAD_CMD_LEN;
                    pu32Addr = (MS_U32 *)(DstAddr);
                #else
                    DstAddr = stMLoadInfo[enMLoadType].PhyAddr + i * MS_MLOAD_CMD_LEN;
                    pu32Addr = (MS_U32 *)MS_PA2KSEG1(DstAddr);
                #endif
                    (printf("%02d, %08X \n", i, (int)*pu32Addr));

                }
                Hal_XC_MLoad_set_on_off(DISABLE, MAIN_WINDOW);
                stMLoadInfo[SC_MLOAD_SC0].u16RPoint = 0;
                stMLoadInfo[SC_MLOAD_SC0].u16WPoint = 0;
                stMLoadInfo[SC_MLOAD_SC0].u16FPoint = 0;
                return FALSE;
            }
        }
    }

    //Hal_XC_MLoad_set_sw_dynamic_idx_en(DISABLE); // for T8/T9/M10 patch
    return TRUE;
}

//-------------------------------------------------------------------------------------------------
/// Write command to the Menuload buffer by WORD
/// @param  u32Addr                 \b IN: the address (sub-bank + 8-bit address)
/// @param  u16Data                 \b IN: the data
/// @param  u16Mask                 \b IN: the mask
/// @return  TRUE if succeed, FALSE if failed
//-------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_MLoad_WriteCmd(MS_U32 u32Addr, MS_U16 u16Data, MS_U16 u16Mask)
{
    MS_U8 u8Bank;
    MS_MLoad_Data data;
    MS_BOOL bRet;

	#if defined(MSOS_TYPE_NOS)
	data.CMD.u8Addr = (u32Addr & 0xFF) >> 1;
    data.CMD.u8Bank = (u32Addr >> 8) & 0xFF;
	#else
    data.u8Addr = (MS_U8)((u32Addr & 0xFF) >> 1);
    data.u8Bank = (MS_U8)((u32Addr >> 8) & 0xFF);
	#endif

    if( u16Mask == 0xFFFF )
    {
    	#if defined(MSOS_TYPE_NOS)
		data.CMD.u16Data = u16Data;
		#else
        data.u16Data = u16Data;
		#endif
    }
    else
    {
        u8Bank = MDrv_ReadByte(BK_SELECT_00);

		#if defined(MSOS_TYPE_NOS)
		MDrv_WriteByte(BK_SELECT_00, data.CMD.u8Bank);
		data.CMD.u16Data = (SC_R2BYTE(u32Addr) & ~u16Mask) | (u16Data & u16Mask);
		#else
        MDrv_WriteByte(BK_SELECT_00, data.u8Bank);
        data.u16Data = (SC_R2BYTE(u32Addr) & ~u16Mask) | (u16Data & u16Mask);

		#endif

		MDrv_WriteByte(BK_SELECT_00, u8Bank);
    }

    if(MDrv_XC_MLoad_WriteCmd(data.u32Cmd, MAIN_WINDOW))
    {
        bRet = TRUE;
    }
    else
    {
        MLDBG(printf("Fail: %04lx %04x %04x \n", u32Addr, u16Data, u16Mask));
        bRet = FALSE;
    }

    return bRet;
}

MS_BOOL MApi_XC_MLoad_WriteCmd_And_Fire(MS_U32 u32Addr, MS_U16 u16Data, MS_U16 u16Mask)
{
    MS_BOOL bRet = TRUE;

    _MLOAD_ENTRY();

    if (!MApi_XC_MLoad_WriteCmd(u32Addr, u16Data, u16Mask))
    {
        bRet = FALSE;
        MLDBG(printf("Fail: Cmd %4lx %x %x \n", u32Addr, u16Data, u16Mask));
    }
    else
    {
       if (!MApi_XC_MLoad_Fire(TRUE))
       {
            bRet = FALSE;
            MLDBG(printf("Fire Error!!!!\n "));
       }
    }
    _MLOAD_RETURN();

    return bRet;
}

MS_BOOL MApi_XC_MLoad_WriteCmds_And_Fire(MS_U32 *pu32Addr, MS_U16 *pu16Data, MS_U16 *pu16Mask, MS_U16 u16CmdCnt)
{
    MS_BOOL bRet = TRUE;
    MS_U16 i;

    _MLOAD_ENTRY();

    for(i=0; i<u16CmdCnt; i++)
    {
        if (!MApi_XC_MLoad_WriteCmd(pu32Addr[i], pu16Data[i], pu16Mask[i]))
        {
            bRet = FALSE;
            MLDBG(printf("Fail: Cmd %4lx %x %x \n", pu32Addr[i], pu16Data[i], pu16Mask[i]));
        }
    }

    if (!MApi_XC_MLoad_Fire(TRUE))
    {
        bRet = FALSE;
        MLDBG(printf("Fire Error!!!!\n "));
    }
    _MLOAD_RETURN();

    return bRet;
}

static void MDrv_XC_MLG_Init(MS_PHYADDR phyAddr)
{

    MS_U8 u8MIUSel = 0;

    MLG(printf("%s\n",__FUNCTION__));

    if (phyAddr >= HAL_MIU1_BASE)
    {
        u8MIUSel = 1;
    }
    else
    {
        u8MIUSel = 0;
    }
    MLG(printf("u8MIUSel = %d\n", u8MIUSel));
    if (Hal_XC_MLG_GetCaps())
    {
        Hal_XC_MLG_set_trigger_timing(TRIG_SRC_FE_VSYNC); //output VSync falling edge
        Hal_XC_MLG_set_trigger_delay(0x01);
        Hal_XC_MLG_set_len(MS_MLG_REG_LEN);//length of DMA request

        Hal_XC_MLoad_set_miusel(u8MIUSel, MAIN_WINDOW);
    }

}



static void MDrv_XC_MLG_Trigger(MS_PHYADDR startAddr, MS_U16 u16CmdCnt)
{
    MLG(printf("%s 0x%lx, %u\n",__FUNCTION__, startAddr, u16CmdCnt));

    Hal_XC_MLG_set_base_addr(startAddr);
    Hal_XC_MLG_set_depth(u16CmdCnt);
    Hal_XC_MLG_set_on_off(ENABLE);
}

/////////////////////////////////////////////////////////////////////////////
// Menuload Gamma command (12 bit content)
//
// One MIU Word
// T8, T11, T12 support this function
// [T8, T12]             128 Bit = 16 Bytess
// ______________________________________________________________________________
//      Byte | 0         1                2           3           4          5                      6
// [ gamma table B content]      [ gamma table G content]  [ gamma table R content]  [B enable] [G enable]  [R enable]
//            11:0                      27:16                       43:32               48:48     49:49        50:50
//
static void MDrv_XC_MLG_WriteCmd(MS_U8 *pR, MS_U8 *pG, MS_U8 *pB, MS_U16 u16Count, MS_U16 *pMaxGammaValue)
{
    MS_SC_MLG_TBL *pMLG_Addr;
    MS_PHYADDR DstAddr;
    MS_U16 i,j;

    MLG(printf("%s\n",__FUNCTION__));
    if((Hal_XC_MLG_get_status() == 0) &&
       (stMLGInfo.u16WPoint == stMLGInfo.u16RPoint))
    {
        stMLGInfo.u16RPoint = 0;
        stMLGInfo.u16WPoint = 0;
    }
    pMaxGammaValue[0] = pMaxGammaValue [1] = pMaxGammaValue [2] = 0;

    DstAddr = stMLGInfo.PhyAddr + stMLGInfo.u16WPoint * MS_MLG_CMD_LEN;
    pMLG_Addr = (MS_SC_MLG_TBL *)MS_PA2KSEG1(DstAddr);

    if ((stMLGInfo.u16WPoint + u16Count) <= stMLGInfo.u16MaxCmdCnt)
    {

        for (i = 0, j = 0; i < u16Count; i+=2 , j+=3)
        {
            pMLG_Addr[i].u16R = (pR[j] & 0x0F) | (pR[j+1] << 4);
            pMLG_Addr[i].u16G = (pG[j] & 0x0F) | (pG[j+1] << 4);
            pMLG_Addr[i].u16B = (pB[j] & 0x0F) | (pB[j+1] << 4);
            pMLG_Addr[i].u16Enable = BIT(0) | BIT(1) | BIT(2);

            MLG(printf("MLG_Addr[%d].u16R = 0x%x\n",i, pMLG_Addr[i].u16R));
            MLG(printf("MLG_Addr[%d].u16G = 0x%x\n",i, pMLG_Addr[i].u16G));
            MLG(printf("MLG_Addr[%d].u16B = 0x%x\n",i, pMLG_Addr[i].u16B));
            if(pMaxGammaValue[0] < pMLG_Addr[i].u16R)
            {
                pMaxGammaValue[0] = pMLG_Addr[i].u16R;
            }
            if(pMaxGammaValue[1] < pMLG_Addr[i].u16G)
            {
                pMaxGammaValue[1] = pMLG_Addr[i].u16G;
            }
            if(pMaxGammaValue[2] < pMLG_Addr[i].u16B)
            {
                pMaxGammaValue[2] = pMLG_Addr[i].u16B;
            }

            pMLG_Addr[i+1].u16R = (pR[j] >>4) | (pR[j+2] << 4);
            pMLG_Addr[i+1].u16G = (pG[j] >>4) | (pG[j+2] << 4);
            pMLG_Addr[i+1].u16B = (pB[j] >>4) | (pB[j+2] << 4);
            pMLG_Addr[i+1].u16Enable = BIT(0) | BIT(1) | BIT(2);

            MLG(printf("MLG_Addr[%d].u16R = 0x%x\n",i+1, pMLG_Addr[i+1].u16R));
            MLG(printf("MLG_Addr[%d].u16G = 0x%x\n",i+1, pMLG_Addr[i+1].u16G));
            MLG(printf("MLG_Addr[%d].u16B = 0x%x\n",i+1, pMLG_Addr[i+1].u16B));
            if(pMaxGammaValue[0] < pMLG_Addr[i+1].u16R)
            {
                pMaxGammaValue[0] = pMLG_Addr[i+1].u16R;
            }
            if(pMaxGammaValue[1] < pMLG_Addr[i+1].u16G)
            {
                pMaxGammaValue[1] = pMLG_Addr[i+1].u16G;
            }
            if(pMaxGammaValue[2] < pMLG_Addr[i+1].u16B)
            {
                pMaxGammaValue[2] = pMLG_Addr[i+1].u16B;
            }
            MLG(printf("Red   MaxGammaValue = 0x%x\n",pMaxGammaValue[0]));
            MLG(printf("Green MaxGammaValue = 0x%x\n",pMaxGammaValue[1]));
            MLG(printf("Blue  MaxGammaValue =0x%x\n",pMaxGammaValue[2]));
        }

        stMLGInfo.u16WPoint +=u16Count;
    }
    else
    {
        MLDBG(printf("Size is not enough, Cmd Too Many !!!!!!!!!!!!!!!!!\n"));
    }
    MLG(printf("stMLGInfo.u16WPoint = %d\n",stMLGInfo.u16WPoint));
}

//-------------------------------------------------------------------------------------------------
/// Initialize the Menuload Gamma
/// @param  PhyAddr                 \b IN: the physical address for the menuload
/// @param  u32BufByteLen           \b IN: the buffer length of the menu load commands
/// @return  void
//-------------------------------------------------------------------------------------------------
void MApi_XC_MLG_Init(MS_PHYADDR PhyAddr, MS_U32 u32BufByteLen)
{
    MLG(printf("%s: 0x%lx, 0x%lx\n", __FUNCTION__, PhyAddr, u32BufByteLen));

    if(Hal_XC_MLG_GetCaps())
    {

        memset((void *)MS_PA2KSEG1(PhyAddr), 0, u32BufByteLen);

        stMLGInfo.bEnable = FALSE;
        stMLGInfo.u16RPoint = 0;
        stMLGInfo.u16WPoint = 0;
        stMLGInfo.u16MaxCmdCnt = (MS_U16)(u32BufByteLen / MS_MLG_CMD_LEN);
        stMLGInfo.PhyAddr = PhyAddr;

        MDrv_XC_MLG_Init(PhyAddr);
        MLG(printf("Cmd max number = %d\n",stMLGInfo.u16MaxCmdCnt));
    }
}

//-------------------------------------------------------------------------------------------------
/// Enable/Disable the Menuload Gamma
/// @return  void
//-------------------------------------------------------------------------------------------------
void MApi_XC_MLG_Enable(MS_BOOL bEnable)
{

    stMLGInfo.bEnable = bEnable;

}

//-------------------------------------------------------------------------------------------------
/// Get the caps of Menuload Gamma
/// @return  TRUE if succeed, FALSE if failed
//-------------------------------------------------------------------------------------------------
MS_BOOL MApi_XC_MLG_GetCaps(void)
{
    return Hal_XC_MLG_GetCaps();
}

//-------------------------------------------------------------------------------------------------
/// Get the status of Menuload Gamma
/// @return  MLG_TYPE
//-------------------------------------------------------------------------------------------------
MLG_TYPE MApi_XC_MLG_GetStatus(void)
{
    MLG_TYPE type = E_MLG_UNSUPPORTED;

    if (Hal_XC_MLG_GetCaps())
    {
        if (stMLGInfo.bEnable)
        {
            type = E_MLG_ENABLED;
        }
        else
        {
            type = E_MLG_DISABLED;
        }
    }

    return type;
}

//-------------------------------------------------------------------------------------------------
/// Fire the Menuload gamma commands
/// @return  TRUE if succeed, FALSE if failed
//-------------------------------------------------------------------------------------------------
void MApi_XC_MLG_Fire(MS_U8 *pR, MS_U8 *pG, MS_U8 *pB, MS_U16 u16Count, MS_U16 *pMaxGammaValue)
{
    if(Hal_XC_MLG_GetCaps() && (Hal_XC_MLG_get_status() == 0))
    {
        MS_PHYADDR CmdBufAddr;
        MDrv_XC_MLG_WriteCmd(pR, pG, pB, u16Count, pMaxGammaValue);
        MsOS_FlushMemory();
        CmdBufAddr = stMLGInfo.PhyAddr + stMLGInfo.u16RPoint * MS_MLG_CMD_LEN;
        MDrv_XC_MLG_Trigger(CmdBufAddr, u16Count);
        stMLGInfo.u16RPoint += u16Count;
    }
}



