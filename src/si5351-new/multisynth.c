#include "multisynth.h"

#include "device.h"

void si5351_multisynth_set_source(si5351_ctx *ctx, const si5351_clock clock, const si5351_pll pll) {
    size_t clock_index;

    switch (clock) {
        case SI5351_CLOCK_0:
            clock_index = 0;
            break;
        case SI5351_CLOCK_1:
            clock_index = 1;
            break;
        case SI5351_CLOCK_2:
            clock_index = 2;
            break;
        case SI5351_CLOCK_3:
            clock_index = 3;
            break;
        case SI5351_CLOCK_4:
            clock_index = 4;
            break;
        case SI5351_CLOCK_5:
            clock_index = 5;
            break;
        case SI5351_CLOCK_6:
            clock_index = 6;
            break;
        case SI5351_CLOCK_7:
            clock_index = 7;
            break;
        default:
            return;
    }

    ctx->pll_assignment[clock_index] = pll;

    uint8_t register_value = si5351_device_read(ctx, SI5351_CLK0_CTRL);

    switch (pll) {
        case SI5351_PLL_A:
            register_value &= ~(SI5351_CLK_PLL_SELECT);
            break;
        case SI5351_PLL_B:
            register_value |= SI5351_CLK_PLL_SELECT;
            break;
    }

    si5351_device_write_single(ctx,SI5351_CLK0_CTRL, register_value);
}
