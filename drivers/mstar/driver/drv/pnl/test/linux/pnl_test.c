#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "cam_os_wrapper.h"
#include "mdrv_pnl_ioc_st.h"
#include "mdrv_pnl_ioc.h"

#define PNL_DEVICE_NAME	"/dev/ms_pnl"

static s32 nDevFd=-1;

static PnlConfig_t _gtPnlCfg[]=
{
    // LX50FWB4001_RM68172_LongXi-IVO_MST
    {
        {
            480,               // u16PanelWidth
            854,               // u16PanelHeight
            LCD_TYPE_RGBLCD,   // ubDevType
            LCD_RGB_ORDER_RGB, // Order
            LCD_RGB565,        // BusCfg
            LCD_BUS_WIDTH_8,   // bBusWidth
            0,                 // bBitReverse
        },
        .tRgbLcdAttr=
        {
            0,                    // u16HBPorch
            0,                    // u16HBlanking
            0,                    // u16VBPorch
            0,                    // u16VBlanking
            0,                    // u16HSyncW
            0,                    // u16HSyncW
            0,                    // u16VFreqx10
            LCD_SIG_POLARITY_H,   // ubDclkPor
            LCD_SIG_POLARITY_L,   // ubHsyncPor
            LCD_SIG_POLARITY_L,   // ubVsyncPor
            0,                    // bDeltaRBG
            0,                    // bDummyRBG
            0,                    // OddOrder
            0,                    // EvenOrder
        },
    },
};


static void DisplayMenuSetting(void)
{
    printf("===============================\n");
    printf("==            Menu           ==\n");
    printf("===============================\n");
    printf("=[r] Reset LCD panel\n");
    printf("=[s] Set LCD panel settings\n");
    //printf("=[g] Print the current LCD panel settings\n");
    printf("=[o] On/Off LCD panel output\n");
    printf("=[a] Read register\n");
    printf("=[w] Write register\n");
    printf("=[b] Write register only one bit\n");
    printf("=[q] Quit\n");
    printf("===============================\n");
    printf("==>");
}

static void LcdPanelReset(void)
{
    s32 nRet=0;

    if ((nRet = ioctl(nDevFd, IOCTL_PNL_RESET, NULL)) < 0)
        printf("ioctl %d fail, errno = %d\n", IOCTL_PNL_RESET, errno);
}

static void LcdPanelSetConfig(void)
{
    s32 nRet=0;
    char szCmd[64];
    pPnlConfig ptPnlCfg=NULL;

    printf("===============================\n");
    printf("==       LCD Panel Type      ==\n");
    printf("===============================\n");
    printf("=[1] LX50FWB4001_RM68172_LongXi-IVO_MST(RGB LCD)\n");
    printf("=[q] quit\n");
    printf("===============================\n");
    printf("==>");

    scanf("%s", szCmd);
    switch(szCmd[0])
    {
        case '1':
            ptPnlCfg = &_gtPnlCfg[0];
            break;
        case 'q':
            break;
        default:
            printf("Unsupport LCD panel type : %c\n", szCmd[0]);
            nRet = -1;
            break;
    }

    if (nRet < 0)
        return;

    if (ptPnlCfg != NULL && (nRet = ioctl(nDevFd, IOCTL_PNL_SET_CONFIG, ptPnlCfg)) < 0)
        printf("ioctl %d fail, errno = %d\n", IOCTL_PNL_SET_CONFIG, errno);
}

static void LcdPanelEnable(void)
{
    s32 nRet=0;
    char szCmd[64];
    u8 nEnable;

    printf("===============================\n");
    printf("==      LCD Panel Output     ==\n");
    printf("===============================\n");
    printf("=[n] On\n");
    printf("=[f] Off\n");
    printf("===============================\n");
    printf("==>");
    scanf("%s", szCmd);
    switch(szCmd[0])
    {
        case 'n':
            nEnable = 1;;
            break;
        case 'f':
            nEnable = 0;
            break;
        default:
            printf("Unsupport LCD panel output : %c\n", szCmd[0]);
            nRet = -1;
            break;
    }

    if (nRet < 0)
        return;

    if ((nRet = ioctl(nDevFd, IOCTL_PNL_ENABLE, &nEnable)) < 0)
        printf("ioctl %d fail, errno = %d\n", IOCTL_PNL_ENABLE, errno);
}

