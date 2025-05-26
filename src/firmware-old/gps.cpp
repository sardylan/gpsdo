#include "gps.hpp"

#include <chrono>
#include <format>
#include <cinttypes>
#include <iostream>
#include <ranges>
#include <sstream>
#include <vector>
#include <pico/stdlib.h>
#include <hardware/uart.h>
#include <hardware/irq.h>

#include "config.hpp"
#include "logger.hpp"
#include "datetime.hpp"
#include "led.hpp"

GpsStatus gpsStatus;

std::stringstream gpsBuffer;
std::string gpsSentence;
bool gpsSentenceAvailable = false;

std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<long long> > GpsStatus::getTime() const {
    return time;
}

void GpsStatus::setTime(
    const std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<long long> > time) {
    this->time = time;
}

bool GpsStatus::isValid() const {
    return valid;
}

void GpsStatus::setValid(const bool valid) {
    this->valid = valid;
}

double GpsStatus::getLatitude() const {
    return latitude;
}

void GpsStatus::setLatitude(const double latitude) {
    this->latitude = latitude;
}

double GpsStatus::getLongitude() const {
    return longitude;
}

void GpsStatus::setLongitude(const double longitude) {
    this->longitude = longitude;
}

GpsFix GpsStatus::getGPSFix() const {
    return gpsFix;
}

void GpsStatus::setGPSFix(const GpsFix gps_fix) {
    gpsFix = gps_fix;
}

uint8_t GpsStatus::getSatellites() const {
    return satellites;
}

void GpsStatus::setSatellites(const uint8_t satellites) {
    this->satellites = satellites;
}

float GpsStatus::getHDoP() const {
    return hdop;
}

void GpsStatus::setHDoP(const float hdop) {
    this->hdop = hdop;
}

double GpsStatus::getAltitude() const {
    return altitude;
}

void GpsStatus::setAltitude(const double altitude) {
    this->altitude = altitude;
}

double GpsStatus::getGeoidSeparation() const {
    return geoidSeparation;
}

void GpsStatus::setGeoidSeparation(const double geoid_separation) {
    geoidSeparation = geoid_separation;
}

float GpsStatus::getSpeed() const {
    return speed;
}

void GpsStatus::setSpeed(const float speed) {
    this->speed = speed;
}

float GpsStatus::getCourse() const {
    return course;
}

void GpsStatus::setCourse(const float course) {
    this->course = course;
}

std::ostream &operator<<(std::ostream &os, const GpsStatus &gpsStatus) {
    return os << "[" << (gpsStatus.isValid() ? "VALID" : "INVALID") << "]"
           << " - Time: " << gpsStatus.getTime()
           << " - Latitude: " << gpsStatus.getLatitude()
           << " - Longitude: " << gpsStatus.getLongitude()
           << " - Altitude: " << gpsStatus.getAltitude()
           << " - Speed: " << gpsStatus.getSpeed()
           << " - Course: " << gpsStatus.getCourse();
}

void gpsInit() {
    logInfo("Initializing GPS");

    irq_set_exclusive_handler(GPS_UART_IRQ, gpsRx);
    irq_set_enabled(GPS_UART_IRQ, true);
    uart_init(GPS_UART, GPS_UART_SPEED);
    gpio_set_function(GPS_UART_PIN_TX, UART_FUNCSEL_NUM(GPS_UART, GPS_UART_PIN_TX));
    gpio_set_function(GPS_UART_PIN_RX, UART_FUNCSEL_NUM(GPS_UART, GPS_UART_PIN_RX));
    uart_set_hw_flow(GPS_UART, false, false);
    uart_set_format(GPS_UART, GPS_UART_DATA_BITS, GPS_UART_STOP_BITS, GPS_UART_PARITY);
    uart_set_fifo_enabled(GPS_UART, false);
    uart_set_irq_enables(GPS_UART, true, false);
}

void gpsRx() {
    ledStatus(Led::GPSData, true);

    while (uart_is_readable(GPS_UART)) {
        const char ch = uart_getc(GPS_UART);

        if (ch == '\n' || ch == '\r') {
            gpsSentence = gpsBuffer.str();
            gpsSentenceAvailable = true;
            gpsBuffer.str("");
            gpsBuffer.clear();
            continue;
        }

        gpsBuffer << ch;
    }

    ledStatus(Led::GPSData, false);
}

