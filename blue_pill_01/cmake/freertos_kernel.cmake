## Add FreeRTOS to project
include(FetchContent)

FetchContent_Declare( freertos_kernel
  GIT_REPOSITORY https://github.com/FreeRTOS/FreeRTOS-Kernel.git
  GIT_TAG        V11.0.1
  GIT_SHALLOW    TRUE
  GIT_PROGRESS   TRUE
  SOURCE_DIR     "${CMAKE_SOURCE_DIR}/thirdy-party/freertos_kernel"
)

add_library(freertos_config INTERFACE)

# Set path to folder containing FreeRTOSConfig.h
target_include_directories(
  freertos_config
  INTERFACE
  "${CMAKE_SOURCE_DIR}/lib/include"
)

# Select memory allocation implementation
set( FREERTOS_HEAP "4" CACHE STRING "" FORCE)
# Select the native compile PORT
set( FREERTOS_PORT "GCC_POSIX" CACHE STRING "" FORCE)
# Select the cross-compile PORT
if (CMAKE_CROSSCOMPILING)
    set(FREERTOS_PORT "GCC_ARM_CM3" CACHE STRING "" FORCE)
endif()

FetchContent_MakeAvailable( freertos_kernel )
