
#if defined(MSOS_TYPE_LINUX_KERNEL)
#include <linux/wait.h>
#endif

#include "dynamic_scale.h"

#if DS_compiler
#include <stdint.h>
#include "global.h"

/////////////////////////////////////////////////////////
// local defines, structures
/////////////////////////////////////////////////////////

#define DS_DEBUG_PRT 0

#if DS_DEBUG_PRT
  #define ds_printf   aeon_printf3
#else
  #define ds_printf
#endif

#define H_PreScalingDownRatio(Input, Output)     ( (((Output)) * 1048576ul)/ (Input) + 1 )
#define H_PreScalingDownRatioAdv(Input, Output)  ( (((Input)-1) * 1048576ul)/ ((Output)-1) ) //Advance scaling
#define V_PreScalingDownRatio(Input, Output)     ( (((Output)) * 1048576ul)/ (Input) + 1 )

#define H_PostScalingRatio(Input, Output)        ( ((Input)) * 1048576ul / (Output) + 1 )
#define V_PostScalingRatio(Input, Output)        ( ((Input)-1) * 1048576ul / ((Output)-1) + 1 )

typedef enum
{
    DS_IP,
    DS_OP,
} ds_reg_ip_op_sel;

typedef enum
{
    MM_DS_XC_CMD_UPDATE_ZOOM_INFO = 1,
    MM_DS_XC_CMD_UPDATE_XC_INFO = 2,
} MM_DS_XC_CMD;

/////////////////////////////////////////////////////////
// local utilities
/////////////////////////////////////////////////////////
static void ds_memset(void *dstvoid, char val, int length)
{
  char *dst = dstvoid;

  while (length--)
    *dst++ = val;
}

static void *ds_memcpy (void *__restrict dstvoid,
              __const void *__restrict srcvoid, unsigned int length)
{
  char *dst = dstvoid;
  const char *src = (const char *) srcvoid;

  while (length--)
    *dst++ = *src++;
  return dst;
}

void dynamic_scale_init(ds_init_structure *pDS_Init_Info)
{
    ds_internal_data_structure *pDSIntData;

    pDSIntData = pDS_Init_Info->pDSIntData;

    // clear DS buffer
    ds_memset(pDS_Init_Info->pDSBufBase, 0xFF, pDS_Init_Info->DSBufSize);

    // decide memory alignement based on chip ID
    pDSIntData->enChipID =  pDS_Init_Info->ChipID;
    switch(pDS_Init_Info->ChipID)
    {
        case DS_T3:
        case DS_T8:
        case DS_T9:
        case DS_T12:
        case DS_T13:
        case DS_J2:
        case DS_A1:
        case DS_A2:
        case DS_A6:
        case DS_A7:
        case DS_AMETHYST:
        case DS_EAGLE:
            pDSIntData->MemAlign = 16;
            break;

        case DS_A3:
        case DS_A5:
        case DS_AGATE:
            pDSIntData->MemAlign = 32;
            break;

        case DS_Euclid:
        case DS_T4:
        case DS_T7:
        case DS_MARIA10:
        case DS_K1:
        case DS_K2:
        default:
            pDSIntData->MemAlign = 8;
            break;
    }

    pDSIntData->OffsetAlign = pDSIntData->MemAlign * 2;     // for calculate crop

    // remember other settings
    pDSIntData->DS_Depth = DS_BUFFER_DEPTH;

    pDSIntData->MaxWidth = pDS_Init_Info->MaxWidth;
    pDSIntData->MaxHeight = pDS_Init_Info->MaxHeight;
    pDSIntData->pDSBufBase = pDS_Init_Info->pDSBufBase;
    pDSIntData->DSBufSize = pDS_Init_Info->DSBufSize;

    pDSIntData->pDSXCData = (ds_xc_data_structure *) pDS_Init_Info->pDSXCData;
    pDSIntData->pDSXCData->u8DSVersion = DS_VERSION;
    pDSIntData->pDSXCData->bFWGotXCInfo = 0;
    pDSIntData->pDSXCData->bFWGotNewSetting = 0;
    pDSIntData->pDSXCData->u8XCInfoUpdateIdx = 0;
    pDSIntData->pDSXCData->bFWIsUpdating = 0;
    pDSIntData->pDSXCData->bHKIsUpdating = 0;

    pDSIntData->bIsInitialized = 1;
}

