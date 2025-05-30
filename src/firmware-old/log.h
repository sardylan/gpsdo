#ifndef LOG_H
#define LOG_H

#include <stdarg.h>
#include <stdio.h>
#include <hardware/uart.h>

#include "config.h"

#define log_string(string) uart_puts(LOG_UART, string)

#define log_message(level, format, ...) log_message_impl(__FILE_NAME__, __LINE__, level, format, ##__VA_ARGS__)

#define log_text(format, ...) log_message("     ", format, ##__VA_ARGS__)

#ifdef LOG_ERROR_ENABLED
#define log_error(format, ...) log_message("ERROR", format, ##__VA_ARGS__)
#else
#define log_error(format, ...)
#endif

#ifdef LOG_WARNING_ENABLED
#define log_warn(format, ...) log_message("WARN ", format, ##__VA_ARGS__)
#else
#define log_warn(format, ...)
#endif

#ifdef LOG_INFO_ENABLED
#define log_info(format, ...) log_message("INFO ", format, ##__VA_ARGS__)
#else
#define log_info(format, ...)
#endif

#ifdef LOG_DEBUG_ENABLED
#define log_debug(format, ...) log_message("DEBUG", format, ##__VA_ARGS__)
#else
#define log_debug(format, ...)
#endif

#ifdef LOG_TRACE_ENABLED
#define log_trace(format, ...) log_message("TRACE", format, ##__VA_ARGS__)
#else
#define log_trace(format, ...)
#endif

void log_init();

void log_message_impl(const char *file, size_t line, const char *level, const char *format, ...);

#endif
