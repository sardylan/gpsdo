#ifndef LED_H
#define LED_H

enum class Led {
    GPSData,
    GPSFix,
    GPSPPS,
    ClockSync
};

void ledInit();

void ledStatus(Led led, bool on);

#endif
