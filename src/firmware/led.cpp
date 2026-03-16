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

#include "led.hpp"

#include <hardware/timer.h>

#include "hardware/gpio.h"

using namespace gpsdo::firmware;

Led::Led(const uint pin, const uint64_t delay) : pin(pin), delay(delay), lastTrigger(0) {
}

Led::~Led() = default;

void Led::init() const {
    gpio_put(pin, false);
}

void Led::set(const bool on) const {
    gpio_put(pin, on);
}

void Led::check() const {
    if (time_us_64() - lastTrigger >= delay) {
        gpio_put(pin, false);
    }
}

void Led::trigger() {
    gpio_put(pin, true);
    lastTrigger = time_us_64();
}
