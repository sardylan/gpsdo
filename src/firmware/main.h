#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>

#include "pico/types.h"

#define loopDelay(x) \
    sleep_ms(x); \
    tight_loop_contents()

void counter_overflow();

void gps_rx();

void gps_pps_callback(uint gpio, uint32_t event_mask);

#endif
