///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////

///

/// file    halMMIO.c

/// @brief  memory map io (MMIO) HAL

/// @author MStar Semiconductor Inc.

///////////////////////////////////////////////////////////////////////////////////////////////////





//-------------------------------------------------------------------------------------------------

//  Include Files

//-------------------------------------------------------------------------------------------------
#include "MsCommon.h"

#include "halMMIO.h"

#include "drvMMIO.h"



// for getting mapped IO base from DRV

extern MS_U32   _u32PM_Bank         ;

extern MS_U32   _u32PM_Bank_SIZE    ;

extern MS_U32   _u32NonPM_Bank      ;

extern MS_U32   _u32NonPM_Bank_SIZE ;

#if defined(CHIP_A5)

extern MS_U32   _u32FRC_Bank      ;

extern MS_U32   _u32FRC_Bank_SIZE ;

#endif



//-------------------------------------------------------------------------------------------------

//  Driver Compiler Options

//-------------------------------------------------------------------------------------------------





//--------------------------------------------------------------------------------------------------

//  Global Variables

//--------------------------------------------------------------------------------------------------

MS_U32 u32_ge0_mmio_base;





//-------------------------------------------------------------------------------------------------

//  Local Defines

//-------------------------------------------------------------------------------------------------

#if defined(MSOS_TYPE_LINUX) || defined(MSOS_TYPE_LINUX_KERNEL)


    #define HAL_MMIO_PM_BASE            0x1F000000
    #define HAL_MMIO_PM_SIZE            0x00800000
    #define HAL_MMIO_NONPM_BASE         0x1F200000
    #define HAL_MMIO_NONPM_SIZE         0x00080000
    #define HAL_MMIO_FLASH_BASE0        0x14000000
    #define HAL_MMIO_FLASH_SIZE0        0x00400000
	#define HAL_MMIO_IMI_BASE           0xE0000000
	#define HAL_MMIO_IMI_SIZE           0x00100000
#elif defined(MSOS_TYPE_CE)

    #define HAL_MMIO_PM_BASE            0x1F000000
    #define HAL_MMIO_PM_SIZE            0x00800000
    #define HAL_MMIO_NONPM_BASE         0x1F200000
    #define HAL_MMIO_NONPM_SIZE         0x00080000
    #define HAL_MMIO_FLASH_BASE0        0x14000000
    #define HAL_MMIO_FLASH_SIZE0        0x00400000
	#define HAL_MMIO_IMI_BASE           0xE0000000
	#define HAL_MMIO_IMI_SIZE           0x00100000

#else


    #if defined (MCU_AEON)

        #define HAL_MMIO_PM_BASE        0xA0000000

        #define HAL_MMIO_PM_SIZE        0x00007B80

        #define HAL_MMIO_NONPM_BASE     0xA0200000

        #define HAL_MMIO_NONPM_SIZE     0x00025600

        #define HAL_MMIO_FLASH_BASE0    0xA1000000 // non-cache // 0xA1000000 for cache

        #define HAL_MMIO_FLASH_SIZE0    0x4000000

    #elif defined (MCU_MIPS_4KE)

        #define HAL_MMIO_PM_BASE        0xbf000000

        #define HAL_MMIO_PM_SIZE        0x00007B80

        #define HAL_MMIO_NONPM_BASE     0xbf200000

        #define HAL_MMIO_NONPM_SIZE     0x00025600

        #define HAL_MMIO_FLASH_BASE0    0xB4000000 // 0x94000000 for cache

        #define HAL_MMIO_FLASH_SIZE0    0x4000000

    #elif defined (MCU_MIPS_34K)

        #define HAL_MMIO_PM_BASE        0xbf000000

        #define HAL_MMIO_PM_SIZE        0x00007B80

        #define HAL_MMIO_NONPM_BASE     0xbf200000

        #define HAL_MMIO_NONPM_SIZE     0x00025600

        #define HAL_MMIO_FLASH_BASE0    0xB4000000 // 0x94000000 for cache

        #define HAL_MMIO_FLASH_SIZE0    0x4000000

    #elif defined (MCU_MIPS_74K)

        #define HAL_MMIO_PM_BASE        0xbf000000

        #define HAL_MMIO_PM_SIZE        0x00007B80

        #define HAL_MMIO_NONPM_BASE     0xbf200000

        #define HAL_MMIO_NONPM_SIZE     0x00025600

        #define HAL_MMIO_FLASH_BASE0    0xB4000000 // 0x94000000 for cache

        #define HAL_MMIO_FLASH_SIZE0    0x4000000

    #elif defined (MCU_ARM_9)

        #define HAL_MMIO_PM_BASE        0xfd000000

        #define HAL_MMIO_PM_SIZE        0x00007B80

        #define HAL_MMIO_NONPM_BASE     0xfd200000

        #define HAL_MMIO_NONPM_SIZE     0x00025600

        #define HAL_MMIO_FLASH_BASE0    0xfe000000

        #define HAL_MMIO_FLASH_SIZE0    0x4000000

    #else

        #error "Please choose MCU";

    #endif

