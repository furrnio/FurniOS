.section .init

.global reset
.global init_stack

reset:
    /* Initialize stack */
    bl init_stack

	/* Is hypervisor mode active */
	mrs r0, cpsr
	and r0, r0, #0x1F
	mov r1, #0x1A	    // mode bits 0x1A -> hypervisor mode
	cmp r0, r1		    // Compare and set status bits for conditional jump

	/* If hypervisor mode active: exit (jump to _exit_hyper)
	 * Else: Continue to _furnios
	 */
	beq _exit_hyper

	/* We only need one core.
     * => Send other three cores into infinity loop 
	 */
_check_cores:
	/* Request id of the cpu core */
	mrc p15, 0, r0, c0, c0, 5
	/* If not core 0 => infinity loop */
	tst r0, #3
	bne _shut_off_core

/* not modeled in qemu 6.0 */
_enable_align_check:
	/* Load SCTLR of cp15 */
	mrc p15, 0, r0, c1, c0, 0
	/* Set A-bit (c1[1]) to 1 */
	orr r0, r0, #0x2
	/* Save new SCTLR */
	mcr p15, 0, r0, c1, c0, 0


_furnios:
	/* Set valid stack pointer */
	ldr sp,=0x100000
	/* Jump to c code */
	bl  start_kernel
.Lend:
	WFI
	b .Lend

_shut_off_core:
	/* Disable interrupts for core 1, 2, 3  */
	cpsid if
	/* Jump to infinity loop */
	b .Lend

_exit_hyper:
	/* Rücksprungadresse ins Hypervisor Rücksprungregister schreiben.
	 * Dies ist das Label _furnios für die normale Programmausfürung.
	 */
	ldr lr, =_check_cores
	msr ELR_hyp, lr

	/* Der Hypervisor mode ist auch "nur" eine etwas kompliziertere Exception.
	 * Also springen wir aus dieser zurück.
	 * (Rücksprunge in Aufgabe 2 sind anders zu lösen!).
	 * Dazu zunächst das CPSR auslesen und die Modebits zum Supervisor ändern.
	 * Danach dies in das SPSR für den Rücksprung schreiben.
	 */
	mrs r0, cpsr
	bic r0, r0, #0x1F
	orr r0, r0, #0x13	/* supervisor mode */
	msr spsr_hyp, r0
	eret
