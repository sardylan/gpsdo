#include "counter.h"

#include <hardware/pio.h>
#include <hardware/irq.h>
#include <hardware/pwm.h>

#include "log.h"
// #include "counter.pio.h"

volatile bool counter_event_pps;

unsigned int counter_pwm_slice;
volatile uint16_t pwm_counter_value;
volatile uint32_t overflow_counter;
volatile uint32_t counter_value;

void counter_init() {
    log_counter(info, "Initializing");

    log_counter(debug, "Initializing vars");
    counter_event_pps = false;
    overflow_counter = 0;

    log_counter(debug, "Initializing GPIO pin for frequency counter");
    counter_pwm_slice = pwm_gpio_to_slice_num(COUNTER_PIN_CLOCK);
    pwm_config cfg = pwm_get_default_config();
    pwm_config_set_clkdiv_mode(&cfg, PWM_DIV_B_RISING);
    pwm_init(counter_pwm_slice, &cfg, false);
    gpio_set_function(3, GPIO_FUNC_PWM);

    log_counter(debug, "Initializing overflow interrupt");
    pwm_clear_irq(counter_pwm_slice);
    pwm_set_irq_enabled(counter_pwm_slice, true);
    irq_set_exclusive_handler(PWM_IRQ_WRAP, counter_overflow);
    irq_set_enabled(PWM_IRQ_WRAP, true);
    irq_set_priority(PWM_IRQ_WRAP, 0);

    log_counter(debug, "Resetting counter");
    pwm_set_counter(counter_pwm_slice, 0);

    log_counter(debug, "Enabling counter");
    pwm_set_enabled(counter_pwm_slice, true);

    log_counter(debug, "Initializing PPS GPIO");
    gpio_init(COUNTER_PIN_PPS);
    gpio_set_dir(COUNTER_PIN_PPS, GPIO_IN);
    // gpio_pull_down(COUNTER_PIN_PPS);
    gpio_set_irq_enabled_with_callback(COUNTER_PIN_PPS, GPIO_IRQ_EDGE_RISE, true, &counter_pps_irq_handler);
    irq_set_priority(GPIO_IRQ_EDGE_RISE, 1);
}

uint32_t counter_get_value() {
    return counter_value;
}

void counter_pps_irq_handler(uint gpio, uint32_t events) {
    // if (gpio != COUNTER_PIN_PPS)
    //     return;

    // if (!(events & GPIO_IRQ_EDGE_RISE))
    //     return;

    const uint16_t old_counter = pwm_counter_value;
    pwm_counter_value = pwm_get_counter(counter_pwm_slice);
    counter_value = (overflow_counter << 16) + pwm_counter_value - old_counter;

    overflow_counter = 0;

    counter_event_pps = true;
}

bool counter_event_get(const counter_event event) {
    switch (event) {
        case COUNTER_EVENT_PPS:
            return counter_event_pps;
        default:
            return false;
    }
}

void counter_event_reset(const counter_event event) {
    switch (event) {
        case COUNTER_EVENT_PPS:
            counter_event_pps = false;
            break;
        default:
            break;
    }
}

void counter_overflow() {
    pwm_clear_irq(counter_pwm_slice);
    overflow_counter += 1;
}
