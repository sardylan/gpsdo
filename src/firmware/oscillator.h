#ifndef GPSDO__FIRMWARE__OSCILLATOR__H
#define GPSDO__FIRMWARE__OSCILLATOR__H

#include "buildflags.h"
#include "log.h"

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

void oscillator_start();

uint32_t oscillator_get_frequency(oscillator_clk clk);

void oscillator_set_frequency(oscillator_clk clk, uint32_t frequency);

int32_t oscillator_get_shift();

void oscillator_set_shift(int32_t shift);

void oscillator_recompute_ratio();

void oscillator_update();

const char *oscillator_to_string(oscillator_clk clk);

#endif
