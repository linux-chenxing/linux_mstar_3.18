#include "mdrv_tsensor.h"
#include "mdrv_types.h"

#define REG_ARM_BASE                0xFD000000

#define MsOS_DelayTaskUs(x)				udelay(x)
#define MDrv_TSENSOR_REG(addr)         (*(volatile U8*)(REG_ARM_BASE + (((addr) & ~1) << 1) + (addr & 1)))
#define MDrv_TSENSOR_REG2(addr)        (*((volatile U16*)(REG_ARM_BASE + (addr << 1))))

static U16  _u16_25SARCode = 0;


static U8 MDrv_SAR_ReadByte(U32 u32RegAddr)
{
    return MDrv_TSENSOR_REG(u32RegAddr);
}


static U16 MDrv_SAR_Read2Byte(U32 u32RegAddr)
{
    return MDrv_TSENSOR_REG2(u32RegAddr);
}


static BOOL MDrv_SAR_WriteByte(U32 u32RegAddr, U8 u8Val)
{
    if (!u32RegAddr)
    {
        printk("%s reg error!\n", __FUNCTION__);
        return FALSE;
    }

    MDrv_TSENSOR_REG(u32RegAddr) = u8Val;

    return TRUE;
}


BOOL MDrv_TSensor_Init(void)
{
    BOOL bRet = TRUE;
    U16 u16VbeCodeFt = 0 ,u16VbeCode= 0;

    //Tsensor Initialization
    bRet &= MDrv_SAR_WriteByte(REG_SAR_ADC_REFV_SEL, MDrv_SAR_ReadByte(REG_SAR_ADC_REFV_SEL)&~_SAR_ADC_CH8_REFV_SEL); // 0:2.0V,  1:3.3V
    bRet &= MDrv_SAR_WriteByte(REG_SAR_PM_DMY, MDrv_SAR_ReadByte(REG_SAR_PM_DMY)&~BIT0); //select external t-sensor from CPU side

    bRet &= MDrv_SAR_WriteByte(REG_PM_TR, MDrv_SAR_ReadByte(REG_PM_TR)|BIT7); //select external t-sensor from CPU side
    bRet &= MDrv_SAR_WriteByte(REG_PM_TSENSOR_H, MDrv_SAR_ReadByte(REG_PM_TSENSOR_H)|_EN_TSEN); //enable t-sensor

    //Freerun mode
    bRet &= MDrv_SAR_WriteByte(REG_SAR_CTRL0, MDrv_SAR_ReadByte(REG_SAR_CTRL0)&~_SAR_SINGLE_CH_EN);
    bRet &= MDrv_SAR_WriteByte(REG_SAR_CTRL0, MDrv_SAR_ReadByte(REG_SAR_CTRL0)|_SAR_MODE);
    bRet &= MDrv_SAR_WriteByte(REG_SAR_CTRL1, MDrv_SAR_ReadByte(REG_SAR_CTRL1)|_SAR_FREERUN);
    bRet &= MDrv_SAR_WriteByte(REG_SAR_CTRL1, MDrv_SAR_ReadByte(REG_SAR_CTRL1)|_SAR_NCH_EN);

    //Power ON
    bRet &= MDrv_SAR_WriteByte(REG_SAR_CTRL1, MDrv_SAR_ReadByte(REG_SAR_CTRL1)&~_SAR_ADC_PD);// power on SAR_ATOP
    MsOS_DelayTaskUs(10);
    bRet &= MDrv_SAR_WriteByte(REG_SAR_CTRL0, MDrv_SAR_ReadByte(REG_SAR_CTRL0)&~_SAR_PD);// power on SAR_DTOP

    //Read SAR DATA of T-sensor
    bRet &= MDrv_SAR_WriteByte(REG_SAR_CTRL1, MDrv_SAR_ReadByte(REG_SAR_CTRL1)|_SAR_LOAD_EN);//Set this value to 1 before getting the value of SAR
    u16VbeCode = MDrv_SAR_Read2Byte(REG_SAR_ADC_CH8_DATA);
    u16VbeCode = u16VbeCode & _SAR_ADC_OUT_10BITMSK;

    //Read VBE_Code trimmed in FT
    u16VbeCodeFt = MDrv_SAR_Read2Byte(REG_VBE_CODE_FT);
    u16VbeCodeFt = (u16VbeCodeFt>>6) & _SAR_ADC_OUT_10BITMSK;
    //printk("u16VbeCodeFt %x\n",u16VbeCodeFt);

    MsOS_DelayTaskUs(100);

    _u16_25SARCode = u16VbeCodeFt;

    return bRet;
}

static S16 _SarToCelsius(U16 u16SarCode)
{
    return (S16)(5 * ((S16) _u16_25SARCode - (S16) u16SarCode) + 2) / 4 + 27;
}


S16 MDrv_TSensor_GetTemperature(void)
{
    U16 u16VbeCode = 0;
    S16 s16CurTemperature_C = 0;
    //Read SAR DATA of T-sensor
    MDrv_SAR_WriteByte(REG_SAR_CTRL1, MDrv_SAR_ReadByte(REG_SAR_CTRL1)|_SAR_LOAD_EN);//Set this value to 1 before getting the value of SAR
    u16VbeCode = MDrv_SAR_Read2Byte(REG_SAR_ADC_CH8_DATA);
    s16CurTemperature_C = _SarToCelsius(u16VbeCode);

    //printk("[TSNSR] Read current SAR code = 0x%x = %d C\n",u16VbeCode,s16CurTemperature_C);

    return s16CurTemperature_C;
}



int tsensor_open(struct inode *inode, struct file *fp)
{
    BOOL bRet = TRUE;

    bRet = MDrv_TSensor_Init();

    if(bRet == FALSE)
    {
        printk("open failed!\n");
        return -1;
    }

    return 0;
}

ssize_t tsensor_read(struct file *filp, char __user *buf, size_t count, loff_t *loff)
{
    if(buf == NULL)
    {
        printk("param error!\n");
        return -1;
    }

    S16 temp;
    int i;

    for(i = 0; i < 3; i++)
    {
        temp = MDrv_TSensor_GetTemperature();

        if(temp < 0)
            continue;
        else
            break;
    }

    if(i == 3)
    {
        printk("Get Temperature failed\n");
        return -1;
    }

    //printk("Get the Chip's Temperature : %d\n", temp);

    if(copy_to_user( (void *)buf, &temp, sizeof(temp) ))
    {
        printk( "copy_to_user error\n" );
        return -1;
    }

    printk("Get Temperature OK!\n");

    return 0;
}

ssize_t tsensor_write(struct file *filp, const char __user *buf, size_t count, loff_t *loff)
{

    return 0;
}

int tsensor_close(struct inode *inode, struct file *fp)
{
    return 0;
}


static struct file_operations dev_fops = {
	.owner			= THIS_MODULE,
	.open	        = tsensor_open,
	.read           = tsensor_read,
	.write          = tsensor_write,
	.release        = tsensor_close,
};

static struct miscdevice misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name  = MTD_DEV_NAME,
	.fops  = &dev_fops,
};

static int __init dev_init(void)
{
	int ret;

    printk("tsensor driver is invoked\n");

	ret = misc_register(&misc);

	return ret;
}

static void __exit dev_exit(void)
{
	misc_deregister(&misc);
}

module_init(dev_init);
module_exit(dev_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("aaron.guo");
