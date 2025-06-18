#include "oscillator.h"

#include <si5351/si5351.h>

uint32_t oscillator_frequency_0;
uint32_t oscillator_frequency_1;
uint32_t oscillator_frequency_2;

int32_t oscillator_shift_0;
int32_t oscillator_shift_1;
int32_t oscillator_shift_2;

double oscillator_ratio_0;
double oscillator_ratio_1;

void oscillator_init() {
    log_oscillator(info, "Initializing");

    log_oscillator(debug, "Initializing Si5351");
    si5351_init(OSCILLATOR_I2C_ADDRESS, SI5351_CRYSTAL_LOAD_8PF, OSCILLATOR_I2C_XTAL_FREQUENCY,
                OSCILLATOR_I2C_XTAL_FREQUENCY_CORRECTION);

    log_oscillator(debug, "Disabling clock powers");
    si5351_set_clock_pwr(SI5351_CLK0, 0);
    si5351_set_clock_pwr(SI5351_CLK1, 0);
    si5351_set_clock_pwr(SI5351_CLK2, 0);

    log_oscillator(debug, "Setting drive strength");
    si5351_drive_strength(SI5351_CLK0, SI5351_DRIVE_8MA);
    si5351_drive_strength(SI5351_CLK1, SI5351_DRIVE_8MA);
    si5351_drive_strength(SI5351_CLK2, SI5351_DRIVE_8MA);

    log_oscillator(debug, "Setting default frequencies");
    oscillator_frequency_0 = OSCILLATOR_FREQUENCY_CLK0;
    oscillator_frequency_1 = OSCILLATOR_FREQUENCY_CLK1;
    oscillator_frequency_2 = OSCILLATOR_FREQUENCY_CLK2;

    log_oscillator(debug, "Setting default shift");
    oscillator_shift_2 = 0;

    log_oscillator(debug, "Recomputing ratios");
    oscillator_recompute_ratio();

    log_oscillator(debug, "Updating oscillators");
    oscillator_update();
}

void oscillator_start() {
    log_oscillator(info, "Oscillator start");

    log_oscillator(debug, "Enabling clock power");
    si5351_set_clock_pwr(SI5351_CLK0, 1);
    si5351_set_clock_pwr(SI5351_CLK1, 1);
    si5351_set_clock_pwr(SI5351_CLK2, 1);

    log_oscillator(debug, "Enabling outputs");
    si5351_output_enable(SI5351_CLK0, 1);
    si5351_output_enable(SI5351_CLK1, 1);
    si5351_output_enable(SI5351_CLK2, 1);
}

uint32_t oscillator_get_frequency(const oscillator_clk clk) {
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

void oscillator_set_frequency(const oscillator_clk clk, const uint32_t frequency) {
    log_oscillator(debug, "Setting frequency for oscillator %s to %u", oscillator_to_string(clk), frequency);

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

    oscillator_recompute_ratio();

    oscillator_update();
}

int32_t oscillator_get_shift() {
    return oscillator_shift_2;
}

void oscillator_set_shift(const int32_t shift) {
    log_oscillator(debug, "Setting shift to %u", shift);
    oscillator_shift_2 = shift;
    oscillator_update();
}

void oscillator_recompute_ratio() {
    log_oscillator(debug, "Recomputing ratios");

    oscillator_ratio_0 = (double) oscillator_frequency_0 / (double) oscillator_frequency_2;
    oscillator_ratio_1 = (double) oscillator_frequency_1 / (double) oscillator_frequency_2;
    log_oscillator(trace, "Ratio CLK0 / CLK2 = %f", oscillator_ratio_0);
    log_oscillator(trace, "Ratio CLK1 / CLK2 = %f", oscillator_ratio_1);
}

void oscillator_update() {
    log_oscillator(debug, "Updating oscillators");

    oscillator_shift_0 = (int32_t) (oscillator_ratio_0 * (double) oscillator_shift_2);
    log_oscillator(trace, "Shift CLK0 = %u", oscillator_shift_0);
    oscillator_shift_1 = (int32_t) (oscillator_ratio_1 * (double) oscillator_shift_2);
    log_oscillator(trace, "Shift CLK1 = %u", oscillator_shift_1);
    log_oscillator(trace, "Shift CLK2 = %u", oscillator_shift_2);

    const uint32_t freq_0 = oscillator_frequency_0 + oscillator_shift_0;
    log_oscillator(trace, "Frequency CLK0 = %u", freq_0);
    const uint32_t freq_1 = oscillator_frequency_1 + oscillator_shift_1;
    log_oscillator(trace, "Frequency CLK1 = %u", freq_1);
    const uint32_t freq_2 = oscillator_frequency_2 + oscillator_shift_2;
    log_oscillator(trace, "Frequency CLK2 = %u", freq_2);

    log_oscillator(debug, "Setting frequencies");
    log_oscillator(trace, "Setting frequency for clock 0");
    si5351_set_freq(freq_0 * SI5351_FREQ_MULT, SI5351_CLK0);
    log_oscillator(trace, "Setting frequency for clock 1");
    si5351_set_freq(freq_1 * SI5351_FREQ_MULT, SI5351_CLK1);
    log_oscillator(trace, "Setting frequency for clock 2");
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
