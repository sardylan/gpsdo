#include "timertc.h"

#include <pico/mutex.h>
#include <hardware/rtc.h>
#include <hardware/timer.h>

#include "led.h"

mutex_t timertc_mutex;
uint64_t timertc_last_set_time;

void timertc_init() {
    mutex_init(&timertc_mutex);

    rtc_init();

    timertc_last_set_time = 0;
}

void timertc_check() {
    const uint64_t now = time_us_64();

    if (now - timertc_last_set_time >= 60000000) // 1 minute
        led_set_state(LED_CLOCK_SYNC, false);
}

void timertc_get_time(datetime_t *dt) {
    mutex_enter_blocking(&timertc_mutex);
    rtc_get_datetime(dt);
    mutex_exit(&timertc_mutex);
}

void timertc_set_time(const datetime_t *dt) {
    mutex_enter_blocking(&timertc_mutex);
    rtc_set_datetime(dt);
    mutex_exit(&timertc_mutex);

    timertc_last_set_time = time_us_64();
    led_set_state(LED_CLOCK_SYNC, true);
}
