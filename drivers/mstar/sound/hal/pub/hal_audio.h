#ifndef __HAL_AUDIO_H__
#define __HAL_AUDIO_H__
#include "hal_audio_reg.h"

typedef unsigned char   U8;
typedef signed char     S8;
typedef unsigned short  U16;
typedef short           S16;
typedef unsigned int    U32;
typedef int             S32;
typedef unsigned char   BOOL;

#define TRUE 1
#define FALSE 0

#define WRITE_BYTE(_reg, _val)      (*((volatile U8*)(_reg)))  = (U8)(_val)
#define WRITE_WORD(_reg, _val)      (*((volatile U16*)(_reg))) = (U16)(_val)
#define READ_BYTE(_reg)             (*(volatile U8*)(_reg))
#define READ_WORD(_reg)             (*(volatile U16*)(_reg))



#define AUD_RIU_BASE_ADDR        0x1f000000 //fixed me
#define AUD_REG_BANK_0           0x311000
#define AUD_REG_BANK_1           0x311100
#define AUD_REG_BANK_2           0x311200
#define AUD_REG_BANK_3           0x311300
#define AUD_REG_BANK_4           0x311400
#define AUD_REG_BANK_5           0x311500


#define MIU_LINE_SIZE          (16) //bytes
#define DMA_LOCALBUF_SIZE      (64) //bytes
#define DMA_LOCALBUF_LINE      (DMA_LOCALBUF_SIZE/MIU_LINE_SIZE)

#if 1
#define AUD_INT_EN_MSK (AUD_INT_DMA_R1 | \
                      AUD_INT_DMA_W1 | \
                      AUD_INT_DMA_R2 | \
                      AUD_INT_DMA_W2 | \
                      AUD_INT_DMA_R3 | \
                      AUD_INT_DMA_W3 | \
                      AUD_INT_DMA_R4 | \
                      AUD_INT_DMA_W4 | \
                      AUD_INT_DMA_R5_HDMI | \
                      AUD_INT_DMA_W5_VREC )
#else
#define AUD_INT_EN_MSK AUD_INT_DMA_R1
#endif
#endif //__HAL_AUDIO_H__