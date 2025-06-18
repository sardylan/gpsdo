#include "log.h"

#include <stdio.h>
#include <string.h>

#include <pico/mutex.h>

#include <hardware/rtc.h>
#include <hardware/gpio.h>
#include <hardware/uart.h>

#include "timertc.h"

mutex_t log_cs;

void log_init() {
    mutex_init(&log_cs);

    uart_init(LOG_UART, LOG_UART_SPEED);
    gpio_set_function(LOG_UART_PIN_TX, UART_FUNCSEL_NUM(LOG_UART, LOG_UART_PIN_TX));
    gpio_set_function(LOG_UART_PIN_RX, UART_FUNCSEL_NUM(LOG_UART, LOG_UART_PIN_RX));
    uart_set_hw_flow(LOG_UART, false, false);
    uart_set_format(LOG_UART, LOG_UART_DATA_BITS, LOG_UART_STOP_BITS, LOG_UART_PARITY);
    uart_set_fifo_enabled(LOG_UART, true);
}

void log_message_impl(const char *file, const size_t line, const char *level, const char *format, ...) {
    char datetime[20];
    char mcu_us[22];
    char message[512];
    char buffer[1024];
    va_list args;
    datetime_t now;

    timertc_get_time(&now);
    snprintf(datetime, sizeof(datetime), "%04d-%02d-%02d %02d:%02d:%02d",
             now.year, now.month, now.day, now.hour, now.min, now.sec);

    snprintf(mcu_us, sizeof(mcu_us), "%07llu", time_us_64());
    const size_t mcu_us_len = strlen(mcu_us);
    mcu_us[mcu_us_len + 1] = '\0';
    mcu_us[mcu_us_len] = mcu_us[mcu_us_len - 1];
    mcu_us[mcu_us_len - 1] = mcu_us[mcu_us_len - 2];
    mcu_us[mcu_us_len - 2] = mcu_us[mcu_us_len - 3];
    mcu_us[mcu_us_len - 3] = mcu_us[mcu_us_len - 4];
    mcu_us[mcu_us_len - 4] = mcu_us[mcu_us_len - 5];
    mcu_us[mcu_us_len - 5] = mcu_us[mcu_us_len - 6];
    mcu_us[mcu_us_len - 6] = '.';

    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    snprintf(buffer, sizeof(buffer), "%s (%s) [%s] {%s:%u}: %s\n\r",
             datetime, mcu_us, level, file, line, message);

    mutex_enter_blocking(&log_cs);
    log_string(buffer);
    mutex_exit(&log_cs);
}
