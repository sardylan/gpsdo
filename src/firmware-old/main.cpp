#include "main.hpp"

#include <iostream>
#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <hardware/uart.h>
#include <hardware/irq.h>
#include <hardware/gpio.h>
#include <hardware/pwm.h>
#include <hardware/clocks.h>
#include <hardware/rtc.h>

#include "config.hpp"
#include "gps.hpp"
#include "oscillator.hpp"
#include "led.hpp"
#include "ui.hpp"

unsigned int counter_slice_num;
uint64_t overflow_counter;

[[noreturn]] int main() {
    init();

    multicore_launch_core1(secondMain);
    firstMain();
}

void init() {
    stdio_usb_init();
    rtc_init();

    uiHeader();
}

[[noreturn]] void firstMain() {
    ledInit();

    counter_slice_num = pwm_gpio_to_slice_num(3);
    pwm_config cfg = pwm_get_default_config();
    pwm_config_set_clkdiv_mode(&cfg, PWM_DIV_B_RISING);
    pwm_init(counter_slice_num, &cfg, false);
    gpio_set_function(3, GPIO_FUNC_PWM);

    overflow_counter = 0;
    pwm_clear_irq(counter_slice_num);
    pwm_set_irq_enabled(counter_slice_num, true);
    irq_set_exclusive_handler(PWM_IRQ_WRAP, counter_overflow);
    irq_set_enabled(PWM_IRQ_WRAP, true);

    pwm_set_counter(counter_slice_num, 0);

    oscillatorInit();

    pwm_set_enabled(counter_slice_num, true);

    while (true) {
        const uint16_t counterValue = pwm_get_counter(counter_slice_num);
        const uint64_t value = (overflow_counter << 16) + counterValue;
        overflow_counter = 0;
        std::cout << "Count: " << value << std::endl;

        loopDelay(1000);
    }
}

[[noreturn]] void secondMain() {
    gpsInit();

    while (true) {
        gpsLoop();

        loopDelay(500);
    }
}

void counter_overflow() {
    pwm_clear_irq(counter_slice_num);
    overflow_counter++;
}
