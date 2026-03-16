/*
 * si5351.cpp - Si5351 library for Arduino
 *
 * Copyright (C) 2015 - 2019 Jason Milldrum <milldrum@gmail.com>
 *                           Dana H. Myers <k6jq@comcast.net>
 *                      2026 Luca Cireddu <sardylan@gmail.com>
 *
 * Some tuning algorithms derived from clk-si5351.c in the Linux kernel.
 * Sebastian Hesselbarth <sebastian.hesselbarth@gmail.com>
 * Rabeeh Khoury <rabeeh@solid-run.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "si5351.hpp"

#include <cstddef>
#include <cstdint>

#include "enums.hpp"
#include "structs.hpp"
#include "const.hpp"


using namespace gpsdo::si5351;

AbstractSi5351::AbstractSi5351(const uint8_t i2cAddr) : i2cAddress(i2cAddr) {
    xtalFrequency[0] = SI5351_XTAL_FREQ;

    // Start by using XO ref osc as default for each PLL
    pllaRefOsc = PLLInput::SI5351_PLL_INPUT_XO;
    pllbRefOsc = PLLInput::SI5351_PLL_INPUT_XO;
    clkinDiv = SI5351_CLKIN_DIV_1;
}

/*
 * init(uint8_t xtal_load_c, uint32_t ref_osc_freq, int32_t corr)
 *
 * Setup communications to the Si5351 and set the crystal
 * load capacitance.
 *
 * xtal_load_c - Crystal load capacitance. Use the SI5351_CRYSTAL_LOAD_*PF
 * defines in the header file
 * xo_freq - Crystal/reference oscillator frequency in 1 Hz increments.
 * Defaults to 25000000 if a 0 is used here.
 * corr - Frequency correction constant in parts-per-billion
 *
 */
void AbstractSi5351::init(const uint8_t xtalLoadCap, const uint32_t xtalFreq, const int32_t correction) {
    // Wait for SYS_INIT flag to be clear, indicating that device is ready
    uint8_t status_reg = 0;
    do {
        status_reg = read(SI5351_DEVICE_STATUS);
    } while (status_reg >> 7 == 1);

    // Set crystal load capacitance
    writeSingle(SI5351_CRYSTAL_LOAD, (xtalLoadCap & SI5351_CRYSTAL_LOAD_MASK) | 0b00010010);

    // Set up the XO and CLKIN reference frequencies
    if (xtalFreq != 0) {
        setReferenceFrequency(PLLInput::SI5351_PLL_INPUT_XO, xtalFreq);
        setReferenceFrequency(PLLInput::SI5351_PLL_INPUT_CLKIN, xtalFreq); //Also CLKIN
    } else {
        setReferenceFrequency(PLLInput::SI5351_PLL_INPUT_XO, SI5351_XTAL_FREQ);
        setReferenceFrequency(PLLInput::SI5351_PLL_INPUT_CLKIN, SI5351_XTAL_FREQ); //Also CLKIN
    }

    // Set the frequency calibrations for the XO and CLKIN
    setCorrection(PLLInput::SI5351_PLL_INPUT_XO, correction);
    setCorrection(PLLInput::SI5351_PLL_INPUT_CLKIN, correction);

    reset();
}

/*
 * reset(void)
 *
 * Call to reset the Si5351 to the state initialized by the library.
 *
 */
void AbstractSi5351::reset() {
    // Initialize the CLK outputs according to flowchart in datasheet
    // First, turn them off
    writeSingle(16, 0x80);
    writeSingle(17, 0x80);
    writeSingle(18, 0x80);
    writeSingle(19, 0x80);
    writeSingle(20, 0x80);
    writeSingle(21, 0x80);
    writeSingle(22, 0x80);
    writeSingle(23, 0x80);

    // Turn the clocks back on...
    writeSingle(16, 0x0c);
    writeSingle(17, 0x0c);
    writeSingle(18, 0x0c);
    writeSingle(19, 0x0c);
    writeSingle(20, 0x0c);
    writeSingle(21, 0x0c);
    writeSingle(22, 0x0c);
    writeSingle(23, 0x0c);

    // Set PLLA and PLLB to 800 MHz for automatic tuning
    setPLL(PLL::SI5351_PLLA, SI5351_PLL_FIXED);
    setPLL(PLL::SI5351_PLLB, SI5351_PLL_FIXED);

    // Make PLL to CLK assignments for automatic tuning
    pllAssignment[0] = PLL::SI5351_PLLA;
    pllAssignment[1] = PLL::SI5351_PLLA;
    pllAssignment[2] = PLL::SI5351_PLLA;
    pllAssignment[3] = PLL::SI5351_PLLA;
    pllAssignment[4] = PLL::SI5351_PLLA;
    pllAssignment[5] = PLL::SI5351_PLLA;
    pllAssignment[6] = PLL::SI5351_PLLB;
    pllAssignment[7] = PLL::SI5351_PLLB;

    setMultiSynthSource(Clock::SI5351_CLK0, PLL::SI5351_PLLA);
    setMultiSynthSource(Clock::SI5351_CLK1, PLL::SI5351_PLLA);
    setMultiSynthSource(Clock::SI5351_CLK2, PLL::SI5351_PLLA);
    setMultiSynthSource(Clock::SI5351_CLK3, PLL::SI5351_PLLA);
    setMultiSynthSource(Clock::SI5351_CLK4, PLL::SI5351_PLLA);
    setMultiSynthSource(Clock::SI5351_CLK5, PLL::SI5351_PLLA);
    setMultiSynthSource(Clock::SI5351_CLK6, PLL::SI5351_PLLB);
    setMultiSynthSource(Clock::SI5351_CLK7, PLL::SI5351_PLLB);

    // Reset the VCXO param
    writeSingle(SI5351_VXCO_PARAMETERS_LOW, 0);
    writeSingle(SI5351_VXCO_PARAMETERS_MID, 0);
    writeSingle(SI5351_VXCO_PARAMETERS_HIGH, 0);

    // Then reset the PLLs
    pllReset(PLL::SI5351_PLLA);
    pllReset(PLL::SI5351_PLLB);

    // Set initial frequencies
    for (uint8_t i = 0; i < 8; i++) {
        clkFreq[i] = 0;
        setOutputEnabled(static_cast<Clock>(i), false);
        clkFirstSet[i] = false;
    }
}

/*
 * set_freq(uint64_t freq, enum si5351_clock clk)
 *
 * Sets the clock frequency of the specified CLK output.
 * Frequency range of 8 kHz to 150 MHz
 *
 * freq - Output frequency in centi-hertz (Hz * 100)
 * clk - Clock output
 *   (use the si5351_clock enum)
 */
