/*
 * Carsten Langgaard, carstenl@mips.com
 * Copyright (C) 1999,2000 MIPS Technologies, Inc.  All rights reserved.
 *
 *  This program is free software; you can distribute it and/or modify it
 *  under the terms of the GNU General Public License (Version 2) as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
 *
 * PROM library functions for acquiring/using memory descriptors given to
 * us from the YAMON.
 */
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/bootmem.h>
#include <linux/pfn.h>
#include <linux/string.h>
#include <linux/module.h>
#include <linux/binfmts.h>
//#include <asm/bootinfo.h>
#include <asm/page.h>
#include <asm/sections.h>
#include <asm/arm-boards/prom.h>
#include <Board.h> // #include "../titania2/board/Board.h"
/*#define DEBUG*/
#include <mach/io.h>
#include <mach/memory.h>
#include <linux/version.h>
#include <mstar/mpatch_macro.h>
#include <mstar/mstar_chip.h>
#include "mdrv_types.h"

enum yamon_memtypes {
    yamon_dontuse,
    yamon_prom,
    yamon_free,
};
//static struct prom_pmemblock mdesc[PROM_MAX_PMEMBLOCKS];

#ifdef DEBUG
static char *mtypes[3] = {
    "Dont use memory",
    "YAMON PROM memory",
    "Free memmory",
};
#endif

/* determined physical memory size, not overridden by command line args  */
unsigned long physical_memsize = 0L;
#if 0
static struct prom_pmemblock * __init prom_getmdesc(void)
{
    char *memsize_str;
    unsigned int memsize;
    char cmdline[CL_SIZE], *ptr;

    /* otherwise look in the environment */
    memsize_str = prom_getenv("memsize");
    if (!memsize_str) {
        printk(KERN_WARNING
               "memsize not set in boot prom, set to default (32Mb)\n");
        physical_memsize = 0x02000000;
    } else {
#ifdef DEBUG
        pr_debug("prom_memsize = %s\n", memsize_str);
#endif
        physical_memsize = simple_strtol(memsize_str, NULL, 0);
    }

#ifdef CONFIG_CPU_BIG_ENDIAN
    /* SOC-it swaps, or perhaps doesn't swap, when DMA'ing the last
       word of physical memory */
    physical_memsize -= PAGE_SIZE;
#endif

    /* Check the command line for a memsize directive that overrides
       the physical/default amount */
    strcpy(cmdline, arcs_cmdline);
    ptr = strstr(cmdline, "memsize=");
    if (ptr && (ptr != cmdline) && (*(ptr - 1) != ' '))
        ptr = strstr(ptr, " memsize=");

    if (ptr)
        memsize = memparse(ptr + 8, &ptr);
    else
        memsize = physical_memsize;

    memset(mdesc, 0, sizeof(mdesc));

    mdesc[0].type = yamon_dontuse;
    mdesc[0].base = 0x00000000;
    mdesc[0].size = 0x00001000;

    mdesc[1].type = yamon_prom;
    mdesc[1].base = 0x00001000;
    mdesc[1].size = 0x000ef000;

#ifdef CONFIG_MIPS_MALTA
    /*
     * The area 0x000f0000-0x000fffff is allocated for BIOS memory by the
     * south bridge and PCI access always forwarded to the ISA Bus and
     * BIOSCS# is always generated.
     * This mean that this area can't be used as DMA memory for PCI
     * devices.
     */
    mdesc[2].type = yamon_dontuse;
    mdesc[2].base = 0x000f0000;
    mdesc[2].size = 0x00010000;
#else
    mdesc[2].type = yamon_prom;
    mdesc[2].base = 0x000f0000;
    mdesc[2].size = 0x00010000;
#endif

    mdesc[3].type = yamon_dontuse;
    mdesc[3].base = 0x00100000;
    mdesc[3].size = CPHYSADDR(PFN_ALIGN((unsigned long)&_end)) - mdesc[3].base;

    mdesc[4].type = yamon_free;
    mdesc[4].base = CPHYSADDR(PFN_ALIGN(&_end));
    mdesc[4].size = memsize - mdesc[4].base;

    return &mdesc[0];
}

static int __init prom_memtype_classify(unsigned int type)
{
    switch (type) {
    case yamon_free:
        return BOOT_MEM_RAM;
    case yamon_prom:
        return BOOT_MEM_ROM_DATA;
    default:
        return BOOT_MEM_RESERVED;
    }
}
#endif

