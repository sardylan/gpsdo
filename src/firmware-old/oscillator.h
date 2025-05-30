#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include <stdint.h>

#include "buildflags.h"

#ifdef LOG_OSCILLATOR_ENABLED
#define log_oscillator(level, format, ...) log_##level(format, ##__VA_ARGS__)
#else
#define log_oscillator(level, format, ...)
#endif

enum oscillator_clk_t {
    OSCILLATOR_CLK_0,
    OSCILLATOR_CLK_1,
    OSCILLATOR_CLK_2
};

typedef enum oscillator_clk_t oscillator_clk;

void oscillator_init();

uint64_t oscillator_get_frequency(oscillator_clk clk);

void oscillator_set_frequency(oscillator_clk clk, uint64_t frequency);

int64_t oscillator_get_shift(oscillator_clk clk);

void oscillator_set_shift(int64_t shift);

void oscillator_recompute_ratio();

void oscillator_update();

const char *oscillator_to_string(oscillator_clk clk);

#endif
