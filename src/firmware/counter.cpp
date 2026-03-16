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

#include "counter.hpp"

#include <cstdint>
#include <pico/types.h>

using namespace gpsdo::firmware;

namespace gpsdo::firmware {
    uint counterPwmSlice;
    volatile uint16_t counterPwmValue;
    volatile uint32_t counterOverflow;
    volatile uint32_t counterValue;
    volatile bool counterEventPps;

    void counterInit() {
        counterPwmValue = 0;
        counterOverflow = 0;
        counterValue = 0;
        counterEventPps = false;
    }

    bool takePpsFlag() {
        if (counterEventPps) {
            counterEventPps = false;
            return true;
        }
        return false;
    }

    uint32_t counterGetValue() {
        return counterValue;
    }
}
