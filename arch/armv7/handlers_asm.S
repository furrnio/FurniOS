/*
 * Handle interrupts and exceptions 
 */

.section .text

reset_handler:
    bl reset   /* Restart */

.global undef_handler
undef_handler:
    cpsid   if                      // disable IRQ/FIQ

    // Save USER context onto SVC stack
    push    {r0-r12}
    #sub     lr, lr, #4
    push    {lr}
    mrs     r1, lr_usr
    push    {r1}
    mrs     r1, spsr
    push    {r1}

    mov     r0, sp
    bl      handle_undef            // arg[0] := sp

    // Restore USER context from SVC stack
    mov     sp, r0                  // return =: sp

    pop     {r1}
    msr     spsr_cxsf, r1
    pop     {r1}
    msr     lr_usr, r1 
    pop     {lr}
    pop     {r0-r12}

    cpsie   if                      // enable IRQ/FIQ
    movs    pc, lr                  // return and switch mode

.global svc_handler
svc_handler:
    cpsid   if                      // disable IRQ/FIQ

    // Save USER context onto SVC stack
    push    {r0-r12}
    #sub     lr, lr, #4
    push    {lr}
    mrs     r1, lr_usr
    push    {r1}
    mrs     r1, spsr
    push    {r1}

    mov     r0, sp
    bl      handle_svc              // arg[0] := sp

    // Restore USER context from SVC stack
    mov     sp, r0                  // return =: sp

    pop     {r1}
    msr     spsr_cxsf, r1
    pop     {r1}
    msr     lr_usr, r1 
    pop     {lr}
    pop     {r0-r12}

    cpsie   if                      // enable IRQ/FIQ
    movs    pc, lr                  // return and switch mode

.global prefetch_abort
prefetch_abort:
    cpsid   if                      // disable IRQ/FIQ

    // Save USER context onto SVC stack
    push    {r0-r12}
    #sub     lr, lr, #4
    push    {lr}
    mrs     r1, lr_usr
    push    {r1}
    mrs     r1, spsr
    push    {r1}

    mov     r0, sp
    bl      handle_prefetch_abort   // arg[0] := sp

    // Restore USER context from SVC stack
    mov     sp, r0                  // return =: sp

    pop     {r1}
    msr     spsr_cxsf, r1
    pop     {r1}
    msr     lr_usr, r1 
    pop     {lr}
    pop     {r0-r12}

    cpsie   if                      // enable IRQ/FIQ
    movs    pc, lr                  // return and switch mode

.global data_abort
data_abort:
    cpsid   if                      // disable IRQ/FIQ

    // Save USER context onto SVC stack
    push    {r0-r12}
    sub     lr, lr, #4
    push    {lr}
    mrs     r1, lr_usr
    push    {r1}
    mrs     r1, spsr
    push    {r1}

    mov     r0, sp
    bl      handle_data_abort        // arg[0] := sp

    // Restore USER context from SVC stack
    mov     sp, r0                  // return =: sp

    pop     {r1}
    msr     spsr_cxsf, r1
    pop     {r1}
    msr     lr_usr, r1 
    pop     {lr}
    pop     {r0-r12}

    cpsie   if                      // enable IRQ/FIQ
    movs    pc, lr                  // return and switch mode

.global irq
irq:
    cpsid   if                      // disable IRQ/FIQ

    // Save USER context onto SVC stack
    push    {r0-r12}
    #sub     lr, lr, #4
    push    {lr}
    mrs     r1, lr_usr
    push    {r1}
    mrs     r1, spsr
    push    {r1}

    mov     r0, sp
    bl      handle_irq              // arg[0] := sp

    // Restore USER context from SVC stack
    mov     sp, r0                  // return =: sp

    pop     {r1}
    msr     spsr_cxsf, r1
    pop     {r1}
    msr     lr_usr, r1 
    pop     {lr}
    pop     {r0-r12}

    cpsie   if                      // enable IRQ/FIQ
    movs    pc, lr                  // return and switch mode

.global fiq
fiq:
    cpsid   if                      // disable IRQ/FIQ

    // Save USER context onto SVC stack
    push    {r0-r12}
    #sub     lr, lr, #4
    push    {lr}
    mrs     r1, lr_usr
    push    {r1}
    mrs     r1, spsr
    push    {r1}

    mov     r0, sp
    bl      handle_fiq              // arg[0] := sp

    // Restore USER context from SVC stack
    mov     sp, r0                  // return =: sp

    pop     {r1}
    msr     spsr_cxsf, r1
    pop     {r1}
    msr     lr_usr, r1 
    pop     {lr}
    pop     {r0-r12}

    cpsie   if                      // enable IRQ/FIQ
    movs    pc, lr                  // return and switch mode
