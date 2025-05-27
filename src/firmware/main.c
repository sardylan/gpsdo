#include "main.h"

#include <string.h>

#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <hardware/rtc.h>

#include <si5351/si5351.h>

#include "config.h"
#include "counter.h"
#include "led.h"
#include "log.h"
#include "gps.h"
#include "oscillator.h"

int main() {
    rtc_init();

    log_init();
    counter_init();
    gps_init();
    led_init();
    oscillator_init();

    while (true) {
        if (gps_event_get(GPS_EVENT_PPS)) {
            gps_event_reset(GPS_EVENT_PPS);
            log_text("CLOCK: current %llu - delta %llu", counter_value(), counter_delta());
            led_set_state(LED_GPS_PPS, false);
        }

        if (gps_event_get(GPS_EVENT_NMEA)) {
            gps_event_reset(GPS_EVENT_NMEA);

            const char *sentence = gps_tail_get();
            // log_text("GPS: %s", sentence);
            gps_sentence_parse(sentence);

            gps_tail_forward();
            led_set_state(LED_GPS_DATA, false);
        }

        tight_loop_contents();
    }
}
