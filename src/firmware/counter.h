#ifndef GPSDO__FIRMWARE__COUNTER__H
#define GPSDO__FIRMWARE__COUNTER__H

#include "buildflags.h"
#include "log.h"

#ifdef LOG_COUNTER_ENABLED
#define log_counter(level, format, ...) log_##level(format, ##__VA_ARGS__)
#else
#define log_counter(level, format, ...)
#endif

enum counter_event_t {
    COUNTER_EVENT_PPS
};

typedef enum counter_event_t counter_event;

void counter_init();

uint32_t counter_get_value();

void counter_pps_irq_handler(uint gpio, uint32_t events);

bool counter_event_get(counter_event event);

void counter_event_reset(counter_event event);

void counter_overflow();

#endif
