#ifndef GPSDO__SI5351__ENUMS__H
#define GPSDO__SI5351__ENUMS__H

enum si5351_clock_t {
    SI5351_CLOCK_0,
    SI5351_CLOCK_1,
    SI5351_CLOCK_2,
    SI5351_CLOCK_3,
    SI5351_CLOCK_4,
    SI5351_CLOCK_5,
    SI5351_CLOCK_6,
    SI5351_CLOCK_7,
};

typedef enum si5351_clock_t si5351_clock;

enum si5351_pll_t {
    SI5351_PLL_A,
    SI5351_PLL_B
};

typedef enum si5351_pll_t si5351_pll;

enum si5351_drive_strength_t {
    SI5351_DRIVE_STRENGTH_2MA,
    SI5351_DRIVE_STRENGTH_4MA,
    SI5351_DRIVE_STRENGTH_6MA,
    SI5351_DRIVE_STRENGTH_8MA,
};

typedef enum si5351_drive_strength_t si5351_drive_strength;

#endif
