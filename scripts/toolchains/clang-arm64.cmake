set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# No extra packages should need to be installed
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

set(CMAKE_C_COMPILER_TARGET aarch64-linux-gnu)
set(CMAKE_CXX_COMPILER_TARGET aarch64-linux-gnu)

set(CMAKE_C_COMPILER_EXTERNAL_TOOLCHAIN /usr)
set(CMAKE_CXX_COMPILER_EXTERNAL_TOOLCHAIN /usr)

# Requires qemu-user-static on debian
set(CMAKE_CROSSCOMPILING_EMULATOR
        "qemu-aarch64-static;-L;/usr/aarch64-linux-gnu"
        CACHE STRING "ARM64 QEMU emulator"
)
