# Target definition
set(CMAKE_SYSTEM_NAME  Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

# Select a custom path for ARM toolchain if it is not installed system wide
# otherwise default system paths will be used
# This variable must be a path to the folder that contains 'bin' and 'lib' folders
# Add next line to CMakeLists.txt
# set(TOOLCHAIN_PATH /path/to/arm-toolchain/gcc-arm-none-eabi)

set(TOOLCHAIN arm-none-eabi)

if (TOOLCHAIN_PATH)
    set(TOOLCHAIN_BIN_DIR ${TOOLCHAIN_PATH}/bin)
    set(TOOLCHAIN_INC_DIR ${TOOLCHAIN_PATH}/${TOOLCHAIN}/include)
    set(TOOLCHAIN_LIB_DIR ${TOOLCHAIN_PATH}/${TOOLCHAIN}/lib)
    set(CMAKE_FIND_ROOT_PATH ${TOOLCHAIN_PATH}/${${TOOLCHAIN}} ${CMAKE_PREFIX_PATH})
else ()
    set(TOOLCHAIN_BIN_DIR /usr/bin)
    set(TOOLCHAIN_INC_DIR /lib/arm-none-eabi/include)
    set(TOOLCHAIN_LIB_DIR /lib/arm-none-eabi/lib)
    set(CMAKE_FIND_ROOT_PATH /lib/${${TOOLCHAIN}} ${CMAKE_PREFIX_PATH})
endif()


# Perform compiler test with static library
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'DEBUG' as none was specified.")
  set(CMAKE_BUILD_TYPE "DEBUG" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

#---------------------------------------------------------------------------------------
# Set compiler/linker flags
#---------------------------------------------------------------------------------------

# Object build options
# -O0                   No optimizations, reduce compilation time and make debugging produce the expected results.
# -mthumb               Generat thumb instructions.
# -fno-builtin          Do not use built-in functions provided by GCC.
# -Wall                 Print only standard warnings, for all use Wextra
# -ffunction-sections   Place each function item into its own section in the output file.
# -fdata-sections       Place each data item into its own section in the output file.
# -fomit-frame-pointer  Omit the frame pointer in functions that don’t need one.
# -mabi=aapcs           Defines enums to be a variable sized type.
#set(OBJECT_GEN_FLAGS "-O0 -mthumb -fno-builtin -Wall -ffunction-sections -fdata-sections -fomit-frame-pointer -mabi=aapcs")

set(CPU_CORE_FLAGS "-mcpu=cortex-m3 -mthumb -msoft-float ")

set(OBJECT_GEN_FLAGS "${CPU_CORE_FLAGS} -ffunction-sections -fdata-sections --specs=nosys.specs --specs=nano.specs")

#  -std=gnu99
set(CMAKE_C_FLAGS   "${OBJECT_GEN_FLAGS} -std=gnu99" CACHE INTERNAL "C Compiler options")

# -std=c++11
set(CMAKE_CXX_FLAGS "${OBJECT_GEN_FLAGS} -fno-exceptions -std=c++11" CACHE INTERNAL "C++ Compiler options")

# set Debug and Release compile options
set(CMAKE_C_FLAGS_DEBUG      "-Og -g -ggdb3 -DDEBUG" CACHE INTERNAL "")
set(CMAKE_C_FLAGS_RELEASE    "-Os" CACHE INTERNAL "")
set(CMAKE_CXX_FLAGS_DEBUG    "${CMAKE_C_FLAGS_DEBUG}" CACHE INTERNAL "")
set(CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_C_FLAGS_RELEASE}" CACHE INTERNAL "")

set(CMAKE_EXECUTABLE_SUFFIX_C   .elf)
set(CMAKE_EXECUTABLE_SUFFIX_CXX .elf)
set(CMAKE_EXECUTABLE_SUFFIX_ASM .elf)

# -Wl,--gc-sections     Perform the dead code elimination.
# -Map                  Generate linker map file
set(CMAKE_EXE_LINKER_FLAGS "-Wl,--gc-sections ${CPU_CORE_FLAGS} -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}" CACHE INTERNAL "Linker options")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}" CACHE INTERNAL "Linker options")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)


#---------------------------------------------------------------------------------------
# Set tools
#---------------------------------------------------------------------------------------
set(CMAKE_C_COMPILER ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-gcc CACHE INTERNAL "C Compiler")
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-g++ CACHE INTERNAL "C++ Compiler")
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-gcc CACHE INTERNAL "ASM Compiler")
set(CMAKE_SIZE ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-size)
set(CMAKE_OBJCOPY ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-objcopy)

#---------------------------------------------------------------------------------------
# Prints the section sizes
#---------------------------------------------------------------------------------------
function(print_section_sizes TARGET)
    add_custom_command(TARGET ${TARGET} POST_BUILD
                       COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${TARGET}>)
endfunction()

#---------------------------------------------------------------------------------------
# Creates output in binary format
#---------------------------------------------------------------------------------------
function(create_bin_output TARGET)
    add_custom_target("${TARGET}_binary" ALL DEPENDS ${TARGET}
                      COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${TARGET}> $<TARGET_FILE_BASE_NAME:${TARGET}>.bin)

    set_property(
        TARGET ${TARGET}
        APPEND
        PROPERTY ADDITIONAL_CLEAN_FILES "$<TARGET_FILE_BASE_NAME:${TARGET}>.bin;$<TARGET_FILE:${TARGET}>.map"
    )
endfunction()
