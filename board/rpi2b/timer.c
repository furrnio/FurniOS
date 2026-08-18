#include <drivers/io.h>
#include <drivers/timer.h>
#include <stdint.h>
#include <kernel/kprintf.h>

#define INTERRUPT_BASE      0x3f00b000
#define IRQ_ENABLE1         (INTERRUPT_BASE + 0x210)
#define IRQ_DISABLE1        (INTERRUPT_BASE + 0x21c)

#define TIMER_BASE          0x3f003000
#define TIMER_CS            (TIMER_BASE + 0x0)
#define TIMER_CLO           (TIMER_BASE + 0x04)
#define TIMER_C1            (TIMER_BASE + 0x10)

#define TIMER_INTERVAL      10000000

static uint32_t comp_value;

static void reset_timer() {
    uint32_t current_time = read32(TIMER_CLO);
    comp_value = current_time + TIMER_INTERVAL;
    write32(TIMER_C1, comp_value);
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
    if (read32(TIMER_CS) & 0x2) {
        write32(TIMER_CS, (1 << 1));
        reset_timer();
        kprintf(".");
    }
}

/* Stop timer */
void timer_stop(void)
{
    write32(IRQ_DISABLE1, 1 << 1);
}
