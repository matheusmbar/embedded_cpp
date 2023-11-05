## Add libopencm3 to project
# reference: https://github.com/plusk01/stm32-libopencm3-cmake-blink/blob/main/cmake/stm32f765.cmake

include(FetchContent)

FetchContent_Declare(libopencm3
  GIT_REPOSITORY https://github.com/libopencm3/libopencm3
  GIT_TAG master
)
FetchContent_MakeAvailable(libopencm3)
FetchContent_GetProperties(libopencm3)

# create a target to build libopencm3 -- only for the target we need
add_custom_target(libopencm3 make TARGETS=stm32/f1
    WORKING_DIRECTORY ${libopencm3_SOURCE_DIR}
    BYPRODUCTS ${libopencm3_SOURCE_DIR}/lib/libopencm3_stm32f1.a
)

add_library(libopencm3_stm32f103 STATIC IMPORTED)
set_property(TARGET libopencm3_stm32f103 PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${libopencm3_SOURCE_DIR}/include)
set_property(TARGET libopencm3_stm32f103 PROPERTY IMPORTED_LOCATION ${libopencm3_SOURCE_DIR}/lib/libopencm3_stm32f1.a)

add_dependencies(libopencm3_stm32f103 libopencm3)

target_link_directories(libopencm3_stm32f103 INTERFACE ${libopencm3_SOURCE_DIR}/lib)
target_compile_definitions(libopencm3_stm32f103 INTERFACE -DSTM32F1)
