#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <linux/sockios.h>
#include <sys/file.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/uio.h>
#include <sys/poll.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>

#include <pthread.h>
#include <netinet/ip.h>
#include <linux/icmp.h>
//#include <linux/delay.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <linux/netlink.h>
#include <linux/fs.h>
#include <sys/mman.h>

//#ifdef MSOS_TYPE_LINUX_KERNEL
//#include <asm/div64.h>
//#else
//    #define do_div(x,y) ((x)/=(y))
//#endif

//#include <private/android_filesystem_config.h>
#include "mdrv_msys_io.h"
#include "mdrv_msys_io_st.h"
#include "mdrv_verchk.h"

//#include "apiXC.h"
#include "mhal_divp_datatype.h"
#include "mhal_divp.h"
#include "dip_test.h"
#include "mdrv_dip_io.h"
#include "mdrv_dip_io_st.h"

//---------------------------------------------------------------
// enum
//---------------------------------------------------------------
typedef enum
{
    E_DRV_ID_DIP = 0,
    E_DRV_ID_MSYS,
    E_DRV_ID_MEM,
    E_DRV_ID_NUM,
} EN_DRV_ID_TYPE;


static int g_FD[E_DRV_ID_NUM] = {-1, -1, -1};

typedef struct __Param
{
    MS_BOOL bHMirror;
    MS_BOOL bVMirror;
    MHAL_DIVP_PixelFormat_e eInputDataFmt;
    MS_U16 u16SrcWidth;
    MS_U16 u16SrcHeight;
    MS_U16 u16CapX;
    MS_U16 u16CapY;
    MS_U16 u16CapWidth;
    MS_U16 u16CapHeight;
    MS_U16 u16DstWidth;
    MS_U16 u16DstHeight;
    MHAL_DIVP_PixelFormat_e eOutputFormat;
    MHAL_DIVP_ScanMode_e enScan;
    MHAL_DIVP_TileMode_e eTileBlock;
    char	Infilename[128];
    char	Outfilename[128];
} Param;


// DIP test properties
void* pDIPInstance = NULL;
void* pCtx = NULL;
char cInputFileName[128] = "/system/bin/DIP_640x360_YUV422.raw";
char cOutputFileName[128] = "/data/DIP_output.raw";

static MS_U32 u32ChanId = 0;
static DIP_COVER_PROPERTY gstCoverProperty = {0};

ST_DIP_Test_Property stDIPTestProperty =
{
    FALSE,        // Snapshot or not
    DIP_WINDOW, // eDIPWindow
    FALSE,      // bHMirror
    FALSE,      // bVMirror
    E_MHAL_DIVP_PIXEL_FORMAT_YC420_MSTTILE1_H264,            // eInputDataFmt
    1920,       // u16SrcWidth
    1088,       // u16SrcHeight
    1920,       // u16SrcPitch
    0,          // u16CapX
    0,          // u16CapY
    1920,       // u16CapWidth
    1088,       // u16CapHeight
    1,          // u8BufCnt
    1920,       // u16WinLineOffSet
    0,          // u64WinOffSet
    1920,       // u16DstWidth
    1088,       // u16DstHeight
    FALSE,      // bPIP
    FALSE,      // b2P_Enable
    SCALER_DIP_SOURCE_TYPE_DRAM,    // eSource
#if 0
    //E_MHAL_DIVP_PIXEL_FORMAT_YUV422_YUYV,      // eOutputFormat
    //E_MHAL_DIVP_PIXEL_FORMAT_YUV_SEMIPLANAR_420,      // eOutputFormat
    E_MHAL_DIVP_PIXEL_FORMAT_YC420_MSTTILE1_H264,      // eOutputFormat
#else
    E_MHAL_DIVP_PIXEL_FORMAT_ARGB8888,      // eOutputFormat
#endif
    E_MHAL_DIVP_SCAN_MODE_PROGRESSIVE,  // enScan
    E_MHAL_DIVP_TILE_MODE_32x32,          // eTileBlock
    E_MHAL_DIVP_TNR_LEVEL_OFF,  // u8TNRType
    E_MHAL_DIVP_DI_TYPE_OFF,    // u83DDIType
    E_MHAL_DIVP_ROTATE_NONE,    // u8Rotation
    {
        FALSE,  //bDbEnable
        0,  //u8DbSelect
        {
            FALSE,  //bHMirror
            FALSE,  //bVMirror
        },
        FALSE,  //bUncompressMode
        FALSE,  //bBypassCodecMode
        E_MHAL_DIVP_DB_MODE_H264_H265,  //eDbMode
        0,  //u16StartX
        0,  //u16StartY
        1920,  //u16Width
        1088,  //u16Height
        0,  //u64DbBaseAddr
        1920,  //u16DbPitch
        0,  //u8DbMiuSel
        0,  //u64LbAddr
        0,  //u8LbSize
        0,  //u8LbTableId
    },
    // SW crop
    {
        0,  // bEnable
        0,  // u16X
        0,  // u16Y
        0,  // u16Width
        0,	// u16Height
    }
};

//----------------------------read file------------------------------------------------
void _ReadFile(void **pBinbuf, long *pFileSize, char *pFileName)
{
    FILE *pFile = NULL;
    long lFileSize = 0;
    long lFileRsize;
    char *pu8Buffer = NULL;

    //printf("%s \n", pFileName);
    pFile = fopen(pFileName, "rb");
    if(pFile == NULL)
    {
        printf("Open File Fail %s !! \n", pFileName);
    }
    else
    {
        long i;
        char *buf;
        //printf("Open File Success \n!!");

        fseek(pFile, 0, SEEK_END);//set seek start point
        lFileSize = ftell(pFile);//return position
        printf("Open File size:%d \n!!", (int)lFileSize);
        //pu8Buffer = (char*) malloc(lFileSize);
        printf("*pBinbuf=0x%x\n!!", *pBinbuf);
        rewind(pFile);//back to file start point

        if(*pBinbuf)
        {
            lFileRsize = fread(*pBinbuf, 1, lFileSize, pFile);
            if(lFileRsize != lFileSize)
            {
                printf("Size is not correct %d \n", (int)lFileRsize);
            }
        }

        fclose(pFile);

        *pFileSize = lFileSize;

        //*pBinbuf = pu8Buffer;

    }
}
//----------------------------save file------------------------------------------------
FILE* _OpenFile(char *pFileName)
{
    FILE *pFile = NULL;
    pFile = fopen(pFileName, "wb");
    if(pFile == NULL)
    {
        printf("Open File Fail %s !! \n", pFileName);
    }
    else
    {
        printf("Open File Success %s !! \n", pFileName);
        fseek(pFile, 0, SEEK_END);
    }
    return pFile;
}

unsigned char* _MMAP_mem(FILE *pFile, MSYS_DMEM_INFO meminfo)
{
    unsigned char *u64Vir_data;
    u64Vir_data   = mmap(NULL, meminfo.length, PROT_READ | PROT_WRITE, MAP_SHARED, g_FD[E_DRV_ID_MEM], (meminfo.phys));
    return u64Vir_data;
}

void _CloseFile(FILE *pFile, unsigned char *u64Vir_data, unsigned long long buffersize)
{
    munmap(u64Vir_data, buffersize);
    fclose(pFile);

}

void Buffer_clear(unsigned char *u64Vir_data, unsigned long long buffersize)
{
    memset(u64Vir_data, 0, buffersize);
}

unsigned long long _GetSystemTimeStamp (void)
{
    struct timeval         tv;
    struct timezone tz;
    unsigned long long u64TimeStamp;
    gettimeofday(&tv, &tz);
    u64TimeStamp = tv.tv_sec * 1000000ULL + tv.tv_usec;
    return u64TimeStamp;
}


int Argb8888ToBmpFile(int *pBmpVaRawData, int u16BmpW, int u16BmpH, char *szFileName)
{
    int bSuccess = 0;
    int u32Width = u16BmpW;
    int u32Height = u16BmpH;
    int u32Pitch = u16BmpW * 4;
    int fd = -1;

    unsigned char BMP_HEADER[54] =
    {
        0x42, 0x4d,     // signature, must be 4D42 hex
        0x36, 0x40, 0x38, 0x00,   // size of BMP file in bytes 1280x720x4 + 54(header)
        0x00, 0x00, 0x00, 0x00, // reserved, must be zero
        0x36, 0x00, 0x00, 0x00,    // offset to start of image data in bytes
        0x28, 0x00, 0x00, 0x00,    // size of BITMAPINFOHEADER structure, must be 40
        0x00, 0x05, 0x00, 0x00,    // image width in pixels
        0xd0, 0x02, 0x00, 0x00, // image height in pixels
        0x01, 0x00,                // number of planes in the image, must be 1
        0x20, 0x00,              // number of bits per pixel (1, 4, 8, or 24, 32)
        0x00, 0x00, 0x00, 0x00, // compression type (0=none, 1=RLE-8, 2=RLE-4)
        0x00, 0x40, 0x38, 0x00,   // size of image data in bytes (including padding)
        0x00, 0x00, 0x00, 0x00,   // horizontal resolution in pixels per meter
        0x00, 0x00, 0x00, 0x00, // vertical resolution in pixels per meter
        0x00, 0x00, 0x00, 0x00,   // number of colors in image, or zero
        0x00, 0x00, 0x00, 0x00,    // number of important colors, or zero
    };

    do
    {
        // fill BMP header , size of BMP file in bytes
        int u32BmpFileSize = u32Pitch * u32Height + 54;
        BMP_HEADER[2] = u32BmpFileSize & 0xFF;
        BMP_HEADER[3] = (u32BmpFileSize & 0xFF00) >> 8;
        BMP_HEADER[4] = (u32BmpFileSize & 0xFF0000) >> 16;
        BMP_HEADER[5] = (u32BmpFileSize & 0xFF000000) >> 24;    // image width in pixel
        BMP_HEADER[18] = u32Width & 0xFF;
        BMP_HEADER[19] = (u32Width & 0xFF00) >> 8;    // image height in pixel
        BMP_HEADER[22] = u32Height & 0xFF;
        BMP_HEADER[23] = (u32Height & 0xFF00) >> 8;    // size of image data in bytes (including padding)
        u32BmpFileSize -= 54;
        BMP_HEADER[34] = u32BmpFileSize & 0xFF;
        BMP_HEADER[35] = (u32BmpFileSize & 0xFF00) >> 8;
        BMP_HEADER[36] = (u32BmpFileSize & 0xFF0000) >> 16;
        BMP_HEADER[37] = (u32BmpFileSize & 0xFF000000) >> 24;

        fd = open(szFileName, O_RDWR | O_CREAT, 0644);
        if (fd < 0)
        {
            FUNC_ERR(" open \"%s\" failed \n", szFileName);
            break;
        }

        //write bmp header
        write(fd, (void *)BMP_HEADER, sizeof(BMP_HEADER));

        int i = 0;
        unsigned char *p = (unsigned char *)pBmpVaRawData;
        p += (u32Pitch * (u32Height - 1));
        for(i = 0; i < u32Height; i++)
        {
            // write p, pitch
            write(fd, (void *)p, u32Pitch);
            p -= u32Pitch;
        }
        sync();
        close(fd);

        FUNC_MSG("dumpbmp file \"%s\" done !!!\n", szFileName);
        bSuccess = 1;
    }
    while (0);


    return bSuccess;
}

