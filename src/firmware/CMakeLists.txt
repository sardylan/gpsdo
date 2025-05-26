include_directories(${CMAKE_CURRENT_BINARY_DIR})

configure_file(version.h.in version.h)

add_executable(gpsdo
        config.h
        gps.h gps.c
        led.h led.c
        log.h log.c
        main.h main.c
        oscillator.h oscillator.c
        si5351.h si5351.c)

target_link_libraries(gpsdo
        pico_stdlib
        pico_multicore
        hardware_uart
        hardware_rtc
        hardware_pwm
        hardware_irq
        hardware_i2c)

pico_add_extra_outputs(gpsdo)
