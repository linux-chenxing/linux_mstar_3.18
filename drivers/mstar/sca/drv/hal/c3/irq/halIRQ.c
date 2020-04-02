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

#if defined (MSOS_TYPE_NOS) || defined (MSOS_TYPE_CE)

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "MsCommon.h"
#include "MsOS.h"
#include "halIRQTBL.h"
#include "regCHIP.h"
#include "drvIRQ.h"
#include "halIRQ.h"
#include "regIRQ.h"


#define MST_MACRO_START     do {
#define MST_MACRO_END       } while (0)

#if defined (__mips__)
#define mtspr(spr, value)  printf("[NIY] mtspr in line:%s %d\n",__FILE__, __LINE__);
#define mfspr(spr)		  printf("[NIY] mfspr in line:%s %d\n",__FILE__, __LINE__);

inline MS_U32 __mhal_lsbit_index(MS_U32 _value_)
{
    MS_U32  index = 1;

    while((_value_&0x01) == 0x00)
    {
        _value_ = (_value_ >> 1);
        index++;
        if(index == 32)
        {
            index = 0;
            break;
        }
    }

    return index;
    //printf(const char * fmt, ...)("[NIY] __mhal_lsbit_index in line: %s %d\n",__FILE__, __LINE__);
}


#define __mhal_interrupt_disable(_old_) (_old_=_old_)

#define __mhal_interrupt_restore(_old_) (_old_=_old_)

#elif defined (__arm__)
#define mtspr(spr, value)  printf("[NIY] mtspr in line:%s %d\n",__FILE__, __LINE__);
#define mfspr(spr)		  printf("[NIY] mfspr in line:%s %d\n",__FILE__, __LINE__);

#ifdef MSOS_TYPE_CE
__inline MS_U32 __mhal_lsbit_index(MS_U32 _value_)
#else
inline MS_U32 __mhal_lsbit_index(MS_U32 _value_)
#endif
{
    MS_U32  index = 1;

    while((_value_&0x01) == 0x00)
    {
        _value_ = (_value_ >> 1);
        index++;
        if(index == 32)
        {
            index = 0;
            break;
        }
    }

    return index;
    //printf(const char * fmt, ...)("[NIY] __mhal_lsbit_index in line: %s %d\n",__FILE__, __LINE__);
}

#define __mhal_interrupt_disable(_old_) (_old_=_old_)

#define __mhal_interrupt_restore(_old_) (_old_=_old_)
#else
#define mtspr(spr, value) \
    __asm__ __volatile__ ("l.mtspr\t\t%0,%1,0" : : "r" (spr), "r" (value))

#define mfspr(spr) \
    ({ \
        unsigned long value; \
        __asm__ __volatile__ ("l.mfspr\t\t%0,%1,0" : "=r" (value) : "r" (spr) : "memory"); \
        value; \
    })

#define __mhal_lsbit_index(_value_)                                         \
    ({                                                                      \
    unsigned long _index_;                                                  \
    __asm__ __volatile__ ("l.ff1\t\t%0,%1" : "=r" (_index_) : "r" (_value_));\
    _index_;                                                                \
    })


#define GRP_BITS                (11)
#define SPR_SR                  ((0 << GRP_BITS) + 17)
#define SPR_SR_TEE              0x00000002  // Tick timer Exception Enable
#define SPR_SR_IEE              0x00000004  // Interrupt Exception Enable

#define __mhal_interrupt_disable(_old_)                                     \
    MST_MACRO_START                                                         \
    _old_ = mfspr(SPR_SR);                                                  \
    mtspr(SPR_SR, (_old_) & ~(SPR_SR_IEE | SPR_SR_TEE));                    \
    MST_MACRO_END

#define __mhal_interrupt_restore(_old_)                                     \
    mtspr(SPR_SR, (~(SPR_SR_IEE|SPR_SR_TEE) & mfspr(SPR_SR) ) |             \
                  ( (SPR_SR_IEE|SPR_SR_TEE) & (_old_) ))
#endif


