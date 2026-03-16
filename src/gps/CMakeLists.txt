add_library(gps
        data.hpp data.cpp
        enums.hpp
        gps.hpp gps.cpp)

target_link_libraries(gps
        pico_stdlib
        hardware_gpio
        hardware_uart)

target_include_directories(gps
        PUBLIC ${CMAKE_CURRENT_LIST_DIR})
