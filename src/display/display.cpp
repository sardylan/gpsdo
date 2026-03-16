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

#include "display.hpp"

#include <cstdarg>
#include <cstdio>
#include <hardware/gpio.h>
#include <pico/time.h>

using namespace gpsdo::display;

Display::Display(const uint rs,
                 const uint enable,
                 const uint d4,
                 const uint d5,
                 const uint d6,
                 const uint d7) : rs(rs),
                                  enable(enable),
                                  d4(d4),
                                  d5(d5),
                                  d6(d6),
                                  d7(d7) {
}

Display::~Display() = default;

void Display::init() const {
    gpio_put(rs, false);
    gpio_put(enable, false);
    gpio_put(d4, false);
    gpio_put(d5, false);
    gpio_put(d6, false);
    gpio_put(d7, false);

    sleep_ms(100);

    send_command(0x03);
    sleep_ms(20);
    send_command(0x03);
    sleep_ms(20);
    send_command(0x03);
    sleep_ms(20);

    send_command(0x02);
    sleep_ms(5);
    send_command(0x28);
    sleep_ms(5);
    send_command(0x0c);
    sleep_ms(5);

    send_command(0x01);
    sleep_ms(50);
}

void Display::clear() const {
    send_command(0x01);
    sleep_ms(2);
    send_command(0x02);
    sleep_ms(2);
}

void Display::printf(const Line line, const char *format, ...) const {
    std::va_list args;
    va_start(args, format);

    char buffer[17];
    std::vsnprintf(buffer, sizeof(buffer), format, args);
    text(line, buffer);
}

void Display::text(const Line line, const char *str) const {
    switch (line) {
        case Line::Upper:
            send_command(0x80);
            break;
        case Line::Lower:
            send_command(0xc0);
            break;
    }

    size_t pos = 0;
    while (str[pos] != '\0' && pos < 16) {
        send_data(str[pos]);
        pos++;
    }
}

void Display::send_command(const uint8_t cmd) const {
    gpio_put(rs, false);
    send_byte(cmd);
}

void Display::send_data(const uint8_t cmd) const {
    gpio_put(rs, true);
    send_byte(cmd);
}

void Display::send_byte(const uint8_t byte) const {
    gpio_put(d4, (byte & 0b00010000) != 0);
    gpio_put(d5, (byte & 0b00100000) != 0);
    gpio_put(d6, (byte & 0b01000000) != 0);
    gpio_put(d7, (byte & 0b10000000) != 0);

    pulseEnable();

    gpio_put(d4, (byte & 0b00000001) != 0);
    gpio_put(d5, (byte & 0b00000010) != 0);
    gpio_put(d6, (byte & 0b00000100) != 0);
    gpio_put(d7, (byte & 0b00001000) != 0);

    pulseEnable();
}

void Display::pulseEnable() const {
    gpio_put(enable, false);
    sleep_us(1);
    gpio_put(enable, true);
    sleep_us(1);
    gpio_put(enable, false);
    sleep_us(100);
}
