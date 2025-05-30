#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>

#include "pico/types.h"

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

#define loopDelay(x) \
    sleep_ms(x); \
    tight_loop_contents()

#define task_init(name) uint64_t last_execution_##name = 0

#define task_now() const uint64_t now = time_us_64();

#define task_schedule(name, interval) \
    if (now - last_execution_##name >= interval) { \
        last_execution_##name = now; \
        task_##name(); \
    }

void program();
void task_frequency_correction();

#endif
