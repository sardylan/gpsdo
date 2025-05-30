#ifndef GPS_H
#define GPS_H

#include <stdint.h>
#include <pico/types.h>

#include "buildflags.h"

#ifdef LOG_GPS_ENABLED
#define log_gps(level, format, ...) log_##level(format, ##__VA_ARGS__)
#else
#define log_gps(level, format, ...)
#endif

#define GPS_CHUNK_SIZE  256
#define GPS_CHUNKS      32

enum gps_event_t {
    GPS_EVENT_NMEA,
    GPS_EVENT_PPS,
};

typedef enum gps_event_t gps_event;

enum gps_fix_t {
    GPS_FIX_INVALID,
    GPS_FIX_SPS,
    GPS_FIX_DIFFERENTIAL,
    GPS_FIX_DEAD_RECKONING,
};

typedef enum gps_fix_t gps_fix;

void gps_init();

const char *gps_head_get();

void gps_head_put(char c);

void gps_head_forward();

const char *gps_tail_get();

void gps_tail_forward();

bool gps_event_get(gps_event event);

void gps_event_reset(gps_event event);

void gps_rx();

void gps_pps_callback(uint gpio, uint32_t event_mask);

void gps_sentence_parse(const char *s);

const char *gps_fix_to_string(gps_fix fix);

static uint8_t gps_nmea_checksum_compute(const char *s);

bool gps_nmea_checksum_validate(const char *s);

#endif
