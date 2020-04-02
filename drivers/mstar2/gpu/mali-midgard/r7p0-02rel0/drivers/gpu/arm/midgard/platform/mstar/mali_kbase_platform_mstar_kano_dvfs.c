/*
 *
 * (C) COPYRIGHT 2014-2015 MStar Semiconductor, Inc. All rights reserved.
 *
 * This program is free software and is provided to you under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation, and any use by you of this program is subject to the terms
 * of such GNU licence.
 *
 * A copy of the licence is included with the program, and can also be obtained
 * from Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 */

#ifdef CONFIG_MALI_MIDGARD_DVFS

#include <mali_kbase.h>
#include "mali_kbase_platform_mstar.h"

#include <linux/delay.h>
#include "mstar/mstar_chip.h"

#define MSTAR_ENABLE_VOLTAGE_SCALE 1

#ifdef MSTAR_ENABLE_VOLTAGE_SCALE
#include "mali_kbase_mstar_voltage_scale.h"
#else
#define adjust_voltage(voltage, freq) 0
#define voltage_scale_init(kbdev) 0
#define voltage_scale_term(kbdev)
#define mstar_platform_gpu_dynamic_power_init() 0
#endif

/* REG_GPU_PLL */
#define REG_GPU_PLL_BASE            0x160400
#define REG_SET_0                   0x60
#define REG_SET_1                   0x61
#define REG_SLD                     0x62
#define MSK_SLD                     0x0001

/* REG_MPLL */
#define REG_CLKGEN2_BASE            0x100a00
#define REG_MPLL                    0x3d
#define MSK_HW_MODE                 0x0010  //bit[4] (default=0)
#define MSK_SWITCH_FROM_G3DPLL      0x0008  //bit[3] (default=0)

/* RIU mapping */
#define GPU_PLL_REG(addr)           RIU[(REG_GPU_PLL_BASE) + ((addr) << 1)]
#define CLKGEN2_MPLL_REG(addr)      RIU[(REG_CLKGEN2_BASE) + ((addr) << 1)]

/* DVFS */
#define MHZ_TO_REG(MHZ)             (0x6c000000 / (MHZ))
#define CLK_REG_TO_MHZ(clk_reg)     (0x6c000000 / (clk_reg))

#ifdef MSTAR_ENABLE_VOLTAGE_SCALE
#define GPU_DVFS_NUM_SCALES         7
#define GPU_DVFS_DEFAULT_SCALE      4
#else
#define GPU_DVFS_NUM_SCALES         4
#define GPU_DVFS_DEFAULT_SCALE      3
#endif
#define GPU_DVFS_MAX_SCALE          (GPU_DVFS_NUM_SCALES - 1)
#define GPU_DVFS_SCALE_STEP         1
#define GPU_MIN_FREQ                400

extern int mali_gpu_clock;

