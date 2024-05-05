cmake_minimum_required(VERSION 3.22)

set(APP_NAME ${PROJECT_NAME}-app)

add_executable(${APP_NAME} "")

target_sources(${APP_NAME}
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/peripherals/clock.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/peripherals/usart.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sys/io.c"
  )

target_include_directories(${APP_NAME}
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
  )

add_dependencies(${APP_NAME}
  ${PROJECT_NAME}-lib
  myopenstm32
  libopencm3
  libopencm3_ld
  freertos_kernel
  etl::etl
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
  ${PROJECT_NAME}-lib
  myopenstm32
  libopencm3
  libopencm3_ld
  freertos_kernel
  etl::etl
  )

print_section_sizes(${APP_NAME})
create_bin_output(${APP_NAME})
