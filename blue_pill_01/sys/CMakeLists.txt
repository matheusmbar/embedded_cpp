cmake_minimum_required(VERSION 3.22)

add_library(Sys "")

target_sources(Sys
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/startup.c
    ${CMAKE_CURRENT_LIST_DIR}/syscalls.c
    ${CMAKE_CURRENT_LIST_DIR}/sysmem.c
  )

target_include_directories(Sys
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../app/include
  )
