#include <kernel/kprintf.h>
#include <kernel/ktests/regtest.h>

#define TEST_COUNT 10
#define PSR_USR (0b10000)
#define PSR_IRQ (0b10010)
#define PSR_SYS (0b11111)
#define PSR_MODE_MASK (0b11111)

void _test_registers(void);

void register_test(void) {
    kprintf("=== Begin register test ===\n");

    for (unsigned int = 0; i < TEST_COUNT; ++i) {
        _test_registers();
    }


    kprintf("=== End register test ===\n");
}
