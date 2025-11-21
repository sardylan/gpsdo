#ifndef GPSDO__PID__PID__H
#define GPSDO__PID__PID__H

#include "context.h"
#include <stdint.h>

uint64_t pid_compute(pid_context_t *ctx, uint64_t current_value);

#endif