//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#define COUNTOF( array )    (sizeof(array) / sizeof((array)[0]))
//#define E_INTERRUPT_FIQ     E_INTERRUPT_02
//#define E_INTERRUPT_IRQ     E_INTERRUPT_03

//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------
typedef void (*IRQCb)(MS_U32 u32Vector);

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
static IRQCb irq_table[E_IRQ_FIQ_ALL] = {0};
// static MS_U32 _u32FIQ, _u32IRQ, _u32FIQExp, _u32IRQExp, _u32MIO_MapBase = 0;
static MS_U32 _u32FIQ_Msk, _u32IRQ_Msk, _u32FIQExp_Msk, _u32IRQExp_Msk;
#ifdef MSOS_TYPE_CE
#define _u32MIO_MapBase 0xbf200000
#else

#ifdef MCU_AEON
#define _u32MIO_MapBase 0xA0200000
#elif MCU_ARM_9
#define _u32MIO_MapBase 0xfd200000
#else
#define _u32MIO_MapBase 0xbf200000
#endif

#endif

static MS_BOOL _bInIRQ = FALSE, _bInFIQ = FALSE;

//-------------------------------------------------------------------------------------------------
//  External Functions
//-------------------------------------------------------------------------------------------------

#define REG16_R(u32RegAddr) ((*((volatile MS_U16*)(_u32MIO_MapBase+ ((u32RegAddr)<< 1)))) & 0xFFFF)
#define REG16_W(u32RegAddr, u32Value) (*((volatile MS_U32*)(_u32MIO_MapBase+ ((u32RegAddr)<< 1))))= ((u32Value) & 0xFFFF)

/*
static MS_U16 REG16_R(MS_U32 u32RegAddr_in)
{
    MS_U32 u32RegAddr1 = (u32RegAddr_in << 1);
    MS_U32 u32RegAddr = (_u32MIO_MapBase+ (u32RegAddr1));
    MS_U16 u16RegValue = (*((volatile MS_U16*)(u32RegAddr)) & 0xFFFF);

    printf("[%s][%d] 0x%08x, 0x%08x\n", __FUNCTION__, __LINE__, u32RegAddr, u16RegValue);
    return u16RegValue;
}

static MS_U16 REG16_W(MS_U32 u32RegAddr_in, MS_U32 u32Value)
{
    MS_U32 u32RegAddr1 = (u32RegAddr_in << 1);
    MS_U32 u32RegAddr = (_u32MIO_MapBase+ (u32RegAddr1));
    *((volatile MS_U16*)(u32RegAddr)) = ((u32Value) & 0xFFFF);

    // printf("[%s][%d] 0x%08x, 0x%08x\n", __FUNCTION__, __LINE__, u32RegAddr, u32Value);
    // REG16_R(u32RegAddr_in);
}
*/

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
static void _HAL_IRQ_Enable(MS_U32 u32Vector, int enable);

static MS_U16 _IRQ_Read2Byte(MS_U32 u32RegAddr)
{
    return REG16_R(u32RegAddr);
}

static MS_U32 _IRQ_Read4Byte(MS_U32 u32RegAddr)
{
    return (_IRQ_Read2Byte(u32RegAddr) | _IRQ_Read2Byte(u32RegAddr+2) << 16);
}

#if 0
static void _IRQ_WriteByte(MS_U32 u32RegAddr, MS_U8 u8Val)
{
    if (u32RegAddr & 1)
    {
        REG16_W(u32RegAddr, (REG16_R(u32RegAddr) & ~(0xFF00))| (u8Val<< 8));
    }
    else
    {
        REG16_W(u32RegAddr, (REG16_R(u32RegAddr) & ~(0x00FF))| (u8Val));
    }
}
#endif

static void _IRQ_Write2Byte(MS_U32 u32RegAddr, MS_U16 u16Val)
{
    REG16_W(u32RegAddr, u16Val);
}

static void _IRQ_Write4Byte(MS_U32 u32RegAddr, MS_U32 u32Val)
{
    _IRQ_Write2Byte(u32RegAddr, (MS_U16)(u32Val & 0x0000FFFF));
    _IRQ_Write2Byte(u32RegAddr+2, (MS_U16)(u32Val >> 16));
}