static void LcdPanelReadReg(void)
{
    s32 nRet=0;
    PnlReg_t tPnlReg={0};

    printf("Please enter register address(hex):\n");
    printf("==>");

    scanf("%x", &tPnlReg.nRegAddr);

    if ((nRet = ioctl(nDevFd, IOCTL_PNL_READ_REG, &tPnlReg)) < 0)
        printf("ioctl %d fail, errno = %d\n", IOCTL_PNL_READ_REG, errno);

    printf("Register value: 0x%x\n", tPnlReg.nRegVal);
}

static void LcdPanelWriteReg(void)
{
    s32 nRet=0;
    PnlReg_t tPnlReg;

    printf("Please enter register address(hex):\n");
    printf("==>");
    scanf("%x", &tPnlReg.nRegAddr);

    printf("Please enter register value(hex):\n");
    printf("==>");
    scanf("%x", &tPnlReg.nRegVal);

    if ((nRet = ioctl(nDevFd, IOCTL_PNL_WRITE_REG, &tPnlReg)) < 0)
        printf("ioctl %d fail, errno = %d\n", IOCTL_PNL_WRITE_REG, errno);
}

static void LcdPanelWriteRegBit(void)
{
    s32 nRet=0;
    PnlReg_t tPnlReg;

    printf("Please enter register address(hex):\n");
    printf("==>");
    scanf("%x", &tPnlReg.nRegAddr);

    printf("Please enter register value(0 or 1):\n");
    printf("==>");
    scanf("%x", &tPnlReg.nRegVal);

    printf("Please enter register mask(hex):\n");
    printf("==>");
    scanf("%x", &tPnlReg.nRegMask);

    if ((nRet = ioctl(nDevFd, IOCTL_PNL_WRITE_REG_BIT, &tPnlReg)) < 0)
        printf("ioctl %d fail, errno = %d\n", IOCTL_PNL_WRITE_REG_BIT, errno);
}

int main(int argc, char *argv[])
{
    s32 nOpt;
    char szCmd[64];

    if ((nDevFd = open(PNL_DEVICE_NAME, O_RDONLY)) < 0)
    {
        printf("%s open fail!\n", PNL_DEVICE_NAME);
        return -1;
    }
#if 0
    printf("IOCTL_PNL_RESET         = %x\n", IOCTL_PNL_RESET);
    printf("IOCTL_PNL_SET_CONFIG    = %x\n", IOCTL_PNL_SET_CONFIG);
    printf("IOCTL_PNL_ENABLE        = %x\n", IOCTL_PNL_ENABLE);
    printf("IOCTL_PNL_READ_REG      = %x\n", IOCTL_PNL_READ_REG);
    printf("IOCTL_PNL_WRITE_REG     = %x\n", IOCTL_PNL_WRITE_REG);
    printf("IOCTL_PNL_WRITE_REG_BIT = %x\n", IOCTL_PNL_WRITE_REG_BIT);
#endif
    do
    {
        DisplayMenuSetting();
        scanf("%s", szCmd);
        switch(szCmd[0])
        {
            case 'r':
                LcdPanelReset();
                break;
            case 's':
                LcdPanelSetConfig();
                break;
            case 'o':
                LcdPanelEnable();
                break;
            case 'a':
                LcdPanelReadReg();
                break;
            case 'w':
                //LcdPanelWriteReg();
                break;
            case 'b':
                //LcdPanelWriteRegBit();
                break;
            default:
                printf("Unsupported option: %c\n", szCmd[0]);
                break;
        }
        printf("\n");
    }
    while('q' != szCmd[0]);

    if (nDevFd > 0)
        close(nDevFd);

    return 0;
}
