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

///////////////////////////////////////////////////////////////////////////////////////////////////
///
/// file    drvMMIO.c
/// @brief  MMIO SYS Driver Interface
/// @author MStar Semiconductor Inc.
///////////////////////////////////////////////////////////////////////////////////////////////////


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------

#if defined ( MSOS_TYPE_LINUX) || defined(MSOS_TYPE_LINUX_KERNEL)

//#include <unistd.h>
//#include <fcntl.h>
//#include <sys/ioctl.h>
//#include <sys/mman.h>
#include <linux/kernel.h>
#include "MsCommon.h"
#include "drvMMIO.h"
#include "halMMIO.h"
#include "regCHIP.h"

/* Use 'I' as magic number */
#define MIOMAP_IOC_MAGIC                    'I'
#define MMIO_IOC_SET_MAP                    _IOW (MIOMAP_IOC_MAGIC, 0x01, MIOMap_Info_t)

/* MIOMAP_IOC_INFO */
typedef struct
{
    unsigned int                            u32Addr;
    unsigned int                            u32Size;
} MIOMap_Info_t;

#else // #ifdef MSOS_TYPE_LINUX

#if defined(MSOS_TYPE_CE)

#include <windows.h>
#include <ceddk.h>

#include "MsCommon.h"
#include "drvMMIO.h"
#include "halMMIO.h"
#include "regCHIP.h"

#else

#include "MsCommon.h"
#include "drvMMIO.h"
#include "halMMIO.h"
#include "regCHIP.h"

#endif

#endif


#include "MsVersion.h"
//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------
#ifdef MS_DEBUG
#define MMIO_DEBUG
#endif


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#ifdef MMIO_DEBUG
    #define MMIO_ERROR(fmt, args...)        MS_DEBUG_MSG(printf("[MMIO USER ERR][%06d]     " fmt, __LINE__, ##args))
    #define MMIO_WARN(fmt, args...)         MS_DEBUG_MSG(printf("[MMIO USER WARN][%06d]    " fmt, __LINE__, ##args))
    #define MMIO_PRINT(fmt, args...)        MS_DEBUG_MSG(printf("[MMIO USER][%06d]     " fmt, __LINE__, ##args))
    #define MMIO_ASSERT(_bool, _f)          if (!(_bool)) { MS_DEBUG_MSG((_f); MS_ASSERT(0);) }
#else
#ifdef MSOS_TYPE_CE
	#define MMIO_ERROR(fmt, ...)        MS_DEBUG_MSG(while (0))
    #define MMIO_WARN(fmt, ...)         MS_DEBUG_MSG(while (0))
    #define MMIO_PRINT(fmt, ...)        MS_DEBUG_MSG(while (0))
    #define MMIO_ASSERT(_bool, _f)          if (!(_bool)) { MS_DEBUG_MSG((_f)); }
#else
    #define MMIO_ERROR(fmt, args...)        MS_DEBUG_MSG(while (0))
    #define MMIO_WARN(fmt, args...)         MS_DEBUG_MSG(while (0))
    #define MMIO_PRINT(fmt, args...)        MS_DEBUG_MSG(while (0))
    #define MMIO_ASSERT(_bool, _f)          if (!(_bool)) { MS_DEBUG_MSG((_f)); }
#endif
#endif


//-------------------------------------------------------------------------------------------------
//  Local Structurs
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------
MS_U32                              _u32PM_Bank                 = 0x0 ;
MS_U32                              _u32PM_Bank_SIZE            = 0x0 ;
MS_U32                              _u32NonPM_Bank              = 0x0 ;
MS_U32                              _u32NonPM_Bank_SIZE         = 0x0 ;
MS_U32                              _u32Flash_Bank0             = 0x0 ;
MS_U32                              _u32Flash_Bank0_SIZE        = 0x0 ;
MS_U32                              _u32OTP_Bank                = 0x0 ;
MS_U32                              _u32OTP_BankSize            = 0x0 ;
#if defined(CHIP_A5) || defined(CHIP_A5P)
MS_U32                              _u32FRC_Bank                = 0x0 ;
MS_U32                              _u32FRC_Bank_SIZE           = 0x0 ;
#endif

