#ifndef GPSDO__LCD__TEXT__H
#define GPSDO__LCD__TEXT__H

#include "context.h"

bool lcd_text_clear(const lcd_context_t *ctx);

bool lcd_text_line(const lcd_context_t *ctx, uint8_t line, const char *text);

#endif