//DRAMlen is the length of DRAM mapping area,not the actual length of DRAM
static unsigned long LXmem=0, LXmem2Addr=0,LXmem2Size=0,EMACmem=0, DRAMlen=0, BBAddr=0;
static unsigned long G3Dmem0Addr=0, G3Dmem0Len=0, G3Dmem1Addr=0, G3Dmem1Len=0, G3DCmdQAddr=0, G3DCmdQLen=0;
static unsigned long LXmem3Addr=0,LXmem3Size=0;
static unsigned long GMACmemAddr=0,GMACmemLen=0;
static unsigned long  Miu01_adj=0;
static char coredump_path[CORENAME_MAX_SIZE]={0};
#ifdef CONFIG_MSTAR_STR_CRC
static unsigned int str_crc=0;
#endif
#ifdef CONFIG_MP_R2_STR_ENABLE
static volatile unsigned long R2TEE_STR_HANDSHAKE_addr=0;
#endif

unsigned long lx_mem_addr = PHYS_OFFSET;
unsigned long lx_mem_size = INVALID_PHY_ADDR; //default setting
unsigned long lx_mem2_addr = INVALID_PHY_ADDR; //default setting
unsigned long lx_mem2_size = INVALID_PHY_ADDR; //default setting
unsigned long lx_mem3_addr = INVALID_PHY_ADDR; //default setting
unsigned long lx_mem3_size = INVALID_PHY_ADDR; //default setting
unsigned long lx_mem_left_size = INVALID_PHY_ADDR; // to avoid overflow in va2pa
unsigned long lx_mem2_left_size = INVALID_PHY_ADDR; // to avoid overflow in va2pa
unsigned long miu01_adj = 0x0; //default setting
unsigned long  ulPM_GPIO_NUM=0;
unsigned long  ulSD_PAD_NUM=0;
unsigned long ulSD_CONFIG_NUM=0;
unsigned long ulSDIO_CONFIG_NUM=0;
char UTOPIA_MODE[10];
int TEEINFO_TYPTE =0;


EXPORT_SYMBOL(lx_mem_addr);
EXPORT_SYMBOL(lx_mem_size);
EXPORT_SYMBOL(lx_mem2_addr);
EXPORT_SYMBOL(lx_mem2_size);
EXPORT_SYMBOL(lx_mem3_addr);
EXPORT_SYMBOL(lx_mem3_size);
EXPORT_SYMBOL(lx_mem_left_size);
EXPORT_SYMBOL(lx_mem2_left_size);
EXPORT_SYMBOL(miu01_adj);
EXPORT_SYMBOL(ulPM_GPIO_NUM);
EXPORT_SYMBOL(ulSD_PAD_NUM);
EXPORT_SYMBOL(ulSD_CONFIG_NUM);
EXPORT_SYMBOL(ulSDIO_CONFIG_NUM);
EXPORT_SYMBOL(UTOPIA_MODE);
EXPORT_SYMBOL(TEEINFO_TYPTE);

#ifdef CONFIG_MP_MM_MALI_RESERVE
unsigned long mali_reserve_size_miu0 = 0;
unsigned long mali_reserve_size_miu1 = 0;

//extern unsigned long mali_reserve_level;
#endif

phys_addr_t MIU1_BASE = MSTAR_MIU1_BUS_BASE_L;
EXPORT_SYMBOL(MIU1_BASE);
void modify_miu_info(phys_addr_t base){
	if(base > 0x1FFFFFFFF){ //8G
		MIU1_BASE = MSTAR_MIU1_BUS_BASE_H;
		printk("MSTAR_MIU1_BUS_BASE %pa \n",&MIU1_BASE);
	}
}

static int SD_CONFIG_NUM_setup(char *str)
{
    printk("SD_CONFIG_NUM = %s\n", str);
    if( str != NULL )
    {
        sscanf(str,"%ld",&ulSD_CONFIG_NUM);
    }
    else
    {
        printk("\nSD_CONFIG_NUM not set, and it will be zero.\n");
    }
    return 0;
}
static int SDIO_CONFIG_NUM_setup(char *str)
{
    printk("SDIO_CONFIG_NUM = %s\n", str);
    if( str != NULL )
    {
        sscanf(str,"%ld",&ulSDIO_CONFIG_NUM);
    }
    else
    {
        printk("\nSDIO_CONFIG_NUM not set, and it will be zero.\n");
    }
    return 0;
}
static int __init LX_MEM_setup(char *str)
{
    //printk("LX_MEM= %s\n", str);
    if( str != NULL )
    {
        sscanf(str,"%lx",&LXmem);
		lx_mem_size = LXmem;
    }
    else
    {
        printk("\nLX_MEM not set\n");
    }
    return 0;
}