#endif


#if defined(MSOS_TYPE_CE) || defined(MSOS_TYPE_LINUX_KERNEL)

    #define HAL_MMIO_SC_IP_OFFSET   0x00130000

#else

    #define HAL_MMIO_SC_IP_OFFSET   0x00130000

#endif

//-------------------------------------------------------------------------------------------------

//  Local Structurs

//-------------------------------------------------------------------------------------------------





//-------------------------------------------------------------------------------------------------

//  Global Variables

//-------------------------------------------------------------------------------------------------





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

MS_U16  HAL_MMIO_GetType(MS_U32 u32Module)

{
	MS_U16 u16MmioType = 0xFFFF;

    switch (u32Module)

    {

    //HAL_MMIO_PM_BANK

    case MS_MODULE_PM       :

    case MS_MODULE_IR       :

    case MS_MODULE_ISP      :

    case MS_MODULE_PWS      :

        u16MmioType = DRV_MMIO_PM_BANK;
		break;


    //HAL_MMIO_NONPM_BANK

    case MS_MODULE_HW       :

    case MS_MODULE_CHIPTOP  :

    case MS_MODULE_MIU      :

    case MS_MODULE_ACE      :

    case MS_MODULE_AUDIO    :

    case MS_MODULE_AVD      :

    case MS_MODULE_BDMA     :

    case MS_MODULE_DLC      :

    case MS_MODULE_DMD      :

    case MS_MODULE_GE       :

    case MS_MODULE_GOP      :

    case MS_MODULE_GPIO     :

    case MS_MODULE_HVD      :

    case MS_MODULE_HWI2C    :

    case MS_MODULE_IRQ      :

    case MS_MODULE_JPD      :

    case MS_MODULE_MBX      :

    case MS_MODULE_MFE      :

    case MS_MODULE_MHEG5    :

    case MS_MODULE_MVD      :

    case MS_MODULE_MVOP     :

    case MS_MODULE_RVD      :

    case MS_MODULE_TSP      :

    case MS_MODULE_UART     :

    case MS_MODULE_VPU      :

    case MS_MODULE_XC       :

    case MS_MODULE_PCMCIA   :

    case MS_MODULE_PFSH     :

    case MS_MODULE_PNL      :

    case MS_MODULE_PWM      :

    case MS_MODULE_SEM      :

    case MS_MODULE_VBI      :

    case MS_MODULE_VIF      :

    case MS_MODULE_DIP      :

	case MS_MODULE_MPIF     :

	case MS_MODULE_MMFILEIN :

    case MS_MODULE_GPD      :

        u16MmioType = DRV_MMIO_NONPM_BANK;
		break;


    case MS_MODULE_SC       :

        u16MmioType = DRV_MMIO_SC_BANK;
		break;


     //HAL_MMIO_FLASH_BANK0

    case MS_MODULE_FLASH    :

        u16MmioType = DRV_MMIO_FLASH_BANK0;
		break;


	case MS_MODULE_IMI      :

		u16MmioType = DRV_MMIO_IMI_BANK;
        break;


    default:

        u16MmioType = 0xFFFF; //undefine type
		break;
    }



    return u16MmioType; //undefine type

}





MS_BOOL HAL_MMIO_GetBase(MS_U32* pu32BaseAddr, MS_U32* pu32BaseSize, MS_U32 u32BankType)

