#ifndef GPSDO__SI5351__FREQUENCY__H
#define GPSDO__SI5351__FREQUENCY__H

#include "enums.h"
#include "const.h"
#include "context.h"

void si5351_frequency_set(si5351_ctx *ctx, si5351_clock clock, si5351_frequency freq);

#endif
