#ifndef IO_H
#define IO_H

#include <stdint.h>

static inline void write32(uint32_t reg, uint32_t data)
{
    *(volatile uint32_t*) reg = data;
}

static inline uint32_t read32(uint32_t reg)
{
    return *(volatile uint32_t*) reg;
}

#endif // IO_H
