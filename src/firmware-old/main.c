#include "main.h"

#include <string.h>
#include <math.h>

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
    // multicore_launch_core1(program);
    program();

    // while (true) {
        // __wfi(); // Wait for interrupt — low-power idle
    // }
}

void program() {
    rtc_init();

    log_init();
    counter_init();
    gps_init();
    led_init();

    sleep_ms(1000);

    oscillator_init();

    task_init(frequency_correction);

    while (true) {
        task_now();

        // task_schedule(frequency_correction, 250);

        if (gps_event_get(GPS_EVENT_PPS)) {
            gps_event_reset(GPS_EVENT_PPS);

            const int delta = (int) counter_delta();
            const unsigned int module = abs(delta);
            log_text("CLOCK: current %llu - delta %d", counter_value(), delta);

            int64_t shift = oscillator_get_shift(OSCILLATOR_CLK_2);

            if (module >= 100000)
                shift += delta;
            else if (delta > 0)
                shift += min(500, module);
            else if (delta < 0)
                shift -= min(500, module);

            oscillator_set_shift(shift);

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

void task_frequency_correction() {
}
