add_library(si5351
        si5351.h si5351.c)

target_link_libraries(si5351
        pico_stdlib
        hardware_gpio
        hardware_i2c
        hardware_irq)
