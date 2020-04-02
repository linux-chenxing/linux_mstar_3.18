#ifndef __DYNAMIC__SCALE__
#define __DYNAMIC__SCALE__

#include  "mhal_xc_chip_config.h"

/*
Check BYTE_PER_WORD with dynamic_scale.c
For memory alignment, if complier error need to add define and modify dynamic_scale_init()
at dynamic_scale.c

#define BYTE_PER_WORD           (16)

*/
#if (defined(CHIP_T12)||defined(CHIP_T13)||defined(CHIP_J2)||defined(CHIP_A1)||defined(CHIP_A2)||defined(CHIP_A6)||defined(CHIP_A7)||defined(CHIP_AMETHYST) || defined(CHIP_EAGLE) || defined(CHIP_C3))
  #if(BYTE_PER_WORD!=16)
   #ERROR
  #endif
#elif(defined(CHIP_A5)||defined(CHIP_A3)  || defined(CHIP_AGATE))
  #if(BYTE_PER_WORD!=32)
   #ERROR
  #endif
#elif(defined(CHIP_T7)||defined(CHIP_K1)||defined(CHIP_K2)||defined(CHIP_U4))
  #if(BYTE_PER_WORD !=8)
  #ERROR
  #endif
#else
  #if(BYTE_PER_WORD !=8)
  #ERROR
  #endif
#endif

/*
M10 J2 A5 A6 supoort field packet mode
#define _FIELD_PACKING_MODE_SUPPORTED       0
*/
#if(defined(CHIP_J2)|| defined(CHIP_A5) || defined(CHIP_A6) ||defined(CHIP_A3) || defined(CHIP_AGATE))|| defined(CHIP_C3)
#define ENABEL_FIELD_PACKING_MODE_SUPPORTED 1
#else
#define ENABEL_FIELD_PACKING_MODE_SUPPORTED 0
#endif

#if(ENABEL_FIELD_PACKING_MODE_SUPPORTED !=_FIELD_PACKING_MODE_SUPPORTED)
#ERROR
#endif

#define DS_compiler 0  // not to compiler oringial dynamic scaling code.

#if DS_compiler
#define DS_DEBUG    1
#if DS_DEBUG
    #define ERROR_CODE(x)       do{pDSIntData->ErrorCode = x;}while(0)
#else
    #define ERROR_CODE(x)
#endif

#define DS_VERSION          0x03


#define DS_COMMAND_BYTE_LEN 16
#define DS_BUFFER_DEPTH     16
#define DS_BUFFER_NUM       12

typedef enum
{
    DS_Euclid,
    DS_T3,
    DS_T4,
    DS_T7,
    DS_T8,
    DS_T9,
    DS_Unarus4,
    DS_T12,
    DS_T13,
    DS_MARIA10,
    DS_J2,
    DS_K1,
    DS_K2,
    DS_A1,
    DS_A2,
    DS_A3,
    DS_A5,
    DS_A6,
    DS_A7,
    DS_AGATE,
    DS_AMETHYST,
    DS_EAGLE,
} ds_chip_id;

typedef struct
{
    union
    {
        struct
        {
            unsigned int reg_value      : 16;
            unsigned int addr           : 8;
            unsigned int bank           : 8;
        };

        unsigned int reg_setting;
    };
} ds_reg_setting_structure;

typedef struct
{
    unsigned short x;           ///<start x of the window
    unsigned short y;           ///<start y of the window
    unsigned short width;       ///<width of the window
    unsigned short height;      ///<height of the window
} MS_WINDOW_TYPE;

