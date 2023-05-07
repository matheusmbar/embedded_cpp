

## Build instructions

The default build time is "DEBUG", used if none is specified.

**Debug config**
```sh
cd blue_pill_01
cmake -B build_debug .
cd build_debug
cmake --build .
```

**Release config**
```sh
cd blue_pill_01
cmake -B build . -DCMAKE_BUILD_TYPE=release
cd build_release
cmake --build .
```
