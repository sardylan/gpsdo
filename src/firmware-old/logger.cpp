#include "logger.hpp"

#include <algorithm>
#include <chrono>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>

#include <buildprop.hpp>
#include <hardware/rtc.h>

Logger::Logger(
    const std::chrono::time_point<std::chrono::system_clock> now,
    const LogLevel logLevel,
    std::string function,
    std::string file,
    const int line)
    : now(now), logLevel(logLevel), function(std::move(function)), fileName(std::move(file)), line(line) {
    stream = new std::stringstream();

    init();
}

Logger::~Logger() {
    finalize();

    delete stream;
}

void Logger::init() {
    addDateTime();
    addLevel();
    addCodeReferences();

    std::ostringstream imploded;
    std::ranges::copy(items, std::ostream_iterator<std::string>(imploded, " "));
    *stream << imploded.str();
}

void Logger::finalize() const {
    const std::string &logLine = stream->str();
    Logger::logLine(logLevel, logLine);
}

void Logger::addDateTime() {
    datetime_t t;
    rtc_get_datetime(&t);

    char datetimeString[31];
    std::snprintf(datetimeString, 20, "%d-%d-%d %d:%d:%d", t.year, t.month, t.day, t.hour, t.month, t.day);
    snprintf(datetimeString + 19, 8, ".%06lu", 0);
    snprintf(datetimeString + 26, 5, " UTC");

    items.emplace_back(datetimeString);
}

void Logger::addLevel() {
    switch (logLevel) {
        case LogLevel::OFF:
            items.emplace_back("[ OFF ]");
            break;
        case LogLevel::ERROR:
            items.emplace_back("[ERROR]");
            break;
        case LogLevel::WARNING:
            items.emplace_back("[WARN ]");
            break;
        case LogLevel::INFO:
            items.emplace_back("[INFO ]");
            break;
        case LogLevel::DEBUG:
            items.emplace_back("[DEBUG]");
            break;
        case LogLevel::TRACE:
            items.emplace_back("[TRACE]");
            break;
    }
}

void Logger::addCodeReferences() {
    std::stringstream string;
    string << "(" << fileName.replace(0, sizeof(APPLICATION_BUILD_PATH), "") << ":" << line << ")";
    string << " ";
    string << "[" << function << "]";
    items.emplace_back(string.str());
}

LogLevel Logger::parse(const std::string &originalValue) {
    std::string value = originalValue;
    std::ranges::transform(value, value.begin(), toupper);

    if (value == "OFF")
        return LogLevel::OFF;
    if (value == "ERROR")
        return LogLevel::ERROR;
    if (value == "WARNING")
        return LogLevel::WARNING;
    if (value == "INFO")
        return LogLevel::INFO;
    if (value == "DEBUG")
        return LogLevel::DEBUG;
    if (value == "TRACE")
        return LogLevel::TRACE;

    return LogLevel::OFF;
}

void Logger::logLine(const LogLevel level, const std::string &line) {
    LOGGER_OUTPUT_STREAM << line << std::endl << std::unitbuf;
}

std::ostream &operator<<(std::ostream &ostream, const LogLevel &logLevel) {
    switch (logLevel) {
        case LogLevel::OFF:
            ostream << "OFF";
            break;
        case LogLevel::ERROR:
            ostream << "ERROR";
            break;
        case LogLevel::WARNING:
            ostream << "WARNING";
            break;
        case LogLevel::INFO:
            ostream << "INFO";
            break;
        case LogLevel::DEBUG:
            ostream << "DEBUG";
            break;
        case LogLevel::TRACE:
            ostream << "TRACE";
            break;
    }

    return ostream;
}
