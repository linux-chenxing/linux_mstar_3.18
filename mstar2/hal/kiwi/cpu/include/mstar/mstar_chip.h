#define MSTAR_MIU0_BUS_BASE                      0x40000000  // this shoule be start at 0x00000000 
#define MSTAR_MIU1_BUS_BASE                      0x80000000  // not used, keres do not have MIU1

#define REG_ADDR(addr)                          (*((volatile U16*)(0xBF200000 + (addr << 1))))
#define REG_CKG_AEONTS0                         0x0B24
#define AEON_CLK_ENABLE                         0x0084 //CLK 172.8MHz
#define AEON_CLK_DISABLE                        0x0001
#define MIU_PROTECT_EN                          0x12C0
#define MIU_PROTECT3_ID0                        0x12D6
#define MIU_PROTECT3_START_ADDR_H               0x12D8
#define MIU_PROTECT3_END_ADDR_H                 0x12DA
#define MIU_CLI_AEON_RW                         0x0005
#define MIU_PROTECT_4                           0x0008
#define MBX_AEON_JUDGEMENT                      0x33DE
#define MHEG5_CPU_STOP                          0x0FE6
#define STOP_AEON                               0x0001
#define MHEG5_REG_IOWINORDER                    0x0F80
#define REG_AEON_C_FIQ_MASK_L                   0x1948
#define REG_AEON_C_FIQ_MASK_H                   0x194A
#define REG_AEON_C_IRQ_MASK_L                   0x1968
#define REG_AEON_C_IRQ_MASK_H                   0x196A
#define REG_MAU0_MIU0_SIZE                      0x1842

#define REG_CHIP_NAND_MODE			            0x1E10
#define REG_CHIP_NAND_MODE_MASK			        0x0030

#define REG_EMMC_CONFIG                         0x1E10
#define REG_EMMC_CONFIG_MASK                    0x0003

#define REG_SD_CONFIG2                          0x1E10
#define REG_SD_CONFIG2_MASK                     0x0300

#define REG_CHIP_PCMCFG                         0x1E0E
#define REG_CHIP_PCMCFG_MASK                    0x0001
#define REG_CHIP_PCMCFG_CTRL_EN                 0x0001

#define MIPS_MIU0_BUS_BASE                      MSTAR_MIU0_BUS_BASE
#define MIPS_MIU1_BUS_BASE                      MSTAR_MIU1_BUS_BASE

#define MS_MIU_INTERVAL                         0x20000000

// Debug register 
#define R_DEBUG                     0xBF206700 // mailbox 0 

// L2 Pre-Fetch Buffer
#define REG_PREFTECH_L2_BASE (0xBF203000)
#define REG_PREFTECH_L2_CONFIG   (0xBF203100)
#define PREFTECH_L2_CONFIG_DISABLE (0x0002)
#define REG_PREFTECH_L2_CONTROL  (0xBF203104)
#define PREFTECH_L2_CONTROL_MODESEL   (0x000F)
#define PREFTECH_L2_CONTROL_ACTSTART  (0x0010)
#define PREFTECH_L2_CONTROL_MODE0DONE (0x0100)
#define PREFTECH_L2_CONTROL_MODE34DONE (0x0400)

#define PREFTECH_L2_CONTROL_MODE PREFTECH_L2_CONTROL_MODE0DONE


// L2 op mode 
#define L2_OP_MODE_FLUSH_ALL_AND_INVALID_CACHE 0x0004
#define L2_OP_MODE_INITIAL_ALL_INVALID 0x0000
#define REG_CACHE_OP_FIRE 0x0010

#define L2_OP_MODE L2_OP_MODE_INITIAL_ALL_INVALID
