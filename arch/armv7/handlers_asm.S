/*
 * Handle interrupts and exceptions 
 */

.section .text

reset_handler:
    bl reset   /* Restart */

.global undef_handler
undef_handler:
    bl handle_undef
    bx lr

.global svc_handler
svc_handler:
    bl handle_svc
    bx lr

.global prefetch_abort
prefetch_abort:
    bl handle_prefetch_abort
    bx lr

.global data_abort
data_abort:
    bl handle_data_abort
    bx lr

.global irq
irq:
    bl handle_irq
    bx lr

.global fiq
fiq:
    bl handle_fiq
    bx lr
