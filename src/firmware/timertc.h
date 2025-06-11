#ifndef GPSDO__FIRMWARE__TIMERTC__H
#define GPSDO__FIRMWARE__TIMERTC__H

#include "pico/types.h"

#define TIMERTC_CLOCK_SYNC_TIMEOUT 60000000

#define timertc_clock_sync_check() (now - timertc_last_set_time >= TIMERTC_CLOCK_SYNC_TIMEOUT)

void timertc_init();

void timertc_check();

void timertc_get_time(datetime_t *dt);

void timertc_set_time(const datetime_t *dt);

#endif
