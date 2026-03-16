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

#ifndef __FIRMWARE__FIRMWARE__H
#define __FIRMWARE__FIRMWARE__H

#include <scheduler/scheduler.hpp>
#include <gps/gps.hpp>
#include <display/display.hpp>
#include <si5351/si5351.hpp>
#include <pid/pid.hpp>

#include "led.hpp"
#include "timertc.hpp"
#include "counter.hpp"

namespace gpsdo::firmware {
    class Firmware {
    public:
        Firmware();

        ~Firmware();

        void init();

        void job0();

        void job1();

        void gpsPutChar(char c);

    private:
        scheduler::Scheduler scheduler;

        display::Display display;
        gps::GPS gps;
        PID<uint64_t> pid;
        TimeRTC timeRTC;

        Led ledGpsUart;
        Led ledGpsValid;
        Led ledGpsPps;
        Led ledRtcSync;

        si5351::Si5351 si5351;

        uint8_t displayMessage;
        uint32_t currentFrequency;

        void rotateDisplayMessage();

        void updateSi5351Frequencies();

        static constexpr double frequencyFactor0 = static_cast<double>(SI5351_FREQ_CLK0) / SI5351_FREQ_CLK2;
        static constexpr double frequencyFactor1 = static_cast<double>(SI5351_FREQ_CLK1) / SI5351_FREQ_CLK2;
    };
}

#endif
