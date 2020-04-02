#include <linux/linkage.h>
#include <asm/assembler.h>
#include <asm/memory.h>
#include <linux/init.h>
#include <asm/ptrace.h>
#include <asm/asm-offsets.h>
#include <asm/cputype.h>
#include <asm/thread_info.h>
#include <asm/pgtable-hwdef.h>
#include <asm/pgtable.h>
#include <asm/page.h>
#include <asm/virt.h>
#include <mach/pm.h>

.align 3
sleep_saved_magic: .quad 0
sleep_save: .fill 10, 8, 0
sleep_save_spt: .quad 0
sleep_save_ret_addr: .quad 0
sleep_save_wakup_vir: .quad 0
sleep_save_wakup_phy: .quad 0
sleep_save_regs: .fill 14, 8, 0
sleep_save2: .quad 0
sleep_save_wakup_save: .quad 0
sleep_save_L2CTLR_EL1: .quad 0
sleep_save_L2ECTLR_EL1: .quad 0
sleep_save_L2ACTLR_EL1: .quad 0
sleep_save_PAR_EL1: .quad 0
sleep_save_CPUACTLR_EL1: .quad 0
sleep_save_CPUECTLR_EL1: .quad 0
sleep_save_CPUMERRSR_EL1: .quad 0
sleep_save_TPIDR_EL1: .quad 0
sleep_save_CNTP_CVAL_EL0: .quad 0
sleep_save_CNTP_TVAL_EL0: .quad 0
sleep_save_CNTV_CVAL_EL0: .quad 0
sleep_save_CNTV_TVAL_EL0: .quad 0
sleep_save_CNTP_CTL_EL0: .quad 0
sleep_save_CNTV_CTL_EL0: .quad 0
sleep_save_CNTKCTL_EL1: .quad 0

.macro MVEX1,a1, a2, a3,tmp
    LDR \tmp,[\a2]
    STR \tmp,[\a1],#8
    LDR \tmp,[\a3],#8
    STR \tmp,[\a2],#8
.endm
.macro MVEX,a1, a2, tmp
    LDR \tmp,[\a2],#8
    STR \tmp,[\a1],#8
