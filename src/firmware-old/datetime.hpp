#ifndef DATETIME_H
#define DATETIME_H

#include <chrono>
#include <cstdint>

void datetimeSetRtc(std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<long long> > datetime);

void datetimeSetRtc(int16_t year, int8_t month, int8_t day, int8_t hour, int8_t min, int8_t sec);

#endif
