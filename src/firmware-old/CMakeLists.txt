include_directories(${CMAKE_CURRENT_BINARY_DIR})

configure_file(version.hpp.in version.hpp)

add_executable(gpsdo
        config.hpp
        datetime.hpp datetime.cpp
        gps.hpp gps.cpp
        led.hpp led.cpp
        logger.hpp logger.tpp logger.cpp
        main.hpp main.cpp
        oscillator.hpp oscillator.cpp
        si5351.hpp si5351.cpp
        ui.hpp ui.cpp)

target_link_libraries(gpsdo
        pico_stdlib
        pico_multicore
        hardware_uart
        hardware_rtc
        hardware_pwm
        hardware_irq
        hardware_i2c)

pico_enable_stdio_usb(gpsdo 1)

pico_add_extra_outputs(gpsdo)
