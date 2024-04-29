cmake_minimum_required(VERSION 3.22)

# file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
#     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
#     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
# )

set(APP_NAME ${PROJECT_NAME}-app)

add_executable(${APP_NAME} "")

target_sources(${APP_NAME}
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/devices/lcd/ssd1306.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/snake.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/snake_ui.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/snake_game.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/peripherals/clock.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/peripherals/i2c.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/peripherals/usart.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/peripherals/timer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sys/checks/test_cpp.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sys/checks/test_c.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/sys/io.c"
  )

target_include_directories(${APP_NAME}
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
  )

add_dependencies(${APP_NAME}
  myopenstm32
  libopencm3
  libopencm3_ld
  freertos_kernel
  etl::etl
  EmbeddedCLI::EmbeddedCLI
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
  myopenstm32
  libopencm3
  libopencm3_ld
  freertos_kernel
  etl::etl
  EmbeddedCLI::EmbeddedCLI
  u8g2
  )

print_section_sizes(${APP_NAME})
create_bin_output(${APP_NAME})
