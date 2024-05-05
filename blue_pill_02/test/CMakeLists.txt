cmake_minimum_required(VERSION 3.22)

include(${PROJECT_SOURCE_DIR}/../cmake/gtest.cmake)

include(GoogleTest)

set(TEST_NAME ${PROJECT_NAME}-test)

option(CMAKE_BUILD_TYPE "Choose the type of build." "DEBUG")

add_executable(${TEST_NAME}
    "${CMAKE_CURRENT_LIST_DIR}/src/test_dummy.cpp"
  )

target_include_directories(${TEST_NAME}
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/include
  )

add_dependencies(
  ${TEST_NAME}
  ${PROJECT_NAME}-lib
  )

target_link_libraries(${TEST_NAME}
  GTest::gmock
  GTest::gmock_main
  ${PROJECT_NAME}-lib
  )

gtest_discover_tests(${TEST_NAME}
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  DISCOVERY_MODE PRE_TEST
  )
