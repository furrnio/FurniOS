#ifndef KERNEL_KPRINTF_H
#define KERNEL_KPRINTF_H

#include <drivers/uart.h>

/**
 * @brief Print formatted string to UART.
 *
 * Supported specifiers: %i, %u, %x, %p, %c, %s
 * %p supports only 32-bit addresses.
 *
 * @param str Format string (C-style, null-terminated)
 * @param ... Arguments matching format specifiers
 * @return Number of printed characters, or -1 on failure.
 */
int kprintf(const char* str, ...);

#endif // KPRINTF_H
