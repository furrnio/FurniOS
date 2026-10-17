#include <drivers/status_led.h>
#include <stddef.h>

#define GPIO_BASE               (0x7E200000 - 0x3F000000)
#define STATUS_LED              7u
#define GPF_BITS                3u
#define GPF_MASK                0b111u
#define STATUS_LED_GPF_SHIFT    (STATUS_LED * GPF_BITS)

enum gpio_func {
	gpio_input  = 0x0,
	gpio_output = 0x1,
};

struct gpio {
	unsigned int func[6];
	unsigned int unused0;
	unsigned int set[2];
	unsigned int unused1;
	unsigned int clr[2];
};

_Static_assert(offsetof(struct gpio, func) == 0x00);
_Static_assert(offsetof(struct gpio, set)  == 0x1C);
_Static_assert(offsetof(struct gpio, clr)  == 0x28);

static volatile struct gpio *const gpio_port = (struct gpio *) GPIO_BASE;

void status_init(void)
{
	gpio_port->func[0] = 
        (gpio_port->func[0] & ~(GPF_MASK << STATUS_LED_GPF_SHIFT)) |
		(gpio_output << STATUS_LED_GPF_SHIFT);
}

void status_on(void)
{
	gpio_port->set[0] = 1 << STATUS_LED;
}

void status_off(void)
{
	gpio_port->clr[0] = 1 << STATUS_LED;
}
