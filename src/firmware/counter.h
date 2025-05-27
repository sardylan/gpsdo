#ifndef COUNTER_H
#define COUNTER_H

#include <stdint.h>

void counter_init();

void counter_overflow();

void counter_read();

uint64_t counter_value();

uint64_t counter_delta();

#endif
