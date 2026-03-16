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

#include "firmware.hpp"

#include <functional>

#include "logger.hpp"

using namespace gpsdo::firmware;

Firmware::Firmware() : display(DISPLAY_PIN_RS,
                               DISPLAY_PIN_ENABLE,
                               DISPLAY_PIN_D4,
                               DISPLAY_PIN_D5,
                               DISPLAY_PIN_D6,
                               DISPLAY_PIN_D7),
                       ledGpsUart(LED_PIN_GPS_UART),
                       ledGpsValid(LED_PIN_GPS_VALID),
                       ledGpsPps(LED_PIN_GPS_PPS),
                       ledRtcSync(LED_PIN_RTC_SYNC),
                       si5351(SI5351_ADDRESS, SI5351_BUS),
                       displayMessage(0),
                       currentFrequency(SI5351_FREQ_CLK2) {
}

Firmware::~Firmware() = default;

void Firmware::init() {
    logInfo("Firmware init");

    scheduler.addTask([this] { rotateDisplayMessage(); }, 3000000);
    scheduler.addTask([this] { updateSi5351Frequencies(); }, 5000000);

    display.init();
    display.clear();

    timeRTC.init();

    ledGpsUart.init();
    ledGpsValid.init();
    ledGpsPps.init();
    ledRtcSync.init();

    pid.setTarget(SI5351_FREQ_CLK2 * SI5351_FREQ_MULT);
    pid.setKP(0.1);
    pid.setKI(0.1);
    pid.setKD(0.0);

    counterInit();

    si5351.init(SI5351_CRYSTAL_LOAD_8PF, SI5351_XTAL_FREQ, SI5351_XTAL_FREQ_CORRECTION);
    updateSi5351Frequencies();
    si5351.setClockPower(si5351::Clock::SI5351_CLK0, true);
    si5351.setClockPower(si5351::Clock::SI5351_CLK1, true);
    si5351.setClockPower(si5351::Clock::SI5351_CLK2, true);
    si5351.setOutputEnabled(si5351::Clock::SI5351_CLK0, true);
    si5351.setOutputEnabled(si5351::Clock::SI5351_CLK1, true);
    si5351.setOutputEnabled(si5351::Clock::SI5351_CLK2, true);
    si5351.setDriveStrength(si5351::Clock::SI5351_CLK0, si5351::Drive::SI5351_DRIVE_2MA);
    si5351.setDriveStrength(si5351::Clock::SI5351_CLK1, si5351::Drive::SI5351_DRIVE_2MA);
    si5351.setDriveStrength(si5351::Clock::SI5351_CLK2, si5351::Drive::SI5351_DRIVE_2MA);
}

void Firmware::job0() {
    if (takePpsFlag()) {
        const uint32_t newFrequency = counterGetValue();
        //logDebug("PPS event: counter value: %u", newFrequency);
        if (std::abs(static_cast<int64_t>(newFrequency) - SI5351_FREQ_CLK2) >= 50000) {
            logWarn("Error is too big, skipping frequency update");
            return;
        }

        currentFrequency = newFrequency;
        ledGpsPps.trigger();
        display.printf(display::Line::Upper, "F:   %8u Hz", currentFrequency);
    }
}

void Firmware::job1() {
    gps.parseBuffer();

    if (gps.takeFlagStatus()) {
        const bool status = gps.getStatus();
        ledGpsValid.set(status);
        logDebug("GPS status changed: %s", status ? "Valid" : "Invalid");
    }

    if (gps.takeFlagDateTime()) {
        const auto dateTime = gps.getDateTime();
        // logTrace("GPS date/time: %04u-%02u-%02u %02u:%02u:%02u.%03u",
        // dateTime.year, dateTime.month, dateTime.day,
        // dateTime.hour, dateTime.minute, dateTime.second, dateTime.milliseconds);

        const datetime_t rtcDateTime = {
            .year = dateTime.year,
            .month = dateTime.month,
            .day = dateTime.day,
            .dotw = 0,
            .hour = dateTime.hour,
            .min = dateTime.minute,
            .sec = dateTime.second
        };
        timeRTC.setDateTime(rtcDateTime);
    }

    ledGpsUart.check();
    ledGpsPps.check();

    ledRtcSync.set(timeRTC.isValid());

    scheduler.execute();
}

void Firmware::gpsPutChar(const char c) {
    gps.putChar(c);
    ledGpsUart.trigger();
}

void Firmware::rotateDisplayMessage() {
    switch (displayMessage++) {
        case 0: {
            if (timeRTC.isValid()) {
                const auto dateTime = timeRTC.getDateTime();
                display.printf(display::Line::Lower, "Date: %04u-%02u-%02u", dateTime.year, dateTime.month,
                               dateTime.day);
            } else {
                display.printf(display::Line::Lower, "Date: ____-__-__");
            }
        }
        break;
        case 1: {
            if (timeRTC.isValid()) {
                const auto dateTime = timeRTC.getDateTime();
                display.printf(display::Line::Lower, "Time:   %02u:%02u:%02u", dateTime.hour, dateTime.min,
                               dateTime.sec);
            } else {
                display.printf(display::Line::Lower, "Time:   __:__:__");
            }
        }
        break;
        default:
            break;
    }

    if (displayMessage >= 2) {
        displayMessage = 0;
    }
}

void Firmware::updateSi5351Frequencies() {
    const double correction = pid.compute(static_cast<uint64_t>(currentFrequency) * SI5351_FREQ_MULT);
    const uint64_t newFrequency0 = static_cast<uint64_t>(SI5351_FREQ_CLK0) * SI5351_FREQ_MULT
                                   + static_cast<int64_t>(correction * frequencyFactor0);
    const uint64_t newFrequency1 = static_cast<uint64_t>(SI5351_FREQ_CLK1) * SI5351_FREQ_MULT
                                   + static_cast<int64_t>(correction * frequencyFactor1);
    const uint64_t newFrequency2 = static_cast<uint64_t>(SI5351_FREQ_CLK2) * SI5351_FREQ_MULT
                                   + static_cast<int64_t>(correction);
    logTrace("PID correction: %.0f, frequencies: %llu, %llu, %llu", correction, newFrequency0, newFrequency1,
             newFrequency2);
    si5351.setFrequency(si5351::Clock::SI5351_CLK0, newFrequency0);
    si5351.setFrequency(si5351::Clock::SI5351_CLK1, newFrequency1);
    si5351.setFrequency(si5351::Clock::SI5351_CLK2, newFrequency2);
}
