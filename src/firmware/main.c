#include "main.h"

#include <string.h>

#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <hardware/uart.h>
#include <hardware/irq.h>
#include <hardware/gpio.h>
#include <hardware/pwm.h>
#include <hardware/clocks.h>
#include <hardware/rtc.h>

#include "config.h"
#include "led.h"
#include "log.h"
#include "gps.h"
#include "oscillator.h"
#include "si5351.h"

unsigned int counter_slice_num;
uint64_t overflow_counter;
uint64_t value;

volatile bool pps_event;
volatile bool gps_event;

int main() {
    rtc_init();

    log_init();
    gps_init();
    led_init();
    oscillator_init();

    irq_set_exclusive_handler(GPS_UART_IRQ, gps_rx);
    irq_set_enabled(GPS_UART_IRQ, true);
    uart_init(GPS_UART, GPS_UART_SPEED);
    gpio_set_function(GPS_UART_PIN_TX, UART_FUNCSEL_NUM(GPS_UART, GPS_UART_PIN_TX));
    gpio_set_function(GPS_UART_PIN_RX, UART_FUNCSEL_NUM(GPS_UART, GPS_UART_PIN_RX));
    uart_set_hw_flow(GPS_UART, false, false);
    uart_set_format(GPS_UART, GPS_UART_DATA_BITS, GPS_UART_STOP_BITS, GPS_UART_PARITY);
    uart_set_fifo_enabled(GPS_UART, false);
    uart_set_irq_enables(GPS_UART, true, false);

    gpio_init(GPS_PIN_PPS);
    gpio_set_dir(GPS_PIN_PPS, GPIO_IN);
    gpio_set_irq_enabled_with_callback(GPS_PIN_PPS, GPIO_IRQ_EDGE_RISE, true, gps_pps_callback);

    counter_slice_num = pwm_gpio_to_slice_num(3);
    pwm_config cfg = pwm_get_default_config();
    pwm_config_set_clkdiv_mode(&cfg, PWM_DIV_B_RISING);
    pwm_init(counter_slice_num, &cfg, false);
    gpio_set_function(3, GPIO_FUNC_PWM);

    overflow_counter = 0;
    pwm_clear_irq(counter_slice_num);
    pwm_set_irq_enabled(counter_slice_num, true);
    irq_set_exclusive_handler(PWM_IRQ_WRAP, counter_overflow);
    irq_set_enabled(PWM_IRQ_WRAP, true);

    pwm_set_counter(counter_slice_num, 0);
    pwm_set_enabled(counter_slice_num, true);

    while (true) {
        if (pps_event) {
            pps_event = false;
            log_debug("CLOCK: current %llu - delta %llu", value, REFERENCE_CLOCK_FREQUENCY - value);
            led_set_state(LED_GPS_PPS, false);
        }

        if (gps_event) {
            gps_event = false;

            const char *sentence = gps_tail_get();
            log_trace("GPS: %s", sentence);

            gps_tail_forward();
            led_set_state(LED_GPS_DATA, false);
        }

        tight_loop_contents();
    }
}

void counter_overflow() {
    pwm_clear_irq(counter_slice_num);
    overflow_counter++;
}

void gps_rx() {
    while (uart_is_readable(GPS_UART)) {
        const char ch = uart_getc(GPS_UART);

        if (ch == '\n' || ch == '\r') {
            if (strlen(gps_head_get()) == 0)
                continue;

            led_set_state(LED_GPS_DATA, true);
            gps_head_forward();

            gps_event = true;
        } else {
            gps_head_put(ch);
        }
    }
}

void gps_pps_callback(uint gpio, uint32_t event_mask) {
    if (gpio != GPS_PIN_PPS)
        return;
    if (!(event_mask & GPIO_IRQ_EDGE_RISE))
        return;

    value = (overflow_counter << 16) + pwm_get_counter(counter_slice_num);
    overflow_counter = 0;

    led_set_state(LED_GPS_PPS, true);
    pps_event = true;
}