typedef struct
{
    // 0x000
    unsigned char u8DSVersion;                  // Versin
    unsigned char bHKIsUpdating;                // House keeping is writing
    unsigned char bFWIsUpdating;                // firmware is reading
    unsigned char bFWGotXCInfo;                 // for firmware internal use, indicate that firmware received XC current setting
    unsigned char u8XCInfoUpdateIdx;            //
    unsigned char bFWGotNewSetting;             // for firmware internal use, indicate that firmware received XC current setting
    unsigned char u8Dummy_3;
    unsigned char u8Dummy_2;

    // 0x008
    MS_WINDOW_TYPE stCapWin;
    MS_WINDOW_TYPE stCropWin;

    // 0x018
    MS_WINDOW_TYPE stDispWin;
    unsigned short u16H_SizeAfterPreScaling;
    unsigned short u16V_SizeAfterPreScaling;
    unsigned long  u32PNL_Width;                ///<Panel active width

    // 0x028
    unsigned long u32IPMBase0;                  ///<IPM base 0
    unsigned long u32IPMBase1;                  ///<IPM base 1
    unsigned long u32IPMBase2;                  ///<IPM base 2

    // 0x034
    MS_WINDOW_TYPE stNewCropWin;                ///< Zoom in/out new crop win
    MS_WINDOW_TYPE stNewDispWin;                ///< Zoom in/out new disp win

    // 0x044
    unsigned char bLinearMode;                  ///<Is current memory format LinearMode?
    unsigned char u8BitPerPixel;                ///<Bits number Per Pixel
    unsigned char bInterlace;                   ///<Interlaced or Progressive
    unsigned char u8StoreFrameNum;

    // 0x048
    unsigned short u16IPMOffset;                ///<IPM offset
    unsigned short u16Dummy0;

    // 0x4C
    unsigned char bMirrorMode;                  ///<Mirror
    unsigned char u8Dummy2[3];

    // internal use only
    // 0x050
} ds_xc_data_structure;

typedef struct
{
    ds_chip_id enChipID;                            // ChipID
    unsigned int bIsInitialized;                    // BSS will be cleared to 0x00
    unsigned int MemAlign;                          // depends on chips, unit = bytes
    unsigned int OffsetAlign;                       // depends on chips, unit = bytes (for Crop)
    unsigned int DS_Depth;                          // dynamic scaling depth
    unsigned int MaxWidth;                          // max width of frame buffer, assigned by MM application
    unsigned int MaxHeight;                         // max height of frame buffer, assigned by MM application
    unsigned char *pDSBufBase;                      // DS buffer
    unsigned int  DSBufSize;                        // DS buffer size
    ds_xc_data_structure *pDSXCData;                // MM passed XC info data

    // for filling DS buffer
    ds_reg_setting_structure *pCurDSBuf;            // pointer to current DS buffer based on pDSBufBase & DSBufIdx
    ds_reg_setting_structure *pOPRegSetting;        // point to current writable OP reg setting
    ds_reg_setting_structure *pIPRegSetting;        // point to current writable IP reg setting
    unsigned char OPRegMaxDepth;                    // point to record max OP reg depth
    unsigned char IPRegMaxDepth;                    // point to record max IP reg depth
    unsigned char ErrorCode;                        // point to record error code
} ds_internal_data_structure;

typedef struct
{
    // IN
    ds_chip_id ChipID;                              // chip id
    ds_internal_data_structure *pDSIntData;          // internal buffer for DS to store global variables
    unsigned int MaxWidth;                          // max width of frame buffer
    unsigned int MaxHeight;                         // max height of frame buffer
    unsigned char *pDSBufBase;                      // DS buffer
    unsigned int  DSBufSize;                        // DS buffer size
    unsigned char *pDSXCData;                       // To store XC info passed from MM
} ds_init_structure;

typedef struct
{
    ds_internal_data_structure *pDSIntData;          // internal buffer for DS to store global variables
    unsigned short  CodedWidth;                     // coded width, current decoded width
    unsigned short  CodedHeight;                    // coded height, current decoded height
    unsigned short  DSBufIdx;                       // fill to which index of DS buffer
} ds_recal_structure;

extern void dynamic_scale_init(ds_init_structure *pDS_Init_Info);
extern void dynamic_scale(ds_recal_structure *pDS_Info);
extern void dynamic_scale_info_update(unsigned char *pDS_Update_Info, ds_internal_data_structure *pDS_Int_Data);
extern unsigned char dynamic_scale_is_got_xc_info(ds_internal_data_structure *pDS_Int_Data);
extern unsigned char dynamic_scale_get_xc_info_update_idx(ds_internal_data_structure *pDS_Int_Data);
extern unsigned char dynamic_scale_is_xc_fw_get_new_setting(ds_internal_data_structure *pDS_Int_Data);
#endif
#endif

