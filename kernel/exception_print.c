#include "kernel/exception_print.h"
#include <stdbool.h>

#define USR_MODE        (0x10)
#define IRQ_MODE        (0x12)
#define SVC_MODE        (0x13)
#define ABT_MODE        (0x17)
#define UND_MODE        (0x1b)
#define SYS_MODE        (0x1f)
#define PSR_MODE_MASK   (0x1f)

#define SPEC_REG_SP     (0x0)
#define SPEC_REG_LR     (0x1)
#define SPEC_REG_SPSR   (0x2)


void print_exception_infos(struct exception_infos *infos) {

    kprintf("\n–––––––––––––––––––––––––– EXCEPTION ––––––––––––––––––––––––––––––––\n");
    kprintf("  %s at 0x%08x\n", infos->exception_name, infos->exception_source_addr);
    kprintf("–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––\n");
    kprintf("  R0=0x%08x    R1=0x%08x    R2=0x%08x    R3=0x%08x\n", 
            infos->r0, infos->r1, infos->r2, infos->r3);
    kprintf("  R4=0x%08x    R5=0x%08x    R6=0x%08x    R7=0x%08x\n", 
            infos->r4, infos->r5, infos->r6, infos->r7);
    kprintf("  R8=0x%08x    R9=0x%08x    R10=0x%08x   R11=0x%08x\n", 
            infos->r8, infos->r9, infos->r10, infos->r11);
    kprintf("  R12=0x%08x\n",
            infos->r12);

    kprintf("–––––––––––––––––––––– MODE SPECIFIC REGISTERS –––––––––––––––––––––––\n");
    
	struct mode_regs mode_regs = read_mode_specific_registers();

	kprintf(  "  User/System  LR=0x%08x  SP=0x%08x  CPSR=0x%08x", 
            mode_regs.lr_usr, mode_regs.sp_usr, mode_regs.cpsr);
	kprintf("\n  IRQ          LR=0x%08x  SP=0x%08x  SPSR=0x%08x", 
            mode_regs.lr_irq, mode_regs.sp_irq, mode_regs.spsr_irq);
	kprintf("\n  Abort        LR=0x%08x  SP=0x%08x  SPSR=0x%08x", 
            mode_regs.lr_abt, mode_regs.sp_abt, mode_regs.spsr_abt);
	kprintf("\n  Undefined    LR=0x%08x  SP=0x%08x  SPSR=0x%08x", 
            mode_regs.lr_und, mode_regs.sp_und, mode_regs.spsr_und);
	kprintf("\n  Supervisor   LR=0x%08x  SP=0x%08x  SPSR=0x%08x", 
            mode_regs.lr_svc, mode_regs.sp_svc, mode_regs.spsr_svc);
	kprintf("\n");
    kprintf("–––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––\n");
}


void get_special_register(uint32_t current_mode, 
                          uint32_t target_mode, 
                          struct mode_regs *regs) {

    bool same_mode = (current_mode == target_mode);

    switch (target_mode) {

        case (USR_MODE):
            if (same_mode) {
                asm volatile ("mov %0, lr" : "=r" (regs->lr_usr));
                asm volatile ("mov %0, sp" : "=r" (regs->sp_irq));
            } else {
                asm volatile ("mrs %0, lr_usr" : "=r" (regs->lr_usr));
                asm volatile ("mrs %0, sp_usr" : "=r" (regs->sp_usr));
            }
            break;

        case (IRQ_MODE):
            if (same_mode) {
                asm volatile ("mrs %0, spsr" : "=r" (regs->spsr_irq));
                asm volatile ("mov %0, lr" : "=r" (regs->lr_irq));
                asm volatile ("mov %0, sp" : "=r" (regs->sp_irq));
            } else {
                asm volatile ("mrs %0, spsr_irq" : "=r" (regs->spsr_irq));
                asm volatile ("mrs %0, lr_irq" : "=r" (regs->lr_irq));
                asm volatile ("mrs %0, sp_irq" : "=r" (regs->sp_irq));
            }
            break;

        case (ABT_MODE):
            if (same_mode) {
                asm volatile ("mrs %0, spsr" : "=r" (regs->spsr_abt));
                asm volatile ("mov %0, lr" : "=r" (regs->lr_abt));
                asm volatile ("mov %0, sp" : "=r" (regs->sp_abt));
            } else {
                asm volatile ("mrs %0, spsr_abt" : "=r" (regs->spsr_abt));
                asm volatile ("mrs %0, lr_abt" : "=r" (regs->lr_abt));
                asm volatile ("mrs %0, sp_abt" : "=r" (regs->sp_abt));
            }
            break;

        case (UND_MODE):
            if (same_mode) {
                asm volatile ("mrs %0, spsr" : "=r" (regs->spsr_und));
                asm volatile ("mov %0, lr" : "=r" (regs->lr_und));
                asm volatile ("mov %0, sp" : "=r" (regs->sp_und));
            } else {
                asm volatile ("mrs %0, spsr_und" : "=r" (regs->spsr_und));
                asm volatile ("mrs %0, lr_und" : "=r" (regs->lr_und));
                asm volatile ("mrs %0, sp_und" : "=r" (regs->sp_und));
            }
            break;

        case (SVC_MODE):
            if (same_mode) {
                asm volatile ("mrs %0, spsr" : "=r" (regs->spsr_svc));
                asm volatile ("mov %0, lr" : "=r" (regs->lr_svc));
                asm volatile ("mov %0, sp" : "=r" (regs->sp_svc));
            } else {
                asm volatile ("mrs %0, spsr_svc" : "=r" (regs->spsr_svc));
                asm volatile ("mrs %0, lr_svc" : "=r" (regs->lr_svc));
                asm volatile ("mrs %0, sp_svc" : "=r" (regs->sp_svc));
            }
            break;
    }
}

struct mode_regs read_mode_specific_registers(void)
{
    struct mode_regs regs;

    asm volatile (" mrs     %[cpsr],    CPSR"   :
                    [cpsr]  "=r"        (regs.cpsr));

    uint32_t curr_mode = regs.cpsr & PSR_MODE_MASK;

    get_special_register(curr_mode, USR_MODE, &regs);
    get_special_register(curr_mode, IRQ_MODE, &regs);
    get_special_register(curr_mode, ABT_MODE, &regs);
    get_special_register(curr_mode, UND_MODE, &regs);
    get_special_register(curr_mode, SVC_MODE, &regs);

    return regs;
}
