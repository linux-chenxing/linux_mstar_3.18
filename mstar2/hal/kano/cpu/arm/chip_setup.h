#ifndef _CHIP_SETUP_H_
#define _CHIP_SETUP_H_

#define MIU_NUM                     2
#ifdef CONFIG_ARM_LPAE
#define ARM_MIU0_BASE               0x20000000UL
#define ARM_MIU1_BASE               0x200000000ULL
#define MIU1_BUS_BASE               0x200000000ULL
#define ARM_MIU1_BASE_L             0xC0000000UL   //32bit address
#define ARM_MIU1_SIZE_L             0x20000000UL   //32bit size
#else
#define ARM_MIU0_BASE               0x20000000UL
#define ARM_MIU1_BASE               0xC0000000UL
#define MIU1_BUS_BASE               0xC0000000UL
#endif
#define RIU_VIRT_BASE               0xFD000000UL

#define REG(addr) (*(volatile unsigned int *)(addr))

#define SECOND_MAGIC_NUMBER_ADRESS	0xC0008000
#define SECOND_START_ADDR		0xC0008004

void* Chip_mphy_cachevirt( unsigned long mphyaddr );
void* Chip_mphy_noncachevirt( unsigned long mphyaddr );
unsigned long Chip_mphy_bus( unsigned long phyaddress );
unsigned long Chip_bus_mphy( unsigned long busaddress );

extern void _chip_flush_miu_pipe(void); //flush miu pipe
extern void Chip_Flush_Memory(void);  //flush miu pipe
extern void Chip_Read_Memory(void) ;  //flush miu pipe
extern void Chip_Flush_Memory_Range(unsigned long pAddress , unsigned long  size);  //flush miu pipe
extern void Chip_Read_Memory_Range(unsigned long pAddress , unsigned long  size ) ;  //flush miu pipe


extern void Chip_Flush_Cache_Range(unsigned long u32Addr, unsigned long u32Size); //Clean & Invalid L1/L2 cache
extern void Chip_Clean_Cache_Range(unsigned long u32Addr, unsigned long u32Size); //Clean L1/L2 cache, keep valid
extern void Chip_Inv_Cache_Range(unsigned long u32Addr, unsigned long u32Size);  //Invalid L1/L2 cache

extern void Chip_Flush_Cache_Range_VA_PA(unsigned long u32VAddr,unsigned long u32PAddr,unsigned long u32Size); //Clean & Invalid L1/L2 cache
extern void Chip_Clean_Cache_Range_VA_PA(unsigned long u32VAddr, unsigned long u32PAddr,unsigned long u32Size); //Clean L1/L2 cache, keep valid
extern void Chip_Inv_Cache_Range_VA_PA(unsigned long u32VAddr, unsigned long u32PAddr,unsigned long u32Size);  //Invalid L1/L2 cache
extern void Chip_Flush_Cache_All(void); //Clean & Invalid L1/L2 cache
extern void Chip_Flush_Cache_All_Single(void);

extern unsigned int Chip_Query_CLK(void);
extern unsigned int Chip_Query_Rev(void);
extern void Chip_Query_L2_Config(void);

#endif

