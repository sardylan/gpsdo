#include "output.h"

#include "enums.h"
#include "const.h"
#include "context.h"
#include "device.h"

void si5351_output_set(const si5351_ctx *ctx, const si5351_clock clock, const bool enabled) {
    uint8_t val;

    switch (clock) {
        case SI5351_CLOCK_0:
            val = 0b00000001;
            break;
        case SI5351_CLOCK_1:
            val = 0b00000010;
            break;
        case SI5351_CLOCK_2:
            val = 0b00000100;
            break;
        case SI5351_CLOCK_3:
            val = 0b00001000;
            break;
        case SI5351_CLOCK_4:
            val = 0b00010000;
            break;
        case SI5351_CLOCK_5:
            val = 0b00100000;
            break;
        case SI5351_CLOCK_6:
            val = 0b01000000;
            break;
        case SI5351_CLOCK_7:
            val = 0b10000000;
            break;
        default:
            return;
    }

    uint8_t reg_val = si5351_device_read(ctx, SI5351_OUTPUT_ENABLE_CTRL);

    if (enabled)
        reg_val &= ~(1 << val);
    else
        reg_val |= 1 << val;

    si5351_device_write_single(ctx,SI5351_OUTPUT_ENABLE_CTRL, reg_val);
}

void si5351_output_drive_strength(const si5351_ctx *ctx,
                                  const si5351_clock clock,
                                  si5351_drive_strength drive_strength) {
    uint8_t register_address;

    switch (clock) {
        case SI5351_CLOCK_0:
            register_address = SI5351_CLK0_CTRL;
            break;
        case SI5351_CLOCK_1:
            register_address = SI5351_CLK1_CTRL;
            break;
        case SI5351_CLOCK_2:
            register_address = SI5351_CLK2_CTRL;
            break;
        case SI5351_CLOCK_3:
            register_address = SI5351_CLK3_CTRL;
            break;
        case SI5351_CLOCK_4:
            register_address = SI5351_CLK4_CTRL;
            break;
        case SI5351_CLOCK_5:
            register_address = SI5351_CLK5_CTRL;
            break;
        case SI5351_CLOCK_6:
            register_address = SI5351_CLK6_CTRL;
            break;
        case SI5351_CLOCK_7:
            register_address = SI5351_CLK7_CTRL;
            break;
        default:
            return;
    }

    uint8_t reg_val = si5351_device_read(ctx, register_address) & ~0x03;

    switch (drive_strength) {
        case SI5351_DRIVE_STRENGTH_2MA:
            reg_val |= 0x00;
            break;
        case SI5351_DRIVE_STRENGTH_4MA:
            reg_val |= 0x01;
            break;
        case SI5351_DRIVE_STRENGTH_6MA:
            reg_val |= 0x02;
            break;
        case SI5351_DRIVE_STRENGTH_8MA:
            reg_val |= 0x03;
            break;
        default:
            break;
    }

    si5351_device_write_single(ctx, register_address, reg_val);
}