int YUVRawToFile(MS_U8 *pYUVRawData, char *szFileName, int FileSize)
{
    int bSuccess = 0;
    FILE *pFile = NULL;
    unsigned char *p;
    long lFileWsize;

    do
    {
        pFile = fopen(szFileName, "wb");
        if(pFile == NULL)
        {
            FUNC_ERR(" open \"%s\" failed \n", szFileName);
        }
        else
        {
            //FUNC_MSG(" open \"%s\" success \n", szFileName);
            fseek(pFile, 0, SEEK_END);
        }
        p = (unsigned char *)pYUVRawData;
        lFileWsize = fwrite(p, sizeof(unsigned char), FileSize, pFile);

        fclose(pFile);

        //FUNC_MSG("dumpYUV raw file \"%s\" done !!!\n", szFileName);
        bSuccess = 1;
    }
    while (0);

    return bSuccess;
}

static void Int2Str(MS_U16 value, char *string)
{
    MS_U32   i, j;
    MS_U8    temp[3];

    for (i = 0; i < 3; i++)
    {
        j = value / 10;
        if ( j != 0 )
        {
            temp[i] = '0' + (value - j * 10); //tbd value % 10;

            value = j;
        }
        else
        {
            temp[i] = '0' + (value - j * 10); //tbd value % 10;
            break;
        }
    }

    for(j = 0; j < (i + 1); j++)
    {
        string[j] = temp[i - j];
    }

    string[j] = 0;
}


int Open_Device(EN_DRV_ID_TYPE enDrvID)
{
    int ret = 1;
    char device_name[E_DRV_ID_NUM][50] =
    {
        {"/dev/mdip"},
        {"/dev/msys"},
        {"/dev/mem"},
    };

    //FUNC_MSG("Open_Device: id=%d\n", enDrvID);
    if(enDrvID >= E_DRV_ID_NUM)
    {
        printf("ID is not correct\n");
        return 0;
    }

    //FUNC_MSG("Open_Device handle: %d\n", g_FD[enDrvID]);
    if(g_FD[enDrvID] != -1)
    {
    }
    else
    {
        //FUNC_MSG("Open_Device: %s\n", &device_name[enDrvID][0]);
        //printf("Open: %s\n", &device_name[enDrvID][0]);
        g_FD[enDrvID] = open(&device_name[enDrvID][0], O_RDWR | O_SYNC, S_IRUSR | S_IWUSR);

        if(g_FD[enDrvID] == -1)
        {
            printf("open %s fail\n", &device_name[enDrvID][0]);
            ret = 0;
        }
        else
        {
            printf("open %s  %d sucess\n", &device_name[enDrvID][0], g_FD[enDrvID]);
            ret = 1;
        }
    }
    return ret;
}


#if 1
MS_BOOL _SysInit(void)
{
    MS_BOOL ret = TRUE;

    //FUNC_MSG("System Init Finish\n");

    return ret;
}

MS_BOOL _SysDeInit(void)
{
    MS_BOOL ret = TRUE;

    return ret;
}
#endif

void Test_DIVP_SetAttr_TNR(MS_U32 nCtxid, MHAL_DIVP_TnrLevel_e *stCfg)
{
    IOCTL_DIVP_TnrLevel_t stInCfg;

    if(Open_Device(E_DRV_ID_DIP) == 0)
    {
        return ;
    }

    stInCfg.Ctxid = nCtxid;
    stInCfg.eTnrLevel = *stCfg;

    ioctl(g_FD[E_DRV_ID_DIP], IOCTL_DIVP_SET_ATTR_TNR, &stInCfg);
}

void Test_DIVP_SetAttr_DITYPE(MS_U32 nCtxid, MHAL_DIVP_DiType_e *stCfg)
{
    IOCTL_DIVP_DiType_t stInCfg;

    if(Open_Device(E_DRV_ID_DIP) == 0)
    {
        return ;
    }

    stInCfg.Ctxid = nCtxid;
    stInCfg.eDiType = *stCfg;

    ioctl(g_FD[E_DRV_ID_DIP], IOCTL_DIVP_SET_ATTR_DITYPE, &stInCfg);
}

void Test_DIVP_SetAttr_ROTATE(MS_U32 nCtxid, MHAL_DIVP_Rotate_e *stCfg)
{
    IOCTL_DIVP_Rotate_t stInCfg;

    if(Open_Device(E_DRV_ID_DIP) == 0)
    {
        return ;
    }

    stInCfg.Ctxid = nCtxid;
    stInCfg.eRotateType = *stCfg;

    ioctl(g_FD[E_DRV_ID_DIP], IOCTL_DIVP_SET_ATTR_ROTATE, &stInCfg);
}

void Test_DIVP_SetAttr_WINDOW(MS_U32 nCtxid, MHAL_DIVP_Window_t *stCfg)
{
    IOCTL_DIVP_Window_t stInCfg;

    if(Open_Device(E_DRV_ID_DIP) == 0)
    {
        return ;
    }

    stInCfg.Ctxid = nCtxid;
    stInCfg.stCropWin.u16X = stCfg->u16X;
    stInCfg.stCropWin.u16Y = stCfg->u16Y;
    stInCfg.stCropWin.u16Width = stCfg->u16Width;
    stInCfg.stCropWin.u16Height = stCfg->u16Height;

    ioctl(g_FD[E_DRV_ID_DIP], IOCTL_DIVP_SET_ATTR_WINDOW, &stInCfg);
}

void Test_DIVP_SetAttr_MIRROR(MS_U32 nCtxid, MHAL_DIVP_Mirror_t *stCfg)
{
    IOCTL_DIVP_Mirror_t stInCfg;

    if(Open_Device(E_DRV_ID_DIP) == 0)
    {
        return ;
    }

    stInCfg.Ctxid = nCtxid;
    stInCfg.stMirror.bHMirror = stCfg->bHMirror;
    stInCfg.stMirror.bVMirror = stCfg->bVMirror;

    ioctl(g_FD[E_DRV_ID_DIP], IOCTL_DIVP_SET_ATTR_MIRROR, &stInCfg);
}

void Test_DIVP_SetCOVER(MS_U32 nCtxid, DIP_COVER_PROPERTY *stCfg)
{
    IOCTL_DIVP_Cover_t stInCfg;

    if(Open_Device(E_DRV_ID_DIP) == 0)
    {
        return ;
    }

    stInCfg.Ctxid = nCtxid;
    memcpy(&stInCfg.stCoverProperty, stCfg, sizeof(DIP_COVER_PROPERTY));

    ioctl(g_FD[E_DRV_ID_DIP], IOCTL_DIVP_SET_COVER, &stInCfg);
}


void Test_DIVP_ProcessDramData(MS_U32 nCtxid, MHAL_DIVP_InputInfo_t *stInCfg, MHAL_DIVP_OutPutInfo_t *stOutCfg)
{
    IOCTL_DIVP_ProcessDramInfo_t stCfg;

    if(Open_Device(E_DRV_ID_DIP) == 0)
    {
        return ;
    }

    stCfg.Ctxid = nCtxid;
    memcpy(&stCfg.stInputInfo, stInCfg, sizeof(MHAL_DIVP_InputInfo_t));
    memcpy(&stCfg.stOutputInfo, stOutCfg, sizeof(MHAL_DIVP_OutPutInfo_t));

    ioctl(g_FD[E_DRV_ID_DIP], IOCTL_DIVP_PROCESS_DRAM_DATA, &stCfg);
}

void Test_DIVP_CaptureTiming(MS_U32 nCtxid, MHAL_DIVP_CaptureInfo_t *stCfg)
{
    IOCTL_DIVP_CaptureInfo_t stInCfg;

    if(Open_Device(E_DRV_ID_DIP) == 0)
    {
        return ;
    }

    stInCfg.Ctxid = nCtxid;
    memcpy(&stInCfg.stCaptureInfo, stCfg, sizeof(MHAL_DIVP_CaptureInfo_t));

    ioctl(g_FD[E_DRV_ID_DIP], IOCTL_DIVP_CAPTURE_TIMING, &stInCfg);
}

MS_S32 fnAlloc(MS_U8 *pnMMAHeapName, MS_U32 nSize, MS_U64 *pnAddr)
{
    //kmalloc((nSize), GFP_KERNEL);
    return TRUE;
}

MS_S32 fnFree(MS_U64 nAddr)
{
    //kfree((MS_U8 *)nAddr);
    return TRUE;
}

