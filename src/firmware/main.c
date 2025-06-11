#include "main.h"

#include <stdlib.h>
#include <pico/multicore.h>

#include "gps.h"
#include "led.h"
#include "log.h"
#include "timertc.h"
#include "ui.h"
#include "counter.h"
#include "oscillator.h"

int main() {
    init();

    multicore_launch_core1(program_core1);

    program_core0();

    return EXIT_SUCCESS;
}

void init() {
    log_init();
    ui_header();

    timertc_init();
    led_init();
    gps_init();
    counter_init();
    oscillator_init();

    sleep_ms(500);

    oscillator_start();
}

void program_core0() {
    while (true) {
        job_core0();
        tight_loop_contents();
    }
}

void program_core1() {
    while (true) {
        job_core1();
        tight_loop_contents();
    }
}

void job_core0() {
    // log_text("Core 0");

    // led_blink(LED_GPS_DATA);

    // sleep_ms(5000);
}

void job_core1() {
    // log_text("Core 1");

    if (counter_event_get(COUNTER_EVENT_PPS)) {
        counter_event_reset(COUNTER_EVENT_PPS);

        const uint64_t value = counter_get_value();
        const int delta = (int) (REFERENCE_CLOCK_FREQUENCY - value);
        // const unsigned int module = abs(delta);
        log_text("CLOCK: current %llu - delta %d", value, delta);

        led_blink(LED_GPS_PPS);
    }

    if (gps_event_get(GPS_EVENT_NMEA)) {
        gps_event_reset(GPS_EVENT_NMEA);

        const char *sentence = gps_tail_get();
        // log_text("GPS: %s", sentence);
        gps_sentence_parse(sentence);

        gps_tail_forward();
        led_blink(LED_GPS_DATA);
    }

    led_blink_check();
}