static void _HAL_IRQ_FIQHnd(void)
{
    MS_U32 status;
    MS_U32 index;

    _bInFIQ = TRUE;
    status = _IRQ_Read4Byte(REG_FIQ_FINAL_STATUS);

    index = __mhal_lsbit_index(status);
    if (index)
    {

        _IRQ_Write4Byte(REG_C_FIQ_CLR + 0, status);
        _IRQ_Write4Byte(REG_C_FIQ_CLR + 0, 0);

        do
        {
            status &= ~(1 << --index);
            index += (MS_U32)E_FIQL_START;
            if (irq_table[index])
            {
                _HAL_IRQ_Enable(index, DISABLE);
                irq_table[index](HWIdx2IntEnum[index]);
            }
            index = __mhal_lsbit_index(status);
        } while (index);
    }

    status = _IRQ_Read4Byte(REG_C_FIQ_EXP_FINAL_STATUS);

    index = __mhal_lsbit_index(status);
    if (index)
    {
        _IRQ_Write4Byte(REG_C_FIQ_EXP_CLR, status);
        _IRQ_Write4Byte(REG_C_FIQ_EXP_CLR, 0);
        do {
            status &= ~(1 << --index);
            index += (MS_U32)E_FIQEXPL_START;
            if (irq_table[index])
            {
                _HAL_IRQ_Enable(index, DISABLE);
                irq_table[index](HWIdx2IntEnum[index]);
            }
            index = __mhal_lsbit_index(status);
        } while (index);
    }
    _bInFIQ = FALSE;
}

static void _HAL_IRQ_IRQHnd(void)
{
    MS_U32 status;
    MS_U32 index;

    _bInIRQ = TRUE;
    status = _IRQ_Read4Byte(REG_IRQ_FINAL_STATUS);
    index = __mhal_lsbit_index(status);
    if (index)
    {
        do {
            status &= ~(1 << --index);
            index += (MS_U32)E_IRQL_START;
            if (irq_table[index])
            {
                _HAL_IRQ_Enable(index, DISABLE);
                //fix Uart Rx interrupt can't work
                irq_table[index](HWIdx2IntEnum[index]);
            }
            index = __mhal_lsbit_index(status);
        } while (index);
    }

    status = _IRQ_Read4Byte(REG_C_IRQ_EXP_FINAL_STATUS);
    index = __mhal_lsbit_index(status);
    if (index)
    {
        do {
            status &= ~(1 << --index);
            index += (MS_U32)E_IRQEXPL_START;
            if (irq_table[index])
            {
                _HAL_IRQ_Enable(index, DISABLE);
                irq_table[index](HWIdx2IntEnum[index]);
            }
            index = __mhal_lsbit_index(status);
        } while (index);
    }
    _bInIRQ = FALSE;
}

static void _HAL_IRQ_Enable(MS_U32 u32Vector, int enable)
{
    MS_U32 reg = E_IRQ_FIQ_INVALID;
    MS_U32 mask;
    MS_U32 old = 0;

    if ((MS_U32)u32Vector <= COUNTOF(irq_table))
    {
        if ( (u32Vector >= E_IRQL_START) && (u32Vector <= E_IRQH_END) )
        {
            u32Vector -= E_IRQL_START;
            reg = REG_C_IRQ_MASK;
        }
        else if ( (u32Vector >= E_FIQL_START) && (u32Vector <= E_FIQH_END) )
        {
            u32Vector -= E_FIQL_START;
            reg = REG_C_FIQ_MASK;
        }
        else if ( (u32Vector >= E_IRQEXPL_START) && (u32Vector <= E_IRQEXPH_END) )
        {
            u32Vector -= E_IRQEXPL_START;
            reg = REG_C_IRQ_EXP_MASK;
        }
        else if ( (u32Vector >= E_FIQEXPL_START) && (u32Vector <= E_FIQEXPH_END) )
        {
            u32Vector -= E_FIQEXPL_START;
            reg = REG_C_FIQ_EXP_MASK;
        }

        if( E_IRQ_FIQ_INVALID == reg )
        {
            //printf("_HAL_IRQ_Enable: unknow vector\n");
            return;
        }

        __mhal_interrupt_disable(old);
        mask = _IRQ_Read4Byte(reg);
        u32Vector = (1 << u32Vector);

        if (enable)
            mask &= ~u32Vector;
        else
            mask |= u32Vector;

        _IRQ_Write4Byte(reg, mask);
        __mhal_interrupt_restore(old);

    }
}

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
void HAL_IRQ_Set_IOMap(MS_U32 u32Base)
{
    // _u32MIO_MapBase = u32Base;
}

