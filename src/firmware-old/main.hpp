#ifndef MAIN_H
#define MAIN_H

#define loopDelay(x) \
    sleep_ms(x); \
    tight_loop_contents()

void init();

[[noreturn]] void firstMain();

[[noreturn]] void secondMain();

void counter_overflow();

#endif
