#include "pid.h"

uint64_t pid_compute(pid_context_t *ctx, const uint64_t current_value) {
    const double error = (double) ctx->target_value - (double) current_value;
    ctx->integral += error;

    const double derivative = error - ctx->last_error;
    ctx->last_error = error;

    const double output = ctx->kp * error + ctx->ki * ctx->integral + ctx->kd * derivative;

    const int64_t output_hHz = (int64_t) output;
    const uint64_t new_value = (uint64_t) ((int64_t) ctx->target_value + output_hHz);

    return new_value;
}