void HAL_IRQ_Init(void)
{
    MsOS_CPU_AttachInterrupt(E_INTERRUPT_FIQ, _HAL_IRQ_FIQHnd, E_INTERRUPT_FIQ);
    MsOS_CPU_AttachInterrupt(E_INTERRUPT_IRQ, _HAL_IRQ_IRQHnd, E_INTERRUPT_IRQ);
    HAL_IRQ_DetechAll();
}

void HAL_IRQ_Attach(MS_U32 u32Vector, void *pIntCb)
{
    MS_U32 u32VectorIndex = 0;

    u32VectorIndex = (MS_U32)IntEnum2HWIdx[u32Vector];

    if ((MS_U32)u32VectorIndex <= COUNTOF(irq_table))
        irq_table[u32VectorIndex] = (IRQCb)pIntCb;
    else
        printf("%s error vector: %x\n", __FUNCTION__, (unsigned int)u32VectorIndex);
}

void HAL_IRQ_DetechAll()
{
    MS_U16 u16Cnt= 0;
    for (; u16Cnt <= COUNTOF(irq_table); u16Cnt++)
        irq_table[u16Cnt] = 0;
}

void HAL_IRQ_Detech(MS_U32 u32Vector)
{
    MS_U32 u32VectorIndex = 0;

    u32VectorIndex = (MS_U32)IntEnum2HWIdx[u32Vector];

    if ((MS_U32)u32VectorIndex <= COUNTOF(irq_table))
        irq_table[u32VectorIndex] = 0;
    else
        printf("%s error vector: %x\n", __FUNCTION__, (unsigned int)u32Vector);
}

void HAL_IRQ_MaskAll(MS_BOOL bMask)
{
    if (bMask)
    {
        _u32FIQ_Msk = _IRQ_Read4Byte(REG_C_FIQ_MASK);
        _u32IRQ_Msk = _IRQ_Read4Byte(REG_C_IRQ_MASK);
        _u32FIQExp_Msk = _IRQ_Read4Byte(REG_C_FIQ_EXP_MASK);
        _u32IRQExp_Msk = _IRQ_Read4Byte(REG_C_IRQ_EXP_MASK);
        _IRQ_Write4Byte(REG_C_FIQ_MASK, 0xFFFFFFFF);
        _IRQ_Write4Byte(REG_C_IRQ_MASK, 0xFFFFFFFF);
        _IRQ_Write4Byte(REG_C_FIQ_EXP_MASK, 0xFFFFFFFF);
        _IRQ_Write4Byte(REG_C_IRQ_EXP_MASK, 0xFFFFFFFF);
    }
    else
    {
        _IRQ_Write4Byte(REG_C_FIQ_MASK, 0);
        _IRQ_Write4Byte(REG_C_IRQ_MASK, 0);
        _IRQ_Write4Byte(REG_C_FIQ_EXP_MASK, 0);
        _IRQ_Write4Byte(REG_C_IRQ_EXP_MASK, 0);
    }
}

void HAL_IRQ_Restore()
{
    _IRQ_Write4Byte(REG_C_FIQ_MASK, _u32FIQ_Msk);
    _IRQ_Write4Byte(REG_C_IRQ_MASK, _u32IRQ_Msk);
    _IRQ_Write4Byte(REG_C_FIQ_EXP_MASK, _u32FIQExp_Msk);
    _IRQ_Write4Byte(REG_C_IRQ_EXP_MASK, _u32IRQExp_Msk);
}