#if defined(CHIP_C3)
MS_U32                              _u32IMI_Bank                = 0x0 ;
MS_U32                              _u32IMI_Bank_SIZE           = 0x0 ;
#endif


MS_U32                              _u32NSK_Bank                = 0x0 ; // U3 Only
MS_U32                              _u32NSK_BankSize            = 0x0 ;

MS_U32                              _u32SPRAM_Bank              = 0x0 ; // Scratchpad memory K2 Only
MS_U32                              _u32SPRAM_BankSize          = 0x0 ;

static MSIF_Version _drv_mmio_version = {

#if ( defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_NOS) || defined(MSOS_TYPE_LINUX_KERNEL) || defined(MSOS_TYPE_LINUX))
	{ MMIO_DRV_VERSION, },
#else
    .DDI = { MMIO_DRV_VERSION, },
#endif
};
////////////////////////////////////////////////////////////////////////////////
/// @brief \b Function  \b Name: MDrv_MSOS_GetLibVer
/// @brief \b Function  \b Description: Show the MSOS driver version
/// @param ppVersion    \b Out: Library version string
/// @return             \b Result
////////////////////////////////////////////////////////////////////////////////
MS_BOOL MDrv_MMIO_GetLibVer(const MSIF_Version **ppVersion)
{
    if (!ppVersion)
        return FALSE;

    *ppVersion = &_drv_mmio_version;
    return TRUE;
}

#ifdef MSOS_TYPE_LINUX

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
static MS_S32                       _s32MIOMapFd                = -1;


//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

