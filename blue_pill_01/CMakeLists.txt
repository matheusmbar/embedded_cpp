
cmake_minimum_required(VERSION 3.22)
project(blue_pill_01 LANGUAGES C CXX)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "src/*.c"
    "src/*.cpp")

include_directories(include)

set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/STM32F103C8TX.ld")

set(CMAKE_EXECUTABLE_SUFFIX ".elf")

add_executable(${PROJECT_NAME} ${SOURCES})

print_section_sizes(${PROJECT_NAME})
create_bin_output(${PROJECT_NAME})

# Linker flags
set(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} -T${LINKER_SCRIPT}"
    CACHE INTERNAL "Linker options")
