cmake_minimum_required(VERSION 3.22)

add_library(project_lib "")

target_sources(project_lib
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/mymath.cpp
  )

target_include_directories(project_lib
  INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/include
  )

add_dependencies(
  project_lib
  freertos_kernel
  etl::etl
  )

target_link_libraries(project_lib PUBLIC etl::etl freertos_kernel)
