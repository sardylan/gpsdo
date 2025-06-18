#include "gps.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <hardware/rtc.h>
#include <pico/mutex.h>

#include "log.h"
// #include "counter.h"
#include "led.h"
#include "timertc.h"

char gps_buffer[GPS_CHUNKS][GPS_CHUNK_SIZE];
volatile bool gps_in_sentence;
volatile size_t gps_buffer_head;
volatile size_t gps_buffer_head_pos;
volatile size_t gps_buffer_tail;

volatile bool gps_event_nmea;

double latitude;
double longitude;
gps_fix fix;
unsigned int satellites;
float hdop;
float altitude;
float geoid_separation;

mutex_t gps_mutex;

void gps_init() {
    log_info("Initialization");

    log_gps(debug, "Initializing mutex");
    mutex_init(&gps_mutex);

    log_gps(debug, "Resetting buffers");
    memset(gps_buffer, '\0', sizeof(gps_buffer));
    gps_in_sentence = false;
    gps_buffer_head = 0;
    gps_buffer_head_pos = 0;
    gps_buffer_tail = 0;

    log_gps(debug, "Resetting data");
    latitude = 0;
    longitude = 0;
    fix = GPS_FIX_INVALID;
    satellites = 0;
    hdop = 0;
    altitude = 0;
    geoid_separation = 0;

    log_gps(debug, "Configuring UART interrupt");
    irq_set_exclusive_handler(GPS_UART_IRQ, gps_rx);
    irq_set_enabled(GPS_UART_IRQ, true);
    irq_set_priority(GPS_UART_IRQ, 2);

    log_gps(debug, "Configuring UART port");
    uart_init(GPS_UART, GPS_UART_SPEED);
    gpio_set_function(GPS_UART_PIN_TX, UART_FUNCSEL_NUM(GPS_UART, GPS_UART_PIN_TX));
    gpio_set_function(GPS_UART_PIN_RX, UART_FUNCSEL_NUM(GPS_UART, GPS_UART_PIN_RX));
    uart_set_hw_flow(GPS_UART, false, false);
    uart_set_format(GPS_UART, GPS_UART_DATA_BITS, GPS_UART_STOP_BITS, GPS_UART_PARITY);
    uart_set_fifo_enabled(GPS_UART, true);
    uart_set_irq_enables(GPS_UART, true, false);
}

const char *gps_head_get() {
    mutex_enter_blocking(&gps_mutex);
    const char *string = gps_buffer[gps_buffer_head];
    mutex_exit(&gps_mutex);
    return string;
}

void gps_head_put(const char c) {
    mutex_enter_blocking(&gps_mutex);

    gps_buffer[gps_buffer_head][gps_buffer_head_pos] = c;
    gps_buffer_head_pos += 1;
    if (gps_buffer_head_pos >= GPS_CHUNK_SIZE) {
        gps_buffer_head_pos = 0;
        memset(gps_buffer[gps_buffer_head], '\0', GPS_CHUNK_SIZE);
    }

    mutex_exit(&gps_mutex);
}

void gps_head_forward() {
    mutex_enter_blocking(&gps_mutex);

    gps_buffer_head += 1;
    if (gps_buffer_head >= GPS_CHUNKS)
        gps_buffer_head = 0;

    gps_buffer_head_pos = 0;

    mutex_exit(&gps_mutex);
}

const char *gps_tail_get() {
    mutex_enter_blocking(&gps_mutex);
    const char *string = gps_buffer[gps_buffer_tail];
    mutex_exit(&gps_mutex);
    return string;
}

void gps_tail_forward() {
    mutex_enter_blocking(&gps_mutex);

    memset(gps_buffer[gps_buffer_tail], '\0', GPS_CHUNK_SIZE);
    gps_buffer_tail += 1;
    if (gps_buffer_tail >= GPS_CHUNKS)
        gps_buffer_tail = 0;

    mutex_exit(&gps_mutex);
}

bool gps_event_get(const gps_event event) {
    switch (event) {
        case GPS_EVENT_NMEA:
            return gps_event_nmea;
        default:
            return false;
    }
}

void gps_event_reset(const gps_event event) {
    switch (event) {
        case GPS_EVENT_NMEA:
            gps_event_nmea = false;
            break;
        default:
            break;
    }
}

