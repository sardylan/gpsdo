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

#ifndef __PID__PID__H
#define __PID__PID__H

#include <type_traits>

namespace gpsdo::pid {
    template<typename T>
    class PID {
        static_assert(std::is_arithmetic_v<T>, "PID<T>: T must be a numeric type (integer, float, or double)");

    public:
        PID();

        ~PID();

        [[nodiscard]] double getTarget() const;

        void setTarget(double newValue);

        [[nodiscard]] double getKP() const;

        void setKP(double newValue);

        [[nodiscard]] double getKI() const;

        void setKI(double newValue);

        [[nodiscard]] double getKD() const;

        void setKD(double newValue);

        void reset();

        double compute(T input);

    private:
        T target;

        double kp;
        double ki;
        double kd;

        double integral;
        double lastError;
    };
}

#include "pid.tpp"

#endif