static int MIU01_ADJ_setup(char *str)
{
    printk("MIU01_ADJ = %s\n", str);
    if( str != NULL )
    {
        sscanf(str,"%ld",&Miu01_adj);
        miu01_adj = Miu01_adj * 1024 * 1024;
    }
    else
    {
        printk("\nMIU01_ADJ not set, and it will be zero.\n");
    }
    return 0;
}

static int PM_GPIO_NUM_setup(char *str)
{
    printk("PM_GPIO_NUM = %s\n", str);
    if( str != NULL )
    {
        sscanf(str,"%ld",&ulPM_GPIO_NUM);
    }
    else
    {
        printk("\nPM_GPIO_NUM not set, and it will be zero.\n");
    }
    return 0;
}

static int SD_PAD_NUM_setup(char *str)
{
    printk("SD_PAD_NUM_setup = %s\n", str);
    if( str != NULL )
    {
        sscanf(str,"%ld",&ulSD_PAD_NUM);
    }
    else
    {
        printk("\nSD_PAD_NUM not set, and it will be zero.\n");
    }
    return 0;
}

#ifdef CONFIG_MP_R2_STR_ENABLE
static int __init R2TEE_STR_ADDR_setup(char *str)
{
    if( str != NULL )
    {
        sscanf(str,"%lx",&R2TEE_STR_HANDSHAKE_addr);
        printk("\nR2TEE STR handshake_addr = 0x%lx\n",R2TEE_STR_HANDSHAKE_addr);
    }
    else
    {
        printk("\nR2TEE STR handshake_addr not set\n");
    }
    return 0;
}
#endif

int __init TEE_MODE_INFO_setup(char *cmdline)
{

    if(cmdline == NULL)
    {
        printk("Warning: tee_mode setup error\n");
    }
    else
    {
        strncpy(UTOPIA_MODE, cmdline, strlen(cmdline));
        if(!strcmp(UTOPIA_MODE,"optee"))
        {
            TEEINFO_TYPTE = SECURITY_TEEINFO_OSTYPE_OPTEE;
        }
        else if(!strcmp(UTOPIA_MODE,"nuttx"))
        {
            TEEINFO_TYPTE = SECURITY_TEEINFO_OSTYPE_NUTTX;
        }
        else if(!strcmp(UTOPIA_MODE,"secarm"))
        {
            TEEINFO_TYPTE = SECURITY_TEEINFO_OSTYPE_SECARM;
        }
        else
        {
            TEEINFO_TYPTE = SECURITY_TEEINFO_OSTYPE_NOTEE;
        }
	}
    return 0;
}


static int __init EMAC_MEM_setup(char *str)
{
   //printk("EMAC_MEM= %s\n", str);
    if( str != NULL )
    {
        EMACmem = simple_strtol(str, NULL, 16);
    }
    else
    {
        printk("\nEMAC_MEM not set\n");
    }
    return 0;
}

static int __init DRAM_LEN_setup(char *str)
{
    //printk("DRAM_LEN= %s\n", str);
    if( str != NULL )
    {
        DRAMlen = simple_strtol(str, NULL, 16);
    }
    else
    {
        printk("\nDRAM_LEN not set\n");
    }
    return 0;
}

static int __init LX_MEM2_setup(char *str)
{
    //printk("LX_MEM2= %s\n", str);
    if( str != NULL )
    {
        sscanf(str,"%lx,%lx",&LXmem2Addr,&LXmem2Size);
        lx_mem2_addr = LXmem2Addr ;
        lx_mem2_size = LXmem2Size;
		modify_miu_info(lx_mem2_addr);
    }
    else
    {
        printk("\nLX_MEM2 not set\n");
    }
    return 0;
}