void HAL_IRQ_Mask(MS_U32 u32Vector)
{
    MS_U32 u32VectorIndex = 0;

    u32VectorIndex = (MS_U32)IntEnum2HWIdx[u32Vector];
    _HAL_IRQ_Enable(u32VectorIndex, DISABLE);
}

void HAL_IRQ_UnMask(MS_U32 u32Vector)
{
    MS_U32 u32VectorIndex = 0;

    u32VectorIndex = (MS_U32)IntEnum2HWIdx[u32Vector];
    _HAL_IRQ_Enable(u32VectorIndex, ENABLE);
}

void HAL_IRQ_NotifyCpu(IRQ_CPU_TYPE type)
{
    type = type;
    printf("[%s][%d] has not implemented yet\n", __FUNCTION__, __LINE__);
#if 0
    switch (type)
    {
        case E_IRQ_CPU0_2_CPU1:
            _IRQ_WriteByte(REG_SEND_IRQ_FROM_CPU0, BIT(0));
            _IRQ_WriteByte(REG_SEND_IRQ_FROM_CPU0, 0);
            break;
        case E_IRQ_CPU0_2_CPU2:
            _IRQ_WriteByte(REG_SEND_IRQ_FROM_CPU0, BIT(1));
            _IRQ_WriteByte(REG_SEND_IRQ_FROM_CPU0, 0);
            break;
        case E_IRQ_CPU1_2_CPU0:
            _IRQ_WriteByte(REG_SEND_IRQ_FROM_CPU1, BIT(0));
            _IRQ_WriteByte(REG_SEND_IRQ_FROM_CPU1, 0);
            break;
        case E_IRQ_CPU1_2_CPU2:
            _IRQ_WriteByte(REG_SEND_IRQ_FROM_CPU1, BIT(1));
            _IRQ_WriteByte(REG_SEND_IRQ_FROM_CPU1, 0);
            break;
        case E_IRQ_CPU2_2_CPU0:
            _IRQ_WriteByte(REG_SEND_IRQ_FROM_CPU2, BIT(0));
            _IRQ_WriteByte(REG_SEND_IRQ_FROM_CPU2, 0);
            break;
        case E_IRQ_CPU2_2_CPU1:
            _IRQ_WriteByte(REG_SEND_IRQ_FROM_CPU2, BIT(1));
            _IRQ_WriteByte(REG_SEND_IRQ_FROM_CPU2, 0);
            break;
        default:
            break;
    }
#endif
}

MS_BOOL HAL_IRQ_InISR()
{
    return (_bInIRQ || _bInFIQ);
}

#endif // #if defined (MSOS_TYPE_NOS)


#if defined (MSOS_TYPE_CE)
#include <windows.h>
#include <winbase.h>
#include <winioctl.h>
#include "MsCommon.h"
#include "MsOS.h"
#include "halIRQTBL.h"


typedef struct
{
    MS_BOOL bUsed;
    MS_BOOL bPending;
    MS_BOOL bEnable;

    MS_U32  u32SysIntr;
    HANDLE  hSysIntrEvent;
    InterruptCb pIntCb;
    HANDLE  hThread;
    MS_U32  u32ThreadID;
    MS_U32  u32Irq;
} CHIP_HISR_Info ;


//WinCE code
#define IOCTL_HAL_REQUEST_SYSINTR               CTL_CODE(FILE_DEVICE_HAL, 38, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_HAL_RELEASE_SYSINTR               CTL_CODE(FILE_DEVICE_HAL, 54, METHOD_BUFFERED, FILE_ANY_ACCESS)
extern BOOL KernelIoControl(DWORD dwIoControlCode, LPVOID lpInBuf, DWORD nInBufSize, LPVOID lpOutBuf, DWORD nOutBufSize, LPDWORD lpBytesReturned);
extern BOOL InterruptInitialize(DWORD idInt, HANDLE hEvent, LPVOID pvData, DWORD cbData);
extern void InterruptDisable( DWORD idInt);
//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
static CHIP_HISR_Info               _HISR_Info[MS_IRQ_MAX];
static MS_BOOL                      _bInHISR = FALSE ;
static MS_BOOL                      _bInLISR = FALSE ;
static MS_BOOL                      _bInitialized=FALSE;