uint8_t AbstractSi5351::setFrequency(const Clock clk, uint64_t freq) {
    RegSet ms_reg{};
    uint64_t pll_freq;
    uint8_t int_mode = 0;
    uint8_t div_by_4 = 0;
    uint8_t r_div = 0;

    // Check which Multisynth is being set
    if (const auto clk_idx = to_index(clk); clk_idx <= to_index(Clock::SI5351_CLK5)) {
        // MS0 through MS5 logic
        // ---------------------

        // Lower bounds check
        if (freq > 0 && freq < SI5351_CLKOUT_MIN_FREQ * SI5351_FREQ_MULT) {
            freq = SI5351_CLKOUT_MIN_FREQ * SI5351_FREQ_MULT;
        }

        // Upper bounds check
        if (freq > SI5351_MULTISYNTH_MAX_FREQ * SI5351_FREQ_MULT) {
            freq = SI5351_MULTISYNTH_MAX_FREQ * SI5351_FREQ_MULT;
        }

        // If requested freq >100 MHz and no other outputs are already >100 MHz,
        // we need to recalculate PLLA and then recalculate all other CLK outputs
        // on same PLL
        if (freq > (SI5351_MULTISYNTH_SHARE_MAX * SI5351_FREQ_MULT)) {
            // Check other clocks on same PLL
            for (std::size_t i = 0; i < 6; i++) {
                if (clkFreq[i] > (SI5351_MULTISYNTH_SHARE_MAX * SI5351_FREQ_MULT)) {
                    if (i != clk_idx && pllAssignment[i] == pllAssignment[clk_idx]) {
                        return 1; // won't set if any other clks already >100 MHz
                    }
                }
            }

            // Enable the output on first set_freq only
            if (!clkFirstSet[clk_idx]) {
                setOutputEnabled(clk, true);
                clkFirstSet[clk_idx] = true;
            }

            // Set the freq in memory
            clkFreq[clk_idx] = freq;

            // Calculate the proper PLL frequency
            pll_freq = multiSynthCalc(&ms_reg, freq, 0);

            // Set PLL
            setPLL(pllAssignment[clk_idx], pll_freq);

            // Recalculate params for other synths on same PLL
            for (std::size_t i = 0; i < 6; i++) {
                if (clkFreq[i] != 0) {
                    if (pllAssignment[i] == pllAssignment[clk_idx]) {
                        RegSet temp_reg{};
                        uint64_t temp_freq;

                        // Select the proper R div value
                        temp_freq = clkFreq[i];
                        r_div = selectRDiv(&temp_freq);

                        multiSynthCalc(&temp_reg, temp_freq, pll_freq);

                        // If freq > 150 MHz, we need to use DIVBY4 and integer mode
                        if (temp_freq >= SI5351_MULTISYNTH_DIVBY4_FREQ * SI5351_FREQ_MULT) {
                            div_by_4 = 1;
                            int_mode = 1;
                        } else {
                            div_by_4 = 0;
                            int_mode = 0;
                        }

                        // Set multisynth registers
                        setMultiSynth(static_cast<Clock>(i), temp_reg, int_mode, r_div, div_by_4);
                    }
                }
            }

            // Reset the PLL
            pllReset(pllAssignment[clk_idx]);
        } else {
            clkFreq[clk_idx] = freq;

            // Enable the output on first set_freq only
            if (!clkFirstSet[clk_idx]) {
                setOutputEnabled(clk, true);
                clkFirstSet[clk_idx] = true;
            }

            // Select the proper R div value
            r_div = selectRDiv(&freq);

            // Calculate the synth parameters
            // If the PLL has not been set yet, compute the optimal PLL frequency
            // and program it before setting the multisynth parameters.
            switch (pllAssignment[clk_idx]) {
                case PLL::SI5351_PLLA:
                    if (pllaFreq == 0) {
                        pll_freq = multiSynthCalc(&ms_reg, freq, 0);
                        setPLL(PLL::SI5351_PLLA, pll_freq);
                    } else {
                        multiSynthCalc(&ms_reg, freq, pllaFreq);
                    }
                    break;
                case PLL::SI5351_PLLB:
                    if (pllbFreq == 0) {
                        pll_freq = multiSynthCalc(&ms_reg, freq, 0);
                        setPLL(PLL::SI5351_PLLB, pll_freq);
                    } else {
                        multiSynthCalc(&ms_reg, freq, pllbFreq);
                    }
                    break;
            }

            // Set multisynth registers
            setMultiSynth(clk, ms_reg, int_mode, r_div, div_by_4);

            // Reset the PLL
            //pll_reset(pll_assignment[clk_idx]);
        }

        return 0;
    } else {
        // MS6 and MS7 logic
        // -----------------

        // Lower bounds check

        if (freq > 0 && freq < SI5351_CLKOUT67_MIN_FREQ * SI5351_FREQ_MULT) {
            freq = SI5351_CLKOUT_MIN_FREQ * SI5351_FREQ_MULT;
        }

        // Upper bounds check
        if (freq >= SI5351_MULTISYNTH_DIVBY4_FREQ * SI5351_FREQ_MULT) {
            freq = SI5351_MULTISYNTH_DIVBY4_FREQ * SI5351_FREQ_MULT - 1;
        }

        // If one of CLK6 or CLK7 is already set when trying to set the other,
        // we have to ensure that it will also have an integer division ratio
        // with the same PLL, otherwise do not set it.
        if (clk == Clock::SI5351_CLK6) {
            if (clkFreq[7] != 0) {
                if (pllbFreq % freq == 0) {
                    if ((pllbFreq / freq) % 2 != 0) {
                        // Not an even divide ratio, no bueno
                        return 1;
                    } else {
                        // Set the freq in memory
                        clkFreq[clk_idx] = freq;

                        // Select the proper R div value
                        r_div = selectRDivMultySynth67(&freq);

                        multiSynth67Calc(&ms_reg, freq, pllbFreq);
                    }
                } else {
                    // Not an integer divide ratio, no good
                    return 1;
                }
            } else {
                // No previous assignment, so set PLLB based on CLK6

                // Set the freq in memory
                clkFreq[clk_idx] = freq;

                // Select the proper R div value
                r_div = selectRDivMultySynth67(&freq);

                pll_freq = multiSynth67Calc(&ms_reg, freq, 0);
                //pllb_freq = pll_freq;
                setPLL(PLL::SI5351_PLLB, pll_freq);
            }
        } else {
            if (clkFreq[6] != 0) {
                if (pllbFreq % freq == 0) {
                    if ((pllbFreq / freq) % 2 != 0) {
                        // Not an even divide ratio, no bueno
                        return 1;
                    } else {
                        // Set the freq in memory
                        clkFreq[clk_idx] = freq;

                        // Select the proper R div value
                        r_div = selectRDivMultySynth67(&freq);

                        multiSynth67Calc(&ms_reg, freq, pllbFreq);
                    }
                } else {
                    // Not an integer divide ratio, no good
                    return 1;
                }
            } else {
                // No previous assignment, so set PLLB based on CLK7

                // Set the freq in memory
                clkFreq[clk_idx] = freq;

                // Select the proper R div value
                r_div = selectRDivMultySynth67(&freq);

                pll_freq = multiSynth67Calc(&ms_reg, freq, 0);
                //pllb_freq = pll_freq;
                setPLL(pllAssignment[clk_idx], pll_freq);
            }
        }

        div_by_4 = 0;
        int_mode = 0;

        // Set multisynth registers (MS must be set before PLL)
        setMultiSynth(clk, ms_reg, int_mode, r_div, div_by_4);

        return 0;
    }
}

/*
 * set_freq_manual(uint64_t freq, uint64_t pll_freq, enum si5351_clock clk)
 *
 * Sets the clock frequency of the specified CLK output using the given PLL
 * frequency. You must ensure that the MS is assigned to the correct PLL and
 * that the PLL is set to the correct frequency before using this method.
 *
 * It is important to note that if you use this method, you will have to
 * track that all settings are sane yourself.
 *
 * freq - Output frequency in centi-hertz (Hz * 100)
 * pll_freq - Frequency of the PLL driving the Multisynth in Hz * 100
 * clk - Clock output
 *   (use the si5351_clock enum)
 */
