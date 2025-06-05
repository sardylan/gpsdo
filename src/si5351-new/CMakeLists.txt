add_library(si5351
        bus.h bus.c
        context.h context.c
        device.h device.c
        multisynth.h multisynth.c
        output.h output.c
        pll.h pll.c
        utilities.h utilities.c)

target_link_libraries(si5351
        pico_stdlib
        hardware_gpio
        hardware_i2c
        hardware_irq)