add_library(logger
        logger.hpp logger.cpp)

target_link_libraries(logger
        pico_stdlib)

target_include_directories(logger
        INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