MS_BOOL MDrv_MMIO_Init(void)
{
    #if defined(CHIP_JANUS) || defined(CHIP_MARAI10)

        return true;

    #else
    MIOMap_Info_t       stMIOMapInfo;

    MS_U32              PM_Bank ;
    MS_U32              PM_Bank_SIZE ;
    MS_U32              NonPM_Bank ;
    MS_U32              NonPM_Bank_SIZE ;
    MS_U32              Flash_Bank0 ;
    MS_U32              Flash_Bank0_SIZE ;
    #if defined(CHIP_A5) || defined(CHIP_A5P)
    MS_U32              FRC_Bank ;
    MS_U32              FRC_Bank_SIZE ;
    #endif
    MS_U32              bank_addr = 0;
    MS_U32              bank_size = 0;

    if (0 <= _s32MIOMapFd)
    {
        MMIO_WARN("%s is initiated more than once\n", __FUNCTION__);
        return FALSE;
    }

    if (0 > (_s32MIOMapFd = open("/dev/miomap", O_RDWR)))
    {
        MMIO_ERROR("Open /dev/miomap fail\n");
        MS_ASSERT(0);
        return FALSE;
    }


    if (FALSE == HAL_MMIO_GetBase(&PM_Bank, &PM_Bank_SIZE, DRV_MMIO_PM_BANK))
    {
        MS_ASSERT(0);
    }
    if (FALSE == HAL_MMIO_GetBase(&NonPM_Bank, &NonPM_Bank_SIZE, DRV_MMIO_NONPM_BANK))
    {
        MS_ASSERT(0);
    }
    if (FALSE == HAL_MMIO_GetBase(&Flash_Bank0, &Flash_Bank0_SIZE, DRV_MMIO_FLASH_BANK0))
    {
        MS_ASSERT(0);
    }

    #if defined(CHIP_A5) || defined(CHIP_A5P)
    if (FALSE == HAL_MMIO_GetBase(&FRC_Bank, &FRC_Bank_SIZE, DRV_MMIO_FRC_BANK))
    {
        MS_ASSERT(0);
    }
    #endif

    if (PM_Bank_SIZE)
    {
        stMIOMapInfo.u32Addr = PM_Bank ;
        stMIOMapInfo.u32Size = PM_Bank_SIZE ;
        if (ioctl(_s32MIOMapFd, MMIO_IOC_SET_MAP , &stMIOMapInfo))
        {
            return FALSE;
        }
        if ((MS_U32)MAP_FAILED == (_u32PM_Bank= (MS_U32)mmap(0, stMIOMapInfo.u32Size, PROT_READ| PROT_WRITE, MAP_SHARED, _s32MIOMapFd, 0)))
        {
            MS_ASSERT(0);
            return FALSE;
        }
        _u32PM_Bank_SIZE = PM_Bank_SIZE ;
        MS_DEBUG_MSG(printf("PM Bank mapping success. Base=[%x], Size=[%x]\n", (unsigned int)_u32PM_Bank, (unsigned int)_u32PM_Bank_SIZE));
    }

    if (NonPM_Bank_SIZE)
    {
        stMIOMapInfo.u32Addr = NonPM_Bank ;
        stMIOMapInfo.u32Size = NonPM_Bank_SIZE ;
        if (ioctl(_s32MIOMapFd, MMIO_IOC_SET_MAP , &stMIOMapInfo))
        {
            return FALSE;
        }
        if ((MS_U32)MAP_FAILED == (_u32NonPM_Bank= (MS_U32)mmap(0, stMIOMapInfo.u32Size, PROT_READ| PROT_WRITE, MAP_SHARED, _s32MIOMapFd, 0)))
        {
            MS_ASSERT(0);
            return FALSE;
        }
        _u32NonPM_Bank_SIZE  = NonPM_Bank_SIZE ;
        MS_DEBUG_MSG(printf("NonPM Bank mapping success. Base=[%x], Size=[%x]\n",(unsigned int) _u32NonPM_Bank, (unsigned int)_u32NonPM_Bank_SIZE));
    }

    if (Flash_Bank0_SIZE)
    {
        stMIOMapInfo.u32Addr = Flash_Bank0 ;
        stMIOMapInfo.u32Size = Flash_Bank0_SIZE ;
        if (ioctl(_s32MIOMapFd, MMIO_IOC_SET_MAP , &stMIOMapInfo))
        {
            return FALSE;
        }
        if ((MS_U32)MAP_FAILED == (_u32Flash_Bank0= (MS_U32)mmap(0, stMIOMapInfo.u32Size, PROT_READ| PROT_WRITE, MAP_SHARED, _s32MIOMapFd, 0)))
        {
            MS_ASSERT(0);
            return FALSE;
        }
        _u32Flash_Bank0_SIZE = Flash_Bank0_SIZE ;
        MS_DEBUG_MSG(printf("Flash Bank0 mapping success. Base=[%x], Size=[%x]\n", (unsigned int)_u32Flash_Bank0, (unsigned int)_u32Flash_Bank0_SIZE));
    }

    #if defined(CHIP_A5) || defined(CHIP_A5P)
    if (FRC_Bank_SIZE)
    {
        stMIOMapInfo.u32Addr = FRC_Bank ;
        stMIOMapInfo.u32Size = FRC_Bank_SIZE ;
        if (ioctl(_s32MIOMapFd, MMIO_IOC_SET_MAP , &stMIOMapInfo))
        {
            return FALSE;
        }
        if ((MS_U32)MAP_FAILED == (_u32FRC_Bank= (MS_U32)mmap(0, stMIOMapInfo.u32Size, PROT_READ| PROT_WRITE, MAP_SHARED, _s32MIOMapFd, 0)))
        {
            MS_ASSERT(0);
            return FALSE;
        }
        _u32FRC_Bank_SIZE  = FRC_Bank_SIZE ;
        MS_DEBUG_MSG(printf("FRC Bank mapping success. Base=[%x], Size=[%x]\n",(unsigned int) _u32FRC_Bank, (unsigned int)_u32FRC_Bank_SIZE));
    }
    #endif


/*
    Not every chip support OTP / NSK, the return value of MMIO_GetBase could be FALSE
*/

    if (HAL_MMIO_GetBase(&bank_addr, &bank_size, DRV_MMIO_OTP_BANK))
    {
        stMIOMapInfo.u32Addr = bank_addr;
        stMIOMapInfo.u32Size = bank_size;
        if (ioctl(_s32MIOMapFd, MMIO_IOC_SET_MAP, &stMIOMapInfo))
        {
            return FALSE;
        }
        _u32OTP_Bank = (MS_U32)mmap(0, stMIOMapInfo.u32Size, PROT_READ | PROT_WRITE, MAP_SHARED, _s32MIOMapFd, 0);
        if ((MS_U32)MAP_FAILED == _u32OTP_Bank)
        {
            _u32OTP_Bank = 0x0;
            MS_ASSERT(0);
            return FALSE;
        }
        _u32OTP_BankSize = bank_size;
        MS_DEBUG_MSG(printf("OTP Bank mapping success. Base=[%x], Size=[%x]\n", (unsigned int)_u32OTP_Bank, (unsigned int)_u32OTP_BankSize));
    }

    #if defined(CHIP_K2)
    // K2 Only Scratchpad memory/SPRAM
    if (HAL_MMIO_GetBase(&bank_addr, &bank_size, DRV_MMIO_SPRAM_BANK))
    {
        stMIOMapInfo.u32Addr = bank_addr;
        stMIOMapInfo.u32Size = bank_size;
        if (ioctl(_s32MIOMapFd, MMIO_IOC_SET_MAP, &stMIOMapInfo))
        {
            return FALSE;
        }
        _u32SPRAM_Bank = (MS_U32)mmap(0, stMIOMapInfo.u32Size, PROT_READ | PROT_WRITE, MAP_SHARED, _s32MIOMapFd, 0);
        if ((MS_U32)MAP_FAILED == _u32SPRAM_Bank)
        {
            _u32SPRAM_Bank = 0x0;
            MS_ASSERT(0);
            return FALSE;
        }
        _u32SPRAM_BankSize = bank_size;
        MS_DEBUG_MSG(printf("SPRAM Bank mapping success. Base=[%x], Size=[%x]\n", (unsigned int)_u32SPRAM_Bank, (unsigned int)_u32SPRAM_BankSize));
    }
    #endif

    // U3 Only
    if (HAL_MMIO_GetBase(&bank_addr, &bank_size, DRV_MMIO_NSK_BANK))
    {
        stMIOMapInfo.u32Addr = bank_addr;
        stMIOMapInfo.u32Size = bank_size;
        if (ioctl(_s32MIOMapFd, MMIO_IOC_SET_MAP, &stMIOMapInfo))
        {
            return FALSE;
        }
        _u32NSK_Bank = (MS_U32)mmap(0, stMIOMapInfo.u32Size, PROT_READ | PROT_WRITE, MAP_SHARED, _s32MIOMapFd, 0);
        if ((MS_U32)MAP_FAILED == _u32NSK_Bank)
        {
            _u32NSK_Bank = 0x0;
            MS_ASSERT(0);
            return FALSE;
        }
        _u32NSK_BankSize = bank_size;
        MS_DEBUG_MSG(printf("NSK Bank mapping success. Base=[%x], Size=[%x]\n", (unsigned int)_u32NSK_Bank, (unsigned int)_u32NSK_BankSize));
    }

    return TRUE;

    #endif
}


