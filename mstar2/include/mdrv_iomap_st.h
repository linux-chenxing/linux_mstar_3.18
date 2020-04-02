
#ifndef _MDRV_MIOMAP_ST_H_
#define _MDRV_MIOMAP_ST_H_


//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
// MIOMAP_IOC_INFO 
#if defined(CONFIG_ARM) || defined(CONFIG_MIPS)
#ifdef CONFIG_MP_NEW_UTOPIA_32BIT
typedef struct
{
    u64                u32Addr;
    u64                u32Size;
} DrvMIOMap_Info_t;
#else
typedef struct
{
    unsigned int                u32Addr;
    unsigned int                u32Size;
} DrvMIOMap_Info_t;
#endif //CONFIG_MP_NEW_UTOPIA_32BIT
#elif defined(CONFIG_ARM64)
typedef struct
{
    u64                u32Addr;
    u64                u32Size;
} DrvMIOMap_Info_t;
#endif

typedef struct
{
	unsigned short u16chiptype ; // T2/Oberon/Euclid ...
	unsigned short u16chipversion ; // U01 U02 U03 ...
} DrvMIOMap_ChipInfo_t ;

#endif // _MDRV_MIOMAP_ST_H_

