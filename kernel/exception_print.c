#include "kernel/exception_print.h"

#define USR_MODE        0x10
#define IRQ_MODE        0x12
#define SVC_MODE        0x13
#define ABT_MODE        0x17
#define UND_MODE        0x1b
#define SYS_MODE        0x1f

#define SPEC_REG_SP     0x0
#define SPEC_REG_LR     0x1
#define SPEC_REG_SPSR   0x2


void print_exception_infos(struct exception_infos* infos) {

	kprintf("############ EXCEPTION ############\n");
	kprintf("%s an Adresse: 0x%08x\n", infos->exception_name, infos->exception_source_addr);
	if (infos->is_data_abort) {
		const char* dfsr_description = get_fsr_description(infos->data_fault_status_register);
		kprintf("Data Fault Status Register: 0x%08x -> %s\n", infos->data_fault_status_register, dfsr_description);
		kprintf("Data Fault Adress Register: 0x%08x\n", infos->data_fault_address_register);
	}
		
	if (infos->is_prefetch_abort) {
		const char* ifsr_description = get_fsr_description(infos->instruction_fault_status_register);
		kprintf("Instruction Fault Status Register: 0x%08x -> %s\n", infos->instruction_fault_status_register, ifsr_description);
		kprintf("Instruction Fault Adress Register: 0x%08x\n", infos->instruction_fault_address_register);
	}
			
	kprintf("\n>> Registerschnappschuss <<\n");
	kprintf("R0: 0x%08x  R5: 0x%08x  R10: 0x%08x\n", infos->r0, infos->r5, infos->r10);
	kprintf("R1: 0x%08x  R6: 0x%08x  R11: 0x%08x\n", infos->r1, infos->r6, infos->r11);
	kprintf("R2: 0x%08x  R7: 0x%08x  R12: 0x%08x\n", infos->r2, infos->r7, infos->r12);
	kprintf("R3: 0x%08x  R8: 0x%08x\n", infos->r3, infos->r8);
	kprintf("R4: 0x%08x  R9: 0x%08x\n", infos->r4, infos->r9);

	struct mode_regs mode_regs = read_mode_specific_registers();

	kprintf("\n>> Modusspezifische Register <<\n");
	kprintf("User/System | LR: 0x%08x | SP: 0x%08x | CPSR: ", mode_regs.lr_usr, mode_regs.sp_usr);
	print_psr(mode_regs.cpsr);
	kprintf("\nIRQ         | LR: 0x%08x | SP: 0x%08x | SPSR: ", mode_regs.lr_irq, mode_regs.sp_irq);
	print_psr(mode_regs.spsr_irq);
	kprintf("\nAbort       | LR: 0x%08x | SP: 0x%08x | SPSR: ", mode_regs.lr_abt, mode_regs.sp_abt);
	print_psr(mode_regs.spsr_abt);
	kprintf("\nUndefined   | LR: 0x%08x | SP: 0x%08x | SPSR: ", mode_regs.lr_und, mode_regs.sp_und);
	print_psr(mode_regs.spsr_und);
	kprintf("\nSupervisor  | LR: 0x%08x | SP: 0x%08x | SPSR: ", mode_regs.lr_svc, mode_regs.sp_svc);
	print_psr(mode_regs.spsr_svc);
	kprintf("\n");
}


#define _GET_SPEC_REG(cmode, tmode, spec_reg, spec_reg_str, ret_reg) {                  \
        switch (spec_reg) {                                                             \
            case SPEC_REG_SPSR: {                                                       \
                if ((cmode) == (tmode))                                                 \
                    asm volatile ("mrs %0, spsr" : "=r" (ret_reg));                     \
                else                                                                    \
                    asm volatile ("mrs %0, " spec_reg_str : "=r" (ret_reg));            \
                break;                                                                  \
            }                                                                           \
            case SPEC_REG_LR: {                                                         \
                if ((cmode) == (tmode))                                                 \
                    asm volatile ("mov %0, lr" : "=r" (ret_reg));                       \
                else                                                                    \
                    asm volatile ("mrs %0, " spec_reg_str : "=r" (ret_reg));            \
                break;                                                                  \
            }                                                                           \
            case SPEC_REG_SP: {                                                         \
                if ((cmode) == (tmode))                                                 \
                    asm volatile ("mov %0, sp" : "=r" (ret_reg));                       \
                else                                                                    \
                    asm volatile ("mrs %0, " spec_reg_str : "=r" (ret_reg));            \
                break;                                                                  \
            }                                                                           \
        }                                                                               \
    }                                                                                   \


struct mode_regs read_mode_specific_registers(void)
{
    struct mode_regs regs;

    asm volatile (" mrs     %[cpsr],    CPSR"   :
                    [cpsr]  "=r"        (regs.cpsr));

    uint32_t curr_mode = regs.cpsr & 0x1f;