MS_BOOL MDrv_MMIO_Close(void)
{
    if (0 > _s32MIOMapFd)
    {
        MMIO_WARN("%s is closed before initiated\n", __FUNCTION__);
        return FALSE;
    }
    close(_s32MIOMapFd);
    _s32MIOMapFd = -1;
    return TRUE;
}


MS_BOOL MDrv_MMIO_GetBASE(MS_U32 *u32Baseaddr, MS_U32 *u32Basesize, MS_U32 u32Module)
{

    MS_U16              u16Type;

    u16Type = HAL_MMIO_GetType(u32Module);

    #if defined(CHIP_JANUS) || defined(CHIP_MARAI10)

    if (u16Type == DRV_MMIO_PM_BANK)
    {
        *u32Baseaddr = 0xA0000000 ;
        *u32Basesize = 0x00007B80 ;
        return TRUE ;
    }
    if (u16Type == DRV_MMIO_NONPM_BANK)
    {
        *u32Baseaddr = 0xA0200000 ;
        *u32Basesize = 0x00025600 ;
        return TRUE ;
    }
    if (u16Type == DRV_MMIO_FLASH_BANK0 )
    {
        *u32Baseaddr = 0xA1000000 ;
        *u32Basesize = 0x4000000 ;
        return TRUE ;
    }

    //MS_DEBUG_MSG(printf(">>> MDrv_MMIO_GetBASE=[%x] (Bank=%x)\n", (unsigned int)*u32Baseaddr,(unsigned int)DRV_MMIO_PM_BANK));


    #else

    if (u16Type == DRV_MMIO_PM_BANK)
    {
        *u32Baseaddr = _u32PM_Bank;
        *u32Basesize = _u32PM_Bank_SIZE;
        return TRUE ;
    }
    if (u16Type == DRV_MMIO_NONPM_BANK)
    {
        *u32Baseaddr = _u32NonPM_Bank;
        *u32Basesize = _u32NonPM_Bank_SIZE;
        return TRUE ;
    }
    if (u16Type == DRV_MMIO_FLASH_BANK0 )
    {
        *u32Baseaddr = _u32Flash_Bank0;
        *u32Basesize = _u32Flash_Bank0_SIZE;
        return TRUE ;
    }
    if ((u16Type == DRV_MMIO_OTP_BANK) && (_u32OTP_BankSize)) // not support if BankSize = 0
    {
        *u32Baseaddr = _u32OTP_Bank;
        *u32Basesize = _u32OTP_BankSize;
        return TRUE ;
    }
    if (((u16Type == DRV_MMIO_SPRAM_BANK) && (_u32SPRAM_BankSize))) // not support if BankSize = 0
    {
        *u32Baseaddr = _u32SPRAM_Bank;
        *u32Basesize = _u32SPRAM_BankSize;
        return TRUE ;
    }
    #if defined(CHIP_A5) || defined(CHIP_A5P)
    if (u16Type == DRV_MMIO_FRC_BANK)
    {
        *u32Baseaddr = _u32FRC_Bank;
        *u32Basesize = _u32FRC_Bank_SIZE;
        return TRUE ;
    }
    #endif
    #if defined(CHIP_C3)
    if (u16Type == DRV_MMIO_IMI_BANK)
    {
        *u32Baseaddr = _u32IMI_Bank;
        *u32Basesize = _u32IMI_Bank_SIZE;
        return TRUE ;
    }
    #endif

    // Particular HW base address
    if (HAL_MMIO_GetIPBase(u32Baseaddr, u16Type))
    {
        *u32Basesize = 0;
        return TRUE;
    }

    #endif

    return FALSE ;

}


