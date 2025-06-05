#ifndef GPSDO__SI5351__CONTEXT__H
#define GPSDO__SI5351__CONTEXT__H

#include <stdlib.h>
#include <pico/types.h>
#include <hardware/i2c.h>

#include "const.h"
#include "enums.h"

struct si5351_ctx_t {
    i2c_inst_t *bus;
    uint8_t address;
    uint baudrate;
    uint sda;
    uint scl;
    uint8_t xtal_load_capacitance;
    si5351_frequency xtal_freq;
    si5351_frequency_correction xtal_freq_correction;

    si5351_frequency pll_a_freq;
    si5351_frequency pll_b_freq;

    si5351_pll pll_assignment[8];
    si5351_frequency frequency[8];
};

typedef struct si5351_ctx_t si5351_ctx;

#define SI5351_CTX_NULL_CHECK(ctx) \
    if (ctx == nullptr) \
        si5351_exit_failure()

si5351_exit_type si5351_context_init(si5351_ctx *ctx, i2c_inst_t *bus, uint sda, uint scl);

si5351_exit_type si5351_context_set_address(si5351_ctx *ctx, uint address);

si5351_exit_type si5351_context_set_baudrate(si5351_ctx *ctx, uint baudrate);

si5351_exit_type si5351_context_set_xtal_load_c(si5351_ctx *ctx, uint8_t xtal_load_c);

si5351_exit_type si5351_context_set_xtal_freq(si5351_ctx *ctx, si5351_frequency xtal_freq);

si5351_exit_type si5351_context_set_xtal_freq_correction(si5351_ctx *ctx,
                                                         si5351_frequency_correction xtal_freq_correction);

#endif