void AbstractSi5351::setFrequencyManual(const Clock clk, uint64_t freq, const uint64_t pllFreq) {
    RegSet ms_reg{};
    uint8_t int_mode = 0;
    uint8_t div_by_4 = 0;
    const auto clk_idx = to_index(clk);

    // Lower bounds check
    if (freq > 0 && freq < SI5351_CLKOUT_MIN_FREQ * SI5351_FREQ_MULT) {
        freq = SI5351_CLKOUT_MIN_FREQ * SI5351_FREQ_MULT;
    }

    // Upper bounds check
    if (freq > SI5351_CLKOUT_MAX_FREQ * SI5351_FREQ_MULT) {
        freq = SI5351_CLKOUT_MAX_FREQ * SI5351_FREQ_MULT;
    }

    clkFreq[clk_idx] = freq;

    setPLL(pllAssignment[clk_idx], pllFreq);

    // Enable the output
    setOutputEnabled(clk, true);

    // Select the proper R div value
    const uint8_t r_div = selectRDiv(&freq);

    // Calculate the synth parameters
    multiSynthCalc(&ms_reg, freq, pllFreq);

    // If freq > 150 MHz, we need to use DIVBY4 and integer mode
    if (freq >= SI5351_MULTISYNTH_DIVBY4_FREQ * SI5351_FREQ_MULT) {
        div_by_4 = 1;
        int_mode = 1;
    }

    // Set multisynth registers (MS must be set before PLL)
    setMultiSynth(clk, ms_reg, int_mode, r_div, div_by_4);
}

/*
 * set_pll(uint64_t pll_freq, enum si5351_pll target_pll)
 *
 * Set the specified PLL to a specific oscillation frequency
 *
 * pll_freq - Desired PLL frequency in Hz * 100
 * target_pll - Which PLL to set
 *     (use the si5351_pll enum)
 */
void AbstractSi5351::setPLL(const PLL pll, const uint64_t pllFreq) {
    RegSet pll_reg{};

    if (pll == PLL::SI5351_PLLA) {
        pllCalc(&pll_reg, PLL::SI5351_PLLA, pllFreq, refCorrection[to_index(pllaRefOsc)], 0);
    } else {
        pllCalc(&pll_reg, PLL::SI5351_PLLB, pllFreq, refCorrection[to_index(pllbRefOsc)], 0);
    }

    // Derive the register values to write

    // Prepare an array for parameters to be written to
    uint8_t params[20];
    uint8_t i = 0;

    // Registers 26-27
    uint8_t temp = pll_reg.p3 >> 8 & 0xFF;
    params[i++] = temp;

    temp = static_cast<uint8_t>(pll_reg.p3 & 0xFF);
    params[i++] = temp;

    // Register 28
    temp = static_cast<uint8_t>(pll_reg.p1 >> 16 & 0x03);
    params[i++] = temp;

    // Registers 29-30
    temp = static_cast<uint8_t>(pll_reg.p1 >> 8 & 0xFF);
    params[i++] = temp;

    temp = static_cast<uint8_t>(pll_reg.p1 & 0xFF);
    params[i++] = temp;

    // Register 31
    temp = static_cast<uint8_t>(pll_reg.p3 >> 12 & 0xF0);
    temp += static_cast<uint8_t>(pll_reg.p2 >> 16 & 0x0F);
    params[i++] = temp;

    // Registers 32-33
    temp = static_cast<uint8_t>(pll_reg.p2 >> 8 & 0xFF);
    params[i++] = temp;

    temp = static_cast<uint8_t>(pll_reg.p2 & 0xFF);
    params[i++] = temp;

    // Write the parameters
    if (pll == PLL::SI5351_PLLA) {
        write(SI5351_PLLA_PARAMETERS, i, params);
        pllaFreq = pllFreq;
    } else if (pll == PLL::SI5351_PLLB) {
        write(SI5351_PLLB_PARAMETERS, i, params);
        pllbFreq = pllFreq;
    }
}

/*
 * set_ms(enum si5351_clock clk, struct Si5351RegSet ms_reg, uint8_t int_mode, uint8_t r_div, uint8_t div_by_4)
 *
 * Set the specified multisynth parameters. Not normally needed, but public for advanced users.
 *
 * clk - Clock output
 *   (use the si5351_clock enum)
 * int_mode - Set integer mode
 *  Set to 1 to enable, 0 to disable
 * r_div - Desired r_div ratio
 * div_by_4 - Set Divide By 4 mode
 *   Set to 1 to enable, 0 to disable
 */
void AbstractSi5351::setMultiSynth(Clock clk, const RegSet &regSet, const uint8_t intMode, const uint8_t rDiv,
                                   const uint8_t divBy4) {
    uint8_t params[20];
    uint8_t i = 0;
    uint8_t temp;

    if (const auto clk_idx = to_index(clk); clk_idx <= to_index(Clock::SI5351_CLK5)) {
        // Registers 42-43 for CLK0
        temp = static_cast<uint8_t>(regSet.p3 >> 8 & 0xFF);
        params[i++] = temp;

        temp = static_cast<uint8_t>(regSet.p3 & 0xFF);
        params[i++] = temp;

        // Register 44 for CLK0
        uint8_t reg_val = read((SI5351_CLK0_PARAMETERS + 2) + (clk_idx * 8));
        reg_val &= ~(0x03);
        temp = reg_val | static_cast<uint8_t>(regSet.p1 >> 16 & 0x03);
        params[i++] = temp;

        // Registers 45-46 for CLK0
        temp = static_cast<uint8_t>(regSet.p1 >> 8 & 0xFF);
        params[i++] = temp;

        temp = static_cast<uint8_t>(regSet.p1 & 0xFF);
        params[i++] = temp;

        // Register 47 for CLK0
        temp = static_cast<uint8_t>(regSet.p3 >> 12 & 0xF0);
        temp += static_cast<uint8_t>(regSet.p2 >> 16 & 0x0F);
        params[i++] = temp;

        // Registers 48-49 for CLK0
        temp = static_cast<uint8_t>(regSet.p2 >> 8 & 0xFF);
        params[i++] = temp;

        temp = static_cast<uint8_t>(regSet.p2 & 0xFF);
        params[i++] = temp;
    } else {
        // MS6 and MS7 only use one register
        temp = regSet.p1;
    }

    // Write the parameters
    switch (clk) {
        case Clock::SI5351_CLK0:
            write(SI5351_CLK0_PARAMETERS, i, params);
            setMultiSynthIntegerMode(clk, intMode);
            multySynthDiv(clk, rDiv, divBy4);
            break;
        case Clock::SI5351_CLK1:
            write(SI5351_CLK1_PARAMETERS, i, params);
            setMultiSynthIntegerMode(clk, intMode);
            multySynthDiv(clk, rDiv, divBy4);
            break;
        case Clock::SI5351_CLK2:
            write(SI5351_CLK2_PARAMETERS, i, params);
            setMultiSynthIntegerMode(clk, intMode);
            multySynthDiv(clk, rDiv, divBy4);
            break;
        case Clock::SI5351_CLK3:
            write(SI5351_CLK3_PARAMETERS, i, params);
            setMultiSynthIntegerMode(clk, intMode);
            multySynthDiv(clk, rDiv, divBy4);
            break;
        case Clock::SI5351_CLK4:
            write(SI5351_CLK4_PARAMETERS, i, params);
            setMultiSynthIntegerMode(clk, intMode);
            multySynthDiv(clk, rDiv, divBy4);
            break;
        case Clock::SI5351_CLK5:
            write(SI5351_CLK5_PARAMETERS, i, params);
            setMultiSynthIntegerMode(clk, intMode);
            multySynthDiv(clk, rDiv, divBy4);
            break;
        case Clock::SI5351_CLK6:
            writeSingle(SI5351_CLK6_PARAMETERS, temp);
            multySynthDiv(clk, rDiv, divBy4);
            break;
        case Clock::SI5351_CLK7:
            writeSingle(SI5351_CLK7_PARAMETERS, temp);
            multySynthDiv(clk, rDiv, divBy4);
            break;
    }
}

