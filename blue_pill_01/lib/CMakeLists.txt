cmake_minimum_required(VERSION 3.22)

set(LIB_NAME "${PROJECT_NAME}-lib")

add_library(${LIB_NAME} STATIC)

target_sources(
  ${LIB_NAME}
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/snake.cpp
  )

target_include_directories(
  ${LIB_NAME}
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${PROJECT_SOURCE_DIR}/app/include
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  )

add_dependencies(
  ${LIB_NAME}
  myopenstm32
  etl::etl
  )

target_link_libraries(
  ${LIB_NAME}
  PUBLIC
  myopenstm32
  etl::etl
  )
