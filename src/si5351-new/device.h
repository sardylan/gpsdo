#ifndef GPSDO__SI5351__DEVICE__H
#define GPSDO__SI5351__DEVICE__H

#include <stdint.h>

#include "context.h"

#define SI5351_BUFFER_SIZE 192

struct si5351_reg_set_t {
    uint32_t p1;
    uint32_t p2;
    uint32_t p3;
};

typedef struct si5351_reg_set_t si5351_reg_set;

si5351_exit_type si5351_device_init(si5351_ctx *ctx);

void si5351_device_reset(si5351_ctx *ctx);

uint8_t si5351_device_read(const si5351_ctx *ctx, uint8_t reg_addr);

void si5351_device_write(const si5351_ctx *ctx, uint8_t reg_addr, const uint8_t *data, size_t data_size);

void si5351_device_write_single(const si5351_ctx *ctx, uint8_t reg_addr, uint8_t val);

#endif
