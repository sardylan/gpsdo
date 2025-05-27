#include "led.h"

#include <hardware/gpio.h>

#include "config.h"
#include "log.h"

void led_init() {
    log_info("Initialization");

    log_debug("Initializing GPIO for GPS data LED");
    gpio_init(LED_PIN_GPS_DATA);
    gpio_set_dir(LED_PIN_GPS_DATA, GPIO_OUT);
    gpio_put(LED_PIN_GPS_DATA, false);

    log_debug("Initializing GPIO for GPS fix LED");
    gpio_init(LED_PIN_GPS_FIX);
    gpio_set_dir(LED_PIN_GPS_FIX, GPIO_OUT);
    gpio_put(LED_PIN_GPS_FIX, false);

    log_debug("Initializing GPIO for GPS PPS LED");
    gpio_init(LED_PIN_GPS_PPS);
    gpio_set_dir(LED_PIN_GPS_PPS, GPIO_OUT);
    gpio_put(LED_PIN_GPS_PPS, false);

    log_debug("Initializing GPIO for clock sync LED");
    gpio_init(LED_PIN_CLOCK_SYNC);
    gpio_set_dir(LED_PIN_CLOCK_SYNC, GPIO_OUT);
    gpio_put(LED_PIN_CLOCK_SYNC, false);
}

void led_set_state(const led l, const bool state) {
    log_debug("Setting LED %s to state %s", led_to_string(l), state ? "ON" : "OFF");

    switch (l) {
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

const char *led_to_string(const led l) {
    switch (l) {
        case LED_GPS_DATA:
            return "GPS data";
        case LED_GPS_FIX:
            return "GPS fix";
        case LED_GPS_PPS:
            return "GPS PPS";
        case LED_CLOCK_SYNC:
            return "clock sync";
        case LED_EMBEDDED:
            return "Embedded";
        default:
            return "Unknown";
    }
}
