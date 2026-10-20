/* 
    ======================================================================
    This file is part of FurniOS.

    Copyright (C) 2026 Oliver Arendt (furnio, fkolleg)
    www.fkolleg.de
    info@fkolleg.de

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 3
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, see
    <https://www.gnu.org/licenses/>.

    Additional restriction:
    This software may not be used, in whole or in part, to train machine 
    learning models (including LLMs) without explicit prior written 
    permission.
    Violations will terminate your rights under this license.

    ======================================================================
*/

#include <kernel/kprintf.h>
#include <stdarg.h>
#include <stdint.h>


// buffer size for number conversion (should be enough for all possible datatypes)
#define PRINTF_BUFFER_SIZE 32

// Flag defintions
#define FLAG_ZERO_PAD        (1u << 0u)
#define FLAG_IS_NEGATIVE     (1u << 2u)


// Determine length of zero-terminated string
// \return length of string
static int _len(const char* str) 
{
    int len = 0;
    while (str[len] != '\0') len++;
    return len;
}

// Print a string with fixed length
static void _puts(const char* str, unsigned int len) 
{
    while(len > 0) {
        uart_putc(*str++);
        len--;
    }
}

// Print a null-terminated string
// \return length of string
static int _puts0(const char* str) {
    const unsigned int len = _len(str);
    _puts(str, len);

    return len;
}

// Reverse string of size len
static void _reverseStr(char* str, unsigned int len) 
{
    // Swap characters like so "XYZ..zyx"
    for (unsigned int i = 0; i < len / 2; i++) {
        char temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
    }
}

// Convert number to string
// Only 32-bit values are supported
static unsigned int _numToStr(uint32_t number, 
                              unsigned int width, 
                              unsigned int base, 
                              unsigned int flags, 
                              char* buffer) {
    
    unsigned int len = 0;

    // Convert number to string (reverse order)
    if (number == 0) {
        buffer[len++] = '0';
    } else {
        while (number > 0) {
            unsigned int digit = (unsigned int) (number % base);
            buffer[len++] = (digit < 10) ? (digit + '0') : (digit - 10 + 'a');
            number /= base;
        }
    }

    // Add padding if needed
    const unsigned int padStartIdx = len; 
    while (len < width) {
        buffer[len++] = (flags & FLAG_ZERO_PAD) ? '0' : ' ';
    }

    // Add sign if negative
    if (flags & FLAG_IS_NEGATIVE) {
        if (len > padStartIdx) {
            if (flags & FLAG_ZERO_PAD) {
                buffer[len - 1] = '-';
            } else {
                buffer[padStartIdx] = '-';
            }
        } else {
            buffer[len++] = '-';
        }
    }

    // Null-terminate and reverse
    buffer[len] = '\0';
    _reverseStr(buffer, len);

    return len;
}


// Print formatted string to UART.
// Implemented specifiers: %i, %u, %x, %p, %c, %s
// %p supports only 32-bit addresses.
// Returns length of printed string, or -1 on failure.
// Implementation note: minimal version, no floating-point or precision specifiers.
int kprintf(const char* str, ...)
{
    va_list args;
    va_start(args, str);

    unsigned int strLen, width, base, flags;

    char numBuffer[PRINTF_BUFFER_SIZE];

    strLen = 0;

    while(*str != '\0') {
        if (*str != '%') { // Just print char
            uart_putc(*str);
            strLen++;
            str++;
            continue;
        } else { // Evaluate format
            str++;
        }

        // Set flags
        flags = 0u;

        if (*str == '0') {
            flags |= FLAG_ZERO_PAD;
            str++;
        }

        // Set field width
        width = 0;
        while(*str >= '0' && *str <= '9') {
            width = (width * 10) + (*str - '0');
            str++;
        }

        // Evaluate specifier
        switch (*str) {
            case 'd':
            case 'i': {
                base = 10u;
                int number = va_arg(args, int);
                if (number < 0) {
                    flags |= FLAG_IS_NEGATIVE;
                    number = -number;
                }
                unsigned int numLen = _numToStr(number, width, base, flags, numBuffer);
                _puts(numBuffer, numLen);
                strLen += numLen;
                break;
            }
            case 'u': {
                base = 10u;
                unsigned int number = va_arg(args, unsigned int);
                unsigned int numLen = _numToStr(number, width, base, flags, numBuffer);
                _puts(numBuffer, numLen);
                strLen += numLen;
                break;
            }
            case 'x': {
                base = 16u;
                unsigned int number = va_arg(args, unsigned int);
                unsigned int numLen = _numToStr(number, width, base, flags, numBuffer);
                _puts(numBuffer, numLen);
                strLen += numLen;
                break;
            }
            case 'p': { // => 0x%08x
                base = 16u;
                void* pointer = va_arg(args, void*);
                flags |= FLAG_ZERO_PAD;
                width = 10u; // 2 prefix + 8 hex num (only for 32-bit)

                // Convert hex part of adress
                _numToStr((uint32_t) pointer, width, base, flags, numBuffer);
                
                // Add prefix
                numBuffer[0] = '0';
                numBuffer[1] = 'x';

                _puts(numBuffer, width);
                strLen += width;
                break;
            }
            case 'c': {
                char c = (char) va_arg(args, int);
                uart_putc(c);
                strLen++;
                break;
            }
            case 's': {
                char* s = va_arg(args, char*);
                strLen += _puts0(s);
                break;
            }
            case '%': {
                uart_putc('%');
                strLen++;
                break;
            }
            default: {
                strLen += _puts0("Unknown conversion specifier\n");
                goto END; 
            }
        }
        str++;
    }
    va_end(args);
    return strLen;

END:
    va_end(args);
    return -1; // error
}

