#ifndef GPSDO__SCHEDULER__SCHEDULER__H
#define GPSDO__SCHEDULER__SCHEDULER__H

#include "context.h"
#include "types.h"

void scheduler_init(scheduler_ctx_t *context);

bool scheduler_add_task(scheduler_ctx_t *context, const char *name, uint64_t interval_us,
                        scheduler_task_func_t function);

void scheduler_run(scheduler_ctx_t *context);

#endif
