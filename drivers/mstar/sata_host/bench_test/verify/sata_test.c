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

//#include "apiXC.h"
#include "sata_test.h"
#include "mdrv_sata_io.h"
#include "mdrv_sata_io_st.h"

//---------------------------------------------------------------
// enum
//---------------------------------------------------------------
typedef enum
{
    E_DRV_ID_SATA = 0,
    E_DRV_ID_MSYS,
    E_DRV_ID_MEM,
    E_DRV_ID_NUM,
} EN_DRV_ID_TYPE;


static int g_FD[E_DRV_ID_NUM] = {-1, -1, -1};

typedef struct __Param
{
    MS_U16 u16PortNo;
    MS_U16 u16GenNo;
} Param;

ST_SATA_Test_Property stSATATestProperty =
{
    0,       // u16PortNo
    3,       // u16GenNo
};

unsigned long long _GetSystemTimeStamp (void)
{
    struct timeval         tv;
    struct timezone tz;
    unsigned long long u64TimeStamp;
    gettimeofday(&tv, &tz);
    u64TimeStamp = tv.tv_sec * 1000000ULL + tv.tv_usec;
    return u64TimeStamp;
}

int Open_Device(EN_DRV_ID_TYPE enDrvID)
{
    int ret = 1;
    char device_name[E_DRV_ID_NUM][50] =
    {
        {"/dev/msata"},
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
            //printf("open %s  %d sucess\n", &device_name[enDrvID][0], g_FD[enDrvID]);
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

MS_BOOL Test_SATA_Set_LoopbackTest(MS_U16 u16PortNo, MS_U16 u16GenNo)
{
    stSata_Loopback_Test stInCfg;

    if(Open_Device(E_DRV_ID_SATA) == 0)
    {
        return FALSE;
    }

    if((u16PortNo >= 2)  || ((u16GenNo == 0)  || (u16GenNo >= 4) ))
    {
        return FALSE;
    }

    stInCfg.u16PortNo = u16PortNo;
    stInCfg.u16GenNo = u16GenNo;
    stInCfg.s32Result = 0;

    ioctl(g_FD[E_DRV_ID_SATA], IOCTL_SATA_SET_LOOPBACK_TEST, &stInCfg);

    printf("SATA loopback test result = %d\n", stInCfg.s32Result);

    return TRUE;
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

MS_BOOL _SATAInit(void)
{
    MS_BOOL ret = TRUE;

    if(Open_Device(E_DRV_ID_SATA) == 0)
    {
        return FALSE;
    }

    return ret;
}

MS_BOOL _SATADeInit(void)
{
    MS_BOOL ret = TRUE;

    if(Open_Device(E_DRV_ID_SATA) == 0)
    {
        return FALSE;
    }

    return ret;
}

MS_BOOL _SATATest(void)
{
    MS_BOOL ret = TRUE;
    MS_U16 u16PortNo = stSATATestProperty.u16PortNo;
    MS_U16 u16GenNo = stSATATestProperty.u16GenNo;

    ret = Test_SATA_Set_LoopbackTest(u16PortNo, u16GenNo);

    return ret;
}

static void UsingGuide(int argc, char **argv)
{
    fprintf(stdout,	"Usage: %s [-loopback PortNo GenNo]\n"
            "like : '%s -loopback 0 3 '\n"
            "Arguments:\n", argc ? argv[0] : NULL, argc ? argv[0] : NULL);
    /* Input pixel format */
    fprintf(stdout,	"-loopback\t: loopback test,\n"
            "\t '-loopback <port no.> <gen no.>', <port no.>: 0 or 1, <gen no.>: 1 ~ 3\n");

    exit(EXIT_FAILURE);
}

static void get_opt(Param *SetParam, int argc, char **argv)
{
    char	*endptr = NULL;
    int		i = 0;

    /* Default value */
    memset(SetParam, 0x0, sizeof(Param));

    if(argc > 1)
    {
        if ( argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) )
            UsingGuide(argc, argv);

        for (i = 1; i < argc; i++)
        {
            /* Output resolution */
            if (!strcmp(argv[i], "-loopback"))
            {
                SetParam->u16PortNo = strtol(argv[++i], &endptr, 10);
                SetParam->u16GenNo = strtol(argv[++i], &endptr, 10);
                if ((SetParam->u16PortNo > 1)  || ((SetParam->u16GenNo == 0) || (SetParam->u16GenNo >= 4)))
                {
                    fprintf(stderr, "Invalid loopback test parameter(port=%d, gen=%d).\n", SetParam->u16PortNo, SetParam->u16GenNo);
                    UsingGuide(argc, argv);
                }
                stSATATestProperty.u16PortNo = SetParam->u16PortNo;
                stSATATestProperty.u16GenNo = SetParam->u16GenNo;
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
        goto SATA_TEST_EXIT;

    if (!_SATAInit())
        goto SATA_TEST_EXIT;

    if (!_SATATest())
        goto SATA_TEST_EXIT;

SATA_TEST_EXIT:
    _SATADeInit();
    _SysDeInit();

    return 0;
}