/*************************************************************************************************/
// mhal_dvfs.h (\\perforce\theale\redlion_develop\3.10.40_lollipop_release\mstar2\hal\kano\dvfs)
#define CONFIG_DVFS_CORNER_CHIP_SS                      0x00
#define CONFIG_DVFS_CORNER_CHIP_TT                      0x01
#define CONFIG_DVFS_CORNER_CHIP_FF                      0x02
#define CONFIG_DVFS_CORNER_CHIP_MAX                     0x03
#define CONFIG_DVFS_CORNER_CHIP_UNKNOWN                 0xFF

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
typedef struct  __attribute__((packed))
{
    u32     reg_vid_dvfs_id: 16;        //0x1005_00[15:0]: Identify VID Only or VID with DVFS
    u32     reg_reserved_00: 16;        //0x1005_00[31:16]: Reserved

    u32     reg_cur_cpu_clock: 12;      //0x1005_02[11:0]: Current Value of CPU Clock
    u32     reg_reserved_01: 20;        //0x1005_02[31:12]: Reserved

    u32     reg_cur_cpu_temp: 8;        //0x1005_04[7:0]: Current Value of CPU Temperature
    u32     reg_reserved_02: 24;        //0x1005_04[31:8]: Reserved

    u32     reg_vid_enable: 1;          //0x1005_06[0]: Enable VID Flow
    u32     reg_dvfs_enable: 1;         //0x1005_06[1]: Enable DVFS Flow
    u32     reg_chip_package: 2;        //0x1005_06[3:2]: Chip Package Type
    u32     reg_cpu_pwr_type: 2;        //0x1005_06[5:4]: CPU Power Type
    u32     reg_core_pwr_type: 2;       //0x1005_06[7:6]: Core Power Type
    u32     reg_cpu_pwr_ctl: 2;         //0x1005_06[9:8]: IO Control Mode for CPU Power
    u32     reg_core_pwr_ctl: 2;        //0x1005_06[11:10]: IO Control Mode for Core Power
    u32     reg_cur_dvfs_state: 3;      //0x1005_06[14:12]: Current DVFS Running State
    u32     reg_reserved_03_1: 17;      //0x1005_06[31:15]: Reserved

    u32     reg_low_high_temp_en: 1;    //0x1005_08[0]: Enable Lower/Upper Level Threshold of Temperature
    u32     reg_max_temp_en: 1;         //0x1005_08[1]: Enable Maximum Level Threshold of Temperature
    u32     reg_reserved_04_0: 6;       //0x1005_08[7:2]: Reserved
    u32     reg_low_level_temp: 8;      //0x1005_08[15:8]: Lower Level Threshold of Temperature
    u32     reg_reserved_04_1: 16;      //0x1005_08[31:16]: Reserved

    u32     reg_high_level_temp: 8;     //0x1005_0A[7:0]: Upper Level Threshold of Temperature
    u32     reg_max_level_temp: 8;      //0x1005_0A[15:8]: Maximum Level Threshold of Temperature
    u32     reg_reserved_05: 16;        //0x1005_0A[31:0]: Reserved

    u32     reg_reserved_06: 32;        //0x1005_0C[31:0]: Reserved

    u32     reg_special_cpu_clk: 16;    //0x1005_0E[15:0]: User-defined Specific CPU Clock
    u32     reg_reserved_07: 16;        //0x1005_0E[31:16]: Reserved

} MSTAR_DVFS_REG_INFO;

static volatile MSTAR_DVFS_REG_INFO *DvfsRegInfo = 0;

/*************************************************************************************************/

enum
{
    FREQ = 0,
    VOLTAGE = 1
};

#ifdef MSTAR_ENABLE_VOLTAGE_SCALE
#define CLKGEN2_MPLL_216MHZ    216
static u32 vf_table[CONFIG_DVFS_CORNER_CHIP_MAX+1][GPU_DVFS_NUM_SCALES][2] =
{
    {  //Table 1: CONFIG_DVFS_CORNER_CHIP_SS
        {MHZ_TO_REG(216), 90}, /* MPLL working freq */
        {MHZ_TO_REG(400), 98}, /* GPLL Minimum working freq */
        {MHZ_TO_REG(450), 98},
        {MHZ_TO_REG(500), 98},
        {MHZ_TO_REG(550), 98},  /* default */
        {MHZ_TO_REG(600), 100},
        {MHZ_TO_REG(650), 100},
    },
    {  //Table 1: CONFIG_DVFS_CORNER_CHIP_TT
        {MHZ_TO_REG(216), 90}, /* MPLL working freq */
        {MHZ_TO_REG(400), 98}, /* GPLL Minimum working freq */
        {MHZ_TO_REG(450), 98},
        {MHZ_TO_REG(500), 98},
        {MHZ_TO_REG(550), 98},  /* default */
        {MHZ_TO_REG(600), 100},
        {MHZ_TO_REG(650), 100},
     },
    {  //Table 2: CONFIG_DVFS_CORNER_CHIP_FF
        {MHZ_TO_REG(216), 88}, /* MPLL working freq */
        {MHZ_TO_REG(400), 93}, /* GPLL Minimum working freq */
        {MHZ_TO_REG(450), 93},
        {MHZ_TO_REG(500), 93},
        {MHZ_TO_REG(550), 93},  /* default */
        {MHZ_TO_REG(600), 98},
        {MHZ_TO_REG(650), 98},
     },
    {  //Table 2
        {MHZ_TO_REG(216), 88}, /* MPLL working freq */
        {MHZ_TO_REG(400), 93}, /* GPLL Minimum working freq */
        {MHZ_TO_REG(450), 93},
        {MHZ_TO_REG(500), 93},
        {MHZ_TO_REG(550), 93},  /* default */
        {MHZ_TO_REG(600), 98},
        {MHZ_TO_REG(650), 98},
     }
};
#else
static u32 vf_table[1][GPU_DVFS_NUM_SCALES][2] =
{
    {
        {MHZ_TO_REG(400), 98}, /* GPLL Minimum working freq */
        {MHZ_TO_REG(450), 98},
        {MHZ_TO_REG(500), 98},
        {MHZ_TO_REG(550), 98},  /* default */
    }
};
#endif //MSTAR_ENABLE_VOLTAGE_SCALE

