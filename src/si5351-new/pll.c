#include "pll.h"

#include "const.h"
#include "utilities.h"
#include "device.h"

void si5351_pll_set_frequency(si5351_ctx *ctx, const si5351_pll pll, const si5351_frequency freq) {
    si5351_reg_set pll_reg;

    si5351_pll_calc(ctx, freq, &pll_reg, ctx->xtal_freq_correction, false);

    const uint8_t params[] = {
        pll_reg.p3 >> 8 & 0xFF,
        pll_reg.p3 & 0xFF,
        pll_reg.p1 >> 16 & 0x03,
        pll_reg.p1 >> 8 & 0xFF,
        pll_reg.p1 & 0xFF,
        (pll_reg.p3 >> 12 & 0xF0) + (pll_reg.p2 >> 16 & 0x0F),
        pll_reg.p2 >> 8 & 0xFF,
        pll_reg.p2 & 0xFF
    };

    switch (pll) {
        case SI5351_PLL_A:
            ctx->pll_a_freq = freq;
            si5351_device_write(ctx, SI5351_PLLA_PARAMETERS, params, 8);
            break;

        case SI5351_PLL_B:
            ctx->pll_b_freq = freq;
            si5351_device_write(ctx, SI5351_PLLB_PARAMETERS, params, 8);
            break;
    }
}

uint64_t si5351_pll_calc(const si5351_ctx *ctx,
                         si5351_frequency freq,
                         si5351_reg_set *reg,
                         const si5351_frequency_correction correction,
                         const bool vcxo) {
    si5351_frequency ref_freq = ctx->xtal_freq * SI5351_FREQ_MULT;
    ref_freq += (int32_t) (((int64_t) correction << 31) / 1000000000LL * ref_freq >> 31);

    if (freq < SI5351_PLL_VCO_MIN * SI5351_FREQ_MULT)
        freq = SI5351_PLL_VCO_MIN * SI5351_FREQ_MULT;
    else if (freq > SI5351_PLL_VCO_MAX * SI5351_FREQ_MULT)
        freq = SI5351_PLL_VCO_MAX * SI5351_FREQ_MULT;

    const si5351_frequency a = freq / ref_freq;
    if (a < SI5351_PLL_MIN)
        freq = ref_freq * SI5351_PLL_MIN;
    else if (a > SI5351_PLL_A_MAX)
        freq = ref_freq * SI5351_PLL_A_MAX;

    si5351_frequency b;
    si5351_frequency c;
    if (vcxo) {
        b = freq % ref_freq * 1000000ULL / ref_freq;
        c = 1000000ULL;
    } else {
        b = freq % ref_freq * RFRAC_DENOM / ref_freq;
        c = b ? RFRAC_DENOM : 1;
    }

    const uint32_t p1 = 128 * a + 128 * b / c - 512;
    const uint32_t p2 = 128 * b - c * (128 * b / c);
    const uint32_t p3 = c;

    uint64_t lltmp = ref_freq;
    lltmp *= b;
    do_div(&lltmp, c);
    freq = lltmp;
    freq += ref_freq * a;

    reg->p1 = p1;
    reg->p2 = p2;
    reg->p3 = p3;

    if (vcxo) {
        return 128 * a * 1000000ULL + b;
    }

    return freq;
}

void si5351_pll_reset(const si5351_ctx *ctx, const si5351_pll pll) {
    switch (pll) {
        case SI5351_PLL_A:
            si5351_device_write_single(ctx, SI5351_PLL_RESET, SI5351_PLL_RESET_A);
            break;
        case SI5351_PLL_B:
            si5351_device_write_single(ctx, SI5351_PLL_RESET, SI5351_PLL_RESET_B);
            break;
    }
}
