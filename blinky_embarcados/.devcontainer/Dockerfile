FROM mcr.microsoft.com/devcontainers/cpp:0-ubuntu-22.04

# Install additional packages.
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
    gcc-arm-none-eabi \
    libnewlib-arm-none-eabi \
    libstdc++-arm-none-eabi-newlib \
    gdb-multiarch

RUN usermod -aG dialout vscode
RUN usermod -aG plugdev vscode
