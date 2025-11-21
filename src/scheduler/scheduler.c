#include "scheduler.h"
#include "context.h"

#include <pico/time.h>

void scheduler_init(scheduler_ctx_t *context) {
    context->num_tasks = 0;
}

bool scheduler_add_task(scheduler_ctx_t *context, const char *name, const uint64_t interval_us,
                        const scheduler_task_func_t function) {
    if (context->num_tasks >= MAX_TASKS) {
        return false;
    }

    context->tasks[context->num_tasks].name = name;
    context->tasks[context->num_tasks].interval_us = interval_us;
    context->tasks[context->num_tasks].last_run_us = 0;
    context->tasks[context->num_tasks].function = function;
    context->num_tasks++;

    return true;
}

void scheduler_run(scheduler_ctx_t *context) {
    const uint64_t now = time_us_64();
    for (uint8_t i = 0; i < context->num_tasks; i++) {
        if (now - context->tasks[i].last_run_us >= context->tasks[i].interval_us) {
            context->tasks[i].last_run_us = now;
            context->tasks[i].function();
        }
    }
}
