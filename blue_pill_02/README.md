# Blue Pill 02

## Description

This is a basic project setup for STM32 Blue Pill boards, containing:
- FreeRTOS kernel
- Embedded Template Library (ETL)
- libopencm3
- lib-stm32
- unit test infrastructure trough GTest/GMock

## Build instructions

The default build type is "DEBUG", used if none is specified. It cross compiles using an ARM toolchain.

**Debug config**
```sh
$ cd blue_pill_01
$ cmake -B build .
$ cd build
$ cmake --build .
```

**Release config**
```sh
$ cd blue_pill_01
$ cmake -B build . -DCMAKE_BUILD_TYPE=release
$ cd build_release
$ cmake --build .
```

**Build tests**
```sh
$ cd blue_pill_01
$ cmake -B build_test . -DBUILD_LIB_TESTS=true -G Ninja
$ cd build_test
$ cmake --build .

# Run tests
$ ctest --output-on-failure
```
