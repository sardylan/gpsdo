#ifndef LOGGER_H
#define LOGGER_H

#include <chrono>
#include <list>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#define LOGGER_OUTPUT_STREAM std::cout

#define getLogger(level) \
    Logger(std::chrono::system_clock::now(), level, __PRETTY_FUNCTION__, __FILE__, __LINE__)

#ifdef LOG_ERROR_ENABLED
#define logError(x) getLogger(LogLevel::ERROR) << x
#else
#define logError(x)
#endif

#ifdef LOG_WARNING_ENABLED
#define logWarning(x) getLogger(LogLevel::WARNING) << x
#else
#define logWarning(x)
#endif

#ifdef LOG_INFO_ENABLED
#define logInfo(x) getLogger(LogLevel::INFO) << x
#else
#define logInfo(x)
#endif

#ifdef LOG_DEBUG_ENABLED
#define logDebug(x) getLogger(LogLevel::DEBUG) << x
#else
#define logDebug(x)
#endif

#ifdef LOG_TRACE_ENABLED
#define logTrace(x) getLogger(LogLevel::TRACE) << x
#else
#define logTrace(x)
#endif

#define elseError(message, ...) \
    else { \
        logError(message, ##__VA_ARGS__); \
    }

#define elseWarning(message, ...) \
    else { \
        logWarning(message, ##__VA_ARGS__); \
    }

#define elseInfo(message, ...) \
    else { \
        logInfo(message, ##__VA_ARGS__); \
    }

#define elseDebug(message, ...) \
    else { \
        logDebug(message, ##__VA_ARGS__); \
    }

#define elseTrace(message, ...) \
    else { \
        logTrace(message, ##__VA_ARGS__); \
    }

enum class LogLevel {
    OFF = 0,
    ERROR = 1,
    WARNING = 2,
    INFO = 3,
    DEBUG = 4,
    TRACE = 5
};

class Logger {
public:
    explicit Logger(
        std::chrono::time_point<std::chrono::system_clock> now,
        LogLevel logLevel,
        std::string function,
        std::string file,
        int line);

    ~Logger();

    template<class T>
    Logger &operator<<(const T &item);

    static LogLevel parse(const std::string &originalValue);

private:
    std::list<std::string> items;

    std::stringstream *stream;

    std::chrono::time_point<std::chrono::system_clock> now;
    LogLevel logLevel;
    std::string function;
    std::string fileName;
    int line;

    void init();

    void finalize() const;

    void addDateTime();

    void addLevel();

    void addCodeReferences();

    static void logLine(LogLevel level, const std::string &line);
};

std::ostream &operator<<(std::ostream &ostream, const LogLevel &logLevel);

template<typename T>
std::ostream &operator<<(std::ostream &ostream, const std::vector<T> &vector);

#include "logger.tpp"

#endif
