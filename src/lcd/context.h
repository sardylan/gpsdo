#ifndef GPSDO__LCD__CONTEXT__H
#define GPSDO__LCD__CONTEXT__H

#include <pico/types.h>

struct lcd_context_t {
    uint pin_rs;
    uint pin_enable;
    uint pin_d4;
    uint pin_d5;
    uint pin_d6;
    uint pin_d7;
};

typedef struct lcd_context_t lcd_context;

bool lcd_context_init(lcd_context *ctx, uint pin_rs, uint pin_enable, uint pin_d4, uint pin_d5, uint pin_d6,
                          uint pin_d7);

#endif
