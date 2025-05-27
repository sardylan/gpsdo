#ifndef LED_H
#define LED_H

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

#endif