MS_BOOL _DIPInit(void)
{
    MS_BOOL ret = TRUE;
    MHAL_DIVP_DeviceId_e eId = 0;
    IOCTL_DIVP_Instance_t stInstCfg;

    //DIPOpen(&pDIPInstance, NULL);
    //FUNC_MSG("pDIPInstance = 0x%x\n", (int)pDIPInstance);

    if(Open_Device(E_DRV_ID_DIP) == 0)
    {
        return FALSE;
    }

    //FUNC_MSG("System Init Finish\n");

#if 1  // test HAL API
    //HalDivpInit(0);
    eId = 0;
    ioctl(g_FD[E_DRV_ID_DIP], IOCTL_DIVP_INIT_CONFIG, &eId);

    //HalDivpCreateInstance(0, 1920, 1088, NULL, NULL, &pCtx);
    stInstCfg.eId = 0;
    stInstCfg.u16MaxWidth = 1920;
    stInstCfg.u16MaxHeight = 1088;
    stInstCfg.Ctxid = u32ChanId;
    ioctl(g_FD[E_DRV_ID_DIP], IOCTL_DIVP_CREATE_INST, &stInstCfg);

    //FUNC_MSG("pCtxId = %d\n", (int)stInstCfg.Ctxid);
#else
    HAL_XC_DIP_Init(pDIPInstance, stDIPTestProperty.eDIPWindow);
    HAL_XC_DIP_SetFRC(pDIPInstance, FALSE, 1, 1, stDIPTestProperty.eDIPWindow);
    HAL_XC_DIP_ClearIntr(pDIPInstance, 0, stDIPTestProperty.eDIPWindow);
#endif

    //FUNC_MSG("DIP Init Finish\n");

    return ret;
}

MS_BOOL _DIPDeInit(void)
{
    MS_BOOL ret = TRUE;
    MHAL_DIVP_DeviceId_e eId = 0;
    IOCTL_DIVP_Instance_t stInstCfg;

    if(Open_Device(E_DRV_ID_DIP) == 0)
    {
        return FALSE;
    }

#if 1  // test HAL API
    //HalDivpDestroyInstance(pCtx);
    stInstCfg.eId = 0;
    stInstCfg.Ctxid = u32ChanId;
    ioctl(g_FD[E_DRV_ID_DIP], IOCTL_DIVP_DESTROY_INST, &stInstCfg);
    FUNC_MSG("pCtxId = %d\n", (int)stInstCfg.Ctxid);

    //HalDivpDeInit(0);
    eId = 0;
    ioctl(g_FD[E_DRV_ID_DIP], IOCTL_DIVP_DEINIT_CONFIG, &eId);

#endif

    //if (pDIPInstance)
    //    DIPClose(pDIPInstance);

    //FUNC_MSG("DIP DeInit Finish\n");

    return ret;
}

MSYS_DMEM_INFO _AllocateDmem(const char *name, unsigned short u16BufNumber, unsigned long buffsize)
{
    MSYS_DMEM_INFO stMsysMemInfo;
    int ret = 0;

    memset(&stMsysMemInfo, 0, sizeof(MSYS_DMEM_INFO));
    strcpy(stMsysMemInfo.name, name);
#if 1
    stMsysMemInfo = FILL_VERCHK_TYPE(stMsysMemInfo, stMsysMemInfo.VerChk_Version,
                                     stMsysMemInfo.VerChk_Size, IOCTL_MSYS_VERSION);
    ioctl(g_FD[E_DRV_ID_MSYS], IOCTL_MSYS_RELEASE_DMEM, &stMsysMemInfo);
#endif

    stMsysMemInfo.length =  buffsize * u16BufNumber * 1; // Y C buffer
    stMsysMemInfo = FILL_VERCHK_TYPE(stMsysMemInfo, stMsysMemInfo.VerChk_Version, stMsysMemInfo.VerChk_Size, IOCTL_MSYS_VERSION);

    //FUNC_MSG("g_FD[%d] = %d\n", E_DRV_ID_MSYS, g_FD[E_DRV_ID_MSYS]);
    ret = ioctl(g_FD[E_DRV_ID_MSYS], IOCTL_MSYS_REQUEST_DMEM, &stMsysMemInfo);

    printf(ASCII_COLOR_GREEN);
    // [0][AllocateDmem]Phy:0x2bad0000,Vir:0xcbad0000
    FUNC_MSG("[%d][AllocateDmem]Phy:0x%llx,Vir:0x%llx\n", ret, (stMsysMemInfo.phys), stMsysMemInfo.kvirt);
    printf(ASCII_COLOR_END);

    return stMsysMemInfo;
}

void _FreeDmem(const char *name)
{
    MSYS_DMEM_INFO stMsysMemInfo;
    memset(&stMsysMemInfo, 0, sizeof(MSYS_DMEM_INFO));
    strcpy(stMsysMemInfo.name, name);
    stMsysMemInfo = FILL_VERCHK_TYPE(stMsysMemInfo, stMsysMemInfo.VerChk_Version, stMsysMemInfo.VerChk_Size, IOCTL_MSYS_VERSION);

    FUNC_MSG("[FreeDmem]Phy:0x%llx,Vir:0x%llx\n", (stMsysMemInfo.phys), stMsysMemInfo.kvirt);
    ioctl(g_FD[E_DRV_ID_MSYS], IOCTL_MSYS_RELEASE_DMEM, &stMsysMemInfo);

    return;
}

void _CacheMemoryFlush(void)
{
    MSYS_DUMMY_INFO stMsysInfo;
    memset(&stMsysInfo, 0, sizeof(MSYS_DUMMY_INFO));
    //    stMsysInfo = FILL_VERCHK_TYPE(stMsysInfo, stMsysInfo.VerChk_Version,
    //                                  stMsysInfo.VerChk_Size, IOCTL_MSYS_VERSION);
    //    ioctl(g_FD[E_DRV_ID_MSYS], IOCTL_MSYS_FLUSH_CACHE, &stMsysInfo);

    stMsysInfo = FILL_VERCHK_TYPE(stMsysInfo, stMsysInfo.VerChk_Version,
                                  stMsysInfo.VerChk_Size, IOCTL_MSYS_VERSION);
    ioctl(g_FD[E_DRV_ID_MSYS], IOCTL_MSYS_FLUSH_MEMORY, &stMsysInfo);
}

MSYS_ADDR_TRANSLATION_INFO _TranslationPhyToMIU(MSYS_DMEM_INFO stMsysMemInfo)
{
    MSYS_ADDR_TRANSLATION_INFO           u64MIU_Addr;
    int ret;

    u64MIU_Addr.addr = stMsysMemInfo.phys;
    u64MIU_Addr = FILL_VERCHK_TYPE(u64MIU_Addr, u64MIU_Addr.VerChk_Version,
                                   u64MIU_Addr.VerChk_Size, IOCTL_MSYS_VERSION);

    ret = ioctl(g_FD[E_DRV_ID_MSYS], IOCTL_MSYS_PHYS_TO_MIU, &u64MIU_Addr);

    //FUNC_MSG("[FPGA]MIU ADDR=0x%llx\n", u64MIU_Addr.addr);
    //FUNC_MSG("[FPGA]stMsysMemInfo.phys=0x%llx\n", stMsysMemInfo.phys);
    //FUNC_MSG("[FPGA]stMsysMemInfo.kvirt=0x%llx\n", stMsysMemInfo.kvirt);

    return u64MIU_Addr;
}

MS_BOOL _InputBufferLoad(MHAL_DIVP_PixelFormat_e eFmt, MS_U16 u16Width, MS_U16 u16Height, DIVP_Buffer_Config_t *pstInBuf)
{
    unsigned long  buffersize;
    IOCTL_DIVP_Buffer_Config_t stBufCfg;
    MSYS_DMEM_INFO stMsysMemInfo;
    MSYS_ADDR_TRANSLATION_INFO           u64MIU_Addr;
    unsigned long long  u64Vir_Addr;
    void *u64Vir_data[3];
    long lFileSize;

    if(Open_Device(E_DRV_ID_DIP) == 0)
    {
        return FALSE;
    }

    if(Open_Device(E_DRV_ID_MSYS) == 0)
    {
        return FALSE;
    }

    if(Open_Device(E_DRV_ID_MEM) == 0)
    {
        return FALSE;
    }

    memset(&stMsysMemInfo, 0, sizeof(MSYS_DMEM_INFO));
    if(pstInBuf->u8FrameCnt > 1)  // for 3DDI test
    {
        if((eFmt == E_MHAL_DIVP_PIXEL_FORMAT_YC420_MSTTILE1_H264) || \
                (eFmt == E_MHAL_DIVP_PIXEL_FORMAT_YC420_MSTTILE2_H265) || \
                (eFmt == E_MHAL_DIVP_PIXEL_FORMAT_YUV_SEMIPLANAR_420))
        {
            pstInBuf->u32BufOffset = (u16Width * u16Height * 3) / 2;
        }
        else if(eFmt == E_MHAL_DIVP_PIXEL_FORMAT_YUV422_YUYV)
        {
            pstInBuf->u32BufOffset = u16Width * u16Height * 2;
        }
        buffersize = pstInBuf->u32BufOffset * pstInBuf->u8FrameCnt;
    }
    else
    {
        buffersize = u16Width * u16Height * 2; // 2bpp
    }
    stMsysMemInfo = _AllocateDmem("DIVP_IN_BUF", 1, buffersize);
    if(stMsysMemInfo.kvirt == 0)
    {
        printf("%s:++++++++++++++++ allocate memory fail ++++++++++++++++\n", __FUNCTION__);
        return FALSE;
    }
    u64MIU_Addr = _TranslationPhyToMIU(stMsysMemInfo);
    stBufCfg.u32BufSize = buffersize;

    stBufCfg.u16Width = u16Width;
    stBufCfg.u16Height = u16Height;
    stBufCfg.eFmt = eFmt;

    u64Vir_Addr    = stMsysMemInfo.kvirt;

    if((eFmt == E_MHAL_DIVP_PIXEL_FORMAT_YC420_MSTTILE1_H264) || \
            (eFmt == E_MHAL_DIVP_PIXEL_FORMAT_YC420_MSTTILE2_H265) || \
            (eFmt == E_MHAL_DIVP_PIXEL_FORMAT_YUV_SEMIPLANAR_420))
    {
        stBufCfg.u64BufAddr[0] = u64MIU_Addr.addr;
        stBufCfg.u64BufAddr[1] = u64MIU_Addr.addr + (stBufCfg.u16Width * stBufCfg.u16Height);
    }
    else  // YUV422
    {
        stBufCfg.u64BufAddr[0] = u64MIU_Addr.addr;
        stBufCfg.u64BufAddr[1] = 0;
    }
    u64Vir_data[0] = mmap(NULL, buffersize, PROT_READ | PROT_WRITE, MAP_SHARED, g_FD[E_DRV_ID_MEM], stMsysMemInfo.phys);

    //FUNC_MSG("[InBuf]u64Vir_data:0x%lx \n", u64Vir_data[0]);
    _ReadFile(&u64Vir_data[0], &lFileSize, cInputFileName);
    //_CacheMemoryFlush();
    //printf("File size = 0x%x\n", lFileSize);
    pstInBuf->u64VirBufAddr[0] = (u64Vir_data[0]);
#if 0
    if(msync(u64Vir_data[0], buffersize, MS_SYNC) == -1)
    {
        perror("msync fail:");
        exit(1);
    }
#endif
    munmap(u64Vir_data[0], buffersize);

    pstInBuf->u64PhyBufAddr[0] = stBufCfg.u64BufAddr[0];
    pstInBuf->u64PhyBufAddr[1] = stBufCfg.u64BufAddr[1];
    pstInBuf->u64PhyBufAddr[2] = stBufCfg.u64BufAddr[2];
    pstInBuf->u32BufSize = stBufCfg.u32BufSize;
    // [InBuf]PhyBufMIUAddr[0]:0xbad0000, BufSize = 0x78000
    FUNC_MSG("[InBuf]MIUAddr[0]:0x%llx, Addr[1]:0x%llx\n", pstInBuf->u64PhyBufAddr[0], pstInBuf->u64PhyBufAddr[1]);

    return TRUE;
}

