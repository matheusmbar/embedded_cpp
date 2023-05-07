cmake_minimum_required(VERSION 3.22)

if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/cmake/toolchain-arm-none-eabi.cmake")
endif()

project(blue_pill_01 LANGUAGES C CXX)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "src/*.c"
    "src/*.cpp"
)

include_directories(include)

set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/cmake/STM32F103C8TX.ld")

## Add linker script to flags
set(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} -T${LINKER_SCRIPT}"
    CACHE INTERNAL "Linker options")

add_executable(${PROJECT_NAME} ${SOURCES})

## Create debug setup
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "_debug")

## Execute final steps
print_section_sizes(${PROJECT_NAME})
create_bin_output(${PROJECT_NAME})