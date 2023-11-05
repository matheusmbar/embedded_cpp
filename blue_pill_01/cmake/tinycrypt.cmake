## Add Tinycrypt to project
include(FetchContent)

# tinycrypt provides a config.mk file that sets build variables and must be edited to fit
# the project that includes it. This file does not allow overriding the default values through
# ENV values so a patch is applied to adjust this and support customizations on command
#
# Information about PATCH_COMMAND: https://stackoverflow.com/a/73725257
set(tinycrypt_patch git reset --hard && git apply ${CMAKE_SOURCE_DIR}/cmake/patches/tinycrypt_config.patch)

FetchContent_Declare(
  tinycrypt
  GIT_REPOSITORY https://github.com/intel/tinycrypt.git
  GIT_TAG        v0.2.8
  GIT_SHALLOW    TRUE
  GIT_PROGRESS   TRUE
  PATCH_COMMAND  ${tinycrypt_patch}
)

FetchContent_MakeAvailable(tinycrypt)
FetchContent_GetProperties(tinycrypt)

# create a target to build tinycrypt
add_custom_target(
    tinycrypt
    COMMAND CC=${CMAKE_C_COMPILER} CFLAGS_EXT=${CMAKE_C_FLAGS} AR=${CMAKE_AR} make -C lib
    WORKING_DIRECTORY ${tinycrypt_SOURCE_DIR}
    BYPRODUCTS ${tinycrypt_SOURCE_DIR}/lib/libtinycrypt.a
)

add_library(libtinycrypt STATIC IMPORTED)
target_include_directories(libtinycrypt INTERFACE ${tinycrypt_SOURCE_DIR}/lib/include)
set_property(TARGET libtinycrypt PROPERTY IMPORTED_LOCATION ${tinycrypt_SOURCE_DIR}/lib/libtinycrypt.a)

add_dependencies(libtinycrypt tinycrypt)

target_include_directories(libtinycrypt INTERFACE "${tinycrypt_SOURCE_DIR}/lib/include")
