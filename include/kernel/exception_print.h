#ifndef KERNEL_EXCEPTION_PRINT_H
#define KERNEL_EXCEPTION_PRINT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "kernel/kprintf.h"

// Global irq debug flag
extern volatile bool irq_debug;

struct exception_infos {
    char* exception_name;
    uint32_t exception_source_addr;

    uint32_t data_fault_status_register;
    uint32_t data_fault_address_register;
    uint32_t instruction_fault_status_register;
    uint32_t instruction_fault_address_register;

    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;
    uint32_t r12;

    bool is_data_abort;
    bool is_prefetch_abort;
};

struct mode_regs {
    uint32_t cpsr;
    uint32_t lr_usr;
    uint32_t sp_usr;
    uint32_t lr_irq;
    uint32_t sp_irq;
    uint32_t spsr_irq;
    uint32_t lr_abt;
    uint32_t sp_abt;
    uint32_t spsr_abt;
    uint32_t lr_und;
    uint32_t sp_und;
    uint32_t spsr_und;
    uint32_t lr_svc;
    uint32_t sp_svc;
    uint32_t spsr_svc;
};

void print_exception_infos(struct exception_infos* infos);

struct mode_regs read_mode_specific_registers(void);

void print_psr(unsigned int psr);

const char* get_fsr_description(unsigned int fsr);

#endif // KERNEL_EXCEPTION_PRINT_H
