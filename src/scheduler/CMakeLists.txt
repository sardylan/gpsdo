add_library(scheduler
        scheduler.c
        scheduler.h
        context.h
        types.h)

target_link_libraries(scheduler
        pico_stdlib)

target_include_directories(scheduler
        PUBLIC ${CMAKE_CURRENT_LIST_DIR})
