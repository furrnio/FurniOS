#include "config.h"
#include "kernel/kprintf.h"
#include "drivers/uart.h"
#include "tests/regcheck.h"
#include "stdbool.h"

static void subprogram [[noreturn]] (void);
static void do_data_abort(void);
static void do_prefetch_abort(void);
static void do_supervisor_call(void);
static void do_undefined_inst(void);

void start_kernel [[noreturn]] (void) {
	kprintf("=== FurniOS booted ===\n");
	test_kernel();

    bool irq_debug = false;

    kprintf("READY\n");

#if 1
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
				register_checker();
				break;
			case 'e':
				subprogram();
			default:
				kprintf("Unknown input: %c\n", c);
				break;
		}
	}
}

static void subprogram [[noreturn]] (void) {
	while(true) {
		char c = uart_getc();
		for(unsigned int n = 0; n < PRINT_COUNT; n++) {
			uart_putc(c);
			volatile unsigned int i = 0;
			for(; i < BUSY_WAIT_COUNTER; i++) {}
		}
	}
#endif
}

void do_data_abort() 
{
    kprintf("Do data abort...\n");
}

void do_prefetch_abort() 
{
    kprintf("Do prefetch abort...\n");
}

void do_supervisor_call() 
{
    kprintf("Do supervisor call...\n");

    __asm__ volatile ("svc #0x1");
}

void do_undefined_inst() 
{
    kprintf("Do undefined instruction...\n");
}
