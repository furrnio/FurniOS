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

#ifndef KERNEL_LOG_H
#define KERNEL_LOG_H

//#include <kernel/kprintf.h>

int kprintf(const char* fmt, ...);

#define LOG_NONE   0
#define LOG_ERR    1
#define LOG_WARN   2
#define LOG_INFO   3
#define LOG_DEBUG  4

// Default log level
#ifndef KERNEL_LOG_LEVEL
#define KERNEL_LOG_LEVEL LOG_NONE
#endif

// Internal helper
#define KLOG_PRINT(tag, fmt, ...)          \
    do {                                   \
        kprintf("[%s] ", tag);             \
        kprintf(fmt, ##__VA_ARGS__);       \
    } while (0)

#if KERNEL_LOG_LEVEL >= LOG_DEBUG
#define klog_debug(tag, fmt, ...) KLOG_PRINT(tag, fmt, ##__VA_ARGS__)
#else
#define klog_debug(tag, fmt, ...)
#endif

#if KERNEL_LOG_LEVEL >= LOG_INFO
#define klog_info(tag, fmt, ...)  KLOG_PRINT(tag, fmt, ##__VA_ARGS__)
#else
#define klog_info(tag, fmt, ...)
#endif

#if KERNEL_LOG_LEVEL >= LOG_WARN
#define klog_warn(tag, fmt, ...)  KLOG_PRINT(tag, fmt, ##__VA_ARGS__)
#else
#define klog_warn(tag, fmt, ...)
#endif

#if KERNEL_LOG_LEVEL >= LOG_ERR
#define klog_err(tag, fmt, ...)   KLOG_PRINT(tag, fmt, ##__VA_ARGS__)
#else
#define klog_err(tag, fmt, ...)
#endif

#endif // KERNEL_LOG_H

