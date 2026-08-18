#include "kernel/kprintf.h"
#include "drivers/uart.h"
#include "stdbool.h"

static void do_data_abort(void);
static void do_prefetch_abort(void);
static void do_supervisor_call(void);
static void do_undefined_inst(void);

void start_kernel [[noreturn]] (void) {
	kprintf("=== FurniOS booted ===\n");

    bool irq_debug = false;

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