static void interrupt_thread(void *arg)
{

	MS_U8  vectorIndex = 0xFF;
	MS_U32 tmp;
	HANDLE evt=0;

	vectorIndex=(MS_U8)(*((MS_U32 *)arg));
	tmp=vectorIndex;


	if (!KernelIoControl(IOCTL_HAL_REQUEST_SYSINTR, &tmp, sizeof(MS_U32), &(_HISR_Info[vectorIndex].u32SysIntr), sizeof(MS_U32), NULL))
	{
		RETAILMSG(1, (TEXT("[IST] !!ERROR!! IRQ=0x%02X request SYSINTR failed!!\n"),vectorIndex));
		goto BEACH;

	}

	if(_HISR_Info[vectorIndex].u32SysIntr==0 || MAXDWORD==_HISR_Info[vectorIndex].u32SysIntr)
	{
		RETAILMSG(1, (TEXT("[IST] !!ERROR!! Incorrect SYSINTR=0x%X!!\n"),_HISR_Info[vectorIndex].u32SysIntr));
		goto BEACH;

	}

	DEBUGMSG(1, (TEXT("[IST] IRQ=0x%02X requested SYSINTR=0x%X\n"),vectorIndex, _HISR_Info[vectorIndex].u32SysIntr));


	if((evt = CreateEvent(0, FALSE, FALSE, NULL))==NULL)
	{
		RETAILMSG(1, (TEXT("[IST] !!ERROR!! Create event failed!!!!\n")));
		goto BEACH;

	}

    if (!InterruptInitialize(_HISR_Info[vectorIndex].u32SysIntr, evt, NULL, 0))
	{
    	RETAILMSG(1, (TEXT("[IST] !!0x%X ERROR=0x%X!! InterruptInitialize failed!!!! 0x%X, 0x%X\n"),vectorIndex, GetLastError(),_HISR_Info[vectorIndex].u32SysIntr,evt));
		goto BEACH;
	}


	//complete the thread creation
    _HISR_Info[vectorIndex].hSysIntrEvent=evt;

    while (1)
	{
		// Wait for the Event to be Signaled
    	DWORD RetVal= WaitForSingleObject(_HISR_Info[vectorIndex].hSysIntrEvent, INFINITE );

		if( RetVal == WAIT_OBJECT_0 )
		{

			if(!_HISR_Info[vectorIndex].bUsed)
			{
				break;
			}

			(void)(_HISR_Info[vectorIndex].pIntCb)((InterruptNum)HWIdx2IntEnum[vectorIndex]);
			// Tell the Kernel that the Interrupt has been Serviced

		}
		else
		{
			RETAILMSG(1, (TEXT("[IST] !!ERROR!! IRQ=0x%02X IST wait error!!\n"),vectorIndex));
			break;
		}
	}

BEACH:


    if(!CloseHandle(_HISR_Info[vectorIndex].hSysIntrEvent))
    {
    	RETAILMSG(1, (TEXT("[IST] !!ERROR=0x%X!! IRQ=0x%02X failed to close sysEvent!!\n"),GetLastError(),vectorIndex));
    }

    if(!KernelIoControl(IOCTL_HAL_RELEASE_SYSINTR, &_HISR_Info[vectorIndex].u32SysIntr, sizeof(DWORD), NULL, 0, NULL))
    {
    	RETAILMSG(1, (TEXT("[IST] !!ERROR=0x%X!! IRQ=0x%02X failed to release sysIntr!!\n"),GetLastError(),vectorIndex));
    }

    _HISR_Info[vectorIndex].bUsed = FALSE ;

    RETAILMSG(1, (TEXT("[IST] IRQ=0x%02X IST exit DONE!!\n"),vectorIndex));
}

