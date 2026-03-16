cmake_minimum_required(VERSION 3.30)

include(pico-sdk/pico_sdk_init.cmake)

project(gpsdo
        VERSION 1.90.0
        LANGUAGES C CXX ASM)

pico_sdk_init()

set(APPLICATION_NAME "GPS Dominated Oscillator for RaspberryPi Pico (RP2040)")
set(APPLICATION_VERSION_MAJOR "1")
set(APPLICATION_VERSION_MINOR "90")
set(APPLICATION_VERSION_PATCH "0")

set(ORGANIZATION_NAME "The HellNet.org")
set(ORGANIZATION_DOMAIN "thehellnet.org")

include(cmake/tags.cmake)

include(cmake/flags.cmake)
include(cmake/config.cmake)
include(cmake/log.cmake)

add_subdirectory(src)
