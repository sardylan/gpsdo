#ifndef LOGGER_T
#define LOGGER_T

#include "logger.hpp"


template <class T>
Logger& Logger::operator<<(const T& item) {
    *stream << " " << item;
    return *this;
}

template <typename T>
std::ostream& operator<<(std::ostream& ostream, const std::vector<T>& vector) {
    ostream << "[";

    for (size_t i = 0; i < vector.size(); i++) {
        if (i > 0)
            ostream << " ";
        ostream << vector[i];
    }

    ostream << "]";

    return ostream;
}

#endif