#else // #ifdef MSOS_TYPE_LINUX


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
#if defined( MSOS_TYPE_CE)
static BOOL initialized=FALSE;

MS_BOOL MDrv_MMIO_Init(void)
{

	if(!initialized)
	{
		MS_U32              PM_Bank ;
		MS_U32              PM_Bank_SIZE ;
		MS_U32              NonPM_Bank ;
		MS_U32              NonPM_Bank_SIZE ;
		MS_U32              Flash_Bank0 ;
		MS_U32              Flash_Bank0_SIZE ;
	    #if defined(CHIP_C3)
    	MS_U32              IMI_Bank ;
	    MS_U32              IMI_Bank_SIZE ;
	    #endif
		PHYSICAL_ADDRESS    ioPhysicalBase = {0};

		if (FALSE == HAL_MMIO_GetBase(&PM_Bank, &PM_Bank_SIZE, DRV_MMIO_PM_BANK))
		{
			MS_ASSERT(0);
			return FALSE;
		}
		if (FALSE == HAL_MMIO_GetBase(&NonPM_Bank, &NonPM_Bank_SIZE, DRV_MMIO_NONPM_BANK))
		{
			MS_ASSERT(0);
			return FALSE;
		}

		if (FALSE == HAL_MMIO_GetBase(&Flash_Bank0, &Flash_Bank0_SIZE, DRV_MMIO_FLASH_BANK0))
		{
			MS_ASSERT(0);
			return FALSE;
		}

		#if defined(CHIP_C3)
		if (FALSE == HAL_MMIO_GetBase(&IMI_Bank, &IMI_Bank_SIZE, DRV_MMIO_IMI_BANK))
		{
			MS_ASSERT(0);
			return FALSE;
		}
		#endif

#ifdef ASIC_VERIFY

        #define OALPAtoUA(pa)       (VOID*)(((UINT32)(pa))|0xA0000000)

		if (PM_Bank)
		{
		    _u32PM_Bank = (MS_U32)OALPAtoUA(PM_Bank);

            if (0==_u32PM_Bank)
			{
				return FALSE;
			}
		}

		if (NonPM_Bank)
		{
		    _u32NonPM_Bank = (MS_U32)OALPAtoUA(NonPM_Bank);

            if (0==_u32NonPM_Bank)
			{
				return FALSE;
			}

		}

		if (Flash_Bank0)
		{
		    _u32Flash_Bank0 = (MS_U32)OALPAtoUA(Flash_Bank0);

            if (0==_u32Flash_Bank0)
			{
				return FALSE;
			}

		}
		#if defined(CHIP_C3)
		if (IMI_Bank)
		{
		    _u32IMI_Bank = (MS_U32)OALPAtoUA(IMI_Bank);

            if (0==_u32IMI_Bank)
			{
				return FALSE;
			}

		}
		#endif
#else

		if (PM_Bank)
		{
			ioPhysicalBase.QuadPart = PM_Bank;
			if (0==(_u32PM_Bank= (MS_U32)MmMapIoSpace(ioPhysicalBase, PM_Bank_SIZE, FALSE)))
			{
				return FALSE;
			}
		}

		if (NonPM_Bank)
		{
			ioPhysicalBase.QuadPart = NonPM_Bank;
			if (0==(_u32NonPM_Bank= (MS_U32)MmMapIoSpace(ioPhysicalBase, NonPM_Bank_SIZE, FALSE)))
			{
				return FALSE;
			}
		}

		if (Flash_Bank0)
		{
			ioPhysicalBase.QuadPart = Flash_Bank0;
			if (0==(_u32Flash_Bank0= (MS_U32)MmMapIoSpace(ioPhysicalBase, Flash_Bank0_SIZE, FALSE)))
			{
				return FALSE;
			}
		}
		#if defined(CHIP_C3)
		if (IMI_Bank)
		{
			ioPhysicalBase.QuadPart = IMI_Bank;
			if (0==(_u32IMI_Bank= (MS_U32)MmMapIoSpace(ioPhysicalBase, IMI_Bank_SIZE, FALSE)))
			{
				return FALSE;
			}
		}
		#endif
#endif
		initialized=TRUE;
		RETAILMSG(TRUE,( TEXT( "[MMIO] initialize success...\r\n" )));
	}

	return TRUE;
}