static int __init LX_MEM3_setup(char *str)
{
    //printk("LX_MEM3= %s\n", str);
    if( str != NULL )
    {
        sscanf(str,"%lx,%lx",&LXmem3Addr,&LXmem3Size);
        lx_mem3_addr = LXmem3Addr ;
        lx_mem3_size = LXmem3Size;
		modify_miu_info(lx_mem3_addr);
    }
    else
    {
        printk("\nLX_MEM3 not set\n");
    }
    return 0;
}

static int __init G3D_MEM_setup(char *str)
{
    if( str != NULL )
    {
        sscanf(str,"%lx,%lx,%lx,%lx,%lx,%lx",
        &G3Dmem0Addr,&G3Dmem0Len,&G3Dmem1Addr,&G3Dmem1Len,
        &G3DCmdQAddr,&G3DCmdQLen);
    }
    else
    {
        printk("\nG3D_MEM not set\n");
    }
    return 0;
}


static int __init GMAC_MEM_setup(char *str)
{
    printk("GMAC_MEM= %s\n", str);
    if( str != NULL )
    {
        sscanf(str,"%lx,%lx",&GMACmemAddr,&GMACmemLen);
    }
    else
    {
        printk("\nGMAC_MEM not set\n");
    }
    return 0;
}
static int __init BBAddr_setup(char *str)
{
    //printk("LX_MEM2= %s\n", str);
    if( str != NULL )
    {
        sscanf(str,"%lx",&BBAddr);
    }

    return 0;
}

static int __init Coredump_setup(char *str)
{
    printk("!!!!!!!!!!!!!!!!!!!!!! coredump_path is:%s\n",str);
    if( str != NULL)
    {
        strncpy(coredump_path, str, CORENAME_MAX_SIZE);
    }
    else
    {
        printk("depend on core pattern\n");
    }
    return 0;
}
#ifdef CONFIG_MSTAR_STR_CRC
static int __init str_crc_setup(char *str)
{
    if( str != NULL )
    {
        str_crc = simple_strtol(str, NULL, 16);
    }
    else
    {
        printk("\nstr_crc is disable\n");
    }
    printk("\nstr_crc = %d\n", str_crc);
    return 0;
}
#endif

#ifdef CONFIG_MP_MM_MALI_RESERVE
static int MALI_MIU0_RESERVE_setup(char *str)
{
    printk("MALI_RESERVE_IN_MIU0_SIZE = %s\n", str);
    if( str != NULL )
    {
    	int ret = -1;
        ret = strict_strtol(str,0,&mali_reserve_size_miu0);
        //mali_reserve_size_miu0 = mali_reserve_size_miu0 *1024 * 1024;
    }
    else
    {
        printk("\nMALI_RESERVE_IN_MIU0_SIZE not set, and it will be zero.\n");
    }
	//mali_reserve_size_miu0 = 0x6400000;
    printk("MALI_RESERVE_IN_MIU0_SIZE = %s, mali_reserve_size_miu0 = %lx \n", str,mali_reserve_size_miu0);

    return 0;
}

static int MALI_MIU1_RESERVE_setup(char *str)
{
    if( str != NULL )
    {
    	int ret = -1;
        ret = strict_strtol(str,0,&mali_reserve_size_miu1);
    	printk("ret=%d,mali_reserve_size_miu1 = %ld \n",ret,mali_reserve_size_miu1);

        //mali_reserve_size_miu1 = mali_reserve_size_miu1 *1024 * 1024;
    }
    else
    {
        printk("\nMALI_RESERVE_IN_MIU1_SIZE not set, and it will be zero.\n");
    }
	//mali_reserve_size_miu1 = 0x10000000;
	//mali_reserve_size_miu1 = 0;
	//mali_reserve_size_miu0 = 0x6400000;
    printk("MALI_RESERVE_IN_MIU1_SIZE = %s, mali_reserve_size_miu1 = %lx \n", str,mali_reserve_size_miu1);

	return 0;
}

static int MALI_RESERVE_migrate(char *str)
{
    //printk("%s, disable migrate!\n", str);
	if( str != NULL )
    {
        //int ret = -1;
        //ret = strict_strtol(str,0,&mali_reserve_level);
    	//printk("mali_reserve_level = %ld \n",mali_reserve_level);
    }
    else
    {
        printk("MALI_RESERVE_migrate not set, and it will be true.\n");
		//mali_reserve_level = 1;
    }
	return 0;
}
#endif

