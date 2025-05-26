#ifndef LOG_H
#define LOG_H

#include <stdarg.h>
#include <stdio.h>
#include <hardware/uart.h>

#include "config.h"

#define log_string(string) uart_puts(LOG_UART, string)

#define log_message(level, format, ...) log_message_impl(__FILE_NAME__, __LINE__, level, format, ##__VA_ARGS__)

#define log_error(format, ...) log_message("ERROR", format, ##__VA_ARGS__)
#define log_warn(format, ...) log_message("WARN ", format, ##__VA_ARGS__)
#define log_info(format, ...) log_message("INFO ", format, ##__VA_ARGS__)
#define log_debug(format, ...) log_message("DEBUG", format, ##__VA_ARGS__)
#define log_trace(format, ...) log_message("TRACE", format, ##__VA_ARGS__)

void log_init();

void log_message_impl(const char *file, size_t line, const char *level, const char *format, ...);

#endif
