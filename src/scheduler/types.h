#ifndef GPSDO__SCHEDULER__TYPES_H
#define GPSDO__SCHEDULER__TYPES_H

#include <stdint.h>

typedef void (*scheduler_task_func_t)(void);

typedef struct {
    const char *name;
    uint64_t interval_us;
    uint64_t last_run_us;
    scheduler_task_func_t function;
} scheduler_task_t;

#endif
