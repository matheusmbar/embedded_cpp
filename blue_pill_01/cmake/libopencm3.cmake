## Add libopencm3 to project
# reference: https://github.com/plusk01/stm32-libopencm3-cmake-blink/blob/main/cmake/stm32f765.cmake

include(FetchContent)

FetchContent_Declare(opencm3
  GIT_REPOSITORY https://github.com/libopencm3/libopencm3
  GIT_TAG        master
  GIT_SHALLOW    TRUE
  GIT_PROGRESS   TRUE
  SOURCE_DIR     "${CMAKE_SOURCE_DIR}/thirdy-party/libopencm3"
)
FetchContent_MakeAvailable(opencm3)
FetchContent_GetProperties(opencm3)

set(LDSCRIPT "${CMAKE_BINARY_DIR}/${DEVICE}.ld" )
set(PARAMS DEVICE=${DEVICE} CPP=${CMAKE_C_COMPILER} LDSCRIPT=${LDSCRIPT} OPENCM3_DIR=${opencm3_SOURCE_DIR})

execute_process(
  COMMAND               make -s ${PARAMS} target
  WORKING_DIRECTORY     ${CMAKE_CURRENT_LIST_DIR}/opencm3
  OUTPUT_VARIABLE       target
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

add_custom_target(
  opencm3_ld
  make -s ${PARAMS}
  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/opencm3
  BYPRODUCTS        ${LDSCRIPT}
)

execute_process(
  COMMAND               make -s ${PARAMS} libfile
  WORKING_DIRECTORY     ${CMAKE_CURRENT_LIST_DIR}/opencm3
  OUTPUT_VARIABLE       LIBFILE
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
  COMMAND               make -s ${PARAMS} arch_flags
  WORKING_DIRECTORY     ${CMAKE_CURRENT_LIST_DIR}/opencm3
  OUTPUT_VARIABLE       ARCH_FLAGS
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
  COMMAND               make -s ${PARAMS} target_flags
  WORKING_DIRECTORY     ${CMAKE_CURRENT_LIST_DIR}/opencm3
  OUTPUT_VARIABLE       OPENCM3_TARGET_FLAGS
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
  COMMAND               make -s ${PARAMS} all_vars
  WORKING_DIRECTORY     ${CMAKE_CURRENT_LIST_DIR}/opencm3
  OUTPUT_VARIABLE       OPENCM3_ALL_VARS
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

add_library(libopencm3_ld OBJECT IMPORTED)
add_dependencies(libopencm3_ld opencm3_ld)
target_link_options(libopencm3_ld INTERFACE "-T${LDSCRIPT}")

# create a target to build libopencm3 -- only for the target we need (e.g. stm32/f1)
add_custom_target(
    opencm3
    make TARGETS=${target}
    WORKING_DIRECTORY ${opencm3_SOURCE_DIR}
    BYPRODUCTS ${LIBFILE}
)

add_library(libopencm3 STATIC IMPORTED)
set_property(TARGET libopencm3 PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${opencm3_SOURCE_DIR}/include)
set_property(TARGET libopencm3 PROPERTY IMPORTED_LOCATION ${LIBFILE})

add_dependencies(libopencm3 opencm3)

target_link_directories(libopencm3 INTERFACE ${opencm3_SOURCE_DIR}/lib)
target_compile_definitions(libopencm3 INTERFACE ${OPENCM3_TARGET_FLAGS})

set_property(TARGET libopencm3 PROPERTY ARCH_FLAGS    ${ARCH_FLAGS})
set_property(TARGET libopencm3 PROPERTY OPENCM3_TARGET_FLAGS ${OPENCM3_TARGET_FLAGS})


message(STATUS "OPENCM3_TARGET_FLAGS: ${OPENCM3_TARGET_FLAGS}")

message(VERBOSE "ARCH_FLAGS: ${ARCH_FLAGS}")
message(VERBOSE "LIBFILE: ${LIBFILE}")
message(VERBOSE "OPENCM3_ALL_VARS\n${OPENCM3_ALL_VARS}")

if(NOT BUILD_LIB_TESTS)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ARCH_FLAGS}" CACHE INTERNAL "")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ARCH_FLAGS}" CACHE INTERNAL "")
  message(VERBOSE "CMAKE_C_FLAGS:   ${CMAKE_C_FLAGS}")
  message(VERBOSE "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
else()
  message(WARNING "BUILDING TESTS")
endif()
