#include "context.h"

#include <stdlib.h>

#include "default.h"

si5351_exit_type si5351_context_init(si5351_ctx *ctx, i2c_inst_t *bus, const uint sda, const uint scl) {
    SI5351_CTX_NULL_CHECK(ctx);

    ctx->bus = bus;
    ctx->sda = sda;
    ctx->scl = scl;

    ctx->address = SI5351_DEFAULT_ADDRESS;
    ctx->baudrate = SI5351_DEFAULT_BAUDRATE;
    ctx->xtal_load_capacitance = SI5351_DEFAULT_XTAL_LOAD_C;
    ctx->xtal_freq = SI5351_DEFAULT_XTAL_FREQ;
    ctx->xtal_freq_correction = SI5351_DEFAULT_XTAL_FREQ_CORRECTION;
    ctx->pll_a_freq = SI5351_DEFAULT_PLL_A_FREQ;
    ctx->pll_b_freq = SI5351_DEFAULT_PLL_B_FREQ;

    ctx->pll_assignment[0] = SI5351_PLL_A;
    ctx->pll_assignment[1] = SI5351_PLL_A;
    ctx->pll_assignment[2] = SI5351_PLL_A;
    ctx->pll_assignment[3] = SI5351_PLL_A;
    ctx->pll_assignment[4] = SI5351_PLL_A;
    ctx->pll_assignment[5] = SI5351_PLL_A;
    ctx->pll_assignment[6] = SI5351_PLL_B;
    ctx->pll_assignment[7] = SI5351_PLL_B;

    ctx->frequency[0] = 0;
    ctx->frequency[1] = 0;
    ctx->frequency[2] = 0;
    ctx->frequency[3] = 0;
    ctx->frequency[4] = 0;
    ctx->frequency[5] = 0;
    ctx->frequency[6] = 0;
    ctx->frequency[7] = 0;

    si5351_exit_success();
}

si5351_exit_type si5351_context_set_address(si5351_ctx *ctx, const uint address) {
    SI5351_CTX_NULL_CHECK(ctx);
    ctx->address = address;
    si5351_exit_success();
}

si5351_exit_type si5351_context_set_baudrate(si5351_ctx *ctx, const uint baudrate) {
    SI5351_CTX_NULL_CHECK(ctx);
    ctx->baudrate = baudrate;
    si5351_exit_success();
}

si5351_exit_type si5351_context_set_xtal_load_c(si5351_ctx *ctx, const uint8_t xtal_load_c) {
    SI5351_CTX_NULL_CHECK(ctx);
    ctx->xtal_load_capacitance = xtal_load_c;
    si5351_exit_success();
}

si5351_exit_type si5351_context_set_xtal_freq(si5351_ctx *ctx, const si5351_frequency xtal_freq) {
    SI5351_CTX_NULL_CHECK(ctx);
    ctx->xtal_freq = xtal_freq;
    si5351_exit_success();
}

si5351_exit_type si5351_context_set_xtal_freq_correction(si5351_ctx *ctx, const si5351_frequency_correction xtal_freq_correction) {
    SI5351_CTX_NULL_CHECK(ctx);
    ctx->xtal_freq_correction = xtal_freq_correction;
    si5351_exit_success();
}