/*
 * output_enable(enum si5351_clock clk, uint8_t enable)
 *
 * Enable or disable a chosen output
 * clk - Clock output
 *   (use the si5351_clock enum)
 * enable - Set to 1 to enable, 0 to disable
 */
void AbstractSi5351::setOutputEnabled(const Clock clk, const bool enable) {
    const auto clk_idx = to_index(clk);

    uint8_t reg_val = read(SI5351_OUTPUT_ENABLE_CTRL);

    if (enable) {
        reg_val &= ~(1 << clk_idx);
    } else {
        reg_val |= (1 << clk_idx);
    }

    writeSingle(SI5351_OUTPUT_ENABLE_CTRL, reg_val);
}

/*
 * drive_strength(enum si5351_clock clk, enum si5351_drive drive)
 *
 * Sets the drive strength of the specified clock output
 *
 * clk - Clock output
 *   (use the si5351_clock enum)
 * drive - Desired drive level
 *   (use the si5351_drive enum)
 */
void AbstractSi5351::setDriveStrength(const Clock clk, const Drive drive) {
    constexpr uint8_t mask = 0x03;
    const auto clk_idx = to_index(clk);

    uint8_t reg_val = read(SI5351_CLK0_CTRL + clk_idx);
    reg_val &= ~mask;

    switch (drive) {
        case Drive::SI5351_DRIVE_2MA:
            reg_val |= 0x00;
            break;
        case Drive::SI5351_DRIVE_4MA:
            reg_val |= 0x01;
            break;
        case Drive::SI5351_DRIVE_6MA:
            reg_val |= 0x02;
            break;
        case Drive::SI5351_DRIVE_8MA:
            reg_val |= 0x03;
            break;
        default:
            break;
    }

    writeSingle(SI5351_CLK0_CTRL + clk_idx, reg_val);
}

/*
 * update_status(void)
 *
 * Call this to update the status structs, then access them
 * via the dev_status and dev_int_status global members.
 *
 * See the header file for the struct definitions. These
 * correspond to the flag names for registers 0 and 1 in
 * the Si5351 datasheet.
 */
void AbstractSi5351::updateStatus() {
    updateSystemStatus(&status);
    updateIntStatus(&intStatus);
}

/*
 * set_correction(int32_t corr, enum si5351_pll_input ref_osc)
 *
 * corr - Correction factor in ppb
 * ref_osc - Desired reference oscillator
 *     (use the si5351_pll_input enum)
 *
 * Use this to set the oscillator correction factor.
 * This value is a signed 32-bit integer of the
 * parts-per-billion value that the actual oscillation
 * frequency deviates from the specified frequency.
 *
 * The frequency calibration is done as a one-time procedure.
 * Any desired test frequency within the normal range of the
 * Si5351 should be set, then the actual output frequency
 * should be measured as accurately as possible. The
 * difference between the measured and specified frequencies
 * should be calculated in Hertz, then multiplied by 10 in
 * order to get the parts-per-billion value.
 *
 * Since the Si5351 itself has an intrinsic 0 PPM error, this
 * correction factor is good across the entire tuning range of
 * the Si5351. Once this calibration is done accurately, it
 * should not have to be done again for the same Si5351 and
 * crystal.
 */
void AbstractSi5351::setCorrection(PLLInput pllInput, const int32_t correction) {
    refCorrection[to_index(pllInput)] = correction;

    // Recalculate and set PLL freqs based on correction value
    setPLL(PLL::SI5351_PLLA, pllaFreq);
    setPLL(PLL::SI5351_PLLB, pllbFreq);
}

/*
 * set_phase(enum si5351_clock clk, uint8_t phase)
 *
 * clk - Clock output
 *   (use the si5351_clock enum)
 * phase - 7-bit phase word
 *   (in units of VCO/4 period)
 *
 * Write the 7-bit phase register. This must be used
 * with a user-set PLL frequency so that the user can
 * calculate the proper tuning word based on the PLL period.
 */
void AbstractSi5351::setPhase(Clock clk, uint8_t phase) {
    // Mask off the upper bit since it is reserved
    phase = phase & 0b01111111;

    writeSingle(SI5351_CLK0_PHASE_OFFSET + to_index(clk), phase);
}

/*
 * get_correction(enum si5351_pll_input ref_osc)
 *
 * ref_osc - Desired reference oscillator
 *     0: crystal oscillator (XO)
 *     1: external clock input (CLKIN)
 *
 * Returns the oscillator correction factor stored
 * in RAM.
 */
int32_t AbstractSi5351::getCorrection(const PLLInput pllInput) const {
    return refCorrection[to_index(pllInput)];
}

/*
 * pll_reset(enum si5351_pll target_pll)
 *
 * target_pll - Which PLL to reset
 *     (use the si5351_pll enum)
 *
 * Apply a reset to the indicated PLL.
 */
void AbstractSi5351::pllReset(const PLL pll) {
    if (pll == PLL::SI5351_PLLA) {
        writeSingle(SI5351_PLL_RESET, SI5351_PLL_RESET_A);
    } else if (pll == PLL::SI5351_PLLB) {
        writeSingle(SI5351_PLL_RESET, SI5351_PLL_RESET_B);
    }
}

/*
 * set_ms_source(enum si5351_clock clk, enum si5351_pll pll)
 *
 * clk - Clock output
 *   (use the si5351_clock enum)
 * pll - Which PLL to use as the source
 *     (use the si5351_pll enum)
 *
 * Set the desired PLL source for a multisynth.
 */
void AbstractSi5351::setMultiSynthSource(const Clock clk, const PLL pll) {
    const auto clk_idx = to_index(clk);

    uint8_t reg_val = read(SI5351_CLK0_CTRL + clk_idx);

    switch (pll) {
        case PLL::SI5351_PLLA:
            reg_val &= ~(SI5351_CLK_PLL_SELECT);
            break;
        case PLL::SI5351_PLLB:
            reg_val |= SI5351_CLK_PLL_SELECT;
            break;
    }

    writeSingle(SI5351_CLK0_CTRL + clk_idx, reg_val);

    pllAssignment[clk_idx] = pll;
}

/*
 * set_int(enum si5351_clock clk, uint8_t int_mode)
 *
 * clk - Clock output
 *   (use the si5351_clock enum)
 * enable - Set to 1 to enable, 0 to disable
 *
 * Set the indicated multisynth into integer mode.
 */
