#include "gps.h"

#include <stddef.h>
#include <string.h>

#include "log.h"

char gps_buffer[GPS_CHUNKS][GPS_CHUNK_SIZE];
size_t gps_buffer_head;
size_t gps_buffer_head_pos;
size_t gps_buffer_tail;

void gps_init() {
    log_info("Initialization");

    memset(gps_buffer, '\0', sizeof(gps_buffer));
    gps_buffer_head = 0;
    gps_buffer_head_pos = 0;
    gps_buffer_tail = 0;
}

const char *gps_head_get() {
    return gps_buffer[gps_buffer_head];
}

void gps_head_put(const char c) {
    gps_buffer[gps_buffer_head][gps_buffer_head_pos] = c;
    gps_buffer_head_pos += 1;
    if (gps_buffer_head_pos >= 1024) {
        gps_buffer_head_pos = 0;
        memset(gps_buffer[gps_buffer_head], '\0', 1024);
    }
}

void gps_head_forward() {
    gps_buffer_head += 1;
    if (gps_buffer_head >= 32)
        gps_buffer_head = 0;

    gps_buffer_head_pos = 0;
}

const char *gps_tail_get() {
    return gps_buffer[gps_buffer_tail];
}

void gps_tail_forward() {
    memset(gps_buffer[gps_buffer_tail], '\0', 1024);
    gps_buffer_tail += 1;
    if (gps_buffer_tail >= 32)
        gps_buffer_tail = 0;
}
