#include "timertc.h"

#include <pico/mutex.h>
#include <hardware/rtc.h>
#include <hardware/timer.h>

#include "led.h"

mutex_t timertc_mutex;
volatile uint64_t timertc_last_set_time;

void timertc_init() {
    mutex_init(&timertc_mutex);

    rtc_init();

    timertc_last_set_time = 0;
}

void timertc_check() {
    mutex_enter_blocking(&timertc_mutex);

    const uint64_t now = time_us_64();

    if (timertc_clock_sync_check())
        led_set_state(LED_CLOCK_SYNC, false);

    mutex_exit(&timertc_mutex);
}

void timertc_get_time(datetime_t *dt) {
    mutex_enter_blocking(&timertc_mutex);

    rtc_get_datetime(dt);

    mutex_exit(&timertc_mutex);
}

void timertc_set_time(const datetime_t *dt) {
    mutex_enter_blocking(&timertc_mutex);

    const uint64_t now = time_us_64();
    timertc_last_set_time = now;

    rtc_set_datetime(dt);
    sleep_ms(100);

    led_set_state(LED_CLOCK_SYNC, true);

    mutex_exit(&timertc_mutex);
}
