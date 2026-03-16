add_library(scheduler
        scheduler.hpp scheduler.cpp
        task.hpp task.cpp)

target_link_libraries(scheduler
        pico_stdlib)

target_include_directories(scheduler
        PUBLIC ${CMAKE_CURRENT_LIST_DIR})
