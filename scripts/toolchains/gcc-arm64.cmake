set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# Requires both gcc-aarch64-linux-gnu and g++-aarch64-linux-gnu on debian
set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)

# Requires qemu-user-static on debian
set(CMAKE_CROSSCOMPILING_EMULATOR
        "qemu-aarch64-static;-L;/usr/aarch64-linux-gnu"
        CACHE STRING "ARM64 QEMU emulator"
)
