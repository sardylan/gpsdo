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

#include "interrupts.hpp"

#include <hardware/uart.h>
#include <hardware/irq.h>
#include <hardware/pwm.h>

#include "firmware.hpp"

namespace gpsdo::firmware {
    extern Firmware firmware;

    extern uint counterPwmSlice;
    extern volatile uint16_t counterPwmValue;
    extern volatile uint32_t counterOverflow;
    extern volatile uint32_t counterValue;
    extern volatile bool counterEventPps;
}

void isrGpsRx() {
    while (uart_is_readable(GPS_UART_PORT)) {
        const char ch = uart_getc(GPS_UART_PORT);
        gpsdo::firmware::firmware.gpsPutChar(ch);
    }
}

void isrCounterOverflow() {
    pwm_clear_irq(gpsdo::firmware::counterPwmSlice);
    gpsdo::firmware::counterOverflow += 1;
}

void isrCounterPps(uint gpio, uint32_t eventMask) {
    const uint16_t old_counter = gpsdo::firmware::counterPwmValue;
    gpsdo::firmware::counterPwmValue = pwm_get_counter(gpsdo::firmware::counterPwmSlice);
    gpsdo::firmware::counterValue = (gpsdo::firmware::counterOverflow << 16) + gpsdo::firmware::counterPwmValue -
                                    old_counter;

    gpsdo::firmware::counterOverflow = 0;

    gpsdo::firmware::counterEventPps = true;
}