MS_BOOL MDrv_MMIO_Close(void)
{
#ifdef ASIC_VERIFY

#else
	if (_u32PM_Bank)
		MmUnmapIoSpace((void*)_u32PM_Bank,   _u32PM_Bank_SIZE);

	if (_u32NonPM_Bank)
		MmUnmapIoSpace((void*)_u32NonPM_Bank, _u32NonPM_Bank_SIZE);

	if (_u32Flash_Bank0)
		MmUnmapIoSpace((void*)_u32Flash_Bank0, _u32Flash_Bank0_SIZE);

    #if defined(CHIP_C3)
    if (_u32IMI_Bank)
        MmUnmapIoSpace((void*)_u32IMI_Bank, _u32IMI_Bank_SIZE);
    #endif
#endif
	return TRUE;
}

MS_BOOL MDrv_MMIO_GetBASE(MS_U32 *pu32BaseAddr, MS_U32 *pu32BaseSize, MS_U32 u32Module)
{
	MS_U16              u16Type;
	MS_BOOL             bRet;

	u16Type = HAL_MMIO_GetType(u32Module);
	bRet    = FALSE;

	if (u16Type == DRV_MMIO_PM_BANK)
	{
		*pu32BaseAddr = _u32PM_Bank;
		*pu32BaseSize = _u32PM_Bank_SIZE;
		bRet          = TRUE;
	}

	else if (u16Type == DRV_MMIO_NONPM_BANK)
	{
		*pu32BaseAddr = _u32NonPM_Bank;
		*pu32BaseSize = _u32NonPM_Bank_SIZE;
		bRet          = TRUE;
	}

	else if (u16Type == DRV_MMIO_FLASH_BANK0)
	{
		*pu32BaseAddr = _u32Flash_Bank0;
		*pu32BaseSize = _u32Flash_Bank0_SIZE;
		bRet          = TRUE;
	}

    #if defined(CHIP_C3)
	else if (u16Type == DRV_MMIO_IMI_BANK)
	{
		*pu32BaseAddr = _u32IMI_Bank;
		*pu32BaseSize = _u32IMI_Bank_SIZE;
		bRet          = TRUE;
	}
    #endif

	// Particular HW base address
	if ( !bRet && HAL_MMIO_GetIPBase(pu32BaseAddr, u16Type))
	{
		*pu32BaseSize = 0;
		bRet          = TRUE;
	}

	return bRet;
}


