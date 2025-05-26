#include "led.hpp"
#include "config.hpp"

void ledInit() {
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

void ledStatus(const Led led, const bool on) {
    switch (led) {
        case Led::GPSData:
            gpio_put(LED_PIN_GPS_DATA, on);
            break;

        case Led::GPSFix:
            gpio_put(LED_PIN_GPS_FIX, on);
            break;

        case Led::GPSPPS:
            gpio_put(LED_PIN_GPS_PPS, on);
            break;

        case Led::ClockSync:
            gpio_put(LED_PIN_CLOCK_SYNC, on);
            break;
    }
}
