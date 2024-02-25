cmake_minimum_required(VERSION 3.22)

if (NOT DEFINED DEVICE)
    message( FATAL_ERROR "It is required to set a DEVICE.")
endif()

set(LDSCRIPT "${CMAKE_BINARY_DIR}/${DEVICE}.ld" )

add_custom_target(
    ldscript
    make
    DEVICE=${DEVICE} CPP=${CMAKE_C_COMPILER} LDSCRIPT=${LDSCRIPT} OPENCM3_DIR=${libopencm3_SOURCE_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    BYPRODUCTS ${LDSCRIPT}
)

add_dependencies(
  ldscript
  libopencm3
)
