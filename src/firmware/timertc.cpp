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

#include "timertc.hpp"

#include <pico/mutex.h>
#include <hardware/rtc.h>
#include <hardware/timer.h>

using namespace gpsdo::firmware;

TimeRTC::TimeRTC() : mutex(), firstCall(false), lastSet(0) {
}

TimeRTC::~TimeRTC() = default;

void TimeRTC::init() {
    mutex_init(&mutex);
    rtc_init();
}

datetime_t TimeRTC::getDateTime() {
    datetime_t dateTime;

    mutex_enter_blocking(&mutex);
    rtc_get_datetime(&dateTime);
    mutex_exit(&mutex);

    return dateTime;
}

void TimeRTC::setDateTime(const datetime_t dateTime) {
    mutex_enter_blocking(&mutex);

    rtc_set_datetime(&dateTime);
    sleep_ms(100);

    mutex_exit(&mutex);

    lastSet = time_us_64();
    firstCall = true;
}

bool TimeRTC::isValid() const {
    return firstCall && time_us_64() - lastSet < 15000000;
}