void AbstractSi5351::setMultiSynthIntegerMode(Clock clk, const uint8_t enable) {
    const auto clk_idx = to_index(clk);
    uint8_t reg_val = read(SI5351_CLK0_CTRL + clk_idx);

    if (enable == 1) {
        reg_val |= (SI5351_CLK_INTEGER_MODE);
    } else {
        reg_val &= ~(SI5351_CLK_INTEGER_MODE);
    }

    writeSingle(SI5351_CLK0_CTRL + clk_idx, reg_val);

    // Integer mode indication
    /*
    switch(clk)
    {
    case Clock::SI5351_CLK0:
        clk0_int_mode = enable;
        break;
    case Clock::SI5351_CLK1:
        clk1_int_mode = enable;
        break;
    case Clock::SI5351_CLK2:
        clk2_int_mode = enable;
        break;
    default:
        break;
    }
    */
}

/*
 * set_clock_pwr(enum si5351_clock clk, uint8_t pwr)
 *
 * clk - Clock output
 *   (use the si5351_clock enum)
 * pwr - Set to 1 to enable, 0 to disable
 *
 * Enable or disable power to a clock output (a power
 * saving feature).
 */
void AbstractSi5351::setClockPower(const Clock clk, const bool pwr) {
    const auto clk_idx = to_index(clk);
    uint8_t reg_val = read(SI5351_CLK0_CTRL + clk_idx);

    if (pwr) {
        reg_val &= 0b01111111;
    } else {
        reg_val |= 0b10000000;
    }

    writeSingle(SI5351_CLK0_CTRL + clk_idx, reg_val);
}

/*
 * set_clock_invert(enum si5351_clock clk, uint8_t inv)
 *
 * clk - Clock output
 *   (use the si5351_clock enum)
 * inv - Set to 1 to enable, 0 to disable
 *
 * Enable to invert the clock output waveform.
 */
void AbstractSi5351::setClockInvert(const Clock clk, const bool inv) {
    const auto clk_idx = to_index(clk);
    uint8_t reg_val = read(SI5351_CLK0_CTRL + clk_idx);

    if (inv) {
        reg_val |= SI5351_CLK_INVERT;
    } else {
        reg_val &= ~(SI5351_CLK_INVERT);
    }

    writeSingle(SI5351_CLK0_CTRL + clk_idx, reg_val);
}

/*
 * set_clock_source(enum si5351_clock clk, enum si5351_clock_source src)
 *
 * clk - Clock output
 *   (use the si5351_clock enum)
 * src - Which clock source to use for the multisynth
 *   (use the si5351_clock_source enum)
 *
 * Set the clock source for a multisynth (based on the options
 * presented for Registers 16-23 in the Silicon Labs AN619 document).
 * Choices are XTAL, CLKIN, MS0, or the multisynth associated with
 * the clock output.
 */
void AbstractSi5351::setClockSource(const Clock clk, const ClockSource src) {
    const auto clk_idx = to_index(clk);
    uint8_t reg_val = read(SI5351_CLK0_CTRL + clk_idx);

    // Clear the bits first
    reg_val &= ~(SI5351_CLK_INPUT_MASK);

    switch (src) {
        case ClockSource::SI5351_CLK_SRC_XTAL:
            reg_val |= (SI5351_CLK_INPUT_XTAL);
            break;
        case ClockSource::SI5351_CLK_SRC_CLKIN:
            reg_val |= (SI5351_CLK_INPUT_CLKIN);
            break;
        case ClockSource::SI5351_CLK_SRC_MS0:
            if (clk == Clock::SI5351_CLK0) {
                return;
            }

            reg_val |= (SI5351_CLK_INPUT_MULTISYNTH_0_4);
            break;
        case ClockSource::SI5351_CLK_SRC_MS:
            reg_val |= (SI5351_CLK_INPUT_MULTISYNTH_N);
            break;
        default:
            return;
    }

    writeSingle(SI5351_CLK0_CTRL + clk_idx, reg_val);
}

/*
 * set_clock_disable(enum si5351_clock clk, enum si5351_clock_disable dis_state)
 *
 * clk - Clock output
 *   (use the si5351_clock enum)
 * dis_state - Desired state of the output upon disable
 *   (use the si5351_clock_disable enum)
 *
 * Set the state of the clock output when it is disabled. Per page 27
 * of AN619 (Registers 24 and 25), there are four possible values: low,
 * high, high impedance, and never disabled.
 */
void AbstractSi5351::setClockDisable(const Clock clk, const ClockDisable disable) {
    uint8_t reg;
    const auto clk_idx = to_index(clk);
    const auto dis_idx = to_index(disable);

    if (clk_idx <= to_index(Clock::SI5351_CLK3)) {
        reg = SI5351_CLK3_0_DISABLE_STATE;
    } else if (clk_idx <= to_index(Clock::SI5351_CLK7)) {
        reg = SI5351_CLK7_4_DISABLE_STATE;
    } else return;

    uint8_t reg_val = read(reg);

    if (clk_idx <= to_index(Clock::SI5351_CLK3)) {
        reg_val &= ~(0b11u << (clk_idx * 2));
        reg_val |= (dis_idx << (clk_idx * 2));
    } else if (clk_idx <= to_index(Clock::SI5351_CLK7)) {
        reg_val &= ~(0b11u << ((clk_idx - 4) * 2));
        reg_val |= (dis_idx << ((clk_idx - 4) * 2));
    }

    writeSingle(reg, reg_val);
}

/*
 * set_clock_fanout(enum si5351_clock_fanout fanout, uint8_t enable)
 *
 * fanout - Desired clock fanout
 *   (use the si5351_clock_fanout enum)
 * enable - Set to 1 to enable, 0 to disable
 *
 * Use this function to enable or disable the clock fanout options
 * for individual clock outputs. If you intend to output the XO or
 * CLKIN on the clock outputs, enable this first.
 *
 * By default, only the Multisynth fanout is enabled at startup.
 */
void AbstractSi5351::setClockFanout(const ClockFanout fanout, const uint8_t enable) {
    uint8_t reg_val = read(SI5351_FANOUT_ENABLE);

    switch (fanout) {
        case ClockFanout::SI5351_FANOUT_CLKIN:
            if (enable) {
                reg_val |= SI5351_CLKIN_ENABLE;
            } else {
                reg_val &= ~(SI5351_CLKIN_ENABLE);
            }
            break;
        case ClockFanout::SI5351_FANOUT_XO:
            if (enable) {
                reg_val |= SI5351_XTAL_ENABLE;
            } else {
                reg_val &= ~(SI5351_XTAL_ENABLE);
            }
            break;
        case ClockFanout::SI5351_FANOUT_MS:
            if (enable) {
                reg_val |= SI5351_MULTISYNTH_ENABLE;
            } else {
                reg_val &= ~(SI5351_MULTISYNTH_ENABLE);
            }
            break;
    }

    writeSingle(SI5351_FANOUT_ENABLE, reg_val);
}

/*
 * set_pll_input(enum si5351_pll pll, enum si5351_pll_input input)
 *
 * pll - Which PLL to use as the source
 *     (use the si5351_pll enum)
 * input - Which reference oscillator to use as PLL input
 *     (use the si5351_pll_input enum)
 *
 * Set the desired reference oscillator source for the given PLL.
 */
