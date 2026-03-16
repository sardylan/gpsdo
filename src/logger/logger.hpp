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

#ifndef __LOGGER__LOGGER__H
#define __LOGGER__LOGGER__H

#include <cstddef>

#include <pico/mutex.h>

#define logMessage(level, format, ...) gpsdo::logger::Logger::getInstance().message(__FILE__, __LINE__, level, format, ##__VA_ARGS__)

#ifdef LOG_ERROR_ENABLED
#define logError(format, ...) logMessage("ERROR", format, ##__VA_ARGS__)
#else
#define logError(format, ...)
#endif

#ifdef LOG_WARNING_ENABLED
#define logWarn(format, ...) logMessage("WARN ", format, ##__VA_ARGS__)
#else
#define logWarn(format, ...)
#endif

#ifdef LOG_INFO_ENABLED
#define logInfo(format, ...) logMessage("INFO ", format, ##__VA_ARGS__)
#else
#define logInfo(format, ...)
#endif

#ifdef LOG_DEBUG_ENABLED
#define logDebug(format, ...) logMessage("DEBUG", format, ##__VA_ARGS__)
#else
#define logDebug(format, ...)
#endif

#ifdef LOG_TRACE_ENABLED
#define logTrace(format, ...) logMessage("TRACE", format, ##__VA_ARGS__)
#else
#define logTrace(format, ...)
#endif

namespace gpsdo::logger {
    class Logger {
    public:
        static Logger &getInstance();

        void message(const char *file, std::size_t line, const char *level, const char *format, ...);

    private:
        static Logger instance;

        Logger();

        ~Logger();

        mutex_t log_cs{};
    };
}

#endif
