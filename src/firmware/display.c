#include "display.h"

#include <stdio.h>
#include <lcd/context.h>
#include <lcd/comm.h>

#include "config.h"
#include "lcd/text.h"

lcd_context ctx;

void display_init() {
    lcd_context_init(&ctx, LCD_PIN_RS, LCD_PIN_ENABLE, LCD_PIN_DATA_4, LCD_PIN_DATA_5, LCD_PIN_DATA_6, LCD_PIN_DATA_7);
    lcd_comm_reset(&ctx);
}

void display_frequency(const uint32_t frequency) {
    char line[17];
    snprintf(line, 17, "Ref: %8u Hz", frequency);
    lcd_text_line(&ctx, 1, line);
}

void display_shift(const int32_t shift) {
    char line[17];
    snprintf(line, 17, "Shift: %6d Hz", shift);
    lcd_text_line(&ctx, 2, line);
}
