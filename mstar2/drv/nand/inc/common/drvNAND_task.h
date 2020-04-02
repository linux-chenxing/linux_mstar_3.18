/********************************************************/
/*              Define                                     */
/********************************************************/
#define UNFD_DATA_WAIT_TIME         1500
#define UNFD_TASK_STACK_SIZE        0x1000
#define NAND_MSG_FORCE_WRITE_BACK   0x200
#define NAND_MSG_WEAR_LEVELING1     0x100
#define NAND_MSG_WEAR_LEVELING      0x80
#define NAND_MSG_WRITE_FLOW         0x40
#define NAND_MSG_READ_FLOW          0x20
#define NAND_MSG_FLUSHCACHE         0x10

#define NAND_MSG_STATE_MASK        (NAND_MSG_WRITE_FLOW | NAND_MSG_READ_FLOW | NAND_MSG_WEAR_LEVELING | NAND_MSG_WEAR_LEVELING1 | NAND_MSG_FORCE_WRITE_BACK)



void drvNAND_TaskBGTrequest_fn(int flag);
int drvNAND_TaskBGT_Init(void);
