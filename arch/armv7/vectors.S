/*
 * Interrupt Vector Table
 */

.section .vector_table, "ax"

.global _ivt
.balign 64

_ivt:
    b       reset
    ldr     pc, _undefined_instruction
    ldr     pc, _supervisor_call
    ldr     pc, _prefetch_abort
    ldr     pc, _data_abort
    nop 
    ldr     pc, _irq
    ldr     pc, _fiq
_undefined_instruction: .word undef_handler
_supervisor_call:       .word svc_handler
_prefetch_abort:        .word prefetch_abort
_data_abort:            .word data_abort
_irq:                   .word irq
_fiq:                   .word irq
