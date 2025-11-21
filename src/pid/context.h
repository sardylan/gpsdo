#ifndef GPSDO__PID__CONTEXT__H
#define GPSDO__PID__CONTEXT__H

#include <stdint.h>

typedef struct pid_context {
    uint64_t target_value;
    double kp;
    double ki;
    double kd;
    double integral;
    double last_error;
} pid_context_t;

void pid_ctx_init(pid_context_t *ctx, uint64_t target_freq_hHz, double kp, double ki, double kd);

#endif
