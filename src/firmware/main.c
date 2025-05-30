#include "main.h"

#include <stdlib.h>
#include <pico/multicore.h>

#include "gps.h"
#include "led.h"
#include "log.h"
#include "timertc.h"
#include "ui.h"

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

    if (gps_event_get(GPS_EVENT_PPS)) {
        gps_event_reset(GPS_EVENT_PPS);

        // const int delta = (int) counter_delta();
        // const unsigned int module = abs(delta);
        // log_text("CLOCK: current %llu - delta %d", counter_value(), delta);
        //
        // int64_t shift = oscillator_get_shift(OSCILLATOR_CLK_2);
        //
        // if (module >= 100000)
        //     shift += delta;
        // else if (delta > 0)
        //     shift += min(500, module);
        // else if (delta < 0)
        //     shift -= min(500, module);
        //
        // oscillator_set_shift(shift);

        led_blink(LED_GPS_PPS);
    }

    if (gps_event_get(GPS_EVENT_NMEA)) {
        gps_event_reset(GPS_EVENT_NMEA);

        const char *sentence = gps_tail_get();
        log_text("GPS: %s", sentence);
        gps_sentence_parse(sentence);

        gps_tail_forward();
        led_blink(LED_GPS_DATA);
    }

    led_blink_check();

    // sleep_ms(333);
}
