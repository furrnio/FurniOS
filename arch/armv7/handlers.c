#include "kernel/kprintf.h"

extern void handle_undef();
extern void handle_svc();
extern void handle_prefetch_abort();
extern void handle_data_abort();
extern void handle_irq();
extern void handle_fiq();

void handle_undef()
{
    kprintf("Handle undefined behaviour...\n");
}

void handle_svc()
{
    kprintf("Handle supervisor call...\n");
}

void handle_prefetch_abort()
{
    kprintf("Handle prefetch abort...\n");
}

void handle_data_abort()
{
    kprintf("Handle data abort...\n");
}

void handle_irq()
{
    kprintf("Handle irq...\n");
}

void handle_fiq()
{
    kprintf("Handle fiq...\n");
}
