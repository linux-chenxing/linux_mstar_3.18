#ifdef CONFIG_ARM_LPAE
#define MSTAR_MIU0_BUS_BASE                      0x20000000UL
#define MSTAR_MIU1_BUS_BASE                      0x200000000ULL

#define ARM_MIU0_BUS_BASE                        MSTAR_MIU0_BUS_BASE
#define ARM_MIU1_BUS_BASE                        MSTAR_MIU1_BUS_BASE
#define ARM_MIU2_BUS_BASE                        0xFFFFFFFFFFFFFFFFULL
#define ARM_MIU3_BUS_BASE                        0xFFFFFFFFFFFFFFFFULL

#define ARM_MIU0_BASE_ADDR                       0x00000000UL
#define ARM_MIU1_BASE_ADDR                       0x80000000UL
#define ARM_MIU2_BASE_ADDR                       0xFFFFFFFFFFFFFFFFULL
#define ARM_MIU3_BASE_ADDR                       0xFFFFFFFFFFFFFFFFULL
#else
#define MSTAR_MIU0_BUS_BASE                      0x20000000UL
/*there is no miu1 TLB at k6,so we can't access 0xC0000000(address of miu1 tlb,which is used by us to replace accessing 0x60000000) anymore
*Instead, we need to directly access address of miu1(0x60000000)
*/
#define MSTAR_MIU1_BUS_BASE                      0x60000000UL

#define ARM_MIU0_BUS_BASE                        MSTAR_MIU0_BUS_BASE
#define ARM_MIU1_BUS_BASE                        MSTAR_MIU1_BUS_BASE
#define ARM_MIU2_BUS_BASE                        0xFFFFFFFFUL
#define ARM_MIU3_BUS_BASE                        0xFFFFFFFFUL

#define ARM_MIU0_BASE_ADDR                       0x00000000UL
#define ARM_MIU1_BASE_ADDR                       0x80000000UL
#define ARM_MIU2_BASE_ADDR                       0xFFFFFFFFUL
#define ARM_MIU3_BASE_ADDR                       0xFFFFFFFFUL
#endif

extern unsigned int query_frequency(void);
