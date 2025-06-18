#include "context.h"

#include <hardware/gpio.h>

#include "pico/time.h"

bool lcd_context_init(lcd_context *ctx,
                      const uint pin_rs,
                      const uint pin_enable,
                      const uint pin_d4,
                      const uint pin_d5,
                      const uint pin_d6,
                      const uint pin_d7) {
    if (ctx == nullptr)
        return false;

    ctx->pin_rs = pin_rs;
    ctx->pin_enable = pin_enable;
    ctx->pin_d4 = pin_d4;
    ctx->pin_d5 = pin_d5;
    ctx->pin_d6 = pin_d6;
    ctx->pin_d7 = pin_d7;

    gpio_init(ctx->pin_rs);
    gpio_init(ctx->pin_enable);
    gpio_init(ctx->pin_d4);
    gpio_init(ctx->pin_d5);
    gpio_init(ctx->pin_d6);
    gpio_init(ctx->pin_d7);

    gpio_set_dir(ctx->pin_rs, GPIO_OUT);
    gpio_set_dir(ctx->pin_enable, GPIO_OUT);
    gpio_set_dir(ctx->pin_d4, GPIO_OUT);
    gpio_set_dir(ctx->pin_d5, GPIO_OUT);
    gpio_set_dir(ctx->pin_d6, GPIO_OUT);
    gpio_set_dir(ctx->pin_d7, GPIO_OUT);

    gpio_put(ctx->pin_rs, false);
    gpio_put(ctx->pin_enable, false);
    gpio_put(ctx->pin_d4, false);
    gpio_put(ctx->pin_d5, false);
    gpio_put(ctx->pin_d6, false);
    gpio_put(ctx->pin_d7, false);

    sleep_ms(100);

    return true;
}
