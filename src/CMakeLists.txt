include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

configure_file(buildprop.hpp.in buildprop.hpp)

add_subdirectory(display)
add_subdirectory(gps)
add_subdirectory(logger)
add_subdirectory(pid)
add_subdirectory(scheduler)
add_subdirectory(si5351)

add_subdirectory(firmware)
