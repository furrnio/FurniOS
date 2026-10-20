/* 
    ======================================================================
    This file is part of FurniOS.

    Copyright (C) 2026 Oliver Arendt (furnio, fkolleg)
    www.fkolleg.de
    info@fkolleg.de

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 3
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, see
    <https://www.gnu.org/licenses/>.

    Additional restriction:
    This software may not be used, in whole or in part, to train machine 
    learning models (including LLMs) without explicit prior written 
    permission.
    Violations will terminate your rights under this license.

    ======================================================================
*/

#include <drivers/timer.h>
#include <kernel/kprintf.h>
#include <kernel/exception_print.h>
#include <kernel/log.h>
#include <stdint.h>
#include <stdbool.h>

#define INTERRUPT_BASE  0x3F00B000
#define IRQ_PENDING1    (INTERRUPT_BASE + 0x204)

// TODO Später in scheduler.h
typedef struct {
    uint32_t r[13];
    uint32_t lr;
    uint32_t pc;
    uint32_t sp;
    uint32_t cpsr;
} cpu_context_t;

extern uint32_t* handle_undef(uint32_t *sp);
extern uint32_t* handle_svc(uint32_t *sp);
extern uint32_t* handle_prefetch_abort(uint32_t *sp);
extern uint32_t* handle_data_abort(uint32_t *sp);
extern uint32_t* handle_irq(uint32_t *sp);

extern volatile bool irq_debug;

static void _save_exception_infos(uint32_t *sp, 
                                  struct exception_infos *infos)
{
    int reg_offset = 3;  

    infos->exception_source_addr = sp[2] - 4;   // one before $lr 
    
    infos->r0     = sp[ 0 + reg_offset];
    infos->r1     = sp[ 1 + reg_offset];
    infos->r2     = sp[ 2 + reg_offset];
    infos->r3     = sp[ 3 + reg_offset];
    infos->r4     = sp[ 4 + reg_offset];
    infos->r5     = sp[ 5 + reg_offset];
    infos->r6     = sp[ 6 + reg_offset];
    infos->r7     = sp[ 7 + reg_offset];
    infos->r8     = sp[ 8 + reg_offset];
    infos->r9     = sp[ 9 + reg_offset];
    infos->r10    = sp[10 + reg_offset];
    infos->r11    = sp[11 + reg_offset];
    infos->r12    = sp[12 + reg_offset];

    asm volatile ("mrc p15, 0, %0, c5, c0, 0"                   
        : "=r" (infos->data_fault_status_register));             
    asm volatile ("mrc p15, 0, %0, c6, c0, 0"                   
        : "=r" (infos->data_fault_address_register));            
    asm volatile ("mrc p15, 0, %0, c5, c0, 1"                   
        : "=r" (infos->instruction_fault_status_register));      
    asm volatile ("mrc p15, 0, %0, c6, c0, 2"
        : "=r" (infos->instruction_fault_address_register));
}

[[maybe_unused]] static void _print_stack(uint32_t* sp, int n)
{
    klog_info("CHANDLER", "Stack from %p to %p\n", (void*) sp, (void*) sp - n);
    for (int i = 0; i < n; ++i) {
        kprintf("%08x ", sp[i]);
        if ((i+1) % 8 == 0) kprintf("\n");
    }
    kprintf("\n");
}

[[maybe_unused]] static void _print_cpu_context(cpu_context_t *context)
{
    klog_info("CHANDLER", "cpu_context: ");
    //for (int i = 0; i < 13; ++i) kprintf("r[%d] = %08p\n", i, context->r[i]);
    kprintf("lr = %08p | pc = %08p | cpsr = %08p\n", context->lr, context->pc, context->cpsr);
}

[[maybe_unused]] static void _save_context(uint32_t *sp, cpu_context_t *context) 
{
    int reg_offset = 4;

    context->cpsr   = sp[0];
    context->sp     = sp[1];
    context->lr     = sp[2];    // last LR_usr
    context->pc     = sp[3];    // last PC+4 to jump to

    for (int i = 0; i < 13; ++i) {
        context->r[i] = sp[i + reg_offset];
    }
}

[[maybe_unused]] static void _restore_context(uint32_t *sp, cpu_context_t *context) 
{
    int reg_offset = 4;

    sp[0] = context->cpsr;
    sp[1] = context->sp;
    sp[2] = context->lr;   // LR_usr := syscall_exit() | last PC+4
    sp[3] = context->pc;   // LR_svc := func() | last PC

    for (int i = 0; i < 13; ++i) {
        sp[i + reg_offset] = context->r[i];
    }
}

uint32_t* handle_undef(uint32_t *sp)
{
    struct exception_infos infos = { 0 };          
    infos.exception_name = "Undefined Instruction";           

    infos.is_data_abort = false;
    infos.is_prefetch_abort = false;

    _save_exception_infos(sp, &infos);
    print_exception_infos(&infos);

    return sp;
}

uint32_t* handle_svc(uint32_t *sp)
{
    struct exception_infos infos = { 0 };          
    infos.exception_name = "Supervisor Call";           

    infos.is_data_abort = false;
    infos.is_prefetch_abort = false;

    _save_exception_infos(sp, &infos);
    print_exception_infos(&infos);

    return sp;
}

uint32_t* handle_prefetch_abort(uint32_t *sp)
{
    struct exception_infos infos;          
    infos.exception_name = "Prefetch Abort";           

    infos.is_data_abort = false;
    infos.is_prefetch_abort = true;
    _save_exception_infos(sp, &infos);
    print_exception_infos(&infos);

    return sp;
}

uint32_t* handle_data_abort(uint32_t *sp)
{
    struct exception_infos infos;          
    infos.exception_name = "Data abort";           

    infos.is_data_abort = true;
    infos.is_prefetch_abort = false;
    _save_exception_infos(sp, &infos);
    print_exception_infos(&infos);

    return sp;
}

uint32_t* handle_irq(uint32_t *sp)
{
    struct exception_infos infos;          
    infos.exception_name = "IRQ";           

    infos.is_data_abort = false;
    infos.is_prefetch_abort = true;
    _save_exception_infos(sp, &infos);
    
    uint32_t pending1 = *(volatile uint32_t*)(IRQ_PENDING1);

    if (pending1 & (1 << 1)) {
        timer_irq_handler();
    }

    if (irq_debug) {
        print_exception_infos(&infos);
    }

    return sp;
}

uint32_t* handle_fiq(uint32_t *sp)
{
    struct exception_infos infos;          
    infos.exception_name = "FIQ";           

    infos.is_data_abort = false;
    infos.is_prefetch_abort = true;
    _save_exception_infos(sp, &infos);
    print_exception_infos(&infos);

    return sp;
}
