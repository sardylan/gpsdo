#ifndef GPSDO__FIRMWARE__TIMERTC__H
#define GPSDO__FIRMWARE__TIMERTC__H

#include "pico/types.h"

void timertc_init();

void timertc_check();

void timertc_get_time(datetime_t *dt);

void timertc_set_time(const datetime_t *dt);

#endif
