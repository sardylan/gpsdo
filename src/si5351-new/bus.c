#include "bus.h"

#include <stdlib.h>

#include <hardware/gpio.h>

si5351_exit_type si5351_bus_init(const si5351_ctx *ctx) {
    SI5351_CTX_NULL_CHECK(ctx);

    i2c_init(ctx->bus, ctx->baudrate);
    gpio_set_function(ctx->sda, GPIO_FUNC_I2C);
    gpio_set_function(ctx->scl, GPIO_FUNC_I2C);
    gpio_pull_up(ctx->sda);
    gpio_pull_up(ctx->scl);

    si5351_exit_success();
}
