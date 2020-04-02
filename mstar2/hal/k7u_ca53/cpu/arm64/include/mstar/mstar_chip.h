#define MSTAR_MIU0_BUS_BASE                      0x20000000UL
extern phys_addr_t MIU1_BASE;  
#define MSTAR_MIU1_BUS_BASE                      MIU1_BASE
#define MSTAR_MIU1_BUS_BASE_L                    0x60000000UL
#define MSTAR_MIU1_BUS_BASE_H                    0x200000000UL

#define ARM_MIU0_BUS_BASE                      MSTAR_MIU0_BUS_BASE
#define ARM_MIU1_BUS_BASE                      MSTAR_MIU1_BUS_BASE
#define ARM_MIU2_BUS_BASE                      0xFFFFFFFFFFFFFFFFUL
#define ARM_MIU3_BUS_BASE                      0xFFFFFFFFFFFFFFFFUL

#define ARM_MIU0_BASE_ADDR                     0x00000000UL
#define ARM_MIU1_BASE_ADDR                     0x80000000UL
#define ARM_MIU2_BASE_ADDR                     0xFFFFFFFFFFFFFFFFUL
#define ARM_MIU3_BASE_ADDR                     0xFFFFFFFFFFFFFFFFUL

extern unsigned int query_frequency(void);
extern ptrdiff_t mstar_pm_base;
