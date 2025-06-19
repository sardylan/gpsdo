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
#include "scheduler.h"
#include "display.h"

scheduler_declare(oscillator_correction);
scheduler_declare(display_update);

uint32_t clock_current;
int32_t clock_delta;

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
    display_init();

    sleep_ms(500);

    oscillator_start();

    scheduler_init(oscillator_correction);
    scheduler_init(display_update);
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
}

void job_core1() {
    if (counter_event_get(COUNTER_EVENT_PPS)) {
        counter_event_reset(COUNTER_EVENT_PPS);

        clock_current = counter_get_value();
        display_frequency(clock_current);

        clock_delta = REFERENCE_CLOCK_FREQUENCY - (int32_t) clock_current;
        log_text("CLOCK: clock_current: %u - clock_delta: %d", clock_current, clock_delta);

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

    scheduler_now();

    scheduler_task_ms(oscillator_correction, 250);
    scheduler_task_s(display_update, 1);

    led_blink_check();
}

scheduler_task_header(oscillator_correction) {
    if (clock_delta == 0) {
        log_text("CLOCK: no correction needed");
        return;
    }

    if (clock_delta > 500000 || clock_delta < -500000) {
        log_text("CLOCK: delta %d too large, skipping correction", clock_delta);
        return;
    }

    // if (clock_delta > -10 && clock_delta < 10) {
    // log_text("CLOCK: delta %d too small, skipping correction", clock_delta);
    // return;
    // }

    // const int32_t shift_increment = clock_delta > 0 ? 1 : -1;

    // int32_t shift_increment = clock_delta;
    // if (shift > -10 && shift < 10) {
    // shift_increment = shift >= 0 ? 1 : -1;
    // }

    int32_t shift_increment = 0;

    const int32_t i = clock_delta / 10;
    if (clock_delta < 10 && clock_delta > -10 && (clock_delta > 2 || clock_delta < -2)) {
        shift_increment = clock_delta > 0 ? 1 : -1;
    } else {
        shift_increment = i;
        if (shift_increment > 10) {
            shift_increment = 10;
        } else if (shift_increment < -10) {
            shift_increment = -10;
        }
    }
    clock_delta -= shift_increment;

    if (shift_increment == 0) {
        log_text("CLOCK: within tolerance, skipping correction");
        return;
    }

    const int32_t shift_new = oscillator_get_shift() + shift_increment;
    log_text("CLOCK: shift_new: %d", shift_new);
    oscillator_set_shift(shift_new);
}

scheduler_task_header(display_update) {
    display_frequency(clock_current);
    display_shift(oscillator_get_shift());
}
