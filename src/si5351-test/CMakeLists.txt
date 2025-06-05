include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(si5351-test
        main.c
        si5351.h si5351.c)

target_link_libraries(si5351-test
        pico_stdlib
        hardware_gpio
        hardware_uart
        hardware_i2c)

pico_add_extra_outputs(si5351-test)
