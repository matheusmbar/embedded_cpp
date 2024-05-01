## Add U8G2 to project
include(FetchContent)

FetchContent_Declare(
  u8g2
  GIT_REPOSITORY https://github.com/olikraus/u8g2
  GIT_TAG        master
  GIT_SHALLOW    TRUE
  GIT_PROGRESS   TRUE
  SOURCE_DIR     "${CMAKE_SOURCE_DIR}/thirdy-party/u8g2"
)

FetchContent_MakeAvailable(u8g2)
