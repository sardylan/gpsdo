include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

configure_file(buildprop.h.in buildprop.h)

add_subdirectory(si5351-test)
add_subdirectory(si5351)
add_subdirectory(lcd)
add_subdirectory(firmware)