void gps_rx() {
    while (uart_is_readable(GPS_UART)) {
        const char ch = uart_getc(GPS_UART);

        if (!gps_in_sentence) {
            if (ch == '$') {
                gps_in_sentence = true;
                gps_buffer_head_pos = 0;
                gps_head_put(ch);
            }
            continue;
        }

        if (ch == '\n' || ch == '\r') {
            gps_head_forward();

            gps_buffer_head_pos = 0;
            gps_in_sentence = false;

            gps_event_nmea = true;

            continue;
        }

        gps_head_put(ch);
    }
}

void gps_sentence_parse(const char *s) {
    char sentence[GPS_CHUNK_SIZE];
    char *saveptr;

    log_gps(info, "Parsing NMEA sentence");

    log_gps(debug, "Copying sentence");
    strncpy(sentence, s, GPS_CHUNK_SIZE);

    log_gps(debug, "Checking if string is a valid NMEA sentence");
    if (strncmp(sentence, "$GP", 3) != 0) {
        log_gps(warn, "Invalid NMEA sentence: %s", sentence);
        return;
    }

    if (strncmp(sentence + 3, "GGA", 3) == 0) {
        log_gps(debug, "GGA sentence");

        char buf[4];

        const char *item = strtok_r(sentence + 6, ",", &saveptr);
        size_t item_num = 0;
        while (item != nullptr) {
            log_gps(trace, "Item %zu: %s", item_num, item);
            switch (item_num) {
                case 1: {
                    if (strlen(item) == 0)
                        continue;
                    buf[0] = item[0];
                    buf[1] = item[1];
                    buf[2] = '\0';
                    latitude = strtod(buf, nullptr);
                    latitude += strtod(item + 2, nullptr) / 60.0;
                }
                break;

                case 2: {
                    if (*item == 'S')
                        latitude *= -1;
                }
                break;

                case 3: {
                    if (strlen(item) == 0)
                        continue;
                    buf[0] = item[0];
                    buf[1] = item[1];
                    buf[2] = item[2];
                    buf[3] = '\0';
                    longitude = strtod(buf, nullptr);
                    longitude += strtod(item + 2, nullptr) / 60.0;
                }
                break;

                case 4: {
                    if (*item == 'W')
                        longitude *= -1;
                }
                break;

                case 5: {
                    switch (*item) {
                        case '1':
                            fix = GPS_FIX_SPS;
                            break;
                        case '2':
                            fix = GPS_FIX_DIFFERENTIAL;
                            break;
                        case '6':
                            fix = GPS_FIX_DEAD_RECKONING;
                            break;
                        default:
                            fix = GPS_FIX_INVALID;
                            break;
                    }
                }
                break;

                case 6: {
                    if (strlen(item) == 0)
                        continue;
                    satellites = strtoul(item, nullptr, 10);
                }
                break;

                case 7: {
                    if (strlen(item) == 0)
                        continue;
                    hdop = (float) strtod(item, nullptr);
                }
                break;

                case 8: {
                    if (strlen(item) == 0)
                        continue;
                    altitude = (float) strtod(item, nullptr);
                }
                break;

                case 10: {
                    if (strlen(item) == 0)
                        continue;
                    geoid_separation = (float) strtod(item, nullptr);
                }
                break;

                default:
                    break;
            }

            item = strtok_r(nullptr, ",", &saveptr);
            item_num++;
        }

        log_gps(trace,
                "GGA (Fix: %s - Lat: %.06f - Long: %.06f - Alt: %.01f - Sats: %d - HDoP: %.02f - Geoid separation: %.02f)",
                gps_fix_to_string(fix), latitude, longitude, altitude, satellites, hdop, geoid_separation);

        switch (fix) {
            case GPS_FIX_SPS:
            case GPS_FIX_DIFFERENTIAL:
            case GPS_FIX_DEAD_RECKONING:
                led_set_state(LED_GPS_FIX, true);
                break;

            default:
                led_set_state(LED_GPS_FIX, false);
        }
    }

    if (strncmp(sentence + 3, "RMC", 3) == 0) {
        log_gps(debug, "RMC sentence");

        bool valid = false;

        datetime_t dt;
        dt.year = 0;
        dt.month = 0;
        dt.day = 0;
        dt.hour = 0;
        dt.min = 0;
        dt.sec = 0;

        float speed = 0;
        float course = 0;

        const char *item = strtok_r(sentence + 6, ",", &saveptr);
        size_t item_num = 0;
        while (item != nullptr) {
            log_gps(trace, "Item %zu: %s", item_num, item);

            char buf[4];

            switch (item_num) {
                case 0: {
                    if (strlen(item) == 0)
                        continue;

                    buf[0] = item[0];
                    buf[1] = item[1];
                    buf[2] = '\0';
                    dt.hour = (int8_t) strtoul(buf, nullptr, 10);

                    buf[0] = item[2];
                    buf[1] = item[3];
                    buf[2] = '\0';
                    dt.min = (int8_t) strtoul(buf, nullptr, 10);

                    buf[0] = item[4];
                    buf[1] = item[5];
                    buf[2] = '\0';
                    dt.sec = (int8_t) strtoul(buf, nullptr, 10);
                }
                break;

                case 1: {
                    if (strlen(item) == 0)
                        continue;

                    valid = item[0] == 'A';
                }
                break;

                case 6: {
                    if (strlen(item) == 0)
                        continue;

                    speed = (float) strtod(item, nullptr);;
                }
                break;

                case 7: {
                    if (strlen(item) == 0)
                        continue;

                    course = (float) strtod(item, nullptr);;
                }
                break;

                case 8: {
                    if (strlen(item) == 0)
                        continue;

                    buf[0] = item[0];
                    buf[1] = item[1];
                    buf[2] = '\0';
                    dt.day = (int8_t) strtoul(buf, nullptr, 10);

                    buf[0] = item[2];
                    buf[1] = item[3];
                    buf[2] = '\0';
                    dt.month = (int8_t) strtoul(buf, nullptr, 10);

                    buf[0] = item[4];
                    buf[1] = item[5];
                    buf[2] = '\0';
                    dt.year = (int16_t) (2000 + strtoul(buf, nullptr, 10));
                }
                break;

                default:
                    break;
            }

            item = strtok_r(nullptr, ",", &saveptr);
            item_num++;
        }

        log_gps(trace, "RMC (Valid: %s - Time: %04d-%02d-%02d %02d:%02d:%02d)",
                valid ? "YES" : "NO", dt.year, dt.month, dt.day, dt.hour, dt.min, dt.sec);

        if (valid) {
            log_gps(debug, "Setting RTC clock to %04d-%02d-%02d %02d:%02d:%02d",
                    dt.year, dt.month, dt.day, dt.hour, dt.min, dt.sec);
            timertc_set_time(&dt);

            datetime_t new_dt;
            timertc_get_time(&new_dt);
            log_gps(debug, "RTC read: %04d-%02d-%02d %02d:%02d:%02d",
                    new_dt.year, new_dt.month, new_dt.day, new_dt.hour, new_dt.min, new_dt.sec);
        }
    }
}

