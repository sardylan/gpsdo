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

#include "logger.hpp"

#include <cstdarg>
#include <cstring>
#include <iostream>

#include <buildprop.hpp>

using namespace gpsdo::logger;

Logger Logger::instance;

Logger &Logger::getInstance() {
    return instance;
}

Logger::Logger() {
    mutex_init(&log_cs);
}

Logger::~Logger() = default;


void Logger::message(const char *file, const std::size_t line, const char *level, const char *format, ...) {
    char message[512];
    char buffer[1024];
    std::va_list args;

    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    snprintf(buffer, sizeof(buffer),
             "%020llu (%d) [%s] {%s:%u}: %s",
             time_us_64(),
             get_core_num(),
             level,
             file + APPLICATION_BUILD_PATH_LENGTH,
             line,
             message);

    mutex_enter_blocking(&log_cs);
    std::printf("%s\n\r", buffer);
    mutex_exit(&log_cs);
}
