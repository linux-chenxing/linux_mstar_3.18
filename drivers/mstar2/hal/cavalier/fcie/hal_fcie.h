////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2011 MStar Semiconductor, Inc.
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

#ifndef __DRVFCIE_H__
#define __DRVFCIE_H__



#include "hal_fcie_platform.h"
//#include "drvFCIE_config.h"

#define NONE			"\033[m"
#define RED				"\033[0;32;31m"
#define LIGHT_RED		"\033[1;31m"
#define GREEN			"\033[0;32;32m"
#define LIGHT_GREEN		"\033[1;32m"
#define BLUE			"\033[0;32;34m"
#define LIGHT_BLUE		"\033[1;34m"
#define DARY_GRAY		"\033[1;30m"
#define CYAN			"\033[0;36m"
#define LIGHT_CYAN		"\033[1;36m"
#define PURPLE			"\033[0;35m"
#define LIGHT_PURPLE	"\033[1;35m"
#define BROWN			"\033[0;33m"
#define YELLOW			"\033[1;33m"
#define LIGHT_GRAY		"\033[0;37m"
#define WHITE			"\033[1;37m"


/**************************************
* Function define
**************************************/
#define SD_TIMEOUT_VALUE       1000 // 1 second
#define SD_DMA_TIMEOUT_VALUE   3000 // 3 second
#define BYTES_8          0
#define BYTES_16         1
#define BYTES_32         2
#define MIU_BURST_LEN    BYTES_8

/**************************************
* Structure define
**************************************/
#define TEST_WRITECRC		0
#define	TEST_READSTOPCLOCK  0
#define	TEST_RESPONSESHIFT	0
#define	TEST_POWERSAVE		0
#define	TEST_CMDNORESPONSE	1
#define	TEST_RW_SBIT_TOUT	0
#define TEST_RESPONSECRCERR	0
#define TEST_READCRCERR		0
#define	TEST_WRITECRCERR	0
#define TEST_BIST			0
#define REGRESSION_TEST		1
#define TEST_DMA			0
#define TEST_R2N			0
#define	TEST_ADMA			0

#define DMA_MODE 0
#define ADMA_MODE 1
#define R2N_MODE  2

typedef union _CMD
{
    U8 Array[20];

	struct
	{
		U8 Dummy[3];
		U8 Index;
		U32 Arg;
		U8 Reserve[12];

	} __attribute__((__packed__)) Cmd;

} CMD;

typedef union _RSP
{
    U8 Array[20];

	struct
	{
		U8 Dummy[3];
		U8 Index;
		U32 DeviceStatus;
		U8 Reserve[12];

	} __attribute__((__packed__)) Rsp;

	U32 Resp[5];

} RSP;

typedef enum
{
    CLK_STOP	= 0,
    CLK_KEEP	= 1,

} E_CLK_STOP;

typedef enum
{
    CMD_TYPE_BC     = 0, // broadcast command
    CMD_TYPE_BCR    = 1, // broadcast command with response
    CMD_TYPE_AC     = 2, // addressed command
    CMD_TYPE_ADTC   = 3, // addressed data transfer command
    CMD_TYPE_NDTC   = 4, // no data transfer command

} E_CMD_TYPE;

typedef enum
{
    RSP_TYPE_NO  = 0,
    RSP_TYPE_R1  = 1,
    RSP_TYPE_R1b = 2,
    RSP_TYPE_R2  = 3,
    RSP_TYPE_R3  = 4,
    RSP_TYPE_R6  = 5,
    RSP_TYPE_R7  = 6, // CMD08 send if cond

} E_RSP_TYPE;

typedef enum
{
    DIR_R = 0,
    DIR_W = 1,

} E_RWDIR;

typedef enum
{
    PATH_R2N	= 0, // only GPIO_PAD & 8 bits macro can use R2N
    PATH_DMA	= 1,
    PATH_ADMA	= 2,

} E_PATH;

typedef enum
{
    BUS_1_BIT   = 0,
    BUS_4_BITS  = 1,
    BUS_8_BITS  = 2,

} E_DATA_BUS;

typedef struct _CMD_RSP_INFO
{
	U32				CardClock;
	CMD				Command;
	RSP				Response;

	E_CLK_STOP		ClockStop;
	E_CMD_TYPE		CmdType;
	E_RSP_TYPE		RspType;

	// add for data transfer
	E_RWDIR			ReadWriteDir;
	E_PATH			DataPath; // R2N or DMA
	E_DATA_BUS		BusWidth;

	U32				BusAddr;	// MIU0, MIU1, MIU2 base added address
	U32				MiuAddr;	// DMA & ADMA use only, this is a MIU address
	U32 *			pBufferAddr; // R2N use only, this is a data buffer pointer
	U16				BlockSize;
	U32				FakeDmaLength; // IP verify use only
	U16				BlockCount; // ADMA don't care
	U16				ReadWriteTimeOut;

} CMD_RSP_INFO;


struct kernel_like_scatterlist {

	U32	u32_dma_addr;
	U32	u32_length;
};

