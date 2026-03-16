/*
 * GPS Dominated Oscillator for RaspberryPi Pico (RP2040)
 * Copyright (C) 2026  Luca Cireddu <sardylan@gmail.com>
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef __SI5351__PICOSDK__H
#define __SI5351__PICOSDK__H

#include "device.hpp"

#include <hardware/i2c.h>

namespace gpsdo::si5351 {
    class Si5351 : public AbstractSi5351 {
    public:
        explicit Si5351(uint8_t i2c_addr, i2c_inst_t *i2c);

        ~Si5351() override;

    protected:
        void write(uint8_t addr, uint8_t bytes, uint8_t *data) override;

        uint8_t read(uint8_t addr) override;

    private:
        i2c_inst_t *i2c;
    };
}

#endif
