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

#include "gps.hpp"

#include <array>
#include <cstdint>
#include <string>
#include <charconv>
#include <cstring>

#include "enums.hpp"

namespace {
    template<typename T>
    T parse(const std::string_view sv, const T defaultValue = T{}) {
        T value = defaultValue;
        std::from_chars(sv.data(), sv.data() + sv.size(), value);
        return value;
    }

    template<size_t N>
    std::array<std::string_view, N> tokenize(const std::string_view sv) {
        std::array<std::string_view, N> result{};
        size_t idx = 0;
        size_t start = 0;
        for (size_t i = 0; i <= sv.size() && idx < result.size(); ++i) {
            if (i == sv.size() || sv[i] == ',') {
                result[idx++] = sv.substr(start, i - start);
                start = i + 1;
            }
        }
        return result;
    }
}

using namespace gpsdo::gps;

GPS::GPS() : buffer{},
             bufferPos(0),
             nmea{},
             status(false),
             fixQuality(FixQuality::Invalid),
             satellites(0),
             flagStatus(false),
             flagDatetime(false),
             parseSentence(false) {
}

GPS::~GPS() = default;

void GPS::putChar(const char c) {
    if (c == '\n' || c == '\r') {
        if (bufferPos == 0)
            return;

        std::memcpy(nmea, buffer, bufferPos);
        nmea[bufferPos] = '\0';
        parseSentence = true;
        bufferPos = 0;
        return;
    }

    buffer[bufferPos++] = c;
    if (bufferPos >= sizeof(buffer)) {
        bufferPos = 0;
    }
}

bool GPS::getStatus() const {
    return status;
}

FixQuality GPS::getFixQuality() const {
    return fixQuality;
}

uint8_t GPS::getSatellites() const {
    return satellites;
}

DateTime GPS::getDateTime() const {
    return {dateTime};
}

Position GPS::getPosition() const {
    return {position};
}

bool GPS::takeFlagStatus() {
    if (flagStatus) {
        flagStatus = false;
        return true;
    }
    return false;
}

bool GPS::takeFlagDateTime() {
    if (flagDatetime) {
        flagDatetime = false;
        return true;
    }
    return false;
}

void GPS::parseBuffer() {
    if (parseSentence == false) {
        return;
    }

    parseSentence = false;

    const std::string sentence(this->nmea);
    if (!sentence.starts_with("$GP")) {
        return;
    }

    const size_t asteriskPos = sentence.find('*');
    if (asteriskPos == std::string::npos || asteriskPos + 2 >= sentence.size()) {
        return;
    }

    const std::string_view checksumStr = std::string_view(sentence).substr(asteriskPos + 1, 2);
    uint8_t expected = 0;
    std::from_chars(checksumStr.data(), checksumStr.data() + checksumStr.size(), expected, 16);
    uint8_t computed = 0;
    for (size_t i = 1; i < asteriskPos; ++i) {
        computed ^= static_cast<uint8_t>(sentence[i]);
    }
    if (computed != expected) {
        return;
    }

    const std::string data = sentence.substr(7, asteriskPos - 7);
    if (sentence.starts_with("$GPGGA")) {
        parseGGA(data);
    }
    if (sentence.starts_with("$GPRMC")) {
        parseRMC(data);
    }
}

void GPS::parseGGA(const std::string &data) {
    const auto fields = tokenize<14>(data);

    if (fields[5].empty()) {
        return;
    }

    // const auto hours = parse<int>(fields[0].substr(0, 2));
    // const auto minutes = parse<int>(fields[0].substr(2, 2));
    // const auto seconds = parse<int>(fields[0].substr(4, 2));
    // const auto milliseconds = parse<int>(fields[0].substr(7));

    position.latitude = (parse<float>(fields[1].substr(0, 2))
                         + parse<float>(fields[1].substr(3)) / 60.0f)
                        * (fields[2][0] == 'N' ? 1.0f : -1.0f);

    position.longitude = (parse<float>(fields[3].substr(0, 3))
                          + parse<float>(fields[3].substr(4)) / 60.0f)
                         * (fields[4][0] == 'E' ? 1.0f : -1.0f);

    fixQuality = [&]() -> FixQuality {
        switch (fields[5][0]) {
            case '1': return FixQuality::SPS;
            case '2': return FixQuality::Differential;
            case '6': return FixQuality::DeadReckoning;
            default: return FixQuality::Invalid;
        }
    }();

    satellites = parse<uint8_t>(fields[6]);

    // const auto hdop = parse<float>(fields[7]);
    position.altitude = parse<float>(fields[8]);
    // const auto geoidSep = parse<float>(fields[10]);
    // const auto dgpsAge = parse<float>(fields[12]);
}

void GPS::parseRMC(const std::string &data) {
    const auto fields = tokenize<12>(data);

    if (fields[1].empty()) {
        return;
    }

    dateTime.hour = parse<int8_t>(fields[0].substr(0, 2));
    dateTime.minute = parse<int8_t>(fields[0].substr(2, 2));
    dateTime.second = parse<int8_t>(fields[0].substr(4, 2));
    dateTime.milliseconds = parse<int16_t>(fields[0].substr(7));

    const bool newStatus = fields[1][0] == 'A';
    if (newStatus != status) {
        status = newStatus;
        flagStatus = true;
    }

    position.latitude = (parse<float>(fields[2].substr(0, 2))
                         + parse<float>(fields[2].substr(3)) / 60.0f)
                        * (fields[3][0] == 'N' ? 1.0f : -1.0f);

    position.longitude = (parse<float>(fields[4].substr(0, 3))
                          + parse<float>(fields[4].substr(4)) / 60.0f)
                         * (fields[5][0] == 'E' ? 1.0f : -1.0f);

    position.speed = parse<float>(fields[6]);
    position.course = parse<float>(fields[7]);

    dateTime.day = parse<int8_t>(fields[8].substr(0, 2));
    dateTime.month = parse<int8_t>(fields[8].substr(2, 2));
    dateTime.year = [&]() -> int16_t {
        const auto yy = parse<int16_t>(fields[8].substr(4, 2));
        return static_cast<int16_t>(yy >= 80 ? 1900 + yy : 2000 + yy);
    }();

    if (newStatus) {
        flagDatetime = true;
    }

    // const auto magneticVariation    = parse<float>(fields[9]);
    // const auto magneticVariationDir = fields[10][0];    // E or W
}
