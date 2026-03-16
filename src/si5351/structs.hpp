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

#ifndef __SI5351__STRUCTS__H
#define __SI5351__STRUCTS__H

#include <cstdint>

namespace gpsdo::si5351 {
    struct RegSet
    {
        uint32_t p1;
        uint32_t p2;
        uint32_t p3;
    };

    struct Status
    {
        uint8_t SYS_INIT;
        uint8_t LOL_B;
        uint8_t LOL_A;
        uint8_t LOS;
        uint8_t REVID;
    };

    struct IntStatus
    {
        uint8_t SYS_INIT_STKY;
        uint8_t LOL_B_STKY;
        uint8_t LOL_A_STKY;
        uint8_t LOS_STKY;
    };
}

#endif
