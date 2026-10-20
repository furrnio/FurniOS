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

.section .init

.global reset

/* Stack config */
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


reset:
    /* Initialize stack for each mode */
    ldr     R0, =0x80000        // Top of stack

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
