#ifndef GPSDO__SI5351__MULTISYNTH__H
#define GPSDO__SI5351__MULTISYNTH__H

#include "context.h"
#include "enums.h"

void si5351_multisynth_set_source(si5351_ctx *ctx, si5351_clock clock, si5351_pll pll);

#endif
