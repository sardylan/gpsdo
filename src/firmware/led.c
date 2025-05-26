#include "led.h"

#include <hardware/gpio.h>

#include "config.h"
#include "log.h"

void led_init() {
    log_info("Initialization");

    gpio_init(LED_PIN_GPS_DATA);
    gpio_set_dir(LED_PIN_GPS_DATA, GPIO_OUT);
    gpio_put(LED_PIN_GPS_DATA, false);

    gpio_init(LED_PIN_GPS_FIX);
    gpio_set_dir(LED_PIN_GPS_FIX, GPIO_OUT);
    gpio_put(LED_PIN_GPS_FIX, false);

    gpio_init(LED_PIN_GPS_PPS);
    gpio_set_dir(LED_PIN_GPS_PPS, GPIO_OUT);
    gpio_put(LED_PIN_GPS_PPS, false);

    gpio_init(LED_PIN_CLOCK_SYNC);
    gpio_set_dir(LED_PIN_CLOCK_SYNC, GPIO_OUT);
    gpio_put(LED_PIN_CLOCK_SYNC, false);
}

void led_set_state(const enum led_t led, const bool state) {
    switch (led) {
        case LED_GPS_DATA:
            gpio_put(LED_PIN_GPS_DATA, state);
            break;
        case LED_GPS_FIX:
            gpio_put(LED_PIN_GPS_FIX, state);
            break;
        case LED_GPS_PPS:
            gpio_put(LED_PIN_GPS_PPS, state);
            break;
        case LED_CLOCK_SYNC:
            gpio_put(LED_PIN_CLOCK_SYNC, state);
            break;
        case LED_EMBEDDED:
            break;
    }
}
