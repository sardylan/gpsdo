add_library(pid
        pid.h pid.c
        context.h context.c)

target_link_libraries(pid
        pico_stdlib)

target_include_directories(pid
        PUBLIC ${CMAKE_CURRENT_LIST_DIR})
