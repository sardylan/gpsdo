#include "utilities.h"

uint32_t do_div(uint64_t *n, const uint32_t base) {
    const uint32_t remainder = *n % base;
    *n = *n / base;
    return remainder;
}