{

    MS_BOOL bRet = TRUE;



    *pu32BaseAddr = 0;

    *pu32BaseSize = 0;

    switch (u32BankType)

    {

    case DRV_MMIO_PM_BANK:

        *pu32BaseAddr = HAL_MMIO_PM_BASE;

        *pu32BaseSize = HAL_MMIO_PM_SIZE;

        break;

    case DRV_MMIO_NONPM_BANK:

        *pu32BaseAddr = HAL_MMIO_NONPM_BASE;

        *pu32BaseSize = HAL_MMIO_NONPM_SIZE;

        break;

    case DRV_MMIO_FLASH_BANK0:

        *pu32BaseAddr = HAL_MMIO_FLASH_BASE0;

        *pu32BaseSize = HAL_MMIO_FLASH_SIZE0;

	case DRV_MMIO_IMI_BANK:

#if defined(MSOS_TYPE_CE)
        *pu32BaseAddr = HAL_MMIO_IMI_BASE;

        *pu32BaseSize = HAL_MMIO_IMI_SIZE;
#endif
        break;

    default:

//        MS_ASSERT(0);

        bRet = FALSE;

        break;

    }



    return bRet;

}





// @NOTE: Only run after MMIO_Init

MS_BOOL HAL_MMIO_GetIPBase(MS_U32 *pu32BaseAddr, MS_U16 u16BankType)

{
    MS_U32 u32mmioSCIPBase = 0;

    *pu32BaseAddr = 0;


    // if MMIO_Init is not yet initialized.

    if ( (_u32PM_Bank_SIZE == 0x0) || (_u32NonPM_Bank_SIZE == 0x0) )

    {

        return FALSE;

    }

#ifdef MSOS_TYPE_CE

    u32mmioSCIPBase = _u32PM_Bank;

#else

    u32mmioSCIPBase = _u32NonPM_Bank;

#endif



    switch (u16BankType)

    {

    case DRV_MMIO_SC_BANK:

        *pu32BaseAddr =   u32mmioSCIPBase    + HAL_MMIO_SC_IP_OFFSET;

        break;

    default:

        return FALSE;

    }



    return TRUE;

}





void HAL_MMIO_FlushMemory(void)

{



//#if defined(MIPS_MCU)

//#ifndef MSOS_TYPE_LINUX //Linux driver call will be handled at drv level

//

//    static unsigned char u8_4Lines[64];

//    volatile unsigned char *pu8;

//    volatile unsigned char tmp ;

//

//    // MIU0

//    // Transfer the memory to noncache memory

//    pu8 = ((volatile unsigned char *)(((unsigned int)u8_4Lines) | 0xa0000000));

//

//    // Flush the data from pipe and buffer in MIU

//    pu8[0] = pu8[16] = pu8[32] = pu8[48] = 1;

//    // Flush the data in the EC bridge buffer

//    MAsm_CPU_Sync();

//    // final read back

//    tmp = pu8[48] ;

//

//    // MIU1

//    // Transfer the memory to noncache memory

//    // The address 0x3FFF000 is reserved 4K for flush memoy temp buffer

//    pu8 = ((volatile unsigned char *)(((unsigned int)MsOS_PA2KSEG1(0x3FFF000 + HAL_MIU1_BASE)));

//

//    // Flush the data from pipe and buffer in MIU

//    pu8[0] = pu8[16] = pu8[32] = pu8[48] = 1;

//    // Flush the data in the EC bridge buffer

//    MAsm_CPU_Sync();

//    // final read back

//    tmp = pu8[48] ;

//

//#endif

//#endif



}



void HAL_MMIO_ReadMemory(void)

{



//#if defined(MIPS_MCU)

//#ifndef MSOS_TYPE_LINUX //Linux driver call will be handled at drv level

//

//    volatile unsigned int *pu8;

//    volatile unsigned int t ;

//

//    // MIU0

//    // Transfer the memory to noncache memory

//    pu8 = ((volatile unsigned int *)0xA0380000);

//    t = pu8[0] ;

//    t = pu8[64] ;

//

//    // MIU1

//    // Transfer the memory to noncache memory

//    pu8 = ((volatile unsigned int *)MsOS_PA2KSEG1(0x380000 + HAL_MIU1_BASE));

//    t = pu8[0] ;

//    t = pu8[64] ;

//#endif

//#endif



}





