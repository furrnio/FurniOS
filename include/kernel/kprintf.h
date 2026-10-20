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

#ifndef KERNEL_KPRINTF_H
#define KERNEL_KPRINTF_H

#include <drivers/uart.h>

/**
 * @brief Print formatted string to UART.
 *
 * Supported specifiers: %i, %u, %x, %p, %c, %s
 * %p supports only 32-bit addresses.
 *
 * @param str Format string (C-style, null-terminated)
 * @param ... Arguments matching format specifiers
 * @return Number of printed characters, or -1 on failure.
 */
int kprintf(const char* str, ...);

#endif // KPRINTF_H
