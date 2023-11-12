## Add FreeRTOS to project
include(FetchContent)

FetchContent_Declare( freertos_kernel
  GIT_REPOSITORY https://github.com/FreeRTOS/FreeRTOS-Kernel.git
  GIT_TAG        V10.5.1
  GIT_SHALLOW TRUE
  GIT_PROGRESS TRUE
)

# Set path to folder containing FreeRTOSConfig.h
set( FREERTOS_CONFIG_FILE_DIRECTORY
    "${CMAKE_SOURCE_DIR}/lib/include" CACHE STRING "" FORCE)
# Select memory allocation implementation
set( FREERTOS_HEAP "4" CACHE STRING "" FORCE)
# Select the native compile PORT
set( FREERTOS_PORT "GCC_POSIX" CACHE STRING "" FORCE)
# Select the cross-compile PORT
if (CMAKE_CROSSCOMPILING)
    set(FREERTOS_PORT "GCC_ARM_CM3" CACHE STRING "" FORCE)
endif()

FetchContent_MakeAvailable( freertos_kernel )
