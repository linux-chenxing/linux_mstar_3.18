////////////////////////////////////////////////////////////////////////////////

//

// Copyright (c) 2008-2009 MStar Semiconductor, Inc.

// All rights reserved.

//

// Unless otherwise stipulated in writing, any and all information contained

// herein regardless in any format shall remain the sole proprietary of

// MStar Semiconductor Inc. and be kept in strict confidence

// (!¡±MStar Confidential Information!¡L) by the recipient.

// Any unauthorized act including without limitation unauthorized disclosure,

// copying, use, reproduction, sale, distribution, modification, disassembling,

// reverse engineering and compiling of the contents of MStar Confidential

// Information is unlawful and strictly prohibited. MStar hereby reserves the

// rights to any and all damages, losses, costs and expenses resulting therefrom.

//

////////////////////////////////////////////////////////////////////////////////

//==============================================================================



#ifndef DRV_SC_MENULOAD_H

#define DRV_SC_MENULOAD_H



#define MLOAD_MUTEX

#define MLOAD_MUTEX_DBG	0




#ifdef MSOS_TYPE_CE

#ifdef ASIC_VERIFY

    #ifdef DRV_SC_MENULOAD_C

    #define INTERFACE

    INTERFACE MS_S32            _MLOAD_MUTEX = -1;

    #else

    #define INTERFACE extern

    INTERFACE MS_S32            _MLOAD_MUTEX;

    #endif

#else

	#ifdef DRV_SC_MENULOAD_C

	#pragma data_seg(".XCTex")
	MS_S32 _MLOAD_MUTEX = -1;
	#pragma data_seg()
	#pragma comment(linker,"/SECTION:.XCTex,RWS")

	#else

	#if (1==WITHIN_MDRV_XC_DLL)
	extern  MS_S32 _MLOAD_MUTEX;
	#else
	__declspec(dllimport) MS_S32 _MLOAD_MUTEX;
	#endif

	#endif
#endif

#else


    #ifdef DRV_SC_MENULOAD_C

    #define INTERFACE

    INTERFACE MS_S32            _MLOAD_MUTEX = -1;

    #else

    #define INTERFACE extern

    INTERFACE MS_S32            _MLOAD_MUTEX;

    #endif

#endif // MSOS_TYPE_CE

#define _MLOAD_MUTEX_TIME_OUT                 MSOS_WAIT_FOREVER



#ifdef MLOAD_MUTEX

#if(MLOAD_MUTEX_DBG)

#define _MLOAD_ENTRY()                                                                  \
            printf("1,==========================\n");                              \
            printf("[%s][%s][%06d]\n",__FILE__,__FUNCTION__,__LINE__);    \
            if(!MsOS_ObtainMutex(_MLOAD_MUTEX, _MLOAD_MUTEX_TIME_OUT))                     \
            {                                                                        \
                printf("==========================\n");                              \
                printf("[%s][%s][%06d] Mutex taking timeout\n",__FILE__,__FUNCTION__,__LINE__);    \
            }

#define _MLOAD_RETURN()                                                                \
            printf("0,==========================\n");                              \
            printf("[%s][%s][%06d] \n",__FILE__,__FUNCTION__,__LINE__);    \
            MsOS_ReleaseMutex(_MLOAD_MUTEX);

                                //return _ret;

#else

#define _MLOAD_ENTRY()                                                                  \
            if(!MsOS_ObtainMutex(_MLOAD_MUTEX, _MLOAD_MUTEX_TIME_OUT))                     \
            {                                                                        \
            }

#define _MLOAD_RETURN()    MsOS_ReleaseMutex(_MLOAD_MUTEX);

                                //return _ret;

#endif

#else // #if not def MLOAD_MUTEX

#define _MLOAD_ENTRY()         while(0)

#define _MLOAD_RETURN()    while(0)

#endif // #ifdef _MLOAD_MUTEX



MS_BOOL MDrv_XC_MLoad_Check_Done(SCALER_WIN eWindow);

void  MDrv_XC_MLoad_Init(MS_PHYADDR phyAddr, SCALER_WIN eWindow);

void MDrv_XC_MLoad_Trigger(MS_PHYADDR startAddr, MS_U16 u16CmdCnt, SCALER_WIN eWindow);

void MDrv_XC_MLoad_AddCmd(MS_U32 u32Cmd, SCALER_WIN eWindow);

MS_U32 MDrv_XC_MLoad_GetCmd(MS_PHYADDR DstAddr);

MS_BOOL MDrv_XC_MLoad_BufferEmpty(SCALER_WIN eWindow);

MS_BOOL MDrv_XC_MLoad_KickOff(SCALER_WIN eWindow);

MS_BOOL MDrv_XC_MLoad_GetCaps(SCALER_WIN eWindow);

void MDrv_XC_MLoad_AddNull(SCALER_WIN eWindow);


#if defined(MSOS_TYPE_LINUX_KERNEL)
void MApi_XC_MLoad_Init(MS_PHYADDR PhyAddr, MS_U32 u32VirAddr, MS_U32 u32BufByteLen);
#else
void MApi_XC_MLoad_Init(MS_PHYADDR PhyAddr, MS_U32 u32BufByteLen);
#endif

void MApi_XC_MLoad_Enable(MS_BOOL bEnable);

MLOAD_TYPE MApi_XC_MLoad_GetStatus(SCALER_WIN eWindow);

MS_BOOL MApi_XC_MLoad_Fire(MS_BOOL bImmeidate);

MS_BOOL MApi_XC_MLoad_WriteCmd(MS_U32 u32Addr, MS_U16 u16Data, MS_U16 u16Mask);



void MApi_XC_MLG_Init(MS_PHYADDR PhyAddr, MS_U32 u32BufByteLen);

void MApi_XC_MLG_Enable(MS_BOOL bEnable);

MS_BOOL MApi_XC_MLG_GetCaps(void);

MLG_TYPE MApi_XC_MLG_GetStatus(void);

void MApi_XC_MLG_Fire(MS_U8 *pR, MS_U8 *pG, MS_U8 *pB, MS_U16 u16Count, MS_U16 *pMaxGammaValue);



#endif



