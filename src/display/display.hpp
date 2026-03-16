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

#ifndef __DISPLAY__DISPLAY__H
#define __DISPLAY__DISPLAY__H

#include <pico/types.h>

namespace gpsdo::display {
    enum class Line {
        Upper,
        Lower
    };

    class Display {
    public:
        explicit Display(uint rs, uint enable, uint d4, uint d5, uint d6, uint d7);

        ~Display();

        void init() const;

        void clear() const;

        void printf(Line line, const char *format, ...) const;

        void text(Line line, const char *str) const;

    private:
        const uint rs;
        const uint enable;
        const uint d4;
        const uint d5;
        const uint d6;
        const uint d7;

        void send_command(uint8_t cmd) const;

        void send_data(uint8_t cmd) const;

        void send_byte(uint8_t byte) const;

        void pulseEnable() const;
    };
}

#endif
