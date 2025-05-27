#ifndef MAIN_H
#define MAIN_H

#include <stdint.h>

#include "pico/types.h"

#define loopDelay(x) \
    sleep_ms(x); \
    tight_loop_contents()

#endif
