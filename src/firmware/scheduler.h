#ifndef GPSDO__FIRMWARE__SCHEDULER__H
#define GPSDO__FIRMWARE__SCHEDULER__H

#include <stdint.h>


typedef uint64_t scheduler_timestamp_t;

#define scheduler_declare(name) scheduler_timestamp_t scheduler_##name##_last_run

#define scheduler_init(name) scheduler_##name##_last_run = 0

#define scheduler_now() const scheduler_timestamp_t scheduler_time_now = time_us_64()

#define scheduler_task_s(name, interval) scheduler_task_us(name, interval##000000)

#define scheduler_task_ms(name, interval) scheduler_task_us(name, interval##000)

#define scheduler_task_us(name, interval) \
    if (scheduler_time_now - scheduler_##name##_last_run >= interval) { \
        scheduler_##name##_last_run = scheduler_time_now; \
        task_##name(); \
    }

#define scheduler_task_header(x) void task_##x()

#endif