MS_BOOL _OutputBufferConfig(MHAL_DIVP_PixelFormat_e eFmt, MS_U16 u16Width, MS_U16 u16Height, DIVP_Buffer_Config_t *pstOutBuf)
{
    unsigned long  buffersize;
    IOCTL_DIVP_Buffer_Config_t stBufCfg;
    MSYS_DMEM_INFO stMsysMemInfo;
    MSYS_ADDR_TRANSLATION_INFO u64MIU_Addr;
    unsigned long long  u64Vir_Addr;
    void *u64Vir_data[3];
    long lFileSize;
    MS_U32 i;
    MS_U64 u64BufAddr;

    if(Open_Device(E_DRV_ID_DIP) == 0)
    {
        return FALSE;
    }

    if(Open_Device(E_DRV_ID_MSYS) == 0)
    {
        return FALSE;
    }

    if(Open_Device(E_DRV_ID_MEM) == 0)
    {
        return FALSE;
    }

    memset(&stMsysMemInfo, 0, sizeof(MSYS_DMEM_INFO));
    if((eFmt == E_MHAL_DIVP_PIXEL_FORMAT_YUV_MST_420) || \
            (eFmt == E_MHAL_DIVP_PIXEL_FORMAT_YC420_MSTTILE1_H264) || \
            (eFmt == E_MHAL_DIVP_PIXEL_FORMAT_YC420_MSTTILE2_H265) || \
            (eFmt == E_MHAL_DIVP_PIXEL_FORMAT_YUV_SEMIPLANAR_420))
    {
        pstOutBuf->u32BufOffset = (u16Width * u16Height * 3) / 2;
    }
    else if(eFmt == E_MHAL_DIVP_PIXEL_FORMAT_YUV422_YUYV)
    {
        pstOutBuf->u32BufOffset = u16Width * u16Height * 2;
    }
    buffersize = pstOutBuf->u32BufOffset * pstOutBuf->u8FrameCnt;
    stMsysMemInfo = _AllocateDmem("DIVP_OUT_BUF", 1, buffersize);
    if(stMsysMemInfo.kvirt == 0)
    {
        printf("%s:++++++++++++++++ allocate memory fail ++++++++++++++++\n", __FUNCTION__);
        return FALSE;
    }
    u64BufAddr = stMsysMemInfo.kvirt;
    for(i = 0 ; i < pstOutBuf->u8FrameCnt ; i++)
    {
        FUNC_MSG("[Buf_%d]:0x%llx\n", i, u64BufAddr);
        u64BufAddr += pstOutBuf->u32BufOffset;
    }

    u64MIU_Addr = _TranslationPhyToMIU(stMsysMemInfo);
    stBufCfg.u32BufSize = buffersize;

    stBufCfg.u16Width = u16Width;
    stBufCfg.u16Height = u16Height;
    stBufCfg.eFmt = eFmt;

    u64Vir_Addr    = stMsysMemInfo.kvirt;

    if((eFmt == E_MHAL_DIVP_PIXEL_FORMAT_YUV_MST_420) || \
            (eFmt == E_MHAL_DIVP_PIXEL_FORMAT_YC420_MSTTILE1_H264) || \
            (eFmt == E_MHAL_DIVP_PIXEL_FORMAT_YC420_MSTTILE2_H265) || \
            (eFmt == E_MHAL_DIVP_PIXEL_FORMAT_YUV_SEMIPLANAR_420))
    {
        stBufCfg.u64BufAddr[0] = u64MIU_Addr.addr;
        stBufCfg.u64BufAddr[1] = u64MIU_Addr.addr + (stBufCfg.u16Width * stBufCfg.u16Height);
    }
    else  // YUV422
    {
        stBufCfg.u64BufAddr[0] = u64MIU_Addr.addr;
        stBufCfg.u64BufAddr[1] = 0;
    }
    u64Vir_data[0] = mmap(NULL, buffersize, PROT_READ | PROT_WRITE, MAP_SHARED, g_FD[E_DRV_ID_MEM], stMsysMemInfo.phys);

    FUNC_MSG("[OutBuf]u64Vir_data:0x%lx\n", u64Vir_data[0]);
    pstOutBuf->u64VirBufAddr[0] = (u64Vir_data[0]);

    pstOutBuf->u64PhyBufAddr[0] = stBufCfg.u64BufAddr[0];
    pstOutBuf->u64PhyBufAddr[1] = stBufCfg.u64BufAddr[1];
    pstOutBuf->u64PhyBufAddr[2] = stBufCfg.u64BufAddr[2];
    pstOutBuf->u32BufSize = stBufCfg.u32BufSize;
    FUNC_MSG("[OutBuf]MIUAddr[0]:0x%llx, Addr[1]:0x%llx\n", pstOutBuf->u64PhyBufAddr[0], pstOutBuf->u64PhyBufAddr[1]);

    return TRUE;
}

MS_BOOL _DIProcessBufferConfig(MS_U16 u16Width, MS_U16 u16Height, DIVP_Buffer_Config_t *pstDiBuf)
{
    unsigned long  buffersize;
    IOCTL_DIVP_Buffer_Config_t stBufCfg;
    MSYS_DMEM_INFO stMsysMemInfo;
    MSYS_ADDR_TRANSLATION_INFO u64MIU_Addr;
    unsigned long long  u64Vir_Addr;
    void *u64Vir_data[3];

    if(Open_Device(E_DRV_ID_DIP) == 0)
    {
        return FALSE;
    }

    if(Open_Device(E_DRV_ID_MSYS) == 0)
    {
        return FALSE;
    }

    if(Open_Device(E_DRV_ID_MEM) == 0)
    {
        return FALSE;
    }

    memset(&stMsysMemInfo, 0, sizeof(MSYS_DMEM_INFO));
    buffersize = u16Width * u16Height * 2 * 4;
    stMsysMemInfo = _AllocateDmem("DIVP_DI_BUF", 1, buffersize);
    if(stMsysMemInfo.kvirt == 0)
    {
        printf("%s:++++++++++++++++ allocate memory fail ++++++++++++++++\n", __FUNCTION__);
        return FALSE;
    }
    u64MIU_Addr = _TranslationPhyToMIU(stMsysMemInfo);
    stBufCfg.u32BufSize = buffersize;

    stBufCfg.u16Width = u16Width;
    stBufCfg.u16Height = u16Height;

    u64Vir_Addr    = stMsysMemInfo.kvirt;

    stBufCfg.u64BufAddr[0] = u64MIU_Addr.addr;
    stBufCfg.u64BufAddr[1] = 0;

    u64Vir_data[0] = mmap(NULL, buffersize, PROT_READ | PROT_WRITE, MAP_SHARED, g_FD[E_DRV_ID_MEM], stMsysMemInfo.phys);

    FUNC_MSG("[DiBuf]u64Vir_data:0x%lx\n", u64Vir_data[0]);
    pstDiBuf->u64VirBufAddr[0] = (u64Vir_data[0]);
    munmap(u64Vir_data[0], buffersize);

    pstDiBuf->u64PhyBufAddr[0] = stBufCfg.u64BufAddr[0];
    pstDiBuf->u64PhyBufAddr[1] = stBufCfg.u64BufAddr[1];
    pstDiBuf->u64PhyBufAddr[2] = stBufCfg.u64BufAddr[2];
    pstDiBuf->u32BufSize = stBufCfg.u32BufSize;
    FUNC_MSG("[DiBuf]PhyBufAddr[0]:0x%llx, BufSize = 0x%x\n", pstDiBuf->u64PhyBufAddr[0], pstDiBuf->u32BufSize);

    return TRUE;
}

