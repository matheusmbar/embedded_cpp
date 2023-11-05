## Add mcuboot to project
include(FetchContent)

# Set to not download submodules if that option is available
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.16.0")
    cmake_policy(SET CMP0097 NEW)
endif()

FetchContent_Declare(
  bootutil
  GIT_REPOSITORY https://github.com/mcu-tools/mcuboot.git
  GIT_TAG        v1.10.0
  GIT_SHALLOW    TRUE
  GIT_PROGRESS   TRUE
  GIT_SUBMODULES ""
  SOURCE_SUBDIR  boot/bootutil
)
FetchContent_GetProperties(bootutil)
FetchContent_MakeAvailable(bootutil)

target_link_libraries(bootutil libtinycrypt)
