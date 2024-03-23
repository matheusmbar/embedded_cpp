cmake_minimum_required(VERSION 3.22)

add_library(project_lib "")

target_sources(project_lib
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/mymath.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/devices/gpio/gpio_opencm3.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/devices/led/led_gpio.cpp
  )

target_include_directories(project_lib
  INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/include
  )

add_dependencies(
  project_lib
  freertos_kernel
  etl::etl
  libopencm3
  )

target_link_libraries(project_lib PUBLIC
                      etl::etl
                      freertos_kernel
                      libopencm3
                      )