struct kernel_like_mmc_data {

	unsigned int		sg_len;		/* size of scatter list */
	struct kernel_like_scatterlist	*sg;		/* I/O scatter list */
};

///////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct _ADMA_DESCRIPTOR
{
	U32 adma_end_flag   :1;
	U32 adma_miu_sel    :2;
	U32                 :13;    // reserve
	U32 adma_job_cnt    :16;
	U32 adma_miu_addr;
	U32 adma_length;
	U32 reserved;

} ADMA_DESCRIPTOR ;

#define SG_SEG_NUM 512

typedef struct _ADMA_INFO
{
    ADMA_DESCRIPTOR Descriptor[SG_SEG_NUM];
    //U32 const DescriptorAddr;
    U32 DescriptorBusAddr;

} ADMA_INFO ;

typedef enum
{
    EC_NO_ERROR = 0,
    EC_CRC_ERROR,
    EC_CMD_TIME_OUT,
    EC_DMA_TIME_OUT,
    EC_CMD_NO_RSP,

} ERR_CODE_SD;

typedef enum
{
    IO_SUCCESS              = 0x00,
    IO_TIME_OUT             = 0x01,
    IO_CARD_REMOVED         = 0x02,
    IO_ERROR                = 0x03,
    IO_ERROR_DETECT         = 0x04,

    IO_VOLTAGE_NOT_ACCEPT   = 0x05,
    IO_MISS_CHECK_PATTERN   = 0x06,

    IO_CMD_NO_RSP           = 0x16,
    IO_RSP_CRC_ERROR        = 0x17,
    IO_RSP_ERROR            = 0x18,

    IO_R_DATA_CRC_ERROR     = 0x20,
    IO_W_DATA_STS_ERROR     = 0x21,
    IO_W_DATA_STS_NEGATIVE  = 0x22,

    IO_STILL_POWERING_UP    = 0x30,

    IO_CARD_CHANGED         = 0x40,

} E_IO_STS;


extern U8 gu8FcieSilenceTuning;
extern U32 gu32BestFcieSkew4;

void		HalFcie_Init(void);
void		HalFcie_DumpRegister(void);
void		HalFcie_OpenClock(U8 On_off);
U8			HalFcie_CmdFifoGet(U8 u8addr);
void		HalFcie_CmdFifoSet(U8 u8addr, U8 u8value);
E_IO_STS	HalFcie_WaitGetCIFD(U32 * pu32_DataBuf, U32 u32_ByteCnt);
E_IO_STS	HalFcie_WaitSetCIFD(U32 * pu32_DataBuf, U32 u32_ByteCnt);
E_IO_STS	HalFcie_WaitD0High(U32);
E_IO_STS	HalFcie_SendCommand(CMD_RSP_INFO * pCmdRspInfo);
E_IO_STS	HalFcie_SetBusWidth(U8 * pu8BusWidth, E_DATA_BUS DataBus);
E_IO_STS	HalFcie_WaitMieEvent(U16 u16ReqVal, U32 u32WaitMs);
U32			HalFcie_WaitCIFD_Event(U16 u16_WaitEvent, U32  u32_MicroSec);
void		HalFcie_SetVDD(U8 u8OnOff);
U8			HalFcie_SendClocknWaitD0High(void);
U8			HalFcie_TestAdvanceDMA(U8 u8Pattern);
U32			HalFcie_PowerSaveMode(void);
U32 		HalFcie_BistTest(U8 u8_PatSel);
U16			HalFcie_ReadCRCFifo(U32 i);
void		HalFcie_SetShiftTune(U8 u8Select, U8 u8Type, U8 u8Count);
void		HalFcie_TestDataOneIntr(void);
E_IO_STS	HalFcie_DetectBusyEnd(U32 timeout);
//U32			HalFcie_TranslateVirtAddr(U32 u32_DMAAddr, U32 u32_ByteCnt);
void		HalFcie_SetSkew(U8 u8SkewIdx, U32 u32SkewValue);
void		HalFcie_DumpRegisterBank(U16 u16BankNo, U16 u16_16BitAddrOffset, U16 u16WordCount);
void		HalFcie_TimerTest(void);
void		HalFcie_TimerStart(void);
U32			HalFcie_TimerStop(void);
irqreturn_t HalFcie_KernelIrq(int irq, void *devid);
void		HalFcie_ClearWaitQueue(void);
U32			HalFcie_PrepareDescriptors(struct mmc_data *data);
void		HalFcie_SetupDescriptorAddr(CMD_RSP_INFO * pCmdRspInfo);


#ifdef SDIO_SUPPORT_SD30
void		HalFcie_SetDqs(U32 u32DqsValue);
void		HalFcie_SetIOVoltage(U8 u8Volt);
#endif

void		fcie_dump_mem(unsigned char *buf, U32 cnt);
void		sd_bist_test_n_check_CRC_value(void);
void		sd_r2n_read_write_test(void);
void		sd_adma_read_write_test(void);
void		sd_rw_timeout_error_detect(U8 u8ReadWriteDir);


#endif
