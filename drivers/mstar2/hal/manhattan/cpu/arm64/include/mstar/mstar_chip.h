#define MSTAR_MIU0_BUS_BASE                      0x20000000
extern phys_addr_t MIU1_BASE;  
#define MSTAR_MIU1_BUS_BASE                      MIU1_BASE
#define MSTAR_MIU1_BUS_BASE_L                    0xC0000000UL
#define MSTAR_MIU1_BUS_BASE_H                    0x200000000UL

#define ARM_MIU0_BUS_BASE                      MSTAR_MIU0_BUS_BASE
#define ARM_MIU1_BUS_BASE                      MSTAR_MIU1_BUS_BASE
#define ARM_MIU2_BUS_BASE                      0xFFFFFFFFFFFFFFFFUL
#define ARM_MIU3_BUS_BASE                      0xFFFFFFFFFFFFFFFFUL

#define ARM_MIU0_BASE_ADDR                     0x00000000UL
#define ARM_MIU1_BASE_ADDR                     0x80000000UL
#define ARM_MIU2_BASE_ADDR                     0xFFFFFFFFFFFFFFFFUL
#define ARM_MIU3_BASE_ADDR                     0xFFFFFFFFFFFFFFFFUL

#define MST_XTAL_CLOCK_HZ   (12000000UL)

#define REG_WDT_BASE         (mstar_pm_base + 0x006000)
#define REG_WDT_SET(x)      ( REG_WDT_BASE + 2*x )
#define REG_WDT_CLR         REG_WDT_SET(0x00UL)
#define REG_WDT_RST         REG_WDT_SET(0x02UL)
#define REG_WDT_INT         REG_WDT_SET(0x03UL)
#define REG_WDT_MAX         REG_WDT_SET(0x04UL)
#define REG_WDT_MAX2        REG_WDT_SET(0x05UL)

#define WDT_CLR_RESET_FLAG  BIT(0)
#define WDT_RST             BIT(0)
#define WDT_CLOSE_KEY       (0x0)

#define WDT_GET_TIMER() ((*((volatile unsigned short *)(REG_WDT_MAX))) | ((*((volatile unsigned short *)(REG_WDT_MAX2))) << 16))
#define WDT_CLEAR_RST_FLAG() ((*((volatile unsigned char *)(REG_WDT_RST))) = WDT_CLR_RESET_FLAG)
#define WDT_SET_TIMER(x) \
    ((*((volatile unsigned short *)(REG_WDT_MAX))) = (unsigned short)(((x) * (MST_XTAL_CLOCK_HZ)) & 0x0000FFFFUL), \
    (*((volatile unsigned short *)(REG_WDT_MAX2))) = (unsigned short)(((x) * (MST_XTAL_CLOCK_HZ)) >> 16))
#define WDT_REFRESH() (*((volatile unsigned char *)(REG_WDT_CLR))) = WDT_RST;

extern unsigned int query_frequency(void);
extern ptrdiff_t mstar_pm_base;
