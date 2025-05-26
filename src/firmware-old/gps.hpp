#ifndef GPS_H
#define GPS_H

#include <cstdint>
#include <string>
#include <chrono>

enum class GpsFix : uint8_t {
    Invalid = 0,
    SPS = 1,
    Differential = 2,
    DeadReckoning = 6
};

class GpsStatus {
public:
    [[nodiscard]] std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<long long> > getTime() const;

    void setTime(std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<long long> > time);

    [[nodiscard]] bool isValid() const;

    void setValid(bool valid);

    [[nodiscard]] double getLatitude() const;

    void setLatitude(double latitude);

    [[nodiscard]] double getLongitude() const;

    void setLongitude(double longitude);

    [[nodiscard]] GpsFix getGPSFix() const;

    void setGPSFix(GpsFix gps_fix);

    [[nodiscard]] uint8_t getSatellites() const;

    void setSatellites(uint8_t satellites);

    [[nodiscard]] float getHDoP() const;

    void setHDoP(float hdop);

    [[nodiscard]] double getAltitude() const;

    void setAltitude(double altitude);

    [[nodiscard]] double getGeoidSeparation() const;

    void setGeoidSeparation(double geoid_separation);

    [[nodiscard]] float getSpeed() const;

    void setSpeed(float speed);

    [[nodiscard]] float getCourse() const;

    void setCourse(float course);

private:
    std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<long long> > time;
    bool valid = false;

    double latitude = 0;
    double longitude = 0;
    GpsFix gpsFix = GpsFix::Invalid;
    uint8_t satellites = 0;
    float hdop = 0;
    double altitude = 0;
    double geoidSeparation = 0;

    float speed = 0;
    float course = 0;
};

std::ostream &operator<<(std::ostream &os, const GpsStatus &gpsStatus);

void gpsInit();

void gpsRx();

void gpsLoop();

void gpsParseSentence(const std::string &sentence);

int parseInteger(const std::string &value);

float parseFloat(const std::string &value);

double parseDouble(const std::string &value);

double parseCoordinateValue(const std::string &value, const std::string &sign, std::size_t len);

GpsFix parseGpsFix(const std::string &sentence);

#endif
