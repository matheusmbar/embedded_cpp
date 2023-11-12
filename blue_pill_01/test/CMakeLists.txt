cmake_minimum_required(VERSION 3.22)

include(${PROJECT_SOURCE_DIR}/cmake/gtest.cmake)

include(GoogleTest)

set(TEST_NAME ${PROJECT_NAME}-test)

set(CMAKE_C_FLAGS_DEBUG      "-Og -g -ggdb3 -DDEBUG" CACHE INTERNAL "")
set(CMAKE_CXX_FLAGS_DEBUG    "${CMAKE_C_FLAGS_DEBUG}" CACHE INTERNAL "")

set(CMAKE_C_FLAGS_RELEASE    "-Os" CACHE INTERNAL "")
set(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_C_FLAGS_RELEASE}" CACHE INTERNAL "")

option(CMAKE_BUILD_TYPE "Choose the type of build." "DEBUG")

add_executable(${TEST_NAME}
  "${CMAKE_CURRENT_SOURCE_DIR}/src/test1.cpp"
  )

target_include_directories(${TEST_NAME}
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include
  )

target_link_libraries(${TEST_NAME}
  GTest::gmock
  GTest::gmock_main
  project_lib
  )

gtest_discover_tests(${TEST_NAME}
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  DISCOVERY_MODE PRE_TEST
  )