MS_BOOL _DIPTest(void)
{
    MS_BOOL ret = TRUE;
    DIVP_Buffer_Config_t stInBuf;
    DIVP_Buffer_Config_t stOutBuf;
    DIVP_Buffer_Config_t stDiBuf;
    MS_PHY phyInBufAddr, phyOutBufAddr;
    MS_U8 u8BufFrameCnt;
    MS_U16 u16InWidth, u16InHeight;
    MHAL_DIVP_PixelFormat_e eInfmt;
    MS_U32 u32InBufOffset;
    MS_U16 u16OutWidth, u16OutHeight, u16OutLineOffset;
    MHAL_DIVP_PixelFormat_e eOutfmt;
    MS_U32 u32OutBufOffset;
    MS_U8 i, j, cnt, InCnt;
    char pStrNum[3];
    char pFname[128];

    MHAL_DIVP_TnrLevel_e eDIVPTnrAttr = 0;
    MHAL_DIVP_DiType_e eDIVPDiAttr = 0;
    MHAL_DIVP_Rotate_e eDIVPRotAttr = 0;
    MHAL_DIVP_Mirror_t stDIVPMirrorAttr = {0};
    //MHAL_DIVP_Window_t stDIVPWinAttr = {0};
    MHAL_DIVP_InputInfo_t stDIVPInputInfo = {0};
    MHAL_DIVP_OutPutInfo_t stDIVPOutputInfo = {0};
    MHAL_DIVP_CaptureInfo_t stDIVPCaptureInfo = {0};

    u8BufFrameCnt = stDIPTestProperty.u8BufCnt;
    u16InWidth = stDIPTestProperty.u16SrcWidth;
    u16InHeight = stDIPTestProperty.u16SrcHeight;
    eInfmt = stDIPTestProperty.eInputDataFmt;
    u16OutWidth = stDIPTestProperty.u16DstWidth;
    u16OutHeight = stDIPTestProperty.u16DstHeight;
    eOutfmt = stDIPTestProperty.eOutputFormat;

    memset(&stDIVPInputInfo, 0x0, sizeof(MHAL_DIVP_InputInfo_t));
    memset(&stDIVPOutputInfo, 0x0, sizeof(MHAL_DIVP_OutPutInfo_t));
    memset(&stDIVPCaptureInfo, 0x0, sizeof(MHAL_DIVP_CaptureInfo_t));

    eDIVPTnrAttr = stDIPTestProperty.eTNRType;
    Test_DIVP_SetAttr_TNR(u32ChanId, &eDIVPTnrAttr);

    eDIVPDiAttr = stDIPTestProperty.e3DDIType;
    Test_DIVP_SetAttr_DITYPE(u32ChanId, &eDIVPDiAttr);

    eDIVPRotAttr = stDIPTestProperty.eRotation;
    Test_DIVP_SetAttr_ROTATE(u32ChanId, &eDIVPRotAttr);

#if 0
    stDIVPMirrorAttr.bHMirror = stDIPTestProperty.bHMirror;
    stDIVPMirrorAttr.bVMirror = stDIPTestProperty.bVMirror;
    Test_DIVP_SetAttr_MIRROR(u32ChanId, &stDIVPMirrorAttr);
#endif

#if 0  // old version
    stDIVPWinAttr.u16X = stDIPTestProperty.u16CapX;
    stDIVPWinAttr.u16Y = stDIPTestProperty.u16CapY;
    stDIVPWinAttr.u16Width = stDIPTestProperty.u16CapWidth;
    stDIVPWinAttr.u16Height = stDIPTestProperty.u16CapHeight;
    Test_DIVP_SetAttr_WINDOW(u32ChanId, &stDIVPWinAttr);
#endif

    Test_DIVP_SetCOVER(u32ChanId, &gstCoverProperty);

    if(stDIPTestProperty.u8SnapShotEnable > 0)
    {
        stDIVPCaptureInfo.stCropWin.u16X = stDIPTestProperty.u16CapX;
        stDIVPCaptureInfo.stCropWin.u16Y = stDIPTestProperty.u16CapY;
        stDIVPCaptureInfo.stCropWin.u16Width = stDIPTestProperty.u16CapWidth;
        stDIVPCaptureInfo.stCropWin.u16Height = stDIPTestProperty.u16CapHeight;

        stDIVPOutputInfo.ePxlFmt = eOutfmt;
        stDIVPOutputInfo.u16OutputWidth = u16OutWidth;
        stDIVPOutputInfo.u16OutputHeight = u16OutHeight;

        if(eOutfmt == E_MHAL_DIVP_PIXEL_FORMAT_YUV422_YUYV)
        {
            u16OutLineOffset = (((u16OutWidth + 15) >> 4) << 4);
            stDIVPOutputInfo.u16Stride[0] = (u16OutLineOffset * 2) ;  // by bytes  //stDIPTestProperty.u16WinLineOffSet;
        }
        else
        {
            u16OutLineOffset = (((u16OutWidth + 31) >> 5) << 5);
            stDIVPOutputInfo.u16Stride[0] = u16OutLineOffset;
        }
        stOutBuf.u8FrameCnt = u8BufFrameCnt;
        _OutputBufferConfig(eOutfmt, u16OutLineOffset, u16OutHeight, &stOutBuf);

        stDIVPOutputInfo.u64BufAddr[0] = stOutBuf.u64PhyBufAddr[0];
        stDIVPOutputInfo.u64BufAddr[1] = stOutBuf.u64PhyBufAddr[1];
        stDIVPOutputInfo.u32BufSize =  stOutBuf.u32BufSize;

        //Test_DIVP_Out(u32ChanId, &stDIVPOutputInfo);

        stDIVPCaptureInfo.eOutputPxlFmt = stDIVPOutputInfo.ePxlFmt;
        stDIVPCaptureInfo.u16Width = stDIVPOutputInfo.u16OutputWidth;
        stDIVPCaptureInfo.u16Height = stDIVPOutputInfo.u16OutputHeight;
        stDIVPCaptureInfo.u16Stride[0] = stDIVPOutputInfo.u16Stride[0];
        stDIVPCaptureInfo.u64BufAddr[0] = stDIVPOutputInfo.u64BufAddr[0];
        stDIVPCaptureInfo.u64BufAddr[1] = stDIVPOutputInfo.u64BufAddr[1];
        stDIVPCaptureInfo.u32BufSize = stDIVPOutputInfo.u32BufSize;

        stDIVPCaptureInfo.eInputPxlFmt = eInfmt;
        stDIVPCaptureInfo.eRotate = E_MHAL_DIVP_ROTATE_NONE;
        stDIVPCaptureInfo.stMirror.bHMirror = stDIPTestProperty.bHMirror;
        stDIVPCaptureInfo.stMirror.bVMirror = stDIPTestProperty.bVMirror;

#if 1  // manual test
        if(stDIPTestProperty.u8SnapShotEnable == 2)  // SC1 OP
        {
            stDIVPCaptureInfo.eDispId = E_MHAL_DIVP_Display1;

            //stDIVPCaptureInfo.eCapStage = E_MHAL_DIVP_CAP_STAGE_INPUT;
            stDIVPCaptureInfo.eCapStage = E_MHAL_DIVP_CAP_STAGE_OUTPUT;
            //stDIVPCaptureInfo.eCapStage = E_MHAL_DIVP_CAP_STAGE_OUTPUT_WITH_OSD;
        }
        else  // SC0 OP
        {
            stDIVPCaptureInfo.eDispId = E_MHAL_DIVP_Display0;

            //stDIVPCaptureInfo.eCapStage = E_MHAL_DIVP_CAP_STAGE_INPUT;
            stDIVPCaptureInfo.eCapStage = E_MHAL_DIVP_CAP_STAGE_OUTPUT;
            //stDIVPCaptureInfo.eCapStage = E_MHAL_DIVP_CAP_STAGE_OUTPUT_WITH_OSD;
        }
#endif

        //FUNC_MSG("stDIVPCaptureInfo: Addr0=0x%x, Addr1=0x%x\n", (MS_U32)(stDIVPCaptureInfo.u64BufAddr[0]), (MS_U32)(stDIVPCaptureInfo.u64BufAddr[1]));
        //FUNC_MSG("    OutW=%d, OutH= %d\n", stDIVPCaptureInfo.u16Width, stDIVPCaptureInfo.u16Height);
        //FUNC_MSG("    u16Stride=%d, ePxlFmt=%d\n", stDIVPCaptureInfo.u16Stride[0], stDIVPCaptureInfo.eOutputPxlFmt);

        Test_DIVP_CaptureTiming(u32ChanId, &stDIVPCaptureInfo);

#if 1
        // Convert and Dump Image
        FUNC_MSG("Out file size = 0x%x\n", stOutBuf.u32BufSize);
        YUVRawToFile((MS_U8 *)(stOutBuf.u64VirBufAddr[0]), cOutputFileName, stOutBuf.u32BufSize);
#endif

        munmap((void *)(stOutBuf.u64VirBufAddr[0]), stOutBuf.u32BufSize);

    }
    else
    {

        stInBuf.u8FrameCnt = u8BufFrameCnt;
        _InputBufferLoad(eInfmt, u16InWidth, u16InHeight, &stInBuf);

        stDIVPInputInfo.stCropWin.u16X = stDIPTestProperty.u16CapX;
        stDIVPInputInfo.stCropWin.u16Y = stDIPTestProperty.u16CapY;
        stDIVPInputInfo.stCropWin.u16Width = stDIPTestProperty.u16CapWidth;
        stDIVPInputInfo.stCropWin.u16Height = stDIPTestProperty.u16CapHeight;

        //stDIVPInputInfo.eFieldType = E_MHAL_DIVP_FIELD_TYPE_NONE;
        stDIVPInputInfo.ePxlFmt = eInfmt;
        stDIVPInputInfo.eScanMode = stDIPTestProperty.enScan;
        stDIVPInputInfo.eTileMode = stDIPTestProperty.eTileBlock;

        if(stDIPTestProperty.stSwCropWin.bEnable)
        {
            stDIVPInputInfo.u16InputWidth = stDIPTestProperty.stSwCropWin.u16Width;
            stDIVPInputInfo.u16InputHeight = stDIPTestProperty.stSwCropWin.u16Height;
        }
        else
        {
            stDIVPInputInfo.u16InputWidth = u16InWidth;
            stDIVPInputInfo.u16InputHeight = u16InHeight;
        }
        stDIVPInputInfo.u64Pts = 0;  //
#if 1
        if(eInfmt == E_MHAL_DIVP_PIXEL_FORMAT_YUV422_YUYV)
        {
            stDIVPInputInfo.u16Stride[0] = (u16InWidth * 2); //stDIPTestProperty.u16SrcPitch;
        }
        //else if((eInfmt == E_MHAL_DIVP_PIXEL_FORMAT_YC420_MSTTILE1_H264) || (eInfmt == E_MHAL_DIVP_PIXEL_FORMAT_YC420_MSTTILE2_H265))
        //{
        //    stDIVPInputInfo.u16Stride[0] = (u16InWidth * 3) / 2; //stDIPTestProperty.u16SrcPitch;
        //}
        else
#endif
        {
            stDIVPInputInfo.u16Stride[0] = u16InWidth; //stDIPTestProperty.u16SrcPitch;
        }

        if(stDIPTestProperty.stSwCropWin.bEnable)
        {
            if(eInfmt == E_MHAL_DIVP_PIXEL_FORMAT_YUV422_YUYV)
            {
                stDIVPInputInfo.u64BufAddr[0] = stInBuf.u64PhyBufAddr[0] + stDIPTestProperty.stSwCropWin.u16Y * u16InWidth * 2 + (stDIPTestProperty.stSwCropWin.u16X / 32) * 32 * 2;
            }
            else if((eInfmt == E_MHAL_DIVP_PIXEL_FORMAT_YC420_MSTTILE1_H264) || (eInfmt == E_MHAL_DIVP_PIXEL_FORMAT_YC420_MSTTILE2_H265))
            {
                if(stDIVPInputInfo.eTileMode == E_MHAL_DIVP_TILE_MODE_32x16)  // for tile 32x16
                {
                    stDIVPInputInfo.u64BufAddr[0] = stInBuf.u64PhyBufAddr[0] + stDIPTestProperty.stSwCropWin.u16Y * u16InWidth + (stDIPTestProperty.stSwCropWin.u16X) * 16;
                    stDIVPInputInfo.u64BufAddr[1] = stInBuf.u64PhyBufAddr[1] + (stDIPTestProperty.stSwCropWin.u16Y / 2) * u16InWidth + (stDIPTestProperty.stSwCropWin.u16X) * 16;
                    FUNC_MSG("===== TILE 32x16 =====\n");
                }
                else  // for tile 32x32 and tile 16x32
                {
                    stDIVPInputInfo.u64BufAddr[0] = stInBuf.u64PhyBufAddr[0] + stDIPTestProperty.stSwCropWin.u16Y * u16InWidth + (stDIPTestProperty.stSwCropWin.u16X) * 32;
                    stDIVPInputInfo.u64BufAddr[1] = stInBuf.u64PhyBufAddr[1] + (stDIPTestProperty.stSwCropWin.u16Y / 2) * u16InWidth + (stDIPTestProperty.stSwCropWin.u16X) * 32;
                    FUNC_MSG("===== TILE 32x32/16x32 =====\n");
                }
            }
            else
            {
                stDIVPInputInfo.u64BufAddr[0] = stInBuf.u64PhyBufAddr[0] + stDIPTestProperty.stSwCropWin.u16Y * u16InWidth + (stDIPTestProperty.stSwCropWin.u16X / 32) * 32;
                stDIVPInputInfo.u64BufAddr[1] = stInBuf.u64PhyBufAddr[1] + (stDIPTestProperty.stSwCropWin.u16Y / 2) * u16InWidth + (stDIPTestProperty.stSwCropWin.u16X / 32) * 32;
            }
        }
        else
        {
            stDIVPInputInfo.u64BufAddr[0] = stInBuf.u64PhyBufAddr[0];
            stDIVPInputInfo.u64BufAddr[1] = stInBuf.u64PhyBufAddr[1];
        }
        stDIVPInputInfo.u32BufSize = stInBuf.u32BufSize;

        stDIVPInputInfo.stMfdecInfo.bDbEnable = FALSE;
        if(stDIPTestProperty.stMfdecInfo.bDbEnable == TRUE)
        {
            if(stDIPTestProperty.stMfdecInfo.bBypassCodecMode == FALSE)
            {
                stDIPTestProperty.stMfdecInfo.u64DbBaseAddr = (stInBuf.u64PhyBufAddr[0] + (u16InWidth * u16InHeight * 3) / 2);
            }
            memcpy(&stDIVPInputInfo.stMfdecInfo, &stDIPTestProperty.stMfdecInfo, sizeof(MHAL_DIVP_MFdecInfo_t));
        }

        //if(stDIPTestProperty.e3DDIType != E_MHAL_DIVP_DI_TYPE_3D)
        //    Test_DIVP_In(u32ChanId, &stDIVPInputInfo);

        stDIVPOutputInfo.ePxlFmt = eOutfmt;
        stDIVPOutputInfo.u16OutputWidth = u16OutWidth;
        stDIVPOutputInfo.u16OutputHeight = u16OutHeight;

        if(eOutfmt == E_MHAL_DIVP_PIXEL_FORMAT_YUV422_YUYV)
        {
            u16OutLineOffset = (((u16OutWidth + 15) >> 4) << 4);
            stDIVPOutputInfo.u16Stride[0] = (u16OutLineOffset * 2) ;  // by bytes  //stDIPTestProperty.u16WinLineOffSet;
        }
        else
        {
            u16OutLineOffset = (((u16OutWidth + 31) >> 5) << 5);
            if(eOutfmt == E_MHAL_DIVP_PIXEL_FORMAT_YUV_MST_420)
            {
                stDIVPOutputInfo.u16Stride[0] = (u16OutLineOffset * 3 / 2);
            }
            else
            {
                stDIVPOutputInfo.u16Stride[0] = u16OutLineOffset;
            }
        }
        stOutBuf.u8FrameCnt = u8BufFrameCnt;
        _OutputBufferConfig(eOutfmt, u16OutLineOffset, u16OutHeight, &stOutBuf);

        stDIVPOutputInfo.u64BufAddr[0] = stOutBuf.u64PhyBufAddr[0];
        stDIVPOutputInfo.u64BufAddr[1] = stOutBuf.u64PhyBufAddr[1];
        stDIVPOutputInfo.u32BufSize =  stOutBuf.u32BufSize;

        //if(stDIPTestProperty.e3DDIType != E_MHAL_DIVP_DI_TYPE_3D)
        //    Test_DIVP_Out(u32ChanId, &stDIVPOutputInfo);

        if(stDIPTestProperty.e3DDIType == E_MHAL_DIVP_DI_TYPE_3D)
        {
#if 1
            _DIProcessBufferConfig(736, 480, &stDiBuf);

            for(i = 0 ; i < u8BufFrameCnt ; i++)
            {

                FUNC_MSG("*********** Process 3DDI image %d  ************\n", i);

                //stDIVPInputInfo.eScanMode = E_MHAL_DIVP_SCAN_MODE_PROGRESSIVE;
                //stDIVPInputInfo.eScanMode = E_MHAL_DIVP_SCAN_MODE_INTERLACE_FRAME;
                stDIVPInputInfo.eScanMode = E_MHAL_DIVP_SCAN_MODE_INTERLACE_FIELD;

                if(i < 2)
                {
                    stDIVPInputInfo.stDiSettings.eDiMode = E_MHAL_DIVP_DI_MODE_BOB;
                }
                else
                {
                    stDIVPInputInfo.stDiSettings.eDiMode = E_MHAL_DIVP_DI_MODE_WAVE;
                }
                if(i % 2)
                {
                    stDIVPInputInfo.stDiSettings.eFieldOrderType = E_MHAL_DIVP_FIELD_ORDER_TYPE_BOTTOM;
                    stDIVPInputInfo.stDiSettings.eFieldType = E_MHAL_DIVP_FIELD_TYPE_BOTTOM;
                }
                else
                {
                    stDIVPInputInfo.stDiSettings.eFieldOrderType = E_MHAL_DIVP_FIELD_ORDER_TYPE_TOP;
                    stDIVPInputInfo.stDiSettings.eFieldType = E_MHAL_DIVP_FIELD_TYPE_TOP;
                }

                stDIVPInputInfo.u64BufAddr[0] = stInBuf.u64PhyBufAddr[0] + (stInBuf.u32BufOffset * i);
                stDIVPInputInfo.u64BufAddr[1] = stInBuf.u64PhyBufAddr[1] + (stInBuf.u32BufOffset * i);
#if 1  //#ifdef DIP_UT_ALLOC_MEM_FOR_3DDI  // using u64BufAddr[2] to store 3DDI bufer for UT case
                stDIVPInputInfo.u64BufAddr[2] = stDiBuf.u64PhyBufAddr[0];  // for 3DDI buffer
#endif

                //Test_DIVP_In(u32ChanId, &stDIVPInputInfo);

                stDIVPOutputInfo.u64BufAddr[0] = stOutBuf.u64PhyBufAddr[0] + (stOutBuf.u32BufOffset * i);
                stDIVPOutputInfo.u64BufAddr[1] = stOutBuf.u64PhyBufAddr[1] + (stOutBuf.u32BufOffset * i);

                //Test_DIVP_Out(u32ChanId, &stDIVPOutputInfo);

                //FUNC_MSG("3DDI: Addr0=0x%x, Addr1=0x%x\n", (MS_U32)(stDIVPInputInfo.u64BufAddr[0]), (MS_U32)(stDIVPInputInfo.u64BufAddr[1]));
                //FUNC_MSG("    3DDI eScanMode=%d, eDiMode=%d\n", stDIVPInputInfo.eScanMode, stDIVPInputInfo.stDiSettings.eDiMode);
                //FUNC_MSG("    3DDI eFieldType=%d, eFieldOrderType=%d\n", stDIVPInputInfo.stDiSettings.eFieldType, stDIVPInputInfo.stDiSettings.eFieldOrderType);

                Test_DIVP_ProcessDramData(u32ChanId, &stDIVPInputInfo, &stDIVPOutputInfo);

#if 1
                Int2Str(i, pStrNum);
                InCnt = 0;
                for(cnt = 0 ; cnt < (strlen(cOutputFileName) - 4) ; cnt++)
                {
                    pFname[cnt] = cOutputFileName[InCnt++];
                }
                for(j = 0 ; j < strlen(pStrNum) ; j++)
                {
                    pFname[cnt++] = pStrNum[j];
                }
                for(j = 0 ; j < 4 ; j++)
                {
                    pFname[cnt++] = cOutputFileName[InCnt++];
                }
                pFname[cnt++] = 0;

                // Convert and Dump Image
                FUNC_MSG("Out file size = 0x%x\n", stOutBuf.u32BufOffset);
                YUVRawToFile((MS_U8 *)(stOutBuf.u64VirBufAddr[0] + (stOutBuf.u32BufOffset * i)), pFname, stOutBuf.u32BufOffset);

#endif

                FUNC_MSG("    \n");
                FUNC_MSG("    \n");
                FUNC_MSG("    \n");
                FUNC_MSG("    \n");
                FUNC_MSG("    \n");
                FUNC_MSG("    \n");

            }

            _FreeDmem("DIVP_DI_BUF");

#endif

        }
        else
        {

            //MHAL_DIVP_ProcessDramData(pCtx, &stDIVPInputInfo, &stDIVPOutputInfo, NULL);
            Test_DIVP_ProcessDramData(u32ChanId, &stDIVPInputInfo, &stDIVPOutputInfo);

#if 1
            // Convert and Dump Image
            FUNC_MSG("Out file size = 0x%x\n", stOutBuf.u32BufSize);
            YUVRawToFile((MS_U8 *)(stOutBuf.u64VirBufAddr[0]), cOutputFileName, stOutBuf.u32BufSize);

            //YUVRawToFile((MS_U8 *)(stOutBuf.u64VirBufAddr[0]), "test.raw", stOutBuf.u32BufSize);
#endif
        }

        munmap((void *)(stOutBuf.u64VirBufAddr[0]), stOutBuf.u32BufSize);
        //munmap((void *)(stInBuf.u64VirBufAddr[0]), stInBuf.u32BufSize);

        //_FreeDmem("DIVP_IN_BUF");

    }

    //_FreeDmem("DIVP_OUT_BUF");

    return ret;
}