void gpsLoop() {
    logDebug("Loop GPS");

    if (gpsSentenceAvailable) {
        gpsSentenceAvailable = false;

        if (!gpsSentence.empty()) {
            gpsParseSentence(gpsSentence);

            if (gpsStatus.isValid())
                datetimeSetRtc(gpsStatus.getTime());
        }
    }

    ledStatus(Led::GPSFix, gpsStatus.isValid());
}

void gpsParseSentence(const std::string &sentence) {
    logDebug("Parsing GPS sentence");
    logTrace(sentence);

    // $GPGGA,134626.086,,,,,0,0,,,M,,M,,*42
    // $GPGSA,A,1,,,,,,,,,,,,,,,*1E
    // $GPGSV,1,1,01,24,,,28*74
    // $GPRMC,134626.086,V,,,,,0.00,0.00,061224,,,N*44
    // $GPVTG,0.00,T,,M,0.00,N,0.00,K,N*32

    // $GPGGA,100336.000,3914.0592,N,00911.5889,E,1,4,2.06,97.0,M,47.7,M,,*65
    // $GPGSA,A,3,15,12,24,13,,,,,,,,,2.28,2.06,0.99*0D
    // $GPGSV,2,1,07,15,63,196,18,24,61,318,30,12,43,233,30,13,40,144,35*77
    // $GPGSV,2,2,07,40,18,114,24,10,12,317,,19,,,16*79
    // $GPRMC,100336.000,A,3914.0592,N,00911.5889,E,0.75,195.97,130125,,,A*68
    // $GPVTG,195.97,T,,M,0.75,N,1.38,K,A*36

    std::vector<std::string> items;
    std::stringstream ss(sentence);
    std::string token;
    while (std::getline(ss, token, ','))
        items.push_back(token);

    if (items[0] == "$GPGGA") {
        gpsStatus.setLatitude(parseCoordinateValue(items[2], items[3], 2));
        gpsStatus.setLongitude(parseCoordinateValue(items[4], items[5], 3));
        gpsStatus.setGPSFix(parseGpsFix(items[6]));
        gpsStatus.setSatellites(parseInteger(items[7]));
        gpsStatus.setHDoP(parseFloat(items[8]));
        gpsStatus.setAltitude(parseDouble(items[9]));
        gpsStatus.setGeoidSeparation(parseDouble(items[11]));
    }

    // if (items[0] == "$GPGSA") {
    // }

    // if (items[0] == "$GPGSV") {
    // }

    if (items[0] == "$GPRMC") {
        const int hour = std::stoi(items[1].substr(0, 2));
        const int minute = std::stoi(items[1].substr(2, 2));
        const int second = std::stoi(items[1].substr(4, 2));
        const int day = std::stoi(items[9].substr(0, 2));
        const int month = std::stoi(items[9].substr(2, 2));
        const int year = std::stoi(items[9].substr(4, 2)) + 2000;

        gpsStatus.setTime(
            std::chrono::sys_days(std::chrono::year{year} / month / day)
            + std::chrono::hours{hour}
            + std::chrono::minutes{minute}
            + std::chrono::seconds{second}
        );

        gpsStatus.setValid(items[2] == "A");

        gpsStatus.setSpeed(parseFloat(items[7]));
        gpsStatus.setCourse(parseFloat(items[8]));
    }

    // if (items[0] == "$GPVTG") {
    // }

    logTrace(gpsStatus);
}

int parseInteger(const std::string &value) {
    if (value.empty())
        return 0;
    return std::stoi(value);
}

float parseFloat(const std::string &value) {
    if (value.empty())
        return 0.0;
    return std::stof(value);
}

double parseDouble(const std::string &value) {
    if (value.empty())
        return 0.0;
    return std::stod(value);
}

double parseCoordinateValue(const std::string &value, const std::string &sign, std::size_t len) {
    if (value.empty())
        return 0.0;
    return (std::stoi(value.substr(0, len)) + 60.0 / std::stod(value.substr(len))) * (
               sign == "S" || sign == "W" ? -1 : 1);
}

GpsFix parseGpsFix(const std::string &sentence) {
    if (sentence == "1")
        return GpsFix::SPS;
    if (sentence == "2")
        return GpsFix::Differential;
    if (sentence == "6")
        return GpsFix::DeadReckoning;

    return GpsFix::Invalid;
}
