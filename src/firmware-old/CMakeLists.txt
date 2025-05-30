include_directories(${CMAKE_CURRENT_BINARY_DIR})

configure_file(version.h.in version.h)
configure_file(buildflags.h.in buildflags.h)

add_executable(gpsdo
        buildflags.h.in
        config.h
        counter.h counter.c
        gps.h gps.c
        led.h led.c
        log.h log.c
        main.h main.c
        oscillator.h oscillator.c
        version.h.in)

target_link_libraries(gpsdo
        pico_stdlib
        pico_multicore
        hardware_gpio
        hardware_uart
        hardware_rtc
        hardware_pwm
        hardware_irq
        hardware_i2c
        si5351)

pico_add_extra_outputs(gpsdo)
