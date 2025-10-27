/*
 * Initialize stack pointer for CPU modes
 */

.section .text
.global init_stack

.equ Len_IRQ_Stack, 1024
.equ Len_SVC_Stack, 1024
.equ Len_ABT_Stack, 1024
.equ Len_UND_Stack, 1024
.equ Len_SYS_Stack, 1024

.equ Mode_IRQ, 0x12
.equ Mode_SVC, 0x13
.equ Mode_ABT, 0x17
.equ Mode_UND, 0x1B
.equ Mode_SYS, 0x1F

.equ F_Bit, 0x40   /* Bit 6 -> Disable FIQ */
.equ I_Bit, 0x80   /* Bit 7 -> Disable IRQ */

init_stack:
    ldr     R0, =0x80000        // Top of stack

    /* Enter each mode in turn and set up the stack pointer */

    cpsid   if, #Mode_IRQ
    //msr     CPSR_c, #(Mode_IRQ | I_Bit | F_Bit)
    mov     sp, R0
    sub     R0, R0, #Len_IRQ_Stack

    msr     CPSR_c, #(Mode_ABT | I_Bit | F_Bit)
    mov     sp, R0
    sub     R0, R0, #Len_ABT_Stack

    msr     CPSR_c, #(Mode_UND | I_Bit | F_Bit)
    mov     sp, R0
    sub     R0, R0, #Len_UND_Stack

    msr     CPSR_c, #(Mode_SYS | I_Bit | F_Bit)
    mov     sp, R0
    sub     R0, R0, #Len_SYS_Stack

    msr     CPSR_c, #(Mode_SVC | I_Bit | F_Bit)
    mov     sp, R0

    /* Leave processor in SVC mode */

    bx lr
