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

#include <drivers/uart.h>
#include <drivers/io.h>
#include <stdint.h>

// UART IO adresses in memory
#define UART_BASE           0x3F201000u
#define UART_DR             (UART_BASE + 0x00u)
#define UART_FR             (UART_BASE + 0x18u)

// UART_FR bits
#define UART_FR_TXFF_BIT    5u
#define UART_FR_RXFE_BIT    4u


int uart_putc(unsigned char c)
{
    // Wait until TX FIFO is not full
    while (read32(UART_FR) & (1u << UART_FR_TXFF_BIT)) {}
    write32(UART_DR, ((uint32_t)c) & 0xFF);
    return 0;
}

int uart_getc(void)
{
    // Wait until RX FIFO has data
    while (read32(UART_FR) & (1 << UART_FR_RXFE_BIT)) {}
    return (int)(read32(UART_DR) & 0xFF);
}
