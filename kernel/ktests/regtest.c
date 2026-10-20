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

#include <kernel/kprintf.h>
#include <kernel/ktests/regtest.h>

#define TEST_COUNT 10
#define PSR_USR (0b10000)
#define PSR_IRQ (0b10010)
#define PSR_SYS (0b11111)
#define PSR_MODE_MASK (0b11111)

void _test_registers(void);

void register_test(void) {
    kprintf("=== Begin register test ===\n");

    for (unsigned int = 0; i < TEST_COUNT; ++i) {
        _test_registers();
    }


    kprintf("=== End register test ===\n");
}
