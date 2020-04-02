#ifndef MHAL_XC_CONFIG_H
#define MHAL_XC_CONFIG_H

#define ENABLE_64BITS_COMMAND               1
#define ENABLE_64BITS_SPREAD_MODE           1 //need enable ENABLE_64BITS_COMMAND first

#define ENABLE_MLOAD_SAME_REG_COMBINE       1
#define DS_CMD_LEN_64BITS                   8
#define ENABLE_DS_4_BASEADDR_MODE           1 // need enable both ENABLE_64BITS_COMMAND and ENABLE_64BITS_SPREAD_MODE first
#define SUPPORT_DS_MULTI_USERS              1
#define DS_BUFFER_NUM_EX                    6

#define IS_SUPPORT_64BITS_COMMAND(bEnable64bitsCmd, u32DeviceID)           ((bEnable64bitsCmd == 1) && (u32DeviceID == 0)) 

#endif /* MHAL_XC_CONFIG_H */
