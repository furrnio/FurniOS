#ifndef UART_H
#define UART_H


/**
 * @brief Write one character to PL011 UART
 *
 * @return 0 on success 
 */ 
int uart_putc(unsigned char c);

/** 
 * @brief Read one character from PL011 UART
 *
 * @return read character
 */
int uart_getc();

#endif // UART_H