#else

MS_BOOL MDrv_MMIO_Init(void)
{
	// @TODO: remove the getbase code in MMIO_GetBASE?
	HAL_MMIO_GetBase(&_u32PM_Bank,      &_u32PM_Bank_SIZE,      DRV_MMIO_PM_BANK);
	HAL_MMIO_GetBase(&_u32NonPM_Bank,   &_u32NonPM_Bank_SIZE,   DRV_MMIO_NONPM_BANK);
	HAL_MMIO_GetBase(&_u32Flash_Bank0,  &_u32Flash_Bank0_SIZE,  DRV_MMIO_FLASH_BANK0);
	HAL_MMIO_GetBase(&_u32OTP_Bank,     &_u32OTP_BankSize,      DRV_MMIO_OTP_BANK); // query DRV_MMIO capability to HAL
	HAL_MMIO_GetBase(&_u32SPRAM_Bank,   &_u32SPRAM_BankSize,    DRV_MMIO_SPRAM_BANK);
	#if defined(CHIP_A5) || defined(CHIP_A5P)
	HAL_MMIO_GetBase(&_u32FRC_Bank,     &_u32FRC_Bank_SIZE,     DRV_MMIO_FRC_BANK);
	#endif

	// U3 Only
	HAL_MMIO_GetBase(&_u32NSK_Bank,     &_u32NSK_BankSize,      DRV_MMIO_NSK_BANK); // U3 Only
	return TRUE;

}


MS_BOOL MDrv_MMIO_Close(void)
{
	return TRUE;
}


MS_BOOL MDrv_MMIO_GetBASE(MS_U32 *pu32BaseAddr, MS_U32 *pu32BaseSize, MS_U32 u32Module)
{

	MS_BOOL             bRet;
	MS_U16              u16Type;

	u16Type = HAL_MMIO_GetType(u32Module);

	// @TODO: Leave only in MMIO_Init?
	switch (u16Type)
	{
	case DRV_MMIO_PM_BANK:
		bRet = HAL_MMIO_GetBase(pu32BaseAddr, pu32BaseSize, DRV_MMIO_PM_BANK);
		break;
	case DRV_MMIO_NONPM_BANK:
		bRet = HAL_MMIO_GetBase(pu32BaseAddr, pu32BaseSize, DRV_MMIO_NONPM_BANK);
		break;
	case DRV_MMIO_FLASH_BANK0:
		bRet = HAL_MMIO_GetBase(pu32BaseAddr, pu32BaseSize, DRV_MMIO_FLASH_BANK0);
		break;
	case DRV_MMIO_OTP_BANK:
		bRet = HAL_MMIO_GetBase(pu32BaseAddr, pu32BaseSize, DRV_MMIO_OTP_BANK); // query DRV_MMIO capability to HAL
		break;
	case DRV_MMIO_SPRAM_BANK:
		bRet = HAL_MMIO_GetBase(pu32BaseAddr, pu32BaseSize, DRV_MMIO_SPRAM_BANK); // query DRV_MMIO capability to HAL
		break;
	#if defined(CHIP_A5) || defined(CHIP_A5P)
	case DRV_MMIO_FRC_BANK:
		bRet = HAL_MMIO_GetBase(pu32BaseAddr, pu32BaseSize, DRV_MMIO_FRC_BANK);
		break;
	#endif

	default:
		bRet = FALSE;
		break;
	}

	// specific HW base address
	if (bRet == FALSE)
	{
		bRet = HAL_MMIO_GetIPBase(pu32BaseAddr, u16Type);
	}

	return bRet;

}
#endif

#endif
