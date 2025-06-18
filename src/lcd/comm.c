#include "comm.h"

#include <hardware/gpio.h>

#include "pico/time.h"

bool lcd_comm_reset(const lcd_context *ctx) {
    if (ctx == nullptr)
        return false;

    gpio_put(ctx->pin_rs, false);
    gpio_put(ctx->pin_enable, false);

    lcd_comm_send_command(ctx, 0x03);
    sleep_ms(20);
    lcd_comm_send_command(ctx, 0x03);
    sleep_ms(20);
    lcd_comm_send_command(ctx, 0x03);
    sleep_ms(20);

    lcd_comm_send_command(ctx, 0x02);
    sleep_ms(5);
    lcd_comm_send_command(ctx, 0x28);
    sleep_ms(5);
    lcd_comm_send_command(ctx, 0x0c);
    sleep_ms(5);

    lcd_comm_send_command(ctx, 0x01);
    sleep_ms(50);

    return true;
}

bool lcd_comm_send_byte(const lcd_context *ctx, const uint8_t byte) {
    if (ctx == nullptr)
        return false;

    gpio_put(ctx->pin_d4, (byte & 0b00010000) != 0);
    gpio_put(ctx->pin_d5, (byte & 0b00100000) != 0);
    gpio_put(ctx->pin_d6, (byte & 0b01000000) != 0);
    gpio_put(ctx->pin_d7, (byte & 0b10000000) != 0);

    gpio_put(ctx->pin_enable, true);
    sleep_us(40);
    gpio_put(ctx->pin_enable, false);
    sleep_us(40);

    gpio_put(ctx->pin_d4, (byte & 0b00000001) != 0);
    gpio_put(ctx->pin_d5, (byte & 0b00000010) != 0);
    gpio_put(ctx->pin_d6, (byte & 0b00000100) != 0);
    gpio_put(ctx->pin_d7, (byte & 0b00001000) != 0);

    gpio_put(ctx->pin_enable, true);
    sleep_us(40);
    gpio_put(ctx->pin_enable, false);
    sleep_us(40);

    return true;
}

bool lcd_comm_send_command(const lcd_context *ctx, const uint8_t command) {
    if (ctx == nullptr)
        return false;

    gpio_put(ctx->pin_rs, false);
    return lcd_comm_send_byte(ctx, command);
}

bool lcd_comm_send_data(const lcd_context *ctx, const uint8_t command) {
    if (ctx == nullptr)
        return false;

    gpio_put(ctx->pin_rs, true);
    const bool result = lcd_comm_send_byte(ctx, command);
    gpio_put(ctx->pin_rs, false);

    return result;
}