MS_BOOL CHIP_EnableIRQ(InterruptNum eIntNum)
{
    int enable = 1;
    MS_U32 u32Irq = 0;

	if(!_bInitialized)
	{
		RETAILMSG(1, (TEXT("[CHIP_AttachISR] !!ERROR!! Please initialize first!!\n")));
		return FALSE;
	}

    u32Irq = (MS_U32)IntEnum2HWIdx[eIntNum];
    _HISR_Info[u32Irq].bEnable=TRUE;

    _HAL_IRQ_Enable(u32Irq,TRUE);

    return TRUE ;
}


MS_BOOL CHIP_DisableIRQ(InterruptNum eIntNum)
{
	int enable = 1;
	MS_U32 u32Irq = 0;

	if(!_bInitialized)
	{
		RETAILMSG(1, (TEXT("[CHIP_AttachISR] !!ERROR!! Please initialize first!!\n")));
		return FALSE;
	}

	u32Irq = (MS_U32)IntEnum2HWIdx[eIntNum];
	_HISR_Info[u32Irq].bEnable=FALSE;

	_HAL_IRQ_Enable(u32Irq,FALSE);


	return TRUE ;
}

void CHIP_InitISR(void)
{

    MS_U16 i ;
    //printf("Init\n");

    if(_bInitialized)
    {
    	RETAILMSG(1, (TEXT("[CHIP_InitISR] !!WARNING!! Already initialized!!\n")));
    	return;
    }

    for( i = 0 ; i < MS_IRQ_MAX ; i++)
    {
        _HISR_Info[i].bUsed = FALSE ;
        _HISR_Info[i].bPending = FALSE ;
        _HISR_Info[i].bEnable = FALSE ;
        _HISR_Info[i].hThread = 0 ;
        _HISR_Info[i].u32SysIntr=0;
        _HISR_Info[i].hSysIntrEvent=0;
        _HISR_Info[i].pIntCb = NULL;
        _HISR_Info[i].u32Irq = 0xFFFFFFFF;
    }

    _bInitialized=TRUE;

    // printf("+pthread_mutex_init\n");
    // pthread_mutex_init(&_HISR_Info,NULL);
    //printf("-CHIP_InitISR\n");
}

MS_BOOL CHIP_AttachISR(InterruptNum eIntNum, InterruptCb pIntCb)
{

	MS_U8  vectorIndex = 0xFF;
	HANDLE thread;
	MS_U32 threadID;
	MS_U32 count=0;

	vectorIndex = IntEnum2HWIdx[eIntNum];

	if(!_bInitialized)
	{
		RETAILMSG(1, (TEXT("[CHIP_AttachISR] !!ERROR!! Please initialize first!!\n")));
		return FALSE;
	}

	if(_HISR_Info[vectorIndex].bUsed)
	{
		RETAILMSG(1, (TEXT("[CHIP_AttachISR] !!WARNING!! IRQ=0x%02X is already attached!!\n"),vectorIndex));
		return TRUE;
	}

    _HISR_Info[vectorIndex].u32Irq = vectorIndex ;
    _HISR_Info[vectorIndex].pIntCb = pIntCb;
    _HISR_Info[vectorIndex].bEnable = FALSE;
    _HISR_Info[vectorIndex].bUsed = TRUE ;
    _HISR_Info[vectorIndex].hSysIntrEvent = 0 ;


    if((thread=CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)interrupt_thread, &(_HISR_Info[vectorIndex].u32Irq), 0, &threadID))==NULL)
    {
    	RETAILMSG(1, (TEXT("[CHIP_AttachISR] !!ERROR!! Thread create failed!!!!\n")));
    	return FALSE;
    }

    _HISR_Info[vectorIndex].hThread = thread;
    _HISR_Info[vectorIndex].u32ThreadID = threadID;

    while(0== _HISR_Info[vectorIndex].hSysIntrEvent)
    {
    	Sleep(10);
    	if(count>0xFFFF)
    	{
    		RETAILMSG(1, (TEXT("[CHIP_AttachISR] !!ERROR!! Failed to start thread for IRQ=0x%02X with SysIntr=0x%X, ThreadID=0x%X, IntCb=0x%X\n"),vectorIndex, _HISR_Info[vectorIndex].u32SysIntr, _HISR_Info[vectorIndex].u32ThreadID, _HISR_Info[vectorIndex].pIntCb));
    		goto ATTACH_FAILED;
    	}
    }

    RETAILMSG(1, (TEXT("[CHIP_AttachISR] IRQ=0x%02X successfully attached with SysIntr=0x%X, ThreadID=0x%X, IntCb=0x%X\n"),vectorIndex, _HISR_Info[vectorIndex].u32SysIntr, _HISR_Info[vectorIndex].u32ThreadID, _HISR_Info[vectorIndex].pIntCb));

    if(0xFFFFFFFF== ResumeThread(_HISR_Info[vectorIndex].hThread))
    {

    	RETAILMSG(1, (TEXT("[CHIP_AttachISR] !!ERROR!! Thread resume failed for IRQ=0x%02X!!!! Terminate it !!!! \n")));
ATTACH_FAILED:
    	CHIP_DisableIRQ(eIntNum);
    	TerminateThread(_HISR_Info[vectorIndex].hThread,-1);
    	CloseHandle(_HISR_Info[vectorIndex].hThread);
    	CloseHandle(_HISR_Info[vectorIndex].hSysIntrEvent);

    	KernelIoControl(IOCTL_HAL_RELEASE_SYSINTR, &_HISR_Info[vectorIndex].u32SysIntr, sizeof(DWORD), NULL, 0, NULL);

    	_HISR_Info[vectorIndex].hThread = 0 ;
    	_HISR_Info[vectorIndex].bUsed = FALSE ;

    	return FALSE;
    }

    return TRUE;
}