#if defined(MSTAR_ENABLE_VOLTAGE_SCALE) && defined(CONFIG_MSTAR_IIC)

#define CONFIG_DVFS_POWER_SWI2C_BUS             0x01
#define CONFIG_DVFS_POWER_SWI2C_GPU_ID          0x80
#define CONFIG_DVFS_POWER_SWI2C_ADDR_GPU        ((CONFIG_DVFS_POWER_SWI2C_BUS << 8) | CONFIG_DVFS_POWER_SWI2C_GPU_ID)

#define CONFIG_DVFS_CHIP_ID_UNKNOWN             0xFF
#define CONFIG_DVFS_CHIP_ID_PRADA               0x79
#define CONFIG_DVFS_CHIP_ID_PRADO               0x9A

#define CONFIG_DVFS_GPU_POWER_SHIFT_PRADO       (69 - 2) /* FIXME: exact value to GPU? */

extern int MDrv_SW_IIC_Write(u8 u8ChIIC, u8 u8SlaveID, u8 u8AddrCnt, u8* pu8Addr, u32 u32BufLen, u8* pu8Buf);
extern int MDrv_SW_IIC_Read(u8 u8ChIIC, u8 u8SlaveID, u8 u8AddrCnt, u8* pu8Addr, u32 u32BufLen, u8* pu8Buf);

static u32 power_chip_id = CONFIG_DVFS_CHIP_ID_UNKNOWN;

static int mstar_platform_gpu_dynamic_power_init(void)
{
    int init_status = 1;

    u8 targetRegAddress[5] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    u8 targetData[5] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

    targetRegAddress[0] = 0x53;
    targetRegAddress[1] = 0x45;
    targetRegAddress[2] = 0x52;
    targetRegAddress[3] = 0x44;
    targetRegAddress[4] = 0x42;

    if (MDrv_SW_IIC_Write(CONFIG_DVFS_POWER_SWI2C_BUS, CONFIG_DVFS_POWER_SWI2C_GPU_ID, 5, targetRegAddress, 0, targetData) == -1)
    {
        printk("[ERROR] I2C_Enter_I2C Failed\n");
        init_status = 0;
    }

    targetRegAddress[0] = 0x7F;

    if (MDrv_SW_IIC_Write(CONFIG_DVFS_POWER_SWI2C_BUS, CONFIG_DVFS_POWER_SWI2C_GPU_ID, 1, targetRegAddress, 0, targetData) == -1)
    {
        printk("[ERROR] I2C_USE_CFG Failed\n");
        init_status = 0;
    }

    targetRegAddress[0] = 0x7D;

    if (MDrv_SW_IIC_Write(CONFIG_DVFS_POWER_SWI2C_BUS, CONFIG_DVFS_POWER_SWI2C_GPU_ID, 1, targetRegAddress, 0, targetData) == -1)
    {
        printk("[ERROR] I2C_OUT_NO_DELAY Failed\n");
        init_status = 0;
    }

    targetRegAddress[0] = 0x50;

    if (MDrv_SW_IIC_Write(CONFIG_DVFS_POWER_SWI2C_BUS, CONFIG_DVFS_POWER_SWI2C_GPU_ID, 1, targetRegAddress, 0, targetData) == -1)
    {
        printk("[ERROR] I2C_AD_BYTE_EN0 Failed\n");
        init_status = 0;
    }

    targetRegAddress[0] = 0x55;

    if (MDrv_SW_IIC_Write(CONFIG_DVFS_POWER_SWI2C_BUS, CONFIG_DVFS_POWER_SWI2C_GPU_ID, 1, targetRegAddress, 0, targetData) == -1)
    {
        printk("[ERROR] I2C_DA_BYTE_EN1 Failed\n");
        init_status = 0;
    }

    targetRegAddress[0] = 0x35;

    if (MDrv_SW_IIC_Write(CONFIG_DVFS_POWER_SWI2C_BUS, CONFIG_DVFS_POWER_SWI2C_GPU_ID, 1, targetRegAddress, 0, targetData) == -1)
    {
        printk("[ERROR] I2C_USE Failed\n");
        init_status = 0;
    }

    targetRegAddress[0] = 0x10;
    targetRegAddress[1] = 0xc0;

    if (MDrv_SW_IIC_Read(CONFIG_DVFS_POWER_SWI2C_BUS, CONFIG_DVFS_POWER_SWI2C_GPU_ID, 2, targetRegAddress, 2, targetData) != -1)
    {
        printk("[INFO] MStar Power IC Chip ID: %x%x\n", (unsigned int) targetData[0], (unsigned int) targetData[1]);
        power_chip_id = (unsigned int) targetData[1];
    }
    else
    {
        init_status = 0;
    }

    if (power_chip_id == CONFIG_DVFS_CHIP_ID_PRADO)
    {
        //Set OTP Level
        targetRegAddress[0] = 0x10;
        targetRegAddress[1] = (0x05 << 1);
        targetRegAddress[2] = 0x40;
        targetRegAddress[3] = 0x00;

        if(MDrv_SW_IIC_Write(CONFIG_DVFS_POWER_SWI2C_BUS, CONFIG_DVFS_POWER_SWI2C_GPU_ID, 4, targetRegAddress, 0, targetData) == -1)
        {
            printk("[ERROR] Software I2C Write Failed\n");
            init_status = 0;
        }

        //Set Password
        targetRegAddress[0] = 0x10;
        targetRegAddress[1] = (0x0C << 1);
        targetRegAddress[2] = 0xbe;
        targetRegAddress[3] = 0xaf;

        if (MDrv_SW_IIC_Write(CONFIG_DVFS_POWER_SWI2C_BUS, CONFIG_DVFS_POWER_SWI2C_GPU_ID, 4, targetRegAddress, 0, targetData) == -1)
        {
            printk("[ERROR] Software I2C Write Failed\n");
            init_status = 0;
        }
    }
    else
    {
        printk("[ERROR] PowerChip ID: 0x%X  is not PRADO\n", power_chip_id);
        init_status = 0;
    }

    return init_status;
}