early_param("LX_MEM", LX_MEM_setup);
early_param("EMAC_MEM", EMAC_MEM_setup);
early_param("GMAC_MEM", GMAC_MEM_setup);
early_param("DRAM_LEN", DRAM_LEN_setup);
early_param("LX_MEM2", LX_MEM2_setup);
early_param("LX_MEM3", LX_MEM3_setup);
early_param("BB_ADDR", BBAddr_setup);
early_param("G3D_MEM", G3D_MEM_setup);
early_param("MIU01_ADJ", MIU01_ADJ_setup);
early_param("PM_GPIO_NUM", PM_GPIO_NUM_setup);
early_param("SD_PAD_NUM", SD_PAD_NUM_setup);
#ifdef CONFIG_MP_R2_STR_ENABLE
early_param("FSTR", R2TEE_STR_ADDR_setup);
#endif

#ifdef CONFIG_MP_MM_MALI_RESERVE
early_param("MALI_RESERVE_IN_MIU0_SIZE", MALI_MIU0_RESERVE_setup);
early_param("MALI_RESERVE_IN_MIU1_SIZE", MALI_MIU1_RESERVE_setup);
early_param("MALI_RESERVE_MIGRATE", MALI_RESERVE_migrate);
#endif
early_param("CORE_DUMP_PATH", Coredump_setup);
#ifdef CONFIG_MSTAR_STR_CRC
early_param("str_crc", str_crc_setup);
#endif

early_param("SD_CONFIG", SD_CONFIG_NUM_setup);
early_param("SDIO_CONFIG", SDIO_CONFIG_NUM_setup);
early_param("tee_mode", TEE_MODE_INFO_setup);

static char bUseDefMMAP=0;
#if 0
static void check_boot_mem_info(void)
{
    if( LXmem==0 || EMACmem==0 || DRAMlen==0 )
    {
        bUseDefMMAP = 1;
    }
}
#endif

#if defined(CONFIG_MSTAR_OFFSET_FOR_SBOOT)
#define SBOOT_LINUX_MEM_START 0x00400000    //4M
#define SBOOT_LINUX_MEM_LEN   0x01400000    //20M
#define SBOOT_EMAC_MEM_LEN    0x100000      //1M
#define SBOOT_GMAC_MEM_LEN    0x100000      //1M
void get_boot_mem_info_sboot(BOOT_MEM_INFO type, unsigned int *addr, unsigned int *len)
{
    switch (type)
    {
    case LINUX_MEM:
        *addr = SBOOT_LINUX_MEM_START;
        *len = SBOOT_LINUX_MEM_LEN;
        break;

    case EMAC_MEM:
        *addr = SBOOT_LINUX_MEM_START+SBOOT_LINUX_MEM_LEN;
        *len = SBOOT_EMAC_MEM_LEN;
        break;

    case GMAC_MEM:
        *addr = SBOOT_LINUX_MEM_START + SBOOT_LINUX_MEM_LEN + SBOOT_EMAC_MEM_LEN;
        *len = SBOOT_GMAC_MEM_LEN;
        break;

    case MPOOL_MEM:
        *addr = SBOOT_LINUX_MEM_START + SBOOT_LINUX_MEM_LEN + SBOOT_EMAC_MEM_LEN;
        *len = 256*1024*1024;
        break;

    case LINUX_MEM2:
        *addr = 0;
        *len = 0;
        break;

    case LINUX_MEM3:
        *addr = 0;
        *len = 0;
        break;

    default:
        *addr = 0;
        *len = 0;
        break;
    }
}
#endif//CONFIG_MSTAR_OFFSET_FOR_SBOOT


