#ifndef LED_H
#define LED_H

enum led_t {
    LED_GPS_DATA,
    LED_GPS_FIX,
    LED_GPS_PPS,
    LED_CLOCK_SYNC,
    LED_EMBEDDED
};

void led_init();

void led_set_state(enum led_t led, bool state);

#endif