void AbstractSi5351::setPllInput(const PLL pll, const PLLInput input) {
    uint8_t reg_val = read(SI5351_PLL_INPUT_SOURCE);

    // Clear the bits first
    //reg_val &= ~(Clock::SI5351_CLKIN_DIV_MASK);

    switch (pll) {
        case PLL::SI5351_PLLA:
            if (input == PLLInput::SI5351_PLL_INPUT_CLKIN) {
                reg_val |= SI5351_PLLA_SOURCE;
                reg_val |= clkinDiv;
                pllaRefOsc = PLLInput::SI5351_PLL_INPUT_CLKIN;
            } else {
                reg_val &= ~(SI5351_PLLA_SOURCE);
                pllaRefOsc = PLLInput::SI5351_PLL_INPUT_XO;
            }
            break;
        case PLL::SI5351_PLLB:
            if (input == PLLInput::SI5351_PLL_INPUT_CLKIN) {
                reg_val |= SI5351_PLLB_SOURCE;
                reg_val |= clkinDiv;
                pllbRefOsc = PLLInput::SI5351_PLL_INPUT_CLKIN;
            } else {
                reg_val &= ~(SI5351_PLLB_SOURCE);
                pllbRefOsc = PLLInput::SI5351_PLL_INPUT_XO;
            }
            break;
        default:
            return;
    }

    writeSingle(SI5351_PLL_INPUT_SOURCE, reg_val);

    setPLL(PLL::SI5351_PLLA, pllaFreq);
    setPLL(PLL::SI5351_PLLB, pllbFreq);
}

/*
 * set_vcxo(uint64_t pll_freq, uint8_t ppm)
 *
 * pll_freq - Desired PLL base frequency in Hz * 100
 * ppm - VCXO pull limit in ppm
 *
 * Set the parameters for the VCXO on the Si5351B.
 */
void AbstractSi5351::setVxco(const uint64_t frequency, uint8_t ppm) {
    RegSet pll_reg{};

    // Bounds check
    if (ppm < SI5351_VCXO_PULL_MIN) {
        ppm = SI5351_VCXO_PULL_MIN;
    }

    if (ppm > SI5351_VCXO_PULL_MAX) {
        ppm = SI5351_VCXO_PULL_MAX;
    }

    // Set PLLB params
    uint64_t vcxo_param = pllCalc(&pll_reg, PLL::SI5351_PLLB, frequency, refCorrection[to_index(pllbRefOsc)], 1);

    // Derive the register values to write

    // Prepare an array for parameters to be written to
    uint8_t params[20];
    uint8_t i = 0;

    // Registers 26-27
    uint8_t temp = pll_reg.p3 >> 8 & 0xFF;
    params[i++] = temp;

    temp = static_cast<uint8_t>(pll_reg.p3 & 0xFF);
    params[i++] = temp;

    // Register 28
    temp = static_cast<uint8_t>(pll_reg.p1 >> 16 & 0x03);
    params[i++] = temp;

    // Registers 29-30
    temp = static_cast<uint8_t>(pll_reg.p1 >> 8 & 0xFF);
    params[i++] = temp;

    temp = static_cast<uint8_t>(pll_reg.p1 & 0xFF);
    params[i++] = temp;

    // Register 31
    temp = static_cast<uint8_t>(pll_reg.p3 >> 12 & 0xF0);
    temp += static_cast<uint8_t>(pll_reg.p2 >> 16 & 0x0F);
    params[i++] = temp;

    // Registers 32-33
    temp = static_cast<uint8_t>(pll_reg.p2 >> 8 & 0xFF);
    params[i++] = temp;

    temp = static_cast<uint8_t>(pll_reg.p2 & 0xFF);
    params[i++] = temp;

    // Write the parameters
    write(SI5351_PLLB_PARAMETERS, i, params);

    // Write the VCXO parameters
    vcxo_param = vcxo_param * ppm * SI5351_VCXO_MARGIN / 100ULL / 1000000ULL;

    temp = static_cast<uint8_t>(vcxo_param & 0xFF);
    writeSingle(SI5351_VXCO_PARAMETERS_LOW, temp);

    temp = static_cast<uint8_t>(vcxo_param >> 8 & 0xFF);
    writeSingle(SI5351_VXCO_PARAMETERS_MID, temp);

    temp = static_cast<uint8_t>(vcxo_param >> 16 & 0x3F);
    writeSingle(SI5351_VXCO_PARAMETERS_HIGH, temp);
}

/*
 * set_ref_freq(uint32_t ref_freq, enum si5351_pll_input ref_osc)
 *
 * ref_freq - Reference oscillator frequency in Hz
 * ref_osc - Which reference oscillator frequency to set
 *    (use the si5351_pll_input enum)
 *
 * Set the reference frequency value for the desired reference oscillator
 */
void AbstractSi5351::setReferenceFrequency(const PLLInput pllInput, const uint32_t referenceFrequency) {
    // uint8_t reg_val;
    //reg_val = si5351_read(SI5351_PLL_INPUT_SOURCE);

    // Clear the bits first
    //reg_val &= ~(Clock::SI5351_CLKIN_DIV_MASK);

    if (referenceFrequency <= 30000000UL) {
        xtalFrequency[to_index(pllInput)] = referenceFrequency;
        //reg_val |= Clock::SI5351_CLKIN_DIV_1;
        if (pllInput == PLLInput::SI5351_PLL_INPUT_CLKIN) {
            clkinDiv = SI5351_CLKIN_DIV_1;
        }
    } else if (referenceFrequency <= 60000000UL) {
        xtalFrequency[to_index(pllInput)] = referenceFrequency / 2;
        //reg_val |= Clock::SI5351_CLKIN_DIV_2;
        if (pllInput == PLLInput::SI5351_PLL_INPUT_CLKIN) {
            clkinDiv = SI5351_CLKIN_DIV_2;
        }
    } else if (referenceFrequency <= 100000000UL) {
        xtalFrequency[to_index(pllInput)] = referenceFrequency / 4;
        //reg_val |= Clock::SI5351_CLKIN_DIV_4;
        if (pllInput == PLLInput::SI5351_PLL_INPUT_CLKIN) {
            clkinDiv = SI5351_CLKIN_DIV_4;
        }
    } else {
        //reg_val |= Clock::SI5351_CLKIN_DIV_1;
    }

    //si5351_write(SI5351_PLL_INPUT_SOURCE, reg_val);
}

void AbstractSi5351::writeSingle(const uint8_t addr, uint8_t data) {
    write(addr, 1, &data);
}

/*********************/
/* Private functions */
/*********************/

uint64_t AbstractSi5351::pllCalc(RegSet *reg, const PLL pll, uint64_t freq, const int32_t correction,
                                 const uint8_t vcxo) const {
    uint64_t ref_freq = 0;
    switch (pll) {
        case PLL::SI5351_PLLA:
            ref_freq = xtalFrequency[to_index(pllaRefOsc)] * SI5351_FREQ_MULT;
            break;
        case PLL::SI5351_PLLB:
            ref_freq = xtalFrequency[to_index(pllbRefOsc)] * SI5351_FREQ_MULT;
            break;
    }
    uint32_t b, c;

    // Factor calibration value into nominal crystal frequency
    // Measured in parts-per-billion

    ref_freq = ref_freq + static_cast<int32_t>((((static_cast<int64_t>(correction) << 31) / 1000000000LL) * ref_freq) >>
                                               31);

    // PLL bounds checking
    if (freq < SI5351_PLL_VCO_MIN * SI5351_FREQ_MULT) {
        freq = SI5351_PLL_VCO_MIN * SI5351_FREQ_MULT;
    }
    if (freq > SI5351_PLL_VCO_MAX * SI5351_FREQ_MULT) {
        freq = SI5351_PLL_VCO_MAX * SI5351_FREQ_MULT;
    }

    // Determine integer part of feedback equation
    const uint32_t a = freq / ref_freq;

    if (a < SI5351_PLL_A_MIN) {
        freq = ref_freq * SI5351_PLL_A_MIN;
    }
    if (a > SI5351_PLL_A_MAX) {
        freq = ref_freq * SI5351_PLL_A_MAX;
    }

    // Find best approximation for b/c = fVCO mod fIN
    // denom = 1000ULL * 1000ULL;
    // lltmp = freq % ref_freq;
    // lltmp *= denom;
    // do_div(lltmp, ref_freq);

    //b = (((uint64_t)(freq % ref_freq)) * RFRAC_DENOM) / ref_freq;
    if (vcxo) {
        b = freq % ref_freq * 1000000ULL / ref_freq;
        c = 1000000ULL;
    } else {
        b = freq % ref_freq * RFRAC_DENOM / ref_freq;
        c = b ? RFRAC_DENOM : 1;
    }

    // Calculate parameters
    const uint32_t p1 = 128 * a + 128 * b / c - 512;
    const uint32_t p2 = 128 * b - c * (128 * b / c);
    const uint32_t p3 = c;

    // Recalculate frequency as fIN * (a + b/c)
    uint64_t lltmp = ref_freq;
    lltmp *= b;
    do_div(lltmp, c);
    freq = lltmp;
    freq += ref_freq * a;

    reg->p1 = p1;
    reg->p2 = p2;
    reg->p3 = p3;

    if (vcxo) {
        return 128 * a * 1000000ULL + b;
    } else {
        return freq;
    }
}

