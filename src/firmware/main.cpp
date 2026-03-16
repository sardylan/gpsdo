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

#include "main.hpp"

#include <pico/multicore.h>

#include <logger/logger.hpp>

#include "firmware.hpp"
#include "init.hpp"

using namespace gpsdo::firmware;

namespace gpsdo::firmware {
    Firmware firmware;
}

int main() {
    initShared();

    multicore_launch_core1(programCore1);
    programCore0();
}

void programCore0() {
    initCore0();

    logInfo("Starting Core 0");
    while (true) {
        firmware.job0();
        tight_loop_contents();
    }
}

void programCore1() {
    initCore1();

    logInfo("Starting Core 1");
    while (true) {
        firmware.job1();
        tight_loop_contents();
    }
}
