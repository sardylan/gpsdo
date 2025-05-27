#include "oscillator.h"

#include <si5351/si5351.h>

#include "config.h"
#include "log.h"

uint64_t oscillator_frequency_0;
uint64_t oscillator_frequency_1;
uint64_t oscillator_frequency_2;

int64_t oscillator_shift_0;
int64_t oscillator_shift_1;
int64_t oscillator_shift_2;

double oscillator_ratio_1;
double oscillator_ratio_2;

void oscillator_init() {
    log_info("Initialization");

    log_debug("Setting crystal frequency");
    si5351_init(0x60, SI5351_CRYSTAL_LOAD_10PF, 26000000, 0); // I am using a 26 MHz TCXO

    log_debug("Setting drive strength");
    si5351_drive_strength(SI5351_CLK0, SI5351_DRIVE_8MA);
    si5351_drive_strength(SI5351_CLK1, SI5351_DRIVE_8MA);
    si5351_drive_strength(SI5351_CLK2, SI5351_DRIVE_8MA);

    log_debug("Setting default frequencies");
    oscillator_frequency_0 = OSCILLATOR_FREQUENCY_CLK0;
    oscillator_frequency_1 = OSCILLATOR_FREQUENCY_CLK1;
    oscillator_frequency_2 = OSCILLATOR_FREQUENCY_CLK2;

    log_debug("Setting default shift");
    oscillator_shift_0 = 0;

    log_debug("Updating oscillators");
    oscillator_update();

    log_debug("Setting output enable status");
    si5351_output_enable(SI5351_CLK0, 1);
    si5351_output_enable(SI5351_CLK1, 1);
    si5351_output_enable(SI5351_CLK2, 1);

    log_debug("Resetting PLL");
    pll_reset(SI5351_PLLA);
}

uint64_t oscillator_get_frequency(const oscillator_clk clk) {
    switch (clk) {
        case OSCILLATOR_CLK_0:
            return oscillator_frequency_0;
        case OSCILLATOR_CLK_1:
            return oscillator_frequency_1;
        case OSCILLATOR_CLK_2:
            return oscillator_frequency_2;
        default:
            return 0;
    }
}

void oscillator_set_frequency(const oscillator_clk clk, const uint64_t frequency) {
    log_debug("Setting frequency for oscillator %s to %llu", oscillator_to_string(clk), frequency);

    switch (clk) {
        case OSCILLATOR_CLK_0:
            oscillator_frequency_0 = frequency;
            break;
        case OSCILLATOR_CLK_1:
            oscillator_frequency_1 = frequency;
            break;
        case OSCILLATOR_CLK_2:
            oscillator_frequency_2 = frequency;
            break;
    }

    oscillator_update();
}

int64_t oscillator_get_shift(const oscillator_clk clk) {
    switch (clk) {
        case OSCILLATOR_CLK_0:
            return oscillator_shift_0;
        case OSCILLATOR_CLK_1:
            return oscillator_shift_1;
        case OSCILLATOR_CLK_2:
            return oscillator_shift_2;
        default:
            return 0;
    }
}

void oscillator_set_shift(const int64_t shift) {
    log_debug("Setting shift to %llu", shift);
    oscillator_shift_0 = shift;
    oscillator_update();
}

void oscillator_update() {
    log_debug("Updating oscillators");

    oscillator_ratio_1 = (double) oscillator_frequency_1 / (double) oscillator_frequency_0;
    log_trace("Ratio CLK1 / CLK0 = %f", oscillator_ratio_1);
    oscillator_ratio_2 = (double) oscillator_frequency_2 / (double) oscillator_frequency_0;
    log_trace("Ratio CLK2 / CLK0 = %f", oscillator_ratio_2);

    log_trace("Shift CLK0 = %llu", oscillator_shift_0);
    oscillator_shift_1 = (int64_t) (oscillator_ratio_1 * (double) oscillator_shift_0);
    log_trace("Shift CLK1 = %llu", oscillator_shift_1);
    oscillator_shift_2 = (int64_t) (oscillator_ratio_2 * (double) oscillator_shift_0);
    log_trace("Shift CLK2 = %llu", oscillator_shift_2);

    const uint64_t freq_0 = oscillator_frequency_0 + oscillator_shift_0;
    log_trace("Frequency CLK0 = %llu", freq_0);
    const uint64_t freq_1 = oscillator_frequency_1 + oscillator_shift_1;
    log_trace("Frequency CLK1 = %llu", freq_1);
    const uint64_t freq_2 = oscillator_frequency_2 + oscillator_shift_2;
    log_trace("Frequency CLK2 = %llu", freq_2);

    si5351_set_freq(freq_0 * SI5351_FREQ_MULT, SI5351_CLK0);
    si5351_set_freq(freq_1 * SI5351_FREQ_MULT, SI5351_CLK1);
    si5351_set_freq(freq_2 * SI5351_FREQ_MULT, SI5351_CLK2);
}

const char *oscillator_to_string(const oscillator_clk clk) {
    switch (clk) {
        case OSCILLATOR_CLK_0:
            return "CLK0";
        case OSCILLATOR_CLK_1:
            return "CLK1";
        case OSCILLATOR_CLK_2:
            return "CLK2";
        default:
            return "Unknown";
    }
}