void get_boot_mem_info(BOOT_MEM_INFO type, u64 *addr, u64 *len)
{
#if defined(CONFIG_MSTAR_OFFSET_FOR_SBOOT)
    get_boot_mem_info_sboot(type, addr, len);
    printk("!!!!!!!!!!SBOOT memory type=%x addr=%x, len=%x!!!!!!!!!!\n",type, *addr, *len);
    return;
#endif//CONFIG_MSTAR_OFFSET_FOR_SBOOT

    if (bUseDefMMAP == 0)
    {
        switch (type)
        {
        case LINUX_MEM:
            *addr = PHYS_OFFSET;
            *len = LXmem;
            break;

        case EMAC_MEM:
            *addr = PHYS_OFFSET + LXmem;
            *len = EMACmem;
            break;

        case MPOOL_MEM:
            *addr = LINUX_MEM_BASE_ADR + LXmem + EMACmem;
            *len = DRAMlen - *addr;
            break;

        case LINUX_MEM2:
            if (LXmem2Addr!=0 && LXmem2Size!=0)
            {
                *addr = LXmem2Addr;
                *len = LXmem2Size;
            }
            else
            {
                *addr = 0;
                *len = 0;
            }
            break;
        case LINUX_MEM3:
            if (LXmem3Addr!=0 && LXmem3Size!=0)
            {
                *addr = LXmem3Addr;
                *len = LXmem3Size;
            }
            else
            {
                *addr = 0;
                *len = 0;
            }
            break;
        case G3D_MEM0:
            *addr = G3Dmem0Addr;
            *len = G3Dmem0Len;
            break;
        case G3D_MEM1:
            *addr = G3Dmem1Addr;
            *len = G3Dmem1Len;
            break;
        case G3D_CMDQ:
            *addr = G3DCmdQAddr;
            *len = G3DCmdQLen;
            break;
        case DRAM:
            *addr = 0;
            *len = DRAMlen;
            break;
        case BB:
            *addr = BBAddr;
            *len = 0;
            break;
        case GMAC_MEM:
            *addr = GMACmemAddr;
            *len = GMACmemLen;
            break;
        default:
            *addr = 0;
            *len = 0;
            break;
        }
    }
    else
    {
        switch (type)
        {
        case LINUX_MEM:
            *addr =PHYS_OFFSET;
            *len = LINUX_MEM_LEN;
            break;

        case EMAC_MEM:
            *addr = EMAC_MEM_ADR;
            *len = EMAC_MEM_LEN;
            break;

        case MPOOL_MEM:
            *addr = MPOOL_ADR;
            *len = MPOOL_LEN;
            break;

        case LINUX_MEM2:
            if (LXmem2Addr!=0 && LXmem2Size!=0)
            {
                *addr = LXmem2Addr;
                *len  = LXmem2Size;
            }
            else
            {
                #ifdef LINUX_MEM2_BASE_ADR    // reserved miu1 memory for linux
                *addr = LINUX_MEM2_BASE_ADR;
                *len = LINUX_MEM2_LEN;
                printk("LINUX_MEM2 addr %08llx, len %08llx\n", *addr, *len);
                #else
                *addr = 0;
                *len = 0;
                #endif
            }
            break;
        case LINUX_MEM3:
            if (LXmem3Addr!=0 && LXmem3Size!=0)
            {
                *addr = LXmem3Addr;
                *len  = LXmem3Size;
            }
            else
            {
                *addr = 0;
                *len = 0;
            }
            break;
        case G3D_MEM0:
            *addr = G3Dmem0Addr;
            *len = G3Dmem0Len;
            break;
        case G3D_MEM1:
            *addr = G3Dmem1Addr;
            *len = G3Dmem1Len;
            break;
        case G3D_CMDQ:
            *addr = G3DCmdQAddr;
            *len = G3DCmdQLen;
            break;
        case DRAM:
            *addr = 0;
            *len = DRAMlen;
            break;
        case BB:
            *addr = BBAddr;
            *len = 0;
            break;
        case GMAC_MEM:
            *addr = GMACmemAddr;
            *len = GMACmemLen;
            break;
        default:
            *addr = 0;
            *len = 0;
            break;
        }
    }
}
EXPORT_SYMBOL(get_boot_mem_info);

#ifdef CONFIG_CMA
extern phys_addr_t arm_lowmem_limit;
void __init dumpMemInfo(unsigned long start, unsigned long size)
{
	unsigned long va_arm_lowmem_limit = __phys_to_virt((unsigned long )(arm_lowmem_limit));

    if(start >= va_arm_lowmem_limit)
        return;

    if((start+size) > va_arm_lowmem_limit)
        size = va_arm_lowmem_limit - start;

    printk(KERN_NOTICE "    lowmem  : 0x%08lx - 0x%08lx   (%6ld kB)\n",
        start, start+size, (size >> 10));
}

void __init lowMemInfo(void)
{
    if(PHYS_OFFSET != INVALID_PHY_ADDR)
        dumpMemInfo(PAGE_OFFSET, lx_mem_size);

    if(lx_mem2_addr != INVALID_PHY_ADDR)
        dumpMemInfo(PAGE_OFFSET1, lx_mem2_size);

    if(lx_mem3_addr != INVALID_PHY_ADDR)
        dumpMemInfo(PAGE_OFFSET2, lx_mem3_size);

	printk(KERN_NOTICE "    va_arm_lowmem_limit  : 0x%llX\n", (unsigned long long)__va(arm_lowmem_limit));
}
#endif

