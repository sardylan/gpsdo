add_library(si5351
        enums.hpp
        structs.hpp
        const.hpp
        device.hpp device.cpp
        picosdk.hpp picosdk.cpp
        si5351.hpp)

target_link_libraries(si5351
        pico_stdlib
        hardware_i2c
        hardware_gpio)