uint64_t AbstractSi5351::multiSynthCalc(RegSet *reg, uint64_t freq, uint64_t frequency) {
    uint32_t a, b, c, p1, p2, p3;
    uint8_t divby4 = 0;
    uint8_t ret_val = 0;

    // Multisynth bounds checking
    if (freq > SI5351_MULTISYNTH_MAX_FREQ * SI5351_FREQ_MULT) {
        freq = SI5351_MULTISYNTH_MAX_FREQ * SI5351_FREQ_MULT;
    }
    if (freq < SI5351_MULTISYNTH_MIN_FREQ * SI5351_FREQ_MULT) {
        freq = SI5351_MULTISYNTH_MIN_FREQ * SI5351_FREQ_MULT;
    }

    if (freq >= SI5351_MULTISYNTH_DIVBY4_FREQ * SI5351_FREQ_MULT) {
        divby4 = 1;
    }

    if (frequency == 0) {
        // Find largest integer divider for max
        // VCO frequency and given target frequency
        if (divby4 == 0) {
            uint64_t lltmp = SI5351_PLL_VCO_MAX * SI5351_FREQ_MULT; // margin needed?
            do_div(lltmp, freq);
            if (lltmp == 5) {
                lltmp = 4;
            } else if (lltmp == 7) {
                lltmp = 6;
            }
            a = (uint32_t) lltmp;
        } else {
            a = 4;
        }

        b = 0;
        c = 1;
        frequency = a * freq;
    } else {
        // Preset PLL, so return the actual freq for these params instead of PLL freq
        ret_val = 1;

        // Determine integer part of feedback equation
        a = frequency / freq;

        if (a < SI5351_MULTISYNTH_A_MIN) {
            freq = frequency / SI5351_MULTISYNTH_A_MIN;
        }
        if (a > SI5351_MULTISYNTH_A_MAX) {
            freq = frequency / SI5351_MULTISYNTH_A_MAX;
        }

        b = (frequency % freq * RFRAC_DENOM) / freq;
        c = b ? RFRAC_DENOM : 1;
    }

    // Calculate parameters
    if (divby4 == 1) {
        p3 = 1;
        p2 = 0;
        p1 = 0;
    } else {
        p1 = 128 * a + 128 * b / c - 512;
        p2 = 128 * b - c * (128 * b / c);
        p3 = c;
    }

    reg->p1 = p1;
    reg->p2 = p2;
    reg->p3 = p3;

    if (ret_val == 0) {
        return frequency;
    } else {
        return freq;
    }
}

uint64_t AbstractSi5351::multiSynth67Calc(RegSet *reg, uint64_t freq, uint64_t frequency) {
    //uint8_t p1;
    // uint8_t ret_val = 0;
    uint32_t a;

    // Multisynth bounds checking
    if (freq > SI5351_MULTISYNTH67_MAX_FREQ * SI5351_FREQ_MULT) {
        freq = SI5351_MULTISYNTH67_MAX_FREQ * SI5351_FREQ_MULT;
    }
    if (freq < SI5351_MULTISYNTH_MIN_FREQ * SI5351_FREQ_MULT) {
        freq = SI5351_MULTISYNTH_MIN_FREQ * SI5351_FREQ_MULT;
    }

    if (frequency == 0) {
        // Find largest integer divider for max
        // VCO frequency and given target frequency
        uint64_t lltmp = SI5351_PLL_VCO_MAX * SI5351_FREQ_MULT - 100000000UL; // margin needed?
        do_div(lltmp, freq);
        a = static_cast<uint32_t>(lltmp);

        // Divisor has to be even
        if (a % 2 != 0) {
            a++;
        }

        // Divisor bounds check
        if (a < SI5351_MULTISYNTH_A_MIN) {
            a = SI5351_MULTISYNTH_A_MIN;
        }
        if (a > SI5351_MULTISYNTH67_A_MAX) {
            a = SI5351_MULTISYNTH67_A_MAX;
        }

        frequency = a * freq;

        // PLL bounds checking
        if (frequency > (SI5351_PLL_VCO_MAX * SI5351_FREQ_MULT)) {
            a -= 2;
            frequency = a * freq;
        } else if (frequency < (SI5351_PLL_VCO_MIN * SI5351_FREQ_MULT)) {
            a += 2;
            frequency = a * freq;
        }

        reg->p1 = static_cast<uint8_t>(a);
        reg->p2 = 0;
        reg->p3 = 0;
        return frequency;
    } else {
        // Multisynth frequency must be integer division of PLL
        if (frequency % freq) {
            // No good
            return 0;
        } else {
            a = frequency / freq;

            // Division ratio bounds check
            if (a < SI5351_MULTISYNTH_A_MIN || a > SI5351_MULTISYNTH67_A_MAX) {
                // No bueno
                return 0;
            } else {
                reg->p1 = static_cast<uint8_t>(a);
                reg->p2 = 0;
                reg->p3 = 0;
                return 1;
            }
        }
    }
}

void AbstractSi5351::updateSystemStatus(Status *status) {
    uint8_t reg_val = 0;

    reg_val = read(SI5351_DEVICE_STATUS);

    // Parse the register
    status->SYS_INIT = (reg_val >> 7) & 0x01;
    status->LOL_B = (reg_val >> 6) & 0x01;
    status->LOL_A = (reg_val >> 5) & 0x01;
    status->LOS = (reg_val >> 4) & 0x01;
    status->REVID = reg_val & 0x03;
}

void AbstractSi5351::updateIntStatus(IntStatus *intStatus) {
    uint8_t reg_val = 0;

    reg_val = read(SI5351_INTERRUPT_STATUS);

    // Parse the register
    intStatus->SYS_INIT_STKY = (reg_val >> 7) & 0x01;
    intStatus->LOL_B_STKY = (reg_val >> 6) & 0x01;
    intStatus->LOL_A_STKY = (reg_val >> 5) & 0x01;
    intStatus->LOS_STKY = (reg_val >> 4) & 0x01;
}

