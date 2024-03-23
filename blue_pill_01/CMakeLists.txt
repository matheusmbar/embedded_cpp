cmake_minimum_required(VERSION 3.22)

## Set C and C++ standards
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(DEVICE "STM32F103C8TX")

if (NOT DEFINED CMAKE_TOOLCHAIN_FILE AND NOT DEFINED BUILD_LIB_TESTS)
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/cmake/toolchain-arm-none-eabi.cmake")
endif()

project(blue_pill_01 LANGUAGES C CXX)

## Export build parameters from CMake to use on VS Code c_cpp_properties.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON )

## Uncomment to increase FetchContent verbose
# set(FETCHCONTENT_QUIET FALSE)

## Add libopencm3 to project
include(cmake/libopencm3.cmake)

## Add ETL CPP to project
include(cmake/etlcpp.cmake)

## Add FreeRTOS to project
include(cmake/freertos_kernel.cmake)

## Add embedded-cli to project
include(cmake/embedded-cli.cmake)

## Add U8G2 to project
include(cmake/u8g2.cmake)

option(BUILD_LIB_TESTS "Build tests" FALSE)

add_subdirectory(lib)

## Execute final steps

if(NOT BUILD_LIB_TESTS)
  add_subdirectory(app)
else()
  enable_testing()
  add_subdirectory(test)
endif()