extern void early_putstr(const char *fmt, ...);
extern int early_init_dt_add_memory_arch(u64 base, u64 size);
u64 linux_memory_address = 0,linux_memory_length = 0;
u64 linux_memory2_address = 0, linux_memory2_length = 0;
u64 linux_memory3_address = 0, linux_memory3_length = 0;
void __init prom_meminit(void)
{
//the lx_mem info is parising in function "early_init_dt_scan_memory"
#if (MP_PLATFORM_ARM_64bit_BOOTARGS_NODTB == 1)
    //check_boot_mem_info();
    get_boot_mem_info(LINUX_MEM, &linux_memory_address, &linux_memory_length);
    get_boot_mem_info(LINUX_MEM2, &linux_memory2_address, &linux_memory2_length);
    get_boot_mem_info(LINUX_MEM3, &linux_memory3_address, &linux_memory3_length);
#endif

    if ((linux_memory_address | linux_memory_length | linux_memory2_address | linux_memory2_length
                | linux_memory3_address | linux_memory3_length) & (0x100000-1))
    {
        printk("[ERROR] LX_MEM, LX_MEM2, LX_MEM3 should be aligned to 1MB\n");
        printk("[ERROR] LX_MEM, LX_MEM2, LX_MEM3 should be aligned to 1MB\n");
        printk("[ERROR] LX_MEM, LX_MEM2, LX_MEM3 should be aligned to 1MB\n");
        //while(1); can't block it, it will cause printk message not output
    }

	/*
	 * if you use cma_buffer, you must ensure having PMD_SIZE alignment
	 */
#ifdef CONFIG_CMA
#ifdef CONFIG_MP_CMA_PATCH_LX_MEMORY_ALIGN_TO_PMD_SIZE_CHECK
    if ((linux_memory_address | linux_memory_length | linux_memory2_address | linux_memory2_length
                | linux_memory3_address | linux_memory3_length) & (PMD_SIZE-1))
    {
        printk("[ERROR] LX_MEM, LX_MEM2, LX_MEM3 should be aligned to PMD_SIZE\n");
        printk("[ERROR] LX_MEM, LX_MEM2, LX_MEM3 should be aligned to PMD_SIZE\n");
        printk("[ERROR] LX_MEM, LX_MEM2, LX_MEM3 should be aligned to PMD_SIZE\n");
        //while(1); can't block it, it will cause printk message not output
    }
#endif
#endif

//the lx_mem is added in  function "early_init_dt_scan_memory"
#if (MP_PLATFORM_ARM_64bit_BOOTARGS_NODTB == 1)
    if (linux_memory_length != 0)
        early_init_dt_add_memory_arch(linux_memory_address, linux_memory_length);
    if (linux_memory2_length != 0)
        early_init_dt_add_memory_arch(linux_memory2_address, linux_memory2_length);
    if (linux_memory3_length != 0)
        early_init_dt_add_memory_arch(linux_memory3_address, linux_memory3_length);
#endif

#if (MP_PLATFORM_ARM_64bit_BOOTARGS_NODTB == 0)
    /*for low mem VA range setting(__phys_to_virt)*/
    lx_mem_addr = linux_memory_address;
    lx_mem_size = linux_memory_length;
   early_putstr("value:0x%lx and:0x%lx\n",lx_mem_addr,lx_mem_size);
    if(linux_memory2_address != 0){
	    lx_mem2_addr = linux_memory2_address;
	    lx_mem2_size = linux_memory2_length;
    }
    if(linux_memory3_address != 0){
	    lx_mem3_addr = linux_memory3_address;
	    lx_mem3_size = linux_memory3_length;
   }
#endif

    printk("\n");
    printk("LX_MEM  = 0x%llX, 0x%llX\n", linux_memory_address,linux_memory_length);
    printk("LX_MEM2 = 0x%llX, 0x%llX\n",linux_memory2_address, linux_memory2_length);
    printk("LX_MEM3 = 0x%llX, 0x%llX\n",linux_memory3_address, linux_memory3_length);
    printk("EMAC_LEN= 0x%lX\n", EMACmem);
    printk("DRAM_LEN= 0x%lX\n", DRAMlen);

	/* prevent overflow while converting pa to va for linux memory */
	if(linux_memory_length > (0xFFFFFFFF - PAGE_OFFSET))
	{
		lx_mem_left_size = 0xFFFFFFFF - PAGE_OFFSET;
		lx_mem2_left_size = 0;
	}
	else
	{
		lx_mem_left_size = lx_mem_size;
		if(linux_memory2_length > (0xFFFFFFFF - PAGE_OFFSET - lx_mem_left_size))
		{
			lx_mem2_left_size = 0xFFFFFFFF - PAGE_OFFSET - lx_mem_left_size;
		}
		else
		{
			lx_mem2_left_size = lx_mem2_size;
		}
	}

	if(lx_mem_left_size != lx_mem_size)
	{
		printk("\033[31mLX_LEFT_MEM_SIZE  = 0x%X\033[m\n", (unsigned int)lx_mem_left_size);
		printk("\033[31mLX2_LEFT_MEM_SIZE  = 0x%X\033[m\n", (unsigned int)lx_mem2_left_size);
	}

	if(lx_mem2_left_size != lx_mem2_size)
	{
		printk("\033[31mLX2_LEFT_MEM_SIZE  = 0x%X\033[m\n", (unsigned int)lx_mem2_left_size);
	}
}