const char *gps_fix_to_string(const gps_fix fix) {
    switch (fix) {
        case GPS_FIX_SPS:
            return "SPS";
        case GPS_FIX_DIFFERENTIAL:
            return "Differential";
        case GPS_FIX_DEAD_RECKONING:
            return "Dead Reckoning";
        default:
            return "INVALID";
    }
}

uint8_t gps_nmea_checksum_compute(const char *s) {
    uint8_t checksum = 0;

    while (*s && *s != '*') {
        checksum ^= (uint8_t) (*s);
        s++;
    }

    return checksum;
}

bool gps_nmea_checksum_validate(const char *s) {
    log_info("Validating NMEA checksum");

    if (s == nullptr || *s != '$') {
        log_gps(warn, "Empty or invalid start of NMEA sentence");
        return false;
    }

    const char *asterisk = strchr(s, '*');
    if (!asterisk || (asterisk - s) < 1) {
        log_gps(warn, "No '*' found in NMEA sentence: %s", s);
        return false;
    }

    uint8_t computed_checksum = 0;
    for (const char *p = s + 1; p < asterisk; ++p)
        computed_checksum ^= (uint8_t) *p;

    if (*(asterisk + 1) == '\0' || *(asterisk + 2) == '\0') {
        log_gps(warn, "Incomplete checksum digits in sentence: %s", s);
        return false;
    }

    const char hex[3] = {asterisk[1], asterisk[2], '\0'};
    const uint8_t expected_checksum = (uint8_t) strtoul(hex, nullptr, 16);

    log_gps(trace, "Expected checksum: %02X", expected_checksum);
    log_gps(trace, "Computed checksum: %02X", computed_checksum);

    return computed_checksum == expected_checksum;
}
