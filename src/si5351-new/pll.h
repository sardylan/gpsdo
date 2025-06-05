#ifndef GPSDO__SI5351__PLL__H
#define GPSDO__SI5351__PLL__H

#include "enums.h"
#include "const.h"
#include "context.h"
#include "device.h"

void si5351_pll_set_frequency(si5351_ctx *ctx, si5351_pll pll, si5351_frequency freq);

uint64_t si5351_pll_calc(const si5351_ctx *ctx,
                         si5351_frequency freq,
                         si5351_reg_set *reg,
                         si5351_frequency_correction correction,
                         bool vcxo);

void si5351_pll_reset(const si5351_ctx *ctx, si5351_pll pll);

#endif