// addr is 16bit address, reg_value contains 16bits data
static int ds_write_2bytes(ds_internal_data_structure *pDSIntData, ds_reg_ip_op_sel IPOP_Sel, unsigned int bank, unsigned int addr, unsigned int reg_value)
{
    unsigned int pntJump;

    pntJump = pDSIntData->MemAlign / sizeof(ds_reg_setting_structure);

    if(IPOP_Sel == DS_IP)
    {
        pDSIntData->pIPRegSetting->bank = bank;
        pDSIntData->pIPRegSetting->addr = addr;
        pDSIntData->pIPRegSetting->reg_value = reg_value;
        pDSIntData->pIPRegSetting += pntJump;

        pDSIntData->IPRegMaxDepth++;
    }
    else
    {

        pDSIntData->pOPRegSetting->bank = bank;
        pDSIntData->pOPRegSetting->addr = addr;
        pDSIntData->pOPRegSetting->reg_value = reg_value;
        pDSIntData->pOPRegSetting += pntJump;

        pDSIntData->OPRegMaxDepth++;
    }
}

// addr is 16bit address, reg_value contains 16bits data
static int ds_write_4bytes(ds_internal_data_structure *pDSIntData, ds_reg_ip_op_sel IPOP_Sel, unsigned int bank, unsigned int addr, unsigned int reg_value)
{
    ds_write_2bytes(pDSIntData, IPOP_Sel, bank, addr, reg_value & 0xFFFF);
    ds_write_2bytes(pDSIntData, IPOP_Sel, bank, addr+1, (reg_value & 0xFFFF0000) >> 16);
}

static void ds_write_buf_flush(ds_internal_data_structure *pDSIntData)
{
    while(pDSIntData->IPRegMaxDepth < pDSIntData->DS_Depth)
    {
        ds_write_2bytes(pDSIntData, DS_IP, 0xFF, 0x01, 0x0000);
    }

    while(pDSIntData->OPRegMaxDepth < pDSIntData->DS_Depth)
    {
        ds_write_2bytes(pDSIntData, DS_OP, 0xFF, 0x01, 0x0000);
    }
}

