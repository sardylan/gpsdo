add_library(pid
        pid.hpp pid.tpp)

target_link_libraries(pid
        pico_stdlib)

target_include_directories(pid
        PUBLIC ${CMAKE_CURRENT_LIST_DIR})
