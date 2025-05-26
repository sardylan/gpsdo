cmake_minimum_required(VERSION 3.30)

include(pico-sdk/pico_sdk_init.cmake)

project(gpsdo)

pico_sdk_init()

set(APPLICATION_NAME "GPS Dominated Oscillator for RaspberryPi Pico")
set(APPLICATION_VERSION_MAJOR "0")
set(APPLICATION_VERSION_MINOR "4")
set(APPLICATION_VERSION_PATCH "0")

set(ORGANIZATION_NAME "The HellNet.org")
set(ORGANIZATION_DOMAIN "thehellnet.org")

set(APPLICATION_VERSION "${APPLICATION_VERSION_MAJOR}.${APPLICATION_VERSION_MINOR}.${APPLICATION_VERSION_PATCH}")

include(cmake/flags.cmake)
include(cmake/tags.cmake)
include(cmake/log.cmake)

add_subdirectory(src)
