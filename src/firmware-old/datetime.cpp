#include "datetime.hpp"

#include <hardware/rtc.h>
#include <pico/types.h>

void datetimeSetRtc(
    const std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<long long> > datetime) {
    const time_t tt = std::chrono::system_clock::to_time_t(datetime);
    const tm local_tm = *localtime(&tt);
    datetimeSetRtc(local_tm.tm_year + 1900,
                   local_tm.tm_mon + 1,
                   local_tm.tm_mday,
                   local_tm.tm_hour,
                   local_tm.tm_min,
                   local_tm.tm_sec);
}

void datetimeSetRtc(const int16_t year,
                    const int8_t month,
                    const int8_t day,
                    const int8_t hour,
                    const int8_t min,
                    const int8_t sec) {
    const datetime_t t = {
        .year = year,
        .month = month,
        .day = day,
        .hour = hour,
        .min = min,
        .sec = sec
    };

    rtc_set_datetime(&t);
}
