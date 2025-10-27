#include <drivers/uart.h>
#include <drivers/io.h>
#include <stdint.h>

// UART IO adresses in memory
#define UART_BASE           0x3F201000u
#define UART_DR             (UART_BASE + 0x00u)
#define UART_FR             (UART_BASE + 0x18u)

// UART_FR bits
#define UART_FR_TXFF_BIT    5u
#define UART_FR_RXFE_BIT    4u


int uart_putc(unsigned char c)
{
    // Wait until TX FIFO is not full
    while (read32(UART_FR) & (1u << UART_FR_TXFF_BIT)) {}
    write32(UART_DR, ((uint32_t)c) & 0xFF);
    return 0;
}

int uart_getc(void)
{
    // Wait until RX FIFO has data
    while (read32(UART_FR) & (1 << UART_FR_RXFE_BIT)) {}
    return (int)(read32(UART_DR) & 0xFF);
}
