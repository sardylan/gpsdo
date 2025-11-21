add_library(lcd
        comm.h comm.c
        context.h context.c
        text.h text.c)

target_link_libraries(lcd
        pico_stdlib
        hardware_gpio)

target_include_directories(lcd
        PUBLIC ${CMAKE_CURRENT_LIST_DIR})
