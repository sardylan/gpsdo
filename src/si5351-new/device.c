#include "device.h"

#include <string.h>

#include "const.h"
#include "multisynth.h"
#include "output.h"
#include "pll.h"

si5351_exit_type si5351_device_init(si5351_ctx *ctx) {
    SI5351_CTX_NULL_CHECK(ctx);

    uint8_t status_reg = 0;
    do {
        status_reg = si5351_device_read(ctx,SI5351_DEVICE_STATUS);
    } while (status_reg >> 7 == 1);

    si5351_device_write_single(ctx,
                               SI5351_CRYSTAL_LOAD,
                               ctx->xtal_load_capacitance & SI5351_CRYSTAL_LOAD_MASK | 0b00010010);

    // if (ref_freq <= 30000000UL) {
    //     xtal_freq[(uint8_t) ref_osc] = ref_freq;
    //     //reg_val |= SI5351_CLKIN_DIV_1;
    //     if (ref_osc == SI5351_PLL_INPUT_CLKIN) {
    //         clkin_div = SI5351_CLKIN_DIV_1;
    //     }
    // } else if (ref_freq > 30000000UL && ref_freq <= 60000000UL) {
    //     xtal_freq[(uint8_t) ref_osc] = ref_freq / 2;
    //     //reg_val |= SI5351_CLKIN_DIV_2;
    //     if (ref_osc == SI5351_PLL_INPUT_CLKIN) {
    //         clkin_div = SI5351_CLKIN_DIV_2;
    //     }
    // } else if (ref_freq > 60000000UL && ref_freq <= 100000000UL) {
    //     xtal_freq[(uint8_t) ref_osc] = ref_freq / 4;
    //     //reg_val |= SI5351_CLKIN_DIV_4;
    //     if (ref_osc == SI5351_PLL_INPUT_CLKIN) {
    //         clkin_div = SI5351_CLKIN_DIV_4;
    //     }
    // } else {
    //     //reg_val |= SI5351_CLKIN_DIV_1;
    // }

    // si5351_pll_set_frequency(ctx, SI5351_PLL_A, SI5351_PLL_FIXED);
    // si5351_pll_set_frequency(ctx, SI5351_PLL_B, SI5351_PLL_FIXED);

    si5351_device_reset(ctx);

    si5351_exit_success();
}

void si5351_device_reset(si5351_ctx *ctx) {
    si5351_device_write_single(ctx, 16, 0x80);
    si5351_device_write_single(ctx, 17, 0x80);
    si5351_device_write_single(ctx, 18, 0x80);
    si5351_device_write_single(ctx, 19, 0x80);
    si5351_device_write_single(ctx, 20, 0x80);
    si5351_device_write_single(ctx, 21, 0x80);
    si5351_device_write_single(ctx, 22, 0x80);
    si5351_device_write_single(ctx, 23, 0x80);

    si5351_device_write_single(ctx, 16, 0x0c);
    si5351_device_write_single(ctx, 17, 0x0c);
    si5351_device_write_single(ctx, 18, 0x0c);
    si5351_device_write_single(ctx, 19, 0x0c);
    si5351_device_write_single(ctx, 20, 0x0c);
    si5351_device_write_single(ctx, 21, 0x0c);
    si5351_device_write_single(ctx, 22, 0x0c);
    si5351_device_write_single(ctx, 23, 0x0c);

    si5351_pll_set_frequency(ctx, SI5351_PLL_A, SI5351_PLL_FIXED);
    si5351_pll_set_frequency(ctx, SI5351_PLL_B, SI5351_PLL_FIXED);

    si5351_multisynth_set_source(ctx, SI5351_CLOCK_0, SI5351_PLL_A);
    si5351_multisynth_set_source(ctx, SI5351_CLOCK_1, SI5351_PLL_A);
    si5351_multisynth_set_source(ctx, SI5351_CLOCK_2, SI5351_PLL_A);
    si5351_multisynth_set_source(ctx, SI5351_CLOCK_3, SI5351_PLL_A);
    si5351_multisynth_set_source(ctx, SI5351_CLOCK_4, SI5351_PLL_A);
    si5351_multisynth_set_source(ctx, SI5351_CLOCK_5, SI5351_PLL_A);
    si5351_multisynth_set_source(ctx, SI5351_CLOCK_6, SI5351_PLL_B);
    si5351_multisynth_set_source(ctx, SI5351_CLOCK_7, SI5351_PLL_B);

    si5351_device_write_single(ctx,SI5351_VXCO_PARAMETERS_LOW, 0);
    si5351_device_write_single(ctx,SI5351_VXCO_PARAMETERS_MID, 0);
    si5351_device_write_single(ctx,SI5351_VXCO_PARAMETERS_HIGH, 0);

    // Then reset the PLLs
    si5351_pll_reset(ctx, SI5351_PLL_A);
    si5351_pll_reset(ctx, SI5351_PLL_B);

    ctx->frequency[0] = 0;
    ctx->frequency[1] = 0;
    ctx->frequency[2] = 0;
    ctx->frequency[3] = 0;
    ctx->frequency[4] = 0;
    ctx->frequency[5] = 0;
    ctx->frequency[6] = 0;
    ctx->frequency[7] = 0;

    si5351_output_set(ctx, SI5351_CLOCK_0, false);
    si5351_output_set(ctx, SI5351_CLOCK_1, false);
    si5351_output_set(ctx, SI5351_CLOCK_2, false);
    si5351_output_set(ctx, SI5351_CLOCK_3, false);
    si5351_output_set(ctx, SI5351_CLOCK_4, false);
    si5351_output_set(ctx, SI5351_CLOCK_5, false);
    si5351_output_set(ctx, SI5351_CLOCK_6, false);
    si5351_output_set(ctx, SI5351_CLOCK_7, false);
}

uint8_t si5351_device_read(const si5351_ctx *ctx, const uint8_t reg_addr) {
    uint8_t buf;

    i2c_write_blocking(ctx->bus, ctx->address, &reg_addr, 1, true);
    i2c_read_blocking(ctx->bus, ctx->address, &buf, 1, false);

    return buf;
}

void si5351_device_write(const si5351_ctx *ctx, const uint8_t reg_addr, const uint8_t *data, const size_t data_size) {
    uint8_t msg[SI5351_BUFFER_SIZE];
    size_t length = data_size + 1;

    if (length > SI5351_BUFFER_SIZE)
        length = SI5351_BUFFER_SIZE;

    msg[0] = reg_addr;
    memcpy(&msg[1], data, length - 1);

    i2c_write_blocking(ctx->bus, ctx->address, msg, length, false);
}

void si5351_device_write_single(const si5351_ctx *ctx, const uint8_t reg_addr, uint8_t val) {
    si5351_device_write(ctx, reg_addr, &val, 1);
}
