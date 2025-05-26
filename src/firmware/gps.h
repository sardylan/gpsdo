#ifndef GPS_H
#define GPS_H

#define GPS_CHUNK_SIZE  1024
#define GPS_CHUNKS      32

void gps_init();

const char *gps_head_get();

void gps_head_put(char c);

void gps_head_forward();

const char *gps_tail_get();

void gps_tail_forward();

#endif