void dynamic_scale(ds_recal_structure *pDS_Info)
{
    ds_internal_data_structure *pDSIntData;
    ds_xc_data_structure *pDSXCData;
    MS_WINDOW_TYPE stDSCropWin;
    MS_WINDOW_TYPE stDSScaledCropWin;
    unsigned short u16HSizeAfterPreScaling;
    unsigned short u16VSizeAfterPreScaling;
    unsigned short u16LineBufOffset;
    unsigned short u16OPMFetch,u16IPMFetch,u16PNLWidth;
    unsigned short u16H_mirrorOffset = 0;
    unsigned long u32CropOffset;
    unsigned long u32PixelOffset;
    char u8Framenumber;

    pDSIntData = pDS_Info->pDSIntData;
    pDSXCData = pDSIntData->pDSXCData;
    u8Framenumber = pDSXCData->u8StoreFrameNum;

    print2(0xE000, PHD, pDSIntData->pDSBufBase, pDS_Info->DSBufIdx);

    // decide DS buf start point
    pDSIntData->pCurDSBuf = (ds_reg_setting_structure *)
                (pDSIntData->pDSBufBase + (pDS_Info->DSBufIdx * (pDSIntData->MemAlign * pDSIntData->DS_Depth)));

    // decide OP/IP reg setting start point
    pDSIntData->pOPRegSetting = pDSIntData->pCurDSBuf;
    pDSIntData->pIPRegSetting = pDSIntData->pCurDSBuf + 1;
    pDSIntData->OPRegMaxDepth = 0;
    pDSIntData->IPRegMaxDepth = 0;

    //ds_printf("XC[%d,%d, %d]\n", pDSXCData->bFWGotXCInfo, pDSXCData->u8XCInfoUpdateIdx, pDSXCData->bFWGotNewSetting);
    print1(0xEE0, PD, pDSXCData->u8StoreFrameNum);
    print3(0xEE1, PDDD, pDSXCData->bFWGotXCInfo, pDSXCData->u8XCInfoUpdateIdx, pDSXCData->bFWGotNewSetting);

    pDSXCData->bFWIsUpdating = 1;
    // no need to clear DS buffer here because all registers are the same except value
    if(pDSXCData->bFWGotXCInfo)
    {

        if(pDSXCData->bFWGotNewSetting)
        {
            ds_memcpy(&pDSXCData->stCropWin, &pDSXCData->stNewCropWin, sizeof(MS_WINDOW_TYPE));
            ds_memcpy(&pDSXCData->stDispWin, &pDSXCData->stNewDispWin, sizeof(MS_WINDOW_TYPE));
            pDSXCData->bFWGotNewSetting = 0;
            //UTL_printf("NewSetting !!!\n");
        }

        #if 0
        UTL_printf("Code[%d,%d]\n", pDS_Info->CodedWidth, pDS_Info->CodedHeight);
        UTL_printf("Max[%d,%d]\n",  pDSIntData->MaxWidth, pDSIntData->MaxHeight);
        UTL_printf("Cap[%d,%d]\n",  pDSXCData->stCapWin.width, pDSXCData->stCapWin.height);
        UTL_printf("Disp[%d,%d]\n", pDSXCData->stDispWin.width, pDSXCData->stDispWin.height);
        UTL_printf("Crop[%d,%d]\n", pDSXCData->stCropWin.x , pDSXCData->stCropWin.y);
        UTL_printf("Crop[%d,%d]\n", pDSXCData->stCropWin.width, pDSXCData->stCropWin.height);
        #endif

        print2(0xEE2, PDD, pDS_Info->CodedWidth, pDS_Info->CodedHeight);
        print2(0xEE3, PDD, pDSIntData->MaxWidth, pDSIntData->MaxHeight);
        print2(0xEE4, PDD, pDSXCData->stCapWin.width, pDSXCData->stCapWin.height);
        print2(0xEE5, PDD, pDSXCData->stDispWin.width,  pDSXCData->stDispWin.height);
        print2(0xEE6, PDD, pDSXCData->stCropWin.x, pDSXCData->stCropWin.y);
        print2(0xEE7, PDD, pDSXCData->stCropWin.width, pDSXCData->stCropWin.height);

        //ds_write_2bytes(pDSIntData, DS_IP, 0x12, 0x02, 0x47BB);
        if ( pDSIntData->enChipID == DS_T8 ||  pDSIntData->enChipID == DS_T9 ||
             pDSIntData->enChipID == DS_T12 || pDSIntData->enChipID == DS_T13 ||
             pDSIntData->enChipID == DS_MARIA10 )
        {
            ds_write_2bytes(pDSIntData, DS_IP, 0x1F, 0x14, 0x0000);
            ds_write_2bytes(pDSIntData, DS_OP, 0x1F, 0x14, 0x0000);
        }

        ///////////////////////////////////////////////////////
        // set IP capture & pre-scaling, DNR fetch/offset
        ///////////////////////////////////////////////////////
        ds_write_2bytes(pDSIntData, DS_IP, 0x01, 0x07, pDS_Info->CodedWidth);
        ds_write_2bytes(pDSIntData, DS_IP, 0x01, 0x06, pDS_Info->CodedHeight);
        if( (pDSIntData->enChipID != DS_MARIA10) && (pDSIntData->enChipID != DS_J2) &&
                (pDSIntData->enChipID != DS_A3)  && (pDSIntData->enChipID != DS_A5) &&
                (pDSIntData->enChipID != DS_A6)  &&(pDSIntData->enChipID != DS_AGATE))
        {
            ds_write_2bytes(pDSIntData, DS_IP, 0x12, 0x18, 0x1000 | pDS_Info->CodedHeight);
        }
        else
        {
            ds_write_2bytes(pDSIntData, DS_IP, 0x12, 0x18, 0x8000 | pDS_Info->CodedHeight);
        }
        #if 1
        // calculate H pre-scaling
        print2(0xEE8, PDD, pDS_Info->CodedWidth, pDSXCData->stDispWin.width);
        // calculate H pre-scaling
        //if(pDS_Info->CodedWidth > pDSIntData->MaxWidth)
        if(pDS_Info->CodedWidth > pDSXCData->stDispWin.width)
        {
            //u16HSizeAfterPreScaling = pDSIntData->MaxWidth;
            u16HSizeAfterPreScaling = pDSXCData->stDispWin.width;

            // pre-scaling down
            print2(0xEE9, PDD,   pDS_Info->CodedWidth,  pDSIntData->MaxWidth);
            ds_write_4bytes(pDSIntData, DS_IP, 0x02, 0x04, 0x80000000 | H_PreScalingDownRatio(pDS_Info->CodedWidth, u16HSizeAfterPreScaling));
        }
        else
        {
            u16HSizeAfterPreScaling = pDS_Info->CodedWidth;

            // pre-scaling down
            ds_write_4bytes(pDSIntData, DS_IP, 0x02, 0x04, 0x00000000);
        }

        // calculate V pre-scaling
        //if(pDS_Info->CodedHeight > pDSIntData->MaxHeight)
        if(pDS_Info->CodedHeight > pDSXCData->stDispWin.height)
        {
            //u16VSizeAfterPreScaling = pDSIntData->MaxHeight;
            u16VSizeAfterPreScaling = pDSXCData->stDispWin.height;

            // pre-scaling down
            print2(0xEE10, PDD,   pDS_Info->CodedHeight,  pDSIntData->MaxHeight);
            ds_write_4bytes(pDSIntData, DS_IP, 0x02, 0x08, 0x80000000 | V_PreScalingDownRatio(pDS_Info->CodedHeight, u16VSizeAfterPreScaling));
        }
        else
        {
            u16VSizeAfterPreScaling = pDS_Info->CodedHeight -1;
            ds_write_4bytes(pDSIntData, DS_IP, 0x02, 0x08, 0x80000000 | V_PreScalingDownRatio(pDS_Info->CodedHeight, u16VSizeAfterPreScaling));
        }

        /*
        for bandwidth issue, adjust the IPMFetch according the Hsize after pre-scaling
        But change the IPM fetch will cause the vidoe transition, suppose when change display window(zoom)
        will use black video and keep IPM fetch accroding the panel size.
        for mirror mode need to do horizontal mirror offset.
        */
        u16PNLWidth = (pDSXCData->u32PNL_Width+pDSIntData->MemAlign*2 - 1) & ~(pDSIntData->MemAlign*2-1);
        //u16IPMFetch = (u16HSizeAfterPreScaling + pDSIntData->MemAlign*2 - 1) & ~(pDSIntData->MemAlign*2-1);  //TODO: T3 should be 32 alignment
        u16IPMFetch = (pDSXCData->stDispWin.width + pDSIntData->MemAlign*2 - 1) & ~(pDSIntData->MemAlign*2-1);  //TODO: T3 should be 32 alignment
        print2(0xEE11, PDD,   pDSXCData->u32PNL_Width,  pDSXCData->stDispWin.width);
        //when do zoom change, change IPM for BW issue.
        //keep the IPM fetch when do scene change according display window.
        ds_write_2bytes(pDSIntData, DS_IP, 0x12, 0x0F, u16IPMFetch);

        if(pDSXCData->bMirrorMode)
        {
            if( (pDSIntData->enChipID != DS_MARIA10) && (pDSIntData->enChipID != DS_J2) &&
                (pDSIntData->enChipID != DS_A3)  && (pDSIntData->enChipID != DS_A5) &&
                (pDSIntData->enChipID != DS_A6)  &&(pDSIntData->enChipID != DS_AGATE))
            {
                u16H_mirrorOffset = (pDSIntData->MaxWidth * (pDSXCData->u8BitPerPixel/8)*u8Framenumber/pDSIntData->MemAlign)-3;
                ds_write_2bytes(pDSIntData, DS_IP, 0x12, 0x1C, u16H_mirrorOffset|0x1000);
            }
            else
            {
                u16H_mirrorOffset = (pDSIntData->MaxWidth * (pDSXCData->u8BitPerPixel/8)*u8Framenumber/pDSIntData->MemAlign);
                ds_write_2bytes(pDSIntData, DS_IP, 0x12, 0x1C, u16H_mirrorOffset|0x8000);
            }
        }
        else
        {
            ds_write_2bytes(pDSIntData, DS_IP, 0x12, 0x1C, 0x0000);
        }
        //ds_write_2bytes(pDSIntData, DS_IP, 0x12, 0x0F, u16IPMFetch);
        print2(0xEE12, PDD, u16H_mirrorOffset,u16IPMFetch);
        print2(0xEE13, PDD, u16HSizeAfterPreScaling, u16VSizeAfterPreScaling);
        #endif

        ///////////////////////////////////////////////////////
        // set OP base, fetch/offset & scaling ratio
        ///////////////////////////////////////////////////////
        // set v length
        // set v length later.
        //ds_write_2bytes(pDSIntData, DS_OP, 0x20, 0x15, u16VSizeAfterPreScaling);

        // calculate DS crop win based on new coded width/height and original crop win
        //stDSCropWin.x = (unsigned long) pDSXCData->stCropWin.x * pDS_Info->CodedWidth / pDSXCData->stCapWin.width;
        //stDSCropWin.width = (unsigned long) pDSXCData->stCropWin.width * pDS_Info->CodedWidth / pDSXCData->stCapWin.width;
        stDSCropWin.x = (unsigned long) pDSXCData->stCropWin.x * u16HSizeAfterPreScaling/ pDSXCData->stCapWin.width;
        stDSCropWin.width = (unsigned long) pDSXCData->stCropWin.width * u16HSizeAfterPreScaling / pDSXCData->stCapWin.width;

        //stDSCropWin.y = (unsigned long) pDSXCData->stCropWin.y * pDS_Info->CodedHeight / pDSXCData->stCapWin.height;
        //stDSCropWin.height = (unsigned long) pDSXCData->stCropWin.height * pDS_Info->CodedHeight / pDSXCData->stCapWin.height;
        stDSCropWin.y = (unsigned long) pDSXCData->stCropWin.y * u16VSizeAfterPreScaling / pDSXCData->stCapWin.height;
        stDSCropWin.height = (unsigned long) pDSXCData->stCropWin.height * u16VSizeAfterPreScaling / pDSXCData->stCapWin.height;

        //UTL_printf("DSCrop (%d,%d)\n", stDSCropWin.x, stDSCropWin.y);
        //UTL_printf("DSCrop (%d,%d)\n", stDSCropWin.width, stDSCropWin.height);

        print3(0xEE14, PDDD,  pDSXCData->stCropWin.height, pDS_Info->CodedHeight, pDSXCData->stCapWin.height);

        print2(0xEE15, PDD,  stDSCropWin.x, stDSCropWin.y);
        print2(0xEE16, PDD,  stDSCropWin.width, stDSCropWin.height);


        // re-calculate scaled crop window based if pre-scaling down
        //if(pDS_Info->CodedWidth > pDSIntData->MaxWidth)
        if(pDS_Info->CodedWidth > pDSXCData->stDispWin.width)
        {
            // pre-scaling down, need to re-calculate crop width
            print2(0xEE17, PDD,   pDS_Info->CodedWidth,  pDSIntData->MaxWidth);

            //stDSScaledCropWin.x = ((unsigned long) stDSCropWin.x * pDSIntData->MaxWidth / pDS_Info->CodedWidth) & ~0x1;
            //stDSScaledCropWin.width = ((unsigned long) stDSCropWin.width * pDSIntData->MaxWidth / pDS_Info->CodedWidth) & ~0x1;
            stDSScaledCropWin.x = ((unsigned long) stDSCropWin.x ) & ~0x1;
            stDSScaledCropWin.width = ((unsigned long) stDSCropWin.width ) & ~0x1;
        }
        else
        {
            stDSScaledCropWin.x = (stDSCropWin.x + 1) & ~0x1;
            stDSScaledCropWin.width = (stDSCropWin.width + 1) & ~0x1 ;
        }

        // re-calculate crop window based on input V capture change, V has no pre-scaling in DS case
        //if(pDS_Info->CodedHeight > pDSIntData->MaxHeight)
        if(pDS_Info->CodedHeight > pDSXCData->stDispWin.height)
        {
            // pre-scaling down, need to re-calculate crop width
            print2(0xEE20, PDD,   pDS_Info->CodedHeight,  pDSIntData->MaxHeight);

            //stDSScaledCropWin.y = ((unsigned long) stDSCropWin.y * pDSIntData->MaxHeight/ pDS_Info->CodedHeight) & ~0x1;
            //stDSScaledCropWin.height = ((unsigned long) stDSCropWin.height * pDSIntData->MaxHeight / pDS_Info->CodedHeight) & ~0x1;
            stDSScaledCropWin.y = stDSCropWin.y;//((unsigned long) stDSCropWin.y * pDSIntData->MaxHeight/ pDS_Info->CodedHeight) & ~0x1;
            stDSScaledCropWin.height = stDSCropWin.height;//((unsigned long) stDSCropWin.height * pDSIntData->MaxHeight / pDS_Info->CodedHeight) & ~0x1;
        }
        else
        {
            stDSScaledCropWin.y = (stDSCropWin.y + 1) & ~0x1;
            stDSScaledCropWin.height = (stDSCropWin.height + 1) & ~0x1;
        }

        //check height value
        //set V length
        //when do crop v length should set according the crop size
        //due to at the lower boundary filter will do 4 line scaling set v length to avoid capture garbge line.
        if(stDSScaledCropWin.y+stDSScaledCropWin.height>u16VSizeAfterPreScaling)
        {
            stDSScaledCropWin.height = u16VSizeAfterPreScaling - stDSScaledCropWin.y;
            ds_write_2bytes(pDSIntData, DS_OP, 0x20, 0x15, stDSScaledCropWin.height);
            print2(0xEE21, PDD,  stDSScaledCropWin.y,stDSScaledCropWin.height);

        }
        else
        {
            ds_write_2bytes(pDSIntData, DS_OP, 0x20, 0x15, u16VSizeAfterPreScaling);
        }
        //UTL_printf("ScaledCrop (%d,%d)\n", stDSScaledCropWin.x, stDSScaledCropWin.y);
        //UTL_printf("ScaledCrop (%d,%d)\n", stDSScaledCropWin.width, stDSScaledCropWin.height);
        print2(0xEE22, PDD,  stDSScaledCropWin.x, stDSScaledCropWin.y);
        print2(0xEE23, PDD,  stDSScaledCropWin.width, stDSScaledCropWin.height);


        if(pDSXCData->bInterlace)
        {
            u32PixelOffset = ((unsigned long)stDSScaledCropWin.y/2 * (unsigned long)pDSXCData->u16IPMOffset) + (unsigned long)stDSScaledCropWin.x;
        }
        else
        {
            u32PixelOffset = ((unsigned long)stDSScaledCropWin.y * (unsigned long)pDSXCData->u16IPMOffset) + (unsigned long)stDSScaledCropWin.x;
        }

        if(pDSXCData->bMirrorMode)
        {
            u32PixelOffset = u32PixelOffset - stDSScaledCropWin.x;
        }

        print1(0xEE24, PH,  u32PixelOffset);

        u16LineBufOffset = (unsigned short)(u32PixelOffset % pDSIntData->OffsetAlign);
        u32PixelOffset -= (unsigned long)u16LineBufOffset;
        u32PixelOffset *= (unsigned long)pDSXCData->bLinearMode ? 1:u8Framenumber;       // if not linear mode, need to multiply by 2

        print2(0xEE25, PHH,  u32PixelOffset,pDSXCData->u8BitPerPixel);

        if(u32PixelOffset != 0)
        {
            u32CropOffset =  u32PixelOffset * (pDSXCData->u8BitPerPixel/8);
        }
        else
        {
            u32CropOffset = 0;
        }

        u16OPMFetch = (stDSScaledCropWin.width + u16LineBufOffset + pDSIntData->MemAlign*2 -1) & ~(pDSIntData->MemAlign*2 -1);

        ds_printf("PixelOff %d (align %d, linear %d)\n", u32PixelOffset, pDSIntData->OffsetAlign, pDSXCData->bLinearMode);
        ds_printf("IPMOffset=%d, OPMFetch=%d\n\n", pDSXCData->u16IPMOffset, u16OPMFetch);
        ds_printf("PixelOffset=%d, LBOffset=%d\n", u32PixelOffset, u16LineBufOffset);
        print2(0xEE30, PDD,  pDSIntData->OffsetAlign, pDSXCData->bLinearMode);
        print2(0xEE31, PDD,  pDSXCData->u16IPMOffset, u16OPMFetch);
        print2(0xEE32, PDD,  u32PixelOffset, u16LineBufOffset);

        if(pDSXCData->bMirrorMode)
        {
            if(pDSXCData->bLinearMode)
            {
                u32CropOffset = (pDSIntData->MaxWidth - u16OPMFetch -
                ((stDSScaledCropWin.x +pDSIntData->MemAlign*2 -1)& ~(pDSIntData->MemAlign*2 -1)))*(pDSXCData->u8BitPerPixel/8) + u32CropOffset;
                print1(0xEE33, PD,  u32CropOffset);
                //u32CropOffset = (pDSIntData->MaxWidth - stDSScaledCropWin.width)*3 + u32CropOffset;
                u32CropOffset = u32CropOffset + (pDSIntData->MaxHeight-u16VSizeAfterPreScaling) *pDSIntData->MaxWidth*(pDSXCData->u8BitPerPixel/8);

            }
            else
            {
                u32CropOffset = (pDSIntData->MaxWidth - u16OPMFetch-
                    ((stDSScaledCropWin.x +pDSIntData->MemAlign*2 -1)& ~(pDSIntData->MemAlign*2 -1)))*(pDSXCData->u8BitPerPixel/8)*u8Framenumber + u32CropOffset ;
                ds_printf("u32CropOffset %x--\n",u32CropOffset);
                print1(0xEE34, PD,  u32CropOffset);
                u32CropOffset = u32CropOffset + ((pDSIntData->MaxHeight-u16VSizeAfterPreScaling)*u8Framenumber *pDSIntData->MaxWidth*(pDSXCData->u8BitPerPixel/8));// + 575
            }
            print1(0xEE35, PD,  u32CropOffset);
            ds_printf("u16OPMFetch= %d,%x\n",u16OPMFetch,u16OPMFetch);
            ds_printf("u16VSizeAfterPreScaling %d,%x\n",(0x23F-u16VSizeAfterPreScaling) *1920*3,(0x23F-u16VSizeAfterPreScaling) *1920*3);
            ds_printf("u32CropOffset %x--\n",u32CropOffset);
        }
        print2(0xEE36, PDD,  pDSIntData->MaxWidth,pDSIntData->MaxHeight);
        ds_printf("---pDSIntData->MaxWidth %d--\n",pDSIntData->MaxWidth);
        ds_printf("---pDSIntData->MaxHeight %d--\n",pDSIntData->MaxHeight);

        //pDSIntData->MaxWidth
        if(pDSXCData->bMirrorMode)
        {
            u16LineBufOffset = u16OPMFetch - stDSScaledCropWin.width;
            // write OP base
            ds_write_4bytes(pDSIntData, DS_OP, 0x12, 0x10, (pDSXCData->u32IPMBase0 + u32CropOffset)/pDSIntData->MemAlign);
            ds_write_4bytes(pDSIntData, DS_OP, 0x12, 0x12, (pDSXCData->u32IPMBase1 + u32CropOffset)/pDSIntData->MemAlign);

            if( (pDSIntData->enChipID != DS_MARIA10) && (pDSIntData->enChipID != DS_J2) &&
                (pDSIntData->enChipID != DS_A3)  && (pDSIntData->enChipID != DS_A5) &&
                (pDSIntData->enChipID != DS_A6)  &&(pDSIntData->enChipID != DS_AGATE))
            {
                ds_write_4bytes(pDSIntData, DS_OP, 0x12, 0x14, (pDSXCData->u32IPMBase2 + u32CropOffset)/pDSIntData->MemAlign);
            }
        }
        else
        {
            // write OP base
            ds_write_4bytes(pDSIntData, DS_OP, 0x12, 0x10, (pDSXCData->u32IPMBase0 + u32CropOffset)/pDSIntData->MemAlign);
            ds_write_4bytes(pDSIntData, DS_OP, 0x12, 0x12, (pDSXCData->u32IPMBase1 + u32CropOffset)/pDSIntData->MemAlign);

            if( (pDSIntData->enChipID != DS_MARIA10) && (pDSIntData->enChipID != DS_J2) &&
                (pDSIntData->enChipID != DS_A3)  && (pDSIntData->enChipID != DS_A5) &&
                (pDSIntData->enChipID != DS_A6)  &&(pDSIntData->enChipID != DS_AGATE))
            {
                ds_write_4bytes(pDSIntData, DS_OP, 0x12, 0x14, (pDSXCData->u32IPMBase2 + u32CropOffset)/pDSIntData->MemAlign);
            }
        }

        //write OP fetch & LBoffset
        ds_write_2bytes(pDSIntData, DS_OP, 0x12, 0x17, u16OPMFetch);
        ds_write_2bytes(pDSIntData, DS_OP, 0x20, 0x1D, u16LineBufOffset&0xFF);

        // post H/V scaling
        ds_write_4bytes(pDSIntData, DS_OP, 0x23, 0x07, 0x01000000 | H_PostScalingRatio(stDSScaledCropWin.width, pDSXCData->stDispWin.width));
        ds_write_4bytes(pDSIntData, DS_OP, 0x23, 0x09, 0x01000000 | V_PostScalingRatio(stDSScaledCropWin.height, pDSXCData->stDispWin.height));
        //ds_write_2bytes(pDSIntData, DS_OP, 0x12, 0x02, 0x07BB);

        #if 0
        // write OP display buffer offset
        ds_write_2bytes(pDSIntData, DS_OP, 0x20, 0x1C, u16LineBufOffset * pDSXCData->stDispWin.height / stScaledCropWin.width);
        #endif
        // flush memory
        ds_write_buf_flush(pDSIntData);
    }
    else
    {
        ERROR_CODE(1);
    }
    pDSXCData->bFWIsUpdating = 0;

}