static bool adjust_voltage(u32 target_voltage, u32 freq)
{
    bool bRet = true;
    u32 registerValue = 0;
    u8 targetRegAddress[5] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
    u8 targetData[5] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

    if (power_chip_id == CONFIG_DVFS_CHIP_ID_PRADO)
    {
        /* Set Core Voltage */
        registerValue = (target_voltage - CONFIG_DVFS_GPU_POWER_SHIFT_PRADO);
        targetRegAddress[0] = 0x10;
        targetRegAddress[1] = (0x06 << 1);
        targetRegAddress[2] = 0x10;
        targetRegAddress[3] = registerValue;

        if (MDrv_SW_IIC_Write(CONFIG_DVFS_POWER_SWI2C_BUS,
                            CONFIG_DVFS_POWER_SWI2C_GPU_ID,
                            4,
                            targetRegAddress,
                            0,
                            targetData) == true)
        {
            printk("[Mali DVFS] Adj Voltage to: %u, reg %X\n", target_voltage, registerValue);
        }
    }

    return bRet;
}
#endif //defined(MSTAR_ENABLE_VOLTAGE_SCALE) && defined(CONFIG_MSTAR_IIC)

#ifdef MSTAR_ENABLE_VOLTAGE_SCALE
static void switch_freq_from_G3DPLL_to_MPLL216(u32 clk_reg)
{
    CLKGEN2_MPLL_REG(REG_MPLL) &= ~MSK_HW_MODE;
    CLKGEN2_MPLL_REG(REG_MPLL) |= MSK_SWITCH_FROM_G3DPLL;
    udelay(1);
    mali_gpu_clock = CLK_REG_TO_MHZ(clk_reg);
    //assert (mali_gpu_clock == CLKGEN2_MPLL_216MHZ);
}

static void switch_freq_from_MPLL216_to_G3DPLL(u32 clk_reg)
{
    CLKGEN2_MPLL_REG(REG_MPLL) &= ~(MSK_HW_MODE | MSK_SWITCH_FROM_G3DPLL);
    udelay(1);
}
#endif //#ifdef MSTAR_ENABLE_VOLTAGE_SCALE