static void UsingGuide(int argc, char **argv)
{
    fprintf(stdout,	"Usage: %s [-if format][-of format][-ir resolution][-or resolution][-crop range][-i input file name][-o output file name]\n"
            "like : '%s -if YUYV -of YUYV -ir 640 360 -or 640 360 -crop 0 0 640 360 -i /system/bin/DIP_640x360_YUV422.raw -o /data/DIP_output.raw'\n"
            "Arguments:\n", argc ? argv[0] : NULL, argc ? argv[0] : NULL);
    /* Input pixel format */
    fprintf(stdout,	"-if\t: input format,\n"
            "\t '-if <format>', '-if YUYV' as YUV422,\n"
            "\t 'YUYV' for packed YUV422 payload,\n"
            "\t 'YUV420' for mstar tile mode YUV420 payload,\n"
            "\t 'NV12' for YUV420 semi planer payload,\n");
    /* Output pixel format */
    fprintf(stdout,	"-of\t: output format,\n"
            "\t '-of <format>', '-of YUYV' as YUV422,\n"
            "\t 'YUYV' for packed YUV422 payload,\n"
            "\t 'YUV420' for mstar tile mode YUV420 payload,\n"
            "\t 'NV12' for YUV420 semi planer payload,\n"
            "\t 'MDWIN' for YUV420 8b payload for mdwin,\n"
            "\t 'RGB888' for BMP file format.\n");
    /* Input resolution */
    fprintf(stdout,	"-ir\t: input resolution,\n"
            "\t '-ir <width> <height>', '-ir 1920 1080' as full HD,\n"
            "\t resolution parameters MUST be a pair like '-ir 1920 1080'.\n");
    /* Output resolution */
    fprintf(stdout,	"-or\t: output resolution,\n"
            "\t '-or <width> <height>', '-or 1920 1080' as full HD,\n"
            "\t resolution parameters MUST be a pair like '-or 1920 1080'.\n");
    /* Crop resolution */
    fprintf(stdout,	"-crop\t: crop range,\n"
            "\t '-crop <startx> <starty> <width> <height>', '-crop 0 0 1920 1080' as full HD,\n"
            "\t resolution parameters MUST be a pair like '-crop 0 0 1920 1080'.\n");
    /* mirror flip */
    fprintf(stdout,	"-rotn/-mirror/-flip\t: image orientation,\n"
            "\t '-rotn <value>', '-rotn 1' as rotation 90 degree, '-rotn 3 as rotation 270 degree,'\n"
            "\t '-mirror <enable>', '-mirror 1' as enable mirror,\n"
            "\t '-flip <enable>', '-flip 1' as enable flip.\n");
    /* input file name */
    fprintf(stdout,	"-i\t: input frame to file\n"
            "\t '-i <filename>', input file name, '-i /system/bin/DIP_640x360_YUV422.raw' as source frame from /system/bin/DIP_640x360_YUV422.raw.\n");
    /* output file name */
    fprintf(stdout,	"-o\t: output frame to file\n"
            "\t '-o <filename>', saved output data to file, '-o /data/DIP_outputYUV422.raw' as save frame to /data/DIP_outputYUV422.raw,\n"
            "\t '-o /data/DIP_output.bmp' as save frame to /data/DIP_output.bmp.\n");
    /* Input date YUV420 tile mode */
    fprintf(stdout,	"-tile\t: input tile mode,\n"
            "\t '-tile <format>', '-tile 32x32' as Tile mode 32x32,\n"
            "\t '16x16' for Tile mode 16x16,\n"
            "\t '32x16' for Tile mode 32x16,\n"
            "\t '16x32' for Tile mode 16x32,\n"
            "\t '32x32' for Tile mode 32x32.\n");
    /* Set TNR level */
    fprintf(stdout,	"-tnr\t: input tnr level,\n"
            "\t '-tnr <value>', '-tnr 1' as low level TNR, '-tnr 2' as middle level TNR, '-tnr 3' as high level TNR.\n");
    /* Set 3DDI mode */
    fprintf(stdout,	"-ddi\t: input 3DDI mode and frame count,\n"
            "\t '-ddi <value> <fcnt>', '-ddi 1 3' as 2d mode with 3 frame data, '-ddi 2 6' as 3d mode with 6 frame data.\n");
    /* Set Snapshot mode */
    fprintf(stdout,	"-snapshot\t: enable snapshot mode,\n"
            "\t '-snapshot <value>', '-snapshot 1' as enable, 'others as disable.\n");
    /* Set MFDEC mode */
    fprintf(stdout,	"-mfdec\t: enable MFDEC mode,\n"
            "\t '-mfdec <enable> <bypass>', '-mfdec 1 0' as enable mfdec and use compressed data,\n"
            "\t '-mfdec 1 1' as enable mfdec and bypass codec to use YUV420 tile raw data,\n"
            "\t '-mfdec 0 0' as disable mfdec engine.\n");
    /* Set SW crop window mode */
    fprintf(stdout,	"-swcrop\t: enable SW CROP mode,\n"
            "\t '-swcrop <X> <Y> <Hsize> <Vsize>', with 32 pixel alignment\n"
            "\t '-swcrop 128 128 192 128' as enable SW CROP mode, Crop window is (128, 128, 192, 128).\n");

    exit(EXIT_FAILURE);
}

