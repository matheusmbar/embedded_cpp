cmake_minimum_required(VERSION 3.22)

# file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
#     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
#     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
# )

set(APP_NAME ${PROJECT_NAME}-app)

add_executable(${APP_NAME} "")

target_sources(${APP_NAME}
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/test_cpp.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/test_c.c"
  )

target_include_directories(${APP_NAME}
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
  )

add_dependencies(${APP_NAME}
  Sys
  libopencm3_stm32f103
  freertos_kernel
  project_lib
  )

## Create debug setup
set_target_properties(${APP_NAME}
  PROPERTIES
  DEBUG_POSTFIX
  "-DEBUG"
  )

## Link application and libraries
target_link_libraries(${APP_NAME}
  PUBLIC
  Sys
  libopencm3_stm32f103
  freertos_kernel
  etl::etl
  project_lib
  )

print_section_sizes(${APP_NAME})
create_bin_output(${APP_NAME})
