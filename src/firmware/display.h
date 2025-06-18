#ifndef GPSDO__FIRMWARE__DISPLAY__H
#define GPSDO__FIRMWARE__DISPLAY__H

#include <stdint.h>

void display_init();

void display_frequency(uint32_t frequency);

void display_shift(int32_t shift);

#endif
