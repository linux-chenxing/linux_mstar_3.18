
typedef enum
{
      XC_RESULT_OK          //= 0
     ,XC_RESULT_ERR         //= 1
     ,XC_RESULT_UND         //= 2
     ,XC_RESULT_MAX
} XCResult;

//This filed predefine the opcode of SMC protocol,
typedef enum
{
     XC_SMC_CLASS_SMC_NORMAL_CALL       //= 0
    ,XC_SMC_CLASS_INTERRUPT             //= 1
    ,XC_SMC_CLASS_WSM                   //= 2
    ,XC_SMC_CLASS_MAX

} XC_SMC_CLASS;

int init_xc_proc( void );
unsigned int xc_proc_wake_up(void);
