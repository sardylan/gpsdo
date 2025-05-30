#ifndef COUNTER_H
#define COUNTER_H

#include <stdint.h>

#include "buildflags.h"

#ifdef LOG_COUNTER_ENABLED
#define log_counter(level, format, ...) log_##level(format, ##__VA_ARGS__)
#else
#define log_counter(level, format, ...)
#endif

void counter_init();

void counter_overflow();

void counter_read();

uint64_t counter_value();

int64_t counter_delta();

#endif
