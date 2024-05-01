## Add Lib-stm32 to project
include(FetchContent)

FetchContent_Declare(
  lib-stm32
  GIT_REPOSITORY git@github.com:matheusmbar/lib-stm32
  GIT_TAG        main
  GIT_SHALLOW    FALSE
  GIT_PROGRESS   TRUE
  SOURCE_DIR     "${CMAKE_SOURCE_DIR}/thirdy-party/lib-stm32"
)

FetchContent_MakeAvailable(lib-stm32)
