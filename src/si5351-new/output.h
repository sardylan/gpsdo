#ifndef GPSDO__SI5351__OUTPUT__H
#define GPSDO__SI5351__OUTPUT__H

#include "enums.h"
#include "context.h"

void si5351_output_set(const si5351_ctx *ctx, si5351_clock clock, bool enabled);

void si5351_output_drive_strength(const si5351_ctx *ctx, si5351_clock clock, si5351_drive_strength drive_strength);

#endif
