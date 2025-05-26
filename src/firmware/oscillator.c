#include "oscillator.h"

#include "config.h"
#include "si5351.h"
#include "log.h"

void oscillator_init() {
    log_info("Initialization");

    log_debug("Setting crystal frequency");
    si5351_init(0x60, SI5351_CRYSTAL_LOAD_10PF, 26000000, 0); // I am using a 26 MHz TCXO

    log_debug("Setting drive strength");
    si5351_drive_strength(SI5351_CLK0, SI5351_DRIVE_8MA);
    si5351_drive_strength(SI5351_CLK1, SI5351_DRIVE_8MA);
    si5351_drive_strength(SI5351_CLK2, SI5351_DRIVE_8MA);

    log_debug("Setting frequencies for each oscillator");
    log_trace("Setting frequency to %llu for oscillator CLK0", OSCILLATOR_FREQUENCY_CLK0);
    si5351_set_freq(OSCILLATOR_FREQUENCY_CLK0 * SI5351_FREQ_MULT, SI5351_CLK0);
    log_trace("Setting frequency to %llu for oscillator CLK1", OSCILLATOR_FREQUENCY_CLK1);
    si5351_set_freq(OSCILLATOR_FREQUENCY_CLK1 * SI5351_FREQ_MULT, SI5351_CLK1);
    log_trace("Setting frequency to %llu for oscillator CLK2", OSCILLATOR_FREQUENCY_CLK2);
    si5351_set_freq(OSCILLATOR_FREQUENCY_CLK2 * SI5351_FREQ_MULT, SI5351_CLK2);

    log_debug("Setting output enable status");
    si5351_output_enable(SI5351_CLK0, 1);
    si5351_output_enable(SI5351_CLK1, 1);
    si5351_output_enable(SI5351_CLK2, 1);

    log_debug("Resetting PLL");
    pll_reset(SI5351_PLLA);
}
