#ifndef MHAL_XC_CONFIG_H
#define MHAL_XC_CONFIG_H

#define ENABLE_64BITS_COMMAND               1
#define ENABLE_64BITS_SPREAD_MODE           1 //need enable ENABLE_64BITS_COMMAND first

#define ENABLE_MLOAD_SAME_REG_COMBINE       1
#define DS_CMD_LEN_64BITS                   8
#define ENABLE_DS_4_BASEADDR_MODE           1 // need enable both ENABLE_64BITS_COMMAND and ENABLE_64BITS_SPREAD_MODE first
#define SUPPORT_DS_MULTI_USERS              1
#define DS_BUFFER_NUM_EX                    6
#define XC_REPLACE_MEMCPY_BY_BDMA
#define IS_SUPPORT_64BITS_COMMAND(bEnable64bitsCmd, u32DeviceID)           ((bEnable64bitsCmd == 1) && (u32DeviceID == 0))
//#define HDR10_DOLBY
#define IPT
#define IPT_COUNT 1

//if you want to modify this define, please modify the same define in utopia (mhal_xc_chip_config.h)
//these irq will be used in both kernle and utopia
#define IRQ_REMAIN_IN_DUMMY                 (BIT(4))     //op vsync,BK00_10[4]

#define IRQ_INT_DISP_IP_VS                  (11)    //hdr dma,BK00_10[11]
#define IRQ_INT_DISP_OP_VS                  (4)     //op vsync,BK00_10[4]
#define IRQ_INT_DISP_DESCRB                 (22)    //hdr descrb,BK00_11[6]

#define INTERRUPT_DUMMY_REGISTER_L          REG_SC_BK30_4C_L
#define INTERRUPT_DUMMY_REGISTER_H          REG_SC_BK30_4D_L

#define IRQ_INT_DISP_IP1                    (11)    //IP1 vsync,BK00_10[11]
#endif /* MHAL_XC_CONFIG_H */
