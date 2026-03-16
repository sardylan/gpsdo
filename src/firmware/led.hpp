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

#ifndef __FIRMWARE__LED__H
#define __FIRMWARE__LED__H

#include <cstdint>

#include <pico/types.h>

namespace gpsdo::firmware {
    class Led {
    public:
        explicit Led(uint pin, uint64_t delay = 150000);

        ~Led();

        void init() const;

        void set(bool on) const;

        void check() const;

        void trigger();

    private:
        const uint pin;
        const uint64_t delay;

        uint64_t lastTrigger;
    };
}

#endif