inline unsigned long get_BBAddr(void)
{
    return BBAddr;
}

//void __init prom_free_prom_memory(void)
//{
//    unsigned long addr;
//    int i;
//
//    for (i = 0; i < boot_mem_map.nr_map; i++) {
//        if (boot_mem_map.map[i].type != BOOT_MEM_ROM_DATA)
//            continue;
//
//        addr = boot_mem_map.map[i].addr;
//        free_init_pages("prom memory",
//                addr, addr + boot_mem_map.map[i].size);
//    }
//}

char* get_coredump_path(void)
{
    return coredump_path;
}
EXPORT_SYMBOL(get_coredump_path);
#ifdef CONFIG_MSTAR_STR_CRC
int get_str_crc(void)
{
    return str_crc;
}
EXPORT_SYMBOL(get_str_crc);
#endif

#ifdef CONFIG_MP_R2_STR_ENABLE
unsigned long get_str_handshake_addr(void)
{
    return R2TEE_STR_HANDSHAKE_addr;
}
EXPORT_SYMBOL(get_str_handshake_addr);
#endif

#if  (MP_CHECKPT_BOOT == 1)
#define piu_timer1_cap_low   0x1f006090UL
#define piu_timer1_cap_high  0x1f006094UL
int Mstar_Timer1_GetMs(void)
{
     int timer_value = 0;
	     timer_value = reg_readw(piu_timer1_cap_low);
	     timer_value += (reg_readw(piu_timer1_cap_high) << 16);
	     timer_value = timer_value / 12000;
		 return timer_value;
}
#endif // MP_CHECKPT_BOOT

#ifdef CONFIG_MP_PLATFORM_VERIFY_LX_MEM_ALIGN
void mstar_lx_mem_alignment_check(void)
{
    unsigned int linux_memory_address = 0, linux_memory_length = 0;
    unsigned int linux_memory2_address = 0, linux_memory2_length = 0;
    unsigned int linux_memory3_address = 0, linux_memory3_length = 0;

    //check_boot_mem_info();
    get_boot_mem_info(LINUX_MEM, &linux_memory_address, &linux_memory_length);
    get_boot_mem_info(LINUX_MEM2, &linux_memory2_address, &linux_memory2_length);
    get_boot_mem_info(LINUX_MEM3, &linux_memory3_address, &linux_memory3_length);

    if ((linux_memory_address | linux_memory_length | linux_memory2_address | linux_memory2_length
                | linux_memory3_address | linux_memory3_length) & (0x100000-1))
    {
        printk("[ERROR] LX_MEM, LX_MEM2, LX_MEM3 should be aligned to 1MB\n");
        printk("[ERROR] LX_MEM, LX_MEM2, LX_MEM3 should be aligned to 1MB\n");
        printk("[ERROR] LX_MEM, LX_MEM2, LX_MEM3 should be aligned to 1MB\n");
        while(1);
    }
}
#endif //#ifdef CONFIG_MP_PLATFORM_VERIFY_LX_MEM_ALIGN

