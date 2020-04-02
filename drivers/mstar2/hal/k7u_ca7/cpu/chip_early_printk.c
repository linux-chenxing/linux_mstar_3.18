#include <linux/console.h>
#include <linux/init.h>
#include <linux/kernel.h>


#define UART_RX                     (0)  // In:  Receive buffer (DLAB=0)
#define UART_TX                     (0)  // Out: Transmit buffer (DLAB=0)
#define UART_DLL                    (0)  // Out: Divisor Latch Low (DLAB=1)
#define UART_DLM                    (1)  // Out: Divisor Latch High (DLAB=1)
#define UART_IER                    (1)  // Out: Interrupt Enable Register
#define UART_IIR                    (2)  // In:  Interrupt ID Register
#define UART_FCR                    (2)  // Out: FIFO Control Register
#define UART_LCR                    (3)  // Out: Line Control Register
#define UART_LSR                    (5)  // In:  Line Status Register
#define UART_MSR                    (6)  // In:  Modem Status Register
#define UART_USR                    (7)


// UART_LSR(5)
// Line Status Register
#define UART_LSR_DR                 0x01          // Receiver data ready
#define UART_LSR_OE                 0x02          // Overrun error indicator
#define UART_LSR_PE                 0x04          // Parity error indicator
#define UART_LSR_FE                 0x08          // Frame error indicator
#define UART_LSR_BI                 0x10          // Break interrupt indicator
#define UART_LSR_THRE               0x20          // Transmit-hold-register empty
#define UART_LSR_TEMT               0x40          // Transmitter empty

/* 1F000000 => FD000000 */
/* UART 0 : base + 0x1f201300 */ 
/* UART 1 : base + 0x1f220c00 */ 
/* UART 2 : base + 0x1f220d00 */ 
//#define UART_REG(addr) *((volatile unsigned int*)(0xFD201300 + ((addr)<< 3)))
//#define UART_REG(addr) *((volatile unsigned int*)(0xFD220D00 + ((addr)<< 3)))
#define UART_REG(addr) *((volatile unsigned int*)(0xfd201300UL + ((addr)<< 3)))
#define DIRECT_UART_READ_REG(reg, addr) do{  \
                                            reg = *((volatile unsigned int*)(0xfd201300UL + ((addr)<< 3))); \
                                        }while(0)

#define DIRECT_UART_WRITE_REG(reg, addr) do{  \
                                             *((volatile unsigned int*)(0xfd201300UL + ((addr)<< 3))) = reg; \
                                         }while(0)

/*------------------------------------------------------------------------------
    Local Function Prototypes
-------------------------------------------------------------------------------*/
int prom_putchar(char c);

static u32 UART16550_READ(u8 addr)
{
	u32 data;

	if (addr>80) //ERROR: Not supported
	{
		return(0);
	}

	data = UART_REG((unsigned int)addr);
	return(data);
}

static void UART16550_WRITE(u8 addr, u8 data)
{
	if (addr>80) //ERROR: Not supported
	{
		//printk("W: %d\n",addr);
		return;
	}
	UART_REG((unsigned int)addr) = data;
}

static inline unsigned int serial_in(int offset)
{
	return UART16550_READ((u8)offset);
}

static inline void serial_out(int offset, int value)
{
	UART16550_WRITE((u8)offset, (u8)value);
}

int prom_putchar(char c)
{

       //volatile int i=0;

 	while ((serial_in(UART_LSR) & UART_LSR_THRE) == 0)
		;
       // for ( i=0;i<1000;i++) 
       // {
       //     serial_in(UART_LSR);
       // }
	serial_out(UART_TX, c);

	return 1;
}

static char early_printk_buf[1024];
// only use before prepare_page_table()
void early_putstr(const char *fmt, ...)
{
	char c;
	va_list args;
	char* ptr;
        u16 reg;

	va_start(args, fmt);
	vscnprintf(early_printk_buf, sizeof(early_printk_buf), fmt, args);
	va_end(args);
	ptr = early_printk_buf;
        

    while ((c = *ptr++) != '\0') {
        if (c == '\n')
        {
            DIRECT_UART_READ_REG(reg, UART_LSR);
            while ((reg & UART_LSR_THRE) == 0)
            {
                DIRECT_UART_READ_REG(reg, UART_LSR);
            }
            //DIRECT_UART_REG(0) = 'e';
            DIRECT_UART_WRITE_REG('\r', 0);
	}
        DIRECT_UART_READ_REG(reg, UART_LSR);
        while ((reg & UART_LSR_THRE) == 0)
        {
            DIRECT_UART_READ_REG(reg, UART_LSR);
        }
        DIRECT_UART_WRITE_REG(c, 0);

       //DIRECT_UART_REG(0) = 'e';
    }
}

static void chip_printch(int n)
{
	u16 reg;

	do {
		DIRECT_UART_READ_REG(reg, UART_LSR);
	} while (!(reg & UART_LSR_THRE));
	DIRECT_UART_WRITE_REG(n, 0);
}

static void chip_early_write(const char *s, unsigned n)
{
    while (n-- > 0) {
        if (*s == '\n')
            chip_printch('\r');
        chip_printch(*s);
        s++;
    }
}

void __init chip_early_map_io(void);
int chip_early_vprintk(const char *fmt, va_list ap)
{
	char buf[512];
	int n = vscnprintf(buf, sizeof(buf), fmt, ap);
	static bool riu_mapped = false;

	if (!riu_mapped) {
		chip_early_map_io();
		riu_mapped = true;
	}

	chip_early_write(buf, n);

	return n;
}