.endm
ENTRY(sleep_prepare_last)
    STP X29, X30, [SP,#-32]!
    MOV X29, SP
    STP X27, X28, [X29,#16]
    ADR X4, sleep_save_L2CTLR_EL1
    MRS X5, S3_1_C11_C0_2
    STR X5, [X4],#8
    MRS X5, S3_1_C11_C0_3
    STR X5, [X4],#8
    MRS X5, S3_1_C15_C0_0
    STR X5, [X4],#8
    MRS X5, PAR_EL1
    STR X5, [X4],#8
    MRS X5, S3_1_C15_C2_0
    STR X5, [X4],#8
    MRS X5, S3_1_C15_C2_1
    STR X5, [X4],#8
    MRS X5, S3_1_c15_c2_2
    STR X5, [X4],#8
    MRS X5, TPIDR_EL1
    STR X5, [X4],#8
    MRS X5, CNTP_CVAL_EL0
    STR X5, [X4],#8
    MRS X5, CNTP_TVAL_EL0
    STR X5, [X4],#8
    MRS X5, CNTV_CVAL_EL0
    STR X5, [X4],#8
    MRS X5, CNTV_TVAL_EL0
    STR X5, [X4],#8
    MRS X5, CNTP_CTL_EL0
    STR X5, [X4],#8
    MRS X5, CNTV_CTL_EL0
    STR X5, [X4],#8
    MRS X5, CNTKCTL_EL1
    STR X5, [X4],#8
    ADR X4, wakup
    ADR X5, sleep_saved_magic
    LDR X6, [X4],#8
    MOV X0, X6
    LDR X27, =MSTAR_SLEEP_MAGIC
    STR X27, [X5],#8
    MVEX1 X5, X6, X4, X7
    MVEX1 X5, X6, X4, X7
    MVEX1 X5, X6, X4, X7
    LDR X28, =WAKEUP_SAVE_ADDR
    MOV X1, X28
    ADR X2, sleep_save2
    MVEX X2, X1, X7
    ADR X0, sleep_wakeup_first
    BL  mstar_virt_to_phy
    STP W27, W0, [X28]
    MOV X0, X28
    BL  mstar_virt_to_phy
    ADR X1, sleep_save_wakup_save
    STR X0, [X1]
    LSR X0, X0, #20
    LSL X0, X0, #4
    ORR X0, X0, #WAKEUP_FLAG_SLEPT
    BL  mstar_pm_regw
    LDP X27, X28, [X29, #16]
    LDP X29, X30, [SP],#32
    RET
.align 3
       .quad .
wakup: .quad _text
wakup_vect:
    ADR X5, wakup2
    LDR X4, [X5]
    BR   X4
    BL   sleep_wakeup_first
wakup2: .quad ((PHYS_OFFSET-PAGE_OFFSET)-4+.)
ENTRY(sleep_set_wakeup_addr_phy)
    ADR X5, sleep_save_wakup_vir
    STP X1, X0, [X5]
    RET
ENTRY(sleep_suspend_ret)
    ADR X5, sleep_save_spt
    LDP X29, X30, [X5],#16
    ADR X5, sleep_save_regs
    LDP X16,X17, [X5],#16
    LDP X18,X19, [X5],#16
    LDP X20,X21, [X5],#16
    LDP X22,X23, [X5],#16
    LDP X24,X25, [X5],#16
    LDP X26,X27, [X5],#16
    LDP X28,X8, [X5],#16
    RET
ENTRY(sleep_save_cpu_registers)
    ADR X5, sleep_save_spt
    STP X29, X30, [X5]
    ADR X5, sleep_save_regs
    STP X16,X17, [X5],#16
    STP X18,X19, [X5],#16
    STP X20,X21, [X5],#16
    STP X22,X23, [X5],#16
    STP X24,X25, [X5],#16
    STP X26,X27, [X5],#16
    STP X28,X8, [X5],#16
    STP X29, X30, [SP,#-16]!
    MOV X29, SP
    MOV X0, XZR
    ADR X1, sleep_suspend_ret
    BL  cpu_suspend
    LDP X29, X30, [SP],#16
    ADR X5, sleep_save_ret_addr
    LDP X5, X30, [X5]
    RET
/*------------------------------------------------------------------------------
    Function: sleep_wakeup_first

    Description:
        wake up first.
    Input: (The arguments were used by caller to input data.)
        x0: boot start code
    Output: (The arguments were used by caller to receive data.)
        none
    Return:
        None.
    Remark:
        x0-x5: damaged
-------------------------------------------------------------------------------*/
ENTRY(sleep_wakeup_first)
    MOV X19, X0
    MOV X20, X1
    MOV X21, X5
    BL  __flush_dcache_all
    IC IALLU
    TLBI VMALLE1IS
    DSB SY
    ISB
    SUB X5, X21, #16
    ADR X6, sleep_saved_magic
    STR XZR, [X6],#8
    MVEX X5,X6,X7
    MVEX X5,X6,X7
    MVEX X5,X6,X7
    ADR X2, sleep_save2
    LDP X0, X1, [X2]
    STR X0, [X1]
    ADR X4, sleep_save_L2CTLR_EL1
    LDR X5, [X4],#8
    MSR S3_1_C11_C0_2, X5
    LDR X5, [X4],#8
    MSR S3_1_C11_C0_3, X5
    LDR X5, [X4],#8
    MSR S3_1_C15_C0_0, X5
    LDR X5, [X4],#8
    MSR PAR_EL1, X5
    LDR X5, [X4],#8
    MSR S3_1_C15_C2_0, X5
    LDR X5, [X4],#8
    MSR S3_1_C15_C2_1, X5
    LDR X5, [X4],#8
    MSR S3_1_c15_c2_2, X5
    LDR X5, [X4],#8
    MSR TPIDR_EL1, X5
    LDR X5, [X4],#8
    MSR CNTP_CVAL_EL0, X5
    LDR X5, [X4],#8
    MSR CNTP_TVAL_EL0, X5
    LDR X5, [X4],#8
    MSR CNTV_CVAL_EL0, X5
    LDR X5, [X4],#8
    MSR CNTV_TVAL_EL0, X5
    LDR X5, [X4],#8
    MSR CNTP_CTL_EL0, X5
    LDR X5, [X4],#8
    MSR CNTV_CTL_EL0, X5
    LDR X5, [X4],#8
    MSR CNTKCTL_EL1, X5
    B   cpu_resume
ENTRY(save_performance_monitors)
    MOV X1, X0
    MRS X2, PMCR_EL0
    BIC X3, X2, #1
    MSR PMCR_EL0, X3
    ISB
    MRS X3, PMSELR_EL0
    MRS X4, PMOVSSET_EL0
    MRS X5, PMCNTENSET_EL0
    MRS X6, PMINTENSET_EL1
    MRS X7, PMUSERENR_EL0
    STR W2, [X1], #4
    STR W3, [X1], #4
    STR W4, [X1], #4
    STR W5, [X1], #4
    STR W6, [X1], #4
    STR W7, [X1], #4
    MRS X3, PMCCNTR_EL0
    STR W3, [X1], #4
    LSR X3, X3, #32
    STR W3, [X1], #4
    MRS X3, PMCCFILTR_EL0
    STR W3, [X1], #4
    UBFX X3, X2, #11, #5
    TST X3, X3
    BEQ 1f
2:
    SUBS X3, X3, #1
    MSR PMSELR_EL0, X3
    ISB
    MRS X4, PMXEVCNTR_EL0
    MRS X5, PMXEVTYPER_EL0
    STR W4, [X1], #4
    STR W5, [X1], #4
    BNE 2b
1:
    RET
ENDPROC(save_performance_monitors)

ENTRY(restore_performance_monitors)
    MOV X1, X0
    LDR W2, [X1], #4
    BIC X3, X2, #1
    MSR PMCR_EL0, X3
    ISB
    LDR W3, [X1], #4
    LDR W4, [X1], #4
    LDR W5, [X1], #4
    LDR W6, [X1], #4
    LDR W7, [X1], #4
    MSR PMOVSSET_EL0,X4
    MSR PMCNTENSET_EL0,X5
    MSR PMINTENSET_EL1,X6
    MSR PMUSERENR_EL0,X7
    MOV X4, XZR
    LDR W4, [X1], #4
    LDR W5, [X1], #4
    LDR W6, [X1], #4
    ORR X4, X4, X5, LSL #32
    MSR PMCCNTR_EL0, X4
    MSR PMCCFILTR_EL0, X6
    UBFX X4, X2, #11, #5
    TST X4, X4
    BEQ 1f
2:
    SUBS X4, X4, #1
    MSR PMSELR_EL0, X4
    ISB
    LDR W5, [X1], #4
    LDR W6, [X1], #4
    MSR PMXEVCNTR_EL0,X5
    MSR PMXEVTYPER_EL0,X6
    BNE 2b
1:
    MSR PMSELR_EL0,X3
    MSR PMCR_EL0,X2
    RET
ENDPROC(restore_performance_monitors)

    .align 2
SerPrintPhyStk: .fill 256, 8, 0
SerPrintPhyStk_bottom: .quad 0,0,0,0
sleep_save_registers: .fill SLEEPDATA_SIZE, 8, 0

ENTRY(mstar_fastboot_save)
	adr x0, SerPrintPhyStk_bottom
	str x30, [x0]
	bl mstar_suspend_save
	adr x0, sleep_save_registers
    bl  sleep_save_cpu_registers
    adr x0, SerPrintPhyStk_bottom
    ldr x30, [x0]
	ret
#if 0
    adrl r0, SerPrintPhyStk_bottom
    str lr, [r0]
    bl  mstar_save_context
    adrl r0, sleep_save_registers
    bl  sleep_save_cpu_registers
    adrl r0, SerPrintPhyStk_bottom
    ldr lr, [r0]
    mov pc, lr
#endif

ENTRY(mstar_fastboot_restore)
    adr x0, SerPrintPhyStk_bottom
    str x30, [x0]
    adr x0, sleep_save_registers
    //bl  sleep_restore_cpu_registers
    bl  mstar_resume_restore
	bl  mstar_sleep_cur_cpu_flush
    adr x0, SerPrintPhyStk_bottom
    ldr x30, [x0]
	ret

#if 0
    adrl r0, SerPrintPhyStk_bottom
    adrl r0, SerPrintPhyStk_bottom
    str lr, [r0]
    adrl r0, sleep_save_registers
    bl  sleep_restore_cpu_registers
    bl  mstar_restore_context
    adrl r0, SerPrintPhyStk_bottom
    ldr lr, [r0]
    mov pc, lr
#endif
