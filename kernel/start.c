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
#include <drivers/uart.h>
#include <drivers/status_led.h>
#include <drivers/timer.h>
#include <stdbool.h>

bool irq_debug = false;

static void do_data_abort(void);
static void do_prefetch_abort(void);
static void do_supervisor_call(void);
static void do_undefined_inst(void);

void start_kernel [[noreturn]] (void) 
{
    status_on();
    kprintf("Status led switched on...\n");

    timer_init();
    kprintf("Timer initialized...\n");

    // Enable interrupts
    asm volatile ("cpsie if");
    kprintf("Interrupts enabled...\n");

	kprintf("=== FurniOS booted ===\n");

    kprintf("now32 = %d\n", timer_now32());
    kprintf("now64 = %d\n", timer_now64());
    
	while(true) {
		char c = uart_getc();
		switch(c) {
			case 'd':
				irq_debug = !irq_debug;
				break;
			case 'a':
				do_data_abort();
				break;
			case 'p':
				do_prefetch_abort();
				break;
			case 's':
				do_supervisor_call();
				break;
			case 'u':
				do_undefined_inst();
				break;
			case 'c':
                // check();
				break;
			default:
				kprintf("Unknown input: %c\n", c);
				break;
		}
	}
}

void do_data_abort() 
{
    kprintf("Do data abort...\n");
    asm volatile ("mov r0, #0x1\n\t"        // lr = 0x8844, mode = 0x13
                  "ldr r0, [r0, #0]");
}

void do_prefetch_abort() 
{
    kprintf("Do prefetch abort...\n");
    asm volatile ("bkpt #0");
}

void do_supervisor_call() 
{
    kprintf("Do supervisor call...\n");
    asm volatile ("svc #0x1");
}

void do_undefined_inst() 
{
    kprintf("Do undefined instruction...\n");
    asm volatile ("udf #0");
}
