#ifndef GPSDO__SCHEDULER__CONTEXT__H
#define GPSDO__SCHEDULER__CONTEXT__H

#include "types.h"

#define MAX_TASKS 10

typedef struct {
    scheduler_task_t tasks[MAX_TASKS];
    uint8_t num_tasks;
} scheduler_ctx_t;

#endif
