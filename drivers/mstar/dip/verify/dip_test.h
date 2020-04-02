#ifndef DIP_TEST_H
#define DIP_TEST_H

/// data type unsigned char, data length 1 byte
typedef unsigned char               MS_U8;                              // 1 byte
/// data type unsigned short, data length 2 byte
typedef unsigned short              MS_U16;                             // 2 bytes
/// data type unsigned int, data length 4 byte
typedef unsigned int                MS_U32;                             // 4 bytes
/// data type unsigned int, data length 8 byte
typedef unsigned long long          MS_U64;                             // 8 bytes
/// data type signed char, data length 1 byte
typedef signed char                 MS_S8;                              // 1 byte
/// data type signed short, data length 2 byte
typedef signed short                MS_S16;                             // 2 bytes
/// data type signed int, data length 4 byte
typedef signed int                  MS_S32;                             // 4 bytes
/// data type signed int, data length 8 byte
typedef signed long long            MS_S64;                             // 8 bytes
/// data type float, data length 4 byte
typedef float                       MS_FLOAT;                           // 4 bytes
/// data type pointer content
typedef size_t                      MS_VIRT;                            // 8 bytes
/// data type hardware physical address
typedef size_t                      MS_PHYADDR;                         // 8 bytes
/// data type 64bit physical address
typedef MS_U64                      MS_PHY;                             // 8 bytes
/// data type size_t
typedef size_t                      MS_SIZE;                            // 8 bytes
/// definition for MS_BOOL
typedef unsigned char               MS_BOOL;                            // 1 byte
/// print type  MPRI_PHY
#define                             MPRI_PHY                            "%x"
/// print type  MPRI_PHY
#define                             MPRI_VIRT                           "%tx"

typedef unsigned char         bool;

#ifdef NULL
    #undef NULL
#endif
#define NULL                        0

#define TRUE                        1
/// definition for FALSE
#define FALSE                       0
#define ENABLE                      1
#define DISABLE                     0

//#include "MsTypes.h"
//#include "MsOS.h" // for MsOS_ObtainMutex test
//#include "drvMMIO.h"
//#include "apiXC.h"
//#include "apiXC_DWIN.h"
//#include "mvideo_context.h"
//#include "drv_sc_DIP_scaling.h"
#include "mhal_divp_datatype.h"

// Debug Message
///ASCII color code
#define ASCII_COLOR_RED     "\033[1;31m"
#define ASCII_COLOR_WHITE   "\033[1;37m"
#define ASCII_COLOR_YELLOW  "\033[1;33m"
#define ASCII_COLOR_BLUE    "\033[1;36m"
#define ASCII_COLOR_GREEN   "\033[1;32m"
#define ASCII_COLOR_END     "\033[0m"

#if 1
    //#define FUNC_MSG(fmt, args...)            ({do{printf(ASCII_COLOR_GREEN"%s[%d] ",__FUNCTION__,__LINE__);printf(fmt, ##args);printf(ASCII_COLOR_END);}while(0);})
    #define FUNC_MSG(fmt, args...)            ({do{printf(fmt, ##args);}while(0);})
    #define FUNC_ERR(fmt, args...)            ({do{printf(ASCII_COLOR_RED"%s[%d] ",__FUNCTION__,__LINE__);printf(fmt, ##args);printf(ASCII_COLOR_END);}while(0);})
#else
    #define FUNC_MSG(fmt, args...)
    #define FUNC_ERR(fmt, args...)
#endif
// Buffer Management
#define DIP_MIU                 0
#define INPUT_BUFFER_OFFSET     (0x000E222000)
#define INPUT_BUFFER_SIZE       (0x0000400000)
#define OUTPUT_BUFFER_OFFSET    (0x000E622000)
#define OUTPUT_BUFFER_SIZE      (0x0000600000)
#if 1 //
    #define _3DDI_BUFFER_OFFSET    (0x000DC22000)
#else
    #define _3DDI_BUFFER_OFFSET    (0x000EC22000)
