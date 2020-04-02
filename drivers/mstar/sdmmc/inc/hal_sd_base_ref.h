/***************************************************************************************************************
 *
 * FileName hal_sd_base_ref.h
 *     @author jeremy.wang (2017/09/14)
 * Desc:
 *
 *     Thie file could be reffered by hal_sd_base.h and put to the same folder.
 *     More detail description please look at hal_sd_base.h.
 *
 *     For Base Setting:
 *     (1) BASE OS/CPU/FCIE_Ver/Board_Ver/IC_Ver.. Option Type Define
 *     (2) BASE TYPE Define
 *     (3) BASE IP Type Define
 *     (4) BASE SD Logic Type Define
 *
 *     P.S. D_XX for define and D_XX__ two under line("__") to distinguish define and its define option.
 *
 ***************************************************************************************************************/

#ifndef __HAL_SD_BASE_REF_H
#define __HAL_SD_BASE_REF_H

//***********************************************************************************************************
// (1) BASE OS/CPU/FCIE_Ver/Board_Ver/IC_Ver.. Option Type Define
//***********************************************************************************************************


// OS Type Option
//-----------------------------------------------------------------------------------------------------------
#define D_OS__LINUX       1
#define D_OS__UBOOT       2
#define D_OS__WINCE       3
#define D_OS__EBOOT       4
#define D_OS__ROM         5
#define D_OS__YBOX        6
#define D_OS__RTK         7
#define D_OS__MBOOT       8
#define D_OS__NONE        9


// CPU Type Option
//-----------------------------------------------------------------------------------------------------------
#define D_CPU__ARM        1
#define D_CPU__MIPS       2


// FCIE Type Major Version
//-----------------------------------------------------------------------------------------------------------
#define D_FCIE_M_VER__04        1
#define D_FCIE_M_VER__05        2


// FCIE Type Revision Version
//-----------------------------------------------------------------------------------------------------------
#define D_FCIE_R_VER__00        0
#define D_FCIE_R_VER__01        1
#define D_FCIE_R_VER__02        2
#define D_FCIE_R_VER__03        3
#define D_FCIE_R_VER__04        4
#define D_FCIE_R_VER__05        5


// BOARD Type Version
//-----------------------------------------------------------------------------------------------------------
#define D_BOARD_VER__00         0
#define D_BOARD_VER__01         1
#define D_BOARD_VER__02         2
#define D_BOARD_VER__03         3
#define D_BOARD_VER__04         4
#define D_BOARD_VER__05         5


// IC Version Option
//-----------------------------------------------------------------------------------------------------------
#define D_ICVER__00      0
#define D_ICVER__01      1
#define D_ICVER__02      2
#define D_ICVER__03      3
#define D_ICVER__04      4
#define D_ICVER__05      5


//***********************************************************************************************************
// (2) BASE Type Define
//***********************************************************************************************************
typedef unsigned char      U8_T;
typedef unsigned short     U16_T;
typedef unsigned int       U32_T;
typedef unsigned long long U64_T;
typedef signed char        S8_T;
typedef signed short       S16_T;
typedef signed int         S32_T;
typedef signed long long   S64_T;
typedef unsigned char      BOOL_T;

#define FALSE   0
#define TRUE    1

#define BIT00_T 0x0001
#define BIT01_T 0x0002
#define BIT02_T 0x0004
#define BIT03_T 0x0008
#define BIT04_T 0x0010
#define BIT05_T 0x0020
#define BIT06_T 0x0040
#define BIT07_T 0x0080
#define BIT08_T 0x0100
#define BIT09_T 0x0200
#define BIT10_T 0x0400
#define BIT11_T 0x0800
#define BIT12_T 0x1000
#define BIT13_T 0x2000
#define BIT14_T 0x4000
#define BIT15_T 0x8000
#define BIT16_T 0x00010000
#define BIT17_T 0x00020000
#define BIT18_T 0x00040000
#define BIT19_T 0x00080000
#define BIT20_T 0x00100000
#define BIT21_T 0x00200000
#define BIT22_T 0x00400000
#define BIT23_T 0x00800000
#define BIT24_T 0x01000000
#define BIT25_T 0x02000000
#define BIT26_T 0x04000000
#define BIT27_T 0x08000000
#define BIT28_T 0x10000000
#define BIT29_T 0x20000000
#define BIT30_T 0x40000000
#define BIT31_T 0x80000000


//***********************************************************************************************************
// (3) BASE IP Type Define
//***********************************************************************************************************
typedef enum
{
    EV_IP_FCIE1     = 0,
    EV_IP_FCIE2     = 1,
    EV_IP_FCIE3     = 2,

} IPEmType;


typedef enum
{
    EV_PORT_NOSET             = 0,
    EV_PORT_SD                = 1,
    EV_PORT_SDIO1             = 2,
    EV_PORT_SDIO2             = 3,
    EV_PFCIE5_FCIE            = 4,   //Use Port Setting for FCIE5 => Function Ctrl Setting (FCIE)
    EV_PFCIE5_SDIO            = 5,   //Use Port Setting for FCIE5 => Function Ctrl Setting (SDIO)
    EV_PFCIE5_SDIO_PLL        = 6,   //Use Port Setting for FCIE5 => Function Ctrl Setting (SDIO) and PLL PAD using
    EV_PFCIE5_SDIO_PLL_NU     = 7,   //Use Port Setting for FCIE5 => Function Ctrl Setting (SDIO) and PLL PAD but no using

} PortEmType;


typedef enum
{
    EV_PAD1         = 0,
    EV_PAD2         = 1,
    EV_PAD3         = 2,
    EV_PAD4         = 3,
    EV_PAD5         = 4,

} PADEmType;


//***********************************************************************************************************
// (4) BASE SD Logic Type Define
//***********************************************************************************************************
typedef enum
{
    EV_BUS_LOW            = 0,
    EV_BUS_DEF_SDR12      = 1,
    EV_BUS_HS_SDR25       = (1<<1),  //
    EV_BUS_SDR50          = (1<<2),  //1<<(2)UHS_SDR50_BUS_SPEED
    EV_BUS_SDR104_HS200   = (1<<3),  //1<<(3)UHS_SDR104_BUS_SPEED
    EV_BUS_DDR50          = (1<<4),  //1<<(4)UHS_DDR50_BUS_SPEED

} BusTimingEmType;


typedef enum
{
    EV_OK   = 0,
    EV_FAIL = 1,

} RetEmType;



#endif //End of __HAL_SD_BASE_REF_H