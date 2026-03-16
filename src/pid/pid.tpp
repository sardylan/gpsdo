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

#ifndef __PID__PID__T
#define __PID__PID__T

#include "pid.hpp"

using namespace gpsdo::pid;

template<typename T>
PID<T>::PID() {
    target = 0;

    kp = 0;
    ki = 0;
    kd = 0;

    integral = 0;
    lastError = 0;
}

template<typename T>
PID<T>::~PID() = default;

template<typename T>
double PID<T>::getTarget() const {
    return this->target;
}

template<typename T>
void PID<T>::setTarget(double newValue) {
    this->target = newValue;
}

template<typename T>
double PID<T>::getKP() const {
    return kp;
}

template<typename T>
void PID<T>::setKP(double newValue) {
    this->kp = newValue;
}

template<typename T>
double PID<T>::getKI() const {
    return ki;
}

template<typename T>
void PID<T>::setKI(double newValue) {
    this->ki = newValue;
}

template<typename T>
double PID<T>::getKD() const {
    return kd;
}

template<typename T>
void PID<T>::setKD(double newValue) {
    this->kd = newValue;
}

template<typename T>
void PID<T>::reset() {
    this->integral = 0;
    this->lastError = 0;
}

template<typename T>
double PID<T>::compute(T input) {
    const double error = static_cast<double>(target) - input;
    integral += error;

    const double derivative = error - lastError;
    lastError = error;

    return kp * error + ki * integral + kd * derivative;
}

#endif