#endif
#define _3DDI_BUFFER_SIZE      (0x00600000)

typedef enum
{
    DIP_WINDOW = 0,         ///< DIP window
    DWIN0_WINDOW = 1,
    DWIN1_WINDOW = 2,
    MAX_DIP_WINDOW          /// The max support window
} SCALER_DIP_WIN;

/// Define source type for DIP
typedef enum
{
    SCALER_DIP_SOURCE_TYPE_SUB2 = 0,        ///< DIP from SUB2(IP_F3)
    SCALER_DIP_SOURCE_TYPE_MAIN = 1,        ///< DIP from MAIN(IP_MAIN)
    SCALER_DIP_SOURCE_TYPE_SUB  = 2,        ///< DIP from SUB (IP_SUB)
    SCALER_DIP_SOURCE_TYPE_OP_MAIN = 3,     ///< HVSP MAIN
    SCALER_DIP_SOURCE_TYPE_OP_SUB = 4,      ///< HVSP SUB
    SCALER_DIP_SOURCE_TYPE_DRAM = 5,        ///< DIP from DRAM
    SCALER_DIP_SOURCE_TYPE_OP_CAPTURE = 6, ///< DIP from OP capture
    SCALER_DIP_SOURCE_TYPE_OSD = 7,        ///< DIP from OSD capture
    SCALER_DIP_SOURCE_TYPE_SC2_OP_CAPTURE = 8, ///< DIP from SC2 OP capture
    SCALER_DIP_SOURCE_TYPE_OP_SC1_CAPTURE = 9, ///< DIP from OP SC1 capture
    SCALER_DIP_SOURCE_TYPE_MVOP0 = 10,           ///< DIP from MVOP0 capture
    SCALER_DIP_SOURCE_TYPE_MVOP1 = 11,          ///< DIP from MVOP1 capture
    MAX_SCALER_DIP_SOURCE_NUM            ///< The max support window
} SCALER_DIP_SOURCE_TYPE;

typedef struct
{
    MS_BOOL bEnable;
    MS_U16 u16X;
    MS_U16 u16Y;
    MS_U16 u16Width;
    MS_U16 u16Height;
} SWCrop_Window_t;

typedef struct
{
    MS_U8 u8SnapShotEnable;
    SCALER_DIP_WIN eDIPWindow;
    MS_BOOL bHMirror;
    MS_BOOL bVMirror;
    MHAL_DIVP_PixelFormat_e eInputDataFmt;
    MS_U16 u16SrcWidth;
    MS_U16 u16SrcHeight;
    MS_U16 u16SrcPitch;
    MS_U16 u16CapX;
    MS_U16 u16CapY;
    MS_U16 u16CapWidth;
    MS_U16 u16CapHeight;
    MS_U8 u8BufCnt;
    MS_U16 u16WinLineOffSet;
    MS_PHY u64WinOffSet;
    MS_U16 u16DstWidth;
    MS_U16 u16DstHeight;
    MS_BOOL bPIP;
    MS_BOOL b2P_Enable;
    SCALER_DIP_SOURCE_TYPE eSource;
    MHAL_DIVP_PixelFormat_e eOutputFormat;
    MHAL_DIVP_ScanMode_e enScan;
    MHAL_DIVP_TileMode_e eTileBlock;
    MHAL_DIVP_TnrLevel_e  eTNRType;
    MHAL_DIVP_DiType_e  e3DDIType;
    MHAL_DIVP_Rotate_e  eRotation;
    MHAL_DIVP_MFdecInfo_t stMfdecInfo;///MFdec info
    SWCrop_Window_t stSwCropWin;
} ST_DIP_Test_Property;

typedef struct
{
    MS_U64 u64PhyBufAddr[3];
    void *u64VirBufAddr[3];
    MS_U32 u32BufSize;
    MS_U8 u8FrameCnt;
    MS_U32 u32BufOffset;
} DIVP_Buffer_Config_t;

#endif // DIP_TEST_H
