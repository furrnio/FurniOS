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

#include <drivers/io.h>
#include <drivers/timer.h>
#include <stdint.h>
#include <kernel/kprintf.h>

/* Set timer 1. Timer 3 can be added optionally. 
 * Timer 0 and 2 are reserved for GPU use 
 */

#define INTERRUPT_BASE      0x3f00b000
#define IRQ_ENABLE1         (INTERRUPT_BASE + 0x210)
#define IRQ_DISABLE1        (INTERRUPT_BASE + 0x21c)

#define TIMER_BASE          0x3f003000
#define TIMER_CS            (TIMER_BASE + 0x0)
#define TIMER_CLO           (TIMER_BASE + 0x04)
#define TIMER_CHI           (TIMER_BASE + 0x88)
#define TIMER_C1            (TIMER_BASE + 0x10)

#define TIMER_CS_M1         0b10
#define TIMER_1             1u

#define TIMER_INTERVAL      10000000

static uint32_t interval = 1000000;
static uint32_t last_comp, next_comp;

/* Calculate next comparison value by adding the interval 
 * to the last comparison value to reduce the drift. */
static void reset_timer() {
    do {
        last_comp = next_comp;
        next_comp = last_comp + interval;
    } while (next_comp < read32(TIMER_CLO));

    write32(TIMER_C1, next_comp);
}

/* Initialize timer */
void timer_init(void)
{
    reset_timer();
    write32(IRQ_ENABLE1, 1 << TIMER_1);
}

/* Handle interrupt */
void timer_irq_handler(void)
{
    // Reset timer interrupt
    if (read32(TIMER_CS) & TIMER_CS_M1) {
        write32(TIMER_CS, TIMER_CS_M1);
        reset_timer();
        kprintf(".");
    }
}

/* Set timer interval */
void timer_set_interval(uint32_t d)
{
    interval = d;
}

/* Get current time as 32-bit unsigned integer */
uint32_t timer_now32(void)
{
   return read32(TIMER_CLO);
}

/* Get current time as 64-bit unsigned integer */
uint64_t timer_now64(void)
{
    uint32_t chi = read32(TIMER_CHI);
    uint32_t clo = read32(TIMER_CLO);
    return ((uint64_t) chi << 32) | (uint64_t) clo;
}

/* Stop timer */
void timer_stop(void)
{
    write32(IRQ_DISABLE1, 1 << TIMER_1);
}
