## Add embedded-cli to project
include(FetchContent)

FetchContent_Declare(
  embedded_cli
  GIT_REPOSITORY https://github.com/funbiscuit/embedded-cli
  GIT_TAG        v0.1.3
  GIT_SHALLOW    TRUE
  GIT_PROGRESS   TRUE
  SOURCE_DIR     "${CMAKE_SOURCE_DIR}/thirdy-party/embedded_cli"
)

FetchContent_MakeAvailable(embedded_cli)
