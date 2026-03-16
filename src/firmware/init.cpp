/*
 * GPS Dominated Oscillator for RaspberryPi Pico (RP2040)
 * Copyright (C) 2026  Luca Cireddu <sardylan@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "init.hpp"

#include <pico/multicore.h>
#include <pico/stdio_uart.h>
#include <hardware/gpio.h>
#include <hardware/uart.h>
#include <hardware/irq.h>
#include <hardware/pwm.h>

#include <logger/logger.hpp>

#include "firmware.hpp"
#include "interrupts.hpp"

namespace gpsdo::firmware {
    extern Firmware firmware;
    extern uint counterPwmSlice;

    void initShared() {
        stdio_uart_init_full(DEBUG_UART_PORT, DEBUG_UART_SPEED, DEBUG_UART_PIN_TX, DEBUG_UART_PIN_RX);
        logInfo("%s %s", APPLICATION_NAME, APPLICATION_VERSION);

        initDisplay();
        initGPS();
        initLEDs();
        initSi5351();
    }

    void initCore0() {
        initCounter();
    }

    void initCore1() {
        initGPSInterrupt();

        firmware.init();
    }

    void initDisplay() {
        logInfo("Initializing display");

        logDebug("Initializing GPIO pins");
        gpio_init(DISPLAY_PIN_RS);
        gpio_init(DISPLAY_PIN_ENABLE);
        gpio_init(DISPLAY_PIN_D4);
        gpio_init(DISPLAY_PIN_D5);
        gpio_init(DISPLAY_PIN_D6);
        gpio_init(DISPLAY_PIN_D7);

        logDebug("Setting GPIO direction");
        gpio_set_dir(DISPLAY_PIN_RS, GPIO_OUT);
        gpio_set_dir(DISPLAY_PIN_ENABLE, GPIO_OUT);
        gpio_set_dir(DISPLAY_PIN_D4, GPIO_OUT);
        gpio_set_dir(DISPLAY_PIN_D5, GPIO_OUT);
        gpio_set_dir(DISPLAY_PIN_D6, GPIO_OUT);
        gpio_set_dir(DISPLAY_PIN_D7, GPIO_OUT);
    }

    void initGPS() {
        logInfo("Initializing GPS");

        logDebug("Initializing UART");
        uart_init(GPS_UART_PORT, GPS_UART_SPEED);
        gpio_set_function(GPS_UART_PIN_TX, UART_FUNCSEL_NUM(GPS_UART, GPS_UART_PIN_TX));
        gpio_set_function(GPS_UART_PIN_RX, UART_FUNCSEL_NUM(GPS_UART, GPS_UART_PIN_RX));
        uart_set_hw_flow(GPS_UART_PORT, false, false);
        uart_set_format(GPS_UART_PORT, GPS_UART_DATA_BITS, GPS_UART_STOP_BITS, GPS_UART_PARITY);
        uart_set_fifo_enabled(GPS_UART_PORT, true);
        uart_set_irq_enables(GPS_UART_PORT, true, false);
    }

    void initGPSInterrupt() {
        logInfo("Initializing GPS interrupt");

        logDebug("Configuring GPS UART interrupt");
        irq_set_exclusive_handler(GPS_UART_IRQ, isrGpsRx);
        irq_set_enabled(GPS_UART_IRQ, true);
        irq_set_priority(GPS_UART_IRQ, 2);
    }

    void initLEDs() {
        logInfo("Initializing LEDs");

        logDebug("Initializing GPIO pins");
        gpio_init(LED_PIN_GPS_UART);
        gpio_init(LED_PIN_GPS_VALID);
        gpio_init(LED_PIN_GPS_PPS);
        gpio_init(LED_PIN_RTC_SYNC);

        logDebug("Setting GPIO direction");
        gpio_set_dir(LED_PIN_GPS_UART, GPIO_OUT);
        gpio_set_dir(LED_PIN_GPS_VALID, GPIO_OUT);
        gpio_set_dir(LED_PIN_GPS_PPS, GPIO_OUT);
        gpio_set_dir(LED_PIN_RTC_SYNC, GPIO_OUT);
    }

    void initCounter() {
        logInfo("Initializing Counter");

        logDebug("Initializing GPIO pin");
        counterPwmSlice = pwm_gpio_to_slice_num(COUNTER_PIN_CLOCK);
        pwm_config cfg = pwm_get_default_config();
        pwm_config_set_clkdiv_mode(&cfg, PWM_DIV_B_RISING);
        pwm_init(counterPwmSlice, &cfg, false);
        gpio_set_function(3, GPIO_FUNC_PWM);

        logDebug("Initializing overflow interrupt");
        pwm_clear_irq(counterPwmSlice);
        pwm_set_irq_enabled(counterPwmSlice, true);
        irq_set_exclusive_handler(PWM_IRQ_WRAP, isrCounterOverflow);
        irq_set_enabled(PWM_IRQ_WRAP, true);
        irq_set_priority(PWM_IRQ_WRAP, 0);

        logDebug("Resetting counter");
        pwm_set_counter(counterPwmSlice, 0);

        logDebug("Enabling counter");
        pwm_set_enabled(counterPwmSlice, true);

        logDebug("Initializing PPS GPIO");
        gpio_init(COUNTER_PIN_PPS);
        gpio_set_dir(COUNTER_PIN_PPS, GPIO_IN);
        gpio_set_irq_enabled_with_callback(COUNTER_PIN_PPS, GPIO_IRQ_EDGE_RISE, true, &isrCounterPps);
        irq_set_priority(GPIO_IRQ_EDGE_RISE, 1);
    }

    void initSi5351() {
        i2c_init(SI5351_BUS, SI5351_BAUDRATE);
        gpio_set_function(SI5351_BUS_PIN_SDA, GPIO_FUNC_I2C);
        gpio_set_function(SI5351_BUS_PIN_SCL, GPIO_FUNC_I2C);
        gpio_pull_up(SI5351_BUS_PIN_SDA);
        gpio_pull_up(SI5351_BUS_PIN_SCL);
    }
}