static void adjust_frequency(struct kbase_device* kbdev, u32 clk_reg)
{
    if (!kbdev->dev)
    {
        return;
    }

    /* dev_info(kbdev->dev, "%s: target:%x\n", __func__, clk_reg); */

    /* GPU clock */
    GPU_PLL_REG(REG_SET_0) = clk_reg & 0xffff;
    GPU_PLL_REG(REG_SET_1) = (clk_reg >> 16) & 0xffff;
    GPU_PLL_REG(REG_SLD) |= MSK_SLD;
    udelay(1);

    mali_gpu_clock = CLK_REG_TO_MHZ(clk_reg);
}

/* DVFS callbacks */
static bool dvfs_enable(struct kbase_device* kbdev)
{
    if (!kbdev)
    {
        return false;
    }

    kbdev->dvfs.enabled = 1;

    return true;
}

static bool dvfs_disable(struct kbase_device* kbdev)
{
    u32 current_scale;
    u32 default_freq_reg;
    u32 default_voltage;

    if (!kbdev)
    {
        return false;
    }

    current_scale = kbdev->dvfs.current_scale;
    default_freq_reg = vf_table[DvfsRegInfo->reg_cpu_pwr_type][GPU_DVFS_DEFAULT_SCALE][FREQ];
    default_voltage = vf_table[DvfsRegInfo->reg_cpu_pwr_type][GPU_DVFS_DEFAULT_SCALE][VOLTAGE];

    /* restore frequency and voltage to default */
    if (current_scale > GPU_DVFS_DEFAULT_SCALE)
    {
        /* decrease frequency first */
        adjust_frequency(kbdev, default_freq_reg);
        adjust_voltage(default_voltage, CLK_REG_TO_MHZ(default_freq_reg));
    }
    else if (current_scale < GPU_DVFS_DEFAULT_SCALE)
    {
        /* increase voltage first */
#ifdef MSTAR_ENABLE_VOLTAGE_SCALE
        if (current_scale == 0)  // 216 MHz to 400 MHz
        {
            switch_freq_from_MPLL216_to_G3DPLL(default_freq_reg);
        }
#endif
        adjust_voltage(default_voltage, CLK_REG_TO_MHZ(default_freq_reg));
        adjust_frequency(kbdev, default_freq_reg);
    }

    kbdev->dvfs.enabled = 0;
    kbdev->dvfs.current_scale = default_freq_reg;

#if 0
    dev_info(kbdev->dev,
             "%s: disable DVFS, adjust GPU Freq scale from %u -> %u scale\n",
             __func__, current_scale, GPU_DVFS_DEFAULT_SCALE);
#endif

    return false;
}

static bool dvfs_clock_up(struct kbase_device* kbdev)
{
    u32 current_scale;
    u32 current_voltage;
    u32 target_scale;
    u32 target_freq_reg;
    u32 target_voltage;
    bool bVoltageAdjusted = true;

    if (!kbdev)
    {
        return false;
    }

    current_scale = kbdev->dvfs.current_scale;
    target_scale = current_scale + GPU_DVFS_SCALE_STEP;

    if (current_scale >= GPU_DVFS_MAX_SCALE)
    {
        return false;
    }

    /* frequency and voltage lookup */
    current_voltage = vf_table[DvfsRegInfo->reg_cpu_pwr_type][current_scale][VOLTAGE];
    target_freq_reg = vf_table[DvfsRegInfo->reg_cpu_pwr_type][target_scale][FREQ];
    target_voltage = vf_table[DvfsRegInfo->reg_cpu_pwr_type][target_scale][VOLTAGE];

    /* Increase to target voltage first */
    if (target_voltage != current_voltage)
    {
        bVoltageAdjusted = adjust_voltage(target_voltage, CLK_REG_TO_MHZ(target_freq_reg));
    }

    /* [Maximum Frequency] DFS: 550 MHz; DVFS: 650 MHz */
    if ((target_scale > GPU_DVFS_DEFAULT_SCALE) && (bVoltageAdjusted == false))
        return false;

    /* then perform GPU clockup */
#ifdef MSTAR_ENABLE_VOLTAGE_SCALE
    if (current_scale == 0)  // 216 MHz to 400 MHz
    {
        switch_freq_from_MPLL216_to_G3DPLL(target_freq_reg);
    }
#endif
    adjust_frequency(kbdev, target_freq_reg);

    kbdev->dvfs.current_scale = target_scale;

#if 0
    dev_info(kbdev->dev, "%s: clockup GPU Freq from %u -> %u scale\n", __func__, current_scale, target_scale);
#endif

    return true;
}

