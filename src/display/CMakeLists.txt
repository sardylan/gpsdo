add_library(display
        display.hpp display.cpp)

target_link_libraries(display
        pico_stdlib
        hardware_gpio)

target_include_directories(display
        PUBLIC ${CMAKE_CURRENT_LIST_DIR})