void AbstractSi5351::multySynthDiv(const Clock clk, const uint8_t rDiv, const uint8_t divBy4) {
    uint8_t reg_val = 0;
    uint8_t reg_addr = 0;
    const auto clk_idx = to_index(clk);

    switch (clk) {
        case Clock::SI5351_CLK0:
            reg_addr = SI5351_CLK0_PARAMETERS + 2;
            break;
        case Clock::SI5351_CLK1:
            reg_addr = SI5351_CLK1_PARAMETERS + 2;
            break;
        case Clock::SI5351_CLK2:
            reg_addr = SI5351_CLK2_PARAMETERS + 2;
            break;
        case Clock::SI5351_CLK3:
            reg_addr = SI5351_CLK3_PARAMETERS + 2;
            break;
        case Clock::SI5351_CLK4:
            reg_addr = SI5351_CLK4_PARAMETERS + 2;
            break;
        case Clock::SI5351_CLK5:
            reg_addr = SI5351_CLK5_PARAMETERS + 2;
            break;
        case Clock::SI5351_CLK6:
        case Clock::SI5351_CLK7:
            reg_addr = SI5351_CLK6_7_OUTPUT_DIVIDER;
            break;
    }

    reg_val = read(reg_addr);

    if (clk_idx <= to_index(Clock::SI5351_CLK5)) {
        // Clear the relevant bits
        reg_val &= ~(0x7c);

        if (divBy4 == 0) {
            reg_val &= ~(SI5351_OUTPUT_CLK_DIVBY4);
        } else {
            reg_val |= (SI5351_OUTPUT_CLK_DIVBY4);
        }

        reg_val |= (rDiv << SI5351_OUTPUT_CLK_DIV_SHIFT);
    } else if (clk == Clock::SI5351_CLK6) {
        // Clear the relevant bits
        reg_val &= ~(0x07);

        reg_val |= rDiv;
    } else if (clk == Clock::SI5351_CLK7) {
        // Clear the relevant bits
        reg_val &= ~(0x70);

        reg_val |= (rDiv << SI5351_OUTPUT_CLK_DIV_SHIFT);
    }

    writeSingle(reg_addr, reg_val);
}

uint8_t AbstractSi5351::selectRDiv(uint64_t *frequency) {
    uint8_t r_div = SI5351_OUTPUT_CLK_DIV_1;

    // Choose the correct R divider
    if ((*frequency >= SI5351_CLKOUT_MIN_FREQ * SI5351_FREQ_MULT) && (
            *frequency < SI5351_CLKOUT_MIN_FREQ * SI5351_FREQ_MULT * 2)) {
        r_div = SI5351_OUTPUT_CLK_DIV_128;
        *frequency *= 128ULL;
    } else if ((*frequency >= SI5351_CLKOUT_MIN_FREQ * SI5351_FREQ_MULT * 2) && (
                   *frequency < SI5351_CLKOUT_MIN_FREQ * SI5351_FREQ_MULT * 4)) {
        r_div = SI5351_OUTPUT_CLK_DIV_64;
        *frequency *= 64ULL;
    } else if ((*frequency >= SI5351_CLKOUT_MIN_FREQ * SI5351_FREQ_MULT * 4) && (
                   *frequency < SI5351_CLKOUT_MIN_FREQ * SI5351_FREQ_MULT * 8)) {
        r_div = SI5351_OUTPUT_CLK_DIV_32;
        *frequency *= 32ULL;
    } else if ((*frequency >= SI5351_CLKOUT_MIN_FREQ * SI5351_FREQ_MULT * 8) && (
                   *frequency < SI5351_CLKOUT_MIN_FREQ * SI5351_FREQ_MULT * 16)) {
        r_div = SI5351_OUTPUT_CLK_DIV_16;
        *frequency *= 16ULL;
    } else if ((*frequency >= SI5351_CLKOUT_MIN_FREQ * SI5351_FREQ_MULT * 16) && (
                   *frequency < SI5351_CLKOUT_MIN_FREQ * SI5351_FREQ_MULT * 32)) {
        r_div = SI5351_OUTPUT_CLK_DIV_8;
        *frequency *= 8ULL;
    } else if ((*frequency >= SI5351_CLKOUT_MIN_FREQ * SI5351_FREQ_MULT * 32) && (
                   *frequency < SI5351_CLKOUT_MIN_FREQ * SI5351_FREQ_MULT * 64)) {
        r_div = SI5351_OUTPUT_CLK_DIV_4;
        *frequency *= 4ULL;
    } else if ((*frequency >= SI5351_CLKOUT_MIN_FREQ * SI5351_FREQ_MULT * 64) && (
                   *frequency < SI5351_CLKOUT_MIN_FREQ * SI5351_FREQ_MULT * 128)) {
        r_div = SI5351_OUTPUT_CLK_DIV_2;
        *frequency *= 2ULL;
    }

    return r_div;
}

uint8_t AbstractSi5351::selectRDivMultySynth67(uint64_t *frequency) {
    uint8_t r_div = SI5351_OUTPUT_CLK_DIV_1;

    // Choose the correct R divider
    if (*frequency >= SI5351_CLKOUT67_MIN_FREQ * SI5351_FREQ_MULT && *frequency < SI5351_CLKOUT67_MIN_FREQ *
        SI5351_FREQ_MULT *
        2) {
        r_div = SI5351_OUTPUT_CLK_DIV_128;
        *frequency *= 128ULL;
    } else if (*frequency >= SI5351_CLKOUT67_MIN_FREQ * SI5351_FREQ_MULT * 2 && *frequency < SI5351_CLKOUT67_MIN_FREQ *
               SI5351_FREQ_MULT * 4) {
        r_div = SI5351_OUTPUT_CLK_DIV_64;
        *frequency *= 64ULL;
    } else if (*frequency >= SI5351_CLKOUT67_MIN_FREQ * SI5351_FREQ_MULT * 4 && *frequency < SI5351_CLKOUT67_MIN_FREQ *
               SI5351_FREQ_MULT * 8) {
        r_div = SI5351_OUTPUT_CLK_DIV_32;
        *frequency *= 32ULL;
    } else if (*frequency >= SI5351_CLKOUT67_MIN_FREQ * SI5351_FREQ_MULT * 8 && *frequency < SI5351_CLKOUT67_MIN_FREQ *
               SI5351_FREQ_MULT * 16) {
        r_div = SI5351_OUTPUT_CLK_DIV_16;
        *frequency *= 16ULL;
    } else if (*frequency >= SI5351_CLKOUT67_MIN_FREQ * SI5351_FREQ_MULT * 16 && *frequency < SI5351_CLKOUT67_MIN_FREQ *
               SI5351_FREQ_MULT * 32) {
        r_div = SI5351_OUTPUT_CLK_DIV_8;
        *frequency *= 8ULL;
    } else if (*frequency >= SI5351_CLKOUT67_MIN_FREQ * SI5351_FREQ_MULT * 32 && *frequency < SI5351_CLKOUT67_MIN_FREQ *
               SI5351_FREQ_MULT * 64) {
        r_div = SI5351_OUTPUT_CLK_DIV_4;
        *frequency *= 4ULL;
    } else if (*frequency >= SI5351_CLKOUT67_MIN_FREQ * SI5351_FREQ_MULT * 64 && *frequency < SI5351_CLKOUT67_MIN_FREQ *
               SI5351_FREQ_MULT * 128) {
        r_div = SI5351_OUTPUT_CLK_DIV_2;
        *frequency *= 2ULL;
    }

    return r_div;
}
