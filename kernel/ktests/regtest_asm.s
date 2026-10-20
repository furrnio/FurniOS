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


_throw_undef:
    udf #0
    

.global _test_return_jump
_test_return_jump
    mov, r0, #10
    b 1f
    _throw_undef
    _throw_undef
    _throw_undef
    _throw_undef

1:
    subs r0, r0, #1
    bne 2b
    bxeq lr
    _throw_undef
    _throw_undef
    _throw_undef
    _throw_undef

2: b 1f
    _throw_undef
    _throw_undef
    _throw_undef
    _throw_undef

.global _test_registers
_test_registers:
    push {r0-r5}
    mov r0  r6
    mov r1  r7
    mov r2  r8
    mov r3  r9
    mov r4  r10
    mov r5  r11

    add r0  r0  137
    add r1  r1  137
    add r2  r2  137
    add r3  r3  137
    add r4  r4  137
    add r5  r5  137

    be  r0  r6  _undef
    be  r1  r7  _undef
    be  r2  r8  _undef
    be  r3  r9  _undef
    be  r4  r10 _undef
    be  r5  r11 _undef

    add r6  r6  137
    add r7  r7  137
    add r8  r8  137
    add r9  r9  137
    add r10 r10 137
    add r11 r11 137

    bne r0  r6  _undef
    bne r1  r7  _undef
    bne r2  r8  _undef
    bne r3  r9  _undef
    bne r4  r10 _undef
    bne r5  r11 _undef

_undef:
    /* Do an undef */

