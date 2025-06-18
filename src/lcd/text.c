#include "text.h"

#include "comm.h"
#include "pico/time.h"

bool lcd_text_clear(const lcd_context *ctx) {
    if (ctx == nullptr)
        return false;

    if (!lcd_comm_send_command(ctx, 0x01))
        return false;
    if (!lcd_comm_send_command(ctx, 0x02))
        return false;

    sleep_ms(2);

    return true;
}

bool lcd_text_line(const lcd_context *ctx, const uint8_t line, const char *text) {
    if (ctx == nullptr || !text)
        return false;

    switch (line) {
        case 1:
            if (!lcd_comm_send_command(ctx, 0x80))
                return false;
            break;
        case 2:
            if (!lcd_comm_send_command(ctx, 0xc0))
                return false;
            break;
        default:
            return false;
    }

    size_t pos = 0;
    while (text[pos] != '\0' && pos < 16) {
        if (!lcd_comm_send_data(ctx, text[pos]))
            return false;
        pos++;
    }

    return true;
}