static bool dvfs_clock_down(struct kbase_device* kbdev)
{
    u32 current_scale;
    u32 current_voltage;
    u32 target_scale;
    u32 target_freq_reg;
    u32 target_voltage;

    if (!kbdev)
    {
        return false;
    }

    current_scale = kbdev->dvfs.current_scale;

    if (current_scale == 0)
    {
        return true;
    }

    if (current_scale < GPU_DVFS_SCALE_STEP)
    {
        target_scale = 0;
    }
    else
    {
        target_scale = current_scale - GPU_DVFS_SCALE_STEP;
    }

    /* frequency and voltage lookup */
    current_voltage = vf_table[DvfsRegInfo->reg_cpu_pwr_type][current_scale][VOLTAGE];
    target_freq_reg = vf_table[DvfsRegInfo->reg_cpu_pwr_type][target_scale][FREQ];
    target_voltage = vf_table[DvfsRegInfo->reg_cpu_pwr_type][target_scale][VOLTAGE];

    /* Perform GPU clockdown */
#ifdef MSTAR_ENABLE_VOLTAGE_SCALE
    if (target_scale == 0)  // 400 MHz to 216 MHz, ignore GPLL setting
    {
        switch_freq_from_G3DPLL_to_MPLL216(target_freq_reg);
    }
    else
#endif
    {
        adjust_frequency(kbdev, target_freq_reg);
    }
    /* then decrease voltage level */
    if (target_voltage != current_voltage)
    {
        adjust_voltage(target_voltage, CLK_REG_TO_MHZ(target_freq_reg));
    }

    kbdev->dvfs.current_scale = target_scale;

#if 0
    dev_info(kbdev->dev, "%s: clockdown GPU Freq from %u -> %u scale\n", __func__, current_scale, target_scale);
#endif

    return true;
}

/* */
int mstar_platform_dvfs_init(struct kbase_device* kbdev)
{
    int err = 0;

    //Refer to  mhal_dvfs.c (\\perforce\theale\redlion_develop\3.10.40_lollipop_release\mstar2\hal\kano\dvfs)
    if(DvfsRegInfo == NULL)
    {
        DvfsRegInfo = (volatile MSTAR_DVFS_REG_INFO *)(MSTAR_RIU_BASE + (0x100500 << 1));
        dev_info(kbdev->dev, "[%s] Get GPU Power Type Info: %d\n", __func__, DvfsRegInfo->reg_cpu_pwr_type);
    }

#ifndef MSTAR_ENABLE_VOLTAGE_SCALE
    DvfsRegInfo->reg_cpu_pwr_type = 0;
#endif

    memset(&(kbdev->dvfs), 0, sizeof(struct mstar_dvfs));

    dev_info(kbdev->dev, "[%s] register DVFS callbacks\n" ,__func__);

    kbdev->dvfs.current_scale = GPU_DVFS_DEFAULT_SCALE;
    kbdev->dvfs.enable = dvfs_enable;
    kbdev->dvfs.disable = dvfs_disable;
    kbdev->dvfs.clock_up = dvfs_clock_up;
    kbdev->dvfs.clock_down = dvfs_clock_down;

    /* Kano do not support boost mode*/
    kbdev->dvfs.boost_enabled = 0;
    kbdev->dvfs.enable_boost = NULL;
    kbdev->dvfs.disable_boost = NULL;

    err = voltage_scale_init(kbdev);
    if (err != 0)
        return err;

    err = mstar_platform_gpu_dynamic_power_init();
    if (err != 0)
    {
        voltage_scale_term(kbdev);
        return err;
    }

    dvfs_enable(kbdev);

    return 0;
}

void mstar_platform_dvfs_term(struct kbase_device* kbdev)
{
    voltage_scale_term(kbdev);
}

#endif /* CONFIG_MALI_MIDGARD_DVFS */
