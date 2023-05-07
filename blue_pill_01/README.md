
## Setup

1. Open `blue_pill_01` as Workspace folder on VS Code
2. Install recommended extensions
## Build instructions

The default build time is "DEBUG", used if none is specified.

**Debug config**
```sh
cd blue_pill_01
cmake -B build .
cd build
cmake --build .
```

**Release config**
```sh
cd blue_pill_01
cmake -B build . -DCMAKE_BUILD_TYPE=release
cd build_release
cmake --build .
```

## Debugging

A few configurations for VS Code are provided to make debugging easier.

The `Cortex Debug` extension allows connecting to a target through a debugger. The default setup provided is set to use a Black Magic Probe at port `/dev/ttyACM0`, adjust at `launch.json` if needed.

A correctly adjusted setup supports:
- start a debug session using the `Start Debugging (F5)` function
- set breakpoints, watch variables and all default GDB features
- watch Cortex peripherals and registers on debug view
