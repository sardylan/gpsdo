#include "counter.h"

#include <hardware/pwm.h>

#include <hardware/gpio.h>

#include "config.h"
#include "log.h"

unsigned int counter_slice_num;
volatile uint64_t overflow_counter;
volatile uint64_t value;

void counter_init() {
    log_counter(info, "Initialization");

    log_counter(debug, "Initializing GPIO pin for frequency counter");
    counter_slice_num = pwm_gpio_to_slice_num(3);
    pwm_config cfg = pwm_get_default_config();
    pwm_config_set_clkdiv_mode(&cfg, PWM_DIV_B_RISING);
    pwm_init(counter_slice_num, &cfg, false);
    gpio_set_function(3, GPIO_FUNC_PWM);

    log_counter(debug, "Initializing overflow coutner");
    overflow_counter = 0;

    log_counter(debug, "Initializing overflow interrupt");
    pwm_clear_irq(counter_slice_num);
    pwm_set_irq_enabled(counter_slice_num, true);
    irq_set_exclusive_handler(PWM_IRQ_WRAP, counter_overflow);
    irq_set_enabled(PWM_IRQ_WRAP, true);
    irq_set_priority(PWM_IRQ_WRAP, 0);

    log_counter(debug, "Resetting counter");
    pwm_set_counter(counter_slice_num, 0);

    log_counter(debug, "Enabling counter");
    pwm_set_enabled(counter_slice_num, true);
}

void counter_overflow() {
    pwm_clear_irq(counter_slice_num);
    overflow_counter++;
}

void counter_read() {
    value = (overflow_counter << 16) + pwm_get_counter(counter_slice_num);
    overflow_counter = 0;
}

uint64_t counter_value() {
    return value;
}

int64_t counter_delta() {
    return REFERENCE_CLOCK_FREQUENCY - value;
}