    _GET_SPEC_REG(curr_mode, USR_MODE,  SPEC_REG_LR,    "lr_usr",   regs.lr_usr);
    _GET_SPEC_REG(curr_mode, USR_MODE,  SPEC_REG_SP,    "sp_usr",   regs.sp_usr);
    _GET_SPEC_REG(curr_mode, IRQ_MODE,  SPEC_REG_LR,    "lr_irq",   regs.lr_irq);
    _GET_SPEC_REG(curr_mode, IRQ_MODE,  SPEC_REG_SP,    "sp_irq",   regs.sp_irq);  
    _GET_SPEC_REG(curr_mode, IRQ_MODE,  SPEC_REG_SPSR,  "spsr_irq", regs.spsr_irq);
    _GET_SPEC_REG(curr_mode, ABT_MODE,  SPEC_REG_LR,    "lr_abt",   regs.lr_abt);
    _GET_SPEC_REG(curr_mode, ABT_MODE,  SPEC_REG_SP,    "sp_abt",   regs.sp_abt);
    _GET_SPEC_REG(curr_mode, ABT_MODE,  SPEC_REG_SPSR,  "spsr_abt", regs.spsr_abt);
    _GET_SPEC_REG(curr_mode, UND_MODE,  SPEC_REG_LR,    "lr_und",   regs.lr_und);
    _GET_SPEC_REG(curr_mode, UND_MODE,  SPEC_REG_SP,    "sp_und",   regs.sp_und);
    _GET_SPEC_REG(curr_mode, UND_MODE,  SPEC_REG_SPSR,  "spsr_und", regs.spsr_und);
    _GET_SPEC_REG(curr_mode, SVC_MODE,  SPEC_REG_LR,    "lr_svc",   regs.lr_svc);
    _GET_SPEC_REG(curr_mode, SVC_MODE,  SPEC_REG_SP,    "sp_svc",   regs.sp_svc);
    _GET_SPEC_REG(curr_mode, SVC_MODE,  SPEC_REG_SPSR,  "spsr_svc", regs.spsr_svc);

    return regs;
}


void print_psr(unsigned int psr) {

    uint32_t mode = psr & 0x1f;
    
    uint32_t psr_n = (psr >> 31) & 1;
    uint32_t psr_z = (psr >> 30) & 1;
    uint32_t psr_c = (psr >> 29) & 1;
    uint32_t psr_v = (psr >> 28) & 1;
    uint32_t psr_e = (psr >>  9) & 1;
    uint32_t psr_i = (psr >>  7) & 1;
    uint32_t psr_f = (psr >>  6) & 1;
    uint32_t psr_t = (psr >>  5) & 1;
    
	kprintf("%c%c%c%c %c %c%c%c", 
		psr_n ? 'N' : '_',
		psr_z ? 'Z' : '_',
		psr_c ? 'C' : '_',
		psr_v ? 'V' : '_',
		psr_e ? 'E' : '_',
		psr_i ? 'I' : '_',
		psr_f ? 'F' : '_',
		psr_t ? 'T' : '_'
	);

    const char* right_aligned_mode_names[0x20] = { 
        [0x00] = "   Invalid",
        [0x10] = "      User",
        [0x12] = "       IRQ",
        [0x13] = "Supervisor",
        [0x17] = "     Abort",
        [0x1b] = " Undefined",
        [0x1f] = "    System",
    };

    if (mode != 0x10 && 0x12 && mode != 0x13 && 
        mode != 0x17 && mode != 0x1b && mode != 0x1f) {
            mode = 0x00;
    }

	kprintf(" %s", right_aligned_mode_names[mode]);
	kprintf(" 0x%08x", psr);
}

const char* get_fsr_description(unsigned int fsr) {
    static const char *fsr_sources[] = {
        [0b00000] =  "No function, reset value",
        [0b00001] =  "Alignment fault",
        [0b00010] =  "Debug event fault",
        [0b00011] =  "Access Flag fault on Section",
        [0b00100] =  "Cache maintenance operation fault",
        [0b00101] =  "Translation fault on Section",
        [0b00110] =  "Access Flag fault on Page",
        [0b00111] =  "Translation fault on Page",
        [0b01000] =  "Precise External Abort",
        [0b01001] =  "Domain fault on Section",
        [0b01011] =  "Domain fault on Page",
        [0b01100] =  "External abort on Section",
        [0b01101] =  "Permission fault on Section",
        [0b01110] =  "External abort on Page",
        [0b01111] =  "Permission fault on Page",
        [0b10000] =  "TLB conflict abort",
        [0b10100] =  "Implementation defined fault",
        [0b10110] =  "External Abort",
        [0b11000] =  "Asynchronous parity error on memory access",
        [0b11001] =  "Synchronous parity error on memory access",
        [0b11010] =  "Implementation defined fault",
        [0b11100] =  "Synchronous parity error on translation table walk on section",
        [0b11110] =  "Synchronous parity error on translation table walk on page",
    };

    const int fsr_status_4_index = 10;
    unsigned int fsr_status = (fsr & 0b1111) |
     ((fsr & (1 << (fsr_status_4_index - 1))) >> (fsr_status_4_index - 4 - 1));

    if (fsr_status > sizeof(fsr_sources) / sizeof(const char*) ||
       fsr_sources[fsr_status] == NULL) {
        return "Invalid fault status register value";
    }

    return fsr_sources[fsr_status];
}
