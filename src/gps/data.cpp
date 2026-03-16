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

#include "data.hpp"

#include <algorithm>

using namespace gpsdo::gps;

Position::Position() : latitude(0),
                       longitude(0),
                       altitude(0),
                       speed(0),
                       course(0) {
}

Position::Position(const float latitude,
                   const float longitude,
                   const float altitude,
                   const float speed,
                   const float course) : latitude(latitude),
                                         longitude(longitude),
                                         altitude(altitude),
                                         speed(speed),
                                         course(course) {
}

Position::Position(const Position &other) = default;

DateTime::DateTime() : year(2000),
                       month(1),
                       day(1),
                       hour(0),
                       minute(0),
                       second(0),
                       milliseconds(0) {
}

DateTime::DateTime(const int16_t year,
                   const int8_t month,
                   const int8_t day,
                   const int8_t hour,
                   const int8_t minute,
                   const int8_t second,
                   const int16_t milliseconds) : year(year),
                                                 month(month),
                                                 day(day),
                                                 hour(hour),
                                                 minute(minute),
                                                 second(second),
                                                 milliseconds(milliseconds) {
}

DateTime::DateTime(const DateTime &other) = default;
