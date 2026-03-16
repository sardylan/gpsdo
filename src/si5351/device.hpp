/*
 * si5351.h - Si5351 library for Arduino
 *
 * Copyright (C) 2015 - 2019 Jason Milldrum <milldrum@gmail.com>
 *                           Dana H. Myers <k6jq@comcast.net>
 *                      2026 Luca Cireddu <sardylan@gmail.com>
 *
 * Many defines derived from clk-si5351.h in the Linux kernel.
 * Sebastian Hesselbarth <sebastian.hesselbarth@gmail.com>
 * Rabeeh Khoury <rabeeh@solid-run.com>
 *
 * do_div() macro derived from /include/asm-generic/div64.h in
 * the Linux kernel.
 * Copyright (C) 2003 Bernardo Innocenti <bernie@develer.com>
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

#ifndef __SI5351__DEVICE__H
#define __SI5351__DEVICE__H

#include <array>
#include <cstdint>

#include "enums.hpp"
#include "structs.hpp"
#include "const.hpp"

namespace gpsdo::si5351 {
    class AbstractSi5351 {
    public:
        explicit AbstractSi5351(uint8_t i2cAddr = SI5351_BUS_BASE_ADDR);

        virtual ~AbstractSi5351() = default;

        void init(uint8_t xtalLoadCap, uint32_t xtalFreq, int32_t correction);

        void reset();

        uint8_t setFrequency(Clock clk, uint64_t freq);

        void setFrequencyManual(Clock clk, uint64_t freq, uint64_t pllFreq);

        void setPLL(PLL pll, uint64_t pllFreq);

        void setMultiSynth(Clock clk, const RegSet &regSet, uint8_t intMode, uint8_t rDiv, uint8_t divBy4);

        void setOutputEnabled(Clock clk, bool enable);

        void setDriveStrength(Clock clk, Drive drive);

        void updateStatus();

        void setCorrection(PLLInput pllInput, int32_t correction);

        void setPhase(Clock clk, uint8_t phase);

        [[nodiscard]] int32_t getCorrection(PLLInput pllInput) const;

        void pllReset(PLL pll);

        void setMultiSynthSource(Clock clk, PLL pll);

        void setMultiSynthIntegerMode(Clock clk, uint8_t enable);

        void setClockPower(Clock clk, bool pwr);

        void setClockInvert(Clock clk, bool inv);

        void setClockSource(Clock clk, ClockSource src);

        void setClockDisable(Clock clk, ClockDisable disable);

        void setClockFanout(ClockFanout fanout, uint8_t enable);

        void setPllInput(PLL pll, PLLInput input);

        void setVxco(uint64_t frequency, uint8_t ppm);

        void setReferenceFrequency(PLLInput pllInput, uint32_t referenceFrequency);

    protected:
        const uint8_t i2cAddress;

        virtual void write(uint8_t addr, uint8_t bytes, uint8_t *data) = 0;

        virtual uint8_t read(uint8_t addr) = 0;

    private:
        void writeSingle(uint8_t addr, uint8_t data);

        uint64_t pllCalc(RegSet *reg, PLL pll, uint64_t freq, int32_t correction, uint8_t vcxo) const;

        void updateSystemStatus(Status *status);

        void updateIntStatus(IntStatus *intStatus);

        void multySynthDiv(Clock clk, uint8_t rDiv, uint8_t divBy4);

        static uint64_t multiSynthCalc(RegSet *reg, uint64_t freq, uint64_t frequency);

        static uint64_t multiSynth67Calc(RegSet *reg, uint64_t freq, uint64_t frequency);

        static uint8_t selectRDiv(uint64_t *frequency);

        static uint8_t selectRDivMultySynth67(uint64_t *frequency);

        Status status = {
            .SYS_INIT = 0,
            .LOL_B = 0,
            .LOL_A = 0,
            .LOS = 0,
            .REVID = 0
        };

        IntStatus intStatus = {
            .SYS_INIT_STKY = 0,
            .LOL_B_STKY = 0,
            .LOL_A_STKY = 0,
            .LOS_STKY = 0
        };

        std::array<PLL, 8> pllAssignment{};

        std::array<uint64_t, 8> clkFreq{};

        uint64_t pllaFreq{};

        uint64_t pllbFreq{};

        PLLInput pllaRefOsc{};

        PLLInput pllbRefOsc{};

        std::array<uint32_t, 2> xtalFrequency{};

        std::array<int32_t, 2> refCorrection{};

        uint8_t clkinDiv{};

        std::array<bool, 8> clkFirstSet{};
    };
}

#endif
