#define MSTAR_MIU0_BUS_BASE                      0x20000000
#define MSTAR_MIU1_BUS_BASE                      0xA0000000
#define MSTAR_MIU2_BUS_BASE                      0xE0000000

#define ARM_MIU0_BUS_BASE                        MSTAR_MIU0_BUS_BASE
#define ARM_MIU1_BUS_BASE                        MSTAR_MIU1_BUS_BASE
#define ARM_MIU2_BUS_BASE                        MSTAR_MIU2_BUS_BASE
#define ARM_MIU3_BUS_BASE                        0xFFFFFFFFFFFFFFFFULL

#define ARM_MIU0_BASE_ADDR                       0x00000000UL
#define ARM_MIU1_BASE_ADDR                       0x80000000UL
#define ARM_MIU2_BASE_ADDR                       0xC0000000UL
#define ARM_MIU3_BASE_ADDR                       0xFFFFFFFFUL
extern unsigned int query_frequency(void);
