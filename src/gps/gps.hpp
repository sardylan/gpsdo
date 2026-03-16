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

#ifndef __GPS__GPS__H
#define __GPS__GPS__H

#include <cstdint>
#include <vector>
#include <string>

#include "data.hpp"
#include "enums.hpp"

namespace gpsdo::gps {
    class GPS {
    public:
        explicit GPS();

        ~GPS();

        void putChar(char c);

        [[nodiscard]] bool getStatus() const;

        [[nodiscard]] FixQuality getFixQuality() const;

        [[nodiscard]] uint8_t getSatellites() const;

        [[nodiscard]] DateTime getDateTime() const;

        [[nodiscard]] Position getPosition() const;

        [[nodiscard]] bool takeFlagStatus();

        [[nodiscard]] bool takeFlagDateTime();

        void parseBuffer();

    private:
        char buffer[256];
        size_t bufferPos;
        char nmea[256];

        bool status;
        FixQuality fixQuality;
        uint8_t satellites;
        DateTime dateTime;
        Position position;

        volatile bool flagStatus;
        volatile bool flagDatetime;

        volatile bool parseSentence;

        void parseGGA(const std::string &data);

        void parseRMC(const std::string &data);
    };
}

#endif