void dynamic_scale_info_update(unsigned char *pDS_Update_Info, ds_internal_data_structure *pDS_Int_Data)
{
    static unsigned int i = 1;
    ds_xc_data_structure *pDSXCData = pDS_Int_Data->pDSXCData;

    if(pDS_Int_Data->bIsInitialized == 0)
    {
        ds_printf("ISR: DS not inited\n");

        while(1);

    }

    // copy info to DS buffer
    if(pDS_Update_Info != 0)
    {
        switch(*pDS_Update_Info)
        {
            case MM_DS_XC_CMD_UPDATE_ZOOM_INFO:
                //printf("ISR update zoom info\n");

                pDSXCData->u8XCInfoUpdateIdx += 1;

                // update new crop/disp win based on first CapWin
                // it's safe to copy NewCrop info here because when interrupt here, there is no change that HK doing DS buf calculation
                // (because when HK doing DS calculation, it will stop interrupt)
                //ds_memcpy(&pDSXCData->stCropWin, &pDSXCData->stNewCropWin, sizeof(MS_WINDOW_TYPE));
                //ds_memcpy(&pDSXCData->stDispWin, &pDSXCData->stNewDispWin, sizeof(MS_WINDOW_TYPE));
                pDSXCData->bFWGotNewSetting = 1;

                print2(0xDDBB, PHH, pDSXCData->bFWGotNewSetting, pDSXCData->u8XCInfoUpdateIdx);

                break;

            case MM_DS_XC_CMD_UPDATE_XC_INFO:
                //printf("ISR: update XC info\n");

                print2(0xDDAA, PHH,  pDSXCData->bFWGotXCInfo, pDSXCData->u8XCInfoUpdateIdx);

                pDSXCData->u8XCInfoUpdateIdx += 1;
                pDSXCData->bFWGotXCInfo = 1;
                break;

            default:
                break;
        }
    }
    else
    {
    }
}

unsigned char dynamic_scale_is_got_xc_info(ds_internal_data_structure *pDS_Int_Data)
{
    return pDS_Int_Data->pDSXCData->bFWGotXCInfo;
}

unsigned char dynamic_scale_get_xc_info_update_idx(ds_internal_data_structure *pDS_Int_Data)
{
    return pDS_Int_Data->pDSXCData->u8XCInfoUpdateIdx;
}

unsigned char dynamic_scale_is_xc_fw_get_new_setting(ds_internal_data_structure *pDS_Int_Data)
{
    return pDS_Int_Data->pDSXCData->bFWGotNewSetting;
}
#endif