MS_BOOL CHIP_DetachISR(InterruptNum eIntNum)
{
    MS_U8 vectorIndex = 0xFF;

    vectorIndex = (MS_U8)IntEnum2HWIdx[eIntNum];

    InterruptDisable(_HISR_Info[vectorIndex].u32SysIntr);

    if(TRUE == _HISR_Info[vectorIndex].bEnable)
    {
        CHIP_DisableIRQ(eIntNum);
    }

    _HISR_Info[vectorIndex].bUsed = FALSE ;

    if( _HISR_Info[vectorIndex].hThread )
    {

    	DWORD ret=0;
    	SetEvent(_HISR_Info[vectorIndex].hSysIntrEvent);
    	ret=WaitForSingleObject(_HISR_Info[vectorIndex].hThread,5000);

    	if(WAIT_FAILED==ret || WAIT_TIMEOUT ==ret)
    	{
    		RETAILMSG(1, (TEXT("[CHIP_DetachISR] !!ERROR!! IRQ=0x%02X thread destroy failed!! ThreadID=0x%X\n"),vectorIndex, _HISR_Info[vectorIndex].u32ThreadID));
    		TerminateThread(_HISR_Info[vectorIndex].hThread,-1);

    		CloseHandle(_HISR_Info[vectorIndex].hSysIntrEvent);
    		KernelIoControl(IOCTL_HAL_RELEASE_SYSINTR, &_HISR_Info[vectorIndex].u32SysIntr, sizeof(DWORD), NULL, 0, NULL);
    	}
    	else
    	{
    		RETAILMSG(1, (TEXT("[CHIP_DetachISR] IRQ=0x%02X thread is successfully destroyed!!\n"),vectorIndex));
    	}

      	CloseHandle(_HISR_Info[vectorIndex].hThread);
    	_HISR_Info[vectorIndex].hThread = 0 ;
    	_HISR_Info[vectorIndex].hSysIntrEvent = 0 ;
    	_HISR_Info[vectorIndex].pIntCb = 0 ;
    	_HISR_Info[vectorIndex].u32ThreadID = 0 ;
    	_HISR_Info[vectorIndex].u32Irq = 0xFFFFFFFF;
    }

    return TRUE;
}


MS_BOOL CHIP_InISRContext(void)
{
	return FALSE;
}

MS_BOOL CHIP_EnableAllInterrupt(void)
{
	return FALSE;
}

MS_BOOL CHIP_DisableAllInterrupt(void)
{
	return FALSE;
}

#endif
