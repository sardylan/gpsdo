#include "log.h"

#include <stdio.h>
#include <string.h>

#include <hardware/rtc.h>

#include "version.h"

void log_init() {
    char buffer[1024];

    uart_init(LOG_UART, LOG_UART_SPEED);
    gpio_set_function(LOG_UART_PIN_TX, UART_FUNCSEL_NUM(LOG_UART, LOG_UART_PIN_TX));
    gpio_set_function(LOG_UART_PIN_RX, UART_FUNCSEL_NUM(LOG_UART, LOG_UART_PIN_RX));
    uart_set_hw_flow(LOG_UART, false, false);
    uart_set_format(LOG_UART, LOG_UART_DATA_BITS, LOG_UART_STOP_BITS, LOG_UART_PARITY);
    uart_set_fifo_enabled(LOG_UART, false);

    log_string("\n\r");
    log_string("\n\r");

    snprintf(buffer, sizeof(buffer), "%s %s\n\r", APPLICATION_NAME, APPLICATION_VERSION);
    log_string(buffer);
    snprintf(buffer, sizeof(buffer), "Build at %s\n\r", APPLICATION_BUILD);
    log_string(buffer);
#ifdef APPLICATION_GIT_SHA1
    snprintf(buffer, sizeof(buffer), "Git: %s\n\r", APPLICATION_GIT_SHA1);
    log_string(buffer);
#endif

    log_string("\n\r");
    log_string("\n\r");
}

void log_message_impl(const char *file, const size_t line, const char *level, const char *format, ...) {
    char datetime[20];
    char message[512];
    char buffer[1024];
    va_list args;
    datetime_t now;

    rtc_get_datetime(&now);
    snprintf(datetime, sizeof(datetime), "%04d-%02d-%02d %02d:%02d:%02d",
             now.year, now.month, now.day, now.hour, now.min, now.sec);

    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    snprintf(buffer, sizeof(buffer), "%s (%llu) [%s] {%s:%u}: %s\n\r",
             datetime, time_us_64(), level, file, line, message);
    log_string(buffer);
}
