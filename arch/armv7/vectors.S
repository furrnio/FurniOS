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
