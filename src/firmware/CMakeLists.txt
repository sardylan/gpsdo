include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(firmware
        counter.hpp counter.cpp
        firmware.hpp firmware.cpp
        init.hpp init.cpp
        interrupts.hpp interrupts.cpp
        led.hpp led.cpp
        main.hpp main.cpp
        timertc.hpp timertc.cpp)

target_link_libraries(firmware
        pico_stdlib
        pico_multicore
        hardware_sync
        hardware_gpio
        hardware_uart
        hardware_rtc
        hardware_pwm
        hardware_irq
        hardware_i2c
        hardware_pio
        hardware_dma
        display
        gps
        logger
        pid
        scheduler
        si5351)

pico_add_extra_outputs(firmware)
