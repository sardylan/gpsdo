#ifndef GPSDO__FIRMWARE__LED__H
#define GPSDO__FIRMWARE__LED__H

#include <stdint.h>

#include "log.h"

#ifdef LOG_LED_ENABLED
#define log_led(level, format, ...) log_##level(format, ##__VA_ARGS__)
#else
#define log_led(level, format, ...)
#endif

#define LED_BLINK_PERIOD_US 150000

enum led_t {
    LED_GPS_DATA,
    LED_GPS_FIX,
    LED_GPS_PPS,
    LED_CLOCK_SYNC,
    LED_EMBEDDED
};

typedef enum led_t led;

void led_init();

void led_set_state(led l, bool state);

const char *led_to_string(led l);

void led_blink(led l);

void led_blink_check();

#endif