static void get_opt(Param *SetParam, int argc, char **argv)
{
    char	*endptr = NULL;
    int		i = 0;

    /* Default value */
    memset(SetParam, 0x0, sizeof(Param));
    stDIPTestProperty.stMfdecInfo.bDbEnable = FALSE;
    stDIPTestProperty.u8BufCnt = 1;
    u32ChanId = 0;
    gstCoverProperty.bEnable = 0;
    stDIPTestProperty.u8SnapShotEnable = 0;
    stDIPTestProperty.stSwCropWin.bEnable = 0;

    if(argc > 1)
    {
        if ( argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) )
            UsingGuide(argc, argv);

        for (i = 1; i < argc; i++)
        {
            /* Output resolution */
            if (!strcmp(argv[i], "-or"))
            {
                SetParam->u16DstWidth = strtol(argv[++i], &endptr, 10);
                SetParam->u16DstHeight = strtol(argv[++i], &endptr, 10);
                if (SetParam->u16DstWidth == 0 || SetParam->u16DstHeight == 0)
                {
                    fprintf(stderr, "Invalid output resolution parameter(%dx%d).\n", SetParam->u16DstWidth, SetParam->u16DstHeight);
                    UsingGuide(argc, argv);
                }
                stDIPTestProperty.u16DstWidth = SetParam->u16DstWidth;
                stDIPTestProperty.u16DstHeight = SetParam->u16DstHeight;
            }
            /* Input resolution */
            else if (!strcmp(argv[i], "-ir"))
            {
                SetParam->u16SrcWidth = strtol(argv[++i], &endptr, 10);
                SetParam->u16SrcHeight = strtol(argv[++i], &endptr, 10);
                if (SetParam->u16SrcWidth == 0 || SetParam->u16SrcHeight == 0)
                {
                    fprintf(stderr, "Invalid input resolution parameter(%dx%d).\n", SetParam->u16SrcWidth, SetParam->u16SrcHeight);
                    UsingGuide(argc, argv);
                }
                stDIPTestProperty.u16SrcWidth = SetParam->u16SrcWidth;
                stDIPTestProperty.u16SrcHeight = SetParam->u16SrcHeight;
            }
            /* Crop resolution */
            else if (!strcmp(argv[i], "-crop"))
            {
                SetParam->u16CapX = strtol(argv[++i], &endptr, 10);
                SetParam->u16CapY = strtol(argv[++i], &endptr, 10);
                SetParam->u16CapWidth = strtol(argv[++i], &endptr, 10);
                SetParam->u16CapHeight = strtol(argv[++i], &endptr, 10);
                if ((SetParam->u16CapWidth == 0) || (SetParam->u16CapHeight == 0))
                {
                    fprintf(stderr, "Invalid crop resolution parameter(%d, %d, %d, %d).\n", SetParam->u16CapX, SetParam->u16CapY, SetParam->u16CapWidth, SetParam->u16CapHeight);
                    UsingGuide(argc, argv);
                }
                stDIPTestProperty.u16CapX = SetParam->u16CapX;
                stDIPTestProperty.u16CapY = SetParam->u16CapY;
                stDIPTestProperty.u16CapWidth = SetParam->u16CapWidth;
                stDIPTestProperty.u16CapHeight = SetParam->u16CapHeight;
            }
            /* Input pixel format */
            else if(!strcmp(argv[i], "-if"))
            {
                if(!strcmp(argv[i + 1], "YUYV"))
                    SetParam->eInputDataFmt = E_MHAL_DIVP_PIXEL_FORMAT_YUV422_YUYV;
                else if(!strcmp(argv[i + 1], "YUV420"))
                    SetParam->eInputDataFmt = E_MHAL_DIVP_PIXEL_FORMAT_YC420_MSTTILE1_H264;
                else if(!strcmp(argv[i + 1], "NV12"))
                    SetParam->eInputDataFmt = E_MHAL_DIVP_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
                else
                {
                    fprintf(stderr, "Invalid input pixel format parameter(%s).\n", argv[i + 1]);
                    UsingGuide(argc, argv);
                }
                stDIPTestProperty.eInputDataFmt = SetParam->eInputDataFmt;
                ++i;
            }
            /* Output pixel format */
            else if(!strcmp(argv[i], "-of"))
            {
                if(!strcmp(argv[i + 1], "YUYV"))
                    SetParam->eOutputFormat = E_MHAL_DIVP_PIXEL_FORMAT_YUV422_YUYV;
                else if(!strcmp(argv[i + 1], "YUV420"))
                    SetParam->eOutputFormat = E_MHAL_DIVP_PIXEL_FORMAT_YC420_MSTTILE1_H264;
                else if(!strcmp(argv[i + 1], "NV12"))
                    SetParam->eOutputFormat = E_MHAL_DIVP_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
                else if(!strcmp(argv[i + 1], "MDWIN"))
                    SetParam->eOutputFormat = E_MHAL_DIVP_PIXEL_FORMAT_YUV_MST_420;
                else if(!strcmp(argv[i + 1], "RGB888"))
                    SetParam->eOutputFormat = E_MHAL_DIVP_PIXEL_FORMAT_ARGB8888;
                else
                {
                    fprintf(stderr, "Invalid output pixel format parameter(%s).\n", argv[i + 1]);
                    UsingGuide(argc, argv);
                }
                stDIPTestProperty.eOutputFormat = SetParam->eOutputFormat;
                ++i;
            }
            /* mirror flip */
            else if(!strcmp(argv[i], "-rotn")		||
                    !strcmp(argv[i], "-mirror")		||
                    !strcmp(argv[i], "-flip"))
            {
                if (!strcmp(argv[i], "-mirror"))
                {
                    SetParam->bHMirror = strtol(argv[++i], &endptr, 10);
                    stDIPTestProperty.bHMirror = SetParam->bHMirror;
                }
                else if (!strcmp(argv[i], "-flip"))
                {
                    SetParam->bVMirror = strtol(argv[++i], &endptr, 10);
                    stDIPTestProperty.bVMirror = SetParam->bVMirror;
                }
                else if(!strcmp(argv[i], "-rotn"))
                {
                    stDIPTestProperty.eRotation = strtol(argv[++i], &endptr, 10);
                }
                else
                {
                    fprintf(stderr, "Invalid input mirror flip parameter(%d, %d).\n", SetParam->bHMirror, SetParam->bVMirror);
                    UsingGuide(argc, argv);
                }
            }
            /* Input file name */
            else if(!strcmp(argv[i], "-i"))
            {
                if(argv[i + 1])
                {
                    strcpy(SetParam->Infilename, (char*)argv[++i]);
                }
                else
                {
                    fprintf(stderr, "Invalid input file name parameter.\n");
                    UsingGuide(argc, argv);
                }
                memset(cInputFileName, 0x0, 128);
                strcpy(cInputFileName, SetParam->Infilename);
            }
            /* Output file name */
            else if(!strcmp(argv[i], "-o"))
            {
                if(argv[i + 1])
                {
                    strcpy(SetParam->Outfilename, (char*)argv[++i]);
                }
                else
                {
                    fprintf(stderr, "Invalid output file name parameter.\n");
                    UsingGuide(argc, argv);
                }
                memset(cOutputFileName, 0x0, 128);
                strcpy(cOutputFileName, SetParam->Outfilename);
            }
            /* Input pixel format */
            else if(!strcmp(argv[i], "-tile"))
            {
                if(!strcmp(argv[i + 1], "16x16"))
                    SetParam->eTileBlock = E_MHAL_DIVP_TILE_MODE_16x16;
                else if(!strcmp(argv[i + 1], "16x32"))
                    SetParam->eTileBlock = E_MHAL_DIVP_TILE_MODE_16x32;
                else if(!strcmp(argv[i + 1], "32x16"))
                    SetParam->eTileBlock = E_MHAL_DIVP_TILE_MODE_32x16;
                else if(!strcmp(argv[i + 1], "32x32"))
                    SetParam->eTileBlock = E_MHAL_DIVP_TILE_MODE_32x32;
                else
                {
                    fprintf(stderr, "Invalid input pixel format parameter(%s).\n", argv[i + 1]);
                    UsingGuide(argc, argv);
                }
                stDIPTestProperty.eTileBlock = SetParam->eTileBlock;
                ++i;
            }
            /* tnr */
            else if(!strcmp(argv[i], "-tnr"))
            {
                stDIPTestProperty.eTNRType = strtol(argv[++i], &endptr, 10);
            }
            /* 3ddi */
            else if(!strcmp(argv[i], "-ddi"))
            {
                stDIPTestProperty.e3DDIType = strtol(argv[++i], &endptr, 10);
                stDIPTestProperty.u8BufCnt = strtol(argv[++i], &endptr, 10);
            }
            /* snapshot mode */
            else if(!strcmp(argv[i], "-snapshot"))
            {
                stDIPTestProperty.u8SnapShotEnable = strtol(argv[++i], &endptr, 10);
            }
            /* mfdec mode */
            else if(!strcmp(argv[i], "-mfdec"))
            {
                stDIPTestProperty.stMfdecInfo.bDbEnable = strtol(argv[++i], &endptr, 10);
                stDIPTestProperty.stMfdecInfo.bBypassCodecMode = strtol(argv[++i], &endptr, 10);

                if(stDIPTestProperty.stMfdecInfo.bDbEnable)
                {
                    stDIPTestProperty.stMfdecInfo.u16StartX = strtol(argv[++i], &endptr, 10);
                    stDIPTestProperty.stMfdecInfo.u16StartY = strtol(argv[++i], &endptr, 10);
                    stDIPTestProperty.stMfdecInfo.u16Width = strtol(argv[++i], &endptr, 10);
                    stDIPTestProperty.stMfdecInfo.u16Height = strtol(argv[++i], &endptr, 10);
                }
            }
            /* cover test mode */
            else if(!strcmp(argv[i], "-cover"))
            {
                gstCoverProperty.bEnable = strtol(argv[++i], &endptr, 10);
                gstCoverProperty.u16WinHStart = strtol(argv[++i], &endptr, 10);
                gstCoverProperty.u16WinVStart = strtol(argv[++i], &endptr, 10);
                gstCoverProperty.u16WinHSize = strtol(argv[++i], &endptr, 10);
                gstCoverProperty.u16WinVSize = strtol(argv[++i], &endptr, 10);
                gstCoverProperty.u16ColorR = strtol(argv[++i], &endptr, 10);
                gstCoverProperty.u16ColorG = strtol(argv[++i], &endptr, 10);
                gstCoverProperty.u16ColorB = strtol(argv[++i], &endptr, 10);
            }
            /* sw crop mode */
            else if(!strcmp(argv[i], "-swcrop"))
            {
                stDIPTestProperty.stSwCropWin.bEnable = 1;
                stDIPTestProperty.stSwCropWin.u16X = strtol(argv[++i], &endptr, 10);
                stDIPTestProperty.stSwCropWin.u16Y = strtol(argv[++i], &endptr, 10);
                stDIPTestProperty.stSwCropWin.u16Width = strtol(argv[++i], &endptr, 10);
                stDIPTestProperty.stSwCropWin.u16Height = strtol(argv[++i], &endptr, 10);
            }
            else
            {
                fprintf(stderr, "Invalid input parameter(%s).\n", argv[i]);
                UsingGuide(argc, argv);
            }
        }
    }
    else
    {
        UsingGuide(argc, argv);
    }
}

int main(int argc, char *argv[])
{
    Param			SetParam;

    get_opt(&SetParam, argc, argv);

    if (!_SysInit())
        goto DIP_TEST_EXIT;

    if (!_DIPInit())
        goto DIP_TEST_EXIT;

    if (!_DIPTest())
        goto DIP_TEST_EXIT;

DIP_TEST_EXIT:
    _DIPDeInit();
    _SysDeInit();

    return 0;
}
