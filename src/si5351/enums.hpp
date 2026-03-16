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

#ifndef __SI5351__ENUMS__H
#define __SI5351__ENUMS__H

#include <cstddef>
#include <cstdint>

namespace gpsdo::si5351 {

    template<typename E>
    constexpr std::size_t to_index(E e) noexcept {
        return static_cast<std::size_t>(e);
    }

    enum class Clock : uint8_t {
        SI5351_CLK0,
        SI5351_CLK1,
        SI5351_CLK2,
        SI5351_CLK3,
        SI5351_CLK4,
        SI5351_CLK5,
        SI5351_CLK6,
        SI5351_CLK7
    };

    enum class PLL : uint8_t {
        SI5351_PLLA,
        SI5351_PLLB
    };

    enum class Drive : uint8_t {
        SI5351_DRIVE_2MA,
        SI5351_DRIVE_4MA,
        SI5351_DRIVE_6MA,
        SI5351_DRIVE_8MA
    };

    enum class ClockSource : uint8_t {
        SI5351_CLK_SRC_XTAL,
        SI5351_CLK_SRC_CLKIN,
        SI5351_CLK_SRC_MS0,
        SI5351_CLK_SRC_MS
    };

    enum class ClockDisable : uint8_t {
        SI5351_CLK_DISABLE_LOW,
        SI5351_CLK_DISABLE_HIGH,
        SI5351_CLK_DISABLE_HI_Z,
        SI5351_CLK_DISABLE_NEVER
    };

    enum class ClockFanout : uint8_t {
        SI5351_FANOUT_CLKIN,
        SI5351_FANOUT_XO,
        SI5351_FANOUT_MS
    };

    enum class PLLInput : uint8_t {
        SI5351_PLL_INPUT_XO,
        SI5351_PLL_INPUT_CLKIN
    };
}

#endif
