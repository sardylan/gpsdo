#include <math.h>
#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"
#include "hardware/uart.h"

#include "si5351.h"

#define LOG_UART                                    uart1
#define LOG_UART_SPEED                              115200
#define LOG_UART_PIN_TX                             4
#define LOG_UART_PIN_RX                             5
#define LOG_UART_DATA_BITS                          8
#define LOG_UART_STOP_BITS                          1
#define LOG_UART_PARITY                             UART_PARITY_NONE

int main(void) {
    uart_init(LOG_UART, LOG_UART_SPEED);
    gpio_set_function(LOG_UART_PIN_TX, UART_FUNCSEL_NUM(LOG_UART, LOG_UART_PIN_TX));
    gpio_set_function(LOG_UART_PIN_RX, UART_FUNCSEL_NUM(LOG_UART, LOG_UART_PIN_RX));
    uart_set_hw_flow(LOG_UART, false, false);
    uart_set_format(LOG_UART, LOG_UART_DATA_BITS, LOG_UART_STOP_BITS, LOG_UART_PARITY);
    uart_set_fifo_enabled(LOG_UART, true);

    uart_puts(LOG_UART, "Initializing SI5351\n\r");
    si5351_init(0x60, SI5351_CRYSTAL_LOAD_8PF, 25000000, 0); // I am using a 26 MHz TCXO

    uart_puts(LOG_UART, "Setting clock power\n\r");
    si5351_set_clock_pwr(SI5351_CLK0, 0);
    si5351_set_clock_pwr(SI5351_CLK1, 0);
    si5351_set_clock_pwr(SI5351_CLK2, 0);

    uart_puts(LOG_UART, "Setting drive strength\n\r");
    si5351_drive_strength(SI5351_CLK0, SI5351_DRIVE_8MA);
    si5351_drive_strength(SI5351_CLK1, SI5351_DRIVE_8MA);
    si5351_drive_strength(SI5351_CLK2, SI5351_DRIVE_8MA);

    uart_puts(LOG_UART, "Setting frequencies\n\r");
    si5351_set_freq(10000000ULL * SI5351_FREQ_MULT, SI5351_CLK0);
    si5351_set_freq(10000000ULL * SI5351_FREQ_MULT, SI5351_CLK1);
    si5351_set_freq(10000000ULL * SI5351_FREQ_MULT, SI5351_CLK2);

    uart_puts(LOG_UART, "Setting clock power\n\r");
    si5351_set_clock_pwr(SI5351_CLK0, 1);
    si5351_set_clock_pwr(SI5351_CLK1, 1);
    si5351_set_clock_pwr(SI5351_CLK2, 1);

    uart_puts(LOG_UART, "Enabling outputs\n\r");
    si5351_output_enable(SI5351_CLK0, 1);
    si5351_output_enable(SI5351_CLK1, 0);
    si5351_output_enable(SI5351_CLK2, 0);

    return 0;
}
