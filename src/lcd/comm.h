#ifndef GPSDO__LCD__COMM__H
#define GPSDO__LCD__COMM__H

#include "context.h"

bool lcd_comm_reset(const lcd_context *ctx);

bool lcd_comm_send_byte(const lcd_context *ctx, uint8_t byte);

bool lcd_comm_send_command(const lcd_context *ctx, uint8_t command);

bool lcd_comm_send_data(const lcd_context *ctx, uint8_t command);

#endif
