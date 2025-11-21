#include "context.h"

void pid_ctx_init(pid_context_t *ctx, const uint64_t target_freq_hHz, const double kp, const double ki, const double kd) {
    ctx->target_value = target_freq_hHz;
    ctx->kp = kp;
    ctx->ki = ki;
    ctx->kd = kd;
    ctx->integral = 0.0;
    ctx->last_error = 0.0;
}
