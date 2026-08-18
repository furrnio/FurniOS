#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

extern volatile uint32_t timer_ticks;

void timer_init(void);
void timer_irq_handler(void);
void timer_stop(void);

#endif /* TIMER_H */
