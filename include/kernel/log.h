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

