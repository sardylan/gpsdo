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

#include "picosdk.hpp"

#include <cstring>
#include <hardware/i2c.h>

using namespace gpsdo::si5351;

Si5351::Si5351(const uint8_t i2c_addr, i2c_inst_t *i2c) : AbstractSi5351(i2c_addr), i2c(i2c) {
}

Si5351::~Si5351() = default;

void Si5351::write(const uint8_t addr, const uint8_t bytes, uint8_t *data) {
    uint8_t msg[256];

    msg[0] = addr;
    std::memcpy(msg + 1, data, bytes);

    i2c_write_blocking(i2c, i2cAddress, msg, bytes + 1, false);
}

uint8_t Si5351::read(const uint8_t addr) {
    i2c_write_blocking(i2c, i2cAddress, &addr, 1, true);

    uint8_t data;
    i2c_read_blocking(i2c, i2cAddress, &data, 1, false);

    return data;
}
