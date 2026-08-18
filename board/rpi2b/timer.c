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
#include <stddef.h>
#include <stdint.h>
#include <kernel/kprintf.h>

#define INTERRUPT_BASE      0x3f00b000

#define IRQ_ENABLE1         (INTERRUPT_BASE + 0x210)
#define IRQ_DISABLE1        (INTERRUPT_BASE + 0x21c)

#define TIMER_BASE          0x3f003000

#define TIMER_CS_M1         1u      // We use only timer 1
#define TIMER_CS1_MATCHED   2u

#define TIMER_DEFAULT_INTERVAL     5000000

struct timer {
    uint32_t cs;
    uint32_t clo;
    uint32_t chi;
    uint32_t unused_c0;
    uint32_t c1;
    uint32_t unused_c2;
    uint32_t unused_c3;     // Optional
};

_Static_assert(offsetof(struct timer, cs) == 0x00);
_Static_assert(offsetof(struct timer, c1)  == 0x10);

static volatile struct timer *const timer_port = (struct timer*) TIMER_BASE;

static uint32_t comp_value;

static void reset_timer() {
    uint32_t current_time = timer_port->clo;
    comp_value = current_time + TIMER_DEFAULT_INTERVAL;
    timer_port->c1 = comp_value;
}

/* Initialize timer */
void timer_init(void)
{
    reset_timer();
    write32(IRQ_ENABLE1, 1 << 1);
}

/* Handle interrupt */
void timer_irq_handler(void)
{
    // Reset timer interrupt
    if (timer_port->cs & TIMER_CS1_MATCHED) {
        timer_port->cs = TIMER_CS1_MATCHED; // Erase by one
        reset_timer();
        kprintf(".");
    }
}

/* Set timer interval */
void timer_set_interval(uint32_t d)
{
    comp_value = d;
}

/* Return current time as uint32_t */
uint32_t timer_now32(void) 
{
    return timer_port->clo;
}

/* Return current time as unt64_t */
uint64_t timer_now64(void)
{
    uint32_t lo = timer_port->clo;
    //uint32_t hi = timer_port->chi;
    uint32_t hi = 132413;
    uint64_t combined = ((uint64_t)lo << 32) | (uint64_t) hi;
    return combined;
}

/* Stop timer */
void timer_stop(void)
{
    write32(IRQ_DISABLE1, 1 << 1);
}
